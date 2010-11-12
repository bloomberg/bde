// bdesb_memoutstreambuf.cpp               -*-C++-*-
#include <bdesb_memoutstreambuf.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bdesb_memoutstreambuf_cpp,"$Id$ $CSID$")


#include <bsls_assert.h>

#include <bsl_cstring.h>

namespace BloombergLP {

                         // ---------------------------
                         // class bdesb_MemOutStreamBuf
                         // ---------------------------

// PRIVATE MANIPULATORS
void bdesb_MemOutStreamBuf::grow(int newLength)
{
    int newCapacity = capacity();
    if (0 == newCapacity) {
        newCapacity = BDESB_INITIAL_BUFFER_SIZE;
    }

    while (newCapacity < newLength) {
        newCapacity *= BDESB_GROWTH_FACTOR;
    }

    reserveCapacity(newCapacity);
}

// PROTECTED MANIPULATORS
int bdesb_MemOutStreamBuf::overflow(int_type insertionChar)
{
    if (traits_type::eof() == insertionChar) {
        return traits_type::not_eof(insertionChar);                   // RETURN
    }

    grow(capacity() + 1);
    return sputc(insertionChar);
}

bdesb_MemOutStreamBuf::pos_type
bdesb_MemOutStreamBuf::seekoff(bdesb_MemOutStreamBuf::off_type offset,
                               bsl::ios_base::seekdir          fixedPosition,
                               bsl::ios_base::openmode         which)
{
    // This is an output-only buffer, so cannot "seek" in "get" area.

    if (!(which & bsl::ios_base::out)) {
        return pos_type(-1);                                          // RETURN
    }

    // Compute offset from current position.  In this stream, 'pptr()' defines
    // both the current position and the end of the logical byte stream.
    // Thus, 'bsl::ios_base::curr' and 'bsl::ios_base::end' are handled
    // identically.

    off_type currOffset = bsl::ios_base::beg == fixedPosition
                          ? offset - length()
                          : offset;

    // 'currOffset' is invalid if it is positive or has an absolute-value
    // greater than 'length()'.

    if (currOffset > 0 || -currOffset > length()) {
        return pos_type(-1);                                          // RETURN
    }

    pbump(currOffset);

    return pos_type(length());
}

bdesb_MemOutStreamBuf::pos_type
bdesb_MemOutStreamBuf::seekpos(bdesb_MemOutStreamBuf::pos_type position,
                               bsl::ios_base::openmode         which)
{
    return seekoff(off_type(position), bsl::ios_base::beg, which);
}

bsl::streamsize bdesb_MemOutStreamBuf::xsputn(const char_type *source,
                                              bsl::streamsize  numChars)
{
    const int newLength = length() + numChars;
    if (newLength > capacity()) {
        grow(newLength);
    }

    bsl::memcpy(pptr(), source, numChars);
    pbump(numChars);
    return numChars;
}

// MANIPULATORS
void bdesb_MemOutStreamBuf::reserveCapacity(int numCharacters)
{
    BSLS_ASSERT(0 <= numCharacters);

    if (numCharacters <= capacity()) {
        return;                                                       // RETURN
    }

    char_type *oldBuffer = pbase();
    char_type *newBuffer = (char_type *)d_allocator_p->allocate(numCharacters);

    // Copy over existing characters.

    const int oldLength = length();
    bsl::memcpy(newBuffer, oldBuffer, oldLength * sizeof(char_type));
    d_allocator_p->deallocate(oldBuffer);

    // Reset data members appropriately.

    setp(newBuffer, newBuffer + numCharacters);
    pbump(oldLength);
}

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2004
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
