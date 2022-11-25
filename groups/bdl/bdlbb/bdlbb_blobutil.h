// bdlbb_blobutil.h                                                   -*-C++-*-
#ifndef INCLUDED_BDLBB_BLOBUTIL
#define INCLUDED_BDLBB_BLOBUTIL

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a suite of utilities for I/O operations on 'bdlbb::Blob'.
//
//@CLASSES:
//  bdlbb::BlobUtil: suite of utilities on 'bdlbb::Blob'
//  bdlbb::BlobUtilAsciiDumper: helper class for ascii dump of a 'blbb::Blob'
//  bdlbb::BlobUtilHexDumper: helper class for hex dump of a 'bdlbb::Blob'
//
//@SEE_ALSO: bdlbb_blob
//
//@DESCRIPTION: This 'struct' provides a variety of utilities for 'bdlbb::Blob'
// objects, 'bdlbb::BlobUtil', such as I/O functions, comparison functions, and
// streaming functions.

#include <bdlscm_version.h>

#include <bdlbb_blob.h>

#include <bslma_allocator.h>

#include <bsls_assert.h>
#include <bsls_performancehint.h>
#include <bsls_review.h>

#include <bsl_algorithm.h>
#include <bsl_cstring.h>
#include <bsl_iosfwd.h>
#include <bsl_utility.h>

namespace BloombergLP {
namespace bdlbb {

                              // ===============
                              // struct BlobUtil
                              // ===============

struct BlobUtil {
    // This 'struct' is a namespace for a collection of static methods used
    // for manipulating and accessing 'Blob' objects.

    // CLASS METHODS
    static void append(Blob *dest, const Blob& source, int offset, int length);
        // Append the specified 'length' bytes from the specified 'offset' in
        // the specified 'source' to the specified 'dest'.

    static void append(Blob *dest, const Blob& source, int offset);
        // Append from the specified 'offset' in the specified 'source' to the
        // specified 'dest'.

    static void append(Blob *dest, const Blob& source);
        // Append the specified 'source' to the specified 'dest'.

    static void append(Blob *dest, const char *source, int offset, int length);
        // Append the specified 'length' bytes starting from the specified
        // 'offset' from the specified 'source' address to the specified
        // 'dest'.  The behavior of this function is undefined unless the range
        // '[source + offset, source + offset + length)' represents a readable
        // sequence of memory.

    static void append(Blob *dest, const char *source, int length);
        // Append the specified 'length' bytes starting from the specified
        // 'source' address to the specified 'dest'.  The behavior is undefined
        // unless the range '[source, source + length)' is valid memory.

    static void appendWithCapacityBuffer(Blob       *dest,
                                         BlobBuffer *buffer,
                                         const char *source,
                                         int         length);
        // Append the specified 'length' bytes from the specified 'source'
        // address to the specified 'dest'.  Use the existing capacity in
        // 'dest' first, followed by that in the 'buffer', and finally allocate
        // from the blob buffer factory associated with the 'dest'.  Load any
        // unused space into the specified 'buffer'.  The behavior is undefined
        // unless the range '[source, source + length)' represents a readable
        // sequence of memory.

    static void erase(Blob *blob, int offset, int length);
        // Erase the specified 'length' bytes starting at the specified
        // 'offset' from the specified 'blob'.  The behavior is undefined
        // unless 'offset >= 0', 'length >= 0', and
        // 'offset + length <= blob->length()'.

    static void insert(Blob        *dest,
                       int          destOffset,
                       const Blob&  source,
                       int          sourceOffset,
                       int          sourceLength);
        // Insert the specified 'sourceLength' bytes from the specified
        // 'sourceOffset' in the specified 'source' to the specified
        // 'destOffset' in the specified 'dest'.

    static void insert(Blob        *dest,
                       int          destOffset,
                       const Blob&  source,
                       int          sourceOffset);
        // Insert from the specified 'sourceOffset' in the specified 'source'
        // to the specified 'destOffset' in the specified 'dest'.

    static void insert(Blob *dest, int destOffset, const Blob& source);
        // Insert the specified 'source' to the specified 'destOffset' in the
        // specified 'dest'.

