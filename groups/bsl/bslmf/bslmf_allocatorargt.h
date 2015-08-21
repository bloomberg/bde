// bslmf_allocatorargt.h                                              -*-C++-*-
#ifndef INCLUDED_BSLMF_ALLOCATORARGT
#define INCLUDED_BSLMF_ALLOCATORARGT

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a tag type to precede allocator arguments.
//
//@CLASSES:
//  bsl::allocator_arg_t: tag indicating the next parameter is an allocator
//
//@SEE_ALSO: bslalg_scalarprimitives
//
//@DESCRIPTION: The C++11 standard defines the empty class
// 'bsl::allocator_arg_t' as a tag that precedes an argument of allocator type
// in circumstances where context alone cannot be used to determine which
// argument is an allocator.  Typically, this disambiguation is needed when a
// class has templated constructors taking variable numbers of arguments, each
// of which is of parameterized type.  If that class also uses an allocator (of
// either STL style or 'bslma'/'memory_resource' style), then the allocator
// argument must be flagged as special.  An argument of type
// 'std::allocator_arg_t' is used to distinguish constructors that take an
// allocator from constructors that don't.
//
// In addition to the 'allocator_arg_t' class type, this component (and the
// standard) define a constant, 'allocator_arg', of type 'allocator_arg_t'.
// That constant is used for passing an 'allocator_arg_t' argument to a
// function or constructor.
//
///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: Disambiguate a constructor invocation
/// - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we want to define a nullable type that can be in the full state
// (holding an object) or the null state (not holding an object).  When in the
// full state, memory is allocated for the held object using a memory
// allocator.  For simplicity, this memory allocator is not automatically
// propagated to the held object.
//
// First, we define a simple allocator class hierarchy with an abstract
// 'xyzma::Allocator' base class and two derived classes:
// 'xyzma::NewDeleteAllocator' and 'xyzma::TestAllocator':
//..
//  #include <cstddef>
//
//  namespace xyzma {
//
//  class Allocator {
//      // Abstract allocator base class
//  public:
//      virtual ~Allocator() { }
//
//      virtual void *allocate(std::size_t nbytes) = 0;
//      virtual void deallocate(void *ptr) = 0;
//  };
//
//  class NewDeleteAllocator : public Allocator {
//      // Concrete allocator that uses operators 'new' and 'delete'
//
//  public:
//      static NewDeleteAllocator* singleton();
//          // Returns a singleton instance of this class
//
//      virtual void *allocate(std::size_t nbytes);
//      virtual void deallocate(void *ptr);
//  };
//
//  NewDeleteAllocator *NewDeleteAllocator::singleton() {
//      static NewDeleteAllocator s;
//      return &s;
//  }
//
//  void *NewDeleteAllocator::allocate(std::size_t nbytes) {
//      return ::operator new(nbytes);
//  }
//
//  void NewDeleteAllocator::deallocate(void *ptr) {
//      ::operator delete(ptr);
//  }
//
//  class TestAllocator : public Allocator {
//      // Concrete allocator that keeps track of number of blocks allocated
//      // and deallocated.
//
//      std::size_t d_allocatedBlocks;
//      std::size_t d_deallocatedBlocks;
//
//  public:
//      TestAllocator() : d_allocatedBlocks(0), d_deallocatedBlocks(0) { }
//
//      virtual void *allocate(std::size_t nbytes);
//      virtual void deallocate(void *ptr);
//
//      // ACCESSORS
//      std::size_t allocatedBlocks() const { return d_allocatedBlocks; }
//      std::size_t deallocatedBlocks() const { return d_deallocatedBlocks; }
//      std::size_t outstandingBlocks() const {
//          return d_allocatedBlocks - d_deallocatedBlocks;
//      }
//  };
//
//  void *TestAllocator::allocate(std::size_t nbytes) {
//      ++d_allocatedBlocks;
//      return ::operator new(nbytes);
//  }
//
//  void TestAllocator::deallocate(void *ptr) {
//      ++d_deallocatedBlocks;
//      ::operator delete(ptr);
//  }
//
//  }  // close namespace xyzma
//..
// Next, we define our nullable class template, declaring two constructors: one
// that constructs the null object, and one that constructs a non-null object
// using the specified constructor argument.  For flexibility, the second
// constructor is a template that takes any type and can therefore construct
// the object without necessarily invoking the copy constructor.  (Ideally,
// this second constructor would be variadic, but that is not necessary for
// this example.):
//..
//  #include <new>
//
//  namespace xyzutl {
//
//  template <class TYPE>
//  class Nullable {
//      xyzma::Allocator *d_alloc_p;
//      TYPE             *d_object_p;
//
//  public:
//      // CREATORS
//      Nullable();
//          // Construct a null object.  Note: this is ctor A.
//
//      template <class ARG>
//      Nullable(const ARG& arg);
//          // Construct a non-null object using the specified 'arg' as the
//          // constructor argument for the 'TYPE' object.  Note: this is ctor
//          // B.
//..
// Next, we want to add constructors that supply an allocator for use by the
// 'Nullable' object.  Our first thought is to add two more constructors like
// the two above, but with an additional allocator argument at the end:
//..
//      // Nullable(xyzma::Allocator *alloc);
//          // ctor C
//
//      // template <class ARG>
//      // Nullable(const ARG& arg, xyzma::Allocator *alloc);
//          // ctor D
//..
// However, ctor C is difficult to invoke, because ctor B is almost always a
// better match.  Nor can we use SFINAE to disqualify ctor B in cases where
// ARG is 'xyzma::Allocator*' because 'xyzma::Allocator*' is a perfectly valid
// constructor argument for many 'TYPE's.
//
// We solve this problem by using 'allocator_arg_t' to explicitly tag the
// constructor that takes an allocator argument:
//..
//      Nullable(bsl::allocator_arg_t, xyzma::Allocator *alloc);
//          // Construct a null object with the specified 'alloc' allocator.
//          // Note: this is ctor E
//..
// The 'allocator_arg_t' argument disambiguates the constructor.
//
// Next, to make things consistent (which is important for generic
// programming), we use the 'allocator_arg_t' tag in the other allocator-aware
// constructor, as well:
//..
//      template <class ARG>
//      Nullable(bsl::allocator_arg_t,
//               xyzma::Allocator *alloc,
//               const ARG&        arg);
//          // Construct a non-null object using the specified 'arg' as the
//          // constructor argument for the 'TYPE' object, and the specified
//          // 'alloc' allocator.  Note: this is ctor F.
//..
// Next, we finish the class interface and implementation:
//..
//      ~Nullable();
//
//      // MANIPULATORS
//      Nullable& operator=(const Nullable& rhs);
//          // Copy assign this object from the specified 'rhs'.
//
//      Nullable& operator=(const TYPE& rhs);
//          // Construct a non-null object holding a copy of the specified
//          // 'rhs' object.
//
//      // ACCESSORS
//      const TYPE& value() const { return *d_object_p; }
//          // Return the object stored in this Nullable. The behavior is
//          // undefined if this is null.
//
//      bool isNull() const { return ! d_object_p; }
//          // Returns true if this object is not null.
//
//      xyzma::Allocator *allocator() const { return d_alloc_p; }
//  };
//
//  template <class TYPE>
//  Nullable<TYPE>::Nullable()
//      : d_alloc_p(xyzma::NewDeleteAllocator::singleton())
//      , d_object_p(0)
//  {
//  }
//
//  template <class TYPE>
//  template <class ARG>
//  Nullable<TYPE>::Nullable(const ARG& arg)
//      : d_alloc_p(xyzma::NewDeleteAllocator::singleton())
//      , d_object_p(static_cast<TYPE*>(d_alloc_p->allocate(sizeof(TYPE))))
//  {
//      ::new(d_object_p) TYPE(arg);
//  }
//
//  template <class TYPE>
//  Nullable<TYPE>::Nullable(bsl::allocator_arg_t, xyzma::Allocator *alloc)
//      : d_alloc_p(alloc)
//      , d_object_p(0)
//  {
//  }
//
//  template <class TYPE>
//  template <class ARG>
//  Nullable<TYPE>::Nullable(bsl::allocator_arg_t,
//                           xyzma::Allocator *alloc,
//                           const ARG&        arg)
//      : d_alloc_p(alloc)
//      , d_object_p(static_cast<TYPE*>(d_alloc_p->allocate(sizeof(TYPE))))
//  {
//      ::new(d_object_p) TYPE(arg);
//  }
//
//  template <class TYPE>
//  Nullable<TYPE>::~Nullable() {
//      if (d_object_p) {
//          d_object_p->~TYPE();
//          d_alloc_p->deallocate(d_object_p);
//      }
//  }
//
//  template <class TYPE>
//  Nullable<TYPE>& Nullable<TYPE>::operator=(const Nullable& rhs) {
//      if (&rhs == this) return *this;                               // RETURN
//      if (!isNull() && !rhs.isNull()) {
//          *d_object_p = *rhs.d_object_p;
//      }
//      else if (!isNull() /* && rhs.isNull() */) {
//          // Make null
//          d_object_p->~TYPE();
//          d_alloc_p->deallocate(d_object_p);
//          d_object_p = 0;
//      }
//      else if (/* isNull() && */ !rhs.isNull()) {
//          // Allocate and copy from 'rhs'
//          d_object_p = static_cast<TYPE*>(d_alloc_p->allocate(sizeof(TYPE)));
//          ::new(d_object_p) TYPE(*rhs.d_object_p);
//      }
//      // else both are null
//
//      return *this;
//  }
//
//  template <class TYPE>
//  Nullable<TYPE>& Nullable<TYPE>::operator=(const TYPE& rhs) {
//      if (isNull()) {
//          d_object_p = static_cast<TYPE*>(d_alloc_p->allocate(sizeof(TYPE)));
//          ::new(d_object_p) TYPE(*rhs.d_object_p);
//      }
//      else {
//          *d_object_p = rhs;
//      }
//
//      return *this;
//  }
//
//  }  // close namespace xyzutl
//..
// Now, for testing purposes, we define a class that takes an allocator
// constructor argument:
//..
//  class Obj {
//      xyzma::Allocator *d_alloc_p;
//      int               d_count;
//  public:
//      explicit Obj(xyzma::Allocator *alloc = 0)
//          : d_alloc_p(alloc), d_count(0)
//      {
//      }
//
//      Obj(int count, xyzma::Allocator *alloc = 0)                 // IMPLICIT
//          : d_alloc_p(alloc), d_count(count)
//      {
//      }
//
//      int count() const { return d_count; }
//      xyzma::Allocator *allocator() const { return d_alloc_p; }
//  };
//
//  bool operator==(const Obj& a, const Obj& b) {
//      return a.count() == b.count();
//  }
//
//  bool operator!=(const Obj& a, const Obj& b) {
//      return a.count() != b.count();
//  }
//..
// Finally, we test that our nullable type can be constructed with and without
// an allocator pointer and that the allocator pointer can unambiguously be
// used for the object's allocator.
//..
//  int main() {
//
//      using xyzutl::Nullable;
//
//      xyzma::TestAllocator ta;
//
//      Nullable<Obj> no1;
//      assert(  no1.isNull());
//      assert(xyzma::NewDeleteAllocator::singleton() == no1.allocator());
//
//      Nullable<Obj> no2(2);
//      assert(! no2.isNull());
//      assert(xyzma::NewDeleteAllocator::singleton() == no2.allocator());
//      assert(2 == no2.value());
//      assert(0 == no2.value().allocator());
//
//      Nullable<Obj> no3(bsl::allocator_arg, &ta);
//      assert(  no3.isNull());
//      assert(&ta == no3.allocator());
//      assert(0 == ta.outstandingBlocks());
//
//      Nullable<Obj> no4(bsl::allocator_arg, &ta, 4);
//      assert(! no4.isNull());
//      assert(&ta == no4.allocator());
//      assert(1 == ta.outstandingBlocks());
//      assert(4 == no4.value());
//      assert(0 == no4.value().allocator());
//
//      // '&ta' used by 'Obj', not by 'Nullable'.
//      Nullable<Obj> no5(&ta);
//      assert(! no5.isNull());
//      assert(xyzma::NewDeleteAllocator::singleton() == no5.allocator());
//      assert(1 == ta.outstandingBlocks());  // No change
//      assert(0 == no5.value());
//      assert(&ta == no5.value().allocator());
//
//      // '&ta' used by both 'Nullable' and by 'Obj'
//      Nullable<Obj> no6(bsl::allocator_arg, &ta, &ta);
//      assert(! no6.isNull());
//      assert(&ta == no6.allocator());
//      assert(2 == ta.outstandingBlocks());
//      assert(0 == no6.value());
//      assert(&ta == no6.value().allocator());
//
//      return 0;
//  }
//..

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

namespace bsl {

                        // ======================
                        // struct allocator_arg_t
                        // ======================

struct allocator_arg_t {
    // Tag type indicating that next argument is an allocator.
    // TBD: If native library declares 'std::allocator_arg_t', then this
    // struct should be an replaced by an alias for 'std::allocator_arg_t'.

};

static const allocator_arg_t allocator_arg = { };
    // Value of type 'allocator_arg_t' used as actual argument to function
    // that takes an allocator argument.  Note that 'constexpr' would be better
    // than 'const' here, but any compiler that supports 'constexpr' would also
    // provide this class and named value, that should be imported into
    // namespace 'bsl' with using declarations instead.

}  // close namespace bsl

#endif // ! defined(INCLUDED_BSLMF_ALLOCATORARGT)

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
