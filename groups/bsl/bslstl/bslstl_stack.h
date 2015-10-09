// bslstl_stack.h                                                     -*-C++-*-
#ifndef INCLUDED_BSLSTL_STACK
#define INCLUDED_BSLSTL_STACK

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide an STL-compliant stack class.
//
//@CLASSES:
//   bslstl::stack: STL-compliant stack template
//
//@SEE_ALSO: bslstl_deque, bslstl_vector, bslstl_list, bslstl_queue,
//           bslstl_priorityqueue
//
//@DESCRIPTION: This component defines a single class template 'bslstl::stack',
// a container adapter that takes an underlying container and provides a stack
// interface which the user accesses primarily through 'push', 'pop', and 'top'
// operations.  A 'deque' (the default), 'vector', or 'list' may be used, but
// any container which supports 'push_back', 'pop_back', 'back', and 'size',
// plus a template specialization 'uses_allocator::type', may be used.
//
///Requirements of Parametrized 'CONTAINER' Type
///---------------------------------------------
// This class will accept 'bsl::queue', 'bsl::vector', or 'bsl::list' as the
// template parameter 'CONTAINER'.  In addition, other container classes could
// be supplied for the CONTAINER argument, but the supplied 'CONTAINER'
// template parameter must support the following public types:
//: o value_type
//: o reference
//: o const_reference
//: o size_type
// In addition, the supplied 'CONTAINER' template parameter must support the
// following methods, (depending on the methods of 'stack' being used):
//: o constructors used must take a parameter of type 'allocator_type'
//: o void push_back(const value_type&)
//: o void pop_back()
//: o value_type& back()
//: o size_type size()
//: o '==', '!=', '<', '>', '<=', '>='
//: o 'operator='
//: o std::swap(CONTAINER&, CONTAINER&) must work
//
///Note on Parameterized 'VALUE' Type
///----------------------------------
// If the 'CONTAINER' type is specified, the 'VALUE' type specified is ignored
// and 'CONTAINER::value_type' is used in its place.  It is recommended, if
// 'CONTAINER' is specified, that a type equivalent to 'CONTAINER::value_type'
// be specified to the 'VALUE' template parameter so as not to mislead readers.
//
///Memory Allocation
///-----------------
// No memory allocator template arg is directly supplied to this class, the
// allocator type used is the allocator specified for the container class.
// Some functions of this template only exist if type
// 'CONTAINER::allocator_type' exists, and if it does exist it is assumed to be
// the allocator type used by 'CONTAINER', and that 'CONTAINER' supports
// constructors of this type.
//
///'bslma'-Style Allocators
///------------------------
// The constructors of this class take, as optional parameters, allocators of
// the object's parameterized 'CONTAINER::allocator_type' type, and allocators
// of this type are propagated to all constructors of the underlying container.
// In the case of container types 'bsl::deque' (the default type),
// 'bsl::vector', and 'bsl::list', 'CONTAINER::allocator_type' is
// 'bsl::allocator' which is implicitly convertible from 'bslma_Allocator *',
// and which can be converted to a 'bslma_Allocator *' through the 'mechanism'
// accessor.
//
// Hence if the underlying container takes 'bsl::allocator', then the 'stack'
// object can take 'bslma_Allocator *'s to supply memory allocation.  If no
// allocator is specified, 'allocator()' is used, which winds up using
// 'bslma::Default::allocator(0)'.
//
///Operations
///----------
// This section describes the run-time complexity of operations on instances
// of 'stack':
//..
//  Legend
//  ------
//  'C'             - parameterized container-type of the stack
//  'V'             - 'C::value_type'
//  'c'             - container of type 'C'
//  'nc'            - number of elements in container 'c'
//  's', 't'        - two distinct objects of type 'stack<V, C>'
//  'n', 'm'        - number of elements in 's' and 't' respectively
//  'al'            - a STL-style memory allocator
//  'v'             - an object of type 'V'
//
//  +----------------------------------------------------+--------------------+
//  | Note: the following estimations of operation complexity assume the      |
//  | underlying container is a 'bsl::deque', 'bsl::vector', or 'bsl::list'.  |
//  +----------------------------------------------------+--------------------+
//  | Operation                                          | Complexity         |
//  +====================================================+====================+
//  | stack<V, C> s;    (default construction)           | O(1)               |
//  | stack<V, C> s(al);                                 |                    |
//  +----------------------------------------------------+--------------------+
//  | stack<V, C> s(c);                                  | O(nc)              |
//  | stack<V, C> s(c, al);                              |                    |
//  +----------------------------------------------------+--------------------+
//  | stack<V, C> s(t);                                  | O(n)               |
//  | stack<V, C> s(t, al);                              |                    |
//  +----------------------------------------------------+--------------------+
//  | s.~stack(V, C>(); (destruction)                    | O(n)               |
//  +----------------------------------------------------+--------------------+
//  | s = t;          (assignment)                       | O(n)               |
//  +----------------------------------------------------+--------------------+
//  | s.push(v)                                          | O(1)               |
//  +----------------------------------------------------+--------------------+
//  | s.pop()                                            | O(1)               |
//  +----------------------------------------------------+--------------------+
//  | s.top()                                            | O(1)               |
//  +----------------------------------------------------+--------------------+
//  | s == t, s != t                                     | O(n)               |
//  +---------------------------------------------------+--------------------+
//  | s < t, s <= t, s > t, s >= t                       | O(n)               |
//  +----------------------------------------------------+--------------------+
//  | s.swap(t), swap(s,t)                               | depends on the     |
//  |                                                    | container; for     |
//  |                                                    | deque, vector, and |
//  |                                                    | list:              |
//  |                                                    | O(1) if 's' and    |
//  |                                                    | 't' use the same   |
//  |                                                    | allocator,         |
//  |                                                    | O(n + m) otherwise |
//  +----------------------------------------------------+--------------------+
//  | s.size()                                           | O(1) if 'C' is     |
//  |                                                    | deque or vector    |
//  +----------------------------------------------------+--------------------+
//  | s.empty()                                          | O(1)               |
//  +----------------------------------------------------+--------------------+
//..
//
///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: Household Chores To Do List
/// - - - - - - - - - - - - - - - - - - -
// Suppose a husband wants to keep track of chores his wife has asked him to
// do.  Over the years of being married, he has noticed that his wife generally
// wants the most recently requested task done first.  If she has a new task in
// mind that is low-priority, she will avoid asking for it until higher
// priority tasks are finished.  When he has finished all tasks, he is to
// report to his wife that he is ready for more.
//
// First, we define the class implementing the 'to-do' list.
//..
//  class ToDoList {
//      // DATA
//      bsl::stack<const char *> d_stack;
//
//    public:
//      // MANIPULATORS
//      void enqueueTask(const char *task);
//          // Add the specified 'task', a string describing a task, to the
//          // list.  Note the lifetime of the string referred to by 'task'
//          // must exceed the lifetime of the task in this list.
//
//      bool finishTask();
//          // Remove the current task from the list.  Return 'true' if a task
//          // was removed and it was the last task on the list, and return
//          // 'false' otherwise.
//
//      // ACCESSORS
//      const char *currentTask() const;
//          // Return the string representing the current task.  If there
//          // is no current task, return the string "<EMPTY>", which is
//          // not a valid task.
//  };
//
//  // MANIPULATORS
//  void ToDoList::enqueueTask(const char *task)
//  {
//      d_stack.push(task);
//  }
//
//  bool ToDoList::finishTask()
//  {
//      if (!d_stack.empty()) {
//          d_stack.pop();
//
//          return d_stack.empty();
//      }
//
//      return false;
//  };
//
//  // ACCESSORS
//  const char *ToDoList::currentTask() const
//  {
//      if (d_stack.empty()) {
//          return "<EMPTY>";
//      }
//
//      return d_stack.top();
//  }
//..
// Then, create an object of type 'ToDoList'.
//
//  ToDoList toDoList;
//
// Next, a few tasks are requested:
//..
//  toDoList.enqueueTask("Change the car's oil.");
//  toDoList.enqueueTask("Pay the bills.");
//..
// Then, the husband watches the Yankee's game on TV.  Upon returning to the
// list he consults the list to see what task is up next:
//..
//  assert(!strcmp("Pay the bills.", toDoList.currentTask()));
//..
// Next, he sees that he has to pay the bills.  When the bills are finished, he
// flushes that task from the list:
//..
//  assert(false == toDoList.finishTask());
//..
// Then, he consults the list for the next task.
//..
//  assert(!strcmp("Change the car's oil.", toDoList.currentTask()));
//..
// Next, he sees he has to change the car's oil.  Before he can get started,
// another request comes:
//..
//  toDoList.enqueueTask("Get some hot dogs.");
//  assert(!strcmp("Get some hot dogs.", toDoList.currentTask()));
//..
// Then, he drives the car to the convenience store and picks up some hot dogs
// and buns.  Upon returning home, he gives the hot dogs to his wife, updates
// the list, and consults it for the next task.
//..
//  assert(false == toDoList.finishTask());
//  assert(!strcmp("Change the car's oil.", toDoList.currentTask()));
//..
// Next, he finishes the oil change, updates the list, and consults it for the
// next task.
//..
//  assert(true == toDoList.finishTask());
//  assert(!strcmp("<EMPTY>", toDoList.currentTask()));
//..
// Finally, the wife has now been informed that everything is done, and she
// makes another request:
//..
//  toDoList.enqueueTask("Clean the rain gutters.");
//..

