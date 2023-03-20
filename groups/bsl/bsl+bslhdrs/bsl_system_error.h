// bsl_system_error.h                                                 -*-C++-*-
#ifndef INCLUDED_BSL_SYSTEM_ERROR
#define INCLUDED_BSL_SYSTEM_ERROR

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide functionality of the corresponding C++ Standard header.
//
//@DESCRIPTION: Provide types, in the 'bsl' namespace, equivalent to those
// defined in the corresponding C++ standard header.  In C++03 mode we provide
// our own implementation, and in C++11 or later we import the 'std' names.

// Include Bloomberg's implementation.
#include <bslstl_errc.h>
#include <bslstl_error.h>
#include <bslstl_iserrorcodeenum.h>
#include <bslstl_iserrorconditionenum.h>
#include <bslstl_systemerror.h>

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
