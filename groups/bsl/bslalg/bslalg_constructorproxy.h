// bslalg_constructorproxy.h                                          -*-C++-*-
#ifndef INCLUDED_BSLALG_CONSTRUCTORPROXY
#define INCLUDED_BSLALG_CONSTRUCTORPROXY

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a proxy for constructing and destroying objects.
//
//@CLASSES:
//  bslalg::ConstructorProxy: proxy for constructing and destroying objects
//
//@SEE_ALSO: bslma_allocator
//
//@DESCRIPTION: This component provides a proxy for constructing and
// automatically destroying objects.  The proxy class
// 'bslalg::ConstructorProxy' is parameterized on a 'OBJECT_TYPE', where
// 'OBJECT_TYPE' may or may not use a 'bslma' allocator to supply memory.  Upon
// construction of a proxy, a proxied 'OBJECT_TYPE' instance is also
// constructed; the 'bslma' allocator supplied to the proxy constructor is
// passed to the constructor of the proxied object only if 'OBJECT_TYPE'
// declares the 'bslma::UsesBslmaAllocator' trait.  If this trait is
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
// 'SomeClass' may optionally declare the 'bslma::UsesBslmaAllocator'
// trait.  The following code illustrates how a 'SomeClass' object can be
// constructed using a constructor proxy that detects this trait:
//..
//  using namespace BloombergLP;
//
//  bslma::TestAllocator                testAllocator;
//  bslalg::ConstructorProxy<SomeClass> proxy(&testAllocator);
//
//  SomeClass& myObject = proxy.object();
//..
// If 'SomeClass' declares the 'bslma::UsesBslmaAllocator' trait,
// then the object of type 'SomeClass' held by 'proxy' will obtain its memory
// from the supplied 'testAllocator'.  Otherwise, 'testAllocator' will be
// ignored.
//
///Example 2
///- - - - -
// The following snippets of code illustrate a use of this component in a more
// typical scenario.
//
// The 'MyContainer' class below contains an object of the specified parameter
// 'TYPE':
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
//
//  // TRAITS
//  namespace bslma {
//
//  template <>
//  struct UsesBslmaAllocator<SomeClassUsingAllocator> : bsl::true_type
//  {};
//
//  }
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
// 'bslma::UsesBslmaAllocator' trait to complete the definition of
// 'MyContainer':
//..
//  template <typename TYPE>
//  class MyContainer {
//      // This class contains an object of parameterized 'TYPE' and declares
//      // the 'bslma::UsesBslmaAllocator' trait.
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
//
//  // TRAITS
//  namespace bslma {
//
//  template <typename TYPE>
//  struct UsesBslmaAllocator<MyContainer<TYPE> > : bsl::true_type
//  {};
//
//  }
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

#include <bslscm_version.h>

#include <bslma_constructionutil.h>
#include <bslma_destructionutil.h>
#include <bslma_usesbslmaallocator.h>

#include <bslmf_enableif.h>
#include <bslmf_decay.h>
#include <bslmf_integralconstant.h>
#include <bslmf_movableref.h>
#include <bslmf_util.h>    // 'forward(V)'

#include <bsls_compilerfeatures.h>
#include <bsls_keyword.h>
#include <bsls_objectbuffer.h>
#include <bsls_util.h>     // 'forward<T>(V)'

#ifndef BDE_DONT_ALLOW_TRANSITIVE_INCLUDES
#include <bslalg_scalarprimitives.h>
#endif // BDE_DONT_ALLOW_TRANSITIVE_INCLUDES

namespace BloombergLP {

namespace bslma { class Allocator; }

namespace bslalg {

template <class> class ConstructorProxy;

                        // ===============================
                        // struct ConstructorProxy_IsProxy
                        // ===============================

template <class TYPE>
struct ConstructorProxy_IsProxy : bsl::false_type {
    // Provides a metafunction to determine if the specified 'TYPE' is
    // 'ConstructorProxy'. This non-specialized class template always returns
    // 'false'.
};

template <class TYPE>
struct ConstructorProxy_IsProxy<ConstructorProxy<TYPE> > : bsl::true_type {
    // Provides a metafunction to determine if the specified 'TYPE' is
    // 'ConstructorProxy'. This specialized class template always returns
    // 'true'.
};

                        // ======================
                        // class ConstructorProxy
                        // ======================

template <class OBJECT_TYPE>
class ConstructorProxy {
    // This class acts as a proxy for constructing and destroying an object of
    // parameterized 'OBJECT_TYPE', where 'OBJECT_TYPE' may or may not use a
    // 'bslma' allocator for supplying memory.  The constructors for this proxy
    // class take a 'bslma::Allocator *'.  If 'OBJECT_TYPE' has the
    // 'bslma::UsesBslmaAllocator' trait declared, then the supplied
    // allocator will be used to construct the proxied object.  Otherwise, the
    // allocator is ignored.

    // DATA
    bsls::ObjectBuffer<OBJECT_TYPE> d_objectBuffer;  // footprint of proxied
                                                     // object (raw buffer)

  private:
    // NOT IMPLEMENTED
    ConstructorProxy(const ConstructorProxy&)            BSLS_KEYWORD_DELETED;
    ConstructorProxy& operator=(const ConstructorProxy&) BSLS_KEYWORD_DELETED;

