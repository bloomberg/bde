// bslstl_syncbuf.h                                                   -*-C++-*-
#ifndef INCLUDED_BSLSTL_SYNCBUF
#define INCLUDED_BSLSTL_SYNCBUF

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a C++20-compatible 'basic_syncbuf' class template.
//
//@CLASSES:
//  bsl::basic_syncbuf: C++20-compatible 'basic_syncbuf' class template.
//  bsl::syncbuf: C++20-compatible 'syncbuf' class.
//  bsl::wsyncbuf: C++20-compatible 'wsyncbuf' class.
//
//@CANONICAL_HEADER: bsl_syncstream.h
//
//@SEE_ALSO: bslstl_osyncstream
//
//@DESCRIPTION: This component is for internal use only.  Please include
// '<bsl_syncstream.h>' instead.
//
// This component defines a class template, 'bsl::basic_syncbuf', that is a
// wrapper for a 'bsl::basic_streambuf' (provided at construction time as a
// pointer).  It accumulates output in its own internal buffer, and atomically
// transmits its entire contents to the wrapped buffer on destruction and when
// explicitly requested, so that they appear as a contiguous sequence of
// characters.  It guarantees that there are no data races and no interleaving
// of characters sent to the wrapped buffer as long as all other outputs made
// to the same buffer are made through, possibly different, instances of
// 'bsl::basic_syncbuf'.
//
// Each 'bsl::basic_syncbuf' has the associated "emit-on-sync" boolean flag
// that is 'false' after the object construction and its value can be changed
// using the 'set_emit_on_sync' member function call.  If this flag has value
// 'true', the 'emit' function is called by each 'sync' call.
//
// Types 'bsl::syncbuf' and 'bsl::wsyncbuf' are aliases for
// 'bsl::basic_syncbuf<char>' and 'bsl::basic_syncbuf<wchar_t>', respectively.
//
///Usage
///-----
// This section illustrates possible use of this component.  But note that this
// component is not intended for direct usage - usually 'osyncstream' should be
// used instead.
//
///Example 1: Usage with existing 'ostream'
/// - - - - - - - - - - - - - - - - - - - -
// The following example demonstrates temporary replacement of the underlying
// 'streambuf' within the existing 'ostream' object.
//..
//  void writeSync(bsl::ostream& os)
//      // Write atomically to the specified 'os' output stream.
//  {
//      // Temporarily replace the underlying 'streambuf'
//      bsl::syncbuf buf(os.rdbuf());
//      os.rdbuf(&buf);
//
//      // Write to the 'syncbuf'
//      os << "Hello, ";
//      os << "World!\n";
//
//      // Restore the underlying 'streambuf'
//      os.rdbuf(buf.get_wrapped());
//
//      // The accumulated output will be atomically flushed/emitted here
//  }
//..
// Now call the function:
//..
//  writeSync(bsl::cout);
//..

#include <bslscm_version.h>

#include <bslalg_swaputil.h>

#include <bslma_stdallocator.h>
#include <bslma_usesbslmaallocator.h>

#include <bslmf_movableref.h>

#include <bsls_assert.h>
#include <bsls_bsllock.h>
#include <bsls_exceptionutil.h>
#include <bsls_keyword.h>
#include <bsls_libraryfeatures.h>

#include <bslstl_iosfwd.h>
#include <bslstl_stringbuf.h>

#include <streambuf>
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_STREAM_MOVE
#include <utility>  // move
#endif

namespace bsl {

// Internal type
typedef BloombergLP::bsls::BslLock syncbuf_Mutex;

                            // ==================
                            // class syncbuf_Base
                            // ==================

template <class CHAR_TYPE, class CHAR_TRAITS>
class syncbuf_Base : public std::basic_streambuf<CHAR_TYPE, CHAR_TRAITS> {
    // Allocator-independent base of 'basic_syncbuf'.  Also this is used by the
    // manipulators: 'emit_on_flush', 'noemit_on_flush', 'flush_emit' (the
    // 'ALLOCATOR' template argument cannot be deduced from
    // 'basic_ostream<CHAR_TYPE, CHAR_TRAITS>').

