// bslstl_sstream.h                                                   -*-C++-*-
#ifndef INCLUDED_BSLSTL_SSTREAM
#define INCLUDED_BSLSTL_SSTREAM

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide C++03-compatible 'stringstream' classes.
//
//@SEE_ALSO: bslstl_stringbuf, bslstl_istringstream, bslstl_ostringstream,
//           bslstl_stringstream
//
//@AUTHOR: Alexei Zakharov (azakhar1)
//
//@DESCRIPTION: This component is for internal use only.  Please include
// '<bsl_sstream.h>' instead.
//
///Usage
///-----
// This component is for use by the 'bsl+stdhdrs' package.

// Prevent 'bslstl' headers from being included directly in 'BSL_OVERRIDES_STD'
// mode.  Doing so is unsupported, and is likely to cause compilation errors.
#if defined(BSL_OVERRIDES_STD) && !defined(BSL_STDHDRS_PROLOGUE_IN_EFFECT)
#error "include <bsl_sstream.h> instead of <bslstl_sstream.h> in \
BSL_OVERRIDES_STD mode"
#endif

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLSTL_ISTRINGSTREAM
#include <bslstl_istringstream.h>
#endif

#ifndef INCLUDED_BSLSTL_OSTRINGSTREAM
#include <bslstl_ostringstream.h>
#endif

#ifndef INCLUDED_BSLSTL_STRINGBUF
#include <bslstl_stringbuf.h>
#endif

#ifndef INCLUDED_BSLSTL_STRINGSTREAM
#include <bslstl_stringstream.h>
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
