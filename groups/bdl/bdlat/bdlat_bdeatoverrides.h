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

#ifndef BDE_OPENSOURCE_PUBLICATION

// BDE_VERIFY pragma: -SLM01  // leaking macros is the whole point!

#define bdeat_ArrayFunctions         bdlat_ArrayFunctions
#define bdeat_arrayAccessElement     bdlat_arrayAccessElement
#define bdeat_arrayManipulateElement bdlat_arrayManipulateElement
#define bdeat_arrayResize            bdlat_arrayResize
#define bdeat_arraySize              bdlat_arraySize

#define bdeat_ArrayIterators bdlat_ArrayIterators

#define bdeat_AttributeInfo bdlat_AttributeInfo

#define bdeat_ChoiceFunctions           bdlat_ChoiceFunctions
#define bdeat_choiceAccessSelection     bdlat_choiceAccessSelection
#define bdeat_choiceHasSelection        bdlat_choiceHasSelection
#define bdeat_choiceMakeSelection       bdlat_choiceMakeSelection
#define bdeat_choiceManipulateSelection bdlat_choiceManipulateSelection
#define bdeat_choiceSelectionId         bdlat_choiceSelectionId

#define bdeat_CustomizedTypeFunctions bdlat_CustomizedTypeFunctions

#define bdeat_EnumeratorInfo bdlat_EnumeratorInfo
#define bdeat_EnumFunctions  bdlat_EnumFunctions
#define bdeat_enumFromInt    bdlat_enumFromInt
#define bdeat_enumFromString bdlat_enumFromString
#define bdeat_enumToInt      bdlat_enumToInt
#define bdeat_enumToString   bdlat_enumToString

#define bdeat_FormattingMode bdlat_FormattingMode

#define bdeat_NullableValueFunctions       bdlat_NullableValueFunctions
#define bdeat_nullableValueAccessValue     bdlat_nullableValueAccessValue
#define bdeat_nullableValueIsNull          bdlat_nullableValueIsNull
#define bdeat_nullableValueMakeValue       bdlat_nullableValueMakeValue
#define bdeat_nullableValueManipulateValue bdlat_nullableValueManipulateValue

#define bdeat_SelectionInfo bdlat_SelectionInfo

#define bdeat_SequenceFunctions            bdlat_SequenceFunctions
#define bdeat_sequenceAccessAttribute      bdlat_sequenceAccessAttribute
#define bdeat_sequenceAccessAttributes     bdlat_sequenceAccessAttributes
#define bdeat_sequenceHasAttribute         bdlat_sequenceHasAttribute
#define bdeat_sequenceManipulateAttribute  bdlat_sequenceManipulateAttribute
#define bdeat_sequenceManipulateAttributes bdlat_sequenceManipulateAttributes

#define bdeat_SymbolicConverter bdlat_SymbolicConverter

#define bdeat_TypeCategory                  bdlat_TypeCategory
#define bdeat_TypeCategoryDeclareDynamic    bdlat_TypeCategoryDeclareDynamic
#define bdeat_TypeCategoryFunctions         bdlat_TypeCategoryFunctions
#define bdeat_TypeCategoryUtil              bdlat_TypeCategoryUtil
#define bdeat_typeCategoryAccessArray       bdlat_typeCategoryAccessArray
#define bdeat_typeCategoryAccessChoice      bdlat_typeCategoryAccessChoice
#define bdeat_typeCategoryAccessCustomizedType                                \
        bdlat_typeCategoryAccessCustomizedType
#define bdeat_typeCategoryAccessEnumeration bdlat_typeCategoryAccessEnumeration
#define bdeat_typeCategoryAccessNullableValue                                 \
        bdlat_typeCategoryAccessNullableValue
#define bdeat_typeCategoryAccessSequence    bdlat_typeCategoryAccessSequence
#define bdeat_typeCategoryAccessSimple      bdlat_typeCategoryAccessSimple
#define bdeat_typeCategoryManipulateArray   bdlat_typeCategoryManipulateArray
#define bdeat_typeCategoryManipulateChoice  bdlat_typeCategoryManipulateChoice
#define bdeat_typeCategoryManipulateCustomizedType                            \
        bdlat_typeCategoryManipulateCustomizedType
#define bdeat_typeCategoryManipulateEnumeration                               \
        bdlat_typeCategoryManipulateEnumeration
#define bdeat_typeCategoryManipulateNullableValue                             \
        bdlat_typeCategoryManipulateNullableValue
