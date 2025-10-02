// bdlar_accessorref.h                                                -*-C++-*-
#ifndef INCLUDED_BDLAR_ACCESSORREF
#define INCLUDED_BDLAR_ACCESSORREF

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide ...
//
//@CLASSES:
//  bdlar::AccessorRef: ...
//  bdlar::AccessorWithInfoRef: ...
//
//@DESCRIPTION: This component provides ...
//

#include <bdlar_accessorvtable.h>
#include <bdlar_accessorvtableutil.h>
#include <bdlar_fieldinfo.h>
#include <bdlar_typecategory.h>

namespace BloombergLP {
namespace bdlar {

                           // =================
                           // class AccessorRef
                           // =================

class AccessorRef {
    // DATA
    const void           *d_accessor_p;
    const AccessorVtable *d_vtable_p;

    // PRIVATE ACCESSORS
    template <class t_TYPE>
    int access(const t_TYPE& value, bdlat_TypeCategory::DynamicType) const;

    template <class t_TYPE>
    int access(const t_TYPE& value, bdlat_TypeCategory::Array) const;

    template <class t_TYPE>
    int access(const t_TYPE& value, bdlat_TypeCategory::Choice) const;

    template <class t_TYPE>
    int access(const t_TYPE& value, bdlat_TypeCategory::CustomizedType) const;

    template <class t_TYPE>
    int access(const t_TYPE& value, bdlat_TypeCategory::Enumeration) const;

    template <class t_TYPE>
    int access(const t_TYPE& value, bdlat_TypeCategory::NullableValue) const;

    template <class t_TYPE>
    int access(const t_TYPE& value, bdlat_TypeCategory::Sequence) const;

    template <class t_TYPE>
    int access(const t_TYPE& value, bdlat_TypeCategory::Simple) const;

  public:
    // CREATORS
    template <class t_ACCESSOR>
    explicit AccessorRef(t_ACCESSOR& accessor);

    // ACCESSORS
    template <class t_TYPE>
    int operator()(const t_TYPE& value) const;
};

                           // =========================
                           // class AccessorWithInfoRef
                           // =========================

class AccessorWithInfoRef {
    // DATA
    const void                   *d_accessor_p;
    const AccessorWithInfoVtable *d_vtable_p;

    // PRIVATE ACCESSORS
    template <class t_TYPE>
    int access(const t_TYPE&       value,
               const FieldInfoRef& info,
               bdlat_TypeCategory::DynamicType) const;

    template <class t_TYPE>
    int access(const t_TYPE&       value,
               const FieldInfoRef& info,
               bdlat_TypeCategory::Array) const;

    template <class t_TYPE>
    int access(const t_TYPE&       value,
               const FieldInfoRef& info,
               bdlat_TypeCategory::Choice) const;

    template <class t_TYPE>
    int access(const t_TYPE&       value,
               const FieldInfoRef& info,
               bdlat_TypeCategory::CustomizedType) const;

    template <class t_TYPE>
    int access(const t_TYPE&       value,
               const FieldInfoRef& info,
               bdlat_TypeCategory::Enumeration) const;

    template <class t_TYPE>
    int access(const t_TYPE&       value,
               const FieldInfoRef& info,
               bdlat_TypeCategory::NullableValue) const;

    template <class t_TYPE>
    int access(const t_TYPE&       value,
               const FieldInfoRef& info,
               bdlat_TypeCategory::Sequence) const;

    template <class t_TYPE>
    int access(const t_TYPE&       value,
               const FieldInfoRef& info,
               bdlat_TypeCategory::Simple) const;

  public:
    // CREATORS
    template <class t_ACCESSOR>
    explicit AccessorWithInfoRef(t_ACCESSOR& accessor);

    // ACCESSORS
    template <class t_TYPE, class t_INFO>
    int operator()(const t_TYPE& value, const t_INFO& info) const;
};

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

                           // -----------------
                           // class AccessorRef
                           // -----------------

// CREATORS
template <class t_ACCESSOR>
inline
AccessorRef::AccessorRef(t_ACCESSOR& accessor)
: d_accessor_p(reinterpret_cast<const void *>(&accessor))
, d_vtable_p(AccessorVtableUtil::getVtable<t_ACCESSOR>())
{
}

// PRIVATE ACCESSORS
template <class t_TYPE>
inline
int AccessorRef::access(const t_TYPE& value,
                        bdlat_TypeCategory::DynamicType) const
{
    typename FakeDependency<t_TYPE, DynamicTypeConstRef>::Type ref(value);
    return d_vtable_p->d_accessDynamic_fp(d_accessor_p, ref);
}

template <class t_TYPE>
inline
int AccessorRef::access(const t_TYPE& value, bdlat_TypeCategory::Array) const
{
    typename FakeDependency<t_TYPE, ArrayConstRef>::Type ref(value);
    return d_vtable_p->d_accessArray_fp(d_accessor_p, ref);
}

template <class t_TYPE>
inline
int AccessorRef::access(const t_TYPE& value, bdlat_TypeCategory::Choice) const
{
    typename FakeDependency<t_TYPE, ChoiceConstRef>::Type ref(value);
    return d_vtable_p->d_accessChoice_fp(d_accessor_p, ref);
}

template <class t_TYPE>
inline
int AccessorRef::access(const t_TYPE& value,
                        bdlat_TypeCategory::CustomizedType) const
{
    typename FakeDependency<t_TYPE, CustomizedTypeConstRef>::Type ref(value);
    return d_vtable_p->d_accessCustomized_fp(d_accessor_p, ref);
}

template <class t_TYPE>
inline
int AccessorRef::access(const t_TYPE& value,
                        bdlat_TypeCategory::Enumeration) const
{
    typename FakeDependency<t_TYPE, EnumConstRef>::Type ref(value);
    return d_vtable_p->d_accessEnum_fp(d_accessor_p, ref);
}

template <class t_TYPE>
inline
int AccessorRef::access(const t_TYPE& value,
                        bdlat_TypeCategory::NullableValue) const
{
    typename FakeDependency<t_TYPE, NullableValueConstRef>::Type ref(value);
    return d_vtable_p->d_accessNullable_fp(d_accessor_p, ref);
}

template <class t_TYPE>
inline
int AccessorRef::access(const t_TYPE& value,
                        bdlat_TypeCategory::Sequence) const
{
    typename FakeDependency<t_TYPE, SequenceConstRef>::Type ref(value);
    return d_vtable_p->d_accessSequence_fp(d_accessor_p, ref);
}

template <class t_TYPE>
inline
int AccessorRef::access(const t_TYPE& value, bdlat_TypeCategory::Simple) const
{
    typename FakeDependency<t_TYPE, SimpleTypeConstRef>::Type ref(value);
    return d_vtable_p->d_accessSimple_fp(d_accessor_p, ref);
}

// ACCESSORS
template <class t_TYPE>
inline
int AccessorRef::operator()(const t_TYPE& value) const
{
    typedef typename SelectCategory<t_TYPE>::Type TypeTag;
    return access(value, TypeTag());
}