    // PRIVATE MANIPULATORS
    virtual bool emitInternal() = 0;
        // Internal function for the 'flush_emit' manipulator.

    // FRIENDS
    template <class CHAR, class TRAITS>
    friend std::basic_ostream<CHAR,TRAITS>& flush_emit(
                                          std::basic_ostream<CHAR,TRAITS>& os);

  public:
    // TYPES
    typedef std::basic_streambuf<CHAR_TYPE, CHAR_TRAITS> streambuf_type;

    // MANIPULATORS
    void set_emit_on_sync(bool value) BSLS_KEYWORD_NOEXCEPT;
        // Set the "emit-on-sync" flag to the specified 'value'.

    // ACCESSORS
    streambuf_type *get_wrapped() const BSLS_KEYWORD_NOEXCEPT;
        // Return the wrapped buffer.

  protected:
    // PROTECTED CREATORS
    explicit syncbuf_Base(streambuf_type *wrapped);
        // Create a 'syncbuf_Base' object.  Set the specified 'wrapped' as a
        // wrapped buffer.

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_STREAM_MOVE
    syncbuf_Base(syncbuf_Base&& original);
        // Create a 'syncbuf_Base' object having the same value as the
        // specified 'original' object by moving the contents of 'original' to
        // the newly-created object.  'original' is left in a valid but
        // unspecified state.
#endif
    ~syncbuf_Base();
        // Destroy this object.

    // PROTECTED MANIPULATORS
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_STREAM_MOVE
    void swap(syncbuf_Base &other);
        // Efficiently exchange the value of this object with the value of the
        // specified 'other' object.
#endif

    // PROTECTED DATA
    streambuf_type *d_wrapped_p;     // wrapped buffer
    syncbuf_Mutex  *d_mutex_p;       // mutex for 'emit'
    bool            d_emit_on_sync;  // "emit-on-sync" flag
    bool            d_needs_sync;    // sync call was requested
};

                            // ===================
                            // class basic_syncbuf
                            // ===================

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
class basic_syncbuf : public syncbuf_Base<CHAR_TYPE, CHAR_TRAITS> {
    // This class implements a standard stream buffer providing an internal
    // buffer to accumulate the written data in order to atomically transmit
    // its entire contents to the wrapped buffer on destruction (or 'emit'
    // call).

    // PRIVATE TYPES
    typedef syncbuf_Base<CHAR_TYPE, CHAR_TRAITS> Base;
    typedef BloombergLP::bslmf::MovableRefUtil   MoveUtil;

    // PRIVATE MANIPULATORS
    bool emitInternal() BSLS_KEYWORD_OVERRIDE;
        // Internal function for the 'flush_emit' manipulator.

  public:
    // TYPES
    typedef CHAR_TYPE                      char_type;
    typedef typename CHAR_TRAITS::int_type int_type;
    typedef typename CHAR_TRAITS::pos_type pos_type;
    typedef typename CHAR_TRAITS::off_type off_type;
    typedef CHAR_TRAITS                    traits_type;
    typedef ALLOCATOR                      allocator_type;
    typedef typename Base::streambuf_type  streambuf_type;

    // CREATORS
    explicit basic_syncbuf(const ALLOCATOR& allocator = ALLOCATOR());
        // Create a 'basic_syncbuf' object without a wrapped buffer.
        // Optionally specify an 'allocator' used to supply memory.

    explicit basic_syncbuf(streambuf_type   *wrapped,
                           const ALLOCATOR&  allocator = ALLOCATOR());
        // Create a 'basic_syncbuf' object.  Set the specified 'wrapped' as a
        // wrapped buffer.  Optionally specify an 'allocator' used to supply
        // memory.

