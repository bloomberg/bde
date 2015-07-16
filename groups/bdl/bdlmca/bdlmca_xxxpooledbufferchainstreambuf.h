// bdlmca_xxxpooledbufferchainstreambuf.h                                 -*-C++-*-
#ifndef INCLUDED_BDLMCA_XXXPOOLEDBUFFERCHAINSTREAMBUF
#define INCLUDED_BDLMCA_XXXPOOLEDBUFFERCHAINSTREAMBUF

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide pooled buffer chain implementing the 'streambuf' interface.
//
//@CLASSES:
// bdlmca::PooledBufferChainStreamBuf: pooled buffer chain 'streambuf'
// bdlmca::PooledBufferChainStreamBufFactory: a default factory for 'streambuf's
//
//@AUTHOR: Vladimir Kliatchko (vkliatch)
//
//@SEE_ALSO: bdlmca_xxxpooledbufferchain
//
//@DESCRIPTION: This component implements the 'bsl::basic_streambuf' protocol
// using a user-supplied 'bdlmca::PooledBufferChain'.  Method names necessarily
// correspond to the protocol-specified method names.  Refer to the C++
// Standard, Section 27.5.2, for a full specification of the
// 'bsl::basic_streambuf' interface.
//
// A pooled buffer chain is an indexed sequence of fixed-size buffers.  The
// number of buffers in the sequence can increase or decrease, but the order of
// the buffers cannot change.  Therefore, the buffer chain behaves logically as
// a single indexed buffer.  'bdlmca::PooledBufferChainStreamBuf' can therefore
// read from and write to this buffer as if there were a single continuous
// index.

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

#ifndef INCLUDED_BDLMCA_XXXPOOLEDBUFFERCHAIN
#include <bdlmca_xxxpooledbufferchain.h>
#endif

#ifndef INCLUDED_BDLMA_FACTORY
#include <bdlma_factory.h>
#endif

#ifndef INCLUDED_BSLS_PERFORMANCEHINT
#include <bsls_performancehint.h>
#endif

#ifndef INCLUDED_BSL_IOS
#include <bsl_ios.h>
#endif

#ifndef INCLUDED_BSL_MEMORY
#include <bsl_memory.h>
#endif

#ifndef INCLUDED_BSL_STREAMBUF
#include <bsl_streambuf.h>
#endif

#ifndef INCLUDED_BSL_CSTRING
#include <bsl_cstring.h>
#endif

#ifndef INCLUDED_BSL_CSTDLIB
#include <bsl_cstdlib.h>
#endif

#ifndef BDE_DONT_ALLOW_TRANSITIVE_INCLUDES

#ifndef INCLUDED_BSL_MEMORY
#include <bsl_memory.h>
#endif

#endif

namespace BloombergLP {

namespace bdlmca {
                   //=======================================
                   // class PooledBufferChainStreamBuf
                   //=======================================

class PooledBufferChainStreamBuf : public bsl::streambuf {
    // This class implements the input functionality of the 'basic_streambuf'
    // protocol, using a client-supplied 'bdlmca::PooledBufferChain'.

    // PRIVATE TYPES
    typedef bsl::ios_base ios_base;

    // DATA
    bsl::shared_ptr<bdlmca::PooledBufferChain> d_pooledBufferChain_p;
    int                                      d_getBufferIndex;
    int                                      d_putBufferIndex;

  private:
    // PRIVATE MANIPULATORS
    int privateSync();
    void setGetPosition(int off);
    void setPutPosition(int off);

    // PRIVATE ACCESSORS
    int calcGetPosition() const;
    int calcPutPosition() const;
    int checkInvariant() const;

  protected:
    // PROTECTED VIRTUAL FUNCTIONS
    virtual int_type overflow(int_type c = bsl::streambuf::traits_type::eof());
        // Append the optionally specified character 'c' to this streambuf,
        // and return 'c'.  By default, 'traits_type::eof()' is appended.

    virtual int_type pbackfail(int_type c =
                                           bsl::streambuf::traits_type::eof());
        // Adjust the underlying buffer chain and put the specified character
        // 'c' at the newly valid 'gptr()'.  Return 'c' (or '~EOF' if
        // 'c == EOF') on success, and 'EOF' otherwise.

