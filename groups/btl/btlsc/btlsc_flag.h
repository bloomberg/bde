// btlsc_flag.h              -*-C++-*-
#ifndef INCLUDED_BTLSC_FLAG
#define INCLUDED_BTLSC_FLAG

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

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
// Functionality is provided to convert each of these enumerated values to its
// corresponding string representation, and to write its string form directly
// to a standard 'ostream'.
//
// In addition, this class supports functions that convert these types to
// a well-defined ascii representation.

#ifndef INCLUDED_BTLSCM_VERSION
#include <btlscm_version.h>
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
        k_ASYNC_INTERRUPT = 0x01  // If set, this flag permits an
                                  // operation to be interrupted by an
                                  // unspecified asynchronous event.  By
                                  // default, the implementation will
                                  // ignore such events if possible, or
                                  // fail otherwise.

      , k_RAW             = 0x02  // If set, this flag permits a
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
      , BTESC_ASYNC_INTERRUPT = k_ASYNC_INTERRUPT
      , BTESC_RAW             = k_RAW
      , ASYNC_INTERRUPT = BTESC_ASYNC_INTERRUPT
      , RAW             = BTESC_RAW
#endif // BDE_OMIT_INTERNAL_DEPRECATED
    };

    enum {
        k_LENGTH = 2 // Define LENGTH to be the number of enumerators; they are
                     // flags and *not* consecutive, and so the length must be
                     // kept consistent with the 'enum' "by hand".
#ifndef BDE_OMIT_INTERNAL_DEPRECATED
      , BTESC_LENGTH = k_LENGTH
#endif // BDE_OMIT_INTERNAL_DEPRECATED
    };

    // CLASS METHODS
    static const char *toAscii(Flag value);
        // Return the string representation exactly matching the enumerator
        // name corresponding to the specified enumerator 'value'.

    static
    bsl::ostream& streamOut(bsl::ostream&    stream,
                            btesc_Flag::Flag flag);
        // Write to the specified 'stream' the string representation exactly
        // matching the name corresponding to the specified 'flag' value.
};

// FREE FUNCTIONS
bsl::ostream& operator<<(bsl::ostream& stream, btesc_Flag::Flag rhs);
    // Write to the specified 'stream' the string representation exactly
    // matching the name corresponding to the specified 'rhs' value.

// ===========================================================================
//                        INLINE FUNCTION DEFINITIONS
// ===========================================================================

inline
bsl::ostream& btesc_Flag::streamOut(bsl::ostream&    stream,
                                    btesc_Flag::Flag flag)
{
    return stream << flag;
}

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
