// bdlat_typetraits.h                                                 -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BDLAT_TYPETRAITS
#define INCLUDED_BDLAT_TYPETRAITS

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide compile-time traits for generated types.
//
//@CLASSES:
//  bdlat_IsBasicChoice<TYPE>:           true for generated "choice" types
//  bdlat_IsBasicSequence<TYPE>:         true for generated "sequence" types
//  bdlat_IsBasicEnumeration<TYPE>:      true for generated "enumeration" types
//  bdlat_BasicEnumerationWrapper<TYPE>: characteristics of enumeration type
//  bdlat_IsBasicCustomizedType<TYPE>:   true for generated "customized" types
//
//@MACROS:
//  BDLAT_DECL_CHOICE_TRAITS(ClassName)
//  BDLAT_DECL_CHOICE_WITH_ALLOCATOR_TRAITS(ClassName)
//  BDLAT_DECL_CHOICE_WITH_BITWISEMOVEABLE_TRAITS(ClassName)
//  BDLAT_DECL_CHOICE_WITH_ALLOCATOR_BITWISEMOVEABLE_TRAITS(ClassName)
//  BDLAT_DECL_SEQUENCE_TRAITS(ClassName)
//  BDLAT_DECL_SEQUENCE_WITH_ALLOCATOR_TRAITS(ClassName)
//  BDLAT_DECL_SEQUENCE_WITH_BITWISEMOVEABLE_TRAITS(ClassName)
//  BDLAT_DECL_SEQUENCE_WITH_ALLOCATOR_BITWISEMOVEABLE_TRAITS(ClassName)
//  BDLAT_DECL_ENUMERATION_TRAITS(ClassName)
//  BDLAT_DECL_CUSTOMIZEDTYPE_TRAITS(ClassName)
//  BDLAT_DECL_CUSTOMIZEDTYPE_WITH_ALLOCATOR_TRAITS(ClassName)
//  BDLAT_DECL_CUSTOMIZEDTYPE_WITH_BITWISEMOVEABLE_TRAITS(ClassName)
//  BDLAT_DECL_CUSTOMIZEDTYPE_WITH_ALLOCATOR_BITWISEMOVEABLE_TRAITS(ClassName)
//
//@SEE_ALSO:
//
//@DESCRIPTION: This component provides compile-time traits that pertain to
// generated types.  The four traits provided, 'bdlat_IsBasicChoice',
// 'bdlat_IsBasicSequence', 'bdlat_IsBasicEnumeration',
// 'bdlat_IsBasicCustomizedType' are used for generated "choice" types,
// "sequence" types, "enumeration", and "customized type" types respectively.
//
///Usage
///-----
// This component is intended to be used by components generated using the
// 'bas_codegen.pl' tool.

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

#ifndef INCLUDED_BDLB_PRINTMETHODS
#include <bdlb_printmethods.h>      // for 'bdlb::TypeTraitHasPrintMethod'
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITBITWISEMOVEABLE
#include <bslalg_typetraitbitwisemoveable.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITS
#include <bslalg_typetraits.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITUSESBSLMAALLOCATOR
#include <bslalg_typetraitusesbslmaallocator.h>
#endif

#ifndef INCLUDED_BDLAT_BDEATOVERRIDES
#include <bdlat_bdeatoverrides.h>
#endif


namespace BloombergLP {

template <class TYPE>
struct bdlat_IsBasicChoice :
        bslmf::DetectNestedTrait<TYPE, bdlat_IsBasicChoice>::type
{
    // This trait may be declared for "choice" types.
};

template <class TYPE>
struct bdlat_IsBasicSequence :
        bslmf::DetectNestedTrait<TYPE, bdlat_IsBasicSequence>::type
{
    // This trait may be declared for "sequence" types.
};

template <class TYPE>
struct bdlat_IsBasicEnumeration :
        bslmf::DetectNestedTrait<TYPE, bdlat_IsBasicEnumeration>::type
{
    // This trait may be declared for "enumeration" types.
};

template <class TYPE>
struct bdlat_IsBasicCustomizedType :
        bslmf::DetectNestedTrait<TYPE, bdlat_IsBasicCustomizedType>::type
{
    // This trait may be declared for "customized type" types.
};

template <class TYPE>
struct bdlat_BasicEnumerationWrapper;
    // This template has no definition for most types.  However for types that
    // meet the requirements of 'BasicEnumeration', this template is
    // specialized to inherit from the wrapper class that defines functions
    // like 'toString' and 'toInt'.  The specialization should also define the
    // type 'Wrapper' to be the name of the wrapper class.

struct bdlat_TypeTraitBasicChoice {
    // Legacy adaptor for bdlat_IsBasicChoice.