  public:
    // CREATORS
    explicit ConstructorProxy(bslma::Allocator *basicAllocator);
        // Construct a proxy, and a proxied object of parameterized
        // 'OBJECT_TYPE'.  Use the specified 'basicAllocator' to supply memory
        // to the proxied object if 'OBJECT_TYPE' declares the
        // 'bslma::UsesBslmaAllocator' trait, and ignore 'basicAllocator'
        // otherwise.

    template <class SOURCE_TYPE>
    ConstructorProxy(
            const ConstructorProxy<SOURCE_TYPE>&               original,
            bslma::Allocator                                  *basicAllocator);
    template <class SOURCE_TYPE>
    ConstructorProxy(
            bslmf::MovableRef<ConstructorProxy<SOURCE_TYPE> >  original,
            bslma::Allocator                                  *basicAllocator);
        // Construct a proxy, and a proxied object of the parameterized
        // 'OBJECT_TYPE' having the value of the object of the parameterized
        // 'SOURCE_TYPE' held by the specified 'original' proxy.  Use the
        // specified 'basicAllocator' to supply memory to the proxied object if
        // 'OBJECT_TYPE' declares the 'bslma::UsesBslmaAllocator' trait, and
        // ignore 'basicAllocator' otherwise.  Note that a compilation error
        // will result unless an instance of 'OBJECT_TYPE' can be constructed
        // from an instance of 'SOURCE_TYPE'.

