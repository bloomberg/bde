// bslalg_typetraits.h                                                -*-C++-*-
#ifndef INCLUDED_BSLALG_TYPETRAITS
#define INCLUDED_BSLALG_TYPETRAITS

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide compile-time traits that can be associated with types.
//
//@DEPRECATED: See 'bslmf_nestedtraitdeclaration'.
//
//@CLASSES:
//  bslalg::TypeTraitNil: nil type trait (no traits)
//  bslalg::TypeTraitBitwiseMoveable: bitwise-moveable trait
//  bslalg::TypeTraitBitwiseCopyable: bitwise-copyable trait
//  bslalg::TypeTraitBitwiseEqualityComparable: bitwise-eq.-comparable trait
//  bslalg::TypeTraitHasStlIterators: has STL-like iterators
//  bslalg::TypeTraitHasTrivialDefaultConstructor: has trivial default ctor
//  bslalg::TypeTraitPair: for 'std::pair'-like classes
//  bslalg::TypeTraitUsesBslmaAllocator: uses 'bslma' allocators
//
//@MACROS:
//  BSLALG_DECLARE_NESTED_TRAITS: declares a trait in a nested fashion
//  BSLALG_DECLARE_NESTED_TRAITS2: declares two traits in a nested fashion
//  BSLALG_DECLARE_NESTED_TRAITS3: declares three traits in a nested fashion
//  BSLALG_DECLARE_NESTED_TRAITS4: declares four traits in a nested fashion
//  BSLALG_DECLARE_NESTED_TRAITS5: declares five traits in a nested fashion
//
//@SEE_ALSO: bslalg_constructorproxy, bslalg_scalarprimitives
//
//@DESCRIPTION: This component provides a set of type traits, and a set of
// macros used to assign traits to user-defined class.  Traits are used to
// enable certain optimizations or discriminations at compile-time.  For
// instance, a class having the trait 'bslalg::TypeTraitBitwiseMoveable' may
// allow resizing an array of objects by simply calling 'std::memcpy' instead
// of invoking a copy-constructor on every objects.  The usage example shows
// how to use the 'bslalg::TypeTraitUsesBslmaAllocator' to propagate allocators
// to nested objects that may require them.
//
// This component should be used in conjunction with other components from the
// package 'bslalg'.  See the package-level documentation for an overview.  The
// most useful classes and macros defined in this component are:
//..
//  Entity                                    Description
//  ------                                    -----------
//  bslalg::TypeTraitBitwiseMoveable         (See the corresponding class-level
//  bslalg::TypeTraitBitwiseCopyable                            documentation.)
//  bslalg::TypeTraitBitwiseEqualityComparable
//  bslalg::TypeTraitHasStlIterators
//  bslalg::TypeTraitHasTrivialDefaultConstructor
//  bslalg::TypeTraitPair
//  bslalg::TypeTraitUsesBslmaAllocator
//
//  BSLALG_DECLARE_NESTED_TRAITS(TYPE, TRAIT) A macro to attach a given 'TRAIT'
//                                            to a given type 'T' in a nested
//                                            fashion.  This macro must be used
//                                            within the 'public' section of a
//                                            class body.
//
//  bslalg::HasTrait<TYPE, TRAIT>             This meta-function computes
//                                            whether the parameterized 'TYPE'
//                                            possesses the parameterized
//                                            'TRAIT'.
//
//  bslalg::SelectTrait<T, TRAIT1, ...>       This meta-function selects the
//                                            first trait possessed by the
//                                            parameterized 'TYPE' from the
//                                            ordered list 'TRAIT1', ....
//..
//
///Usage
///-----
// In this usage example, we show how to enable the 'bslma' allocator model for
// generic containers, by implementing simplified versions of the
// 'bslalg_constructorproxy' and 'bslalg_scalarprimitives' components.  The
// interested reader should refer to the documentation of those components.
//
///A Generic Container
///- - - - - - - - - -
// Suppose we want to implement a generic container of a parameterized 'TYPE',
// which may or may not follow the 'bslma' allocator model.  If it does, our
// container should pass an extra 'bslma::Allocator*' argument to copy
// construct a value; but if it does not, then passing this extra argument is
// going to generate a compile-time error.  It thus appears we need two
// implementations of our container.  This can be done more succinctly by
// encapsulating into the constructor some utilities which will, through a
// single interface, determine whether 'TYPE' has the trait
// 'bslalg::TypeTraitUsesBslmaAllocator' and copy-construct it accordingly.
//
// The container contains a single data member of the parameterized 'TYPE'.
// Since we are going to initialize this data member manually, we do not want
// it to be automatically constructed by the compiler.  For this reason, we
// encapsulate it in a 'bsls::ObjectBuffer'.
//..
//  // my_genericcontainer.hpp          -*-C++-*-
//
//  template <class TYPE>
//  class MyGenericContainer {
//      // This generic container type contains a single object, always
//      // initialized, which can be replaced and accessed.  This container
//      // always takes an allocator argument and thus follows the
//      // 'bslalg::TypeTraitUsesBslmaAllocator' protocol.
//
//      // PRIVATE DATA MEMBERS
//      bsls::ObjectBuffer<TYPE> d_object;
//..
// Since the container offers a uniform interface that always takes an extra
// allocator argument, regardless of whether 'TYPE' does or not, we can declare
// it to have the 'bslalg::TypeTraitUsesBslmaAllocator' trait:
//..
//  public:
//    // TRAITS
//    BSLALG_DECLARE_NESTED_TRAITS(MyGenericContainer,
//                                 bslalg::TypeTraitUsesBslmaAllocator);
//..
// For simplicity, we let the container contain only a single element, and
// require that an element always be initialized.
//..
//  // CREATORS
//  MyGenericContainer(const TYPE& object, bslma::Allocator *allocator = 0);
//      // Create an container containing the specified 'object', using the
//      // optionally specified 'allocator' to allocate memory.  If
//      // 'allocator' is 0, the currently installed allocator is used.
//
//  MyGenericContainer(const MyGenericContainer&  container,
//                     bslma::Allocator          *allocator = 0);
//      // Create an container containing the same object as the specified
//      // 'container', using the optionally specified 'allocator' to
//      // allocate memory.  If 'allocator' is 0, the currently installed
//      // allocator is used.
//
//  ~MyGenericContainer();
//      // Destroy this container.
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
//
///Using the Type Traits
///- - - - - - - - - - -
// The challenge in the implementation lays in using the traits of the
// contained 'TYPE' to determine whether to pass the allocator argument to its
// copy constructor.  We rely here on a property of templates that templates
// are not compiled (and thus will not generate compilation errors) until they
// are instantiated.  Hence, we can use two function templates, and let the
// overloading resolution (based on the nested traits) decide which to
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
//      static void copyConstruct(TYPE             *location,
//                                const TYPE&       value,
//                                bslma::Allocator *allocator,
//                                bslalg::TypeTraitUsesBslmaAllocator)
//          // Create a copy of the specified 'value' at the specified
//          // 'location', using the specified 'allocator' to allocate memory.
//      {
//          new (location) TYPE(value, allocator);
//      }
//..
// For types that don't use an allocator, we offer the following overload which
// will be selected if the type trait of 'TYPE' cannot be converted to
// 'bslalg::TypeTraitUsesBslmaAllocator'.  In that case, note that the type
// traits always inherits from 'bslalg::TypeTraitNil'.
//..
//  template <class TYPE>
//  static void copyConstruct(TYPE             *location,
//                            const TYPE&       value,
//                            bslma::Allocator *allocator,
//                            bslalg::TypeTraitNil)
//      // Create a copy of the specified 'value' at the specified
//      // 'location'.  Note that the specified 'allocator' is ignored.
//  {
//      new (location) TYPE(value);
//  }
//..
// And finally, this function will instantiate the type trait and pass it to
// the appropriately (compiler-)chosen overload:
//..
//      template <class TYPE>
//      static void copyConstruct(TYPE             *location,
//                                const TYPE&       value,
//                                bslma::Allocator *allocator)
//          // Create a copy of the specified 'value' at the specified
//          // 'location', optionally using the specified 'allocator' to supply
//          // memory if the parameterized 'TYPE' possesses the
//          // 'bslalg::TypeTraitUsesBslmaAllocator'.
//      {
//          copyConstruct(location, value, allocator,
//              typename bslmf::If<HasTrait<TYPE,
//                            bslalg::TypeTraitUsesBslmaAllocator>::VALUE,
//                        bslalg::TypeTraitUsesBslmaAllocator,
//                        bslalg::TypeTraitNil>::Type());
//      }
//
//  };
//..
//
///Generic Container Implementation
/// - - - - - - - - - - - - - - - -
// With these utilities, we can now implement 'MyGenericContainer'.
//..
//  // CREATORS
//  template <typename TYPE>
//  MyGenericContainer<TYPE>::MyGenericContainer(const TYPE&       object,
//                                               bslma::Allocator *allocator)
//  {
//      my_GenericContainerUtil::copyConstruct(&d_object.object(),
//                                             object,
//                                             allocator);
//  }
//
//  template <typename TYPE>
//  MyGenericContainer<TYPE>::MyGenericContainer(
//                                        const MyGenericContainer&  container,
//                                        bslma::Allocator          *allocator)
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
//
///Usage Verification
/// - - - - - - - - -
// We can check that our container actually forwards the correct allocator to
// its contained objects with a very simple test apparatus, consisting of two
// classes which have exactly the same signature and implementation except that
// one has the 'bslalg::TypeTraitUsesBslmaAllocator' trait and the other has
// not:
//..
//  bslma::Allocator *allocSlot;
//
//  struct MyTestTypeWithBslmaAllocatorTraits {
//      // Class with declared traits.  Calling copy constructor without an
//      // allocator will compile, but will not set 'allocSlot'.
//
//      // TRAITS
//      BSLALG_DECLARE_NESTED_TRAITS(MyTestTypeWithBslmaAllocatorTraits,
//                           BloombergLP::bslalg::TypeTraitUsesBslmaAllocator);
//
//      // CREATORS
//      MyTestTypeWithBslmaAllocatorTraits() {}
//
//      MyTestTypeWithBslmaAllocatorTraits(
//                       const MyTestTypeWithBslmaAllocatorTraits&,
//                       bslma::Allocator                           *allocator)
//      {
//          allocSlot = allocator;
//      }
//  };
//
//  struct MyTestTypeWithNoBslmaAllocatorTraits {
//      // Class with no declared traits.  Calling copy constructor without
//      // an allocator will not set the 'allocSlot', but passing it by mistake
//      // will set it.
//
//      // CREATORS
//      MyTestTypeWithNoBslmaAllocatorTraits() {}
//
//      MyTestTypeWithNoBslmaAllocatorTraits(
//                    const MyTestTypeWithNoBslmaAllocatorTraits &,
//                    bslma::Allocator                              *allocator)
//      {
//          allocSlot = allocator;
//      }
//  };
//..
// Our verification program simply instantiate several 'MyGenericContainer'
// templates with the two test types above, and checks that the allocator slot
// is as expected:
//..
//  int main()
//  {
//      bslma::TestAllocator ta0;
//      bslma::TestAllocator ta1;
//..
// With 'MyTestTypeWithNoBslmaAllocatorTraits', the slot should never be set.
//..
//  MyTestTypeWithNoBslmaAllocatorTraits x;
//
//  allocSlot = &ta0;
//  MyGenericContainer<MyTestTypeWithNoBslmaAllocatorTraits> x0(x);
//  assert(&ta0 == allocSlot);
//
//  allocSlot = &ta0;
//  MyGenericContainer<MyTestTypeWithNoBslmaAllocatorTraits> x1(x, &ta1);
//  assert(&ta0 == allocSlot);
//..
// With 'MyTestTypeWithBslmaAllocatorTraits', the slot should be set to the
// allocator argument, or to 0 if not specified:
//..
//      MyTestTypeWithBslmaAllocatorTraits y;
//
//      allocSlot = &ta0;
//      MyGenericContainer<MyTestTypeWithBslmaAllocatorTraits> y0(y);
//      assert(0 == allocSlot);
//
//      allocSlot = &ta0;
//      MyGenericContainer<MyTestTypeWithBslmaAllocatorTraits> y1(y, &ta1);
//      assert(&ta1 == allocSlot);
//
//      return 0;
//  }
//..

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLALG_HASTRAIT
#include <bslalg_hastrait.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITBITWISECOPYABLE
#include <bslalg_typetraitbitwisecopyable.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITBITWISEEQUALITYCOMPARABLE
#include <bslalg_typetraitbitwiseequalitycomparable.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITBITWISEMOVEABLE
#include <bslalg_typetraitbitwisemoveable.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITHASPOINTERSEMANTICS
#include <bslalg_typetraithaspointersemantics.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITHASSTLITERATORS
#include <bslalg_typetraithasstliterators.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITHASTRIVIALDEFAULTCONSTRUCTOR
#include <bslalg_typetraithastrivialdefaultconstructor.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITNIL
#include <bslalg_typetraitnil.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITPAIR
#include <bslalg_typetraitpair.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITUSESBSLMAALLOCATOR
#include <bslalg_typetraitusesbslmaallocator.h>
#endif

