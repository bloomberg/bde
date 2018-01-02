// bsltf_stdstatefulallocator.h                                       -*-C++-*-
#ifndef INCLUDED_BSLTF_STDSTATEFULALLOCATOR
#define INCLUDED_BSLTF_STDSTATEFULALLOCATOR

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a minimal standard compliant allocator.
//
//@CLASSES:
//  bsltf::StdStatefulAllocator: standard compliant allocator managing state
//
//@SEE_ALSO: bsltf_stdtestallocator
//
//@DESCRIPTION: This component provides an allocator,
// 'bsltf::StdStatefulAllocator', that defines the minimal interface to comply
// with section 17.6.3.5 ([allocator.requirements]) of the C++11 standard,
// while still providing an externally visible and potentially distinct state
// for each allocator object.  This type can be used to verify that constructs
// designed to support a standard-compliant allocator access the allocator only
// through the standard-defined interface.
//
// 'StdStatefulAllocator' delegates its operations to the allocator passed at
// construction (or the default allocator if no allocator is passed) that is
// also the sole attribute of this class.  In most tests, a
// 'bslma::TestAllocator' will be passed.
//
// The key differences between this test allocator and a regular BDE allocator
// are:
//
//: o This allocator does not support the 'scoped' allocation model, so that
//:   elements in a container will often have a different allocator to the
//:   container object itself.
//:
//: o This allocator may propagate through copy operations, move operations
//:   and 'swap' operations, depending on how the template is configured as
//:   it is instantiated.
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
// First, we define a simple container type intended to be used with a C++11
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
//      // CREATORS
//      MyContainer(const TYPE& object, const ALLOCATOR& allocator);
//          // Create an container containing the specified 'object', using the
//          // specified 'allocator' to supply memory.
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
//  MyContainer<TYPE, ALLOCATOR>::MyContainer(const TYPE&      object,
//                                            const ALLOCATOR& allocator)
//  : d_allocator(allocator)
//  {
//      d_object_p = d_allocator.allocate(1);
//      new (static_cast<void *>(d_object_p)) TYPE(object);
//  }
//
//  template <class TYPE, class ALLOCATOR>
//  MyContainer<TYPE, ALLOCATOR>::~MyContainer()
//  {
//      d_object_p->~TYPE();
//      d_allocator.deallocate(d_object_p, 1);
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
// Now, we use 'bsltf::StdStatefulAllocator' to implement a simple test for
// 'MyContainer' to verify it correctly uses a parameterized allocator using
// only the C++11 standard methods:
//..
//  bslma::TestAllocator oa("object", veryVeryVeryVerbose);
//  {
//      typedef MyContainer<int, bsltf::StdStatefulAllocator<int> > Obj;
//
//      Obj        mX(2, bsltf::StdStatefulAllocator<int>(&oa));
//      const Obj& X = mX;
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

#ifndef INCLUDED_BSLMA_DEFAULT
#include <bslma_default.h>
#endif

#ifndef INCLUDED_BSLMF_ASSERT
#include <bslmf_assert.h>
#endif

#ifndef INCLUDED_BSLMF_INTEGRALCONSTANT
#include <bslmf_integralconstant.h>
#endif

#ifndef INCLUDED_BSLMF_ISSAME
#include <bslmf_issame.h>
#endif

#ifndef INCLUDED_BSLMF_UTIL
#include <bslmf_util.h>
#endif

#ifndef INCLUDED_NEW
#include <new>
#define INCLUDED_NEW
#endif

namespace BloombergLP {
namespace bsltf {

                        // ==========================
                        // class StdStatefulAllocator
                        // ==========================

template <class TYPE,
          bool  PROPAGATE_ON_CONTAINER_COPY_CONSTRUCTION = true,
          bool  PROPAGATE_ON_CONTAINER_COPY_ASSIGNMENT = true,
          bool  PROPAGATE_ON_CONTAINER_SWAP = true,
          bool  PROPAGATE_ON_CONTAINER_MOVE_ASSIGNMENT = true>
class StdStatefulAllocator {
    // This allocator implements the minimal interface to comply with section
    // 17.6.3.5 ([allocator.requirements]) of the C++11 standard, while
    // maintaining a distinct object state - in this case a wrapped pointer to
    // a 'bslma::Allocator'.  The template is configurable to control its
    // allocator propagation properties, but does not support the BDE "scoped"
    // allocator model, as scoped allocators should never propagate.  Instances
    // of this allocator delegate their operations to the wrapped allocator
    // that constitutes its state.  Note that while we define the various
    // traits used by the C++11 allocator traits facility, they actually mean
    // very little for this component, as it is the consumer of the allocator's
    // responsibility to check and apply the traits correctly, typically by
    // using 'bsl::allocator_traits' to perform all memory allocation tasks
    // rather than using the allocator directly.  The
    // 'PROPAGATE_ON_CONTAINER_COPY_CONSTRUCTION' flag is consumed directly
    // though, in the static member function
    // 'select_on_container_copy_construction'.

  private:
    // DATA
    bslma::Allocator *d_allocator_p;      // the wrapped allocator (held, not
                                          // owned)

  public:
    // PUBLIC TYPES
    typedef TYPE value_type;

    // For a minimal allocator, these should all be deducible for a C++11
    // container implementation.  Unfortunately, the C++03 implementation of
    // 'allocator_traits' supported by BDE does not try the leaps of template
    // metaprogramming necessary to deduce these types.  That is left for a
    // future C++11 implementation, where language makes such metaprograms
    // much simpler to write.

#if !defined(BSLSTL_ALLOCATOR_TRAITS_SUPPORTS_ALL_CPP11_DEDUCTIONS)
    typedef std::size_t     size_type;
    typedef std::ptrdiff_t  difference_type;
    typedef TYPE           *pointer;
    typedef const TYPE     *const_pointer;
#endif

    // TBD The following two 'typedef's are required for testing 'bslstl_deque'
    // and 'bslstl_vector' (because 'bslalg::ContainerBase', which is used by
    // both of those containers, expects these to exist).
    typedef TYPE&           reference;
    typedef const TYPE&     const_reference;

    typedef bsl::integral_constant<bool,
                                   PROPAGATE_ON_CONTAINER_COPY_ASSIGNMENT>
                                        propagate_on_container_copy_assignment;

    typedef bsl::integral_constant<bool, PROPAGATE_ON_CONTAINER_SWAP>
                                                   propagate_on_container_swap;

    typedef bsl::integral_constant<bool,
                                   PROPAGATE_ON_CONTAINER_MOVE_ASSIGNMENT>
                                        propagate_on_container_move_assignment;

    template <class BDE_OTHER_TYPE>
    struct rebind
    {
        // This nested 'struct' template, parameterized by some
        // 'BDE_OTHER_TYPE', provides a namespace for an 'other' type alias,
        // which is an allocator type following the same template as this one
        // but that allocates elements of 'BDE_OTHER_TYPE'.  Note that this
        // allocator type is convertible to and from 'other' for any
        // 'BDE_OTHER_TYPE' including 'void'.

        typedef StdStatefulAllocator<
                                 BDE_OTHER_TYPE,
                                 PROPAGATE_ON_CONTAINER_COPY_CONSTRUCTION,
                                 PROPAGATE_ON_CONTAINER_COPY_ASSIGNMENT,
                                 PROPAGATE_ON_CONTAINER_SWAP,
                                 PROPAGATE_ON_CONTAINER_MOVE_ASSIGNMENT> other;
    };

