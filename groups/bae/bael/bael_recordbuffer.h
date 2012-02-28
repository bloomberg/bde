// bael_recordbuffer.h                 -*-C++-*-
#ifndef INCLUDED_BAEL_RECORDBUFFER
#define INCLUDED_BAEL_RECORDBUFFER

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")


//@PURPOSE: Provide a protocol for managing log record handles.
//
//@CLASSES:
//  bael_RecordBuffer: protocol class for managing log record handles
//
//@SEE_ALSO: bael_record, bael_fixedsizerecordbuffer
//
//@AUTHOR: Hong Shi (hshi2)
//
//@DESCRIPTION: This component defines the base-level protocol for managing
// record handles (specifically instances of
// 'bcema_SharedPtr<bael_Record>') in a double-ended buffer.  In
// particular, the 'bael_RecordBuffer' protocol class provides abstract methods
// to push a record handle into either end (back or front) of the buffer
// ('pushBack' and 'pushFront'), to obtain read-only access to the log record
// positioned at either end ('back' and 'front') and to remove the record
// positioned at either end ('popBack' and 'popFront').  Note that the classes
// implementing this protocol are supposed to manage record handles and not the
// records themselves, specifically, they should not allocate the memory for
// records and should not explicitly destroy records (a record is destroyed
// automatically when the reference count associated with its handle becomes
// zero).  The push methods ('pushBack' and 'pushFront') are not guaranteed to
// succeed.  Concrete implementations implementations are permitted to remove
// records from the buffer in order to attempt to accommodate a push request
// (which implies that, after a successful call to a push method, 'length' is
// not guaranteed to be more than one, and an unsuccessful call to a push
// method is permitted to leave the buffer empty).
//
///Usage
///-----
//
///Example 1
///- - - - -
// This example shows the definition of a simple concrete record
// buffer.  The requisite steps are:
//..
//    1. Define a concrete class derived from 'bael_RecordBuffer'.
//    2. Implement all pure virtual functions.
//..
// The concrete thread-safe 'my_RecordBuffer' class in this example
// implements the 'bael_RecordBuffer' protocol by delegating to an instance of
// 'bsl::vector<bcema_SharedPtr<bael_Record> > ':
//..
// my_recordbuffer.h
//
//    #include <bcemt_lockguard.h>
//    #include <bcemt_thread.h>
//
//    #include <vector>
//
//    namespace BloombergLP {
//    class my_RecordBuffer : public bael_RecordBuffer {
//        // This class provides a thread-safe implementation of the
//        // 'bael_RecordBuffer' protocol.  This implementation
//        // employs a vector to hold an unlimited number of record
//        // handles.
//
//        mutable bcemt_RecursiveMutex d_mutex; // thread safety provider (see
//                                              // the implementation notes for
//                                              // the justification for using
//                                              // recursive mutex rather a
//                                              // plain mutex)
//
//        bsl::vector<bcema_SharedPtr<bael_Record> >
//                                     d_buffer; // buffer of record handles
//
//      private:
//        // NOT IMPLEMENTED
//        my_RecordBuffer(const my_RecordBuffer&);
//        my_RecordBuffer& operator=(const my_RecordBuffer&);
//
//      public:
//        // CREATORS
//        my_RecordBuffer();
//        virtual ~my_RecordBuffer();
//
//        // MANIPULATORS
//        virtual void beginSequence();
//        virtual void endSequence();
//        virtual void popBack();
//        virtual void popFront();
//        virtual int pushBack(const bcema_SharedPtr<bael_Record>& handle);
//        virtual int pushFront(
//                             const bcema_SharedPtr<bael_Record>& handle);
//        virtual void removeAll();
//
//        // ACCESSORS
//        virtual const bael_Record& back() const;
//        virtual const bael_Record& front() const;
//        virtual int length() const;
//    };
//..
///Implementation Notes
///- - - - - - - - - - -
// Recursive mutex (rather than plain mutex) is chosen to provide
// thread safety.  This allows the manipulation of the record buffer between
// the call to 'beginSequence' and 'endSequence'.  If we had used plain
// mutex, calling any method on the record buffer between the calls to
// 'beginSequence' and 'endSequence' would result in a deadlock.
//..
//    // CREATORS
//    inline
//    my_RecordBuffer::my_RecordBuffer() { }
//
//    // MANIPULATORS
//    inline
//    void my_RecordBuffer::beginSequence()
//    {
//        d_mutex.lock();
//    }
//
//    inline
//    void my_RecordBuffer::endSequence()
//    {
//        d_mutex.unlock();
//    }
//
//    inline
//    void my_RecordBuffer::popBack()
//    {
//        bcemt_LockGuard<bcemt_RecursiveMutex> guard(&d_mutex);
//        d_buffer.pop_back();
//    }
//
//    inline
//    void my_RecordBuffer::popFront()
//    {
//        bcemt_LockGuard<bcemt_RecursiveMutex> guard(&d_mutex);
//        d_buffer.erase(d_buffer.begin());
//    }
//
//    inline
//    int my_RecordBuffer::pushBack(
//                                  const bcema_SharedPtr<bael_Record>& handle)
//    {
//        bcemt_LockGuard<bcemt_RecursiveMutex> guard(&d_mutex);
//        d_buffer.push_back(handle);
//        return 0;
//    }
//
//    inline
//    int my_RecordBuffer::pushFront(
//                                  const bcema_SharedPtr<bael_Record>& handle)
//    {
//        bcemt_LockGuard<bcemt_RecursiveMutex> guard(&d_mutex);
//        d_buffer.insert(d_buffer.begin(), handle);
//        return 0;
//    }
//
//    inline
//    void my_RecordBuffer::removeAll()
//    {
//        bcemt_LockGuard<bcemt_RecursiveMutex> guard(&d_mutex);
//        d_buffer.clear();
//    }
//
//    // ACCESSORS
//    inline
//    const bael_Record& my_RecordBuffer::back() const
//    {
//        bcemt_LockGuard<bcemt_RecursiveMutex> guard(&d_mutex);
//        return *(d_buffer.back());
//    }
//
//    inline
//    const bael_Record& my_RecordBuffer::front() const
//    {
//        bcemt_LockGuard<bcemt_RecursiveMutex> guard(&d_mutex);
//        return *(d_buffer.front());
//    }
//
//    inline
//    int my_RecordBuffer::length() const
//    {
//        bcemt_LockGuard<bcemt_RecursiveMutex> guard(&d_mutex);
//        return d_buffer.size();
//    }
//    } // namespace BloombergLP
//..
// Note that we always implement a virtual destructor (non-inline) in the
// .cpp file (to indicate the *unique* location of the class's virtual table):
//..
//    // my_recordbuffer.cpp
//
//    namespace BloombergLP {
//    my_RecordBuffer::~my_RecordBuffer() { }
//    }  // close namespace BloombergLP
//..
///Example 2
///- - - - -
// This example demonstrates working with the 'bael_RecordBuffer' protocol.
// We implement a function 'processRecord' that processes a specified record
// handle based on its severity.
//..
//    void processRecord(const bcema_SharedPtr<bael_Record>& handle,
//                             bael_RecordBuffer&            buffer)
//        // Process the specified 'handle', based on it's severity.
//        // Records (encapsulated in 'handle') with severity equal to or
//        // *greater* severe than (i.e., with *numerical* value *less*
//        // than or equal to) 'bael_Severity::BAEL_WARN' are pushed back into
//        // the specified 'buffer'.  Records with a severity equal to or
//        // more severe than 'bael_Severity::BAEL_ERROR' are (in addition to
//        // get pushed back into the 'buffer') printed to 'stdout', and
//        // then each record contained in 'buffer' is in turn printed to
//        // 'stdout' and then removed from 'buffer'.  That is, any
//        // severity level equal to or more severe than
//        // 'bael_Severity::BAEL_ERROR' triggers a trace-back of all records
//        // accumulated in the buffer and flushes the buffer.  The
//        // function is thread safe and thus allows multiple concurrent
//        // invocations of this function with the same record buffer.
//    {
//        int severity = handle->fixedFields().severity();
//
//        if (bael_Severity::BAEL_WARN >= severity) {
//            buffer.pushBack(handle);
//        }
//        if (bael_Severity::BAEL_ERROR >= severity) {
//            bsl::cout << *handle;
//            buffer.beginSequence(); // lock the buffer before traversing
//            int length = buffer.length();
//            while (length--) {
//                bsl::cout << buffer.back();
//                buffer.popBack();
//            }
//            buffer.endSequence();   // unlock the buffer after traversing
//        }
//
//    }
//..

