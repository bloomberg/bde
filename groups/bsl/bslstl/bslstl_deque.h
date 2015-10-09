// bslstl_deque.h                                                     -*-C++-*-
#ifndef INCLUDED_BSLSTL_DEQUE
#define INCLUDED_BSLSTL_DEQUE

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide an STL-compliant deque class.
//
//@CLASSES:
//  bslstl_Deque: standard-compliant 'bsl::deque' implementation
//
//@SEE_ALSO: bslstl_vector, bsl+stlhdrs
//
//@DESCRIPTION: This component is for internal use only.  Please include
// '<bsl_deque.h>' instead and use 'bsl::deque' directly.  This component
// implements a dynamic double-ended array class that supports the
// 'bslma::Allocator' model and is suitable for use as an implementation of the
// 'bsl::deque' class template.
//
///Exceptional Behavior
///--------------------
// Since this component is below the BSL STL, we centralize all the exceptional
// behavior into a 'bslstl::StdExceptUtil' class, which has a dual purpose:
//
//: o Remove the dependency of this header on the '<exception>' header, so that
//:   this implementation can offer an exception handler with the native
//:   exceptions, and so that all the C-strings may be defined in a single
//:   library ('bsl') and not in all the translation units including this
//:   header.
//:
//: o Allow installation of exception handlers at a higher level to throw BSL
//:   STL exceptions (which differ from the native exceptions) and thus
//:   establish a full standard compliance for this component when used as
//:   'bsl::deque' in the BSL STL.
//
///Usage
///-----
// In this section we show intended usage of this component.
//
///Example 1: Using a 'deque' to Implement a Laundry Queue
///- - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we want to define a class to maintain a process queue of names of
// customers who are dropping off their laundry at a drop-off laundry service.
// We can accomplish this by defining a new class characterizing a
// laundry-process queue that uses 'bsl::deque' in its implementation.
//
// The process queue provides two methods, 'push' and 'expeditedPush', for
// inserting names of customers onto the queue.  When calling the 'push'
// method, the customer's name will be inserted at the end of the queue -- his
// laundry will be done after the laundry of customers previously on the queue.
// The 'expeditedPush' method is reserved for customers who have bribed the
// merchant for expedited service.  When calling the 'expeditedPush' method,
// the customer's name will be inserted onto the front of the queue -- his
// laundry will be done before customers previously on the queue.
//
// When the workers are ready to do some laundry, they call the 'next' method
// of the queue, which returns the name of the customer whose laundry is to be
// done next.  For brevity of the usage example, we do not show how customers
// are track while or after their laundry is being done.
//
// In addition, the laundry queue also provides the 'find' method, which
// returns a 'bool' to indicate whether a given customer is still in the queue.
//
// First, we declare a class 'LaundryQueue' based on a deque, to store names of
// customers at a drop-off laundry:
//..
//  class LaundryQueue {
//      // This 'class' keeps track of customers enqueued to have their laundry
//      // done by a laundromat.
//
//      // DATA
//      bsl::deque<bsl::string> d_queue;
//
//    public:
//      // CREATORS
//      LaundryQueue(bslma::Allocator *basicAllocator = 0);
//          // Create a 'LaundryQueue' object using the specified
//          // 'basicAllocator'.  If 'basicAllocator' is not provided, use the
//          // default allocator.
//
//      // MANIPULATORS
//      void push(const bsl::string& customerName);
//          // Add the specified 'customerName' to the back of the laundry
//          // queue.
//
//      void expeditedPush(const bsl::string& customerName);
//          // Add the specified 'customerName' to the laundry queue at the
//          // front.
//
//      bsl::string next();
//          // Return the name from the front of the queue, removing it from
//          // the queue.  If the queue is empty, return '(* empty *)' which is
//          // not a valid name for a customer.
//
//      // ACCESSORS
//      bool find(const bsl::string& customerName);
//          // Return 'true' if 'customerName' is in the queue, and 'false'
//          // otherwise.
//  };
//..
// Then, we define the implementation of the methods of 'LaundryQueue'
//..
// CREATORS
//  LaundryQueue::LaundryQueue(bslma::Allocator *basicAllocator)
//  : d_queue(basicAllocator)
//  {
//      // Note that the allocator is propagated to the underlying 'deque',
//      // which will use the default allocator is '0 == basicAllocator'.
//  }
//
// MANIPULATORS
//  void LaundryQueue::push(const bsl::string& customerName)
//  {
//      d_queue.push_back(customerName);     // note constant time
//  }
//
//  void LaundryQueue::expeditedPush(const bsl::string& customerName)
//  {
//      d_queue.push_front(customerName);    // note constant time
//  }
//
//  bsl::string LaundryQueue::next()
//  {
//      if (d_queue.empty()) {
//          return "(* empty *)";
//      }
//
//      bsl::string ret = d_queue.front();   // note constant time
//
//      d_queue.pop_front();                 // note constant time
//
//      return ret;
//  }
//
//  // ACCESSORS
//  bool LaundryQueue::find(const bsl::string& customerName)
//  {
//      // Note 'd_queue.empty() || d_queue[0] == d_queue.front()'
//
//      for (size_t i = 0; i < d_queue.size(); ++i) {
//          if (customerName == d_queue[i]) {    // note '[]' is constant time
//              return true;
//          }
//      }
//
//      return false;
//  }
//..


// Prevent 'bslstl' headers from being included directly in 'BSL_OVERRIDES_STD'
// mode.  Doing so is unsupported, and is likely to cause compilation errors.
#if defined(BSL_OVERRIDES_STD) && !defined(BSL_STDHDRS_PROLOGUE_IN_EFFECT)
#error "include <bsl_deque.h> instead of <bslstl_deque.h> in \
BSL_OVERRIDES_STD mode"
#endif

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLSTL_ALLOCATOR
#include <bslstl_allocator.h>
#endif

#ifndef INCLUDED_BSLSTL_ITERATOR
#include <bslstl_iterator.h>
#endif

#ifndef INCLUDED_BSLSTL_RANDOMACCESSITERATOR
#include <bslstl_randomaccessiterator.h>
#endif

#ifndef INCLUDED_BSLSTL_STDEXCEPTUTIL
#include <bslstl_stdexceptutil.h>
#endif

#ifndef INCLUDED_BSLALG_CONTAINERBASE
#include <bslalg_containerbase.h>
#endif

#ifndef INCLUDED_BSLALG_DEQUEIMPUTIL
#include <bslalg_dequeimputil.h>
#endif

#ifndef INCLUDED_BSLALG_DEQUEITERATOR
#include <bslalg_dequeiterator.h>
#endif

#ifndef INCLUDED_BSLALG_DEQUEPRIMITIVES
#include <bslalg_dequeprimitives.h>
#endif

#ifndef INCLUDED_BSLALG_RANGECOMPARE
#include <bslalg_rangecompare.h>
#endif

#ifndef INCLUDED_BSLALG_SCALARDESTRUCTIONPRIMITIVES
#include <bslalg_scalardestructionprimitives.h>
#endif

#ifndef INCLUDED_BSLALG_SCALARPRIMITIVES
#include <bslalg_scalarprimitives.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITHASSTLITERATORS
#include <bslalg_typetraithasstliterators.h>
#endif

#ifndef INCLUDED_BSLMF_ASSERT
#include <bslmf_assert.h>
#endif

#ifndef INCLUDED_BSLMF_ISSAME
#include <bslmf_issame.h>
#endif

#ifndef INCLUDED_BSLMF_MATCHANYTYPE
#include <bslmf_matchanytype.h>
#endif

#ifndef INCLUDED_BSLMF_MATCHARITHMETICTYPE
#include <bslmf_matcharithmetictype.h>
#endif

#ifndef INCLUDED_BSLMF_NIL
#include <bslmf_nil.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSLS_PERFORMANCEHINT
#include <bsls_performancehint.h>
#endif

#ifndef INCLUDED_BSLS_UTIL
#include <bsls_util.h>
#endif

#ifndef INCLUDED_CSTRING
#include <cstring>
#define INCLUDED_CSTRING
#endif

#ifndef BDE_DONT_ALLOW_TRANSITIVE_INCLUDES

#ifndef INCLUDED_STDEXCEPT
#include <stdexcept>
#define INCLUDED_STDEXCEPT
#endif

#endif

namespace bsl {

template <class VALUE_TYPE, class ALLOCATOR>
class Deque_BlockCreator;
template <class VALUE_TYPE, class ALLOCATOR>
class Deque_ClearGuard;
template <class VALUE_TYPE, class ALLOCATOR>
class Deque_Guard;

                  // =====================================
                  // struct bsl::Deque_BlockLengthCalcUtil
                  // =====================================

template <class VALUE_TYPE>
struct Deque_BlockLengthCalcUtil {
    // This 'struct' provides a namespace for the calculation of block length
    // (the number of elements per block within a 'deque').  This ensures that
    // each block in the deque can hold at least 16 elements.

    // TYPES
    enum {
        DEFAULT_BLOCK_SIZE = 200,  // number of bytes per block
        BLOCK_LENGTH       = (16 * sizeof(VALUE_TYPE) >= DEFAULT_BLOCK_SIZE)
                             ? 16
                             : (DEFAULT_BLOCK_SIZE / sizeof(VALUE_TYPE))
                                   // number of elements per block
    };
};

                          // ======================
                          // struct bsl::Deque_Util
                          // ======================

struct Deque_Util {
    // This 'struct' provides a namespace to implement the 'swap' member
    // function of 'deque<VALUE_TYPE, ALLOCATOR>'.  This function can be
    // implemented irrespective of the 'VALUE_TYPE' or 'ALLOCATOR' template
    // parameters which is why we implement it in this non-templated,
    // non-inlined utility.

    // CLASS METHODS
    static void move(void *dest, void *src);
        // Assign the value of the specified 'dest' deque to that of the
        // specified 'src' deque, and reset the 'src' deque to a raw state.

    static void swap(void *a, void *b);
        // Exchange the value of the specified 'lhs' deque with that of the
        // specified 'rhs' deque.
};

                        // =====================
                        // class bsl::Deque_Base
                        // =====================

template <class VALUE_TYPE>
class Deque_Base {
    // This class describes the basic layout for a deque class.  It is
    // important that this class has the same layout as the deque class
    // implementation.  It is parameterized by 'VALUE_TYPE' only and implements
    // the portion of 'bsl::deque' that does not need to know about its
    // parameterized 'ALLOCATOR' (in order to generate shorter debug strings).
    // Note that this class must have the same layout as 'Deque_Imp' (see
    // implementation file).

    // PRIVATE TYPES
    enum {
        BLOCK_LENGTH = Deque_BlockLengthCalcUtil<VALUE_TYPE>::BLOCK_LENGTH
    };

    typedef BloombergLP::bslalg::DequeImpUtil<VALUE_TYPE,
                                              BLOCK_LENGTH>      Imp;
    typedef typename Imp::Block                                  Block;
    typedef typename Imp::BlockPtr                               BlockPtr;
    typedef BloombergLP::bslalg::DequeIterator<VALUE_TYPE,
                                               BLOCK_LENGTH>     IteratorImp;
    typedef BloombergLP::
            bslstl::RandomAccessIterator<VALUE_TYPE, IteratorImp>
                                                                 Iterator;

    typedef BloombergLP::
            bslstl::RandomAccessIterator<const VALUE_TYPE, IteratorImp>
                                                                 ConstIterator;

  public:
    // PUBLIC TYPES
    typedef VALUE_TYPE&                             reference;
    typedef const VALUE_TYPE &                      const_reference;
    typedef Iterator                                iterator;
    typedef ConstIterator                           const_iterator;
    typedef std::size_t                             size_type;
    typedef std::ptrdiff_t                          difference_type;
    typedef VALUE_TYPE                              value_type;

    // Even though we're in bslstl namespace, removing 'bsl::' seems to confuse
    // the compiler.

    typedef bsl::reverse_iterator<Iterator>      reverse_iterator;
    typedef bsl::reverse_iterator<ConstIterator> const_reverse_iterator;

  protected:
    // DATA
    BlockPtr    *d_blocks;       // array of pointer to blocks (owned)
    std::size_t  d_blocksLength; // length of d_blocks array
    IteratorImp  d_start;        // iterator to first element
    IteratorImp  d_finish;       // iterator to one past last element

  public:
    // MANIPULATORS

    // *** iterators: ***

    iterator begin();
        // Return an iterator pointing the first element in this modifiable
        // deque (or the past-the-end iterator if this deque is empty).

    iterator end();
        // Return the past-the-end iterator for this modifiable deque.

    reverse_iterator rbegin();
        // Return a reverse iterator pointing the last element in this
        // modifiable deque (or the past-the-end reverse iterator if this deque
        // is empty).

    reverse_iterator rend();
        // Return the past-the-end reverse iterator for this modifiable deque.

    // *** element access: ***

    reference operator[](size_type position);
        // Return a reference to the modifiable element at the specified
        // 'position' in this deque.  The behavior is undefined unless
        // '0 <= position < size()'.

    reference at(size_type position);
        // Return a reference to the modifiable element at the specified
        // 'position'.  Call 'StdExceptUtil::throwOutOfRange' if
        // 'position >= size()'.

    reference front();
        // Return a reference to the modifiable character at the first position
        // in this deque.  The behavior is undefined if this deque is empty.

    reference back();
        // Return a reference to the modifiable character at the last position
        // in this deque.  The behavior is undefined if this deque is empty.
        // Note that the last position is 'size() - 1'.

