// bcecs_callbackregistry.h                                           -*-C++-*-
#ifndef INCLUDED_BCECS_CALLBACKREGISTRY
#define INCLUDED_BCECS_CALLBACKREGISTRY

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a template for a registry of callbacks grouped by category.
//
//@CLASSES:
//    bcecs_CallbackRegistry: a template for a registry of callback functions
//
//@AUTHOR: David Rubin (drubin6)
//
//@SEE_ALSO:
//
//@DESCRIPTION: This component defines a template for a registry of callbacks,
// parameterized by an Event type, an Event Category type, and a Callback
// Identifier type.  Each callback is registered with a client-specified
// identifier, and is associated with a event category.  Any number of
// callbacks may be registered for a particular event category.  Each
// callback identifier must unique within a category list, although the same
// callback may be registered for several different category types.  The
// callback functions are parameterized by the event type.  A client can
// "notify" the registry that a particular event (with a corresponding event
// type) has occurred, at which point all registered callbacks for that event
// type are called, in no particular order, with the specified event in the
// client's thread.  Clients may also deregister individual callbacks, or they
// may deregister all callbacks for a particular event type.  The callback
// registry is thread safe, meaning callbacks can be registered, deregistered,
// and executed from different threads.  However, the callback registry
// guarantees that once a callback has been deregistered, that callback will
// not be executed.  In particular, this means that if a client deregisters a
// callback in one thread while the callback is executing in another thread,
// the deregister call will block all callback executions are complete
// (including those initiated after the deregister call and while this call is
// blocking), and that once the deregister call has succeeded, no callbacks
// will execute.
//
///Usage Examples
///--------------
// The following usage example shows how 'bcecs_CallbackRegistry' can be
// used to implement a simple application using standard vocabulary types.
//
///Example 1
///- - - - -
// This example illustrates the basic use of the 'bcecs_CallbackRegistry' to
// simulate a portion of an email filtering application.  The application
// stores various filters, implemented as callbacks, under categories
// corresponding to user account names.  Events represent email headers in
// the form
//..
//   <recipient@host> <sender@host> <date> <subject>
//..
// We instantiate a registry, register several callbacks under different
// categories, and then notify those categories with various events.  In this
// example, our event type, category type, and callback ID type are all
// 'bsl::string'.  First, we define some helper functions and filters:
//..
//  int parseDateTime(const char *dateString, bdet_Datetime *result)
//  {
//      // Parse the string 'dateString' into day, month, year, hour, and
//      // minute; and store the results into the 'bdet_Datetime' object
//      // specified by 'result'.  Return 0 on success, and a non-zero value
//      // otherwise.
//
//      int  year, month, day, hour, minute;
//      char mon[4] = {0};
//
//      int rc = bsl::sscanf(dateString, "%2d%3s%4d_%2d:%2d",
//                           &day, mon, &year, &hour, &minute);
//      if (5 != rc) {
//          return rc ? rc : -1;
//      }
//
//      const char *months = "JAN FEB MAR APR JUN JUL SEP OCT NOV DEC";
//      const char *p      = bsl::strstr(months, mon);
//      month = p ? (p - months) / 4 : -1;
//
//      rc = result->validateAndSetDatetime(year, month, day, hour, minute);
//      return rc ? -2 : 0;
//  }
//
//  void deleteCallback(const bsl::string& message, const char *sender)
//  {
//      // Delete 'message' if 'sender' matches the sender specified in
//      // 'message'.
//
//      bsl::string::size_type pos = message.find_first_of(' ');
//      if (0 == message.compare(pos + 1, bsl::strlen(sender), sender)) {
//          bsl::cout << "\tDelete message." << bsl::endl;
//      }
//  }
//
//  void moveCallback(const bsl::string& message,
//                    const char        *subject,
//                    const char        *folder)
//  {
//      // Move 'message' to the specified 'folder' if the 'message' subject
//      // contains the specified 'subject'.
//
//      bsl::string::size_type pos = message.find_first_of(' ');
//      pos = message.find_first_of(' ', pos + 1);  // space after sender
//      pos = message.find_first_of(' ', pos + 1);  // space after date
//
//      if (bsl::string::npos != message.find(subject, pos)) {
//          bsl::cout << "\tMove message to folder '" << folder << "'."
//                    << bsl::endl;
//      }
//  }
//
//  void flagCallback(const bsl::string& message, const bdet_Datetime& date)
//  {
//      // Flag 'message' if its date is later than the specified 'date'.
//
//      bsl::string::size_type pos = message.find_first_of(' ');
//      pos = message.find_first_of(' ', pos + 1);  // space after sender
//
//      bdet_Datetime sendDate;
//      if (0 == parseDateTime(message.c_str() + pos + 1, &sendDate)) {
//          if (sendDate < date) {
//              bsl::cout << "\tFlag message." << bsl::endl;
//          }
//      }
//  }
//..
// Next, we instantiate the registry, bind various callbacks, and register
// them for various users.
//..
//  typedef bcecs_CallbackRegistry<bsl::string,
//                                 bsl::string,
//                                 bsl::string> RegistryType;
//      // This typedef is provided for notational convenience when
//      // declaring callbacks usable by the registry.
//
//  RegistryType registry;
//
//  // Set up filters for user "jimbo@bloomberg.net".
//
//  const char *sender = "erefson@bloomberg.net";
//  RegistryType::Callback refson =
//      bdef_BindUtil::bindA(&ta, &deleteCallback, _1, sender);
//
//  RegistryType::Callback nlrt =
//      bdef_BindUtil::bindA( &ta
//                          , &moveCallback, _1
//                          , (const char *)"TODAY'S NEWS"
//                          , (const char *)"NLRT");
//
//  registry.registerCallback("jimbo@bloomberg.net", refson, "refson");
//  registry.registerCallback("jimbo@bloomberg.net", nlrt, "nlrt");
//
//  // Set up filters for user "ronaldo7@bloomberg.net".
//
//  bdet_Datetime          flagDate;
//  bdetu_Datetime::convertFromTimeT(&flagDate, bsl::time(0));
//  flagDate.addDays(-5);
//
//  RegistryType::Callback fiveDays =
//      bdef_BindUtil::bindA(&ta, &flagCallback, _1, flagDate);
//
//  registry.registerCallback("ronaldo7@bloomberg.net",
//  fiveDays, "Five Day Filter");
//
//  // Iterate over a set of email message headers, and notify the
//  // corresponding recipient in the registry.
//
//  const bsl::string MESSAGES[] = {
//      "jimbo@bloomberg.net erefson@bloomberg.net "
//      "16SEP2004_02:13 Interested in the business of football?",
//      "ronaldo7@bloomberg.net erefson@bloomberg.net "
//      "16SEP2004_02:13 Interested in the business of football?",
//      "ronaldo9@bloomberg.net erefson@bloomberg.net "
//      "16SEP2004_02:13 Interested in the business of football?",
//      "ronaldo7@bloomberg.net ronaldo9@bloomberg.net "
//      "28SEP2004_23:11 Real Madrid 4 - 2 Roma !",
//      "ronaldo9@bloomberg.net ronaldo7@bloomberg.net "
//      "28SEP2004_23:13 Man. United 6 - 2 Fenerbahce :-)",
//      "jimbo@bloomberg.net nlrt@bloomberg.net "
//      "08OCT2004_16:02 TODAY'S NEWS: 5 News Articles",
//      "ronaldo7@bloomberg.net nlrt@bloomberg.net "
//      "08OCT2004_16:02 TODAY'S NEWS: 5 News Articles",
//      "ronaldo9@bloomberg.net nlrt@bloomberg.net "
//      "08OCT2004_16:02 TODAY'S NEWS: 5 News Articles",
//      "jimbo@bloomberg.net erefson@bloomberg.net "
//      "15OCT2004_07:34 "
//      "Another benefit of BLAW for your financial clients:",
//      "ronaldo7@bloomberg.net erefson@bloomberg.net "
//      "15OCT2004_07:34 "
//      "Another benefit of BLAW for your financial clients:",
//      "ronaldo9@bloomberg.net erefson@bloomberg.net "
//      "15OCT2004_07:34 "
//      "Another benefit of BLAW for your financial clients:",
//      "jimbo@bloomberg.net admin@testhost.com 15OCT2004_12:45 test",
//      "ronaldo7@bloomberg.net admin@testhost.com 15OCT2004_12:45 test",
//      "ronaldo9@bloomberg.net admin@testhost.com 15OCT2004_12:45 test",
//  };
//  enum { NUM_MESSAGES = sizeof MESSAGES / sizeof *MESSAGES };
//
//  for (int i = 0; i < NUM_MESSAGES; ++i) {
//      const bsl::string&     MESSAGE = MESSAGES[i];
//      bsl::string::size_type pos     = MESSAGE.find_first_of(' ');
//      bsl::string            recipient(MESSAGE.substring(0, pos));
//
//      bsl::cout << MESSAGE << bsl::endl;
//      registry.notify(recipient, MESSAGE);
//  }
//..

