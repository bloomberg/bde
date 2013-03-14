// bdeat_typetraits.h                  -*-C++-*-
#ifndef INCLUDED_BDEAT_TYPETRAITS
#define INCLUDED_BDEAT_TYPETRAITS

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide compile-time traits for generated types.
//
//@CLASSES:
//  bdeat_IsBasicChoice<TYPE>:           true for generated "choice" types
//  bdeat_IsBasicSequence<TYPE>:         true for generated "sequence" types
//  bdeat_IsBasicEnumeration<TYPE>:      true for generated "enumeration" types
//  bdeat_BasicEnumerationWrapper<TYPE>: chararacteristics of enumeration type
//  bdeat_IsBasicCustomizedType<TYPE>:   true for generated "customized" types
//
//@MACROS:
//  BDEAT_DECL_CHOICE_TRAITS(ClassName)
//  BDEAT_DECL_CHOICE_WITH_ALLOCATOR_TRAITS(ClassName)
//  BDEAT_DECL_CHOICE_WITH_BITWISEMOVEABLE_TRAITS(ClassName)
//  BDEAT_DECL_CHOICE_WITH_ALLOCATOR_BITWISEMOVEABLE_TRAITS(ClassName)
//  BDEAT_DECL_SEQUENCE_TRAITS(ClassName)
//  BDEAT_DECL_SEQUENCE_WITH_ALLOCATOR_TRAITS(ClassName)
//  BDEAT_DECL_SEQUENCE_WITH_BITWISEMOVEABLE_TRAITS(ClassName)
//  BDEAT_DECL_SEQUENCE_WITH_ALLOCATOR_BITWISEMOVEABLE_TRAITS(ClassName)
//  BDEAT_DECL_ENUMERATION_TRAITS(ClassName)
//  BDEAT_DECL_CUSTOMIZEDTYPE_TRAITS(ClassName)
//  BDEAT_DECL_CUSTOMIZEDTYPE_WITH_ALLOCATOR_TRAITS(ClassName)
//  BDEAT_DECL_CUSTOMIZEDTYPE_WITH_BITWISEMOVEABLE_TRAITS(ClassName)
//  BDEAT_DECL_CUSTOMIZEDTYPE_WITH_ALLOCATOR_BITWISEMOVEABLE_TRAITS(ClassName)
//
//@SEE_ALSO:
//
//@AUTHOR: Clay Wilson (cwilson9), Shezan Baig (sbaig)
//
//@CONTACT: Rohan Bhindwale (rbhindwa)
//
//@DESCRIPTION: This component provides compile-time traits that pertain to
// generated types.  The four traits provided, 'bdeat_IsBasicChoice',
// 'bdeat_IsBasicSequence', 'bdeat_IsBasicEnumeration',
// 'bdeat_IsBasicCustomizedType' are used for generated "choice" types,
// "sequence" types, "enumeration", and "customized type" types respectively.
//
///Usage
///-----
// This component is intended to be used by components generated using the
// 'bas_codegen.pl' tool.

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BDEU_PRINTMETHODS
#include <bdeu_printmethods.h>      // for 'bdeu_TypeTraitHasPrintMethod'
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


namespace BloombergLP {

template <class TYPE>
struct bdeat_IsBasicChoice :
        bslmf::DetectNestedTrait<TYPE, bdeat_IsBasicChoice>::type
{
    // This trait may be declared for "choice" types.
};

template <class TYPE>
struct bdeat_IsBasicSequence :
        bslmf::DetectNestedTrait<TYPE, bdeat_IsBasicSequence>::type
{
    // This trait may be declared for "sequence" types.
};

template <class TYPE>
struct bdeat_IsBasicEnumeration :
        bslmf::DetectNestedTrait<TYPE, bdeat_IsBasicEnumeration>::type
{
    // This trait may be declared for "enumeration" types.
};

template <class TYPE>
struct bdeat_IsBasicCustomizedType :
        bslmf::DetectNestedTrait<TYPE, bdeat_IsBasicCustomizedType>::type
{
    // This trait may be declared for "customized type" types.
};

template <class TYPE>
struct bdeat_BasicEnumerationWrapper;
    // This template has no definition for most types.  However for types that
    // meet the requirements of 'BasicEnumeration', this template is
    // specialized to inherit from the wrapper class that defines functions
    // like 'toString' and 'toInt'.  The specialization should also define the
    // type 'Wrapper' to be the name of the wrapper class.

struct bdeat_TypeTraitBasicChoice {
    // Legacy adaptor for bdeat_IsBasicChoice.

