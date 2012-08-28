// bslmf_istriviallycopyable.h                                        -*-C++-*-
#ifndef INCLUDED_BSLMF_ISTRIVIALLYCOPYABLE
#define INCLUDED_BSLMF_ISTRIVIALLYCOPYABLE

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a primitive type trait for bit-wise copyable classes.
//
//@CLASSES:
//  bsl::is_trivially_copyable<TYPE>: bit-wise copyable trait metafunction
//
//@SEE_ALSO:
//
//@AUTHOR: Pablo Halpern (phalpern)
//
//@DESCRIPTION:This component provides a single trait metafunction,
// 'bsl::is_trivially_copyable<TYPE>', which allows generic code to determine
// whether 'TYPE' can be destructively moved using 'memcpy'.  Given a pointer,
// 'p1', to an object of 'TYPE', and a pointer 'p2' of the same type pointing
// to allocated but uninitialized storage, a destructive move from 'p1' to
// 'p2' comprises the following pair of operations:
//..
//  new ((void*) p2) TYPE(*p1);  // Or new ((void*) p2) TYPE(std::move(*p1));
//  p1->~TYPE();
//..
// An object of a 'TYPE' is *bit-wise* *copyable*, if the above operation
// can be replaced by the following operation without affecting correctness:
//..
//  std::memcpy(p2, p1, sizeof(TYPE));
//..
// If 'bsl::is_trivially_copyable<TYPE>::value' inherits from 'true_type' for a
// given 'TYPE', then a generic algorithm can infer that 'TYPE' is bit-wise
// Copyable.
//
// This trait is used by various components for providing optimizations for
// types that can be bit-wise moved.  The major benefit of this trait is not
// for a single object but for an array of such types, as a loop of
// Copy/destroy operations can be replaced by a single call to 'memcpy'.
//
// 'bsl::is_trivially_copyable<TYPE>' will inherit from 'true_type' if 'TYPE'
// is a fundamental type, enumeration type, or pointer type.  Most user-defined
// classes are are bit-wise copyable, but generic code must assume that an
// arbrary 'TYPE' is not bitwise-copyable, as bit-wise moving a type that is
// not bit-wise copyable is likely to result in a dangling pointer.  Thus, it
// is necessary to explicitly associate the bit-wise copyable trait with a
// class (via template specialization or by use of the
// 'BSLMF_DECLARE_NESTED_TRAIT' macro) in order to generic algorithms to
// recognize that class as bit-wise copyable.
//
///What classes are not bit-wise copyable?
///---------------------------------------
// A class that has any of the following attributes is !not! bit-wise
// copyable:
//
//: o Its address is one of the salient attributes that comprise its value.
//: o It contains a pointer that might (directly or indirectly) point to
//:   itself or to one of its own members.  For example, a list implementation
//:   that includes an embedded sentinal node such that the last node in the
//:   list points back to the sentinal node within the list class object is
//:   not bit-wise copyable.
//: o Its constructor registers a pointer to itself in some static registry.
//: o It contains a data member that is not bit-wise copyable.
//
// Because of the destructive nature of a bit-wise move (the original object
// must be treated as uninitialized storage after the move), a class can be
// bit-wise copyable but not also bit-wise copiable.  For example, a class
// that contains a pointer to heap-allocated storage is generally bit-wise
// copyable.  The moved object simply refers to the same storage as the
// (defunct) original.  However a bit-wise copy of the same object would
// incorrectly cause the original and the copy to share the same
// heap-allocated storage.
//
///Usage
///-----

#ifndef INCLUDED_BSLMF_INTEGERCONSTANT
#include <bslmf_integerconstant.h>
#endif

#ifndef INCLUDED_BSLMF_ISFUNDAMENTAL
#include <bslmf_isfundamental.h>
#endif

#ifndef INCLUDED_BSLMF_ISENUM
#include <bslmf_isenum.h>
#endif

#ifndef INCLUDED_BSLMF_ISPOINTER
#include <bslmf_ispointer.h>
#endif

#ifndef INCLUDED_BSLMF_ISPOINTERTOMEMBER
#include <bslmf_ispointertomember.h>
#endif

#ifndef INCLUDED_BSLMF_DETECTNESTEDTRAIT
#include <bslmf_detectnestedtrait.h>
#endif

#ifndef INCLUDED_BSLMF_REMOVECVQ
#include <bslmf_removecvq.h>
#endif

namespace bsl {

template <typename TYPE>
struct is_trivially_copyable;

}

namespace BloombergLP {
namespace bslmf {

template <typename TYPE>
struct IsTriviallyCopyable_Imp
    : bsl::integer_constant<
                bool,
                IsFundamental<TYPE>::value
             || IsEnum<TYPE>::value
             || bsl::is_pointer<TYPE>::value
             || bslmf::IsPointerToMember<TYPE>::value
             || DetectNestedTrait<TYPE, bsl::is_trivially_copyable>::value>
{};

}  // close package namespace
}  // close enterprise namespace

namespace bsl {

template <typename TYPE>
struct is_trivially_copyable
: BloombergLP::bslmf::IsTriviallyCopyable_Imp<typename remove_cv<TYPE>::type>
{
    // Trait metafunction that determines whether the specified parameter
    // 'TYPE' is bit-wise Copyable.  If 'is_trivially_copyable<TYPE>' is
    // derived from 'true_type' then 'TYPE' is bit-wise Copyable.  Otherwise,
    // bit-wise moveability cannot be inferred for 'TYPE'.  This trait can be
    // associated with a bit-wise Copyable user-defined class by specializing
    // this class or by using the 'BSLMF_DECLARE_NESTED_TRAIT' macro.
};

template <typename TYPE>
struct is_trivially_copyable<TYPE&> : false_type
{
    // Specialization of that prevents associating the 'is_trivially_copyable'
    // trait with any reference type.
};

}

#endif // ! defined(INCLUDED_BSLMF_ISTRIVIALLYCOPYABLE)

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2012
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------

