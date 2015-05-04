// bsl_slist.h                                                        -*-C++-*-
#ifndef INCLUDED_BSL_SLIST
#define INCLUDED_BSL_SLIST

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide functionality of the corresponding C++ Standard header.
//
//@INTERNAL_DEPRECATED: Do not use.
//
//@SEE_ALSO: package bsl+stdhdrs
//
//@AUTHOR: Arthur Chiu (achiu21)
//
//@DESCRIPTION: Provide types, in the 'bsl' namespace, equivalent to those
// defined in the corresponding C++ standard header.  Include the native
// compiler-provided standard header, and also directly include Bloomberg's
// implementation of the C++ standard type (if one exists).  Finally, place the
// included symbols from the 'std' namespace (if any) into the 'bsl' namespace.

#ifdef BDE_OPENSOURCE_PUBLICATION // DEPRECATED
#error "bsl_slist is deprecated"
#endif

#ifndef INCLUDED_BSLS_NATIVESTD
#include <bsls_nativestd.h>
#endif

// Include the "standard" implementation of 'slist'.  There is no standard
// 'slist' but in BSL_OVERRIDES_STD mode the standard headers are intercepted
// by Bloomberg-specific wrappers that do the right thing.

#ifdef BSL_OVERRIDES_STD
#include <slist>
#endif

// Include Bloomberg's implementation, unless compilation is configured to
// override native types in the 'std' namespace with Bloomberg's
// implementation, in which case the implementation file will be included by
// the Bloomberg supplied standard header file.

#ifndef BSL_OVERRIDES_STD
#include <bslstp_slist.h>
#endif

#endif

// ----------------------------------------------------------------------------
// Copyright 2013 Bloomberg Finance L.P.
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
