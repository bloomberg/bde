// bdlat_sequencefunctions.h                                          -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BDLAT_SEQUENCEFUNCTIONS
#define INCLUDED_BDLAT_SEQUENCEFUNCTIONS

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a namespace defining sequence functions.
//
//@CLASSES:
//  bdlat_SequenceFunctions: namespace for calling sequence functions
//
//@SEE_ALSO: bdlat_attributeinfo
//
//@DESCRIPTION: The 'bdlat_SequenceFunctions' 'namespace' provided in this
// component defines parameterized functions that expose "sequence" behavior
// for "sequence" types.  See the package-level documentation for a full
// description of "sequence" types.  The functions in this namespace allow
// users to:
//..
//      o manipulate an attribute by attribute id or attribute name using a
//        parameterized manipulator ('manipulateAttribute').
//      o manipulate all attributes sequentially using a parameterized
//        manipulator ('manipulateAttributes').
//      o access an attribute by attribute id or attribute name using a
//        parameterized accessor ('accessAttribute').
//      o access all attributes sequentially using a parameterized accessor
//        ('accessAttributes').
//..
// Also, the meta-function 'IsSequence' contains a compile-time constant
// 'VALUE' that is non-zero if the parameterized 'TYPE' exposes "sequence"
// behavior through the 'bdlat_SequenceFunctions' 'namespace'.
//
// This component specializes all of these functions for types that have the
// 'bdlat_TypeTraitBasicSequence' trait.
//
// Types that do not have the 'bdlat_TypeTraitBasicSequence' trait can be
// plugged into the 'bdlat' framework.  This is done by overloading the
// 'bdlat_sequence*' functions inside the namespace of the plugged in type.
// For example, suppose there is a type called 'mine::MySequence'.  In order to
// plug this type into the 'bdlat' framework as a "sequence", the following
// functions must be declared and implemented in the 'mine' namespace:
//..
//  namespace mine {
//
//  // MANIPULATORS
//  template <typename MANIPULATOR>
//  int bdlat_sequenceManipulateAttribute(
//                                      MySequence   *object,
//                                      MANIPULATOR&  manipulator,
//                                      const char   *attributeName,
//                                      int           attributeNameLength);
//      // Invoke the specified 'manipulator' on the address of the
//      // (modifiable) attribute indicated by the specified 'attributeName'
//      // and 'attributeNameLength' of the specified 'object', supplying
//      // 'manipulator' with the corresponding attribute information
//      // structure.  Return non-zero value if the attribute is not found, and
//      // the value returned from the invocation of 'manipulator' otherwise.
//
//  template <typename MANIPULATOR>
//  int bdlat_sequenceManipulateAttribute(MySequence   *object,
//                                        MANIPULATOR&  manipulator,
//                                        int           attributeId);
//      // Invoke the specified 'manipulator' on the address of the
//      // (modifiable) attribute indicated by the specified 'attributeId' of
//      // the specified 'object', supplying 'manipulator' with the
//      // corresponding attribute information structure.  Return non-zero
//      // value if the attribute is not found, and the value returned from the
//      // invocation of 'manipulator' otherwise.
//
//  template <typename MANIPULATOR>
//  int bdlat_sequenceManipulateAttributes(MySequence   *object,
//                                         MANIPULATOR&  manipulator);
//      // Invoke the specified 'manipulator' sequentially on the address of
//      // each (modifiable) attribute of the specified 'object', supplying
//      // 'manipulator' with the corresponding attribute information structure
//      // until such invocation returns non-zero value.  Return the value from
//      // the last invocation of 'manipulator' (i.e., the invocation that
//      // terminated the sequence).
//
//  // ACCESSORS
//  template <typename ACCESSOR>
//  int bdlat_sequenceAccessAttribute(const MySequence&  object,
//                                    ACCESSOR&          accessor,
//                                    const char        *attributeName,
//                                    int                attributeNameLength);
//      // Invoke the specified 'accessor' on the (non-modifiable) attribute of
//      // the specified 'object' indicated by the specified 'attributeName'
//      // and 'attributeNameLength', supplying 'accessor' with the
//      // corresponding attribute information structure.  Return non-zero
//      // value if the attribute is not found, and the value returned from the
//      // invocation of 'accessor' otherwise.
//
//  template <typename ACCESSOR>
//  int bdlat_sequenceAccessAttribute(const MySequence& object,
//                                    ACCESSOR&         accessor,
//                                    int               attributeId);
//      // Invoke the specified 'accessor' on the attribute of the specified
//      // 'object' with the given 'attributeId', supplying 'accessor' with the
//      // corresponding attribute information structure.  Return non-zero if
//      // the attribute is not found, and the value returned from the
//      // invocation of 'accessor' otherwise.
//
//  template <typename ACCESSOR>
//  int bdlat_sequenceAccessAttributes(const MySequence& object,
//                                     ACCESSOR&         accessor);
//      // Invoke the specified 'accessor' sequentially on each attribute of
//      // the specified 'object', supplying 'accessor' with the corresponding
//      // attribute information structure until such invocation returns a
//      // non-zero value.  Return the value from the last invocation of
//      // 'accessor' (i.e., the invocation that terminated the sequence).
//
//  bool bdlat_sequenceHasAttribute(const MySequence&  object,
//                                  const char        *attributeName,
//                                  int                attributeNameLength);
//      // Return true if the specified 'object' has an attribute with the
//      // specified 'attributeName' of the specified 'attributeNameLength',
//      // and false otherwise.
//
//  bool bdlat_sequenceHasAttribute(const MySequence& object,
//                                  int               attributeId);
//      // Return true if the specified 'object' has an attribute with the
//      // specified 'attributeId', and false otherwise.
//
//  }  // close namespace 'mine'
//..
// Also, the 'IsSequence' meta-function must be specialized for the
// 'mine::MySequence' type in the 'bdlat_SequenceFunctions' namespace.
//
// An example of plugging in a user-defined sequence type into the 'bdlat'
// framework is shown in the 'Usage' section of this document.
//
///Usage
///-----
// The following snippets of code illustrate the usage of this component.
// Suppose you had a 'struct' that contains three members:
//..
//  #include <bdlat_sequencefunctions.h>
//  #include <bdlat_formattingmode.h>
//  #include <bdlat_attributeinfo.h>
//  #include <bdlb_string.h>
//  #include <string>
//
//  namespace BloombergLP {
//
//  namespace mine {
//
//  struct MySequence {
//      // This struct represents a sequence containing a 'string' member, an
//      // 'int' member, and a 'float' member.
//
//      // CONSTANTS
//      enum {
//          NAME_ATTRIBUTE_ID   = 1,
//          AGE_ATTRIBUTE_ID    = 2,
//          SALARY_ATTRIBUTE_ID = 3
//      };
//
//      // DATA MEMBERS
//      bsl::string d_name;
//      int         d_age;
//      float       d_salary;
//  };
//
//  }  // close namespace 'mine'
//..
// We can now make 'mine::MySequence' expose "sequence" behavior by
// implementing the necessary 'bdlat_sequence*' functions for 'MySequence'
// inside the 'mine' namespace.  First, we should forward declare all the
// functions that we will implement inside the 'mine' namespace:
//..
//  namespace mine {
//
//  template <typename MANIPULATOR>
//  int bdlat_sequenceManipulateAttribute(MySequence   *object,
//                                        MANIPULATOR&  manipulator,
//                                        const char   *attributeName,
//                                        int           attributeNameLength);
//  template <typename MANIPULATOR>
//  int bdlat_sequenceManipulateAttribute(MySequence   *object,
//                                        MANIPULATOR&  manipulator,
//                                        int           attributeId);
//  template <typename MANIPULATOR>
//  int bdlat_sequenceManipulateAttributes(MySequence   *object,
//                                         MANIPULATOR&  manipulator);
//  template <typename ACCESSOR>
//  int bdlat_sequenceAccessAttribute(const MySequence&  object,
//                                    ACCESSOR&          accessor,
//                                    const char        *attributeName,
//                                    int                attributeNameLength);
//  template <typename ACCESSOR>
//  int bdlat_sequenceAccessAttribute(const MySequence& object,
//                                    ACCESSOR&         accessor,
//                                    int               attributeId);
//  template <typename ACCESSOR>
//  int bdlat_sequenceAccessAttributes(const MySequence& object,
//                                     ACCESSOR&         accessor);
//  bool bdlat_sequenceHasAttribute(const MySequence&  object,
//                                  const char        *attributeName,
//                                  int                attributeNameLength);
//  bool bdlat_sequenceHasAttribute(const MySequence& object,
//                                  int               attributeId);
//
//  }  // close namespace 'mine'
//..
// Now, we will implement these functions.  Note that for this implementation,
// we will create a temporary 'bdlat_AttributeInfo' object and pass it along
// when invoking the manipulator or accessor.  See the 'bdlat_attributeinfo'
// component-level documentation for more information.  The implementation of
// the functions are as follows:
//..
//  template <typename MANIPULATOR>
//  int mine::bdlat_sequenceManipulateAttribute(
//                                        MySequence   *object,
//                                        MANIPULATOR&  manipulator,
//                                        const char   *attributeName,
//                                        int           attributeNameLength)
//  {
//      enum { NOT_FOUND = -1 };
//
//      if (bdlb::String::areEqualCaseless("name",
//                                        attributeName,
//                                        attributeNameLength)) {
//          return bdlat_sequenceManipulateAttribute(
//                                              object,
//                                              manipulator,
//                                              MySequence::NAME_ATTRIBUTE_ID);
//      }
//
//      if (bdlb::String::areEqualCaseless("age",
//                                        attributeName,
//                                        attributeNameLength)) {
//          return bdlat_sequenceManipulateAttribute(
//                                               object,
//                                               manipulator,
//                                               MySequence::AGE_ATTRIBUTE_ID);
//      }
//
//      if (bdlb::String::areEqualCaseless("salary",
//                                        attributeName,
//                                        attributeNameLength)) {
//          return bdlat_sequenceManipulateAttribute(
//                                            object,
//                                            manipulator,
//                                            MySequence::SALARY_ATTRIBUTE_ID);
//      }
//
//      return NOT_FOUND;
//  }
//
//  template <typename MANIPULATOR>
//  int mine::bdlat_sequenceManipulateAttribute(MySequence   *object,
//                                              MANIPULATOR&  manipulator,
//                                              int           attributeId)
//  {
//      enum { NOT_FOUND = -1 };
//
//      switch (attributeId) {
//        case MySequence::NAME_ATTRIBUTE_ID: {
//          bdlat_AttributeInfo info;
//
//          info.annotation()     = "Name of employee";
//          info.formattingMode() = bdlat_FormattingMode::DEFAULT;
//          info.id()             = MySequence::NAME_ATTRIBUTE_ID;
//          info.name()           = "name";
//          info.nameLength()     = 4;
//
//          return manipulator(&object->d_name, info);
//        }
//        case MySequence::AGE_ATTRIBUTE_ID: {
//          bdlat_AttributeInfo info;
//
//          info.annotation()     = "Age of employee";
//          info.formattingMode() = bdlat_FormattingMode::DEFAULT;
//          info.id()             = MySequence::AGE_ATTRIBUTE_ID;
//          info.name()           = "age";
//          info.nameLength()     = 3;
//
//          return manipulator(&object->d_age, info);
//        }
//        case MySequence::SALARY_ATTRIBUTE_ID: {
//          bdlat_AttributeInfo info;
//
//          info.annotation()     = "Salary of employee";
//          info.formattingMode() = bdlat_FormattingMode::DEFAULT;
//          info.id()             = MySequence::SALARY_ATTRIBUTE_ID;
//          info.name()           = "salary";
//          info.nameLength()     = 6;
//
//          return manipulator(&object->d_salary, info);
//        }
//        default: {
//            return NOT_FOUND;
//        }
//      }
//  }
//
//  template <typename MANIPULATOR>
//  int mine::bdlat_sequenceManipulateAttributes(MySequence   *object,
//                                               MANIPULATOR&  manipulator)
//  {
//      int retVal;
//
//      retVal = bdlat_sequenceManipulateAttribute(
//                                              object,
//                                              manipulator,
//                                              MySequence::NAME_ATTRIBUTE_ID);
//
//      if (0 != retVal) {
//          return retVal;
//      }
//
//      retVal = bdlat_sequenceManipulateAttribute(
//                                               object,
//                                               manipulator,
//                                               MySequence::AGE_ATTRIBUTE_ID);
//
//      if (0 != retVal) {
//          return retVal;
//      }
//
//      retVal = bdlat_sequenceManipulateAttribute(
//                                            object,
//                                            manipulator,
//                                            MySequence::SALARY_ATTRIBUTE_ID);
//
//      return retVal;
//  }
//
//  // ACCESSORS
//
//  template <typename ACCESSOR>
//  int mine::bdlat_sequenceAccessAttribute(
//                                      const MySequence&  object,
//                                      ACCESSOR&          accessor,
//                                      const char        *attributeName,
//                                      int                attributeNameLength)
//  {
//      enum { NOT_FOUND = -1 };
//
//      if (bdlb::String::areEqualCaseless("name",
//                                        attributeName,
//                                        attributeNameLength)) {
//          return bdlat_sequenceAccessAttribute(
//                                              object,
//                                              accessor,
//                                              MySequence::NAME_ATTRIBUTE_ID);
//      }
//
//      if (bdlb::String::areEqualCaseless("age",
//                                        attributeName,
//                                        attributeNameLength)) {
//          return bdlat_sequenceAccessAttribute(object,
//                                               accessor,
//                                               MySequence::AGE_ATTRIBUTE_ID);
//      }
//
//      if (bdlb::String::areEqualCaseless("salary",
//                                        attributeName,
//                                        attributeNameLength)) {
//          return bdlat_sequenceAccessAttribute(
//                                            object,
//                                            accessor,
//                                            MySequence::SALARY_ATTRIBUTE_ID);
//      }
//
//      return NOT_FOUND;
//  }
//
//  template <typename ACCESSOR>
//  int mine::bdlat_sequenceAccessAttribute(const MySequence& object,
//                                          ACCESSOR&         accessor,
//                                          int               attributeId)
//  {
//      enum { NOT_FOUND = -1 };
//
//      switch (attributeId) {
//        case MySequence::NAME_ATTRIBUTE_ID: {
//          bdlat_AttributeInfo info;
//
//          info.annotation()     = "Name of employee";
//          info.formattingMode() = bdlat_FormattingMode::DEFAULT;
//          info.id()             = MySequence::NAME_ATTRIBUTE_ID;
//          info.name()           = "name";
//          info.nameLength()     = 4;
//
//          return accessor(object.d_name, info);
//        }
//        case MySequence::AGE_ATTRIBUTE_ID: {
//          bdlat_AttributeInfo info;
//
//          info.annotation()     = "Age of employee";
//          info.formattingMode() = bdlat_FormattingMode::DEFAULT;
//          info.id()             = MySequence::AGE_ATTRIBUTE_ID;
//          info.name()           = "age";
//          info.nameLength()     = 3;
//
//          return accessor(object.d_age, info);
//        }
//        case MySequence::SALARY_ATTRIBUTE_ID: {
//          bdlat_AttributeInfo info;
//
//          info.annotation()     = "Salary of employee";
//          info.formattingMode() = bdlat_FormattingMode::DEFAULT;
//          info.id()             = MySequence::SALARY_ATTRIBUTE_ID;
//          info.name()           = "salary";
//          info.nameLength()     = 6;
//
//          return accessor(object.d_salary, info);
//        }
//        default: {
//            return NOT_FOUND;
//        }
//      }
//  }
//
//  template <typename ACCESSOR>
//  int mine::bdlat_sequenceAccessAttributes(const MySequence& object,
//                                           ACCESSOR&         accessor)
//  {
//      int retVal;
//
//      retVal = bdlat_sequenceAccessAttribute(object,
//                                             accessor,
//                                             MySequence::NAME_ATTRIBUTE_ID);
//
//      if (0 != retVal) {
//          return retVal;
//      }
//
//      retVal = bdlat_sequenceAccessAttribute(object,
//                                             accessor,
//                                             MySequence::AGE_ATTRIBUTE_ID);
//
//      if (0 != retVal) {
//          return retVal;
//      }
//
//      retVal = bdlat_sequenceAccessAttribute(object,
//                                            accessor,
//                                            MySequence::SALARY_ATTRIBUTE_ID);
//
//      return retVal;
//  }
//
//  bool mine::bdlat_sequenceHasAttribute(
//                                      const MySequence&  object,
//                                      const char        *attributeName,
//                                      int                attributeNameLength)
//  {
//      return bdlb::String::areEqualCaseless("name",
//                                           attributeName,
//                                           attributeNameLength)
//          || bdlb::String::areEqualCaseless("age",
//                                           attributeName,
//                                           attributeNameLength)
//          || bdlb::String::areEqualCaseless("salary",
//                                           attributeName,
//                                           attributeNameLength);
//  }
//
//  bool mine::bdlat_sequenceHasAttribute(const MySequence& object,
//                                        int               attributeId)
//  {
//      return MySequence::NAME_ATTRIBUTE_ID   == attributeId
//          || MySequence::AGE_ATTRIBUTE_ID    == attributeId
//          || MySequence::SALARY_ATTRIBUTE_ID == attributeId;
//  }
//..
// Finally, we need to specialize the 'IsSequence' meta-function in the
// 'bdlat_SequenceFunctions' namespace for the 'mine::MySequence' type.  This
// makes the 'bdlat' infrastructure recognize 'mine::MySequence' as a sequence
// abstraction:
//..
//  namespace bdlat_SequenceFunctions {
//
//      template <>
//      struct IsSequence<mine::MySequence> {
//          enum { VALUE = 1 };
//      };
//
//  }  // close namespace 'bdlat_SequenceFunctions'
//  }  // close namespace 'BloombergLP'
//..
// The 'bdlat' infrastructure (and any component that uses this infrastructure)
// will now recognize 'mine::MySequence' as a "sequence" type.  For example,
// suppose we have the following XML data:
//..
//  <?xml version='1.0' encoding='UTF-8' ?>
//  <MySequence>
//      <name>John Doe</name>
//      <age>29</age>
//      <salary>12345.00</salary>
//  </MySequence>
//..
// Using the 'balxml_decoder' component, we can now load this XML data into a
// 'mine::MySequence' object:
//..
//  #include <balxml_decoder.h>
//
//  void decodeMySequenceFromXML(bsl::istream& inputData)
//  {
//      using namespace BloombergLP;
//
//      mine::MySequence object;
//
//      balxml::DecoderOptions options;
//      balxml::MiniReader     reader;
//      balxml::ErrorInfo      errInfo;
//
//      balxml::Decoder decoder(&options, &reader, &errInfo);
//      int result = decoder.decode(inputData, &object);
//
//      assert(0          == result);
//      assert("John Doe" == object.d_name);
//      assert(29         == object.d_age);
//      assert(12345.00   == object.d_salary);
//  }
//..
// Note that the 'bdlat' framework can be used for functionality other than
// encoding/decoding into XML.  When 'mine::MySequence' is plugged into the
// framework, then it will be automatically usable within the framework.  For
// example, the following snippets of code will print out all the attributes of
// a sequence object:
//..
//  struct PrintAttribute {
//      // Print each visited object to the bound 'd_stream_p' object.
//
//      // DATA MEMBERS
//      bsl::ostream *d_stream_p;
//
//      template <typename TYPE, typename INFO>
//      int operator()(const TYPE& object, const INFO& info)
//      {
//          (*d_stream_p) << info.name() << ": " << object << bsl::endl;
//          return 0;
//      }
//  };
//
//  template <typename TYPE>
//  void printSequenceAttributes(bsl::ostream& stream, const TYPE& object)
//  {
//      PrintAttribute accessor;
//      accessor.d_stream_p = &stream;
//
//      bdlat_SequenceFunctions::accessAttributes(object, accessor);
//  }
//..
// Now we have a generic function that takes an output stream and a sequence
// object, and prints out each attribute with its name and value.  We can use
// this generic function as follows:
//..
//  void printMySequence(bsl::ostream& stream)
//  {
//      using namespace BloombergLP;
//
//      mine::MySequence object;
//
//      object.d_name   = "John Doe";
//      object.d_age    = 25;
//      object.d_salary = 12345.00;
//
//      stream << bsl::fixed << bsl::setprecision(2);
//
//      printSequenceAttributes(stream, object);
//  }
//..
// The function above will print the following to provided stream:
//..
//  name: John Doe
//  age: 25
//  salary: 12345.00
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

