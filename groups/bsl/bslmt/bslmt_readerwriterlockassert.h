// bslmt_readerwriterlockassert.h                                     -*-C++-*-
#ifndef INCLUDED_BSLMT_READERWRITERLOCKASSERT
#define INCLUDED_BSLMT_READERWRITERLOCKASSERT

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide an assert macro for verifying reader-writer lock status.
//
//@CLASSES:
//
//@MACROS:
//  BSLMT_READERWRITERLOCKASSERT_IS_LOCKED:            test in non-opt modes
//  BSLMT_READERWRITERLOCKASSERT_IS_LOCKED_SAFE:       test in safe    mode
//  BSLMT_READERWRITERLOCKASSERT_IS_LOCKED_OPT:        test in all     modes
//  BSLMT_READERWRITERLOCKASSERT_IS_LOCKED_READ:       test in non-opt modes
//  BSLMT_READERWRITERLOCKASSERT_IS_LOCKED_READ_SAFE:  test in safe    mode
//  BSLMT_READERWRITERLOCKASSERT_IS_LOCKED_READ_OPT:   test in all     modes
//  BSLMT_READERWRITERLOCKASSERT_IS_LOCKED_WRITE:      test in non-opt modes
//  BSLMT_READERWRITERLOCKASSERT_IS_LOCKED_WRITE_SAFE: test in safe    mode
//  BSLMT_READERWRITERLOCKASSERT_IS_LOCKED_WRITE_OPT:  test in all     modes
//
//@SEE_ALSO: bslmt_lockassert, bslmt_readerwriterlock, bslmt_readerwritermutex
//
//@DESCRIPTION: This component provides macros for asserting that a
// reader-writer lock is locked.  It does not distinguish between locks held by
// the current thread or other threads.  If the macro is active in the current
// build mode, when the macro is called, if the supplied lock is unlocked, the
// assert handler installed for 'BSLS_ASSERT' will be called.  The assert
// handler installed by default will report an error and abort the task.  Note
// that the type of lock (pointer) passed to each of these macros is determined
// at compile-time.  See {Requirements on the Lock Type} below.
//
// The nine macros defined by the component are analogous to the macros defined
// by 'BSLS_ASSERT':
//..
//  +---------------------------------------------------+------------------=+
//  | Macro                                             | When Active       |
//  +===================================================+===================+
//  |'BSLMT_READERWRITERLOCKASSERT_IS_LOCKED'           | When              |
//  |'BSLMT_READERWRITERLOCKASSERT_IS_LOCKED_READ'      | 'BSLS_ASSERT'     |
//  |'BSLMT_READERWRITERLOCKASSERT_IS_LOCKED_WRITE'     | is active.        |
//  +---------------------------------------------------+-------------------+
//  |'BSLMT_READERWRITERLOCKASSERT_IS_LOCKED_SAFE'      | When              |
//  |'BSLMT_READERWRITERLOCKASSERT_IS_LOCKED_READ_SAFE' | 'BSLS_ASSERT_SAFE'|
//  |'BSLMT_READERWRITERLOCKASSERT_IS_LOCKED_WRITE_SAFE'| is active.        |
//  +---------------------------------------------------+-------------------+
//  |'BSLMT_READERWRITERLOCKASSERT_IS_LOCKED_OPT'       | When              |
//  |'BSLMT_READERWRITERLOCKASSERT_IS_LOCKED_READ_OPT'  | 'BSLS_ASSERT_OPT' |
//  |'BSLMT_READERWRITERLOCKASSERT_IS_LOCKED_WRITE_OPT' | is active.        |
//  +---------------------------------------------------+-------------------+
//..
// In build modes where any one of these macros is not active, the presence of
// the macros has no effect.
//
// If any of these asserts are in effect and fail (because the reader-writer
// lock in question was unlocked), the behavior parallels the behavior of the
// assertion macros defined in 'bsls_assert.h' -- 'bsls::Assert::invokeHandler'
// is called, with a source code expression, the name of the source file, and
// the line number in the source file where the macro was called.  If the
// default handler is installed, this will result in an error message and an
// abort.
//
///Caveat: False Positives
///-----------------------
// Preconditions on locks typically require that the lock exist *and* *is*
// *held* *by* *the* *calling* *thread*.  Unfortunately, lock ownership is not
// recorded in the lock and cannot be confirmed.  The absence of any lock when
// the calling thread should hold one is certainly a problem; however, the
// existence of a lock does not guarantee that the complete precondition is
// met.
//
///Requirements on the Lock Type
///-----------------------------
// This system of macros accept pointers to reader-write lock objects that
// provide the methods:
//: o 'isLocked',
//: o 'isLockedRead', and
//: o 'isLockedWrite'
// 
// Two compatible classes are:
//: o 'bslmt::ReaderWriteLock' and
//: o 'bslmt::ReaderWriteMutex'
//
// Although the required methods are typically 'const'-qualified (i.e.,
// "accessor" methods), that is not a requriement.  Some client lock classes
// may implement these methods in terms of 'tryLock'/'unlock' methods that
// require non-'const' access to the lock.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Checking Consistency Within Private Methods
/// - - - - - - - - - - - - - - - - - - - - - - - - - - -
// This example is an generalization of {'bslmt_mutexassert'|Example 1:
// Checking Consistency Within a Private Method}.  In that example, a mutex was
// used to control access.  Here, the (simple) mutex is replaced with a
// 'bslmt::ReaderWriterLock' that is allows multiple concurrent access to the
// queue when conditions allow.
//
// Sometimes multithreaded code is written such that the author of a function
// requires that a caller has already acquired a lock.  The
// 'BSLMT_READERWRITERLOCKASSERT_IS_LOCKED*' family of assertions allows the
// programmers to detect, using defensive programming techniques, if the
// required lock has *not* been acquired.
//
// Suppose we have a fully thread-safe queue that contains 'int' values, and is
// guarded by an internal 'bslmt::ReaderWriterLock' object.
//
// First, we define the container class:
//..
//  class MyThreadSafeQueue {
//      // This 'class' provides a fully *thread-safe* unidirectional queue of
//      // 'int' values.  See {'bsls_glossary'|Fully Thread-Safe}.  All public
//      // methods operate as single, atomic actions.
//
//      // DATA
//      bsl::deque<int>      d_deque;    // underlying non-*thread-safe*
//                                       // standard container
//
//      mutable bslmt::ReaderWriterLock
//                           d_rwLock;   // coordinate thread access
//
//      // PRIVATE MANIPULATOR
//      int popImp(int *result);
//          // Assign the value at the front of the queue to the specified
//          // '*result', and remove the value at the front of the queue;
//          // return 0 if the queue was not initially empty, and a non-zero
//          // value (with no effect) otherwise.  The behavior is undefined
//          // unless 'd_rwLock' is locked for writing.
//
//      // PRIVATE ACCESSOR
//      bsl::pair<int, double> getStats() const;
//          // Return a 'bsl::pair<int, int>' containing the number of elements
//          // and the mean value of the elements of this queue.  The mean
//          // values is set to 'DBL_MIN' if the number of elements is 0.  The
//          // behavior is undefined unless the call has locked this queue
//          // (either a read lock or write lock).
//
//    public:
//      // ...
//
//      // MANIPULATORS
//
//      int pop(int *result);
//          // Assign the value at the front of the queue to the specified
//          // '*result', and remove the value at the front of the queue;
//          // return 0 if the queue was not initially empty, and a non-zero
//          // value (with no effect) otherwise.
//
//      void popAll(bsl::vector<int> *result);
//          // Assign the values of all the elements from this queue, in order,
//          // to the specified '*result', and remove them from this queue.
//          // Any previous contents of '*result' are discarded.  Note that, as
//          // with the other public manipulators, this entire operation occurs
//          // as a single, atomic action.
//
//      void purgeAll(double limit);
//          // Remove all elements from this queue if their mean exceeds the
//          // specified 'limit'.
//
//      void push(int value);
//          // Push the specified 'value' onto this queue.
//
//      template <class INPUT_ITER>
//      void pushRange(const INPUT_ITER& first, const INPUT_ITER& last);
//          // Push the values from the specified 'first' (inclusive) to the
//          // specified 'last' (exclusive) onto this queue.
//
//      // ACCESSORS
//      double mean() const;
//          // Return the mean value of the elements of this queue.
//
//      bsl::size_t numElements() const;
//          // Return the number of elements in this queue.
//  };
//..
// Notice that this version of the 'MyThreadSafeQueue' class has two public
// accessors, 'numElements' and 'mean', and an additional manipulator,
// 'purgeAll'.
//
// Then, we implement most of the manipulators:
//..
//  // PRIVATE MANIPULATOR
//  int MyThreadSafeQueue::popImp(int *result)
//  {
//      BSLMT_READERWRITERLOCKASSERT_IS_LOCKED_WRITE(&d_rwLock);
//
//      if (d_deque.empty()) {
//          return -1;                                                // RETURN
//      }
//      else {
//          *result = d_deque.front();
//          d_deque.pop_front();
//          return 0;                                                 // RETURN
//      }
//  }
//
//  // MANIPULATORS
//  int MyThreadSafeQueue::pop(int *result)
//  {
//      BSLS_ASSERT(result);
//
//      d_rwLock.lockWrite();
//      int rc = popImp(result);
//      d_rwLock.unlockWrite();
//      return rc;
//  }
//
//  void MyThreadSafeQueue::popAll(bsl::vector<int> *result)
//  {
//      BSLS_ASSERT(result);
//
//      const int size = static_cast<int>(d_deque.size());
//      result->resize(size);
//      int *begin = result->begin();
//      for (int index = 0; index < size; ++index) {
//          int rc = popImp(&begin[index]);
//          BSLS_ASSERT(0 == rc);
//      }
//  }
//
//  void MyThreadSafeQueue::push(int value)
//  {
//      d_rwLock.lockWrite();
//      d_deque.push_back(value);
//      d_rwLock.unlockWrite();
//  }
//
//  template <class INPUT_ITER>
//  void MyThreadSafeQueue::pushRange(const INPUT_ITER& first,
//                                    const INPUT_ITER& last)
//  {
//      d_rwLock.lockWrite();
//      d_deque.insert(d_deque.begin(), first, last);
//      d_rwLock.unlockWrite();
//  }
//..
// Notice that these implementations are identical to those shown in
// {'bslmt_mutexassert'|Example 1} except that the 'lock' calls to the
// 'bslmt::Mutex' there have been changed here to 'lockWrite' calls on a
// 'bslmt::ReaderWriterLock'.  Both operations provide exclusive access to the
// container.
//
// Also notice that, having learned the lesson of {'bslmt_mutexassert'|Example
// 1}, we were careful to acquire a write lock for the duration of each of
// these operation and to check the precondition of the the private 'popImp'
// method by using the 'BSLMT_READERWRITERLOCKASSERT_IS_LOCKED_WRITE' macro.
//
// Finally notice that we use the "normal" flavor of the macro (rather than the
// '*_SAFE' version) because this test is not particularly expensive.
//
// Next, we implement the accessor methods of the container:
//..
//  // ACCESSORS
//  double MyThreadSafeQueue::mean() const
//  {
//      d_rwLock.lockRead();
//      bsl::pair<int, double> result = getStats();
//      d_rwLock.unlockRead();
//      return result.second;
//  }
//
//  bsl::size_t MyThreadSafeQueue::numElements() const
//  {
//      d_rwLock.lockRead();
//      bsl::size_t numElements = d_deque.size();
//      d_rwLock.unlockRead();
//      return numElements;
//  }
//..
// Notice that each of these methods acquire a read lock for the duration of
// the operation.  These locks allow shared access provided that the container
// is not changed, a reasonable assumption for these 'const'-qualified methods.
//
// Also notice that the bulk of the work of 'mean' is done by the private
// method 'getStats'.  One's might except the private method to confirm that a
// lock was acquired by using the
// 'BSLMT_READERWRITERLOCKASSERT_IS_LOCKED_READ' macro; however, the reason
// for creating that private method is so that it can be reused by the
// 'purgeAll' method, a non-'const' method that requires a write lock.  Thus,
// 'getStats' is an occassion to use the
// 'BSLMT_READERWRITERLOCKASSERT_IS_LOCKED' check (for either a read lock *or*
// a write lock).
//..
//  // PRIVATE ACCESSORS
//  bsl::pair<int, double> MyThreadSafeQueue::getStats() const
//  {
//      BSLMT_READERWRITERLOCKASSERT_IS_LOCKED(&d_rwLock);
//
//      int numElements = d_deque.size();
//
//      if (0 == numElements) {
//          return bsl::make_pair(numElements, DBL_MIN);              // RETURN
//      }
//
//      int    sum  = bsl::accumulate(d_deque.cbegin(), d_deque.cend(), 0);
//      double mean = static_cast<double>(sum)
//                  / static_cast<double>(numElements);
//
//      return bsl::make_pair(numElements, mean);
//  }
//..
// Next, we implement the 'purgeAll' method:
//..
//  void MyThreadSafeQueue::purgeAll(double limit)
//  {
//      d_rwLock.lockWrite();
//      bsl::pair<int, double> results = getStats();      // requires some lock
//      if (0 < results.first && limit < results.second) {
//          for (int i = 0; i < results.first; ++i) {
//              int dummy;
//              int rc = popImp(&dummy);               // requires a write lock
//              assert(0 == rc);
//          }
//      }
//      d_rwLock.unlockWrite();
//  }
//..
// Finally, we confirm that our accessors work as expected:
//..
//  void testEnhancedThreadSafeQueue()
//      // Exercise the added methods of the 'MyThreadSafeQueue' class.
//  {
//      MyThreadSafeQueue queue;
//
//      const int rawData[] = { 17, 3, -20, 7, 28 };
//      enum { k_RAW_DATA_LENGTH = sizeof rawData / sizeof *rawData };
//
//      queue.pushRange(rawData + 0, rawData + k_RAW_DATA_LENGTH);
//
//      assert(5 == queue.numElements());
//      assert(7 == queue.mean());
//
//      queue.push(100000);
//      queue.purgeAll(10);
//
//      assertV(queue.numElements(), 0       == queue.numElements());
//      assertV(queue.mean()       , DBL_MIN == queue.mean());
//  }
//..

