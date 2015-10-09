// btlb_blob.h                                                        -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BTLB_BLOB
#define INCLUDED_BTLB_BLOB

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide an indexed set of buffers from multiple sources.
//
//@CLASSES:
//        btlb::BlobBuffer: in-core representation of a shared buffer
// btlb::BlobBufferFactory: factory of blob buffers
//              btlb::Blob: indexed sequence of buffers
//
//@SEE_ALSO: bcema_sharedptr, btlb_pooledblobbufferfactory
//
//@DESCRIPTION: This component provides an indexed sequence ('btlb::Blob') of
// 'btlb::BlobBuffer' objects allocated from potentially multiple
// 'btlb::BlobBufferFactory' objects.  A 'btlb::BlobBuffer' is a simple in-core
// value object owning a shared pointer to a memory buffer.  Therefore, the
// lifetime of the underlying memory is determined by shared ownership between
// the blob buffer, the blob(s) that may contain it, and any other entities
// that may share ownership of the memory buffer.
//
// Logically, a 'btlb::Blob' can be thought of as a sequence of bytes (although
// not contiguous).  Each buffer in a blob contributes its own size to the
// blob, with the total size of a blob being the sum of sizes over all its
// buffers.  A prefix of these bytes, collectively referred to as the data of
// the blob, are defined by the data length, which can be set by the user using
// the 'setLength' method.  Note that the data length never exceeds the total
// size.  When setting the length to a value greater than the total size, the
// latter is increased automatically by adding buffers created from a factory
// passed at construction; the behavior is undefined if no factory was supplied
// at construction.
//
// The blob also updates its data length during certain operations (e.g.,
// insertion/removal of buffers containing some data bytes), as well as several
// attributes driven by the data length.  The first bytes numbered by the data
// length belong to the data buffers.  Note that all data buffers, except
// perhaps the last, contribute all their bytes to the 'btlb::Blob' data.  The
// last data buffer contributes anywhere between one and all of its bytes to
// the 'btlb::Blob' data.  The number of data buffers (returned by the
// 'numDataBuffers' method), as well as the last data buffer length (returned
// by 'lastDataBufferLength'), are maintained by 'btlb::Blob' automatically
// when setting the length to a new value.
//
// Buffers which do not contain data are referred to as capacity buffers.  The
// total size of a blob does not decrease when setting the length to a value
// smaller than the current length.  Instead, any data buffer that no longer
// contains data after the call to 'setLength' becomes a capacity buffer, and
// may become a data buffer again later if setting length past its prefix size.
//
// This design is intended to allow very efficient re-assignment of buffers (or
// part of buffers using shared pointer aliasing) between different blobs,
// without copying of the underlying data, while promoting efficient allocation
// of resources (via retaining capacity).  Thus, 'btlb::Blob' is an
// advantageous replacement for 'btlb::PooledBufferChain' when manipulation of
// the sequence, sharing of portions of the sequence, and lifetime management
// of individual portions of the sequence, are desired.  Another added
// flexibility of 'btlb::Blob' is the possibility for buffers in the sequence
// to have different sizes (as opposed to a uniform fixed size for
// 'btlb::PooledBufferChain').  When choosing whether to use a 'btlb::Blob' vs.
// a 'btlb::PooledBufferChain', one must consider the added flexibility versus
// the added cost of shared ownership for each individual buffer and random
// access to the buffer.
//
///Thread Safety
///-------------
// Different instances of the classes defined in this component can be
// concurrently modified by different threads.  Thread safety of a particular
// instance is not guaranteed, and therefore must be handled by the user.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: A Simple Blob Buffer Factory
///- - - - - - - - - - - - - - - - - - - -
// Classes that implement the 'btlb::BlobBufferFactory' protocol are used to
// allocate 'btlb::BlobBuffer' objects.  A simple implementation follows:
//..
//  class SimpleBlobBufferFactory : public btlb::BlobBufferFactory {
//      // This factory creates blob buffers of a fixed size specified at
//      // construction.
//
//      // DATA
//      bsl::size_t       d_bufferSize;
//      bslma::Allocator *d_allocator_p;
//
//    private:
//      // Not implemented:
//      SimpleBlobBufferFactory(const SimpleBlobBufferFactory&);
//      SimpleBlobBufferFactory& operator=(const SimpleBlobBufferFactory&);
//
//    public:
//      // CREATORS
//      explicit SimpleBlobBufferFactory(int               bufferSize = 1024,
//                                       bslma::Allocator *basicAllocator = 0);
//      ~SimpleBlobBufferFactory();
//
//      // MANIPULATORS
//      void allocate(btlb::BlobBuffer *buffer);
//  };
//
//  SimpleBlobBufferFactory::SimpleBlobBufferFactory(
//                                            int               bufferSize,
//                                            bslma::Allocator *basicAllocator)
//  : d_bufferSize(bufferSize)
//  , d_allocator_p(bslma::Default::allocator(basicAllocator))
//  {
//  }
//
//  SimpleBlobBufferFactory::~SimpleBlobBufferFactory()
//  {
//  }
//
//  void SimpleBlobBufferFactory::allocate(btlb::BlobBuffer *buffer)
//  {
//      bsl::shared_ptr<char> shptr(
//                              (char *) d_allocator_p->allocate(d_bufferSize),
//                              d_allocator_p);
//
//      buffer->reset(shptr, d_bufferSize);
//  }
//..
// Note that should the user desire a blob buffer factory for his/her
// application, a better implementation that pools buffers is available in the
// 'btlb_pooledblobbufferfactory' component.
//
///Simple Blob Usage
///- - - - - - - - -
// Blobs can be created just by passing a factory that is responsible to
// allocate the 'btlb::BlobBuffer'.  The following simple program illustrates
// how.
//..
//  {
//      SimpleBlobBufferFactory myFactory(1024);
//
//      btlb::Blob blob(&myFactory);
//      assert(0    == blob.length());
//      assert(0    == blob.totalSize());
//
//      blob.setLength(512);
//      assert( 512 == blob.length());
//      assert(1024 == blob.totalSize());
//..
// Users need to access buffers directly in order to read/write data.
//..
//      char data[] = "12345678901234567890"; // 20 bytes
//      assert(0 != blob.numBuffers());
//      assert(static_cast<int>(sizeof(data)) <= blob.buffer(0).size());
//      bsl::memcpy(blob.buffer(0).data(), data, sizeof(data));
//
//      blob.setLength(sizeof(data));
//      assert(sizeof data == blob.length());
//      assert(       1024 == blob.totalSize());
//..
// A 'btlb::BlobBuffer' can easily be re-assigned from one blob to another with
// no copy.  In that case, the memory held by the buffer will be returned to
// its factory when the last blob referencing the buffer is destroyed.  For the
// following example, a blob will be created using the default constructor.  In
// this case, the 'btlb::Blob' object will not able to grow on its own.
// Calling 'setLength' for a number equal or greater than 'totalSize()' will
// result in undefined behavior.
//..
//      btlb::Blob dest;
//      assert(   0 == dest.length());
//      assert(   0 == dest.totalSize());
//
//      assert(0 != blob.numBuffers());
//      dest.appendBuffer(blob.buffer(0));
//      assert(   0 == dest.length());
//      assert(1024 == dest.totalSize());
//..
// Note that at this point, the logical length (returned by 'length') of this
// object has not changed.  'setLength' must be called explicitly by the user
// if the logical length of the 'btlb::Blob' must be changed:
//..
//      dest.setLength(dest.buffer(0).size());
//      assert(1024 == dest.length());
//      assert(1024 == dest.totalSize());
//..
// Sharing only a part of a buffer is also possible through shared pointer
// aliasing.  In the following example, a buffer that contains only bytes 11-16
// from the first buffer of 'blob' will be appended to 'blob'.
//..
//      assert(0 != blob.numBuffers());
//      assert(16 <= blob.buffer(0).size());
//
//      bsl::shared_ptr<char> shptr(blob.buffer(0).buffer(),
//                                  blob.buffer(0).data() + 10);
//          // 'shptr' is now an alias of 'blob.buffer(0).buffer()'.
//
//      btlb::BlobBuffer partialBuffer(shptr, 6);
//      dest.appendBuffer(partialBuffer);
//          // The last buffer of 'dest' contains only bytes 11-16 from
//          // 'blob.buffer(0)'.
//  }
//..
//
///Example 2: Data-Oriented Manipulation of a Blob
///- - - - - - - - - - - - - - - - - - - - - - - -
// There are several typical ways of manipulating a blob: the simplest lets the
// blob automatically manage the length, by using only 'prependBuffer',
// 'appendBuffer', and 'insertBuffer'.  Consider the following typical
// utilities (these utilities are to illustrate usage, they are not meant to be
// copy-pasted into application programs although they can provide a foundation
// for application utilities):
//..
//  void prependProlog(btlb::Blob         *blob,
//                     const bsl::string&  prolog,
//                     bslma::Allocator   *allocator = 0);
//      // Prepend the specified 'prolog' to the specified 'blob', using the
//      // optionally specified 'allocator' to supply any memory (or the
//      // currently installed default allocator if 'allocator' is 0).  The
//      // behavior is undefined unless 'blob' points to an initialized
//      // 'btlb::Blob' instance.
//
//  template <class DELETER>
//  void composeMessage(btlb::Blob         *blob,
//                      const bsl::string&  prolog,
//                      char * const       *vectors,
//                      const int          *vectorSizes,
//                      int                 numVectors,
//                      const DELETER&      deleter,
//                      bslma::Allocator   *allocator = 0);
//      // Load into the specified 'blob' the data composed of the specified
//      // 'prolog' and of the payload in the 'numVectors' buffers pointed to
//      // by the specified 'vectors' of the respective 'vectorSizes'.
//      // Ownership of the vectors is transferred to the 'blob' which will use
//      // the specified 'deleter' to destroy them.  Use the optionally
//      // specified 'allocator' to supply memory, or the currently installed
//      // default allocator if 'allocator' is 0.  Note that any buffer
//      // belonging to 'blob' prior to composing the message is not longer in
//      // 'blob' after composing the message.  Note also that 'blob' need not
//      // have been created with a blob buffer factory.  The behavior is
//      // undefined unless 'blob' points to an initialized 'btlb::Blob'
//      // instance.
//
//  int timestampMessage(btlb::Blob *blob, bslma::Allocator *allocator = 0);
//      // Insert a timestamp data buffer immediately after the prolog buffer
//      // and prior to any payload buffer.  Return the number of bytes
//      // inserted.  Use the optionally specified 'allocator' to supply
//      // memory, or the currently installed default allocator if 'allocator'
//      // is 0.  The behavior is undefined unless the specified 'blob' points
//      // to an initialized 'btlb::Blob' instance with at least one data
//      // buffer.
//..
// A possible implementation using only 'prependBuffer', 'appendBuffer', and
// 'insertBuffer' could be as follows:
//..
//  void prependProlog(btlb::Blob         *blob,
//                     const bsl::string&  prolog,
//                     bslma::Allocator   *allocator)
//  {
//      BSLS_ASSERT(blob);
//
//      (void)allocator;
//
//      int prologLength = prolog.length();
//      SimpleBlobBufferFactory fa(prologLength + sizeof(int));
//      btlb::BlobBuffer prologBuffer;
//      fa.allocate(&prologBuffer);
//
//      bslx::MarshallingUtil::putInt32(prologBuffer.data(), prologLength);
//      bsl::memcpy(prologBuffer.data() + sizeof(int),
//                  prolog.c_str(),
//                  prologLength);
//      BSLS_ASSERT(prologBuffer.size() == prologLength + sizeof(int));
//
//      blob->prependDataBuffer(prologBuffer);
//  }
//..
// Note that the length of 'blob' in the above implementation is automatically
// incremented by 'prologBuffer.size()'.  Consider instead:
//..
//      blob->insertBuffer(0, prologBuffer);
//..
// which inserts the prologBuffer before the first buffer of 'blob'.  This call
// will almost always adjust the length properly *except* if the length of
// 'blob' is 0 before the insertion (i.e., the message has an empty payload).
// In that case, the resulting 'blob' will still be empty after
// 'prependProlog', which, depending on the intention of the programmer, could
// be intended (avoid sending empty messages) or could be (most likely) a
// mistake.
//
// The 'composeMessage' implementation is simplified by using 'prependProlog':
//..
//  template <class DELETER>
//  void composeMessage(btlb::Blob         *blob,
//                      const bsl::string&  prolog,
//                      char * const       *vectors,
//                      const int          *vectorSizes,
//                      int                 numVectors,
//                      const DELETER&      deleter,
//                      bslma::Allocator   *allocator)
//  {
//      BSLS_ASSERT(blob);
//      BSLS_ASSERT(vectors);
//      BSLS_ASSERT(0 <= numVectors);
//
//      blob->removeAll();
//      prependProlog(blob, prolog, allocator);
//
//      for (int i = 0; i < numVectors; ++i) {
//          bsl::shared_ptr<char> shptr(vectors[i], deleter, allocator);
//          btlb::BlobBuffer partialBuffer(shptr, vectorSizes[i]);
//          blob->appendDataBuffer(partialBuffer);
//              // The last buffer of 'dest' contains only bytes 11-16 from
//              // 'blob.buffer(0)'.
//      }
//  }
//..
// Note that the 'deleter' is used to destroy the buffers transferred by
// 'vectors', but not the prolog buffer.
//
// Timestamping a message is done by creating a buffer holding a timestamp, and
// inserting it after the prolog and before the payload of the message.  Note
// that in typical messages, timestamps would be part of the prolog itself, so
// this is a somewhat contrived example for exposition only.
//..
//  int timestampMessage(btlb::Blob *blob, bslma::Allocator *allocator)
//  {
//      BSLS_ASSERT(blob);
//      BSLS_ASSERT(0 < blob->numDataBuffers());
//
//      btlb::BlobBuffer buffer;
//      bdlt::Datetime now = bdlt::CurrentTime::utc();
//
//      SimpleBlobBufferFactory fa(128, allocator);
//      btlb::BlobBuffer timestampBuffer;
//      fa.allocate(&timestampBuffer);
//
//      bslx::ByteOutStream bdexStream(20150826);
//      now.bdexStreamOut(bdexStream, 1);
//      BSLS_ASSERT(bdexStream);
//      BSLS_ASSERT(bdexStream.length() < 128);
//      bsl::memcpy(timestampBuffer.data(),
//                  bdexStream.data(),
//                  bdexStream.length());
//      timestampBuffer.setSize(bdexStream.length());
//..
// Now that we have fabricated the buffer holding the current data and time, we
// must insert it into the blob after the first buffer (i.e., before the buffer
// at index 1).  Note however that the payload could be empty, a condition
// tested by the fact that there is only one data buffer in 'blob'.  In that
// case, it would be a mistake to use 'insertBuffer' since it would not modify
// the length of the blob.
//..
//      if (1 < blob->numDataBuffers()) {
//          blob->insertBuffer(1, timestampBuffer);
//      } else {
//          blob->appendDataBuffer(timestampBuffer);
//      }
//
//      return bdexStream.length();
//  }
//..
// Note that the call to 'appendDataBuffer' also takes care of the possibility
// that the first buffer of 'blob' may not be full to capacity (if the length
// of the blob was smaller than the buffer size, only the first
// 'blob->length()' bytes would contain prolog data).  In that case, that
// buffer is trimmed before appending the 'timestampBuffer' so that the first
// byte of the 'timestampBuffer' appears immediately next to the last prolog
// byte, and the blob length is automatically incremented by the size of the
// 'timestampBuffer'.

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITBITWISEMOVEABLE
#include <bslalg_typetraitbitwisemoveable.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITS
#include <bslalg_typetraits.h>
#endif

