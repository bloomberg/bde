// bdlt_monthofyear.h                                                 -*-C++-*-
#ifndef INCLUDED_BDLT_MONTHOFYEAR
#define INCLUDED_BDLT_MONTHOFYEAR

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Enumerate the set of month-of-year values.
//
//@CLASSES:
//  bdlt::MonthOfYear: namespace for month-of-year 'enum'
//
//@DESCRIPTION: This component provides a namespace for the 'enum' type,
// 'bdlt::MonthOfYear::Enum', that enumerates the set of month-of-year values.
//
///Enumerators
///-----------
//..
//  Name                           Description
//  -------------------            ----------------------------------
//  e_JANUARY,   e_JAN             Enumerators representing January
//  e_FEBRUARY,  e_FEB             Enumerators representing February
//  e_MARCH,     e_MAR             Enumerators representing March
//  e_APRIL,     e_APR             Enumerators representing April
//  e_MAY                          Enumerator  representing May
//  e_JUNE,      e_JUN             Enumerators representing June
//  e_JULY,      e_JUL             Enumerators representing July
//  e_AUGUST,    e_AUG             Enumerators representing August
//  e_SEPTEMBER, e_SEP             Enumerators representing September
//  e_OCTOBER,   e_OCT             Enumerators representing October
//  e_NOVEMBER,  e_NOV             Enumerators representing November
//  e_DECEMBER,  e_DEC             Enumerators representing December
//
//  k_NUM_MONTHS                   Number of enumerators in the range
//                                 '[ e_JAN .. e_DEC ]'.
//..
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Basic Syntax
///- - - - - - - - - - - -
// The following snippets of code provide a simple illustration of
// 'bdlt::MonthOfYear' usage.
//
// First, we create a variable 'value' of type 'bdlt::MonthOfYear::Enum' and
// initialize it to the value 'bdlt::MonthOfYear::e_APRIL':
//..
//  bdlt::MonthOfYear::Enum value = bdlt::MonthOfYear::e_APRIL;
//..
// Next, we store a pointer to its ASCII representation in a variable
// 'asciiValue' of type 'const char *':
//..
//  const char *asciiValue = bdlt::MonthOfYear::toAscii(value);
//  assert(0 == bsl::strcmp(asciiValue, "APR"));
//..
// Finally, we print the value to 'bsl::cout':
//..
//  bsl::cout << value << bsl::endl;
//..
// This statement produces the following output on 'stdout':
//..
//  APR
//..

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif

namespace BloombergLP {
namespace bdlt {

                        // ==================
                        // struct MonthOfYear
                        // ==================

struct MonthOfYear {
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

        e_JAN = 1, e_JANUARY   = e_JAN,
        e_FEB,     e_FEBRUARY  = e_FEB,
        e_MAR,     e_MARCH     = e_MAR,
        e_APR,     e_APRIL     = e_APR,
        e_MAY,
        e_JUN,     e_JUNE      = e_JUN,
        e_JUL,     e_JULY      = e_JUL,
        e_AUG,     e_AUGUST    = e_AUG,
        e_SEP,     e_SEPTEMBER = e_SEP,
        e_OCT,     e_OCTOBER   = e_OCT,
        e_NOV,     e_NOVEMBER  = e_NOV,
        e_DEC,     e_DECEMBER  = e_DEC


    };

    enum {
        // Define 'k_NUM_MONTHS' to be the number of consecutively valued
        // enumerators in the range '[ e_JAN .. e_DEC ]'.

        k_NUM_MONTHS = e_DEC

    };


    // CLASS METHODS
    template <class STREAM>
    static STREAM& bdexStreamIn(STREAM&            stream,
                                MonthOfYear::Enum& variable,
                                int                version);
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
    static STREAM& bdexStreamOut(STREAM&           stream,
                                 MonthOfYear::Enum value,
                                 int               version);
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

    static bsl::ostream& print(bsl::ostream&     stream,
                               MonthOfYear::Enum value,
                               int               level          = 0,
                               int               spacesPerLevel = 4);
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
        // what constitutes the string representation of a 'MonthOfYear::Enum'
        // value.

