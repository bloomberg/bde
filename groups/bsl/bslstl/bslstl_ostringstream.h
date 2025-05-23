// bslstl_ostringstream.h                                             -*-C++-*-
#ifndef INCLUDED_BSLSTL_OSTRINGSTREAM
#define INCLUDED_BSLSTL_OSTRINGSTREAM

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a C++03-compatible `ostringstream` class.
//
//@CLASSES:
//  bsl::ostringstream: C++03-compatible `ostringstream` class
//
//@CANONICAL_HEADER: bsl_sstream.h
//
//@SEE_ALSO:
//
//@DESCRIPTION: This component is for internal use only.  Please include
// `<bsl_sstream.h>` instead.
//
// This component defines a class template, `bsl::basic_ostringstream`,
// implementing a standard output stream that provides a method for obtaining a
// `bsl::basic_string`, which contains the characters that have been written to
// the stream (see 27.8.4 [ostringstream] of the C++11 standard).  This
// component also defines two standard aliases, `bsl::ostringstream` and
// `bsl::wostringstream`, that refer to specializations of the
// `bsl::basic_ostringstream` template for `char` and `wchar_t` types,
// respectively.  The `bsl::basic_ostringstream` template has three parameters,
// `CHAR_TYPE`, `CHAR_TRAITS`, and `ALLOCATOR`.  The `CHAR_TYPE` and
// `CHAR_TRAITS` parameters respectively define the character type for the
// string-stream and a type providing a set of operations the string-stream
// will use to manipulate characters of that type, which must meet the
// character traits requirements defined by the C++11 standard, 21.2
// [char.traits].  The `ALLOCATOR` template parameter is described in the
// "Memory Allocation" section below.
//
///Memory Allocation
///-----------------
// The type supplied as a string-stream's `ALLOCATOR` template parameter
// determines how that string-stream will allocate memory.  The
// `basic_ostringstream` template supports allocators meeting the requirements
// of the C++11 standard, 17.6.3.5 [allocator.requirements]; in addition, it
// supports scoped-allocators derived from the `bslma::Allocator` memory
// allocation protocol.  Clients intending to use `bslma`-style allocators
// should use `bsl::allocator`, which provides a C++11 standard-compatible
// adapter for a `bslma::Allocator` object.  Note that the standard aliases
// `bsl::ostringstream` and `bsl::wostringstream` both use `bsl::allocator`.
//
///`bslma`-Style Allocators
/// - - - - - - - - - - - -
// If the type supplied for the `ALLOCATOR` template parameter of an
// `ostringstream` instantiation is `bsl::allocator`, then objects of that
// string-stream type will conform to the standard behavior of a
// `bslma`-allocator-enabled type.  Such a string-stream accepts an optional
// `bslma::Allocator` argument at construction.  If the address of a
// `bslma::Allocator` object is explicitly supplied at construction, it will be
// used to supply memory for the string-stream throughout its lifetime;
// otherwise, the string-stream will use the default allocator installed at the
// time of the string-stream's construction (see `bslma_default`).
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Basic Output Operations
/// - - - - - - - - - - - - - - - - -
// The following example demonstrates the use of `bsl::ostringstream` to write
// data of various types into a `bsl::string` object.
//
// Suppose we want to implement a simplified converter from a generic type,
// `TYPE`, to `bsl::string`.  We use `bsl::ostringstream` to implement the
// `toString` function.  We write the data into the stream with `operator<<`
// and then use the `str` method to retrieve the resulting string from the
// stream:
// ```
// template <class TYPE>
// bsl::string toString(const TYPE& what)
// {
//     bsl::ostringstream out;
//     out << what;
//     return out.str();
// }
// ```
// Finally, we verify that our `toString` function works on some simple test
// cases:
// ```
// assert(toString(1234) == "1234");
//
// assert(toString<short>(-5) == "-5");
//
// assert(toString("abc") == "abc");
// ```

#include <bslscm_version.h>

#include <bslma_isstdallocator.h>
#include <bslma_usesbslmaallocator.h>

#include <bslmf_enableif.h>
#include <bslmf_issame.h>
#include <bslmf_movableref.h>

#include <bsls_compilerfeatures.h>
#include <bsls_keyword.h>
#include <bsls_libraryfeatures.h>
#include <bsls_platform.h>

