// bcema_pooledbufferchain.h                                          -*-C++-*-
#ifndef INCLUDED_BCEMA_POOLEDBUFFERCHAIN
#define INCLUDED_BCEMA_POOLEDBUFFERCHAIN

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a pooled sequence of fixed-size buffers.
//
//@CLASSES:
//        bcema_PooledBufferChain: memory-manager for chained buffers
// bcema_PooledBufferChainFactory: factory for buffer chains
//
//@SEE_ALSO: bcema_pool, bcema_deleter, bdema_allocator
//
//@AUTHOR: Andrei Basov (abasov)
//
//@DETAILS: This component provides a sequence ("chain") of fixed-size buffers
// allocated from a user-installed memory pool and a factory for efficient
// allocation and deallocation of such sequences.  The buffer chain provides
// access to individual buffers in the chain for efficient data manipulation.
// It also provides methods for efficient copying in / out data from external
// memory.  The manipulation of buffers in the sequence, such as changing the
// order or removing particular buffer, is not allowed.
//
// Chain factory provided by this component manages the buffer chains of
// particular buffer size; it (factory) adheres to 'bcema_Deleter' protocol,
// and thus can be used in various smart pointers such as
// 'bcema_SharedPtr'.
//
///Usage
///-----
// In the following usage example, let's assume that we have an array of
// fixed-size structs that we would like to pack into a buffer chain, and
// then create a counted handle to such a buffer-chain, possibly for
// enqueuing it later.  The structure is POD and may look something like:
//..
//  struct my_Tick {
//      char   d_name[4];
//      double d_bid;
//      double d_offer;
//      // ...
//  };
//..
// and the corresponding array that will be packed is
//..
//  enum { NUM_TICKS = 2000 };
//  my_Tick packet[NUM_TICKS];
//..
// First, let's pick a buffer size, say 8192 and create a pooled buffer chain
// factory:
//..
//  enum { BUFFER_SIZE = 8192 };
//  bcema_TestAllocator ta;
//  bcema_PooledBufferChainFactory factory(BUFFER_SIZE);
//..
// Now, allocate a buffer chain.  For simplicity, we allocate it with initial
// length of 0 bytes and let 'replace' operation resize it as needed
// later:
//..
//  bcema_PooledBufferChain *chain = factory.allocate(0);
//  assert(chain);                      assert(0 == chain->length());
//..
// Next, copy the ticks into 'chain':
//..
//  int length = sizeof packet;
//  chain->replace(0,     // initial offset into the chain
//                 (const char *)packet,
//                 length // number of bytes to copy
//                 );
//  assert(length == chain->length());
//..
// Finally, we can create a 'bcema_SharedPtr' for this chain:
//..
//  bcema_SharedPtr<bcema_PooledBufferChain> handle;
//  handle.load(chain, &factory, &ta);
//..
// Note that the user must guaranteed that the lifetime of the factory
// in the above example is longer than the lifetime of 'handle'; otherwise,
// the destruction of 'handle' will result in an undefined behavior.

#ifndef INCLUDED_BCESCM_VERSION
#include <bcescm_version.h>
#endif

#ifndef INCLUDED_BCEMA_DELETER
#include <bcema_deleter.h>
#endif

#ifndef INCLUDED_BCEMA_POOL
#include <bcema_pool.h>
#endif

#ifndef INCLUDED_BSL_VECTOR
#include <bsl_vector.h>
#endif

#ifndef INCLUDED_BSL_CSTRING
#include <bsl_cstring.h>
#endif

#ifndef INCLUDED_BSLFWD_BSLMA_ALLOCATOR
#include <bslfwd_bslma_allocator.h>
#endif

namespace BloombergLP {

                        // =============================
                        // class bcema_PooledBufferChain
                        // =============================

class bcema_PooledBufferChain {
    // This class provides a linked sequence of fixed-size buffers holding
    // data.  The length of the data extends over all the buffers, except
    // perhaps for some portion of the last buffer; in particular, a chain of
    // length 0 has no buffers.  Data from one chain can be copied into an
    // external buffer, and replace or be appended to another in another chain.
    // Buffers are supplied by and returned to a user-installed pool; in
    // particular, the size of the buffers is fixed and depends on (but is not
    // equal to) the pool's object size.  A factory can be used to produce
    // chains of the desired buffer size.

