// bdlc_queue.h                                                       -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BDLC_QUEUE
#define INCLUDED_BDLC_QUEUE

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide an in-place double-ended queue of 'T' values.
//
//@DEPRECATED: Use 'bsl::deque' instead.
//
//@CLASSES:
//   bdlc::Queue: memory manager for in-place queue of 'T' values
//
//@DESCRIPTION: This component implements an efficient, in-place, indexable,
// double-ended queue of 'T' values, where 'T' is a templatized, user-defined
// type.  The functionality of a 'bdlc::Queue' is relatively rich; it is almost
// a proper superset of a vector, with efficient implementations of 'front',
// 'back', 'pushFront', and 'popBack' methods added.  However, the queue does
// *not* provide a 'data' method (yielding direct access to the underlying
// memory), because its internal organization is not array-like.
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
// 'bdlc::Queue' object 'q' is shown below, along with an illustration of some
// of its most common methods:
//..
//                                    QUEUE
//                 v = q.front()                          v = q.back()
//                  +------+------+------+------+--//--+------+
//   q.popFront() <-|      |      |      |      |      |      |<- pushBack(v)
// q.pushFront(v) ->|      |      |      |      |      |      |-> popBack()
//                  +------+------+------+------+--//--+------+
//                    q[0]   q[1]                       q[n-1]
//                  <------------ n = q.length() --//--------->
//..
//
///Performance
///-----------
// The following characterizes the performance of representative operations
// using big-oh notation, O[f(N,M)], where the names 'N' and 'M' also refer to
// the number of respective elements in each container (i.e., its 'length').
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
//
///Usage
///-----
// The following snippets of code illustrate how to create and use a queue.
// First, create an empty 'bdlc::Queue<double>' 'q' and populate it with two
// elements 'E1' and 'E2'.
//..
//      const double E1 = 100.01;
//      const double E2 = 200.02;
//
//      bdlc::Queue<double> q;          assert( 0 == q.length());
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
//      const double E3 = 300.03;
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
//..
// Then, insert a new value in the middle (index position 1) of 'q'.
//..
//      const double E4 = 400.04;
//
//      q.insert(1, E4);                assert( 3 == q.length());
//                                      assert(E2 == q[0]);
//                                      assert(E4 == q[1]);
//                                      assert(E3 == q[2]);
//..
// Next, iterate over the elements in 'q', printing them in increasing order of
// their index positions, '[0 .. q.length() - 1]',
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
//      [ 200.02 400.04 300.03 ]
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

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITS
#include <bslalg_typetraits.h>
#endif

#ifndef INCLUDED_BSLX_INSTREAMFUNCTIONS
#include <bslx_instreamfunctions.h>
#endif

#ifndef INCLUDED_BSLX_OUTSTREAMFUNCTIONS
#include <bslx_outstreamfunctions.h>
#endif

#ifndef INCLUDED_BDLB_PRINT
#include <bdlb_print.h>
#endif

#ifndef INCLUDED_BDLB_PRINTMETHODS
#include <bdlb_printmethods.h>
#endif

#ifndef INCLUDED_BSLMA_DEFAULT
#include <bslma_default.h>
#endif

#ifndef INCLUDED_BSL_CSTRING
#include <bsl_cstring.h>        // memmove(), memcmp(), memcpy()
#endif

#ifndef INCLUDED_BSL_OSTREAM
#include <bsl_ostream.h>
#endif

#ifndef INCLUDED_BSL_NEW
#include <bsl_new.h>
#endif

namespace BloombergLP {

namespace bdlc {
                                // ===========
                                // class Queue
                                // ===========

template <class T>
class Queue {
    // This class implements an efficient, in-place double-ended queue of
    // values of parameterized type 'T'.  The physical capacity of this queue
    // may grow, but never shrinks.  Capacity may be reserved initially via a
    // constructor, or at any time thereafter by using the 'reserveCapacity'
    // and 'reserveCapacityRaw' methods.  Note that there is no guarantee of
    // contiguous storage of consecutive elements.
    //
    // More generally, this container class supports a complete set of *value*
    // *semantics* operations, including copy construction, assignment,
    // equality comparison, 'ostream' printing, and 'bdex' serialization.  (A
    // precise operational definition of when two objects have the same value
    // can be found in the description of 'operator==' for the class.)  This
    // container is *exception* *neutral* with no guarantee of rollback: if an
    // exception is thrown during the invocation of a method on a pre-existing
    // object, the container is left in a valid state, but its value is
    // undefined.  In no event is memory leaked.  Finally, *aliasing* (e.g.,
    // using all or part of an object as both source and destination) is
    // supported in all cases.

    // PRIVATE TYPES
    enum {
        // The queue is full when 'd_front == d_back'.  Hence, 'k_INITIAL_SIZE'
        // must be at least two.

        k_INITIAL_SIZE   = 2,  // initial physical capacity (in elements)
        k_GROW_FACTOR    = 2,  // multiplicative factor for growing 'd_size'
        k_EXTRA_CAPACITY = 2   // extra capacity needed by implementation
    };

  public:
    // TYPES
    struct InitialCapacity {
        // Enable uniform use of an optional integral constructor argument to
        // specify the initial internal capacity (in elements).  For example,
        //..
        //   Queue<unsigned int> x(Queue::InitialCapacity(8));
        //..
        // instantiates an object 'x' with an initial capacity of 8 elements,
        // but with a logical length of 0 elements.

        unsigned int d_i;

        // CREATORS
        explicit InitialCapacity(unsigned int i) : d_i(i) { }
        ~InitialCapacity() { }
    };

  private:
    // DATA
    T                *d_array_p;      // dynamically allocated array ('d_size'
                                      // elements)

    int               d_size;         // physical capacity of this array (in
                                      // elements)

    int               d_front;        // index of element before first stored
                                      // element

    int               d_back;         // index of element past last stored
                                      // element

    bslma::Allocator *d_allocator_p;  // holds (but not own) memory allocator

  private:
    // PRIVATE MANIPULATORS
    int calculateSufficientSize(int minLength, int size);
        // Grow geometrically the specified current 'size' value while it is
        // less than the specified 'minLength' value plus any additional
        // capacity required by the implementation (i.e., 'k_EXTRA_CAPACITY'
        // elements).  Return the new size value.  The behavior is undefined
        // unless 'k_INITIAL_SIZE <= size' and '0 <= minLength'.  Note that if
        // 'minLength + k_EXTRA_CAPACITY <= size' then 'size' is returned.

    int memcpyCircular(T       *dstArray,
                       int      dstSize,
                       int      dstIndex,
                       const T *srcArray,
                       int      srcSize,
                       int      srcIndex,
                       int      numElements);
        // Copy efficiently the specified 'numElements' data values from the
        // specified 'srcArray' of the specified 'srcSize' starting at the
        // specified 'srcIndex' into the specified 'dstArray' of the specified
        // 'dstSize' starting at the specified 'dstIndex'.  Return the new
        // value for the back of the queue.  The 'srcArray' and the 'dstArray'
        // are assumed to be queues; they are circular which implies copy may
        // have to be broken into multiple parts since the underlying array is
        // linear.  The behavior is undefined unless '0 <= dstSize',
        // '0 <= dstIndex < dstSize', '0 <= srcIndex < srcSize',
        // '0 <= numElements', 'numElements <= dstSize - k_EXTRA_CAPACITY', and
        // 'numElements <= srcSize - k_EXTRA_CAPACITY' (the 'k_EXTRA_CAPACITY'
        // accounts for the locations of 'd_front' and 'd_back').  Note that
        // aliasing is not handled properly.

