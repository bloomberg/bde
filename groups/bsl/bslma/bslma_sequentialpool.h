// bslma_sequentialpool.h                                             -*-C++-*-
#ifndef INCLUDED_BSLMA_SEQUENTIALPOOL
#define INCLUDED_BSLMA_SEQUENTIALPOOL

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide fast variable-size memory pool with allocation methods.
//
//@DEPRECATED: Use 'bdema_sequentialpool' instead.
//
//@BDE_TRANSITIONAL
//
//@CLASSES:
//  bslma::SequentialPool: fast variable-size memory pool
//
//@SEE_ALSO: bdema_sequentialpool
//
//@AUTHOR: Shao-wei Hung (shung1)
//
//@DESCRIPTION: This component implements a memory pool that dispenses memory
// blocks of any requested size from an internal buffer or an optional
// user-supplied buffer.  If an allocation request exceeds the remaining free
// memory space in the pool, the pool either replenishes its buffer with new
// memory to satisfy the request, or returns a separate memory block, depending
// on whether the request size exceeds an optionally specified maximum buffer
// size.  By default, buffer growth is not capped.  The 'release' method
// releases all memory allocated through this pool, as does the destructor.
// Note, however, that individual allocated blocks of memory cannot be
// separately deallocated.
//
///Alignment Strategy
///------------------
// The 'bslma::SequentialPool' allocates memory using one of the two alignment
// strategies (defined in 'bslma_bufferallocator') optionally specified at
// construction: 1) MAXIMUM ALIGNMENT or 2) NATURAL ALIGNMENT.
//
//: 1 MAXIMUM ALIGNMENT: This strategy always allocates memory aligned with the
//:   most restrictive alignment on the host platform.  The value is defined by
//:   'bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT'.
//:
//: 2 NATURAL ALIGNMENT: This strategy allocates memory whose alignment depends
//:   on the requested number of bytes.  An instance of a fundamental type
//:   ('int', etc.) is *naturally* *aligned* when it's size evenly divides its
//:   address.  An instance of an aggregate type has natural alignment if the
//:   alignment of the most-restrictively aligned sub-object evenly divides the
//:   address of the aggregate.  Natural alignment is always at least as
//:   restrictive as the compiler's required alignment.  When only the size of
//:   an aggregate is known, and not its composition, we compute the alignment
//:   by finding the largest integral power of 2 (up to and including
//:   'bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT') that divides the requested
//:   (non-zero) number of bytes.  This computed alignment is guaranteed to be
//:   at least as restrictive as any sub-object within the aggregate.
//
// The default strategy is NATURAL ALIGNMENT.
//
///Optional 'buffer' Parameter
///---------------------------
// A buffer can be supplied to a 'bslma::SequentialPool' object at construction
// in which case the pool will try to satisfy allocation requests using this
// buffer before switching to a dynamically-allocated internal pool.  Once the
// object is using an internal pool, it will not try to satisfy any subsequent
// allocation requests from the supplied buffer.  Note that the pool does *not*
// take ownership of the buffer.  Also note that 'bufferSize' may be specified
// using a positive or negative value to indicate a buffer growth strategy (see
// "Internal Buffer Growth").
//
///Optional 'initialSize' Parameter
///--------------------------------
// In lieu of an externally-supplied buffer, a value for the 'initialSize'
// parameter may be supplied at construction to specify the initial size of the
// internal pool.  If neither a buffer nor an 'initialSize' is specified, an
// implementation-defined value is used for an initial size of the internal
// pool.  Note that 'initialSize' may be specified using a positive or negative
// value to indicate a buffer growth strategy (see "Internal Buffer Growth").
//
///Internal Buffer Growth
///----------------------
// A 'bslma::SequentialPool' replenishes its internal buffer if the current
// buffer cannot satisfy an allocation request.  It does so by one of two
// growth strategies:
//
//: Constant Growth: The new buffer is always of the same size as the current
//:   buffer (possibly supplied at construction).
//:
//: Geometric Growth: The new buffer will be geometrically larger than the
//:   current buffer up to an optionally-specified maximum limit.
//
// If a 'bufferSize' (and corresponding 'buffer') or 'initialSize' is supplied
// at construction, the sign of its value implicitly specifies which growth
// strategy to use.  A positive value indicates Constant Growth, whereas a a
// negative value indicates Geometric Growth.  If neither 'bufferSize' nor
// 'initialSize' is supplied, Geometric Growth is used.  The optional
// 'maxBufferSize' parameter may be used to place a cap on Geometric Growth
// ('maxBufferSize' is ignored if Constant Growth is in effect).  If no value
// is specified for 'maxBufferSize', there is no cap on Geometric Growth.  Note
// that 'reserveCapacity' always ensures that the requested number of bytes is
// available (allocating a new internal pool if necessary) irrespective of
// whether the size of the request exceeds 'maxBufferSize'.
//
///Usage
///-----
// The 'bslma::SequentialPool' can be used to allocate memory for containers of
// non-homogeneous elements, such as 'my_List' below.  Note that the use of a
// sequential pool allows the 'operator=' and 'removeAll' methods to quickly
// deallocate memory of all elements by calling the 'release' method of the
// pool.  Similarly, the destructor of 'my_List' simply allows the pool's
// destructor to deallocate memory for all elements:
//..
//  // my_list.h
//  #include <bsls_types.h>
//  #include <bslma_sequentialpool.h>
//
//  namespace bslma { class Allocator; }
//
//  class my_List {
//      char                   *d_typeArray_p;
//      void                  **d_list_p;
//      int                     d_length;
//      int                     d_size;
//      bslma::Allocator       *d_allocator_p;
//      bslma::SequentialPool   d_pool;
//
//      // NOT IMPLEMENTED
//      my_List(const my_List&);
//
//    private:
//      my_List(char* buffer, int bufferSize, bslma::Allocator *basicAllocator);
//      void increaseSize();
//
//    public:
//      enum Type { INT, DOUBLE, INT64 };
//
//      my_List(bslma::Allocator *basicAllocator);
//
//      ~my_List();
//      my_List& operator=(const my_List& rhs);
//      void append(int value);
//      void append(double value);
//      void append(bsls::Types::Int64 value);
//      void removeAll();
//
//      const int *theInt(int index) const;
//      const double *theDouble(int index) const;
//      const bsls::Types::Int64 *theInt64(int index) const;
//      const Type type(int index) const;
//      int length() const;
//  };
//
//  inline
//  void my_List::removeAll()
//  {
//      d_pool.release();
//      d_length = 0;
//  }
//
//  inline
//  const int *my_List::theInt(int index) const
//  {
//      return (int *) d_list_p[index];
//  }
//
//  inline
//  const double *my_List::theDouble(int index) const
//  {
//      return (double *) d_list_p[index];
//  }
//
//  inline
//  const bsls::Types::Int64 *my_List::theInt64(int index) const
//  {
//      return (bsls::Types::Int64 *) d_list_p[index];
//  }
//
//  inline
//  const my_List::Type my_List::type(int index) const
//  {
//      return (Type) d_typeArray_p[index];
//  }
//
//  inline
//  int my_List::length() const
//  {
//      return d_length;
//  }
//
//  // ...
//
//  // my_list.cpp
//  #include <my_list.h>
//  #include <bslma_allocator.h>
//
//  enum { INITIAL_SIZE = 1, GROW_FACTOR = 2 };
//
//  static
//  void copyElement(void **list, my_List::Type type, int index,
//                   void *srcElement, bslma::SequentialPool *pool)
//      // Copy the value of the specified 'srcElement' of the specified 'type'
//      // to the specified 'index' position in the specified 'list'.  Use the
//      // specified 'pool' to supply memory.
//  {
//      assert(list);
//      assert(0 <= index);
//      assert(srcElement);
//      assert(pool);
//
//      typedef bsls::Types::Int64 Int64;
//
//      switch (type) {
//        case my_List::INT:
//          list[index] = new(pool->allocate(sizeof(int)))
//                        int(*((int *) srcElement));
//          break;
//        case my_List::DOUBLE:
//          list[index] = new(pool->allocate(sizeof(double)))
//                        double(*((double *) srcElement));
//          break;
//        case my_List::INT64:
//          list[index] = new(pool->allocate(sizeof(Int64)))
//                        Int64(*((Int64 *) srcElement));
//          break;
//        default:
//          assert(0 && "ERROR (my_List): Invalid element type.");
//      }
//  }
//
//  static
//  void reallocate(void ***list, char **typeArray, int *size,
//                  int newSize, int length, bslma::Allocator *basicAllocator)
//      // Reallocate memory in the specified 'list' and 'typeArray' using the
//      // specified 'basicAllocator' and update the specified size to the
//      // specified 'newSize'.  The specified 'length' number of leading
//      // elements are preserved in 'list' and 'typeArray'.  If 'allocate'
//      // should throw an exception, this function has no effect.  The
//      // behavior is undefined unless 1 <= newSize, 0 <= length, and
//      // newSize <= length.
//  {
//      assert(list);
//      assert(*list);
//      assert(typeArray);
//      assert(*typeArray);
//      assert(size);
//      assert(1 <= newSize);
//      assert(0 <= length);
//      assert(length <= *size);    // sanity check
//      assert(length <= newSize);  // ensure class invariant
//      assert(basicAllocator);
//
//      void **newList =
//          (void **) basicAllocator->allocate(newSize * sizeof *newList);
//      char *newTypeArray =
//          (char *) basicAllocator->allocate(newSize * sizeof *newTypeArray);
//      memcpy(newList, *list, length * sizeof **list);
//      memcpy(newTypeArray, *typeArray, length * sizeof **typeArray);
//      basicAllocator->deallocate(*list);
//      basicAllocator->deallocate(*typeArray);
//      *list = newList;
//      *typeArray = newTypeArray;
//      *size = newSize;
//  }
//
//  void my_List::increaseSize()
//  {
//       int newSize = d_size * GROW_FACTOR;
//       reallocate(&d_list_p, &d_typeArray_p, &d_size, newSize,
//                  d_length, d_allocator_p);
//  }
//
//  my_List::my_List(char* buffer, int bufferSize,
//                   bslma::Allocator *basicAllocator)
//  : d_length()
//  , d_size(MY_INITIAL_SIZE)
//  , d_pool(buffer, bufferSize, basicAllocator)
//  , d_allocator_p(basicAllocator)
//  {
//      assert(d_allocator_p);
//
//      d_typeArray_p =
//          (char *) d_allocator_p->allocate(d_size * sizeof *d_typeArray_p);
//      d_list_p =
//          (void **) d_allocator_p->allocate(d_size * sizeof *d_list_p);
//  }
//
//  my_List::my_List(bslma::Allocator *basicAllocator)
//  : d_size(INITIAL_SIZE)
//  , d_length(0)
//  , d_pool(basicAllocator)
//  , d_allocator_p(basicAllocator)
//  {
//      assert(d_allocator_p);
//
//      d_typeArray_p =
//          (char *) d_allocator_p->allocate(d_size * sizeof *d_typeArray_p);
//      d_list_p =
//          (void **) d_allocator_p->allocate(d_size * sizeof *d_list_p);
//  }
//
//  my_List::~my_List()
//  {
//      assert(d_typeArray_p);
//      assert(d_list_p);
//      assert(0 <= d_size);
//      assert(0 <= d_length);  assert(d_length <= d_size);
//      assert(d_allocator_p);
//
//      d_allocator_p->deallocate(d_typeArray_p);
//      d_allocator_p->deallocate(d_list_p);
//  }
//
//  my_List& my_List::operator=(const my_List& rhs)
//  {
//      if (&rhs != this) {
//          // not aliased
//          d_pool.release();
//          d_length = 0;
//
//          int newLength = rhs.d_length;
//          if (newLength > d_size) {
//              reallocate(&d_list_p, &d_typeArray_p, &d_size,
//                         newLength, d_length, d_allocator_p);
//          }
//          for (int i = 0; i < newLength; ++i) {
//              d_typeArray_p[i] = rhs.d_typeArray_p[i];
//              copyElement(d_list_p, (Type) d_typeArray_p[i], i,
//                          rhs.d_list_p[i], &d_pool);
//          }
//          d_length = newLength;
//      }
//      return *this;
//  }
//
//  void my_List::append(int value)
//  {
//      if (d_length >= d_size) {
//          increaseSize();
//      }
//      int *item = (int *) d_pool.allocate(sizeof *item);
//      *item = value;
//      d_typeArray_p[d_length] = (char) my_List::INT;
//      d_list_p[d_length++] = item;
//  }
//
//  void my_List::append(double value)
//  {
//      if (d_length >= d_size) {
//          increaseSize();
//      }
//      double *item = (double *) d_pool.allocate(sizeof *item);
//      *item = value;
//      d_typeArray_p[d_length] = (char) my_List::DOUBLE;
//      d_list_p[d_length++] = item;
//  }
//
//  void my_List::append(bsls::Types::Int64 value)
//  {
//      typedef bsls::Types::Int64 Int64;
//
//      if (d_length >= d_size) {
//          increaseSize();
//      }
//      Int64 *item = (Int64 *) d_pool.allocate(sizeof *item);
//      *item = value;
//      d_typeArray_p[d_length] = (char) my_List::INT64;
//      d_list_p[d_length++] = item;
//  }
//..