    // ACCESSORS

    // *** iterators: ***

    const_iterator begin() const;
    const_iterator cbegin() const;
        // Return an iterator pointing to the first element in this
        // non-modifiable deque (or the past-the-end iterator if this deque is
        // empty).

    const_iterator end() const;
    const_iterator cend() const;
        // Return the past-the-end iterator for this non-modifiable deque.

    const_reverse_iterator rbegin() const;
    const_reverse_iterator crbegin() const;
        // Return a reverse iterator pointing the last element in this
        // non-modifiable deque (or the past-the-end reverse iterator if this
        // deque is empty).

    const_reverse_iterator rend() const;
    const_reverse_iterator crend() const;
        // Return the past-the-end reverse iterator for this non-modifiable
        // deque.

    // *** 23.2.1.2 capacity: ***

    size_type size() const;
        // Return the length of this deque.

    size_type capacity() const;
        // Return the sum of the current size plus the minimum number of
        // 'push_front' or 'push_back' operations needed to invalidate
        // iterators in this deque.

    bool empty() const;
        // Return 'true' if this deque has length 0, and 'false' otherwise.

    // *** element access: ***

    const_reference operator[](size_type position) const;
        // Return a reference to the non-modifiable element at the specified
        // 'position' in this deque.  The behavior is undefined unless
        // '0 <= position < size()'.

    const_reference at(size_type position) const;
        // Return a reference to the non-modifiable element at the specified
        // 'position'.  Call 'bslstl::StdExceptUtil::throwOutOfRange' if
        // 'position >= size()'.

    const_reference front() const;
        // Return a reference to the non-modifiable character at the first
        // position in this deque.  The behavior is undefined if this deque is
        // empty.

    const_reference back() const;
        // Return a reference to the non-modifiable character at the last
        // position in this deque.  The behavior is undefined if this deque is
        // empty.  Note that the last position is 'size() - 1'.
};

