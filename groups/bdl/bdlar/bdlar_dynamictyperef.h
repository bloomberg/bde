// bdlar_dynamictyperef.h                                             -*-C++-*-
#ifndef INCLUDED_BDLAR_DYNAMICTYPEREF
#define INCLUDED_BDLAR_DYNAMICTYPEREF

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide ...
//
//@CLASSES:
//  bdlar::DynamicTypeRef: ...
//  bdlar::DynamicTypeConstRef: ...
//
//@DESCRIPTION: This component provides ...
//

#include <bdlar_anyrefdata.h>
#include <bdlar_dynamictypevtable.h>
#include <bdlar_dynamictypevtableutil.h>

#include <bdlar_arrayref.h>
#include <bdlar_choiceref.h>
#include <bdlar_customizedtyperef.h>
#include <bdlar_enumref.h>
#include <bdlar_nullablevalueref.h>
#include <bdlar_sequenceref.h>
#include <bdlar_simpletyperef.h>
#include <bdlar_typecategory.h>

#include <bdlat_valuetypefunctions.h>

#include <bsl_type_traits.h>

namespace BloombergLP {
namespace bdlar {

                           // ====================
                           // class DynamicTypeRef
                           // ====================

class DynamicTypeRef {
    // DATA
    void                    *d_object_p;
    const DynamicTypeVtable *d_vtable_p;

  public:
    // CREATORS

    /// Create `DynamicTypeRef` object that type erases the supplied `dynamic`
    /// object.
    template <class t_DYNAMIC>
    explicit DynamicTypeRef(t_DYNAMIC& dynamic,
                            typename bsl::enable_if<
                                IsDynamic<t_DYNAMIC>::value>::type * = 0);

    /// Create `DynamicTypeRef` object.  The behaviour is undefined unless
    /// `objectAddress` and `vtable` point to the valid objects.
    DynamicTypeRef(void *objectAddress, const DynamicTypeVtable *vtable);

    // MANIPULATORS

    /// Reset the referred object to the default value.
    void reset() const;

    /// Return a reference to the (modifiable) value.  The behavior is
    /// undefined if the referred object is null.
    AnyRefData valueRef() const;

    // ACCESSORS

    /// Return a pointer to the vtable.
    const DynamicTypeVtable *vtable() const;

    /// Return a pointer to the wrapped object.
    void *objectAddress() const;

    /// Return type category of the referred object.
    bdlat_TypeCategory::Value typeCategory() const;

    /// Return a reference to the (non-modifiable) value.  The behavior is
    /// undefined if the referred object is null.
    AnyConstRefData valueConstRef() const;
};

                           // =========================
                           // class DynamicTypeConstRef
                           // =========================

class DynamicTypeConstRef {
    // DATA
    const void                   *d_object_p;
    const DynamicTypeConstVtable *d_vtable_p;

  public:
    // CREATORS

    /// Create `DynamicTypeConstRef` object that type erases the supplied
    /// `dynamic` object.
    template <class t_DYNAMIC>
    explicit DynamicTypeConstRef(const t_DYNAMIC& dynamic,
                                 typename bsl::enable_if<
                                     IsDynamic<t_DYNAMIC>::value>::type * = 0);

    /// Create `DynamicTypeConstRef` object.  The behaviour is undefined unless
    /// `objectAddress` and `vtable` point to the valid objects.
    DynamicTypeConstRef(const void                   *objectAddress,
                        const DynamicTypeConstVtable *vtable);

    /// Create `DynamicTypeConstRef` object that wraps the same object as the
    /// specified `object`.
    DynamicTypeConstRef(const DynamicTypeRef& object); // IMPLICIT

    // ACCESSORS

    /// Return a pointer to the vtable.
    const DynamicTypeConstVtable *vtable() const;

    /// Return a pointer to the wrapped object.
    const void *objectAddress() const;

    /// Return type category of the referred object.
    bdlat_TypeCategory::Value typeCategory() const;

    /// Return a reference to the (non-modifiable) value.  The behavior is
    /// undefined if the referred object is null.
    AnyConstRefData valueConstRef() const;

