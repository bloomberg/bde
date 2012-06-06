// bcesb_pooledbufferchainstreambuf.cpp        -*-C++-*-
#include <bcesb_pooledbufferchainstreambuf.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bcesb_pooledbufferchainstreambuf_cpp,"$Id$ $CSID$")

#include <bcema_pooledbufferchain.h>

#include <bsls_assert.h>

#include <bsl_cstdio.h>

// Note: on Windows -> WinDef.h:#define min(a,b) ...
#if defined(BSLS_PLATFORM__CMP_MSVC) && defined(min)
#undef min
#endif

namespace BloombergLP {

                   // --------------------------------------
                   // class bcesb_PooledBufferChainStreamBuf
                   // --------------------------------------

// PRIVATE MANIPULATORS
void bcesb_PooledBufferChainStreamBuf::setGetPosition(int position)
{
    // Untested precondition: position <= d_pooledBufferChain_p->length()

    const int totalSize = d_pooledBufferChain_p->length();
    const int bufferSize = d_pooledBufferChain_p->bufferSize();
    const int numBuffers = d_pooledBufferChain_p->numBuffers();

    d_getBufferIndex = position / bufferSize;

    if (d_getBufferIndex >= numBuffers) {
        setg(0, 0, 0);
    }
    else {
        const int basePosition = d_getBufferIndex * bufferSize;
        char *base = d_pooledBufferChain_p->buffer(d_getBufferIndex);
        char *ptr  = base + (position - basePosition);
        char *end  = base + bsl::min(bufferSize, totalSize - basePosition);

        setg(base, ptr, end);
    }
}

void bcesb_PooledBufferChainStreamBuf::setPutPosition(int position)
{
    const int bufferSize = d_pooledBufferChain_p->bufferSize();
    const int numBuffers = d_pooledBufferChain_p->numBuffers();

    d_putBufferIndex = position / bufferSize;

    if (d_putBufferIndex >= numBuffers) {
        setp(0, 0);
    }
    else {
        int basePosition = d_putBufferIndex * bufferSize;
        char *base = d_pooledBufferChain_p->buffer(d_putBufferIndex);
        setp(base, base + bufferSize);
        pbump(position - basePosition);
    }
}

// PRIVATE ACCESSORS
int bcesb_PooledBufferChainStreamBuf::checkInvariant() const
{
    const int bufferSize = d_pooledBufferChain_p->bufferSize();
    const int numBuffers = d_pooledBufferChain_p->numBuffers();

    // check get area
    if (gptr()) {
        BSLS_ASSERT(eback());
        BSLS_ASSERT(egptr());
        BSLS_ASSERT(gptr() - eback() <= egptr()-eback());
        BSLS_ASSERT(egptr() - eback() <= bufferSize);
        BSLS_ASSERT(d_getBufferIndex < numBuffers);
    }
    else {
        BSLS_ASSERT(0 == eback());
        BSLS_ASSERT(0 == egptr());
        BSLS_ASSERT(d_getBufferIndex <= numBuffers);
    }

    // check put area
    if (pptr()) {
        BSLS_ASSERT(pbase());
        BSLS_ASSERT(epptr());
        BSLS_ASSERT(pptr() - pbase() <= bufferSize);
        BSLS_ASSERT(epptr() - pbase() == bufferSize);
        BSLS_ASSERT(d_putBufferIndex < numBuffers);
    }
    else {
        BSLS_ASSERT(0 == pbase());
        BSLS_ASSERT(0 == epptr());
        BSLS_ASSERT(d_putBufferIndex == numBuffers);
    }

    return 0;
}

// PROTECTED MANIPULATORS
bcesb_PooledBufferChainStreamBuf::int_type
bcesb_PooledBufferChainStreamBuf::overflow(
        bcesb_PooledBufferChainStreamBuf::int_type c)
{
    BSLS_ASSERT(0 == checkInvariant());

    if (EOF == c) {
        return traits_type::not_eof(c);
    }

    if (pptr() == epptr())
    {
        const int bufferSize = d_pooledBufferChain_p->bufferSize();
        const int putPosition = calcPutPosition();
        BSLS_ASSERT(0 == putPosition % bufferSize);

        if (putPosition >= d_pooledBufferChain_p->length()) {
            d_pooledBufferChain_p->setLength(putPosition + 1);// grow the chain
        }

        d_putBufferIndex = putPosition / bufferSize;
        char *pbuf = d_pooledBufferChain_p->buffer(d_putBufferIndex);
        setp(pbuf, pbuf + bufferSize);
    }

    if (pptr() != epptr())
    {
        *pptr() = c;
        pbump(1);
        return c;
    }
    else {
        return traits_type::eof();
    }
}

bcesb_PooledBufferChainStreamBuf::int_type
bcesb_PooledBufferChainStreamBuf::pbackfail(
        bcesb_PooledBufferChainStreamBuf::int_type c)
{
    BSLS_ASSERT(0 == checkInvariant());

    if (gptr() == eback()) {
        if (0 == d_getBufferIndex) {
            return EOF;  // No put-back position available
        }
        else {
            d_getBufferIndex--;
            char *gbuf = d_pooledBufferChain_p->buffer(d_getBufferIndex);
            const int bufferSize = d_pooledBufferChain_p->bufferSize();
            setg(gbuf, gbuf + bufferSize, gbuf + bufferSize);
        }
    }

    gbump(-1);
    if (EOF == c) {
        return ~EOF;
    }
    else {
        *gptr() = c;
        return c;
    }
}

bcesb_PooledBufferChainStreamBuf::pos_type
bcesb_PooledBufferChainStreamBuf::seekpos(pos_type                position,
                                          bsl::ios_base::openmode which)
{
    return seekoff(off_type(position), bsl::ios_base::beg, which);
}

bcesb_PooledBufferChainStreamBuf::pos_type
bcesb_PooledBufferChainStreamBuf::seekoff(
        off_type                offset,
        bsl::ios_base::seekdir  fixedPosition,
        bsl::ios_base::openmode which)
{
    BSLS_ASSERT(0 == checkInvariant());

    const bool gseek = which & bsl::ios_base::in;
    const bool pseek = which & bsl::ios_base::out;

    if ((!gseek && !pseek)
     || (gseek && pseek && fixedPosition == bsl::ios_base::cur)) {
        return pos_type(-1);
    }

    privateSync();
    const int totalSize = d_pooledBufferChain_p->length();

    off_type newoff;
    switch (fixedPosition)
    {
        case bsl::ios_base::beg:
          newoff = 0;
          break;
        case bsl::ios_base::cur:
          newoff = (pseek ? calcPutPosition() : calcGetPosition());
          break;
        case bsl::ios_base::end:
          newoff = totalSize;
          break;
        default:
          return off_type(-1);
    }

    newoff += offset;
    if (newoff < 0 || totalSize < newoff) {
        return off_type(-1);
    }

    if (pseek) {
        setPutPosition(newoff);
    }

    if (gseek) {
        setGetPosition(newoff);
    }

    return newoff;
}

bsl::streamsize bcesb_PooledBufferChainStreamBuf::showmanyc()
{
    BSLS_ASSERT(0 == checkInvariant());

    privateSync();

    const int totalSize = d_pooledBufferChain_p->length();
    const int getPosition = calcGetPosition();

    return totalSize - getPosition;
}

int bcesb_PooledBufferChainStreamBuf::sync()
{
    return privateSync();
}

bcesb_PooledBufferChainStreamBuf::int_type
bcesb_PooledBufferChainStreamBuf::underflow()
{
    BSLS_ASSERT(0 == checkInvariant());

    privateSync();

    const int totalSize = d_pooledBufferChain_p->length();
    const int getPosition =  calcGetPosition();

    if (getPosition >= totalSize) {
        BSLS_ASSERT(getPosition == totalSize);
        return EOF;
    }
    else {
        const int bufferSize = d_pooledBufferChain_p->bufferSize();

        d_getBufferIndex = getPosition / bufferSize;

        char *gbuf = d_pooledBufferChain_p->buffer(d_getBufferIndex);

        setg(gbuf,
             gbuf + getPosition % bufferSize,
             gbuf + bsl::min(bufferSize,
                             totalSize - d_getBufferIndex*bufferSize));
    }

    return traits_type::to_int_type(*gptr());
}

bsl::streamsize bcesb_PooledBufferChainStreamBuf::xsgetn(
        char_type       *destination,
        bsl::streamsize  numChars)
{
    bsl::streamsize numLeft   = numChars;
    bsl::streamsize numCopied = 0;
    while (0 < numLeft) {
        bsl::streamsize remainingChars = egptr() - gptr();
        const int       canCopy = bsl::min(remainingChars, numLeft);
        bsl::memcpy(destination + numCopied, gptr(), canCopy);
        gbump(canCopy);
        numLeft -= canCopy;
        if (0 < numLeft
         && gptr() == egptr()
         && traits_type::eof() == underflow()) {
            return numCopied + canCopy;
        }
        numCopied += canCopy;
    }
    return numCopied;
}

bsl::streamsize bcesb_PooledBufferChainStreamBuf::xsputn(
        const char_type *source,
        bsl::streamsize  numChars)
{
    bsl::streamsize numLeft   = numChars;
    bsl::streamsize numCopied = 0;

    while (0 < numLeft) {
        bsl::streamsize remainingChars = epptr() - pptr();
        const int       canCopy = bsl::min(remainingChars, numLeft);
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
bcesb_PooledBufferChainStreamBuf::~bcesb_PooledBufferChainStreamBuf()
{
    privateSync();
    BSLS_ASSERT(0 == checkInvariant());
}

// MANIPULATORS
void bcesb_PooledBufferChainStreamBuf::reset(
                                    bcema_PooledBufferChain *pooledBufferChain)
{
    if (pooledBufferChain) {
        d_pooledBufferChain_p =
            bcema_SharedPtr<bcema_PooledBufferChain>
                (pooledBufferChain, bcema_SharedPtrNilDeleter(), 0);
    }
    const int totalSize = d_pooledBufferChain_p->length();
    setGetPosition(0);
    setPutPosition(totalSize);
}

               // ---------------------------------------------
               // class bcesb_PooledBufferChainStreamBufFactory
               // ---------------------------------------------

bcesb_PooledBufferChainStreamBufFactory::
                                       bcesb_PooledBufferChainStreamBufFactory(
    bcema_PooledBufferChainFactory *factory,
    bslma_Allocator                *basicAllocator)
: d_factory_p(factory)
, d_allocator_p(bslma_Default::allocator(basicAllocator))
{

}

bcesb_PooledBufferChainStreamBufFactory::
                                     ~bcesb_PooledBufferChainStreamBufFactory()
{
}

bsl::streambuf *bcesb_PooledBufferChainStreamBufFactory::createObject() {
    bcema_PooledBufferChain *data = d_factory_p->allocate(0);
    bcema_SharedPtr<bcema_PooledBufferChain> ptr(data,
                                                 d_factory_p,
                                                 d_allocator_p);

    return new (*d_allocator_p)
        bcesb_PooledBufferChainStreamBuf(ptr);
}

void bcesb_PooledBufferChainStreamBufFactory::deleteObject(
                                                    bsl::streambuf *instance) {
    d_allocator_p->deleteObject(instance);
}

}  // close namespace BloombergLP

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2007
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ----------------------------------
