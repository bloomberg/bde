// bdlar_manipulatorref.h                                             -*-C++-*-
#ifndef INCLUDED_BDLAR_MANIPULATORREF
#define INCLUDED_BDLAR_MANIPULATORREF

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide ...
//
//@CLASSES:
//  bdlar::ManipulatorRef: ...
//  bdlar::ManipulatorWithInfoRef: ...
//
//@DESCRIPTION: This component provides ...
//

#include <bdlar_fieldinfo.h>
#include <bdlar_manipulatorvtable.h>
#include <bdlar_manipulatorvtableutil.h>
#include <bdlar_typecategory.h>

namespace BloombergLP {
namespace bdlar {

                           // ====================
                           // class ManipulatorRef
                           // ====================

class ManipulatorRef {
    // DATA
    const void              *d_manipulator_p;
    const ManipulatorVtable *d_vtable_p;

    // PRIVATE ACCESSORS
    template <class t_TYPE>
    int manipulate(t_TYPE *value, bdlat_TypeCategory::DynamicType) const;

    template <class t_TYPE>
    int manipulate(t_TYPE *value, bdlat_TypeCategory::Array) const;

    template <class t_TYPE>
    int manipulate(t_TYPE *value, bdlat_TypeCategory::Choice) const;

    template <class t_TYPE>
    int manipulate(t_TYPE *value, bdlat_TypeCategory::CustomizedType) const;

    template <class t_TYPE>
    int manipulate(t_TYPE *value, bdlat_TypeCategory::Enumeration) const;

    template <class t_TYPE>
    int manipulate(t_TYPE *value, bdlat_TypeCategory::NullableValue) const;

    template <class t_TYPE>
    int manipulate(t_TYPE *value, bdlat_TypeCategory::Sequence) const;

    template <class t_TYPE>
    int manipulate(t_TYPE *value, bdlat_TypeCategory::Simple) const;

  public:
    // CREATORS
    template <class t_MANIPULATOR>
    explicit ManipulatorRef(t_MANIPULATOR& manipulator);

    // ACCESSORS
    template <class t_TYPE>
    int operator()(t_TYPE *value) const;
};

                           // ============================
                           // class ManipulatorWithInfoRef
                           // ============================

class ManipulatorWithInfoRef {
    // DATA
    const void                      *d_manipulator_p;
    const ManipulatorWithInfoVtable *d_vtable_p;

    // PRIVATE ACCESSORS
    template <class t_TYPE>
    int manipulate(t_TYPE              *value,
                   const FieldInfoRef&  info,
                   bdlat_TypeCategory::DynamicType) const;

    template <class t_TYPE>
    int manipulate(t_TYPE              *value,
                   const FieldInfoRef&  info,
                   bdlat_TypeCategory::Array) const;

    template <class t_TYPE>
    int manipulate(t_TYPE              *value,
                   const FieldInfoRef&  info,
                   bdlat_TypeCategory::Choice) const;

    template <class t_TYPE>
    int manipulate(t_TYPE              *value,
                   const FieldInfoRef&  info,
                   bdlat_TypeCategory::CustomizedType) const;

    template <class t_TYPE>
    int manipulate(t_TYPE              *value,
                   const FieldInfoRef&  info,
                   bdlat_TypeCategory::Enumeration) const;

    template <class t_TYPE>
    int manipulate(t_TYPE              *value,
                   const FieldInfoRef&  info,
                   bdlat_TypeCategory::NullableValue) const;

    template <class t_TYPE>
    int manipulate(t_TYPE              *value,
                   const FieldInfoRef&  info,
                   bdlat_TypeCategory::Sequence) const;

    template <class t_TYPE>
    int manipulate(t_TYPE              *value,
                   const FieldInfoRef&  info,
                   bdlat_TypeCategory::Simple) const;

  public:
    // CREATORS
    template <class t_MANIPULATOR>
    explicit ManipulatorWithInfoRef(t_MANIPULATOR& manipulator);

