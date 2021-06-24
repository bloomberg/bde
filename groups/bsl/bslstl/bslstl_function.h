// bslstl_function.h                                                  -*-C++-*-
#ifndef INCLUDED_BSLSTL_FUNCTION
#define INCLUDED_BSLSTL_FUNCTION

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a polymorphic function object with a specific prototype.
//
//@CLASSES:
// bsl::function: polymorphic function object with a specific prototype.
//
//@DESCRIPTION: This component provides a single class template,
// 'bsl::function', implementing the standard template 'std::function', a
// runtime-polymorphic wrapper that encapsulates an arbitrary callable object
// (the *target*) and allows the wrapped object to be invoked.  'bsl::function'
// extends 'std::function' by adding allocator support in a manner consistent
// with standards proposal P0987 (http://wg21.link/P0987).
//
// Objects of type 'bsl::function' generalize the notion of function pointers
// and are generally used to pass callbacks to a non-template function or
// class.  For example, 'bsl::function<RET (ARG1, ARG2, ...)>' can be used
// similarly to 'RET (*)(ARG1, ARG2, ...)' but, unlike the function pointer,
// the 'bsl::function' can hold a non-function callable type such as pointer to
// member function, pointer to member data, lambda expression, or functor
// (class type having an 'operator()').  A 'bsl::function' can also be "empty",
// i.e., having no target object.  In a 'bool' context, a 'bsl::function'
// object will evaluate to false if it is empty, and true otherwise.  The
// target type is determined at runtime using *type* *erasure* in the
// constructors and can be changed by means of assignment, but the function
// prototype (argument types and return type) is specified as a template
// parameter at compile time.
//
// An instantiation of 'bsl::function' is an in-core value-semantic type whose
// salient attributes are the type and value of its target, if any.  The
// 'bsl::function' owns the target object and manages its lifetime; copying or
// moving the 'bsl::function' object copies or moves the target and destroying
// the 'bsl::function' destroys the target.  Somewhat counter-intuitively, the
// target is always mutable within the 'bsl::function'; when wrapping a class
// type, calling a 'bsl::function' can modify its target object, even if the
// 'bsl::function' itself is const-qualified.
//
// Although, as a value-semantic type, 'bsl::function' does have an abstract
// notion of "value", there is no general equality operator comparing between
// two 'bsl::function' objects.  This limitation is a consequence of the target
// type not being required to provide equality comparison operators.  The
// 'operator==' overloads that *are* provided compare a 'bsl::function' against
// the null pointer and do not satisfy the requirements we typically expect for
// value-semantic equality operators.
//
///Invocation
///----------
// Calling an empty 'bsl::function' object will cause it to throw a
// 'bsl::bad_function_call' exception.  Given a non-empty object of type
// 'bsl::function<RET(ARG0, ARG1, ...)>' invoked with arguments 'arg0', 'arg1',
// ..., invocation of the target follows the definition of *INVOKE* in section
// [func.require] of the C++ standard.  These rules are summarized in the
// following table:
//..
//  +----------------------------+-----------------------+
//  | Type of target object, 'f' | Invocation expression |
//  +============================+=======================+
//  | Functor, function, or      | f(arg0, arg1, ...)    |
//  | pointer to function        |                       |
//  +----------------------------+-----------------------+
//  | Pointer to member function | (arg0X.*f)(arg1, ...) |
//  +----------------------------+-----------------------+
//  | Pointer to member data     | arg0X.*f              |
//  +----------------------------+-----------------------+
//..
// The arguments to 'f' must be implicitly convertible from the corresponding
// argument types 'ARG0', 'ARG1', ... and the return value of the call
// expression must be implicitly convertible to 'RET', unless 'RET' is 'void'.
//
// In the case of a pointer to member function, 'R (T::*f)(...)', or pointer to
// data member 'R T::*f', 'arg0X' is one of the following:
//
//: o 'arg0' if 'ARG0' is 'T' or derived from 'T'
//: o 'arg0.get()' if 'ARG0' is a specialization of 'reference_wrapper'
//: o '(*arg0)' if 'ARG0' is a pointer type or pointer-like type (e.g., a smart
//:   pointer).
//
// Note that, consistent with the C++ Standard definition of *INVOKE*, we
// consider pointer-to-member-function and pointer-to-member-data types to be
// "callable" even though, strictly speaking, they cannot be called directly
// due to the lack of an 'operator()'.
//
///Allocator Usage
///---------------
// The C++11 standard specified a type erasure scheme for allocator support in
// 'std::function'.  This specification was never implemented by any vendor or
// popular open-source standard library and allocator support was removed from
// the 2017 standard version of 'std::function'.  A new design for
// allocator support using 'std::pmr::polymorphic_allocator' instead of type
// erasure is currently part of version 3 of the Library Fundamentals Technical
// Specification (LFTS 3), after acceptance of paper P0987
// (http://wg21.link/P0987).  This component follows the P0987 specification,
// substituting 'bsl::allocator' for 'std::pmr::polymorphic_allocator'.
//
// 'bsl::function' meets the requirements for an allocator-aware type.
// Specifically:
//
//: o The type 'allocator_type' is an alias for 'bsl::allocator<char>',
//: o Every constructor can be invoked with an allocator argument, using the
//:   'bsl::allocator_arg_t' leading-allocator argument convention.
//: o 'get_allocator()' returns the allocator specified at construction.
//
// There are two uses for the allocator in 'bsl::function':
//
//: 1 To allocate storage for holding the target object.
//: 2 To pass to the constructor of the wrapped object if the wrapped object is
//:   allocator aware.
//
///Small-object Optimization
///-------------------------
// A 'bsl::function' class has a buffer capable of holding a small callable
// object without allocating dynamic memory.  The buffer is guaranteed to be
// large enough to hold a pointer to function, pointer to member function,
// pointer to member data, a 'bsl::reference_wrapper', or a stateless functor.
// In practice, it is large enough to hold many stateful functors up to six
// times the size of a 'void *'.  Note that, even if the target object is
// stored in the small object buffer, memory might still be allocated by the
// target object itself.
//
// There are only two circumstances under which 'bsl::function' will store the
// target object in allocated memory:
//
//: 1 If the object is too large to fit into the small object buffer
//: 2 If the object has a move constructor that might throw an exception
//
// The second restriction allows the move constructor and swap operation on
// 'bsl::function' to be 'noexcept', as required by the C++ Standard.
//
///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: Polymorphic Invocation
///- - - - - - - - - - - - - - - - -
// In this example, we create a single 'bsl::function' object, then assign it
// to callable objects of different types at run time.
//
// First, we define a simple function that returns the XOR of its two integer
// arguments:
//..
//  int intXor(int a, int b) { return a ^ b; }
//..
// Next, we create a 'bsl::function' that takes two integers and returns an
// integer.  Because we have not initialized the object with a target, it
// starts out as empty and evaluates to false in a Boolean context:
//..
//  void main()
//  {
//      bsl::function<int(int, int)> funcObject;
//      assert(! funcObject);
//..
// Next, we use assignment to give it the value of (a pointer to) 'intXor' and
// test that we can invoke it to get the expected result:
//..
//      funcObject = intXor;
//      assert(funcObject);
//      assert(5 == funcObject(6, 3));
//..
// Next, we assign an instance of 'std::plus<int>' functor to 'funcObject',
// which then holds a copy of it, and again test that we get the expected
// result when we invoke 'funcObject'.
//..
//      funcObject = std::plus<int>();
//      assert(funcObject);
//      assert(9 == funcObject(6, 3));
//..
// Then, if we are using C++11 or later, we assign it to a lambda expression
// that multiplies its arguments:
//..
//    #if BSLS_COMPILERFEATURES_CPLUSPLUS >= 201103L
//      funcObject = [](int a, int b) { return a * b; };
//      assert(funcObject);
//      assert(18 == funcObject(6, 3));
//    #endif
//..
// Finally, we assign 'funcObject' to 'nullptr', which makes it empty again:
//..
//      funcObject = bsl::nullptr_t();
//      assert(! funcObject);
//  }
//..
//
///Example 2: Use in Generic a Algorithm
///- - - - - - - - - - - - - - - - - - -
// Suppose we want to define an algorithm that performs a mutating operation on
// every element of an array of integers.  The inputs are pointers to the first
// and last element to transform, a pointer to the first element into which the
// to write the output, and an operation that takes an integer in and produces
// an integer return value.  Although the pointer arguments have known type
// ('int *'), the type of the transformation operation can be anything that can
// be called with an integral argument and produces an integral return value.
// We do not want to accept this operation as a template argument, however
// (perhaps because our algorithm is sufficiently complex and/or proprietary
// that we want to keep it out of header files).  We solve these disparate
// requirements by passing the operation as a 'bsl::function' object, whose
// type is known at compile time but which can be set to an arbitrary
// operation at run time:
//..
//  void myAlgorithm(const int                      *begin,
//                   const int                      *end,
//                   int                            *output,
//                   const bsl::function<int(int)>&  op);
//      // Apply my special algorithm to the elements in the contiguous address
//      // range from the specified 'begin' pointer up to but not including the
//      // specified 'end' pointer, writing the result to the contiguous range
//      // starting at the specified 'output' pointer.  The specified 'op'
//      // function is applied to each element before it is fed into the
//      // algorithm.
//..
// For the purpose of illustration, 'myAlgorithm' is a simple loop that
// invokes the specified 'op' on each element in the input range and writes it
// directly to the output:
//..
//  void myAlgorithm(const int                      *begin,
//                   const int                      *end,
//                   int                            *output,
//                   const bsl::function<int(int)>&  op)
//  {
//      for (; begin != end; ++begin) {
//          *output++ = op(*begin);
//      }
//  }
//..
// Next, we define input and output arrays to be used throughout the rest of
// this example:
//..
//  static const std::size_t DATA_SIZE = 5;
//  static const int         testInput[DATA_SIZE] = { 4, 3, -2, 9, -7 };
//  static int               testOutput[DATA_SIZE];
//..
// Next, we define a function that simply negates its argument:
//..
//  long negate(long v) { return -v; }
//      // Return the arithmetic negation of the specified 'v' integer.
//..
// Then, we test our algorithm using our negation function:
//..
//  bool testNegation()
//      // Test the use of the 'negation' function with 'myAlgorithm'.
//  {
//      myAlgorithm(testInput, testInput + DATA_SIZE, testOutput, negate);
//
//      for (std::size_t i = 0; i < DATA_SIZE; ++i) {
//          if (-testInput[i] != testOutput[i]) {
//              return false;                                         // RETURN
//          }
//      }
//      return true;
//  }
//..
// Note that the prototype for 'negate' is not identical to the prototype used
// to instantiate the 'op' argument in 'myAlgorithm'.  All that is required is
// that each argument to 'op' be convertible to the corresponding argument in
// the function and that the return type of the function be convertible to the
// return type of 'op'.
//
// Next, we get a bit more sophisticated and define an operation that produces
// a running sum over its inputs.  A running sum requires holding on to state,
// so we define a functor class for this purpose:
//..
//  class RunningSum {
//      // Keep a running total of all of the inputs provided to 'operator()'.
//
//      // DATA
//      int d_sum;
//
//    public:
//      // CREATORS
//      explicit RunningSum(int initial = 0) : d_sum(initial) { }
//          // Create a 'RunningSum' with initial value set to the specified
//          // 'initial' argument.
//
//      // MANIPULATORS
//      int operator()(int v)
//          // Add the specified 'v' to the running sum and return the running
//          // sum.
//          { return d_sum += v; }
//  };
//..
// Then, we test 'myAlgorithm' with 'RunningSum':
//..
//  bool testRunningSum()
//      // Test the user of 'RunningSum' with 'myAlgorithm'.
//  {
//      myAlgorithm(testInput, testInput+DATA_SIZE, testOutput, RunningSum());
//
//      int sum = 0;
//      for (std::size_t i = 0; i < DATA_SIZE; ++i) {
//          sum += testInput[i];
//          if (sum != testOutput[i]) {
//              return false;                                         // RETURN
//          }
//      }
//      return true;
//  }
//..
// Note that 'RunningSum::operator()' is a mutating operation and that, within
// 'myAlgorithm', 'op' is const.  Even though 'bsl::function' owns a copy of
// its target, logical constness does not apply, as per the standard.
//
// Finally, we run our tests and validate the results:
//..
//  void main()
//  {
//      assert(testNegation());
//      assert(testRunningSum());
//  }
//..
//
///Example 3: A Parallel Work queue
///- - - - - - - - - - - - - - - -
// In this example, we'll simulate a simple library whereby worker threads take
// work items from a queue and execute them asynchronously.  This simulation is
// single-threaded, but keeps metrics on how much work each worker accomplished
// so that we can get a rough idea of how much parallelism was expressed by the
// program.
//
// We start by defining a work item type to be stored in our work queue.  This
// type is simply a 'bsl::function' taking a 'WorkQueue' pointer argument and
// returning 'void'.
//..
//  class WorkQueue;  // Forward declaration
//
//  typedef bsl::function<void(WorkQueue *)> WorkItem;
//..
// Next, we define a work queue class.  For simplicity, we'll implement our
// queue as a fixed-sized circular buffer and (because this is a
// single-threaded simulation), ignore synchronization concerns.
//..
//  class WorkQueue {
//      // A FIFO queue of tasks to be executed.
//
//      // PRIVATE CONSTANTS
//      static const int k_MAX_ITEMS = 16;
//
//      // DATA
//      int      d_numItems;
//      int      d_head;
//      WorkItem d_items[k_MAX_ITEMS];
//
//    public:
//      // CREATORS
//      WorkQueue()
//          // Create an empty work queue.
//          : d_numItems(0), d_head(0) { }
//
//      // MANIPULATORS
//      void dequeue(WorkItem *result)
//          // Move the work item at the head of the queue into the specified
//          // 'result' and remove it from the queue.  The behavior is
//          // undefined if this queue is empty.
//      {
//          assert(d_numItems > 0);
//          *result = bslmf::MovableRefUtil::move(d_items[d_head]);
//          d_head = (d_head + 1) % k_MAX_ITEMS;  // circular
//          --d_numItems;
//      }
//
//      void enqueue(bslmf::MovableRef<WorkItem> item)
//          // Enqueue the specified 'item' work item onto the tail of the
//          // queue.  The work is moved from 'item'.
//      {
//          int tail = (d_head + d_numItems++) % k_MAX_ITEMS; // circular
//          assert(d_numItems <= k_MAX_ITEMS);
//          d_items[tail] = bslmf::MovableRefUtil::move(item);
//      }
//
//      // ACCESSORS
//      bool isEmpty() const
//          // Return true if there are no items in the queue; otherwise return
//          // false.
//          { return 0 == d_numItems; }
//
//      int size() const
//          // Return the number of items currently in the queue.
//          { return d_numItems; }
//  };
//..
// Next, we'll create a worker class that represents the state of a worker
// thread:
//..
//  class Worker {
//      // A simulated worker thread.
//
//      // DATA
//      bool d_isIdle;             // True if the worker is idle
//
//    public:
//      // CREATORS
//      Worker()
//          // Create an idle worker.
//          : d_isIdle(true) { }
//
//      // MANIPULATORS
//      void run(WorkQueue *queue);
//          // Dequeue a task from the specified 'queue' and execute it
//          // (asynchronously, in theory).  The behavior is undefined unless
//          // this worker is idle before the call to 'run'.
//
//      // ACCESSORS
//      bool isIdle() const
//          // Return whether this worker is idle.  An idle worker is one that
//          // can except work.
//          { return d_isIdle; }
//  };
//..
// Next, we implement the 'run' function, which removes a 'bsl::function'
// object from the work queue and then executes it, passing the work queue as
// the sole argument:
//..
//  void Worker::run(WorkQueue *queue)
//  {
//      if (queue->isEmpty()) {
//          // No work to do
//          return;                                                   // RETURN
//      }
//
//      WorkItem task;
//      queue->dequeue(&task);
//
//      d_isIdle = false;  // We're about to do work.
//      task(queue);       // Do the work.
//      d_isIdle = true;   // We're idle again.
//  }
//..
// Now, we implement a simple scheduler containing a work queue and an array of
// four workers, which are run in a round-robin fashion:
//..
//  class Scheduler {
//      // Parallel work scheduler.
//
//      // PRIVATE CONSTANTS
//      static const int k_NUM_WORKERS = 4;
//
//      // DATA
//      WorkQueue d_workQueue;
//      Worker    d_workers[k_NUM_WORKERS];
//
//    public:
//      // CREATORS
//      explicit Scheduler(bslmf::MovableRef<WorkItem> initialTask)
//          // Create a scheduler and enqueue the specified 'initialTask'.
//      {
//          d_workQueue.enqueue(bslmf::MovableRefUtil::move(initialTask));
//      }
//
//      // MANIPULATORS
//      void run();
//          // Execute the tasks in the work queue (theoretically in parallel)
//          // until the queue is empty.
//  };
//..
// Next, we implement the scheduler's 'run' method: which does a round-robin
// scheduling of the workers, allowing each to pull work off of the queue and
// run it.  As tasks are run, they may enqueue more work.  The scheduler
// returns when there are no more tasks in the queue.
//..
//  void Scheduler::run()
//  {
//      while (! d_workQueue.isEmpty()) {
//          for (int i = 0; i < k_NUM_WORKERS; ++i) {
//              if (d_workers[i].isIdle()) {
//                  d_workers[i].run(&d_workQueue);
//              }
//          }
//      }
//  }
//..
// Next, we create a job for the parallel system to execute.  A popular
// illustration of parallel execution is the quicksort algorithm, which is a
// recursive algorithm whereby the input array is partitioned into a low and
// high half and quicksort is recursively applied, in parallel, to the two
// halves.  We define a class that encapsulates an invocation of quicksort on
// an input range:
//..
//  template <class TYPE>
//  class QuickSortTask {
//      // A functor class to execute parallel quicksort on a contiguous range
//      // of elements of specified 'TYPE' supplied at construction.
//
//      // DATA
//      TYPE *d_begin_p;
//      TYPE *d_end_p;
//
//      // PRIVATE CLASS METHODS
//      static TYPE* partition(TYPE *begin, TYPE *end);
//          // Partition the contiguous range specified by '[begin, end)' and
//          // return an iterator, 'mid', such that every element in the range
//          // '[begin, mid)' is less than '*mid' and every element in the
//          // range '[mid + 1, end)' is not less than '*mid'.  The behavior is
//          // undefined unless 'begin < end'.
//
//    public:
//      // CREATORS
//      QuickSortTask(TYPE *begin, TYPE *end)
//          // Create a task to sort the contiguous range from the item at the
//          // specified 'begin' location up to but not included the item at
//          // the specified 'end' location.
//          : d_begin_p(begin), d_end_p(end) { }
//
//      // MANIPULATORS
//      void operator()(WorkQueue *queue);
//          // Preform the sort in parallel using the specified 'queue' to
//          // enqueue parallel work.
//  };
//..
// Next we implement the 'partition' method, using a variation of the Lomuto
// partition scheme:
//..
//  template <class TYPE>
//  TYPE* QuickSortTask<TYPE>::partition(TYPE *begin, TYPE *end)
//  {
//      using std::swap;
//
//      swap(begin[(end - begin) / 2], end[-1]); // Put pivot at end
//      TYPE& pivot = *--end;
//      TYPE *divider = begin;
//      for (; begin != end; ++begin) {
//          if (*begin < pivot) {
//              swap(*divider, *begin);
//              ++divider;
//          }
//      }
//      swap(*divider, pivot);  // Put pivot in the middle
//      return divider;
//  }
//..
// Then we define the call operator for our task type, which performs the
// quicksort:
//..
//  template <class TYPE>
//  void QuickSortTask<TYPE>::operator()(WorkQueue *queue)
//  {
//      if (d_end_p - d_begin_p < 2) {
//          // Zero or one element. End recursion.
//          return;                                                   // RETURN
//      }
//
//      // Partition returns end iterator for low partition == begin iterator
//      // for high partition.
//      TYPE *mid = partition(d_begin_p, d_end_p);
//
//      // Asynchronously sort the two partitions
//      WorkItem sortLoPart(QuickSortTask(d_begin_p, mid));
//      WorkItem sortHiPart(QuickSortTask(mid + 1, d_end_p));
//      queue->enqueue(bslmf::MovableRefUtil::move(sortLoPart));
//      queue->enqueue(bslmf::MovableRefUtil::move(sortHiPart));
//  }
//..
// Finally, we use our scheduler and our 'QuickSortTask' to sort an array
// initially containing the integers between 1 and 31 in random order:
//..
//  void main()
//  {
//      short data[] = {
//          23, 12, 2, 28, 1, 10, 5, 13, 15, 8, 19, 14, 31, 29, 9, 11, 24, 3,
//          30, 7, 17, 27, 20, 21, 18, 4, 22, 25, 16, 6, 26
//      };
//
//      static const int DATA_SIZE = sizeof(data) / sizeof(data[0]);
//
//      WorkItem  initialTask(QuickSortTask<short>(data, data + DATA_SIZE));
//      Scheduler sched(bslmf::MovableRefUtil::move(initialTask));
//      sched.run();
//
//      // Validate results
//      for (int i = 0; i < DATA_SIZE; ++i) {
//          assert(i + 1 == data[i]);
//      }
//  }
//..

