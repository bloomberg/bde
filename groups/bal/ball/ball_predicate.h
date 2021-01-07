// ball_predicate.h                                                   -*-C++-*-
#ifndef INCLUDED_BALL_PREDICATE
#define INCLUDED_BALL_PREDICATE

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a predicate object that consists of a name/value pair.
//
//@CLASSES:
//  ball::Predicate: a predicate in the form of an attribute name/value pair
//
//@DEPRECATED: Use 'ball_managedattribute' instead.
//
//@SEE_ALSO:
//
//@DESCRIPTION: This component implements a value-semantic predicate object,
// 'ball::Predicate', that consists of a name and a value.  The value can be an
// 'int', a 64-bit integer, or a 'bsl::string'.  Both the name and value are
// managed by this object.

#include <balscm_version.h>

#include <ball_managedattribute.h>

namespace BloombergLP {
namespace ball {

typedef ManagedAttribute Predicate;
    // !DEPRECATED!: Use 'ball::ManagedAttribute' instead.

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
