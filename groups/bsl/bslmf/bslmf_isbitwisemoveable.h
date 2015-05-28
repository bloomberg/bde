// bslmf_isbitwisemoveable.h                                          -*-C++-*-
#ifndef INCLUDED_BSLMF_ISBITWISEMOVEABLE
#define INCLUDED_BSLMF_ISBITWISEMOVEABLE

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a primitive type trait for bitwise moveable classes.
//
//@CLASSES:
//  bslmf::IsBitwiseMoveable<TYPE>: bitwise moveable trait metafunction
//
//@SEE_ALSO:
//
//@DESCRIPTION: This component provides a single trait metafunction,
// 'bslmf::IsBitwiseMoveable<TYPE>', which allows generic code to determine
// whether 'TYPE' can be destructively moved using 'memcpy'.  Given a pointer,
// 'p1', to an object of 'TYPE', and a pointer 'p2' of the same type pointing
// to allocated but uninitialized storage, a destructive move from 'p1' to
// 'p2' comprises the following pair of operations:
//..
//  new ((void*) p2) TYPE(*p1);  // Or new ((void*) p2) TYPE(std::move(*p1));
//  p1->~TYPE();
//..
// An object of a 'TYPE' is *bitwise* *moveable*, if the above operation
// can be replaced by the following operation without affecting correctness:
//..
//  std::memcpy(p2, p1, sizeof(TYPE));
//..
// If 'IsBitwiseMoveable<TYPE>::value' inherits from 'true_type' for a given
// 'TYPE', then a generic algorithm can infer that 'TYPE' is bitwise
// moveable.
//
// This trait is used by various components for providing optimizations for
// types that can be bitwise moved.  The major benefit of this trait is not
// for a single object but for an array of such types, as a loop of
// copy/destroy operations can be replaced by a single call to 'memcpy'.
//
// 'IsBitwiseMoveable<TYPE>' will inherit from 'true_type' if 'TYPE' is a
// fundamental type, enumeration type, or pointer type.  Most user-defined
// classes are are bitwise moveable, but generic code must assume that an
// arbitrary 'TYPE' is not bitwise-moveable, as bitwise moving a type that is
// not bitwise moveable is likely to result in a dangling pointer.  Thus, it
// is necessary to explicitly associate the bitwise moveable trait with a
// class (via template specialization or by use of the
// 'BSLMF_DECLARE_NESTED_TRAIT' macro) in order to generic algorithms to
// recognize that class as bitwise moveable.
//
///What classes are not bitwise moveable?
///---------------------------------------
// A class that has any of the following attributes is !not! bitwise
// moveable:
//
//: o Its address is one of the salient attributes that comprise its value.
//: o It contains a pointer that might (directly or indirectly) point to
//:   itself or to one of its own members.  For example, a list implementation
//:   that includes an embedded sentinel node such that the last node in the
//:   list points back to the sentinel node within the list class object is
//:   not bitwise moveable.
//: o Its constructor registers a pointer to itself in some static registry.
//: o It contains a data member that is not bitwise moveable.
//
// Because of the destructive nature of a bitwise move (the original object
// must be treated as uninitialized storage after the move), a class can be
// bitwise moveable but not also bitwise copyable.  For example, a class
// that contains a pointer to heap-allocated storage is generally bitwise
// moveable.  The moved object simply refers to the same storage as the
// (defunct) original.  However a bitwise copy of the same object would
// incorrectly cause the original and the copy to share the same
// heap-allocated storage.
//
///Usage
///-----

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLMF_DETECTNESTEDTRAIT
#include <bslmf_detectnestedtrait.h>
#endif

#ifndef INCLUDED_BSLMF_INTEGRALCONSTANT
#include <bslmf_integralconstant.h>
#endif

#ifndef INCLUDED_BSLMF_ISENUM
#include <bslmf_isenum.h>
#endif

#ifndef INCLUDED_BSLMF_ISFUNDAMENTAL
#include <bslmf_isfundamental.h>
#endif

#ifndef INCLUDED_BSLMF_ISREFERENCE
#include <bslmf_isreference.h>
#endif

#ifndef INCLUDED_BSLMF_ISTRIVIALLYCOPYABLE
#include <bslmf_istriviallycopyable.h>
#endif

#ifndef INCLUDED_BSLMF_REMOVECV
#include <bslmf_removecv.h>
#endif

namespace BloombergLP {

namespace bslmf {

template <class TYPE>
struct IsBitwiseMoveable;

template <class TYPE>
struct IsBitwiseMoveable_Imp
    : bsl::integral_constant<bool,
                            !bsl::is_reference<TYPE>::value
                         && (  bsl::is_trivially_copyable<TYPE>::value
                            || DetectNestedTrait<TYPE,
                                                 IsBitwiseMoveable>::value)>
{};

                        // ========================
                        // struct IsBitwiseMoveable
                        // ========================

template <class TYPE>
struct IsBitwiseMoveable
   : IsBitwiseMoveable_Imp<typename bsl::remove_cv<TYPE>::type>::type
{
    // Trait metafunction that determines whether the specified parameter
    // 'TYPE' is bitwise moveable.  If 'IsBitwiseMoveable<TYPE>' is derived
    // from 'bsl::true_type' then 'TYPE' is bitwise moveable.  Otherwise,
    // bitwise moveability cannot be inferred for 'TYPE'.  This trait can be
    // associated with a bitwise moveable user-defined class by specializing
    // this class or by using the 'BSLMF_NESTED_TRAIT_DECLARATION' macro.
};

}  // close package namespace

}  // close namespace BloombergLP

#endif // ! defined(INCLUDED_BSLMF_ISBITWISEMOVEABLE)

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
