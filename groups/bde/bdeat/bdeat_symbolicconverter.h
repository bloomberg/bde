// bdeat_symbolicconverter.h                  -*-C++-*-
#ifndef INCLUDED_BDEAT_SYMBOLICCONVERTER
#define INCLUDED_BDEAT_SYMBOLICCONVERTER

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a utility for convert types with matching member symbols.
//
//@CLASSES:
//  bdeat_SymbolicConverter: symbolic converter utility
//
//@SEE_ALSO:
//
//@AUTHOR: Shezan Baig (sbaig)
//
//@CONTACT: Rohan Bhindwale (rbhindwa)
//
//@DESCRIPTION: The 'bdeat_SymbolicConverter' utility provided by this
// component defines a single parameterized function 'convert'.  The 'convert'
// function takes two arguments: a destination and a source object.  The
// destination and source objects may be of different types.
//
// Each type can fall into one of the following categories:
//..
//  Category          Reference
//  --------          ---------
//  Sequence          bdeat_sequencefunctions
//  Choice            bdeat_choicefunctions
//  Array             bdeat_arrayfunctions
//  Enumeration       bdeat_enumfunctions
//  NullableValue     bdeat_nullablevaluefunctions
//  CustomizedType    bdeat_customizedtypefunctions
//  Simple            basic C++ fundamental types & other value-semantic types
//..
// The 'bdeat_SymbolicConverter' utility converts from one type to another
// using the following criteria:
//..
//  Destination Category  Source Category   Comments
//  --------------------  ---------------   --------
//  Sequence              Sequence          The conversion will fail if each
//                                          attribute in the set of attributes
//                                          from the source does not have a
//                                          corresponding attribute (with the
//                                          same name) from the source.  The
//                                          conversion will also fail if any
//                                          attributes from the source fail to
//                                          convert to the corresponding
//                                          attribute in the destination.  Any
//                                          attribute in the destination that
//                                          does not have a corresponding
//                                          attribute in the source will be set
//                                          to its default value.
//
//  Choice                Choice            The conversion will fail if the
//                                          destination does not have a
//                                          selection with the same name as the
//                                          current selection in the source.
//                                          The conversion will also fail if
//                                          the selection from the source fails
//                                          to convert to the corresponding
//                                          selection in the destination.  If
//                                          nothing is selected in the source,
//                                          then the destination will be reset.
//
//  Array                 Array             The conversion will fail if the
//                                          elements in the source fail to
//                                          convert to the elements in the
//                                          destination.  Upon completion, the
//                                          destination array will contain the
//                                          same number of elements as the
//                                          source array.
//
//  Enumeration           Enumeration       The conversion will fail if the
//                                          destination does not have a string
//                                          value that is identical to the
//                                          string value of the source.
//
//  Enumeration           char/short/int    The conversion will fail if the
//                                          destination does not have an
//                                          enumerator with the numeric value
//                                          of the source.
//
//  char/short/int        Enumeration       The conversion will fail if the
//                                          numeric value of the enumeration is
//                                          outside the bounds of the
//                                          destination type.
//
//  Enumeration           bsl::string       The conversion will fail if the
//                                          destination does not have an
//                                          enumerator with the symbolic string
//                                          name of the source.
//
//  bsl::string           Enumeration       This conversion always succeeds.
//
//  NullableValue         NullableValue     The conversion will fail if the
//                                          source has a value that fails to
//                                          convert to the destination value.
//                                          If the source is null, then the
//                                          destination is nulled.
//
//  NullableValue         AnyType           The conversion will fail if the
//                                          source fails to convert to the
//                                          destination value.
//
//  AnyType               NullableValue     The conversion will fail if the
//                                          source is not null and the value in
//                                          the source fails to convert to the
//                                          destination.  If the source is
//                                          null, then the destination will
//                                          be set to its default value.
//
//  CustomizedType        CustomizedType    The conversion will fail if the
//                                          base value in the source fails to
//                                          convert to the base value in the
//                                          destination and the base value is
//                                          able to convert to the customized
//                                          value.
//
//  CustomizedType        AnyType           The conversion will fail if the
//                                          source fails to convert to the base
//                                          value in the destination and the
//                                          base value is able to convert to
//                                          the customized value.
//
//  AnyType               CustomizedType    The conversion will fail if the
//                                          base value in the source fails to
//                                          convert to the destination.
//
//  SimpleType            SimpleType        The conversion will fail if there
//                                          is no accessible compile-time
//                                          assignment operator from the
//                                          destination to the source.  This is
//                                          determined using
//                                          'bslmf_isconvertible'.
//..
// Any other combination of destination and source categories will fail to
// convert.
//
///Usage
///-----
// The following snippets of code illustrate the usage of this component.  This
// component can be used with types supported by the 'bdeat' framework.  In
// particular, types generated by the 'bas_codegen.pl' tool can be used.  For
// example, suppose we have the following XML schema inside a file called
// 'xsdfile.xsd':
//..
//  <?xml version='1.0' encoding='UTF-8'?>
//  <xs:schema xmlns:xs='http://www.w3.org/2001/XMLSchema'
//             xmlns:bdem='http://bloomberg.com/schemas/bdem'
//             elementFormDefault='unqualified'>
//
//      <xs:complexType name='Employee'>
//          <xs:sequence>
//              <xs:element name='Name'   type='string'/>
//              <xs:element name='Dept'   type='string'/>
//              <xs:element name='Age'    type='int'/>
//              <xs:element name='Salary' type='float'/>
//          </xs:sequence>
//      </xs:complexType>
//
//      <xs:complexType name='Trainee'>
//          <xs:sequence>
//              <xs:element name='Name' type='string'/>
//              <xs:element name='Dept' type='string'/>
//              <xs:element name='Age'  type='int'/>
//          </xs:sequence>
//      </xs:complexType>
//
//  </xs:schema>
//..
// Using the 'bas_codegen.pl' tool, we can generate C++ classes for this
// schema:
//..
//  $ bas_codegen.pl -g h -g cpp -p test xsdfile.xsd
//..
// This tool will generate the header and implementation files for the
// 'test_employee' and 'test_trainee' components in the current directory.
//
// Now suppose we want to create a 'hireTrainee' function, that converts a
// trainee to an employee.  Such a function could be written as follows:
//..
//  #include <test_employee.h>
//  #include <test_trainee.h>
//
//  #include <bdeat_symbolicconverter.h>
//
//  using namespace BloombergLP;
//
//  int hireTrainee(test::Employee       *result,
//                  const test::Trainee&  trainee,
//                  float                 salary)
//  {
//      int retCode = bdeat_SymbolicConverter::convert(result, trainee);
//
//      result->salary() = salary;
//
//      return retCode;
//  }
//..
//  The 'hireTrainee' function can be used as follows:
//..
//  void usageExample()
//  {
//      test::Trainee trainee;
//
//      trainee.name() = "Bob";
//      trainee.dept() = "RnD";
//      trainee.age()  = 24;
//
//      test::Employee employee;
//
//      int result = hireTrainee(&employee, trainee, 20000.00f);
//
//      assert(0         == result);
//      assert("Bob"     == employee.name());
//      assert("RnD"     == employee.dept());
//      assert(24        == employee.age());
//      assert(20000.00f == employee.salary());
//  }
//..

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BDEAT_ARRAYFUNCTIONS
#include <bdeat_arrayfunctions.h>
#endif