#if defined(BSL_PUBLISHED)
#error "bslma_sequentialpool is deprecated"
#endif

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLMA_BUFFERALLOCATOR
#include <bslma_bufferallocator.h>
#endif

#ifndef INCLUDED_BSLMA_INFREQUENTDELETEBLOCKLIST
#include <bslma_infrequentdeleteblocklist.h>
#endif

#ifndef INCLUDED_CSTDDEF
#include <cstddef>         // for 'std::size_t'
#define INCLUDED_CSTDDEF
#endif

namespace BloombergLP {

namespace bslma {

                         // ====================
                         // class SequentialPool
                         // ====================

class SequentialPool {
    // This class implements a memory pool that dispenses arbitrarily-sized
    // blocks of memory from an internal buffer or an optionally user-supplied
    // buffer.  If an allocation request exceeds the remaining free memory
    // space in the current buffer, the pool either replenishes its buffer with
    // new memory to satisfy the request, or returns a separate memory block,
    // depending on whether the request size exceeds an optionally specified
    // maximum buffer size.  By default, buffer growth is not capped.  The
    // 'release' method releases all memory allocated through this pool, as
    // does the destructor.  Note, however, that individual allocated blocks of
    // memory cannot be individually deallocated.

    // TYPES
    enum GrowthStrategy {
        // Enumerate the various internal buffer growth strategies.

