// bdlar_enumref.h                                                    -*-C++-*-
#ifndef INCLUDED_BDLAR_ENUMREF
#define INCLUDED_BDLAR_ENUMREF

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide ...
//
//@CLASSES:
//  bdlar::EnumRef: ...
//  bdlar::EnumConstRef: ...
//
//@DESCRIPTION: This component provides ...
//

#include <bdlar_enumvtable.h>
#include <bdlar_enumvtableutil.h>
#include <bdlar_typecategory.h>

#include <bdlat_valuetypefunctions.h>

#include <bsl_type_traits.h>

namespace BloombergLP {
namespace bdlar {

                           // =============
                           // class EnumRef
                           // =============

class EnumRef {
    // DATA
    void             *d_object_p;
    const EnumVtable *d_vtable_p;

  public:
    // CREATORS

    /// Create an `EnumRef` object that type erases the supplied `enumeration`
    /// object.
    template <class t_ENUM>
    explicit EnumRef(t_ENUM& enumeration,
                     typename bsl::enable_if<
                         IsEnum<t_ENUM>::value>::type * = 0);

    /// Create an `EnumRef` object.  The behaviour is undefined unless
    /// `objectAddress` and `vtable` point to the valid objects.
    EnumRef(void *objectAddress, const EnumVtable *vtable);

    // MANIPULATORS

    /// Assign the enumerator value matching the specified `number`.  Return 0
    /// on success, and a non-zero value with no effect on the object if
    /// `number` does not match any enumerator.
    int fromInt(int number) const;

    /// Assign the enumerator value matching the specified `string` of the
    /// specified `stringLength`.  Return 0 on success, and a non-zero value
    /// with no effect on the object if `string` and `stringLength` do not
    /// match any enumerator.
    int fromString(const char *string, int stringLength) const;

    /// Assign the fallback enumerator value.  Return 0 on success, and a
    /// non-zero value with no effect on the object if it does not have a
    /// fallback enumerator.
    int makeFallback() const;

    /// Reset the referred object to the default value.
    void reset() const;

    // ACCESSORS

    /// Return a pointer to the vtable.
    const EnumVtable *vtable() const;

    /// Return `true` if the specified enumerator held by this object supports
    /// a fallback enumerator, and `false` otherwise.
    bool hasFallback() const;

    /// Return `true` if the specified enumerator held by this object is equal
    /// to a fallback enumerator, and `false` otherwise.
    bool isFallback() const;

    /// Return a pointer to the wrapped object.
    void *objectAddress() const;

    /// Load into the specified `result` the integer representation of the
    /// enumerator value held by this object.
    void toInt(int *result) const;

    /// Load into the specified `result` the string representation of the
    /// enumerator value held by this object.
    void toString(bsl::string *result) const;
};

                           // ==================
                           // class EnumConstRef
                           // ==================

class EnumConstRef {
    // DATA
    const void            *d_object_p;
    const EnumConstVtable *d_vtable_p;

  public:
    // CREATORS

    /// Create an `EnumConstRef` object that type erases the supplied
    /// `enumeration` object.
    template <class t_ENUM>
    explicit EnumConstRef(const t_ENUM& enumeration,
                          typename bsl::enable_if<
                              IsEnum<t_ENUM>::value>::type * = 0);

    /// Create an `EnumConstRef` object.  The behaviour is undefined unless
    /// `objectAddress` and `vtable` point to the valid objects.
    EnumConstRef(const void *objectAddress, const EnumConstVtable *vtable);

    /// Create an `EnumConstRef` object that wraps the same object as the
    /// specified `object`.
    EnumConstRef(const EnumRef& object); // IMPLICIT

    // ACCESSORS

    /// Return a pointer to the vtable.
    const EnumConstVtable *vtable() const;

    /// Return `true` if the specified enumerator held by this object supports
    /// a fallback enumerator, and `false` otherwise.
    bool hasFallback() const;

    /// Return `true` if the specified enumerator held by this object is equal
    /// to a fallback enumerator, and `false` otherwise.
    bool isFallback() const;

    /// Return a pointer to the wrapped object.
    const void *objectAddress() const;

    /// Load into the specified `result` the integer representation of the
    /// enumerator value held by this object.
    void toInt(int *result) const;

    /// Load into the specified `result` the string representation of the
    /// enumerator value held by this object.
    void toString(bsl::string *result) const;

    /// Return the `bdlat_TypeName::xsdName` value for the underlying object.
    const char *xsdName(int format) const;
};

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

