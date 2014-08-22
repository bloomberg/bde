// bslh_hash.h                                                        -*-C++-*-
#ifndef INCLUDED_BSLH_HASH
#define INCLUDED_BSLH_HASH

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a struct to run 'bslh' hash algorithms on supported types.
//
//@CLASSES:
//  bslh::Hash: Functor that runs 'bslh' hash algorithms on supported types
//
//@SEE_ALSO:
//
//@DESCRIPTION: This component provides a templated 'struct', 'bslh::Hash',
// that defines a hash-functor that can be used with standard containers (a
// drop in replacement for 'bsl::hash'), and which applies the supplied
// (template parameter) 'HASH_ALGORITHM' to the attributes of the (template
// parameter) 'TYPE' which have been identified as salient to hashing.  The
// 'bslh::Hash' template parameter 'HASH_ALGORITHM' must be a hashing algorithm
// that conforms the the requirements outlined below (see {'Requirements for
// Regular 'bslh' Hashing Algorithms'}).  Note that there are several hashing
// algorithms defined within the 'bslh' package and some, such as those that
// require seeds, will not meet these requirements, meaning they cannot be used
// with 'bslh::Hash'.  A call to 'bslh::Hash::operator()' for a (template
// parameter) 'TYPE' will call the 'hashAppend' free function for 'TYPE' and
// provide 'hashAppend' an instance of the 'HASH_ALGORITHM'.
//
// This component also contains 'hashAppend' definitions for fundamental types,
// which are required by algorithms deinfed in 'bslh'.  Clients are expected to
// define a free-function 'hashAppend' for each of the types they wish to be
// hashable (see {'hashAppend'} below).   More information can be found in the
// package level documentation for 'bslh' (internal users can also find
// information here {TEAM BDE:USING MODULAR HASHING<GO>})
//
///Modularity
///----------
// 'bslh::Hash' provides a modular system for hashing.  This modularity refers
// to the decoupling of the various tasks associated with hashing. Using this
// system, type implementers can identify attributes of their type that are
// salient to hashing, without having to write a hashing algorithm. Conversely,
// hashing algorithms can be written independant of types.  Attributes that are
// salient to hashing are called out on a type using 'hashAppend'.  Hashing
// algorithms are written to operate on the attributes called out by
// 'hashAppend'.  Some default algorithms have been provided in the 'bslh'
// package.  This modularity allows type creators to avoid writing hashing
// algorithms, which can save work and avoid bad hashing algorithm
// implementations.
//
///'hashAppend'
///------------
// 'hashAppend' is the function that is used to pass attributes that are
// salient to hashing into a hashing algorithm.  A type must define a
// 'hashAppend' overload that can be discovered through ADL in order to be
// hashed using this facility.  A simple implementation of an overload for
// 'hashAppend' might call 'hashAppend' on each of the type's attributes that
// are salient to hashing.  Note that when writing a 'hashAppend' function,
// 'using bslh::hashAppend;' must be included as the first line of code in the
// function. The using statement ensures that ADL will always be able to find
// the fundamental type 'hashAppend' functions, even when the (template
// parameter) type 'HASH_ALGORITHM' is not implemented in 'bslh'.
//
// Some types may require more subtle implementations
// for 'hashAppend', such as types containing C-strings which are salient to
// hashing.  These C-strings must be passed directly into the (template
// parameter) type 'HASH_ALGORITHM', rather than calling 'hashAppend' with the
// pointer as an argument.  This special case exists because calling
// 'hashAppend' with a pointer will hash the pointer rather than the data that
// is pointed to.
//
// Within this component, 'hashAppend' has been implemented for all of the
// fundamental types.  When 'hashAppend is reached on a fundamental type, the
// hashing algorithm is no longer propagated, and instead a pointer to the
// beginning of the type in memory is passed to the algorithm, along with the
// length of the type.  There are special cases with floating point numbers and
// bools where the data is tweaked before hashing to ensure that values that
// compare equal will be hashed with the same bit-wise representation.  The
// algorithm will then incorporate the type into its internal state and return
// a finalized hash when requested.
//
///Hashing Algorithms
///------------------
// There are algorithms implemented in the 'bslh' package that can be passed in
// and used as template parameters for 'bslh::Hash' or other structs like it.
// Some of these algorithms, such as 'bslh::SpookyHashAlgorithm', are named for
// the algorithm they implement.  These named algorithms are intended for use
// by those who want a specific algorithm.  There are other algorithms, such as
// 'bslh::DefaultHashAlgorithm', which wrap an unspecified algorithm and
// describe the properties of the wrapped algorithm.  The descriptive
// algorithms are intended for use by those who need specific properties and
// want to be updated to a new algorithm when one is published with
// improvements to the desired properties.  'bslh::DefaultHashAlgorithm' has
// the property of being a good default algorithm, specifically for use in a
// hash table.
//
///Requirements for Regular 'bslh' Hashing Algorithms
///--------------------------------------------------
// Users of this modular hashing system are free write their own hashing
// algorithms.  In order to plug into 'bslh::Hash', the user-implemented
// algorithms must implement the interface shown here:
//..
// class SomeHashAlgorithm
// {
//   public:
//     // TYPES
//     typedef Uint64 result_type;
//
//     // CREATORS
//     SomeHashAlgorithm();
//
//     // MANIPULATORS
//     void operator()(const void *data, size_t numBytes);
//
//     result_type computeHash();
// };
//..
// The 'result_type' 'typedef' must define the return type of this particular
// algorithm.  A default constructor (either implicit or explicit) must be
// supplied that creates an algorithm functor that is in a usable state.  An
// 'operator()' must be supplied that takes a 'const void *' to the data to be
// hashed and a 'size_t' length of bytes to be hashed.  This operator must
// operate on all data uniformly, meaning that regardless of whether data is
// passed in all at once, or one byte at a time, the result returned by
// 'computeHash()' will be the same.  'computeHash()' will return the final
// result of the hashing algorithm, as type 'result_type'.  'computeHash()' is
// allowed to modify the internal state of the algorithm, meaning calling
// 'computeHash()' more than once may not return the correct value.


