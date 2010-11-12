// bdeut_constructorproxy.h                -*-C++-*-
#ifndef INCLUDED_BDEUT_CONSTRUCTORPROXY
#define INCLUDED_BDEUT_CONSTRUCTORPROXY

//@PURPOSE: Provide a proxy for constructing and destroying objects.
//
//@CLASSES:
//    bdeut_ConstructorProxy: proxy for constructing and destroying objects
//
//@AUTHOR: Shezan Baig (sbaig)
//
//@SEE_ALSO: bdema_allocator, bdealg_typetraits
//
//@DESCRIPTION: This component provides a proxy for constructing and
// automatically destroying objects.  The proxy class 'bdeut_ConstructorProxy'
// is parameterized on a 'TYPE', where 'TYPE' may or may not use a 'bdema'
// allocator to supply memory.  Upon construction of a proxy, a proxied 'TYPE'
// object is also constructed; the 'bdema' allocator supplied to the proxy
// constructor is passed to the constructor of the proxied object only if
// 'TYPE' declares the 'bdealg_TypeTraitUsesBdemaAllocator' trait.  If this
// trait is not declared for 'TYPE', the allocator is ignored.
//
// Following construction of the proxied object, it is held by the proxy.
// Modifiable and non-modifiable access to the proxied object may be obtained
// using the overloaded 'object' methods.  When the proxy is destroyed, the
// proxied object is automatically destroyed.
//
// This proxy is useful in situations where an object of a given type must be
// constructed, but it is not known whether the object's constructor takes a
// 'bdema' allocator to supply memory.  This occurs frequently in generic
// programming.
//
// See the 'bdema' package-level documentation for more information about using
// 'bdema' allocators.  Also see the "I want to use 'saverange' in C++"
// document at the BDE Syndication website for more information about
// propagating allocators from containers to their contained objects.  This
// document is available at the following URL:
//..
//  http://sundev3.bloomberg.com/~tmarshal/Solutions/saverange1.0.html
//..
//
///Usage
///-----
// The snippets of code in the first usage example below illustrate very basic
// use of the constructor proxy.  The second usage example provides a more
// extended illustration of a scenario that can occur in generic programming.
//
///Example 1
///- - - - -
// Suppose we have an arbitrary class:
//..
//  class SomeClass {
//      // ... class definition ...
//  };
//..
// 'SomeClass' may optionally declare the 'bdealg_TypeTraitUsesBdemaAllocator'
// trait.  The following code illustrates how a 'SomeClass' object can be
// constructed using a constructor proxy which detects this trait:
//..
//  using namespace BloombergLP;
//
//  bdema_TestAllocator               testAllocator;
//  bdeut_ConstructorProxy<SomeClass> proxy(&testAllocator);
//
//  SomeClass& myObject = proxy.object();
//..
// If 'SomeClass' declares the 'bdealg_TypeTraitUsesBdemaAllocator' trait, then
// the object of type 'SomeClass' held by 'proxy' will obtain its memory from
// the supplied 'testAllocator'.  Otherwise, 'testAllocator' will be ignored.
//
///Example 2
///- - - - -
// The following snippets of code illustrate a use of this component in a more
// typical scenario.
//
// The 'MyContainer' class below contains an object of a parameterized 'TYPE':
//..
//  template <typename TYPE>
//  class MyContainer {
//      // This class contains an object of parameterized 'TYPE'.
//
//      // PRIVATE DATA MEMBERS
//      TYPE d_object;  // contained object
//
//    public:
//      // CREATORS
//      explicit MyContainer(bdema_Allocator *basicAllocator = 0);
//          // Construct a container using the specified 'basicAllocator' to
//          // supply memory.  If 'basicAllocator' is 0, the currently
//          // installed default allocator is used.
//
//      ~MyContainer();
//          // Destroy this container.
//
//      // ACCESSORS
//      const TYPE& getObject() const;
//          // Return a reference to the non-modifiable object stored in this
//          // container.
//
//      // ... rest of class definition ...
//  };
//..
// The implementation for the 'MyContainer' constructor is a little tricky
// without a constructor proxy.  One possible implementation is as follows:
//..
//  template <typename TYPE>
//  MyContainer<TYPE>::MyContainer(bdema_Allocator *basicAllocator)
//  {
//  }
//..
// This implementation will compile successfully for each 'TYPE' that has a
// default constructor, but it will not behave as documented.  In particular,
// the specified 'basicAllocator' will not be used to supply memory.
//
// Another possible implementation for the 'MyContainer' constructor is as
// follows:
//..
//  template <typename TYPE>
//  MyContainer<TYPE>::MyContainer(bdema_Allocator *basicAllocator)
//  : d_object(basicAllocator)
//  {
//  }
//..
// This implementation behaves as documented, but it will not compile
// unless 'TYPE' has a constructor taking a 'bdema_Allocator *'.  For example,
// the following declaration of 'container' will fail to compile:
//..
//  bdema_TestAllocator testAllocator;
//
//  MyContainer<int> container(&testAllocator);
//..
// The solution to this problem is to use the constructor proxy provided by
// this component.  The following definition of 'MyContainer' uses a
// constructor proxy for the contained object of parameterized 'TYPE':
//..
//  template <typename TYPE>
//  class MyContainer {
//      // This class contains an object of parameterized 'TYPE'.
//
//      // PRIVATE DATA MEMBERS
//      bdeut_ConstructorProxy<TYPE> d_proxy;
//
//    public:
//      // CREATORS
//      explicit MyContainer(bdema_Allocator *basicAllocator = 0);
//          // Construct a container using the specified 'basicAllocator' to
//          // supply memory.  If 'basicAllocator' is 0, the currently
//          // installed default allocator is used.
//
//      ~MyContainer();
//          // Destroy this container.
//
//      // ACCESSORS
//      const TYPE& getObject() const;
//          // Return a reference to the non-modifiable object stored in this
//          // container.
//
//      // ... rest of class definition ...
//  };
//..
// The constructor for 'MyContainer' can now be implemented as follows:
//..
//  template <typename TYPE>
//  MyContainer<TYPE>::MyContainer(bdema_Allocator *basicAllocator)
//  : d_proxy(basicAllocator)
//  {
//  }
//..
//  The 'getObject' method of 'MyContainer' is implemented as follows:
//..
//  template <typename TYPE>
//  const TYPE& MyContainer<TYPE>::getObject() const
//  {
//      return d_proxy.object();
//  }
//..
// Now the following code, which previously did not compile, *will* compile
// successfully:
//..
//  bdema_TestAllocator testAllocator;
//
//  MyContainer<int> container(&testAllocator);
//..
// The specified 'testAllocator' will simply be ignored because 'int' does not
// use a 'bdema' allocator to supply memory.
//
// Next suppose we have a class defined as follows:
//..
//  class SomeClassUsingAllocator {
//      // This class uses a 'bdema' allocator.
//
//      // PRIVATE DATA MEMBERS
//      bdema_Allocator *d_allocator_p;
//
//    public:
//      // TRAITS
//      BDEALG_DECLARE_NESTED_TRAITS(SomeClassUsingAllocator,
//                                   bdealg_TypeTraitUsesBdemaAllocator);
//
//      // CREATORS
//      explicit SomeClassUsingAllocator(bdema_Allocator *basicAllocator = 0)
//      : d_allocator_p(bdema_Default::allocator(basicAllocator))
//      {
//      }
//
//      // ACCESSORS
//      bdema_Allocator *getAllocator() const
//      {
//          return d_allocator_p;
//      }
//  };
//..
// The following code will compile and run without an assertion failure:
//..
//  bdema_TestAllocator testAllocator;
//
//  MyContainer<SomeClassUsingAllocator> container(&testAllocator);
//
//  assert(&testAllocator == container.getObject().getAllocator());
//..
// Finally, since the 'MyContainer' class uses a 'bdema' allocator to supply
// memory, it is useful to expose this property.  This is done by declaring the
// 'bdealg_TypeTraitUsesBdemaAllocator' trait to complete the definition of
// 'MyContainer':
//..
//  template <typename TYPE>
//  class MyContainer {
//      // This class contains an object of parameterized 'TYPE' and declares
//      // the 'bdealg_TypeTraitUsesBdemaAllocator' trait.
//
//      // PRIVATE DATA MEMBERS
//      bdeut_ConstructorProxy<TYPE> d_proxy;
//
//    public:
//      // TRAITS
//      BDEALG_DECLARE_NESTED_TRAITS(MyContainer,
//                                   bdealg_TypeTraitUsesBdemaAllocator);
//
//      // CREATORS
//      explicit MyContainer(bdema_Allocator *basicAllocator = 0);
//          // Construct a container using the specified 'basicAllocator' to
//          // supply memory.  If 'basicAllocator' is 0, the currently
//          // installed default allocator is used.
//
//      ~MyContainer();
//          // Destroy this container.
//
//      // ACCESSORS
//      const TYPE& getObject() const;
//          // Return a reference to the non-modifiable object stored in this
//          // container.
//
//      // ... rest of class definition ...
//  };
//..
// The following code will also compile and run without an assertion failure:
//..
//  bdema_TestAllocator testAllocator;
//
//  MyContainer<MyContainer<SomeClassUsingAllocator> >
//                                          containedContainer(&testAllocator);
//
//  assert(&testAllocator
//               == containedContainer.getObject().getObject().getAllocator());
//..

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BDEALG_SCALARPRIMITIVES
#include <bdealg_scalarprimitives.h>
#endif

