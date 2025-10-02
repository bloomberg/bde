// bdlar_anyref.h                                                     -*-C++-*-
#ifndef INCLUDED_BDLAR_ANYREF
#define INCLUDED_BDLAR_ANYREF

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide ...
//
//@CLASSES:
//  bdlar::AnyRef: ...
//  bdlar::AnyConstRef: ...
//
//@DESCRIPTION: This component provides ...
//
// ```
//              +---------+
//              | Any Ref |  TYPE-ERASED UNIVERSAL REF
//              +---------+
//             /     |     \.
//            v      v      v
// +----------+ +---------+ +----------+
// | Cat1 Ref | | Dyn Ref | | Cat2 Ref |  VALUE REFS
// +----------+ +---------+ +----------+
//      |      /           \     |
//      v     v             v    v
// +----------+             +----------+
// | Cat1 Obj |             | Cat2 Obj |  VALUES (OBJECTS)
// +----------+             +----------+
// ```

#include <bdlar_anyrefdata.h>
#include <bdlar_anyvtable.h>

#include <bdlat_typecategory.h>
#include <bdlat_valuetypefunctions.h>

namespace BloombergLP {
namespace bdlar {

                           // ============
                           // class AnyRef
                           // ============

/// A type-erased universal wrapper for reference to any type category with
/// read/write access.
class AnyRef {
    // DATA
    void                      *d_object_p;
    AnyVtable                  d_vtable;
    bdlat_TypeCategory::Value  d_category;

  public:
    // CREATORS

    /// Create an `AnyRef` object.  The behaviour is undefined unless
    /// `objectAddress` and `vtable` point to the valid objects.
    AnyRef(void *objectAddress, const SimpleTypeVtable *vtable);

    /// Create an `AnyRef` object.  The behaviour is undefined unless
    /// `objectAddress` and `vtable` point to the valid objects.
    AnyRef(void *objectAddress, const EnumVtable *vtable);

    /// Create an `AnyRef` object.  The behaviour is undefined unless
    /// `objectAddress` and `vtable` point to the valid objects.
    AnyRef(void *objectAddress, const NullableValueVtable *vtable);

    /// Create an `AnyRef` object.  The behaviour is undefined unless
    /// `objectAddress` and `vtable` point to the valid objects.
    AnyRef(void *objectAddress, const ArrayVtable *vtable);

    /// Create an `AnyRef` object.  The behaviour is undefined unless
    /// `objectAddress` and `vtable` point to the valid objects.
    AnyRef(void *objectAddress, const SequenceVtable *vtable);

    /// Create an `AnyRef` object.  The behaviour is undefined unless
    /// `objectAddress` and `vtable` point to the valid objects.
    AnyRef(void *objectAddress, const ChoiceVtable *vtable);

    /// Create an `AnyRef` object.  The behaviour is undefined unless
    /// `objectAddress` and `vtable` point to the valid objects.
    AnyRef(void *objectAddress, const CustomizedTypeVtable *vtable);

    /// Create an `AnyRef` object.  The behaviour is undefined unless
    /// `objectAddress` and `vtable` point to the valid objects.
    AnyRef(void *objectAddress, const DynamicTypeVtable *vtable);

    /// Create an `AnyRef` object.
    AnyRef(void                      *objectAddress,
           AnyVtable                  vtable,
           bdlat_TypeCategory::Value  category);

    /// Create an `AnyRef` object.
    AnyRef(const AnyRefData& data); // IMPLICIT

    // MANIPULATORS

    /// Reset the referred object to the default value.
    void reset() const;

    // ACCESSORS

    /// Return a pointer to the vtable.  The behavior is undefined unless
    /// `typeCategory() == bdlat_TypeCategory::e_ARRAY_CATEGORY`.
    const ArrayVtable *arrayVtable() const;

    /// Return a pointer to the vtable.  The behavior is undefined unless
    /// `typeCategory() == bdlat_TypeCategory::e_CHOICE_CATEGORY`.
    const ChoiceVtable *choiceVtable() const;

    /// Return a pointer to the vtable.  The behavior is undefined unless
    /// `typeCategory() == bdlat_TypeCategory::e_CUSTOMIZED_TYPE_CATEGORY`.
    const CustomizedTypeVtable *customizedTypeVtable() const;

    /// Return a pointer to the vtable.  The behavior is undefined unless
    /// `typeCategory() == bdlat_TypeCategory::e_DYNAMIC_CATEGORY`.
    const DynamicTypeVtable *dynamicTypeVtable() const;