    /// Return the `bdlat_TypeName::xsdName` value for the underlying object.
    const char *xsdName(int format) const;
};

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

                           // --------------------
                           // class DynamicTypeRef
                           // --------------------

// CREATORS
template <class t_DYNAMIC>
inline
DynamicTypeRef::DynamicTypeRef(t_DYNAMIC& dynamic,
                               typename bsl::enable_if<
                                   IsDynamic<t_DYNAMIC>::value>::type *)
: d_object_p(&dynamic)
, d_vtable_p(DynamicTypeVtableUtil::getVtable<t_DYNAMIC>())
{
}

inline
DynamicTypeRef::DynamicTypeRef(void                    *objectAddress,
                               const DynamicTypeVtable *vtable)
: d_object_p(objectAddress)
, d_vtable_p(vtable)
{
}

// MANIPULATORS
inline
void DynamicTypeRef::reset() const
{
    return d_vtable_p->d_reset_fp(d_object_p);
}

inline
AnyRefData DynamicTypeRef::valueRef() const
{
    return d_vtable_p->d_valueRef_fp(d_object_p);
}

// ACCESSORS
inline
const DynamicTypeVtable *DynamicTypeRef::vtable() const
{
    return d_vtable_p;
}

inline
void *DynamicTypeRef::objectAddress() const
{
    return d_object_p;
}

inline
bdlat_TypeCategory::Value DynamicTypeRef::typeCategory() const
{
    return d_vtable_p->d_const.d_typeCategory_fp(d_object_p);
}

inline
AnyConstRefData DynamicTypeRef::valueConstRef() const
{
    return d_vtable_p->d_const.d_valueRef_fp(d_object_p);
}

// FREE FUNCTIONS
// Customization-point functions

inline
bdlat_TypeCategory::Value bdlat_typeCategorySelect(const DynamicTypeRef& ref)
{
    return ref.typeCategory();
}

template <class t_MANIPULATOR>
inline
int bdlat_typeCategoryManipulateArray(DynamicTypeRef *ref,
                                      t_MANIPULATOR&  manipulator)
{
    AnyRefData any = ref->valueRef();
    if (any.d_category == bdlat_TypeCategory::e_ARRAY_CATEGORY) {
        ArrayRef ref(any.d_object_p, any.d_vtable.d_array_p);
        return manipulator(&ref, bdlat_TypeCategory::Array());        // RETURN
    }
    return manipulator(ref, bslmf::Nil());
}

template <class t_MANIPULATOR>
inline
int bdlat_typeCategoryManipulateChoice(DynamicTypeRef *ref,
                                       t_MANIPULATOR&  manipulator)
{
    AnyRefData any = ref->valueRef();
    if (any.d_category == bdlat_TypeCategory::e_CHOICE_CATEGORY) {
        ChoiceRef ref(any.d_object_p, any.d_vtable.d_choice_p);
        return manipulator(&ref, bdlat_TypeCategory::Choice());       // RETURN
    }
    return manipulator(ref, bslmf::Nil());
}

template <class t_MANIPULATOR>
inline
int bdlat_typeCategoryManipulateCustomizedType(DynamicTypeRef  *ref,
                                               t_MANIPULATOR&   manipulator)
{
    AnyRefData any = ref->valueRef();
    if (any.d_category == bdlat_TypeCategory::e_CUSTOMIZED_TYPE_CATEGORY) {
        CustomizedTypeRef ref(any.d_object_p, any.d_vtable.d_customized_p);
        return manipulator(&ref, bdlat_TypeCategory::CustomizedType());
                                                                      // RETURN
    }
    return manipulator(ref, bslmf::Nil());
}

template <class t_MANIPULATOR>
inline
int bdlat_typeCategoryManipulateEnumeration(DynamicTypeRef *ref,
                                            t_MANIPULATOR&  manipulator)
{
    AnyRefData any = ref->valueRef();
    if (any.d_category == bdlat_TypeCategory::e_ENUMERATION_CATEGORY) {
        EnumRef ref(any.d_object_p, any.d_vtable.d_enum_p);
        return manipulator(&ref, bdlat_TypeCategory::Enumeration());  // RETURN
    }
    return manipulator(ref, bslmf::Nil());
}

template <class t_MANIPULATOR>
inline
int bdlat_typeCategoryManipulateNullableValue(DynamicTypeRef *ref,
                                              t_MANIPULATOR&  manipulator)
{
    AnyRefData any = ref->valueRef();
    if (any.d_category == bdlat_TypeCategory::e_NULLABLE_VALUE_CATEGORY) {
        NullableValueRef ref(any.d_object_p, any.d_vtable.d_nullable_p);
        return manipulator(&ref, bdlat_TypeCategory::NullableValue());
                                                                      // RETURN
    }
    return manipulator(ref, bslmf::Nil());
}

template <class t_MANIPULATOR>
inline
int bdlat_typeCategoryManipulateSequence(DynamicTypeRef *ref,
                                         t_MANIPULATOR&  manipulator)
{
    AnyRefData any = ref->valueRef();
    if (any.d_category == bdlat_TypeCategory::e_SEQUENCE_CATEGORY) {
        SequenceRef ref(any.d_object_p, any.d_vtable.d_sequence_p);
        return manipulator(&ref, bdlat_TypeCategory::Sequence());     // RETURN
    }
    return manipulator(ref, bslmf::Nil());
}

template <class t_MANIPULATOR>
inline
int bdlat_typeCategoryManipulateSimple(DynamicTypeRef *ref,
                                       t_MANIPULATOR&  manipulator)
{
    AnyRefData any = ref->valueRef();
    if (any.d_category == bdlat_TypeCategory::e_SIMPLE_CATEGORY) {
        SimpleTypeRef ref(any.d_object_p, any.d_vtable.d_simple_p);
        return ref.manipulateWithCategory(manipulator);               // RETURN
    }
    return manipulator(ref, bslmf::Nil());
}

template <class t_ACCESSOR>
inline
int bdlat_typeCategoryAccessArray(const DynamicTypeRef& ref,
                                  t_ACCESSOR&           accessor)
{
    AnyRefData any = ref.valueRef();
    if (any.d_category == bdlat_TypeCategory::e_ARRAY_CATEGORY) {
        const ArrayRef ref(any.d_object_p, any.d_vtable.d_array_p);
        return accessor(ref, bdlat_TypeCategory::Array());            // RETURN
    }
    return accessor(ref, bslmf::Nil());
}

template <class t_ACCESSOR>
inline
int bdlat_typeCategoryAccessChoice(const DynamicTypeRef& ref,
                                   t_ACCESSOR&           accessor)
{
    AnyRefData any = ref.valueRef();
    if (any.d_category == bdlat_TypeCategory::e_CHOICE_CATEGORY) {
        const ChoiceRef ref(any.d_object_p, any.d_vtable.d_choice_p);
        return accessor(ref, bdlat_TypeCategory::Choice());           // RETURN
    }
    return accessor(ref, bslmf::Nil());
}

template <class t_ACCESSOR>
inline
int bdlat_typeCategoryAccessCustomizedType(const DynamicTypeRef& ref,
                                           t_ACCESSOR&           accessor)
{
    AnyRefData any = ref.valueRef();
    if (any.d_category == bdlat_TypeCategory::e_CUSTOMIZED_TYPE_CATEGORY) {
        const CustomizedTypeRef ref(any.d_object_p,
                                    any.d_vtable.d_customized_p);
        return accessor(ref, bdlat_TypeCategory::CustomizedType());   // RETURN
    }
    return accessor(ref, bslmf::Nil());
}

template <class t_ACCESSOR>
inline
int bdlat_typeCategoryAccessEnumeration(const DynamicTypeRef& ref,
                                        t_ACCESSOR&           accessor)
{
    AnyRefData any = ref.valueRef();
    if (any.d_category == bdlat_TypeCategory::e_ENUMERATION_CATEGORY) {
        const EnumRef ref(any.d_object_p, any.d_vtable.d_enum_p);
        return accessor(ref, bdlat_TypeCategory::Enumeration());      // RETURN
    }
    return accessor(ref, bslmf::Nil());
}

template <class t_ACCESSOR>
inline
int bdlat_typeCategoryAccessNullableValue(const DynamicTypeRef& ref,
                                          t_ACCESSOR&           accessor)
{
    AnyRefData any = ref.valueRef();
    if (any.d_category == bdlat_TypeCategory::e_NULLABLE_VALUE_CATEGORY) {
        const NullableValueRef ref(any.d_object_p, any.d_vtable.d_nullable_p);
        return accessor(ref, bdlat_TypeCategory::NullableValue());    // RETURN
    }
    return accessor(ref, bslmf::Nil());
}

template <class t_ACCESSOR>
inline
int bdlat_typeCategoryAccessSequence(const DynamicTypeRef& ref,
                                     t_ACCESSOR&           accessor)
{
    AnyRefData any = ref.valueRef();
    if (any.d_category == bdlat_TypeCategory::e_SEQUENCE_CATEGORY) {
        const SequenceRef ref(any.d_object_p, any.d_vtable.d_sequence_p);
        return accessor(ref, bdlat_TypeCategory::Sequence());         // RETURN
    }
    return accessor(ref, bslmf::Nil());
}

template <class t_ACCESSOR>
inline
int bdlat_typeCategoryAccessSimple(const DynamicTypeRef& ref,
                                   t_ACCESSOR&           accessor)
{
    AnyRefData any = ref.valueRef();
    if (any.d_category == bdlat_TypeCategory::e_SIMPLE_CATEGORY) {
        const SimpleTypeRef ref(any.d_object_p, any.d_vtable.d_simple_p);
        return ref.accessWithCategory(accessor);                      // RETURN
    }
    return accessor(ref, bslmf::Nil());
}

inline
void bdlat_valueTypeReset(DynamicTypeRef *ref)
{
    ref->reset();
}