#include <bslscm_version.h>

#include <bsls_assert.h>

#include <bsl_string.h>  // 'native_std::strcmp'

// ----------------------------------------------------------------------------

#if defined(BSLS_ASSERT_IS_ACTIVE)
    #define BSLMT_READERWRITERLOCKASSERT_IS_LOCKED(rwLock_p) do {             \
        bslmt::ReaderWriterLockAssert_Imp::assertIsLocked(                    \
                   (rwLock_p),                                                \
                   "BSLMT_READERWRITERLOCKASSERT_IS_LOCKED(" #rwLock_p ")",   \
                   __FILE__,                                                  \
                   __LINE__,                                                  \
                   bsls::Assert::k_LEVEL_ASSERT); } while (false)
#else
    #define BSLMT_READERWRITERLOCKASSERT_IS_LOCKED(rwLock_p) ((void) 0)
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
     #define BSLMT_READERWRITERLOCKASSERT_IS_LOCKED_SAFE(rwLock_p) do {       \
         bslmt::ReaderWriterLockAssert_Imp::assertIsLocked(                   \
              (rwLock_p),                                                     \
              "BSLMT_READERWRITERLOCKASSERT_IS_LOCKED_SAFE(" #rwLock_p ")",   \
              __FILE__,                                                       \
              __LINE__,                                                       \
              bsls::Assert::k_LEVEL_SAFE); } while (false)
