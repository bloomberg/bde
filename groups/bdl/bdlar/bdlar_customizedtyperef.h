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

#include <bdlar_anyrefdata.h>
#include <bdlar_customizedtypevtable.h>
#include <bdlar_customizedtypevtableutil.h>
#include <bdlar_typecategory.h>

#include <bdlat_valuetypefunctions.h>

#include <bslmf_enableif.h>

#include <bsls_alignedbuffer.h>

namespace BloombergLP {
namespace bdlar {

const size_t k_TMP_ANY_BUFFER_SIZE = sizeof(void*) * 3;

                           // =======================
                           // class CustomizedTypeRef
                           // =======================

class CustomizedTypeRef {
    // DATA
    void                       *d_object_p;
    const CustomizedTypeVtable *d_vtable_p;

    // Required only to return `const AnyConstRef&`
    mutable bsls::AlignedBuffer<k_TMP_ANY_BUFFER_SIZE> d_tmpBuffer;

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

    /// Return a reference to the base value.
    AnyConstRefData convertToBaseType() const;

    /// Return a pointer to the vtable.
    const CustomizedTypeVtable *vtable() const;

    /// Return a pointer to the wrapped object.
    void *objectAddress() const;

    /// Return a pointer to buffer large enough to store one `AnyConstRef`
    /// object.
    void *tmpAnyBuffer() const;
};

                           // ============================
                           // class CustomizedTypeConstRef
                           // ============================

class CustomizedTypeConstRef {
    // DATA
    const void                      *d_object_p;
    const CustomizedTypeConstVtable *d_vtable_p;

    // Required only to return `const AnyConstRef&`
    mutable bsls::AlignedBuffer<k_TMP_ANY_BUFFER_SIZE> d_tmpBuffer;

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

    /// Return a reference to the base value.
    AnyConstRefData convertToBaseType() const;

    /// Return a pointer to the vtable.
    const CustomizedTypeConstVtable *vtable() const;

    /// Return a pointer to the wrapped object.
    const void *objectAddress() const;

    /// Return a pointer to buffer large enough to store one `AnyConstRef`
    /// object.
    void *tmpAnyBuffer() const;
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
{
}

inline
CustomizedTypeRef::CustomizedTypeRef(void                       *objectAddress,
                                     const CustomizedTypeVtable *vtable)
: d_object_p(objectAddress)
, d_vtable_p(vtable)
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
AnyConstRefData CustomizedTypeRef::convertToBaseType() const
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

inline
void *CustomizedTypeRef::tmpAnyBuffer() const
{
    return d_tmpBuffer.buffer();
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
{
}

inline
CustomizedTypeConstRef::CustomizedTypeConstRef(
                                const void                      *objectAddress,
                                const CustomizedTypeConstVtable *vtable)
: d_object_p(objectAddress)
, d_vtable_p(vtable)
{
}

inline
CustomizedTypeConstRef::CustomizedTypeConstRef(const CustomizedTypeRef& object)
: d_object_p(object.objectAddress())
, d_vtable_p(&object.vtable()->d_const)
{
}

// ACCESSORS
inline
bdlat_TypeCategory::Value CustomizedTypeConstRef::baseCategory() const
{
    return d_vtable_p->d_baseCategory_fp();
}

inline
AnyConstRefData CustomizedTypeConstRef::convertToBaseType() const
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

inline
void *CustomizedTypeConstRef::tmpAnyBuffer() const
{
    return d_tmpBuffer.buffer();
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