                        // ================
                        // class bsl::Deque
                        // ================

template <class VALUE_TYPE, class ALLOCATOR = allocator<VALUE_TYPE> >
class deque : public  Deque_Base<VALUE_TYPE>
            , private BloombergLP::bslalg::ContainerBase<ALLOCATOR> {
    // This class template provides an STL-compliant 'deque' that conforms to
    // the 'bslma::Allocator' model.  For the requirements of a deque class,
    // consult the second revision of the ISO/IEC 14882 Programming Language
    // C++ (Working Paper, 2009).  In particular, this implementation offers
    // the general rules that:
    //..
    //   (1) any method that would result in a deque of length larger than
    //       the size returned by 'max_size' calls
    //       'bslstl::StdExceptUtil::throwLengthError', and
    //   (2) a call to the 'at' method that attempts to access a position
    //       outside the valid range of a deque triggers a call to
    //       'bslstl::StdExceptUtil::throwOutOfRange'.
    //..
    // Note that portions of the standard methods are implemented in
    // 'Deque_Base', which is parameterized only 'VALUE_TYPE', in order to
    // generate smaller debug strings.
    //
    // More generally, this class supports an almost complete set of *in-core*
    // *value* *semantic* operations, including copy construction, assignment,
    // equality comparison (but excluding 'ostream' printing since this
    // component is below STL).  A precise operational definition of when two
    // objects have the same value can be found in the description of
    // 'operator==' for the class.  This class is *exception* *neutral* with no
    // guarantee of rollback: if an exception is thrown during the invocation
    // of a method on a pre-existing object, the object is left in a valid
    // state, but its value is undefined.  In addition, the following members
    // offer a full guarantee of rollback: if an exception is thrown during the
    // invocation of 'insert', 'push_front' or 'push_back' on a pre-existing
    // object, the object is left in a valid state and its value is unchanged.
    // In no event is memory leaked.  Finally, *aliasing* (e.g., using all or
    // part of an object as both source and destination) is *not* supported.

    // PRIVATE TYPES
    enum {
        BLOCK_LENGTH = Deque_BlockLengthCalcUtil<VALUE_TYPE>::BLOCK_LENGTH
    };

    typedef Deque_Base<VALUE_TYPE>                             Base;

    typedef BloombergLP::bslalg::ContainerBase<ALLOCATOR>      ContainerBase;

    typedef BloombergLP::bslalg::DequeImpUtil<VALUE_TYPE,
                                             BLOCK_LENGTH>     Imp;
    typedef typename Imp::Block                                Block;
    typedef typename Imp::BlockPtr                             BlockPtr;

    typedef BloombergLP::bslalg::DequeIterator<VALUE_TYPE,
                                              BLOCK_LENGTH>    IteratorImp;

    typedef BloombergLP::
            bslstl::RandomAccessIterator<VALUE_TYPE,
                                        IteratorImp>           Iterator;

    typedef BloombergLP::
            bslstl::RandomAccessIterator<const VALUE_TYPE,
                                        IteratorImp>           ConstIterator;

    typedef Deque_BlockCreator<VALUE_TYPE, ALLOCATOR>          BlockCreator;
    typedef Deque_ClearGuard<VALUE_TYPE, ALLOCATOR>            ClearGuard;
    typedef Deque_Guard<VALUE_TYPE, ALLOCATOR>                 Guard;

    typedef BloombergLP::bslalg::DequePrimitives<VALUE_TYPE,
                                                BLOCK_LENGTH>  DequePrimitives;

    enum RawInit { RAW_INIT  = 0 };
        // Special type (and value) used to create a "raw" deque, which has 0
        // block length, and null start and finish pointers.

  public:
    // PUBLIC TYPES
    typedef typename ALLOCATOR::reference           reference;
    typedef typename ALLOCATOR::const_reference     const_reference;
    typedef Iterator                                iterator;
    typedef ConstIterator                           const_iterator;
    typedef std::size_t                             size_type;
    typedef std::ptrdiff_t                          difference_type;
    typedef VALUE_TYPE                              value_type;
    typedef ALLOCATOR                               allocator_type;
    typedef typename ALLOCATOR::pointer             pointer;
    typedef typename ALLOCATOR::const_pointer       const_pointer;

    // Even though we're in bslstl namespace, removing 'bsl::' seems to confuse
    // the compiler.

    typedef bsl::reverse_iterator<Iterator>      reverse_iterator;
    typedef bsl::reverse_iterator<ConstIterator> const_reverse_iterator;

  private:
    // ASSERTIONS

    BSLMF_ASSERT((is_same<reference, typename Base::reference>::value));
    BSLMF_ASSERT((is_same<const_reference,
                  typename Base::const_reference>::value));
        // This need not necessarily be true as per the C++ standard, but is a
        // safe assumption for this implementation and allows to implement the
        // element access within the 'Base' type (that is parameterized by
        // 'VALUE_TYPE' only).

    // PRIVATE CREATORS
    deque(RawInit, const allocator_type& alloc);
        // Constructs a "raw" deque.  This deque obeys the raw deque invariants
        // and is destructible.  Postcondition: 'd_blocks == 0'
        // 'd_blocksLength == 0', 'd_start' and 'd_finish' are singular
        // iterators (have null internal pointers).  The constructed deque
        // contains no allocated storage.  The purpose of a raw deque is to
        // provide an exception-safe repository for intermediate calculations.

    // PRIVATE MANIPULATORS
    template <class INPUT_ITER>
    size_type privateAppend(INPUT_ITER                     first,
                            INPUT_ITER                     last,
                            std::input_iterator_tag);
    template <class INPUT_ITER>
    size_type privateAppend(INPUT_ITER                     first,
                            INPUT_ITER                     last,
                            std::random_access_iterator_tag);
        // Append the elements in the range specified as '[first .. last)' to
        // this deque, and return the number of elements appended.  The third
        // argument is used for overload resolution.  The behavior is undefined
        // unless 'first' and 'last' refer to a sequence of valid values where
        // 'first' is at a position at or before 'last'.

    void privateAppendRaw(size_type numElements, const VALUE_TYPE& value);
        // Append the specified 'numElements' copies of the specified 'value'
        // to this deque.

    void privateInit(size_type numElements);
        // Initialize 'd_start' and 'd_finish' for eventual insertion of the
        // specified 'numElements' elements.  After this call, '*this' object
        // is fully constructed with memory allocated for 'numElements' and
        // with all member variables obeying the class invariants, and the
        // deque has length zero.  The behavior is undefined unless '*this'
        // /object is in a "raw" state before this call.  Note that this
        // function will throw if the allocator throws.  It must never be
        // called while constructing the '*this' object (although a constructor
        // may call it for a temporary object).

    template <class INTEGER_TYPE>
    void privateInsertDispatch(
                           const_iterator                          position,
                           INTEGER_TYPE                            numElements,
                           INTEGER_TYPE                            value,
                           BloombergLP::bslmf::MatchArithmeticType ,
                           BloombergLP::bslmf::Nil                 );
        // Insert the specified 'numElements' copies of the specified 'value'
        // into this deque at the specified 'position'.  This overload matches
        // 'privateInsert' when the second and third arguments are of the same
        // type which happens to be an integral type.  The fourth and fifth
        // arguments are used for overload resolution only.

    template <class INPUT_ITER>
    void privateInsertDispatch(const_iterator                   position,
                               INPUT_ITER                       first,
                               INPUT_ITER                       last,
                               BloombergLP::bslmf::MatchAnyType ,
                               BloombergLP::bslmf::MatchAnyType );
        // Insert the elements in the range specified as '[first .. last)' into
        // this deque at the specified 'position'.  The third and fourth
        // arguments are used for overload resolution only, so that this
        // function is not called if 'first' and 'last' are of integral type.
        // The behavior is undefined unless 'first' and 'last' refer to a
        // sequence of valid values where 'first' is at a position at or before
        // 'last'.

    template <class INPUT_ITER>
    void privateInsert(const_iterator            position,
                       INPUT_ITER                first,
                       INPUT_ITER                last,
                       std::input_iterator_tag);
        // Specialized insertion for input iterators.

    template <class INPUT_ITER>
    void privateInsert(const_iterator                 position,
                       INPUT_ITER                     first,
                       INPUT_ITER                     last,
                       std::random_access_iterator_tag);
        // Specialized insertion for forward, bidirectional, and random-access
        // iterators.

    void privateJoinPrepend(deque *other);
        // Join '*this' and the specified 'other' deque into one.  After the
        // join, '*this' contains the concatenated sequence, in order, of
        // '*other' and '*this' elements, and 'other becomes a raw deque.

    void privateJoinAppend(deque *other);
        // Join '*this' and the specified 'other' deque into one.  After the
        // join, '*this' contains the concatenated sequence, in order, of
        // '*this' and '*other' elements, and 'other becomes a raw deque.

    template <class INPUT_ITER>
    size_type privatePrepend(INPUT_ITER                        first,
                             INPUT_ITER                        last,
                             std::input_iterator_tag);
    template <class INPUT_ITER>
    size_type privatePrepend(INPUT_ITER                        first,
                             INPUT_ITER                        last,
                             std::bidirectional_iterator_tag);
    template <class INPUT_ITER>
    size_type privatePrepend(INPUT_ITER                        first,
                             INPUT_ITER                        last,
                             std::random_access_iterator_tag);
        // Prepend the elements in the range specified as '[first .. last)' to
        // this deque, and return the number of elements appended.  The third
        // argument is used for overload resolution only.  The behavior is
        // undefined unless 'first' and 'last' refer to a sequence of valid
        // values where 'first' is at a position at or before 'last'.

    void privatePrependRaw(size_type numElements, const VALUE_TYPE& value);
        // Prepend the specified 'numElements' copies of the specified 'value'
        // to this deque.

    void privateSplit(deque *other, IteratorImp pos);
        // Split this deque in two.  After the split, '*this' contains elements
        // formerly in the range '[d_start .. pos)' and the specified 'other'
        // contains elements formerly in the range '[pos .. d_finish)'.  The
        // behavior is undefined unless 'other' is a raw deque, i.e., the
        // 'RawInit' constructor is used to create 'other'.

    // FRIENDS
    template <class VALUE_TYPE2, class ALLOCATOR2>
    friend class Deque_BlockCreator;

    template <class VALUE_TYPE2, class ALLOCATOR2>
    friend class Deque_Guard;

  public:
    // CREATORS

    // *** 23.2.1.1 construct/copy/destroy: ***

    explicit
    deque(const ALLOCATOR& basicAllocator = ALLOCATOR());
        // Create an empty deque.  Optionally specified the 'basicAllocator'
        // used to supply memory.  If 'basicAllocator' is not specified, a
        // default-constructed allocator is used.

    explicit
    deque(size_type         numElements,
          const ALLOCATOR&  basicAllocator = ALLOCATOR());
        // Create a deque of the specified 'numElements' length whose every
        // element is default-constructed.  Optionally specify the
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is not
        // specified, a default-constructed allocator is used.  Throw
        // 'bsl::length_error' if 'numElements > max_size()'.

    deque(size_type         numElements,
          const VALUE_TYPE& value,
          const ALLOCATOR&  basicAllocator = ALLOCATOR());
        // Create a deque of the specified 'numElements' length whose every
        // element equals the specified 'value'.  Optionally specify the
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is not
        // specified, a default-constructed allocator is used.  Throw
        // 'bsl::length_error' if 'numElements > max_size()'.

    template <class INPUT_ITER>
    deque(INPUT_ITER       first,
          INPUT_ITER       last,
          const ALLOCATOR& basicAllocator = ALLOCATOR());
        // Create a deque initially containing copies of the values in the
        // range starting at the specified 'first' and ending immediately
        // before the specified 'last' iterators of the parameterized
        // 'INPUT_ITER' type.  Optionally specify the 'basicAllocator' used to
        // supply memory.  If 'basicAllocator' is not specified, a
        // default-constructed allocator is used.  Throw 'bsl::length_error' if
        // the number of elements in '[first .. last)' exceeds the size
        // returned by 'max_size'.  The behavior is undefined unless 'first'
        // and 'last' refer to a sequence of valid values where 'first' is at a
        // position at or before 'last'.

    deque(const deque&     original);
    deque(const deque&     original,
          const ALLOCATOR& basicAllocator);
        // Create a deque that has the same value as the specified 'original'
        // deque.  Optionally specify the 'basicAllocator' used to supply
        // memory.  If 'basicAllocator' is not specified, then if 'ALLOCATOR'
        // is convertible from 'bslma::Allocator *', the currently installed
        // default allocator is used, otherwise the 'original' allocator is
        // used (as mandated per the ISO standard).

    ~deque();
        // Destroy this deque object.

    // MANIPULATORS
    deque& operator=(const deque& rhs);
        // Assign to this deque the value of the specified 'rhs' deque.

    template <class INPUT_ITER>
    void assign(INPUT_ITER first, INPUT_ITER last);
        // Assign to this deque the values in the range starting at the
        // specified 'first' and ending immediately before the specified 'last'
        // iterators of the parameterized 'INPUT_ITER' type.  This method
        // offers full guarantee of rollback in case an exception is thrown
        // other than by the 'VALUE_TYPE' copy constructor or assignment
        // operator.  The behavior is undefined unless 'first' and 'last' refer
        // to a sequence of valid values where 'first' is at a position at or
        // before 'last'.

    void assign(size_type numElements, const VALUE_TYPE& value);
        // Assign to this deque the value of the string of the specified
        // 'numElements' length whose every elements equal the specified
        // 'value'.  This method offers full guarantee of rollback in case an
        // exception is thrown other than by the 'VALUE_TYPE' copy constructor
        // or assignment operator.

    // *** 23.2.1.2 capacity: ***

    void reserve(size_type numElements);
        // Change the capacity of this deque such that existing iterators
        // remain valid, provided that no more than the specified 'numElements'
        // objects are pushed to the front or back of the deque after this
        // call.  If 'numElements < capacity()', this method has no effect.
        // Note that inserting to the deque may still incur memory allocations.
        // Also note that this method is not part of the C++ standard.

    void resize(size_type newLength);
    void resize(size_type newLength, VALUE_TYPE value);
        // Change the length of this deque to the specified 'newLength',
        // erasing characters at the end if 'newLength < size()' or appending
        // the appropriate number of copies of the optionally specified 'value'
        // at the end if 'length() < newLength'.  If 'value' is not specified,
        // a default-constructed 'VALUE_TYPE' value is used.  Throw
        // 'bsl::length_error' if 'newLength > max_size()'.

    // *** 23.2.1.3 modifiers: ***

    void push_front(const VALUE_TYPE& value);
        // Append a copy of the specified 'value' at the end of this deque.  If
        // 'value' has move semantics, then its value is valid but unspecified
        // upon returning from this function.  This method offers full
        // guarantee of rollback in case an exception is thrown other than by
        // the 'VALUE_TYPE' copy constructor or assignment operator.

    void push_back(const VALUE_TYPE& value);
        // Prepend a copy of the specified 'value' before the beginning of this
        // deque.  If 'value' has move semantics, then its value is valid but
        // unspecified upon returning from this function.  This method offers
        // full guarantee of rollback in case an exception is thrown other than
        // by the 'VALUE_TYPE' copy constructor or assignment operator.

    void pop_front();
        // Erase the first element from this deque.  The behavior is undefined
        // if this deque is empty.

    void pop_back();
        // Erase the last element from this deque.  The behavior is undefined
        // if this deque is empty.

    iterator insert(const_iterator position, const VALUE_TYPE& value);
        // Insert at the specified 'position' in this deque a copy of the
        // specified 'value', and return an iterator pointing to the newly
        // inserted element.  If 'value' has move semantics, then its value is
        // valid but unspecified upon returning from this function.  This
        // method offers full guarantee of rollback in case an exception is
        // thrown other than by the 'VALUE_TYPE' copy constructor or assignment
        // operator.  The behavior is undefined unless 'position' is an
        // iterator in the range '[begin() .. end()]'.

    void insert(const_iterator    position,
                size_type         numElements,
                const VALUE_TYPE& value);
        // Insert at the specified 'position' in this deque the specified
        // 'numElements' copies of the specified 'value'.  This method offers
        // full guarantee of rollback in case an exception is thrown other than
        // by the 'VALUE_TYPE' copy constructor or assignment operator.  The
        // behavior is undefined unless 'position' is an iterator in the range
        // '[begin() .. end()]'.

    template <class INPUT_ITER>
    void insert(const_iterator position, INPUT_ITER first, INPUT_ITER last);
        // Insert at the specified 'position' in this deque the values in the
        // range starting at the specified 'first' and ending immediately
        // before the specified 'last' iterators of the parameterized
        // 'INPUT_ITER' type.  This method offers full guarantee of rollback in
        // case an exception is thrown other than by the 'VALUE_TYPE' copy
        // constructor or assignment operator.  The behavior is undefined
        // unless 'position' is an iterator in the range '[begin() .. end()]',
        // and 'first' and 'last' refer to a sequence of valid values where
        // 'first' is at a position at or before 'last'.

    iterator erase(const_iterator position);
        // Remove from this deque the element at the specified 'position', and
        // return an iterator pointing to the element immediately following the
        // removed element, or the position returned by the method 'end' if the
        // removed element was the last in the sequence.  The behavior is
        // undefined unless 'position' is an iterator in the range
        // '[begin() .. end())'.

    iterator erase(const_iterator first, const_iterator last);
        // Remove from this deque the sequence of elements starting at the
        // specified 'first' position and ending before the specified 'last'
        // position, and return an iterator pointing to the element immediately
        // following the last removed element, or the position returned by the
        // method 'end' if the removed elements were last in the sequence.  The
        // behavior is undefined unless 'first' is an iterator in the range
        // '[begin() .. end()]' and 'last' is an iterator in the range
        // '[first .. end()]'.

    void swap(deque<VALUE_TYPE,ALLOCATOR>& other);
        // Exchange the value of this deque with that of the specified 'rhs'
        // deque, such that each deque has upon return the value of the other
        // deque prior to this call.  Note that this method does not throw or
        // invalidate iterators if the method 'get_allocator' invoked on this
        // object and 'rhs' returns the same value.

    void clear();
        // Remove all the elements from this deque.  Note that this deque is
        // empty after this call, but conserves the same capacity.

    // ACCESSORS

    // *** 23.2.1.1 construct/copy/destroy: ***

    allocator_type get_allocator() const;
        // Return the allocator used by this deque to supply memory.

    size_type max_size() const;
        // Return the maximum possible length of this deque.  This is largely a
        // theoretical maximum (such as the maximum size that can be held by
        // 'size_type').  Note that requests to create a deque longer than this
        // number of elements are guaranteed to raise a 'bsl::length_error'
        // exception.
};

// FREE OPERATORS
template <class VALUE_TYPE, class ALLOCATOR>
bool operator==(const deque<VALUE_TYPE,ALLOCATOR>& lhs,
                const deque<VALUE_TYPE,ALLOCATOR>& rhs);
    // Return 'true' if the specified 'lhs' deque has the same value as the
    // specified 'rhs' deque.  Two deques have the same value if they have the
    // same number of elements and the same element value at each index
    // position in the range 0 to 'size() - 1'.  This operator may only be used
    // when the class 'VALUE_TYPE' defines the operator:
    //..
    //  bool operator==(const VALUE_TYPE& lhs, const VALUE_TYPE& rhs);
    //..

template <class VALUE_TYPE, class ALLOCATOR>
inline
bool operator!=(const deque<VALUE_TYPE,ALLOCATOR>& lhs,
                const deque<VALUE_TYPE,ALLOCATOR>& rhs);
    // Return 'true' if the specified 'lhs' deque does not have the same value
    // as the specified 'rhs' deque.  Two deques do not have the same value if
    // they have different numbers of elements or different element values in
    // at least one index position in the range 0 to 'size() - 1'.  This
    // operator may only be used when the class 'VALUE_TYPE' defines the
    // operator:
    //..
    //  bool operator==(const VALUE_TYPE& lhs, const VALUE_TYPE& rhs);
    //..
    // Note that this operator returns '!(lhs == rhs)'.

template <class VALUE_TYPE, class ALLOCATOR>
inline
bool operator< (const deque<VALUE_TYPE,ALLOCATOR>& lhs,
                const deque<VALUE_TYPE,ALLOCATOR>& rhs);
    // Return 'true' if the specified 'lhs' deque is lexicographically smaller
    // than the specified 'rhs' deque, and 'false' otherwise.  A deque 'lhs' is
    // lexicographically smaller than another deque 'rhs' if there exists an
    // index 'i' between 0 and the minimum of 'lhs.size()' and 'rhs.size()'
    // such that 'lhs[i] == rhs[j]' for every '0 <= j < i', 'i < rhs.size()',
    // and either 'i == lhs.size()' or 'lhs[i] < rhs[i]'.  This operator may
    // only be used when the class 'VALUE_TYPE' defines the operator:
    //..
    //  bool operator<(const VALUE_TYPE& lhs, const VALUE_TYPE& rhs);
    //..

template <class VALUE_TYPE, class ALLOCATOR>
inline
bool operator> (const deque<VALUE_TYPE,ALLOCATOR>& lhs,
                const deque<VALUE_TYPE,ALLOCATOR>& rhs);
    // Return 'true' if the specified 'lhs' deque is lexicographically larger
    // than the specified 'rhs' deque, and 'false' otherwise.  This operator
    // may only be used when the class 'VALUE_TYPE' defines the operator:
    //..
    //  bool operator<(const VALUE_TYPE& lhs, const VALUE_TYPE& rhs);
    //..
    // Note that this operator returns 'rhs < lhs'.

template <class VALUE_TYPE, class ALLOCATOR>
inline
bool operator<=(const deque<VALUE_TYPE,ALLOCATOR>& lhs,
                const deque<VALUE_TYPE,ALLOCATOR>& rhs);
    // Return 'true' if the specified 'lhs' deque is lexicographically larger
    // than or equal to the specified 'rhs' deque, and 'false' otherwise.  This
    // operator may only be used when the class 'VALUE_TYPE' defines the
    // operator:
    //..
    //  bool operator<(const VALUE_TYPE& lhs, const VALUE_TYPE& rhs);
    //..
    // Note that this operator returns '!(rhs < lhs)'.

template <class VALUE_TYPE, class ALLOCATOR>
inline
bool operator>=(const deque<VALUE_TYPE,ALLOCATOR>& lhs,
                const deque<VALUE_TYPE,ALLOCATOR>& rhs);
    // Return 'true' if the specified 'lhs' deque is lexicographically larger
    // than the specified 'rhs' deque, and 'false' otherwise.  This operator
    // may only be used when the class 'VALUE_TYPE' defines the operator:
    //..
    //  bool operator<(const VALUE_TYPE& lhs, const VALUE_TYPE& rhs);
    //..
    // Note that this operator returns '!(lhs < rhs)'.

template <class VALUE_TYPE, class ALLOCATOR>
inline
void swap(deque<VALUE_TYPE, ALLOCATOR>& lhs,
          deque<VALUE_TYPE, ALLOCATOR>& rhs);
    // Exchange the value of the specified 'lhs' and 'rhs' deque, such that
    // each deque has upon return the value of the other deque prior to this
    // call.  Note that this method does not throw or invalidate iterators if
    // 'lhs.get_allocator()' and 'rhs.get_allocator()' have the same value.

                      // =============================
                      // class bsl::Deque_BlockCreator
                      // =============================

template <class VALUE_TYPE, class ALLOCATOR>
class Deque_BlockCreator {
    // Allocates blocks at the beginning or end of a deque and tentatively adds
    // them to the deque.  Keeps track of how many of the newly allocated
    // blocks have actually been used by the deque.  Destructor automatically
    // frees any unused blocks (e.g., in case an exception is thrown).

    // PRIVATE TYPES
    enum {
        BLOCK_LENGTH = Deque_BlockLengthCalcUtil<VALUE_TYPE>::BLOCK_LENGTH
    };

    typedef BloombergLP::bslalg::DequeImpUtil<VALUE_TYPE,
                                             BLOCK_LENGTH>       Imp;
    typedef typename Imp::Block                                  Block;
    typedef typename Imp::BlockPtr                               BlockPtr;
    typedef std::size_t                                          size_type;

    // DATA
    deque<VALUE_TYPE, ALLOCATOR> *d_deque_p;
    BlockPtr                     *d_boundary;

  public:
    // CREATORS
    explicit
    Deque_BlockCreator(deque<VALUE_TYPE, ALLOCATOR> *deque_p);
        // Construct a block allocator for the specified 'deque_p'.

    ~Deque_BlockCreator();
        // Free any blocks that have been allocated by this allocator but have
        // not yet been used by the deque.

