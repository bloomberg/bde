// bdec_voidptrqueue.h              -*-C++-*-
#ifndef INCLUDED_BDEC_VOIDPTRQUEUE
#define INCLUDED_BDEC_VOIDPTRQUEUE

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide an in-place double-ended queue of 'void *' values.
//
//@DEPRECATED: Use 'bsl::deque' instead.
//
//@CLASSES:
//   bdec_VoidPtrQueue: memory manager for in-place queue of 'void *' values
//
//@AUTHOR: Jeffrey Mendelsohn (jmendelsohn)
//
//@DESCRIPTION: This component implements an efficient, in-place, indexable,
// double-ended queue of 'void *' values.  The functionality of a 'bdec' queue
// is relatively rich; it is almost a proper superset of a vector, with
// efficient implementations of 'front', 'back', 'pushFront', and 'popBack'
// methods added.  The queue does not, however, provide a 'data' method (i.e.,
// direct access to the underlying memory), because its internal organization
// is not array-like.
//
// Typical usage involves pushing (appending) values to the back of the queue,
// popping (removing) values from the front of the queue, and retrieving
// (operator[]) values from a specified index; unlike the O[n] runtime cost for
// an 'insert(0, v)', however, a 'pushFront' has a constant average-case cost.
//
// Note that appending, inserting, removing or pushing (back and front)
// elements potentially alters the memory address of other element in the
// queue, and there is no guarantee of contiguous storage of consecutive queued
// elements.
//
///Abstract Representation
///-----------------------
// The logical organization of an indexable, in-place, double-ended
// 'bdec_VoidPtrQueue' object 'q' is shown below, along with an illustration of
// some of its most common methods:
//..
//                                    QUEUE
//                 v = q.front()                          v = q.back()
//                  +------+------+------+------+--//--+------+
//   q.popFront() <-|      |      |      |      |      |      |<- pushBack(v)
// q.pushFront(v) ->|      |      |      |      |      |      |-> popBack()
//                  +------+------+------+------+--//--+------+
//                    q[0]   q[1]                       q[n-1]
//                  <------------ n = q.length() --//--------->
//
//..
//
///Performance
///-----------
// The following characterizes the performance of representative operations
// using big-oh notation, O[f(N,M)], where the names 'N' and 'M' also refer to
// the number of respective elements in each container (i.e., its 'length()').
// Here the average case, A[f(N)], is the amortized cost, which is defined as
// the cost of 'N' successive invocations of the operation divided by 'N'.
//..
//     Operation           Worst Case          Average Case
//     ---------           ----------          ------------
//     DEFAULT CTOR        O[1]
//     COPY CTOR(N)        O[N]
//     N.DTOR()            O[1]
//     N.OP=(M)            O[M]
//     OP==(N,M)           O[min(N,M)]
//
//     N.pushFront(value)  O[N]                A[1]
//     N.pushBack(value)   O[N]                A[1]
//     N.popFront()        O[1]
//     N.popBack()         O[1]
//
//     N.append(value)     O[N]                A[1]
//     N.insert(value)     O[N]
//     N.replace(value)    O[1]
//     N.remove(index)     O[N]
//
//     N.OP[]()            O[1]
//     N.length()          O[1]
//..
///Usage
///-----
// The following snippets of code illustrate how to create and use a queue.
// First, create an empty 'bdec_VoidPtrQueue' 'q' and populate it with two
// elements 'E1' and 'E2'.
//..
//      void *E1 = (void *)0x100;
//      void *E2 = (void *)0x200;
//
//      bdec_VoidPtrQueue q;              assert( 0 == q.length());
//
//      q.append(E1);                   assert( 1 == q.length());
//                                      assert(E1 == q[0]);
//                                      assert(E1 == q.front());
//                                      assert(E1 == q.back());
//
//      q.append(E2);                   assert( 2 == q.length());
//                                      assert(E1 == q[0]);
//                                      assert(E2 == q[1]);
//                                      assert(E1 == q.front());
//                                      assert(E2 == q.back());
//..
// Now, pop the first element ('E1') from 'q' and push the same value to the
// front of the queue.
//..
//      q.popFront();                   assert( 1 == q.length());
//                                      assert(E2 == q[0]);
//                                      assert(E2 == q.front());
//                                      assert(E2 == q.back());
//
//      q.pushFront(E1);                assert( 2 == q.length());
//                                      assert(E1 == q[0]);
//                                      assert(E2 == q[1]);
//                                      assert(E1 == q.front());
//                                      assert(E2 == q.back());
//..
// Then, pop the last element ('E2') from the back of 'q' and push a new value
// 'E3' at the end of the queue.
//..
//      void *E3 = (void *)0x300;
//
//      q.popBack();                    assert( 1 == q.length());
//                                      assert(E1 == q[0]);
//                                      assert(E1 == q.front());
//                                      assert(E1 == q.back());
//
//      q.pushBack(E3);                 assert( 2 == q.length());
//                                      assert(E1 == q[0]);
//                                      assert(E3 == q[1]);
//                                      assert(E1 == q.front());
//                                      assert(E3 == q.back());
//..
// Now, assign 'E2' to the first element (index position 0) of 'q'.
//..
//      q[0] = E2;                      assert( 2 == q.length());
//                                      assert(E2 == q[0]);
//                                      assert(E3 == q[1]);
//
//..
// Then, insert a new value in the middle (index position 1) of 'q'.
//..
//      void *E4 = (void *)0x400;
//
//      q.insert(1, E4);                assert( 3 == q.length());
//                                      assert(E2 == q[0]);
//                                      assert(E4 == q[1]);
//                                      assert(E3 == q[2]);
//..
// Next, iterate over the elements in 'q', printing them in increasing order of
// their index positions, [0 .. q.length() - 1],
//..
//      bsl::cout << '[';
//      int len = q.length();
//      for (int i = 0; i < len; ++i) {
//          bsl::cout << ' ' << q[i];
//      }
//      bsl::cout << " ]" << bsl::endl;
//
//..
// which produces the following output on 'stdout':
//..
//      [ 0x200 0x400 0x300 ]
//..
// Finally, remove the elements from queue 'q'.
//..
//      q.remove(2);                    assert( 2 == q.length());
//                                      assert(E2 == q[0]);
//                                      assert(E4 == q[1]);
//
//      q.remove(0);                    assert( 1 == q.length());
//                                      assert(E4 == q[0]);
//
//      q.remove(0);                    assert( 0 == q.length());
//
//..
// Note that, in general, specifying index positions greater than or equal to
// length() will result in undefined behavior.

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITS
#include <bslalg_typetraits.h>
#endif

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif

