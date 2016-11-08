// bdld_datummaker.cpp                                                -*-C++-*-

#include <bdld_datummaker.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdld_datummaker_cpp, "$Id$ $CSID$")

//
///Implementation Notes
///--------------------
///
///R-value and forwarding references
///- - - - - - - - - - - - - - - - -
// As you may see in the header file variadic overloads (for example the
// 'pushBackHelper') do not support perfect forwarding of their arguments.  In
// case 'bdld::Datum' will ever support move semantics to some (any) of its
// directly supported types ('create*' function arguments) variadic functions
// in this component shall be updated to support perfect forwarding using the
// 'BSLS_COMPILERFEATURES_FORWARD', and 'BSLS_COMPILERFEATURES_FORWARDING_REF'
// macros.

namespace BloombergLP {
namespace bdld {
                          // All methods are inline.
}  // close package namespace
}  // close enterprise namespace

// ----------------------------------------------------------------------------
// Copyright 2016 Bloomberg Finance L.P.
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
