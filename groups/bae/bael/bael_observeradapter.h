// bael_observeradapter.h                                             -*-C++-*-
#ifndef INCLUDED_BAEL_OBSERVERADAPTER
#define INCLUDED_BAEL_OBSERVERADAPTER

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")



//@PURPOSE: Provide an adapter for existing observer implementation.
//
//@CLASSES:
//  bael_ObserverAdapter: class for adapting existing observer implementation.
//
//@SEE_ALSO: bael_observer, bael_record, bael_context
//
//@AUTHOR: Shijin Kong (skong25)
//
//@DESCRIPTION: This component inherits from 'bael_Observer', implementing the
// async publish method:
// 'publish(const bcema_SharedPtr<const bael_Record>&, const bael_Context&)'
// by calling the sync publish method:
// 'publish(const bael_Record&, const bael_Context&)'.  This component provides
// a base class for existing observers which implement sync publish method, and
// enables these observers to receive log records through the async publish
// method.  Existing observers derived from this component do not need to
// change their concrete implementations of sync publish method while we can
// safely deprecate the sync publish interface in 'bael_Observer'.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Concrete Observer Derived From 'bael_ObserverAdapter'
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// The following code fragments illustrate the essentials of defining and using
// a concrete observer inherited from 'bael_ObserverAdapter'.
//
// First define a concrete observer 'my_OstreamObserver' derived from
// 'bael_ObserverAdapter' and declare the sync publish method:
//..
//  class my_OstreamObserver : public bael_ObserverAdapter {
//    ostream& d_stream;
//
//  public:
//    explicit my_OstreamObserver(ostream& stream) : d_stream(stream) { }
//    virtual ~my_OstreamObserver();
//    virtual void publish(const bael_Record&  record,
//                         const bael_Context& context);
//  };
//..
// Then, define public methods of 'bael_OstreamObserver', including the sync
// publish method.  The sync publish method simply prints out the content of
// the record it receives.
//..
//  my_OstreamObserver::~my_OstreamObserver()
//  {
//  }
//
//  void my_OstreamObserver::publish(const bael_Record&  record,
//                                   const bael_Context& context)
//  {
//      const bael_RecordAttributes& fixedFields = record.fixedFields();
//
//      *d_stream << '\n';
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
// Now, create a 'my_OstreamObserver' object and assign the address of this
// object to a 'bael_ObserverAdapter' pointer:
//..
//  char buf[2048];
//  ostrstream out(buf, sizeof buf);
//  my_OstreamObserver    myObserver(out);
//  bael_ObserverAdapter *adapter = &myObserver;
//..
// Finally, publish three messages by calling the async publish method in
// 'bael_ObserverAdapter' which in turn calls the sync publish method defined
// in 'my_OstreamObserver':
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
//  out << ends;
//  cout << buf << endl;
//..
// The above code fragments print to 'stdout' like this:
//
//     Publish a sequence of three messages.
//
//     22FEB2012_00:12:12.000 201 31  0
//
//     22FEB2012_00:12:12.000 202 32  0
//
//     22FEB2012_00:12:12.000 203 33  0

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
    // This class provides an adaptation for existing concrete observers which
    // implement the deprecated
    // 'publish(const bael_Record&, const bael_Context&)' to receive and
    // process log record through
    // 'publish(const bcema_SharedPtr<bael_Record>&, const bael_Context&)'.

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
        // Discard any reference to a record stored by this observer.  This
        // method is called when the underlying resources of the records are
        // being released or becoming invalid.
};

// ============================================================================
//                          INLINE FUNCTION DEFINITIONS
// ============================================================================

                          // --------------------------
                          // class bael_ObserverAdapter
                          // --------------------------

// MANIPULATORS
inline
void bael_ObserverAdapter::publish(const bael_Record&  record,
                                   const bael_Context& context)
{
    BSLS_ASSERT_OPT(false);  // Should not be called
}

inline
void bael_ObserverAdapter::publish(
                            const bcema_SharedPtr<const bael_Record>& record,
                            const bael_Context&                       context)
{
    publish(*record, context);
}

inline
void bael_ObserverAdapter::clear()
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