// Prevent this header from being included directly in 'BSL_OVERRIDES_STD'
// mode.  Doing so is unsupported, and is likely to cause compilation errors.
#if defined(BSL_OVERRIDES_STD) && !defined(BOS_STDHDRS_PROLOGUE_IN_EFFECT)
#error "include <bsl_functional.h> instead of <bslstl_function.h> in \
BSL_OVERRIDES_STD mode"
#endif
#include <bslscm_version.h>

#include <bslma_allocator.h>
#include <bslma_stdallocator.h>
#include <bslma_usesbslmaallocator.h>

#include <bslmf_allocatorargt.h>
#include <bslmf_assert.h>
#include <bslmf_forwardingtype.h>
#include <bslmf_isintegral.h>
#include <bslmf_movableref.h>
#include <bslmf_nestedtraitdeclaration.h>
#include <bslmf_usesallocatorargt.h>
#include <bslmf_util.h>    // 'forward(V)'

#include <bsls_assert.h>
#include <bsls_buildtarget.h>
#include <bsls_compilerfeatures.h>
#include <bsls_exceptionutil.h>
#include <bsls_keyword.h>
#include <bsls_nullptr.h>
#include <bsls_platform.h>
#include <bsls_unspecifiedbool.h>
#include <bsls_util.h>     // 'forward<T>(V)'

