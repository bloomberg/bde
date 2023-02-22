// ball_broadcastobserver.h                                           -*-C++-*-
#ifndef INCLUDED_BALL_BROADCASTOBSERVER
#define INCLUDED_BALL_BROADCASTOBSERVER

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a broadcast observer that forwards to other observers.
//
//@CLASSES:
//  ball::BroadcastObserver: observer that forwards to other observers
//
//@SEE_ALSO: ball_record, ball_context, ball_observer,
//           ball_loggermanager
//
//@DESCRIPTION: This component provides a concrete implementation of the
// 'ball::Observer' protocol for receiving and processing log records:
//..
//                 ,-----------------------.
//                ( ball::BroadcastObserver )
//                 `-----------------------'
//                            |             ctor
//                            |             registerObserver
//                            |             deregisterObserver
//                            |             deregisterAllObservers
//                            |             findObserver
//                            |             numRegisteredObservers
//                            |             visitObservers
//                            V
//                     ,--------------.
//                    ( ball::Observer )
//                     `--------------'
//                                          dtor
//                                          publish
//                                          releaseRecords
//..
// 'ball::BroadcastObserver' is a concrete class derived from 'ball::Observer'
// that processes the log records it receives through its 'publish' method by
// forwarding them to other concrete observers.  'ball::BroadcastObserver'
// maintains a registry of named observers to which it forwards log records.
// Clients of 'ball::BroadcastObserver' register observers using the
// 'registerObserver' method and unregister observers with the
// 'deregisterObserver' method.  Once registered, an observer receives all log
// records that its associated broadcast observer receives.
//
///Thread Safety
///-------------
// 'ball::BroadcastObserver' is thread-safe, meaning that multiple threads may
// share the same instance, or may have their own instances (see
// 'bsldoc_glossary').
//
///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: Delayed Observer Configuration
///- - - - - - - - - - - - - - - - - - - - -
// In this example, we will show how 'ball::BroadcastObserver' can be used to
// implement delayed observer configuration.
//
// First, we define an elided custom observer that conforms to the
// 'ball::Observer' protocol and supports a 'configure' method:
//..
//  class ConfigurableObserver : public ball::Observer {
//      // DATA
//      bool d_configureFlag;   // configuration completion flag
//
//    public:
//      // CREATORS
//      ConfigurableObserver() : d_configureFlag(false)
//      {
//      }
//
//      // MANIPULATORS
//      void configure() {
//          // Configure this observer.
//          d_configureFlag = true;
//      }
//
//      void publish(const bsl::shared_ptr<ball::Record>& record,
//                   const ball::Context&                 context)
//          // Publish the specified 'record' with the specified 'context'.
//      {
//          // Do not publish any records until configuration has been done.
//          if (!d_configureFlag) {
//              return;                                               // RETURN
//          }
//          // Publish the record.
//          // ...
//      }
//      // ACCESSORS
//      bool isConfigured() const
//      {
//          return d_configureFlag;
//      }
//  };
//..
// Then, we create a shared pointer to a 'ConfigurableObserver' object and
// register it with a broadcast observer:
//..
//  bsl::shared_ptr<ConfigurableObserver>
//                                   myObserverPtr(new ConfigurableObserver());
//
//  ball::BroadcastObserver broadcastObserver;
//
//  int rc = broadcastObserver.registerObserver(myObserver, "observer");
//
//  assert(0 == rc);
//..
// Finally, we can retrieve the registered observer and configure it:
//..
//  bsl::shared_ptr<ConfigurableObserver> tmpObserverPtr;
//
//  rc = broadcastObserver.findObserver(&tmpObserverPtr, "observer");
//
//  assert(0 == rc);
//  assert(myObserverPtr == tmpObserverPtr);
//  assert(false == tmpObserverPtr->isConfigured());
//
//  tmpObserverPtr->configure();
//
//  assert(true == tmpObserverPtr->isConfigured());
//..
// Note that there is an alternative way to obtain a shared pointer to the
// registered observer:
//..
//  bsl::shared_ptr<Observer> oPtr =
//                                  broadcastObserver.findObserver("observer");
//
//  assert(oPtr.ptr());
//
//  bsl::shared_ptr<ConfigurableObserver> anotherObserverPtr;
//
//  bslstl::SharedPtrUtil::dynamicCast(&anotherObserverPtr, oPtr);
//
//  assert(myObserverPtr == anotherObserverPtr);
//..

#include <balscm_version.h>

#include <ball_observer.h>

