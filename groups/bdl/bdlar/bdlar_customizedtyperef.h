// bdlar_customizedtyperef.h                                          -*-C++-*-
#ifndef INCLUDED_BDLAR_CUSTOMIZEDTYPEREF
#define INCLUDED_BDLAR_CUSTOMIZEDTYPEREF

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide ...
//
//@CLASSES:
//  bdlar::CustomizedTypeRef: ...
//  bdlar::CustomizedTypeConstRef: ...
//
//@DESCRIPTION: This component provides ...
//

#include <bdlar_anyref.h>
#include <bdlar_customizedtypevtable.h>
#include <bdlar_customizedtypevtableutil.h>
#include <bdlar_typecategory.h>

#include <bdlat_valuetypefunctions.h>

#include <bslmf_enableif.h>

namespace BloombergLP {
namespace bdlar {

                           // =======================
                           // class CustomizedTypeRef
                           // =======================

class CustomizedTypeRef {
    // DATA
    void                       *d_object_p;
    const CustomizedTypeVtable *d_vtable_p;
    mutable AnyConstRef         d_baseAny; // required only to
                                           // return `const AnyConstRef&`

  public:
    // CREATORS

    /// Create `CustomizedTypeRef` object that type erases the supplied
    /// "customized type" object.
    template <class t_TYPE>
    explicit CustomizedTypeRef(t_TYPE& object,
                               typename bsl::enable_if<
                                   IsCustomized<t_TYPE>::value>::type * = 0);

    /// Create `CustomizedTypeRef` object.  The behaviour is undefined unless
    /// `objectAddress` and `vtable` point to the valid objects.
    CustomizedTypeRef(void *objectAddress, const CustomizedTypeVtable *vtable);

    // MANIPULATORS

    /// Assign a value converted from the specified base `value`.  Return 0 if
    /// successful and non-zero otherwise.
    template <class t_BASE_TYPE>
    int convertFromBaseType(const t_BASE_TYPE &value) const;

    /// Create an uninitialized value of the base type, apply the specified
    /// `baseManipulator` to it, then call the
    /// `bdlat_CustomizedTypeFunctions::convertFromBaseType` function with this
    /// object and the value.
    template <class t_MANIPULATOR>
    int createBaseAndConvert(t_MANIPULATOR& baseManipulator) const;

    /// Reset the referred object to the default value.
    void reset() const;

    // ACCESSORS

    /// Return a type category of the base type.
    bdlat_TypeCategory::Value baseCategory() const;

    /// Return a reference to `AnyConstRef` that refers to the base value.
    /// Used for the `bdlat_customizedTypeConvertToBaseType` function
    /// implementation.
    const AnyConstRef& baseRef() const;

    /// Return a reference to the base value.
    AnyConstRef convertToBaseType() const;

    /// Return a pointer to the vtable.
    const CustomizedTypeVtable *vtable() const;

    /// Return a pointer to the wrapped object.
    void *objectAddress() const;
};

                           // ============================
                           // class CustomizedTypeConstRef
                           // ============================

class CustomizedTypeConstRef {
    // DATA
    const void                      *d_object_p;
    const CustomizedTypeConstVtable *d_vtable_p;
    mutable AnyConstRef              d_baseAny; // required only to
                                                // return `const AnyConstRef&`

  public:
    // CREATORS

    /// Create `CustomizedTypeConstRef` object that type erases the supplied
    /// "customized type" object.
    template <class t_TYPE>
    explicit CustomizedTypeConstRef(
                                 const t_TYPE& object,
                                 typename bsl::enable_if<
                                     IsCustomized<t_TYPE>::value>::type * = 0);

    /// Create `CustomizedTypeConstRef` object.  The behaviour is undefined
    /// unless `objectAddress` and `vtable` point to the valid objects.
    CustomizedTypeConstRef(const void                      *objectAddress,
                           const CustomizedTypeConstVtable *vtable);

    /// Create `CustomizedTypeConstRef` object that wraps the same object as
    /// the specified `object`.
    CustomizedTypeConstRef(const CustomizedTypeRef& object); // IMPLICIT

    // ACCESSORS

    /// Return a type category of the base type.
    bdlat_TypeCategory::Value baseCategory() const;

    /// Return a reference to `AnyConstRef` that refers to the base value.
    /// Used for the `bdlat_customizedTypeConvertToBaseType` function
    /// implementation.
    const AnyConstRef& baseRef() const;

    /// Return a reference to the base value.
    AnyConstRef convertToBaseType() const;

    /// Return a pointer to the vtable.
    const CustomizedTypeConstVtable *vtable() const;

    /// Return a pointer to the wrapped object.
    const void *objectAddress() const;
};

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

