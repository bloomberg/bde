// bslstl_hash.h                                                      -*-C++-*-
#ifndef INCLUDED_BSLSTP_HASH
#define INCLUDED_BSLSTP_HASH

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
// This component is for use by the deprecated 'bslstp' hash containers.

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITS
#include <bslalg_typetraits.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORM
#include <bsls_platform.h>
#endif

#ifndef INCLUDED_BSLSTL_HASH
#include <bslstl_hash.h>
#endif

#ifndef INCLUDED_CSTDDEF
#include <cstddef>  // for 'std::size_t'
#define INCLUDED_CSTDDEF
#endif

namespace BloombergLP {
namespace bslstp {

                          // ==================
                          // class bslst::hash
                          // ==================

template <class HASH_KEY> struct Hash;
    // Empty base class for hashing.  No general hash struct defined, each type
    // requires a specialization.  Leaving this struct declared but undefined
    // will generate error messages that are more clear when someone tries to
    // use a key that does not have a corresponding hash function.

template <> struct Hash<void *>;
template <> struct Hash<const void *>;
template <> struct Hash<const char *>;
template <> struct Hash<char>;
template <> struct Hash<signed char>;
template <> struct Hash<unsigned char>;
template <> struct Hash<short>;
template <> struct Hash<unsigned short>;
template <> struct Hash<int>;
template <> struct Hash<unsigned int>;
template <> struct Hash<long>;
template <> struct Hash<unsigned long>;
template <> struct Hash<long long>;
template <> struct Hash<unsigned long long>;

                       // ==========================
                       // class bslstp::HashSelector
                       // ==========================

template <class HASH_KEY>
struct HashSelector {
    // This meta-function selects the appropriate implementation for comparing
    // the parameterized 'TYPE'.  This generic template uses the
    // 'std::equal_to' functor.

    // TYPES
    typedef ::bsl::hash<HASH_KEY> Type;
};

template <>
struct HashSelector<void *> {
    typedef Hash<void *> Type;
};

template <>
struct HashSelector<const void *> {
    typedef Hash<const void *> Type;
};

template <>
struct HashSelector<const char *> {
    typedef Hash<const char *> Type;
};

template <>
struct HashSelector<char> {
    typedef Hash<char> Type;
};

template <>
struct HashSelector<signed char> {
    typedef Hash<signed char> Type;
};

template <>
struct HashSelector<unsigned char> {
    typedef Hash<unsigned char> Type;
};

template <>
struct HashSelector<short> {
    typedef Hash<short> Type;
};

template <>
struct HashSelector<unsigned short> {
    typedef Hash<unsigned short> Type;
};

template <>
struct HashSelector<int> {
    typedef Hash<int> Type;
};

template <>
struct HashSelector<unsigned int> {
    typedef Hash<unsigned int> Type;
};

template <>
struct HashSelector<long> {
    typedef Hash<long> Type;
};

template <>
struct HashSelector<unsigned long> {
    typedef Hash<unsigned long> Type;
};

template <>
struct HashSelector<long long> {
    typedef Hash<long long> Type;
};

template <>
struct HashSelector<unsigned long long> {
    typedef Hash<unsigned long long> Type;
};

                 // ============================================
                 // explcit class bslstp::Hash<> specializations
                 // ============================================

template <>
struct Hash<void *> {
    // Specialization of 'hash' for 'void' pointers.

    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(
                                Hash,
                                BloombergLP::bslalg::TypeTraitBitwiseCopyable);

    // ACCESSORS
    std::size_t operator()(const void *x) const
        // Return a hash value computed using the specified 'x'.
    {
        return (std::size_t)x;
    }
};

template <>
struct Hash<const void *> {
    // Specialization of 'hash' for 'const' 'void' pointers.

    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(
                                Hash,
                                BloombergLP::bslalg::TypeTraitBitwiseCopyable);

    // ACCESSORS
    std::size_t operator()(const void *x) const
        // Return a hash value computed using the specified 'x'.
    {
        return (std::size_t)x;
    }
};

template <>
struct Hash<const char *> {
    // Specialization of 'hash' for 'const' 'char' pointers.

    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(
                                Hash,
                                BloombergLP::bslalg::TypeTraitBitwiseCopyable);

    // ACCESSORS
    std::size_t operator()(const char *s) const
        // Return a hash value computed using the specified 's'.
    {
        unsigned long result = 0;

        for (; *s; ++s) {
            result = 5 * result + *s;
        }

        return std::size_t(result);
    }
};

template <>
struct Hash<char> {
    // Specialization of 'Hash' for 'char' values.

    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(
                                Hash,
                                BloombergLP::bslalg::TypeTraitBitwiseCopyable);

