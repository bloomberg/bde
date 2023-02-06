// bslmf_forwardingreftype.h                                          -*-C++-*-
#ifndef INCLUDED_BSLMF_FORWARDINGREFTYPE
#define INCLUDED_BSLMF_FORWARDINGREFTYPE

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a meta-function for determining a forwarding type.
//
//@CLASSES:
//  bslmf::ForwardingRefType: meta-function to determine forwarding type
//  bslmf::ForwardingRefTypeUtil: Namespace for forwarding functions
//
//@SEE_ALSO: bslmf_removecvq
//
//@DESCRIPTION: This component provides a meta function,
// 'bslmf::ForwardingReftype', that determines the forwarding type for a given
// template type 't_TYPE'.  A 'FowardingRefType' is type used as a parameter
// type in template functions that seek to "forward" their arguments to an
// underlying function in a way that is type safe and efficient.  This
// component also provides a utility class template,
// 'bslmf::ForwardingRefTypeUtil', that primarily supplies a function
// 'bslmf::ForwadingRefTypeUtil::forwardToTarget' that forwards an argument to
// another function.
//
// An argument 'v' of type 'T' can be passed as type
// 'ForwardingRefType<T>::Type' down an arbitrarily-long chain of function
// calls without ever calling 'std::forward'.  However, in order to avoid an
// extra copy as well as to select the correct overload and instantiation of
// the eventual target function, it should be converted back to a type that
// more closely resembles the original 'T' by calling
// 'ForwardingRefTypeUtil<T>::forwardToTarget(v)'.
//
// One optimization performed by this component is the early decay of arrays to
// pointers, preventing a proliferation of different template instantiations
// for every array size being used.  Although the outermost function may still
// be instantiated on the full array type, intermediate functions are all
// instantiated on the same pointer type, regardless of array size.  This decay
// also applies to reference-to-array types.  The user can recover the original
// array type when forwarding to the final consumer by using
// 'bslmf::ForwardingRefTypeUtil<T>::forwardToTarget()' (see below).
//
///Comparison To 'bslmf_forwardingtype'
///------------------------------------
// The components 'bslmf_forwardingtype' and 'bslmf_forwardingreftype' serve
// the same purpose but have small behavioral differences.  In general, we
// recommend 'bslmf_forwardingreftype' (the new component) in most contexts.
//
// Most notably, 'bslmf::ForwardingType' (the older class) forwards fundamental
// and pointer types by value, where as 'bslmf::ForwardingRefType' will forward
// fundamental and pointer types by const-reference.  For example,
// 'bslmf::ForwardingType<int>::Type' is 'int' where as
// 'bslmf::ForwardingRefType<int>::Type' is 'const int&'.  This applies to
// fundamental types, pointer types (including member-pointer types), and enum
// types (which we'll collectively call "basic types").  Forwarding these basic
// types by value was a performance optimization (and in some rare
// circumstances was hack needed by older compilers), which predated the
// standardization of many of the places where 'bslmf::ForwardingType' was used
// (function and bind components in particular).  The optimzation (potentially)
// being that passing an 'int' by value is more likely to be done through a
// register, where as passing by reference is more likely to require
// de-referencing memory.  Forwarding the types by const-reference, as the
// newer 'bslmf::ForwardingRefType' does', is generally simpler and more in
// line with the modern C++ standard.  Using 'bslmf::ForwardingRefType' avoids
// some awkward edge cases at the expense of a possible optimization in
// parameter passing.
//
///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: Direct Look at Metafunction Results
/// - - - - - - - - - - - - - - - - - - - - - - -
// In this example, we invoke 'ForwardingRefType' on a variety of types and
// look at the resulting 'Type' member:
//..
//  struct MyType {};
//  typedef MyType& MyTypeRef;
//
//  void main()
//      // Usage example.
//  {
//      typedef int                     T1;
//      typedef int&                    T2;
//      typedef const volatile double&  T3;
//      typedef const double &          T4;
//      typedef const float * &         T5;
//      typedef const float * const &   T6;
//      typedef MyType                  T7;
//      typedef const MyType&           T8;
//      typedef MyType&                 T9;
//      typedef MyType                 *T10;
//      typedef int                     T11[];
//      typedef int                     T12[3];
//
//      typedef const int&              EXP1;
//      typedef int&                    EXP2;
//      typedef const volatile double&  EXP3;
//      typedef const double &          EXP4;
//      typedef const float * &         EXP5;
//      typedef const float * const &   EXP6;
//      typedef const MyType&           EXP7;
//      typedef const MyType&           EXP8;
//      typedef MyType&                 EXP9;
//      typedef MyType * const &        EXP10;
//      typedef int * const &           EXP11;
//      typedef int * const &           EXP12;
//
//      using bslmf::ForwardingRefType;
//      ASSERT((bsl::is_same<ForwardingRefType<T1>::Type,  EXP1>::value));
//      ASSERT((bsl::is_same<ForwardingRefType<T2>::Type,  EXP2>::value));
//      ASSERT((bsl::is_same<ForwardingRefType<T3>::Type,  EXP3>::value));
//      ASSERT((bsl::is_same<ForwardingRefType<T4>::Type,  EXP4>::value));
//      ASSERT((bsl::is_same<ForwardingRefType<T5>::Type,  EXP5>::value));
//      ASSERT((bsl::is_same<ForwardingRefType<T6>::Type,  EXP6>::value));
//      ASSERT((bsl::is_same<ForwardingRefType<T7>::Type,  EXP7>::value));
//      ASSERT((bsl::is_same<ForwardingRefType<T8>::Type,  EXP8>::value));
//      ASSERT((bsl::is_same<ForwardingRefType<T9>::Type,  EXP9>::value));
//      ASSERT((bsl::is_same<ForwardingRefType<T10>::Type, EXP10>::value));
//      ASSERT((bsl::is_same<ForwardingRefType<T11>::Type, EXP11>::value));
//      ASSERT((bsl::is_same<ForwardingRefType<T12>::Type, EXP12>::value));
//  }
//..
//

