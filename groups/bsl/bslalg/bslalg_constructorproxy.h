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
//@DESCRIPTION: This component provides a proxy class template,
// 'bslalg::ConstructorProxy', for creating a proxied object of parameter type
// 'OBJECT_TYPE' using a uniform constructor syntax, regardless of whether
// 'OBJECT_TYPE' is allocator-aware (AA) -- i.e., uses an allocator to supply
// memory.  This proxy is useful in generic programming situations where an
// object of a given type must be constructed, but it is not known in advance
// which allocator model the object supports, if any.  In these situations,
// client code unconditionally passes an allocator as the last argument to the
// 'ConstructorProxy' constructor; the constructor forwards the allocator to
// the proxied object if 'OBJECT_TYPE' is AA and discards it otherwise.
//
// The proxied object is owned by the 'ConstructorProxy' object.  Modifiable
// and non-modifiable access to the proxied object may be obtained using the
// overloaded 'object' methods.  When the proxy is destroyed, it automatically
// destroys its proxied object.
//
// See the 'bslma' package-level documentation for more information about using
// allocators.
//
///Usage
///-----
///Example 1: Conditionally pass an allocator to a template member ctor
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// In this example, we create a key-value class template consiting of a
// string key paired with a value of template-parameter type.  Since the value
// type might be allocator aware (AA), we want to ensure that our key-value
// class template can pass an allocator to its value-type constructor.
//
// First, we define a simple AA string class that will be our value type for
// testing:
//..
//  #include <bslma_bslallocator.h>
//  #include <bslma_allocatorutil.h>
//  #include <cstring>
//
//  class String {
//      // Basic allocator-aware string class
//
//      // DATA
//      bsl::allocator<char>  d_allocator;
//      std::size_t           d_length;
//      char                 *d_data;
//
//    public:
//      // TYPES
//      typedef bsl::allocator<char> allocator_type;
//
//      // CREATORS
//      String(const char            *str = "",
//             const allocator_type&  a = allocator_type());        // IMPLICIT
//      String(const String&          original,
//             const allocator_type&  a = allocator_type());
//      ~String();
//
//      // MANIPULATORS
//      String& operator=(const String& rhs);
//
//      // ACCESSORS
//      const char* c_str() const { return d_data; }
//      allocator_type get_allocator() const { return d_allocator; }
//      std::size_t size() const { return d_length; }
//  };
//
//  // FREE FUNCTIONS
//  bool operator==(const String& a, const String& b);
//  bool operator!=(const String& a, const String& b);
//..
// Next, we define the constructors, destructor, and equality-comparison
// operators.  For brevity, we've omited the implementation of the assignment
// operator, which is not used in this example:
//..
//  String::String(const char *str, const allocator_type& a)
//      : d_allocator(a), d_length(std::strlen(str))
//  {
//      d_data = static_cast<char *>(
//          bslma::AllocatorUtil::allocateBytes(a, d_length + 1));
//      std::memcpy(d_data, str, d_length + 1);
//  }
//
//  String::String(const String& original, const allocator_type& a)
//      : d_allocator(a), d_length(original.d_length)
//  {
//      d_data = static_cast<char *>(
//          bslma::AllocatorUtil::allocateBytes(a, d_length + 1));
//      std::memcpy(d_data, original.c_str(), d_length);
//      d_data[d_length] = '\0';
//  }
//
//  String::~String()
//  {
//      bslma::AllocatorUtil::deallocateBytes(d_allocator, d_data, d_length+1);
//  }
//
//  bool operator==(const String& a, const String& b)
//  {
//      return (a.size() == b.size() &&
//              0 == std::memcmp(a.c_str(), b.c_str(), a.size()));
//  }
//
//  bool operator!=(const String& a, const String& b)
//  {
//      return ! (a == b);
//  }
//..
// Now we are ready to define our key-value template.  The data portion of the
// template needs a member for the key and one for the value.  Rather than
// defining the value member as simply a member variable of 'TYPE', we use
// 'bslalg::ConstructorProxy' to ensure that we will be able to construct it in
// a uniform way even though we do not know whether or not it is
// allocator-aware:
//..
//  #include <bslalg_constructorproxy.h>
//
//  template <class TYPE>
//  class KeyValue {
//      // Key-value pair with string key and arbitrary value type.
//
//      // DATA
//      String                         d_key;
//      bslalg::ConstructorProxy<TYPE> d_valueProxy;
//..
// Next, we declare the creators and manipulators typical of an AA attribute
// class:
//..
//    public:
//      typedef bsl::allocator<> allocator_type;
//
//      // CREATORS
//      KeyValue(const String&         k,
//               const TYPE&           v,
//               const allocator_type& a = allocator_type());
//      KeyValue(const KeyValue&       original,
//               const allocator_type& a = allocator_type());
//      ~KeyValue();
//
//      // MANIPULATORS
//      KeyValue& operator=(const KeyValue& rhs);
//..
// Next, we declare the accessessors and, for convenience in this example,
// define them inline.  Note that the 'value' accessor extracts the proxied
// object from the 'd_valueProxy' member:
//..
//      // ACCESSESSORS
//      allocator_type get_allocator() const { return d_key.get_allocator(); }
//      const String&  key()   const { return d_key; }
//      const TYPE&    value() const { return d_valueProxy.object(); }
//  };
//..
// Next, we define the value constructor, which passes its allocator argument
// to both data members' constructors.  Note that the 'd_valueProxy',
// constructor always expects an allocator argument, even if 'TYPE' is not AA:
//..
//  template <class TYPE>
//  KeyValue<TYPE>::KeyValue(const String&         k,
//                           const TYPE&           v,
//                           const allocator_type& a)
//      : d_key(k, a), d_valueProxy(v, a)
//  {
//  }
//..
// Next, we define the copy constructor and assignment operator.  Since
// 'bslalg::ConstructorProxy' is not copyable, we must manually extract the
// proxied object in the assignment operator.  This extraction is not needed in
// the copy constructor because the single-value proxy constructor
// automatically "unwraps" its argument when presented with an instantiation of
// 'bslalg::ConstructorProxy':
//..
//  template <class TYPE>
//  KeyValue<TYPE>::KeyValue(const KeyValue&       original,
//                           const allocator_type& a)
//      : d_key(original.d_key, a)
//      , d_valueProxy(original.d_valueProxy, a)  // Automatically unwrapped
//  {
//  }
//
//  template <class TYPE>
//  KeyValue<TYPE>& KeyValue<TYPE>::operator=(const KeyValue& rhs)
//  {
//      d_key                 = rhs.d_key;
//      d_valueProxy.object() = rhs.d_valueProxy.object();
//      return *this;
//  }
//..
// Last, we define the destructor, which does nothing explicit (and could
// therefore have been defaulted), because both 'String' and 'ConstructorProxy'
// clean up after themselves:
//..
//  template <class TYPE>
//  KeyValue<TYPE>::~KeyValue()
//  {
//  }
//..
// Now we can illustrate the use of our key-value pair by defining a string-int
// pair and constructing it with a test allocator.  Note that the allocator was
// passed to the ('String') key, as we would expect:
//..
//  #include <bslma_testallocator.h>
//
//  int main()
//  {
//      bslma::TestAllocator ta;
//
//      KeyValue<int> kv1("hello", 2023, &ta);
//      assert("hello" == kv1.key());
//      assert(2023    == kv1.value());
//      assert(&ta     == kv1.get_allocator());
//      assert(&ta     == kv1.key().get_allocator());
//..
// Next, we define a string-string pair and show that the allocator was
// passed to *both* the key and value parts of the pair:
//..
//      KeyValue<String> kv2("March", "Madness", &ta);
//      assert("March"   == kv2.key());
//      assert("Madness" == kv2.value());
//      assert(&ta       == kv2.get_allocator());
//      assert(&ta       == kv2.key().get_allocator());
//      assert(&ta       == kv2.value().get_allocator());
//..
// Finally, we declare a 'bslalg::ConstructorProxy' of 'KeyValue' and show how
// we can pass more than one argument (up to 14) -- in addition to the
// allocator -- to the proxied type's constructor:
//..
//      typedef KeyValue<int> UnitVal;
//
//      bslalg::ConstructorProxy<UnitVal> uvProxy("km", 14, &ta);
//      UnitVal& uv = uvProxy.object();
//      assert("km" == uv.key());
//      assert(14   == uv.value());
//      assert(&ta  == uv.get_allocator());
//  }
//..