    template <class TYPE>
    struct NestedTraitDeclaration :
        bslmf::NestedTraitDeclaration<TYPE, bdlat_IsBasicChoice>
    {
        // This class template ties the 'bdlat_TypeTraitBasicChoice' trait tag
        // to the 'bdlat_IsBasicChoice' trait metafunction.
    };

    template <class TYPE>
    struct Metafunction : bdlat_IsBasicChoice<TYPE>::type { };
};

struct bdlat_TypeTraitBasicSequence {
    // Legacy adaptor for bdlat_IsBasicSequence.

    template <class TYPE>
    struct NestedTraitDeclaration :
        bslmf::NestedTraitDeclaration<TYPE, bdlat_IsBasicSequence>
    {
        // This class template ties the 'bdlat_TypeTraitBasicSequence' trait
        // tag to the 'bdlat_IsBasicSequence' trait metafunction.
    };

    template <class TYPE>
    struct Metafunction : bdlat_IsBasicSequence<TYPE>::type { };
};

struct bdlat_TypeTraitBasicEnumeration {
    // Legacy adaptor for bdlat_IsBasicEnumeration.

    template <class TYPE>
    struct NestedTraitDeclaration :
        bslmf::NestedTraitDeclaration<TYPE, bdlat_IsBasicEnumeration>
    {
        // This class template ties the 'bdlat_TypeTraitBasicEnumeration' trait
        // tag to the 'bdlat_IsBasicEnumeration' trait metafunction.
    };

    template <class TYPE>
    struct Metafunction : bdlat_IsBasicEnumeration<TYPE>::type { };
};

struct bdlat_TypeTraitBasicCustomizedType {
    // Legacy adaptor for bdlat_IsBasicCustomizedType.

    template <class TYPE>
    struct NestedTraitDeclaration :
        bslmf::NestedTraitDeclaration<TYPE, bdlat_IsBasicCustomizedType>
    {
        // This class template ties the 'bdlat_TypeTraitBasicCustomizedType'
        // trait tag to the 'bdlat_IsBasicCustomizedType' trait metafunction.
    };

