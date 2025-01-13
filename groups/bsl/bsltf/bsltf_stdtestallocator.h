// bsltf_stdtestallocator.h                                           -*-C++-*-
#ifndef INCLUDED_BSLTF_STDTESTALLOCATOR
#define INCLUDED_BSLTF_STDTESTALLOCATOR

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a minimal standard compliant allocator.
//
//@CLASSES:
//      bsltf::StdTestAllocatorConfiguration: namespace to configure allocator
// bsltf::StdTestAllocatorConfigurationGuard: configuration scoped guard
//                   bsltf::StdTestAllocator: standard compliant allocator
//
//@DESCRIPTION: This component provides an allocator, `StdTestAllocator`, that
// defines the minimal interface to comply with section 20.1.5
// ([lib.allocator.requirements]) of the C++03 standard.  This type can be used
// to verify that constructs designed to support a standard-compliant allocator
// access the allocator only through the standard-defined interface.
//
// `StdTestAllocator` delegates its operations to a static `bslma::Allocator`
// (delegate allocator) that can be configured by the utilities provided in the
// namespace `StdTestAllocatorConfiguration`.
// `StdTestAllocatorConfigurationGuard` provides a scoped guard to enable
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
// ```
// template <class TYPE, class ALLOCATOR>
// class MyContainer {
//     // This container type is parameterized on a standard allocator type
//     // and contains a single object, always initialized, which can be
//     // replaced and accessed.
//
//     // DATA MEMBERS
//     ALLOCATOR  d_allocator;  // allocator used to supply memory (held, not
//                              // owned)
//
//     TYPE      *d_object_p;   // pointer to the contained object
//
//   public:
//     // CONSTRUCTORS
//     MyContainer(const TYPE& object);
//         // Create an container containing the specified 'object', using the
//         // parameterized 'ALLOCATOR' to supply memory.
//
//     ~MyContainer();
//         // Destroy this container.
//
//     // MANIPULATORS
//     TYPE& object();
//         // Return a reference providing modifiable access to the object
//         // contained in this container.
//
//     // ACCESSORS
//     const TYPE& object() const;
//         // Return a reference providing non-modifiable access to the object
//         // contained in this container.
// };
// ```
// Then, we define the member functions of `MyContainer`:
// ```
// // CREATORS
// template <class TYPE, class ALLOCATOR>
// MyContainer<TYPE, ALLOCATOR>::MyContainer(const TYPE& object)
// {
//     d_object_p = d_allocator.allocate(1);
//     d_allocator.construct(d_object_p, object);
// }
//
// template <class TYPE, class ALLOCATOR>
// MyContainer<TYPE, ALLOCATOR>::~MyContainer()
// {
//     d_allocator.destroy(d_object_p);
//     d_allocator.deallocate(d_object_p);
// }
//
// // MANIPULATORS
// template <class TYPE, class ALLOCATOR>
// TYPE& MyContainer<TYPE, ALLOCATOR>::object()
// {
//     return *d_object_p;
// }
//
// // ACCESSORS
// template <class TYPE, class ALLOCATOR>
// const TYPE& MyContainer<TYPE, ALLOCATOR>::object() const
// {
//     return *d_object_p;
// }
// ```
// Now, we use `StdTestAllocator` to implement a simple test for `MyContainer`
// to verify it correctly uses a parameterized allocator using only the C++03
// standard methods:
// ```
// bslma_TestAllocator oa("object", veryVeryVeryVerbose);
// StdTestAllocatorConfigurationGuard stag(&oa);
// {
//     typedef MyContainer<int, StdTestAllocator<int> > Obj;
//
//     Obj mX(2); const Obj& X = mX;
//     assert(sizeof(int) == oa.numBytesInUse());
//
//     assert(X.object() == 2);
//
//     mX.object() = -10;
//     assert(X.object() == -10);
// }
//
// assert(0 == oa.numBytesInUse());
// ```

#include <bslscm_version.h>

#include <bslma_allocator.h>
#include <bslma_isstdallocator.h>

