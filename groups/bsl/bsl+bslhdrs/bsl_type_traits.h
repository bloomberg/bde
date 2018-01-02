// bsl_type_traits.h                                                  -*-C++-*-
#ifndef INCLUDED_BSL_TYPE_TRAITS
#define INCLUDED_BSL_TYPE_TRAITS

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide functionality of the corresponding C++ Standard header.
//
//@SEE_ALSO: package bsl+stdhdrs
//
//@DESCRIPTION: Provide types, in the 'bsl' namespace, equivalent to those
// defined in the corresponding C++ standard header.  Include the native
// compiler-provided standard header, and also directly include Bloomberg's
// implementation of the C++ standard type (if one exists).  Finally, place the
// included symbols from the 'std' namespace (if any) into the 'bsl' namespace.

#ifndef INCLUDED_BSLS_NATIVESTD
#include <bsls_nativestd.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORM
#include <bsls_platform.h>
#endif

#if __cplusplus < 201103L \
    && (defined(BSLS_PLATFORM_CMP_SUN) || defined(BSLS_PLATFORM_CMP_IBM))
#   error This file requires compiler and library support for \
          the ISO C++ 2011 standard.
#endif

#include <type_traits>

// PLATFORM SUPPORT MACROS
// -----------------------
// The following macros diagnose whether the current platform supports the full
// set of C++11 type traits.  Although this should be detected by directly
// testing the version of the native standard library (which may deviate from
// the default tool-chain) we are using the compiler version as a proxy for the
// library, which is a good match on the platforms available to us for testing.
// Library-specific detection and support may follow as subsequent patch
// requests.

// Our default implementation assumes that a full set of C++11-conformant
// traits are not available; some standard library implementations omit them
// silently, with no indicator macros.

// BSL_TYPE_TRAITS_HAS_IS_TRIVIALLY_TRAITS
    // C++11 renamed a variety of 'has_trivial_*' traits to 'is_trivially_*'
    // to provide a more consistent naming convention for the standard traits.
    // Several standard library implementations took a while to catch up, after
    // initially promoting their TR1 traits implementation into 'std'.  This
    // macro tracks whether a given library version provides these traits.

// BSL_TYPE_TRAITS_HAS_ALIGNED_UNION
    // The 'aligned_union' type was added, removed, and then restored, over the
    // evolution of C++11.  This macro tracks whether a given library version
    // provides the type.

// BSL_TYPE_TRAITS_MININAL_VC2010_TRAITS
    // This macro is defined for a version of the standard library on Windows
    // that has very few of the standard traits.

// We then detect specific platforms that may have some elements of support,
// and define the support macro for just those cases.  We do not redefine the
// support macros if they are already defined, so that it is possible to define
// them on the compiler command line to 0 or 1 regardless of platform.

#if defined(BSLS_PLATFORM_CMP_MSVC)
#  if BSLS_PLATFORM_CMP_VERSION == 1600
#    ifndef   BSL_TYPE_TRAITS_MININAL_VC2010_TRAITS
#      define BSL_TYPE_TRAITS_MININAL_VC2010_TRAITS   1
#    endif
#  else
#    ifndef   BSL_TYPE_TRAITS_HAS_IS_TRIVIALLY_TRAITS
#      define BSL_TYPE_TRAITS_HAS_IS_TRIVIALLY_TRAITS 1
#    endif
#    ifndef   BSL_TYPE_TRAITS_HAS_ALIGNED_UNION
#      define BSL_TYPE_TRAITS_HAS_ALIGNED_UNION       1
#    endif
#  endif
#endif

#if defined(BSLS_PLATFORM_CMP_GNU)
#  if BSLS_PLATFORM_CMP_VERSION >= 50000
#    ifndef   BSL_TYPE_TRAITS_HAS_IS_TRIVIALLY_TRAITS
#      define BSL_TYPE_TRAITS_HAS_IS_TRIVIALLY_TRAITS 1
#    endif
#    ifndef   BSL_TYPE_TRAITS_HAS_ALIGNED_UNION
#      define BSL_TYPE_TRAITS_HAS_ALIGNED_UNION       1
#    endif
#  endif
#endif

