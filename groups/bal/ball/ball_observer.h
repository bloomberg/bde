// ball_observer.h                                                    -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BALL_OBSERVER
#define INCLUDED_BALL_OBSERVER

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Define a protocol for receiving and processing log records.
//
//@CLASSES:
//  ball::Observer: protocol class for receiving and processing log records
//
//@SEE_ALSO: ball_record, ball_loggermanager
//
//@DESCRIPTION: This component defines the base-level protocol,
// 'ball::Observer', for receiving and processing log records.  Concrete types
// derived from this protocol, receive log records, and process them in a
// manner defined by the derived class author.
//
///Usage
///-----
// This example shows the definition and use of a simple concrete observer that
// writes three of the log record's fields, timestamp, process ID, and thread
// ID, to an 'ostream' that is provided to the observer at construction.  This
// (trivial) functionality suffices to demonstrate the requisite steps for
// having a working observer:
//
//: 1 Define a concrete class derived from 'ball::Observer'.
//: 2 Implement the pure virtual 'publish' method.
//: 3 Instantiate and use an object of the concrete type.
//
// Note that the "publish attributes" object provided to the 'publish' method
// indicates, among other properties, whether the log record to be published is
// a "solo" message or whether it is one of a sequence.  In general, a useful
// observer object should incorporate the attributes information as part of
// the "publication" of the log record.  In this example, the attributes
// information is used to generate an appropriate heading for each log record
// that is printed to the observer's 'ostream'.
//
// We first define the (derived) 'my_OstreamObserver' class and implement its
// simple constructor inline (for convenience, directly within the
// derived-class definition):
//..
//     // my_ostreamobserver.h
//
//     class my_OstreamObserver : public ball::Observer {
//         ostream& d_stream;
//
//       public:
//         my_OstreamObserver(ostream& stream) : d_stream(stream) { }
//         virtual ~my_OstreamObserver();
//         virtual void publish(const ball::Record&  record,
//                              const ball::Context& context);
//     };
//..
// Note, however, that we always implement a virtual destructor (non-inline)
// in the .cpp file (to indicate the *unique* location of the class's virtual
// table):
//..
//     // my_ostreamobserver.cpp
//
//     // ...
//
//     my_OstreamObserver::~my_OstreamObserver() { }
//..
// We next implement the (virtual) 'publish' method, which incorporates the
// "policy" of what it means for this observer to "publish" a log record.  In
// this example, the policy is that three log record fields are written to an
// 'ostream', along with an appropriate heading, and the rest of the log record
// is ignored.  Note that, in this implementation, the zero-based 'index'
// attribute is incremented by one before it is written, which produces a more
// natural record count:
//..
//     // my_ostreamobserver.cpp
//
//     // ...
//
//     my_OstreamObserver::~my_OstreamObserver() { }
//
//     void my_OstreamObserver::publish(const ball::Record&  record,
//                                      const ball::Context& context)
//     {
//         using namespace std;
//
//         d_stream << endl;  // skip a line
//
//         switch (context.transmissionCause()) {
//           case ball::Transmission::PASSTHROUGH: {
//             d_stream << "Single Pass-through Message:" << endl;
//           } break;
//           case ball::Transmission::TRIGGER_ALL: {
//             d_stream << "Remotely ";      // no 'break'; concatenated output
//           }
//           case ball::Transmission::TRIGGER: {
//             d_stream << "Triggered Publication Sequence: Message "
//                      << context.recordIndex() + 1  // Account for 0-based
//                                                    // index.
//                      << " of " << context.sequenceLength()
//                      << ':' << endl;
//           } break;
//           default: {
//             d_stream << "***ERROR*** Unknown Message Cause:" << endl;
//             return;
//           } break;
//         }
//
//         d_stream << "\tTimestamp:  " << record.fixedFields().timestamp()
//                  << endl;
//         d_stream << "\tProcess ID: " << record.fixedFields().processID()
//                  << endl;
//         d_stream << "\tThread ID:  " << record.fixedFields().threadID()
//                  << endl;
//     }
//..
// We now want to use the 'my_OstreamObserver' object and its 'publish' method;
// we illustrate this use in the body of the otherwise-unrealistic function
// 'recordPublisher', which generates the relevant fields of four dummy
// records.  The first record is published singly (i.e., as a "Pass-through"
// record).  Note that we call the observer's 'publish' method with a
// 'ball::Context' object appropriately initialized for a "Pass-through".  The
// last three records are published as a sequence of "Triggered" records.  Note
// that, in the sequenced output, the 'publish' method is called with a
// zero-based 'index' attribute; in this example, the 'publish' implementation
// will print a natural-number message count equal to index + 1:
//..
//     void recordPublisher()
//     {
//         my_OstreamObserver     myObserver(bsl::cout);
//         bdlt::Datetime         now;
//         ball::RecordAttributes fixed;
//         ball::UserFields       emptyList;
//
//         bdlt::EpochUtil::convertFromTimeT(&now, time(0));
//         fixed.setTimestamp(now);
//         fixed.setProcessID(100);
//         fixed.setThreadID(0);
//         myObserver.publish(ball::Record(fixed, emptyList),
//                            ball::Context(ball::Transmission::PASSTHROUGH,
//                                         0,
//                                         1));
//
//         const int NUM_MESSAGES = 3;
//         for (int n = 0; n < NUM_MESSAGES; ++n) {
//             bdlt::EpochUtil::convertFromTimeT(&now, time(0));
//             fixed.setTimestamp(now);
//             fixed.setProcessID(201 + n);
//             fixed.setThreadID(31 + n);
//             myObserver.publish(ball::Record(fixed, emptyList),
//                                ball::Context(ball::Transmission::TRIGGER,
//                                             n,
//                                             NUM_MESSAGES));
//         }
//     }
//..
// 'recordPublisher', when invoked, prints the following to 'stdout':
//..
//     Single Pass-through Message:
//             Timestamp:  15JAN2004_23:12:59.000
//             Process ID: 100
//             Thread ID:  0
//
//     Triggered Publication Sequence: Message 1 of 3:
//             Timestamp:  15JAN2004_23:12:59.000
//             Process ID: 201
//             Thread ID:  31
//
//     Triggered Publication Sequence: Message 2 of 3:
//             Timestamp:  15JAN2004_23:12:59.000
//             Process ID: 202
//             Thread ID:  32
//
//     Triggered Publication Sequence: Message 3 of 3:
//             Timestamp:  15JAN2004_23:12:59.000
//             Process ID: 203
//             Thread ID:  33
//..

