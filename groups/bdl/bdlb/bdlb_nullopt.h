// bdlb_nullopt.h                                                     -*-C++-*-
#ifndef INCLUDED_BDLB_NULLOPT
#define INCLUDED_BDLB_NULLOPT

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a tag type and constant indicating an empty nullable value.
//
//@CLASSES:
//  NullOptType: tag type representing an empty nullable value
//  nullOpt:     literal value of type 'NullOptType'
//
//@SEE_ALSO: bdlb_nullablevalue
//
//@DESCRIPTION: This component provides a class, 'bdlb::NullOptType', that
// defines a vocabulary to be used in functions that want to indicate an empty
// nullable value.
//
// In addition to the 'NullOptType' class type, this component defines a
// constant, 'bdlb::nullOpt', of type 'NullOptType', to act as a literal of
// this type, much as 'nullptr' is a (built in) literal for null pointers.
//
///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: ...
/// - - - - - - -
// Suppose we are implementing a type that has an optional null (empty) state,
// such as 'NullableValue'.  First we declare the class template.
//..
//  namespace xyza {
//  template <class TYPE>
//  class NullableValue {
//      // This class can hold a value of (the template parameter) 'TYPE' or be
//      // in an empty "null" state.
//..
// Then, we privide a buffer than can be uninitialized or hold a valid object,
// and a 'bool' variable to indicate whether an object is stored in the buffer.
//..
//      bsls::ObjectBuffer<TYPE> d_value;
//      bool                     d_isNull;
//..
// Next, we provide a constructor that makes a copy of an argument of the given
// 'TYPE'.
//..
//    public:
//      // CREATORS
//      explicit NullableValue(const TYPE& value);
//          // Create an object having the specified 'value'.
//..
// Then, we provide an implicit constructor using 'bdlb::NullOptType' to allow
// our users to explicitly create a nullable object in the null state.
//..
//      NullableValue(bdlb::NullOptType);                           // IMPLICIT
//          // Create a null object.  Note that the passed argument is used
//          // only to select this constructor overload, the actual value is
//          // not used.
//..
// Next we provide accessors to indicate whether the object is in the null
// state, and to return the wrapped value if one is avaialble.
//..
//      // ACCESSORS
//      const TYPE& getValue() const;
//          // Return a reference to the non-modifiable object held by this
//          // 'NullableValue'.  The behavior is undefined if this object is
//          // null.
//
//      bool isNull() const;
//          // Return 'false' if this object holds a value, and 'true'
//          // otherwise.  Note that a 'NullableValue' that does not hold an
//          // object is said to be "null".
//  };
//
//  }  // close namespace xyza
//..
// Now, we can write a function that consumes a nullable value.
//..
//  namespace xyzb {
//
//  struct Utility {
//      static int unwrap(const xyza::NullableValue<int>& optionalValue);
//          // Return the value of the specified 'optionalValue' if it holds a
//          // value; otherwise, return 0.
//  };
//
//  int Utility::unwrap(const xyza::NullableValue<int>& optionalValue) {
//      if (optionalValue.isNull()) {
//          return 0;                                                 // RETURN
//      }
//
//      return optionalValue.getValue();
//  }
//
//  }  // close namespace xyzb
//..
// Finally, our clients can call 'xyza::Utility::unwrap' with either a nullable
// value of their own, or with 'bdlb::nullOpt' to indicate a null object.
//..
//  int xyz_main() {
//      return xyzb::Utility::unwrap(bdlb::nullOpt);
//  }
//..


#include <bslscm_version.h>

#include <bsls_compilerfeatures.h>
#include <bsls_keyword.h>

namespace BloombergLP {
namespace bdlb {

                                // =================
                                // class NullOptType
                                // =================

class NullOptType {
    // This 'class' provides an empty tag type so that 'bdlb::NullableValue'
    // can be explicitly constructed in or assigned to an empty state.  There
    // are no publicly accessible constructors for this type other than the
    // copy constructor.  In particular, it is not default constructible, nor
    // list-initializable in C++11.

  private:
    // PRIVATE CREATORS
    BSLS_KEYWORD_CONSTEXPR NullOptType()
        // Create a 'NullOptType' value.  This constructor is not accessible to
        // users.
    {
        // This 'constexpr' function is defined inline only to allow its use
        // when called from 'makeInitialValue' to initialialize the 'constexpr'
        // value, 'nullOpt', below.
    }

  public:
    // CLASS METHODS
#if defined(BSLS_COMPILERFEATURES_SUPPORT_INLINE_VARIABLES)
    static BSLS_KEYWORD_CONSTEXPR NullOptType makeInitialValue()
        // Return a value initialized 'NullOptType' object.  It is undefined
        // behavior to call this function other than to initialize the global
        // constant, 'nullOpt', below.
    {
        // This 'constexpr' function is defined inline only to allow its use
        // initialializing the 'constexpr' value, 'nullOpt', below.

        return NullOptType();
    }
#else
    static const NullOptType s_nullOpt;
#endif
};

#if defined(BSLS_COMPILERFEATURES_SUPPORT_INLINE_VARIABLES)
inline constexpr NullOptType nullOpt = NullOptType::makeInitialValue();
#else
extern const NullOptType nullOpt;
#endif
    // Value of type 'NullOptType' that serves as a literal value for the empty
    // state of any nullable value.

// ============================================================================
//                  TEMPLATE AND INLINE FUNCTION DEFINITIONS
// ============================================================================

}  // close package namespace
}  // close enterprise namespace

#endif // ! defined(INCLUDED_BDLB_NULLOPT)

// ----------------------------------------------------------------------------
// Copyright 2019 Bloomberg Finance L.P.
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
