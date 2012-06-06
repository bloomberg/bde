// bael_transmission.h                                                -*-C++-*-
#ifndef INCLUDED_BAEL_TRANSMISSION
#define INCLUDED_BAEL_TRANSMISSION

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Enumerate the set of states for log record transmission.
//
//@CLASSES:
//   bael_Transmission: namespace for log record transmission states
//
//@SEE_ALSO: bael_context, bael_observer
//
//@AUTHOR: Hong Shi (hshi2)
//
//@DESCRIPTION: This component provides a namespace for the 'enum' type
// 'bael_Transmission::Cause'.  'Cause' enumerates the list of conditions
// (or states) that can cause a log record to be transmitted.
//
// Templatized functions are provided to support the streaming in and out of
// the enumerated values.  Input streams must be compatible with the
// 'bdex_InStream' protocol, and output streams must be compatible with the
// 'bdex_OutStream' protocol or else convertible to a standard 'ostream'; in
// the latter case, the value is written as its corresponding string
// representation.  In addition, this component supports functions that
// convert the 'Cause' enumerators to a well-defined ASCII representation.
//
///USAGE EXAMPLE 1 - SYNTAX
///------------------------
// The following snippets of code provide a simple illustration of
// 'bael_Transmission' usage.
//
// First create a variable 'cause' of type 'bael_Transmission::Cause' and
// initialize it to the value 'bael_Transmission::BAEL_TRIGGER_ALL':
//..
//  bael_Transmission::Cause cause = bael_Transmission::BAEL_TRIGGER_ALL;
//..
// Next, store a pointer to its ASCII representation in a variable 'asciiCause'
// of type 'const char *':
//..
//  const char *asciiCause = bael_Transmission::toAscii(cause);
//  assert(0 == strcmp(asciiCause, "TRIGGER_ALL"));
//..
// Finally, print the value of 'cause' to 'bsl::cout'.
//..
//  bsl::cout << cause << bsl::endl;
//..
// This statement produces the following output on 'stdout':
//..
//  TRIGGER_ALL
//..
///USAGE EXAMPLE 2 - LOGGING
///-------------------------
// The 'Cause' enumeration defined in this component allows a logging system
// to describe the condition causing the publication of a log message.  One
// possible interpretation of three of these conditions is as follows:
//..
//  (1) BAEL_PASSTHROUGH - indicate that a message is being output as a
//                         stand-alone message.
//
//  (2) BAEL_TRIGGER - indicate that a message is being output as part of a
//                     dump of all messages archived for the current thread.
//
//  (3) BAEL_TRIGGER_ALL - indicate that a message is being output as part of
//                         a dump of all messages archived for *all* threads.
//..
// This example illustrates the use of 'bael_Transmission::Cause' by a
// hypothetical logging system.
//
// We define a simple logger class named 'my_Logger'.  Assume that 'my_Logger'
// accepts messages from clients (e.g., the threads in a multi-threaded
// application) and archives them in-core.  Further assume that the message
// archive is bounded in size in which case 'my_Logger' removes older messages
// to make room for newer ones as the need arises.  When a thread encounters
// an unusual, unexpected, or undesirable condition that it wishes to bring to
// the attention of an operator (say, sitting at a console terminal), it
// "publishes" the message.  That is the role of the 'publish' method in the
// interface of 'my_Logger':
//..
//  // my_logger.h
//
//  class my_Logger {
//    // ...
//    public:
//      my_Logger();
//      ~my_Logger();
//      void publish(const char *message, bael_Transmission::Cause cause);
//      // ...
//  };
//..
// The 'publish' method, defined in the following, shows the different actions
// that are taken for the three distinct causes of log message publication:
//..
//  // my_logger.cpp
//
//  my_Logger::my_Logger() { }
//
//  my_Logger::~my_Logger() { }
//
//  void my_Logger::publish(const char               *message,
//                          bael_Transmission::Cause  cause)
//  {
//      using namespace std;
//      switch (cause) {
//        case bael_Transmission::BAEL_PASSTHROUGH: {
//          cout << bael_Transmission::toAscii(cause) << ":\t" << message
//               << endl;
//        } break;
//        case bael_Transmission::BAEL_TRIGGER: {
//          cout << bael_Transmission::toAscii(cause) << ":\t" << message
//               << endl;
//          cout << "\t[ dump all messages archived for current thread ]"
//               << endl;
//        } break;
//        case bael_Transmission::BAEL_TRIGGER_ALL: {
//          cout << bael_Transmission::toAscii(cause) << ":\t" << message
//               << endl;
//          cout << "\t[ dump all messages archived for *all* threads ]"
//               << endl;
//        } break;
//        default: {
//          cout << "***ERROR*** Unsupported Message Cause: "  << message
//               << endl;
//          return;
//        } break;
//      }
//  }
//
//  // ...
//..
// Finally, we create a 'my_Logger' object and 'publish' three (simplistic)
// messages, each with a different cause:
//..
//  my_Logger logger;
//
//  const char *MSG_PASSTHROUGH = "report relatively minor problem";
//  const char *MSG_TRIGGER     = "report serious thread-specific problem";
//  const char *MSG_TRIGGER_ALL = "report process-impacting problem";
//
//  logger.publish(MSG_PASSTHROUGH, bael_Transmission::BAEL_PASSTHROUGH);
//  logger.publish(MSG_TRIGGER,     bael_Transmission::BAEL_TRIGGER);
//  logger.publish(MSG_TRIGGER_ALL, bael_Transmission::BAEL_TRIGGER_ALL);
//..
// The following output is produced on 'stdout':
//..
//  PASSTHROUGH:    report relatively minor problem
//  TRIGGER:        report serious thread-specific problem
//                  [ dump all messages archived for current thread ]
//  TRIGGER_ALL:    report process-impacting problem
//                  [ dump all messages archived for *all* threads ]
//..

