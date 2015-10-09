// bslstl_istringstream.h                                             -*-C++-*-
#ifndef INCLUDED_BSLSTL_ISTRINGSTREAM
#define INCLUDED_BSLSTL_ISTRINGSTREAM

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a C++03-compatible 'istringstream' class.
//
//@CLASSES:
//  bsl::istringstream: C++03-compatible 'istringstream' class
//
//@SEE_ALSO:
//
//@DESCRIPTION: This component is for internal use only.  Please include
// '<bsl_sstream.h>' instead.
//
// This component defines a class template, 'bsl::basic_istringstream',
// implementing a standard input stream that provides a constructor (as well as
// a manipulator) allowing clients to set the sequence of characters
// from which input is streamed to a supplied 'bsl::basic_string' (see
// 27.8.3 [istringstream] of the C++11 standard).  This component also defines
// two standard aliases, 'bsl::istringstream' and 'bsl::wistringstream', that
// refer to specializations of the 'bsl::basic_istringstream' template for
// 'char' and 'wchar_t' types, respectively.  The 'bsl::basic_istringstream'
// template has three parameters, 'CHAR_TYPE', 'CHAR_TRAITS', and 'ALLOCATOR'.
// The 'CHAR_TYPE' and 'CHAR_TRAITS' parameters respectively define the
// character type for the string-stream and a type providing a set of
// operations the string-stream will use to manipulate characters of that type,
// which must meet the character traits requirements defined by the C++11
// standard, 21.2 [char.traits].  The 'ALLOCATOR' template parameter is
// described in the "Memory Allocation" section below.
//
///Memory Allocation
///-----------------
// The type supplied as a string-stream's 'ALLOCATOR' template parameter
// determines how that string-stream will allocate memory.  The
// 'basic_istringstream' template supports allocators meeting the requirements
// of the C++11 standard, 17.6.3.5 [allocator.requirements]; in addition, it
// supports scoped-allocators derived from the 'bslma::Allocator' memory
// allocation protocol.  Clients intending to use 'bslma'-style allocators
// should use 'bsl::allocator', which provides a C++11 standard-compatible
// adapter for a 'bslma::Allocator' object.  Note that the standard aliases
// 'bsl::istringstream' and 'bsl::wistringstream' both use 'bsl::allocator'.
//
///'bslma'-Style Allocators
/// - - - - - - - - - - - -
// If the type supplied for the 'ALLOCATOR' template parameter of an
// 'istringstream' instantiation is 'bsl::allocator', then objects of that
// string-stream type will conform to the standard behavior of a
// 'bslma'-allocator-enabled type.  Such a string-stream accepts an optional
// 'bslma::Allocator' argument at construction.  If the address of a
// 'bslma::Allocator' object is explicitly supplied at construction, it will be
// used to supply memory for the string-stream throughout its lifetime;
// otherwise, the string-stream will use the default allocator installed at the
// time of the string-stream's construction (see 'bslma_default').
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Basic Input Operations
///- - - - - - - - - - - - - - - - -
// The following example demonstrates the use of 'bsl::istringstream' to read
// data of various types from a 'bsl::string' object.
//
// Suppose we want to implement a simplified converter from 'bsl::string' to a
// generic type, 'TYPE'.  We use 'bsl::istringstream' to implement the
// 'fromString' function.  We initialize the input stream with the string
// passed as a parameter and then we read the data from the input stream with
// 'operator>>':
//..
//  template <class TYPE>
//  TYPE fromString(const bsl::string& from)
//  {
//      bsl::istringstream in(from);
//      TYPE val;
//      in >> val;
//      return val;
//  }
//..
// Finally, we verify that our 'fromString' function works on some simple test
// cases:
//..
//  assert(fromString<int>("1234") == 1234);
//
//  assert(fromString<short>("-5") == -5);
//
//  assert(fromString<bsl::string>("abc") == "abc");
//..