#ifndef INCLUDED_BCESCM_VERSION
#include <bcescm_version.h>
#endif

#ifndef INCLUDED_BCEMT_READERWRITERLOCK
#include <bcemt_readerwriterlock.h>
#endif

#ifndef INCLUDED_BCEC_OBJECTPOOL
#include <bcec_objectpool.h>
#endif

#ifndef INCLUDED_BCES_ATOMICTYPES
#include <bces_atomictypes.h>
#endif

#ifndef INCLUDED_BDEF_FUNCTION
#include <bdef_function.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITS
#include <bslalg_typetraits.h>
#endif

#ifndef INCLUDED_BSLMA_ALLOCATOR
#include <bslma_allocator.h>
#endif

#ifndef INCLUDED_BSLMA_DEFAULT
#include <bslma_default.h>
#endif

#ifndef INCLUDED_BDEMA_MANAGEDPTR
#include <bdema_managedptr.h>
#endif

#ifndef INCLUDED_BSL_FUNCTIONAL
#include <bsl_functional.h>
#endif

#ifndef INCLUDED_BSL_MAP
#include <bsl_map.h>
#endif

#ifndef INCLUDED_BSL_UTILITY
#include <bsl_utility.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

namespace BloombergLP {

                        // ============================
                        // class bcecs_CallbackRegistry
                        // ============================

template <
          typename EVENT,
          typename CATEGORY,
          typename CALLBACK_ID,
          typename CATEGORY_COMPARATOR = bsl::less<CATEGORY>,
          typename CALLBACK_COMPARATOR = bsl::less<CALLBACK_ID>
         >
class bcecs_CallbackRegistry {
    // This class defines a template for a registry of callback functions.
    // Each function is parameterized by the 'EVENT' type.  Callbacks may be
    // registered under various 'CATEGORY' groups, and each callback
    // is identified by a 'CALLBACK_ID', which is unique within a particular
    // event category, but which may be shared between different event
    // categories.  Callbacks may also be deregistered, either individually,
    // or by event category.  Internally, categories are maintained in an
    // ordered associative map whose order is determined by the binary
    // predicate 'CATEGORY_ORDER_OP'.  Similarly, callbacks are maintained in
    // an ordered associative map whose order is determined by the binary
    // predicate 'CALLBACK_ORDER_OP'.

