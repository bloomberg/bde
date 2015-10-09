// btlso_lingeroptions.h                                              -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BTLSO_LINGEROPTIONS
#define INCLUDED_BTLSO_LINGEROPTIONS

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide an attribute class to describe socket linger options.
//
//@CLASSES:
//  btlso::LingerOptions: attributes describing socket linger options
//
//@SEE_ALSO: btlso_socketoptions
//
//@DESCRIPTION: This component provides a single, simply constrained
// (value-semantic) attribute class, 'btlso::LingerOptions', that is used to
// describe the linger options of a stream-based socket.  Linger options
// describe how a stream-based socket behaves when it is being closed.  Note
// that linger options is designed to be used in conjunction with the
// 'btlso_socketoptions' component to configure a stream-based socket.
//
///Attributes
///----------
//..
//  Name        Type         Default  Simple Constraints
//  ----------  -----------  -------  ------------------
//  lingerFlag  bool         false    none
//  timeout     int          0        >= 0
//..
//: o lingerFlag: 'true' if the process should block when trying to 'close' a
//:   socket if there is untransmitted data.
//:
//: o timeout: maximum time (in seconds) that a process should block when
//:   trying to 'close' a socket if there is untransmitted data.
//
///Usage
///-----
// In this section we show intended usage of this component.
//
///Example 1: Creating functions to set linger option
/// - - - - - - - - - - - - - - - - - - - - - - - - -
// This component is designed to be used at a higher level to set the linger
// options for a stream-based socket.  This example shows how to create a
// function that takes 'btlso::LingerOptions' as an argument and sets the
// linger options of a socket.  We will assume Berkeley socket API is available
// to configure the socket.
//
// First, we define a cross-platform compatible typedef for a socket handle:
//..
//  #ifdef BSLS_PLATFORM_OS_WINDOWS
//      typedef SOCKET Handle;
//  #else
//      typedef int Handle;
//  #endif
//..
// Then, we declare the function, 'setLingerOptions', that takes a 'Handle' and
// a 'btlso::LingerOptions' object, and sets the linger options for 'Handle':
//..
//  int setLingerOptions(Handle                      handle,
//                       const btlso::LingerOptions& lingerOptions)
//  {
//..
// Next, we define a 'typedef' for the 'struct' needed to set the linger
// options:
//..
//  #if defined(BSLS_PLATFORM_OS_WINDOWS)
//      typedef LINGER LingerData;
//  #else
//      typedef linger LingerData;
//  #endif
//..
// Then, we initialize a 'LingerData' object with data from 'lingerOptions',
// which will be supplied to the 'setsockopt' system call:
//..
//      LingerData linger;
//      linger.l_onoff  = lingerOptions.lingerFlag();
//      linger.l_linger = lingerOptions.timeout();
//..
// Next, we configure the linger options for the socket:
//..
//  #if defined(BSLS_PLATFORM_OS_WINDOWS)
//      return ::setsockopt(handle,
//                          SOL_SOCKET,
//                          SO_LINGER,
//                          reinterpret_cast<char *>(&linger),
//                          sizeof linger);
//  #else
//      return ::setsockopt(handle,
//                          SOL_SOCKET,
//                          SO_LINGER,
//                          reinterpret_cast<void *>(&linger),
//                          sizeof linger);
//  #endif
//  }
//..
// Then, we create a new socket using the 'socket' function from Berkeley API:
//..
//  Handle socketHandle = ::socket(AF_INET, SOCK_STREAM, 0);
//..
// Now, we create a 'btlso::LingerOptions' object, 'option', indicating an
// associated socket should block for 2 seconds when closing a stream with
// untransmitted data (i.e., lingering):
//..
//  btlso::LingerOptions option(true, 2);
//..
// Finally, we call 'setLingerOptions' (defined above), to configure the
// options for the socket:
//..
//  setLingerOptions(socketHandle, option);
//..

#ifndef INCLUDED_BTLSCM_VERSION
#include <btlscm_version.h>
#endif

#ifndef INCLUDED_BSLMF_ISBITWISEMOVEABLE
#include <bslmf_isbitwisemoveable.h>
#endif

#ifndef INCLUDED_BSLMF_NESTEDTRAITDECLARATION
#include <bslmf_nestedtraitdeclaration.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif

namespace BloombergLP {

namespace btlso {

