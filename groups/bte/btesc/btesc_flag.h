// btesc_flag.h              -*-C++-*-
#ifndef INCLUDED_BTESC_FLAG
#define INCLUDED_BTESC_FLAG

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Enumerate all flags for stream-based-channel transport.
//
//@CLASSES:
// btesc_Flag: namespace for enumerating all stream-based-channel flags
//
//@SEE_ALSO:
//
//@AUTHOR: Tom Marshall (tmarshal)
//
//@DESCRIPTION: This component provides a namespace, 'btesc_Flag', for
// enumerating all flags of use to the various stream-based channel,
// channel-allocator, and allocator-factory components of this package.
// Functionality is provided to convert each of these enumerated values to
// its corresponding string representation, to write its string form directly
// to a standard 'ostream', and to externalize it to/from a 'bdex' stream.
// In addition, this class supports functions that convert these types to
// a well-defined ascii representation.

#ifndef INCLUDED_BTESCM_VERSION
#include <btescm_version.h>
#endif

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif

namespace BloombergLP {

                        // ================
                        // class btesc_Flag
                        // ================

struct btesc_Flag {
    // This class provides a namespace for enumerating all flags for the
    // 'btesc' package

    // TYPES
    enum Flag {
        BTESC_ASYNC_INTERRUPT = 0x01,  // If set, this flag permits an
                                       // operation to be interrupted by an
                                       // unspecified asynchronous event.  By
                                       // default, the implementation will
                                       // ignore such events if possible, or
                                       // fail otherwise.

        BTESC_RAW             = 0x02   // If set, this flag permits a
                                       // transmission method to suspend itself
                                       // between OS-level atomic operations
                                       // provided (1) at least one additional
                                       // byte was transmitted and (2) no
                                       // additional bytes can be transmitted
                                       // immediately -- e.g., without
                                       // (potentially) blocking.  By default,
                                       // the implementation will continue
                                       // until it either succeeds, fails, or
                                       // returns a partial result for some
                                       // other, authorized reason.
#ifndef BDE_OMIT_INTERNAL_DEPRECATED
      , ASYNC_INTERRUPT = BTESC_ASYNC_INTERRUPT
      , RAW             = BTESC_RAW
#endif
    };

    enum { BTESC_LENGTH = 2 };
        // Define LENGTH to be the number of enumerators; they are flags and
        // *not* consecutive, and so the length must be kept consistent with
        // the 'enum' "by hand".

    // CLASS METHODS
    static const char *toAscii(Flag value);
        // Return the string representation exactly matching the enumerator
        // name corresponding to the specified enumerator 'value'.

    static int maxSupportedBdexVersion();
        // Return the most current 'bdex' streaming version number supported by
        // this class.  (See the package-group-level documentation for more
        // information on 'bdex' streaming of container types.)

    static int maxSupportedVersion();
        // Return the most current 'bdex' streaming version number supported by
        // this class.  (See the package-group-level documentation for more
        // information on 'bdex' streaming of container types.)
        //
        // DEPRECATED: replaced by 'maxSupportedBdexVersion()'

    template <class STREAM>
    static STREAM& bdexStreamIn(STREAM&           stream,
                                btesc_Flag::Flag& value,
                                int               version);
        // Assign to the specified 'value' the value read from the specified
        // input 'stream' using the specified 'version' format and return a
        // reference to the modifiable 'stream'.  If 'stream' is initially
        // invalid, this operation has no effect.  If 'stream' becomes invalid
        // during this operation, the 'value' is valid, but its value is
        // undefined.  If the specified 'version' is not supported, 'stream' is
        // marked invalid, but 'value' is unaltered.  Note that no version is
        // read from 'stream'.  (See the package-group-level documentation for
        // more information on 'bdex' streaming of container types.)

    static
    bsl::ostream& streamOut(bsl::ostream&    stream,
                            btesc_Flag::Flag flag);
        // Write to the specified 'stream' the string representation exactly
        // matching the name corresponding to the specified 'flag' value.

    template <class STREAM>
    static STREAM& bdexStreamOut(STREAM&          stream,
                                 btesc_Flag::Flag value,
                                 int              version);
        // Write the specified 'value' to the specified output 'stream' and
        // return a reference to the modifiable 'stream'.  Optionally specify
        // an explicit 'version' format; by default, the maximum supported
        // version is written to 'stream' and used as the format.  If 'version'
        // is specified, that format is used but *not* written to 'stream'.  If
        // 'version' is not supported, 'stream' is left unmodified.  (See the
        // package-group-level documentation for more information on 'bdex'
        // streaming of container types).
};

// FREE FUNCTIONS
bsl::ostream& operator<<(bsl::ostream& stream, btesc_Flag::Flag rhs);
    // Write to the specified 'stream' the string representation exactly
    // matching the name corresponding to the specified 'rhs' value.

// ===========================================================================
//                        INLINE FUNCTION DEFINITIONS
// ===========================================================================

inline
int btesc_Flag::maxSupportedBdexVersion()
{
    return 1;
}

inline
int btesc_Flag::maxSupportedVersion()
{
    return maxSupportedBdexVersion();
}

template <class STREAM>
STREAM& btesc_Flag::bdexStreamIn(STREAM&           stream,
                                 btesc_Flag::Flag& value,
                                 int               version)
{
    switch(version) {
      case 1: {
        char tmpValue;
        stream.getInt8(tmpValue);
        if (stream) {
            if (tmpValue == 1 || (tmpValue % 2 == 0  &&
                1 << btesc_Flag::BTESC_LENGTH > tmpValue)) {
                value = btesc_Flag::Flag(tmpValue);
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
    return stream;
}

inline
bsl::ostream& btesc_Flag::streamOut(bsl::ostream&    stream,
                                    btesc_Flag::Flag flag)
{
    return stream << flag;
}

template <class STREAM>
inline
STREAM& btesc_Flag::bdexStreamOut(STREAM& stream, btesc_Flag::Flag value, int)

{
    // TBD switch over version
    stream.putInt8((char)value);     // Write the value as a single byte.
    return stream;
}

// ============================================================================
//                     namespace bdex_InStreamFunctions
// ============================================================================

namespace bdex_InStreamFunctions {

template <typename STREAM>
inline
STREAM& streamIn(STREAM&           stream,
                 btesc_Flag::Flag& value,
                 int               version)
{
    return btesc_Flag::bdexStreamIn(stream, value, version);
}

}  // close namespace bdex_InStreamFunctions;

// ============================================================================
//                      namespace bdex_VersionFunctions
// ============================================================================

namespace bdex_VersionFunctions {

inline
int maxSupportedVersion(btesc_Flag::Flag)
{
    return btesc_Flag::maxSupportedBdexVersion();
}

}  // close namespace bdex_VersionFunctions;

// ============================================================================
//                     namespace bdex_OutStreamFunctions
// ============================================================================

namespace bdex_OutStreamFunctions {

template <typename STREAM>
inline
STREAM& streamOut(STREAM&                 stream,
                  const btesc_Flag::Flag& value,
                  int                     version)
{
    return btesc_Flag::bdexStreamOut(stream, value, version);
}

}  // close namespace bdex_OutStreamFunctions

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2002
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