// Prevent 'bslstl' headers from being included directly in 'BSL_OVERRIDES_STD'
// mode.  Doing so is unsupported, and is likely to cause compilation errors.
#if defined(BSL_OVERRIDES_STD) && !defined(BSL_STDHDRS_PROLOGUE_IN_EFFECT)
#error "include <bsl_sstream.h> instead of <bslstl_istringstream.h> in \
BSL_OVERRIDES_STD mode"
#endif

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLSTL_STRING
#include <bslstl_string.h>
#endif

#ifndef INCLUDED_BSLSTL_STRINGBUF
#include <bslstl_stringbuf.h>
#endif

#ifndef INCLUDED_BSLMA_USESBSLMAALLOCATOR
#include <bslma_usesbslmaallocator.h>
#endif

#ifndef INCLUDED_IOS
#include <ios>
#define INCLUDED_IOS
#endif

#ifndef INCLUDED_ISTREAM
#include <istream>
#define INCLUDED_ISTREAM
#endif


namespace bsl {

                          // =========================
                          // class basic_istringstream
                          // =========================

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
class basic_istringstream
    : private StringBufContainer<CHAR_TYPE, CHAR_TRAITS, ALLOCATOR>
    , public native_std::basic_istream<CHAR_TYPE, CHAR_TRAITS> {
    // This class implements a standard input stream that provides a
    // constructor and manipulator for setting the sequence of characters from
    // which input is streamed to a supplied 'bsl::basic_string'.

  private:
    // PRIVATE TYPES
    typedef basic_stringbuf<CHAR_TYPE, CHAR_TRAITS, ALLOCATOR>   StreamBufType;
    typedef StringBufContainer<CHAR_TYPE, CHAR_TRAITS, ALLOCATOR>
                                                                 BaseType;
    typedef bsl::basic_string<CHAR_TYPE, CHAR_TRAITS, ALLOCATOR> StringType;
    typedef native_std::basic_istream<CHAR_TYPE, CHAR_TRAITS>    BaseStream;
    typedef native_std::ios_base                                 ios_base;

  private:
    // NOT IMPLEMENTED
    basic_istringstream(const basic_istringstream&);             // = delete
    basic_istringstream& operator=(const basic_istringstream&);  // = delete

  public:
    // TYPES
    typedef CHAR_TYPE                       char_type;
    typedef CHAR_TRAITS                     traits_type;
    typedef ALLOCATOR                       allocator_type;
    typedef typename traits_type::int_type  int_type;
    typedef typename traits_type::off_type  off_type;
    typedef typename traits_type::pos_type  pos_type;

    // CREATORS
    explicit
    basic_istringstream(
                      const allocator_type& basicAllocator = allocator_type());
    explicit
    basic_istringstream(
                      ios_base::openmode    modeBitMask,
                      const allocator_type& basicAllocator = allocator_type());
    explicit
    basic_istringstream(
                      const StringType&     initialString,
                      const allocator_type& basicAllocator = allocator_type());
    basic_istringstream(
                      const StringType&     initialString,
                      ios_base::openmode    modeBitMask,
                      const allocator_type& basicAllocator = allocator_type());
        // Create a 'basic_istringstream' object.  Optionally specify a
        // 'modeBitMask' indicating whether the underlying stream-buffer may
        // also be written to ('rdbuf' is created using
        // 'modeBitMask | ios_base::in').  If 'modeBitMask' is not supplied,
        // 'rdbuf' will be created using 'ios_base::in'.  Optionally specify an
        // 'initialString' indicating the sequence of characters from which
        // input will be streamed.  If 'initialString' is not supplied, there
        // will not be data to stream (until a subsequent call to the 'str'
        // manipulator).  Optionally specify the 'basicAllocator' used to
        // supply memory.  If 'basicAllocator' is not supplied, a
        // default-constructed object of the (template parameter) 'ALLOCATOR'
        // type is used.  If the 'ALLOCATOR' argument is of type
        // 'bsl::allocator' (the default), then 'basicAllocator', if supplied,
        // shall be convertible to 'bslma::Allocator *'.  If the 'ALLOCATOR'
        // argument is of type 'bsl::allocator' and 'basicAllocator' is not
        // supplied, the currently installed default allocator will be used to
        // supply memory.

    //! ~basic_istringstream() = default;
        // Destroy this object.

    // MANIPULATORS
    void str(const StringType& value);
        // Reset the internally buffered sequence of characters provided as
        // input by this stream to the specified 'value'.

    // ACCESSORS
    StringType str() const;
        // Return the sequence of characters referred to by this stream object.

    StreamBufType *rdbuf() const;
        // Return an address providing modifiable access to the
        // 'basic_stringbuf' object that is internally used by this string
        // stream to buffer unformatted characters.
};

// STANDARD TYPEDEFS
typedef basic_istringstream<char, char_traits<char>, allocator<char> >
                                                                 istringstream;
typedef basic_istringstream<wchar_t, char_traits<wchar_t>, allocator<wchar_t> >
                                                                wistringstream;

// ============================================================================
//                       TEMPLATE FUNCTION DEFINITIONS
// ============================================================================

