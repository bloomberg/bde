// bdlat_bdeatoverrides.h                                             -*-C++-*-
#ifndef INCLUDED_BDLAT_BDEATOVERRIDES
#define INCLUDED_BDLAT_BDEATOVERRIDES

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

#ifndef BDE_OMIT_INTERNAL_DEPRECATED

// BDE_VERIFY pragma: -SLM01  // leaking macros is the whole point!

#define bdlat_ArrayFunctions         bdlat_ArrayFunctions
#define bdlat_arrayAccessElement     bdlat_arrayAccessElement
#define bdlat_arrayManipulateElement bdlat_arrayManipulateElement
#define bdlat_arrayResize            bdlat_arrayResize
#define bdlat_arraySize              bdlat_arraySize

#define bdlat_ArrayIterators bdlat_ArrayIterators

#define bdlat_AttributeInfo bdlat_AttributeInfo

#define bdlat_ChoiceFunctions           bdlat_ChoiceFunctions
#define bdlat_choiceAccessSelection     bdlat_choiceAccessSelection
#define bdlat_choiceHasSelection        bdlat_choiceHasSelection
#define bdlat_choiceMakeSelection       bdlat_choiceMakeSelection
#define bdlat_choiceManipulateSelection bdlat_choiceManipulateSelection
#define bdlat_choiceSelectionId         bdlat_choiceSelectionId

#define bdlat_CustomizedTypeFunctions bdlat_CustomizedTypeFunctions

#define bdlat_EnumeratorInfo bdlat_EnumeratorInfo
#define bdlat_EnumFunctions  bdlat_EnumFunctions
#define bdlat_enumFromInt    bdlat_enumFromInt
#define bdlat_enumFromString bdlat_enumFromString
#define bdlat_enumToInt      bdlat_enumToInt
#define bdlat_enumToString   bdlat_enumToString

#define bdlat_FormattingMode bdlat_FormattingMode

#define bdlat_NullableValueFunctions       bdlat_NullableValueFunctions
#define bdlat_nullableValueAccessValue     bdlat_nullableValueAccessValue
#define bdlat_nullableValueIsNull          bdlat_nullableValueIsNull
#define bdlat_nullableValueMakeValue       bdlat_nullableValueMakeValue
#define bdlat_nullableValueManipulateValue bdlat_nullableValueManipulateValue

#define bdlat_SelectionInfo bdlat_SelectionInfo

#define bdlat_SequenceFunctions            bdlat_SequenceFunctions
#define bdlat_sequenceAccessAttribute      bdlat_sequenceAccessAttribute
#define bdlat_sequenceAccessAttributes     bdlat_sequenceAccessAttributes
#define bdlat_sequenceHasAttribute         bdlat_sequenceHasAttribute
#define bdlat_sequenceManipulateAttribute  bdlat_sequenceManipulateAttribute
#define bdlat_sequenceManipulateAttributes bdlat_sequenceManipulateAttributes

#define bdlat_SymbolicConverter bdlat_SymbolicConverter

#define bdlat_TypeCategory                  bdlat_TypeCategory
#define bdlat_TypeCategoryDeclareDynamic    bdlat_TypeCategoryDeclareDynamic
#define bdlat_TypeCategoryFunctions         bdlat_TypeCategoryFunctions
#define bdlat_TypeCategoryUtil              bdlat_TypeCategoryUtil
#define bdlat_typeCategoryAccessArray       bdlat_typeCategoryAccessArray
#define bdlat_typeCategoryAccessChoice      bdlat_typeCategoryAccessChoice
#define bdlat_typeCategoryAccessCustomizedType                                \
        bdlat_typeCategoryAccessCustomizedType
#define bdlat_typeCategoryAccessEnumeration bdlat_typeCategoryAccessEnumeration
#define bdlat_typeCategoryAccessNullableValue                                 \
        bdlat_typeCategoryAccessNullableValue
#define bdlat_typeCategoryAccessSequence    bdlat_typeCategoryAccessSequence
#define bdlat_typeCategoryAccessSimple      bdlat_typeCategoryAccessSimple
#define bdlat_typeCategoryManipulateArray   bdlat_typeCategoryManipulateArray
#define bdlat_typeCategoryManipulateChoice  bdlat_typeCategoryManipulateChoice
#define bdlat_typeCategoryManipulateCustomizedType                            \
        bdlat_typeCategoryManipulateCustomizedType
#define bdlat_typeCategoryManipulateEnumeration                               \
        bdlat_typeCategoryManipulateEnumeration