#ifndef INCLUDED_BSLMA_ALLOCATOR
#include <bslma_allocator.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif

#ifndef INCLUDED_BSL_MEMORY
#include <bsl_memory.h>
#endif

#ifndef INCLUDED_BSL_VECTOR
#include <bsl_vector.h>
#endif

namespace BloombergLP {
namespace btlb {

                             // ================
                             // class BlobBuffer
                             // ================

class BlobBuffer {
    // 'BlobBuffer' is a simple in-core representation of a shared buffer.
    // This class is exception-neutral with no guarantee of rollback: if an
    // exception is thrown during the invocation of a method on a pre-existing
    // instance, the container is left in a valid state, but its value is
    // undefined.  In no event is memory leaked.

    // DATA
    bsl::shared_ptr<char> d_buffer;  // shared buffer
    int                   d_size;    // buffer size (in bytes)

    // FRIENDS
    friend bool operator==(const BlobBuffer&, const BlobBuffer&);

    friend bool operator!=(const BlobBuffer&, const BlobBuffer&);

  public:
    // CREATORS
    BlobBuffer();
        // Create a blob buffer representing a null buffer.  Note that the
        // 'size' and 'data' methods of a default-constructed blob buffer both
        // return 0.

    BlobBuffer(const bsl::shared_ptr<char>& buffer, int size);
        // Create a blob buffer representing the specified 'buffer' of the
        // specified 'size'.  Undefined unless '0 <= size'.