namespace BloombergLP {

class bslma_Allocator;

                          // ======================
                          // class bdec_VoidPtrQueue
                          // ======================

class bdec_VoidPtrQueue {
    // This class implements an efficient, in-place double-ended queue of
    // 'void *' values.  The physical capacity of this queue may grow, but
    // never shrinks.  Capacity may be reserved initially via a constructor, or
    // at any time thereafter by using the 'reserveCapacity' and
    // 'reserveCapacityRaw' methods.  Note that there is no guarantee of
    // contiguous storage of consecutive elements.
    //
    // More generally, this container class supports a complete set of *value*
    // *semantics* operations, including copy construction, assignment,
    // equality comparison, 'ostream' printing, and 'bdex' serialization.  (A
    // precise operational definition of when two instances have the same value
    // can be found in the description of 'operator==' for the class.)  This
    // container is *exception* *neutral* with no guarantee of rollback: if an
    // exception is thrown during the invocation of a method on a pre-existing
    // instance, the container is left in a valid state, but its value is
    // undefined.  In no event is memory leaked.  Finally, *aliasing* (e.g.,
    // using all or part of an object as both source and destination) is
    // supported in all cases.

    void           **d_array_p;// dynamically allocated array (d_size elements)
    int              d_size;   // physical capacity of this array (in elements)
    int              d_front;  // index of element before first stored element
    int              d_back;   // index of element past last stored element
    bslma_Allocator *d_allocator_p; // holds (but not own) memory allocator

    friend bool operator==(const bdec_VoidPtrQueue& lhs,
                           const bdec_VoidPtrQueue& rhs);

