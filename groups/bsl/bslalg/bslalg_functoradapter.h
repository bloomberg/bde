// bslalg_functoradapter.h                                            -*-C++-*-
#ifndef INCLUDED_BSLALG_FUNCTORADAPTER
#define INCLUDED_BSLALG_FUNCTORADAPTER

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide an utility that adapts callable objects to functors.
//
//@CLASSES:
//   bslalg::FunctorAdapter: utility for using callable objects as functors
//
//@SEE_ALSO: bslstl_setcomparator, bslstl_mapcomparator
//
//@DESCRIPTION: This component provides a single utility template,
// 'FunctorAdapter', that adapts a parameterized adaptee type, which can be any
// callable object type, to a target functor type.  This adaptation enables a
// client to inherit from the target functor type even if the adaptee callable
// object type is a function pointer type.  This is particularly useful if the
// client of the callable object type wants to take advantage of the empty-base
// optimization to avoid paying storage cost when the callable object type is a
// functor type with no data members.
//
// 'FunctorAdapter' defines an alias to the target functor type.  If the
// adaptee type is a functor type, the target type is an alias to the adaptee
// type.  If the adaptee type is a function pointer type, the target type is a
// functor type that delegates to a function referred to by a function pointer
// of the adaptee type.
//
///Usage
///-----
// This section illustrates the intended use of this component.
//
///Example 1: Using function pointer base for an empty-base optimized class
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose that we wanted to define a binder that binds a binary predicate of a
// parameterized type to a value passed on construction.  Also suppose that we
// wanted to use the empty-base optimization to avoid paying storage cost when
// the predicate type is a functor type with no data members.  Unfortunately,
// the binary predicate type may be a function pointer type, which cannot serve
// as a base class.  The solution is to have the binder inherit from
// 'FunctorAdapter::Type', which adapts a function pointer type to a functor
// type that is a suitable base class.
//
// First, we define the class 'Bind2ndInteger', which inherits from
// 'FunctorAdapter::Type' to take advantage of the empty-base optimization:
//..
//  template <class BINARY_PREDICATE>
//  class Bind2ndInteger : private FunctorAdapter<BINARY_PREDICATE>::Type {
//      // This class provides a functor that delegate its function-call
//      // operator to the parameterized 'BINARY_PREDICATE', passing the user
//      // supplied parameter as the first argument and the integer value
//      // passed on construction as the second argument.
//
//      // DATA
//      int d_bondValue;  // the bound value
//
//      // NOT IMPLEMENTED
//      Bind2ndInteger(const Bind2ndInteger&);
//      Bind2ndInteger& operator=(const Bind2ndInteger&);
//
//    public:
//      // CREATORS
//      Bind2ndInteger(int value, const BINARY_PREDICATE& predicate);
//          // Create a 'Bind2ndInteger' object that will bind the second
//          // parameter of the specified 'predicate' with the specified
//          // integer 'value'.
//
//      // ~Bind2ndInteger() = default;
//          // Destroy this object.
//
//      // ACCESSORS
//      bool operator() (const int value) const;
//          // Return the result of calling the parameterized
//          // 'BINARY_PREDICATE' passing the specified 'value' as the first
//          // argument and the integer value passed on construction as the
//          // second argument.
//  };
//..
//  Then, we implement the methods of the 'Bind2ndInteger' class:
//..
//  template <class BINARY_PREDICATE>
//  Bind2ndInteger<BINARY_PREDICATE>::Bind2ndInteger(int value,
//                                          const BINARY_PREDICATE& predicate)
//  : FunctorAdapter<BINARY_PREDICATE>::Type(predicate), d_bondValue(value)
//  {
//  }
//..
// Here, we implement the 'operator()' member function that simply delegates to
// 'BINARY_PREDICATE'
//..
//  template <class BINARY_PREDICATE>
//  bool Bind2ndInteger<BINARY_PREDICATE>::operator() (const int value) const
//  {
//      const BINARY_PREDICATE& predicate = *this;
//      return predicate(value, d_bondValue);
//  }
//..
// Next, we define a function, 'intCompareFunction', that compares two
// integers:
//..
//  bool intCompareFunction(const int lhs, const int rhs)
//  {
//      return lhs < rhs;
//  }
//..
// Now, we define a 'Bind2ndInteger' object 'functorLessThan10' using the
// 'std::less<int>' functor as the parameterized 'BINARY_PREDICATE' and invoke
// the function call operator:
//..
//  Bind2ndInteger<native_std::less<int> > functorLessThan10(10,
//                                                    native_std::less<int>());
//
//  assert(functorLessThan10(1));
//  assert(!functorLessThan10(12));
//..
// Finally, we define a 'Bind2ndInteger' object 'functionLessThan10' passing
// the address of 'intCompareFunction' on construction and invoke the function
// call operator:
//..
//  Bind2ndInteger<bool (*)(const int, const int)>
//                                 functionLessThan10(10, &intCompareFunction);
//
//  assert(functionLessThan10(1));
//  assert(!functionLessThan10(12));
//..

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLMF_ASSERT
#include <bslmf_assert.h>
#endif