        CONSTANT,  // constant growth
        GEOMETRIC  // geometric growth
    };

    // DATA
    char                  *d_buffer;             // holds current free memory
                                                 // buffer

    int                    d_cursor;             // position of the next
                                                 // available byte

    int                    d_bufSize;            // current free memory buffer
                                                 // size

    BufferAllocator::AlignmentStrategy
                           d_alignmentStrategy;  // alignment strategy

    int                    d_maxBufferSize;      // maximum buffer size

    GrowthStrategy         d_growthStrategy;     // growth strategy

    InfrequentDeleteBlockList
                           d_blockList;          // provides free memory

    // NOT IMPLEMENTED
    SequentialPool(const SequentialPool&);
    SequentialPool& operator=(const SequentialPool&);

  private:
    // PRIVATE ACCESSORS
    int calculateNextBufferSize(int size) const;
        // Calculate and return the next buffer size corresponding to a reserve
        // or allocation of the specified 'size' bytes.

  public:
    // CREATORS
    explicit SequentialPool(Allocator *basicAllocator = 0);
    explicit SequentialPool(
                       BufferAllocator::AlignmentStrategy  strategy,
                       Allocator                          *basicAllocator = 0);
    explicit SequentialPool(int        initialSize,
                            Allocator *basicAllocator = 0);
    SequentialPool(int                                 initialSize,
                   BufferAllocator::AlignmentStrategy  strategy,
                   Allocator                          *basicAllocator = 0);
        // Create a sequential pool for allocating memory blocks from an
        // internal buffer.  Optionally specify an alignment 'strategy' used to
        // align allocated memory blocks.  If 'strategy' is not specified,
        // *Natural Alignment* is used.  Optionally specify an 'initialSize'
        // the absolute value of which indicates the initial size (in bytes)
        // for the internal buffer.  If 'initialSize' is not specified, an
        // implementation-defined value is used.  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.  If an 'allocate'
        // or 'reserveCapacity' request cannot be satisfied from the current
        // buffer, a new buffer is allocated, the size of which is determined
        // by a buffer growth strategy implied by 'initialSize'.  If
        // 'initialSize' was specified and is negative, or if it was *not*
        // specified, the buffer growth strategy used is *Geometric Growth*;
        // otherwise it is *Constant Growth*.  (See the component level
        // documentation for further details.)  In either case, the new buffer
        // will have sufficient capacity to satisfy the request.  If *Geometric
        // Growth* is in effect, no limit is imposed on the size of buffers.

