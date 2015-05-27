// bdlt_dayofweek.h                                                   -*-C++-*-
#ifndef INCLUDED_BDLT_DAYOFWEEK
#define INCLUDED_BDLT_DAYOFWEEK

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide an enumeration of the set of days of the week.
//
//@CLASSES:
//  bdlt::DayOfWeek: namespace for day-of-week enumeration (and its operations)
//
//@DESCRIPTION: This component provides a namespace for the 'enum' type
// 'bdlt::DayOfWeek::Enum', which enumerates the set of days of the week.
//
///Enumerators
///-----------
//..
//  Long Name      Short Name   Description
//  -----------    ----------   -----------
//  e_SUNDAY       e_SUN        Sunday
//  e_MONDAY       e_MON        Monday
//  e_TUESDAY      e_TUE        Tuesday
//  e_WEDNESDAY    e_WED        Wednesday
//  e_THURSDAY     e_THU        Thursday
//  e_FRIDAY       e_FRI        Friday
//  e_SATURDAY     e_SAT        Saturday
//..
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Basic Syntax
///- - - - - - - - - - - -
// The following snippets of code provide a simple illustration of
// 'bdlt::DayOfWeek' operation.
//
// First, create a variable, 'day', of type 'bdlt::DayOfWeek::Enum', and
// initialize it to the value 'bdlt::DayOfWeek::e_MONDAY':
//..
//  bdlt::DayOfWeek::Enum day = bdlt::DayOfWeek::e_MONDAY;
//..
// Next, store its representation in a variable, 'rep', of type 'const char *':
//..
//  const char *rep = bdlt::DayOfWeek::toAscii(day);
//  assert(0 == bsl::strcmp(rep, "MON"));
//..
// Finally, we print the value of 'day' to 'stdout':
//..
//  bsl::cout << day << bsl::endl;
//..
// This statement produces the following output on 'stdout':
//..
//  MON
//..

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

#ifndef INCLUDED_BSL_OSTREAM
#include <bsl_ostream.h>
#endif

namespace BloombergLP {
namespace bdlt {

                        // ===============
                        // class DayOfWeek
                        // ===============

struct DayOfWeek {
    // This 'struct' provides a namespace for enumerating day-of-week values.
    // See 'Enum' in the TYPES sub-section for details.
    //
    // This 'struct':
    //: o supports a complete set of *enumeration* operations
    // For terminology see 'bsldoc_glossary'.

  public:
    // TYPES
    enum Enum {
        // Enumerated day-of-week values.

        e_SUN = 1, e_SUNDAY    = e_SUN,
        e_MON,     e_MONDAY    = e_MON,
        e_TUE,     e_TUESDAY   = e_TUE,
        e_WED,     e_WEDNESDAY = e_WED,
        e_THU,     e_THURSDAY  = e_THU,
        e_FRI,     e_FRIDAY    = e_FRI,
        e_SAT,     e_SATURDAY  = e_SAT

    };


    // CLASS METHODS
    template <class STREAM>
    static STREAM& bdexStreamIn(STREAM&          stream,
                                DayOfWeek::Enum& variable,
                                int              version);
        // Assign to the specified 'variable' the value read from the specified
        // input 'stream' using the specified 'version' format, and return a
        // reference to 'stream'.  If 'stream' is initially invalid, this
        // operation has no effect.  If 'version' is not supported, 'variable'
        // is unaltered and 'stream' is invalidated, but otherwise unmodified.
        // If 'version' is supported but 'stream' becomes invalid during this
        // operation, 'variable' has an undefined, but valid, state.  Note that
        // no version is read from 'stream'.  See the 'bslx' package-level
        // documentation for more information on BDEX streaming of
        // value-semantic types and containers.