    // MANIPULATORS
    void insertAtFront(size_type n);
        // Allocate the specified 'n' blocks at the front of the block array.
        // This method invalidates all iterators except 'd_deque_p->d_start'
        // and 'd_deque_p->d_finish'.

    void insertAtBack(size_type n);
        // Allocate the specified 'n' blocks at the back of the block array.
        // This method invalidates all iterators except 'd_deque_p->d_start'
        // and 'd_deque_p->d_finish'.

    BlockPtr *reserveBlockSlots(size_type numNewBlocks, bool atFront);
        // Make room for the specified 'numNewBlocks' pointers in the blocks
        // array.  If the specified 'atFront' is 'true', then make room at the
        // front of the array, else make room at the end of the array.  Return
        // a pointer to the insertion point, i.e., the point where new blocks
        // can be stored into the array, working backwards if 'atFront' is
        // 'true', or working forwards if 'atFront' is 'false'.  This method
        // invalidates all iterators except 'd_deque_p->d_start' and
        // 'd_deque_p->d_finish'.

    void release();
        // Relinquish control over any allocated blocks.  The destructor will
        // do nothing.
};

                        // ======================
                        // class Deque_ClearGuard
                        // ======================

template <class VALUE_TYPE, class ALLOCATOR>
class Deque_ClearGuard {
    // This class provides a proctor which, at destruction, calls 'clear' on
    // the deque passed at construction, unless the guard has been released
    // prior.

    // DATA
    deque<VALUE_TYPE, ALLOCATOR> *d_deque_p;

  public:
    // CREATORS
    explicit
    Deque_ClearGuard(deque<VALUE_TYPE, ALLOCATOR> *deque_p);
        // Initializes object to guard zero items from the specified 'deque_p'.

    ~Deque_ClearGuard();
        // Call the method 'clear' on the deque supplied at construction,
        // unless the method 'release' has been called before.

    // MANIPULATORS
    void release();
        // Release the deque protected by this guard from further management.
        // Note that this guard destructor will do nothing afterwards.
};

                        // ======================
                        // class bsl::Deque_Guard
                        // ======================

template <class VALUE_TYPE, class ALLOCATOR>
class Deque_Guard {
    // This class provides a proctor which maintains a count of the number of
    // elements constructed at the beginning or end of a deque, but not yet
    // committed to the deque's range of valid elements; if the count is
    // non-zero at destruction, the destructor destroys the elements in the
    // range '[d_deque_p->end() .. d_deque_p->end() + d_count)', or the range
    // '[d_deque_p->begin() - d_count .. d_deque_p->begin())', depending on
    // whether this proctor guards the end or beginning.  This guard is used to
    // undo element constructors in the event of an exception.  It is up to the
    // client code to increment the count whenever a new element is constructed
    // and to decrement the count whenever 'd_start' or 'd_finish' of the
    // guarded deque is moved to incorporate more elements.

    // PRIVATE TYPES
    enum {
        BLOCK_LENGTH = Deque_BlockLengthCalcUtil<VALUE_TYPE>::BLOCK_LENGTH
    };

    typedef BloombergLP::bslalg::DequeIterator<VALUE_TYPE,
                                              BLOCK_LENGTH>    IteratorImp;
    typedef BloombergLP::bslalg::DequePrimitives<VALUE_TYPE,
                                                BLOCK_LENGTH>  DequePrimitives;

    // DATA
    deque<VALUE_TYPE, ALLOCATOR> *d_deque_p;
    std::size_t                   d_count;
    bool                          d_isTail;

  public:
    // CREATORS
    Deque_Guard(deque<VALUE_TYPE, ALLOCATOR> *deque_p,
                bool                          isTail);
        // Initializes object to guard zero items from the specified 'deque_p'.
        // This guards either the tail or the head, as determined by the
        // specified 'isTail' boolean.

    ~Deque_Guard();
        // Call the parameterized 'VALUE_TYPE' destructor on objects in the
        // range '[d.end() .. d.end() + count())' if 'isTail' was specified as
        // 'true' during construction, or '[d.start() - count() .. d.start()]'
        // if 'isTail' was specified as 'false' during construction, where 'd'
        // is the deque used to construct this guard.

    // MANIPULATORS
    std::size_t operator++();
        // Increment the count of this guard, and return new count.

    std::size_t operator--();
        // Decrement the count of this guard, and return new count.

    void release();
        // Set the count of this tail guard to zero.  Note that this guard
        // destructor will do nothing if count is not incremented again after
        // this call.

    // ACCESSORS
    std::size_t count() const;
        // Return the current tail count.

    IteratorImp begin() const;
        // Return a pointer after the item the first item in the guarded range.

    IteratorImp end() const;
        // Return a pointer after the item the last item in the guarded range.
};

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================
// See IMPLEMENTATION NOTES in the .cpp before modifying anything below.

                             // ---------------------
                             // class bsl::Deque_Base
                             // ---------------------

// MANIPULATORS
template <class VALUE_TYPE>
inline
typename Deque_Base<VALUE_TYPE>::iterator
Deque_Base<VALUE_TYPE>::begin()
{
    return d_start;
}

template <class VALUE_TYPE>
inline
typename Deque_Base<VALUE_TYPE>::iterator
Deque_Base<VALUE_TYPE>::end()
{
    return d_finish;
}

template <class VALUE_TYPE>
inline
typename Deque_Base<VALUE_TYPE>::reverse_iterator
Deque_Base<VALUE_TYPE>::rbegin()
{
    return reverse_iterator(end());
}

template <class VALUE_TYPE>
inline
typename Deque_Base<VALUE_TYPE>::reverse_iterator
Deque_Base<VALUE_TYPE>::rend()
{
    return reverse_iterator(begin());
}

template <class VALUE_TYPE>
inline
typename Deque_Base<VALUE_TYPE>::reference
Deque_Base<VALUE_TYPE>::operator[](size_type position)
{
    BSLS_ASSERT_SAFE(begin() + position < end());

    // BSLS_ASSERT_SAFE(position >= 0); // 'size_type' is 'size_t', always >= 0

    return *(begin() + position);
}

template <class VALUE_TYPE>
typename Deque_Base<VALUE_TYPE>::reference
Deque_Base<VALUE_TYPE>::at(size_type position)
{
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(position >= size())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        BloombergLP::bslstl::StdExceptUtil::throwOutOfRange(
                                 "deque<...>::at(position): invalid position");
    }
    return *(begin() + position);
}

template <class VALUE_TYPE>
inline
typename Deque_Base<VALUE_TYPE>::reference
Deque_Base<VALUE_TYPE>::front()
{
    BSLS_ASSERT_SAFE(!empty());

    return *d_start;
}

template <class VALUE_TYPE>
inline
typename Deque_Base<VALUE_TYPE>::reference
Deque_Base<VALUE_TYPE>::back()
{
    BSLS_ASSERT_SAFE(!empty());

    IteratorImp backIterator = d_finish;
    --backIterator;
    return *backIterator;
}

// ACCESSORS
template <class VALUE_TYPE>
inline
typename Deque_Base<VALUE_TYPE>::const_iterator
Deque_Base<VALUE_TYPE>::begin() const
{
    return d_start;
}

template <class VALUE_TYPE>
inline
typename Deque_Base<VALUE_TYPE>::const_iterator
Deque_Base<VALUE_TYPE>::cbegin() const
{
    return d_start;
}

template <class VALUE_TYPE>
inline
typename Deque_Base<VALUE_TYPE>::const_iterator
Deque_Base<VALUE_TYPE>::end() const
{
    return d_finish;
}

template <class VALUE_TYPE>
inline
typename Deque_Base<VALUE_TYPE>::const_iterator
Deque_Base<VALUE_TYPE>::cend() const
{
    return d_finish;
}

template <class VALUE_TYPE>
inline
typename Deque_Base<VALUE_TYPE>::const_reverse_iterator
Deque_Base<VALUE_TYPE>::rbegin() const
{
    return const_reverse_iterator(end());
}

template <class VALUE_TYPE>
inline
typename Deque_Base<VALUE_TYPE>::const_reverse_iterator
Deque_Base<VALUE_TYPE>::crbegin() const
{
    return const_reverse_iterator(end());
}

template <class VALUE_TYPE>
inline
typename Deque_Base<VALUE_TYPE>::const_reverse_iterator
Deque_Base<VALUE_TYPE>::rend() const
{
    return const_reverse_iterator(begin());
}

template <class VALUE_TYPE>
inline
typename Deque_Base<VALUE_TYPE>::const_reverse_iterator
Deque_Base<VALUE_TYPE>::crend() const
{
    return const_reverse_iterator(begin());
}

template <class VALUE_TYPE>
inline
typename Deque_Base<VALUE_TYPE>::size_type
Deque_Base<VALUE_TYPE>::size() const
{
    return d_finish - d_start;
}

template <class VALUE_TYPE>
typename Deque_Base<VALUE_TYPE>::size_type
Deque_Base<VALUE_TYPE>::capacity() const
{
    IteratorImp first(d_blocks);
    IteratorImp last(d_blocks + d_blocksLength);  // note: not past-the-end
    size_type numPushFront = d_finish - first;
    size_type numPushBack  = (last - d_start) + 1;
    return numPushFront < numPushBack ? numPushFront : numPushBack;
}

template <class VALUE_TYPE>
inline
bool Deque_Base<VALUE_TYPE>::empty() const
{
    return d_start == d_finish;
}

template <class VALUE_TYPE>
inline
typename Deque_Base<VALUE_TYPE>::const_reference
Deque_Base<VALUE_TYPE>::operator[](size_type position) const
{
    BSLS_ASSERT_SAFE(begin() + position < end());

    // BSLS_ASSERT_SAFE(position >= 0); // 'size_type' is 'size_t', always >= 0

    return *(begin() + position);
}

template <class VALUE_TYPE>
typename Deque_Base<VALUE_TYPE>::const_reference
Deque_Base<VALUE_TYPE>::at(size_type position) const
{
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(position >= size())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        BloombergLP::bslstl::StdExceptUtil::throwOutOfRange(
                           "const deque<...>::at(position): invalid position");
    }
    return *(begin() + position);
}

template <class VALUE_TYPE>
inline
typename Deque_Base<VALUE_TYPE>::const_reference
Deque_Base<VALUE_TYPE>::front() const
{
    BSLS_ASSERT_SAFE(!empty());

    return *d_start;
}

template <class VALUE_TYPE>
inline
typename Deque_Base<VALUE_TYPE>::const_reference
Deque_Base<VALUE_TYPE>::back() const
{
    BSLS_ASSERT_SAFE(!empty());

    IteratorImp backIterator = d_finish;
    --backIterator;
    return *backIterator;
}

                             // ----------------
                             // class bsl::Deque
                             // ----------------

// PRIVATE CREATORS
template <class VALUE_TYPE, class ALLOCATOR>
inline
deque<VALUE_TYPE, ALLOCATOR>::deque(RawInit, const ALLOCATOR& basicAllocator)
: Deque_Base<VALUE_TYPE>()
, ContainerBase(basicAllocator)
{
    this->d_blocks = 0;
}

// PRIVATE MANIPULATORS
template <class VALUE_TYPE, class ALLOCATOR>
template <class INPUT_ITER>
typename deque<VALUE_TYPE,ALLOCATOR>::size_type
deque<VALUE_TYPE,ALLOCATOR>::privateAppend(
                                          INPUT_ITER                     first,
                                          INPUT_ITER                     last,
                                          std::random_access_iterator_tag)
{
    BlockCreator newBlocks(this);
    Guard guard(this, true);

    const size_type numElements = bsl::distance(first, last);
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(
                                    numElements > max_size() - this->size())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        BloombergLP::bslstl::StdExceptUtil::throwLengthError(
                                "deque<...>::insert(pos,n,v): deque too long");
    }

    for ( ; first != last; ++first) {
        IteratorImp insertPoint = guard.end();

        // There must be room for the iterator to be incremented.  Allocate new
        // block now if necessary.  We cannot wait until after the new element
        // is constructed or else we won't be able to increment the guard right
        // away and there will be a window where an exception will cause a
        // resource leak.

        if (1 == insertPoint.remainingInBlock()) {
            newBlocks.insertAtBack(1);
            insertPoint = guard.end();  // 'insertAtBack(1)' invalidated iter
        }
        BloombergLP::bslalg::ScalarPrimitives::copyConstruct(
                                             BSLS_UTIL_ADDRESSOF(*insertPoint),
                                             *first,
                                             this->bslmaAllocator());
        ++guard;
    }

    this->d_finish += guard.count();

    guard.release();
    return numElements;
}

template <class VALUE_TYPE, class ALLOCATOR>
template <class INPUT_ITER>
typename deque<VALUE_TYPE,ALLOCATOR>::size_type
deque<VALUE_TYPE,ALLOCATOR>::privateAppend(INPUT_ITER              first,
                                           INPUT_ITER              last,
                                           std::input_iterator_tag)
{
    BlockCreator newBlocks(this);
    Guard guard(this, true);

    size_type numElements = 0;
    size_type maxNumElements = max_size() - this->size();
    for ( ; first != last; ++first) {
        ++numElements;
        if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(
                                               numElements > maxNumElements)) {
            BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
            BloombergLP::bslstl::StdExceptUtil::throwLengthError(
                                "deque<...>::insert(pos,n,v): deque too long");
        }
        IteratorImp insertPoint = guard.end();

        // There must be room for the iterator to be incremented.  Allocate new
        // block now if necessary.  We cannot wait until after the new element
        // is constructed or else we won't be able to increment the guard right
        // away and there will be a window where an exception will cause a
        // resource leak.

        if (1 == insertPoint.remainingInBlock()) {
            newBlocks.insertAtBack(1);
            insertPoint = guard.end();  // 'insertAtBack(1)' invalidated iter
        }
        BloombergLP::bslalg::ScalarPrimitives::copyConstruct(
                                             BSLS_UTIL_ADDRESSOF(*insertPoint),
                                             *first,
                                             this->bslmaAllocator());
        ++guard;
    }

    this->d_finish += guard.count();

    guard.release();
    return numElements;
}

