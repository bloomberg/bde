// bteso_lingeroptions.h                                              -*-C++-*-
#ifndef INCLUDED_BTESO_LINGEROPTIONS
#define INCLUDED_BTESO_LINGEROPTIONS

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide an attribute class to describe socket linger options.
//
//@CLASSES:
//  bteso_LingerOptions: attributes describing socket linger options
//
//@AUTHOR: Rohan Bhindwale (rbhindwa), Raymond Chiu (schiu49)
//
//@SEE_ALSO: bteso_socketoptions
//
//@DESCRIPTION: This component provides a single, simply constrained
// (value-semantic) attribute class, 'bteso_LingerOptions', that is used
// to describe the linger options of a stream-based socket.  Linger options
// describe how a stream-based socket behaves when closed.  Note that loger
// options is designed to be used in conjunction with the 'bteso_socketoptions'
// component to configure a stream-based socket.
//
///Attributes
///----------
//..
//  Name        Type         Default  Simple Constraints
//  ----------  -----------  -------  ------------------
//  timeout     int          0        >= 0
//  lingerFlag  bool         false    none
//..
//: o timeout: maximum time (in seconds) that a process should block when
//:   trying to 'close' a socket if there is untransmitted data.
//:
//: o lingerFlag: 'true' if the process should block when trying to 'close' a
//:   socket if there is untransmitted data.
//
///Usage
///-----
// In this section we show intended usage of this component.
//
///Example 1: Creating functions to set linger option.
///---------------------------------------------------
// This component is designed to be used at a higher level to set the linger
// option for a stream-based socket.  This example shows how to create a
// function that takes 'bteso_LingerOptions' as an argument and set the linger
// option of a socket.  We will assume Berkeley socket API is available to
// configure the socket.
//
// First, we define a cross-platform compatible typedef for a socket handle.
//..
//#ifdef BSLS_PLATFORM__OS_WINDOWS
//  typedef SOCKET Handle;
//#else
//  typedef int Handle;
//#endif
//..
// Then, we declare the 'struct' needed to set the linger option.
//..
//struct LingerData {
//#if defined(BSLS_PLATFORM__OS_UNIX) && !defined(BSLS_PLATFORM__OS_CYGWIN)
//  int  l_onoff;
//  int  l_linger;
//#elif defined(BSLS_PLATFORM__OS_WINDOWS) || defined(BSLS_PLATFORM__OS_CYGWIN)
//  u_short  l_onoff;
//  u_short  l_linger;
//#endif
//};
//..
// Next, we declare the function, 'setLingerOption', that takes a 'Handle' and
// a 'bteso_LingerOptions' object, and set the linger option for the 'Handle'.
//..
//int setLingerOption(Handle                     handle,
//                    const bteso_LingerOptions& lingerOptions)
//{
//..
// Then, we use create 'LingerData' struct that was just defined and copies the
// data from 'lingerOptions'
//..
//    LingerData linger;
//    linger.l_onoff  = lingerOptions.lingerFlag();
//    linger.l_linger = lingerOptions.timeout();
//..
// Next, we set the linger option for the socket.
//..
//    return ::setsockopt(handle,
//                        SOL_SOCKET,
//                        SO_LINGER,
//                        reinterpret_cast<void*>(&linger),
//                        sizeof linger);
//}
//..
// Then, we will also implement a 'getLingerOption' to retrieve the option we
// set.
//..
//void getLingerOption(bteso_LingerOptions *result,
//                     Handle               handle)
//{
//    LingerData linger;
//#if defined (BSLS_PLATFORM__OS_AIX) || defined (BSLS_PLATFORM__CMP_GNU)
//    socklen_t len;
//#else
//    int len;
//#endif
//    int len = sizeof linger;
//    ::getsockopt(handle,
//                 SOL_SOCKET,
//                 SO_LINGER,
//                 reinterpret_cast<void*>(&linger),
//                 &len);
//    result->setLingerFlag(linger.l_onoff);
//    result->setTimeout(linger.l_linger);
//}
//..
// Next, we can start using these functions.  We create a 'bteso_LingerOptions'
// object, set 'timeout' to 2 seconds, and 'lingerFlag' to true.
//..
//  bteso_LingerOptions option(2, true);
//..
// Then, we create a new socket using the 'socket' function from Berkeley API
// and set the linger option for that socket.
//..
//  Handle socketHandle = ::socket(AF_INET, SOCK_STREAM, 0);
//..
// Now, we set the linger option of 'socketHandle'.
//..
//  setLingerOption(socketHandle, option);
//..
// Finally, we use the 'getLingerOption' function to verify the linger option
// is set.
//..
//  bteso_LingerOptions result;
//  getLingerOption(&result, socketHandle);
//  assert(option == result);
//..

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
    // This simply constrained (value-semantic) attribute class describes
    // linger options on a stream-based socket.  See the Attributes section
    // under @DESCRIPTION in the component-level documentation.  Note that the
    // class invariants are identically the constraints on the individual
    // attributes.
    //
    // This class:
    //: o supports a complete set of *value-semantic* operations
    //: o is *exception-safe*
    //: o is *alias-safe*
    //: o is 'const' *thread-safe*
    // For terminology see 'bsldoc_glossary'.

    // DATA
    int   d_timeout;     // maximum time out value (in seconds) that a process
                         // should block when trying to 'close' a socket if
                         // there is untransmitted data

    bool  d_lingerFlag;  // flag specifying if the process should block when
                         // trying to 'close' a socket if there is
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
        // Create a 'bteso_LingerOptions' object having the (default) attribute
        // values:
        //..
        //  timeout    == 0
        //  lingerFlag == false
        //..

    bteso_LingerOptions(int timeout, bool lingerFlag);
        // Create a 'bteso_LingerOptions' object having the specified
        // 'timeout', and 'lingerFlag' attribute values.  The behavior is
        // undefined unless '0 <= timeout'.

    //! bteso_LingerOptions(const bteso_LingerOptions& original) = default;
        // Create a 'bteso_LingerOptions' object having the same value as the
        // specified 'original' object.

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
    // The following destructor is generated by the compiler, except in "SAFE"
    // build modes (e.g., to enable the checking of class invariants).

    ~bteso_LingerOptions();
        // Destroy this object.
