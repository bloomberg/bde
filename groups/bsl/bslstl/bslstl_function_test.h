// bslstl_function_test.h                                             -*-C++-*-
#ifndef INCLUDED_BSLSTL_FUNCTION_TEST
#define INCLUDED_BSLSTL_FUNCTION_TEST

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide support for the 'bslstl_function_test.t.cpp' test driver.
//
//@CLASSES:
//
//@SEE_ALSO: bslstl_function
//
//@DESCRIPTION: This component provides a second translation unit for
// 'function' test cases to isolate certain tests that may be triggering an
// error in 'gcc' or else may be exhibiting undefined behavior.  Some of these
// tests fail to fail merely in the presence of other code, such as in the main
// 'bslstl_function' test driver.

#include <bslscm_version.h>

#include <bslstl_function.h>

#endif
// ----------------------------------------------------------------------------
// Copyright 2020 Bloomberg Finance L.P.
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