    template <class STREAM>
    static STREAM& bdexStreamOut(STREAM&         stream,
                                 DayOfWeek::Enum value,
                                 int             version);
        // Write the value of the specified 'value', using the specified
        // 'version' format, to the specified output 'stream', and return a
        // reference to 'stream'.  If 'stream' is initially invalid, this
        // operation has no effect.  If 'version' is not supported, 'stream' is
        // invalidated, but otherwise unmodified.  Note that 'version' is not
        // written to 'stream'.  See the 'bslx' package-level documentation for
        // more information on BDEX streaming of value-semantic types and
        // containers.

    static int maxSupportedBdexVersion(int versionSelector);
        // Return the maximum valid BDEX format version, as indicated by the
        // specified 'versionSelector', to be passed to the 'bdexStreamOut'
        // method.  Note that it is highly recommended that 'versionSelector'
        // be formatted as "YYYYMMDD", a date representation.  Also note that
        // 'versionSelector' should be a *compile*-time-chosen value that
        // selects a format version supported by both externalizer and
        // unexternalizer.  See the 'bslx' package-level documentation for more
        // information on BDEX streaming of value-semantic types and
        // containers.

    static bsl::ostream& print(bsl::ostream&          stream,
                               DayOfWeek::Enum        value,
                               int                    level          = 0,
                               int                    spacesPerLevel = 4);
        // Write the string representation of the specified enumeration 'value'
        // to the specified output 'stream', and return a reference to
        // 'stream'.  Optionally specify an initial indentation 'level', whose
        // absolute value is incremented recursively for nested objects.  If
        // 'level' is specified, optionally specify 'spacesPerLevel', whose
        // absolute value indicates the number of spaces per indentation level
        // for this and all of its nested objects.  If 'level' is negative,
        // suppress indentation of the first line.  If 'spacesPerLevel' is
        // negative, format the entire output on one line, suppressing all but
        // the initial indentation (as governed by 'level').  The behavior is
        // undefined unless 'dayOfWeek' is in the range '[ e_SUN .. e_SAT ]'.
        // See 'toAscii' for what constitutes the string representation of a
        // 'bdlt::DayOfWeek::Enum' value.

    static const char *toAscii(Enum dayOfWeek);
        // Return the abbreviated character-string representation of the
        // enumerator corresponding to the specified 'dayOfWeek'.  This
        // representation matches the enumerator's three-character name (e.g.,
        // 'e_SUN') with the "e_" prefix elided.  For example:
        //..
        //  bsl::cout << bdlt::DayOfWeek::toAscii(bdlt::DayOfWeek::e_SUN);
        //..
        // prints the following on standard output:
        //..
        //  SUN
        //..
        // Note that specifying a 'dayOfWeek' that does not match any of the
        // enumerators will result in an unspecified string representation
        // being returned that is distinct from the values returned for any
        // valid enumeration.

};

// FREE OPERATORS
bsl::ostream& operator<<(bsl::ostream& stream, DayOfWeek::Enum value);
    // Write the string representation of the specified enumeration 'value' to
    // the specified output 'stream' in a single-line format, and return a
    // reference to 'stream'.  See 'toAscii' for what constitutes the string
    // representation of a 'bdlt::DayOfWeek::Enum' value.  Note that this
    // method has the same behavior as
    //..
    //  bdlt::DayOfWeek::print(stream, value, 0, -1);
    //..

// FREE FUNCTIONS
template <class STREAM>
STREAM& bdexStreamIn(STREAM& stream, DayOfWeek::Enum& variable, int version);
    // Load into the specified 'variable' the 'DayOfWeek::Enum' value read from
    // the specified input 'stream' using the specified 'version' format, and
    // return a reference to 'stream'.  If 'stream' is initially invalid, this
    // operation has no effect.  If 'version' is not supported by 'DayOfWeek',
    // 'variable' is unaltered and 'stream' is invalidated, but otherwise
    // unmodified.  If 'version' is supported by 'DayOfWeek' but 'stream'
    // becomes invalid during this operation, 'variable' has an undefined, but
    // valid, state.  The behavior is undefined unless 'STREAM' is
    // BDEX-compliant.  Note that no version is read from 'stream'.  See the
    // 'bslx' package-level documentation for more information on BDEX
    // streaming of value-semantic types and containers.

template <class STREAM>
STREAM& bdexStreamOut(STREAM&                stream,
                      const DayOfWeek::Enum& value,
                      int                    version);
    // Write the specified 'value', using the specified 'version' format, to
    // the specified output 'stream', and return a reference to 'stream'.  If
    // 'stream' is initially invalid, this operation has no effect.  If
    // 'version' is not supported by 'DayOfWeek', 'stream' is invalidated, but
    // otherwise unmodified.  The behavior is undefined unless 'STREAM' is
    // BDEX-compliant.  Note that 'version' is not written to 'stream'.  See
    // the 'bslx' package-level documentation for more information on BDEX
    // streaming of value-semantic types and containers.

int maxSupportedBdexVersion(const DayOfWeek::Enum *, int versionSelector);
    // Return the maximum valid BDEX format version, as indicated by the
    // specified 'versionSelector', to be passed to the 'bdexStreamOut' method
    // while streaming an object of the type 'DayOfWeek::Enum'.  Note that it
    // is highly recommended that 'versionSelector' be formatted as "YYYYMMDD",
    // a date representation.  Also note that 'versionSelector' should be a
    // *compile*-time-chosen value that selects a format version supported by
    // both externalizer and unexternalizer.  See the 'bslx' package-level
    // documentation for more information on BDEX streaming of value-semantic
    // types and containers.

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