#ifdef BDE_BUILD_TARGET_EXC
#include <bslstl_badfunctioncall.h>
#endif
#ifndef BDE_DONT_ALLOW_TRANSITIVE_INCLUDES
#include <bslstl_pair.h>
#endif
#include <bslstl_referencewrapper.h>

// Sub-components:
#ifdef BSLSTL_FUNCTION_VARIADIC_LIMIT
#define BSLSTL_FUNCTION_INVOKERUTIL_VARIADIC_LIMIT \
    BSLSTL_FUNCTION_VARIADIC_LIMIT
#endif
#include <bslstl_function_invokerutil.h>
#include <bslstl_function_isreferencecompatible.h>
#include <bslstl_function_rep.h>
#include <bslstl_function_smallobjectoptimization.h>

#include <cstddef>
#include <cstdlib>
#include <typeinfo>
#include <utility>

#ifndef BDE_DONT_ALLOW_TRANSITIVE_INCLUDES
#include <stdlib.h>  // Import global-scope 'abs(double)'
#endif

// 'BSLS_ASSERT' filename fix -- See {'bsls_assertimputil'}
#ifdef BSLS_ASSERTIMPUTIL_AVOID_STRING_CONSTANTS
namespace BloombergLP {
extern const char s_bslstl_function_h[];
#undef BSLS_ASSERTIMPUTIL_FILE
#define BSLS_ASSERTIMPUTIL_FILE BloombergLP::s_bslstl_function_h
}  // close enterprise namespace
#endif

#if BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
// Include version that can be compiled with C++03
// Generated on Wed Apr 28 15:58:26 2021
// Command line: sim_cpp11_features.pl bslstl_function.h
# define COMPILING_BSLSTL_FUNCTION_H
# include <bslstl_function_cpp03.h>
# undef COMPILING_BSLSTL_FUNCTION_H
#else

// FORWARD DECLARATIONS
namespace bsl {

template <class PROTOTYPE>
class function;
    // Forward declaration.

}  // close namespace bsl

namespace BloombergLP {

#ifndef BDE_OMIT_INTERNAL_DEPRECATED

template <class PROTOTYPE>
class bdef_Function;
    // Forward declaration of legacy 'bdef_Function' in order to implement
    // by-reference conversion from 'bsl::function<F>'.  This declaration
    // produces a by-name cyclic dependency between 'bsl' and 'bde' in order to
    // allow legacy code to transition to 'bsl::function' from (the deprecated)
    // 'bdef_Function'.  The conversion, and therefore this forward reference,
    // should not appear in the open-source version of this component.

#endif // BDE_OMIT_INTERNAL_DEPRECATED

namespace bslstl {

                        // =================================
                        // struct template Function_ArgTypes
                        // =================================

template <class PROTOTYPE>
struct Function_ArgTypes {
    // This component-private struct template provides the following nested
    // typedefs for 'bsl::function' for a specified 'PROTOTYPE' which must be a
    // function type:
    //..
    //  argument_type        -- Only if PROTOTYPE takes exactly one argument
    //  first_argument_type  -- Only if PROTOTYPE takes exactly two arguments
    //  second_argument_type -- Only if PROTOTYPE takes exactly two arguments
    //..
    // The C++ Standard requires that 'function' define these typedefs for
    // compatibility with one- and two-argument legacy (now deprecated) functor
    // adaptors.  'bsl::function' publicly inherits from an instantiation of
    // this template in order to conditionally declare the above nested types.
    // This primary (unspecialized) template provides no typedefs.
};

template <class RET, class ARG>
struct Function_ArgTypes<RET(ARG)> {
    // This component-private specialization of 'Function_ArgTypes' is for
    // function prototypes that take exactly one argument and provides an
    // 'argument_type' nested typedef.

