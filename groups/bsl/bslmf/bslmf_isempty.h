// bslmf_isempty.h                  -*-C++-*-
#ifndef INCLUDED_BSLMF_ISEMPTY
#define INCLUDED_BSLMF_ISEMPTY

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a compile-time check for detecting an empty class type.
//
//@CLASSES: bsl::is_empty: standard meta-function for detecting empty classes
//
//@SEE_ALSO: bslmf_isclass.h
//
//@DESCRIPTION: This component defines a metafunction, 'bsl::is_empty', which
// may be used to determine whether a type is a 'class' or 'struct' with no
// non-static data members other than bit-fields of length 0, no virtual
// member functions, no virtual base classes, and no base class 'B' for which
// 'is_empty<B>::value' is false. This metafunction conforms to the definition
// of the C++11 standard 'is_empty metafunction in section [meta.unary.prop].
//
// An empty class type type is *usually* stateless and, can be "stored" in a
// zero-length memory region. (Hypothetically, an empty object can hold state
// by means a global address-to-state map, but such a design is rare and is
// discouraged.) When a class inherits from an empty type, the compiler is
// expected to optimize away the storage requirements of the empty base
// class.  This optimization is known as the "Empty Base Optimization" or
// "EBO".
//
///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: Compute Storage Requirements for a Type
/// - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we wish to create a generic function which will allocate a record
// comprising a value of specified 'TYPE' and a description in the form of a
// null-terminated character string.  First, we declare the function
// prototype:
//..
//  template <class TYPE>
//  void *makeRecord(const TYPE& value, const char* description);
//..
// Next, we implement the function so that the copy of 'value' takes up no
// space if 'TYPE' is an empty class.  We manage this by computing a zero
// storage requirement if 'is_empty<TYPE>::value' is true:
//..
//  #include <cstring>
//  #include <new>
//
//  template <class TYPE>
//  void *makeRecord(const TYPE& value, const char* description)
//  {
//      // 'ValueSize' is computed at compile time.
//      static const std::size_t ValueSize = bsl::is_empty<TYPE>::value ?
//          0 : sizeof(TYPE);
//
//      // Allocate memory for value and description
//      const std::size_t MemSize = ValueSize + std::strlen(description) + 1;
//      void *mem = ::operator new(MemSize);
//
//      // Construct copy of value at front of allocated memory
//      ::new(mem) TYPE(value);
//
//      // Copy description into space following value.
//      std::strcpy(static_cast<char*>(mem) + ValueSize, description);
//
//      return mem;
//  }
//..
// Finally, we use 'makeRecord' with both an empty and non-empty value type:
//..
//  struct EmptyMarker { };
//
//  int main()
//  {
//      void *record1 = makeRecord(9999, "four nines");
//      // Value takes 'sizeof(int)' bytes at front of record.
//      assert(9999 == *static_cast<int*>(record1));
//      assert(0 == std::strcmp(static_cast<char*>(record1) + sizeof(int),
//                              "four nines"));
//
//      void *record2 = makeRecord(EmptyMarker(), "Empty");
//      // Value takes no space at front of record.
//      assert(0 == std::strcmp(static_cast<char*>(record2), "Empty"));
//
//      ::operator delete(record1);
//      ::operator delete(record2);
//
//      return 0;
//  }
//..

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLMF_INTEGRALCONSTANT
#include <bslmf_integralconstant.h>
#endif

#ifndef INCLUDED_BSLMF_ISCLASS
#include <bslmf_isclass.h>
#endif

#ifndef INCLUDED_BSLMF_REMOVECV
#include <bslmf_removecv.h>
#endif

namespace bsl {

                        // ===========================
                        // class template Is_Empty_Imp
                        // ===========================

template <class TYPE, bool IS_CLASS = bsl::is_class<TYPE>::value>
struct Is_Empty_Imp {
    // Private class: do not use outside of 'bslmf_isempty' component.
    // Implementation of 'bsl::is_empty'.

    // VALUE is false for non-class types.
    static const bool VALUE = false;
};

template <class TYPE>
struct Is_Empty_Imp<TYPE, true> {
    // Private class: do not use outside of 'bslmf_isempty' component.
    // Implementation of 'bsl::is_empty'.

    struct Derived : TYPE
    {
        Derived();                // Declared but not defined
        Derived(const Derived&);  // Declared but not defined
        ~Derived();               // Declared but not defined

        int d_data; // 'Derived' is not empty
    };

    // 'VALUE' will be true if EBO has optimized away storage for 'TYPE'.
    static const bool VALUE = (sizeof(Derived) == sizeof(int));
};

                        // =======================
                        // class template is_empty
                        // =======================

template <class TYPE>
struct is_empty :
        integral_constant<bool,
                          Is_Empty_Imp<typename remove_cv<TYPE>::type>::VALUE>
{
    // Metafunction to determine whether 'TYPE' is an empty class type.
    // Inherits from 'true_type' if 'TYPE' is a 'class' or 'struct' with no
    // non-static data members other than bit-fields of length 0, no virtual
    // member functions, no virtual base classes, and no base class 'B' for
    // which 'is_empty<B>::value' is false.
};

}  // close bsl namespace

#endif // ! defined(INCLUDED_BSLMF_ISEMPTY)

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