#ifndef INCLUDED_BDEAT_CHOICEFUNCTIONS
#include <bdeat_choicefunctions.h>
#endif

#ifndef INCLUDED_BDEAT_CUSTOMIZEDTYPEFUNCTIONS
#include <bdeat_customizedtypefunctions.h>
#endif

#ifndef INCLUDED_BDEAT_ENUMFUNCTIONS
#include <bdeat_enumfunctions.h>
#endif

#ifndef INCLUDED_BDEAT_NULLABLEVALUEFUNCTIONS
#include <bdeat_nullablevaluefunctions.h>
#endif

#ifndef INCLUDED_BDEAT_SEQUENCEFUNCTIONS
#include <bdeat_sequencefunctions.h>
#endif

#ifndef INCLUDED_BDEAT_TYPECATEGORY
#include <bdeat_typecategory.h>
#endif

#ifndef INCLUDED_BDEAT_VALUETYPEFUNCTIONS
#include <bdeat_valuetypefunctions.h>
#endif

#ifndef INCLUDED_BDEU_PRINTMETHODS
#include <bdeu_printmethods.h>
#endif

#ifndef INCLUDED_BDEUT_STRINGREF
#include <bdeut_stringref.h>
#endif

#ifndef INCLUDED_BSLMF_SWITCH
#include <bslmf_switch.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSL_OSTREAM
#include <bsl_ostream.h>
#endif

#ifndef INCLUDED_BSL_STRING
#include <bsl_string.h>
#endif

namespace BloombergLP {

                       // ==============================
                       // struct bdeat_SymbolicConverter
                       // ==============================

struct bdeat_SymbolicConverter {
    // This utility contains a single 'convert' function that converts a value
    // from one type to another compatible type.

    template <typename LHS_TYPE, typename RHS_TYPE>
    static int convert(LHS_TYPE        *lhs,
                       const RHS_TYPE&  rhs);
    template <typename LHS_TYPE, typename RHS_TYPE>
    static int convert(LHS_TYPE        *lhs,
                       const RHS_TYPE&  rhs,
                       bsl::ostream&    errorStream);
        // Convert the value of the specified 'rhs' object to the specified
        // (modifiable) 'lhs' object.  Optionally specify an 'errorStream' to
        // print error messages.  Return 0 on success and a non-zero value
        // otherwise.  The supported conversions are described in the
        // 'bdeat_symbolicconverter' component-level documentation.
};

// ---  Anything below this line is implementation specific.  Do not use.  ----

                     // =================================
                     // class bdeat_SymbolicConverter_Imp
                     // =================================

class bdeat_SymbolicConverter_Imp {
    // This class contains implementation functions for this component.

    // PRIVATE DATA MEMBERS
    bsl::ostream *d_errorStream_p;  // held, not owned

  public:
    // IMPLEMENTATION MANIPULATORS
    template <typename LHS_TYPE, typename RHS_TYPE>
    int convert(LHS_TYPE                     *lhs,
                bdeat_TypeCategory::Sequence  lhsCategory,
                const RHS_TYPE&               rhs,
                bdeat_TypeCategory::Sequence  rhsCategory);
        // Convert to sequence from sequence.

    template <typename LHS_TYPE, typename RHS_TYPE>
    int convert(LHS_TYPE                   *lhs,
                bdeat_TypeCategory::Choice  lhsCategory,
                const RHS_TYPE&             rhs,
                bdeat_TypeCategory::Choice  rhsCategory);
        // Convert to choice from choice.

    template <typename LHS_TYPE, typename RHS_TYPE>
    int convert(LHS_TYPE                  *lhs,
                bdeat_TypeCategory::Array  lhsCategory,
                const RHS_TYPE&            rhs,
                bdeat_TypeCategory::Array  rhsCategory);
        // Convert to array from array.

    template <typename LHS_TYPE, typename RHS_TYPE>
    int convert(LHS_TYPE                        *lhs,
                bdeat_TypeCategory::Enumeration  lhsCategory,
                const RHS_TYPE&                  rhs,
                bdeat_TypeCategory::Enumeration  rhsCategory);
        // Convert to enum from enum.

    template <typename LHS_TYPE, typename RHS_TYPE>
    int convert(LHS_TYPE                        *lhs,
                bdeat_TypeCategory::Enumeration  lhsCategory,
                const RHS_TYPE&                  rhs,
                bdeat_TypeCategory::Simple       rhsCategory);
        // Convert to enum from simple type.

    template <typename LHS_TYPE, typename RHS_TYPE>
    int convert(LHS_TYPE                        *lhs,
                bdeat_TypeCategory::Simple       lhsCategory,
                const RHS_TYPE&                  rhs,
                bdeat_TypeCategory::Enumeration  rhsCategory);
        // Convert to simple type from enum.

