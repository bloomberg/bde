// bslstl_priorityqueue.h                                             -*-C++-*-
#ifndef INCLUDED_BSLSTL_PRIORITYQUEUE
#define INCLUDED_BSLSTL_PRIORITYQUEUE

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")


//@PURPOSE: Provide container adapter class template 'priority_queue'.
//
//@CLASSES:
//   bslstl::priority_queue: template of highest-priority-first data structure
//
//@SEE_ALSO: bslstl_queue, bslstl_stack
//
//@DESCRIPTION: This component defines a class template, 'bsl::priority_queue',
// holding a container (of a parameterized type 'CONTAINER' containing elements
// of another parameterized type 'VALUE'), and adapting it to provide
// highest-priority-first priority queue data structure.  The component takes a
// third parameterized type 'COMPARATOR' for customized priorities comparison
// between two elements.
//
// An instantiation of 'priority_queue' is an allocator-aware, value-semantic
// type whose salient attributes are its size (number of held elements) and the
// sorted sequence of values (of held elements).  If 'priority_queue' is
// instantiated with a parameterized type 'VALUE' that is not itself
// value-semantic, then it will not retain all of its value-semantic qualities.
// A 'priority_queue' cannot be tested for equality, but its parameterized type
// 'VALUE' must be able to be tested for comparing less by its parameterized
// type 'COMPARATOR'.
//
// 'priority_queue' meets the requirements of a container adapter in the C++
// standard [23.6].  The 'priority_queue' implemented here adheres to the C++11
// standard, except that it does not have methods that take rvalue references
// and 'initializer_lists'.  Note that excluded C++11 features are those that
// require C++11 compiler support.
//
///Memory Allocation
///-----------------
// The type supplied as 'ALLOCATOR' template parameter in some of
// 'priority_queue' constructors determines how the held container (of
// parameterized 'CONTAINER') will allocate memory.  A 'priority_queue'
// supports allocators meeting the requirements of the C++11 standard
// [17.6.3.5] as long as the held container does.  In addition it supports
// scoped-allocators derived from the 'bslma_Allocator' memory allocation
// protocol.  Clients intending to use 'bslma' style allocators should use
// 'bsl::allocator' as the 'ALLOCATOR' template parameter,  providing a C++11
// standard-compatible adapter for a 'bslma_Allocator' object.
//
///Operations
///----------
// The C++11 standard [23.6.4] declares any container type supporting
// operations 'front', 'push_back', and 'pop_back' can be used to instantiate
// the parameterized type 'CONTAINER'.  Below is a list of public methods of
// 'priority_queue' class that are effectively implementated as calling the
// corresponding operations in the held container (referenced as 'c').
//  +--------------------------------------+---------------------------+
//  | Public methods in 'priority_queue'   | Operation in 'CONTAINER'  |
//  +======================================+===========================+
//  | void push(const value_type& value);  | c.push_back(value);       |
//  | void pop();                          | c.pop_back();             |
//  +--------------------------------------+---------------------------+
//  | bool empty() const;                  | c.empty();                |
//  | size_type size() const;              | c.size();                 |
//  | const_reference top() const;         | c.front();                |
//  +--------------------------------------+---------------------------+
//
///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: Task Scheduler
///- - - - - - - - - - - - -
// In this example, we will use the 'bsl::priority_queue' class to implement a
// task scheduler that schedules a group of tasks based on their designated
// priorities.
//
// Suppose we want to write a background process that runs tasks needed by
// foreground applications.  Each task has a task id, a priority, and a
// function pointer that can be invoked by the background process.  This
// background process has two threads: one thread (receiving thread) receives
// requests from other applications, passing required tasks to a task
// scheduler; the other thread (processing thread) runs the task scheduler,
// executing the tasks one-by-one from higher to lower priorities.  To
// implement this functionality, we can use 'bsl::priority_queue' in the task
// scheduler to buffer received, but as yet unprocessed, tasks.  The task
// scheduler adds newly received tasks into the priority queue in the receiving
// thread, and extracts tasks from the the priority queue for execution
// according to their priorities in the processing thread.
//
// First, we define a 'TaskFunction' type:
//..
//  typedef void (*TaskFunction)(int, int, int);
//..
// Then, we define a 'Task' class, which contains a task id, a 'TaskFunction'
// object and an associated task priority:
//..
//  class Task
//      // This class represents a task that has an integer task id, a task
//      // function, and an integer priority.  The smaller the numerical value
//      // of a priority, the higher the priority.
//  {
//    private:
//      // DATA
//      int          d_taskId;          // task id
//
//      TaskFunction d_taskFunction_p;  // task function
//
//      int          d_priority;        // priority of the task
//
//    public:
//      // CREATORS
//      explicit Task(int taskId, TaskFunction taskFunction, int priority);
//          // Create a 'Task' object having the specified 'taskId', the
//          // specified 'd_taskFunction_p', and the specified 'priority'.
//
//      // ACCESSORS
//      int getId() const;
//          // Return the contained task id.
//
//      int getPriority() const;
//          // Return the priority of the task.
//
//      TaskFunction getFunction() const;
//          // Return the contained task function object.
//  };
//
//  // CREATORS
//  Task::Task(int taskId, TaskFunction taskFunction, int priority)
//  : d_taskId(taskId)
//  , d_taskFunction_p(taskFunction)
//  , d_priority(priority)
//  {
//  }
//
//  // ACCESSORS
//  inline
//  int Task::getId() const
//  {
//      return d_taskId;
//  }
//
//  inline
//  int Task::getPriority() const
//  {
//      return d_priority;
//  }
//
//  inline
//  TaskFunction Task::getFunction() const
//  {
//      return d_taskFunction_p;
//  }
//..
// Next, we define a functor to compare the priorities of two 'Task' objects:
//..
//  struct TaskComparator {
//      // This 'struct' defines an ordering on 'Task' objects, allowing them
//      // to be included in sorted data structures such as
//      // 'bsl::priority_queue'.
//
//      bool operator()(const Task& lhs, const Task& rhs) const
//          // Return 'true' if the priority of the specified 'lhs' is
//          // numerically less than that of the specified 'rhs', and 'false'
//          // otherwise.  Note that the smaller the value returned by the
//          // 'Task::getPriority' method, the higher the priority.
//      {
//          return lhs.getPriority() > rhs.getPriority();
//      }
//  };
//..
// Then, we define a 'TaskScheduler' class that provides methods to hold and
// schedule unprocessed tasks:
//..
//  class TaskScheduler {
//      // This class holds and schedules tasks to execute.
//..
// Here, we define a private data member that is an instantiation of
// 'bsl::priority_queue', which uses 'Task' for its 'VALUE' (template
// parameter) type, 'bsl::vector<Task>' for its 'CONTAINER' (template
// parameter) type, and 'TaskComparator' for its 'COMPARATOR' (template
// parameter) type:
//..
//      // DATA
//      bsl::priority_queue<Task,
//                          bsl::vector<Task>,
//                          TaskComparator>
//            d_taskPriorityQueue;  // priority queue holding unprocessed tasks
//
//      // ...
//
//    public:
//      // CREATORS
//      explicit TaskScheduler(bslma::Allocator *basicAllocator = 0);
//          // Create a 'TaskScheduler' object.  Optionally specify a
//          // 'basicAllocator' used to supply memory.  If 'basicAllocator' is
//          // 0, the currently installed default allocator is used.
//
//      // MANIPULATORS
//      void addTask(int taskId, TaskFunction taskFunction, int priority);
//          // Enqueue the specified 'task' having the specified 'priority'
//          // onto this scheduler.
//
//      void processTasks(int verbose);
//          // Dequeue the task having the highest priority in this scheduler,
//          // and call its task function by passing in the specified 'verbose'
//          // flag.
//  };
//..
// Next, we implement the 'TaskScheduler' constructor:
//..
//  TaskScheduler::TaskScheduler(bslma::Allocator *basicAllocator)
//  : d_taskPriorityQueue(basicAllocator)
//  {
//  }
//..
// Notice that we pass to the contained 'd_taskPriorityQueue' object the
// 'bslma::Allocator' supplied to the 'TaskScheduler' at construction.
//
// Then, we implement the 'addTask' method, which constructs a 'Task' object
// and adds it into the priority queue:
//..
//  void TaskScheduler::addTask(int taskId,
//                              TaskFunction taskFunction,
//                              int priority)
//  {
//      // ... (some synchronization)
//
//      d_taskPriorityQueue.push(Task(taskId, taskFunction, priority));
//
//      // ...
//  }
//..
// Next, we implement the 'processTasks' method, which extracts tasks from the
// priority queue in order of descending priorities, and executes them:
//..
//  void TaskScheduler::processTasks(int verbose)
//  {
//      // ... (some synchronization)
//
//      while (!d_taskPriorityQueue.empty()) {
//          const Task& task = d_taskPriorityQueue.top();
//          TaskFunction taskFunction = task.getFunction();
//          if (taskFunction) {
//              taskFunction(task.getId(), task.getPriority(), verbose);
//          }
//          d_taskPriorityQueue.pop();
//      }
//
//      // ...
//  }
//..
// Note that the 'top' method always returns the 'Task' object having the
// highest priority in the priority queue.
//
// Then, we define two task functions:
//..
//  void taskFunction1(int taskId, int priority, int verbose)
//  {
//      if (verbose) {
//          printf("Executing task %d (priority = %d) in 'taskFunction1'.\n",
//                 taskId,
//                 priority);
//      }
//  }
//
//  void taskFunction2(int taskId, int priority, int verbose)
//  {
//      if (verbose) {
//          printf("Executing task %d (priority = %d) in 'taskFunction2'.\n",
//                 taskId,
//                 priority);
//      }
//  }
//..
// Next, we create a global 'TaskScheduler' object:
//..
//  TaskScheduler taskScheduler;
//..
// Now, we call the 'addTask' method of 'taskScheduler' in the receiving
// thread:
//..
//  // (in receiving thread)
//  // ...
//
//  taskScheduler.addTask(1, taskFunction1, 50);
//
//  // ...
//
//  taskScheduler.addTask(2, taskFunction1, 99);
//
//  // ...
//
//  taskScheduler.addTask(3, taskFunction2, 4);
//
//  // ...
//..
// Finally, we call the 'processTasks' method of 'taskScheduler' in the
// processing thread:
//..
//  // (in processing thread)
//  // ...
//
//  taskScheduler.processTasks(veryVerbose);
//
//  // ...
//..

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLSTL_ALLOCATOR
#include <bslstl_allocator.h>
#endif