    // PUBLIC TYPES
    typedef ARG argument_type;
};

template <class RET, class ARG1, class ARG2>
struct Function_ArgTypes<RET(ARG1, ARG2)> {
    // This component-private specialization of 'Function_ArgTypes' is for
    // functions that take exactly two arguments and provides
    // 'first_argument_type' and 'second_argument_type' nested typedefs.

    // PUBLIC TYPES
    typedef ARG1 first_argument_type;
    typedef ARG2 second_argument_type;
};

                        // ================================
                        // class template Function_Variadic
                        // ================================

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES // $var-args=13

template <class PROTOTYPE>
class Function_Variadic;  // Primary template is never instantiated

template <class RET, class... ARGS>
class Function_Variadic<RET(ARGS...)> : public Function_ArgTypes<RET(ARGS...)>
{
    // This component-private class template contains the physical
    // representation and provides the variadic interfaces for 'bsl::function'
    // (see class and component documentation for 'bsl::function').
    // 'bsl::function' publicly inherits from an instantiation of this
    // template.  This implementation class exists to 1) minimize the amount of
    // variadic template expansion required in C++03 using the
    // 'sim_cpp11_features.pl' utility and 2) work around issues with the Sun
    // CC compiler, which has trouble with argument type deduction when a
    // template argument has a partial specialization (as 'Function_Variadic'
    // does).  'bsl::function' does not have a partial specialization and
    // delegates to the 'Function_Variadic' base class only those parts of the
    // interface and implementation that depend on decomposing the function
    // prototype into a return type and variadic list of argument types.

    // PRIVATE TYPES
    typedef bslstl::Function_Rep Function_Rep;

    typedef RET Invoker(const Function_Rep *,
                        typename bslmf::ForwardingType<ARGS>::Type...);
        // Type of invocation function.  A generic function pointer is stored
        // in the representation and is cast to this type to invoke the
        // specific type of target stored in this wrapper.

    // 'protected' to workaround a Sun bug when instantiating 'bsl::function'
    // implicitly from an 'extern "C"' function pointer, e.g. in a 'bind'
    // expression.
  protected:
    // DATA
    Function_Rep d_rep;   // Non-templated representation

  private:
    // NOT IMPLEMENTED
    Function_Variadic(const Function_Variadic&) BSLS_KEYWORD_DELETED;
    Function_Variadic&
    operator=(const Function_Variadic&) BSLS_KEYWORD_DELETED;
        // This component-private base class is not directly copyable.

    // FRIENDS
    friend class bsl::function<RET(ARGS...)>;

  public:
    // PUBLIC TYPES
    typedef RET                          result_type;
    typedef Function_Rep::allocator_type allocator_type;

    // CREATORS
    Function_Variadic(const allocator_type& allocator);
        // Create an empty object.  Use the specified 'allocator' (e.g., the
        // address of a 'bslma::Allocator') to supply memory.

    //! ~Function_Variadic() = default;
    //    // Destroy this object and its target object.

    // MANIPULATORS
    RET operator()(ARGS... args) const;
        // If this object is empty, throw 'bsl::bad_function_call'; otherwise
        // invoke the target object with the specified 'args...' and return the
        // result (after conversion to 'RET').  Note that, even though it is
        // declared 'const', this call operator can mutate the target object
        // and is thus considered a manipulator rather than an accessor.
};

#endif

              // =================================================
              // struct template Function_IsInvocableWithPrototype
              // =================================================

template <class PROTOTYPE, class FUNC>
struct Function_IsInvocableWithPrototype;
    // Forward declaration of the component-private
    // 'Function_IsInvocableWithPrototype' 'struct' template.  The primary
    // (unspecialized) template is not defined.  This 'struct' template
    // implements a boolean metafunction that publicly inherits from
    // 'bsl::true_type' if an object of the specified 'FUNC' type is invocable
    // under the specified 'PROTOTYPE', and inherits from 'bsl::false_type'
    // otherwise.  An object of 'FUNC' type is invocable under the 'PROTOTYPE'
    // if it is Lvalue-Callable with the arguments of the 'PROTOTYPE', and
    // returns an object of type convertible to the return type of the
    // 'PROTOTYPE'.  If the return type of the 'PROTOTYPE' is 'void', then any
    // type is considered convertible to the return type of the 'PROTOTYPE'.
    // In C++03, 'FUNC' is considered Lvalue-Callable with the argument and
    // return types of the 'PROTOTYPE' if it is not an integral type.  This
    // 'struct' template requires 'PROTOTYPE" to be an unqualified function
    // type.

}  // close package namespace
}  // close enterprise namespace

namespace bsl {

                    // =======================
                    // class template function
                    // =======================

template <class PROTOTYPE>
class function : public BloombergLP::bslstl::Function_Variadic<PROTOTYPE> {
    // This class template implements the C++ Standard Library 'std::function'
    // template, enhanced for allocator support as per Standards Proposal
    // P0987.  An instantiation of this template generalizes the notion of a
    // pointer to a function having the specified 'PROTOTYPE' expressed as a
    // function type (e.g., 'int(const char *, float)').  An object of this
    // class wraps a copy of the callable object specified at construction (if
    // any), such as a function pointer, member-function pointer, member-data
    // pointer, or functor object.  The wrapped object (called the *target* or
    // *target* *object*) is owned by the 'bsl::function' object (unlike the
    // function pointer that it mimics).  Invoking the 'bsl::function' object
    // will invoke the target (or throw an exception, if there is no target).
    // Note that 'function' will compile only if 'PROTOTYPE' is a function
    // type.
    //
    // To optimize away many heap allocations, objects of this type have a
    // buffer into which small callable objects can be stored.  In order to
    // qualify for this small-object optimization, a callable type must not
    // only fit in the buffer but must also be nothrow move constructible.  The
    // latter constraint allows this type to be nothrow move constructible and
    // nothrow swappable, as required by the C++ Standard.  The small object
    // buffer is guaranteed to be large enough to hold a pointer to function,
    // pointer to member function, pointer to member data, a
    // 'bsl::reference_wrapper', or an empty struct.  Although the standard
    // does not specify a minimum size beyond the aforementioned guarantee,
    // many small structs will fit in the small object buffer, as defined in
    // the 'bslstl_function_smallobjectoptimization' component.

  private:
    // PRIVATE TYPES
    typedef BloombergLP::bslstl::Function_Variadic<PROTOTYPE> Base;
    typedef BloombergLP::bslstl::Function_Rep                 Function_Rep;
    typedef BloombergLP::bslmf::MovableRefUtil                MovableRefUtil;

    template <class FROM, class TO>
    struct IsReferenceCompatible
    : BloombergLP::bslstl::Function_IsReferenceCompatible<FROM, TO>::type {
        // Abbreviation for metafunction that determines whether a reference
        // from 'FROM' can be cast to a reference to 'TO' without loss of
        // information.
    };

    template <class TYPE>
    struct Decay : MovableRefUtil::Decay<TYPE> {
        // Abbreviation for metafunction used to provide a C++03-compatible
        // implementation of 'std::decay' that treats 'bslmf::MovableReference'
        // as an rvalue reference.
    };

    template <class FUNC>
    struct IsInvocableWithPrototype
    : BloombergLP::bslstl::Function_IsInvocableWithPrototype<PROTOTYPE, FUNC> {
        // Abbreviation for a metafunction used to determine whether an object
        // of the specified 'FUNC' is callable with argument types of the
        // specified 'PROTOTYPE' and returns a type convertible to the return
        // type of the 'PROTOTYPE'.
    };

#ifndef BSLS_COMPILERFEATURES_SUPPORT_OPERATOR_EXPLICIT
    typedef BloombergLP::bsls::UnspecifiedBool<function> UnspecifiedBoolUtil;
    typedef typename UnspecifiedBoolUtil::BoolType       UnspecifiedBool;
        // Unique type that evaluates to true or false in a boolean control
        // construct such as an 'if' or 'while' statement.  In C++03,
        // 'function' is implicitly convertible to this type but is not
        // implicitly convertible to 'bool'.  In C++11 and later, 'function' is
        // explicitly convertible to 'bool', so this type is not needed.

    // NOT IMPLEMENTED
    bool operator==(const function&) const;  // Declared but not defined
    bool operator!=(const function&) const;  // Declared but not defined
        // Since 'function' does not support 'operator==' and 'operator!=',
        // they must be deliberately suppressed; otherwise 'function' objects
        // would be implicitly comparable by implicit conversion to
        // 'UnspecifiedBool'.
#endif // !defined(BSLS_COMPILERFEATURES_SUPPORT_OPERATOR_EXPLICIT)