#ifndef INCLUDED_BAESCM_VERSION
#include <baescm_version.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORM
#include <bsls_platform.h>
#endif

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif

#if !defined(BSL_LEGACY) || 1 == BSL_LEGACY
#if defined(BSLS_PLATFORM__CMP_MSVC) && defined(PASSTHROUGH)
    // Note: on Windows -> WinGDI.h:#define PASSTHROUGH 19
#undef PASSTHROUGH
#endif
#endif

namespace BloombergLP {

                        // ========================
                        // struct bael_Transmission
                        // ========================

struct bael_Transmission {
    // This struct provides a namespace for enumerating the causes of the
    // transmission of a log record.

  public:
    // TYPES
    enum Cause {
        BAEL_PASSTHROUGH        = 0,  // single record emitted; caused locally
        BAEL_TRIGGER            = 1,  // all records emitted; caused locally
        BAEL_TRIGGER_ALL        = 2,  // all records emitted; caused remotely
        BAEL_MANUAL_PUBLISH     = 3,  // manually publish a single record
        BAEL_MANUAL_PUBLISH_ALL = 4,  // manually publish all records
        BAEL_END                = 5   // end flag for asynchronous publication

#if !defined(BSL_LEGACY) || 1 == BSL_LEGACY
      , PASSTHROUGH        = BAEL_PASSTHROUGH
      , TRIGGER            = BAEL_TRIGGER
      , TRIGGER_ALL        = BAEL_TRIGGER_ALL
      , MANUAL_PUBLISH     = BAEL_MANUAL_PUBLISH
      , MANUAL_PUBLISH_ALL = BAEL_MANUAL_PUBLISH_ALL
#endif
    };

    enum { BAEL_LENGTH = BAEL_MANUAL_PUBLISH_ALL + 1 };
        // Define 'LENGTH' to be the number of consecutively-valued enumerators
        // in the range '[ BAEL_PASSTHROUGH .. BAEL_MANUAL_PUBLISH_ALL ]'.

  private:
    // PRIVATE CLASS METHODS
    static void print(bsl::ostream& stream, bael_Transmission::Cause value);
        // Write to the specified 'stream' the string representation of
        // the specified enumeration 'value'.

