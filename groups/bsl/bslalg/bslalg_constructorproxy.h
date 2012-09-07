// bslalg_constructorproxy.h                                          -*-C++-*-
#ifndef INCLUDED_BSLALG_CONSTRUCTORPROXY
#define INCLUDED_BSLALG_CONSTRUCTORPROXY

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a proxy for constructing and destroying objects.
//
//@CLASSES:
//  bslalg::ConstructorProxy: proxy for constructing and destroying objects
//
//@SEE_ALSO: bslma_allocator, bslalg_typetraits
//
//@AUTHOR: Shezan Baig (sbaig)
//
//@CONTACT: Rohan Bhindwale (rbhindwa)
//
//@DESCRIPTION: This component provides a proxy for constructing and
// automatically destroying objects.  The proxy class
// 'bslalg::ConstructorProxy' is parameterized on a 'OBJECT_TYPE', where
// 'OBJECT_TYPE' may or may not use a 'bslma' allocator to supply memory.  Upon
// construction of a proxy, a proxied 'OBJECT_TYPE' instance is also
// constructed; the 'bslma' allocator supplied to the proxy constructor is
// passed to the constructor of the proxied object only if 'OBJECT_TYPE'
// declares the 'bslalg::TypeTraitUsesBslmaAllocator' trait.  If this trait is
// not declared for 'OBJECT_TYPE', the allocator is ignored.
//
// Following construction of the proxied object, it is held by the proxy.
// Modifiable and non-modifiable access to the proxied object may be obtained
// using the overloaded 'object' methods.  When the proxy is destroyed, the
// proxied object is automatically destroyed.
//
// This proxy is useful in situations where an object of a given type must be
// constructed, but it is not known whether the object's constructor takes a
// 'bslma' allocator to supply memory.  This occurs frequently in generic
// programming.
//
// See the 'bslma' package-level documentation for more information about using
// 'bslma' allocators.
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
// 'SomeClass' may optionally declare the 'bslalg::TypeTraitUsesBslmaAllocator'
// trait.  The following code illustrates how a 'SomeClass' object can be
// constructed using a constructor proxy which detects this trait:
//..
//  using namespace BloombergLP;
//
//  bslma::TestAllocator                testAllocator;
//  bslalg::ConstructorProxy<SomeClass> proxy(&testAllocator);
//
//  SomeClass& myObject = proxy.object();
//..
// If 'SomeClass' declares the 'bslalg::TypeTraitUsesBslmaAllocator' trait,
// then the object of type 'SomeClass' held by 'proxy' will obtain its memory
// from the supplied 'testAllocator'.  Otherwise, 'testAllocator' will be
// ignored.
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
//      explicit MyContainer(bslma::Allocator *basicAllocator = 0);
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
//  MyContainer<TYPE>::MyContainer(bslma::Allocator *basicAllocator)
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
//  MyContainer<TYPE>::MyContainer(bslma::Allocator *basicAllocator)
//  : d_object(basicAllocator)
//  {
//  }
//..
// This implementation behaves as documented, but it will not compile unless
// 'TYPE' has a constructor taking a 'bslma::Allocator *'.  For example, the
// following declaration of 'container' will fail to compile:
//..
//  bslma::TestAllocator testAllocator;
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
//      bslalg::ConstructorProxy<TYPE> d_proxy;
//
//    public:
//      // CREATORS
//      explicit MyContainer(bslma::Allocator *basicAllocator = 0);
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
//  MyContainer<TYPE>::MyContainer(bslma::Allocator *basicAllocator)
//  : d_proxy(basicAllocator)
//  {
//  }
//..
// The 'getObject' method of 'MyContainer' is implemented as follows:
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
//  bslma::TestAllocator testAllocator;
//
//  MyContainer<int> container(&testAllocator);
//..
// The specified 'testAllocator' will simply be ignored because 'int' does not
// use a 'bslma' allocator to supply memory.
//
// Next suppose we have a class defined as follows:
//..
//  class SomeClassUsingAllocator {
//      // This class uses a 'bslma' allocator.
//
//      // PRIVATE DATA MEMBERS
//      bslma::Allocator *d_allocator_p;
//
//    public:
//      // TRAITS
//      BSLALG_DECLARE_NESTED_TRAITS(SomeClassUsingAllocator,
//                                   bslalg::TypeTraitUsesBslmaAllocator);
//
//      // CREATORS
//      explicit SomeClassUsingAllocator(bslma::Allocator *basicAllocator = 0)
//      : d_allocator_p(bslma::Default::allocator(basicAllocator))
//      {
//      }
//
//      // ACCESSORS
//      bslma::Allocator *getAllocator() const
//      {
//          return d_allocator_p;
//      }
//  };
//..
// The following code will compile and run without an assertion failure:
//..
//  bslma::TestAllocator testAllocator;
//
//  MyContainer<SomeClassUsingAllocator> container(&testAllocator);
//
//  assert(&testAllocator == container.getObject().getAllocator());
//..
// Finally, since the 'MyContainer' class uses a 'bslma' allocator to supply
// memory, it is useful to expose this property.  This is done by declaring the
// 'bslalg::TypeTraitUsesBslmaAllocator' trait to complete the definition of
// 'MyContainer':
//..
//  template <typename TYPE>
//  class MyContainer {
//      // This class contains an object of parameterized 'TYPE' and declares
//      // the 'bslalg::TypeTraitUsesBslmaAllocator' trait.
//
//      // PRIVATE DATA MEMBERS
//      bslalg::ConstructorProxy<TYPE> d_proxy;
//
//    public:
//      // TRAITS
//      BSLALG_DECLARE_NESTED_TRAITS(MyContainer,
//                                   bslalg::TypeTraitUsesBslmaAllocator);
//
//      // CREATORS
//      explicit MyContainer(bslma::Allocator *basicAllocator = 0);
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
//  bslma::TestAllocator testAllocator;
//
//  MyContainer<MyContainer<SomeClassUsingAllocator> >
//                                          containedContainer(&testAllocator);
//
//  assert(&testAllocator
//               == containedContainer.getObject().getObject().getAllocator());
//..

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLALG_SCALARDESTRUCTIONPRIMITIVES
#include <bslalg_scalardestructionprimitives.h>
#endif

