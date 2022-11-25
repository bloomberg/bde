// bdlbb_blob.h                                                       -*-C++-*-
#ifndef INCLUDED_BDLBB_BLOB
#define INCLUDED_BDLBB_BLOB

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide an indexed set of buffers from multiple sources.
//
//@CLASSES:
//  bdlbb::BlobBuffer: in-core representation of a shared buffer
//  bdlbb::BlobBufferFactory: factory of blob buffers
//  bdlbb::Blob: indexed sequence of buffers
//
//@SEE_ALSO: bslstl_sharedptr, bdlbb_pooledblobbufferfactory
//
//@DESCRIPTION: This component provides an indexed sequence ('bdlbb::Blob') of
// 'bdlbb::BlobBuffer' objects allocated from potentially multiple
// 'bdlbb::BlobBufferFactory' objects.  A 'bdlbb::BlobBuffer' is a simple
// in-core value object owning a shared pointer to a memory buffer.  Therefore,
// the lifetime of the underlying memory is determined by shared ownership
// between the blob buffer, the blob(s) that may contain it, and any other
// entities that may share ownership of the memory buffer.
//
// Logically, a 'bdlbb::Blob' can be thought of as a sequence of bytes
// (although not contiguous).  Each buffer in a blob contributes its own size
// to the blob, with the total size of a blob being the sum of sizes over all
// its buffers.  A prefix of these bytes, collectively referred to as the data
// of the blob, are defined by the data length, which can be set by the user
// using the 'setLength' method.  Note that the data length never exceeds the
// total size.  When setting the length to a value greater than the total size,
// the latter is increased automatically by adding buffers created from a
// factory passed at construction; the behavior is undefined if no factory was
// supplied at construction.
//
// The blob also updates its data length during certain operations (e.g.,
// insertion/removal/replacement of buffers containing some data bytes), as
// well as several attributes driven by the data length.  The first bytes
// numbered by the data length belong to the data buffers.  Note that all data
// buffers, except perhaps the last, contribute all their bytes to the
// 'bdlbb::Blob' data.  The last data buffer contributes anywhere between one
// and all of its bytes to the 'bdlbb::Blob' data.  The number of data buffers
// (returned by the 'numDataBuffers' method), as well as the last data buffer
// length (returned by 'lastDataBufferLength'), are maintained by 'bdlbb::Blob'
// automatically when setting the length to a new value.
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
// of resources (via retaining capacity).  Thus, 'bdlbb::Blob' is an
// advantageous replacement for 'bdlbb::PooledBufferChain' when manipulation of
// the sequence, sharing of portions of the sequence, and lifetime management
// of individual portions of the sequence, are desired.  Another added
// flexibility of 'bdlbb::Blob' is the possibility for buffers in the sequence
// to have different sizes (as opposed to a uniform fixed size for
// 'bdlbb::PooledBufferChain').  When choosing whether to use a 'bdlbb::Blob'
// vs. a 'bdlbb::PooledBufferChain', one must consider the added flexibility
// versus the added cost of shared ownership for each individual buffer and
// random access to the buffer.
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
// Classes that implement the 'bdlbb::BlobBufferFactory' protocol are used to
// allocate 'bdlbb::BlobBuffer' objects.  A simple implementation follows:
//..
//  class SimpleBlobBufferFactory : public bdlbb::BlobBufferFactory {
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
//      void allocate(bdlbb::BlobBuffer *buffer);
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
//  void SimpleBlobBufferFactory::allocate(bdlbb::BlobBuffer *buffer)
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
// 'bdlbb_pooledblobbufferfactory' component.
//
///Simple Blob Usage
///- - - - - - - - -
// Blobs can be created just by passing a factory that is responsible to
// allocate the 'bdlbb::BlobBuffer'.  The following simple program illustrates
// how.
//..
//  {
//      SimpleBlobBufferFactory myFactory(1024);
//
//      bdlbb::Blob blob(&myFactory);
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
// A 'bdlbb::BlobBuffer' can easily be re-assigned from one blob to another
// with no copy.  In that case, the memory held by the buffer will be returned
// to its factory when the last blob referencing the buffer is destroyed.  For
// the following example, a blob will be created using the default constructor.
// In this case, the 'bdlbb::Blob' object will not able to grow on its own.
// Calling 'setLength' for a number equal or greater than 'totalSize()' will
// result in undefined behavior.
//..
//      bdlbb::Blob dest;
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
// if the logical length of the 'bdlbb::Blob' must be changed:
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
//      bdlbb::BlobBuffer partialBuffer(shptr, 6);
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
//  void prependProlog(bdlbb::Blob        *blob,
//                     const char         *prolog,
//                     int                 length,
//                     bslma::Allocator   *allocator = 0);
//      // Prepend the specified 'prolog' of the specified 'length' to the
//      // specified 'blob', using the optionally specified 'allocator' to
//      // supply any memory (or the currently installed default allocator if
//      // 'allocator' is 0).  The behavior is undefined unless
//      // 'blob->totalSize() <= INT_MAX - length - sizeof(int)' and
//      // 'blob->numBuffers() < INT_MAX'.
//
//  template <class DELETER>
//  void composeMessage(bdlbb::Blob        *blob,
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
//      // undefined unless 'blob' points to an initialized 'bdlbb::Blob'
//      // instance.
//
//  int timestampMessage(bdlbb::Blob *blob, bslma::Allocator *allocator = 0);
//      // Insert a timestamp data buffer immediately after the prolog buffer
//      // and prior to any payload buffer.  Return the number of bytes
//      // inserted.  Use the optionally specified 'allocator' to supply
//      // memory, or the currently installed default allocator if 'allocator'
//      // is 0.  The behavior is undefined unless the specified 'blob' points
//      // to an initialized 'bdlbb::Blob' instance with at least one data
//      // buffer.
//..
// A possible implementation using only 'prependBuffer', 'appendBuffer', and
// 'insertBuffer' could be as follows:
//..
//  void prependProlog(bdlbb::Blob        *blob,
//                     const char         *prolog,
//                     int                 length,
//                     bslma::Allocator   *allocator)
//  {
//      assert(blob);
//      assert(blob->totalSize() <=
//                           INT_MAX - length - static_cast<int>(sizeof(int)));
//      assert(blob->numBuffers() < INT_MAX);
//
//      (void)allocator;
//
//      int                     prologBufferSize =
//                                      static_cast<int>(length + sizeof(int));
//      SimpleBlobBufferFactory fa(prologBufferSize);
//      bdlbb::BlobBuffer       prologBuffer;
//      fa.allocate(&prologBuffer);
//
//      bslx::MarshallingUtil::putInt32(prologBuffer.data(), length);
//      bsl::memcpy(prologBuffer.data() + sizeof(int),
//                  prolog,
//                  length);
//      assert(prologBuffer.size() == prologBufferSize);
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
//  void composeMessage(bdlbb::Blob        *blob,
//                      const char         *prolog,
//                      int                 prologLength,
//                      char * const       *vectors,
//                      const int          *vectorSizes,
//                      int                 numVectors,
//                      const DELETER&      deleter,
//                      bslma::Allocator   *allocator)
//  {
//      assert(blob);
//      assert(vectors);
//      assert(0 <= numVectors);
//
//      blob->removeAll();
//      prependProlog(blob, prolog, prologLength, allocator);
//
//      for (int i = 0; i < numVectors; ++i) {
//          bsl::shared_ptr<char> shptr(vectors[i], deleter, allocator);
//          bdlbb::BlobBuffer partialBuffer(shptr, vectorSizes[i]);
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
//  int timestampMessage(bdlbb::Blob *blob, bslma::Allocator *allocator)
//  {
//      assert(blob);
//      assert(0 < blob->numDataBuffers());
//
//      bdlbb::BlobBuffer buffer;
//      bdlt::Datetime now = bdlt::CurrentTime::utc();
//
//      SimpleBlobBufferFactory fa(128, allocator);
//      bdlbb::BlobBuffer timestampBuffer;
//      fa.allocate(&timestampBuffer);
//
//      bslx::ByteOutStream bdexStream(20150826);
//      now.bdexStreamOut(bdexStream, 1);
//      assert(bdexStream);
//      assert(bdexStream.length() < 128);
//      bsl::memcpy(timestampBuffer.data(),
//                  bdexStream.data(),
//                  bdexStream.length());
//      timestampBuffer.setSize(static_cast<int>(bdexStream.length()));
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
//      return static_cast<int>(bdexStream.length());
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