    // CREATORS
    explicit StdStatefulAllocator(bslma::Allocator *allocator = 0);
        // Create a 'StdStatefulAllocator' object wrapping the specified
        // 'allocator'.

    //! StdStatefulAllocator(const StdStatefulAllocator& original) = default;
        // Create an allocator having the same value as the specified
        // 'original' object.

    template <class BDE_OTHER_TYPE>
    StdStatefulAllocator(const StdStatefulAllocator<
                            BDE_OTHER_TYPE,
                            PROPAGATE_ON_CONTAINER_COPY_CONSTRUCTION,
                            PROPAGATE_ON_CONTAINER_COPY_ASSIGNMENT,
                            PROPAGATE_ON_CONTAINER_SWAP,
                            PROPAGATE_ON_CONTAINER_MOVE_ASSIGNMENT>& original);
        // Create a 'StdStatefulAllocator' object wrapping the same test
        // allocator as the specified 'original'.

    //! ~StdStatefulAllocator() = default;
        // Destroy this object.

    // MANIPULATORS
    //! StdStatefulAllocator&
    //! operator=(const StdStatefulAllocator& rhs) = default;
        // Assign to this object the value of the specified 'rhs' object, and
        // return a reference providing modifiable access to this object.

    TYPE *allocate(bslma::Allocator::size_type numElements);
        // Allocate enough (properly aligned) space for the specified
        // 'numElements' of the (template parameter) type 'TYPE'.

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES // $var-args=14
    template <class ELEMENT_TYPE, class... Args>
    void construct(ELEMENT_TYPE *address, Args&&... arguments);
        // Construct an object of the (template parameter) 'ELEMENT_TYPE', by
        // forwarding the specified (variable number of) 'arguments' to the
        // corresponding constructor of 'ELEMENT_TYPE', at the specified
        // uninitialized memory 'address'.  The behavior is undefined unless
        // 'address' is properly aligned for objects of 'ELEMENT_TYPE'.
#elif BSLS_COMPILERFEATURES_SIMULATE_VARIADIC_TEMPLATES
// {{{ BEGIN GENERATED CODE
// The following section is automatically generated.  **DO NOT EDIT**
// Generator command line: sim_cpp11_features.pl bsltf_stdstatefulallocator.h
    template <class ELEMENT_TYPE>
    void construct(ELEMENT_TYPE *address);

    template <class ELEMENT_TYPE, class Args_01>
    void construct(ELEMENT_TYPE *address,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01);

    template <class ELEMENT_TYPE, class Args_01,
                                  class Args_02>
    void construct(ELEMENT_TYPE *address,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02);

    template <class ELEMENT_TYPE, class Args_01,
                                  class Args_02,
                                  class Args_03>
    void construct(ELEMENT_TYPE *address,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03);

    template <class ELEMENT_TYPE, class Args_01,
                                  class Args_02,
                                  class Args_03,
                                  class Args_04>
    void construct(ELEMENT_TYPE *address,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04);

    template <class ELEMENT_TYPE, class Args_01,
                                  class Args_02,
                                  class Args_03,
                                  class Args_04,
                                  class Args_05>
    void construct(ELEMENT_TYPE *address,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05);

    template <class ELEMENT_TYPE, class Args_01,
                                  class Args_02,
                                  class Args_03,
                                  class Args_04,
                                  class Args_05,
                                  class Args_06>
    void construct(ELEMENT_TYPE *address,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06);

    template <class ELEMENT_TYPE, class Args_01,
                                  class Args_02,
                                  class Args_03,
                                  class Args_04,
                                  class Args_05,
                                  class Args_06,
                                  class Args_07>
    void construct(ELEMENT_TYPE *address,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07);

    template <class ELEMENT_TYPE, class Args_01,
                                  class Args_02,
                                  class Args_03,
                                  class Args_04,
                                  class Args_05,
                                  class Args_06,
                                  class Args_07,
                                  class Args_08>
    void construct(ELEMENT_TYPE *address,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) arguments_08);

    template <class ELEMENT_TYPE, class Args_01,
                                  class Args_02,
                                  class Args_03,
                                  class Args_04,
                                  class Args_05,
                                  class Args_06,
                                  class Args_07,
                                  class Args_08,
                                  class Args_09>
    void construct(ELEMENT_TYPE *address,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) arguments_08,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_09) arguments_09);

    template <class ELEMENT_TYPE, class Args_01,
                                  class Args_02,
                                  class Args_03,
                                  class Args_04,
                                  class Args_05,
                                  class Args_06,
                                  class Args_07,
                                  class Args_08,
                                  class Args_09,
                                  class Args_10>
    void construct(ELEMENT_TYPE *address,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) arguments_08,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_09) arguments_09,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_10) arguments_10);

    template <class ELEMENT_TYPE, class Args_01,
                                  class Args_02,
                                  class Args_03,
                                  class Args_04,
                                  class Args_05,
                                  class Args_06,
                                  class Args_07,
                                  class Args_08,
                                  class Args_09,
                                  class Args_10,
                                  class Args_11>
    void construct(ELEMENT_TYPE *address,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) arguments_08,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_09) arguments_09,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_10) arguments_10,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_11) arguments_11);

    template <class ELEMENT_TYPE, class Args_01,
                                  class Args_02,
                                  class Args_03,
                                  class Args_04,
                                  class Args_05,
                                  class Args_06,
                                  class Args_07,
                                  class Args_08,
                                  class Args_09,
                                  class Args_10,
                                  class Args_11,
                                  class Args_12>
    void construct(ELEMENT_TYPE *address,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) arguments_08,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_09) arguments_09,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_10) arguments_10,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_11) arguments_11,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_12) arguments_12);

    template <class ELEMENT_TYPE, class Args_01,
                                  class Args_02,
                                  class Args_03,
                                  class Args_04,
                                  class Args_05,
                                  class Args_06,
                                  class Args_07,
                                  class Args_08,
                                  class Args_09,
                                  class Args_10,
                                  class Args_11,
                                  class Args_12,
                                  class Args_13>
    void construct(ELEMENT_TYPE *address,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) arguments_08,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_09) arguments_09,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_10) arguments_10,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_11) arguments_11,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_12) arguments_12,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_13) arguments_13);

    template <class ELEMENT_TYPE, class Args_01,
                                  class Args_02,
                                  class Args_03,
                                  class Args_04,
                                  class Args_05,
                                  class Args_06,
                                  class Args_07,
                                  class Args_08,
                                  class Args_09,
                                  class Args_10,
                                  class Args_11,
                                  class Args_12,
                                  class Args_13,
                                  class Args_14>
    void construct(ELEMENT_TYPE *address,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) arguments_08,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_09) arguments_09,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_10) arguments_10,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_11) arguments_11,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_12) arguments_12,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_13) arguments_13,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_14) arguments_14);

#else
// The generated code below is a workaround for the absence of perfect
// forwarding in some compilers.
    template <class ELEMENT_TYPE, class... Args>
    void construct(ELEMENT_TYPE *address,
                         BSLS_COMPILERFEATURES_FORWARD_REF(Args)... arguments);
// }}} END GENERATED CODE
#endif

    void deallocate(TYPE *address, bslma::Allocator::size_type numElements);
        // Return memory previously allocated at the specified 'address' for
        // 'numElements' back to this allocator.  The 'numElements' argument is
        // ignored by this allocator type.  The behavior is undefined unless
        // 'address' was allocated using this allocator object and has not
        // already been deallocated.

    template <class ELEMENT_TYPE>
    void destroy(ELEMENT_TYPE *address);
        // Invoke the 'ELEMENT_TYPE' destructor for the object at the specified
        // 'address'.

    // ACCESSORS
