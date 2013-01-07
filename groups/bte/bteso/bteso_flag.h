// bteso_flag.h              -*-C++-*-
#ifndef INCLUDED_BTESO_FLAG
#define INCLUDED_BTESO_FLAG

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Enumerate all flags for stream-based-channel transport.
//
//@CLASSES:
// bteso_Flag: namespace for enumerating all stream-based-channel flags
//
//@SEE_ALSO:
//
//@AUTHOR: Tom Marshall (tmarshal)
//
//@DESCRIPTION: This component provides a namespace, 'bteso_Flag', for
// enumerating all flags of use to the various socket-based components.
// Functionality is provided to convert each of these enumerated values to
// its corresponding string representation, to write its string form directly
// to a standard 'ostream', and to externalize it to/from a 'bdex'-compatible
// stream.  In addition, this class supports functions that convert these types
// to a well-defined ascii representation.

#ifndef INCLUDED_BTESCM_VERSION
#include <btescm_version.h>
#endif

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif

namespace BloombergLP {

                        // ================
                        // class bteso_Flag
                        // ================

struct bteso_Flag {
    // This class provides a namespace for enumerating all flags for the
    // 'bteso' package.

    // TYPES
    enum Flag {
        BTESO_ASYNC_INTERRUPT = 0x01,  // If set, this flag permits an
                                       // operation to be interrupted by an
                                       // unspecified asynchronous event.  By
                                       // default, the implementation will
                                       // ignore such events if possible, or
                                       // fail otherwise.

        BTESO_NFLAGS = 1               // The number of Flag enumerators.  This
                                       // must be maintained "by hand" since
                                       // flags are not consecutive.
#ifndef BDE_OMIT_INTERNAL_DEPRECATED
      , ASYNC_INTERRUPT = BTESO_ASYNC_INTERRUPT
      , NFLAGS          = BTESO_NFLAGS
#endif // BDE_OMIT_INTERNAL_DEPRECATED
    };

    enum BlockingMode {
        // Values used to set/determine the blocking mode of a
        // 'bteso_StreamSocket' object.
        BTESO_BLOCKING_MODE,     // Indicates blocking mode
        BTESO_NONBLOCKING_MODE   // Indicates non-blocking mode
#ifndef BDE_OMIT_INTERNAL_DEPRECATED
      , BLOCKING_MODE    = BTESO_BLOCKING_MODE
      , NONBLOCKING_MODE = BTESO_NONBLOCKING_MODE
#endif // BDE_OMIT_INTERNAL_DEPRECATED
    };

    enum ShutdownType {
        // Values for options used by 'bteso_StreamSocket<>::shutdown'.
        BTESO_SHUTDOWN_RECEIVE,  // Shut down the input stream of the
                                 // full-duplex connection associated with a
                                 // 'bteso_StreamSocket' object.
        BTESO_SHUTDOWN_SEND,     // Shut down the output stream of the
                                 // full-duplex connection associated with a
                                 // 'bteso_StreamSocket' object.
        BTESO_SHUTDOWN_BOTH      // Shut down the input and output streams of
                                 // the full-duplex connection associated a
                                 // 'bteso_StreamSocket' object.
#ifndef BDE_OMIT_INTERNAL_DEPRECATED
      , SHUTDOWN_RECEIVE = BTESO_SHUTDOWN_RECEIVE
      , SHUTDOWN_SEND    = BTESO_SHUTDOWN_SEND
      , SHUTDOWN_BOTH    = BTESO_SHUTDOWN_BOTH
#endif // BDE_OMIT_INTERNAL_DEPRECATED
    };

    enum IOWaitType {
        // Values for options used by 'bteso_StreamSocket<>::waitForIO'
        BTESO_IO_READ,   // Wait for data to arrive on a socket.
        BTESO_IO_WRITE,  // Wait for buffer space to become available on a
                         // socket.
        BTESO_IO_RW      // Wait for data to arrive or space to become
                         // available on a socket.
#ifndef BDE_OMIT_INTERNAL_DEPRECATED
      , IO_READ  = BTESO_IO_READ
      , IO_WRITE = BTESO_IO_WRITE
      , IO_RW    = BTESO_IO_RW
#endif // BDE_OMIT_INTERNAL_DEPRECATED
    };

