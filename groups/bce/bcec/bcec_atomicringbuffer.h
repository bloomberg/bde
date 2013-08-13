// bcec_atomicringbuffer.h                                          -*-C++-*-
#ifndef INCLUDED_ATOMICRINGBUFFER
#define INCLUDED_ATOMICRINGBUFFER

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a thread-enabled fixed-size queue of values.
//
//@CLASSES:
//  bcec_AtomicRingBuffer: thread-enabled fixed-size queue of 'TYPE' values
//
//@AUTHOR: Eric Winseman (ewinseman), Dave Schumann (dschuman)
//
//@DESCRIPTION: This component implements an efficient, thread-enabled 
// fixed-size queue of values.  This class is ideal for synchronization and
// communication between threads in a producer-consumer model.  Its API is
// identical to 'bcec_FixedQueue'.  In comparison with that type, it may occupy
// performs faster in benchmarks.  
//
///TEMPLATE REQUIREMENTS
///---------------------
// 'bcec_AtomicRingBuffer' is a template which stores items of a parameterized
// 'TYPE'.  'TYPE' must supply default and copy constructors and the assignment
//  operator; if the default constructors accept a 'bslma::Allocator*', 
// 'TYPE' must declare the Uses Allocator trait (see 'bslalg_TypeTraits') so
//  that the allocator of the queue is propagated to the elements.
//

#ifndef INCLUDED_BCESCM_VERSION
#include <bcescm_version.h>
#endif

#ifndef INCLUDED_BCEMT_SEMAPHORE
#include <bcemt_semaphore.h>
#endif

#ifndef INCLUDED_BCEMT_THREADUTIL
#include <bcemt_threadutil.h>
#endif

#ifndef INCLUDED_BCES_ATOMICTYPES
#include <bces_atomictypes.h>
#endif

#ifndef INCLUDED_BDES_BITUTIL
#include <bdes_bitutil.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITS
#include <bslalg_typetraits.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORM
#include <bsls_platform.h>
#endif

#ifndef INCLUDED_BSLS_PERFORMANCEHINT
#include <bsls_performancehint.h>
#endif

#ifndef INCLUDED_BSL_VECTOR
#include <bsl_vector.h>
#endif

#ifndef INCLUDED_BSLMF_SELECTTRAIT
#include <bslmf_selecttrait.h>
#endif

#ifndef INCLUDED_BSLALG_SCALARPRIMITIVES
#include <bslalg_scalarprimitives.h>
#endif

#ifndef INCLUDE_BSLMA_DESTRUCTORPROCTOR
#include <bslma_destructorproctor.h>
#endif

namespace BloombergLP {

class bcec_AtomicRingBuffer_Impl {
    // This component-private class holds type-independent data and utilities
    // used by bcec_AtomicRingBuffer. 
    enum {
        INDEX_STATE_OLD      = 0,
        INDEX_STATE_WRITING  = 1,
        INDEX_STATE_NEW      = 2,
        INDEX_STATE_READING  = 3,
        INDEX_STATE_MASK     = 0x3,
                               // roundUpToBinaryPower(<max state value>) - 1
        INDEX_STATE_SHIFT    = 0x2, 
                               // base2Log(INDEX_STATE_MASK)
        MAX_OP_INDEX = (1 << (8 * sizeof(int) - 2)) - 1,
        DISABLED_STATE_MASK   = MAX_OP_INDEX + 1,

        // We'll pad certain data members so they're stored on different
        // cache lines to prevent false sharing.   See bcec_fixedqueue
        // for a discussion of these constants.  This value affects 
        // performance only and not correctness. 
#ifdef BSLS_PLATFORM__CPU_POWERPC
        BCEC_PAD = 124
#else
        BCEC_PAD = 60
#endif
    };

    bces_AtomicInt                    d_pushIndex;
    const char                        d_pushIndexPad[BCEC_PAD]; 
    bces_AtomicInt                    d_popIndex;
    const char                        d_popIndexPad[BCEC_PAD];

    const unsigned int                d_capacity;
    const unsigned int                d_maxGeneration;
    const unsigned int                d_alignmentMod;
    bsl::vector<bces_AtomicInt>       d_states;    // index states

