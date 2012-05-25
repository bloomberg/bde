// bslstl_istringstream.h                                             -*-C++-*-
#ifndef INCLUDED_BSLSTL_ISTRINGSTREAM
#define INCLUDED_BSLSTL_ISTRINGSTREAM

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide the C++03 compatible istringstream class.
//
//@CLASSES:
//  bsl::istringstream: C++03-compatible istringstream class
//
//@SEE_ALSO:
//
//@AUTHOR: Alexei Zakharov (azakhar1)
//
//@DESCRIPTION: This component is for internal use only.  Please include
// '<bsl_sstream.h>' instead.
//
// This component provides 'bsl::basic_istringstream' class template
// parameterized with a character type and two specializations: one for 'char'
// and another for 'wchar_t'.  'bsl::basic_istringstream' implements the
// functionality of the standard class 'std::basic_istream' for reading from
// 'bsl::basic_string' objects.  It's a 'bsl' replacement for the standard
// 'std::basic_istringstream' class.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Basic input operations
///- - - - - - - - - - - - - - - - -
// The following example demonstrates the use of 'bsl::istringstream' to read
// data of various types from a 'bsl::string' object.
//
// Suppose we want to implement a simplified converter from 'bsl::string' to a
// generic type 'TYPE'.  We use 'bsl::istringstream' to implement the
// 'fromStream' function.  We initialize the input stream with the string
// passed as a parameter and the we read the data from the input stream with
// 'operator>>':
//..
//  template <typename TYPE>
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
//  assert(fromString<short>("-5") == -5);
//  assert(fromString<bsl::string>("abc") == "abc");
//..

// Prevent 'bslstl' headers from being included directly in 'BSL_OVERRIDES_STD'
// mode.  Doing so is unsupported, and is likely to cause compilation errors.
#if defined(BSL_OVERRIDES_STD) && !defined(BSL_STDHDRS_PROLOGUE_IN_EFFECT)
#error "include <bsl_sstream.h> instead of <bslstl_istringstream.h> in \
BSL_OVERRIDES_STD mode"
#endif

#ifndef INCLUDED_BSLSTL_STRINGBUF
#include <bslstl_stringbuf.h>
#endif

namespace bsl {

                          // =========================
                          // class basic_istringstream
                          // =========================

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
class basic_istringstream
    : private basic_stringbuf<CHAR_TYPE, CHAR_TRAITS, ALLOCATOR>
    , public native_std::basic_istream<CHAR_TYPE, CHAR_TRAITS>
    // This class implements the 'istream' interface to manipulate
    // 'bsl::string' objects as input streams of data.
{
  private:
    // PRIVATE TYPES
    typedef basic_stringbuf<CHAR_TYPE, CHAR_TRAITS, ALLOCATOR>   StreamBufType;
    typedef bsl::basic_string<CHAR_TYPE, CHAR_TRAITS, ALLOCATOR> StringType;
    typedef native_std::basic_istream<CHAR_TYPE, CHAR_TRAITS>    BaseStream;
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
            basic_istringstream,
            BloombergLP::bslalg::TypeTraitUsesBslmaAllocator);

    // CREATORS
    explicit
    basic_istringstream(const allocator_type& alloc = allocator_type());
    explicit
    basic_istringstream(ios_base::openmode which,
                        const allocator_type& alloc = allocator_type());
    explicit
    basic_istringstream(const StringType& str,
                        const allocator_type& alloc = allocator_type());
    explicit
    basic_istringstream(const StringType& str,
                        ios_base::openmode which,
                        const allocator_type& alloc = allocator_type());
        // Create an 'istringstream' object with an optionally specified
        // 'alloc' allocator, with the 'which' open mode, and the initial 'str'
        // string.

    // ACCESSORS
    StringType str() const;
        // Return the intput string of this 'istringstream' object.

    StreamBufType * rdbuf() const;
        // Return the modifiable pointer to the 'stringbuf' object that
        // performs the unformatted input for this 'istringstream' object.

    // MANIPULATORS
    void str(const StringType& s);
        // Initialize this 'istringstream' object with the specified string
        // 's'.
};

// ==========================================================================
//                       TEMPLATE FUNCTION DEFINITIONS
// ==========================================================================

                          // -------------------------
                          // class basic_istringstream
                          // -------------------------

// CREATORS
template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
inline
basic_istringstream<CHAR_TYPE, CHAR_TRAITS, ALLOCATOR>::
    basic_istringstream(const allocator_type& alloc)
: StreamBufType(ios_base::in, alloc)
, BaseStream(this->rdbuf())
{
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
inline
basic_istringstream<CHAR_TYPE, CHAR_TRAITS, ALLOCATOR>::
    basic_istringstream(ios_base::openmode which,
                        const allocator_type& alloc)
: StreamBufType(which, alloc)
, BaseStream(this->rdbuf())
{
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
inline
basic_istringstream<CHAR_TYPE, CHAR_TRAITS, ALLOCATOR>::
    basic_istringstream(const StringType& str,
                        const allocator_type& alloc)
: StreamBufType(str, ios_base::in, alloc)
, BaseStream(this->rdbuf())
{
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
inline
basic_istringstream<CHAR_TYPE, CHAR_TRAITS, ALLOCATOR>::
    basic_istringstream(const StringType& str,
                        ios_base::openmode which,
                        const allocator_type& alloc)
: StreamBufType(str, which, alloc)
, BaseStream(this->rdbuf())
{
}

// ACCESSORS
template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
inline
typename basic_istringstream<CHAR_TYPE, CHAR_TRAITS, ALLOCATOR>::StringType
    basic_istringstream<CHAR_TYPE, CHAR_TRAITS, ALLOCATOR>::str() const
{
    return this->rdbuf()->str();
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
inline
typename basic_istringstream<CHAR_TYPE, CHAR_TRAITS, ALLOCATOR>::StreamBufType *
    basic_istringstream<CHAR_TYPE, CHAR_TRAITS, ALLOCATOR>::rdbuf() const
{
    return const_cast<StreamBufType *>(
            static_cast<const StreamBufType *>(this));
}

// MANIPULATORS
template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
inline
void basic_istringstream<CHAR_TYPE, CHAR_TRAITS, ALLOCATOR>::str(
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
