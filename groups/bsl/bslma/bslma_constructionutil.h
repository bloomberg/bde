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
// This component provides full support for in-place construction of objects
// such that the object type's allocator policy is respected and all arguments
// are perfectly forwarded to the appropriate constructor.  This component also
// provides partial support for the C++20 'std::make_obj_using_allocator'
// utility via the (overloaded) 'bslma::ConstructionUtil::make' method.
// Currently, 'make' supports only default construction and construction from
// one (non-allocator) argument.
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
//      explicit MyContainer(bslma::Allocator *basicAllocator = 0);
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
//      MyContainer& operator=(const TYPE& rhs);
//      MyContainer& operator=(const MyContainer& rhs);
//          // Assign to this object the value of the specified 'rhs' object,
//          // and return a reference providing modifiable access to this
//          // object.
//
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
// Next, the assignment operator needs to assign the value without modifying
// the allocator.
//..
//  template <class TYPE>
//  MyContainer<TYPE>& MyContainer<TYPE>::operator=(const TYPE& rhs)
//  {
//      *d_value_p = rhs;
//      return *this;
//  }
//
//  template <class TYPE>
//  MyContainer<TYPE>& MyContainer<TYPE>::operator=(const MyContainer& rhs)
//  {
//      *d_value_p = *rhs.d_value_p;
//      return *this;
//  }
//..
// Finally, we perform a simple test of 'MyContainer', instantiating it with
// element type 'int':
//..
//  int main()
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
//
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
//          (void) original;
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
//  int main()
//  {
//      bslma::TestAllocator testAlloc;
//      MyContainer<MyType>  C1(&testAlloc);
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
//
///Example 3: Constructing into Non-heap Memory
///- - - - - - - - - - - - - - - - - - - - - -
// This example demonstrates using 'bslma::ConstructionUtil::make' to
// implement a simple wrapper class that contains a single item that might or
// might not use the 'bslma' allocator protocol.
//
// First, we define a wrapper class that holds an object and a functor and
// calls the functor (called the listener) each time the wrapped object is
// changed.  We store the object directly as a member variable, instead of
// using an uninitialized buffer, to avoid a separate construction step:
//..
//  template <class TYPE, class FUNC>
//  class MyTriggeredWrapper {
//      // This class is a wrapper around an object of the specified 'TYPE'
//      // that triggers a call to an object, called the "listener", of the
//      // specified 'FUNC' invocable type whenever the wrapped object is
//      // changed.
//
//      // DATA
//      TYPE d_value;
//      FUNC d_listener;
//
//    public:
//      // CREATORS
//      explicit MyTriggeredWrapper(const FUNC&       f,
//                                  bslma::Allocator *basicAllocator = 0);
//      MyTriggeredWrapper(const TYPE&       v,
//                         const FUNC&       f,
//                         bslma::Allocator *basicAllocator = 0);
//          // Create an object with the specified 'f' as the listener to be
//          // called when a change is triggered.  Optionally specify 'v' as
//          // the wrapped value; otherwise the wrapped value is default
//          // constructed.  Optionally specify 'basicAllocator' to supply
//          // memory; otherwise the current default allocator is used.  If
//          // 'TYPE' is not allocator aware, 'basicAllocator' is ignored.
//
//      MyTriggeredWrapper(const MyTriggeredWrapper&  original,
//                         bslma::Allocator          *basicAllocator = 0);
//          // Create a copy of the specified 'original'.  Optionally specify
//          // 'basicAllocator' to supply memory; otherwise the current
//          // default allocator is used.
//
//      ~MyTriggeredWrapper()
//          // Destroy the wrapped object and listener.
//      {
//      }
//
//      // MANIPULATORS
//      MyTriggeredWrapper& operator=(const TYPE& rhs);
//      MyTriggeredWrapper& operator=(const MyTriggeredWrapper& rhs);
//          // Assign the wrapped value to the value of the specified 'rhs',
//          // invoke the listener with the new value, and return a reference
//          // providing modifiable access to this object.  Note that the
//          // listener itself is not assigned.
//
//      void setValue(const TYPE& value);
//          // Set the wrapped value to the specified 'value' and invoke the
//          // listener with the new value.
//
//      // ACCESSORS
//      const TYPE& value() const
//          // Return a reference providing read-only access to the wrapped
//          // value.
//      {
//          return d_value;
//      }
//
//      const FUNC& listener() const
//          // Return a reference providing read-only access to the listener.
//      {
//          return d_listener;
//      }
//  };
//..
// Next, we define the constructors such that they initialize 'd_value' using
// the specified allocator if and only if 'TYPE' accepts an allocator.  The
// 'bslma::ConstructionUtil::make' family of functions encapsulate all of the
// metaprogramming that detects whether or not 'TYPE' uses an allocator and,
// if so, which construction protocol it uses (allocator at the front or at
// the back of the argument list), making all three constructors straight-
// forward:
//..
//  template <class TYPE, class FUNC>
//  MyTriggeredWrapper<TYPE, FUNC>::MyTriggeredWrapper(
//                                            const FUNC&       f,
//                                            bslma::Allocator *basicAllocator)
//  : d_value(bslma::ConstructionUtil::make<TYPE>(basicAllocator))
//  , d_listener(f)
//  {
//  }
//
//  template <class TYPE, class FUNC>
//  MyTriggeredWrapper<TYPE, FUNC>::MyTriggeredWrapper(
//                                            const TYPE&       v,
//                                            const FUNC&       f,
//                                            bslma::Allocator *basicAllocator)
//  : d_value(bslma::ConstructionUtil::make<TYPE>(basicAllocator, v))
//  , d_listener(f)
//  {
//  }
//
//  template <class TYPE, class FUNC>
//  MyTriggeredWrapper<TYPE, FUNC>::MyTriggeredWrapper(
//                                   const MyTriggeredWrapper&  other,
//                                   bslma::Allocator          *basicAllocator)
//  : d_value(bslma::ConstructionUtil::make<TYPE>(basicAllocator,
//                                                other.value()))
//  , d_listener(other.d_listener)
//  {
//  }
//..
// Note that, in order for 'd_value' to be constructed with the correct
// allocator, the compiler must construct the result returned by 'make'
// directly into the 'd_value' variable, an optimization formerly known prior
// to C++17 as "copy elision".  This optimization is required by the C++17
// standard and is optional in pre-2017 standards, but is implemented in all
// of the compilers for which this component is expected to be used at
// Bloomberg.
//
// Next, we implement the assignment operators, which simply call 'setValue':
//..
//  template <class TYPE, class FUNC>
//  MyTriggeredWrapper<TYPE, FUNC>&
//  MyTriggeredWrapper<TYPE, FUNC>::operator=(const TYPE& rhs)
//  {
//      setValue(rhs);
//      return *this;
//  }
//
//  template <class TYPE, class FUNC>
//  MyTriggeredWrapper<TYPE, FUNC>&
//  MyTriggeredWrapper<TYPE, FUNC>::operator=(const MyTriggeredWrapper& rhs)
//  {
//      setValue(rhs.value());
//      return *this;
//  }
//..
// Then, we implement 'setValue', which calls the listener after modifying the
// value:
//..
//  template <class TYPE, class FUNC>
//  void MyTriggeredWrapper<TYPE, FUNC>::setValue(const TYPE& value)
//  {
//      d_value = value;
//      d_listener(d_value);
//  }
//..
// Finally, we check our work by creating a listener for 'MyContainer<int>'
// that stores its last-seen value in a known location and a wrapper around
// 'MyContainer<int>' to test it:
//..
//  int lastSeen = 0;
//  void myListener(const MyContainer<int>& c)
//  {
//      lastSeen = c.front();
//  }
//
//  void main()
//  {
//      bslma::TestAllocator testAlloc;
//      MyTriggeredWrapper<MyContainer<int>,
//                         void (*)(const MyContainer<int>&)>
//                           wrappedContainer(myListener, &testAlloc);
//      assert(&testAlloc == wrappedContainer.value().allocator());
//
//      wrappedContainer = MyContainer<int>(99);
//
//      assert(99 == lastSeen);
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
#include <bslmf_util.h>    // 'forward(V)'

