// bslstl_queue.h                                                     -*-C++-*-
#ifndef INCLUDED_BSLSTL_QUEUE
#define INCLUDED_BSLSTL_QUEUE

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide container adapter class template 'queue'.
//
//@CLASSES:
//   bsl::queue: class template of a first-in-first-out data structure
//
//@CANONICAL_HEADER: bsl_queue.h
//
//@SEE_ALSO: bslstl_priorityqueue, bslstl_stack
//
//@DESCRIPTION: This component defines a class template, 'bsl::queue', holding
// a container (of a parameterized type 'CONTAINER' containing elements of
// another parameterized type 'VALUE'), and adapting it to provide a
// first-in-first-out queue data structure.
//
// An instantiation of 'queue' is an allocator-aware, value-semantic type whose
// salient attributes are its size (number of elements held) and the sequence
// of values (of held elements) in the order that they were pushed into the
// 'queue'.  If 'queue' is instantiated with a parameterized type 'VALUE' that
// is not itself value-semantic, then it will not retain all of its
// value-semantic qualities.
//
// A queue meets the requirements of a container adaptor as described in the
// C++ standard [queue].  The 'queue' implemented here adheres to the C++11
// standard when compiled with a C++11 compiler, and makes the best
// approximation when compiled with a C++03 compiler.  In particular, for C++03
// we emulate move semantics, but limit forwarding (in 'emplace') to 'const'
// lvalues, and make no effort to emulate 'noexcept' or initializer-lists.
//
///Requirements on 'CONTAINER'
///---------------------------
// The 'bsl::queue' adapter can accept for its (optional) 'CONTAINER' template
// parameter 'bsl::deque' (the default), 'bsl::vector', or other container
// classes that support the following types and methods.
//
///Required Types
/// - - - - - - -
//: o 'value_type'
//: o 'reference'
//: o 'const_reference'
//: o 'size_type'
//: o 'allocator_type' (if any 'queue' constructor taking an allocator is used)
//
///Required Methods, Free Operators, and Free Functions
/// - - - - - - - - - - - - - - - - - - - - - - - - - -
//: o 'void push_back(const value_type&)' (and variant taking rvalue reference)
//: o 'void pop_front()'
//: o 'reference front()'
//: o 'reference back()'
//: o 'bool empty() const'
//: o 'size_type size() const'
//: o 'const_reference front() const'
//: o 'const_reference back()  const'
//: o 'emplace_back'
//: o copy-assignment and move-assignment operators
//: o free '==', '!=', '<', '>', '<=', '>=' operators
//: o free 'swap' function (found via ADL with 'std::swap' in the lookup set)
//
///Requirements on 'VALUE'
///-----------------------
// The following term is used to more precisely specify the requirements on
// template parameter types in function-level documentation:
//
//: *equality-comparable*:
//:   The type provides an equality-comparison operator that defines an
//:   equivalence relationship and is both reflexive and transitive.
//
///'VALUE' and 'CONTAINER::value_type'
///- - - - - - - - - - - - - - - - - -
// When the 'CONTAINER' template parameter is omitted the 'VALUE' template
// parameter specifies the 'value_type' of 'bsl::vector', the default container
// type.  The 'VALUE' template has no other role.
//
// For C++17 and later, the behavior is undefined unless:
//..
//  true == bsl::is_same<VALUE, typename CONTAINER::value_type>::value
//..
// Prior to C++17, 'CONTAINER::value_type' determines the contained value type
// and 'VALUE' is simply ignored.  The resulting code may work with instances
// of 'VALUE' (e.g., 'VALUE' is convertible to 'CONTAINER::value_type') or not
// (compiler errors).
//
///Memory Allocation
///-----------------
// The type supplied as 'ALLOCATOR' template parameter in some of 'queue'
// constructors determines how the held container (of parameterized
// 'CONTAINER') will allocate memory.  A 'queue' supports allocators meeting
// the requirements of the C++11 standard [allocator.requirements] as long as
// the held container does.  In addition it supports scoped-allocators derived
// from the 'bslma::Allocator' memory allocation protocol.  Clients intending
// to use 'bslma' style allocators should use 'bsl::allocator' as the
// 'ALLOCATOR' template parameter, providing a C++11 standard-compatible
// adapter for a 'bslma::Allocator' object.
//
///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: Messages Queue
///- - - - - - - - - - - - -
// In this example, we will use the 'bsl::queue' container adapter to implement
// a message processor in a server program that receives and displays messages
// from clients.
//
// Suppose we want to write a server program that has two threads: one thread
// (receiving thread) receives messages from clients, passing them to a message
// processor; the other thread (processing thread) runs the message processor,
// printing the messages to the console in the same order as they were
// received.  To accomplish this task, we can use 'bsl::queue' in the message
// processor to buffer received, but as yet unprinted, messages.  The message
// processor pushes newly received messages onto the queue in the receiving
// thread, and pops them off the queue in the processing thread.
//
// First, we define a 'Message' type:
//..
//  struct Message {
//      int         d_msgId;  // message identifier given by client
//      const char *d_msg_p;  // message content (C-style string, not owned)
//  };
//..
// Then, we define the class 'MessageProcessor', which provides methods to
// receive and process messages:
//..
//  class MessageProcessor {
//      // This class receives and processes messages from clients.
//..
// Here, we define a private data member of 'bsl::queue<Message>' type, which
// is an instantiation of 'bsl::queue' that uses 'Message' for its 'VALUE'
// (template parameter) type and (by default) 'bsl::deque<Message>' for its
// 'CONTAINER' (template parameter) type:
//..
//      // DATA
//      bsl::queue<Message> d_msgQueue;  // queue holding received but
//                                       // unprocessed messages
//      // ...
//
//    public:
//      // CREATORS
//      explicit MessageProcessor(bslma::Allocator *basicAllocator = 0);
//          // Create a message processor object.  Optionally specify a
//          // 'basicAllocator' used to supply memory.  If 'basicAllocator' is
//          // 0, the currently installed default allocator is used.
//
//      // MANIPULATORS
//      void receiveMessage(const Message &message);
//          // Enqueue the specified 'message' onto this message processor.
//
//      void processMessages(int verbose);
//          // Dequeue all messages currently contained by this processor,
//          // and print them to the console if the specified 'verbose' flag
//          // is not 0.
//  };
//..
// Next, we implement the 'MessageProcessor' constructor:
//..
//  MessageProcessor::MessageProcessor(bslma::Allocator *basicAllocator)
//  : d_msgQueue(basicAllocator)
//  {
//  }
//..
// Notice that we pass to the contained 'd_msgQueue' object the
// 'bslma::Allocator*' supplied to the 'MessageProcessor' at construction.
//
// Now, we implement the 'receiveMessage' method, which pushes the given
// message onto the queue object:
//..
//  void MessageProcessor::receiveMessage(const Message &message)
//  {
//      // ... (some synchronization)
//
//      d_msgQueue.push(message);
//
//      // ...
//  }
//..
// Finally, we implement the 'processMessages' method, which pops all messages
// off the queue object:
//..
//  void MessageProcessor::processMessages(int verbose)
//  {
//      // ... (some synchronization)
//
//      while (!d_msgQueue.empty()) {
//          const Message& message = d_msgQueue.front();
//          if (verbose) {
//              printf("Msg %d: %s\n", message.d_msgId, message.d_msg_p);
//          }
//          d_msgQueue.pop();
//      }
//
//      // ...
//  }
//..
// Note that the sequence of messages popped from the queue will be in exactly
// the same order in which they were pushed, due to the first-in-first-out
// property of the queue.