    template <class ARG01>
    ConstructorProxy(BSLS_COMPILERFEATURES_FORWARD_REF(ARG01)  a01,
                     typename bsl::enable_if<
                         !ConstructorProxy_IsProxy<
                             typename bsl::decay<ARG01>::type>::value,
                             bslma::Allocator>::type          *basicAllocator);
    template <class ARG01, class ARG02>
    ConstructorProxy(BSLS_COMPILERFEATURES_FORWARD_REF(ARG01)  a01,
                     BSLS_COMPILERFEATURES_FORWARD_REF(ARG02)  a02,
                     bslma::Allocator                         *basicAllocator);
    template <class ARG01, class ARG02, class ARG03>
    ConstructorProxy(BSLS_COMPILERFEATURES_FORWARD_REF(ARG01)  a01,
                     BSLS_COMPILERFEATURES_FORWARD_REF(ARG02)  a02,
                     BSLS_COMPILERFEATURES_FORWARD_REF(ARG03)  a03,
                     bslma::Allocator                         *basicAllocator);
    template <class ARG01, class ARG02, class ARG03, class ARG04>
    ConstructorProxy(BSLS_COMPILERFEATURES_FORWARD_REF(ARG01)  a01,
                     BSLS_COMPILERFEATURES_FORWARD_REF(ARG02)  a02,
                     BSLS_COMPILERFEATURES_FORWARD_REF(ARG03)  a03,
                     BSLS_COMPILERFEATURES_FORWARD_REF(ARG04)  a04,
                     bslma::Allocator                         *basicAllocator);
    template <class ARG01, class ARG02, class ARG03, class ARG04,
              class ARG05>
    ConstructorProxy(BSLS_COMPILERFEATURES_FORWARD_REF(ARG01)  a01,
                     BSLS_COMPILERFEATURES_FORWARD_REF(ARG02)  a02,
                     BSLS_COMPILERFEATURES_FORWARD_REF(ARG03)  a03,
                     BSLS_COMPILERFEATURES_FORWARD_REF(ARG04)  a04,
                     BSLS_COMPILERFEATURES_FORWARD_REF(ARG05)  a05,
                     bslma::Allocator                         *basicAllocator);
    template <class ARG01, class ARG02, class ARG03, class ARG04,
              class ARG05, class ARG06>
    ConstructorProxy(BSLS_COMPILERFEATURES_FORWARD_REF(ARG01)  a01,
                     BSLS_COMPILERFEATURES_FORWARD_REF(ARG02)  a02,
                     BSLS_COMPILERFEATURES_FORWARD_REF(ARG03)  a03,
                     BSLS_COMPILERFEATURES_FORWARD_REF(ARG04)  a04,
                     BSLS_COMPILERFEATURES_FORWARD_REF(ARG05)  a05,
                     BSLS_COMPILERFEATURES_FORWARD_REF(ARG06)  a06,
                     bslma::Allocator                         *basicAllocator);
    template <class ARG01, class ARG02, class ARG03, class ARG04,
              class ARG05, class ARG06, class ARG07>
    ConstructorProxy(BSLS_COMPILERFEATURES_FORWARD_REF(ARG01)  a01,
                     BSLS_COMPILERFEATURES_FORWARD_REF(ARG02)  a02,
                     BSLS_COMPILERFEATURES_FORWARD_REF(ARG03)  a03,
                     BSLS_COMPILERFEATURES_FORWARD_REF(ARG04)  a04,
                     BSLS_COMPILERFEATURES_FORWARD_REF(ARG05)  a05,
                     BSLS_COMPILERFEATURES_FORWARD_REF(ARG06)  a06,
                     BSLS_COMPILERFEATURES_FORWARD_REF(ARG07)  a07,
                     bslma::Allocator                         *basicAllocator);
    template <class ARG01, class ARG02, class ARG03, class ARG04,
              class ARG05, class ARG06, class ARG07, class ARG08>
    ConstructorProxy(BSLS_COMPILERFEATURES_FORWARD_REF(ARG01)  a01,
                     BSLS_COMPILERFEATURES_FORWARD_REF(ARG02)  a02,
                     BSLS_COMPILERFEATURES_FORWARD_REF(ARG03)  a03,
                     BSLS_COMPILERFEATURES_FORWARD_REF(ARG04)  a04,
                     BSLS_COMPILERFEATURES_FORWARD_REF(ARG05)  a05,
                     BSLS_COMPILERFEATURES_FORWARD_REF(ARG06)  a06,
                     BSLS_COMPILERFEATURES_FORWARD_REF(ARG07)  a07,
                     BSLS_COMPILERFEATURES_FORWARD_REF(ARG08)  a08,
                     bslma::Allocator                         *basicAllocator);
    template <class ARG01, class ARG02, class ARG03, class ARG04,
              class ARG05, class ARG06, class ARG07, class ARG08,
              class ARG09>
    ConstructorProxy(BSLS_COMPILERFEATURES_FORWARD_REF(ARG01)  a01,
                     BSLS_COMPILERFEATURES_FORWARD_REF(ARG02)  a02,
                     BSLS_COMPILERFEATURES_FORWARD_REF(ARG03)  a03,
                     BSLS_COMPILERFEATURES_FORWARD_REF(ARG04)  a04,
                     BSLS_COMPILERFEATURES_FORWARD_REF(ARG05)  a05,
                     BSLS_COMPILERFEATURES_FORWARD_REF(ARG06)  a06,
                     BSLS_COMPILERFEATURES_FORWARD_REF(ARG07)  a07,
                     BSLS_COMPILERFEATURES_FORWARD_REF(ARG08)  a08,
                     BSLS_COMPILERFEATURES_FORWARD_REF(ARG09)  a09,
                     bslma::Allocator                         *basicAllocator);
    template <class ARG01, class ARG02, class ARG03, class ARG04,
              class ARG05, class ARG06, class ARG07, class ARG08,
              class ARG09, class ARG10>
    ConstructorProxy(BSLS_COMPILERFEATURES_FORWARD_REF(ARG01)  a01,
                     BSLS_COMPILERFEATURES_FORWARD_REF(ARG02)  a02,
                     BSLS_COMPILERFEATURES_FORWARD_REF(ARG03)  a03,
                     BSLS_COMPILERFEATURES_FORWARD_REF(ARG04)  a04,
                     BSLS_COMPILERFEATURES_FORWARD_REF(ARG05)  a05,
                     BSLS_COMPILERFEATURES_FORWARD_REF(ARG06)  a06,
                     BSLS_COMPILERFEATURES_FORWARD_REF(ARG07)  a07,
                     BSLS_COMPILERFEATURES_FORWARD_REF(ARG08)  a08,
                     BSLS_COMPILERFEATURES_FORWARD_REF(ARG09)  a09,
                     BSLS_COMPILERFEATURES_FORWARD_REF(ARG10)  a10,
                     bslma::Allocator                         *basicAllocator);
    template <class ARG01, class ARG02, class ARG03, class ARG04,
              class ARG05, class ARG06, class ARG07, class ARG08,
              class ARG09, class ARG10, class ARG11>
    ConstructorProxy(BSLS_COMPILERFEATURES_FORWARD_REF(ARG01)  a01,
                     BSLS_COMPILERFEATURES_FORWARD_REF(ARG02)  a02,
                     BSLS_COMPILERFEATURES_FORWARD_REF(ARG03)  a03,
                     BSLS_COMPILERFEATURES_FORWARD_REF(ARG04)  a04,
                     BSLS_COMPILERFEATURES_FORWARD_REF(ARG05)  a05,
                     BSLS_COMPILERFEATURES_FORWARD_REF(ARG06)  a06,
                     BSLS_COMPILERFEATURES_FORWARD_REF(ARG07)  a07,
                     BSLS_COMPILERFEATURES_FORWARD_REF(ARG08)  a08,
                     BSLS_COMPILERFEATURES_FORWARD_REF(ARG09)  a09,
                     BSLS_COMPILERFEATURES_FORWARD_REF(ARG10)  a10,
                     BSLS_COMPILERFEATURES_FORWARD_REF(ARG11)  a11,
                     bslma::Allocator                         *basicAllocator);
    template <class ARG01, class ARG02, class ARG03, class ARG04,
              class ARG05, class ARG06, class ARG07, class ARG08,
              class ARG09, class ARG10, class ARG11, class ARG12>
    ConstructorProxy(BSLS_COMPILERFEATURES_FORWARD_REF(ARG01)  a01,
                     BSLS_COMPILERFEATURES_FORWARD_REF(ARG02)  a02,
                     BSLS_COMPILERFEATURES_FORWARD_REF(ARG03)  a03,
                     BSLS_COMPILERFEATURES_FORWARD_REF(ARG04)  a04,
                     BSLS_COMPILERFEATURES_FORWARD_REF(ARG05)  a05,
                     BSLS_COMPILERFEATURES_FORWARD_REF(ARG06)  a06,
                     BSLS_COMPILERFEATURES_FORWARD_REF(ARG07)  a07,
                     BSLS_COMPILERFEATURES_FORWARD_REF(ARG08)  a08,
                     BSLS_COMPILERFEATURES_FORWARD_REF(ARG09)  a09,
                     BSLS_COMPILERFEATURES_FORWARD_REF(ARG10)  a10,
                     BSLS_COMPILERFEATURES_FORWARD_REF(ARG11)  a11,
                     BSLS_COMPILERFEATURES_FORWARD_REF(ARG12)  a12,
                     bslma::Allocator                         *basicAllocator);
    template <class ARG01, class ARG02, class ARG03, class ARG04,
              class ARG05, class ARG06, class ARG07, class ARG08,
              class ARG09, class ARG10, class ARG11, class ARG12,
              class ARG13>
    ConstructorProxy(BSLS_COMPILERFEATURES_FORWARD_REF(ARG01)  a01,
                     BSLS_COMPILERFEATURES_FORWARD_REF(ARG02)  a02,
                     BSLS_COMPILERFEATURES_FORWARD_REF(ARG03)  a03,
                     BSLS_COMPILERFEATURES_FORWARD_REF(ARG04)  a04,
                     BSLS_COMPILERFEATURES_FORWARD_REF(ARG05)  a05,
                     BSLS_COMPILERFEATURES_FORWARD_REF(ARG06)  a06,
                     BSLS_COMPILERFEATURES_FORWARD_REF(ARG07)  a07,
                     BSLS_COMPILERFEATURES_FORWARD_REF(ARG08)  a08,
                     BSLS_COMPILERFEATURES_FORWARD_REF(ARG09)  a09,
                     BSLS_COMPILERFEATURES_FORWARD_REF(ARG10)  a10,
                     BSLS_COMPILERFEATURES_FORWARD_REF(ARG11)  a11,
                     BSLS_COMPILERFEATURES_FORWARD_REF(ARG12)  a12,
                     BSLS_COMPILERFEATURES_FORWARD_REF(ARG13)  a13,
                     bslma::Allocator                         *basicAllocator);
    template <class ARG01, class ARG02, class ARG03, class ARG04,
              class ARG05, class ARG06, class ARG07, class ARG08,
              class ARG09, class ARG10, class ARG11, class ARG12,
              class ARG13, class ARG14>
    ConstructorProxy(BSLS_COMPILERFEATURES_FORWARD_REF(ARG01)  a01,
                     BSLS_COMPILERFEATURES_FORWARD_REF(ARG02)  a02,
                     BSLS_COMPILERFEATURES_FORWARD_REF(ARG03)  a03,
                     BSLS_COMPILERFEATURES_FORWARD_REF(ARG04)  a04,
                     BSLS_COMPILERFEATURES_FORWARD_REF(ARG05)  a05,
                     BSLS_COMPILERFEATURES_FORWARD_REF(ARG06)  a06,
                     BSLS_COMPILERFEATURES_FORWARD_REF(ARG07)  a07,
                     BSLS_COMPILERFEATURES_FORWARD_REF(ARG08)  a08,
                     BSLS_COMPILERFEATURES_FORWARD_REF(ARG09)  a09,
                     BSLS_COMPILERFEATURES_FORWARD_REF(ARG10)  a10,
                     BSLS_COMPILERFEATURES_FORWARD_REF(ARG11)  a11,
                     BSLS_COMPILERFEATURES_FORWARD_REF(ARG12)  a12,
                     BSLS_COMPILERFEATURES_FORWARD_REF(ARG13)  a13,
                     BSLS_COMPILERFEATURES_FORWARD_REF(ARG14)  a14,
                     bslma::Allocator                         *basicAllocator);
        // Construct a proxy, and a proxied object of the parameterized
        // 'OBJECT_TYPE' using the specified arguments 'a01' up to 'a14' of the
        // respective parameterized 'ARG01' up to 'ARG14' types.  Use the
        // specified 'basicAllocator' to supply memory to the proxied object if
        // 'OBJECT_TYPE' declares the 'bslma::UsesBslmaAllocator' trait, and
        // ignore 'basicAllocator' otherwise.  If 'basicAllocator' is 0, the
        // currently installed default allocator is used.  Note that a
        // compilation error will result unless 'OBJECT_TYPE' has a constructor
        // of signature compatible with 'OBJECT_TYPE(ARG01&&, ARG2&&, ...)'.