  public:
    // TYPES
    typedef bdef_Function<void (*)(EVENT)> Callback;
        // This type defines a callback, which may be registered with this
        // callback registry.  Each callback is parameterized by the registry
        // event type.

  private:
    // TYPES
    class CallbackWrapper {
        // This class encapsulates a user-specified callback with
        // various mechanisms needed to manage the callback vis-a-vis
        // the behavior guaranteed by 'bcecs_CallbackRegistry' when
        // deregistering callbacks.

        Callback       d_callback;
        bces_SpinLock  d_lock;        // synchronizes access to caller count
        bces_AtomicInt d_numCallers;  // counts the number of active callers

      public:
        // CREATORS
        CallbackWrapper();
            // Create a 'CallbackWrapper' object with an unbound callback.
            // The callback must be set by calling 'setCallback' prior to
            // executing the 'execute' function.

        ~CallbackWrapper();
            // Destroy this callback wrapper.

        // MANIPULATORS
        void reset();
            // Reset the value of this wrapper so that the callback is unbound
            // and the caller count is cleared.  The behavior is undefined
            // unless 'execute' is not currently running in any thread.

        void execute(const EVENT& event);
            // Call the callback maintained by this wrapper with the
            // specified 'event'.

        void setCallback(const Callback& callback);
            // Set the callback maintained by this wrapper to the
            // specified 'callback'.
    };

