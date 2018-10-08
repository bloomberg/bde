// bsls_compilerfeatures.cpp                                          -*-C++-*-
#include <bsls_compilerfeatures.h>

#include <bsls_ident.h>
BSLS_IDENT("$Id$ $CSID$")

#include <bsls_bsltestutil.h>   // for testing only
#include <bsls_buildtarget.h>   // for testing only

#if defined(BSLS_COMPILERFEATURES_INTIIALIZER_LIST_LEAKS_ON_EXCEPTIONS)

    #ifndef BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS
    #error "'BSLS_COMPILERFEATURES_INITIALIZER_LIST_LEAKS_ON_EXCEPTIONS' \
             requires 'BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS'"
    #endif

#endif

#if defined(BSLS_COMPILERFEATURES_SUPPORT_CONSTEXPR_RELAXED) &&               \
    !defined(BSLS_COMPILERFEATURES_SUPPORT_CONSTEXPR)
#error Relaxed 'constexpr' support should imply 'constexpr' support!
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