// Prevent 'bslstl' headers from being included directly in 'BSL_OVERRIDES_STD'
// mode.  Doing so is unsupported, and is likely to cause compilation errors.
#if defined(BSL_OVERRIDES_STD) && !defined(BSL_STDHDRS_PROLOGUE_IN_EFFECT)
#error "include <bsl_stack.h> instead of <bslstl_stack.h> in \
BSL_OVERRIDES_STD mode"
#endif

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLSTL_ALLOCATOR
#include <bslstl_allocator.h>
#endif

#ifndef INCLUDED_BSLSTL_DEQUE
#include <bslstl_deque.h>
#endif

#ifndef INCLUDED_BSLSTL_STDEXCEPTUTIL
#include <bslstl_stdexceptutil.h>
#endif

#ifndef INCLUDED_BSLALG_SWAPUTIL
#include <bslalg_swaputil.h>
#endif

#ifndef INCLUDED_BSLMA_USESBSLMAALLOCATOR
#include <bslma_usesbslmaallocator.h>
#endif

#ifndef INCLUDED_BSLMF_ENABLEIF
#include <bslmf_enableif.h>
#endif

#ifndef INCLUDED_BSLMF_MOVABLEREF
#include <bslmf_movableref.h>
#endif

#ifndef INCLUDED_BSLS_COMPILERFEATURES
#include <bsls_compilerfeatures.h>
#endif

