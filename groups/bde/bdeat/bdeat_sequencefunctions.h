// bdeat_sequencefunctions.h                                          -*-C++-*-
#ifndef INCLUDED_BDEAT_SEQUENCEFUNCTIONS
#define INCLUDED_BDEAT_SEQUENCEFUNCTIONS

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a namespace defining sequence functions.
//
//@CLASSES:
//  bdeat_SequenceFunctions: namespace for calling sequence functions
//
//@SEE_ALSO: bdeat_attributeinfo
//
//@AUTHOR: Clay Wilson (cwilson9), Shezan Baig (sbaig)
//
//@CONTACT: Rohan Bhindwale (rbhindwa)
//
//@DESCRIPTION: The 'bdeat_SequenceFunctions' 'namespace' provided in this
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
// behavior through the 'bdeat_SequenceFunctions' 'namespace'.
//
// This component specializes all of these functions for types that have the
// 'bdeat_TypeTraitBasicSequence' trait.
//
// Types that do not have the 'bdeat_TypeTraitBasicSequence' trait can be
// plugged into the bdeat framework.  This is done by overloading the
// 'bdeat_sequence*' functions inside the namespace of the plugged in type.
// For example, suppose there is a type called 'mine::MySequence'.  In order to
// plug this type into the 'bdeat' framework as a "sequence", the following
// functions must be declared and implemented in the 'mine' namespace:
//..
//  namespace mine {
//
//  // MANIPULATORS
//  template <typename MANIPULATOR>
//  int bdeat_sequenceManipulateAttribute(
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
//  int bdeat_sequenceManipulateAttribute(MySequence   *object,
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
//  int bdeat_sequenceManipulateAttributes(MySequence   *object,
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
//  int bdeat_sequenceAccessAttribute(const MySequence&  object,
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
//  int bdeat_sequenceAccessAttribute(const MySequence& object,
//                                    ACCESSOR&         accessor,
//                                    int               attributeId);
//      // Invoke the specified 'accessor' on the attribute of the specified
//      // 'object' with the given 'attributeId', supplying 'accessor' with the
//      // corresponding attribute information structure.  Return non-zero if
//      // the attribute is not found, and the value returned from the
//      // invocation of 'accessor' otherwise.
//
//  template <typename ACCESSOR>
//  int bdeat_sequenceAccessAttributes(const MySequence& object,
//                                     ACCESSOR&         accessor);
//      // Invoke the specified 'accessor' sequentially on each attribute of
//      // the specified 'object', supplying 'accessor' with the corresponding
//      // attribute information structure until such invocation returns a
//      // non-zero value.  Return the value from the last invocation of
//      // 'accessor' (i.e., the invocation that terminated the sequence).
//
//  bool bdeat_sequenceHasAttribute(const MySequence&  object,
//                                  const char        *attributeName,
//                                  int                attributeNameLength);
//      // Return true if the specified 'object' has an attribute with the
//      // specified 'attributeName' of the specified 'attributeNameLength',
//      // and false otherwise.
//
//  bool bdeat_sequenceHasAttribute(const MySequence& object,
//                                  int               attributeId);
//      // Return true if the specified 'object' has an attribute with the
//      // specified 'attributeId', and false otherwise.
//
//  }  // close namespace 'mine'
//..
// Also, the 'IsSequence' meta-function must be specialized for the
// 'mine::MySequence' type in the 'bdeat_SequenceFunctions' namespace.
//
// An example of plugging in a user-defined sequence type into the 'bdeat'
// framework is shown in the 'Usage' section of this document.
//
///Usage
///-----
// The following snippets of code illustrate the usage of this component.
// Suppose you had a 'struct' that contains three members:
//..
//  #include <bdeat_sequencefunctions.h>
//  #include <bdeat_formattingmode.h>
//  #include <bdeat_attributeinfo.h>
//  #include <bdeu_string.h>
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
// implementing the necessary 'bdeat_sequence*' functions for 'MySequence'
// inside the 'mine' namespace.  First, we should forward declare all the
// functions that we will implement inside the 'mine' namespace:
//..
//  namespace mine {
//
//  template <typename MANIPULATOR>
//  int bdeat_sequenceManipulateAttribute(MySequence   *object,
//                                        MANIPULATOR&  manipulator,
//                                        const char   *attributeName,
//                                        int           attributeNameLength);
//  template <typename MANIPULATOR>
//  int bdeat_sequenceManipulateAttribute(MySequence   *object,
//                                        MANIPULATOR&  manipulator,
//                                        int           attributeId);
//  template <typename MANIPULATOR>
//  int bdeat_sequenceManipulateAttributes(MySequence   *object,
//                                         MANIPULATOR&  manipulator);
//  template <typename ACCESSOR>
//  int bdeat_sequenceAccessAttribute(const MySequence&  object,
//                                    ACCESSOR&          accessor,
//                                    const char        *attributeName,
//                                    int                attributeNameLength);
//  template <typename ACCESSOR>
//  int bdeat_sequenceAccessAttribute(const MySequence& object,
//                                    ACCESSOR&         accessor,
//                                    int               attributeId);
//  template <typename ACCESSOR>
//  int bdeat_sequenceAccessAttributes(const MySequence& object,
//                                     ACCESSOR&         accessor);
//  bool bdeat_sequenceHasAttribute(const MySequence&  object,
//                                  const char        *attributeName,
//                                  int                attributeNameLength);
//  bool bdeat_sequenceHasAttribute(const MySequence& object,
//                                  int               attributeId);
//
//  }  // close namespace 'mine'
//..
// Now, we will implement these functions.  Note that for this implementation,
// we will create a temporary 'bdeat_AttributeInfo' object and pass it along
// when invoking the manipulator or accessor.  See the 'bdeat_attributeinfo'
// component-level documentation for more information.  The implementation of
// the functions are as follows:
//..
//  template <typename MANIPULATOR>
//  int mine::bdeat_sequenceManipulateAttribute(
//                                        MySequence   *object,
//                                        MANIPULATOR&  manipulator,
//                                        const char   *attributeName,
//                                        int           attributeNameLength)
//  {
//      enum { NOT_FOUND = -1 };
//
//      if (bdeu_String::areEqualCaseless("name",
//                                        attributeName,
//                                        attributeNameLength)) {
//          return bdeat_sequenceManipulateAttribute(
//                                              object,
//                                              manipulator,
//                                              MySequence::NAME_ATTRIBUTE_ID);
//      }
//
//      if (bdeu_String::areEqualCaseless("age",
//                                        attributeName,
//                                        attributeNameLength)) {
//          return bdeat_sequenceManipulateAttribute(
//                                               object,
//                                               manipulator,
//                                               MySequence::AGE_ATTRIBUTE_ID);
//      }
//
//      if (bdeu_String::areEqualCaseless("salary",
//                                        attributeName,
//                                        attributeNameLength)) {
//          return bdeat_sequenceManipulateAttribute(
//                                            object,
//                                            manipulator,
//                                            MySequence::SALARY_ATTRIBUTE_ID);
//      }
//
//      return NOT_FOUND;
//  }
//
//  template <typename MANIPULATOR>
//  int mine::bdeat_sequenceManipulateAttribute(MySequence   *object,
//                                              MANIPULATOR&  manipulator,
//                                              int           attributeId)
//  {
//      enum { NOT_FOUND = -1 };
//
//      switch (attributeId) {
//        case MySequence::NAME_ATTRIBUTE_ID: {
//          bdeat_AttributeInfo info;
//
//          info.annotation()     = "Name of employee";
//          info.formattingMode() = bdeat_FormattingMode::DEFAULT;
//          info.id()             = MySequence::NAME_ATTRIBUTE_ID;
//          info.name()           = "name";
//          info.nameLength()     = 4;
//
//          return manipulator(&object->d_name, info);
//        }
//        case MySequence::AGE_ATTRIBUTE_ID: {
//          bdeat_AttributeInfo info;
//
//          info.annotation()     = "Age of employee";
//          info.formattingMode() = bdeat_FormattingMode::DEFAULT;
//          info.id()             = MySequence::AGE_ATTRIBUTE_ID;
//          info.name()           = "age";
//          info.nameLength()     = 3;
//
//          return manipulator(&object->d_age, info);
//        }
//        case MySequence::SALARY_ATTRIBUTE_ID: {
//          bdeat_AttributeInfo info;
//
//          info.annotation()     = "Salary of employee";
//          info.formattingMode() = bdeat_FormattingMode::DEFAULT;
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
//  int mine::bdeat_sequenceManipulateAttributes(MySequence   *object,
//                                               MANIPULATOR&  manipulator)
//  {
//      int retVal;
//
//      retVal = bdeat_sequenceManipulateAttribute(
//                                              object,
//                                              manipulator,
//                                              MySequence::NAME_ATTRIBUTE_ID);
//
//      if (0 != retVal) {
//          return retVal;
//      }
//
//      retVal = bdeat_sequenceManipulateAttribute(
//                                               object,
//                                               manipulator,
//                                               MySequence::AGE_ATTRIBUTE_ID);
//
//      if (0 != retVal) {
//          return retVal;
//      }
//
//      retVal = bdeat_sequenceManipulateAttribute(
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
//  int mine::bdeat_sequenceAccessAttribute(
//                                      const MySequence&  object,
//                                      ACCESSOR&          accessor,
//                                      const char        *attributeName,
//                                      int                attributeNameLength)
//  {
//      enum { NOT_FOUND = -1 };
//
//      if (bdeu_String::areEqualCaseless("name",
//                                        attributeName,
//                                        attributeNameLength)) {
//          return bdeat_sequenceAccessAttribute(
//                                              object,
//                                              accessor,
//                                              MySequence::NAME_ATTRIBUTE_ID);
//      }
//
//      if (bdeu_String::areEqualCaseless("age",
//                                        attributeName,
//                                        attributeNameLength)) {
//          return bdeat_sequenceAccessAttribute(object,
//                                               accessor,
//                                               MySequence::AGE_ATTRIBUTE_ID);
//      }
//
//      if (bdeu_String::areEqualCaseless("salary",
//                                        attributeName,
//                                        attributeNameLength)) {
//          return bdeat_sequenceAccessAttribute(
//                                            object,
//                                            accessor,
//                                            MySequence::SALARY_ATTRIBUTE_ID);
//      }
//
//      return NOT_FOUND;
//  }
//
//  template <typename ACCESSOR>
//  int mine::bdeat_sequenceAccessAttribute(const MySequence& object,
//                                          ACCESSOR&         accessor,
//                                          int               attributeId)
//  {
//      enum { NOT_FOUND = -1 };
//
//      switch (attributeId) {
//        case MySequence::NAME_ATTRIBUTE_ID: {
//          bdeat_AttributeInfo info;
//
//          info.annotation()     = "Name of employee";
//          info.formattingMode() = bdeat_FormattingMode::DEFAULT;
//          info.id()             = MySequence::NAME_ATTRIBUTE_ID;
//          info.name()           = "name";
//          info.nameLength()     = 4;
//
//          return accessor(object.d_name, info);
//        }
//        case MySequence::AGE_ATTRIBUTE_ID: {
//          bdeat_AttributeInfo info;
//
//          info.annotation()     = "Age of employee";
//          info.formattingMode() = bdeat_FormattingMode::DEFAULT;
//          info.id()             = MySequence::AGE_ATTRIBUTE_ID;
//          info.name()           = "age";
//          info.nameLength()     = 3;
//
//          return accessor(object.d_age, info);
//        }
//        case MySequence::SALARY_ATTRIBUTE_ID: {
//          bdeat_AttributeInfo info;
//
//          info.annotation()     = "Salary of employee";
//          info.formattingMode() = bdeat_FormattingMode::DEFAULT;
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
//  int mine::bdeat_sequenceAccessAttributes(const MySequence& object,
//                                           ACCESSOR&         accessor)
//  {
//      int retVal;
//
//      retVal = bdeat_sequenceAccessAttribute(object,
//                                             accessor,
//                                             MySequence::NAME_ATTRIBUTE_ID);
//
//      if (0 != retVal) {
//          return retVal;
//      }
//
//      retVal = bdeat_sequenceAccessAttribute(object,
//                                             accessor,
//                                             MySequence::AGE_ATTRIBUTE_ID);
//
//      if (0 != retVal) {
//          return retVal;
//      }
//
//      retVal = bdeat_sequenceAccessAttribute(object,
//                                            accessor,
//                                            MySequence::SALARY_ATTRIBUTE_ID);
//
//      return retVal;
//  }
//
//  bool mine::bdeat_sequenceHasAttribute(
//                                      const MySequence&  object,
//                                      const char        *attributeName,
//                                      int                attributeNameLength)
//  {
//      return bdeu_String::areEqualCaseless("name",
//                                           attributeName,
//                                           attributeNameLength)
//          || bdeu_String::areEqualCaseless("age",
//                                           attributeName,
//                                           attributeNameLength)
//          || bdeu_String::areEqualCaseless("salary",
//                                           attributeName,
//                                           attributeNameLength);
//  }
//
//  bool mine::bdeat_sequenceHasAttribute(const MySequence& object,
//                                        int               attributeId)
//  {
//      return MySequence::NAME_ATTRIBUTE_ID   == attributeId
//          || MySequence::AGE_ATTRIBUTE_ID    == attributeId
//          || MySequence::SALARY_ATTRIBUTE_ID == attributeId;
//  }
//..
// Finally, we need to specialize the 'IsSequence' meta-function in the
// 'bdeat_SequenceFunctions' namespace for the 'mine::MySequence' type.  This
// makes the 'bdeat' infrastructure recognize 'mine::MySequence' as a sequence
// abstraction:
//..
//  namespace bdeat_SequenceFunctions {
//
//      template <>
//      struct IsSequence<mine::MySequence> {
//          enum { VALUE = 1 };
//      };
//
//  }  // close namespace 'bdeat_SequenceFunctions'
//  }  // close namespace 'BloombergLP'
//..
// The 'bdeat' infrastructure (and any component that uses this infrastructure)
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
// Using the 'baexml_decoder' component, we can now load this XML data
// into a 'mine::MySequence' object:
//..
//  #include <baexml_decoder.h>
//
//  void decodeMySequenceFromXML(bsl::istream& inputData)
//  {
//      using namespace BloombergLP;
//
//      mine::MySequence object;
//
//      baexml_DecoderOptions options;
//      baexml_MiniReader     reader;
//      baexml_ErrorInfo      errInfo;
//
//      baexml_Decoder decoder(&options, &reader, &errInfo);
//      int result = decoder.decode(inputData, &object);
//
//      assert(0          == result);
//      assert("John Doe" == object.d_name);
//      assert(29         == object.d_age);
//      assert(12345.00   == object.d_salary);
//  }
//..
// Note that the 'bdeat' framework can be used for functionality other than
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
//      bdeat_SequenceFunctions::accessAttributes(object, accessor);
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

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BDEAT_TYPETRAITS
#include <bdeat_typetraits.h>
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
                      // namespace bdeat_SequenceFunctions
                      // =================================

