// bslim_bslstandardheadertest.h                                      -*-C++-*-
#ifndef INCLUDED_BSLIM_BSLSTANDARDHEADERTEST
#define INCLUDED_BSLIM_BSLSTANDARDHEADERTEST

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a location for testing 'bsl' standard library headers.
//
//@CLASSES:
//
//@DESCRIPTION: This component provides a location to define a set of tests for
// the inclusion of 'bsl' standard library headers, and in particular to test
// types defined in 'bslstl'.  These tests are located in 'bslim' in order to
// allow the standard library types to be included through the standard
// '<bsl_*.h>' headers.  Note that this component does not define any types or
// functions.

#include <bslscm_version.h>

#include <bsls_compilerfeatures.h>

#if defined(BSLS_COMPILERFEATURES_SUPPORT_INCLUDE_NEXT)
  // The following '#include_next' test was previously done in
  // 'bsls_compilerfeatures.t.cpp' but generated unsuppressable warnings in GNU
  // about inappropriate use of '#include_next' in a .cpp file.

# include_next<cstdio>      // Preprocessor feature test: this *IS* the check.
#endif  // BSLS_COMPILERFEATURES_SUPPORT_INCLUDE_NEXT

#endif

// ----------------------------------------------------------------------------
// Copyright 2017 Bloomberg Finance L.P.
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
