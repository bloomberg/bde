// bdlsb_fixedmemoutstreambuf.h                                       -*-C++-*-
#ifndef INCLUDED_BDLSB_FIXEDMEMOUTSTREAMBUF
#define INCLUDED_BDLSB_FIXEDMEMOUTSTREAMBUF

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide an output 'basic_streambuf' using a client buffer.
//
//@CLASSES:
//   bdlsb::FixedMemOutStreamBuf: output stream buffer using client memory
//
//@SEE_ALSO: bdlsb_memoutstreambuf, bdlsb_fixedmeminstreambuf
//
//@DESCRIPTION: This component defines a class 'bdlsb::FixedMemOutStreamBuf'
// that implements the output portion of the 'bsl::basic_streambuf' protocol
// using a client-supplied memory buffer.  Method names necessarily correspond
// to the protocol-specified method names.  Clients supply the character buffer
// at stream buffer construction, and can later reinitialize the stream buffer
// with a different character buffer by calling the 'pubsetbuf' method.
//
// This component provides none of the input-related functionality of
// 'basic_streambuf' (see Streaming Architecture, below), nor does it use
// locales in any way.
//
///Streaming Architecture
///----------------------
// Stream buffers are designed to decouple device handling from content
// formatting, providing the requisite device handling and possible buffering
// services, and leaving the formatting to the client stream.  The standard
// C++ IOStreams library further partitions streaming into input streaming and
// output streaming, separating responsibilities for each at both the stream
// layer and the stream buffer layer.  The BDE streaming library for 'bdex',
// including all of 'bdesb', follows this model.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Directly Observing Stream Buffer Contents
/// - - - - - - - - - - - - - - - - - - - - - - - - - -
// Unlike most implementations of the 'bsl::basic_streambuf' concept,
// 'bdlsb::FixedMemOutStreamBuf' gives the user direct access to the stream's
// storage, both through the 'data' accessor and through the buffer originally
// supplied to the constructor.  Note that this can be useful in many contexts,
// such as when we need to perform extra security validation on buffer during
// the streaming process.
//
// First, we create an array to provide storage for the stream buffer, and
// construct a 'bdlsb::FixedMemOutStreamBuf' on that array:
//..
//  const unsigned int          STORAGE_SIZE = 64;
//  char                        storage[STORAGE_SIZE];
//  bdlsb::FixedMemOutStreamBuf buffer(storage, STORAGE_SIZE);
//..
// Notice that 'storage' is on the stack.  'bdlsb::FixedMemOutStreamBuf' can be
// easily used without resorting to dynamic memory allocation.
//
// Then, we observe that 'buffer' already has a capacity of 64.  Note that this
// capacity is fixed at construction:
//..
//  assert(STORAGE_SIZE == buffer.capacity());
//  assert( 0 == buffer.length());
//  assert(buffer.data() == storage);
//..
// Next, we use 'buffer' to construct a 'bsl::ostream':
//..
//  bsl::ostream stream(&buffer);
//..
// Now, we output some data to the 'stream':
//..
//  stream << "The answer is " << 42 << ".";
//..
// Finally, we observe that the data is present in the storage array that we
// supplied to 'buffer':
//..
//  assert(17 == buffer.length());
//  assert(buffer.length() < STORAGE_SIZE);
//  assert(0 == strncmp("The answer is 42.", storage, 17));
//..
//
///Example 2: Fixed Buffer Size
/// - - - - - - - - - - - - - -
// Unlike most implementations of the 'bsl::basic_streambuf' concept,
// 'bdlsb::FixedMemOutStreamBuf' uses a buffer of limited size, provided to the
// constructor together with the address of the storage buffer.  That limit
// will not be exceeded even in case of superfluous data.  Symbols beyond this
// limit will be ignored.  Note that this can be useful if memory allocation
// should be strictly controlled.
//
// First, we create an array to provide storage for the stream buffer, fill it
// with some data and construct a 'bdlsb::FixedMemOutStreamBuf' on the part of
// that array:
//..
//  const unsigned int SMALL_STORAGE_SIZE = 16;
//  const unsigned int SMALL_BUFFER_CAPACITY = SMALL_STORAGE_SIZE/2;
//  char               smallStorage[SMALL_STORAGE_SIZE];
//  memset(smallStorage, 'Z', SMALL_STORAGE_SIZE);
//
//  bdlsb::FixedMemOutStreamBuf smallBuffer(smallStorage,
//                                          SMALL_BUFFER_CAPACITY);
//..
// Next, we write some characters to the buffer and check that it handles them
// correctly and superfluous data is ignored:
//..
//  bsl::streamsize returnedSize = smallBuffer.sputn("The answer is 42.", 17);
//  assert(SMALL_BUFFER_CAPACITY == returnedSize);
//  assert(SMALL_BUFFER_CAPACITY == smallBuffer.length());
//  assert('Z' == smallStorage[smallBuffer.length()]);
//..
// Then, we reset position indicator to the beginning of storage:
//..
//  smallBuffer.pubseekpos(0,bsl::ios_base::out);
//  assert(0 == smallBuffer.length());
//..
// Now, we write another string, containing fewer characters than the storage
// capacity:
//..
//  returnedSize = smallBuffer.sputn("Truth.", 6);
//..
// Finally, we observe that given string has been successfully placed to
// buffer:
//..
//  assert(6 == returnedSize);
//  assert(6 == smallBuffer.length());
//  assert(0 == strncmp("Truth.", smallStorage, 6));
//..

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORM
#include <bsls_platform.h>
#endif