    BlobBuffer(const BlobBuffer& original);
        // Create a blob buffer having the same value as the specified
        // 'original' blob buffer.

    ~BlobBuffer();
        // Destroy this blob buffer.

    // MANIPULATORS
    BlobBuffer& operator=(const BlobBuffer& rhs);
        // Assign to this blob buffer the value of the specified 'rhs' blob
        // buffer, and return a reference to this modifiable blob buffer.

    void reset();
        // Reset this blob buffer to its default-constructed state.

    void reset(const bsl::shared_ptr<char>& buffer, int size);
        // Set the buffer represented by this object to the specified 'buffer'
        // of the specified 'size'.  Undefined unless '0 <= size'.

    bsl::shared_ptr<char>& buffer();
        // Return a reference to the shared pointer to the modifiable buffer
        // represented by this object.

    void setSize(int size);
        // Set the size of this blob buffer to the specified 'size'.  The
        // behavior is undefined unless '0 <= size' and the capacity of the
        // buffer returned by the 'buffer' method is at least 'size' bytes.

    // ACCESSORS
    const bsl::shared_ptr<char>& buffer() const;
        // Return a reference to the non-modifiable shared pointer to the
        // buffer represented by this object.

    char *data() const;
        // Return the address of the modifiable buffer represented by this
        // object.

