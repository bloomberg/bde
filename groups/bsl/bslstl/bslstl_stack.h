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
//   bsl::stack: STL-compliant stack template
//
//@SEE_ALSO: bslstl_deque, bslstl_vector, bslstl_list, bslstl_queue,
//           bslstl_priorityqueue
//
//@DESCRIPTION: This component defines a single class template, 'bsl::stack',
// a container adapter that takes an underlying container and provides a stack
// interface which the user accesses primarily through 'push', 'pop', and 'top'
// operations.  A 'deque' (the default), 'vector', or 'list' may be used, but
// any container which supports 'push_back', 'pop_back', 'back', and 'size',
// plus a template specialization 'uses_allocator::type', may be used.
//
// A stack meets the requirements of a container adaptor as described in the
// C++ standard [stack].  The 'stack' implemented here adheres to the C++11
// standard when compiled with a C++11 compiler, and makes the best
// approximation when compiled with a C++03 compiler.  In particular, for C++03
// we emulate move semantics, but limit forwarding (in 'emplace') to 'const'
// lvalues, and make no effort to emulate 'noexcept' or initializer-lists.
//
///Requirements on 'CONTAINER'
///---------------------------
// The 'bsl::stack' adapter can accept for its (optional) 'CONTAINER' template
// parameter 'bsl::deque' (the default), 'bsl::vector', 'bsl::list', or other
// container classes that support the following types and methods.
//
///Required Types
/// - - - - - - -
//: o 'value_type'
//: o 'reference'
//: o 'const_reference'
//: o 'size_type'
//: o 'allocator_type' (if any 'stack' constructor taking an allocator is used)
//
///Required Methods, Free Operators, and Free Functions
/// - - - - - - - - - - - - - - - - - - - - - - - - - -
//: o 'void push_back(const value_type&)' (and variant taking rvalue reference)
//: o 'void pop_back()'
//: o 'reference back()'
//: o 'size_type size() const'
//: o 'const_reference back() const'
//: o 'emplace_back'
//: o copy-assignment and move-assignment operators
//: o free '==', '!=', '<', '>', '<=', '>=' operators
//: o free 'swap' function (found via ADL with 'std::swap' in the lookup set)
//
///Requirements on 'VALUE'
///-----------------------
// If a type is specified for the 'CONTAINER' template parameter, a type
// equivalent to 'CONTAINER::value_type' must be specified for 'VALUE' or else
// compilation failure will result.
//
// The following term is used to more precisely specify the requirements on
// template parameter types in function-level documentation:
//
//: *equality-comparable*:
//:   The type provides an equality-comparison operator that defines an
//:   equivalence relationship and is both reflexive and transitive.
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
// 'bsl::allocator' which is implicitly convertible from 'bslma::Allocator *',
// and which can be converted to a 'bslma::Allocator *' through the 'mechanism'
// accessor.
//
// Hence if the underlying container takes 'bsl::allocator', then the 'stack'
// object can take 'bslma_Allocator *'s to supply memory allocation.  If no
// allocator is specified, 'allocator()' is used, which winds up using
// 'bslma::Default::allocator(0)'.
//
///Operations
///----------
// Below is a list of public methods of the 'bsl::stack' class that effectively
// forward their implementations to corresponding operations in the held
// container (referenced as 'c') which is here assumed to be either
// 'bsl::deque', or 'bsl::vector', or 'bsl::list'.
//..
//  Legend
//  ------
//  'C'             - (template parameter) type 'CONTAINER' of the stack
//  'V'             - (template parameter) type 'VALUE'     of the stack
//  's', 't'        - two distinct objects of type 'stack<V, C>'
//
//  'nc'            - number of elements in container 'c'
//  'n', 'm'        - number of elements in 's' and 't', respectively
//  'al'            - STL-style memory allocator
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
// First, we define the class implementing the to-do list.
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
//..
//  ToDoList toDoList;
//..
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

#ifndef INCLUDED_BSLSTL_DEQUE
#include <bslstl_deque.h>
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

#ifndef INCLUDED_BSLMF_NESTEDTRAITDECLARATION
#include <bslmf_nestedtraitdeclaration.h>
#endif

#ifndef INCLUDED_BSLMF_USESALLOCATOR
#include <bslmf_usesallocator.h>
#endif