#include <bslscm_version.h>

#include <bslstl_compare.h>
#include <bslstl_deque.h>

#include <bslalg_swaputil.h>

#include <bslma_isstdallocator.h>
#include <bslma_usesbslmaallocator.h>

#include <bslmf_assert.h>
#include <bslmf_enableif.h>
#include <bslmf_isconvertible.h>
#include <bslmf_issame.h>
#include <bslmf_movableref.h>
#include <bslmf_nestedtraitdeclaration.h>
#include <bslmf_usesallocator.h>
#include <bslmf_util.h>    // 'forward(V)'

#include <bsls_compilerfeatures.h>
#include <bsls_libraryfeatures.h>
#include <bsls_keyword.h>
#include <bsls_platform.h>
#include <bsls_util.h>     // 'forward<T>(V)'

#if BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
// Include version that can be compiled with C++03
// Generated on Thu Oct 21 10:11:37 2021
// Command line: sim_cpp11_features.pl bslstl_queue.h
# define COMPILING_BSLSTL_QUEUE_H
# include <bslstl_queue_cpp03.h>
# undef COMPILING_BSLSTL_QUEUE_H
#else

namespace bsl {

                             // ===========
                             // class queue
                             // ===========

template <class VALUE, class CONTAINER = deque<VALUE> >
class queue {
    // This class is a value-semantic class template, having a container of the
    // parameterized 'CONTAINER' type that holds elements of the parameterized
    // 'VALUE' type, to provide a first-in-first-out queue data structure.  The
    // container object held by a 'queue' class object is referenced as 'c' in
    // the following function-level documentation.

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY
    // STATIC CHECK: Type mismatch is UB per C++17
    BSLMF_ASSERT((is_same<VALUE, typename CONTAINER::value_type>::value));
#endif