#ifndef INCLUDED_BSLS_UTIL
#include <bsls_util.h>
#endif

#ifndef INCLUDED_ALGORITHM
#include <algorithm>
#define INCLUDED_ALGORITHM
#endif

namespace bsl {

template <class CONTAINER>
class Stack_HasAllocatorType {
    // This 'class' computes a public constant 'VALUE', which is 'true' if the
    // passed 'CONTAINER' defines a type 'CONTAINER::allocator_type' and
    // 'false' otherwise.  This is used in conjunction with 'enable_if' to make
    // methods of 'stack' that take allocator arguments exist if
    // 'CONTAINER::allocator_type' is present, and remove them from the
    // constructor overload set otherwise.

    // TYPES
    typedef char YesType;
    struct NoType { char a[2]; };

    // CLASS METHODS
    template <class TYPE>
    static YesType match(const typename TYPE::allocator_type *);
    template <class TYPE>
    static NoType match(...);
        // The return type of 'match' is 'YesType' if 'TYPE' has an allocator
        // type, and 'NoType' otherwise.

  public:
    enum { VALUE = (sizeof(YesType) == sizeof(match<CONTAINER>(0))) };
};

template <class VALUE, class CONTAINER = deque<VALUE> >
class stack {
    // This 'class' defines a container adapter which supports access primarily
    // via 'push', 'pop', and 'top'.  This type is value-semantic, and can be
    // based on a variety of other container types, including 'deque',
    // 'vector', and 'list'.
    //
    // Note that we never use 'VALUE' in the implementation except in the
    // default argument of 'CONTAINER'.  We use 'CONTAINER::value_type' for
    // everything, which means that if 'CONTAINER' is specified, then 'VALUE'
    // is ignored.

  public:
    // PUBLIC TYPES

    typedef typename CONTAINER::value_type      value_type;
    typedef typename CONTAINER::reference       reference;
    typedef typename CONTAINER::const_reference const_reference;
    typedef typename CONTAINER::size_type       size_type;
    typedef          CONTAINER                  container_type;

  private:
    // PRIVATE DATA
    container_type  d_container;    // container in which objects are stored

  protected:
    // PROTECTED DATA
    container_type& c;    // We are required by the standard to have the
                          // container be a protected variable named 'c'.  Just
                          // a reference to 'd_container'.

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION_IF(
        stack,
        BloombergLP::bslma::UsesBslmaAllocator,
        BloombergLP::bslma::UsesBslmaAllocator<container_type>::value);

  private:
    // FRIENDS
    template <class VAL, class CONT>
    friend bool operator==(const stack<VAL, CONT>&, const stack<VAL, CONT>&);
    template <class VAL, class CONT>
    friend bool operator!=(const stack<VAL, CONT>&, const stack<VAL, CONT>&);
    template <class VAL, class CONT>
    friend bool operator< (const stack<VAL, CONT>&, const stack<VAL, CONT>&);
    template <class VAL, class CONT>
    friend bool operator> (const stack<VAL, CONT>&, const stack<VAL, CONT>&);
    template <class VAL, class CONT>
    friend bool operator<=(const stack<VAL, CONT>&, const stack<VAL, CONT>&);
    template <class VAL, class CONT>
    friend bool operator>=(const stack<VAL, CONT>&, const stack<VAL, CONT>&);

  public:
    // CREATORS
    stack();
        // Construct an empty stack.  No allocator will be provided to the
        // underlying container, and the container's memory allocation will be
        // provided by whatever is the default for the container type.

    template <class ALLOCATOR>
    explicit
    stack(const ALLOCATOR& basicAllocator,
          typename enable_if<Stack_HasAllocatorType<CONTAINER>::VALUE,
                             ALLOCATOR>::type * = 0);
        // Construct an empty stack, and use the specified 'basicAllocator' to
        // supply memory.  If 'CONTAINER::allocator_type' does not exist, this
        // constructor may not be used.

    explicit
    stack(const CONTAINER& container);
        // Construct a stack whose underlying container has the value of the
        // specified 'container'.

    template <class ALLOCATOR>
    stack(const CONTAINER& container,
          const ALLOCATOR& basicAllocator,
          typename enable_if<Stack_HasAllocatorType<CONTAINER>::VALUE,
                             ALLOCATOR>::type * = 0);
        // Construct a stack whose underlying container has the value of the
        // specified 'container', and use the specified 'basicAllocator' to
        // supply memory.  If 'CONTAINER::allocator_type' does not exist, this
        // constructor may not be used.

