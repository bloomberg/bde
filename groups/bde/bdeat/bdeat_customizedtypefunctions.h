// bdeat_customizedtypefunctions.h                                    -*-C++-*-
#ifndef INCLUDED_BDEAT_CUSTOMIZEDTYPEFUNCTIONS
#define INCLUDED_BDEAT_CUSTOMIZEDTYPEFUNCTIONS

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a namespace defining customized type functions.
//
//@CLASSES:
//  bdeat_CustomizedTypeFunctions: namespace for customized type functions
//
//@SEE_ALSO:
//
//@AUTHOR: Shezan Baig (sbaig)
//
//@CONTACT: Rohan Bhindwale (rbhindwa)
//
//@DESCRIPTION: The 'bdeat_CustomizedTypeFunctions' 'namespace' provided in
// this component defines parameterized functions that expose
// "customized type" behavior for "customized type" types.  See the
// package-level documentation for a full description of "customized type"
// types.  The functions in this namespace allow users to:
//..
//      o convert from base type to customized type ('convertFromBaseType').
//      o convert from customized type to base type ('convertToBaseType').
//..
// Also, the meta-function 'IsCustomizedType' contains a compile-time constant
// 'VALUE' that is non-zero if the parameterized 'TYPE' exposes
// "customized type" behavior through the 'bdeat_CustomizedTypeFunctions'
// 'namespace'.
//
// The 'BaseType' meta-function contains a typedef 'Type' that specifies the
// base type of the value for the parameterized "customized type" type.
//
// This component specializes all of these functions for types that have the
// 'bdeat_TypeTraitBasicCustomizedType' trait.
//
// Types that do not have the 'bdeat_TypeTraitBasicCustomizedType' trait
// can be plugged into the bdeat framework.  This is done by overloading
// the 'bdeat_choice*' functions inside the namespace of the plugged in
// type.  For example, suppose there is a type called 'mine::Cusip'
// (defined in the example below).  In order to plug this type into the
// 'bdeat' framework as a "CustomizedType", the following functions must be
// declared and implemented in the 'mine' namespace:
//..
//      // MANIPULATORS
//      template <typename TYPE, typename BASE_TYPE>
//      int bdeat_customizedTypeConvertFromBaseType(TYPE             *object,
//                                                  const BASE_TYPE&  value);
//          // Convert from the specified 'value' to the specified customized
//          // 'object'.  Return 0 if successful and non-zero otherwise.
//
//      // ACCESSORS
//      template <typename TYPE>
//      const typename BaseType<TYPE>::Type&
//      bdeat_customizedTypeConvertToBaseType(const TYPE& object);
//          // Load into the specified 'result' the value of the specified
//          // 'object'.
//..
// Also, the 'IsCustomizedType' meta-function must be specialized for the
// 'mine::Cusip' type in the 'bdeat_CustomizedTypeFunctions' namespace.
//
///Usage
///-----
// The following snippets of code illustrate the usage of this component.
// Suppose we have a customized type called 'Cusip', holding an object of type
// 'bsl::string' with a restriction that the length of the string cannot be
// longer than nine characters.  We can obtain the value of the string using
// the following code:
//..
//    Cusip       myCusip = "281C82UE";
//    bsl::string base    = bdeat_CustomizedTypeFunctions::convertToBaseType(
//                                                                    myCusip);
//
//    assert("281C82UE" == base);
//..
// Attempting to assign a string longer than nine characters will not succeed:
//..
//    bsl::string invalidCusip = "1234567890";
//
//    int retCode = bdeat_CustomizedTypeFunctions::convertFromBaseType(
//                                                               &myCusip,
//                                                               invalidCusip);
//
//    assert(0 != retCode);
//..
// For the purpose of this example, the class definition is as follows:
//..
//  #include <bdeat_customizedtypefunctions.h>
//  #include <bdes_assert.h>
//  #include <bdeu_string.h>
//  #include <sstream>
//  #include <string>
//
//  namespace BloombergLP {
//
//  namespace mine {
//
//  class Cusip {
//     //  Identification number for the US and Canada.  It is a 9-digit number
//     //  consisting of 8 digits and a check digit.  The Bloomberg ID will be
//     // returned for Corp, Govt, Pfd if a CUSIP is not available.
//
//    private:
//      // PRIVATE DATA MEMBERS
//      bsl::string d_value;  // stored value
//
//      // FRIENDS
//      friend bool operator==(const Cusip& lhs, const Cusip& rhs);
//      friend bool operator!=(const Cusip& lhs, const Cusip& rhs);
//    public:
//      // TYPES
//      typedef bsl::string BaseType;
//
//      // CREATORS
//      explicit Cusip(bslma_Allocator *basicAllocator = 0);
//          // Create an object of type 'Cusip' having the default value.
//          // Use the optionally specified 'basicAllocator' to supply memory.
//          // If 'basicAllocator' is 0, the currently installed default
//          // allocator is used.
//
//      Cusip(const Cusip& original, bslma_Allocator *basicAllocator = 0);
//          // Create an object of type 'Cusip' having the value
//          // of the specified 'original' object.  Use the optionally
//          // specified 'basicAllocator' to supply memory.  If
//          // 'basicAllocator' is 0, the currently installed default
//          // allocator is used.
//
//      explicit Cusip(const bsl::string&  value,
//                     bslma_Allocator    *basicAllocator = 0);
//          // Create an object of type 'Cusip' having the specified 'value'.
//          // Use the optionally specified 'basicAllocator' to supply memory.
//          // If 'basicAllocator' is 0, the currently installed default
//          // allocator is used.
//
//      ~Cusip();
//          // Destroy this object.
//
//      // MANIPULATORS
//      Cusip& operator=(const Cusip& rhs);
//          // Assign to this object the value of the specified 'rhs' object.
//
//      void reset();
//          // Reset this object to the default value (i.e., its value upon
//          // default construction).
//
//      int fromString(const bsl::string& value);
//         // Convert from the specified 'value' to this type.  Return 0 if
//          // successful and non-zero otherwise.
//
//      // ACCESSORS
//      bsl::ostream& print(bsl::ostream& stream,
//                          int           level = 0,
//                          int           spacesPerLevel = 4) const;
//          // Format this object to the specified output 'stream' at the
//          // optionally specified indentation 'level' and return a reference
//          // to the modifiable 'stream'.  If 'level' is specified, optionally
//          // specify 'spacesPerLevel', the number of spaces per indentation
//          // level for this and all of its nested objects.  Each line is
//          // indented by the absolute value of 'level * spacesPerLevel'.
//          // If 'level' is negative, suppress indentation of the first
//          // line.  If 'spacesPerLevel' is negative, suppress line breaks
//          // and format the entire output on one line.  If 'stream' is
//          // initially invalid, this operation has no effect.  Note that a
//          // trailing newline is provided in multiline mode only.
//
//      const bsl::string& toString() const;
//          // Convert this value to 'bsl::string'.
//  };
//
//  // FREE OPERATORS
//  inline
//  bool operator==(const Cusip& lhs, const Cusip& rhs);
//
//      // Return 'true' if the specified 'lhs' and 'rhs' attribute objects
//      // have the same value, and 'false' otherwise.  Two attribute objects
//      // have the same value if each respective attribute has the same
//      // value.
//
//  inline
//  bool operator!=(const Cusip& lhs, const Cusip& rhs);
//
//      // Return 'true' if the specified 'lhs' and 'rhs' attribute objects
//      // do not have the same value, and 'false' otherwise.  Two attribute
//      // objects do not have the same value if one or more respective
//      // attributes differ in values.
//
//  inline
//  bsl::ostream& operator<<(bsl::ostream& stream, const Cusip& rhs);
//      // Format the specified 'rhs' to the specified output 'stream' and
//      // return a reference to the modifiable 'stream'.
//..
// The class implementation is straightforward and is deferred to the end
// of this usage example.
//
// We can now make 'Cusip' expose "customized type" behavior by implementing
// 'bdeat_CustomizedTypeFunctions' for 'Cusip'.  The first method (the
// longer one) overloads all the 'bdeat_customizedType*' functions.
// In the second method, we show how to bypass this by simply declaring the
// class 'mine::Cusip' to have the 'bdeat_TypeTraitBasicCustomizedType'
// trait.
//
/// Longer usage
///- - - - - - -
// First, we should forward declare all the functions that we will
// implement inside the 'mine' namespace:
//..
//      // MANIPULATORS
//      template <typename TYPE, typename BASE_TYPE>
//      int bdeat_customizedTypeConvertFromBaseType(TYPE             *object,
//                                                  const BASE_TYPE&  value);
//          // Convert from the specified 'value' to the specified customized
//          // 'object'.  Return 0 if successful and non-zero otherwise.
//
//      // ACCESSORS
//      template <typename TYPE>
//      const typename BaseType<TYPE>::Type&
//      bdeat_customizedTypeConvertToBaseType(const TYPE& object);
//          // Load into the specified 'result' the value of the specified
//          // 'object'.
//
//  } // close namespace 'mine'
//..
// Next, we provide the definitions for each of these functions:
//..
// // MANIPULATORS
// template <typename TYPE, typename BASE_TYPE>
// int mine::bdeat_customizedTypeConvertFromBaseType(TYPE             *object,
//                                                   const BASE_TYPE&  value);
// {
//     return object->fromString(value);
// }
//
// // ACCESSORS
// template <typename TYPE>
// const typename BaseType<TYPE>::Type&
// mine::bdeat_customizedTypeConvertToBaseType(const TYPE& object);
// {
//     return object.toString();
// }
//..
// Finally, we need to specialize the 'IsCustomizedType' meta-function in the
// 'bdeat_CustomizedTypeFunctions' namespace for the 'mine::Cusip'
// type.  This makes the 'bdeat' infrastructure recognize
// 'mine::Cusip' as a customized type abstraction:
//..
//  namespace bdeat_CustomizedTypeFunctions {
//
//      template <>
//      struct IsCustomizedType<mine::Cusip> {
//          enum { VALUE = 1 };
//      };
//
//  } // close namespace 'bdeat_CustomizedTypeFunctions'
//  } // close namespace 'BloombergLP'
//..
// The 'bdeat' infrastructure (and any component that uses this infrastructure)
// will now recognize 'mine::Cusip' as a "customized" type.
//
/// Shorter usage
///- - - - - - -
// We can bypass all the code from the longer usage example by simply
// declaring 'mine::Cusip' to have the 'bdeat_TypeTraitBasicCustomizedType'
// trait as follows:
//..
//  // TRAITS
//
//  BDEAT_DECL_CUSTOMIZEDTYPE_WITH_ALLOCATOR_TRAITS(mine::Cusip)
//..
// Again, the 'bdeat' infrastructure (and any component that uses this
// infrastructure) will now recognize 'mine::Cusip' as a "customized" type.
//
// For example, suppose we have the following XML data:
//..
//  <?xml version='1.0' encoding='UTF-8' ?>
//  <Cusip>
//      <value>"281C82UE"</value>
//  </Cusip>
//..
// Using the 'baexml_decoder' component, we can load this XML data into a
// 'mine::Cusip' object:
//..
//  #include <baexml_decoder.h>
//
//  void decodeMyCustomizedTypeFromXML(bsl::istream& inputData)
//  {
//      using namespace BloombergLP;
//
//      Cusip object;
//
//      baexml_DecoderOptions options;
//      baexml_MiniReader     reader;
//      baexml_ErrorInfo      errInfo;
//
//      baexml_Decoder decoder(&options, &reader, &errInfo);
//      int result = decoder.decode(inputData, &object);
//
//      assert(0          == result);
//      assert("281C82UE" == object.toString());
//  }
//..
// Note that the 'bdeat' framework can be used for functionality other than
// encoding/decoding into XML.  When 'mine::Cusip' is plugged into
// the framework, then it will be automatically usable within the framework.
// For example, the following snippets of code will convert a string
// from a stream and load it into a 'Cusip' object:
//..
//  template <typename TYPE>
//  int readCusip(bsl::istream& stream, TYPE *object)
//  {
//      bsl::string value;
//      stream >> value;
//
//      return bdeat_CustomizedType::convertFromBaseType(cusip, value);
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
//      mine::Cusip object;
//
//      ss << "281C82UE\n1234567890\n";
//
//      assert(0          == readCusip(ss, &object));
//      assert("281C82UE" == object.toString());
//
//      assert(0          != readCusip(ss, &object));
//  }
//..
// This concludes the usage example.
//
// For completeness, we finish by providing the straightforward details of
// the implementation of the class 'Cusip':
//..
//  // CREATORS
//
//  inline
//  Cusip::Cusip(bslma_Allocator *basicAllocator)
//  : d_value(basicAllocator)
//  {
//  }
//
//  inline
//  Cusip::Cusip(const Cusip& original, bslma_Allocator *basicAllocator)
//  : d_value(original.d_value, basicAllocator)
//  {
//  }
//
//  inline
//  Cusip::Cusip(const bsl::string& value, bslma_Allocator *basicAllocator)
//  : d_value(value, basicAllocator)
//  {
//  }
//
//  inline
//  Cusip::~Cusip()
//  {
//  }
//
//  // MANIPULATORS
//
//  inline
//  Cusip& Cusip::operator=(const Cusip& rhs)
//  {
//      d_value = rhs.d_value;
//      return *this;
//  }
//
//  inline
//  void Cusip::reset()
//  {
//      // bdeat_ValueTypeFunctions::reset(&d_value);
//      d_value.erase();
//  }
//
//  inline
//  int Cusip::fromString(const bsl::string& value)
//  {
//      enum { SUCCESS = 0, FAILURE = -1 };
//
//      globalFlag = 1;
//
//      if (9 < value.size()) {
//          return FAILURE;
//      }
//
//      d_value = value;
//
//      return SUCCESS;
//  }
//
//  // ACCESSORS
//
//  inline
//  bsl::ostream& Cusip::print(bsl::ostream& stream,
//                             int           level,
//                             int           spacesPerLevel) const
//  {
//      return bdeu_PrintMethods::print(stream,
//                                      d_value,
//                                      level,
//                                      spacesPerLevel);
//  }
//
//  inline
//  const bsl::string& Cusip::toString() const
//  {
//      globalFlag = 2;
//
//      return d_value;
//  }
//
//  // FREE OPERATORS
//
//  inline
//  bool geom::operator==(const geom::Cusip& lhs,
//                                   const geom::Cusip& rhs)
//  {
//      return lhs.d_value == rhs.d_value;
//  }
//
//  inline
//  bool geom::operator!=(const geom::Cusip& lhs,
//                                   const geom::Cusip& rhs)
//  {
//      return lhs.d_value != rhs.d_value;
//  }
//
//  inline
//  bsl::ostream& geom::operator<<(bsl::ostream& stream,
//                                            const geom::Cusip& rhs)
//  {
//      return rhs.print(stream, 0, -1);
//  }
//..