    // CLASS METHODS
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
                                bteso_Flag::Flag& value,
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

    template <class STREAM>
    static STREAM& bdexStreamIn(STREAM&                   stream,
                                bteso_Flag::BlockingMode& value,
                                int                       version);
        // Assign to the specified 'value' the value read from the specified
        // input 'stream' using the specified 'version' format and return a
        // reference to the modifiable 'stream'.  If 'stream' is initially
        // invalid, this operation has no effect.  If 'stream' becomes invalid
        // during this operation, the 'value' is valid, but its value is
        // undefined.  If the specified 'version' is not supported, 'stream' is
        // marked invalid, but 'value' is unaltered.  Note that no version is
        // read from 'stream'.  (See the package-group-level documentation for
        // more information on 'bdex' streaming of container types.)

    template <class STREAM>
    static STREAM& bdexStreamIn(STREAM&                   stream,
                                bteso_Flag::ShutdownType& value,
                                int                       version);
        // Assign to the specified 'value' the value read from the specified
        // input 'stream' using the specified 'version' format and return a
        // reference to the modifiable 'stream'.  If 'stream' is initially
        // invalid, this operation has no effect.  If 'stream' becomes invalid
        // during this operation, the 'value' is valid, but its value is
        // undefined.  If the specified 'version' is not supported, 'stream' is
        // marked invalid, but 'value' is unaltered.  Note that no version is
        // read from 'stream'.  (See the package-group-level documentation for
        // more information on 'bdex' streaming of container types.)

    template <class STREAM>
    static STREAM& bdexStreamIn(STREAM&                 stream,
                                bteso_Flag::IOWaitType& value,
                                int                     version);
        // Assign to the specified 'value' the value read from the specified
        // input 'stream' using the specified 'version' format and return a
        // reference to the modifiable 'stream'.  If 'stream' is initially
        // invalid, this operation has no effect.  If 'stream' becomes invalid
        // during this operation, the 'value' is valid, but its value is
        // undefined.  If the specified 'version' is not supported, 'stream' is
        // marked invalid, but 'value' is unaltered.  Note that no version is
        // read from 'stream'.  (See the package-group-level documentation for
        // more information on 'bdex' streaming of container types.)

    static bsl::ostream& streamOut(bsl::ostream&    stream,
                                   bteso_Flag::Flag rhs);
        // Write to the specified 'stream' the string representation exactly
        // matching the name corresponding to the specified 'rhs' value.

    static bsl::ostream& streamOut(bsl::ostream&            stream,
                                   bteso_Flag::BlockingMode rhs);
        // Write to the specified 'stream' the string representation exactly
        // matching the name corresponding to the specified 'rhs' value.

    static bsl::ostream& streamOut(bsl::ostream&            stream,
                                   bteso_Flag::ShutdownType rhs);
        // Write to the specified 'stream' the string representation exactly
        // matching the name corresponding to the specified 'rhs' value.

    static bsl::ostream& streamOut(bsl::ostream&          stream,
                                   bteso_Flag::IOWaitType rhs);
        // Write to the specified 'stream' the string representation exactly
        // matching the name corresponding to the specified 'rhs' value.

    template <class STREAM>
    static STREAM& bdexStreamOut(STREAM&          stream,
                                 bteso_Flag::Flag value,
                                 int              version);
        // Write the specified 'value' to the specified output 'stream' and
        // return a reference to the modifiable 'stream'.  Optionally specify
        // an explicit 'version' format; by default, the maximum supported
        // version is written to 'stream' and used as the format.  If 'version'
        // is specified, that format is used but *not* written to 'stream'.  If
        // 'version' is not supported, 'stream' is left unmodified.  (See the
        // package-group-level documentation for more information on 'bdex'
        // streaming of container types).