    template <typename LHS_TYPE, typename RHS_TYPE>
    int convert(LHS_TYPE                          *lhs,
                bdeat_TypeCategory::NullableValue  lhsCategory,
                const RHS_TYPE&                    rhs,
                bdeat_TypeCategory::NullableValue  rhsCategory);
        // Convert to nullable from nullable.

    template <typename LHS_TYPE, typename RHS_TYPE, typename RHS_CATEGORY>
    int convert(LHS_TYPE                          *lhs,
                bdeat_TypeCategory::NullableValue  lhsCategory,
                const RHS_TYPE&                    rhs,
                RHS_CATEGORY                       rhsCategory);
        // Convert to nullable from non-nullable.

    template <typename LHS_TYPE, typename LHS_CATEGORY, typename RHS_TYPE>
    int convert(LHS_TYPE                          *lhs,
                LHS_CATEGORY                       lhsCategory,
                const RHS_TYPE&                    rhs,
                bdeat_TypeCategory::NullableValue  rhsCategory);
        // Convert to non-nullable from nullable.

    template <typename LHS_TYPE, typename RHS_TYPE>
    int convert(LHS_TYPE                           *lhs,
                bdeat_TypeCategory::CustomizedType  lhsCategory,
                const RHS_TYPE&                     rhs,
                bdeat_TypeCategory::NullableValue   rhsCategory);
        // Convert to customized type from nullable.  Note that this overload
        // is required to resolve ambiguities when there are nullable and
        // customized types in the same sequence.

    template <typename LHS_TYPE, typename RHS_TYPE>
    int convert(LHS_TYPE                           *lhs,
                bdeat_TypeCategory::CustomizedType  lhsCategory,
                const RHS_TYPE&                     rhs,
                bdeat_TypeCategory::CustomizedType  rhsCategory);
        // Convert to customized from customized.

    template <typename LHS_TYPE, typename RHS_TYPE, typename RHS_CATEGORY>
    int convert(LHS_TYPE                           *lhs,
                bdeat_TypeCategory::CustomizedType  lhsCategory,
                const RHS_TYPE&                     rhs,
                RHS_CATEGORY                        rhsCategory);
        // Convert to customized from non-customized.

    template <typename LHS_TYPE, typename LHS_CATEGORY, typename RHS_TYPE>
    int convert(LHS_TYPE                           *lhs,
                LHS_CATEGORY                        lhsCategory,
                const RHS_TYPE&                     rhs,
                bdeat_TypeCategory::CustomizedType  rhsCategory);
        // Convert to non-customized from customized.

    template <typename LHS_TYPE, typename RHS_TYPE>
    int convert(LHS_TYPE                           *lhs,
                bdeat_TypeCategory::NullableValue   lhsCategory,
                const RHS_TYPE&                     rhs,
                bdeat_TypeCategory::CustomizedType  rhsCategory);
        // Convert to nullable from customized.  Note that this overload is
        // required to resolve ambiguities when there are nullable and
        // customized types in the same sequence.

    template <typename LHS_TYPE, typename RHS_TYPE>
    int convert(LHS_TYPE                   *lhs,
                bdeat_TypeCategory::Simple  lhsCategory,
                const RHS_TYPE&             rhs,
                bdeat_TypeCategory::Simple  rhsCategory);
        // Convert to simple from simple of the same type.  Note that this just
        // evaluates to an assignment using the assignment operator.

    template <typename LHS_TYPE, typename LHS_CATEGORY,
              typename RHS_TYPE, typename RHS_CATEGORY>
    int convert(LHS_TYPE        *lhs,
                LHS_CATEGORY     lhsCategory,
                const RHS_TYPE&  rhs,
                RHS_CATEGORY     rhsCategory);
        // No match found.  This function does nothing, it just returns a
        // FAILURE code (to be used to detect type-mismatch at runtime).

    template <typename LHS_TYPE, typename RHS_TYPE>
    int resolveDynamicTypes(LHS_TYPE                        *lhs,
                            bdeat_TypeCategory::DynamicType  lhsCategory,
                            const RHS_TYPE&                  rhs,
                            bdeat_TypeCategory::DynamicType  rhsCategory);
    template <typename LHS_TYPE,
              typename RHS_TYPE, typename RHS_CATEGORY>
    int resolveDynamicTypes(LHS_TYPE                        *lhs,
                            bdeat_TypeCategory::DynamicType  lhsCategory,
                            const RHS_TYPE&                  rhs,
                            RHS_CATEGORY                     rhsCategory);
    template <typename LHS_TYPE, typename LHS_CATEGORY,
              typename RHS_TYPE>
    int resolveDynamicTypes(LHS_TYPE                        *lhs,
                            LHS_CATEGORY                     lhsCategory,
                            const RHS_TYPE&                  rhs,
                            bdeat_TypeCategory::DynamicType  rhsCategory);
    template <typename LHS_TYPE, typename LHS_CATEGORY,
              typename RHS_TYPE, typename RHS_CATEGORY>
    int resolveDynamicTypes(LHS_TYPE        *lhs,
                            LHS_CATEGORY     lhsCategory,
                            const RHS_TYPE&  rhs,
                            RHS_CATEGORY     rhsCategory);
        // Resolve dynamic types.

  private:
    // NOT IMPLEMENTED
    bdeat_SymbolicConverter_Imp(const bdeat_SymbolicConverter_Imp&);
    bdeat_SymbolicConverter_Imp& operator=(const bdeat_SymbolicConverter_Imp&);

  public:
    // CREATORS
    bdeat_SymbolicConverter_Imp(bsl::ostream *errorStream);
        // Create the imp object.

    // ~bdeat_SymbolicConverter_Imp();
        // Destroy this object.  Note that this trivial destructor is generated
        // by the compiler.

    // MANIPULATORS
    template <typename LHS_TYPE, typename RHS_TYPE>
    int convert(LHS_TYPE        *lhs,
                const RHS_TYPE&  rhs);
        // Implementation for convert function.

    bsl::ostream& errorStream();
        // Return a reference to the error stream.
};

           // =====================================================
           // class bdeat_SymbolicConverter_StoreValue<LVALUE_TYPE>
           // =====================================================

template <typename LVALUE_TYPE>
class bdeat_SymbolicConverter_StoreValue {
    // This visitor assigns the value of the visited member to
    // 'd_destination_p'.