#ifndef INCLUDED_BALSCM_VERSION
#include <balscm_version.h>
#endif

#ifndef INCLUDED_BSL_MEMORY
#include <bsl_memory.h>
#endif

namespace BloombergLP {

namespace ball {

class Record;
class Context;

                           // ==============
                           // class Observer
                           // ==============

class Observer {
    // This class provides a protocol for receiving and processing log record
    // output.

  public:
    // CREATORS
    virtual ~Observer();
        // Destroy this observer.

    // MANIPULATORS
    virtual void publish(const Record&  record,
                         const Context& context);
        // Process the specified log 'record' having the specified publishing
        // 'context'.
        //
        // DEPRECATED: use the alternative 'publish' overload instead.

    virtual void publish(const bsl::shared_ptr<const Record>& record,
                         const Context&                       context);
        // Process the specified log 'record' having the specified publishing
        // 'context'.  The exact definition of publish depends on the
        // implementing class, though the intention is that the log 'record'
        // (whose publication has occurred according to 'context') be
        // distributed in a human or machine readable form.

    virtual void releaseRecords();
        // Discard any shared reference to a 'Record' object that was supplied
        // to the 'publish' method, and is held by this observer.  Note that
        // this operation should be called if resources underlying the
        // previously provided shared-pointers must be released.
};

}  // close package namespace

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