#include <bdlscm_version.h>

#include <bslma_allocator.h>

#include <bslmf_isbitwisemoveable.h>
#include <bslmf_movableref.h>

#include <bsls_assert.h>
#include <bsls_keyword.h>
#include <bsls_review.h>

#include <bsl_iosfwd.h>
#include <bsl_memory.h>
#include <bsl_vector.h>

#ifndef BDE_DONT_ALLOW_TRANSITIVE_INCLUDES
#include <bslalg_typetraits.h>
#endif  // BDE_DONT_ALLOW_TRANSITIVE_INCLUDES

namespace BloombergLP {
namespace bdlbb {

                              // ================
                              // class BlobBuffer
                              // ================

class BlobBuffer {
    // 'BlobBuffer' is a simple in-core representation of a shared buffer.
    // This class is exception-neutral with no guarantee of rollback: if an
    // exception is thrown during the invocation of a method on a pre-existing
    // instance, the container is left in a valid state, but its value is
    // undefined.  In no event is memory leaked.

    // PRIVATE TYPES
    typedef bslmf::MovableRefUtil MoveUtil;
        // Used in move construction and assignment to make lines shorter.

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
        // specified 'size'.  The behavior is undefined unless '0 <= size' and
        // the 'buffer' refers to a continuous block of memory of at least
        // 'size' bytes.

