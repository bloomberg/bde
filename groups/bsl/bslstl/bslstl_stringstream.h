// bslstl_stringstream.h                                              -*-C++-*-
#ifndef INCLUDED_BSLSTL_STRINGSTREAM
#define INCLUDED_BSLSTL_STRINGSTREAM

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a C++03 compatible stringstream class.
//
//@CLASSES:
//  bsl::stringstream: C++03-compatible stringstream class
//
//@SEE_ALSO:
//
//@AUTHOR: Alexei Zakharov (azakhar1)
//
//@DESCRIPTION: This component is for internal use only.  Please include
// '<bsl_sstream.h>' instead.
//
// This component provides 'bsl::basic_stringstream' class template
// parameterized with a character type and two specializations: one for 'char'
// and another for 'wchar_t'.  'bsl::basic_stringstream' implements the
// functionality of the standard class 'std::basic_stream' for reading and
// writing to 'bsl::basic_string' objects.  It's a 'bsl' replacement for the
// standard 'std::basic_stringstream' class.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Basic input operations
///- - - - - - - - - - - - - - - - -
// The following example demonstrates the use of 'bsl::stringstream' to read
// and write data of various types to a 'bsl::string' object.
//
// Suppose we want to implement a simplified converter between a pair of
// generic types 'TYPE1' and 'TYPE2'.  We use 'bsl::stringstream' to implement
// the 'lexicalCast' function.  We write the data of type 'TYPE1' into the
// stream with 'operator<<' and then read it back as the data of 'TYPE2' with
// 'operator>>':
//..
//  template <typename TYPE2, typename TYPE1>
//  TYPE2 lexicalCast(const TYPE1& what)
//  {
//      bsl::stringstream converter;
//      converter << what;
//
//      TYPE2 val;
//      converter >> val;
//      return val;
//  }
//..
// Finally, we verify that the 'lexicalCast' function works on some simple test
// cases:
//..
//  assert(lexicalCast<int>("1234") == 1234);
//  assert(lexicalCast<short>("-5") == -5);
//  assert(lexicalCast<bsl::string>("abc") == "abc");
//  assert(lexicalCast<bsl::string>(1234) == "1234");
//  assert(lexicalCast<short>(-5) == -5);
//..

// Prevent 'bslstl' headers from being included directly in 'BSL_OVERRIDES_STD'
// mode.  Doing so is unsupported, and is likely to cause compilation errors.
#if defined(BSL_OVERRIDES_STD) && !defined(BSL_STDHDRS_PROLOGUE_IN_EFFECT)
#error "include <bsl_sstream.h> instead of <bslstl_stringstream.h> in \
BSL_OVERRIDES_STD mode"
#endif

#ifndef INCLUDED_BSLSTL_STRINGBUF
#include <bslstl_stringbuf.h>
#endif

namespace bsl {

                          // ========================
                          // class basic_stringstream
                          // ========================

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
class basic_stringstream
    : private basic_stringbuf_container<CHAR_TYPE, CHAR_TRAITS, ALLOCATOR>
    , public native_std::basic_iostream<CHAR_TYPE, CHAR_TRAITS>
    // This class implements the 'stream' interface to manipulate
    // 'bsl::string' objects as input and output streams of data.
{
  private:
    // PRIVATE TYPES
    typedef basic_stringbuf<CHAR_TYPE, CHAR_TRAITS, ALLOCATOR>   StreamBufType;
    typedef basic_stringbuf_container<CHAR_TYPE, CHAR_TRAITS, ALLOCATOR>
                                                                 BaseType;
    typedef bsl::basic_string<CHAR_TYPE, CHAR_TRAITS, ALLOCATOR> StringType;
    typedef native_std::basic_iostream<CHAR_TYPE, CHAR_TRAITS>   BaseStream;
    typedef native_std::ios_base                                 ios_base;

  public:
    // TYPES
    typedef CHAR_TYPE                       char_type;
    typedef CHAR_TRAITS                     traits_type;
    typedef ALLOCATOR                       allocator_type;
    typedef typename traits_type::int_type  int_type;
    typedef typename traits_type::off_type  off_type;
    typedef typename traits_type::pos_type  pos_type;

    // TYPETRAITS
    BSLALG_DECLARE_NESTED_TRAITS(
            basic_stringstream,
            BloombergLP::bslalg::TypeTraitUsesBslmaAllocator);

    // CREATORS
    explicit
    basic_stringstream(const allocator_type& alloc = allocator_type());
    explicit
    basic_stringstream(ios_base::openmode which,
                       const allocator_type& alloc = allocator_type());
    explicit
    basic_stringstream(const StringType& str,
                       const allocator_type& alloc = allocator_type());
    explicit
    basic_stringstream(const StringType& str,
                       ios_base::openmode which,
                       const allocator_type& alloc = allocator_type());
        // Create a 'stringstream' object with an optionally specified
        // 'alloc' allocator, with the 'which' open mode, and the initial 'str'
        // string.

    // ACCESSORS
    StringType str() const;
        // Return the string used for input and output of this 'stringstream'
        // object.

    StreamBufType * rdbuf() const;
        // Return the modifiable pointer to the 'stringbuf' object that
        // performs the unformatted intput/output for this 'stringstream'
        // object.

    // MANIPULATORS
    void str(const StringType& s);
        // Initialize this 'stringstream' object with the specified string
        // 's'.
};

// ==========================================================================
//                       TEMPLATE FUNCTION DEFINITIONS
// ==========================================================================

                          // ------------------------
                          // class basic_stringstream
                          // ------------------------

// CREATORS
template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
inline
basic_stringstream<CHAR_TYPE, CHAR_TRAITS, ALLOCATOR>::
    basic_stringstream(const allocator_type& alloc)
: BaseType(ios_base::in | ios_base::out, alloc)
, BaseStream(this->rdbuf())
{
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
inline
basic_stringstream<CHAR_TYPE, CHAR_TRAITS, ALLOCATOR>::
    basic_stringstream(ios_base::openmode which,
                       const allocator_type& alloc)
: BaseType(which, alloc)
, BaseStream(this->rdbuf())
{
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
inline
basic_stringstream<CHAR_TYPE, CHAR_TRAITS, ALLOCATOR>::
    basic_stringstream(const StringType& str,
                       const allocator_type& alloc)
: BaseType(str, ios_base::in | ios_base::out, alloc)
, BaseStream(this->rdbuf())
{
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
inline
basic_stringstream<CHAR_TYPE, CHAR_TRAITS, ALLOCATOR>::
    basic_stringstream(const StringType& str,
                       ios_base::openmode which,
                       const allocator_type& alloc)
: BaseType(str, which, alloc)
, BaseStream(this->rdbuf())
{
}

// ACCESSORS
template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
inline
typename basic_stringstream<CHAR_TYPE, CHAR_TRAITS, ALLOCATOR>::StringType
    basic_stringstream<CHAR_TYPE, CHAR_TRAITS, ALLOCATOR>::str() const
{
    return this->rdbuf()->str();
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
inline
typename basic_stringstream<CHAR_TYPE, CHAR_TRAITS, ALLOCATOR>::StreamBufType *
    basic_stringstream<CHAR_TYPE, CHAR_TRAITS, ALLOCATOR>::rdbuf() const
{
    return this->BaseType::rdbuf();
}

// MANIPULATORS
template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
inline
void basic_stringstream<CHAR_TYPE, CHAR_TRAITS, ALLOCATOR>::str(
        const StringType& s)
{
    this->rdbuf()->str(s);
}

}  // close namespace bsl

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2012
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
