// bsl_print_ostream.h                                                -*-C++-*-
#ifndef INCLUDED_BSL_PRINT_OSTREAM
#define INCLUDED_BSL_PRINT_OSTREAM

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide 'bsl::print' overloads for 'ostream'
//
//@DESCRIPTION: Provides the overloads of 'bsl::print' that take an 'ostream'
// as the first argument, and are defined in '<ostream>' in C++23.
//
// These overloads are not included by 'bsl_print.h' and must be included
// separately to control overhead.  Including this functionality by default
// causes too much debug information growth in existing large systems that do
// not use 'print'.

#include <bslfmt_print_ostream.h>

#endif

// ----------------------------------------------------------------------------
// Copyright 2026 Bloomberg Finance L.P.
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
