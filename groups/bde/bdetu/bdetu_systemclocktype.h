// bdetu_systemclocktype.h                                            -*-C++-*-
#ifndef INCLUDED_BDETU_SYSTEMCLOCKTYPE
#define INCLUDED_BDETU_SYSTEMCLOCKTYPE

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Enumerate the set of sytem clock types.
//
//@CLASSES:
//  bdetu_SystemClockType: namespace for a clock type 'enum'
//
//@SEE_ALSO: bdetu_systemtime, bcemt_condition, bcemt_timedsemaphore
//
//@AUTHOR: Alexei Zakharov, Jeffrey Mendelsohn (jmendelsohn4)
//
//@DESCRIPTION: This component provides a namespace for the 'enum' type
// 'bdetu_SystemTimeClock::Enum', which enumerates the set of system clock
// types.  A 'bdetu_SystemTimeClock' is particularly important when providing
// time-out values to synchronization methods like 'timedWait' (e.g., see
// 'bcemt_condition') where those time-outs must be consistent in environments
// where the system clocks may be changed.
//
///Enumerators
///-----------
//..
//  Name          Description
//  -----------   -------------------------------------------------------------
//  e_REALTIME    System clock that returns the current wall time maintained by
//                the system, and which is affected by adjustments to the
//                system's clock (i.e., time values returned may jump
//                "forwards" and "backwards" as the current time-of-day is
//                changed on the system).
//
//  e_MONOTONIC   System clock that returns the elapsed time since some
//                unspecified starting point in the past.  The returned values
//                are monotonically non-decreasing, and are generally not
//                affected by changes to the system time.
//..
//
///USAGE
///-----
// This section illustrates intended use of this component.
//
///Example 1: Basic Syntax
///- - - - - - - - - - - -
// The following snippets of code provide a simple illustration of using
// 'bdetu_SystemClockType'.
//
// First, we create a variable 'value' of type 'bdetu_SystemClockType::Enum'
// and initialize it with the enumerator value
// 'bdetu_SystemClockType::e_MONOTONIC':
//..
//  bdetu_SystemClockType::Enum value = bdetu_SystemClockType::e_MONOTONIC;
//..
// Now, we store the address of its ASCII representation in a pointer variable,
// 'asciiValue', of type 'const char *':
//..
//  const char *asciiValue = bdetu_SystemClockTypetoAscii(value);
//  assert(0 == bsl::strcmp(asciiValue, "MONOTONIC"));
//..
// Finally, we print 'value' to 'bsl::cout'.
//..
//  bsl::cout << value << bsl::endl;
//..
// This statement produces the following output on 'stdout':
//..
//  MONOTONIC
//..

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif

namespace BloombergLP {

                        // ============================
                        // struct bdetu_SystemClockType
                        // ============================

struct bdetu_SystemClockType {
    // This 'struct' provides a namespace for enumerating the set of system
    // clock type for use in distinguishing which system clock to use for
    // measuring time.  See 'Enum' in the TYPES sub-section for details.
    //
    // This class:
    //: o supports a complete set of *enumeration* operations
    //:   o except for 'bdex' serialization
    //: o is 'const' *thread-safe*
    // For terminology see 'bsldoc_glossary'.

  public:
    // TYPES
    enum Enum {
        // Enumeration clock type values.

        e_REALTIME,   // System clock that returns the current wall time
                      // maintained by the system, and which is affected by
                      // adjustments to the system's clock (i.e., time values
                      // returned may jump "forwards" and "backwards" as the
                      // current time-of-day is changed on the system).

        e_MONOTONIC,  // System clock that returns the elapsed time since some
                      // unspecified starting point in the past.  The returned
                      // values are monotonically non-decreasing, and are
                      // generally not affected by changes to the system time.
    };

    // CLASS METHODS
    static bsl::ostream& print(bsl::ostream&               stream,
                               bdetu_SystemClockType::Enum value,
                               int                         level          = 0,
                               int                         spacesPerLevel = 4);
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
        // 'bdetu_SystemClockType::Enum' value.

    static const char *toAscii(bdetu_SystemClockType::Enum value);
        // Return the non-modifiable string representation corresponding to the
        // specified enumeration 'value', if it exists, and a unique (error)
        // string otherwise.  The string representation of 'value' matches its
        // corresponding enumerator name with the "e_" prefix elided.  For
        // example:
        //..
        //  bsl::cout << bdetu_SystemClockType::toAscii(
        //                                 bdetu_SystemClockType::e_MONOTONIC);
        //..
        // will print the following on standard output:
        //..
        //  MONOTONIC
        //..
        // Note that specifying a 'value' that does not match any of the
        // enumerators will result in a string representation that is distinct
        // from any of those corresponding to the enumerators, but is otherwise
        // unspecified.
};

// FREE OPERATORS
bsl::ostream& operator<<(bsl::ostream&               stream,
                         bdetu_SystemClockType::Enum value);
    // Write the string representation of the specified enumeration 'value' to
    // the specified output 'stream' in a single-line format, and return a
    // reference to 'stream'.  See 'toAscii' for what constitutes the string
    // representation of a 'bdetu_SystemClockType::Enum' value.  Note that this
    // method has the same behavior as
    //..
    //  bdetu_SystemClockType::print(stream, value, 0, -1);
    //..

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

                        // ----------------------------
                        // struct bdetu_SystemClockType
                        // ----------------------------

// FREE OPERATORS
inline
bsl::ostream& operator<<(bsl::ostream&               stream,
                         bdetu_SystemClockType::Enum value)
{
    return bdetu_SystemClockType::print(stream, value, 0, -1);
}

}  // close namespace BloombergLP

#endif

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2014
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ----------------------------------