#else
    #define BSLMT_READERWRITERLOCKASSERT_IS_LOCKED_SAFE(rwLock_p) ((void) 0)
#endif

#if defined(BSLS_ASSERT_OPT_IS_ACTIVE)
    #define BSLMT_READERWRITERLOCKASSERT_IS_LOCKED_OPT(rwLock_p) do {         \
        bslmt::ReaderWriterLockAssert_Imp::assertIsLocked(                    \
               (rwLock_p),                                                    \
               "BSLMT_READERWRITERLOCKASSERT_IS_LOCKED_OPT(" #rwLock_p ")",   \
               __FILE__,                                                      \
               __LINE__,                                                      \
               bsls::Assert::k_LEVEL_OPT); } while (false)
#else
    #define BSLMT_LOCKASSERT_IS_LOCKED_OPT(rwLock_p) ((void) 0)
#endif

// ----------------------------------------------------------------------------

#if defined(BSLS_ASSERT_IS_ACTIVE)
    #define BSLMT_READERWRITERLOCKASSERT_IS_LOCKED_READ(rwLock_p) do {        \
        bslmt::ReaderWriterLockAssert_Imp::assertIsLockedRead(                \
              (rwLock_p),                                                     \
              "BSLMT_READERWRITERLOCKASSERT_IS_LOCKED_READ(" #rwLock_p ")",   \
              __FILE__,                                                       \
              __LINE__,                                                       \
              bsls::Assert::k_LEVEL_ASSERT); } while (false)