#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLH_DEFAULTHASHALGORITHM
#include <bslh_defaulthashalgorithm.h>
#endif

#ifndef INCLUDED_BSLMF_ISBITWISEMOVEABLE
#include <bslmf_isbitwisemoveable.h>
#endif

#ifndef INCLUDED_BSLMF_ISSAME
#include <bslmf_issame.h>
#endif

#ifndef INCLUDED_BSLMF_ISTRIVIALLYCOPYABLE
#include <bslmf_istriviallycopyable.h>
#endif

#ifndef INCLUDED_BSLMF_ISTRIVIALLYDEFAULTCONSTRUCTIBLE
#include <bslmf_istriviallydefaultconstructible.h>
#endif

#ifndef INCLUDED_BSLS_COMPILERFEATURES
#include <bsls_compilerfeatures.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORM
#include <bsls_platform.h>
#endif

#ifndef INCLUDED_STDDEF_H
#include <stddef.h>  // for 'size_t'
#define INCLUDED_STDDEF_H
#endif

namespace BloombergLP {

namespace bslh {

                          // ================
                          // class bslh::Hash
                          // ================

template <class HASH_ALGORITHM = bslh::DefaultHashAlgorithm>
struct Hash {
    // This struct wraps the (template parameter) type 'HASH_ALGORITHM' in an
    // interface that satisfies the 'hash' requirements of the C++11 standard.

    // TYPES
    typedef size_t result_type;
        // The type of the hash value that will be returned by the
        // function-call operator.

    // CREATORS
    //! Hash() = default;
        // Create a 'Hash' object.

    //! Hash(const Hash& original) = default;
        // Create a 'Hash' object.  Note that as 'Hash' is an empty (stateless)
        // type, this operation will have no observable effect.

    //! ~Hash() = default;
        // Destroy this object.

    // MANIPULATORS
    //! Hash& operator=(const Hash& rhs) = default;
        // Assign to this object the value of the specified 'rhs' object, and
        // return a reference providing modifiable access to this object.  Note
        // that as 'Hash' is an empty (stateless) type, this operation will
        // have no observable effect.