    static bsl::pair<int, int> findBufferIndexAndOffset(const Blob& blob,
                                                        int         position);
        // Return a value, designated here as 'p', such that for the specified
        // 'blob', 'blob.buffer(p.first)' is the buffer that contains the byte
        // at the specified 'position' in 'blob', and 'p.second' is the offset
        // corresponding to 'position' within said buffer.  The behavior of
        // this function is undefined unless '0 <= position',
        // '0 < blob.totalSize()', and 'position < blob.totalSize()'.  Note
        // that (1) subsequent changes to 'blob' may invalidate the result of
        // this function, and (2) 'p.first' never indicates a zero-size buffer.

    static void copy(char        *dstBuffer,
                     const Blob&  srcBlob,
                     int          position,
                     int          length);
        // Copy the specified 'length' bytes starting at the specified
        // 'position' in the specified 'srcBlob' to the specified 'dstBuffer'.
        // The behavior of this function is undefined unless '0 <= length',
        // '0 <= position', 'position <= srcBlob.totalSize() - length', and
        // 'dstBuffer' has room for 'length' bytes.  Note that this function
        // does *not* set 'dstBuffer[length]' to 0.

    static void copy(Blob       *dst,
                     int         dstOffset,
                     const char *src,
                     int         length);
        // Copy into the specified 'dst' starting at the specified 'dstOffset'
        // the specified 'length' bytes from the specified 'src'.  The behavior
        // is undefined unless '0 <= dstOffset', '0 <= length',
        // 'dst || 0 == length', 'src || 0 == length',
        // '!dst || dstOffset <= dst->length() - length', and 'src' refers to a
        // buffer with at least 'length' bytes.

    static void copy(Blob        *dst,
                     int          dstOffset,
                     const Blob&  src,
                     int          srcOffset,
                     int          length);
        // Copy into the specified 'dst' starting at the specified 'dstOffset'
        // the specified 'length' bytes starting at the specified 'srcOffset'
        // in the specified 'src'.  The behavior is undefined unless
        // '0 <= dstOffset', '0 <= srcOffset', '0 <= length',
        // 'dst || 0 == length', '!dst || dstOffset <= dst->length() - length',
        // and 'srcOffset <= src->length() - length'.

    static char *getContiguousRangeOrCopy(char        *dstBuffer,
                                          const Blob&  srcBlob,
                                          int          position,
                                          int          length,
                                          int          alignment = 1);
        // Return the address of the byte at the specified 'position' in the
        // specified 'srcBlob', if that address is aligned to the optionally
        // specified 'alignment' and the specified 'length' bytes are stored
        // contiguously; otherwise, *copy* 'length' bytes to the specified
        // buffer 'dstBuffer', and return 'dstBuffer'.  If alignment is not
        // specified, none is enforced.  (An address is aligned to A if, when
        // converted to an integral value 'a', 'a & (A - 1)' is 0.)  The
        // behavior of this function is undefined unless '0 < length',
        // '0 <= position', 'alignment' is a power of two, 'dstBuffer' is
        // aligned as required, 'dstBuffer' has room for 'length' bytes, and
        // 'position <= srcBlob.totalSize() - length'.

    static char *getContiguousDataBuffer(Blob              *blob,
                                         int                addLength,
                                         BlobBufferFactory *factory);
        // Obtain contiguous storage for at least the specified 'addLength'
        // bytes in the specified 'blob' at position 'blob->length()', and then
        // grow 'blob->length()' by 'addLength'.  If, upon entry, such storage
        // does not exist in 'blob', first trim the final data buffer, if any,
        // and insert a new buffer obtained from the specified 'factory'.
        // Return a pointer to the beginning of the storage obtained.  The
        // behavior of this function is undefined unless '0 < addLength', and
        // 'factory->allocate()', if called, yields a block of memory of a size
        // at least as large as 'addLength'.

    static bsl::ostream& asciiDump(bsl::ostream& stream, const Blob& source);
        // Write to the specified 'stream' an ascii dump of the specified
        // 'source', and return a reference to the modifiable 'stream'.

    static bsl::ostream& hexDump(bsl::ostream& stream, const Blob& source);
        // Write to the specified 'stream' a hexdump of the specified 'source',
        // and return a reference to the modifiable 'stream'.

