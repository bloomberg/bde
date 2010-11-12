// btemt_message.t.cpp           -*-C++-*-

#include <btemt_message.h>

#include <bcec_queue.h>
#include <bcema_testallocator.h>                // for testing only
#include <bcema_pool.h>
//#include <bcema_defaultdeleter.h>
#include <bcema_blob.h>
#include <bcema_blobutil.h>
#include <bcema_pooledblobbufferfactory.h>
#include <bcema_pooledbufferchain.h>
#include <bcep_threadpool.h>
#include <bces_atomictypes.h>

#include <bdef_function.h>
#include <bdef_bind.h>

#include <bdesb_fixedmemoutput.h>
#include <bdex_genericbyteoutstream.h>

#include <bsls_platform.h>

#include <bsl_iostream.h>
#include <bsl_strstream.h>

#include <bsl_c_stdlib.h>     // atoi()

using namespace bsl;  // automatically added by script

using namespace BloombergLP;

//#############################################################################
//#*** TBD: This test driver is not sufficient - we must run               ***#
//#*** btemt_channelpool's test driver to validate this component.  This   ***#
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
void processMessages(bcec_Queue<btemt_Message> *queue,
                     bces_AtomicInt *count)
{
    ASSERT(queue);
    btemt_Message msg = queue->popFront();
    btemt_DataMsg& dataMsg = msg.dataMsg();
    ASSERT(dataMsg.userDataField1() < dataMsg.userDataField2());
    ++(*count);
}

static
void processBlobMessages(bcec_Queue<btemt_Message> *queue,
                         bces_AtomicInt *count)
{
    ASSERT(queue);
    btemt_Message msg = queue->popFront();
    btemt_BlobMsg& blobMsg = msg.blobMsg();
    ASSERT(blobMsg.userDataField1() < blobMsg.userDataField2());
    ++(*count);
}

                        // --------------------
                        // class DefaultDeleter
                        // --------------------

template <class TYPE>
class DefaultDeleter : public bcema_Deleter<TYPE> {
    bslma_Allocator *d_allocator_p;  // memory allocator (held, *not* owned)

  public:
    DefaultDeleter(bslma_Allocator *basicAllocator = 0)
    : d_allocator_p(bslma_Default::allocator(basicAllocator))
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
// It is possible to initialize a 'btemt_Message' object to a value of one of
// the specific-message types defined in this component (e.g., 'btemt_DataMsg',
// 'btemt_ChannelMsg').  (See the Usage Example of 'btemt_channelqueuepool' for
// additional examples.)  First, create a channel message object
// 'channelMessage' initialized with arbitrary data:
//..
    btemt_ChannelMsg::ChannelState state   =
                                            btemt_ChannelMsg::BTEMT_CHANNEL_UP;
    int                            chanId  = 123;
    int                            allocId = 456;

    btemt_ChannelMsg channelMessage(state, chanId, allocId);
//..
// We can now initialize a newly created object 'message' of type
// 'btemt_Message' with the value of 'channelMessage', and use 'message' to
// discover the attributes of 'channelMessage'.
//..
    btemt_Message message1 = channelMessage; // calls initializing constructor

    ASSERT(btemt_ChannelMsg::BTEMT_CHANNEL_UP
               == message1.channelMsg().channelState());
    ASSERT(123 == message1.channelMsg().channelId());
    ASSERT(456 == message1.channelMsg().allocatorId());
//..
//
// Another way is to use the 'btemt_Message' constructor taking a
// 'btemt_Message::MessageType' argument to construct a message representing
// the indicated type and then use that message, e.g., via a specific-message
// reference, to manipulate the "contained" specific message.  For example,
// using the previous definitions:
//..
    btemt_Message message2(btemt_Message::BTEMT_CHANNEL_STATE);
    btemt_ChannelMsg &channelMsg = message2.channelMsg();

    channelMsg.setChannelState(state);
    channelMsg.setChannelId(chanId);
    channelMsg.setAllocatorId(allocId);
//..
//
// Note that the channel messages created in either way should have the same
// type and value:
//..
    ASSERT(btemt_Message::BTEMT_CHANNEL_STATE == message1.type());
    ASSERT(btemt_Message::BTEMT_CHANNEL_STATE == message2.type());
    ASSERT(message1.channelMsg() == message2.channelMsg());
//..
// The second way presented above is slightly more error-prone in case the
// 'message2' is initialized with the wrong message type.  The user should make
// sure that specific-message references are always used in conjunction with a
// valid 'btemt_Message' of the same type.
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