#ifndef INCLUDED_BDEALG_TYPETRAITS
#include <bdealg_typetraits.h>
#endif

#ifndef INCLUDED_BDES_OBJECTBUFFER
#include <bdes_objectbuffer.h>
#endif

namespace BloombergLP {

class bdema_Allocator;

                        // ============================
                        // class bdeut_ConstructorProxy
                        // ============================

template <typename TYPE>
class bdeut_ConstructorProxy {
    // This class acts as a proxy for constructing and destroying an object of
    // parameterized 'TYPE', where 'TYPE' may or may not use a 'bdema'
    // allocator for supplying memory.  The constructors for this proxy class
    // take a 'bdema_Allocator *'.  If 'TYPE' has the
    // 'bdealg_TypeTraitUsesBdemaAllocator' trait declared, then the supplied
    // allocator will be used to construct the proxied object.  Otherwise, the
    // allocator is ignored.

    // PRIVATE DATA MEMBERS
    bdes_ObjectBuffer<TYPE> d_objectBuffer;  // footprint of proxied object

  private:
    // NOT IMPLEMENTED
    bdeut_ConstructorProxy();
    bdeut_ConstructorProxy(const bdeut_ConstructorProxy&);
    bdeut_ConstructorProxy& operator=(const bdeut_ConstructorProxy&);