template <class VALUE_TYPE, class ALLOCATOR>
void deque<VALUE_TYPE,ALLOCATOR>::privateAppendRaw(
                                                 size_type         numElements,
                                                 const VALUE_TYPE& value)
{
    // Create new blocks at back.  In case of exception, any unused blocks are
    // returned to the allocator.

    size_type numNewBlocks = (this->d_finish.offsetInBlock() + numElements) /
                                                                  BLOCK_LENGTH;
    BlockCreator newBlocks(this);
    newBlocks.insertAtBack(numNewBlocks);

    DequePrimitives::uninitializedFillNBack(&this->d_finish,
                                            this->d_finish,
                                            numElements,
                                            value,
                                            this->bslmaAllocator());
}

template <class VALUE_TYPE, class ALLOCATOR>
void deque<VALUE_TYPE,ALLOCATOR>::privateInit(size_type numElements)
{
    size_type blocksLength = numElements / BLOCK_LENGTH + 1 +
                                                  2 * Imp::BLOCK_ARRAY_PADDING;

    // Allocate block pointer array.

    this->d_blocks = this->allocateN((BlockPtr*) 0, blocksLength);
    this->d_blocksLength = blocksLength;

    // Allocate the first block and store its pointer into the array.  Leave a
    // little room at the front and back of the array for growth.

    BlockPtr *firstBlockPtr = &this->d_blocks[Imp::BLOCK_ARRAY_PADDING];
    *firstBlockPtr = this->allocateN((Block*) 0, 1);

    // Calculate the offset into the first block such that 'n' elements will
    // leave equal space at the front of the first block and at the end of the
    // last block, remembering that the last element of the last block cannot
    // be used.  Centering the elements reduces the chance that either
    // 'push_back' or 'push_front' will need to allocate a new block.  In case
    // of an odd number of unused elements, slight preference is given to
    // 'push_back' over 'push_front'.

    const int offset = static_cast<int>(
                          (BLOCK_LENGTH - 1 - numElements % BLOCK_LENGTH) / 2);

    // Initialize the begin and end iterators.

    this->d_start = this->d_finish = IteratorImp(
                                            firstBlockPtr,
                                            (*firstBlockPtr)->d_data + offset);
}

template <class VALUE_TYPE, class ALLOCATOR>
template <class INTEGRAL_TYPE>
inline
void deque<VALUE_TYPE,ALLOCATOR>::privateInsertDispatch(
                           const_iterator                          position,
                           INTEGRAL_TYPE                           numElements,
                           INTEGRAL_TYPE                           value,
                           BloombergLP::bslmf::MatchArithmeticType ,
                           BloombergLP::bslmf::Nil                 )
{
    insert(position,
           static_cast<size_type>(numElements),
           static_cast<VALUE_TYPE>(value));
}

template <class VALUE_TYPE, class ALLOCATOR>
template <class INPUT_ITER>
void deque<VALUE_TYPE,ALLOCATOR>::privateInsertDispatch(
                                     const_iterator                   position,
                                     INPUT_ITER                       first,
                                     INPUT_ITER                       last,
                                     BloombergLP::bslmf::MatchAnyType ,
                                     BloombergLP::bslmf::MatchAnyType )
{
    typedef typename iterator_traits<INPUT_ITER>::iterator_category Tag;

    if (first == last) {
        return;                                                       // RETURN
    }

    if (position == this->cbegin()) {
        privatePrepend(first, last, Tag());
        return;                                                       // RETURN
    }

    if (position == this->cend()) {
        privateAppend(first, last, Tag());
        return;                                                       // RETURN
    }

    privateInsert(position, first, last, Tag());
}

template <class VALUE_TYPE, class ALLOCATOR>
template <class INPUT_ITER>
void deque<VALUE_TYPE,ALLOCATOR>::privateInsert(
                                             const_iterator           position,
                                             INPUT_ITER               first,
                                             INPUT_ITER               last,
                                             std::input_iterator_tag  tag)
{
    BSLS_ASSERT(first != last);

    iterator pos(position.imp());
    size_type currentSize = this->size();
    size_type posIdx = pos - this->begin();

    deque temp(RAW_INIT, this->get_allocator());
    privateSplit(&temp, position.imp());

    if (posIdx <= currentSize / 2) {
        Deque_Util::swap(
                        static_cast<Base *>(this), static_cast<Base *>(&temp));
        privatePrepend(first, last, tag);
        privateJoinPrepend(&temp);
    }
    else {
        privateAppend(first, last, tag);
        privateJoinAppend(&temp);
    }
}

template <class VALUE_TYPE, class ALLOCATOR>
void deque<VALUE_TYPE,ALLOCATOR>::privateSplit(
                                           deque<VALUE_TYPE, ALLOCATOR> *other,
                                           IteratorImp                   pos)
{
    // BEFORE:
    //..
    //               this->d_start.valuePtr() -+
    //                                        V
    //  this->d_start.blockPtr() --> H -> __AAA
    //                                 I -> AAAAA
    //             pos.blockPtr() -> J -> BBBxx <- pos.valuePtr() (at 1st x)
    //                                 K -> yyyyy
    //  this->d_finish.blockPtr() -> L -> y____
    //                                       ^
    //                                       +- this->d_finish.valuePtr()
    //
    // AFTER:
    //               this->d_start.valuePtr() -+
    //                                        V
    //  this->d_start.blockPtr() --> H -> __AAA
    //                                 I -> AAAAA
    //  this->d_finish.blockPtr() -> J -> BBB__
    //                                         ^
    //                                         +- this->d_finish.valuePtr()
    //
    //               other->d_start.valuePtr() -+
    //                                         V
    // other->d_start.blockPtr() --> M -> ___xx
    //                                 K -> yyyyy
    // other->d_finish.blockPtr() -> L -> y____
    //                                       ^
    //                                       +- other->d_finish.valuePtr()
    //
    // assert(! other.d_blocks);
    //..

    if (pos.blockPtr() == this->d_finish.blockPtr()) {
        // Split point is in last block.  Just copy portion after the split to
        // new block in 'other'.

        difference_type numAfter = this->d_finish.valuePtr() - pos.valuePtr();
        other->privateInit(numAfter);
        BloombergLP::bslalg::ArrayPrimitives::destructiveMove(
                                                     other->d_start.valuePtr(),
                                                     pos.valuePtr(),
                                                     this->d_finish.valuePtr(),
                                                     this->bslmaAllocator());
        other->d_finish += numAfter;
        this->d_finish = pos;
        return;                                                       // RETURN
    }

    if (pos.blockPtr() == this->d_start.blockPtr()) {
        // Split point is in first block.  Copy portion before the split to new
        // block in 'other' and swap.

        difference_type numBefore = pos.valuePtr() - this->d_start.valuePtr();
        other->privateInit(numBefore);
        BloombergLP::bslalg::ArrayPrimitives::destructiveMove(
                                                     other->d_start.valuePtr(),
                                                     this->d_start.valuePtr(),
                                                     pos.valuePtr(),
                                                     this->bslmaAllocator());
        other->d_finish += numBefore;
        this->d_start = pos;
        Deque_Util::swap(
                        static_cast<Base *>(this), static_cast<Base *>(other));
        return;                                                       // RETURN
    }

    // Compute number of unsplit blocks to move.

    difference_type numMoveBlocks = this->d_finish.blockPtr() - pos.blockPtr();

    size_type otherBlocksLength = numMoveBlocks + 1 +
                                              2 * Imp::BLOCK_ARRAY_PADDING;

    other->d_blocks = this->allocateN((BlockPtr *)0, otherBlocksLength);
    other->d_blocksLength = otherBlocksLength;

    // Good time to allocate block for exception safety.

    Block *newBlock = this->allocateN((Block*) 0, 1);

    // The following chunk of code will never throw an exception.  Move unsplit
    // blocks from 'this' to 'other', then adjust the iterators.

    std::memcpy(other->d_blocks + 1 + Imp::BLOCK_ARRAY_PADDING,
                pos.blockPtr() + 1,
                sizeof(BlockPtr) * numMoveBlocks);

    other->d_start = IteratorImp(&other->d_blocks[
                                                1 + Imp::BLOCK_ARRAY_PADDING]);
    other->d_finish = IteratorImp(other->d_start.blockPtr() +
                                  numMoveBlocks - 1,
                                  this->d_finish.valuePtr());

    BlockPtr *newBlockPtr = pos.blockPtr() + 1;
    *newBlockPtr = newBlock;
    this->d_finish = IteratorImp(newBlockPtr);

    // Current situation:
    //..
    //               this->d_start.valuePtr() -+
    //                                        V
    //  this->d_start.blockPtr() --> H -> __AAA
    //                                 I -> AAAAA
    //             pos.blockPtr() -> J -> BBBxx <- pos.valuePtr() (1st x)
    //  this->d_finish.blockPtr() -+> M -> _____
    //                              /        ^
    //                newBlockPtr -+         +- this->d_finish.valuePtr()
    //
    //            other->d_start.valuePtr() -+
    //                                      V
    //  other->d_start.blockPtr() --> K -> yyyyy
    //  other->d_finish.blockPtr() -> L -> y____
    //                                       ^
    //                                       +- other->d_finish.valuePtr()
    //..
    // Now we split the block containing "BBBxx" into two blocks, with the "xx"
    // part going into '*newBlockPtr'.  An exception can safely occur here
    // because the 'bslalg::ArrayPrimitives' functions are exception-neutral
    // and because all class invariants for both '*this' and 'other' hold going
    // in to this section.

    size_type splitOffset = pos.offsetInBlock();
    if (splitOffset >= pos.remainingInBlock()) {
        // Move the tail part of the block into the new block.

        value_type *splitValuePtr = newBlock->d_data + splitOffset;
        BloombergLP::bslalg::ArrayPrimitives::destructiveMove(
                                                       splitValuePtr,
                                                       pos.valuePtr(),
                                                       pos.blockEnd(),
                                                       this->bslmaAllocator());
    }
    else {
        // Move the head part of the block into the new block, then swap the
        // blocks within the 'd_blocks' array.

        BloombergLP::bslalg::ArrayPrimitives::destructiveMove(
                                                       newBlock->d_data,
                                                       pos.blockBegin(),
                                                       pos.valuePtr(),
                                                       this->bslmaAllocator());
        *newBlockPtr = *pos.blockPtr();
        *pos.blockPtr() = newBlock;
        // bslalg::ScalarPrimitives::swap(*pos.blockPtr(), *newBlockPtr); NOTE:
        // pos is invalid after swap.
    }

    // Move block to 'other' and adjust the iterators.  This will not throw.

    this->d_finish = IteratorImp(&newBlockPtr[-1],
                                 newBlockPtr[-1]->d_data + splitOffset);
    other->d_start.previousBlock();
    *(other->d_start.blockPtr()) = *newBlockPtr;
    other->d_start = IteratorImp(other->d_start.blockPtr(),
                                 other->d_start.blockBegin() + splitOffset);
}

template <class VALUE_TYPE, class ALLOCATOR>
inline
void deque<VALUE_TYPE,ALLOCATOR>::privateJoinPrepend(
                                           deque<VALUE_TYPE, ALLOCATOR> *other)
{
    privatePrepend(other->begin(),
                   other->end(),
                   std::random_access_iterator_tag());

    // Make 'other' raw again, and free its resources.

    deque<VALUE_TYPE, ALLOCATOR> temp(RAW_INIT, other->allocator());
    Deque_Util::move(static_cast<Base *>(&temp), static_cast<Base *>(other));
}

template <class VALUE_TYPE, class ALLOCATOR>
inline
void deque<VALUE_TYPE,ALLOCATOR>::privateJoinAppend(
                                           deque<VALUE_TYPE, ALLOCATOR> *other)
{
    privateAppend(other->begin(),
                  other->end(),
                  std::random_access_iterator_tag());

    // Make 'other' raw again, and free its resources.

    deque<VALUE_TYPE, ALLOCATOR> temp(RAW_INIT, other->allocator());
    Deque_Util::move(static_cast<Base *>(&temp), static_cast<Base *>(other));
}

