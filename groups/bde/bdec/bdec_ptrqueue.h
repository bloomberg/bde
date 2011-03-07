// bdec_ptrqueue.h                                                    -*-C++-*-
#ifndef INCLUDED_BDEC_PTRQUEUE
#define INCLUDED_BDEC_PTRQUEUE

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide an in-place double-ended queue of 'T *' values.
//
//@DEPRECATED: Use 'bsl::deque' instead.
//
//@CLASSES:
//   bdec_PtrQueue: memory manager for in-place queue of 'T *' values
//
//@AUTHOR: Jeffrey Mendelsohn (jmendelsohn)
//
//@DESCRIPTION: This component implements an efficient, in-place, indexable,
// double-ended queue of 'T *' values.  The functionality of a 'bdec' queue
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
// 'bdec_PtrQueue' object 'q' is shown below, along with an illustration of
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
// First, create an empty 'bdec_PtrQueue' 'q' and populate it with two
// elements 'E1' and 'E2'.
//..
//      void *E1 = (void *)0x100;
//      void *E2 = (void *)0x200;
//
//      bdec_PtrQueue<void> q;          assert( 0 == q.length());
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
// their index positions, [0 - q.length() - 1],
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

#ifndef INCLUDED_BDEC_VOIDPTRQUEUE
#include <bdec_voidptrqueue.h>
#endif

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif


namespace BloombergLP {

class bslma_Allocator;



                          // ===================
                          // class bdec_PtrQueue
                          // ===================

template <class T>
class bdec_PtrQueue {
    // This class implements an efficient, in-place double-ended queue of
    // 'T *' values.  The physical capacity of this queue may grow, but
    // never shrinks.  Capacity may be reserved initially via a constructor, or
    // at any time thereafter by using the 'reserveCapacity' and
    // 'reserveCapacityRaw' methods.  Note that there is no guarantee of
    // contiguous storage of consecutive elements.
    //
    // More generally, this container class supports a complete set of *value*
    // *semantics* operations, including copy construction, assignment,
    // equality comparison, and 'ostream' printing.  (A precise operational
    // definition of when two instances have the same value can be found in the
    // description of 'operator==' for the class.)  This container is
    // *exception* *neutral* with no guarantee of rollback: if an exception is
    // thrown during the invocation of a method on a pre-existing instance, the
    // container is left in a valid state, but its value is undefined.  In no
    // event is memory leaked.  Finally, *aliasing* (e.g., using all or part of
    // an object as both source and destination) is supported in all cases.

    bdec_VoidPtrQueue d_queue;

public:
    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(bdec_PtrQueue,
                                 bslalg_TypeTraitUsesBslmaAllocator);

    // TYPES
    struct InitialCapacity {
        // Enable uniform use of an optional integral constructor argument to
        // specify the initial internal capacity (in elements).  For example,
        //..
        //   bdec_PtrQueue x(bdec_PtrQueue<T>::InitialCapacity(8));
        //..
        // defines an instance 'x' with an initial capacity of 8 elements, but
        // with a logical length of 0 elements.

        // DATA
        unsigned int d_i;

        // CREATORS
        explicit InitialCapacity(unsigned int i) : d_i(i) { }
        ~InitialCapacity() { }
    };

    // CREATORS
    explicit
    bdec_PtrQueue(bslma_Allocator *basicAllocator = 0);
    explicit
    bdec_PtrQueue(unsigned int     initialLength,
                  bslma_Allocator *basicAllocator = 0);
    bdec_PtrQueue(unsigned int     initialLength,
                  T               *initialValue,
                  bslma_Allocator *basicAllocator = 0);
        // Create an in-place queue.  By default, the queue is empty.
        // Optionally specify the 'initialLength' of the queue.  Queue elements
        // are initialized with the specified 'initialValue', or to 0.0 if
        // 'initialValue' is not specified.  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.  The behavior is
        // undefined unless '0 <= initialLength'.

