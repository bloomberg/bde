// bdlar_anycastutil.h                                                     -*-C++-*-
#ifndef INCLUDED_BDLAR_ANYCASTUTIL
#define INCLUDED_BDLAR_ANYCASTUTIL

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide ...
//
//@CLASSES:
//  bdlar::AnyCastUtil: ...
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

#include <bdlar_accessorref.h>
#include <bdlar_manipulatorref.h>

namespace BloombergLP {
namespace bdlar {

                           // ==================
                           // struct AnyCastUtil
                           // ==================

struct AnyCastUtil {
    // CLASS METHODS

    /// Cast the specified `ref` to `AnyConstRef`.
    static AnyConstRef toAnyConstRef(const ArrayConstRef& ref);
    static AnyConstRef toAnyConstRef(const ChoiceConstRef& ref);
    static AnyConstRef toAnyConstRef(const CustomizedTypeConstRef& ref);
    static AnyConstRef toAnyConstRef(const DynamicTypeConstRef& ref);
    static AnyConstRef toAnyConstRef(const EnumConstRef& ref);
    static AnyConstRef toAnyConstRef(const NullableValueConstRef& ref);
    static AnyConstRef toAnyConstRef(const SequenceConstRef& ref);
    static AnyConstRef toAnyConstRef(const SimpleTypeConstRef& ref);

    /// Cast the specified `ref` to `AnyRef`.
    static AnyRef toAnyRef(const ArrayRef& ref);
    static AnyRef toAnyRef(const ChoiceRef& ref);
    static AnyRef toAnyRef(const CustomizedTypeRef& ref);
    static AnyRef toAnyRef(const DynamicTypeRef& ref);
    static AnyRef toAnyRef(const EnumRef& ref);
    static AnyRef toAnyRef(const NullableValueRef& ref);
    static AnyRef toAnyRef(const SequenceRef& ref);
    static AnyRef toAnyRef(const SimpleTypeRef& ref);

    /// Cast the specified `any` to `ArrayConstRef`.  The behaviour is
    /// undefined unless
    /// `any.typeCategory() == bdlat_TypeCategory::e_ARRAY_CATEGORY`.
    static ArrayConstRef toArrayConstRef(const AnyConstRef& any);

    /// Cast the specified `any` to `ArrayRef`.  The behaviour is undefined
    /// unless `any.typeCategory() == bdlat_TypeCategory::e_ARRAY_CATEGORY`.
    static ArrayRef toArrayRef(const AnyRef& any);

    /// Cast the specified `any` to `ChoiceConstRef`.  The behaviour is
    /// undefined unless
    /// `any.typeCategory() == bdlat_TypeCategory::e_CHOICE_CATEGORY`.
    static ChoiceConstRef toChoiceConstRef(const AnyConstRef& any);

    /// Cast the specified `any` to `ChoiceRef`.  The behaviour is undefined
    /// unless
    /// `any.typeCategory() == bdlat_TypeCategory::e_CUSTOMIZED_TYPE_CATEGORY`.
    static ChoiceRef toChoiceRef(const AnyRef& any);

    /// Cast the specified `any` to `CustomizedTypeConstRef`.  The behaviour is
    /// undefined unless
    /// `any.typeCategory() == bdlat_TypeCategory::e_CUSTOMIZED_TYPE_CATEGORY`.
    static CustomizedTypeConstRef toCustomizedTypeConstRef(
                                                       const AnyConstRef& any);

    /// Cast the specified `any` to `CustomizedTypeRef`.  The behaviour is
    /// undefined unless
    /// `any.typeCategory() == bdlat_TypeCategory::e_CHOICE_CATEGORY`.
    static CustomizedTypeRef toCustomizedTypeRef(const AnyRef& any);

    /// Cast the specified `any` to `DynamicTypeConstRef`.  The behaviour is
    /// undefined unless
    /// `any.typeCategory() == bdlat_TypeCategory::e_DYNAMIC_CATEGORY`.
    static DynamicTypeConstRef toDynamicTypeConstRef(const AnyConstRef& any);

    /// Cast the specified `any` to `DynamicTypeRef`.  The behaviour is
    /// undefined unless
    /// `any.typeCategory() == bdlat_TypeCategory::e_DYNAMIC_CATEGORY`.
    static DynamicTypeRef toDynamicTypeRef(const AnyRef& any);

