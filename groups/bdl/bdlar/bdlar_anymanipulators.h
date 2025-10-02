// bdlar_anymanipulators.h                                            -*-C++-*-
#ifndef INCLUDED_BDLAR_ANYMANIPULATORS
#define INCLUDED_BDLAR_ANYMANIPULATORS

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide ...
//
//@DESCRIPTION: This component provides ...
//

#include <bdlar_anyref.h>

#include <bdlar_arrayref.h>
#include <bdlar_choiceref.h>
#include <bdlar_customizedtyperef.h>
#include <bdlar_dynamictyperef.h>
#include <bdlar_enumref.h>
#include <bdlar_nullablevalueref.h>
#include <bdlar_sequenceref.h>
#include <bdlar_simpletyperef.h>

#include <bdlat_typecategory.h>

namespace BloombergLP {
namespace bdlar {


// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

                           // ------------
                           // class AnyRef
                           // ------------

// FREE FUNCTIONS
// Customization-point functions

template <class t_MANIPULATOR>
inline
int bdlat_typeCategoryManipulateArray(AnyRef         *any,
                                      t_MANIPULATOR&  manipulator)
{
    if (any->typeCategory() == bdlat_TypeCategory::e_ARRAY_CATEGORY) {
        ArrayRef ref(any->objectAddress(), any->arrayVtable());
        return manipulator(&ref, bdlat_TypeCategory::Array());        // RETURN
    }
    return manipulator(any, bslmf::Nil());
}

template <class t_MANIPULATOR>
inline
int bdlat_typeCategoryManipulateChoice(AnyRef *any, t_MANIPULATOR& manipulator)
{
    if (any->typeCategory() == bdlat_TypeCategory::e_CHOICE_CATEGORY) {
        ChoiceRef ref(any->objectAddress(), any->choiceVtable());
        return manipulator(&ref, bdlat_TypeCategory::Choice());       // RETURN
    }
    return manipulator(any, bslmf::Nil());
}

template <class t_MANIPULATOR>
inline
int bdlat_typeCategoryManipulateCustomizedType(AnyRef         *any,
                                               t_MANIPULATOR&  manipulator)
{
    if (any->typeCategory() ==
                              bdlat_TypeCategory::e_CUSTOMIZED_TYPE_CATEGORY) {
        CustomizedTypeRef ref(any->objectAddress(),
                              any->customizedTypeVtable());
        return manipulator(&ref, bdlat_TypeCategory::CustomizedType());
                                                                      // RETURN
    }
    return manipulator(any, bslmf::Nil());
}

template <class t_MANIPULATOR>
inline
int bdlat_typeCategoryManipulateEnumeration(AnyRef         *any,
                                            t_MANIPULATOR&  manipulator)
{
    if (any->typeCategory() == bdlat_TypeCategory::e_ENUMERATION_CATEGORY) {
        EnumRef ref(any->objectAddress(), any->enumVtable());
        return manipulator(&ref, bdlat_TypeCategory::Enumeration());  // RETURN
    }
    return manipulator(any, bslmf::Nil());
}

template <class t_MANIPULATOR>
inline
int bdlat_typeCategoryManipulateNullableValue(AnyRef         *any,
                                              t_MANIPULATOR&  manipulator)
{
    if (any->typeCategory() == bdlat_TypeCategory::e_NULLABLE_VALUE_CATEGORY) {
        NullableValueRef ref(any->objectAddress(), any->nullableValueVtable());
        return manipulator(&ref, bdlat_TypeCategory::NullableValue());
                                                                      // RETURN
    }
    return manipulator(any, bslmf::Nil());
}

template <class t_MANIPULATOR>
inline
int bdlat_typeCategoryManipulateSequence(AnyRef         *any,
                                         t_MANIPULATOR&  manipulator)
{
    if (any->typeCategory() == bdlat_TypeCategory::e_SEQUENCE_CATEGORY) {
        SequenceRef ref(any->objectAddress(), any->sequenceVtable());
        return manipulator(&ref, bdlat_TypeCategory::Sequence());     // RETURN
    }
    return manipulator(any, bslmf::Nil());
}

template <class t_MANIPULATOR>
inline
int bdlat_typeCategoryManipulateSimple(AnyRef         *any,
                                       t_MANIPULATOR&  manipulator)
{
    if (any->typeCategory() == bdlat_TypeCategory::e_SIMPLE_CATEGORY) {
        SimpleTypeRef ref(any->objectAddress(), any->simpleTypeVtable());
        return ref.manipulateWithCategory(manipulator);               // RETURN
    }
    return manipulator(any, bslmf::Nil());
}

template <class t_ACCESSOR>
inline
int bdlat_typeCategoryAccessArray(const AnyRef& any, t_ACCESSOR& accessor)
{
    if (any.typeCategory() == bdlat_TypeCategory::e_ARRAY_CATEGORY) {
        const ArrayRef ref(any.objectAddress(), any.arrayVtable());
        return accessor(ref, bdlat_TypeCategory::Array());            // RETURN
    }
    return accessor(any, bslmf::Nil());
}

template <class t_ACCESSOR>
inline
int bdlat_typeCategoryAccessChoice(const AnyRef& any, t_ACCESSOR& accessor)
{
    if (any.typeCategory() == bdlat_TypeCategory::e_CHOICE_CATEGORY) {
        const ChoiceRef ref(any.objectAddress(), any.choiceVtable());
        return accessor(ref, bdlat_TypeCategory::Choice());           // RETURN
    }
    return accessor(any, bslmf::Nil());
}

template <class t_ACCESSOR>
inline
int bdlat_typeCategoryAccessCustomizedType(const AnyRef& any,
                                           t_ACCESSOR&   accessor)
{
    if (any.typeCategory() == bdlat_TypeCategory::e_CUSTOMIZED_TYPE_CATEGORY) {
        const CustomizedTypeRef ref(any.objectAddress(),
                                    any.customizedTypeVtable());
        return accessor(ref, bdlat_TypeCategory::CustomizedType());   // RETURN
    }
    return accessor(any, bslmf::Nil());
}

template <class t_ACCESSOR>
inline
int bdlat_typeCategoryAccessEnumeration(const AnyRef& any,
                                        t_ACCESSOR&   accessor)
{
    if (any.typeCategory() == bdlat_TypeCategory::e_ENUMERATION_CATEGORY) {
        const EnumRef ref(any.objectAddress(), any.enumVtable());
        return accessor(ref, bdlat_TypeCategory::Enumeration());      // RETURN
    }
    return accessor(any, bslmf::Nil());
}

template <class t_ACCESSOR>
inline
int bdlat_typeCategoryAccessNullableValue(const AnyRef& any,
                                          t_ACCESSOR&   accessor)
{
    if (any.typeCategory() == bdlat_TypeCategory::e_NULLABLE_VALUE_CATEGORY) {
        const NullableValueRef ref(any.objectAddress(),
                                   any.nullableValueVtable());
        return accessor(ref, bdlat_TypeCategory::NullableValue());    // RETURN
    }
    return accessor(any, bslmf::Nil());
}

template <class t_ACCESSOR>
inline
int bdlat_typeCategoryAccessSequence(const AnyRef& any, t_ACCESSOR& accessor)
{
    if (any.typeCategory() == bdlat_TypeCategory::e_SEQUENCE_CATEGORY) {
        const SequenceRef ref(any.objectAddress(), any.sequenceVtable());
        return accessor(ref, bdlat_TypeCategory::Sequence());         // RETURN
    }
    return accessor(any, bslmf::Nil());
}

template <class t_ACCESSOR>
inline
int bdlat_typeCategoryAccessSimple(const AnyRef& any, t_ACCESSOR& accessor)
{
    if (any.typeCategory() == bdlat_TypeCategory::e_SIMPLE_CATEGORY) {
        const SimpleTypeRef ref(any.objectAddress(), any.simpleTypeVtable());
        return ref.accessWithCategory(accessor);                      // RETURN
    }
    return accessor(any, bslmf::Nil());
}

