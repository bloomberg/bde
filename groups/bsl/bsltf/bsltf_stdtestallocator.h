// bsltf_stdtestallocator.h                                           -*-C++-*-
#ifndef INCLUDED_BSLTF_STDTESTALLOCATOR
#define INCLUDED_BSLTF_STDTESTALLOCATOR

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a minimal standard compliant allocator.
//
//@CLASSES:
//      bsltf::StdTestAllocatorConfiguration: namespace to configure allocator
// bsltf::StdTestAllocatorConfigurationGuard: configuration scoped guard
//                   bsltf::StdTestAllocator: standard compliant allocator
//
//@DESCRIPTION: This component provides an allocator, 'StdTestAllocator', that
// defines the minimal interface to comply with section 20.1.5
// ([lib.allocator.requirements]) of the C++03 standard.  This type can be used
// to verify that constructs designed to support a standard-compliant allocator
// access the allocator only through the standard-defined interface.
//
// 'StdTestAllocator' delegates its operations to a static 'bslma::Allocator'
// (delegate allocator) that can be configured by the utilities provided in the
// namespace 'StdTestAllocatorConfiguration'.
// 'StdTestAllocatorConfigurationGuard' provides a scoped guard to enable
// temporary replacement of the delegate allocator.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Testing The Support for STL-Compliant Allocator
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// In this example we will verify that a type supports the use of a
// STL-compliant allocator.
//
// First we define a simple container type intended to be used with a C++03
// standard compliant allocator:
//..
//  template <class TYPE, class ALLOCATOR>
//  class MyContainer {
//      // This container type is parameterized on a standard allocator type
//      // and contains a single object, always initialized, which can be
//      // replaced and accessed.
//
//      // DATA MEMBERS
//      ALLOCATOR  d_allocator;  // allocator used to supply memory (held, not
//                               // owned)
//
//      TYPE      *d_object_p;   // pointer to the contained object
//
//    public:
//      // CONSTRUCTORS
//      MyContainer(const TYPE& object);
//          // Create an container containing the specified 'object', using the
//          // parameterized 'ALLOCATOR' to allocate memory.
//
//      ~MyContainer();
//          // Destroy this container.
//
//      // MANIPULATORS
//      TYPE& object();
//          // Return a reference providing modifiable access to the object
//          // contained in this container.
//
//      // ACCESSORS
//      const TYPE& object() const;
//          // Return a reference providing non-modifiable access to the object
//          // contained in this container.
//  };
//..
// Then, we define the member functions of 'MyContainer':
//..
//  // CREATORS
//  template <class TYPE, class ALLOCATOR>
//  MyContainer<TYPE, ALLOCATOR>::MyContainer(const TYPE& object)
//  {
//      d_object_p = d_allocator.allocate(1);
//      d_allocator.construct(d_object_p, object);
//  }
//
//  template <class TYPE, class ALLOCATOR>
//  MyContainer<TYPE, ALLOCATOR>::~MyContainer()
//  {
//      d_allocator.destroy(d_object_p);
//      d_allocator.deallocate(d_object_p);
//  }
//
//  // MANIPULATORS
//  template <class TYPE, class ALLOCATOR>
//  TYPE& MyContainer<TYPE, ALLOCATOR>::object()
//  {
//      return *d_object_p;
//  }
//
//  // ACCESSORS
//  template <class TYPE, class ALLOCATOR>
//  const TYPE& MyContainer<TYPE, ALLOCATOR>::object() const
//  {
//      return *d_object_p;
//  }
//..
// Now, we use 'StdTestAllocator' to implement a simple test for 'MyContainer'
// to verify it correctly uses a parameterized allocator using only the C++03
// standard methods:
//..
//  bslma_TestAllocator oa("object", veryVeryVeryVerbose);
//  StdTestAllocatorConfigurationGuard stag(&oa);
//  {
//      typedef MyContainer<int, StdTestAllocator<int> > Obj;
//
//      Obj mX(2); const Obj& X = mX;
//      assert(sizeof(int) == oa.numBytesInUse());
//
//      assert(X.object() == 2);
//
//      mX.object() = -10;
//      assert(X.object() == -10);
//  }
//
//  assert(0 == oa.numBytesInUse());
//..

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLMA_ALLOCATOR
#include <bslma_allocator.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSLS_UTIL
#include <bsls_util.h>
#endif