template <class VALUE_TYPE, class ALLOCATOR>
template <class INPUT_ITER>
void deque<VALUE_TYPE,ALLOCATOR>::privateInsert(
                                           const_iterator            position,
                                           INPUT_ITER                first,
                                           INPUT_ITER                last,
                                           std::random_access_iterator_tag tag)
{
    BSLS_ASSERT(first != last);

    if (position == this->cbegin()) {
        privatePrepend(first, last, tag);
        return;                                                       // RETURN
    }

    if (position == this->cend()) {
        privateAppend(first, last, tag);
        return;                                                       // RETURN
    }

    const size_type currentSize = this->size();
    const size_type numElements = bsl::distance(first, last);
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(
                                     numElements > max_size() - currentSize)) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        BloombergLP::bslstl::StdExceptUtil::throwLengthError(
                                "deque<...>::insert(pos,n,v): deque too long");
    }

    iterator pos(position.imp());
    size_type posIdx = position - this->cbegin();
    if (posIdx <= currentSize / 2) {
        // Create new blocks at front.  In case of exception, any unused blocks
        // are returned to the allocator.

        size_type numNewBlocks = (this->d_start.remainingInBlock()
                                             + numElements - 1) / BLOCK_LENGTH;
        BlockCreator newBlocks(this);
        newBlocks.insertAtFront(numNewBlocks);

        DequePrimitives::insertAndMoveToFront(&this->d_start,
                                              this->d_start,
                                              this->d_start + posIdx,
                                              first,
                                              last,
                                              numElements,
                                              this->bslmaAllocator());
    } else {
        // Create new blocks at front.  In case of exception, any unused blocks
        // are returned to the allocator.

        size_type numNewBlocks = (this->d_finish.offsetInBlock() + numElements)
                                                                / BLOCK_LENGTH;
        BlockCreator newBlocks(this);
        newBlocks.insertAtBack(numNewBlocks);

        DequePrimitives::insertAndMoveToBack(&this->d_finish,
                                             this->d_finish,
                                             this->d_start + posIdx,
                                             first,
                                             last,
                                             numElements,
                                             this->bslmaAllocator());
    }
}

template <class VALUE_TYPE, class ALLOCATOR>
void deque<VALUE_TYPE,ALLOCATOR>::privatePrependRaw(
                                                 size_type         numElements,
                                                 const VALUE_TYPE& value)
{
    // Create new blocks at front.  In case of exception, any unused blocks are
    // returned to the allocator.

    size_type numNewBlocks = (this->d_start.remainingInBlock() +
                                               numElements - 1) / BLOCK_LENGTH;
    BlockCreator newBlocks(this);
    newBlocks.insertAtFront(numNewBlocks);

    DequePrimitives::uninitializedFillNFront(&this->d_start,
                                             this->d_start,
                                             numElements,
                                             value,
                                             this->bslmaAllocator());
}

template <class VALUE_TYPE, class ALLOCATOR>
template <class INPUT_ITER>
typename deque<VALUE_TYPE,ALLOCATOR>::size_type
deque<VALUE_TYPE,ALLOCATOR>::privatePrepend(INPUT_ITER              first,
                                            INPUT_ITER              last,
                                            std::input_iterator_tag tag)
{
    deque temp(RAW_INIT, this->get_allocator());
    temp.privateInit(this->size() + 1);
    size_type numElements = temp.privateAppend(first, last, tag);

    // Check whether appending or prepending is more economical.

    if (numElements > this->size()) {
        Deque_Util::swap((Base *)this, (Base *)&temp);
        privateJoinAppend(&temp);
    }
    else {
        privateJoinPrepend(&temp);
    }

    return numElements;
}

template <class VALUE_TYPE, class ALLOCATOR>
template <class INPUT_ITER>
typename deque<VALUE_TYPE,ALLOCATOR>::size_type
deque<VALUE_TYPE,ALLOCATOR>::privatePrepend(
                                        INPUT_ITER                       first,
                                        INPUT_ITER                       last,
                                        std::bidirectional_iterator_tag)
{

    BlockCreator newBlocks(this);
    Guard guard(this, false);

    size_type numElements = 0;
    size_type maxNumElements = max_size() - this->size();
    do {
        ++numElements;
        if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(
                                               numElements > maxNumElements)) {
            BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
            BloombergLP::bslstl::StdExceptUtil::throwLengthError(
                                "deque<...>::insert(pos,n,v): deque too long");
        }

        IteratorImp insertPoint = guard.begin();

        // There must be room for the iterator to be decremented.  Allocate new
        // block now if necessary, with same caveat as above.

        if (insertPoint.valuePtr() == insertPoint.blockBegin()) {
            newBlocks.insertAtFront(1);
            insertPoint = guard.begin();  // 'insertAtFront' invalidates
                                          // 'insertPoint'
        }
        --insertPoint;
        BloombergLP::bslalg::ScalarPrimitives::copyConstruct(
                                             BSLS_UTIL_ADDRESSOF(*insertPoint),
                                             *--last,
                                             this->bslmaAllocator());
        ++guard;
    } while (first != last);

    this->d_start -= guard.count();
    guard.release();
    return numElements;
}

template <class VALUE_TYPE, class ALLOCATOR>
template <class INPUT_ITER>
typename deque<VALUE_TYPE,ALLOCATOR>::size_type
deque<VALUE_TYPE,ALLOCATOR>::privatePrepend(
                                         INPUT_ITER                      first,
                                         INPUT_ITER                      last,
                                         std::random_access_iterator_tag)
{

    const size_type numElements = bsl::distance(first, last);
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(
                                    numElements > max_size() - this->size())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        BloombergLP::bslstl::StdExceptUtil::throwLengthError(
                                "deque<...>::insert(pos,n,v): deque too long");
    }

    BlockCreator newBlocks(this);
    Guard guard(this, false);

    do {
        IteratorImp insertPoint = guard.begin();

        // There must be room for the iterator to be decremented.  Allocate new
        // block now if necessary, with same caveat as above.

        if (insertPoint.valuePtr() == insertPoint.blockBegin()) {
            newBlocks.insertAtFront(1);
            insertPoint = guard.begin();  // 'insertAtFront' invalidates
                                          // 'insertPoint'
        }
        --insertPoint;
        BloombergLP::bslalg::ScalarPrimitives::copyConstruct(
                                             BSLS_UTIL_ADDRESSOF(*insertPoint),
                                             *--last,
                                             this->bslmaAllocator());
        ++guard;
    } while (first != last);

    this->d_start -= guard.count();
    guard.release();
    return numElements;
}

// CREATORS
template <class VALUE_TYPE, class ALLOCATOR>
deque<VALUE_TYPE, ALLOCATOR>::deque(const ALLOCATOR& basicAllocator)
: Deque_Base<VALUE_TYPE>()
, ContainerBase(basicAllocator)
{
    deque temp(RAW_INIT, this->get_allocator());
    temp.privateInit(0);
    Deque_Util::move(static_cast<Base*>(this), static_cast<Base *>(&temp));
}

template <class VALUE_TYPE, class ALLOCATOR>
deque<VALUE_TYPE,ALLOCATOR>::deque(size_type         numElements,
                                   const ALLOCATOR&  basicAllocator)
: Deque_Base<VALUE_TYPE>()
, ContainerBase(basicAllocator)
{
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(numElements > max_size())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        BloombergLP::bslstl::StdExceptUtil::throwLengthError(
                                       "deque<...>::deque(n): deque too long");
    }
    deque temp(RAW_INIT, this->get_allocator());
    temp.privateInit(numElements);
    temp.privateAppendRaw(numElements, VALUE_TYPE());
    Deque_Util::move(static_cast<Base*>(this), static_cast<Base *>(&temp));
}

template <class VALUE_TYPE, class ALLOCATOR>
deque<VALUE_TYPE,ALLOCATOR>::deque(size_type         numElements,
                                   const VALUE_TYPE& value,
                                   const ALLOCATOR&  basicAllocator)
: Deque_Base<VALUE_TYPE>()
, ContainerBase(basicAllocator)
{
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(numElements > max_size())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        BloombergLP::bslstl::StdExceptUtil::throwLengthError(
                                     "deque<...>::deque(n,v): deque too long");
    }
    deque temp(RAW_INIT, this->get_allocator());
    temp.privateInit(numElements);
    temp.privateAppendRaw(numElements, value);
    Deque_Util::move(static_cast<Base*>(this), static_cast<Base *>(&temp));
}

template <class VALUE_TYPE, class ALLOCATOR>
template <class INPUT_ITER>
deque<VALUE_TYPE,ALLOCATOR>::deque(INPUT_ITER       first,
                                   INPUT_ITER       last,
                                   const ALLOCATOR& basicAllocator)
: Deque_Base<VALUE_TYPE>()
, ContainerBase(basicAllocator)
{
    deque temp(RAW_INIT, this->get_allocator());
    temp.privateInit(0);
    temp.insert(temp.begin(), first, last);
    Deque_Util::move(static_cast<Base*>(this), static_cast<Base *>(&temp));
}

template <class VALUE_TYPE, class ALLOCATOR>
deque<VALUE_TYPE,ALLOCATOR>::deque(const deque<VALUE_TYPE,ALLOCATOR>& original)
: Deque_Base<VALUE_TYPE>()
, ContainerBase(original)
{
    deque temp(RAW_INIT, this->get_allocator());
    temp.privateInit(original.size());
    temp.privateAppend(original.begin(),
                       original.end(),
                       std::random_access_iterator_tag());
    Deque_Util::move(static_cast<Base*>(this), static_cast<Base *>(&temp));
}

template <class VALUE_TYPE, class ALLOCATOR>
deque<VALUE_TYPE,ALLOCATOR>::deque(
                             const deque<VALUE_TYPE,ALLOCATOR>& original,
                             const ALLOCATOR&                   basicAllocator)
: Deque_Base<VALUE_TYPE>()
, ContainerBase(basicAllocator)
{
    deque temp(RAW_INIT, this->get_allocator());
    temp.privateInit(original.size());
    temp.privateAppend(original.begin(),
                       original.end(),
                       std::random_access_iterator_tag());
    Deque_Util::move(static_cast<Base*>(this), static_cast<Base *>(&temp));
}

template <class VALUE_TYPE, class ALLOCATOR>
deque<VALUE_TYPE,ALLOCATOR>::~deque()
{
    if (0 == this->d_blocks) {
        // Nothing to do when destroying raw deques.

        return;                                                       // RETURN
    }

    if (0 != this->d_start.blockPtr()) {
        // Destroy all elements and deallocate all but one block.

        clear();

        // Deallocate the remaining (empty) block.

        this->deallocateN(*this->d_start.blockPtr(), 1);
    }

    // Deallocate the array of block pointers.

    this->deallocateN(this->d_blocks, this->d_blocksLength);
}

// MANIPULATORS
template <class VALUE_TYPE, class ALLOCATOR>
deque<VALUE_TYPE,ALLOCATOR>&
deque<VALUE_TYPE,ALLOCATOR>::operator=(const deque<VALUE_TYPE,ALLOCATOR>& rhs)
{
    if (this == &rhs) {
        return *this;                                                 // RETURN
    }

    size_type origSize = this->size();
    size_type rhsSize  = rhs.size();
    size_type minSize;

    if (origSize > rhsSize) {
        // Make shorter by deleting excess elements.

        minSize = rhsSize;
        erase(this->begin() + minSize, this->end());
    }
    else {
        // Make longer by appending new elements.

        minSize = origSize;
        privateAppend(rhs.begin() + minSize,
                      rhs.end(),
                      std::random_access_iterator_tag());
    }

    // Copy the smaller of the number of elements in 'rhs' and '*this'.

    IteratorImp from = rhs.d_start;
    IteratorImp to   = this->d_start;
    for (size_type i = 0; i < minSize; ++i) {
        *to = *from;
        ++to;
        ++from;
    }

    return *this;
}

template <class VALUE_TYPE, class ALLOCATOR>
template <class INPUT_ITER>
void deque<VALUE_TYPE,ALLOCATOR>::assign(INPUT_ITER first,
                                         INPUT_ITER last)
{
    typedef typename iterator_traits<INPUT_ITER>::iterator_category Tag;

    // If anything throws, clear the deque, to simulate standard behavior,
    // which is 'erase(begin(), end()); insert(begin(), first, last);'.

    ClearGuard guard(this);

    // Copy over existing elements.

    IteratorImp i;
    for (i = this->d_start; !(i == this->d_finish) && first != last;
                                                                ++i, ++first) {
        *i = *first;
    }

    if (!(i == this->d_finish)) {
        // Erase elements past the last one copied.

        erase(i, this->end());
    }
    else {
        // Still more elements to copy.  Append them.

        privateAppend(first, last, Tag());
    }

    guard.release();
}

template <class VALUE_TYPE, class ALLOCATOR>
void deque<VALUE_TYPE,ALLOCATOR>::assign(size_type         numElements,
                                         const VALUE_TYPE& value)
{
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(numElements > max_size())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        BloombergLP::bslstl::StdExceptUtil::throwLengthError(
                                    "deque<...>::assign(n,v): deque too long");
    }

    // If anything throws, clear the deque, to simulate standard behavior,
    // which is 'erase(begin, end(); insert(begin(), first, last);'.

    ClearGuard guard(this);

    size_type origSize = this->size();
    size_type minSize;

    if (numElements < origSize) {
        minSize = numElements;
        erase(this->begin() + numElements, this->end());
    }
    else {
        minSize = origSize;
        privateAppendRaw(numElements - origSize, value);
    }

    IteratorImp to = this->d_start;
    for (size_type i = 0; i < minSize; ++i) {
        *to = value;
        ++to;
    }

    guard.release();
}