#if !defined(BSLSTL_ALLOCATOR_TRAITS_SUPPORTS_ALL_CPP11_DEDUCTIONS)
    size_type max_size() const;
        // Return the maximum number of elements of type 'TYPE' that can be
        // allocated using this allocator in a single call to the 'allocate'
        // method.  Note that there is no guarantee that attempts at allocating
        // less elements than the value returned by 'max_size' will not throw.
        // *** DO NOT RELY ON THE CONTINUING PRESENT OF THIS METHOD *** THIS
        // METHOD WILL BE REMOVED ONCE 'bslstl::allocator_traits' PROPERLY
        // DEDUCES AN IMPLEMENTATION FOR THIS FUNCTION WHEN NOT SUPPLIED BY THE
        // ALLOCATOR DIRECTLY.
#endif

    StdStatefulAllocator select_on_container_copy_construction() const;
        // Return a copy of this object if the 'bool' template parameter
        // 'PROPAGATE_ON_CONTAINER_COPY_CONSTRUCTION' is true, and a copy of a
        // 'StdStatefulAllocator' object wrapping the default allocator
        // otherwise.

    bslma::Allocator *allocator() const;
        // Return the address of the allocator wrapped by this object.
};

// FREE OPERATORS
template <class TYPE,
          bool  PROPAGATE_ON_CONTAINER_COPY_CONSTRUCTION,
          bool  PROPAGATE_ON_CONTAINER_COPY_ASSIGNMENT,
          bool  PROPAGATE_ON_CONTAINER_SWAP,
          bool  PROPAGATE_ON_CONTAINER_MOVE_ASSIGNMENT>