    static bsl::ostream& hexDump(bsl::ostream& stream,
                                 const Blob&   source,
                                 int           offset,
                                 int           length);
        // Write to the specified 'stream' a hexdump of the specified 'length'
        // bytes of the specified 'source' starting at the specified 'offset',
        // and return a reference to the modifiable 'stream'.

    static void padToAlignment(Blob *dest,
                               int   alignment,
                               char  fillChar = '\0');
        // Append padding bytes to the specified 'dest' so that its resulting
        // length is an integer multiple of the specified 'alignment'.
        // Optionally specify 'fillChar' with which the padding is to be
        // filled.  If 'fillChar' is not specified, a 0 byte will be used.  The
        // behavior is undefined unless 'alignment' is a power of 2, and less
        // than or equal to 64.

    static void prependWithCapacityBuffer(Blob       *dest,
                                          BlobBuffer *buffer,
                                          const char *source,
                                          int         length);
        // Prepend the specified 'length' bytes from the specified 'source'
        // address to the specified 'dest'.  Use the existing capacity in
        // 'dest' first if '0 == dest->length()', followed by that in the
        // 'buffer', and finally allocate from the blob buffer factory
        // associated with the 'dest'.  Load any unused space into the
        // specified 'buffer'.  The behavior is undefined unless the range
        // '[source, source + length)' represents a readable sequence of
        // memory.

    template <class STREAM>
    static STREAM& read(STREAM& stream, Blob *dest, int numBytes);
        // Read the specified 'numBytes' from the specified 'stream' and load
        // it into the specified 'dest', and return a reference to the
        // modifiable 'stream'.

    template <class STREAM>
    static STREAM& write(STREAM& stream, const Blob& source);
        // Write the specified 'source' to the specified 'stream', and return a
        // reference to the modifiable 'stream'.

    template <class STREAM>
    static int write(STREAM&     stream,
                     const Blob& source,
                     int         sourcePosition,
                     int         numBytes);
        // Write to the specified 'stream' the specified 'numBytes' starting at
        // the specified 'sourcePosition' in the specified 'source' blob.
        // Return 0 on success or a non-zero value otherwise.  Note that this
        // function will fail (immediately) if the length of 'source' is less
        // than 'numBytes'; or if there is any error writing to 'stream'.

    static int compare(const Blob& a, const Blob& b);
        // Compare, lexicographically, the data (data length and character data
        // values at each index position) stored by the specified 'a' and 'b'
        // blobs.  Return 0 if the data stored by 'a' is lexicographically
        // equal to the data stored by 'b', a negative value if 'a' is
        // lexicographically less than 'b', and a positive value if 'a' is
        // lexicographically greater than 'b'.

    static int appendBufferIfValid(Blob *dest, const BlobBuffer& buffer);
        // Append the specified 'buffer' after the last buffer of the specified
        // 'dest' if neither the resulting total size of 'dest' nor its
        // resulting total number of buffers exceeds 'INT_MAX'.  Return 0 on
        // success, and a non-zero value (with no effect) otherwise.  The
        // length of the 'dest' is unaffected.

    static int appendBufferIfValid(Blob                          *dest,
                                   bslmf::MovableRef<BlobBuffer>  buffer);
        // Append the specified move-insertable 'buffer' after the last buffer
        // of the specified 'dest' if neither the resulting total size of
        // 'dest' nor its resulting total number of buffers exceeds 'INT_MAX'.
        // Return 0 on success, and a non-zero value (with no effect)
        // otherwise.  The length of the 'dest' is unaffected.  In case of
        // success the 'buffer' is left in a valid but unspecified state.

    static int appendDataBufferIfValid(Blob *dest, const BlobBuffer& buffer);
        // Append the specified 'buffer' after the last *data* buffer of the
        // specified 'dest' if neither the resulting total size of 'dest' nor
        // its resulting total number of buffers exceeds 'INT_MAX'.  Return 0
        // on success, and a non-zero value (with no effect) otherwise.  The
        // last data buffer of the 'dest' is trimmed, if necessary.  The length
        // of the 'dest' is incremented by the size of 'buffer'.

