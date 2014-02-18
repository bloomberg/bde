// bdetu_systemclocktype.h                                            -*-C++-*-
#ifndef INCLUDED_BDETU_SYSTEMCLOCKTYPE
#define INCLUDED_BDETU_SYSTEMCLOCKTYPE

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide an enumeration of (and operations on) clock types.
//
//@CLASSES:
//  bdetu_SystemClockType: namespace for clock types enumeration (and its
//                         operations)
//
//@AUTHOR: Alexei Zakharov, Jeffrey Mendelsohn (jmendelsohn4)
//
//@SEE_ALSO: bdetu_systemtime
//
//@DESCRIPTION: This component provides a 'struct', 'bdetu_SystemTimeClock',
// which serves as a namespace for enumerating clock types along with a
// standard set of supporting operations.  These operations include converting
// an enumerated value to its corresponding string representation, writing the
// string form directly to a standard 'bsl::ostream', and reading from and
// writing to a 'bdex' stream.
//
// The clock types defined map closely to the clock types supported by the
// POSIX Standard:
//..
// SystemClockType value    POSIX value
// e_REALTIME               CLOCK_REALTIME
// e_MONOTONIC              CLOCK_MONOTONIC
//..
//
///USAGE
///-----
// The following snippets of code provide a simple illustration of using
// 'bdetu_SystemClockType'.  First we create a variable, 'clock', of type
// 'bdetu_SystemClockType::Type', and initialize it to the value
// 'bdetu_SystemClockType::e_MONOTONIC':
//..
//  bdetu_SystemClockType::Type clock = bdetu_SystemClockType::e_MONOTONIC;
//..
// Next store its representation in a variable, 'rep', of type 'const char *':
//..
//  const char *rep = bdetu_SystemClockType::toAscii(clock);
//  assert(0 == bsl::strcmp(rep, "MONOTONIC"));
//..
// Finally, we print the value of 'clock' to 'stdout':
//..
//  bsl::cout << clock << bsl::endl;
//..
// This statement produces the following output on 'stdout':
//..
//  MONOTONIC
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

                        // ============================
                        // struct bdetu_SystemClockType
                        // ============================

struct bdetu_SystemClockType {
    // This 'struct' provides a namespace for enumerating system clock values
    // as well as standard operations thereon, which include conversion to a
    // corresponding character string representation, writing the string form
    // directly to a standard 'ostream', and reading from and writing to a
    // 'bdex' stream.

  public:
    // TYPES
    enum Type {
        // Enumeration clock type values.

        e_REALTIME =  1,  // wall time (includes all system adjustments)

        e_MONOTONIC = 2   // monotonic non-decreasing clock which includes
                          // a subset, dependant upon platform, of the
                          // adjustments to the realtime clock
    };

    // CLASS METHODS
    static int maxSupportedBdexVersion();
        // Return the most current 'bdex' streaming version number supported by
        // this class.  (See the package-group-level documentation for more
        // information on 'bdex' streaming of container types.)

    static const char *toAscii(bdetu_SystemClockType::Type clock);
        // Return the character-string representation of the enumerator
        // corresponding to the specified 'clock'.

    template <class STREAM>
    static STREAM& bdexStreamIn(STREAM&                      stream,
                                bdetu_SystemClockType::Type& clock,
                                int                          version);
        // Assign, to the specified 'clock', the value read from the specified
        // input 'stream' using the specified 'version' format, and return a
        // reference to the modifiable 'stream'.  If 'stream' is initially
        // invalid, this operation has no effect.  If 'stream' becomes invalid
        // during this operation, 'clock' is valid, but its value is undefined.
        // If 'version' is not supported, 'stream' is marked invalid and
        // 'clock' is unaltered.  Note that no version is read from 'stream'.
        // (See the 'bdex' package-level documentation for more information on
        // 'bdex' streaming of value-semantic types and containers.)

    template <class STREAM>
    static STREAM& bdexStreamOut(STREAM&                     stream,
                                 bdetu_SystemClockType::Type clock,
                                 int                         version);
        // Write the specified 'clock' value to the specified output 'stream'
        // using the specified 'version' format, and return a reference to the
        // modifiable 'stream'.  If 'stream' is initially invalid, this
        // operation has no effect.  If 'version' is not supported, 'stream' is
        // invalidated.  Note that in no event is 'version' written to
        // 'stream'.  (See the 'bdex' package-level documentation for more
        // information on 'bdex' streaming of value-semantic types and
        // containers.)
};

// FREE OPERATORS
bsl::ostream& operator<<(bsl::ostream& lhs, bdetu_SystemClockType::Type rhs);
    // Format, to the specified 'lhs' output stream, the abbreviated
    // character-string representation corresponding to the specified 'rhs'
    // clock value, and return a reference to the modifiable 'lhs'
    // stream.

// ===========================================================================
//                      INLINE FUNCTION DEFINITIONS
// ===========================================================================

                        // ----------------------------
                        // struct bdetu_SystemClockType
                        // ----------------------------

// CLASS METHODS
inline
int bdetu_SystemClockType::maxSupportedBdexVersion()
{
    return 1;
}

template <class STREAM>
STREAM& bdetu_SystemClockType::bdexStreamIn(
                                          STREAM&                      stream,
                                          bdetu_SystemClockType::Type& clock,
                                          int                          version)
{
    if (stream) {
        switch(version) {
          case 1: {
            char newValue;
            stream.getInt8(newValue);     // get the value as a single byte
            if (stream) {
                if (newValue >= e_REALTIME && newValue <= e_MONOTONIC) {
                    clock = bdetu_SystemClockType::Type(newValue);
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
STREAM& bdetu_SystemClockType::bdexStreamOut(
                                           STREAM&                     stream,
                                           bdetu_SystemClockType::Type clock,
                                           int                         version)
{
    if (stream) {
        switch (version) {
          case 1: {

            // Write the value as one byte.

            stream.putInt8(static_cast<char>(clock));

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
bsl::ostream& operator<<(bsl::ostream& lhs, bdetu_SystemClockType::Type rhs)
{
    return lhs << bdetu_SystemClockType::toAscii(rhs);
}

                     // --------------------------------
                     // namespace bdex_InStreamFunctions
                     // --------------------------------

namespace bdex_InStreamFunctions {

template <typename STREAM>
inline
STREAM& streamIn(STREAM&                      stream,
                 bdetu_SystemClockType::Type& value,
                 int                          version)
{
    return bdetu_SystemClockType::bdexStreamIn(stream, value, version);
}

}  // close namespace bdex_InStreamFunctions

                     // -------------------------------
                     // namespace bdex_VersionFunctions
                     // -------------------------------

namespace bdex_VersionFunctions {

inline
int maxSupportedVersion(bdetu_SystemClockType::Type)
{
    return bdetu_SystemClockType::maxSupportedBdexVersion();
}

}  // close namespace bdex_VersionFunctions

                     // ---------------------------------
                     // namespace bdex_OutStreamFunctions
                     // ---------------------------------

namespace bdex_OutStreamFunctions {

template <typename STREAM>
inline
STREAM& streamOut(STREAM&                            stream,
                  const bdetu_SystemClockType::Type& value,
                  int                                version)
{
    return bdetu_SystemClockType::bdexStreamOut(stream, value, version);
}

}  // close namespace bdex_OutStreamFunctions

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2014
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