    // FRIENDS
    template <class VALUE2, class CONTAINER2>
    friend bool operator==(const queue<VALUE2, CONTAINER2>&,
                           const queue<VALUE2, CONTAINER2>&);

    template <class VALUE2, class CONTAINER2>
    friend bool operator!=(const queue<VALUE2, CONTAINER2>&,
                           const queue<VALUE2, CONTAINER2>&);

    template <class VALUE2, class CONTAINER2>
    friend bool operator< (const queue<VALUE2, CONTAINER2>&,
                           const queue<VALUE2, CONTAINER2>&);

    template <class VALUE2, class CONTAINER2>
    friend bool operator> (const queue<VALUE2, CONTAINER2>&,
                           const queue<VALUE2, CONTAINER2>&);

    template <class VALUE2, class CONTAINER2>
    friend bool operator<=(const queue<VALUE2, CONTAINER2>&,
                           const queue<VALUE2, CONTAINER2>&);

    template <class VALUE2, class CONTAINER2>
    friend bool operator>=(const queue<VALUE2, CONTAINER2>&,
                           const queue<VALUE2, CONTAINER2>&);

#if defined BSLS_COMPILERFEATURES_SUPPORT_THREE_WAY_COMPARISON \
 && defined BSLS_LIBRARYFEATURES_HAS_CPP20_CONCEPTS
    template <class VALUE2, three_way_comparable CONTAINER2>
    friend compare_three_way_result_t<CONTAINER2>
    operator<=>(const queue<VALUE2, CONTAINER2>&,
                const queue<VALUE2, CONTAINER2>&);
#endif

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
    CONTAINER c;  // Contains the elements of this queue.
                  // 'protected' and named ('c') per the C++11 standard.

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION_IF(
        queue,
        BloombergLP::bslma::UsesBslmaAllocator,
        BloombergLP::bslma::UsesBslmaAllocator<container_type>::value);

    // CREATORS
    explicit queue();
        // Create an empty queue having a container of the parameterized
        // 'CONTAINER' type.

    queue(const queue& original);
        // Create a queue having the value of the specified 'original'.

    queue(BloombergLP::bslmf::MovableRef<queue> container);
        // Create a queue having the value of the specified 'original'.  The
        // allocator associated with 'original' (if any) is propagated for use
        // in the new queue.  'original' is left in valid but unspecified
        // state.

    explicit queue(const CONTAINER& container);
        // Create a queue having the specified 'container' that holds elements
        // of the parameterized 'VALUE' type.