                           // -----------------
                           // class AnyConstRef
                           // -----------------

// FREE FUNCTIONS
// Customization-point functions

template <class t_ACCESSOR>
inline
int bdlat_typeCategoryAccessArray(const AnyConstRef& any, t_ACCESSOR& accessor)
{
    if (any.typeCategory() == bdlat_TypeCategory::e_ARRAY_CATEGORY) {
        const ArrayConstRef ref(any.objectAddress(), any.arrayVtable());
        return accessor(ref, bdlat_TypeCategory::Array());            // RETURN
    }
    return accessor(any, bslmf::Nil());
}

template <class t_ACCESSOR>
inline
int bdlat_typeCategoryAccessChoice(const AnyConstRef& any,
                                   t_ACCESSOR&        accessor)
{
    if (any.typeCategory() == bdlat_TypeCategory::e_CHOICE_CATEGORY) {
        const ChoiceConstRef ref(any.objectAddress(), any.choiceVtable());
        return accessor(ref, bdlat_TypeCategory::Choice());           // RETURN
    }
    return accessor(any, bslmf::Nil());
}

template <class t_ACCESSOR>
inline
int bdlat_typeCategoryAccessCustomizedType(const AnyConstRef& any,
                                           t_ACCESSOR&        accessor)
{
    if (any.typeCategory() == bdlat_TypeCategory::e_CUSTOMIZED_TYPE_CATEGORY) {
        const CustomizedTypeConstRef ref(any.objectAddress(),
                                         any.customizedTypeVtable());
        return accessor(ref, bdlat_TypeCategory::CustomizedType());   // RETURN
    }
    return accessor(any, bslmf::Nil());
}

template <class t_ACCESSOR>
inline
int bdlat_typeCategoryAccessEnumeration(const AnyConstRef& any,
                                        t_ACCESSOR&        accessor)
{
    if (any.typeCategory() == bdlat_TypeCategory::e_ENUMERATION_CATEGORY) {
        const EnumConstRef ref(any.objectAddress(), any.enumVtable());
        return accessor(ref, bdlat_TypeCategory::Enumeration());      // RETURN
    }
    return accessor(any, bslmf::Nil());
}

template <class t_ACCESSOR>
inline
int bdlat_typeCategoryAccessNullableValue(const AnyConstRef& any,
                                          t_ACCESSOR&        accessor)
{
    if (any.typeCategory() == bdlat_TypeCategory::e_NULLABLE_VALUE_CATEGORY) {
        const NullableValueConstRef ref(any.objectAddress(),
                                        any.nullableValueVtable());
        return accessor(ref, bdlat_TypeCategory::NullableValue());    // RETURN
    }
    return accessor(any, bslmf::Nil());
}

template <class t_ACCESSOR>
inline
int bdlat_typeCategoryAccessSequence(const AnyConstRef& any,
                                     t_ACCESSOR&        accessor)
{
    if (any.typeCategory() == bdlat_TypeCategory::e_SEQUENCE_CATEGORY) {
        const SequenceConstRef ref(any.objectAddress(), any.sequenceVtable());
        return accessor(ref, bdlat_TypeCategory::Sequence());         // RETURN
    }
    return accessor(any, bslmf::Nil());
}

template <class t_ACCESSOR>
inline
int bdlat_typeCategoryAccessSimple(const AnyConstRef& any,
                                   t_ACCESSOR&        accessor)
{
    if (any.typeCategory() == bdlat_TypeCategory::e_SIMPLE_CATEGORY) {
        const SimpleTypeConstRef ref(any.objectAddress(),
                                     any.simpleTypeVtable());
        return ref.accessWithCategory(accessor);                      // RETURN
    }
    return accessor(any, bslmf::Nil());
}

}  // close package namespace
}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2024 Bloomberg Finance L.P.
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