    void memShiftLeft(T   *array,
                      int  size,
                      int  dstIndex,
                      int  srcIndex,
                      int  numElements);
        // Copy efficiently the specified 'numElements' data values from the
        // specified 'array' of the specified 'size' starting at the specified
        // index 'srcIndex' to the specified 'dstIndex' assuming the elements
        // are to be moved to the left (towards the front of the queue).
        // 'array' is assumed to be a queue; it is circular.  The behavior is
        // undefined unless '0 <= size', '0 <= dstIndex < size',
        // '0 <= srcIndex < size', and
        // '0 <= numElements <= size - k_EXTRA_CAPACITY'.  Note that this
        // function is alias safe.

    void memShiftRight(T   *array,
                       int  size,
                       int  dstIndex,
                       int  srcIndex,
                       int  numElements);
        // Copy efficiently the specified 'numElements' data values from the
        // specified 'array' of the specified 'size' starting at the specified
        // index 'srcIndex' to the specified 'dstIndex' assuming the elements
        // are to be moved to the right (towards the back of the queue).
        // 'array' is assumed to be a queue; it is circular.  The behavior is
        // undefined unless '0 <= size', '0 <= dstIndex < size',
        // '0 <= srcIndex < size', and
        // '0 <= numElements <= size - k_EXTRA_CAPACITY'.  Note that this
        // function is alias safe.

    void copyData(T       *dstArray,
                  int     *dstBack,
                  int      dstSize,
                  int      dstFront,
                  const T *srcArray,
                  int      srcSize,
                  int      srcFront,
                  int      srcBack);
        // Copy efficiently the queue indicated by the specified 'srcArray' of
        // the specified 'srcSize' with the specified 'srcFront' and the
        // specified 'srcBack' into the queue indicated by the specified
        // 'dstArray' of the specified 'dstSize', with the specified
        // 'dstFront'.  The specified 'dstBack' is set to make the length of
        // the destination queue the same as the length of the source queue.
        // The behavior is undefined unless '0 <= dstSize',
        // '0 <= dstFront < dstSize', '0 <= srcSize',
        // '0 <= srcFront < srcSize', and '0 <= srcBack < srcSize'.  Note that
        // aliasing is not handled properly.

    int increaseSizeImp(T                **addrArray,
                        int               *front,
                        int               *back,
                        int                newSize,
                        int                size,
                        bslma::Allocator  *allocator);
        // Increase the physical capacity of the queue represented by the
        // specified 'addrArray' to specified 'newSize' from the specified
        // 'size'.  Return the new size of the queue.  This function copies the
        // data contained within the queue between the specified 'front' and
        // 'back' to the new queue and update the values of both 'front' and
        // 'back'.  Use the specified 'allocator' to supply and retrieve
        // memory.  The behavior is undefined unless
        // 'k_INITIAL_SIZE <= newSize', 'k_INITIAL_SIZE <= size',
        // 'size <= newSize', '0 <= *front < size', and '0 <= *back < size'.

    void increaseSize();
        // Increase the physical capacity of this array by at least one
        // element.

  public:
    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS2(Queue,
                                  bslalg::TypeTraitUsesBslmaAllocator,
                                  bdlb::TypeTraitHasPrintMethod);

    // CLASS METHODS
    static int maxSupportedBdexVersion(int versionSelector);
        // Return the maximum valid BDEX format version, as indicated by the
        // specified 'versionSelector', to be passed to the 'bdexStreamOut'
        // method.  Note that it is highly recommended that 'versionSelector'
        // be formatted as "YYYYMMDD", a date representation.  Also note that
        // 'versionSelector' should be a *compile*-time-chosen value that
        // selects a format version supported by both externalizer and
        // unexternalizer.  See the 'bslx' package-level documentation for more
        // information on BDEX streaming of value-semantic types and
        // containers.

    // CREATORS
    explicit
    Queue(bslma::Allocator *basicAllocator = 0);
    explicit
    Queue(unsigned int      initialLength,
          bslma::Allocator *basicAllocator = 0);
    Queue(int               initialLength,
          const T&          initialValue,
          bslma::Allocator *basicAllocator = 0);
        // Create an in-place queue.  By default, the queue is empty.
        // Optionally specify the 'initialLength' of the queue.  Queue elements
        // are initialized with the specified 'initialValue', or to 0.0 if
        // 'initialValue' is not specified.  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.  The behavior is
        // undefined unless '0 <= initialLength'.

    explicit
    Queue(const InitialCapacity&  numElements,
          bslma::Allocator       *basicAllocator = 0);
        // Create an in-place queue with sufficient initial capacity to
        // accommodate up to the specified 'numElements' values without
        // subsequent reallocation.  A valid reference returned by the
        // 'operator[]' method is guaranteed to remain valid unless the value
        // returned by the 'length' method exceeds 'numElements' (which would
        // potentially cause a reallocation).  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.  The behavior is
        // undefined unless '0 <= numElements'.

    Queue(const T          *srcArray,
          int               numElements,
          bslma::Allocator *basicAllocator = 0);
        // Create an in-place queue initialized with the specified
        // 'numElements' leading values from the specified 'srcArray'.
        // Optionally specify the 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.  The behavior is undefined unless '0 <= numElements'.  Note
        // that 'srcArray' must refer to sufficient memory to hold
        // 'numElements' values.

    Queue(const Queue& original, bslma::Allocator* basicAllocator = 0);
        // Create an in-place queue initialized to the value of the specified
        // 'original' queue.  Optionally specify the 'basicAllocator' used to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.

    ~Queue();
        // Destroy this object.

    // MANIPULATORS
    Queue& operator=(const Queue& rhs);
        // Assign to this queue the value of the specified 'rhs' queue and
        // return a reference to this modifiable queue.

    T& operator[](int index);
        // Return a reference to the modifiable element at the specified
        // 'index' position in this queue.  The reference will remain valid as
        // long as this queue is not destroyed or modified (e.g., via 'insert',
        // 'remove', or 'append').  The behavior is undefined unless
        // '0 <= index < length()'.

    void append(const T& item);
        // Append to the end of this queue the value of the specified 'item'.
        // Note that this function is a synonym for 'pushBack' and is logically
        // equivalent to (but generally more efficient than):
        //..
        //   insert(length(), item);
        //..

    void append(const Queue& srcQueue);
        // Append to the end of this queue the sequence of values in the
        // specified 'srcQueue'.  Note that this function is logically
        // equivalent to:
        //..
        //   insert(length(), srcQueue);
        //..

    void append(const Queue& srcQueue, int srcIndex, int numElements);
        // Append to the end of this queue the specified 'numElements' value in
        // the specified 'srcQueue' starting at the specified index position
        // 'srcIndex'.  Note that this function is logically equivalent to:
        //..
        //   insert(length(), srcQueue, srcIndex, numElements);
        //..
        // The behavior is undefined unless '0 <= srcIndex',
        // '0 <= numElements', and
        // 'srcIndex + numElements <= srcQueue.length()'.

    T& back();
        // Return a reference to the modifiable value at the back of this
        // queue.  The reference will remain valid as long as the queue is not
        // destroyed or modified (e.g., via 'insert', 'remove', or 'append').
        // The behavior is undefined if the queue is empty.  Note that this
        // function is logically equivalent to:
        //..
        //  operator[](length() - 1)
        //..

