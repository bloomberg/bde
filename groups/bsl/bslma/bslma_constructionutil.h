// bslma_constructionutil.h                                           -*-C++-*-
#ifndef INCLUDED_BSLMA_CONSTRUCTIONUTIL
#define INCLUDED_BSLMA_CONSTRUCTIONUTIL

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide methods to construct arbitrarily-typed objects uniformly.
//
//@CLASSES:
//  bslma::ConstructionUtil: namespace for methods to construct objects
//
//@SEE_ALSO: bslma_allocatortraits, bslma_destructionutil
//
//@DESCRIPTION: This component provides a 'struct', 'bslma::ConstructionUtil',
// that serves as a namespace for utility functions to construct objects of an
// arbitrary (template parameter) type, given an arbitrary number of arguments.
// These functions are useful in implementing 'allocator_trait' classes that,
// in turn, are used in implementing generic components such as containers.
// How a type is constructed may depend on several type traits.  The traits
// under consideration by this component are:
//..
//  Trait                                         Description
//  --------------------------------------------  -----------------------------
//  bsl::is_trivially_default_constructible       "TYPE has the trivial default
//                                                constructor trait", or
//                                                "TYPE has a trivial default
//                                                constructor"
//
//  bslma::UsesBslmaAllocator                     "the 'TYPE' constructor takes
//                                                an allocator argument", or
//                                                "'TYPE' supports 'bslma'-
//                                                style allocators"
//
//  bslmf::UsesAllocatorArgT                      "the 'TYPE' constructor takes
//                                                an allocator argument", and
//                                                optionally passes allocators
//                                                as the first two arguments to
//                                                each constructor, where the
//                                                tag type 'allocator_arg_t' is
//                                                first, and the allocator type
//                                                is second
//
//  bsl::is_trivially_copyable                    "TYPE has the bitwise
//                                                copyable trait", or
//                                                "TYPE is bitwise copyable"
//                                                (implies that it has a
//                                                trivial destructor too)
//
//  bslmf::IsBitwiseMoveable                      "TYPE has the bitwise
//                                                movable trait", or
//                                                "TYPE is bitwise movable"
//..
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Using 'bslma::ConstructionUtil' to Implement a Container
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// This example demonstrates the intended use of 'bslma::ConstructionUtil' to
// implement a simple container class that uses the 'bslma::Allocator' protocol
// for memory management.
//
// First, because allocation and construction are done in two separate steps,
// we need to define a proctor type that will deallocate the allocated memory
// in case the constructor throws an exception:
//..
//  template <class TYPE>
//  class MyContainerProctor {
//      // This class implements a proctor to release memory allocated during
//      // the construction of a 'MyContainer' object if the constructor for
//      // the container's data element throws an exception.  Such a proctor
//      // should be 'release'd once the element is safely constructed.
//
//      // DATA
//      bslma::Allocator *d_allocator_p;
//      TYPE             *d_address_p;    // proctored memory
//
//    private:
//      // NOT IMPLEMENTED
//      MyContainerProctor(const MyContainerProctor&);             // = delete
//      MyContainerProctor& operator=(const MyContainerProctor&);  // = delete
//
//    public:
//      // CREATORS
//      MyContainerProctor(bslma::Allocator *allocator, TYPE *address)
//          // Create a proctor that conditionally manages the memory at the
//          // specified 'address', and that uses the specified 'allocator' to
//          // deallocate the block of memory (if not released -- see
//          // 'release') upon destruction.  The behavior is undefined unless
//          // 'allocator' is non-zero and supplied the memory at 'address'.
//      : d_allocator_p(allocator)
//      , d_address_p(address)
//      {
//      }
//
//      ~MyContainerProctor()
//          // Destroy this proctor, and deallocate the block of memory it
//          // manages (if any) by invoking the 'deallocate' method of the
//          // allocator that was supplied at construction of this proctor.  If
//          // no memory is currently being managed, this method has no effect.
//      {
//          if (d_address_p) {
//              d_allocator_p->deallocate(d_address_p);
//          }
//      }
//
//      // MANIPULATORS
//      void release()
//          // Release from management the block of memory currently managed by
//          // this proctor.  If no memory is currently being managed, this
//          // method has no effect.
//      {
//          d_address_p = 0;
//      }
//  };
//..
// Then, we create a container class that holds a single element and uses
// 'bslma' allocators:
//..
//  #include <bslma_constructionutil.h>
//
//  template <class TYPE>
//  class MyContainer {
//      // This class provides a container that always holds exactly one
//      // element, dynamically allocated using the specified 'bslma'
//      // allocator.
//
//      // DATA
//      TYPE             *d_value_p;
//      bslma::Allocator *d_allocator_p;
//
//    public:
//      // TRAITS
//      BSLMF_NESTED_TRAIT_DECLARATION(MyContainer, bslma::UsesBslmaAllocator);
//
//      // CREATORS
//      MyContainer(bslma::Allocator *basicAllocator = 0);
//          // Create a container with a default-constructed element.
//          // Optionally specify a 'basicAllocator' used to supply memory.  If
//          // 'basicAllocator' is 0, the currently installed default allocator
//          // is used.
//
//      template <class OTHER>
//      explicit MyContainer(
//          BSLS_COMPILERFEATURES_FORWARD_REF(OTHER) value,
//          typename bsl::enable_if<bsl::is_convertible<OTHER, TYPE>::value,
//                                  void *>::type * = 0)
//          // Create a container with an element constructed by (perfectly)
//          // forwarding the specified 'value' and that uses the currently
//          // installed default allocator to supply memory.  Note that this
//          // constructor participates in overload resolution only if 'OTHER'
//          // is implicitly convertible to 'TYPE'.
//      : d_allocator_p(bslma::Default::defaultAllocator())
//      {
//          d_value_p =
//              static_cast<TYPE *>(d_allocator_p->allocate(sizeof(TYPE)));
//
//          MyContainerProctor<TYPE> proctor(d_allocator_p, d_value_p);
//
//          // Call 'construct' by forwarding 'value'.
//
//          bslma::ConstructionUtil::construct(
//              d_value_p,
//              d_allocator_p,
//              BSLS_COMPILERFEATURES_FORWARD(OTHER, value));
//          proctor.release();
//      }
//
//      template <class OTHER>
//      explicit MyContainer(
//          BSLS_COMPILERFEATURES_FORWARD_REF(OTHER)  value,
//          bslma::Allocator                         *basicAllocator);
//          // Create a container with an element constructed by (perfectly)
//          // forwarding the specified 'value' and that uses the specified
//          // 'basicAllocator' to supply memory.  If 'basicAllocator' is 0,
//          // the currently installed default allocator is used.  Note that
//          // this constructor participates in overload resolution only if
//          // 'OTHER' is implicitly convertible to 'TYPE'.
//
//      MyContainer(const MyContainer&  original,
//                  bslma::Allocator   *basicAllocator = 0);
//          // Create a container having the same value as the specified
//          // 'original' object.  Optionally specify a 'basicAllocator' used
//          // to supply memory.  If 'basicAllocator' is 0, the currently
//          // installed default allocator is used.
//
//      ~MyContainer();
//          // Destroy this object.
//
//      // MANIPULATORS
//      TYPE& front()
//          // Return a non-'const' reference to the element contained in this
//          // object.
//      {
//          return *d_value_p;
//      }
//
//      // ACCESSORS
//      const TYPE& front() const
//          // Return a 'const' reference to the element contained in this
//          // object.
//      {
//          return *d_value_p;
//      }
//
//      bslma::Allocator *allocator() const
//          // Return the allocator used by this object to supply memory.
//      {
//          return d_allocator_p;
//      }
//
//      // etc.
//  };
//..
// Next, we implement the constructors that allocate memory and construct a
// 'TYPE' object in the allocated memory.  We perform the allocation using the
// 'allocate' method of 'bslma::Allocator' and the construction using the
// 'construct' method of 'ConstructionUtil' that provides the correct semantics
// for passing the allocator to the constructed object when appropriate:
//..
//  template <class TYPE>
//  MyContainer<TYPE>::MyContainer(bslma::Allocator *basicAllocator)
//  : d_allocator_p(bslma::Default::allocator(basicAllocator))
//  {
//      d_value_p = static_cast<TYPE *>(d_allocator_p->allocate(sizeof(TYPE)));
//      MyContainerProctor<TYPE> proctor(d_allocator_p, d_value_p);
//
//      // Call 'construct' with no constructor arguments (aside from the
//      // allocator).
//
//      bslma::ConstructionUtil::construct(d_value_p, d_allocator_p);
//      proctor.release();
//  }
//
//  template <class TYPE>
//  template <class OTHER>
//  MyContainer<TYPE>::MyContainer(
//                    BSLS_COMPILERFEATURES_FORWARD_REF(OTHER)  value,
//                    bslma::Allocator                         *basicAllocator)
//  : d_allocator_p(bslma::Default::allocator(basicAllocator))
//  {
//      d_value_p = static_cast<TYPE *>(d_allocator_p->allocate(sizeof(TYPE)));
//      MyContainerProctor<TYPE> proctor(d_allocator_p, d_value_p);
//
//      // Call 'construct' by forwarding 'value'.
//
//      bslma::ConstructionUtil::construct(
//          d_value_p,
//          d_allocator_p,
//          BSLS_COMPILERFEATURES_FORWARD(OTHER, value));
//      proctor.release();
//  }
//..
// Then, we define the copy constructor for 'MyContainer'.  Note that we don't
// propagate the allocator from the 'original' container, but use
// 'basicAllocator' instead:
//..
//  template <class TYPE>
//  MyContainer<TYPE>::MyContainer(const MyContainer&  original,
//                                 bslma::Allocator   *basicAllocator)
//  : d_allocator_p(bslma::Default::allocator(basicAllocator))
//  {
//      d_value_p = static_cast<TYPE *>(d_allocator_p->allocate(sizeof(TYPE)));
//      MyContainerProctor<TYPE> proctor(d_allocator_p, d_value_p);
//
//      // Call 'construct' so as to copy-construct the element contained by
//      // 'original'.
//
//      bslma::ConstructionUtil::construct(d_value_p,
//                                         d_allocator_p,
//                                         *original.d_value_p);
//      proctor.release();
//  }
//..
// Now, the destructor destroys the object and deallocates the memory used to
// hold the element using the allocator:
//..
//  template <class TYPE>
//  MyContainer<TYPE>::~MyContainer()
//  {
//      d_value_p->~TYPE();
//      d_allocator_p->deallocate(d_value_p);
//  }
//..
// Finally, we perform a simple test of 'MyContainer', instantiating it with
// element type 'int':
//..
//  int usageExample1()
//  {
//      bslma::TestAllocator testAlloc;
//      MyContainer<int>     C1(123, &testAlloc);
//      assert(C1.allocator() == &testAlloc);
//      assert(C1.front()     == 123);
//
//      MyContainer<int> C2(C1);
//      assert(C2.allocator() == bslma::Default::defaultAllocator());
//      assert(C2.front()     == 123);
//
//      return 0;
//  }
//..
///Example 2: 'bslma' Allocator Propagation
///- - - - - - - - - - - - - - - - - - - -
// This example demonstrates that 'MyContainer' does indeed propagate the
// allocator to its contained element.
//
// First, we create a representative element class, 'MyType', that allocates
// memory using the 'bslma' allocator protocol:
//..
//  #include <bslma_default.h>
//
//  class MyType {
//
//      // DATA
//      // ...
//      bslma::Allocator *d_allocator_p;
//
//    public:
//      // TRAITS
//      BSLMF_NESTED_TRAIT_DECLARATION(MyType, bslma::UsesBslmaAllocator);
//
//      // CREATORS
//      explicit MyType(bslma::Allocator *basicAllocator = 0)
//          // Create a 'MyType' object having the default value.  Optionally
//          // specify a 'basicAllocator' used to supply memory.  If
//          // 'basicAllocator' is 0, the currently installed default allocator
//          // is used.
//      : d_allocator_p(bslma::Default::allocator(basicAllocator))
//      {
//          // ...
//      }
//
//      MyType(const MyType& original, bslma::Allocator *basicAllocator = 0)
//          // Create a 'MyType' object having the same value as the specified
//          // 'original' object.  Optionally specify a 'basicAllocator' used
//          // to supply memory.  If 'basicAllocator' is 0, the currently
//          // installed default allocator is used.
//      : d_allocator_p(bslma::Default::allocator(basicAllocator))
//      {
//          // ...
//      }
//
//      // ...
//
//      // ACCESSORS
//      bslma::Allocator *allocator() const
//          // Return the allocator used by this object to supply memory.
//      {
//          return d_allocator_p;
//      }
//
//      // ...
//  };
//..
// Finally, we instantiate 'MyContainer' using 'MyType' and verify that, when
// we provide the address of an allocator to the constructor of the container,
// the same address is passed to the constructor of the contained element.  We
// also verify that, when the container is copy-constructed without supplying
// an allocator, the copy uses the default allocator, not the allocator from
// the original object.  Moreover, we verify that the element stored in the
// copy also uses the default allocator:
//..
//  #include <bslmf_issame.h>
//
//  int usageExample2()
//  {
//      bslma::TestAllocator testAlloc;
//      MyContainer<MyType> C1(&testAlloc);
//      assert(C1.allocator()         == &testAlloc);
//      assert(C1.front().allocator() == &testAlloc);
//
//      MyContainer<MyType> C2(C1);
//      assert(C2.allocator()         != C1.allocator());
//      assert(C2.allocator()         == bslma::Default::defaultAllocator());
//      assert(C2.front().allocator() != &testAlloc);
//      assert(C2.front().allocator() == bslma::Default::defaultAllocator());
//
//      return 0;
//  }
//..

#include <bslscm_version.h>

#include <bslma_allocator.h>
#include <bslma_destructionutil.h>
#include <bslma_usesbslmaallocator.h>

#include <bslmf_allocatorargt.h>
#include <bslmf_integralconstant.h>
#include <bslmf_isbitwisemoveable.h>
#include <bslmf_isempty.h>
#include <bslmf_isfundamental.h>
#include <bslmf_ismemberpointer.h>
#include <bslmf_ispointer.h>
#include <bslmf_istriviallycopyable.h>
#include <bslmf_istriviallydefaultconstructible.h>
#include <bslmf_movableref.h>
#include <bslmf_removecv.h>
#include <bslmf_usesallocatorargt.h>
#include <bslmf_util.h>

#include <bsls_assert.h>
#include <bsls_compilerfeatures.h>
#include <bsls_libraryfeatures.h>
#include <bsls_platform.h>
#include <bsls_util.h>

#include <stddef.h>
#include <string.h>

#include <new>          // placement 'new'

#ifndef BDE_DONT_ALLOW_TRANSITIVE_INCLUDES
#include <bslma_destructorproctor.h>
#endif

namespace BloombergLP {
namespace bslma {

// Workaround for optimization issue in xlC that mishandles pointer aliasing.
//   IV56864: ALIASING BEHAVIOUR FOR PLACEMENT NEW
//   http://www-01.ibm.com/support/docview.wss?uid=swg1IV56864
// Place this macro following each use of placement new.  Alternatively,
// compile with xlC_r -qalias=noansi, which reduces optimization opportunities
// across entire translation unit instead of simply across optimization fence.
// Update: issue is fixed in xlC 13.1 (__xlC__ >= 0x0d01).

#if defined(BSLS_PLATFORM_CMP_IBM) && BSLS_PLATFORM_CMP_VERSION < 0x0d01
    #define BSLMA_CONSTRUCTIONUTIL_XLC_PLACEMENT_NEW_FIX                     \
                             BSLS_PERFORMANCEHINT_OPTIMIZATION_FENCE
#else
    #define BSLMA_CONSTRUCTIONUTIL_XLC_PLACEMENT_NEW_FIX
#endif

struct ConstructionUtil_Imp;

                          // =======================
                          // struct ConstructionUtil
                          // =======================

struct ConstructionUtil {
    // This 'struct' provides a namespace for utility functions that construct
    // elements of (a template parameter) 'TARGET_TYPE'.

  private:
    // PRIVATE TYPES
    typedef ConstructionUtil_Imp Imp;
        // This 'typedef' is a convenient alias for the implementation-specific
        // utility class defined in this component.

  public:
    // CLASS METHODS
    template <class TARGET_TYPE>
    static void construct(TARGET_TYPE *address, bslma::Allocator *allocator);
    template <class TARGET_TYPE>
    static void construct(TARGET_TYPE *address, void *allocator);
        // Create a default-constructed object of (template parameter) type
        // 'TARGET_TYPE' at the specified 'address'.  If 'allocator' is of type
        // 'bslma::Allocator' and 'TARGET_TYPE' supports 'bslma'-style
        // allocation, 'allocator' is passed to the default constructor.  If
        // the constructor throws, the memory at 'address' is left in an
        // unspecified state.  Note that this operation may bypass the call to
        // the default constructor of a user-defined type entirely if
        // 'TARGET_TYPE' 1) does not use 'bslma'-style allocators and 2) has a
        // trivial default constructor.

    template <class TARGET_TYPE>
    static void construct(TARGET_TYPE        *address,
                          bslma::Allocator   *allocator,
                          const TARGET_TYPE&  original);
    template <class TARGET_TYPE>
    static void
    construct(TARGET_TYPE *address, void *, const TARGET_TYPE& original);
    template <class TARGET_TYPE>
    static void construct(TARGET_TYPE        *address,
                          bslma::Allocator   *allocator,
                          TARGET_TYPE&        original);
    template <class TARGET_TYPE>
    static void
    construct(TARGET_TYPE *address, void *, TARGET_TYPE& original);
        // Create an object of (template parameter) type 'TARGET_TYPE', having
        // the same value as the specified 'original' object, at the specified
        // 'address'.  If 'allocator' is of type 'bslma::Allocator' and
        // 'TARGET_TYPE' supports 'bslma'-style allocation, 'allocator' is
        // propagated to the newly created object.  If a constructor throws,
        // the memory at 'address' is left in an unspecified state.  Note that
        // this operation may elide the call to the copy constructor entirely
        // if 'TARGET_TYPE' 1) does not use 'bslma'-style allocators and 2) is
        // trivially copyable.  Further note that we provide (unconventional)
        // overloads for modifiable lvalues because these may match more
        // generic overloads (below) taking a variable number of deduced
        // template parameters.

    template <class TARGET_TYPE>
    static void construct(TARGET_TYPE                    *address,
                          bslma::Allocator               *allocator,
                          bslmf::MovableRef<TARGET_TYPE>  original);
    template <class TARGET_TYPE>
    static void construct(TARGET_TYPE                    *address,
                          void                           *allocator,
                          bslmf::MovableRef<TARGET_TYPE>  original);
        // Create an object of (template parameter) type 'TARGET_TYPE', having
        // the same value as the specified 'original' object, at the specified
        // 'address'.  'original' is left in a valid but unspecified state.  If
        // the specified 'allocator' is of type 'bslma::Allocator' and
        // 'TARGET_TYPE' supports 'bslma'-style allocation, 'allocator' is
        // propagated to the newly created object.  If a constructor throws,
        // the memory at 'address' is left in an unspecified state.  Note that
        // this operation may elide the call to the copy constructor entirely
        // if 'TARGET_TYPE' 1) does not use 'bslma'-style allocators and 2) is
        // trivially copyable.  Further note that we provide (unconventional)
        // overloads for modifiable lvalues because these may match more
        // generic overloads (below) taking a variable number of deduced
        // template parameters.

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES // $var-args=15
    template <class TARGET_TYPE, class Arg1, class... Args>
    static void construct(TARGET_TYPE      *address,
                          bslma::Allocator *allocator,
                          Arg1&&            argument,
                          Args&&...         arguments);
    template <class TARGET_TYPE, class Arg1, class... Args>
    static void construct(TARGET_TYPE *address,
                          void        *allocator,
                          Arg1&&       argument,
                          Args&&...    arguments);
        // Create an object of (template parameter) type 'TARGET_TYPE' at
        // the specified 'address', constructed by forwarding the specified
        // 'argument' and the (variable number of) additional 'arguments' to
        // the corresponding constructor of 'TARGET_TYPE'.  If 'allocator' is
        // of type 'bslma::Allocator' and 'TARGET_TYPE' supports 'bslma'-style
        // allocation, the allocator is passed to the constructor (as the last
        // argument).  If the constructor throws, the memory at 'address' is
        // left in an unspecified state.
#elif BSLS_COMPILERFEATURES_SIMULATE_VARIADIC_TEMPLATES
// {{{ BEGIN GENERATED CODE
// The following section is automatically generated.  **DO NOT EDIT**
// Generator command line: sim_cpp11_features.pl bslma_constructionutil.h
    template <class TARGET_TYPE, class Arg1>
    static void construct(TARGET_TYPE      *address,
                          bslma::Allocator *allocator,
                          BSLS_COMPILERFEATURES_FORWARD_REF(Arg1) argument);