#else
    #define BSLMT_READERWRITERLOCKASSERT_IS_LOCKED_READ(rwLock_p) ((void) 0)
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
    #define BSLMT_READERWRITERLOCKASSERT_IS_LOCKED_READ_SAFE(rwLock_p) do {   \
        bslmt::ReaderWriterLockAssert_Imp::assertIsLockedRead(                \
         (rwLock_p),                                                          \
         "BSLMT_READERWRITERLOCKASSERT_IS_LOCKED_READ_SAFE(" #rwLock_p ")",   \
         __FILE__,                                                            \
         __LINE__,                                                            \
         bsls::Assert::k_LEVEL_SAFE); } while (false)
#else
    #define BSLMT_READERWRITERLOCKASSERT_IS_LOCKED_READ_SAFE(rwLock_p)        \
                                                                     ((void) 0)
#endif

#if defined(BSLS_ASSERT_OPT_IS_ACTIVE)
    #define BSLMT_READERWRITERLOCKASSERT_IS_LOCKED_READ_OPT(rwLock_p) do {    \
        bslmt::ReaderWriterLockAssert_Imp::assertIsLockedRead(                \
          (rwLock_p),                                                         \
          "BSLMT_READERWRITERLOCKASSERT_IS_LOCKED_READ_OPT(" #rwLock_p ")",   \
          __FILE__,                                                           \
          __LINE__,                                                           \
          bsls::Assert::k_LEVEL_OPT); } while (false)
