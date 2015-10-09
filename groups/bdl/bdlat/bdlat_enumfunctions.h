// bdlat_enumfunctions.h                                              -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BDLAT_ENUMFUNCTIONS
#define INCLUDED_BDLAT_ENUMFUNCTIONS

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a namespace defining enumeration functions.
//
//@CLASSES:
//  bdlat_EnumFunctions: namespace for calling enumeration functions
//
//@SEE_ALSO:
//
//@DESCRIPTION: The 'bdlat_EnumFunctions' 'namespace' provided in this
// component defines parameterized functions that expose "enumeration" behavior
// for "enumeration" types.  See the package-level documentation for a full
// description of "enumeration" types.  The functions in this namespace allow
// users to:
//..
//      o load an enumeration value from an integer value ('fromInt').
//      o load an enumeration value from a string value ('fromString').
//      o load an integer value from an enumeration value ('toInt').
//      o load a string value from an enumeration value ('toString').
//..
// Also, the meta-function 'IsEnumeration' contains a compile-time constant
// 'VALUE' that is non-zero if the parameterized 'TYPE' exposes "enumeration"
// behavior through the 'bdlat_EnumFunctions' 'namespace'.
//
// This component specializes all of these functions for types that have the
// 'bdlat_TypeTraitBasicEnumeration' trait.
//
// Types that do not have the 'bdlat_TypeTraitBasicEnumeration' trait may have
// the functions in the 'bdlat_EnumFunctions' 'namespace' specialized for them.
// An example of this is provided in the 'Usage' section of this document.
//
///Usage
///-----
// The following snippets of code illustrate the usage of this component.
// Suppose you had a C++ 'enum' type called 'MyEnum':
//..
// #include <bdlat_enumfunctions.h>
// #include <bdlb_string.h>
// #include <sstream>
// #include <string>
//
// namespace BloombergLP {
//
// namespace mine {
//
// enum MyEnum {
//     RED   = 1,
//     GREEN = 2,
//     BLUE  = 3
// };
//..
// We can now make 'MyEnum' expose "enumeration" behavior by implementing all
// the necessary 'bdlat_enum*' functions for 'MyEnum' inside the 'mine'
// namespace.  First we should forward declare all the functions that we will
// implement inside the 'mine' namespace:
//..
//      // MANIPULATORS
//
//      int bdlat_enumFromInt(MyEnum *result, int number);
//          // Load into the specified 'result' the enumerator matching the
//          // specified 'number'.  Return 0 on success, and a non-zero value
//          // with no effect on 'result' if 'number' does not match any
//          // enumerator.
//
//      int bdlat_enumFromString(MyEnum *result,
//                               const char *string, int stringLength);
//          // Load into the specified 'result' the enumerator matching the
//          // specified 'string' of the specified 'stringLength'.  Return 0 on
//          // success, and a non-zero value with no effect on 'result' if
//          // 'string' and 'stringLength' do not match any enumerator.
//
//      // ACCESSORS
//
//      void bdlat_enumToInt(int *result, const MyEnum& value);
//          // Return the integer representation exactly matching the
//          // enumerator name corresponding to the specified enumeration
//          // 'value'.
//
//      void bdlat_enumToString(bsl::string *result, const MyEnum& value);
//          // Return the string representation exactly matching the enumerator
//          // name corresponding to the specified enumeration 'value'.
//
//  }  // close namespace mine
//..
// Next, we provide the definitions for each of these functions:
//..
//  // MANIPULATORS
//
//  inline
//  int mine::bdlat_enumFromInt(MyEnum *result, int number)
//  {
//      enum { SUCCESS = 0, NOT_FOUND = -1 };
//
//      switch (number) {
//        case RED: {
//          *result = RED;
//
//          return SUCCESS;
//        }
//        case GREEN: {
//          *result = GREEN;
//
//          return SUCCESS;
//        }
//        case BLUE: {
//          *result = BLUE;
//
//          return SUCCESS;
//        }
//        default: {
//          return NOT_FOUND;
//        }
//      }
//  }
//
//  inline
//  int mine::bdlat_enumFromString(MyEnum    *result,
//                                 const char *string,
//                                 int         stringLength)
//  {
//      enum { SUCCESS = 0, NOT_FOUND = -1 };
//
//      if (bdlb::String::areEqualCaseless("red",
//                                        string,
//                                        stringLength)) {
//          *result = RED;
//
//          return SUCCESS;
//      }
//
//      if (bdlb::String::areEqualCaseless("green",
//                                        string,
//                                        stringLength)) {
//          *result = GREEN;
//
//          return SUCCESS;
//      }
//
//      if (bdlb::String::areEqualCaseless("blue",
//                                        string,
//                                        stringLength)) {
//          *result = BLUE;
//
//          return SUCCESS;
//      }
//
//      return NOT_FOUND;
//  }
//
//  // ACCESSORS
//
//  void mine::bdlat_enumToInt(int *result, const MyEnum& value)
//  {
//      *result = static_cast<int>(value);
//  }
//
//  void mine::bdlat_enumToString(bsl::string    *result, const MyEnum&  value)
//  {
//      switch (value) {
//        case RED: {
//          *result = "RED";
//        } break;
//        case GREEN: {
//          *result = "GREEN";
//        } break;
//        case BLUE: {
//          *result = "BLUE";
//        } break;
//        default: {
//          *result = "UNKNOWN";
//        } break;
//      }
//  }
//..
// Finally, we need to specialize the 'IsEnum' meta-function in the
// 'bdlat_EnumFunctions' namespace for the 'mine::MyEnum' type.  This makes the
// 'bdlat' infrastructure recognize 'MyEnum' as an enumeration abstraction:
//..
//  namespace bdlat_EnumFunctions {
//
//      template <>
//      struct IsEnumeration<mine::MyEnum> {
//          enum { VALUE = 1 };
//      };
//
//  }  // close namespace 'bdlat_EnumFunctions'
//  }  // close namespace 'BloombergLP'
//..
// The 'bdlat' infrastructure (and any component that uses this infrastructure)
// will now recognize 'MyEnum' as an "enumeration" type.  For example, suppose
// we have the following XML data:
//..
//  <?xml version='1.0' encoding='UTF-8' ?>
//  <MyEnum>GREEN</MyEnum>
//..
// Using the 'balxml_decoder' component, we can load this XML data into a
// 'MyEnum' object:
//..
//  #include <balxml_decoder.h>
//
//  void decodeMyEnumFromXML(bsl::istream& inputData)
//  {
//      using namespace BloombergLP;
//
//      MyEnum object = 0;
//
//      balxml::DecoderOptions options;
//      balxml::MiniReader     reader;
//      balxml::ErrorInfo      errInfo;
//
//      balxml::Decoder decoder(&options, &reader, &errInfo);
//      int result = decoder.decode(inputData, &object);
//
//      assert(0     == result);
//      assert(GREEN == object);
//  }
//..
// Note that the 'bdlat' framework can be used for functionality other than
// encoding/decoding into XML.  When 'mine::MyEnum' is plugged into the
// framework, then it will be automatically usable within the framework.  For
// example, the following snippets of code will convert a string from a stream
// and load it into a 'mine::MyEnum' object:
//..
//  template <typename TYPE>
//  void readMyEnum(bsl::istream& stream, TYPE *object)
//  {
//      bsl::string value;
//      stream >> value;
//
//      return bdlat_EnumType::fromString(object, value);
//  }
//..
// Now we have a generic function that takes an input stream and a 'Cusip'
// object, and inputs its value.  We can use this generic function as follows:
//..
//  void usageExample()
//  {
//      using namespace BloombergLP;
//
//      bsl::stringstream ss;
//      mine::MyEnum object;
//
//      ss << "GREEN" << bsl::endl << "BROWN" << bsl::endl;
//
//      assert(0           == readMyEnum(ss, &object));
//      assert(mine::GREEN == object);
//
//      assert(0           != readMyEnum(ss, &object));
//  }
//..