    bcec_AtomicRingBuffer_Impl(bsl::size_t       capacity,
                               bslma::Allocator *basicAllocator);
       // TBD DOC

    unsigned int incrementGeneration(unsigned int currGeneration, 
                                     unsigned int index);
    
    int  acquirePushIndex(unsigned int &generation, 
                          unsigned int &index);
    int  acquirePopIndex(unsigned int &generation, 
                         unsigned int &index, 
                         unsigned int &n);
    void disable();
    void enable();
       // TBD DOC ALL OF THESE

    // ACCESSORS
    bool isEnabled() const;        
    int length() const;
    
    
    template <typename TYPE>
    friend class bcec_AtomicRingBuffer;
};
    
template <typename TYPE>
class bcec_AtomicRingBuffer {
    // This class provides a thread-enabled, lock-free, fixed-size queue of
    // values.      
private:

    enum {
        INDEX_STATE_OLD      = bcec_AtomicRingBuffer_Impl::INDEX_STATE_OLD,
        INDEX_STATE_WRITING  = bcec_AtomicRingBuffer_Impl::INDEX_STATE_WRITING,
        INDEX_STATE_NEW      = bcec_AtomicRingBuffer_Impl::INDEX_STATE_NEW,
        INDEX_STATE_MASK     = bcec_AtomicRingBuffer_Impl::INDEX_STATE_MASK,
        INDEX_STATE_SHIFT    = bcec_AtomicRingBuffer_Impl::INDEX_STATE_SHIFT,
        BCEC_PAD             = bcec_AtomicRingBuffer_Impl::BCEC_PAD
    };

    // DATA
    bsl::vector<TYPE>                 d_elements;   
    const char                        d_elementsPad[BCEC_PAD]; 
    bcec_AtomicRingBuffer_Impl        d_impl;
    
    bcemt_Semaphore                   d_popControlSema;
    const char                        d_popControlSemaPad[BCEC_PAD];
    bces_AtomicInt                    d_numWaitingPushers; 
    bcemt_Semaphore                   d_pushControlSema;
    bces_AtomicInt                    d_numWaitingPoppers;
    bslma::Allocator                 *d_allocator_p;

    // NOT IMPLEMENTED
    bcec_AtomicRingBuffer(const bcec_AtomicRingBuffer&);
    bcec_AtomicRingBuffer& operator=(const bcec_AtomicRingBuffer&);

    // PRIVATE MANIPULATORS
    int  tryPushBackImpl(const TYPE& data);
    int  tryPopFrontImpl(TYPE* data);

    // TBD TBD -- copyOut must die
    void copyOut(TYPE &dst, TYPE &src);

    static void copyOut(TYPE &dst,
                        TYPE &src,
                        bslma::Allocator *allocator_p,
                        bslmf::SelectTraitCase<bsl::is_trivially_copyable>);

    static void copyOut(TYPE &dst,
                        TYPE &src,
                        bslma::Allocator *allocator_p,
                        bslmf::SelectTraitCase<>);
public:
    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(bcec_AtomicRingBuffer,
                                 bslalg_TypeTraitUsesBslmaAllocator);

    // CREATORS
    explicit
    bcec_AtomicRingBuffer(bsl::size_t       capacity,
                          bslma::Allocator *basicAllocator = 0);
       // Create a thread-enabled lock-free queue having the specified
       // 'capacity'. Optionally specify a 'basicAllocator' used to supply
       // memory.  If 'basicAllocator' is 0, the currently installed default
       // allocator is used.

    ~bcec_AtomicRingBuffer();
       // Destroy this object.  

    // MANIPULATORS
    int pushBack(const TYPE& object);
       // Append the specified 'object' to the back of this queue.  If 
       // the queue is full, block until the queue is not full or until
       // the queue is disabled.  Return 0 on success, and a nonzero value if
       // the queue is disabled.

    void popFront(TYPE* data);
    TYPE popFront();
       // Remove the element from the from of that queue and load that 
       // element into the specified 'data'.  If the queue is empty, block
       // until it is not empty.

    int tryPopFront(TYPE *data);
       // Remove the element from the front of this queue and load that
       // element into the specified 'data'. If this queue is empty, fail
       // immediately.  Return 0 on success, and a non-zero value
       // otherwise.  