#include <bsls_assert.h>
#include <bsls_compilerfeatures.h>
#include <bsls_libraryfeatures.h>
#include <bsls_platform.h>
#include <bsls_util.h>     // 'forward<T>(V)'

#include <stddef.h>
#include <string.h>

#include <new>          // placement 'new'

#ifndef BDE_DONT_ALLOW_TRANSITIVE_INCLUDES
#include <bslma_destructorproctor.h>
#endif

#if BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
// Include version that can be compiled with C++03
// Generated on Mon Oct  5 16:41:00 2020
// Command line: sim_cpp11_features.pl bslma_constructionutil.h
# define COMPILING_BSLMA_CONSTRUCTIONUTIL_H
# include <bslma_constructionutil_cpp03.h>
# undef COMPILING_BSLMA_CONSTRUCTIONUTIL_H
#else

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
        // Create a default-constructed object of (template parameter)
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
        // Create an object of (template parameter) 'TARGET_TYPE', having
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
        // Create an object of (template parameter) 'TARGET_TYPE', having
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
    template <class TARGET_TYPE, class ARG1, class... ARGS>
    static void construct(TARGET_TYPE      *address,
                          bslma::Allocator *allocator,
                          ARG1&&            argument,
                          ARGS&&...         arguments);
    template <class TARGET_TYPE, class ARG1, class... ARGS>
    static void construct(TARGET_TYPE *address,
                          void        *allocator,
                          ARG1&&       argument,
                          ARGS&&...    arguments);
        // Create an object of (template parameter) 'TARGET_TYPE' at the
        // specified 'address', constructed by forwarding the specified
        // 'argument' and the (variable number of) additional specified
        // 'arguments' to the corresponding constructor of 'TARGET_TYPE'.  If
        // the specified 'allocator' is of type 'bslma::Allocator *' and
        // 'TARGET_TYPE' supports 'bslma'-style allocation, the allocator is
        // passed to the constructor (as the last argument).  If the
        // constructor throws, the memory at 'address' is left in an
        // unspecified state.