    typedef bsl::map<CALLBACK_ID, CallbackWrapper*, CALLBACK_COMPARATOR>
                                                          CallbackRegistryType;
        // This type defines an individual registry element.  The first
        // parameter specifies a binary predicate which defines a partial
        // ordering on the callback ID type.  The second parameter specifies
        // a callback identifier.  The third parameter specifies the
        // associated callback, along with some managerial infrastructure.

    typedef bsl::map<CATEGORY, CallbackRegistryType, CATEGORY_COMPARATOR>
                                                          CategoryRegistryType;
        // This type defines the top-level structure of the callback registry.
        // The first parameter specifies a binary predicate which defines a
        // partial ordering on the category type.  The second parameter
        // specifies an event category.  The third parameter specifies the
        // value type.  Any number of 'RegistryValue's can be associated with
        // a particular event category.

  private:
    // PRIVATE TYPES
    typedef  bcec_ObjectPool<CallbackWrapper,
                             bcec_ObjectPoolFunctors::DefaultCreator,
                             bcec_ObjectPoolFunctors::Reset<CallbackWrapper> >
                                           CallbackWrapperPool;

    // INSTANCE DATA
    bslma_Allocator      *d_allocator_p;     // memory allocator (held)

    CallbackWrapperPool   d_pool;            // pool of callback wrappers

    CategoryRegistryType  d_registry;        // registry of categories

    mutable bcemt_ReaderWriterLock
                          d_registryLock;    // synchronizes access to registry

    CALLBACK_COMPARATOR   d_callbackComparator;  // defines partial ordering

  private:
    // NOT IMPLEMENTED
    bcecs_CallbackRegistry(const bcecs_CallbackRegistry&);
    bcecs_CallbackRegistry& operator=(const bcecs_CallbackRegistry&);

  public:
    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(bcecs_CallbackRegistry,
                                 bslalg_TypeTraitUsesBslmaAllocator);

    // CREATORS
    bcecs_CallbackRegistry(const CATEGORY_COMPARATOR& categoryComparator,
                           const CALLBACK_COMPARATOR& callbackComparator,
                           bslma_Allocator           *basicAllocator = 0);
        // Create a 'bcecs_CallbackRegistry' using the specified
        // 'categoryComparator' and 'callbackComparator' predicates to define
        // a partial ordering on categories and callback IDs respectively.
        // Optionally specify a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0, the default memory allocator is used.

    bcecs_CallbackRegistry(bslma_Allocator *basicAllocator = 0);
        // Create a 'bcecs_CallbackRegistry'.  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the default memory allocator is used.

    ~bcecs_CallbackRegistry();
        // Destroy this callback registry.