    template <class TARGET_TYPE, class Arg1, class Args_01>
    static void construct(TARGET_TYPE      *address,
                          bslma::Allocator *allocator,
                          BSLS_COMPILERFEATURES_FORWARD_REF(Arg1) argument,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01);

    template <class TARGET_TYPE, class Arg1, class Args_01,
                                             class Args_02>
    static void construct(TARGET_TYPE      *address,
                          bslma::Allocator *allocator,
                          BSLS_COMPILERFEATURES_FORWARD_REF(Arg1) argument,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02);

    template <class TARGET_TYPE, class Arg1, class Args_01,
                                             class Args_02,
                                             class Args_03>
    static void construct(TARGET_TYPE      *address,
                          bslma::Allocator *allocator,
                          BSLS_COMPILERFEATURES_FORWARD_REF(Arg1) argument,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03);

    template <class TARGET_TYPE, class Arg1, class Args_01,
                                             class Args_02,
                                             class Args_03,
                                             class Args_04>
    static void construct(TARGET_TYPE      *address,
                          bslma::Allocator *allocator,
                          BSLS_COMPILERFEATURES_FORWARD_REF(Arg1) argument,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04);

    template <class TARGET_TYPE, class Arg1, class Args_01,
                                             class Args_02,
                                             class Args_03,
                                             class Args_04,
                                             class Args_05>
    static void construct(TARGET_TYPE      *address,
                          bslma::Allocator *allocator,
                          BSLS_COMPILERFEATURES_FORWARD_REF(Arg1) argument,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05);

    template <class TARGET_TYPE, class Arg1, class Args_01,
                                             class Args_02,
                                             class Args_03,
                                             class Args_04,
                                             class Args_05,
                                             class Args_06>
    static void construct(TARGET_TYPE      *address,
                          bslma::Allocator *allocator,
                          BSLS_COMPILERFEATURES_FORWARD_REF(Arg1) argument,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06);

    template <class TARGET_TYPE, class Arg1, class Args_01,
                                             class Args_02,
                                             class Args_03,
                                             class Args_04,
                                             class Args_05,
                                             class Args_06,
                                             class Args_07>
    static void construct(TARGET_TYPE      *address,
                          bslma::Allocator *allocator,
                          BSLS_COMPILERFEATURES_FORWARD_REF(Arg1) argument,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07);

    template <class TARGET_TYPE, class Arg1, class Args_01,
                                             class Args_02,
                                             class Args_03,
                                             class Args_04,
                                             class Args_05,
                                             class Args_06,
                                             class Args_07,
                                             class Args_08>
    static void construct(TARGET_TYPE      *address,
                          bslma::Allocator *allocator,
                          BSLS_COMPILERFEATURES_FORWARD_REF(Arg1) argument,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) arguments_08);