#endif

    template <class TARGET_TYPE, class ALLOCATOR>
    static void destructiveMove(TARGET_TYPE *address,
                                ALLOCATOR   *allocator,
                                TARGET_TYPE *original);
        // Create an object of (template parameter) 'TARGET_TYPE' at the
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

#if defined(BSLS_COMPILERFEATURES_GUARANTEED_COPY_ELISION)
    template <class TARGET_TYPE>
    static TARGET_TYPE make(bslma::Allocator *allocator);
    template <class TARGET_TYPE>
    static TARGET_TYPE make(void             *allocator);
        // Return, by value, an object of the specified (template parameter)
        // 'TARGET_TYPE', having default value.  If the specified 'allocator'
        // is a pointer to a class derived from 'bslma::Allocator *' and
        // 'TARGET_TYPE' supports 'bslma'-style allocation, 'allocator' is
        // propagated to the newly created object; otherwise, 'allocator' is
        // ignored.  Note that this method is available only for compilers that
        // reliably implement copy/move elision (i.e., RVO) on the returned
        // object.  This copy/move elision is required starting with C++17 and
        // is widely implemented, though optional, prior to C++17.

    template <class TARGET_TYPE, class ANY_TYPE>
    static TARGET_TYPE make(
                        bslma::Allocator                            *allocator,
                        BSLS_COMPILERFEATURES_FORWARD_REF(ANY_TYPE)  argument);
    template <class TARGET_TYPE,class ANY_TYPE>
    static TARGET_TYPE make(
                        void                                        *allocator,
                        BSLS_COMPILERFEATURES_FORWARD_REF(ANY_TYPE)  argument);
        // Return, by value, an object of the specified (template parameter)
        // 'TARGET_TYPE', constructed from the specified 'argument'.  If the
        // specified 'allocator' is a pointer to a class derived from
        // 'bslma::Allocator *' and 'TARGET_TYPE' supports 'bslma'-style
        // allocation, 'allocator' is propagated to the newly created object;
        // otherwise, 'allocator' is ignored.  Note that this method is
        // available only for compilers that reliably implement copy/move
        // elision (i.e., RVO) on the returned object.  This copy/move elision
        // is required starting with C++17 and is widely implemented, though
        // optional, prior to C++17.