namespace bdeat_SequenceFunctions {
    // This 'namespace' provides methods that expose "sequence" behavior for
    // "sequence" types.  See the component-level documentation for more
    // information.

    // META-FUNCTIONS
#ifndef BDE_OMIT_INTERNAL_DEPRECATED

    template <typename TYPE>
    bslmf_MetaInt<0> isSequenceMetaFunction(const TYPE&);
        // This function can be overloaded to support partial specialization
        // (Sun5.2 compiler is unable to partially specialize the 'struct'
        // below).  Note that this function is has no definition and should not
        // be called at runtime.
        //
        // This function is *DEPRECATED*.  User's should specialize the
        // 'IsSequence' meta-function.

#endif // BDE_OMIT_INTERNAL_DEPRECATED
    template <typename TYPE>
    struct IsSequence {
        // This 'struct' should be specialized for third-party types that need
        // to expose "sequence" behavior.  See the component-level
        // documentation for further information.

        enum {
            VALUE = bslalg_HasTrait<TYPE, bdeat_TypeTraitBasicSequence>::VALUE
#ifndef BDE_OMIT_INTERNAL_DEPRECATED
                 || BSLMF_METAINT_TO_BOOL(isSequenceMetaFunction(
                                                   bslmf_TypeRep<TYPE>::rep()))
#endif // BDE_OMIT_INTERNAL_DEPRECATED
        };
    };