#ifndef INCLUDED_BSLSTL_VECTOR
#include <bslstl_vector.h>
#endif

#ifndef INCLUDED_BSLALG_SWAPUTIL
#include <bslalg_swaputil.h>
#endif

#ifndef INCLUDED_BSLMF_ENABLEIF
#include <bslmf_enableif.h>
#endif

#ifndef INCLUDED_FUNCTIONAL
#include <functional>
#define INCLUDED_FUNCTIONAL
#endif

namespace BloombergLP {

namespace bslstl {

template <class CONTAINER, class ALLOCATOR>
struct PriorityQueue_HasAllocatorType {
  private:
    typedef char YesType;
    struct NoType { char a[2]; };

  public:
    template <class TYPE>
    static YesType match(const typename TYPE::allocator_type *);
    template <class TYPE>
    static NoType match(...);

    enum { VALUE = (sizeof(YesType) == sizeof(match<CONTAINER>(0))) };
};

}  // close package namespace

}  // close enterprise namespace

namespace bsl {

                         // ====================
                         // class priority_queue
                         // ====================

template <class VALUE,
          class CONTAINER  = vector<VALUE>,
          class COMPARATOR = native_std::less<typename CONTAINER::value_type> >
class priority_queue
    // This class is a value-semantic class template, adapting a container of
    // the parameterized 'CONTAINER' type that holds elements of the
    // parameterized 'VALUE' type, to provides a highest-priority-first
    // priority queue data structure, where the priorities of elements are
    // compared by a comparator of the parameterized 'COMPARATOR' type.  The
    // container object held by a 'priority_queue' class object is referenced
    // as 'c' in the following documentation.
{
  protected:
    // DATA
    CONTAINER c;        // container for elements in the 'priority_queue',
                        // protected as required by the C++11 Standard
    COMPARATOR comp;    // comparator that defines the priority order of
                        // elements in the 'priority_queue'

  public:
    // PUBLIC TYPES
    typedef typename CONTAINER::value_type      value_type;
    typedef typename CONTAINER::reference       reference;
    typedef typename CONTAINER::const_reference const_reference;
    typedef typename CONTAINER::size_type       size_type;
    typedef          CONTAINER                  container_type;

    // CREATORS
    priority_queue(const COMPARATOR& comparator,
                   const CONTAINER&  container);
        // Create a priority queue, adapting the specified 'container' of the
        // (template parameter) type 'CONTAINER', and having the specified
        // 'comparator' of the (template parameter) type 'COMPARATOR' to order
        // priorities of elements held in 'container'.

    explicit priority_queue();
        // Create an empty priority queue, adapting a default-constructed
        // container of the (template parameter) type 'CONTAINER'.  Use a
        // default-constructed comparator of the (template parameter) type
        // 'COMPARATOR' to order priorities of elements.

    explicit priority_queue(const COMPARATOR& comparator);
        // Create an empty priority queue, adapting a default-constructed
        // container of the parameterized 'CONTAINER' type, and having the
        // specified 'comparator' of the (template parameter) type 'COMPARATOR'
        // to order priorities of elements in 'container'.

    template <class INPUT_ITERATOR>
    priority_queue(INPUT_ITERATOR first,
                   INPUT_ITERATOR last);
        // Create a priority queue, adapting a default-constructed container of
        // the (template parameter) type 'CONTAINER', and inserting into the
        // container a sequence of 'value_type' elements that starts at the
        // specified 'first' and ends immediately before the specified 'last'.
        // Use a default-constructed comparator of the parameterized
        // 'COMPARATOR' type to order the priorities of elements.

    template <class INPUT_ITERATOR>
    priority_queue(INPUT_ITERATOR    first,
                   INPUT_ITERATOR    last,
                   const COMPARATOR& comparator,
                   const CONTAINER&  container);
        // Create a priority queue, adapting the specified 'container', having
        // the specified 'comparator' to order the priorities of elements,
        // including those originally existed in 'container', and those
        // inserted into the 'container' from a sequence of 'value_type'
        // elements starting at the specified 'first', and ending immediately
        // before the specified 'last'.

    priority_queue(const priority_queue& original);
        // Create a priority queue having the same value as the specified
        // 'original'.  Use the comparator from 'original' to order the
        // priorities of elements.

    template <class ALLOCATOR>
    explicit priority_queue(
                           const ALLOCATOR& basicAllocator,
                           typename enable_if<
                           BloombergLP::bslstl::PriorityQueue_HasAllocatorType<
                                               CONTAINER,
                                               ALLOCATOR>::VALUE>::type * = 0);
        // Create an empty priority queue that adapts a default-constructed
        // container of the parameterized 'CONTAINER' type and will use the
        // specified 'basicAllocator' to supply memory.  Use a
        // default-constructed comparator of the parameterized 'COMPARATOR'
        // type to order the priorities of elements.  Note that the 'ALLOCATOR'
        // parameter type has to be convertible to the allocator of the
        // 'CONTAINER' parameter type, 'CONTAINER::allocator_type'.  Otherwise
        // this constructor is disabled.

    template <class ALLOCATOR>
    priority_queue(const COMPARATOR& comparator,
                   const ALLOCATOR& basicAllocator,
                   typename enable_if<
                           BloombergLP::bslstl::PriorityQueue_HasAllocatorType<
                                               CONTAINER,
                                               ALLOCATOR>::VALUE>::type * = 0);
        // Create an empty priority queue that adapts a default-constructed
        // container of the parameterized 'CONTAINER'type and will use the
        // specified 'basicAllocator' to supply memory, and the specified
        // 'comparator' to order priorities of elements in a
        // default-constructed container of the parameterized 'CONTAINER' type.
        // Note that the 'ALLOCATOR' parameter type has to be convertible to
        // the allocator of the 'CONTAINER' parameter type,
        // 'CONTAINER::allocator_type'.  Otherwise this constructor is
        // disabled.

    template <class ALLOCATOR>
    priority_queue(const COMPARATOR& comparator,
                   const CONTAINER&  container,
                   const ALLOCATOR&  basicAllocator,
                   typename enable_if<
                           BloombergLP::bslstl::PriorityQueue_HasAllocatorType<
                                               CONTAINER,
                                               ALLOCATOR>::VALUE>::type * = 0);
        // Create a priority queue that will use the specified 'basicAllocator'
        // to supply memory, and the specified 'comparator' to order priorities
        // of elements in the specified 'container'.  Note that the 'ALLOCATOR'
        // parameter type has to be convertible to the allocator of the
        // 'CONTAINER' parameter type, 'CONTAINER::allocator_type'.  Otherwise
        // this constructor is disabled.

    template <class ALLOCATOR>
    priority_queue(const priority_queue& original,
                   const ALLOCATOR& basicAllocator,
                   typename enable_if<
                           BloombergLP::bslstl::PriorityQueue_HasAllocatorType<
                                               CONTAINER,
                                               ALLOCATOR>::VALUE>::type * = 0);
        // Create a priority queue having the same value as the specified
        // 'original' that will use the specified 'basicAllocator' to supply
        // memory.  Use the comparator from 'original' to order the priorities
        // of elements.  Note that the 'ALLOCATOR' parameter type has to be
        // convertible to the allocator of the 'CONTAINER' parameter type,
        // 'CONTAINER::allocator_type'.  Otherwise this constructor is
        // disabled.

    // MANIPULATORS
    void push(const value_type& value);
        // Insert a new element having the specified priority 'value' into this
        // 'priority_queue' object.  In effect, performs 'c.push_back(value);'.

    void pop();
        // Remove the top element from this 'priority_queue' object that has
        // the highest priority.  In effect, performs 'c.pop();'.  The behavior
        // is undefined if there is currently no elements in this object.

    void swap(priority_queue& other);
        // Efficiently exchange the value of this object with the value of the
        // specified 'other' object.  In effect, performs
        // 'using bsl::swap; swap(c, other.c);'.

    // ACCESSORS
    bool empty() const;
        // Return 'true' if this 'priority_queue' object contains no elements,
        // and 'false' otherwise.  In effect, performs 'return c.empty();'.

    size_type size() const;
        // Return the number of elements in this 'priority_queue' object.  In
        // effect, performs 'return c.size()'.

    const_reference top() const;
        // Return the immutable element having the highest priority in this
        // 'priority_queue' object.  In effect, performs 'return c.front()'.
        // The behavior is undefined if there is currently no elements in this
        // object.
};

// FREE FUNCTIONS

template <class VALUE, class CONTAINER, class COMPARATOR>
void swap(priority_queue<VALUE, CONTAINER, COMPARATOR>& lhs,
          priority_queue<VALUE, CONTAINER, COMPARATOR>& rhs);

// ============================================================================
//                  TEMPLATE AND INLINE FUNCTION DEFINITIONS
// ============================================================================