#endif // defined(BSLS_COMPILERFEATURES_GUARANTEED_COPY_ELISION)

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
        // Construct a default instance of (template parameter) 'TARGET_TYPE'
        // that has a trivial default constructor, at the specified 'address'.
        // If the constructor throws, the memory at 'address' is left in an
        // unspecified state.  Note that the behavior is undefined if
        // 'TARGET_TYPE' supports 'bslma'-style allocators.

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
        // Create an object of a bitwise copyable (template parameter)
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
        // Create an object of a bitwise moveable (template parameter)
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
    template <class TARGET_TYPE, class... ARGS>
    static void construct(
                  TARGET_TYPE      *address,
                  bslma::Allocator *allocator,
                  bsl::integral_constant<int, e_USES_BSLMA_ALLOCATOR_TRAITS> *,
                  ARGS&&...         arguments);
    template <class TARGET_TYPE, class... ARGS>
    static void construct(
                  TARGET_TYPE      *address,
                  bslma::Allocator *allocator,
                  bsl::integral_constant<int, e_USES_ALLOCATOR_ARG_T_TRAITS> *,
                  ARGS&&...         arguments);
        // Construct an object at the specified 'address' having the specified
        // (template parameter) 'TARGET_TYPE' that supports 'bslma'-style
        // allocators.  Invoke the constructor of 'TARGET_TYPE' using the
        // specified 'arguments'.  For the 'e_USES_BSLMA_ALLOCATOR_TRAITS'
        // overload, the specified 'allocator' is passed as an additional last
        // argument to the constructor.  For the
        // 'e_USES_ALLOCATOR_ARG_T_TRAITS' overload, the 'allocator' is passed
        // as the second argument to the constructor, preceded by
        // 'bsl::allocator_arg'.  If the constructor throws, the memory at
        // 'address' is left in an uninitialized state.

    template <class TARGET_TYPE, class... ARGS>
    static void construct(TARGET_TYPE      *address,
                          bslma::Allocator *allocator,
                          bsl::integral_constant<int, e_NIL_TRAITS> *,
                          ARGS&&...         arguments);

    template <class TARGET_TYPE, class... ARGS>
    static void construct(TARGET_TYPE      *address,
                          bsl::integral_constant<int, e_NIL_TRAITS> *,
                          ARGS&&...         arguments);
        // Construct an object of (template parameter) 'TARGET_TYPE', that does
        // not support 'bslma'-style allocators, at the specified 'address',
        // invoking the constructor of 'TARGET_TYPE' corresponding to the
        // specified 'arguments'.  If the constructor throws, the memory at
        // 'address' is left in an unspecified state.
#endif

#if defined(BSLS_PLATFORM_CMP_MSVC) && BSLS_PLATFORM_CMP_VERSION < 1900
    template <class TARGET_TYPE>
    static void defaultConstructScalar(bsl::false_type, TARGET_TYPE *address);
        // Value-initialize a scalar object of the (template parameter)
        // 'TARGET_TYPE' at the specified 'address'.  The unused
        // 'bsl::false_type' value indicates that the scalar 'TARGET_TYPE' is
        // not a pointer-to-member.

    template <class TARGET_TYPE>
    static void defaultConstructScalar(bsl::true_type, TARGET_TYPE *address);
        // Value-initialize a pointer-to-member at the specified 'address' to
        // the null pointer value.  Note that early versions of the Microsoft
        // Visual C++ compiler would fail to initialize such an object when
        // requested with the simple value-initialization syntax of
        // 'new (address) TYPE();', requiring this additional workaround.
