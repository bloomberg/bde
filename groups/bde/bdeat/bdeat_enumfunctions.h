// bdeat_enumfunctions.h                                              -*-C++-*-
#ifndef INCLUDED_BDEAT_ENUMFUNCTIONS
#define INCLUDED_BDEAT_ENUMFUNCTIONS

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a namespace defining enumeration functions.
//
//@CLASSES:
//  bdeat_EnumFunctions: namespace for calling enumeration functions
//
//@SEE_ALSO:
//
//@AUTHOR: Clay Wilson (cwilson9), Shezan Baig (sbaig)
//
//@CONTACT: Rohan Bhindwale (rbhindwa)
//
//@DESCRIPTION: The 'bdeat_EnumFunctions' 'namespace' provided in this
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
// behavior through the 'bdeat_EnumFunctions' 'namespace'.
//
// This component specializes all of these functions for types that have the
// 'bdeat_TypeTraitBasicEnumeration' trait.
//
// Types that do not have the 'bdeat_TypeTraitBasicEnumeration' trait may have
// the functions in the 'bdeat_EnumFunctions' 'namespace' specialized for them.
// An example of this is provided in the 'Usage' section of this document.
//
///Usage
///-----
// The following snippets of code illustrate the usage of this component.
// Suppose you had a C++ 'enum' type called 'MyEnum':
//..
// #include <bdeat_enumfunctions.h>
// #include <bdeu_string.h>
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
// We can now make 'MyEnum' expose "enumeration" behavior by implementing
// all the necessary 'bdeat_enum*' functions for 'MyEnum' inside the 'mine
// namespace'.  First we should forward declare all the functions that we
// will implement inside the 'mine' namespace:
//..
//      // MANIPULATORS
//      int bdeat_enumFromInt(MyEnum *result, int number);
//          // Load into the specified 'result' the enumerator matching the
//          // specified 'number'.  Return 0 on success, and a non-zero value
//          // with no effect on 'result' if 'number' does not match any
//          // enumerator.
//
//      int bdeat_enumFromString(MyEnum *result,
//                               const char *string, int stringLength);
//          // Load into the specified 'result' the enumerator matching the
//          // specified 'string' of the specified 'stringLength'.  Return 0 on
//          // success, and a non-zero value with no effect on 'result' if
//          // 'string' and 'stringLength' do not match any enumerator.
//
//      // ACCESSORS
//      void bdeat_enumToInt(int *result, const MyEnum& value);
//          // Return the integer representation exactly matching the
//          // enumerator name corresponding to the specified enumeration
//          // 'value'.
//
//      void bdeat_enumToString(bsl::string *result, const MyEnum& value);
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
//  int mine::bdeat_enumFromInt(MyEnum *result, int number)
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
//  int mine::bdeat_enumFromString(MyEnum    *result,
//                                 const char *string,
//                                 int         stringLength)
//  {
//      enum { SUCCESS = 0, NOT_FOUND = -1 };
//
//      if (bdeu_String::areEqualCaseless("red",
//                                        string,
//                                        stringLength)) {
//          *result = RED;
//
//          return SUCCESS;
//      }
//
//      if (bdeu_String::areEqualCaseless("green",
//                                        string,
//                                        stringLength)) {
//          *result = GREEN;
//
//          return SUCCESS;
//      }
//
//      if (bdeu_String::areEqualCaseless("blue",
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
//  void mine::bdeat_enumToInt(int *result, const MyEnum& value)
//  {
//      *result = static_cast<int>(value);
//  }
//
//  void mine::bdeat_enumToString(bsl::string    *result, const MyEnum&  value)
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
// 'bdeat_EnumFunctions' namespace for the 'mine::MyEnum' type.  This makes the
// 'bdeat' infrastructure recognize 'MyEnum' as an enumeration abstraction:
//..
//  namespace bdeat_EnumFunctions {
//
//      template <>
//      struct IsEnumeration<mine::MyEnum> {
//          enum { VALUE = 1 };
//      };
//
//  }  // close namespace 'bdeat_EnumFunctions'
//  }  // close namespace 'BloombergLP'
//..
// The 'bdeat' infrastructure (and any component that uses this infrastructure)
// will now recognize 'MyEnum' as an "enumeration" type.  For example, suppose
// we have the following XML data:
//..
//  <?xml version='1.0' encoding='UTF-8' ?>
//  <MyEnum>GREEN</MyEnum>
//..
// Using the 'baexml_decoder' component, we can load this XML data into a
// 'MyEnum' object:
//..
//  #include <baexml_decoder.h>
//
//  void decodeMyEnumFromXML(bsl::istream& inputData)
//  {
//      using namespace BloombergLP;
//
//      MyEnum object = 0;
//
//      baexml_DecoderOptions options;
//      baexml_MiniReader     reader;
//      baexml_ErrorInfo      errInfo;
//
//      baexml_Decoder decoder(&options, &reader, &errInfo);
//      int result = decoder.decode(inputData, &object);
//
//      assert(0     == result);
//      assert(GREEN == object);
//  }
//..
// Note that the 'bdeat' framework can be used for functionality other than
// encoding/decoding into XML.  When 'mine::MyEnum' is plugged into the
// framework, then it will be automatically usable within the framework.
// For example, the following snippets of code will convert a string from a
// stream and load it into a 'mine::MyEnum' object:
//..
//  template <typename TYPE>
//  void readMyEnum(bsl::istream& stream, TYPE *object)
//  {
//      bsl::string value;
//      stream >> value;
//
//      return bdeat_EnumType::fromString(object, value);
//  }
//..
// Now we have a generic function that takes an input stream and a 'Cusip'
// object, and inputs its value.  We can use this generic function as
// follows:
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