    template <class TARGET_TYPE, class Arg1, class Args_01,
                                             class Args_02,
                                             class Args_03,
                                             class Args_04,
                                             class Args_05,
                                             class Args_06,
                                             class Args_07,
                                             class Args_08,
                                             class Args_09>
    static void construct(TARGET_TYPE      *address,
                          bslma::Allocator *allocator,
                          BSLS_COMPILERFEATURES_FORWARD_REF(Arg1) argument,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) arguments_08,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_09) arguments_09);

    template <class TARGET_TYPE, class Arg1, class Args_01,
                                             class Args_02,
                                             class Args_03,
                                             class Args_04,
                                             class Args_05,
                                             class Args_06,
                                             class Args_07,
                                             class Args_08,
                                             class Args_09,
                                             class Args_10>
    static void construct(TARGET_TYPE      *address,
                          bslma::Allocator *allocator,
                          BSLS_COMPILERFEATURES_FORWARD_REF(Arg1) argument,
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

    template <class TARGET_TYPE, class Arg1, class Args_01,
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
    static void construct(TARGET_TYPE      *address,
                          bslma::Allocator *allocator,
                          BSLS_COMPILERFEATURES_FORWARD_REF(Arg1) argument,
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

    template <class TARGET_TYPE, class Arg1, class Args_01,
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
    static void construct(TARGET_TYPE      *address,
                          bslma::Allocator *allocator,
                          BSLS_COMPILERFEATURES_FORWARD_REF(Arg1) argument,
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

    template <class TARGET_TYPE, class Arg1, class Args_01,
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
    static void construct(TARGET_TYPE      *address,
                          bslma::Allocator *allocator,
                          BSLS_COMPILERFEATURES_FORWARD_REF(Arg1) argument,
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

    template <class TARGET_TYPE, class Arg1, class Args_01,
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
    static void construct(TARGET_TYPE      *address,
                          bslma::Allocator *allocator,
                          BSLS_COMPILERFEATURES_FORWARD_REF(Arg1) argument,
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

    template <class TARGET_TYPE, class Arg1, class Args_01,
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
                                             class Args_14,
                                             class Args_15>
    static void construct(TARGET_TYPE      *address,
                          bslma::Allocator *allocator,
                          BSLS_COMPILERFEATURES_FORWARD_REF(Arg1) argument,
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
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_14) arguments_14,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_15) arguments_15);

    template <class TARGET_TYPE, class Arg1>
    static void construct(TARGET_TYPE *address,
                          void        *allocator,
                          BSLS_COMPILERFEATURES_FORWARD_REF(Arg1) argument);

    template <class TARGET_TYPE, class Arg1, class Args_01>
    static void construct(TARGET_TYPE *address,
                          void        *allocator,
                          BSLS_COMPILERFEATURES_FORWARD_REF(Arg1) argument,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01);

    template <class TARGET_TYPE, class Arg1, class Args_01,
                                             class Args_02>
    static void construct(TARGET_TYPE *address,
                          void        *allocator,
                          BSLS_COMPILERFEATURES_FORWARD_REF(Arg1) argument,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02);

    template <class TARGET_TYPE, class Arg1, class Args_01,
                                             class Args_02,
                                             class Args_03>
    static void construct(TARGET_TYPE *address,
                          void        *allocator,
                          BSLS_COMPILERFEATURES_FORWARD_REF(Arg1) argument,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03);

    template <class TARGET_TYPE, class Arg1, class Args_01,
                                             class Args_02,
                                             class Args_03,
                                             class Args_04>
    static void construct(TARGET_TYPE *address,
                          void        *allocator,
                          BSLS_COMPILERFEATURES_FORWARD_REF(Arg1) argument,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04);

    template <class TARGET_TYPE, class Arg1, class Args_01,
                                             class Args_02,
                                             class Args_03,
                                             class Args_04,
                                             class Args_05>
    static void construct(TARGET_TYPE *address,
                          void        *allocator,
                          BSLS_COMPILERFEATURES_FORWARD_REF(Arg1) argument,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05);

    template <class TARGET_TYPE, class Arg1, class Args_01,
                                             class Args_02,
                                             class Args_03,
                                             class Args_04,
                                             class Args_05,
                                             class Args_06>
    static void construct(TARGET_TYPE *address,
                          void        *allocator,
                          BSLS_COMPILERFEATURES_FORWARD_REF(Arg1) argument,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06);

    template <class TARGET_TYPE, class Arg1, class Args_01,
                                             class Args_02,
                                             class Args_03,
                                             class Args_04,
                                             class Args_05,
                                             class Args_06,
                                             class Args_07>
    static void construct(TARGET_TYPE *address,
                          void        *allocator,
                          BSLS_COMPILERFEATURES_FORWARD_REF(Arg1) argument,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07);

    template <class TARGET_TYPE, class Arg1, class Args_01,
                                             class Args_02,
                                             class Args_03,
                                             class Args_04,
                                             class Args_05,
                                             class Args_06,
                                             class Args_07,
                                             class Args_08>
    static void construct(TARGET_TYPE *address,
                          void        *allocator,
                          BSLS_COMPILERFEATURES_FORWARD_REF(Arg1) argument,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) arguments_08);

    template <class TARGET_TYPE, class Arg1, class Args_01,
                                             class Args_02,
                                             class Args_03,
                                             class Args_04,
                                             class Args_05,
                                             class Args_06,
                                             class Args_07,
                                             class Args_08,
                                             class Args_09>
    static void construct(TARGET_TYPE *address,
                          void        *allocator,
                          BSLS_COMPILERFEATURES_FORWARD_REF(Arg1) argument,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) arguments_08,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_09) arguments_09);

    template <class TARGET_TYPE, class Arg1, class Args_01,
                                             class Args_02,
                                             class Args_03,
                                             class Args_04,
                                             class Args_05,
                                             class Args_06,
                                             class Args_07,
                                             class Args_08,
                                             class Args_09,
                                             class Args_10>
    static void construct(TARGET_TYPE *address,
                          void        *allocator,
                          BSLS_COMPILERFEATURES_FORWARD_REF(Arg1) argument,
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

    template <class TARGET_TYPE, class Arg1, class Args_01,
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
    static void construct(TARGET_TYPE *address,
                          void        *allocator,
                          BSLS_COMPILERFEATURES_FORWARD_REF(Arg1) argument,
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

    template <class TARGET_TYPE, class Arg1, class Args_01,
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
    static void construct(TARGET_TYPE *address,
                          void        *allocator,
                          BSLS_COMPILERFEATURES_FORWARD_REF(Arg1) argument,
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

    template <class TARGET_TYPE, class Arg1, class Args_01,
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
    static void construct(TARGET_TYPE *address,
                          void        *allocator,
                          BSLS_COMPILERFEATURES_FORWARD_REF(Arg1) argument,
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

    template <class TARGET_TYPE, class Arg1, class Args_01,
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
    static void construct(TARGET_TYPE *address,
                          void        *allocator,
                          BSLS_COMPILERFEATURES_FORWARD_REF(Arg1) argument,
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

    template <class TARGET_TYPE, class Arg1, class Args_01,
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
                                             class Args_14,
                                             class Args_15>
    static void construct(TARGET_TYPE *address,
                          void        *allocator,
                          BSLS_COMPILERFEATURES_FORWARD_REF(Arg1) argument,
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
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_14) arguments_14,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_15) arguments_15);

#else
// The generated code below is a workaround for the absence of perfect
// forwarding in some compilers.
    template <class TARGET_TYPE, class Arg1, class... Args>
    static void construct(TARGET_TYPE      *address,
                          bslma::Allocator *allocator,
                          BSLS_COMPILERFEATURES_FORWARD_REF(Arg1) argument,
                         BSLS_COMPILERFEATURES_FORWARD_REF(Args)... arguments);
    template <class TARGET_TYPE, class Arg1, class... Args>
    static void construct(TARGET_TYPE *address,
                          void        *allocator,
                          BSLS_COMPILERFEATURES_FORWARD_REF(Arg1) argument,
                         BSLS_COMPILERFEATURES_FORWARD_REF(Args)... arguments);
// }}} END GENERATED CODE
#endif

    template <class TARGET_TYPE, class ALLOCATOR>
    static void destructiveMove(TARGET_TYPE *address,
                                ALLOCATOR   *allocator,
                                TARGET_TYPE *original);
        // Create an object of (template parameter) type 'TARGET_TYPE' at the
        // specified 'address' having the same value as the object at the
        // specified 'original' address, propagating the specified 'allocator'
        // to the moved object if 'TARGET_TYPE' uses 'bslma'-style allocation
        // and the (template parameter) type 'ALLOCATOR' is implicitly
        // convertible to 'bslma::Allocator', and destroy 'original'.  If the
        // move constructor throws an exception, the memory at 'address' is
        // left in an unspecified state and 'original' is left in a valid but
        // unspecified state.  The behavior is undefined unless either
        // 'TARGET_TYPE' does not support 'bslma'-style allocation or
        // 'original' uses 'allocator' to supply memory.  Note that this class
        // method is equivalent to move-constructing an object at 'address'
        // from '*original' and then destroying 'original', except that this
        // method elides the calls to the constructor and destructor for
        // objects that are bitwise movable.  Also note that if 'original'
        // actually points to an object of a type derived from 'TARGET_TYPE'
        // (i.e., a slicing move) where 'TARGET_TYPE' has a non-'virtual'
        // destructor and is not bitwise-movable, then 'original' will be only
        // partially destroyed.
};

                        // ===========================
                        // struct ConstructionUtil_Imp
                        // ===========================

struct ConstructionUtil_Imp {
    // This 'struct' provides a namespace for a suite of utility functions that
    // are used to implement functions in 'ConstructionUtil'.  In particular,
    // they provide overloads, resolved at compile-time, for various features
    // (e.g., passing down the allocator to sub-elements of 'pair'-like types)
    // and optimizations (e.g., bypassing the call to the constructor for
    // classes with trivial default and copy constructors).  These functions
    // should not be used outside this component.

    // TYPES
    enum {
        // These constants are used in the overloads below, when the last
        // argument is of type 'bsl::integral_constant<int, N> *', indicating
        // that 'TARGET_TYPE' has the traits for which the enumerator equal to
        // 'N' is named.

        e_USES_ALLOCATOR_ARG_T_TRAITS     = 5, // Implies USES_BSLMA_ALLOCATOR
        e_USES_BSLMA_ALLOCATOR_TRAITS     = 4,
        e_HAS_TRIVIAL_DEFAULT_CTOR_TRAITS = 3,
        e_BITWISE_COPYABLE_TRAITS         = 2,
        e_BITWISE_MOVABLE_TRAITS          = 1,
        e_NIL_TRAITS                      = 0
    };

    // CLASS METHODS
    template <class TARGET_TYPE>
    static void construct(
             TARGET_TYPE      *address,
             bslma::Allocator *allocator,
             bsl::integral_constant<int, e_HAS_TRIVIAL_DEFAULT_CTOR_TRAITS> *);

    template <class TARGET_TYPE>
    static void construct(
             TARGET_TYPE *address,
             bsl::integral_constant<int, e_HAS_TRIVIAL_DEFAULT_CTOR_TRAITS> *);
        // Construct a default instance of (template parameter) type
        // 'TARGET_TYPE' that has a trivial default constructor, at the
        // specified 'address'.  If the constructor throws, the memory at
        // 'address' is left in an unspecified state.  Note that the behavior
        // is undefined if 'TARGET_TYPE' supports 'bslma'-style allocators.

    // In order to implement:
    //   'allocator_traits<A>::construct(m, p, rv)'
    //   'allocator_traits<A>::construct(m, p,  v)'
    // if 'e_BITWISE_COPYABLE_TRAITS'.

    template <class TARGET_TYPE>
    static void
    construct(TARGET_TYPE        *address,
              bslma::Allocator   *allocator,
              bsl::integral_constant<int, e_BITWISE_COPYABLE_TRAITS> *,
              const TARGET_TYPE&  original);

    template <class TARGET_TYPE>
    static void
    construct(TARGET_TYPE        *address,
              bsl::integral_constant<int, e_BITWISE_COPYABLE_TRAITS> *,
              const TARGET_TYPE&  original);
        // Create an object of a bitwise copyable (template parameter) type
        // 'TARGET_TYPE' at the specified 'address', with the same value as the
        // specified 'original' object.  If the constructor throws, the memory
        // at 'address' is left in an unspecified state.  Note that the
        // behavior is undefined if 'TARGET_TYPE' supports 'bslma'-style
        // allocators.

    template <class TARGET_TYPE>
    static void
    construct(
             TARGET_TYPE                                            *address,
             bslma::Allocator                                       *allocator,
             bsl::integral_constant<int, e_BITWISE_COPYABLE_TRAITS> *,
             bslmf::MovableRef<TARGET_TYPE>                          original);
    template <class TARGET_TYPE>
    static void
    construct(
             TARGET_TYPE                                            *address,
             bsl::integral_constant<int, e_BITWISE_COPYABLE_TRAITS> *,
             bslmf::MovableRef<TARGET_TYPE>                          original);
        // Create an object of a bitwise moveable (template parameter) type
        // 'TARGET_TYPE' at the specified 'address', with the same value as the
        // specified 'original' object.  If the constructor throws, the memory
        // at 'address' is left in an unspecified state.  Note that the
        // behavior is undefined if 'TARGET_TYPE' supports 'bslma'-style
        // allocators.

    // In order to implement:
    //   'allocator_traits<A>::construct(m, p    )'
    //   'allocator_traits<A>::construct(m, p, rv)'
    //   'allocator_traits<A>::construct(m, p,  v)'
    // if 'e_USES_BSLMA_ALLOCATOR_TRAITS', and:
    //   'allocator_traits<A>::construct(m, p, args)'

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES // $var-args=15
    template <class TARGET_TYPE, class... Args>
    static void construct(
                  TARGET_TYPE      *address,
                  bslma::Allocator *allocator,
                  bsl::integral_constant<int, e_USES_BSLMA_ALLOCATOR_TRAITS> *,
                  Args&&...         arguments);
    template <class TARGET_TYPE, class... Args>
    static void construct(
                  TARGET_TYPE      *address,
                  bslma::Allocator *allocator,
                  bsl::integral_constant<int, e_USES_ALLOCATOR_ARG_T_TRAITS> *,
                  Args&&...         arguments);
        // TBD: fix comment
        // Construct an object of (template parameter) type 'TARGET_TYPE',
        // that supports 'bslma'-style allocators, at the specified 'address',
        // invoking the constructor of 'TARGET_TYPE' corresponding to the
        // specified 'arguments'.  If 'allocator' is of type
        // 'bslma::Allocator', the allocator is passed to the constructor.  If
        // the constructor throws, the memory at 'address' is left in an
        // unspecified state.

    template <class TARGET_TYPE, class... Args>
    static void construct(TARGET_TYPE      *address,
                          bslma::Allocator *allocator,
                          bsl::integral_constant<int, e_NIL_TRAITS> *,
                          Args&&...         arguments);

    template <class TARGET_TYPE, class... Args>
    static void construct(TARGET_TYPE      *address,
                          bsl::integral_constant<int, e_NIL_TRAITS> *,
                          Args&&...         arguments);
        // Construct an object of (template parameter) type 'TARGET_TYPE',
        // that does not support 'bslma'-style allocators, at the specified
        // 'address', invoking the constructor of 'TARGET_TYPE' corresponding
        // to the specified 'arguments'.  If the constructor throws, the memory
        // at 'address' is left in an unspecified state.
#elif BSLS_COMPILERFEATURES_SIMULATE_VARIADIC_TEMPLATES
// {{{ BEGIN GENERATED CODE
// The following section is automatically generated.  **DO NOT EDIT**
// Generator command line: sim_cpp11_features.pl bslma_constructionutil.h
    template <class TARGET_TYPE>
    static void construct(
                  TARGET_TYPE      *address,
                  bslma::Allocator *allocator,
                  bsl::integral_constant<int, e_USES_BSLMA_ALLOCATOR_TRAITS> *);

    template <class TARGET_TYPE, class Args_01>
    static void construct(
                  TARGET_TYPE      *address,
                  bslma::Allocator *allocator,
                  bsl::integral_constant<int, e_USES_BSLMA_ALLOCATOR_TRAITS> *,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01);

    template <class TARGET_TYPE, class Args_01,
                                 class Args_02>
    static void construct(
                  TARGET_TYPE      *address,
                  bslma::Allocator *allocator,
                  bsl::integral_constant<int, e_USES_BSLMA_ALLOCATOR_TRAITS> *,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02);

    template <class TARGET_TYPE, class Args_01,
                                 class Args_02,
                                 class Args_03>
    static void construct(
                  TARGET_TYPE      *address,
                  bslma::Allocator *allocator,
                  bsl::integral_constant<int, e_USES_BSLMA_ALLOCATOR_TRAITS> *,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03);

    template <class TARGET_TYPE, class Args_01,
                                 class Args_02,
                                 class Args_03,
                                 class Args_04>
    static void construct(
                  TARGET_TYPE      *address,
                  bslma::Allocator *allocator,
                  bsl::integral_constant<int, e_USES_BSLMA_ALLOCATOR_TRAITS> *,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04);

    template <class TARGET_TYPE, class Args_01,
                                 class Args_02,
                                 class Args_03,
                                 class Args_04,
                                 class Args_05>
    static void construct(
                  TARGET_TYPE      *address,
                  bslma::Allocator *allocator,
                  bsl::integral_constant<int, e_USES_BSLMA_ALLOCATOR_TRAITS> *,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05);

    template <class TARGET_TYPE, class Args_01,
                                 class Args_02,
                                 class Args_03,
                                 class Args_04,
                                 class Args_05,
                                 class Args_06>
    static void construct(
                  TARGET_TYPE      *address,
                  bslma::Allocator *allocator,
                  bsl::integral_constant<int, e_USES_BSLMA_ALLOCATOR_TRAITS> *,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06);

    template <class TARGET_TYPE, class Args_01,
                                 class Args_02,
                                 class Args_03,
                                 class Args_04,
                                 class Args_05,
                                 class Args_06,
                                 class Args_07>
    static void construct(
                  TARGET_TYPE      *address,
                  bslma::Allocator *allocator,
                  bsl::integral_constant<int, e_USES_BSLMA_ALLOCATOR_TRAITS> *,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07);

    template <class TARGET_TYPE, class Args_01,
                                 class Args_02,
                                 class Args_03,
                                 class Args_04,
                                 class Args_05,
                                 class Args_06,
                                 class Args_07,
                                 class Args_08>
    static void construct(
                  TARGET_TYPE      *address,
                  bslma::Allocator *allocator,
                  bsl::integral_constant<int, e_USES_BSLMA_ALLOCATOR_TRAITS> *,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) arguments_08);

    template <class TARGET_TYPE, class Args_01,
                                 class Args_02,
                                 class Args_03,
                                 class Args_04,
                                 class Args_05,
                                 class Args_06,
                                 class Args_07,
                                 class Args_08,
                                 class Args_09>
    static void construct(
                  TARGET_TYPE      *address,
                  bslma::Allocator *allocator,
                  bsl::integral_constant<int, e_USES_BSLMA_ALLOCATOR_TRAITS> *,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) arguments_08,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_09) arguments_09);

    template <class TARGET_TYPE, class Args_01,
                                 class Args_02,
                                 class Args_03,
                                 class Args_04,
                                 class Args_05,
                                 class Args_06,
                                 class Args_07,
                                 class Args_08,
                                 class Args_09,
                                 class Args_10>
    static void construct(
                  TARGET_TYPE      *address,
                  bslma::Allocator *allocator,
                  bsl::integral_constant<int, e_USES_BSLMA_ALLOCATOR_TRAITS> *,
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

    template <class TARGET_TYPE, class Args_01,
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
    static void construct(
                  TARGET_TYPE      *address,
                  bslma::Allocator *allocator,
                  bsl::integral_constant<int, e_USES_BSLMA_ALLOCATOR_TRAITS> *,
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

    template <class TARGET_TYPE, class Args_01,
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
    static void construct(
                  TARGET_TYPE      *address,
                  bslma::Allocator *allocator,
                  bsl::integral_constant<int, e_USES_BSLMA_ALLOCATOR_TRAITS> *,
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

    template <class TARGET_TYPE, class Args_01,
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
    static void construct(
                  TARGET_TYPE      *address,
                  bslma::Allocator *allocator,
                  bsl::integral_constant<int, e_USES_BSLMA_ALLOCATOR_TRAITS> *,
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

    template <class TARGET_TYPE, class Args_01,
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
    static void construct(
                  TARGET_TYPE      *address,
                  bslma::Allocator *allocator,
                  bsl::integral_constant<int, e_USES_BSLMA_ALLOCATOR_TRAITS> *,
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

    template <class TARGET_TYPE, class Args_01,
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
                                 class Args_14,
                                 class Args_15>
    static void construct(
                  TARGET_TYPE      *address,
                  bslma::Allocator *allocator,
                  bsl::integral_constant<int, e_USES_BSLMA_ALLOCATOR_TRAITS> *,
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
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_14) arguments_14,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_15) arguments_15);

    template <class TARGET_TYPE>
    static void construct(
                  TARGET_TYPE      *address,
                  bslma::Allocator *allocator,
                  bsl::integral_constant<int, e_USES_ALLOCATOR_ARG_T_TRAITS> *);

    template <class TARGET_TYPE, class Args_01>
    static void construct(
                  TARGET_TYPE      *address,
                  bslma::Allocator *allocator,
                  bsl::integral_constant<int, e_USES_ALLOCATOR_ARG_T_TRAITS> *,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01);

    template <class TARGET_TYPE, class Args_01,
                                 class Args_02>
    static void construct(
                  TARGET_TYPE      *address,
                  bslma::Allocator *allocator,
                  bsl::integral_constant<int, e_USES_ALLOCATOR_ARG_T_TRAITS> *,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02);

    template <class TARGET_TYPE, class Args_01,
                                 class Args_02,
                                 class Args_03>
    static void construct(
                  TARGET_TYPE      *address,
                  bslma::Allocator *allocator,
                  bsl::integral_constant<int, e_USES_ALLOCATOR_ARG_T_TRAITS> *,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03);

    template <class TARGET_TYPE, class Args_01,
                                 class Args_02,
                                 class Args_03,
                                 class Args_04>
    static void construct(
                  TARGET_TYPE      *address,
                  bslma::Allocator *allocator,
                  bsl::integral_constant<int, e_USES_ALLOCATOR_ARG_T_TRAITS> *,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04);

    template <class TARGET_TYPE, class Args_01,
                                 class Args_02,
                                 class Args_03,
                                 class Args_04,
                                 class Args_05>
    static void construct(
                  TARGET_TYPE      *address,
                  bslma::Allocator *allocator,
                  bsl::integral_constant<int, e_USES_ALLOCATOR_ARG_T_TRAITS> *,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05);

    template <class TARGET_TYPE, class Args_01,
                                 class Args_02,
                                 class Args_03,
                                 class Args_04,
                                 class Args_05,
                                 class Args_06>
    static void construct(
                  TARGET_TYPE      *address,
                  bslma::Allocator *allocator,
                  bsl::integral_constant<int, e_USES_ALLOCATOR_ARG_T_TRAITS> *,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06);

    template <class TARGET_TYPE, class Args_01,
                                 class Args_02,
                                 class Args_03,
                                 class Args_04,
                                 class Args_05,
                                 class Args_06,
                                 class Args_07>
    static void construct(
                  TARGET_TYPE      *address,
                  bslma::Allocator *allocator,
                  bsl::integral_constant<int, e_USES_ALLOCATOR_ARG_T_TRAITS> *,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07);

    template <class TARGET_TYPE, class Args_01,
                                 class Args_02,
                                 class Args_03,
                                 class Args_04,
                                 class Args_05,
                                 class Args_06,
                                 class Args_07,
                                 class Args_08>
    static void construct(
                  TARGET_TYPE      *address,
                  bslma::Allocator *allocator,
                  bsl::integral_constant<int, e_USES_ALLOCATOR_ARG_T_TRAITS> *,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) arguments_08);

    template <class TARGET_TYPE, class Args_01,
                                 class Args_02,
                                 class Args_03,
                                 class Args_04,
                                 class Args_05,
                                 class Args_06,
                                 class Args_07,
                                 class Args_08,
                                 class Args_09>
    static void construct(
                  TARGET_TYPE      *address,
                  bslma::Allocator *allocator,
                  bsl::integral_constant<int, e_USES_ALLOCATOR_ARG_T_TRAITS> *,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) arguments_08,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_09) arguments_09);

    template <class TARGET_TYPE, class Args_01,
                                 class Args_02,
                                 class Args_03,
                                 class Args_04,
                                 class Args_05,
                                 class Args_06,
                                 class Args_07,
                                 class Args_08,
                                 class Args_09,
                                 class Args_10>
    static void construct(
                  TARGET_TYPE      *address,
                  bslma::Allocator *allocator,
                  bsl::integral_constant<int, e_USES_ALLOCATOR_ARG_T_TRAITS> *,
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

    template <class TARGET_TYPE, class Args_01,
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
    static void construct(
                  TARGET_TYPE      *address,
                  bslma::Allocator *allocator,
                  bsl::integral_constant<int, e_USES_ALLOCATOR_ARG_T_TRAITS> *,
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

    template <class TARGET_TYPE, class Args_01,
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
    static void construct(
                  TARGET_TYPE      *address,
                  bslma::Allocator *allocator,
                  bsl::integral_constant<int, e_USES_ALLOCATOR_ARG_T_TRAITS> *,
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

    template <class TARGET_TYPE, class Args_01,
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
    static void construct(
                  TARGET_TYPE      *address,
                  bslma::Allocator *allocator,
                  bsl::integral_constant<int, e_USES_ALLOCATOR_ARG_T_TRAITS> *,
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

    template <class TARGET_TYPE, class Args_01,
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
    static void construct(
                  TARGET_TYPE      *address,
                  bslma::Allocator *allocator,
                  bsl::integral_constant<int, e_USES_ALLOCATOR_ARG_T_TRAITS> *,
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

    template <class TARGET_TYPE, class Args_01,
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
                                 class Args_14,
                                 class Args_15>
    static void construct(
                  TARGET_TYPE      *address,
                  bslma::Allocator *allocator,
                  bsl::integral_constant<int, e_USES_ALLOCATOR_ARG_T_TRAITS> *,
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
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_14) arguments_14,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_15) arguments_15);

    template <class TARGET_TYPE>
    static void construct(TARGET_TYPE      *address,
                          bslma::Allocator *allocator,
                          bsl::integral_constant<int, e_NIL_TRAITS> *);

    template <class TARGET_TYPE, class Args_01>
    static void construct(TARGET_TYPE      *address,
                          bslma::Allocator *allocator,
                          bsl::integral_constant<int, e_NIL_TRAITS> *,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01);

    template <class TARGET_TYPE, class Args_01,
                                 class Args_02>
    static void construct(TARGET_TYPE      *address,
                          bslma::Allocator *allocator,
                          bsl::integral_constant<int, e_NIL_TRAITS> *,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02);

    template <class TARGET_TYPE, class Args_01,
                                 class Args_02,
                                 class Args_03>
    static void construct(TARGET_TYPE      *address,
                          bslma::Allocator *allocator,
                          bsl::integral_constant<int, e_NIL_TRAITS> *,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03);

    template <class TARGET_TYPE, class Args_01,
                                 class Args_02,
                                 class Args_03,
                                 class Args_04>
    static void construct(TARGET_TYPE      *address,
                          bslma::Allocator *allocator,
                          bsl::integral_constant<int, e_NIL_TRAITS> *,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04);

    template <class TARGET_TYPE, class Args_01,
                                 class Args_02,
                                 class Args_03,
                                 class Args_04,
                                 class Args_05>
    static void construct(TARGET_TYPE      *address,
                          bslma::Allocator *allocator,
                          bsl::integral_constant<int, e_NIL_TRAITS> *,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05);

    template <class TARGET_TYPE, class Args_01,
                                 class Args_02,
                                 class Args_03,
                                 class Args_04,
                                 class Args_05,
                                 class Args_06>
    static void construct(TARGET_TYPE      *address,
                          bslma::Allocator *allocator,
                          bsl::integral_constant<int, e_NIL_TRAITS> *,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06);

    template <class TARGET_TYPE, class Args_01,
                                 class Args_02,
                                 class Args_03,
                                 class Args_04,
                                 class Args_05,
                                 class Args_06,
                                 class Args_07>
    static void construct(TARGET_TYPE      *address,
                          bslma::Allocator *allocator,
                          bsl::integral_constant<int, e_NIL_TRAITS> *,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07);

    template <class TARGET_TYPE, class Args_01,
                                 class Args_02,
                                 class Args_03,
                                 class Args_04,
                                 class Args_05,
                                 class Args_06,
                                 class Args_07,
                                 class Args_08>
    static void construct(TARGET_TYPE      *address,
                          bslma::Allocator *allocator,
                          bsl::integral_constant<int, e_NIL_TRAITS> *,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) arguments_08);

    template <class TARGET_TYPE, class Args_01,
                                 class Args_02,
                                 class Args_03,
                                 class Args_04,
                                 class Args_05,
                                 class Args_06,
                                 class Args_07,
                                 class Args_08,
                                 class Args_09>
    static void construct(TARGET_TYPE      *address,
                          bslma::Allocator *allocator,
                          bsl::integral_constant<int, e_NIL_TRAITS> *,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) arguments_08,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_09) arguments_09);

    template <class TARGET_TYPE, class Args_01,
                                 class Args_02,
                                 class Args_03,
                                 class Args_04,
                                 class Args_05,
                                 class Args_06,
                                 class Args_07,
                                 class Args_08,
                                 class Args_09,
                                 class Args_10>
    static void construct(TARGET_TYPE      *address,
                          bslma::Allocator *allocator,
                          bsl::integral_constant<int, e_NIL_TRAITS> *,
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

    template <class TARGET_TYPE, class Args_01,
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
    static void construct(TARGET_TYPE      *address,
                          bslma::Allocator *allocator,
                          bsl::integral_constant<int, e_NIL_TRAITS> *,
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

    template <class TARGET_TYPE, class Args_01,
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
    static void construct(TARGET_TYPE      *address,
                          bslma::Allocator *allocator,
                          bsl::integral_constant<int, e_NIL_TRAITS> *,
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

    template <class TARGET_TYPE, class Args_01,
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
    static void construct(TARGET_TYPE      *address,
                          bslma::Allocator *allocator,
                          bsl::integral_constant<int, e_NIL_TRAITS> *,
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

    template <class TARGET_TYPE, class Args_01,
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
    static void construct(TARGET_TYPE      *address,
                          bslma::Allocator *allocator,
                          bsl::integral_constant<int, e_NIL_TRAITS> *,
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

    template <class TARGET_TYPE, class Args_01,
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
                                 class Args_14,
                                 class Args_15>
    static void construct(TARGET_TYPE      *address,
                          bslma::Allocator *allocator,
                          bsl::integral_constant<int, e_NIL_TRAITS> *,
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
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_14) arguments_14,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_15) arguments_15);


    template <class TARGET_TYPE>
    static void construct(TARGET_TYPE      *address,
                          bsl::integral_constant<int, e_NIL_TRAITS> *);

    template <class TARGET_TYPE, class Args_01>
    static void construct(TARGET_TYPE      *address,
                          bsl::integral_constant<int, e_NIL_TRAITS> *,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01);

    template <class TARGET_TYPE, class Args_01,
                                 class Args_02>
    static void construct(TARGET_TYPE      *address,
                          bsl::integral_constant<int, e_NIL_TRAITS> *,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02);

    template <class TARGET_TYPE, class Args_01,
                                 class Args_02,
                                 class Args_03>
    static void construct(TARGET_TYPE      *address,
                          bsl::integral_constant<int, e_NIL_TRAITS> *,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03);

    template <class TARGET_TYPE, class Args_01,
                                 class Args_02,
                                 class Args_03,
                                 class Args_04>
    static void construct(TARGET_TYPE      *address,
                          bsl::integral_constant<int, e_NIL_TRAITS> *,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04);

    template <class TARGET_TYPE, class Args_01,
                                 class Args_02,
                                 class Args_03,
                                 class Args_04,
                                 class Args_05>
    static void construct(TARGET_TYPE      *address,
                          bsl::integral_constant<int, e_NIL_TRAITS> *,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05);

    template <class TARGET_TYPE, class Args_01,
                                 class Args_02,
                                 class Args_03,
                                 class Args_04,
                                 class Args_05,
                                 class Args_06>
    static void construct(TARGET_TYPE      *address,
                          bsl::integral_constant<int, e_NIL_TRAITS> *,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06);

    template <class TARGET_TYPE, class Args_01,
                                 class Args_02,
                                 class Args_03,
                                 class Args_04,
                                 class Args_05,
                                 class Args_06,
                                 class Args_07>
    static void construct(TARGET_TYPE      *address,
                          bsl::integral_constant<int, e_NIL_TRAITS> *,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07);

    template <class TARGET_TYPE, class Args_01,
                                 class Args_02,
                                 class Args_03,
                                 class Args_04,
                                 class Args_05,
                                 class Args_06,
                                 class Args_07,
                                 class Args_08>
    static void construct(TARGET_TYPE      *address,
                          bsl::integral_constant<int, e_NIL_TRAITS> *,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) arguments_08);

    template <class TARGET_TYPE, class Args_01,
                                 class Args_02,
                                 class Args_03,
                                 class Args_04,
                                 class Args_05,
                                 class Args_06,
                                 class Args_07,
                                 class Args_08,
                                 class Args_09>
    static void construct(TARGET_TYPE      *address,
                          bsl::integral_constant<int, e_NIL_TRAITS> *,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) arguments_08,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_09) arguments_09);

    template <class TARGET_TYPE, class Args_01,
                                 class Args_02,
                                 class Args_03,
                                 class Args_04,
                                 class Args_05,
                                 class Args_06,
                                 class Args_07,
                                 class Args_08,
                                 class Args_09,
                                 class Args_10>
    static void construct(TARGET_TYPE      *address,
                          bsl::integral_constant<int, e_NIL_TRAITS> *,
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

    template <class TARGET_TYPE, class Args_01,
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
    static void construct(TARGET_TYPE      *address,
                          bsl::integral_constant<int, e_NIL_TRAITS> *,
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

    template <class TARGET_TYPE, class Args_01,
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
    static void construct(TARGET_TYPE      *address,
                          bsl::integral_constant<int, e_NIL_TRAITS> *,
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

    template <class TARGET_TYPE, class Args_01,
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
    static void construct(TARGET_TYPE      *address,
                          bsl::integral_constant<int, e_NIL_TRAITS> *,
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

    template <class TARGET_TYPE, class Args_01,
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
    static void construct(TARGET_TYPE      *address,
                          bsl::integral_constant<int, e_NIL_TRAITS> *,
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

    template <class TARGET_TYPE, class Args_01,
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
                                 class Args_14,
                                 class Args_15>
    static void construct(TARGET_TYPE      *address,
                          bsl::integral_constant<int, e_NIL_TRAITS> *,
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
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_14) arguments_14,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_15) arguments_15);

#else
// The generated code below is a workaround for the absence of perfect
// forwarding in some compilers.
    template <class TARGET_TYPE, class... Args>
    static void construct(
                  TARGET_TYPE      *address,
                  bslma::Allocator *allocator,
                  bsl::integral_constant<int, e_USES_BSLMA_ALLOCATOR_TRAITS> *,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args)... arguments);
    template <class TARGET_TYPE, class... Args>
    static void construct(
                  TARGET_TYPE      *address,
                  bslma::Allocator *allocator,
                  bsl::integral_constant<int, e_USES_ALLOCATOR_ARG_T_TRAITS> *,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args)... arguments);

    template <class TARGET_TYPE, class... Args>
    static void construct(TARGET_TYPE      *address,
                          bslma::Allocator *allocator,
                          bsl::integral_constant<int, e_NIL_TRAITS> *,
                         BSLS_COMPILERFEATURES_FORWARD_REF(Args)... arguments);

    template <class TARGET_TYPE, class... Args>
    static void construct(TARGET_TYPE      *address,
                          bsl::integral_constant<int, e_NIL_TRAITS> *,
                         BSLS_COMPILERFEATURES_FORWARD_REF(Args)... arguments);
// }}} END GENERATED CODE
#endif

#if defined(BSLS_PLATFORM_CMP_MSVC) && BSLS_PLATFORM_CMP_VERSION < 1900
    template <class TARGET_TYPE>
    static void defaultConstructScalar(bsl::false_type, TARGET_TYPE *address);
        // Value-initialize a scalar object of the (template parameter) type
        // 'TARGET_TYPE' at the specifified 'address'.  The unused
        // 'bsl::false_type' value indicates that the scalar 'TARGET_TYPE' is
        // not a pointer-to-member.

    template <class TARGET_TYPE>
    static void defaultConstructScalar(bsl::true_type, TARGET_TYPE *address);
        // Value-initialize a pointer-to-member at the specified 'address' to
        // the null pointer value.  Note that early version of the Microsoft
        // Visual C++ compiler would fail to initialize such an object when
        // requested with the simple value-initialization syntax of
        // 'new (address) TYPE();', requiring this additional workaround.
#endif

    template <class TARGET_TYPE, class ALLOCATOR>
    static void destructiveMove(
                       TARGET_TYPE *address,
                       ALLOCATOR   *allocator,
                       bsl::integral_constant<int, e_BITWISE_MOVABLE_TRAITS> *,
                       TARGET_TYPE *original);
        // Move the bitwise movable object of (template parameter) type
        // 'TARGET_TYPE' at the specified 'original' address to the specified
        // 'address', eliding the call to the move constructor and destructor
        // in favor of performing a bitwise copy.  The behavior is undefined
        // unless either 'TARGET_TYPE' does not support 'bslma'-style
        // allocation or 'original' uses the specified 'allocator' to supply
        // memory.

    template <class TARGET_TYPE, class ALLOCATOR>
    static void destructiveMove(TARGET_TYPE *address,
                                ALLOCATOR   *allocator,
                                bsl::integral_constant<int, e_NIL_TRAITS> *,
                                TARGET_TYPE *original);
        // Create an object of (template parameter) type 'TARGET_TYPE' at the
        // specified 'address' having the same value as the object at the
        // specified 'original' address, propagating the specified 'allocator'
        // to the moved object if 'TARGET_TYPE' uses 'bslma'-style allocation
        // and the (template parameter) type 'ALLOCATOR' is implicitly
        // convertible to 'bslma::Allocator', and destroy 'original'.  If the
        // move constructor throws an exception, the memory at 'address' is
        // left in an unspecified state and 'original' is left in a valid but
        // unspecified state.  The behavior is undefined unless either
        // 'TARGET_TYPE' does not support 'bslma'-style allocation or
        // 'original' uses 'allocator' to supply memory.  Note that this class
        // method is equivalent to move-constructing an object at 'address'
        // from '*original' and then destroying 'original'.  Also note that if
        // 'original' actually points to an object of a type derived from
        // 'TARGET_TYPE' (i.e., a slicing move) where 'TARGET_TYPE' has a
        // non-'virtual' destructor, then 'original' will be only partially
        // destroyed.

    template <class TARGET_TYPE>
    static void *voidify(TARGET_TYPE *address);
        // Return the specified 'address' cast as a pointer to 'void', even if
        // the (template parameter) 'TARGET_TYPE' is cv-qualified.
};