#ifndef INCLUDED_BSLMF_METAINT
#include <bslmf_metaint.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITS
#include <bslalg_typetraits.h>
#endif

#ifndef INCLUDED_BSLMF_ASSERT
#include <bslmf_assert.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSLMF_MATCHANYTYPE
#include <bslmf_matchanytype.h>
#endif

namespace BloombergLP {

                      // =================================
                      // namespace bdlat_SequenceFunctions
                      // =================================

namespace bdlat_SequenceFunctions {
    // This 'namespace' provides methods that expose "sequence" behavior for
    // "sequence" types.  See the component-level documentation for more
    // information.

    // META-FUNCTIONS
    template <class TYPE>
    struct IsSequence {
        // This 'struct' should be specialized for third-party types that need
        // to expose "sequence" behavior.  See the component-level
        // documentation for further information.

        enum {
//ARB:VALUE
            VALUE = bslalg::HasTrait<TYPE, bdlat_TypeTraitBasicSequence>::VALUE
        };
    };

    // MANIPULATORS
    template <class TYPE, class MANIPULATOR>
    int manipulateAttribute(TYPE         *object,
                            MANIPULATOR&  manipulator,
                            const char   *attributeName,
                            int           attributeNameLength);
        // Invoke the specified 'manipulator' on the address of the
        // (modifiable) attribute indicated by the specified 'attributeName'
        // and 'attributeNameLength' of the specified 'object', supplying
        // 'manipulator' with the corresponding attribute information
        // structure.  Return non-zero value if the attribute is not found, and
        // the value returned from the invocation of 'manipulator' otherwise.