bool operator==(const StdStatefulAllocator<
                                 TYPE,
                                 PROPAGATE_ON_CONTAINER_COPY_CONSTRUCTION,
                                 PROPAGATE_ON_CONTAINER_COPY_ASSIGNMENT,
                                 PROPAGATE_ON_CONTAINER_SWAP,
                                 PROPAGATE_ON_CONTAINER_MOVE_ASSIGNMENT>& lhs,
                const StdStatefulAllocator<
                                 TYPE,
                                 PROPAGATE_ON_CONTAINER_COPY_CONSTRUCTION,
                                 PROPAGATE_ON_CONTAINER_COPY_ASSIGNMENT,
                                 PROPAGATE_ON_CONTAINER_SWAP,
                                 PROPAGATE_ON_CONTAINER_MOVE_ASSIGNMENT>& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' have the same underlying
    // test allocator, and 'false' otherwise.

template <class TYPE,
          bool  PROPAGATE_ON_CONTAINER_COPY_CONSTRUCTION,
          bool  PROPAGATE_ON_CONTAINER_COPY_ASSIGNMENT,
          bool  PROPAGATE_ON_CONTAINER_SWAP,
          bool  PROPAGATE_ON_CONTAINER_MOVE_ASSIGNMENT>
bool operator!=(const StdStatefulAllocator<
                                 TYPE,
                                 PROPAGATE_ON_CONTAINER_COPY_CONSTRUCTION,
                                 PROPAGATE_ON_CONTAINER_COPY_ASSIGNMENT,
                                 PROPAGATE_ON_CONTAINER_SWAP,
                                 PROPAGATE_ON_CONTAINER_MOVE_ASSIGNMENT>& lhs,
                const StdStatefulAllocator<
                                 TYPE,
                                 PROPAGATE_ON_CONTAINER_COPY_CONSTRUCTION,
                                 PROPAGATE_ON_CONTAINER_COPY_ASSIGNMENT,
                                 PROPAGATE_ON_CONTAINER_SWAP,
                                 PROPAGATE_ON_CONTAINER_MOVE_ASSIGNMENT>& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' have different underlying
    // test allocators, and 'false' otherwise.


// ============================================================================
//                          INLINE DEFINITIONS
// ============================================================================

                        // --------------------------
                        // class StdStatefulAllocator
                        // --------------------------

// CREATORS
template <class TYPE,
          bool  PROPAGATE_ON_CONTAINER_COPY_CONSTRUCTION,
          bool  PROPAGATE_ON_CONTAINER_COPY_ASSIGNMENT,
          bool  PROPAGATE_ON_CONTAINER_SWAP,
          bool  PROPAGATE_ON_CONTAINER_MOVE_ASSIGNMENT>
inline
StdStatefulAllocator<TYPE,
                     PROPAGATE_ON_CONTAINER_COPY_CONSTRUCTION,
                     PROPAGATE_ON_CONTAINER_COPY_ASSIGNMENT,
                     PROPAGATE_ON_CONTAINER_SWAP,
                     PROPAGATE_ON_CONTAINER_MOVE_ASSIGNMENT>::
StdStatefulAllocator(bslma::Allocator *allocator)
: d_allocator_p(bslma::Default::allocator(allocator))
{
}

template <class TYPE,
          bool  PROPAGATE_ON_CONTAINER_COPY_CONSTRUCTION,
          bool  PROPAGATE_ON_CONTAINER_COPY_ASSIGNMENT,
          bool  PROPAGATE_ON_CONTAINER_SWAP,
          bool  PROPAGATE_ON_CONTAINER_MOVE_ASSIGNMENT>
template <class BDE_OTHER_TYPE>
inline
StdStatefulAllocator<TYPE,
                     PROPAGATE_ON_CONTAINER_COPY_CONSTRUCTION,
                     PROPAGATE_ON_CONTAINER_COPY_ASSIGNMENT,
                     PROPAGATE_ON_CONTAINER_SWAP,
                     PROPAGATE_ON_CONTAINER_MOVE_ASSIGNMENT>::
StdStatefulAllocator(const StdStatefulAllocator<
                             BDE_OTHER_TYPE,
                             PROPAGATE_ON_CONTAINER_COPY_CONSTRUCTION,
                             PROPAGATE_ON_CONTAINER_COPY_ASSIGNMENT,
                             PROPAGATE_ON_CONTAINER_SWAP,
                             PROPAGATE_ON_CONTAINER_MOVE_ASSIGNMENT>& original)
: d_allocator_p(original.allocator())
{
}

// MANIPULATORS
template <class TYPE,
          bool  PROPAGATE_ON_CONTAINER_COPY_CONSTRUCTION,
          bool  PROPAGATE_ON_CONTAINER_COPY_ASSIGNMENT,
          bool  PROPAGATE_ON_CONTAINER_SWAP,
          bool  PROPAGATE_ON_CONTAINER_MOVE_ASSIGNMENT>
inline
TYPE *
StdStatefulAllocator<TYPE,
                     PROPAGATE_ON_CONTAINER_COPY_CONSTRUCTION,
                     PROPAGATE_ON_CONTAINER_COPY_ASSIGNMENT,
                     PROPAGATE_ON_CONTAINER_SWAP,
                     PROPAGATE_ON_CONTAINER_MOVE_ASSIGNMENT>::allocate(
                                       bslma::Allocator::size_type numElements)
{
    return static_cast<TYPE *>(d_allocator_p->allocate(
                     bslma::Allocator::size_type(numElements * sizeof(TYPE))));
}

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES // $var-args=14
template <class TYPE,
          bool  PROPAGATE_ON_CONTAINER_COPY_CONSTRUCTION,
          bool  PROPAGATE_ON_CONTAINER_COPY_ASSIGNMENT,
          bool  PROPAGATE_ON_CONTAINER_SWAP,
          bool  PROPAGATE_ON_CONTAINER_MOVE_ASSIGNMENT>
template <class ELEMENT_TYPE, class... Args>
inline
void
StdStatefulAllocator<TYPE,
                     PROPAGATE_ON_CONTAINER_COPY_CONSTRUCTION,
                     PROPAGATE_ON_CONTAINER_COPY_ASSIGNMENT,
                     PROPAGATE_ON_CONTAINER_SWAP,
                     PROPAGATE_ON_CONTAINER_MOVE_ASSIGNMENT>::construct(
                                    ELEMENT_TYPE *address, Args&&... arguments)
{
    ::new (static_cast<void*>(address)) ELEMENT_TYPE(
                             BSLS_COMPILERFEATURES_FORWARD(Args,arguments)...);
}
#elif BSLS_COMPILERFEATURES_SIMULATE_VARIADIC_TEMPLATES
// {{{ BEGIN GENERATED CODE
// The following section is automatically generated.  **DO NOT EDIT**
// Generator command line: sim_cpp11_features.pl bsltf_stdstatefulallocator.h
template <class TYPE,
          bool  PROPAGATE_ON_CONTAINER_COPY_CONSTRUCTION,
          bool  PROPAGATE_ON_CONTAINER_COPY_ASSIGNMENT,
          bool  PROPAGATE_ON_CONTAINER_SWAP,
          bool  PROPAGATE_ON_CONTAINER_MOVE_ASSIGNMENT>
template <class ELEMENT_TYPE>
inline
void
StdStatefulAllocator<TYPE,
                     PROPAGATE_ON_CONTAINER_COPY_CONSTRUCTION,
                     PROPAGATE_ON_CONTAINER_COPY_ASSIGNMENT,
                     PROPAGATE_ON_CONTAINER_SWAP,
                     PROPAGATE_ON_CONTAINER_MOVE_ASSIGNMENT>::construct(
                                    ELEMENT_TYPE *address)
{
    ::new (static_cast<void*>(address)) ELEMENT_TYPE(
                             );
}

template <class TYPE,
          bool  PROPAGATE_ON_CONTAINER_COPY_CONSTRUCTION,
          bool  PROPAGATE_ON_CONTAINER_COPY_ASSIGNMENT,
          bool  PROPAGATE_ON_CONTAINER_SWAP,
          bool  PROPAGATE_ON_CONTAINER_MOVE_ASSIGNMENT>
template <class ELEMENT_TYPE, class Args_01>
inline
void
StdStatefulAllocator<TYPE,
                     PROPAGATE_ON_CONTAINER_COPY_CONSTRUCTION,
                     PROPAGATE_ON_CONTAINER_COPY_ASSIGNMENT,
                     PROPAGATE_ON_CONTAINER_SWAP,
                     PROPAGATE_ON_CONTAINER_MOVE_ASSIGNMENT>::construct(
                                    ELEMENT_TYPE *address,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01)
{
    ::new (static_cast<void*>(address)) ELEMENT_TYPE(
                          BSLS_COMPILERFEATURES_FORWARD(Args_01,arguments_01));
}

template <class TYPE,
          bool  PROPAGATE_ON_CONTAINER_COPY_CONSTRUCTION,
          bool  PROPAGATE_ON_CONTAINER_COPY_ASSIGNMENT,
          bool  PROPAGATE_ON_CONTAINER_SWAP,
          bool  PROPAGATE_ON_CONTAINER_MOVE_ASSIGNMENT>
template <class ELEMENT_TYPE, class Args_01,
                              class Args_02>
inline
void
StdStatefulAllocator<TYPE,
                     PROPAGATE_ON_CONTAINER_COPY_CONSTRUCTION,
                     PROPAGATE_ON_CONTAINER_COPY_ASSIGNMENT,
                     PROPAGATE_ON_CONTAINER_SWAP,
                     PROPAGATE_ON_CONTAINER_MOVE_ASSIGNMENT>::construct(
                                    ELEMENT_TYPE *address,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02)
{
    ::new (static_cast<void*>(address)) ELEMENT_TYPE(
                          BSLS_COMPILERFEATURES_FORWARD(Args_01,arguments_01),
                          BSLS_COMPILERFEATURES_FORWARD(Args_02,arguments_02));
}

template <class TYPE,
          bool  PROPAGATE_ON_CONTAINER_COPY_CONSTRUCTION,
          bool  PROPAGATE_ON_CONTAINER_COPY_ASSIGNMENT,
          bool  PROPAGATE_ON_CONTAINER_SWAP,
          bool  PROPAGATE_ON_CONTAINER_MOVE_ASSIGNMENT>
template <class ELEMENT_TYPE, class Args_01,
                              class Args_02,
                              class Args_03>
inline
void
StdStatefulAllocator<TYPE,
                     PROPAGATE_ON_CONTAINER_COPY_CONSTRUCTION,
                     PROPAGATE_ON_CONTAINER_COPY_ASSIGNMENT,
                     PROPAGATE_ON_CONTAINER_SWAP,
                     PROPAGATE_ON_CONTAINER_MOVE_ASSIGNMENT>::construct(
                                    ELEMENT_TYPE *address,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03)
{
    ::new (static_cast<void*>(address)) ELEMENT_TYPE(
                          BSLS_COMPILERFEATURES_FORWARD(Args_01,arguments_01),
                          BSLS_COMPILERFEATURES_FORWARD(Args_02,arguments_02),
                          BSLS_COMPILERFEATURES_FORWARD(Args_03,arguments_03));
}

template <class TYPE,
          bool  PROPAGATE_ON_CONTAINER_COPY_CONSTRUCTION,
          bool  PROPAGATE_ON_CONTAINER_COPY_ASSIGNMENT,
          bool  PROPAGATE_ON_CONTAINER_SWAP,
          bool  PROPAGATE_ON_CONTAINER_MOVE_ASSIGNMENT>
template <class ELEMENT_TYPE, class Args_01,
                              class Args_02,
                              class Args_03,
                              class Args_04>
inline
void
StdStatefulAllocator<TYPE,
                     PROPAGATE_ON_CONTAINER_COPY_CONSTRUCTION,
                     PROPAGATE_ON_CONTAINER_COPY_ASSIGNMENT,
                     PROPAGATE_ON_CONTAINER_SWAP,
                     PROPAGATE_ON_CONTAINER_MOVE_ASSIGNMENT>::construct(
                                    ELEMENT_TYPE *address,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04)
{
    ::new (static_cast<void*>(address)) ELEMENT_TYPE(
                          BSLS_COMPILERFEATURES_FORWARD(Args_01,arguments_01),
                          BSLS_COMPILERFEATURES_FORWARD(Args_02,arguments_02),
                          BSLS_COMPILERFEATURES_FORWARD(Args_03,arguments_03),
                          BSLS_COMPILERFEATURES_FORWARD(Args_04,arguments_04));
}

template <class TYPE,
          bool  PROPAGATE_ON_CONTAINER_COPY_CONSTRUCTION,
          bool  PROPAGATE_ON_CONTAINER_COPY_ASSIGNMENT,
          bool  PROPAGATE_ON_CONTAINER_SWAP,
          bool  PROPAGATE_ON_CONTAINER_MOVE_ASSIGNMENT>
template <class ELEMENT_TYPE, class Args_01,
                              class Args_02,
                              class Args_03,
                              class Args_04,
                              class Args_05>
inline
void
StdStatefulAllocator<TYPE,
                     PROPAGATE_ON_CONTAINER_COPY_CONSTRUCTION,
                     PROPAGATE_ON_CONTAINER_COPY_ASSIGNMENT,
                     PROPAGATE_ON_CONTAINER_SWAP,
                     PROPAGATE_ON_CONTAINER_MOVE_ASSIGNMENT>::construct(
                                    ELEMENT_TYPE *address,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05)
{
    ::new (static_cast<void*>(address)) ELEMENT_TYPE(
                          BSLS_COMPILERFEATURES_FORWARD(Args_01,arguments_01),
                          BSLS_COMPILERFEATURES_FORWARD(Args_02,arguments_02),
                          BSLS_COMPILERFEATURES_FORWARD(Args_03,arguments_03),
                          BSLS_COMPILERFEATURES_FORWARD(Args_04,arguments_04),
                          BSLS_COMPILERFEATURES_FORWARD(Args_05,arguments_05));
}

template <class TYPE,
          bool  PROPAGATE_ON_CONTAINER_COPY_CONSTRUCTION,
          bool  PROPAGATE_ON_CONTAINER_COPY_ASSIGNMENT,
          bool  PROPAGATE_ON_CONTAINER_SWAP,
          bool  PROPAGATE_ON_CONTAINER_MOVE_ASSIGNMENT>
template <class ELEMENT_TYPE, class Args_01,
                              class Args_02,
                              class Args_03,
                              class Args_04,
                              class Args_05,
                              class Args_06>
inline
void
StdStatefulAllocator<TYPE,
                     PROPAGATE_ON_CONTAINER_COPY_CONSTRUCTION,
                     PROPAGATE_ON_CONTAINER_COPY_ASSIGNMENT,
                     PROPAGATE_ON_CONTAINER_SWAP,
                     PROPAGATE_ON_CONTAINER_MOVE_ASSIGNMENT>::construct(
                                    ELEMENT_TYPE *address,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06)
{
    ::new (static_cast<void*>(address)) ELEMENT_TYPE(
                          BSLS_COMPILERFEATURES_FORWARD(Args_01,arguments_01),
                          BSLS_COMPILERFEATURES_FORWARD(Args_02,arguments_02),
                          BSLS_COMPILERFEATURES_FORWARD(Args_03,arguments_03),
                          BSLS_COMPILERFEATURES_FORWARD(Args_04,arguments_04),
                          BSLS_COMPILERFEATURES_FORWARD(Args_05,arguments_05),
                          BSLS_COMPILERFEATURES_FORWARD(Args_06,arguments_06));
}

template <class TYPE,
          bool  PROPAGATE_ON_CONTAINER_COPY_CONSTRUCTION,
          bool  PROPAGATE_ON_CONTAINER_COPY_ASSIGNMENT,
          bool  PROPAGATE_ON_CONTAINER_SWAP,
          bool  PROPAGATE_ON_CONTAINER_MOVE_ASSIGNMENT>
template <class ELEMENT_TYPE, class Args_01,
                              class Args_02,
                              class Args_03,
                              class Args_04,
                              class Args_05,
                              class Args_06,
                              class Args_07>
inline
void
StdStatefulAllocator<TYPE,
                     PROPAGATE_ON_CONTAINER_COPY_CONSTRUCTION,
                     PROPAGATE_ON_CONTAINER_COPY_ASSIGNMENT,
                     PROPAGATE_ON_CONTAINER_SWAP,
                     PROPAGATE_ON_CONTAINER_MOVE_ASSIGNMENT>::construct(
                                    ELEMENT_TYPE *address,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07)
{
    ::new (static_cast<void*>(address)) ELEMENT_TYPE(
                          BSLS_COMPILERFEATURES_FORWARD(Args_01,arguments_01),
                          BSLS_COMPILERFEATURES_FORWARD(Args_02,arguments_02),
                          BSLS_COMPILERFEATURES_FORWARD(Args_03,arguments_03),
                          BSLS_COMPILERFEATURES_FORWARD(Args_04,arguments_04),
                          BSLS_COMPILERFEATURES_FORWARD(Args_05,arguments_05),
                          BSLS_COMPILERFEATURES_FORWARD(Args_06,arguments_06),
                          BSLS_COMPILERFEATURES_FORWARD(Args_07,arguments_07));
}

template <class TYPE,
          bool  PROPAGATE_ON_CONTAINER_COPY_CONSTRUCTION,
          bool  PROPAGATE_ON_CONTAINER_COPY_ASSIGNMENT,
          bool  PROPAGATE_ON_CONTAINER_SWAP,
          bool  PROPAGATE_ON_CONTAINER_MOVE_ASSIGNMENT>
template <class ELEMENT_TYPE, class Args_01,
                              class Args_02,
                              class Args_03,
                              class Args_04,
                              class Args_05,
                              class Args_06,
                              class Args_07,
                              class Args_08>
inline
void
StdStatefulAllocator<TYPE,
                     PROPAGATE_ON_CONTAINER_COPY_CONSTRUCTION,
                     PROPAGATE_ON_CONTAINER_COPY_ASSIGNMENT,
                     PROPAGATE_ON_CONTAINER_SWAP,
                     PROPAGATE_ON_CONTAINER_MOVE_ASSIGNMENT>::construct(
                                    ELEMENT_TYPE *address,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) arguments_08)
{
    ::new (static_cast<void*>(address)) ELEMENT_TYPE(
                          BSLS_COMPILERFEATURES_FORWARD(Args_01,arguments_01),
                          BSLS_COMPILERFEATURES_FORWARD(Args_02,arguments_02),
                          BSLS_COMPILERFEATURES_FORWARD(Args_03,arguments_03),
                          BSLS_COMPILERFEATURES_FORWARD(Args_04,arguments_04),
                          BSLS_COMPILERFEATURES_FORWARD(Args_05,arguments_05),
                          BSLS_COMPILERFEATURES_FORWARD(Args_06,arguments_06),
                          BSLS_COMPILERFEATURES_FORWARD(Args_07,arguments_07),
                          BSLS_COMPILERFEATURES_FORWARD(Args_08,arguments_08));
}

template <class TYPE,
          bool  PROPAGATE_ON_CONTAINER_COPY_CONSTRUCTION,
          bool  PROPAGATE_ON_CONTAINER_COPY_ASSIGNMENT,
          bool  PROPAGATE_ON_CONTAINER_SWAP,
          bool  PROPAGATE_ON_CONTAINER_MOVE_ASSIGNMENT>
template <class ELEMENT_TYPE, class Args_01,
                              class Args_02,
                              class Args_03,
                              class Args_04,
                              class Args_05,
                              class Args_06,
                              class Args_07,
                              class Args_08,
                              class Args_09>
inline
void
StdStatefulAllocator<TYPE,
                     PROPAGATE_ON_CONTAINER_COPY_CONSTRUCTION,
                     PROPAGATE_ON_CONTAINER_COPY_ASSIGNMENT,
                     PROPAGATE_ON_CONTAINER_SWAP,
                     PROPAGATE_ON_CONTAINER_MOVE_ASSIGNMENT>::construct(
                                    ELEMENT_TYPE *address,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) arguments_08,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_09) arguments_09)
{
    ::new (static_cast<void*>(address)) ELEMENT_TYPE(
                          BSLS_COMPILERFEATURES_FORWARD(Args_01,arguments_01),
                          BSLS_COMPILERFEATURES_FORWARD(Args_02,arguments_02),
                          BSLS_COMPILERFEATURES_FORWARD(Args_03,arguments_03),
                          BSLS_COMPILERFEATURES_FORWARD(Args_04,arguments_04),
                          BSLS_COMPILERFEATURES_FORWARD(Args_05,arguments_05),
                          BSLS_COMPILERFEATURES_FORWARD(Args_06,arguments_06),
                          BSLS_COMPILERFEATURES_FORWARD(Args_07,arguments_07),
                          BSLS_COMPILERFEATURES_FORWARD(Args_08,arguments_08),
                          BSLS_COMPILERFEATURES_FORWARD(Args_09,arguments_09));
}

