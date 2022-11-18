// bdljsn_jsonnull.h                                                  -*-C++-*-
#ifndef INCLUDED_BDLJSN_JSONNULL
#define INCLUDED_BDLJSN_JSONNULL

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a type that represents the JSON 'null' value.
//
//@CLASSES:
//  bdljsn::JsonNull: type that represents the JSON 'null' value.
//
//@SEE_ALSO: bdljsn_json
//
//@DESCRIPTION: This component provides a single value-semantic type,
// 'bdljsn::JsonNull', that can represent the JSON 'null' value.  This
// provides a degenerate (extremely limited) set of the conventional
// functionality.  Objects of this class can be:
//
//: o default constructed
//: o printed ("null" is output)
//: o compared with each other (always equal).
//
// Significantly, there is no way to specify or change the state of these
// objects.  Thus, each object always has the same value as the others.
//
// Additionally, support is provided for hashing via the 'hashAppend' free
// function and also a 'swap' function.
//
///Usage
///-----
// In this section we show intended usage of this component.
//
///Example 1: Basic Syntax
///- - - - - - - - - - - -
// The scenario below illustrates almost all of the supported operations on the
// 'bdljsn::JsonNull' type:
//
// First, we create a 'bdljsn::JsonNull' object:
//..
//  bdljsn::JsonNull a;
//..
// Then, we examine the object's printed representation:
//..
//  bsl::ostringstream oss;
//  oss << a;
//  assert("null" == oss.str());
//..
// Next, we create a second object of that class and confirm that it equals the
// object created above.
//..
//  bdljsn::JsonNull b;
//  assert( (a == b));
//  assert(!(a != b));
//..
// Finally, we confirm that swapping the two objects has no effect.
//..
//  swap(a, b);
//  assert(a == b);
//..

#include <bdlscm_version.h>

#include <bslh_hash.h> // 'hashAppend(HASH_ALG, int)'

#include <bsl_iosfwd.h>

namespace BloombergLP {
namespace bdljsn {

                               // ==============
                               // class JsonNull
                               // ==============

class JsonNull {
    // This type represents the JSON null value.  There is only 1 value of this
    // type, so all objects compare equal, hash to the same value, etc.

  public:
    // CREATORS
    JsonNull();
        // Create a "null" JSON respresentation.  The created object is equal
        // to all other 'JsonNull' objects.

    // ACCESSORS
    bsl::ostream& print(bsl::ostream& stream,
                        int           level          = 0,
                        int           spacesPerLevel = 4) const;
        // Write "null" to the specified output 'stream', and return a
        // reference to 'stream'.  Optionally specify an initial indentation
        // 'level', whose absolute value is incremented recursively for nested
        // objects.  If 'level' is specified, optionally specify
        // 'spacesPerLevel', whose absolute value indicates the number of
        // spaces per indentation level for this and all of its nested objects.
        // If 'level' is negative, suppress indentation of the first line.  If
        // 'spacesPerLevel' is negative, format the entire output on one line,
        // suppressing all but the initial indentation (as governed by
        // 'level').
};

// FREE OPERATORS
bool operator==(const JsonNull& lhs, const JsonNull& rhs);
    // Return 'true' (unconditionally) as the specified 'lhs' and 'rhs' must
    // have the same value.

bool operator!=(const JsonNull& lhs, const JsonNull& rhs);
    // Return 'false' (unconditionally) as the specified 'lhs' and 'rhs' cannot
    // have the different values.

bsl::ostream& operator<<(bsl::ostream& stream, const JsonNull& object);
    // Write "null" -- the invariant value of the specified 'object' -- to the
    // specified output 'stream' in a single-line format and return a
    // reference to 'stream'.  Note that this method has the same behavior as
    //..
    //  object.print(stream, 0, -1);
    //..

// FREE FUNCTIONS
template <class HASHALG>
void hashAppend(HASHALG& hashAlgorithm, const JsonNull& object);
    // Pass the specified 'object' to the specified 'hashAlgorithm'.  This
    // function allows 'JsonNull' objects to be hashable using 'bsl::hash'.
    // Note that 'object' always as the same value so, for a given state of
    // 'hashAppend', the effect of calling 'hashAppend' is always the same.

void swap(JsonNull& a, JsonNull& b);
    // Exchange the values of the specified 'a' and 'b' objects.  This
    // exception provides the no-throw exception-safety guarantee.  Note that,
    // since 'a' and 'b' have the same value (a class invariant), this
    // operation is a no-op.

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

                              // --------------
                              // class JsonNull
                              // --------------

// CREATORS
inline
JsonNull::JsonNull()
{
}

}  // close package namespace

// FREE OPERATORS
inline
bsl::ostream& bdljsn::operator<<(bsl::ostream&           stream,
                                 const bdljsn::JsonNull& object)
{
    return object.print(stream, 0, -1);
}

inline
bool bdljsn::operator==(const bdljsn::JsonNull& ,
                        const bdljsn::JsonNull& )
{
    return true;
}

inline
bool bdljsn::operator!=(const bdljsn::JsonNull& ,
                        const bdljsn::JsonNull& )
{
    return false;
}

template <class HASHALG>
inline
void bdljsn::hashAppend(HASHALG& hashAlgorithm, const bdljsn::JsonNull& )
{
    using bslh::hashAppend;
    hashAppend(hashAlgorithm, -1);
}

inline
void bdljsn::swap(bdljsn::JsonNull& , bdljsn::JsonNull& )
{
}

}  // close enterprise namespace

#endif  // INCLUDED_BDLJSN_JSONNULL

// ----------------------------------------------------------------------------
// Copyright 2022 Bloomberg Finance L.P.
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