    BlobBuffer(bslmf::MovableRef<bsl::shared_ptr<char> > buffer, int size);
        // Create a blob buffer representing the specified moveable 'buffer' of
        // the specified 'size'.  The behavior is undefined unless '0 <= size'
        // and the 'buffer' refers to a continuous block of memory of at least
        // 'size' bytes.

    BlobBuffer(const BlobBuffer& original);
        // Create a blob buffer having the same value as the specified
        // 'original' blob buffer.

    BlobBuffer(bslmf::MovableRef<BlobBuffer> original) BSLS_KEYWORD_NOEXCEPT;
        // Create a blob buffer object having the same value as the specified
        // 'original' object by moving the contents of 'original' to the
        // newly-created object.  'original' is left in a valid but unspecified
        // state.

    ~BlobBuffer();
        // Destroy this blob buffer.

    // MANIPULATORS
    BlobBuffer& operator=(const BlobBuffer& rhs);
        // Assign to this blob buffer the value of the specified 'rhs' blob
        // buffer, and return a reference to this modifiable blob buffer.

    BlobBuffer& operator=(bslmf::MovableRef<BlobBuffer> rhs);
        // Assign to this object the value of the specified 'rhs', and return a
        // reference providing modifiable access to this object.  The contents
        // of 'rhs' are move-assigned to this object.  'rhs' is left in a valid
        // but unspecified state.

    void reset();
        // Reset this blob buffer to its default-constructed state.

    void reset(const bsl::shared_ptr<char>& buffer, int size);
        // Set the buffer represented by this object to the specified 'buffer'
        // of the specified 'size'.  The behavior is undefined unless
        // '0 <= size' and the 'buffer' refers to a continuous block of memory
        // of at least 'size' bytes.

    void reset(bslmf::MovableRef<bsl::shared_ptr<char> > buffer, int size);
        // Set the buffer represented by this object to the specified moveable
        // 'buffer' of the specified 'size'.  The behavior is undefined unless
        // '0 <= size' and the 'buffer' refers to a continuous block of memory
        // of at least 'size' bytes.

    bsl::shared_ptr<char>& buffer();
        // Return a reference to the shared pointer to the modifiable buffer
        // represented by this object.

    void setSize(int size);
        // Set the size of this blob buffer to the specified 'size'.  The
        // behavior is undefined unless '0 <= size' and the capacity of the
        // buffer returned by the 'buffer' method is at least 'size' bytes.

