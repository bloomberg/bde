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
//  bdeat_TypeTraitBasicChoice:         trait for generated "choice" types
//  bdeat_TypeTraitBasicSequence:       trait for generated "sequence" types
//  bdeat_TypeTraitBasicEnumeration:    trait for generated "enumeration" types
//  bdeat_TypeTraitBasicCustomizedType: trait for generated "customized" types
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
// generated types.  The four traits provided, 'bdeat_TypeTraitBasicChoice',
// 'bdeat_TypeTraitBasicSequence', 'bdeat_TypeTraitBasicEnumeration',
// 'bdeat_TypeTraitBasicCustomizedType' are used for generated "choice" types,
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

struct bdeat_TypeTraitBasicChoice {
    // This trait may be declared for "choice" types.
};

struct bdeat_TypeTraitBasicSequence {
    // This trait may be declared for "sequence" types.
};

struct bdeat_TypeTraitBasicEnumeration {
    // This trait may be declared for "enumeration" types.
};

struct bdeat_TypeTraitBasicCustomizedType {
    // This trait may be declared for "customized type" types.
};

// Macros defined to facilitate declaring traits for generated code.

#define BDEAT_DECL_CHOICE_TRAITS(ClassName)                                   \
    template <>                                                               \
    struct bslalg_TypeTraits<ClassName> : bdeat_TypeTraitBasicChoice,         \
                                          bdeu_TypeTraitHasPrintMethod {      \
    };                                                                        \

#define BDEAT_DECL_CHOICE_WITH_ALLOCATOR_TRAITS(ClassName)                    \
    template <>                                                               \
    struct bslalg_TypeTraits<ClassName> : bdeat_TypeTraitBasicChoice,         \
                                          bslalg_TypeTraitUsesBslmaAllocator, \
                                          bdeu_TypeTraitHasPrintMethod {      \
    };                                                                        \

#define BDEAT_DECL_CHOICE_WITH_BITWISEMOVEABLE_TRAITS(ClassName)              \
    template <>                                                               \
    struct bslalg_TypeTraits<ClassName> : bdeat_TypeTraitBasicChoice,         \
                                          bslalg_TypeTraitBitwiseMoveable,    \
                                          bdeu_TypeTraitHasPrintMethod {      \
    };                                                                        \

#define BDEAT_DECL_CHOICE_WITH_ALLOCATOR_BITWISEMOVEABLE_TRAITS(ClassName)    \
    template <>                                                               \
    struct bslalg_TypeTraits<ClassName> : bdeat_TypeTraitBasicChoice,         \
                                          bslalg_TypeTraitBitwiseMoveable,    \
                                          bslalg_TypeTraitUsesBslmaAllocator, \
                                          bdeu_TypeTraitHasPrintMethod {      \
    };                                                                        \

#define BDEAT_DECL_SEQUENCE_TRAITS(ClassName)                                 \
    template <>                                                               \
    struct bslalg_TypeTraits<ClassName> : bdeat_TypeTraitBasicSequence,       \
                                          bdeu_TypeTraitHasPrintMethod {      \
    };                                                                        \

#define BDEAT_DECL_SEQUENCE_WITH_ALLOCATOR_TRAITS(ClassName)                  \
    template <>                                                               \
    struct bslalg_TypeTraits<ClassName> : bdeat_TypeTraitBasicSequence,       \
                                          bslalg_TypeTraitUsesBslmaAllocator, \
                                          bdeu_TypeTraitHasPrintMethod {      \
    };                                                                        \

#define BDEAT_DECL_SEQUENCE_WITH_BITWISEMOVEABLE_TRAITS(ClassName)            \
    template <>                                                               \
    struct bslalg_TypeTraits<ClassName> : bdeat_TypeTraitBasicSequence,       \
                                          bslalg_TypeTraitBitwiseMoveable,    \
                                          bdeu_TypeTraitHasPrintMethod {      \
    };                                                                        \

#define BDEAT_DECL_SEQUENCE_WITH_ALLOCATOR_BITWISEMOVEABLE_TRAITS(ClassName)  \
    template <>                                                               \
    struct bslalg_TypeTraits<ClassName> : bdeat_TypeTraitBasicSequence,       \
                                          bslalg_TypeTraitBitwiseMoveable,    \
                                          bslalg_TypeTraitUsesBslmaAllocator, \
                                          bdeu_TypeTraitHasPrintMethod {      \
    };                                                                        \

#define BDEAT_DECL_ENUMERATION_TRAITS(ClassName)                              \
    template <>                                                               \
    struct bslalg_TypeTraits<ClassName::Value>                                \
                                        : bdeat_TypeTraitBasicEnumeration,    \
                                          bslalg_TypeTraitBitwiseMoveable {   \
        typedef ClassName Wrapper;                                            \
    };                                                                        \

#define BDEAT_DECL_CUSTOMIZEDTYPE_TRAITS(ClassName)                           \
    template <>                                                               \
    struct bslalg_TypeTraits<ClassName> : bdeat_TypeTraitBasicCustomizedType, \
                                          bdeu_TypeTraitHasPrintMethod {      \
    };                                                                        \

#define BDEAT_DECL_CUSTOMIZEDTYPE_WITH_ALLOCATOR_TRAITS(ClassName)            \
    template <>                                                               \
    struct bslalg_TypeTraits<ClassName> : bdeat_TypeTraitBasicCustomizedType, \
                                          bslalg_TypeTraitUsesBslmaAllocator, \
                                          bdeu_TypeTraitHasPrintMethod {      \
    };                                                                        \

#define BDEAT_DECL_CUSTOMIZEDTYPE_WITH_BITWISEMOVEABLE_TRAITS(ClassName)      \
    template <>                                                               \
    struct bslalg_TypeTraits<ClassName> : bdeat_TypeTraitBasicCustomizedType, \
                                          bslalg_TypeTraitBitwiseMoveable,    \
                                          bdeu_TypeTraitHasPrintMethod {      \
    };                                                                        \

#define                                                                       \
   BDEAT_DECL_CUSTOMIZEDTYPE_WITH_ALLOCATOR_BITWISEMOVEABLE_TRAITS(ClassName) \
    template <>                                                               \
    struct bslalg_TypeTraits<ClassName> : bdeat_TypeTraitBasicCustomizedType, \
                                          bslalg_TypeTraitBitwiseMoveable,    \
                                          bslalg_TypeTraitUsesBslmaAllocator, \
                                          bdeu_TypeTraitHasPrintMethod {      \
    };                                                                        \

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