#include <bslma_allocator.h>
#include <bslma_default.h>

#include <bslmf_enableif.h>
#include <bslmf_isconvertible.h>

#include <bslmt_readerwritermutex.h>
#include <bslmt_readlockguard.h>

#include <bsl_memory.h>
#include <bsl_string.h>
#include <bsl_unordered_map.h>


namespace BloombergLP {
namespace ball {

class Record;
class Context;

                         // =======================
                         // class BroadcastObserver
                         // =======================

class BroadcastObserver : public Observer {
    // This class provides a broadcasting implementation of the 'Observer'
    // protocol.  Other concrete observers may be registered and named with
    // this broadcast observer ('registerObserver' method), retrieved
    // ('findObserver' method), and unregistered ('deregisterObserver' method).
    // The 'publish' method of this class forwards the log records that it
    // receives to the 'publish' method of each registered observer.

  public:
    // TYPES
    typedef bsl::unordered_map<bsl::string,
                               bsl::shared_ptr<Observer> > ObserverRegistry;
        // This 'typedef' is an alias for the type of the registry maintained
        // by this observer.

  private:
    // DATA
    ObserverRegistry                 d_observers;  // observer registry

    mutable bslmt::ReaderWriterMutex d_rwMutex;    // protects concurrent
                                                   // access to 'd_observers'

    // NOT IMPLEMENTED
    BroadcastObserver(const BroadcastObserver&);
    BroadcastObserver& operator=(const BroadcastObserver&);

  public:
    // CREATORS
    explicit BroadcastObserver(bslma::Allocator *basicAllocator = 0);
        // Create a broadcast observer having no registered observers.
        // Optionally specify a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.

    virtual ~BroadcastObserver();
        // Destroy this broadcast observer.  Note that this method has no
        // effect on the lifetime of observers registered with this observer,
        // if any.

    // MANIPULATORS
    void deregisterAllObservers();
        // Remove all observers from the registry of this broadcast observer.

    int deregisterObserver(const bsl::string_view& observerName);
        // Remove the observer having the specified 'observerName' from the
        // registry of this broadcast observer.  Return 0 if the observer
        // having 'observerName' was successfully deregistered, and a non-zero
        // value (with no effect) otherwise.  Henceforth, the observer that had
        // 'observerName' will no longer receive log records from this
        // observer.

    bsl::shared_ptr<Observer> findObserver(
                                         const bsl::string_view& observerName);
        // Return a shared pointer to the observer having the specified
        // 'observerName' in the registry of this broadcast observer, and an
        // empty shared pointer if there is no such observer otherwise.

    template <class t_OBSERVER>
    int findObserver(
                bsl::shared_ptr<t_OBSERVER> *result,
                const bsl::string_view&      observerName,
                typename bsl::enable_if<
                    bsl::is_convertible<t_OBSERVER *, ball::Observer *>::value,
                    void *>::type = 0)
        // Load into the specified 'result' a shared pointer to the observer of
        // (template parameter) 't_OBSERVER' type having the specified
        // 'observerName' in the registry of this broadcast observer, and an
        // empty shared pointer if there is no such observer otherwise.  Return
        // 0 if a non-empty shared pointer was loaded, and a non-zero value
        // otherwise.  Note that an empty shared pointer will be loaded if
        // either no observer having 'observerName' is in the registry or the
        // observer registered with that name is not of 't_OBSERVER' type.
        //
        // The implementation is placed here in the class definition to work
        // around a Microsoft C++ compiler (version 16) bug where the
        // definition cannot be matched to the declaration when an 'enable_if'
        // is used.
    {
        bslstl::SharedPtrUtil::dynamicCast(result, findObserver(observerName));
        return *result ? 0 : 1;
    }

    using Observer::publish;  // Avoid hiding base class method.

    virtual void publish(const bsl::shared_ptr<const Record>& record,
                         const Context&                       context);
        // Process the specified log 'record' having the specified publishing
        // 'context' by forwarding 'record' and 'context' to each of the
        // observers registered with this broadcast observer.

    int registerObserver(const bsl::shared_ptr<Observer>& observer,
                         const bsl::string_view&          observerName);
        // Add the specified 'observer' with the specified 'observerName' to
        // the registry of this broadcast observer.  Return 0 if 'observer' was
        // successfully registered, and a non-zero value (with no effect)
        // otherwise.  Henceforth, this observer will forward each record it
        // receives through its 'publish' method, including the record's
        // context, to the 'publish' method of 'observer', until 'observer' is
        // deregistered.  The behavior is undefined if a cyclic reference is
        // created among registered observers.  Note that this method will fail
        // if an observer having 'observerName' is already registered.