    stack(const stack& original);
        // Construct a stack having the same value as the specified 'original'.

    template <class ALLOCATOR>
    stack(const stack&     original,
          const ALLOCATOR& basicAllocator,
          typename enable_if<Stack_HasAllocatorType<CONTAINER>::VALUE,
                             ALLOCATOR>::type * = 0);
        // Construct a stack having the same value as the specified stack
        // 'original', and use the specified 'basicAllocator' to supply memory.
        // If 'CONTAINER::allocator_type' does not exist, this constructor may
        // not be used.

#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
    explicit
    stack(CONTAINER&&                               container);

    template <class ALLOCATOR>
    stack(CONTAINER&&      container,
          const ALLOCATOR& basicAllocator,
          typename enable_if<Stack_HasAllocatorType<CONTAINER>::VALUE,
                             ALLOCATOR>::type * = 0);
        // TBD

    explicit
    stack(stack&&          original);
        // TBD

    template <class ALLOCATOR>
    stack(stack&&          original,
          const ALLOCATOR& basicAllocator,
          typename enable_if<Stack_HasAllocatorType<CONTAINER>::VALUE,
                             ALLOCATOR>::type * = 0);
        // TBD
#endif

    // MANIPULATORS
    stack& operator=(const stack& rhs);
        // Assign to this object the value of the specified 'rhs' object, and
        // return a reference providing modifiable access to this object.

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
    template <class... Args>
    void emplace(Args&&... args);
        // TBD
#elif BSLS_COMPILERFEATURES_SIMULATE_VARIADIC_TEMPLATES
// {{{ BEGIN GENERATED CODE
// The following section is automatically generated.  **DO NOT EDIT**
// Generator command line: sim_cpp11_features.pl bslstl_stack.h
    void emplace();

    template <class Args_01>
    void emplace(BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) args_01);

    template <class Args_01,
              class Args_02>
    void emplace(BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) args_01,
                 BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) args_02);

    template <class Args_01,
              class Args_02,
              class Args_03>
    void emplace(BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) args_01,
                 BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) args_02,
                 BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) args_03);

    template <class Args_01,
              class Args_02,
              class Args_03,
              class Args_04>
    void emplace(BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) args_01,
                 BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) args_02,
                 BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) args_03,
                 BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) args_04);

    template <class Args_01,
              class Args_02,
              class Args_03,
              class Args_04,
              class Args_05>
    void emplace(BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) args_01,
                 BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) args_02,
                 BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) args_03,
                 BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) args_04,
                 BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) args_05);

    template <class Args_01,
              class Args_02,
              class Args_03,
              class Args_04,
              class Args_05,
              class Args_06>
    void emplace(BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) args_01,
                 BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) args_02,
                 BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) args_03,
                 BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) args_04,
                 BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) args_05,
                 BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) args_06);

    template <class Args_01,
              class Args_02,
              class Args_03,
              class Args_04,
              class Args_05,
              class Args_06,
              class Args_07>
    void emplace(BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) args_01,
                 BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) args_02,
                 BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) args_03,
                 BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) args_04,
                 BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) args_05,
                 BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) args_06,
                 BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) args_07);

    template <class Args_01,
              class Args_02,
              class Args_03,
              class Args_04,
              class Args_05,
              class Args_06,
              class Args_07,
              class Args_08>
    void emplace(BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) args_01,
                 BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) args_02,
                 BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) args_03,
                 BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) args_04,
                 BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) args_05,
                 BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) args_06,
                 BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) args_07,
                 BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) args_08);

    template <class Args_01,
              class Args_02,
              class Args_03,
              class Args_04,
              class Args_05,
              class Args_06,
              class Args_07,
              class Args_08,
              class Args_09>
    void emplace(BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) args_01,
                 BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) args_02,
                 BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) args_03,
                 BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) args_04,
                 BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) args_05,
                 BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) args_06,
                 BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) args_07,
                 BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) args_08,
                 BSLS_COMPILERFEATURES_FORWARD_REF(Args_09) args_09);

    template <class Args_01,
              class Args_02,
              class Args_03,
              class Args_04,
              class Args_05,
              class Args_06,
              class Args_07,
              class Args_08,
              class Args_09,
              class Args_10>
    void emplace(BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) args_01,
                 BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) args_02,
                 BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) args_03,
                 BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) args_04,
                 BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) args_05,
                 BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) args_06,
                 BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) args_07,
                 BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) args_08,
                 BSLS_COMPILERFEATURES_FORWARD_REF(Args_09) args_09,
                 BSLS_COMPILERFEATURES_FORWARD_REF(Args_10) args_10);

#else
// The generated code below is a workaround for the absence of perfect
// forwarding in some compilers.
    template <class... Args>
    void emplace(BSLS_COMPILERFEATURES_FORWARD_REF(Args)... args);