    // PRIVATE DATA MEMBERS
    LVALUE_TYPE                 *d_destination_p;  // held, not owned
    bdeat_SymbolicConverter_Imp *d_imp_p;          // held, not owned

  public:
    // CREATORS
    explicit bdeat_SymbolicConverter_StoreValue(
                                      LVALUE_TYPE                 *destination,
                                      bdeat_SymbolicConverter_Imp *imp);

    // ACCESSORS
    template <typename RVALUE_TYPE, typename INFO_TYPE>
    int operator()(const RVALUE_TYPE& object,
                   const INFO_TYPE&) const;
        // Assign 'object' to '*d_lValue_p'.

    template <typename RVALUE_TYPE>
    int operator()(const RVALUE_TYPE& object) const;
        // Assign 'object' to '*d_lValue_p'.
};

            // ====================================================
            // class bdeat_SymbolicConverter_LoadValue<RVALUE_TYPE>
            // ====================================================

template <typename RVALUE_TYPE>
class bdeat_SymbolicConverter_LoadValue {
    // This visitor assigns 'd_value' to the visited member.

    // PRIVATE DATA MEMBERS
    bdeat_SymbolicConverter_Imp *d_imp_p;  // held, not owned
    const RVALUE_TYPE&           d_value;  // held, not owned

  public:
    // CREATORS
    explicit bdeat_SymbolicConverter_LoadValue(
                                            const RVALUE_TYPE&           value,
                                            bdeat_SymbolicConverter_Imp *imp);

    // ACCESSORS
    template <typename LVALUE_TYPE, typename INFO_TYPE>
    int operator()(LVALUE_TYPE *object,
                   const INFO_TYPE&) const;
        // Assign 'd_value' to '*object'.

    template <typename LVALUE_TYPE>
    int operator()(LVALUE_TYPE *object) const;
        // Assign 'd_value' to '*object'.
};

        // ============================================================
        // class bdeat_SymbolicConverter_StoreInSequence<SEQUENCE_TYPE>
        // ============================================================

template <typename SEQUENCE_TYPE>
class bdeat_SymbolicConverter_StoreInSequence {
    // This visitor is used when assigning to a sequence.  It will visit each
    // member from the source object.  Each time a member is visited, it will
    // use the 'LoadValue' visitor template to visit the member with the same
    // name in the destination sequence.  This will cause the value of the
    // member in the source object to be assigned to the member (with the same
    // name) of the destination sequence object.

    // PRIVATE DATA MEMBERS
    SEQUENCE_TYPE               *d_destination_p;  // held, not owned
    bdeat_SymbolicConverter_Imp *d_imp_p;          // held, not owned

  public:
    // CREATORS
    explicit bdeat_SymbolicConverter_StoreInSequence(
                                      SEQUENCE_TYPE               *destination,
                                      bdeat_SymbolicConverter_Imp *imp);

    // ACCESSORS
    template <typename SOURCE_MEMBER_TYPE, typename INFO_TYPE>
    int operator()(const SOURCE_MEMBER_TYPE& sourceMember,
                   const INFO_TYPE&          info) const;
};

          // ========================================================
          // class bdeat_SymbolicConverter_StoreInChoice<CHOICE_TYPE>
          // ========================================================

template <typename CHOICE_TYPE>
class bdeat_SymbolicConverter_StoreInChoice {
    // Similar to 'StoreInSequence' but this is for choice.

    // PRIVATE DATA MEMBERS
    CHOICE_TYPE                 *d_destination_p;  // held, not owned
    bdeat_SymbolicConverter_Imp *d_imp_p;          // held, not owned

  public:
    // CREATORS
    explicit bdeat_SymbolicConverter_StoreInChoice(
                                      CHOICE_TYPE                 *destination,
                                      bdeat_SymbolicConverter_Imp *imp);

    // ACCESSORS
    template <typename SOURCE_MEMBER_TYPE, typename INFO_TYPE>
    int operator()(const SOURCE_MEMBER_TYPE& sourceMember,
                   const INFO_TYPE&          info) const;
};

       // =============================================================
       // class bdeat_SymbolicConverter_StoreInArrayElement<ARRAY_TYPE>
       // =============================================================

template <typename ARRAY_TYPE>
class bdeat_SymbolicConverter_StoreInArrayElement {
    // Assign the value of the visited object to the 'd_index'th element inside
    // 'd_array_p'.

    // PRIVATE DATA MEMBERS
    ARRAY_TYPE                  *d_array_p;  // held, not owned
    bdeat_SymbolicConverter_Imp *d_imp_p;    // held, not owned
    int                          d_index;    // element index to assign to

  public:
    // CREATORS
    bdeat_SymbolicConverter_StoreInArrayElement(
                                            ARRAY_TYPE                  *array,
                                            int                          index,
                                            bdeat_SymbolicConverter_Imp *imp);

    // ACCESSORS
    template <typename SOURCE_ELEMENT_TYPE>
    int operator()(const SOURCE_ELEMENT_TYPE& sourceElement) const;
};

        // ============================================================
        // class bdeat_SymbolicConverter_StoreInNullable<NULLABLE_TYPE>
        // ============================================================

template <typename NULLABLE_TYPE>
class bdeat_SymbolicConverter_StoreInNullable {
    // Similar to 'StoreInSequence' but this is for nullable.

    // PRIVATE DATA MEMBERS
    NULLABLE_TYPE               *d_destination_p;  // held, not owned
    bdeat_SymbolicConverter_Imp *d_imp_p;          // held, not owned

  public:
    // CREATORS
    explicit bdeat_SymbolicConverter_StoreInNullable(
                                      NULLABLE_TYPE               *destination,
                                      bdeat_SymbolicConverter_Imp *imp);

    // ACCESSORS
    template <typename VALUE_TYPE>
    int operator()(const VALUE_TYPE& value) const;
};

// ===========================================================================
//                               PROXY CLASSES
// ===========================================================================

         // =========================================================
         // struct bdeat_SymbolicConverter_Imp_resolveDynamicRhsProxy
         // =========================================================

template <typename LHS_TYPE, typename LHS_CATEGORY>
struct bdeat_SymbolicConverter_Imp_resolveDynamicRhsProxy {
    // Component-private struct.  Do not use.

