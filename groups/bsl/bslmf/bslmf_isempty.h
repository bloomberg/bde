// bslmf_isempty.h                                                    -*-C++-*-
#ifndef INCLUDED_BSLMF_ISEMPTY
#define INCLUDED_BSLMF_ISEMPTY

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a compile-time check for detecting an empty class type.
//
//@CLASSES:
//   bsl::is_empty: standard meta-function for detecting empty classes
//
//@SEE_ALSO: bslmf_isclass.h
//
//@DESCRIPTION: This component defines a metafunction, 'bsl::is_empty', which
// may be used to determine whether a type is a 'class' or 'struct' with no
// non-static data members other than bit-fields of length 0, no virtual member
// functions, no virtual base classes, and no base class 'B' for which
// 'is_empty<B>::value' is 'false'.  This metafunction conforms to the
// definition of the C++11 standard 'is_empty metafunction in section
// [meta.unary.prop].
//
// An empty class type type is *usually* stateless and, can be "stored" in a
// zero-length memory region.  (Hypothetically, an empty object can hold state
// by means a global address-to-state map, but such a design is rare and is
// discouraged.) When a class inherits from an empty type, the compiler is
// expected to optimize away the storage requirements of the empty base class.
// This optimization is known as the "Empty Base Optimization" or "EBO".
//
///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: Compute Storage Requirements for a Type
/// - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we wish to create a generic function that will allocate a record
// comprising a value of specified 'TYPE' and a description in the form of a
// null-terminated character string.  First, we declare the function prototype:
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

                        // ============================
                        // class template Is_Empty_Size
                        // ============================

struct Is_Empty_Size {
    // Private class: do not use outside of 'bslmf_isempty' component.  This
    // component-private class is an example of an empty type, that can be used
    // to calculate the likely size of an empty type on the current platform.
    // As the inheritance-based technique used below disallows unions with a
    // hard error, rather than a SFINAE-friendly failure, it is important to
    // avoid instantiating the 'Imp' test unless there is a real chance that
    // the type might be empty.  While we cannot protect against testing small
    // unions, we can rule out the more common cases where at least one union
    // member has more than the smallest permissible size on the current ABI.
};

                // =================================
                // class template Is_Empty_Class_Imp
                // =================================

template <class TYPE,
          bool  IS_CLASS = sizeof(TYPE) == sizeof(Is_Empty_Size)>
struct Is_Empty_Class_Imp : false_type {
    // Private class: do not use outside of 'bslmf_isempty' component.  This
    // metafunction derives from 'false_type' unless (the template parameter)
    // 'TYPE' is exactly the same size as a known empty type, in which case
    // the following partial specialization is chosen.  This test filters out
    // the majority of problems with 'union' types that are classes, and would
    // produce a hard, non-SFINAEable error trying to a create a derived class
    // to compare size in the next test.
};

template <class TYPE>
struct Is_Empty_Class_Imp<TYPE, true> {
    // Private class: do not use outside of 'bslmf_isempty' component.
    // Implementation of 'bsl::is_empty' for class types that are small enough
    // that they might be an empty class.

  private:
    struct Derived : TYPE {
        // This 'struct' derives from the template parameter 'TYPE' from the
        // enclosing class template, and will have the same size as an 'int' if
        // 'TYPE' is an empty class, and a larger size otherwise.

        Derived();                // Declared but not defined
        Derived(const Derived&);  // Declared but not defined
        ~Derived();               // Declared but not defined

        int d_data; // 'Derived' is not empty
    };

  public:
    typedef integral_constant<bool, sizeof(Derived) == sizeof(int)> type;
        // 'true_type' if (the template parameter) 'TYPE' is an empty class,
        // and 'false_type' otherwise.
};

                        // ===========================
                        // class template Is_Empty_Imp
                        // ===========================

template <class TYPE, bool IS_CLASS = bsl::is_class<TYPE>::value>
struct Is_Empty_Imp : false_type {
    // Private class: do not use outside of 'bslmf_isempty' component.  This
    // metafunction provides an initial dispatch that always derives from
    // 'false_type' unless the template parameter 'TYPE' is a class type, as
    // only class types can be empty.  The following partial specialization
    // forwards all class types to a final test.  This two-phase dispatch is
    // necessary as some types, such as 'void' and function types, cannot be
    // passed to a 'sizeof' operator that is used to implement the next stage
    // of matching.
};

template <class TYPE>
struct Is_Empty_Imp<TYPE, true> : Is_Empty_Class_Imp<TYPE>::type {
    // Private class: do not use outside of 'bslmf_isempty' component.
    // Implementation of 'bsl::is_empty'.  This partial specialization derives
    // from the nested 'type' member of the 'Is_Empty_Class_Imp' metafunction,
    // which must be 'true_type' if (the template parameter) 'TYPE' is an
    // empty class, and 'false_type' otherwise.
};

                        // =======================
                        // class template is_empty
                        // =======================

template <class TYPE>
struct is_empty : Is_Empty_Imp<typename remove_cv<TYPE>::type>::type
{
    // This 'struct' is a metafunction to determine whether the (template
    // parameter) 'TYPE' is an empty class type.  'is_empty' inherits from
    // 'true_type' if 'TYPE' is a 'class' or 'struct' with no non-static data
    // members other than bit-fields of length 0, no virtual member functions,
    // no virtual base classes, and no base class 'B' for which
    // 'is_empty<B>::value' is 'false'; otherwise 'is_empty' inherits from
    // 'false_type'.  Note that this metafunction will fail to compile for a
    // union that is the same size as an empty class in C++03
};

}  // close namespace bsl

#endif // ! defined(INCLUDED_BSLMF_ISEMPTY)

// ----------------------------------------------------------------------------
// Copyright 2015 Bloomberg Finance L.P.
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
