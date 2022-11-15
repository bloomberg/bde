// bdlde_utf8checkinginstreambufwrapper.h                             -*-C++-*-

#ifndef INCLUDED_BDLDE_UTF8CHECKINGINSTREAMBUFWRAPPER
#define INCLUDED_BDLDE_UTF8CHECKINGINSTREAMBUFWRAPPER

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a stream buffer wrapper for validating UTF-8 input.
//
//@CLASSES:
//   bdlde::Utf8CheckingInStreamBufWrapper: wraps input streambuf, checks UTF-8
//
//@SEE_ALSO: bsl_streambuf
//
//@DESCRIPTION: This component provides a mechanism,
// 'bdlde::Utf8CheckingInStreamBufWrapper', that inherits from
// 'bsl::streambuf', and that holds and wraps another 'streambuf'.  It forwards
// input through the held streambuf, checking for invalid UTF-8.  The wrapping
// object does not support output, only input.  All normal input functions are
// supported.  If the held 'streambuf' supports seeking, seeks are supported,
// though not forward seeks, and 'pubseekoff(0, bsl::ios_base::cur)' is
// supported whether the wrapped 'streambuf' supports seeking or not.
//
// Input is buffered, the buffer cannot be changed -- 'pubsetbuf' is a no-op.
//
// The client is normally recommended to use this object by reading from it
// until it behaves as though it has reached the end of input, and then call
// 'errorStatus' to see if a UTF-8 error happened, and if so, then call
// 'pubseekoff(0, bsl::ios_base::cur)' to find the position of the beginning of
// the invalid UTF-8 code point.
//
///Positioning at the Start
///------------------------
// When starting to read, the wrapped 'streambuf' must be positioned at the
// beginning of a UTF-8 code point, or the end of data, otherwise, the wrapper
// will interpret the first byte read as incorrect UTF-8.
//
///Behavior of Reads
///-----------------
// If incorrect UTF-8 exists in the data stream, reads will succeed until
// reaching the start of the incorrect code point, after which reads will
// behave as though the end of data were reached.  All data returned by reads
// will be valid UTF-8.  Reads of limited length that end before the end of
// data may return incomplete, truncated portions of valid UTF-8 code points.
// In that case, following reads will return the remainder of the same valid
// UTF-8 code point.
//
///'errorStatus'
///-------------
// The 'errorStatus' accessor is not a virtual function and is not inherited
// from 'streambuf'.
//
// If invalid UTF-8 is encountered while reading, input will succeed right up
// to the beginning of the invalid code point, at which point the object will
// behave as though it has reached the end of data, with the object positioned
// to exactly the start of the invalid code point.  'errorStatus' will reflect
// the nature of the UTF-8 error.
//
// If a seek error occurs, 'errorStatus' will change to 'k_SEEK_FAIL' and
// subsequent reads and relative seeks will fail, including
// 'pubseekoff(0, bsl::ios_base::cur)'.  A 'reset' or an absolute seek to the
// start of data will reset 'errorStatus' to 0 and the object will recover to
// being able to perform input and relative seeks.
//
// UTF-8 errors can be recovered from by calling 'reset' or by seeking at least
// one byte backward.  Note that 'pubseekoff(0, bsl::ios_base::cur)' after a
// UTF-8 error will return the object's position without changing the error
// state.  Note that an absolute seek to the beginning of data will not recover
// unless it amounts to a seek at least one byte backward.
//
// If input has reached invalid UTf-8, 'errorStatus()' will be negative, and
// one of the values from 'bdlde::Utf8Util::ErrorStatus'.
//
// The class method 'toAscii' can be called to translate any value returned by
// 'errorStatus()' to a human-readable string.
//
///Seeking
///-------
// The wrapped 'streambuf' must either support seeking or always return a
// negative value when a seek attempt is made.
//
// Forward seeks and seeks relative to the end of data are not supported.
//
// If the wrapped 'streambuf' does not support seeking,
// 'pubseekoff(0, bsl::ios_base::cur)' will still work on the wrapper and will
// return the offset relative to the input position when the wrapper was bound
// to the held 'streambuf', without changing the error state.
//
// Seeks can fail for a number of reasons (see 'seekoff'), and if that happens,
// the object will enter a "failed seek state", having no valid position, and
// will no longer be able to do input or do relative seeks until recovering by
// either doing an absolute seek to 0 or by having 'reset' called.  When the
// object is in a failed seek state, 'errorStatus()' will equal 'k_SEEK_FAIL'.
//
///Valid State
///-----------
// If the object has been bound via 'reset' to a held 'streambuf' and is not in
// a failed seek state, the object is in a valid state.
//
///Usage
///-----
//
///Example 1: Detecting invalid UTF-8 read from a 'streambuf':
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose one has a 'streambuf', 'myStreamBuf' containing UTF-8 that one wants
// to read, checking that it is valid UTF-8.
//
// First, create a 'Utf8CheckingInStreamBufWrapper' that will wrap
// 'myStreamBuf':
//..
//  typedef bdlde::Utf8CheckingInStreamBufWrapper Obj;
//  Obj wrapper;
//  wrapper.reset(&myStreamBuf);
//..
// Then, read the data from the 'wrapper' 'streambuf' until it stops yielding
// data.
//..
//  std::string s;
//  bsl::streamsize len = 0, bytesRead;
//  do {
//      enum { k_READ_CHUNK = 10 };
//
//      s.resize(len + k_READ_CHUNK);
//
//      bytesRead = wrapper.sgetn(&s[len], k_READ_CHUNK);
//
//      assert(0 <= bytesRead);
//      assert(bytesRead <= k_READ_CHUNK);
//
//      s.resize((len += bytesRead));
//  } while (0 < bytesRead);
//
//  assert(wrapper.pubseekoff(0, bsl::ios_base::cur) == Obj::pos_type(len));
//..
// Next, use the 'errorStatus' accessor and 'pubseekoff' manipulator to see
// what, if anything, went wrong and where.
//..
//  const int es = wrapper.errorStatus();
//
//  if      (0 == es) {
//      cout << "No errors occurred.\n";
//  }
//  else if (es < 0) {
//      cout << "Incorrect UTF-8 encountered " << Obj::toAscii(es) <<
//          " at offset " << wrapper.pubseekoff(0, bsl::ios_base::cur) << endl;
//  }
//  else {
//      cout << "Non-UTF-8 error " << Obj::toAscii(es) << endl;
//  }
//..
// Now, we observe the output:
//..
//  Incorrect UTF-8 encountered UNEXPECTED_CONTINUATION_OCTET at offset 79
//..
// Finally, we observe that all the data from 'myStreamBuf' up to offset 79
// was read into 's', and that it's all correct UTF-8.
//..
//  assert(len == s.end() - s.begin());
//  assert(bdlde::Utf8Util::isValid(&s[0], len));
//..

