// bslma_memoryresource.h                                             -*-C++-*-
#ifndef INCLUDED_BSLMA_MEMORYRESOURCE
#define INCLUDED_BSLMA_MEMORYRESOURCE

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a pure abstract interface for memory-allocation mechanisms.
//
//@CLASSES:
//  bsl::memory_resource: protocol class for memory allocation and deallocation
//
//@CANONICAL_HEADER: bsl_memory_resource.h
//
//@SEE_ALSO: bslma_allocator, bslma_polymorphicallocator
//
//@DESCRIPTION: This component is for internal use only.  Please include
// '<bsl_memory_resource.h>' instead and use 'bsl::memory_resource' directly.
//
// This component provides a protocol (pure abstract interface) class,
// 'bsl::memory_resource', comprising member functions for allocating and
// deallocating memory.  The 'bsl::memory_resource' interface is identical to
// that of 'std::pmr::memory_resource' from the C++17 Standard Library; in
// fact, the former type is an alias for the latter type when using a C++17 or
// later library supplied by the platform.
//
// A concrete class derived from 'bsl::memory_resource' might use pooling or
// other mechanisms that improve on 'new' and 'delete' in some way, such as
// speeding up the program or providing instrumentation for debugging or
// security.  A 'memory_resource' thus provides a customizable alterantive to
// using raw calls to 'new' and 'delete'.
//
// An object, 'Obj', holding a base-class pointer, 'd_resource_p', of type
// 'bsl::memory_resource *' would allocate and deallocate memory by calling the
// resource's 'allocate' and 'deallocate' member functions (through
// 'd_resource_p'), which subsequently invoke the respective virtual functions,
// 'do_allocate' and 'do_deallocate'.  A client can thus customize the memory
// allocation mechanism used by 'Obj' by providing it with an appropriate
// concrete resource whose class overrides 'do_allocate' and 'do_deallocate'.
//
///Thread Safety
///-------------
// Unless otherwise documented, a single memory resource object is not safe for
// concurrent access by multiple threads.  Classes derived from
// 'bsl::memory_resource' that are specifically designed for concurrent access
// must be documented as such.  Unless specifically documented otherwise,
// separate objects of classes derived from 'bsl::memory_resource' may safely
// be used in separate threads.
//
// Note that some memory resources delegate to other memory resource objects.
// When used in a concurrent context, the thread safety of the entire chain
// must be considered.
//
///Usage
///-----
// The 'bsl::memory_resource' protocol provided in this component defines a
// bilateral contract between suppliers and consumers of raw memory.  The
// following subsections illustrate (1) implementation of a concrete resource
// derived from the abstract 'bsl::memory_resource' base class and (2) use of a
// 'bsl::memory_resource'.
//
///Example 1: a counting memory resource
///- - - - - - - - - - - - - - - - - - -
// In this example, we derive a concrete 'CountingResource' class from
// 'bsl::memory_resource', overriding and providing concrete implementations
// for all of the virtual functions declared in the base class.  This resource
// keeps track of the number of blocks of memory that were allocated from the
// resource but not yet returned to the resource.
//
// First, we define the 'CountingResource' class with a single private data
// member to keep track of the number of blocks outstanding.  We don't want
// this type to be copyable, so we disable copying here, too.
//..
//  #include <bslmf_movableref.h>
//  #include <bsls_assert.h>
//  #include <bsls_keyword.h>
//  #include <bsls_exceptionutil.h>
//  #include <stdint.h>  // 'uintptr_t'
//
//  class CountingResource : public bsl::memory_resource {
//
//      // DATA
//      int d_blocksOutstanding;
//
//      CountingResource(const CountingResource&) BSLS_KEYWORD_DELETED;
//      CountingResource& operator=(const CountingResource&)
//                                                        BSLS_KEYWORD_DELETED;
//..
// Next, we declare the protected virtual functions that override the
// base-class virtual functions:
//..
//    protected:
//      // PROTECTED MANIPULATORS
//      void* do_allocate(std::size_t bytes,
//                        std::size_t alignment) BSLS_KEYWORD_OVERRIDE;
//      void do_deallocate(void* p, std::size_t bytes,
//                         std::size_t alignment) BSLS_KEYWORD_OVERRIDE;
//
//      // PROTECTED ACCESSORS
//      bool do_is_equal(const bsl::memory_resource& other) const
//                                 BSLS_KEYWORD_NOEXCEPT BSLS_KEYWORD_OVERRIDE;
//..
// Now we can declare the public interface, comprising the default constructor,
// the destructor, and an accessor to return the current block count; all other
// public members are inherited from the base class:
//..
//    public:
//      // CREATORS
//      CountingResource() : d_blocksOutstanding(0) { }
//      ~CountingResource() BSLS_KEYWORD_OVERRIDE;
//
//      // ACCESSORS
//      int blocksOutstanding() const { return d_blocksOutstanding; }
//  };
//..
// Next, we implement the 'do_allocate' method to allocate memory using
// 'operator new', then increment the block counter.  We cannot, in C++11,
// force 'operator new' to return memory that is more than maximally aligned,
// so we throw an exception if the specified 'alignment' is not met; other
// resources can use the 'alignment' argument more productively.
//..
//  void *CountingResource::do_allocate(std::size_t bytes,
//                                      std::size_t alignment)
//  {
//      void *ret = ::operator new(bytes);
//      if (uintptr_t(ret) & (alignment - 1)) {
//          ::operator delete(ret);
//          BSLS_THROW(this);  // Alignment failed
//      }
//      ++d_blocksOutstanding;
//      return ret;
//  }
//..
// Next, we implement 'do_deallocate', which returns the memory referenced by
// 'p' to the heap and decrements the block counter.  The 'bytes' and
// 'alignment' arguments are ignored:
//..
//  void CountingResource::do_deallocate(void* p, std::size_t, std::size_t)
//  {
//      ::operator delete(p);
//      --d_blocksOutstanding;
//  }
//..
// Next, we implement 'do_is_equal', which determines if the specified 'other'
// resource is equal to this one.  For this and most other resource types,
// 'do_is_equal' returns 'true' if and only if the two resources are the same
// object:
//..
//  bool CountingResource::do_is_equal(const bsl::memory_resource& other) const
//                                                        BSLS_KEYWORD_NOEXCEPT
//  {
//      return this == &other;
//  }
//..
// Next, we implement the destructor, which simply asserts that the block count
// is zero upon destruction:
//..
//  CountingResource::~CountingResource()
//  {
//      BSLS_ASSERT(0 == d_blocksOutstanding);
//  }
//..
// Finally, we construct an object of 'CountingResource' and verify that
// allocation, deallocation, and equality testing work as expected.
//..
//  int main()
//  {
//      CountingResource obj;
//      assert(0 == obj.blocksOutstanding());
//
//      void *p = obj.allocate(16, 4);
//      assert(p);
//      assert(0 == (uintptr_t(p) & 3));
//      assert(1 == obj.blocksOutstanding());
//
//      obj.deallocate(p, 16, 4);
//      assert(0 == obj.blocksOutstanding());
//
//      CountingResource obj2;
//      assert(obj == obj);
//      assert(obj != obj2);
//  }
//..
//
///Example 2: A class that allocates memory
///- - - - - - - - - - - - - - - - - - - -
// In this example, we define a class template, 'Holder<TYPE>', that holds a
// single instance of 'TYPE' on the heap.  'Holder' is designed such that its
// memory use can be customized by supplying an appropriate memory resource.  A
// holder object can be empty and it can be move-constructed even if 'TYPE' is
// not movable.  In addition, the footprint of a 'Holder' object is the same
// (typically the size of 2 pointers), regardless of the size of 'TYPE'.
//
// First, we define a simple class template modeled after the C++17 standard
// library 'std::pmr::polymorphic_allocator' template, which is a thin wrapper
// around a 'memory_resource' pointer.  By wrapping the pointer in a class, we
// avoid some the problems of raw pointers such as accidental use of a null
// pointer:
//..
//  #include <bsls_alignmentfromtype.h>
//
//  template <class TYPE>
//  class PolymorphicAllocator {
//
//      // DATA
//      bsl::memory_resource *d_resource_p;
//
//    public:
//      // CREATORS
//      PolymorphicAllocator(bsl::memory_resource *r);              // IMPLICIT
//
//      // MANIPULATORS
//      TYPE *allocate(std::size_t n);
//      void deallocate(TYPE *p, size_t n);
//
//      // ACCESSORS
//      bsl::memory_resource *resource() const { return d_resource_p; }
//  };
//..
// Next, we implement the constructor for 'PolymorphicAllocator', which stores
// the pointer argument and defensively checks that it is not null:
//..
//  template <class TYPE>
//  PolymorphicAllocator<TYPE>::PolymorphicAllocator(bsl::memory_resource *r)
//      : d_resource_p(r)
//  {
//      BSLS_ASSERT(0 != r);
//  }
//..
// Next, we implement the allocation and deallocation functions by forwarding
// to the corresponding function of the memory resource.  Note that the size
// and alignment of 'TYPE' are used to compute the appropriate number of bytes
// and alignment to request from the memory resource:
//..
//  template <class TYPE>
//  TYPE *PolymorphicAllocator<TYPE>::allocate(std::size_t n)
//  {
//      void *p = d_resource_p->allocate(n * sizeof(TYPE),
//                                       bsls::AlignmentFromType<TYPE>::VALUE);
//      return static_cast<TYPE *>(p);
//  }
//
//  template <class TYPE>
//  void PolymorphicAllocator<TYPE>::deallocate(TYPE *p, std::size_t n)
//  {
//      d_resource_p->deallocate(p, n * sizeof(TYPE),
//                               bsls::AlignmentFromType<TYPE>::VALUE);
//  }
//..
// Now we define our actual 'Holder' template with with data members to hold
// the memory allocator and a pointer to the contained object:
//..
//  template <class TYPE>
//  class Holder {
//      PolymorphicAllocator<TYPE>  d_allocator;
//      TYPE                       *d_data_p;
//..
// Next, we declare the constructors.  Following the pattern for
// allocator-aware types used in BDE, the public interface contains an
// 'allocator_type' typedef that can be passed to each constructor.
// Typically, the allocator constructor argument would be optional, but,
// because our 'PolymorphicAllocator' has no default constructor (unlike the
// 'std::pmr::polymorphic_allocator'), the allocator is *required* for all
// constructors except the move constructor:
//..
//    public:
//      // TYPES
//      typedef PolymorphicAllocator<TYPE> allocator_type;
//
//      // CREATORS
//      explicit Holder(const allocator_type& allocator);
//      Holder(const TYPE& value, const allocator_type& allocator);
//      Holder(const Holder& other, const allocator_type& allocator);
//      Holder(bslmf::MovableRef<Holder> other);                    // IMPLICIT
//      Holder(bslmf::MovableRef<Holder> other,
//             const allocator_type&     allocator);
//      ~Holder();
//..
// Next, we declare the manipulators and accessors, allowing a 'Holder' to be
// assigned and giving a client access to its value and allocator:
//..
//      // MANIPULATORS
//      Holder& operator=(const Holder& rhs);
//      Holder& operator=(bslmf::MovableRef<Holder> rhs);
//      TYPE& value() { return *d_data_p; }
//
//      // ACCESSORS
//      bool isEmpty() const { return 0 == d_data_p; }
//      const TYPE& value() const { return *d_data_p; }
//      allocator_type get_allocator() const { return d_allocator; }
//  };
//..
// Next, we'll implement the first constructor, which creates an empty object;
// its only job is to store the allocator:
//..
//  template <class TYPE>
//  Holder<TYPE>::Holder(const allocator_type& allocator)
//      : d_allocator(allocator)
//      , d_data_p(0)
//  {
//  }
//..
// Next, we'll implement the second constructor, which allocates memory and
// constructs an object in it.  The 'try'/'catch' block is needed to free the
// memory in case the constructor for 'TYPE' throws and exception.  An
// alternative implementation would use an RAII object to automatically free
// the memory in the case of an exception (see 'bslma_deallocatorproctor'):
//..
//  template <class TYPE>
//  Holder<TYPE>::Holder(const TYPE& value, const allocator_type& allocator)
//      : d_allocator(allocator)
//      , d_data_p(0)
//  {
//      d_data_p = d_allocator.allocate(1);
//      BSLS_TRY {
//          ::new(d_data_p) TYPE(value);
//      }
//      BSLS_CATCH(...) {
//          d_allocator.deallocate(d_data_p, 1);
//          BSLS_RETHROW;
//      }
//  }
//..
// Next, we'll implement a destructor that deletes the value object and
// deallocates the allocated memory:
//..
//  template <class TYPE>
//  Holder<TYPE>::~Holder()
//  {
//      if (! isEmpty()) {
//          d_data_p->~TYPE();                    // Destroy object.
//          d_allocator.deallocate(d_data_p, 1);  // Deallocate memory.
//      }
//  }
//..
// Finally, we've implemented enough of 'Holder' to demonstrate its use.
// Below, we pass the 'CountingResource' from Example 1 to the constructors
// several 'Holder' objects.  Each non-empty 'Holder' allocates one block of
// memory, which is reflected in the outstanding block count.  Note that the
// address of the resource can be passed directly to the constructors because
// 'PolymorphicAllocator' is implicitly convertible from 'bsl::memory_resource
// *':
//..
//  int main()
//  {
//      CountingResource rsrc;
//
//      {
//          Holder<int> h1(&rsrc);   // Empty resource
//          assert(h1.isEmpty());
//          assert(0 == rsrc.blocksOutstanding());
//
//          Holder<int> h2(2, &rsrc);
//          assert(! h2.isEmpty());
//          assert(1 == rsrc.blocksOutstanding());
//
//          Holder<double> h3(3.0, &rsrc);
//          assert(! h3.isEmpty());
//          assert(2 == rsrc.blocksOutstanding());
//      }
//
//      assert(0 == rsrc.blocksOutstanding());  // Destructors freed memory
//  }
//..