    explicit
    bdec_PtrQueue(const InitialCapacity&  numElements,
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

    bdec_PtrQueue(T * const       *srcArray,
                  int              numElements,
                  bslma_Allocator *basicAllocator = 0);
        // Create an in-place queue initialized with the specified
        // 'numElements' leading values from the specified 'srcArray'.
        // Optionally specify the 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.  The behavior is undefined unless '0 <= numElements'.  Note
        // that 'srcArray' must refer to sufficient memory to hold
        // 'numElements' values.

    bdec_PtrQueue(const bdec_PtrQueue<T>&  original,
                      bslma_Allocator          *basicAllocator = 0);
        // Create an in-place queue initialized to the value of the specified
        // 'original' queue.  Optionally specify the 'basicAllocator' used to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.

    ~bdec_PtrQueue();
        // Destroy this object.

    // MANIPULATORS
    bdec_PtrQueue<T>& operator=(const bdec_PtrQueue<T>& rhs);
        // Assign to this queue the value of the specified 'rhs' queue and
        // return a reference to this modifiable queue.

    T *& operator[](int index);
        // Return a reference to the modifiable element at the specified
        // 'index' position in this queue.  The reference will remain valid as
        // long as this queue is not destroyed or modified (e.g., via 'insert',
        // 'remove', or 'append').  The behavior is undefined unless
        // '0 <= index < length()'.

    void append(T *item);
        // Append to the end of this queue the value of the specified 'item'.
        // Note that this function is a synonym for 'pushBack' and is logically
        // equivalent to (but generally more efficient than) the following:
        //..
        //   insert(length(), item)
        //..

    void append(const bdec_PtrQueue<T>& srcQueue);
        // Append to the end of this queue the sequence of values in the
        // specified 'srcQueue'.  Note that this function is logically
        // equivalent to the following:
        //..
        //   insert(length(), srcQueue)
        //..

    void append(const bdec_PtrQueue<T>& srcQueue,
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
        // '0 <= // numElements', and
        // 'srcIndex + numElements <= srcQueue.length()'.

    T *& back();
        // Return a reference to the modifiable value at the back of this
        // queue.  The reference will remain valid as long as the queue is not
        // destroyed or modified (e.g., via 'insert', 'remove', or 'append').
        // The behavior is undefined if the queue is empty.  Note that this
        // function is logically equivalent to the following:
        //..
        //    operator[](length() - 1)
        //..

    T *& front();
        // Return a reference to the modifiable value at the front of this
        // queue.  The reference will remain valid as long as the queue is not
        // destroyed or modified (e.g., via 'insert', 'remove', or 'append').
        // The behavior is undefined if the queue is empty.  Note that this
        // function is logically equivalent to the following:
        //..
        //    operator[](0)
        //..

    void insert(int dstIndex, T *item);
        // Insert the specified 'item' into this queue at the specified
        // 'dstIndex'.  All current values with indices at or above 'dstIndex'
        // are shifted up by one index position.  The behavior is undefined
        // unless '0 <= dstIndex <= length()'.

    void insert(int dstIndex, const bdec_PtrQueue<T>& srcQueue);
        // Insert the specified 'srcQueue' into this queue at the specified
        // 'dstIndex'.  All current values with indices at or above 'dstIndex'
        // are shifted up by 'srcQueue.length()' index positions.  The behavior
        // is undefined unless '0 <= dstIndex <= length()'.

    void insert(int                     dstIndex,
                const bdec_PtrQueue<T>& srcQueue,
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

    void pushBack(T *item);
        // Append the specified 'item' to the back of this queue efficiently
        // (in O[1] time when memory reallocation is not required).  Note that
        // this function is logically equivalent to (but generally more
        // efficient than) the following:
        //..
        //    insert(length(), item);
        //..

    void pushFront(T *item);
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
        // positions.  The behavior is undefined unless 0 <= index,
        // 0 <= numElements, and index + numElements <= length().

    void removeAll();
        // Remove all elements from this queue.  Note that 'length()' is now 0.

    void replace(int dstIndex, T *item);
        // Replace the element at the specified 'dstIndex' in this queue with
        // the specified 'item'.  The behavior is undefined unless
        // 0 <= dstIndex < length().  Note that this function is logically
        // equivalent to (but more efficient than the following):
        //..
        //    insert(dstIndex, item);
        //    remove(dstIndex + 1);
        //..

    void replace(int                     dstIndex,
                 const bdec_PtrQueue<T>& srcQueue,
                 int                     srcIndex,
                 int                     numElements);
        // Replace the specified 'numElements' values beginning at the
        // specified 'dstIndex' in this queue with values from the specified
        // 'srcQueue' beginning at the specified 'srcIndex'.  The behavior is
        // undefined unless 0 <= dstIndex, 0 <= numElements,
        // dstIndex + numElements <= length(), 0 <= srcIndex, and
        // srcIndex + numElements <= srcQueue.length().  Note that this
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
    void setLength(int newLength, T *initialValue);
        // Set the length of this queue to the specified 'newLength'.  If
        // 'newLength' is less than the current length, elements at index
        // positions at or above 'newLength' are removed.  Otherwise any new
        // elements (at or above the current length) are initialized to the
        // specified 'initialValue', or to 0.0 if 'initialValue' is not
        // specified.  The behavior is undefined unless 0 <= newLength.

    void setLengthRaw(int newLength);
        // Set the length of this queue to the specified 'newLength'.  If
        // 'newLength' is less than the current length, elements at index
        // positions at or above 'newLength' are removed.  If 'newLength' is
        // equal to the current length, this function has no effect.  Otherwise
        // new elements at or above the current length are not initialized to
        // any value.

    void swap(int index1, int index2);
        // Swap efficiently the values at the specified indices 'index1' and
        // 'index2'.  The behavior is undefined unless 0 <= index1 < length()
        // and 0 <= index2 < length().

    // ACCESSORS
    T * const & operator[](int index) const;
        // Return a reference to the non-modifiable element at the specified
        // 'index' position in this queue.  The reference will remain valid as
        // long as this queue is not destroyed or modified (e.g., via 'insert',
        // 'remove', or 'append').  The behavior is undefined unless
        // '0 <= index < length()'.

    T * const & back() const;
        // Return a reference to the non-modifiable element at the back of this
        // queue.  The reference will remain valid as long as this queue is not
        // destroyed or modified (e.g., via 'insert', 'remove', or 'append').
        // The behavior is undefined if this queue is empty.  Note that this
        // function is logically equivalent to the following:
        //..
        //    operator[](length() - 1)
        //..

    T * const & front() const;
        // Return a reference to the non-modifiable element at the front of
        // this queue.  The reference will remain valid as long as this queue
        // is not destroyed or modified (e.g., via 'insert', 'remove', or
        // 'append').  The behavior is undefined if this queue is empty.  Note
        // that this function is logically equivalent to the following:
        //..
        //    operator[](0)
        //..

    bool isEqual(const bdec_PtrQueue<T>& rhs) const;
        // Return 'true' if the this and 'rhs' queues have the same value,
        // and 'false' otherwise.  Two queues have the same value if they have
        // the same length and same element value at each respective index
        // position.  Called by the operator== free operator.

    bool isNotEqual(const bdec_PtrQueue<T>& rhs) const;
        // Return 'true' if the this and 'rhs' queues do not have the same
        // value, and 'false' otherwise.  Two queues do not have the same
        // value if they have different lengths or differ in at least one
        // index position.  Called by the operator!= free operator.

    int length() const;
        // Return the number of elements in this queue.

    bsl::ostream& ostreamOut(bsl::ostream& stream) const;
        // Write this queue to the specified output 'stream' in some
        // reasonable (single-line) format, and return a reference to 'stream'.
        // Called by the operator<< free operator.

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
template <class T>
inline
bool operator==(const bdec_PtrQueue<T>& lhs,
                const bdec_PtrQueue<T>& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' queues have the same
    // value, and 'false' otherwise.  Two queues have the same value if they
    // have the same length and the same element value at each respective
    // index position.

template <class T>
inline
bool operator!=(const bdec_PtrQueue<T>& lhs,
                const bdec_PtrQueue<T>& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' queues do not have the
    // same value, and 'false' otherwise.  Two queues do not have the same
    // value if they have different lengths or differ in at least one index
    // position.

template <class T>
inline
bsl::ostream& operator<<(bsl::ostream& stream, const bdec_PtrQueue<T>& queue);
    // Write the specified 'queue' to the specified output 'stream' in some
    // reasonable (single-line) format, and return a reference to the
    // modifiable 'stream'.

// ===========================================================================
//                        INLINE FUNCTION DEFINITIONS
// ===========================================================================

                                 // --------
                                 // CREATORS
                                 // --------

template <class T>
inline
bdec_PtrQueue<T>::bdec_PtrQueue(bslma_Allocator *basicAllocator)
: d_queue(basicAllocator)
{
}

template <class T>
inline
bdec_PtrQueue<T>::bdec_PtrQueue(unsigned int     initialLength,
                                bslma_Allocator *basicAllocator)
: d_queue(initialLength, basicAllocator)
{
}

template <class T>
inline
bdec_PtrQueue<T>::bdec_PtrQueue(unsigned int     initialLength,
                                T               *initialValue,
                                bslma_Allocator *basicAllocator)
: d_queue(initialLength, initialValue, basicAllocator)
{
}

template <class T>
inline
bdec_PtrQueue<T>::bdec_PtrQueue(const InitialCapacity&  numElements,
                                bslma_Allocator        *basicAllocator)
: d_queue(bdec_VoidPtrQueue::InitialCapacity(numElements.d_i), basicAllocator)
{
}

template <class T>
inline
bdec_PtrQueue<T>::bdec_PtrQueue(T * const       *srcArray,
                                int              numElements,
                                bslma_Allocator *basicAllocator)
: d_queue(srcArray, numElements, basicAllocator)
{
}

template <class T>
inline
bdec_PtrQueue<T>::bdec_PtrQueue(const bdec_PtrQueue<T>&  original,
                                bslma_Allocator         *basicAllocator)
: d_queue(original.d_queue, basicAllocator)
{
}

template <class T>
inline
bdec_PtrQueue<T>::~bdec_PtrQueue()
{
}



                               // ------------
                               // MANIPULATORS
                               // ------------

template <class T>
inline
bdec_PtrQueue<T>& bdec_PtrQueue<T>::operator=(const bdec_PtrQueue<T>& rhs)
{
    d_queue = rhs.d_queue;
    return *this;
}

template <class T>
inline
T *& bdec_PtrQueue<T>::operator[](int index)
{
    return (T *&)d_queue[index];
}

template <class T>
inline
T *& bdec_PtrQueue<T>::back()
{
    return (T *&)d_queue.back();
}

template <class T>
inline
T *& bdec_PtrQueue<T>::front()
{
    return (T *&)d_queue.front();
}

template <class T>
inline
void bdec_PtrQueue<T>::popBack()
{
    d_queue.popBack();
}

template <class T>
inline
void bdec_PtrQueue<T>::popFront()
{
    d_queue.popFront();
}

template <class T>
inline
void bdec_PtrQueue<T>::pushBack(T *item)
{
    d_queue.pushBack(item);
}

template <class T>
inline
void bdec_PtrQueue<T>::pushFront(T *item)
{
    d_queue.pushFront(item);
}

template <class T>
inline
void bdec_PtrQueue<T>::append(T *item)
{
    d_queue.pushBack(item);
}

template <class T>
inline
void bdec_PtrQueue<T>::append(const bdec_PtrQueue<T>& srcQueue)
{
    d_queue.append(srcQueue.d_queue);
}

template <class T>
inline
void bdec_PtrQueue<T>::append(const bdec_PtrQueue<T>& srcQueue,
                              int                     srcIndex,
                              int                     numElements)
{
    d_queue.append(srcQueue.d_queue, srcIndex, numElements);
}

template <class T>
inline
void bdec_PtrQueue<T>::insert(int dstIndex, T *item)
{
    d_queue.insert(dstIndex, item);
}

template <class T>
inline
void bdec_PtrQueue<T>::insert(int dstIndex, const bdec_PtrQueue<T>& srcQueue)
{
    d_queue.insert(dstIndex, srcQueue.d_queue, 0, srcQueue.length());
}

template <class T>
inline
void bdec_PtrQueue<T>::insert(int                     dstIndex,
                              const bdec_PtrQueue<T>& srcQueue,
                              int                     srcIndex,
                              int                     numElements)
{
    d_queue.insert(dstIndex, srcQueue.d_queue, srcIndex, numElements);
}

template <class T>
inline
void bdec_PtrQueue<T>::remove(int index)
{
    d_queue.remove(index);
}

template <class T>
inline
void bdec_PtrQueue<T>::remove(int index, int numElements)
{
    d_queue.remove(index, numElements);
}

template <class T>
inline
void bdec_PtrQueue<T>::removeAll()
{
    d_queue.removeAll();
}

template <class T>
inline
void bdec_PtrQueue<T>::replace(int dstIndex, T *item)
{
    d_queue.replace(dstIndex, item);
}

template <class T>
inline
void bdec_PtrQueue<T>::replace(int                     dstIndex,
                               const bdec_PtrQueue<T>& srcQueue,
                               int                     srcIndex,
                               int                     numElements)
{
    d_queue.replace(dstIndex, srcQueue.d_queue, srcIndex, numElements);
}

template <class T>
inline
void bdec_PtrQueue<T>::reserveCapacity(int numElements)
{
    d_queue.reserveCapacity(numElements);
}

template <class T>
inline
void bdec_PtrQueue<T>::reserveCapacityRaw(int numElements)
{
    d_queue.reserveCapacityRaw(numElements);
}

template <class T>
inline
void bdec_PtrQueue<T>::setLength(int newLength)
{
    d_queue.setLength(newLength);
}

template <class T>
inline
void bdec_PtrQueue<T>::setLength(int newLength, T *initialValue)
{
    d_queue.setLength(newLength, initialValue);
}

template <class T>
inline
void bdec_PtrQueue<T>::setLengthRaw(int newLength)
{
    d_queue.setLengthRaw(newLength);
}

template <class T>
inline
void bdec_PtrQueue<T>::swap(int index1, int index2)
{
    d_queue.swap(index1, index2);
}



                                 // ---------
                                 // ACCESSORS
                                 // ---------

template <class T>
inline
T * const & bdec_PtrQueue<T>::operator[](int index) const
{
    return (T * const &)d_queue[index];
}

template <class T>
inline
T * const & bdec_PtrQueue<T>::back() const
{
    return (T * const &)d_queue.back();
}

template <class T>
inline
T * const & bdec_PtrQueue<T>::front() const
{
    return (T * const &)d_queue.front();
}

template <class T>
inline
bool bdec_PtrQueue<T>::isEqual(const bdec_PtrQueue<T>& rhs) const
{
    return d_queue == rhs.d_queue;
}

template <class T>
inline
bool bdec_PtrQueue<T>::isNotEqual(const bdec_PtrQueue<T>& rhs) const
{
    return d_queue != rhs.d_queue;
}

template <class T>
inline
int bdec_PtrQueue<T>::length() const
{
    return d_queue.length();
}

template <class T>
inline
bsl::ostream& bdec_PtrQueue<T>::ostreamOut(bsl::ostream& stream) const
{
    return stream << d_queue;
}

template <class T>
inline
bsl::ostream& bdec_PtrQueue<T>::print(bsl::ostream& stream,
                                 int      level,
                                 int      spacesPerLevel) const
{
    return d_queue.print(stream, level, spacesPerLevel);
}


                              // --------------
                              // FREE OPERATORS
                              // --------------

template <class T>
inline
bool operator==(const bdec_PtrQueue<T>& lhs, const bdec_PtrQueue<T>& rhs)
{
    return lhs.isEqual(rhs);
}

template <class T>
inline
bool operator!=(const bdec_PtrQueue<T>& lhs, const bdec_PtrQueue<T>& rhs)
{
    return lhs.isNotEqual(rhs);
}

template <class T>
inline
bsl::ostream& operator<<(bsl::ostream& stream, const bdec_PtrQueue<T>& rhs)
{
    return rhs.ostreamOut(stream);
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