#include <bslscm_version.h>

#include <bslmf_isarray.h>
#include <bslmf_isenum.h>
#include <bslmf_isfunction.h>
#include <bslmf_isfundamental.h>
#include <bslmf_ismemberpointer.h>
#include <bslmf_ispointer.h>
#include <bslmf_isreference.h>
#include <bslmf_isrvaluereference.h>
#include <bslmf_movableref.h>
#include <bslmf_removecv.h>
#include <bslmf_removereference.h>

#include <bsls_compilerfeatures.h>
#include <bsls_platform.h>

#include <stddef.h>

namespace BloombergLP {


namespace bslmf {

// FORWARD DECLARATIONS
template <class t_TYPE>
class ForwardingRefTypeUtil;
template <class t_TYPE, int k_CATEGORY> struct ForwardingRefType_Imp;

                        // ================================
                        // class ForwardingRefType_Category
                        // ================================

struct ForwardingRefType_Category {
    // This component-private struct provides a namespace for the type
    // dispatch category enumeration values.

    // CONSTANTS
    enum {
        e_LVALUE_REF,  // Lvalue reference
        e_MOVABLE_REF, // Movable (rvalue) reference
        e_FUNCTION,    // Function or pointer to function
        e_ARRAY,       // Array
        e_BASIC,       // Built-in, pointer, or enum type
        e_CLASS        // Class, struct or union
    };
};

                        // ================================
                        // class ForwardingRefType_Dispatch
                        // ================================

template <class t_TYPE>
class ForwardingRefType_Dispatch {
    // This component-private class template is a metafunction whose 'value'
    // member is the forwarding category for the specified 't_TYPE'.

    // PRIVATE TYPES
    typedef ForwardingRefType_Category Cat;  // Abbreviation

  public:
    enum {
        value = (MovableRefUtil::IsLvalueReference<t_TYPE>::value
                     ? Cat::e_LVALUE_REF
                 : MovableRefUtil::IsMovableReference<t_TYPE>::value
                     ? Cat::e_MOVABLE_REF
                 : bsl::is_function<t_TYPE>::value       ? Cat::e_FUNCTION
                 : bsl::is_array<t_TYPE>::value          ? Cat::e_ARRAY
                 : bsl::is_fundamental<t_TYPE>::value    ? Cat::e_BASIC
                 : bsl::is_pointer<t_TYPE>::value        ? Cat::e_BASIC
                 : bsl::is_member_pointer<t_TYPE>::value ? Cat::e_BASIC
                 : bsl::is_enum<t_TYPE>::value           ? Cat::e_BASIC
                                                         : Cat::e_CLASS)
    };
};

