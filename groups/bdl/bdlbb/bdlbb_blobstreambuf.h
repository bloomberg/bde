// bdlbb_blobstreambuf.h                                              -*-C++-*-
#ifndef INCLUDED_BDLBB_BLOBSTREAMBUF
#define INCLUDED_BDLBB_BLOBSTREAMBUF

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide blob implementing the `streambuf` interface.
//
//@CLASSES:
//  bdlbb::InBlobStreamBuf: `bdlbb::Blob` input `streambuf`
//  bdlbb::OutBlobStreamBuf: `bdlbb::Blob` output `streambuf`
//
//@SEE_ALSO: bdlbb_blob
//
//@DESCRIPTION: This component implements the input and output
// `bsl::basic_streambuf` protocol using a user-supplied `bdlbb::Blob`.  Method
// names necessarily correspond to the protocol-specified method names.  Refer
// to the C++ Standard, Section 27.5.2, for a full specification of the
// interface.
//
// A `bdlbb::Blob` is an indexed sequence of `bdlbb::BlobBuffer` of potentially
// different sizes.  The number of buffers in the sequence can increase or
// decrease, but the order of the buffers cannot change.  Therefore, the blob
// behaves logically as a single indexed buffer.  `bdlbb::InBlobStreamBuf` and
// `bdlbb::OutBlobStreamBuf` can therefore respectively read from and write to
// this buffer as if there were a single continuous index.

#include <bdlscm_version.h>

#include <bdlbb_blob.h>

#include <bsls_assert.h>
#include <bsls_keyword.h>
#include <bsls_review.h>

#include <bsl_ios.h>  // for 'bsl::streamsize'
#include <bsl_streambuf.h>
#include <bsl_cstddef.h>  // bsl::size_t

namespace BloombergLP {
namespace bdlbb { class Blob; }
namespace bdlbb {

                           // =====================
                           // class InBlobStreamBuf
                           // =====================

/// This class implements the input functionality of the `basic_streambuf`
/// protocol, using a client-supplied `bdlbb::Blob`.
class InBlobStreamBuf : public bsl::streambuf {

    // PRIVATE TYPES
    typedef bsl::ios_base ios_base;

    // DATA
    const bdlbb::Blob *d_blob_p;          // "streamed" blob (held)
    int                d_getBufferIndex;  // index of current buffer
    int d_previousBuffersLength;          // length of buffers before the
                                          // current one

    // NOT IMPLEMENTED
    InBlobStreamBuf(const InBlobStreamBuf&);
    InBlobStreamBuf& operator=(const InBlobStreamBuf&);

  private:
    // PRIVATE MANIPULATORS

    /// Set the current location to the specified `position`.
    void setGetPosition(bsl::size_t position);

    // PRIVATE ACCESSORS

    /// Check this object's invariant.
    int checkInvariant() const;

  protected:
    // PROTECTED VIRTUAL FUNCTIONS

    /// Return `traits_type::eof()` unconditionally.
    int_type overflow(int_type c = bsl::streambuf::traits_type::eof())
                                                         BSLS_KEYWORD_OVERRIDE;

    /// Adjust the underlying blob and put the optionally specified
    /// character `c` at the newly valid `gptr()`.  Return `c` (or
    /// `~traits_type::eof` if `c == traits_type::eof`) on success, and
    /// `traits_type::eof()` otherwise.
    int_type pbackfail(int_type c = bsl::streambuf::traits_type::eof())
                                                         BSLS_KEYWORD_OVERRIDE;

    /// Set the location from which the next I/O operation indicated by the
    /// optionally specified `which` mode will occur to the specified
    /// `offset` position from the location indicated by the specified
    /// `fixedPosition`.  Return the new offset on success, and
    /// `off_type(-1)` otherwise.  `offset` may be negative.  Note that this
    /// method will fail if `bsl::ios_base::out` is set.
    pos_type seekoff(
       off_type                offset,
       bsl::ios_base::seekdir  fixedPosition,
       bsl::ios_base::openmode which = bsl::ios_base::in | bsl::ios_base::out)
                                                         BSLS_KEYWORD_OVERRIDE;