#include <bslscm_version.h>

#include <bslma_aamodel.h>
#include <bslma_constructionutil.h>
#include <bslma_destructionutil.h>
#include <bslma_usesbslmaallocator.h>

#include <bslmf_enableif.h>
#include <bslmf_integralconstant.h>
#include <bslmf_issame.h>
#include <bslmf_movableref.h>
#include <bslmf_removecvref.h>

#include <bsls_compilerfeatures.h>
#include <bsls_keyword.h>
#include <bsls_objectbuffer.h>

#ifndef BDE_DONT_ALLOW_TRANSITIVE_INCLUDES
#include <bslalg_scalarprimitives.h>
#endif // BDE_DONT_ALLOW_TRANSITIVE_INCLUDES

namespace BloombergLP {
namespace bslalg {

// FORWARD DECLARATIONS
template <class OBJECT_TYPE> class  ConstructorProxy;
template <class TYPE = bsl::polymorphic_allocator<>::value_type>
class ConstructorProxy_PolymorphicAllocator;
template <class TYPE, class AAMODEL = typename bslma::AAModel<TYPE>::type >
struct ConstructorProxy_AllocatorType;

                    // ===============================
                    // struct ConstructorProxy_ImpUtil
                    // ===============================

struct ConstructorProxy_ImpUtil {
    // Component-private utility class for implementation methods.

