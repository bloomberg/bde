// bslstl_osyncstream.h                                               -*-C++-*-
#ifndef INCLUDED_BSLSTL_OSYNCSTREAM
#define INCLUDED_BSLSTL_OSYNCSTREAM

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a C++20-compatible 'basic_osyncstream' class template.
//
//@CLASSES:
//  bsl::basic_osyncstream: C++20-compatible 'basic_osyncstream' class.
//  bsl::osyncstream: C++20-compatible 'osyncstream' class.
//  bsl::wosyncstream: C++20-compatible 'wosyncstream' class.
//
//@CANONICAL_HEADER: bsl_syncstream.h
//
//@SEE_ALSO: bslstl_syncbuf
//
//@DESCRIPTION: This component is for internal use only.  Please include
// '<bsl_syncstream.h>' instead.
//
// This component defines a class template, 'bsl::basic_osyncstream', that is a
// convenience wrapper for 'bsl::basic_syncbuf'.  It provides a mechanism to
// synchronize threads writing to the same stream, or more precisely, to the
// same 'streambuf'.  It's guaranteed that all output made to the same final
// destination buffer will be free of data races and will not be interleaved or
// garbled in any way, as long as every write to that final destination buffer
// is made through (possibly different) instances of 'bsl::basic_syncbuf'.
//
// Types 'bsl::osyncstream' and 'bsl::wosyncstream' are aliases for
// 'bsl::basic_osyncstream<char>' and 'bsl::basic_osyncstream<wchar_t>',
// respectively.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
// In a multi-threaded environment attempts to write from different threads to
// one 'ostream' "may result in a data race", according to the ISO C++
// Standard.  Concurrent access to the special synchronized iostream objects,
// like 'cout', 'cerr', etc, does not result in a data race, but output
// characters from one thread can interleave with the characters from other
// threads still.  'osyncstream' solves both problems: prevents data races and
// characters interleaving.
//
///Example 1: Using 'osyncstream'
/// - - - - - - - - - - - - - - -
// The following example demonstrates concurrent printing of a standard STL
// container of values that can be streamed out.  The elements are separated by
// comma and the whole sequence is enclosed in curly brackets.  Note that this
// example requires at least C++11.
//..
//  template <class t_CONTAINER>
//  void printContainer(bsl::ostream& stream, const t_CONTAINER& container)
//      // Print elements of the specified 'container' to the specified
//      // 'stream' in a multi-threaded environment without interleaving with
//      // output from another threads.
//  {
//      bsl::osyncstream out(stream);
//      out << '{';
//      bool first = true;
//      for(auto& value : container) {
//          if (first) {
//              first = false;
//          }
//          else {
//              out << ", ";
//          }
//          out << value;
//      }
//      out << '}';
//  } // all output is atomically transferred to 'stream' on 'out' destruction
//..
// Now this function can safely be used in a multi-threaded environment:
//..
//  int main()
//  {
//      bsl::vector<int>    container1 = {1, 2, 3};
//      bsl::vector<double> container2 = {4.0, 5.0, 6.0, 7.0};
//
//      bsl::thread thread1{[&]{ printContainer(bsl::cout, container1); }};
//      bsl::thread thread2{[&]{ printContainer(bsl::cout, container2); }};
//      thread1.join();
//      thread2.join();
//  }
//..

#include <bslscm_version.h>

#include <bslstl_syncbuf.h>

#include <bsls_keyword.h>
#include <bsls_libraryfeatures.h>

#include <ostream>

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_STREAM_MOVE
#include <utility>  // move
#endif

namespace bsl {

                            // =======================
                            // class basic_osyncstream
                            // =======================

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
class basic_osyncstream : public std::basic_ostream<CHAR_TYPE, CHAR_TRAITS> {
    // This class implements a standard output stream providing an internal
    // buffer to accumulate the written data in order to atomically transmit
    // its entire contents to the wrapped buffer on destruction (or 'emit'
    // call).

    // PRIVATE TYPES
    typedef std::basic_ostream<CHAR_TYPE, CHAR_TRAITS> Base;

