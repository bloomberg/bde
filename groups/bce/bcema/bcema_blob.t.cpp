// bcema_blob.t.cpp                                                   -*-C++-*-
#include <bcema_blob.h>

#include <bdetu_datetime.h>                     // for testing only
#include <bdetu_systemtime.h>                   // for testing only
#include <bdex_byteoutstreamraw.h>              // for testing only
#include <bdex_bytestreamimputil.h>             // for testing only

#include <bslma_defaultallocatorguard.h>        // for testing only
#include <bslma_testallocator.h>                // for testing only
#include <bslma_testallocatorexception.h>       // for testing only
#include <bsls_assert.h>                        // for testing only

#include <bsl_algorithm.h>
#include <bsl_exception.h>
#include <bsl_iostream.h>
#include <bsl_sstream.h>
#include <bsl_vector.h>

#include <bsl_cctype.h>      // isdigit() isupper() islower()
#include <bsl_cstdlib.h>     // atoi()
#include <bsl_cstring.h>

using namespace BloombergLP;
using bsl::cout;
using bsl::flush;
using bsl::endl;
using bsl::cerr;

//==========================================================================
//                             TEST PLAN
//--------------------------------------------------------------------------
//                              Overview
//                              --------
// This test driver verifies many claims about 'bcema_Blob's which are made in
// the component-level and class-level documentation, as well as the individual
// contracts of each function, namely:
// - that 'bcema_BlobBuffer' is an in-core value-semantic type containing a
//   memory buffer and length with shared ownership.
// - that 'bcema_Blob' is an in-core exception-neutral value-semantic type
//   representing a non-contiguous sequence of bytes stored in a sequence of
//   'bcema_BlobBuffer's.
// We do not provide a full value-semantic type test driver, since it is more
// appropriate here to focus on the way blobs can be created: using 'setLength'
// (which can be considered the primary manipulator) with a blob buffer factory
// passed to the constructor, or by inserting/prepending/appending/removing
// buffers.  The main concerns about 'bcema_Blob' have to do with the automatic
// maintenance of the data length, number of buffers and of data buffers, and
// length of the last data buffer (all of them could be considered primary
// accessors) in a sequence of manipulators.  For this, we consider blobs
// created with three main characteristics:
//   1. the buffer size passed to the factory,
//   2. the number of buffers the blob holds (in capacity), and
//   3. the data length (between 0 and total size).
// In addition, for the insertion/removal, we also consider:
//   4. the position of the insertion / removed buffer (between 0 and number of
//      buffers).
// It turns out we can derive full confidence in our implementation by trying
// all possible buffer sizes and number of buffers between 1 and (say) 5, and
// letting all the other parameters (data length, buffer index) take all
// possible values in their constrained range.  We make sure that the case of
// empty blobs with empty or non-empty capacities are always taken into
// account, and run bdema exception test loops around all these.  With this
// test driver, there is almost no room for a bug in the component.
//--------------------------------------------------------------------------
// [ 2] bcema_Blob(allocator);
// [ 2] bcema_Blob(factory, allocator);
// [ 2] bcema_Blob(buffers, numBuffers, allocator);
// [ 3] void bcema_Blob::setLength(newLength);
// [ 3] int bcema_Blob::buffer(index);
// [ 3] int bcema_Blob::length();
// [ 3] int bcema_Blob::numBuffers();
// [ 4] void bcema_Blob::trimLastDataBuffer();
// [ 4] int bcema_Blob::lastDataBufferLength();
// [ 4] int bcema_Blob::numDataBuffers();
// [ 5] void bcema_Blob::insertBuffer(index, buffer);
// [ 6] void bcema_Blob::appendBuffer(buffer);
// [ 7] void bcema_Blob::removeAll();
// [ 7] void bcema_Blob::removeBuffer(index);
// [ 8] void bcema_Blob::prependDataBuffer(buffer);
// [ 8] void bcema_Blob::appendDataBuffer(buffer)
// [ 9] void bcema_Blob::moveBuffers(bcema_Blob *srcBlob);
// [11] void bcema_Blob::moveDataBuffers(bcema_Blob *srcBlob);
// [11] void bcema_Blob::moveAndAppendDataBuffers(bcema_Blob *srcBlob);
// [10] void bcema_Blob::swapBufferRaw(int index, bcema_BlobBuffer *srcBuffer);
//--------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [12] CONCERN: BUFFER ALIASING
// [13] USAGE EXAMPLE
//--------------------------------------------------------------------------

//==========================================================================
//                      STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
int testStatus = 0;

void aSsErT(int c, const char *s, int i) // not static, to use it in template
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
#define T_() cout << '\t' << flush;

//=============================================================================
//                  GLOBAL HELPER CLASSES/FUNCTIONS
//-----------------------------------------------------------------------------

static int numUnknownFactoryHandlerInvocations = 0;

class UnknownFactoryException : public bsl::exception {
    const char *d_what;
  public:
    UnknownFactoryException(const char *s) : d_what(s) {}
    virtual ~UnknownFactoryException() throw() {}
    virtual const char* what() const throw() { return d_what; }
};

void unknownFactoryHandler(const char *, const char *, int) {
    ++numUnknownFactoryHandlerInvocations;
    throw UnknownFactoryException("Oops!");
}

void checkNoAliasedBlobBuffers(const bcema_Blob& blob)
    // This function checks that all buffers are valid for write, and that no
    // two buffers are aliased.
{
    for (int jump = 0; jump < blob.numBuffers(); ++jump) {
        // Fill in forward order with increasing values, with jump in between
        // buffers.
        char filler = 0;
        for (int i = 0; i < blob.numBuffers(); ++i) {
            for (int j = 0; j < blob.buffer(i).size(); ++j) {
                blob.buffer(i).data()[j] = ++filler;
            }
            filler += jump;
        }
        // Compare in forward order.
        filler = 0;
        for (int i = 0; i < blob.numBuffers(); ++i) {
            for (int j = 0; j < blob.buffer(i).size(); ++j) {
                ASSERT(blob.buffer(i).data()[j] == ++filler);
            }
            filler += jump;
        }
    }
}

void checkBlobBuffers(const bcema_Blob& blob)
    // This function checks that all buffers are valid for write.  Buffers can
    // be aliased.
{
    char filler = 0;
    for (int i = 0; i < blob.numBuffers(); ++i, ++filler) {
        // Fill in forward order with increasing values, and check result.
        bsl::memset(blob.buffer(i).data(), filler, blob.buffer(i).size());
        for (int j = 0; j < blob.buffer(i).size(); ++j) {
            ASSERT(blob.buffer(i).data()[j] == filler);
        }
    }
}

bool checkTotalSize(const bcema_Blob& blob)
    // check d_totalSize is accurate and sane
{
    int total = 0;
    for (int i = 0; i < blob.numBuffers(); ++i) {
        total += blob.buffer(i).size();
    }

    LOOP2_ASSERT(blob.totalSize(), total, blob.totalSize() == total);
    return blob.totalSize() == total;
}

void loadBlob(bcema_Blob *blob, bsl::string& dataString)
{
    const char *data = dataString.data();
    const int NUM_DATA_BUFFERS = blob->numDataBuffers();
    const int DATA_LENGTH      = blob->length();

    ASSERT((int) dataString.length() == DATA_LENGTH);

    int bufferIdx = 0;
    int dataCharIdx = 0;

    for ( ; bufferIdx < NUM_DATA_BUFFERS; ++bufferIdx) {
        const bcema_BlobBuffer& buffer = blob->buffer(bufferIdx);
        for (char *pc = buffer.data(), *end = pc + buffer.size();
                  pc < end && dataCharIdx < DATA_LENGTH; ++pc, ++dataCharIdx) {
            *pc = data[dataCharIdx];
        }
    }
}

void blobToStr(bsl::string * str, const bcema_Blob& blob) {
    const int NUM_PRE_DATA_BUFFERS = blob.numDataBuffers() - 1;
    const int DATA_LENGTH          = blob.length();

    if (0 == DATA_LENGTH) {
        return;                                                       // RETURN
    }

    str->reserve(DATA_LENGTH);

    int bufferIdx = 0;

    for ( ; bufferIdx < NUM_PRE_DATA_BUFFERS; ++bufferIdx) {
        const bcema_BlobBuffer& buffer = blob.buffer(bufferIdx);
        str->append(buffer.data(), buffer.size());
    }

    const bcema_BlobBuffer& buffer = blob.buffer(bufferIdx);
    int toAppend = DATA_LENGTH - str->length();
    ASSERT(toAppend <= buffer.size());
    str->append(buffer.data(), toAppend);
}

bool checkBlob(const bcema_Blob& blob, const bsl::string& dataString)
{
    if (blob.length() != (int) dataString.length()) {
        return false;
    }

    const char *data = dataString.data();
    const int NUM_DATA_BUFFERS = blob.numDataBuffers();
    const int DATA_LENGTH      = blob.length();

    int bufferIdx = 0;
    int dataCharIdx = 0;

    for ( ; bufferIdx < NUM_DATA_BUFFERS; ++bufferIdx) {
        const bcema_BlobBuffer& buffer = blob.buffer(bufferIdx);
        for (char *pc = buffer.data(), *end = pc + buffer.size();
                  pc < end && dataCharIdx < DATA_LENGTH; ++pc, ++dataCharIdx) {
            if (*pc != data[dataCharIdx]) {
                return false;
            }
        }
    }

    return false;
}

void populateBuffersWithData(bcema_Blob *blob, int numBuffers, char value)
    // Populate the specified 'numBuffers' in the specified 'blob' with the
    // specified 'value'.
{
    if (0 == numBuffers) {
        return;
    }
    for (int i = 0; i < numBuffers; ++i) {
        bsl::memset(blob->buffer(i).data(), value, blob->buffer(i).size());
    }
}

bool compareBlobBufferData(const bcema_BlobBuffer& blobBuffer, char value)
    // Return 'true' if each char in the specified 'buffer' has the specified
    // 'value', and 'false' otherwise.
{
    const int   bufSize = blobBuffer.size();
    const char *bufData = blobBuffer.data();
    for (int i = 0; i < bufSize; ++i) {
        if (bufData[i] != value) {
            return false;
        }
    }
    return true;
}

bool compareBuffersData(const bcema_Blob& blob,
                        int               numBuffers,
                        char              value,
                        int               exceptIdx = -1)
    // Return 'true' if the specified 'numBuffers' in the specified 'blob' all
    // contain the specified 'value' and 'false' otherwise.  Optionally,
    // specify 'exceptIdx' of the a blob buffer to skip from comparison.
{
    ASSERT(-1 <= exceptIdx && exceptIdx < numBuffers);

    if (0 == numBuffers) {
        return false;
    }
    for (int i = 0; i < numBuffers; ++i) {
        if (i == exceptIdx) {
            continue;
        }
        if (!compareBlobBufferData(blob.buffer(i), value)) {
            return false;
        }
    }
    return true;
}

                         // ===========================
                         // class TestBlobBufferFactory
                         // ===========================

class TestBlobBufferFactory : public bcema_BlobBufferFactory
{
    // This class constructs buffers with a size growing in a geometric series
    // or ratio 2, starting with a size specified at construction.

    bslma_Allocator *d_allocator_p;
    bsl::size_t      d_currentBufferSize;
    bool             d_growFlag;

    private:
    // not implemented
    TestBlobBufferFactory(const TestBlobBufferFactory&);
    TestBlobBufferFactory& operator=(const TestBlobBufferFactory&);

    public:
    // CREATORS
    explicit TestBlobBufferFactory(bslma_Allocator *allocator,
                                   bsl::size_t      currentBufferSize = 4,
                                   bool             growFlag = true);
    ~TestBlobBufferFactory();

    // MANIPULATORS
    void allocate(bcema_BlobBuffer *buffer);
    void setGrowFlag(bool growFlag);

    // ACCESSORS
    bsl::size_t currentBufferSize() const;
    bool growFlag() const;
};

TestBlobBufferFactory::TestBlobBufferFactory(bslma_Allocator *allocator,
                                             bsl::size_t      currentBufSize,
                                             bool             growFlag)
: d_allocator_p(allocator)
, d_currentBufferSize(currentBufSize)
, d_growFlag(growFlag)
{
}

TestBlobBufferFactory::~TestBlobBufferFactory()
{
}

void TestBlobBufferFactory::allocate(bcema_BlobBuffer *buffer)
{

    bcema_SharedPtr<char> shptr(
                         (char *) d_allocator_p->allocate(d_currentBufferSize),
                          d_allocator_p);

    buffer->reset(shptr, d_currentBufferSize);
    if (d_growFlag && d_currentBufferSize < 1024) {
        d_currentBufferSize *= 2;
    }
}

void TestBlobBufferFactory::setGrowFlag(bool growFlag)
{
    d_growFlag = growFlag;
}

bsl::size_t TestBlobBufferFactory::currentBufferSize() const
{
    return d_currentBufferSize;
}

bool TestBlobBufferFactory::growFlag() const
{
    return d_growFlag;
}

                         // =================
                         // class NullDeleter
                         // =================

class NullDeleter {
    public:
    void deleteObject(char *) {};
};

//=============================================================================
//                              USAGE EXAMPLE
//-----------------------------------------------------------------------------

                         // =============================
                         // class SimpleBlobBufferFactory
                         // =============================

class SimpleBlobBufferFactory : public bcema_BlobBufferFactory
{
    // This factory creates blob buffers of a fixed size specified at
    // construction.  It is part of the usage example.

    bsl::size_t      d_bufferSize;
    bslma_Allocator *d_allocator_p;

    private:
    // not implemented
    SimpleBlobBufferFactory(const SimpleBlobBufferFactory&);
    SimpleBlobBufferFactory& operator=(const SimpleBlobBufferFactory&);

    public:
    // CREATORS
    explicit SimpleBlobBufferFactory(int              bufferSize = 1024,
                                     bslma_Allocator *basicAllocator = 0);
    ~SimpleBlobBufferFactory();

    // MANIPULATORS
    void allocate(bcema_BlobBuffer *buffer);
};

SimpleBlobBufferFactory::SimpleBlobBufferFactory(
                                           int              bufferSize,
                                           bslma_Allocator *basicAllocator)
: d_bufferSize(bufferSize)
, d_allocator_p(bslma_Default::allocator(basicAllocator))
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

///Simple blob usage
///- - - - - - - - -
// Blobs can be created just by passing a factory that is responsible to
// allocate the 'bcema_BlobBuffer'.  The following simple program illustrate
// how.
//..
void  usageExample() {
    SimpleBlobBufferFactory myFactory(1024);

    bcema_Blob blob(&myFactory);
    ASSERT(0    == blob.length());
    ASSERT(0    == blob.totalSize());

    blob.setLength(512);
    ASSERT( 512 == blob.length());
    ASSERT(1024 == blob.totalSize());
//..
// Users need to access buffers directly in order to read/write data.
//..
    char data[] = "12345678901234567890"; // 20 bytes
    ASSERT(0 != blob.numBuffers());
    ASSERT((int) sizeof(data) <= blob.buffer(0).size());
    bsl::memcpy(blob.buffer(0).data(), data, sizeof(data));

    blob.setLength(sizeof(data));
    ASSERT(sizeof data == blob.length());
    ASSERT(       1024 == blob.totalSize());
//..
// A 'bcema_BlobBuffer' can easily be re-assigned from one blob to another with
// no copy.  In that case, the memory held by the buffer will be returned to
// its factory when the last blob referencing the buffer is destroyed.  For the
// following example, a blob will be created using the default constructor.  In
// this case, the 'bcema_Blob' object will not able to grow on its own.
// Calling 'setLength' for a number equal or greater than 'totalSize()' will
// result in undefined behavior.
//..
    bcema_Blob dest;
    ASSERT(   0 == dest.length());
    ASSERT(   0 == dest.totalSize());

    ASSERT(0 != blob.numBuffers());
    dest.appendBuffer(blob.buffer(0));
    ASSERT(   0 == dest.length());
    ASSERT(1024 == dest.totalSize());
//..
// Note that at this point, the logical length (returned by 'length') of this
// object has not changed.  'setLength' must be called explicitly by the user
// if the logical length of the 'bcema_Blob' should be changed:
//..
    dest.setLength(dest.buffer(0).size());
    ASSERT(1024 == dest.length());
    ASSERT(1024 == dest.totalSize());
//..
// Sharing only a part of a buffer is also possible through shared pointer
// aliasing.  In the following example, a buffer that contains only bytes 11-16
// from the first buffer of 'blob' will be appended to 'blob'.
//..
    ASSERT(0 != blob.numBuffers());
    ASSERT(16 <= blob.buffer(0).size());

    bcema_SharedPtr<char> shptr(blob.buffer(0).buffer(),
                                blob.buffer(0).data() + 10);
        // 'shptr' is now an alias of 'blob.buffer(0).buffer()'.

    bcema_BlobBuffer partialBuffer(shptr, 6);
    dest.appendBuffer(partialBuffer);
        // The last buffer of 'dest' contains only bytes 11-16 from
        // 'blob.buffer(0)'.
}