#if defined(BSLS_PLATFORM_CMP_CLANG)
#  if defined(__APPLE_CC__) && __APPLE_CC__ >= 6000
#    ifndef   BSL_TYPE_TRAITS_HAS_IS_TRIVIALLY_TRAITS
#      define BSL_TYPE_TRAITS_HAS_IS_TRIVIALLY_TRAITS 1
#    endif
#    if defined(BSLS_COMPILERFEATURES_SUPPORT_VARIADIC_TEMPLATES)
#      ifndef   BSL_TYPE_TRAITS_HAS_ALIGNED_UNION
#        define BSL_TYPE_TRAITS_HAS_ALIGNED_UNION     1
#      endif
#    endif
#  endif
#endif

namespace bsl {
    // Import selected symbols into bsl namespace

    // 20.10.4.1, primary type categories:
    using native_std::is_union;

    // 20.10.4.2, composite type categories:
    using native_std::is_object;
    using native_std::is_scalar;
    using native_std::is_compound;

    // 20.10.4.3, type properties:
    using native_std::is_trivial;
    using native_std::is_standard_layout;
    using native_std::is_pod;
#if !BSL_TYPE_TRAITS_MININAL_VC2010_TRAITS
    using native_std::is_literal_type;
#endif
    using native_std::is_abstract;
    using native_std::is_signed;
    using native_std::is_unsigned;
#if !BSL_TYPE_TRAITS_MININAL_VC2010_TRAITS
    using native_std::is_constructible;
    using native_std::is_default_constructible;
    using native_std::is_move_constructible;
    using native_std::is_assignable;
    using native_std::is_copy_assignable;
    using native_std::is_move_assignable;
    using native_std::is_destructible;
#endif
#if BSL_TYPE_TRAITS_HAS_IS_TRIVIALLY_TRAITS
    using native_std::is_trivially_constructible;
    using native_std::is_trivially_copy_constructible;
    using native_std::is_trivially_move_constructible;
    using native_std::is_trivially_assignable;
    using native_std::is_trivially_copy_assignable;
    using native_std::is_trivially_move_assignable;
    using native_std::is_trivially_destructible;
#endif
#if !BSL_TYPE_TRAITS_MININAL_VC2010_TRAITS
    using native_std::is_nothrow_constructible;
    using native_std::is_nothrow_default_constructible;
    using native_std::is_nothrow_copy_constructible;
    using native_std::is_nothrow_assignable;
    using native_std::is_nothrow_copy_assignable;
    using native_std::is_nothrow_move_assignable;
# if !defined(BSLS_PLATFORM_CMP_GNU) || BSLS_PLATFORM_CMP_VERSION >= 40800
    using native_std::is_nothrow_destructible;
# endif
#endif
    using native_std::has_virtual_destructor;

    // 20.10.5, type property queries:
    using native_std::alignment_of;
    using native_std::rank;
    using native_std::extent;

    // 20.10.7.3, sign modifications:
    using native_std::make_signed;
    using native_std::make_unsigned;

    // 20.10.7.4, array modifications:
    using native_std::remove_extent;
    using native_std::remove_all_extents;

    // 20.10.7.6, other transformations:
    using native_std::aligned_storage;
#if BSL_TYPE_TRAITS_HAS_ALIGNED_UNION
    using native_std::aligned_union;
#endif
    using native_std::decay;
    using native_std::common_type;
#if !BSL_TYPE_TRAITS_MININAL_VC2010_TRAITS
    using native_std::underlying_type;
    using native_std::result_of;
#endif

#if __cplusplus >= 201402L
    using native_std::is_null_pointer;
    using native_std::is_final;
#endif

#if __cplusplus >= 201712L
    // 20.10.8, logical operator traits:
    using native_std::conjunction;
    using native_std::disjunction;
    using native_std::negation;
#endif


#if 0
    // These traits are provided by BDE, and have additional members for
    // Bloomberg legacy code still using the pre-standard interface.