// }}} END GENERATED CODE
#endif

    void pop();
        // Remove the top element from this stack.  The behavior is undefined
        // if the stack is empty.

    void push(const value_type& value);
        // Push the specified 'value' onto the top of the stack.

#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
    void push(value_type&& value);
        // TBD
#endif

    void swap(stack& other);
        // Exchange the value of this object with the value of the specified
        // 'other' object.

    reference top();
        // Return a reference to the element at the top of this stack.  The
        // behavior is undefined if the stack is empty.

    // ACCESSORS
    bool empty() const;
        // Return 'true' if this stack contains no elements and 'false'
        // otherwise.

    size_type size() const;
        // Return the number of elements contained in this stack.

    const_reference top() const;
        // Return a reference providing non-modifiable access to the element at
        // the top of this stack.  The behavior is undefined if the stack is
        // empty.
};

#if 0
// 'uses_allocator' and 'is_constructible' depend on the underlying 'container'
// having these constructs, which 'deque', 'vector', and 'list' don't yet.

                            // ==============
                            // uses_allocator
                            // ==============

template <class VALUE, class CONTAINER, class ALLOCATOR>
struct uses_allocator<stack<VALUE, CONTAINER>, ALLOCATOR>
: uses_allocator<CONTAINER, ALLOCATOR>::type {
    // doc TBD
};

#endif

//=============================================================================
//                              FREE OPERATORS
//=============================================================================

template <class VALUE, class CONTAINER>
bool operator==(const stack<VALUE, CONTAINER>& lhs,
                const stack<VALUE, CONTAINER>& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects have the same
    // value, and 'false' otherwise.  Two 'stack' objects have the same value
    // if their underlying containers have the same value.

template <class VALUE, class CONTAINER>
bool operator!=(const stack<VALUE, CONTAINER>& lhs,
                const stack<VALUE, CONTAINER>& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects do not have the
    // same value, and 'false' otherwise.  Two 'stack' objects have the same
    // value if their underlying containers have the same value.

template <class VALUE, class CONTAINER>
bool operator< (const stack<VALUE, CONTAINER>& lhs,
                const stack<VALUE, CONTAINER>& rhs);
    // Return 'true' if the specified 'lhs' is less than the specified 'rhs'.
    // One 'stack' object is less than another if its underlying container is
    // less than the other's underlying container.

template <class VALUE, class CONTAINER>
bool operator> (const stack<VALUE, CONTAINER>& lhs,
                const stack<VALUE, CONTAINER>& rhs);
    // Return 'true' if the specified 'lhs' is greater than the specified
    // 'rhs'.  One 'stack' object is greater than another if its underlying
    // container is greater than the other's underlying container.

template <class VALUE, class CONTAINER>
bool operator<=(const stack<VALUE, CONTAINER>& lhs,
                const stack<VALUE, CONTAINER>& rhs);
    // Return 'true' if the specified 'lhs' is less than or equal to the
    // specified 'rhs'.  One 'stack' object is less than or equal to another if
    // its underlying container is less than or equal to the other's underlying
    // container.

template <class VALUE, class CONTAINER>
bool operator>=(const stack<VALUE, CONTAINER>& lhs,
                const stack<VALUE, CONTAINER>& rhs);
    // Return 'true' if the specified 'lhs' is greater than or equal to the
    // specified 'rhs'.  One 'stack' object is greater than or equal to another
    // if its underlying container is greater than or equal to the other's
    // underlying container.

template <class VALUE, class CONTAINER>
void swap(stack<VALUE, CONTAINER>& lhs,
          stack<VALUE, CONTAINER>& rhs);
    // Swap the contents of 'lhs' and 'rhs'.  The behavior is undefined unless
    // 'lhs' and 'rhs' use the same allocator.

//=============================================================================
//                          INLINE FUNCTION DEFINITIONS
//=============================================================================

// CREATORS
template <class VALUE, class CONTAINER>
inline
stack<VALUE, CONTAINER>::stack()
: d_container()
, c(d_container)
{}

template <class VALUE, class CONTAINER>
template <class ALLOCATOR>
inline
stack<VALUE, CONTAINER>::stack(const ALLOCATOR& basicAllocator,
                               typename enable_if<
                                      Stack_HasAllocatorType<CONTAINER>::VALUE,
                                      ALLOCATOR>::type *)
: d_container(basicAllocator)
, c(d_container)
{}

template <class VALUE, class CONTAINER>
inline
stack<VALUE, CONTAINER>::stack(const CONTAINER& container)
: d_container(container)
, c(d_container)
{}

template <class VALUE, class CONTAINER>
template <class ALLOCATOR>
inline
stack<VALUE, CONTAINER>::stack(
                           const CONTAINER& container,
                           const ALLOCATOR& basicAllocator,
                           typename enable_if<
                                      Stack_HasAllocatorType<CONTAINER>::VALUE,
                                      ALLOCATOR>::type *)
: d_container(container, basicAllocator)
, c(d_container)
{}

template <class VALUE, class CONTAINER>
inline
stack<VALUE, CONTAINER>::stack(const stack& original)
: d_container(original.d_container)
, c(d_container)
{}

template <class VALUE, class CONTAINER>
template <class ALLOCATOR>
inline
stack<VALUE, CONTAINER>::stack(
                           const stack&     original,
                           const ALLOCATOR& basicAllocator,
                           typename enable_if<
                                      Stack_HasAllocatorType<CONTAINER>::VALUE,
                                      ALLOCATOR>::type *)
: d_container(original.d_container, basicAllocator)
, c(d_container)
{}

#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES

template <class VALUE, class CONTAINER>
inline
stack<VALUE, CONTAINER>::stack(CONTAINER&& container)
: d_container(BloombergLP::bslmf::MovableRefUtil::move(container))
, c(d_container)
{}

template <class VALUE, class CONTAINER>
template <class ALLOCATOR>
inline
stack<VALUE, CONTAINER>::stack(
                           CONTAINER&&      container,
                           const ALLOCATOR& basicAllocator,
                           typename enable_if<
                                      Stack_HasAllocatorType<CONTAINER>::VALUE,
                                      ALLOCATOR>::type *)
: d_container(BloombergLP::bslmf::MovableRefUtil::move(container),
              basicAllocator)
, c(d_container)
{}

template <class VALUE, class CONTAINER>
inline
stack<VALUE, CONTAINER>::stack(stack&& original)
: d_container(BloombergLP::bslmf::MovableRefUtil::move(original.d_container))
, c(d_container)
{}

template <class VALUE, class CONTAINER>
template <class ALLOCATOR>
inline
stack<VALUE, CONTAINER>::stack(
                           stack&&          original,
                           const ALLOCATOR& basicAllocator,
                           typename enable_if<
                                      Stack_HasAllocatorType<CONTAINER>::VALUE,
                                      ALLOCATOR>::type *)
: d_container(BloombergLP::bslmf::MovableRefUtil::move(original.d_container),
              basicAllocator)
, c(d_container)
{}

#endif

// MANIPULATORS
template <class VALUE, class CONTAINER>
inline
stack<VALUE, CONTAINER>& stack<VALUE, CONTAINER>::operator=(const stack& rhs)
{
    d_container = rhs.d_container;

    return *this;
}

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
template <class VALUE, class CONTAINER>
template <class... Args>
inline
void stack<VALUE, CONTAINER>::emplace(Args&&... args)
{
    d_container.emplace_back(BSLS_COMPILERFEATURES_FORWARD(Args,args)...);
}
#elif BSLS_COMPILERFEATURES_SIMULATE_VARIADIC_TEMPLATES
// {{{ BEGIN GENERATED CODE
// The following section is automatically generated.  **DO NOT EDIT**
// Generator command line: sim_cpp11_features.pl bslstl_stack.h
template <class VALUE, class CONTAINER>
inline
void stack<VALUE, CONTAINER>::emplace(
                               )
{
    d_container.emplace_back();
}

template <class VALUE, class CONTAINER>
template <class Args_01>
inline
void stack<VALUE, CONTAINER>::emplace(
                            BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) args_01)
{
    d_container.emplace_back(BSLS_COMPILERFEATURES_FORWARD(Args_01,args_01));
}

template <class VALUE, class CONTAINER>
template <class Args_01,
          class Args_02>
inline
void stack<VALUE, CONTAINER>::emplace(
                            BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) args_01,
                            BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) args_02)
{
    d_container.emplace_back(BSLS_COMPILERFEATURES_FORWARD(Args_01,args_01),
                             BSLS_COMPILERFEATURES_FORWARD(Args_02,args_02));
}

