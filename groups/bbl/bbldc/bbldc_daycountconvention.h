// bbldc_daycountconvention.h                                         -*-C++-*-
#ifndef INCLUDED_BBLDC_DAYCOUNTCONVENTION
#define INCLUDED_BBLDC_DAYCOUNTCONVENTION

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide an enumeration of 'bbldc' day-count conventions.
//
//@CLASSES:
//  bbldc::DayCountConvention: namespace for day-count convention enumeration
//
//@DESCRIPTION: This component provides a namespace,
// 'bbldc::DayCountConvention', for the 'enum' type
// 'bbldc::DayCountConvention::Enum', that enumerates the list of day-count
// conventions supported by the 'bbldc' package.
//
///Enumerators
///-----------
//..
//  Name                         Description
//  ---------------------------  ----------------------------------------------
//  e_ACTUAL_360                 Convention is Actual/360.
//  e_ACTUAL_365_FIXED           Convention is Actual/365 (fixed).
//  e_ISDA_ACTUAL_ACTUAL         Convention is ISDA Actual/Actual.
//  e_ISMA_30_360                Convention is ISMA 30/360.
//  e_PSA_30_360_EOM             Convention is PSA 30/360 end-of-month.
//  e_SIA_30_360_EOM             Convention is SIA 30/360 end-of-month.
//  e_SIA_30_360_NEOM            Convention is SIA 30/360 no-end-of-month.
//  e_PERIOD_ICMA_ACTUAL_ACTUAL  Convention is period-based ICMA Actual/Actual.
//  e_CALENDAR_BUS_252           Convention is calendar-based BUS-252.
//..
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Basic Syntax
///- - - - - - - - - - - -
// The following snippets of code provide a simple illustration of using
// 'bbldc::DayCountConvention'.
//
// First, we create a variable 'convention' of type
// 'bbldc::DayCountConvention::Enum' and initialize it to the value
// 'bbldc::DayCountConvention::e_ISMA_30_360':
//..
//  bbldc::DayCountConvention::Enum convention =
//                                    bbldc::DayCountConvention::e_ISMA_30_360;
//..
// Now, we store the address of its ASCII representation in a pointer variable,
// 'asciiValue', of type 'const char *':
//..
//  const char *asciiValue = bbldc::DayCountConvention::toAscii(convention);
//  assert(0 == bsl::strcmp(asciiValue, "ISMA_30_360"));
//..
// Finally, we print 'convention' to 'bsl::cout';
//..
//  bsl::cout << convention << bsl::endl;
//..
// This statement produces the following output on 'stdout':
//..
//  ISMA_30_360
//..

#ifndef INCLUDED_BBLSCM_VERSION
#include <bblscm_version.h>
#endif

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif

namespace BloombergLP {
namespace bbldc {

                        // =========================
                        // struct DayCountConvention
                        // =========================

struct DayCountConvention {
    // This 'struct' provides a namespace for enumerating the set of day-count
    // conventions supported within 'bbldc'.  See 'Enum' in the 'TYPES'
    // sub-section for details.
    //
    // This 'struct':
    //: o supports a complete set of *enumeration* operations
    // For terminology see 'bsldoc_glossary'.

    // TYPES
    enum Enum {
        // Enumerated 'bbldc' day-count conventions.

        e_ACTUAL_360                =  0,
        e_ACTUAL_365_FIXED          =  1,
        // 2 is deprecated
        e_ISDA_ACTUAL_ACTUAL        =  3,
        // 4 is deprecated
        e_ISMA_30_360               =  5,
        e_PSA_30_360_EOM            =  6,
        e_SIA_30_360_EOM            =  7,
        e_SIA_30_360_NEOM           =  8,
        e_PERIOD_ICMA_ACTUAL_ACTUAL =  9,
        e_CALENDAR_BUS_252          = 10
    };

    // CLASS METHODS
    template <class STREAM>
    static STREAM& bdexStreamIn(STREAM&                   stream,
                                DayCountConvention::Enum& variable,
                                int                       version);
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
    static STREAM& bdexStreamOut(STREAM&                  stream,
                                 DayCountConvention::Enum value,
                                 int                      version);
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

    static bsl::ostream& print(bsl::ostream&            stream,
                               DayCountConvention::Enum value,
                               int                      level          = 0,
                               int                      spacesPerLevel = 4);
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
        // undefined unless 'value' is in the range
        // '[e_ACTUAL_360 .. e_CALENDAR_BUS_252]'.  See 'toAscii' for what
        // constitutes the string representation of a
        // 'bbldc::DayCountConvention::Enum' value.

