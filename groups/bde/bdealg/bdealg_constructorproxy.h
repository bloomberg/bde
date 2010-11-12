// bdealg_constructorproxy.h                -*-C++-*-
#ifndef INCLUDED_BDEALG_CONSTRUCTORPROXY
#define INCLUDED_BDEALG_CONSTRUCTORPROXY

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a proxy for constructing and destroying objects.
//
//@DEPRECATED: Use 'bslalg_constructorproxy' instead.
//
//@CLASSES:
//    bdealg_ConstructorProxy: proxy for constructing and destroying objects
//
//@AUTHOR: Shezan Baig (sbaig)
//
//@CONTACT: Rohan Bhindwale (rbhindwa)
//
//@SEE_ALSO: bdema_allocator, bdealg_typetraits
//
//@DESCRIPTION: This component provides a proxy for constructing and
// automatically destroying objects.  The proxy class 'bdealg_ConstructorProxy'
// is parameterized on a 'OBJECT_TYPE', where 'OBJECT_TYPE' may or may not use
// a 'bdema' allocator to supply memory.  Upon construction of a proxy, a
// proxied 'OBJECT_TYPE' instance is also constructed; the 'bdema' allocator
// supplied to the proxy constructor is passed to the constructor of the
// proxied object only if 'OBJECT_TYPE' declares the
// 'bdealg_TypeTraitUsesBdemaAllocator' trait.  If this trait is not declared
// for 'OBJECT_TYPE', the allocator is ignored.
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
//  bdema_TestAllocator                testAllocator;
//  bdealg_ConstructorProxy<SomeClass> proxy(&testAllocator);
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
//      bdealg_ConstructorProxy<TYPE> d_proxy;
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
//      bdealg_ConstructorProxy<TYPE> d_proxy;
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

#ifndef INCLUDED_BSLALG_CONSTRUCTORPROXY
#include <bslalg_constructorproxy.h>
#endif

namespace BloombergLP {

                        // =============================
                        // class bdealg_ConstructorProxy
                        // =============================

#define bdealg_ConstructorProxy bslalg_ConstructorProxy
    // This class acts as a proxy for constructing and destroying an object of
    // parameterized 'OBJECT_TYPE', where 'OBJECT_TYPE' may or may not use a
    // 'bdema' allocator for supplying memory.  The constructors for this proxy
    // class take a 'bdema_Allocator *'.  If 'OBJECT_TYPE' has the
    // 'bdealg_TypeTraitUsesBdemaAllocator' trait declared, then the supplied
    // allocator will be used to construct the proxied object.  Otherwise, the
    // allocator is ignored.

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2008
//      All Rights Reserved.
//      Property of Bloomberg L.P.  (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