#endif

    template <class TARGET_TYPE, class ALLOCATOR>
    static void destructiveMove(
              TARGET_TYPE                                           *address,
              ALLOCATOR                                             *allocator,
              bsl::integral_constant<int, e_BITWISE_MOVABLE_TRAITS> *,
              TARGET_TYPE                                           *original);
        // Move the bitwise movable object of (template parameter)
        // 'TARGET_TYPE' at the specified 'original' address to the specified
        // 'address', eliding the call to the move constructor and destructor
        // in favor of performing a bitwise copy.  The behavior is undefined
        // unless either 'TARGET_TYPE' does not support 'bslma'-style
        // allocation or 'original' uses the specified 'allocator' to supply
        // memory.

    template <class TARGET_TYPE, class ALLOCATOR>
    static void destructiveMove(
                          TARGET_TYPE                               *address,
                          ALLOCATOR                                 *allocator,
                          bsl::integral_constant<int, e_NIL_TRAITS> *,
                          TARGET_TYPE                               *original);
        // Create an object of (template parameter) 'TARGET_TYPE' at the
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

#if defined(BSLS_COMPILERFEATURES_GUARANTEED_COPY_ELISION)
    template <class TARGET_TYPE>
    static TARGET_TYPE make(
         bslma::Allocator                                           *allocator,
         bsl::integral_constant<int, e_USES_BSLMA_ALLOCATOR_TRAITS> *);
    template <class TARGET_TYPE>
    static TARGET_TYPE make(
         bslma::Allocator                                           *allocator,
         bsl::integral_constant<int, e_USES_ALLOCATOR_ARG_T_TRAITS> *);
    template <class TARGET_TYPE>
    static TARGET_TYPE make(bslma::Allocator *allocator,
                            bsl::integral_constant<int, e_NIL_TRAITS> *);
        // Return, by value, a default-constructed object of the specified
        // (template parameter) 'TARGET_TYPE', using the specified 'allocator'
        // to supply memory.  The 'integral_constant' pointer argument is used
        // to dispatch on various traits so that the correct constructor is
        // invoked for the specified 'TARGET_TYPE'.

    template <class TARGET_TYPE, class ANY_TYPE>
    static TARGET_TYPE make(
         bslma::Allocator                                           *allocator,
         bsl::integral_constant<int, e_USES_BSLMA_ALLOCATOR_TRAITS> *,
         BSLS_COMPILERFEATURES_FORWARD_REF(ANY_TYPE)                 argument);
    template <class TARGET_TYPE, class ANY_TYPE>
    static TARGET_TYPE make(
         bslma::Allocator                                           *allocator,
         bsl::integral_constant<int, e_USES_ALLOCATOR_ARG_T_TRAITS> *,
         BSLS_COMPILERFEATURES_FORWARD_REF(ANY_TYPE)                 argument);
    template <class TARGET_TYPE, class ANY_TYPE>
    static TARGET_TYPE make(
                        bslma::Allocator                            *allocator,
                        bsl::integral_constant<int, e_NIL_TRAITS>   *,
                        BSLS_COMPILERFEATURES_FORWARD_REF(ANY_TYPE)  argument);
        // Return, by value, an object of the specified (template parameter)
        // 'TARGET_TYPE', constructed from the specified 'argument' object,
        // using the specified 'allocator' to supply memory.  The
        // 'integral_constant' pointer argument is used to dispatch on various
        // traits so that the correct constructor is invoked for the specified
        // 'TARGET_TYPE'.