#include <bslmf_util.h>    // 'forward(V)'

#include <bsls_assert.h>
#include <bsls_bslexceptionutil.h>
#include <bsls_compilerfeatures.h>
#include <bsls_types.h>
#include <bsls_util.h>

#include <new>
#include <stddef.h>

#if BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
// clang-format off
// Include version that can be compiled with C++03
// Generated on Mon Jan 13 08:31:33 2025
// Command line: sim_cpp11_features.pl bsltf_stdtestallocator.h

# define COMPILING_BSLTF_STDTESTALLOCATOR_H
# include <bsltf_stdtestallocator_cpp03.h>
# undef COMPILING_BSLTF_STDTESTALLOCATOR_H

// clang-format on
#else

namespace BloombergLP {
namespace bsltf {

                        // ===================================
                        // class StdTestAllocatorConfiguration
                        // ===================================

/// This `struct` provides a namespace for functions that manipulate and
/// access the *delegate allocator* for `StdTestAllocator`.  The delegate
/// allocator is the allocator to which `StdTestAllocator` objects delegate
/// their operations.  The provided operations are *not* thread-safe.  Note
/// that this allocator is configured globally as C++03 standard compliant
/// allocators cannot have individually identifiable state.
struct StdTestAllocatorConfiguration {

  public:
    // CLASS METHODS

    /// Set the address of the delegate allocator to the specified
    /// `basicAllocator`.
    static void setDelegateAllocatorRaw(bslma::Allocator *basicAllocator);

    /// Return the address of the delegate allocator.  Note that, this
    /// method will initially return
    /// `&bslma_NewDeleteAllocator::singleton()` if the
    /// `setDelegatingAllocator` class method has not been called.
    static bslma::Allocator* delegateAllocator();
};

                        // ========================================
                        // class StdTestAllocatorConfigurationGuard
                        // ========================================

/// Upon construction, an object of this class saves the current *delegate
/// allocator* for `StdTestAllocator` and and installs the user-specified
/// allocator as the delegate allocator.  The delegate allocator is the
/// globally configured allocator to which an `StdTestAllocator` objects
/// delegate their operations.  On destruction, the original delegate
/// allocator is restored.
class StdTestAllocatorConfigurationGuard {

    bslma::Allocator *d_original_p;  // original (restore at destruction)

  private:
    // NOT IMPLEMENTED
    StdTestAllocatorConfigurationGuard(
                                    const StdTestAllocatorConfigurationGuard&);
    StdTestAllocatorConfigurationGuard& operator=(
                                    const StdTestAllocatorConfigurationGuard&);

  public:
    // CREATORS

    /// Create a scoped guard that installs the specified
    /// `temporaryAllocator` as the delegate allocator.
    explicit
    StdTestAllocatorConfigurationGuard(bslma::Allocator *temporaryAllocator);

    /// Restore the delegate allocator that was in place when this scoped
    /// guard was created and destroy this guard.
    ~StdTestAllocatorConfigurationGuard();
};


                        // ======================
                        // class StdTestAllocator
                        // ======================

/// This allocator implements the minimal interface to comply with section
/// 20.1.5 ([lib.allocator.requirements]) of the C++03 standard.  Instances
/// of this allocator delegate their operations to a globally configured
/// delegate allocator as C++03 compliant allocators cannot have
/// individually identifiable state (see `StdTestAllocatorConfiguration` and
/// 'StdTestAllocatorConfigurationGuard).
template <class TYPE>
class StdTestAllocator {

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(StdTestAllocator, bslma::IsStdAllocator);

    // PUBLIC TYPES
    // Deliberately use types that will *not* have the same representation as
    // the default 'size_t/ptrdiff_t' on most 64-bit platforms, yet will be
    // wide enough to support our regular testing, as verified on 32-bit
    // platforms.
    typedef bsls::Types::UintPtr  size_type;
    typedef bsls::Types::IntPtr   difference_type;
    typedef TYPE                 *pointer;
    typedef const TYPE           *const_pointer;
    typedef TYPE&                 reference;
    typedef const TYPE&           const_reference;
    typedef TYPE                  value_type;

