// ball_recordbuffer.h                                                -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BALL_RECORDBUFFER
#define INCLUDED_BALL_RECORDBUFFER

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a protocol for managing log record handles.
//
//@CLASSES:
//  ball::RecordBuffer: protocol class for managing log record handles
//
//@SEE_ALSO: ball_record, ball_fixedsizerecordbuffer
//
//@DESCRIPTION: This component defines the base-level protocol,
// 'ball::RecordBuffer', for managing record handles (specifically instances of
// 'bsl::shared_ptr<ball::Record>') in a double-ended buffer.  In particular,
// the 'ball::RecordBuffer' protocol class provides abstract methods to push a
// record handle into either end (back or front) of the buffer ('pushBack' and
// 'pushFront'), to obtain read-only access to the log record positioned at
// either end ('back' and 'front') and to remove the record positioned at
// either end ('popBack' and 'popFront').  Note that the classes implementing
// this protocol are supposed to manage record handles and not the records
// themselves, specifically, they should not allocate the memory for records
// and should not explicitly destroy records (a record is destroyed
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
// This section illustrates intended use of this component.
//
///Example 1: Defining a Concrete 'RecordBuffer' Type
///- - - - - - - - - - - - - - - - - -- - - - - - - -
// This example shows the definition of a simple concrete record buffer.  The
// requisite steps are:
//
//: 1 Define a concrete class derived from 'ball::RecordBuffer'.
//: 2 Implement all pure virtual functions.
//
// The concrete thread-safe 'my_RecordBuffer' class in this example implements
// the 'ball::RecordBuffer' protocol by delegating to an instance of
// 'bsl::vector<bsl::shared_ptr<ball::Record> > ':
//..
//  // my_recordbuffer.h
//
//  class my_RecordBuffer : public ball::RecordBuffer {
//      // This class provides a thread-safe implementation of the
//      // 'ball::RecordBuffer' protocol.  This implementation employs a vector
//      // to hold an unlimited number of record handles.
//
//      mutable bslmt::RecursiveMutex d_mutex;
//                                   // thread safety provider (see
//                                   // the implementation notes for the
//                                   // justification for using recursive mutex
//                                   // rather a plain mutex)
//
//      bsl::vector<bsl::shared_ptr<ball::Record> >
//                         d_buffer; // buffer of record handles
//
//    private:
//      // NOT IMPLEMENTED
//      my_RecordBuffer(const my_RecordBuffer&);
//      my_RecordBuffer& operator=(const my_RecordBuffer&);
//
//    public:
//      // CREATORS
//      my_RecordBuffer();
//      virtual ~my_RecordBuffer();
//
//      // MANIPULATORS
//      virtual void beginSequence();
//      virtual void endSequence();
//      virtual void popBack();
//      virtual void popFront();
//      virtual int pushBack(const bsl::shared_ptr<ball::Record>& handle);
//      virtual int pushFront(
//                   const bsl::shared_ptr<ball::Record>& handle);
//      virtual void removeAll();
//
//      // ACCESSORS
//      virtual const bsl::shared_ptr<ball::Record>& back() const;
//      virtual const bsl::shared_ptr<ball::Record>& front() const;
//      virtual int length() const;
//  };
//..
//
///Implementation Notes
/// - - - - - - - - - -
// Recursive mutex (rather than plain mutex) is chosen to provide thread
// safety.  This allows the manipulation of the record buffer between the call
// to 'beginSequence' and 'endSequence'.  If we had used plain mutex, calling
// any method on the record buffer between the calls to 'beginSequence' and
// 'endSequence' would result in a deadlock.
//..
//  // CREATORS
//  inline
//  my_RecordBuffer::my_RecordBuffer() { }
//
//  // MANIPULATORS
//  inline
//  void my_RecordBuffer::beginSequence()
//  {
//      d_mutex.lock();
//  }
//
//  inline
//  void my_RecordBuffer::endSequence()
//  {
//      d_mutex.unlock();
//  }
//
//  inline
//  void my_RecordBuffer::popBack()
//  {
//      bslmt::LockGuard<bslmt::RecursiveMutex> guard(&d_mutex);
//      d_buffer.pop_back();
//  }
//
//  inline
//  void my_RecordBuffer::popFront()
//  {
//      bslmt::LockGuard<bslmt::RecursiveMutex> guard(&d_mutex);
//      d_buffer.erase(d_buffer.begin());
//  }
//
//  inline
//  int my_RecordBuffer::pushBack(
//                       const bsl::shared_ptr<ball::Record>& handle)
//  {
//      bslmt::LockGuard<bslmt::RecursiveMutex> guard(&d_mutex);
//      d_buffer.push_back(handle);
//      return 0;
//  }
//
//  inline
//  int my_RecordBuffer::pushFront(
//                       const bsl::shared_ptr<ball::Record>& handle)
//  {
//      bslmt::LockGuard<bslmt::RecursiveMutex> guard(&d_mutex);
//      d_buffer.insert(d_buffer.begin(), handle);
//      return 0;
//  }
//
//  inline
//  void my_RecordBuffer::removeAll()
//  {
//      bslmt::LockGuard<bslmt::RecursiveMutex> guard(&d_mutex);
//      d_buffer.clear();
//  }
//
//  // ACCESSORS
//  inline
//  const bsl::shared_ptr<ball::Record>& my_RecordBuffer::back() const
//  {
//      bslmt::LockGuard<bslmt::RecursiveMutex> guard(&d_mutex);
//      return d_buffer.back();
//  }
//
//  inline
//  const bsl::shared_ptr<ball::Record>& my_RecordBuffer::front() const
//  {
//      bslmt::LockGuard<bslmt::RecursiveMutex> guard(&d_mutex);
//      return d_buffer.front();
//  }
//
//  inline
//  int my_RecordBuffer::length() const
//  {
//      bslmt::LockGuard<bslmt::RecursiveMutex> guard(&d_mutex);
//      return d_buffer.size();
//  }
//..
// Note that we always implement a virtual destructor (non-inline) in the .cpp
// file (to indicate the *unique* location of the class's virtual table):
//..
//  // my_recordbuffer.cpp
//
//  my_RecordBuffer::~my_RecordBuffer() { }
//..
//
///Example 2: Using a 'ball::RecordBuffer'
///- - - - - - - - - - - - - - - - - - - -
// This example demonstrates working with the 'ball::RecordBuffer' protocol.
// We implement a function 'processRecord' that processes a specified record
// handle based on its severity.
//..
//  void processRecord(const bsl::shared_ptr<ball::Record>& handle,
//                           ball::RecordBuffer&            buffer)
//      // Process the specified 'handle', based on it's severity.  Records
//      // (encapsulated in 'handle') with severity equal to or *greater*
//      // severe than (i.e., with *numerical* value *less* than or equal to)
//      // 'ball::Severity::e_WARN' are pushed back into the specified
//      // 'buffer'.  Records with a severity equal to or more severe than
//      // 'ball::Severity::e_ERROR' are (in addition to get pushed back
//      // into the 'buffer') printed to 'stdout', and then each record
//      // contained in 'buffer' is in turn printed to 'stdout' and then
//      // removed from 'buffer'.  That is, any severity level equal to or
//      // more severe than 'ball::Severity::e_ERROR' triggers a trace-back
//      // of all records accumulated in the buffer and flushes the buffer.
//      // The function is thread safe and thus allows multiple concurrent
//      // invocations of this function with the same record buffer.
//
//  {
//      int severity = handle->fixedFields().severity();
//
//      if (ball::Severity::e_WARN >= severity) {
//          buffer.pushBack(handle);
//      }
//      if (ball::Severity::e_ERROR >= severity) {
//          bsl::cout << *handle;
//          buffer.beginSequence(); // lock the buffer before traversing
//          int length = buffer.length();
//          while (length--) {
//              bsl::cout << buffer.back();
//              buffer.popBack();
//          }
//          buffer.endSequence();   // unlock the buffer after traversing
//      }
//
//  }
//..