    static int appendDataBufferIfValid(Blob                          *dest,
                                       bslmf::MovableRef<BlobBuffer>  buffer);
        // Append the specified move-insertable 'buffer' after the last *data*
        // buffer of the specified 'dest' if neither the resulting total size
        // of 'dest' nor its resulting total number of buffers exceeds
        // 'INT_MAX'.  Return 0 on success, and a non-zero value (with no
        // effect) otherwise.  The last data buffer of the 'dest' is trimmed,
        // if necessary.  The length of the 'dest' is incremented by the size
        // of 'buffer'.  In case of success the 'buffer' is left in a valid but
        // unspecified state.

    static int insertBufferIfValid(Blob              *dest,
                                   int                index,
                                   const BlobBuffer&  buffer);
        // Insert the specified 'buffer' at the specified 'index' in the
        // specified 'dest' if '0 <= index <= dest->numBuffers()' and neither
        // the resulting total size of 'dest' nor its resulting total number of
        // buffers exceeds 'INT_MAX'.  Return 0 on success, and a non-zero
        // value (with no effect) otherwise.  Increment the length of the 'dest
        // by the size of the 'buffer' if 'buffer' is inserted *before* the
        // logical end of the 'dest'.  The length of the 'dest' is _unchanged_
        // if inserting at a position following all data buffers (e.g.,
        // inserting into an empty blob or inserting a buffer to increase
        // capacity); in that case, the blob length must be changed by an
        // explicit call to 'setLength'.  Buffers at 'index' and higher
        // positions (if any) are shifted up by one index position.

    static int insertBufferIfValid(Blob                          *dest,
                                   int                            index,
                                   bslmf::MovableRef<BlobBuffer>  buffer);
        // Insert the specified move-insertable 'buffer' at the specified
        // 'index' in the specified 'dest' if
        // '0 <= index <= dest->numBuffers()' and neither the resulting total
        // size of 'dest' nor its resulting total number of buffers exceeds
        // 'INT_MAX'.  Return 0 on success, and a non-zero value (with no
        // effect) otherwise.  Increment the length of the 'dest by the size of
        // the 'buffer' if 'buffer' is inserted *before* the logical end of the
        // 'dest'.  The length of the 'dest' is _unchanged_ if inserting at a
        // position following all data buffers (e.g., inserting into an empty
        // blob or inserting a buffer to increase capacity); in that case, the
        // blob length must be changed by an explicit call to 'setLength'.
        // Buffers at 'index' and higher positions (if any) are shifted up by
        // one index position.  In case of success the 'buffer' is left in a
        // valid but unspecified state.

    static int prependDataBufferIfValid(Blob *dest, const BlobBuffer& buffer);
        // Insert the specified 'buffer' before the beginning of the specified
        // 'dest' if neither the resulting total size of 'dest' nor its
        // resulting total number of buffers exceeds 'INT_MAX'.  Return 0 on
        // success, and a non-zero value (with no effect) otherwise.  The
        // length of the 'dest' is incremented by the length of the prepended
        // buffer.

    static int prependDataBufferIfValid(Blob                          *dest,
                                        bslmf::MovableRef<BlobBuffer>  buffer);
        // Insert the specified move-insertable 'buffer' before the beginning
        // of the specified 'dest' if neither the resulting total size of
        // 'dest' nor its resulting total number of buffers exceeds 'INT_MAX'.
        // Return 0 on success, and a non-zero value (with no effect)
        // otherwise.  The length of the 'dest' is incremented by the length of
        // the prepended buffer.  In case of success the 'buffer' is left in a
        // valid but unspecified state.

    // ---------- DEPRECATED FUNCTIONS ------------- //

    // DEPRECATED FUNCTIONS: basicAllocator is no longer used
    static void append(Blob             *dest,
                       const Blob&       source,
                       int               offset,
                       int               length,
                       bslma::Allocator *);

    static void append(Blob             *dest,
                       const Blob&       source,
                       int               offset,
                       bslma::Allocator *);

    static void append(Blob *dest, const Blob& source, bslma::Allocator *);
};