    template <class STREAM>
    static STREAM& bdexStreamOut(STREAM&                  stream,
                                 bteso_Flag::BlockingMode value,
                                 int                      version);
        // Write the specified 'value' to the specified output 'stream' and
        // return a reference to the modifiable 'stream'.  Optionally specify
        // an explicit 'version' format; by default, the maximum supported
        // version is written to 'stream' and used as the format.  If 'version'
        // is specified, that format is used but *not* written to 'stream'.  If
        // 'version' is not supported, 'stream' is left unmodified.  (See the
        // package-group-level documentation for more information on 'bdex'
        // streaming of container types).

    template <class STREAM>
    static STREAM& bdexStreamOut(STREAM&                  stream,
                                 bteso_Flag::ShutdownType value,
                                 int                      version);
        // Write the specified 'value' to the specified output 'stream' and
        // return a reference to the modifiable 'stream'.  Optionally specify
        // an explicit 'version' format; by default, the maximum supported
        // version is written to 'stream' and used as the format.  If 'version'
        // is specified, that format is used but *not* written to 'stream'.  If
        // 'version' is not supported, 'stream' is left unmodified.  (See the
        // package-group-level documentation for more information on 'bdex'
        // streaming of container types).

    template <class STREAM>
    static STREAM& bdexStreamOut(STREAM&                stream,
                                 bteso_Flag::IOWaitType value,
                                 int                    version);
        // Write the specified 'value' to the specified output 'stream' and
        // return a reference to the modifiable 'stream'.  Optionally specify
        // an explicit 'version' format; by default, the maximum supported
        // version is written to 'stream' and used as the format.  If 'version'
        // is specified, that format is used but *not* written to 'stream'.  If
        // 'version' is not supported, 'stream' is left unmodified.  (See the
        // package-group-level documentation for more information on 'bdex'
        // streaming of container types).

    static const char *toAscii(bteso_Flag::Flag value);
        // Return the string representation exactly matching the enumerator
        // name corresponding to the specified enumerator 'value'.

    static const char *toAscii(bteso_Flag::BlockingMode value);
        // Return the string representation exactly matching the enumerator
        // name corresponding to the specified enumerator 'value'.

    static const char *toAscii(bteso_Flag::ShutdownType value);
        // Return the string representation exactly matching the enumerator
        // name corresponding to the specified enumerator 'value'.