    /// This nested `struct` template, parameterized by some
    /// `BDE_OTHER_TYPE`, provides a namespace for an `other` type alias,
    /// which is an allocator type following the same template as this one
    /// but that allocates elements of `BDE_OTHER_TYPE`.  Note that this
    /// allocator type is convertible to and from `other` for any
    /// `BDE_OTHER_TYPE` including `void`.
    template <class BDE_OTHER_TYPE>
    struct rebind
    {

        typedef StdTestAllocator<BDE_OTHER_TYPE> other;
    };

    // CREATORS

    /// Create a `StdTestAllocator` object.
    StdTestAllocator();

    // StdTestAllocator(const StdTestAllocator& original) = default;
        // Create a 'StdTestAllocator' object.  Note that this object will
        // compare equal to the default constructed object, because this type
        // has no state.

    /// Create a `StdTestAllocator` object.  Note that this object will
    /// compare equal to the default constructed object, because this type
    /// has no state.
    template <class BDE_OTHER_TYPE>
    StdTestAllocator(const StdTestAllocator<BDE_OTHER_TYPE>&);

    // ~StdTestAllocator() = default;
        // Destroy this object.

    // MANIPULATORS
    // StdTestAllocator& operator=(const StdTestAllocator& rhs) = default;
        // Assign to this object the value of the specified 'rhs' object, and
        // return a reference providing modifiable access to this object.

    /// Allocate enough (properly aligned) space for the specified
    /// `numElements` of type `T`.  If the configured delegate allocator is
    /// unable to fulfill the allocation request, an exception (typically
    /// `bsl::bad_alloc`) will be thrown.  The behavior is undefined unless
    /// `numElements <= max_size()`.
    pointer allocate(size_type numElements);

    /// Return memory previously at the specified `address` for
    /// `numElements` back to this allocator.  The `numElements` argument is
    /// ignored by this allocator type.  The behavior is undefined unless
    /// `address` was allocated using this allocator object and has not
    /// already been deallocated.
    void deallocate(pointer address, size_type numElements = 1);

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES // $var-args=14

    /// Create an object of (template parameter) `ELEMENT_TYPE` at the
    /// specified `address`, constructed by forwarding the specified
    /// `argument1` and the (variable number of) additional specified
    /// `arguments` to the corresponding constructor of `ELEMENT_TYPE`.  The
    /// behavior is undefined unless `address` refers to a block of memory
    /// having sufficient size and alignment for an object of
    /// `ELEMENT_TYPE`.
    template <class ELEMENT_TYPE, class... Args>
    void construct(ELEMENT_TYPE *address, Args&&... arguments);
#endif

    /// Call the `ELEMENT_TYPE` destructor for the object at the specified
    /// `address` but do not deallocate the memory at `address`.
    template <class ELEMENT_TYPE>
    void destroy(ELEMENT_TYPE *address);

    // ACCESSORS

    /// Return the address providing modifiable access to `object`.
    pointer address(reference object) const;

    /// Return the address providing non-modifiable access to `object`.
    const_pointer address(const_reference object) const;

    /// Return the maximum number of elements of type `TYPE` that can be
    /// allocated using this allocator in a single call to the `allocate`
    /// method.  Note that there is no guarantee that attempts at allocating
    /// less elements than the value returned by `max_size` will not throw.
    size_type max_size() const;
};

                          // ============================
                          // class StdTestAllocator<void>
                          // ============================

/// This specialization of `StdTestAllocator` for `void` type as the
/// parameterized `TYPE` does not contain members that are unrepresentable
/// for `void`.
template <>
class StdTestAllocator<void> {

  public:
    // PUBLIC TYPES

    // 'size_type' and 'difference_type' were deliberately changed from fixed
    // 32 bit types to being the size of a pointer, to avoid a cascade of
    // warnings on 64-bit builds.

    typedef bsls::Types::UintPtr  size_type;
    typedef bsls::Types::IntPtr   difference_type;
    typedef void                 *pointer;
    typedef const void           *const_pointer;
    typedef void                  value_type;