#endif // defined(BSLS_COMPILERFEATURES_GUARANTEED_COPY_ELISION)

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
template <class TARGET_TYPE, class ARG1, class... ARGS>
inline
void
ConstructionUtil::construct(TARGET_TYPE      *address,
                            bslma::Allocator *allocator,
                            ARG1&&            argument1,
                            ARGS&&...         arguments)
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
                   BSLS_COMPILERFEATURES_FORWARD(ARG1,argument1),
                   BSLS_COMPILERFEATURES_FORWARD(ARGS,arguments)...);
}

template <class TARGET_TYPE, class ARG1, class... ARGS>
inline
void
ConstructionUtil::construct(TARGET_TYPE *address,
                            void        *,
                            ARG1&&       argument1,
                            ARGS&&...    arguments)
{
    ::new (Imp::voidify(address)) TARGET_TYPE(
                             BSLS_COMPILERFEATURES_FORWARD(ARG1,argument1),
                             BSLS_COMPILERFEATURES_FORWARD(ARGS,arguments)...);
    BSLMA_CONSTRUCTIONUTIL_XLC_PLACEMENT_NEW_FIX;
}
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

#if defined(BSLS_COMPILERFEATURES_GUARANTEED_COPY_ELISION)
// Suppress bde_verify warnings about return-by-value in this region.
// BDE_VERIFY pragma: push
// BDE_VERIFY pragma: -AR01: Type using allocator is returned by value

template <class TARGET_TYPE>
inline
TARGET_TYPE
ConstructionUtil::make(bslma::Allocator *allocator)
{
    enum {
        k_VALUE = bslma::UsesBslmaAllocator<TARGET_TYPE>::value
                ? (bslmf::UsesAllocatorArgT<TARGET_TYPE>::value
                   ? Imp::e_USES_ALLOCATOR_ARG_T_TRAITS
                   : Imp::e_USES_BSLMA_ALLOCATOR_TRAITS)
                : Imp::e_NIL_TRAITS
    };

    return Imp::make<TARGET_TYPE>(allocator,
                                  (bsl::integral_constant<int, k_VALUE> *) 0);
}

template <class TARGET_TYPE>
inline
TARGET_TYPE
ConstructionUtil::make(void *)
{
    return TARGET_TYPE();
}

template <class TARGET_TYPE, class ANY_TYPE>
inline
TARGET_TYPE
ConstructionUtil::make(bslma::Allocator                            *allocator,
                       BSLS_COMPILERFEATURES_FORWARD_REF(ANY_TYPE)  argument)
{
    enum {
        k_VALUE = bslma::UsesBslmaAllocator<TARGET_TYPE>::value
                ? (bslmf::UsesAllocatorArgT<TARGET_TYPE>::value
                   ? Imp::e_USES_ALLOCATOR_ARG_T_TRAITS
                   : Imp::e_USES_BSLMA_ALLOCATOR_TRAITS)
                : Imp::e_NIL_TRAITS
    };

    return Imp::make<TARGET_TYPE>(allocator,
                                  (bsl::integral_constant<int, k_VALUE> *) 0,
                                  BSLS_COMPILERFEATURES_FORWARD(ANY_TYPE,
                                                                argument));
}

template <class TARGET_TYPE, class ANY_TYPE>
inline
TARGET_TYPE
ConstructionUtil::make(void                                        *,
                       BSLS_COMPILERFEATURES_FORWARD_REF(ANY_TYPE)  argument)
{
    return TARGET_TYPE(BSLS_COMPILERFEATURES_FORWARD(ANY_TYPE, argument));
}

// BDE_VERIFY pragma: pop
#endif // defined(BSLS_COMPILERFEATURES_GUARANTEED_COPY_ELISION)

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
void ConstructionUtil_Imp::construct(
              TARGET_TYPE                                            *address,
              bslma::Allocator                                       *,
              bsl::integral_constant<int, e_BITWISE_COPYABLE_TRAITS> *,
              const TARGET_TYPE&                                      original)
{
    construct(address,
              (bsl::integral_constant<int, e_BITWISE_COPYABLE_TRAITS>*)0,
               original);
}

