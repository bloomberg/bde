// bcesb_pooledbufferchainstreambuf.h                                 -*-C++-*-
#ifndef INCLUDED_BCESB_POOLEDBUFFERCHAINSTREAMBUF
#define INCLUDED_BCESB_POOLEDBUFFERCHAINSTREAMBUF

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide pooled buffer chain implementing the 'streambuf' interface.
//
//@CLASSES:
// bcesb_PooledBufferChainStreamBuf: pooled buffer chain 'streambuf'
// bcesb_PooledBufferChainStreamBufFactory: a default factory for 'streambuf's
//
//@AUTHOR: Vladimir Kliatchko (vkliatch)
//
//@SEE_ALSO: bcema_pooledbufferchain
//
//@DESCRIPTION: This component implements the 'bsl::basic_streambuf' protocol
// using a user-supplied 'bcema_PooledBufferChain'.  Method names necessarily
// correspond to the protocol-specified method names.  Refer to the C++
// Standard, Section 27.5.2, for a full specification of the
// 'bsl::basic_streambuf' interface.
//
// A pooled buffer chain is an indexed sequence of fixed-size buffers.  The
// number of buffers in the sequence can increase or decrease, but the order of
// the buffers cannot change.  Therefore, the buffer chain behaves logically as
// a single indexed buffer.  'bcesb_PooledBufferChainStreamBuf' can therefore
// read from and write to this buffer as if there were a single continuous
// index.

#ifndef INCLUDED_BCESCM_VERSION
#include <bcescm_version.h>
#endif

#ifndef INCLUDED_BCEMA_POOLEDBUFFERCHAIN
#include <bcema_pooledbufferchain.h>
#endif

#ifndef INCLUDED_BCEMA_FACTORY
#include <bcema_factory.h>
#endif

#ifndef INCLUDED_BCEMA_SHAREDPTR
#include <bcema_sharedptr.h>
#endif

#ifndef INCLUDED_BSLS_PERFORMANCEHINT
#include <bsls_performancehint.h>
#endif

#ifndef INCLUDED_BSL_IOS
#include <bsl_ios.h>
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

namespace BloombergLP {

                   //=======================================
                   // class bcesb_PooledBufferChainStreamBuf
                   //=======================================

class bcesb_PooledBufferChainStreamBuf : public bsl::streambuf {
    // This class implements the input functionality of the 'basic_streambuf'
    // protocol, using a client-supplied 'bcema_PooledBufferChain'.

    // PRIVATE TYPES
    typedef bsl::ios_base ios_base;

    // DATA
    bcema_SharedPtr<bcema_PooledBufferChain> d_pooledBufferChain_p;
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
    bcesb_PooledBufferChainStreamBuf(const bcesb_PooledBufferChainStreamBuf&);
    bcesb_PooledBufferChainStreamBuf& operator=(
                                      const bcesb_PooledBufferChainStreamBuf&);

  public:
    // CREATORS
    bcesb_PooledBufferChainStreamBuf(
                                   bcema_PooledBufferChain *pooledBufferChain);
        // Create a 'bcesb_PooledBufferChainStreamBuf' using the specified
        // 'pooledBufferChain'.  Note that the 'pooledBufferChain' is held, not
        // owned, and will *not* be destroyed upon destruction of this stream
        // buffer.

    bcesb_PooledBufferChainStreamBuf(
         const bcema_SharedPtr<bcema_PooledBufferChain>& pooledBufferChainPtr);
        // Create a 'bcesb_PooledBufferChainStreamBuf' using the specified
        // 'pooledBufferChainPtr'.

    ~bcesb_PooledBufferChainStreamBuf();
        // Destroy this stream buffer.

    // MANIPULATORS
    bcema_PooledBufferChain *data();
        // Return the address of the buffer chain held by this stream buffer.

    bcema_SharedPtr<bcema_PooledBufferChain>& sharedData();
        // Return a reference to the modifiable shared pointer owned by this
        // stream buffer.

    void reset(bcema_PooledBufferChain *pooledBufferChain = 0);
        // Reset the get and put areas.  Optionally set the underlying
        // buffer chain to the specified 'pooledBufferChain' if
        // 'pooledBufferChain' is not 0.

    // ACCESSORS
    const bcema_PooledBufferChain *data() const;
        // Return the address of the buffer chain held by this stream buffer.

