// bdet_dayofweek.h                                                   -*-C++-*-
#ifndef INCLUDED_BDET_DAYOFWEEK
#define INCLUDED_BDET_DAYOFWEEK

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide an enumeration of (and oprations on) day-of-week values.
//
//@CLASSES:
//  bdet_DayOfWeek: namespace for day-of-week enumeration (and its operations)
//
//@AUTHOR: John Lakos (jlakos)
//
//@DESCRIPTION: This component provides a 'struct', 'bdet_DayOfWeek', which
// serves as a namespace for enumerating the seven days of the week,
// '[ BDET_SUNDAY .. BDET_SATURDAY ]', along with a standard set of supporting
// operations.  These operations include converting an enumerated value to its
// corresponding string representation, writing the string form directly to a
// standard 'bsl::ostream', and reading from and writing to a 'bdex' stream.
//
///Usage
///-----
// The following snippets of code provide a simple illustration of using
// 'bdet_DayOfWeek'.  First we create a variable, 'day', of type
// 'bdet_DayOfWeek::Day', and initialize it to the value
// 'bdet_DayOfWeek::BDET_MONDAY':
//..
//  bdet_DayOfWeek::Day day = bdet_DayOfWeek::BDET_MONDAY;
//..
// Next we store its representation in a variable, 'rep', of type
// 'const char *':
//..
//  const char *rep = bdet_DayOfWeek::toAscii(day);
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

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BSL_OSTREAM
#include <bsl_ostream.h>
#endif

#ifndef BDE_DONT_ALLOW_TRANSITIVE_INCLUDES

#ifndef INCLUDED_BDEX_VERSIONFUNCTIONS
#include <bdex_versionfunctions.h>
#endif

#endif

namespace BloombergLP {

                        // ====================
                        // class bdet_DayOfWeek
                        // ====================

struct bdet_DayOfWeek {
    // This 'struct' provides a namespace for enumerating day-of-week values
    // as well as standard operations thereon, which include conversion to a
    // corresponding character string representation, writing the string form
    // directly to a standard 'ostream', and reading from and writing to a
    // 'bdex' stream.  Note that both a short and long enumerator name are
    // provided for each supported value; however, when rendered in string
    // form, the short name (with its "BDET_" prefix omitted) is used.

  public:
    // TYPES
    enum Day {
        // Enumerated day-of-week values.

        BDET_SUN = 1, BDET_SUNDAY    = BDET_SUN,
        BDET_MON = 2, BDET_MONDAY    = BDET_MON,
        BDET_TUE = 3, BDET_TUESDAY   = BDET_TUE,
        BDET_WED = 4, BDET_WEDNESDAY = BDET_WED,
        BDET_THU = 5, BDET_THURSDAY  = BDET_THU,
        BDET_FRI = 6, BDET_FRIDAY    = BDET_FRI,
        BDET_SAT = 7, BDET_SATURDAY  = BDET_SAT

#if !defined(BSL_LEGACY) || 1 == BSL_LEGACY
      , SUN       = BDET_SUN
      , MON       = BDET_MON
      , TUE       = BDET_TUE
      , WED       = BDET_WED
      , THU       = BDET_THU
      , FRI       = BDET_FRI
      , SAT       = BDET_SAT
      , SUNDAY    = BDET_SUNDAY
      , MONDAY    = BDET_MONDAY
      , TUESDAY   = BDET_TUESDAY
      , WEDNESDAY = BDET_WEDNESDAY
      , THURSDAY  = BDET_THURSDAY
      , FRIDAY    = BDET_FRIDAY
      , SATURDAY  = BDET_SATURDAY
#endif
    };

    // CLASS METHODS
    static int maxSupportedBdexVersion();
        // Return the most current 'bdex' streaming version number supported by
        // this class.  (See the package-group-level documentation for more
        // information on 'bdex' streaming of container types.)

    static const char *toAscii(Day dayOfWeek);
        // Return the abbreviated character-string representation of the
        // enumerator corresponding to the specified 'dayOfWeek'.  This
        // representation matches the enumerator's eight-character name (e.g.,
        // 'BDET_SUN') with the "BDET_" prefix elided.  For example:
        //..
        //  bsl::cout << bdet_DayOfWeek::toAscii(bdet_DayOfWeek::BDET_SUN);
        //..
        // prints the following on standard output:
        //..
        //  SUN
        //..
        // Note that the string representation of 'BDET_SUNDAY' is the same as
        // that for 'BDET_SUN' (and similarly for the other days of the week).