    void swap(BlobBuffer& other);
        // Efficiently exchange the value of this object with the value of the
        // specified 'other' object.  This method provides the no-throw
        // exception-safety guarantee.

    BlobBuffer trim(int toSize);
        // Reduce this buffer to the specified 'toSize' and return the
        // leftover.  The behaviour is undefined unless '0 <= toSize && toSize
        // <= size()'.

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
                        int           level          = 0,
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
struct IsBitwiseMoveable<BloombergLP::bdlbb::BlobBuffer>
: IsBitwiseMoveable<bsl::shared_ptr<char> >::type {
};

}  // close namespace bslmf

namespace bdlbb {

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

// FREE FUNCTIONS
void swap(BlobBuffer& a, BlobBuffer& b);
    // Efficiently exchange the values of the specified 'a' and 'b' objects.
    // This method provides the no-throw exception-safety guarantee.

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

    // PRIVATE TYPES
    typedef bslmf::MovableRefUtil MoveUtil;
        // Used in move construction and assignment to make lines shorter.

    // DATA
    bsl::vector<BlobBuffer>  d_buffers;             // buffer sequence

    int                      d_totalSize;           // capacity of blob (in
                                                    // bytes)

    int                      d_dataLength;          // length (in bytes) of
                                                    // user-managed data

    int                      d_dataIndex;           // index of the last data
                                                    // buffer, or -1 if the
                                                    // blob has no data buffers

    int                      d_preDataIndexLength;  // sum of the lengths of
                                                    // all data buffers,
                                                    // excluding the last one

    BlobBufferFactory       *d_bufferFactory_p;     // factory used to grow
                                                    // blob (held)

    // FRIENDS
    friend bool operator==(const Blob&, const Blob&);
    friend bool operator!=(const Blob&, const Blob&);

  private:
    // PRIVATE MANIPULATORS
    void slowSetLength(int length);
        // Set the length of this blob to the specified 'length' and, if
        // 'length' is greater than its total size, grow this blob by appending
        // buffers allocated using this object's underlying
        // 'BlobBufferFactory'.  This function implements the "slow-path" for
        // 'setLength', handling the cases where the supplied 'length' is lies
        // beyond the boundaries of the last data buffer. The behavior is
        // undefined if 'length' is a negative value, if the new length
        // requires growing the blob and this blob has no underlying factory,
        // or if the 'length' lies within the boundaries of the last data
        // buffer.

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

    Blob(bslmf::MovableRef<Blob> original) BSLS_KEYWORD_NOEXCEPT;
        // Create a blob object having the same value as the specified
        // 'original' object by moving the contents of 'original' to the
        // newly-created object.  The allocator associated with 'original' is
        // propagated for use in the newly-created object.  'original' is left
        // in a valid but unspecified state.

    Blob(bslmf::MovableRef<Blob>  original,
         bslma::Allocator        *basicAllocator);
        // Create a blob object having the same value as the specified
        // 'original' object that uses the specified 'basicAllocator' to supply
        // memory.  If 'basicAllocator' is 0, the currently installed default
        // allocator is used.  The contents of 'original' are moved to the
        // newly-created object.  'original' is left in a valid but unspecified
        // state.

    ~Blob();
        // Destroy this blob.

    // MANIPULATORS
    Blob& operator=(const Blob& rhs);
        // Assign to this blob the value of the specified 'rhs' blob, and
        // return a reference to this modifiable blob.

    Blob& operator=(bslmf::MovableRef<Blob> rhs);
        // Assign to this object the value of the specified 'rhs', and return a
        // reference providing modifiable access to this object.  The contents
        // of 'rhs' are move-assigned to this object.  'rhs' is left in a valid
        // but unspecified state.

    void appendBuffer(const BlobBuffer& buffer);
        // Append the specified 'buffer' after the last buffer of this blob.
        // The length of this blob is unaffected.  The behavior is undefined
        // unless neither the total size of the resulting blob nor its total
        // number of buffers exceeds 'INT_MAX'.  Note that this operation is
        // equivalent to 'insertBuffer(numBuffers(), buffer)', but is more
        // efficient.