#ifndef INCLUDED_BALSCM_VERSION
#include <balscm_version.h>
#endif

#ifndef INCLUDED_BSL_MEMORY
#include <bsl_memory.h>
#endif

namespace BloombergLP {

namespace ball {

class Record;

                           // ==================
                           // class RecordBuffer
                           // ==================

class RecordBuffer {
    // Provide a protocol (or pure interface) for managing record handles
    // (specifically instances of 'bsl::shared_ptr<Record>').

  public:
    // CREATORS
    virtual ~RecordBuffer();
        // Remove all record handles stored in this record buffer and destroy
        // this record buffer.

    // MANIPULATORS
    virtual void popBack() = 0;
        // Remove from this record buffer the record handle positioned at the
        // back of the buffer.  The behavior is undefined unless
        // '0 < length()'.

    virtual void popFront() = 0;
        // Remove from this record buffer the record handle positioned at the
        // front of the buffer.  The behavior is undefined unless
        // '0 < length()'.

    virtual int pushBack(const bsl::shared_ptr<Record>& handle) = 0;
        // Append the specified 'handle' to the back of this record buffer.
        // Return 0 on success, and a non-zero value otherwise.  Note that
        // concrete implementations are permitted to remove records from the
        // buffer in order to attempt to accommodate a 'pushBack' request
        // (which implies that, after a successful call to 'pushBack', 'length'
        // is not guaranteed to be more than one, and an unsuccessful call to
        // 'pushBack' is permitted to leave the buffer empty).

    virtual int pushFront(const bsl::shared_ptr<Record>& handle) = 0;
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
    virtual const bsl::shared_ptr<Record>& back() const = 0;
        // Return a reference of the shared pointer referring to the record
        // positioned at the back of this record buffer.  The behavior is
        // undefined unless this record buffer has been locked by the
        // 'beginSequence' method and '0 < length()'.

    virtual const bsl::shared_ptr<Record>& front() const = 0;
        // Return a reference of the shared pointer referring to the record
        // positioned at the front of this record buffer.  The behavior is
        // undefined unless this record buffer has been locked by the
        // 'beginSequence' method and '0 < length()'.

    virtual int length() const = 0;
        // Return the number of record handles in this record buffer.
};

}  // close package namespace

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
