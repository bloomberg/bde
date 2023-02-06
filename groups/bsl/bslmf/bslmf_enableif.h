// bslmf_enableif.h                                                   -*-C++-*-
#ifndef INCLUDED_BSLMF_ENABLEIF
#define INCLUDED_BSLMF_ENABLEIF

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a utility to set up SFINAE conditions in type deduction.
//
//@CLASSES:
//  bsl::enable_if: standard meta-function to drop templates from overload sets
//  bsl::enable_if_t: alias to the return type of the meta-function
//  bslmf::EnableIf: meta-function to drop templates from overload sets
//
//@DESCRIPTION: This component defines two meta-functions, 'bsl::enable_if' and
// 'bslmf::EnableIf', both of which may be used to conditionally remove
// (potential) template instantiations as candidates for overload resolution by
// causing a deduced template instantiation to fail in a way compatible with
// the C++ SFINAE rules.
//
// 'bsl::enable_if' meets the requirements of the 'enable_if' template defined
// in the C++11 standard [meta.trans.ptr], while 'bslmf::EnableIf' was devised
// before 'enable_if' was standardized.
//
// The two meta-functions provide identical functionality.  Both meta-functions
// provide a 'typedef' 'type' that is an alias to a (template parameter) type
// if a (template parameter) condition is 'true'; otherwise, 'type' is not
// provided.
//
// Note that 'bsl::enable_if' should be preferred over 'bslmf::EnableIf', and
// in general, should be used by new components.
//
///Visual Studio Workaround
///------------------------
// Because of a Visual Studio bug, described here:
//   http://connect.microsoft.com/VisualStudio/feedback/details/332179/
// the Microsoft Visual Studio compiler may not correctly associate a function
// declaration that uses 'bsl::enable_if' with that function's definition, if
// the definition is not inline to the declaration.  This bug affects at least
// Visual Studio 2008 and 2010.  The workaround is to implement functions using
// 'bsl::enable_if' inline with their declaration.
//
///Usage
///-----
// The following snippets of code illustrate basic use of the 'bsl::enable_if'
// meta-function.  We will demonstrate how to use this utility to control
// overload sets with three increasingly complex examples.
//
///Example 1: Implementing a Simple Function with 'bsl::enable_if'
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose that we want to implement a simple 'swap' function template to
// exchange two arbitrary values, as if defined below:
//..
//  template<class t_TYPE>
//  void DummySwap(t_TYPE& a, t_TYPE& b)
//      // Exchange the values of the specified objects, 'a' and 'b'.
//  {
//      t_TYPE temp(a);
//      a = b;
//      b = temp;
//  }
//..
// However, we want to take advantage of member-swap methods supplied by user-
// defined types, so we define a trait that can be customized by a class
// implementer to indicate that their class supports an optimized member-swap
// method:
//..
//  template<class t_TYPE>
//  struct HasMemberSwap : bsl::false_type {
//      // This traits class indicates whether the (template parameter)
//      // 't_TYPE' has a public 'swap' method to exchange values.
//  };
//..
// Now, we implement a generic 'swap' function template that will invoke the
// member swap operation for any type that specialized our trait.  The use of
// 'bsl::enable_if' to declare the result type causes an attempt to deduce the
// type 't_TYPE' to fail unless the specified condition is 'true', and this
// falls under the "Substitution Failure Is Not An Error" (SFINAE) clause of
// the C++ standard, so the compiler will look for a more suitable overload
// rather than fail with an error.  Note that we provide two overloaded
// declarations that appear to differ only in their return type, which would
// normally raise an ambiguity error.  This works, and is in fact required, in
// this case as the "enable-if" conditions are mutually exclusive, so that only
// one overload will ever be present in an overload set.  Also note that the
// 'type' 'typedef' of 'bsl::enable_if' is an alias to 'void' when the
// (template parameter) type is unspecified and the (template parameter)
// condition value is 'true'.
//..
//  template<class t_TYPE>
//  typename bsl::enable_if<HasMemberSwap<t_TYPE>::value>::type
//  swap(t_TYPE& a, t_TYPE& b)
//  {
//      a.swap(b);
//  }
//
//  template<class t_TYPE>
//  typename bsl::enable_if< ! HasMemberSwap<t_TYPE>::value>::type
//  swap(t_TYPE& a, t_TYPE& b)
//  {
//      t_TYPE temp(a);
//      a = b;
//      b = temp;
//  }
//..
// Next, we define a simple container template, that supports an optimized
// 'swap' operation by merely swapping the internal pointer to the array of
// elements rather than exchanging each element:
//..
//  template<class t_TYPE>
//  class MyContainer {
//      // This is a simple container implementation for demonstration purposes
//      // that is modeled after 'std::vector'.
//
//      // DATA
//      t_TYPE *d_storage;
//      size_t d_length;
//
//      // Copy operations are declared private and not defined.
//
//      // NOT IMPLEMENTED
//      MyContainer(const MyContainer&);
//      MyContainer& operator=(const MyContainer&);
//
//    public:
//      MyContainer(const t_TYPE& value, int n);
//          // Create a 'MyContainer' object having the specified 'n' copies of
//          // the specified 'value'.  The behavior is undefined unless
//          // '0 <= n'.
//
//      ~MyContainer();
//          // Destroy this container and all of its elements, reclaiming any
//          // allocated memory.
//
//      // MANIPULATORS
//      void swap(MyContainer &other);
//          // Exchange the contents of 'this' container with those of the
//          // specified 'other'.  No memory will be allocated, and no
//          // exceptions are thrown.
//
//      // ACCESSORS
//      const t_TYPE& front() const;
//          // Return a reference providing non-modifiable access to the first
//          // element in this container.  The behavior is undefined if this
//          // container is empty.
//
//      size_t size() const;
//          // Return the number of elements held by this container.
//  };
//..
// Then, we specialize our 'HasMemberSwap' trait for this new container type.
//..
//  template<class t_TYPE>
//  struct HasMemberSwap<MyContainer<t_TYPE> > : bsl::true_type {
//  };
//..
// Next, we implement the methods of this class:
//..
//  // CREATORS
//  template<class t_TYPE>
//  MyContainer<t_TYPE>::MyContainer(const t_TYPE& value, int n)
//  : d_storage(new t_TYPE[n])
//  , d_length(n)
//  {
//      for (int i = 0; i != n; ++i) {
//          d_storage[i] = value;
//      }
//  }
//
//  template<class t_TYPE>
//  MyContainer<t_TYPE>::~MyContainer()
//  {
//      delete[] d_storage;
//  }
//
//  // MANIPULATORS
//  template<class t_TYPE>
//  void MyContainer<t_TYPE>::swap(MyContainer& other)
//  {
//      ::swap(d_storage, other.d_storage);
//      ::swap(d_length,  other.d_length);
//  }
//
//  // ACCESSORS
//  template<class t_TYPE>
//  const t_TYPE& MyContainer<t_TYPE>::front() const
//  {
//      return d_storage[0];
//  }
//
//  template<class t_TYPE>
//  size_t MyContainer<t_TYPE>::size() const
//  {
//      return d_length;
//  }
//..
// Finally, we can test that the member-'swap' method is called by the generic
// 'swap' function.  Note that the following code will not compile unless the
// member-function 'swap' is used, as the copy constructor and assignment
// operator for the 'MyContainer' class template are declared as 'private'.
//..
//  void TestSwap()
//  {
//      MyContainer<int> x(3, 14);
//      MyContainer<int> y(2, 78);
//      assert(14 == x.size());
//      assert( 3 == x.front());
//      assert(78 == y.size());
//      assert( 2 == y.front());
//
//      swap(x, y);
//
//      assert(78 == x.size());
//      assert( 2 == x.front());
//      assert(14 == y.size());
//      assert( 3 == y.front());
// }
//..
//
///Example 2: Using the 'bsl::enable_if' Result Type
///- - - - - - - - - - - - - - - - - - - - - - - - -
// For the next example, we will demonstrate the use of the second template
// parameter in the 'bsl::enable_if' template, which serves as the "result"
// type if the test condition passes.  Suppose that we want to write a generic
// function to allow us to cast between pointers of different types.  If the
// types are polymorphic, we can use 'dynamic_cast' to potentially cast between
// two seemingly unrelated types.  However, if either type is not polymorphic
// then the attempt to use 'dynamic_cast' would be a compile-time failure, and
// we must use 'static_cast' instead.
//..
//#ifdef BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES
//..
// Note that if the current compiler supports alias templates C++11 feature, we
// can use 'bsl::enable_if_t' alias to the "result" type of 'bsl::enable_if'
// meta-function, that avoids the '::type' suffix and 'typename' prefix in the
// declaration of the function return type:
//..
//  template<class TO, class FROM>
//  typename bsl::enable_if<bsl::is_polymorphic<FROM>::value &&
//                                              bsl::is_polymorphic<TO>::value,
//                          TO>::type *
//#else
//  template<class TO, class FROM>
//  bsl::enable_if_t<bsl::is_polymorphic<FROM>::value &&
//                      bsl::is_polymorphic<TO  >::value, TO> *
//#endif
//  smart_cast(FROM *from)
//      // Return a pointer to the specified 'TO' type if the specified 'from'
//      // pointer refers to an object whose complete class publicly derives,
//      // directly or indirectly, from 'TO', and a null pointer otherwise.
//  {
//      return dynamic_cast<TO *>(from);
//  }
//
//#ifdef BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES
//  template<class TO, class FROM>
//  bsl::enable_if_t<not(bsl::is_polymorphic<FROM>::value &&
//                       bsl::is_polymorphic<TO  >::value), TO> *
//#else
//  template<class TO, class FROM>
//  typename bsl::enable_if<not(bsl::is_polymorphic<FROM>::value &&
//                                            bsl::is_polymorphic<TO>::value),
//                          TO>::type *
//#endif
//  smart_cast(FROM *from)
//      // Return the specified 'from' pointer value cast as a pointer to type
//      // 'TO'.  The behavior is undefined unless such a conversion is valid.
//  {
//      return static_cast<TO *>(from);
//  }
//..
// Next, we define a small number of classes to demonstrate that this casting
// utility works correctly:
//..
//  class A {
//      // Sample non-polymorphic type
//
//    public:
//      ~A() {}
//  };
//
//  class B {
//      // Sample polymorphic base-type
//
//    public:
//      virtual ~B() {}
//  };
//
//  class C {
//      // Sample polymorphic base-type
//
//    public:
//      virtual ~C() {}
//  };
//
//  class ABC : public A, public B, public C {
//      // Most-derived example class using multiple bases in order to
//      // demonstrate cross-casting.
//  };
//..
// Finally, we demonstrate the correct behavior of the 'smart_cast' utility:
//..
//  void TestSmartCast()
//  {
//      ABC object;
//      ABC *pABC = &object;
//      A   *pA   = &object;
//      B   *pB   = &object;
//      C   *pC   = &object;
//
//      A *pA2 = smart_cast<A>(pABC);
//      B *pB2 = smart_cast<B>(pC);
//      C *pC2 = smart_cast<C>(pB);
//
//      (void) pA;
//
//      assert(&object == pA2);
//      assert(&object == pB2);
//      assert(&object == pC2);
//
//      // These lines would fail to compile
//      // A *pA3 = smart_cast<A>(pB);
//      // C *pC3 = smart_cast<C>(pA);
//  }
//..
//
///Example 3: Controlling Constructor Selection with 'bsl::enable_if'
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// The final example demonstrates controlling the selection of a constructor
// template in a class with (potentially) many constructors.  We define a
// simple container template based on 'std::vector' that illustrates a problem
// that may occur when trying to call the constructor the user expects.  For
// this example, assume we are trying to create a 'vector<int>' with '42'
// copies of the value '13'.  When we pass the literal values '42' and '13' to
// the compiler, the "best" candidate constructor should be the template
// constructor that takes two arguments of the same kind, deducing that type to
// be 'int'.  Unfortunately, that constructor expects those values to be of an
// iterator type, forming a valid range.  We need to avoid calling this
// constructor unless the deduced type really is an iterator, otherwise a
// compile-error will occur trying to instantiate that constructor with an
// incompatible argument type.  We use 'bsl::enable_if' to create a deduction
// context where SFINAE can kick in.  Note that we cannot deduce the '::type'
// result of a meta-function, and there is no result type (as with a regular
// function) to decorate, so we add an extra dummy argument using a pointer
// type (produced from 'bsl::enable_if::type') with a default null argument:
//..
//  template<class t_TYPE>
//  class MyVector {
//      // This is a simple container implementation for demonstration purposes
//      // that is modeled after 'std::vector'.
//
//      // DATA
//      t_TYPE *d_storage;
//      size_t  d_length;
//
//      // NOT IMPLEMENTED
//      MyVector(const MyVector&);
//      MyVector& operator=(const MyVector&);
//
//    public:
//      // CREATORS
//      MyVector(const t_TYPE& value, int n);
//          // Create a 'MyVector' object having the specified 'n' copies of
//          // the specified 'value'.  The behavior is undefined unless
//          // '0 <= n'.
//
//      template<class FORWARD_ITERATOR>
//      MyVector(FORWARD_ITERATOR first, FORWARD_ITERATOR last,
//                  typename bsl::enable_if<
//                      !bsl::is_fundamental<FORWARD_ITERATOR>::value
//                                                               >::type * = 0)
//          // Create a 'MyVector' object having the same sequence of values as
//          // found in the range described by the specified iterators
//          // '[first, last)'.  The behavior is undefined unless 'first' and
//          // 'last' refer to a sequence of values of the (template parameter)
//          // type 't_TYPE' where 'first' is at a position at or before
//          // 'last'.  Note that this function is currently defined inline to
//          // work around an issue with the Microsoft Visual Studio compiler.
//
//      {
//          d_length = 0;
//          for (FORWARD_ITERATOR cursor = first; cursor != last; ++cursor) {
//               ++d_length;
//          }
//
//          d_storage = new t_TYPE[d_length];
//          for (size_t i = 0; i != d_length; ++i) {
//               d_storage[i] = *first;
//               ++first;
//          }
//      }
//
//      ~MyVector();
//          // Destroy this container and all of its elements, reclaiming any
//          // allocated memory.
//
//      // ACCESSORS
//      const t_TYPE& operator[](int index) const;
//          // Return a reference providing non-modifiable access to the
//          // element held by this container at the specified 'index'.  The
//          // behavior is undefined unless 'index < size()'.
//
//      size_t size() const;
//          // Return the number of elements held by this container.
//  };
//..
// Note that there is no easy test for whether a type is an iterator, so we
// assume that any attempt to call a constructor with two arguments that are
// not fundamental (such as 'int') must be passing iterators.  Now that we have
// defined the class template, we implement its methods:
//..
//  template<class t_TYPE>
//  MyVector<t_TYPE>::MyVector(const t_TYPE& value, int n)
//  : d_storage(new t_TYPE[n])
//  , d_length(n)
//  {
//      for (int i = 0; i != n; ++i) {
//          d_storage[i] = value;
//      }
//  }
//
//  template<class t_TYPE>
//  MyVector<t_TYPE>::~MyVector()
//  {
//      delete[] d_storage;
//  }
//
//  // ACCESSORS
//  template<class t_TYPE>
//  const t_TYPE& MyVector<t_TYPE>::operator[](int index) const
//  {
//      return d_storage[index];
//  }
//
//  template<class t_TYPE>
//  size_t MyVector<t_TYPE>::size() const
//  {
//      return d_length;
//  }
//..
// Finally, we demonstrate that the correct constructors are called when
// invoked with appropriate arguments:
//..
//  void TestContainerConstructor()
//  {
//      const unsigned int TEST_DATA[] = { 1, 2, 3, 4, 5 };
//
//      const MyVector<unsigned int> x(&TEST_DATA[0], &TEST_DATA[5]);
//      const MyVector<unsigned int> y(13, 42);
//
//      assert(5 == x.size());
//      for (int i = 0; i != 5; ++i) {
//          assert(TEST_DATA[i] == x[i]);
//      }
//
//      assert(42 == y.size());
//      for (int i = 0; i != 42; ++i) {
//          assert(13 == y[i]);
//      }
//  }
//..