    // PRIVATE MANIPULATORS
    template <class FUNC>
    void installFunc(BSLS_COMPILERFEATURES_FORWARD_REF(FUNC) func);
        // Set the target of this 'function' by constructing from the specified
        // 'func' callable object.  If the type of 'func' is a movable
        // reference, then the target is constructed by extended move
        // construction; otherwise by extended copy construction.
        // Instantiation will fail unless 'FUNC' is a callable type that is
        // invocable with arguments in 'PROTOTYPE' and yields a return type
        // that is convertible to the return type in 'PROTOTYPE'.

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(function,
                                   BloombergLP::bslma::UsesBslmaAllocator);
    BSLMF_NESTED_TRAIT_DECLARATION(function,
                                   BloombergLP::bslmf::UsesAllocatorArgT);
    BSLMF_NESTED_TRAIT_DECLARATION(function,
                                   bsl::is_nothrow_move_constructible);

    // TYPES
    typedef Function_Rep::allocator_type allocator_type;

    // CREATORS
    function() BSLS_KEYWORD_NOEXCEPT;
    function(nullptr_t) BSLS_KEYWORD_NOEXCEPT;                      // IMPLICIT
    function(allocator_arg_t       ,
             const allocator_type& allocator) BSLS_KEYWORD_NOEXCEPT;
    function(allocator_arg_t       ,
             const allocator_type& allocator,
             nullptr_t             ) BSLS_KEYWORD_NOEXCEPT;
        // Create an empty 'function' object.  Optionally specify an
        // 'allocator' (e.g., the address of a 'bslma::Allocator' object) to
        // supply memory; otherwise, the default allocator is used.

    template <class FUNC>
    function(BSLS_COMPILERFEATURES_FORWARD_REF(FUNC) func,          // IMPLICIT
             typename enable_if<
                    ! IsReferenceCompatible<typename Decay<FUNC>::type,
                                            function>::value
                 &&   IsInvocableWithPrototype<
                                             typename Decay<FUNC>::type>::value
#ifndef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
                 && ! MovableRefUtil::IsMovableReference<FUNC>::value
#endif
#ifdef BSLS_PLATFORM_CMP_IBM
                 && ! is_function<FUNC>::value
#endif
                 , int>::type = 0)
        // Create an object wrapping the specified 'func' callable object.  Use
        // the default allocator to supply memory.  If 'func' is a null pointer
        // or null pointer-to-member, then the resulting object will be empty.
        // This constructor will not participate in overload resolution if
        // 'func' is of the same type as (or reference compatible with) this
        // object (to avoid ambiguity with the copy and move constructors) or
        // is an integral type (to avoid matching null pointer literals).  In
        // C++03, this function will not participate in overload resolution if
        // 'FUNC' is a 'MovableRef' (see overload, below), and instantiation
        // will fail unless 'FUNC' is invocable using the arguments and return
        // type specified in 'PROTOTYPE'.  In C++11 and later, this function
        // will not participate in overload resolution if 'FUNC' is not
        // invocable using the arguments and return type specified in
        // 'PROTOTYPE'.  Note that this constructor implicitly converts from
        // any type that is so invocable.
        : Base(allocator_type())
    {
        ///Implementation Note
        ///- - - - - - - - - -
        // The body of this constructor must be inlined inplace because the use
        // of 'enable_if' will otherwise break the MSVC 2010 compiler.
        //
        // The '! bsl::is_function<FUNC>::value' constraint is required in
        // C++03 mode when using the IBM XL C++ compiler.  In C++03,
        // 'BSLS_COMPILERFEATURES_FORWARD_REF(FUNC) func' expands to
        // 'const FUNC& func'.  A conforming compiler deduces a
        // reference-to-function type for 'func' when it binds to a function
        // argument.  The IBM XL C++ compiler erroneously does not collapse the
        // 'const' qualifier when 'FUNC' is deduced to be a function type, and
        // instead attempts to deduce the type of 'func' to be a reference to a
        // 'const'-qualified function.  This causes substitution to fail
        // because function-typed expressions are never 'const'.  This
        // component solves the problem by accepting a 'func' having a function
        // type as a pointer to a (non-'const') function.  An overload for the
        // corresponding constructor is defined below.

        installFunc(BSLS_COMPILERFEATURES_FORWARD(FUNC, func));
    }

#ifdef BSLS_PLATFORM_CMP_IBM
    template <class FUNC>
    function(FUNC                                            *func, // IMPLICIT
             typename enable_if<is_function<FUNC>::value, int>::type = 0)

        : Base(allocator_type())
    {
        ///Implementation Note
        ///- - - - - - - - - -
        // This constructor overload only exists to work around an IBM XL C++
        // compiler defect.  See the implementation notes for the above
        // constructor overload for more information.
        //
        // This constructor also forwards the 'func' as a pointer-to-function
        // type to downstream operations in order to work around the
        // aforementioned reference-to-function type deduction defects.
        //
        // Further, note that instantiation of this constructor will fail
        // unless 'FUNC' is invocable using the arguments and return type
        // specified in 'PROTOTYPE'.  This component assumes that the IBM XL
        // C++ compiler does not support C++11 or later.

        installFunc(func);
    }
#endif

#ifndef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
    template <class FUNC>
    explicit function(const BloombergLP::bslmf::MovableRef<FUNC>& func,
             typename enable_if<
                    ! IsReferenceCompatible<typename Decay<FUNC>::type,
                                            function>::value
                 &&   IsInvocableWithPrototype<
                                             typename Decay<FUNC>::type>::value
                 , int>::type = 0)
        // Create an object wrapping the specified 'func' callable object.
        // This constructor (ctor 2) is identical to the previous constructor
        // (ctor 1) except that, in C++03 ctor 2 provides for explicit
        // construction from a 'MovableRef' referencing a callable type, rather
        // than an implicit conversion for 'FUNC' not being a 'MovableRef'.  In
        // C++11, overload resolution matching an argument of type 'T&&' to a
        // parameter of type 'T' (exact match) is always preferred over
        // matching 'T&&' to 'bsl::function' (conversion).  In C++03, however
        // 'MovableRef' is not a real reference type, so it sometimes creates
        // overload ambiguities whereby matching 'MovableRef<T>' to 'T'
        // (conversion) is no better than matching 'MovableRef<T>' to
        // 'bsl::function' (also conversion).  This ambiguity is resolved by
        // making this constructor from 'MovableRef<T>' explicit, while leaving
        // other constructor from 'FUNC' implicit.  This means that
        // 'move' will fail in a narrow set of cases in C++03, as shown below:
        //..
        //  typedef bsl::function<void(int)> Obj;
        //  MyCallableType x;
        //
        //  Obj f1 = x;                              // OK
        //  Obj f2 = bslmf::MovableRefUtil::move(x); // No conversion in C++03
        //  Obj f3(bslmf::MovableRefUtil::move(x));  // OK, normal ctor call
        //
        //  void y(const Obj& f);
        //  y(x);                                    // OK
        //  y(bslmf::MovableRefUtil::move(x));       // Not found in C++03
        //  y(Obj(bslmf::MovableRefUtil::move(x)));  // OK, explicit cast
        //..
        // As you can see from the examples above, there are simple workarounds
        // for the problem cases, although generic code might need to be extra
        // careful.
        : Base(allocator_type())
    {
        ///Implementation Note
        ///- - - - - - - - - -
        // The body of this constructor must inlined inplace because the use of
        // 'enable_if' will otherwise break the MSVC 2010 compiler.

        installFunc(BloombergLP::bslmf::MovableRefUtil::move(func));
    }
#endif