template <class TARGET_TYPE>
inline
void ConstructionUtil_Imp::construct(
              TARGET_TYPE                                            *address,
              bsl::integral_constant<int, e_BITWISE_COPYABLE_TRAITS> *,
              const TARGET_TYPE&                                      original)
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
              bslmf::MovableRef<TARGET_TYPE>                          original)
{
    construct(address,
              (bsl::integral_constant<int, e_BITWISE_COPYABLE_TRAITS>*)0,
              BSLS_COMPILERFEATURES_FORWARD(TARGET_TYPE, original));
}

template <class TARGET_TYPE>
inline
void ConstructionUtil_Imp::construct(
              TARGET_TYPE                                            *address,
              bsl::integral_constant<int, e_BITWISE_COPYABLE_TRAITS> *,
              bslmf::MovableRef<TARGET_TYPE>                          original)
{
    ::new (voidify(address)) TARGET_TYPE(BSLS_COMPILERFEATURES_FORWARD(
        TARGET_TYPE, bslmf::MovableRefUtil::move(original)));
}

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES // $var-args=15
template <class TARGET_TYPE, class... ARGS>
inline
void
ConstructionUtil_Imp::construct(
         TARGET_TYPE                                                *address,
         bslma::Allocator                                           *allocator,
         bsl::integral_constant<int, e_USES_BSLMA_ALLOCATOR_TRAITS> *,
         ARGS&&...                                                   arguments)
{
    ::new (voidify(address)) TARGET_TYPE(
                  BSLS_COMPILERFEATURES_FORWARD(ARGS,arguments)..., allocator);
    BSLMA_CONSTRUCTIONUTIL_XLC_PLACEMENT_NEW_FIX;
}

template <class TARGET_TYPE, class... ARGS>
inline
void
ConstructionUtil_Imp::construct(
         TARGET_TYPE                                                *address,
         bslma::Allocator                                           *allocator,
         bsl::integral_constant<int, e_USES_ALLOCATOR_ARG_T_TRAITS> *,
         ARGS&&...                                                   arguments)
{
    ::new (voidify(address)) TARGET_TYPE(
                             bsl::allocator_arg,
                             allocator,
                             BSLS_COMPILERFEATURES_FORWARD(ARGS,arguments)...);
    BSLMA_CONSTRUCTIONUTIL_XLC_PLACEMENT_NEW_FIX;
}

template <class TARGET_TYPE, class... ARGS>
inline
void
ConstructionUtil_Imp::construct(
                          TARGET_TYPE                               *address,
                          bslma::Allocator                          *,
                          bsl::integral_constant<int, e_NIL_TRAITS> *,
                          ARGS&&...                                  arguments)
{
    construct(address,
              (bsl::integral_constant<int, e_NIL_TRAITS> *)0,
              BSLS_COMPILERFEATURES_FORWARD(ARGS, arguments)...);
}

template <class TARGET_TYPE, class... ARGS>
inline
void
ConstructionUtil_Imp::construct(
                          TARGET_TYPE                               *address,
                          bsl::integral_constant<int, e_NIL_TRAITS> *,
                          ARGS&&...                                  arguments)
{
    ::new (voidify(address)) TARGET_TYPE(
                             BSLS_COMPILERFEATURES_FORWARD(ARGS,arguments)...);
    BSLMA_CONSTRUCTIONUTIL_XLC_PLACEMENT_NEW_FIX;
}
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
ConstructionUtil_Imp::destructiveMove(
               TARGET_TYPE                                           *address,
               ALLOCATOR                                             *,
               bsl::integral_constant<int, e_BITWISE_MOVABLE_TRAITS> *,
               TARGET_TYPE                                           *original)
{
    if (bsl::is_fundamental<TARGET_TYPE>::value ||
        bsl::is_pointer<TARGET_TYPE>::value) {
        ::new (voidify(address)) TARGET_TYPE(*original);
        BSLMA_CONSTRUCTIONUTIL_XLC_PLACEMENT_NEW_FIX;
    }
    else {
        // voidify(address) is used here to suppress compiler warning
        // "-Wclass-memaccess".
        memcpy(voidify(address), original, sizeof *original);
    }
}