template <class VALUE, class CONTAINER>
template <class Args_01,
          class Args_02,
          class Args_03>
inline
void stack<VALUE, CONTAINER>::emplace(
                            BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) args_01,
                            BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) args_02,
                            BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) args_03)
{
    d_container.emplace_back(BSLS_COMPILERFEATURES_FORWARD(Args_01,args_01),
                             BSLS_COMPILERFEATURES_FORWARD(Args_02,args_02),
                             BSLS_COMPILERFEATURES_FORWARD(Args_03,args_03));
}

template <class VALUE, class CONTAINER>
template <class Args_01,
          class Args_02,
          class Args_03,
          class Args_04>
inline
void stack<VALUE, CONTAINER>::emplace(
                            BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) args_01,
                            BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) args_02,
                            BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) args_03,
                            BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) args_04)
{
    d_container.emplace_back(BSLS_COMPILERFEATURES_FORWARD(Args_01,args_01),
                             BSLS_COMPILERFEATURES_FORWARD(Args_02,args_02),
                             BSLS_COMPILERFEATURES_FORWARD(Args_03,args_03),
                             BSLS_COMPILERFEATURES_FORWARD(Args_04,args_04));
}

template <class VALUE, class CONTAINER>
template <class Args_01,
          class Args_02,
          class Args_03,
          class Args_04,
          class Args_05>