                          // -------------------------
                          // class basic_istringstream
                          // -------------------------

// CREATORS
template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
inline
basic_istringstream<CHAR_TYPE, CHAR_TRAITS, ALLOCATOR>::
basic_istringstream(const allocator_type& basicAllocator)
: BaseType(ios_base::in, basicAllocator)
, BaseStream(BaseType::rdbuf())
{
}

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
inline
basic_istringstream<CHAR_TYPE, CHAR_TRAITS, ALLOCATOR>::
basic_istringstream(ios_base::openmode    modeBitMask,
                    const allocator_type& basicAllocator)
: BaseType(modeBitMask | ios_base::in, basicAllocator)
, BaseStream(BaseType::rdbuf())
{
}

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
inline
basic_istringstream<CHAR_TYPE, CHAR_TRAITS, ALLOCATOR>::
basic_istringstream(const StringType&     initialString,
                    const allocator_type& basicAllocator)
: BaseType(initialString, ios_base::in, basicAllocator)
, BaseStream(BaseType::rdbuf())
{
}

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
inline
basic_istringstream<CHAR_TYPE, CHAR_TRAITS, ALLOCATOR>::
basic_istringstream(const StringType&     initialString,
                    ios_base::openmode    modeBitMask,
                    const allocator_type& basicAllocator)
: BaseType(initialString, modeBitMask | ios_base::in, basicAllocator)
, BaseStream(BaseType::rdbuf())
{
}

// MANIPULATORS
template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
inline
void basic_istringstream<CHAR_TYPE, CHAR_TRAITS, ALLOCATOR>::str(
                                                       const StringType& value)
{
    this->rdbuf()->str(value);
}

// ACCESSORS
template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
inline
typename basic_istringstream<CHAR_TYPE, CHAR_TRAITS, ALLOCATOR>::StringType
basic_istringstream<CHAR_TYPE, CHAR_TRAITS, ALLOCATOR>::str() const
{
    return this->rdbuf()->str();
}

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
inline
typename
basic_istringstream<CHAR_TYPE, CHAR_TRAITS, ALLOCATOR>::StreamBufType *
basic_istringstream<CHAR_TYPE, CHAR_TRAITS, ALLOCATOR>::rdbuf() const
{
    return this->BaseType::rdbuf();
}

}  // close namespace bsl

// ============================================================================
//                                TYPE TRAITS
// ============================================================================

namespace BloombergLP {
namespace bslma {

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
struct UsesBslmaAllocator<
        bsl::basic_istringstream<CHAR_TYPE, CHAR_TRAITS, ALLOCATOR> >
    : bsl::true_type
{};

}  // close namespace bslma
}  // close enterprise namespace

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