#include <bslscm_version.h>

#include <bsls_compilerfeatures.h>

namespace bsl {

                         // ================
                         // struct enable_if
                         // ================

template <bool t_COND, class t_TYPE = void>
struct enable_if {
    // This 'struct' template implements the 'enable_if' meta-function defined
    // in the C++11 standard [meta.trans.ptr].  This 'struct' template provides
    // a 'typedef' 'type' that is an alias to the (template parameter) 't_TYPE'
    // if the (template parameter) 't_COND' is 'true'; otherwise, 'type' is not
    // provided.  If 't_TYPE' is not specified, it is set to 'void'.  Note that
    // this generic default template provides 'type' for when 't_COND' is
    // 'true'; a template specialization is provided (below) that omits 'type'
    // for when 't_COND' is 'false'.

    typedef t_TYPE type;
        // This 'typedef' is an alias to the (template parameter) 't_TYPE'.
};

                      // ===============================
                      // struct enable_if<false, t_TYPE>
                      // ===============================

template <class t_TYPE>
struct enable_if<false, t_TYPE> {
    // This partial specialization of 'enable_if', for when the (template
    // parameter) 't_COND' is 'false', guarantees that no 'typedef' 'type' is
    // supplied.  Note that this class definition is intentionally empty.
};

#ifdef BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES

// ALIASES
template <bool t_COND, class t_TYPE = void>
using enable_if_t = typename enable_if<t_COND, t_TYPE>::type;
    // 'enable_if_t' is an alias to the return type of the 'bsl::enable_if'
    // meta-function.  Note, that the 'enable_if_t' avoids the '::type' suffix
    // and 'typename' prefix when we want to use the result of the
    // meta-function in templates.

#endif  // BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES

}  // close namespace bsl

