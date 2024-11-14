// bslma_memoryresourceimpsupport.h                                   -*-C++-*-
#ifndef INCLUDED_BSLMA_MEMORYRESOURCEIMPSUPPORT
#define INCLUDED_BSLMA_MEMORYRESOURCEIMPSUPPORT

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide support for implementing memory resources
//
//@CLASSES:
// bslma::MemoryResourceImpSupport - namespace for resource support functions
//
//@SEE_ALSO: bslma_memoryresource, bslma_allocator
//
//@DESCRIPTION: This component provides support functions and types for
// implementing a class derived from `bsl::memory_resource`. Currently, it
// provides a utility function, `singularPointer`, that provides a pointer
// suitable for an `allocate` function to return when a zero-sized allocation
// is requested.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Write a `memory_resource` using `singularPointer`
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// In this example, we derive a `NewDeleteResource` class, derived from
// `bsl::memory_resource`, that allocates and deallocate using `operator new`
// and `operator delete`, respectively, and uses `singularPointer()` as a
// special value when allocating and allocating zero bytes.
//
// First, we define the class interface, which implements the protected virtual
// functions `do_allocate`, `do_deallocate`, and `do_is_equal`:
// ```
//  #include <bslma_memoryresource.h>
//  #include <bslma_memoryresourceimpsupport.h>
//  #include <bsls_alignmentutil.h>
//  #include <bsls_assert.h>
//
//  /// Memory resource that allocates using `operator new`.
//  class NewDeleteResource : public bsl::memory_resource {
//    protected:
//      // PROTECTED MANIPULATORS
//
//      /// Return the specified `size` bytes with the specified `alignment`
//      /// allocated from the global heap using `operator new`.  The behavior
//      /// is undefined unless the specified `alignment` is less than or equal
//      /// to the maximum platform alignment.
//      void *do_allocate(std::size_t size, std::size_t alignment)
//                                                       BSLS_KEYWORD_OVERRIDE;
//
//      /// Deallocate the memory block specified by `p` having the specified
//      /// `size` and `alignment` from the global heap using `operator
//      /// delete`.  The behavior is undefined unless `p` was returned from a
//      /// previous call to `allocate`, using the same `size` and `alignment`.
//      void do_deallocate(void *p, std::size_t size, std::size_t alignment)
//                                                       BSLS_KEYWORD_OVERRIDE;
//
//      /// Return `true` if `x` is a `NewDeleteResource` and `false`
//      /// otherwise.
//      bool do_is_equal(const bsl::memory_resource& x) const
//                                 BSLS_KEYWORD_NOEXCEPT BSLS_KEYWORD_OVERRIDE;
//  };
// ```
// Next, we implement the `do_allocate` method, which forwards most requests to
// `operator new`. Section [basic.stc.dynamic.allocation] of the C++ standard,
// however states that the return value of an allocation function when the
// requested size is zero is a *non-null* pointer to a suitably aligned block
// of storage, so we use `singularPointer` to provide the address in such
// circumstances:
// ```
//  #include <new>      // `align_val_t` and aligned `new`
//
//  void *NewDeleteResource::do_allocate(std::size_t size,
//                                       std::size_t alignment)
//  {
//      if (0 == size) {
//          return bslma::MemoryResourceImpSupport::singularPointer();
//      }
//
//  #ifdef __cpp_aligned_new
//      return ::operator new(size, std::align_val_t(alignment));
//  #else
//      (void) alignment;
//      BSLS_ASSERT(alignment <= bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT);
//      return ::operator new(size);
//  #endif
//  }
// ```
// Next, we implement the `do_deallocate` method, which forwards most requests
// to `operator delete`, but does nothing if the incoming pointer was the
// result of a zero-sized allocation:
// ```
//  void NewDeleteResource::do_deallocate(void        *p,
//                                        std::size_t  size,
//                                        std::size_t  alignment)
//  {
//      (void) size;  // Not used in OPT build
//
//      if (bslma::MemoryResourceImpSupport::singularPointer() == p) {
//          BSLS_ASSERT(0 == size);
//          return;
//      }
//
//      BSLS_ASSERT(0 < size);
//  #ifdef __cpp_aligned_new
//      ::operator delete(p, std::align_val_t(alignment));
//  #else
//      (void) (size, alignment);
//      BSLS_ASSERT(alignment <= bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT);
//      ::operator delete(p);
//  #endif
//  }
// ```
// Next, we complete the implementation with `do_is_equal`.  All instances of
// `NewDeleteResource` compare equal, so we need only check that the argument
// is a `NewDeleteResource`.  As a short-cut, we check if the address of `b` is
// the same as `this`, to quickly catch the common case that they are both
// pointers to the singleton object:
// ```
//  bool NewDeleteResource::do_is_equal(const bsl::memory_resource& b) const
//                                                        BSLS_KEYWORD_NOEXCEPT
//  {
//      return this == &b || 0 != dynamic_cast<const NewDeleteResource *>(&b);
//  }
// ```
// Now, when we call `allocate`, we observe that each non-zero allocation
// yields a different pointer, whereas all of the zero allocations yield the
// same address.
// ```
//  int main()
//  {
//      NewDeleteResource r;
//      void *p1 = r.allocate(1, 1);
//      void *p2 = r.allocate(8);
//      assert(p1 != p2);
//
//      void *p3 = r.allocate(0, 1);
//      void *p4 = r.allocate(0, 4);
//      assert(p3 != p1);
//      assert(p3 != p2);
//      assert(p3 == p4);
// ```
// Finally, when we deallocate memory, we would expect nothing to happen when
// deallocating the zero-sized blocks `p3` and `p4`:
// ```
//      r.deallocate(p1, 1, 1);
//      r.deallocate(p2, 8);
//      r.deallocate(p3, 0, 1);
//      r.deallocate(p4, 0, 4);
//  }
// ```

