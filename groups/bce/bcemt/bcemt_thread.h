// bcemt_thread.h                                                     -*-C++-*-
#ifndef INCLUDED_BCEMT_THREAD
#define INCLUDED_BCEMT_THREAD

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide portable thread utilities and synchronization primitives.
//
//@DEPRECATED: See below for alternative components.
//
//@CLASSES:
//   None
//
//@SEE_ALSO: bcemt_condition, bcemt_mutex, bcemt_recursivemutex,
//  bcemt_threadattributes, bcemt_threadutil
//
//@AUTHOR: Sergey Tarsis (starsis)
//
//@DESCRIPTION: [!DEPRECATED!]  This component is deprecated.  The collection
// of classes previously provided by 'bcemt_thread' have been properly factored
// into individual components (with 'bcemt_Attribute' also being renamed) as
// follows:
//..
//          Class                  Component
//  +----------------------+-----------------------------+
//  | bcemt_Attribute      | bcemt_threadattributes      |
//  | bcemt_Condition      | bcemt_condition             |
//  | bcemt_Mutex          | bcemt_mutex                 |
//  | bcemt_RecursiveMutex | bcemt_recursivemutex        |
//  | bcemt_ThreadUtil     | bcemt_threadutil            |
//  +----------------------+-----------------------------+
//..
// Client code should be modified to use the five aforementioned components
// directly, as needed.  For backward compatibility, this file includes the
// header files corresponding to the five replacement components.  The original
// component-level documentation follows, and may be used for reference until
// this component is retired.
//
// This component defines a portable and efficient thread management utility
// class and several synchronization primitives for intra-process use.  The
// usage model of the facilities provided is based primarily on the POSIX
// threads standard (for an overview of pthreads, see the book "Programming
// with POSIX Threads" by Butenhof, Addison Wesley, ISBN 0-201-63392-2).  The
// functionality provided in this component can be categorized as follows:
//..
//  o Thread Management
//  o Locking (a.k.a. mutual exclusion, or "mutex")
//  o Inter-Thread Signaling (a.k.a. condition variables)
//  o Thread-specific (a.k.a. thread-local) storage
//..
///Thread Management
///-----------------
// The 'bcemt_ThreadUtil' class is a suite of pure procedures that provides the
// ability to create and destroy threads, to attach "foreign threads" (that is,
// threads created outside of 'bcemt'), and to join threads (make one thread
// block and wait for another thread to exit).
//
// A thread is identified by an object of the opaque type
// 'bcemt_ThreadUtil::Handle'.  A handle of this type is returned when a thread
// is created (with 'bcemt_ThreadUtil::create()').  A client can also
// retrieve a 'Handle' for the "current" thread with the following function:
//..
//  bcemt_ThreadUtil::Handle myHandle = bcemt_ThreadUtil::self();
//..
// The thread manipulation functions in 'bcemt_ThreadUtil' take a thread handle
// or pointer to a thread handle as an argument.  To facilitate compatibility
// with existing systems and allow for non-portable operations, clients also
// have access to the 'bcemt_ThreadUtil::NativeHandle' type, which exposes the
// underlying, platform-specific thread identifier objects:
//..
//  bcemt_ThreadUtil::NativeHandle myNativeHandle;
//  myNativeHandle = bcemt_ThreadUtil::nativeHandle();
//..
///Thread Creation
///---------------
// Clients call 'bcemt_ThreadUtil::create()' to create threads.  This function
// requires a "C" linkage function pointer 'threadFunction' and a 'void'
// pointer to data 'userData'.  Upon creation, the new thread will call
// '(*threadFunction)(userData)'.  This function becomes the main driver for
// the new thread; when it returns, the thread terminates.
//
///Thread Attributes
///- - - - - - - - -
// Users can specify the attributes of a thread by supplying a
// 'bcemt_ThreadAttributes' object to the 'bcemt_ThreadUtil::create' function.
// 'bcemt_ThreadAttributes' is a value-semantic type that enables clients to
// manipulate a set of basic thread attributes, e.g., the stack size of a
// thread or its scheduling priority.
//..
//  bcemt_ThreadAttributes attributes;
//  attributes.setStackSize(1024);
//  attributes.setSchedulingPriority(1);
//..
// The default values of the attributes are listed in the following table.
//..
//  Attribute             Default Value
//  ===================+=============================
//  DetachedState      |  BCEMT_CREATE_JOINABLE
//  SchedulingPolicy   |  BCEMT_SCHED_OTHER
//  SchedulingPriority |  *platform's default value*
//  InheritSchedule    |  TRUE
//  StackSize          |  *platform's default value*
//  GuardSize          |  *platform's default value*
//..
// Note that, with the exception of 'bcemt_ThreadAttributes::DetachedState',
// specifying the values of the thread attributes only provides a hint to
// the operating system and those values may be ignored.
//
///'bcemt_ThreadAttributes::DetachedState'
/// -  -  -  -  -  -  -  -  -  -  -
// A thread can be created in either a "joinable" or "detached" state, by
// setting the 'detachedState' attribute to
// 'bcemt_ThreadAttributes::BCEMT_CREATE_JOINABLE' or
// 'bcemt_ThreadAttributes::BCEMT_CREATE_DETACHED', respectively.  Any thread
// can join with a "joinable" thread (i.e., the joining thread can block,
// waiting for the joinable thread's execution to complete) by invoking the
// 'bcemt_ThreadUtil::join' function on the joinable thread's 'Handle':
//..
//  int join(Handle handle, void **returnArg);
//  int detach(Handle handle);
//..
// Threads cannot join with a thread that was created in a detached state.  A
// joinable thread can also be put into the detached state via a call to the
// 'bcemt_ThreadUtil::detach' method.
//
// Note that a thread must be joined or detached before it completes.  That is,
// if a joinable thread is not joined before it completes, resources allocated
// by the kernel and possibly by the 'bcemt' library will be leaked.  A
// detached thread will be deallocated properly when it completes.
//
///Locking
///-------
// 'bcemt_thread' supports two variants of a mutually exclusive lock (mutex):
// 'bcemt_Mutex' and 'bcemt_RecursiveMutex'.  These may be used directly by
// client programs.  In addition, they are used by other classes in the
// 'bcemt_thread' component.
//
///'bcemt_Mutex'
///- - - - - - -
// 'bcemt_Mutex' is an efficient synchronization primitive that enables
// serialized access to shared resources.  In order to protect against broken
// data invariants, clients can use a 'bcemt_Mutex' to ensure exclusive access
// to an area of code or data that is shared among multiple threads within a
// single process.  'bcemt_Mutex' provides the following three manipulators:
//..
//  void lock();
//  int tryLock();
//  void unlock();
//..
// Only one thread at a time can return from the 'lock' method, blocking all
// other threads that call the 'lock' method on the same mutex.  The thread
// that has successfully returned from a call to 'lock' is said to have
// "acquired" or "locked" the mutex.  All blocked threads remain blocked until
// the thread that locked the mutex calls the 'unlock' method.  Subsequently,
// another (possibly even the same) thread, can then acquire the lock.
//
// The behavior is undefined if 'unlock' is invoked from a thread that did not
// successfully acquire the lock, or if 'lock' is called twice in a thread
// without calling 'unlock' in between (i.e., 'bcemt_Mutex' is non-recursive).
//..
//  void f(bcemt_Mutex *mx)
//  {
//      mx->lock();
//      mx->unlock();
//      mx->lock();   // OK
//      mx->lock();   // ERROR!
//  }
//..
// Note that two consecutive calls to 'unlock' also results in undefined
// behavior.
//
///'bcemt_RecursiveMutex'
/// - - - - - - - - - - -
// As the name implies, 'bcemt_RecursiveMutex' supports multiple calls to
// 'lock', balanced by a corresponding number of calls to 'unlock'.
//..
//  void f(bcemt_RecursiveMutex *rmx)
//  {
//      rmx->lock();   // First level OK
//      rmx->lock();   // Second level OK
//      rmx->unlock(); // Release level 2 -- mutex stays locked.
//      rmx->unlock(); // Release level 1 -- mutex is unlocked
//  }
//..
///Inter-Thread Notification
///-------------------------
// The 'bcemt_Condition' class implements the concept of a *condition*
// *variable*, enabling multiple threads to communicate information about the
// state of shared data.  A condition variable is a signaling mechanism
// associated with a mutex, which in turn protects a data invariant.
// A condition variable enables threads to wait for a predicate (i.e., logical
// expression) to become true, and to communicate to other threads that the
// predicate might be true.
//
// One or more threads can wait efficiently on a condition variable, either
// indefinitely or until some absolute time, by invoking one of the following
// methods of a 'bcemt_Condition':
//..
//  int wait(bcemt_Mutex *mutex);
//  int timedWait(bcemt_Mutex *mutex, const bdet_TimeInterval& timeout);
//..
// The caller must lock the mutex before invoking these functions.  The
// 'bcemt_Condition' atomically releases the lock and waits, thereby preventing
// other threads from changing the predicate after the lock is released, but
// before the thread begins to wait.  The 'bcemt' library guarantees that this
// lock will be reacquired before returning from a call to the 'wait' methods
// unless an error occurs.
//
// When invoking the 'timedWait' methods, clients must specify a timeout after
// which the call will return even if the condition is not signaled.  The
// timeout is expressed as a 'bdet_TimeInterval' object that holds the absolute
// time since some platform defined epoch (e.g., number of seconds and
// nanoseconds from 00:00:00 UTC, January 1, 1970).  Clients should use the
// 'bdetu_SystemTime' utility to access the current time.
//
// Other threads can indicate that the predicate is true by signaling or
// broadcasting the same 'bcemt_Condition' object.  A broadcast wakes up all
// waiting threads, a signal wakes only one thread.  The client has no control
// over which thread will be signaled if multiple threads are waiting.
//..
//  void signal();
//  void broadcast();
//..
// A thread waiting on a condition variable may be signaled (i.e., the thread
// may wake up without an error) but find that the predicate is still false.
// This situation can arise for a few reasons: spurious wakeups produced by the
// operating system, intercepted wakeups, and loose predicates.  Therefore, a
// waiting thread should always check the predicate *after* (as well as before)
// the call to the 'wait' function.  The following usage pattern should always
// be followed:
//..
//  // ...
//  mutex.lock();
//  while (FALSE == predicate) {
//      condition.wait(&mutex);
//  }
//  // modify shared resources and set the predicate here
//  mutex.unlock();
//  // ...
//..
// The usage pattern for a timed wait is similar with extra branches to handle
// the timeout state.
//..
//  // ...
//  enum { TIMED_OUT = -1 };
//  bdet_TimeInterval timeout = bdetu_SystemTime::now();
//  // advance the timeout to some delta into the future here
//
//  mutex.lock();
//  while (FALSE == predicate) {
//      condition.timedWait(&mutex, timeout);
//      if (TIMED_OUT == status) {
//          break;
//      }
//  }
//
//  if (FALSE == predicate) {
//      // The wait timed out and the predicate was false.  Perform timeout
//      // recovery here.
//      // ...
//  }
//  else {
//      // The condition variable was either signaled or timed out
//      // and the predicate is true.  Modify shared resources and set
//      // predicate here.
//      // ...
//  }
//  mutex.unlock();
//  // ...
//..
///Usage
///-----
//
///Creating a Simple Thread
/// - - - - - - - - - - - -
// This example creates a thread using the default attribute settings.  Upon
// creation, the thread executes the user-supplied function 'myThreadFunction',
// that counts 5 seconds before terminating.  After creating the thread,
// the 'main' routine *joins* the thread; in effect, this causes 'main' to
// wait for execution of 'myThreadFunction' to complete, and this guarantees
// that the output from 'main' will follow the last output from the
// user-supplied function.
//..
//  extern "C" void *myThreadFunction(void *)
//      // Print to standard output "Another second has passed"
//      // every second for five seconds.
//  {
//      for(int i = 0; i < 5; ++i) {
//          bcemt_ThreadUtil::microSleep(0,1);
//          bsl::cout << "Another second has passed." << bsl::endl;
//      }
//      return 0;
//  }
//
//  int main()
//  {
//      bcemt_ThreadAttributes attributes;
//      bcemt_ThreadUtil::Handle handle;
//      bcemt_ThreadUtil::create(&handle, attributes, myThreadFunction, 0);
//      bcemt_ThreadUtil::join(handle);
//      bsl::cout << "A five second interval has elapsed.\n";
//      return 0;
//  }
//..
// The output of this program would be
//..
//   Another second has passed.
//   Another second has passed.
//   Another second has passed.
//   Another second has passed.
//   Another second has passed.
//   A five second interval has elapsed.
//..
///Specifying Thread Attributes
/// - - - - - - - - - - - - - -
// Alternatively, the attributes of a thread can be explicitly specified by
// manipulating the thread's 'bcemt_ThreadAttributes' constructor argument.
// For instance, we could specify a smaller stack size for a thread to conserve
// system resources if we know that we will require not the platform's default
// stack size.
//..
//  extern "C" void *mySmallStackThreadFunction(void *threadArg)
//      // Initialize a small object on the stack and do some work.
//  {
//      char *initValue = (char *)threadArg;
//      char Small[8];
//      memset(&Small[0], *initValue, 8);
//      // do some work ...
//      return 0;
//  }
//
//  void createSmallStackSizeThread()
//      // Create a detached thread with the small stack size
//      // and perform some work
//  {
//      enum { STACK_SIZE = 16384 };
//      bcemt_ThreadAttributes attributes;
//      attributes.setDetachedState(
//                              bcemt_ThreadAttributes::BCEMT_CREATE_DETACHED);
//      attributes.setStackSize(STACK_SIZE);
//
//      char initValue = 1;
//      bcemt_ThreadUtil::Handle handle;
//      int status = bcemt_ThreadUtil::create(&handle,
//                                            attributes,
//                                            mySmallStackThreadFunction,
//                                            &initValue);
//  }
//..
///Producer-Consumer Threads
///- - - - - - - - - - - - -
// The producer-consumer pattern can be implemented simply and efficiently
// using threads.  In this model, one or more threads produce requests for work
// and one or more threads consume or respond to these requests.  The requests
// are typically put in a queue by the producers and removed by the consumers.
// The shared queue is protected by a lock and a condition variable signals the
// existence of new work.
//
// The following snippets of code illustrate the use of 'bcemt_Condition' to
// serialize access to work load queue in a 'producer-consumer' example.  Note
// that higher level packages in this package group provide thread enabled data
// structures that would help simplify this example.
//..
//  struct my_WorkQueue {
//      bdec_Queue<my_WorkItem> d_queue; // queue of work requests
//      bcemt_Mutex             d_mx;    // protects the shared queue
//      bcemt_Condition         d_cv;    // signals the existence of new work
//  };
//
//  extern "C" void *producer(void *arg)
//      // Receive commands from an external source, place them on
//      // the shared 'my_WorkQueue' (passed as the specified void
//      // pointer 'arg'), and return 'arg'.
//  {
//      my_WorkQueue *queue = (my_WorkQueue *)arg;
//
//      // For simplicity, the external source is modeled by a
//      // for loop that generates 100 random numbers.
//
//      for (int i = 0; i < 100; ++i) {
//          my_WorkItem request;
//          request.d_item = rand();
//          queue->d_mx.lock();
//          queue->d_queue.pushBack(request);
//          queue->d_mx.unlock();
//          queue->d_cv.signal();
//      }
//      return queue;
//  }
//
//  extern "C" void *consumer(void *arg)
//      // Consume the commands from the shared 'my_WorkQueue' (passed as
//      // the specified void pointer 'arg') and return 0.  Note that this
//      // function will continue processing commands until there are no
//      // commands for at least one second.
//  {
//      enum { TIMED_OUT = -1 };
//      my_WorkQueue *queue = (my_WorkQueue *)arg;
//      int finished = 0;
//      while (!finished) {
//          // Set the timeout to be one second from now.
//
//          bdet_TimeInterval timeout = bdetu_SystemTime::now();
//          timeout.addSeconds(1);
//
//          // Wait for work requests to be added to the queue.
//
//          queue->d_mx.lock();
//          while (0 == queue->d_queue.length()) {
//              int status = queue->d_cv.timedWait(&queue->d_mx, timeout);
//              if (TIMED_OUT == status) {
//                  break;
//              }
//          }
//
//          if (0 != queue->d_queue.length()) {
//              // The condition variable was either signaled or timed out
//              // and there are work requests in the queue.
//
//              my_WorkItem item = queue->d_queue.front();
//              queue->d_queue.popFront();
//              queue->d_mx.unlock();
//
//              // Process the work requests.
//              // ...
//          }
//          else {
//              // The wait timed out and the queue was empty.  Unlock the
//              // mutex and return.
//
//              queue->d_mx.unlock();
//              finished = 1;
//          }
//      }
//      return 0;
//  }
//
//  int main()
//  {
//      my_WorkQueue queue;
//
//      bcemt_ThreadUtil::Handle mainHandle;
//      mainHandle = bcemt_ThreadUtil::self();
//
//      bcemt_ThreadAttributes attributes;
//      attributes.setDetachedState(
//                              bcemt_ThreadAttributes::BCEMT_CREATE_JOINABLE);
//
//      bcemt_ThreadUtil::Handle producerHandle;
//      int status = bcemt_ThreadUtil::create(&producerHandle,
//                                            attributes,
//                                            &producer,
//                                            &queue);
//      assert(0 == status);
//
//      bcemt_ThreadUtil::Handle consumerHandle;
//      status = bcemt_ThreadUtil::create(&consumerHandle,
//                                        attributes,
//                                        &consumer,
//                                        &queue);
//      assert(0 == status);
//
//      assert(0== bcemt_ThreadUtil::isEqual(consumerHandle,producerHandle));
//      assert(0== bcemt_ThreadUtil::isEqual(mainHandle, producerHandle));
//      assert(0== bcemt_ThreadUtil::isEqual(mainHandle, consumerHandle));
//
//      status = bcemt_ThreadUtil::join(consumerHandle);
//      assert(0 == status);
//      void *retArg;
//      status = bcemt_ThreadUtil::join(producerHandle, &retArg);
//      assert(0 == status);
//      assert((my_WorkQueue*)retArg == &queue);
//      return 0;
//  }
//..
///Using 'bcemt_Mutex' to Add Thread-Safety
/// - - - - - - - - - - - - - - - - - - - -
// The following snippets of code illustrate the use of 'bcemt_Mutex' to write
// a thread safe class ('my_SafeAccount') given a thread-unsafe 'my_Account'
// class.  The simple 'my_Account' class is defined as follows.
//..
//  class my_Account {
//      // This 'class' represents a bank account with a single balance.  It
//      // is not thread-safe.
//
//      // DATA
//      double d_money;   // amount of money in the account
//
//    public:
//      // CREATORS
//      my_Account();
//          // Create an account with zero balance.
//
//      my_Account(const my_Account& other);
//          // Create an account having the balance of the specified
//          // 'other' account.
//
//      ~my_Account();
//          // Destroy this account.
//
//      // MANIPULATORS
//      my_Account& operator=(const my_Account& rhs);
//          // Assign to this account the balance of the specified
//          // 'rhs' account, and return a reference to this modifiable
//          // account.
//
//      void deposit(double amount);
//          // Deposit the specified 'amount' of money into this account.
//
//      void withdraw(double amount);
//          // Withdraw the specified 'amount' of money from this account.
//
//      // ACCESSORS
//      double balance() const;
//          // Return the amount of money that is available for withdrawal
//          // from this account.
//  };
//..
// The implementation of this class is straightforward and omitted here for
// brevity.
//
// Next, use a 'bcemt_Mutex' object to render atomic the function calls of a
// new thread-safe class that uses the thread unsafe class in its
// implementation.  Note the typical usage of 'mutable' for the lock.
//..
//  class my_SafeAccountHandle {
//      // This 'class' provides a thread-safe handle to an account (held,
//      // not owned) passed at construction.
//
//      // DATA
//      my_Account          *d_account_p;  // held, not owned
//      mutable bcemt_Mutex  d_lock;
//
//    private:
//      // NOT IMPLEMENTED
//      my_SafeAccountHandle(const my_SafeAccountHandle&);
//      my_SafeAccountHandle& operator=(const my_SafeAccountHandle&);
//
//    public:
//      // CREATORS
//      my_SafeAccountHandle(my_Account *account);
//          // Create a thread-safe handle to the specified 'account'.
//
//      ~my_SafeAccountHandle();
//          // Destroy this handle.  Note that the held account
//            // is unaffected by this operation.
//
//      // MANIPULATORS
//      void deposit(double amount);
//          // Atomically deposit the specified 'amount' of money into the
//          // account held by this handle.  Note that this operation is
//          // thread-safe, no 'lock' is needed.
//
//      void lock();
//          // Provide exclusive access to the underlying account held by
//          // this object.
//
//      void unlock();
//          // Release exclusivity of the access to the underlying
//          // account held by this object.
//
//      void withdraw(double amount);
//          // Atomically withdraw the specified 'amount' of money from the
//          // account held by this handle.  Note that this operation is
//          // thread-safe, no 'lock' is needed.
//
//      // ACCESSORS
//      my_Account *account() const;
//          // Return the address of the account held by this handle.
//
//      double balance() const;
//          // Atomically return the amount of money that is available for
//          // withdrawal from the account held by this handle.
//  };
//..
// The implementation showcasing the use of the 'bcemt_Mutex' follows.
//..
//  // CREATORS
//  my_SafeAccountHandle::my_SafeAccountHandle(my_Account *account)
//  : d_account_p(account)
//  { }
//
//  my_SafeAccountHandle::~my_SafeAccountHandle()
//  { }
//
//  // MANIPULATORS
//  void my_SafeAccountHandle::deposit(double amount)
//  {
//      d_lock.lock();  // consider using 'bcemt_LockGuard'
//      d_account_p->deposit(amount);
//      d_lock.unlock();
//  }
//
//  void my_SafeAccountHandle::lock()
//  {
//      d_lock.lock();
//  }
//
//  void my_SafeAccountHandle::unlock()
//  {
//      d_lock.unlock();
//  }
//
//  void my_SafeAccountHandle::withdraw(double amount)
//  {
//      d_lock.lock();  // consider using 'bcemt_LockGuard'
//      d_account_p->withdraw(amount);
//      d_lock.unlock();
//  }
//
//  // ACCESSORS
//  my_Account *my_SafeAccountHandle::account() const
//  {
//      return d_account_p;
//  }
//
//  double my_SafeAccountHandle::balance() const
//  {
//      d_lock.lock();
//      double res = d_account_p->balance();
//      d_lock.unlock();
//      return res;
//  }
//..
// The handle's atomic methods are used just as the corresponding methods in
// 'my_Account'.
//..
//  Account account;
//  account.deposit(100.50);
//  double paycheck = 50.25;
//  my_SafeAccountHandle handle(&account);
//
//                             assert(100.50 == handle.balance());
//  handle.deposit(paycheck);  assert(150.75 == handle.balance());
//..
// We can also use the handle's 'lock' and 'unlock' methods to implement
// non-primitive atomic transactions on the account.
//..
//  double check[5] = { 25.0, 100.0, 99.95, 75.0, 50.0 };
//
//  handle.lock();
//  double originalBalance = handle.account()->balance();
//  for (int i = 0; i < 5; ++i) {
//      handle.account()->deposit(check[i]);
//  }
//  assert(originalBalance + 349.95 == handle.account()->balance());
//  handle.unlock();
//..

#ifndef INCLUDED_BCESCM_VERSION
#include <bcescm_version.h>
#endif

#ifndef INCLUDED_BCEMT_CONDITION
#include <bcemt_condition.h>
#endif

#ifndef INCLUDED_BCEMT_MUTEX
#include <bcemt_mutex.h>
#endif

#ifndef INCLUDED_BCEMT_RECURSIVEMUTEX
#include <bcemt_recursivemutex.h>
#endif

#ifndef INCLUDED_BCEMT_THREADATTRIBUTES
#include <bcemt_threadattributes.h>
#endif

#ifndef INCLUDED_BCEMT_THREADUTIL
#include <bcemt_threadutil.h>
#endif

#ifndef BDE_OMIT_INTERNAL_DEPRECATED

// Transitive inclusions depended on by some Robo code.

#ifndef INCLUDED_BDETU_SYSTEMTIME
#include <bdetu_systemtime.h>
#endif

#endif // BDE_OMIT_INTERNAL_DEPRECATED

#ifndef bcemt_Attribute
#define bcemt_Attribute bcemt_ThreadAttributes
#endif

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2010
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