#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BDEAT_TYPETRAITS
#include <bdeat_typetraits.h>
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

#ifndef INCLUDED_BSLMF_ANYTYPE
#include <bslmf_anytype.h>
#endif

#ifndef INCLUDED_BSL_STRING
#include <bsl_string.h>
#endif

namespace BloombergLP {

                      // =============================
                      // namespace bdeat_EnumFunctions
                      // =============================

namespace bdeat_EnumFunctions {
    // This 'namespace' provides functions that expose "enumeration" behavior
    // for "enumeration" types.  See the component-level documentation for more
    // information.

    // META-FUNCTIONS
#ifndef BDE_OMIT_INTERNAL_DEPRECATED

    template <typename TYPE>
    bslmf_MetaInt<0> isEnumerationMetaFunction(const TYPE&);
        // This function can be overloaded to support partial specialization
        // (Sun5.2 compiler is unable to partially specialize the 'struct'
        // below).  Note that this function is has no definition and should not
        // be called at runtime.
        //
        // This function is *DEPRECATED*.  User's should specialize the
        // 'IsEnumeration' meta-function.

#endif
    template <typename TYPE>
    struct IsEnumeration {
        // This 'struct' should be specialized for third-party types that need
        // to expose "enumeration" behavior.  See the component-level
        // documentation for further information.

        enum {
            VALUE = bslalg_HasTrait<TYPE,
                                    bdeat_TypeTraitBasicEnumeration>::VALUE
#ifndef BDE_OMIT_INTERNAL_DEPRECATED
                 || BSLMF_METAINT_TO_BOOL(isEnumerationMetaFunction(
                                                   bslmf_TypeRep<TYPE>::rep()))
#endif
        };
    };

    // MANIPULATORS
    template <typename TYPE>
    int fromInt(TYPE *result, int number);
        // Load into the specified 'result' the enumerator matching the
        // specified 'number'.  Return 0 on success, and a non-zero value with
        // no effect on 'result' if 'number' does not match any enumerator.

    template <typename TYPE>
    int fromString(TYPE *result, const char *string, int stringLength);
        // Load into the specified 'result' the enumerator matching the
        // specified 'string' of the specified 'stringLength'.  Return 0 on
        // success, and a non-zero value with no effect on 'result' if 'string'
        // and 'stringLength' do not match any enumerator.

    // ACCESSORS
    template <typename TYPE>
    void toInt(int *result, const TYPE& value);
        // Return the integer representation exactly matching the enumerator
        // name corresponding to the specified enumeration 'value'.