    // CLASS METHODS
    template <class TYPE>
    static TYPE&                   unproxy(TYPE&                          obj);
    template <class TYPE>
    static TYPE&                   unproxy(ConstructorProxy<TYPE>&        obj);
    template <class TYPE>
    static const TYPE&             unproxy(const TYPE&                    obj);
    template <class TYPE>
    static const TYPE&             unproxy(const ConstructorProxy<TYPE>&  obj);

#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
    template <class TYPE>
    static TYPE&&                  unproxy(TYPE&&                         obj);
    template <class TYPE>
    static TYPE&&                  unproxy(ConstructorProxy<TYPE>&&       obj);
#else
    template <class TYPE>
    static bslmf::MovableRef<TYPE> unproxy(bslmf::MovableRef<TYPE>        obj);
    template <class TYPE>
    static bslmf::MovableRef<TYPE> unproxy(
                               bslmf::MovableRef<ConstructorProxy<TYPE> > obj);
#endif
        // If the specified 'obj' is a specialization of 'ConstructorProxy',
        // return the object stored within 'obj'; otherwise return 'obj'
        // unchanged.  Note that the value category (i.e., lvalue vs. xvalue)
        // of 'obj' is retained.
};

                    // ===============================
                    // class template ConstructorProxy
                    // ===============================

template <class OBJECT_TYPE>
class ConstructorProxy {
    // This class acts as a proxy for constructing and destroying an object of
    // parameterized 'OBJECT_TYPE', where 'OBJECT_TYPE' may or may not use a
    // 'bslma' allocator for supplying memory.  The constructors for this proxy
    // class take a 'bslma::Allocator *'.  If 'OBJECT_TYPE' has the
    // 'bslma::UsesBslmaAllocator' trait declared, then the supplied
    // allocator will be used to construct the proxied object.  Otherwise, the
    // allocator is ignored.

    // PRIVATE TYPES
    typedef typename
    ConstructorProxy_AllocatorType<OBJECT_TYPE>::ArgType CtorAllocArgT;

    // DATA
    bsls::ObjectBuffer<OBJECT_TYPE> d_objectBuffer;  // footprint of proxied
                                                     // object (raw buffer)