inline
void stack<VALUE, CONTAINER>::emplace(
                            BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) args_01,
                            BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) args_02,
                            BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) args_03,
                            BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) args_04,
                            BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) args_05)
{
    d_container.emplace_back(BSLS_COMPILERFEATURES_FORWARD(Args_01,args_01),
                             BSLS_COMPILERFEATURES_FORWARD(Args_02,args_02),
                             BSLS_COMPILERFEATURES_FORWARD(Args_03,args_03),
                             BSLS_COMPILERFEATURES_FORWARD(Args_04,args_04),
                             BSLS_COMPILERFEATURES_FORWARD(Args_05,args_05));
}

template <class VALUE, class CONTAINER>
template <class Args_01,
          class Args_02,
          class Args_03,
          class Args_04,
          class Args_05,
          class Args_06>
inline
void stack<VALUE, CONTAINER>::emplace(
                            BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) args_01,
                            BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) args_02,
                            BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) args_03,
                            BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) args_04,
                            BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) args_05,
                            BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) args_06)
{
    d_container.emplace_back(BSLS_COMPILERFEATURES_FORWARD(Args_01,args_01),
                             BSLS_COMPILERFEATURES_FORWARD(Args_02,args_02),
                             BSLS_COMPILERFEATURES_FORWARD(Args_03,args_03),
                             BSLS_COMPILERFEATURES_FORWARD(Args_04,args_04),
                             BSLS_COMPILERFEATURES_FORWARD(Args_05,args_05),
                             BSLS_COMPILERFEATURES_FORWARD(Args_06,args_06));
}

template <class VALUE, class CONTAINER>
template <class Args_01,
          class Args_02,
          class Args_03,
          class Args_04,
          class Args_05,
          class Args_06,
          class Args_07>
inline
void stack<VALUE, CONTAINER>::emplace(
                            BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) args_01,
                            BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) args_02,
                            BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) args_03,
                            BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) args_04,
                            BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) args_05,
                            BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) args_06,
                            BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) args_07)
{
    d_container.emplace_back(BSLS_COMPILERFEATURES_FORWARD(Args_01,args_01),
                             BSLS_COMPILERFEATURES_FORWARD(Args_02,args_02),
                             BSLS_COMPILERFEATURES_FORWARD(Args_03,args_03),
                             BSLS_COMPILERFEATURES_FORWARD(Args_04,args_04),
                             BSLS_COMPILERFEATURES_FORWARD(Args_05,args_05),
                             BSLS_COMPILERFEATURES_FORWARD(Args_06,args_06),
                             BSLS_COMPILERFEATURES_FORWARD(Args_07,args_07));
}

template <class VALUE, class CONTAINER>
template <class Args_01,
          class Args_02,
          class Args_03,
          class Args_04,
          class Args_05,
          class Args_06,
          class Args_07,
          class Args_08>
inline
void stack<VALUE, CONTAINER>::emplace(
                            BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) args_01,
                            BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) args_02,
                            BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) args_03,
                            BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) args_04,
                            BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) args_05,
                            BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) args_06,
                            BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) args_07,
                            BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) args_08)
{
    d_container.emplace_back(BSLS_COMPILERFEATURES_FORWARD(Args_01,args_01),
                             BSLS_COMPILERFEATURES_FORWARD(Args_02,args_02),
                             BSLS_COMPILERFEATURES_FORWARD(Args_03,args_03),
                             BSLS_COMPILERFEATURES_FORWARD(Args_04,args_04),
                             BSLS_COMPILERFEATURES_FORWARD(Args_05,args_05),
                             BSLS_COMPILERFEATURES_FORWARD(Args_06,args_06),
                             BSLS_COMPILERFEATURES_FORWARD(Args_07,args_07),
                             BSLS_COMPILERFEATURES_FORWARD(Args_08,args_08));
}

template <class VALUE, class CONTAINER>
template <class Args_01,
          class Args_02,
          class Args_03,
          class Args_04,
          class Args_05,
          class Args_06,
          class Args_07,
          class Args_08,
          class Args_09>
inline
void stack<VALUE, CONTAINER>::emplace(
                            BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) args_01,
                            BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) args_02,
                            BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) args_03,
                            BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) args_04,
                            BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) args_05,
                            BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) args_06,
                            BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) args_07,
                            BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) args_08,
                            BSLS_COMPILERFEATURES_FORWARD_REF(Args_09) args_09)
{
    d_container.emplace_back(BSLS_COMPILERFEATURES_FORWARD(Args_01,args_01),
                             BSLS_COMPILERFEATURES_FORWARD(Args_02,args_02),
                             BSLS_COMPILERFEATURES_FORWARD(Args_03,args_03),
                             BSLS_COMPILERFEATURES_FORWARD(Args_04,args_04),
                             BSLS_COMPILERFEATURES_FORWARD(Args_05,args_05),
                             BSLS_COMPILERFEATURES_FORWARD(Args_06,args_06),
                             BSLS_COMPILERFEATURES_FORWARD(Args_07,args_07),
                             BSLS_COMPILERFEATURES_FORWARD(Args_08,args_08),
                             BSLS_COMPILERFEATURES_FORWARD(Args_09,args_09));
}