template <class TYPE,
          bool  PROPAGATE_ON_CONTAINER_COPY_CONSTRUCTION,
          bool  PROPAGATE_ON_CONTAINER_COPY_ASSIGNMENT,
          bool  PROPAGATE_ON_CONTAINER_SWAP,
          bool  PROPAGATE_ON_CONTAINER_MOVE_ASSIGNMENT>
template <class ELEMENT_TYPE, class Args_01,
                              class Args_02,
                              class Args_03,
                              class Args_04,
                              class Args_05,
                              class Args_06,
                              class Args_07,
                              class Args_08,
                              class Args_09,
                              class Args_10>
inline
void
StdStatefulAllocator<TYPE,
                     PROPAGATE_ON_CONTAINER_COPY_CONSTRUCTION,
                     PROPAGATE_ON_CONTAINER_COPY_ASSIGNMENT,
                     PROPAGATE_ON_CONTAINER_SWAP,
                     PROPAGATE_ON_CONTAINER_MOVE_ASSIGNMENT>::construct(
                                    ELEMENT_TYPE *address,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) arguments_08,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_09) arguments_09,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_10) arguments_10)
{
    ::new (static_cast<void*>(address)) ELEMENT_TYPE(
                          BSLS_COMPILERFEATURES_FORWARD(Args_01,arguments_01),
                          BSLS_COMPILERFEATURES_FORWARD(Args_02,arguments_02),
                          BSLS_COMPILERFEATURES_FORWARD(Args_03,arguments_03),
                          BSLS_COMPILERFEATURES_FORWARD(Args_04,arguments_04),
                          BSLS_COMPILERFEATURES_FORWARD(Args_05,arguments_05),
                          BSLS_COMPILERFEATURES_FORWARD(Args_06,arguments_06),
                          BSLS_COMPILERFEATURES_FORWARD(Args_07,arguments_07),
                          BSLS_COMPILERFEATURES_FORWARD(Args_08,arguments_08),
                          BSLS_COMPILERFEATURES_FORWARD(Args_09,arguments_09),
                          BSLS_COMPILERFEATURES_FORWARD(Args_10,arguments_10));
}