#ifndef INCLUDED_BSL_CSTDLIB
#include <bsl_cstdlib.h>
#endif

#ifndef INCLUDED_BSL_CSTRING
#include <bsl_cstring.h>
#endif

#ifndef INCLUDED_BSL_IOS
#include <bsl_ios.h>
#endif

#ifndef INCLUDED_BSL_STREAMBUF
#include <bsl_streambuf.h>
#endif

#if defined(BSLS_PLATFORM_CMP_MSVC) && defined(min)
    // Note: on Windows -> WinDef.h:#define min(a,b) ...
#undef min
#endif

namespace BloombergLP {
namespace bdlsb {

                       // ==========================
                       // class FixedMemOutStreamBuf
                       // ==========================

class FixedMemOutStreamBuf : public bsl::streambuf {
    // This class implements the output functionality of the 'basic_streambuf'
    // protocol for client-supplied memory.

  private:
    // NOT IMPLEMENTED
    FixedMemOutStreamBuf(const FixedMemOutStreamBuf&);
    FixedMemOutStreamBuf& operator=(const FixedMemOutStreamBuf&);

  protected:
    // PROTECTED MANIPULATORS
    virtual pos_type seekoff(
                           off_type                offset,
                           bsl::ios_base::seekdir  fixedPosition,
                           bsl::ios_base::openmode which = bsl::ios_base::out);
        // Set the position indicator to the relative specified 'offset' from
        // the base position indicated by the specified 'fixedPosition' and
        // return the resulting absolute position on success or pos_type(-1)
        // on failure.  Optionally specify 'which' area of the stream buffer.
        // The seek operation will fail if 'which' does not include the flag
        // 'bsl::ios_base::out' or if the resulting absolute position is less
        // than zero or greater than the value returned by 'length'.

    virtual pos_type seekpos(
                           pos_type                position,
                           bsl::ios_base::openmode which = bsl::ios_base::out);
        // Set the position indicator to the specified 'position' and return
        // the resulting absolute position on success or pos_type(-1) on
        // failure.  Optionally specify 'which' area of the stream buffer.  The
        // 'seekpos' operation will fail if 'which' does not include the flag
        // 'bsl::ios_base::out' or if position is less then zero or greater
        // than the value returned by 'length'.

    virtual FixedMemOutStreamBuf *setbuf(char_type       *buffer,
                                         bsl::streamsize  length);
        // Reinitialize this stream buffer to use the specified character
        // 'buffer' having the specified 'length'.  Return the address of this
        // modifiable stream buffer.  The behavior is undefined unless
        // 'length == 0' or 'length > 0 && buffer != 0'.  Upon
        // re-initialization for use of the new buffer, the length and next
        // output location are reset to zero.  Note that 'buffer' is held but
        // not owned.

  public:
    // CREATORS
    FixedMemOutStreamBuf(char *buffer, bsl::streamsize length);
        // Create an empty stream buffer that uses the specified character
        // 'buffer' of the specified 'length'.  The behavior is undefined
        // unless 'length == 0' or 'length > 0 && buffer != 0'.
        // Note that 'buffer' is held but not owned.

    ~FixedMemOutStreamBuf();
        // Destroy this stream buffer.

    // MANIPULATORS
    char *data();
        // Return a pointer providing modifiable access to the character buffer
        // held by this stream buffer (supplied at construction).

    // ACCESSORS
    bsl::streamsize capacity() const;
        // Return the number of characters in the buffer held by this stream
        // buffer.  See 'length', below, for the span of bytes actually
        // written.

    const char *data() const;
        // Return a pointer providing non-modifiable access to the character
        // buffer held by this stream buffer (supplied at construction).

    bsl::streamsize length() const;
        // Return the number of characters from the beginning of the buffer to
        // the current write position.  This function returns the same value
        // as 'seekoff(0, bsl::ios_base::end)'.  The length is modified by a
        // call to 'seekpos' or 'seekoff' and reset to zero by a call to
        // 'pubsetbuf'.
};

// ============================================================================
//                              INLINE DEFINITIONS
// ============================================================================

                       // --------------------------
                       // class FixedMemOutStreamBuf
                       // --------------------------

// PROTECTED MANIPULATORS
inline
FixedMemOutStreamBuf *
FixedMemOutStreamBuf::setbuf(char_type *buffer, bsl::streamsize length)
{
    BSLS_ASSERT_SAFE(buffer || 0 == length);
    BSLS_ASSERT_SAFE(0 <= length);

    // Reset pointers and length.
    setp(buffer, buffer + length);
    return this;
}

// CREATORS
inline
FixedMemOutStreamBuf::FixedMemOutStreamBuf(char            *buffer,
                                           bsl::streamsize  length)
{
    BSLS_ASSERT_SAFE(buffer || 0 == length);
    BSLS_ASSERT_SAFE(0 <= length);

    setp(buffer, buffer + length);
}

inline
FixedMemOutStreamBuf::~FixedMemOutStreamBuf()
{
}

// MANIPULATORS
inline
char *FixedMemOutStreamBuf::data()
{
    return pbase();
}

// ACCESSORS
inline
bsl::streamsize FixedMemOutStreamBuf::capacity() const
{
    return epptr() - pbase();
}

inline
const char *FixedMemOutStreamBuf::data() const
{
    return pbase();
}

inline
bsl::streamsize FixedMemOutStreamBuf::length() const
{
    return pptr() - pbase();
}

}  // close package namespace
}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2015 Bloomberg Finance L.P.
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
