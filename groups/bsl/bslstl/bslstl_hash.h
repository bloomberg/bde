// bslstl_hash.h                                                      -*-C++-*-
#ifndef INCLUDED_BSLSTL_HASH
#define INCLUDED_BSLSTL_HASH

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a namespace for hash functions
//
//@CLASSES:
//  bsl::hash: hash function for primitive types
//
//@SEE_ALSO: bsl+stdhdrs
//
//@AUTHOR: Arthur Chiu (achiu21)
//
//@DESCRIPTION: This component provides a namespace for hash functions used by
// 'hash_map', 'hash_set' and 'hashtable'.
//
// Note that the hash functions here are based on STLPort's implementation,
// with copyright notice as follows:
//..
//-----------------------------------------------------------------------------
// Copyright (c) 1996-1998
// Silicon Graphics Computer Systems, Inc.
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all copies and
// that both that copyright notice and this permission notice appear
// in supporting documentation.  Silicon Graphics makes no
// representations about the suitability of this software for any
// purpose.  It is provided "as is" without express or implied warranty.
//
//
// Copyright (c) 1994
// Hewlett-Packard Company
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all copies and
// that both that copyright notice and this permission notice appear
// in supporting documentation.  Hewlett-Packard Company makes no
// representations about the suitability of this software for any
// purpose.  It is provided "as is" without express or implied warranty.
//-----------------------------------------------------------------------------
//..
//
///Usage
///-----
// This component is for use by the 'bsl+stdhdrs' package.

// Prevent 'bslstl' headers from being included directly in 'BSL_OVERRIDES_STD'
// mode.  Doing so is unsupported, and is likely to cause compilation errors.
#if defined(BSL_OVERRIDES_STD) && !defined(BSL_STDHDRS_PROLOGUE_IN_EFFECT)
#error "<bslstl_hash.h> header can't be included directly in \
BSL_OVERRIDES_STD mode"
#endif

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITS
#include <bslalg_typetraits.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORM
#include <bsls_platform.h>
#endif

#ifndef INCLUDED_CSTDDEF
#include <cstddef>  // for 'std::size_t'
#define INCLUDED_CSTDDEF
#endif

namespace bsl {

namespace {

// FREE FUNCTIONS
inline
std::size_t hashCstr(const char *string)
    // Return a hash value computed using the specified 'string'.
{
    unsigned long result = 0;

    for (; *string; ++string) {
        result = 5 * result + *string;
    }

    return std::size_t(result);
}

}  // close unnamed namespace

                          // ==================
                          // class bslstl::hash
                          // ==================

template <class BSLSTL_KEY> struct hash;
    // Empty base class for hashing.  No general hash struct defined, each type
    // requires a specialization.  Leaving this struct declared but undefined
    // will generate error messages that are more clear when someone tries to
    // use a key that does not have a corresponding hash function.

template <>
struct hash<void *> {
    // Specialization of 'hash' for 'void' pointers.

    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(
                                hash,
                                BloombergLP::bslalg::TypeTraitBitwiseCopyable);

    // ACCESSORS
    std::size_t operator()(const void *x) const
        // Return a hash value computed using the specified 'x'.
    {
        return (std::size_t)x;
    }
};

template <>
struct hash<const void *> {
    // Specialization of 'hash' for 'const' 'void' pointers.

    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(
                                hash,
                                BloombergLP::bslalg::TypeTraitBitwiseCopyable);

    // ACCESSORS
    std::size_t operator()(const void *x) const
        // Return a hash value computed using the specified 'x'.
    {
        return (std::size_t)x;
    }
};

template <>
struct hash<const char *> {
    // Specialization of 'hash' for 'const' 'char' pointers.

    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(
                                hash,
                                BloombergLP::bslalg::TypeTraitBitwiseCopyable);

    // ACCESSORS
    std::size_t operator()(const char *s) const
        // Return a hash value computed using the specified 's'.
    {
        return hashCstr(s);
    }
};

template <>
struct hash<char> {
    // Specialization of 'hash' for 'char' values.

    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(
                                hash,
                                BloombergLP::bslalg::TypeTraitBitwiseCopyable);

    // ACCESSORS
    std::size_t operator()(char x) const
        // Return a hash value computed using the specified 'x'.
    {
        return x;
    }
};

template <>
struct hash<unsigned char> {
    // Specialization of 'hash' for 'unsigned' 'char' values.

    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(
                                hash,
                                BloombergLP::bslalg::TypeTraitBitwiseCopyable);

