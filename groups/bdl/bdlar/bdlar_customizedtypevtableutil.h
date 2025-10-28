// bdlar_customizedtypevtableutil.h                                   -*-C++-*-
#ifndef INCLUDED_BDLAR_CUSTOMIZEDTYPEVTABLEUTIL
#define INCLUDED_BDLAR_CUSTOMIZEDTYPEVTABLEUTIL

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide ...
//
//@CLASSES:
//  bdlar::CustomizedTypeVtableUtil: ...
//
//@DESCRIPTION: This component provides ...
//

#include <bdlar_customizedtypevtable.h>
#include <bdlar_refdatautil.h>

#include <bdlat_customizedtypefunctions.h>
#include <bdlat_typename.h>
#include <bdlat_valuetypefunctions.h>

namespace BloombergLP {
namespace bdlar {

                           // ==============================
                           // class CustomizedTypeVtableUtil
                           // ==============================

class CustomizedTypeVtableUtil {
    // PRIVATE CLASS METHODS

    /// Return a type category of the base type.
    template <class t_CUSTOMIZED>
    static bdlat_TypeCategory::Value baseCategory();

    /// Return a reference to the base value of the specified `object`.
    template <class t_CUSTOMIZED>
    static AnyConstRefData convertToBaseType(const void *object);

    /// Convert from the specified `baseValue` of the specified `baseType` to
    /// the specified customized `object`.  Return 0 if successful and non-zero
    /// otherwise.
    template <class t_CUSTOMIZED>
    static int convertFromBaseType(void                  *object,
                                   const void            *baseValue,
                                   const std::type_info&  baseType);

    /// Create an uninitialized value of the base type, apply the specified
    /// `baseManipulator` to it, then call the
    /// `bdlat_CustomizedTypeFunctions::convertFromBaseType` function with the
    /// specified `object` and the value.
    template <class t_CUSTOMIZED>
    static int createBaseAndConvert(void            *object,
                                    ManipulatorRef&  baseManipulator);

    /// Reset the object pointed by the specified `object` to the default
    /// value.
    template <class t_CUSTOMIZED>
    static void reset(void *object);

    /// Return the `bdlat_TypeName::xsdName` value for `object`.
    template <class t_CUSTOMIZED>
    static const char *xsdName(const void *object, int format);

  public:
    // TYPES
    typedef CustomizedTypeVtable      VtableType;
    typedef CustomizedTypeConstVtable ConstVtableType;

    // CLASS METHODS

    /// Construct and return a `CustomizedTypeConstVtable` object with a series
    /// of function pointers that implement the "customized type" type
    /// category.
    template <class t_CUSTOMIZED>
    static const CustomizedTypeConstVtable *getConstVtable();

    /// Construct and return a `CustomizedTypeVtable` object with a series of
    /// function pointers that implement the "customized type" type category.
    template <class t_CUSTOMIZED>
    static const CustomizedTypeVtable *getVtable();
};

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

                           //-------------------------------
                           // class CustomizedTypeVtableUtil
                           //-------------------------------

// PRIVATE CLASS METHODS
template <class t_CUSTOMIZED>
bdlat_TypeCategory::Value CustomizedTypeVtableUtil::baseCategory()
{
    typedef typename bdlat_CustomizedTypeFunctions::
                                         BaseType<t_CUSTOMIZED>::Type BaseType;
    return SelectCategory<BaseType>::e_SELECTION;
}

template <class t_CUSTOMIZED>
AnyConstRefData CustomizedTypeVtableUtil::convertToBaseType(const void *object)
{
    typedef typename bdlat_CustomizedTypeFunctions::
                                         BaseType<t_CUSTOMIZED>::Type BaseType;
    const BaseType &baseValue =
        bdlat_CustomizedTypeFunctions::convertToBaseType(
                                   *static_cast<const t_CUSTOMIZED *>(object));
    return RefDataUtil::makeAnyConstRefData(&baseValue);
}

template <class t_CUSTOMIZED>
int CustomizedTypeVtableUtil::convertFromBaseType(
                                              void                  *object,
                                              const void            *baseValue,
                                              const std::type_info&  baseType)
{
    typedef typename bdlat_CustomizedTypeFunctions::
                                         BaseType<t_CUSTOMIZED>::Type BaseType;
    if (baseType != typeid(BaseType)) {
        return 1;                                                     // RETURN
    }
    return bdlat_CustomizedTypeFunctions::convertFromBaseType(
                                    static_cast<t_CUSTOMIZED *>(object),
                                    *static_cast<const BaseType *>(baseValue));
}

template <class t_CUSTOMIZED>
int CustomizedTypeVtableUtil::createBaseAndConvert(
                                              void            *object,
                                              ManipulatorRef&  baseManipulator)
{
    return bdlat_CustomizedTypeFunctions::createBaseAndConvert(
                                           static_cast<t_CUSTOMIZED *>(object),
                                           baseManipulator);
}

template <class t_CUSTOMIZED>
void CustomizedTypeVtableUtil::reset(void *object)
{
    bdlat_ValueTypeFunctions::reset(static_cast<t_CUSTOMIZED *>(object));
}

template <class t_CUSTOMIZED>
const char *CustomizedTypeVtableUtil::xsdName(const void *object, int format)
{
    return bdlat_TypeName::xsdName(*static_cast<const t_CUSTOMIZED *>(object),
                                   format);
}

// CLASS METHODS
template <class t_CUSTOMIZED>
const CustomizedTypeConstVtable *CustomizedTypeVtableUtil::getConstVtable()
{
    static const CustomizedTypeConstVtable vtable = {
        &baseCategory<t_CUSTOMIZED>,
        &convertToBaseType<t_CUSTOMIZED>,
        &xsdName<t_CUSTOMIZED>
    };
    return &vtable;
}

template <class t_CUSTOMIZED>
const CustomizedTypeVtable *CustomizedTypeVtableUtil::getVtable()
{
    static const CustomizedTypeVtable vtable = {
        {
            &baseCategory<t_CUSTOMIZED>,
            &convertToBaseType<t_CUSTOMIZED>,
            &xsdName<t_CUSTOMIZED>
        },
        &convertFromBaseType<t_CUSTOMIZED>,
        &createBaseAndConvert<t_CUSTOMIZED>,
        &reset<t_CUSTOMIZED>
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
