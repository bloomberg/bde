// bael_observeradapter.h                                             -*-C++-*-
#ifndef INCLUDED_BAEL_OBSERVERADAPTER
#define INCLUDED_BAEL_OBSERVERADAPTER

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")



//@PURPOSE: A basic asynchronous observer that adapts synchronous observer
//
//@CLASSES:
//  bael_ObserverAdapter: class for synchronous observer adaption
//
//@SEE_ALSO: bael_observer, bael_record, bael_context
//
//@AUTHOR: Shijin Kong (skong25)
//
//@DESCRIPTION: This component inherits from 'bael_Observer', implementing the
// asynchronous 'publish(const sharedptr&)' method by calling the synchronous
// 'public(const record&)' method.  This component proivdes a base class for
// other observers which implement 'public(const record&)', and enables these
// observers to receive log records through 'public(const sharedptr&)'.
// Existing observers derived from this component do not need to change their
// concrete implementations of 'public(const record&) while we can safely
// deprecate the 'public(const record&)' interface in 'bael_Observer'.
//
///USAGE
///-----
// This example shows usage of a simple concrete observer that implements
// 'publish(const record&)' to write three log records through
// 'publish(const sharedptr&)' calls.  The log record's fields, timestamp,
// process ID, and thread ID are written to an 'ostream' that is provided to
// the observer at construction.  This (trivial) functionality suffices to
// demonstrate the requisite steps for having a working observer:
//..
//     1. Define a concrete class derived from 'bael_ObserverAdapter'.
//     2. Implement the virtual 'publish(const record&)' method.
//     3. Instantiate and use an object of the concrete type to log records
//        by calling 'bael_ObserverAdapter::publish(const sharedptr&)'.
//..
// Note that the "publish attributes" object provided to the 'publish' method
// indicates, among other properties, whether the log record to be published is
// a "solo" message or whether it is one of a sequence.  In general, a useful
// observer object should incorporate the attributes information as part of
// the "publication" of the log record.  In this example, the attributes
// information is used to generate an appropriate heading for each log record
// that is printed to the observer's 'ostream'.
//
// We first define the (derived) 'my_OstreamObserver' class and implement
// its simple constructor inline (for convenience, directly within the
// derived-class definition):
//..
//     // my_ostreamobserver.h
//
//     class my_OstreamObserver : public bael_ObserverAdapter {
//         ostream& d_stream;
//
//       public:
//         using bael_ObserverAdapter::publish; // to use the async 'publish'
//         my_OstreamObserver(ostream& stream) : d_stream(stream) { }
//         virtual ~my_OstreamObserver();
//         virtual void publish(const bael_Record&  record,
//                              const bael_Context& context);
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
// We next implement the (virtual) 'publish(const record&)' method, which
// incorporates the "policy" of what it means for this observer to "publish"
// a log record.  In this example, the policy is that three log record fields
// are written to an 'ostream', along with an appropriate heading, and the
// rest of the log record is ignored.  Note that, in this implementation,
// the zero-based 'index' attribute is incremented by one before it is written,
// which produces a more natural record count:
//..
//     // my_ostreamobserver.cpp
//
//     // ...
//
//     my_OstreamObserver::~my_OstreamObserver() { }
//
//     void my_OstreamObserver::publish(const bael_Record&  record,
//                                      const bael_Context& context)
//     {
//         using namespace std;
//
//         d_stream << endl;  // skip a line
//
//         switch (context.transmissionCause()) {
//           case bael_Transmission::PASSTHROUGH: {
//             d_stream << "Single Pass-through Message:" << endl;
//           } break;
//           case bael_Transmission::TRIGGER_ALL: {
//             d_stream << "Remotely ";      // no 'break'; concatenated output
//           }
//           case bael_Transmission::TRIGGER: {
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
// We now want to use the 'my_OstreamObserver' object and its
// 'publish(const record&)' method; we illustrate this use in the body of the
// otherwise-unrealistic function 'recordPublisher', which generates the
// relevant fields of four dummy records.  Then we call the
// 'publish(const sharedptr&)' method to indirectly invoke
// 'publish(const record&)' to log records.  The first record is
// published singly (i.e., as a "Pass-through" record).  Note that we call the
// observer's 'publish(const sharedptr&)' method with a 'bael_Context' object
// appropriately initialized for a "Pass-through".  The last three records are
// published as a sequence of "Triggered" records.  Note that, in the sequenced
// output, the 'publish(const sharedptr&)' method is called with a zero-based
// 'index' attribute; in this example, the 'publish' implementation will print
// a natural-number message count equal to index + 1:
//..
//     void recordPublisher()
//     {
//         bslma_TestAllocator testAllocator(veryVeryVerbose);
//         my_OstreamObserver    myObserver(bsl::cout);
//         bdet_Datetime         now;
//         bael_RecordAttributes fixed;
//         bdem_List             emptyList;
//
//         bdetu_Epoch::convertFromTimeT(&now, time(0));
//         fixed.setTimestamp(now);
//         fixed.setProcessID(100);
//         fixed.setThreadID(0);
//
//         {
//             bcema_SharedPtr<const bael_Record> handle(
//                            new (testAllocator) bael_Record(fixed,
//                                                            emptyList,
//                                                            &testAllocator),
//                            &testAllocator);
//             myObserver.publish(handle,
//                                bael_Context(bael_Transmission::PASSTHROUGH,
//                                             0,
//                                             1));
//         }
//
//         const int NUM_MESSAGES = 3;
//         for (int n = 0; n < NUM_MESSAGES; ++n) {
//             bdetu_Epoch::convertFromTimeT(&now, time(0));
//             fixed.setTimestamp(now);
//             fixed.setProcessID(201 + n);
//             fixed.setThreadID(31 + n);
//             bcema_SharedPtr<const bael_Record> handle(
//                           new (testAllocator) bael_Record(fixed,
//                                                           emptyList,
//                                                           &testAllocator),
//                           &testAllocator);
//             myObserver.publish(handle,
//                                bael_Context(bael_Transmission::TRIGGER,
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

#ifndef INCLUDED_BAESCM_VERSION
#include <baescm_version.h>
#endif

#ifndef INCLUDED_BAEL_OBSERVER
#include <bael_observer.h>
#endif

#ifndef INCLUDED_BCEMA_SHAREDPTR
#include <bcema_sharedptr.h>
#endif

namespace BloombergLP {

class bael_Record;
class bael_Context;

                        // ==========================
                        // class bael_ObserverAdapter
                        // ==========================

class bael_ObserverAdapter : public bael_Observer {
    // This class provides an adaption for existing concrete observers to
    // receive and process log record through asynchronous 'publish'.

  public:
    // CREATORS
    virtual ~bael_ObserverAdapter();
        // Destroy this observer.

    // MANIPULATORS
    virtual void publish(const bael_Record&  record,
                         const bael_Context& context);
        // Process the specified log 'record' having the specified publishing
        // 'context'.
        //
        // DEPRECATED: replaced by 'publish(const sharedptr&, const context&)'.

    virtual void publish(const bcema_SharedPtr<const bael_Record>& record,
                         const bael_Context&                       context);
        // Process the record referred by the specified log shared pointer
        // 'record'.  The record has the specified publishing 'context'.

    virtual void clear();
        // Discard record shared pointers internally stored by this observer.
        // This method is called when the underlying resources referred by
        // these shared pointers are being released or becoming invalid.
};

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2012
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