#ifndef INCLUDED_NEW
#include <new>
#define INCLUDED_NEW
#endif

#ifndef INCLUDED_STDDEF_H
#include <stddef.h>
#define INCLUDED_STDDEF_H
#endif

namespace BloombergLP {
namespace bsltf {

                        // ===================================
                        // class StdTestAllocatorConfiguration
                        // ===================================

struct StdTestAllocatorConfiguration {
    // This 'struct' provides a namespace for functions that manipulate and
    // access the *delegate allocator* for 'StdTestAllocator'.  The delegate
    // allocator is the allocator to which 'StdTestAllocator' objects delegate
    // their operations.  The provided operations are *not* thread-safe.  Note
    // that this allocator is configured globally as C++03 standard compliant
    // allocators cannot have individually identifiable state.

  public:
    // CLASS METHODS
    static void setDelegateAllocatorRaw(bslma::Allocator *basicAllocator);
        // Set the address of the delegate allocator to the specified
        // 'basicAllocator'.

    static bslma::Allocator* delegateAllocator();
        // Return the address of the delegate allocator.  Note that, this
        // method will initially return
        // '&bslma_NewDeleteAllocator::singleton()' if the
        // 'setDelegatingAllocator' class method has not been called.
};

                        // ========================================
                        // class StdTestAllocatorConfigurationGuard
                        // ========================================

class StdTestAllocatorConfigurationGuard {
    // Upon construction, an object of this class saves the current *delegate
    // allocator* for 'StdTestAllocator' and and installs the user-specified
    // allocator as the delegate allocator.  The delegate allocator is the
    // globally configured allocator to which an 'StdTestAllocator' objects
    // delegate their operations.  On destruction, the original delegate
    // allocator is restored.

    bslma::Allocator *d_original_p;  // original (restore at destruction)

  private:
    // NOT IMPLEMENTED
    StdTestAllocatorConfigurationGuard(
                                    const StdTestAllocatorConfigurationGuard&);
    StdTestAllocatorConfigurationGuard& operator=(
                                    const StdTestAllocatorConfigurationGuard&);

  public:
    // CREATORS
    explicit
    StdTestAllocatorConfigurationGuard(bslma::Allocator *temporaryAllocator);
        // Create a scoped guard that installs the specified
        // 'temporaryAllocator' as the delegate allocator.

    ~StdTestAllocatorConfigurationGuard();
        // Restore the delegate allocator that was in place when this scoped
        // guard was created and destroy this guard.
};


                        // ======================
                        // class StdTestAllocator
                        // ======================

template <class TYPE>
class StdTestAllocator {
    // This allocator implements the minimal interface to comply with section
    // 20.1.5 ([lib.allocator.requirements]) of the C++03 standard.  Instances
    // of this allocator delegate their operations to a globally configured
    // delegate allocator as C++03 compliant allocators cannot have
    // individually identifiable state (see 'StdTestAllocatorConfiguration' and
    // 'StdTestAllocatorConfigurationGuard).

  public:
    // PUBLIC TYPES
    // Deliberately use types that will *not* have the same representation as
    // the default 'size_t/ptrdiff_t' on most 64-bit platforms, yet will be
    // wide enough to support our regular testing, as verified on 32-bit
    // platforms.
    typedef unsigned int  size_type;
    typedef int           difference_type;
    typedef TYPE         *pointer;
    typedef const TYPE   *const_pointer;
    typedef TYPE&         reference;
    typedef const TYPE&   const_reference;
    typedef TYPE          value_type;

    template <class OTHER_TYPE>
    struct rebind
    {
        // This nested 'struct' template, parameterized by some 'OTHER_TYPE',
        // provides a namespace for an 'other' type alias, which is an
        // allocator type following the same template as this one but that
        // allocates elements of 'OTHER_TYPE'.  Note that this allocator type
        // is convertible to and from 'other' for any 'OTHER_TYPE' including
        // 'void'.

        typedef StdTestAllocator<OTHER_TYPE> other;
    };

    // CREATORS
    StdTestAllocator();
        // Create a 'StdTestAllocator' object.

    // StdTestAllocator(const StdTestAllocator& original) = default;
        // Create a 'StdTestAllocator' object.  Note that this object will
        // compare equal to the default constructed object, because this type
        // has no state.