                           // -------------------------
                           // class AccessorWithInfoRef
                           // -------------------------

// CREATORS
template <class t_ACCESSOR>
inline
AccessorWithInfoRef::AccessorWithInfoRef(t_ACCESSOR& accessor)
: d_accessor_p(reinterpret_cast<const void *>(&accessor))
, d_vtable_p(AccessorVtableUtil::getWithInfoVtable<t_ACCESSOR>())
{
}

// PRIVATE ACCESSORS
template <class t_TYPE>
inline
int AccessorWithInfoRef::access(const t_TYPE&       value,
                                const FieldInfoRef& info,
                                bdlat_TypeCategory::DynamicType) const
{
    typename FakeDependency<t_TYPE, DynamicTypeConstRef>::Type ref(value);
    return d_vtable_p->d_accessDynamic_fp(d_accessor_p, ref, info);
}

template <class t_TYPE>
inline
int AccessorWithInfoRef::access(const t_TYPE&       value,
                                const FieldInfoRef& info,
                                bdlat_TypeCategory::Array) const
{
    typename FakeDependency<t_TYPE, ArrayConstRef>::Type ref(value);
    return d_vtable_p->d_accessArray_fp(d_accessor_p, ref, info);
}

template <class t_TYPE>
inline
int AccessorWithInfoRef::access(const t_TYPE&       value,
                                const FieldInfoRef& info,
                                bdlat_TypeCategory::Choice) const
{
    typename FakeDependency<t_TYPE, ChoiceConstRef>::Type ref(value);
    return d_vtable_p->d_accessChoice_fp(d_accessor_p, ref, info);
}

template <class t_TYPE>
inline
int AccessorWithInfoRef::access(const t_TYPE&       value,
                                const FieldInfoRef& info,
                                bdlat_TypeCategory::CustomizedType) const
{
    typename FakeDependency<t_TYPE, CustomizedTypeConstRef>::Type ref(value);
    return d_vtable_p->d_accessCustomized_fp(d_accessor_p, ref, info);
}

template <class t_TYPE>
inline
int AccessorWithInfoRef::access(const t_TYPE&       value,
                                const FieldInfoRef& info,
                                bdlat_TypeCategory::Enumeration) const
{
    typename FakeDependency<t_TYPE, EnumConstRef>::Type ref(value);
    return d_vtable_p->d_accessEnum_fp(d_accessor_p, ref, info);
}

template <class t_TYPE>
inline
int AccessorWithInfoRef::access(const t_TYPE&       value,
                                const FieldInfoRef& info,
                                bdlat_TypeCategory::NullableValue) const
{
    typename FakeDependency<t_TYPE, NullableValueConstRef>::Type ref(value);
    return d_vtable_p->d_accessNullable_fp(d_accessor_p, ref, info);
}

template <class t_TYPE>
inline
int AccessorWithInfoRef::access(const t_TYPE&       value,
                                const FieldInfoRef& info,
                                bdlat_TypeCategory::Sequence) const
{
    typename FakeDependency<t_TYPE, SequenceConstRef>::Type ref(value);
    return d_vtable_p->d_accessSequence_fp(d_accessor_p, ref, info);
}

template <class t_TYPE>
inline
int AccessorWithInfoRef::access(const t_TYPE&       value,
                                const FieldInfoRef& info,
                                bdlat_TypeCategory::Simple) const
{
    typename FakeDependency<t_TYPE, SimpleTypeConstRef>::Type ref(value);
    return d_vtable_p->d_accessSimple_fp(d_accessor_p, ref, info);
}

// ACCESSORS
template <class t_TYPE, class t_INFO>
inline
int AccessorWithInfoRef::operator()(const t_TYPE& value,
                                    const t_INFO& info) const
{
    FieldInfoRef infoRef(info);
    typedef typename SelectCategory<t_TYPE>::Type TypeTag;
    return access(value, infoRef, TypeTag());
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