template <class TYPE,
          bool  PROPAGATE_ON_CONTAINER_COPY_CONSTRUCTION,
          bool  PROPAGATE_ON_CONTAINER_COPY_ASSIGNMENT,
          bool  PROPAGATE_ON_CONTAINER_SWAP,
          bool  PROPAGATE_ON_CONTAINER_MOVE_ASSIGNMENT>
template <class ELEMENT_TYPE, class Args_01,
                              class Args_02,
                              class Args_03,
                              class Args_04,
                              class Args_05,
                              class Args_06,
                              class Args_07,
                              class Args_08,
                              class Args_09,
                              class Args_10,
                              class Args_11>
inline
void
StdStatefulAllocator<TYPE,
                     PROPAGATE_ON_CONTAINER_COPY_CONSTRUCTION,
                     PROPAGATE_ON_CONTAINER_COPY_ASSIGNMENT,
                     PROPAGATE_ON_CONTAINER_SWAP,
                     PROPAGATE_ON_CONTAINER_MOVE_ASSIGNMENT>::construct(
                                    ELEMENT_TYPE *address,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) arguments_08,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_09) arguments_09,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_10) arguments_10,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_11) arguments_11)
{
    ::new (static_cast<void*>(address)) ELEMENT_TYPE(
                          BSLS_COMPILERFEATURES_FORWARD(Args_01,arguments_01),
                          BSLS_COMPILERFEATURES_FORWARD(Args_02,arguments_02),
                          BSLS_COMPILERFEATURES_FORWARD(Args_03,arguments_03),
                          BSLS_COMPILERFEATURES_FORWARD(Args_04,arguments_04),
                          BSLS_COMPILERFEATURES_FORWARD(Args_05,arguments_05),
                          BSLS_COMPILERFEATURES_FORWARD(Args_06,arguments_06),
                          BSLS_COMPILERFEATURES_FORWARD(Args_07,arguments_07),
                          BSLS_COMPILERFEATURES_FORWARD(Args_08,arguments_08),
                          BSLS_COMPILERFEATURES_FORWARD(Args_09,arguments_09),
                          BSLS_COMPILERFEATURES_FORWARD(Args_10,arguments_10),
                          BSLS_COMPILERFEATURES_FORWARD(Args_11,arguments_11));
}

template <class TYPE,
          bool  PROPAGATE_ON_CONTAINER_COPY_CONSTRUCTION,
          bool  PROPAGATE_ON_CONTAINER_COPY_ASSIGNMENT,
          bool  PROPAGATE_ON_CONTAINER_SWAP,
          bool  PROPAGATE_ON_CONTAINER_MOVE_ASSIGNMENT>
template <class ELEMENT_TYPE, class Args_01,
                              class Args_02,
                              class Args_03,
                              class Args_04,
                              class Args_05,
                              class Args_06,
                              class Args_07,
                              class Args_08,
                              class Args_09,
                              class Args_10,
                              class Args_11,
                              class Args_12>
inline
void
StdStatefulAllocator<TYPE,
                     PROPAGATE_ON_CONTAINER_COPY_CONSTRUCTION,
                     PROPAGATE_ON_CONTAINER_COPY_ASSIGNMENT,
                     PROPAGATE_ON_CONTAINER_SWAP,
                     PROPAGATE_ON_CONTAINER_MOVE_ASSIGNMENT>::construct(
                                    ELEMENT_TYPE *address,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) arguments_08,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_09) arguments_09,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_10) arguments_10,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_11) arguments_11,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_12) arguments_12)
{
    ::new (static_cast<void*>(address)) ELEMENT_TYPE(
                          BSLS_COMPILERFEATURES_FORWARD(Args_01,arguments_01),
                          BSLS_COMPILERFEATURES_FORWARD(Args_02,arguments_02),
                          BSLS_COMPILERFEATURES_FORWARD(Args_03,arguments_03),
                          BSLS_COMPILERFEATURES_FORWARD(Args_04,arguments_04),
                          BSLS_COMPILERFEATURES_FORWARD(Args_05,arguments_05),
                          BSLS_COMPILERFEATURES_FORWARD(Args_06,arguments_06),
                          BSLS_COMPILERFEATURES_FORWARD(Args_07,arguments_07),
                          BSLS_COMPILERFEATURES_FORWARD(Args_08,arguments_08),
                          BSLS_COMPILERFEATURES_FORWARD(Args_09,arguments_09),
                          BSLS_COMPILERFEATURES_FORWARD(Args_10,arguments_10),
                          BSLS_COMPILERFEATURES_FORWARD(Args_11,arguments_11),
                          BSLS_COMPILERFEATURES_FORWARD(Args_12,arguments_12));
}