#include <bslscm_version.h>

#include <bsls_alignmentutil.h>
#include <bsls_keyword.h>

namespace BloombergLP {
namespace bslma {

                // ===========================================
                // struct MemoryResourceImpSupport_AlignedData
                // ===========================================

/// COMPONENT-PRIVATE struct -- DO NOT USE.  Maximally aligned `struct` holding
/// an unsigned 64-bit integer.  This `struct` is exposed in the component
/// header so that `MemoryResourceImpSupport::singularPointer` can be
/// implemented as `constexpr` and `inline`.
struct MemoryResourceImpSupport_AlignedData {
    unsigned long long                  d_data;
    bsls::AlignmentUtil::MaxAlignedType d_alignment;
};

                    // ==============================
                    // class MemoryResourceImpSupport
                    // ==============================

/// Namespace for functions that support implementing a memory resource.
struct MemoryResourceImpSupport {

  private:

    // PRIVATE CLASS DATA

    /// Singleton used to produce singular address distinct from all other
    /// addresses.
    static const MemoryResourceImpSupport_AlignedData s_singularObject;

  public:
    // CLASS METHODS

    /// Return a maximally aligned, non-null pointer that cannnot organically
    /// result from any operation other than calling this function; i.e., it is
    /// a *singular* pointer.  Within a single program execution, no other
    /// current or future object, of any storage duration, will have an address
    /// matching this function's return value.  The returned address may be
    /// used by an allocation function to indicate a *non-allocation*, e.g.,
    /// the result of allocating zero bytes.  The behavior is undefined if the
    /// returned pointer is dereferenced.  Note that the same address is
    /// returned each time this function is called.
    BSLS_KEYWORD_CONSTEXPR static void *singularPointer();
};

// ============================================================================
//                TEMPLATE AND INLINE FUNCTION IMPLEMENTATIONS
// ============================================================================

// CLASS METHODS

inline BSLS_KEYWORD_CONSTEXPR void *MemoryResourceImpSupport::singularPointer()
{
    // The singleton object is 'const', so any attempt to construct an object
    // through the returned pointer should result in a protection error.
    return const_cast<unsigned long long *>(&s_singularObject.d_data);
}

}  // close package namespace


// FREE OPERATORS

}  // close enterprise namespace

#endif // ! defined(INCLUDED_BSLMA_MEMORYRESOURCEIMPSUPPORT)

// ----------------------------------------------------------------------------
// Copyright 2024 Bloomberg Finance L.P.
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