    ~ConstructorProxy();
        // Destroy this proxy and the object held by this proxy.

    // MANIPULATORS
    OBJECT_TYPE& object() BSLS_KEYWORD_NOEXCEPT;
        // Return a reference to the modifiable object held by this proxy.

    // ACCESSORS
    const OBJECT_TYPE& object() const BSLS_KEYWORD_NOEXCEPT;
        // Return a reference to the non-modifiable object held by this proxy.
};

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

                        // ----------------------
                        // class ConstructorProxy
                        // ----------------------

// CREATORS
template <class OBJECT_TYPE>
inline
ConstructorProxy<OBJECT_TYPE>::ConstructorProxy(
                                              bslma::Allocator *basicAllocator)
{
    bslma::ConstructionUtil::construct(d_objectBuffer.address(),
                                       basicAllocator);
}

template <class OBJECT_TYPE>
template <class SOURCE_TYPE>
inline
ConstructorProxy<OBJECT_TYPE>::ConstructorProxy(
                          const ConstructorProxy<SOURCE_TYPE>&  original,
                          bslma::Allocator                     *basicAllocator)
{
    bslma::ConstructionUtil::construct(d_objectBuffer.address(),
                                       basicAllocator,
                                       original.object());
}

template <class OBJECT_TYPE>
template <class SOURCE_TYPE>
inline
ConstructorProxy<OBJECT_TYPE>::ConstructorProxy(
             bslmf::MovableRef<ConstructorProxy<SOURCE_TYPE> >  original,
             bslma::Allocator                                  *basicAllocator)
{
    bslma::ConstructionUtil::construct(
                        d_objectBuffer.address(),
                        basicAllocator,
                        bslmf::MovableRefUtil::move(
                            bslmf::MovableRefUtil::access(original).object()));
}

template <class OBJECT_TYPE>
template <class ARG01>
inline
ConstructorProxy<OBJECT_TYPE>::ConstructorProxy(
                      BSLS_COMPILERFEATURES_FORWARD_REF(ARG01)  a01,
                      typename bsl::enable_if<
                          !ConstructorProxy_IsProxy<
                              typename bsl::decay<ARG01>::type>::value,
                              bslma::Allocator>::type          *basicAllocator)
{
    // NOTE: 'enable_if' is here so this constructor won't overshadow the copy
    //       and move constructors.

    bslma::ConstructionUtil::construct(
                                    d_objectBuffer.address(),
                                    basicAllocator,
                                    BSLS_COMPILERFEATURES_FORWARD(ARG01, a01));
}

template <class OBJECT_TYPE>
template <class ARG01, class ARG02>
inline
ConstructorProxy<OBJECT_TYPE>::ConstructorProxy(
                      BSLS_COMPILERFEATURES_FORWARD_REF(ARG01)  a01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ARG02)  a02,
                      bslma::Allocator                         *basicAllocator)
{
    bslma::ConstructionUtil::construct(
                                    d_objectBuffer.address(),
                                    basicAllocator,
                                    BSLS_COMPILERFEATURES_FORWARD(ARG01, a01),
                                    BSLS_COMPILERFEATURES_FORWARD(ARG02, a02));
}

template <class OBJECT_TYPE>
template <class ARG01, class ARG02, class ARG03>
inline
ConstructorProxy<OBJECT_TYPE>::ConstructorProxy(
                      BSLS_COMPILERFEATURES_FORWARD_REF(ARG01)  a01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ARG02)  a02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ARG03)  a03,
                      bslma::Allocator                         *basicAllocator)
{
    bslma::ConstructionUtil::construct(
                                    d_objectBuffer.address(),
                                    basicAllocator,
                                    BSLS_COMPILERFEATURES_FORWARD(ARG01, a01),
                                    BSLS_COMPILERFEATURES_FORWARD(ARG02, a02),
                                    BSLS_COMPILERFEATURES_FORWARD(ARG03, a03));
}