    int tryPushBack(const TYPE &data);
       // Append the specified 'data' to the back of this queue. If the
       // queue is full, or if the queue is disabled, fail immediately.
       // Return 0 on success, and a non-zero value otherwise.
    
    void removeAll();
       // Remove all items from this queue. Note that this operation is not atomic; if other threads
       // are concurrently pushing items into the queue the result of length() after this function
       //  returns is not guaranteed to be 0. 

    void disable();
       // Disable this queue.  All subsequent invocations of 'pushBack' or
       // 'tryPushBack' will fail immediately.  If the queue is already
       // disabled, this method has no effect.  

    void enable();
       // Enable queuing.  If the queue is not disabled, this call has no
       // effect.  

    // ACCESSORS
    int size() const;
       // Return the maximum number of elements that may be stored 
       // in this queue.  

    int length() const;
       // Return the number of elements currently in this queue.  

    bool isFull() const;
       // Return 'true' if this queue is full (when the number of elements 
       // currently in this queue equals its capacity), or 'false' otherwise.  

    bool isEmpty() const;
       // Return 'true' if this queue is empty (has no elements), or 'false' 
       // otherwise.  

    bool isEnabled() const;
        // Return 'true' if this queue is enabled, and 'false' otherwise.  Note
        // that the queue is created in the "enabled" state.
};





                           // ======================
                           // class AtomicRingBuffer
                           // ======================
// CREATORS
template <typename TYPE>
bcec_AtomicRingBuffer<TYPE>::bcec_AtomicRingBuffer(
                                             bsl::size_t       capacity,
                                             bslma::Allocator *basicAllocator)
: d_elements(basicAllocator)  
, d_elementsPad()
, d_impl(capacity, basicAllocator)
, d_popControlSema(0)
, d_popControlSemaPad()
, d_numWaitingPushers(0)
, d_pushControlSema(0)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    d_elements.reserve(capacity);
}

template <typename TYPE>
bcec_AtomicRingBuffer<TYPE>::~bcec_AtomicRingBuffer()
{
  removeAll();
}

// PRIVATE MANIPULATORS

template <typename TYPE>
inline
void bcec_AtomicRingBuffer<TYPE>::copyOut(TYPE &dst,
                                          TYPE &src,
                                          bslma::Allocator *allocator_p,
                                          bslmf::SelectTraitCase<bsl::is_trivially_copyable>)
{
  dst = src;
}

template <typename TYPE>
inline
void bcec_AtomicRingBuffer<TYPE>::copyOut(TYPE &dst,
                                          TYPE &src,
                                          bslma::Allocator *allocator_p,
                                          bslmf::SelectTraitCase<>)
{
  bslma::DestructorProctor<TYPE> dp(&src);
  bslalg::ScalarPrimitives::copyConstruct(&dst, src, allocator_p);
}

template <typename TYPE>
inline
void bcec_AtomicRingBuffer<TYPE>::copyOut(TYPE &dst,
                                          TYPE &src)
{
  typedef typename bslmf::SelectTrait<TYPE,
                                      bsl::is_trivially_copyable>::Type 
      Selection;

  copyOut(dst, src, d_allocator_p, Selection());
}

template <typename TYPE>
int bcec_AtomicRingBuffer<TYPE>::tryPushBackImpl(const TYPE& data)
{
    unsigned int generation;
    unsigned int index;
    
    int retval = d_impl.acquirePushIndex(generation, index);
    
    if (0 != retval) {
        return retval;
    }
    
    // save data, mark as new
    bslalg::ScalarPrimitives::copyConstruct(&d_elements[index], 
                                            data, 
                                            d_allocator_p);
    d_impl.d_states[index] = 
        INDEX_STATE_NEW | (generation << INDEX_STATE_SHIFT);
    
    // notify poppers of available data
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(d_numWaitingPoppers)) {
        d_popControlSema.post();
    }
    
  return 0;
}

template <typename TYPE>
int bcec_AtomicRingBuffer<TYPE>::tryPopFrontImpl(TYPE *data)
{
  unsigned int generation;
  unsigned int index;
  unsigned int n;
  int retval = d_impl.acquirePopIndex(generation, index, n);

  if (0 != retval) {
    return retval;
  }

  // copy data
  copyOut(*data, d_elements[index]);
  
  // increment generation count and release element
  d_impl.d_states[index] = INDEX_STATE_OLD | 
      (d_impl.incrementGeneration(generation, index) << INDEX_STATE_SHIFT);

  // notify pusher of available element
  if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(d_numWaitingPushers)) {
    d_pushControlSema.post();
  }

  return 0;
}

