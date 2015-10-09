// bsls_nullptr.h                                                     -*-C++-*-
#ifndef INCLUDED_BSLS_NULLPTR
#define INCLUDED_BSLS_NULLPTR

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a distinct type for null pointer literals.
//
//@CLASSES:
//   bsls::Nullptr: namespace for a type matching only null pointer literals
//
//@DESCRIPTION: This component provides a limited emulation of the C++11 type,
// 'std::nullptr_t', which can be used as a function parameter type to create
// an overload set where null pointer literals are handled specially.  Note
// that this component will be deprecated, and ultimately removed, once BDE
// code can assume support for a C++11 compiler.  On a platform that supports
// the language feature, a fully-conforming 'typedef' is supplied rather than
// using the emulation layer.
//
///Limitations
///-----------
// This component provides a simple emulation of the C++11 facility, which
// cannot be expressed with a pure library solution.  As such it comes with a
// number of limitations.  The most obvious is that C++11 provides a new
// null pointer literal, 'nullptr', which is not emulated by this component.
// The new null pointer literal is an object of a new type, expressed by the
// alias 'nullptr_t', which this component emulates.  However, as this is a
// library-only emulation, it does not have any preference in the overloading
// rules, so will be an equal-rank ambiguous match.  For example, given the
// following overload set, a call to 'myFunction' with a null pointer literal
// would be ambiguous:
//..
//  void myFunction(void *p);
//  void myFunction(bsl::nullptr_t);
//
//  int main() {
//     myFunction(0);  // ERROR, ambiguous function call
//  }
//..
// However, if the pointer-argument is a pointer whose type is deduced from the
// function call, then no pointer type can be deduced from the null pointer and
// this component becomes necessary.
//..
//  template<typename T>
//  void myFunction(T *p);
//  void myFunction(bsl::nullptr_t);
//
//  int main() {
//     myFunction(0);  // call the 'bsl::nullptr_t' method
//  }
//..
// Null pointer values can be created in C++11 by creating objects of type
// 'std::nullptr_t', and then used to initialize pointer and pointer-to-member
// objects:
//..
//  std::nullptr_t nullLiteral = std::nullptr_t();
//  int *pI = nullLiteral;
//..
// The type of a 'bsl::nullptr_t' object cannot be used in such assignments
// or initializations, unless compiled on a platform that natively supports
// this C++11 language feature.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
// Example 1: Constructing a "smart pointer"
// - - - - - - - - - - - - - - - - - - - - -
// First we define a smart pointer class template, as a guard to destroy a
// managed object as the smart pointer leaves scope.  This class will have a
// constructor template taking a pointer to a type potentially derived from
// the parameterized type of the smart pointer, and also a deletion-policy
// function.  By capturing the most-derived type through type-deduction when
// the smart pointer is constructed, we can ensure the correct destructor is
// called, even if the destructor of the base class has not been declared as
// 'virtual'.  However, relying on type-deduction means we cannot pass a null
// pointer to this constructor, as it is not possible to deduce what type a
// null pointer is supposed to refer to, therefore we must use a special null
// pointer type, such as 'bsls::nullptr_t'.  Note that in real code we would
// allocate and reclaim memory using a user-specified allocator, but defining
// such protocols in this low level component would further distract from the
// 'nullptr' usage in this example.
//..
//  template<class TARGET_TYPE>
//  class ScopedPointer {
//      // This class template is a guard to manage a dynamically created
//      // object of the parameterized 'TARGET_TYPE'.
//
//    private:
//      typedef void DeleterFn(TARGET_TYPE *);  // deleter type
//
//      // DATA
//      TARGET_TYPE *d_target_p;    // wrapped pointer
//      DeleterFn   *d_deleter_fn;  // deleter function
//
//      template<class SOURCE_TYPE>
//      static void defaultDeleteFn(TARGET_TYPE *ptr);
//          // Destroy the specified '*ptr' by calling 'delete' on the pointer
//          // cast to the parameterized 'SOURCE_TYPE*'.  It is an error to
//          // instantiate this template with a 'SOURCE_TYPE' that is not
//          // derived from (and cv-compatible with) 'TARGET_TYPE'.
//
//     private:
//      // NOT IMPLEMENTED
//      ScopedPointer(const ScopedPointer&);
//      ScopedPointer& operator=(const ScopedPointer&);
//          // Objects of this type cannot be copied.
//
//    public:
//      template<class SOURCE_TYPE>
//      ScopedPointer(SOURCE_TYPE *pointer,
//                    DeleterFn   *fn = &defaultDeleteFn<SOURCE_TYPE>);
//          // Create a 'ScopedPointer' object owning the specified 'pointer'
//          // and using the specified 'fn' to destroy the owned pointer when
//          // this object is destroyed.
//
//      ScopedPointer(bsl::nullptr_t = 0);
//          // Create an empty 'ScopedPointer' object that does not own a
//          // pointer.
//
//      ~ScopedPointer();
//          // Destroy this 'ScopedPointer' object and the target object
//          // that it owns, using the stored deleter function.
//
//      // Further methods appropriate to a smart pointer, such as
//      // 'operator*' and 'operator->' elided from this example.
//  };
//..
// Then we provide a definition for each of the methods.
//..
//  template<class TARGET_TYPE>
//  template<class SOURCE_TYPE>
//  void ScopedPointer<TARGET_TYPE>::defaultDeleteFn(TARGET_TYPE *ptr)
//  {
//      delete static_cast<SOURCE_TYPE *>(ptr);
//  }
//
//  template<class TARGET_TYPE>
//  template<class SOURCE_TYPE>
//  inline
//  ScopedPointer<TARGET_TYPE>::ScopedPointer(SOURCE_TYPE *pointer,
//                                            DeleterFn   *fn)
//  : d_target_p(pointer)
//  , d_deleter_fn(fn)
//  {
//  }
//
//  template<class TARGET_TYPE>
//  inline
//  ScopedPointer<TARGET_TYPE>::ScopedPointer(bsl::nullptr_t)
//  : d_target_p(0)
//  , d_deleter_fn(0)
//  {
//  }
//
//  template<class TARGET_TYPE>
//  inline
//  ScopedPointer<TARGET_TYPE>::~ScopedPointer()
//  {
//      if (d_deleter_fn) {
//          d_deleter_fn(d_target_p);
//      }
//  }
//..
// Finally, we can construct a 'ScopedPointer' with a null pointer literal,
// that would otherwise be non-deducible, using our 'bsl::nullptr_t' overload.
//..
//  void testScopedPointer()
//  {
//      ScopedPointer<int> x(0);
//  }
//..