  public:
    // TYPES
    typedef CHAR_TYPE                      char_type;
    typedef typename CHAR_TRAITS::int_type int_type;
    typedef typename CHAR_TRAITS::pos_type pos_type;
    typedef typename CHAR_TRAITS::off_type off_type;
    typedef CHAR_TRAITS                    traits_type;
    typedef ALLOCATOR                      allocator_type;

    typedef std::basic_streambuf<CHAR_TYPE, CHAR_TRAITS>     streambuf_type;
    typedef basic_syncbuf<CHAR_TYPE, CHAR_TRAITS, ALLOCATOR> syncbuf_type;

  private:
    // DATA
    syncbuf_type d_syncbuf;  // wrapped 'syncbuf'

  public:
    // CREATORS
    explicit basic_osyncstream(streambuf_type   *wrapped,
                               const ALLOCATOR&  allocator = ALLOCATOR());
        // Create a 'basic_osycnstream' object that will forward stream output
        // to the specified 'wrapped' buffer.  Optionally specify an
        // 'allocator' used to supply memory.  If 'allocator' is not supplied,
        // a default-constructed object of the (template parameter) 'ALLOCATOR'
        // type is used.  If the 'ALLOCATOR' argument is of type
        // 'bsl::allocator' (the default), then 'allocator', if supplied, shall
        // be convertible to 'bslma::Allocator *'.  If the 'ALLOCATOR' argument
        // is of type 'bsl::allocator' and 'allocator' is not supplied, the
        // currently installed default allocator will be used to supply memory.

    explicit basic_osyncstream(
          std::basic_ostream<CHAR_TYPE, CHAR_TRAITS>& stream,
          const ALLOCATOR&                            allocator = ALLOCATOR());
        // Create a 'basic_osycnstream' object that will forward stream output
        // to 'rdbuf' of the specified 'stream'.  Optionally specify an
        // 'allocator' used to supply memory.  If 'allocator' is not supplied,
        // a default-constructed object of the (template parameter) 'ALLOCATOR'
        // type is used.  If the 'ALLOCATOR' argument is of type
        // 'bsl::allocator' (the default), then 'allocator', if supplied, shall
        // be convertible to 'bslma::Allocator *'.  If the 'ALLOCATOR' argument
        // is of type 'bsl::allocator' and 'allocator' is not supplied, the
        // currently installed default allocator will be used to supply memory.

    //! ~basic_osyncstream() = default;
        // Destroy this object.

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_STREAM_MOVE
    basic_osyncstream(basic_osyncstream&& original) BSLS_KEYWORD_NOEXCEPT;
    basic_osyncstream(basic_osyncstream&& original,
                      const ALLOCATOR&    allocator) BSLS_KEYWORD_NOEXCEPT;
        // Create a 'basic_osyncstream' object having the same value as the
        // specified 'original' object by moving the contents of 'original' to
        // the newly-created object.  Optionally specify an 'allocator' used
        // to supply memory.  'original.get_wrapped() == nullptr' after the
        // call.
#endif

    // MANIPULATORS
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_STREAM_MOVE
    basic_osyncstream &operator=(basic_osyncstream&& original) = default;
        // Transfer the associated output to the wrapped stream buffer then
        // assign to this object the value of the specified 'original', and
        // return a reference providing modifiable access to this object.  The
        // contents of 'original' are move-assigned to this object.
        // 'original.get_wrapped() == nullptr' after the call.
#endif

    void emit();
        // Atomically transfer any characters buffered by this object to the
        // wrapped stream buffer, so that it appears in the output stream as a
        // contiguous sequence of characters.  If an error occurs, set the
        // 'badbit' of the 'rdstate' to 'true'.

    // ACCESSORS
    allocator_type get_allocator() const BSLS_KEYWORD_NOEXCEPT;
        // Return the allocator used to supply memory.

    streambuf_type *get_wrapped() const BSLS_KEYWORD_NOEXCEPT;
        // Return the wrapped buffer.

    syncbuf_type *rdbuf() const BSLS_KEYWORD_NOEXCEPT;
        // Return an address providing modifiable access to the 'basic_syncbuf'
        // object that is internally used by this stream object to buffer
        // unformatted characters.
};

// STANDARD TYPEDEFS
typedef basic_osyncstream<char>     osyncstream;
typedef basic_osyncstream<wchar_t> wosyncstream;

