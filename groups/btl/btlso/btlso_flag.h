// btlso_flag.h                                                       -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BTLSO_FLAG
#define INCLUDED_BTLSO_FLAG

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Enumerate all flags for stream-based-channel transport.
//
//@CLASSES:
//  btlso_Flag: namespace for enumerating all stream-based-channel flags
//
//@DESCRIPTION: This component provides a namespace for the 'enum' type,
// 'btlso_Flag', for enumerating all flags of use to the various socket-based
// components.  Functionality is provided to convert each of these enumerated
// values to its corresponding string representation, to write its string form
// directly to a standard 'ostream'.  In addition, this class supports
// functions that convert these types to a well-defined ascii representation.
//
///Enumerators
///-----------
//..
//  Type         Name                Description
//  ------------ -----------------   -----------------------------------------
//  FlagType
//               k_ASYNC_INTERRUPT   Flag permitting an operation to be
//                                   interrupted by an asynchronous event
//
//               k_NFLAGS            Number of 'FlagType' enumerators
//
//  BlockingMode
//               e_BLOCKING_MODE     Flag indicating blocking mode
//
//               e_NONBLOCKING_MODE  Flag indicating non-blocking mode
//
//  ShutdownType
//               e_SHUTDOWN_RECEIVE  Shut down input stream of connection
//
//               e_SHUTDOWN_SEND     Shut down output stream of connection
//
//               e_SHUTDOWN_BOTH     Shut down both input and output streams
//                                   of connection
//
//  IOWaitType
//               e_IO_READ           Wait for data to arrive on a socket
//
//               e_IO_WRITE          Wait for buffer space to write on a socket
//
//               e_IO_RW             Wait for data to arrive or buffer space
//                                   to write on a socket
//..
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Basic Syntax
///- - - - - - - - - - - -
// The following snippets of code provide a simple illustration of using
// one of the enumerations defined in this component, 'btlso::BlockingMode'.
//
// First, we create a variable 'value' of type 'btlso::Flag::BlockingMode' and
// initialize it with the enumerator value 'btlso::Flag::e_NONBLOCKING_MODE':
//..
//  btlso::Flag::BlockingMode value = btlso::Flag::e_NONBLOCKING_MODE;
//..
// Now, we store the address of its ASCII representation in a pointer variable,
// 'asciiValue', of type 'const char *':
//..
//  const char *asciiValue = btlso::Flag::toAscii(value);
//  assert(0 == bsl::strcmp(asciiValue, "NONBLOCKING_MODE"));
//..
// Finally, we print 'value' to 'bsl::cout'.
//..
//  bsl::cout << value << bsl::endl;
//..
// This statement produces the following output on 'stdout':
//..
//  NONBLOCKING_MODE
//..

#ifndef INCLUDED_BTLSCM_VERSION
#include <btlscm_version.h>
#endif

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif

namespace BloombergLP {
namespace btlso {

                        // ==========
                        // class Flag
                        // ==========

struct Flag {
    // This class provides a namespace for enumerating all flags for the
    // 'btlso' package.

    // TYPES
    enum FlagType {
        // Value used to specify if an operation can be interrupted by an
        // asynchronous event.

        k_ASYNC_INTERRUPT = 0x01, // If set, this flag permits an operation to
                                  // be interrupted by an unspecified
                                  // asynchronous event.  By default, the
                                  // implementation will ignore such events if
                                  // possible, or fail otherwise.

        k_NFLAGS          = 1     // The number of FlagType enumerators.  This
                                  // must be maintained "by hand" since flags
                                  // are not consecutive.


    };

    enum BlockingMode {
        // Values used to set/determine the blocking mode of a
        // 'btlso::StreamSocket' object.

        e_BLOCKING_MODE,    // Indicates blocking mode
        e_NONBLOCKING_MODE  // Indicates non-blocking mode


    };

    enum ShutdownType {
        // Values for options used by 'btlso::StreamSocket<>::shutdown'.

        e_SHUTDOWN_RECEIVE, // Shut down the input stream of the full-duplex
                            // connection associated with a
                            // 'btlso::StreamSocket' object.