    // PRIVATE MANIPULATORS
    void increaseSize();
        // Increase the physical capacity of this array by at least one
        // element.

public:
    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(bdec_VoidPtrQueue,
                                 bslalg_TypeTraitUsesBslmaAllocator);

    // TYPES
    struct InitialCapacity {
        // Enable uniform use of an optional integral constructor argument to
        // specify the initial internal capacity (in elements).  For example,
        //..
        //   bdec_VoidPtrQueue x(bdec_VoidPtrQueue::InitialCapacity(8));
        //..
        // defines an instance 'x' with an initial capacity of 8 elements, but
        // with a logical length of 0 elements.

        unsigned int d_i;
        ~InitialCapacity() { }
        explicit InitialCapacity(unsigned int i) : d_i(i) { }
    };

    // CREATORS
    explicit
    bdec_VoidPtrQueue(bslma_Allocator *basicAllocator = 0);
    explicit
    bdec_VoidPtrQueue(unsigned int     initialLength,
                      bslma_Allocator *basicAllocator = 0);
    bdec_VoidPtrQueue(unsigned int     initialLength,
                      void            *initialValue,
                      bslma_Allocator *basicAllocator = 0);
        // Create an in-place queue.  By default, the queue is empty.
        // Optionally specify the 'initialLength' of the queue.  Queue
        // elements are initialized with the specified 'initialValue', or to
        // 0.0 if 'initialValue' is not specified.  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.  The behavior is
        // undefined unless '0 <= initialLength'.

    explicit
    bdec_VoidPtrQueue(const InitialCapacity&  numElements,
                      bslma_Allocator        *basicAllocator = 0);
        // Create an in-place queue with sufficient initial capacity to
        // accommodate up to the specified 'numElements' values without
        // subsequent reallocation.  A valid reference returned by the
        // 'operator[]' method is guaranteed to remain valid unless 'length()'
        // exceeds 'numElements' (which would potentially cause a
        // reallocation).  Optionally specify the 'basicAllocator' used to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.  The behavior is undefined unless
        // '0 <= numElements'.

    bdec_VoidPtrQueue(void * const    *srcArray,
                      int              numElements,
                      bslma_Allocator *basicAllocator = 0);
        // Create an in-place queue initialized with the specified
        // 'numElements' leading values from the specified 'srcArray'.
        // Optionally specify the 'basicAllocator' used to supply memory.
        // If 'basicAllocator' is 0, the currently installed default allocator
        // is used.  The behavior is undefined unless '0 <= numElements'.  Note
        // that 'srcArray' must refer to sufficient memory to hold
        // 'numElements' values.

    bdec_VoidPtrQueue(const bdec_VoidPtrQueue&  original,
                      bslma_Allocator          *basicAllocator = 0);
        // Create an in-place queue initialized to the value of the specified
        // 'original' queue.  Optionally specify the 'basicAllocator' used to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.

    ~bdec_VoidPtrQueue();
        // Destroy this object.

    // MANIPULATORS
    bdec_VoidPtrQueue& operator=(const bdec_VoidPtrQueue& rhs);
        // Assign to this queue the value of the specified 'rhs' queue and
        // return a reference to this modifiable queue.

    void *& operator[](int index);
        // Return a reference to the modifiable element at the specified
        // 'index' position in this queue.  The reference will remain valid as
        // long as this queue is not destroyed or modified (e.g., via 'insert',
        // 'remove', or 'append').  The behavior is undefined unless
        // '0 <= index < length()'.

    void append(void *item);
        // Append to the end of this queue the value of the specified 'item'.
        // Note that this function is a synonym for 'pushBack' and is logically
        // equivalent to (but generally more efficient than) the following:
        //..
        //   insert(length(), item)
        //..

    void append(const bdec_VoidPtrQueue& srcQueue);
        // Append to the end of this queue the sequence of values in the
        // specified 'srcQueue'.  Note that this function is logically
        // equivalent to the following:
        //..
        //   insert(length(), srcQueue)
        //..

    void append(const bdec_VoidPtrQueue& srcQueue,
                int                      srcIndex,
                int                      numElements);
        // Append to the end of this queue the specified 'numElements' value in
        // the specified 'srcQueue' starting at the specified index position
        // 'srcIndex'.  Note that this function is logically equivalent to the
        // following:
        //..
        //   insert(length(), srcQueue, srcIndex, numElements)
        //..
        // The behavior is undefined unless '0 <= srcIndex',
        // '0 <= numElements', and
        // 'srcIndex + numElements <= srcQueue.length()'.