#ifndef INCLUDED_BSLALG_SCALARPRIMITIVES
#include <bslalg_scalarprimitives.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITS
#include <bslalg_typetraits.h>
#endif

#ifndef INCLUDED_BSLS_OBJECTBUFFER
#include <bsls_objectbuffer.h>
#endif

#ifndef INCLUDED_BSLS_UTIL
#include <bsls_util.h>
#endif

namespace BloombergLP {

namespace bslma { class Allocator; }

namespace bslalg {

                        // ======================
                        // class ConstructorProxy
                        // ======================

template <typename OBJECT_TYPE>
class ConstructorProxy {
    // This class acts as a proxy for constructing and destroying an object of
    // parameterized 'OBJECT_TYPE', where 'OBJECT_TYPE' may or may not use a
    // 'bslma' allocator for supplying memory.  The constructors for this proxy
    // class take a 'bslma::Allocator *'.  If 'OBJECT_TYPE' has the
    // 'TypeTraitUsesBslmaAllocator' trait declared, then the supplied
    // allocator will be used to construct the proxied object.  Otherwise, the
    // allocator is ignored.

    // DATA
    bsls::ObjectBuffer<OBJECT_TYPE> d_objectBuffer;  // footprint of proxied
                                                     // object (raw buffer)

  private:
    // NOT IMPLEMENTED
    ConstructorProxy(const ConstructorProxy&);
    ConstructorProxy& operator=(const ConstructorProxy&);

  public:
    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(ConstructorProxy,
                                 TypeTraitUsesBslmaAllocator);
        // Declare that the constructors for this class accept a
        // 'bslma::Allocator *' argument for supplying memory.

    // CREATORS
    explicit ConstructorProxy(bslma::Allocator *basicAllocator);
        // Construct a proxy, and a proxied object of parameterized
        // 'OBJECT_TYPE'.  Use the specified 'basicAllocator' to supply memory
        // to the proxied object if 'OBJECT_TYPE' declares the
        // 'TypeTraitUsesBslmaAllocator' trait, and ignore 'basicAllocator'
        // otherwise.

