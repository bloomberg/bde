// btlmt_asyncchannel.t.cpp                                           -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------


#include <btlmt_asyncchannel.h>

#include <btlb_blob.h>
#include <bsls_timeinterval.h>
#include <bslma_testallocator.h>

#include <bsl_iostream.h>

#include <bsl_cstdlib.h>     // atoi()

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// We are testing a pure protocol class.  We need to verify that a concrete
// derived class compiles and links.  We create a sample derived class that
// provides a dummy implementation of the base class virtual methods.  We then
// verify that when a method is called through a base class instance pointer
// the appropriate method in the derived class instance is invoked.
//-----------------------------------------------------------------------------
// [ 1] ~btlmt::AsyncChannel();
// [ 1] int read(int numBytes, const BlobBasedReadCallback& readCallback);
// [ 1] int timedRead(int numBytes, timeout, readCallback);
// [ 1] int write(const btlb::Blob&, int);
// [ 1] int setSocketOption(int option, int level, int value);
// [ 1] void cancelRead()
// [ 1] void close()
// [ 1] btlso::IPv4Address localAddress() const;
// [ 1] btlso::IPv4Address peerAddress() const;
//-----------------------------------------------------------------------------
// [ 1] PROTOCOL TEST - Make sure derived class compiles and links.
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
namespace BloombergLP {
namespace btlmt {

class ChannelPool {
    // This is a dummy implementation just for this usage example.

  public:
    // CREATORS
    ChannelPool() {}
    ~ChannelPool() {}

    // MANIPULATORS
    int disableRead(int) { return 0; }
    int setSocketOption(int, int, int, int) { return 0; }
    int shutdown(int) { return 0; }
    int write(int, btlb::Blob, int) { return 0; }

    // ACCESSORS
    int getLocalAddress(btlso::IPv4Address *, int) const { return 0; }
    int getPeerAddress(btlso::IPv4Address *, int) const { return 0; }
};

}
}