    // DATA MEMBERS
    bdeat_SymbolicConverter_Imp *d_instance_p;
    LHS_TYPE                    *d_lhs_p;

    // CREATORS
    // Creators have been omitted to allow simple static initialization of
    // this struct.

    // FUNCTIONS
    template <typename TYPE>
    inline
    int operator()(const TYPE&, bslmf_Nil)
    {
        BSLS_ASSERT_SAFE(0);
        return -2;
    }

    template <typename TYPE, typename ANY_CATEGORY>
    inline
    int operator()(const TYPE& object, ANY_CATEGORY category)
    {
        return d_instance_p->resolveDynamicTypes(d_lhs_p,
                                                 LHS_CATEGORY(),
                                                 object,
                                                 category);
    }
};

         // =========================================================
         // struct bdeat_SymbolicConverter_Imp_resolveDynamicLhsProxy
         // =========================================================

template <typename RHS_TYPE, typename RHS_CATEGORY>
struct bdeat_SymbolicConverter_Imp_resolveDynamicLhsProxy {
    // Component-private struct.  Do not use.

    // DATA MEMBERS
    bdeat_SymbolicConverter_Imp *d_instance_p;
    const RHS_TYPE              *d_rhs_p;

    // CREATORS
    // Creators have been omitted to allow simple static initialization of
    // this struct.

    // FUNCTIONS
    template <typename TYPE>
    inline
    int operator()(TYPE *object, bslmf_Nil)
    {
        BSLS_ASSERT_SAFE(0);
        return -3;
    }

    template <typename TYPE, typename ANY_CATEGORY>
    inline
    int operator()(TYPE *object, ANY_CATEGORY category)
    {
        return d_instance_p->resolveDynamicTypes(object,
                                                 category,
                                                 *d_rhs_p,
                                                 RHS_CATEGORY());
    }
};

// ===========================================================================
//                      INLINE FUNCTION DEFINITIONS
// ===========================================================================