    T& front();
        // Return a reference to the modifiable value at the front of this
        // queue.  The reference will remain valid as long as the queue is not
        // destroyed or modified (e.g., via 'insert', 'remove', or 'append').
        // The behavior is undefined if the queue is empty.  Note that this
        // function is logically equivalent to:
        //..
        //  operator[](0)
        //..

    void insert(int dstIndex, const T& item);
        // Insert the specified 'item' into this queue at the specified
        // 'dstIndex'.  All current values with indices at or above 'dstIndex'
        // are shifted up by one index position.  The behavior is undefined
        // unless '0 <= dstIndex <= length()'.

    void insert(int dstIndex, const Queue& srcQueue);
        // Insert the specified 'srcQueue' into this queue at the specified
        // 'dstIndex'.  All current values with indices at or above 'dstIndex'
        // are shifted up by 'srcQueue.length()' index positions.  The behavior
        // is undefined unless '0 <= dstIndex <= length()'.

    void insert(int          dstIndex,
                const Queue& srcQueue,
                int          srcIndex,
                int          numElements);
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
        // this function is logically equivalent to (but more efficient than):
        //..
        //    remove(length() - 1)
        //..

    void popFront();
        // Remove the value from the front of this queue efficiently (in O[1]
        // time).  The behavior is undefined if this queue is empty.  Note that
        // this function is logically equivalent to (but more efficient than):
        //..
        //    remove(0)
        //..

    void pushBack(const T& item);
        // Append the specified 'item' to the back of this queue efficiently
        // (in O[1] time when memory reallocation is not required).  Note that
        // this function is logically equivalent to (but generally more
        // efficient than):
        //..
        //    insert(length(), item);
        //..

    void pushFront(const T& item);
        // Insert the specified 'item' into the front of this queue efficiently
        // (in O[1] time when memory reallocation is not required).  Note that
        // this function is logically equivalent to (but generally more
        // efficient than):
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
        // or above 'index + numElements' are shifted down by 'numElements'
        // index positions.  The behavior is undefined unless '0 <= index',
        // '0 <= numElements', and 'index + numElements <= length()'.

    void removeAll(bsl::vector<T> *buffer = 0);
        // Remove all elements from this queue.  If the optionally specified
        // 'buffer' is not 0, append to 'buffer' a copy of each element removed
        // (in front-to-back order of the elements in the queue prior to the
        // invocation of this method).

    void replace(int dstIndex, const T& item);
        // Replace the element at the specified 'dstIndex' in this queue with
        // the specified 'item'.  The behavior is undefined unless
        // '0 <= dstIndex < length()'.  Note that this function is logically
        // equivalent to (but more efficient than):
        //..
        //    insert(dstIndex, item);
        //    remove(dstIndex + 1);
        //..

    void replace(int          dstIndex,
                 const Queue& srcQueue,
                 int          srcIndex,
                 int          numElements);
        // Replace the specified 'numElements' values beginning at the
        // specified 'dstIndex' in this queue with values from the specified
        // 'srcQueue' beginning at the specified 'srcIndex'.  The behavior is
        // undefined unless '0 <= dstIndex, 0 <= numElements',
        // 'dstIndex + numElements <= length()', '0 <= srcIndex', and
        // 'srcIndex + numElements <= srcQueue.length()'.  Note that this
        // function is logically equivalent to (but more efficient than):
        //..
        //    insert(dstIndex, srcQueue, srcIndex, numElements);
        //    remove(dstIndex + numElements, numElements);
        //..

    void reserveCapacity(int numElements);
        // Reserve sufficient internal capacity to accommodate up to the
        // specified 'numElements' values without subsequent reallocation.
        // Note that if 'numElements <= length()', this operation has no
        // effect.

    void reserveCapacityRaw(int numElements);
        // Reserve sufficient and minimal internal capacity to accommodate up
        // to the specified 'numElements' values without subsequent
        // reallocation.  Beware, however, that repeated calls to this function
        // may invalidate bounds on runtime complexity otherwise guaranteed by
        // this container.  Note that if 'numElements <= length()', this
        // operation has no effect.

    void setLength(int newLength);
    void setLength(int newLength, const T& initialValue);
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

    template <class STREAM>
    STREAM& bdexStreamIn(STREAM& stream, int version);
        // Assign to this object the value read from the specified input
        // 'stream' using the specified 'version' format, and return a
        // reference to 'stream'.  If 'stream' is initially invalid, this
        // operation has no effect.  If 'version' is not supported, this object
        // is unaltered and 'stream' is invalidated, but otherwise unmodified.
        // If 'version' is supported but 'stream' becomes invalid during this
        // operation, this object has an undefined, but valid, state.  Note
        // that no version is read from 'stream'.  See the 'bslx' package-level
        // documentation for more information on BDEX streaming of
        // value-semantic types and containers.

    void swap(int index1, int index2);
        // Swap efficiently the values at the specified indices 'index1' and
        // 'index2'.  The behavior is undefined unless '0 <= index1 < length()'
        // and '0 <= index2 < length()'.

    // ACCESSORS
    const T& operator[](int index) const;
        // Return a reference to the non-modifiable element at the specified
        // 'index' position in this queue.  The reference will remain valid as
        // long as this queue is not destroyed or modified (e.g., via 'insert',
        // 'remove', or 'append').  The behavior is undefined unless
        // '0 <= index < length()'.

    const T& back() const;
        // Return a reference to the non-modifiable element at the back of this
        // queue.  The reference will remain valid as long as this queue is not
        // destroyed or modified (e.g., via 'insert', 'remove', or 'append').
        // The behavior is undefined if this queue is empty.  Note that this
        // function is logically equivalent to:
        //..
        //    operator[](length() - 1)
        //..

    const T& front() const;
        // Return a reference to the non-modifiable element at the front of
        // this queue.  The reference will remain valid as long as this queue
        // is not destroyed or modified (e.g., via 'insert', 'remove', or
        // 'append').  The behavior is undefined if this queue is empty.  Note
        // that this function is logically equivalent to:
        //..
        //    operator[](0)
        //..

    int length() const;
        // Return the number of elements in this queue.

    bsl::ostream& print(bsl::ostream& stream,
                        int           level,
                        int           spacesPerLevel) const;
        // Format this object to the specified output 'stream' at the
        // optionally specified indentation 'level' and return a reference to
        // the modifiable 'stream'.  If 'level' is specified, optionally
        // specify 'spacesPerLevel', the number of spaces per indentation level
        // for this and all of its nested objects.  Each line is indented by
        // the absolute value of 'level * spacesPerLevel'.  If 'level' is
        // negative, suppress indentation of the first line.  If
        // 'spacesPerLevel' is negative, suppress line breaks and format the
        // entire output on one line.  If 'stream' is initially invalid, this
        // operation has no effect.  Note that a trailing newline is provided
        // in multi-line mode only.

    bsl::ostream& streamOut(bsl::ostream& stream) const
        // Write the elements of this queue out to the specified 'stream'.
        // Note that for this method to compile, 'operator<<' has to be defined
        // for arguments 'stream' and type 'T'.
    {
        stream << '[';
        for (int i = 0; i < length(); ++i) {
            stream << ' ' << (*this)[i];
        }
        return stream << " ]";
    }