    static const char *toAscii(MonthOfYear::Enum value);
        // Return the non-modifiable string representation corresponding to the
        // specified enumeration 'value', if it exists, and a unique (error)
        // string otherwise.  The string representation of 'value' matches the
        // first 3 characters of its corresponding enumerator name with the
        // "e_" prefix elided.  For example:
        //..
        //  bsl::cout << bdlt::MonthOfYear::toAscii(
        //                                       bdlt::MonthOfYear::e_JANUARY);
        //..
        // will print the following on standard output:
        //..
        //  JAN
        //..
        // Note that specifying a 'value' that does not match any of the
        // enumerators will result in a string representation that is distinct
        // from any of those corresponding to the enumerators, but is otherwise
        // unspecified.

};

// FREE OPERATORS
bsl::ostream& operator<<(bsl::ostream& stream, MonthOfYear::Enum value);
    // Write the string representation of the specified enumeration 'value' to
    // the specified output 'stream' in a single-line format, and return a
    // reference to 'stream'.  See 'toAscii' for what constitutes the string
    // representation of a 'bdlt::MonthOfYear::Enum' value.  Note that this
    // method has the same behavior as
    //..
    //  bdlt::MonthOfYear::print(stream, value, 0, -1);
    //..

// FREE FUNCTIONS
template <class STREAM>
STREAM& bdexStreamIn(STREAM& stream, MonthOfYear::Enum& variable, int version);
    // Load into the specified 'variable' the 'MonthOfYear::Enum' value read
    // from the specified input 'stream' using the specified 'version' format,
    // and return a reference to 'stream'.  If 'stream' is initially invalid,
    // this operation has no effect.  If 'version' is not supported by
    // 'MonthOfYear', 'variable' is unaltered and 'stream' is invalidated, but
    // otherwise unmodified.  If 'version' is supported by 'MonthOfYear' but
    // 'stream' becomes invalid during this operation, 'variable' has an
    // undefined, but valid, state.  The behavior is undefined unless 'STREAM'
    // is BDEX-compliant.  Note that no version is read from 'stream'.  See the
    // 'bslx' package-level documentation for more information on BDEX
    // streaming of value-semantic types and containers.

template <class STREAM>
STREAM& bdexStreamOut(STREAM&                stream,
                      const MonthOfYear::Enum& value,
                      int                    version);
    // Write the specified 'value', using the specified 'version' format, to
    // the specified output 'stream', and return a reference to 'stream'.  If
    // 'stream' is initially invalid, this operation has no effect.  If
    // 'version' is not supported by 'MonthOfYear', 'stream' is invalidated,
    // but otherwise unmodified.  The behavior is undefined unless 'STREAM' is
    // BDEX-compliant.  Note that 'version' is not written to 'stream'.  See
    // the 'bslx' package-level documentation for more information on BDEX
    // streaming of value-semantic types and containers.

int maxSupportedBdexVersion(const MonthOfYear::Enum *, int versionSelector);
    // Return the maximum valid BDEX format version, as indicated by the
    // specified 'versionSelector', to be passed to the 'bdexStreamOut' method
    // while streaming an object of the type 'MonthOfYear::Enum'.  Note that it
    // is highly recommended that 'versionSelector' be formatted as "YYYYMMDD",
    // a date representation.  Also note that 'versionSelector' should be a
    // *compile*-time-chosen value that selects a format version supported by
    // both externalizer and unexternalizer.  See the 'bslx' package-level
    // documentation for more information on BDEX streaming of value-semantic
    // types and containers.

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

                             // ------------------
                             // struct MonthOfYear
                             // ------------------

// CLASS METHODS
template <class STREAM>
STREAM& MonthOfYear::bdexStreamIn(STREAM&            stream,
                                  MonthOfYear::Enum& variable,
                                  int                version)
{
    if (stream) {
        switch (version) { // switch on the schema version
          case 1: {
            char newValue;
            stream.getInt8(newValue);
            if (stream && e_JAN <= newValue && e_DEC >= newValue) {
                variable = static_cast<MonthOfYear::Enum>(newValue);
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
inline
STREAM& MonthOfYear::bdexStreamOut(STREAM&           stream,
                                   MonthOfYear::Enum value,
                                   int               version)
{
    if (stream) {
        switch (version) { // switch on the schema version
          case 1: {
            stream.putInt8(static_cast<char>(value));
          }
          default: {
            stream.invalidate();  // unrecognized version number
          }
        }
    }
    return stream;
}

inline
int MonthOfYear::maxSupportedBdexVersion(int /* versionSelector */)
{
    return 1;
}


}  // close package namespace

// FREE OPERATORS
inline
bsl::ostream& bdlt::operator<<(bsl::ostream&           stream,
                               bdlt::MonthOfYear::Enum value)
{
    return bdlt::MonthOfYear::print(stream, value, 0, -1);
}

// FREE FUNCTIONS
template <class STREAM>
STREAM& bdlt::bdexStreamIn(STREAM&                  stream,
                           bdlt::MonthOfYear::Enum& variable,
                           int                      version)
{
    return bdlt::MonthOfYear::bdexStreamIn(stream, variable, version);
}

template <class STREAM>
STREAM& bdlt::bdexStreamOut(STREAM&                        stream,
                            const bdlt::MonthOfYear::Enum& value,
                            int                            version)
{
    return bdlt::MonthOfYear::bdexStreamOut(stream, value, version);
}

inline
int bdlt::maxSupportedBdexVersion(const bdlt::MonthOfYear::Enum *,
                                  int                          versionSelector)
{
    return bdlt::MonthOfYear::maxSupportedBdexVersion(versionSelector);
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
