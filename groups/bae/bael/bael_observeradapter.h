// bael_observeradapter.h                                             -*-C++-*-
#ifndef INCLUDED_BAEL_OBSERVERADAPTER
#define INCLUDED_BAEL_OBSERVERADAPTER

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a helper for implementing the 'bael_Observer' protocol.
//
//@CLASSES:
//  bael_ObserverAdapter: a helper for implementing 'bael_Observer'
//
//@SEE_ALSO: bael_observer, bael_record, bael_context
//
//@AUTHOR: Shijin Kong (skong25)
//
//@DESCRIPTION: This component provides a single class 'bael_ObserverAdapter'
// that aids in the implementation of the 'bael_Observer' protocol by allowing
// clients to implement that protocol by implementing a single method
// signature: 'publish(const bael_Record&, const bael_Context&)'.  A primary
// goal of this component is to simplify the transition for older
// implementations of the 'bael_Observer' protocol (that accept
// const-references to 'bael_Record' objects) to the updated protocol (that
// accepts shared-pointers to 'bael_Record' objects).  'bael_ObserverAdapter'
// inherits from 'bael_Observer', and implements the (newer) overload of the
// 'publish' method in (accepting a shared-pointer to a record) by calling the
// overload of the 'publish' method (accepting a reference to a record).
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Concrete Observer Derived From 'bael_ObserverAdapter'
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// The following code fragments illustrate the essentials of defining and using
// a concrete observer inherited from 'bael_ObserverAdapter'.
//
// First define a concrete observer 'MyOstreamObserver' derived from
// 'bael_ObserverAdapter' that declares a single publish method accepting a
// const-reference to a 'bael_Record' object:
//..
//  class MyOstreamObserver : public bael_ObserverAdapter {
//    ostream& d_stream;
//
//  public:
//    explicit MyOstreamObserver(ostream& stream) : d_stream(stream) { }
//    virtual ~MyOstreamObserver();
//    virtual void publish(const bael_Record&  record,
//                         const bael_Context& context);
//  };
//..
// Then, we implement the public methods of 'MyOstreamObserver', including the
// 'publish' method.  This implementation of 'publish' simply prints out the
// content of the record it receives to the stream supplied at construction.
//..
//  MyOstreamObserver::~MyOstreamObserver()
//  {
//  }
//
//  void MyOstreamObserver::publish(const bael_Record&  record,
//                                  const bael_Context& context)
//  {
//      const bael_RecordAttributes& fixedFields = record.fixedFields();
//
//      *d_stream << fixedFields.timestamp()               << ' '
//                << fixedFields.processID()               << ' '
//                << fixedFields.threadID()                << ' '
//                << fixedFields.fileName()                << ' '
//                << fixedFields.lineNumber()              << ' '
//                << fixedFields.category()                << ' '
//                << fixedFields.message()                 << ' ';
//
//      const bdem_List& userFields = record.userFields();
//      const int numUserFields = userFields.length();
//      for (int i = 0; i < numUserFields; ++i) {
//          *d_stream << userFields[i] << ' ';
//      }
//
//      *d_stream << '\n' << bsl::flush;
//  }
//..
// Now, create a 'MyOstreamObserver' object and assign the address of this
// object to a 'bael_ObserverAdapter' pointer:
//..
//  char buf[2048];
//  ostrstream out(buf, sizeof buf);
//  MyOstreamObserver    myObserver(out);
//  bael_ObserverAdapter *adapter = &myObserver;
//..
// Finally, publish three messages by calling 'publish' method accepting a
// shared-pointer, provided by 'bael_ObserverAdapter', that in turn will call
// the 'publish' method defined in 'MyOstreamObserver':
//..
//  bdet_Datetime         now;
//  bael_RecordAttributes fixed;
//  bdem_List             emptyList;
//
//  cout << "Publish a sequence of three messages." << endl;
//
//  out.seekp(0);
//  const int NUM_MESSAGES = 3;
//  for (int n = 0; n < NUM_MESSAGES; ++n) {
//      bdetu_Datetime::convertFromTimeT(&now, time(0));
//      fixed.setTimestamp(now);
//      fixed.setProcessID(201 + n);
//      fixed.setThreadID(31 + n);
//
//      bcema_SharedPtr<const bael_Record> handle(
//                        new (testAllocator) bael_Record(fixed,
//                                                        emptyList,
//                                                        &testAllocator));
//      adapter->publish(handle,
//                       bael_Context(bael_Transmission::BAEL_TRIGGER,
//                                    n,
//                                    NUM_MESSAGES));
//  }
//  out << ends;
//  cout << buf << endl;
//..
// The above code fragments print to 'stdout' like this:
//..
//  Publish a sequence of three messages.
//  22FEB2012_00:12:12.000 201 31  0
//  22FEB2012_00:12:12.000 202 32  0
//  22FEB2012_00:12:12.000 203 33  0
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
    // This class aids in the implementation of the 'bael_Observer' protocol
    // by allowing clients to implement that protocol by implementing a single
    // method signature: 'publish(const bael_Record&, const bael_Context&)'.

  public:
    // CREATORS
    virtual ~bael_ObserverAdapter();
        // Destroy this observer.

    // MANIPULATORS
    virtual void publish(const bael_Record&  record,
                         const bael_Context& context) = 0;
        // Process the specified log 'record' having the specified publishing
        // 'context'.

    virtual void publish(const bcema_SharedPtr<const bael_Record>& record,
                         const bael_Context&                       context);
        // Process the record referred by the specified log shared pointer
        // 'record'.  Note that classes that derive from
        // 'bael_ObserverAdapter' should *not* implement this method.

    virtual void releaseRecords();
        // Discard any shared reference to a 'bael_Record' object that was
        // supplied to the 'publish' method and is held by this observer.
        // Note that classes that derive from 'bael_ObserverAdapter' should
        // *not* implement this method.  Also note that this operation should
        // be called if resources underlying the previously provided
        // shared-pointers must be released.
};

// ============================================================================
//                          INLINE FUNCTION DEFINITIONS
// ============================================================================

                          // --------------------------
                          // class bael_ObserverAdapter
                          // --------------------------

// MANIPULATORS
inline
void bael_ObserverAdapter::publish(
                            const bcema_SharedPtr<const bael_Record>& record,
                            const bael_Context&                       context)
{
    publish(*record, context);
}

inline
void bael_ObserverAdapter::releaseRecords()
{
}


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