#ifndef INCLUDED_BSLS_COMPILERFEATURES
#include <bsls_compilerfeatures.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORM
#include <bsls_platform.h>
#endif

#ifndef INCLUDED_BSLS_UTIL
#include <bsls_util.h>
#endif

namespace bsl {

                             // ===========
                             // class stack
                             // ===========

template <class VALUE, class CONTAINER = deque<VALUE> >
class stack {
    // This 'class' defines a container adapter which supports access primarily
    // via 'push', 'pop', and 'top'.  This type can be based on a variety of
    // other container types, including 'deque', 'vector', and 'list'.  This
    // type is value-semantic if the supporting 'CONTAINER' and 'VALUE' are
    // value-semantic.
    //
    // Note that we never use 'VALUE' in the implementation except in the
    // default argument of 'CONTAINER'.  We use 'CONTAINER::value_type' for
    // everything, which means that if 'CONTAINER' is specified, then 'VALUE'
    // is ignored.

  private:
    // PRIVATE TYPES
    typedef BloombergLP::bslmf::MovableRefUtil  MoveUtil;
        // This 'typedef' is a convenient alias for the utility associated with
        // movable references.

  public:
    // PUBLIC TYPES
    typedef typename CONTAINER::value_type      value_type;
    typedef typename CONTAINER::reference       reference;
    typedef typename CONTAINER::const_reference const_reference;
    typedef typename CONTAINER::size_type       size_type;
    typedef          CONTAINER                  container_type;

  protected:
    // PROTECTED DATA
    container_type  c;    // We are required by the standard to have the
                          // container be a protected variable named 'c'.

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
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION_IF(
        stack,
        BloombergLP::bslma::UsesBslmaAllocator,
        BloombergLP::bslma::UsesBslmaAllocator<container_type>::value);

    // CREATORS
    explicit stack();
        // Create an empty stack.  No allocator will be provided to the
        // underlying container.  That container's memory allocation will be
        // provided by the default allocator of its type.

    stack(const stack& original);
        // Create a stack having the value of the specified 'original'.  The
        // currently installed default allocator is used to supply memory.

    stack(BloombergLP::bslmf::MovableRef<stack> original);
        // Create a stack having the value of the specified 'original' by
        // moving the contents of 'original' to the new stack.  The allocator
        // associated with 'original' is propagated for use in the new stack.
        // 'original' is left in a valid but unspecified state.

    explicit
    stack(const CONTAINER& container);
        // Create a stack whose underlying container has the value of the
        // specified 'container'.  The currently installed default allocator is
        // used to supply memory.

    explicit
    stack(BloombergLP::bslmf::MovableRef<CONTAINER> container);
        // Create a stack whose underlying container has the value of the
        // specified 'container' (on entry) by moving the contents of
        // 'container' to the new stack.  The allocator associated with
        // 'container' is propagated for use in the new stack.  'container' is
        // left in a valid but unspecified state.

    template <class ALLOCATOR>
    explicit
    stack(const ALLOCATOR& basicAllocator,
          typename enable_if<bsl::uses_allocator<CONTAINER, ALLOCATOR>::value,
                             ALLOCATOR>::type * = 0);
        // Create an empty stack, and use the specified 'basicAllocator' to
        // supply memory.  If 'CONTAINER::allocator_type' does not exist, this
        // constructor may not be used.

    template <class ALLOCATOR>
    stack(const CONTAINER& container,
          const ALLOCATOR& basicAllocator,
          typename enable_if<bsl::uses_allocator<CONTAINER, ALLOCATOR>::value,
                             ALLOCATOR>::type * = 0);
        // Create a stack whose underlying container has the value of the
        // specified 'container', and use the specified 'basicAllocator' to
        // supply memory.  If 'CONTAINER::allocator_type' does not exist, this
        // constructor may not be used.

    template <class ALLOCATOR>
    stack(const stack&     original,
          const ALLOCATOR& basicAllocator,
          typename enable_if<bsl::uses_allocator<CONTAINER, ALLOCATOR>::value,
                             ALLOCATOR>::type * = 0);
        // Create a stack having the value of the specified stack 'original'
        // and use the specified 'basicAllocator' to supply memory.  If
        // 'CONTAINER::allocator_type' does not exist, this constructor may not
        // be used.