                       // ===================
                       // class LingerOptions
                       // ===================

class LingerOptions {
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
    int  d_timeout;     // maximum time-out value (in seconds) that a process
                        // should block when trying to 'close' a socket if
                        // there is untransmitted data

    bool d_lingerFlag;  // flag specifying if the process should block when
                        // trying to 'close' a socket if there is
                        // untransmitted data

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(LingerOptions, bslmf::IsBitwiseMoveable)

    // CREATORS
    LingerOptions();
        // Create a 'LingerOptions' object having the (default) attribute
        // values:
        //..
        //  timeout    == 0
        //  lingerFlag == false
        //..

    LingerOptions(int timeout, bool lingerFlag);
        // Create a 'LingerOptions' object having the specified 'timeout', and
        // 'lingerFlag' attribute values.  The behavior is undefined unless
        // '0 <= timeout'.

    //! LingerOptions(const LingerOptions& original) = default;
        // Create a 'LingerOptions' object having the same value as the
        // specified 'original' object.

    ~LingerOptions();
        // Destroy this object.

    // MANIPULATORS
    //! LingerOptions& operator=(const LingerOptions& rhs) = default;
        // Assign to this object the value of the specified 'rhs' object, and
        // return a reference providing modifiable access to this object.

    void setTimeout(int value);
        // Set the 'timeout' attribute of this object to the specified 'value'.
        // The behavior is undefined unless '0 <= value'.

    void setLingerFlag(bool value);
        // Set the 'lingerFlag' attribute of this object to the specified
        // 'value'.

    // ACCESSORS
    bool lingerFlag() const;
        // Return the value of the 'lingerFlag' attribute of this object.

    int timeout() const;
        // Return the value of the 'timeout' attribute of this object.  Note
        // that '0 <= timeout()'.

                                  // Aspects

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
bool operator==(const LingerOptions& lhs, const LingerOptions& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects have the same
    // value, and 'false' otherwise.  Two 'LingerOptions' objects have the same
    // value if all of the corresponding values of their 'timeout', and
    // 'lingerFlag' attributes are the same.

inline
bool operator!=(const LingerOptions& lhs, const LingerOptions& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects do not have the
    // same value, and 'false' otherwise.  Two 'LingerOptions' objects do not
    // have the same value if any of the corresponding values of their
    // 'timeout', and 'lingerFlag' attributes are not the same.

bsl::ostream& operator<<(bsl::ostream& stream, const LingerOptions& object);
    // Write the value of the specified 'object' to the specified output
    // 'stream' in a single-line format, and return a reference providing
    // modifiable access to 'stream'.  If 'stream' is not valid on entry, this
    // operation has no effect.  Note that this human-readable format is not
    // fully specified and can change without notice.  Also note that this
    // method has the same behavior as 'object.print(stream, 0, -1)', but with
    // the attribute names elided.

// ============================================================================
//                         INLINE FUNCTION DEFINITIONS
// ============================================================================

                       // -------------------
                       // class LingerOptions
                       // -------------------

// CREATORS
inline
LingerOptions::LingerOptions()
: d_timeout(0)
, d_lingerFlag(false)
{
}

inline
LingerOptions::LingerOptions(int timeout, bool lingerFlag)
: d_timeout(timeout)
, d_lingerFlag(lingerFlag)
{
}

inline
LingerOptions::~LingerOptions()
{
    BSLS_ASSERT_SAFE(0 <= d_timeout);
}

inline
void LingerOptions::setTimeout(int value)
{
    BSLS_ASSERT_SAFE(0 <= value);

    d_timeout = value;
}

inline
void LingerOptions::setLingerFlag(bool value)
{
    d_lingerFlag = value;
}

// ACCESSORS
inline
int LingerOptions::timeout() const
{
    return d_timeout;
}

inline
bool LingerOptions::lingerFlag() const
{
    return d_lingerFlag;
}

}  // close package namespace

// FREE OPERATORS
inline
bool btlso::operator==(const LingerOptions& lhs, const LingerOptions& rhs)
{
    return  lhs.lingerFlag() == rhs.lingerFlag()
         && lhs.timeout()    == rhs.timeout();
}

inline
bool btlso::operator!=(const LingerOptions& lhs, const LingerOptions& rhs)
{
    return  lhs.lingerFlag() != rhs.lingerFlag()
         || lhs.timeout()    != rhs.timeout();
}

}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2015 Bloomberg Finance L.P.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// ----------------------------- END-OF-FILE ----------------------------------
