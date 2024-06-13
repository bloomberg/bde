// bslstl_variant.cpp                                                 -*-C++-*-

#include <bslstl_variant.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bslstl_variant_cpp, "$Id$ $CSID$")

#include <bslstl_string.h>                          // for testing only

///Implementation Notes
///--------------------
// The C++03 versions of the 'get' and 'get_if' functions are declared with
// non-variadic signatures because the IBM compiler does not like the generated
// code for functions that take an argument of template class with variadic
// template arguments.  The problem was only observed with the 'get' and
// 'get_if' functions, and the exact issue is not known.
//
// Because 'get' and 'get_if' are not variadic templates, we rely on SFINAE to
// remove them from the candidate set when the argument is not a (possibly
// cv-qualified) 'bsl::variant' nor derived therefrom.  The implementation of
// SFINAE on SunCC appears to have a bug in which a complex return type of the
// form 'typename A<typename B<T>::type&>::type' causes a hard error when the
// inner type doesn't exist (see DRQS 175366735); therefore, such return types
// must be avoided.

namespace BloombergLP {
namespace bslstl {
}  // close package namespace
}  // close enterprise namespace

// ----------------------------------------------------------------------------
// Copyright 2023 Bloomberg Finance L.P.
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
