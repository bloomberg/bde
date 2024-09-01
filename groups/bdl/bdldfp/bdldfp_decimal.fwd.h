// bdldfp_decimal.fwd.h                                               -*-C++-*-
#ifndef FWD_INCLUDED_BDLDFP_DECIMAL
#define FWD_INCLUDED_BDLDFP_DECIMAL

//@PURPOSE: Provide forward declarations to IEEE-754 decimal floating-pt types.
//
//@CLASSES:
//  bdldfp::Decimal32:   32bit IEEE-754 decimal floating-point type
//  bdldfp::Decimal64:   64bit IEEE-754 decimal floating-point type
//  bdldfp::Decimal128: 128bit IEEE-754 decimal floating-point type
//
//@SEE_ALSO: bdldfp_decimal
//
//@DESCRIPTION: This header provides declarations for those top-level names in
// this component that are intended to be usable in name only.  See above for
// the declared names, and see `bdldfp_decimal.h` for documentation of those
// names.
//
///Implementation Note
///-------------------
// This header also provides forward declarations of component-private types
// due to historical reasons.
//
///The History
///- - - - - -
// At first ISO was expected to quickly standardize IEEE-754 decimal floating
// point types for both C and C++ in a form that could be easily reused by
// aliasing the `bdldfp::Decimal32/64/128` names.  The component-private
// `bdldfp::Decimal_Type32/64/128` classes were introduced to prevent people
// from forward declaring the `bdldfp::Decimal32/64/128` types as classes.
// Unfortunately users forward declared the component-private types despite the
// comment at their declaration instructing them not to do so.  As a result we
// are unable to remove those component-private types without much extra work
// with very little benefits.

namespace BloombergLP {
namespace bdldfp {

                    // HISTORICAL PRIVATE TYPE DECLARATIONS

class Decimal_Type32;
class Decimal_Type64;
class Decimal_Type128;
    // These are the actual (decimal floating-point) types being implemented.
    // They use a different name to cause an error if the official types are
    // forward declared: The exact definition of the decimal types is left
    // unspecified so that that can potentially be aliases for built-in types.

                          // FORWARD DECLARATIONS

typedef Decimal_Type32  Decimal32;
typedef Decimal_Type64  Decimal64;

/// The decimal floating-point types are typedefs to the unspecified
/// implementation types.
typedef Decimal_Type128 Decimal128;

}  // close package namespace
}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2014 Bloomberg Finance L.P.
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
