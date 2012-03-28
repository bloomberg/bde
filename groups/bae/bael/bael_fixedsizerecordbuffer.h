// bael_fixedsizerecordbuffer.h                                       -*-C++-*-
#ifndef INCLUDED_BAEL_FIXEDSIZERECORDBUFFER
#define INCLUDED_BAEL_FIXEDSIZERECORDBUFFER

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a thread-safe fixed-size buffer of record handles.
//
//@CLASSES:
//  bael_FixedSizeRecordBuffer: thread-safe fixed-size buffer of record handles
//
//@SEE_ALSO: bael_recordbuffer
//
//@AUTHOR: Ujjwal Bhoota (ubhoota)
//
//@DESCRIPTION: This component provides a concrete thread-safe implementation
// of the 'bael_RecordBuffer' protocol.  The sum of sizes of all records
// contained in a 'bael_FixedSizeRecordBuffer' object *plus* the amount of
// memory allocated by the 'bael_FixedSizeRecordBuffer' object itself is
// guaranteed to be less than or equal to an upper bound specified at creation.
//..
//              ( bael_FixedSizeRecordBuffer )
//                            |              ctor
//                            V
//                  ( bael_RecordBuffer )
//                                           dtor
//                                           beginSequence
//                                           endSequence
//                                           popBack
//                                           popFront
//                                           pushBack
//                                           pushFront
//                                           removeAll
//                                           length
//                                           back
//                                           front
//..
// The thread-safe class 'bael_FixedSizeRecordBuffer' manages record handles
// (specifically, the instances of 'bcema_SharedPtr<bael_Record>') in a
// double-ended buffer.  At any time, the sum of sizes of all records contained
// in a 'bael_FixedSizeRecordBuffer' object *plus* the amount of memory
// allocated by the 'bael_FixedSizeRecordBuffer' object itself is guaranteed to
// be less than or equal to an upper bound specified at creation.  In order to
// accommodate a record, existing records may be removed from the buffer (see
// below).  The 'bael_FixedSizeRecordBuffer' class provides methods to push a
// record handle into either end (back or front) of the buffer ('pushBack' and
// 'pushFront'), to obtain read-only access to the log record positioned at
// either end ('back' and 'front') and to remove the record positioned at
// either end ('popBack' and 'popFront').  In order to accommodate a 'pushBack'
// request, the records from the front end of the buffer may be removed.
// Similarly, in order to accommodate a 'pushFront' request, the records from
// the back end of the buffer may be removed.  If a record can not be
// accommodated in the buffer, it is silently (but otherwise safely) discarded.
//
///Usage
///-----
// In the following example we demonstrate creation of a limited record
// buffer followed by concurrent access to it by multiple threads.
//..
//    enum {
//        KILO_BYTE      = 1024,   // one kilo is (2^10) bytes
//        MAX_TOTAL_SIZE = 32 * K, // 'maxTotalSize' parameter
//        NUM_ITERATIONS = 1000,   // number of iterations
//        NUM_THREADS    = 4       // number of threads
//    };
//    bslma_Allocator *basicAllocator = bslma_Default::defaultAllocator();
//..
// First we create a record buffer.
//..
//    bcema_DefaultDeleter<bael_Record> recordDeleter(basicAllocator);
//    bael_FixedSizeRecordBuffer recordBuffer(MAX_TOTAL_SIZE, basicAllocator);
//..
// Note that since the record buffer will contain shared pointers to the
// records, 'recordDeleter' must be created before 'recordBuffer' to
// ensure that the former has the longer lifetime.
//
// Now we create several threads each of which repeatedly performs the
// following operations in a tight loop;
//  (1) create a record;
//  (2) build a message and store it into the record;
//  (3) create a record handle;
//  (4) push this record handle at the back end of of the record buffer
//
//..
//    void *workerThread(void *arg)
//    {
//        int id = (int)arg; // thread id
//        for(int i = 0; i < NUM_ITERATIONS; ++i){
//            bael_Record *record =
//                           new (*basicAllocator) bael_Record(basicAllocator);
//
//            // build a message
//            enum { MAX_SIZE = 100 };
//            char msg[MAX_SIZE];
//            sprintf(msg, "message no. %d from thread no. %d", i, id);
//
//            record->getFixedFields().setMessage(msg);
//
//            bcema_SharedPtr<bael_Record>
//              handle(record, &recordDeleter, basicAllocator);
//
//            recordBuffer.pushBack(handle);
//        }
//..
// After completing the loop each thread iterates, in LIFO order, over all of
// the records contained in record buffer.
//..
//        // print messages in LIFO order
//        recordBuffer.beginSequence();
//        while (recordBuffer.length()) {
//            const bael_Record &rec = recordBuffer.back();
//            bsl::cout << rec.getFixedFields().message() << bsl::endl;
//            recordBuffer.popBack();
//        }
//        recordBuffer.endSequence();
//
//        return NULL;
//    }
//..