    /// This nested `struct` template, parameterized by some
    /// `BDE_OTHER_TYPE`, provides a namespace for an `other` type alias,
    /// which is an allocator type following the same template as this one
    /// but that allocates elements of `BDE_OTHER_TYPE`.  Note that this
    /// allocator type is convertible to and from `other` for any
    /// `BDE_OTHER_TYPE` including `void`.
    template <class BDE_OTHER_TYPE>
    struct rebind
    {

        typedef StdTestAllocator<BDE_OTHER_TYPE> other;
    };

    // CREATORS

    /// Create a `StdTestAllocator` object.
    StdTestAllocator();

    // StdTestAllocator(const StdTestAllocator& original) = default;
        // Create a 'StdTestAllocator' object.  Note that this object will
        // compare equal to the default constructed object because, because
        // this type has no state.

    /// Create a `StdTestAllocator` object.  Note that this object will
    /// compare equal to the default constructed object because, because
    /// this type has no state.
    template <class BDE_OTHER_TYPE>
    StdTestAllocator(const StdTestAllocator<BDE_OTHER_TYPE>&);

    // ~StdTestAllocator() = default;
        // Destroy this object.

    // MANIPULATORS
    // StdTestAllocator& operator=(
    //                          const StdTestAllocator& rhs) = default;
        // Assign to this object the value of the specified 'rhs' object, and
        // return a reference providing modifiable access to this object.
};

// FREE OPERATORS

/// Return `true` because `StdTestAllocator` does not hold a state.
template <class TYPE1, class TYPE2>
bool operator==(const StdTestAllocator<TYPE1>& lhs,
                const StdTestAllocator<TYPE2>& rhs);

/// Return `false` because `StdTestAllocator` does not hold a state.
template <class TYPE1, class TYPE2>
bool operator!=(const StdTestAllocator<TYPE1>& lhs,
                const StdTestAllocator<TYPE2>& rhs);


                        // ======================
                        // class StdTestAllocator
                        // ======================

/// This `struct` provides a namespace for utilities that are common to
/// all instantiations of the `StdTestAllocator` class template.
struct StdTestAllocator_CommonUtil {

    // CLASS METHODS

    /// Return the maximum number of objects, each taking the specified
    /// `elementSize` bytes of storage, that can potentially be allocated by
    /// a `StdTestAllocator`.  Note that this function is mostly about
    /// insulating consumers of this component from a standard header, so
    /// that this test component does not hide missing header dependencies
    /// in testing scenarios.
    static size_t maxSize(size_t elementSize);
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
template <class BDE_OTHER_TYPE>
StdTestAllocator<TYPE>::StdTestAllocator(
                                       const StdTestAllocator<BDE_OTHER_TYPE>&)
{
}

// MANIPULATORS
template <class TYPE>
inline
typename StdTestAllocator<TYPE>::pointer
StdTestAllocator<TYPE>::allocate(typename StdTestAllocator<TYPE>::size_type
                                                                   numElements)
{
    if (numElements > this->max_size()) {
        BloombergLP::bsls::BslExceptionUtil::throwBadAlloc();
    }

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

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES // $var-args=14
template <class TYPE>
template <class ELEMENT_TYPE, class... Args>
inline void
StdTestAllocator<TYPE>::construct(ELEMENT_TYPE *address, Args&&... arguments)
{
    ::new (static_cast<void*>(address)) ELEMENT_TYPE(
                             BSLS_COMPILERFEATURES_FORWARD(Args,arguments)...);
}
#endif

template <class TYPE>
template <class ELEMENT_TYPE>
inline
void StdTestAllocator<TYPE>::destroy(ELEMENT_TYPE *address)
{
    address->~ELEMENT_TYPE();
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

template <class BDE_OTHER_TYPE>
StdTestAllocator<void>::StdTestAllocator(
                                       const StdTestAllocator<BDE_OTHER_TYPE>&)
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

#endif // End C++11 code

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
