// bdealg_typetraits.h   -*-C++-*-
#ifndef INCLUDED_BDEALG_TYPETRAITS
#define INCLUDED_BDEALG_TYPETRAITS

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide facilities for associating types with compile-time traits.
//
//@DEPRECATED: Use 'bslalg_typetraits' instead.
//
//@CLASSES:
//                           bdealg_TypeTraitNil: nil type trait (no traits)
//               bdealg_TypeTraitBitwiseMoveable: bitwise-moveable trait
//               bdealg_TypeTraitBitwiseCopyable: bitwise-copyable trait
//     bdealg_TypeTraitBitwiseEqualityComparable: bitwise-eq.-comparable trait
//               bdealg_TypeTraitHasStlIterators: has STL-like iterators
//  bdealg_TypeTraitHasTrivialDefaultConstructor: has trivial default ctor
//                          bdealg_TypeTraitPair: for 'bsl::pair'-like classes
//            bdealg_TypeTraitUsesBdemaAllocator: uses 'bdema' allocators
//                     bdealg_TypeTraitsGroupPod: POD trait
//             bdealg_TypeTraitsGroupStlSequence: for STL sequence containers
//              bdealg_TypeTraitsGroupStlOrdered: for STL ordered containers
//            bdealg_TypeTraitsGroupStlUnordered: for STL unordered containers
//
//                             bdealg_TypeTraits: default trait computation
//
//@MACROS:
//   BDEALG_DECLARE_NESTED_TRAITS:  declares a trait in a nested fashion
//  BDEALG_DECLARE_NESTED_TRAITS2:  declares two traits in a nested fashion
//  BDEALG_DECLARE_NESTED_TRAITS3:  declares three traits in a nested fashion
//  BDEALG_DECLARE_NESTED_TRAITS4:  declares four traits in a nested fashion
//  BDEALG_DECLARE_NESTED_TRAITS5:  declares five traits in a nested fashion
//           BDEALG_IMPLIES_TRAIT:  computes a trait by introspection
//     BDEALG_CHECK_IMPLIED_TRAIT:  detects a trait by introspection
//
//@SEE_ALSO: bslmf_typetraits, bdealg_constructorproxy, bdealg_scalarprimitives
//
//@AUTHOR: Pablo Halpern (phalpern), Herve Bronnimann (hbronnim)
//
//@DESCRIPTION:  This component provides a set of type traits, and a set of
// macros used to assign traits to user-defined class.  Traits are used to
// enable certain optimizations or discriminations at compile-time.  For
// instance, a class having the trait 'bdealg_TypeTraitBitwiseMoveable' may
// allow resizing an array of instances by simply calling 'bsl::memcpy' instead
// of invoking a copy-constructor on every instance.  The usage example shows
// how to use the 'bdealg_TypeTraitUsesBdemaAllocator' to propagate allocators
// to nested instances that may require them.
//
// This component should be used in conjunction with other components from
// the package 'bdealg'.  See the package-level documentation for an overview.
// The most useful classes and macros defined in this component are:
//..
//  Entity                                    Description
//  ------                                    -----------
//  bdealg_TypeTraitNil                       The nil trait.  Types with no
//                                            other traits have the nil trait.
//
//  bdealg_TypeTraits<T>                      Traits for type 'T'.  The general
//                                            definition computes the nested
//                                            and implied traits.  A user can
//                                            specialize it to a combination of
//                                            one or more of the following:
//
//  bdealg_TypeTraitBitwiseMoveable          (See the corresponding class-level
//  bdealg_TypeTraitBitwiseCopyable                             documentation.)
//  bdealg_TypeTraitBitwiseEqualityComparable
//  bdealg_TypeTraitHasStlIterators
//  bdealg_TypeTraitHasTrivialDefaultConstructor
//  bdealg_TypeTraitPair
//  bdealg_TypeTraitUsesBdemaAllocator
//
//  BDEALG_DECLARE_NESTED_TRAITS(TYPE, TRAIT) A macro to attach a given 'TRAIT'
//                                            to a given type 'T' in a nested
//                                            fashion.  This macro must be used
//                                            within the 'public' section of a
//                                            class body.
//
//  bdealg_HasTrait<TYPE, TRAIT>              This meta-function computes
//                                            whether the parameterized 'TYPE'
//                                            possesses the parameterized
//                                            'TRAIT'.
//
//  bdealg_PassthroughTrait<TYPE, TRAIT>      This macro selects the
//                                            parameterized 'TRAIT' if the
//                                            parameterized 'TYPE' possesses
//                                            it, and a distinct and otherwise
//                                            unused trait otherwise.
//
//  bdealg_SelectTrait<T, TRAIT1, ...>        This meta-function selects the
//                                            first trait possessed by the
//                                            parameterized 'TYPE' from the
//                                            ordered list 'TRAIT1', ....
//..
//
///Usage
///-----
// In this usage example, we show how to enable the 'bdema' allocator model for
// generic containers, by implementing simplified versions of the
// 'bdealg_constructorproxy' and 'bdealg_scalarprimitives' components.  The
// interested reader should refer to the documentation of those components.
//
///A generic container
///- - - - - - - - - -
// Suppose we want to implement a generic container of a parameterized 'TYPE',
// which may or may not follow the 'bdema' allocator model.  If it does, our
// container should pass an extra 'bdema_Allocator*' argument to copy construct
// a value; but if it does not, then passing this extra argument is going to
// generate a compile-time error.  It thus appears we need two implementations
// of our container.  This can be done more succinctly by encapsulating into
// the constructor some utilities which will, through a single interface,
// determine whether 'TYPE' has the trait 'bdealg_TypeTraitUsesBdemaAllocator'
// and copy-construct it accordingly.
//
// The container contains a single data member of the parameterized 'TYPE'.
// Since we are going to initialize this data member manually, we do not want
// it to be automatically constructed by the compiler.  For this reason, we
// encapsulate it in a 'bdes_ObjectBuffer'.
//..
//  // my_genericcontainer.hpp          -*-C++-*-
//
//  template <class TYPE>
//  class MyGenericContainer {
//      // This generic container type contains a single object, always
//      // initialized, which can be replaced and accessed.  This container
//      // always takes an allocator argument and thus follows the
//      // 'bdealg_TypeTraitUsesBdemaAllocator' protocol.
//
//      // PRIVATE DATA MEMBERS
//      bdes_ObjectBuffer<TYPE> d_object;
//..
// Since the container offers a uniform interface that always takes an extra
// allocator argument, regardless of whether 'TYPE' does or not, we can declare
// it to have the 'bdealg_TypeTraitUsesBdemaAllocator' trait:
//..
//    public:
//      // TRAITS
//      BDEALG_DECLARE_NESTED_TRAITS(MyGenericContainer,
//                                   bdealg_TypeTraitUsesBdemaAllocator);
//..
// For simplicity, we let the container contain only a single element, and
// require that an element always be initialized.
//..
//      // CREATORS
//      MyGenericContainer(const TYPE& object, bdema_Allocator *allocator = 0);
//          // Create an container containing the specified 'object', using the
//          // optionally specified 'allocator' to allocate memory.  If
//          // 'allocator' is 0, the currently installed allocator is used.
//
//      MyGenericContainer(const MyGenericContainer&  container,
//                         bdema_Allocator           *allocator = 0);
//          // Create an container containing the same object as the specified
//          // 'container', using the optionally specified 'allocator' to
//          // allocate memory.  If 'allocator' is 0, the currently installed
//          // allocator is used.
//
//      ~MyGenericContainer();
//          // Destroy this container.
//..
// We can also allow the container to change the object it contains, by
// granting modifiable as well as non-modifiable access to this object:
//..
//      // MANIPULATORS
//      TYPE& object();
//
//      // ACCESSORS
//      const TYPE& object() const;
//  };
//..
///Using the type traits
///- - - - - - - - - - -
// The challenge in the implementation lays in using the traits of the
// contained 'TYPE' to determine whether to pass the allocator argument to its
// copy constructor.  We rely here on a property of templates that
// templates are not compiled (and thus will not generate compilation errors)
// until they are instantiated.  Hence, we can use two function templates, and
// let the overloading resolution (based on the nested traits) decide which to
// instantiate.  The generic way to create an object, passing through all
// arguments (value and allocator) is as follows.  For brevity and to avoid
// breaking the flow of this example, we have embedded the function definition
// into the class.
//..
//  // my_genericcontainer.cpp          -*-C++-*-
//
//  struct my_GenericContainerUtil {
//      // This 'struct' provides a namespace for utilities implementing the
//      // allocator pass-through mechanism in a generic container.
//
//      template <class TYPE>
//      static void copyConstruct(TYPE            *location,
//                                const TYPE&      value,
//                                bdema_Allocator *allocator,
//                                bdealg_TypeTraitUsesBdemaAllocator)
//          // Create a copy of the specified 'value' at the specified
//          // 'location', using the specified 'allocator' to allocate memory.
//      {
//          new (location) TYPE(value, allocator);
//      }
//..
// For types that don't use an allocator, we offer the following overload which
// will be selected if the type trait of 'TYPE' cannot be converted to
// 'bdealg_TypeTraitUsesBdemaAllocator'.  In that case, note that the type
// traits always inherits from 'bdealg_TypeTraitNil'.
//..
//      template <class TYPE>
//      static void copyConstruct(TYPE            *location,
//                                const TYPE&      value,
//                                bdema_Allocator *allocator,
//                                bdealg_TypeTraitNil)
//          // Create a copy of the specified 'value' at the specified
//          // 'location'.  Note that the specified 'allocator' is ignored.
//      {
//          new (location) TYPE(value);
//      }
//..
// And finally, this function will instantiate the type trait and pass it to
// the appropriately (compiler-)chosen overload:
//..
//      template <class TYPE>
//      static void copyConstruct(TYPE            *location,
//                                const TYPE&      value,
//                                bdema_Allocator *allocator)
//          // Create a copy of the specified 'value' at the specified
//          // 'location', optionally using the specified 'allocator' to supply
//          // memory if the parameterized 'TYPE' possesses the
//          // 'bdealg_TypeTraitUsesBdemaAllocator'.
//      {
//          copyConstruct(location, value, allocator,
//                        bdealg_TypeTraits<TYPE>());
//      }
//
//  };
//..
///Generic container implementation
///- - - - - - - - - - - - - - - -
// With these utilities, we can now implement 'MyGenericContainer'.
//..
//  // CREATORS
//  template <typename TYPE>
//  MyGenericContainer<TYPE>::MyGenericContainer(const TYPE&      object,
//                                               bdema_Allocator *allocator)
//  {
//      my_GenericContainerUtil::copyConstruct(&d_object.object(),
//                                             object,
//                                             allocator);
//  }
//
//  template <typename TYPE>
//  MyGenericContainer<TYPE>::MyGenericContainer(
//                                        const MyGenericContainer&  container,
//                                        bdema_Allocator           *allocator)
//  {
//      my_GenericContainerUtil::copyConstruct(&d_object.object(),
//                                             container.object(),
//                                             allocator);
//  }
//..
// Note that all this machinery only affects the constructors, and not the
// destructor which only invokes the destructor of 'd_object'.
//..
//  template <typename TYPE>
//  MyGenericContainer<TYPE>::~MyGenericContainer()
//  {
//      (&d_object.object())->~TYPE();
//  }
//..
// To finish, the accessors and manipulators are trivially implemented.
//..
//  // MANIPULATORS
//  template <typename TYPE>
//  TYPE& MyGenericContainer<TYPE>::object()
//  {
//      return d_object.object();
//  }
//
//  // ACCESSORS
//  template <typename TYPE>
//  const TYPE& MyGenericContainer<TYPE>::object() const
//  {
//      return d_object.object();
//  }
//..
///Usage verification
///- - - - - - - - -
// We can check that our container actually forwards the correct allocator to
// its contained instance with a very simple test apparatus, consisting of two
// classes which have exactly the same signature and implementation except that
// one has the 'bdealg_TypeTraitUsesBdemaAllocator' trait and the other has
// not:
//..
//  bdema_Allocator *allocSlot;
//
//  struct MyTestTypeWithBdemaAllocatorTraits {
//      // Class with declared traits.  Calling copy constructor without an
//      // allocator will compile, but will not set 'allocSlot'.
//
//      // TRAITS
//      BDEALG_DECLARE_NESTED_TRAITS(MyTestTypeWithBdemaAllocatorTraits,
//                            BloombergLP::bdealg_TypeTraitUsesBdemaAllocator);
//
//      // CREATORS
//      MyTestTypeWithBdemaAllocatorTraits() {}
//
//      MyTestTypeWithBdemaAllocatorTraits(
//                       const MyTestTypeWithBdemaAllocatorTraits&,
//                       bdema_Allocator                            *allocator)
//      {
//          allocSlot = allocator;
//      }
//  };
//
//  struct MyTestTypeWithNoBdemaAllocatorTraits {
//      // Class with no declared traits.  Calling copy constructor without
//      // an allocator will not set the 'allocSlot', but passing it by mistake
//      // will set it.
//
//      // CREATORS
//      MyTestTypeWithNoBdemaAllocatorTraits() {}
//
//      MyTestTypeWithNoBdemaAllocatorTraits(
//                    const MyTestTypeWithNoBdemaAllocatorTraits &,
//                    bdema_Allocator                               *allocator)
//      {
//          allocSlot = allocator;
//      }
//  };
//..
// Our verification program simply instantiate several 'MyGenericContainer'
// templates with the two test types above, and checks that the allocator
// slot is as expected:
//..
//  int main()
//  {
//      bdema_TestAllocator ta0;
//      bdema_TestAllocator ta1;
//..
// With 'MyTestTypeWithNoBdemaAllocatorTraits', the slot should never be set.
//..
//      MyTestTypeWithNoBdemaAllocatorTraits x;
//
//      allocSlot = &ta0;
//      MyGenericContainer<MyTestTypeWithNoBdemaAllocatorTraits> x0(x);
//      ASSERT(&ta0 == allocSlot);
//
//      allocSlot = &ta0;
//      MyGenericContainer<MyTestTypeWithNoBdemaAllocatorTraits> x1(x, &ta1);
//      ASSERT(&ta0 == allocSlot);
//..
// With 'MyTestTypeWithBdemaAllocatorTraits', the slot should be set to the
// allocator argument, or to 0 if not specified:
//..
//      MyTestTypeWithBdemaAllocatorTraits y;
//
//      allocSlot = &ta0;
//      MyGenericContainer<MyTestTypeWithBdemaAllocatorTraits> y0(y);
//      ASSERT(0 == allocSlot);
//
//      allocSlot = &ta0;
//      MyGenericContainer<MyTestTypeWithBdemaAllocatorTraits> y1(y, &ta1);
//      ASSERT(&ta1 == allocSlot);
//
//      return 0;
//  }
//..

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BSLALG_HASTRAIT
#include <bslalg_hastrait.h>
#endif