    SequentialPool(char      *buffer,
                   int        bufferSize,
                   Allocator *basicAllocator = 0);
    SequentialPool(char                               *buffer,
                   int                                 bufferSize,
                   BufferAllocator::AlignmentStrategy  strategy,
                   Allocator                          *basicAllocator = 0);
        // Create a sequential pool for allocating memory blocks initially from
        // the specified 'buffer' the size (in bytes) of which is indicated by
        // the absolute value of the specified 'bufferSize'.  Optionally
        // specify an alignment 'strategy' used to align allocated memory
        // blocks.  If 'strategy' is not specified, *Natural Alignment* is
        // used.  Optionally specify a 'basicAllocator' used to supply memory.
        // If 'basicAllocator' is 0, the currently installed default allocator
        // is used.  If an 'allocate' or 'reserveCapacity' request cannot be
        // satisfied from the current buffer, a new buffer is allocated, the
        // size of which is determined by a buffer growth strategy implied by
        // 'bufferSize'.  If 'bufferSize' is negative, the buffer growth
        // strategy used is *Geometric Growth*; otherwise it is *Constant
        // Growth*.  (See the component level documentation for further
        // details.)  In either case, the new buffer will have sufficient
        // capacity to satisfy the request.  If *Geometric Growth* is in
        // effect, no limit is imposed on the size of buffers.