#else
    #define BSLMT_LOCKASSERT_IS_LOCKED_READ_OPT(rwLock_p) ((void) 0)
#endif

// ----------------------------------------------------------------------------

#if defined(BSLS_ASSERT_IS_ACTIVE)
    #define BSLMT_READERWRITERLOCKASSERT_IS_LOCKED_WRITE(rwLock_p) do {       \
        bslmt::ReaderWriterLockAssert_Imp::assertIsLockedWrite(               \
            (rwLock_p),                                                       \
            "BSLMT_READERWRITERLOCKASSERT_IS_LOCKED_WRITE(" #rwLock_p ")",    \
            __FILE__,                                                         \
            __LINE__,                                                         \
            bsls::Assert::k_LEVEL_ASSERT); } while (false)
#else
    #define BSLMT_READERWRITERLOCKASSERT_IS_LOCKED_WRITE(rwLock_p) ((void) 0)
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
    #define BSLMT_READERWRITERLOCKASSERT_IS_LOCKED_WRITE_SAFE(rwLock_p) do {  \
        bslmt::ReaderWriterLockAssert_Imp::assertIsLockedWrite(               \
       (rwLock_p),                                                            \
       "BSLMT_READERWRITERLOCKASSERT_IS_LOCKED_WRITE_SAFE(" #rwLock_p ")",    \
       __FILE__,                                                              \
       __LINE__,                                                              \
         bsls::Assert::k_LEVEL_SAFE); } while (false)