#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BDEAT_TYPETRAITS
#include <bdeat_typetraits.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITS
#include <bslalg_typetraits.h>
#endif

#ifndef INCLUDED_BSLALG_HASTRAIT
#include <bslalg_hastrait.h>
#endif

#ifndef INCLUDED_BSLMF_ASSERT
#include <bslmf_assert.h>
#endif

#ifndef INCLUDED_BSLMF_METAINT
#include <bslmf_metaint.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORMUTIL
#include <bsls_platformutil.h>
#endif

#ifndef INCLUDED_BDET_DATE
#include <bdet_date.h>
#endif

#ifndef INCLUDED_BDET_DATETZ
#include <bdet_datetz.h>
#endif

#ifndef INCLUDED_BDET_DATETIME
#include <bdet_datetime.h>
#endif

#ifndef INCLUDED_BDET_DATETIMETZ
#include <bdet_datetimetz.h>
#endif

#ifndef INCLUDED_BDET_TIME
#include <bdet_time.h>
#endif

#ifndef INCLUDED_BDET_TIMETZ
#include <bdet_timetz.h>
#endif

#ifndef INCLUDED_BSL_STRING
#include <bsl_string.h>
#endif


namespace BloombergLP {

                  // =======================================
                  // namespace bdeat_CustomizedTypeFunctions
                  // =======================================

namespace bdeat_CustomizedTypeFunctions {
    // This 'namespace' provides functions that expose "customized type"
    // behavior for "customized type" types.  See the component-level
    // documentation for more information.