    /// Return a pointer to the vtable.  The behavior is undefined unless
    /// `typeCategory() == bdlat_TypeCategory::e_ENUMERATION_CATEGORY`.
    const EnumVtable *enumVtable() const;

    /// Return a pointer to the vtable.  The behavior is undefined unless
    /// `typeCategory() == bdlat_TypeCategory::e_NULLABLE_VALUE_CATEGORY`.
    const NullableValueVtable *nullableValueVtable() const;

    /// Return a pointer to the vtable.  The behavior is undefined unless
    /// `typeCategory() == bdlat_TypeCategory::e_SEQUENCE_CATEGORY`.
    const SequenceVtable *sequenceVtable() const;

    /// Return a pointer to the vtable.  The behavior is undefined unless
    /// `typeCategory() == bdlat_TypeCategory::e_SIMPLE_CATEGORY`.
    const SimpleTypeVtable *simpleTypeVtable() const;

    /// Return a pointer to the wrapped object.
    void *objectAddress() const;

    /// Return type category of the referred object.
    bdlat_TypeCategory::Value typeCategory() const;
};

// FREE FUNCTIONS
// Customization-point functions

template <class t_MANIPULATOR>
int bdlat_typeCategoryManipulateArray(AnyRef         *any,
                                      t_MANIPULATOR&  manipulator);
template <class t_MANIPULATOR>
int bdlat_typeCategoryManipulateChoice(AnyRef         *any,
                                       t_MANIPULATOR&  manipulator);
template <class t_MANIPULATOR>
int bdlat_typeCategoryManipulateCustomizedType(AnyRef         *any,
                                               t_MANIPULATOR&  manipulator);
template <class t_MANIPULATOR>
int bdlat_typeCategoryManipulateEnumeration(AnyRef         *any,
                                            t_MANIPULATOR&  manipulator);
template <class t_MANIPULATOR>
int bdlat_typeCategoryManipulateNullableValue(AnyRef         *any,
                                              t_MANIPULATOR&  manipulator);
template <class t_MANIPULATOR>
int bdlat_typeCategoryManipulateSequence(AnyRef         *any,
                                         t_MANIPULATOR&  manipulator);
template <class t_MANIPULATOR>
int bdlat_typeCategoryManipulateSimple(AnyRef         *any,
                                       t_MANIPULATOR&  manipulator);

template <class t_ACCESSOR>
int bdlat_typeCategoryAccessArray(const AnyRef& any, t_ACCESSOR& accessor);
template <class t_ACCESSOR>
int bdlat_typeCategoryAccessChoice(const AnyRef& any, t_ACCESSOR& accessor);
template <class t_ACCESSOR>
int bdlat_typeCategoryAccessCustomizedType(const AnyRef& any,
                                           t_ACCESSOR&   accessor);
template <class t_ACCESSOR>
int bdlat_typeCategoryAccessEnumeration(const AnyRef& any,
                                        t_ACCESSOR&   accessor);
template <class t_ACCESSOR>
int bdlat_typeCategoryAccessNullableValue(const AnyRef& any,
                                          t_ACCESSOR&   accessor);
template <class t_ACCESSOR>
int bdlat_typeCategoryAccessSequence(const AnyRef& any, t_ACCESSOR& accessor);
template <class t_ACCESSOR>
int bdlat_typeCategoryAccessSimple(const AnyRef& any, t_ACCESSOR& accessor);

                           // =================
                           // class AnyConstRef
                           // =================

/// A type-erased universal wrapper for reference to any type category with
/// read-only access.
class AnyConstRef {
    // DATA
    const void                *d_object_p;
    AnyConstVtable             d_vtable;
    bdlat_TypeCategory::Value  d_category;

  public:
    // CREATORS

    /// Create an `AnyConstRef` object.  The behaviour is undefined unless
    /// `objectAddress` and `vtable` point to the valid objects.
    AnyConstRef(const void                  *objectAddress,
                const SimpleTypeConstVtable *vtable);

    /// Create an `AnyConstRef` object.  The behaviour is undefined unless
    /// `objectAddress` and `vtable` point to the valid objects.
    AnyConstRef(const void *objectAddress, const EnumConstVtable *vtable);

    /// Create an `AnyConstRef` object.  The behaviour is undefined unless
    /// `objectAddress` and `vtable` point to the valid objects.
    AnyConstRef(const void                    *objectAddress,
                const NullableValueConstVtable *vtable);

    /// Create an `AnyConstRef` object.  The behaviour is undefined unless
    /// `objectAddress` and `vtable` point to the valid objects.
    AnyConstRef(const void *objectAddress, const ArrayConstVtable *vtable);