///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Implementing a concrete channel type
///- - - - - - - - - - - - - - - - - - - - - - - -
// This 'class', 'btlmt::AsyncChannel', provides a pure protocol for a network
// connection type that once connected to the peer allows asynchronous reads
// and writes.
//
// In this usage example we will implement a derived implementation of
// 'btlmt::AsyncChannel', 'my_AsyncChannel'.  An object of this type is
// constructed with a 'btlmt::ChannelPool' object and provided a channel id
// that represents this connection in that channel pool object.  For the
// brevity of this example, we will elide some portions of the class
// implementation including error checking for connection closure and
// synchronizing access to a 'my_AsyncChannel' across multiple threads.
//
// First, we provide the class definition:
//..
    class my_AsyncChannel : public btlmt::AsyncChannel {
        // This 'class' provides a concrete implementation of
        // 'btlmt::AsyncChannel'.
//
//..
// Next, we specify an 'Entry' type that stores the data corresponding to a
// read callback:
//..
        // TYPES
        struct Entry {
            // This 'struct' provides a type storing the attributes of a read
            // callback.
//
            BlobBasedReadCallback d_readCallback;    // read callback
//
            bsls::TimeInterval    d_timeOut;         // optional read timeout
//
            int                   d_numBytesNeeded;  // number of bytes needed
                                                     // before to invoke the
                                                     // read callback
        };
//
        typedef bsl::vector<Entry> ReadQueue;
//
//..
// Then, we specify the data for this type.  This type stores a
// 'btlmt::ChannelPool', the channelId in that channel pool used to represent
// this connection, and a vector storing all the read callbacks:
//..
        // DATA
        int                 d_channelId;
        btlmt::ChannelPool *d_channelPool_p;
        ReadQueue           d_entries;
//
//..
// Next, we specify the interface of this class:
//..
      public:
        // CREATORS
        my_AsyncChannel(int                 channelId,
                        btlmt::ChannelPool *channelPool,
                        bslma::Allocator   *basicAllocator = 0);
            // Create a 'my_AsyncChannel' concrete implementation reading from
            // and writing to the channel referenced by the specified
            // 'channelId' in the specified 'channelPool'.  Optionally specify
            // a 'basicAllocator' used to supply memory.  If 'basicAllocator'
            // is 0, the currently installed default allocator is used.

        virtual ~my_AsyncChannel();
            // Destroy this channel.

        // MANIPULATORS
        virtual int read(int                          numBytes,
                         const BlobBasedReadCallback& readCallback);
            // Initiate an asynchronous read operation on this channel, or
            // append this request to the currently pending requests if an
            // asynchronous read operation was already initiated.  When at
            // least the specified 'numBytes' of data are available after all
            // previous requests have been processed, if any, the specified
            // 'readCallback' will be invoked (with
            // 'btlmt::AsyncChannel::e_SUCCESS').  Return 0 on success, and a
            // non-zero value otherwise.  On error, the return value *may*
            // equal to one of the enumerators in 'ChannelStatus::Enum'.

        virtual int timedRead(int                          numBytes,
                              const bsls::TimeInterval&    timeOut,
                              const BlobBasedReadCallback& readCallback);
            // Initiate an asynchronous timed read operation on this channel,
            // or append this request to the currently pending requests if an
            // asynchronous read operation was already initiated, with an
            // associated specified absolute 'timeOut'.  When at least the
            // specified 'numBytes' of data are available after all previous
            // requests have been processed, if any, or when the 'timeOut' is
            // reached, the specified 'readCallback' will be invoked (with
            // either 'btlmt::AsyncChannel::e_SUCCESS' or
            // 'btlmt::AsyncChannel::e_TIMEOUT', respectively).  Return 0 on
            // success, and a non-zero value otherwise.  On error, the return
            // value *may* equal to one of the enumerators in
            // 'ChannelStatus::Enum'.

        virtual int write(const btlb::Blob& blob,
                          int               highWaterMark = INT_MAX);
            // Enqueue the specified 'blob' message to be written to this
            // channel.  Optionally provide 'highWaterMark' to specify the
            // maximum data size that can be enqueued.  If 'highWaterMark' is
            // not specified then 'INT_MAX' is used.  Return 0 on success, and
            // a non-zero value otherwise.  On error, the return value *may*
            // equal to one of the enumerators in 'ChannelStatus::Enum'.  Note
            // that success does not imply that the data has been written or
            // will be successfully written to the underlying stream used by
            // this channel.  Also note that in addition to 'highWatermark'
            // the enqueued portion must also be less than a high watermark
            // value supplied at the construction of this channel for the
            // write to succeed.

        virtual int setSocketOption(int option, int level, int value);
            // Set the specified 'option' (of the specified 'level') socket
            // option on this channel to the specified 'value'.  Return 0 on
            // success and a non-zero value otherwise.  (See
            // 'btlso_socketoptutil' for the list of commonly supported
            // options.)

        virtual void cancelRead();
            // Cancel all pending 'read' or 'timedRead' requests, and invoke
            // their read callbacks with a 'btlmt::AsyncChannel::e_CANCELED'
            // status.  Note that if the channel is active, the read callbacks
            // are invoked in the thread in which the channel's data callbacks
            // are invoked, else they are invoked in the thread calling
            // 'cancelRead'.

        virtual void close();
            // Shutdown this channel, and cancel all pending requests.  Note
            // that this call will result in the shutdown of the channel pool
            // channel associated with the channel, and will not invoke the
            // pending read requests.

//..
// Then, we define a method, 'blobBasedReadCb', that can be invoked by a higher
// level component after reading data from the channel pool held by this object
// on the channel id represented by this object.  Note that the 'read' and
// 'timedRead' methods of this type are asynchronous and they do not directly
// result in any data being read from the network.  Instead, this type relies
// on a higher class (like 'btlmt::SessionPool') for the data reads:
//..
        void blobBasedReadCb(int *numNeeded, btlb::Blob *msg);
            // This method is invoked in response to a blob based channel pool
            // data callback on the channel id associated with this channel,
            // and invokes the pending read requests until either more data is
            // needed or this channel is closed.

        // ACCESSORS
        virtual btlso::IPv4Address localAddress() const;
            // Return the address of the "local" end of the channel.

        virtual btlso::IPv4Address peerAddress() const;
            // Return the address of the "remote" end of the channel.
    };
//..
// Then, we provide the function implementations:
//..
    my_AsyncChannel::my_AsyncChannel(int                 channelId,
                                     btlmt::ChannelPool *channelPool,
                                     bslma::Allocator   *basicAllocator)
    : d_channelId(channelId)
    , d_channelPool_p(channelPool)
    , d_entries(basicAllocator)
    {
    }