                            // -----------------------
                            // class basic_osyncstream
                            // -----------------------

// CREATORS
template <class CHAR, class TRAITS, class ALLOCATOR>
basic_osyncstream<CHAR,TRAITS,ALLOCATOR>::basic_osyncstream(
                                                   streambuf_type   *wrapped,
                                                   const ALLOCATOR&  allocator)
:
#ifndef BSLS_LIBRARYFEATURES_STDCPP_LIBCSTD
  // The Rogue Wave library used by Solaris does not correctly initialize the
  // buffer pointer in the constructor for 'basic_streambuf'. As a result, on
  // Solaris, we need to perform two-phase initialization using the
  // 'basic_osyncstream::init' function instead.
  Base(&d_syncbuf),
#endif
  d_syncbuf(wrapped, allocator)
{
#ifdef BSLS_LIBRARYFEATURES_STDCPP_LIBCSTD
    this->init(&d_syncbuf);
#endif
}

template <class CHAR, class TRAITS, class ALLOCATOR>
basic_osyncstream<CHAR,TRAITS,ALLOCATOR>::basic_osyncstream(
                                    std::basic_ostream<CHAR,TRAITS>& stream,
                                    const ALLOCATOR&                 allocator)
:
#ifndef BSLS_LIBRARYFEATURES_STDCPP_LIBCSTD
  Base(&d_syncbuf),
#endif
  d_syncbuf(stream.rdbuf(), allocator)
{
#ifdef BSLS_LIBRARYFEATURES_STDCPP_LIBCSTD
    this->init(&d_syncbuf);
#endif
}

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_STREAM_MOVE
template <class CHAR, class TRAITS, class ALLOCATOR>
basic_osyncstream<CHAR,TRAITS,ALLOCATOR>::basic_osyncstream(
                            basic_osyncstream&& original) BSLS_KEYWORD_NOEXCEPT
: Base(std::move(original))
, d_syncbuf(std::move(original.d_syncbuf))
{
    this->set_rdbuf(&d_syncbuf);
}

template <class CHAR, class TRAITS, class ALLOCATOR>
basic_osyncstream<CHAR,TRAITS,ALLOCATOR>::basic_osyncstream(
                           basic_osyncstream&& original,
                           const ALLOCATOR&    allocator) BSLS_KEYWORD_NOEXCEPT
: Base(std::move(original))
, d_syncbuf(std::move(original.d_syncbuf), allocator)
{
    this->set_rdbuf(&d_syncbuf);
}
#endif

// MANIPULATORS
template <class CHAR, class TRAITS, class ALLOCATOR>
void basic_osyncstream<CHAR,TRAITS,ALLOCATOR>::emit()
{
    typename Base::sentry ok(*this);
    if (!ok) {
        this->setstate(ios_base::badbit);
    }
    else {
        if (!d_syncbuf.emit()) {
            this->setstate(ios_base::badbit);
        }
    }
}

// ACCESSORS
template <class CHAR, class TRAITS, class ALLOCATOR>
inline
ALLOCATOR basic_osyncstream<CHAR,TRAITS,ALLOCATOR>::get_allocator() const
                                                          BSLS_KEYWORD_NOEXCEPT
{
    return d_syncbuf.get_allocator();
}

template <class CHAR, class TRAITS, class ALLOCATOR>
inline
typename basic_osyncstream<CHAR,TRAITS,ALLOCATOR>::streambuf_type *
basic_osyncstream<CHAR,TRAITS,ALLOCATOR>::get_wrapped() const
                                                          BSLS_KEYWORD_NOEXCEPT
{
    return d_syncbuf.get_wrapped();
}

template <class CHAR, class TRAITS, class ALLOCATOR>
inline
typename basic_osyncstream<CHAR,TRAITS,ALLOCATOR>::syncbuf_type *
basic_osyncstream<CHAR,TRAITS,ALLOCATOR>::rdbuf() const BSLS_KEYWORD_NOEXCEPT
{
    return const_cast<syncbuf_type*>(&d_syncbuf);
}

}  // close namespace bsl

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
