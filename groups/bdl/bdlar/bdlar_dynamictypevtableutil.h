// bdlar_dynamictypevtableutil.h                                      -*-C++-*-
#ifndef INCLUDED_BDLAR_DYNAMICTYPEVTABLEUTIL
#define INCLUDED_BDLAR_DYNAMICTYPEVTABLEUTIL

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide ...
//
//@CLASSES:
//  bdlar::DynamicTypeVtableUtil: ...
//
//@DESCRIPTION: This component provides ...
//

#include <bdlar_dynamictypevtable.h>
#include <bdlar_refdatautil.h>

#include <bdlat_typecategory.h>
#include <bdlat_typename.h>
#include <bdlat_valuetypefunctions.h>

namespace BloombergLP {
namespace bdlar {

                           // ===========================
                           // class DynamicTypeVtableUtil
                           // ===========================

class DynamicTypeVtableUtil {
    // PRIVATE TYPES

    /// An accessor that just remembers an address of the accessed value.
    struct Accessor {
        // PUBLIC DATA
        AnyConstRefData d_ref;

        // MANIPULATORS
        template <class t_TYPE, class t_TYPE_TAG>
        int operator()(const t_TYPE &value, t_TYPE_TAG)
        {
            d_ref = RefDataUtil::makeAnyConstRefData(&value);
            return 0;
        }
    };

    /// A manipulator that just remembers an address of the manipulated value.
    struct Manipulator {
        // PUBLIC DATA
        AnyRefData d_ref;

        // MANIPULATORS
        template <class t_TYPE, class t_TYPE_TAG>
        int operator()(t_TYPE *value, t_TYPE_TAG)
        {
            d_ref = RefDataUtil::makeAnyRefData(value);
            return 0;
        }
    };

    // PRIVATE CLASS METHODS

    /// Reset the object pointed by the specified `object` to the default
    /// value.
    template <class t_DYNAMIC>
    static void reset(void *object);

    /// Return the value type category of the specified `object`.
    template <class t_DYNAMIC>
    static bdlat_TypeCategory::Value typeCategory(const void *object);

    /// Return a const reference to the underlying value.
    template <class t_DYNAMIC>
    static AnyConstRefData valueRef(const void *object);

    /// Return a reference to the underlying value.
    template <class t_DYNAMIC>
    static AnyRefData valueRef(void *object);

    /// Return the `bdlat_TypeName::xsdName` value for `object`.
    template <class t_DYNAMIC>
    static const char *xsdName(const void *object, int format);

  public:
    // TYPES
    typedef DynamicTypeVtable      VtableType;
    typedef DynamicTypeConstVtable ConstVtableType;

    // CLASS METHODS

    /// Construct and return a `DynamicTypeConstVtable` object with a series of
    /// function pointers that implement the "dynamic" type category.
    template <class t_DYNAMIC>
    static const DynamicTypeConstVtable *getConstVtable();

    /// Construct and return a `DynamicTypeVtable` object with a series of
    /// function pointers that implement the "dynamic" type category.
    template <class t_DYNAMIC>
    static const DynamicTypeVtable *getVtable();
};

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

                           // ---------------------------
                           // class DynamicTypeVtableUtil
                           // ---------------------------

// PRIVATE CLASS METHODS
template <class t_DYNAMIC>
void DynamicTypeVtableUtil::reset(void *object)
{
    bdlat_ValueTypeFunctions::reset(static_cast<t_DYNAMIC *>(object));
}

template <class t_DYNAMIC>
bdlat_TypeCategory::Value DynamicTypeVtableUtil::typeCategory(
                                                            const void *object)
{
    return bdlat_TypeCategoryFunctions::select(
                                      *static_cast<const t_DYNAMIC *>(object));
}

template <class t_DYNAMIC>
AnyConstRefData DynamicTypeVtableUtil::valueRef(const void *object)
{
    Accessor accessor;
    bdlat_TypeCategoryUtil::accessByCategory(
                                       *static_cast<const t_DYNAMIC *>(object),
                                       accessor);
    return accessor.d_ref;
}

template <class t_DYNAMIC>
AnyRefData DynamicTypeVtableUtil::valueRef(void *object)
{
    Manipulator manipulator;
    bdlat_TypeCategoryUtil::manipulateByCategory(
                                              static_cast<t_DYNAMIC *>(object),
                                              manipulator);
    return manipulator.d_ref;
}

template <class t_DYNAMIC>
const char *DynamicTypeVtableUtil::xsdName(const void *object, int format)
{
    return bdlat_TypeName::xsdName(*static_cast<const t_DYNAMIC *>(object),
                                   format);
}

// CLASS METHODS
template <class t_DYNAMIC>
const DynamicTypeConstVtable *DynamicTypeVtableUtil::getConstVtable()
{
    static const DynamicTypeConstVtable vtable = {
        &typeCategory<t_DYNAMIC>,
        &valueRef<t_DYNAMIC>,
        &xsdName<t_DYNAMIC>
    };
    return &vtable;
}

template <class t_DYNAMIC>
const DynamicTypeVtable *DynamicTypeVtableUtil::getVtable()
{
    static const DynamicTypeVtable vtable = {
        {
            &typeCategory<t_DYNAMIC>,
            &valueRef<t_DYNAMIC>,
            &xsdName<t_DYNAMIC>
        },
        &reset<t_DYNAMIC>,
        &valueRef<t_DYNAMIC>
    };
    return &vtable;
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