    // ACCESSORS
    std::size_t operator()(char x) const
        // Return a hash value computed using the specified 'x'.
    {
        return x;
    }
};

template <>
struct Hash<unsigned char> {
    // Specialization of 'Hash' for 'unsigned' 'char' values.

    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(
                                Hash,
                                BloombergLP::bslalg::TypeTraitBitwiseCopyable);

    // ACCESSORS
    std::size_t operator()(unsigned char x) const
        // Return a hash value computed using the specified 'x'.
    {
        return x;
    }
};

template <>
struct Hash<signed char> {
    // Specialization of 'Hash' for 'signed' 'char' values.

    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(
                                Hash,
                                BloombergLP::bslalg::TypeTraitBitwiseCopyable);

    // ACCESSORS
    std::size_t operator()(signed char x) const
        // Return a hash value computed using the specified 'x'.
    {
        return x;
    }
};

template <>
struct Hash<short> {
    // Specialization of 'Hash' for 'short' values.

    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(
                                Hash,
                                BloombergLP::bslalg::TypeTraitBitwiseCopyable);

    // ACCESSORS
    std::size_t operator()(short x) const
        // Return a hash value computed using the specified 'x'.
    {
        return x;
    }
};

template <>
struct Hash<unsigned short> {
    // Specialization of 'Hash' for 'unsigned' 'short' values.

    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(
                                Hash,
                                BloombergLP::bslalg::TypeTraitBitwiseCopyable);

    // ACCESSORS
    std::size_t operator()(unsigned short x) const
        // Return a hash value computed using the specified 'x'.
    {
        return x;
    }
};

template <>
struct Hash<int> {
    // Specialization of 'Hash' for 'int' values.

    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(
                                Hash,
                                BloombergLP::bslalg::TypeTraitBitwiseCopyable);

    // ACCESSORS
    std::size_t operator()(int x) const
        // Return a hash value computed using the specified 'x'.
    {
        return x;
    }
};

template <>
struct Hash<unsigned int> {
    // Specialization of 'Hash' for 'unsigned' 'int' values.

    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(
                                Hash,
                                BloombergLP::bslalg::TypeTraitBitwiseCopyable);

    // ACCESSORS
    std::size_t operator()(unsigned int x) const
        // Return a hash value computed using the specified 'x'.
    {
        return x;
    }
};

template <>
struct Hash<long> {
    // Specialization of 'Hash' for 'long' values.

    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(
                                Hash,
                                BloombergLP::bslalg::TypeTraitBitwiseCopyable);

    // ACCESSORS
    std::size_t operator()(long x) const
        // Return a hash value computed using the specified 'x'.
    {
        return x;
    }
};

template <>
struct Hash<unsigned long> {
    // Specialization of 'Hash' for 'unsigned' 'long' values.

    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(
                                Hash,
                                BloombergLP::bslalg::TypeTraitBitwiseCopyable);

    // ACCESSORS
    std::size_t operator()(unsigned long x) const
        // Return a hash value computed using the specified 'x'.
    {
        return x;
    }
};

#ifdef BSLS_PLATFORM__CPU_64_BIT
template <>
struct Hash<long long> {
    // Specialization of 'Hash' for 'long long' values.

    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(
                                Hash,
                                BloombergLP::bslalg::TypeTraitBitwiseCopyable);

    // ACCESSORS
    std::size_t operator()(long long x) const
        // Return a hash value computed using the specified 'x'.
    {
        return x;
    }
};

template <>
struct Hash<unsigned long long> {
    // Specialization of 'Hash' for 'unsigned' 'long long' values.

    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(
                                Hash,
                                BloombergLP::bslalg::TypeTraitBitwiseCopyable);

    // ACCESSORS
    std::size_t operator()(unsigned long long x) const
        // Return a hash value computed using the specified 'x'.
    {
        return x;
    }
};

#else  // BSLS_PLATFORM__CPU_32_BIT

template <>
struct Hash<long long> {
    // Specialization of 'Hash' for 'long long' values.

    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(
                                Hash,
                                BloombergLP::bslalg::TypeTraitBitwiseCopyable);

    // ACCESSORS
    std::size_t operator()(unsigned long long x) const
        // Return a hash value computed using the specified 'x'.
    {
        return (std::size_t)(x ^ (x >> 32));
    }
};

template <>
struct Hash<unsigned long long> {
    // Specialization of 'Hash' for 'unsigned' 'long long' values.

    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(
                                Hash,
                                BloombergLP::bslalg::TypeTraitBitwiseCopyable);

    // ACCESSORS
    std::size_t operator()(unsigned long long x) const
        // Return a hash value computed using the specified 'x'.
    {
        return (std::size_t)(x ^ (x >> 32));
    }
};
#endif   // BSLS_PLATFORM__CPU_64_BIT

}  // close package namespace
}  // close enterprise namespace

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2009
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
