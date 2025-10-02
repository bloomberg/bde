// bdlar_nullablevaluevtable.h                                        -*-C++-*-
#ifndef INCLUDED_BDLAR_NULLABLEVALUEVTABLE
#define INCLUDED_BDLAR_NULLABLEVALUEVTABLE

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide ...
//
//@CLASSES:
//  bdlar::NullableValueVtable: ...
//  bdlar::NullableValueConstVtable: ...
//
//@DESCRIPTION: This component provides ...
//

#include <bdlat_typecategory.h>

namespace BloombergLP {
namespace bdlar {

// Forward
class AccessorRef;
class ManipulatorRef;

                           // ===============================
                           // struct NullableValueConstVtable
                           // ===============================

/// This struct stores a series of function pointers to wrappers that invoke
/// the underlying methods with read-only access of the supplied type erased
/// object that implement the "nullable value" type category.
struct NullableValueConstVtable {
    // TYPES
    typedef bdlat_TypeCategory::Value ValueCategory();
    typedef int AccessValue(const void *object, AccessorRef& accessor);
    typedef bool IsNull(const void *object);
    typedef const char *XsdName(const void *object, int format);

    // PUBLIC DATA
    ValueCategory *d_valueCategory_fp;
    AccessValue   *d_accessValue_fp;
    IsNull        *d_isNull_fp;
    XsdName       *d_xsdName_fp;
};

                           // ==========================
                           // struct NullableValueVtable
                           // ==========================

/// This struct stores a series of function pointers to wrappers that invoke
/// the underlying methods of the supplied type erased object that implement
/// the "nullable value" type category.
struct NullableValueVtable {
    // TYPES
    typedef int ManipulateValue(void *object, ManipulatorRef& manipulator);
    typedef void MakeValue(void *object);
    typedef void Reset(void *object);

    // PUBLIC DATA
    NullableValueConstVtable  d_const;
    ManipulateValue          *d_manipulateValue_fp;
    MakeValue                *d_makeValue_fp;
    Reset                    *d_reset_fp;
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
