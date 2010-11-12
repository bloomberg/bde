// bdet_monthofyear.h                 -*-C++-*-
#ifndef INCLUDED_BDET_MONTHOFYEAR
#define INCLUDED_BDET_MONTHOFYEAR

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a namespace for representing month-of-year values.
//
//@CLASSES:
//   bdet_MonthOfYear: namespace for enumerated month-of-year values
//
//@AUTHOR: Tom Marshall (tmarshal)
//
//@DESCRIPTION: This component provides a namespace for enumerating each month
// of the year as well as providing a function that converts each of these
// enumerators to its corresponding string representation.  Functionality is
// also provided to write the string form directly to a standard 'ostream', and
// to externalize it to/from a 'bdex' stream.
//
///Usage
///-----
// The following snippets of code provide a simple illustration of
// 'bdet_MonthOfYear' operation; given the low-level nature of an 'enum'
// component an extended example is unnecessary.
//
// First, create a variable 'month' of type 'bdet_MonthOfYear::Month' and
// initialize it to the value 'bdet_MonthOfYear::APRIL'.
//..
//      bdet_MonthOfYear::Month month = bdet_MonthOfYear::APRIL;
//..
// Next, store its representation in a variable 'rep' of type 'const char*'.
//..
//      const char *rep = bdet_MonthOfYear::toAscii(month);
//      assert(0 == strcmp(rep, "APR"));
//..
// Finally, print the value of 'month' to 'cout'.
//..
//      bsl::cout << month << bsl::endl;
//..
// This statement produces the following output on 'stdout':
//..
//      APR
//..

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif

namespace BloombergLP {

                        // =======================
                        // struct bdet_MonthOfYear
                        // =======================

struct bdet_MonthOfYear {
    // This class provides a namespace for enumerating month-of-year values.

  public:
    // TYPES
    enum Month {
        BDET_JAN = 1, BDET_JANUARY        = BDET_JAN,
        BDET_FEB,     BDET_FEBRUARY       = BDET_FEB,
        BDET_MAR,     BDET_MARCH          = BDET_MAR,
        BDET_APR,     BDET_APRIL          = BDET_APR,
        BDET_MAY,
        BDET_JUN,     BDET_JUNE           = BDET_JUN,
        BDET_JUL,     BDET_JULY           = BDET_JUL,
        BDET_AUG,     BDET_AUGUST         = BDET_AUG,
        BDET_SEP,     BDET_SEPTEMBER      = BDET_SEP,
        BDET_OCT,     BDET_OCTOBER        = BDET_OCT,
        BDET_NOV,     BDET_NOVEMBER       = BDET_NOV,
        BDET_DEC,     BDET_DECEMBER       = BDET_DEC

#if !defined(BSL_LEGACY) || 1 == BSL_LEGACY
      , JAN = BDET_JAN
      , FEB = BDET_FEB
      , MAR = BDET_MAR
      , APR = BDET_APR
      , MAY = BDET_MAY
      , JUN = BDET_JUN
      , JUL = BDET_JUL
      , AUG = BDET_AUG
      , SEP = BDET_SEP
      , OCT = BDET_OCT
      , NOV = BDET_NOV
      , DEC = BDET_DEC

      , JANUARY   = BDET_JANUARY
      , FEBRUARY  = BDET_FEBRUARY
      , MARCH     = BDET_MARCH
      , APRIL     = BDET_APRIL
      , JUNE      = BDET_JUNE
      , JULY      = BDET_JULY
      , AUGUST    = BDET_AUGUST
      , SEPTEMBER = BDET_SEPTEMBER
      , OCTOBER   = BDET_OCTOBER
      , NOVEMBER  = BDET_NOVEMBER
      , DECEMBER  = BDET_DECEMBER
#endif
    };

    enum {
         BDET_LENGTH = BDET_DEC

#if !defined(BSL_LEGACY) || 1 == BSL_LEGACY
       , LENGTH      = BDET_LENGTH
#endif
    };
        // Define 'LENGTH' to be the number of consecutively valued enumerators
        // in the range '[ BDET_JAN .. BDET_DEC ]'.

    // CLASS METHODS
    static int maxSupportedBdexVersion();
        // Return the most current 'bdex' streaming version number supported by
        // this class.  See the 'bdex' package-level documentation for more
        // information on 'bdex' streaming of container types.

