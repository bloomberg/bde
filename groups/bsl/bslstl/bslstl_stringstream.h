// bslstl_stringstream.h                                              -*-C++-*-
#ifndef INCLUDED_BSLSTL_STRINGSTREAM
#define INCLUDED_BSLSTL_STRINGSTREAM

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a C++03-compatible `stringstream` class.
//
//@CLASSES:
//  bsl::stringstream: C++03-compatible `stringstream` class
//
//@CANONICAL_HEADER: bsl_sstream.h
//
//@SEE_ALSO:
//
//@DESCRIPTION: This component is for internal use only.  Please include
// `<bsl_sstream.h>` instead.
//
// This component defines a class template, `bsl::basic_stringstream`,
// implementing a standard stream that provides a constructor and manipulator
// (`str`) that allow clients to directly set the internal sequence of
// characters that is accessed (or modified) by the stream, as well as an
// accessor (`str`) for obtaining a string having the same sequence of
// characters to which the stream currently refers (see 27.8.5 [stringstream]
// of the C++11 standard).  This component also defines two standard aliases,
// `bsl::stringstream` and `bsl::wstringstream`, that refer to specializations
// of the `bsl::basic_stringstream` template for `char` and `wchar_t` types,
// respectively.  The `bsl::basic_stringstream` template has three parameters,
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
// `basic_stringstream` template supports allocators meeting the requirements
// of the C++11 standard, 17.6.3.5 [allocator.requirements]; in addition, it
// supports scoped-allocators derived from the `bslma::Allocator` memory
// allocation protocol.  Clients intending to use `bslma`-style allocators
// should use `bsl::allocator`, which provides a C++11 standard-compatible
// adapter for a `bslma::Allocator` object.  Note that the standard aliases
// `bsl::stringstream` and `bsl::wstringstream` both use `bsl::allocator`.
//
///`bslma`-Style Allocators
/// - - - - - - - - - - - -
// If the type supplied for the `ALLOCATOR` template parameter of an
// `stringstream` instantiation is `bsl::allocator`, then objects of that
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
///Example 1: Basic Input and Output Operations
/// - - - - - - - - - - - - - - - - - - - - - -
// The following example demonstrates the use of `bsl::stringstream` to read
// and write data of various types to a `bsl::string` object.
//
// Suppose we want to implement a simplified converter between a pair of
// generic types, `TYPE1` and `TYPE2`.  We use `bsl::stringstream` to implement
// the `lexicalCast` function.  We write the data of type `TYPE1` into the
// stream with `operator<<` and then read it back as the data of `TYPE2` with
// `operator>>`:
// ```
// template <class TYPE2, class TYPE1>
// TYPE2 lexicalCast(const TYPE1& what)
// {
//     bsl::stringstream converter;
//     converter << what;
//
//     TYPE2 val;
//     converter >> val;
//     return val;
// }
// ```
// Finally, we verify that the `lexicalCast` function works on some simple test
// cases:
// ```
// assert(lexicalCast<int>("1234") == 1234);
//
// assert(lexicalCast<short>("-5") == -5);
//
// assert(lexicalCast<bsl::string>("abc") == "abc");
//
// assert(lexicalCast<bsl::string>(1234) == "1234");
//
// assert(lexicalCast<short>(-5) == -5);
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
#include <bslstl_stringbuf.h>
#include <bslstl_stringview.h>

#include <ios>

#include <iostream>

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_STREAM_MOVE
# include <utility>
#endif

namespace bsl {