    // ACCESSORS
    template <class TYPE>
    result_type operator()(const TYPE& type) const;
        // Returns a hash generated by the (template parameter) type
        // 'HASH_ALGORITHM' for the specified 'type'.  The value returned by the
        // 'HASH_ALGORITHM' is cast to 'size_t' before returning.

};

// FREE FUNCTIONS
template <class HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& hashAlg, bool input);
template <class HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& hashAlg, char input);
template <class HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& hashAlg, signed char input);
template <class HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& hashAlg, unsigned char input);
template <class HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& hashAlg, wchar_t input);
#if defined BSLS_COMPILERFEATURES_SUPPORT_UNICODE_CHAR_TYPES
template <class HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& hashAlg, char16_t input);
template <class HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& hashAlg, char32_t input);
#endif
template <class HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& hashAlg, short input);
template <class HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& hashAlg, unsigned short input);
template <class HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& hashAlg, int input);
template <class HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& hashAlg, unsigned int input);
template <class HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& hashAlg, long input);
template <class HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& hashAlg, unsigned long input);
template <class HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& hashAlg, long long input);
template <class HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& hashAlg, unsigned long long input);
template <class HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& hashAlg, float input);
template <class HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& hashAlg, double input);
template <class HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& hashAlg, long double input);
template <class HASH_ALGORITHM, size_t N>
void hashAppend(HASH_ALGORITHM& hashAlg, const char (&input)[N]);
template <class HASH_ALGORITHM, class TYPE, size_t N>
void hashAppend(HASH_ALGORITHM& hashAlg, const TYPE (&input)[N]);
template <class HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& hashAlg, const void *input);
#if defined BSLS_COMPILERFEATURES_SUPPORT_VARIADIC_TEMPLATES
template <class HASH_ALGORITHM, class RT, class ...  ARGS>
void hashAppend(HASH_ALGORITHM& hashAlg, RT (*input)(ARGS...));
#else
template <class HASH_ALGORITHM, class RT>
void hashAppend(HASH_ALGORITHM& hashAlg, RT (*input)());
template <class HASH_ALGORITHM, class RT, class T0>
void hashAppend(HASH_ALGORITHM& hashAlg, RT (*input)(T0));
template <class HASH_ALGORITHM, class RT, class T0, class T1>
void hashAppend(HASH_ALGORITHM& hashAlg, RT (*input)(T0, T1));
template <class HASH_ALGORITHM, class RT, class T0, class T1, class T2>
void hashAppend(HASH_ALGORITHM& hashAlg, RT (*input)(T0, T1, T2));
template <class HASH_ALGORITHM,
          class RT,
          class T0, class T1, class T2, class T3>
void hashAppend(HASH_ALGORITHM& hashAlg, RT (*input)(T0, T1, T2, T3) );
template <class HASH_ALGORITHM,
          class RT,
          class T0, class T1, class T2, class T3, class T4>
void hashAppend(HASH_ALGORITHM& hashAlg, RT (*input)(T0, T1, T2, T3, T4));
template <class HASH_ALGORITHM,
          class RT,
          class T0, class T1, class T2, class T3, class T4, class T5>
void hashAppend(HASH_ALGORITHM& hashAlg, RT (*input)(T0, T1, T2, T3, T4, T5));
template <class HASH_ALGORITHM,
          class RT,
          class T0, class T1, class T2, class T3, class T4, class T5, class T6>
void hashAppend(HASH_ALGORITHM& hashAlg,
                RT (*input)(T0, T1, T2, T3, T4, T5, T6));
template <class HASH_ALGORITHM,
          class RT,
          class T0, class T1, class T2, class T3, class T4, class T5, class T6,
          class T7>
void hashAppend(HASH_ALGORITHM& hashAlg,
                RT (*input)(T0, T1, T2, T3, T4, T5, T6, T7));
template <class HASH_ALGORITHM,
          class RT,
          class T0, class T1, class T2, class T3, class T4, class T5, class T6,
          class T7, class T8>
void hashAppend(HASH_ALGORITHM& hashAlg,
                RT (*input)(T0, T1, T2, T3, T4, T5, T6, T7, T8));
template <class HASH_ALGORITHM,
          class RT,
          class T0, class T1, class T2, class T3, class T4, class T5, class T6,
          class T7, class T8, class T9>
void hashAppend(HASH_ALGORITHM& hashAlg,
                RT (*input)(T0, T1, T2, T3, T4, T5, T6, T7, T8, T9));
    // Passes the specified 'input' into the specified 'hashAlg' to be combined
    // into the internal state of the algorithm which is used to produce the
    // resulting hash value.
#endif

}  // close package namespace

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