    ~basic_syncbuf();
        // Call 'emit'.  Any exceptions thrown by 'emit' are ignored.

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_STREAM_MOVE
    basic_syncbuf(basic_syncbuf&& original);
    basic_syncbuf(basic_syncbuf&& original, const ALLOCATOR& allocator);
        // Create a 'basic_syncbuf' object having the same value as the
        // specified 'original' object by moving the contents of 'original' to
        // the newly-created object.  Optionally specify an 'allocator' used
        // to supply memory.  'original.get_wrapped() == nullptr' after the
        // call.
#endif

    // MANIPULATORS
    bool emit();
        // Atomically transfer the associated output of '*this' to the wrapped
        // stream buffer, so that it appears in the output stream as a
        // contiguous sequence of characters.  'get_wrapped()->pubsync()' is
        // called if and only if a call was made to 'sync()' since the most
        // recent call to 'emit()', if any.  Return 'true' iff all of the
        // following conditions hold: 'get_wrapped() != nullptr', all of the
        // characters in the associated output were successfully transferred,
        // the call to 'get_wrapped()->pubsync()' (if any) succeeded.

    using Base::set_emit_on_sync;
        // Set the "emit-on-sync" flag to the specified 'value'.

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_STREAM_MOVE
    basic_syncbuf& operator=(basic_syncbuf&& original);
        // Call 'emit()' then assign to this object the value of the specified
        // 'original', and return a reference providing modifiable access to
        // this object.  The contents of 'original' are move-assigned to this
        // object.  'original.get_wrapped() == nullptr' after the call.

    void swap(basic_syncbuf& other);
        // Efficiently exchange the value of this object with the value of the
        // specified 'other' object.  The behavior is undefined unless either
        // '*this' and 'other' allocators compare equal or
        // 'propagate_on_container_swap' is 'true'.
#endif

    // ACCESSORS
    allocator_type get_allocator() const BSLS_KEYWORD_NOEXCEPT;
        // Return the allocator used by the underlying string to supply memory.

    using Base::get_wrapped;
        // Return the wrapped buffer.

  protected:
    // PROTECTED MANIPULATORS
    int_type overflow(int_type ch = traits_type::eof()) BSLS_KEYWORD_OVERRIDE;
        // Do nothing if 'traits_type::eof()' is passed.  Optionally specify
        // 'ch' that has non-default value to add it to the internal buffer and
        // return 'traits_type::to_int_type(ch)'.  Return 'traits_type::eof()'
        // otherwise.

    int sync() BSLS_KEYWORD_OVERRIDE;
        // Note the sync request.  Then call 'emit' if the "emit-on-sync" flag
        // is 'true'.  Return 0 on success and -1 if the 'emit' call has
        // failed.

    std::streamsize xsputn(const char_type *s, std::streamsize count)
                                                         BSLS_KEYWORD_OVERRIDE;
        // Write the specified 's' array of the specified 'count' characters
        // to the internal buffer.  Return the number of characters
        // successfully written.

  private:
    // DATA
    basic_stringbuf<CHAR_TYPE, CHAR_TRAITS, ALLOCATOR> d_buff;
        // internal buffer
};

// STANDARD TYPEDEFS
typedef basic_syncbuf<char>     syncbuf;
typedef basic_syncbuf<wchar_t> wsyncbuf;

// FREE FUNCTIONS

// Internal mutex-related utils
syncbuf_Mutex *syncbuf_GetMutex(void *streambuf) BSLS_KEYWORD_NOEXCEPT;
    // Return address of a mutex associated the specified 'streambuf' object
    // (address).  The behavior is undefined unless 'streambuf' is not null.