    template <class STREAM>
    STREAM& bdexStreamOut(STREAM& stream, int version) const;
        // Write the value of this object, using the specified 'version'
        // format, to the specified output 'stream', and return a reference to
        // 'stream'.  If 'stream' is initially invalid, this operation has no
        // effect.  If 'version' is not supported, 'stream' is invalidated, but
        // otherwise unmodified.  Note that 'version' is not written to
        // 'stream'.  See the 'bslx' package-level documentation for more
        // information on BDEX streaming of value-semantic types and
        // containers.

};

// FREE OPERATORS
template <class T>
inline
bool operator==(const Queue<T>& lhs, const Queue<T>& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' queues have the same
    // value, and 'false' otherwise.  Two queues have the same value if they
    // have the same length and the same element value at each respective index
    // position.

template <class T>
inline
bool operator!=(const Queue<T>& lhs, const Queue<T>& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' queues do not have the
    // same value, and 'false' otherwise.  Two queues do not have the same
    // value if they have different lengths or differ in at least one index
    // position.

template <class T>
inline
bsl::ostream& operator<<(bsl::ostream& stream, const Queue<T>& queue);
    // Write the specified 'queue' to the specified output 'stream' and return
    // a reference to the modifiable 'stream'.

}  // close package namespace

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

// TBD pass through allocator
// TBD isBitwise, etc.

              // ---------------------------------------------
              // inlined methods used by other inlined methods
              // ---------------------------------------------

template <class T>
inline
int bdlc::Queue<T>::length() const
{
    return d_back > d_front ? d_back - d_front - 1
                            : d_back + d_size - d_front - 1;
}

// PRIVATE MANIPULATORS
template <class T>
int bdlc::Queue<T>::calculateSufficientSize(int minLength, int size)
{
    const int len = minLength + k_EXTRA_CAPACITY;
    while (size < len) {
        size *= k_GROW_FACTOR;
    }
    return size;
}

template <class T>
int bdlc::Queue<T>::memcpyCircular(T       *dstArray,
                                   int      dstSize,
                                   int      dstIndex,
                                   const T *srcArray,
                                   int      srcSize,
                                   int      srcIndex,
                                   int      numElements)
{
    int dst;  // temporary value to store the current destination location

    // Break the source queue into one or two linear arrays to copy.

    int srcA = srcIndex;
    if (srcA + numElements <= srcSize) {  // one linear source array
        int lenSrcA = numElements;

        dst = dstIndex;

        // Compute the maximum number of elements that can be copied to the
        // destination array.

        int dstLen = dstSize - dst;

        if (dstLen >= lenSrcA) {  // can copy everything from srcA
            // TBD efficiency

            for (int i = 0; i < lenSrcA; ++i) {
                new (&dstArray[dst + i]) T(srcArray[srcA + i]);
            }
            dst += lenSrcA;
        }
        else {  // can copy only part of srcA without changing dst
            // TBD efficiency

            for (int i = 0; i < dstLen; ++i) {
                new (&dstArray[dst + i]) T(srcArray[srcA + i]);
            }
            srcA += dstLen;
            lenSrcA -= dstLen;

            // WARNING:  There seems to be an AIX compiler issue for the
            // following four lines.  Removing the 'assert' and moving the
            // 'memcpy' down two lines may cause the program to compile, but
            // not execute properly.

            // TBD efficiency

            for (int i = 0; i < lenSrcA; ++i) {
                new (&dstArray[i]) T(srcArray[srcA + i]);
            }
            dstLen = dst;  // max numElements that can be copied to index 0
            dst = lenSrcA;

            // TBD doc above assert(lenSrcA <= dstLen - k_EXTRA_CAPACITY);
        }
    }
    else {  // two linear source arrays
        int lenSrcA = srcSize - srcA;
        int lenSrcB = numElements - lenSrcA;

        dst = dstIndex;

        // Compute the maximum number of elements that can be copied to the
        // destination array.

        int dstLen = dstSize - dst;

        if (dstLen >= lenSrcA) {  // can copy everything from srcA
            // TBD efficiency

            for (int i = 0; i < lenSrcA; ++i) {
                new (&dstArray[dst + i]) T(srcArray[srcA + i]);
            }
            dst += lenSrcA;
        }
        else {  // can copy only part of srcA without changing dst
            // TBD efficiency

            for (int i = 0; i < dstLen; ++i) {
                new (&dstArray[dst + i]) T(srcArray[srcA + i]);
            }
            srcA += dstLen;
            lenSrcA -= dstLen;

            // WARNING:  There seems to be an AIX compiler issue for the
            // following four lines.  Removing the 'assert' and moving the
            // 'memcpy' down two lines may cause the program to compile, but
            // not execute properly.

            // TBD efficiency

            for (int i = 0; i < lenSrcA; ++i) {
                new (&dstArray[i]) T(srcArray[srcA + i]);
            }
            dstLen = dst;  // max numElements that can be copied to index 0
            dst = lenSrcA;

            // TBD
            // doc above assert(
            // lenSrcA + lenSrcB <= dstLen - k_EXTRA_CAPACITY);
        }
        dstLen -= lenSrcA;

        if (dstLen >= lenSrcB) {  // can copy everything from srcB
            // TBD efficiency

            for (int i = 0; i < lenSrcB; ++i) {
                new (&dstArray[dst + i]) T(srcArray[i]);
            }
            dst += lenSrcB;
        }
        else {  // can copy only part of srcB without changing dst
            // NOTE: could not have had insufficient room for srcA
            // TBD efficiency

            for (int i = 0; i < dstLen; ++i) {
                new (&dstArray[dst + i]) T(srcArray[i]);
            }
            lenSrcB -= dstLen;
            dst = lenSrcB;

            // TBD efficiency

            for (int i = 0; i < lenSrcB; ++i) {
                new (&dstArray[i]) T(srcArray[dstLen + i]);
            }
        }
    }

    return dst % dstSize;
}

template <class T>
void bdlc::Queue<T>::memShiftLeft(T   *array,
                                  int  size,
                                  int  dstIndex,
                                  int  srcIndex,
                                  int  numElements)
{
    // Move the elements that do not wrap around the array end.

    if (srcIndex > dstIndex) {
        int numMove = size - srcIndex;
        if (numMove >= numElements) {
            // TBD efficiency

            for (int i = 0; i < numElements; ++i) {
                new (&array[dstIndex + i]) T(array[srcIndex + i]);
                array[srcIndex + i].~T();
            }
            return;                                                   // RETURN
        }

        // TBD efficiency

        for (int i = 0; i < numMove; ++i) {
            new (&array[dstIndex + i]) T(array[srcIndex + i]);
            array[srcIndex + i].~T();
        }
        numElements -= numMove;
        dstIndex += numMove;
        srcIndex = 0;
    }
    else if (srcIndex == dstIndex) {
        return;                                                       // RETURN
    }

    // Move the elements of the source that will just precede the array end.

    int numMove = size - dstIndex;
    if (numMove >= numElements) {
        // TBD efficiency

        for (int i = numElements - 1; i >= 0; --i) {
            new (&array[dstIndex + i]) T(array[srcIndex + i]);
            array[srcIndex + i].~T();
        }

        return;                                                       // RETURN
    }
    // TBD efficiency

    for (int i = numMove - 1; i >= 0; --i) {
        new (&array[dstIndex + i]) T(array[srcIndex + i]);
        array[srcIndex + i].~T();
    }
    numElements -= numMove;
    srcIndex += numMove;

    // Move the elements of the source that are around the array end.

    // TBD efficiency

    for (int i = 0; i < numElements; ++i) {
        new (&array[i]) T(array[srcIndex + i]);
        array[srcIndex + i].~T();
    }
}

template <class T>
void bdlc::Queue<T>::memShiftRight(T   *array,
                                   int  size,
                                   int  dstIndex,
                                   int  srcIndex,
                                   int  numElements)
{
    if (dstIndex == srcIndex) {
        return;                                                       // RETURN
    }

    {

        // Move the elements of the source that wrap around the array end.

        int numMove = srcIndex + numElements;
        if (numMove > size) {
            numMove -= size;
            // TBD efficiency

            for (int i = numMove - 1; i >= 0; --i) {
                new (&array[(dstIndex + numElements - numMove) % size + i])
                                                                   T(array[i]);
                array[i].~T();
            }
            numElements -= numMove;
        }
    }

    {
        // Move the elements of the source that will wrap around the array end.

        int numMove = dstIndex + numElements;
        if (numMove > size) {
            numMove -= size;
            // TBD efficiency

            for (int i = 0; i < numMove; ++i) {
                new (&array[i])
                       T(array[(srcIndex + numElements - numMove) % size + i]);
                array[srcIndex + numElements - numMove + i].~T();
            }
            numElements -= numMove;
        }
    }

    // Move the elements of the source that do not and will not wrap around
    // the array end.

    if (dstIndex < srcIndex) {
        // TBD efficiency

        for (int i = 0; i < numElements; ++i) {
            new (&array[dstIndex + i]) T(array[srcIndex + i]);
            array[srcIndex + i].~T();
        }
    }
    else {
        // TBD efficiency

        for (int i = numElements - 1; i >= 0; --i) {
            new (&array[dstIndex + i]) T(array[srcIndex + i]);
            array[srcIndex + i].~T();
        }
    }
}

template <class T>
inline
void bdlc::Queue<T>::copyData(T       *dstArray,
                              int     *dstBack,
                              int      dstSize,
                              int      dstFront,
                              const T *srcArray,
                              int      srcSize,
                              int      srcFront,
                              int      srcBack)
{
    const int dstIndex = (dstFront + 1) % dstSize;
    const int srcIndex = (srcFront + 1) % srcSize;
    const int numElements = (srcBack + srcSize - srcFront - 1) % srcSize;

    *dstBack = memcpyCircular(dstArray,
                              dstSize,
                              dstIndex,
                              srcArray,
                              srcSize,
                              srcIndex,
                              numElements);
}

template <class T>
int bdlc::Queue<T>::increaseSizeImp(T                **addrArray,
                                    int               *front,
                                    int               *back,
                                    int                newSize,
                                    int                size,
                                    bslma::Allocator  *allocator)
{
    T *array = (T *)allocator->allocate(newSize * sizeof **addrArray);

    // COMMIT

    const int oldFront = *front;
    const int oldBack = *back;
    *front = newSize - 1;
    copyData(array, back, newSize, *front, *addrArray, size, oldFront, *back);

    // TBD efficiency

    for (int i = (oldFront + 1) % size; i != oldBack; i = (i + 1) % size) {
        (*addrArray)[i].~T();
    }

    allocator->deallocate(*addrArray);
    *addrArray = array;
    return newSize;
}

template <class T>
inline
void bdlc::Queue<T>::increaseSize()
{
    d_size = increaseSizeImp(&d_array_p,
                             &d_front,
                             &d_back,
                             d_size * k_GROW_FACTOR,
                             d_size,
                             d_allocator_p);
}

// CLASS METHODS
template <class T>
inline
int bdlc::Queue<T>::maxSupportedBdexVersion(int /* versionSelector */)
{
    return 1;  // Required by BDE policy; versions start at 1.
}


// CREATORS
template <class T>
bdlc::Queue<T>::Queue(bslma::Allocator *basicAllocator)
: d_size(k_INITIAL_SIZE)
, d_front(k_INITIAL_SIZE - 1)
, d_back(0)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    d_array_p = (T *)d_allocator_p->allocate(d_size * sizeof *d_array_p);
}

template <class T>
bdlc::Queue<T>::Queue(unsigned int      initialLength,
                      bslma::Allocator *basicAllocator)
: d_back(initialLength)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    d_size = calculateSufficientSize(initialLength, k_INITIAL_SIZE);
    d_array_p = (T *)d_allocator_p->allocate(d_size * sizeof *d_array_p);
    d_front = d_size - 1;