    // DATA
    bcema_Pool  *d_pool_p;      // buffer pool
    char       **d_first_p;     // buffer chain
    char        *d_last_p;      // last buffer in chain
    int          d_length;      // length of data in chain
    int          d_numBuffers;  // number of buffers in chain

    // NOT IMPLEMENTED
    bcema_PooledBufferChain(const bcema_PooledBufferChain&);
    bcema_PooledBufferChain& operator=(const bcema_PooledBufferChain&);

  public:
    // CREATORS
    bcema_PooledBufferChain(bcema_Pool *pool);
        // Create a pooled buffer chain for the buffers allocated from the
        // specified 'pool'.

    ~bcema_PooledBufferChain();
        // Destroy this buffer chain and return all managed buffers to the
        // pool.

    // MANIPULATORS
    char& operator[](int index);
        // Return the reference to the modifiable character at the specified
        // 'index' byte index.  The behavior is undefined unless
        // ' 0 <= index < length()'.

    void append(const char *buffer, int numBytes);
        // Append to this buffer chain the specified 'numBytes' from the
        // specified 'buffer'.  Allocate additional buffers as needed.  The
        // behavior is undefined unless 'buffer' refers to at least
        // 'numBytes' readable bytes and is not part of this buffer chain.

    char *buffer(int index);
        // Return the address of the modifiable buffer at the specified
        // 'index'.  The behavior is undefined unless
        // '0 <= index < numBuffers()'.

    void removeAll();
        // Deallocate all the buffers managed by this buffer chain (i.e.,
        // return to the memory pool) and remove them from this chain.  Note
        // that the 'numBuffers' method will return 0 after this method
        // returns.

    void replace(int offset, const char *buffer, int numBytes);
        // Copy into this buffer chain starting at the specified 'offset'
        // the specified 'numBytes' from the specified 'arena'.
        // Allocate additional buffers as needed.  The behavior is undefined
        // unless '0 <= offset <= length()', '0 <= numBytes', 'buffer'
        // refers to at least 'numBytes' readable bytes and is not part
        // of this buffer chain.

    void replace(int                            offset,
                 const bcema_PooledBufferChain& source,
                 int                            srcOffset,
                 int                            numBytes);
        // Copy into this buffer chain at the specified 'offset' byte index
        // from the specified 'source' at the specified 'srcOffset' byte
        // offset, the specified 'numBytes'.  If the total data length of this
        // buffer chain is less than 'offset + numBytes', extend the length of
        // this buffer chain to 'offset + numBytes' by appending new buffers as
        // necessary.  The behavior is undefined unless
        // '0 <= offset <= length()', '0 <= srcOffset', '0 <= numBytes',
        // 'srcOffset + numBytes <= source.length()', and 'source' refers to a
        // buffer chain different from this one.

    void setLength(int newLength);
    void setLength(int newLength, bcema_PooledBufferChain *source);
        // Set the total data length of this buffer chain to the specified
        // 'newLength' bytes removing or adding buffers as needed.  Optionally
        // specify a 'source' chain to supply additional buffers as needed; if
        // more buffers are needed beyond what 'source' can provide, those will
        // be obtained for the pool passed at construction to this chain.  The
        // behavior is undefined unless '0 <= newLength' and 'source' uses the
        // same pool as this chain.

    // ACCESSORS
    const char& operator[](int index) const;
        // Return the reference to the non-modifiable character at the
        // specified 'index' byte index.  The behavior is undefined unless
        // '0 <= index < length()'.

    const char *buffer(int index) const;
        // Return the address of the non-modifiable buffer at the specified
        // 'index'.  The behavior is undefined unless
        // '0 <= index < numBuffers()'.

    int bufferSize() const;
        // Return the length of the buffers in this sequence.  Note that all
        // buffers in this sequence have the same length.

    void copyOut(char *buffer, int numBytes, int offset) const;
        // Copy into the specified 'buffer' the specified 'numBytes' from this
        // buffer chain starting at the specified (byte) 'offset'.  The
        // behavior is undefined unless '0 <= offset', '0 <= numBytes',
        // '0 <= offset + numBytes <= numBytes()', and 'buffer' refers to at
        // least 'numBytes' writable bytes and is not part of this buffer
        // chain.

    int length() const;
        // Return the total number of bytes contained in this chain.