    /// Set the location from which the next I/O operation indicated by the
    /// optionally specified `which` mode will occur to the specified
    /// `position`.  Return `position` on success, and `off_type(-1)`
    /// otherwise.  Note that this method will fail if `bsl::ios_base::out`
    /// is set.
    pos_type seekpos(
       pos_type                position,
       bsl::ios_base::openmode which = bsl::ios_base::in | bsl::ios_base::out)
                                                         BSLS_KEYWORD_OVERRIDE;

    /// Return the number of characters currently available for reading from
    /// this stream buffer, or 0 if there are none.
    bsl::streamsize showmanyc() BSLS_KEYWORD_OVERRIDE;

    /// Return 0 unconditionally.
    int sync() BSLS_KEYWORD_OVERRIDE;

    /// Adjust the underlying blob so that the next read position is valid.
    /// Return the character at `gptr()` on success and `traits_type::eof()`
    /// otherwise.
    int_type underflow() BSLS_KEYWORD_OVERRIDE;

    /// Read the specified `numChars` to the specified `destination`.
    /// Return the number of characters successfully read.  The behavior is
    /// undefined unless 0 <= `numChars`.
    bsl::streamsize xsgetn(char_type       *destination,
                           bsl::streamsize  numChars) BSLS_KEYWORD_OVERRIDE;

    /// Return 0 unconditionally.
    bsl::streamsize xsputn(const char_type *source,
                           bsl::streamsize  numChars) BSLS_KEYWORD_OVERRIDE;

  public:
    // CREATORS

    /// Create a `BlobStreamBuf` using the specified `blob`.  The behavior
    /// is undefined unless `blob` remains valid and externally unmodified
    /// for the lifetime of this `streambuf`.
    explicit InBlobStreamBuf(const bdlbb::Blob *blob);

    /// Destroy this stream buffer.
    ~InBlobStreamBuf() BSLS_KEYWORD_OVERRIDE;

    // MANIPULATORS

    /// Reset the get areas.  Optionally set the underlying `bdlbb::Blob`
    /// value to the optionally specified `blob` if `blob` is not 0.  The
    /// behavior is undefined unless `blob` remains valid and externally
    /// unmodified for the lifetime of this `streambuf`.
    void reset(const bdlbb::Blob *blob = 0);

    // ACCESSORS

    /// Return the index of the current buffer.  The behavior is undefined
    /// unless the "streamed" blob has at least one buffer.
    int currentBufferIndex() const;

    /// Return the address of the blob held by this stream buffer.
    const bdlbb::Blob *data() const;

    /// Return the number of bytes contained in the buffers located before
    /// the current one.  The behavior is undefined unless the "streamed"
    /// blob has at least one buffer.
    int previousBuffersLength() const;
};

                           // ======================
                           // class OutBlobStreamBuf
                           // ======================

/// This class implements the output functionality of the `basic_streambuf`
/// protocol, using a client-supplied `bdlbb::Blob`.
class OutBlobStreamBuf : public bsl::streambuf {

    // PRIVATE TYPES
    typedef bsl::ios_base ios_base;

    // DATA
    bdlbb::Blob *d_blob_p;                 // "streamed" blob (held)
    int          d_putBufferIndex;         // index of current buffer
    int          d_previousBuffersLength;  // length of buffers before

    // NOT IMPLEMENTED
    OutBlobStreamBuf(const OutBlobStreamBuf&);
    OutBlobStreamBuf& operator=(const OutBlobStreamBuf&);

  private:
    // PRIVATE MANIPULATORS

    /// Set the current location to the specified `position`.
    void setPutPosition(bsl::size_t position);

    // PRIVATE ACCESSORS

    /// Check this object's invariants and return 0.
    int checkInvariant() const;

  protected:
    // PROTECTED VIRTUAL FUNCTIONS

    /// Append the optionally specified character `c` to this streambuf, and
    /// return `c`.  By default, `traits_type::eof()` is appended.
    int_type overflow(int_type c = bsl::streambuf::traits_type::eof())
                                                         BSLS_KEYWORD_OVERRIDE;

    /// Return `traits_type::eof()` unconditionally.
    int_type pbackfail(int_type c = bsl::streambuf::traits_type::eof())
                                                         BSLS_KEYWORD_OVERRIDE;

