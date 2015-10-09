// bdlma_concurrentallocatoradapter.h                                 -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BDLMA_CONCURRENTALLOCATORADAPTER
#define INCLUDED_BDLMA_CONCURRENTALLOCATORADAPTER

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a thread-enabled adapter for the allocator protocol.
//
//@CLASSES:
//   bdlma::ConcurrentAllocatorAdapter: thread-enabled allocator adapter
//
//@SEE_ALSO: bslma_allocator, bdlma_concurrentmultipool
//
//@DESCRIPTION: This component provides an adapter,
// 'bdlma::ConcurrentAllocatorAdapter', that implements the 'bslma::Allocator'
// protocol and provides synchronization for operations on an allocator
// supplied at construction using a mutex also supplied at construction.
//..
//   ,-----------------------------------.
//  (  bdlma::ConcurrentAllocatorAdapter  )
//   `-----------------------------------'
//                     |                ctor/dtor
//                     V
//             ,-----------------.
//            ( bslma::Allocator  )
//             `-----------------'
//                            allocate
//                            deallocate
//..
//
///Thread Safety
///-------------
// 'bdlma::ConcurrentAllocatorAdapter' is *thread-enabled*, meaning any
// operation on the same instance can be safely invoked from any thread.
//
///Usage
///-----
// In the following usage example, we develop a simple 'AddressBook' class
// containing two thread-enabled vectors of strings: one for names, the other
// for addresses.  We use a 'bdlma::ConcurrentAllocatorAdapter' to synchronize
// memory allocations across our two thread-enabled vectors.  For the purpose
// of this discussion, we first define a simple thread-enabled vector:
//..
//  template <class TYPE>
//  class ThreadEnabledVector {
//      // This class defines a trivial thread-enabled vector.
//
//      // DATA
//      mutable bslmt::Mutex d_mutex;     // synchronize access
//      bsl::vector<TYPE>    d_elements;  // underlying list of strings
//
//      // NOT IMPLEMENTED
//      ThreadEnabledVector(const ThreadEnabledVector&);
//      ThreadEnabledVector& operator=(const ThreadEnabledVector&);
//
//    public:
//      // CREATORS
//      ThreadEnabledVector(bslma::Allocator *basicAllocator = 0)
//          // Create a thread-enabled vector.  Optionally specify a
//          // 'basicAllocator' used to supply memory.  If 'basicAllocator' is
//          // 0, the currently installed default allocator is used.
//      : d_elements(basicAllocator)
//      {
//      }
//
//      ~ThreadEnabledVector() {}
//          // Destroy this thread-enabled vector object.
//
//      // MANIPULATORS
//      int pushBack(const TYPE& value)
//          // Append the specified 'value' to this thread-enabled vector and
//          // return the index of the new element.
//      {
//          bslmt::LockGuard<bslmt::Mutex> guard(&d_mutex);
//          d_elements.push_back(value);
//          return static_cast<int>(d_elements.size()) - 1;
//      }
//
//      void set(int index, const TYPE& value)
//          // Set the element at the specified 'index' in this thread-enabled
//          // vector to the specified 'value'.  The behavior is undefined
//          // unless '0 <= index < length()'.
//      {
//          bslmt::LockGuard<bslmt::Mutex> guard(&d_mutex);
//          d_elements[index] = value;
//      }
//
//      // ACCESSORS
//      TYPE element(int index) const
//          // Return the value of the element at the specified 'index' in this
//          // thread-enabled vector.  Note that elements are returned *by*
//          // *value* because references to elements managed by this container
//          // may be invalidated by another thread.
//      {
//          bslmt::LockGuard<bslmt::Mutex> guard(&d_mutex);
//          return d_elements[index];
//      }
//
//      int length() const
//          // Return the number of elements in this thread-enabled vector.
//      {
//          bslmt::LockGuard<bslmt::Mutex> guard(&d_mutex);
//          return static_cast<int>(d_elements.size());
//      }
//  };
//..
// We use this thread-enabled vector to create a AddressBook class.  However,
// we use the 'bdlma::ConcurrentAllocatorAdapter' to prevent our two
// (thread-enabled) vectors from attempting synchronous memory allocations from
// our (potentially) non-thread safe 'bslma::Allocator'.  Note that we define a
// local class, 'AddressBook_PrivateData', in order to guarantee that
// 'd_allocatorAdapter' and 'd_mutex' are initialized before the thread-enabled
// vectors that depend on them:
//..
//  struct AddressBook_PrivateData {
//      // This 'struct' contains a mutex and an allocator adapter.  The
//      // 'AddressBook' class will inherit from this structure, ensuring that
//      // the mutex and adapter are initialized before other member variables
//      // that depend on them.
//
//    private:
//      // Not implemented:
//      AddressBook_PrivateData(const AddressBook_PrivateData&);
//
//    public:
//      bslmt::Mutex           d_mutex;             // synchronize allocator
//
//      bdlma::ConcurrentAllocatorAdapter
//                            d_allocatorAdapter;  // adapter for allocator
//
//      AddressBook_PrivateData(bslma::Allocator *basicAllocator = 0)
//          // Create a empty AddressBook private data object.  Optionally
//          // specify a 'basicAllocator' used to supply memory.  If
//          // 'basicAllocator' is 0, the currently installed default allocator
//          // is used.
//      : d_allocatorAdapter(&d_mutex, basicAllocator)
//      {
//      }
//  };
//
//  class AddressBook : private AddressBook_PrivateData {
//      // This 'class' defines a thread-enabled AddressBook containing vectors
//      // of names and addresses.  Note that this class uses private
//      // inheritance to ensure that the allocator adapter and mutex are
//      // initialized before the vectors of names and addresses.
//
//      // DATA
//      ThreadEnabledVector<bsl::string> d_names;      // list of names
//      ThreadEnabledVector<bsl::string> d_addresses;  // list of addresses
//
//    private:
//      // Not implemented:
//      AddressBook(const AddressBook&);
//
//    public:
//      // CREATORS
//      AddressBook(bslma::Allocator *basicAllocator = 0)
//          // Create an empty AddressBook for storing names and addresses.
//          // Optionally specify a 'basicAllocator' used to supply memory.  If
//          // 'basicAllocator' is 0, the currently installed default allocator
//          // is used.
//      : AddressBook_PrivateData(basicAllocator)
//      , d_names(&d_allocatorAdapter)
//      , d_addresses(&d_allocatorAdapter)
//      {
//      }
//
//      ~AddressBook()
//          // Destroy this AddressBook.
//      {
//      }
//
//      // MANIPULATORS
//      int addName(const bsl::string& name)
//          // Add the specified 'name' to this AddressBook and return the
//          // index of the newly-added name.
//      {
//          return d_names.pushBack(name);
//      }
//
//      int addAddress(const bsl::string& address)
//          // Add the specified 'address' to this AddressBook and return the
//          // index of the newly-added address.
//      {
//          return d_addresses.pushBack(address);
//      }
//
//      // ACCESSORS
//      bsl::string name(int index) const
//          // Return the value of the name at the specified 'index' in this
//          // AddressBook.
//      {
//          return d_names.element(index);
//      }
//
//      bsl::string address(int index) const
//          // Return the value of the address at the specified 'index' in this
//          // AddressBook.
//      {
//          return d_addresses.element(index);
//      }
//
//      int numNames() const
//          // Return the number of names in this AddressBook.
//      {
//          return d_names.length();
//      }
//
//      int numAddresses() const
//          // Return the number of addresses in this AddressBook.
//      {
//          return d_addresses.length();
//      }
//  };
//..

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