#include <bdlscm_version.h>

#include <bslma_allocator.h>
#include <bslma_usesbslmaallocator.h>
#include <bslmf_nestedtraitdeclaration.h>
#include <bsls_keyword.h>
#include <bsls_types.h>

#include <bsl_ios.h>        // 'streamsize'
#include <bsl_locale.h>
#include <bsl_streambuf.h>  // 'char_type', 'int_type', 'pos_type', 'off_type',
                            // 'traits_type' are within the 'bsl::streambuf'
                            // class

namespace BloombergLP {
namespace bdlde {

                     // ====================================
                     // class Utf8CheckingInStreamBufWrapper
                     // ====================================

class Utf8CheckingInStreamBufWrapper : public bsl::streambuf {
    // This 'class' inherits from 'bsl::streambuf', and holds and wraps another
    // 'streambuf'.  It forwards input through the held streambuf, and checks
    // for invalid UTF-8.  The wrapping object does not support ouput, only
    // input.  If the held 'streambuf' supports seeking, seeks are supported,
    // though not forward seeks, and 'pubseekoff(0, bsl::ios_base::cur)' is
    // supported whether the wrapped 'streambuf' supports seeking or not.

    // PRIVATE TYPES
    typedef bsls::Types::IntPtr IntPtr;   // A signed integral type the size of
                                          // a pointer

    enum {
        k_PBACK_BUF_SIZE = 8,             // size of putback buffer
        k_BUF_SIZE       = 8 * 1024       // input buffer size
    };

