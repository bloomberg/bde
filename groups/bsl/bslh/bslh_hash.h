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
//  bslh::Hash: Functor that runs 'bslh' hash algorithms on supported types.
//
//@SEE_ALSO:
//
//@DESCRIPTION: This component provides a templated 'struct', 'bslh::Hash',
// which provides hashing functionality. This struct is a drop in replacement
// for 'bsl::hash'.  'bslh::Hash' is a wrapper that adapts hashing algorithms
// from 'bslh' and 'hashAppend' free functions to match the interface of
// 'bsl::hash'.  This component also contains 'hashAppend' definitions for
// fundamental types, which are required to make the hashing algorithms in
// 'bslh' work.  For more details, see the following pages:
// https://cms.prod.bloomberg.com/team/pages/viewpage.action?title=
// Modular+Hashing&spaceKey=bde
// https://cms.prod.bloomberg.com/team/pages/viewpage.action?title=
// Using+Modular+Hashing&spaceKey=bde
//
///Modularity
///----------
// 'bslh::Hash' provides a modular system for hashing.  Identification of
// attributes on a type that are salient to hashing and the actual
// implementation of hashing algorithms can be decoupled.  Attributes that are
// salient to hashing can be called out on a type using 'hashAppend'.  Hashing
// algorithms can be written to operate on the attributes called out by
// 'hashAppend'.  Some default algorithms have been provided in the 'bslh'
// package.  This modularity allows type creators to avoid writing hashing
// algorithms, which can save work avoid bad hashing algorithm implementations.
//
///'hashAppend'
///------------
// 'hashAppend' is the function that is used to pass attributes that are
// salient to hashing into a hashing algorithm.  A type must define a
// 'hashAppend' overload that can be discovered through ADL in order to be
// hashed using this facility.  The simplest 'hashAppend' overload will call
// 'hashAppend' on each of the type's attributes that are salient to hashing.
// Other 'hashAppend' implementations are possible, such as for the special
// case of c-strings where the data contained in the string must be passed
// directly into the algorithm, rather than calling 'hashAppend' on the
// pointer.  This special case exists becuase calling 'hashAppend' on a pointer
// will hash the pointer rather than the data that is pointed to.
//
// Within this component, 'hashAppend' has been implemented for all of the
// fundamental types. When 'hashAppend is reached on a fundamental type, the
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
//     void operator()(const void * key, size_t len);
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
//
// More information is availible at:
// https://cms.prod.bloomberg.com/team/pages/viewpage.action?title=
// Using+Modular+Hashing&spaceKey=bde


#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLH_DEFAULTHASHALGORITHM
#include <bslh_defaulthashalgorithm.h>
#endif

#ifndef INCLUDED_BSLMF_ISBITWISEMOVEABLE
#include <bslmf_isbitwisemoveable.h>
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
        // 'HASH_ALGORITHM' for the specified 'type'. The value returned by the
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
template <class HASH_ALGORITHM, class RT, class ... ARGS>
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
    // hash. Any non-zero binary representation of 'input' can be 'true', so we
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
    if (input == 0){
        input = 0;
    }
    hashAlg(&input, sizeof(input));
}

template <class HASH_ALGORITHM>
inline
void bslh::hashAppend(HASH_ALGORITHM& hashAlg, double input)
{
    if (input == 0){
        input = 0;
    }
    hashAlg(&input, sizeof(input));
}

template <class HASH_ALGORITHM>
inline
void bslh::hashAppend(HASH_ALGORITHM& hashAlg, long double input)
{
    if (input == 0){
        input = 0;
    }
    hashAlg(&input, sizeof(input));
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

template <class HASH_ALGORITHM, class RT, class ... ARGS>
void hashAppend(HASH_ALGORITHM& hashAlg, RT (*input)(ARGS...))
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