    template <class STREAM>
    static STREAM& bdexStreamIn(STREAM&              stream,
                                bdet_DayOfWeek::Day& dayOfWeek,
                                int                  version);
        // Assign, to the specified 'dayOfWeek', the value read from the
        // specified input 'stream' using the specified 'version' format, and
        // return a reference to the modifiable 'stream'.  If 'stream' is
        // initially invalid, this operation has no effect.  If 'stream'
        // becomes invalid during this operation, 'dayOfWeek' is valid, but its
        // value is undefined.  If 'version' is not supported, 'stream' is
        // marked invalid and 'dayOfWeek' is unaltered.  Note that no version
        // is read from 'stream'.  (See the 'bdex' package-level documentation
        // for more information on 'bdex' streaming of value-semantic types and
        // containers.)

    template <class STREAM>
    static STREAM& bdexStreamOut(STREAM&             stream,
                                 bdet_DayOfWeek::Day dayOfWeek,
                                 int                 version);
        // Write the specified 'dayOfWeek' value to the specified output
        // 'stream' using the specified 'version' format, and return a
        // reference to the modifiable 'stream'.  If 'stream' is initially
        // invalid, this operation has no effect.  If 'version' is not
        // supported, 'stream' is invalidated.  Note that in no event is
        // 'version' written to 'stream'.  (See the 'bdex' package-level
        // documentation for more information on 'bdex' streaming of
        // value-semantic types and containers.)
};

// FREE OPERATORS
bsl::ostream& operator<<(bsl::ostream& lhs, bdet_DayOfWeek::Day rhs);
    // Format, to the specified 'lhs' output stream, the abbreviated
    // character-string representation corresponding to the specified 'rhs'
    // day-of-week value, and return a reference to the modifiable 'lhs'
    // stream.  This representation matches the enumerator's eight-character
    // name (e.g., 'BDET_SUN') with the "BDET_" prefix elided.  For example:
    //..
    //  bsl::cout << bdet_DayOfWeek::BDET_SUN;
    //..
    // prints the following on standard output:
    //..
    //  SUN
    //..
    // Note that the string representation of 'BDET_SUNDAY' is the same as that
    // for 'BDET_SUN' (and similarly for the other days of the week).

// ===========================================================================
//                      INLINE FUNCTION DEFINITIONS
// ===========================================================================

                        // --------------------
                        // class bdet_DayOfWeek
                        // --------------------

// CLASS METHODS
inline
int bdet_DayOfWeek::maxSupportedBdexVersion()
{
    return 1;
}

template <class STREAM>
STREAM& bdet_DayOfWeek::bdexStreamIn(STREAM&              stream,
                                     bdet_DayOfWeek::Day& dayOfWeek,
                                     int                  version)
{
    if (stream) {
        switch(version) {
          case 1: {
            char newValue;
            stream.getInt8(newValue);     // get the value as a single byte
            if (stream) {
                if (newValue >= BDET_SUN && newValue <= BDET_SAT) {
                    dayOfWeek = bdet_DayOfWeek::Day(newValue);
                }
                else {
                    stream.invalidate();  // bad value in stream
                }
            }
          } break;
          default: {
            stream.invalidate();          // unrecognized version number
          } break;
       }
    }
    return stream;
}

template <class STREAM>
STREAM& bdet_DayOfWeek::bdexStreamOut(STREAM&             stream,
                                      bdet_DayOfWeek::Day dayOfWeek,
                                      int                 version)
{
    if (stream) {
        switch (version) {
          case 1: {

            // Write the value as one byte.

            stream.putInt8(static_cast<char>(dayOfWeek));

          } break;
          default: {
            stream.invalidate();          // unrecognized version number
          } break;
        }
    }
    return stream;
}

// FREE OPERATORS
inline
bsl::ostream& operator<<(bsl::ostream& lhs, bdet_DayOfWeek::Day rhs)
{
    return lhs << bdet_DayOfWeek::toAscii(rhs);
}

                     // --------------------------------
                     // namespace bdex_InStreamFunctions
                     // --------------------------------

namespace bdex_InStreamFunctions {

template <typename STREAM>
inline
STREAM& streamIn(STREAM&              stream,
                 bdet_DayOfWeek::Day& value,
                 int                  version)
{
    return bdet_DayOfWeek::bdexStreamIn(stream, value, version);
}

}  // close namespace bdex_InStreamFunctions

                     // -------------------------------
                     // namespace bdex_VersionFunctions
                     // -------------------------------

namespace bdex_VersionFunctions {

inline
int maxSupportedVersion(bdet_DayOfWeek::Day)
{
    return bdet_DayOfWeek::maxSupportedBdexVersion();
}

}  // close namespace bdex_VersionFunctions

                     // ---------------------------------
                     // namespace bdex_OutStreamFunctions
                     // ---------------------------------

namespace bdex_OutStreamFunctions {

template <typename STREAM>
inline
STREAM& streamOut(STREAM&                    stream,
                  const bdet_DayOfWeek::Day& value,
                  int                        version)
{
    return bdet_DayOfWeek::bdexStreamOut(stream, value, version);
}

}  // close namespace bdex_OutStreamFunctions

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2010
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