    using native_std::is_void;
    using native_std::is_integral;
    using native_std::is_floating_point;
    using native_std::is_array;
    using native_std::is_pointer;
    using native_std::is_lvalue_reference;
    using native_std::is_rvalue_reference;
    using native_std::is_member_object_pointer;
    using native_std::is_member_function_pointer;
    using native_std::is_enum;
    using native_std::is_class;
    using native_std::is_function;
    using native_std::is_reference;
    using native_std::is_fundamental;
    using native_std::is_arithmetic;
    using native_std::is_member_pointer;
    using native_std::is_const;
    using native_std::is_volatile;
    using native_std::is_trivially_copyable;
    using native_std::is_empty;
    using native_std::is_polymorphic;
    using native_std::is_trivially_default_constructible;
    using native_std::is_same;
    using native_std::is_base_of;
    using native_std::is_convertible;
    using native_std::remove_const;
    using native_std::remove_volatile;
    using native_std::remove_cv;
    using native_std::add_const;
    using native_std::add_volatile;
    using native_std::add_cv;
    using native_std::remove_reference;
    using native_std::add_lvalue_reference;
    using native_std::add_rvalue_reference;
    using native_std::remove_pointer;
    using native_std::add_pointer;
    using native_std::enable_if;
    using native_std::conditional;
#endif
}  // close package namespace

// Include Bloomberg's implementation, unless compilation is configured to
// override native types in the 'std' namespace with Bloomberg's
// implementation, in which case the implementation file will be included by
// the Bloomberg supplied standard header file.

// standard traits
#ifndef BSL_OVERRIDES_STD

#include <bslmf_addconst.h>
#include <bslmf_addcv.h>
#include <bslmf_addlvaluereference.h>
#include <bslmf_addpointer.h>
#include <bslmf_addreference.h>
#include <bslmf_addrvaluereference.h>
#include <bslmf_addvolatile.h>
#include <bslmf_conditional.h>
#include <bslmf_enableif.h>
#include <bslmf_integralconstant.h>
#include <bslmf_isarithmetic.h>
#include <bslmf_isarray.h>
#include <bslmf_isclass.h>
#include <bslmf_isconst.h>
#include <bslmf_isconvertible.h>
#include <bslmf_iscopyconstructible.h>
#include <bslmf_isempty.h>
#include <bslmf_isenum.h>
#include <bslmf_isfloatingpoint.h>
#include <bslmf_isfunction.h>
#include <bslmf_isfundamental.h>
#include <bslmf_isintegral.h>
#include <bslmf_islvaluereference.h>
#include <bslmf_ismemberfunctionpointer.h>
#include <bslmf_ismemberobjectpointer.h>
#include <bslmf_ismemberpointer.h>
#include <bslmf_isnothrowmoveconstructible.h>
#include <bslmf_ispointer.h>
#include <bslmf_ispolymorphic.h>
#include <bslmf_isreference.h>
#include <bslmf_isrvaluereference.h>
#include <bslmf_issame.h>
#include <bslmf_istriviallycopyable.h>
#include <bslmf_istriviallydefaultconstructible.h>
#include <bslmf_isvoid.h>
#include <bslmf_isvolatile.h>
#include <bslmf_removeconst.h>
#include <bslmf_removecv.h>
#include <bslmf_removepointer.h>
#include <bslmf_removereference.h>
#include <bslmf_removevolatile.h>

#endif  // BSL_OVERRIDES_STD

#endif

// ----------------------------------------------------------------------------
// Copyright 2015 Bloomberg Finance L.P.
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