#ifndef INCLUDED_BSLMA_ALLOCATOR
#include <bslma_allocator.h>
#endif

#ifndef INCLUDED_BSLMA_DEFAULT
#include <bslma_default.h>
#endif

#ifndef INCLUDED_BSLMT_MUTEX
#include <bslmt_mutex.h>
#endif

namespace BloombergLP {
namespace bslmt { class Mutex; }
namespace bdlma {

                     // ================================
                     // class ConcurrentAllocatorAdapter
                     // ================================

class ConcurrentAllocatorAdapter : public bslma::Allocator {
    // This class defines an implementation of the 'bslma::Allocator' protocol
    // that "decorates" (wraps) a concrete 'bslma::Allocator' to ensure
    // thread-safe access to the decorated allocator.

    // DATA
    bslmt::Mutex      *d_mutex_p;      // synchronizer for operations on the
                                      // allocator (held, not owned)

    bslma::Allocator *d_allocator_p;  // allocator (held, not owned)

    // NOT IMPLEMENTED
    ConcurrentAllocatorAdapter(const ConcurrentAllocatorAdapter&);
    ConcurrentAllocatorAdapter& operator=(const ConcurrentAllocatorAdapter&);
  public:
    // CREATORS
    ConcurrentAllocatorAdapter(bslmt::Mutex     *mutex,
                               bslma::Allocator *basicAllocator);
        // Create a thread-enabled allocator adapter that uses the specified
        // 'mutex' to synchronize access to the specified 'basicAllocator'.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.

    virtual ~ConcurrentAllocatorAdapter();
        // Destroy this thread-enabled allocator adapter.

    // MANIPULATORS
    virtual void *allocate(size_type numBytes);
        // Return a newly-allocated block of memory of (at least) the specified
        // 'numBytes'.  If 'numBytes' is 0, a null pointer is returned with no
        // other effect.  If this allocator cannot return the requested number
        // of bytes, then it will throw a 'bsl::bad_alloc' exception in an
        // exception-enabled build, or else will abort the program in a
        // non-exception build.  Note that the alignment of the address
        // returned conforms to the platform requirement for any object of the
        // 'numBytes'.

    virtual void deallocate(void *address);
        // Return the memory at the specified 'address' back to this allocator.
        // If 'address' is 0, this function has no effect.  The behavior is
        // undefined unless 'address' was allocated using this allocator and
        // has not since been deallocated.
};

// ============================================================================
//                             INLINE DEFINITIONS
// ============================================================================

                     // --------------------------------
                     // class ConcurrentAllocatorAdapter
                     // --------------------------------

// CREATORS
inline
ConcurrentAllocatorAdapter::ConcurrentAllocatorAdapter(
                                              bslmt::Mutex     *mutex,
                                              bslma::Allocator *basicAllocator)
: d_mutex_p(mutex)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}
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