    // META-FUNCTIONS
#if !defined(BSL_LEGACY) || 1 == BSL_LEGACY

    template <typename TYPE>
    bslmf_MetaInt<0> isCustomizedTypeMetaFunction(const TYPE&);
        // This function can be overloaded to support partial specialization
        // (Sun5.2 compiler is unable to partially specialize the 'struct'
        // below).  Note that this function is has no definition and should not
        // be called at runtime.
        //
        // This function is *DEPRECATED*.  User's should specialize the
        // 'IsCustomizedType' meta-function.

#endif
    template <typename TYPE>
    struct IsCustomizedType {
        // This 'struct' should be specialized for third-party types that need
        // to expose "customized type" behavior.  See the component-level
        // documentation for further information.

        enum {
            VALUE = bslalg_HasTrait<TYPE,
                                    bdeat_TypeTraitBasicCustomizedType>::VALUE
#if !defined(BSL_LEGACY) || 1 == BSL_LEGACY
                 || BSLMF_METAINT_TO_BOOL(isCustomizedTypeMetaFunction(
                                                   bslmf_TypeRep<TYPE>::rep()))
#endif
        };
    };

    template <typename TYPE>
    struct BaseType {
        // This meta-function should contain a typedef 'Type' that specifies
        // the default base type for the parameterized 'TYPE'.