    SequentialPool(int        initialSize,
                   int        maxBufferSize,
                   Allocator *basicAllocator = 0);
    SequentialPool(int                                 initialSize,
                   int                                 maxBufferSize,
                   BufferAllocator::AlignmentStrategy  strategy,
                   Allocator                          *basicAllocator = 0);
        // Create a sequential pool for allocating memory blocks from an
        // internal buffer the initial size (in bytes) of which is indicated by
        // the absolute value of the specified 'initialSize'.  The specified
        // 'maxBufferSize' indicates the maximum size (in bytes) allowed for
        // internally allocated buffers.  Optionally specify an alignment
        // 'strategy' used to align allocated memory blocks.  If 'strategy' is
        // not specified, *Natural Alignment* is used.  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.  If an 'allocate'
        // or 'reserveCapacity' request cannot be satisfied from the current
        // buffer, a new buffer is allocated, the size of which is determined
        // by a buffer growth strategy implied by 'initialSize'.  If
        // 'initialSize' is negative the buffer growth strategy used is
        // *Geometric Growth*; otherwise it is *Constant Growth*.  (See the
        // component level documentation for further details.)  In either case,
        // the new buffer will have sufficient capacity to satisfy the request.
        // If *Geometric Growth* is in effect, the geometric progression of
        // buffer sizes is capped at 'maxBufferSize'.  The behavior is
        // undefined unless '0 < maxBufferSize', and
        // '|initialSize| <= maxBufferSize'.  Note that 'maxBufferSize' is
        // ignored if 'initialSize > 0'.  Also note that 'maxBufferSize' may be
        // overridden by a sufficiently large value passed to 'allocate' or
        // 'reserveCapacity'.