    template <class OTHER_TYPE>
    StdTestAllocator(const StdTestAllocator<OTHER_TYPE>&);
        // Create a 'StdTestAllocator' object.  Note that this object will
        // compare equal to the default constructed object, because this type
        // has no state.

    // ~StdTestAllocator() = default;
        // Destroy this object.

    // MANIPULATORS
    // StdTestAllocator& operator=(const StdTestAllocator& rhs) = default;
        // Assign to this object the value of the specified 'rhs' object, and
        // return a reference providing modifiable access to this object.

    pointer allocate(size_type numElements);
        // Allocate enough (properly aligned) space for the specified
        // 'numElements' of type 'T'.  The behavior is undefined unless
        // 'numElements <= max_size()'.

    void deallocate(pointer address, size_type numElements = 1);
        // Return memory previously at the specified 'address' for
        // 'numElements' back to this allocator.  The 'numElements' argument is
        // ignored by this allocator type.  The behavior is undefined unless
        // 'address' was allocated using this allocator object and has not
        // already been deallocated.

    void construct(pointer address, const TYPE& value);
        // Copy-construct a 'TYPE' object at the specified 'address'.  Do not
        // directly allocate memory.  The behavior is undefined unless
        // 'address' is properly aligned for 'TYPE'.

    void destroy(pointer address);
        // Invoke the 'TYPE' destructor for the object at the specified
        // 'address'.

    // ACCESSORS
    pointer address(reference object) const;
        // Return the address providing modifiable access to 'object'.

    const_pointer address(const_reference object) const;
        // Return the address providing non-modifiable access to 'object'.

    size_type max_size() const;
        // Return the maximum number of elements of type 'TYPE' that can be
        // allocated using this allocator in a single call to the 'allocate'
        // method.  Note that there is no guarantee that attempts at allocating
        // less elements than the value returned by 'max_size' will not throw.
};

                          // ============================
                          // class StdTestAllocator<void>
                          // ============================

template <>
class StdTestAllocator<void> {
    // This specialization of 'StdTestAllocator' for 'void' type as the
    // parameterized 'TYPE' does not contain members that are unrepresentable
    // for 'void'.

  public:
    // PUBLIC TYPES
    // Deliberately use types that will *not* have the same representation as
    // the default 'size_t/ptrdiff_t' on most 64-bit platforms, yet will be
    // wide enough to support our regular testing, as verified on 32-bit
    // platforms.
    typedef unsigned int  size_type;
    typedef int           difference_type;
    typedef void         *pointer;
    typedef const void   *const_pointer;
    typedef void          value_type;

    template <class OTHER_TYPE>
    struct rebind
    {
        // This nested 'struct' template, parameterized by some 'OTHER_TYPE',
        // provides a namespace for an 'other' type alias, which is an
        // allocator type following the same template as this one but that
        // allocates elements of 'OTHER_TYPE'.  Note that this allocator type
        // is convertible to and from 'other' for any 'OTHER_TYPE' including
        // 'void'.

        typedef StdTestAllocator<OTHER_TYPE> other;
    };

    // CREATORS
    StdTestAllocator();
        // Create a 'StdTestAllocator' object.

    // StdTestAllocator(const StdTestAllocator& original) = default;
        // Create a 'StdTestAllocator' object.  Note that this object will
        // compare equal to the default constructed object because, because
        // this type has no state.

    template <class OTHER_TYPE>
    StdTestAllocator(const StdTestAllocator<OTHER_TYPE>&);
        // Create a 'StdTestAllocator' object.  Note that this object will
        // compare equal to the default constructed object because, because
        // this type has no state.

    // ~StdTestAllocator() = default;
        // Destroy this object.

    // MANIPULATORS
    // StdTestAllocator& operator=(
    //                          const StdTestAllocator& rhs) = default;
        // Assign to this object the value of the specified 'rhs' object, and
        // return a reference providing modifiable access to this object.
};

// FREE OPERATORS
template <class TYPE1, class TYPE2>
bool operator==(const StdTestAllocator<TYPE1>& lhs,
                const StdTestAllocator<TYPE2>& rhs);
    // Return 'true' because 'StdTestAllocator' does not hold a state.

template <class TYPE1, class TYPE2>
bool operator!=(const StdTestAllocator<TYPE1>& lhs,
                const StdTestAllocator<TYPE2>& rhs);
    // Return 'false' because 'StdTestAllocator' does not hold a state.