    // PRIVATE CLASS METHODS
    const typename ConstructorProxy_AllocatorType<OBJECT_TYPE>::type&
    unwrapAlloc(const CtorAllocArgT& alloc);
        // Unwrap the specified 'alloc', returning the underlying allocator
        // used to construct 'OBJECT_TYPE', if any.

  private:
    // NOT IMPLEMENTED
    ConstructorProxy(const ConstructorProxy&)            BSLS_KEYWORD_DELETED;
    ConstructorProxy& operator=(const ConstructorProxy&) BSLS_KEYWORD_DELETED;

  public:
    // TYPES
    typedef OBJECT_TYPE                               ValueType;

    typedef typename
    ConstructorProxy_AllocatorType<OBJECT_TYPE>::type allocator_type;
        // Minimally picky allocator type that can be used to construct a
        // 'ConstructorProxy'.  Choose 'bsl::polymorphic_allocator' If
        // 'OBJECT_TYPE' is not AA, 'bsl::allocator<char>' if 'OBJECT_TYPE' is
        // *legacy-AA*, and 'OBJECT_TYPE::allocator_type' otherwise.

    // CREATORS
    explicit ConstructorProxy(const CtorAllocArgT&  allocator);
        // Construct a proxy, passing no arguments except possibly a specified
        // 'allocator' to the constructor of the proxied object.  If
        // 'OBJECT_TYPE' is allocator aware and 'allocator_type' is a
        // compatible allocator type, pass 'allocator' to the proxied object
        // constructor; otherwise ignore 'allocator'.  A compilation error will
        // result unless 'OBJECT_TYPE' has an (extended) default constructor.

    template <class ARG01>
    ConstructorProxy(ARG01&                                   a01,
                     const CtorAllocArgT&                     allocator);
    template <class ARG01>
    ConstructorProxy(BSLS_COMPILERFEATURES_FORWARD_REF(ARG01) a01,
                     const CtorAllocArgT&                     allocator);
        // Construct a proxy, passing a single argument and possibly a
        // specified 'allocator' to the constructor of the proxied object,
        // where the non-allocator argument is the specified 'a01' argument if
        // 'ARG01' is not a specialization of 'ConstructorProxy', and
        // 'a01.object()' if it is such a specialization.  If 'OBJECT_TYPE' is
        // allocator aware and 'allocator_type' is a compatible allocator type,
        // pass 'allocator' to the proxied object's constructor; otherwise
        // ignore 'allocator'.  A compilation error will result unless
        // 'OBJECT_TYPE' has a constructor with a signature compatible with
        // 'OBJECT_TYPE(ARG01&&)'.  Note that, if 'ARG01' is
        // 'ConstructorProxy<OBJECT_TYPE>', then these constructors take on the
        // rolls of the extended copy and extended move constructors.