#else
    #define BSLMT_READERWRITERLOCKASSERT_IS_LOCKED_WRITE_SAFE(rwLock_p)       \
                                                                     ((void) 0)
#endif

#if defined(BSLS_ASSERT_OPT_IS_ACTIVE)
    #define BSLMT_READERWRITERLOCKASSERT_IS_LOCKED_WRITE_OPT(rwLock_p) do {   \
        bslmt::ReaderWriterLockAssert_Imp::assertIsLockedWrite(               \
        (rwLock_p),                                                           \
        "BSLMT_READERWRITERLOCKASSERT_IS_LOCKED_WRITE_OPT(" #rwLock_p ")",    \
        __FILE__,                                                             \
        __LINE__,                                                             \
        bsls::Assert::k_LEVEL_OPT); } while (false)
#else
    #define BSLMT_LOCKASSERT_IS_LOCKED_WRITE_OPT(rwLock_p) ((void) 0)
#endif

// ----------------------------------------------------------------------------

namespace BloombergLP {
namespace bslmt {

                          // ================================
                          // class ReaderWriterLockAssert_Imp
                          // ================================

struct ReaderWriterLockAssert_Imp {
    // This 'struct' provides a (component private) namespace for
    // implementation functions of the assert macros defined in this component.
    // This class should *not* be used directly in client code.

    // CLASS METHODS
    template <class RW_LOCK>
    static void assertIsLocked(RW_LOCK    *rwLock,
                               const char *text,
                               const char *file,
                               int         line,
                               const char *level);
        // If the specified 'rwLock' is not locked (i.e., neither a read lock
        // or a write lock), call 'bsls::Assert::invokeHandler' with the
        // specified 'text', 'file', 'line', and 'level', where 'text' is text
        // describing the assertion being performed, 'file' is the name of the
        // source file that called the macro, 'line' is the line number in the
        // file where the macro was called, and 'level' is one of the
        // 'bslsAssert::k_LEVEL_*' string literals.  This function is intended
        // to implement 'BSLMT_READERWRITELOCKASSERT_IS_LOCKED',
        // 'BSLMT_READERWRITELOCKASSERT_IS_LOCKED_SAFE', and
        // 'BSLMT_READERWRITELOCKASSERT_IS_LOCKED_OPT' and should not otherwise
        // be called directly.

    template <class RW_LOCK>
    static void assertIsLockedRead(RW_LOCK    *rwLock,
                                   const char *text,
                                   const char *file,
                                   int         line,
                                   const char *level);
        // If the specified 'rwLock' is not locked for reading, call
        // 'bsls::Assert::invokeHandler' with the specified 'text', 'file',
        // 'line', and 'level', where 'text' is text describing the assertion
        // being performed, 'file' is the name of the source file that called
        // the macro, 'line' is the line number in the file where the macro was
        // called, and 'level' is one of the 'bslsAssert::k_LEVEL_*' string
        // literals.  This function is intended to implement
        // 'BSLMT_READERWRITELOCKASSERT_IS_LOCKED_READ',
        // 'BSLMT_READERWRITELOCKASSERT_IS_LOCKED_READ_SAFE', and
        // 'BSLMT_READERWRITELOCKASSERT_IS_LOCKED_READ_OPT' and should not
        // otherwise be called directly.

