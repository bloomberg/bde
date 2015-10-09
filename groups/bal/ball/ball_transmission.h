// ball_transmission.h                                                -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BALL_TRANSMISSION
#define INCLUDED_BALL_TRANSMISSION

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Enumerate the set of states for log record transmission.
//
//@CLASSES:
//   ball::Transmission: namespace for log record transmission states
//
//@SEE_ALSO: ball_context, ball_observer
//
//@DESCRIPTION: This component provides a namespace, 'ball::Transmission', for
// the 'enum' type 'ball::Transmission::Cause'.  'Cause' enumerates the list of
// conditions (or states) that can cause a log record to be transmitted.  In
// addition, this component supports functions that convert the 'Cause'
// enumerators to a well-defined ASCII representation.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Syntax
///- - - - - - - - -
// The following snippets of code provide a simple illustration of
// 'ball::Transmission' usage.
//
// First create a variable 'cause' of type 'ball::Transmission::Cause' and
// initialize it to the value 'ball::Transmission::e_TRIGGER_ALL':
//..
//  ball::Transmission::Cause cause = ball::Transmission::e_TRIGGER_ALL;
//..
// Next, store a pointer to its ASCII representation in a variable 'asciiCause'
// of type 'const char *':
//..
//  const char *asciiCause = ball::Transmission::toAscii(cause);
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
//
///Example 2: Logging
/// - - - - - - - - -
// The 'Cause' enumeration defined in this component allows a logging system
// to describe the condition causing the publication of a log message.  One
// possible interpretation of three of these conditions is as follows:
//
//: 'e_PASSTHROUGH':
//:   indicate that a message is being output as a stand-alone message.
//:
//: 'e_TRIGGER':
//:   indicate that a message is being output as part of a dump of all
//:   messages archived for the current thread.
//:
//: 'e_TRIGGER_ALL':
//:    indicate that a message is being output as part of a dump of all
//:    messages archived for *all* threads.
//
// This example illustrates the use of 'ball::Transmission::Cause' by a
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
//      void publish(const char *message, ball::Transmission::Cause cause);
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
//                          ball::Transmission::Cause  cause)
//  {
//      using namespace std;
//      switch (cause) {
//        case ball::Transmission::e_PASSTHROUGH: {
//          cout << ball::Transmission::toAscii(cause) << ":\t" << message
//               << endl;
//        } break;
//        case ball::Transmission::e_TRIGGER: {
//          cout << ball::Transmission::toAscii(cause) << ":\t" << message
//               << endl;
//          cout << "\t[ dump all messages archived for current thread ]"
//               << endl;
//        } break;
//        case ball::Transmission::e_TRIGGER_ALL: {
//          cout << ball::Transmission::toAscii(cause) << ":\t" << message
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
//  logger.publish(MSG_PASSTHROUGH, ball::Transmission::e_PASSTHROUGH);
//  logger.publish(MSG_TRIGGER,     ball::Transmission::e_TRIGGER);
//  logger.publish(MSG_TRIGGER_ALL, ball::Transmission::e_TRIGGER_ALL);
//..
// The following output is produced on 'stdout':
//..
//  PASSTHROUGH:    report relatively minor problem
//  TRIGGER:        report serious thread-specific problem
//                  [ dump all messages archived for current thread ]
//  TRIGGER_ALL:    report process-impacting problem
//                  [ dump all messages archived for *all* threads ]
//..

#ifndef INCLUDED_BALSCM_VERSION
#include <balscm_version.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORM
#include <bsls_platform.h>
#endif

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif


namespace BloombergLP {

namespace ball {
                        // ========================
                        // struct Transmission
                        // ========================

struct Transmission {
    // This struct provides a namespace for enumerating the causes of the
    // transmission of a log record.

  public:
    // TYPES
    enum Cause {
        e_PASSTHROUGH        = 0,  // single record emitted; caused locally
        e_TRIGGER            = 1,  // all records emitted; caused locally
        e_TRIGGER_ALL        = 2,  // all records emitted; caused remotely
        e_MANUAL_PUBLISH     = 3,  // manually publish a single record
        e_MANUAL_PUBLISH_ALL = 4,  // manually publish all records
        e_END                = 5   // end flag for asynchronous publication

    };

    enum {
        e_LENGTH = e_MANUAL_PUBLISH_ALL + 1
    };
        // Define 'LENGTH' to be the number of consecutively-valued enumerators
        // in the range '[ e_PASSTHROUGH .. e_MANUAL_PUBLISH_ALL ]'.

  private:
    // PRIVATE CLASS METHODS
    static void print(bsl::ostream& stream, Transmission::Cause value);
        // Write to the specified 'stream' the string representation of the
        // specified enumeration 'value'.

  public:
    // CLASS METHODS
    static bsl::ostream& streamOut(bsl::ostream&       stream,
                                   Transmission::Cause value);
        // Format the specified 'value' to the specified output 'stream' and
        // return a reference to the modifiable 'stream'.

    static const char *toAscii(Transmission::Cause value);
        // Return the string representation exactly matching the enumerator
        // name corresponding to the specified enumeration 'value'.

};

// FREE OPERATORS
bsl::ostream& operator<<(bsl::ostream& stream, Transmission::Cause rhs);
    // Format the specified 'rhs' to the specified output 'stream' and return a
    // reference to the modifiable 'stream'.

// ============================================================================
//                              INLINE DEFINITIONS
// ============================================================================

                        // ------------------------
                        // struct Transmission
                        // ------------------------

// CLASS METHODS
inline
bsl::ostream& Transmission::streamOut(bsl::ostream&       stream,
                                      Transmission::Cause value)
{
    print(stream, value);
    return stream;
}

}  // close package namespace

// FREE OPERATORS
inline
bsl::ostream& ball::operator<<(bsl::ostream&       stream,
                               Transmission::Cause rhs)
{
    return Transmission::streamOut(stream, rhs);
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