    template <class ALLOCATOR>
    stack(BloombergLP::bslmf::MovableRef<CONTAINER> container,
          const ALLOCATOR&                          basicAllocator,
          typename enable_if<bsl::uses_allocator<CONTAINER, ALLOCATOR>::value,
                             ALLOCATOR>::type * = 0);
        // Create a stack whose underlying container has the value of the
        // specified 'container' (on entry) that uses 'basicAllocator' to
        // supply memory by using the allocator-extended move constructor of
        // 'CONTAINER.  'container' is left in a valid but unspecified state.
        // A 'bslma::Allocator *' can be supplied for 'basicAllocator' if the
        // (template parameter) 'ALLOCATOR' is 'bsl::allocator' (the default).
        // This method assumes that 'CONTAINER' has a move constructor.  If
        // 'CONTAINER::allocator_type' does not exist, this constructor may not
        // be used.

    template <class ALLOCATOR>
    stack(BloombergLP::bslmf::MovableRef<stack> original,
          const ALLOCATOR&                      basicAllocator,
          typename enable_if<bsl::uses_allocator<CONTAINER, ALLOCATOR>::value,
                             ALLOCATOR>::type * = 0);
        // Create a stack having the value of the specified 'original' (on
        // entry) that uses 'basicAllocator' to supply memory by using the
        // allocator-extended moved constructor of 'CONTAINER'.  'original' is
        // left in a valid but unspecified state.  Note that a
        // 'bslma::Allocator *' can be supplied for 'basicAllocator' if the
        // (template parameter) 'ALLOCATOR' is 'bsl::allocator' (the default).
        // Also note that this method assumes that 'CONTAINER' has a move
        // constructor.  Also note that if 'CONTAINER::allocator_type' does not
        // exist, this constructor may not be used.

    // MANIPULATORS
    stack& operator=(const stack& rhs);
        // Assign to this object the value of the specified 'rhs' object, and
        // return a reference providing modifiable access to this object.

    stack& operator=(BloombergLP::bslmf::MovableRef<stack> rhs)
             BSLS_CPP11_NOEXCEPT_SPECIFICATION(BSLS_CPP11_PROVISIONALLY_FALSE);
        // Assign to this object the value of the specified 'rhs' object, and
        // return a reference providing modifiable access to this object.  The
        // contents of 'rhs' are moved to this stack using the move-assignment
        // operator of 'CONTAINER'.  'rhs' is left in a valid but unspecified
        // state, and if an exception is thrown, '*this' is left in a valid but
        // unspecified state.

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
    template <class... Args>
    void emplace(Args&&... args);
        // Push onto this stack a newly created 'value_type' object constructed
        // by forwarding 'get_allocator()' (if required) and the specified
        // (variable number of) 'args' to the corresponding constructor of
        // 'value_type'.

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
        // if this stack is empty.

    void push(const value_type& value);
        // Push the specified 'value' onto the top of this stack.

    void push(BloombergLP::bslmf::MovableRef<value_type> value);
        // Push onto this stack a 'value_type' object having the value of the
        // specified 'value' (on entry) by moving the contents of 'value' to
        // the new object on this stack.  'value' is left in a valid but
        // unspecified state.

    void swap(stack& other)
             BSLS_CPP11_NOEXCEPT_SPECIFICATION(BSLS_CPP11_PROVISIONALLY_FALSE);
        // Exchange the value of this stack with the value of the specified
        // 'other' stack.