#endif

    // MANIPULATORS
    void setTimeout(int value);
        // Set the 'timeout' attribute of this object to the specified 'value'.
        // The behavior is undefined unless '0 <= value'.

    void setLingerFlag(bool value);
        // Set the 'lingerFlag' attribute of this object to the specified
        // 'value'.

                                  // Aspects

    template <class STREAM>
    STREAM& bdexStreamIn(STREAM& stream, int version);
        // Assign to this object the value read from the specified input
        // 'stream' using the specified 'version' format, and return a
        // reference to the modifiable 'stream'.  If 'stream' is initially
        // invalid, this operation has no effect.  If 'stream' becomes invalid
        // during this operation, this object is valid, but its value is
        // undefined.  If 'version' is not supported, 'stream' is marked
        // invalid and this object is unaltered.  The behavior is undefined
        // unless 'stream' contains valid data.  Note that no version is read
        // from 'stream'.  See the 'bdex' package-level documentation for more
        // information on 'bdex' streaming of value-semantic types and
        // containers.

    // ACCESSORS
    bool lingerFlag() const;
        // Return the value of the 'lingerFlag' attribute of this object.

    int timeout() const;
        // Return the value of the 'timeout' attribute of this object.  Note
        // that '0 <= timeout()'.

                                  // Aspects

    //! bteso_LingerOptions& operator=(const bteso_LingerOptions& rhs) =
    //!                                                                default;
        // Assign to this object the value of the specified 'rhs' object, and
        // return a reference providing modifiable access to this object.

    template <class STREAM>
    STREAM& bdexStreamOut(STREAM& stream, int version) const;
        // Write the value of this object to the specified output 'stream'
        // using the specified 'version' format and return a reference to the
        // modifiable 'stream'.  If 'stream' is initially invalid, this
        // operation has no effect.  If 'version' is not supported, 'stream' is
        // invalidated.  Note that 'version' is not written to 'stream'.  See
        // the 'bdex' package-level documentation for more information on
        // 'bdex' streaming of value-semantic types and containers.

    bsl::ostream& print(bsl::ostream& stream,
                        int           level = 0,
                        int           spacesPerLevel = 4) const;
        // Write the value of this object to the specified output 'stream' in
        // a human-readable format, and return a reference to 'stream'.
        // Optionally specify an initial indentation 'level', whose absolute
        // value is incremented recursively for nested objects.  If 'level' is
        // specified, optionally specify 'spacesPerLevel', whose absolute
        // value indicates the number of spaces per indentation level for this
        // and all of its nested objects.  If 'level' is negative, suppress
        // indentation of the first line.  If 'spacesPerLevel' is negative,
        // format the entire output on one line, suppressing all but the
        // initial indentation (as governed by 'level').  Note that the
        // format is not fully specified, and can change without notice.
};