#ifndef INCLUDED_BSLALG_PASSTHROUGHTRAIT
#include <bslalg_passthroughtrait.h>
#endif

#ifndef INCLUDED_BSLALG_SELECTTRAIT
#include <bslalg_selecttrait.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITS
#include <bslalg_typetraits.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITSGROUPPOD
#include <bslalg_typetraitsgrouppod.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITSGROUPSTLSEQUENCE
#include <bslalg_typetraitsgroupstlsequence.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITSGROUPSTLORDERED
#include <bslalg_typetraitsgroupstlordered.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITSGROUPSTLUNORDERED
#include <bslalg_typetraitsgroupstlunordered.h>
#endif

// The following were formerly guarded by 'BDE_DONT_ALLOW_TRANSITIVE_INCLUDES'.
// However, it was deemed that one of the purposes of this header file is to
// include these, and although 'bdealg' is DEPRECATED, unconditionally
// providing these inclusions is the correct thing to do.

#ifndef INCLUDED_BDEALG_TYPETRAITHASPOINTERSEMANTICS
#include <bdealg_typetraithaspointersemantics.h>
#endif

#ifndef INCLUDED_BDEALG_TYPETRAITHASTRIVIALDEFAULTCONSTRUCTOR
#include <bdealg_typetraithastrivialdefaultconstructor.h>
#endif

#ifndef INCLUDED_BDEALG_TYPETRAITNIL
#include <bdealg_typetraitnil.h>
#endif

#ifndef INCLUDED_BDEALG_TYPETRAITUSESBDEMAALLOCATOR
#include <bdealg_typetraitusesbdemaallocator.h>
#endif

#ifndef BDE_DONT_ALLOW_TRANSITIVE_INCLUDES

#ifndef INCLUDED_BDEMA_ALLOCATOR
#include <bdema_allocator.h>
#endif

#ifndef INCLUDED_BDES_ALIGNMENT
#include <bdes_alignment.h>
#endif

#ifndef INCLUDED_BDES_PLATFORM
#include <bdes_platform.h>
#endif

#endif

namespace BloombergLP {

    // No symbols are defined here (see 'bslalg_typetraits' component).
    // No aliases or macros for the 'bdealg' types or macros are defined here,
    // instead they are defined in 'bslalg' so that clients that rely on these
    // via transitive includes may still have those aliases or macros.

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2008
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
