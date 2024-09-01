// bslstl_syncbuf.h                                                   -*-C++-*-
#ifndef INCLUDED_BSLSTL_SYNCBUF
#define INCLUDED_BSLSTL_SYNCBUF

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a C++20-compatible `basic_syncbuf` class template.
//
//@CLASSES:
//  bsl::basic_syncbuf: C++20-compatible `basic_syncbuf` class template.
//  bsl::syncbuf: C++20-compatible `syncbuf` class.
//  bsl::wsyncbuf: C++20-compatible `wsyncbuf` class.
//
//@CANONICAL_HEADER: bsl_syncstream.h
//
//@SEE_ALSO: bslstl_osyncstream
//
//@DESCRIPTION: This component is for internal use only.  Please include
// `<bsl_syncstream.h>` instead.
//
// This component defines a class template, `bsl::basic_syncbuf`, that is a
// wrapper for a `bsl::basic_streambuf` (provided at construction time as a
// pointer).  It accumulates output in its own internal buffer, and atomically
// transmits its entire contents to the wrapped buffer on destruction and when
// explicitly requested, so that they appear as a contiguous sequence of
// characters.  It guarantees that there are no data races and no interleaving
// of characters sent to the wrapped buffer as long as all other outputs made
// to the same buffer are made through, possibly different, instances of
// `bsl::basic_syncbuf`.
//
// Each `bsl::basic_syncbuf` has the associated "emit-on-sync" boolean flag
// that is `false` after the object construction and its value can be changed
// using the `set_emit_on_sync` member function call.  If this flag has value
// `true`, the `emit` function is called by each `sync` call.
//
// Types `bsl::syncbuf` and `bsl::wsyncbuf` are aliases for
// `bsl::basic_syncbuf<char>` and `bsl::basic_syncbuf<wchar_t>`, respectively.
//
///Usage
///-----
// This section illustrates possible use of this component.  But note that this
// component is not intended for direct usage - usually `osyncstream` should be
// used instead.
//
///Example 1: Usage with existing `ostream`
/// - - - - - - - - - - - - - - - - - - - -
// The following example demonstrates temporary replacement of the underlying
// `streambuf` within the existing `ostream` object.
// ```
// void writeSync(bsl::ostream& os)
//     // Write atomically to the specified 'os' output stream.
// {
//     // Temporarily replace the underlying 'streambuf'
//     bsl::syncbuf buf(os.rdbuf());
//     os.rdbuf(&buf);
//
//     // Write to the 'syncbuf'
//     os << "Hello, ";
//     os << "World!\n";
//
//     // Restore the underlying 'streambuf'
//     os.rdbuf(buf.get_wrapped());
//
//     // The accumulated output will be atomically flushed/emitted here
// }
// ```
// Now call the function:
// ```
// writeSync(bsl::cout);
// ```

#include <bslscm_version.h>

#include <bslalg_swaputil.h>

#include <bslma_bslallocator.h>
#include <bslma_usesbslmaallocator.h>

#include <bslmf_movableref.h>

#include <bsls_assert.h>
#include <bsls_bsllock.h>
#include <bsls_exceptionutil.h>
#include <bsls_keyword.h>
#include <bsls_libraryfeatures.h>

#include <bslstl_iosfwd.h>
#include <bslstl_stringbuf.h>
#include <bslstl_syncbufbase.h>

#include <streambuf>
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_STREAM_MOVE
#include <utility>  // move
#endif

namespace bsl {

// Internal type
typedef BloombergLP::bsls::BslLock SyncBuf_Mutex;