///Data-oriented manipulation of a blob
///- - - - - - - - - - - - - - - - - -
// There are several typical ways of manipulating a blob: the simplest lets the
// blob automatically manage the length, by using only 'prependBuffer',
// 'appendBuffer', and 'insertBuffer'.  Consider the following typical
// utilities (these utilities are to illustrate usage, they are not meant to be
// copy-pasted into application programs although they can provide a foundation
// for application utilities):
//..
void prependProlog(bcema_Blob          *blob,
                   const bsl::string&   prolog,
                   bslma_Allocator     *allocator = 0);
    // Prepend the specified 'prolog' to the specified 'blob', using the
    // specified 'allocator' to supply any memory (or the currently
    // installed default allocator if 'allocator' is 0).  The behavior is
    // undefined unless 'blob' points to an initialized 'bcema_Blob'
    // instance.

template <class DELETER>
void composeMessage(bcema_Blob          *blob,
                    const bsl::string&   prolog,
                    char * const        *vectors,
                    const int           *vectorSizes,
                    int                  numVectors,
                    const DELETER&       deleter,
                    bslma_Allocator     *allocator = 0);
    // Load into the specified 'blob' the data composed of the specified
    // 'prolog' and of the payload in the 'numVectors' buffers pointed to by
    // the specified 'vectors' of the respective 'vectorSizes'.  Ownership
    // of the vectors is transferred to the 'blob' which will use the
    // specified 'deleter' to destroy them.  Use the specified 'allocator'
    // to supply memory, or the currently installed default allocator if
    // 'allocator' is 0.  Note that any buffer belonging to 'blob' prior to
    // composing the message is not longer in 'blob' after composing the
    // message.  Note also that 'blob' need not have been created with a
    // blob buffer factory.  The behavior is undefined unless 'blob' points
    // to an initialized 'bcema_Blob' instance.

int timestampMessage(bcema_Blob          *blob,
                     bslma_Allocator     *allocator = 0);
    // Insert a timestamp data buffer immediately after the prolog buffer
    // and prior to any payload buffer.  Return the number of bytes
    // inserted.  Use the specified 'allocator' to supply memory, or the
    // currently installed default allocator if 'allocator' is 0.  The
    // behavior is undefined unless 'blob' points to an initialized
    // 'bcema_Blob' instance with at least one data buffer.
//..
// A possible implementation using only 'prependBuffer', 'appendBuffer', and
// 'insertBuffer' could be as follows:
//..
void prependProlog(bcema_Blob          *blob,
                   const bsl::string&   prolog,
                   bslma_Allocator     *)
{
    ASSERT(blob);

    int prologLength = prolog.length();
    SimpleBlobBufferFactory fa(prologLength + sizeof(int));
    bcema_BlobBuffer prologBuffer;
    fa.allocate(&prologBuffer);

    bdex_ByteStreamImpUtil::putInt32(prologBuffer.data(), prologLength);
    bsl::memcpy(prologBuffer.data() + sizeof(int),
                prolog.c_str(),
                prologLength);
    ASSERT(prologBuffer.size() == prologLength + (int) sizeof(int));

    blob->prependDataBuffer(prologBuffer);
}
//..
// Note that the length of 'blob' in the above implementation is automatically
// and always incremented by the 'prologBuffer.size()'.  Consider instead:
//..
//      blob->insertBuffer(0, prologBuffer);
//..
// which inserts the prologBuffer before the first buffer of 'blob'.  This call
// will almost always adjust the length properly *except* if the length of
// 'blob' is 0 before the insertion (i.e., the message has an empty payload).
// In that case, the resulting 'blob' will still be empty after
// 'prependProlog', which depending on the intention of the programmer, could
// be intended (avoid sending empty messages) or could be (most likely) a
// mistake.
//
// The 'composeMessage' implementation is simplified by using 'prependProlog':
//..
template <class DELETER>
void composeMessage(bcema_Blob         *blob,
                    const bsl::string&  prolog,
                    char * const       *vectors,
                    const int          *vectorSizes,
                    int                 numVectors,
                    const DELETER&      deleter,
                    bslma_Allocator    *allocator)
{
    ASSERT(blob);
    ASSERT(vectors);
    ASSERT(0 <= numVectors);

    blob->removeAll();
    prependProlog(blob, prolog, allocator);

    for (int i = 0; i < numVectors; ++i) {
        bcema_SharedPtr<char> shptr(vectors[i], deleter, allocator);
        bcema_BlobBuffer partialBuffer(shptr, vectorSizes[i]);
        blob->appendDataBuffer(partialBuffer);
            // The last buffer of 'dest' contains only bytes 11-16 from
            // 'blob.buffer(0)'.
    }
}
//..
// Note that the 'deleter' is used to destroy the buffers transferred by
// 'vectors', but not the prolog buffer.
//
// Timestamping a message is done by creating a buffer holding a timestamp, and
// inserting it after the prolog and before the payload of the message.  Note
// that in usual messages, timestamps would be part of the prolog itself, so
// this is a somewhat constrained example for exposition only.
//..
int timestampMessage(bcema_Blob          *blob,
                      bslma_Allocator     *allocator)
{
    ASSERT(blob);
    ASSERT(0 < blob->numDataBuffers());

    bcema_BlobBuffer buffer;
    bdet_Datetime now = bdetu_SystemTime::nowAsDatetimeGMT();

    SimpleBlobBufferFactory fa(128, allocator);
    bcema_BlobBuffer timestampBuffer;
    fa.allocate(&timestampBuffer);

    bdex_ByteOutStreamRaw bdexStream(timestampBuffer.data(), 128);
    now.bdexStreamOut(bdexStream, 1);
    ASSERT(bdexStream); // is valid (i.e., did not overflow 128 bytes)
    timestampBuffer.setSize(bdexStream.length());
//..
// Now that we have fabricated the buffer holding the current data and time, we
// must insert it into the blob after the first buffer (i.e., before the
// buffered at the index 1).  Note however that the payload could be empty, a
// condition tested by the fact that there is only one data buffer in 'blob'.
// In that case, it would be a mistake to use 'insertBuffer' since it would not
// modify the length of the blob.
//..
    if (1 < blob->numDataBuffers()) {
        blob->insertBuffer(1, timestampBuffer);
    } else {
        blob->appendDataBuffer(timestampBuffer);
    }

    return bdexStream.length();
}
//..
// Note that the call to 'appendDataBuffer' takes also care of the possibility
// that the first buffer of 'blob' may not be full to capacity (if the length
// of the blob was smaller than the buffer size, only the first
// 'blob->length()' bytes would contain prolog data).  In that case, that
// buffer is trimmed before appending the 'timestampBuffer' so that the first
// byte of the 'timestampBuffer' appears immediately next to the last prolog
// byte, and the blob length is automatically incremented by the size of the
// 'timestampBuffer'.

/// Additional: test those utilities (not in the usage example)