template <class OBJECT_TYPE>
template <class ARG01, class ARG02, class ARG03, class ARG04>
inline
ConstructorProxy<OBJECT_TYPE>::ConstructorProxy(
                      BSLS_COMPILERFEATURES_FORWARD_REF(ARG01)  a01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ARG02)  a02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ARG03)  a03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ARG04)  a04,
                      bslma::Allocator                         *basicAllocator)
{
    bslma::ConstructionUtil::construct(
                                    d_objectBuffer.address(),
                                    basicAllocator,
                                    BSLS_COMPILERFEATURES_FORWARD(ARG01, a01),
                                    BSLS_COMPILERFEATURES_FORWARD(ARG02, a02),
                                    BSLS_COMPILERFEATURES_FORWARD(ARG03, a03),
                                    BSLS_COMPILERFEATURES_FORWARD(ARG04, a04));
}

template <class OBJECT_TYPE>
template <class ARG01, class ARG02, class ARG03, class ARG04,
          class ARG05>
inline
ConstructorProxy<OBJECT_TYPE>::ConstructorProxy(
                      BSLS_COMPILERFEATURES_FORWARD_REF(ARG01)  a01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ARG02)  a02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ARG03)  a03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ARG04)  a04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ARG05)  a05,
                      bslma::Allocator                         *basicAllocator)
{
    bslma::ConstructionUtil::construct(
                                    d_objectBuffer.address(),
                                    basicAllocator,
                                    BSLS_COMPILERFEATURES_FORWARD(ARG01, a01),
                                    BSLS_COMPILERFEATURES_FORWARD(ARG02, a02),
                                    BSLS_COMPILERFEATURES_FORWARD(ARG03, a03),
                                    BSLS_COMPILERFEATURES_FORWARD(ARG04, a04),
                                    BSLS_COMPILERFEATURES_FORWARD(ARG05, a05));
}

template <class OBJECT_TYPE>
template <class ARG01, class ARG02, class ARG03, class ARG04,
          class ARG05, class ARG06>