    virtual pos_type seekoff(
                           off_type                offset,
                           bsl::ios_base::seekdir  fixedPosition,
                           bsl::ios_base::openmode which = bsl::ios_base::in
                                                         | bsl::ios_base::out);
        // Set the location from which the next I/O operation indicated by the
        // specified 'which' mode will occur to the specified 'offset'
        // position from the location indicated by the specified
        // 'fixedPosition'.  Return the new offset on success, and
        // 'off_type(-1)' otherwise.  'offset' may be negative.

    virtual pos_type seekpos(
                           pos_type                position,
                           bsl::ios_base::openmode which = bsl::ios_base::in
                                                         | bsl::ios_base::out);
        // Set the location from which the next I/O operation indicated by the
        // specified 'which' mode will occur to the specified 'position'.
        // Return 'position' on success, and 'off_type(-1)' otherwise.

    virtual bsl::streamsize showmanyc();
        // Return the number of characters currently available for reading
        // from this stream buffer, or -1 if there are none.

    virtual int sync();
        // Synchronize the put position in the buffer chain of this stream
        // buffer.  Return 0 unconditionally.

    virtual int_type underflow();
        // Adjust the underlying buffer chain so that the next write position
        // is valid.  Return the character at 'gptr()' on success and 'EOF'
        // otherwise.

    virtual bsl::streamsize xsgetn(char_type       *destination,
                                   bsl::streamsize  numChars);
        // Read the specified 'numChars' to the specified 'destination'.
        // Return the number of characters successfully read.  The behavior
        // is undefined unless 0 <= 'numChars'.

    virtual bsl::streamsize xsputn(const char_type *source,
                                   bsl::streamsize  numChars);
        // Copy the specified 'numChars' from the specified 'source' to the
        // buffer chain held by this streambuf, starting at the current put
        // area location.  The behavior is undefined unless 0 <= 'numChars'.

  private:
    // NOT IMPLEMENTED
    PooledBufferChainStreamBuf(const PooledBufferChainStreamBuf&);
    PooledBufferChainStreamBuf& operator=(
                                      const PooledBufferChainStreamBuf&);

  public:
    // CREATORS
    PooledBufferChainStreamBuf(
                                   bdlmca::PooledBufferChain *pooledBufferChain);
        // Create a 'PooledBufferChainStreamBuf' using the specified
        // 'pooledBufferChain'.  Note that the 'pooledBufferChain' is held, not
        // owned, and will *not* be destroyed upon destruction of this stream
        // buffer.

    PooledBufferChainStreamBuf(
         const bsl::shared_ptr<bdlmca::PooledBufferChain>& pooledBufferChainPtr);
        // Create a 'PooledBufferChainStreamBuf' using the specified
        // 'pooledBufferChainPtr'.

    ~PooledBufferChainStreamBuf();
        // Destroy this stream buffer.

    // MANIPULATORS
    bdlmca::PooledBufferChain *data();
        // Return the address of the buffer chain held by this stream buffer.

    bsl::shared_ptr<bdlmca::PooledBufferChain>& sharedData();
        // Return a reference to the modifiable shared pointer owned by this
        // stream buffer.

    void reset(bdlmca::PooledBufferChain *pooledBufferChain = 0);
        // Reset the get and put areas.  Optionally set the underlying
        // buffer chain to the specified 'pooledBufferChain' if
        // 'pooledBufferChain' is not 0.

    // ACCESSORS
    const bdlmca::PooledBufferChain *data() const;
        // Return the address of the buffer chain held by this stream buffer.

    const bsl::shared_ptr<bdlmca::PooledBufferChain>& sharedData() const;
        // Return a reference to the non-modifiable shared pointer owned by
        // this stream buffer.
};

