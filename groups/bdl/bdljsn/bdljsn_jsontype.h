// bdljsn_jsontype.h                                                  -*-C++-*-
#ifndef INCLUDED_BDLJSN_JSONTYPE
#define INCLUDED_BDLJSN_JSONTYPE

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Enumerate the set of JSON value types.
//
//@CLASSES:
//  bdljsn::JsonType: namespace for a 'enum' of JSON value types
//
//@SEE_ALSO: bdljsn_json, bdljsn_jsonutil, bdljsn_jsonvalue
//
//@DESCRIPTION: This component provides a namespace for the 'enum' type
// 'bdljsn::JsonType::Enum', which enumerates the set of fundamental JSON
// types in the JSON specification (see http://json.org).
//
///Enumerators
///-----------
//..
//  Name            Description
//  -------------   ---------------------------------------------------
//  e_OBJECT        A JSON object
//  e_ARRAY         A JSON array
//  e_STRING        A JSON string
//  e_NUMBER        A JSON number
//  e_BOOLEAN       true or false
//  e_NULL          null
//..
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Basic Syntax
///- - - - - - - - - - - -
// The following snippets of code provide a simple illustration of using
// 'bdljsn::JsonType'.
//
// First, we create a variable 'value' of type 'bdljsn::JsonType::Enum' and
// initialize it with the enumerator value 'bdljsn::JsonType::e_STRING':
//..
//  bdljsn::JsonType::Enum value = bdljsn::JsonType::e_STRING;
//..
// Now, we store the address of its ASCII representation in a pointer variable,
// 'asciiValue', of type 'const char *':
//..
//  const char *asciiValue = bdljsn::JsonType::toAscii(value);
//  assert(0 == bsl::strcmp(asciiValue, "STRING"));
//..
// Finally, we print 'value' to 'bsl::cout'.
//..
//  bsl::cout << value << bsl::endl;
//..
// This statement produces the following output on 'stdout':
//..
//  STRING
//..

#include <bdlscm_version.h>

#include <bsl_iosfwd.h>

namespace BloombergLP {
namespace bdljsn {

                              // ===============
                              // struct JsonType
                              // ===============

struct JsonType {
    // This 'struct' provides a namespace for enumerating the set value types
    // of the JSON language.  See 'Enum' in the TYPES sub-section for details.
    //
    // This class:
    //: o supports a complete set of *enumeration* operations
    //:   o except for 'bdex' serialization
    //: o is 'const' *thread-safe*
    //
    // For terminology see {'bsldoc_glossary'}.

  public:
    // TYPES
    enum Enum {
        e_OBJECT,
        e_ARRAY,
        e_STRING,
        e_NUMBER,
        e_BOOLEAN,
        e_NULL
    };

  public:
    // CLASS METHODS
    static bsl::ostream& print(bsl::ostream&  stream,
                               JsonType::Enum value,
                               int            level          = 0,
                               int            spacesPerLevel = 4);
        // Write the string representation of the specified enumeration 'value'
        // to the specified output 'stream', and return a reference to
        // 'stream'.  Optionally specify an initial indentation 'level', whose
        // absolute value is incremented recursively for nested objects.  If
        // 'level' is specified, optionally specify 'spacesPerLevel', whose
        // absolute value indicates the number of spaces per indentation level
        // for this and all of its nested objects.  If 'level' is negative,
        // suppress indentation of the first line.  If 'spacesPerLevel' is
        // negative, format the entire output on one line, suppressing all but
        // the initial indentation (as governed by 'level').  See 'toAscii' for
        // what constitutes the string representation of a 'JsonType::Enum'
        // value.

    static const char *toAscii(JsonType::Enum value);
        // Return the non-modifiable string representation corresponding to the
        // specified enumeration 'value', if it exists, and a unique (error)
        // string otherwise.  The string representation of 'value' matches its
        // corresponding enumerator name with the "e_" prefix elided.  For
        // example:
        //..
        //  bsl::cout << JsonType::toAscii(JsonType::e_STRING);
        //..
        // will print the following on standard output:
        //..
        //  STRING
        //..
        // Note that specifying a 'value' that does not match any of the
        // enumerators will result in a string representation that is distinct
        // from any of those corresponding to the enumerators, but is otherwise
        // unspecified.
};

// FREE OPERATORS
bsl::ostream& operator<<(bsl::ostream& stream, JsonType::Enum value);
    // Write the string representation of the specified enumeration 'value' to
    // the specified output 'stream' in a single-line format, and return a
    // reference to 'stream'.  See 'toAscii' for what constitutes the string
    // representation of a 'bdljsn::JsonType::Enum' value.  Note that this
    // method has the same behavior as
    //..
    //  bdljsn::JsonType::print(stream, value, 0, -1);
    //..

}  // close package namespace

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

                              // ---------------
                              // struct JsonType
                              // ---------------

// FREE OPERATORS
inline
bsl::ostream& bdljsn::operator<<(bsl::ostream& stream, JsonType::Enum value)
{
    return JsonType::print(stream, value, 0, -1);
}

}  // close enterprise namespace

#endif  // INCLUDED_BDLJSN_JSONTYPE

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