                            // ===================
                            // class basic_syncbuf
                            // ===================

/// This class implements a standard stream buffer providing an internal
/// buffer to accumulate the written data in order to atomically transmit
/// its entire contents to the wrapped buffer on destruction (or `emit`
/// call).
template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
class basic_syncbuf : public std::basic_streambuf<CHAR_TYPE, CHAR_TRAITS>,
                      public BloombergLP::bslstl::SyncBufBase {

    // PRIVATE TYPES
    typedef BloombergLP::bslmf::MovableRefUtil MoveUtil;

    // PRIVATE MANIPULATORS

    /// This function is a private alias for `emit`. Note that this virtual
    /// function implementation is private because the base class is
    /// deliberately not a template (see `bslstl_syncbufbase`) and we don't
    /// want to expose this method directly to users.
    bool emitInternal() BSLS_KEYWORD_OVERRIDE;

    /// This function is a private alias for `set_emit_on_sync`. Note that
    /// this virtual function implementation is private because the base
    /// class is deliberately not a template (see `bslstl_syncbufbase`) and
    /// we don't want to expose this method directly to users.
    void setEmitOnSync(bool value) BSLS_KEYWORD_NOEXCEPT BSLS_KEYWORD_OVERRIDE;

  public:
    // TYPES
    typedef CHAR_TYPE                      char_type;
    typedef typename CHAR_TRAITS::int_type int_type;
    typedef typename CHAR_TRAITS::pos_type pos_type;
    typedef typename CHAR_TRAITS::off_type off_type;
    typedef CHAR_TRAITS                    traits_type;
    typedef ALLOCATOR                      allocator_type;

    typedef std::basic_streambuf<CHAR_TYPE, CHAR_TRAITS> streambuf_type;

  private:
    // DATA

    // wrapped buffer
    streambuf_type                                     *d_wrapped_p;

    // mutex for `emit`
    SyncBuf_Mutex                                      *d_mutex_p;

    // "emit-on-sync" flag
    bool                                                d_emit_on_sync;

    // sync call was requested
    bool                                                d_needs_sync;

    // internal buffer
    basic_stringbuf<CHAR_TYPE, CHAR_TRAITS, ALLOCATOR>  d_buff;

  public:
    // CREATORS

    /// Create a `basic_syncbuf` object without a wrapped buffer.
    /// Optionally specify an `allocator` used to supply memory.
    explicit basic_syncbuf(const ALLOCATOR& allocator = ALLOCATOR());

    /// Create a `basic_syncbuf` object.  Set the specified `wrapped` as a
    /// wrapped buffer.  Optionally specify an `allocator` used to supply
    /// memory.
    explicit basic_syncbuf(streambuf_type   *wrapped,
                           const ALLOCATOR&  allocator = ALLOCATOR());

    /// Call `emit`.  Any exceptions thrown by `emit` are ignored.
    ~basic_syncbuf() BSLS_KEYWORD_OVERRIDE;

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_STREAM_MOVE
    /// Create a `basic_syncbuf` object having the same value as the
    /// specified `original` object by moving the contents of `original` to
    /// the newly-created object.  Optionally specify an `allocator` used
    /// to supply memory.  `original.get_wrapped() == nullptr` after the
    /// call.
    basic_syncbuf(basic_syncbuf&& original);
    basic_syncbuf(basic_syncbuf&& original, const ALLOCATOR& allocator);
#endif

    // MANIPULATORS

    /// Atomically transfer any characters buffered by this object to the
    /// wrapped stream buffer, so that it appears in the output stream as a
    /// contiguous sequence of characters.  The wrapped stream buffer is
    /// flushed if and only if a call was made to `sync` since the most
    /// recent call to `emit` or construction.  Return `true` if
    /// `get_wrapped() != nullptr`, and all of the characters in the
    /// associated output were successfully transferred, and the flush (if
    /// any) succeeded; return `false` otherwise.
    bool emit();

    /// Call the `emit` function by each `sync` call if the specified
    /// `value` is `true`.
    void set_emit_on_sync(bool value) BSLS_KEYWORD_NOEXCEPT;

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_STREAM_MOVE
    /// Call `emit()` then assign to this object the value of the specified
    /// `original`, and return a reference providing modifiable access to
    /// this object.  The contents of `original` are move-assigned to this
    /// object.  `original.get_wrapped() == nullptr` after the call.
    basic_syncbuf& operator=(basic_syncbuf&& original);

    /// Efficiently exchange the value of this object with the value of the
    /// specified `other` object.  The behavior is undefined unless either
    /// `*this` and `other` allocators compare equal or
    /// `propagate_on_container_swap` is `true`.
    void swap(basic_syncbuf& other);
#endif

    // ACCESSORS

    /// Return the allocator used to supply memory.
    allocator_type get_allocator() const BSLS_KEYWORD_NOEXCEPT;

    /// Return the wrapped buffer supplied at construction.
    streambuf_type *get_wrapped() const BSLS_KEYWORD_NOEXCEPT;

  protected:
    // PROTECTED MANIPULATORS
    int_type overflow(int_type character = traits_type::eof())
                                                         BSLS_KEYWORD_OVERRIDE;
        // Do nothing if 'traits_type::eof()' is passed.  Optionally specify
        // 'character' that has non-default value to add it to the internal
        // buffer and return 'traits_type::to_int_type(character)'.  Return
        // 'traits_type::eof()' otherwise.

    /// Request the wrapped streambuf flush on the next `emit` call, then
    /// call `emit` if the "emit-on-sync" flag is `true`.  Return 0 on
    /// success and -1 if the `emit` call has failed.
    int sync() BSLS_KEYWORD_OVERRIDE;

    std::streamsize xsputn(const char_type *inputString, std::streamsize count)
                                                         BSLS_KEYWORD_OVERRIDE;
        // Write the specified 'inputString' array of the specified 'count'
        // characters to the internal buffer.  Return the number of characters
        // successfully written.
};

// STANDARD TYPEDEFS
typedef basic_syncbuf<char>     syncbuf;
typedef basic_syncbuf<wchar_t> wsyncbuf;