#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

#ifndef INCLUDED_BDLAT_BDEATOVERRIDES
#include <bdlat_bdeatoverrides.h>
#endif

#ifndef INCLUDED_BDLAT_TYPETRAITS
#include <bdlat_typetraits.h>
#endif

#ifndef INCLUDED_BSLMF_ASSERT
#include <bslmf_assert.h>
#endif

#ifndef INCLUDED_BSLMF_METAINT
#include <bslmf_metaint.h>
#endif

#ifndef INCLUDED_BSLALG_HASTRAIT
#include <bslalg_hastrait.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITS
#include <bslalg_typetraits.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSLMF_MATCHANYTYPE
#include <bslmf_matchanytype.h>
#endif

#ifndef INCLUDED_BSL_STRING
#include <bsl_string.h>
#endif

namespace BloombergLP {

                      // =============================
                      // namespace bdlat_EnumFunctions
                      // =============================

namespace bdlat_EnumFunctions {
    // This 'namespace' provides functions that expose "enumeration" behavior
    // for "enumeration" types.  See the component-level documentation for more
    // information.

    // META-FUNCTIONS
    template <class TYPE>
    struct IsEnumeration {
        // This 'struct' should be specialized for third-party types that need
        // to expose "enumeration" behavior.  See the component-level
        // documentation for further information.

        enum {
//ARB:VALUE
            VALUE = bslalg::HasTrait<TYPE,
                                    bdlat_TypeTraitBasicEnumeration>::VALUE
        };
    };

    // MANIPULATORS
    template <class TYPE>
    int fromInt(TYPE *result, int number);
        // Load into the specified 'result' the enumerator matching the
        // specified 'number'.  Return 0 on success, and a non-zero value with
        // no effect on 'result' if 'number' does not match any enumerator.

    template <class TYPE>
    int fromString(TYPE *result, const char *string, int stringLength);
        // Load into the specified 'result' the enumerator matching the
        // specified 'string' of the specified 'stringLength'.  Return 0 on
        // success, and a non-zero value with no effect on 'result' if 'string'
        // and 'stringLength' do not match any enumerator.

    // ACCESSORS
    template <class TYPE>
    void toInt(int *result, const TYPE& value);
        // Return the integer representation exactly matching the enumerator
        // name corresponding to the specified enumeration 'value'.

