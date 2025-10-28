// bdlar_customizedtypetraits.h                                       -*-C++-*-
#ifndef INCLUDED_BDLAR_CUSTOMIZEDTYPETRAITS
#define INCLUDED_BDLAR_CUSTOMIZEDTYPETRAITS

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide ...
//
//@CLASSES:
//
//@DESCRIPTION: This component provides ...
//

#include <bdlar_anyref.h>
#include <bdlar_customizedtyperef.h>

namespace BloombergLP {
namespace bdlar {

BSLMF_ASSERT(k_TMP_ANY_BUFFER_SIZE >= sizeof(AnyConstRef));

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

                           // -----------------------
                           // class CustomizedTypeRef
                           // -----------------------

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
    return *new(ref.tmpAnyBuffer()) AnyConstRef(ref.convertToBaseType());
}

inline
void bdlat_valueTypeReset(CustomizedTypeRef *ref)
{
    ref->reset();
}

                           // ----------------------------
                           // class CustomizedTypeConstRef
                           // ----------------------------

// FREE FUNCTIONS
// Customization-point functions (`bdlat_CustomizedTypeFunctions`)

inline
const AnyConstRef& bdlat_customizedTypeConvertToBaseType(
                                             const CustomizedTypeConstRef& ref)
{
    return *new(ref.tmpAnyBuffer()) AnyConstRef(ref.convertToBaseType());
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
