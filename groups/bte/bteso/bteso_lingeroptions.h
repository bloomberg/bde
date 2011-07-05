// bteso_lingeroptions.h                                              -*-C++-*-
#ifndef INCLUDED_BTESO_LINGEROPTIONS
#define INCLUDED_BTESO_LINGEROPTIONS

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a value-semantic class to store socket linger options.
//
//@CLASSES:
//  bteso_LingerOptions: container for storing socket linger options
//
//@AUTHOR: Rohan Bhindwale (rbhindwa)
//
//@SEE_ALSO: bteso_socketoptions
//
//@DESCRIPTION: This component provides a class, 'bteso_LingerOptions', used to
// specify the linger options that can be set on a socket.  Linger options are
// options on a socket that has data queued to be sent but is being closed.
// Note that linger options cannot be set directly on a socket; instead, they
// must be used in conjunction with 'bteso_SocketOptions'.
//
// The various linger options that can be set on a socket are specified below:
//..
//  Option           Description
//  ------           -----------
//
//  useLingering     This option indicates whether a socket should linger
//                   (i.e., wait if it has queued data even if it was closed).
//
//  timeout          This option specifies the maximum duration (in seconds)
//                   that the socket should be kept open (and continue to try
//                   sending the queued data) if it is lingering.
//..
//
///Usage
///-----
// The following snippets of code illustrate how to set linger options:
//..
//  bteso_LingerOptions lingerOptions;
//
//  // Set the lingering option with a timeout of 2 seconds.
//  lingerOptions.setUseLingering(true);
//  lingerOptions.setTimeout(2);
//..
// We can then set these linger options on any socket handle using
// 'bteso_SocketOptions'.

#ifndef INCLUDED_BTESCM_VERSION
#include <btescm_version.h>
#endif

#ifndef INCLUDED_BDEX_INSTREAMFUNCTIONS
#include <bdex_instreamfunctions.h>
#endif

#ifndef INCLUDED_BDEX_OUTSTREAMFUNCTIONS
#include <bdex_outstreamfunctions.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITS
#include <bslalg_typetraits.h>
#endif

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif

namespace BloombergLP {

                       // =========================
                       // class bteso_LingerOptions
                       // =========================

class bteso_LingerOptions {
    // This class provides an value-semantic object that contains the socket
    // linger options.
    //
    // More generally, this class supports a complete set of *value* *semantic*
    // operations, including copy construction, assignment, equality
    // comparison, 'ostream' printing, and 'bdex' serialization.  (A precise
    // operational definition of when two objects have the same value can be
    // found in the description of the homogeneous (free) 'operator==' for this
    // class.)  This class is *exception* *safe*, but provides no general
    // guarantee of rollback: If an exception is thrown during the invocation
    // of a method on a pre-existing object, the object will be left in a
    // coherent state, but (unless otherwise stated) its value is not defined.
    // In no event is memory leaked.  Finally, *aliasing* (e.g., using all or
    // part of an object as both source and destination) for the same operation
    // is supported in all cases.

    // DATA
    int   d_timeout;       // maximum time out value (in seconds) that a
                           // process should be blocked when trying to 'close'
                           // a socket if there is untransmitted data

    bool  d_useLingering;  // flag specifying if the process should be blocked
                           // when trying to 'close' a socket if there is
                           // untransmitted data

  public:
    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(bteso_LingerOptions,
                                 bslalg_TypeTraitBitwiseMoveable);

    // CLASS METHODS
    static int maxSupportedBdexVersion();
        // Return the most current 'bdex' streaming version number supported by
        // this class.  See the 'bdex' package-level documentation for more
        // information on 'bdex' streaming of value-semantic types and
        // containers.

    // CREATORS
    bteso_LingerOptions();
        // Create a linger options object that does not use lingering and has a
        // timeout of 0 seconds.

    bteso_LingerOptions(const bteso_LingerOptions& original);
        // Create a linger options object having the value of the specified
        // 'original' object.

    ~bteso_LingerOptions();
        // Destroy this linger options object.

    // MANIPULATORS
    bteso_LingerOptions& operator=(const bteso_LingerOptions& rhs);
        // Assign to this object the value of the specified 'rhs' object.

    template <class STREAM>
    STREAM& bdexStreamIn(STREAM& stream, int version);
        // Assign to this object the value read from the specified input
        // 'stream' using the specified 'version' format and return a reference
        // to the modifiable 'stream'.  If 'stream' is initially invalid, this
        // operation has no effect.  If 'stream' becomes invalid during this
        // operation, this object is valid, but its value is undefined.  If
        // 'version' is not supported, 'stream' is marked invalid and this
        // object is unaltered.  Note that no version is read from 'stream'.
        // See the 'bdex' package-level documentation for more information on
        // 'bdex' streaming of value-semantic types and containers.

