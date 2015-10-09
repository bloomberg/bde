// ball_userfieldtype.h                                               -*-C++-*-
#ifndef INCLUDED_BALL_USERFIELDTYPE
#define INCLUDED_BALL_USERFIELDTYPE

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Enumerate the set of data types for a user supplied attribute.
//
//@CLASSES:
//  ball::UserFieldType: namespace for an enum of user supplied attribute types
//
//@DESCRIPTION: This component provides a namespace for the 'enum' type,
// 'ball::UserFieldType', that enumerates the set of types a user defined log
// attribute may have.
//
///Enumerators
///-----------
//..
//  Name                           Description
//  -------------------            ----------------------------------
//  e_VOID                         Enumerator representing an unset value
//  e_INT64                        Enumerator representing an integral value
//  e_DOUBLE                       Enumerator representing an double value
//  e_STRING                       Enumerator representing a string value
//  e_DATETIMETZ                   Enumerator representing a DatetimeTz value
//..
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Basic Syntax
///- - - - - - - - - - - -
// The following snippets of code provide a simple illustration of
// 'ball::UserFieldType' usage.
//
// First, we create a variable 'value' of type 'ball::UserFieldType::Enum' and
// initialize it to the value 'ball::UserFieldType::e_STRING':
//..
//  ball::UserFieldType::Enum value = ball::UserFieldType::e_STRING;
//..
// Next, we store a pointer to its ASCII representation in a variable
// 'asciiValue' of type 'const char *':
//..
//  const char *asciiValue = ball::UserFieldType::toAscii(value);
//  assert(0 == bsl::strcmp(asciiValue, "STRING"));
//..
// Finally, we print the value to 'bsl::cout':
//..
//  bsl::cout << value << bsl::endl;
//..
// This statement produces the following output on 'stdout':
//..
//  STRING
//..

#ifndef INCLUDED_BALSCM_VERSION
#include <balscm_version.h>
#endif

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif

namespace BloombergLP {
namespace ball {

                        // ====================
                        // struct UserFieldType
                        // ====================

struct UserFieldType {
    // This 'struct' provides a namespace for enumerating month-of-year
    // values.  See 'Enum' in the TYPES sub-section for details.
    //
    // This 'struct':
    //: o supports a complete set of *enumeration* operations
    // For terminology see 'bsldoc_glossary'.

  public:
    // TYPES
    enum Enum {
        // Define the list of month-of-year values.

        e_VOID,
        e_INT64,
        e_DOUBLE,
        e_STRING,
        e_DATETIMETZ

    };

    // CLASS METHODS
    static bsl::ostream& print(bsl::ostream&       stream,
                               UserFieldType::Enum value,
                               int                 level          = 0,
                               int                 spacesPerLevel = 4);
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
        // what constitutes the string representation of a
        // 'UserFieldType::Enum' value.

    static const char *toAscii(UserFieldType::Enum value);
        // Return the non-modifiable string representation corresponding to the
        // specified enumeration 'value', if it exists, and a unique (error)
        // string otherwise.  The string representation of 'value' matches the
        // first 3 characters of its corresponding enumerator name with the
        // "e_" prefix elided.  For example:
        //..
        //  bsl::cout << ball::UserFieldType::toAscii(
        //                                    ball::UserFieldType::e_STRING);
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
bsl::ostream& operator<<(bsl::ostream& stream, UserFieldType::Enum value);
    // Write the string representation of the specified enumeration 'value' to
    // the specified output 'stream' in a single-line format, and return a
    // reference to 'stream'.  See 'toAscii' for what constitutes the string
    // representation of a 'ball::UserFieldType::Enum' value.  Note that this
    // method has the same behavior as
    //..
    //  ball::UserFieldType::print(stream, value, 0, -1);
    //..

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

                             // ----------------------
                             // struct UserFieldType
                             // ----------------------


}  // close package namespace

// FREE OPERATORS
inline
bsl::ostream& ball::operator<<(bsl::ostream&             stream,
                               ball::UserFieldType::Enum value)
{
    return ball::UserFieldType::print(stream, value, 0, -1);
}

}  // close enterprise namespace

#endif

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