                          // ========================
                          // class basic_stringstream
                          // ========================

/// This class implements a standard stream providing operations using
/// `bsl::basic_string` for modifying or accessing the sequence of
/// characters read from, or written to, the stream.
template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
class basic_stringstream
    : private StringBufContainer<CHAR_TYPE, CHAR_TRAITS, ALLOCATOR>
    , public std::basic_iostream<CHAR_TYPE, CHAR_TRAITS> {

  private:
    // PRIVATE TYPES
    typedef basic_stringbuf<CHAR_TYPE, CHAR_TRAITS, ALLOCATOR>   StreamBufType;
    typedef StringBufContainer<CHAR_TYPE, CHAR_TRAITS, ALLOCATOR>
                                                                 BaseType;
    typedef bsl::basic_string<CHAR_TYPE, CHAR_TRAITS, ALLOCATOR> StringType;
    typedef bsl::basic_string_view<CHAR_TYPE, CHAR_TRAITS>       ViewType;
    typedef std::basic_iostream<CHAR_TYPE, CHAR_TRAITS>          BaseStream;
    typedef std::ios_base                                        ios_base;

    typedef BloombergLP::bslmf::MovableRefUtil                   MoveUtil;

  private:
    // NOT IMPLEMENTED
    basic_stringstream(const basic_stringstream&);             // = delete
    basic_stringstream& operator=(const basic_stringstream&);  // = delete

  public:
    // TYPES
    typedef CHAR_TYPE                       char_type;
    typedef CHAR_TRAITS                     traits_type;
    typedef ALLOCATOR                       allocator_type;
    typedef typename traits_type::int_type  int_type;
    typedef typename traits_type::off_type  off_type;
    typedef typename traits_type::pos_type  pos_type;

    // CREATORS

    /// Create a `basic_stringstream` object.  Optionally specify a
    /// `modeBitMask` indicating whether the underlying stream-buffer may be
    /// read from, written to, or both (`rdbuf` is created using
    /// `modeBitMask`).  If `modeBitMask` is not supplied, `rdbuf` will be
    /// created using `ios_base::in | ios_base::out`.  Optionally specify an
    /// `initialString` indicating the initial sequence of characters that
    /// this stream may access or manipulate.  If `initialString` is not
    /// supplied, the initial sequence of characters will be empty.
    /// Optionally specify the `allocator` used to supply memory.  If
    /// `allocator` is not supplied, a default-constructed object of the
    /// (template parameter) `ALLOCATOR` type is used.  If the `ALLOCATOR`
    /// argument is of type `bsl::allocator` (the default), then
    /// `allocator`, if supplied, shall be convertible to
    /// `bslma::Allocator *`.  If the `ALLOCATOR` argument is of type
    /// `bsl::allocator` and `allocator` is not supplied, the currently
    /// installed default allocator will be used to supply memory.  If
    /// `initialString` is passed by `MovableRef`, it is left in a valid but
    /// unspecified state.
    explicit
    basic_stringstream(const allocator_type& allocator = allocator_type());
    explicit
    basic_stringstream(ios_base::openmode    modeBitMask,
                       const allocator_type& allocator = allocator_type());
    explicit
    basic_stringstream(const StringType&     initialString,
                       const allocator_type& allocator = allocator_type());
    basic_stringstream(const StringType&     initialString,
                       ios_base::openmode    modeBitMask,
                       const allocator_type& allocator = allocator_type());

    /// Create a `basic_stringstream` object.  Use the specified
    /// `initialString` indicating the initial sequence of characters that
    /// this buffer will access or manipulate.  Optionally specify a
    /// `modeBitMask` indicating whether this buffer may be read from,
    /// written to, or both.  If `modeBitMask` is not supplied, this buffer
    /// is created with `ios_base::in | ios_base::out`.  Optionally specify
    /// the `allocator` used to supply memory.  If `allocator` is not
    /// supplied, the allocator in `initialString` is used.  `initialString`
    /// is left in a valid but unspecified state.
    explicit
    basic_stringstream(
                     BloombergLP::bslmf::MovableRef<StringType> initialString);
    basic_stringstream(
                      BloombergLP::bslmf::MovableRef<StringType> initialString,
                      const allocator_type&                      allocator);
    basic_stringstream(
                      BloombergLP::bslmf::MovableRef<StringType> initialString,
                      ios_base::openmode                         modeBitMask);
    basic_stringstream(
                      BloombergLP::bslmf::MovableRef<StringType> initialString,
                      ios_base::openmode                         modeBitMask,
                      const allocator_type&                      allocator);

    /// Create a `basic_stringstream` object.  Use the specified
    /// `initialString` indicating the initial sequence of characters that
    /// this stream will access or manipulate.  `rdbuf` is created using
    /// `ios_base::in | ios_base::out`.  Optionally specify the `allocator`
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
    basic_stringstream(
        const bsl::basic_string<CHAR_TYPE, CHAR_TRAITS, SALLOC>&
                                                  initialString,
        const allocator_type&                     allocator = allocator_type(),
        typename bsl::enable_if<
                    !bsl::is_same<ALLOCATOR, SALLOC>::value, void *>::type = 0)
    : BaseType(initialString.begin(),
               initialString.end(),
               ios_base::in | ios_base::out,
               allocator)
    , BaseStream(BaseType::rdbuf())
    {
    }

    /// Create a `basic_stringstream` object.  Use the specified
    /// `initialString` indicating the initial sequence of characters that
    /// this stream will access or manipulate.  Use the specified
    /// `modeBitMask` to indicate whether this stream may be read from,
    /// written to, or both.  Optionally specify the `allocator` used to
    /// supply memory.  If `allocator` is not supplied, a
    /// default-constructed object of the (template parameter) `ALLOCATOR`
    /// type is used.  If the `ALLOCATOR` argument is of type
    /// `bsl::allocator` (the default), then `allocator`, if supplied, shall
    /// be convertible to `bslma::Allocator *`.  If the `ALLOCATOR` argument
    /// is of type `bsl::allocator` and `allocator` is not supplied, the
    /// currently installed default allocator will be used to supply memory.
    ///
    /// Note: implemented inline due to Sun CC compilation error.
    template <class SALLOC>
    basic_stringstream(
        const bsl::basic_string<CHAR_TYPE, CHAR_TRAITS, SALLOC>&
                                                  initialString,
        ios_base::openmode                        modeBitMask,
        const allocator_type&                     allocator = allocator_type(),
        typename bsl::enable_if<
                    !bsl::is_same<ALLOCATOR, SALLOC>::value, void *>::type = 0)
    : BaseType(initialString.begin(),
               initialString.end(),
               modeBitMask,
               allocator)
    , BaseStream(BaseType::rdbuf())
    {
    }


#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_STREAM_MOVE
    /// Create a `basic_stringstream` object having the same value as the
    /// specified `original` object by moving the contents of `original` to
    /// the newly-created object.  `original` is left in a valid but
    /// unspecified state.
    basic_stringstream(basic_stringstream&& original);
#endif

    //! ~basic_stringstream() = default;
        // Destroy this object.

    // MANIPULATORS
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_STREAM_MOVE
    /// Assign to this object the value of the specified `rhs`, and return a
    /// reference providing modifiable access to this object.  The contents
    /// of `rhs` are move-assigned to this object.  `rhs` is left in a valid
    /// but unspecified state.
    basic_stringstream& operator=(basic_stringstream&& rhs);
#endif

    /// Reset the internally buffered sequence of characters maintained by
    /// this stream to the specified `value`.  If `value` is passed by
    /// `MovableRef`, it is left in a valid but unspecified state.
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

    // ACCESSORS

    /// Return an address providing modifiable access to the
    /// `basic_stringbuf` object that is internally used by this stream
    /// object to buffer unformatted characters.
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

// STANDARD TYPEDEFS
typedef basic_stringstream<char, char_traits<char>, allocator<char> >
                                                                  stringstream;
typedef basic_stringstream<wchar_t, char_traits<wchar_t>, allocator<wchar_t> >
                                                                 wstringstream;

}  // close namespace bsl

// TYPE TRAITS
namespace BloombergLP {
namespace bslma {

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
struct UsesBslmaAllocator<
        bsl::basic_stringstream<CHAR_TYPE, CHAR_TRAITS, ALLOCATOR> >
    : bsl::true_type
{};

}  // close namespace bslma
}  // close enterprise namespace

namespace bsl {

// ============================================================================
//                       TEMPLATE FUNCTION DEFINITIONS
// ============================================================================