#include <bslstl_string.h>
#include <bslstl_stringview.h>
#include <bslstl_stringbuf.h>

#include <ios>

#include <ostream>

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_STREAM_MOVE
# include <utility>
#endif

namespace bsl {

                          // =========================
                          // class basic_ostringstream
                          // =========================

/// This class implements a standard output stream that provides an accessor
/// for obtaining a `bsl::basic_string` containing the sequence of
/// characters that have been written to the stream.
template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
class basic_ostringstream
    : private StringBufContainer<CHAR_TYPE, CHAR_TRAITS, ALLOCATOR>
    , public std::basic_ostream<CHAR_TYPE, CHAR_TRAITS> {

  private:
    // PRIVATE TYPES
    typedef basic_stringbuf<CHAR_TYPE, CHAR_TRAITS, ALLOCATOR>   StreamBufType;
    typedef StringBufContainer<CHAR_TYPE, CHAR_TRAITS, ALLOCATOR>
                                                                 BaseType;
    typedef bsl::basic_string<CHAR_TYPE, CHAR_TRAITS, ALLOCATOR> StringType;
    typedef bsl::basic_string_view<CHAR_TYPE, CHAR_TRAITS>       ViewType;
    typedef std::basic_ostream<CHAR_TYPE, CHAR_TRAITS>           BaseStream;
    typedef std::ios_base                                        ios_base;

    typedef BloombergLP::bslmf::MovableRefUtil                   MoveUtil;

  private:
    // NOT IMPLEMENTED
    basic_ostringstream(const basic_ostringstream&);             // = delete
    basic_ostringstream& operator=(const basic_ostringstream&);  // = delete

  public:
    // TYPES
    typedef CHAR_TYPE                       char_type;
    typedef CHAR_TRAITS                     traits_type;
    typedef ALLOCATOR                       allocator_type;
    typedef typename traits_type::int_type  int_type;
    typedef typename traits_type::off_type  off_type;
    typedef typename traits_type::pos_type  pos_type;

    // CREATORS

    /// Create a `basic_ostringstream` object.  Optionally specify a
    /// `modeBitMask` indicating whether the underlying stream-buffer may
    /// also be read from (`rdbuf` is created using
    /// `modeBitMask | ios_base::out`).  If `modeBitMask` is not supplied,
    /// `rdbuf` will be created using `ios_base::out`.  Optionally specify
    /// an `initialString` indicating the value that will be returned by a
    /// call to `str` prior to any streaming operations performed on this
    /// object.  If `initialString` is not supplied, `str` will initially
    /// return an empty string.  Optionally specify the `allocator` used to
    /// supply memory.  If `allocator` is not supplied, a
    /// default-constructed object of the (template parameter) `ALLOCATOR`
    /// type is used.  If the `ALLOCATOR` argument is of type
    /// `bsl::allocator` (the default), then `allocator`, if supplied,
    /// shall be convertible to `bslma::Allocator *`.  If the `ALLOCATOR`
    /// argument is of type `bsl::allocator` and `allocator` is not
    /// supplied, the currently installed default allocator will be used to
    /// supply memory.
    explicit
    basic_ostringstream(const allocator_type& allocator = allocator_type());
    explicit
    basic_ostringstream(ios_base::openmode    modeBitMask,
                        const allocator_type& allocator = allocator_type());
    explicit
    basic_ostringstream(const StringType&     initialString,
                        const allocator_type& allocator = allocator_type());
    basic_ostringstream(const StringType&     initialString,
                        ios_base::openmode    modeBitMask,
                        const allocator_type& allocator = allocator_type());

    /// Create a `basic_ostringstream` object.  Use the specified
    /// `initialString` indicating the value that will be returned by a call
    /// to `str` prior to any streaming operations performed on this object.
    /// Optionally specify a `modeBitMask` indicating whether the underlying
    /// stream-buffer may also be read from (`rdbuf` is created using
    /// `modeBitMask | ios_base::out`).  If `modeBitMask` is not supplied,
    /// `rdbuf` will be created using `ios_base::out`.  Optionally specify
    /// the `allocator` used to supply memory.  If `allocator` is not
    /// supplied, the allocator in `initialString` is used.  `initialString`
    /// is left in a valid but unspecified state.
    explicit
    basic_ostringstream(
                     BloombergLP::bslmf::MovableRef<StringType> initialString);
    basic_ostringstream(
                      BloombergLP::bslmf::MovableRef<StringType> initialString,
                      const allocator_type&                      allocator);
    basic_ostringstream(
                      BloombergLP::bslmf::MovableRef<StringType> initialString,
                      ios_base::openmode                         modeBitMask);
    basic_ostringstream(
                      BloombergLP::bslmf::MovableRef<StringType> initialString,
                      ios_base::openmode                         modeBitMask,
                      const allocator_type&                      allocator);

    /// Create a `basic_ostringstream` object.  Use the specified
    /// `initialString` as the value that will be returned by a call to
    /// `str` prior to any streaming operations performed on this object.
    /// `rdbuf` is created using `ios_base::out`.  Optionally specify the
    /// `allocator` used to supply memory.  If `allocator` is not supplied,
    /// a default-constructed object of the (template parameter) `ALLOCATOR`
    /// type is used.  If the `ALLOCATOR` argument is of type
    /// `bsl::allocator` (the default), then `allocator`, if supplied, shall
    /// be convertible to `bslma::Allocator *`.  If the `ALLOCATOR` argument
    /// is of type `bsl::allocator` and `allocator` is not supplied, the
    /// currently installed default allocator will be used to supply memory.
    ///
    /// Note: implemented inline due to Sun CC compilation error.
    template <class SALLOC>
    basic_ostringstream(
        const bsl::basic_string<CHAR_TYPE, CHAR_TRAITS, SALLOC>&
                                                  initialString,
        const allocator_type&                     allocator = allocator_type(),
        typename bsl::enable_if<
                    !bsl::is_same<ALLOCATOR, SALLOC>::value, void *>::type = 0)
    : BaseType(initialString.begin(),
               initialString.end(),
               ios_base::out,
               allocator)
    , BaseStream(BaseType::rdbuf())
    {
    }


    /// Create a `basic_ostringstream` object.  Use the specified
    /// `initialString` as the value that will be returned by a call to
    /// `str` prior to any streaming operations performed on this object.
    /// Use the specified `modeBitMask` to indicate whether the underlying
    /// stream-buffer may also be read from (`rdbuf` is created using
    /// `modeBitMask | ios_base::out`).  Optionally specify the `allocator`
    /// used to supply memory.  If `allocator` is not supplied, a
    /// default-constructed object of the (template parameter) `ALLOCATOR`
    /// type is used.  If the `ALLOCATOR` argument is of type
    /// `bsl::allocator` (the default), then `allocator`, if supplied, shall
    /// be convertible to `bslma::Allocator *`.  If the `ALLOCATOR` argument
    /// is of type `bsl::allocator` and `allocator` is not supplied, the
    /// currently installed default allocator will be used to supply memory.
    ///
    /// Note: implemented inline due to Sun CC compilation error.
    template <class SALLOC>
    basic_ostringstream(
        const bsl::basic_string<CHAR_TYPE, CHAR_TRAITS, SALLOC>&
                                                  initialString,
        ios_base::openmode                        modeBitMask,
        const allocator_type&                     allocator = allocator_type(),
        typename bsl::enable_if<
                    !bsl::is_same<ALLOCATOR, SALLOC>::value, void *>::type = 0)
    : BaseType(initialString.begin(),
               initialString.end(),
               modeBitMask | ios_base::out,
               allocator)
    , BaseStream(BaseType::rdbuf())
    {
    }

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_STREAM_MOVE
    /// Create a `basic_ostringstream` object having the same value as the
    /// specified `original` object by moving the contents of `original` to
    /// the newly-created object.  Optionally specify the `allocator` used
    /// to supply memory.  `original` is left in a valid but unspecified
    /// state.
    basic_ostringstream(basic_ostringstream&& original);
    basic_ostringstream(basic_ostringstream&& original,
                        const allocator_type& allocator);
#endif

    //! ~basic_ostringstream() = default;
        // Destroy this object.

    // MANIPULATORS
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_STREAM_MOVE
    /// Assign to this object the value of the specified `rhs`, and return a
    /// reference providing modifiable access to this object.  The contents
    /// of `rhs` are move-assigned to this object.  `rhs` is left in a valid
    /// but unspecified state.
    basic_ostringstream& operator=(basic_ostringstream&& rhs);
#endif

    /// Reset the internally buffered sequence of characters maintained by
    /// this stream object to the specified `value`.  If `value` is passed
    /// by `MovableRef`, it is left in a valid but unspecified state.
    ///
    /// Note: implemented inline due to Sun CC compilation error.
    void str(const StringType& value);
    void str(BloombergLP::bslmf::MovableRef<StringType> value);
    template <class SALLOC>
    typename
        bsl::enable_if<!bsl::is_same<ALLOCATOR, SALLOC>::value, void>::type
    str(const basic_string<CHAR_TYPE, CHAR_TRAITS, SALLOC>& value)
    {
        return this->rdbuf()->str(value);
    }

#ifdef BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS
    /// Return the internally buffered sequence of characters maintained by
    /// this stream, leaving the stream empty.
    StringType str() &&;
#endif

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
    /// Efficiently exchange the value of this object with the value of the
    /// specified `other` object.  This method provides the no-throw
    /// exception-safety guarantee if `*this` and `other` allocators compare
    /// equal.  The behavior is undefined unless either `*this` and `other`
    /// allocators compare equal or `propagate_on_container_swap` is `true`.
    /// Note that this function is only available for C++11 (and later)
    /// language standards because it requires that `swap` be provided on
    /// the (platform supplied) base class for this type.
    void swap(basic_ostringstream& other);
#endif  // BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY

    // ACCESSORS

    /// Return the allocator used by the underlying buffer to supply memory.
    allocator_type get_allocator() const BSLS_KEYWORD_NOEXCEPT;

    /// Return an address providing modifiable access to the
    /// `basic_stringbuf` object that is internally used by this string
    /// stream to buffer unformatted characters.
    StreamBufType *rdbuf() const;

#ifdef BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS
    StringType str() const &;
#else
    StringType str() const;
#endif
        // Return the internally buffered sequence of characters maintained by
        // this stream object.

#ifndef BSLS_PLATFORM_CMP_SUN
    // To be enabled once DRQS 168075157 is resolved

    /// Return a copy of the internally buffered sequence of characters
    /// maintained by this stream object in a `basic_string` that uses the
    /// specified `allocator`.
    ///
    /// Note: implemented inline due to Sun CC compilation error.
    template <class SALLOC>
    typename bsl::enable_if<
        bsl::IsStdAllocator<SALLOC>::value,
        basic_string<CHAR_TYPE, CHAR_TRAITS, SALLOC> >::type
    str(const SALLOC& allocator) const
    {
        return this->rdbuf()->str(allocator);
    }
#endif

    /// Return a view of the internally buffered sequence of characters
    /// maintained by this stream object.
    ViewType view() const BSLS_KEYWORD_NOEXCEPT;
};

// FREE FUNCTIONS
#if defined(BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY)                  \
 && defined(BSLS_LIBRARYFEATURES_HAS_CPP11_STREAM_MOVE)
/// Efficiently exchange the values of the specified `a` and `b` objects.
/// This method provides the no-throw exception-safety guarantee if `a` and
/// `b` allocators compare equal.  Note that this function is only available
/// for C++11 (and later) language standards.
template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
void swap(basic_ostringstream<CHAR_TYPE, CHAR_TRAITS, ALLOCATOR>& a,
          basic_ostringstream<CHAR_TYPE, CHAR_TRAITS, ALLOCATOR>& b);
#endif  // BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY

// STANDARD TYPEDEFS
typedef basic_ostringstream<char, char_traits<char>, allocator<char> >
                                                                 ostringstream;
typedef basic_ostringstream<wchar_t, char_traits<wchar_t>, allocator<wchar_t> >
                                                                wostringstream;

}  // close namespace bsl