template <class TYPE,
          bool  PROPAGATE_ON_CONTAINER_COPY_CONSTRUCTION,
          bool  PROPAGATE_ON_CONTAINER_COPY_ASSIGNMENT,
          bool  PROPAGATE_ON_CONTAINER_SWAP,
          bool  PROPAGATE_ON_CONTAINER_MOVE_ASSIGNMENT>
template <class ELEMENT_TYPE, class Args_01,
                              class Args_02,
                              class Args_03,
                              class Args_04,
                              class Args_05,
                              class Args_06,
                              class Args_07,
                              class Args_08,
                              class Args_09,
                              class Args_10,
                              class Args_11,
                              class Args_12,
                              class Args_13>
inline
void
StdStatefulAllocator<TYPE,
                     PROPAGATE_ON_CONTAINER_COPY_CONSTRUCTION,
                     PROPAGATE_ON_CONTAINER_COPY_ASSIGNMENT,
                     PROPAGATE_ON_CONTAINER_SWAP,
                     PROPAGATE_ON_CONTAINER_MOVE_ASSIGNMENT>::construct(
                                    ELEMENT_TYPE *address,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) arguments_08,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_09) arguments_09,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_10) arguments_10,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_11) arguments_11,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_12) arguments_12,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_13) arguments_13)
{
    ::new (static_cast<void*>(address)) ELEMENT_TYPE(
                          BSLS_COMPILERFEATURES_FORWARD(Args_01,arguments_01),
                          BSLS_COMPILERFEATURES_FORWARD(Args_02,arguments_02),
                          BSLS_COMPILERFEATURES_FORWARD(Args_03,arguments_03),
                          BSLS_COMPILERFEATURES_FORWARD(Args_04,arguments_04),
                          BSLS_COMPILERFEATURES_FORWARD(Args_05,arguments_05),
                          BSLS_COMPILERFEATURES_FORWARD(Args_06,arguments_06),
                          BSLS_COMPILERFEATURES_FORWARD(Args_07,arguments_07),
                          BSLS_COMPILERFEATURES_FORWARD(Args_08,arguments_08),
                          BSLS_COMPILERFEATURES_FORWARD(Args_09,arguments_09),
                          BSLS_COMPILERFEATURES_FORWARD(Args_10,arguments_10),
                          BSLS_COMPILERFEATURES_FORWARD(Args_11,arguments_11),
                          BSLS_COMPILERFEATURES_FORWARD(Args_12,arguments_12),
                          BSLS_COMPILERFEATURES_FORWARD(Args_13,arguments_13));
}

template <class TYPE,
          bool  PROPAGATE_ON_CONTAINER_COPY_CONSTRUCTION,
          bool  PROPAGATE_ON_CONTAINER_COPY_ASSIGNMENT,
          bool  PROPAGATE_ON_CONTAINER_SWAP,
          bool  PROPAGATE_ON_CONTAINER_MOVE_ASSIGNMENT>
template <class ELEMENT_TYPE, class Args_01,
                              class Args_02,
                              class Args_03,
                              class Args_04,
                              class Args_05,
                              class Args_06,
                              class Args_07,
                              class Args_08,
                              class Args_09,
                              class Args_10,
                              class Args_11,
                              class Args_12,
                              class Args_13,
                              class Args_14>
inline
void
StdStatefulAllocator<TYPE,
                     PROPAGATE_ON_CONTAINER_COPY_CONSTRUCTION,
                     PROPAGATE_ON_CONTAINER_COPY_ASSIGNMENT,
                     PROPAGATE_ON_CONTAINER_SWAP,
                     PROPAGATE_ON_CONTAINER_MOVE_ASSIGNMENT>::construct(
                                    ELEMENT_TYPE *address,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) arguments_08,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_09) arguments_09,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_10) arguments_10,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_11) arguments_11,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_12) arguments_12,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_13) arguments_13,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_14) arguments_14)
{
    ::new (static_cast<void*>(address)) ELEMENT_TYPE(
                          BSLS_COMPILERFEATURES_FORWARD(Args_01,arguments_01),
                          BSLS_COMPILERFEATURES_FORWARD(Args_02,arguments_02),
                          BSLS_COMPILERFEATURES_FORWARD(Args_03,arguments_03),
                          BSLS_COMPILERFEATURES_FORWARD(Args_04,arguments_04),
                          BSLS_COMPILERFEATURES_FORWARD(Args_05,arguments_05),
                          BSLS_COMPILERFEATURES_FORWARD(Args_06,arguments_06),
                          BSLS_COMPILERFEATURES_FORWARD(Args_07,arguments_07),
                          BSLS_COMPILERFEATURES_FORWARD(Args_08,arguments_08),
                          BSLS_COMPILERFEATURES_FORWARD(Args_09,arguments_09),
                          BSLS_COMPILERFEATURES_FORWARD(Args_10,arguments_10),
                          BSLS_COMPILERFEATURES_FORWARD(Args_11,arguments_11),
                          BSLS_COMPILERFEATURES_FORWARD(Args_12,arguments_12),
                          BSLS_COMPILERFEATURES_FORWARD(Args_13,arguments_13),
                          BSLS_COMPILERFEATURES_FORWARD(Args_14,arguments_14));
}

#else
// The generated code below is a workaround for the absence of perfect
// forwarding in some compilers.
template <class TYPE,
          bool  PROPAGATE_ON_CONTAINER_COPY_CONSTRUCTION,
          bool  PROPAGATE_ON_CONTAINER_COPY_ASSIGNMENT,
          bool  PROPAGATE_ON_CONTAINER_SWAP,
          bool  PROPAGATE_ON_CONTAINER_MOVE_ASSIGNMENT>
template <class ELEMENT_TYPE, class... Args>
inline
void
StdStatefulAllocator<TYPE,
                     PROPAGATE_ON_CONTAINER_COPY_CONSTRUCTION,
                     PROPAGATE_ON_CONTAINER_COPY_ASSIGNMENT,
                     PROPAGATE_ON_CONTAINER_SWAP,
                     PROPAGATE_ON_CONTAINER_MOVE_ASSIGNMENT>::construct(
                                    ELEMENT_TYPE *address,
                          BSLS_COMPILERFEATURES_FORWARD_REF(Args)... arguments)
{
    ::new (static_cast<void*>(address)) ELEMENT_TYPE(
                             BSLS_COMPILERFEATURES_FORWARD(Args,arguments)...);
}
// }}} END GENERATED CODE
#endif

template <class TYPE,
          bool  PROPAGATE_ON_CONTAINER_COPY_CONSTRUCTION,
          bool  PROPAGATE_ON_CONTAINER_COPY_ASSIGNMENT,
          bool  PROPAGATE_ON_CONTAINER_SWAP,
          bool  PROPAGATE_ON_CONTAINER_MOVE_ASSIGNMENT>
inline
void StdStatefulAllocator<TYPE,
                          PROPAGATE_ON_CONTAINER_COPY_CONSTRUCTION,
                          PROPAGATE_ON_CONTAINER_COPY_ASSIGNMENT,
                          PROPAGATE_ON_CONTAINER_SWAP,
                          PROPAGATE_ON_CONTAINER_MOVE_ASSIGNMENT>::
deallocate(TYPE *address, bslma::Allocator::size_type)
{
    d_allocator_p->deallocate(address);
}