                        // =======================
                        // class ForwardingRefType
                        // =======================

template <class t_TYPE>
class ForwardingRefType
: private ForwardingRefType_Imp<t_TYPE,
                                ForwardingRefType_Dispatch<t_TYPE>::value> {
    // This template metafunction has a member 'Type' computed such that, for a
    // specified 't_TYPE' parameter, a function with argument of 't_TYPE' can
    // be called efficiently from another function (e.g., a wrapper) by
    // declaring the corresponding parameter of the other wrapper as 'typename
    // ForwardingRefType<t_TYPE>::Type'.  The 'Type' member is computed to
    // minimize the number of expensive copies while forwarding the arguments
    // as faithfully as possible.

    // PRIVATE TYPES
    typedef ForwardingRefType_Imp<t_TYPE,
                                  ForwardingRefType_Dispatch<t_TYPE>::value>
        Imp;

  public:
    // TYPES
    typedef typename Imp::Type Type;
        // The type that should be used to forward 't_TYPE' through a chain of
        // function calls.

    typedef typename Imp::TargetType TargetType;
        // The closest type used to "reconstitute" 't_TYPE' from
        // 'ForwardingRefType<t_TYPE>::Type'.  This type may differ from
        // 't_TYPE' through the addition of a reference.
};

                        // ===========================
                        // class ForwardingRefTypeUtil
                        // ===========================

template <class t_TYPE>
class ForwardingRefTypeUtil
: private ForwardingRefType_Imp<t_TYPE,
                                ForwardingRefType_Dispatch<t_TYPE>::value> {
    // Provide a namespace for the 'forwardToTarget' function.

    // PRIVATE TYPES
    typedef ForwardingRefType_Imp<t_TYPE,
                                  ForwardingRefType_Dispatch<t_TYPE>::value>
        Imp;

  public:
    // TYPES
    typedef typename Imp::TargetType TargetType;
        // The closest type used to "reconstitute" 't_TYPE' from
        // 'ForwardingRefType<t_TYPE>::Type'.  This type may differ from
        // 't_TYPE' through the addition of a reference.

    // CLASS METHODS
    // static TargetType forwardToTarget(ForwardingRefType<t_TYPE>::Type v);

    using Imp::forwardToTarget;
        // Return (for the specified 'v' parameter) 'std::forward<t_TYPE>(v)',
        // where 'v' is assumed to originally have been an argument of 't_TYPE'
        // after forwarding through an intermediate call chain.  Specifically,
        // if 't_TYPE' is an rvalue type, return an rvalue reference to 'v',
        // otherwise return 'v' unchanged, thus converting an rvalue copy into
        // an rvalue move when possible.  For compilers that do not support
        // rvalue references, return 'v' unchanged.  This function is intended
        // to be called to forward an argument to the final target function of
        // a forwarding call chain.  Note that this function is not intended
        // for use with 't_TYPE' parameters of 'volatile'-qualified rvalue
        // type, which are effectively unheard of in real code and have strange
        // and hard-to-understand rules.
};

// ============================================================================
//                              INLINE DEFINITIONS
// ============================================================================

// BDE_VERIFY pragma: push  // Relax some bde_verify rules in the imp section
// BDE_VERIFY pragma: -CD01 // Member function defined in class definition

                        // ===========================
                        // class ForwardingRefType_Imp
                        // ===========================

// PRIMARY TEMPLATE HAS NO DEFINITION

// PARTIAL SPECIALIZATIONS
template <class t_TYPE>
struct ForwardingRefType_Imp<t_TYPE,
                             ForwardingRefType_Category::e_LVALUE_REF> {
    // lvalue reference is forwarded unmodified.

    // TYPES
    typedef t_TYPE             Type;
    typedef t_TYPE             TargetType;

    // CLASS METHODS
    static TargetType forwardToTarget(Type v)
        // Return the specified 'v' argument.
    {
        return v;
    }
};

template <class t_TYPE>
struct ForwardingRefType_Imp<t_TYPE,
                             ForwardingRefType_Category::e_MOVABLE_REF> {
    // Rvalue reference is forwarded as a reference to const lvalue.

    // TYPES
    typedef typename MovableRefUtil::RemoveReference<t_TYPE>::type UnrefType;
    typedef const UnrefType&                                       Type;
    typedef t_TYPE                                                 TargetType;

    // CLASS METHODS
    static TargetType forwardToTarget(Type v)
    {
        // Since rvalues are forwarded as *const* lvalues, we must cast away
        // the constness before converting to an rvalue reference.  If 't_TYPE'
        // is a const reference, then the constness will be reinstated on
        // return.

        // We split this cast up into two lines because Visual Studio 2015 and
        // early versions of Visual Studio 2017 create a temporary in the
        // one-liner.
        UnrefType& result = const_cast<UnrefType&>(v);
        return MovableRefUtil::move(result);
    }
};

template <class t_TYPE>
struct ForwardingRefType_Imp<t_TYPE, ForwardingRefType_Category::e_FUNCTION> {
    // Function type is forwarded as function reference.