    // ACCESSORS
    template <class t_TYPE, class t_INFO>
    int operator()(t_TYPE *value, const t_INFO& info) const;
};

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

                           // --------------------
                           // class ManipulatorRef
                           // --------------------

// CREATORS
template <class t_MANIPULATOR>
inline
ManipulatorRef::ManipulatorRef(t_MANIPULATOR& manipulator)
: d_manipulator_p(reinterpret_cast<const void *>(&manipulator))
, d_vtable_p(ManipulatorVtableUtil::getVtable<t_MANIPULATOR>())
{
}

// PRIVATE ACCESSORS
template <class t_TYPE>
inline
int ManipulatorRef::manipulate(t_TYPE *value, bdlat_TypeCategory::DynamicType)
                                                                          const
{
    typename FakeDependency<t_TYPE, DynamicTypeRef>::Type ref(*value);
    return d_vtable_p->d_manipulateDynamic_fp(d_manipulator_p, &ref);
}

template <class t_TYPE>
inline
int ManipulatorRef::manipulate(t_TYPE *value, bdlat_TypeCategory::Array) const
{
    typename FakeDependency<t_TYPE, ArrayRef>::Type ref(*value);
    return d_vtable_p->d_manipulateArray_fp(d_manipulator_p, &ref);
}

template <class t_TYPE>
inline
int ManipulatorRef::manipulate(t_TYPE *value, bdlat_TypeCategory::Choice) const
{
    typename FakeDependency<t_TYPE, ChoiceRef>::Type ref(*value);
    return d_vtable_p->d_manipulateChoice_fp(d_manipulator_p, &ref);
}

template <class t_TYPE>
inline
int ManipulatorRef::manipulate(t_TYPE *value,
                               bdlat_TypeCategory::CustomizedType) const
{
    typename FakeDependency<t_TYPE, CustomizedTypeRef>::Type ref(*value);
    return d_vtable_p->d_manipulateCustomized_fp(d_manipulator_p, &ref);
}

template <class t_TYPE>
inline
int ManipulatorRef::manipulate(t_TYPE *value, bdlat_TypeCategory::Enumeration)
                                                                          const
{
    typename FakeDependency<t_TYPE, EnumRef>::Type ref(*value);
    return d_vtable_p->d_manipulateEnum_fp(d_manipulator_p, &ref);
}

template <class t_TYPE>
inline
int ManipulatorRef::manipulate(t_TYPE *value,
                               bdlat_TypeCategory::NullableValue) const
{
    typename FakeDependency<t_TYPE, NullableValueRef>::Type ref(*value);
    return d_vtable_p->d_manipulateNullable_fp(d_manipulator_p, &ref);
}

template <class t_TYPE>
inline
int ManipulatorRef::manipulate(t_TYPE *value, bdlat_TypeCategory::Sequence)
                                                                          const
{
    typename FakeDependency<t_TYPE, SequenceRef>::Type ref(*value);
    return d_vtable_p->d_manipulateSequence_fp(d_manipulator_p, &ref);
}

template <class t_TYPE>
inline
int ManipulatorRef::manipulate(t_TYPE *value, bdlat_TypeCategory::Simple) const
{
    typename FakeDependency<t_TYPE, SimpleTypeRef>::Type ref(*value);
    return d_vtable_p->d_manipulateSimple_fp(d_manipulator_p, &ref);
}

// ACCESSORS
template <class t_TYPE>
inline
int ManipulatorRef::operator()(t_TYPE *value) const
{
    typedef typename SelectCategory<t_TYPE>::Type TypeTag;
    return manipulate(value, TypeTag());
}