//
    my_AsyncChannel::~my_AsyncChannel()
    {
        // Cancel future callbacks, but do not invoke them if this channel is
        // closed.
//
        cancelRead();
        d_channelPool_p->shutdown(d_channelId);
    }
//
    int my_AsyncChannel::read(int                          numBytes,
                              const BlobBasedReadCallback& readCallback)
    {
        return timedRead(numBytes, bsls::TimeInterval(), readCallback);
    }

    int my_AsyncChannel::timedRead(int                          numBytes,
                                   const bsls::TimeInterval&    timeOut,
                                   const BlobBasedReadCallback& readCallback)
    {
        Entry entry;
        entry.d_numBytesNeeded = numBytes;
        entry.d_timeOut        = timeOut;
        entry.d_readCallback   = readCallback;
//
        if (bsls::TimeInterval(0) != timeOut) {
            // Register a callback to be invoked when timeout expires (elided
            // for brevity).
//
            // . . .
        }
        d_entries.push_back(entry);
        return 0;
    }

    int my_AsyncChannel::write(const btlb::Blob& blob,
                               int               highWaterMark)
    {
        return d_channelPool_p->write(d_channelId, blob, highWaterMark);
    }
//
    int my_AsyncChannel::setSocketOption(int option, int level, int value)
    {
        return d_channelPool_p->setSocketOption(option,
                                                level,
                                                value,
                                                d_channelId);
    }
//
    void my_AsyncChannel::cancelRead()
    {
        ReadQueue::iterator iter = d_entries.begin();
        ReadQueue::iterator end  = d_entries.end();
//
        int        dummy = 0;
        btlb::Blob dummyBlob;
        for (iter = d_entries.begin(); iter != end; ++iter) {
            BlobBasedReadCallback callback = iter->d_readCallback;
//
            callback(btlmt::AsyncChannel::e_CANCELED,
                     &dummy,
                     &dummyBlob,
                     d_channelId);
        }
//
        d_entries.erase(iter, end);
    }

    void my_AsyncChannel::close()
    {
        d_channelPool_p->shutdown(d_channelId);
//
        ReadQueue::iterator iter = d_entries.begin();
        ReadQueue::iterator end  = d_entries.end();
//
        for (iter = d_entries.begin(); iter != end; ++iter) {
            if (bsls::TimeInterval(0) != iter->d_timeOut) {
                // Deregister timeout.  Not shown here.
//
                // . . .
            }
        }
//
        d_entries.erase(iter, end);
    }
//
    void my_AsyncChannel::blobBasedReadCb(int *numNeeded, btlb::Blob *msg)
    {
        *numNeeded            = 1;
        int numBytesAvailable = msg->length();

        while (d_entries.size()
            && d_entries.front().d_numBytesNeeded <= numBytesAvailable) {

            Entry& entry = d_entries.front();

            int numConsumed = 0;
            int nNeeded     = 0;

            const BlobBasedReadCallback& callback = entry.d_readCallback;
            numBytesAvailable = msg->length();

            {
                callback(e_SUCCESS, &nNeeded, msg, d_channelId);
                numConsumed = numBytesAvailable - msg->length();
            }

            ASSERT(0 <= nNeeded);
            ASSERT(0 <= numConsumed);

            numBytesAvailable -= numConsumed;

            if (nNeeded) {
                entry.d_numBytesNeeded = nNeeded;
                if (nNeeded <= numBytesAvailable) {
                    continue;
                }

                *numNeeded = nNeeded - numBytesAvailable;
            }
            else {
                d_entries.erase(d_entries.begin());
                if (!d_entries.size()) {
                    d_channelPool_p->disableRead(d_channelId);
                }
            }
        }
    }
//
    // ACCESSORS
    btlso::IPv4Address my_AsyncChannel::localAddress() const
    {
        btlso::IPv4Address address;
        d_channelPool_p->getLocalAddress(&address, d_channelId);
        return address;
    }
//
    btlso::IPv4Address my_AsyncChannel::peerAddress() const
    {
        btlso::IPv4Address address;
        d_channelPool_p->getPeerAddress(&address, d_channelId);
        return address;
    }
//..

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

class TestAsyncChannel : public btlmt::AsyncChannel {

