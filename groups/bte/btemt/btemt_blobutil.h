// btemt_blobutil.h                  -*-C++-*-
#ifndef INCLUDED_BTEMT_BLOBUTIL
#define INCLUDED_BTEMT_BLOBUTIL

//@PURPOSE:
//
//@AUTHOR: somebody,
//         Kevin McMahon (kmcmahon)
//
//@CLASSES:
//
//@DESCRIPTION:

#ifndef INCLUDED_BCEMA_BLOB
#include <bcema_blob.h>
#endif

#ifndef INCLUDED_BCEMA_BLOBUTIL
#include <bcema_blobutil.h>
#endif

#ifndef INCLUDED_BCEMA_SHAREDPTR
#include <bcema_sharedptr.h>
#endif

#ifndef INCLUDED_BDEX_BYTEINSTREAM
#include <bdex_byteinstream.h>
#endif

#ifndef INCLUDED_BDEX_BYTEOUTSTREAM
#include <bdex_byteoutstream.h>
#endif

#ifndef INCLUDED_BSLSTL_BIDIRECTIONALITERATOR
#include <bslstl_bidirectionaliterator.h>
#endif

#ifndef INCLUDED_BSL_VECTOR
#include <bsl_vector.h>
#endif

namespace BloombergLP {

class btes_Ovec;


namespace bslma { class Allocator; }

namespace btemt {

                       // ===========================
                        // class BlobUtil_IteratorImp
                        // ==========================

template <typename T> class BlobUtil_IteratorImp {
     template <typename U> friend inline bool operator==(
                                           const BlobUtil_IteratorImp<U>& lhs,
                                           const BlobUtil_IteratorImp<U>& rhs);

    const bcema_Blob *d_blob_p; // the blob
    int               d_buffer; // current buffer
    int               d_offset; // current offset within the buffer

public:
    BlobUtil_IteratorImp();
    BlobUtil_IteratorImp(const bcema_Blob *blob, int buffer, int offset);
    BlobUtil_IteratorImp(const BlobUtil_IteratorImp&);
    ~BlobUtil_IteratorImp();

    BlobUtil_IteratorImp& operator=(const BlobUtil_IteratorImp&);
    void operator++();
    void operator--();

    // ACCESSORS
    T& operator*() const;
};

// FREE OPERATORS

template <typename T>
inline
bool operator==(const BlobUtil_IteratorImp<T>& lhs,
                const BlobUtil_IteratorImp<T>& rhs);


                        // ==============
                        // class BlobUtil
                        // ==============

struct BlobUtil {
    // PUBLIC TYPES
    typedef bslstl::BidirectionalIterator<char,
                                         BlobUtil_IteratorImp<char> > iterator;
    typedef bslstl::BidirectionalIterator<
                                   const char,
                                   BlobUtil_IteratorImp<char> > const_iterator;
        // Bidirectional iterator types for bcema_Blob.

    // STATIC METHODS
    static void copyOut(char              *buffer,
                        const bcema_Blob&  blob,
                        int                numBytes,
                        int                offset = 0);
        // Copy into the specified 'buffer' the specified 'numBytes' from this
        // 'blob' starting at the optionally specified 'offset'.  The behavior
        // is undefined unless 'buffer' refers to at least 'numBytes' writable
        // bytes, '0 <= numBytes', '0 <= offset' and
        // 'offset + numBytes <= blob.length()'.

    static void copyToVector(bsl::vector<char> *result,
                             const bcema_Blob&  blob);
        // Copy the contents of the specified 'blob' into the specified
        // 'result' vector.

    static void extract(char *buffer, int numBytes, bcema_Blob *blob);
        // Copy into the specified 'buffer' the specified 'numBytes' from this
        // 'blob'.  'numBytes' from the front of 'blob' will be removed.
        // The behavior is undefined unless 'buffer' refers to at
        // least 'numBytes' writable bytes and is not part of this buffer
        // chain.

    static const char *getData(char              *storage,
                               int                numBytes,
                               const bcema_Blob&  blob,
                               int                blobOffset = 0);
        // Returns pointer to a buffer containing the first specified
        // 'numBytes' of the specified 'blob'.  May use the specified 'storage'
        // to store the bytes.  Optionally specify an byte offset into 'blob'
        // to specify where to start reading from.  The behavior is undefined
        // unless 'storage' refers to at least 'numBytes' writable bytes, 
        // 'storage' does not refer to any part of 'blob', and
        // 'numBytes <= 'blob.length() - blobOffset'.