template <class TYPE,
          bool  PROPAGATE_ON_CONTAINER_COPY_CONSTRUCTION,
          bool  PROPAGATE_ON_CONTAINER_COPY_ASSIGNMENT,
          bool  PROPAGATE_ON_CONTAINER_SWAP,
          bool  PROPAGATE_ON_CONTAINER_MOVE_ASSIGNMENT>
template <class ELEMENT_TYPE>
inline
void StdStatefulAllocator<TYPE,
                          PROPAGATE_ON_CONTAINER_COPY_CONSTRUCTION,
                          PROPAGATE_ON_CONTAINER_COPY_ASSIGNMENT,
                          PROPAGATE_ON_CONTAINER_SWAP,
                          PROPAGATE_ON_CONTAINER_MOVE_ASSIGNMENT>::destroy(
                                                        ELEMENT_TYPE * address)
{
    address->~ELEMENT_TYPE();
}

template <class TYPE,
          bool  PROPAGATE_ON_CONTAINER_COPY_CONSTRUCTION,
          bool  PROPAGATE_ON_CONTAINER_COPY_ASSIGNMENT,
          bool  PROPAGATE_ON_CONTAINER_SWAP,
          bool  PROPAGATE_ON_CONTAINER_MOVE_ASSIGNMENT>
inline
typename StdStatefulAllocator<
                             TYPE,
                             PROPAGATE_ON_CONTAINER_COPY_CONSTRUCTION,
                             PROPAGATE_ON_CONTAINER_COPY_ASSIGNMENT,
                             PROPAGATE_ON_CONTAINER_SWAP,
                             PROPAGATE_ON_CONTAINER_MOVE_ASSIGNMENT>::size_type
StdStatefulAllocator<TYPE,
                     PROPAGATE_ON_CONTAINER_COPY_CONSTRUCTION,
                     PROPAGATE_ON_CONTAINER_COPY_ASSIGNMENT,
                     PROPAGATE_ON_CONTAINER_SWAP,
                     PROPAGATE_ON_CONTAINER_MOVE_ASSIGNMENT>::
max_size() const
{
    // Return the largest value, 'v', such that 'v * sizeof(T)' fits in a
    // 'size_type' (copied from bslstl_allocator).

    // We will calculate MAX_NUM_BYTES based on our knowledge that
    // 'bslma::Allocator::size_type' is just an alias for 'std::size_t'.  First
    // demonstrate that is true:

    BSLMF_ASSERT((bsl::is_same<BloombergLP::bslma::Allocator::size_type,
                                                         std::size_t>::value));

    static const std::size_t MAX_NUM_BYTES    = ~std::size_t(0);
    static const std::size_t MAX_NUM_ELEMENTS =
                                     std::size_t(MAX_NUM_BYTES) / sizeof(TYPE);
    return MAX_NUM_ELEMENTS;
}

template <class TYPE,
          bool  PROPAGATE_ON_CONTAINER_COPY_CONSTRUCTION,
          bool  PROPAGATE_ON_CONTAINER_COPY_ASSIGNMENT,
          bool  PROPAGATE_ON_CONTAINER_SWAP,
          bool  PROPAGATE_ON_CONTAINER_MOVE_ASSIGNMENT>
inline
StdStatefulAllocator<TYPE,
                     PROPAGATE_ON_CONTAINER_COPY_CONSTRUCTION,
                     PROPAGATE_ON_CONTAINER_COPY_ASSIGNMENT,
                     PROPAGATE_ON_CONTAINER_SWAP,
                     PROPAGATE_ON_CONTAINER_MOVE_ASSIGNMENT>
StdStatefulAllocator<TYPE,
                     PROPAGATE_ON_CONTAINER_COPY_CONSTRUCTION,
                     PROPAGATE_ON_CONTAINER_COPY_ASSIGNMENT,
                     PROPAGATE_ON_CONTAINER_SWAP,
                     PROPAGATE_ON_CONTAINER_MOVE_ASSIGNMENT>::
select_on_container_copy_construction() const
{
    if (PROPAGATE_ON_CONTAINER_COPY_CONSTRUCTION) {
        return *this;                                                 // RETURN
    }

    // else

    return StdStatefulAllocator(bslma::Default::defaultAllocator());
}

template <class TYPE,
          bool  PROPAGATE_ON_CONTAINER_COPY_CONSTRUCTION,
          bool  PROPAGATE_ON_CONTAINER_COPY_ASSIGNMENT,
          bool  PROPAGATE_ON_CONTAINER_SWAP,
          bool  PROPAGATE_ON_CONTAINER_MOVE_ASSIGNMENT>
inline
bslma::Allocator *
StdStatefulAllocator<TYPE,
                     PROPAGATE_ON_CONTAINER_COPY_CONSTRUCTION,
                     PROPAGATE_ON_CONTAINER_COPY_ASSIGNMENT,
                     PROPAGATE_ON_CONTAINER_SWAP,
                     PROPAGATE_ON_CONTAINER_MOVE_ASSIGNMENT>::
allocator() const
{
    return d_allocator_p;
}

}  // close package namespace

// FREE OPERATORS
template <class TYPE,
          bool  PROPAGATE_ON_CONTAINER_COPY_CONSTRUCTION,
          bool  PROPAGATE_ON_CONTAINER_COPY_ASSIGNMENT,
          bool  PROPAGATE_ON_CONTAINER_SWAP,
          bool  PROPAGATE_ON_CONTAINER_MOVE_ASSIGNMENT>
inline
bool bsltf::operator==(const StdStatefulAllocator<
                                  TYPE,
                                  PROPAGATE_ON_CONTAINER_COPY_CONSTRUCTION,
                                  PROPAGATE_ON_CONTAINER_COPY_ASSIGNMENT,
                                  PROPAGATE_ON_CONTAINER_SWAP,
                                  PROPAGATE_ON_CONTAINER_MOVE_ASSIGNMENT>& lhs,
                       const StdStatefulAllocator<
                                  TYPE,
                                  PROPAGATE_ON_CONTAINER_COPY_CONSTRUCTION,
                                  PROPAGATE_ON_CONTAINER_COPY_ASSIGNMENT,
                                  PROPAGATE_ON_CONTAINER_SWAP,
                                  PROPAGATE_ON_CONTAINER_MOVE_ASSIGNMENT>& rhs)
{
    return lhs.allocator() == rhs.allocator();
}

template <class TYPE,
          bool  PROPAGATE_ON_CONTAINER_COPY_CONSTRUCTION,
          bool  PROPAGATE_ON_CONTAINER_COPY_ASSIGNMENT,
          bool  PROPAGATE_ON_CONTAINER_SWAP,
          bool  PROPAGATE_ON_CONTAINER_MOVE_ASSIGNMENT>
inline
bool bsltf::operator!=(const StdStatefulAllocator<
                                  TYPE,
                                  PROPAGATE_ON_CONTAINER_COPY_CONSTRUCTION,
                                  PROPAGATE_ON_CONTAINER_COPY_ASSIGNMENT,
                                  PROPAGATE_ON_CONTAINER_SWAP,
                                  PROPAGATE_ON_CONTAINER_MOVE_ASSIGNMENT>& lhs,
                       const StdStatefulAllocator<
                                  TYPE,
                                  PROPAGATE_ON_CONTAINER_COPY_CONSTRUCTION,
                                  PROPAGATE_ON_CONTAINER_COPY_ASSIGNMENT,
                                  PROPAGATE_ON_CONTAINER_SWAP,
                                  PROPAGATE_ON_CONTAINER_MOVE_ASSIGNMENT>& rhs)
{
    return lhs.allocator() != rhs.allocator();
}

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