#ifndef INCLUDED_BAESCM_VERSION
#include <baescm_version.h>
#endif

#ifndef INCLUDED_BAEL_COUNTINGALLOCATOR
#include <bael_countingallocator.h>
#endif

#ifndef INCLUDED_BAEL_RECORD
#include <bael_record.h>
#endif

#ifndef INCLUDED_BAEL_RECORDBUFFER
#include <bael_recordbuffer.h>
#endif

#ifndef INCLUDED_BCEMA_SHAREDPTR
#include <bcema_sharedptr.h>
#endif

#ifndef INCLUDED_BCEMT_LOCKGUARD
#include <bcemt_lockguard.h>
#endif

#ifndef INCLUDED_BCEMT_THREAD
#include <bcemt_thread.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORMUTIL
#include <bsls_platformutil.h>
#endif

#ifndef INCLUDED_BSL_DEQUE
#include <bsl_deque.h>
#endif

#ifndef INCLUDED_BSLFWD_BSLMA_ALLOCATOR
#include <bslfwd_bslma_allocator.h>
#endif

namespace BloombergLP {

                          // ================================
                          // class bael_FixedSizeRecordBuffer
                          // ================================

class bael_FixedSizeRecordBuffer: public bael_RecordBuffer {
    // This class provides a concrete, thread-safe implementation of the
    // 'bael_RecordBuffer' protocol.  This class is a mechanism.  At any
    // time, the sum of sizes of all records contained in a
    // 'bael_FixedSizeRecordBuffer' object *plus* the amount of memory
    // allocated by the 'bael_FixedSizeRecordBuffer' object itself is
    // guaranteed to be less than or equal to an upper bound specified at
    // creation.  The class is thread-safe, except that the methods 'front'
    // and 'back' must be called after locking the buffer by invoking
    // 'beginSequence'.  In order to accommodate a 'pushBack' request, the
    // records from the front end of the buffer may be removed.  Similarly,
    // in order to accommodate a 'pushFront' request, the records from the
    // back end of the buffer may be removed.  If a record can not
    // accommodate in the buffer, it is silently (but otherwise safely)
    // discarded.

    // DATA
    mutable bcemt_RecursiveMutex d_mutex;        // synchronizes access to
                                                 // the buffer

    int                          d_maxTotalSize; // maximum possible sum of
                                                 // sizes of contained records

    int                          d_currentTotalSize;
                                                 // current sum of sizes of
                                                 // contained records

    bael_CountingAllocator       d_allocator;    // allocator for 'd_deque'

    bsl::deque<bcema_SharedPtr<bael_Record> >
                                 d_deque;        // deque of record handles

    // NOT IMPLEMENTED
    bael_FixedSizeRecordBuffer(const bael_FixedSizeRecordBuffer&);
    bael_FixedSizeRecordBuffer& operator=(const bael_FixedSizeRecordBuffer&);

  public:
    // CREATORS
    bael_FixedSizeRecordBuffer(int maxTotalSize, bslma_Allocator *allocator=0);
        // Create a limited record buffer such that at any time the sum of
        // sizes of all records contained *plus* the amount of memory
        // allocated by this object is guaranteed to be less than or equal
        // to the specified 'maxTotalSize'.  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is
        // 0, the currently installed default allocator is used.  The behavior
        // is undefined unless 'maxTotalSize > 0'.