// ============================================================================
//                      TEMPLATE FUNCTION DEFINITIONS
// ============================================================================

                          // -----------------------
                          // struct ConstructionUtil
                          // -----------------------

// CLASS METHODS
template <class TARGET_TYPE>
inline
void
ConstructionUtil::construct(TARGET_TYPE      *address,
                            bslma::Allocator *allocator)
{
    enum {
        k_VALUE = bslma::UsesBslmaAllocator<TARGET_TYPE>::value
                ? (bslmf::UsesAllocatorArgT<TARGET_TYPE>::value
                 ? Imp::e_USES_ALLOCATOR_ARG_T_TRAITS
                 : Imp::e_USES_BSLMA_ALLOCATOR_TRAITS)
                : bsl::is_trivially_default_constructible<TARGET_TYPE>::value
                    ? Imp::e_HAS_TRIVIAL_DEFAULT_CTOR_TRAITS
                    : Imp::e_NIL_TRAITS
    };
    Imp::construct(address,
                   allocator,
                   (bsl::integral_constant<int, k_VALUE>*)0);
}

template <class TARGET_TYPE>
inline
void
ConstructionUtil::construct(TARGET_TYPE *address,
                            void        *)
{
    enum {
        k_VALUE = bsl::is_trivially_default_constructible<TARGET_TYPE>::value
                ? Imp::e_HAS_TRIVIAL_DEFAULT_CTOR_TRAITS
                : Imp::e_NIL_TRAITS
    };
    Imp::construct(address, (bsl::integral_constant<int, k_VALUE>*)0);
}

template <class TARGET_TYPE>
inline
void ConstructionUtil::construct(TARGET_TYPE        *address,
                                 bslma::Allocator   *allocator,
                                 const TARGET_TYPE&  original)
{
    enum {
        k_VALUE = bslma::UsesBslmaAllocator<TARGET_TYPE>::value
                ? (bslmf::UsesAllocatorArgT<TARGET_TYPE>::value
                 ? Imp::e_USES_ALLOCATOR_ARG_T_TRAITS
                 : Imp::e_USES_BSLMA_ALLOCATOR_TRAITS)
                : bsl::is_trivially_copyable<TARGET_TYPE>::value
                    ? Imp::e_BITWISE_COPYABLE_TRAITS
                    : Imp::e_NIL_TRAITS
    };
    Imp::construct(address,
                   allocator,
                   (bsl::integral_constant<int, k_VALUE> *)0,
                   original);
}

template <class TARGET_TYPE>
inline
void ConstructionUtil::construct(TARGET_TYPE      *address,
                                 bslma::Allocator *allocator,
                                 TARGET_TYPE&      original)
{
    enum {
        k_VALUE = bslma::UsesBslmaAllocator<TARGET_TYPE>::value
                ? (bslmf::UsesAllocatorArgT<TARGET_TYPE>::value
                 ? Imp::e_USES_ALLOCATOR_ARG_T_TRAITS
                 : Imp::e_USES_BSLMA_ALLOCATOR_TRAITS)
                : bsl::is_trivially_copyable<TARGET_TYPE>::value
                    ? Imp::e_BITWISE_COPYABLE_TRAITS
                    : Imp::e_NIL_TRAITS
    };
    Imp::construct(address,
                   allocator,
                   (bsl::integral_constant<int, k_VALUE> *)0,
                   original);
}

template <class TARGET_TYPE>
inline
void ConstructionUtil::construct(TARGET_TYPE        *address,
                                 void               *,
                                 const TARGET_TYPE&  original)
{
    enum {
        k_VALUE = bsl::is_trivially_copyable<TARGET_TYPE>::value
                ? Imp::e_BITWISE_COPYABLE_TRAITS
                : Imp::e_NIL_TRAITS
    };
    Imp::construct(address,
                   (bsl::integral_constant<int, k_VALUE>*)0,
                   original);
}

template <class TARGET_TYPE>
inline
void ConstructionUtil::construct(TARGET_TYPE  *address,
                                 void         *,
                                 TARGET_TYPE&  original)
{
    enum {
        k_VALUE = bsl::is_trivially_copyable<TARGET_TYPE>::value
                ? Imp::e_BITWISE_COPYABLE_TRAITS
                : Imp::e_NIL_TRAITS
    };
    Imp::construct(address,
                   (bsl::integral_constant<int, k_VALUE>*)0,
                   original);
}

template <class TARGET_TYPE>
inline
void ConstructionUtil::construct(TARGET_TYPE                    *address,
                                 bslma::Allocator               *allocator,
                                 bslmf::MovableRef<TARGET_TYPE>  original)
{
    enum {
        k_VALUE = bslma::UsesBslmaAllocator<TARGET_TYPE>::value
                ? (bslmf::UsesAllocatorArgT<TARGET_TYPE>::value
                 ? Imp::e_USES_ALLOCATOR_ARG_T_TRAITS
                 : Imp::e_USES_BSLMA_ALLOCATOR_TRAITS)
                : bsl::is_trivially_copyable<TARGET_TYPE>::value
                 ? Imp::e_BITWISE_COPYABLE_TRAITS
                 : Imp::e_NIL_TRAITS
    };
    Imp::construct(address,
                   allocator,
                   (bsl::integral_constant<int, k_VALUE> *)0,
                   BSLS_COMPILERFEATURES_FORWARD(TARGET_TYPE, original));
}

template <class TARGET_TYPE>
inline
void ConstructionUtil::construct(TARGET_TYPE                    *address,
                                 void                           *,
                                 bslmf::MovableRef<TARGET_TYPE>  original)
{

    enum {
        k_VALUE = bsl::is_trivially_copyable<TARGET_TYPE>::value
                ? Imp::e_BITWISE_COPYABLE_TRAITS
                : Imp::e_NIL_TRAITS
    };

    Imp::construct(address,
                   (bsl::integral_constant<int, k_VALUE>*)0,
                   BSLS_COMPILERFEATURES_FORWARD(TARGET_TYPE, original));
}

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES // $var-args=15
template <class TARGET_TYPE, class Arg1, class... Args>
inline
void
ConstructionUtil::construct(TARGET_TYPE      *address,
                            bslma::Allocator *allocator,
                            Arg1&&            argument1,
                            Args&&...         arguments)
{
    enum {
        k_VALUE = bslma::UsesBslmaAllocator<TARGET_TYPE>::value
                ? (bslmf::UsesAllocatorArgT<TARGET_TYPE>::value
                 ? Imp::e_USES_ALLOCATOR_ARG_T_TRAITS
                 : Imp::e_USES_BSLMA_ALLOCATOR_TRAITS)
                : Imp::e_NIL_TRAITS
    };
    Imp::construct(address,
                   allocator,
                   (bsl::integral_constant<int, k_VALUE>*)0,
                   BSLS_COMPILERFEATURES_FORWARD(Arg1,argument1),
                   BSLS_COMPILERFEATURES_FORWARD(Args,arguments)...);
}

template <class TARGET_TYPE, class Arg1, class... Args>
inline
void
ConstructionUtil::construct(TARGET_TYPE *address,
                            void        *,
                            Arg1&&       argument1,
                            Args&&...    arguments)
{
    ::new (Imp::voidify(address)) TARGET_TYPE(
                             BSLS_COMPILERFEATURES_FORWARD(Arg1,argument1),
                             BSLS_COMPILERFEATURES_FORWARD(Args,arguments)...);
    BSLMA_CONSTRUCTIONUTIL_XLC_PLACEMENT_NEW_FIX;
}
#elif BSLS_COMPILERFEATURES_SIMULATE_VARIADIC_TEMPLATES
// {{{ BEGIN GENERATED CODE
// The following section is automatically generated.  **DO NOT EDIT**
// Generator command line: sim_cpp11_features.pl bslma_constructionutil.h
template <class TARGET_TYPE, class Arg1>
inline
void
ConstructionUtil::construct(TARGET_TYPE      *address,
                            bslma::Allocator *allocator,
                            BSLS_COMPILERFEATURES_FORWARD_REF(Arg1) argument1)
{
    enum {
        k_VALUE = bslma::UsesBslmaAllocator<TARGET_TYPE>::value
                ? (bslmf::UsesAllocatorArgT<TARGET_TYPE>::value
                 ? Imp::e_USES_ALLOCATOR_ARG_T_TRAITS
                 : Imp::e_USES_BSLMA_ALLOCATOR_TRAITS)
                : Imp::e_NIL_TRAITS
    };
    Imp::construct(address,
                   allocator,
                   (bsl::integral_constant<int, k_VALUE>*)0,
                   BSLS_COMPILERFEATURES_FORWARD(Arg1,argument1));
}

template <class TARGET_TYPE, class Arg1, class Args_01>
inline
void
ConstructionUtil::construct(TARGET_TYPE      *address,
                            bslma::Allocator *allocator,
                            BSLS_COMPILERFEATURES_FORWARD_REF(Arg1) argument1,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01)
{
    enum {
        k_VALUE = bslma::UsesBslmaAllocator<TARGET_TYPE>::value
                ? (bslmf::UsesAllocatorArgT<TARGET_TYPE>::value
                 ? Imp::e_USES_ALLOCATOR_ARG_T_TRAITS
                 : Imp::e_USES_BSLMA_ALLOCATOR_TRAITS)
                : Imp::e_NIL_TRAITS
    };
    Imp::construct(address,
                   allocator,
                   (bsl::integral_constant<int, k_VALUE>*)0,
                   BSLS_COMPILERFEATURES_FORWARD(Arg1,argument1),
                   BSLS_COMPILERFEATURES_FORWARD(Args_01,arguments_01));
}

template <class TARGET_TYPE, class Arg1, class Args_01,
                                         class Args_02>
inline
void
ConstructionUtil::construct(TARGET_TYPE      *address,
                            bslma::Allocator *allocator,
                            BSLS_COMPILERFEATURES_FORWARD_REF(Arg1) argument1,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02)
{
    enum {
        k_VALUE = bslma::UsesBslmaAllocator<TARGET_TYPE>::value
                ? (bslmf::UsesAllocatorArgT<TARGET_TYPE>::value
                 ? Imp::e_USES_ALLOCATOR_ARG_T_TRAITS
                 : Imp::e_USES_BSLMA_ALLOCATOR_TRAITS)
                : Imp::e_NIL_TRAITS
    };
    Imp::construct(address,
                   allocator,
                   (bsl::integral_constant<int, k_VALUE>*)0,
                   BSLS_COMPILERFEATURES_FORWARD(Arg1,argument1),
                   BSLS_COMPILERFEATURES_FORWARD(Args_01,arguments_01),
                   BSLS_COMPILERFEATURES_FORWARD(Args_02,arguments_02));
}

template <class TARGET_TYPE, class Arg1, class Args_01,
                                         class Args_02,
                                         class Args_03>
inline
void
ConstructionUtil::construct(TARGET_TYPE      *address,
                            bslma::Allocator *allocator,
                            BSLS_COMPILERFEATURES_FORWARD_REF(Arg1) argument1,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03)
{
    enum {
        k_VALUE = bslma::UsesBslmaAllocator<TARGET_TYPE>::value
                ? (bslmf::UsesAllocatorArgT<TARGET_TYPE>::value
                 ? Imp::e_USES_ALLOCATOR_ARG_T_TRAITS
                 : Imp::e_USES_BSLMA_ALLOCATOR_TRAITS)
                : Imp::e_NIL_TRAITS
    };
    Imp::construct(address,
                   allocator,
                   (bsl::integral_constant<int, k_VALUE>*)0,
                   BSLS_COMPILERFEATURES_FORWARD(Arg1,argument1),
                   BSLS_COMPILERFEATURES_FORWARD(Args_01,arguments_01),
                   BSLS_COMPILERFEATURES_FORWARD(Args_02,arguments_02),
                   BSLS_COMPILERFEATURES_FORWARD(Args_03,arguments_03));
}

template <class TARGET_TYPE, class Arg1, class Args_01,
                                         class Args_02,
                                         class Args_03,
                                         class Args_04>
inline
void
ConstructionUtil::construct(TARGET_TYPE      *address,
                            bslma::Allocator *allocator,
                            BSLS_COMPILERFEATURES_FORWARD_REF(Arg1) argument1,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04)
{
    enum {
        k_VALUE = bslma::UsesBslmaAllocator<TARGET_TYPE>::value
                ? (bslmf::UsesAllocatorArgT<TARGET_TYPE>::value
                 ? Imp::e_USES_ALLOCATOR_ARG_T_TRAITS
                 : Imp::e_USES_BSLMA_ALLOCATOR_TRAITS)
                : Imp::e_NIL_TRAITS
    };
    Imp::construct(address,
                   allocator,
                   (bsl::integral_constant<int, k_VALUE>*)0,
                   BSLS_COMPILERFEATURES_FORWARD(Arg1,argument1),
                   BSLS_COMPILERFEATURES_FORWARD(Args_01,arguments_01),
                   BSLS_COMPILERFEATURES_FORWARD(Args_02,arguments_02),
                   BSLS_COMPILERFEATURES_FORWARD(Args_03,arguments_03),
                   BSLS_COMPILERFEATURES_FORWARD(Args_04,arguments_04));
}

template <class TARGET_TYPE, class Arg1, class Args_01,
                                         class Args_02,
                                         class Args_03,
                                         class Args_04,
                                         class Args_05>
inline
void
ConstructionUtil::construct(TARGET_TYPE      *address,
                            bslma::Allocator *allocator,
                            BSLS_COMPILERFEATURES_FORWARD_REF(Arg1) argument1,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05)
{
    enum {
        k_VALUE = bslma::UsesBslmaAllocator<TARGET_TYPE>::value
                ? (bslmf::UsesAllocatorArgT<TARGET_TYPE>::value
                 ? Imp::e_USES_ALLOCATOR_ARG_T_TRAITS
                 : Imp::e_USES_BSLMA_ALLOCATOR_TRAITS)
                : Imp::e_NIL_TRAITS
    };
    Imp::construct(address,
                   allocator,
                   (bsl::integral_constant<int, k_VALUE>*)0,
                   BSLS_COMPILERFEATURES_FORWARD(Arg1,argument1),
                   BSLS_COMPILERFEATURES_FORWARD(Args_01,arguments_01),
                   BSLS_COMPILERFEATURES_FORWARD(Args_02,arguments_02),
                   BSLS_COMPILERFEATURES_FORWARD(Args_03,arguments_03),
                   BSLS_COMPILERFEATURES_FORWARD(Args_04,arguments_04),
                   BSLS_COMPILERFEATURES_FORWARD(Args_05,arguments_05));
}

template <class TARGET_TYPE, class Arg1, class Args_01,
                                         class Args_02,
                                         class Args_03,
                                         class Args_04,
                                         class Args_05,
                                         class Args_06>
inline
void
ConstructionUtil::construct(TARGET_TYPE      *address,
                            bslma::Allocator *allocator,
                            BSLS_COMPILERFEATURES_FORWARD_REF(Arg1) argument1,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06)
{
    enum {
        k_VALUE = bslma::UsesBslmaAllocator<TARGET_TYPE>::value
                ? (bslmf::UsesAllocatorArgT<TARGET_TYPE>::value
                 ? Imp::e_USES_ALLOCATOR_ARG_T_TRAITS
                 : Imp::e_USES_BSLMA_ALLOCATOR_TRAITS)
                : Imp::e_NIL_TRAITS
    };
    Imp::construct(address,
                   allocator,
                   (bsl::integral_constant<int, k_VALUE>*)0,
                   BSLS_COMPILERFEATURES_FORWARD(Arg1,argument1),
                   BSLS_COMPILERFEATURES_FORWARD(Args_01,arguments_01),
                   BSLS_COMPILERFEATURES_FORWARD(Args_02,arguments_02),
                   BSLS_COMPILERFEATURES_FORWARD(Args_03,arguments_03),
                   BSLS_COMPILERFEATURES_FORWARD(Args_04,arguments_04),
                   BSLS_COMPILERFEATURES_FORWARD(Args_05,arguments_05),
                   BSLS_COMPILERFEATURES_FORWARD(Args_06,arguments_06));
}

template <class TARGET_TYPE, class Arg1, class Args_01,
                                         class Args_02,
                                         class Args_03,
                                         class Args_04,
                                         class Args_05,
                                         class Args_06,
                                         class Args_07>
inline
void
ConstructionUtil::construct(TARGET_TYPE      *address,
                            bslma::Allocator *allocator,
                            BSLS_COMPILERFEATURES_FORWARD_REF(Arg1) argument1,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07)
{
    enum {
        k_VALUE = bslma::UsesBslmaAllocator<TARGET_TYPE>::value
                ? (bslmf::UsesAllocatorArgT<TARGET_TYPE>::value
                 ? Imp::e_USES_ALLOCATOR_ARG_T_TRAITS
                 : Imp::e_USES_BSLMA_ALLOCATOR_TRAITS)
                : Imp::e_NIL_TRAITS
    };
    Imp::construct(address,
                   allocator,
                   (bsl::integral_constant<int, k_VALUE>*)0,
                   BSLS_COMPILERFEATURES_FORWARD(Arg1,argument1),
                   BSLS_COMPILERFEATURES_FORWARD(Args_01,arguments_01),
                   BSLS_COMPILERFEATURES_FORWARD(Args_02,arguments_02),
                   BSLS_COMPILERFEATURES_FORWARD(Args_03,arguments_03),
                   BSLS_COMPILERFEATURES_FORWARD(Args_04,arguments_04),
                   BSLS_COMPILERFEATURES_FORWARD(Args_05,arguments_05),
                   BSLS_COMPILERFEATURES_FORWARD(Args_06,arguments_06),
                   BSLS_COMPILERFEATURES_FORWARD(Args_07,arguments_07));
}