#ifndef INCLUDED_BSLS_COMPILERFEATURES
#include <bsls_compilerfeatures.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORM
#include <bsls_platform.h>
#endif

#if defined(BSLS_COMPILERFEATURES_SUPPORT_NULLPTR)
#  if !defined(BSLS_COMPILERFEATURES_SUPPORT_DECLTYPE)
    // We currently know of no platform that supports 'nullptr' and does not
    // also support 'decltype'.  We conservatively error should such a
    // surprising platform emerge.
#  error No support for 'std::nullptr_t' unless 'decltype' is also available.
#  else
#  define BSLS_NULLPTR_USING_NATIVE_NULLPTR_T  // feature detection macro
namespace bsl
{
    // We must define this 'typedef' appropriately for platforms that support
    // 'nullptr' to avoid accidental clashes in 'BSL_OVERRIDES_STD' mode.

#if defined(BSLS_PLATFORM_CMP_MSVC) && defined(__cplusplus_cli)
    // MSVC in /clr mode defines 'nullptr' as the .NET null pointer type, which
    // is different from C++11 'nullptr'.  To resolve this conflict MSVC
    // provides '__nullptr' for C++11 'nullptr'.

    typedef decltype(__nullptr) nullptr_t;
#else
    typedef decltype(nullptr) nullptr_t;
#endif
}
#  endif
#else
namespace BloombergLP {

namespace bsls
{
                       // ===================
                       // class bsls::Nullptr
                       // ===================

struct Nullptr_Impl {
    // This implementation-private 'struct' provides an alias for a type that
    // can match a null pointer literal, but is not a pointer itself.  It
    // offers a limited emulation of the C++11 'std::nullptr_t' type.

  private:
    struct Nullptr_ProxyType { int dummy; }; // private class to supply a
                                             // unique pointer-to-member type.

  public:
    typedef int Nullptr_ProxyType::* Type;   // alias to an "unspecified" null
                                             // pointer type.
};

}  // close package namespace
}  // close enterprise namespace

namespace bsl
{
    typedef BloombergLP::bsls::Nullptr_Impl::Type nullptr_t;
        // Alias for a type that can match a null pointer literal, but is not a
        // pointer type itself.
}  // close namespace bsl
#endif

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