    /// Create an `AnyConstRef` object.  The behaviour is undefined unless
    /// `objectAddress` and `vtable` point to the valid objects.
    AnyConstRef(const void *objectAddress, const SequenceConstVtable *vtable);

    /// Create an `AnyConstRef` object.  The behaviour is undefined unless
    /// `objectAddress` and `vtable` point to the valid objects.
    AnyConstRef(const void *objectAddress, const ChoiceConstVtable *vtable);

    /// Create an `AnyConstRef` object.  The behaviour is undefined unless
    /// `objectAddress` and `vtable` point to the valid objects.
    AnyConstRef(const void                      *objectAddress,
                const CustomizedTypeConstVtable *vtable);

    /// Create an `AnyConstRef` object.  The behaviour is undefined unless
    /// `objectAddress` and `vtable` point to the valid objects.
    AnyConstRef(const void                   *objectAddress,
                const DynamicTypeConstVtable *vtable);

    /// Create an `AnyConstRef` object.
    AnyConstRef(const void                *objectAddress,
                AnyConstVtable             vtable,
                bdlat_TypeCategory::Value  category);

    /// Create an `AnyConstRef` object.
    AnyConstRef(const AnyConstRefData& data); // IMPLICIT

    /// Create an `AnyConstRef` object that wraps the same object as the
    /// specified `object`.
    AnyConstRef(const AnyRef& object); // IMPLICIT

    // ACCESSORS

    /// Return a pointer to the vtable.  The behavior is undefined unless
    /// `typeCategory() == bdlat_TypeCategory::e_ARRAY_CATEGORY`.
    const ArrayConstVtable *arrayVtable() const;

    /// Return a pointer to the vtable.  The behavior is undefined unless
    /// `typeCategory() == bdlat_TypeCategory::e_CHOICE_CATEGORY`.
    const ChoiceConstVtable *choiceVtable() const;

    /// Return a pointer to the vtable.  The behavior is undefined unless
    /// `typeCategory() == bdlat_TypeCategory::e_CUSTOMIZED_TYPE_CATEGORY`.
    const CustomizedTypeConstVtable *customizedTypeVtable() const;

    /// Return a pointer to the vtable.  The behavior is undefined unless
    /// `typeCategory() == bdlat_TypeCategory::e_DYNAMIC_CATEGORY`.
    const DynamicTypeConstVtable *dynamicTypeVtable() const;

    /// Return a pointer to the vtable.  The behavior is undefined unless
    /// `typeCategory() == bdlat_TypeCategory::e_ENUMERATION_CATEGORY`.
    const EnumConstVtable *enumVtable() const;

    /// Return a pointer to the vtable.  The behavior is undefined unless
    /// `typeCategory() == bdlat_TypeCategory::e_NULLABLE_VALUE_CATEGORY`.
    const NullableValueConstVtable *nullableValueVtable() const;

    /// Return a pointer to the vtable.  The behavior is undefined unless
    /// `typeCategory() == bdlat_TypeCategory::e_SEQUENCE_CATEGORY`.
    const SequenceConstVtable *sequenceVtable() const;

    /// Return a pointer to the vtable.  The behavior is undefined unless
    /// `typeCategory() == bdlat_TypeCategory::e_SIMPLE_CATEGORY`.
    const SimpleTypeConstVtable *simpleTypeVtable() const;

    /// Return a pointer to the wrapped object.
    const void *objectAddress() const;

    /// Return type category of the referred object.
    bdlat_TypeCategory::Value typeCategory() const;

    /// Return the `bdlat_TypeName::xsdName` value for the underlying object.
    const char *xsdName(int format) const;
};

// FREE FUNCTIONS
// Customization-point functions

template <class t_ACCESSOR>
int bdlat_typeCategoryAccessArray(const AnyConstRef& any,
                                  t_ACCESSOR&        accessor);
template <class t_ACCESSOR>
int bdlat_typeCategoryAccessChoice(const AnyConstRef& any,
                                   t_ACCESSOR&        accessor);
template <class t_ACCESSOR>
int bdlat_typeCategoryAccessCustomizedType(const AnyConstRef& any,
                                           t_ACCESSOR&        accessor);
template <class t_ACCESSOR>
int bdlat_typeCategoryAccessEnumeration(const AnyConstRef& any,
                                        t_ACCESSOR&        accessor);
template <class t_ACCESSOR>
int bdlat_typeCategoryAccessNullableValue(const AnyConstRef& any,
                                          t_ACCESSOR&        accessor);
template <class t_ACCESSOR>
int bdlat_typeCategoryAccessSequence(const AnyConstRef& any,
                                     t_ACCESSOR&        accessor);
template <class t_ACCESSOR>
int bdlat_typeCategoryAccessSimple(const AnyConstRef& any,
                                   t_ACCESSOR&        accessor);

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