    template <class FUNC>
    function(allocator_arg_t,
             const allocator_type&                   allocator,
             BSLS_COMPILERFEATURES_FORWARD_REF(FUNC) func,
             typename enable_if<
                    ! IsReferenceCompatible<typename Decay<FUNC>::type,
                                            function>::value
                 &&   IsInvocableWithPrototype<
                                             typename Decay<FUNC>::type>::value
#ifdef BSLS_PLATFORM_CMP_IBM
                 && ! is_function<FUNC>::value
#endif
                 , int>::type = 0)
        // Create an object wrapping the specified 'func' callable object.  Use
        // the specified 'allocator' (i.e., the address of a 'bslma::Allocator'
        // object) to supply memory.  If 'func' is a null pointer or null
        // pointer-to-member, then the resulting object will be empty.  This
        // constructor will not participate in overload resolution if 'func' is
        // of the same type as (or reference compatible with) this object (to
        // avoid ambiguity with the extended copy and move constructors) or is
        // an integral type (to avoid matching null pointer literals).  In
        // C++03, this function will not participate in overload resolution if
        // 'FUNC' is a 'MovableRef' (see overload, below), and instantiation
        // will fail unless 'FUNC' is invocable using the arguments and return
        // type specified in 'PROTOTYPE'.  In C++11 and later, this function
        // will not participate in overload resolution if 'FUNC' is not
        // invocable using the arguments and return type specified in
        // 'PROTOTYPE'.  Note that this constructor implicitly converts from
        // any type that is so invocable.
        : Base(allocator)
    {
        ///Implementation Note
        ///- - - - - - - - - -
        // The body of this constructor must inlined inplace because the use of
        // 'enable_if' will otherwise break the MSVC 2010 compiler.
        //
        // The '! bsl::is_function<FUNC>::value' constraint is required in
        // C++03 mode when using the IBM XL C++ compiler.  In C++03,
        // 'BSLS_COMPILERFEATURES_FORWARD_REF(FUNC) func' expands to
        // 'const FUNC& func'.  A conforming compiler deduces a
        // reference-to-function type for 'func' when it binds to a function
        // argument.  The IBM XL C++ compiler erroneously does not collapse the
        // 'const' qualifier when 'FUNC' is deduced to be a function type, and
        // instead attempts to deduce the type of 'func' to be a reference to a
        // 'const'-qualified function.  This causes substitution to fail
        // because function-typed expressions are never 'const'.  This
        // component solves the problem by accepting a 'func' having a function
        // type as a pointer to a (non-'const') function.  An overload for the
        // corresponding constructor is defined below.

        installFunc(BSLS_COMPILERFEATURES_FORWARD(FUNC, func));
    }

#ifdef BSLS_PLATFORM_CMP_IBM
    template <class FUNC>
    function(allocator_arg_t,
             const allocator_type&                                   allocator,
             FUNC                                                   *func,
             typename enable_if<is_function<FUNC>::value, int>::type = 0)
        : Base(allocator)
    {
        ///Implementation Note
        ///- - - - - - - - - -
        // This constructor overload only exists to work around an IBM XL C++
        // compiler defect.  See the implementation notes for the above
        // constructor overload for more information.
        //
        // This constructor also forwards the 'func' as a pointer-to-function
        // type to downstream operations in order to work around the
        // aforementioned reference-to-function type deduction defects.
        //
        // Further, note that instantiation of this constructor will fail
        // unless 'FUNC' is invocable using the arguments and return type
        // specified in 'PROTOTYPE'.  This component assumes that the IBM XL
        // C++ compiler does not support C++11 or later.

        installFunc(func);
    }
#endif

    function(const function&       original);
    function(allocator_arg_t       ,
             const allocator_type& allocator,
             const function&       original);
        // Create a 'function' having the same value as (i.e., wrapping a copy
        // of the target held by) the specified 'original' object.  Optionally
        // specify an 'allocator' (e.g., the address of a 'bslma::Allocator'
        // object) to supply memory; otherwise, the default allocator is used.

    function(BloombergLP::bslmf::MovableRef<function> original)
                                             BSLS_KEYWORD_NOEXCEPT; // IMPLICIT
        // Create a 'function' having the same target as the specified
        // 'original' object.  Use 'original.get_allocator()' as the allocator
        // to supply memory.  The 'original' object is set to empty after the
        // new object is created.  If the target qualifies for the small-object
        // optimization (see class-level documentation), then it is
        // move-constructed into the new object; otherwise ownership of the
        // target is transferred without using the target's move constructor.

    function(allocator_arg_t                          ,
             const allocator_type&                    allocator,
             BloombergLP::bslmf::MovableRef<function> original);
        // Create a 'function' having the same value as (i.e., wrapping a copy
        // of the target held by) the specified 'original' object.  Use the
        // specified 'allocator' (e.g., the address of a 'bslma::Allocator'
        // object) to supply memory.  If 'allocator == original.allocator()',
        // this object is created as if by move construction; otherwise it is
        // created as if by extended copy construction using 'allocator'.

    // MANIPULATORS
    function& operator=(const function& rhs);
        // Set the target of this object to a copy of the target (if any)
        // held by the specified 'rhs' object, destroy the target (if any)
        // previously held by '*this', and return '*this'.  The result is
        // equivalent to having constructed '*this' from 'rhs' using the
        // extended copy constructor with allocator 'this->get_allocator()'.
        // If an exception is thrown, '*this' is not modified (i.e., copy
        // assignment provides the strong exception guarantee).

    function& operator=(BloombergLP::bslmf::MovableRef<function> rhs);
        // Set the target of this object to the target (if any) held by the
        // specified 'rhs' object, destroy the target (if any) previously held
        // by '*this', and return '*this'.  The result is equivalent to having
        // constructed '*this' from 'rhs' using the extended move constructor
        // with allocator 'this->get_allocator()'.  If an exception is thrown,
        // 'rhs' will have a valid but unspecified value and '*this' will not
        // be modified.  Note that an exception will never be thrown if
        // 'get_allocator() == rhs.get_allocator()'.

    template <class FUNC>
    typename enable_if<
           ! IsReferenceCompatible<typename Decay<FUNC>::type, function>::value
        &&   IsInvocableWithPrototype<typename Decay<FUNC>::type>::value
     , function&>::type
    operator=(BSLS_COMPILERFEATURES_FORWARD_REF(FUNC) rhs)
        // Set the target of this object to the specified 'rhs' callable
        // object, destroy the previous target (if any), and return '*this'.
        // The result is equivalent to having constructed '*this' from
        // 'std::forward<FUNC>(rhs)' and 'this->get_allocator()'.  Note that
        // this assignment operator will not participate in overload resolution
        // if 'func' is of the same type as this object (to avoid ambiguity
        // with the copy and move assignment operators.)  In C++03,
        // instantiation will fail unless 'FUNC' is invocable with the
        // arguments and return type specified in 'PROTOTYPE'.  In C++11 and
        // later, this assignment operator will not participate in overload
        // resolution unless 'FUNC' is invocable with the arguments and return
        // type specified in 'PROTOTYPE'.
    {
        ///Implementation Note
        ///- - - - - - - - - -
        // The body of this operator must inlined inplace because the use of
        // 'enable_if' will otherwise break the MSVC 2010 compiler.

        function(allocator_arg, this->get_allocator(),
                 BSLS_COMPILERFEATURES_FORWARD(FUNC, rhs)).swap(*this);
        return *this;
    }

#ifdef BSLS_PLATFORM_CMP_IBM
    template <class FUNC>
    typename enable_if<is_function<FUNC>::value, function&>::type
    operator=(FUNC *rhs)
        // Set the target of this object to the specified 'rhs' function
        // pointer.  This overload exists only for the IBM compiler, which has
        // trouble decaying functions to function pointers in
        // pass-by-const-reference template arguments.
    {
        ///Implementation Note
        ///- - - - - - - - - -
        // The body of this operator must inlined inplace.
        //
        // Further, note that instantiation of this assignment operator will
        // fail unless 'FUNC' is invocable using the arguments and return type
        // specified in 'PROTOTYPE'.  This component assumes that the IBM XL
        // C++ compiler does not support C++11 or later.

        function(allocator_arg, this->get_allocator(), rhs).swap(*this);
        return *this;
    }
#endif

    template <class FUNC>
    typename enable_if<
             IsInvocableWithPrototype<typename Decay<FUNC>::type>::value
     , function &>::type
    operator=(bsl::reference_wrapper<FUNC> rhs) BSLS_KEYWORD_NOEXCEPT
        // Destroy the current target (if any) of this object, then set the
        // target to the specified 'rhs' wrapper containing a reference to a
        // callable object and return '*this'.  The result is equivalent to
        // having constructed '*this' from 'rhs' and 'this->get_allocator()'.
        // Note that this assignment is a separate overload only because it is
        // unconditionally 'noexcept'.
    {
        /// Implementation Note
        ///- - - - - - - - - -
        // The body of this operator must inlined inplace because the use of
        // 'enable_if' will otherwise break the MSVC 2010 compiler.

        function(allocator_arg, this->get_allocator(), rhs).swap(*this);
        return *this;
    }

    function& operator=(nullptr_t) BSLS_KEYWORD_NOEXCEPT;
        // Set this object to empty and return '*this'.

    // Inherit 'operator()' from 'Function_Variadic' base class.
    using Base::operator();
        // If this object is empty, throw 'bsl::bad_function_call'; otherwise
        // invoke the target object with the specified 'args...' and return the
        // result (after conversion to 'RET').  Note that, even though it is
        // declared 'const', this call operator can mutate the target object
        // and is thus considered a manipulator rather than an accessor.

    void swap(function& other) BSLS_KEYWORD_NOEXCEPT;
        // Exchange the targets held by this 'function' and the specified
        // 'other' 'function'.  The behavior is undefined unless
        // 'get_allocator() == other.get_allocator()'.

    template<class TP> TP* target() BSLS_KEYWORD_NOEXCEPT;
        // If 'TP' is the same type as the target object, returns a pointer
        // granting modifiable access to the target; otherwise return a null
        // pointer.

