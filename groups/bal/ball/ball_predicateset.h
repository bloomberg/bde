// ball_predicateset.h                                                -*-C++-*-
#ifndef INCLUDED_BALL_PREDICATESET
#define INCLUDED_BALL_PREDICATESET

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a container for managed attributes.
//
//@CLASSES:
//  ball::PredicateSet: a container for managed predicates
//
//@DEPRECATED: Use `ball_managedattributeset` instead.
//
//@SEE_ALSO:
//
//@DESCRIPTION: This deprecated component defines `ball::PredicateSet`, a
// compatibility type alias to `ball::ManagedAttributeSet`.

#include <balscm_version.h>

#include <ball_managedattributeset.h>

namespace BloombergLP {
namespace ball {

/// **DEPRECATED**: Use `ball::ManagedAttributeSet` instead.
typedef ManagedAttributeSet PredicateSet;

}  // close package namespace
}  // close enterprise namespace

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