    template <class RW_LOCK>
    static void assertIsLockedWrite(RW_LOCK     *rwLock,
                                    const char  *text,
                                    const char  *file,
                                    int          line,
                                    const char  *level);
        // If the specified 'rwLock' is not locked for writing, call
        // 'bsls::Assert::invokeHandler' with the specified 'text', 'file',
        // 'line', and 'level', where 'text' is text describing the assertion
        // being performed, 'file' is the name of the source file that called
        // the macro, 'line' is the line number in the file where the macro was
        // called, and 'level' is one of the 'bsls::Assert::k_LEVEL_*' string
        // literals.  This function is intended to implement
        // 'BSLMT_WRITEERWRITELOCKASSERT_IS_LOCKED_READ',
        // 'BSLMT_WRITEERWRITELOCKASSERT_IS_LOCKED_READ_SAFE', and
        // 'BSLMT_WRITEERWRITELOCKASSERT_IS_LOCKED_READ_OPT' and should not
        // otherwise be called directly.

    static bool isValidLevel(const char *level);
        // Return 'true' if the specified 'level' compares equal to one of the
        // 'bsls::Assert::k_LEVEL_*' string literals, and 'false' otherwise.
};

// ============================================================================
//                      INLINE DEFINITIONS
// ============================================================================

                        // --------------------------------
                        // class ReaderWriterLockAssert_Imp
                        // --------------------------------


// CLASS METHODS
template <class RW_LOCK>
inline
void ReaderWriterLockAssert_Imp::assertIsLocked(RW_LOCK    *rwLock,
                                                const char *text,
                                                const char *file,
                                                int         line,
                                                const char *level)
{
    BSLS_ASSERT(rwLock);
    BSLS_ASSERT(text);
    BSLS_ASSERT(file);
    BSLS_ASSERT(level);
    BSLS_ASSERT(isValidLevel(level));

    if (!rwLock->isLocked()) {
        bsls::AssertViolation violation(text, file, line, level);
        bsls::Assert::invokeHandler(violation);
    }
}

template <class RW_LOCK>
inline
void ReaderWriterLockAssert_Imp::assertIsLockedRead(RW_LOCK    *rwLock,
                                                    const char *text,
                                                    const char *file,
                                                    int         line,
                                                    const char *level)
{
    BSLS_ASSERT(rwLock);
    BSLS_ASSERT(text);
    BSLS_ASSERT(file);
    BSLS_ASSERT(level);
    BSLS_ASSERT(isValidLevel(level));

    if (!rwLock->isLockedRead()) {
        bsls::AssertViolation violation(text, file, line, level);
        bsls::Assert::invokeHandler(violation);
    }
}

template <class RW_LOCK>
inline
void ReaderWriterLockAssert_Imp::assertIsLockedWrite(RW_LOCK    *rwLock,
                                                     const char *text,
                                                     const char *file,
                                                     int         line,
                                                     const char *level)
{
    BSLS_ASSERT(rwLock);
    BSLS_ASSERT(text);
    BSLS_ASSERT(file);
    BSLS_ASSERT(level);
    BSLS_ASSERT(isValidLevel(level));

    if (!rwLock->isLockedWrite()) {
        bsls::AssertViolation violation(text, file, line, level);
        bsls::Assert::invokeHandler(violation);
    }
}

inline
bool ReaderWriterLockAssert_Imp::isValidLevel(const char *level)
{
    BSLS_ASSERT(level);

    return 0 == native_std::strcmp(level, bsls::Assert::k_LEVEL_SAFE)
        || 0 == native_std::strcmp(level, bsls::Assert::k_LEVEL_OPT)
        || 0 == native_std::strcmp(level, bsls::Assert::k_LEVEL_ASSERT)
        || 0 == native_std::strcmp(level, bsls::Assert::k_LEVEL_INVOKE);

}

}  // close package namespace
}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2019 Bloomberg Finance L.P.
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