    // MANIPULATORS
    template <typename TYPE, typename MANIPULATOR>
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


    template <typename TYPE, typename MANIPULATOR>
    int manipulateAttribute(TYPE         *object,
                            MANIPULATOR&  manipulator,
                            int           attributeId);
        // Invoke the specified 'manipulator' on the address of the
        // (modifiable) attribute indicated by the specified 'attributeId' of
        // the specified 'object', supplying 'manipulator' with the
        // corresponding attribute information structure.  Return non-zero
        // value if the attribute is not found, and the value returned from the
        // invocation of 'manipulator' otherwise.

    template <typename TYPE, typename MANIPULATOR>
    int manipulateAttributes(TYPE *object, MANIPULATOR& manipulator);
        // Invoke the specified 'manipulator' sequentially on the address of
        // each (modifiable) attribute of the specified 'object', supplying
        // 'manipulator' with the corresponding attribute information structure
        // until such invocation returns non-zero value.  Return the value
        // from the last invocation of 'manipulator' (i.e., the invocation that
        // terminated the sequence).

    // ACCESSORS
    template <typename TYPE, typename ACCESSOR>
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

    template <typename TYPE, typename ACCESSOR>
    int accessAttribute(const TYPE& object,
                        ACCESSOR&   accessor,
                        int         attributeId);
        // Invoke the specified 'accessor' on the attribute of the specified
        // 'object' with the given 'attributeId', supplying 'accessor' with the
        // corresponding attribute information structure.  Return non-zero if
        // the attribute is not found, and the value returned from the
        // invocation of 'accessor' otherwise.