inline
ConstructorProxy<OBJECT_TYPE>::ConstructorProxy(
                      BSLS_COMPILERFEATURES_FORWARD_REF(ARG01)  a01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ARG02)  a02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ARG03)  a03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ARG04)  a04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ARG05)  a05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ARG06)  a06,
                      bslma::Allocator                         *basicAllocator)
{
    bslma::ConstructionUtil::construct(
                                    d_objectBuffer.address(),
                                    basicAllocator,
                                    BSLS_COMPILERFEATURES_FORWARD(ARG01, a01),
                                    BSLS_COMPILERFEATURES_FORWARD(ARG02, a02),
                                    BSLS_COMPILERFEATURES_FORWARD(ARG03, a03),
                                    BSLS_COMPILERFEATURES_FORWARD(ARG04, a04),
                                    BSLS_COMPILERFEATURES_FORWARD(ARG05, a05),
                                    BSLS_COMPILERFEATURES_FORWARD(ARG06, a06));
}

template <class OBJECT_TYPE>
template <class ARG01, class ARG02, class ARG03, class ARG04,
          class ARG05, class ARG06, class ARG07>
inline
ConstructorProxy<OBJECT_TYPE>::ConstructorProxy(
                      BSLS_COMPILERFEATURES_FORWARD_REF(ARG01)  a01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ARG02)  a02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ARG03)  a03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ARG04)  a04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ARG05)  a05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ARG06)  a06,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ARG07)  a07,
                      bslma::Allocator                         *basicAllocator)
{
    bslma::ConstructionUtil::construct(
                                    d_objectBuffer.address(),
                                    basicAllocator,
                                    BSLS_COMPILERFEATURES_FORWARD(ARG01, a01),
                                    BSLS_COMPILERFEATURES_FORWARD(ARG02, a02),
                                    BSLS_COMPILERFEATURES_FORWARD(ARG03, a03),
                                    BSLS_COMPILERFEATURES_FORWARD(ARG04, a04),
                                    BSLS_COMPILERFEATURES_FORWARD(ARG05, a05),
                                    BSLS_COMPILERFEATURES_FORWARD(ARG06, a06),
                                    BSLS_COMPILERFEATURES_FORWARD(ARG07, a07));
}

template <class OBJECT_TYPE>
template <class ARG01, class ARG02, class ARG03, class ARG04,
          class ARG05, class ARG06, class ARG07, class ARG08>
inline
ConstructorProxy<OBJECT_TYPE>::ConstructorProxy(
                      BSLS_COMPILERFEATURES_FORWARD_REF(ARG01)  a01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ARG02)  a02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ARG03)  a03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ARG04)  a04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ARG05)  a05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ARG06)  a06,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ARG07)  a07,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ARG08)  a08,
                      bslma::Allocator                         *basicAllocator)
{
    bslma::ConstructionUtil::construct(
                                    d_objectBuffer.address(),
                                    basicAllocator,
                                    BSLS_COMPILERFEATURES_FORWARD(ARG01, a01),
                                    BSLS_COMPILERFEATURES_FORWARD(ARG02, a02),
                                    BSLS_COMPILERFEATURES_FORWARD(ARG03, a03),
                                    BSLS_COMPILERFEATURES_FORWARD(ARG04, a04),
                                    BSLS_COMPILERFEATURES_FORWARD(ARG05, a05),
                                    BSLS_COMPILERFEATURES_FORWARD(ARG06, a06),
                                    BSLS_COMPILERFEATURES_FORWARD(ARG07, a07),
                                    BSLS_COMPILERFEATURES_FORWARD(ARG08, a08));
}

template <class OBJECT_TYPE>
template <class ARG01, class ARG02, class ARG03, class ARG04,
          class ARG05, class ARG06, class ARG07, class ARG08,
          class ARG09>
inline
ConstructorProxy<OBJECT_TYPE>::ConstructorProxy(
                      BSLS_COMPILERFEATURES_FORWARD_REF(ARG01)  a01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ARG02)  a02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ARG03)  a03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ARG04)  a04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ARG05)  a05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ARG06)  a06,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ARG07)  a07,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ARG08)  a08,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ARG09)  a09,
                      bslma::Allocator                         *basicAllocator)
{
    bslma::ConstructionUtil::construct(
                                    d_objectBuffer.address(),
                                    basicAllocator,
                                    BSLS_COMPILERFEATURES_FORWARD(ARG01, a01),
                                    BSLS_COMPILERFEATURES_FORWARD(ARG02, a02),
                                    BSLS_COMPILERFEATURES_FORWARD(ARG03, a03),
                                    BSLS_COMPILERFEATURES_FORWARD(ARG04, a04),
                                    BSLS_COMPILERFEATURES_FORWARD(ARG05, a05),
                                    BSLS_COMPILERFEATURES_FORWARD(ARG06, a06),
                                    BSLS_COMPILERFEATURES_FORWARD(ARG07, a07),
                                    BSLS_COMPILERFEATURES_FORWARD(ARG08, a08),
                                    BSLS_COMPILERFEATURES_FORWARD(ARG09, a09));
}

template <class OBJECT_TYPE>
template <class ARG01, class ARG02, class ARG03, class ARG04,
          class ARG05, class ARG06, class ARG07, class ARG08,
          class ARG09, class ARG10>