                         // ==========================
                         // struct BlobUtilAsciiDumper
                         // ==========================

struct BlobUtilAsciiDumper {
    // Utility for ascii dumping a blob to standard output streams.  This class
    // has 'operator<<' defined for it, so it can be used, for example, in
    // 'ball' logs.

    // DATA
    const Blob *d_blob_p;

    // CREATORS
    explicit BlobUtilAsciiDumper(const Blob *blob);
        // Create an ascii dumper for the specified 'blob'.
};

// FREE OPERATORS
bsl::ostream& operator<<(bsl::ostream& stream, const BlobUtilAsciiDumper& rhs);
    // Ascii-dump the blob referenced by the specified 'rhs' to the specified
    // 'stream', and return a reference to the modifiable 'stream'.

                          // ========================
                          // struct BlobUtilHexDumper
                          // ========================

struct BlobUtilHexDumper {
    // Utility for hex dumping a blob to standard output streams.  This class
    // has 'operator<<' defined for it, so it can be used, for example, in
    // 'ball' logs.

    // DATA
    const Blob *d_blob_p;
    int         d_offset;
    int         d_length;

    // CREATORS
    explicit BlobUtilHexDumper(const Blob *blob);
        // Create a hex dumper for the specified 'blob'.

    BlobUtilHexDumper(const Blob *blob, int offset, int length);
        // Create a hex dumper for the specified 'blob' that dumps the
        // specified 'length' bytes starting at the specified 'offset'.
};

// FREE OPERATORS
bsl::ostream& operator<<(bsl::ostream& stream, const BlobUtilHexDumper& rhs);
    // Hex-dump the blob referenced by the specified 'rhs' to the specified
    // 'stream', and return a reference to the modifiable 'stream'.

// ============================================================================
//                             INLINE DEFINITIONS
// ============================================================================