                            // =======================
                            // class SyncBuf_MutexUtil
                            // =======================

/// Internal mutex-related utils.
struct SyncBuf_MutexUtil {

    // CLASS METHODS

    /// Return address of a mutex associated the specified `streambuf`
    /// object (address).  The behavior is undefined unless `streambuf` is
    /// not null and points to `basic_streambuf<...>` object.
    static SyncBuf_Mutex *get(void *streambuf) BSLS_KEYWORD_NOEXCEPT;
};

                            // -------------------
                            // class basic_syncbuf
                            // -------------------

// CREATORS
template <class CHAR, class TRAITS, class ALLOCATOR>
basic_syncbuf<CHAR,TRAITS,ALLOCATOR>::basic_syncbuf(const ALLOCATOR& allocator)
: d_wrapped_p(0)
, d_mutex_p(0)
, d_emit_on_sync(false)
, d_needs_sync(false)
, d_buff(allocator)
{
}

template <class CHAR, class TRAITS, class ALLOCATOR>
basic_syncbuf<CHAR,TRAITS,ALLOCATOR>::basic_syncbuf(
                                                   streambuf_type   *wrapped,
                                                   const ALLOCATOR&  allocator)
: d_wrapped_p(wrapped)
, d_mutex_p(wrapped ? SyncBuf_MutexUtil::get(wrapped) : 0)
, d_emit_on_sync(false)
, d_needs_sync(false)
, d_buff(allocator)
{
}

template <class CHAR, class TRAITS, class ALLOCATOR>
basic_syncbuf<CHAR,TRAITS,ALLOCATOR>::~basic_syncbuf()
{
    BSLS_TRY {
        emit();
    }
    BSLS_CATCH(...) {
        // ignore
    }
}

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_STREAM_MOVE
template <class CHAR, class TRAITS, class ALLOCATOR>
basic_syncbuf<CHAR,TRAITS,ALLOCATOR>::basic_syncbuf(basic_syncbuf&& original)
: d_wrapped_p(original.d_wrapped_p)
, d_mutex_p(original.d_mutex_p)
, d_emit_on_sync(original.d_emit_on_sync)
, d_needs_sync(original.d_needs_sync)
, d_buff(std::move(original.d_buff))
{
    original.d_wrapped_p = 0;
    original.d_mutex_p = 0;
}

template <class CHAR, class TRAITS, class ALLOCATOR>
basic_syncbuf<CHAR,TRAITS,ALLOCATOR>::basic_syncbuf(basic_syncbuf&&  original,
                                                    const ALLOCATOR& allocator)
: d_wrapped_p(original.d_wrapped_p)
, d_mutex_p(original.d_mutex_p)
, d_emit_on_sync(original.d_emit_on_sync)
, d_needs_sync(original.d_needs_sync)
, d_buff(std::move(original.d_buff), allocator)
{
    original.d_wrapped_p = 0;
    original.d_mutex_p = 0;
}
#endif

// MANIPULATORS
template <class CHAR, class TRAITS, class ALLOCATOR>
bool basic_syncbuf<CHAR,TRAITS,ALLOCATOR>::emit()
{
    if (!this->d_wrapped_p) {
        return false;                                                 // RETURN
    }

    typedef basic_string<CHAR,TRAITS,ALLOCATOR> String;
    typedef typename string::size_type          SizeType;

    BloombergLP::bsls::BslLockGuard lock(this->d_mutex_p);
    String s =
#ifdef BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS
        std::move(d_buff).str();
#else
        d_buff.str();
        d_buff.str(String());
#endif
    if (SizeType size = s.length()) {
        SizeType n = this->d_wrapped_p->sputn(s.data(), size);
        if (n != size) {
            s.erase(0, n);
            d_buff.str(MoveUtil::move(s));
            return false;                                             // RETURN
        }
    }
    if (this->d_needs_sync) {
        this->d_needs_sync = false;
        if (this->d_wrapped_p->pubsync() != 0) {
            return false;                                             // RETURN
        }
    }
    return true;
}

template <class CHAR, class TRAITS, class ALLOCATOR>
bool basic_syncbuf<CHAR,TRAITS,ALLOCATOR>::emitInternal()
{
    return emit();
}

template <class CHAR, class TRAITS, class ALLOCATOR>
inline
void basic_syncbuf<CHAR,TRAITS,ALLOCATOR>::set_emit_on_sync(bool value)
                                                          BSLS_KEYWORD_NOEXCEPT
{
    d_emit_on_sync = value;
}

template <class CHAR, class TRAITS, class ALLOCATOR>
void basic_syncbuf<CHAR,TRAITS,ALLOCATOR>::setEmitOnSync(bool value)
                                                          BSLS_KEYWORD_NOEXCEPT
{
    set_emit_on_sync(value);
}

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_STREAM_MOVE
template <class CHAR, class TRAITS, class ALLOCATOR>
basic_syncbuf<CHAR,TRAITS,ALLOCATOR>&
basic_syncbuf<CHAR,TRAITS,ALLOCATOR>::operator=(basic_syncbuf&& original)
{
    emit();
    if (&original != this) {
        d_buff = std::move(original.d_buff);
        this->d_wrapped_p    = original.d_wrapped_p;
        this->d_mutex_p      = original.d_mutex_p;
        this->d_emit_on_sync = original.d_emit_on_sync;
        this->d_needs_sync   = original.d_needs_sync;

        original.d_wrapped_p = 0;
        original.d_mutex_p = 0;
    }
    return *this;
}

template <class CHAR, class TRAITS, class ALLOCATOR>
void basic_syncbuf<CHAR,TRAITS,ALLOCATOR>::swap(basic_syncbuf& other)
{
    BSLS_ASSERT(allocator_traits<ALLOCATOR>::propagate_on_container_swap::value
             || get_allocator() == other.get_allocator());
    typedef BloombergLP::bslalg::SwapUtil SwapUtil;
    streambuf_type::swap(other);
    SwapUtil::swap(&d_wrapped_p,    &other.d_wrapped_p);
    SwapUtil::swap(&d_mutex_p,      &other.d_mutex_p);
    SwapUtil::swap(&d_emit_on_sync, &other.d_emit_on_sync);
    SwapUtil::swap(&d_needs_sync,   &other.d_needs_sync);
    d_buff.swap(other.d_buff);
}
#endif

// ACCESSORS
template <class CHAR, class TRAITS, class ALLOCATOR>
inline
ALLOCATOR basic_syncbuf<CHAR,TRAITS,ALLOCATOR>::get_allocator() const
                                                          BSLS_KEYWORD_NOEXCEPT
{
    return d_buff.get_allocator();
}

template <class CHAR, class TRAITS, class ALLOCATOR>
inline
typename basic_syncbuf<CHAR,TRAITS,ALLOCATOR>::streambuf_type *
basic_syncbuf<CHAR,TRAITS,ALLOCATOR>::get_wrapped() const BSLS_KEYWORD_NOEXCEPT
{
    return d_wrapped_p;
}

// PROTECTED MANIPULATORS
template <class CHAR, class TRAITS, class ALLOCATOR>
typename basic_syncbuf<CHAR,TRAITS,ALLOCATOR>::int_type
basic_syncbuf<CHAR,TRAITS,ALLOCATOR>::overflow(int_type character)
{
    if (!traits_type::eq_int_type(character, traits_type::eof())) {
        return d_buff.sputc(traits_type::to_char_type(character));    // RETURN
    }
    return traits_type::eof();
}

template <class CHAR, class TRAITS, class ALLOCATOR>
int basic_syncbuf<CHAR,TRAITS,ALLOCATOR>::sync()
{
    this->d_needs_sync = true;
    if (this->d_emit_on_sync && !emit()) {
        return -1;                                                    // RETURN
    }
    return 0;
}

template <class CHAR, class TRAITS, class ALLOCATOR>
std::streamsize basic_syncbuf<CHAR,TRAITS,ALLOCATOR>::xsputn(
                                                  const char_type *inputString,
                                                  std::streamsize  count)
{
    return d_buff.sputn(inputString, count);
}

// FREE FUNCTIONS
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_STREAM_MOVE
/// Swap the specified `a` with the specified `b` using `a.swap(b)`
/// expression.
template <class CHAR, class TRAITS, class ALLOCATOR>
inline
void swap(basic_syncbuf<CHAR,TRAITS,ALLOCATOR>& a,
          basic_syncbuf<CHAR,TRAITS,ALLOCATOR>& b)
{
    a.swap(b);
}
#endif

}  // close namespace bsl


// ============================================================================
//                                TYPE TRAITS
// ============================================================================

namespace BloombergLP {
namespace bslma {

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
struct UsesBslmaAllocator<bsl::basic_syncbuf<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR> >
    : bsl::true_type
{};

}  // close namespace bslma
}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2023 Bloomberg Finance L.P.
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