    void appendBuffer(bslmf::MovableRef<BlobBuffer> buffer);
        // Append the specified move-insertable 'buffer' after the last buffer
        // of this blob.  The 'buffer' is left in a valid but unspecified
        // state.  The length of this blob is unaffected.  The behavior is
        // undefined unless neither the total size of the resulting blob nor
        // its total number of buffers exceeds 'INT_MAX'.  Note that this
        // operation is equivalent to 'insertBuffer(numBuffers(), buffer)', but
        // is more efficient.

    void appendDataBuffer(const BlobBuffer& buffer);
        // Append the specified 'buffer' after the last *data* buffer of this
        // blob; the last data buffer is trimmed, if necessary.  The length of
        // this blob is incremented by the size of 'buffer'.  The behavior is
        // undefined unless neither the total size of the resulting blob nor
        // its total number of buffers exceeds 'INT_MAX'.  Note that this
        // operation is equivalent to:
        //..
        //  const int n = blob.length();
        //  blob.trimLastDataBuffer();
        //  blob.insertBuffer(numDataBuffers(), buffer);
        //  blob.setLength(n + buffer.size());
        //..
        // but is more efficient.

    void appendDataBuffer(bslmf::MovableRef<BlobBuffer> buffer);
        // Append the specified move-insertable 'buffer' after the last *data*
        // buffer of this blob; the last data buffer is trimmed, if necessary.
        // The 'buffer' is left in a valid but unspecified state.  The length
        // of this blob is incremented by the size of 'buffer'.  The behavior
        // is undefined unless neither the total size of the resulting blob nor
        // its total number of buffers exceeds 'INT_MAX'.  Note that this
        // operation is equivalent to:
        //..
        //  const int n = blob.length();
        //  blob.trimLastDataBuffer();
        //  blob.insertBuffer(numDataBuffers(), MoveUtil::move(buffer));
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
        // '0 <= index <= numBuffers()' and neither the total size of the
        // resulting blob nor its total number of buffers exceeds 'INT_MAX'.

    void insertBuffer(int index, bslmf::MovableRef<BlobBuffer> buffer);
        // Insert the specified move-insertable 'buffer' at the specified
        // 'index' in this blob.  Increment the length of this blob by the size
        // of 'buffer' if 'buffer' is inserted *before* the logical end of this
        // blob.  The length of this blob is _unchanged_ if inserting at a
        // position following all data buffers (e.g., inserting into an empty
        // blob or inserting a buffer to increase capacity); in that case, the
        // blob length must be changed by an explicit call to 'setLength'.
        // Buffers at 'index' and higher positions (if any) are shifted up by
        // one index position.  The 'buffer' is left in a valid but unspecified
        // state.  The behavior is undefined unless
        // '0 <= index <= numBuffers()' and neither the total size of the
        // resulting blob nor its total number of buffers exceeds 'INT_MAX'.

    void prependDataBuffer(const BlobBuffer& buffer);
        // Insert the specified 'buffer' before the beginning of this blob.
        // The length of this blob is incremented by the length of the
        // prepended buffer.  The behavior is undefined unless neither the
        // total size of the resulting blob nor its total number of buffers
        // exceeds 'INT_MAX'.  Note that this operation is equivalent to:
        //..
        //  const int n = blob.length();
        //  blob.insertBuffer(0, buffer);
        //  blob.setLength(n + buffer.size());
        //..
        // but is more efficient.

    void prependDataBuffer(bslmf::MovableRef<BlobBuffer> buffer);
        // Insert the specified move-insertable 'buffer' before the beginning
        // of this blob.  The length of this blob is incremented by the length
        // of the prepended buffer.  The 'buffer' is left in a valid but
        // unspecified state.  The behavior is undefined unless neither the
        // total size of the resulting blob nor its total number of buffers
        // exceeds 'INT_MAX'.  Note that this operation is equivalent to:
        //..
        //  const int n = blob.length();
        //  blob.insertBuffer(0, MoveUtil::move(buffer));
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

    void removeBuffers(int index, int numBuffers);
        // Remove the specified 'numBuffers' starting at the specified 'index'
        // from this blob.  Buffers at positions higher than 'index' (if any)
        // are shifted down by 'numBuffers' index positions.  The behavior is
        // undefined unless '0 <= index', '0 <= numBuffers', and
        // 'index + numBuffers <= numBuffers()'.