    /// Cast the specified `any` to `EnumConstRef`.  The behaviour is
    /// undefined unless
    /// `any.typeCategory() == bdlat_TypeCategory::e_ENUMERATION_CATEGORY`.
    static EnumConstRef toEnumConstRef(const AnyConstRef& any);

    /// Cast the specified `any` to `EnumRef`.  The behaviour is undefined
    /// unless
    /// `any.typeCategory() == bdlat_TypeCategory::e_ENUMERATION_CATEGORY`.
    static EnumRef toEnumRef(const AnyRef& any);

    /// Cast the specified `any` to `NullableValueConstRef`.  The behaviour is
    /// undefined unless
    /// `any.typeCategory()==bdlat_TypeCategory::e_NULLABLE_VALUE_CATEGORY`.
    static NullableValueConstRef toNullableValueConstRef(
                                                       const AnyConstRef& any);

    /// Cast the specified `any` to `NullableValueRef`.  The behaviour is
    /// undefined unless
    /// `any.typeCategory()==bdlat_TypeCategory::e_NULLABLE_VALUE_CATEGORY`.
    static NullableValueRef toNullableValueRef(const AnyRef& any);

    /// Cast the specified `any` to `SequenceConstRef`.  The behaviour is
    /// undefined unless
    /// `any.typeCategory() == bdlat_TypeCategory::e_SEQUENCE_CATEGORY`.
    static SequenceConstRef toSequenceConstRef(const AnyConstRef& any);

    /// Cast the specified `any` to `SequenceRef`.  The behaviour is
    /// undefined unless
    /// `any.typeCategory() == bdlat_TypeCategory::e_SEQUENCE_CATEGORY`.
    static SequenceRef toSequenceRef(const AnyRef& any);

    /// Cast the specified `any` to `SimpleTypeConstRef`.  The behaviour is
    /// undefined unless
    /// `any.typeCategory() == bdlat_TypeCategory::e_SIMPLE_CATEGORY`.
    static SimpleTypeConstRef toSimpleTypeConstRef(const AnyConstRef& any);

    /// Cast the specified `any` to `SimpleTypeRef`.  The behaviour is
    /// undefined unless
    /// `any.typeCategory() == bdlat_TypeCategory::e_SIMPLE_CATEGORY`.
    static SimpleTypeRef toSimpleTypeRef(const AnyRef& any);
};

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