                              // ---------------
                              // class DayOfWeek
                              // ---------------

// CLASS METHODS
template <class STREAM>
STREAM& DayOfWeek::bdexStreamIn(STREAM&          stream,
                                DayOfWeek::Enum& variable,
                                int              version)
{
    if (stream) {
        switch (version) { // switch on the schema version
          case 1: {
            char newValue;
            stream.getInt8(newValue);
            if (stream && e_SUN <= newValue && e_SAT >= newValue) {
                variable = static_cast<DayOfWeek::Enum>(newValue);
            }
            else {
                stream.invalidate();
            }
          } break;
          default: {
            stream.invalidate();  // unrecognized version number
          }
        }
    }
    return stream;
}

template <class STREAM>
STREAM& DayOfWeek::bdexStreamOut(STREAM&         stream,
                                 DayOfWeek::Enum value,
                                 int             version)
{
    if (stream) {
        switch (version) { // switch on the schema version
          case 1: {
            stream.putInt8(static_cast<char>(value));
          } break;
          default: {
            stream.invalidate();  // unrecognized version number
          }
        }
    }
    return stream;
}

inline
int DayOfWeek::maxSupportedBdexVersion(int /* versionSelector */)
{
    return 1;
}


}  // close package namespace

// FREE OPERATORS
inline
bsl::ostream& bdlt::operator<<(bsl::ostream&         stream,
                               bdlt::DayOfWeek::Enum value)
{
    return bdlt::DayOfWeek::print(stream, value, 0, -1);
}

// FREE FUNCTIONS
template <class STREAM>
STREAM& bdlt::bdexStreamIn(STREAM&                stream,
                           bdlt::DayOfWeek::Enum& variable,
                           int                    version)
{
    return bdlt::DayOfWeek::bdexStreamIn(stream, variable, version);
}

template <class STREAM>
STREAM& bdlt::bdexStreamOut(STREAM&                      stream,
                            const bdlt::DayOfWeek::Enum& value,
                            int                          version)
{
    return bdlt::DayOfWeek::bdexStreamOut(stream, value, version);
}

inline
int bdlt::maxSupportedBdexVersion(const bdlt::DayOfWeek::Enum *,
                                  int                          versionSelector)
{
    return bdlt::DayOfWeek::maxSupportedBdexVersion(versionSelector);
}


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