    void *& back();
        // Return a reference to the modifiable value at the back of this
        // queue.  The reference will remain valid as long as the queue is not
        // destroyed or modified (e.g., via 'insert', 'remove', or 'append').
        // The behavior is undefined if the queue is empty.  Note that this
        // function is logically equivalent to the following:
        //..
        //    operator[](length() - 1)
        //..

    void *& front();
        // Return a reference to the modifiable value at the front of this
        // queue.  The reference will remain valid as long as the queue is not
        // destroyed or modified (e.g., via 'insert', 'remove', or 'append').
        // The behavior is undefined if the queue is empty.  Note that this
        // function is logically equivalent to the following:
        //..
        //    operator[](0)
        //..

    void insert(int dstIndex, void *item);
        // Insert the specified 'item' into this queue at the specified
        // 'dstIndex'.  All current values with indices at or above 'dstIndex'
        // are shifted up by one index position.  The behavior is undefined
        // unless '0 <= dstIndex <= length()'.

    void insert(int dstIndex, const bdec_VoidPtrQueue& srcQueue);
        // Insert the specified 'srcQueue' into this queue at the specified
        // 'dstIndex'.  All current values with indices at or above 'dstIndex'
        // are shifted up by 'srcQueue.length()' index positions.  The behavior
        // is undefined unless '0 <= dstIndex <= length()'.

    void insert(int                     dstIndex,
                const bdec_VoidPtrQueue& srcQueue,
                int                     srcIndex,
                int                     numElements);
        // Insert the specified 'numElements' values starting at the specified
        // 'srcIndex' position from the specified 'srcQueue' into this queue at
        // the specified 'dstIndex'.  All current values with indices at or
        // above 'dstIndex' are shifted up by 'numElements' index positions.
        // The behavior is undefined unless '0 <= dstIndex <= length()',
        // '0 <= srcIndex', '0 <= numElements', and
        // 'srcIndex + numElements <= srcQueue.length()'.

    void popBack();
        // Remove the value from the back of this queue efficiently (in O[1]
        // time).  The behavior is undefined if this queue is empty.  Note that
        // this function is logically equivalent to (but more efficient than)
        // the following:
        //..
        //    remove(length() - 1)
        //..


    void popFront();
        // Remove the value from the front of this queue efficiently (in O[1]
        // time).  The behavior is undefined if this queue is empty.  Note that
        // this function is logically equivalent to (but more efficient than)
        // the following:
        //..
        //    remove(0)
        //..

    void pushBack(void *item);
        // Append the specified 'item' to the back of this queue efficiently
        // (in O[1] time when memory reallocation is not required).  Note that
        // this function is logically equivalent to (but generally more
        // efficient than) the following:
        //..
        //    insert(length(), item);
        //..

    void pushFront(void *item);
        // Insert the specified 'item' into the front of this queue efficiently
        // (in O[1] time when memory reallocation is not required).  Note that
        // this function is logically equivalent to (but generally more
        // efficient than) the following:
        //..
        //    insert(0, item);
        //..

    void remove(int index);
        // Remove from this queue the value at the specified 'index'.  All
        // values with initial indices above 'index' are shifted down by one
        // index position.  The behavior is undefined unless
        // '0 <= index < length()'.

    void remove(int index, int numElements);
        // Remove from this queue, beginning at the specified 'index', the
        // specified 'numElements' values.  All values with initial indices at
        // or above index + numElements are shifted down by 'numElements' index
        // positions.  The behavior is undefined unless '0 <= index',
        // '0 <= numElements', and 'index + numElements <= length()'.

    void removeAll();
        // Remove all elements from this queue.  Note that 'length()' is now 0.

    void replace(int dstIndex, void *item);
        // Replace the element at the specified 'dstIndex' in this queue with
        // the specified 'item'.  The behavior is undefined unless
        // '0 <= dstIndex < length()'.  Note that this function is logically
        // equivalent to (but more efficient than the following):
        //..
        //    insert(dstIndex, item);
        //    remove(dstIndex + 1);
        //..