    template <class TYPE, class MANIPULATOR>
    int manipulateAttribute(TYPE         *object,
                            MANIPULATOR&  manipulator,
                            int           attributeId);
        // Invoke the specified 'manipulator' on the address of the
        // (modifiable) attribute indicated by the specified 'attributeId' of
        // the specified 'object', supplying 'manipulator' with the
        // corresponding attribute information structure.  Return non-zero
        // value if the attribute is not found, and the value returned from the
        // invocation of 'manipulator' otherwise.

    template <class TYPE, class MANIPULATOR>
    int manipulateAttributes(TYPE *object, MANIPULATOR& manipulator);
        // Invoke the specified 'manipulator' sequentially on the address of
        // each (modifiable) attribute of the specified 'object', supplying
        // 'manipulator' with the corresponding attribute information structure
        // until such invocation returns non-zero value.  Return the value
        // from the last invocation of 'manipulator' (i.e., the invocation that
        // terminated the sequence).

    // ACCESSORS
    template <class TYPE, class ACCESSOR>
    int accessAttribute(const TYPE&  object,
                        ACCESSOR&    accessor,
                        const char  *attributeName,
                        int          attributeNameLength);
        // Invoke the specified 'accessor' on the (non-modifiable) attribute of
        // the specified 'object' indicated by the specified 'attributeName'
        // and 'attributeNameLength', supplying 'accessor' with the
        // corresponding attribute information structure.  Return non-zero
        // value if the attribute is not found, and the value returned from the
        // invocation of 'accessor' otherwise.