    explicit queue(BloombergLP::bslmf::MovableRef<CONTAINER> container);
        // Create a queue having the same sequence of values as the specified
        // 'container'.  The allocator associated with 'container' (if any) is
        // propagated for use in the new queue.  'container' is left in valid
        // but unspecified state.

    template <class ALLOCATOR>
    explicit
    queue(const ALLOCATOR& basicAllocator,
          typename enable_if<bsl::uses_allocator<CONTAINER, ALLOCATOR>::value,
                             ALLOCATOR>::type * = 0);
        // Create an empty queue.  This queue object uses the specified
        // 'basicAllocator' to supply memory.  Note that the 'ALLOCATOR'
        // parameter type has to be convertible to the allocator of the
        // 'CONTAINER' parameter type, 'CONTAINER::allocator_type'; otherwise,
        // this constructor is disabled.

    template <class ALLOCATOR>
    queue(const CONTAINER& container,
          const ALLOCATOR& basicAllocator,
          typename enable_if<bsl::uses_allocator<CONTAINER, ALLOCATOR>::value,
                             ALLOCATOR>::type * = 0);
        // Create a queue having the same sequence of values as the specified
        // 'container'.  The queue object uses the specified 'basicAllocator'
        // to obtain memory.  Note that the 'ALLOCATOR' parameter type has to
        // be convertible to the allocator of the 'CONTAINER' parameter type,
        // 'CONTAINER::allocator_type'; otherwise, this constructor is
        // disabled.

    template <class ALLOCATOR>
    queue(const queue&     original,
          const ALLOCATOR& basicAllocator,
          typename enable_if<bsl::uses_allocator<CONTAINER, ALLOCATOR>::value,
                             ALLOCATOR>::type * = 0);
        // Create a queue having the value of the specified 'original' that
        // will use the specified 'basicAllocator' to supply memory.  Note that
        // the 'ALLOCATOR' parameter type has to be convertible to the
        // allocator of the 'CONTAINER' parameter type,
        // 'CONTAINER::allocator_type'.  Otherwise this constructor is
        // disabled.

    template <class ALLOCATOR>
    queue(BloombergLP::bslmf::MovableRef<CONTAINER> container,
          const ALLOCATOR&                          basicAllocator,
          typename enable_if<bsl::uses_allocator<CONTAINER, ALLOCATOR>::value,
                             ALLOCATOR>::type * = 0);
        // Create a queue whose underlying container has the value of the
        // specified 'container' (on entry) and uses 'basicAllocator' to supply
        // memory.  The allocated-extended move constructor of 'CONTAINER' is
        // used to create the new queue.  'container' is left in a valid but
        // unspecified state.  Note that a 'bslma::Allocator *' can be supplied
        // for 'basicAllocator' if the (template parameter) 'ALLOCATOR' is
        // 'bsl::allocator' (the default).  Also note that this method assumes
        // that 'CONTAINER' has a move constructor.  Also note that if
        // 'CONTAINER::allocator_type' does not exist, this constructor is
        // disabled.

    template <class ALLOCATOR>
    queue(BloombergLP::bslmf::MovableRef<queue> original,
          const ALLOCATOR&                      basicAllocator,
          typename enable_if<bsl::uses_allocator<CONTAINER, ALLOCATOR>::value,
                             ALLOCATOR>::type * = 0);
        // Create a queue having the value of the specified 'original' (on
        // entry), that uses 'basicAllocator' to supply memory.  The
        // allocator-extended move constructor of 'CONTAINER' is used to create
        // the new queue.  'original' is left in a valid but unspecified state.
        // Note that a 'bslma::Allocator *' can be supplied for
        // 'basicAllocator' if the (template parameter) 'ALLOCATOR' is
        // 'bsl::allocator' (the default).  Also note that this method assumes
        // that 'CONTAINER' has a move constructor.  Also note that if
        // 'CONTAINER::allocator_type' does not exist, this constructor is
        // disabled.

    // MANIPULATORS
    queue& operator=(const queue& rhs);
        // Assign to this queue the value of the specified 'rhs', and return a
        // reference providing modifiable access to this queue.