    static const char *toAscii(bteso_Flag::IOWaitType value);
        // Return the string representation exactly matching the enumerator
        // name corresponding to the specified enumerator 'value'.
};

// FREE FUNCTIONS
inline
bsl::ostream& operator<<(bsl::ostream& stream, bteso_Flag::Flag rhs);
    // Write to the specified 'stream' the string representation exactly
    // matching the name corresponding to the specified 'rhs' value.

inline
bsl::ostream& operator<<(bsl::ostream& stream, bteso_Flag::BlockingMode rhs);
    // Write to the specified 'stream' the string representation exactly
    // matching the name corresponding to the specified 'rhs' value.

inline
bsl::ostream& operator<<(bsl::ostream& stream, bteso_Flag::ShutdownType rhs);
    // Write to the specified 'stream' the string representation exactly
    // matching the name corresponding to the specified 'rhs' value.

inline
bsl::ostream& operator<<(bsl::ostream& stream, bteso_Flag::IOWaitType rhs);
    // Write to the specified 'stream' the string representation exactly
    // matching the name corresponding to the specified 'rhs' value.

// ===========================================================================
//                        INLINE FUNCTION DEFINITIONS
// ===========================================================================

inline
int bteso_Flag::maxSupportedBdexVersion()
{
    return 1;
}

inline
int bteso_Flag::maxSupportedVersion()
{
    return maxSupportedBdexVersion();
}

template <class STREAM>
STREAM& bteso_Flag::bdexStreamIn(STREAM&           stream,
                                 bteso_Flag::Flag& value,
                                 int               version)
{
    switch(version) {
      case 1: {
        char tmpValue;
        stream.getInt8(tmpValue);
        if (stream) {
            if (tmpValue == 1
            || (tmpValue % 2 == 0
             && 1 << bteso_Flag::BTESO_NFLAGS > tmpValue)) {
                value = bteso_Flag::Flag(tmpValue);
            }
            else {
                stream.invalidate();  // Bad value in stream.
            }
        }
      } break;
      default: {
        stream.invalidate();          // Unrecognized version.
      } break;
    }
    return stream;
}

template <class STREAM>
STREAM& bteso_Flag::bdexStreamIn(STREAM&                   stream,
                                 bteso_Flag::BlockingMode& value,
                                 int                       version)
{
    switch(version) {
      case 1: {
        char tmpValue;
        stream.getInt8(tmpValue);
        if (stream) {
            if (bteso_Flag::BTESO_BLOCKING_MODE    <= tmpValue
            &&  bteso_Flag::BTESO_NONBLOCKING_MODE >= tmpValue) {
                value = bteso_Flag::BlockingMode(tmpValue);
            }else{
                stream.invalidate();  // Bad value in stream.
            }
        }
      } break;
      default: {
        stream.invalidate();          // Unrecognized version.
      } break;
    }
    return stream;
}

template <class STREAM>
STREAM& bteso_Flag::bdexStreamIn(STREAM&                   stream,
                                 bteso_Flag::ShutdownType& value,
                                 int                       version)
{
    switch(version) {
      case 1: {
        char tmpValue;
        stream.getInt8(tmpValue);
        if (stream) {
            if (bteso_Flag::BTESO_SHUTDOWN_RECEIVE <= tmpValue
            &&  bteso_Flag::BTESO_SHUTDOWN_BOTH    >= tmpValue) {
                value = bteso_Flag::ShutdownType(tmpValue);
            }else{
                stream.invalidate();  // Bad value in stream.
            }
        }
      } break;
      default: {
        stream.invalidate();          // Unrecognized version.
      } break;
    }
    return stream;
}

template <class STREAM>
STREAM& bteso_Flag::bdexStreamIn(STREAM&                 stream,
                                 bteso_Flag::IOWaitType& value,
                                 int                     version)
{
    switch(version) {
      case 1: {
        char tmpValue;
        stream.getInt8(tmpValue);
        if (stream) {
            if (bteso_Flag::BTESO_IO_READ <= tmpValue
            &&  bteso_Flag::BTESO_IO_RW   >= tmpValue) {
                value = bteso_Flag::IOWaitType(tmpValue);
            }else{
                stream.invalidate();  // Bad value in stream.
            }
        }
      } break;
      default: {
        stream.invalidate();          // Unrecognized version.
      } break;
    }
    return stream;
}

inline
bsl::ostream& operator<<(bsl::ostream& stream, bteso_Flag::Flag rhs)
{
    return bteso_Flag::streamOut(stream, rhs);
}

inline
bsl::ostream& operator<<(bsl::ostream&            stream,
                         bteso_Flag::BlockingMode rhs)
{
    return bteso_Flag::streamOut(stream, rhs);
}

inline
bsl::ostream& operator<<(bsl::ostream&            stream,
                         bteso_Flag::ShutdownType rhs)
{
    return bteso_Flag::streamOut(stream, rhs);
}

inline
bsl::ostream& operator<<(bsl::ostream&          stream,
                         bteso_Flag::IOWaitType rhs)
{
    return bteso_Flag::streamOut(stream, rhs);
}

template <class STREAM>
STREAM& bteso_Flag::bdexStreamOut(STREAM&          stream,
                                  bteso_Flag::Flag value,
                                  int              version)
{
    switch(version) {
      case 1: {
        stream.putInt8(static_cast<char>(value));
      } break;
    }
    return stream;
}

template <class STREAM>
STREAM& bteso_Flag::bdexStreamOut(STREAM&                  stream,
                                  bteso_Flag::BlockingMode value,
                                  int                      version)
{
    switch(version) {
      case 1: {
        stream.putInt8(static_cast<char>(value));
      } break;
    }
    return stream;
}

template <class STREAM>
STREAM& bteso_Flag::bdexStreamOut(STREAM&                  stream,
                                  bteso_Flag::ShutdownType value,
                                  int                      version)
{
    switch(version) {
      case 1: {
        stream.putInt8(static_cast<char>(value));
      } break;
    }
    return stream;
}

template <class STREAM>
STREAM& bteso_Flag::bdexStreamOut(STREAM&                stream,
                                  bteso_Flag::IOWaitType value,
                                  int                    version)
{
    switch(version) {
      case 1: {
        stream.putInt8(static_cast<char>(value));
      } break;
    }
    return stream;
}

// ============================================================================
//                     namespace bdex_InStreamFunctions
// ============================================================================

namespace bdex_InStreamFunctions {

template <typename STREAM>
inline
STREAM& streamIn(STREAM&           stream,
                 bteso_Flag::Flag& value,
                 int               version)
{
    return bteso_Flag::bdexStreamIn(stream, value, version);
}

template <typename STREAM>
inline
STREAM& streamIn(STREAM&                   stream,
                 bteso_Flag::BlockingMode& value,
                 int                       version)
{
    return bteso_Flag::bdexStreamIn(stream, value, version);
}

template <typename STREAM>
inline
STREAM& streamIn(STREAM&                   stream,
                 bteso_Flag::ShutdownType& value,
                 int                       version)
{
    return bteso_Flag::bdexStreamIn(stream, value, version);
}

template <typename STREAM>
inline
STREAM& streamIn(STREAM&                 stream,
                 bteso_Flag::IOWaitType& value,
                 int                     version)
{
    return bteso_Flag::bdexStreamIn(stream, value, version);
}

}  // close namespace bdex_InStreamFunctions;

// ============================================================================
//                      namespace bdex_VersionFunctions
// ============================================================================

namespace bdex_VersionFunctions {

inline
int maxSupportedVersion(bteso_Flag::Flag)
{
    return bteso_Flag::maxSupportedBdexVersion();
}

inline
int maxSupportedVersion(bteso_Flag::BlockingMode)
{
    return bteso_Flag::maxSupportedBdexVersion();
}

inline
int maxSupportedVersion(bteso_Flag::ShutdownType)
{
    return bteso_Flag::maxSupportedBdexVersion();
}

inline
int maxSupportedVersion(bteso_Flag::IOWaitType)
{
    return bteso_Flag::maxSupportedBdexVersion();
}

}  // close namespace bdex_VersionFunctions;

// ============================================================================
//                     namespace bdex_OutStreamFunctions
// ============================================================================

namespace bdex_OutStreamFunctions {

template <typename STREAM>
inline
STREAM& streamOut(STREAM&                 stream,
                  const bteso_Flag::Flag& value,
                  int                     version)
{
    return bteso_Flag::bdexStreamOut(stream, value, version);
}

template <typename STREAM>
inline
STREAM& streamOut(STREAM&                         stream,
                  const bteso_Flag::BlockingMode& value,
                  int                             version)
{
    return bteso_Flag::bdexStreamOut(stream, value, version);
}

template <typename STREAM>
inline
STREAM& streamOut(STREAM&                         stream,
                  const bteso_Flag::ShutdownType& value,
                  int                             version)
{
    return bteso_Flag::bdexStreamOut(stream, value, version);
}

template <typename STREAM>
inline
STREAM& streamOut(STREAM&                       stream,
                  const bteso_Flag::IOWaitType& value,
                  int                           version)
{
    return bteso_Flag::bdexStreamOut(stream, value, version);
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