    // MANIPULATORS
    int registerCallback(const CATEGORY&    category,
                         const Callback&    callback,
                         const CALLBACK_ID& id);
        // Register the specified 'callback' under the specified 'category'
        // with the specified 'id'.  Return 0 if successful, and a non-zero
        // value otherwise.  This call will fail if a callback with the
        // specified 'id' is already registered for the specified 'category'.
        // Note that the 'category' will be established if it does not already
        // exist.

    int deregisterCallback(const CATEGORY& category, const CALLBACK_ID& id);
        // Deregister the callback associated with the specified 'id' from the
        // specified 'category'.  Return 0 if successful, and a non-zero value
        // otherwise.  If the callback associated with 'id' is executing in
        // another thread at the time of this call, this call will block until
        // all callback executions (including those initiated while the
        // deregister call is blocking) are complete.  Note that if 'id'
        // indicates the only registered callback for the 'category', the
        // 'category' itself will not be removed from the registry.

    int deleteCategory(const CATEGORY& category);
        // Deregister all callbacks from the specified 'category', and
        // remove the 'category' from the registry.  Return 0 if successful,
        // and a non-zero value otherwise.  This call will block until all
        // callbacks registered under this category that are executing in
        // other threads at the time of this call and while this call
        // is blocking complete.

    int notify(const CATEGORY& category, const EVENT& event);
        // Call all callbacks registered under the specified 'category'
        // with the specified 'event'.  Return 0 if 'category' exists,
        // and a non-zero value otherwise.

    // ACCESSORS
    int numCategories(const CALLBACK_ID& id) const;
        // Return the number of categories, as a non-negative integer, in which
        // the callback identified by the specified 'id' appears.  Return -1 if
        // there is no registered callback corresponding to 'id' in the
        // registry.

    int numCallbacks(const CATEGORY& category) const;
        // Return the number of callbacks registered for the specified
        // 'category' as a non-negative integer.  Return -1 if 'category'
        // does not exist.

    int totalNumCategories() const;
        // Return a snapshot of the total number of categories present in the
        // callback registry.

    int totalNumCallbacks() const;
        // Return the total number of callbacks in the registry.  Note that
        // some callbacks may be counted twice since the same callback may
        // be registered under different event categories.
};

// ============================================================================
//                          INLINE FUNCTION DEFINITIONS
// ============================================================================