    // initialize the array values
    // TBD efficiency
    // TBD exception neutrality

    for (int i = 0; i < d_back; ++i) {
        new (d_array_p + i) T();
    }
}

template <class T>
bdlc::Queue<T>::Queue(int               initialLength,
                      const T&          initialValue,
                      bslma::Allocator *basicAllocator)
: d_back(initialLength)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    d_size = calculateSufficientSize(initialLength, k_INITIAL_SIZE);
    d_array_p = (T *)d_allocator_p->allocate(d_size * sizeof *d_array_p);
    d_front = d_size - 1;

    // TBD efficiency
    // TBD exception neutrality

    for (int i = 0; i < d_back; ++i) {
        new (d_array_p + i) T(initialValue);
    }
}

template <class T>
bdlc::Queue<T>::Queue(const InitialCapacity&  numElements,
                      bslma::Allocator       *basicAllocator)
: d_size(numElements.d_i + k_EXTRA_CAPACITY) // to hold the empty positions
, d_front(numElements.d_i + k_EXTRA_CAPACITY - 1)
, d_back(0)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    d_array_p = (T *)d_allocator_p->allocate(d_size * sizeof *d_array_p);
}

template <class T>
bdlc::Queue<T>::Queue(const T          *srcArray,
                      int               numElements,
                      bslma::Allocator *basicAllocator)
: d_back(numElements)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    d_size = calculateSufficientSize(numElements, k_INITIAL_SIZE);
    d_front = d_size - 1;
    d_array_p = (T *)d_allocator_p->allocate(d_size * sizeof *d_array_p);

    // TBD efficiency

    for (int i = 0; i < numElements; ++i) {
        new (&d_array_p[i]) T(srcArray[i]);
    }
}

template <class T>
bdlc::Queue<T>::Queue(const Queue& original, bslma::Allocator *basicAllocator)
: d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    d_size = calculateSufficientSize(original.length(), k_INITIAL_SIZE);
    d_array_p = (T *)d_allocator_p->allocate(d_size * sizeof *d_array_p);
    d_front = d_size - 1;
    copyData(d_array_p,
             &d_back,
             d_size,
             d_front,
             original.d_array_p,
             original.d_size,
             original.d_front,
             original.d_back);
}

template <class T>
bdlc::Queue<T>::~Queue()
{
    // TBD efficiency

    for (int i = (d_front + 1) % d_size; i != d_back; i = (i + 1) % d_size) {
        d_array_p[i].~T();
    }

    d_allocator_p->deallocate(d_array_p);
}

// MANIPULATORS
template <class T>
bdlc::Queue<T>& bdlc::Queue<T>::operator=(const Queue& rhs)
{
    if (this != &rhs) {
        const int newSize =
                         calculateSufficientSize(rhs.length(), k_INITIAL_SIZE);
        if (newSize > d_size) {
            T *array =
                     (T *)d_allocator_p->allocate(newSize * sizeof *d_array_p);

            // TBD efficiency

            for (int i = (d_front + 1) % d_size; i != d_back;
                                                        i = (i + 1) % d_size) {
                d_array_p[i].~T();
            }

            d_allocator_p->deallocate(d_array_p);
            d_array_p = array;
            d_size = newSize;
        }
        else {
            // TBD efficiency

            for (int i = (d_front + 1) % d_size; i != d_back;
                                                        i = (i + 1) % d_size) {
                d_array_p[i].~T();
            }
        }
        copyData(d_array_p,
                 &d_back,
                 d_size,
                 d_front,
                 rhs.d_array_p,
                 rhs.d_size,
                 rhs.d_front,
                 rhs.d_back);
    }
    return *this;
}