        BSLMF_ASSERT(
           (bslalg_HasTrait<TYPE, bdeat_TypeTraitBasicCustomizedType>::VALUE));

        typedef typename TYPE::BaseType Type;
    };

    // MANIPULATORS
    template <typename TYPE, typename BASE_TYPE>
    int convertFromBaseType(TYPE *object, const BASE_TYPE& value);
        // Convert from the specified 'value' to the specified customized
        // 'object'.  Return 0 if successful and non-zero otherwise.

    // ACCESSORS
    template <typename TYPE>
    const typename BaseType<TYPE>::Type& convertToBaseType(const TYPE& object);
        // Load into the specified 'result' the value of the specified
        // 'object'.

#if ! defined(BSLS_PLATFORM__CMP_IBM)
    // OVERLOADABLE FUNCTIONS
    // The following functions should be overloaded for other types (in their
    // respective namespaces).  The following functions are the default
    // implementations (for 'bas_codegen.pl'-generated types).  Do *not* call
    // these functions directly.  Use the functions above instead.

    // MANIPULATORS
    template <typename TYPE, typename BASE_TYPE>
    int bdeat_customizedTypeConvertFromBaseType(TYPE             *object,
                                                const BASE_TYPE&  value);
        // Convert from the specified 'value' to the specified customized
        // 'object'.  Return 0 if successful and non-zero otherwise.

    // ACCESSORS
    template <typename TYPE>
    const typename BaseType<TYPE>::Type&
    bdeat_customizedTypeConvertToBaseType(const TYPE& object);
        // Load into the specified 'result' the value of the specified
        // 'object'.
#endif

}  // close namespace bdeat_CustomizedTypeFunctions

// ---- Anything below this line is implementation specific.  Do not use.  ----

                  // ========================================
                  // struct bdeat_CustomizedTypeFunctions_Imp
                  // ========================================

struct bdeat_CustomizedTypeFunctions_Imp {

    template <typename TYPE>
    static int convertFromBaseType(TYPE *object, bool value);

    template <typename TYPE>
    static int convertFromBaseType(TYPE *object, int value);

    template <typename TYPE>
    static int convertFromBaseType(TYPE *object, char value);

    template <typename TYPE>
    static int convertFromBaseType(TYPE *object, short value);

    template <typename TYPE>
    static int convertFromBaseType(TYPE                     *object,
                                   bsls_PlatformUtil::Int64  value);

    template <typename TYPE>
    static int convertFromBaseType(TYPE *object, unsigned int value);

    template <typename TYPE>
    static int convertFromBaseType(TYPE *object, unsigned char value);

    template <typename TYPE>
    static int convertFromBaseType(TYPE *object, unsigned short value);

    template <typename TYPE>
    static int convertFromBaseType(TYPE                      *object,
                                   bsls_PlatformUtil::Uint64  value);