               // ---------------------------------------------
               // class bcecs_CallbackRegistry::CallbackWrapper
               // ---------------------------------------------
// CREATORS

template <
          typename EVENT,
          typename CATEGORY,
          typename CALLBACK_ID,
          typename CATEGORY_COMPARATOR,
          typename CALLBACK_COMPARATOR
         >
inline
bcecs_CallbackRegistry<EVENT,
                            CATEGORY,
                            CALLBACK_ID,
                            CATEGORY_COMPARATOR,
                            CALLBACK_COMPARATOR>::CallbackWrapper::
    CallbackWrapper()
: d_numCallers(0)
{
}

template <
          typename EVENT,
          typename CATEGORY,
          typename CALLBACK_ID,
          typename CATEGORY_COMPARATOR,
          typename CALLBACK_COMPARATOR
         >
inline
bcecs_CallbackRegistry<EVENT,
                       CATEGORY,
                       CALLBACK_ID,
                       CATEGORY_COMPARATOR,
                       CALLBACK_COMPARATOR>::CallbackWrapper::
    ~CallbackWrapper()
{
    // Only one thread will call this function through the
    // callback registry, after which other threads will fail
    // to find the callback in the registry.

    d_lock.lock();
    d_numCallers = -d_numCallers;
    d_lock.unlock();

    while (0 != d_numCallers)
            ;                        // SPIN
}

// MANIPULATORS

template <
          typename EVENT,
          typename CATEGORY,
          typename CALLBACK_ID,
          typename CATEGORY_COMPARATOR,
          typename CALLBACK_COMPARATOR
         >
inline
void bcecs_CallbackRegistry<EVENT,
                           CATEGORY,
                           CALLBACK_ID,
                           CATEGORY_COMPARATOR,
                           CALLBACK_COMPARATOR>::CallbackWrapper::
     reset()
{
   int rc = d_lock.tryLock(1);
   BSLS_ASSERT(0 == rc);
   d_callback = Callback();
   d_numCallers = 0;
   d_lock.unlock();
}

template <
          typename EVENT,
          typename CATEGORY,
          typename CALLBACK_ID,
          typename CATEGORY_COMPARATOR,
          typename CALLBACK_COMPARATOR
         >
inline
void bcecs_CallbackRegistry<EVENT,
                           CATEGORY,
                           CALLBACK_ID,
                           CATEGORY_COMPARATOR,
                           CALLBACK_COMPARATOR>::CallbackWrapper::
    execute(const EVENT& event)
{
    d_lock.lock();
    if (0 > d_numCallers) {
        // This callback is marked for deletion
        d_lock.unlock();
        return;
    }
    ++d_numCallers;
    d_lock.unlock();

    d_callback(event);

    d_numCallers -= (0 > d_numCallers) ? -1 : 1;
}

template <
          typename EVENT,
          typename CATEGORY,
          typename CALLBACK_ID,
          typename CATEGORY_COMPARATOR,
          typename CALLBACK_COMPARATOR
         >
inline
void bcecs_CallbackRegistry<EVENT,
                           CATEGORY,
                           CALLBACK_ID,
                           CATEGORY_COMPARATOR,
                           CALLBACK_COMPARATOR>::CallbackWrapper::
    setCallback(const Callback& callback)
{
    d_callback = callback;
}