    template <class ARG01, class ARG02>
    ConstructorProxy(BSLS_COMPILERFEATURES_FORWARD_REF(ARG01)  a01,
                     BSLS_COMPILERFEATURES_FORWARD_REF(ARG02)  a02,
                     const CtorAllocArgT&                      allocator);
    template <class ARG01, class ARG02, class ARG03>
    ConstructorProxy(BSLS_COMPILERFEATURES_FORWARD_REF(ARG01)  a01,
                     BSLS_COMPILERFEATURES_FORWARD_REF(ARG02)  a02,
                     BSLS_COMPILERFEATURES_FORWARD_REF(ARG03)  a03,
                     const CtorAllocArgT&                      allocator);
    template <class ARG01, class ARG02, class ARG03, class ARG04>
    ConstructorProxy(BSLS_COMPILERFEATURES_FORWARD_REF(ARG01)  a01,
                     BSLS_COMPILERFEATURES_FORWARD_REF(ARG02)  a02,
                     BSLS_COMPILERFEATURES_FORWARD_REF(ARG03)  a03,
                     BSLS_COMPILERFEATURES_FORWARD_REF(ARG04)  a04,
                     const CtorAllocArgT&                      allocator);
    template <class ARG01, class ARG02, class ARG03, class ARG04,
              class ARG05>
    ConstructorProxy(BSLS_COMPILERFEATURES_FORWARD_REF(ARG01)  a01,
                     BSLS_COMPILERFEATURES_FORWARD_REF(ARG02)  a02,
                     BSLS_COMPILERFEATURES_FORWARD_REF(ARG03)  a03,
                     BSLS_COMPILERFEATURES_FORWARD_REF(ARG04)  a04,
                     BSLS_COMPILERFEATURES_FORWARD_REF(ARG05)  a05,
                     const CtorAllocArgT&                      allocator);
    template <class ARG01, class ARG02, class ARG03, class ARG04,
              class ARG05, class ARG06>
    ConstructorProxy(BSLS_COMPILERFEATURES_FORWARD_REF(ARG01)  a01,
                     BSLS_COMPILERFEATURES_FORWARD_REF(ARG02)  a02,
                     BSLS_COMPILERFEATURES_FORWARD_REF(ARG03)  a03,
                     BSLS_COMPILERFEATURES_FORWARD_REF(ARG04)  a04,
                     BSLS_COMPILERFEATURES_FORWARD_REF(ARG05)  a05,
                     BSLS_COMPILERFEATURES_FORWARD_REF(ARG06)  a06,
                     const CtorAllocArgT&                      allocator);
    template <class ARG01, class ARG02, class ARG03, class ARG04,
              class ARG05, class ARG06, class ARG07>
    ConstructorProxy(BSLS_COMPILERFEATURES_FORWARD_REF(ARG01)  a01,
                     BSLS_COMPILERFEATURES_FORWARD_REF(ARG02)  a02,
                     BSLS_COMPILERFEATURES_FORWARD_REF(ARG03)  a03,
                     BSLS_COMPILERFEATURES_FORWARD_REF(ARG04)  a04,
                     BSLS_COMPILERFEATURES_FORWARD_REF(ARG05)  a05,
                     BSLS_COMPILERFEATURES_FORWARD_REF(ARG06)  a06,
                     BSLS_COMPILERFEATURES_FORWARD_REF(ARG07)  a07,
                     const CtorAllocArgT&                      allocator);
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
                     const CtorAllocArgT&                      allocator);
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
                     const CtorAllocArgT&                      allocator);
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
                     const CtorAllocArgT&                      allocator);
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
                     const CtorAllocArgT&                      allocator);
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
                     const CtorAllocArgT&                      allocator);
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
                     const CtorAllocArgT&                      allocator);
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
                     const CtorAllocArgT&                      allocator);
        // Construct a proxy, forwarding the specified 'a01' up to the
        // specified 'a14' arguments and possibly a specified 'allocator' to
        // the constructor of the proxied object.  If 'OBJECT_TYPE' is
        // allocator aware and 'allocator_type' is a compatible allocator type,
        // pass 'allocator' to the proxied object's constructor; otherwise
        // ignore 'allocator'.  A compilation error will result unless
        // 'OBJECT_TYPE' has a constructor with a signature compatible with
        // 'OBJECT_TYPE(ARG01&&, ARG2&&, ...)'.  Note that, in C++03, non-const
        // lvalue arguments will be forwarded as 'const' lvalue references.

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

            // -----------------------------------------------------
            // struct template ConstructorProxy_PolymorphicAllocator
            // -----------------------------------------------------

template <class TYPE>
class ConstructorProxy_PolymorphicAllocator
    : public bsl::polymorphic_allocator<TYPE> {
    // Wrapper around 'bsl::polymorphic_allocator' that can tolerate being
    // constructed with a null pointer.

    typedef bsl::polymorphic_allocator<TYPE> Base;

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(ConstructorProxy_PolymorphicAllocator,
                                   bslma::IsStdAllocator);
    BSLMF_NESTED_TRAIT_DECLARATION_IF(ConstructorProxy_PolymorphicAllocator,
                                      bslma::UsesBslmaAllocator, false);

    // CREATORS
    ConstructorProxy_PolymorphicAllocator(bsl::memory_resource *r = 0)
                                                                    // IMPLICIT
        // Construct from the address of a 'memory_resource' optionally
        // specified by 'r'.  If 'r' is null or not specified, construct from
        // the default allocator.
        : Base(r ? Base(r) : Base()) { }

    template <class T2>
    ConstructorProxy_PolymorphicAllocator(
        const bsl::polymorphic_allocator<T2> &other)                // IMPLICIT
        // Create an allocator using the same 'memory_resource' as the
        // specified 'other' allocator.
        : Base(other) { }

    //! ConstructorProxy_PolymorphicAllocator(
    //!                const ConstructorProxy_PolymorphicAllocator&) = default;
    //! ~ConstructorProxy_PolymorphicAllocator();
};

            // ----------------------------------------------
            // struct template ConstructorProxy_AllocatorType
            // ----------------------------------------------