    static const char *toAscii(Enum convention);
        // Return the abbreviated character-string representation of the
        // enumerator corresponding to the specified 'convention'.  This
        // representation matches the enumerator's name (e.g., 'e_ACTUAL_360')
        // with the "e_" prefix elided.  For example:
        //..
        //  bsl::cout << bbldc::DayCountConvention::toAscii(
        //                            bbldc::DayCountConvention::e_ACTUAL_360);
        //..
        // prints the following on standard output:
        //..
        //  ACTUAL_360
        //..
        // Note that specifying a 'convention' that does not match any of the
        // enumerators will result in an unspecified string representation
        // being returned that is distinct from the values returned for any
        // valid enumerator.
};

// FREE OPERATORS
bsl::ostream& operator<<(bsl::ostream&            stream,
                         DayCountConvention::Enum value);
    // Write the string representation of the specified enumeration 'value' to
    // the specified output 'stream' in a single-line format, and return a
    // reference to 'stream'.  See 'toAscii' for what constitutes the string
    // representation of a 'bbldc::DayCountConvention::Enum' value.  Note that
    // this method has the same behavior as
    //..
    //  bbldc::DayCountConvention::print(stream, value, 0, -1);
    //..

// FREE FUNCTIONS
template <class STREAM>
STREAM& bdexStreamIn(STREAM&                   stream,
                     DayCountConvention::Enum& variable,
                     int                       version);
    // Load into the specified 'variable' the 'DayCountConvention::Enum' value
    // read from the specified input 'stream' using the specified 'version'
    // format, and return a reference to 'stream'.  If 'stream' is initially
    // invalid, this operation has no effect.  If 'version' is not supported by
    // 'DayCountConvention', 'variable' is unaltered and 'stream' is
    // invalidated, but otherwise unmodified.  If 'version' is supported by
    // 'DayCountConvention' but 'stream' becomes invalid during this operation,
    // 'variable' has an undefined, but valid, state.  The behavior is
    // undefined unless 'STREAM' is BDEX-compliant.  Note that no version is
    // read from 'stream'.  See the 'bslx' package-level documentation for more
    // information on BDEX streaming of value-semantic types and containers.

template <class STREAM>
STREAM& bdexStreamOut(STREAM&                         stream,
                      const DayCountConvention::Enum& value,
                      int                             version);
    // Write the specified 'value', using the specified 'version' format, to
    // the specified output 'stream', and return a reference to 'stream'.  If
    // 'stream' is initially invalid, this operation has no effect.  If
    // 'version' is not supported by 'DayCountConvention', 'stream' is
    // invalidated, but otherwise unmodified.  The behavior is undefined unless
    // 'STREAM' is BDEX-compliant.  Note that 'version' is not written to
    // 'stream'.  See the 'bslx' package-level documentation for more
    // information on BDEX streaming of value-semantic types and containers.

int maxSupportedBdexVersion(const DayCountConvention::Enum *,
                            int                             versionSelector);
    // Return the maximum valid BDEX format version, as indicated by the
    // specified 'versionSelector', to be passed to the 'bdexStreamOut' method
    // while streaming an object of the type 'DayCountConvention::Enum'.  Note
    // that it is highly recommended that 'versionSelector' be formatted as
    // "YYYYMMDD", a date representation.  Also note that 'versionSelector'
    // should be a *compile*-time-chosen value that selects a format version
    // supported by both externalizer and unexternalizer.  See the 'bslx'
    // package-level documentation for more information on BDEX streaming of
    // value-semantic types and containers.

// ============================================================================
//                             INLINE DEFINITIONS
// ============================================================================

                        // -------------------------
                        // struct DayCountConvention
                        // -------------------------

// CLASS METHODS
template <class STREAM>
STREAM& DayCountConvention::bdexStreamIn(STREAM&                   stream,
                                         DayCountConvention::Enum& variable,
                                         int                       version)
{
    if (stream) {
        switch (version) { // switch on the schema version
          case 1: {
            char newValue;
            stream.getInt8(newValue);
            if (   stream
                && e_ACTUAL_360       <= newValue
                && e_CALENDAR_BUS_252 >= newValue) {
                variable = static_cast<DayCountConvention::Enum>(newValue);
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
STREAM& DayCountConvention::bdexStreamOut(STREAM&                  stream,
                                          DayCountConvention::Enum value,
                                          int                      version)
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
int DayCountConvention::maxSupportedBdexVersion(int /* versionSelector */)
{
    return 1;
}

}  // close package namespace

// FREE OPERATORS
inline
bsl::ostream& bbldc::operator<<(bsl::ostream&                   stream,
                                bbldc::DayCountConvention::Enum value)
{
    return bbldc::DayCountConvention::print(stream, value, 0, -1);
}

// FREE FUNCTIONS
template <class STREAM>
STREAM& bbldc::bdexStreamIn(STREAM&                          stream,
                            bbldc::DayCountConvention::Enum& variable,
                            int                              version)
{
    return bbldc::DayCountConvention::bdexStreamIn(stream, variable, version);
}

template <class STREAM>
STREAM& bbldc::bdexStreamOut(STREAM&                                stream,
                             const bbldc::DayCountConvention::Enum& value,
                             int                                    version)
{
    return bbldc::DayCountConvention::bdexStreamOut(stream, value, version);
}

inline
int bbldc::maxSupportedBdexVersion(
                        const bbldc::DayCountConvention::Enum *,
                        int                                    versionSelector)
{
    return bbldc::DayCountConvention::maxSupportedBdexVersion(versionSelector);
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