    virtual void releaseRecords();
        // Discard any shared reference to a 'Record' object that was supplied
        // to the 'publish' method, and is held by this observer.  This
        // implementation processes 'releaseRecords' by calling
        // 'releaseRecords' on each of the registered observers.  Note that
        // this operation should be called if resources underlying the
        // previously provided shared pointers must be released.

    template <class t_VISITOR>
    void visitObservers(t_VISITOR& visitor);
        // Invoke the specified 'visitor' functor of (template parameter)
        // 't_VISITOR' type on each element in the registry of this broadcast
        // observer, supplying that functor modifiable access to each observer.
        // 'visitor' must be a functor that can be called as if it had the
        // following signature:
        //..
        //  void operator()(const bsl::shared_ptr<Observer>& observer,
        //                  const bsl::string_view&          observerName);
        //..

    // ACCESSORS
    bsl::shared_ptr<const Observer> findObserver(
                                   const bsl::string_view& observerName) const;
        // Return a shared pointer to the observer having the specified
        // 'observerName' in the registry of this broadcast observer, and an
        // empty shared pointer if there is no such observer otherwise.

    template <class t_OBSERVER>
    int findObserver(bsl::shared_ptr<const t_OBSERVER> *result,
                     const bsl::string_view&            observerName,
                     typename bsl::enable_if<
                         bsl::is_convertible<const t_OBSERVER *,
                                             const ball::Observer *>::value,
                         void *>::type = 0) const
        // Load into the specified 'result' a shared pointer to the observer of
        // (template parameter) 't_OBSERVER' type having the specified
        // 'observerName' in the registry of this broadcast observer, and an
        // empty shared pointer if there is no such observer otherwise.  Return
        // 0 if a non-empty shared pointer was loaded, and a non-zero value
        // otherwise.  Note that an empty shared pointer will be loaded if
        // either no observer having 'observerName' is in the registry or the
        // observer registered with that name is not of 't_OBSERVER' type.
        //
        // The implementation is placed here in the class definition to work
        // around a Microsoft C++ compiler (version 16) bug where the
        // definition cannot be matched to the declaration when an 'enable_if'
        // is used.
    {
        bslstl::SharedPtrUtil::dynamicCast(result, findObserver(observerName));
        return *result ? 0 : 1;
    }

    int numRegisteredObservers() const;
        // Return the number of observers registered with this broadcast
        // observer.

    template <class t_VISITOR>
    void visitObservers(const t_VISITOR& visitor) const;
        // Invoke the specified 'visitor' functor of (template parameter)
        // 't_VISITOR' type on each element in the registry of this broadcast
        // observer, supplying that functor modifiable access to each observer.
        // 'visitor' must be a functor that can be called as if it had the
        // following signature:
        //..
        //  void operator()(const bsl::shared_ptr<Observer>& observer,
        //                  const bsl::string_view&          observerName);
        //..
};

// ============================================================================
//                              INLINE DEFINITIONS
// ============================================================================

                         // -----------------------
                         // class BroadcastObserver
                         // -----------------------

// CREATORS
inline
BroadcastObserver::BroadcastObserver(bslma::Allocator *basicAllocator)
: d_observers(bslma::Default::allocator(basicAllocator))
{
}

// MANIPULATORS
template <class t_VISITOR>
inline
void BroadcastObserver::visitObservers(t_VISITOR& visitor)
{
    bslmt::ReadLockGuard<bslmt::ReaderWriterMutex> guard(&d_rwMutex);

    for (ObserverRegistry::const_iterator it = d_observers.cbegin();
         it != d_observers.cend();
         ++it) {
        visitor(it->second, it->first);
    }
}

// ACCESSORS
inline
int BroadcastObserver::numRegisteredObservers() const
{
    bslmt::ReadLockGuard<bslmt::ReaderWriterMutex> guard(&d_rwMutex);

    return static_cast<int>(d_observers.size());
}

template <class t_VISITOR>
inline
void BroadcastObserver::visitObservers(const t_VISITOR& visitor) const
{
    bslmt::ReadLockGuard<bslmt::ReaderWriterMutex> guard(&d_rwMutex);

    for (ObserverRegistry::const_iterator it = d_observers.cbegin();
         it != d_observers.cend();
         ++it) {
        visitor(it->second, it->first);
    }
}

}  // close package namespace
}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2017 Bloomberg Finance L.P.
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