                     // ---------------------------------
                     // class bdeat_SymbolicConverter_Imp
                     // ---------------------------------

// PRIVATE MANIPULATORS

template <typename LHS_TYPE, typename RHS_TYPE>
inline
int bdeat_SymbolicConverter_Imp::convert(LHS_TYPE                      *lhs,
                                         bdeat_TypeCategory::Sequence,
                                         const RHS_TYPE&                rhs,
                                         bdeat_TypeCategory::Sequence)
{
    bdeat_SymbolicConverter_StoreInSequence<LHS_TYPE> storeInLhs(lhs, this);

    return bdeat_SequenceFunctions::accessAttributes(rhs, storeInLhs);
}

template <typename LHS_TYPE, typename RHS_TYPE>
int bdeat_SymbolicConverter_Imp::convert(LHS_TYPE                    *lhs,
                                         bdeat_TypeCategory::Choice,
                                         const RHS_TYPE&              rhs,
                                         bdeat_TypeCategory::Choice)
{
    enum { BDEAT_SUCCESS = 0 };

    bdeat_SymbolicConverter_StoreInChoice<LHS_TYPE> storeInLhs(lhs, this);

    if (bdeat_ChoiceFunctions::BDEAT_UNDEFINED_SELECTION_ID
                                  == bdeat_ChoiceFunctions::selectionId(rhs)) {
        bdeat_ValueTypeFunctions::reset(lhs);

        return BDEAT_SUCCESS;
    }

    return bdeat_ChoiceFunctions::accessSelection(rhs, storeInLhs);
}

template <typename LHS_TYPE, typename RHS_TYPE>
int bdeat_SymbolicConverter_Imp::convert(LHS_TYPE                   *lhs,
                                         bdeat_TypeCategory::Array,
                                         const RHS_TYPE&             rhs,
                                         bdeat_TypeCategory::Array)
{
    enum { BDEAT_SUCCESS = 0, BDEAT_FAILURE = -4 };

    const int size = static_cast<int>(bdeat_ArrayFunctions::size(rhs));

    bdeat_ArrayFunctions::resize(lhs, size);

    for (int i = 0; i < size; ++i) {
        bdeat_SymbolicConverter_StoreInArrayElement<LHS_TYPE>
                                                      storeInLhs(lhs, i, this);

        if (0 != bdeat_ArrayFunctions::accessElement(rhs, storeInLhs, i)) {
            return BDEAT_FAILURE;
        }
    }

    return BDEAT_SUCCESS;
}

template <typename LHS_TYPE, typename RHS_TYPE>
inline
int bdeat_SymbolicConverter_Imp::convert(LHS_TYPE                         *lhs,
                                         bdeat_TypeCategory::Enumeration,
                                         const RHS_TYPE&                   rhs,
                                         bdeat_TypeCategory::Enumeration)
{
    bsl::string str;

    bdeat_EnumFunctions::toString(&str, rhs);

    return bdeat_EnumFunctions::fromString(lhs,
                                           str.data(),
                                           static_cast<int>(str.length()));
}

template <typename LHS_TYPE, typename RHS_TYPE>
inline
int bdeat_SymbolicConverter_Imp::convert(LHS_TYPE                         *lhs,
                                         bdeat_TypeCategory::Enumeration,
                                         const RHS_TYPE&                   rhs,
                                         bdeat_TypeCategory::Simple)
{
    return bdeat_ValueTypeFunctions::assign(lhs, rhs);
}

template <typename LHS_TYPE, typename RHS_TYPE>
inline
int bdeat_SymbolicConverter_Imp::convert(LHS_TYPE                         *lhs,
                                         bdeat_TypeCategory::Simple,
                                         const RHS_TYPE&                   rhs,
                                         bdeat_TypeCategory::Enumeration)
{
    return bdeat_ValueTypeFunctions::assign(lhs, rhs);
}

template <typename LHS_TYPE, typename RHS_TYPE>
int bdeat_SymbolicConverter_Imp::convert(
                                       LHS_TYPE                           *lhs,
                                       bdeat_TypeCategory::NullableValue,
                                       const RHS_TYPE&                     rhs,
                                       bdeat_TypeCategory::NullableValue)
{
    enum { BDEAT_SUCCESS = 0 };

    if (bdeat_NullableValueFunctions::isNull(rhs)) {
        bdeat_ValueTypeFunctions::reset(lhs);

        return BDEAT_SUCCESS;
    }

    bdeat_SymbolicConverter_StoreInNullable<LHS_TYPE> storeInLhs(lhs, this);

    return bdeat_NullableValueFunctions::accessValue(rhs, storeInLhs);
}

template <typename LHS_TYPE, typename RHS_TYPE, typename RHS_CATEGORY>
inline
int bdeat_SymbolicConverter_Imp::convert(
                                       LHS_TYPE                           *lhs,
                                       bdeat_TypeCategory::NullableValue,
                                       const RHS_TYPE&                     rhs,
                                       RHS_CATEGORY)
{
    bdeat_SymbolicConverter_StoreInNullable<LHS_TYPE> storeInLhs(lhs, this);

    return storeInLhs(rhs);
}

template <typename LHS_TYPE, typename LHS_CATEGORY, typename RHS_TYPE>
int bdeat_SymbolicConverter_Imp::convert(
                                       LHS_TYPE                           *lhs,
                                       LHS_CATEGORY,
                                       const RHS_TYPE&                     rhs,
                                       bdeat_TypeCategory::NullableValue)
{
    enum { BDEAT_SUCCESS = 0 };

    if (bdeat_NullableValueFunctions::isNull(rhs)) {
        return BDEAT_SUCCESS;  // ignore the value and let '*lhs' contain its
                         // *default* value
    }

    bdeat_SymbolicConverter_StoreValue<LHS_TYPE> storeIntoLhs(lhs, this);

    return bdeat_NullableValueFunctions::accessValue(rhs, storeIntoLhs);
}

template <typename LHS_TYPE, typename RHS_TYPE>
int bdeat_SymbolicConverter_Imp::convert(
                                      LHS_TYPE                            *lhs,
                                      bdeat_TypeCategory::CustomizedType,
                                      const RHS_TYPE&                      rhs,
                                      bdeat_TypeCategory::NullableValue)
{
    enum { BDEAT_SUCCESS = 0 };

    if (bdeat_NullableValueFunctions::isNull(rhs)) {
        return BDEAT_SUCCESS;  // ignore the value and let '*lhs' contain its
                         // *default* value
    }

    bdeat_SymbolicConverter_StoreValue<LHS_TYPE> storeIntoLhs(lhs, this);

    return bdeat_NullableValueFunctions::accessValue(rhs, storeIntoLhs);
}

template <typename LHS_TYPE, typename RHS_TYPE>
inline
int bdeat_SymbolicConverter_Imp::convert(
                                      LHS_TYPE                            *lhs,
                                      bdeat_TypeCategory::CustomizedType,
                                      const RHS_TYPE&                      rhs,
                                      bdeat_TypeCategory::CustomizedType)
{
    return convert(lhs,
                   bdeat_CustomizedTypeFunctions::convertToBaseType(rhs));
}

template <typename LHS_TYPE, typename RHS_TYPE, typename RHS_CATEGORY>
int bdeat_SymbolicConverter_Imp::convert(
                                      LHS_TYPE                            *lhs,
                                      bdeat_TypeCategory::CustomizedType,
                                      const RHS_TYPE&                      rhs,
                                      RHS_CATEGORY)
{
    enum { BDEAT_FAILURE = -5 };

    typedef typename
    bdeat_CustomizedTypeFunctions::BaseType<LHS_TYPE>::Type LhsBaseType;

    LhsBaseType lhsBaseValue;

    if (0 != convert(&lhsBaseValue, rhs)) {
        return BDEAT_FAILURE;
    }

    return bdeat_CustomizedTypeFunctions::convertFromBaseType(lhs,
                                                              lhsBaseValue);
}

template <typename LHS_TYPE, typename LHS_CATEGORY, typename RHS_TYPE>
inline
int bdeat_SymbolicConverter_Imp::convert(
                                      LHS_TYPE                            *lhs,
                                      LHS_CATEGORY,
                                      const RHS_TYPE&                      rhs,
                                      bdeat_TypeCategory::CustomizedType)
{
    return convert(lhs,
                   bdeat_CustomizedTypeFunctions::convertToBaseType(rhs));
}

template <typename LHS_TYPE, typename RHS_TYPE>
inline
int bdeat_SymbolicConverter_Imp::convert(
                                      LHS_TYPE                            *lhs,
                                      bdeat_TypeCategory::NullableValue,
                                      const RHS_TYPE&                      rhs,
                                      bdeat_TypeCategory::CustomizedType)
{
    return convert(lhs,
                   bdeat_CustomizedTypeFunctions::convertToBaseType(rhs));
}

template <typename LHS_TYPE, typename RHS_TYPE>
inline
int bdeat_SymbolicConverter_Imp::convert(LHS_TYPE                    *lhs,
                                         bdeat_TypeCategory::Simple,
                                         const RHS_TYPE&              rhs,
                                         bdeat_TypeCategory::Simple)
{
    return bdeat_ValueTypeFunctions::assign(lhs, rhs);
}

template <typename LHS_TYPE, typename LHS_CATEGORY,
          typename RHS_TYPE, typename RHS_CATEGORY>
inline
int bdeat_SymbolicConverter_Imp::convert(LHS_TYPE        *lhs,
                                         LHS_CATEGORY,
                                         const RHS_TYPE&  rhs,
                                         RHS_CATEGORY)
{
    enum { BDEAT_FAILURE = -6 };

    (void)lhs;  // quell warning
    (void)rhs;  // quell warning
    return BDEAT_FAILURE;
}

template <typename LHS_TYPE, typename RHS_TYPE>
inline
int bdeat_SymbolicConverter_Imp::resolveDynamicTypes(
                                         LHS_TYPE                         *lhs,
                                         bdeat_TypeCategory::DynamicType,
                                         const RHS_TYPE&                   rhs,
                                         bdeat_TypeCategory::DynamicType)
{
    bdeat_SymbolicConverter_Imp_resolveDynamicRhsProxy<
                        LHS_TYPE,
                        bdeat_TypeCategory::DynamicType> proxy = { this, lhs };

    return bdeat_TypeCategoryUtil::accessByCategory(rhs, proxy);
}

template <typename LHS_TYPE,
          typename RHS_TYPE, typename RHS_CATEGORY>
inline
int bdeat_SymbolicConverter_Imp::resolveDynamicTypes(
                                          LHS_TYPE                        *lhs,
                                          bdeat_TypeCategory::DynamicType,
                                          const RHS_TYPE&                  rhs,
                                          RHS_CATEGORY)
{
    bdeat_SymbolicConverter_Imp_resolveDynamicLhsProxy<
                                          RHS_TYPE,
                                          RHS_CATEGORY> proxy = { this, &rhs };

    return bdeat_TypeCategoryUtil::manipulateByCategory(lhs, proxy);
}

template <typename LHS_TYPE, typename LHS_CATEGORY,
          typename RHS_TYPE>
inline
int bdeat_SymbolicConverter_Imp::resolveDynamicTypes(
                                         LHS_TYPE                         *lhs,
                                         LHS_CATEGORY,
                                         const RHS_TYPE&                   rhs,
                                         bdeat_TypeCategory::DynamicType)
{
    bdeat_SymbolicConverter_Imp_resolveDynamicRhsProxy<
                                           LHS_TYPE,
                                           LHS_CATEGORY> proxy = { this, lhs };

    return bdeat_TypeCategoryUtil::accessByCategory(rhs, proxy);
}

template <typename LHS_TYPE, typename LHS_CATEGORY,
          typename RHS_TYPE, typename RHS_CATEGORY>
inline
int bdeat_SymbolicConverter_Imp::resolveDynamicTypes(
                                                  LHS_TYPE        *lhs,
                                                  LHS_CATEGORY     lhsCategory,
                                                  const RHS_TYPE&  rhs,
                                                  RHS_CATEGORY     rhsCategory)
{
    return convert(lhs, lhsCategory, rhs, rhsCategory);
}

// CREATORS

inline
bdeat_SymbolicConverter_Imp::bdeat_SymbolicConverter_Imp(
                                                     bsl::ostream *errorStream)
: d_errorStream_p(errorStream)
{
    BSLS_ASSERT_SAFE(d_errorStream_p);
}

// MANIPULATORS

template <typename LHS_TYPE, typename RHS_TYPE>
inline
int bdeat_SymbolicConverter_Imp::convert(LHS_TYPE        *lhs,
                                         const RHS_TYPE&  rhs)
{
    typedef typename bdeat_TypeCategory::Select<LHS_TYPE>::Type LhsCategory;
    typedef typename bdeat_TypeCategory::Select<RHS_TYPE>::Type RhsCategory;

    return resolveDynamicTypes(lhs, LhsCategory(), rhs, RhsCategory());
}

inline
bsl::ostream& bdeat_SymbolicConverter_Imp::errorStream()
{
    return *d_errorStream_p;
}