#define bdlat_typeCategoryManipulateNullableValue                             \
        bdlat_typeCategoryManipulateNullableValue
#define bdlat_typeCategoryManipulateSequence                                  \
        bdlat_typeCategoryManipulateSequence
#define bdlat_typeCategoryManipulateSimple  bdlat_typeCategoryManipulateSimple
#define bdlat_typeCategorySelect            bdlat_typeCategorySelect

#define bdlat_TypeName           bdlat_TypeName
#define bdlat_TypeName_className bdlat_TypeName_className
#define bdlat_TypeName_name      bdlat_TypeName_name
#define bdlat_TypeName_xsdName   bdlat_TypeName_xsdName

#define e_DECL_CHOICE_TRAITS                                              \
        BDLAT_DECL_CHOICE_TRAITS
#define e_DECL_CHOICE_WITH_ALLOCATOR_BITWISEMOVEABLE_TRAITS               \
        BDLAT_DECL_CHOICE_WITH_ALLOCATOR_BITWISEMOVEABLE_TRAITS
#define e_DECL_CHOICE_WITH_ALLOCATOR_TRAITS                               \
        BDLAT_DECL_CHOICE_WITH_ALLOCATOR_TRAITS
#define e_DECL_CHOICE_WITH_BITWISEMOVEABLE_TRAITS                         \
        BDLAT_DECL_CHOICE_WITH_BITWISEMOVEABLE_TRAITS
#define e_DECL_CUSTOMIZEDTYPE_TRAITS                                      \
        BDLAT_DECL_CUSTOMIZEDTYPE_TRAITS
#define e_DECL_CUSTOMIZEDTYPE_WITH_ALLOCATOR_BITWISEMOVEABLE_TRAITS       \
        BDLAT_DECL_CUSTOMIZEDTYPE_WITH_ALLOCATOR_BITWISEMOVEABLE_TRAITS
#define e_DECL_CUSTOMIZEDTYPE_WITH_ALLOCATOR_TRAITS                       \
        BDLAT_DECL_CUSTOMIZEDTYPE_WITH_ALLOCATOR_TRAITS
#define e_DECL_CUSTOMIZEDTYPE_WITH_BITWISEMOVEABLE_TRAITS                 \
        BDLAT_DECL_CUSTOMIZEDTYPE_WITH_BITWISEMOVEABLE_TRAITS
#define e_DECL_ENUMERATION_TRAITS                                         \
        BDLAT_DECL_ENUMERATION_TRAITS
#define e_DECL_SEQUENCE_TRAITS                                            \
        BDLAT_DECL_SEQUENCE_TRAITS
#define e_DECL_SEQUENCE_WITH_ALLOCATOR_BITWISEMOVEABLE_TRAITS             \
        BDLAT_DECL_SEQUENCE_WITH_ALLOCATOR_BITWISEMOVEABLE_TRAITS
#define e_DECL_SEQUENCE_WITH_ALLOCATOR_TRAITS                             \
        BDLAT_DECL_SEQUENCE_WITH_ALLOCATOR_TRAITS
#define e_DECL_SEQUENCE_WITH_BITWISEMOVEABLE_TRAITS                       \
        BDLAT_DECL_SEQUENCE_WITH_BITWISEMOVEABLE_TRAITS

#define bdlat_BasicEnumerationWrapper      bdlat_BasicEnumerationWrapper
#define bdlat_IsBasicChoice                bdlat_IsBasicChoice
#define bdlat_IsBasicCustomizedType        bdlat_IsBasicCustomizedType
#define bdlat_IsBasicEnumeration           bdlat_IsBasicEnumeration
#define bdlat_IsBasicSequence              bdlat_IsBasicSequence
#define bdlat_TypeTraitBasicChoice         bdlat_TypeTraitBasicChoice
#define bdlat_TypeTraitBasicCustomizedType bdlat_TypeTraitBasicCustomizedType
#define bdlat_TypeTraitBasicEnumeration    bdlat_TypeTraitBasicEnumeration
#define bdlat_TypeTraitBasicSequence       bdlat_TypeTraitBasicSequence

#define bdlat_ValueTypeFunctions bdlat_ValueTypeFunctions
#define bdlat_valueTypeAssign bdlat_valueTypeAssign
#define bdlat_valueTypeReset bdlat_valueTypeReset

#endif  // BDE_OMIT_INTERNAL_DEPRECATED

#endif  // INCLUDED_BDLAT_BDEATOVERRIDES

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