  public:
    // PUBLIC TYPES
    enum {
        k_SEEK_FAIL = +1                  // seek failure
    };

  private:
    // DATA
    bsl::streambuf   *d_heldStreamBuf_p;  // the 'streambuf' that this object
                                          // wraps around, which is held, not
                                          // owned.

    int               d_errorStatus;      // The error status of this object.
                                          //: o A value from
                                          //:   'Utf8Util::ErrorStatus' if a
                                          //:   UTF-8 error has occurred.  Note
                                          //:   that these are all -ve values.
                                          //:
                                          //: o 'k_SEEK_FAIL' (positive)
                                          //:   if a seek error has occurred
                                          //:
                                          //: o 0 if no error has occured,
                                          //:   including if end of file has
                                          //:   been reached

    int               d_bufEndStatus;     // status at the end of the buffer,
                                          // which may not have been reached
                                          // yet

    char_type        *d_buf_p;            // input buffer

    char              d_pBackBuf[k_PBACK_BUF_SIZE];
                                          // for putback mode (see above)

    char_type        *d_savedEback_p;     // only used in putback-mode, the
                                          // saved value of non-putback mode
                                          // 'eback' from the base class (note
                                          // that when we enter putback-mode,
                                          // 'eback() == gptr()', so it's not
                                          // necessary to have a
                                          // 'd_savedGptr_p')

    char_type        *d_savedEgptr_p;     // only used in putback-mode, the
                                          // saved value of non-putback mode
                                          // 'egptr' from the base class

    pos_type          d_offset;           // in non-putback mode, the offset of
                                          // 'eback()', in putback mode, the
                                          // offset of 'egptr()'

    bool              d_seekable;         // 'true' if held 'streambuf' is
                                          // seekable and 'false' otherwise

    bool              d_putBackMode;      // 'true' if we're in putback mode,
                                          // 'false' if normal input

    bslma::Allocator *d_allocator_p;      // used for allocation of 'd_buf_p'

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(Utf8CheckingInStreamBufWrapper,
                                   bslma::UsesBslmaAllocator);

  private:
    // PRIVATE MANIPULATOR
    pos_type setSeekFailure(bsl::ios_base::openmode mode);
        // Set the state of this object to the failed seek state and return a
        // negative position, if the held 'streambuf' is seekable, pass the
        // specified 'mode' to a seek to the beginning of the file.

  protected:
    // PROTECTED MANIPULATORS

                            // implementation functions

    // The following member functions are virtual and protected.  They are part
    // of the implementation and are called by other functions in this class or
    // by functions in the base class.  These functions have no corresponding
    // public member functions that call them.

    int_type overflow(int_type = traits_type::eof()) BSLS_KEYWORD_OVERRIDE;
        // Unconditionally return 'traits_type::eof()'.  The optionally
        // specified argument is ignored.

    bsl::streamsize showmanyc() BSLS_KEYWORD_OVERRIDE;
        // Return the number of bytes that are guaranteed that can be read
        // before 'underflow' returns 'eof'.  If the object is not in a valid
        // state, -1 will be returned.  Note that often, the actual number of
        // bytes that can be read will be much greater than the value returned
        // by this function.

    int_type underflow() BSLS_KEYWORD_OVERRIDE;
        // Replenish the input buffer with data obtained from the held
        // 'streambuf', and return the next byte of input (or 'eof' if no input
        // is available).  This function assumes that either the input buffer
        // is empty or that the end of it has been reached.  If this object is
        // not in a valid state, 'eof' will be returned.

                        //   functions forwarded to by
                        // corresponding public functions

    // The following protected virtual functions all have corresponding public
    // methods in the base class that forward to them.

    void imbue(const bsl::locale& locale) BSLS_KEYWORD_OVERRIDE;
        // If 'sb' is the name of the 'streambuf' held by this object, set 'sb'
        // to the specified 'locale' as though 'sb.pubimbue(locale)' had been
        // called.  If this object does not hold a 'streambuf', this method has
        // no effect.  Note that this function is forwarded to by the public
        // method 'pubimbue' in the base class.