    template <typename TYPE, typename ACCESSOR>
    int accessAttributes(const TYPE& object, ACCESSOR& accessor);
        // Invoke the specified 'accessor' sequentially on each attribute of
        // the specified 'object', supplying 'accessor' with the corresponding
        // attribute information structure until such invocation returns a
        // non-zero value.  Return the value from the last invocation of
        // 'accessor' (i.e., the invocation that terminated the sequence).

    template <typename TYPE>
    bool hasAttribute(const TYPE&  object,
                      const char  *attributeName,
                      int          attributeNameLength);
        // Return true if the specified 'object' has an attribute with the
        // specified 'attributeName' of the specified 'attributeNameLength',
        // and false otherwise.

    template <typename TYPE>
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
    template <typename TYPE, typename MANIPULATOR>
    int bdeat_sequenceManipulateAttribute(TYPE         *object,
                                          MANIPULATOR&  manipulator,
                                          const char   *attributeName,
                                          int           attributeNameLength);
    template <typename TYPE, typename MANIPULATOR>
    int bdeat_sequenceManipulateAttribute(TYPE         *object,
                                          MANIPULATOR&  manipulator,
                                          int           attributeId);
    template <typename TYPE, typename MANIPULATOR>
    int bdeat_sequenceManipulateAttributes(TYPE         *object,
                                           MANIPULATOR&  manipulator);

