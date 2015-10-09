// ball_observeradapter.h                                             -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BALL_OBSERVERADAPTER
#define INCLUDED_BALL_OBSERVERADAPTER

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a helper for implementing the 'ball::Observer' protocol.
//
//@CLASSES:
//  ball::ObserverAdapter: a helper for implementing 'ball::Observer'
//
//@SEE_ALSO: ball_observer, ball_record, ball_context
//
//@DESCRIPTION: This component provides a single class 'ball::ObserverAdapter'
// that aids in the implementation of the 'ball::Observer' protocol by allowing
// clients to implement that protocol by implementing a single method
// signature: 'publish(const ball::Record&, const ball::Context&)'.  A primary
// goal of this component is to simplify the transition for older
// implementations of the 'ball::Observer' protocol (that accept
// const-references to 'ball::Record' objects) to the updated protocol (that
// accepts shared-pointers to 'ball::Record' objects).  'ball::ObserverAdapter'
// inherits from 'ball::Observer', and implements the (newer) overload of the
// 'publish' method in (accepting a shared-pointer to a record) by calling the
// overload of the 'publish' method (accepting a reference to a record).
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Concrete Observer Derived From 'ball::ObserverAdapter'
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// The following code fragments illustrate the essentials of defining and using
// a concrete observer inherited from 'ball::ObserverAdapter'.
//
// First define a concrete observer 'MyOstreamObserver' derived from
// 'ball::ObserverAdapter' that declares a single publish method accepting a
// const-reference to a 'ball::Record' object:
//..
//  class MyOstreamObserver : public ball::ObserverAdapter {
//    bsl::ostream  *d_stream;
//
//  public:
//    explicit MyOstreamObserver(bsl::ostream *stream) : d_stream(stream) { }
//    virtual ~MyOstreamObserver();
//    virtual void publish(const ball::Record&  record,
//                         const ball::Context& context);
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
//  void MyOstreamObserver::publish(const ball::Record&  record,
//                                  const ball::Context& context)
//  {
//      const ball::RecordAttributes& fixedFields = record.fixedFields();
//
//      *d_stream << fixedFields.timestamp()               << ' '
//                << fixedFields.processID()               << ' '
//                << fixedFields.threadID()                << ' '
//                << fixedFields.fileName()                << ' '
//                << fixedFields.lineNumber()              << ' '
//                << fixedFields.category()                << ' '
//                << fixedFields.message()                 << ' ';
//
//      const ball::UserFields& userFields = record.userFields();
//      const int numUserFields = userFields.length();
//      for (int i = 0; i < numUserFields; ++i) {
//          *d_stream << userFields[i] << ' ';
//      }
//
//      *d_stream << '\n' << bsl::flush;
//  }
//..
// Now, we defined a function 'main' in which we create a 'MyOstreamObserver'
// object and assign the address of this object to a 'ball::ObserverAdapter'
// pointer:
//..
//  int main(bool verbose)
//  {
//      bsl::ostringstream     out;
//      MyOstreamObserver      myObserver(&out);
//      ball::ObserverAdapter *adapter = &myObserver;
//..
// Finally, publish three messages by calling 'publish' method accepting a
// shared-pointer, provided by 'ball::ObserverAdapter', that in turn will call
// the 'publish' method defined in 'MyOstreamObserver':
//..
//      bdlt::Datetime         now;
//      ball::RecordAttributes fixedFields;
//      ball::UserFields       userFields;
//
//      const int NUM_MESSAGES = 3;
//      for (int n = 0; n < NUM_MESSAGES; ++n) {
//          fixedFields.setTimestamp(bdlt::CurrentTime::utc());
//
//          bsl::shared_ptr<const ball::Record> handle;
//          handle.createInplace(bslma::Default::allocator(),
//                               fixedFields,
//                               userFields);
//          adapter->publish(handle,
//                           ball::Context(ball::Transmission::e_TRIGGER,
//                                         n,
//                                         NUM_MESSAGES));
//      }
//      if (verbose) {
//          bsl::cout << out.str() << bsl::endl;
//      }
//      return 0;
//  }
//..
// The above code fragments print to 'stdout' like this:
//..
//  Publish a sequence of three messages.
//  22FEB2012_00:12:12.000 201 31  0
//  22FEB2012_00:12:12.000 202 32  0
//  22FEB2012_00:12:12.000 203 33  0
//..

#ifndef INCLUDED_BALSCM_VERSION
#include <balscm_version.h>
#endif

#ifndef INCLUDED_BALL_OBSERVER
#include <ball_observer.h>
#endif

#ifndef INCLUDED_BSL_MEMORY
#include <bsl_memory.h>
#endif

namespace BloombergLP {


namespace ball {

class Record;
class Context;

                        // =====================
                        // class ObserverAdapter
                        // =====================

class ObserverAdapter : public Observer {
    // This class aids in the implementation of the 'Observer' protocol
    // by allowing clients to implement that protocol by implementing a single
    // method signature: 'publish(const Record&, const Context&)'.

  public:
    // CREATORS
    virtual ~ObserverAdapter();
        // Destroy this observer.

    // MANIPULATORS
    virtual void publish(const Record&  record, const Context& context) = 0;
        // Process the specified log 'record' having the specified publishing
        // 'context'.

    virtual void publish(const bsl::shared_ptr<const Record>& record,
                         const Context&                       context);
        // Process the record referred by the specified log shared pointer
        // 'record'.  Note that classes that derive from 'ObserverAdapter'
        // should *not* implement this method.

    virtual void releaseRecords();
        // Discard any shared reference to a 'Record' object that was supplied
        // to the 'publish' method and is held by this observer.  Note that
        // classes that derive from 'ObserverAdapter' should *not* implement
        // this method.  Also note that this operation should be called if
        // resources underlying the previously provided shared-pointers must be
        // released.
};

// ============================================================================
//                              INLINE DEFINITIONS
// ============================================================================

                          // ---------------------
                          // class ObserverAdapter
                          // ---------------------

// MANIPULATORS
inline
void ObserverAdapter::publish(
                            const bsl::shared_ptr<const Record>& record,
                            const Context&                       context)
{
    publish(*record, context);
}

inline
void ObserverAdapter::releaseRecords()
{
}

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