           // -----------------------------------------------------
           // class bdeat_SymbolicConverter_StoreValue<LVALUE_TYPE>
           // -----------------------------------------------------

// CREATORS

template <typename LVALUE_TYPE>
inline
bdeat_SymbolicConverter_StoreValue<LVALUE_TYPE>::
bdeat_SymbolicConverter_StoreValue(LVALUE_TYPE                 *destination,
                                   bdeat_SymbolicConverter_Imp *imp)
: d_destination_p(destination)
, d_imp_p(imp)
{
}

// ACCESSORS

template <typename LVALUE_TYPE>
template <typename RVALUE_TYPE, typename INFO_TYPE>
inline
int bdeat_SymbolicConverter_StoreValue<LVALUE_TYPE>::operator()(
                                                     const RVALUE_TYPE& object,
                                                     const INFO_TYPE&) const
{
    return d_imp_p->convert(d_destination_p, object);
}

template <typename LVALUE_TYPE>
template <typename RVALUE_TYPE>
inline
int bdeat_SymbolicConverter_StoreValue<LVALUE_TYPE>::operator()(
                                               const RVALUE_TYPE& object) const
{
    return d_imp_p->convert(d_destination_p, object);
}

            // ----------------------------------------------------
            // class bdeat_SymbolicConverter_LoadValue<RVALUE_TYPE>
            // ----------------------------------------------------

// CREATORS

template <typename RVALUE_TYPE>
inline
bdeat_SymbolicConverter_LoadValue<RVALUE_TYPE>::
                    bdeat_SymbolicConverter_LoadValue(
                                            const RVALUE_TYPE&           value,
                                            bdeat_SymbolicConverter_Imp *imp)
: d_imp_p(imp)
, d_value(value)
{
}

// ACCESSORS

template <typename RVALUE_TYPE>
template <typename LVALUE_TYPE, typename INFO_TYPE>
inline
int bdeat_SymbolicConverter_LoadValue<RVALUE_TYPE>::operator()(
                                                        LVALUE_TYPE *object,
                                                        const INFO_TYPE&) const
{
    return d_imp_p->convert(object, d_value);
}

template <typename RVALUE_TYPE>
template <typename LVALUE_TYPE>
inline
int bdeat_SymbolicConverter_LoadValue<RVALUE_TYPE>::operator()(
                                                     LVALUE_TYPE *object) const
{
    return d_imp_p->convert(object, d_value);
}