                           // -----------------------
                           // class CustomizedTypeRef
                           // -----------------------

// CREATORS
template <class t_TYPE>
inline
CustomizedTypeRef::CustomizedTypeRef(t_TYPE& object,
                                     typename bsl::enable_if<
                                         IsCustomized<t_TYPE>::value>::type *)
: d_object_p(&object)
, d_vtable_p(CustomizedTypeVtableUtil::getVtable<t_TYPE>())
, d_baseAny(d_object_p, &d_vtable_p->d_const)//any parameters just to construct
{
}

inline
CustomizedTypeRef::CustomizedTypeRef(void                       *objectAddress,
                                     const CustomizedTypeVtable *vtable)
: d_object_p(objectAddress)
, d_vtable_p(vtable)
, d_baseAny(objectAddress, &vtable->d_const) //any parameters just to construct
{
}

// MANIPULATORS
template <class t_BASE_TYPE>
int CustomizedTypeRef::convertFromBaseType(const t_BASE_TYPE &value) const
{
    return d_vtable_p->d_convertFromBaseType_fp(d_object_p,
                                                &value,
                                                typeid(t_BASE_TYPE));
}

template <class t_MANIPULATOR>
inline
int CustomizedTypeRef::createBaseAndConvert(
                                          t_MANIPULATOR& baseManipulator) const
{
    typename FakeDependency<t_MANIPULATOR, ManipulatorRef>::Type
                                               manipulatorRef(baseManipulator);
    return d_vtable_p->d_createBaseAndConvert_fp(d_object_p, manipulatorRef);
}

inline
void CustomizedTypeRef::reset() const
{
    d_vtable_p->d_reset_fp(d_object_p);
}

// ACCESSORS
inline
bdlat_TypeCategory::Value CustomizedTypeRef::baseCategory() const
{
    return d_vtable_p->d_const.d_baseCategory_fp();
}

inline
const AnyConstRef& CustomizedTypeRef::baseRef() const
{
    return d_baseAny = convertToBaseType();
}

inline
AnyConstRef CustomizedTypeRef::convertToBaseType() const
{
    return d_vtable_p->d_const.d_convertToBaseType_fp(d_object_p);
}

inline
const CustomizedTypeVtable *CustomizedTypeRef::vtable() const
{
    return d_vtable_p;
}

inline
void *CustomizedTypeRef::objectAddress() const
{
    return d_object_p;
}

// FREE FUNCTIONS
// Customization-point functions (`bdlat_CustomizedTypeFunctions`)

template <class t_MANIPULATOR>
inline
int bdlat_customizedTypeCreateBaseAndConvert(
                                            CustomizedTypeRef *ref,
                                            t_MANIPULATOR&     baseManipulator)
{
    return ref->createBaseAndConvert(baseManipulator);
}

template <class t_BASE_TYPE>
inline
int bdlat_customizedTypeConvertFromBaseType(CustomizedTypeRef  *ref,
                                            const t_BASE_TYPE&  value)
{
    return ref->convertFromBaseType(value);
}

inline
const AnyConstRef& bdlat_customizedTypeConvertToBaseType(
                                                  const CustomizedTypeRef& ref)
{
    return ref.baseRef();
}

inline
void bdlat_valueTypeReset(CustomizedTypeRef *ref)
{
    ref->reset();
}

                           // ----------------------------
                           // class CustomizedTypeConstRef
                           // ----------------------------

// CREATORS
template <class t_TYPE>
inline
CustomizedTypeConstRef::CustomizedTypeConstRef(
                                      const t_TYPE& object,
                                      typename bsl::enable_if<
                                          IsCustomized<t_TYPE>::value>::type *)
: d_object_p(&object)
, d_vtable_p(CustomizedTypeVtableUtil::getConstVtable<t_TYPE>())
, d_baseAny(d_object_p, d_vtable_p) // any parameters just to construct
{
}

inline
CustomizedTypeConstRef::CustomizedTypeConstRef(
                                const void                      *objectAddress,
                                const CustomizedTypeConstVtable *vtable)
: d_object_p(objectAddress)
, d_vtable_p(vtable)
, d_baseAny(objectAddress, vtable) // any parameters just to construct
{
}

inline
CustomizedTypeConstRef::CustomizedTypeConstRef(const CustomizedTypeRef& object)
: d_object_p(object.objectAddress())
, d_vtable_p(&object.vtable()->d_const)
, d_baseAny(d_object_p, d_vtable_p) // any parameters just to construct
{
}

// ACCESSORS
inline
bdlat_TypeCategory::Value CustomizedTypeConstRef::baseCategory() const
{
    return d_vtable_p->d_baseCategory_fp();
}

inline
const AnyConstRef& CustomizedTypeConstRef::baseRef() const
{
    return d_baseAny = convertToBaseType();
}

inline
AnyConstRef CustomizedTypeConstRef::convertToBaseType() const
{
    return d_vtable_p->d_convertToBaseType_fp(d_object_p);
}

inline
const CustomizedTypeConstVtable *CustomizedTypeConstRef::vtable() const
{
    return d_vtable_p;
}

inline
const void *CustomizedTypeConstRef::objectAddress() const
{
    return d_object_p;
}

// FREE FUNCTIONS
// Customization-point functions (`bdlat_CustomizedTypeFunctions`)

inline
const AnyConstRef& bdlat_customizedTypeConvertToBaseType(
                                             const CustomizedTypeConstRef& ref)
{
    return ref.baseRef();
}

}  // close package namespace

namespace bdlat_CustomizedTypeFunctions {

template <>
struct IsCustomizedType<bdlar::CustomizedTypeRef> : bsl::true_type {
};

template <>
struct IsCustomizedType<bdlar::CustomizedTypeConstRef> : bsl::true_type {
};

template <>
struct BaseType<bdlar::CustomizedTypeRef> {
    typedef bdlar::AnyConstRef Type;
};

template <>
struct BaseType<bdlar::CustomizedTypeConstRef> {
    typedef bdlar::AnyConstRef Type;
};

}  // close namespace bdlat_CustomizedTypeFunctions
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