    template <class TYPE>
    struct Metafunction : bdlat_IsBasicCustomizedType<TYPE>::type { };
};

// Macros defined to facilitate declaring traits for generated code.

// BDE_VERIFY pragma: -SLM01  // leaking macros is the whole point!

#define BDLAT_DECL_CHOICE_TRAITS(ClassName)                                   \
    template <>                                                               \
    struct bdlat_IsBasicChoice<ClassName> : bsl::true_type { };               \
    namespace bdlb {                                                          \
        template <>                                                           \
        struct HasPrintMethod<ClassName> : bsl::true_type { };                \
    }

#define BDLAT_DECL_CHOICE_WITH_ALLOCATOR_TRAITS(ClassName)                    \
    namespace bslma {                                                         \
        template <>                                                           \
        struct UsesBslmaAllocator<ClassName> : bsl::true_type { };            \
    }                                                                         \
    BDLAT_DECL_CHOICE_TRAITS(ClassName)

#define BDLAT_DECL_CHOICE_WITH_BITWISEMOVEABLE_TRAITS(ClassName)              \
    namespace bslmf {                                                         \
        template <>                                                           \
        struct IsBitwiseMoveable<ClassName> : bsl::true_type { };             \
    }                                                                         \
    BDLAT_DECL_CHOICE_TRAITS(ClassName)

#define BDLAT_DECL_CHOICE_WITH_ALLOCATOR_BITWISEMOVEABLE_TRAITS(ClassName)    \
    namespace bslma {                                                         \
        template <>                                                           \
        struct UsesBslmaAllocator<ClassName> : bsl::true_type { };            \
    }                                                                         \
    namespace bslmf {                                                         \
        template <>                                                           \
        struct IsBitwiseMoveable<ClassName> : bsl::true_type { };             \
    }                                                                         \
    BDLAT_DECL_CHOICE_TRAITS(ClassName)

#define BDLAT_DECL_SEQUENCE_TRAITS(ClassName)                                 \
    template <>                                                               \
    struct bdlat_IsBasicSequence<ClassName> : bsl::true_type { };             \
    namespace bdlb {                                                          \
        template <>                                                           \
        struct HasPrintMethod<ClassName> : bsl::true_type { };                \
    }

#define BDLAT_DECL_SEQUENCE_WITH_ALLOCATOR_TRAITS(ClassName)                  \
    namespace bslma {                                                         \
        template <>                                                           \
        struct UsesBslmaAllocator<ClassName> : bsl::true_type { };            \
    }                                                                         \
    BDLAT_DECL_SEQUENCE_TRAITS(ClassName)

#define BDLAT_DECL_SEQUENCE_WITH_BITWISEMOVEABLE_TRAITS(ClassName)            \
    namespace bslmf {                                                         \
        template <>                                                           \
        struct IsBitwiseMoveable<ClassName> : bsl::true_type { };             \
    }                                                                         \
    BDLAT_DECL_SEQUENCE_TRAITS(ClassName)

#define BDLAT_DECL_SEQUENCE_WITH_ALLOCATOR_BITWISEMOVEABLE_TRAITS(ClassName)  \
    namespace bslma {                                                         \
        template <>                                                           \
        struct UsesBslmaAllocator<ClassName> : bsl::true_type { };            \
    }                                                                         \
    namespace bslmf {                                                         \
        template <>                                                           \
        struct IsBitwiseMoveable<ClassName> : bsl::true_type { };             \
    }                                                                         \
    BDLAT_DECL_SEQUENCE_TRAITS(ClassName)

#define BDLAT_DECL_ENUMERATION_TRAITS(ClassName)                              \
    namespace bslmf {                                                         \
        template <>                                                           \
        struct IsBitwiseMoveable<ClassName::Value> : bsl::true_type { };      \
    }                                                                         \
    template <>                                                               \
    struct bdlat_IsBasicEnumeration<ClassName::Value> : bsl::true_type { };   \
    template <>                                                               \
    struct bdlat_BasicEnumerationWrapper<ClassName::Value> : ClassName {      \
        typedef ClassName Wrapper;                                            \
    };

#define BDLAT_DECL_CUSTOMIZEDTYPE_TRAITS(ClassName)                           \
    template <>                                                               \
    struct bdlat_IsBasicCustomizedType<ClassName> : bsl::true_type { };       \
    namespace bdlb {                                                          \
        template <>                                                           \
        struct HasPrintMethod<ClassName> : bsl::true_type { };                \
    }

#define BDLAT_DECL_CUSTOMIZEDTYPE_WITH_ALLOCATOR_TRAITS(ClassName)            \
    namespace bslma {                                                         \
        template <>                                                           \
        struct UsesBslmaAllocator<ClassName> : bsl::true_type { };            \
    }                                                                         \
    BDLAT_DECL_CUSTOMIZEDTYPE_TRAITS(ClassName)

#define BDLAT_DECL_CUSTOMIZEDTYPE_WITH_BITWISEMOVEABLE_TRAITS(ClassName)      \
    namespace bslmf {                                                         \
        template <>                                                           \
        struct IsBitwiseMoveable<ClassName> : bsl::true_type { };             \
    }                                                                         \
    BDLAT_DECL_CUSTOMIZEDTYPE_TRAITS(ClassName)

#define                                                                       \
   BDLAT_DECL_CUSTOMIZEDTYPE_WITH_ALLOCATOR_BITWISEMOVEABLE_TRAITS(ClassName) \
    namespace bslma {                                                         \
        template <>                                                           \
        struct UsesBslmaAllocator<ClassName> : bsl::true_type { };            \
    }                                                                         \
    namespace bslmf {                                                         \
        template <>                                                           \
        struct IsBitwiseMoveable<ClassName> : bsl::true_type { };             \
    }                                                                         \
    BDLAT_DECL_CUSTOMIZEDTYPE_TRAITS(ClassName)

}  // close enterprise namespace

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