#ifndef INCLUDED_BAESCM_VERSION
#include <baescm_version.h>
#endif

#ifndef INCLUDED_BCEMA_SHAREDPTR
#include <bcema_sharedptr.h>
#endif

namespace BloombergLP {

class bael_Record;
                           // =======================
                           // class bael_RecordBuffer
                           // =======================

class bael_RecordBuffer {
    // Provide a protocol (or pure interface) for managing record handles
    // (specifically instances of 'bcema_SharedPtr<bael_Record>').

  public:
    // CREATORS
    virtual ~bael_RecordBuffer();
        // Remove all record handles stored in this record buffer and
        // destroy this record buffer.

    // MANIPULATORS
    virtual void popBack() = 0;
        // Remove from this record buffer the record handle positioned at the
        // back of the buffer.  The behavior is undefined unless
        // '0 < length()'.

    virtual void popFront() = 0;
        // Remove from this record buffer the record handle positioned at the
        // front of the buffer.  The behavior is undefined unless
        // '0 < length()'.

    virtual int pushBack(const bcema_SharedPtr<bael_Record>& handle) = 0;
        // Append the specified 'handle' to the back of this record buffer.
        // Return 0 on success, and a non-zero value otherwise.  Note that
        // concrete implementations are permitted to remove records from the
        // buffer in order to attempt to accommodate a 'pushBack' request
        // (which implies that, after a successful call to 'pushBack', 'length'
        // is not guaranteed to be more than one, and an unsuccessful call to
        // 'pushBack' is permitted to leave the buffer empty).