namespace BloombergLP {

namespace bslmf {

                               // ===============
                               // struct EnableIf
                               // ===============

template <bool t_COND, class t_TYPE = void>
struct EnableIf {
    // This 'struct' template implements a meta-function that provides a
    // 'typedef' 'type' that is an alias to the (template parameter) 't_TYPE'
    // if the (template parameter) 't_COND' is 'true'; otherwise, 'type' is not
    // provided.  If 't_TYPE' is not specified, it is set to 'void'.  Note that
    // this generic default template provides 'type' for when 't_COND' is
    // 'true'; a template specialization is provided (below) that omits 'type'
    // for when 't_COND' is 'false'.
    //
    // Also note that although this 'struct' is functionally identical to
    // 'bsl::enable_if', the use of 'bsl::enable_if' should be preferred.

    typedef t_TYPE type;
        // This 'typedef' is an alias to the (template parameter) 't_TYPE'.
};

template <class t_TYPE>
struct EnableIf<false, t_TYPE> {
    // This partial specialization of 'EnableIf', for when the (template
    // parameter) 't_COND' is 'false', guarantees that no 'typedef' 'type' is
    // supplied.  Note that this class definition is intentionally empty.
};

}  // close package namespace
}  // close enterprise namespace

#ifndef BDE_OPENSOURCE_PUBLICATION  // BACKWARD_COMPATIBILITY
// ============================================================================
//                           BACKWARD COMPATIBILITY
// ============================================================================

#ifdef bslmf_EnableIf
#undef bslmf_EnableIf
#endif
#define bslmf_EnableIf bslmf::EnableIf
    // This alias is defined for backward compatibility.
#endif  // BDE_OPENSOURCE_PUBLICATION -- BACKWARD_COMPATIBILITY

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