// TYPE TRAITS
namespace BloombergLP {
namespace bslma {

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
struct UsesBslmaAllocator<
        bsl::basic_ostringstream<CHAR_TYPE, CHAR_TRAITS, ALLOCATOR> >
    : bsl::true_type
{};

}  // close namespace bslma
}  // close enterprise namespace

namespace bsl {

// ============================================================================
//                       TEMPLATE FUNCTION DEFINITIONS
// ============================================================================

                          // -------------------------
                          // class basic_ostringstream
                          // -------------------------

// CREATORS
template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
inline
basic_ostringstream<CHAR_TYPE, CHAR_TRAITS, ALLOCATOR>::
basic_ostringstream(const allocator_type& allocator)
: BaseType(ios_base::out, allocator)
, BaseStream(BaseType::rdbuf())
{
}

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
inline
basic_ostringstream<CHAR_TYPE, CHAR_TRAITS, ALLOCATOR>::
basic_ostringstream(ios_base::openmode    modeBitMask,
                    const allocator_type& allocator)
: BaseType(modeBitMask | ios_base::out, allocator)
, BaseStream(BaseType::rdbuf())
{
}

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
inline
basic_ostringstream<CHAR_TYPE, CHAR_TRAITS, ALLOCATOR>::
basic_ostringstream(const StringType&     initialString,
                    const allocator_type& allocator)
: BaseType(initialString, ios_base::out, allocator)
, BaseStream(BaseType::rdbuf())
{
}

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
inline
basic_ostringstream<CHAR_TYPE, CHAR_TRAITS, ALLOCATOR>::
basic_ostringstream(const StringType&     initialString,
                    ios_base::openmode    modeBitMask,
                    const allocator_type& allocator)
: BaseType(initialString, modeBitMask | ios_base::out, allocator)
, BaseStream(BaseType::rdbuf())
{
}

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
inline
basic_ostringstream<CHAR_TYPE, CHAR_TRAITS, ALLOCATOR>::
basic_ostringstream(BloombergLP::bslmf::MovableRef<StringType> initialString)
: BaseType(MoveUtil::move(initialString), ios_base::out)
, BaseStream(BaseType::rdbuf())
{
}

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
inline
basic_ostringstream<CHAR_TYPE, CHAR_TRAITS, ALLOCATOR>::
basic_ostringstream(BloombergLP::bslmf::MovableRef<StringType> initialString,
                    const allocator_type&                      allocator)
: BaseType(MoveUtil::move(initialString), ios_base::out, allocator)
, BaseStream(BaseType::rdbuf())
{
}

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
inline
basic_ostringstream<CHAR_TYPE, CHAR_TRAITS, ALLOCATOR>::
basic_ostringstream(BloombergLP::bslmf::MovableRef<StringType> initialString,
                    ios_base::openmode                         modeBitMask)
: BaseType(MoveUtil::move(initialString), modeBitMask | ios_base::out)
, BaseStream(BaseType::rdbuf())
{
}

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
inline
basic_ostringstream<CHAR_TYPE, CHAR_TRAITS, ALLOCATOR>::
basic_ostringstream(BloombergLP::bslmf::MovableRef<StringType> initialString,
                    ios_base::openmode                         modeBitMask,
                    const allocator_type&                      allocator)
: BaseType(MoveUtil::move(initialString),
           modeBitMask | ios_base::out,
           allocator)
, BaseStream(BaseType::rdbuf())
{
}

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_STREAM_MOVE
template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
inline
basic_ostringstream<CHAR_TYPE, CHAR_TRAITS, ALLOCATOR>::
basic_ostringstream(basic_ostringstream&& original)
: BaseType(std::move(original))
, BaseStream(std::move(original))
{
    BaseStream::set_rdbuf(BaseType::rdbuf());
}

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
inline
basic_ostringstream<CHAR_TYPE, CHAR_TRAITS, ALLOCATOR>::
basic_ostringstream(basic_ostringstream&& original,
                    const allocator_type& allocator)
: BaseType(std::move(original), allocator)
, BaseStream(std::move(original))
{
    BaseStream::set_rdbuf(BaseType::rdbuf());
}
#endif

// MANIPULATORS
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_STREAM_MOVE
template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
inline
basic_ostringstream<CHAR_TYPE, CHAR_TRAITS, ALLOCATOR>&
basic_ostringstream<CHAR_TYPE, CHAR_TRAITS, ALLOCATOR>::
operator=(basic_ostringstream&& rhs)
{
    this->BaseType::operator=(std::move(rhs));
    this->BaseStream::operator=(std::move(rhs));

    return *this;
}
#endif

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
inline
void basic_ostringstream<CHAR_TYPE, CHAR_TRAITS, ALLOCATOR>::str(
                                                       const StringType& value)
{
    this->rdbuf()->str(value);
}

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
inline
void basic_ostringstream<CHAR_TYPE, CHAR_TRAITS, ALLOCATOR>::str(
                              BloombergLP::bslmf::MovableRef<StringType> value)
{
    this->rdbuf()->str(MoveUtil::move(value));
}

#ifdef BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS
template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
inline
typename basic_ostringstream<CHAR_TYPE, CHAR_TRAITS, ALLOCATOR>::StringType
basic_ostringstream<CHAR_TYPE, CHAR_TRAITS, ALLOCATOR>::str() &&
{
    return std::move(*this->rdbuf()).str();
}
#endif

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
inline
void basic_ostringstream<CHAR_TYPE, CHAR_TRAITS, ALLOCATOR>::swap(
                                                    basic_ostringstream& other)
{
#ifdef BSLS_PLATFORM_CMP_GNU
# pragma GCC diagnostic push
# pragma GCC diagnostic ignored "-Wstringop-overflow"
#endif
    BSLS_ASSERT(
           bsl::allocator_traits<ALLOCATOR>::propagate_on_container_swap::value
        || get_allocator() == other.get_allocator());
    this->BaseType::swap(other);
    this->BaseStream::swap(other);
#ifdef BSLS_PLATFORM_CMP_GNU
# pragma GCC diagnostic pop
#endif
}
#endif  // BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY

// ACCESSORS
template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
inline
typename basic_ostringstream<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::allocator_type
basic_ostringstream<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::get_allocator() const
                                                          BSLS_KEYWORD_NOEXCEPT
{
    return rdbuf()->get_allocator();
}

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
inline
typename
       basic_ostringstream<CHAR_TYPE, CHAR_TRAITS, ALLOCATOR>::StreamBufType *
basic_ostringstream<CHAR_TYPE, CHAR_TRAITS, ALLOCATOR>::rdbuf() const
{
    return this->BaseType::rdbuf();
}

#ifdef BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS
template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
inline
typename basic_ostringstream<CHAR_TYPE, CHAR_TRAITS, ALLOCATOR>::StringType
basic_ostringstream<CHAR_TYPE, CHAR_TRAITS, ALLOCATOR>::str() const &
{
    return this->rdbuf()->str();
}
#else
template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
inline
typename basic_ostringstream<CHAR_TYPE, CHAR_TRAITS, ALLOCATOR>::StringType
basic_ostringstream<CHAR_TYPE, CHAR_TRAITS, ALLOCATOR>::str() const
{
    return this->rdbuf()->str();
}
#endif

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
inline
typename basic_ostringstream<CHAR_TYPE, CHAR_TRAITS, ALLOCATOR>::ViewType
basic_ostringstream<CHAR_TYPE, CHAR_TRAITS, ALLOCATOR>::view()
                                                    const BSLS_KEYWORD_NOEXCEPT
{
    return this->rdbuf()->view();
}

}  // close namespace bsl

// FREE FUNCTIONS
#if defined(BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY)                  \
 && defined(BSLS_LIBRARYFEATURES_HAS_CPP11_STREAM_MOVE)
template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
void bsl::swap(basic_ostringstream<CHAR_TYPE, CHAR_TRAITS, ALLOCATOR>& a,
               basic_ostringstream<CHAR_TYPE, CHAR_TRAITS, ALLOCATOR>& b)
{
    typedef BloombergLP::bslmf::MovableRefUtil MoveUtil;

    if (a.get_allocator() == b.get_allocator()
    ||  bsl::allocator_traits<ALLOCATOR>::propagate_on_container_swap::value) {
        a.swap(b);
    }
    else {
        basic_ostringstream<CHAR_TYPE, CHAR_TRAITS, ALLOCATOR> aCopy(
                                                            MoveUtil::move(a),
                                                            b.get_allocator());
        basic_ostringstream<CHAR_TYPE, CHAR_TRAITS, ALLOCATOR> bCopy(
                                                            MoveUtil::move(b),
                                                            a.get_allocator());
        swap(a, bCopy);
        swap(b, aCopy);
    }
}
#endif

#endif

// ----------------------------------------------------------------------------
// Copyright 2013 Bloomberg Finance L.P.
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