    template <class TYPE, class ACCESSOR>
    int accessAttribute(const TYPE& object,
                        ACCESSOR&   accessor,
                        int         attributeId);
        // Invoke the specified 'accessor' on the attribute of the specified
        // 'object' with the given 'attributeId', supplying 'accessor' with the
        // corresponding attribute information structure.  Return non-zero if
        // the attribute is not found, and the value returned from the
        // invocation of 'accessor' otherwise.

    template <class TYPE, class ACCESSOR>
    int accessAttributes(const TYPE& object, ACCESSOR& accessor);
        // Invoke the specified 'accessor' sequentially on each attribute of
        // the specified 'object', supplying 'accessor' with the corresponding
        // attribute information structure until such invocation returns a
        // non-zero value.  Return the value from the last invocation of
        // 'accessor' (i.e., the invocation that terminated the sequence).

    template <class TYPE>
    bool hasAttribute(const TYPE&  object,
                      const char  *attributeName,
                      int          attributeNameLength);
        // Return true if the specified 'object' has an attribute with the
        // specified 'attributeName' of the specified 'attributeNameLength',
        // and false otherwise.

    template <class TYPE>
    bool hasAttribute(const TYPE& object,
                      int         attributeId);
        // Return true if the specified 'object' has an attribute with the
        // specified 'attributeId', and false otherwise.



#if ! defined(BSLS_PLATFORM_CMP_IBM)
    // OVERLOADABLE FUNCTIONS

