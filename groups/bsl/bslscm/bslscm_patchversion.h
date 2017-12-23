// bslscm_patchversion.h                                              -*-C++-*-
#ifndef INCLUDED_BSLSCM_PATCHVERSION
#define INCLUDED_BSLSCM_PATCHVERSION

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif

BSLS_IDENT("$Id: $")

//@PURPOSE: Provide common patch versioning information.
//
//@SEE_ALSO: bslscm_version, bslscm_versiontag
//
//@DESCRIPTION: This component provides patch versioning information for the
// 'bsl' package group (and any other package groups released atomically with
// 'bsl').  The 'BSLSCM_PATCHVERSION_PATCH' macro this component defines is of
// limited standalone utility.
//
// This header should only be included by implementation files, to avoid
// unnecessary recompilations of client code.
//
///Usage
///-----
// At compile time, the 'patch' version of 'bsl' can be examined.
//..
//  #if BSLSCM_PATCHVERSION_PATCH > 0
//      printf("This is a patch release\n");
//  #else
//      printf("This is a \".0\" initial release\n");
//  #endif
//..

#define BSLSCM_PATCHVERSION_PATCH 3
    // Provide the patch version number of the current (latest) BSL release.

#endif

// ----------------------------------------------------------------------------
// Copyright 2015 Bloomberg Finance L.P.
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
