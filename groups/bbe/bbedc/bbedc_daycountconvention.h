// bbedc_daycountconvention.h                                         -*-C++-*-
#ifndef INCLUDED_BBEDC_DAYCOUNTCONVENTION
#define INCLUDED_BBEDC_DAYCOUNTCONVENTION

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT_RCSID(bbedc_daycountconvention_h,"$Id$ $CSID$")
BDES_IDENT_PRAGMA_ONCE

//@PURPOSE: Enumerate the set of day-count conventions.
//
//@CLASSES:
//   bbedc_DayCountConvention: namespace for enumerating day-count conventions
//
//@AUTHOR: John Lakos (jlakos)
//
//@DESCRIPTION: This component provides a namespace for the 'enum' type
// 'bbedc_DayCountConvention::Type'.  'Type' enumerates a list of day-count
// conventions.
//
///Usage
///-----
// The following snippets of code provide a simple illustration of ordinary
// 'bbedc_DayCountConvention' operation.
//
// First create a variable 'convention' of type
// 'bbedc_DayCountConvention::Type' and initialize it to the value
// 'bbedc_DayCountConvention::BBEDC_ISMA_30_360':
//..
//  bbedc_DayCountConvention::Type convention =
//                                 bbedc_DayCountConvention::BBEDC_ISMA_30_360;
//..
// Next, store a pointer to its ASCII representation in a variable 'ascii' of
// type 'const char *':
//..
//  const char *ascii = bbedc_DayCountConvention::toAscii(convention);
//  assert(0 == strcmp(ascii, "ISMA_30_360"));
//..
// Finally, print the value of 'convention' to 'bsl::cout';
//..
//  bsl::cout << convention << bsl::endl;
//..
// This statement produces the following output on 'stdout':
//..
//  ISMA_30_360
//..

#ifndef INCLUDED_BBESCM_VERSION
#include <bbescm_version.h>
#endif

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif

namespace BloombergLP {

class bdet_Date;

                     // ===============================
                     // struct bbedc_DayCountConvention
                     // ===============================

struct bbedc_DayCountConvention {
    // This utility provides a namespace for pure procedures determining values
    // based on dates according to an enumerated day-count convention.

    // TYPES
    enum Type {
        BBEDC_ACTUAL_360         = 0,
        BBEDC_ACTUAL_365_FIXED   = 1,
        BBEDC_ICMA_ACTUAL_ACTUAL = 2,
        BBEDC_ISDA_ACTUAL_ACTUAL = 3,
        BBEDC_ISMA_ACTUAL_ACTUAL = 4,
        BBEDC_ISMA_30_360        = 5,
        BBEDC_PSA_30_360_EOM     = 6,
        BBEDC_SIA_30_360_EOM     = 7,
        BBEDC_SIA_30_360_NEOM    = 8
#if !defined(BSL_LEGACY) || 1 == BSL_LEGACY
      , ACTUAL_360         = BBEDC_ACTUAL_360
      , ACTUAL_365_FIXED   = BBEDC_ACTUAL_365_FIXED
      , ICMA_ACTUAL_ACTUAL = BBEDC_ICMA_ACTUAL_ACTUAL
      , ISDA_ACTUAL_ACTUAL = BBEDC_ISDA_ACTUAL_ACTUAL
      , ISMA_ACTUAL_ACTUAL = BBEDC_ISMA_ACTUAL_ACTUAL
      , ISMA_30_360        = BBEDC_ISMA_30_360
      , PSA_30_360_EOM     = BBEDC_PSA_30_360_EOM
      , SIA_30_360_EOM     = BBEDC_SIA_30_360_EOM
      , SIA_30_360_NEOM    = BBEDC_SIA_30_360_NEOM
#endif
    };

    // CLASS METHODS
    template <class STREAM>
    static STREAM& bdexStreamIn(STREAM&                         stream,
                                bbedc_DayCountConvention::Type& value,
                                int                             version);
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
    static STREAM& bdexStreamOut(STREAM&                        stream,
                                 bbedc_DayCountConvention::Type value,
                                 int                            version);
        // Write the specified 'value' to the specified output 'stream' and
        // return a reference to the modifiable 'stream'.  Optionally specify
        // an explicit 'version' format; by default, the maximum supported
        // version is written to 'stream' and used as the format.  If 'version'
        // is specified, that format is used but *not* written to 'stream'.  If
        // 'version' is not supported, 'stream' is left unmodified.  See the
        // 'bdex' package-level documentation for more information on 'bdex'
        // streaming of container types.

    static int maxSupportedBdexVersion();
        // Return the most current 'bdex' streaming version number supported by
        // this class.  See the 'bdex' package-level documentation for more
        // information on 'bdex' streaming of container types.

    static
    const char *toAscii(bbedc_DayCountConvention::Type DayCountConvention);
        // Return the string representation exactly matching the enumerator
        // name corresponding to the specified enumeration
        // 'DayCountConvention'.
};

// FREE OPERATORS
bsl::ostream& operator<<(bsl::ostream&                  output,
                         bbedc_DayCountConvention::Type type);
    // Format the specified 'type' to the specified output stream in a string
    // representation exactly matching its enumerator name.

// ===========================================================================
//                      INLINE FUNCTION DEFINITIONS
// ===========================================================================

                     // -------------------------------
                     // struct bbedc_DayCountConvention
                     // -------------------------------

// CLASS METHODS
template <class STREAM>
STREAM& bbedc_DayCountConvention::bdexStreamIn(
                                       STREAM&                         stream,
                                       bbedc_DayCountConvention::Type& value,
                                       int                             version)
{
    switch (version) {
      case 1: {
        unsigned char readValue;
        stream.getUint8(readValue);
        if (stream) {
            if (readValue <= bbedc_DayCountConvention::BBEDC_SIA_30_360_NEOM) {
                value = bbedc_DayCountConvention::Type(readValue);
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
    return stream;
}

template <class STREAM>
STREAM& bbedc_DayCountConvention::bdexStreamOut(
                                        STREAM&                        stream,
                                        bbedc_DayCountConvention::Type value,
                                        int                            version)
{
    switch (version) {
      case 1: {
        stream.putUint8((unsigned char) value);  // write value as single byte
      } break;
    }
    return stream;
}

inline
int bbedc_DayCountConvention::maxSupportedBdexVersion()
{
    return 1;  // Required by BDE policy; versions start at 1.
}

// ============================================================================
//                     namespace bdex_InStreamFunctions
// ============================================================================

namespace bdex_InStreamFunctions {

template <typename STREAM>
inline
STREAM& streamIn(STREAM&                         stream,
                 bbedc_DayCountConvention::Type& value,
                 int                             version)
{
    return bbedc_DayCountConvention::bdexStreamIn(stream, value, version);
}

}  // close namespace bdex_InStreamFunctions;

// ============================================================================
//                      namespace bdex_VersionFunctions
// ============================================================================

namespace bdex_VersionFunctions {

inline
int maxSupportedVersion(bbedc_DayCountConvention::Type)
{
    return bbedc_DayCountConvention::maxSupportedBdexVersion();
}

}  // close namespace bdex_VersionFunctions;

// ============================================================================
//                     namespace bdex_OutStreamFunctions
// ============================================================================

namespace bdex_OutStreamFunctions {

template <typename STREAM>
inline
STREAM& streamOut(STREAM&                               stream,
                  const bbedc_DayCountConvention::Type& value,
                  int                                   version)
{
    return bbedc_DayCountConvention::bdexStreamOut(stream, value, version);
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