inline
ConstructorProxy<OBJECT_TYPE>::ConstructorProxy(
                      BSLS_COMPILERFEATURES_FORWARD_REF(ARG01)  a01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ARG02)  a02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ARG03)  a03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ARG04)  a04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ARG05)  a05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ARG06)  a06,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ARG07)  a07,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ARG08)  a08,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ARG09)  a09,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ARG10)  a10,
                      bslma::Allocator                         *basicAllocator)
{
    bslma::ConstructionUtil::construct(
                                    d_objectBuffer.address(),
                                    basicAllocator,
                                    BSLS_COMPILERFEATURES_FORWARD(ARG01, a01),
                                    BSLS_COMPILERFEATURES_FORWARD(ARG02, a02),
                                    BSLS_COMPILERFEATURES_FORWARD(ARG03, a03),
                                    BSLS_COMPILERFEATURES_FORWARD(ARG04, a04),
                                    BSLS_COMPILERFEATURES_FORWARD(ARG05, a05),
                                    BSLS_COMPILERFEATURES_FORWARD(ARG06, a06),
                                    BSLS_COMPILERFEATURES_FORWARD(ARG07, a07),
                                    BSLS_COMPILERFEATURES_FORWARD(ARG08, a08),
                                    BSLS_COMPILERFEATURES_FORWARD(ARG09, a09),
                                    BSLS_COMPILERFEATURES_FORWARD(ARG10, a10));
}

template <class OBJECT_TYPE>
template <class ARG01, class ARG02, class ARG03, class ARG04,
          class ARG05, class ARG06, class ARG07, class ARG08,
          class ARG09, class ARG10, class ARG11>
inline
ConstructorProxy<OBJECT_TYPE>::ConstructorProxy(
                      BSLS_COMPILERFEATURES_FORWARD_REF(ARG01)  a01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ARG02)  a02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ARG03)  a03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ARG04)  a04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ARG05)  a05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ARG06)  a06,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ARG07)  a07,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ARG08)  a08,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ARG09)  a09,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ARG10)  a10,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ARG11)  a11,
                      bslma::Allocator                         *basicAllocator)
{
    bslma::ConstructionUtil::construct(
                                    d_objectBuffer.address(),
                                    basicAllocator,
                                    BSLS_COMPILERFEATURES_FORWARD(ARG01, a01),
                                    BSLS_COMPILERFEATURES_FORWARD(ARG02, a02),
                                    BSLS_COMPILERFEATURES_FORWARD(ARG03, a03),
                                    BSLS_COMPILERFEATURES_FORWARD(ARG04, a04),
                                    BSLS_COMPILERFEATURES_FORWARD(ARG05, a05),
                                    BSLS_COMPILERFEATURES_FORWARD(ARG06, a06),
                                    BSLS_COMPILERFEATURES_FORWARD(ARG07, a07),
                                    BSLS_COMPILERFEATURES_FORWARD(ARG08, a08),
                                    BSLS_COMPILERFEATURES_FORWARD(ARG09, a09),
                                    BSLS_COMPILERFEATURES_FORWARD(ARG10, a10),
                                    BSLS_COMPILERFEATURES_FORWARD(ARG11, a11));
}

template <class OBJECT_TYPE>
template <class ARG01, class ARG02, class ARG03, class ARG04,
          class ARG05, class ARG06, class ARG07, class ARG08,
          class ARG09, class ARG10, class ARG11, class ARG12>
inline
ConstructorProxy<OBJECT_TYPE>::ConstructorProxy(
                      BSLS_COMPILERFEATURES_FORWARD_REF(ARG01)  a01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ARG02)  a02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ARG03)  a03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ARG04)  a04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ARG05)  a05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ARG06)  a06,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ARG07)  a07,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ARG08)  a08,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ARG09)  a09,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ARG10)  a10,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ARG11)  a11,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ARG12)  a12,
                      bslma::Allocator                         *basicAllocator)
{
    bslma::ConstructionUtil::construct(
                                    d_objectBuffer.address(),
                                    basicAllocator,
                                    BSLS_COMPILERFEATURES_FORWARD(ARG01, a01),
                                    BSLS_COMPILERFEATURES_FORWARD(ARG02, a02),
                                    BSLS_COMPILERFEATURES_FORWARD(ARG03, a03),
                                    BSLS_COMPILERFEATURES_FORWARD(ARG04, a04),
                                    BSLS_COMPILERFEATURES_FORWARD(ARG05, a05),
                                    BSLS_COMPILERFEATURES_FORWARD(ARG06, a06),
                                    BSLS_COMPILERFEATURES_FORWARD(ARG07, a07),
                                    BSLS_COMPILERFEATURES_FORWARD(ARG08, a08),
                                    BSLS_COMPILERFEATURES_FORWARD(ARG09, a09),
                                    BSLS_COMPILERFEATURES_FORWARD(ARG10, a10),
                                    BSLS_COMPILERFEATURES_FORWARD(ARG11, a11),
                                    BSLS_COMPILERFEATURES_FORWARD(ARG12, a12));
}

template <class OBJECT_TYPE>
template <class ARG01, class ARG02, class ARG03, class ARG04,
          class ARG05, class ARG06, class ARG07, class ARG08,
          class ARG09, class ARG10, class ARG11, class ARG12,
          class ARG13>