    const bcema_SharedPtr<bcema_PooledBufferChain>& sharedData() const;
        // Return a reference to the non-modifiable shared pointer owned by
        // this stream buffer.
};

               //==============================================
               // class bcesb_PooledBufferChainStreamBufFactory
               //==============================================

class bcesb_PooledBufferChainStreamBufFactory :
        public bcema_Factory<bsl::streambuf>
{
    // This class provides a factory for 'bcesb_PooledBufferChainStreamBuf'
    // objects, using an underlying factory supplied at construction for
    // creating the 'bcema_PooledBufferChain' instances used to create the
    // stream buffers.

    // DATA
    bcema_PooledBufferChainFactory
                    *d_factory_p;    // chain factory (held, not owned)
    bslma_Allocator *d_allocator_p;  // memory allocator (held, not owned)

   private:
     // NOT IMPLEMENTED
     bcesb_PooledBufferChainStreamBufFactory(
                               const bcesb_PooledBufferChainStreamBufFactory&);
     bcesb_PooledBufferChainStreamBufFactory& operator=(
                               const bcesb_PooledBufferChainStreamBufFactory&);

   public:
     // CREATORS
     bcesb_PooledBufferChainStreamBufFactory(
           bcema_PooledBufferChainFactory *chainFactory,
           bslma_Allocator                *basicAllocator = 0);
         // Create a pooled buffer chain stream buffer factory that uses the
         // specified 'chainFactory' for supplying chains.  Optionally specify
         // a 'basicAllocator' used to supply memory.  If 'basicAllocator' is
         // 0, the currently-installed default allocator is used.  Note that
         // this stream buffer factory does not assume ownership of the
         // 'chainFactory'.

     virtual ~bcesb_PooledBufferChainStreamBufFactory();
         // Destroy this pooled buffer chain stream buffer factory.

     // MANIPULATORS
     virtual bsl::streambuf *createObject();
         // Create a 'bcesb_PooledBufferChainStreamBuf' and return its
         // address.

     virtual void deleteObject(bsl::streambuf *instance);
         // Delete the specified 'instance'.
};

// ===========================================================================
//                      INLINE FUNCTION DEFINITIONS
// ===========================================================================

// PRIVATE ACCESSORS
inline
int bcesb_PooledBufferChainStreamBuf::calcGetPosition() const
{
    const int bufferSize = d_pooledBufferChain_p->bufferSize();
    return static_cast<int>(
                           d_getBufferIndex * bufferSize + (gptr() - eback()));
}

inline
int bcesb_PooledBufferChainStreamBuf::calcPutPosition() const
{
    const int bufferSize = d_pooledBufferChain_p->bufferSize();
    return static_cast<int>(
                           d_putBufferIndex * bufferSize + (pptr() - pbase()));
}

// CREATORS
inline
bcesb_PooledBufferChainStreamBuf::bcesb_PooledBufferChainStreamBuf(
                                    bcema_PooledBufferChain *pooledBufferChain)
: d_pooledBufferChain_p(pooledBufferChain, bcema_SharedPtrNilDeleter(), 0)
{
    const int totalSize = d_pooledBufferChain_p->length();
    setGetPosition(0);
    setPutPosition(totalSize);
}

inline
bcesb_PooledBufferChainStreamBuf::bcesb_PooledBufferChainStreamBuf(
          const bcema_SharedPtr<bcema_PooledBufferChain>& pooledBufferChainPtr)
: d_pooledBufferChain_p(pooledBufferChainPtr)
{
    const int totalSize = d_pooledBufferChain_p->length();
    setGetPosition(0);
    setPutPosition(totalSize);
}

// MANIPULATORS
inline
bcema_PooledBufferChain *bcesb_PooledBufferChainStreamBuf::data()
{
    return d_pooledBufferChain_p.ptr();
}

inline
int bcesb_PooledBufferChainStreamBuf::privateSync()
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
bcema_SharedPtr<bcema_PooledBufferChain>&
    bcesb_PooledBufferChainStreamBuf::sharedData()
{
    return d_pooledBufferChain_p;
}

// ACCESSORS
inline
const bcema_PooledBufferChain *bcesb_PooledBufferChainStreamBuf::data() const
{
    return d_pooledBufferChain_p.ptr();
}

inline
const bcema_SharedPtr<bcema_PooledBufferChain>&
    bcesb_PooledBufferChainStreamBuf::sharedData() const
{
    return d_pooledBufferChain_p;
}

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