// FREE OPERATORS
inline
bool operator==(const bteso_LingerOptions& lhs,
                const bteso_LingerOptions& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects have the same
    // value, and 'false' otherwise.  Two 'bteso_LingerOptions' objects have
    // the same value if all of the corresponding values of their 'timeout',
    // and 'lingerFlag' attributes are the same.

inline
bool operator!=(const bteso_LingerOptions& lhs,
                const bteso_LingerOptions& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects do not have the
    // same value, and 'false' otherwise.  Two 'bteso_LingerOptions' objects do
    // not have the same value if any of the corresponding values of their
    // 'timeout', and 'lingerFlag' attributes are not the same.

bsl::ostream& operator<<(bsl::ostream&              stream,
                         const bteso_LingerOptions& object);
    // Write the value of the specified 'object' to the specified
    // output 'stream' in a single-line format, and return a reference to
    // 'stream'.  If 'stream' is not valid on entry, this operation has no
    // effect.  Note that this human-readable format is not fully specified
    // and can change without notice.  Also note that this method has the same
    // behavior as 'object.print(stream, 0, -1)' with the attribute names
    // elided.

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

// CREATORS
inline
bteso_LingerOptions::bteso_LingerOptions()
: d_timeout(0)
, d_lingerFlag(false)
{
}

inline
bteso_LingerOptions::bteso_LingerOptions(int timeout, bool lingerFlag)
: d_timeout(timeout)
, d_lingerFlag(lingerFlag)
{
}

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
inline
bteso_LingerOptions::~bteso_LingerOptions()
{
    BSLS_ASSERT_SAFE(0 <= d_timeout);
}
#endif

// MANIPULATORS
template <class STREAM>
STREAM& bteso_LingerOptions::bdexStreamIn(STREAM& stream, int version)
{
    if (stream) {
        switch (version) {
          case 1: {
            bdex_InStreamFunctions::streamIn(stream, d_timeout,    1);
            bdex_InStreamFunctions::streamIn(stream, d_lingerFlag, 1);

            if (!stream) {
                d_timeout    = 0;
                d_lingerFlag = 0;
            }

            BSLS_ASSERT(0 <= d_timeout);
          } break;
          default: {
            stream.invalidate();
          }
        }
    }
    return stream;
}

inline
void bteso_LingerOptions::setTimeout(int value)
{
    BSLS_ASSERT_SAFE(0 <= value)

    d_timeout = value;
}

inline
void bteso_LingerOptions::setLingerFlag(bool value)
{
    d_lingerFlag = value;
}

// ACCESSORS
template <class STREAM>
STREAM& bteso_LingerOptions::bdexStreamOut(STREAM& stream, int version) const
{
    if (stream) {
        switch (version) {
          case 1: {
            bdex_OutStreamFunctions::streamOut(stream, d_timeout, 1);
            bdex_OutStreamFunctions::streamOut(stream, d_lingerFlag, 1);
          } break;
          default: {
            stream.invalidate();
          }
        }
        return stream;                                                // RETURN
    }
}

inline
int bteso_LingerOptions::timeout() const
{
    return d_timeout;
}

inline
bool bteso_LingerOptions::lingerFlag() const
{
    return d_lingerFlag;
}

// FREE OPERATORS
inline
bool operator==(const bteso_LingerOptions& lhs,
                const bteso_LingerOptions& rhs)
{
    return  lhs.lingerFlag() == rhs.lingerFlag()
         && lhs.timeout()    == rhs.timeout();
}

inline
bool operator!=(const bteso_LingerOptions& lhs,
                const bteso_LingerOptions& rhs)
{
    return  lhs.lingerFlag() != rhs.lingerFlag()
         || lhs.timeout()    != rhs.timeout();
}

}  // close enterprise namespace
#endif

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2011
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ------------------------------ END-OF-FILE ---------------------------------