    int loadBuffers(const char **buffers, int numBuffers, int index) const;
        // Load the addresses of the non-modifiable buffers in this chain
        // starting at the specified buffer 'index' into the specified
        // 'buffers' array of the specified 'numBuffers' length.  If the number
        // of buffers in this chain is less than 'index + numBuffers', then the
        // remaining entries in 'buffers' are unchanged.  Return the number of
        // buffers loaded into 'buffers'.  The behavior is undefined unless
        // '0 <= index < numBuffers'.

    void loadBuffers(bsl::vector<const char *> *buffers) const;
        // Load the addresses of all the non-modifiable buffers in this chain
        // into the specified 'buffers' vector.

    int numBuffers() const;
        // Return the total number of buffers contained in this chain.
};

                    // ====================================
                    // class bcema_PooledBufferChainFactory
                    // ====================================

class bcema_PooledBufferChainFactory :
                                public bcema_Deleter<bcema_PooledBufferChain> {

    // DATA
    bcema_Pool d_bufferPool;
    bcema_Pool d_pcbPool;

    // NOT IMPLEMENTED
    bcema_PooledBufferChainFactory(const bcema_PooledBufferChainFactory&);
    bcema_PooledBufferChainFactory& operator=(
                                   const bcema_PooledBufferChainFactory&);
  public:
    // CREATORS
    bcema_PooledBufferChainFactory(int              bufferSize,
                                   bslma_Allocator *basicAllocator = 0);
        // Create a factory for the pooled buffer chains with the specified
        // 'bufferSize'.  Optionally specify a 'basicAllocator' used to supply
        // memory.  If 'basicAllocator' is 0, the currently installed default
        // allocator is used.

    ~bcema_PooledBufferChainFactory();
        // Destroy this factory and release all memory associated with
        // any outstanding buffer chain.

    // MANIPULATORS
    bcema_PooledBufferChain *allocate(int length);
        // Allocate a buffer chain of the specified 'length' byte length.

    virtual void deleteObject(bcema_PooledBufferChain *object);
        // Return the specified 'object' to this factory.

    // ACCESSORS
    int bufferSize() const;
        // Return the size of the data buffers allocated by this factory.
};

// ============================================================================
//                         INLINE FUNCTION DEFINITIONS
// ============================================================================

                       // -----------------------------
                       // class bcema_PooledBufferChain
                       // -----------------------------

// METHODS NEEDED BY OTHER INLINE METHODS

// ACCESSORS
inline
int bcema_PooledBufferChain::bufferSize() const
{
    return static_cast<int>(d_pool_p->blockSize() - sizeof(char *));
}

// CREATORS
inline
bcema_PooledBufferChain::bcema_PooledBufferChain(bcema_Pool *pool)
: d_pool_p(pool)
, d_first_p((char **)0)
, d_last_p(0)
, d_length(0)
, d_numBuffers(0)
{
}

inline
bcema_PooledBufferChain::~bcema_PooledBufferChain()
{
    removeAll();
}

// MANIPULATORS
inline
char& bcema_PooledBufferChain::operator[](int index)
{
    int bufferIdx = index / bufferSize();
    int offset    = index % bufferSize();
    return buffer(bufferIdx)[offset];
}

// ACCESSORS
inline
const char& bcema_PooledBufferChain::operator[](int index) const
{
    int bufferIdx = index / bufferSize();
    int offset    = index % bufferSize();
    return buffer(bufferIdx)[offset];
}

inline
int bcema_PooledBufferChain::length() const
{
    return d_length;
}

inline
int bcema_PooledBufferChain::numBuffers() const
{
    return d_numBuffers;
}

                    // ------------------------------------
                    // class bcema_PooledBufferChainFactory
                    // ------------------------------------

// CREATORS
inline
bcema_PooledBufferChainFactory::bcema_PooledBufferChainFactory(
                                               int              bufferSize,
                                               bslma_Allocator *basicAllocator)
: d_bufferPool(static_cast<int>(bufferSize + sizeof(char *)), basicAllocator)
, d_pcbPool(static_cast<int>(sizeof(bcema_PooledBufferChain)), basicAllocator)
{
}

// MANIPULATORS
inline
bcema_PooledBufferChain *bcema_PooledBufferChainFactory::allocate(int length)
{
    bcema_PooledBufferChain *result =
                        new (d_pcbPool) bcema_PooledBufferChain(&d_bufferPool);
    result->setLength(length);
    return result;
}

// ACCESSORS
inline
int bcema_PooledBufferChainFactory::bufferSize() const
{
    return static_cast<int>(d_bufferPool.blockSize() - sizeof(char *));
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