    // ACCESSORS
    template <typename TYPE, typename ACCESSOR>
    int bdeat_sequenceAccessAttribute(const TYPE&  object,
                                      ACCESSOR&    accessor,
                                      const char  *attributeName,
                                      int          attributeNameLength);
    template <typename TYPE, typename ACCESSOR>
    int bdeat_sequenceAccessAttribute(const TYPE& object,
                                      ACCESSOR&   accessor,
                                      int         attributeId);
    template <typename TYPE, typename ACCESSOR>
    int bdeat_sequenceAccessAttributes(const TYPE& object, ACCESSOR& accessor);
    template <typename TYPE>
    bool bdeat_sequenceHasAttribute(const TYPE&  object,
                                    const char  *attributeName,
                                    int          attributeNameLength);
    template <typename TYPE>
    bool bdeat_sequenceHasAttribute(const TYPE& object,
                                    int         attributeId);
#endif

}  // close namespace bdeat_SequenceFunctions

// ===========================================================================
//                      INLINE FUNCTION DEFINITIONS
// ===========================================================================

                     // ---------------------------------
                     // namespace bdeat_SequenceFunctions
                     // ---------------------------------

// MANIPULATORS

template <typename TYPE, typename MANIPULATOR>
inline
int bdeat_SequenceFunctions::manipulateAttribute(
                                             TYPE         *object,
                                             MANIPULATOR&  manipulator,
                                             const char   *attributeName,
                                             int           attributeNameLength)
{
    return bdeat_sequenceManipulateAttribute(object,
                                             manipulator,
                                             attributeName,
                                             attributeNameLength);
}