    SequentialPool(char      *buffer,
                   int        bufferSize,
                   int        maxBufferSize,
                   Allocator *basicAllocator = 0);
    SequentialPool(char                               *buffer,
                   int                                 bufferSize,
                   int                                 maxBufferSize,
                   BufferAllocator::AlignmentStrategy  strategy,
                   Allocator                          *basicAllocator = 0);
        // Create a sequential pool for allocating memory blocks initially from
        // the specified 'buffer' the size (in bytes) of which is indicated by
        // the absolute value of the specified 'bufferSize'.  The specified
        // 'maxBufferSize' indicates the maximum size (in bytes) allowed for
        // internally allocated buffers.  Optionally specify an alignment
        // 'strategy' used to align allocated memory blocks.  If 'strategy' is
        // not specified, *Natural Alignment* is used.  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.  If an 'allocate'
        // or 'reserveCapacity' request cannot be satisfied from the current
        // buffer, a new buffer is allocated, the size of which is determined
        // by a buffer growth strategy implied by 'bufferSize'.  If
        // 'bufferSize' is negative, the buffer growth strategy used is
        // *Geometric Growth*; otherwise it is *Constant Growth*.  (See the
        // component level documentation for further details.)  In either case,
        // the new buffer will have sufficient capacity to satisfy the request.
        // If *Geometric Growth* is in effect, the geometric progression of
        // buffer sizes is capped at 'maxBufferSize'.  The behavior is
        // undefined unless '0 < maxBufferSize', and
        // '|bufferSize| <= maxBufferSize'.  Note that 'maxBufferSize' is
        // ignored if 'bufferSize > 0'.  Also note that 'maxBufferSize' may be
        // overridden by a sufficiently large value passed to 'allocate' or
        // 'reserveCapacity'.

    ~SequentialPool();
        // Destroy this object and release all memory currently allocated
        // through this pool.

    // MANIPULATORS
    void *allocate(int size);
        // Return memory of the specified 'size'.  If 'size' is 0, no memory is
        // allocated and 0 is returned.  The behavior is undefined unless
        // '0 <= size'.

    void *allocateAndExpand(int *size);
        // Return memory of at least the specified '*size' and return the
        // actual amount of memory allocated in '*size'.  If '*size' is 0, no
        // memory is allocated and 0 is returned.  The behavior is undefined
        // unless '0 <= *size'.

    void *allocateAndExpand(int *size, int maxNumBytes);
        // Return memory of at least the specified '*size' and at most the
        // specified 'maxNumBytes'.  Also return the actual amount of memory
        // allocated in '*size'.  If '*size' is 0, no memory is allocated and 0
        // is returned.  The behavior is undefined unless
        // '0 <= *size <= maxNumBytes'.

    template <class TYPE>
    void deleteObjectRaw(const TYPE *object);
        // Destroy the specified 'object'.  Note that the memory is not
        // deallocated because there is no 'deallocate' method in a
        // 'bslma_sequentialpool'.

    template <class TYPE>
    void deleteObject(const TYPE *object);
        // Destroy the specified 'object'.  Note that this method is exactly
        // the same as the the 'deleteObjectRaw' method since no deallocation
        // is involved.  This method exists purely for consistency across
        // pools.

    int expand(void *address, int originalNumBytes);
        // Increase the amount of memory allocated at the specified 'address'
        // from the specified 'originalNumBytes' to the maximum amount easily
        // obtainable.  Return the amount of memory available at 'address'
        // after the expansion.  The behavior is undefined unless the call to
        // this allocator that provided the 'address' was performed with the
        // 'originalNumBytes'.  Note that this function will not expand the
        // memory unless there have been no allocations since the allocation
        // for 'originalNumBytes'.

    int expand(void *address, int originalNumBytes, int maxNumBytes);
        // Increase the amount of memory allocated at the specified 'address'
        // from the specified 'originalNumBytes' to the maximum amount easily
        // obtainable up to the specified 'maxNumBytes'.  Return the amount of
        // memory available at 'address' after the expansion.  The behavior is
        // undefined unless the call to this allocator that provided the
        // 'address' was performed with the 'originalNumBytes' and
        // 'originalNumBytes < maxNumBytes'.  Note that this function will not
        // expand the memory unless there have been no allocations since the
        // allocation for 'originalNumBytes'.

    void release();
        // Release all memory currently allocated through this pool.

    void reserveCapacity(int numBytes);
        // Reserve sufficient memory to satisfy allocation requests for at
        // least the specified 'numBytes' without replenishment (i.e., without
        // internal allocation).  The behavior is undefined unless
        // '0 <= numBytes'.

    int truncate(void *address, int originalNumBytes, int newNumBytes);
        // Reduce the amount of memory allocated at the specified 'address'
        // from the specified 'originalNumBytes' to the specified
        // 'newNumBytes'.  Return the amount of memory available at 'address'
        // after the truncation.  The behavior is undefined unless the call to
        // this allocator that provided the 'address' was performed with the
        // 'originalNumBytes' and 'newNumBytes <= originalNumBytes'.  Note that
        // this function will not truncate the memory unless there have been no
        // allocations since the allocation for 'originalNumBytes'.
};

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