    /// Set the location from which the next I/O operation indicated by the
    /// optionally specified `which` mode will occur to the specified
    /// `offset` position from the location indicated by the specified
    /// `fixedPosition`.  Return the new offset on success, and
    /// `off_type(-1)` otherwise.  `offset` may be negative.  Note that this
    /// method will fail if `bsl::ios_base::in` is set.
    pos_type seekoff(
       off_type                offset,
       bsl::ios_base::seekdir  fixedPosition,
       bsl::ios_base::openmode which = bsl::ios_base::in | bsl::ios_base::out)
                                                         BSLS_KEYWORD_OVERRIDE;

    /// Set the location from which the next I/O operation indicated by the
    /// optionally specified `which` mode will occur to the specified
    /// `position`.  Return `position` on success, and `off_type(-1)`
    /// otherwise.  Note that this method will fail if `bsl::ios_base::in`
    /// is set.
    pos_type seekpos(
       pos_type                position,
       bsl::ios_base::openmode which = bsl::ios_base::in | bsl::ios_base::out)
                                                         BSLS_KEYWORD_OVERRIDE;

    /// Return 0 unconditionally.
    bsl::streamsize showmanyc() BSLS_KEYWORD_OVERRIDE;

    /// Synchronize the put position in the blob of this stream buffer.
    /// Return 0 unconditionally.
    int sync() BSLS_KEYWORD_OVERRIDE;

    /// Return `traits_type::eof()` unconditionally.
    int_type underflow() BSLS_KEYWORD_OVERRIDE;

    /// Return 0 unconditionally.
    bsl::streamsize xsgetn(char_type       *destination,
                           bsl::streamsize  numChars) BSLS_KEYWORD_OVERRIDE;

    /// Copy the specified `numChars` from the specified `source` to the
    /// blob held by this streambuf, starting at the current put area
    /// location.  The behavior is undefined unless 0 <= `numChars`.
    bsl::streamsize xsputn(const char_type *source,
                           bsl::streamsize  numChars) BSLS_KEYWORD_OVERRIDE;

  public:
    // CREATORS

    /// Create a `OutBlobStreamBuf` using the specified `blob`, and set the
    /// location at which the next write operation will occur to
    /// `blob->length()`.  The behavior is undefined unless `blob` remains
    /// valid and externally unmodified for the lifetime of this
    /// `streambuf`.
    explicit OutBlobStreamBuf(bdlbb::Blob *blob);

    /// Destroy this stream buffer.
    ~OutBlobStreamBuf() BSLS_KEYWORD_OVERRIDE;

    // MANIPULATORS

    /// Return the address of the blob held by this stream buffer.
    bdlbb::Blob *data();

    /// Reset the put position of this buffer to the first location,
    /// available for writing in the underlying `bdlbb::Blob`. Optionally
    /// specify a `blob` used to change current underlying `bdlbb::Blob`
    /// value for.  The behavior is undefined unless `blob` remains valid
    /// and externally unmodified for the lifetime of this `streambuf`.
    void reset(bdlbb::Blob *blob = 0);

    // ACCESSORS

    /// Return the index of the current buffer.  The behavior is undefined
    /// unless the "streamed" blob has at least one buffer.
    int currentBufferIndex() const;

    /// Return the address of the blob held by this stream buffer.
    const bdlbb::Blob *data() const;

    /// Return the number of bytes contained in the buffers located before
    /// the current one.  The behavior is undefined unless the "streamed"
    /// blob has at least one buffer.
    int previousBuffersLength() const;
};

// ============================================================================
//                             INLINE DEFINITIONS
// ============================================================================

                           // =====================
                           // class InBlobStreamBuf
                           // =====================

// MANIPULATORS
inline
void InBlobStreamBuf::reset(const bdlbb::Blob *blob)
{
    if (blob) {
        d_blob_p                = blob;
        d_getBufferIndex        = 0;
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
    BSLS_ASSERT(d_getBufferIndex < d_blob_p->numBuffers());
    return d_getBufferIndex;
}

inline
const bdlbb::Blob *InBlobStreamBuf::data() const
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
bdlbb::Blob *OutBlobStreamBuf::data()
{
    return d_blob_p;
}

inline
void OutBlobStreamBuf::reset(bdlbb::Blob *blob)
{
    if (blob) {
        d_blob_p                = blob;
        d_putBufferIndex        = 0;
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
    BSLS_ASSERT(d_putBufferIndex < d_blob_p->numBuffers());
    return d_putBufferIndex;
}

inline
const bdlbb::Blob *OutBlobStreamBuf::data() const
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