                           // ------------
                           // class AnyRef
                           // ------------

// CREATORS
inline
AnyRef::AnyRef(void *objectAddress, const SimpleTypeVtable *vtable)
: d_object_p(objectAddress)
, d_vtable(vtable)
, d_category(bdlat_TypeCategory::e_SIMPLE_CATEGORY)
{
}

inline
AnyRef::AnyRef(void *objectAddress, const EnumVtable *vtable)
: d_object_p(objectAddress)
, d_vtable(vtable)
, d_category(bdlat_TypeCategory::e_ENUMERATION_CATEGORY)
{
}

inline
AnyRef::AnyRef(void *objectAddress, const NullableValueVtable *vtable)
: d_object_p(objectAddress)
, d_vtable(vtable)
, d_category(bdlat_TypeCategory::e_NULLABLE_VALUE_CATEGORY)
{
}

inline
AnyRef::AnyRef(void *objectAddress, const ArrayVtable *vtable)
: d_object_p(objectAddress)
, d_vtable(vtable)
, d_category(bdlat_TypeCategory::e_ARRAY_CATEGORY)
{
}

inline
AnyRef::AnyRef(void *objectAddress, const SequenceVtable *vtable)
: d_object_p(objectAddress)
, d_vtable(vtable)
, d_category(bdlat_TypeCategory::e_SEQUENCE_CATEGORY)
{
}

inline
AnyRef::AnyRef(void *objectAddress, const ChoiceVtable *vtable)
: d_object_p(objectAddress)
, d_vtable(vtable)
, d_category(bdlat_TypeCategory::e_CHOICE_CATEGORY)
{
}

inline
AnyRef::AnyRef(void *objectAddress, const CustomizedTypeVtable *vtable)
: d_object_p(objectAddress)
, d_vtable(vtable)
, d_category(bdlat_TypeCategory::e_CUSTOMIZED_TYPE_CATEGORY)
{
}

inline
AnyRef::AnyRef(void *objectAddress, const DynamicTypeVtable *vtable)
: d_object_p(objectAddress)
, d_vtable(vtable)
, d_category(bdlat_TypeCategory::e_DYNAMIC_CATEGORY)
{
}

inline
AnyRef::AnyRef(void                      *objectAddress,
               AnyVtable                  vtable,
               bdlat_TypeCategory::Value  category)
: d_object_p(objectAddress)
, d_vtable(vtable)
, d_category(category)
{
}

inline
AnyRef::AnyRef(const AnyRefData& data)
: d_object_p(data.d_object_p)
, d_vtable(data.d_vtable)
, d_category(data.d_category)
{
}

// ACCESSORS
inline
const ArrayVtable *AnyRef::arrayVtable() const
{
    return d_vtable.d_array_p;
}

inline
const ChoiceVtable *AnyRef::choiceVtable() const
{
    return d_vtable.d_choice_p;
}

inline
const CustomizedTypeVtable *AnyRef::customizedTypeVtable() const
{
    return d_vtable.d_customized_p;
}

inline
const DynamicTypeVtable *AnyRef::dynamicTypeVtable() const
{
    return d_vtable.d_dynamic_p;
}

inline
const EnumVtable *AnyRef::enumVtable() const
{
    return d_vtable.d_enum_p;
}

inline
const NullableValueVtable *AnyRef::nullableValueVtable() const
{
    return d_vtable.d_nullable_p;
}

inline
const SequenceVtable *AnyRef::sequenceVtable() const
{
    return d_vtable.d_sequence_p;
}

inline
const SimpleTypeVtable *AnyRef::simpleTypeVtable() const
{
    return d_vtable.d_simple_p;
}

inline
void *AnyRef::objectAddress() const
{
    return d_object_p;
}

inline
bdlat_TypeCategory::Value AnyRef::typeCategory() const
{
    return d_category;
}

// FREE FUNCTIONS
// Customization-point functions
inline
bdlat_TypeCategory::Value bdlat_typeCategorySelect(const AnyRef& any)
{
    return any.typeCategory();
}

inline
void bdlat_valueTypeReset(AnyRef *ref)
{
    ref->reset();
}

// The others are defined in bdlar_anymanipulators.h

