// bslstl_ostringstream.h                                             -*-C++-*-
#ifndef INCLUDED_BSLSTL_OSTRINGSTREAM
#define INCLUDED_BSLSTL_OSTRINGSTREAM

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a C++03 compatible ostringstream class.
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
// This component defines a class template 'bsl::basic_ostringstream',
// implementing a standard output stream that provides a method for obtaining a
// 'bsl::basic_string', which contains the characters that have been written to
// the stream.  This component also defines two standard aliases,
// 'bsl::ostringstream' and 'bsl::wostringstream', that refer to
// specializations of the 'bsl::basic_ostringstream' template for 'char' and
// 'wchar_t' types respectively.  The 'bsl::basic_ostringstream' template has
// three parameters, 'CHAR_TYPE', 'CHAR_TRAITS', and 'ALLOCATOR'.  The
// 'CHAR_TYPE' and 'CHAR_TRAITS' parameters respectively define the character
// type for the string-stream and a type providing a set of operations the
// string-stream will use to manipulate characters of that type, which must
// meet the character traits requirements defined by the C++11 standard, 21.2
// [char.traits].  The 'ALLOCATOR' template parameter is described in the
// "Memory Allocation" section below.
//
///Memory Allocation
///-----------------
// The type supplied as a string-stream's 'ALLOCATOR' template parameter
// determines how that string-stream will allocate memory.  The
// 'basic_ostringstream' template supports allocators meeting the requirements
// of the C++11 standard [17.6.3.5], in addition it supports scoped-allocators
// derived from the 'bslma::Allocator' memory allocation protocol.  Clients
// intending to use 'bslma' style allocators should use 'bsl::allocator', which
// provides a C++11 standard-compatible adapter for a 'bslma::Allocator'
// object.  Note that the standard aliases 'bsl::ostringstream' and
// 'bsl::wostringstream' both use 'bsl::allocator'.
//
///'bslma'-Style Allocators
/// - - - - - - - - - - - -
// If the parameterized 'ALLOCATOR' type of an 'ostringstream' instantiation is
// 'bsl::allocator', then objects of that string-stream type will conform to
// the standard behavior of a 'bslma'-allocator-enabled type.  Such a
// string-stream accepts an optional 'bslma::Allocator' argument at
// construction.  If the address of a 'bslma::Allocator' object is explicitly
// supplied at construction, it will be used to supply memory for the
// string-stream throughout its lifetime; otherwise, the string-stream will use
// the default allocator installed at the time of the string-stream's
// construction (see 'bslma_default').
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
    : private basic_stringbuf_container<CHAR_TYPE, CHAR_TRAITS, ALLOCATOR>
    , public native_std::basic_ostream<CHAR_TYPE, CHAR_TRAITS> {
    // This class implements an output stream capable of creating a
    // 'basic_string' containing the characters that have been written to the
    // stream.

  private:
    // PRIVATE TYPES
    typedef basic_stringbuf<CHAR_TYPE, CHAR_TRAITS, ALLOCATOR>   StreamBufType;
    typedef basic_stringbuf_container<CHAR_TYPE, CHAR_TRAITS, ALLOCATOR>
                                                                 BaseType;
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
        // Create a 'basic_ostringstream' object.  Optionally specify a
        // 'modeBitMask' indicating whether the underlying stream-buffer may
        // also be read from ('rdbuf' is created using
        // 'modeBitMask | ios_base::out').  If 'modeBitMask' is not supplied
        // 'rdbuf' will be created using 'ios_base::out'.  Optionally specify
        // 'initialString' indicating the value that will be returned by a call
        // to 'str' prior to any streaming operations performed on this object.
        // If 'initialString' is not supplied 'str' will initially return an
        // empty strinng.  Optionally specify 'allocator' used to supply
        // memory.  If 'allocator' is not supplied, a default-constructed
        // object of the parameterized 'ALLOCATOR' type is used.  If the
        // template parameter 'ALLOCATOR' argument is of type 'bsl::allocator'
        // (the default) then 'allocator', if supplied, shall be convertible to
        // 'bslma::Allocator *'.  If the template parameter 'ALLOCATOR'
        // argument is of type 'bsl::allocator' and 'allocator' is not
        // supplied, the currently installed default allocator will be used to
        // supply memory.

    // MANIPULATORS
    void str(const StringType& value);
        // Reset the internally buffered sequence of characters maintained by
        // this 'ostringstream' object to the specified 'value'.

    // ACCESSORS
    StringType str() const;
        // Return the sequence of characters that have been written to this
        // stream object.

    StreamBufType *rdbuf() const;
        // Return the address of the 'basic_stringbuf' object that is
        // internally used by this string stream to buffer the formatted
        // characters.

};

// STANDARD TYPEDEFS
typedef basic_ostringstream<char, char_traits<char>, allocator<char> > 
                                                                 ostringstream;
typedef basic_ostringstream<wchar_t, char_traits<wchar_t>, allocator<wchar_t> >
                                                                wostringstream;

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
basic_ostringstream(const allocator_type& allocator)
: BaseType(ios_base::out, allocator)
, BaseStream(this->rdbuf())
{
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
inline
basic_ostringstream<CHAR_TYPE, CHAR_TRAITS, ALLOCATOR>::
basic_ostringstream(ios_base::openmode    modeBitMask,
                    const allocator_type& allocator)
: BaseType(modeBitMask | ios_base::out, allocator)
, BaseStream(this->rdbuf())
{
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
inline
basic_ostringstream<CHAR_TYPE, CHAR_TRAITS, ALLOCATOR>::
basic_ostringstream(const StringType&     initialString,
                    const allocator_type& allocator)
: BaseType(initialString, ios_base::out, allocator)
, BaseStream(this->rdbuf())
{
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
inline
basic_ostringstream<CHAR_TYPE, CHAR_TRAITS, ALLOCATOR>::
basic_ostringstream(const StringType&     initialString,
                    ios_base::openmode    modeBitmask,
                    const allocator_type& allocator)
: BaseType(initialString, modeBitmask, allocator)
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
typename
       basic_ostringstream<CHAR_TYPE, CHAR_TRAITS, ALLOCATOR>::StreamBufType *
basic_ostringstream<CHAR_TYPE, CHAR_TRAITS, ALLOCATOR>::rdbuf() const
{
    return this->BaseType::rdbuf();
}

// MANIPULATORS
template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
inline
void basic_ostringstream<CHAR_TYPE, CHAR_TRAITS, ALLOCATOR>::str(
                                                       const StringType& value)
{
    this->rdbuf()->str(value);
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