void usageExample2()
{
    bslma_TestAllocator ta;
    SimpleBlobBufferFactory fa(1024);

    bcema_Blob blob(&ta);
    bcema_BlobBuffer buffer;
    fa.allocate(&buffer); blob.appendBuffer(buffer);
    fa.allocate(&buffer); blob.appendBuffer(buffer);
    ASSERT(0 == blob.length());
    ASSERT(2 == blob.numBuffers());

    // Testing 'prependProlog'
    const bsl::string PROLOG("This is a prolog");
    const int PROLOG_LENGTH = PROLOG.length() + sizeof(int);

    prependProlog(&blob, PROLOG, &ta);
    ASSERT(PROLOG_LENGTH        == blob.length());
    ASSERT(PROLOG_LENGTH + 2048 == blob.totalSize());
    ASSERT(1                    == blob.numDataBuffers());
    ASSERT(3                    == blob.numBuffers());

    // Testing 'composeMessage'
    const char *const MSG[] = {
        "Here is the first piece",
        "A second piece",
        "trailer"
    };
    const int MSG_SIZES[] = {
        bsl::strlen(MSG[0]),
        bsl::strlen(MSG[1]),
        bsl::strlen(MSG[2])
    };
    const int NUM_MSG_BUFFERS = sizeof MSG / sizeof *MSG;
    const int MSG_LENGTH      = MSG_SIZES[0] + MSG_SIZES[1] + MSG_SIZES[2];
    const int TOTAL_SIZE      = PROLOG_LENGTH + MSG_LENGTH;

    NullDeleter deleter;
    composeMessage(&blob,
                   PROLOG,
                   const_cast<char * const *>(MSG),
                   (const int *)MSG_SIZES,
                   NUM_MSG_BUFFERS,
                   &deleter, &ta);
    ASSERT(PROLOG_LENGTH + MSG_LENGTH == blob.length());
    ASSERT(TOTAL_SIZE                 == blob.totalSize());
    ASSERT(4                          == blob.numDataBuffers());
    ASSERT(4                          == blob.numBuffers());

    // Testing 'timestampMessage'
    const int TIMESTAMP_LENGTH = timestampMessage(&blob, &ta);
    ASSERT(PROLOG_LENGTH + TIMESTAMP_LENGTH + MSG_LENGTH == blob.length());
    ASSERT(TOTAL_SIZE + TIMESTAMP_LENGTH == blob.totalSize());
    ASSERT(5                             == blob.numDataBuffers());
    ASSERT(5                             == blob.numBuffers());
}

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? bsl::atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;;

    switch (test) { case 0:  // Zero is always the leading case.
      case 15: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE
        //
        // Concerns:
        //   - The usage example must compile and run as shown in the header.
        //
        // Plan:
        //   Copy from header and replace assert by ASSERT.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING USAGE EXAMPLE" << endl
                          << "=====================" << endl;

        usageExample();

        usageExample2();

      } break;
      case 14: {
        // --------------------------------------------------------------------
        // VERIFYING NOT PARANOID
        //
        // Concerns:
        //   A developer may have checked in code with the expensive 'paranoid'
        //   checks turned on.
        //
        // Plan:
        //   Call the 'notParanoid' class method which will return the value
        //   of the 'NOT_PARANOID' enum and assert that it is true; thus if a
        //   developer checks in code with '0 == NOT_PARANOID', the tests of
        //   the nightly build will fail.
        // --------------------------------------------------------------------

        ASSERT(bcema_Blob::notParanoid());
      } break;
      case 13: {
        // --------------------------------------------------------------------
        // TESTING IMPLICIT TRIM
        //
        // Concerns:
        //   DRQS 30331343 found a serious bug where 'appendDataBuffer' failed
        //   to trim 'd_totalSize', leading to corrupt datastructures and
        //   reads through invalid pointers.
        //
        // Plan:
        //   Repeat the test case from that DRQS, and also do some more
        //   thorough testing of 'appendDataBuffer'.  Call the method on blobs
        //   in a variety of different states, observing the state of the blob
        //   very closely.
        // --------------------------------------------------------------------

        if (verbose) cout << "TESTING IMPLICIT TRIM\n"
                             "=====================\n";

        bslma_TestAllocator ta;

        {
            // Kevin's McMahon's example from DRQS 30331343

//          bslma_Allocator *allocator = bslma_Default::allocator();
            bslma_Allocator *allocator = &ta;
//          bcema_PooledBlobBufferFactory factory(1024);
            SimpleBlobBufferFactory       factory(1024, allocator);
            bcema_Blob blob(&factory);
            blob.setLength(1);
            bcema_SharedPtr<char> buf((char*) allocator->allocate(4),
                                      allocator);
            bcema_BlobBuffer blobBuffer(buf, 4);
            blob.appendDataBuffer(blobBuffer);
            blob.setLength(blob.length() + 1);

            // with old code, blob fails invariants test upon destruction
        }

        SimpleBlobBufferFactory sbbf(1024, &ta);

        {
            bcema_Blob blob(&sbbf, &ta);
            blob.setLength(4);
            ASSERT(1024 == blob.buffer(0).size());
            ASSERT(1024 == blob.totalSize());
            ASSERT(4    == blob.length());
            for (int i = 0; i < 3; ++i) {
                bcema_BlobBuffer bb;
                sbbf.allocate(&bb);
                blob.appendBuffer(bb);
            }
            bcema_BlobBuffer bb;
            sbbf.allocate(&bb);
            blob.appendDataBuffer(bb);
            ASSERT(1028 == blob.length());
            LOOP_ASSERT(blob.totalSize(), 4100 == blob.totalSize());
            LOOP_ASSERT(blob.buffer(0).size(), 4 == blob.buffer(0).size());
            for (int i = 1; i < 5; ++i) {
                int sz = blob.buffer(i).size();
                LOOP2_ASSERT(i, sz, 1024 == sz);
            }
            ASSERT(5 == blob.numBuffers());
        }

        {
            bcema_Blob blob(&sbbf, &ta);
            for (int i = 0; i < 3; ++i) {
                bcema_BlobBuffer bb;
                sbbf.allocate(&bb);
                blob.appendBuffer(bb);
            }
            bcema_BlobBuffer bb;
            sbbf.allocate(&bb);
            blob.appendDataBuffer(bb);
            ASSERT(1024 == blob.length());
            LOOP_ASSERT(blob.totalSize(), 4096 == blob.totalSize());
            LOOP_ASSERT(blob.buffer(0).size(), 1024 == blob.buffer(0).size());
            for (int i = 1; i < 4; ++i) {
                int sz = blob.buffer(i).size();
                LOOP2_ASSERT(i, sz, 1024 == sz);
            }
            ASSERT(4 == blob.numBuffers());
            ASSERT(1024 == blob.length());
            ASSERT(4096 == blob.totalSize());
            ASSERT(1024 == blob.lastDataBufferLength());
        }

        {
            bcema_Blob blob(&sbbf, &ta);
            ASSERT(0 == blob.totalSize());
            ASSERT(0 == blob.numBuffers());
            bcema_BlobBuffer bb;
            sbbf.allocate(&bb);
            blob.appendDataBuffer(bb);
            ASSERT(1024 == blob.length());
            LOOP_ASSERT(blob.totalSize(), 1024 == blob.totalSize());
            LOOP_ASSERT(blob.buffer(0).size(), 1024 == blob.buffer(0).size());
            ASSERT(1 == blob.numBuffers());
            ASSERT(1024 == blob.totalSize());
            ASSERT(1024 == blob.lastDataBufferLength());
        }

        {
            bcema_Blob blob(&sbbf);
            blob.setLength(1);
            ASSERT(1024 == blob.totalSize());

            bcema_SharedPtr<char> buf((char*) ta.allocate(4), &ta);
            bcema_BlobBuffer blobBuffer(buf, 4);
            blob.appendDataBuffer(blobBuffer);
            blob.setLength(blob.length() + 1);
            ASSERT(3 == blob.numBuffers());
            ASSERT(1 == blob.buffer(0).size());
            ASSERT(4 == blob.buffer(1).size());
            ASSERT(1029 == blob.totalSize());
        }

        {
            bcema_Blob blob(&sbbf);
            blob.setLength(1025);
            ASSERT(2048 == blob.totalSize());
            blob.setLength(1024);
            ASSERT(2048 == blob.totalSize());
            ASSERT(1024 == blob.lastDataBufferLength());
            bcema_SharedPtr<char> buf((char*) ta.allocate(4), &ta);
            bcema_BlobBuffer blobBuffer(buf, 4);
            blob.appendDataBuffer(blobBuffer);
            blob.setLength(blob.length() + 1);
            LOOP_ASSERT(blob.numBuffers(), 3 == blob.numBuffers());
            ASSERT(1024 == blob.buffer(0).size());
            ASSERT(   4 == blob.buffer(1).size());
            ASSERT(1024 == blob.buffer(2).size());
            ASSERT(2052 == blob.totalSize());
        }
      } break;
      case 12: {
        // --------------------------------------------------------------------
        // TESTING CONCERN: BUFFER ALIASING
        //
        // Concerns:
        //   - That inserting the same buffer twice does not lead to double
        //     deletion.
        //   - That inserting buffers which are aliases does not violate length
        //     invariants.
        //
        // Plan:
        //   Create a blob with one buffer, and insert that buffer back into
        //   the same blob either at beginning or at end.  Check that length ,
        //   total size, memory buffers, are all as expected.  Assert that blob
        //   is destroyed accordingly, without double deletion.
        //
        // Testing:
        //   Concern: aliasing
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING CONCERN: Buffer aliasing" << endl
                          << "================================" << endl;

        bslma_TestAllocator defaultAlloc(veryVeryVerbose);
        bslma_DefaultAllocatorGuard guard(&defaultAlloc);
        bslma_TestAllocator ta(veryVeryVerbose);

        bslma_TestAllocator& testAllocator = ta;
        BEGIN_BSLMA_EXCEPTION_TEST
        {
            typedef bcema_Blob Obj;
            const int BUFFER_SIZE = 4;

            SimpleBlobBufferFactory fa(BUFFER_SIZE, &ta);

            Obj mX(&fa, &ta);   const Obj& X = mX;
            mX.setLength(BUFFER_SIZE - 1);
            ASSERT(BUFFER_SIZE - 1 == X.length());
            ASSERT(BUFFER_SIZE     == X.totalSize());
            ASSERT(BUFFER_SIZE     == X.buffer(0).size());
            if (verbose) {
                P_(X.length()); P_(X.totalSize()); P(X.buffer(0).size());
            }

            mX.insertBuffer(0, X.buffer(0));
            ASSERT(2 * BUFFER_SIZE - 1 == X.length());
            ASSERT(2 * BUFFER_SIZE     == X.totalSize());
            ASSERT(X.buffer(0).data()  == X.buffer(1).data());
            if (verbose) {
                P_(X.length()); P_(X.totalSize()); P_(X.buffer(0).size());
                P(X.buffer(1).size());
            }
            checkBlobBuffers(X);

            mX.trimLastDataBuffer();
            ASSERT(2 * BUFFER_SIZE - 1 == X.length());
            ASSERT(2 * BUFFER_SIZE - 1 == X.totalSize());
            ASSERT(BUFFER_SIZE         == X.buffer(0).size());
            ASSERT(BUFFER_SIZE - 1     == X.buffer(1).size());
            if (verbose) {
                P_(X.length()); P_(X.totalSize()); P_(X.buffer(0).size());
                P(X.buffer(1).size());
            }
            checkBlobBuffers(X);

#ifdef BDE_BUILD_TARGET_SAFE
            break;
#endif

            mX.appendDataBuffer(X.buffer(0));
            ASSERT(3 * BUFFER_SIZE - 1 == X.length());
            ASSERT(3 * BUFFER_SIZE - 1 == X.totalSize());
            ASSERT(BUFFER_SIZE         == X.buffer(0).size());
            ASSERT(BUFFER_SIZE - 1     == X.buffer(1).size());
            ASSERT(BUFFER_SIZE         == X.buffer(2).size());
            ASSERT(X.buffer(0).data()  == X.buffer(1).data());
            ASSERT(X.buffer(0).data()  == X.buffer(2).data());
            if (verbose) {
                P_(X.length()); P_(X.totalSize()); P_(X.buffer(0).size());
                P_(X.buffer(1).size()); P(X.buffer(2).size());
            }
            checkBlobBuffers(X);
        }
        ASSERT(0 <  ta.numAllocations());
        ASSERT(0 == ta.numBytesInUse());
        ASSERT(0 == ta.numMismatches());
        ASSERT(0 == defaultAlloc.numAllocations());

        END_BSLMA_EXCEPTION_TEST
      } break;
      case 11: {
        // --------------------------------------------------------------------
        // TESTING: moveDataBuffers
        //
        // Concerns:
        //   That 'moveDataBuffers' moves the data buffers of the 'srcBlob'
        //   blob to the 'dstBlob'.
        //
        // Plan:
        //
        // Testing:
        //   void moveDataBuffers(bcema_Blob *srcBlob);
        //   void moveAndAppendDataBuffers(bcema_Blob *srcBlob);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "moveDataBuffers & moveAndAppendDataBuffers"
                          << endl
                          << "=========================================="
                          << endl;

        bsl::string data1 = "abcdefghijklmnopqrstuvwzyz";
        bsl::string data2 = "01234567890`~!@#$%^&*()_-+";

        typedef bcema_Blob Obj;
        bslma_TestAllocator defaultAlloc(veryVeryVerbose);
        bslma_DefaultAllocatorGuard guard(&defaultAlloc);
        bslma_TestAllocator ta(veryVeryVerbose);

//      bslma_TestAllocator& testAllocator = ta;
        for (int bufferSize1 = 1; bufferSize1 <= 6; bufferSize1 += 2) {
        for (int numBuffers1 = 0; numBuffers1 <= 3; ++numBuffers1) {
        for (int dataLength1 = 0;
             dataLength1 <= bufferSize1 * numBuffers1;
             ++dataLength1) {
            const int BUFFER_SIZE1             = bufferSize1;
            const int NUM_BUFFERS1             = numBuffers1;
            const int TOTAL_SIZE1              = NUM_BUFFERS1 * BUFFER_SIZE1;
            const int DATA_LENGTH1             = dataLength1;
            const int NUM_DATA_BUFFERS1        =
                                               (dataLength1 + BUFFER_SIZE1 - 1)
                                                                / BUFFER_SIZE1;
            const int LAST_DATA_BUFFER_LENGTH1
                = 0 == DATA_LENGTH1
                ? 0
                : DATA_LENGTH1 - ((NUM_DATA_BUFFERS1 - 1) * BUFFER_SIZE1);

            if (veryVerbose) {
                T_(); P_(BUFFER_SIZE1); P_(NUM_BUFFERS1); P_(DATA_LENGTH1);
                P_(TOTAL_SIZE1) P_(NUM_DATA_BUFFERS1)
                P(LAST_DATA_BUFFER_LENGTH1)
            }

            SimpleBlobBufferFactory fa1(BUFFER_SIZE1, &ta);
            Obj mX(&fa1, &ta);  const Obj& X = mX;
            mX.setLength(TOTAL_SIZE1);  // set up capacity
            mX.setLength(DATA_LENGTH1);

            bsl::string data1Substr(data1, &ta);
            data1Substr.resize(DATA_LENGTH1);

            loadBlob(&mX, data1Substr);

            ASSERT(TOTAL_SIZE1              == X.totalSize());
            ASSERT(DATA_LENGTH1             == X.length());
            ASSERT(NUM_BUFFERS1             == X.numBuffers());
            ASSERT(NUM_DATA_BUFFERS1        == X.numDataBuffers());
            ASSERT(LAST_DATA_BUFFER_LENGTH1 == X.lastDataBufferLength());

        for (int bufferSize2 = 1; bufferSize2 <= 6; bufferSize2 += 2) {
        for (int numBuffers2 = 0; numBuffers2 <= 3; ++numBuffers2) {
        for (int dataLength2 = 0;
             dataLength2 <= bufferSize2 * numBuffers2;
             ++dataLength2) {
            const int BUFFER_SIZE2             = bufferSize2;
            const int NUM_BUFFERS2             = numBuffers2;
            const int TOTAL_SIZE2              = NUM_BUFFERS2 * BUFFER_SIZE2;
            const int DATA_LENGTH2             = dataLength2;
            const int NUM_DATA_BUFFERS2        =
                                               (dataLength2 + BUFFER_SIZE2 - 1)
                                                                / BUFFER_SIZE2;
            const int LAST_DATA_BUFFER_LENGTH2
                = 0 == DATA_LENGTH2
                ? 0
                : DATA_LENGTH2 - ((NUM_DATA_BUFFERS2 - 1) * BUFFER_SIZE2);

            const int APPEND_LAST_DATA_BUFFER_LENGTH
                = 0 == LAST_DATA_BUFFER_LENGTH1
                ? LAST_DATA_BUFFER_LENGTH2
                : LAST_DATA_BUFFER_LENGTH1;

            if (veryVerbose) {
                T_(); P_(BUFFER_SIZE2); P_(NUM_BUFFERS2); P_(DATA_LENGTH2);
                P_(TOTAL_SIZE2) P_(NUM_DATA_BUFFERS2)
                P(LAST_DATA_BUFFER_LENGTH2)
            }

            SimpleBlobBufferFactory fa2(BUFFER_SIZE2, &ta);
            Obj mZ1(&fa2, &ta);  const Obj& Z1 = mZ1;
            Obj mZ2(&fa2, &ta);  const Obj& Z2 = mZ2;
            mZ1.setLength(TOTAL_SIZE2);  // set up capacity
            mZ1.setLength(DATA_LENGTH2);
            mZ2.setLength(TOTAL_SIZE2);  // set up capacity
            mZ2.setLength(DATA_LENGTH2);

            bsl::string data2Substr(data2, &ta);
            data2Substr.resize(DATA_LENGTH2);
            bsl::string EXP_DATA(data2Substr, &ta);
            EXP_DATA += data1Substr;

            loadBlob(&mZ1, data2Substr);
            loadBlob(&mZ2, data2Substr);

            ASSERT(TOTAL_SIZE2              == Z1.totalSize());
            ASSERT(DATA_LENGTH2             == Z1.length());
            ASSERT(NUM_BUFFERS2             == Z1.numBuffers());
            ASSERT(NUM_DATA_BUFFERS2        == Z1.numDataBuffers());
            ASSERT(LAST_DATA_BUFFER_LENGTH2 == Z1.lastDataBufferLength());

            ASSERT(TOTAL_SIZE2              == Z2.totalSize());
            ASSERT(DATA_LENGTH2             == Z2.length());
            ASSERT(NUM_BUFFERS2             == Z2.numBuffers());
            ASSERT(NUM_DATA_BUFFERS2        == Z2.numDataBuffers());
            ASSERT(LAST_DATA_BUFFER_LENGTH2 == Z2.lastDataBufferLength());

            Obj mY1(X, &fa1, &ta); const Obj& Y1 = mY1;
            Obj mY2(X, &fa1, &ta); const Obj& Y2 = mY2;

            ASSERT(TOTAL_SIZE1              == Y1.totalSize());
            ASSERT(DATA_LENGTH1             == Y1.length());
            ASSERT(NUM_BUFFERS1             == Y1.numBuffers());
            ASSERT(NUM_DATA_BUFFERS1        == Y1.numDataBuffers());
            ASSERT(LAST_DATA_BUFFER_LENGTH1 == Y1.lastDataBufferLength());

            ASSERT(TOTAL_SIZE1              == Y2.totalSize());
            ASSERT(DATA_LENGTH1             == Y2.length());
            ASSERT(NUM_BUFFERS1             == Y2.numBuffers());
            ASSERT(NUM_DATA_BUFFERS1        == Y2.numDataBuffers());
            ASSERT(LAST_DATA_BUFFER_LENGTH1 == Y2.lastDataBufferLength());

            ASSERT(X  == Y1);
            ASSERT(X  == Y2);
            ASSERT(Y1 == Y2);
            if (DATA_LENGTH1 > 0 || DATA_LENGTH2 > 0) {
                ASSERT(X  != Z1);
                ASSERT(Y1 != Z1);
                ASSERT(Y2 != Z1);
                ASSERT(X  != Z2);
                ASSERT(Y1 != Z2);
                ASSERT(Y2 != Z2);
            }

//          const int TOTAL_TOTAL1 = Z1.totalSize() + Y1.totalSize();
//          const int TOTAL_TOTAL2 = Z2.totalSize() + Y2.totalSize();

            mZ1.moveDataBuffers(&mY1);
            mZ2.moveAndAppendDataBuffers(&mY2);

            if (veryVerbose) {
                P_(BUFFER_SIZE1) P_(NUM_BUFFERS1) P_(DATA_LENGTH1)
                    P_(TOTAL_SIZE1) P_(NUM_DATA_BUFFERS1)
                    P_(Y2.lastDataBufferLength())
                P_(BUFFER_SIZE2) P_(NUM_BUFFERS2) P_(DATA_LENGTH2)
                P_(TOTAL_SIZE2) P_(NUM_DATA_BUFFERS2)
                    P_(Z2.lastDataBufferLength())
            }

            ASSERT(NUM_BUFFERS1 - NUM_DATA_BUFFERS1 == Y1.numBuffers());
            ASSERT(Y1.numBuffers() * BUFFER_SIZE1   == Y1.totalSize());
            ASSERT(checkTotalSize(Y1));
            ASSERT(0                             == Y1.length());
            ASSERT(0                             == Y1.numDataBuffers());
            ASSERT(0                             == Y1.lastDataBufferLength());

            ASSERT(NUM_BUFFERS1 - NUM_DATA_BUFFERS1 == Y2.numBuffers());
            ASSERT(checkTotalSize(Y2));
            ASSERT(Y2.numBuffers() * BUFFER_SIZE1   == Y2.totalSize());
            ASSERT(0                             == Y2.length());
            ASSERT(0                             == Y2.numDataBuffers());
            ASSERT(0                             == Y2.lastDataBufferLength());

            bsl::string z1Data(&ta);
            blobToStr(&z1Data, Z1);
            ASSERT(checkTotalSize(Z1));
            ASSERT(DATA_LENGTH1                  == Z1.length());
            if (0 == Z1.length()) {
                LOOP2_ASSERT(NUM_DATA_BUFFERS1, Z1.numBuffers(),
                         NUM_BUFFERS2 + NUM_DATA_BUFFERS1 == Z1.numBuffers());
            }
            else {
                LOOP2_ASSERT(NUM_DATA_BUFFERS1, Z1.numBuffers(),
                             NUM_DATA_BUFFERS1 == Z1.numBuffers());
            }

            LOOP2_ASSERT(NUM_DATA_BUFFERS1, Z1.numDataBuffers(),
                         NUM_DATA_BUFFERS1 == Z1.numDataBuffers());
            LOOP4_ASSERT(DATA_LENGTH1, DATA_LENGTH2,
                           LAST_DATA_BUFFER_LENGTH1, Z1.lastDataBufferLength(),
                        LAST_DATA_BUFFER_LENGTH1 == Z1.lastDataBufferLength());
            LOOP2_ASSERT(data1Substr, z1Data, data1Substr == z1Data);

            const int EXP_NUM_BUFFERS = bsl::max(
                                        NUM_BUFFERS2,
                                        NUM_DATA_BUFFERS1 + NUM_BUFFERS2);
            bsl::string z2Data(&ta);
            blobToStr(&z2Data, Z2);

            ASSERT(checkTotalSize(Z2));
            ASSERT(DATA_LENGTH1 + DATA_LENGTH2           == Z2.length());
            LOOP2_ASSERT(EXP_NUM_BUFFERS, Z2.numBuffers(),
                         EXP_NUM_BUFFERS == Z2.numBuffers());
            LOOP2_ASSERT(NUM_DATA_BUFFERS1 + NUM_DATA_BUFFERS2,
                         Z2.numDataBuffers(),
                         NUM_DATA_BUFFERS1 + NUM_DATA_BUFFERS2
                                                       == Z2.numDataBuffers());
            LOOP4_ASSERT(DATA_LENGTH1, DATA_LENGTH2,
                           LAST_DATA_BUFFER_LENGTH1, Z2.lastDataBufferLength(),
                  APPEND_LAST_DATA_BUFFER_LENGTH == Z2.lastDataBufferLength());
            LOOP2_ASSERT(EXP_DATA, z2Data, EXP_DATA == z2Data);
        }
        }
        }
        }
        }
        }
        ASSERT(0 <  ta.numAllocations());
        ASSERT(0 == ta.numBytesInUse());
        ASSERT(0 == ta.numMismatches());
        ASSERT(0 == defaultAlloc.numAllocations());
      } break;
      case 10: {
        // --------------------------------------------------------------------
        // TESTING 'swapBufferRaw'
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        //   bcema_Blob::swapBufferRaw(int index, BlobBuffer *src);
        // --------------------------------------------------------------------

        if (verbose)
            cout << endl
                 << "TESTING 'swapBufferRaw'" << endl
                 << "=======================" << endl;

        if (verbose) cout << "\nTesting all blobs with fixed buffer size.\n";

        for (int bufferSize = 1; bufferSize <= 5; ++bufferSize)
        for (int numBuffers = 1; numBuffers <= 5; ++numBuffers)
        for (int dataLength = 1;
             dataLength <= bufferSize * numBuffers;
             ++dataLength)
        {
            bslma_TestAllocator defaultAlloc(veryVeryVerbose);
            bslma_DefaultAllocatorGuard guard(&defaultAlloc);
            bslma_TestAllocator ta(veryVeryVerbose);

            bslma_TestAllocator& testAllocator = ta;
            BEGIN_BSLMA_EXCEPTION_TEST
            {
                const int BUFFER_SIZE     = bufferSize;
                const int LENGTH          = dataLength;
                const int NUM_BUFFERS     = numBuffers;
                const int EXP_LENGTH      = dataLength;
                const int EXP_TOTAL_SIZE  = NUM_BUFFERS * BUFFER_SIZE;
                const int EXP_NUM_BUFFERS = NUM_BUFFERS;

                if (veryVerbose) {
                    T_(); P_(BUFFER_SIZE); P_(LENGTH); P(NUM_BUFFERS);
                    T_(); P_(EXP_LENGTH);  P(EXP_NUM_BUFFERS);
                }

                typedef bcema_Blob Obj;
                SimpleBlobBufferFactory fa(BUFFER_SIZE, &ta);

                Obj mX(&fa, &ta);   const Obj& X = mX;

                mX.setLength(EXP_TOTAL_SIZE);  // set up capacity
                mX.setLength(EXP_LENGTH);
                for (int i = 0; i < numBuffers; ++i) {
                    bcema_BlobBuffer src;
                    fa.allocate(&src);
                    ASSERT(BUFFER_SIZE == src.size());

                    const char srcChar = 'A' + i;
                    bsl::memset(src.data(), srcChar, src.size());
                    ASSERT(compareBlobBufferData(src, srcChar));

                    const char dstChar = 'Z';
                    populateBuffersWithData(&mX, numBuffers, dstChar);
                    ASSERT(compareBuffersData(X, numBuffers, dstChar));

                    mX.swapBufferRaw(i, &src);
                    ASSERT(EXP_LENGTH      == X.length());
                    ASSERT(EXP_TOTAL_SIZE  == X.totalSize());
                    ASSERT(EXP_NUM_BUFFERS == X.numBuffers());
                    ASSERT(BUFFER_SIZE     == X.buffer(i).size());
                    ASSERT(BUFFER_SIZE     == src.size());
                    ASSERT(compareBuffersData(X, numBuffers, dstChar, i));
                    ASSERT(compareBlobBufferData(X.buffer(i), srcChar));
                    ASSERT(compareBlobBufferData(src, dstChar));
                }
            }
            ASSERT(0 <  ta.numAllocations());
            ASSERT(0 == ta.numBytesInUse());
            ASSERT(0 == ta.numMismatches());
            ASSERT(0 == defaultAlloc.numAllocations());

            END_BSLMA_EXCEPTION_TEST
        }
      } break;
      case 9: {
        // --------------------------------------------------------------------
        // TESTING: moveBuffers
        //
        // Concerns:
        //   That 'moveBuffers' assigns the value of the 'rhs' blob, and then
        //   removes all of its buffers.
        //
        // Plan:
        //
        // Testing:
        //   void moveBuffers(bcema_Blob *srcBlob);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING: moveBuffers" << endl
                          << "====================" << endl;

        typedef bcema_Blob Obj;
        bslma_TestAllocator defaultAlloc(veryVeryVerbose);
        bslma_DefaultAllocatorGuard guard(&defaultAlloc);
        bslma_TestAllocator ta(veryVeryVerbose);

        bslma_TestAllocator& testAllocator = ta;
        BEGIN_BSLMA_EXCEPTION_TEST {
        for (int bufferSize = 1; bufferSize < 20; bufferSize +=2) {
        for (int numBuffers = 0; numBuffers < 10; ++numBuffers) {
            const int BUFFER_SIZE = bufferSize;
            const int NUM_BUFFERS = numBuffers;
            if (veryVerbose) {
                T_(); P_(BUFFER_SIZE); P(NUM_BUFFERS);
            }

            SimpleBlobBufferFactory fa(BUFFER_SIZE, &ta);
            Obj mX(&fa, &ta);  const Obj& X = mX;
            mX.setLength(NUM_BUFFERS * BUFFER_SIZE);

            ASSERT(NUM_BUFFERS               == X.numBuffers());
            ASSERT(NUM_BUFFERS * BUFFER_SIZE == X.length());

            Obj mY(X, &fa, &ta); const Obj& Y = mY;

            ASSERT(X                         == Y );
            ASSERT(NUM_BUFFERS               == Y.numBuffers());
            ASSERT(NUM_BUFFERS * BUFFER_SIZE == Y.length());

            Obj mZ(&fa, &ta); const Obj& Z = mZ;

            // X & Y should have the test value V0, and Z should have the
            // default initialized value.
            if (NUM_BUFFERS == 0) {
                ASSERT(Y == Z);
                ASSERT(X == Z);
            }
            else {
                ASSERT(Y != Z);
                ASSERT(X != Z);
            }
            ASSERT(0 == Z.numBuffers());
            ASSERT(0 == Z.length());

            // X & Z should now have the test value V0, and Y should have the
            // default initialized value.
            mZ.moveBuffers(&mY);
            if (NUM_BUFFERS == 0) {
                ASSERT(Y == Z);
                ASSERT(X == Z);
            }
            else {
                ASSERT(Y != Z);
                ASSERT(X == Z);
            }
            ASSERT(0 == Y.numBuffers());
            ASSERT(0 == Y.length());
            ASSERT(NUM_BUFFERS               == Z.numBuffers());
            ASSERT(NUM_BUFFERS * BUFFER_SIZE == Z.length());
        }
        }
        ASSERT(0 <  ta.numAllocations());
        ASSERT(0 == ta.numBytesInUse());
        ASSERT(0 == ta.numMismatches());
        ASSERT(0 == defaultAlloc.numAllocations());
        } END_BSLMA_EXCEPTION_TEST
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // TESTING 'prependDataBuffer' and 'appendDataBuffer'
        //
        // Concerns:
        //   - That pre/appending at the end of a blob
        //     must always increase the length of the blob.
        //   - That the component is exception neutral.
        //
        // Plan:
        //   For all combinations of length, buffer size (for the factory), and
        //   number of buffers, check that a blob constructed to the proper
        //   parameters has the expected characteristics and that, after
        //   prepending and appending data buffers, these characteristics are
        //   changed as expected (length increases by buffer size, last data
        //   buffer length is changed to buffer size for append, and is changed
        //   only if the data length was zero before prepend).  In all cases,
        //   run the test in a bdema exception test loop to test for exception
        //   neutrality (invariants are asserted in the blob destructor upon
        //   throwing the exception).
        //
        // Testing:
        //   bcema_Blob::prependDataBuffer(bcema_BlobBuffer);
        //   bcema_Blob::appendDataBuffer(bcema_BlobBuffer);
        //   bcema_Blob::setLength(int) in the presence of 0-sized buffers
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING '{pre/ap}pendDataBuffer" << endl
                          << "===============================" << endl;

        if (verbose) cout << "\nTesting 'prependDataBuffer'" << endl;

        for (int bufferSize = 1; bufferSize <= 5; ++bufferSize)
        for (int numBuffers = 0; numBuffers <= 5; ++numBuffers)
        for (int dataLength = 0; dataLength <= bufferSize * numBuffers;
                                                  ++dataLength)
        for (int prependSz   = 1; prependSz   <= 5; ++prependSz)
        {
            bslma_TestAllocator defaultAlloc(veryVeryVerbose);
            bslma_DefaultAllocatorGuard guard(&defaultAlloc);
            bslma_TestAllocator ta(veryVeryVerbose);

            bslma_TestAllocator& testAllocator = ta;
            BEGIN_BSLMA_EXCEPTION_TEST
            {
                const int BUFFER_SIZE          = bufferSize;
                const int DATA_LENGTH          = dataLength;
                const int NUM_BUFFERS          = numBuffers;
                const int PREPEND_BUFFER_SIZE   = prependSz;
                const int EXP_NUM_DATA_BUFFERS = (dataLength + BUFFER_SIZE - 1)
                                               / BUFFER_SIZE;
                const int EXP_NUM_BUFFERS      = NUM_BUFFERS;
                const int EXP_LAST_DB_LENGTH   = EXP_NUM_DATA_BUFFERS > 0
                       ? DATA_LENGTH - (EXP_NUM_DATA_BUFFERS - 1) * BUFFER_SIZE
                       : 0;
                if (veryVerbose) {
                    T_(); P_(BUFFER_SIZE); P_(DATA_LENGTH); P(NUM_BUFFERS);
                    T_(); P_(EXP_NUM_DATA_BUFFERS); P_(EXP_NUM_BUFFERS);
                                                         P(EXP_LAST_DB_LENGTH);
                }

                typedef bcema_Blob Obj;
                SimpleBlobBufferFactory fa(BUFFER_SIZE, &ta);

                Obj mX(&fa, &ta);   const Obj& X = mX;
                mX.setLength(BUFFER_SIZE * NUM_BUFFERS);

                mX.setLength(DATA_LENGTH);
                ASSERT(DATA_LENGTH          == X.length());
                ASSERT(EXP_NUM_BUFFERS      == X.numBuffers());
                ASSERT(EXP_NUM_DATA_BUFFERS == X.numDataBuffers());
                ASSERT(EXP_LAST_DB_LENGTH   == X.lastDataBufferLength());

                SimpleBlobBufferFactory ifa(PREPEND_BUFFER_SIZE, &ta);
                bcema_BlobBuffer buffer;
                ifa.allocate(&buffer);
                ASSERT(PREPEND_BUFFER_SIZE == buffer.size());

                mX.prependDataBuffer(buffer); // TEST HERE

                ASSERT(DATA_LENGTH + PREPEND_BUFFER_SIZE == X.length());
                if (0 < DATA_LENGTH) {
                    ASSERT(EXP_NUM_DATA_BUFFERS + 1 == X.numDataBuffers());
                    ASSERT(EXP_LAST_DB_LENGTH == X.lastDataBufferLength());
                } else {
                    if (0 < PREPEND_BUFFER_SIZE) {
                        ASSERT(EXP_NUM_DATA_BUFFERS + 1 == X.numDataBuffers());
                    } else {
                        ASSERT(EXP_NUM_DATA_BUFFERS == X.numDataBuffers());
                    }
                    ASSERT(PREPEND_BUFFER_SIZE == X.lastDataBufferLength());
                }
                ASSERT(EXP_NUM_BUFFERS + 1 == X.numBuffers());

                // Repeat invariants after testing setLength in the presence
                // of zero-sized buffers.

                mX.setLength(0);
                mX.setLength(DATA_LENGTH + PREPEND_BUFFER_SIZE);
                ASSERT(DATA_LENGTH + PREPEND_BUFFER_SIZE == X.length());
                if (0 < DATA_LENGTH) {
                    ASSERT(EXP_NUM_DATA_BUFFERS + 1 == X.numDataBuffers());
                    ASSERT(EXP_LAST_DB_LENGTH == X.lastDataBufferLength());
                } else {
                    if (0 < PREPEND_BUFFER_SIZE) {
                        ASSERT(EXP_NUM_DATA_BUFFERS + 1 == X.numDataBuffers());
                    } else {
                        ASSERT(EXP_NUM_DATA_BUFFERS == X.numDataBuffers());
                    }
                    ASSERT(PREPEND_BUFFER_SIZE == X.lastDataBufferLength());
                }
                ASSERT(EXP_NUM_BUFFERS + 1 == X.numBuffers());

                checkNoAliasedBlobBuffers(X);
            }
            ASSERT(0 <  ta.numAllocations());
            ASSERT(0 == ta.numBytesInUse());
            ASSERT(0 == ta.numMismatches());
            ASSERT(0 == defaultAlloc.numAllocations());

            END_BSLMA_EXCEPTION_TEST
        }

#ifdef  BDE_BUILD_TARGET_SAFE
        break;
#endif

        if (verbose) cout << "\nTesting 'appendDataBuffer'" << endl;

        for (int bufferSize = 1; bufferSize <= 5; ++bufferSize)
        for (int numBuffers = 0; numBuffers <= 5; ++numBuffers)
        for (int dataLength = 0; dataLength <= bufferSize * numBuffers;
                                                  ++dataLength)
        for (int appendSz   = 1; appendSz   <= 5; ++appendSz)
        {
            bslma_TestAllocator defaultAlloc(veryVeryVerbose);
            bslma_DefaultAllocatorGuard guard(&defaultAlloc);
            bslma_TestAllocator ta(veryVeryVerbose);

            bslma_TestAllocator& testAllocator = ta;
            BEGIN_BSLMA_EXCEPTION_TEST
            {
                const int BUFFER_SIZE          = bufferSize;
                const int DATA_LENGTH          = dataLength;
                const int NUM_BUFFERS          = numBuffers;
                const int APPEND_BUFFER_SIZE   = appendSz;
                const int EXP_NUM_DATA_BUFFERS = (dataLength + BUFFER_SIZE - 1)
                                               / BUFFER_SIZE;
                const int EXP_NUM_BUFFERS      = NUM_BUFFERS;
                const int EXP_LAST_DB_LENGTH   = EXP_NUM_DATA_BUFFERS > 0
                       ? DATA_LENGTH - (EXP_NUM_DATA_BUFFERS - 1) * BUFFER_SIZE
                       : 0;
                if (veryVerbose) {
                    T_(); P_(BUFFER_SIZE); P_(DATA_LENGTH); P(NUM_BUFFERS);
                    T_(); P_(EXP_NUM_DATA_BUFFERS); P_(EXP_NUM_BUFFERS);
                                                         P(EXP_LAST_DB_LENGTH);
                }

                typedef bcema_Blob Obj;
                SimpleBlobBufferFactory fa(BUFFER_SIZE, &ta);

                Obj mX(&fa, &ta);   const Obj& X = mX;
                mX.setLength(BUFFER_SIZE * NUM_BUFFERS);

                mX.setLength(DATA_LENGTH);
                ASSERT(DATA_LENGTH          == X.length());
                ASSERT(EXP_NUM_BUFFERS      == X.numBuffers());
                ASSERT(EXP_NUM_DATA_BUFFERS == X.numDataBuffers());
                ASSERT(EXP_LAST_DB_LENGTH   == X.lastDataBufferLength());

                SimpleBlobBufferFactory ifa(APPEND_BUFFER_SIZE, &ta);
                bcema_BlobBuffer buffer;
                ifa.allocate(&buffer);
                ASSERT(APPEND_BUFFER_SIZE == buffer.size());

                mX.appendDataBuffer(buffer); // TEST HERE

                ASSERT(DATA_LENGTH + APPEND_BUFFER_SIZE == X.length());
                ASSERT(EXP_NUM_BUFFERS + 1 == X.numBuffers());
                if (0 < APPEND_BUFFER_SIZE) {
                    ASSERT(EXP_NUM_DATA_BUFFERS + 1 == X.numDataBuffers());
                    ASSERT(APPEND_BUFFER_SIZE   == X.lastDataBufferLength());
                } else {
                    ASSERT(EXP_NUM_DATA_BUFFERS == X.numDataBuffers());
                    ASSERT(EXP_LAST_DB_LENGTH   == X.lastDataBufferLength());
                }

                checkNoAliasedBlobBuffers(X);
            }
            ASSERT(0 <  ta.numAllocations());
            ASSERT(0 == ta.numBytesInUse());
            ASSERT(0 == ta.numMismatches());
            ASSERT(0 == defaultAlloc.numAllocations());

            END_BSLMA_EXCEPTION_TEST
        }

      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING 'removeBuffer' AND 'removeAll'
        //
        // Concerns:
        //   - That removing a non data buffer must not decrease the length of
        //     the blob.
        //   - That removing a data buffer must decrease the length of the
        //     blob.
        //   - That the component is exception neutral.
        //
        // Plan:
        //   For all combinations of length, buffer size (for the factory), and
        //   number of buffers, check that a blob constructed to the proper
        //   parameters has the expected characteristics and that, after
        //   removing a buffer from all possible positions, these
        //   characteristics are changed as expected.  For the
        //   same combinations, construct a blob and remove all its buffers,
        //   and check that it does deallocate all its buffers.  In all cases,
        //   run the test in a bdema exception test loop to test for exception
        //   neutrality (invariants are asserted in the blob destructor upon
        //   throwing the exception).
        //
        // Testing:
        //   bcema_Blob::removeBuffer(int);
        //   bcema_Blob::removeAll();
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING 'remove*'" << endl
                          << "=================" << endl;

        if (verbose) cout << "ntTesting 'removeBuffer'" << endl;

        for (int bufferSize = 1; bufferSize <= 5; ++bufferSize)
        for (int numBuffers = 0; numBuffers <= 5; ++numBuffers)
        for (int dataLength = 0; dataLength <= bufferSize * numBuffers;
                                                  ++dataLength)
        for (int removePos  = 0; removePos < numBuffers; ++removePos)
        {
            bslma_TestAllocator defaultAlloc(veryVeryVerbose);
            bslma_DefaultAllocatorGuard guard(&defaultAlloc);
            bslma_TestAllocator ta(veryVeryVerbose);

            bslma_TestAllocator& testAllocator = ta;
            BEGIN_BSLMA_EXCEPTION_TEST
            {
                const int BUFFER_SIZE          = bufferSize;
                const int DATA_LENGTH          = dataLength;
                const int NUM_BUFFERS          = numBuffers;
                const int REMOVE_POSITION      = removePos;
                const int EXP_NUM_DATA_BUFFERS = (dataLength + BUFFER_SIZE - 1)
                                               / BUFFER_SIZE;
                const int EXP_NUM_BUFFERS      = NUM_BUFFERS;
                const int EXP_LAST_DB_LENGTH   = EXP_NUM_DATA_BUFFERS > 0
                       ? DATA_LENGTH - (EXP_NUM_DATA_BUFFERS - 1) * BUFFER_SIZE
                       : 0;
                if (veryVerbose) {
                    T_(); P_(BUFFER_SIZE); P_(DATA_LENGTH); P_(NUM_BUFFERS);
                          P(REMOVE_POSITION);
                    T_(); P_(EXP_NUM_DATA_BUFFERS); P_(EXP_NUM_BUFFERS);
                          P(EXP_LAST_DB_LENGTH);
                }

                typedef bcema_Blob Obj;
                SimpleBlobBufferFactory fa(BUFFER_SIZE, &ta);

                Obj mX(&fa, &ta);   const Obj& X = mX;
                mX.setLength(BUFFER_SIZE * NUM_BUFFERS);

                mX.setLength(DATA_LENGTH);
                ASSERT(DATA_LENGTH          == X.length());
                ASSERT(EXP_NUM_BUFFERS      == X.numBuffers());
                ASSERT(EXP_NUM_DATA_BUFFERS == X.numDataBuffers());
                ASSERT(EXP_LAST_DB_LENGTH   == X.lastDataBufferLength());
                checkNoAliasedBlobBuffers(X);

                mX.removeBuffer(REMOVE_POSITION);  // TEST HERE

                ASSERT(EXP_NUM_BUFFERS - 1 == X.numBuffers());
                if (REMOVE_POSITION == EXP_NUM_DATA_BUFFERS - 1) {
                    // Removing the last data buffer (variable number of data
                    // bytes, equal to EXP_LAST_DB_LENGTH).
                    if (0 < REMOVE_POSITION) {
                        ASSERT(BUFFER_SIZE == X.lastDataBufferLength());
                    } else {
                        ASSERT(0           == X.lastDataBufferLength());
                    }
                    ASSERT(DATA_LENGTH - EXP_LAST_DB_LENGTH == X.length());
                    ASSERT(EXP_NUM_DATA_BUFFERS - 1 == X.numDataBuffers());
                } else if (REMOVE_POSITION < EXP_NUM_DATA_BUFFERS) {
                    // Removing a data buffer.
                    ASSERT(EXP_LAST_DB_LENGTH == X.lastDataBufferLength());
                    ASSERT(DATA_LENGTH - BUFFER_SIZE == X.length());
                    ASSERT(EXP_NUM_DATA_BUFFERS - 1 == X.numDataBuffers());
                } else {
                    // Removing a capacity buffer.
                    ASSERT(EXP_LAST_DB_LENGTH == X.lastDataBufferLength());
                    ASSERT(DATA_LENGTH == X.length());
                    ASSERT(EXP_NUM_DATA_BUFFERS == X.numDataBuffers());
                }

                checkNoAliasedBlobBuffers(X);
            }
            ASSERT(0 <  ta.numAllocations());
            ASSERT(0 == ta.numBytesInUse());
            ASSERT(0 == ta.numMismatches());
            ASSERT(0 == defaultAlloc.numAllocations());

            END_BSLMA_EXCEPTION_TEST
        }

        if (verbose) cout << "\nTesting 'removeAll'" << endl;

        for (int bufferSize = 1; bufferSize <= 5; ++bufferSize)
        for (int numBuffers = 0; numBuffers <= 5; ++numBuffers)
        for (int dataLength = 0;
             dataLength <= bufferSize * numBuffers; ++dataLength)
        {
            bslma_TestAllocator defaultAlloc(veryVeryVerbose);
            bslma_DefaultAllocatorGuard guard(&defaultAlloc);
            bslma_TestAllocator ta(veryVeryVerbose);

            bslma_TestAllocator& testAllocator = ta;
            BEGIN_BSLMA_EXCEPTION_TEST
            {
                const int BUFFER_SIZE          = bufferSize;
                const int DATA_LENGTH          = dataLength;
                const int NUM_BUFFERS          = numBuffers;
                const int EXP_NUM_DATA_BUFFERS = (dataLength + BUFFER_SIZE - 1)
                                               / BUFFER_SIZE;
                const int EXP_NUM_BUFFERS      = NUM_BUFFERS;
                const int EXP_LAST_DB_LENGTH   = EXP_NUM_DATA_BUFFERS > 0
                       ? DATA_LENGTH - (EXP_NUM_DATA_BUFFERS - 1) * BUFFER_SIZE
                       : 0;
                if (veryVerbose) {
                    T_(); P_(BUFFER_SIZE); P_(DATA_LENGTH); P(NUM_BUFFERS);
                    T_(); P_(EXP_NUM_DATA_BUFFERS); P_(EXP_NUM_BUFFERS);
                                                         P(EXP_LAST_DB_LENGTH);
                }

                typedef bcema_Blob Obj;
                SimpleBlobBufferFactory fa(BUFFER_SIZE, &ta);

                Obj mX(&fa, &ta);   const Obj& X = mX;
                mX.setLength(BUFFER_SIZE * NUM_BUFFERS);

                mX.setLength(DATA_LENGTH);
                ASSERT(DATA_LENGTH          == X.length());
                ASSERT(EXP_NUM_BUFFERS      == X.numBuffers());
                ASSERT(EXP_NUM_DATA_BUFFERS == X.numDataBuffers());
                ASSERT(EXP_LAST_DB_LENGTH   == X.lastDataBufferLength());

                mX.removeAll(); // TEST HERE

                ASSERT(0 == X.length());
                ASSERT(0 == X.numBuffers());
                ASSERT(0 == X.numDataBuffers());
                ASSERT(0 == X.lastDataBufferLength());
            }
            ASSERT(0 == dataLength || 0 <  ta.numAllocations());
            ASSERT(0 == ta.numBytesInUse());
            ASSERT(0 == ta.numMismatches());
            ASSERT(0 == defaultAlloc.numAllocations());

            END_BSLMA_EXCEPTION_TEST
        }

      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING APPEND
        //
        // Concerns:
        //   - That appending at the end of a blob must not increase the length
        //     of the blob.
        //   - That appending an empty buffer does not change the blob except
        //     for the number of buffers.
        //   - That the component is exception neutral.
        //
        // Plan:
        //   For all combinations of length, buffer size (for the factory), and
        //   number of buffers, check that a blob constructed to the proper
        //   parameters has the expected characteristics and that, after
        //   inserted a data buffer in all possible positions, these
        //   characteristics are changed as expected (incrementing the number
        //   of data buffers and length when the position is before the end of
        //   the blob, and leaving them unchanged if not).  In all cases, run
        //   the test in a bdema exception test loop to test for exception
        //   neutrality (invariants are asserted in the blob destructor upon
        //   throwing the exception).
        //
        // Testing:
        //   bcema_Blob::appendBuffer(bcema_BlobBuffer);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING 'appendBuffer'" << endl
                          << "======================" << endl;

        for (int bufferSize = 1; bufferSize <= 5; ++bufferSize)
        for (int numBuffers = 0; numBuffers <= 5; ++numBuffers)
        for (int dataLength = 0; dataLength <= bufferSize * numBuffers;
                                                  ++dataLength)
        for (int appendSz   = 0; appendSz   <= 5; ++appendSz)
        {
            bslma_TestAllocator defaultAlloc(veryVeryVerbose);
            bslma_DefaultAllocatorGuard guard(&defaultAlloc);
            bslma_TestAllocator ta(veryVeryVerbose);

            bslma_TestAllocator& testAllocator = ta;
            BEGIN_BSLMA_EXCEPTION_TEST
            {
                const int BUFFER_SIZE          = bufferSize;
                const int DATA_LENGTH          = dataLength;
                const int NUM_BUFFERS          = numBuffers;
                const int APPEND_BUFFER_SIZE   = appendSz;
                const int EXP_NUM_DATA_BUFFERS = (dataLength + BUFFER_SIZE - 1)
                                               / BUFFER_SIZE;
                const int EXP_NUM_BUFFERS      = NUM_BUFFERS;
                const int EXP_LAST_DB_LENGTH   = EXP_NUM_DATA_BUFFERS > 0
                       ? DATA_LENGTH - (EXP_NUM_DATA_BUFFERS - 1) * BUFFER_SIZE
                       : 0;
                if (veryVerbose) {
                    T_(); P_(BUFFER_SIZE); P_(DATA_LENGTH); P_(NUM_BUFFERS);
                    T_(); P_(EXP_NUM_DATA_BUFFERS); P_(EXP_NUM_BUFFERS);
                                                         P(EXP_LAST_DB_LENGTH);
                }

                typedef bcema_Blob Obj;
                SimpleBlobBufferFactory fa(BUFFER_SIZE, &ta);

                Obj mX(&fa, &ta);   const Obj& X = mX;
                mX.setLength(BUFFER_SIZE * NUM_BUFFERS);

                mX.setLength(DATA_LENGTH);
                ASSERT(DATA_LENGTH          == X.length());
                ASSERT(EXP_NUM_BUFFERS      == X.numBuffers());
                ASSERT(EXP_NUM_DATA_BUFFERS == X.numDataBuffers());
                ASSERT(EXP_LAST_DB_LENGTH   == X.lastDataBufferLength());

                SimpleBlobBufferFactory ifa(APPEND_BUFFER_SIZE, &ta);
                bcema_BlobBuffer buffer;
                ifa.allocate(&buffer);
                ASSERT(APPEND_BUFFER_SIZE == buffer.size());

                mX.appendBuffer(buffer); // TEST HERE

                ASSERT(DATA_LENGTH          == X.length());
                ASSERT(EXP_LAST_DB_LENGTH   == X.lastDataBufferLength());
                ASSERT(EXP_NUM_BUFFERS + 1  == X.numBuffers());
                ASSERT(EXP_NUM_DATA_BUFFERS == X.numDataBuffers());

                checkNoAliasedBlobBuffers(X);
            }
            ASSERT(0 <  ta.numAllocations());
            ASSERT(0 == ta.numBytesInUse());
            ASSERT(0 == ta.numMismatches());
            ASSERT(0 == defaultAlloc.numAllocations());

            END_BSLMA_EXCEPTION_TEST
        }

      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING INSERT
        //
        // Concerns:
        //   - That inserting at the end of a blob (or beginning of an empty
        //     blob) must not increase the length of the blob.
        //   - That inserting inside a non-empty blob must increase the length
        //     of the blob.
        //   - That inserting empty buffers does not affect the blob except for
        //     the number of buffers.
        //   - That the component is exception neutral.
        //
        // Plan:
        //   For all combinations of length, buffer size (for the factory), and
        //   number of buffers, check that a blob constructed to the proper
        //   parameters has the expected characteristics and that, after
        //   inserted a data buffer in all possible positions, these
        //   characteristics are changed as expected (incrementing the number
        //   of data buffers and length when the position is before the end of
        //   the blob, and leaving them unchanged if not).  For the case that
        //   we insert a zero-sized buffer, we make sure that setting length to
        //   0 then back to original length maintains the same state.  In all
        //   cases, run the test in a bdema exception test loop to test for
        //   exception neutrality (invariants are asserted in the blob
        //   destructor upon throwing the exception).
        //
        // Testing:
        //   bcema_Blob::insertBuffer(int, bcema_BlobBuffer);
        //   bcema_Blob::setLength(int) in the presence of 0-sized buffers
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING 'insertBuffer'" << endl
                          << "======================" << endl;

        for (int bufferSize = 1; bufferSize <= 5; ++bufferSize)
        for (int numBuffers = 0; numBuffers <= 5; ++numBuffers)
        for (int dataLength = 0; dataLength <= bufferSize * numBuffers;
                                                  ++dataLength)
        for (int insertPos  = 0; insertPos  <= numBuffers; ++insertPos)
        for (int insertSz   = 0; insertSz   <= 5; ++insertSz)
        {
            bslma_TestAllocator defaultAlloc(veryVeryVerbose);
            bslma_DefaultAllocatorGuard guard(&defaultAlloc);
            bslma_TestAllocator ta(veryVeryVerbose);

            bslma_TestAllocator& testAllocator = ta;
            BEGIN_BSLMA_EXCEPTION_TEST
            {
                const int BUFFER_SIZE          = bufferSize;
                const int DATA_LENGTH          = dataLength;
                const int NUM_BUFFERS          = numBuffers;
                const int INSERT_POSITION      = insertPos;
                const int INSERT_BUFFER_SIZE   = insertSz;
                const int EXP_NUM_DATA_BUFFERS = (dataLength + BUFFER_SIZE - 1)
                                               / BUFFER_SIZE;
                const int EXP_NUM_BUFFERS      = NUM_BUFFERS;
                const int EXP_LAST_DB_LENGTH   = EXP_NUM_DATA_BUFFERS > 0
                       ? DATA_LENGTH - (EXP_NUM_DATA_BUFFERS - 1) * BUFFER_SIZE
                       : 0;
                if (veryVerbose) {
                    T_(); P_(BUFFER_SIZE); P_(DATA_LENGTH); P_(NUM_BUFFERS);
                          P(INSERT_POSITION)
                    T_(); P_(EXP_NUM_DATA_BUFFERS); P_(EXP_NUM_BUFFERS);
                          P(EXP_LAST_DB_LENGTH);
                }

                typedef bcema_Blob Obj;
                SimpleBlobBufferFactory fa(BUFFER_SIZE, &ta);

                Obj mX(&fa, &ta);   const Obj& X = mX;
                mX.setLength(BUFFER_SIZE * NUM_BUFFERS);

                mX.setLength(DATA_LENGTH);
                ASSERT(DATA_LENGTH          == X.length());
                ASSERT(EXP_NUM_BUFFERS      == X.numBuffers());
                ASSERT(EXP_NUM_DATA_BUFFERS == X.numDataBuffers());
                ASSERT(EXP_LAST_DB_LENGTH   == X.lastDataBufferLength());
                checkNoAliasedBlobBuffers(X);

                SimpleBlobBufferFactory ifa(INSERT_BUFFER_SIZE, &ta);
                bcema_BlobBuffer buffer;
                ifa.allocate(&buffer);
                ASSERT(INSERT_BUFFER_SIZE == buffer.size());

                mX.insertBuffer(INSERT_POSITION, buffer); // TEST HERE

                const int INSERT_FLAG = INSERT_POSITION < EXP_NUM_DATA_BUFFERS;
                ASSERT(DATA_LENGTH + INSERT_FLAG * INSERT_BUFFER_SIZE
                                                                == X.length());
                ASSERT(EXP_LAST_DB_LENGTH == X.lastDataBufferLength());
                ASSERT(EXP_NUM_BUFFERS + 1 == X.numBuffers());
                ASSERT(EXP_NUM_DATA_BUFFERS + INSERT_FLAG ==
                                                           X.numDataBuffers());

                // Repeat invariants after testing setLength in the presence
                // of zero-sized buffers.

                mX.setLength(0);
                mX.setLength(DATA_LENGTH + INSERT_FLAG * INSERT_BUFFER_SIZE);
                ASSERT(EXP_LAST_DB_LENGTH == X.lastDataBufferLength());
                ASSERT(EXP_NUM_BUFFERS + 1 == X.numBuffers());
                ASSERT(EXP_NUM_DATA_BUFFERS + INSERT_FLAG ==
                                                           X.numDataBuffers());

                checkNoAliasedBlobBuffers(X);
            }
            ASSERT(0 <  ta.numAllocations());
            ASSERT(0 == ta.numBytesInUse());
            ASSERT(0 == ta.numMismatches());
            ASSERT(0 == defaultAlloc.numAllocations());

            END_BSLMA_EXCEPTION_TEST
        }

      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING '*DataBuffer*'
        //
        // Concerns:
        //   - That the invariants governing the definition of the data buffers
        //     hold (including length, lastDataBufferLength, numDataBuffers,
        //     etc.)
        //   - That the last data buffer can never be empty unless
        //     the blob itself is empty.
        //
        // Plan:
        //   For all combinations of length, buffer size (for the factory), and
        //   number of buffers, check that a blob constructed to the proper
        //   parameters has the expected characteristics and that, after
        //   trimming the last data buffer, these characteristics are
        //   unchanged.  There are no allocations performed or exceptions
        //   thrown while running those functions.
        //
        // Testing:
        //   void bcema_Blob::trimLastDataBuffer();
        //   int bcema_Blob::lastDataBufferLength() const;
        //   int bcema_Blob::numDataBuffers() const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING '*DataBuffer*'" << endl
                          << "======================" << endl;

        for (int bufferSize = 1; bufferSize <= 5; ++bufferSize)
        for (int numBuffers = 0; numBuffers <= 5; ++numBuffers)
        for (int dataLength = 0; dataLength <= bufferSize * numBuffers;
                                                  ++dataLength)
        {
            bslma_TestAllocator ta(veryVeryVerbose);
            bslma_DefaultAllocatorGuard guard(&ta);
            {
                const int BUFFER_SIZE          = bufferSize;
                const int DATA_LENGTH          = dataLength;
                const int NUM_BUFFERS          = numBuffers;
                const int EXP_NUM_DATA_BUFFERS = (dataLength + BUFFER_SIZE - 1)
                                               / BUFFER_SIZE;
                const int EXP_NUM_BUFFERS      = NUM_BUFFERS;
                const int EXP_LAST_DB_LENGTH   = EXP_NUM_DATA_BUFFERS > 0
                      ?  DATA_LENGTH - (EXP_NUM_DATA_BUFFERS - 1) * BUFFER_SIZE
                      : 0;

                if (veryVerbose) {
                    T_(); P_(BUFFER_SIZE); P_(DATA_LENGTH); P(NUM_BUFFERS);
                    T_(); P_(EXP_NUM_DATA_BUFFERS); P_(EXP_NUM_BUFFERS);
                                                         P(EXP_LAST_DB_LENGTH);
                }

                typedef bcema_Blob Obj;
                SimpleBlobBufferFactory fa(BUFFER_SIZE, &ta);

                Obj mX(&fa, &ta);   const Obj& X = mX;
                mX.setLength(BUFFER_SIZE * NUM_BUFFERS);

                mX.setLength(DATA_LENGTH);
                LOOP3_ASSERT(bufferSize, numBuffers, dataLength,
                             EXP_NUM_BUFFERS == X.numBuffers());
                LOOP3_ASSERT(bufferSize, numBuffers, dataLength,
                      EXP_NUM_DATA_BUFFERS == X.numDataBuffers()); // TEST HERE
                LOOP3_ASSERT(bufferSize, numBuffers, dataLength,
                  EXP_LAST_DB_LENGTH == X.lastDataBufferLength()); // TEST HERE
                checkNoAliasedBlobBuffers(X);
                if (veryVerbose) {
                    P(X.numDataBuffers());
                    P(X.lastDataBufferLength());
                }

                mX.trimLastDataBuffer(); // TEST HERE

                LOOP3_ASSERT(bufferSize, numBuffers, dataLength,
                             EXP_NUM_BUFFERS == X.numBuffers());
                LOOP3_ASSERT(bufferSize, numBuffers, dataLength,
                      EXP_NUM_DATA_BUFFERS == X.numDataBuffers()); // TEST HERE
                LOOP3_ASSERT(bufferSize, numBuffers, dataLength,
                  EXP_LAST_DB_LENGTH == X.lastDataBufferLength()); // TEST HERE
                if (X.length()) {
                    LOOP3_ASSERT(bufferSize, numBuffers, dataLength,
                                 EXP_LAST_DB_LENGTH ==
                                        X.buffer(X.numDataBuffers()-1).size());
                }
                checkNoAliasedBlobBuffers(X);

                if (veryVerbose) {
                    P(X.numBuffers());
                    P(X.numDataBuffers());
                    P(X.lastDataBufferLength());
                    P(X.buffer(X.numDataBuffers()-1).size());
                }
            }
            ASSERT(0 == numBuffers || 0 <  ta.numAllocations());
            ASSERT(0 == ta.numBytesInUse());
            ASSERT(0 == ta.numMismatches());
        }

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING 'setLength' AND LENGTH ACCESSORS
        //
        // Concerns:
        //   - That 'setLength' increases the size of the blob properly if a
        //     blob buffer factory was supplied to the constructor.
        //   - That 'setLength' does not decrease the number of buffers.
        //   - That 'setLength' has the same behavior in the presence of
        //     zero-size buffers.
        //   - That 'length' and 'totalSize' return the value as expected.
        //   - That buffers are created and inserted in sequence by the buffer
        //     factory, in front-to-back order.
        //   - That there are no memory leaks, and that all memory is supplied
        //     by the proper allocator.
        //   - That the component is exception neutral.
        //
        // Plan:
        //   For all combinations of length, buffer size (for the factory), and
        //   number of buffers, check that a blob constructed has zero length,
        //   but that setting length to a positive value (not necessarily a
        //   multiple of the buffer size) increases the number of buffers, and
        //   that subsequently setting length to a smaller value changes the
        //   length but not the number of buffers, while changing to a greater
        //   value increases both.  In all cases, make sure that the buffers
        //   can be written into and that they are all distinct (no aliasing)
        //   and hold their value as expected.  Finally, set the length of an
        //   empty buffer with a 'TestBlobBufferFactory' whose buffer size
        //   increases at each call, to make sure the buffers are created in
        //   the proper order.  In all cases, run the test in a bdema exception
        //   test loop to test for exception neutrality (invariants are
        //   asserted in the blob destructor upon throwing the exception).
        //
        //   The only concern not address here is concerned about the
        //   presence of zero-sized buffers.  We address this concern
        //   in case 5 above (inserting empty buffers) and case 8 (prepending
        //   and appending empty data buffers).
        //
        // Testing:
        //   bcema_Blob::setLength(newLength);
        //   bcema_Blob::length();
        //   bcema_Blob::numBuffers();
        //   bcema_Blob::totalSize();
        // --------------------------------------------------------------------

        if (verbose)
            cout << endl
                 << "TESTING 'setLength' AND LENGTH ACCESSORS" << endl
                 << "========================================" << endl;

        if (verbose) cout << "\nTesting all blobs with fixed buffer size.\n";

        for (int bufferSize = 1; bufferSize <= 5; ++bufferSize)
        for (int dataLength = 0; dataLength <= 5 * bufferSize; ++dataLength)
        {
            bslma_TestAllocator defaultAlloc(veryVeryVerbose);
            bslma_DefaultAllocatorGuard guard(&defaultAlloc);
            bslma_TestAllocator ta(veryVeryVerbose);

            bslma_TestAllocator& testAllocator = ta;
            BEGIN_BSLMA_EXCEPTION_TEST
            {
                const int BUFFER_SIZE     = bufferSize;
                const int LENGTH          = dataLength;
                const int NUM_BUFFERS     = (dataLength + BUFFER_SIZE - 1)
                                          / BUFFER_SIZE;
                const int EXP_LENGTH      = dataLength;
                const int EXP_TOTAL_SIZE  = NUM_BUFFERS * BUFFER_SIZE;
                const int EXP_NUM_BUFFERS = NUM_BUFFERS;

                if (veryVerbose) {
                    T_(); P_(BUFFER_SIZE); P_(LENGTH); P(NUM_BUFFERS);
                    T_(); P_(EXP_LENGTH);  P(EXP_NUM_BUFFERS);
                }

                typedef bcema_Blob Obj;
                SimpleBlobBufferFactory fa(BUFFER_SIZE, &ta);

                Obj mX(&fa, &ta);   const Obj& X = mX;
                LOOP2_ASSERT(bufferSize, dataLength, 0 == X.length());
                LOOP2_ASSERT(bufferSize, dataLength, 0 == X.totalSize());
                LOOP2_ASSERT(bufferSize, dataLength, 0 == X.numBuffers());
                if (veryVerbose) {
                    P_(X.length()); P_(X.numBuffers()); P(X.totalSize());
                }

                mX.setLength(LENGTH); // TEST HERE
                LOOP2_ASSERT(bufferSize, dataLength,
                                        EXP_LENGTH == X.length()); // TEST HERE
                LOOP2_ASSERT(bufferSize, dataLength,
                                 EXP_TOTAL_SIZE == X.totalSize()); // TEST HERE
                LOOP2_ASSERT(bufferSize, dataLength,
                               EXP_NUM_BUFFERS == X.numBuffers()); // TEST HERE
                checkNoAliasedBlobBuffers(X);
                if (veryVerbose) {
                    P_(X.length()); P_(X.numBuffers()); P(X.totalSize());
                }

                mX.setLength(LENGTH); // TEST HERE
                LOOP2_ASSERT(bufferSize, dataLength,
                                        EXP_LENGTH == X.length()); // TEST HERE
                LOOP2_ASSERT(bufferSize, dataLength,
                                 EXP_TOTAL_SIZE == X.totalSize()); // TEST HERE
                LOOP2_ASSERT(bufferSize, dataLength,
                               EXP_NUM_BUFFERS == X.numBuffers()); // TEST HERE
                checkNoAliasedBlobBuffers(X);
                if (veryVerbose) {
                    P_(X.length()); P_(X.numBuffers()); P(X.totalSize());
                }

                for (int blobLength = 0; blobLength < EXP_LENGTH; ++blobLength)
                {
                    mX.setLength(blobLength); // TEST HERE
                    LOOP3_ASSERT(bufferSize, dataLength, blobLength,
                                        blobLength == X.length()); // TEST HERE
                    LOOP3_ASSERT(bufferSize, dataLength, blobLength,
                                 EXP_TOTAL_SIZE == X.totalSize()); // TEST HERE
                    LOOP3_ASSERT(bufferSize, dataLength, blobLength,
                               EXP_NUM_BUFFERS == X.numBuffers()); // TEST HERE
                    checkNoAliasedBlobBuffers(X);
                    if (veryVerbose) {
                        P_(X.length()); P_(X.numBuffers()); P(X.totalSize());
                    }
                }

                for (int blobLength = EXP_LENGTH+1;
                     blobLength < 2*EXP_LENGTH+2; ++blobLength)
                {
                    const int EXP_NUM_BUFFERS = (blobLength + BUFFER_SIZE - 1)
                                              / BUFFER_SIZE;
                    const int EXP_TOTAL_SIZE  = EXP_NUM_BUFFERS * BUFFER_SIZE;

                    mX.setLength(blobLength); // TEST HERE
                    LOOP3_ASSERT(bufferSize, dataLength, blobLength,
                                        blobLength == X.length()); // TEST HERE
                    LOOP3_ASSERT(bufferSize, dataLength, blobLength,
                                 EXP_TOTAL_SIZE == X.totalSize()); // TEST HERE
                    LOOP3_ASSERT(bufferSize, dataLength, blobLength,
                               EXP_NUM_BUFFERS == X.numBuffers()); // TEST HERE
                    checkNoAliasedBlobBuffers(X);
                    if (veryVerbose) {
                        P_(X.length()); P_(X.numBuffers()); P(X.totalSize());
                    }
                }
            }
            ASSERT(0 <  ta.numAllocations());
            ASSERT(0 == ta.numBytesInUse());
            ASSERT(0 == ta.numMismatches());
            ASSERT(0 == defaultAlloc.numAllocations());

            END_BSLMA_EXCEPTION_TEST
        }

        if (verbose) cout << "\nTesting blob with different buffer sizes.\n";
        {
            bslma_TestAllocator ta(veryVeryVerbose);
            bslma_DefaultAllocatorGuard guard(&ta);
            {
                typedef bcema_Blob Obj;
                TestBlobBufferFactory fa(&ta, 1);

                Obj mX(&fa, &ta);   const Obj& X = mX;

                mX.setLength(7);
                ASSERT(7 == X.length());
                ASSERT(7 == X.totalSize());
                ASSERT(3 == X.numBuffers());
                ASSERT(1 == X.buffer(0).size());
                ASSERT(2 == X.buffer(1).size());
                ASSERT(4 == X.buffer(2).size());
            }
            ASSERT(0 <  ta.numAllocations());
            ASSERT(0 == ta.numBytesInUse());
            ASSERT(0 == ta.numMismatches());
        }

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING CONSTRUCTORS
        //
        // Concerns:
        //   - That constructing a blob with buffers coming from a different
        //     factory does not create problems at deletion.
        //   - That a blob constructed with a buffer factory can grow.
        //   - That a blob constructed without a factory cannot grow.
        //   - That the component is exception neutral.
        //   - That all memory is allocated from the proper allocator and no
        //     memory is leaked.
        //
        // Plan:
        //
        // Testing:
        //  bcema_Blob(allocator);
        //  bcema_Blob(factory, allocator);
        //  bcema_Blob(buffers, numBuffers, factory, allocator);
        //  bcema_Blob(original, allocator);
        //  bcema_Blob(original, factory, allocator);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING CONSTRUCTORS" << endl
                          << "====================" << endl;

        bslma_TestAllocator         defaultAlloc(veryVeryVerbose);
        bslma_DefaultAllocatorGuard guard(&defaultAlloc);
        bslma_TestAllocator         ta(veryVeryVerbose);
        bslma_TestAllocator&        testAllocator = ta;

        const int BUFFER_SIZE = 4;

#if defined(BDE_BUILD_TARGET_EXC) && (defined(BDE_BUILD_TARGET_SAFE) || \
                                      defined(BDE_BUILD_TARGET_DBG))
        // This component works for *all* targets, including 'opt_exc_mt'.
        // However, portions of this test case fail unless either safe-mode or
        // debug is in effect.  If neither safe-mode nor debug is in effect,
        // then the 'bdes_assert' macros resolve to no-ops and the guard is
        // ineffective at catching assertion failures.

        if (verbose) cout << "\nTesting creating blob without factory.\n";
        {
            bsls_AssertFailureHandlerGuard guard(&unknownFactoryHandler);
            numUnknownFactoryHandlerInvocations = 0;

            typedef bcema_Blob Obj;
            Obj mX(&ta);  const Obj& X = mX;

            try {
                mX.setLength(1); // will throw
            } catch (const UnknownFactoryException&) {
            }
            ASSERT(1 == numUnknownFactoryHandlerInvocations);
            ASSERT(0 == X.length());
            ASSERT(0 == X.numBuffers());

            Obj mY(mX, &ta);  // const Obj& Y = mY;

            try {
                mY.setLength(1); // will throw
            } catch (const UnknownFactoryException&) {
            }
            ASSERT(2 == numUnknownFactoryHandlerInvocations);
            ASSERT(0 == X.length());
            ASSERT(0 == X.numBuffers());
        }
        ASSERT(0 == ta.numAllocations());
        ASSERT(0 == ta.numBytesInUse());
        ASSERT(0 == ta.numMismatches());
        ASSERT(0 == defaultAlloc.numAllocations());