    static const char *toAscii(bdet_MonthOfYear::Month month);
        // Return the string representation of the enumerator corresponding to
        // the specified 'month'.  This representation corresponds exactly
        // to the enumerator's 3 character enumerator name (e.g., "JAN").

    template <class STREAM>
    static STREAM& bdexStreamIn(STREAM&                  stream,
                                bdet_MonthOfYear::Month& value,
                                int                      version);
        // Assign to the specified 'value' the value read from the specified
        // input 'stream' using the specified 'version' format and return a
        // reference to the modifiable 'stream'.  If 'stream' is initially
        // invalid, this operation has no effect.  If 'stream' becomes invalid
        // during this operation, the 'value' is valid, but its value is
        // undefined.  If the specified 'version' is not supported, 'stream' is
        // marked invalid, but 'value' is unaltered.  Note that no version is
        // read from 'stream'.  See the 'bdex' package-level documentation for
        // more information on 'bdex' streaming of container types.

    template <class STREAM>
    static STREAM& bdexStreamOut(STREAM&                 stream,
                                 bdet_MonthOfYear::Month value,
                                 int                     version);
        // Write the specified 'value' to the specified output 'stream' and
        // return a reference to the modifiable 'stream'.  Optionally specify
        // an explicit 'version' format; by default, the maximum supported
        // version is written to 'stream' and used as the format.  If 'version'
        // is specified, that format is used but *not* written to 'stream'.  If
        // 'version' is not supported, 'stream' is left unmodified.  See the
        // 'bdex' package-level documentation for more information on 'bdex'
        // streaming of container types.
};

// FREE OPERATORS
bsl::ostream& operator<<(bsl::ostream& stream, bdet_MonthOfYear::Month rhs);
    // Format to the specified output 'stream' the specified 'rhs' month in
    // a string representation matching the three-character enumerator name
    // (e.g., "JAN"), and return a reference to the modifiable 'stream'.

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

// CLASS METHODS
inline
int bdet_MonthOfYear::maxSupportedBdexVersion()
{
    return 1;
}

template <class STREAM>
STREAM& bdet_MonthOfYear::bdexStreamIn(STREAM&                  stream,
                                       bdet_MonthOfYear::Month& value,
                                       int                      version)
{
    if (stream) {
        switch(version) {
          case 1: {
            char newValue;
            stream.getInt8(newValue);
            if (stream) {
                if (1 <= newValue && newValue <= bdet_MonthOfYear::BDET_LENGTH) {
                    value = bdet_MonthOfYear::Month(newValue);
                }
                else {
                    stream.invalidate(); // Bad value in stream.
                }
            }
          } break;
          default: {
            stream.invalidate();         // Unrecognized version number.
          } break;
       }
    }
    return stream;
}

template <class STREAM>
inline
STREAM& bdet_MonthOfYear::bdexStreamOut(STREAM&                 stream,
                                        bdet_MonthOfYear::Month value,
                                        int)
{
    // TBD switch on version
    stream.putInt8((char)value);  // Write the value as a single byte.
    return stream;
}

// ============================================================================
//                     namespace bdex_InStreamFunctions
// ============================================================================

namespace bdex_InStreamFunctions {

template <typename STREAM>
inline
STREAM& streamIn(STREAM&                  stream,
                 bdet_MonthOfYear::Month& value,
                 int                      version)
{
    return bdet_MonthOfYear::bdexStreamIn(stream, value, version);
}

}  // close namespace bdex_InStreamFunctions;

// ============================================================================
//                      namespace bdex_VersionFunctions
// ============================================================================

namespace bdex_VersionFunctions {

inline
int maxSupportedVersion(bdet_MonthOfYear::Month)
{
    return bdet_MonthOfYear::maxSupportedBdexVersion();
}

}  // close namespace bdex_VersionFunctions;

// ============================================================================
//                     namespace bdex_OutStreamFunctions
// ============================================================================

namespace bdex_OutStreamFunctions {

template <typename STREAM>
inline
STREAM& streamOut(STREAM&                        stream,
                  const bdet_MonthOfYear::Month& value,
                  int                            version)
{
    return bdet_MonthOfYear::bdexStreamOut(stream, value, version);
}

}  // close namespace bdex_OutStreamFunctions

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2004
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
