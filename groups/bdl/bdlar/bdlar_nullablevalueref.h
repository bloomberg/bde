// bdlar_nullablevalueref.h                                           -*-C++-*-
#ifndef INCLUDED_BDLAR_NULLABLEVALUEREF
#define INCLUDED_BDLAR_NULLABLEVALUEREF

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide ...
//
//@CLASSES:
//  bdlar::NullableValueRef: ...
//  bdlar::NullableValueConstRef: ...
//
//@DESCRIPTION: This component provides ...
//

#include <bdlar_nullablevaluevtable.h>
#include <bdlar_nullablevaluevtableutil.h>
#include <bdlar_typecategory.h>

#include <bdlat_valuetypefunctions.h>

#include <bslmf_enableif.h>

namespace BloombergLP {
namespace bdlar {

// Forward
class AnyRef;
class AnyConstRef;

                           // ======================
                           // class NullableValueRef
                           // ======================

class NullableValueRef {
    // DATA
    void                      *d_object_p;
    const NullableValueVtable *d_vtable_p;

  public:
    // TYPES
    typedef bdlat_TypeCategory::NullableValue CategoryTag;

    // CREATORS

    /// Create `NullableValueRef` object that type erases the supplied
    /// `nullable` object.
    template <class t_NULLABLE>
    explicit NullableValueRef(t_NULLABLE& nullable,
                              typename bsl::enable_if<
                                  IsNullable<t_NULLABLE>::value>::type * = 0);

    /// Create `NullableValueRef` object.  The behaviour is undefined unless
    /// `objectAddress` and `vtable` point to the valid objects.
    NullableValueRef(void *objectAddress, const NullableValueVtable *vtable);

    // MANIPULATORS

    /// Assign to the referred object the default value for the contained type.
    void makeValue() const;

    /// Invoke the specified `manipulator` on the referred non-null value.  The
    /// supplied `manipulator` must be a callable type that can be called as if
    /// it had the following signature:
    /// ```
    /// int manipulator(VALUE *refOrValue);
    /// ```
    /// Return the value from the invocation of `manipulator`.  The behavior is
    /// undefined if the referred object is null.
    template <class t_MANIPULATOR>
    int manipulateValue(t_MANIPULATOR& manipulator) const;

    /// Reset the referred object to the default value.
    void reset() const;

    // ACCESSORS

    /// Invoke the specified `accessor` on the referred non-null value.  The
    /// supplied `accessor` must be a callable type that can be called as if it
    /// had the following signature:
    /// ```
    /// int accessor(const VALUE& refOrValue);
    /// ```
    /// Return the value from the invocation of `accessor`.  The behavior is
    /// undefined if the referred object is null.
    template <class t_ACCESSOR>
    int accessValue(t_ACCESSOR& accessor) const;

    /// Return a pointer to the vtable.
    const NullableValueVtable *vtable() const;

    /// Return `true` if the referred object contains a null value, and `false`
    /// otherwise.
    bool isNull() const;

    /// Return a pointer to the wrapped object.
    void *objectAddress() const;

    /// Return the value type category.
    bdlat_TypeCategory::Value valueCategory() const;
};

                           // ===========================
                           // class NullableValueConstRef
                           // ===========================

class NullableValueConstRef {
    // DATA
    const void                     *d_object_p;
    const NullableValueConstVtable *d_vtable_p;

  public:
    // TYPES
    typedef bdlat_TypeCategory::NullableValue CategoryTag;

    // CREATORS

    /// Create `NullableValueConstRef` object that type erases the supplied
    /// `nullable` object.
    template <class t_NULLABLE>
    explicit NullableValueConstRef(
                               const t_NULLABLE& nullable,
                               typename bsl::enable_if<
                                   IsNullable<t_NULLABLE>::value>::type * = 0);

    /// Create `NullableValueConstRef` object.  The behaviour is undefined
    /// unless `objectAddress` and `vtable` point to the valid objects.
    NullableValueConstRef(const void                     *objectAddress,
                          const NullableValueConstVtable *vtable);

    /// Create `NullableValueConstRef` object that wraps the same object as the
    /// specified `object`.
    NullableValueConstRef(const NullableValueRef& object); // IMPLICIT

    // ACCESSORS

    /// Invoke the specified `accessor` on the referred non-null value.  The
    /// supplied `accessor` must be a callable type that can be called as if it
    /// had the following signature:
    /// ```
    /// int accessor(const VALUE& refOrValue);
    /// ```
    /// Return the value from the invocation of `accessor`.  The behavior is
    /// undefined if the referred object is null.
    template <class t_ACCESSOR>
    int accessValue(t_ACCESSOR& accessor) const;

    /// Return a pointer to the vtable.
    const NullableValueConstVtable *vtable() const;

    /// Return `true` if the referred object contains a null value, and `false`
    /// otherwise.
    bool isNull() const;

    /// Return a pointer to the wrapped object.
    const void *objectAddress() const;

    /// Return the value type category.
    bdlat_TypeCategory::Value valueCategory() const;

    /// Return the `bdlat_TypeName::xsdName` value for the underlying object.
    const char *xsdName(int format) const;
};

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