        // ------------------------------------------------------------
        // class bdeat_SymbolicConverter_StoreInSequence<SEQUENCE_TYPE>
        // ------------------------------------------------------------

// CREATORS

template <typename SEQUENCE_TYPE>
inline
bdeat_SymbolicConverter_StoreInSequence<SEQUENCE_TYPE>::
bdeat_SymbolicConverter_StoreInSequence(
                                      SEQUENCE_TYPE               *destination,
                                      bdeat_SymbolicConverter_Imp *imp)
: d_destination_p(destination)
, d_imp_p(imp)
{
}

// ACCESSORS

template <typename SEQUENCE_TYPE>
template <typename SOURCE_MEMBER_TYPE, typename INFO_TYPE>
inline
int bdeat_SymbolicConverter_StoreInSequence<SEQUENCE_TYPE>::operator()(
                                        const SOURCE_MEMBER_TYPE& sourceMember,
                                        const INFO_TYPE&          info) const
{
    enum { BDEAT_SUCCESS = 0, BDEAT_FAILURE = -7 };

    bdeat_SymbolicConverter_LoadValue<SOURCE_MEMBER_TYPE> loadSourceValue(
                                                                  sourceMember,
                                                                  d_imp_p);

    if (0 != bdeat_SequenceFunctions::manipulateAttribute(d_destination_p,
                                                          loadSourceValue,
                                                          info.name(),
                                                          info.nameLength())) {
        d_imp_p->errorStream()
                             << "Failed to convert attribute '"
                             << bdeut_StringRef(info.name(), info.nameLength())
                             << "'" << bsl::endl;

        return BDEAT_FAILURE;
    }

    return BDEAT_SUCCESS;
}

          // --------------------------------------------------------
          // class bdeat_SymbolicConverter_StoreInChoice<CHOICE_TYPE>
          // --------------------------------------------------------

// CREATORS

template <typename CHOICE_TYPE>
inline
bdeat_SymbolicConverter_StoreInChoice<CHOICE_TYPE>::
bdeat_SymbolicConverter_StoreInChoice(CHOICE_TYPE                 *destination,
                                      bdeat_SymbolicConverter_Imp *imp)
: d_destination_p(destination)
, d_imp_p(imp)
{
}

// ACCESSORS

template <typename CHOICE_TYPE>
template <typename SOURCE_MEMBER_TYPE, typename INFO_TYPE>
int bdeat_SymbolicConverter_StoreInChoice<CHOICE_TYPE>::operator()(
                                        const SOURCE_MEMBER_TYPE& sourceMember,
                                        const INFO_TYPE&          info) const
{
    enum { BDEAT_SUCCESS = 0, BDEAT_FAILURE = -8 };

    // Make the selection.

    if (0 != bdeat_ChoiceFunctions::makeSelection(d_destination_p,
                                                  info.name(),
                                                  info.nameLength())) {
        d_imp_p->errorStream()
                             << "Failed to make selection '"
                             << bdeut_StringRef(info.name(), info.nameLength())
                             << "'" << bsl::endl;

        return BDEAT_FAILURE;
    }

    // Assign the value.

    bdeat_SymbolicConverter_LoadValue<SOURCE_MEMBER_TYPE> loadSourceValue(
                                                                  sourceMember,
                                                                  d_imp_p);

    if (0 != bdeat_ChoiceFunctions::manipulateSelection(d_destination_p,
                                                        loadSourceValue)) {
        d_imp_p->errorStream()
                             << "Failed to convert selection '"
                             << bdeut_StringRef(info.name(), info.nameLength())
                             << "'" << bsl::endl;

        return BDEAT_FAILURE;
    }

    return BDEAT_SUCCESS;
}

       // -------------------------------------------------------------
       // class bdeat_SymbolicConverter_StoreInArrayElement<ARRAY_TYPE>
       // -------------------------------------------------------------

// CREATORS

template <typename ARRAY_TYPE>
inline
bdeat_SymbolicConverter_StoreInArrayElement<ARRAY_TYPE>::
bdeat_SymbolicConverter_StoreInArrayElement(ARRAY_TYPE                  *array,
                                            int                          index,
                                            bdeat_SymbolicConverter_Imp *imp)
: d_array_p(array)
, d_imp_p(imp)
, d_index(index)
{
}

// ACCESSORS

template <typename ARRAY_TYPE>
template <typename SOURCE_ELEMENT_TYPE>
inline
int bdeat_SymbolicConverter_StoreInArrayElement<ARRAY_TYPE>::operator()(
                                const SOURCE_ELEMENT_TYPE& sourceElement) const
{
    bdeat_SymbolicConverter_LoadValue<SOURCE_ELEMENT_TYPE> loadSourceValue(
                                                                 sourceElement,
                                                                 d_imp_p);

    return bdeat_ArrayFunctions::manipulateElement(d_array_p,
                                                   loadSourceValue,
                                                   d_index);
}

        // ------------------------------------------------------------
        // class bdeat_SymbolicConverter_StoreInNullable<NULLABLE_TYPE>
        // ------------------------------------------------------------

// CREATORS

template <typename NULLABLE_TYPE>
inline
bdeat_SymbolicConverter_StoreInNullable<NULLABLE_TYPE>::
bdeat_SymbolicConverter_StoreInNullable(
                                      NULLABLE_TYPE               *destination,
                                      bdeat_SymbolicConverter_Imp *imp)
: d_destination_p(destination)
, d_imp_p(imp)
{
}

// ACCESSORS

template <typename NULLABLE_TYPE>
template <typename VALUE_TYPE>
inline
int bdeat_SymbolicConverter_StoreInNullable<NULLABLE_TYPE>::operator()(
                                                 const VALUE_TYPE& value) const
{
    bdeat_NullableValueFunctions::makeValue(d_destination_p);

    // Assign the value.

    bdeat_SymbolicConverter_LoadValue<VALUE_TYPE> loadSourceValue(value,
                                                                  d_imp_p);

    return bdeat_NullableValueFunctions::manipulateValue(d_destination_p,
                                                         loadSourceValue);
}

                       // ------------------------------
                       // struct bdeat_SymbolicConverter
                       // ------------------------------

template <typename LHS_TYPE, typename RHS_TYPE>
inline
int bdeat_SymbolicConverter::convert(LHS_TYPE        *lhs,
                                     const RHS_TYPE&  rhs)
{
    bsl::ostream nullStream(0);
    return bdeat_SymbolicConverter::convert(lhs, rhs, nullStream);
}

template <typename LHS_TYPE, typename RHS_TYPE>
inline
int bdeat_SymbolicConverter::convert(LHS_TYPE        *lhs,
                                     const RHS_TYPE&  rhs,
                                     bsl::ostream&    errorStream)
{
    bdeat_ValueTypeFunctions::reset(lhs);

    bdeat_SymbolicConverter_Imp imp(&errorStream);

    return imp.convert(lhs, rhs);
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