    void reset();
        // Reset this linger options object to the default value (i.e., its
        // value upon default construction).

    void setUseLingering(bool value);
        // Set the 'useLingering' attribute of this object to the specified
        // 'value'.

    void setTimeout(int value);
        // Set the 'timeout' attribute of this object to the specified 'value'.

    // ACCESSORS
    bsl::ostream& print(bsl::ostream& stream,
                        int           level = 0,
                        int           spacesPerLevel = 4) const;
        // Format this object to the specified output 'stream' at the
        // optionally specified indentation 'level' and return a reference to
        // the modifiable 'stream'.  If 'level' is specified, optionally
        // specify 'spacesPerLevel', the number of spaces per indentation level
        // for this and all of its nested objects.  Each line is indented by
        // the absolute value of 'level * spacesPerLevel'.  If 'level' is
        // negative, suppress indentation of the first line.  If
        // 'spacesPerLevel' is negative, suppress line breaks and format the
        // entire output on one line.  If 'stream' is initially invalid, this
        // operation has no effect.  Note that a trailing newline is provided
        // in multiline mode only.

    template <class STREAM>
    STREAM& bdexStreamOut(STREAM& stream, int version) const;
        // Write the value of this object to the specified output 'stream'
        // using the specified 'version' format and return a reference to the
        // modifiable 'stream'.  If 'version' is not supported, 'stream' is
        // unmodified.  Note that 'version' is not written to 'stream'.
        // See the 'bdex' package-level documentation for more information
        // on 'bdex' streaming of value-semantic types and containers.

    bool useLingering() const;
        // Return the 'useLingering' attribute of this object

    int timeout() const;
        // Return the 'timeout' attribute of this object
};

// FREE OPERATORS
inline
bool operator==(const bteso_LingerOptions& lhs,
                const bteso_LingerOptions& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' linger options objects
    // have the same value, and 'false' otherwise.  Two linger options objects
    // have the same value if each of the 'useLingering' and 'timeout'
    // attributes have the same value.

inline
bool operator!=(const bteso_LingerOptions& lhs,
                const bteso_LingerOptions& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' linger options objects
    // do not have the same value, and 'false' otherwise.  Two linger options
    // objects do not have the same value if either of the 'useLingering' and
    // 'timeout' attributes have a different value.

inline
bsl::ostream& operator<<(bsl::ostream& stream, const bteso_LingerOptions& rhs);
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.

// ============================================================================
//                         INLINE FUNCTION DEFINITIONS
// ============================================================================

                       // -------------------------
                       // class bteso_LingerOptions
                       // -------------------------

// CLASS METHODS
inline
int bteso_LingerOptions::maxSupportedBdexVersion()
{
    return 1;  // versions start at 1.
}

// MANIPULATORS
template <class STREAM>
STREAM& bteso_LingerOptions::bdexStreamIn(STREAM& stream, int version)
{
    if (stream) {
        switch (version) {
          case 1: {
            bdex_InStreamFunctions::streamIn(stream, d_useLingering, 1);
            bdex_InStreamFunctions::streamIn(stream, d_timeout, 1);
          } break;
          default: {
            stream.invalidate();
          }
        }
    }
    return stream;
}

inline
void bteso_LingerOptions::setUseLingering(bool value)
{
    d_useLingering = value;
}

inline
void bteso_LingerOptions::setTimeout(int value)
{
    d_timeout = value;
}

// ACCESSORS
template <class STREAM>
STREAM& bteso_LingerOptions::bdexStreamOut(STREAM& stream, int version) const
{
    switch (version) {
      case 1: {
        bdex_OutStreamFunctions::streamOut(stream, d_useLingering, 1);
        bdex_OutStreamFunctions::streamOut(stream, d_timeout, 1);
      } break;
    }
    return stream;
}

inline
bool bteso_LingerOptions::useLingering() const
{
    return d_useLingering;
}

inline
int bteso_LingerOptions::timeout() const
{
    return d_timeout;
}

// FREE FUNCTIONS
inline
bool operator==(const bteso_LingerOptions& lhs,
                const bteso_LingerOptions& rhs)
{
    return  lhs.useLingering() == rhs.useLingering()
         && lhs.timeout()      == rhs.timeout();
}

inline
bool operator!=(const bteso_LingerOptions& lhs,
                const bteso_LingerOptions& rhs)
{
    return  lhs.useLingering() != rhs.useLingering()
         || lhs.timeout() != rhs.timeout();
}

inline
bsl::ostream& operator<<(bsl::ostream&              stream,
                         const bteso_LingerOptions& rhs)
{
    return rhs.print(stream, 0, -1);
}

}  // close namespace BloombergLP
#endif

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2010
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ------------------------------ END-OF-FILE ---------------------------------