                            // ------------------
                            // class syncbuf_Base
                            // ------------------

// CREATORS
template <class CHAR, class TRAITS>
inline
syncbuf_Base<CHAR,TRAITS>::syncbuf_Base(streambuf_type *wrapped)
: d_wrapped_p(wrapped)
, d_mutex_p(wrapped ? syncbuf_GetMutex(wrapped) : 0)
, d_emit_on_sync(false)
, d_needs_sync(false)
{
}

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_STREAM_MOVE
template <class CHAR, class TRAITS>
inline
syncbuf_Base<CHAR,TRAITS>::syncbuf_Base(syncbuf_Base&& original)
: d_wrapped_p(original.d_wrapped_p)
, d_mutex_p(original.d_mutex_p)
, d_emit_on_sync(original.d_emit_on_sync)
, d_needs_sync(original.d_needs_sync)
{
    original.d_wrapped_p = 0;
    original.d_mutex_p = 0;
}
#endif

template <class CHAR, class TRAITS>
syncbuf_Base<CHAR,TRAITS>::~syncbuf_Base()
{
}

// MANIPULATORS
template <class CHAR, class TRAITS>
inline
void syncbuf_Base<CHAR,TRAITS>::set_emit_on_sync(bool value)
                                                          BSLS_KEYWORD_NOEXCEPT
{
    d_emit_on_sync = value;
}

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_STREAM_MOVE
template <class CHAR, class TRAITS>
inline
void syncbuf_Base<CHAR,TRAITS>::swap(syncbuf_Base<CHAR,TRAITS> &other)
{
    typedef BloombergLP::bslalg::SwapUtil SwapUtil;
    streambuf_type::swap(other);
    SwapUtil::swap(&d_wrapped_p,    &other.d_wrapped_p);
    SwapUtil::swap(&d_mutex_p,      &other.d_mutex_p);
    SwapUtil::swap(&d_emit_on_sync, &other.d_emit_on_sync);
    SwapUtil::swap(&d_needs_sync,   &other.d_needs_sync);
}
#endif

// ACCESSORS
template <class CHAR, class TRAITS>
inline
typename syncbuf_Base<CHAR,TRAITS>::streambuf_type *
syncbuf_Base<CHAR,TRAITS>::get_wrapped() const BSLS_KEYWORD_NOEXCEPT
{
    return d_wrapped_p;
}

                            // -------------------
                            // class basic_syncbuf
                            // -------------------

// CREATORS
template <class CHAR, class TRAITS, class ALLOCATOR>
basic_syncbuf<CHAR,TRAITS,ALLOCATOR>::basic_syncbuf(const ALLOCATOR& allocator)
: Base(0)
, d_buff(allocator)
{
}

template <class CHAR, class TRAITS, class ALLOCATOR>
basic_syncbuf<CHAR,TRAITS,ALLOCATOR>::basic_syncbuf(
                                                   streambuf_type   *wrapped,
                                                   const ALLOCATOR&  allocator)
: Base(wrapped)
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
: Base(std::move(original))
, d_buff(std::move(original.d_buff))
{
}

template <class CHAR, class TRAITS, class ALLOCATOR>
basic_syncbuf<CHAR,TRAITS,ALLOCATOR>::basic_syncbuf(basic_syncbuf&&  original,
                                                    const ALLOCATOR& allocator)
: Base(std::move(original))
, d_buff(std::move(original.d_buff), allocator)
{
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
    Base::swap(other);
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

// PROTECTED MANIPULATORS
template <class CHAR, class TRAITS, class ALLOCATOR>
typename basic_syncbuf<CHAR,TRAITS,ALLOCATOR>::int_type
basic_syncbuf<CHAR,TRAITS,ALLOCATOR>::overflow(int_type ch)
{
    if (!traits_type::eq_int_type(ch, traits_type::eof())) {
        return d_buff.sputc(traits_type::to_char_type(ch));           // RETURN
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
                                                        const char_type *s,
                                                        std::streamsize  count)
{
    return d_buff.sputn(s, count);
}

// FREE FUNCTIONS
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_STREAM_MOVE
template <class CHAR, class TRAITS, class ALLOCATOR>
inline
void swap(basic_syncbuf<CHAR,TRAITS,ALLOCATOR>& a,
          basic_syncbuf<CHAR,TRAITS,ALLOCATOR>& b)
    // Swap the specified 'a' with the specified 'b' using 'a.swap(b)'
    // expression.
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