    ConstructorProxy(const ConstructorProxy<OBJECT_TYPE>&  original,
                     bslma::Allocator                     *basicAllocator);
        // Construct a proxy, and a proxied object of parameterized
        // 'OBJECT_TYPE' having the value of the object held by the specified
        // 'original' proxy.  Use the specified 'basicAllocator' to supply
        // memory to the proxied object if 'OBJECT_TYPE' declares the
        // 'TypeTraitUsesBslmaAllocator' trait, and ignore 'basicAllocator'
        // otherwise.

    template <typename SOURCE_TYPE>
    ConstructorProxy(const ConstructorProxy<SOURCE_TYPE>&  original,
                     bslma::Allocator                     *basicAllocator);
        // Construct a proxy, and a proxied object of the parameterized
        // 'OBJECT_TYPE' having the value of the object of the parameterized
        // 'SOURCE_TYPE' held by the specified 'original' proxy.  Use the
        // specified 'basicAllocator' to supply memory to the proxied object if
        // 'OBJECT_TYPE' declares the 'TypeTraitUsesBslmaAllocator' trait, and
        // ignore 'basicAllocator' otherwise.  Note that a compilation error
        // will result unless an instance of 'OBJECT_TYPE' can be constructed
        // from an instance of 'SOURCE_TYPE'.

