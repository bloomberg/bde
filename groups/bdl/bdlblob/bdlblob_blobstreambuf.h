// bdlblob_blobstreambuf.h                                            -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BDLBLOB_BLOBSTREAMBUF
#define INCLUDED_BDLBLOB_BLOBSTREAMBUF

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide blob implementing the 'streambuf' interface.
//
//@CLASSES:
// bdlblob::InBlobStreamBuf: 'bdlblob::Blob' input 'streambuf'
// bdlblob::OutBlobStreamBuf: 'bdlblob::Blob' output 'streambuf'
//
//@AUTHOR: Guillaume Morin (gmorin1)
//
//@SEE_ALSO: bdlblob_blob
//
//@DESCRIPTION: This component implements the input and output
// 'bsl::basic_streambuf' protocol using a user-supplied 'bdlblob::Blob'.
// Method names necessarily correspond to the protocol-specified method names.
// Refer to the C++ Standard, Section 27.5.2, for a full specification of the
// interface.
//
// A 'bdlblob::Blob' is an indexed sequence of 'bdlblob::BlobBuffer' of
// potentially different sizes.  The number of buffers in the sequence can
// increase or decrease, but the order of the buffers cannot change.
// Therefore, the blob behaves logically as a single indexed buffer.
// 'bdlblob::InBlobStreamBuf' and 'bdlblob::OutBlobStreamBuf' can therefore
// respectively read from and write to this buffer as if there were a single
// continuous index.

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

#ifndef INCLUDED_BDLBLOB_BLOB
#include <bdlblob_blob.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSL_IOS
#include <bsl_ios.h>  // for 'bsl::streamsize'
#endif

#ifndef INCLUDED_BSL_STREAMBUF
#include <bsl_streambuf.h>
#endif

#ifndef INCLUDED_BSL_CSTDDEF
#include <bsl_cstddef.h>  // bsl::size_t
#endif

namespace BloombergLP {
namespace bdlblob { class Blob; }
namespace bdlblob {

                          // =====================
                          // class InBlobStreamBuf
                          // =====================

class InBlobStreamBuf : public bsl::streambuf {
    // This class implements the input functionality of the 'basic_streambuf'
    // protocol, using a client-supplied 'bdlblob::Blob'.

    // PRIVATE TYPES
    typedef bsl::ios_base ios_base;

    // DATA
    const bdlblob::Blob *d_blob_p;                 // "streamed" blob (held)
    int               d_getBufferIndex;         // index of current buffer
    int               d_previousBuffersLength;  // length of buffers before the
                                                // current one

    // NOT IMPLEMENTED
    InBlobStreamBuf(const InBlobStreamBuf&);
    InBlobStreamBuf& operator=(const InBlobStreamBuf&);

  private:
    // PRIVATE MANIPULATORS
    void setGetPosition(bsl::size_t position);
        // Set the current location to the specified 'position'.

    // PRIVATE ACCESSORS
    int checkInvariant() const;
        // Check this object's invariant.

  protected:
    // PROTECTED VIRTUAL FUNCTIONS
    virtual int_type overflow(int_type c = bsl::streambuf::traits_type::eof());
        // Return 'traits_type::eof()' unconditionally.

    virtual int_type pbackfail(
                              int_type c = bsl::streambuf::traits_type::eof());
        // Adjust the underlying blob and put the optionally specified
        // character 'c' at the newly valid 'gptr()'.  Return 'c' (or
        // '~traits_type::eof' if 'c
        // == traits_type::eof') on success, and 'traits_type::eof()'
        // otherwise.

    virtual pos_type seekoff(
       off_type                offset,
       bsl::ios_base::seekdir  fixedPosition,
       bsl::ios_base::openmode which = bsl::ios_base::in | bsl::ios_base::out);
        // Set the location from which the next I/O operation indicated by the
        // optionally specified 'which' mode will occur to the specified
        // 'offset' position from the location indicated by the specified
        // 'fixedPosition'.  Return the new offset on success, and
        // 'off_type(-1)' otherwise.  'offset' may be negative.  Note that this
        // method will fail if 'bsl::ios_base::out' is set.

    virtual pos_type seekpos(
       pos_type                position,
       bsl::ios_base::openmode which = bsl::ios_base::in | bsl::ios_base::out);
        // Set the location from which the next I/O operation indicated by the
        // optionally specified 'which' mode will occur to the specified
        // 'position'.  Return 'position' on success, and 'off_type(-1)'
        // otherwise.  Note that this method will fail if 'bsl::ios_base::out'
        // is set.