#ifndef INCLUDED_BSLMF_FUNCTIONPOINTERTRAITS
#include <bslmf_functionpointertraits.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

namespace BloombergLP {
namespace bslalg {

                    // ====================================
                    // class FunctorAdapter_FunctionPointer
                    // ====================================

template <class FUNCTION_POINTER>
class FunctorAdapter_FunctionPointer {
    // This class provides a functor that delegates to the function referred to
    // by a function pointer supplied on construction.  Delegation is supported
    // through the conversion operator, which implicitly returns a reference to
    // the parameterized 'FUNCTION_POINTER'.

  private:
    // DATA
    FUNCTION_POINTER d_function_p;  // the pointer to the function

  public:
    // CREATORS
    explicit FunctorAdapter_FunctionPointer(FUNCTION_POINTER functionPtr);
        // Create a 'FunctorAdapter_FunctionPointer' object that will delegate
        // to the function referred to by the specified 'functionPtr'.

    // MANIPULATORS
    operator FUNCTION_POINTER& ();
        // Convert this object to the parameterized 'FUNCTION_POINTER' by
        // returning the function pointer supplied on construction.

    // ACCESSORS
    operator const FUNCTION_POINTER& () const;
        // Convert this object to the parameterized 'FUNCTION_POINTER' by
        // returning the function pointer supplied on construction.
};

                    // ====================
                    // class FunctorAdapter
                    // ====================

template <class CALLABLE_OBJECT>
class FunctorAdapter {
    // This class provides a metafunction that defines an alias 'Type' for the
    // parameterized 'CALLABLE_OBJECT'.  'Type' is functor type that provides
    // the same operation as the parameterized 'CALLABLE_OBJECT'.  Note that
    // function pointers are supported through a specialization of this
    // template.

  public:
    // PUBLIC TYPES
    typedef CALLABLE_OBJECT Type;
        // This 'typedef' is an alias for the functor.
};

                    // ====================
                    // class FunctorAdapter
                    // ====================

template <class FUNCTION>
class FunctorAdapter<FUNCTION*> {
    // This specialization of 'FunctorAdapter' defines an alias 'Type' for a
    // functor that delegates to a function pointer matching the parameterized
    // 'FUNCTION' type.

    BSLMF_ASSERT(bslmf::IsFunctionPointer<FUNCTION*>::value);
        // This 'BSLMF_ASSERT' statement ensures that the parameter 'FUNCTION'
        // must be a function pointer.

  public:
    // PUBLIC TYPES
    typedef FunctorAdapter_FunctionPointer<FUNCTION*> Type;
        // This 'typedef' is an alias for a functor that delegates to the
        // function referred to by the function pointer matching the
        // parameterized 'FUNCTION' type.
};


// ============================================================================
//                  TEMPLATE AND INLINE FUNCTION DEFINITIONS
// ============================================================================

                    // ------------------------------------
                    // class FunctorAdapter_FunctionPointer
                    // ------------------------------------

// CREATORS
template <class FUNCTION_POINTER>
inline
FunctorAdapter_FunctionPointer<FUNCTION_POINTER>
::FunctorAdapter_FunctionPointer(FUNCTION_POINTER functionPtr)
:d_function_p(functionPtr)
{
}

// MANIPULATORS
template <class FUNCTION_POINTER>
inline
FunctorAdapter_FunctionPointer<FUNCTION_POINTER>
::operator FUNCTION_POINTER& ()
{
    return d_function_p;
}

// ACCESSORS
template <class FUNCTION_POINTER>
inline
FunctorAdapter_FunctionPointer<FUNCTION_POINTER>
::operator const FUNCTION_POINTER& () const
{
    return d_function_p;
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