template <class TYPE, class AAMODEL>
struct ConstructorProxy_AllocatorType
{
    // Metafunction to determine the allocator type for a specified template
    // parameter 'TYPE' using the specified template parater 'AAMODEL' for
    // constructors.  This primary template yields a nested 'type' of
    // 'bsl::polymorphic_allocator', which is the most permisive type to use as
    // a constructor parameter, and an 'ArgType' allocator constructor argument
    // that is a wrapper around 'polymorphic_allocator' that tolerates being
    // constructed with a null pointer.  However, if 'AAMODEL' is 'AAModelNone'
    // or 'AAModelStl', the allocator constructor argument is ignored and not
    // passed to the proxied object.

    // TYPES
    typedef bsl::polymorphic_allocator<>            type;
    typedef ConstructorProxy_PolymorphicAllocator<> ArgType;
};

template <class TYPE>
struct ConstructorProxy_AllocatorType<TYPE, bslma::AAModelBsl>
{
    // Specialization for a bsl-AA 'TYPE'.

    // TYPES
    typedef bsl::allocator<> type;
    typedef bsl::allocator<> ArgType;
};

template <class TYPE>
struct ConstructorProxy_AllocatorType<TYPE, bslma::AAModelLegacy>
{
    // Specialization for a legacy-AA 'TYPE'.  The proxy type will be bsl-AA.

    // TYPES
    typedef bsl::allocator<> type;
    typedef bsl::allocator<> ArgType;
};


                    // -------------------------------
                    // struct ConstructorProxy_ImpUtil
                    // -------------------------------

// PRIVATE METHODS
template <class TYPE>
inline
TYPE& ConstructorProxy_ImpUtil::unproxy(TYPE& obj)
{
    return obj;
}

template <class TYPE>
inline
TYPE& ConstructorProxy_ImpUtil::unproxy(ConstructorProxy<TYPE>& obj)
{
    return obj.object();
}

template <class TYPE>
inline
const TYPE& ConstructorProxy_ImpUtil::unproxy(const TYPE& obj)
{
    return obj;
}

template <class TYPE>
inline
const TYPE&
ConstructorProxy_ImpUtil::unproxy(const ConstructorProxy<TYPE>& obj)
{
    return obj.object();
}

#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES

template <class TYPE>
inline
TYPE&& ConstructorProxy_ImpUtil::unproxy(TYPE&& obj)
{
    return bslmf::MovableRefUtil::move(obj);
}

template <class TYPE>
inline
TYPE&& ConstructorProxy_ImpUtil::unproxy(ConstructorProxy<TYPE>&& obj)
{
    return bslmf::MovableRefUtil::move(obj.object());
}

#else // if !BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES

template <class TYPE>
inline
bslmf::MovableRef<TYPE>
ConstructorProxy_ImpUtil::unproxy(bslmf::MovableRef<TYPE> obj)
{
    return BSLS_COMPILERFEATURES_FORWARD(TYPE, obj);
}

template <class TYPE>
inline
bslmf::MovableRef<TYPE> ConstructorProxy_ImpUtil::unproxy(
                                bslmf::MovableRef<ConstructorProxy<TYPE> > obj)
{
    return bslmf::MovableRefUtil::move(
                                  bslmf::MovableRefUtil::access(obj).object());
}