// ACCESSORS
template <class HASH_ALGORITHM>
template <class TYPE>
inline
typename bslh::Hash<HASH_ALGORITHM>::result_type
bslh::Hash<HASH_ALGORITHM>::operator()(TYPE const& key) const
{
    HASH_ALGORITHM hashAlg;
    hashAppend(hashAlg, key);
    return static_cast<result_type>(hashAlg.computeHash());
}

// FREE FUNCTIONS
template <class HASH_ALGORITHM>
inline
void bslh::hashAppend(HASH_ALGORITHM& hashAlg, bool input)
{
    // We need to ensure that any inputs that compare equal produce the same
    // hash.  Any non-zero binary representation of 'input' can be 'true', so we
    // need to normalize 'input' to ensure that we do not pass two different
    // binary representations of 'true' true into our hashing algorithm.
    unsigned char normalizedData = input;

    hashAlg(&normalizedData, sizeof(normalizedData));
}

template <class HASH_ALGORITHM>
inline
void bslh::hashAppend(HASH_ALGORITHM& hashAlg, char input)
{
    hashAlg(&input, sizeof(input));
}

template <class HASH_ALGORITHM>
inline
void bslh::hashAppend(HASH_ALGORITHM& hashAlg, signed char input)
{
    hashAlg(&input, sizeof(input));
}

template <class HASH_ALGORITHM>
inline
void bslh::hashAppend(HASH_ALGORITHM& hashAlg, unsigned char input)
{
    hashAlg(&input, sizeof(input));
}

template <class HASH_ALGORITHM>
inline
void bslh::hashAppend(HASH_ALGORITHM& hashAlg, wchar_t input)
{
    hashAlg(&input, sizeof(input));
}

#if defined BSLS_COMPILERFEATURES_SUPPORT_UNICODE_CHAR_TYPES
template <class HASH_ALGORITHM>
inline
void bslh::hashAppend(HASH_ALGORITHM& hashAlg, char16_t input)
{
    hashAlg(&input, sizeof(input));
}

template <class HASH_ALGORITHM>
inline
void bslh::hashAppend(HASH_ALGORITHM& hashAlg, char32_t input)
{
    hashAlg(&input, sizeof(input));
}
#endif

template <class HASH_ALGORITHM>
inline
void bslh::hashAppend(HASH_ALGORITHM& hashAlg, short input)
{
    hashAlg(&input, sizeof(input));
}

template <class HASH_ALGORITHM>
inline
void bslh::hashAppend(HASH_ALGORITHM& hashAlg, unsigned short input)
{
    hashAlg(&input, sizeof(input));
}

template <class HASH_ALGORITHM>
inline
void bslh::hashAppend(HASH_ALGORITHM& hashAlg, int input)
{
    hashAlg(&input, sizeof(input));
}

template <class HASH_ALGORITHM>
inline
void bslh::hashAppend(HASH_ALGORITHM& hashAlg, unsigned int input)
{
    hashAlg(&input, sizeof(input));
}

template <class HASH_ALGORITHM>
inline
void bslh::hashAppend(HASH_ALGORITHM& hashAlg, long input)
{
    hashAlg(&input, sizeof(input));
}

template <class HASH_ALGORITHM>
inline
void bslh::hashAppend(HASH_ALGORITHM& hashAlg, unsigned long input)
{
    hashAlg(&input, sizeof(input));
}

template <class HASH_ALGORITHM>
inline
void bslh::hashAppend(HASH_ALGORITHM& hashAlg, long long input)
{
    hashAlg(&input, sizeof(input));
}

template <class HASH_ALGORITHM>
inline
void bslh::hashAppend(HASH_ALGORITHM& hashAlg, unsigned long long input)
{
    hashAlg(&input, sizeof(input));
}

template <class HASH_ALGORITHM>
inline
void bslh::hashAppend(HASH_ALGORITHM& hashAlg, float input)
{
    if (input == 0.0f){
        input = 0;
    }
    hashAlg(&input, sizeof(input));
}

template <class HASH_ALGORITHM>
inline
void bslh::hashAppend(HASH_ALGORITHM& hashAlg, double input)
{
    if (input == 0.0){
        input = 0;
    }
    hashAlg(&input, sizeof(input));
}