                           // ------------------
                           // struct AnyCastUtil
                           // ------------------

inline
AnyConstRef AnyCastUtil::toAnyConstRef(const ArrayConstRef& ref)
{
    return AnyConstRef(ref.objectAddress(), ref.vtable());
}

inline
AnyConstRef AnyCastUtil::toAnyConstRef(const ChoiceConstRef& ref)
{
    return AnyConstRef(ref.objectAddress(), ref.vtable());
}

inline
AnyConstRef AnyCastUtil::toAnyConstRef(const CustomizedTypeConstRef& ref)
{
    return AnyConstRef(ref.objectAddress(), ref.vtable());
}

inline
AnyConstRef AnyCastUtil::toAnyConstRef(const DynamicTypeConstRef& ref)
{
    return AnyConstRef(ref.objectAddress(), ref.vtable());
}

inline
AnyConstRef AnyCastUtil::toAnyConstRef(const EnumConstRef& ref)
{
    return AnyConstRef(ref.objectAddress(), ref.vtable());
}

inline
AnyConstRef AnyCastUtil::toAnyConstRef(const NullableValueConstRef& ref)
{
    return AnyConstRef(ref.objectAddress(), ref.vtable());
}

inline
AnyConstRef AnyCastUtil::toAnyConstRef(const SequenceConstRef& ref)
{
    return AnyConstRef(ref.objectAddress(), ref.vtable());
}

inline
AnyConstRef AnyCastUtil::toAnyConstRef(const SimpleTypeConstRef& ref)
{
    return AnyConstRef(ref.objectAddress(), ref.vtable());
}

inline
AnyRef AnyCastUtil::toAnyRef(const ArrayRef& ref)
{
    return AnyRef(ref.objectAddress(), ref.vtable());
}

inline
AnyRef AnyCastUtil::toAnyRef(const ChoiceRef& ref)
{
    return AnyRef(ref.objectAddress(), ref.vtable());
}

inline
AnyRef AnyCastUtil::toAnyRef(const CustomizedTypeRef& ref)
{
    return AnyRef(ref.objectAddress(), ref.vtable());
}

inline
AnyRef AnyCastUtil::toAnyRef(const DynamicTypeRef& ref)
{
    return AnyRef(ref.objectAddress(), ref.vtable());
}

inline
AnyRef AnyCastUtil::toAnyRef(const EnumRef& ref)
{
    return AnyRef(ref.objectAddress(), ref.vtable());
}

inline
AnyRef AnyCastUtil::toAnyRef(const NullableValueRef& ref)
{
    return AnyRef(ref.objectAddress(), ref.vtable());
}

inline
AnyRef AnyCastUtil::toAnyRef(const SequenceRef& ref)
{
    return AnyRef(ref.objectAddress(), ref.vtable());
}

inline
AnyRef AnyCastUtil::toAnyRef(const SimpleTypeRef& ref)
{
    return AnyRef(ref.objectAddress(), ref.vtable());
}

inline
ArrayConstRef AnyCastUtil::toArrayConstRef(const AnyConstRef& any)
{
    return ArrayConstRef(any.objectAddress(), any.arrayVtable());
}

inline
ArrayRef AnyCastUtil::toArrayRef(const AnyRef& any)
{
    return ArrayRef(any.objectAddress(), any.arrayVtable());
}

inline
ChoiceConstRef AnyCastUtil::toChoiceConstRef(const AnyConstRef& any)
{
    return ChoiceConstRef(any.objectAddress(), any.choiceVtable());
}

inline
ChoiceRef AnyCastUtil::toChoiceRef(const AnyRef& any)
{
    return ChoiceRef(any.objectAddress(), any.choiceVtable());
}

inline
CustomizedTypeConstRef AnyCastUtil::toCustomizedTypeConstRef(
                                                        const AnyConstRef& any)
{
    return CustomizedTypeConstRef(any.objectAddress(),
                                  any.customizedTypeVtable());
}

inline
CustomizedTypeRef AnyCastUtil::toCustomizedTypeRef(const AnyRef& any)
{
    return CustomizedTypeRef(any.objectAddress(), any.customizedTypeVtable());
}

inline
DynamicTypeConstRef AnyCastUtil::toDynamicTypeConstRef(const AnyConstRef& any)
{
    return DynamicTypeConstRef(any.objectAddress(), any.dynamicTypeVtable());
}

inline
DynamicTypeRef AnyCastUtil::toDynamicTypeRef(const AnyRef& any)
{
    return DynamicTypeRef(any.objectAddress(), any.dynamicTypeVtable());
}

inline
EnumConstRef AnyCastUtil::toEnumConstRef(const AnyConstRef& any)
{
    return EnumConstRef(any.objectAddress(), any.enumVtable());
}

inline
EnumRef AnyCastUtil::toEnumRef(const AnyRef& any)
{
    return EnumRef(any.objectAddress(), any.enumVtable());
}

inline
NullableValueConstRef AnyCastUtil::toNullableValueConstRef(
                                                        const AnyConstRef& any)
{
    return NullableValueConstRef(any.objectAddress(),
                                 any.nullableValueVtable());
}

inline
NullableValueRef AnyCastUtil::toNullableValueRef(const AnyRef& any)
{
    return NullableValueRef(any.objectAddress(), any.nullableValueVtable());
}

inline
SequenceConstRef AnyCastUtil::toSequenceConstRef(const AnyConstRef& any)
{
    return SequenceConstRef(any.objectAddress(), any.sequenceVtable());
}

inline
SequenceRef AnyCastUtil::toSequenceRef(const AnyRef& any)
{
    return SequenceRef(any.objectAddress(), any.sequenceVtable());
}

inline
SimpleTypeConstRef AnyCastUtil::toSimpleTypeConstRef(const AnyConstRef& any)
{
    return SimpleTypeConstRef(any.objectAddress(), any.simpleTypeVtable());
}

inline
SimpleTypeRef AnyCastUtil::toSimpleTypeRef(const AnyRef& any)
{
    return SimpleTypeRef(any.objectAddress(), any.simpleTypeVtable());
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