#endif

        if (verbose) cout << "\nTesting creating blob with factory.\n";
        BEGIN_BSLMA_EXCEPTION_TEST
        {
            bsls_AssertFailureHandlerGuard guard(&unknownFactoryHandler);
            numUnknownFactoryHandlerInvocations = 0;

            typedef bcema_Blob Obj;
            SimpleBlobBufferFactory fa(BUFFER_SIZE, &ta);

            // Create X, with a factory.
            Obj mX(&fa, &ta);   const Obj& X = mX;
            ASSERT(0 == X.length());
            ASSERT(0 == X.totalSize());
            ASSERT(0 == X.numBuffers());
            if (veryVerbose) {
                P_(X.length()); P_(X.numBuffers()); P(X.totalSize());
            }

            mX.setLength(1);

            ASSERT(0 == numUnknownFactoryHandlerInvocations);
            ASSERT(1           == X.length());
            ASSERT(BUFFER_SIZE == X.totalSize());
            ASSERT(1           == X.numBuffers());

            // Copy from X, without a factory.
            Obj mY(mX, &ta);  const Obj& Y = mY;
            ASSERT(0 == numUnknownFactoryHandlerInvocations);
            ASSERT(1           == Y.length());
            ASSERT(BUFFER_SIZE == Y.totalSize());
            ASSERT(1           == Y.numBuffers());

#if defined(BDE_BUILD_TARGET_EXC) && (defined(BDE_BUILD_TARGET_SAFE) || \
                                      defined(BDE_BUILD_TARGET_DBG))
            try {
                mY.setLength(BUFFER_SIZE + 1);
            }
            catch (const UnknownFactoryException&) {
            }
            ASSERT(1 == numUnknownFactoryHandlerInvocations);
            numUnknownFactoryHandlerInvocations = 0;
            ASSERT(1           == Y.length());
            ASSERT(BUFFER_SIZE == Y.totalSize());
            ASSERT(1           == Y.numBuffers());