    // DATA
    int *d_funcCode_p;  // code of the function, held

  private:
    // NOT IMPLEMENTED
    TestAsyncChannel(const btlmt::AsyncChannel&);
    TestAsyncChannel& operator=(const btlmt::AsyncChannel&);

  public:
    // CREATORS
    TestAsyncChannel(int *funcCode)
    : d_funcCode_p (funcCode)
    {
        ASSERT(d_funcCode_p);
        *d_funcCode_p = 0;
    }

    ~TestAsyncChannel()
    {
        *d_funcCode_p = 1;
    }

    // MANIPULATORS
    int read(int, const BlobBasedReadCallback&)
    {
        *d_funcCode_p = 3;
        return 0;
    }

    int timedRead(int,
                  const bsls::TimeInterval&,
                  const BlobBasedReadCallback&)
    {
        *d_funcCode_p = 5;
        return 0;
    }

    int write(const btlb::Blob&, int)
    {
        *d_funcCode_p = 6;
        return 0;
    }

    int setSocketOption(int, int, int)
    {
        *d_funcCode_p = 10;
        return 0;
    }

    void cancelRead()
    {
        *d_funcCode_p = 11;
    }

    void close()
    {
        *d_funcCode_p = 12;
    }

    btlso::IPv4Address localAddress() const
    {
        *d_funcCode_p = 13;
        return btlso::IPv4Address();
    }

    btlso::IPv4Address peerAddress() const
    {
        *d_funcCode_p = 14;
        return btlso::IPv4Address();
    }
};

void myBlobBasedReadCallback(int,
                             int *,
                             btlb::Blob *,
                             int)
{
}

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;
    bslma::TestAllocator testAllocator(veryVeryVerbose);

    cout << "TEST " << __FILE__ << " CASE " << test << endl;;

    switch (test) { case 0:  // Zero is always the leading case.
      case 1: {
       // --------------------------------------------------------------------
       // PROTOCOL TEST:
       //   All we need to do is make sure that a concrete subclass of the
       //   'btlsc::CbChannel' class compiles and links when all
       //   virtual functions are defined.
       //
       // Testing:
       //   ~btlmt::AsyncChannel(...);
       //   int read(...);
       //   int timedRead(...);
       //   void cancelRead()
       //   void close()
       //   btlso::IPv4Address localAddress() const;
       //   btlso::IPv4Address peerAddress() const;
       //
       //   PROTOCOL TEST - Make sure derived class compiles and links.
       // --------------------------------------------------------------------

       if (verbose) bsl::cout << "BREATHING TEST." << bsl::endl
                              << "===============" << bsl::endl;

       if (veryVerbose) bsl::cout <<
                "\tTesting 'btlmt::AsyncChannel': protocol test." << bsl::endl;
       {
           int opCode   = -1;
           int numBytes = 1;
           int timeout  = 600;

           TestAsyncChannel mX(&opCode);
           ASSERT(0 == opCode);

           btlmt::AsyncChannel& asyncChannel = mX;

           btlmt::AsyncChannel::BlobBasedReadCallback blobCallback =
                                                      &myBlobBasedReadCallback;
           asyncChannel.read(numBytes, blobCallback);
           ASSERT(3 == opCode);

           asyncChannel.timedRead(numBytes,
                                  bsls::TimeInterval(timeout),
                                  blobCallback);
           ASSERT(5 == opCode);

           asyncChannel.write(btlb::Blob());
           ASSERT(6 == opCode);

           asyncChannel.setSocketOption(1, 2, 3);
           ASSERT(10 == opCode);

           asyncChannel.cancelRead();
           ASSERT(11 == opCode);

           asyncChannel.close();
           ASSERT(12 == opCode);

           asyncChannel.localAddress();
           ASSERT(13 == opCode);

           asyncChannel.peerAddress();
           ASSERT(14 == opCode);
       }

       if (veryVerbose) bsl::cout
        << "\tTesting 'btlmt::AsyncChannel': destructor test." << bsl::endl;
       {
           int opCode = -1;

           btlmt::AsyncChannel *asyncChannel =
                                  new(testAllocator) TestAsyncChannel(&opCode);
           ASSERT(0 == opCode);

           testAllocator.deleteObjectRaw(asyncChannel);
           ASSERT(1 == opCode);
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
