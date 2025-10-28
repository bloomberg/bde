// bdlar_enumvtable.h                                                 -*-C++-*-
#ifndef INCLUDED_BDLAR_ENUMVTABLE
#define INCLUDED_BDLAR_ENUMVTABLE

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide ...
//
//@CLASSES:
//  bdlar::EnumVtable: ...
//  bdlar::EnumConstVtable: ...
//
//@DESCRIPTION: This component provides ...
//

#include <bsl_string.h>

namespace BloombergLP {
namespace bdlar {

                           // ======================
                           // struct EnumConstVtable
                           // ======================

/// This struct stores a series of function pointers to wrappers that invoke
/// the underlying methods with read-only access of the supplied type erased
/// object that implement the "enum" type category.
struct EnumConstVtable {
    // TYPES
    typedef void ToInt(int *result, const void *value);
    typedef void ToString(bsl::string *result, const void *value);
    typedef bool HasFallback(const void *value);
    typedef bool IsFallback(const void *value);
    typedef const char *XsdName(const void *object, int format);

    // PUBLIC DATA
    ToInt       *d_toInt_fp;
    ToString    *d_toString_fp;
    HasFallback *d_hasFallback_fp;
    IsFallback  *d_isFallback_fp;
    XsdName     *d_xsdName_fp;
};

                           // =================
                           // struct EnumVtable
                           // =================

/// This struct stores a series of function pointers to wrappers that invoke
/// the underlying methods of the supplied type erased object that implement
/// the "enum" type category.
struct EnumVtable {
    // TYPES
    typedef int FromInt(void *result, int number);
    typedef int FromString(void *result, const char *string, int stringLength);
    typedef int MakeFallback(void *result);
    typedef void Reset(void *object);

    // PUBLIC DATA
    EnumConstVtable  d_const;
    FromInt         *d_fromInt_fp;
    FromString      *d_fromString_fp;
    MakeFallback    *d_makeFallback_fp;
    Reset           *d_reset_fp;
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