// All headers below here are not really needed, but are kept here in case
// legacy code is depending on a transitive include.
#ifndef INCLUDED_BSLMA_ALLOCATOR
#include <bslma_allocator.h>
#endif

#ifndef INCLUDED_BSLMF_IF
#include <bslmf_if.h>
#endif

#ifndef INCLUDED_BSLMF_ISCONVERTIBLE
#include <bslmf_isconvertible.h>
#endif

#ifndef INCLUDED_BSLMF_ISENUM
#include <bslmf_isenum.h>
#endif

#ifndef INCLUDED_BSLMF_ISFUNDAMENTAL
#include <bslmf_isfundamental.h>
#endif

#ifndef INCLUDED_BSLMF_ISPOINTER
#include <bslmf_ispointer.h>
#endif

#ifndef INCLUDED_BSLMF_ISPOINTERTOMEMBER
#include <bslmf_ispointertomember.h>
#endif

#ifndef INCLUDED_BSLMF_METAINT
#include <bslmf_metaint.h>
#endif

#ifndef INCLUDED_BSLMF_NESTEDTRAITDECLARATION
#include <bslmf_nestedtraitdeclaration.h>
#endif

#ifndef INCLUDED_BSLMF_NIL
#include <bslmf_nil.h>
#endif

#ifndef INCLUDED_BSLMF_REMOVECVQ
#include <bslmf_removecvq.h>
#endif

