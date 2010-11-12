// bcema_pooledbufferchain.cpp   -*-C++-*-
#include <bcema_pooledbufferchain.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bcema_pooledbufferchain_cpp,"$Id$ $CSID$")

#include <bcema_sharedptr.h>           // for testing only
#include <bcema_testallocator.h>       // for testing only
#include <bsls_assert.h>

#include <bsl_cstring.h>

namespace BloombergLP {

                       // -----------------------------
                       // class bcema_PooledBufferChain
                       // -----------------------------

// MANIPULATORS
void bcema_PooledBufferChain::append(const char *buffer, int numBytes)
{
    int newLength = d_length + numBytes;
    int bufSize   = bufferSize();

    if (newLength > d_numBuffers * bufSize) {
        replace(d_length, buffer, numBytes);
    }
    else {
        bsl::memcpy(d_last_p + (d_length % bufSize), buffer, numBytes);
        d_length = newLength;
    }
}

char *bcema_PooledBufferChain::buffer(int index)
{
    char **base = d_first_p;
    for (int i = 0; i < index; ++i) {
        base = (char **)(void *)*base;
    }
    return (char *)base + sizeof(char *);
}

void bcema_PooledBufferChain::removeAll()
{
    while (d_first_p) {
        char  *buffer = (char *)d_first_p;
        char **next   = (char **)(void *)d_first_p[0];
        d_pool_p->deallocate(buffer);
        d_first_p = next;
    }
    d_length     = 0;
    d_first_p    = 0;
    d_last_p     = 0;
    d_numBuffers = 0;
}

void bcema_PooledBufferChain::setLength(int newLength)
{
    int bufSize       = bufferSize();
    int newNumBuffers = (newLength + bufSize - 1) / bufSize;

    if (newNumBuffers < d_numBuffers) {
        if (0 == newLength) {
            removeAll();
            return;
        }
        char *p = buffer(newNumBuffers - 1);
        d_last_p = p;
        p -= sizeof(char*);
        char **newLast =  (char  **)(void *)p;
        char **next    = *(char ***)(void *)p;
        while (next) {
            char *buffer = (char*)next;
            next = *(char ***)(void *)buffer;
            d_pool_p->deallocate(buffer);
        }
        newLast[0] = (char*)0;
    }
    else if (newNumBuffers > d_numBuffers) {
        int numAdditional =  newNumBuffers - d_numBuffers;
        char ***x = d_last_p
                  ? (char ***)(void *)(d_last_p - sizeof(char *))
                  : &d_first_p;
        for (int i = 0; i < numAdditional; ++i) {
            // After the following assignment, 'd_last_p' is not correct
            // because it points to the beginning of the last buffer while it
            // is supposed to the beginning of the data in this buffer.  So it
            // needs to be shifted by the length of "char *".  This is done at
            // the end of the loop.

            d_last_p = (char*)d_pool_p->allocate();
            char **base = (char **)(void *)d_last_p;
            *base = NULL;
            *x = base;
            x = (char***)base;
        }
        d_last_p += sizeof(char*);
    }
    d_length = newLength;
    d_numBuffers = newNumBuffers;
}

void bcema_PooledBufferChain::setLength(int                      newLength,
                                        bcema_PooledBufferChain *source)
{
    BSLS_ASSERT(source->d_pool_p == d_pool_p);

    int bufSize       = bufferSize();
    int newNumBuffers = (newLength + bufSize - 1) / bufSize;

    if (newNumBuffers < d_numBuffers) {
        if (0 == newLength) {
            removeAll();
            return;
        }
        char *p = buffer(newNumBuffers - 1);
        d_last_p = p;
        p -= sizeof(char*);
        char **newLast =  (char  **)(void *)p;
        char **next    = *(char ***)(void *)p;
        while (next) {
            char *buffer = (char *)next;
            next = *(char ***)(void *)buffer;
            d_pool_p->deallocate(buffer);
        }
        newLast[0] = (char*)0;
    }
    else if (newNumBuffers > d_numBuffers) {
        int numAdditional =  newNumBuffers - d_numBuffers;
        char ***x = d_last_p
                  ? (char ***)(void *)(d_last_p - sizeof(char *))
                  : &d_first_p;
        for (int i = 0; i < numAdditional; ++i) {
            // During the loop, 'd_last_p' is not correct because it points to
            // the beginning of the last buffer while it is supposed to the
            // beginning of the *data* in this buffer.  So it needs to be
            // shifted by the length of "char *".  This is done at the end of
            // the loop.

            if (0 < source->d_numBuffers) {
                d_last_p = (char *)source->d_first_p;
                --source->d_numBuffers;
                if (0 < source->d_numBuffers) {
                    source->d_first_p = (char **)(void *)*(source->d_first_p);
                    // source->d_last_p is unchanged
                    source->d_length -= bufSize;
                } else {
                    source->d_first_p = 0;
                    source->d_last_p = 0;
                    source->d_length = 0;
                }
            } else {
                d_last_p = (char*)d_pool_p->allocate();
            }
            char **base = (char **)(void *)d_last_p;
            *base = NULL;
            *x = base;
            x = (char***)base;
        }
        d_last_p += sizeof(char*);
    }
    d_length = newLength;
    d_numBuffers = newNumBuffers;
}

#if 0
    // PROPOSED ADDITION
    void import(bcema_PooledBufferChain *source,
                int                      numBytes);
        // Append to this buffer chain the buffers containing the specified
        // 'numBytes' from the specified 'source' byte offset, and replace
        // these by uninitialized buffers in 'source'.  It is assumed that only
        // the first 'numBytes' of 'source' contain data, but the length could
        // be larger because of pre-allocated buffers.  The behavior is
        // undefined unless the length of this buffer chain is a multiple of
        // 'bufferSize()', 'source' refers to a buffer chain different from
        // this one, 'source' uses the same pool as this chain, and
        // '0 <= numBytes <= source.length()'.

void bcema_PooledBufferChain::import(bcema_PooledBufferChain *source,
                                     int                      numBytes)
{
    int bufSize = bufferSize();
    BSLS_ASSERT(source);
    BSLS_ASSERT(0 <= numBytes && numBytes <= source.length());
    BSLS_ASSERT(0 == d_length % bufSize);

    char **base = (char **)d_last_p;
    base -= sizeof(char *);
    char **srcBase = source->d_first_p;
    source->d_first_p = 0;
    while (0 < numBytes) {
        *base= (char *)srcBase;
        base = srcBase;
        d_length += bufSize;
        numBytes -= bufSize;
        srcBase = (char **)d_pool_p->allocate();
        if (!source->d_first_p) {
            source->d_first_p = srcBase;
        }
        *srcBase = *base;
        srcBase = (char **)*base;
    }
    // TBD
    d_last_p -= sizeof(char *);
}
#endif

void bcema_PooledBufferChain::replace(int         offset,
                                      const char *arena,
                                      int         numBytes)
{
    BSLS_ASSERT(arena);
    BSLS_ASSERT(0 <= offset);
    BSLS_ASSERT(offset <= d_length);

    if (d_length < offset + numBytes) {
        setLength(offset + numBytes);
    }

    int size = bufferSize();
    int bufferIdx = offset / size;
    int bufferOffset = offset % size;
    int endBufferIdx = (offset + numBytes) / size;
    int endBufferOffset = (offset + numBytes) % size;

    if (endBufferIdx == bufferIdx) {
        BSLS_ASSERT(bufferOffset <= endBufferOffset);
        BSLS_ASSERT(endBufferOffset - bufferOffset == numBytes);

        bsl::memcpy(buffer(bufferIdx) + bufferOffset, arena, numBytes);
        return;
    }

    BSLS_ASSERT(bufferIdx < endBufferIdx);
    char  *p    = buffer(bufferIdx);
    char **next = *(char ***)(void *)(p - sizeof(char*));

    bsl::memcpy(p + bufferOffset, arena, size - bufferOffset);
    arena += (size - bufferOffset);
    while(++bufferIdx != endBufferIdx) {
        char *destination = (char*)next + sizeof(char*);
        bsl::memcpy(destination, arena, size);
        arena += size;
        next = (char **)(void *)next[0];
    }

    p = (char*)next + sizeof(char*);
    BSLS_ASSERT(buffer(endBufferIdx) == p);
    bsl::memcpy(p, arena, endBufferOffset);
}

void bcema_PooledBufferChain::replace(int                            offset,
                                      const bcema_PooledBufferChain& source,
                                      int                            srcOffset,
                                      int                            numBytes)
{
    BSLS_ASSERT(0 <= offset);
    BSLS_ASSERT(offset <= d_length);
    BSLS_ASSERT(0 <= srcOffset);
    BSLS_ASSERT(0 <= numBytes);
    BSLS_ASSERT(srcOffset + numBytes <= source.length());

    setLength(offset + numBytes);
    if (0 == numBytes) {
        return;
    }

    const int   dstBufSize = bufferSize();
    const int   srcBufSize = source.bufferSize();
    char       *dst = buffer(offset / dstBufSize);
    const char *src = source.buffer(srcOffset / srcBufSize);

    offset    %= dstBufSize;
    srcOffset %= srcBufSize;

    while (numBytes) {
        int canCopy = dstBufSize - offset;
        if (srcBufSize - srcOffset < canCopy) {
            canCopy = srcBufSize - srcOffset;
        }
        if (canCopy > numBytes) {
            canCopy = numBytes;
        }

        bsl::memcpy(dst + offset, src + srcOffset, canCopy);
        numBytes  -= canCopy;
        offset    += canCopy;
        srcOffset += canCopy;

        if (offset == dstBufSize) {
            offset = 0;
            dst = (((char **)(void *)dst)[-1]) + sizeof dst;
        }
        if (srcOffset == srcBufSize) {
            srcOffset = 0;
            src = (((char **)(void *)src)[-1]) + sizeof src;
        }
    }
}

void bcema_PooledBufferChain::copyOut(char *arena,
                                      int   numBytes,
                                      int   offset) const
{
    BSLS_ASSERT(arena);
    BSLS_ASSERT(0 <= numBytes);
    BSLS_ASSERT(0 <= offset);
    BSLS_ASSERT(offset + numBytes <= length());

    int size = bufferSize();
    int bufferIdx = offset / size;
    int bufferOffset = offset % size;
    int endBufferIdx = (offset + numBytes) / size;
    int endBufferOffset = (offset + numBytes) % size;
    if (bufferIdx == endBufferIdx) {
        BSLS_ASSERT(bufferOffset <= endBufferOffset);
        BSLS_ASSERT(endBufferOffset - bufferOffset == numBytes);

        bsl::memcpy(arena, buffer(bufferIdx) + bufferOffset, numBytes);
        return;
    }
    BSLS_ASSERT(bufferIdx < endBufferIdx);

    const char *p = buffer(bufferIdx);
    char **next = *(char ***)(void *)(p - sizeof(char*));

    bsl::memcpy(arena, p + bufferOffset, size - bufferOffset);
    arena += (size - bufferOffset);
    while(++bufferIdx != endBufferIdx) {
        char *source = (char*)next + sizeof(char*);
        bsl::memcpy(arena, source, size);
        arena += size;
        next = (char **)(void *)next[0];
    }
    p = (char*)next + sizeof(char*);

    BSLS_ASSERT(p == buffer(endBufferIdx));
    bsl::memcpy(arena, p, endBufferOffset);
}

// ACCESSORS
const char *bcema_PooledBufferChain::buffer(int index) const
{
    char **base = d_first_p;
    for (int i = 0; i < index; ++i) {
        base = (char **)(void *)*base;
    }
    return (const char *)base + sizeof(char *);
}

int bcema_PooledBufferChain::loadBuffers(const char **buffers,
                                         int          numBuffers,
                                         int          index) const
{
    int i;
    char **base = d_first_p;
    for (i = 0; i < index; ++i) {
        base = (char **)(void *)*base;
    }
    for (i = 0; base && i < numBuffers; ++i, ++buffers) {
        *buffers = (const char *)base + sizeof(char *);
        base = (char **)(void *)*base;
    }
    return i;
}

void bcema_PooledBufferChain::loadBuffers(
                                       bsl::vector<const char *>*buffers) const
{
    buffers->resize(d_numBuffers);
    char **base = d_first_p;
    for (int i = 0; base; ++i) {
        (*buffers)[i] = (const char *)base + sizeof(char *);
        base = (char **)(void *)*base;
    }
}

                    // ------------------------------------
                    // class bcema_PooledBufferChainFactory
                    // ------------------------------------

bcema_PooledBufferChainFactory::~bcema_PooledBufferChainFactory()
{

}

void bcema_PooledBufferChainFactory::deleteObject(
                                               bcema_PooledBufferChain *object)
{
    d_pcbPool.deleteObject(object);
}

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2007
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