    template <typename TYPE>
    static int convertFromBaseType(TYPE *object, float value);

    template <typename TYPE>
    static int convertFromBaseType(TYPE *object, double value);

    template <typename TYPE>
    static int convertFromBaseType(TYPE *object, const bdet_Date& value);

    template <typename TYPE>
    static int convertFromBaseType(TYPE *object, const bdet_DateTz& value);

    template <typename TYPE>
    static int convertFromBaseType(TYPE *object, const bdet_Datetime& value);

    template <typename TYPE>
    static int convertFromBaseType(TYPE *object, const bdet_DatetimeTz& value);

    template <typename TYPE>
    static int convertFromBaseType(TYPE *object, const bdet_Time& value);

    template <typename TYPE>
    static int convertFromBaseType(TYPE *object, const bdet_TimeTz& value);

    template <typename TYPE>
    static int convertFromBaseType(TYPE *object, const bsl::string& value);

    template <typename TYPE>
    static int convertFromBaseType(TYPE                     *object,
                                   const bsl::vector<char>&  value);

    template <typename TYPE>
    static const bool& convertToBaseType(const TYPE& object, bool*);

    template <typename TYPE>
    static const char& convertToBaseType(const TYPE& object, char*);

    template <typename TYPE>
    static const short& convertToBaseType(const TYPE& object, short*);

    template <typename TYPE>
    static const int& convertToBaseType(const TYPE& object, int*);

    template <typename TYPE>
    static const bsls_PlatformUtil::Int64& convertToBaseType(
                                                    const TYPE& object,
                                                    bsls_PlatformUtil::Int64*);

    template <typename TYPE>
    static const unsigned char& convertToBaseType(const TYPE& object,
                                                  unsigned char*);

    template <typename TYPE>
    static const unsigned short& convertToBaseType(const TYPE& object,
                                                   unsigned short*);

    template <typename TYPE>
    static const unsigned int& convertToBaseType(const TYPE& object,
                                                 unsigned int*);

    template <typename TYPE>
    static const bsls_PlatformUtil::Uint64& convertToBaseType(
                                                   const TYPE& object,
                                                   bsls_PlatformUtil::Uint64*);

    template <typename TYPE>
    static const float& convertToBaseType(const TYPE& object, float*);

    template <typename TYPE>
    static const double& convertToBaseType(const TYPE& object, double*);

    template <typename TYPE>
    static const bdet_Date& convertToBaseType(const TYPE& object, bdet_Date*);

    template <typename TYPE>
    static const bdet_DateTz& convertToBaseType(const TYPE& object,
                                                bdet_DateTz*);

    template <typename TYPE>
    static const bdet_Datetime& convertToBaseType(const TYPE& object,
                                                  bdet_Datetime*);

    template <typename TYPE>
    static const bdet_DatetimeTz& convertToBaseType(const TYPE& object,
                                                    bdet_DatetimeTz*);

    template <typename TYPE>
    static const bdet_Time& convertToBaseType(const TYPE& object, bdet_Time*);

    template <typename TYPE>
    static const bdet_TimeTz& convertToBaseType(const TYPE& object,
                                                bdet_TimeTz*);

    template <typename TYPE>
    static const bsl::string& convertToBaseType(const TYPE& object,
                                                bsl::string*);
    template <typename TYPE>
    static const bsl::vector<char>& convertToBaseType(const TYPE& object,
                                                      bsl::vector<char>*);
};

// ============================================================================
//                        INLINE FUNCTION DEFINITIONS
// ============================================================================

                   // ---------------------------------------
                   // namespace bdeat_CustomizedTypeFunctions
                   // ---------------------------------------

// MANIPULATORS

template <typename TYPE, typename BASE_TYPE>
inline
int bdeat_CustomizedTypeFunctions::convertFromBaseType(
                                                      TYPE             *object,
                                                      const BASE_TYPE&  value)
{
    return bdeat_customizedTypeConvertFromBaseType(object, value);
}

// ACCESSORS

template <typename TYPE>
inline
const typename bdeat_CustomizedTypeFunctions::BaseType<TYPE>::Type&
bdeat_CustomizedTypeFunctions::convertToBaseType(const TYPE&  object)
{
    return bdeat_customizedTypeConvertToBaseType(object);
}

      // ----------------------------------------------------------------
      // namespace bdeat_CustomizedTypeFunctions (OVERLOADABLE FUNCTIONS)
      // ----------------------------------------------------------------

#if defined(BSLS_PLATFORM__CMP_IBM)
namespace bdeat_CustomizedTypeFunctions {
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
    template <typename TYPE, typename BASE_TYPE>
    int bdeat_customizedTypeConvertFromBaseType(TYPE             *object,
                                                const BASE_TYPE&  value);
        // Convert from the specified 'value' to the specified customized
        // 'object'.  Return 0 if successful and non-zero otherwise.