template <class VALUE_TYPE, class ALLOCATOR>
void deque<VALUE_TYPE,ALLOCATOR>::reserve(size_type numElements)
{
    if (this->capacity() > numElements) {
        return;                                                       // RETURN
    }

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(numElements > max_size())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        BloombergLP::bslstl::StdExceptUtil::throwLengthError(
                                     "deque<...>::reserve(n): deque too long");
    }

    // This number of blocks, when present on both sides of start and finish,
    // guarantees that iterators won't get invalidated by 'numElements' further
    // 'push_front' or 'push_back'.  It is not minimal but it suffices.

    size_type numBlocks = (numElements + BLOCK_LENGTH - 1) / BLOCK_LENGTH
                      - (this->d_finish.blockPtr() - this->d_start.blockPtr());

    // This is inefficient, as it can perform two allocations in the worst
    // case, but in most cases only one of the 'reserveBlockSlots' will
    // allocate.

    BlockCreator newBlocks(this);
    newBlocks.reserveBlockSlots(numBlocks, true);
    newBlocks.reserveBlockSlots(numBlocks, false);
}

template <class VALUE_TYPE, class ALLOCATOR>
inline
void deque<VALUE_TYPE,ALLOCATOR>::resize(size_type  newLength)
{
    resize(newLength, VALUE_TYPE());
}

template <class VALUE_TYPE, class ALLOCATOR>
void deque<VALUE_TYPE,ALLOCATOR>::resize(size_type  newLength,
                                         VALUE_TYPE value)
{
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(newLength > max_size())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        BloombergLP::bslstl::StdExceptUtil::throwLengthError(
                                    "deque<...>::resize(n,v): deque too long");
    }

    size_type origSize = this->size();

    if (newLength <= origSize) {
        erase(this->begin() + newLength, this->end());
    }
    else {
        privateAppendRaw(newLength - origSize, value);
    }
}

template <class VALUE_TYPE, class ALLOCATOR>
void deque<VALUE_TYPE,ALLOCATOR>::push_front(const VALUE_TYPE& value)
{
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(this->size() >= max_size())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        BloombergLP::bslstl::StdExceptUtil::throwLengthError(
                              "deque<...>::push_front(pos,v): deque too long");
    }

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(
                                         0 == this->d_start.offsetInBlock())) {

        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        BlockCreator newBlocks(this);
        // If this throws, then the value is unmodified, otherwise, we can
        // always copy construct below.

        newBlocks.insertAtFront(1);

        BloombergLP::bslalg::ScalarPrimitives::copyConstruct(
                                                (this->d_start - 1).valuePtr(),
                                                value,
                                                this->bslmaAllocator());

        --this->d_start;
    }
    else {
        // Since the offset is non-zero, it is safe to directly decrement the
        // pointer.  This is much quicker than calling 'operator--'.

        BloombergLP::bslalg::ScalarPrimitives::copyConstruct(
                                                  this->d_start.valuePtr() - 1,
                                                  value,
                                                  this->bslmaAllocator());
        this->d_start.valuePtrDecrement();
    }
}

template <class VALUE_TYPE, class ALLOCATOR>
void deque<VALUE_TYPE,ALLOCATOR>::push_back(const VALUE_TYPE& value)
{
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(this->size() >= max_size())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        BloombergLP::bslstl::StdExceptUtil::throwLengthError(
                               "deque<...>::push_back(pos,v): deque too long");
    }

    if (BSLS_PERFORMANCEHINT_PREDICT_LIKELY(
                                      1 < this->d_finish.remainingInBlock())) {
        BloombergLP::bslalg::ScalarPrimitives::copyConstruct(
                                                     this->d_finish.valuePtr(),
                                                     value,
                                                     this->bslmaAllocator());
        this->d_finish.valuePtrIncrement();
    }
    else {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        BlockCreator newBlocks(this);

        // If this throws, then the value of the deque is unmodified,
        // otherwise, we can always copy construct below.  Note that 'value' is
        // still a valid reference even if it belongs to this deque.

        newBlocks.insertAtBack(1);
        BloombergLP::bslalg::ScalarPrimitives::copyConstruct(
                                                     this->d_finish.valuePtr(),
                                                     value,
                                                     this->bslmaAllocator());
        this->d_finish.nextBlock();
    }
}

template <class VALUE_TYPE, class ALLOCATOR>
void deque<VALUE_TYPE,ALLOCATOR>::insert(const_iterator    position,
                                         size_type         numElements,
                                         const VALUE_TYPE& value)
{
    BSLS_ASSERT_SAFE(position >= this->begin());
    BSLS_ASSERT_SAFE(position <= this->end());

    if (0 == numElements) {
        return;                                                       // RETURN
    }

    const size_type currentSize = this->size();
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(
                                     numElements > max_size() - currentSize)) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        BloombergLP::bslstl::StdExceptUtil::throwLengthError(
                                "deque<...>::insert(pos,n,v): deque too long");
    }

    if (position == this->cbegin()) {
        privatePrependRaw(numElements, value);
        return;                                                       // RETURN
    }

    if (position == this->cend()) {
        privateAppendRaw(numElements, value);
        return;                                                       // RETURN
    }

    size_type posIdx = position.imp() - this->d_start;

    if (posIdx <= currentSize / 2) {
        // Create new blocks at front.  In case of exception, any unused blocks
        // are returned to the allocator.

        size_type numNewBlocks = (this->d_start.remainingInBlock()
                                             + numElements - 1) / BLOCK_LENGTH;
        BlockCreator newBlocks(this);
        newBlocks.insertAtFront(numNewBlocks);

        DequePrimitives::insertAndMoveToFront(&this->d_start,
                                              this->d_start,
                                              this->d_start + posIdx,
                                              numElements,
                                              value,
                                              this->bslmaAllocator());
    }
    else {
        // Create new blocks at back.  In case of exception, any unused blocks
        // are returned to the allocator.

        size_type numNewBlocks = (this->d_finish.offsetInBlock() + numElements)
                                                                / BLOCK_LENGTH;
        BlockCreator newBlocks(this);
        newBlocks.insertAtBack(numNewBlocks);

        DequePrimitives::insertAndMoveToBack(&this->d_finish,
                                             this->d_finish,
                                             this->d_start + posIdx,
                                             numElements,
                                             value,
                                             this->bslmaAllocator());
    }
}

template <class VALUE_TYPE, class ALLOCATOR>
typename deque<VALUE_TYPE,ALLOCATOR>::iterator
deque<VALUE_TYPE,ALLOCATOR>::insert(const_iterator    position,
                                    const VALUE_TYPE& value)
{
    BSLS_ASSERT_SAFE(position >= this->begin());
    BSLS_ASSERT_SAFE(position <= this->end());

    if (position == this->cbegin()) {
        push_front(value);
        return this->begin();                                         // RETURN
    }

    if (position == this->cend()) {
        push_back(value);
        return iterator(this->d_finish - 1);                          // RETURN
    }

    // The test is placed here because 'push_front' and 'push_back' already do
    // the check.

    const size_type currentSize = this->size();
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(currentSize >= max_size())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        BloombergLP::bslstl::StdExceptUtil::throwLengthError(
                                "deque<...>::insert(pos,n,v): deque too long");
    }

    iterator pos(position.imp());
    size_type posIdx = position - this->cbegin();
    if (posIdx <= currentSize / 2) {
        BlockCreator newBlocks(this);
        if (this->d_start.remainingInBlock() == BLOCK_LENGTH) {
            newBlocks.insertAtFront(1);
        }
        DequePrimitives::insertAndMoveToFront(&this->d_start,
                                              this->d_start,
                                              this->d_start + posIdx,
                                              1,
                                              value,
                                              this->bslmaAllocator());
    }
    else {
        BlockCreator newBlocks(this);
        if (this->d_finish.offsetInBlock() == BLOCK_LENGTH - 1) {
            newBlocks.insertAtBack(1);
        }
        DequePrimitives::insertAndMoveToBack(&this->d_finish,
                                             this->d_finish,
                                             this->d_start + posIdx,
                                             1,
                                             value,
                                             this->bslmaAllocator());
    }
    return this->begin() + posIdx;
}

template <class VALUE_TYPE, class ALLOCATOR>
template <class INPUT_ITER>
inline
void deque<VALUE_TYPE,ALLOCATOR>::insert(const_iterator position,
                                         INPUT_ITER     first,
                                         INPUT_ITER     last)
{
    BSLS_ASSERT_SAFE(position >= this->begin());
    BSLS_ASSERT_SAFE(position <= this->end());

    privateInsertDispatch(position,
                          first,
                          last,
                          first,
                          BloombergLP::bslmf::Nil());
}

template <class VALUE_TYPE, class ALLOCATOR>
void deque<VALUE_TYPE,ALLOCATOR>::pop_front()
{
    BSLS_ASSERT_SAFE(!this->empty());

    BloombergLP::bslalg::ScalarDestructionPrimitives::destroy(
                                                     this->d_start.valuePtr());

    if (1 == this->d_start.remainingInBlock()) {
        this->deallocateN(*this->d_start.blockPtr(), 1);
        this->d_start.nextBlock();
        return;                                                       // RETURN
    }

    this->d_start.valuePtrIncrement();
}

template <class VALUE_TYPE, class ALLOCATOR>
void deque<VALUE_TYPE,ALLOCATOR>::pop_back()
{
    BSLS_ASSERT_SAFE(!this->empty());

    if (0 == this->d_finish.offsetInBlock()) {
        --this->d_finish;
        BloombergLP::bslalg::ScalarDestructionPrimitives::destroy(
                                                    this->d_finish.valuePtr());
        this->deallocateN(this->d_finish.blockPtr()[1], 1);
        return;                                                       // RETURN
    }

    this->d_finish.valuePtrDecrement();
    BloombergLP::bslalg::ScalarDestructionPrimitives::destroy(
                                                    this->d_finish.valuePtr());
}

template <class VALUE_TYPE, class ALLOCATOR>
typename deque<VALUE_TYPE,ALLOCATOR>::iterator
deque<VALUE_TYPE,ALLOCATOR>::erase(const_iterator first,
                                   const_iterator last)
{
    BSLS_ASSERT_SAFE(first >= this->begin());
    BSLS_ASSERT_SAFE(first <= this->end());
    BSLS_ASSERT_SAFE(first <= last);
    BSLS_ASSERT_SAFE(last <= this->end());

    iterator first_imp = this->begin() + (first - this->cbegin());
    iterator last_imp  = this->begin() + (last - this->cbegin());
    iterator oldStart  = this->begin();
    iterator oldFinish = this->end();
    iterator result = iterator(DequePrimitives::erase(&this->d_start,
                                                      &this->d_finish,
                                                      this->d_start,
                                                      first_imp.imp(),
                                                      last_imp.imp(),
                                                      this->d_finish,
                                                      this->bslmaAllocator()));

    // Deallocate blocks not used anymore.

    for ( ; oldStart.imp().blockPtr() != this->d_start.blockPtr();
                                                  oldStart.imp().nextBlock()) {
        this->deallocateN(oldStart.imp().blockPtr()[0], 1);
    }
    for ( ; oldFinish.imp().blockPtr() != this->d_finish.blockPtr();
                                             oldFinish.imp().previousBlock()) {
        this->deallocateN(oldFinish.imp().blockPtr()[0], 1);
    }
    return result;
}

template <class VALUE_TYPE, class ALLOCATOR>
typename deque<VALUE_TYPE,ALLOCATOR>::iterator
deque<VALUE_TYPE,ALLOCATOR>::erase(const_iterator position)
{
    BSLS_ASSERT_SAFE(position >= this->begin());
    BSLS_ASSERT_SAFE(position < this->end());

    if (position == const_iterator(this->d_start)) {
        pop_front();
        return this->begin();                                         // RETURN
    }

    if (position + 1 == const_iterator(this->d_finish)) {
        pop_back();
        return this->end();                                           // RETURN
    }

    return erase(position, position + 1);
}

template <class VALUE_TYPE, class ALLOCATOR>
void deque<VALUE_TYPE,ALLOCATOR>::swap(deque<VALUE_TYPE,ALLOCATOR>& other)
{
    if (BSLS_PERFORMANCEHINT_PREDICT_LIKELY(
                             this->get_allocator() == other.get_allocator())) {
        Deque_Util::swap(static_cast<Base *>(this),
                         static_cast<Base *>(&other));
        return;                                                       // RETURN
    }
    else {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;

        deque d1(other, this->get_allocator());
        deque d2(*this, other.get_allocator());

        Deque_Util::swap(static_cast<Base *>(&d1), static_cast<Base *>(this));
        Deque_Util::swap(static_cast<Base *>(&d2),
                         static_cast<Base *>(&other));
    }
}

template <class VALUE_TYPE, class ALLOCATOR>
void deque<VALUE_TYPE,ALLOCATOR>::clear()
{
    DequePrimitives::destruct(this->d_start, this->d_finish);

    // Deallocate all blocks except finishBlock.

    BlockPtr *startBlock = this->d_start.blockPtr();
    BlockPtr *finishBlock = this->d_finish.blockPtr();
    for ( ; startBlock != finishBlock; ++startBlock) {
        this->deallocateN(*startBlock, 1);
    }

    // Reposition in the middle.

    size_type  blockOffset = this->d_blocksLength / 2;
    int        offset      = BLOCK_LENGTH / 2;
    BlockPtr  *blockPtr    = this->d_blocks + blockOffset;

    *blockPtr = *finishBlock;

    this->d_start = this->d_finish = IteratorImp(blockPtr,
                                                 (*blockPtr)->d_data + offset);
}

// ACCESSORS
template <class VALUE_TYPE, class ALLOCATOR>
inline
typename deque<VALUE_TYPE,ALLOCATOR>::allocator_type
deque<VALUE_TYPE,ALLOCATOR>::get_allocator() const
{
    return ContainerBase::allocator();
}