    reference top();
        // Return a reference to the element at the top of this stack.  The
        // behavior is undefined if this stack is empty.

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

// FREE OPERATORS
template <class VALUE, class CONTAINER>
bool operator==(const stack<VALUE, CONTAINER>& lhs,
                const stack<VALUE, CONTAINER>& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects have the same
    // value, and 'false' otherwise.  Two 'stack' objects 'lhs' and 'rhs' have
    // the same value if they have the same number of elements, and each
    // element in the ordered sequence of elements of 'lhs' has the same value
    // as the corresponding element in the ordered sequence of elements of
    // 'rhs'.  This method requires that the (template parameter) type 'VALUE'
    // be 'equality-comparable' (see {Requirements on 'VALUE'}).

template <class VALUE, class CONTAINER>
bool operator!=(const stack<VALUE, CONTAINER>& lhs,
                const stack<VALUE, CONTAINER>& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects do not have the
    // same value, and 'false' otherwise.  Two 'stack' objects 'lhs' and 'rhs'
    // do not have the same value if they do not have the same number of
    // elements, or some element in the ordered sequence of elements of 'lhs'
    // does not have the same value as the corresponding element in the ordered
    // sequence of elements of 'rhs'.  This method requires that the (template
    // parameter) type 'VALUE' be 'equality-comparable' (see {Requirements on
    // 'VALUE'}).

template <class VALUE, class CONTAINER>
bool operator< (const stack<VALUE, CONTAINER>& lhs,
                const stack<VALUE, CONTAINER>& rhs);
    // Return 'true' if the value of the specified 'lhs' stack is
    // lexicographically less than that of the specified 'rhs' stack, and
    // 'false' otherwise.  Given iterators 'i' and 'j' over the respective
    // sequences '[lhs.begin() .. lhs.end())' and '[rhs.begin() .. rhs.end())',
    // the value of stack 'lhs' is lexicographically less than that of stack
    // 'rhs' if 'true == *i < *j' for the first pair of corresponding iterator
    // positions where '*i < *j' and '*j < *i' are not both 'false'.  If no
    // such corresponding iterator position exists, the value of 'lhs' is
    // lexicographically less than that of 'rhs' if 'lhs.size() < rhs.size()'.
    // This method requires that 'operator<', inducing a total order, be
    // defined for 'value_type'.

template <class VALUE, class CONTAINER>
bool operator> (const stack<VALUE, CONTAINER>& lhs,
                const stack<VALUE, CONTAINER>& rhs);
    // Return 'true' if the value of the specified 'lhs' stack is
    // lexicographically greater than that of the specified 'rhs' stack, and
    // 'false' otherwise.  The value of stack 'lhs' is lexicographically
    // greater than that of stack 'rhs' if 'rhs' is lexicographically less than
    // 'lhs' (see 'operator<').  This method requires that 'operator<',
    // inducing a total order, be defined for 'value_type'.  Note that this
    // operator returns 'rhs < lhs'.

template <class VALUE, class CONTAINER>
bool operator<=(const stack<VALUE, CONTAINER>& lhs,
                const stack<VALUE, CONTAINER>& rhs);
    // Return 'true' if the value of the specified 'lhs' stack is
    // lexicographically less than or equal to that of the specified 'rhs'
    // stack, and 'false' otherwise.  The value of stack 'lhs' is
    // lexicographically less than or equal to that of stack 'rhs' if 'rhs' is
    // not lexicographically less than 'lhs' (see 'operator<').  This method
    // requires that 'operator<', inducing a total order, be defined for
    // 'value_type'.  Note that this operator returns '!(rhs < lhs)'.

template <class VALUE, class CONTAINER>
bool operator>=(const stack<VALUE, CONTAINER>& lhs,
                const stack<VALUE, CONTAINER>& rhs);
    // Return 'true' if the value of the specified 'lhs' stack is
    // lexicographically greater than or equal to that of the specified 'rhs'
    // stack, and 'false' otherwise.  The value of stack 'lhs' is
    // lexicographically greater than or equal to that of stack 'rhs' if 'lhs'
    // is not lexicographically less than 'rhs' (see 'operator<').  This method
    // requires that 'operator<', inducing a total order, be defined for
    // 'value_type'.  Note that this operator returns '!(lhs < rhs)'.

// FREE FUNCTIONS
template <class VALUE, class CONTAINER>
void swap(stack<VALUE, CONTAINER>& lhs,
          stack<VALUE, CONTAINER>& rhs)
             BSLS_CPP11_NOEXCEPT_SPECIFICATION(BSLS_CPP11_PROVISIONALLY_FALSE);
    // Swap the value of the specified 'lhs' stack with the value of the
    // specified 'rhs' stack.

//=============================================================================
//                  TEMPLATE AND INLINE FUNCTION DEFINITIONS
//=============================================================================

                             // -----------
                             // class stack
                             // -----------

// CREATORS
template <class VALUE, class CONTAINER>
inline
stack<VALUE, CONTAINER>::stack()
: c()
{
}

template <class VALUE, class CONTAINER>
inline
stack<VALUE, CONTAINER>::stack(const CONTAINER& container)
: c(container)
{
}

template <class VALUE, class CONTAINER>
inline
stack<VALUE, CONTAINER>::stack(BloombergLP::bslmf::MovableRef<stack> original)
: c(MoveUtil::move(MoveUtil::access(original).c))
{
}

template <class VALUE, class CONTAINER>
template <class ALLOCATOR>
inline
stack<VALUE, CONTAINER>::stack(const ALLOCATOR& basicAllocator,
           typename enable_if<bsl::uses_allocator<CONTAINER, ALLOCATOR>::value,
                              ALLOCATOR>::type *)
: c(basicAllocator)
{
}

template <class VALUE, class CONTAINER>
template <class ALLOCATOR>
inline
stack<VALUE, CONTAINER>::stack(
    const CONTAINER& container,
    const ALLOCATOR& basicAllocator,
    typename enable_if<bsl::uses_allocator<CONTAINER, ALLOCATOR>::value,
                       ALLOCATOR>::type *)
: c(container, basicAllocator)
{
}

template <class VALUE, class CONTAINER>
inline
stack<VALUE, CONTAINER>::stack(const stack& original)
: c(original.c)
{
}

template <class VALUE, class CONTAINER>
template <class ALLOCATOR>
inline
stack<VALUE, CONTAINER>::stack(
    const stack&     original,
    const ALLOCATOR& basicAllocator,
    typename enable_if<bsl::uses_allocator<CONTAINER, ALLOCATOR>::value,
                       ALLOCATOR>::type *)
: c(original.c, basicAllocator)
{
}

template <class VALUE, class CONTAINER>
inline
stack<VALUE, CONTAINER>::stack(BloombergLP::bslmf::MovableRef<CONTAINER>
                                                                     container)
: c(MoveUtil::move(container))
{
}

template <class VALUE, class CONTAINER>
template <class ALLOCATOR>
inline
stack<VALUE, CONTAINER>::stack(
    BloombergLP::bslmf::MovableRef<CONTAINER> container,
    const ALLOCATOR&                          basicAllocator,
    typename enable_if<bsl::uses_allocator<CONTAINER, ALLOCATOR>::value,
                       ALLOCATOR>::type *)
: c(MoveUtil::move(container), basicAllocator)
{
}

template <class VALUE, class CONTAINER>
template <class ALLOCATOR>
inline
stack<VALUE, CONTAINER>::stack(
    BloombergLP::bslmf::MovableRef<stack> original,
    const ALLOCATOR&                      basicAllocator,
    typename enable_if<bsl::uses_allocator<CONTAINER, ALLOCATOR>::value,
                       ALLOCATOR>::type *)
: c(MoveUtil::move(MoveUtil::access(original).c), basicAllocator)
{
}

// MANIPULATORS
template <class VALUE, class CONTAINER>
inline
stack<VALUE, CONTAINER>& stack<VALUE, CONTAINER>::operator=(const stack& rhs)
{
    c = rhs.c;

    return *this;
}

template <class VALUE, class CONTAINER>
inline
stack<VALUE, CONTAINER>& stack<VALUE, CONTAINER>::operator=(
                                     BloombergLP::bslmf::MovableRef<stack> rhs)
              BSLS_CPP11_NOEXCEPT_SPECIFICATION(BSLS_CPP11_PROVISIONALLY_FALSE)
{
    c = MoveUtil::move(MoveUtil::access(rhs).c);
    return *this;
}

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
template <class VALUE, class CONTAINER>
template <class... Args>
inline
void stack<VALUE, CONTAINER>::emplace(Args&&... args)
{
    c.emplace_back(BSLS_COMPILERFEATURES_FORWARD(Args,args)...);
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
    c.emplace_back();
}

template <class VALUE, class CONTAINER>
template <class Args_01>
inline
void stack<VALUE, CONTAINER>::emplace(
                            BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) args_01)
{
    c.emplace_back(BSLS_COMPILERFEATURES_FORWARD(Args_01,args_01));
}

template <class VALUE, class CONTAINER>
template <class Args_01,
          class Args_02>
inline
void stack<VALUE, CONTAINER>::emplace(
                            BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) args_01,
                            BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) args_02)
{
    c.emplace_back(BSLS_COMPILERFEATURES_FORWARD(Args_01,args_01),
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
    c.emplace_back(BSLS_COMPILERFEATURES_FORWARD(Args_01,args_01),
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
    c.emplace_back(BSLS_COMPILERFEATURES_FORWARD(Args_01,args_01),
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
    c.emplace_back(BSLS_COMPILERFEATURES_FORWARD(Args_01,args_01),
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
    c.emplace_back(BSLS_COMPILERFEATURES_FORWARD(Args_01,args_01),
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
    c.emplace_back(BSLS_COMPILERFEATURES_FORWARD(Args_01,args_01),
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
    c.emplace_back(BSLS_COMPILERFEATURES_FORWARD(Args_01,args_01),
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
    c.emplace_back(BSLS_COMPILERFEATURES_FORWARD(Args_01,args_01),
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
    c.emplace_back(BSLS_COMPILERFEATURES_FORWARD(Args_01,args_01),
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
    c.emplace_back(BSLS_COMPILERFEATURES_FORWARD(Args,args)...);
}
// }}} END GENERATED CODE
#endif

template <class VALUE, class CONTAINER>
inline
void stack<VALUE, CONTAINER>::pop()
{
    BSLS_ASSERT_SAFE(!empty());

    c.pop_back();
}

template <class VALUE, class CONTAINER>
inline
void stack<VALUE, CONTAINER>::push(const value_type& value)
{
    c.push_back(value);
}

template <class VALUE, class CONTAINER>
inline
void stack<VALUE, CONTAINER>::push(BloombergLP::bslmf::MovableRef<value_type>
                                                                         value)
{
    c.push_back(MoveUtil::move(value));
}

template <class VALUE, class CONTAINER>
inline
void stack<VALUE, CONTAINER>::swap(stack& other)
              BSLS_CPP11_NOEXCEPT_SPECIFICATION(BSLS_CPP11_PROVISIONALLY_FALSE)
{
    BloombergLP::bslalg::SwapUtil::swap(&c, &other.c);
}

template <class VALUE, class CONTAINER>
inline
typename CONTAINER::reference stack<VALUE, CONTAINER>::top()
{
    BSLS_ASSERT_SAFE(!empty());

    return c.back();
}

// ACCESSORS
template <class VALUE, class CONTAINER>
inline
bool stack<VALUE, CONTAINER>::empty() const
{
    return 0 == c.size();
}

template <class VALUE, class CONTAINER>
inline
typename CONTAINER::size_type stack<VALUE, CONTAINER>::size() const
{
    return c.size();
}

template <class VALUE, class CONTAINER>
inline
typename CONTAINER::const_reference stack<VALUE, CONTAINER>::top() const
{
    return c.back();
}

// FREE OPERATORS
template <class VALUE, class CONTAINER>
inline
bool operator==(const stack<VALUE, CONTAINER>& lhs,
                const stack<VALUE, CONTAINER>& rhs)
{
    return lhs.c == rhs.c;
}

template <class VALUE, class CONTAINER>
inline
bool operator!=(const stack<VALUE, CONTAINER>& lhs,
                const stack<VALUE, CONTAINER>& rhs)
{
    return lhs.c != rhs.c;
}

template <class VALUE, class CONTAINER>
inline
bool operator< (const stack<VALUE, CONTAINER>& lhs,
                const stack<VALUE, CONTAINER>& rhs)
{
    return lhs.c < rhs.c;
}

template <class VALUE, class CONTAINER>
inline
bool operator> (const stack<VALUE, CONTAINER>& lhs,
                const stack<VALUE, CONTAINER>& rhs)
{
    return lhs.c > rhs.c;
}

template <class VALUE, class CONTAINER>
inline
bool operator<=(const stack<VALUE, CONTAINER>& lhs,
                const stack<VALUE, CONTAINER>& rhs)
{
    return lhs.c <= rhs.c;
}

template <class VALUE, class CONTAINER>
inline
bool operator>=(const stack<VALUE, CONTAINER>& lhs,
                const stack<VALUE, CONTAINER>& rhs)
{
    return lhs.c >= rhs.c;
}

// FREE FUNCTIONS
template <class VALUE, class CONTAINER>
inline
void swap(stack<VALUE, CONTAINER>& lhs,
          stack<VALUE, CONTAINER>& rhs)
              BSLS_CPP11_NOEXCEPT_SPECIFICATION(BSLS_CPP11_PROVISIONALLY_FALSE)
{
    lhs.swap(rhs);
}

}  // close namespace bsl

#endif

// ----------------------------------------------------------------------------
// Copyright 2016 Bloomberg Finance L.P.
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