#endif

            // Copy from X, with a factory.
            Obj mZ1(mX, &fa, &ta);  const Obj& Z1 = mZ1;
            ASSERT(0 == numUnknownFactoryHandlerInvocations);
            ASSERT(1           == Z1.length());
            ASSERT(BUFFER_SIZE == Z1.totalSize());
            ASSERT(1           == Z1.numBuffers());

            mZ1.setLength(BUFFER_SIZE + 1);
            ASSERT(0 == numUnknownFactoryHandlerInvocations);
            ASSERT(BUFFER_SIZE + 1 == Z1.length());
            ASSERT(BUFFER_SIZE * 2 == Z1.totalSize());
            ASSERT(2               == Z1.numBuffers());

            // Copy from Y, with a factory (even though Y does not have one).
            Obj mZ2(mY, &fa, &ta);  const Obj& Z2 = mZ2;
            ASSERT(0 == numUnknownFactoryHandlerInvocations);
            ASSERT(1           == Z2.length());
            ASSERT(BUFFER_SIZE == Z2.totalSize());
            ASSERT(1           == Z2.numBuffers());

            mZ2.setLength(BUFFER_SIZE + 1);
            ASSERT(0 == numUnknownFactoryHandlerInvocations);
            ASSERT(BUFFER_SIZE + 1 == Z2.length());
            ASSERT(BUFFER_SIZE * 2 == Z2.totalSize());
            ASSERT(2               == Z2.numBuffers());
        }
        ASSERT(0 <  ta.numAllocations());
        ASSERT(0 == ta.numBytesInUse());
        ASSERT(0 == ta.numMismatches());
        ASSERT(0 == defaultAlloc.numAllocations());
        END_BSLMA_EXCEPTION_TEST