#include <bslscm_version.h>

#include <bsls_alignmentutil.h>
#include <bsls_annotation.h>
#include <bsls_keyword.h>
#include <bsls_libraryfeatures.h>

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
// Use `memory_resource` from native C++17 library, if available.

# include <memory_resource>

namespace bsl {

using std::pmr::memory_resource;

}  // close namespace bsl

#else // If C++17 library is not available

namespace bsl {

                        // =====================
                        // Class memory_resource
                        // =====================

class memory_resource {
    // A protocol (pure abstract interface) class, comprising member functions
    // for allocating and deallocating memory.  This class is a pre-C++17
    // implementation of 'std::pmr::memory_resource' from the C++17 Standard
    // Library.

    // PRIVATE CONSTANTS
    enum {
        k_MAX_ALIGN = BloombergLP::bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT
    };

  public:
    // CREATORS

    //! memory_resource() BSLS_KEYWORD_DEFAULT;
    //! memory_resource(const memory_resource&) BSLS_KEYWORD_DEFAULT;
        // Create this object.  Has no effect other than to begin its lifetime.

    virtual ~memory_resource();
        // Destroy this object.  Has no effect other than to end its lifetime.

    // MANIPULATORS

    //! memory_resource& operator=(const memory_resource&)
    //!                                                   BSLS_KEYWORD_DEFAULT;
        // Return a modifiable reference to this object.