                          // ------------------------
                          // class basic_stringstream
                          // ------------------------

// CREATORS
template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
inline
basic_stringstream<CHAR_TYPE, CHAR_TRAITS, ALLOCATOR>::
basic_stringstream(const allocator_type& allocator)
: BaseType(ios_base::in | ios_base::out, allocator)
, BaseStream(BaseType::rdbuf())
{
}

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
inline
basic_stringstream<CHAR_TYPE, CHAR_TRAITS, ALLOCATOR>::
    basic_stringstream(ios_base::openmode    modeBitMask,
                       const allocator_type& allocator)
: BaseType(modeBitMask, allocator)
, BaseStream(BaseType::rdbuf())
{
}

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
inline
basic_stringstream<CHAR_TYPE, CHAR_TRAITS, ALLOCATOR>::
basic_stringstream(const StringType&     initialString,
                   const allocator_type& allocator)
: BaseType(initialString, ios_base::in | ios_base::out, allocator)
, BaseStream(BaseType::rdbuf())
{
}

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
inline
basic_stringstream<CHAR_TYPE, CHAR_TRAITS, ALLOCATOR>::
basic_stringstream(const StringType&     initialString,
                   ios_base::openmode    modeBitMask,
                   const allocator_type& allocator)
: BaseType(initialString, modeBitMask, allocator)
, BaseStream(BaseType::rdbuf())
{
}

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
inline
basic_stringstream<CHAR_TYPE, CHAR_TRAITS, ALLOCATOR>::
basic_stringstream(BloombergLP::bslmf::MovableRef<StringType> initialString)
: BaseType(MoveUtil::move(initialString), ios_base::in | ios_base::out)
, BaseStream(BaseType::rdbuf())
{
}

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
inline
basic_stringstream<CHAR_TYPE, CHAR_TRAITS, ALLOCATOR>::
basic_stringstream(BloombergLP::bslmf::MovableRef<StringType> initialString,
                   const allocator_type&                      allocator)
: BaseType(MoveUtil::move(initialString),
           ios_base::in | ios_base::out,
           allocator)
, BaseStream(BaseType::rdbuf())
{
}

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
inline
basic_stringstream<CHAR_TYPE, CHAR_TRAITS, ALLOCATOR>::
basic_stringstream(BloombergLP::bslmf::MovableRef<StringType> initialString,
                   ios_base::openmode                         modeBitMask)
: BaseType(MoveUtil::move(initialString), modeBitMask)
, BaseStream(BaseType::rdbuf())
{
}

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
inline
basic_stringstream<CHAR_TYPE, CHAR_TRAITS, ALLOCATOR>::
basic_stringstream(BloombergLP::bslmf::MovableRef<StringType> initialString,
                   ios_base::openmode                         modeBitMask,
                   const allocator_type&                      allocator)
: BaseType(MoveUtil::move(initialString), modeBitMask, allocator)
, BaseStream(BaseType::rdbuf())
{
}

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_STREAM_MOVE
template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
inline
basic_stringstream<CHAR_TYPE, CHAR_TRAITS, ALLOCATOR>::
basic_stringstream(basic_stringstream&& original)
: BaseType(std::move(original))
, BaseStream(std::move(original))
{
    BaseStream::set_rdbuf(BaseType::rdbuf());
}
#endif

// MANIPULATORS
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_STREAM_MOVE
template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
inline
basic_stringstream<CHAR_TYPE, CHAR_TRAITS, ALLOCATOR>&
basic_stringstream<CHAR_TYPE, CHAR_TRAITS, ALLOCATOR>::
operator=(basic_stringstream&& rhs)
{
    this->BaseType::operator=(std::move(rhs));
    this->BaseStream::operator=(std::move(rhs));

    return *this;
}
#endif

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
inline
void basic_stringstream<CHAR_TYPE, CHAR_TRAITS, ALLOCATOR>::str(
                                                       const StringType& value)
{
    this->rdbuf()->str(value);
}

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
inline
void basic_stringstream<CHAR_TYPE, CHAR_TRAITS, ALLOCATOR>::str(
                              BloombergLP::bslmf::MovableRef<StringType> value)
{
    this->rdbuf()->str(MoveUtil::move(value));
}

#ifdef BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS
template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
inline
typename basic_stringstream<CHAR_TYPE, CHAR_TRAITS, ALLOCATOR>::StringType
basic_stringstream<CHAR_TYPE, CHAR_TRAITS, ALLOCATOR>::str() &&
{
    return std::move(*this->rdbuf()).str();
}
#endif

// ACCESSORS
template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
inline
typename basic_stringstream<CHAR_TYPE, CHAR_TRAITS, ALLOCATOR>::StreamBufType *
basic_stringstream<CHAR_TYPE, CHAR_TRAITS, ALLOCATOR>::rdbuf() const
{
    return this->BaseType::rdbuf();
}

#ifdef BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS
template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
inline
typename basic_stringstream<CHAR_TYPE, CHAR_TRAITS, ALLOCATOR>::StringType
basic_stringstream<CHAR_TYPE, CHAR_TRAITS, ALLOCATOR>::str() const &
{
    return this->rdbuf()->str();
}
#else
template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
inline
typename basic_stringstream<CHAR_TYPE, CHAR_TRAITS, ALLOCATOR>::StringType
basic_stringstream<CHAR_TYPE, CHAR_TRAITS, ALLOCATOR>::str() const
{
    return this->rdbuf()->str();
}
#endif

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
inline
typename basic_stringstream<CHAR_TYPE, CHAR_TRAITS, ALLOCATOR>::ViewType
basic_stringstream<CHAR_TYPE, CHAR_TRAITS, ALLOCATOR>::view()
                                                    const BSLS_KEYWORD_NOEXCEPT
{
    return this->rdbuf()->view();
}

}  // close namespace bsl

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
