// bslstl_referencewrapper.h                                          -*-C++-*-
#ifndef INCLUDED_BSLSTL_REFERENCEWRAPPER
#define INCLUDED_BSLSTL_REFERENCEWRAPPER

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide copyable, assignable object wrapper for references.
//
//@CLASSES:
//  bsl::reference_wrapper: class object to hold a reference to an object
//
//@CANONICAL_HEADER: bsl_functional.h
//
//@DESCRIPTION: This component just includes `<bslmf_referencewrapper.h>` due
// to backwards compatibility.  See `bslmf_referencewrapper` for more
// information.

#include <bslscm_version.h>

#include <bslmf_referencewrapper.h>

#ifdef BSLMF_REFERENCEWRAPPER_IS_ALIASED
    // Typo in the name is intentional for backwards compatibility
    #define BSLSTL_REFRENCEWRAPPER_IS_ALIASED
#endif

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