    template <class TYPE>
    void toString(bsl::string *result, const TYPE& value);
        // Return the string representation exactly matching the enumerator
        // name corresponding to the specified enumeration 'value'.

#if ! defined(BSLS_PLATFORM_CMP_IBM)
    // OVERLOADABLE FUNCTIONS

    // The following functions should be overloaded for other types (in their
    // respective namespaces).  The following functions are the default
    // implementations (for 'bas_codegen.pl'-generated types).  Do *not* call
    // these functions directly.  Use the functions above instead.

    // MANIPULATORS
    template <class TYPE>
    int bdlat_enumFromInt(TYPE *result, int number);
    template <class TYPE>
    int bdlat_enumFromString(TYPE       *result,
                             const char *string,
                             int         stringLength);

    // ACCESSORS
    template <class TYPE>
    void bdlat_enumToInt(int *result, const TYPE& value);
    template <class TYPE>
    void bdlat_enumToString(bsl::string *result, const TYPE& value);
#endif

}  // close namespace bdlat_EnumFunctions

// ============================================================================
//                        INLINE FUNCTION DEFINITIONS
// ============================================================================

                      // -----------------------------
                      // namespace bdlat_EnumFunctions
                      // -----------------------------

// MANIPULATORS

template <class TYPE>
inline
int bdlat_EnumFunctions::fromInt(TYPE *result, int number)
{
    return bdlat_enumFromInt(result, number);
}

template <class TYPE>
inline
int bdlat_EnumFunctions::fromString(TYPE       *result,
                                    const char *string,
                                    int         stringLength)
{
    return bdlat_enumFromString(result, string, stringLength);
}

// ACCESSORS

template <class TYPE>
inline
void bdlat_EnumFunctions::toInt(int *result, const TYPE& value)
{
    bdlat_enumToInt(result, value);
}

template <class TYPE>
inline
void bdlat_EnumFunctions::toString(bsl::string *result, const TYPE& value)
{
    bdlat_enumToString(result, value);
}

           // ------------------------------------------------------
           // namespace bdlat_EnumFunctions (OVERLOADABLE FUNCTIONS)
           // ------------------------------------------------------

#if defined(BSLS_PLATFORM_CMP_IBM)
namespace bdlat_EnumFunctions {
    // xlC 6 will not do Koenig (argument-dependent) lookup if the function
    // being called has already been declared in some scope at the point of
    // the template function *definition* (not instantiation).  We work around
    // this bug by not declaring these functions until *after* the template
    // definitions that call them.

    // OVERLOADABLE FUNCTIONS
    // The following functions should be overloaded for other types (in their
    // respective namespaces).  The following functions are the default
    // implementations (for 'bas_codegen.pl'-generated types).  Do *not* call
    // these functions directly.  Use the functions above instead.

    // MANIPULATORS
    template <typename TYPE>
    int bdlat_enumFromInt(TYPE *result, int number);
    template <typename TYPE>
    int bdlat_enumFromString(TYPE       *result,
                             const char *string,
                             int         stringLength);

    // ACCESSORS
    template <typename TYPE>
    void bdlat_enumToInt(int *result, const TYPE& value);
    template <typename TYPE>
    void bdlat_enumToString(bsl::string *result, const TYPE& value);

} // Close namespace bdlat_EnumFunctions
#endif

// MANIPULATORS

template <class TYPE>
inline
int bdlat_EnumFunctions::bdlat_enumFromInt(TYPE *result, int number)
{
    BSLMF_ASSERT(bdlat_IsBasicEnumeration<TYPE>::value);

    typedef typename bdlat_BasicEnumerationWrapper<TYPE>::Wrapper Wrapper;
    return Wrapper::fromInt(result, number);
}

template <class TYPE>
inline
int bdlat_EnumFunctions::bdlat_enumFromString(TYPE       *result,
                                              const char *string,
                                              int         stringLength)
{
    BSLMF_ASSERT(
             (bslalg::HasTrait<TYPE, bdlat_TypeTraitBasicEnumeration>::VALUE));

    typedef typename bdlat_BasicEnumerationWrapper<TYPE>::Wrapper Wrapper;
    return Wrapper::fromString(result, string, stringLength);
}

// ACCESSORS

template <class TYPE>
inline
void bdlat_EnumFunctions::bdlat_enumToInt(int *result, const TYPE& value)
{
    BSLMF_ASSERT(
             (bslalg::HasTrait<TYPE, bdlat_TypeTraitBasicEnumeration>::VALUE));

    *result = static_cast<int>(value);
}

template <class TYPE>
inline
void bdlat_EnumFunctions::bdlat_enumToString(bsl::string *result,
                                             const TYPE&  value)
{
    BSLMF_ASSERT(
             (bslalg::HasTrait<TYPE, bdlat_TypeTraitBasicEnumeration>::VALUE));

    typedef typename bdlat_BasicEnumerationWrapper<TYPE>::Wrapper Wrapper;
    *result = Wrapper::toString(value);
}

}  // close enterprise namespace

#endif

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