    queue& operator=(BloombergLP::bslmf::MovableRef<queue> rhs);
        // Assign to this queue the value as the specified 'rhs' and return a
        // reference providing modifiable access to this queue.  The
        // move-assignment operator of 'CONTAINER' is used to set the value of
        // this queue.  'rhs' is left in a valid but unspecified state, and if
        // an exception is thrown, '*this' is left in a valid but unspecified
        // state.

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
    template <class... Args>
    reference emplace(Args&&... args);
        // Push onto this queue a newly created 'value_type' object constructed
        // by forwarding 'get_allocator()' (if required) and the specified
        // (variable number of) 'args' to the corresponding constructor of
        // 'value_type'.  Return a reference providing modifiable access to the
        // inserted element.

#endif

    void push(const value_type& value);
        // Push onto the back of this queue a 'value_type' object having the
        // specified 'value'.

    void push(BloombergLP::bslmf::MovableRef<value_type> value);
        // Push onto the back of this queue a 'value_type' object having the
        // value of the specified 'value' (on entry) by moving the
        // contents of 'value' to the new object on this queue.  'value' is
        // left in a valid but unspecified state.

    void pop();
        // Remove the front (the earliest pushed) element from this 'queue'
        // object.

    void swap(queue& other) BSLS_KEYWORD_NOEXCEPT_SPECIFICATION(
                                  bsl::is_nothrow_swappable<CONTAINER>::value);
        // Efficiently exchange the value of this object with the value of the
        // specified 'other' object.  In effect, performs
        // 'using bsl::swap; swap(c, other.c);'.

    reference front();
        // Return a reference providing modifiable access to the front (the
        // earliest pushed) element from this 'queue' object.

    reference back();
        // Return a reference providing modifiable access to the back (the
        // latest pushed) element of this 'queue' object.

    // ACCESSORS
    bool empty() const;
        // Return 'true' if this 'queue' object contains no elements, and
        // 'false' otherwise.  In effect, performs 'return c.empty();'.

    size_type size() const;
        // Return the number of elements in this queue.  In effect, performs
        // 'return c.size();'.

    const_reference front() const;
        // Return the immutable front (the earliest pushed) element from this
        // 'queue' object.  In effect, performs 'c.front()'.

