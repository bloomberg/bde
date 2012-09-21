// btemt_channeloutstreambuf.cpp                                      -*-C++-*-
#include <btemt_channeloutstreambuf.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(btemt_channeloutstreambuf_cpp,"$Id$ $CSID$")

#include <btemt_channelpool.h>

#ifdef BTE_FOR_TESTING_ONLY
// These dependencies need to be here for the bde_build.pl script to
// generate the proper makefiles, but do not need to be compiled into the
// component's .o file.  The symbol BTE_FOR_TESTING_ONLY should remain
// undefined, and is here only because '#if 0' is optimized away by the
// bde_build.pl script.

#include <btemt_channelpoolconfiguration.h> // for testing only
#endif

#include <bcema_pooledbufferchain.h>

#include <bsls_timeutil.h>
#include <bsls_assert.h>

// Note: on Windows -> WinDef.h:#define min(a,b) ...
#if defined(BSLS_PLATFORM_CMP_MSVC) && defined(min)
#undef min
#endif

namespace BloombergLP {

enum {
    MILLISECS_PER_SEC = 1000,
    NANOSECS_PER_SEC  = 1000000000,
    RESOLUTION        = NANOSECS_PER_SEC / MILLISECS_PER_SEC
};

                      // -------------------------------
                      // class btemt_ChannelOutStreamBuf
                      // -------------------------------

// PRIVATE MANIPULATORS

int btemt_ChannelOutStreamBuf::sendDataOverChannel()
{
    BSLS_ASSERT(d_bufferChain_p);

    if (0 == length()) {
        return 0;
    }

    BSLS_ASSERT(d_bufferChainFactory_p);
    BSLS_ASSERT(d_channelPool_p);

    if (d_callback) {
        d_callback(d_dataMsg);
    }

    if (0 != d_channelPool_p->write(d_channelId, d_dataMsg)) {
        return -1;
    }

    d_bufferChain_p = d_bufferChainFactory_p->allocate(0);
    if (0 == d_bufferChain_p) {
        return -1;
    }
    setPutPosition(d_bufferChain_p->length());
    d_dataMsg.setData(d_bufferChain_p, d_bufferChainFactory_p, d_allocator_p);

    d_timeOfLastSend = d_cacheLifespan
                     ? bsls_TimeUtil::getTimer() / RESOLUTION
                     : 0;
    return 0;
}

void btemt_ChannelOutStreamBuf::setPutPosition(off_type offset)
{
    BSLS_ASSERT(d_bufferChain_p);

    int bufferSize = d_bufferChain_p->bufferSize();
    int numBuffers = d_bufferChain_p->numBuffers();

    d_putBufferIndex = offset / bufferSize;

    if (d_putBufferIndex >= numBuffers) {
        setp(0, 0);
        d_putBufferIndex = numBuffers;
    }
    else {
        char *base         = d_bufferChain_p->buffer(d_putBufferIndex);
        int   basePosition = d_putBufferIndex * bufferSize;
        setp(base, base + bufferSize);
        pbump(offset - basePosition);
    }
}

// PRIVATE ACCESSORS

int btemt_ChannelOutStreamBuf::checkInvariant() const
{
    BSLS_ASSERT(d_bufferChain_p);

    int bufferSize = d_bufferChain_p->bufferSize();
    int numBuffers = d_bufferChain_p->numBuffers();

    // check "put" area
    if (pptr()) {
        BSLS_ASSERT(pbase());
        BSLS_ASSERT(epptr());
        BSLS_ASSERT( pptr() - pbase() <= bufferSize);
        BSLS_ASSERT(epptr() - pbase() == bufferSize);
        BSLS_ASSERT(0 <= d_putBufferIndex && d_putBufferIndex < numBuffers);
    }
    else {
        BSLS_ASSERT(0 == pbase());
        BSLS_ASSERT(0 == epptr());
        BSLS_ASSERT(d_putBufferIndex == numBuffers);
    }

    return 0;
}

// PROTECTED MANIPULATORS

bsl::streambuf::int_type btemt_ChannelOutStreamBuf::overflow(
        int_type insertionChar)
{
    BSLS_ASSERT(0 == checkInvariant());

    if (traits_type::eof() == insertionChar) {
        return traits_type::not_eof(insertionChar);
    }

    if (pptr() == epptr()) {
        // Send data if specified number of buffers have been filled.

        int bufferSize  = d_bufferChain_p->bufferSize();
        int putPosition = calculatePutPosition();
        BSLS_ASSERT(0 == putPosition % bufferSize);

        if (d_bufferChain_p->numBuffers() >= d_highWaterMark) {
            d_bufferChain_p->setLength(putPosition);
            if (0 != sendDataOverChannel()) {
                return traits_type::eof();
            }
            putPosition = calculatePutPosition();
            d_putBufferIndex = putPosition / bufferSize;
        }

        d_bufferChain_p->setLength(putPosition + 1);
        putPosition = calculatePutPosition();
        d_putBufferIndex = putPosition / bufferSize;

        char *base = d_bufferChain_p->buffer(d_putBufferIndex);
        setp(base, base + bufferSize);
    }

    if (pptr() != epptr()) {
        *pptr() = insertionChar;
        pbump(1);
        return insertionChar;
    }
    else {
        return traits_type::eof();
    }
}

bsl::streambuf::pos_type btemt_ChannelOutStreamBuf::seekoff(
        off_type                offset,
        bsl::ios_base::seekdir  fixedPosition,
        bsl::ios_base::openmode which)
{
    BSLS_ASSERT(0 == checkInvariant());

    if (!(which & bsl::ios_base::out)) {
        return pos_type(-1);
    }

    // Update buffer size.
    int totalSize   = d_bufferChain_p->length();
    int putPosition = calculatePutPosition();

    if (putPosition > totalSize) {
        d_bufferChain_p->setLength(putPosition);
        totalSize = putPosition;
    }

    off_type newOffset;
    switch (fixedPosition) {
      case bsl::ios_base::beg: {
        newOffset = 0;
      } break;
      case bsl::ios_base::cur: {
        newOffset = calculatePutPosition();
      } break;
      case bsl::ios_base::end: {
        newOffset = totalSize;
      } break;
      default: {
        return off_type(-1);
      }
    }

    newOffset += offset;
    if (newOffset < 0 || totalSize < newOffset) {
        return off_type(-1);
    }
    setPutPosition(newOffset);

    return newOffset;
}

bsl::streambuf::pos_type btemt_ChannelOutStreamBuf::seekpos(
        pos_type                position,
        bsl::ios_base::openmode which)
{
    return seekoff(off_type(position), bsl::ios_base::beg, which);
}

int btemt_ChannelOutStreamBuf::sync()
{
    BSLS_ASSERT(0 == checkInvariant());

    int totalSize   = d_bufferChain_p->length();
    int putPosition = calculatePutPosition();

    if (putPosition > totalSize) {
        d_bufferChain_p->setLength(putPosition);
        totalSize = putPosition;
    }

    if (0 == totalSize) {
        return 0;
    }

    // Conditionally send data if above "low water mark" or if enough time
    // has elapsed since last send.

    int rc = 0;
    if (totalSize >= d_lowWaterMark || 0 == d_cacheLifespan) {
        rc = sendDataOverChannel();
    }
    else {
        const bsls_PlatformUtil::Int64 now =
                                       d_cacheLifespan
                                       ? bsls_TimeUtil::getTimer() / RESOLUTION
                                       : 0;
        if (now - d_timeOfLastSend >= d_cacheLifespan) {
            rc = sendDataOverChannel();
        }
    }

    return rc;
}

bsl::streamsize btemt_ChannelOutStreamBuf::xsputn(
        const char_type *source,
        bsl::streamsize  numChars)
{
    bsl::streamsize numLeft   = numChars;
    bsl::streamsize numCopied = 0;

    while (0 < numLeft) {
        bsl::streamsize remainingChars = epptr() - pptr();
        int             canCopy = bsl::min(remainingChars, numLeft);
        bsl::memcpy(pptr(), source + numCopied, canCopy);
        pbump(canCopy);
        numCopied += canCopy;
        numLeft   -= canCopy;
        if (0 < numLeft) {
            if (traits_type::eof() ==
                         overflow(traits_type::to_int_type(source[numCopied])))
            {
                return numCopied;
            } else {
                numCopied += 1;
                numLeft   -= 1;
            }
        }
    }
    return numCopied;
}

// CREATORS

btemt_ChannelOutStreamBuf::btemt_ChannelOutStreamBuf(
        int                             channelId,
        btemt_ChannelPool              *channelPool,
        bcema_PooledBufferChainFactory *bufferChainFactory,
        int                             lowWaterMark,
        int                             highWaterMark,
        int                             cacheLifespan,
        bslma_Allocator                *basicAllocator)
: d_allocator_p(basicAllocator)
, d_bufferChainFactory_p(bufferChainFactory)
, d_channelPool_p(channelPool)
, d_channelId(channelId)
, d_cacheLifespan(cacheLifespan)
, d_highWaterMark(highWaterMark)
, d_lowWaterMark(lowWaterMark)
{
    d_bufferChain_p = d_bufferChainFactory_p->allocate(0);
    setPutPosition(d_bufferChain_p->length());

    d_dataMsg.setData(d_bufferChain_p, d_bufferChainFactory_p, d_allocator_p);
    d_dataMsg.setChannelId(channelId);

    d_timeOfLastSend = d_cacheLifespan
                       ? bsls_TimeUtil::getTimer() / RESOLUTION
                       : 0;
}

btemt_ChannelOutStreamBuf::~btemt_ChannelOutStreamBuf()
{
    forceSync();

//    d_bufferChainFactory_p->deleteObject(d_bufferChain_p);
}

// MANIPULATORS

int btemt_ChannelOutStreamBuf::forceSync()
{
    int lwm = d_lowWaterMark;
    d_lowWaterMark = 0;    // guarantee that data is sent
    int rc = sync();
    d_lowWaterMark = lwm;

    return rc;
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