template <typename TYPE, typename MANIPULATOR>
inline
int bdeat_SequenceFunctions::manipulateAttribute(TYPE         *object,
                                                 MANIPULATOR&  manipulator,
                                                 int           attributeId)
{
    return bdeat_sequenceManipulateAttribute(object, manipulator, attributeId);
}

template <typename TYPE, typename MANIPULATOR>
inline
int bdeat_SequenceFunctions::manipulateAttributes(TYPE         *object,
                                                  MANIPULATOR&  manipulator)
{
    return bdeat_sequenceManipulateAttributes(object, manipulator);
}

// ACCESSORS

template <typename TYPE, typename ACCESSOR>
inline
int bdeat_SequenceFunctions::accessAttribute(const TYPE&  object,
                                             ACCESSOR&    accessor,
                                             const char  *attributeName,
                                             int          attributeNameLength)
{
    return bdeat_sequenceAccessAttribute(object,
                                         accessor,
                                         attributeName,
                                         attributeNameLength);
}

template <typename TYPE, typename ACCESSOR>
inline
int bdeat_SequenceFunctions::accessAttribute(const TYPE& object,
                                             ACCESSOR&   accessor,
                                             int         attributeId)
{
    return bdeat_sequenceAccessAttribute(object, accessor, attributeId);
}

template <typename TYPE, typename ACCESSOR>
inline
int bdeat_SequenceFunctions::accessAttributes(const TYPE& object,
                                              ACCESSOR&   accessor)
{
    return bdeat_sequenceAccessAttributes(object, accessor);
}

template <typename TYPE>
inline
bool bdeat_SequenceFunctions::hasAttribute(const TYPE&  object,
                                           const char  *attributeName,
                                           int          attributeNameLength)
{
    return bdeat_sequenceHasAttribute(object,
                                      attributeName,
                                      attributeNameLength);
}

template <typename TYPE>
inline
bool bdeat_SequenceFunctions::hasAttribute(const TYPE& object,
                                           int         attributeId)
{
    return bdeat_sequenceHasAttribute(object, attributeId);
}

         // ----------------------------------------------------------
         // namespace bdeat_SequenceFunctions (OVERLOADABLE FUNCTIONS)
         // ----------------------------------------------------------

#if defined(BSLS_PLATFORM_CMP_IBM)
namespace bdeat_SequenceFunctions {
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
    int bdeat_sequenceManipulateAttribute(TYPE         *object,
                                          MANIPULATOR&  manipulator,
                                          const char   *attributeName,
                                          int           attributeNameLength);
    template <typename TYPE, typename MANIPULATOR>
    int bdeat_sequenceManipulateAttribute(TYPE         *object,
                                          MANIPULATOR&  manipulator,
                                          int           attributeId);
    template <typename TYPE, typename MANIPULATOR>
    int bdeat_sequenceManipulateAttributes(TYPE         *object,
                                           MANIPULATOR&  manipulator);

    // ACCESSORS
    template <typename TYPE, typename ACCESSOR>
    int bdeat_sequenceAccessAttribute(const TYPE&  object,
                                      ACCESSOR&    accessor,
                                      const char  *attributeName,
                                      int          attributeNameLength);
    template <typename TYPE, typename ACCESSOR>
    int bdeat_sequenceAccessAttribute(const TYPE& object,
                                      ACCESSOR&   accessor,
                                      int         attributeId);
    template <typename TYPE, typename ACCESSOR>
    int bdeat_sequenceAccessAttributes(const TYPE& object, ACCESSOR& accessor);
    template <typename TYPE>
    bool bdeat_sequenceHasAttribute(const TYPE&  object,
                                    const char  *attributeName,
                                    int          attributeNameLength);
    template <typename TYPE>
    bool bdeat_sequenceHasAttribute(const TYPE& object,
                                    int         attributeId);

}  // close namespace bdeat_SequenceFunctions
#endif