    const_reference back() const;
        // Return the immutable back (the latest pushed) element from this
        // 'queue' object.  In effect, performs 'c.back()'.
};

#ifdef BSLS_COMPILERFEATURES_SUPPORT_CTAD
// CLASS TEMPLATE DEDUCTION GUIDES

template<class CONTAINER,
         class = bsl::enable_if_t<!bsl::IsStdAllocator_v<CONTAINER>>
        >
queue(CONTAINER) -> queue<typename CONTAINER::value_type, CONTAINER>;
    // Deduce the template parameters 'VALUE' and 'CONTAINER' from the
    // parameters supplied to the constructor of 'queue'.  This deduction guide
    // does not participate if the parameter meets the requirements for a
    // standard allocator.

template<
    class CONTAINER,
    class ALLOCATOR,
    class = bsl::enable_if_t<bsl::uses_allocator_v<CONTAINER, ALLOCATOR>>
    >
queue(CONTAINER, ALLOCATOR) -> queue<typename CONTAINER::value_type, CONTAINER>;
    // Deduce the template parameters 'VALUE' and 'CONTAINER' from the
    // parameters supplied to the constructor of 'queue'.  This deduction
    // guide does not participate unless the supplied allocator is convertible
    // to the underlying container's 'allocator_type'.
#endif

// FREE OPERATORS
template <class VALUE, class CONTAINER>
bool operator==(const queue<VALUE, CONTAINER>& lhs,
                const queue<VALUE, CONTAINER>& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects have the same
    // value, and 'false' otherwise.  Two 'queue' objects 'lhs' and 'rhs' have
    // the same value if they have the same number of elements, and each
    // element in the ordered sequence of elements of 'lhs' has the same value
    // as the corresponding element in the ordered sequence of elements of
    // 'rhs'.  This method requires that the (template parameter) type 'VALUE'
    // be 'equality-comparable' (see {Requirements on 'VALUE'}).

template <class VALUE, class CONTAINER>
bool operator!=(const queue<VALUE, CONTAINER>& lhs,
                const queue<VALUE, CONTAINER>& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects do not have the
    // same value, and 'false' otherwise.  Two 'queue' objects 'lhs' and 'rhs'
    // do not have the same value if they do not have the same number of
    // elements, or some element in the ordered sequence of elements of 'lhs'
    // does not have the same value as the corresponding element in the ordered
    // sequence of elements of 'rhs'.  This method requires that the (template
    // parameter) type 'VALUE' be 'equality-comparable' (see {Requirements on
    // 'VALUE'}).

template <class VALUE, class CONTAINER>
bool operator< (const queue<VALUE, CONTAINER>& lhs,
                const queue<VALUE, CONTAINER>& rhs);
    // Return 'true' if the value of the specified 'lhs' queue is
    // lexicographically less than that of the specified 'rhs' queue, and
    // 'false' otherwise.  Given iterators 'i' and 'j' over the respective
    // sequences '[lhs.begin() .. lhs.end())' and '[rhs.begin() .. rhs.end())',
    // the value of queue 'lhs' is lexicographically less than that of queue
    // 'rhs' if 'true == *i < *j' for the first pair of corresponding iterator
    // positions where '*i < *j' and '*j < *i' are not both 'false'.  If no
    // such corresponding iterator position exists, the value of 'lhs' is
    // lexicographically less than that of 'rhs' if 'lhs.size() < rhs.size()'.
    // This method requires that 'operator<', inducing a total order, be
    // defined for 'value_type'.

template <class VALUE, class CONTAINER>
bool operator> (const queue<VALUE, CONTAINER>& lhs,
                const queue<VALUE, CONTAINER>& rhs);
    // Return 'true' if the value of the specified 'lhs' queue is
    // lexicographically greater than that of the specified 'rhs' queue, and
    // 'false' otherwise.  The value of queue 'lhs' is lexicographically
    // greater than that of queue 'rhs' if 'rhs' is lexicographically less than
    // 'lhs' (see 'operator<').  This method requires that 'operator<',
    // inducing a total order, be defined for 'value_type'.  Note that this
    // operator returns 'rhs < lhs'.

template <class VALUE, class CONTAINER>
bool operator<=(const queue<VALUE, CONTAINER>& lhs,
                const queue<VALUE, CONTAINER>& rhs);
    // Return 'true' if the value of the specified 'lhs' queue is
    // lexicographically less than or equal to that of the specified 'rhs'
    // queue, and 'false' otherwise.  The value of queue 'lhs' is
    // lexicographically less than or equal to that of queue 'rhs' if 'rhs' is
    // not lexicographically less than 'lhs' (see 'operator<').  This method
    // requires that 'operator<', inducing a total order, be defined for
    // 'value_type'.  Note that this operator returns '!(rhs < lhs)'.

template <class VALUE, class CONTAINER>
bool operator>=(const queue<VALUE, CONTAINER>& lhs,
                const queue<VALUE, CONTAINER>& rhs);
    // Return 'true' if the value of the specified 'lhs' queue is
    // lexicographically greater than or equal to that of the specified 'rhs'
    // queue, and 'false' otherwise.  The value of queue 'lhs' is
    // lexicographically greater than or equal to that of queue 'rhs' if 'lhs'
    // is not lexicographically less than 'rhs' (see 'operator<').  This method
    // requires that 'operator<', inducing a total order, be defined for
    // 'value_type'.  Note that this operator returns '!(lhs < rhs)'.

// FREE FUNCTIONS
template <class VALUE, class CONTAINER>
void swap(queue<VALUE, CONTAINER>& lhs,
          queue<VALUE, CONTAINER>& rhs)
                                    BSLS_KEYWORD_NOEXCEPT_SPECIFICATION(false);
    // Swap the value of the specified 'lhs' queue with the value of the
    // specified 'rhs' queue.

// ============================================================================
//                  TEMPLATE AND INLINE FUNCTION DEFINITIONS
// ============================================================================

                              // -----------
                              // class queue
                              // -----------

// CREATORS
template <class VALUE, class CONTAINER>
inline
queue<VALUE, CONTAINER>::queue()
{
}

template <class VALUE, class CONTAINER>
inline
queue<VALUE, CONTAINER>::queue(const queue& original)
: c(original.c)
{
}

template <class VALUE, class CONTAINER>
inline
queue<VALUE, CONTAINER>::queue(BloombergLP::bslmf::MovableRef<queue> original)
: c(MoveUtil::move(MoveUtil::access(original).c))
{
}

template <class VALUE, class CONTAINER>
inline
queue<VALUE, CONTAINER>::queue(const CONTAINER& container)
: c(container)
{
}

template <class VALUE, class CONTAINER>
template <class ALLOCATOR>
inline
queue<VALUE, CONTAINER>::queue(
           const ALLOCATOR& basicAllocator,
           typename enable_if<bsl::uses_allocator<CONTAINER, ALLOCATOR>::value,
                              ALLOCATOR>::type *)
: c(basicAllocator)
{
}

template <class VALUE, class CONTAINER>
template <class ALLOCATOR>
inline
queue<VALUE, CONTAINER>::queue(
           const CONTAINER& container,
           const ALLOCATOR& basicAllocator,
           typename enable_if<bsl::uses_allocator<CONTAINER, ALLOCATOR>::value,
                              ALLOCATOR>::type *)
: c(container, basicAllocator)
{
}

template <class VALUE, class CONTAINER>
template <class ALLOCATOR>
inline
queue<VALUE, CONTAINER>::queue(
           const queue&     queue,
           const ALLOCATOR& basicAllocator,
           typename enable_if<bsl::uses_allocator<CONTAINER, ALLOCATOR>::value,
                              ALLOCATOR>::type *)
: c(queue.c, basicAllocator)
{
}

template <class VALUE, class CONTAINER>
inline
queue<VALUE, CONTAINER>::queue(BloombergLP::bslmf::MovableRef<CONTAINER>
                                                                     container)
: c(MoveUtil::move(container))
{
}

template <class VALUE, class CONTAINER>
template <class ALLOCATOR>
inline
queue<VALUE, CONTAINER>::queue(
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
queue<VALUE, CONTAINER>::queue(
           BloombergLP::bslmf::MovableRef<queue> original,
           const ALLOCATOR&                      basicAllocator,
           typename enable_if<bsl::uses_allocator<CONTAINER, ALLOCATOR>::value,
                              ALLOCATOR>::type *)
: c(MoveUtil::move(MoveUtil::access(original).c), basicAllocator)
{
}

// MANIPULATORS
template <class VALUE, class CONTAINER>
inline
queue<VALUE, CONTAINER>& queue<VALUE, CONTAINER>::operator=(const queue& rhs)
{
    c = rhs.c;
    return *this;
}

template <class VALUE, class CONTAINER>
inline
queue<VALUE, CONTAINER>& queue<VALUE, CONTAINER>::operator=(
                                     BloombergLP::bslmf::MovableRef<queue> rhs)
{
    c = MoveUtil::move(MoveUtil::access(rhs).c);
    return *this;
}

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
template <class VALUE, class CONTAINER>
template <class... Args>
inline
typename queue<VALUE, CONTAINER>::reference
queue<VALUE, CONTAINER>::emplace(Args&&... args)
{
    c.emplace_back(BSLS_COMPILERFEATURES_FORWARD(Args,args)...);
    return back();
}
#endif

template <class VALUE, class CONTAINER>
inline
void queue<VALUE, CONTAINER>::push(const value_type& value)
{
    c.push_back(value);
}

template <class VALUE, class CONTAINER>
inline
void queue<VALUE, CONTAINER>::push(BloombergLP::bslmf::MovableRef<value_type>
                                                                         value)
{
    c.push_back(MoveUtil::move(value));
}

template <class VALUE, class CONTAINER>
inline
void queue<VALUE, CONTAINER>::pop()
{
    c.pop_front();
}

template <class VALUE, class CONTAINER>
inline
void queue<VALUE, CONTAINER>::swap(queue& other)
    BSLS_KEYWORD_NOEXCEPT_SPECIFICATION(
                                   bsl::is_nothrow_swappable<CONTAINER>::value)
{
    BloombergLP::bslalg::SwapUtil::swap(&c, &other.c);
}

// ACCESSORS
template <class VALUE, class CONTAINER>
inline
bool queue<VALUE, CONTAINER>::empty() const
{
    return c.empty();
}

template <class VALUE, class CONTAINER>
inline
typename queue<VALUE, CONTAINER>::size_type
queue<VALUE, CONTAINER>::size() const
{
    return c.size();
}

template <class VALUE, class CONTAINER>
inline
typename queue<VALUE, CONTAINER>::reference
queue<VALUE, CONTAINER>::front()
{
    return c.front();
}

template <class VALUE, class CONTAINER>
inline
typename queue<VALUE, CONTAINER>::const_reference
queue<VALUE, CONTAINER>::front() const
{
    return c.front();
}

template <class VALUE, class CONTAINER>
inline
typename queue<VALUE, CONTAINER>::reference
queue<VALUE, CONTAINER>::back()
{
    return c.back();
}

template <class VALUE, class CONTAINER>
inline
typename queue<VALUE, CONTAINER>::const_reference
queue<VALUE, CONTAINER>::back() const
{
    return c.back();
}

// FREE OPERATORS
template <class VALUE, class CONTAINER>
inline
bool operator==(const queue<VALUE, CONTAINER>& lhs,
                const queue<VALUE, CONTAINER>& rhs)
{
    return lhs.c == rhs.c;
}

template <class VALUE, class CONTAINER>
inline
bool operator!=(const queue<VALUE, CONTAINER>& lhs,
                const queue<VALUE, CONTAINER>& rhs)
{
    return lhs.c != rhs.c;
}

template <class VALUE, class CONTAINER>
inline
bool operator< (const queue<VALUE, CONTAINER>& lhs,
                const queue<VALUE, CONTAINER>& rhs)
{
    return lhs.c < rhs.c;
}

template <class VALUE, class CONTAINER>
inline
bool operator> (const queue<VALUE, CONTAINER>& lhs,
                const queue<VALUE, CONTAINER>& rhs)
{
    return lhs.c > rhs.c;
}

template <class VALUE, class CONTAINER>
inline
bool operator<=(const queue<VALUE, CONTAINER>& lhs,
                const queue<VALUE, CONTAINER>& rhs)
{
    return lhs.c <= rhs.c;
}

template <class VALUE, class CONTAINER>
inline
bool operator>=(const queue<VALUE, CONTAINER>& lhs,
                const queue<VALUE, CONTAINER>& rhs)
{
    return lhs.c >= rhs.c;
}

#if defined BSLS_COMPILERFEATURES_SUPPORT_THREE_WAY_COMPARISON \
 && defined BSLS_LIBRARYFEATURES_HAS_CPP20_CONCEPTS
template <class VALUE, three_way_comparable CONTAINER>
inline compare_three_way_result_t<CONTAINER>
operator<=>(const queue<VALUE, CONTAINER>& lhs,
            const queue<VALUE, CONTAINER>& rhs)
{
    return lhs.c <=> rhs.c;
}
#endif

// FREE FUNCTIONS
template <class VALUE, class CONTAINER>
inline
void swap(queue<VALUE, CONTAINER>& lhs,
          queue<VALUE, CONTAINER>& rhs)
                                     BSLS_KEYWORD_NOEXCEPT_SPECIFICATION(false)
{
    lhs.swap(rhs);
}

}  // close namespace bsl

#endif // End C++11 code

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