                              // ---------------
                              // struct BlobUtil
                              // ---------------

// CLASS METHODS
inline
void BlobUtil::append(Blob *dest, const Blob& source, int offset)
{
    append(dest, source, offset, source.length() - offset);
}

inline
void BlobUtil::append(Blob *dest, const Blob& source)
{
    append(dest, source, 0, source.length());
}

inline
void BlobUtil::append(Blob             *dest,
                      const Blob&       source,
                      int               offset,
                      int               length,
                      bslma::Allocator *)
{
    return append(dest, source, offset, length);
}

inline
void BlobUtil::append(Blob             *dest,
                      const Blob&       source,
                      int               offset,
                      bslma::Allocator *)
{
    return append(dest, source, offset);
}

inline
void BlobUtil::append(Blob *dest, const Blob& source, bslma::Allocator *)
{
    return append(dest, source);
}

inline
void BlobUtil::append(Blob *dest, const char *source, int length)
{
    BSLS_ASSERT(0 != dest);
    BSLS_ASSERT(0 != source || 0 == length);

    if (BSLS_PERFORMANCEHINT_PREDICT_LIKELY(dest->numDataBuffers())) {
        const int         lastDataBufIdx = dest->numDataBuffers() - 1;
        const BlobBuffer& lastBuf        = dest->buffer(lastDataBufIdx);
        const int         offsetInBuf    = dest->lastDataBufferLength();
        if (BSLS_PERFORMANCEHINT_PREDICT_LIKELY(lastBuf.size() - offsetInBuf >=
                                                length)) {
            dest->setLength(dest->length() + length);
            bsl::memcpy(lastBuf.buffer().get() + offsetInBuf, source, length);
            return;                                                   // RETURN
        }
    }
    BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
    append(dest, source, 0, length);
}

inline
void BlobUtil::insert(Blob        *dest,
                      int          destOffset,
                      const Blob&  source,
                      int          sourceOffset)
{
    insert(dest,
           destOffset,
           source,
           sourceOffset,
           source.length() - sourceOffset);
}

inline
void BlobUtil::insert(Blob *dest, int destOffset, const Blob& source)
{
    insert(dest, destOffset, source, 0, source.length());
}

inline
bsl::ostream& BlobUtil::hexDump(bsl::ostream& stream, const Blob& source)
{
    return hexDump(stream, source, 0, source.length());
}

inline
void BlobUtil::padToAlignment(Blob *dest, int alignment, char fillChar)
{
    BSLS_ASSERT(0 != dest);
    BSLS_ASSERT(static_cast<unsigned>(alignment) <= 64);

    const int modMask = alignment - 1;

    BSLS_ASSERT(0 == (alignment & modMask));    // power of 2

    const int padLength = (alignment - (dest->length() & modMask)) & modMask;
    char padBuffer[63];
    bsl::memset(padBuffer, fillChar, padLength);

    append(dest, padBuffer, padLength);
}

template <class STREAM>
STREAM& BlobUtil::read(STREAM& stream, Blob *dest, int numBytes)
{
    BSLS_ASSERT(0 != dest);

    dest->setLength(numBytes);

    for (int numBytesRemaining = numBytes, i = 0; 0 < numBytesRemaining; ++i) {
        BSLS_ASSERT(i < dest->numBuffers());

        BlobBuffer buffer = dest->buffer(i);

        const int bytesToRead = numBytesRemaining < buffer.size()
                                    ? numBytesRemaining
                                    : buffer.size();

        stream.getArrayInt8(buffer.data(), bytesToRead);

        numBytesRemaining -= bytesToRead;
    }

    return stream;
}

template <class STREAM>
STREAM& BlobUtil::write(STREAM& stream, const Blob& source)
{
    int numBytes = source.length();

    for (int numBytesRemaining = numBytes, i = 0; 0 < numBytesRemaining; ++i) {
        BSLS_ASSERT(i < source.numBuffers());

        BlobBuffer buffer = source.buffer(i);

        const int bytesToWrite = numBytesRemaining < buffer.size()
                                     ? numBytesRemaining
                                     : buffer.size();

        stream.putArrayInt8(buffer.data(), bytesToWrite);

        numBytesRemaining -= bytesToWrite;
    }

    return stream;
}

template <class STREAM>
int BlobUtil::write(STREAM&     stream,
                    const Blob& source,
                    int         sourcePosition,
                    int         numBytes)
{
    BSLS_ASSERT(0 <= sourcePosition);
    BSLS_ASSERT(0 <= numBytes);

    if (sourcePosition + numBytes > source.length()) {
        return -1;                                                    // RETURN
    }

    if (sourcePosition == 0 && numBytes == 0) {
        return 0;                                                     // RETURN
    }

    int bufferIndex  = 0;
    int bytesSkipped = 0;
    while (bytesSkipped + source.buffer(bufferIndex).size() <=
           sourcePosition) {
        bytesSkipped += source.buffer(bufferIndex).size();
        ++bufferIndex;
    }

    int bytesRemaining = numBytes;
    while (0 < bytesRemaining) {
        const BlobBuffer& buffer = source.buffer(bufferIndex);

        const int startingIndex = 0 < bytesSkipped || 0 == bufferIndex
                                      ? sourcePosition - bytesSkipped
                                      : 0;

        const int bytesToCopy = bytesRemaining > buffer.size() - startingIndex
                                    ? buffer.size() - startingIndex
                                    : bytesRemaining;

        stream.putArrayInt8(buffer.data() + startingIndex, bytesToCopy);
        if (!stream) {
            return -1;                                                // RETURN
        }

        bytesRemaining -= bytesToCopy;
        bytesSkipped = 0;
        ++bufferIndex;
    }

    BSLS_ASSERT(bytesRemaining == 0);
    return 0;
}

inline
int BlobUtil::appendBufferIfValid(Blob *dest, const BlobBuffer& buffer)
{
    BlobBuffer objectToMove(buffer);
    return appendBufferIfValid(dest,
                               bslmf::MovableRefUtil::move(objectToMove));
}

inline
int BlobUtil::appendBufferIfValid(Blob                          *dest,
                                  bslmf::MovableRef<BlobBuffer>  buffer)
{
    BlobBuffer& lvalue = buffer;

    if (dest->totalSize() <= INT_MAX - lvalue.size()
    && (dest->numBuffers() < INT_MAX)) {
        dest->appendBuffer(bslmf::MovableRefUtil::move(buffer));
        return 0;                                                     // RETURN
    }
    return -1;
}

inline
int BlobUtil::appendDataBufferIfValid(Blob *dest, const BlobBuffer& buffer)
{
    BlobBuffer objectToMove(buffer);
    return appendDataBufferIfValid(dest,
                                   bslmf::MovableRefUtil::move(objectToMove));
}

inline
int BlobUtil::appendDataBufferIfValid(Blob                          *dest,
                                      bslmf::MovableRef<BlobBuffer>  buffer)
{
    // Last data buffer can be trimmed during appending new buffer.  Therefore,
    // the potentially allowed size of the added buffer should be adjusted
    // accordingly.

    BlobBuffer& lvalue = buffer;

    const int TRIMMED_SIZE =
        0 == dest->numDataBuffers()
            ? 0
            : dest->buffer(dest->numDataBuffers() - 1).size() -
                  dest->lastDataBufferLength();

    if ((dest->totalSize() - TRIMMED_SIZE <= INT_MAX - lvalue.size())
     && (dest->numBuffers() < INT_MAX)) {

        dest->appendDataBuffer(bslmf::MovableRefUtil::move(lvalue));
        return 0;                                                     // RETURN
    }
    return -1;
}

inline
int BlobUtil::insertBufferIfValid(Blob              *dest,
                                  int                index,
                                  const BlobBuffer&  buffer)
{
    BlobBuffer objectToMove(buffer);
    return insertBufferIfValid(dest,
                               index,
                               bslmf::MovableRefUtil::move(objectToMove));
}

inline
int BlobUtil::insertBufferIfValid(Blob                          *dest,
                                  int                            index,
                                  bslmf::MovableRef<BlobBuffer>  buffer)
{
    BlobBuffer& lvalue = buffer;

    if (0 <= index
     && dest->numBuffers() >= index
     && (dest->totalSize() <= INT_MAX - lvalue.size())
     && (dest->numBuffers() < INT_MAX)) {
        dest->insertBuffer(index, bslmf::MovableRefUtil::move(lvalue));
        return 0;                                                     // RETURN
    }
    return -1;
}

inline
int BlobUtil::prependDataBufferIfValid(Blob *dest, const BlobBuffer& buffer)
{
    BlobBuffer objectToMove(buffer);
    return prependDataBufferIfValid(dest,
                                    bslmf::MovableRefUtil::move(objectToMove));
}

inline
int BlobUtil::prependDataBufferIfValid(Blob                          *dest,
                                       bslmf::MovableRef<BlobBuffer>  buffer)
{
    BlobBuffer& lvalue = buffer;

    int bufferSize = lvalue.size();
    if ((dest->totalSize() <= INT_MAX - bufferSize)
     && (dest->numBuffers() < INT_MAX)) {
        dest->prependDataBuffer(bslmf::MovableRefUtil::move(lvalue));
        return 0;                                                     // RETURN
    }
    return -1;
}