    static void prepend(bcema_Blob        *destination,
                        const bcema_Blob&  source);
        // Prepend to the specified 'destination', the data from the specified
        // 'source'.

    static void prepend(bcema_Blob        *destination,
                        const bcema_Blob&  source,
                        int                offset);
        // Prepend to the specified 'destination', the data from the specified
        // 'source' starting at the specified 'offset' into 'source'.  The
        // behavior is undefined unless '0 <= offset' and
        // 'offset <= source.length()'.

    static void prepend(bcema_Blob        *destination,
                        const bcema_Blob&  source,
                        int                offset,
                        int                length);
        // Prepend to the specified 'destination', the specified 'length' of
        // bytes from the specified 'source' starting at the specified 'offset'
        // in 'source'.  The behavior is undefined unless '0 <= offset',
        // '0 <= length', and 'offset + length <= source.length()'.

    static void prepend(bcema_Blob        *destination,
                        const char        *source,
                        int                length);
        // Prepend to the specified 'destination', the specified 'length' bytes
        // from the specified 'source'.  The behavior is undefined unless
        // '0 <= length'.

    static int dataBufferSize(const bcema_Blob& blob,
                              int               bufferIndex);
        // Returns the size of the data buffer in the specified 'blob' at the
        // specified 'bufferIndex'.  If it is the last data buffer, the size is
        // 'blob.lastDataBufferLength'(), otherwise it is the size of that
        // 'bcema_BlobBuffer'.

    static void getDataBuffer(char              **buf,
                              int                *bufLength,
                              const bcema_Blob&   blob,
                              int                 bufferIndex);
        // Load and 'buf' and 'bufLength' with the data of the data buffer of
        // the 'blob' at 'bufferIndex'.

    static char *appendEmptyBuffer(bcema_Blob       *blob,
                                   int               size,
                                   bslma::Allocator  *allocator = 0);
        // Allocates an empty buffer of the specified 'size' and appends it to
        // the specified 'blob'.  Returns a pointer to the empty buffer.
        // Note that 'blob' does not need to possess a
        // 'bcema_BlobBufferFactory'.

    static void generateEmptyBuffer(
                                  char                    **buffer,
                                  int                      *size,
                                  bcema_Blob               *blob,
                                  bcema_BlobBufferFactory  *blobBufferFactory);
        // Loads the specified 'buffer' and 'size' with the address and size of
        // the first available empty range of data in the specified 'blob'.  If
        // the 'blob' is filled to capacity, uses the specified
        // 'blobBufferFactory' to add a new empty buffer.  Note that once
        // 'buffer' is written to, the length of 'blob' must be adjusted
        // (with 'bcema_Blob::setLength').

    static iterator beginIterator(bcema_Blob *blob);
    static const_iterator beginIterator(const bcema_Blob *blob);
        // Returns an bidirectional iterator to the start of the specified
        // 'blob'.

    static iterator endIterator(bcema_Blob *blob);
    static const_iterator endIterator(const bcema_Blob *blob);
        // Returns a bidirectional iterator to the end of the specified 'blob'.

    template <typename T> static void appendToBlob(
                                          bcema_Blob                *blob,
                                          const bcema_SharedPtr<T>&  buf,
                                          int                        size = 0);
        // Appends 'buf' to the 'blob'.  If 'size' is 0, sizeof(T) bytes are
        // appended.  Note that the data is not copied.

    static void loadOvec(bsl::vector<btes_Ovec> *vec, const bcema_Blob& blob);
        // Load the 'vec' with the data buffers of the 'blob'.

    template <typename BDEXABLE> static void toBdex(bcema_Blob      *blob,
                                                    const BDEXABLE&  bdexable);
        // Append a bdex-encoding of 'bdexable' to 'blob'.

    template <typename BDEXABLE> static int fromBdex(
                                                   BDEXABLE          *bdexable,
                                                   const bcema_Blob&  blob);
        // Load 'bdexable' by un-bdexing the contents of blob.  Returns
        // the numbers of bytes that were parsed, negative on failure.


