// bdlar_simpletypevtable.h                                           -*-C++-*-
#ifndef INCLUDED_BDLAR_SIMPLETYPEVTABLE
#define INCLUDED_BDLAR_SIMPLETYPEVTABLE

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide ...
//
//@CLASSES:
//  bdlar::SimpleTypeVtable: ...
//  bdlar::SimpleTypeConstVtable: ...
//
//@DESCRIPTION: This component provides ...
//

#include <bdlar_simpletypeid.h>

#include <bsl_string.h>

namespace BloombergLP {
namespace bdlar {

                           // ============================
                           // struct SimpleTypeConstVtable
                           // ============================

/// This struct stores a series of function pointers to wrappers that invoke
/// the underlying methods with read-only access of the supplied type erased
/// object that implement the "simple" type category.
struct SimpleTypeConstVtable {
    // TYPES
    typedef SimpleTypeId TypeId();
    typedef const char *XsdName(const void *object, int format);

    // PUBLIC DATA
    TypeId  *d_typeId_fp;
    XsdName *d_xsdName_fp;
};

                           // =======================
                           // struct SimpleTypeVtable
                           // =======================

/// This struct stores a series of function pointers to wrappers that invoke
/// the underlying methods of the supplied type erased object that implement
/// the "simple" type category.
struct SimpleTypeVtable {
    // TYPES
    typedef void Reset(void *object);

    // PUBLIC DATA
    SimpleTypeConstVtable  d_const;
    Reset                 *d_reset_fp;
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