    template <typename TYPE>
    void toString(bsl::string *result, const TYPE& value);
        // Return the string representation exactly matching the enumerator
        // name corresponding to the specified enumeration 'value'.

#if ! defined(BSLS_PLATFORM__CMP_IBM)
    // OVERLOADABLE FUNCTIONS
    // The following functions should be overloaded for other types (in their
    // respective namespaces).  The following functions are the default
    // implementations (for 'bas_codegen.pl'-generated types).  Do *not* call
    // these functions directly.  Use the functions above instead.

    // MANIPULATORS
    template <typename TYPE>
    int bdeat_enumFromInt(TYPE *result, int number);
    template <typename TYPE>
    int bdeat_enumFromString(TYPE       *result,
                             const char *string,
                             int         stringLength);

    // ACCESSORS
    template <typename TYPE>
    void bdeat_enumToInt(int *result, const TYPE& value);
    template <typename TYPE>
    void bdeat_enumToString(bsl::string *result, const TYPE& value);
#endif

}  // close namespace bdeat_EnumFunctions

// ===========================================================================
//                      INLINE FUNCTION DEFINITIONS
// ===========================================================================

                      // -----------------------------
                      // namespace bdeat_EnumFunctions
                      // -----------------------------

// MANIPULATORS

template <typename TYPE>
inline
int bdeat_EnumFunctions::fromInt(TYPE *result, int number)
{
    return bdeat_enumFromInt(result, number);
}

template <typename TYPE>
inline
int bdeat_EnumFunctions::fromString(TYPE       *result,
                                    const char *string,
                                    int         stringLength)
{
    return bdeat_enumFromString(result, string, stringLength);
}

// ACCESSORS

template <typename TYPE>
inline
void bdeat_EnumFunctions::toInt(int *result, const TYPE& value)
{
    bdeat_enumToInt(result, value);
}

template <typename TYPE>
inline
void bdeat_EnumFunctions::toString(bsl::string *result, const TYPE& value)
{
    bdeat_enumToString(result, value);
}

           // ------------------------------------------------------
           // namespace bdeat_EnumFunctions (OVERLOADABLE FUNCTIONS)
           // ------------------------------------------------------

#if defined(BSLS_PLATFORM__CMP_IBM)
namespace bdeat_EnumFunctions {
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
    int bdeat_enumFromInt(TYPE *result, int number);
    template <typename TYPE>
    int bdeat_enumFromString(TYPE       *result,
                             const char *string,
                             int         stringLength);

    // ACCESSORS
    template <typename TYPE>
    void bdeat_enumToInt(int *result, const TYPE& value);
    template <typename TYPE>
    void bdeat_enumToString(bsl::string *result, const TYPE& value);

} // Close namespace bdeat_EnumFunctions
#endif

// MANIPULATORS

template <typename TYPE>
inline
int bdeat_EnumFunctions::bdeat_enumFromInt(TYPE *result, int number)
{
    BSLMF_ASSERT(
              (bslalg_HasTrait<TYPE, bdeat_TypeTraitBasicEnumeration>::VALUE));

    typedef typename bslalg_TypeTraits<TYPE>::Wrapper Type;
    return Type::fromInt(result, number);
}

template <typename TYPE>
inline
int bdeat_EnumFunctions::bdeat_enumFromString(TYPE       *result,
                                              const char *string,
                                              int         stringLength)
{
    BSLMF_ASSERT(
              (bslalg_HasTrait<TYPE, bdeat_TypeTraitBasicEnumeration>::VALUE));

    typedef typename bslalg_TypeTraits<TYPE>::Wrapper Type;
    return Type::fromString(result, string, stringLength);
}

// ACCESSORS

template <typename TYPE>
inline
void bdeat_EnumFunctions::bdeat_enumToInt(int *result, const TYPE& value)
{
    BSLMF_ASSERT(
              (bslalg_HasTrait<TYPE, bdeat_TypeTraitBasicEnumeration>::VALUE));

    *result = static_cast<int>(value);
}

template <typename TYPE>
inline
void bdeat_EnumFunctions::bdeat_enumToString(bsl::string *result,
                                             const TYPE&  value)
{
    BSLMF_ASSERT(
              (bslalg_HasTrait<TYPE, bdeat_TypeTraitBasicEnumeration>::VALUE));

    typedef typename bslalg_TypeTraits<TYPE>::Wrapper Wrapper;
    *result = Wrapper::toString(value);
}

}  // close namespace BloombergLP



#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2005
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