template <class T>
inline
T& bdlc::Queue<T>::operator[](int index)
{
    return d_array_p[(index + d_front + 1) % d_size];
}

template <class T>
void bdlc::Queue<T>::append(const Queue& srcQueue)
{
    const int numElements = srcQueue.length();
    const int newLength = length() + numElements;
    const int minSize = calculateSufficientSize(newLength, d_size);
    if (d_size < minSize) {
        d_size = increaseSizeImp(&d_array_p,
                                 &d_front,
                                 &d_back,
                                 minSize,
                                 d_size,
                                 d_allocator_p);
    }
    d_back = memcpyCircular(d_array_p,
                            d_size,
                            d_back,
                            srcQueue.d_array_p,
                            srcQueue.d_size,
                            (srcQueue.d_front + 1) % srcQueue.d_size,
                            numElements);
}

template <class T>
void bdlc::Queue<T>::append(const Queue& srcQueue,
                            int          srcIndex,
                            int          numElements)
{
    const int newLength = length() + numElements;
    const int minSize = calculateSufficientSize(newLength, d_size);
    if (d_size < minSize) {
        d_size = increaseSizeImp(&d_array_p,
                                 &d_front,
                                 &d_back,
                                 minSize,
                                 d_size,
                                 d_allocator_p);
    }
    d_back = memcpyCircular(d_array_p,
                            d_size,
                            d_back,
                            srcQueue.d_array_p,
                            srcQueue.d_size,
                            (srcQueue.d_front + 1 + srcIndex) %
                                srcQueue.d_size,
                            numElements);
}

template <class T>
inline
T& bdlc::Queue<T>::back()
{
    return d_array_p[(d_back - 1 + d_size) % d_size];
}

template <class T>
inline
T& bdlc::Queue<T>::front()
{
    return d_array_p[(d_front + 1) % d_size];
}

template <class T>
void bdlc::Queue<T>::insert(int dstIndex, const T& item)
{
    T itemCopy(item);  // TBD hack for aliased case

    // The capacity must always be greater than or equal to
    // 'length + k_EXTRA_CAPACITY'.

    const int originalLength = length();
    const int newLength = originalLength + 1;
    const int newSize = calculateSufficientSize(newLength, d_size);

    if (d_size < newSize) {
        // resize, makes move easy

        T *array = (T *)d_allocator_p->allocate(newSize * sizeof *d_array_p);

        // COMMIT

        const int start = d_front + 1;

        // NOTE: newSize >= size + 1 so '% newSize' is not needed in next line.

        memcpyCircular(array,
                       newSize,
                       start,           // no '% newSize'
                       d_array_p,
                       d_size,
                       start % d_size,
                       dstIndex);
        memcpyCircular(array,
                       newSize,
                       (start + dstIndex + 1) % newSize,
                       d_array_p,
                       d_size,
                       (start + dstIndex) % d_size,
                       originalLength - dstIndex);

        // TBD efficiency

        for (int i = (d_front + 1) % d_size; i != d_back;
                                                        i = (i + 1) % d_size) {
            d_array_p[i].~T();
        }

        d_allocator_p->deallocate(d_array_p);
        d_array_p = array;

        d_size = newSize;
        d_back = (start + newLength) % d_size;
        new (&d_array_p[(start + dstIndex) % d_size]) T(itemCopy);
    }
    else {  // sufficient capacity

        // No resize is required.  Copy as few elements as possible.

        // Compute number of elements that are past the insertion point: the
        // back length.

        const int backLen = originalLength - dstIndex;

        if (dstIndex < backLen) {

            // We will choose to shift 'dstIndex' elements to the left.

            const int src = (d_front + 1) % d_size;
            const int dst = d_front;

            memShiftLeft(d_array_p, d_size, dst, src, dstIndex);
            new (&d_array_p[(d_front + dstIndex) % d_size]) T(itemCopy);
            d_front = (d_front - 1 + d_size) % d_size;
        }
        else {

            // We will choose to shift 'backLen' elements to the right.

            const int src = (d_front + 1 + dstIndex) % d_size;
            const int dst = (src + 1) % d_size;

            memShiftRight(d_array_p,
                          d_size,
                          dst,
                          src,
                          backLen);
            new (&d_array_p[(d_front + 1 + dstIndex) % d_size]) T(itemCopy);
            d_back = (d_back + 1) % d_size;
        }
    }
}

template <class T>
void bdlc::Queue<T>::insert(int          dstIndex,
                            const Queue& srcQueue,
                            int          srcIndex,
                            int          numElements)
{
    // The capacity must always be greater than or equal to
    // 'length + k_EXTRA_CAPACITY'.

    const int originalLength = length();
    const int newLength = originalLength + numElements;
    const int newSize = calculateSufficientSize(newLength, d_size);

    if (d_size < newSize) {
        // resize, makes move easy

        T *array = (T *)d_allocator_p->allocate(newSize * sizeof *d_array_p);

        // COMMIT

        const int start = d_front + 1;
        const int startIndex = start + dstIndex;

        // NOTE: newSize >= size + 1 so '% newSize' is not needed in next line.

        memcpyCircular(array,
                       newSize,
                       start,           // no '% newSize'
                       d_array_p,
                       d_size,
                       start % d_size,
                       dstIndex);
        memcpyCircular(array,
                       newSize,
                       (startIndex + numElements) % newSize,
                       d_array_p,
                       d_size,
                       (startIndex) % d_size,
                       originalLength - dstIndex);
        memcpyCircular(array,
                       newSize,
                       startIndex % newSize,
                       srcQueue.d_array_p,
                       srcQueue.d_size,
                       (srcQueue.d_front + 1 + srcIndex) % srcQueue.d_size,
                       numElements);

        // TBD efficiency

        for (int i = (d_front + 1) % d_size; i != d_back;
                                                        i = (i + 1) % d_size) {
            d_array_p[i].~T();
        }

        d_allocator_p->deallocate(d_array_p);
        d_array_p = array;
        d_size = newSize;
        d_back = (start + newLength) % d_size;
    }
    else { // sufficient capacity

        // No resize is required.  Copy as few elements as possible.

        // Compute number of elements that are past the insertion point: the
        // back length.

        const int backLen = originalLength - dstIndex;
        if (dstIndex < backLen) {

            // We will shift 'dstIndex' elements to the left.

            const int d = (d_front + 1 - numElements + d_size) % d_size;
            memShiftLeft(d_array_p,
                         d_size,
                         d,
                         (d_front + 1) % d_size,
                         dstIndex);

            if (this != &srcQueue || srcIndex >= dstIndex) {  // not aliased
                memcpyCircular(d_array_p,
                               d_size,
                               (d + dstIndex) % d_size,
                               srcQueue.d_array_p,
                               srcQueue.d_size,
                               (srcQueue.d_front + 1 + srcIndex) %
                                                               srcQueue.d_size,
                               numElements);
            }
            else { // aliased
                const int distance = dstIndex - srcIndex;
                if (distance >= numElements) {
                    memcpyCircular(d_array_p,
                                   d_size,
                                   (d + dstIndex) % d_size,
                                   d_array_p,
                                   d_size,
                                   (d + srcIndex) % d_size,
                                   numElements);
                }
                else {
                    memcpyCircular(d_array_p,
                                   d_size,
                                   (d + dstIndex) % d_size,
                                   d_array_p,
                                   d_size,
                                   (d + srcIndex) % d_size,
                                   distance);
                    memcpyCircular(d_array_p,
                                   d_size,
                                   (d + dstIndex + distance) % d_size,
                                   d_array_p,
                                   d_size,
                                   (d_front + 1 + dstIndex) % d_size,
                                   numElements - distance);
                }
            }
            d_front = (d_front - numElements + d_size) % d_size;
        }
        else {

            // We will shift 'backLen' elements to the right.

            // Destination index is as close or closer to the back as to the
            // front.

            const int s = (d_front + 1 + dstIndex) % d_size;
            memShiftRight(d_array_p,
                          d_size,
                          (s + numElements) % d_size,
                          s,
                          backLen);

            if (this != &srcQueue ||
                           srcIndex + numElements <= dstIndex) { // not aliased
                memcpyCircular(d_array_p,
                               d_size,
                               s,
                               srcQueue.d_array_p,
                               srcQueue.d_size,
                               (srcQueue.d_front + 1 + srcIndex) %
                                                               srcQueue.d_size,
                               numElements);
            }
            else { // aliased
                if (dstIndex <= srcIndex) {
                    memcpyCircular(d_array_p,
                                   d_size,
                                   s,
                                   d_array_p,
                                   d_size,
                                   (d_front + 1 + srcIndex + numElements) %
                                                                        d_size,
                                   numElements);
                }
                else {
                    const int distance = dstIndex - srcIndex;
                    memcpyCircular(d_array_p,
                                   d_size,
                                   s,
                                   d_array_p,
                                   d_size,
                                   (d_front + 1 + srcIndex) % d_size,
                                   distance);
                    memcpyCircular(d_array_p,
                                   d_size,
                                   (s + distance) % d_size,
                                   d_array_p,
                                   d_size,
                                   (d_front + 1 + srcIndex + distance +
                                                         numElements) % d_size,
                                   numElements - distance);
                }
            }
            d_back = (d_back + numElements) % d_size;
        }
    }
}