#endif // !BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES


                    // -------------------------------
                    // class template ConstructorProxy
                    // -------------------------------

// PRIVATE CLASS METHODS
template <class OBJECT_TYPE>
inline
const typename ConstructorProxy_AllocatorType<OBJECT_TYPE>::type&
ConstructorProxy<OBJECT_TYPE>::unwrapAlloc(const CtorAllocArgT& alloc)
{
    return alloc;
}

// CREATORS
template <class OBJECT_TYPE>
inline
ConstructorProxy<OBJECT_TYPE>::ConstructorProxy(const CtorAllocArgT& allocator)
{
    bslma::ConstructionUtil::construct(d_objectBuffer.address(), allocator);
}

template <class OBJECT_TYPE>
template <class ARG01>
inline
ConstructorProxy<OBJECT_TYPE>::ConstructorProxy(
                                               ARG01&                a01,
                                               const CtorAllocArgT&  allocator)
{
    bslma::ConstructionUtil::construct(d_objectBuffer.address(),
                                       allocator,
                                       ConstructorProxy_ImpUtil::unproxy(a01));
}

template <class OBJECT_TYPE>
template <class ARG01>
inline
ConstructorProxy<OBJECT_TYPE>::ConstructorProxy(
                            BSLS_COMPILERFEATURES_FORWARD_REF(ARG01) a01,
                            const CtorAllocArgT&                     allocator)
{
    bslma::ConstructionUtil::construct(
        d_objectBuffer.address(),
        allocator,
        ConstructorProxy_ImpUtil::unproxy(
                                   BSLS_COMPILERFEATURES_FORWARD(ARG01, a01)));
}

template <class OBJECT_TYPE>
template <class ARG01, class ARG02>
inline
ConstructorProxy<OBJECT_TYPE>::ConstructorProxy(
                      BSLS_COMPILERFEATURES_FORWARD_REF(ARG01)  a01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ARG02)  a02,
                      const CtorAllocArgT&                      allocator)
{
    bslma::ConstructionUtil::construct(
                                    d_objectBuffer.address(),
                                    allocator,
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
                      const CtorAllocArgT&                      allocator)
{
    bslma::ConstructionUtil::construct(
                                    d_objectBuffer.address(),
                                    allocator,
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
                      const CtorAllocArgT&                      allocator)
{
    bslma::ConstructionUtil::construct(
                                    d_objectBuffer.address(),
                                    allocator,
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
                      const CtorAllocArgT&                      allocator)
{
    bslma::ConstructionUtil::construct(
                                    d_objectBuffer.address(),
                                    allocator,
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
                      const CtorAllocArgT&                      allocator)
{
    bslma::ConstructionUtil::construct(
                                    d_objectBuffer.address(),
                                    allocator,
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
                      const CtorAllocArgT&                      allocator)
{
    bslma::ConstructionUtil::construct(
                                    d_objectBuffer.address(),
                                    allocator,
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
                      const CtorAllocArgT&                      allocator)
{
    bslma::ConstructionUtil::construct(
                                    d_objectBuffer.address(),
                                    allocator,
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
                      const CtorAllocArgT&                      allocator)
{
    bslma::ConstructionUtil::construct(
                                    d_objectBuffer.address(),
                                    allocator,
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
                      const CtorAllocArgT&                      allocator)
{
    bslma::ConstructionUtil::construct(
                                    d_objectBuffer.address(),
                                    allocator,
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
                      const CtorAllocArgT&                      allocator)
{
    bslma::ConstructionUtil::construct(
                                    d_objectBuffer.address(),
                                    allocator,
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
                      const CtorAllocArgT&                      allocator)
{
    bslma::ConstructionUtil::construct(
                                    d_objectBuffer.address(),
                                    allocator,
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
                      const CtorAllocArgT&                      allocator)
{
    bslma::ConstructionUtil::construct(
                                    d_objectBuffer.address(),
                                    allocator,
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
                      const CtorAllocArgT&                      allocator)
{
    bslma::ConstructionUtil::construct(
                                    d_objectBuffer.address(),
                                    allocator,
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