    BSLS_ANNOTATION_NODISCARD
    void *allocate(size_t bytes, size_t alignment = k_MAX_ALIGN);
        // Return the (non-null) address of a block of memory suitable for
        // holding an object having at least the specified 'bytes' and
        // 'alignment'.  If this memory resource is unable to fulfill the
        // request, i.e., because 'bytes' or 'alignment' is too large, then
        // throw 'bad_alloc' or other suitable exception.  The behavior is
        // undefined unless 'alignment' is a power of two.  Note that this
        // function calls the derived-class implementation of 'do_allocate'.

    void deallocate(void *p, size_t bytes, size_t alignment = k_MAX_ALIGN);
        // Deallocate the block of memory at the specified address 'p' and
        // having the specified 'bytes' and 'alignment' by returning it to the
        // derived-class memory resource.  The behavior is undefined unless 'p'
        // was allocated from this resource using the same size and alignment
        // and has not yet been deallocated.  Note that this function calls the
        // derived-class implementation of 'do_deallocate'.

    // ACCESSORS
    bool is_equal(const memory_resource& other) const BSLS_KEYWORD_NOEXCEPT;
        // Return 'true' if memory allocated from this resource can be
        // deallocated from the specified 'other' resource and vice-versa;
        // otherwise return 'false'.  Note that this function calls the
        // derived-class implementation of 'do_is_equal'.