    void replace(int                     dstIndex,
                 const bdec_VoidPtrQueue& srcQueue,
                 int                     srcIndex,
                 int                     numElements);
        // Replace the specified 'numElements' values beginning at the
        // specified 'dstIndex' in this queue with values from the specified
        // 'srcQueue' beginning at the specified 'srcIndex'.  The behavior is
        // undefined unless '0 <= dstIndex', '0 <= numElements',
        // 'dstIndex + numElements <= length()', '0 <= srcIndex', and
        // 'srcIndex + numElements <= srcQueue.length()'.  Note that this
        // function is logically equivalent to (but more efficient than the
        // following):
        //..
        //    insert(dstIndex, srcQueue, srcIndex, numElements);
        //    remove(dstIndex + numElements, numElements);
        //..

    void reserveCapacity(int numElements);
        // Reserve sufficient internal capacity to accommodate up to the
        // specified 'numElements' values without subsequent reallocation.
        // Note that if numElements <= length(), this operation has no effect.

    void reserveCapacityRaw(int numElements);
        // Reserve sufficient and minimal internal capacity to accommodate up
        // to the specified 'numElements' values without subsequent
        // reallocation.  Beware, however, that repeated calls to this function
        // may invalidate bounds on runtime complexity otherwise guaranteed by
        // this container.  Note that if numElements <= length(), this
        // operation has no effect.

    void setLength(int newLength);
    void setLength(int newLength, void *initialValue);
        // Set the length of this queue to the specified 'newLength'.  If
        // 'newLength' is less than the current length, elements at index
        // positions at or above 'newLength' are removed.  Otherwise any new
        // elements (at or above the current length) are initialized to the
        // specified 'initialValue', or to 0.0 if 'initialValue' is not
        // specified.  The behavior is undefined unless '0 <= newLength'.

    void setLengthRaw(int newLength);
        // Set the length of this queue to the specified 'newLength'.  If
        // 'newLength' is less than the current length, elements at index
        // positions at or above 'newLength' are removed.  If 'newLength' is
        // equal to the current length, this function has no effect.  Otherwise
        // new elements at or above the current length are not initialized to
        // any value.

    void swap(int index1, int index2);
        // Swap efficiently the values at the specified indices 'index1' and
        // 'index2'.  The behavior is undefined unless '0 <= index1 < length()'
        // and '0 <= index2 < length()'.

    // ACCESSORS
    void * const & operator[](int index) const;
        // Return a reference to the non-modifiable element at the specified
        // 'index' position in this queue.  The reference will remain valid as
        // long as this queue is not destroyed or modified (e.g., via 'insert',
        // 'remove', or 'append').  The behavior is undefined unless
        // '0 <= index < length()'.

    void * const & back() const;
        // Return a reference to the non-modifiable element at the back of this
        // queue.  The reference will remain valid as long as this queue is not
        // destroyed or modified (e.g., via 'insert', 'remove', or 'append').
        // The behavior is undefined if this queue is empty.  Note that this
        // function is logically equivalent to the following:
        //..
        //    operator[](length() - 1)
        //..

    void * const & front() const;
        // Return a reference to the non-modifiable element at the front of
        // this queue.  The reference will remain valid as long as this queue
        // is not destroyed or modified (e.g., via 'insert', 'remove', or
        // 'append').  The behavior is undefined if this queue is empty.  Note
        // that this function is logically equivalent to the following:
        //..
        //    operator[](0)
        //..

    int length() const;
        // Return the number of elements in this queue.