    // ACCESSORS
    template <typename TYPE>
    const typename BaseType<TYPE>::Type&
    bdeat_customizedTypeConvertToBaseType(const TYPE& object);
        // Load into the specified 'result' the value of the specified
        // 'object'.
} // Close namespace bdeat_CustomizedTypeFunctions
#endif

// MANIPULATORS

template <typename TYPE, typename BASE_TYPE>
inline
int bdeat_CustomizedTypeFunctions::bdeat_customizedTypeConvertFromBaseType(
                                                      TYPE             *object,
                                                      const BASE_TYPE&  value)
{
    return bdeat_CustomizedTypeFunctions_Imp::convertFromBaseType(object,
                                                                  value);
}

// ACCESSORS

template <typename TYPE>
inline
const typename bdeat_CustomizedTypeFunctions::BaseType<TYPE>::Type&
bdeat_CustomizedTypeFunctions::bdeat_customizedTypeConvertToBaseType(
                                                           const TYPE&  object)
{
    typedef typename
    bdeat_CustomizedTypeFunctions::BaseType<TYPE>::Type BaseType;

    return bdeat_CustomizedTypeFunctions_Imp::convertToBaseType(object,
                                                                (BaseType*)0);
}

                  // ----------------------------------------
                  // struct bdeat_CustomizedTypeFunctions_Imp
                  // ----------------------------------------

template <typename TYPE>
inline
int bdeat_CustomizedTypeFunctions_Imp::convertFromBaseType(TYPE *object,
                                                           bool  value)
{
    BSLMF_ASSERT((bslalg_HasTrait<TYPE,
                                  bdeat_TypeTraitBasicCustomizedType>::VALUE));

    return object->fromBool(value);
}

template <typename TYPE>
inline
int bdeat_CustomizedTypeFunctions_Imp::convertFromBaseType(TYPE *object,
                                                           int   value)
{
    BSLMF_ASSERT((bslalg_HasTrait<TYPE,
                                  bdeat_TypeTraitBasicCustomizedType>::VALUE));

    return object->fromInt(value);
}

template <typename TYPE>
inline
int bdeat_CustomizedTypeFunctions_Imp::convertFromBaseType(TYPE *object,
                                                           char  value)
{
    BSLMF_ASSERT((bslalg_HasTrait<TYPE,
                                  bdeat_TypeTraitBasicCustomizedType>::VALUE));

    return object->fromChar(value);
}

template <typename TYPE>
inline
int bdeat_CustomizedTypeFunctions_Imp::convertFromBaseType(TYPE  *object,
                                                           short  value)
{
    BSLMF_ASSERT((bslalg_HasTrait<TYPE,
                                  bdeat_TypeTraitBasicCustomizedType>::VALUE));

    return object->fromShort(value);
}

template <typename TYPE>
inline
int bdeat_CustomizedTypeFunctions_Imp::convertFromBaseType(
                                              TYPE                     *object,
                                              bsls_PlatformUtil::Int64  value)
{
    BSLMF_ASSERT((bslalg_HasTrait<TYPE,
                                  bdeat_TypeTraitBasicCustomizedType>::VALUE));

    return object->fromInt64(value);
}

template <typename TYPE>
inline
int bdeat_CustomizedTypeFunctions_Imp::convertFromBaseType(
                                                          TYPE         *object,
                                                          unsigned int  value)
{
    BSLMF_ASSERT((bslalg_HasTrait<TYPE,
                                  bdeat_TypeTraitBasicCustomizedType>::VALUE));

    return object->fromUnsignedInt(value);
}

template <typename TYPE>
inline
int bdeat_CustomizedTypeFunctions_Imp::convertFromBaseType(
                                                         TYPE          *object,
                                                         unsigned char  value)
{
    BSLMF_ASSERT((bslalg_HasTrait<TYPE,
                                  bdeat_TypeTraitBasicCustomizedType>::VALUE));

    return object->fromUnsignedChar(value);
}

template <typename TYPE>
inline
int bdeat_CustomizedTypeFunctions_Imp::convertFromBaseType(
                                                        TYPE           *object,
                                                        unsigned short  value)
{
    BSLMF_ASSERT((bslalg_HasTrait<TYPE,
                                  bdeat_TypeTraitBasicCustomizedType>::VALUE));

    return object->fromUnsignedShort(value);
}

template <typename TYPE>
inline
int bdeat_CustomizedTypeFunctions_Imp::convertFromBaseType(
                                             TYPE                      *object,
                                             bsls_PlatformUtil::Uint64  value)
{
    BSLMF_ASSERT((bslalg_HasTrait<TYPE,
                                  bdeat_TypeTraitBasicCustomizedType>::VALUE));

    return object->fromUnsignedInt64(value);
}

template <typename TYPE>
inline
int bdeat_CustomizedTypeFunctions_Imp::convertFromBaseType(TYPE  *object,
                                                           float  value)
{
    BSLMF_ASSERT((bslalg_HasTrait<TYPE,
                                  bdeat_TypeTraitBasicCustomizedType>::VALUE));

    return object->fromFloat(value);
}

template <typename TYPE>
inline
int bdeat_CustomizedTypeFunctions_Imp::convertFromBaseType(TYPE   *object,
                                                           double  value)
{
    BSLMF_ASSERT((bslalg_HasTrait<TYPE,
                                  bdeat_TypeTraitBasicCustomizedType>::VALUE));

    return object->fromDouble(value);
}

template <typename TYPE>
inline
int bdeat_CustomizedTypeFunctions_Imp::convertFromBaseType(
                                                      TYPE             *object,
                                                      const bdet_Date&  value)
{
    BSLMF_ASSERT((bslalg_HasTrait<TYPE,
                                  bdeat_TypeTraitBasicCustomizedType>::VALUE));

    return object->fromDate(value);
}

template <typename TYPE>
inline
int bdeat_CustomizedTypeFunctions_Imp::convertFromBaseType(
                                                    TYPE               *object,
                                                    const bdet_DateTz&  value)
{
    BSLMF_ASSERT((bslalg_HasTrait<TYPE,
                                  bdeat_TypeTraitBasicCustomizedType>::VALUE));

    return object->fromDateTz(value);
}

template <typename TYPE>
inline
int bdeat_CustomizedTypeFunctions_Imp::convertFromBaseType(
                                                  TYPE                 *object,
                                                  const bdet_Datetime&  value)
{
    BSLMF_ASSERT((bslalg_HasTrait<TYPE,
                                  bdeat_TypeTraitBasicCustomizedType>::VALUE));

    return object->fromDatetime(value);
}

template <typename TYPE>
inline
int bdeat_CustomizedTypeFunctions_Imp::convertFromBaseType(
                                                TYPE                   *object,
                                                const bdet_DatetimeTz&  value)
{
    BSLMF_ASSERT((bslalg_HasTrait<TYPE,
                                  bdeat_TypeTraitBasicCustomizedType>::VALUE));

    return object->fromDatetimeTz(value);
}

template <typename TYPE>
inline
int bdeat_CustomizedTypeFunctions_Imp::convertFromBaseType(
                                                      TYPE             *object,
                                                      const bdet_Time&  value)
{
    BSLMF_ASSERT((bslalg_HasTrait<TYPE,
                                  bdeat_TypeTraitBasicCustomizedType>::VALUE));

    return object->fromTime(value);
}

template <typename TYPE>
inline
int bdeat_CustomizedTypeFunctions_Imp::convertFromBaseType(
                                                    TYPE               *object,
                                                    const bdet_TimeTz&  value)
{
    BSLMF_ASSERT((bslalg_HasTrait<TYPE,
                                  bdeat_TypeTraitBasicCustomizedType>::VALUE));

    return object->fromTimeTz(value);
}

template <typename TYPE>
inline
int bdeat_CustomizedTypeFunctions_Imp::convertFromBaseType(
                                                    TYPE               *object,
                                                    const bsl::string&  value)
{
    BSLMF_ASSERT((bslalg_HasTrait<TYPE,
                                  bdeat_TypeTraitBasicCustomizedType>::VALUE));

    return object->fromString(value);
}

template <typename TYPE>
inline
int bdeat_CustomizedTypeFunctions_Imp::convertFromBaseType(
                                              TYPE                     *object,
                                              const bsl::vector<char>&  value)
{
    BSLMF_ASSERT((bslalg_HasTrait<TYPE,
                                  bdeat_TypeTraitBasicCustomizedType>::VALUE));

    return object->fromVector(value);
}

template <typename TYPE>
inline
const bool& bdeat_CustomizedTypeFunctions_Imp::convertToBaseType(
                                                            const TYPE& object,
                                                            bool*)
{
    BSLMF_ASSERT((bslalg_HasTrait<TYPE,
                                  bdeat_TypeTraitBasicCustomizedType>::VALUE));

    return object.toBool();
}

template <typename TYPE>
inline
const int& bdeat_CustomizedTypeFunctions_Imp::convertToBaseType(
                                                            const TYPE& object,
                                                            int*)
{
    BSLMF_ASSERT((bslalg_HasTrait<TYPE,
                                  bdeat_TypeTraitBasicCustomizedType>::VALUE));

    return object.toInt();
}

template <typename TYPE>
inline
const char& bdeat_CustomizedTypeFunctions_Imp::convertToBaseType(
                                                            const TYPE& object,
                                                            char*)
{
    BSLMF_ASSERT((bslalg_HasTrait<TYPE,
                                  bdeat_TypeTraitBasicCustomizedType>::VALUE));

    return object.toChar();
}

template <typename TYPE>
inline
const short& bdeat_CustomizedTypeFunctions_Imp::convertToBaseType(
                                                            const TYPE& object,
                                                            short*)
{
    BSLMF_ASSERT((bslalg_HasTrait<TYPE,
                                  bdeat_TypeTraitBasicCustomizedType>::VALUE));

    return object.toShort();
}

template <typename TYPE>
inline
const bsls_PlatformUtil::Int64&
bdeat_CustomizedTypeFunctions_Imp::convertToBaseType(const TYPE& object,
                                                     bsls_PlatformUtil::Int64*)
{
    BSLMF_ASSERT((bslalg_HasTrait<TYPE,
                                  bdeat_TypeTraitBasicCustomizedType>::VALUE));

    return object.toInt64();
}

template <typename TYPE>
inline
const unsigned int& bdeat_CustomizedTypeFunctions_Imp::convertToBaseType(
                                                            const TYPE& object,
                                                            unsigned int*)
{
    BSLMF_ASSERT((bslalg_HasTrait<TYPE,
                                  bdeat_TypeTraitBasicCustomizedType>::VALUE));

    return object.toUnsignedInt();
}

template <typename TYPE>
inline
const unsigned char& bdeat_CustomizedTypeFunctions_Imp::convertToBaseType(
                                                            const TYPE& object,
                                                            unsigned char*)
{
    BSLMF_ASSERT((bslalg_HasTrait<TYPE,
                                  bdeat_TypeTraitBasicCustomizedType>::VALUE));

    return object.toUnsignedChar();
}

template <typename TYPE>
inline
const unsigned short& bdeat_CustomizedTypeFunctions_Imp::convertToBaseType(
                                                            const TYPE& object,
                                                            unsigned short*)
{
    BSLMF_ASSERT((bslalg_HasTrait<TYPE,
                                  bdeat_TypeTraitBasicCustomizedType>::VALUE));

    return object.toUnsignedShort();
}

template <typename TYPE>
inline
const bsls_PlatformUtil::Uint64&
bdeat_CustomizedTypeFunctions_Imp::convertToBaseType(
                                                    const TYPE& object,
                                                    bsls_PlatformUtil::Uint64*)
{
    BSLMF_ASSERT((bslalg_HasTrait<TYPE,
                                  bdeat_TypeTraitBasicCustomizedType>::VALUE));

    return object.toUnsignedInt64();
}

template <typename TYPE>
inline
const float& bdeat_CustomizedTypeFunctions_Imp::convertToBaseType(
                                                            const TYPE& object,
                                                            float*)
{
    BSLMF_ASSERT((bslalg_HasTrait<TYPE,
                                  bdeat_TypeTraitBasicCustomizedType>::VALUE));

    return object.toFloat();
}

template <typename TYPE>
inline
const double& bdeat_CustomizedTypeFunctions_Imp::convertToBaseType(
                                                            const TYPE& object,
                                                            double*)
{
    BSLMF_ASSERT((bslalg_HasTrait<TYPE,
                                  bdeat_TypeTraitBasicCustomizedType>::VALUE));

    return object.toDouble();
}

template <typename TYPE>
inline
const bdet_Date& bdeat_CustomizedTypeFunctions_Imp::convertToBaseType(
                                                            const TYPE& object,
                                                            bdet_Date*)
{
    BSLMF_ASSERT((bslalg_HasTrait<TYPE,
                                  bdeat_TypeTraitBasicCustomizedType>::VALUE));

    return object.toDate();
}

template <typename TYPE>
inline
const bdet_DateTz& bdeat_CustomizedTypeFunctions_Imp::convertToBaseType(
                                                            const TYPE& object,
                                                            bdet_DateTz*)
{
    BSLMF_ASSERT((bslalg_HasTrait<TYPE,
                                  bdeat_TypeTraitBasicCustomizedType>::VALUE));

    return object.toDateTz();
}

template <typename TYPE>
inline
const bdet_Datetime& bdeat_CustomizedTypeFunctions_Imp::convertToBaseType(
                                                            const TYPE& object,
                                                            bdet_Datetime*)
{
    BSLMF_ASSERT((bslalg_HasTrait<TYPE,
                                  bdeat_TypeTraitBasicCustomizedType>::VALUE));

    return object.toDatetime();
}

template <typename TYPE>
inline
const bdet_DatetimeTz& bdeat_CustomizedTypeFunctions_Imp::convertToBaseType(
                                                            const TYPE& object,
                                                            bdet_DatetimeTz*)
{
    BSLMF_ASSERT((bslalg_HasTrait<TYPE,
                                  bdeat_TypeTraitBasicCustomizedType>::VALUE));

    return object.toDatetimeTz();
}

template <typename TYPE>
inline
const bdet_Time& bdeat_CustomizedTypeFunctions_Imp::convertToBaseType(
                                                            const TYPE& object,
                                                            bdet_Time*)
{
    BSLMF_ASSERT((bslalg_HasTrait<TYPE,
                                  bdeat_TypeTraitBasicCustomizedType>::VALUE));

    return object.toTime();
}

template <typename TYPE>
inline
const bdet_TimeTz& bdeat_CustomizedTypeFunctions_Imp::convertToBaseType(
                                                            const TYPE& object,
                                                            bdet_TimeTz*)
{
    BSLMF_ASSERT((bslalg_HasTrait<TYPE,
                                  bdeat_TypeTraitBasicCustomizedType>::VALUE));

    return object.toTimeTz();
}

template <typename TYPE>
inline
const bsl::string& bdeat_CustomizedTypeFunctions_Imp::convertToBaseType(
                                                            const TYPE& object,
                                                            bsl::string*)
{
    BSLMF_ASSERT((bslalg_HasTrait<TYPE,
                                  bdeat_TypeTraitBasicCustomizedType>::VALUE));

    return object.toString();
}

template <typename TYPE>
inline
const bsl::vector<char>& bdeat_CustomizedTypeFunctions_Imp::convertToBaseType(
                                                            const TYPE& object,
                                                            bsl::vector<char>*)
{
    BSLMF_ASSERT((bslalg_HasTrait<TYPE,
                                  bdeat_TypeTraitBasicCustomizedType>::VALUE));

    return object.toVector();
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
