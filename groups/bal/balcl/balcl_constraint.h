// balcl_constraint.h                                                 -*-C++-*-
#ifndef INCLUDED_BALCL_CONSTRAINT
#define INCLUDED_BALCL_CONSTRAINT

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Define constraint function signatures for supported value types.
//
//@CLASSES:
//   balcl::Constraint: constraint function signatures for supported types
//
//@DESCRIPTION: This component defines a single 'struct', 'balcl::Constraint',
// that provides a namespace for function signatures used to impose
// user-defined constraints on values entered with command-line options (if so
// defined).  Signatures are defined for each of the supported
// command-line-option values of scalar type.
//
// For further details see {'balcl_commandline'|Type-and-Constraint Field} and
// {'balcl_commandline'|Example: Type-and-Constraint Field}.
//
///Usage
///-----
// The intended use of this component is illustrated in
// {'balcl_commandline'|Usage}.

#include <balscm_version.h>

#include <bsls_types.h>     // 'bsls::Types::Int64'

#include <bsl_functional.h> // 'bsl::function'
#include <bsl_iosfwd.h>
#include <bsl_string.h>

namespace BloombergLP {

namespace bdlt { class Date; }
namespace bdlt { class Datetime; }
namespace bdlt { class Time; }

namespace balcl {
                        // =================
                        // struct Constraint
                        // =================

struct Constraint {
    // This 'struct' provides a namespace that defines the types used to impose
    // user-defined constraints on values associated with command-line options.
    // One type is defined for each scalar type defined in 'balcl::OptionType'
    // except for 'bool'.

    // TYPES
    typedef bsl::function<bool(const char               *,
                               bsl::ostream&             )> CharConstraint;
    typedef bsl::function<bool(const int                *,
                               bsl::ostream&             )> IntConstraint;
    typedef bsl::function<bool(const bsls::Types::Int64 *,
                               bsl::ostream&             )> Int64Constraint;
    typedef bsl::function<bool(const double             *,
                               bsl::ostream&             )> DoubleConstraint;
    typedef bsl::function<bool(const bsl::string        *,
                               bsl::ostream&             )> StringConstraint;
    typedef bsl::function<bool(const bdlt::Datetime     *,
                               bsl::ostream&             )> DatetimeConstraint;
    typedef bsl::function<bool(const bdlt::Date         *,
                               bsl::ostream&             )> DateConstraint;
    typedef bsl::function<bool(const bdlt::Time         *,
                               bsl::ostream&             )> TimeConstraint;
        // These types are aliases for function objects (functors) used to
        // express user-defined constraints on option values.  Such objects
        // should return 'true' if the supplied value (first argument) are
        // deemed valid, and 'false' otherwise.  Descriptive error messages may
        // be written to the supplied stream (second argument).
};

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