    template <typename ARG1>
    ConstructorProxy(const ARG1& a1, bslma::Allocator *basicAllocator);
    template <typename ARG1, typename ARG2>
    ConstructorProxy(const ARG1& a1, const ARG2& a2,
                     bslma::Allocator *basicAllocator);
    template <typename ARG1, typename ARG2, typename ARG3>
    ConstructorProxy(const ARG1& a1, const ARG2& a2, const ARG3& a3,
                     bslma::Allocator *basicAllocator);
    template <typename ARG1, typename ARG2, typename ARG3, typename ARG4>
    ConstructorProxy(const ARG1& a1, const ARG2& a2, const ARG3& a3,
                     const ARG4& a4, bslma::Allocator *basicAllocator);
    template <typename ARG1, typename ARG2, typename ARG3, typename ARG4,
              typename ARG5>
    ConstructorProxy(const ARG1& a1, const ARG2& a2, const ARG3& a3,
                     const ARG4& a4, const ARG5& a5,
                     bslma::Allocator *basicAllocator);
    template <typename ARG1, typename ARG2, typename ARG3, typename ARG4,
              typename ARG5, typename ARG6>
    ConstructorProxy(const ARG1& a1, const ARG2& a2, const ARG3& a3,
                     const ARG4& a4, const ARG5& a5, const ARG6& a6,
                     bslma::Allocator *basicAllocator);
    template <typename ARG1, typename ARG2, typename ARG3, typename ARG4,
              typename ARG5, typename ARG6, typename ARG7>
    ConstructorProxy(const ARG1& a1, const ARG2& a2, const ARG3& a3,
                     const ARG4& a4, const ARG5& a5, const ARG6& a6,
                     const ARG7& a7, bslma::Allocator *basicAllocator);
    template <typename ARG1, typename ARG2, typename ARG3, typename ARG4,
              typename ARG5, typename ARG6, typename ARG7, typename ARG8>
    ConstructorProxy(const ARG1& a1, const ARG2& a2, const ARG3& a3,
                     const ARG4& a4, const ARG5& a5, const ARG6& a6,
                     const ARG7& a7, const ARG8& a8,
                     bslma::Allocator *basicAllocator);
    template <typename ARG1, typename ARG2, typename ARG3, typename ARG4,
              typename ARG5, typename ARG6, typename ARG7, typename ARG8,
              typename ARG9>
    ConstructorProxy(const ARG1& a1, const ARG2& a2, const ARG3& a3,
                     const ARG4& a4, const ARG5& a5, const ARG6& a6,
                     const ARG7& a7, const ARG8& a8, const ARG9& a9,
                     bslma::Allocator *basicAllocator);
    template <typename ARG1, typename ARG2, typename ARG3, typename ARG4,
              typename ARG5, typename ARG6, typename ARG7, typename ARG8,
              typename ARG9, typename ARG10>
    ConstructorProxy(const ARG1&  a1, const ARG2& a2, const ARG3& a3,
                     const ARG4&  a4, const ARG5& a5, const ARG6& a6,
                     const ARG7&  a7, const ARG8& a8, const ARG9& a9,
                     const ARG10& a10, bslma::Allocator *basicAllocator);
    template <typename ARG1, typename ARG2, typename ARG3, typename ARG4,
              typename ARG5, typename ARG6, typename ARG7, typename ARG8,
              typename ARG9, typename ARG10, typename ARG11>
    ConstructorProxy(const ARG1&  a1, const ARG2& a2,  const ARG3& a3,
                     const ARG4&  a4, const ARG5& a5,  const ARG6& a6,
                     const ARG7&  a7, const ARG8& a8,  const ARG9& a9,
                     const ARG10& a10, const ARG11& a11,
                     bslma::Allocator *basicAllocator);
    template <typename ARG1, typename ARG2, typename ARG3, typename ARG4,
              typename ARG5, typename ARG6, typename ARG7, typename ARG8,
              typename ARG9, typename ARG10, typename ARG11, typename ARG12>
    ConstructorProxy(const ARG1&  a1,  const ARG2&  a2,  const ARG3& a3,
                     const ARG4&  a4,  const ARG5&  a5,  const ARG6& a6,
                     const ARG7&  a7,  const ARG8&  a8,  const ARG9& a9,
                     const ARG10& a10, const ARG11& a11,
                     const ARG12& a12, bslma::Allocator *basicAllocator);
    template <typename ARG1, typename ARG2,  typename ARG3,  typename ARG4,
              typename ARG5, typename ARG6,  typename ARG7,  typename ARG8,
              typename ARG9, typename ARG10, typename ARG11, typename ARG12,
              typename ARG13>
    ConstructorProxy(const ARG1&  a1,  const ARG2&  a2, const ARG3&  a3,
                     const ARG4&  a4,  const ARG5&  a5, const ARG6&  a6,
                     const ARG7&  a7,  const ARG8&  a8, const ARG9&  a9,
                     const ARG10& a10, const ARG11& a11,
                     const ARG12& a12, const ARG13& a13,
                     bslma::Allocator *basicAllocator);
    template <typename ARG1, typename ARG2,  typename ARG3,  typename ARG4,
              typename ARG5, typename ARG6,  typename ARG7,  typename ARG8,
              typename ARG9, typename ARG10, typename ARG11, typename ARG12,
              typename ARG13, typename ARG14>
    ConstructorProxy(const ARG1&  a1,  const ARG2&  a2, const ARG3&  a3,
                     const ARG4&  a4,  const ARG5&  a5, const ARG6&  a6,
                     const ARG7&  a7,  const ARG8&  a8, const ARG9&  a9,
                     const ARG10& a10, const ARG11& a11,
                     const ARG12& a12, const ARG13& a13,
                     const ARG14& a14,
                     bslma::Allocator *basicAllocator);
        // Construct a proxy, and a proxied object of the parameterized
        // 'OBJECT_TYPE' using the specified arguments 'a1' up to 'a14' of the
        // respective parameterized 'ARG1' up to 'ARG14' types.  Use the
        // specified 'basicAllocator' to supply memory to the proxied object if
        // 'OBJECT_TYPE' declares the 'TypeTraitUsesBslmaAllocator' trait, and
        // ignore 'basicAllocator' otherwise.  If 'basicAllocator' is 0, the
        // currently installed default allocator is used.  Note that a
        // compilation error will result unless 'OBJECT_TYPE' has a constructor
        // of signature compatible with
        // 'OBJECT_TYPE(ARG1 const&, ARG2 const&, ...)'.

    ~ConstructorProxy();
        // Destroy this proxy and the object held by this proxy.

    // MANIPULATORS
    OBJECT_TYPE& object();
        // Return a reference to the modifiable object held by this proxy.

    // ACCESSORS
    const OBJECT_TYPE& object() const;
        // Return a reference to the non-modifiable object held by this proxy.
};

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