template <class HASH_ALGORITHM>
inline
void bslh::hashAppend(HASH_ALGORITHM& hashAlg, long double input)
{
    if (input == 0.0l){
        input = 0;
    }

#if defined BSLS_PLATFORM_OS_LINUX   &&                                       \
    (defined BSLS_PLATFORM_CMP_GNU   || defined BSLS_PLATFORM_CMP_CLANG)
    // This needs to be done to work around issues when compiling with GCC and
    // Clang on Linux.  On 64-bit hardware, 'sizeof(long double)' is advertised
    // as 16 bytes, but only 10 bytes of precision is used.  The remaining 6
    // bytes are padding.
    //
    // For Clang, the final 2 bytes of the padding are zeroed, but the 4 bytes
    // that proceed the final two appear to be garbage.
    //
    //..
    //      Actual Data --+*****************************+
    //                    |                             |
    // Actual long double: 5d e9 79 a9 c2 82 bb ef 2b 40 87 d8 5c 2b  0  0
    //                                                   |          ||   |
    //      Garbage -------------------------------------+**********+|   |
    //      Zeroed --------------------------------------------------+***+
    //..
    //
    // For GCC, the first and last 2 bytes of the padding are zeroed, but the 2
    // bytes in the middle appear to be garbage.
    //
    //..
    //      Garbage -------------------------------------------+****+
    //      Actual Data --+*****************************+     |     |
    //                    |                             |     |     |
    // Actual long double: 5d e9 79 a9 c2 82 bb ef 2b 40  0  0 5c 2b  0  0
    //                                                   |    |      |    |
    //      Zeroed --------------------------------------+****+------+****+
    //..
    //
    // On 32-bit hardware, 'sizeof(long double)' is advertised as 12 bytes, but
    // again, only 10 bytes of precision is used.  The remaining 2 bytes are
    // padding.
    //
    // For Clang, the 2 bytes of the padding appear to be garbage.
    //
    //..
    //      Actual Data --+*****************************+
    //                    |                             |
    // Actual long double: 5d e9 79 a9 c2 82 bb ef 2b 40 87 d8
    //                                                   |    |
    //      Garbage -------------------------------------+****+
    //..
    //
    // For GCC, the 2 bytes of the padding are zeroed.
    //
    //..
    //      Actual Data --+*****************************+
    //                    |                             |
    // Actual long double: 5d e9 79 a9 c2 82 bb ef 2b 40  0  0
    //                                                   |    |
    //      Zeroed --------------------------------------+****+
    //..
    //
    // To address all of these issues, we will pass in only 10 bytes for a
    // 'long double' even if it is longer.

#if !defined(BSLS_PLATFORM_CMP_CLANG) && BSLS_PLATFORM_CPU_64_BIT
    // We cant just check 'defined(BSLS_PLATFORM_CMP_GNU)' because Clang
    // masquerades as GCC.  Since we know that to be in this block we must be
    // using GCC or Clang, we can just check
    // '!defined(BSLS_PLATFORM_CMP_CLANG)' to get the same result.

    if (bsl::is_same<long double, __float128>::value) {
        // We need to handle the posibility that somebody has set the GCC
        // compiler flag that makes 'long double' actually be 128-bit.
        hashAlg(&input, sizeof(input));
        return;                                                       // RETURN
    }
#endif
    hashAlg(&input, sizeof(input) > 10 ? 10 : sizeof(input));
#else
    hashAlg(&input, sizeof(input));
#endif
}

template <class HASH_ALGORITHM, size_t N>
inline
void bslh::hashAppend(HASH_ALGORITHM& hashAlg, const char (&input)[N])
{
    hashAlg(&input, sizeof(char)*N);
}

template <class HASH_ALGORITHM, class TYPE, size_t N>
inline
void bslh::hashAppend(HASH_ALGORITHM& hashAlg, const TYPE (&input)[N])
{
    for (size_t i = 0; i < N; ++i) {
        hashAppend(hashAlg, input[i]);
    }
}

template <class HASH_ALGORITHM>
inline
void bslh::hashAppend(HASH_ALGORITHM& hashAlg, const void *input)
{
    hashAlg(&input, sizeof(input));
}

#if defined BSLS_COMPILERFEATURES_SUPPORT_VARIADIC_TEMPLATES

template <class HASH_ALGORITHM, class RT, class ...  ARGS>
void bslh::hashAppend(HASH_ALGORITHM& hashAlg, RT (*input)(ARGS...))
{
    hashAlg(&input, sizeof(input));
}