    int size() const;
        // Return the size of the buffer represented by this object.

    bsl::ostream& print(bsl::ostream& stream,
                        int           level = 0,
                        int           spacesPerLevel = 4) const;
        // Format this object as a hexadecimal dump on the specified 'stream',
        // and return a reference to the modifiable 'stream'.  Note that the
        // optionally specified 'level' and 'spacesPerLevel' arguments are
        // specified for interface compatibility only and are effectively
        // ignored.
};
}  // close package namespace

// TYPE TRAITS

namespace bslmf {

template <>
struct IsBitwiseMoveable<BloombergLP::btlb::BlobBuffer>
    : IsBitwiseMoveable<bsl::shared_ptr<char> >::type
{};

}  // close namespace bslmf

namespace btlb {

// FREE OPERATORS
bool operator==(const BlobBuffer& lhs, const BlobBuffer& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' blob buffers have the
    // same value, and 'false' otherwise.  Two blob buffers have the same value
    // if they represent the same buffer of the same size.

bool operator!=(const BlobBuffer& lhs, const BlobBuffer& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' blob buffers do not have
    // the same value, and 'false' otherwise.  Two blob buffers do not have the
    // same value if they do not represent the same buffer of the same size.

bsl::ostream& operator<<(bsl::ostream& stream, const BlobBuffer& buffer);
    // Format the specified blob 'buffer' to the specified output 'stream', and
    // return a reference to the modifiable 'stream'.

                         // =======================
                         // class BlobBufferFactory
                         // =======================

class BlobBufferFactory {
    // This class defines a base-level protocol for a 'BlobBuffer' factory.

  public:
    // CREATORS
    virtual ~BlobBufferFactory();
        // Destroy this blob buffer factory.

    // MANIPULATORS
    virtual void allocate(BlobBuffer *buffer) = 0;
        // Allocate a blob buffer from this blob buffer factory, and load it
        // into the specified 'buffer'.
};

                                // ==========
                                // class Blob
                                // ==========

class Blob {
    // 'Blob' is an in-core container for 'BlobBuffer' objects.  This class is
    // exception-neutral with no guarantee of rollback: if an exception is
    // thrown during the invocation of a method on a pre-existing instance, the
    // container is left in a valid state, but its value is undefined.  In no
    // event is memory leaked.

    // DATA
    bsl::vector<BlobBuffer>  d_buffers;             // buffer sequence

    int                            d_totalSize;           // capacity of blob
                                                          // (in bytes)

    int                            d_dataLength;          // length (in bytes)
                                                          // of user-managed
                                                          // data

    int                            d_dataIndex;           // index of the last
                                                          // data buffer

    int                            d_preDataIndexLength;  // sum of the lengths
                                                          // of all data
                                                          // buffers, excluding
                                                          // the last one

    BlobBufferFactory       *d_bufferFactory_p;     // factory used to
                                                          // grow blob (held)

    // FRIENDS
    friend bool operator==(const Blob&, const Blob&);
    friend bool operator!=(const Blob&, const Blob&);

  private:
    // PRIVATE MANIPULATORS
    void slowSetLength(int length);
        // "Slow" setLength.

    // PRIVATE ACCESSORS
    int assertInvariants() const;
        // Assert the invariants of this object and return 0 on success.

  public:
    // CREATORS
    explicit Blob(bslma::Allocator *basicAllocator = 0);
        // Create an empty blob having no factory to allocate blob buffers.
        // Since there is no factory, the behavior is undefined if the length
        // of the blob is set beyond the total size.  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.

    explicit Blob(BlobBufferFactory *factory,
                  bslma::Allocator  *basicAllocator = 0);
        // Create an empty blob using the specified 'factory' to allocate blob
        // buffers.  Optionally specify a 'basicAllocator' used to supply
        // memory.  If 'basicAllocator' is 0, the currently installed default
        // allocator is used.

    Blob(const BlobBuffer  *buffers,
         int                numBuffers,
         BlobBufferFactory *factory,
         bslma::Allocator  *basicAllocator = 0);
        // Create a blob that initially holds the specified 'numBuffers'
        // buffers referenced by the specified 'buffers', and uses the
        // specified 'factory' to allocate blob buffers.  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.

    Blob(const Blob&        original,
         BlobBufferFactory *factory,
         bslma::Allocator  *basicAllocator = 0);
        // Create a blob that holds the same buffers as the specified
        // 'original' blob, and uses the specified 'factory' to allocate blob
        // buffers.  Optionally specify a 'basicAllocator' used to supply
        // memory.  If 'basicAllocator' is 0, the currently installed default
        // allocator is used.

    Blob(const Blob& original, bslma::Allocator *basicAllocator = 0);
        // Create a blob that holds the same buffers as the specified
        // 'original' blob, and has no factory to allocate blob buffers.  Since
        // there is no factory, the behavior is undefined if the length of the
        // blob is set beyond the total size.  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.

    ~Blob();
        // Destroy this blob.

    // MANIPULATORS
    Blob& operator=(const Blob& rhs);
        // Assign to this blob the value of the specified 'rhs' blob, and
        // return a reference to this modifiable blob.

    void appendBuffer(const BlobBuffer& buffer);
        // Append the specified 'buffer' after the last buffer of this blob.
        // The length of this blob is unaffected.  Note that this operation is
        // equivalent to 'insert(numBuffers(), buffer)', but is more efficient.

    void appendDataBuffer(const BlobBuffer& buffer);
        // Append the specified 'buffer' after the last *data* buffer of this
        // blob; the last data buffer is trimmed, if necessary.  The length of
        // this blob is incremented by the size of 'buffer'.  The behavior is
        // undefined unless '0 < buffer.size()'.  Note that this operation is
        // equivalent to:
        //..
        //  const int n = blob.length();
        //  blob.trimLastDataBuffer();
        //  blob.insert(numDataBuffers(), buffer);
        //  blob.setLength(n + buffer.size());
        //..
        // but is more efficient.

    void insertBuffer(int index, const BlobBuffer& buffer);
        // Insert the specified 'buffer' at the specified 'index' in this blob.
        // Increment the length of this blob by the size of 'buffer' if
        // 'buffer' is inserted *before* the logical end of this blob.  The
        // length of this blob is _unchanged_ if inserting at a position
        // following all data buffers (e.g., inserting into an empty blob or
        // inserting a buffer to increase capacity); in that case, the blob
        // length must be changed by an explicit call to 'setLength'.  Buffers
        // at 'index' and higher positions (if any) are shifted up by one index
        // position.  The behavior is undefined unless
        // '0 <= index <= numBuffers()'.

    void prependDataBuffer(const BlobBuffer& buffer);
        // Insert the specified 'buffer' before the beginning of this blob.
        // The length of this blob is incremented by the length of the
        // prepended buffer.  The behavior is undefined unless
        // '0 < buffer.size()'.  Note that this operation is equivalent to:
        //..
        //  const int n = blob.length();
        //  blob.insert(0, buffer);
        //  blob.setLength(n + buffer.size());
        //..
        // but is more efficient.

    void removeAll();
        // Remove all blob buffers from this blob, and set its length to 0.

    void removeBuffer(int index);
        // Remove the buffer at the specified 'index' from this blob, and
        // decrement the length of this blob by the size of 'buffer' if the
        // buffer at 'index' contains data bytes (i.e., if the first byte of
        // 'buffer' occurs before the logical end of this blob).  Buffers at
        // positions higher than 'index' (if any) are shifted down by one index
        // position.  The behavior is undefined unless
        // '0 <= index < numBuffers()'.

    void reserveBufferCapacity(int numBuffers);
        // Allocate sufficient capacity to store at least the specified
        // 'numBuffers' buffers.  The behavior is undefined unless
        // '0 <= numBuffers'.  Note that this method does not change the length
        // of this blob or add any buffers to it.

    void setLength(int length);
        // Set the length of this blob to the specified 'length' and, if
        // 'length' is greater than its total size, grow this blob by appending
        // buffers allocated using this object's underlying
        // 'BlobBufferFactory'.  The behavior is undefined if 'length' is a
        // negative value, or if the new length requires growing the blob and
        // this blob has no underlying factory.

    void swapBufferRaw(int index, BlobBuffer *srcBuffer);
        // Swap the blob buffer at the specified 'index' with the specified
        // 'srcBuffer'.  The behavior is undefined unless
        // '0 <= index < numBuffers()' and
        // 'srcBuffer->size() == buffer(index).size()'.  Note that other than
        // the buffer swap the state of this object remains unchanged.

    void trimLastDataBuffer();
        // Set the size of the last data buffer to 'lastDataBufferLength()'.
        // If there are no data buffers, or if the last data buffer is full
        // (i.e., its size is 'lastDataBufferLength()'), then this method has
        // no effect.  Note that the length of the blob is unchanged, and that
        // capacity buffers (i.e., of indices 'numDataBuffers()' and higher)
        // are *not* removed.

    void moveBuffers(Blob *srcBlob);
        // Remove all blob buffers from this blob and move the buffers held by
        // the specified 'srcBlob' to this blob.  Note that this method is
        // logically equivalent to:
        //..
        //  *this = *srcBlob;
        //  srcBlob->removeAll();
        //..
        // but its implementation is more efficient.

    void moveDataBuffers(Blob *srcBlob);
        // Remove all blob buffers from this blob and move the data buffers
        // held by the specified 'srcBlob' to this blob.

    void moveAndAppendDataBuffers(Blob *srcBlob);
        // Move the data buffers held by the specified 'srcBlob' to this blob
        // appending them to the current data buffers of this blob.

    // ACCESSORS
    const BlobBuffer& buffer(int index) const;
        // Return a reference to the non-modifiable blob buffer at the
        // specified 'index' in this blob.  The behavior is undefined unless
        // '0 <= index < numBuffers()'.

    int lastDataBufferLength() const;
        // Return the length of the last blob buffer in this blob, or 0 if this
        // blob is of 0 length.

    int length() const;
        // Return the length of this blob.

    int numDataBuffers() const;
        // Return the number of blob buffers containing data in this blob.

    int numBuffers() const;
        // Return the number of blob buffers held by this blob.

    int totalSize() const;
        // Return the sum of the sizes of all blob buffers in this blob (i.e.,
        // the capacity of this blob).
};
}  // close package namespace

// TYPE TRAITS

namespace bslmf {

template <>
struct IsBitwiseMoveable<BloombergLP::btlb::Blob>
    : IsBitwiseMoveable<bsl::vector<BloombergLP::btlb::BlobBuffer> >::type
{};
}  // close namespace bslmf

namespace bslma {

template <>
struct UsesBslmaAllocator<BloombergLP::btlb::Blob> : bsl::true_type
{};
}  // close namespace bslma

namespace btlb {

// FREE OPERATORS
bool operator==(const Blob& lhs, const Blob& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' blobs have the same
    // value, and 'false' otherwise.  Two blobs have the same value if they
    // hold the same buffers, and have the same length.

bool operator!=(const Blob& lhs, const Blob& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' blobs do not have the
    // same value, and 'false' otherwise.  Two blobs do not have the same value
    // if they do not hold the same buffers, or do not have the same length.

// ============================================================================
//                             INLINE DEFINITIONS
// ============================================================================

                             // ================
                             // class BlobBuffer
                             // ================

// CREATORS
inline
BlobBuffer::BlobBuffer()
: d_size(0)
{
}

inline
BlobBuffer::BlobBuffer(const bsl::shared_ptr<char>& buffer,
                       int                          size)
: d_buffer(buffer)
, d_size(size)
{
    BSLS_ASSERT(0 <= size);
}

inline
BlobBuffer::BlobBuffer(const BlobBuffer& original)
: d_buffer(original.d_buffer)
, d_size(original.d_size)
{
}

inline
BlobBuffer::~BlobBuffer()
{
}

// MANIPULATORS
inline
bsl::shared_ptr<char>& BlobBuffer::buffer()
{
    return d_buffer;
}

inline
void BlobBuffer::setSize(int size)
{
    BSLS_ASSERT_SAFE(0 <= size);

    d_size = size;
}

// ACCESSORS
inline
const bsl::shared_ptr<char>& BlobBuffer::buffer() const
{
    return d_buffer;
}

inline
char *BlobBuffer::data() const
{
    return d_buffer.get();
}

inline
int BlobBuffer::size() const
{
    return d_size;
}
}  // close package namespace

// FREE OPERATORS
inline
bool btlb::operator==(const BlobBuffer& lhs, const BlobBuffer& rhs)
{
    return lhs.d_buffer.get() == rhs.d_buffer.get()
        && lhs.d_size         == rhs.d_size;
}

inline
bool btlb::operator!=(const BlobBuffer& lhs, const BlobBuffer& rhs)
{
    return lhs.d_buffer.get() != rhs.d_buffer.get()
        || lhs.d_size         != rhs.d_size;
}

namespace btlb {

                                // ==========
                                // class Blob
                                // ==========

// MANIPULATORS
inline
void Blob::reserveBufferCapacity(int numBuffers)
{
    BSLS_ASSERT_SAFE(0 <= numBuffers);

    d_buffers.reserve(numBuffers);
}

// ACCESSORS
inline
const BlobBuffer& Blob::buffer(int index) const
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(     index < static_cast<int>(d_buffers.size()));

    return d_buffers[index];
}

inline
int Blob::lastDataBufferLength() const
{
    return d_dataLength - d_preDataIndexLength;
}

inline
int Blob::length() const
{
    return d_dataLength;
}

inline
int Blob::numBuffers() const
{
    return static_cast<int>(d_buffers.size());
}

inline
int Blob::numDataBuffers() const
{
    return d_dataLength != 0 ? d_dataIndex + 1 : 0;
}

inline
int Blob::totalSize() const
{
    return d_totalSize;
}

}  // close package namespace
}  // close enterprise namespace

#endif

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