               //==============================================
               // class PooledBufferChainStreamBufFactory
               //==============================================

class PooledBufferChainStreamBufFactory :
        public bdlma::Factory<bsl::streambuf>
{
    // This class provides a factory for 'PooledBufferChainStreamBuf'
    // objects, using an underlying factory supplied at construction for
    // creating the 'bdlmca::PooledBufferChain' instances used to create the
    // stream buffers.

    // DATA
    bdlmca::PooledBufferChainFactory
                     *d_factory_p;    // chain factory (held, not owned)
    bslma::Allocator *d_allocator_p;  // memory allocator (held, not owned)

   private:
     // NOT IMPLEMENTED
     PooledBufferChainStreamBufFactory(
                               const PooledBufferChainStreamBufFactory&);
     PooledBufferChainStreamBufFactory& operator=(
                               const PooledBufferChainStreamBufFactory&);

   public:
     // CREATORS
     PooledBufferChainStreamBufFactory(
           bdlmca::PooledBufferChainFactory *chainFactory,
           bslma::Allocator               *basicAllocator = 0);
         // Create a pooled buffer chain stream buffer factory that uses the
         // specified 'chainFactory' for supplying chains.  Optionally specify
         // a 'basicAllocator' used to supply memory.  If 'basicAllocator' is
         // 0, the currently-installed default allocator is used.  Note that
         // this stream buffer factory does not assume ownership of the
         // 'chainFactory'.

     virtual ~PooledBufferChainStreamBufFactory();
         // Destroy this pooled buffer chain stream buffer factory.

     // MANIPULATORS
     virtual bsl::streambuf *createObject();
         // Create a 'PooledBufferChainStreamBuf' and return its
         // address.

     virtual void deleteObject(bsl::streambuf *instance);
         // Delete the specified 'instance'.
};

// ===========================================================================
//                      INLINE FUNCTION DEFINITIONS
// ===========================================================================

// PRIVATE ACCESSORS
inline
int PooledBufferChainStreamBuf::calcGetPosition() const
{
    const int bufferSize = d_pooledBufferChain_p->bufferSize();
    return static_cast<int>(
                           d_getBufferIndex * bufferSize + (gptr() - eback()));
}

inline
int PooledBufferChainStreamBuf::calcPutPosition() const
{
    const int bufferSize = d_pooledBufferChain_p->bufferSize();
    return static_cast<int>(
                           d_putBufferIndex * bufferSize + (pptr() - pbase()));
}

// CREATORS
inline
PooledBufferChainStreamBuf::PooledBufferChainStreamBuf(
                                    bdlmca::PooledBufferChain *pooledBufferChain)
: d_pooledBufferChain_p(pooledBufferChain, bslstl::SharedPtrNilDeleter(), 0)
{
    const int totalSize = d_pooledBufferChain_p->length();
    setGetPosition(0);
    setPutPosition(totalSize);
}

inline
PooledBufferChainStreamBuf::PooledBufferChainStreamBuf(
          const bsl::shared_ptr<bdlmca::PooledBufferChain>& pooledBufferChainPtr)
: d_pooledBufferChain_p(pooledBufferChainPtr)
{
    const int totalSize = d_pooledBufferChain_p->length();
    setGetPosition(0);
    setPutPosition(totalSize);
}

// MANIPULATORS
inline
bdlmca::PooledBufferChain *PooledBufferChainStreamBuf::data()
{
    return d_pooledBufferChain_p.get();
}

inline
int PooledBufferChainStreamBuf::privateSync()
{
    BSLS_ASSERT(0 == checkInvariant());

    const int totalSize = d_pooledBufferChain_p->length();
    const int putPosition = calcPutPosition();

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(putPosition > totalSize)) {
        d_pooledBufferChain_p->setLength(putPosition);
    }
    return 0;
}

inline
bsl::shared_ptr<bdlmca::PooledBufferChain>&
    PooledBufferChainStreamBuf::sharedData()
{
    return d_pooledBufferChain_p;
}

// ACCESSORS
inline
const bdlmca::PooledBufferChain *PooledBufferChainStreamBuf::data() const
{
    return d_pooledBufferChain_p.get();
}

inline
const bsl::shared_ptr<bdlmca::PooledBufferChain>&
    PooledBufferChainStreamBuf::sharedData() const
{
    return d_pooledBufferChain_p;
}
}  // close package namespace

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2004
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
