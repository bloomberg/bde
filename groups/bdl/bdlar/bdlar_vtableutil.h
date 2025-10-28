// bdlar_vtableutil.h                                                 -*-C++-*-
#ifndef INCLUDED_BDLAR_VTABLEUTIL
#define INCLUDED_BDLAR_VTABLEUTIL

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide ...
//
//@CLASSES:
//  bdlar::VtableUtil: ...
//
//@DESCRIPTION: This component provides ...
//

#include <bdlar_typecategory.h>

namespace BloombergLP {
namespace bdlar {

// Forward
class SimpleTypeVtableUtil;
class EnumVtableUtil;
class NullableValueVtableUtil;
class ArrayVtableUtil;
class SequenceVtableUtil;
class ChoiceVtableUtil;
class CustomizedTypeVtableUtil;
class DynamicTypeVtableUtil;

                           // ============================
                           // struct VtableUtil_ByCategory
                           // ============================

template <class t_CATEGORY>
struct VtableUtil_ByCategory; // must be specialized

// Category -> VtableUtil mappings:

template <>
struct VtableUtil_ByCategory<bdlat_TypeCategory::Enumeration> {
    typedef EnumVtableUtil Type;
};

template <>
struct VtableUtil_ByCategory<bdlat_TypeCategory::NullableValue> {
    typedef NullableValueVtableUtil Type;
};

template <>
struct VtableUtil_ByCategory<bdlat_TypeCategory::Simple> {
    typedef SimpleTypeVtableUtil Type;
};

template <>
struct VtableUtil_ByCategory<bdlat_TypeCategory::Array> {
    typedef ArrayVtableUtil Type;
};

template <>
struct VtableUtil_ByCategory<bdlat_TypeCategory::Sequence> {
    typedef SequenceVtableUtil Type;
};

template <>
struct VtableUtil_ByCategory<bdlat_TypeCategory::Choice> {
    typedef ChoiceVtableUtil Type;
};

template <>
struct VtableUtil_ByCategory<bdlat_TypeCategory::CustomizedType> {
    typedef CustomizedTypeVtableUtil Type;
};

template <>
struct VtableUtil_ByCategory<bdlat_TypeCategory::DynamicType> {
    typedef DynamicTypeVtableUtil Type;
};

                           // ================
                           // class VtableUtil
                           // ================

/// Functions returning vtables by type category.
class VtableUtil {

    // PRIVATE TYPES
    template <class t_TYPE>
    struct ForType {
        // TYPES
        typedef typename SelectCategory<t_TYPE>::Type          Category;
        typedef typename VtableUtil_ByCategory<Category>::Type Util;

        typedef typename Util::VtableType           Vtable;
        typedef typename Util::ConstVtableType ConstVtable;
    };

  public:
    // CLASS METHODS
    template <class t_TYPE>
    static const typename ForType<t_TYPE>::ConstVtable *getConstVtable();

    template <class t_TYPE>
    static const typename ForType<t_TYPE>::Vtable *getVtable();
};

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

                           // ----------------
                           // class VtableUtil
                           // ----------------

// CLASS METHODS
template <class t_TYPE>
inline
const typename VtableUtil::ForType<t_TYPE>::ConstVtable *
VtableUtil::getConstVtable()
{
    typedef typename ForType<t_TYPE>::Util Util;
    return Util::template getConstVtable<t_TYPE>();
}

template <class t_TYPE>
inline
const typename VtableUtil::ForType<t_TYPE>::Vtable *
VtableUtil::getVtable()
{
    typedef typename ForType<t_TYPE>::Util Util;
    return Util::template getVtable<t_TYPE>();
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