    virtual int pushFront(const bcema_SharedPtr<bael_Record>& handle) = 0;
        // Insert the specified 'handle' at the front of this record buffer.
        // Return 0 on success, and a non-zero value otherwise.  Note that
        // concrete implementations are permitted to remove records from the
        // buffer in order to attempt to accommodate a 'pushFront' request
        // (which implies that, after a successful call to 'pushFront',
        // 'length' is not guaranteed to be more than one, and an unsuccessful
        // call to 'pushFront' is permitted to leave the buffer empty).

    virtual void removeAll() = 0;
        // Remove all record handles stored in this record buffer.  Note that
        // 'length()' is now 0.

    virtual void beginSequence() = 0;
        // *Lock* this record buffer so that a sequence of method invocations
        // on this record buffer can occur uninterrupted by other threads.  The
        // buffer will remain *locked* until 'endSequence' is called.

    virtual void endSequence() = 0;
        // *Unlock* this record buffer, thus allowing other threads to access
        // it.  The behavior is undefined unless the buffer is already *locked*
        // by 'beginSequence' method.

    // ACCESSORS
    virtual const bcema_SharedPtr<bael_Record>& back() const = 0;
        // Return a reference of the shared pointer referring to the record
        // positioned at the back of this record buffer.  The behavior is
        // undefined unless this record buffer has been locked by the
        // 'beginSequence' method and '0 < length()'.

    virtual const bcema_SharedPtr<bael_Record>& front() const = 0;
        // Return a reference of the shared pointer referring to the record
        // positioned at the front of this record buffer.  The behavior is
        // undefined unless this record buffer has been locked by the
        // 'beginSequence' method and '0 < length()'.

    virtual int length() const = 0;
        // Return the number of record handles in this record buffer.
};

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2004
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