    bsl::ostream& print(bsl::ostream& stream,
                   int      level,
                   int      spacesPerLevel) const;
        // Format the element values in this queue to the specified output
        // 'stream' at (the absolute value of) the specified indentation
        // 'level' using the specified 'spacesPerLevel' of indentation.  Making
        // 'level' negative suppress indentation for the first line only.  The
        // behavior is undefined unless '0 <= spacesPerLevel'.
};


// FREE OPERATORS
bool operator==(const bdec_VoidPtrQueue& lhs,
                const bdec_VoidPtrQueue& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' queues have the same
    // value, and 'false' otherwise.  Two queues have the same value if they
    // have the same length and the same element value at each respective index
    // position.

inline
bool operator!=(const bdec_VoidPtrQueue& lhs,
                const bdec_VoidPtrQueue& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' queues do not have the
    // same value, and 'false' otherwise.  Two queues do not have the same
    // value if they have different lengths or differ in at least one index
    // position.

bsl::ostream& operator<<(bsl::ostream& stream, const bdec_VoidPtrQueue& queue);
    // Write the specified 'queue' to the specified output 'stream' in some
    // reasonable (single-line) format, and return a reference to the
    // modifiable 'stream'.

// ===========================================================================
//                        INLINE FUNCTION DEFINITIONS
// ===========================================================================


              // ---------------------------------------------
              // inlined methods used by other inlined methods
              // ---------------------------------------------

inline
int bdec_VoidPtrQueue::length() const
{
    return (d_back > d_front) ? d_back - d_front - 1
                              : d_back + d_size - d_front - 1;
}



                               // ------------
                               // MANIPULATORS
                               // ------------

inline
void *& bdec_VoidPtrQueue::operator[](int index)
{
    return d_array_p[(index + d_front + 1) % d_size];
}

inline
void *& bdec_VoidPtrQueue::back()
{
    return d_array_p[(d_back - 1 + d_size) % d_size];
}

inline
void *& bdec_VoidPtrQueue::front()
{
    return d_array_p[(d_front + 1) % d_size];
}

inline
void bdec_VoidPtrQueue::insert(int                     dstIndex,
                              const bdec_VoidPtrQueue& srcQueue)
{
    insert(dstIndex, srcQueue, 0, srcQueue.length());
}

inline
void bdec_VoidPtrQueue::popBack()
{
    d_back = (d_back - 1 + d_size) % d_size;
}

inline
void bdec_VoidPtrQueue::popFront()
{
    d_front = (d_front + 1) % d_size;
}

inline
void bdec_VoidPtrQueue::pushBack(void *item)
{
    int newBack = (d_back + 1) % d_size;
    if (d_front == newBack) {
        increaseSize();  // NOTE: this can change the value of d_back
        newBack = (d_back + 1) % d_size;
    }
    d_array_p[d_back] = item;
    d_back = newBack;
}

inline
void bdec_VoidPtrQueue::pushFront(void *item)
{
    int newFront = (d_front - 1 + d_size) % d_size;
    if (newFront == d_back) {
        increaseSize();  // NOTE: this can change the value of d_front
        newFront = (d_front - 1 + d_size) % d_size;
    }
    d_array_p[d_front] = item;
    d_front = newFront;
}

inline
void bdec_VoidPtrQueue::append(void *item)
{
    pushBack(item);
}

inline
void bdec_VoidPtrQueue::removeAll()
{
    d_back = (d_front + 1) % d_size;
}

inline
void bdec_VoidPtrQueue::replace(int dstIndex, void *item)
{
    d_array_p[(d_front + 1 + dstIndex) % d_size] = item;
}

inline
void bdec_VoidPtrQueue::swap(int index1, int index2)
{
    const int tmp = d_front + 1;
    const int i1 = (tmp + index1) % d_size;
    const int i2 = (tmp + index2) % d_size;

    void *temp = d_array_p[i1];
    d_array_p[i1] = d_array_p[i2];
    d_array_p[i2] = temp;
}



                                 // ---------
                                 // ACCESSORS
                                 // ---------

inline
void * const & bdec_VoidPtrQueue::operator[](int index) const
{
    return d_array_p[(index + d_front + 1) % d_size];
}

inline void * const & bdec_VoidPtrQueue::back() const
{
    return d_array_p[(d_back - 1 + d_size) % d_size];
}

inline void * const & bdec_VoidPtrQueue::front() const
{
    return d_array_p[(d_front + 1) % d_size];
}



                              // --------------
                              // FREE OPERATORS
                              // --------------

inline
bool operator!=(const bdec_VoidPtrQueue& lhs, const bdec_VoidPtrQueue& rhs)
{
    return !(lhs == rhs);
}

}  // close namespace BloombergLP



#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2005
//      All Rights Reserved.
//      Property of Bloomberg L.P.  (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