template <class VALUE_TYPE, class ALLOCATOR>
inline
typename deque<VALUE_TYPE,ALLOCATOR>::size_type
deque<VALUE_TYPE,ALLOCATOR>::max_size() const
{
    return this->get_allocator().max_size();
}

// FREE OPERATORS
template <class VALUE_TYPE, class ALLOCATOR>
bool operator==(const deque<VALUE_TYPE,ALLOCATOR>& lhs,
                const deque<VALUE_TYPE,ALLOCATOR>& rhs)
{
    if (lhs.size() != rhs.size()) {
        return false;                                                 // RETURN
    }

    enum {
        BLOCK_LENGTH = Deque_BlockLengthCalcUtil<VALUE_TYPE>::BLOCK_LENGTH
    };

    typedef BloombergLP::bslalg::DequeIterator<VALUE_TYPE,
                                               BLOCK_LENGTH> Iterator;

    Iterator lhsBegin = lhs.begin().imp();
    Iterator lhsEnd   = lhs.end().imp();
    Iterator rhsBegin = rhs.begin().imp();

    for (; !(lhsBegin == lhsEnd); ++lhsBegin, ++rhsBegin) {
        if (!(*lhsBegin == *rhsBegin)) {
            return false;                                             // RETURN
        }
    }
    return true;
}

template <class VALUE_TYPE, class ALLOCATOR>
inline
bool operator!=(const deque<VALUE_TYPE,ALLOCATOR>& lhs,
                const deque<VALUE_TYPE,ALLOCATOR>& rhs)
{
    return ! (lhs == rhs);
}

template <class VALUE_TYPE, class ALLOCATOR>
inline
bool operator< (const deque<VALUE_TYPE,ALLOCATOR>& lhs,
                const deque<VALUE_TYPE,ALLOCATOR>& rhs)
{
    return 0 > BloombergLP::bslalg::RangeCompare::lexicographical(lhs.begin(),
                                                                  lhs.end(),
                                                                  lhs.size(),
                                                                  rhs.begin(),
                                                                  rhs.end(),
                                                                  rhs.size());
}

template <class VALUE_TYPE, class ALLOCATOR>
inline
bool operator> (const deque<VALUE_TYPE,ALLOCATOR>& lhs,
                const deque<VALUE_TYPE,ALLOCATOR>& rhs)
{
    return (rhs < lhs);
}

template <class VALUE_TYPE, class ALLOCATOR>
inline
bool operator>=(const deque<VALUE_TYPE,ALLOCATOR>& lhs,
                const deque<VALUE_TYPE,ALLOCATOR>& rhs)
{
    return ! (lhs < rhs);
}

template <class VALUE_TYPE, class ALLOCATOR>
inline
bool operator<=(const deque<VALUE_TYPE,ALLOCATOR>& lhs,
                const deque<VALUE_TYPE,ALLOCATOR>& rhs)
{
    return ! (rhs < lhs);
}

template <class VALUE_TYPE, class ALLOCATOR>
inline
void swap(deque<VALUE_TYPE, ALLOCATOR>& a,
          deque<VALUE_TYPE, ALLOCATOR>& b)
{
    a.swap(b);
}

                      // --------------------------------
                      // class bslstl::Deque_BlockCreator
                      // --------------------------------

// CREATORS
template <class VALUE_TYPE, class ALLOCATOR>
inline
Deque_BlockCreator<VALUE_TYPE, ALLOCATOR>::Deque_BlockCreator(
                                         deque<VALUE_TYPE, ALLOCATOR> *deque_p)
: d_deque_p(deque_p)
, d_boundary(0)
{
}

template <class VALUE_TYPE, class ALLOCATOR>
Deque_BlockCreator<VALUE_TYPE, ALLOCATOR>::~Deque_BlockCreator()
{
    if (0 != d_boundary) {
        BlockPtr *delFirst, *delLast;
        if (d_boundary <= d_deque_p->d_start.blockPtr()) {
            delFirst = d_boundary;
            delLast = d_deque_p->d_start.blockPtr();
        }
        else {
            delFirst = d_deque_p->d_finish.blockPtr() + 1;
            delLast = d_boundary;
        }

        for (; delFirst != delLast; ++delFirst) {
            // Deallocate the block that '*d_start' points to.

            d_deque_p->deallocateN(*delFirst, 1);
        }
    }
}

// MANIPULATORS
template <class VALUE_TYPE, class ALLOCATOR>
void Deque_BlockCreator<VALUE_TYPE, ALLOCATOR>::insertAtFront(size_type n)
{
    d_boundary = reserveBlockSlots(n, true);
    for ( ; n > 0; --n) {
        d_boundary[-1] = d_deque_p->allocateN((Block*) 0, 1);
        --d_boundary;
    }
}

template <class VALUE_TYPE, class ALLOCATOR>
void Deque_BlockCreator<VALUE_TYPE, ALLOCATOR>::insertAtBack(size_type n)
{
    d_boundary = reserveBlockSlots(n, false);
    for ( ; n > 0; --n) {
        *d_boundary = d_deque_p->allocateN((Block*) 0, 1);
        ++d_boundary;
    }
}

template <class VALUE_TYPE, class ALLOCATOR>
typename Deque_BlockCreator<VALUE_TYPE, ALLOCATOR>::BlockPtr *
Deque_BlockCreator<VALUE_TYPE, ALLOCATOR>::reserveBlockSlots(
                                                        size_type numNewBlocks,
                                                        bool      atFront)
{
    BlockPtr *blocks = d_deque_p->d_blocks;
    size_type blocksLength = d_deque_p->d_blocksLength;

    BlockPtr *firstSlot = d_deque_p->d_start.blockPtr();
    BlockPtr *lastSlot = d_deque_p->d_finish.blockPtr() + 1;

    if (atFront) {
        if (d_boundary) {
            firstSlot = d_boundary;
        }
        if (size_type(firstSlot - blocks) >= numNewBlocks) {
            // Enough room to insert at front.

            return firstSlot;                                         // RETURN
        }
    }
    else {
        if (d_boundary) {
            lastSlot = d_boundary;
        }
        if (size_type(blocks + blocksLength - lastSlot) >= numNewBlocks) {
            // Enough room to insert at end.

            return lastSlot;                                          // RETURN
        }
    }

    BlockPtr  *newBlocks          = blocks;
    size_type  newBlocksLength    = blocksLength;
    size_type  numUsedBlocks      = lastSlot - firstSlot;
    size_type  blockOffsetStart   = d_deque_p->d_start.blockPtr() - firstSlot;
    size_type  numCommittedBlocks = (d_deque_p->d_finish.blockPtr() -
                                            d_deque_p->d_start.blockPtr() + 1);
    size_type  newNumUsedBlocks   = numUsedBlocks + numNewBlocks;

    if (newNumUsedBlocks > blocksLength) {
        const size_type newThreshold = newNumUsedBlocks +
                                                  2 * Imp::BLOCK_ARRAY_PADDING;
        while (newThreshold > newBlocksLength) {
            // Insufficient room.  Allocate new blocks array with geometric
            // growth.  Note that this should never overflow, because there are
            // at least 16 elements in each block, thus the requested block
            // array pointer will never be close to 'max_size() / 2'.

            newBlocksLength *= 2;
        }
        newBlocks = d_deque_p->allocateN((BlockPtr*)0, newBlocksLength);
    }

    // Center block pointers within new blocks array.

    BlockPtr *newFirstSlot = newBlocks +
                                      (newBlocksLength - newNumUsedBlocks) / 2;

    if (atFront) {
        newFirstSlot += numNewBlocks;
    }

    // Calculate offset for start and finish.  Need to do this before moving
    // around blocks.

    std::ptrdiff_t offsetStart  = d_deque_p->d_start.offsetInBlock();
    std::ptrdiff_t offsetFinish = d_deque_p->d_finish.offsetInBlock();

    // Move old block pointers into new position.

    std::memmove(newFirstSlot, firstSlot, numUsedBlocks * sizeof(BlockPtr));

    if (newBlocks != blocks) {
        // Deallocate old blocks array and install the new one.

        if (blocks) {
            d_deque_p->deallocateN(blocks, d_deque_p->d_blocksLength);
        }
        d_deque_p->d_blocks       = newBlocks;
        d_deque_p->d_blocksLength = newBlocksLength;
    }

    // Adjust start and finish iterators.

    d_deque_p->d_start.setBlock(newFirstSlot + blockOffsetStart);
    d_deque_p->d_start += offsetStart;
    d_deque_p->d_finish.setBlock(newFirstSlot + blockOffsetStart +
                                                       numCommittedBlocks - 1);
    d_deque_p->d_finish += offsetFinish;

    BlockPtr *ret = newFirstSlot;
    if (!atFront) {
        ret += numUsedBlocks;
    }

    return ret;
}

template <class VALUE_TYPE, class ALLOCATOR>
inline
void Deque_BlockCreator<VALUE_TYPE, ALLOCATOR>::release()
{
    d_boundary = 0;
}

                    // ---------------------------
                    // class bsl::Deque_ClearGuard
                    // ---------------------------

// CREATORS
template <class VALUE_TYPE, class ALLOCATOR>
inline
Deque_ClearGuard<VALUE_TYPE, ALLOCATOR>::Deque_ClearGuard(
                                         deque<VALUE_TYPE, ALLOCATOR> *deque_p)
: d_deque_p(deque_p)
{
}

template <class VALUE_TYPE, class ALLOCATOR>
inline
Deque_ClearGuard<VALUE_TYPE, ALLOCATOR>::~Deque_ClearGuard()
{
    if (d_deque_p) {
        d_deque_p->clear();
    }
}

// MANIPULATORS
template <class VALUE_TYPE, class ALLOCATOR>
inline
void Deque_ClearGuard<VALUE_TYPE, ALLOCATOR>::release()
{
    d_deque_p = 0;
}

                    // ----------------------
                    // class bsl::Deque_Guard
                    // ----------------------

// CREATORS
template <class VALUE_TYPE, class ALLOCATOR>
inline
Deque_Guard<VALUE_TYPE, ALLOCATOR>::Deque_Guard(
                                         deque<VALUE_TYPE, ALLOCATOR> *deque_p,
                                         bool                          isTail)
: d_deque_p(deque_p)
, d_count(0)
, d_isTail(isTail)
{
}

template <class VALUE_TYPE, class ALLOCATOR>
Deque_Guard<VALUE_TYPE, ALLOCATOR>::~Deque_Guard()
{
    if (d_count == 0) {
        return;                                                       // RETURN
    }
    BSLS_ASSERT_SAFE(d_count > 0);

    IteratorImp begin, end;

    if (d_isTail) {
        begin = d_deque_p->d_finish;
        end   = begin + d_count;
    }
    else {
        end   = d_deque_p->d_start;
        begin = end - d_count;
    }

    DequePrimitives::destruct(begin, end);
}

// MANIPULATORS
template <class VALUE_TYPE, class ALLOCATOR>
inline
std::size_t Deque_Guard<VALUE_TYPE, ALLOCATOR>::operator++()
{
    return ++d_count;
}

template <class VALUE_TYPE, class ALLOCATOR>
inline
std::size_t Deque_Guard<VALUE_TYPE, ALLOCATOR>::operator--()
{
    return --d_count;
}

template <class VALUE_TYPE, class ALLOCATOR>
inline
void Deque_Guard<VALUE_TYPE, ALLOCATOR>::release()
{
    d_count = 0;
}

// ACCESSORS
template <class VALUE_TYPE, class ALLOCATOR>
inline
std::size_t Deque_Guard<VALUE_TYPE, ALLOCATOR>::count() const
{
    return d_count;
}

template <class VALUE_TYPE, class ALLOCATOR>
inline
typename Deque_Guard<VALUE_TYPE, ALLOCATOR>::IteratorImp
Deque_Guard<VALUE_TYPE, ALLOCATOR>::begin() const
{
    return d_deque_p->d_start - d_count;
}

template <class VALUE_TYPE, class ALLOCATOR>
inline
typename Deque_Guard<VALUE_TYPE, ALLOCATOR>::IteratorImp
Deque_Guard<VALUE_TYPE, ALLOCATOR>::end() const
{
    return d_deque_p->d_finish + d_count;
}

}  // close namespace bsl

// ============================================================================
//                                TYPE TRAITS
// ============================================================================

// Type traits for STL *sequence* containers:
//: o A sequence container defines STL iterators.
//: o A sequence container is bitwise moveable if the allocator is bitwise
//:     moveable.
//: o A sequence container uses 'bslma' allocators if the parameterized
//:     'ALLOCATOR' is convertible from 'bslma::Allocator*'.

namespace BloombergLP {

namespace bslalg {

template <class VALUE_TYPE, class ALLOCATOR>
struct HasStlIterators<bsl::deque<VALUE_TYPE, ALLOCATOR> > : bsl::true_type
{};

}  // close namespace bslalg

namespace bslmf {

template <class VALUE_TYPE, class ALLOCATOR>
struct IsBitwiseMoveable<bsl::deque<VALUE_TYPE, ALLOCATOR> >
    : IsBitwiseMoveable<ALLOCATOR>
{};

}  // close namespace bslmf

namespace bslma {

template <class VALUE_TYPE, class ALLOCATOR>
struct UsesBslmaAllocator<bsl::deque<VALUE_TYPE, ALLOCATOR> >
    : bsl::is_convertible<Allocator*, ALLOCATOR>
{};

}  // close namespace bslma

}  // close enterprise namespace

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