    static void toString(bsl::string *string, const bcema_Blob& blob);
        // Load 'string' with the contents of the 'blob'.
};

// ===========================================================================
//                  INLINE AND TEMPLATE FUNCTION IMPLEMENTATIONS
// ===========================================================================

                        // --------------------------
                        // class BlobUtil_IteratorImp
                        // --------------------------

// CREATORS
template <typename T> 
inline
BlobUtil_IteratorImp<T>::BlobUtil_IteratorImp()
: d_blob_p(0)
, d_buffer(0)
, d_offset(0)
{
}

template <typename T>
inline
BlobUtil_IteratorImp<T>::BlobUtil_IteratorImp(const bcema_Blob *blob,
                                              int               buffer,
                                              int               offset)
: d_blob_p(blob)
, d_buffer(buffer)
, d_offset(offset)
{
}

template <typename T>
inline
BlobUtil_IteratorImp<T>::BlobUtil_IteratorImp(const BlobUtil_IteratorImp& rhs)
: d_blob_p(rhs.d_blob_p)
, d_buffer(rhs.d_buffer)
, d_offset(rhs.d_offset)
{
}

template <typename T>
inline
BlobUtil_IteratorImp<T>::~BlobUtil_IteratorImp()
{
}

// MANIPULATORS
template <typename T>
inline
BlobUtil_IteratorImp<T>& BlobUtil_IteratorImp<T>::operator=(
                                               const BlobUtil_IteratorImp& rhs)

{
    d_blob_p = rhs.d_blob_p;
    d_buffer = rhs.d_buffer;
    d_offset = rhs.d_offset;
}

template <typename T>
inline
void BlobUtil_IteratorImp<T>::operator++()
{
    int bufferSize = (d_blob_p->numDataBuffers() - 1 == d_buffer)
                   ? d_blob_p->lastDataBufferLength()
                   : d_blob_p->buffer(d_buffer).size();
 
    if (++d_offset >= bufferSize) {
        ++d_buffer;
        d_offset = 0;
    }
}

template <typename T>
inline
void BlobUtil_IteratorImp<T>::operator--()
{
    if (--d_offset < 0) {
        --d_buffer;
        d_offset = d_blob_p->buffer(d_buffer).size() - 1;
    }
}


// ACCESSORS
template <typename T>
inline
T& BlobUtil_IteratorImp<T>::operator*() const
{
    return *(d_blob_p->buffer(d_buffer).data() + d_offset);
}

template <typename T>
inline
void BlobUtil::appendToBlob(bcema_Blob                *blob,
                            const bcema_SharedPtr<T>&  buf,
                            int                        size)
{
    BSLS_ASSERT(blob);
    bcema_SharedPtr<char> alias(buf, reinterpret_cast<char*>(buf.ptr()));
    bcema_BlobBuffer blobBuffer(alias, (0 == size) ? sizeof(T) : size);
    blob->trimLastDataBuffer(); // Note: this should not be necessary
                                // according to BDE doc, but there's a bug
    blob->appendDataBuffer(blobBuffer);
}

template <typename BDEXABLE>
inline
void BlobUtil::toBdex(bcema_Blob      *blob,
                      const BDEXABLE&  bdexable)
{
    BSLS_ASSERT(blob);
    bdex_ByteOutStream outStream;
    outStream << bdexable;
    bcema_BlobUtil::append(blob, outStream.data(), outStream.length());
}

template <typename BDEXABLE>
inline
int BlobUtil::fromBdex(BDEXABLE          *bdexable,
                       const bcema_Blob&  blob)
{
    BSLS_ASSERT(bdexable);
    bsl::vector<char> buf(blob.length());
    btemt::BlobUtil::copyOut(buf.data(), blob, buf.size());
    bdex_ByteInStream instream(buf.data(), buf.size());
    instream >> *bdexable;
    const void *valid = instream;
    return valid ? instream.cursor() : -1;
}

}   // close package-level namespace

// FREE OPERATORS

template <typename T>
inline
bool btemt::operator==(const BlobUtil_IteratorImp<T>& lhs,
                      const BlobUtil_IteratorImp<T>& rhs)
{
    return lhs.d_blob_p == rhs.d_blob_p
        && lhs.d_buffer == rhs.d_buffer
        && lhs.d_offset == rhs.d_offset;
}



}   // close enterprise-wide namespace

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2010
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