    // ACCESSORS
    std::size_t operator()(unsigned char x) const
        // Return a hash value computed using the specified 'x'.
    {
        return x;
    }
};

template <>
struct hash<signed char> {
    // Specialization of 'hash' for 'signed' 'char' values.

    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(
                                hash,
                                BloombergLP::bslalg::TypeTraitBitwiseCopyable);

    // ACCESSORS
    std::size_t operator()(unsigned char x) const
        // Return a hash value computed using the specified 'x'.
    {
        return x;
    }
};

template <>
struct hash<short> {
    // Specialization of 'hash' for 'short' values.

    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(
                                hash,
                                BloombergLP::bslalg::TypeTraitBitwiseCopyable);

    // ACCESSORS
    std::size_t operator()(short x) const
        // Return a hash value computed using the specified 'x'.
    {
        return x;
    }
};

template <>
struct hash<unsigned short> {
    // Specialization of 'hash' for 'unsigned' 'short' values.

    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(
                                hash,
                                BloombergLP::bslalg::TypeTraitBitwiseCopyable);

    // ACCESSORS
    std::size_t operator()(unsigned short x) const
        // Return a hash value computed using the specified 'x'.
    {
        return x;
    }
};

template <>
struct hash<int> {
    // Specialization of 'hash' for 'int' values.

    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(
                                hash,
                                BloombergLP::bslalg::TypeTraitBitwiseCopyable);

    // ACCESSORS
    std::size_t operator()(int x) const
        // Return a hash value computed using the specified 'x'.
    {
        return x;
    }
};

template <>
struct hash<unsigned int> {
    // Specialization of 'hash' for 'unsigned' 'int' values.

    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(
                                hash,
                                BloombergLP::bslalg::TypeTraitBitwiseCopyable);

    // ACCESSORS
    std::size_t operator()(unsigned int x) const
        // Return a hash value computed using the specified 'x'.
    {
        return x;
    }
};

template <>
struct hash<long> {
    // Specialization of 'hash' for 'long' values.

    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(
                                hash,
                                BloombergLP::bslalg::TypeTraitBitwiseCopyable);

    // ACCESSORS
    std::size_t operator()(long x) const
        // Return a hash value computed using the specified 'x'.
    {
        return x;
    }
};

template <>
struct hash<unsigned long> {
    // Specialization of 'hash' for 'unsigned' 'long' values.

    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(
                                hash,
                                BloombergLP::bslalg::TypeTraitBitwiseCopyable);

    // ACCESSORS
    std::size_t operator()(unsigned long x) const
        // Return a hash value computed using the specified 'x'.
    {
        return x;
    }
};

#ifdef BSLS_PLATFORM_CPU_64_BIT
template <>
struct hash<long long> {
    // Specialization of 'hash' for 'long long' values.

    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(
                                hash,
                                BloombergLP::bslalg::TypeTraitBitwiseCopyable);

    // ACCESSORS
    std::size_t operator()(unsigned long long x) const
        // Return a hash value computed using the specified 'x'.
    {
        return x;
    }
};

template <>
struct hash<unsigned long long> {
    // Specialization of 'hash' for 'unsigned' 'long long' values.

    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(
                                hash,
                                BloombergLP::bslalg::TypeTraitBitwiseCopyable);

    // ACCESSORS
    std::size_t operator()(unsigned long long x) const
        // Return a hash value computed using the specified 'x'.
    {
        return x;
    }
};

#else  // BSLS_PLATFORM_CPU_32_BIT

template <>
struct hash<long long> {
    // Specialization of 'hash' for 'long long' values.

    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(
                                hash,
                                BloombergLP::bslalg::TypeTraitBitwiseCopyable);

    // ACCESSORS
    std::size_t operator()(unsigned long long x) const
        // Return a hash value computed using the specified 'x'.
    {
        return (std::size_t)(x ^ (x >> 32));
    }
};

template <>
struct hash<unsigned long long> {
    // Specialization of 'hash' for 'unsigned' 'long long' values.

    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(
                                hash,
                                BloombergLP::bslalg::TypeTraitBitwiseCopyable);

    // ACCESSORS
    std::size_t operator()(unsigned long long x) const
        // Return a hash value computed using the specified 'x'.
    {
        return (std::size_t)(x ^ (x >> 32));
    }
};
#endif   // BSLS_PLATFORM_CPU_64_BIT

}  // close namespace bsl

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2009
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