                           // ----------------------
                           // class NullableValueRef
                           // ----------------------

// CREATORS
template <class t_NULLABLE>
inline
NullableValueRef::NullableValueRef(t_NULLABLE& nullable,
                                   typename bsl::enable_if<
                                       IsNullable<t_NULLABLE>::value>::type *)
: d_object_p(&nullable)
, d_vtable_p(NullableValueVtableUtil::getVtable<t_NULLABLE>())
{
}

inline
NullableValueRef::NullableValueRef(void                      *objectAddress,
                                   const NullableValueVtable *vtable)
: d_object_p(objectAddress)
, d_vtable_p(vtable)
{
}

// MANIPULATORS
inline
void NullableValueRef::makeValue() const
{
    d_vtable_p->d_makeValue_fp(d_object_p);
}

template <class t_MANIPULATOR>
inline
int NullableValueRef::manipulateValue(t_MANIPULATOR& manipulator) const
{
    typename FakeDependency<t_MANIPULATOR, ManipulatorRef>::Type
                                                   manipulatorRef(manipulator);
    return d_vtable_p->d_manipulateValue_fp(d_object_p, manipulatorRef);
}

inline
void NullableValueRef::reset() const
{
    return d_vtable_p->d_reset_fp(d_object_p);
}

// ACCESSORS
template <class t_ACCESSOR>
inline
int NullableValueRef::accessValue(t_ACCESSOR& accessor) const
{
    typename FakeDependency<t_ACCESSOR, AccessorRef>::Type
                                                         accessorRef(accessor);
    return d_vtable_p->d_const.d_accessValue_fp(d_object_p, accessorRef);
}

inline
const NullableValueVtable *NullableValueRef::vtable() const
{
    return d_vtable_p;
}

inline
bool NullableValueRef::isNull() const
{
    return d_vtable_p->d_const.d_isNull_fp(d_object_p);
}

inline
void *NullableValueRef::objectAddress() const
{
    return d_object_p;
}

inline
bdlat_TypeCategory::Value NullableValueRef::valueCategory() const
{
    return d_vtable_p->d_const.d_valueCategory_fp();
}

// FREE FUNCTIONS
// Customization-point functions (`bdlat_NullableValueFunctions`)

inline
void bdlat_nullableValueMakeValue(NullableValueRef *ref)
{
    ref->makeValue();
}

template <class t_MANIPULATOR>
inline
int bdlat_nullableValueManipulateValue(NullableValueRef *ref,
                                       t_MANIPULATOR&    manipulator)
{
    return ref->manipulateValue(manipulator);
}

template <class t_ACCESSOR>
inline
int bdlat_nullableValueAccessValue(const NullableValueRef& ref,
                                   t_ACCESSOR&             accessor)
{
    return ref.accessValue(accessor);
}

inline
bool bdlat_nullableValueIsNull(const NullableValueRef& ref)
{
    return ref.isNull();
}

inline
void bdlat_valueTypeReset(NullableValueRef *ref)
{
    ref->reset();
}

                           // ---------------------------
                           // class NullableValueConstRef
                           // ---------------------------

// CREATORS
template <class t_NULLABLE>
inline
NullableValueConstRef::NullableValueConstRef(
                                    const t_NULLABLE& nullable,
                                    typename bsl::enable_if<
                                        IsNullable<t_NULLABLE>::value>::type *)
: d_object_p(&nullable)
, d_vtable_p(NullableValueVtableUtil::getConstVtable<t_NULLABLE>())
{
}

inline
NullableValueConstRef::NullableValueConstRef(
                                 const void                     *objectAddress,
                                 const NullableValueConstVtable *vtable)
: d_object_p(objectAddress)
, d_vtable_p(vtable)
{
}

inline
NullableValueConstRef::NullableValueConstRef(const NullableValueRef& object)
: d_object_p(object.objectAddress())
, d_vtable_p(&object.vtable()->d_const)
{
}

// ACCESSORS
template <class t_ACCESSOR>
inline
int NullableValueConstRef::accessValue(t_ACCESSOR& accessor) const
{
    typename FakeDependency<t_ACCESSOR, AccessorRef>::Type
                                                         accessorRef(accessor);
    return d_vtable_p->d_accessValue_fp(d_object_p, accessorRef);
}

inline
const NullableValueConstVtable *NullableValueConstRef::vtable() const
{
    return d_vtable_p;
}

inline
bool NullableValueConstRef::isNull() const
{
    return d_vtable_p->d_isNull_fp(d_object_p);
}

inline
const void *NullableValueConstRef::objectAddress() const
{
    return d_object_p;
}

inline
bdlat_TypeCategory::Value NullableValueConstRef::valueCategory() const
{
    return d_vtable_p->d_valueCategory_fp();
}

inline
const char *NullableValueConstRef::xsdName(int format) const
{
    return d_vtable_p->d_xsdName_fp(d_object_p, format);
}

// FREE FUNCTIONS
// Customization-point functions (`bdlat_NullableValueFunctions`)

template <class t_ACCESSOR>
inline
int bdlat_nullableValueAccessValue(const NullableValueConstRef& ref,
                                   t_ACCESSOR&                  accessor)
{
    return ref.accessValue(accessor);
}

inline
bool bdlat_nullableValueIsNull(const NullableValueConstRef& ref)
{
    return ref.isNull();
}

inline
const char *bdlat_TypeName_xsdName(const NullableValueConstRef& ref,
                                   int                          format)
{
    return ref.xsdName(format);
}

}  // close package namespace

namespace bdlat_NullableValueFunctions {

template <>
struct IsNullableValue<bdlar::NullableValueRef> : bsl::true_type {
};

template <>
struct IsNullableValue<bdlar::NullableValueConstRef> : bsl::true_type {
};

template <>
struct ValueType<bdlar::NullableValueRef> {
    typedef bdlar::AnyRef Type;
};

template <>
struct ValueType<bdlar::NullableValueConstRef> {
    typedef bdlar::AnyConstRef Type;
};

}  // close namespace bdlat_NullableValueFunctions
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