template <class VALUE, class CONTAINER>
template <class Args_01,
          class Args_02,
          class Args_03,
          class Args_04,
          class Args_05,
          class Args_06,
          class Args_07,
          class Args_08,
          class Args_09,
          class Args_10>
inline
void stack<VALUE, CONTAINER>::emplace(
                            BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) args_01,
                            BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) args_02,
                            BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) args_03,
                            BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) args_04,
                            BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) args_05,
                            BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) args_06,
                            BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) args_07,
                            BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) args_08,
                            BSLS_COMPILERFEATURES_FORWARD_REF(Args_09) args_09,
                            BSLS_COMPILERFEATURES_FORWARD_REF(Args_10) args_10)
{
    d_container.emplace_back(BSLS_COMPILERFEATURES_FORWARD(Args_01,args_01),
                             BSLS_COMPILERFEATURES_FORWARD(Args_02,args_02),
                             BSLS_COMPILERFEATURES_FORWARD(Args_03,args_03),
                             BSLS_COMPILERFEATURES_FORWARD(Args_04,args_04),
                             BSLS_COMPILERFEATURES_FORWARD(Args_05,args_05),
                             BSLS_COMPILERFEATURES_FORWARD(Args_06,args_06),
                             BSLS_COMPILERFEATURES_FORWARD(Args_07,args_07),
                             BSLS_COMPILERFEATURES_FORWARD(Args_08,args_08),
                             BSLS_COMPILERFEATURES_FORWARD(Args_09,args_09),
                             BSLS_COMPILERFEATURES_FORWARD(Args_10,args_10));
}

#else
// The generated code below is a workaround for the absence of perfect
// forwarding in some compilers.
template <class VALUE, class CONTAINER>
template <class... Args>
inline
void stack<VALUE, CONTAINER>::emplace(
                               BSLS_COMPILERFEATURES_FORWARD_REF(Args)... args)
{
    d_container.emplace_back(BSLS_COMPILERFEATURES_FORWARD(Args,args)...);
}
// }}} END GENERATED CODE
#endif

template <class VALUE, class CONTAINER>
inline
void stack<VALUE, CONTAINER>::pop()
{
    BSLS_ASSERT_SAFE(!empty());

    d_container.pop_back();
}

template <class VALUE, class CONTAINER>
inline
void stack<VALUE, CONTAINER>::push(const value_type& value)
{
    d_container.push_back(value);
}

#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
template <class VALUE, class CONTAINER>
inline
void stack<VALUE, CONTAINER>::push(value_type&& value)
{
    d_container.push_back(BloombergLP::bslmf::MovableRefUtil::move(value));
}
#endif

template <class VALUE, class CONTAINER>
inline
void stack<VALUE, CONTAINER>::swap(stack& other)
{
    BloombergLP::bslalg::SwapUtil::swap(&d_container, &other.d_container);
}

template <class VALUE, class CONTAINER>
inline
typename CONTAINER::reference stack<VALUE, CONTAINER>::top()
{
    BSLS_ASSERT_SAFE(!empty());

    return d_container.back();
}

// ACCESSORS
template <class VALUE, class CONTAINER>
inline
bool stack<VALUE, CONTAINER>::empty() const
{
    return 0 == d_container.size();
}

template <class VALUE, class CONTAINER>
inline
typename CONTAINER::size_type stack<VALUE, CONTAINER>::size() const
{
    return d_container.size();
}

template <class VALUE, class CONTAINER>
inline
typename CONTAINER::const_reference stack<VALUE, CONTAINER>::top() const
{
    return d_container.back();
}

// FREE OPERATORS
template <class VALUE, class CONTAINER>
inline
bool operator==(const stack<VALUE, CONTAINER>& lhs,
                const stack<VALUE, CONTAINER>& rhs)
{
    return lhs.d_container == rhs.d_container;
}

template <class VALUE, class CONTAINER>
inline
bool operator!=(const stack<VALUE, CONTAINER>& lhs,
                const stack<VALUE, CONTAINER>& rhs)
{
    return lhs.d_container != rhs.d_container;
}

template <class VALUE, class CONTAINER>
inline
bool operator< (const stack<VALUE, CONTAINER>& lhs,
                const stack<VALUE, CONTAINER>& rhs)
{
    return lhs.d_container < rhs.d_container;
}

template <class VALUE, class CONTAINER>
inline
bool operator> (const stack<VALUE, CONTAINER>& lhs,
                const stack<VALUE, CONTAINER>& rhs)
{
    return lhs.d_container > rhs.d_container;
}

template <class VALUE, class CONTAINER>
inline
bool operator<=(const stack<VALUE, CONTAINER>& lhs,
                const stack<VALUE, CONTAINER>& rhs)
{
    return lhs.d_container <= rhs.d_container;
}

template <class VALUE, class CONTAINER>
inline
bool operator>=(const stack<VALUE, CONTAINER>& lhs,
                const stack<VALUE, CONTAINER>& rhs)
{
    return lhs.d_container >= rhs.d_container;
}

template <class VALUE, class CONTAINER>
inline
void swap(stack<VALUE, CONTAINER>& lhs,
          stack<VALUE, CONTAINER>& rhs)
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