template <class T>
inline
void bdlc::Queue<T>::insert(int dstIndex, const Queue& srcQueue)
{
    insert(dstIndex, srcQueue, 0, srcQueue.length());
}

template <class T>
inline
void bdlc::Queue<T>::popBack()
{
    d_back = (d_back - 1 + d_size) % d_size;
    d_array_p[d_back].~T();
}

template <class T>
inline
void bdlc::Queue<T>::popFront()
{
    d_front = (d_front + 1) % d_size;
    d_array_p[d_front].~T();
}

template <class T>
void bdlc::Queue<T>::pushBack(const T& item)
{
    T itemCopy(item);  // TBD aliasing hack

    int newBack = (d_back + 1) % d_size;
    if (d_front == newBack) {
        increaseSize();  // NOTE: this can change the value of d_back
        newBack = (d_back + 1) % d_size;
    }
    new (&d_array_p[d_back]) T(itemCopy);
    d_back = newBack;
}

template <class T>
void bdlc::Queue<T>::pushFront(const T& item)
{
    T itemCopy(item);  // TBD aliasing hack

    int newFront = (d_front - 1 + d_size) % d_size;
    if (newFront == d_back) {
        increaseSize();  // NOTE: this can change the value of d_front
        newFront = (d_front - 1 + d_size) % d_size;
    }
    new (&d_array_p[d_front]) T(itemCopy);
    d_front = newFront;
}

template <class T>
inline
void bdlc::Queue<T>::append(const T& item)
{
    pushBack(item);
}

template <class T>
void bdlc::Queue<T>::remove(int index)
{
    d_array_p[(index + d_front + 1) % d_size].~T();

    // Compute number of elements that are past the insertion point: the back
    // length.

    const int backLen =
               (d_back - d_front - k_EXTRA_CAPACITY - index + d_size) % d_size;

    if (index < backLen) {
        d_front = (d_front + 1) % d_size;
        memShiftRight(d_array_p,
                      d_size,
                      (d_front + 1) % d_size,
                      d_front,
                      index);
    }
    else {
        const int d = (d_front + 1 + index) % d_size;
        memShiftLeft(d_array_p,
                     d_size,
                     d,
                     (d + 1) % d_size,
                     (d_back + d_size - d_front - 1) % d_size - 1 - index);
        d_back = (d_back - 1 + d_size) % d_size;
    }
}

template <class T>
void bdlc::Queue<T>::remove(int index, int numElements)
{
    // TBD efficiency

    for (int i = 0; i < numElements; ++i) {
        d_array_p[(index + d_front + 1 + i) % d_size].~T();
    };

    // Compute number of elements that are past the insertion point: the back
    // length.

    const int backLen = (d_back - d_front - 1
                                      - index - numElements + d_size) % d_size;
    if (index < backLen) {
        const int dst = (d_front + 1 + numElements) % d_size;
        const int src = (d_front + 1) % d_size;

        memShiftRight(d_array_p, d_size, dst, src, index);
        d_front = (d_front + numElements) % d_size;
    }
    else {
        const int dst = (d_front + 1 + index) % d_size;
        const int src = (dst + numElements) % d_size;

        memShiftLeft(d_array_p,
                     d_size,
                     dst,
                     src,
                     (d_back + d_size - d_front - 1) % d_size -
                                                          numElements - index);
        d_back = (d_back - numElements + d_size) % d_size;
    }
}

template <class T>
void bdlc::Queue<T>::removeAll(bsl::vector<T> *buffer)
{
    d_front = (d_front + 1) % d_size;

    // TBD efficiency

    if (buffer) {
        while (d_back != d_front) {
            buffer->push_back(d_array_p[d_front]);
            d_array_p[d_front].~T();
            d_front = (d_front + 1) % d_size;
        }
    } else {
        while (d_back != d_front) {
            d_array_p[d_front].~T();
            d_front = (d_front + 1) % d_size;
        }
    }
    d_front = (d_back - 1 + d_size) % d_size;
}

template <class T>
void bdlc::Queue<T>::replace(int dstIndex, const T& item)
{
    T itemCopy(item);  // TBD hack for aliased case

    // TBD efficiency

    d_array_p[(d_front + 1 + dstIndex) % d_size].~T();
    new (&d_array_p[(d_front + 1 + dstIndex) % d_size]) T(itemCopy);
}

template <class T>
void bdlc::Queue<T>::replace(int          dstIndex,
                             const Queue& srcQueue,
                             int          srcIndex,
                             int          numElements)
{
    // TBD need placement new

    if (this != &srcQueue || srcIndex + numElements <= dstIndex ||
                          dstIndex + numElements <= srcIndex) {  // not aliased
        memcpyCircular(d_array_p,
                       d_size,
                       (d_front + 1 + dstIndex) % d_size,
                       srcQueue.d_array_p,
                       srcQueue.d_size,
                       (srcQueue.d_front + 1 + srcIndex) % srcQueue.d_size,
                       numElements);
    }
    else {  // aliased; do nothing if srcIndex == dstIndex
        if (srcIndex < dstIndex) {
            memShiftRight(d_array_p,
                          d_size,
                          (d_front + 1 + dstIndex) % d_size,
                          (d_front + 1 + srcIndex) % d_size,
                          numElements);
        }
        else if (srcIndex > dstIndex) {
            memShiftLeft(d_array_p,
                         d_size,
                         (d_front + 1 + dstIndex) % d_size,
                         (d_front + 1 + srcIndex) % d_size,
                         numElements);
        }
    }
}