template <class TARGET_TYPE, class ALLOCATOR>
inline
void
ConstructionUtil_Imp::destructiveMove(
                          TARGET_TYPE                               *address,
                          ALLOCATOR                                 *allocator,
                          bsl::integral_constant<int, e_NIL_TRAITS> *,
                          TARGET_TYPE                               *original)
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

#if defined(BSLS_COMPILERFEATURES_GUARANTEED_COPY_ELISION)
// Suppress bde_verify warnings about return-by-value in this region.
// BDE_VERIFY pragma: push
// BDE_VERIFY pragma: -AR01: Type using allocator is returned by value

template <class TARGET_TYPE>
inline
TARGET_TYPE
ConstructionUtil_Imp::make(
         bslma::Allocator                                           *allocator,
         bsl::integral_constant<int, e_USES_BSLMA_ALLOCATOR_TRAITS> *)
{
    return TARGET_TYPE(allocator);
}

template <class TARGET_TYPE>
inline
TARGET_TYPE
ConstructionUtil_Imp::make(
         bslma::Allocator                                           *allocator,
         bsl::integral_constant<int, e_USES_ALLOCATOR_ARG_T_TRAITS> *)
{
    return TARGET_TYPE(bsl::allocator_arg, allocator);
}

template <class TARGET_TYPE>
inline
TARGET_TYPE
ConstructionUtil_Imp::make(bslma::Allocator                          *,
                           bsl::integral_constant<int, e_NIL_TRAITS> *)
{
    return TARGET_TYPE();
}

template <class TARGET_TYPE, class ANY_TYPE>
inline
TARGET_TYPE
ConstructionUtil_Imp::make(
         bslma::Allocator                                           *allocator,
         bsl::integral_constant<int, e_USES_BSLMA_ALLOCATOR_TRAITS> *,
         BSLS_COMPILERFEATURES_FORWARD_REF(ANY_TYPE)                 argument)
{
    return TARGET_TYPE(BSLS_COMPILERFEATURES_FORWARD(ANY_TYPE, argument),
                       allocator);
}

template <class TARGET_TYPE, class ANY_TYPE>
inline
TARGET_TYPE
ConstructionUtil_Imp::make(
         bslma::Allocator                                           *allocator,
         bsl::integral_constant<int, e_USES_ALLOCATOR_ARG_T_TRAITS> *,
         BSLS_COMPILERFEATURES_FORWARD_REF(ANY_TYPE)                 argument)
{
    return TARGET_TYPE(bsl::allocator_arg, allocator,
                       BSLS_COMPILERFEATURES_FORWARD(ANY_TYPE, argument));
}

template <class TARGET_TYPE, class ANY_TYPE>
inline
TARGET_TYPE
ConstructionUtil_Imp::make(
                         bslma::Allocator                            *,
                         bsl::integral_constant<int, e_NIL_TRAITS>   *,
                         BSLS_COMPILERFEATURES_FORWARD_REF(ANY_TYPE)  argument)
{
    return TARGET_TYPE(BSLS_COMPILERFEATURES_FORWARD(ANY_TYPE, argument));
}

// BDE_VERIFY pragma: pop
#endif // defined(BSLS_COMPILERFEATURES_GUARANTEED_COPY_ELISION)

template <class TARGET_TYPE>
inline
void *ConstructionUtil_Imp::voidify(TARGET_TYPE *address)
{
    return static_cast<void *>(
            const_cast<typename bsl::remove_cv<TARGET_TYPE>::type *>(address));
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
