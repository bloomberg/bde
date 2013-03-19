// bslmf_isconvertibletoany.h                                         -*-C++-*-
#ifndef INCLUDED_BSLMF_ISCONVERTIBLETOANY
#define INCLUDED_BSLMF_ISCONVERTIBLETOANY

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a compile-time check type conversion to any other type.
//
//@CLASSES:
//  bslmf::IsConvertibleToAny: meta-function for checking conversion
//
//@SEE_ALSO: bslmf_isconvertible, bslmf_integralconstant
//
//@DESCRIPTION: This component provides a meta-function that allows to check at
// compile-time if the specified type can be converted to any other type using
// a template conversion operator.
//
///Usage
//------
// In this section we show the intended use of this component.
//
///Example 1: Determine If a Class Has a Template Conversion Operator
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we have a class 'ValueInitializer' defined as follows:
//..
//  class ValueInitializer {
//      // This class provides a way to portably and predictably
//      // value-initialize objects of any type.
//
//    public:
//      template <typename TYPE>
//      operator TYPE() {
//          return TYPE();
//      }
//  };
//..
// Possible purpose of such class could be to provide a convenient way to
// value-initialize an object of any type.  For example:
//..
//  // initialize an object of a fundamental type
//  int val1 = ValueInitializer();
//  assert(val1 == 0);
//
//  // initialize an object of an aggregate type
//  struct MyStruct {
//      int d_val;
//  };
//
//  MyStruct val2 = ValueInitializer();
//  assert(val2.d_val == 0);
//
//  // initialize an object of a generic type
//  template <typename T>
//  void foo() {
//      T val = ValueInitializer();
//      assert(val == T());
//  }
//
//  foo<int>();
//  foo<double>();
//..
// To make the above code work 'ValueInitializer' needs to have a conversion
// operator that allows to convert it to any type.  The existance of such
// conversion operator can be checked with the 'bslmf::IsConvertibleToAny'
// meta-function.
//..
//  assert(bslmf::IsConvertibleToAny<ValueInitializer>::value);
//..

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLMF_INTEGRALCONSTANT
#include <bslmf_integralconstant.h>
#endif

#ifndef INCLUDED_BSLMF_ISCONVERTIBLE
#include <bslmf_isconvertible.h>
#endif

namespace BloombergLP {

namespace bslmf {

                          // =========================
                          // struct IsConvertibleToAny
                          // =========================

template <typename TYPE>
class IsConvertibleToAny_Imp {
    // Implementation of the test if 'TYPE' is convertible to any type by
    // trying to convert it to a privately defined type.

    struct UniqueType {
        // A class convertible to this private type must have a template
        // conversion operator.
    };

public:
    typedef typename bsl::is_convertible<TYPE, UniqueType>::type Type;
        // 'Type' is defined as 'bsl::true_type' if 'TYPE' is convertible to
        // 'UniqueType' and 'bsl::false_type' otherwise.
};

template <typename TYPE>
struct IsConvertibleToAny : IsConvertibleToAny_Imp<TYPE>::Type {
    // A meta-function to check if the template parameter 'TYPE' is convertible
    // to any type.  'IsConvertibleToAny<TYPE>' derives from 'bsl::true_type'
    // if 'TYPE' is convertible to any type and 'bsl::false_type' otherwise.
};

}  // close package namespace

}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright (C) 2013 Bloomberg L.P.
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