    bcema_TestAllocator testAllocator(veryVeryVerbose);

    switch (test) { case 0:  // Zero is always the leading case.
      case 9: {
        // --------------------------------------------------------------------
        // TESTING btemt_MessageUtil::assignData
        // Testing:
        //   btemt_MessageUtil::assignData(bcema_Blob*, const btemt_DataMsg&,
        //                                 int)
        //   btemt_MessageUtil::assignData(btemt_DataMsg*, const bcema_Blob&,
        //                                 int,
        //                                 bcema_PooledBufferChainFactory*,
        //                                 bslma_Allocator*);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING btemt_MessageUtil::assignData"
                          << "\n=============" << endl;

        enum {
            NUM_SIZES = 7,
            DATA_SIZE = 81799
        };
        int bufSizes[NUM_SIZES] = {5, 17, 64, 239, 8192, 81799, 131072};

        for(int i = 0; i< NUM_SIZES; ++i) {
            int blobBufSize = bufSizes[i];
            bcema_PooledBlobBufferFactory blobFactory(blobBufSize);
            for(int j=0; j < NUM_SIZES; ++j) {
                int chainBufSize = bufSizes[j];
                if (veryVerbose) {
                    bsl::cout << "Testing blob buffer size = " << blobBufSize
                              << ", chain buffer size = " << chainBufSize
                              << bsl::endl;
                }
                bcema_PooledBufferChainFactory chainFactory(chainBufSize);

                char origData[DATA_SIZE];
                for(int p=0; p<DATA_SIZE; ++p) {
                    origData[p] = rand() & 0xFF;
                }

                bcema_Blob blob(&blobFactory);
                bcema_BlobUtil::append(&blob, origData, DATA_SIZE);
                ASSERT(blob.length() == DATA_SIZE);

                btemt_DataMsg dataMsg;
                btemt_MessageUtil::assignData(
                    &dataMsg, blob, DATA_SIZE, &chainFactory,
                    bslma_Default::allocator());
                ASSERT(dataMsg.data()->length() == blob.length());
                char dataCopy[DATA_SIZE];
                dataMsg.data()->copyOut(dataCopy, DATA_SIZE, 0);
                ASSERT(0 == memcmp(origData, dataCopy, DATA_SIZE));
                memset(dataCopy, 0, DATA_SIZE);

                blob.removeAll();
                btemt_MessageUtil::assignData(&blob, dataMsg, DATA_SIZE);
                ASSERT(blob.length() == DATA_SIZE);

                // TBD: don't we have a simple utility method somewhere?
                {
                bdesb_FixedMemOutput osb(dataCopy, DATA_SIZE);
                bdex_GenericByteOutStream<bdesb_FixedMemOutput> bos(&osb);
                bcema_BlobUtil::write(bos, blob, 0, DATA_SIZE);
                }
                ASSERT(0 == memcmp(origData, dataCopy, DATA_SIZE));
                memset(dataCopy, 0, DATA_SIZE);
            }
        }
      } break;
      case 8: {
        ;
        // --------------------------------------------------------------------
        // TESTING btemt_Message ON USER MESSAGES
        // Testing:
        //   btemt_Message(const btemt_UserMsg&)
        //   btemt_UserMsg& userMsg();
        //   const btemt_UserMsg& userMsg() const;
        // --------------------------------------------------------------------

        if (verbose) cout << "\nUSAGE EXAMPLE"
                          << "\n=============" << endl;

        using namespace BTEMT_MESSAGE_USAGE_EXAMPLE;

        usageExample();

      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING btemt_Message ON USER MESSAGES
        // Testing:
        //   btemt_Message(const btemt_UserMsg&)
        //   btemt_UserMsg& userMsg();
        //   const btemt_UserMsg& userMsg() const;
        // --------------------------------------------------------------------
        if (verbose) cout << endl
            << "TESTING btemt_Message ON USER MESSAGES" << endl
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
            bcema_TestAllocator localAllocator(veryVeryVerbose);
            bcema_PooledBufferChainFactory factory(BUFFER_LENGTH,
                                                   &localAllocator);

            btemt_UserMsg src;
            src.setType(DATA[i].d_srcType);
            src.setIntData(DATA[i].d_srcIntData);

            if (DATA[i].d_srcIsManaged) {
                bcema_PooledBufferChain *chain =
                    factory.allocate(0);
                src.setManagedData(chain, &factory, &localAllocator);
            }
            else {
                src.setVoidPtrData(DATA[i].d_srcVoidPtrData);
            }

            btemt_Message mX(src);
            const btemt_Message& X = mX;

            btemt_Message mY(mX);

            btemt_UserMsg &srcMX = mX.userMsg();
            const btemt_UserMsg &srcX = X.userMsg();

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
        // TESTING btemt_Message ON BLOB MESSAGES
        // Plan:
        //   We will consider a simple producer-consumer test in which
        //   the main thread will produce items, in form of data messages,
        //   and an auxiliary threads, managed by the pool will consume them.
        // Testing:
        //   btemt_Message(const btemt_DataMsg&)
        //   btemt_DataMsg& dataMsg();
        //   const btemt_UserMsg& dataMsg() const;
        // --------------------------------------------------------------------
        if (verbose) cout << endl
            << "TESTING btemt_Message ON BLOB MESSAGES" << endl
            << "======================================" << endl;

        bcema_TestAllocator testAllocator;
        enum { CHUNK_SIZE = 32 };
        bcema_PooledBlobBufferFactory factory(CHUNK_SIZE, &testAllocator);

        enum {
            NUM_MESSAGES     = 100,
            NUM_SUB_MESSAGES = 30
        };

        bcemt_Attribute defaultAttributes;
        bcep_ThreadPool pool(defaultAttributes, 1, 2, 100);
        bcec_Queue<btemt_Message> queue;
        pool.start();
        bces_AtomicInt count = 0;
        bdef_Function<void (*)()> functor(
                bdef_BindUtil::bind(&processMessages, &queue, &count));
        for (int i = 0; i < NUM_MESSAGES; ++i) {
            int length = NUM_SUB_MESSAGES * CHUNK_SIZE + 1;
            bcema_SharedPtr<bcema_Blob> blob;
            blob.createInplace(&testAllocator, &factory, &testAllocator);
            blob->setLength(length);

            btemt_BlobMsg msg(blob, i);
            for (int j = 0; j < NUM_SUB_MESSAGES; ++j) {
                msg.setUserDataField1(j);
                msg.setUserDataField2(j + CHUNK_SIZE - 1);
                btemt_Message subMsg(msg);
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
        // TESTING btemt_Message ON DATA MESSAGES
        // Plan:
        //   We will consider a simple producer-consumer test in which
        //   the main thread will produce items, in form of data messages,
        //   and an auxiliary threads, managed by the pool will consume them.
        // Testing:
        //   btemt_Message(const btemt_DataMsg&)
        //   btemt_DataMsg& dataMsg();
        //   const btemt_UserMsg& dataMsg() const;
        // --------------------------------------------------------------------
        if (verbose) cout << endl
            << "TESTING btemt_Message ON DATA MESSAGES" << endl
            << "======================================" << endl;

        bcema_TestAllocator testAllocator;
        enum { CHUNK_SIZE = 32 };
        bcema_PooledBufferChainFactory factory(CHUNK_SIZE, &testAllocator);

        enum {
            NUM_MESSAGES     = 100,
            NUM_SUB_MESSAGES = 30
        };

        bcemt_Attribute defaultAttributes;
        bcep_ThreadPool pool(defaultAttributes, 1, 2, 100);
        bcec_Queue<btemt_Message> queue;
        pool.start();
        bces_AtomicInt count = 0;
        bdef_Function<void (*)()>functor(
                bdef_BindUtil::bind(&processMessages, &queue, &count));
        for (int i = 0; i < NUM_MESSAGES; ++i) {
            int length = NUM_SUB_MESSAGES * CHUNK_SIZE + 1;
            bcema_PooledBufferChain *chain = factory.allocate(length);
            btemt_DataMsg msg(chain, &factory, i, &testAllocator);

            for (int j = 0; j < NUM_SUB_MESSAGES; ++j) {
                msg.setUserDataField1(j);
                msg.setUserDataField2(j + chain->bufferSize() - 1);
                btemt_Message subMsg(msg);
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
        // TESTING btemt_UserMsg ASSIGNMENT OPERATOR
        //  Verify that 'btemt_UserMsg''s assignment operator by creating
        //  two objects in different states, assigning one to another and
        //  verifying state.  Also verify that aliasing is handled properly.
        //
        // Testing:
        //  btemt_UserMsg& operator=(const btemt_UserMsg& rhs);
        // --------------------------------------------------------------------
        if (verbose) cout << endl
            << "TESTING btemt_UserMsg ASSIGNMENT OPERATOR" << endl
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
            bcema_TestAllocator localAllocator(veryVeryVerbose);
            bcema_Pool myPool(BUFFER_LENGTH, &localAllocator);
            DefaultDeleter<bcema_PooledBufferChain>
                deleter(&localAllocator);

            btemt_UserMsg src;
            src.setType(DATA[i].d_srcType);
            src.setIntData(DATA[i].d_srcIntData);
            if (DATA[i].d_srcIsManaged) {
                bcema_PooledBufferChain *chain =
                    new (localAllocator) bcema_PooledBufferChain(&myPool);

                src.setManagedData(chain, &deleter, &localAllocator);
            }
            else {
                src.setVoidPtrData(DATA[i].d_srcVoidPtrData);
            }

            btemt_UserMsg dst;
            dst.setType(DATA[i].d_dstType);
            dst.setIntData(DATA[i].d_dstIntData);
            if (DATA[i].d_dstIsManaged) {
                bcema_PooledBufferChain *chain =
                    new (localAllocator) bcema_PooledBufferChain(&myPool);

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
        // TESTING btemt_UserMsg COPY CONSTRUCTOR
        //  Verify that 'btemt_UserMsg''s copy constructor functions properly.
        //  Particularly, copy construct from both opaque and managed messages.
        //  and verify state.
        //
        // Testing:
        //  btemt_UserMsg(const btemt_UserMsg& original);
        // --------------------------------------------------------------------
        if (verbose) cout << endl
            << "TESTING btemt_UserMsg COPY CONSTRUCTOR" << endl
            << "======================================" << endl;
        {
            btemt_UserMsg mX;
            btemt_UserMsg mY(mX);
            ASSERT(btemt_UserMsg::BTEMT_INVALID_INT_DATA == mY.intData());
            ASSERT(btemt_UserMsg::BTEMT_INVALID_MESSAGE_TYPE == mY.type());
            ASSERT(0 == mY.isManaged());
            ASSERT((void*)btemt_UserMsg::BTEMT_INVALID_VOID_DATA ==
                   mY.voidPtrData());
        }
        enum {
            TEST_TYPE = 5,
            TEST_INT_DATA = 6
        };
        void *TEST_VOID_DATA = (void*) 0xAB;

        {
            btemt_UserMsg mX; mX.setType(TEST_TYPE);
            btemt_UserMsg mY(mX);
            ASSERT(btemt_UserMsg::BTEMT_INVALID_INT_DATA == mY.intData());
            ASSERT(TEST_TYPE == mY.type());
            ASSERT(0 == mY.isManaged());
            ASSERT((void*)btemt_UserMsg::BTEMT_INVALID_VOID_DATA ==
                   mY.voidPtrData());
        }
        {
            btemt_UserMsg mX; mX.setIntData(TEST_INT_DATA);
            btemt_UserMsg mY(mX);

            ASSERT(TEST_INT_DATA == mY.intData());
            ASSERT(btemt_UserMsg::BTEMT_INVALID_MESSAGE_TYPE == mY.type());
            ASSERT(0 == mY.isManaged());
            ASSERT((void*)btemt_UserMsg::BTEMT_INVALID_VOID_DATA ==
                   mY.voidPtrData());

        }
        {
            btemt_UserMsg mX; mX.setVoidPtrData(TEST_VOID_DATA);
            btemt_UserMsg mY(mX);

            ASSERT(btemt_UserMsg::BTEMT_INVALID_INT_DATA == mY.intData());
            ASSERT(btemt_UserMsg::BTEMT_INVALID_MESSAGE_TYPE == mY.type());
            ASSERT(0 == mY.isManaged());
            ASSERT(TEST_VOID_DATA == mY.voidPtrData());
        }
        {
            bcema_TestAllocator   localTestAllocator(veryVeryVerbose);
            enum { NUM_BYTES = 64 }; // some number
            bcema_Pool myPool(NUM_BYTES, &localTestAllocator);
            DefaultDeleter<bcema_PooledBufferChain>
                deleter(&localTestAllocator);

            bcema_PooledBufferChain *chain =
                    new (localTestAllocator) bcema_PooledBufferChain(&myPool);

            btemt_UserMsg mX; mX.setManagedData(chain, &deleter,
                                                &localTestAllocator);
            btemt_UserMsg mY(mX);
            ASSERT(btemt_UserMsg::BTEMT_INVALID_INT_DATA == mY.intData());
            ASSERT(btemt_UserMsg::BTEMT_INVALID_MESSAGE_TYPE == mY.type());
            ASSERT(1 == mY.isManaged());
            ASSERT(chain == mY.managedData().ptr());
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING btemt_UserMsg
        //  Verify that 'btemt_UserMsg' class functions correctly.
        //
        // Plan:
        //  Exercise methods one at a time; pay extra attention to
        // managed property and managed data members.
        // --------------------------------------------------------------------
        if (verbose) cout << endl
            << "TESTING btemt_UserMsg: Basic Manipulators" << endl
            << "=========================================" << endl;
        {
            btemt_UserMsg mX;
            ASSERT(btemt_UserMsg::BTEMT_INVALID_INT_DATA == mX.intData());
            ASSERT(btemt_UserMsg::BTEMT_INVALID_MESSAGE_TYPE == mX.type());
            ASSERT(0 == mX.isManaged());
            ASSERT((void*)btemt_UserMsg::BTEMT_INVALID_VOID_DATA ==
                   mX.voidPtrData());

        }
        enum {
            TEST_TYPE = 5,
            TEST_INT_DATA = 6
        };
        void *TEST_VOID_DATA = (void*) 0xAB;

        {
            btemt_UserMsg mX; mX.setType(TEST_TYPE);
            ASSERT(btemt_UserMsg::BTEMT_INVALID_INT_DATA == mX.intData());
            ASSERT(TEST_TYPE == mX.type());
            ASSERT(0 == mX.isManaged());
            ASSERT((void*)btemt_UserMsg::BTEMT_INVALID_VOID_DATA ==
                   mX.voidPtrData());
        }
        {
            btemt_UserMsg mX; mX.setIntData(TEST_INT_DATA);
            ASSERT(TEST_INT_DATA == mX.intData());
            ASSERT(btemt_UserMsg::BTEMT_INVALID_MESSAGE_TYPE == mX.type());
            ASSERT(0 == mX.isManaged());
            ASSERT((void*)btemt_UserMsg::BTEMT_INVALID_VOID_DATA ==
                   mX.voidPtrData());

        }
        {
            btemt_UserMsg mX; mX.setVoidPtrData(TEST_VOID_DATA);
            ASSERT(btemt_UserMsg::BTEMT_INVALID_INT_DATA == mX.intData());
            ASSERT(btemt_UserMsg::BTEMT_INVALID_MESSAGE_TYPE == mX.type());
            ASSERT(0 == mX.isManaged());
            ASSERT(TEST_VOID_DATA == mX.voidPtrData());
        }
        {
            bcema_TestAllocator   localTestAllocator(veryVeryVerbose);
            enum { NUM_BYTES = 64 }; // some number
            bcema_Pool myPool(NUM_BYTES, &localTestAllocator);
            DefaultDeleter<bcema_PooledBufferChain>
                deleter(&localTestAllocator);

            bcema_PooledBufferChain *chain =
                    new (localTestAllocator) bcema_PooledBufferChain(&myPool);

            btemt_UserMsg mX; mX.setManagedData(chain, &deleter,
                                                &localTestAllocator);
            ASSERT(btemt_UserMsg::BTEMT_INVALID_INT_DATA == mX.intData());
            ASSERT(btemt_UserMsg::BTEMT_INVALID_MESSAGE_TYPE == mX.type());
            ASSERT(1 == mX.isManaged());
            ASSERT(chain == mX.managedData().ptr());
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
            btemt_ChannelMsg mX; const btemt_ChannelMsg& X = mX;
            if (veryVerbose) P(mX);
            ASSERT(btemt_ChannelMsg::BTEMT_INVALID_ID == X.channelId());
            ASSERT(btemt_ChannelMsg::BTEMT_INVALID_ID == X.allocatorId());
            ASSERT(btemt_ChannelMsg::BTEMT_CHANNEL_DOWN == X.event());
        }
        {
            btemt_PoolMsg mX; const btemt_PoolMsg& X = mX;
            if (veryVerbose) P(mX);
            ASSERT(btemt_PoolMsg::BTEMT_INVALID_ID == X.sourceId());
            ASSERT(btemt_PoolMsg::BTEMT_ACCEPT_TIMEOUT == X.event());
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