    template <class TYPE>
    struct NestedTraitDeclaration :
        bslmf::NestedTraitDeclaration<TYPE, bdeat_IsBasicChoice>
    {
        // This class template ties the 'bdeat_TypeTraitBasicChoice'
        // trait tag to the 'bdeat_IsBasicChoice' trait metafunction.
    };

    template <class TYPE>
    struct Metafunction : bdeat_IsBasicChoice<TYPE>::type { };
};

struct bdeat_TypeTraitBasicSequence {
    // Legacy adaptor for bdeat_IsBasicSequence.

    template <class TYPE>
    struct NestedTraitDeclaration :
        bslmf::NestedTraitDeclaration<TYPE, bdeat_IsBasicSequence>
    {
        // This class template ties the 'bdeat_TypeTraitBasicSequence'
        // trait tag to the 'bdeat_IsBasicSequence' trait metafunction.
    };

    template <class TYPE>
    struct Metafunction : bdeat_IsBasicSequence<TYPE>::type { };
};

struct bdeat_TypeTraitBasicEnumeration {
    // Legacy adaptor for bdeat_IsBasicEnumeration.

    template <class TYPE>
    struct NestedTraitDeclaration :
        bslmf::NestedTraitDeclaration<TYPE, bdeat_IsBasicEnumeration>
    {
        // This class template ties the 'bdeat_TypeTraitBasicEnumeration'
        // trait tag to the 'bdeat_IsBasicEnumeration' trait metafunction.
    };

    template <class TYPE>
    struct Metafunction : bdeat_IsBasicEnumeration<TYPE>::type { };
};

struct bdeat_TypeTraitBasicCustomizedType {
    // Legacy adaptor for bdeat_IsBasicCustomizedType.

    template <class TYPE>
    struct NestedTraitDeclaration :
        bslmf::NestedTraitDeclaration<TYPE, bdeat_IsBasicCustomizedType>
    {
        // This class template ties the 'bdeat_TypeTraitBasicCustomizedType'
        // trait tag to the 'bdeat_IsBasicCustomizedType' trait metafunction.
    };