                         // --------------------
                         // class priority_queue
                         // --------------------

// CREATORS
template <class VALUE, class CONTAINER, class COMPARATOR>
inline
priority_queue<VALUE, CONTAINER, COMPARATOR>::priority_queue(
                                                  const COMPARATOR& comparator,
                                                  const CONTAINER&  container)
: c(container)
, comp(comparator)
{
    native_std::make_heap(c.begin(), c.end(), comp);
}

template <class VALUE, class CONTAINER, class COMPARATOR>
inline
priority_queue<VALUE, CONTAINER, COMPARATOR>::priority_queue()
{
}

template <class VALUE, class CONTAINER, class COMPARATOR>
inline
priority_queue<VALUE, CONTAINER, COMPARATOR>::priority_queue(
                                                  const COMPARATOR& comparator)
: comp(comparator)
{
}

template <class VALUE, class CONTAINER, class COMPARATOR>
template <class INPUT_ITERATOR>
inline
priority_queue<VALUE, CONTAINER, COMPARATOR>::priority_queue(
                                                       INPUT_ITERATOR    first,
                                                       INPUT_ITERATOR    last)
{
    c.insert(c.end(), first, last);
    native_std::make_heap(c.begin(), c.end(), comp);
}

template <class VALUE, class CONTAINER, class COMPARATOR>
template <class INPUT_ITERATOR>
inline
priority_queue<VALUE, CONTAINER, COMPARATOR>::priority_queue(
                                                  INPUT_ITERATOR    first,
                                                  INPUT_ITERATOR    last,
                                                  const COMPARATOR& comparator,
                                                  const CONTAINER&  container)
: c(container)
, comp(comparator)
{
    c.insert(c.end(), first, last);
    native_std::make_heap(c.begin(), c.end(), comp);
}

template <class VALUE, class CONTAINER, class COMPARATOR>
inline
priority_queue<VALUE, CONTAINER, COMPARATOR>::priority_queue(
                                                const priority_queue& original)
: c(original.c)
, comp(original.comp)
{
}

template <class VALUE, class CONTAINER, class COMPARATOR>
template <class ALLOCATOR>
inline
priority_queue<VALUE, CONTAINER, COMPARATOR>::priority_queue(
                           const ALLOCATOR& basicAllocator,
                           typename enable_if<
                           BloombergLP::bslstl::PriorityQueue_HasAllocatorType<
                                                    CONTAINER,
                                                    ALLOCATOR>::VALUE>::type *)
: c(basicAllocator)
, comp(COMPARATOR())
{
}

template <class VALUE, class CONTAINER, class COMPARATOR>
template <class ALLOCATOR>
inline
priority_queue<VALUE, CONTAINER, COMPARATOR>::priority_queue(
                           const COMPARATOR& comparator,
                           const ALLOCATOR&  basicAllocator,
                           typename enable_if<
                           BloombergLP::bslstl::PriorityQueue_HasAllocatorType<
                                                    CONTAINER,
                                                    ALLOCATOR>::VALUE>::type *)
: c(basicAllocator)
, comp(comparator)
{
}

template <class VALUE, class CONTAINER, class COMPARATOR>
template <class ALLOCATOR>
inline
priority_queue<VALUE, CONTAINER, COMPARATOR>::priority_queue(
                           const COMPARATOR& comparator,
                           const CONTAINER&  container,
                           const ALLOCATOR&  basicAllocator,
                           typename enable_if<
                           BloombergLP::bslstl::PriorityQueue_HasAllocatorType<
                                                    CONTAINER,
                                                    ALLOCATOR>::VALUE>::type *)
: c(container, basicAllocator)
, comp(comparator)
{
    native_std::make_heap(c.begin(), c.end(), comp);
}


template <class VALUE, class CONTAINER, class COMPARATOR>
template <class ALLOCATOR>
inline
priority_queue<VALUE, CONTAINER, COMPARATOR>::priority_queue(
                           const priority_queue& original,
                           const ALLOCATOR&      basicAllocator,
                           typename enable_if<
                           BloombergLP::bslstl::PriorityQueue_HasAllocatorType<
                                                    CONTAINER,
                                                    ALLOCATOR>::VALUE>::type *)
: c(original.c, basicAllocator)
, comp(original.comp)
{
}


// MANIPULATORS
template <class VALUE, class CONTAINER, class COMPARATOR>
inline
void priority_queue<VALUE, CONTAINER, COMPARATOR>::push(
                                                       const value_type& value)
{
    c.push_back(value);
    native_std::push_heap(c.begin(), c.end(), comp);
}

template <class VALUE, class CONTAINER, class COMPARATOR>
inline
void priority_queue<VALUE, CONTAINER, COMPARATOR>::pop()
{
    native_std::pop_heap(c.begin(), c.end(), comp);
    c.pop_back();
}

template <class VALUE, class CONTAINER, class COMPARATOR>
inline
void priority_queue<VALUE, CONTAINER, COMPARATOR>::swap(priority_queue& other)
{
    BloombergLP::bslalg::SwapUtil::swap(&c   , &other.c   );
    BloombergLP::bslalg::SwapUtil::swap(&comp, &other.comp);
}

// ACCESSORS
template <class VALUE, class CONTAINER, class COMPARATOR>
inline
bool priority_queue<VALUE, CONTAINER, COMPARATOR>::empty() const
{
    return c.empty();
}

template <class VALUE, class CONTAINER, class COMPARATOR>
inline
typename priority_queue<VALUE, CONTAINER, COMPARATOR>::size_type
priority_queue<VALUE, CONTAINER, COMPARATOR>::size() const
{
    return c.size();
}

template <class VALUE, class CONTAINER, class COMPARATOR>
inline
typename priority_queue<VALUE, CONTAINER, COMPARATOR>::const_reference
priority_queue<VALUE, CONTAINER, COMPARATOR>::top() const
{
    return c.front();
}

// FREE FUNCTIONS
template <class VALUE, class CONTAINER, class COMPARATOR>
void swap(priority_queue<VALUE, CONTAINER, COMPARATOR>& lhs,
          priority_queue<VALUE, CONTAINER, COMPARATOR>& rhs)
{
    lhs.swap(rhs);
}

}  // close namespace bsl

#endif

// ----------------------------------------------------------------------------
// Copyright 2013 Bloomberg Finance L.P.
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