    int_type pbackfail(int_type c = traits_type::eof()) BSLS_KEYWORD_OVERRIDE;
        // Back up input one byte.  Return the byte at the new position, or
        // 'eof' with the state of this object unchanged on failure.  If the
        // optionally specified 'c' is not 'eof', substitute 'c' for the
        // previous byte and return that value.  If 'c' is 'eof', do not
        // substitute it for the previous byte and return the byte was there,
        // or if the previous byte is unknown, fail.  If values of 'c' that are
        // not 'eof' are specified, this function will succeed for at least 8
        // successive calls, possibly many more times.  The behavior is
        // undefined unless 'c' is either 'eof' or a value representable as a
        // 'char_type'.  Note that this is forwarded to with a 'char_type'
        // passed to 'c' by the public method 'sputbackc' in the base class,
        // and called with 'eof' passed to 'c' by the public method 'sungetc'
        // in the base class.

    pos_type seekoff(off_type                offset,
                     bsl::ios_base::seekdir  whence,
                     bsl::ios_base::openmode mode) BSLS_KEYWORD_OVERRIDE;
        // Move the position associated with this object according to the
        // specified 'offset' and 'whence':
        //
        //: o If 'whence' is 'bsl::ios_base::beg', set the position to 'offset'
        //:   bytes from the beginning.
        //: o If 'whence' is 'bsl::ios_base::cur', advance the position by
        //:   'offset' bytes (note that 'offset' is signed).
        //: o 'whence == bsl::ios_base::end' is unsupported and a seek fail
        //:   will result.
        //
        // A seek can fail if
        //
        //: o the object was already in a failed seek state and the seek was
        //:   not an absolute seek to the beginning,
        //:
        //: o the object is not bound to a held 'streambuf',
        //:
        //: o 'whence' is not 'bsl::ios_base::beg' or 'bsl::ios_base::cur',
        //:
        //: o the destination is negative,
        //:
        //: o the destination is forward of the current position, or
        //:
        //: o a seek on the held 'streambuf' is necessary and that 'streambuf'
        //:   does not support seeking,
        //
        // which will put the object into a 'failed seek state'.  When the
        // object is in a failed seek state, 'errorStatus()' will equal
        // 'k_SEEK_FAIL' and the object will no longer have a valid position,
        // meaning that input and relative seeks will fail, until the object is
        // made to recover by either calling 'reset' or an absolute seek to
        // position 0.
        //
        // If a seek is performed on the held 'streambuf', the specified 'mode'
        // will be propagated to it.  The behavior is undefined unless
        // 'bsl::ios_base::in' is set in 'mode'.  Note that this function is
        // forwarded to by the public method 'pubseekoff' in the base class.
        //
        // 'seekoff(0, bsl::ios_base::cur, mode)' is permissible whether the
        // held 'streambuf' is seekable or not and will never result in a seek
        // on the held 'streambuf', returning the position in terms of the held
        // 'streambuf' if that 'streambuf' is seekable and returning the
        // position relative to when the held 'streambuf' was bound to this
        // object otherwise.
        //
        // Some non-zero seeks will be performed without a seek on the held
        // 'streambuf', but there is no simple way for the client to predict
        // when this will be the case.

    pos_type seekpos(pos_type                offset,
                     bsl::ios_base::openmode mode) BSLS_KEYWORD_OVERRIDE;
        // Set the position of this object to the specified absolute 'offset'.
        // If a seek on the held 'streambuf' occurs, the specified 'mode' is
        // passed to it.  This function delegates to
        // 'seekoff(offset, bsl::ios_base::beg, mode)', see that function for
        // further detail.  The behavior is undefined unless
        // 'bsl::ios_base::in' is set in 'mode'.  Note that this function is
        // forwarded to by the public method 'pubseekpos' in the base class.

    bsl::streamsize xsgetn(char            *buffer,
                           bsl::streamsize  numBytes) BSLS_KEYWORD_OVERRIDE;
        // Read up to the specified 'numBytes' characters from this object to
        // the specified 'buffer' and return the number of characters
        // successfully read.  A return value of 0 means that either a UTF-8
        // error or end of file has been encountered ('errorStatus' must be
        // called to distinguish between the two), but a non-zero return value
        // less than 'numBytes' will usually be returned when neither end of
        // file nor a UTF-8 error has been encountered.  The behavior is
        // undefined unless '4 <= numBytes'.  Note that this function is
        // forwarded to by the public method 'sgetn' in the base class.