   virtual ~bael_FixedSizeRecordBuffer();
        // Remove all record handles from this record buffer and
        // destroy this record buffer.

    // MANIPULATORS
    virtual void beginSequence();
        // *Lock* this record buffer so that a sequence of method
        // invocations on this record buffer can occur uninterrupted by
        // other threads.  The buffer will remain *locked* until
        // 'endSequence' is called.  It is valid to invoke other methods
        // on this record buffer between the calls to 'beginSequence'
        // and 'endSequence' (the implementation guarantees this by employing
        // a recursive mutex).

    virtual void endSequence();
        // *Unlock* this record buffer, thus allowing other threads to
        // access it.  The behavior is undefined unless the buffer is
        // already *locked* by 'beginSequence'.

    virtual void popBack();
        // Remove from this record buffer the record handle positioned at the
        // back end of the buffer.  The behavior is undefined
        // unless 0 < length().

    virtual void popFront();
        // Remove from this record buffer the record handle positioned at the
        // front end of the buffer.  The behavior is undefined
        // unless 0 < length().

    virtual int pushBack(const bcema_SharedPtr<bael_Record>& handle);
        // Push the specified 'handle' at the back end of this record
        // buffer.  Return 0 on success, and a non-zero value otherwise.
        // In order to accommodate a record, the records from the front
        // end of the buffer may be removed.  If a record can not be
        // accommodated in the buffer, it is silently discarded.

    virtual int pushFront(const bcema_SharedPtr<bael_Record>& handle);
        // Push the specified 'handle' at the front end of this record
        // buffer.  Return 0 on success, and a non-zero value otherwise.
        // In order to accommodate a record, the records from the end end
        // of the buffer may be removed.  If a record can not be accommodated
        // in the buffer, it is silently discarded.

    virtual void removeAll();
        // Remove all record handles stored in this record buffer.  Note
        // that 'length()' is now 0.

    // ACCESSORS
    virtual const bael_Record& back() const;
        // Return a reference to the non-modifiable log record positioned at
        // the back end of this record buffer.  The behavior is undefined
        // unless this record buffer has been locked by the 'beginSequence'
        // method and unless '0 < length()'.

    virtual const bael_Record& front() const;
        // Return a reference to the non-modifiable log record positioned at
        // the front end of this record buffer.  The behavior is undefined
        // unless this record buffer has been locked by the 'beginSequence'
        // method and unless '0 < length()'.

    virtual int length() const;
        // Return the number of record handles in this record buffer.
};

//-----------------------------------------------------------------------------
//                      INLINE FUNCTION DEFINITIONS
//-----------------------------------------------------------------------------

                          // --------------------------------
                          // class bael_FixedSizeRecordBuffer
                          // --------------------------------

// CREATORS
inline
bael_FixedSizeRecordBuffer::bael_FixedSizeRecordBuffer(
                                                 int              maxTotalSize,
                                                 bslma_Allocator *allocator)
: d_maxTotalSize(maxTotalSize)
, d_currentTotalSize(0)
, d_allocator(allocator)
, d_deque(&d_allocator)
{
}

// MANIPULATORS
inline
void bael_FixedSizeRecordBuffer::beginSequence()
{
    d_mutex.lock();
}

inline
void bael_FixedSizeRecordBuffer::endSequence()
{
    d_mutex.unlock();
}

inline
void bael_FixedSizeRecordBuffer::removeAll()
{
    bcemt_LockGuard<bcemt_RecursiveMutex> guard(&d_mutex);
    d_deque.clear();
    d_currentTotalSize = 0;
}

// ACCESSORS
inline
const bael_Record& bael_FixedSizeRecordBuffer::back() const
{
    bcemt_LockGuard<bcemt_RecursiveMutex> guard(&d_mutex);
    return *d_deque.back();
}

inline
const bael_Record& bael_FixedSizeRecordBuffer::front() const
{
    bcemt_LockGuard<bcemt_RecursiveMutex> guard(&d_mutex);
    return *d_deque.front();
}

inline
int bael_FixedSizeRecordBuffer::length() const
{
    bcemt_LockGuard<bcemt_RecursiveMutex> guard(&d_mutex);
    return static_cast<int>(d_deque.size());
}

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2003
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