                         // --------------------------
                         // struct BlobUtilAsciiDumper
                         // --------------------------

// CREATORS
inline
BlobUtilAsciiDumper::BlobUtilAsciiDumper(const Blob *blob)
: d_blob_p(blob)
{
}
}  // close package namespace

// FREE OPERATORS
inline
bsl::ostream& bdlbb::operator<<(bsl::ostream&              stream,
                                const BlobUtilAsciiDumper& rhs)
{
    return BlobUtil::asciiDump(stream, *rhs.d_blob_p);
}

namespace bdlbb {

                          // ------------------------
                          // struct BlobUtilHexDumper
                          // ------------------------

// CREATORS
inline
BlobUtilHexDumper::BlobUtilHexDumper(const Blob *blob)
: d_blob_p(blob)
, d_offset(0)
, d_length(blob->length())
{
}

inline
BlobUtilHexDumper::BlobUtilHexDumper(const Blob *blob, int offset, int length)
: d_blob_p(blob)
, d_offset(offset)
, d_length((bsl::min)(length, blob->length() - offset))
{
}
}  // close package namespace

// FREE OPERATORS
inline
bsl::ostream& bdlbb::operator<<(bsl::ostream&            stream,
                                const BlobUtilHexDumper& rhs)
{
    return BlobUtil::hexDump(
        stream, *rhs.d_blob_p, rhs.d_offset, rhs.d_length);
}

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
