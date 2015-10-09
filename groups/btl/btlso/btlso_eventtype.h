// btlso_eventtype.h                                                  -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BTLSO_EVENTTYPE
#define INCLUDED_BTLSO_EVENTTYPE

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Enumerate the set of socket event types.
//
//@CLASSES:
//  btlso::EventType: namespace for enumerating socket event types
//
//@DESCRIPTION: This component provides a namespace for the 'enum' type,
// 'btlso::EventType::Type', which enumerates the set of event types that can
// be associated with a socket.
//
///Enumerators
///-----------
//..
//  Name       Description
//  ---------  ---------------------------------------------------
//  e_ACCEPT   Event denoting that socket is ready to accept a connection.
//
//  e_CONNECT  Event denoting that socket is ready to connect to a peer.
//
//  e_READ     Event denoting that socket has data available to be read.
//
//  e_WRITE    Event denoting that socket is ready to write data.
//..
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Basic Syntax
///- - - - - - - - - - - -
// The following snippets of code provide a simple illustration of using
// 'btlso::EventType'.
//
// First, we create a variable 'value' of type 'btlso::EventType::Type' and
// initialize it with the enumerator value 'btlso::EventType::e_CONNECT':
//..
//  btlso::EventType::Type value = btlso::EventType::e_CONNECT;
//..
// Now, we store the address of its ASCII representation in a pointer variable,
// 'asciiValue', of type 'const char *':
//..
//  const char *asciiValue = btlso::EventType::toAscii(value);
//  assert(0 == bsl::strcmp(asciiValue, "CONNECT"));
//..
// Finally, we print 'value' to 'bsl::cout'.
//..
//  bsl::cout << value << bsl::endl;
//..
// This statement produces the following output on 'stdout':
//..
//  CONNECT
//..

#ifndef INCLUDED_BTLSCM_VERSION
#include <btlscm_version.h>
#endif

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif

namespace BloombergLP {
namespace btlso {

                    // ===============
                    // class EventType
                    // ===============

struct EventType {
    // This 'struct' provides a namespace for enumerating the set of socket
    // event types.  See 'Type' in the TYPES sub-section for details.
    //
    // This class:
    //: o supports a complete set of *enumeration* operations
    //:   o except for 'bdex' serialization
    // For terminology see 'bsldoc_glossary'.

  public:
    // TYPES
    enum Type {
        e_ACCEPT  = 0,
        e_CONNECT = 1,
        e_READ    = 2,
        e_WRITE   = 3


    };

    enum {
        k_LENGTH = e_WRITE + 1  // Define 'k_LENGTH' to be the number of
                                // consecutively-valued enumerators in 'Type',
                                // in the range '[0 .. k_LENGTH - 1]'.

    };

    // CLASS METHODS
    static bsl::ostream& print(bsl::ostream&   stream,
                               EventType::Type value,
                               int             level          = 0,
                               int             spacesPerLevel = 4);
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
        // what constitutes the string representation of a 'EventType::Type'
        // value.

    static const char *toAscii(EventType::Type value);
        // Return the non-modifiable string representation corresponding to the
        // specified enumeration 'value', if it exists, and a unique (error)
        // string otherwise.  The string representation of 'value' matches its
        // corresponding enumerator name with the "e_" prefix elided.  For
        // example:
        //..
        //  bsl::cout << EventType::toAscii(EventType::e_CONNECT);
        //..
        // will print the following on standard output:
        //..
        //  CONNECT
        //..
        // Note that specifying a 'value' that does not match any of the
        // enumerators will result in a string representation that is distinct
        // from any of those corresponding to the enumerators, but is otherwise
        // unspecified.
};

// FREE OPERATORS
bsl::ostream& operator<<(bsl::ostream& stream, EventType::Type value);
    // Write the string representation of the specified enumeration 'value' to
    // the specified output 'stream' in a single-line format, and return a
    // reference to 'stream'.  See 'toAscii' for what constitutes the string
    // representation of a 'btlso::EventType::Type' value.  Note that this
    // method has the same behavior as
    //..
    //  btlso::EventType::print(stream, value, 0, -1);
    //..

}  // close package namespace

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

                            // ----------------
                            // struct EventType
                            // ----------------

// FREE OPERATORS
inline
bsl::ostream& btlso::operator<<(bsl::ostream&          stream,
                                btlso::EventType::Type value)
{
    return EventType::print(stream, value, 0, -1);
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