#if 0
        if (verbose) cout << "\nTesting creating blob importing buffers.\n";
        BEGIN_BSLMA_EXCEPTION_TEST
        {
            bsls_AssertFailureHandlerGuard guard(&unknownFactoryHandler);
            numUnknownFactoryHandlerInvocations = 0;

            typedef bcema_Blob Obj;
            SimpleBlobBufferFactory fa(BUFFER_SIZE, &ta);

            NullDeleter deleter;
            char buffer[BUFFER_SIZE - 1];
            bcema_SharedPtr<char> shptr1(buffer, &deleter, &ta);
            bcema_BlobBuffer b1(shptr1, BUFFER_SIZE - 1);

            bcema_SharedPtr<char> shptr2((char *)ta.allocate(BUFFER_SIZE + 1),
                                         &ta, &ta);
            bcema_BlobBuffer b2(shptr2, BUFFER_SIZE + 1);

            SimpleBlobBufferFactory fa3(BUFFER_SIZE - 2, &ta);
            bcema_BlobBuffer b3; fa3.allocate(&b3);

            SimpleBlobBufferFactory fa4(BUFFER_SIZE + 2, &ta);
            bcema_BlobBuffer b4; fa4.allocate(&b4);

            const int NUM_BUFFERS = 4;
            const int DATA_LENGTH = 4 * BUFFER_SIZE;
            const int TOTAL_SIZE  = 4 * BUFFER_SIZE;
            const bcema_BlobBuffer BUFFERS[NUM_BUFFERS] = { b1, b2, b3, b4 };

            Obj mX(BUFFERS, NUM_BUFFERS, &ta);   const Obj& X = mX;
            ASSERT(DATA_LENGTH == X.length());
            ASSERT(TOTAL_SIZE  == X.totalSize());
            ASSERT(NUM_BUFFERS == X.numBuffers());
            if (veryVerbose) {
                P_(X.length()); P_(X.numBuffers()); P(X.totalSize());
            }

            try {
                mX.setLength(DATA_LENGTH + 1);
            } catch (UnknownFactoryException e) {
            }
            ASSERT(1 == numUnknownFactoryHandlerInvocations);
            ASSERT(DATA_LENGTH == X.length());
            ASSERT(TOTAL_SIZE  == X.totalSize());
            ASSERT(NUM_BUFFERS == X.numBuffers());
        }
        ASSERT(0 <  ta.numAllocations());
        ASSERT(0 == ta.numBytesInUse());
        ASSERT(0 == ta.numMismatches());
        ASSERT(0 == defaultAlloc.numAllocations());
        END_BSLMA_EXCEPTION_TEST