    virtual bsl::streamsize showmanyc();
        // Return the number of characters currently available for reading from
        // this stream buffer, or 0 if there are none.

    virtual int sync();
        // Return 0 unconditionally.

    virtual int_type underflow();
        // Adjust the underlying blob so that the next read position is valid.
        // Return the character at 'gptr()' on success and 'traits_type::eof()'
        // otherwise.

    virtual bsl::streamsize xsgetn(char_type       *destination,
                                   bsl::streamsize  numChars);
        // Read the specified 'numChars' to the specified 'destination'.
        // Return the number of characters successfully read.  The behavior is
        // undefined unless 0 <= 'numChars'.

    virtual bsl::streamsize xsputn(const char_type *source,
                                   bsl::streamsize  numChars);
        // Return 0 unconditionally.

  public:
    // CREATORS
    explicit InBlobStreamBuf(const bdlblob::Blob *blob);
        // Create a 'BlobStreamBuf' using the specified 'blob'.  The behavior
        // is undefined unless 'blob' remains valid and externally unmodified
        // for the lifetime of this 'streambuf'.

    ~InBlobStreamBuf();
        // Destroy this stream buffer.

    // MANIPULATORS
    void reset(const bdlblob::Blob *blob = 0);
        // Reset the get areas.  Optionally set the underlying 'bdlblob::Blob'
        // value to the optionally specified 'blob' if 'blob' is not 0.  The
        // behavior is undefined unless 'blob' remains valid and externally
        // unmodified for the lifetime of this 'streambuf'.

    // ACCESSORS
    int currentBufferIndex() const;
        // Return the index of the current buffer.  The behavior is undefined
        // unless the "streamed" blob has at least one buffer.

    const bdlblob::Blob *data() const;
        // Return the address of the blob held by this stream buffer.

    int previousBuffersLength() const;
        // Return the number of bytes contained in the buffers located before
        // the current one.  The behavior is undefined unless the "streamed"
        // blob has at least one buffer.
};

                          // ======================
                          // class OutBlobStreamBuf
                          // ======================

class OutBlobStreamBuf : public bsl::streambuf {
    // This class implements the output functionality of the 'basic_streambuf'
    // protocol, using a client-supplied 'bdlblob::Blob'.

    // PRIVATE TYPES
    typedef bsl::ios_base ios_base;

    // DATA
    bdlblob::Blob *d_blob_p;                 // "streamed" blob (held)
    int         d_putBufferIndex;         // index of current buffer
    int         d_previousBuffersLength;  // length of buffers before

    // NOT IMPLEMENTED
    OutBlobStreamBuf(const OutBlobStreamBuf&);
    OutBlobStreamBuf& operator=(const OutBlobStreamBuf&);

  private:
    // PRIVATE MANIPULATORS
    void setPutPosition(bsl::size_t position);
        // Set the current location to the specified 'position'.

    // PRIVATE ACCESSORS
    int checkInvariant() const;
        // Check this object's invariants and return 0.

  protected:
    // PROTECTED VIRTUAL FUNCTIONS
    virtual int_type overflow(int_type c = bsl::streambuf::traits_type::eof());
        // Append the optionally specified character 'c' to this streambuf, and
        // return 'c'.  By default, 'traits_type::eof()' is appended.

    virtual int_type pbackfail(
                              int_type c = bsl::streambuf::traits_type::eof());
        // Return 'traits_type::eof()' unconditionally.

    virtual pos_type seekoff(
       off_type                offset,
       bsl::ios_base::seekdir  fixedPosition,
       bsl::ios_base::openmode which = bsl::ios_base::in | bsl::ios_base::out);
        // Set the location from which the next I/O operation indicated by the
        // optionally specified 'which' mode will occur to the specified
        // 'offset' position from the location indicated by the specified
        // 'fixedPosition'.  Return the new offset on success, and
        // 'off_type(-1)' otherwise.  'offset' may be negative.  Note that this
        // method will fail if 'bsl::ios_base::in' is set.

    virtual pos_type seekpos(
       pos_type                position,
       bsl::ios_base::openmode which = bsl::ios_base::in | bsl::ios_base::out);
        // Set the location from which the next I/O operation indicated by the
        // optionally specified 'which' mode will occur to the specified
        // 'position'.  Return 'position' on success, and 'off_type(-1)'
        // otherwise.  Note that this method will fail if 'bsl::ios_base::in'
        // is set.

