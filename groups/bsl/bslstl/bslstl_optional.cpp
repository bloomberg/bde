// bslstl_optional.cpp                                             -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include "../../bsl/bslstl/bslstl_optional.h"

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bslstl_optional_cpp,"$Id$ $CSID$")
namespace bsl {
#ifndef __cpp_lib_optional
nullopt_t nullopt = nullopt_t(2);
in_place_t in_place;
#endif // __cpp_lib_optional
}  // close package namespace
namespace BloombergLP {
namespace bslstl {
Optional_OptNoSuchType optNoSuchType;
} // close package namespace
} // close enterprise namespace

// todo : update copyright
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