                             // --------------
                             // SequentialPool
                             // --------------

// CREATORS
inline
SequentialPool::~SequentialPool()
{
}

// MANIPULATORS
template <class TYPE>
inline
void SequentialPool::deleteObjectRaw(const TYPE *object)
{
    if (0 != object) {
#ifndef BSLS_PLATFORM__CMP_SUN
        object->~TYPE();
#else
        const_cast<TYPE *>(object)->~TYPE();
#endif
    }
}

template <class TYPE>
inline
void SequentialPool::deleteObject(const TYPE *object)
{
    deleteObjectRaw(object);
}

}  // close package namespace

#if defined(BDE_BACKWARD_COMPATIBILITY) && 1 == BDE_BACKWARD_COMPATIBILITY
// ===========================================================================
//                           BACKWARD COMPATIBILITY
// ===========================================================================

typedef bslma::SequentialPool bslma_SequentialPool;
    // This alias is defined for backward compatibility.
#endif // BDE_BACKWARD_COMPATIBILITY

}  // close enterprise namespace

// FREE OPERATORS

// Note that the operators 'new' and 'delete' are declared outside the
// 'BloombergLP' namespace so that they do not hide the standard placement
// 'new' and 'delete' operators (i.e., 'void *operator new(size_t, void *)' and
// 'void operator delete(void *)').
//
// Note also that only the scalar versions of operators 'new' and 'delete' are
// provided, because overloading 'new' (and 'delete') with their array versions
// would cause dangerous ambiguity.  Consider what would have happened had we
// overloaded the array version of operator 'new':
//..
//  void *operator new[](std::size_t size,
//                       BloombergLP::bslma::Pool& pool)
//..
// The user of the pool class would have expected to be able to use
// 'new-expression':
//..
//  new (*pool) my_Type[...];
//..
// The problem is that this expression returns an array that cannot be safely
// deallocated.  On the one hand there is no syntax in C++ to invoke an
// overloaded operator delete; on the other hand the pointer returned by the
// 'new-expression' cannot be passed to the 'deallocate' method directly
// because the pointer is different from the one returned by the 'allocate'
// method.  The compiler offsets the value of this pointer by an extra header,
// which the compiler uses to maintain the number of the objects in the array
// (so that the 'delete-expression' knows how many objects it needs to
// destroy).

inline
void *operator new(std::size_t size, BloombergLP::bslma::SequentialPool& pool);
    // Return the memory allocated from the specified 'pool'.  The behavior is
    // undefined unless 'size' is the same as 'objectSize' that 'pool' has been
    // constructed with.  Note that an object may allocate additional memory
    // internally, requiring the allocator to be passed in as a constructor
    // argument:
    //..
    //  my_Type *newMyType(bslma::Pool *pool, bslma::Allocator *basicAllocator) {
    //      return new (*pool) my_Type(..., basicAllocator);
    //  }
    //..
    // Note also that the analogous version of operator 'delete' should not be
    // called directly.  Instead, this component provides a static template
    // member function 'deleteObject' parameterized by 'TYPE' that performs the
    // following:
    //..
    //  void deleteMyType(bslma::Pool *pool, my_Type *t) {
    //      t->~my_Type();
    //      pool->deallocate(t);
    //  }
    //..

inline
void operator delete(void *address, BloombergLP::bslma::SequentialPool& pool);
    // Use the specified 'pool' to deallocate the memory at the specified
    // 'address'.  The behavior is undefined unless 'address' was allocated
    // using 'pool' and has not already been deallocated.  This operator is
    // supplied solely to allow the compiler to arrange for it to be called in
    // case of an exception.

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

inline
void *operator new(std::size_t size, BloombergLP::bslma::SequentialPool& pool)
{
    return pool.allocate(static_cast<int>(size));
}

inline
void operator delete(void *, BloombergLP::bslma::SequentialPool&)
{
    // NOTE: there is no deallocation from this allocation mechanism.
}

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2007
//      All Rights Reserved.
//      Property of Bloomberg L.P.  (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
