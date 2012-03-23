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
//@AUTHOR: Alisdair Meredith (ameredit)
//
//@DESCRIPTION: This component provides a limited emulation of the C++11 type,
// 'std::nullptr_t', which can be used as a function parameter type to help
// overload sets treat null pointer literals specially.  Note that this
// component will be deprecated, and ultimately removed, once BDE code can
// assume support for a C++11 compiler.  On a platform that supports the
// language feature, a fully-conforming 'typedef' is supplied rather than
// using the emulation layer.
//
///Limitations
//------------
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
//
//..
//  template<class TARGET_TYPE>
//  class ScopedPointer {
//    private:
//      typedef void DeleterFn(TARGET_TYPE *);
//
//      T         *d_target_p;
//      DeleterFn *d_deleter_fn;
//
//      // Objects of this type cannot be copied.
//      ScopedPointer(const ScopedPointer&);
//      ScopedPointer& operator=(const ScopedPointer&);
//
//      template<SOURCE_TYPE>
//      static void defaultDeleteFn(TARGET_TYPE *ptr)
//      {
//          delete static_cast<SOURCE_TYPE *>(ptr);
//      }
//      
//    public:
//      template<class SOURCE_TYPE>
//      ScopedPointer(SOURCE_TYPE *ptr,
//                    DeleterFn fn = &DefaultDeleteFn<SOURCE_TYPE>)
//      : d_target_p(ptr)
//      , d_deleter_fn(deleter)
//      {
//      }
//
//      ScopedPointer(bsl::nullptr_t = 0)
//      : d_target_p(0)
//      , d_deleter_fn(0)
//      {
//      }
//
//      ~ScopedPointer()
//      {
//          if(d_deleter_fn) {
//              d_deleter_fn(d_target_t);
//          }
//      }
//  };
//..

#ifndef INCLUDED_BSLS_COMPILERFEATURES
#include <bsls_compilerfeatures.h>
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
    typedef decltype(nullptr) nullptr_t;
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

}  // close namespace bsls
}  // close namespace BloombergLP

namespace bsl
{
    typedef BloombergLP::bsls::Nullptr_Impl::Type nullptr_t;
        //  Alias for a type that can match a null pointer literal, but is not
        //  a pointer type itself.
}
#endif

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2012
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
