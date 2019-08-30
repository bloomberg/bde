// bsls_compilerfeatures_test.h                                       -*-C++-*-
#ifndef INCLUDED_BSLS_COMPILERFEATURES_TEST
#define INCLUDED_BSLS_COMPILERFEATURES_TEST

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide testing for 'bsls_compilerfeatures.h'.
//
//@CLASSES:
//
//@SEE_ALSO: bsls_compilerfeatures
//
//@DESCRIPTION: This component provides testing for the 'bsls_compilerfeatures'
// component that is best kept out of the component itself.  The only thing
// tested here is '#include_next', which must be tested in an include file and
// is better not tested in 'bsls_compilerfeatures.h'.

#include <bsls_compilerfeatures.h>

// The following '#include_next' test was previously done in
// 'bsls_compilerfeatures.t.cpp' but generated warnings in C++17 about
// inappropriate use of '#include_next' in a .cpp file.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_INCLUDE_NEXT)
# include_next<cstdio>  // Preprocessor feature test: this *IS* the check.
#endif  // BSLS_COMPILERFEATURES_SUPPORT_INCLUDE_NEXT

#endif

// ----------------------------------------------------------------------------
// Copyright 2019 Bloomberg Finance L.P.
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