    // The following functions should be overloaded for other types (in their
    // respective namespaces).  The following functions are the default
    // implementations (for 'bas_codegen.pl'-generated types).  Do *not* call
    // these functions directly.  Use the functions above instead.

    // MANIPULATORS
    template <class TYPE, class MANIPULATOR>
    int bdlat_sequenceManipulateAttribute(TYPE         *object,
                                          MANIPULATOR&  manipulator,
                                          const char   *attributeName,
                                          int           attributeNameLength);
    template <class TYPE, class MANIPULATOR>
    int bdlat_sequenceManipulateAttribute(TYPE         *object,
                                          MANIPULATOR&  manipulator,
                                          int           attributeId);
    template <class TYPE, class MANIPULATOR>
    int bdlat_sequenceManipulateAttributes(TYPE         *object,
                                           MANIPULATOR&  manipulator);

    // ACCESSORS
    template <class TYPE, class ACCESSOR>
    int bdlat_sequenceAccessAttribute(const TYPE&  object,
                                      ACCESSOR&    accessor,
                                      const char  *attributeName,
                                      int          attributeNameLength);
    template <class TYPE, class ACCESSOR>
    int bdlat_sequenceAccessAttribute(const TYPE& object,
                                      ACCESSOR&   accessor,
                                      int         attributeId);
    template <class TYPE, class ACCESSOR>
    int bdlat_sequenceAccessAttributes(const TYPE& object, ACCESSOR& accessor);
    template <class TYPE>
    bool bdlat_sequenceHasAttribute(const TYPE&  object,
                                    const char  *attributeName,
                                    int          attributeNameLength);
    template <class TYPE>
    bool bdlat_sequenceHasAttribute(const TYPE& object,
                                    int         attributeId);
#endif

}  // close namespace bdlat_SequenceFunctions

// ============================================================================
//                        INLINE FUNCTION DEFINITIONS
// ============================================================================