                        // ----------------------------
                        // class bcecs_CallbackRegistry
                        // ----------------------------

// CREATORS

template <
          typename EVENT,
          typename CATEGORY,
          typename CALLBACK_ID,
          typename CATEGORY_COMPARATOR,
          typename CALLBACK_COMPARATOR
         >
bcecs_CallbackRegistry<EVENT,
                       CATEGORY,
                       CALLBACK_ID,
                       CATEGORY_COMPARATOR,
                       CALLBACK_COMPARATOR>::bcecs_CallbackRegistry(
        const CATEGORY_COMPARATOR& categoryComparator,
        const CALLBACK_COMPARATOR& callbackComparator,
        bslma_Allocator           *basicAllocator)
: d_allocator_p(bslma_Default::allocator(basicAllocator))
, d_registry(categoryComparator, basicAllocator)
, d_callbackComparator(callbackComparator)
{
}

template <
          typename EVENT,
          typename CATEGORY,
          typename CALLBACK_ID,
          typename CATEGORY_COMPARATOR,
          typename CALLBACK_COMPARATOR
         >
bcecs_CallbackRegistry<EVENT,
                       CATEGORY,
                       CALLBACK_ID,
                       CATEGORY_COMPARATOR,
                       CALLBACK_COMPARATOR>::bcecs_CallbackRegistry(
        bslma_Allocator *basicAllocator)
: d_allocator_p(bslma_Default::allocator(basicAllocator))
, d_registry(CATEGORY_COMPARATOR(), basicAllocator)
, d_callbackComparator(CALLBACK_COMPARATOR())
{
}

template <
          typename EVENT,
          typename CATEGORY,
          typename CALLBACK_ID,
          typename CATEGORY_COMPARATOR,
          typename CALLBACK_COMPARATOR
         >
bcecs_CallbackRegistry<EVENT,
                       CATEGORY,
                       CALLBACK_ID,
                       CATEGORY_COMPARATOR,
                       CALLBACK_COMPARATOR>::~bcecs_CallbackRegistry()
{
    d_registry.clear();
}

// MANIPULATORS

template <
          typename EVENT,
          typename CATEGORY,
          typename CALLBACK_ID,
          typename CATEGORY_COMPARATOR,
          typename CALLBACK_COMPARATOR
         >
int bcecs_CallbackRegistry<EVENT,
                           CATEGORY,
                           CALLBACK_ID,
                           CATEGORY_COMPARATOR,
                           CALLBACK_COMPARATOR>::registerCallback(
        const CATEGORY&    category,
        const Callback&    callback,
        const CALLBACK_ID& id)
{
    int rc = 1;

    d_registryLock.lockReadReserveWrite();
    typename CategoryRegistryType::iterator it = d_registry.find(category);
    if (it == d_registry.end()) {
        // The 'category' is not found.  Create a new callback registry
        // for 'category', populate it with the 'callback', and associate
        // the 'callback' with 'id'.

        d_registryLock.upgradeToWriteLock();
        CallbackRegistryType callbackRegistry(d_callbackComparator,
                                              d_allocator_p);

        // This wrapper is a managed ptr so the object will be returned
        // to the pool in case of exception
        bdema_ManagedPtr<CallbackWrapper> wrapper(d_pool.getObject(), &d_pool);
        wrapper->setCallback(callback);

        callbackRegistry.insert(bsl::make_pair(id, wrapper.ptr()));
        d_registry.insert(bsl::make_pair(category, callbackRegistry));
        wrapper.release();
        rc = 0;
    }
    else {
        typename CallbackRegistryType::const_iterator jt = it->second.find(id);
        if (jt == it->second.end()) {
            // The 'category' was found, but there is no callback
            // associated with 'id'.  Create a new entry in the callback
            // registry for the 'callback', and associate it with 'id'.

            // This wrapper is a managed ptr so the object will be returned
            // to the pool in case of exception
            bdema_ManagedPtr<CallbackWrapper> wrapper(d_pool.getObject(),
                                                      &d_pool);
            wrapper->setCallback(callback);

            d_registryLock.upgradeToWriteLock();
            it->second.insert(bsl::make_pair(id, wrapper.ptr()));
            wrapper.release();
            rc = 0;
        }
    }
    d_registryLock.unlock();
    return rc;
}

template <
          typename EVENT,
          typename CATEGORY,
          typename CALLBACK_ID,
          typename CATEGORY_COMPARATOR,
          typename CALLBACK_COMPARATOR
         >
int bcecs_CallbackRegistry<EVENT,
                           CATEGORY,
                           CALLBACK_ID,
                           CATEGORY_COMPARATOR,
                           CALLBACK_COMPARATOR>::deregisterCallback(
        const CATEGORY&    category,
        const CALLBACK_ID& id)
{
    int rc = 0;

    d_registryLock.lockReadReserveWrite();
    typename CategoryRegistryType::iterator it = d_registry.find(category);
    if (it != d_registry.end()) {
        typename CallbackRegistryType::iterator jt = it->second.find(id);
        if (jt != it->second.end()) {
            d_registryLock.upgradeToWriteLock();
            d_pool.releaseObject(jt->second);
            it->second.erase(jt);
        }
        else {
            rc = -1;
        }
    }
    else {
        rc = -2;
    }
    d_registryLock.unlock();
    return rc;
}

template <
          typename EVENT,
          typename CATEGORY,
          typename CALLBACK_ID,
          typename CATEGORY_COMPARATOR,
          typename CALLBACK_COMPARATOR
         >
int bcecs_CallbackRegistry<EVENT,
                           CATEGORY,
                           CALLBACK_ID,
                           CATEGORY_COMPARATOR,
                           CALLBACK_COMPARATOR>::deleteCategory(
        const CATEGORY& category)
{
    int rc = 0;

    d_registryLock.lockReadReserveWrite();
    typename CategoryRegistryType::iterator it = d_registry.find(category);
    if (it != d_registry.end()) {
        d_registryLock.upgradeToWriteLock();
        d_registry.erase(it);
    }
    else {
        rc = -1;
    }
    d_registryLock.unlock();
    return rc;
}

template <
          typename EVENT,
          typename CATEGORY,
          typename CALLBACK_ID,
          typename CATEGORY_COMPARATOR,
          typename CALLBACK_COMPARATOR
         >
int bcecs_CallbackRegistry<EVENT,
                           CATEGORY,
                           CALLBACK_ID,
                           CATEGORY_COMPARATOR,
                           CALLBACK_COMPARATOR>::notify(
        const CATEGORY& category,
        const EVENT&    event)
{
    int rc = 0;

    d_registryLock.lockRead();
    typename CategoryRegistryType::const_iterator it =
        d_registry.find(category);
    if (it != d_registry.end()) {
        for (typename CallbackRegistryType::const_iterator jt =
                it->second.begin();
             jt != it->second.end(); ++jt)
        {
            jt->second->execute(event);
        }
    }
    else {
        rc = -1;
    }
    d_registryLock.unlock();
    return rc;
}

// ACCESSORS

template <
          typename EVENT,
          typename CATEGORY,
          typename CALLBACK_ID,
          typename CATEGORY_COMPARATOR,
          typename CALLBACK_COMPARATOR
         >
inline
int bcecs_CallbackRegistry<EVENT,
                           CATEGORY,
                           CALLBACK_ID,
                           CATEGORY_COMPARATOR,
                           CALLBACK_COMPARATOR>::numCategories(
        const CALLBACK_ID& id) const
{
    bool found = false;
    int  num   = 0;

    d_registryLock.lockRead();
    for (typename CategoryRegistryType::const_iterator it = d_registry.begin();
         it != d_registry.end(); ++it)
    {
        if (it->second.find(id) != it->second.end()) {
            found = true;
            ++num;
        }
    }
    d_registryLock.unlock();

    if (!found) {
        num = -1;
    }
    return num;
}

template <
          typename EVENT,
          typename CATEGORY,
          typename CALLBACK_ID,
          typename CATEGORY_COMPARATOR,
          typename CALLBACK_COMPARATOR
         >
inline
int bcecs_CallbackRegistry<EVENT,
                           CATEGORY,
                           CALLBACK_ID,
                           CATEGORY_COMPARATOR,
                           CALLBACK_COMPARATOR>::numCallbacks(
        const CATEGORY& category) const
{
    int num = -1;

    d_registryLock.lockRead();
    typename CategoryRegistryType::const_iterator it =
        d_registry.find(category);
    if (it != d_registry.end()) {
        num = it->second.size();
    }
    d_registryLock.unlock();
    return num;
}

template <
          typename EVENT,
          typename CATEGORY,
          typename CALLBACK_ID,
          typename CATEGORY_COMPARATOR,
          typename CALLBACK_COMPARATOR
         >
inline
int bcecs_CallbackRegistry<EVENT,
                           CATEGORY,
                           CALLBACK_ID,
                           CATEGORY_COMPARATOR,
                           CALLBACK_COMPARATOR>::totalNumCategories() const
{
    return d_registry.size();
}

template <
          typename EVENT,
          typename CATEGORY,
          typename CALLBACK_ID,
          typename CATEGORY_COMPARATOR,
          typename CALLBACK_COMPARATOR
         >
inline
int bcecs_CallbackRegistry<EVENT,
                           CATEGORY,
                           CALLBACK_ID,
                           CATEGORY_COMPARATOR,
                           CALLBACK_COMPARATOR>::totalNumCallbacks() const
{
    int num = 0;

    d_registryLock.lockRead();
    for (typename CategoryRegistryType::const_iterator it = d_registry.begin();
         it != d_registry.end(); ++it)
    {
        num += it->second.size();
    }
    d_registryLock.unlock();
    return num;
}

}  // close namespace BloombergLP

#endif

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2004
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ------------------------------- END-OF-FILE --------------------------------