    // TYPES
    typedef t_TYPE& Type;
    typedef t_TYPE& TargetType;

    // CLASS METHODS
    static TargetType forwardToTarget(Type v)
        // Return the specified 'v' argument.
    {
        return v;
    }
};

template <class t_TYPE, size_t k_NUM_ELEMENTS>
struct ForwardingRefType_Imp<t_TYPE[k_NUM_ELEMENTS],
                             ForwardingRefType_Category::e_ARRAY> {
    // Array of known size and reference to array of known size is forwarded as
    // pointer to array element type.

    // TYPES
    typedef t_TYPE *const& Type;
    typedef t_TYPE (&TargetType)[k_NUM_ELEMENTS];

    // CLASS METHODS
    static TargetType forwardToTarget(Type v)
        // Return the specified 'v', cast to a reference to array.
    {
        return reinterpret_cast<TargetType>(*v);
    }
};

template <class t_TYPE>
struct ForwardingRefType_Imp<t_TYPE[], ForwardingRefType_Category::e_ARRAY> {
    // Array of unknown size and reference to array of unknown size is
    // forwarded as pointer to array element type.

    // TYPES
    typedef t_TYPE *const& Type;
    typedef t_TYPE       (&TargetType)[];

    // CLASS METHODS
    static TargetType forwardToTarget(Type v)
        // Return the specified 'v' argument cast to a reference to array of
        // unknown size.
    {
        return reinterpret_cast<TargetType>(*v);
    }
};

template <class t_TYPE>
struct ForwardingRefType_Imp<t_TYPE, ForwardingRefType_Category::e_BASIC> {
    // Rvalue of basic type is forwarded without any cv-qualifier removed.

    // TYPES
    typedef const t_TYPE& Type;
    typedef const t_TYPE& TargetType;

    // CLASS METHODS
    static TargetType forwardToTarget(Type v)
        // Return the specified 'v' argument cast to a 'const' reference to the
        // (template parameter) 't_TYPE'.
    {
        return v;
    }
};

template <class t_TYPE>
struct ForwardingRefType_Imp<t_TYPE, ForwardingRefType_Category::e_CLASS> {
    // Rvalue of user type (i.e., class or union) is forwarded as a const
    // reference.

    // TYPES
    typedef const t_TYPE& Type;
#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
    typedef MovableRef<t_TYPE>      TargetType;

    // CLASS METHODS
    static TargetType forwardToTarget(Type v)
        // Return the specified 'v' argument cast to a modifiable movable
        // reference.
    {
        // Since rvalues are forwarded as *const* lvalues, we must cast away
        // the constness before converting to an rvalue reference.  If 't_TYPE'
        // is a const reference, then the constness will be reinstated on
        // return.

# if defined(BSLS_PLATFORM_CMP_MSVC)
        // We use a C-style cast because Visual Studio 2013, 2015, and early
        // versions of Visual Studio 2017 create a temporary with various
        // formulations using C++ casts.
        return MovableRefUtil::move((t_TYPE&)(v));
#else
        // However, other platforms are known to complain about casting away
        // the 'const' qualifier in 'Type' (i.e., in 'const t_TYPE&') unless a
        // 'const_cast' is explicitly used.
        return MovableRefUtil::move(const_cast<t_TYPE&>(v));
# endif
    }
#else
    typedef const t_TYPE& TargetType;

    static TargetType forwardToTarget(Type v)
    {
        return v;
    }
#endif
};

// BDE_VERIFY pragma: pop  // Relax some bde_verify rules in the imp section

}  // close package namespace
}  // close enterprise namespace



#endif

// ----------------------------------------------------------------------------
// Copyright 2021 Bloomberg Finance L.P.
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