                     // ---------------------------------
                     // namespace bdlat_SequenceFunctions
                     // ---------------------------------

// MANIPULATORS

template <class TYPE, class MANIPULATOR>
inline
int bdlat_SequenceFunctions::manipulateAttribute(
                                             TYPE         *object,
                                             MANIPULATOR&  manipulator,
                                             const char   *attributeName,
                                             int           attributeNameLength)
{
    return bdlat_sequenceManipulateAttribute(object,
                                             manipulator,
                                             attributeName,
                                             attributeNameLength);
}


template <class TYPE, class MANIPULATOR>
inline
int bdlat_SequenceFunctions::manipulateAttribute(TYPE         *object,
                                                 MANIPULATOR&  manipulator,
                                                 int           attributeId)
{
    return bdlat_sequenceManipulateAttribute(object, manipulator, attributeId);
}

template <class TYPE, class MANIPULATOR>
inline
int bdlat_SequenceFunctions::manipulateAttributes(TYPE         *object,
                                                  MANIPULATOR&  manipulator)
{
    return bdlat_sequenceManipulateAttributes(object, manipulator);
}

// ACCESSORS

template <class TYPE, class ACCESSOR>
inline
int bdlat_SequenceFunctions::accessAttribute(const TYPE&  object,
                                             ACCESSOR&    accessor,
                                             const char  *attributeName,
                                             int          attributeNameLength)
{
    return bdlat_sequenceAccessAttribute(object,
                                         accessor,
                                         attributeName,
                                         attributeNameLength);
}

template <class TYPE, class ACCESSOR>
inline
int bdlat_SequenceFunctions::accessAttribute(const TYPE& object,
                                             ACCESSOR&   accessor,
                                             int         attributeId)
{
    return bdlat_sequenceAccessAttribute(object, accessor, attributeId);
}

template <class TYPE, class ACCESSOR>
inline
int bdlat_SequenceFunctions::accessAttributes(const TYPE& object,
                                              ACCESSOR&   accessor)
{
    return bdlat_sequenceAccessAttributes(object, accessor);
}

template <class TYPE>
inline
bool bdlat_SequenceFunctions::hasAttribute(const TYPE&  object,
                                           const char  *attributeName,
                                           int          attributeNameLength)
{
    return bdlat_sequenceHasAttribute(object,
                                      attributeName,
                                      attributeNameLength);
}

template <class TYPE>
inline
bool bdlat_SequenceFunctions::hasAttribute(const TYPE& object,
                                           int         attributeId)
{
    return bdlat_sequenceHasAttribute(object, attributeId);
}