// MANIPULATORS

template <typename TYPE, typename MANIPULATOR>
inline
int bdeat_SequenceFunctions::bdeat_sequenceManipulateAttribute(
                                             TYPE         *object,
                                             MANIPULATOR&  manipulator,
                                             const char   *attributeName,
                                             int           attributeNameLength)
{
    BSLMF_ASSERT((bslalg_HasTrait<TYPE, bdeat_TypeTraitBasicSequence>::VALUE));

    return object->manipulateAttribute(manipulator,
                                       attributeName,
                                       attributeNameLength);
}

template <typename TYPE, typename MANIPULATOR>
inline
int bdeat_SequenceFunctions::bdeat_sequenceManipulateAttribute(
                                                     TYPE         *object,
                                                     MANIPULATOR&  manipulator,
                                                     int           attributeId)
{
    BSLMF_ASSERT((bslalg_HasTrait<TYPE, bdeat_TypeTraitBasicSequence>::VALUE));

    return object->manipulateAttribute(manipulator, attributeId);
}

template <typename TYPE, typename MANIPULATOR>
inline
int bdeat_SequenceFunctions::bdeat_sequenceManipulateAttributes(
                                                     TYPE         *object,
                                                     MANIPULATOR&  manipulator)
{
    BSLMF_ASSERT((bslalg_HasTrait<TYPE, bdeat_TypeTraitBasicSequence>::VALUE));

    return object->manipulateAttributes(manipulator);
}

// ACCESSORS

template <typename TYPE, typename ACCESSOR>
inline
int bdeat_SequenceFunctions::bdeat_sequenceAccessAttribute(
                                               const TYPE&  object,
                                               ACCESSOR&   accessor,
                                               const char *attributeName,
                                               int         attributeNameLength)
{
    BSLMF_ASSERT((bslalg_HasTrait<TYPE, bdeat_TypeTraitBasicSequence>::VALUE));

    return object.accessAttribute(accessor,
                                  attributeName,
                                  attributeNameLength);
}

template <typename TYPE, typename ACCESSOR>
inline
int bdeat_SequenceFunctions::bdeat_sequenceAccessAttribute(
                                                       const TYPE& object,
                                                       ACCESSOR&   accessor,
                                                       int         attributeId)
{
    BSLMF_ASSERT((bslalg_HasTrait<TYPE, bdeat_TypeTraitBasicSequence>::VALUE));

    return object.accessAttribute(accessor, attributeId);
}

template <typename TYPE, typename ACCESSOR>
inline
int bdeat_SequenceFunctions::bdeat_sequenceAccessAttributes(
                                                          const TYPE& object,
                                                          ACCESSOR&   accessor)
{
    BSLMF_ASSERT((bslalg_HasTrait<TYPE, bdeat_TypeTraitBasicSequence>::VALUE));

    return object.accessAttributes(accessor);
}

#ifdef BSLS_PLATFORM_CMP_MSVC
#pragma warning( push )
#pragma warning( disable : 4100 )
#endif
template <typename TYPE>
inline
bool bdeat_SequenceFunctions::bdeat_sequenceHasAttribute(
                                              const TYPE&  object,
                                              const char  *attributeName,
                                              int          attributeNameLength)
{
    BSLMF_ASSERT((bslalg_HasTrait<TYPE, bdeat_TypeTraitBasicSequence>::VALUE));

    return 0 != object.lookupAttributeInfo(attributeName, attributeNameLength);
}

template <typename TYPE>
inline
bool bdeat_SequenceFunctions::bdeat_sequenceHasAttribute(
                                                       const TYPE& object,
                                                       int         attributeId)
{
    BSLMF_ASSERT((bslalg_HasTrait<TYPE, bdeat_TypeTraitBasicSequence>::VALUE));

    return 0 != object.lookupAttributeInfo(attributeId);
}
#ifdef BSLS_PLATFORM_CMP_MSVC
#pragma warning( pop )
#endif

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