    template <class TYPE>
    struct Metafunction : bdeat_IsBasicCustomizedType<TYPE>::type { };
};

// Macros defined to facilitate declaring traits for generated code.

#define BDEAT_DECL_CHOICE_TRAITS(ClassName)                                   \
    template <>                                                               \
    struct bdeat_IsBasicChoice<ClassName> : bsl::true_type { };               \
    template <>                                                               \
    struct bdeu_HasPrintMethod<ClassName> : bsl::true_type { };

#define BDEAT_DECL_CHOICE_WITH_ALLOCATOR_TRAITS(ClassName)                    \
    namespace bslma {                                                         \
        template <>                                                           \
        struct UsesBslmaAllocator<ClassName> : bsl::true_type { };            \
    }                                                                         \
    BDEAT_DECL_CHOICE_TRAITS(ClassName)

#define BDEAT_DECL_CHOICE_WITH_BITWISEMOVEABLE_TRAITS(ClassName)              \
    namespace bslmf {                                                         \
        template <>                                                           \
        struct IsBitwiseMoveable<ClassName> : bsl::true_type { };             \
    }                                                                         \
    BDEAT_DECL_CHOICE_TRAITS(ClassName)

#define BDEAT_DECL_CHOICE_WITH_ALLOCATOR_BITWISEMOVEABLE_TRAITS(ClassName)    \
    namespace bslma {                                                         \
        template <>                                                           \
        struct UsesBslmaAllocator<ClassName> : bsl::true_type { };            \
    }                                                                         \
    namespace bslmf {                                                         \
        template <>                                                           \
        struct IsBitwiseMoveable<ClassName> : bsl::true_type { };             \
    }                                                                         \
    BDEAT_DECL_CHOICE_TRAITS(ClassName)

#define BDEAT_DECL_SEQUENCE_TRAITS(ClassName)                                 \
    template <>                                                               \
    struct bdeat_IsBasicSequence<ClassName> : bsl::true_type { };             \
    template <>                                                               \
    struct bdeu_HasPrintMethod<ClassName> : bsl::true_type { };

#define BDEAT_DECL_SEQUENCE_WITH_ALLOCATOR_TRAITS(ClassName)                  \
    namespace bslma {                                                         \
        template <>                                                           \
        struct UsesBslmaAllocator<ClassName> : bsl::true_type { };            \
    }                                                                         \
    BDEAT_DECL_SEQUENCE_TRAITS(ClassName)

#define BDEAT_DECL_SEQUENCE_WITH_BITWISEMOVEABLE_TRAITS(ClassName)            \
    namespace bslmf {                                                         \
        template <>                                                           \
        struct IsBitwiseMoveable<ClassName> : bsl::true_type { };             \
    }                                                                         \
    BDEAT_DECL_SEQUENCE_TRAITS(ClassName)

#define BDEAT_DECL_SEQUENCE_WITH_ALLOCATOR_BITWISEMOVEABLE_TRAITS(ClassName)  \
    namespace bslma {                                                         \
        template <>                                                           \
        struct UsesBslmaAllocator<ClassName> : bsl::true_type { };            \
    }                                                                         \
    namespace bslmf {                                                         \
        template <>                                                           \
        struct IsBitwiseMoveable<ClassName> : bsl::true_type { };             \
    }                                                                         \
    BDEAT_DECL_SEQUENCE_TRAITS(ClassName)

#define BDEAT_DECL_ENUMERATION_TRAITS(ClassName)                              \
    namespace bslmf {                                                         \
        template <>                                                           \
        struct IsBitwiseMoveable<ClassName::Value> : bsl::true_type { };      \
    }                                                                         \
    template <>                                                               \
    struct bdeat_IsBasicEnumeration<ClassName::Value> : bsl::true_type { };   \
    template <>                                                               \
    struct bdeat_BasicEnumerationWrapper<ClassName::Value> : ClassName {      \
        typedef ClassName Wrapper;                                            \
    };

#define BDEAT_DECL_CUSTOMIZEDTYPE_TRAITS(ClassName)                           \
    template <>                                                               \
    struct bdeat_IsBasicCustomizedType<ClassName> : bsl::true_type { };       \
    template <>                                                               \
    struct bdeu_HasPrintMethod<ClassName> : bsl::true_type { };

#define BDEAT_DECL_CUSTOMIZEDTYPE_WITH_ALLOCATOR_TRAITS(ClassName)            \
    namespace bslma {                                                         \
        template <>                                                           \
        struct UsesBslmaAllocator<ClassName> : bsl::true_type { };            \
    }                                                                         \
    BDEAT_DECL_CUSTOMIZEDTYPE_TRAITS(ClassName)

#define BDEAT_DECL_CUSTOMIZEDTYPE_WITH_BITWISEMOVEABLE_TRAITS(ClassName)      \
    namespace bslmf {                                                         \
        template <>                                                           \
        struct IsBitwiseMoveable<ClassName> : bsl::true_type { };             \
    }                                                                         \
    BDEAT_DECL_CUSTOMIZEDTYPE_TRAITS(ClassName)

#define                                                                       \
   BDEAT_DECL_CUSTOMIZEDTYPE_WITH_ALLOCATOR_BITWISEMOVEABLE_TRAITS(ClassName) \
    namespace bslma {                                                         \
        template <>                                                           \
        struct UsesBslmaAllocator<ClassName> : bsl::true_type { };            \
    }                                                                         \
    namespace bslmf {                                                         \
        template <>                                                           \
        struct IsBitwiseMoveable<ClassName> : bsl::true_type { };             \
    }                                                                         \
    BDEAT_DECL_CUSTOMIZEDTYPE_TRAITS(ClassName)

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2005
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