    void removeUnusedBuffers();
        // Remove any unused capacity buffers from this blob.  Note that this
        // method does not trim the last data buffer, and that the resulting
        // 'totalSize' will be 'length' plus any unused capacity in the last
        // buffer having data.

    void replaceDataBuffer(int index, const BlobBuffer& buffer);
        // Replace the data buffer at the specified 'index' with the specified
        // 'buffer'.  The behavior is undefined unless
        // '0 <= index < numDataBuffers()' and the total size of the resulting
        // blob does not exceed 'INT_MAX'.  Note that this operation is
        // equivalent to:
        //..
        //  blob.removeBuffer(index);
        //  const int n = blob.length();
        //  blob.insertBuffer(index, buffer);
        //  blob.setLength(n + buffer.size());
        //..
        // but is more efficient.

    void reserveBufferCapacity(int numBuffers);
        // Allocate sufficient capacity to store at least the specified
        // 'numBuffers' buffers.  The behavior is undefined unless
        // '0 <= numBuffers'.  Note that this method does not change the length
        // of this blob or add any buffers to it.  Note also that the internal
        // capacity will be increased to maintain a geometric growth factor.

    void setLength(int length);
        // Set the length of this blob to the specified 'length' and, if
        // 'length' is greater than its total size, grow this blob by appending
        // buffers allocated using this object's underlying
        // 'BlobBufferFactory'.  The behavior is undefined if 'length' is a
        // negative value, or if the new length requires growing the blob and
        // this blob has no underlying factory.

    void swap(Blob& other);
        // Efficiently exchange the value of this object with the value of the
        // specified 'other' object.  This method provides the no-throw
        // exception-safety guarantee.  The behavior is undefined unless this
        // object was created with the same allocator as 'other'.

    void swapBufferRaw(int index, BlobBuffer *srcBuffer);
        // Swap the blob buffer at the specified 'index' with the specified
        // 'srcBuffer'.  The behavior is undefined unless
        // '0 <= index < numBuffers()' and
        // 'srcBuffer->size() == buffer(index).size()'.  Note that other than
        // the buffer swap the state of this object remains unchanged.

    BlobBuffer trimLastDataBuffer();
        // Set the size of the last data buffer to 'lastDataBufferLength()'.
        // If there are no data buffers, or if the last data buffer is full
        // (i.e., its size is 'lastDataBufferLength()'), then this method has
        // no effect.  Return the leftover of the trimmed buffer or default
        // constructed 'BlobBuffer' if nothing to trim.  Note that the length
        // of the blob is unchanged, and that capacity buffers (i.e., of
        // indices 'numDataBuffers()' and higher) are *not* removed.

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
        // appending them to the current data buffers of this blob.  The
        // behavior is undefined unless the total size of the resulting blob
        // and the total number of buffers in this blob are less than or
        // equal to 'INT_MAX'.

    // ACCESSORS
    bslma::Allocator *allocator() const;
        // Return the allocator used by this object to supply memory.

    const BlobBuffer& buffer(int index) const;
        // Return a reference to the non-modifiable blob buffer at the
        // specified 'index' in this blob.  The behavior is undefined unless
        // '0 <= index < numBuffers()'.

