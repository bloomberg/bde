// baetzo_dstpolicy.h                                                 -*-C++-*-
#ifndef INCLUDED_BAETZO_DSTPOLICY
#define INCLUDED_BAETZO_DSTPOLICY

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Enumerate the set of daylight-saving time (DST) policy values.
//
//@CLASSES:
//  baetzo_DstPolicy: namespace for a daylight-saving time policy 'enum'
//
//@SEE_ALSO: baetzo_localtimevalidity, baetzo_timezoneutil
//
//@AUTHOR: Stefano Pacifico (spacifico1), Henry Verschell (hverschell)
//
//@DESCRIPTION: This component provides a namespace for the 'enum' type
// 'baetzo_DstPolicy::Enum', which enumerates the set of policies for
// interpreting whether an associated local time is a daylight-saving time
// value.  A 'baetzo_DstPolicy' is particularly important when interpreting a
// local time that is not associated with a UTC offset (e.g., a 'bdet_Datetime'
// object), as such a representation may be ambiguous or invalid (see
// 'baetzo_localtimevalidity').
//
///Enumerators
///-----------
//..
//  Name                 Description
//  ------------------   ---------------------------------------------------
//  BAETZO_DST           Local time is interpreted as daylight-saving time.
//
//  BAETZO_STANDARD      Local time is interpreted as standard time.
//
//  BAETZO_UNSPECIFIED   Local time is interpreted as either daylight-saving
//                       time or standard time (as appropriate).
//..
//
///Usage
///-----
// In this section we show intended usage of this component.
//
///Example 1: Basic Syntax
///- - - - - - - - - - - -
// The following snippets of code provide a simple illustration of
// 'baetzo_DstPolicy' usage.
//
// First, we create a variable 'value' of type 'baetzo_DstPolicy::Enum' and
// initialize it with the enumerator value 'baetzo_DstPolicy::BAETZO_STANDARD':
//..
//  baetzo_DstPolicy::Enum value = baetzo_DstPolicy::BAETZO_STANDARD;
//..
// Next, we store a pointer to its ASCII representation in a variable
// 'asciiValue' of type 'const char *':
//..
//  const char *asciiValue = baetzo_DstPolicy::toAscii(value);
//  assert(0 == bsl::strcmp(asciiValue, "STANDARD"));
//..
// Finally, we print 'value' to 'bsl::cout'.
//..
//  bsl::cout << value << bsl::endl;
//..
// This statement produces the following output on 'stdout':
//..
//  STANDARD
//..

#ifndef INCLUDED_BAESCM_VERSION
#include <baescm_version.h>
#endif

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif

namespace BloombergLP {

                         // =======================
                         // struct baetzo_DstPolicy
                         // =======================

struct baetzo_DstPolicy {
    // This 'struct' provides a namespace for enumerating the set of policies
    // for interpreting whether a local time is a daylight-saving time.  See
    // 'Enum' in the TYPES sub-section for details.
    //
    // This class:
    //: o supports a complete set of *enumeration* operations
    //:   o except for 'bdex' serialization
    //: o is 'const' *thread-safe*
    // For terminology see 'bsldoc_glossary'.

  public:
    // TYPES
    enum Enum {
        BAETZO_DST,         // Local time is interpreted as daylight-saving
                            // time.

        BAETZO_STANDARD,    // Local time is interpreted as standard time.

        BAETZO_UNSPECIFIED  // Local time is interpreted as either
                            // daylight-saving time or standard time (as
                            // appropriate).
    };

  public:
    // CLASS METHODS
    static bsl::ostream& print(
                            bsl::ostream&          stream,
                            baetzo_DstPolicy::Enum value,
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
        // the initial indentation (as governed by 'level').  If 'stream' is
        // not valid on entry, this operation has no effect.  See 'toAscii' for
        // what constitutes the string representation of a
        // 'baetzo_DstPolicy::Enum' value.

    static const char *toAscii(baetzo_DstPolicy::Enum value);
        // Return the non-modifiable string representation corresponding to the
        // specified enumeration 'value'.  The string representation of 'value'
        // matches its corresponding enumerator name with the "BAETZO_" prefix
        // elided.  For example:
        //..
        //  bsl::cout << baetzo_DstPolicy::toAscii(
        //                                  baetzo_DstPolicy::BAETZO_STANDARD);
        //..
        // will print the following on standard output:
        //..
        //  STANDARD
        //..
        // Note that specifying a 'value' that does not match any of the
        // enumerators will result in a string representation that is distinct
        // from any of those corresponding to the enumerators, but is otherwise
        // unspecified.
};

// FREE OPERATORS
bsl::ostream& operator<<(bsl::ostream& stream, baetzo_DstPolicy::Enum value);
    // Write the string representation of the specified enumeration 'value' to
    // the specified output 'stream' in a single-line format, and return a
    // reference to 'stream'.  If 'stream' is not valid on entry, this
    // operation has no effect.  See 'toAscii' for what constitutes the string
    // representation of a 'baetzo_DstPolicy::Enum' value.  Note that this
    // method has the same behavior as
    //..
    //  baetzo_DstPolicy::print(stream, value, 0, -1);
    //..

// ===========================================================================
//                      INLINE FUNCTION DEFINITIONS
// ===========================================================================

                        // -----------------------
                        // struct baetzo_DstPolicy
                        // -----------------------

// FREE OPERATORS
inline
bsl::ostream& operator<<(bsl::ostream& stream, baetzo_DstPolicy::Enum value)
{
    return baetzo_DstPolicy::print(stream, value, 0, -1);
}

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2011
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