template <class T>
void bdlc::Queue<T>::reserveCapacity(int numElements)
{
    const int newSize = calculateSufficientSize(numElements, d_size);
    if (d_size < newSize) {
        d_size = increaseSizeImp(&d_array_p,
                                 &d_front,
                                 &d_back,
                                 newSize,
                                 d_size,
                                 d_allocator_p);

        // To improve testability, all empty queues have canonical front and
        // back values.

        if (0 == length()) {
            d_front = d_size - 1;
            d_back = 0;
        }
    }
}

template <class T>
void bdlc::Queue<T>::reserveCapacityRaw(int numElements)
{
    const int newSize = numElements + k_EXTRA_CAPACITY;
                                            // to hold the front/back positions

    if (d_size < newSize) {
        d_size = increaseSizeImp(&d_array_p,
                                 &d_front,
                                 &d_back,
                                 newSize,
                                 d_size,
                                 d_allocator_p);
    }
}

template <class T>
void bdlc::Queue<T>::setLength(int newLength)
{
    const int newSize = newLength + k_EXTRA_CAPACITY;
                                            // to hold the front/back positions

    if (d_size < newSize) {
        d_size = increaseSizeImp(&d_array_p,
                                 &d_front,
                                 &d_back,
                                 newSize,
                                 d_size,
                                 d_allocator_p);
    }
    const int oldBack = d_back;
    const int oldLength = length();
    d_back = (d_front + 1 + newLength) % d_size;
    if (newLength > oldLength) {
        if (oldBack < d_back) {
            // TBD efficiency

            for (int i = 0; i < d_back - oldBack; ++i) {
                new (d_array_p + oldBack + i) T();
            }
        }
        else {
            // TBD efficiency

            for (int i = 0; i < d_size - oldBack; ++i) {
                new (d_array_p + oldBack + i) T();
            }

            // TBD efficiency

            for (int i = 0; i < d_back; ++i) {
                new (d_array_p + i) T();
            }
        }
    }
}

template <class T>
void bdlc::Queue<T>::setLength(int newLength, const T& initialValue)
{
    const int newSize = newLength + k_EXTRA_CAPACITY;
                                                 // to hold the empty positions

    if (d_size < newSize) {
        d_size = increaseSizeImp(&d_array_p,
                                 &d_front,
                                 &d_back,
                                 newSize,
                                 d_size,
                                 d_allocator_p);
    }
    const int oldBack = d_back;
    const int oldLength = length();
    d_back = (d_front + 1 + newLength) % d_size;
    if (newLength > oldLength) {
        if (oldBack < d_back) {
            // TBD efficiency

            for (int i = 0; i < d_back - oldBack; ++i) {
                new (d_array_p + oldBack + i) T(initialValue);
            }
        }
        else {
            // TBD efficiency

            for (int i = 0; i < d_size - oldBack; ++i) {
                new (d_array_p + oldBack + i) T(initialValue);
            }
            // TBD efficiency

            for (int i = 0; i < d_back; ++i) {
                new (d_array_p + i) T(initialValue);
            }
        }
    }
}

template <class T>
void bdlc::Queue<T>::setLengthRaw(int newLength)
{
    const int newSize = newLength + k_EXTRA_CAPACITY;
                                                 // to hold the empty positions

    if (d_size < newSize) {
        d_size = increaseSizeImp(&d_array_p,
                                 &d_front,
                                 &d_back,
                                 newSize,
                                 d_size,
                                 d_allocator_p);
    }
    d_back = (d_front + 1 + newLength) % d_size;
}

template <class T>
template <class STREAM>
STREAM& bdlc::Queue<T>::bdexStreamIn(STREAM& stream, int version)
{
    if (stream) {
        switch (version) { // switch on the schema version
          case 1: {
            int newLength;
            stream.getLength(newLength);

            if (stream) {
                int newSize = calculateSufficientSize(newLength, d_size);
                if (d_size < newSize) {
                    d_size = increaseSizeImp(&d_array_p,
                                             &d_front,
                                             &d_back,
                                             newSize,
                                             d_size,
                                             d_allocator_p);
                }
                d_front = d_size - 1;
                d_back = newLength;
                for (int i = 0; i < newLength && stream; ++i) {
                    bslx::InStreamFunctions::bdexStreamIn(
                                                  stream, (*this)[i], version);
                }
            }
          } break;
          default: {
            stream.invalidate();  // unrecognized version number
          }
        }
    }
    return stream;
}

template <class T>
void bdlc::Queue<T>::swap(int index1, int index2)
{
    if (index1 != index2) {
        const int tmp = d_front + 1;
        const int i1 = (tmp + index1) % d_size;
        const int i2 = (tmp + index2) % d_size;

        T temp(d_array_p[i1]);
        d_array_p[i1].~T();
        new (d_array_p + i1) T(d_array_p[i2]);
        d_array_p[i2].~T();
        new (d_array_p + i2) T(temp);
    }
}

// ACCESSORS
template <class T>
inline
const T& bdlc::Queue<T>::operator[](int index) const
{
    return d_array_p[(index + d_front + 1) % d_size];
}

template <class T>
inline
const T& bdlc::Queue<T>::back() const
{
    return d_array_p[(d_back - 1 + d_size) % d_size];
}

template <class T>
inline
const T& bdlc::Queue<T>::front() const
{
    return d_array_p[(d_front + 1) % d_size];
}

template <class T>
bsl::ostream& bdlc::Queue<T>::print(bsl::ostream& stream,
                                    int           level,
                                    int           spacesPerLevel) const
{
    if (level < 0) {
        level = -level;
    }
    else {
        bdlb::Print::indent(stream, level, spacesPerLevel);
    }

    int levelPlus1 = level + 1;
    if (0 <= spacesPerLevel) {

        stream << "[\n";

        const int len = length();
        for (int i = 0; i < len; ++i) {
            bdlb::Print::indent(stream, levelPlus1, spacesPerLevel);
            stream << d_array_p[(i + d_front + 1) % d_size] << '\n';
        }

        bdlb::Print::indent(stream, level, spacesPerLevel);
        stream << "]\n";
    }
    else {
        stream << "[ ";

        const int len = length();
        for (int i = 0; i < len; ++i) {
            stream << ' ';
            stream << d_array_p[(i + d_front + 1) % d_size];
        }

        stream << " ] ";
    }
    return stream << bsl::flush;
}

template <class T>
template <class STREAM>
STREAM& bdlc::Queue<T>::bdexStreamOut(STREAM& stream, int version) const
{
    if (stream) {
        switch (version) { // switch on the schema version
          case 1: {
            const int len = length();
            stream.putLength(len);
            for (int i = 0; i < len && stream; ++i) {
                bslx::OutStreamFunctions::bdexStreamOut(
                                                  stream, (*this)[i], version);
            }
          } break;
          default: {
            stream.invalidate();  // unrecognized version number
          }
        }
    }
    return stream;
}

// FREE OPERATORS
template <class T>
bool bdlc::operator==(const Queue<T>& lhs, const Queue<T>& rhs)
{
    const int len = lhs.length();
    if (rhs.length() != len) {
        return 0;                                                     // RETURN
    }

    // Lengths are equal.

    for (int i = 0; i < len; ++i) {
        if (!(lhs[i] == rhs[i])) {
            return 0;                                                 // RETURN
        }
    }
    return 1;
}

template <class T>
inline
bool bdlc::operator!=(const Queue<T>& lhs, const Queue<T>& rhs)
{
    return !(lhs == rhs);
}

template <class T>
inline
bsl::ostream& bdlc::operator<<(bsl::ostream& stream, const Queue<T>& queue)
{
    return queue.streamOut(stream);
}

}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2015 Bloomberg Finance L.P.
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