template <class TARGET_TYPE, class Arg1, class Args_01,
                                         class Args_02,
                                         class Args_03,
                                         class Args_04,
                                         class Args_05,
                                         class Args_06,
                                         class Args_07,
                                         class Args_08>
inline
void
ConstructionUtil::construct(TARGET_TYPE      *address,
                            bslma::Allocator *allocator,
                            BSLS_COMPILERFEATURES_FORWARD_REF(Arg1) argument1,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) arguments_08)
{
    enum {
        k_VALUE = bslma::UsesBslmaAllocator<TARGET_TYPE>::value
                ? (bslmf::UsesAllocatorArgT<TARGET_TYPE>::value
                 ? Imp::e_USES_ALLOCATOR_ARG_T_TRAITS
                 : Imp::e_USES_BSLMA_ALLOCATOR_TRAITS)
                : Imp::e_NIL_TRAITS
    };
    Imp::construct(address,
                   allocator,
                   (bsl::integral_constant<int, k_VALUE>*)0,
                   BSLS_COMPILERFEATURES_FORWARD(Arg1,argument1),
                   BSLS_COMPILERFEATURES_FORWARD(Args_01,arguments_01),
                   BSLS_COMPILERFEATURES_FORWARD(Args_02,arguments_02),
                   BSLS_COMPILERFEATURES_FORWARD(Args_03,arguments_03),
                   BSLS_COMPILERFEATURES_FORWARD(Args_04,arguments_04),
                   BSLS_COMPILERFEATURES_FORWARD(Args_05,arguments_05),
                   BSLS_COMPILERFEATURES_FORWARD(Args_06,arguments_06),
                   BSLS_COMPILERFEATURES_FORWARD(Args_07,arguments_07),
                   BSLS_COMPILERFEATURES_FORWARD(Args_08,arguments_08));
}

