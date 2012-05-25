// bslstl_ostringstream.h                                             -*-C++-*-
#ifndef INCLUDED_BSLSTL_OSTRINGSTREAM
#define INCLUDED_BSLSTL_OSTRINGSTREAM

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide the C++03 compatible ostringstream class.
//
//@CLASSES:
//  bsl::ostringstream: C++03-compatible ostringstream class
//
//@SEE_ALSO:
//
//@AUTHOR: Alexei Zakharov (azakhar1)
//
//@DESCRIPTION: This component is for internal use only.  Please include
// '<bsl_sstream.h>' instead.
//
// This component provides 'bsl::basic_ostringstream' class template
// parameterized with a character type and two specializations: one for 'char'
// and another for 'wchar_t'.  'bsl::basic_ostringstream' implements the
// functionality of the standard class 'std::basic_ostream' for writing into
// 'bsl::basic_string' objects.  It's a 'bsl' replacement for the standard
// 'std::basic_ostringstream' class.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Basic input operations
///- - - - - - - - - - - - - - - - -
// The following example demonstrates the use of 'bsl::ostringstream' to write
// data of various types into a 'bsl::string' object.
//
// Suppose we want to implement a simplified converter from a generic type
// 'TYPE' to 'bsl::string'.  We use 'bsl::ostringstream' to implement the
// 'toString' function.  We write the data into the stream with 'operator<<'
// and then use the 'str' method to retrieve the resulting string from the
// stream:
//..
//  template <typename TYPE>
//  bsl::string toString(const TYPE& what)
//  {
//      bsl::ostringstream out;
//      out << what;
//      return out.str();
//  }
//..
// Finally, we verify that our 'toString' function works on some simple test
// cases:
//..
//  assert(toString(1234) == "1234");
//  assert(toString<short>(-5) == "-5");
//  assert(toString("abc") == "abc");
//..

// Prevent 'bslstl' headers from being included directly in 'BSL_OVERRIDES_STD'
// mode.  Doing so is unsupported, and is likely to cause compilation errors.
#if defined(BSL_OVERRIDES_STD) && !defined(BSL_STDHDRS_PROLOGUE_IN_EFFECT)
#error "include <bsl_sstream.h> instead of <bslstl_ostringstream.h> in \
BSL_OVERRIDES_STD mode"
#endif

#ifndef INCLUDED_BSLSTL_STRINGBUF
#include <bslstl_stringbuf.h>
#endif

namespace bsl {

                          // =========================
                          // class basic_ostringstream
                          // =========================

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
class basic_ostringstream
    : private basic_stringbuf<CHAR_TYPE, CHAR_TRAITS, ALLOCATOR>
    , public native_std::basic_ostream<CHAR_TYPE, CHAR_TRAITS>
    // This class implements the 'ostream' interface to manipulate
    // 'bsl::string' objects as output streams of data.
{
  private:
    // PRIVATE TYPES
    typedef basic_stringbuf<CHAR_TYPE, CHAR_TRAITS, ALLOCATOR>   StreamBufType;
    typedef bsl::basic_string<CHAR_TYPE, CHAR_TRAITS, ALLOCATOR> StringType;
    typedef native_std::basic_ostream<CHAR_TYPE, CHAR_TRAITS>    BaseStream;
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
            basic_ostringstream,
            BloombergLP::bslalg::TypeTraitUsesBslmaAllocator);

    // CREATORS
    explicit
    basic_ostringstream(const allocator_type& alloc = allocator_type());
    explicit
    basic_ostringstream(ios_base::openmode which,
                        const allocator_type& alloc = allocator_type());
    explicit
    basic_ostringstream(const StringType& str,
                        const allocator_type& alloc = allocator_type());
    explicit
    basic_ostringstream(const StringType& str,
                        ios_base::openmode which,
                        const allocator_type& alloc = allocator_type());
        // Create an 'ostringstream' object with an optionally specified
        // 'alloc' allocator, with the 'which' open mode, and the initial 'str'
        // string.

    // ACCESSORS
    StringType str() const;
        // Return the string used for output of this 'ostringstream' object.

    StreamBufType * rdbuf() const;
        // Return the modifiable pointer to the 'stringbuf' object that
        // performs the unformatted output for this 'ostringstream' object.

    // MANIPULATORS
    void str(const StringType& s);
        // Initialize this 'ostringstream' object with the specified string
        // 's'.
};

// ==========================================================================
//                       TEMPLATE FUNCTION DEFINITIONS
// ==========================================================================

                          // -------------------------
                          // class basic_ostringstream
                          // -------------------------

// CREATORS
template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
inline
basic_ostringstream<CHAR_TYPE, CHAR_TRAITS, ALLOCATOR>::
    basic_ostringstream(const allocator_type& alloc)
: StreamBufType(ios_base::out, alloc)
, BaseStream(this->rdbuf())
{
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
inline
basic_ostringstream<CHAR_TYPE, CHAR_TRAITS, ALLOCATOR>::
    basic_ostringstream(ios_base::openmode which,
                        const allocator_type& alloc)
: StreamBufType(which, alloc)
, BaseStream(this->rdbuf())
{
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
inline
basic_ostringstream<CHAR_TYPE, CHAR_TRAITS, ALLOCATOR>::
    basic_ostringstream(const StringType& str,
                        const allocator_type& alloc)
: StreamBufType(str, ios_base::out, alloc)
, BaseStream(this->rdbuf())
{
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
inline
basic_ostringstream<CHAR_TYPE, CHAR_TRAITS, ALLOCATOR>::
    basic_ostringstream(const StringType& str,
                        ios_base::openmode which,
                        const allocator_type& alloc)
: StreamBufType(str, which, alloc)
, BaseStream(this->rdbuf())
{
}

// ACCESSORS
template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
inline
typename basic_ostringstream<CHAR_TYPE, CHAR_TRAITS, ALLOCATOR>::StringType
    basic_ostringstream<CHAR_TYPE, CHAR_TRAITS, ALLOCATOR>::str() const
{
    return this->rdbuf()->str();
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
inline
typename basic_ostringstream<CHAR_TYPE, CHAR_TRAITS, ALLOCATOR>::StreamBufType *
    basic_ostringstream<CHAR_TYPE, CHAR_TRAITS, ALLOCATOR>::rdbuf() const
{
    return const_cast<StreamBufType *>(
            static_cast<const StreamBufType *>(this));
}

// MANIPULATORS
template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
inline
void basic_ostringstream<CHAR_TYPE, CHAR_TRAITS, ALLOCATOR>::str(
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