  private:
    // PRIVATE MANIPULATORS
    virtual void* do_allocate(size_t bytes, size_t alignment) = 0;
        // Return a block of memory, allocated from the derived-class resource,
        // suitable for holding an object having at least the specified 'bytes'
        // and 'alignment'.  The behavior is undefined unless 'alignment' is a
        // power of two.

    virtual void do_deallocate(void* p, size_t bytes, size_t alignment) = 0;
        // Deallocate the block of memory at the specified address 'p' and
        // having the specified 'bytes' and 'alignment' by returning it to the
        // derived-class memory resource.  The behavior is undefined unless 'p'
        // was allocated from this resource using the same size and alignment
        // and has not yet been deallocated.

    // PRIVATE ACCESSORS
    virtual bool do_is_equal(const memory_resource& other) const
                                                     BSLS_KEYWORD_NOEXCEPT = 0;
        // Return 'true' if memory allocated from this resource can be
        // deallocated from the specified 'other' resource and vice-versa;
        // otherwise return 'false'.
};

// FREE OPERATORS
bool operator==(const memory_resource& a, const memory_resource& b);
    // Return 'true' if memory allocated from the specified 'a' resource can be
    // deallocated from the specified 'b' resource; otherwise return 'false'.

bool operator!=(const memory_resource& a, const memory_resource& b);
    // Return 'true' if memory allocated from the specified 'a' resource cannot
    // be deallocated from the specified 'b' resource; otherwise return
    // 'false'.

// ============================================================================
//                       INLINE FUNCTION IMPLEMENTATIONS
// ============================================================================

// CREATORS
inline
memory_resource::~memory_resource()
{
    // Implementation note: because 'memory_resource' is a pure abstract class
    // with a trivial constructor, the virtual destructor can be inlined
    // without forcing the implementation to generate a vtbl for the class.
}

// MANIPULATORS
inline
void *memory_resource::allocate(size_t bytes, size_t alignment)
{
    return do_allocate(bytes, alignment);
}

inline
void memory_resource::deallocate(void *p, size_t bytes, size_t alignment)
{
    do_deallocate(p, bytes, alignment);
}

// ACCESSORS
inline
bool memory_resource::is_equal(const memory_resource& other) const
                                                          BSLS_KEYWORD_NOEXCEPT
{
    return do_is_equal(other);
}

}  // close namespace bsl


// FREE OPERATORS

inline
bool bsl::operator==(const bsl::memory_resource& a,
                     const bsl::memory_resource& b)
{
    return a.is_equal(b);
}

inline
bool bsl::operator!=(const bsl::memory_resource& a,
                     const bsl::memory_resource& b)
{
    return ! a.is_equal(b);
}

#endif // ! defined(BSLS_LIBRARYFEATURES_HAS_CPP17_PMR)

#endif // ! defined(INCLUDED_BSLMA_MEMORYRESOURCE)

// ----------------------------------------------------------------------------
// Copyright 2022 Bloomberg Finance L.P.
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