    virtual bsl::streamsize showmanyc();
        // Return 0 unconditionally.

    virtual int sync();
        // Synchronize the put position in the blob of this stream buffer.
        // Return 0 unconditionally.

    virtual int_type underflow();
        // Return 'traits_type::eof()' unconditionally.

    virtual bsl::streamsize xsgetn(char_type       *destination,
                                   bsl::streamsize  numChars);
        // Return 0 unconditionally.

    virtual bsl::streamsize xsputn(const char_type *source,
                                   bsl::streamsize  numChars);
        // Copy the specified 'numChars' from the specified 'source' to the
        // blob held by this streambuf, starting at the current put area
        // location.  The behavior is undefined unless 0 <= 'numChars'.

  public:
    // CREATORS
    explicit OutBlobStreamBuf(bdlblob::Blob *blob);
        // Create a 'OutBlobStreamBuf' using the specified 'blob', and set the
        // location at which the next write operation will occur to
        // 'blob->length()'.  The behavior is undefined unless 'blob' remains
        // valid and externally unmodified for the lifetime of this
        // 'streambuf'.

    ~OutBlobStreamBuf();
        // Destroy this stream buffer.

    // MANIPULATORS
    bdlblob::Blob *data();
        // Return the address of the blob held by this stream buffer.

    void reset(bdlblob::Blob *blob = 0);
        // Reset the put position of this buffer to the first location,
        // available for writing in the underlying 'bdlblob::Blob'. Optionally
        // specify a 'blob' used to change current underlying 'bdlblob::Blob'
        // value for.  The behavior is undefined unless 'blob' remains valid
        // and externally unmodified for the lifetime of this 'streambuf'.

    // ACCESSORS
    int currentBufferIndex() const;
        // Return the index of the current buffer.  The behavior is undefined
        // unless the "streamed" blob has at least one buffer.

    const bdlblob::Blob *data() const;
        // Return the address of the blob held by this stream buffer.

    int previousBuffersLength() const;
        // Return the number of bytes contained in the buffers located before
        // the current one.  The behavior is undefined unless the "streamed"
        // blob has at least one buffer.
};

// ============================================================================
//                             INLINE DEFINITIONS
// ============================================================================

                          // =====================
                          // class InBlobStreamBuf
                          // =====================

// MANIPULATORS
inline
void InBlobStreamBuf::reset(const bdlblob::Blob *blob)
{
    if (blob) {
        d_blob_p = blob;
        d_getBufferIndex = 0;
        d_previousBuffersLength = 0;
        setg(0, 0, 0);
        if (0 == d_blob_p->length()) {
            return;                                                   // RETURN
        }
    }
    setGetPosition(0);
}

// ACCESSORS
inline
int InBlobStreamBuf::currentBufferIndex() const
{
    BSLS_ASSERT_SAFE(d_getBufferIndex < d_blob_p->numBuffers());
    return d_getBufferIndex;
}

inline
const bdlblob::Blob *InBlobStreamBuf::data() const
{
    return d_blob_p;
}

inline
int InBlobStreamBuf::previousBuffersLength() const
{
    return d_previousBuffersLength;
}

                          // ======================
                          // class OutBlobStreamBuf
                          // ======================

// MANIPULATORS
inline
bdlblob::Blob *OutBlobStreamBuf::data()
{
    return d_blob_p;
}

inline
void OutBlobStreamBuf::reset(bdlblob::Blob *blob)
{
    if (blob) {
        d_blob_p = blob;
        d_putBufferIndex = 0;
        d_previousBuffersLength = 0;
        setp(0, 0);
        if (0 == d_blob_p->totalSize()) {
            return;                                                   // RETURN
        }
    }
    setPutPosition(d_blob_p->length());
}

// ACCESSORS
inline
int OutBlobStreamBuf::currentBufferIndex() const
{
    BSLS_ASSERT_SAFE(d_putBufferIndex < d_blob_p->numBuffers());
    return d_putBufferIndex;
}

inline
const bdlblob::Blob *OutBlobStreamBuf::data() const
{
    return d_blob_p;
}

inline
int OutBlobStreamBuf::previousBuffersLength() const
{
    return d_previousBuffersLength;
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