                           // -------------
                           // class EnumRef
                           // -------------

// CREATORS
template <class t_ENUM>
inline
EnumRef::EnumRef(t_ENUM& enumeration,
                 typename bsl::enable_if<
                     IsEnum<t_ENUM>::value>::type *)
: d_object_p(&enumeration)
, d_vtable_p(EnumVtableUtil::getVtable<t_ENUM>())
{
}

inline
EnumRef::EnumRef(void *objectAddress, const EnumVtable *vtable)
: d_object_p(objectAddress)
, d_vtable_p(vtable)
{
}

// MANIPULATORS
inline
int EnumRef::fromInt(int number) const
{
    return d_vtable_p->d_fromInt_fp(d_object_p, number);
}

inline
int EnumRef::fromString(const char *string, int stringLength) const
{
    return d_vtable_p->d_fromString_fp(d_object_p, string, stringLength);
}

inline
int EnumRef::makeFallback() const
{
    return d_vtable_p->d_makeFallback_fp(d_object_p);
}

inline
void EnumRef::reset() const
{
    return d_vtable_p->d_reset_fp(d_object_p);
}

// ACCESSORS
inline
const EnumVtable *EnumRef::vtable() const
{
    return d_vtable_p;
}

inline
bool EnumRef::hasFallback() const
{
    return d_vtable_p->d_const.d_hasFallback_fp(d_object_p);
}

inline
bool EnumRef::isFallback() const
{
    return d_vtable_p->d_const.d_isFallback_fp(d_object_p);
}

inline
void *EnumRef::objectAddress() const
{
    return d_object_p;
}

inline
void EnumRef::toInt(int *result) const
{
    d_vtable_p->d_const.d_toInt_fp(result, d_object_p);
}

inline
void EnumRef::toString(bsl::string *result) const
{
    d_vtable_p->d_const.d_toString_fp(result, d_object_p);
}

// FREE FUNCTIONS
// Customization-point functions (`bdlat_EnumFunctions`)

inline
int bdlat_enumFromInt(EnumRef *result, int number)
{
    return result->fromInt(number);
}

inline
int bdlat_enumFromString(EnumRef *result, const char *string, int stringLength)
{
    return result->fromString(string, stringLength);
}

inline
int bdlat_enumMakeFallback(EnumRef *result)
{
    return result->makeFallback();
}

inline
bool bdlat_enumHasFallback(const EnumRef& value)
{
    return value.hasFallback();
}

inline
bool bdlat_enumIsFallback(const EnumRef& value)
{
    return value.isFallback();
}

inline
void bdlat_enumToInt(int *result, const EnumRef& value)
{
    value.toInt(result);
}

inline
void bdlat_enumToString(bsl::string *result, const EnumRef& value)
{
    value.toString(result);
}

inline
void bdlat_valueTypeReset(EnumRef *ref)
{
    ref->reset();
}

                           // ------------------
                           // class EnumConstRef
                           // ------------------

// CREATORS
template <class t_ENUM>
inline
EnumConstRef::EnumConstRef(const t_ENUM& enumeration,
                           typename bsl::enable_if<
                               IsEnum<t_ENUM>::value>::type *)
: d_object_p(&enumeration)
, d_vtable_p(EnumVtableUtil::getConstVtable<t_ENUM>())
{
}

inline
EnumConstRef::EnumConstRef(const void            *objectAddress,
                           const EnumConstVtable *vtable)
: d_object_p(objectAddress)
, d_vtable_p(vtable)
{
}

inline
EnumConstRef::EnumConstRef(const EnumRef& object)
: d_object_p(object.objectAddress())
, d_vtable_p(&object.vtable()->d_const)
{
}

// ACCESSORS
inline
const EnumConstVtable *EnumConstRef::vtable() const
{
    return d_vtable_p;
}

inline
bool EnumConstRef::hasFallback() const
{
    return d_vtable_p->d_hasFallback_fp(d_object_p);
}

inline
bool EnumConstRef::isFallback() const
{
    return d_vtable_p->d_isFallback_fp(d_object_p);
}

inline
const void *EnumConstRef::objectAddress() const
{
    return d_object_p;
}

inline
void EnumConstRef::toInt(int *result) const
{
    d_vtable_p->d_toInt_fp(result, d_object_p);
}

inline
void EnumConstRef::toString(bsl::string *result) const
{
    d_vtable_p->d_toString_fp(result, d_object_p);
}

inline
const char *EnumConstRef::xsdName(int format) const
{
    return d_vtable_p->d_xsdName_fp(d_object_p, format);
}

// FREE FUNCTIONS
// Customization-point functions (`bdlat_EnumFunctions`)

inline
bool bdlat_enumHasFallback(const EnumConstRef& value)
{
    return value.hasFallback();
}

inline
bool bdlat_enumIsFallback(const EnumConstRef& value)
{
    return value.isFallback();
}

inline
void bdlat_enumToInt(int *result, const EnumConstRef& value)
{
    value.toInt(result);
}

inline
void bdlat_enumToString(bsl::string *result, const EnumConstRef& value)
{
    value.toString(result);
}

inline
const char *bdlat_TypeName_xsdName(const EnumConstRef& ref, int format)
{
    return ref.xsdName(format);
}

}  // close package namespace

namespace bdlat_EnumFunctions {

template <>
struct IsEnumeration<bdlar::EnumRef> : bsl::true_type {
};

template <>
struct IsEnumeration<bdlar::EnumConstRef> : bsl::true_type {
};

template <>
struct HasFallbackEnumerator<bdlar::EnumRef> : bsl::true_type {
};

template <>
struct HasFallbackEnumerator<bdlar::EnumConstRef> : bsl::true_type {
};

}  // close namespace bdlat_EnumFunctions
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