inline
ConstructorProxy<OBJECT_TYPE>::ConstructorProxy(
                      BSLS_COMPILERFEATURES_FORWARD_REF(ARG01)  a01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ARG02)  a02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ARG03)  a03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ARG04)  a04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ARG05)  a05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ARG06)  a06,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ARG07)  a07,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ARG08)  a08,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ARG09)  a09,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ARG10)  a10,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ARG11)  a11,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ARG12)  a12,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ARG13)  a13,
                      bslma::Allocator                         *basicAllocator)
{
    bslma::ConstructionUtil::construct(
                                    d_objectBuffer.address(),
                                    basicAllocator,
                                    BSLS_COMPILERFEATURES_FORWARD(ARG01, a01),
                                    BSLS_COMPILERFEATURES_FORWARD(ARG02, a02),
                                    BSLS_COMPILERFEATURES_FORWARD(ARG03, a03),
                                    BSLS_COMPILERFEATURES_FORWARD(ARG04, a04),
                                    BSLS_COMPILERFEATURES_FORWARD(ARG05, a05),
                                    BSLS_COMPILERFEATURES_FORWARD(ARG06, a06),
                                    BSLS_COMPILERFEATURES_FORWARD(ARG07, a07),
                                    BSLS_COMPILERFEATURES_FORWARD(ARG08, a08),
                                    BSLS_COMPILERFEATURES_FORWARD(ARG09, a09),
                                    BSLS_COMPILERFEATURES_FORWARD(ARG10, a10),
                                    BSLS_COMPILERFEATURES_FORWARD(ARG11, a11),
                                    BSLS_COMPILERFEATURES_FORWARD(ARG12, a12),
                                    BSLS_COMPILERFEATURES_FORWARD(ARG13, a13));
}

template <class OBJECT_TYPE>
template <class ARG01, class ARG02, class ARG03, class ARG04,
          class ARG05, class ARG06, class ARG07, class ARG08,
          class ARG09, class ARG10, class ARG11, class ARG12,
          class ARG13, class ARG14>
inline
ConstructorProxy<OBJECT_TYPE>::ConstructorProxy(
                      BSLS_COMPILERFEATURES_FORWARD_REF(ARG01)  a01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ARG02)  a02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ARG03)  a03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ARG04)  a04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ARG05)  a05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ARG06)  a06,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ARG07)  a07,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ARG08)  a08,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ARG09)  a09,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ARG10)  a10,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ARG11)  a11,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ARG12)  a12,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ARG13)  a13,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ARG14)  a14,
                      bslma::Allocator                         *basicAllocator)
{
    bslma::ConstructionUtil::construct(
                                    d_objectBuffer.address(),
                                    basicAllocator,
                                    BSLS_COMPILERFEATURES_FORWARD(ARG01, a01),
                                    BSLS_COMPILERFEATURES_FORWARD(ARG02, a02),
                                    BSLS_COMPILERFEATURES_FORWARD(ARG03, a03),
                                    BSLS_COMPILERFEATURES_FORWARD(ARG04, a04),
                                    BSLS_COMPILERFEATURES_FORWARD(ARG05, a05),
                                    BSLS_COMPILERFEATURES_FORWARD(ARG06, a06),
                                    BSLS_COMPILERFEATURES_FORWARD(ARG07, a07),
                                    BSLS_COMPILERFEATURES_FORWARD(ARG08, a08),
                                    BSLS_COMPILERFEATURES_FORWARD(ARG09, a09),
                                    BSLS_COMPILERFEATURES_FORWARD(ARG10, a10),
                                    BSLS_COMPILERFEATURES_FORWARD(ARG11, a11),
                                    BSLS_COMPILERFEATURES_FORWARD(ARG12, a12),
                                    BSLS_COMPILERFEATURES_FORWARD(ARG13, a13),
                                    BSLS_COMPILERFEATURES_FORWARD(ARG14, a14));
}

template <class OBJECT_TYPE>
inline
ConstructorProxy<OBJECT_TYPE>::~ConstructorProxy()
{
    bslma::DestructionUtil::destroy(d_objectBuffer.address());
}

// MANIPULATORS
template <class OBJECT_TYPE>
inline
OBJECT_TYPE& ConstructorProxy<OBJECT_TYPE>::object() BSLS_KEYWORD_NOEXCEPT
{
    return d_objectBuffer.object();
}

// ACCESSORS
template <class OBJECT_TYPE>
inline
const OBJECT_TYPE& ConstructorProxy<OBJECT_TYPE>::object() const
                                                          BSLS_KEYWORD_NOEXCEPT
{
    return d_objectBuffer.object();
}

}  // close package namespace

// ============================================================================
//                                TYPE TRAITS
// ============================================================================

namespace bslma {

template <class OBJECT_TYPE>
struct UsesBslmaAllocator<bslalg::ConstructorProxy<OBJECT_TYPE> >
    : bsl::true_type
{};

}  // close namespace bslma

namespace bslmf {

template <class OBJECT_TYPE>
struct IsBitwiseMoveable<bslalg::ConstructorProxy<OBJECT_TYPE> > :
                                                 IsBitwiseMoveable<OBJECT_TYPE>
{};

}  // close namespace bslmf

#ifndef BDE_OPENSOURCE_PUBLICATION  // BACKWARD_COMPATIBILITY
// ============================================================================
//                           BACKWARD COMPATIBILITY
// ============================================================================

#ifdef bslalg_ConstructorProxy
#undef bslalg_ConstructorProxy
#endif
#define bslalg_ConstructorProxy bslalg::ConstructorProxy
    // This alias is defined for backward compatibility.
#endif  // BDE_OPENSOURCE_PUBLICATION -- BACKWARD_COMPATIBILITY

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