  public:
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
        // DEPRECATED: replaced by the 'maxSupportedBdexVersion' method.

    static const char *toAscii(bael_Transmission::Cause value);
        // Return the string representation exactly matching the enumerator
        // name corresponding to the specified enumeration 'value'.

    template <class STREAM>
    static STREAM& bdexStreamIn(STREAM&                   stream,
                                bael_Transmission::Cause& value,
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

    static bsl::ostream& streamOut(bsl::ostream&            stream,
                                   bael_Transmission::Cause value);
        // Format the specified 'value' to the specified output 'stream'
        // and return a reference to the modifiable 'stream'.

    template <class STREAM>
    static STREAM& bdexStreamOut(STREAM&                  stream,
                                 bael_Transmission::Cause value,
                                 int                      version);
        // Write the specified 'value' to the specified output 'stream' and
        // return a reference to the modifiable 'stream'.  Optionally specify
        // an explicit 'version' format; by default, the maximum supported
        // version is written to 'stream' and used as the format.  If 'version'
        // is specified, that format is used but *not* written to 'stream'.  If
        // 'version' is not supported, 'stream' is left unmodified.  (See the
        // package-group-level documentation for more information on 'bdex'
        // streaming of container types).
};

// FREE OPERATORS
inline
bsl::ostream& operator<<(bsl::ostream& stream, bael_Transmission::Cause rhs);
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.

// ===========================================================================
//                      INLINE FUNCTION DEFINITIONS
// ===========================================================================

                        // ------------------------
                        // struct bael_Transmission
                        // ------------------------

// CLASS METHODS
inline
int bael_Transmission::maxSupportedBdexVersion()
{
    return 1;  // Required by BDE policy; versions start at 1.
}

inline
int bael_Transmission::maxSupportedVersion()
{
    return maxSupportedBdexVersion();
}

template <class STREAM>
STREAM& bael_Transmission::bdexStreamIn(STREAM&                   stream,
                                        bael_Transmission::Cause& value,
                                        int                       version)
{
    switch(version) {
      case 1: {
        unsigned char readValue;
        stream.getUint8(readValue);
        if (stream) {
            if (readValue < bael_Transmission::BAEL_LENGTH) {
                value = bael_Transmission::Cause(readValue);
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

inline
bsl::ostream& bael_Transmission::streamOut(bsl::ostream&            stream,
                                           bael_Transmission::Cause value)
{
    print(stream, value);
    return stream;
}

template <class STREAM>
STREAM& bael_Transmission::bdexStreamOut(STREAM&                  stream,
                                         bael_Transmission::Cause value,
                                         int                      version)
{
    switch (version) {
      case 1: {
        stream.putUint8((unsigned char) value);  // write value as single byte
      } break;
    }
    return stream;
}

// FREE OPERATORS
inline
bsl::ostream& operator<<(bsl::ostream&            stream,
                         bael_Transmission::Cause rhs)
{
    return bael_Transmission::streamOut(stream, rhs);
}

// ============================================================================
//                     namespace bdex_InStreamFunctions
// ============================================================================

namespace bdex_InStreamFunctions {

template <typename STREAM>
inline
STREAM& streamIn(STREAM&                   stream,
                 bael_Transmission::Cause& value,
                 int                       version)
{
    return bael_Transmission::bdexStreamIn(stream, value, version);
}

}  // close namespace bdex_InStreamFunctions;

// ============================================================================
//                      namespace bdex_VersionFunctions
// ============================================================================

namespace bdex_VersionFunctions {

inline
int maxSupportedVersion(bael_Transmission::Cause)
{
    return bael_Transmission::maxSupportedBdexVersion();
}

}  // close namespace bdex_VersionFunctions;

// ============================================================================
//                     namespace bdex_OutStreamFunctions
// ============================================================================

namespace bdex_OutStreamFunctions {

template <typename STREAM>
inline
STREAM& streamOut(STREAM&                         stream,
                  const bael_Transmission::Cause& value,
                  int                             version)
{
    return bael_Transmission::bdexStreamOut(stream, value, version);
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
