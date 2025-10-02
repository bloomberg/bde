// bdlar_typecategory.h                                               -*-C++-*-
#ifndef INCLUDED_BDLAR_TYPECATEGORY
#define INCLUDED_BDLAR_TYPECATEGORY

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide ...
//
//@CLASSES:
//  bdlar::IsArray: ...
//  bdlar::IsChoice: ...
//  bdlar::IsCustomized: ...
//  bdlar::IsEnum: ...
//  bdlar::IsNullable: ...
//  bdlar::IsSequence: ...
//  bdlar::IsSimple: ...
//  bdlar::IsDynamic: ...
//  bdlar::SelectCategory: ...
//
//@DESCRIPTION: This component provides ...
//

#include <bdlat_typecategory.h>

#include <bslmf_switch.h>

#include <bsls_keyword.h>

#include <bsl_type_traits.h>
#include <bsl_vector.h>

namespace BloombergLP {
namespace bdlar {

                           // ==============
                           // struct IsArray
                           // ==============

template<class t_TYPE>
struct IsArray : ::BloombergLP::bdlat_ArrayFunctions::IsArray<t_TYPE> {
};

// bsl::vector<char> is a simple type
template<>
struct IsArray<bsl::vector<char> > : bsl::false_type {
};

                           // ===============
                           // struct IsChoice
                           // ===============

using ::BloombergLP::bdlat_ChoiceFunctions::IsChoice;

                           // ===================
                           // struct IsCustomized
                           // ===================

template<class t_TYPE>
struct IsCustomized : ::BloombergLP::bdlat_CustomizedTypeFunctions::
                                                     IsCustomizedType<t_TYPE> {
};

                           // =============
                           // struct IsEnum
                           // =============

template<class t_TYPE>
struct IsEnum : ::BloombergLP::bdlat_EnumFunctions::IsEnumeration<t_TYPE> {
};

                           // =================
                           // struct IsNullable
                           // =================

template<class t_TYPE>
struct IsNullable : ::BloombergLP::bdlat_NullableValueFunctions::
                                                      IsNullableValue<t_TYPE> {
};

                           // =================
                           // struct IsSequence
                           // =================

using ::BloombergLP::bdlat_SequenceFunctions::IsSequence;

                           // ===============
                           // struct IsSimple
                           // ===============

template <class TYPE>
struct IsSimple : bsl::integral_constant<bool,
    static_cast<bdlat_TypeCategory::Value>(
        bdlat_TypeCategory::Select<TYPE>::e_SELECTION) ==
                                       bdlat_TypeCategory::e_SIMPLE_CATEGORY> {
};

template <>
struct IsSimple<bsl::vector<char> > : bsl::true_type {
};

                           // ================
                           // struct IsDynamic
                           // ================

template <class t_TYPE>
struct IsDynamic : bsl::integral_constant<bool,
    static_cast<bdlat_TypeCategory::Value>(
        bdlat_TypeCategory::Select<t_TYPE>::e_SELECTION) ==
                                       bdlat_TypeCategory::e_DYNAMIC_CATEGORY>{
};

                           // =====================
                           // struct SelectCategory
                           // =====================

template <class t_TYPE>
struct SelectCategory {
    // CLASS DATA
    static const BSLS_KEYWORD_CONSTEXPR bdlat_TypeCategory::Value e_SELECTION =
        IsDynamic<t_TYPE>::value ? bdlat_TypeCategory::e_DYNAMIC_CATEGORY :
        IsArray<t_TYPE>::value ? bdlat_TypeCategory::e_ARRAY_CATEGORY :
        IsChoice<t_TYPE>::value ? bdlat_TypeCategory::e_CHOICE_CATEGORY :
        IsCustomized<t_TYPE>::value ?
                               bdlat_TypeCategory::e_CUSTOMIZED_TYPE_CATEGORY :
        IsEnum<t_TYPE>::value ? bdlat_TypeCategory::e_ENUMERATION_CATEGORY :
        IsNullable<t_TYPE>::value ?
                                bdlat_TypeCategory::e_NULLABLE_VALUE_CATEGORY :
        IsSequence<t_TYPE>::value ? bdlat_TypeCategory::e_SEQUENCE_CATEGORY :
        /* else */ bdlat_TypeCategory::e_SIMPLE_CATEGORY;

    // TYPES
    typedef typename
    bslmf::Switch<e_SELECTION, bdlat_TypeCategory::DynamicType,
                               bdlat_TypeCategory::Array,
                               bdlat_TypeCategory::Choice,
                               bdlat_TypeCategory::CustomizedType,
                               bdlat_TypeCategory::Enumeration,
                               bdlat_TypeCategory::NullableValue,
                               bdlat_TypeCategory::Sequence,
                               bdlat_TypeCategory::Simple>::Type Type;
};

                           // =====================
                           // struct FakeDependency
                           // =====================

/// This class is used only to prevent eager template instantiantions by clang.
template <class, class t_TYPE>
struct FakeDependency {
    // TYPES
    typedef t_TYPE Type;
};

}  // close namespace bdlar
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