namespace BloombergLP {

                    // ========================================
                    // macros BSLALG_DECLARE_NESTED_TRAITS[1-5]
                    // ========================================

#define BSLALG_DECLARE_NESTED_TRAITS(T, TRAIT)            \
    operator TRAIT::NestedTraitDeclaration<T>() const {   \
        return TRAIT::NestedTraitDeclaration<T>();        \
    }
    // Associate the specified 'TRAIT' tag with the specified 'T' class.  This
    // macro must be invoked only within the public part of the definition of
    // class 'T'.  'TRAIT' must name a class such that
    // 'TRAIT::NestedTraitDeclaration<T>' designates a class derived from
    // 'bslmf::DetectNestedTrait'.

#define BSLALG_DECLARE_NESTED_TRAITS2(T, TRAIT1, TRAIT2)                      \
    BSLALG_DECLARE_NESTED_TRAITS(T, TRAIT1);                                  \
    BSLALG_DECLARE_NESTED_TRAITS(T, TRAIT2)
    // Like 'BSLALG_DECLARE_NESTED_TRAITS', but for two traits.

#define BSLALG_DECLARE_NESTED_TRAITS3(T, TRAIT1, TRAIT2, TRAIT3)              \
    BSLALG_DECLARE_NESTED_TRAITS(T, TRAIT1);                                  \
    BSLALG_DECLARE_NESTED_TRAITS(T, TRAIT2);                                  \
    BSLALG_DECLARE_NESTED_TRAITS(T, TRAIT3)
    // Like 'BSLALG_DECLARE_NESTED_TRAITS', but for three traits.

#define BSLALG_DECLARE_NESTED_TRAITS4(T, TRAIT1, TRAIT2, TRAIT3, TRAIT4)      \
    BSLALG_DECLARE_NESTED_TRAITS(T, TRAIT1);                                  \
    BSLALG_DECLARE_NESTED_TRAITS(T, TRAIT2);                                  \
    BSLALG_DECLARE_NESTED_TRAITS(T, TRAIT3);                                  \
    BSLALG_DECLARE_NESTED_TRAITS(T, TRAIT4)
    // Like 'BSLALG_DECLARE_NESTED_TRAITS', but for four traits.

#define BSLALG_DECLARE_NESTED_TRAITS5(T, TRAIT1,TRAIT2,TRAIT3,TRAIT4,TRAIT5)  \
    BSLALG_DECLARE_NESTED_TRAITS(T, TRAIT1);                                  \
    BSLALG_DECLARE_NESTED_TRAITS(T, TRAIT2);                                  \
    BSLALG_DECLARE_NESTED_TRAITS(T, TRAIT3);                                  \
    BSLALG_DECLARE_NESTED_TRAITS(T, TRAIT4);                                  \
    BSLALG_DECLARE_NESTED_TRAITS(T, TRAIT5)
    // Like 'BSLALG_DECLARE_NESTED_TRAITS', but for five traits.


}  // close enterprise namespace

#endif // INCLUDED_BSLALG_TYPETRAITS

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