                           // -------------------------
                           // class DynamicTypeConstRef
                           // -------------------------

// CREATORS
template <class t_DYNAMIC>
inline
DynamicTypeConstRef::DynamicTypeConstRef(const t_DYNAMIC& dynamic,
                                         typename bsl::enable_if<
                                             IsDynamic<t_DYNAMIC>::value
                                         >::type *)
: d_object_p(&dynamic)
, d_vtable_p(DynamicTypeVtableUtil::getConstVtable<t_DYNAMIC>())
{
}

inline
DynamicTypeConstRef::DynamicTypeConstRef(
                                   const void                   *objectAddress,
                                   const DynamicTypeConstVtable *vtable)
: d_object_p(objectAddress)
, d_vtable_p(vtable)
{
}

inline
DynamicTypeConstRef::DynamicTypeConstRef(const DynamicTypeRef& object)
: d_object_p(object.objectAddress())
, d_vtable_p(&object.vtable()->d_const)
{
}

// ACCESSORS
inline
const DynamicTypeConstVtable *DynamicTypeConstRef::vtable() const
{
    return d_vtable_p;
}

inline
const void *DynamicTypeConstRef::objectAddress() const
{
    return d_object_p;
}

inline
bdlat_TypeCategory::Value DynamicTypeConstRef::typeCategory() const
{
    return d_vtable_p->d_typeCategory_fp(d_object_p);
}

inline
AnyConstRefData DynamicTypeConstRef::valueConstRef() const
{
    return d_vtable_p->d_valueRef_fp(d_object_p);
}

inline
const char *DynamicTypeConstRef::xsdName(int format) const
{
    return d_vtable_p->d_xsdName_fp(d_object_p, format);
}

// FREE FUNCTIONS
// Customization-point functions

inline
bdlat_TypeCategory::Value bdlat_typeCategorySelect(
                                                const DynamicTypeConstRef& ref)
{
    return ref.typeCategory();
}

template <class t_ACCESSOR>
inline
int bdlat_typeCategoryAccessArray(const DynamicTypeConstRef& ref,
                                  t_ACCESSOR&                accessor)
{
    AnyConstRefData any = ref.valueConstRef();
    if (any.d_category == bdlat_TypeCategory::e_ARRAY_CATEGORY) {
        const ArrayConstRef ref(any.d_object_p, any.d_vtable.d_array_p);
        return accessor(ref, bdlat_TypeCategory::Array());            // RETURN
    }
    return accessor(ref, bslmf::Nil());
}

template <class t_ACCESSOR>
inline
int bdlat_typeCategoryAccessChoice(const DynamicTypeConstRef& ref,
                                   t_ACCESSOR&                accessor)
{
    AnyConstRefData any = ref.valueConstRef();
    if (any.d_category == bdlat_TypeCategory::e_CHOICE_CATEGORY) {
        const ChoiceConstRef ref(any.d_object_p, any.d_vtable.d_choice_p);
        return accessor(ref, bdlat_TypeCategory::Choice());           // RETURN
    }
    return accessor(ref, bslmf::Nil());
}

template <class t_ACCESSOR>
inline
int bdlat_typeCategoryAccessCustomizedType(const DynamicTypeConstRef& ref,
                                           t_ACCESSOR&                accessor)
{
    AnyConstRefData any = ref.valueConstRef();
    if (any.d_category == bdlat_TypeCategory::e_CUSTOMIZED_TYPE_CATEGORY) {
        const CustomizedTypeConstRef ref(any.d_object_p,
                                         any.d_vtable.d_customized_p);
        return accessor(ref, bdlat_TypeCategory::CustomizedType());   // RETURN
    }
    return accessor(ref, bslmf::Nil());
}

template <class t_ACCESSOR>
inline
int bdlat_typeCategoryAccessEnumeration(const DynamicTypeConstRef& ref,
                                        t_ACCESSOR&                accessor)
{
    AnyConstRefData any = ref.valueConstRef();
    if (any.d_category == bdlat_TypeCategory::e_ENUMERATION_CATEGORY) {
        const EnumConstRef ref(any.d_object_p, any.d_vtable.d_enum_p);
        return accessor(ref, bdlat_TypeCategory::Enumeration());      // RETURN
    }
    return accessor(ref, bslmf::Nil());
}

template <class t_ACCESSOR>
inline
int bdlat_typeCategoryAccessNullableValue(const DynamicTypeConstRef& ref,
                                          t_ACCESSOR&                accessor)
{
    AnyConstRefData any = ref.valueConstRef();
    if (any.d_category == bdlat_TypeCategory::e_NULLABLE_VALUE_CATEGORY) {
        const NullableValueConstRef ref(any.d_object_p,
                                        any.d_vtable.d_nullable_p);
        return accessor(ref, bdlat_TypeCategory::NullableValue());    // RETURN
    }
    return accessor(ref, bslmf::Nil());
}

template <class t_ACCESSOR>
inline
int bdlat_typeCategoryAccessSequence(const DynamicTypeConstRef& ref,
                                     t_ACCESSOR&                accessor)
{
    AnyConstRefData any = ref.valueConstRef();
    if (any.d_category == bdlat_TypeCategory::e_SEQUENCE_CATEGORY) {
        const SequenceConstRef ref(any.d_object_p, any.d_vtable.d_sequence_p);
        return accessor(ref, bdlat_TypeCategory::Sequence());         // RETURN
    }
    return accessor(ref, bslmf::Nil());
}

template <class t_ACCESSOR>
inline
int bdlat_typeCategoryAccessSimple(const DynamicTypeConstRef& ref,
                                   t_ACCESSOR&                accessor)
{
    AnyConstRefData any = ref.valueConstRef();
    if (any.d_category == bdlat_TypeCategory::e_SIMPLE_CATEGORY) {
        const SimpleTypeConstRef ref(any.d_object_p, any.d_vtable.d_simple_p);
        return ref.accessWithCategory(accessor);                      // RETURN
    }
    return accessor(ref, bslmf::Nil());
}

inline
const char *bdlat_TypeName_xsdName(const DynamicTypeConstRef& ref, int format)
{
    return ref.xsdName(format);
}

}  // close package namespace

template <>
struct bdlat_TypeCategoryDeclareDynamic<bdlar::DynamicTypeRef>
    : bsl::true_type {
};

template <>
struct bdlat_TypeCategoryDeclareDynamic<bdlar::DynamicTypeConstRef>
    : bsl::true_type {
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