                           // -----------------
                           // class AnyConstRef
                           // -----------------

// CREATORS
inline
AnyConstRef::AnyConstRef(const void                  *objectAddress,
                         const SimpleTypeConstVtable *vtable)
: d_object_p(objectAddress)
, d_vtable(vtable)
, d_category(bdlat_TypeCategory::e_SIMPLE_CATEGORY)
{
}

inline
AnyConstRef::AnyConstRef(const void            *objectAddress,
                         const EnumConstVtable *vtable)
: d_object_p(objectAddress)
, d_vtable(vtable)
, d_category(bdlat_TypeCategory::e_ENUMERATION_CATEGORY)
{
}

inline
AnyConstRef::AnyConstRef(const void                     *objectAddress,
                         const NullableValueConstVtable *vtable)
: d_object_p(objectAddress)
, d_vtable(vtable)
, d_category(bdlat_TypeCategory::e_NULLABLE_VALUE_CATEGORY)
{
}

inline
AnyConstRef::AnyConstRef(const void             *objectAddress,
                         const ArrayConstVtable *vtable)
: d_object_p(objectAddress)
, d_vtable(vtable)
, d_category(bdlat_TypeCategory::e_ARRAY_CATEGORY)
{
}

inline
AnyConstRef::AnyConstRef(const void                *objectAddress,
                         const SequenceConstVtable *vtable)
: d_object_p(objectAddress)
, d_vtable(vtable)
, d_category(bdlat_TypeCategory::e_SEQUENCE_CATEGORY)
{
}

inline
AnyConstRef::AnyConstRef(const void              *objectAddress,
                         const ChoiceConstVtable *vtable)
: d_object_p(objectAddress)
, d_vtable(vtable)
, d_category(bdlat_TypeCategory::e_CHOICE_CATEGORY)
{
}

inline
AnyConstRef::AnyConstRef(const void                   *objectAddress,
                         const DynamicTypeConstVtable *vtable)
: d_object_p(objectAddress)
, d_vtable(vtable)
, d_category(bdlat_TypeCategory::e_DYNAMIC_CATEGORY)
{
}

inline
AnyConstRef::AnyConstRef(const void                      *objectAddress,
                         const CustomizedTypeConstVtable *vtable)
: d_object_p(objectAddress)
, d_vtable(vtable)
, d_category(bdlat_TypeCategory::e_CUSTOMIZED_TYPE_CATEGORY)
{
}

inline
AnyConstRef::AnyConstRef(const void                *objectAddress,
                         AnyConstVtable             vtable,
                         bdlat_TypeCategory::Value  category)
: d_object_p(objectAddress)
, d_vtable(vtable)
, d_category(category)
{
}

inline
AnyConstRef::AnyConstRef(const AnyConstRefData& data)
: d_object_p(data.d_object_p)
, d_vtable(data.d_vtable)
, d_category(data.d_category)
{
}

// ACCESSORS
inline
const ArrayConstVtable *AnyConstRef::arrayVtable() const
{
    return d_vtable.d_array_p;
}

inline
const ChoiceConstVtable *AnyConstRef::choiceVtable() const
{
    return d_vtable.d_choice_p;
}

inline
const CustomizedTypeConstVtable *AnyConstRef::customizedTypeVtable() const
{
    return d_vtable.d_customized_p;
}

inline
const DynamicTypeConstVtable *AnyConstRef::dynamicTypeVtable() const
{
    return d_vtable.d_dynamic_p;
}

inline
const EnumConstVtable *AnyConstRef::enumVtable() const
{
    return d_vtable.d_enum_p;
}

inline
const NullableValueConstVtable *AnyConstRef::nullableValueVtable() const
{
    return d_vtable.d_nullable_p;
}

inline
const SequenceConstVtable *AnyConstRef::sequenceVtable() const
{
    return d_vtable.d_sequence_p;
}

inline
const SimpleTypeConstVtable *AnyConstRef::simpleTypeVtable() const
{
    return d_vtable.d_simple_p;
}

inline
const void *AnyConstRef::objectAddress() const
{
    return d_object_p;
}

inline
bdlat_TypeCategory::Value AnyConstRef::typeCategory() const
{
    return d_category;
}

// FREE FUNCTIONS
// Customization-point functions
inline
const char *bdlat_TypeName_xsdName(const AnyConstRef& ref, int format)
{
    return ref.xsdName(format);
}

inline
bdlat_TypeCategory::Value bdlat_typeCategorySelect(const AnyConstRef& any)
{
    return any.typeCategory();
}

// The others are defined in bdlar_anymanipulators.h

}  // close package namespace

template <>
struct bdlat_TypeCategoryDeclareDynamic<bdlar::AnyRef> : bsl::true_type {
};

template <>
struct bdlat_TypeCategoryDeclareDynamic<bdlar::AnyConstRef> : bsl::true_type {
};

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