        e_SHUTDOWN_SEND,    // Shut down the output stream of the full-duplex
                            // connection associated with a
                            // 'btlso::StreamSocket' object.

        e_SHUTDOWN_BOTH     // Shut down the input and output streams of the
                            // full-duplex connection associated a
                            // 'btlso::StreamSocket' object.


    };

    enum IOWaitType {
        // Values for options used by 'btlso::StreamSocket<>::waitForIO'.

        e_IO_READ,  // Wait for data to arrive on a socket.

        e_IO_WRITE, // Wait for buffer space to become available on a socket.

        e_IO_RW     // Wait for data to arrive or space to become available on
                    // a socket.


    };

    // CLASS METHODS
    static bsl::ostream& streamOut(bsl::ostream& stream, Flag::FlagType rhs);
        // Write to the specified 'stream' the string representation exactly
        // matching the name corresponding to the specified 'rhs' value.

    static bsl::ostream& streamOut(bsl::ostream&      stream,
                                   Flag::BlockingMode rhs);
        // Write to the specified 'stream' the string representation exactly
        // matching the name corresponding to the specified 'rhs' value.

    static bsl::ostream& streamOut(bsl::ostream&      stream,
                                   Flag::ShutdownType rhs);
        // Write to the specified 'stream' the string representation exactly
        // matching the name corresponding to the specified 'rhs' value.

    static bsl::ostream& streamOut(bsl::ostream& stream, Flag::IOWaitType rhs);
        // Write to the specified 'stream' the string representation exactly
        // matching the name corresponding to the specified 'rhs' value.

    static const char *toAscii(Flag::FlagType value);
        // Return the string representation exactly matching the enumerator
        // name corresponding to the specified enumerator 'value'.

    static const char *toAscii(Flag::BlockingMode value);
        // Return the string representation exactly matching the enumerator
        // name corresponding to the specified enumerator 'value'.

    static const char *toAscii(Flag::ShutdownType value);
        // Return the string representation exactly matching the enumerator
        // name corresponding to the specified enumerator 'value'.

    static const char *toAscii(Flag::IOWaitType value);
        // Return the string representation exactly matching the enumerator
        // name corresponding to the specified enumerator 'value'.
};

// FREE FUNCTIONS
inline
bsl::ostream& operator<<(bsl::ostream& stream, Flag::FlagType rhs);
    // Write to the specified 'stream' the string representation exactly
    // matching the name corresponding to the specified 'rhs' value.

inline
bsl::ostream& operator<<(bsl::ostream& stream, Flag::BlockingMode rhs);
    // Write to the specified 'stream' the string representation exactly
    // matching the name corresponding to the specified 'rhs' value.

inline
bsl::ostream& operator<<(bsl::ostream& stream, Flag::ShutdownType rhs);
    // Write to the specified 'stream' the string representation exactly
    // matching the name corresponding to the specified 'rhs' value.

inline
bsl::ostream& operator<<(bsl::ostream& stream, Flag::IOWaitType rhs);
    // Write to the specified 'stream' the string representation exactly
    // matching the name corresponding to the specified 'rhs' value.

}  // close package namespace

// ============================================================================
//                        INLINE FUNCTION DEFINITIONS
// ============================================================================

                        // ----------
                        // class Flag
                        // ----------
// FREE OPERATORS
inline
bsl::ostream& btlso::operator<<(bsl::ostream&         stream,
                                btlso::Flag::FlagType rhs)
{
    return btlso::Flag::streamOut(stream, rhs);
}

inline
bsl::ostream& btlso::operator<<(bsl::ostream&             stream,
                                btlso::Flag::BlockingMode rhs)
{
    return btlso::Flag::streamOut(stream, rhs);
}

inline
bsl::ostream& btlso::operator<<(bsl::ostream&             stream,
                                btlso::Flag::ShutdownType rhs)
{
    return btlso::Flag::streamOut(stream, rhs);
}

inline
bsl::ostream& btlso::operator<<(bsl::ostream&           stream,
                                btlso::Flag::IOWaitType rhs)
{
    return btlso::Flag::streamOut(stream, rhs);
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