  public:
    // TRAITS
    BDEALG_DECLARE_NESTED_TRAITS(bdeut_ConstructorProxy,
                                 bdealg_TypeTraitUsesBdemaAllocator);
        // Declare that the constructors for this class accept a
        // 'bdema_Allocator *' for supplying memory.

    // CREATORS
    explicit bdeut_ConstructorProxy(bdema_Allocator *basicAllocator);
        // Construct a proxy, and a proxied object of parameterized 'TYPE'.
        // Use the specified 'basicAllocator' to supply memory to the proxied
        // object if 'TYPE' declares the 'bdealg_TypeTraitUsesBdemaAllocator'
        // trait, and ignore 'basicAllocator' otherwise.

    template <typename OTHER_TYPE>
    bdeut_ConstructorProxy(
                    const bdeut_ConstructorProxy<OTHER_TYPE>&  original,
                    bdema_Allocator                           *basicAllocator);
        // Construct a proxy, and a proxied object of parameterized 'TYPE'
        // having the value of the object held by the specified 'original'
        // proxy.  Use the specified 'basicAllocator' to supply memory to the
        // proxied object if 'TYPE' declares the
        // 'bdealg_TypeTraitUsesBdemaAllocator' trait, and ignore
        // 'basicAllocator' otherwise.  Note that a compilation error will
        // result unless a 'TYPE' object can be constructed from an
        // 'OTHER_TYPE' object.

    template <typename OTHER_TYPE>
    bdeut_ConstructorProxy(const OTHER_TYPE&  value,
                           bdema_Allocator   *basicAllocator);
        // Construct a proxy, and a proxied object of parameterized 'TYPE'
        // having the specified 'value'.  Use the specified 'basicAllocator' to
        // supply memory to the proxied object if 'TYPE' declares the
        // 'bdealg_TypeTraitUsesBdemaAllocator' trait, and ignore
        // 'basicAllocator' otherwise.  If 'basicAllocator' is 0, the currently
        // installed default allocator is used.  Note that a compilation error
        // will result unless a 'TYPE' object can be constructed from an
        // 'OTHER_TYPE' object.

    ~bdeut_ConstructorProxy();
        // Destroy this proxy and the object held by this proxy.

    // MANIPULATORS
    TYPE& object();
        // Return a reference to the modifiable object held by this proxy.

    // ACCESSORS
    const TYPE& object() const;
        // Return a reference to the non-modifiable object held by this proxy.
};

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

// CREATORS

template <typename TYPE>
inline
bdeut_ConstructorProxy<TYPE>::bdeut_ConstructorProxy(
                                               bdema_Allocator *basicAllocator)
{
    bdealg_ScalarPrimitives::defaultConstruct(&d_objectBuffer.object(),
                                              basicAllocator);
}

template <typename TYPE>
template <typename OTHER_TYPE>
inline
bdeut_ConstructorProxy<TYPE>::bdeut_ConstructorProxy(
                     const bdeut_ConstructorProxy<OTHER_TYPE>&  original,
                     bdema_Allocator                           *basicAllocator)
{
    bdealg_ScalarPrimitives::copyConstruct(&d_objectBuffer.object(),
                                           original.object(),
                                           basicAllocator);
}

template <typename TYPE>
template <typename OTHER_TYPE>
inline
bdeut_ConstructorProxy<TYPE>::bdeut_ConstructorProxy(
                                             const OTHER_TYPE&  value,
                                             bdema_Allocator   *basicAllocator)
{
    bdealg_ScalarPrimitives::copyConstruct(&d_objectBuffer.object(),
                                           value,
                                           basicAllocator);
}

template <typename TYPE>
inline
bdeut_ConstructorProxy<TYPE>::~bdeut_ConstructorProxy()
{
    bdealg_ScalarPrimitives::destruct(&d_objectBuffer.object(),
                                      (bdema_Allocator*)0);
}

// MANIPULATORS

template <typename TYPE>
inline
TYPE& bdeut_ConstructorProxy<TYPE>::object()
{
    return d_objectBuffer.object();
}

// ACCESSORS

template <typename TYPE>
inline
const TYPE& bdeut_ConstructorProxy<TYPE>::object() const
{
    return d_objectBuffer.object();
}

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2005
//      All Rights Reserved.
//      Property of Bloomberg L.P.  (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