#define bdeat_typeCategoryManipulateSequence                                  \
        bdlat_typeCategoryManipulateSequence
#define bdeat_typeCategoryManipulateSimple  bdlat_typeCategoryManipulateSimple
#define bdeat_typeCategorySelect            bdlat_typeCategorySelect

#define bdeat_TypeName           bdlat_TypeName
#define bdeat_TypeName_className bdlat_TypeName_className
#define bdeat_TypeName_name      bdlat_TypeName_name
#define bdeat_TypeName_xsdName   bdlat_TypeName_xsdName

#define BDEAT_DECL_CHOICE_TRAITS                                              \
        BDLAT_DECL_CHOICE_TRAITS
#define BDEAT_DECL_CHOICE_WITH_ALLOCATOR_BITWISEMOVEABLE_TRAITS               \
        BDLAT_DECL_CHOICE_WITH_ALLOCATOR_BITWISEMOVEABLE_TRAITS
#define BDEAT_DECL_CHOICE_WITH_ALLOCATOR_TRAITS                               \
        BDLAT_DECL_CHOICE_WITH_ALLOCATOR_TRAITS
#define BDEAT_DECL_CHOICE_WITH_BITWISEMOVEABLE_TRAITS                         \
        BDLAT_DECL_CHOICE_WITH_BITWISEMOVEABLE_TRAITS
#define BDEAT_DECL_CUSTOMIZEDTYPE_TRAITS                                      \
        BDLAT_DECL_CUSTOMIZEDTYPE_TRAITS
#define BDEAT_DECL_CUSTOMIZEDTYPE_WITH_ALLOCATOR_BITWISEMOVEABLE_TRAITS       \
        BDLAT_DECL_CUSTOMIZEDTYPE_WITH_ALLOCATOR_BITWISEMOVEABLE_TRAITS
#define BDEAT_DECL_CUSTOMIZEDTYPE_WITH_ALLOCATOR_TRAITS                       \
        BDLAT_DECL_CUSTOMIZEDTYPE_WITH_ALLOCATOR_TRAITS
#define BDEAT_DECL_CUSTOMIZEDTYPE_WITH_BITWISEMOVEABLE_TRAITS                 \
        BDLAT_DECL_CUSTOMIZEDTYPE_WITH_BITWISEMOVEABLE_TRAITS
#define BDEAT_DECL_ENUMERATION_TRAITS                                         \
        BDLAT_DECL_ENUMERATION_TRAITS
#define BDEAT_DECL_SEQUENCE_TRAITS                                            \
        BDLAT_DECL_SEQUENCE_TRAITS
#define BDEAT_DECL_SEQUENCE_WITH_ALLOCATOR_BITWISEMOVEABLE_TRAITS             \
        BDLAT_DECL_SEQUENCE_WITH_ALLOCATOR_BITWISEMOVEABLE_TRAITS
#define BDEAT_DECL_SEQUENCE_WITH_ALLOCATOR_TRAITS                             \
        BDLAT_DECL_SEQUENCE_WITH_ALLOCATOR_TRAITS
#define BDEAT_DECL_SEQUENCE_WITH_BITWISEMOVEABLE_TRAITS                       \
        BDLAT_DECL_SEQUENCE_WITH_BITWISEMOVEABLE_TRAITS

#define bdeat_BasicEnumerationWrapper      bdlat_BasicEnumerationWrapper
#define bdeat_IsBasicChoice                bdlat_IsBasicChoice
#define bdeat_IsBasicCustomizedType        bdlat_IsBasicCustomizedType
#define bdeat_IsBasicEnumeration           bdlat_IsBasicEnumeration
#define bdeat_IsBasicSequence              bdlat_IsBasicSequence
#define bdeat_TypeTraitBasicChoice         bdlat_TypeTraitBasicChoice
#define bdeat_TypeTraitBasicCustomizedType bdlat_TypeTraitBasicCustomizedType
#define bdeat_TypeTraitBasicEnumeration    bdlat_TypeTraitBasicEnumeration
#define bdeat_TypeTraitBasicSequence       bdlat_TypeTraitBasicSequence

#define bdeat_ValueTypeFunctions bdlat_ValueTypeFunctions
#define bdeat_valueTypeAssign bdlat_valueTypeAssign
#define bdeat_valueTypeReset bdlat_valueTypeReset

#endif  // BDE_OPENSOURCE_PUBLICATION

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