    BlobBufferFactory *factory() const;
        // Return the factory used by this object.

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
struct IsBitwiseMoveable<BloombergLP::bdlbb::Blob>
: IsBitwiseMoveable<bsl::vector<BloombergLP::bdlbb::BlobBuffer> >::type {
};
}  // close namespace bslmf

namespace bslma {

template <>
struct UsesBslmaAllocator<BloombergLP::bdlbb::Blob> : bsl::true_type {
};
}  // close namespace bslma

namespace bdlbb {

// FREE OPERATORS
bool operator==(const Blob& lhs, const Blob& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' blobs have the same
    // value, and 'false' otherwise.  Two blobs have the same value if they
    // hold the same buffers, and have the same length.

bool operator!=(const Blob& lhs, const Blob& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' blobs do not have the
    // same value, and 'false' otherwise.  Two blobs do not have the same value
    // if they do not hold the same buffers, or do not have the same length.

// FREE FUNCTIONS
void swap(Blob& a, Blob& b);
    // Efficiently exchange the values of the specified 'a' and 'b' objects.
    // This method provides the no-throw exception-safety guarantee if both
    // objects were created with the same allocator.

// ============================================================================
//                             INLINE DEFINITIONS
// ============================================================================

                              // ----------------
                              // class BlobBuffer
                              // ----------------

// CREATORS
inline
BlobBuffer::BlobBuffer()
: d_size(0)
{
}

inline
BlobBuffer::BlobBuffer(const bsl::shared_ptr<char>& buffer, int size)
: d_buffer(buffer)
, d_size(size)
{
    BSLS_ASSERT(0 <= size);
    BSLS_ASSERT(size == 0 || buffer);
}

inline
BlobBuffer::BlobBuffer(bslmf::MovableRef<bsl::shared_ptr<char> > buffer,
                       int                                       size)
: d_buffer(MoveUtil::move(buffer))
, d_size(size)
{
    BSLS_ASSERT(0 <= size);
    BSLS_ASSERT(size == 0 || d_buffer);
}

inline
BlobBuffer::BlobBuffer(const BlobBuffer& original)
: d_buffer(original.d_buffer)
, d_size(original.d_size)
{
}

inline
BlobBuffer::BlobBuffer(bslmf::MovableRef<BlobBuffer> original)
                                                          BSLS_KEYWORD_NOEXCEPT
: d_buffer(MoveUtil::move(MoveUtil::access(original).d_buffer))
, d_size(MoveUtil::move(MoveUtil::access(original).d_size))
{
    MoveUtil::access(original).d_size = 0;
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
    BSLS_ASSERT(0 <= size);

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
bool bdlbb::operator==(const BlobBuffer& lhs, const BlobBuffer& rhs)
{
    return lhs.d_buffer.get() == rhs.d_buffer.get() &&
           lhs.d_size == rhs.d_size;
}

inline
bool bdlbb::operator!=(const BlobBuffer& lhs, const BlobBuffer& rhs)
{
    return lhs.d_buffer.get() != rhs.d_buffer.get() ||
           lhs.d_size != rhs.d_size;
}

namespace bdlbb {

                                 // ----------
                                 // class Blob
                                 // ----------

// MANIPULATORS
inline
void Blob::appendBuffer(const BlobBuffer& buffer)
{
    BlobBuffer objectToMove(buffer);
    appendBuffer(MoveUtil::move(objectToMove));
}

inline
void Blob::appendDataBuffer(const BlobBuffer& buffer)
{
    BlobBuffer objectToMove(buffer);
    appendDataBuffer(MoveUtil::move(objectToMove));
}

inline
void Blob::insertBuffer(int index, const BlobBuffer& buffer)
{
    BlobBuffer objectToMove(buffer);
    insertBuffer(index, MoveUtil::move(objectToMove));
}

inline
void Blob::prependDataBuffer(const BlobBuffer& buffer)
{
    BlobBuffer objectToMove(buffer);
    prependDataBuffer(MoveUtil::move(objectToMove));
}

inline
void Blob::reserveBufferCapacity(int numBuffers)
{
    BSLS_ASSERT(0 <= numBuffers);

    typedef bsl::vector<BlobBuffer>::size_type size_t;
    size_t newCapacity = static_cast<size_t>(numBuffers);
    if (newCapacity > d_buffers.capacity()) {
        size_t geometric = d_buffers.capacity() * 2;
        newCapacity = geometric > newCapacity ? geometric : newCapacity;
        d_buffers.reserve(newCapacity);
    }
}

// ACCESSORS
inline
bslma::Allocator *Blob::allocator() const
{
    return d_buffers.get_allocator().mechanism();
}

inline
const BlobBuffer& Blob::buffer(int index) const
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(index < static_cast<int>(d_buffers.size()));

    return d_buffers[index];
}

inline
BlobBufferFactory *Blob::factory() const
{
    return d_bufferFactory_p;
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
    return d_dataIndex + 1;
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
// Copyright 2018 Bloomberg Finance L.P.
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
