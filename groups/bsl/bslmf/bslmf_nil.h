// bslmf_nil.h                                                        -*-C++-*-
#ifndef INCLUDED_BSLMF_NIL
#define INCLUDED_BSLMF_NIL

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a nil type.
//
//@CLASSES:
//  bslmf::Nil: class representing a nil (non-existent) type.
//  bslmf::IsNil: meta-function to test for nil.
//
//@DESCRIPTION: 'bslmf::Nil' is mainly used for template meta-programming.  It
// is useful for providing defaults for template parameters and terminating
// template recursions.  'bslmf::Nil' can also be used to represent an unset
// state (i.e., nil) in components that require one.
//
///Usage
///-----
// The following usage illustrates a typical use of 'bslmf::Nil' as a default
// template parameter for another meta-function.
//..
//  template <int   CONDITION,
//            class TRUE_TYPE = bslmf::Nil,
//            class FALSE_TYPE = bslmf::Nil>
//  struct my_If {
//      typedef TRUE_TYPE Type;
//  };
//
//  template <class TRUE_TYPE, class FALSE_TYPE>
//  struct my_If<0, TRUE_TYPE, FALSE_TYPE> {
//      typedef FALSE_TYPE Type;
//  };
//..
// Next, we can use the 'my_If' meta-function to implement a print function
// that checks whether a type has a print method (using a fictitious
// meta-function 'my_IsPrintable'), and print the value.  Note the use of
// 'bslmf::Nil' as the type for overloading:
//..
//  template <class TYPE>
//  void print(const TYPE& value, int)
//  {
//      bsl::cout << value << bsl::endl;
//  }
//
//  template <class TYPE>
//  void print(const TYPE&, bslmf::Nil)
//  {
//      bsl::cout << "Cannot be printed." << bsl::endl;
//  }
//
//  template <class TYPE>
//  void print(const TYPE& value)
//  {
//      my_If<my_IsPrintable<TYPE>::value, int>::Type Type;  // default false
//                                                           // type is
//                                                           // 'bslmf::Nil'.
//      print(value, Type());
//  }
//..
// Using the above functions, the following code:
//..
//  int         x = 3;
//  bslmf::Nil nil;
//..
// Will print the following to 'stdout':
//..
//  3
//  Cannot be printed.
//..
// Finally, the 'bslmf::IsNil' meta-function returns true if the type passed to
// it is 'bslmf::Nil', and false otherwise:
//..
//  assert(1 == bslmf::IsNil<bslmf::Nil>::value)
//  assert(0 == bslmf::IsNil<int>::value)
//  assert(0 == bslmf::IsNil<char>::value)
//..

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLMF_INTEGRALCONSTANT
#include <bslmf_integralconstant.h>
#endif

namespace BloombergLP {

namespace bslmf {

                      // ==========
                      // struct Nil
                      // ==========

struct Nil {
    // This struct is empty and represents a nil type.
};

                      // ============
                      // struct IsNil
                      // ============

template <class T>
struct IsNil : bsl::false_type {
};

template <>
struct IsNil<Nil> : bsl::true_type {
};

}  // close package namespace


}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright (C) 2012 Bloomberg L.P.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
// ----------------------------- END-OF-FILE ----------------------------------