template <class TARGET_TYPE, class Arg1, class Args_01,
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
ConstructionUtil::construct(TARGET_TYPE      *address,
                            bslma::Allocator *allocator,
                            BSLS_COMPILERFEATURES_FORWARD_REF(Arg1) argument1,
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
    enum {
        k_VALUE = bslma::UsesBslmaAllocator<TARGET_TYPE>::value
                ? (bslmf::UsesAllocatorArgT<TARGET_TYPE>::value
                 ? Imp::e_USES_ALLOCATOR_ARG_T_TRAITS
                 : Imp::e_USES_BSLMA_ALLOCATOR_TRAITS)
                : Imp::e_NIL_TRAITS
    };
    Imp::construct(address,
                   allocator,
                   (bsl::integral_constant<int, k_VALUE>*)0,
                   BSLS_COMPILERFEATURES_FORWARD(Arg1,argument1),
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

template <class TARGET_TYPE, class Arg1, class Args_01,
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
ConstructionUtil::construct(TARGET_TYPE      *address,
                            bslma::Allocator *allocator,
                            BSLS_COMPILERFEATURES_FORWARD_REF(Arg1) argument1,
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
    enum {
        k_VALUE = bslma::UsesBslmaAllocator<TARGET_TYPE>::value
                ? (bslmf::UsesAllocatorArgT<TARGET_TYPE>::value
                 ? Imp::e_USES_ALLOCATOR_ARG_T_TRAITS
                 : Imp::e_USES_BSLMA_ALLOCATOR_TRAITS)
                : Imp::e_NIL_TRAITS
    };
    Imp::construct(address,
                   allocator,
                   (bsl::integral_constant<int, k_VALUE>*)0,
                   BSLS_COMPILERFEATURES_FORWARD(Arg1,argument1),
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

template <class TARGET_TYPE, class Arg1, class Args_01,
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
ConstructionUtil::construct(TARGET_TYPE      *address,
                            bslma::Allocator *allocator,
                            BSLS_COMPILERFEATURES_FORWARD_REF(Arg1) argument1,
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
    enum {
        k_VALUE = bslma::UsesBslmaAllocator<TARGET_TYPE>::value
                ? (bslmf::UsesAllocatorArgT<TARGET_TYPE>::value
                 ? Imp::e_USES_ALLOCATOR_ARG_T_TRAITS
                 : Imp::e_USES_BSLMA_ALLOCATOR_TRAITS)
                : Imp::e_NIL_TRAITS
    };
    Imp::construct(address,
                   allocator,
                   (bsl::integral_constant<int, k_VALUE>*)0,
                   BSLS_COMPILERFEATURES_FORWARD(Arg1,argument1),
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

template <class TARGET_TYPE, class Arg1, class Args_01,
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
ConstructionUtil::construct(TARGET_TYPE      *address,
                            bslma::Allocator *allocator,
                            BSLS_COMPILERFEATURES_FORWARD_REF(Arg1) argument1,
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
    enum {
        k_VALUE = bslma::UsesBslmaAllocator<TARGET_TYPE>::value
                ? (bslmf::UsesAllocatorArgT<TARGET_TYPE>::value
                 ? Imp::e_USES_ALLOCATOR_ARG_T_TRAITS
                 : Imp::e_USES_BSLMA_ALLOCATOR_TRAITS)
                : Imp::e_NIL_TRAITS
    };
    Imp::construct(address,
                   allocator,
                   (bsl::integral_constant<int, k_VALUE>*)0,
                   BSLS_COMPILERFEATURES_FORWARD(Arg1,argument1),
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

template <class TARGET_TYPE, class Arg1, class Args_01,
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
ConstructionUtil::construct(TARGET_TYPE      *address,
                            bslma::Allocator *allocator,
                            BSLS_COMPILERFEATURES_FORWARD_REF(Arg1) argument1,
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
    enum {
        k_VALUE = bslma::UsesBslmaAllocator<TARGET_TYPE>::value
                ? (bslmf::UsesAllocatorArgT<TARGET_TYPE>::value
                 ? Imp::e_USES_ALLOCATOR_ARG_T_TRAITS
                 : Imp::e_USES_BSLMA_ALLOCATOR_TRAITS)
                : Imp::e_NIL_TRAITS
    };
    Imp::construct(address,
                   allocator,
                   (bsl::integral_constant<int, k_VALUE>*)0,
                   BSLS_COMPILERFEATURES_FORWARD(Arg1,argument1),
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

template <class TARGET_TYPE, class Arg1, class Args_01,
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
ConstructionUtil::construct(TARGET_TYPE      *address,
                            bslma::Allocator *allocator,
                            BSLS_COMPILERFEATURES_FORWARD_REF(Arg1) argument1,
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
    enum {
        k_VALUE = bslma::UsesBslmaAllocator<TARGET_TYPE>::value
                ? (bslmf::UsesAllocatorArgT<TARGET_TYPE>::value
                 ? Imp::e_USES_ALLOCATOR_ARG_T_TRAITS
                 : Imp::e_USES_BSLMA_ALLOCATOR_TRAITS)
                : Imp::e_NIL_TRAITS
    };
    Imp::construct(address,
                   allocator,
                   (bsl::integral_constant<int, k_VALUE>*)0,
                   BSLS_COMPILERFEATURES_FORWARD(Arg1,argument1),
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

template <class TARGET_TYPE, class Arg1, class Args_01,
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
                                         class Args_14,
                                         class Args_15>
inline
void
ConstructionUtil::construct(TARGET_TYPE      *address,
                            bslma::Allocator *allocator,
                            BSLS_COMPILERFEATURES_FORWARD_REF(Arg1) argument1,
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
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_14) arguments_14,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_15) arguments_15)
{
    enum {
        k_VALUE = bslma::UsesBslmaAllocator<TARGET_TYPE>::value
                ? (bslmf::UsesAllocatorArgT<TARGET_TYPE>::value
                 ? Imp::e_USES_ALLOCATOR_ARG_T_TRAITS
                 : Imp::e_USES_BSLMA_ALLOCATOR_TRAITS)
                : Imp::e_NIL_TRAITS
    };
    Imp::construct(address,
                   allocator,
                   (bsl::integral_constant<int, k_VALUE>*)0,
                   BSLS_COMPILERFEATURES_FORWARD(Arg1,argument1),
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
                   BSLS_COMPILERFEATURES_FORWARD(Args_14,arguments_14),
                   BSLS_COMPILERFEATURES_FORWARD(Args_15,arguments_15));
}

template <class TARGET_TYPE, class Arg1>
inline
void
ConstructionUtil::construct(TARGET_TYPE *address,
                            void        *,
                            BSLS_COMPILERFEATURES_FORWARD_REF(Arg1) argument1)
{
    ::new (Imp::voidify(address)) TARGET_TYPE(
                             BSLS_COMPILERFEATURES_FORWARD(Arg1,argument1));
    BSLMA_CONSTRUCTIONUTIL_XLC_PLACEMENT_NEW_FIX;
}

template <class TARGET_TYPE, class Arg1, class Args_01>
inline
void
ConstructionUtil::construct(TARGET_TYPE *address,
                            void        *,
                            BSLS_COMPILERFEATURES_FORWARD_REF(Arg1) argument1,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01)
{
    ::new (Imp::voidify(address)) TARGET_TYPE(
                             BSLS_COMPILERFEATURES_FORWARD(Arg1,argument1),
                          BSLS_COMPILERFEATURES_FORWARD(Args_01,arguments_01));
    BSLMA_CONSTRUCTIONUTIL_XLC_PLACEMENT_NEW_FIX;
}

template <class TARGET_TYPE, class Arg1, class Args_01,
                                         class Args_02>
inline
void
ConstructionUtil::construct(TARGET_TYPE *address,
                            void        *,
                            BSLS_COMPILERFEATURES_FORWARD_REF(Arg1) argument1,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02)
{
    ::new (Imp::voidify(address)) TARGET_TYPE(
                             BSLS_COMPILERFEATURES_FORWARD(Arg1,argument1),
                          BSLS_COMPILERFEATURES_FORWARD(Args_01,arguments_01),
                          BSLS_COMPILERFEATURES_FORWARD(Args_02,arguments_02));
    BSLMA_CONSTRUCTIONUTIL_XLC_PLACEMENT_NEW_FIX;
}

template <class TARGET_TYPE, class Arg1, class Args_01,
                                         class Args_02,
                                         class Args_03>
inline
void
ConstructionUtil::construct(TARGET_TYPE *address,
                            void        *,
                            BSLS_COMPILERFEATURES_FORWARD_REF(Arg1) argument1,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03)
{
    ::new (Imp::voidify(address)) TARGET_TYPE(
                             BSLS_COMPILERFEATURES_FORWARD(Arg1,argument1),
                          BSLS_COMPILERFEATURES_FORWARD(Args_01,arguments_01),
                          BSLS_COMPILERFEATURES_FORWARD(Args_02,arguments_02),
                          BSLS_COMPILERFEATURES_FORWARD(Args_03,arguments_03));
    BSLMA_CONSTRUCTIONUTIL_XLC_PLACEMENT_NEW_FIX;
}

template <class TARGET_TYPE, class Arg1, class Args_01,
                                         class Args_02,
                                         class Args_03,
                                         class Args_04>
inline
void
ConstructionUtil::construct(TARGET_TYPE *address,
                            void        *,
                            BSLS_COMPILERFEATURES_FORWARD_REF(Arg1) argument1,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04)
{
    ::new (Imp::voidify(address)) TARGET_TYPE(
                             BSLS_COMPILERFEATURES_FORWARD(Arg1,argument1),
                          BSLS_COMPILERFEATURES_FORWARD(Args_01,arguments_01),
                          BSLS_COMPILERFEATURES_FORWARD(Args_02,arguments_02),
                          BSLS_COMPILERFEATURES_FORWARD(Args_03,arguments_03),
                          BSLS_COMPILERFEATURES_FORWARD(Args_04,arguments_04));
    BSLMA_CONSTRUCTIONUTIL_XLC_PLACEMENT_NEW_FIX;
}

template <class TARGET_TYPE, class Arg1, class Args_01,
                                         class Args_02,
                                         class Args_03,
                                         class Args_04,
                                         class Args_05>
inline
void
ConstructionUtil::construct(TARGET_TYPE *address,
                            void        *,
                            BSLS_COMPILERFEATURES_FORWARD_REF(Arg1) argument1,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05)
{
    ::new (Imp::voidify(address)) TARGET_TYPE(
                             BSLS_COMPILERFEATURES_FORWARD(Arg1,argument1),
                          BSLS_COMPILERFEATURES_FORWARD(Args_01,arguments_01),
                          BSLS_COMPILERFEATURES_FORWARD(Args_02,arguments_02),
                          BSLS_COMPILERFEATURES_FORWARD(Args_03,arguments_03),
                          BSLS_COMPILERFEATURES_FORWARD(Args_04,arguments_04),
                          BSLS_COMPILERFEATURES_FORWARD(Args_05,arguments_05));
    BSLMA_CONSTRUCTIONUTIL_XLC_PLACEMENT_NEW_FIX;
}

template <class TARGET_TYPE, class Arg1, class Args_01,
                                         class Args_02,
                                         class Args_03,
                                         class Args_04,
                                         class Args_05,
                                         class Args_06>
inline
void
ConstructionUtil::construct(TARGET_TYPE *address,
                            void        *,
                            BSLS_COMPILERFEATURES_FORWARD_REF(Arg1) argument1,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06)
{
    ::new (Imp::voidify(address)) TARGET_TYPE(
                             BSLS_COMPILERFEATURES_FORWARD(Arg1,argument1),
                          BSLS_COMPILERFEATURES_FORWARD(Args_01,arguments_01),
                          BSLS_COMPILERFEATURES_FORWARD(Args_02,arguments_02),
                          BSLS_COMPILERFEATURES_FORWARD(Args_03,arguments_03),
                          BSLS_COMPILERFEATURES_FORWARD(Args_04,arguments_04),
                          BSLS_COMPILERFEATURES_FORWARD(Args_05,arguments_05),
                          BSLS_COMPILERFEATURES_FORWARD(Args_06,arguments_06));
    BSLMA_CONSTRUCTIONUTIL_XLC_PLACEMENT_NEW_FIX;
}

template <class TARGET_TYPE, class Arg1, class Args_01,
                                         class Args_02,
                                         class Args_03,
                                         class Args_04,
                                         class Args_05,
                                         class Args_06,
                                         class Args_07>
inline
void
ConstructionUtil::construct(TARGET_TYPE *address,
                            void        *,
                            BSLS_COMPILERFEATURES_FORWARD_REF(Arg1) argument1,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07)
{
    ::new (Imp::voidify(address)) TARGET_TYPE(
                             BSLS_COMPILERFEATURES_FORWARD(Arg1,argument1),
                          BSLS_COMPILERFEATURES_FORWARD(Args_01,arguments_01),
                          BSLS_COMPILERFEATURES_FORWARD(Args_02,arguments_02),
                          BSLS_COMPILERFEATURES_FORWARD(Args_03,arguments_03),
                          BSLS_COMPILERFEATURES_FORWARD(Args_04,arguments_04),
                          BSLS_COMPILERFEATURES_FORWARD(Args_05,arguments_05),
                          BSLS_COMPILERFEATURES_FORWARD(Args_06,arguments_06),
                          BSLS_COMPILERFEATURES_FORWARD(Args_07,arguments_07));
    BSLMA_CONSTRUCTIONUTIL_XLC_PLACEMENT_NEW_FIX;
}

template <class TARGET_TYPE, class Arg1, class Args_01,
                                         class Args_02,
                                         class Args_03,
                                         class Args_04,
                                         class Args_05,
                                         class Args_06,
                                         class Args_07,
                                         class Args_08>
inline
void
ConstructionUtil::construct(TARGET_TYPE *address,
                            void        *,
                            BSLS_COMPILERFEATURES_FORWARD_REF(Arg1) argument1,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) arguments_08)
{
    ::new (Imp::voidify(address)) TARGET_TYPE(
                             BSLS_COMPILERFEATURES_FORWARD(Arg1,argument1),
                          BSLS_COMPILERFEATURES_FORWARD(Args_01,arguments_01),
                          BSLS_COMPILERFEATURES_FORWARD(Args_02,arguments_02),
                          BSLS_COMPILERFEATURES_FORWARD(Args_03,arguments_03),
                          BSLS_COMPILERFEATURES_FORWARD(Args_04,arguments_04),
                          BSLS_COMPILERFEATURES_FORWARD(Args_05,arguments_05),
                          BSLS_COMPILERFEATURES_FORWARD(Args_06,arguments_06),
                          BSLS_COMPILERFEATURES_FORWARD(Args_07,arguments_07),
                          BSLS_COMPILERFEATURES_FORWARD(Args_08,arguments_08));
    BSLMA_CONSTRUCTIONUTIL_XLC_PLACEMENT_NEW_FIX;
}

template <class TARGET_TYPE, class Arg1, class Args_01,
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
ConstructionUtil::construct(TARGET_TYPE *address,
                            void        *,
                            BSLS_COMPILERFEATURES_FORWARD_REF(Arg1) argument1,
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
    ::new (Imp::voidify(address)) TARGET_TYPE(
                             BSLS_COMPILERFEATURES_FORWARD(Arg1,argument1),
                          BSLS_COMPILERFEATURES_FORWARD(Args_01,arguments_01),
                          BSLS_COMPILERFEATURES_FORWARD(Args_02,arguments_02),
                          BSLS_COMPILERFEATURES_FORWARD(Args_03,arguments_03),
                          BSLS_COMPILERFEATURES_FORWARD(Args_04,arguments_04),
                          BSLS_COMPILERFEATURES_FORWARD(Args_05,arguments_05),
                          BSLS_COMPILERFEATURES_FORWARD(Args_06,arguments_06),
                          BSLS_COMPILERFEATURES_FORWARD(Args_07,arguments_07),
                          BSLS_COMPILERFEATURES_FORWARD(Args_08,arguments_08),
                          BSLS_COMPILERFEATURES_FORWARD(Args_09,arguments_09));
    BSLMA_CONSTRUCTIONUTIL_XLC_PLACEMENT_NEW_FIX;
}

template <class TARGET_TYPE, class Arg1, class Args_01,
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
ConstructionUtil::construct(TARGET_TYPE *address,
                            void        *,
                            BSLS_COMPILERFEATURES_FORWARD_REF(Arg1) argument1,
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
    ::new (Imp::voidify(address)) TARGET_TYPE(
                             BSLS_COMPILERFEATURES_FORWARD(Arg1,argument1),
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
    BSLMA_CONSTRUCTIONUTIL_XLC_PLACEMENT_NEW_FIX;
}

template <class TARGET_TYPE, class Arg1, class Args_01,
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
ConstructionUtil::construct(TARGET_TYPE *address,
                            void        *,
                            BSLS_COMPILERFEATURES_FORWARD_REF(Arg1) argument1,
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
    ::new (Imp::voidify(address)) TARGET_TYPE(
                             BSLS_COMPILERFEATURES_FORWARD(Arg1,argument1),
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
    BSLMA_CONSTRUCTIONUTIL_XLC_PLACEMENT_NEW_FIX;
}

template <class TARGET_TYPE, class Arg1, class Args_01,
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
ConstructionUtil::construct(TARGET_TYPE *address,
                            void        *,
                            BSLS_COMPILERFEATURES_FORWARD_REF(Arg1) argument1,
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
    ::new (Imp::voidify(address)) TARGET_TYPE(
                             BSLS_COMPILERFEATURES_FORWARD(Arg1,argument1),
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
    BSLMA_CONSTRUCTIONUTIL_XLC_PLACEMENT_NEW_FIX;
}

template <class TARGET_TYPE, class Arg1, class Args_01,
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
ConstructionUtil::construct(TARGET_TYPE *address,
                            void        *,
                            BSLS_COMPILERFEATURES_FORWARD_REF(Arg1) argument1,
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
    ::new (Imp::voidify(address)) TARGET_TYPE(
                             BSLS_COMPILERFEATURES_FORWARD(Arg1,argument1),
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
    BSLMA_CONSTRUCTIONUTIL_XLC_PLACEMENT_NEW_FIX;
}

template <class TARGET_TYPE, class Arg1, class Args_01,
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
ConstructionUtil::construct(TARGET_TYPE *address,
                            void        *,
                            BSLS_COMPILERFEATURES_FORWARD_REF(Arg1) argument1,
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
    ::new (Imp::voidify(address)) TARGET_TYPE(
                             BSLS_COMPILERFEATURES_FORWARD(Arg1,argument1),
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
    BSLMA_CONSTRUCTIONUTIL_XLC_PLACEMENT_NEW_FIX;
}

template <class TARGET_TYPE, class Arg1, class Args_01,
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
                                         class Args_14,
                                         class Args_15>
inline
void
ConstructionUtil::construct(TARGET_TYPE *address,
                            void        *,
                            BSLS_COMPILERFEATURES_FORWARD_REF(Arg1) argument1,
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
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_14) arguments_14,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_15) arguments_15)
{
    ::new (Imp::voidify(address)) TARGET_TYPE(
                             BSLS_COMPILERFEATURES_FORWARD(Arg1,argument1),
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
                          BSLS_COMPILERFEATURES_FORWARD(Args_14,arguments_14),
                          BSLS_COMPILERFEATURES_FORWARD(Args_15,arguments_15));
    BSLMA_CONSTRUCTIONUTIL_XLC_PLACEMENT_NEW_FIX;
}

#else
// The generated code below is a workaround for the absence of perfect
// forwarding in some compilers.
template <class TARGET_TYPE, class Arg1, class... Args>
inline
void
ConstructionUtil::construct(TARGET_TYPE      *address,
                            bslma::Allocator *allocator,
                            BSLS_COMPILERFEATURES_FORWARD_REF(Arg1) argument1,
                          BSLS_COMPILERFEATURES_FORWARD_REF(Args)... arguments)
{
    enum {
        k_VALUE = bslma::UsesBslmaAllocator<TARGET_TYPE>::value
                ? (bslmf::UsesAllocatorArgT<TARGET_TYPE>::value
                 ? Imp::e_USES_ALLOCATOR_ARG_T_TRAITS
                 : Imp::e_USES_BSLMA_ALLOCATOR_TRAITS)
                : Imp::e_NIL_TRAITS
    };
    Imp::construct(address,
                   allocator,
                   (bsl::integral_constant<int, k_VALUE>*)0,
                   BSLS_COMPILERFEATURES_FORWARD(Arg1,argument1),
                   BSLS_COMPILERFEATURES_FORWARD(Args,arguments)...);
}

template <class TARGET_TYPE, class Arg1, class... Args>
inline
void
ConstructionUtil::construct(TARGET_TYPE *address,
                            void        *,
                            BSLS_COMPILERFEATURES_FORWARD_REF(Arg1) argument1,
                          BSLS_COMPILERFEATURES_FORWARD_REF(Args)... arguments)
{
    ::new (Imp::voidify(address)) TARGET_TYPE(
                             BSLS_COMPILERFEATURES_FORWARD(Arg1,argument1),
                             BSLS_COMPILERFEATURES_FORWARD(Args,arguments)...);
    BSLMA_CONSTRUCTIONUTIL_XLC_PLACEMENT_NEW_FIX;
}
// }}} END GENERATED CODE
#endif

template <class TARGET_TYPE, class ALLOCATOR>
inline
void
ConstructionUtil::destructiveMove(TARGET_TYPE *address,
                                  ALLOCATOR   *allocator,
                                  TARGET_TYPE *original)
{
    BSLS_ASSERT_SAFE(address);
    BSLS_ASSERT_SAFE(original);

    enum {
        k_VALUE = bslmf::IsBitwiseMoveable<TARGET_TYPE>::value
                ? Imp::e_BITWISE_MOVABLE_TRAITS
                : Imp::e_NIL_TRAITS
    };

    Imp::destructiveMove(address,
                         allocator,
                         (bsl::integral_constant<int, k_VALUE>*)0,
                         original);
}

                       // ---------------------------
                       // struct ConstructionUtil_Imp
                       // ---------------------------

// CLASS METHODS
template <class TARGET_TYPE>
inline
void
ConstructionUtil_Imp::construct(
              TARGET_TYPE *address,
              bslma::Allocator *,
              bsl::integral_constant<int, e_HAS_TRIVIAL_DEFAULT_CTOR_TRAITS> *)
{
    construct(address,
              (bsl::integral_constant<int,
                                      e_HAS_TRIVIAL_DEFAULT_CTOR_TRAITS> *) 0);
}

template <class TARGET_TYPE>
inline
void
ConstructionUtil_Imp::construct(
              TARGET_TYPE *address,
              bsl::integral_constant<int, e_HAS_TRIVIAL_DEFAULT_CTOR_TRAITS> *)
{
#if defined(BSLS_PLATFORM_CMP_MSVC) && BSLS_PLATFORM_CMP_VERSION < 1900
    defaultConstructScalar(bsl::is_member_pointer<TARGET_TYPE>(), address);
#else
    ::new (voidify(address)) TARGET_TYPE();
    BSLMA_CONSTRUCTIONUTIL_XLC_PLACEMENT_NEW_FIX;
#endif
}

template <class TARGET_TYPE>
inline
void ConstructionUtil_Imp::construct(TARGET_TYPE       *address,
                                     bslma::Allocator *,
                      bsl::integral_constant<int, e_BITWISE_COPYABLE_TRAITS> *,
                                     const TARGET_TYPE& original)
{
    construct(address,
              (bsl::integral_constant<int, e_BITWISE_COPYABLE_TRAITS>*)0,
               original);
}

template <class TARGET_TYPE>
inline
void ConstructionUtil_Imp::construct(TARGET_TYPE       *address,
                      bsl::integral_constant<int, e_BITWISE_COPYABLE_TRAITS> *,
                                     const TARGET_TYPE& original)
{
    ::new (voidify(address)) TARGET_TYPE(original);
    BSLMA_CONSTRUCTIONUTIL_XLC_PLACEMENT_NEW_FIX;
}

template <class TARGET_TYPE>
inline
void ConstructionUtil_Imp::construct(
              TARGET_TYPE                                            *address,
              bslma::Allocator                                       *,
              bsl::integral_constant<int, e_BITWISE_COPYABLE_TRAITS> *,
              bslmf::MovableRef<TARGET_TYPE>                         original)
{
    construct(address,
              (bsl::integral_constant<int, e_BITWISE_COPYABLE_TRAITS>*)0,
              BSLS_COMPILERFEATURES_FORWARD(TARGET_TYPE, original));
}

template <class TARGET_TYPE>
inline
void ConstructionUtil_Imp::construct(
               TARGET_TYPE                                           *address,
               bsl::integral_constant<int, e_BITWISE_COPYABLE_TRAITS> *,
               bslmf::MovableRef<TARGET_TYPE>                         original)
{
    ::new (voidify(address)) TARGET_TYPE(BSLS_COMPILERFEATURES_FORWARD(
        TARGET_TYPE, bslmf::MovableRefUtil::move(original)));
}

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES // $var-args=15
template <class TARGET_TYPE, class... Args>
inline
void
ConstructionUtil_Imp::construct(
                  TARGET_TYPE      *address,
                  bslma::Allocator *allocator,
                  bsl::integral_constant<int, e_USES_BSLMA_ALLOCATOR_TRAITS> *,
                  Args&&...         arguments)
{
    ::new (voidify(address)) TARGET_TYPE(
                  BSLS_COMPILERFEATURES_FORWARD(Args,arguments)..., allocator);
    BSLMA_CONSTRUCTIONUTIL_XLC_PLACEMENT_NEW_FIX;
}

template <class TARGET_TYPE, class... Args>
inline
void
ConstructionUtil_Imp::construct(
                  TARGET_TYPE      *address,
                  bslma::Allocator *allocator,
                  bsl::integral_constant<int, e_USES_ALLOCATOR_ARG_T_TRAITS> *,
                  Args&&...         arguments)
{
    ::new (voidify(address)) TARGET_TYPE(
                             bsl::allocator_arg,
                             allocator,
                             BSLS_COMPILERFEATURES_FORWARD(Args,arguments)...);
    BSLMA_CONSTRUCTIONUTIL_XLC_PLACEMENT_NEW_FIX;
}

template <class TARGET_TYPE, class... Args>
inline
void
ConstructionUtil_Imp::construct(TARGET_TYPE      *address,
                                bslma::Allocator *,
                                bsl::integral_constant<int, e_NIL_TRAITS> *,
                                Args&&...         arguments)
{
    construct(address,
              (bsl::integral_constant<int, e_NIL_TRAITS> *)0,
              BSLS_COMPILERFEATURES_FORWARD(Args,arguments)...);
}

template <class TARGET_TYPE, class... Args>
inline
void
ConstructionUtil_Imp::construct(TARGET_TYPE      *address,
                                bsl::integral_constant<int, e_NIL_TRAITS> *,
                                Args&&...         arguments)
{
    ::new (voidify(address)) TARGET_TYPE(
                             BSLS_COMPILERFEATURES_FORWARD(Args,arguments)...);
    BSLMA_CONSTRUCTIONUTIL_XLC_PLACEMENT_NEW_FIX;
}
#elif BSLS_COMPILERFEATURES_SIMULATE_VARIADIC_TEMPLATES
// {{{ BEGIN GENERATED CODE
// The following section is automatically generated.  **DO NOT EDIT**
// Generator command line: sim_cpp11_features.pl bslma_constructionutil.h
template <class TARGET_TYPE>
inline
void
ConstructionUtil_Imp::construct(
                  TARGET_TYPE      *address,
                  bslma::Allocator *allocator,
                  bsl::integral_constant<int, e_USES_BSLMA_ALLOCATOR_TRAITS> *)
{
    ::new (voidify(address)) TARGET_TYPE(
                  allocator);
    BSLMA_CONSTRUCTIONUTIL_XLC_PLACEMENT_NEW_FIX;
}

template <class TARGET_TYPE, class Args_01>
inline
void
ConstructionUtil_Imp::construct(
                  TARGET_TYPE      *address,
                  bslma::Allocator *allocator,
                  bsl::integral_constant<int, e_USES_BSLMA_ALLOCATOR_TRAITS> *,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01)
{
    ::new (voidify(address)) TARGET_TYPE(
                  BSLS_COMPILERFEATURES_FORWARD(Args_01,arguments_01),
                  allocator);
    BSLMA_CONSTRUCTIONUTIL_XLC_PLACEMENT_NEW_FIX;
}

template <class TARGET_TYPE, class Args_01,
                             class Args_02>
inline
void
ConstructionUtil_Imp::construct(
                  TARGET_TYPE      *address,
                  bslma::Allocator *allocator,
                  bsl::integral_constant<int, e_USES_BSLMA_ALLOCATOR_TRAITS> *,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02)
{
    ::new (voidify(address)) TARGET_TYPE(
                  BSLS_COMPILERFEATURES_FORWARD(Args_01,arguments_01),
                  BSLS_COMPILERFEATURES_FORWARD(Args_02,arguments_02),
                  allocator);
    BSLMA_CONSTRUCTIONUTIL_XLC_PLACEMENT_NEW_FIX;
}

template <class TARGET_TYPE, class Args_01,
                             class Args_02,
                             class Args_03>
inline
void
ConstructionUtil_Imp::construct(
                  TARGET_TYPE      *address,
                  bslma::Allocator *allocator,
                  bsl::integral_constant<int, e_USES_BSLMA_ALLOCATOR_TRAITS> *,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03)
{
    ::new (voidify(address)) TARGET_TYPE(
                  BSLS_COMPILERFEATURES_FORWARD(Args_01,arguments_01),
                  BSLS_COMPILERFEATURES_FORWARD(Args_02,arguments_02),
                  BSLS_COMPILERFEATURES_FORWARD(Args_03,arguments_03),
                  allocator);
    BSLMA_CONSTRUCTIONUTIL_XLC_PLACEMENT_NEW_FIX;
}

template <class TARGET_TYPE, class Args_01,
                             class Args_02,
                             class Args_03,
                             class Args_04>
inline
void
ConstructionUtil_Imp::construct(
                  TARGET_TYPE      *address,
                  bslma::Allocator *allocator,
                  bsl::integral_constant<int, e_USES_BSLMA_ALLOCATOR_TRAITS> *,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04)
{
    ::new (voidify(address)) TARGET_TYPE(
                  BSLS_COMPILERFEATURES_FORWARD(Args_01,arguments_01),
                  BSLS_COMPILERFEATURES_FORWARD(Args_02,arguments_02),
                  BSLS_COMPILERFEATURES_FORWARD(Args_03,arguments_03),
                  BSLS_COMPILERFEATURES_FORWARD(Args_04,arguments_04),
                  allocator);
    BSLMA_CONSTRUCTIONUTIL_XLC_PLACEMENT_NEW_FIX;
}

template <class TARGET_TYPE, class Args_01,
                             class Args_02,
                             class Args_03,
                             class Args_04,
                             class Args_05>
inline
void
ConstructionUtil_Imp::construct(
                  TARGET_TYPE      *address,
                  bslma::Allocator *allocator,
                  bsl::integral_constant<int, e_USES_BSLMA_ALLOCATOR_TRAITS> *,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05)
{
    ::new (voidify(address)) TARGET_TYPE(
                  BSLS_COMPILERFEATURES_FORWARD(Args_01,arguments_01),
                  BSLS_COMPILERFEATURES_FORWARD(Args_02,arguments_02),
                  BSLS_COMPILERFEATURES_FORWARD(Args_03,arguments_03),
                  BSLS_COMPILERFEATURES_FORWARD(Args_04,arguments_04),
                  BSLS_COMPILERFEATURES_FORWARD(Args_05,arguments_05),
                  allocator);
    BSLMA_CONSTRUCTIONUTIL_XLC_PLACEMENT_NEW_FIX;
}

template <class TARGET_TYPE, class Args_01,
                             class Args_02,
                             class Args_03,
                             class Args_04,
                             class Args_05,
                             class Args_06>
inline
void
ConstructionUtil_Imp::construct(
                  TARGET_TYPE      *address,
                  bslma::Allocator *allocator,
                  bsl::integral_constant<int, e_USES_BSLMA_ALLOCATOR_TRAITS> *,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06)
{
    ::new (voidify(address)) TARGET_TYPE(
                  BSLS_COMPILERFEATURES_FORWARD(Args_01,arguments_01),
                  BSLS_COMPILERFEATURES_FORWARD(Args_02,arguments_02),
                  BSLS_COMPILERFEATURES_FORWARD(Args_03,arguments_03),
                  BSLS_COMPILERFEATURES_FORWARD(Args_04,arguments_04),
                  BSLS_COMPILERFEATURES_FORWARD(Args_05,arguments_05),
                  BSLS_COMPILERFEATURES_FORWARD(Args_06,arguments_06),
                  allocator);
    BSLMA_CONSTRUCTIONUTIL_XLC_PLACEMENT_NEW_FIX;
}

template <class TARGET_TYPE, class Args_01,
                             class Args_02,
                             class Args_03,
                             class Args_04,
                             class Args_05,
                             class Args_06,
                             class Args_07>
inline
void
ConstructionUtil_Imp::construct(
                  TARGET_TYPE      *address,
                  bslma::Allocator *allocator,
                  bsl::integral_constant<int, e_USES_BSLMA_ALLOCATOR_TRAITS> *,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07)
{
    ::new (voidify(address)) TARGET_TYPE(
                  BSLS_COMPILERFEATURES_FORWARD(Args_01,arguments_01),
                  BSLS_COMPILERFEATURES_FORWARD(Args_02,arguments_02),
                  BSLS_COMPILERFEATURES_FORWARD(Args_03,arguments_03),
                  BSLS_COMPILERFEATURES_FORWARD(Args_04,arguments_04),
                  BSLS_COMPILERFEATURES_FORWARD(Args_05,arguments_05),
                  BSLS_COMPILERFEATURES_FORWARD(Args_06,arguments_06),
                  BSLS_COMPILERFEATURES_FORWARD(Args_07,arguments_07),
                  allocator);
    BSLMA_CONSTRUCTIONUTIL_XLC_PLACEMENT_NEW_FIX;
}

template <class TARGET_TYPE, class Args_01,
                             class Args_02,
                             class Args_03,
                             class Args_04,
                             class Args_05,
                             class Args_06,
                             class Args_07,
                             class Args_08>
inline
void
ConstructionUtil_Imp::construct(
                  TARGET_TYPE      *address,
                  bslma::Allocator *allocator,
                  bsl::integral_constant<int, e_USES_BSLMA_ALLOCATOR_TRAITS> *,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) arguments_08)
{
    ::new (voidify(address)) TARGET_TYPE(
                  BSLS_COMPILERFEATURES_FORWARD(Args_01,arguments_01),
                  BSLS_COMPILERFEATURES_FORWARD(Args_02,arguments_02),
                  BSLS_COMPILERFEATURES_FORWARD(Args_03,arguments_03),
                  BSLS_COMPILERFEATURES_FORWARD(Args_04,arguments_04),
                  BSLS_COMPILERFEATURES_FORWARD(Args_05,arguments_05),
                  BSLS_COMPILERFEATURES_FORWARD(Args_06,arguments_06),
                  BSLS_COMPILERFEATURES_FORWARD(Args_07,arguments_07),
                  BSLS_COMPILERFEATURES_FORWARD(Args_08,arguments_08),
                  allocator);
    BSLMA_CONSTRUCTIONUTIL_XLC_PLACEMENT_NEW_FIX;
}

template <class TARGET_TYPE, class Args_01,
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
ConstructionUtil_Imp::construct(
                  TARGET_TYPE      *address,
                  bslma::Allocator *allocator,
                  bsl::integral_constant<int, e_USES_BSLMA_ALLOCATOR_TRAITS> *,
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
    ::new (voidify(address)) TARGET_TYPE(
                  BSLS_COMPILERFEATURES_FORWARD(Args_01,arguments_01),
                  BSLS_COMPILERFEATURES_FORWARD(Args_02,arguments_02),
                  BSLS_COMPILERFEATURES_FORWARD(Args_03,arguments_03),
                  BSLS_COMPILERFEATURES_FORWARD(Args_04,arguments_04),
                  BSLS_COMPILERFEATURES_FORWARD(Args_05,arguments_05),
                  BSLS_COMPILERFEATURES_FORWARD(Args_06,arguments_06),
                  BSLS_COMPILERFEATURES_FORWARD(Args_07,arguments_07),
                  BSLS_COMPILERFEATURES_FORWARD(Args_08,arguments_08),
                  BSLS_COMPILERFEATURES_FORWARD(Args_09,arguments_09),
                  allocator);
    BSLMA_CONSTRUCTIONUTIL_XLC_PLACEMENT_NEW_FIX;
}

template <class TARGET_TYPE, class Args_01,
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
ConstructionUtil_Imp::construct(
                  TARGET_TYPE      *address,
                  bslma::Allocator *allocator,
                  bsl::integral_constant<int, e_USES_BSLMA_ALLOCATOR_TRAITS> *,
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
    ::new (voidify(address)) TARGET_TYPE(
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
                  allocator);
    BSLMA_CONSTRUCTIONUTIL_XLC_PLACEMENT_NEW_FIX;
}

template <class TARGET_TYPE, class Args_01,
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
ConstructionUtil_Imp::construct(
                  TARGET_TYPE      *address,
                  bslma::Allocator *allocator,
                  bsl::integral_constant<int, e_USES_BSLMA_ALLOCATOR_TRAITS> *,
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
    ::new (voidify(address)) TARGET_TYPE(
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
                  allocator);
    BSLMA_CONSTRUCTIONUTIL_XLC_PLACEMENT_NEW_FIX;
}

template <class TARGET_TYPE, class Args_01,
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
ConstructionUtil_Imp::construct(
                  TARGET_TYPE      *address,
                  bslma::Allocator *allocator,
                  bsl::integral_constant<int, e_USES_BSLMA_ALLOCATOR_TRAITS> *,
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
    ::new (voidify(address)) TARGET_TYPE(
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
                  allocator);
    BSLMA_CONSTRUCTIONUTIL_XLC_PLACEMENT_NEW_FIX;
}

template <class TARGET_TYPE, class Args_01,
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
ConstructionUtil_Imp::construct(
                  TARGET_TYPE      *address,
                  bslma::Allocator *allocator,
                  bsl::integral_constant<int, e_USES_BSLMA_ALLOCATOR_TRAITS> *,
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
    ::new (voidify(address)) TARGET_TYPE(
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
                  allocator);
    BSLMA_CONSTRUCTIONUTIL_XLC_PLACEMENT_NEW_FIX;
}

template <class TARGET_TYPE, class Args_01,
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
ConstructionUtil_Imp::construct(
                  TARGET_TYPE      *address,
                  bslma::Allocator *allocator,
                  bsl::integral_constant<int, e_USES_BSLMA_ALLOCATOR_TRAITS> *,
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
    ::new (voidify(address)) TARGET_TYPE(
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
                  BSLS_COMPILERFEATURES_FORWARD(Args_14,arguments_14),
                  allocator);
    BSLMA_CONSTRUCTIONUTIL_XLC_PLACEMENT_NEW_FIX;
}

template <class TARGET_TYPE, class Args_01,
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
                             class Args_14,
                             class Args_15>
inline
void
ConstructionUtil_Imp::construct(
                  TARGET_TYPE      *address,
                  bslma::Allocator *allocator,
                  bsl::integral_constant<int, e_USES_BSLMA_ALLOCATOR_TRAITS> *,
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
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_14) arguments_14,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_15) arguments_15)
{
    ::new (voidify(address)) TARGET_TYPE(
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
                  BSLS_COMPILERFEATURES_FORWARD(Args_14,arguments_14),
                  BSLS_COMPILERFEATURES_FORWARD(Args_15,arguments_15),
                  allocator);
    BSLMA_CONSTRUCTIONUTIL_XLC_PLACEMENT_NEW_FIX;
}

template <class TARGET_TYPE>
inline
void
ConstructionUtil_Imp::construct(
                  TARGET_TYPE      *address,
                  bslma::Allocator *allocator,
                  bsl::integral_constant<int, e_USES_ALLOCATOR_ARG_T_TRAITS> *)
{
    ::new (voidify(address)) TARGET_TYPE(
                             bsl::allocator_arg,
                             allocator);
    BSLMA_CONSTRUCTIONUTIL_XLC_PLACEMENT_NEW_FIX;
}

template <class TARGET_TYPE, class Args_01>
inline
void
ConstructionUtil_Imp::construct(
                  TARGET_TYPE      *address,
                  bslma::Allocator *allocator,
                  bsl::integral_constant<int, e_USES_ALLOCATOR_ARG_T_TRAITS> *,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01)
{
    ::new (voidify(address)) TARGET_TYPE(
                             bsl::allocator_arg,
                             allocator,
                          BSLS_COMPILERFEATURES_FORWARD(Args_01,arguments_01));
    BSLMA_CONSTRUCTIONUTIL_XLC_PLACEMENT_NEW_FIX;
}

template <class TARGET_TYPE, class Args_01,
                             class Args_02>
inline
void
ConstructionUtil_Imp::construct(
                  TARGET_TYPE      *address,
                  bslma::Allocator *allocator,
                  bsl::integral_constant<int, e_USES_ALLOCATOR_ARG_T_TRAITS> *,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02)
{
    ::new (voidify(address)) TARGET_TYPE(
                             bsl::allocator_arg,
                             allocator,
                          BSLS_COMPILERFEATURES_FORWARD(Args_01,arguments_01),
                          BSLS_COMPILERFEATURES_FORWARD(Args_02,arguments_02));
    BSLMA_CONSTRUCTIONUTIL_XLC_PLACEMENT_NEW_FIX;
}

template <class TARGET_TYPE, class Args_01,
                             class Args_02,
                             class Args_03>
inline
void
ConstructionUtil_Imp::construct(
                  TARGET_TYPE      *address,
                  bslma::Allocator *allocator,
                  bsl::integral_constant<int, e_USES_ALLOCATOR_ARG_T_TRAITS> *,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03)
{
    ::new (voidify(address)) TARGET_TYPE(
                             bsl::allocator_arg,
                             allocator,
                          BSLS_COMPILERFEATURES_FORWARD(Args_01,arguments_01),
                          BSLS_COMPILERFEATURES_FORWARD(Args_02,arguments_02),
                          BSLS_COMPILERFEATURES_FORWARD(Args_03,arguments_03));
    BSLMA_CONSTRUCTIONUTIL_XLC_PLACEMENT_NEW_FIX;
}

template <class TARGET_TYPE, class Args_01,
                             class Args_02,
                             class Args_03,
                             class Args_04>
inline
void
ConstructionUtil_Imp::construct(
                  TARGET_TYPE      *address,
                  bslma::Allocator *allocator,
                  bsl::integral_constant<int, e_USES_ALLOCATOR_ARG_T_TRAITS> *,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04)
{
    ::new (voidify(address)) TARGET_TYPE(
                             bsl::allocator_arg,
                             allocator,
                          BSLS_COMPILERFEATURES_FORWARD(Args_01,arguments_01),
                          BSLS_COMPILERFEATURES_FORWARD(Args_02,arguments_02),
                          BSLS_COMPILERFEATURES_FORWARD(Args_03,arguments_03),
                          BSLS_COMPILERFEATURES_FORWARD(Args_04,arguments_04));
    BSLMA_CONSTRUCTIONUTIL_XLC_PLACEMENT_NEW_FIX;
}

template <class TARGET_TYPE, class Args_01,
                             class Args_02,
                             class Args_03,
                             class Args_04,
                             class Args_05>
inline
void
ConstructionUtil_Imp::construct(
                  TARGET_TYPE      *address,
                  bslma::Allocator *allocator,
                  bsl::integral_constant<int, e_USES_ALLOCATOR_ARG_T_TRAITS> *,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05)
{
    ::new (voidify(address)) TARGET_TYPE(
                             bsl::allocator_arg,
                             allocator,
                          BSLS_COMPILERFEATURES_FORWARD(Args_01,arguments_01),
                          BSLS_COMPILERFEATURES_FORWARD(Args_02,arguments_02),
                          BSLS_COMPILERFEATURES_FORWARD(Args_03,arguments_03),
                          BSLS_COMPILERFEATURES_FORWARD(Args_04,arguments_04),
                          BSLS_COMPILERFEATURES_FORWARD(Args_05,arguments_05));
    BSLMA_CONSTRUCTIONUTIL_XLC_PLACEMENT_NEW_FIX;
}

template <class TARGET_TYPE, class Args_01,
                             class Args_02,
                             class Args_03,
                             class Args_04,
                             class Args_05,
                             class Args_06>
inline
void
ConstructionUtil_Imp::construct(
                  TARGET_TYPE      *address,
                  bslma::Allocator *allocator,
                  bsl::integral_constant<int, e_USES_ALLOCATOR_ARG_T_TRAITS> *,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06)
{
    ::new (voidify(address)) TARGET_TYPE(
                             bsl::allocator_arg,
                             allocator,
                          BSLS_COMPILERFEATURES_FORWARD(Args_01,arguments_01),
                          BSLS_COMPILERFEATURES_FORWARD(Args_02,arguments_02),
                          BSLS_COMPILERFEATURES_FORWARD(Args_03,arguments_03),
                          BSLS_COMPILERFEATURES_FORWARD(Args_04,arguments_04),
                          BSLS_COMPILERFEATURES_FORWARD(Args_05,arguments_05),
                          BSLS_COMPILERFEATURES_FORWARD(Args_06,arguments_06));
    BSLMA_CONSTRUCTIONUTIL_XLC_PLACEMENT_NEW_FIX;
}

template <class TARGET_TYPE, class Args_01,
                             class Args_02,
                             class Args_03,
                             class Args_04,
                             class Args_05,
                             class Args_06,
                             class Args_07>
inline
void
ConstructionUtil_Imp::construct(
                  TARGET_TYPE      *address,
                  bslma::Allocator *allocator,
                  bsl::integral_constant<int, e_USES_ALLOCATOR_ARG_T_TRAITS> *,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07)
{
    ::new (voidify(address)) TARGET_TYPE(
                             bsl::allocator_arg,
                             allocator,
                          BSLS_COMPILERFEATURES_FORWARD(Args_01,arguments_01),
                          BSLS_COMPILERFEATURES_FORWARD(Args_02,arguments_02),
                          BSLS_COMPILERFEATURES_FORWARD(Args_03,arguments_03),
                          BSLS_COMPILERFEATURES_FORWARD(Args_04,arguments_04),
                          BSLS_COMPILERFEATURES_FORWARD(Args_05,arguments_05),
                          BSLS_COMPILERFEATURES_FORWARD(Args_06,arguments_06),
                          BSLS_COMPILERFEATURES_FORWARD(Args_07,arguments_07));
    BSLMA_CONSTRUCTIONUTIL_XLC_PLACEMENT_NEW_FIX;
}

template <class TARGET_TYPE, class Args_01,
                             class Args_02,
                             class Args_03,
                             class Args_04,
                             class Args_05,
                             class Args_06,
                             class Args_07,
                             class Args_08>
inline
void
ConstructionUtil_Imp::construct(
                  TARGET_TYPE      *address,
                  bslma::Allocator *allocator,
                  bsl::integral_constant<int, e_USES_ALLOCATOR_ARG_T_TRAITS> *,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) arguments_08)
{
    ::new (voidify(address)) TARGET_TYPE(
                             bsl::allocator_arg,
                             allocator,
                          BSLS_COMPILERFEATURES_FORWARD(Args_01,arguments_01),
                          BSLS_COMPILERFEATURES_FORWARD(Args_02,arguments_02),
                          BSLS_COMPILERFEATURES_FORWARD(Args_03,arguments_03),
                          BSLS_COMPILERFEATURES_FORWARD(Args_04,arguments_04),
                          BSLS_COMPILERFEATURES_FORWARD(Args_05,arguments_05),
                          BSLS_COMPILERFEATURES_FORWARD(Args_06,arguments_06),
                          BSLS_COMPILERFEATURES_FORWARD(Args_07,arguments_07),
                          BSLS_COMPILERFEATURES_FORWARD(Args_08,arguments_08));
    BSLMA_CONSTRUCTIONUTIL_XLC_PLACEMENT_NEW_FIX;
}

template <class TARGET_TYPE, class Args_01,
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
ConstructionUtil_Imp::construct(
                  TARGET_TYPE      *address,
                  bslma::Allocator *allocator,
                  bsl::integral_constant<int, e_USES_ALLOCATOR_ARG_T_TRAITS> *,
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
    ::new (voidify(address)) TARGET_TYPE(
                             bsl::allocator_arg,
                             allocator,
                          BSLS_COMPILERFEATURES_FORWARD(Args_01,arguments_01),
                          BSLS_COMPILERFEATURES_FORWARD(Args_02,arguments_02),
                          BSLS_COMPILERFEATURES_FORWARD(Args_03,arguments_03),
                          BSLS_COMPILERFEATURES_FORWARD(Args_04,arguments_04),
                          BSLS_COMPILERFEATURES_FORWARD(Args_05,arguments_05),
                          BSLS_COMPILERFEATURES_FORWARD(Args_06,arguments_06),
                          BSLS_COMPILERFEATURES_FORWARD(Args_07,arguments_07),
                          BSLS_COMPILERFEATURES_FORWARD(Args_08,arguments_08),
                          BSLS_COMPILERFEATURES_FORWARD(Args_09,arguments_09));
    BSLMA_CONSTRUCTIONUTIL_XLC_PLACEMENT_NEW_FIX;
}

template <class TARGET_TYPE, class Args_01,
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
ConstructionUtil_Imp::construct(
                  TARGET_TYPE      *address,
                  bslma::Allocator *allocator,
                  bsl::integral_constant<int, e_USES_ALLOCATOR_ARG_T_TRAITS> *,
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
    ::new (voidify(address)) TARGET_TYPE(
                             bsl::allocator_arg,
                             allocator,
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
    BSLMA_CONSTRUCTIONUTIL_XLC_PLACEMENT_NEW_FIX;
}

template <class TARGET_TYPE, class Args_01,
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
ConstructionUtil_Imp::construct(
                  TARGET_TYPE      *address,
                  bslma::Allocator *allocator,
                  bsl::integral_constant<int, e_USES_ALLOCATOR_ARG_T_TRAITS> *,
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
    ::new (voidify(address)) TARGET_TYPE(
                             bsl::allocator_arg,
                             allocator,
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
    BSLMA_CONSTRUCTIONUTIL_XLC_PLACEMENT_NEW_FIX;
}

template <class TARGET_TYPE, class Args_01,
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
ConstructionUtil_Imp::construct(
                  TARGET_TYPE      *address,
                  bslma::Allocator *allocator,
                  bsl::integral_constant<int, e_USES_ALLOCATOR_ARG_T_TRAITS> *,
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
    ::new (voidify(address)) TARGET_TYPE(
                             bsl::allocator_arg,
                             allocator,
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
    BSLMA_CONSTRUCTIONUTIL_XLC_PLACEMENT_NEW_FIX;
}

template <class TARGET_TYPE, class Args_01,
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
ConstructionUtil_Imp::construct(
                  TARGET_TYPE      *address,
                  bslma::Allocator *allocator,
                  bsl::integral_constant<int, e_USES_ALLOCATOR_ARG_T_TRAITS> *,
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
    ::new (voidify(address)) TARGET_TYPE(
                             bsl::allocator_arg,
                             allocator,
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
    BSLMA_CONSTRUCTIONUTIL_XLC_PLACEMENT_NEW_FIX;
}

template <class TARGET_TYPE, class Args_01,
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
ConstructionUtil_Imp::construct(
                  TARGET_TYPE      *address,
                  bslma::Allocator *allocator,
                  bsl::integral_constant<int, e_USES_ALLOCATOR_ARG_T_TRAITS> *,
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
    ::new (voidify(address)) TARGET_TYPE(
                             bsl::allocator_arg,
                             allocator,
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
    BSLMA_CONSTRUCTIONUTIL_XLC_PLACEMENT_NEW_FIX;
}

template <class TARGET_TYPE, class Args_01,
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
                             class Args_14,
                             class Args_15>
inline
void
ConstructionUtil_Imp::construct(
                  TARGET_TYPE      *address,
                  bslma::Allocator *allocator,
                  bsl::integral_constant<int, e_USES_ALLOCATOR_ARG_T_TRAITS> *,
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
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_14) arguments_14,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_15) arguments_15)
{
    ::new (voidify(address)) TARGET_TYPE(
                             bsl::allocator_arg,
                             allocator,
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
                          BSLS_COMPILERFEATURES_FORWARD(Args_14,arguments_14),
                          BSLS_COMPILERFEATURES_FORWARD(Args_15,arguments_15));
    BSLMA_CONSTRUCTIONUTIL_XLC_PLACEMENT_NEW_FIX;
}

template <class TARGET_TYPE>
inline
void
ConstructionUtil_Imp::construct(TARGET_TYPE      *address,
                                bslma::Allocator *,
                                bsl::integral_constant<int, e_NIL_TRAITS> *)
{
    construct(address,
              (bsl::integral_constant<int, e_NIL_TRAITS> *)0);
}

template <class TARGET_TYPE, class Args_01>
inline
void
ConstructionUtil_Imp::construct(TARGET_TYPE      *address,
                                bslma::Allocator *,
                                bsl::integral_constant<int, e_NIL_TRAITS> *,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01)
{
    construct(address,
              (bsl::integral_constant<int, e_NIL_TRAITS> *)0,
              BSLS_COMPILERFEATURES_FORWARD(Args_01,arguments_01));
}

template <class TARGET_TYPE, class Args_01,
                             class Args_02>
inline
void
ConstructionUtil_Imp::construct(TARGET_TYPE      *address,
                                bslma::Allocator *,
                                bsl::integral_constant<int, e_NIL_TRAITS> *,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02)
{
    construct(address,
              (bsl::integral_constant<int, e_NIL_TRAITS> *)0,
              BSLS_COMPILERFEATURES_FORWARD(Args_01,arguments_01),
              BSLS_COMPILERFEATURES_FORWARD(Args_02,arguments_02));
}

template <class TARGET_TYPE, class Args_01,
                             class Args_02,
                             class Args_03>
inline
void
ConstructionUtil_Imp::construct(TARGET_TYPE      *address,
                                bslma::Allocator *,
                                bsl::integral_constant<int, e_NIL_TRAITS> *,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03)
{
    construct(address,
              (bsl::integral_constant<int, e_NIL_TRAITS> *)0,
              BSLS_COMPILERFEATURES_FORWARD(Args_01,arguments_01),
              BSLS_COMPILERFEATURES_FORWARD(Args_02,arguments_02),
              BSLS_COMPILERFEATURES_FORWARD(Args_03,arguments_03));
}

template <class TARGET_TYPE, class Args_01,
                             class Args_02,
                             class Args_03,
                             class Args_04>
inline
void
ConstructionUtil_Imp::construct(TARGET_TYPE      *address,
                                bslma::Allocator *,
                                bsl::integral_constant<int, e_NIL_TRAITS> *,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04)
{
    construct(address,
              (bsl::integral_constant<int, e_NIL_TRAITS> *)0,
              BSLS_COMPILERFEATURES_FORWARD(Args_01,arguments_01),
              BSLS_COMPILERFEATURES_FORWARD(Args_02,arguments_02),
              BSLS_COMPILERFEATURES_FORWARD(Args_03,arguments_03),
              BSLS_COMPILERFEATURES_FORWARD(Args_04,arguments_04));
}

template <class TARGET_TYPE, class Args_01,
                             class Args_02,
                             class Args_03,
                             class Args_04,
                             class Args_05>
inline
void
ConstructionUtil_Imp::construct(TARGET_TYPE      *address,
                                bslma::Allocator *,
                                bsl::integral_constant<int, e_NIL_TRAITS> *,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05)
{
    construct(address,
              (bsl::integral_constant<int, e_NIL_TRAITS> *)0,
              BSLS_COMPILERFEATURES_FORWARD(Args_01,arguments_01),
              BSLS_COMPILERFEATURES_FORWARD(Args_02,arguments_02),
              BSLS_COMPILERFEATURES_FORWARD(Args_03,arguments_03),
              BSLS_COMPILERFEATURES_FORWARD(Args_04,arguments_04),
              BSLS_COMPILERFEATURES_FORWARD(Args_05,arguments_05));
}

template <class TARGET_TYPE, class Args_01,
                             class Args_02,
                             class Args_03,
                             class Args_04,
                             class Args_05,
                             class Args_06>
inline
void
ConstructionUtil_Imp::construct(TARGET_TYPE      *address,
                                bslma::Allocator *,
                                bsl::integral_constant<int, e_NIL_TRAITS> *,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06)
{
    construct(address,
              (bsl::integral_constant<int, e_NIL_TRAITS> *)0,
              BSLS_COMPILERFEATURES_FORWARD(Args_01,arguments_01),
              BSLS_COMPILERFEATURES_FORWARD(Args_02,arguments_02),
              BSLS_COMPILERFEATURES_FORWARD(Args_03,arguments_03),
              BSLS_COMPILERFEATURES_FORWARD(Args_04,arguments_04),
              BSLS_COMPILERFEATURES_FORWARD(Args_05,arguments_05),
              BSLS_COMPILERFEATURES_FORWARD(Args_06,arguments_06));
}

template <class TARGET_TYPE, class Args_01,
                             class Args_02,
                             class Args_03,
                             class Args_04,
                             class Args_05,
                             class Args_06,
                             class Args_07>
inline
void
ConstructionUtil_Imp::construct(TARGET_TYPE      *address,
                                bslma::Allocator *,
                                bsl::integral_constant<int, e_NIL_TRAITS> *,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07)
{
    construct(address,
              (bsl::integral_constant<int, e_NIL_TRAITS> *)0,
              BSLS_COMPILERFEATURES_FORWARD(Args_01,arguments_01),
              BSLS_COMPILERFEATURES_FORWARD(Args_02,arguments_02),
              BSLS_COMPILERFEATURES_FORWARD(Args_03,arguments_03),
              BSLS_COMPILERFEATURES_FORWARD(Args_04,arguments_04),
              BSLS_COMPILERFEATURES_FORWARD(Args_05,arguments_05),
              BSLS_COMPILERFEATURES_FORWARD(Args_06,arguments_06),
              BSLS_COMPILERFEATURES_FORWARD(Args_07,arguments_07));
}

template <class TARGET_TYPE, class Args_01,
                             class Args_02,
                             class Args_03,
                             class Args_04,
                             class Args_05,
                             class Args_06,
                             class Args_07,
                             class Args_08>
inline
void
ConstructionUtil_Imp::construct(TARGET_TYPE      *address,
                                bslma::Allocator *,
                                bsl::integral_constant<int, e_NIL_TRAITS> *,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) arguments_08)
{
    construct(address,
              (bsl::integral_constant<int, e_NIL_TRAITS> *)0,
              BSLS_COMPILERFEATURES_FORWARD(Args_01,arguments_01),
              BSLS_COMPILERFEATURES_FORWARD(Args_02,arguments_02),
              BSLS_COMPILERFEATURES_FORWARD(Args_03,arguments_03),
              BSLS_COMPILERFEATURES_FORWARD(Args_04,arguments_04),
              BSLS_COMPILERFEATURES_FORWARD(Args_05,arguments_05),
              BSLS_COMPILERFEATURES_FORWARD(Args_06,arguments_06),
              BSLS_COMPILERFEATURES_FORWARD(Args_07,arguments_07),
              BSLS_COMPILERFEATURES_FORWARD(Args_08,arguments_08));
}

template <class TARGET_TYPE, class Args_01,
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
ConstructionUtil_Imp::construct(TARGET_TYPE      *address,
                                bslma::Allocator *,
                                bsl::integral_constant<int, e_NIL_TRAITS> *,
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
    construct(address,
              (bsl::integral_constant<int, e_NIL_TRAITS> *)0,
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

template <class TARGET_TYPE, class Args_01,
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
ConstructionUtil_Imp::construct(TARGET_TYPE      *address,
                                bslma::Allocator *,
                                bsl::integral_constant<int, e_NIL_TRAITS> *,
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
    construct(address,
              (bsl::integral_constant<int, e_NIL_TRAITS> *)0,
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

template <class TARGET_TYPE, class Args_01,
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
ConstructionUtil_Imp::construct(TARGET_TYPE      *address,
                                bslma::Allocator *,
                                bsl::integral_constant<int, e_NIL_TRAITS> *,
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
    construct(address,
              (bsl::integral_constant<int, e_NIL_TRAITS> *)0,
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

template <class TARGET_TYPE, class Args_01,
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
ConstructionUtil_Imp::construct(TARGET_TYPE      *address,
                                bslma::Allocator *,
                                bsl::integral_constant<int, e_NIL_TRAITS> *,
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
    construct(address,
              (bsl::integral_constant<int, e_NIL_TRAITS> *)0,
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

template <class TARGET_TYPE, class Args_01,
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
ConstructionUtil_Imp::construct(TARGET_TYPE      *address,
                                bslma::Allocator *,
                                bsl::integral_constant<int, e_NIL_TRAITS> *,
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
    construct(address,
              (bsl::integral_constant<int, e_NIL_TRAITS> *)0,
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

template <class TARGET_TYPE, class Args_01,
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
ConstructionUtil_Imp::construct(TARGET_TYPE      *address,
                                bslma::Allocator *,
                                bsl::integral_constant<int, e_NIL_TRAITS> *,
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
    construct(address,
              (bsl::integral_constant<int, e_NIL_TRAITS> *)0,
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

template <class TARGET_TYPE, class Args_01,
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
                             class Args_14,
                             class Args_15>
inline
void
ConstructionUtil_Imp::construct(TARGET_TYPE      *address,
                                bslma::Allocator *,
                                bsl::integral_constant<int, e_NIL_TRAITS> *,
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
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_14) arguments_14,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_15) arguments_15)
{
    construct(address,
              (bsl::integral_constant<int, e_NIL_TRAITS> *)0,
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
              BSLS_COMPILERFEATURES_FORWARD(Args_14,arguments_14),
              BSLS_COMPILERFEATURES_FORWARD(Args_15,arguments_15));
}

template <class TARGET_TYPE>
inline
void
ConstructionUtil_Imp::construct(TARGET_TYPE      *address,
                                bsl::integral_constant<int, e_NIL_TRAITS> *)
{
    ::new (voidify(address)) TARGET_TYPE(
                             );
    BSLMA_CONSTRUCTIONUTIL_XLC_PLACEMENT_NEW_FIX;
}

template <class TARGET_TYPE, class Args_01>
inline
void
ConstructionUtil_Imp::construct(TARGET_TYPE      *address,
                                bsl::integral_constant<int, e_NIL_TRAITS> *,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01)
{
    ::new (voidify(address)) TARGET_TYPE(
                          BSLS_COMPILERFEATURES_FORWARD(Args_01,arguments_01));
    BSLMA_CONSTRUCTIONUTIL_XLC_PLACEMENT_NEW_FIX;
}

template <class TARGET_TYPE, class Args_01,
                             class Args_02>
inline
void
ConstructionUtil_Imp::construct(TARGET_TYPE      *address,
                                bsl::integral_constant<int, e_NIL_TRAITS> *,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02)
{
    ::new (voidify(address)) TARGET_TYPE(
                          BSLS_COMPILERFEATURES_FORWARD(Args_01,arguments_01),
                          BSLS_COMPILERFEATURES_FORWARD(Args_02,arguments_02));
    BSLMA_CONSTRUCTIONUTIL_XLC_PLACEMENT_NEW_FIX;
}

template <class TARGET_TYPE, class Args_01,
                             class Args_02,
                             class Args_03>
inline
void
ConstructionUtil_Imp::construct(TARGET_TYPE      *address,
                                bsl::integral_constant<int, e_NIL_TRAITS> *,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03)
{
    ::new (voidify(address)) TARGET_TYPE(
                          BSLS_COMPILERFEATURES_FORWARD(Args_01,arguments_01),
                          BSLS_COMPILERFEATURES_FORWARD(Args_02,arguments_02),
                          BSLS_COMPILERFEATURES_FORWARD(Args_03,arguments_03));
    BSLMA_CONSTRUCTIONUTIL_XLC_PLACEMENT_NEW_FIX;
}

template <class TARGET_TYPE, class Args_01,
                             class Args_02,
                             class Args_03,
                             class Args_04>
inline
void
ConstructionUtil_Imp::construct(TARGET_TYPE      *address,
                                bsl::integral_constant<int, e_NIL_TRAITS> *,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04)
{
    ::new (voidify(address)) TARGET_TYPE(
                          BSLS_COMPILERFEATURES_FORWARD(Args_01,arguments_01),
                          BSLS_COMPILERFEATURES_FORWARD(Args_02,arguments_02),
                          BSLS_COMPILERFEATURES_FORWARD(Args_03,arguments_03),
                          BSLS_COMPILERFEATURES_FORWARD(Args_04,arguments_04));
    BSLMA_CONSTRUCTIONUTIL_XLC_PLACEMENT_NEW_FIX;
}

template <class TARGET_TYPE, class Args_01,
                             class Args_02,
                             class Args_03,
                             class Args_04,
                             class Args_05>
inline
void
ConstructionUtil_Imp::construct(TARGET_TYPE      *address,
                                bsl::integral_constant<int, e_NIL_TRAITS> *,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05)
{
    ::new (voidify(address)) TARGET_TYPE(
                          BSLS_COMPILERFEATURES_FORWARD(Args_01,arguments_01),
                          BSLS_COMPILERFEATURES_FORWARD(Args_02,arguments_02),
                          BSLS_COMPILERFEATURES_FORWARD(Args_03,arguments_03),
                          BSLS_COMPILERFEATURES_FORWARD(Args_04,arguments_04),
                          BSLS_COMPILERFEATURES_FORWARD(Args_05,arguments_05));
    BSLMA_CONSTRUCTIONUTIL_XLC_PLACEMENT_NEW_FIX;
}

template <class TARGET_TYPE, class Args_01,
                             class Args_02,
                             class Args_03,
                             class Args_04,
                             class Args_05,
                             class Args_06>
inline
void
ConstructionUtil_Imp::construct(TARGET_TYPE      *address,
                                bsl::integral_constant<int, e_NIL_TRAITS> *,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06)
{
    ::new (voidify(address)) TARGET_TYPE(
                          BSLS_COMPILERFEATURES_FORWARD(Args_01,arguments_01),
                          BSLS_COMPILERFEATURES_FORWARD(Args_02,arguments_02),
                          BSLS_COMPILERFEATURES_FORWARD(Args_03,arguments_03),
                          BSLS_COMPILERFEATURES_FORWARD(Args_04,arguments_04),
                          BSLS_COMPILERFEATURES_FORWARD(Args_05,arguments_05),
                          BSLS_COMPILERFEATURES_FORWARD(Args_06,arguments_06));
    BSLMA_CONSTRUCTIONUTIL_XLC_PLACEMENT_NEW_FIX;
}

template <class TARGET_TYPE, class Args_01,
                             class Args_02,
                             class Args_03,
                             class Args_04,
                             class Args_05,
                             class Args_06,
                             class Args_07>
inline
void
ConstructionUtil_Imp::construct(TARGET_TYPE      *address,
                                bsl::integral_constant<int, e_NIL_TRAITS> *,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07)
{
    ::new (voidify(address)) TARGET_TYPE(
                          BSLS_COMPILERFEATURES_FORWARD(Args_01,arguments_01),
                          BSLS_COMPILERFEATURES_FORWARD(Args_02,arguments_02),
                          BSLS_COMPILERFEATURES_FORWARD(Args_03,arguments_03),
                          BSLS_COMPILERFEATURES_FORWARD(Args_04,arguments_04),
                          BSLS_COMPILERFEATURES_FORWARD(Args_05,arguments_05),
                          BSLS_COMPILERFEATURES_FORWARD(Args_06,arguments_06),
                          BSLS_COMPILERFEATURES_FORWARD(Args_07,arguments_07));
    BSLMA_CONSTRUCTIONUTIL_XLC_PLACEMENT_NEW_FIX;
}

template <class TARGET_TYPE, class Args_01,
                             class Args_02,
                             class Args_03,
                             class Args_04,
                             class Args_05,
                             class Args_06,
                             class Args_07,
                             class Args_08>
inline
void
ConstructionUtil_Imp::construct(TARGET_TYPE      *address,
                                bsl::integral_constant<int, e_NIL_TRAITS> *,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) arguments_08)
{
    ::new (voidify(address)) TARGET_TYPE(
                          BSLS_COMPILERFEATURES_FORWARD(Args_01,arguments_01),
                          BSLS_COMPILERFEATURES_FORWARD(Args_02,arguments_02),
                          BSLS_COMPILERFEATURES_FORWARD(Args_03,arguments_03),
                          BSLS_COMPILERFEATURES_FORWARD(Args_04,arguments_04),
                          BSLS_COMPILERFEATURES_FORWARD(Args_05,arguments_05),
                          BSLS_COMPILERFEATURES_FORWARD(Args_06,arguments_06),
                          BSLS_COMPILERFEATURES_FORWARD(Args_07,arguments_07),
                          BSLS_COMPILERFEATURES_FORWARD(Args_08,arguments_08));
    BSLMA_CONSTRUCTIONUTIL_XLC_PLACEMENT_NEW_FIX;
}

template <class TARGET_TYPE, class Args_01,
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
ConstructionUtil_Imp::construct(TARGET_TYPE      *address,
                                bsl::integral_constant<int, e_NIL_TRAITS> *,
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
    ::new (voidify(address)) TARGET_TYPE(
                          BSLS_COMPILERFEATURES_FORWARD(Args_01,arguments_01),
                          BSLS_COMPILERFEATURES_FORWARD(Args_02,arguments_02),
                          BSLS_COMPILERFEATURES_FORWARD(Args_03,arguments_03),
                          BSLS_COMPILERFEATURES_FORWARD(Args_04,arguments_04),
                          BSLS_COMPILERFEATURES_FORWARD(Args_05,arguments_05),
                          BSLS_COMPILERFEATURES_FORWARD(Args_06,arguments_06),
                          BSLS_COMPILERFEATURES_FORWARD(Args_07,arguments_07),
                          BSLS_COMPILERFEATURES_FORWARD(Args_08,arguments_08),
                          BSLS_COMPILERFEATURES_FORWARD(Args_09,arguments_09));
    BSLMA_CONSTRUCTIONUTIL_XLC_PLACEMENT_NEW_FIX;
}

template <class TARGET_TYPE, class Args_01,
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
ConstructionUtil_Imp::construct(TARGET_TYPE      *address,
                                bsl::integral_constant<int, e_NIL_TRAITS> *,
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
    ::new (voidify(address)) TARGET_TYPE(
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
    BSLMA_CONSTRUCTIONUTIL_XLC_PLACEMENT_NEW_FIX;
}

template <class TARGET_TYPE, class Args_01,
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
ConstructionUtil_Imp::construct(TARGET_TYPE      *address,
                                bsl::integral_constant<int, e_NIL_TRAITS> *,
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
    ::new (voidify(address)) TARGET_TYPE(
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
    BSLMA_CONSTRUCTIONUTIL_XLC_PLACEMENT_NEW_FIX;
}

template <class TARGET_TYPE, class Args_01,
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
ConstructionUtil_Imp::construct(TARGET_TYPE      *address,
                                bsl::integral_constant<int, e_NIL_TRAITS> *,
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
    ::new (voidify(address)) TARGET_TYPE(
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
    BSLMA_CONSTRUCTIONUTIL_XLC_PLACEMENT_NEW_FIX;
}

template <class TARGET_TYPE, class Args_01,
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
ConstructionUtil_Imp::construct(TARGET_TYPE      *address,
                                bsl::integral_constant<int, e_NIL_TRAITS> *,
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
    ::new (voidify(address)) TARGET_TYPE(
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
    BSLMA_CONSTRUCTIONUTIL_XLC_PLACEMENT_NEW_FIX;
}

template <class TARGET_TYPE, class Args_01,
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
ConstructionUtil_Imp::construct(TARGET_TYPE      *address,
                                bsl::integral_constant<int, e_NIL_TRAITS> *,
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
    ::new (voidify(address)) TARGET_TYPE(
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
    BSLMA_CONSTRUCTIONUTIL_XLC_PLACEMENT_NEW_FIX;
}

template <class TARGET_TYPE, class Args_01,
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
                             class Args_14,
                             class Args_15>
inline
void
ConstructionUtil_Imp::construct(TARGET_TYPE      *address,
                                bsl::integral_constant<int, e_NIL_TRAITS> *,
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
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_14) arguments_14,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_15) arguments_15)
{
    ::new (voidify(address)) TARGET_TYPE(
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
                          BSLS_COMPILERFEATURES_FORWARD(Args_14,arguments_14),
                          BSLS_COMPILERFEATURES_FORWARD(Args_15,arguments_15));
    BSLMA_CONSTRUCTIONUTIL_XLC_PLACEMENT_NEW_FIX;
}

#else
// The generated code below is a workaround for the absence of perfect
// forwarding in some compilers.
template <class TARGET_TYPE, class... Args>
inline
void
ConstructionUtil_Imp::construct(
                  TARGET_TYPE      *address,
                  bslma::Allocator *allocator,
                  bsl::integral_constant<int, e_USES_BSLMA_ALLOCATOR_TRAITS> *,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args)... arguments)
{
    ::new (voidify(address)) TARGET_TYPE(
                  BSLS_COMPILERFEATURES_FORWARD(Args,arguments)..., allocator);
    BSLMA_CONSTRUCTIONUTIL_XLC_PLACEMENT_NEW_FIX;
}

template <class TARGET_TYPE, class... Args>
inline
void
ConstructionUtil_Imp::construct(
                  TARGET_TYPE      *address,
                  bslma::Allocator *allocator,
                  bsl::integral_constant<int, e_USES_ALLOCATOR_ARG_T_TRAITS> *,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args)... arguments)
{
    ::new (voidify(address)) TARGET_TYPE(
                             bsl::allocator_arg,
                             allocator,
                             BSLS_COMPILERFEATURES_FORWARD(Args,arguments)...);
    BSLMA_CONSTRUCTIONUTIL_XLC_PLACEMENT_NEW_FIX;
}

template <class TARGET_TYPE, class... Args>
inline
void
ConstructionUtil_Imp::construct(TARGET_TYPE      *address,
                                bslma::Allocator *,
                                bsl::integral_constant<int, e_NIL_TRAITS> *,
                          BSLS_COMPILERFEATURES_FORWARD_REF(Args)... arguments)
{
    construct(address,
              (bsl::integral_constant<int, e_NIL_TRAITS> *)0,
              BSLS_COMPILERFEATURES_FORWARD(Args,arguments)...);
}

template <class TARGET_TYPE, class... Args>
inline
void
ConstructionUtil_Imp::construct(TARGET_TYPE      *address,
                                bsl::integral_constant<int, e_NIL_TRAITS> *,
                          BSLS_COMPILERFEATURES_FORWARD_REF(Args)... arguments)
{
    ::new (voidify(address)) TARGET_TYPE(
                             BSLS_COMPILERFEATURES_FORWARD(Args,arguments)...);
    BSLMA_CONSTRUCTIONUTIL_XLC_PLACEMENT_NEW_FIX;
}
// }}} END GENERATED CODE
#endif

#if defined(BSLS_PLATFORM_CMP_MSVC) && BSLS_PLATFORM_CMP_VERSION < 1900
template <class TARGET_TYPE>
inline
void
ConstructionUtil_Imp::defaultConstructScalar(bsl::false_type,
                                             TARGET_TYPE *address)
{
    ::new (voidify(address)) TARGET_TYPE();
}

template <class TARGET_TYPE>
inline
void
ConstructionUtil_Imp::defaultConstructScalar(bsl::true_type,
                                             TARGET_TYPE *address)
{
    ::new (voidify(address)) TARGET_TYPE(nullptr);
}
#endif

template <class TARGET_TYPE, class ALLOCATOR>
inline
void
ConstructionUtil_Imp::destructiveMove(TARGET_TYPE *address,
                                      ALLOCATOR   *,
                       bsl::integral_constant<int, e_BITWISE_MOVABLE_TRAITS> *,
                                      TARGET_TYPE *original)
{
    if (bsl::is_fundamental<TARGET_TYPE>::value
     || bsl::is_pointer<TARGET_TYPE>::value) {
         ::new (voidify(address)) TARGET_TYPE(*original);
         BSLMA_CONSTRUCTIONUTIL_XLC_PLACEMENT_NEW_FIX;
     }
     else {
         memcpy((void *)address, original, sizeof *original);
     }
}

template <class TARGET_TYPE, class ALLOCATOR>
inline
void
ConstructionUtil_Imp::destructiveMove(TARGET_TYPE *address,
                                      ALLOCATOR   *allocator,
                                   bsl::integral_constant<int, e_NIL_TRAITS> *,
                                      TARGET_TYPE *original)
{
    // TBD: should be ok with C++03 as well, but need to test edge cases first
#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
    ConstructionUtil::construct(address,
                                allocator,
                                bslmf::MovableRefUtil::move(*original));
#else
    ConstructionUtil::construct(address, allocator, *original);
#endif
    DestructionUtil::destroy(original);
}

template <class TARGET_TYPE>
inline
void *ConstructionUtil_Imp::voidify(TARGET_TYPE *address)
{
    return static_cast<void *>(
            const_cast<typename bsl::remove_cv<TARGET_TYPE>::type *>(address));
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
