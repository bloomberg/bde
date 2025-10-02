// bdlar_anyrefdata.h                                                 -*-C++-*-
#ifndef INCLUDED_BDLAR_ANYREFDATA
#define INCLUDED_BDLAR_ANYREFDATA

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide ...
//
//@CLASSES:
//  bdlar::AnyRefData: ...
//  bdlar::AnyConstRefData: ...
//
//@DESCRIPTION: This component provides ...
//

#include <bdlar_anyvtable.h>

#include <bdlat_typecategory.h>

namespace BloombergLP {
namespace bdlar {

                           // =================
                           // struct AnyRefData
                           // =================

struct AnyRefData {
    // DATA
    void                      *d_object_p;
    AnyVtable                  d_vtable;
    bdlat_TypeCategory::Value  d_category;
};

                           // ======================
                           // struct AnyConstRefData
                           // ======================

struct AnyConstRefData {
    // DATA
    const void                *d_object_p;
    AnyConstVtable             d_vtable;
    bdlat_TypeCategory::Value  d_category;
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