    // ACCESSORS
#ifdef BSLS_COMPILERFEATURES_SUPPORT_OPERATOR_EXPLICIT
    explicit  // Explicit conversion available only with C++11
    operator bool() const BSLS_KEYWORD_NOEXCEPT;
        // (C++11 and later) Return false if this object is empty, otherwise
        // return true.  Note that this is an explicit conversion operator and
        // is typically invoked implicitly in contexts such as in the condition
        // of an 'if' or 'while' statement, though it can also be invoked via
        // an explicit cast.
#else
    operator UnspecifiedBool() const BSLS_KEYWORD_NOEXCEPT
        // (C++03 only) Return a null value if this object is empty, otherwise
        // an arbitrary non-null value.  Note that this operator will be
        // invoked implicitly in boolean contexts such as in the condition of
        // an 'if' or 'while' statement, but does not constitute an implicit
        // conversion to 'bool'.
    {
        // Inplace inlined to work around xlC bug when out-of-line.
        return UnspecifiedBoolUtil::makeValue(0 != this->d_rep.invoker());
    }
#endif

    allocator_type get_allocator() const BSLS_KEYWORD_NOEXCEPT;
        // Return (a copy of) the allocator used to supply memory for this
        // 'function'.

    template<class TP> const TP* target() const BSLS_KEYWORD_NOEXCEPT;
        // If 'TP' is the same type as the target object, returns a pointer
        // granting read-only access to the target; otherwise return a null
        // pointer.

    const std::type_info& target_type() const BSLS_KEYWORD_NOEXCEPT;
        // Return 'typeid(void)' if this object is empty; otherwise
        // 'typeid(FUNC)' where 'FUNC' is the type of the target object.

#ifndef BDE_OMIT_INTERNAL_DEPRECATED
    // LEGACY METHODS
    operator BloombergLP::bdef_Function<PROTOTYPE *>&() BSLS_KEYWORD_NOEXCEPT;
        // !DEPRECATED!: Use 'bsl::function' instead of 'bdef_Function'.
        //
        // Return '*this', converted to a mutable 'bdef_Function' reference by
        // downcasting.  The behavior is undefined unless 'bdef_Function<F*>'
        // is derived from 'bsl::function<F>' and adds no new data members.

    operator const BloombergLP::bdef_Function<PROTOTYPE *>&() const
                                                         BSLS_KEYWORD_NOEXCEPT;
        // !DEPRECATED!: Use 'bsl::function' instead of 'bdef_Function'.
        //
        // Return '*this' converted to a const 'bdef_Function' reference by
        // downcasting.  The behavior is undefined unless 'bdef_Function<F*>'
        // is derived from 'bsl::function<F>' and adds no new data members.

    // LEGACY ACCESSORS
    BloombergLP::bslma::Allocator *allocator() const BSLS_KEYWORD_NOEXCEPT;
        // !DEPRECATED!: Use 'get_allocator()' instead.
        //
        // Return 'get_allocator().mechanism()'.  Note that this function
        // exists for BDE compatibility and is not part of the C++ Standard
        // Library.

    bool isInplace() const BSLS_KEYWORD_NOEXCEPT;
        // !DEPRECATED!: Runtime checking of this optimization is discouraged.
        //
        // Return 'true' if this 'function' is empty or if it is non-empty and
        // its target qualifies for the small-object optimization (and is thus
        // allocated within this object's footprint); otherwise, return false.
#endif
};

// FREE FUNCTIONS
template <class PROTOTYPE>
bool operator==(const function<PROTOTYPE>&, nullptr_t) BSLS_KEYWORD_NOEXCEPT;
template <class PROTOTYPE>
bool operator==(nullptr_t, const function<PROTOTYPE>&) BSLS_KEYWORD_NOEXCEPT;
    // Return true if the 'function' argument is empty, otherwise return false.

template <class PROTOTYPE>
bool operator!=(const function<PROTOTYPE>&, nullptr_t) BSLS_KEYWORD_NOEXCEPT;
template <class PROTOTYPE>
bool operator!=(nullptr_t, const function<PROTOTYPE>&) BSLS_KEYWORD_NOEXCEPT;
    // Return false if the 'function' argument is empty, otherwise return true.

template <class PROTOTYPE>
void swap(function<PROTOTYPE>& a,function<PROTOTYPE>& b) BSLS_KEYWORD_NOEXCEPT;
    // Exchange the targets held by the specified 'a' and specified 'b'
    // objects.  The behavior is undefined unless 'a.get_allocator() ==
    // b.get_allocator()'.

}  // close namespace bsl

// ============================================================================
//                     TEMPLATE AND INLINE FUNCTION DEFINITIONS
// ============================================================================

namespace BloombergLP {

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES

                        // --------------------------------
                        // class template Function_Variadic
                        // --------------------------------

// CREATORS
template <class RET, class... ARGS>
inline
bslstl::Function_Variadic<RET(ARGS...)>::
Function_Variadic(const allocator_type& allocator)
    : d_rep(allocator)
{
}

// MANIPULATORS
template <class RET, class... ARGS>
inline
RET bslstl::Function_Variadic<RET(ARGS...)>::operator()(ARGS... args) const
{
    // BDE_VERIFY pragma: push
    // BDE_VERIFY pragma: -SAL01 // Possible strict-aliasing violation
    Invoker *invoker_p = reinterpret_cast<Invoker*>(d_rep.invoker());
    // BDE_VERIFY pragma: pop

#ifdef BDE_BUILD_TARGET_EXC
    if (! invoker_p) {
        throw bsl::bad_function_call();
    }
#else
    // Non-exception build
    BSLS_ASSERT_OPT(invoker_p);
#endif

    // It is not necessary to call 'std::forward<ARGS>' because 'args...'  is
    // not composed of forwarding references.  The arguments to 'invoker_p',
    // however, are not the same as 'args...' but compatible types produced by
    // 'bslmf::ForwardingTypes' for efficiency.
    return invoker_p(&d_rep, args...);
}

#endif

namespace bslstl {

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES // $var-args=13

             // -------------------------------------------------
             // struct template Function_IsInvocableWithPrototype
             // -------------------------------------------------

#ifdef BSLSTL_FUNCTION_INVOKERUTIL_SUPPORT_IS_FUNC_INVOCABLE

template <class RET, class FUNC, class... ARGS>
struct Function_IsInvocableWithPrototype<RET(ARGS...), FUNC>
: Function_InvokerUtil::IsFuncInvocable<RET(ARGS...), FUNC> {
    // This component-private 'struct' template provides a boolean metafunction
    // that derives from 'bsl::true_type' if a 'bsl::function' object having a
    // prototype of 'RET(ARGS...)' is constructible from an object of type
    // 'FUNC', and derives from 'bsl::false_type' otherwise.  This metafunction
    // is a wrapper around 'bsl::invoke_result' that unwraps 'FUNC' if it is a
    // specialization of 'bslalg::NothrowMovableWrapper'; and, if
    // 'bsl::invoke_result' provides a nested 'type' typedef for 'FUNC' and
    // 'RET' is non-void, checks that the return type of the invoke operation
    // on 'FUNC' is convertible to 'RET'.
};

#else // if !defined(BSLSTL_FUNCTION_INVOKERUTIL_SUPPORT_IS_FUNC_INVOCABLE)

template <class RET, class FUNC, class... ARGS>
struct Function_IsInvocableWithPrototype<RET(ARGS...), FUNC>
: bsl::integral_constant<bool, !bsl::is_integral<FUNC>::value> {
    // This component-private 'struct' template provides a partial
    // specialization of 'Function_IsInvocableWithPrototype' for any 'FUNC'
    // type, and for 'PROTOTYPE' types that are function types.  This
    // specialization only exists in pre-C++11 (e.g. C++03) compilers.  It
    // approximates a boolean metafunction for detecting whether the specified
    // 'FUNC' type is Lvalue-Callable with the prototype 'RET(ARGS...)'.  This
    // approximation is extremely coarse, and only checks that the 'FUNC' is
    // not an integral type.  It does this for the sole purpose of ensuring
    // that there are no overload resolution ambiguities in the constructors
    // and assignment operators of 'bsl::function', which provide overloads for
    // both integral types (to accept the literal '0' is a null pointer
    // constant), and for callable types like 'FUNC'.
};

#endif // !defined(BSLSTL_FUNCTION_INVOKERUTIL_SUPPORT_IS_FUNC_INVOCABLE)
#endif

}  // close package namespace
}  // close enterprise namespace

                        // ----------------------------
                        // class template bsl::function
                        // ----------------------------

// PRIVATE MANIPULATORS
template <class PROTOTYPE>
template <class FUNC>
inline
void bsl::function<PROTOTYPE>::installFunc(
                                  BSLS_COMPILERFEATURES_FORWARD_REF(FUNC) func)
{
    typedef BloombergLP::bslstl::Function_InvokerUtil InvokerUtil;
    typedef InvokerUtil::GenericInvoker               GenericInvoker;
    typedef typename Decay<FUNC>::type                DecayedFunc;

    const DecayedFunc& decayedFunc = func;  // Force function-to-pointer decay.
    GenericInvoker *const invoker =
        InvokerUtil::invokerForFunc<PROTOTYPE>(decayedFunc);

    this->d_rep.installFunc(BSLS_COMPILERFEATURES_FORWARD(FUNC, func),
                            invoker);
}