#endif

        if (verbose)
           cout << "\nTesting creating blob with factory importing buffers.\n";
        BEGIN_BSLMA_EXCEPTION_TEST
        {
            bsls_AssertFailureHandlerGuard guard(&unknownFactoryHandler);
            numUnknownFactoryHandlerInvocations = 0;

            typedef bcema_Blob Obj;
            SimpleBlobBufferFactory fa(BUFFER_SIZE, &ta);

            NullDeleter deleter;
            char buffer[BUFFER_SIZE - 1];
            bcema_SharedPtr<char> shptr1(buffer, &deleter, &ta);
            bcema_BlobBuffer b1(shptr1, BUFFER_SIZE - 1);

            bcema_SharedPtr<char> shptr2((char *) ta.allocate(BUFFER_SIZE + 1),
                                         &ta, &ta);
            bcema_BlobBuffer b2(shptr2, BUFFER_SIZE + 1);

            SimpleBlobBufferFactory fa3(BUFFER_SIZE - 2, &ta);
            bcema_BlobBuffer b3; fa3.allocate(&b3);

            SimpleBlobBufferFactory fa4(BUFFER_SIZE + 2, &ta);
            bcema_BlobBuffer b4; fa4.allocate(&b4);

            const int NUM_BUFFERS = 4;
            const int DATA_LENGTH = 4 * BUFFER_SIZE;
            const int TOTAL_SIZE  = 4 * BUFFER_SIZE;
            const bcema_BlobBuffer BUFFERS[NUM_BUFFERS] = { b1, b2, b3, b4 };

            Obj mX(BUFFERS, NUM_BUFFERS, &fa, &ta);  const Obj& X = mX;
            ASSERT(0           == X.length());
            ASSERT(TOTAL_SIZE  == X.totalSize());
            ASSERT(NUM_BUFFERS == X.numBuffers());
            if (veryVerbose) {
                P_(X.length()); P_(X.numBuffers()); P(X.totalSize());
            }

            mX.setLength(DATA_LENGTH + 1);
            ASSERT(0 == numUnknownFactoryHandlerInvocations);
            ASSERT(DATA_LENGTH + 1          == X.length());
            ASSERT(TOTAL_SIZE + BUFFER_SIZE == X.totalSize());
            ASSERT(NUM_BUFFERS + 1          == X.numBuffers());
        }
        ASSERT(0 <  ta.numAllocations());
        ASSERT(0 == ta.numBytesInUse());
        ASSERT(0 == ta.numMismatches());
        ASSERT(0 == defaultAlloc.numAllocations());
        END_BSLMA_EXCEPTION_TEST

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST:
        //   Developers' Sandbox.
        //
        // Concerns:
        //   - That 'bcema_BlobBuffer' is an in-core value type.
        //   - That 'bcema_Blob' basic manipulation does what is expected.
        //
        // Plan:
        //   Do a mini value-semantic 10 case driver for 'bcema_BlobBuffer'.
        //   Manipulate 'bcema_Blob' using a non-fixed size blob buffer factory
        //   with 'setLength', 'insertBuffer', 'removeBuffer', and
        //   'appendBuffer'.  For each blob, assert the state is as expected
        //   and check the validity of the buffers.
        //
        // Testing:
        //   This "test" *exercises* basic functionality, but *tests* nothing.
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BREATHING TEST" << endl
                          << "==============" << endl;

        bslma_TestAllocator ta(veryVeryVerbose);
        bslma_DefaultAllocatorGuard guard(&ta);
        NullDeleter deleter;

        if (verbose) cout << "\nTesting bcema_BlobBuffer." << endl;
        {
            typedef bcema_BlobBuffer Obj;
            bcema_SharedPtr<char> shptrA((char *) 0, &deleter, &ta);
            Obj mVA(shptrA, 1); const Obj& VA = mVA;

            bcema_SharedPtr<char> shptrB((char *) 1, &deleter, &ta);
            Obj mVB(shptrB, 2); const Obj& VB = mVB;

            bcema_SharedPtr<char> shptrC((char *) ULONG_MAX, &deleter, &ta);
            Obj mVC(shptrC, INT_MAX); const Obj& VC = mVC;

            if (verbose) {
                cout << "\n 1. Create an object x1 (initialize to VA)."
                     << "\t\t{ x1:VA }"
                     << endl;
            }

            Obj        mX1 = VA;
            const Obj& X1 = mX1;

            if (verbose) {
                cout << "\t(a) Check initial state of x1." << endl;
            }

            ASSERT(VA.data() == X1.data());
            ASSERT(VA.buffer().ptr() ==  X1.buffer().ptr());
            ASSERT(VA.size() == X1.size());
            ASSERT(VA == X1);

            // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
            if (verbose) {
                cout << "\n 2. Create an object x2 (copy from x1)."
                     << "\t\t{ x1:VA x2:VA }"
                     << endl;
            }

            Obj mX2(X1);  const Obj& X2 = mX2;

            if (verbose) {
                cout << "\t(a) Check the initial state of x2." << endl;
            }
            ASSERT(VA.data() == X2.data());
            ASSERT(VA.buffer().ptr() ==  X2.buffer().ptr());
            ASSERT(VA.size() == X2.size());

            if (verbose) {
                cout << "\t(b) Try equality operators: x2 <op> x1, x2."
                     << endl;
            }
            ASSERT(true == (X2 == X1));    ASSERT(false == (X2 != X1));
            ASSERT(true == (X2 == X2));    ASSERT(false == (X2 != X2));

            // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
            if (verbose) {
                cout << "\n 3. Set x1 to a new value VB."
                     << "\t\t\t{ x1:VB x2:VA }"
                     << endl;
            }

            mX1 = VB;

            if (verbose) {
                cout << "\t(a) Check new state of x1." << endl;
            }
            ASSERT(VB.data() == X1.data());
            ASSERT(VB.buffer().ptr() ==  X1.buffer().ptr());
            ASSERT(VB.size() == X1.size());
            ASSERT(mX1 == VB);

            if (verbose) {
                cout << "\t(b) Try equality operators: x1 <op> x1, x2."
                     << endl;
            }
            ASSERT(true  == (X1 == X1));    ASSERT(false == (X1 != X1));
            ASSERT(false == (X1 == X2));    ASSERT(true  == (X1 != X2));

            // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
            if (verbose) {
                cout << "\n 4. Create a default object x3()."
                     << "\t\t{ x1:VB x2:VA x3:U }"
                     << endl;
            }

            Obj mX3;  const Obj& X3 = mX3;

            if (verbose) {
                cout << "\t(a) Check initial state of x3." << endl;
            }
            ASSERT(0 == X3.data());
            ASSERT(0 ==  X3.buffer().ptr());
            ASSERT(0 == X3.size());

            if (verbose) {
                cout << "\t(b) Try equality operators: x3 <op> x1, x2, x3."
                     << endl;
            }
            ASSERT(false == (X3 == X1));    ASSERT(true  == (X3 != X1));
            ASSERT(false == (X3 == X2));    ASSERT(true  == (X3 != X2));
            ASSERT(true  == (X3 == X3));    ASSERT(false == (X3 != X3));

            // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
            if (verbose) {
                cout << "\n 5. Create an object x4 (copy from x3)."
                     << "\t\t{ x1:VA x2:VA x3:U  x4:U }"
                     << endl;
            }

            Obj mX4(X3);  const Obj& X4 = mX4;

            if (verbose) {
                cout << "\t(a) Check initial state of x4." << endl;
            }
            ASSERT(0 == X4.data());
            ASSERT(0 ==  X4.buffer().ptr());
            ASSERT(0 == X4.size());

            if (verbose) {
                cout << "\t(b) Try equality operators: x4 <op> x1, x2, x3, x4."
                     << endl;
            }
            ASSERT(false == (X4 == X1));    ASSERT(true  == (X4 != X1));
            ASSERT(false == (X4 == X2));    ASSERT(true  == (X4 != X2));
            ASSERT(true  == (X4 == X3));    ASSERT(false == (X4 != X3));
            ASSERT(true  == (X4 == X4));    ASSERT(false == (X4 != X4));

            // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
            if (verbose) {
                cout << "\n 6. Set x3 to a new value VC."
                     << "\t\t\t{ x1:VB x2:VA x3:VC x4:U }"
                     << endl;
            }

            mX3 = VC;

            if (verbose) {
                cout << "\t(a) Check new state of x3." << endl;
            }
            ASSERT(VC.data() == X3.data());
            ASSERT(VC.buffer().ptr() ==  X3.buffer().ptr());
            ASSERT(VC.size() == X3.size());
            ASSERT(VC == X3);

            if (verbose) {
                cout << "\t(b) Try equality operators: x3 <op> x1, x2, x3, x4."
                     << endl;
            }
            ASSERT(false == (X3 == X1));    ASSERT(true  == (X3 != X1));
            ASSERT(false == (X3 == X2));    ASSERT(true  == (X3 != X2));
            ASSERT(true  == (X3 == X3));    ASSERT(false == (X3 != X3));
            ASSERT(false == (X3 == X4));    ASSERT(true  == (X3 != X4));

            // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
            if (verbose) {
                cout << "\n 7. Assign x2 = x1."
                     << "\t\t\t\t{ x1:VB x2:VB x3:VC x4:U }"
                     << endl;
            }

            mX2 = X1;

            if (verbose) {
                cout << "\t(a) Check new state of x2." << endl;
            }
            ASSERT(VB.data() == X2.data());
            ASSERT(VB.buffer().ptr() ==  X2.buffer().ptr());
            ASSERT(VB.size() == X2.size());
            ASSERT(VB == X2);

            if (verbose) {
                cout << "\t(b) Try equality operators: x2 <op> x1, x2, x3, x4."
                     << endl;
            }
            ASSERT(true  == (X2 == X1));    ASSERT(false == (X2 != X1));
            ASSERT(true  == (X2 == X2));    ASSERT(false == (X2 != X2));
            ASSERT(false == (X2 == X3));    ASSERT(true  == (X2 != X3));
            ASSERT(false == (X2 == X4));    ASSERT(true  == (X2 != X4));

            // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
            if (verbose) {
                cout << "\n 8. Assign x2 = x3."
                     << "\t\t\t\t{ x1:VB x2:VC x3:VC x4:U }"
                     << endl;
            }

            mX2 = X3;

            if (verbose) {
                cout << "\t(a) Check new state of x2." << endl;
            }
            ASSERT(VC.data() == X2.data());
            ASSERT(VC.buffer().ptr() ==  X2.buffer().ptr());
            ASSERT(VC.size() == X2.size());
            ASSERT(VC == X2);

            if (verbose) {
                cout << "\t(b) Try equality operators: x2 <op> x1, x2, x3, x4."
                     << endl;
            }
            ASSERT(false == (X2 == X1));    ASSERT(true  == (X2 != X1));
            ASSERT(true  == (X2 == X2));    ASSERT(false == (X2 != X2));
            ASSERT(true  == (X2 == X3));    ASSERT(false == (X2 != X3));
            ASSERT(false == (X2 == X4));    ASSERT(true  == (X2 != X4));

            // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
            if (verbose) {
                cout << "\n 9. Assign x1 = x1 (aliasing)."
                     << "\t\t\t{ x1:VB x2:VC x3:VC x4:U }"
                     << endl;
            }

            mX1 = X1;

            if (verbose) {
                cout << "\t(a) Check new state of x1." << endl;
            }
            ASSERT(VB == X1);

            if (verbose) {
                cout << "\t(a) Try equality operators: x1 <op> x1, x2, x3, x4."
                     << endl;
            }
            ASSERT(true  == (X1 == X1));    ASSERT(false == (X1 != X1));
            ASSERT(false == (X1 == X2));    ASSERT(true  == (X1 != X2));
            ASSERT(false == (X1 == X3));    ASSERT(true  == (X1 != X3));
            ASSERT(false == (X1 == X4));    ASSERT(true  == (X1 != X4));

            // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
            if (verbose) {
                cout << "\n 10.  Reset x1 (to empty)."
                     << "\t\t\t{ x1:U x2:VC x3:VC x4:U }"
                     << endl;
            }

            mX1.reset();
            ASSERT(X4 == X1);

            if (verbose) {
                cout << "\t(b) Try equality operators: x1 <op> x1, x2, x3, x4."
                     << endl;
            }
            ASSERT(true  == (X1 == X1));    ASSERT(false == (X1 != X1));
            ASSERT(false == (X1 == X2));    ASSERT(true  == (X1 != X2));
            ASSERT(false == (X1 == X3));    ASSERT(true  == (X1 != X3));
            ASSERT(true  == (X1 == X4));    ASSERT(false == (X1 != X4));
        }

        if (verbose) cout << "\nTesting bcema_Blob." << endl;
        {
            typedef bcema_Blob Obj;
            TestBlobBufferFactory fa(&ta);

            Obj mX(&fa, &ta);  const Obj& X = mX;
            ASSERT(0 == X.length());
            ASSERT(0 == X.totalSize());
            ASSERT(0 == X.numBuffers());
            ASSERT(4 == fa.currentBufferSize());

            mX.setLength(0);
            ASSERT(0 == X.totalSize());
            ASSERT(0 == X.length());
            ASSERT(0 == X.numBuffers());

            mX.setLength(1);
            ASSERT(4 == X.totalSize());
            ASSERT(1 == X.length());
            ASSERT(1 == X.numBuffers());
            ASSERT(4 == X.buffer(0).size());
            checkNoAliasedBlobBuffers(X);

            mX.setLength(2);
            ASSERT(4 == X.totalSize());
            ASSERT(2 == X.length());
            ASSERT(1 == X.numBuffers());
            ASSERT(4 == X.buffer(0).size());
            checkNoAliasedBlobBuffers(X);

            mX.setLength(0);
            ASSERT(4 == X.totalSize());
            ASSERT(0 == X.length());
            ASSERT(1 == X.numBuffers());
            ASSERT(4 == X.buffer(0).size());
            checkNoAliasedBlobBuffers(X);

            ASSERT(8 == fa.currentBufferSize());
            mX.setLength(1);
            ASSERT(4 == X.totalSize());
            ASSERT(1 == X.length());
            ASSERT(1 == X.numBuffers());
            ASSERT(4 == X.buffer(0).size());
            checkNoAliasedBlobBuffers(X);

            ASSERT(8 == fa.currentBufferSize());
            mX.setLength(4);
            ASSERT(4 == X.totalSize());
            ASSERT(4 == X.length());
            ASSERT(1 == X.numBuffers());
            ASSERT(4 == X.buffer(0).size());
            checkNoAliasedBlobBuffers(X);

            ASSERT(8 == fa.currentBufferSize());
            mX.setLength(5);
            ASSERT(12 == X.totalSize());
            ASSERT(5 == X.length());
            ASSERT(2 == X.numBuffers());
            ASSERT(4 == X.buffer(0).size());
            ASSERT(8 == X.buffer(1).size());
            checkNoAliasedBlobBuffers(X);

            ASSERT(16 == fa.currentBufferSize());
            mX.setLength(30);
            ASSERT(60 == X.totalSize());
            ASSERT(30 == X.length());
            ASSERT(4 == X.numBuffers());
            ASSERT(4  == X.buffer(0).size());
            ASSERT(8  == X.buffer(1).size());
            ASSERT(16 == X.buffer(2).size());
            ASSERT(32 == X.buffer(3).size());
            checkNoAliasedBlobBuffers(X);

            ASSERT(64 == fa.currentBufferSize());
            mX.setLength(124);
            ASSERT(124 == X.totalSize());
            ASSERT(124 == X.length());
            ASSERT(5 == X.numBuffers());
            ASSERT(4  == X.buffer(0).size());
            ASSERT(8  == X.buffer(1).size());
            ASSERT(16 == X.buffer(2).size());
            ASSERT(32 == X.buffer(3).size());
            ASSERT(64 == X.buffer(4).size());
            checkNoAliasedBlobBuffers(X);

            fa.setGrowFlag(false);
            ASSERT(128 == fa.currentBufferSize());
            mX.setLength(125);
            ASSERT(252 == X.totalSize());
            ASSERT(125 == X.length());
            ASSERT(6   == X.numBuffers());
            ASSERT(4   == X.buffer(0).size());
            ASSERT(8   == X.buffer(1).size());
            ASSERT(16  == X.buffer(2).size());
            ASSERT(32  == X.buffer(3).size());
            ASSERT(64  == X.buffer(4).size());
            ASSERT(128 == X.buffer(5).size());
            checkNoAliasedBlobBuffers(X);

            ASSERT(128 == fa.currentBufferSize());
            mX.removeBuffer(5);
            ASSERT(124 == X.totalSize());
            ASSERT(124 == X.length());
            ASSERT(5   == X.numBuffers());
            ASSERT(4   == X.buffer(0).size());
            ASSERT(8   == X.buffer(1).size());
            ASSERT(16  == X.buffer(2).size());
            ASSERT(32  == X.buffer(3).size());
            ASSERT(64  == X.buffer(4).size());
            checkNoAliasedBlobBuffers(X);

            mX.removeBuffer(2);
            ASSERT(108 == X.totalSize());
            ASSERT(108 == X.length());
            ASSERT(4   == X.numBuffers());
            ASSERT(4   == X.buffer(0).size());
            ASSERT(8   == X.buffer(1).size());
            ASSERT(32  == X.buffer(2).size());
            ASSERT(64  == X.buffer(3).size());
            checkNoAliasedBlobBuffers(X);

            mX.setLength(45);
            ASSERT(108 == X.totalSize());
            ASSERT(45  == X.length());
            ASSERT(4   == X.numBuffers());
            ASSERT(4   == X.buffer(0).size());
            ASSERT(8   == X.buffer(1).size());
            ASSERT(32  == X.buffer(2).size());
            ASSERT(64  == X.buffer(3).size());
            checkNoAliasedBlobBuffers(X);

            mX.setLength(44);
            ASSERT(108 == X.totalSize());
            ASSERT(44  == X.length());
            ASSERT(4   == X.numBuffers());
            ASSERT(4   == X.buffer(0).size());
            ASSERT(8   == X.buffer(1).size());
            ASSERT(32  == X.buffer(2).size());
            ASSERT(64  == X.buffer(3).size());
            checkNoAliasedBlobBuffers(X);

            mX.setLength(1);
            ASSERT(108 == X.totalSize());
            ASSERT(1   == X.length());
            ASSERT(4   == X.numBuffers());
            ASSERT(4   == X.buffer(0).size());
            ASSERT(8   == X.buffer(1).size());
            ASSERT(32  == X.buffer(2).size());
            ASSERT(64  == X.buffer(3).size());
            checkNoAliasedBlobBuffers(X);

            bcema_BlobBuffer buf;
            fa.allocate(&buf);
            ASSERT(128 == buf.size());
            mX.appendBuffer(buf);
            ASSERT(236 == X.totalSize());
            ASSERT(1   == X.length());
            ASSERT(5   == X.numBuffers());
            ASSERT(4   == X.buffer(0).size());
            ASSERT(8   == X.buffer(1).size());
            ASSERT(32  == X.buffer(2).size());
            ASSERT(64  == X.buffer(3).size());
            ASSERT(128 == X.buffer(4).size());
            checkNoAliasedBlobBuffers(X);

            mX.setLength(28);
            ASSERT(236 == X.totalSize());
            ASSERT(28  == X.length());
            ASSERT(5   == X.numBuffers());
            ASSERT(4   == X.buffer(0).size());
            ASSERT(8   == X.buffer(1).size());
            ASSERT(32  == X.buffer(2).size());
            ASSERT(64  == X.buffer(3).size());
            ASSERT(128 == X.buffer(4).size());
            checkNoAliasedBlobBuffers(X);

            fa.allocate(&buf);
            mX.insertBuffer(1, buf);
            ASSERT(364 == X.totalSize());
            ASSERT(156 == X.length());
            ASSERT(6   == X.numBuffers());
            ASSERT(4   == X.buffer(0).size());
            ASSERT(128 == X.buffer(1).size());
            ASSERT(8   == X.buffer(2).size());
            ASSERT(32  == X.buffer(3).size());
            ASSERT(64  == X.buffer(4).size());
            ASSERT(128 == X.buffer(5).size());
            checkNoAliasedBlobBuffers(X);

            fa.allocate(&buf);
            mX.insertBuffer(3, buf);
            ASSERT(492 == X.totalSize());
            ASSERT(284 == X.length());
            ASSERT(7   == X.numBuffers());
            ASSERT(4   == X.buffer(0).size());
            ASSERT(128 == X.buffer(1).size());
            ASSERT(8   == X.buffer(2).size());
            ASSERT(128 == X.buffer(3).size());
            ASSERT(32  == X.buffer(4).size());
            ASSERT(64  == X.buffer(5).size());
            ASSERT(128 == X.buffer(6).size());
            checkNoAliasedBlobBuffers(X);

            fa.allocate(&buf);
            mX.insertBuffer(7, buf);
            ASSERT(620 == X.totalSize());
            ASSERT(284 == X.length());
            ASSERT(8   == X.numBuffers());
            ASSERT(4   == X.buffer(0).size());
            ASSERT(128 == X.buffer(1).size());
            ASSERT(8   == X.buffer(2).size());
            ASSERT(128 == X.buffer(3).size());
            ASSERT(32  == X.buffer(4).size());
            ASSERT(64  == X.buffer(5).size());
            ASSERT(128 == X.buffer(6).size());
            ASSERT(128 == X.buffer(7).size());
            checkNoAliasedBlobBuffers(X);

            mX.setLength(0);
            ASSERT(620 == X.totalSize());
            ASSERT(0   == X.length());
            ASSERT(8   == X.numBuffers());
            checkNoAliasedBlobBuffers(X);
        }
        ASSERT(0 <  ta.numAllocations());
        ASSERT(0 == ta.numBytesInUse());
        ASSERT(0 == ta.numMismatches());

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
//      Copyright (C) Bloomberg L.P., 2005
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