         // ----------------------------------------------------------
         // namespace bdlat_SequenceFunctions (OVERLOADABLE FUNCTIONS)
         // ----------------------------------------------------------

#if defined(BSLS_PLATFORM_CMP_IBM)
namespace bdlat_SequenceFunctions {
    // xlC 6 will not do Koenig (argument-dependent) lookup is the function
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
    template <typename TYPE, typename MANIPULATOR>
    int bdlat_sequenceManipulateAttribute(TYPE         *object,
                                          MANIPULATOR&  manipulator,
                                          const char   *attributeName,
                                          int           attributeNameLength);
    template <typename TYPE, typename MANIPULATOR>
    int bdlat_sequenceManipulateAttribute(TYPE         *object,
                                          MANIPULATOR&  manipulator,
                                          int           attributeId);
    template <typename TYPE, typename MANIPULATOR>
    int bdlat_sequenceManipulateAttributes(TYPE         *object,
                                           MANIPULATOR&  manipulator);

    // ACCESSORS
    template <typename TYPE, typename ACCESSOR>
    int bdlat_sequenceAccessAttribute(const TYPE&  object,
                                      ACCESSOR&    accessor,
                                      const char  *attributeName,
                                      int          attributeNameLength);
    template <typename TYPE, typename ACCESSOR>
    int bdlat_sequenceAccessAttribute(const TYPE& object,
                                      ACCESSOR&   accessor,
                                      int         attributeId);
    template <typename TYPE, typename ACCESSOR>
    int bdlat_sequenceAccessAttributes(const TYPE& object, ACCESSOR& accessor);
    template <typename TYPE>
    bool bdlat_sequenceHasAttribute(const TYPE&  object,
                                    const char  *attributeName,
                                    int          attributeNameLength);
    template <typename TYPE>
    bool bdlat_sequenceHasAttribute(const TYPE& object,
                                    int         attributeId);

}  // close namespace bdlat_SequenceFunctions
#endif

// MANIPULATORS

template <class TYPE, class MANIPULATOR>
inline
int bdlat_SequenceFunctions::bdlat_sequenceManipulateAttribute(
                                             TYPE         *object,
                                             MANIPULATOR&  manipulator,
                                             const char   *attributeName,
                                             int           attributeNameLength)
{
    BSLMF_ASSERT(
                (bslalg::HasTrait<TYPE, bdlat_TypeTraitBasicSequence>::VALUE));

    return object->manipulateAttribute(manipulator,
                                       attributeName,
                                       attributeNameLength);
}

template <class TYPE, class MANIPULATOR>
inline
int bdlat_SequenceFunctions::bdlat_sequenceManipulateAttribute(
                                                     TYPE         *object,
                                                     MANIPULATOR&  manipulator,
                                                     int           attributeId)
{
    BSLMF_ASSERT(
                (bslalg::HasTrait<TYPE, bdlat_TypeTraitBasicSequence>::VALUE));

    return object->manipulateAttribute(manipulator, attributeId);
}

template <class TYPE, class MANIPULATOR>
inline
int bdlat_SequenceFunctions::bdlat_sequenceManipulateAttributes(
                                                     TYPE         *object,
                                                     MANIPULATOR&  manipulator)
{
    BSLMF_ASSERT(
                (bslalg::HasTrait<TYPE, bdlat_TypeTraitBasicSequence>::VALUE));

    return object->manipulateAttributes(manipulator);
}

// ACCESSORS

template <class TYPE, class ACCESSOR>
inline
int bdlat_SequenceFunctions::bdlat_sequenceAccessAttribute(
                                              const TYPE&  object,
                                              ACCESSOR&    accessor,
                                              const char  *attributeName,
                                              int          attributeNameLength)
{
    BSLMF_ASSERT(
                (bslalg::HasTrait<TYPE, bdlat_TypeTraitBasicSequence>::VALUE));

    return object.accessAttribute(accessor,
                                  attributeName,
                                  attributeNameLength);
}

template <class TYPE, class ACCESSOR>
inline
int bdlat_SequenceFunctions::bdlat_sequenceAccessAttribute(
                                                       const TYPE& object,
                                                       ACCESSOR&   accessor,
                                                       int         attributeId)
{
    BSLMF_ASSERT(
                (bslalg::HasTrait<TYPE, bdlat_TypeTraitBasicSequence>::VALUE));

    return object.accessAttribute(accessor, attributeId);
}

template <class TYPE, class ACCESSOR>
inline
int bdlat_SequenceFunctions::bdlat_sequenceAccessAttributes(
                                                          const TYPE& object,
                                                          ACCESSOR&   accessor)
{
    BSLMF_ASSERT(
                (bslalg::HasTrait<TYPE, bdlat_TypeTraitBasicSequence>::VALUE));

    return object.accessAttributes(accessor);
}

#ifdef BSLS_PLATFORM_CMP_MSVC
#pragma warning( push )
#pragma warning( disable : 4100 )
#endif
template <class TYPE>
inline
bool bdlat_SequenceFunctions::bdlat_sequenceHasAttribute(
                                              const TYPE&  object,
                                              const char  *attributeName,
                                              int          attributeNameLength)
{
    BSLMF_ASSERT(
                (bslalg::HasTrait<TYPE, bdlat_TypeTraitBasicSequence>::VALUE));

    return 0 != object.lookupAttributeInfo(attributeName, attributeNameLength);
}

template <class TYPE>
inline
bool bdlat_SequenceFunctions::bdlat_sequenceHasAttribute(
                                                       const TYPE& object,
                                                       int         attributeId)
{
    BSLMF_ASSERT(
                (bslalg::HasTrait<TYPE, bdlat_TypeTraitBasicSequence>::VALUE));

    return 0 != object.lookupAttributeInfo(attributeId);
}
#ifdef BSLS_PLATFORM_CMP_MSVC
#pragma warning( pop )
#endif

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