// CREATORS
template <class PROTOTYPE>
inline bsl::function<PROTOTYPE>::function() BSLS_KEYWORD_NOEXCEPT
    : Base(allocator_type())
{
}

template <class PROTOTYPE>
inline bsl::function<PROTOTYPE>::function(nullptr_t) BSLS_KEYWORD_NOEXCEPT
    : Base(allocator_type())
{
}

template <class PROTOTYPE>
inline
bsl::function<PROTOTYPE>::function(allocator_arg_t       ,
                                   const allocator_type& allocator)
                                                          BSLS_KEYWORD_NOEXCEPT
    : Base(allocator)
{
}

template <class PROTOTYPE>
inline
bsl::function<PROTOTYPE>::function(allocator_arg_t       ,
                                   const allocator_type& allocator,
                                   nullptr_t) BSLS_KEYWORD_NOEXCEPT
    : Base(allocator)
{
}

template <class PROTOTYPE>
inline bsl::function<PROTOTYPE>::function(const function& original)
    : Base(allocator_type())
{
    this->d_rep.copyInit(original.d_rep);
}

template <class PROTOTYPE>
inline bsl::function<PROTOTYPE>::function(allocator_arg_t,
                                          const allocator_type& allocator,
                                          const function&       original)
    : Base(allocator)
{
    this->d_rep.copyInit(original.d_rep);
}

template <class PROTOTYPE>
inline
bsl::function<PROTOTYPE>::function(
       BloombergLP::bslmf::MovableRef<function> original) BSLS_KEYWORD_NOEXCEPT
    : Base(MovableRefUtil::access(original).get_allocator())
{
    this->d_rep.moveInit(&MovableRefUtil::access(original).d_rep);
}

template <class PROTOTYPE>
inline bsl::function<PROTOTYPE>::function(
                            allocator_arg_t,
                            const allocator_type&                    allocator,
                            BloombergLP::bslmf::MovableRef<function> original)
    : Base(allocator)
{
    this->d_rep.moveInit(&MovableRefUtil::access(original).d_rep);
}

// MANIPULATORS
template <class PROTOTYPE>
inline bsl::function<PROTOTYPE>&
bsl::function<PROTOTYPE>::operator=(const function& rhs)
{
    function temp(allocator_arg, this->get_allocator(), rhs);
    this->d_rep.makeEmpty();            // Won't throw
    this->d_rep.moveInit(&temp.d_rep);  // Won't throw
    return *this;
}

template <class PROTOTYPE>
inline bsl::function<PROTOTYPE>&
bsl::function<PROTOTYPE>::operator=(
                                  BloombergLP::bslmf::MovableRef<function> rhs)
{
    function temp(allocator_arg, this->get_allocator(),
                  MovableRefUtil::move(rhs));
    this->d_rep.makeEmpty();            // Won't throw
    this->d_rep.moveInit(&temp.d_rep);  // Won't throw
    return *this;
}

template <class PROTOTYPE>
inline bsl::function<PROTOTYPE>&
bsl::function<PROTOTYPE>::operator=(nullptr_t) BSLS_KEYWORD_NOEXCEPT
{
    this->d_rep.makeEmpty();
    return *this;
}

template <class PROTOTYPE>
inline
void bsl::function<PROTOTYPE>::swap(function& other) BSLS_KEYWORD_NOEXCEPT
{
    this->d_rep.swap(other.d_rep);  // Won't throw
}

template <class PROTOTYPE>
template<class TP>
inline
TP *bsl::function<PROTOTYPE>::target() BSLS_KEYWORD_NOEXCEPT
{
    return this->d_rep.template target<TP>();
}

// ACCESSORS

#ifdef BSLS_COMPILERFEATURES_SUPPORT_OPERATOR_EXPLICIT
template <class PROTOTYPE>
inline
bsl::function<PROTOTYPE>::operator bool() const BSLS_KEYWORD_NOEXCEPT
{
    // If there is an invoker, then this function is non-empty (return true);
    // otherwise it is empty (return false).
    return 0 != this->d_rep.invoker();
}
#endif // BSLS_COMPILERFEATURES_SUPPORT_OPERATOR_EXPLICIT

template <class PROTOTYPE>
inline
typename bsl::function<PROTOTYPE>::allocator_type
bsl::function<PROTOTYPE>::get_allocator() const BSLS_KEYWORD_NOEXCEPT
{
    return this->d_rep.get_allocator();
}

template <class PROTOTYPE>
template<class TP>
inline
const TP* bsl::function<PROTOTYPE>::target() const BSLS_KEYWORD_NOEXCEPT
{
#if defined(BSLS_PLATFORM_CMP_MSVC) && BSLS_PLATFORM_CMP_VERSION < 1900
    // MSVC 2013 has a problem with implicit conversion to 'const nullptr_t*'.
    return (const TP*) this->d_rep.target<TP>();
#else
    return this->d_rep.template target<TP>();
#endif
}

template <class PROTOTYPE>
const std::type_info&
bsl::function<PROTOTYPE>::target_type() const BSLS_KEYWORD_NOEXCEPT
{
    return this->d_rep.target_type();
}

#ifndef BDE_OMIT_INTERNAL_DEPRECATED
// CONVERSIONS TO LEGACY TYPE
template <class PROTOTYPE>
inline
bsl::function<PROTOTYPE>::operator BloombergLP::bdef_Function<PROTOTYPE *>&()
                                                          BSLS_KEYWORD_NOEXCEPT
{
    typedef BloombergLP::bdef_Function<PROTOTYPE *> Ret;
    return *static_cast<Ret*>(this);
}

template <class PROTOTYPE>
inline
bsl::function<PROTOTYPE>::
operator const BloombergLP::bdef_Function<PROTOTYPE *>&() const
                                                          BSLS_KEYWORD_NOEXCEPT
{
    typedef const BloombergLP::bdef_Function<PROTOTYPE *> Ret;
    return *static_cast<Ret*>(this);
}

template <class PROTOTYPE>
inline
BloombergLP::bslma::Allocator *
bsl::function<PROTOTYPE>::allocator() const BSLS_KEYWORD_NOEXCEPT
{
    return get_allocator().mechanism();
}

template <class PROTOTYPE>
inline
bool bsl::function<PROTOTYPE>::isInplace() const BSLS_KEYWORD_NOEXCEPT
{
    return this->d_rep.isInplace();
}
#endif // BDE_OMIT_INTERNAL_DEPRECATED

// FREE FUNCTIONS
template <class PROTOTYPE>
inline
bool bsl::operator==(const bsl::function<PROTOTYPE>& f,
                     bsl::nullptr_t) BSLS_KEYWORD_NOEXCEPT
{
    return !f;
}

template <class PROTOTYPE>
inline
bool bsl::operator==(bsl::nullptr_t,
                     const bsl::function<PROTOTYPE>& f) BSLS_KEYWORD_NOEXCEPT
{
    return !f;
}

template <class PROTOTYPE>
inline
bool bsl::operator!=(const bsl::function<PROTOTYPE>& f,
                     bsl::nullptr_t                   ) BSLS_KEYWORD_NOEXCEPT
{
    return !!f;
}

template <class PROTOTYPE>
inline
bool bsl::operator!=(bsl::nullptr_t,
                     const bsl::function<PROTOTYPE>& f) BSLS_KEYWORD_NOEXCEPT
{
    return !!f;
}

template <class PROTOTYPE>
inline
void bsl::swap(bsl::function<PROTOTYPE>& a,
               bsl::function<PROTOTYPE>& b) BSLS_KEYWORD_NOEXCEPT
{
    a.swap(b);
}

       // --------------------------------------------------------------
       // specialization of class template Function_InvokerUtil_Dispatch
       // --------------------------------------------------------------

namespace BloombergLP {
namespace bslstl {

template <class PROTO>
struct Function_InvokerUtilNullCheck<bsl::function<PROTO> > {
    // Specialization of null checker for instantiations of 'bsl::function'.
    // This specialization treats an empty 'bsl::function' as a null object.

    // CLASS METHODS
    static bool isNull(const bsl::function<PROTO>& f)
        // Return true if the 'bsl::function' specified by 'f' is empty; else
        // false.
    {
        return !f;
    }
};

}  //  close package namespace
}  //  close enterprise namespace

// Undo 'BSLS_ASSERT' filename fix -- See {'bsls_assertimputil'}
#ifdef BSLS_ASSERTIMPUTIL_AVOID_STRING_CONSTANTS
#undef BSLS_ASSERTIMPUTIL_FILE
#define BSLS_ASSERTIMPUTIL_FILE BSLS_ASSERTIMPUTIL_DEFAULTFILE
#endif

#endif // End C++11 code

#endif // End C++11 code

// ----------------------------------------------------------------------------
// Copyright 2020 Bloomberg Finance L.P.
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
