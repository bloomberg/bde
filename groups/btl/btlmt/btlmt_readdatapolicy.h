// btlmt_readdatapolicy.h                                             -*-C++-*-
#ifndef INCLUDED_BTLMT_READDATAPOLICY
#define INCLUDED_BTLMT_READDATAPOLICY

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Enumerate policies for reading data from multiple channels.
//
//@CLASSES:
//  btlmt::ReadDataPolicy: namespace for channel read data policy 'enum'
//
//@SEE_ALSO: btlmt_channelpoolconfiguration, btlmt_channelpool
//
//@AUTHOR: Rohan Bhindwale (rbhindwale)
//
//@DESCRIPTION: This component provides a namespace for the 'enum' type
// 'btlmt::ReadDataPolicy::Value', which enumerates the set of policies for
// reading data from a collection of channels that have data available.
//
///Enumerators
///-----------
//..
//  Name            Description
//  -------------   ---------------------------------------------------
//  e_GREEDY        Read data repeatedly from a channel until the data for
//                  that channel is exhausted before moving onto the next
//                  channel to read data.
//
//  e_ROUND_ROBIN   Perform a single read for each channel, in a round-robin
//                  manner
//..
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Basic Syntax
///- - - - - - - - - - - -
// The following snippets of code provide a simple illustration of using
// 'btlmt::ReadDataPolicy'.
//
// First, we create a variable 'value' of type 'btlmt::ReadDataPolicy::Value'
// and initialize it with the enumerator value
// 'btlmt::ReadDataPolicy::e_GREEDY':
//..
//  btlmt::ReadDataPolicy::Value value = btlmt::ReadDataPolicy::e_GREEDY;
//..
// Now, we store the address of its ASCII representation in a pointer variable,
// 'asciiValue', of type 'const char *':
//..
//  const char *asciiValue = btlmt::ReadDataPolicy::toAscii(value);
//  assert(0 == bsl::strcmp(asciiValue, "GREEDY"));
//..
// Finally, we print 'value' to 'bsl::cout'.
//..
//  bsl::cout << value << bsl::endl;
//..
// This statement produces the following output on 'stdout':
//..
//  GREEDY
//..

#ifndef INCLUDED_BTLSCM_VERSION
#include <btlscm_version.h>
#endif

#ifndef INCLUDED_BDLAT_ENUMERATORINFO
#include <bdlat_enumeratorinfo.h>
#endif

#ifndef INCLUDED_BDLAT_TYPETRAITS
#include <bdlat_typetraits.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif

#ifndef INCLUDED_BSL_OSTREAM
#include <bsl_ostream.h>
#endif

#ifndef INCLUDED_BSL_STRING
#include <bsl_string.h>
#endif

namespace BloombergLP {
namespace btlmt {

                              // =====================
                              // struct ReadDataPolicy
                              // =====================

struct ReadDataPolicy {
    // This 'struct' provides a namespace for enumerating the set of policies
    // for reading data from a channel.  See 'Value' in the TYPES sub-section
    // for details.
    //
    // This class:
    //: o supports a complete set of *enumeration* operations
    //:   o except for 'bdex' serialization
    //: o is 'const' *thread-safe*
    // For terminology see 'bsldoc_glossary'.

  public:
    // TYPES
    enum Value {
        e_GREEDY = 0, // Read data repeatedly from a channel until the data for
                      // that channel is exhausted before moving onto the next
                      // channel to read data.

        e_ROUND_ROBIN // Perform a single read for each channel, in a
                      // round-robin manner.
    };

    enum {
        NUM_ENUMERATORS = 2
    };

    // CONSTANTS
    static const char CLASS_NAME[];

    static const bdlat_EnumeratorInfo ENUMERATOR_INFO_ARRAY[];

    // CLASS METHODS
    static const char *toString(Value value);
        // Return the string representation exactly matching the enumerator
        // name corresponding to the specified enumeration 'value'.

    static int fromString(Value        *result,
                          const char   *string,
                          int           stringLength);
        // Load into the specified 'result' the enumerator matching the
        // specified 'string' of the specified 'stringLength'.  Return 0 on
        // success, and a non-zero value with no effect on 'result' otherwise
        // (i.e., 'string' does not match any enumerator).

    static int fromString(Value              *result,
                          const bsl::string&  string);
        // Load into the specified 'result' the enumerator matching the
        // specified 'string'.  Return 0 on success, and a non-zero value with
        // no effect on 'result' otherwise (i.e., 'string' does not match any
        // enumerator).

    static int fromInt(Value *result, int number);
        // Load into the specified 'result' the enumerator matching the
        // specified 'number'.  Return 0 on success, and a non-zero value with
        // no effect on 'result' otherwise (i.e., 'number' does not match any
        // enumerator).

    static bsl::ostream& print(bsl::ostream&         stream,
                               ReadDataPolicy::Value value,
                               int                   level          = 0,
                               int                   spacesPerLevel = 4);
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
        // 'ReadDataPolicy::Value' value.

    static const char *toAscii(ReadDataPolicy::Value value);
        // Return the non-modifiable string representation corresponding to the
        // specified enumeration 'value', if it exists, and a unique (error)
        // string otherwise.  The string representation of 'value' matches its
        // corresponding enumerator name with the "e_" prefix elided.  For
        // example:
        //..
        //  bsl::cout << ReadDataPolicy::toAscii(ReadDataPolicy::e_GREEDY);
        //..
        // will print the following on standard output:
        //..
        //  GREEDY
        //..
        // Note that specifying a 'value' that does not match any of the
        // enumerators will result in a string representation that is distinct
        // from any of those corresponding to the enumerators, but is otherwise
        // unspecified.
};

// FREE OPERATORS
bsl::ostream& operator<<(bsl::ostream& stream, ReadDataPolicy::Value value);
    // Write the string representation of the specified enumeration 'value' to
    // the specified output 'stream' in a single-line format, and return a
    // reference to 'stream'.  See 'toAscii' for what constitutes the string
    // representation of a 'btlmt::ReadDataPolicy::Value' value.  Note that
    // this method has the same behavior as
    //..
    //  btlmt::ReadDataPolicy::print(stream, value, 0, -1);
    //..

}  // close package namespace

// TRAITS
BDLAT_DECL_ENUMERATION_TRAITS(btlmt::ReadDataPolicy)

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

namespace btlmt {
                              // ---------------------
                              // struct ReadDataPolicy
                              // ---------------------

// CLASS METHODS
inline
int ReadDataPolicy::fromString(Value *result, const bsl::string& string)
{
    return fromString(result,
                      string.c_str(),
                      static_cast<int>(string.length()));
}

inline
const char *ReadDataPolicy::toString(ReadDataPolicy::Value value)
{
    return toAscii(value);
}

}  // close package namespace

// FREE OPERATORS
inline
bsl::ostream& btlmt::operator<<(bsl::ostream&         stream,
                                ReadDataPolicy::Value value)
{
    return ReadDataPolicy::print(stream, value, 0, -1);
}

}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2018 Bloomberg Finance L.P.
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