                           // ----------------------------
                           // class ManipulatorWithInfoRef
                           // ----------------------------

// CREATORS
template <class t_MANIPULATOR>
inline
ManipulatorWithInfoRef::ManipulatorWithInfoRef(t_MANIPULATOR& manipulator)
: d_manipulator_p(reinterpret_cast<const void *>(&manipulator))
, d_vtable_p(ManipulatorVtableUtil::getWithInfoVtable<t_MANIPULATOR>())
{
}

// PRIVATE ACCESSORS
template <class t_TYPE>
inline
int ManipulatorWithInfoRef::manipulate(t_TYPE              *value,
                                       const FieldInfoRef&  info,
                                       bdlat_TypeCategory::DynamicType) const
{
    typename FakeDependency<t_TYPE, DynamicTypeRef>::Type ref(*value);
    return d_vtable_p->d_manipulateDynamic_fp(d_manipulator_p, &ref, info);
}

template <class t_TYPE>
inline
int ManipulatorWithInfoRef::manipulate(t_TYPE              *value,
                                       const FieldInfoRef&  info,
                                       bdlat_TypeCategory::Array) const
{
    typename FakeDependency<t_TYPE, ArrayRef>::Type ref(*value);
    return d_vtable_p->d_manipulateArray_fp(d_manipulator_p, &ref, info);
}

template <class t_TYPE>
inline
int ManipulatorWithInfoRef::manipulate(t_TYPE              *value,
                                       const FieldInfoRef&  info,
                                       bdlat_TypeCategory::Choice) const
{
    typename FakeDependency<t_TYPE, ChoiceRef>::Type ref(*value);
    return d_vtable_p->d_manipulateChoice_fp(d_manipulator_p, &ref, info);
}

template <class t_TYPE>
inline
int ManipulatorWithInfoRef::manipulate(t_TYPE              *value,
                                       const FieldInfoRef&  info,
                                       bdlat_TypeCategory::CustomizedType)const
{
    typename FakeDependency<t_TYPE, CustomizedTypeRef>::Type ref(*value);
    return d_vtable_p->d_manipulateCustomized_fp(d_manipulator_p, &ref, info);
}

template <class t_TYPE>
inline
int ManipulatorWithInfoRef::manipulate(t_TYPE              *value,
                                       const FieldInfoRef&  info,
                                       bdlat_TypeCategory::Enumeration) const
{
    typename FakeDependency<t_TYPE, EnumRef>::Type ref(*value);
    return d_vtable_p->d_manipulateEnum_fp(d_manipulator_p, &ref, info);
}

template <class t_TYPE>
inline
int ManipulatorWithInfoRef::manipulate(t_TYPE              *value,
                                       const FieldInfoRef&  info,
                                       bdlat_TypeCategory::NullableValue) const
{
    typename FakeDependency<t_TYPE, NullableValueRef>::Type ref(*value);
    return d_vtable_p->d_manipulateNullable_fp(d_manipulator_p, &ref, info);
}

template <class t_TYPE>
inline
int ManipulatorWithInfoRef::manipulate(t_TYPE              *value,
                                       const FieldInfoRef&  info,
                                       bdlat_TypeCategory::Sequence) const
{
    typename FakeDependency<t_TYPE, SequenceRef>::Type ref(*value);
    return d_vtable_p->d_manipulateSequence_fp(d_manipulator_p, &ref, info);
}

template <class t_TYPE>
inline
int ManipulatorWithInfoRef::manipulate(t_TYPE              *value,
                                       const FieldInfoRef&  info,
                                       bdlat_TypeCategory::Simple) const
{
    typename FakeDependency<t_TYPE, SimpleTypeRef>::Type ref(*value);
    return d_vtable_p->d_manipulateSimple_fp(d_manipulator_p, &ref, info);
}

// ACCESSORS
template <class t_TYPE, class t_INFO>
inline
int ManipulatorWithInfoRef::operator()(t_TYPE *value, const t_INFO& info) const
{
    FieldInfoRef infoRef(info);
    typedef typename SelectCategory<t_TYPE>::Type TypeTag;
    return manipulate(value, infoRef, TypeTag());
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
