// bdlar_customizedtypevtable.h                                       -*-C++-*-
#ifndef INCLUDED_BDLAR_CUSTOMIZEDTYPEVTABLE
#define INCLUDED_BDLAR_CUSTOMIZEDTYPEVTABLE

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide ...
//
//@CLASSES:
//  bdlar::CustomizedTypeVtable: ...
//  bdlar::CustomizedTypeConstVtable: ...
//
//@DESCRIPTION: This component provides ...
//

#include <bdlar_anyrefdata.h>

#include <bdlat_typecategory.h>
#include <bdlat_valuetypefunctions.h>

#include <typeinfo>

namespace BloombergLP {
namespace bdlar {

// Forward
class ManipulatorRef;

                           // ================================
                           // struct CustomizedTypeConstVtable
                           // ================================

/// This struct stores a series of function pointers to wrappers that invoke
/// the underlying methods with read-only access of the supplied type erased
/// object that implement the "customized type" type category.
struct CustomizedTypeConstVtable {
    // TYPES
    typedef bdlat_TypeCategory::Value BaseCategory();
    typedef AnyConstRefData ConvertToBaseType(const void *object);
    typedef const char *XsdName(const void *object, int format);

    // PUBLIC DATA
    BaseCategory      *d_baseCategory_fp;
    ConvertToBaseType *d_convertToBaseType_fp;
    XsdName           *d_xsdName_fp;
};

                           // ===========================
                           // struct CustomizedTypeVtable
                           // ===========================

/// This struct stores a series of function pointers to wrappers that invoke
/// the underlying methods of the supplied type erased object that implement
/// the "customized type" type category.
struct CustomizedTypeVtable {
    // TYPES
    typedef int ConvertFromBaseType(void                  *object,
                                    const void            *baseValue,
                                    const std::type_info&  baseType);
    typedef int CreateBaseAndConvert(void            *object,
                                     ManipulatorRef&  baseManipulator);
    typedef void Reset(void *object);

    // PUBLIC DATA
    CustomizedTypeConstVtable  d_const;
    ConvertFromBaseType       *d_convertFromBaseType_fp;
    CreateBaseAndConvert      *d_createBaseAndConvert_fp;
    Reset                     *d_reset_fp;
};

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
