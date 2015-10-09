// ball_severity.h                                                    -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BALL_SEVERITY
#define INCLUDED_BALL_SEVERITY

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Enumerate a set of logging severity levels.
//
//@CLASSES:
//   ball::Severity: namespace for enumerating logging severity levels
//
//@DESCRIPTION: This component provides a namespace, 'ball::Severity', for the
// 'enum' type 'ball::Severity::Level'.  'Level' enumerates a list of severity
// levels that can be attached to a logging event.  In addition, this component
// supports functions that convert the 'Level' enumerators to a well-defined
// ASCII representation.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Syntax
///- - - - - - - - -
// The following snippets of code provide a simple illustration of ordinary
// 'ball::Severity' operation.  (The next example discusses a more elaborate
// usage scenario.)
//
// First create a variable 'level' of type 'ball::Severity::Level' and
// initialize it to the value 'ball::Severity::e_ERROR'.
//..
//      ball::Severity::Level level = ball::Severity::e_ERROR;
//..
// Next, store a pointer to its ASCII representation in a variable 'asciiLevel'
// of type 'const char *'.
//..
//      const char *asciiLevel = ball::Severity::toAscii(level);
//      assert(0 == strcmp(asciiLevel, "ERROR"));
//..
// Finally, print the value of 'level' to 'bsl::cout'.
//..
//      bsl::cout << level << bsl::endl;
//..
// This statement produces the following output on 'stdout':
//..
//      ERROR
//..
//
///Example 2: Logging
/// - - - - - - - - -
// Consider a general-purpose logging facility that provides two interfaces:
// one for developers to use when logging from subroutines and another to be
// used by the owner of the main program to administer how much information is
// to be published to a log file.  Messages logged with numeric values at or
// below the globally-administered threshold level are published to the log,
// while those logged with higher (less severe) levels are not.  Being
// general-purpose, we envision that additional levels may be useful in some
// applications.  Hence, the numeric values supplied in this component might be
// augmented with additional severity levels.  For example:
//..
//      enum {
//          MAJOR_ERROR =  48
//          MINOR_ERROR =  80,
//          DEBUG2      = 162
//      };
//..
// Given that library modules using augmented logging schemes may coexist in a
// single program, we would choose not to have the core logging facility depend
// on this enumeration, but instead accept integer log levels in the range
// [0 .. 255].  Hence, those that choose to limit their logging levels to the
// seven defined in 'ball::Severity' can do so, and still coexist on the same
// logging facility along side routines that log with more finely-graduated
// levels of severity.
//
// To facilitate administration, the following enumerated values, in addition
// to any level values supplied to programmers, should be available to the
// owner of the main program to control output:
//..
//      enum {
//          ALL = 255,  // publish all log messages
//          OFF =  -1   // disable logging
//      };
//..
// Setting the global threshold to 'ALL' causes all messages to be published;
// setting it to 'OFF' disables logging.

#ifndef INCLUDED_BALSCM_VERSION
#include <balscm_version.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORM
#include <bsls_platform.h>
#endif

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif




namespace BloombergLP {

namespace ball {
                        // ===============
                        // struct Severity
                        // ===============

struct Severity {
    // This struct provides a namespace for enumerating severity levels.

  public:
    enum Level {
        e_OFF   =   0,  // disable generation of corresponding message
        e_FATAL =  32,  // a condition that will (likely) cause a *crash*
        e_ERROR =  64,  // a condition that *will* cause incorrect behavior
        e_WARN  =  96,  // a *potentially* problematic condition
        e_INFO  = 128,  // data about the running process
        e_DEBUG = 160,  // information useful while debugging
        e_TRACE = 192   // execution trace data

    };

    enum {
        e_LENGTH = 7
    };
        // Define 'e_LENGTH' to be the number of enumerators in the 'Level'
        // enumeration.

  private:
    // PRIVATE CLASS METHODS
    static void print(bsl::ostream& stream, Severity::Level value);
        // Write to the specified 'stream' the string representation of the
        // specified enumeration 'value'.

  public:
    // CLASS METHODS
    static int fromAscii(Severity::Level *level,
                         const char      *string,
                         int              stringLength);
        // Load into the specified 'level' the severity matching the specified
        // case-insensitive 'string' of the specified 'stringLength'.  Return 0
        // on success, and a non-zero value with no effect on 'level'
        // otherwise (i.e., 'string' does not match any 'Level' enumerator).

    static const char *toAscii(Severity::Level value);
        // Return the string representation exactly matching the enumerator
        // name corresponding to the specified enumeration 'value'.

    static bsl::ostream& streamOut(bsl::ostream&   stream,
                                   Severity::Level value);
        // Format the specified 'value' to the specified output 'stream' and
        // return a reference to the modifiable 'stream'.

};

// FREE OPERATORS
bsl::ostream& operator<<(bsl::ostream& stream, Severity::Level rhs);
    // Format the specified 'rhs' to the specified output 'stream' and return a
    // reference to the modifiable 'stream'.

// ============================================================================
//                              INLINE DEFINITIONS
// ============================================================================

                        // ---------------
                        // struct Severity
                        // ---------------

// CLASS METHODS
inline
bsl::ostream& Severity::streamOut(bsl::ostream&   stream,
                                  Severity::Level value)
{
    print(stream, value);
    return stream;
}

}  // close package namespace

// FREE OPERATORS
inline
bsl::ostream& ball::operator<<(bsl::ostream& stream, Severity::Level rhs)
{
    return Severity::streamOut(stream, rhs);
}

}  // close enterprise namespace

#if defined(MSVC_REQUEST_POP_MACRO_DEBUG)
#pragma pop_macro("DEBUG")
#undef MSVC_REQUEST_POP_MACRO_DEBUG
#endif

#if defined(MSVC_REQUEST_POP_MACRO_ERROR)
#pragma pop_macro("ERROR")
#undef MSVC_REQUEST_POP_MACRO_ERROR
#endif

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