    bsl::streamsize xsputn(const char      *,
                           bsl::streamsize  ) BSLS_KEYWORD_OVERRIDE;
        // Output function, not supported in this input-only implementation;
        // stubbed out, arguments ignored, returns 0.  Note that this function
        // is forwarded to by 'sputn' in the base class.

  private:
    // NOT IMPLEMENTED
    Utf8CheckingInStreamBufWrapper(const Utf8CheckingInStreamBufWrapper&)
                                                          BSLS_KEYWORD_DELETED;
    Utf8CheckingInStreamBufWrapper& operator=(
                   const Utf8CheckingInStreamBufWrapper&) BSLS_KEYWORD_DELETED;

  public:
    // CLASS METHODS
    static
    const char *toAscii(int errorStatus);
        // Return a description of the specified 'errorStatus'.  Note that
        // 'errorStatus' is either:
        //: o 'k_SEEK_FAIL'
        //:
        //: o A value from 'Utf8Util::ErrorStatus', which are all negative, in
        //:   the case of invalid UTF-8.
        //:
        //: o 0 if no errors have occurred, in which case 'NO_ERROR' will be
        //:   returned.  Note that this includes the case where end of file has
        //:   been reached without any error occurring.
        //:
        //: o If 'errorStatus' is an invalid value, "(* unrecognized value *)"
        //:   will be returned.

    // CREATORS
    Utf8CheckingInStreamBufWrapper();
    explicit Utf8CheckingInStreamBufWrapper(bslma::Allocator *basicAllocator);
        // Create a 'Utf8StreamBufInputWrapper' object having no associated
        // 'streambuf'.  Optionally specify a 'basicAllocator' used to supply
        // memory.  If 'basicAllocator' is 0 or not specified, the currently
        // installed default allocator is used.

    explicit
    Utf8CheckingInStreamBufWrapper(bsl::streambuf   *streamBuf,
                                   bslma::Allocator *basicAllocator = 0);
        // Create a 'Utf8StreamBufInputWrapper' associated with the specified
        // 'streamBuf'.  Optionally specify a 'basicAllocator' used to supply
        // memory.  If 'basicAllocator' is 0, the currently installed default
        // allocator is used.

    ~Utf8CheckingInStreamBufWrapper();
        // Destroy this object.

    // MANIPULATOR
    void reset(bsl::streambuf *streamBuf);
        // Associate this object with the specified 'streamBuf', releasing any
        // previously held 'streambuf'.

    // ACCESSORS
    int errorStatus() const;
        // Return the current error mode of this object.  This will be either 0
        // (no errors or end of data), 'k_SEEK_FAIL', which is positive, or a
        // value from 'Utf8Util::ErrorStatus', which are all negative.

    bool isValid() const;
        // Return 'true' if this wrapper currently holds a 'streambuf' and is
        // not in a failed seek state.
};

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

// ACCESSORS
inline
int Utf8CheckingInStreamBufWrapper::errorStatus() const
{
    return d_errorStatus;
}

inline
bool Utf8CheckingInStreamBufWrapper::isValid() const
{
    return d_heldStreamBuf_p && k_SEEK_FAIL != d_errorStatus;
}

}  // close package namespace
}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Note that this implementation is derived from 'bdls::FdStreamBuf' which is
// based on STLPort's implementation of 'filebuf', with copyright notice as
// follows:
//
// Adapted to bde from STLport, 2009
//     'bdls::FdStreamBuf' from 'bsl::filebuf'
//     'bdls::FdStreamBuf_FileHandler' from 'bsl::_Filebuf_base'
//
// Copyright (c) 1999
// Silicon Graphics Computer Systems, Inc.
//
// Copyright (c) 1999
// Boris Fomitchev
//
// This material is provided "as is", with absolutely no warranty expressed
// or implied.  Any use is at your own risk.
//
// Permission to use or copy this software for any purpose is hereby granted
// without fee, provided the above notices are retained on all copies.
// Permission to modify the code and to distribute modified code is granted,
// provided the above notices are retained, and a notice that the code was
// modified is included with the above copyright notice.
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// Copyright 2020 Bloomberg Finance L.P.
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