                        // ----------------------
                        // class ConstructorProxy
                        // ----------------------

// CREATORS
template <typename OBJECT_TYPE>
inline
ConstructorProxy<OBJECT_TYPE>::ConstructorProxy(
                                              bslma::Allocator *basicAllocator)
{
    ScalarPrimitives::defaultConstruct(
                                  BSLS_UTIL_ADDRESSOF(d_objectBuffer.object()),
                                  basicAllocator);
}

template <typename OBJECT_TYPE>
inline
ConstructorProxy<OBJECT_TYPE>::ConstructorProxy(
                          const ConstructorProxy<OBJECT_TYPE>&  original,
                          bslma::Allocator                     *basicAllocator)
{
    ScalarPrimitives::copyConstruct(
                                  BSLS_UTIL_ADDRESSOF(d_objectBuffer.object()),
                                  original.object(),
                                  basicAllocator);
}

template <typename OBJECT_TYPE>
template <typename SOURCE_TYPE>
inline
ConstructorProxy<OBJECT_TYPE>::ConstructorProxy(
                          const ConstructorProxy<SOURCE_TYPE>&  original,
                          bslma::Allocator                     *basicAllocator)
{
    ScalarPrimitives::construct(BSLS_UTIL_ADDRESSOF(d_objectBuffer.object()),
                                original.object(),
                                basicAllocator);
}

template <typename OBJECT_TYPE>
template <typename ARG1>
inline
ConstructorProxy<OBJECT_TYPE>::ConstructorProxy(
                              const ARG1& a1, bslma::Allocator *basicAllocator)
{
    ScalarPrimitives::construct(BSLS_UTIL_ADDRESSOF(d_objectBuffer.object()),
                                a1,
                                basicAllocator);
}

template <typename OBJECT_TYPE>
template <typename ARG1, typename ARG2>
inline
ConstructorProxy<OBJECT_TYPE>::ConstructorProxy(
              const ARG1& a1, const ARG2& a2, bslma::Allocator *basicAllocator)
{
    ScalarPrimitives::construct(BSLS_UTIL_ADDRESSOF(d_objectBuffer.object()),
                                a1, a2,
                                basicAllocator);
}

template <typename OBJECT_TYPE>
template <typename ARG1, typename ARG2, typename ARG3>
inline
ConstructorProxy<OBJECT_TYPE>::ConstructorProxy(
                                const ARG1& a1, const ARG2& a2, const ARG3& a3,
                                bslma::Allocator *basicAllocator)
{
    ScalarPrimitives::construct(BSLS_UTIL_ADDRESSOF(d_objectBuffer.object()),
                                a1, a2, a3,
                                basicAllocator);
}

template <typename OBJECT_TYPE>
template <typename ARG1, typename ARG2, typename ARG3, typename ARG4>
inline
ConstructorProxy<OBJECT_TYPE>::ConstructorProxy(
                const ARG1& a1, const ARG2& a2, const ARG3& a3, const ARG4& a4,
                bslma::Allocator *basicAllocator)
{
    ScalarPrimitives::construct(BSLS_UTIL_ADDRESSOF(d_objectBuffer.object()),
                                a1, a2, a3, a4,
                                basicAllocator);
}

template <typename OBJECT_TYPE>
template <typename ARG1, typename ARG2, typename ARG3, typename ARG4,
          typename ARG5>
inline
ConstructorProxy<OBJECT_TYPE>::ConstructorProxy(
                const ARG1& a1, const ARG2& a2, const ARG3& a3, const ARG4& a4,
                const ARG5& a5, bslma::Allocator *basicAllocator)
{
    ScalarPrimitives::construct(BSLS_UTIL_ADDRESSOF(d_objectBuffer.object()),
                                a1, a2, a3, a4, a5,
                                basicAllocator);
}

template <typename OBJECT_TYPE>
template <typename ARG1, typename ARG2, typename ARG3, typename ARG4,
          typename ARG5, typename ARG6>
inline
ConstructorProxy<OBJECT_TYPE>::ConstructorProxy(
              const ARG1& a1, const ARG2& a2, const ARG3& a3, const ARG4& a4,
              const ARG5& a5, const ARG6& a6, bslma::Allocator *basicAllocator)
{
    ScalarPrimitives::construct(BSLS_UTIL_ADDRESSOF(d_objectBuffer.object()),
                                a1, a2, a3, a4, a5, a6,
                                basicAllocator);
}

template <typename OBJECT_TYPE>
template <typename ARG1, typename ARG2, typename ARG3, typename ARG4,
          typename ARG5, typename ARG6, typename ARG7>
inline
ConstructorProxy<OBJECT_TYPE>::ConstructorProxy(
                const ARG1& a1, const ARG2& a2, const ARG3& a3, const ARG4& a4,
                const ARG5& a5, const ARG6& a6, const ARG7& a7,
                bslma::Allocator *basicAllocator)
{
    ScalarPrimitives::construct(BSLS_UTIL_ADDRESSOF(d_objectBuffer.object()),
                                a1, a2, a3, a4, a5, a6, a7,
                                basicAllocator);
}

template <typename OBJECT_TYPE>
template <typename ARG1, typename ARG2, typename ARG3, typename ARG4,
          typename ARG5, typename ARG6, typename ARG7, typename ARG8>
inline
ConstructorProxy<OBJECT_TYPE>::ConstructorProxy(
                const ARG1& a1, const ARG2& a2, const ARG3& a3, const ARG4& a4,
                const ARG5& a5, const ARG6& a6, const ARG7& a7, const ARG8& a8,
                bslma::Allocator *basicAllocator)
{
    ScalarPrimitives::construct(BSLS_UTIL_ADDRESSOF(d_objectBuffer.object()),
                                a1, a2, a3, a4, a5, a6, a7, a8,
                                basicAllocator);
}

template <typename OBJECT_TYPE>
template <typename ARG1, typename ARG2, typename ARG3, typename ARG4,
          typename ARG5, typename ARG6, typename ARG7, typename ARG8,
          typename ARG9>
inline
ConstructorProxy<OBJECT_TYPE>::ConstructorProxy(
                const ARG1& a1, const ARG2& a2, const ARG3& a3, const ARG4& a4,
                const ARG5& a5, const ARG6& a6, const ARG7& a7, const ARG8& a8,
                const ARG9& a9, bslma::Allocator *basicAllocator)
{
    ScalarPrimitives::construct(BSLS_UTIL_ADDRESSOF(d_objectBuffer.object()),
                                a1, a2, a3, a4, a5, a6, a7, a8, a9,
                                basicAllocator);
}

template <typename OBJECT_TYPE>
template <typename ARG1, typename ARG2, typename ARG3, typename ARG4,
          typename ARG5, typename ARG6, typename ARG7, typename ARG8,
          typename ARG9, typename ARG10>
inline
ConstructorProxy<OBJECT_TYPE>::ConstructorProxy(
            const ARG1& a1, const ARG2& a2, const ARG3& a3, const ARG4& a4,
            const ARG5& a5, const ARG6& a6, const ARG7& a7, const ARG8& a8,
            const ARG9& a9, const ARG10& a10, bslma::Allocator *basicAllocator)
{
    ScalarPrimitives::construct(BSLS_UTIL_ADDRESSOF(d_objectBuffer.object()),
                                a1, a2, a3, a4, a5, a6, a7, a8, a9, a10,
                                basicAllocator);
}

template <typename OBJECT_TYPE>
template <typename ARG1, typename ARG2,  typename ARG3, typename ARG4,
          typename ARG5, typename ARG6,  typename ARG7, typename ARG8,
          typename ARG9, typename ARG10, typename ARG11>
inline
ConstructorProxy<OBJECT_TYPE>::ConstructorProxy(
                const ARG1& a1, const ARG2& a2, const ARG3& a3, const ARG4& a4,
                const ARG5& a5, const ARG6& a6, const ARG7& a7, const ARG8& a8,
                const ARG9& a9, const ARG10& a10, const ARG11& a11,
                bslma::Allocator *basicAllocator)
{
    ScalarPrimitives::construct(BSLS_UTIL_ADDRESSOF(d_objectBuffer.object()),
                                a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11,
                                basicAllocator);
}

template <typename OBJECT_TYPE>
template <typename ARG1, typename ARG2,  typename ARG3, typename ARG4,
          typename ARG5, typename ARG6,  typename ARG7, typename ARG8,
          typename ARG9, typename ARG10, typename ARG11, typename ARG12>
inline
ConstructorProxy<OBJECT_TYPE>::ConstructorProxy(
          const ARG1& a1, const ARG2&  a2,  const ARG3&  a3,  const ARG4&  a4,
          const ARG5& a5, const ARG6&  a6,  const ARG7&  a7,  const ARG8&  a8,
          const ARG9& a9, const ARG10& a10, const ARG11& a11, const ARG12& a12,
          bslma::Allocator *basicAllocator)
{
    ScalarPrimitives::construct(BSLS_UTIL_ADDRESSOF(d_objectBuffer.object()),
                                a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11,
                                a12, basicAllocator);
}

template <typename OBJECT_TYPE>
template <typename ARG1, typename  ARG2,  typename ARG3, typename  ARG4,
          typename ARG5, typename  ARG6,  typename ARG7, typename  ARG8,
          typename ARG9, typename  ARG10, typename ARG11, typename ARG12,
          typename ARG13>
inline
ConstructorProxy<OBJECT_TYPE>::ConstructorProxy(
          const ARG1& a1, const ARG2& a2, const ARG3& a3, const ARG4& a4,
          const ARG5& a5, const ARG6& a6, const ARG7& a7, const ARG8& a8,
          const ARG9& a9, const ARG10& a10, const ARG11& a11, const ARG12& a12,
          const ARG13& a13, bslma::Allocator *basicAllocator)
{
    ScalarPrimitives::construct(BSLS_UTIL_ADDRESSOF(d_objectBuffer.object()),
                                a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11,
                                a12, a13, basicAllocator);
}

template <typename OBJECT_TYPE>
template <typename ARG1, typename ARG2, typename ARG3, typename ARG4,
          typename ARG5, typename ARG6, typename ARG7, typename ARG8,
          typename ARG9, typename ARG10, typename ARG11, typename ARG12,
          typename ARG13, typename ARG14>
inline
ConstructorProxy<OBJECT_TYPE>::ConstructorProxy(
          const ARG1& a1, const ARG2& a2, const ARG3& a3, const ARG4& a4,
          const ARG5& a5, const ARG6& a6, const ARG7& a7, const ARG8& a8,
          const ARG9& a9, const ARG10& a10, const ARG11& a11, const ARG12& a12,
          const ARG13& a13, const ARG14& a14, bslma::Allocator *basicAllocator)
{
    ScalarPrimitives::construct(BSLS_UTIL_ADDRESSOF(d_objectBuffer.object()),
                                a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11,
                                a12, a13, a14, basicAllocator);
}

template <typename OBJECT_TYPE>
inline
ConstructorProxy<OBJECT_TYPE>::~ConstructorProxy()
{
    ScalarDestructionPrimitives::destroy(
                                 BSLS_UTIL_ADDRESSOF(d_objectBuffer.object()));
}

// MANIPULATORS
template <typename OBJECT_TYPE>
inline
OBJECT_TYPE& ConstructorProxy<OBJECT_TYPE>::object()
{
    return d_objectBuffer.object();
}

// ACCESSORS
template <typename OBJECT_TYPE>
inline
const OBJECT_TYPE& ConstructorProxy<OBJECT_TYPE>::object() const
{
    return d_objectBuffer.object();
}

}  // close package namespace

#if defined(BDE_BACKWARD_COMPATIBILITY) && 1 == BDE_BACKWARD_COMPATIBILITY
// ===========================================================================
//                           BACKWARD COMPATIBILITY
// ===========================================================================

#ifdef bslalg_ConstructorProxy
#undef bslalg_ConstructorProxy
#endif
#define bslalg_ConstructorProxy bslalg::ConstructorProxy
    // This alias is defined for backward compatibility.
#endif // BDE_BACKWARD_COMPATIBILITY

}  // close enterprise namespace

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2008
//      All Rights Reserved.
//      Property of Bloomberg L.P.  (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