// MANIPULATORS
template <typename TYPE>
int bcec_AtomicRingBuffer<TYPE>::pushBack(const TYPE &data)
{
  int retval;
  while (0 != (retval = tryPushBackImpl(data)))
  {
    if (-2 == retval) {
      return -2;
    }

    d_numWaitingPushers.relaxedAdd(1);

    if (isFull() && isEnabled()) {
      d_pushControlSema.wait();
    }
    
    d_numWaitingPushers.relaxedAdd(-1);
  }

  return 0;
}

template <typename TYPE>
void bcec_AtomicRingBuffer<TYPE>::popFront(TYPE *data)
{
  while(0 != tryPopFrontImpl(data)) {
    d_numWaitingPoppers.relaxedAdd(1);

    if (isEmpty()) {
      d_popControlSema.wait();
    }

    d_numWaitingPoppers.relaxedAdd(-1);
  }
}

template <typename TYPE>
TYPE bcec_AtomicRingBuffer<TYPE>::popFront()
{
  unsigned int n;
  unsigned int generation;
  unsigned int index;

  while(0 != d_impl.acquirePopIndex(generation, index, n)) {
    d_numWaitingPoppers.relaxedAdd(1);

    if (isEmpty()) {
      d_popControlSema.wait();
    }

    d_numWaitingPoppers.relaxedAdd(-1);
  }

  // TBD SWAP?!
  TYPE result(d_elements[index]);
  d_elements[index].~TYPE();
  
  // increment generation count and release element
  d_impl.d_states[index] = INDEX_STATE_OLD | 
      (d_impl.incrementGeneration(generation, index) << INDEX_STATE_SHIFT);

  // notify pusher of available element
  if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(d_numWaitingPushers)) {
    d_pushControlSema.post();
  }

  return result;
}           

template <typename TYPE>
int bcec_AtomicRingBuffer<TYPE>::tryPushBack(const TYPE& data)
{
  if (isFull()) {
    return -1;
  }

  return tryPushBackImpl(data);
}
     
template <typename TYPE>
int bcec_AtomicRingBuffer<TYPE>::tryPopFront(TYPE* data)
{
  return tryPopFrontImpl(data);
}

template <typename TYPE>
void bcec_AtomicRingBuffer<TYPE>::removeAll()
{
    const int numItems = length();
    int poppedItems = 0;
    unsigned char buffer[sizeof(TYPE)];
    TYPE *t(reinterpret_cast<TYPE*>(buffer));
    
    while (poppedItems++ < numItems) {
        if(tryPopFront(t))
            break;
    }
  
    int numWakeUps = bsl::min(poppedItems, (int) d_numWaitingPushers);
    while (numWakeUps--) {
        // Wake up waiting pushers.
        d_pushControlSema.post();
    }
}

template <typename TYPE>
void bcec_AtomicRingBuffer<TYPE>::disable()
{
    d_impl.disable();
    
    const int numWaitingPushers = d_numWaitingPushers;
    
    for (int i = 0; i < numWaitingPushers; ++i) {
        d_pushControlSema.post();
    }
}
 
template <typename TYPE>
inline
void bcec_AtomicRingBuffer<TYPE>::enable() {
    d_impl.enable();
}

// ACCESSORS    
template <typename TYPE>
inline
int bcec_AtomicRingBuffer<TYPE>::size() const {
    return d_impl.d_capacity;
}

template <typename TYPE>
inline
int bcec_AtomicRingBuffer<TYPE>::length() const {
    return d_impl.length();
}

template <typename TYPE>
inline
bool bcec_AtomicRingBuffer<TYPE>::isFull() const {
    return (size() <= length());
}
    
template <typename TYPE>
inline
bool bcec_AtomicRingBuffer<TYPE>::isEmpty() const {
    return (0 >= length());
}

template <typename TYPE>
inline
bool bcec_AtomicRingBuffer<TYPE>::isEnabled() const {
    return d_impl.isEnabled();
}

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2012
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