                        // ======================
                        // class StdTestAllocator
                        // ======================

struct StdTestAllocator_CommonUtil {
    // This 'struct' provides a namespace for utilities that are common to
    // all instantiations of the 'StdTestAllocator' class template.

    // CLASS METHODS
    static unsigned int maxSize(size_t elementSize);
        // Return the maximum number of objects, each taking the specified
        // 'elementSize' bytes of storage, that can potentially be allocated by
        // a 'StdTestAllocator'.  Note that this function is mostly about
        // insulating consumers of this component from a standard header, so
        // that this test component does not hide missing header dependencies
        // in testing scenarios.
};

// ============================================================================
//                  INLINE AND TEMPLATE FUNCTION IMPLEMENTATIONS
// ============================================================================

                        // ----------------------------------------
                        // class StdTestAllocatorConfigurationGuard
                        // ----------------------------------------

// CREATORS
inline
StdTestAllocatorConfigurationGuard::StdTestAllocatorConfigurationGuard(
                                          bslma::Allocator *temporaryAllocator)
: d_original_p(StdTestAllocatorConfiguration::delegateAllocator())
{
    BSLS_ASSERT(temporaryAllocator);

    StdTestAllocatorConfiguration::setDelegateAllocatorRaw(temporaryAllocator);
}

inline
StdTestAllocatorConfigurationGuard::~StdTestAllocatorConfigurationGuard()
{
    BSLS_ASSERT(d_original_p);

    StdTestAllocatorConfiguration::setDelegateAllocatorRaw(d_original_p);
}

                        // ----------------------
                        // class StdTestAllocator
                        // ----------------------

// CREATORS
template <class TYPE>
inline
StdTestAllocator<TYPE>::StdTestAllocator()
{
}

template <class TYPE>
template <class OTHER>
StdTestAllocator<TYPE>::StdTestAllocator(const StdTestAllocator<OTHER>&)
{
}

// MANIPULATORS
template <class TYPE>
inline
typename StdTestAllocator<TYPE>::pointer
StdTestAllocator<TYPE>::allocate(typename StdTestAllocator<TYPE>::size_type
                                                                   numElements)
{
    return
      static_cast<pointer>(StdTestAllocatorConfiguration::delegateAllocator()->
            allocate(bslma::Allocator::size_type(numElements * sizeof(TYPE))));
}

template <class TYPE>
inline
void StdTestAllocator<TYPE>::deallocate(pointer address, size_type)
{
    StdTestAllocatorConfiguration::delegateAllocator()->deallocate(address);
}

template <class TYPE>
inline
void StdTestAllocator<TYPE>::construct(pointer address, const TYPE& value)
{
    new(static_cast<void*>(address)) TYPE(value);
}

template <class TYPE>
inline
void StdTestAllocator<TYPE>::destroy(pointer address)
{
    address->~TYPE();
}

template <class TYPE>
inline
typename StdTestAllocator<TYPE>::pointer
StdTestAllocator<TYPE>::address(reference object) const
{
    return bsls::Util::addressOf(object);
}

template <class TYPE>
inline
typename StdTestAllocator<TYPE>::const_pointer
StdTestAllocator<TYPE>::address(const_reference object) const
{
    return bsls::Util::addressOf(object);
}

template <class TYPE>
inline
typename StdTestAllocator<TYPE>::size_type
StdTestAllocator<TYPE>::max_size() const
{
    return StdTestAllocator_CommonUtil::maxSize(sizeof(TYPE));
}

                        // ----------------------------
                        // class StdTestAllocator<void>
                        // ----------------------------

// CREATORS
inline
StdTestAllocator<void>::StdTestAllocator()
{
}

template <class OTHER>
StdTestAllocator<void>::StdTestAllocator(const StdTestAllocator<OTHER>&)
{
}

// FREE OPERATORS
template <class TYPE1, class TYPE2>
inline
bool operator==(const bsltf::StdTestAllocator<TYPE1>&,
                const bsltf::StdTestAllocator<TYPE2>&)
{
    return true;
}

template <class TYPE1, class TYPE2>
inline
bool operator!=(const bsltf::StdTestAllocator<TYPE1>&,
                const bsltf::StdTestAllocator<TYPE2>&)
{
    return false;
}

}  // close package namespace
}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2013 Bloomberg Finance L.P.
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