#else

template <class HASH_ALGORITHM, class RT>
void bslh::hashAppend(HASH_ALGORITHM& hashAlg, RT (*input)())
{
    hashAlg(&input, sizeof(input));
}

template <class HASH_ALGORITHM, class RT, class T0>
void bslh::hashAppend(HASH_ALGORITHM& hashAlg, RT (*input)(T0))
{
    hashAlg(&input, sizeof(input));
}

template <class HASH_ALGORITHM, class RT, class T0, class T1>
void bslh::hashAppend(HASH_ALGORITHM& hashAlg, RT (*input)(T0, T1))
{
    hashAlg(&input, sizeof(input));
}

template <class HASH_ALGORITHM, class RT, class T0, class T1, class T2>
void bslh::hashAppend(HASH_ALGORITHM& hashAlg, RT (*input)(T0, T1, T2))
{
    hashAlg(&input, sizeof(input));
}

template <class HASH_ALGORITHM,
          class RT,
          class T0, class T1, class T2, class T3>
void bslh::hashAppend(HASH_ALGORITHM& hashAlg, RT (*input)(T0, T1, T2, T3) )
{
    hashAlg(&input, sizeof(input));
}

template <class HASH_ALGORITHM,
          class RT,
          class T0, class T1, class T2, class T3, class T4>
void bslh::hashAppend(HASH_ALGORITHM& hashAlg, RT (*input)(T0, T1, T2, T3, T4))
{
    hashAlg(&input, sizeof(input));
}

template <class HASH_ALGORITHM,
          class RT,
          class T0, class T1, class T2, class T3, class T4, class T5>
void bslh::hashAppend(HASH_ALGORITHM& hashAlg,
                      RT (*input)(T0, T1, T2, T3, T4, T5))
{
    hashAlg(&input, sizeof(input));
}

template <class HASH_ALGORITHM,
          class RT,
          class T0, class T1, class T2, class T3, class T4, class T5, class T6>
void bslh::hashAppend(HASH_ALGORITHM& hashAlg,
                RT (*input)(T0, T1, T2, T3, T4, T5, T6))
{
    hashAlg(&input, sizeof(input));
}

template <class HASH_ALGORITHM,
          class RT,
          class T0, class T1, class T2, class T3, class T4, class T5, class T6,
          class T7>
void bslh::hashAppend(HASH_ALGORITHM& hashAlg,
                RT (*input)(T0, T1, T2, T3, T4, T5, T6, T7))
{
    hashAlg(&input, sizeof(input));
}

template <class HASH_ALGORITHM,
          class RT,
          class T0, class T1, class T2, class T3, class T4, class T5, class T6,
          class T7, class T8>
void bslh::hashAppend(HASH_ALGORITHM& hashAlg,
                RT (*input)(T0, T1, T2, T3, T4, T5, T6, T7, T8))
{
    hashAlg(&input, sizeof(input));
}

template <class HASH_ALGORITHM,
          class RT,
          class T0, class T1, class T2, class T3, class T4, class T5, class T6,
          class T7, class T8, class T9>
void bslh::hashAppend(HASH_ALGORITHM& hashAlg,
                RT (*input)(T0, T1, T2, T3, T4, T5, T6, T7, T8, T9))
{
    hashAlg(&input, sizeof(input));
}

#endif

// ============================================================================
//                                TYPE TRAITS
// ============================================================================

// Type traits for STL 'hash'
//: o 'bsl::hash<TYPE>' is trivially default constructible.
//: o 'bsl::hash<TYPE>' is trivially copyable.
//: o 'bsl::hash<TYPE>' is bitwise movable.

namespace bslmf {
template <class TYPE>
struct IsBitwiseMoveable<bslh::Hash<TYPE> >
    : bsl::true_type {};
}  // close traits namespace


}  // close enterprise namespace

namespace bsl {
template <class TYPE>
struct is_trivially_default_constructible< ::BloombergLP::bslh::Hash<TYPE> >
: bsl::true_type
{};

template <class TYPE>
struct is_trivially_copyable< ::BloombergLP::bslh::Hash<TYPE> >
: bsl::true_type
{};
}  // close traits namespace



#endif

// ----------------------------------------------------------------------------
// Copyright (C) 2014 Bloomberg Finance L.P.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
// ----------------------------- END-OF-FILE ----------------------------------
