// bdlat_symbolicconverter.h                                          -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BDLAT_SYMBOLICCONVERTER
#define INCLUDED_BDLAT_SYMBOLICCONVERTER

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a utility for convert types with matching member symbols.
//
//@CLASSES:
//  bdlat_SymbolicConverter: symbolic converter utility
//
//@SEE_ALSO:
//
//@DESCRIPTION: The 'bdlat_SymbolicConverter' utility provided by this
// component defines a single parameterized function 'convert'.  The 'convert'
// function takes two arguments: a destination and a source object.  The
// destination and source objects may be of different types.
//
// Each type can fall into one of the following categories:
//..
//  Category          Reference
//  --------          ---------
//  Sequence          bdlat_sequencefunctions
//  Choice            bdlat_choicefunctions
//  Array             bdlat_arrayfunctions
//  Enumeration       bdlat_enumfunctions
//  NullableValue     bdlat_nullablevaluefunctions
//  CustomizedType    bdlat_customizedtypefunctions
//  Simple            basic C++ fundamental types & other value-semantic types
//..
// The 'bdlat_SymbolicConverter' utility converts from one type to another
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
// component can be used with types supported by the 'bdlat' framework.  In
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
//  #include <bdlat_symbolicconverter.h>
//
//  using namespace BloombergLP;
//
//  int hireTrainee(test::Employee       *result,
//                  const test::Trainee&  trainee,
//                  float                 salary)
//  {
//      int retCode = bdlat_SymbolicConverter::convert(result, trainee);
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

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

#ifndef INCLUDED_BDLAT_ARRAYFUNCTIONS
#include <bdlat_arrayfunctions.h>
#endif

#ifndef INCLUDED_BDLAT_BDEATOVERRIDES
#include <bdlat_bdeatoverrides.h>
#endif

#ifndef INCLUDED_BDLAT_CHOICEFUNCTIONS
#include <bdlat_choicefunctions.h>
#endif

#ifndef INCLUDED_BDLAT_CUSTOMIZEDTYPEFUNCTIONS
#include <bdlat_customizedtypefunctions.h>
#endif

#ifndef INCLUDED_BDLAT_ENUMFUNCTIONS
#include <bdlat_enumfunctions.h>
#endif

#ifndef INCLUDED_BDLAT_NULLABLEVALUEFUNCTIONS
#include <bdlat_nullablevaluefunctions.h>
#endif

#ifndef INCLUDED_BDLAT_SEQUENCEFUNCTIONS
#include <bdlat_sequencefunctions.h>
#endif

#ifndef INCLUDED_BDLAT_TYPECATEGORY
#include <bdlat_typecategory.h>
#endif

#ifndef INCLUDED_BDLAT_VALUETYPEFUNCTIONS
#include <bdlat_valuetypefunctions.h>
#endif

#ifndef INCLUDED_BDLB_PRINTMETHODS
#include <bdlb_printmethods.h>
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
                       // struct bdlat_SymbolicConverter
                       // ==============================

struct bdlat_SymbolicConverter {
    // This utility contains a single 'convert' function that converts a value
    // from one type to another compatible type.

    template <class LHS_TYPE, class RHS_TYPE>
    static
    int convert(LHS_TYPE *lhs, const RHS_TYPE &rhs);
    template <class LHS_TYPE, class RHS_TYPE>
    static
    int convert(LHS_TYPE *lhs, const RHS_TYPE &rhs, bsl::ostream &errorStream);
        // Convert the value of the specified 'rhs' object to the specified
        // (modifiable) 'lhs' object.  Optionally specify an 'errorStream' to
        // print error messages.  Return 0 on success and a non-zero value
        // otherwise.  The supported conversions are described in the
        // 'bdlat_symbolicconverter' component-level documentation.
};

// ---  Anything below this line is implementation specific.  Do not use.  ----

                     // =================================
                     // class bdlat_SymbolicConverter_Imp
                     // =================================

class bdlat_SymbolicConverter_Imp {
    // This class contains implementation functions for this component.

    // PRIVATE DATA MEMBERS
    bsl::ostream *d_errorStream_p;  // held, not owned

  public:
    // IMPLEMENTATION MANIPULATORS
    template <class LHS_TYPE, class RHS_TYPE>
    int convert(LHS_TYPE                     *lhs,
                bdlat_TypeCategory::Sequence  lhsCategory,
                const RHS_TYPE&               rhs,
                bdlat_TypeCategory::Sequence  rhsCategory);
        // Convert to sequence from sequence.

    template <class LHS_TYPE, class RHS_TYPE>
    int convert(LHS_TYPE                   *lhs,
                bdlat_TypeCategory::Choice  lhsCategory,
                const RHS_TYPE&             rhs,
                bdlat_TypeCategory::Choice  rhsCategory);
        // Convert to choice from choice.

    template <class LHS_TYPE, class RHS_TYPE>
    int convert(LHS_TYPE                  *lhs,
                bdlat_TypeCategory::Array  lhsCategory,
                const RHS_TYPE&            rhs,
                bdlat_TypeCategory::Array  rhsCategory);
        // Convert to array from array.

    template <class LHS_TYPE, class RHS_TYPE>
    int convert(LHS_TYPE                        *lhs,
                bdlat_TypeCategory::Enumeration  lhsCategory,
                const RHS_TYPE&                  rhs,
                bdlat_TypeCategory::Enumeration  rhsCategory);
        // Convert to enum from enum.

    template <class LHS_TYPE, class RHS_TYPE>
    int convert(LHS_TYPE                        *lhs,
                bdlat_TypeCategory::Enumeration  lhsCategory,
                const RHS_TYPE&                  rhs,
                bdlat_TypeCategory::Simple       rhsCategory);
        // Convert to enum from simple type.

    template <class LHS_TYPE, class RHS_TYPE>
    int convert(LHS_TYPE                        *lhs,
                bdlat_TypeCategory::Simple       lhsCategory,
                const RHS_TYPE&                  rhs,
                bdlat_TypeCategory::Enumeration  rhsCategory);
        // Convert to simple type from enum.

    template <class LHS_TYPE, class RHS_TYPE>
    int convert(LHS_TYPE                          *lhs,
                bdlat_TypeCategory::NullableValue  lhsCategory,
                const RHS_TYPE&                    rhs,
                bdlat_TypeCategory::NullableValue  rhsCategory);
        // Convert to nullable from nullable.

    template <class LHS_TYPE, class RHS_TYPE, class RHS_CATEGORY>
    int convert(LHS_TYPE                          *lhs,
                bdlat_TypeCategory::NullableValue  lhsCategory,
                const RHS_TYPE&                    rhs,
                RHS_CATEGORY                       rhsCategory);
        // Convert to nullable from non-nullable.

    template <class LHS_TYPE, class LHS_CATEGORY, class RHS_TYPE>
    int convert(LHS_TYPE                          *lhs,
                LHS_CATEGORY                       lhsCategory,
                const RHS_TYPE&                    rhs,
                bdlat_TypeCategory::NullableValue  rhsCategory);
        // Convert to non-nullable from nullable.

    template <class LHS_TYPE, class RHS_TYPE>
    int convert(LHS_TYPE                           *lhs,
                bdlat_TypeCategory::CustomizedType  lhsCategory,
                const RHS_TYPE&                     rhs,
                bdlat_TypeCategory::NullableValue   rhsCategory);
        // Convert to customized type from nullable.  Note that this overload
        // is required to resolve ambiguities when there are nullable and
        // customized types in the same sequence.

    template <class LHS_TYPE, class RHS_TYPE>
    int convert(LHS_TYPE                           *lhs,
                bdlat_TypeCategory::CustomizedType  lhsCategory,
                const RHS_TYPE&                     rhs,
                bdlat_TypeCategory::CustomizedType  rhsCategory);
        // Convert to customized from customized.

    template <class LHS_TYPE, class RHS_TYPE, class RHS_CATEGORY>
    int convert(LHS_TYPE                           *lhs,
                bdlat_TypeCategory::CustomizedType  lhsCategory,
                const RHS_TYPE&                     rhs,
                RHS_CATEGORY                        rhsCategory);
        // Convert to customized from non-customized.

    template <class LHS_TYPE, class LHS_CATEGORY, class RHS_TYPE>
    int convert(LHS_TYPE                           *lhs,
                LHS_CATEGORY                        lhsCategory,
                const RHS_TYPE&                     rhs,
                bdlat_TypeCategory::CustomizedType  rhsCategory);
        // Convert to non-customized from customized.

    template <class LHS_TYPE, class RHS_TYPE>
    int convert(LHS_TYPE                           *lhs,
                bdlat_TypeCategory::NullableValue   lhsCategory,
                const RHS_TYPE&                     rhs,
                bdlat_TypeCategory::CustomizedType  rhsCategory);
        // Convert to nullable from customized.  Note that this overload is
        // required to resolve ambiguities when there are nullable and
        // customized types in the same sequence.

    template <class LHS_TYPE, class RHS_TYPE>
    int convert(LHS_TYPE                   *lhs,
                bdlat_TypeCategory::Simple  lhsCategory,
                const RHS_TYPE&             rhs,
                bdlat_TypeCategory::Simple  rhsCategory);
        // Convert to simple from simple of the same type.  Note that this just
        // evaluates to an assignment using the assignment operator.

    template <class LHS_TYPE,
              class LHS_CATEGORY,
              class RHS_TYPE,
              class RHS_CATEGORY>
    int convert(LHS_TYPE        *lhs,
                LHS_CATEGORY     lhsCategory,
                const RHS_TYPE&  rhs,
                RHS_CATEGORY     rhsCategory);
        // No match found.  This function does nothing, it just returns a
        // FAILURE code (to be used to detect type-mismatch at runtime).

    template <class LHS_TYPE, class RHS_TYPE>
    int resolveDynamicTypes(LHS_TYPE                        *lhs,
                            bdlat_TypeCategory::DynamicType  lhsCategory,
                            const RHS_TYPE&                  rhs,
                            bdlat_TypeCategory::DynamicType  rhsCategory);
    template <class LHS_TYPE,
              class RHS_TYPE,
              class RHS_CATEGORY>
    int resolveDynamicTypes(LHS_TYPE                        *lhs,
                            bdlat_TypeCategory::DynamicType  lhsCategory,
                            const RHS_TYPE&                  rhs,
                            RHS_CATEGORY                     rhsCategory);
    template <class LHS_TYPE,
              class LHS_CATEGORY,
              class RHS_TYPE>
    int resolveDynamicTypes(LHS_TYPE                        *lhs,
                            LHS_CATEGORY                     lhsCategory,
                            const RHS_TYPE&                  rhs,
                            bdlat_TypeCategory::DynamicType  rhsCategory);
    template <class LHS_TYPE,
              class LHS_CATEGORY,
              class RHS_TYPE,
              class RHS_CATEGORY>
    int resolveDynamicTypes(LHS_TYPE        *lhs,
                            LHS_CATEGORY     lhsCategory,
                            const RHS_TYPE&  rhs,
                            RHS_CATEGORY     rhsCategory);
        // Resolve dynamic types.

  private:
    // NOT IMPLEMENTED
    bdlat_SymbolicConverter_Imp(const bdlat_SymbolicConverter_Imp&);
    bdlat_SymbolicConverter_Imp& operator=(const bdlat_SymbolicConverter_Imp&);

  public:
    // CREATORS
    bdlat_SymbolicConverter_Imp(bsl::ostream *errorStream);
        // Create the imp object.

    // ~bdlat_SymbolicConverter_Imp();
        // Destroy this object.  Note that this trivial destructor is generated
        // by the compiler.

    // MANIPULATORS
    template <class LHS_TYPE, class RHS_TYPE>
    int convert(LHS_TYPE        *lhs,
                const RHS_TYPE&  rhs);
        // Implementation for convert function.

    bsl::ostream& errorStream();
        // Return a reference to the error stream.
};

           // =====================================================
           // class bdlat_SymbolicConverter_StoreValue<LVALUE_TYPE>
           // =====================================================

template <class LVALUE_TYPE>
class bdlat_SymbolicConverter_StoreValue {
    // This visitor assigns the value of the visited member to
    // 'd_destination_p'.

    // PRIVATE DATA MEMBERS
    LVALUE_TYPE                 *d_destination_p;  // held, not owned
    bdlat_SymbolicConverter_Imp *d_imp_p;          // held, not owned

  public:
    // CREATORS
    explicit bdlat_SymbolicConverter_StoreValue(
                                      LVALUE_TYPE                 *destination,
                                      bdlat_SymbolicConverter_Imp *imp);

    // ACCESSORS
    template <class RVALUE_TYPE, class INFO_TYPE>
    int operator()(const RVALUE_TYPE& object,
                   const INFO_TYPE&) const;
        // Assign the specified 'object' to '*d_lValue_p'.

    template <class RVALUE_TYPE>
    int operator()(const RVALUE_TYPE& object) const;
        // Assign the specified 'object' to '*d_lValue_p'.
};

            // ====================================================
            // class bdlat_SymbolicConverter_LoadValue<RVALUE_TYPE>
            // ====================================================

template <class RVALUE_TYPE>
class bdlat_SymbolicConverter_LoadValue {
    // This visitor assigns 'd_value' to the visited member.

    // PRIVATE DATA MEMBERS
    bdlat_SymbolicConverter_Imp *d_imp_p;  // held, not owned
    const RVALUE_TYPE&           d_value;  // held, not owned

  public:
    // CREATORS
    explicit bdlat_SymbolicConverter_LoadValue(
                                            const RVALUE_TYPE&           value,
                                            bdlat_SymbolicConverter_Imp *imp);

    // ACCESSORS
    template <class LVALUE_TYPE, class INFO_TYPE>
    int operator()(LVALUE_TYPE *object,
                   const INFO_TYPE&) const;
        // Assign 'd_value' to the specified '*object'.

    template <class LVALUE_TYPE>
    int operator()(LVALUE_TYPE *object) const;
        // Assign 'd_value' to the specified '*object'.
};

        // ============================================================
        // class bdlat_SymbolicConverter_StoreInSequence<SEQUENCE_TYPE>
        // ============================================================

template <class SEQUENCE_TYPE>
class bdlat_SymbolicConverter_StoreInSequence {
    // This visitor is used when assigning to a sequence.  It will visit each
    // member from the source object.  Each time a member is visited, it will
    // use the 'LoadValue' visitor template to visit the member with the same
    // name in the destination sequence.  This will cause the value of the
    // member in the source object to be assigned to the member (with the same
    // name) of the destination sequence object.

    // PRIVATE DATA MEMBERS
    SEQUENCE_TYPE               *d_destination_p;  // held, not owned
    bdlat_SymbolicConverter_Imp *d_imp_p;          // held, not owned

  public:
    // CREATORS
    explicit bdlat_SymbolicConverter_StoreInSequence(
                                      SEQUENCE_TYPE               *destination,
                                      bdlat_SymbolicConverter_Imp *imp);

    // ACCESSORS
    template <class SOURCE_MEMBER_TYPE, class INFO_TYPE>
    int operator()(const SOURCE_MEMBER_TYPE& sourceMember,
                   const INFO_TYPE&          info) const;
};

          // ========================================================
          // class bdlat_SymbolicConverter_StoreInChoice<CHOICE_TYPE>
          // ========================================================

template <class CHOICE_TYPE>
class bdlat_SymbolicConverter_StoreInChoice {
    // Similar to 'StoreInSequence' but this is for choice.

    // PRIVATE DATA MEMBERS
    CHOICE_TYPE                 *d_destination_p;  // held, not owned
    bdlat_SymbolicConverter_Imp *d_imp_p;          // held, not owned

  public:
    // CREATORS
    explicit bdlat_SymbolicConverter_StoreInChoice(
                                      CHOICE_TYPE                 *destination,
                                      bdlat_SymbolicConverter_Imp *imp);

    // ACCESSORS
    template <class SOURCE_MEMBER_TYPE, class INFO_TYPE>
    int operator()(const SOURCE_MEMBER_TYPE& sourceMember,
                   const INFO_TYPE&          info) const;
};

       // =============================================================
       // class bdlat_SymbolicConverter_StoreInArrayElement<ARRAY_TYPE>
       // =============================================================

template <class ARRAY_TYPE>
class bdlat_SymbolicConverter_StoreInArrayElement {
    // Assign the value of the visited object to the 'd_index'th element inside
    // 'd_array_p'.

    // PRIVATE DATA MEMBERS
    ARRAY_TYPE                  *d_array_p;  // held, not owned
    bdlat_SymbolicConverter_Imp *d_imp_p;    // held, not owned
    int                          d_index;    // element index to assign to

  public:
    // CREATORS
    bdlat_SymbolicConverter_StoreInArrayElement(
                                            ARRAY_TYPE                  *array,
                                            int                          index,
                                            bdlat_SymbolicConverter_Imp *imp);

    // ACCESSORS
    template <class SOURCE_ELEMENT_TYPE>
    int operator()(const SOURCE_ELEMENT_TYPE& sourceElement) const;
};

        // ============================================================
        // class bdlat_SymbolicConverter_StoreInNullable<NULLABLE_TYPE>
        // ============================================================

template <class NULLABLE_TYPE>
class bdlat_SymbolicConverter_StoreInNullable {
    // Similar to 'StoreInSequence' but this is for nullable.

    // PRIVATE DATA MEMBERS
    NULLABLE_TYPE               *d_destination_p;  // held, not owned
    bdlat_SymbolicConverter_Imp *d_imp_p;          // held, not owned

  public:
    // CREATORS
    explicit bdlat_SymbolicConverter_StoreInNullable(
                                      NULLABLE_TYPE               *destination,
                                      bdlat_SymbolicConverter_Imp *imp);

    // ACCESSORS
    template <class VALUE_TYPE>
    int operator()(const VALUE_TYPE& value) const;
};

// ============================================================================
//                               PROXY CLASSES
// ============================================================================

         // =========================================================
         // struct bdlat_SymbolicConverter_Imp_resolveDynamicRhsProxy
         // =========================================================

template <class LHS_TYPE, class LHS_CATEGORY>
struct bdlat_SymbolicConverter_Imp_resolveDynamicRhsProxy {
    // Component-private struct.  Do not use.

    // DATA MEMBERS
    bdlat_SymbolicConverter_Imp *d_instance_p;
    LHS_TYPE                    *d_lhs_p;

    // CREATORS

    // Creators have been omitted to allow simple static initialization of this
    // struct.

    // FUNCTIONS
    template <class TYPE>
    inline
    int operator()(const TYPE&, bslmf::Nil)
    {
        BSLS_ASSERT_SAFE(0);
        return -2;
    }

    template <class TYPE, class ANY_CATEGORY>
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
         // struct bdlat_SymbolicConverter_Imp_resolveDynamicLhsProxy
         // =========================================================

template <class RHS_TYPE, class RHS_CATEGORY>
struct bdlat_SymbolicConverter_Imp_resolveDynamicLhsProxy {
    // Component-private struct.  Do not use.

    // DATA MEMBERS
    bdlat_SymbolicConverter_Imp *d_instance_p;
    const RHS_TYPE              *d_rhs_p;

    // CREATORS

    // Creators have been omitted to allow simple static initialization of this
    // struct.

    // FUNCTIONS
    template <class TYPE>
    inline
    int operator()(TYPE *object, bslmf::Nil)
    {
        BSLS_ASSERT_SAFE(0);
        return -3;
    }

    template <class TYPE, class ANY_CATEGORY>
    inline
    int operator()(TYPE *object, ANY_CATEGORY category)
    {
        return d_instance_p->resolveDynamicTypes(object,
                                                 category,
                                                 *d_rhs_p,
                                                 RHS_CATEGORY());
    }
};

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

                     // ---------------------------------
                     // class bdlat_SymbolicConverter_Imp
                     // ---------------------------------

// PRIVATE MANIPULATORS

template <class LHS_TYPE, class RHS_TYPE>
inline
int bdlat_SymbolicConverter_Imp::convert(LHS_TYPE                      *lhs,
                                         bdlat_TypeCategory::Sequence,
                                         const RHS_TYPE&                rhs,
                                         bdlat_TypeCategory::Sequence)
{
    bdlat_SymbolicConverter_StoreInSequence<LHS_TYPE> storeInLhs(lhs, this);

    return bdlat_SequenceFunctions::accessAttributes(rhs, storeInLhs);
}

template <class LHS_TYPE, class RHS_TYPE>
int bdlat_SymbolicConverter_Imp::convert(LHS_TYPE                    *lhs,
                                         bdlat_TypeCategory::Choice,
                                         const RHS_TYPE&              rhs,
                                         bdlat_TypeCategory::Choice)
{
    enum { k_SUCCESS = 0 };

    bdlat_SymbolicConverter_StoreInChoice<LHS_TYPE> storeInLhs(lhs, this);

    if (bdlat_ChoiceFunctions::k_UNDEFINED_SELECTION_ID
                                  == bdlat_ChoiceFunctions::selectionId(rhs)) {
        bdlat_ValueTypeFunctions::reset(lhs);

        return k_SUCCESS;                                             // RETURN
    }

    return bdlat_ChoiceFunctions::accessSelection(rhs, storeInLhs);
}

template <class LHS_TYPE, class RHS_TYPE>
int bdlat_SymbolicConverter_Imp::convert(LHS_TYPE                   *lhs,
                                         bdlat_TypeCategory::Array,
                                         const RHS_TYPE&             rhs,
                                         bdlat_TypeCategory::Array)
{
    enum { k_SUCCESS = 0, k_FAILURE = -4 };

    const int size = static_cast<int>(bdlat_ArrayFunctions::size(rhs));

    bdlat_ArrayFunctions::resize(lhs, size);

    for (int i = 0; i < size; ++i) {
        bdlat_SymbolicConverter_StoreInArrayElement<LHS_TYPE>
                                                      storeInLhs(lhs, i, this);

        if (0 != bdlat_ArrayFunctions::accessElement(rhs, storeInLhs, i)) {
            return k_FAILURE;                                         // RETURN
        }
    }

    return k_SUCCESS;
}

template <class LHS_TYPE, class RHS_TYPE>
inline
int bdlat_SymbolicConverter_Imp::convert(LHS_TYPE                         *lhs,
                                         bdlat_TypeCategory::Enumeration,
                                         const RHS_TYPE&                   rhs,
                                         bdlat_TypeCategory::Enumeration)
{
    bsl::string str;

    bdlat_EnumFunctions::toString(&str, rhs);

    return bdlat_EnumFunctions::fromString(lhs,
                                           str.data(),
                                           static_cast<int>(str.length()));
}

template <class LHS_TYPE, class RHS_TYPE>
inline
int bdlat_SymbolicConverter_Imp::convert(LHS_TYPE                         *lhs,
                                         bdlat_TypeCategory::Enumeration,
                                         const RHS_TYPE&                   rhs,
                                         bdlat_TypeCategory::Simple)
{
    return bdlat_ValueTypeFunctions::assign(lhs, rhs);
}

template <class LHS_TYPE, class RHS_TYPE>
inline
int bdlat_SymbolicConverter_Imp::convert(LHS_TYPE                         *lhs,
                                         bdlat_TypeCategory::Simple,
                                         const RHS_TYPE&                   rhs,
                                         bdlat_TypeCategory::Enumeration)
{
    return bdlat_ValueTypeFunctions::assign(lhs, rhs);
}

template <class LHS_TYPE, class RHS_TYPE>
int bdlat_SymbolicConverter_Imp::convert(
                                       LHS_TYPE                           *lhs,
                                       bdlat_TypeCategory::NullableValue,
                                       const RHS_TYPE&                     rhs,
                                       bdlat_TypeCategory::NullableValue)
{
    enum { k_SUCCESS = 0 };

    if (bdlat_NullableValueFunctions::isNull(rhs)) {
        bdlat_ValueTypeFunctions::reset(lhs);

        return k_SUCCESS;                                             // RETURN
    }

    bdlat_SymbolicConverter_StoreInNullable<LHS_TYPE> storeInLhs(lhs, this);

    return bdlat_NullableValueFunctions::accessValue(rhs, storeInLhs);
}

template <class LHS_TYPE, class RHS_TYPE, class RHS_CATEGORY>
inline
int bdlat_SymbolicConverter_Imp::convert(
                                       LHS_TYPE                           *lhs,
                                       bdlat_TypeCategory::NullableValue,
                                       const RHS_TYPE&                     rhs,
                                       RHS_CATEGORY)
{
    bdlat_SymbolicConverter_StoreInNullable<LHS_TYPE> storeInLhs(lhs, this);

    return storeInLhs(rhs);
}

template <class LHS_TYPE, class LHS_CATEGORY, class RHS_TYPE>
int bdlat_SymbolicConverter_Imp::convert(
                                       LHS_TYPE                           *lhs,
                                       LHS_CATEGORY,
                                       const RHS_TYPE&                     rhs,
                                       bdlat_TypeCategory::NullableValue)
{
    enum { k_SUCCESS = 0 };

    if (bdlat_NullableValueFunctions::isNull(rhs)) {
        // ignore the value and let '*lhs' contain its *default* value
        return k_SUCCESS;                                             // RETURN
    }

    bdlat_SymbolicConverter_StoreValue<LHS_TYPE> storeIntoLhs(lhs, this);

    return bdlat_NullableValueFunctions::accessValue(rhs, storeIntoLhs);
}

template <class LHS_TYPE, class RHS_TYPE>
int bdlat_SymbolicConverter_Imp::convert(
                                      LHS_TYPE                            *lhs,
                                      bdlat_TypeCategory::CustomizedType,
                                      const RHS_TYPE&                      rhs,
                                      bdlat_TypeCategory::NullableValue)
{
    enum { k_SUCCESS = 0 };

    if (bdlat_NullableValueFunctions::isNull(rhs)) {
        // ignore the value and let '*lhs' contain its *default* value
        return k_SUCCESS;                                             // RETURN
    }

    bdlat_SymbolicConverter_StoreValue<LHS_TYPE> storeIntoLhs(lhs, this);

    return bdlat_NullableValueFunctions::accessValue(rhs, storeIntoLhs);
}

template <class LHS_TYPE, class RHS_TYPE>
inline
int bdlat_SymbolicConverter_Imp::convert(
                                      LHS_TYPE                            *lhs,
                                      bdlat_TypeCategory::CustomizedType,
                                      const RHS_TYPE&                      rhs,
                                      bdlat_TypeCategory::CustomizedType)
{
    return convert(lhs,
                   bdlat_CustomizedTypeFunctions::convertToBaseType(rhs));
}

template <class LHS_TYPE, class RHS_TYPE, class RHS_CATEGORY>
int bdlat_SymbolicConverter_Imp::convert(
                                      LHS_TYPE                            *lhs,
                                      bdlat_TypeCategory::CustomizedType,
                                      const RHS_TYPE&                      rhs,
                                      RHS_CATEGORY)
{
    enum { k_FAILURE = -5 };

    typedef typename
    bdlat_CustomizedTypeFunctions::BaseType<LHS_TYPE>::Type LhsBaseType;

    LhsBaseType lhsBaseValue;

    if (0 != convert(&lhsBaseValue, rhs)) {
        return k_FAILURE;                                             // RETURN
    }

    return bdlat_CustomizedTypeFunctions::convertFromBaseType(lhs,
                                                              lhsBaseValue);
}

template <class LHS_TYPE, class LHS_CATEGORY, class RHS_TYPE>
inline
int bdlat_SymbolicConverter_Imp::convert(
                                      LHS_TYPE                            *lhs,
                                      LHS_CATEGORY,
                                      const RHS_TYPE&                      rhs,
                                      bdlat_TypeCategory::CustomizedType)
{
    return convert(lhs,
                   bdlat_CustomizedTypeFunctions::convertToBaseType(rhs));
}

template <class LHS_TYPE, class RHS_TYPE>
inline
int bdlat_SymbolicConverter_Imp::convert(
                                      LHS_TYPE                            *lhs,
                                      bdlat_TypeCategory::NullableValue,
                                      const RHS_TYPE&                      rhs,
                                      bdlat_TypeCategory::CustomizedType)
{
    return convert(lhs,
                   bdlat_CustomizedTypeFunctions::convertToBaseType(rhs));
}

template <class LHS_TYPE, class RHS_TYPE>
inline
int bdlat_SymbolicConverter_Imp::convert(LHS_TYPE                    *lhs,
                                         bdlat_TypeCategory::Simple,
                                         const RHS_TYPE&              rhs,
                                         bdlat_TypeCategory::Simple)
{
    return bdlat_ValueTypeFunctions::assign(lhs, rhs);
}

template <class LHS_TYPE,
          class LHS_CATEGORY,
          class RHS_TYPE,
          class RHS_CATEGORY>
inline
int bdlat_SymbolicConverter_Imp::convert(LHS_TYPE        *,
                                         LHS_CATEGORY,
                                         const RHS_TYPE&,
                                         RHS_CATEGORY)
{
    enum { k_FAILURE = -6 };

    return k_FAILURE;
}

template <class LHS_TYPE, class RHS_TYPE>
inline
int bdlat_SymbolicConverter_Imp::resolveDynamicTypes(
                                         LHS_TYPE                         *lhs,
                                         bdlat_TypeCategory::DynamicType,
                                         const RHS_TYPE&                   rhs,
                                         bdlat_TypeCategory::DynamicType)
{
    bdlat_SymbolicConverter_Imp_resolveDynamicRhsProxy<
                        LHS_TYPE,
                        bdlat_TypeCategory::DynamicType> proxy = { this, lhs };

    return bdlat_TypeCategoryUtil::accessByCategory(rhs, proxy);
}

template <class LHS_TYPE, class RHS_TYPE, class RHS_CATEGORY>
inline
int bdlat_SymbolicConverter_Imp::resolveDynamicTypes(
                                          LHS_TYPE                        *lhs,
                                          bdlat_TypeCategory::DynamicType,
                                          const RHS_TYPE&                  rhs,
                                          RHS_CATEGORY)
{
    bdlat_SymbolicConverter_Imp_resolveDynamicLhsProxy<
                                          RHS_TYPE,
                                          RHS_CATEGORY> proxy = { this, &rhs };

    return bdlat_TypeCategoryUtil::manipulateByCategory(lhs, proxy);
}

template <class LHS_TYPE, class LHS_CATEGORY, class RHS_TYPE>
inline
int bdlat_SymbolicConverter_Imp::resolveDynamicTypes(
                                         LHS_TYPE                         *lhs,
                                         LHS_CATEGORY,
                                         const RHS_TYPE&                   rhs,
                                         bdlat_TypeCategory::DynamicType)
{
    bdlat_SymbolicConverter_Imp_resolveDynamicRhsProxy<
                                           LHS_TYPE,
                                           LHS_CATEGORY> proxy = { this, lhs };

    return bdlat_TypeCategoryUtil::accessByCategory(rhs, proxy);
}

template <class LHS_TYPE,
          class LHS_CATEGORY,
          class RHS_TYPE,
          class RHS_CATEGORY>
inline
int bdlat_SymbolicConverter_Imp::resolveDynamicTypes(
                                                  LHS_TYPE        *lhs,
                                                  LHS_CATEGORY     lhsCategory,
                                                  const RHS_TYPE&  rhs,
                                                  RHS_CATEGORY     rhsCategory)
{
    return convert(lhs, lhsCategory, rhs, rhsCategory);
}

// CREATORS

inline
bdlat_SymbolicConverter_Imp::bdlat_SymbolicConverter_Imp(
                                                     bsl::ostream *errorStream)
: d_errorStream_p(errorStream)
{
    BSLS_ASSERT_SAFE(d_errorStream_p);
}

// MANIPULATORS

template <class LHS_TYPE, class RHS_TYPE>
inline
int bdlat_SymbolicConverter_Imp::convert(LHS_TYPE        *lhs,
                                         const RHS_TYPE&  rhs)
{
    typedef typename bdlat_TypeCategory::Select<LHS_TYPE>::Type LhsCategory;
    typedef typename bdlat_TypeCategory::Select<RHS_TYPE>::Type RhsCategory;

    return resolveDynamicTypes(lhs, LhsCategory(), rhs, RhsCategory());
}

inline
bsl::ostream& bdlat_SymbolicConverter_Imp::errorStream()
{
    return *d_errorStream_p;
}

           // -----------------------------------------------------
           // class bdlat_SymbolicConverter_StoreValue<LVALUE_TYPE>
           // -----------------------------------------------------

// CREATORS

template <class LVALUE_TYPE>
inline
bdlat_SymbolicConverter_StoreValue<LVALUE_TYPE>::
bdlat_SymbolicConverter_StoreValue(LVALUE_TYPE                 *destination,
                                   bdlat_SymbolicConverter_Imp *imp)
: d_destination_p(destination)
, d_imp_p(imp)
{
}

// ACCESSORS

template <class LVALUE_TYPE>
template <class RVALUE_TYPE, class INFO_TYPE>
inline
int bdlat_SymbolicConverter_StoreValue<LVALUE_TYPE>::operator()(
                                                     const RVALUE_TYPE& object,
                                                     const INFO_TYPE&) const
{
    return d_imp_p->convert(d_destination_p, object);
}

template <class LVALUE_TYPE>
template <class RVALUE_TYPE>
inline
int bdlat_SymbolicConverter_StoreValue<LVALUE_TYPE>::operator()(
                                               const RVALUE_TYPE& object) const
{
    return d_imp_p->convert(d_destination_p, object);
}

            // ----------------------------------------------------
            // class bdlat_SymbolicConverter_LoadValue<RVALUE_TYPE>
            // ----------------------------------------------------

// CREATORS

template <class RVALUE_TYPE>
inline
bdlat_SymbolicConverter_LoadValue<RVALUE_TYPE>::
                    bdlat_SymbolicConverter_LoadValue(
                                            const RVALUE_TYPE&           value,
                                            bdlat_SymbolicConverter_Imp *imp)
: d_imp_p(imp)
, d_value(value)
{
}

// ACCESSORS

template <class RVALUE_TYPE>
template <class LVALUE_TYPE, class INFO_TYPE>
inline
int bdlat_SymbolicConverter_LoadValue<RVALUE_TYPE>::operator()(
                                                        LVALUE_TYPE *object,
                                                        const INFO_TYPE&) const
{
    return d_imp_p->convert(object, d_value);
}

template <class RVALUE_TYPE>
template <class LVALUE_TYPE>
inline
int bdlat_SymbolicConverter_LoadValue<RVALUE_TYPE>::operator()(
                                                     LVALUE_TYPE *object) const
{
    return d_imp_p->convert(object, d_value);
}

        // ------------------------------------------------------------
        // class bdlat_SymbolicConverter_StoreInSequence<SEQUENCE_TYPE>
        // ------------------------------------------------------------

// CREATORS

template <class SEQUENCE_TYPE>
inline
bdlat_SymbolicConverter_StoreInSequence<SEQUENCE_TYPE>::
bdlat_SymbolicConverter_StoreInSequence(
                                      SEQUENCE_TYPE               *destination,
                                      bdlat_SymbolicConverter_Imp *imp)
: d_destination_p(destination)
, d_imp_p(imp)
{
}

// ACCESSORS

template <class SEQUENCE_TYPE>
template <class SOURCE_MEMBER_TYPE, class INFO_TYPE>
inline
int bdlat_SymbolicConverter_StoreInSequence<SEQUENCE_TYPE>::operator()(
                                        const SOURCE_MEMBER_TYPE& sourceMember,
                                        const INFO_TYPE&          info) const
{
    enum { k_SUCCESS = 0, k_FAILURE = -7 };

    bdlat_SymbolicConverter_LoadValue<SOURCE_MEMBER_TYPE> loadSourceValue(
                                                                  sourceMember,
                                                                  d_imp_p);

    if (0 != bdlat_SequenceFunctions::manipulateAttribute(d_destination_p,
                                                          loadSourceValue,
                                                          info.name(),
                                                          info.nameLength())) {
        d_imp_p->errorStream()
                 << "Failed to convert attribute '"
                 << bslstl::StringRef(info.name(), info.nameLength()) << "'\n";

        return k_FAILURE;                                             // RETURN
    }

    return k_SUCCESS;
}

          // --------------------------------------------------------
          // class bdlat_SymbolicConverter_StoreInChoice<CHOICE_TYPE>
          // --------------------------------------------------------

// CREATORS

template <class CHOICE_TYPE>
inline
bdlat_SymbolicConverter_StoreInChoice<CHOICE_TYPE>::
bdlat_SymbolicConverter_StoreInChoice(CHOICE_TYPE                 *destination,
                                      bdlat_SymbolicConverter_Imp *imp)
: d_destination_p(destination)
, d_imp_p(imp)
{
}

// ACCESSORS

template <class CHOICE_TYPE>
template <class SOURCE_MEMBER_TYPE, class INFO_TYPE>
int bdlat_SymbolicConverter_StoreInChoice<CHOICE_TYPE>::operator()(
                                        const SOURCE_MEMBER_TYPE& sourceMember,
                                        const INFO_TYPE&          info) const
{
    enum { k_SUCCESS = 0, k_FAILURE = -8 };

    // Make the selection.

    if (0 != bdlat_ChoiceFunctions::makeSelection(d_destination_p,
                                                  info.name(),
                                                  info.nameLength())) {
        d_imp_p->errorStream()
                 << "Failed to make selection '"
                 << bslstl::StringRef(info.name(), info.nameLength()) << "'\n";

        return k_FAILURE;                                             // RETURN
    }

    // Assign the value.

    bdlat_SymbolicConverter_LoadValue<SOURCE_MEMBER_TYPE> loadSourceValue(
                                                                  sourceMember,
                                                                  d_imp_p);

    if (0 != bdlat_ChoiceFunctions::manipulateSelection(d_destination_p,
                                                        loadSourceValue)) {
        d_imp_p->errorStream()
                 << "Failed to convert selection '"
                 << bslstl::StringRef(info.name(), info.nameLength()) << "'\n";

        return k_FAILURE;                                             // RETURN
    }

    return k_SUCCESS;
}

       // -------------------------------------------------------------
       // class bdlat_SymbolicConverter_StoreInArrayElement<ARRAY_TYPE>
       // -------------------------------------------------------------

// CREATORS

template <class ARRAY_TYPE>
inline
bdlat_SymbolicConverter_StoreInArrayElement<ARRAY_TYPE>::
bdlat_SymbolicConverter_StoreInArrayElement(ARRAY_TYPE                  *array,
                                            int                          index,
                                            bdlat_SymbolicConverter_Imp *imp)
: d_array_p(array)
, d_imp_p(imp)
, d_index(index)
{
}

// ACCESSORS

template <class ARRAY_TYPE>
template <class SOURCE_ELEMENT_TYPE>
inline
int bdlat_SymbolicConverter_StoreInArrayElement<ARRAY_TYPE>::operator()(
                                const SOURCE_ELEMENT_TYPE& sourceElement) const
{
    bdlat_SymbolicConverter_LoadValue<SOURCE_ELEMENT_TYPE> loadSourceValue(
                                                                 sourceElement,
                                                                 d_imp_p);

    return bdlat_ArrayFunctions::manipulateElement(d_array_p,
                                                   loadSourceValue,
                                                   d_index);
}

        // ------------------------------------------------------------
        // class bdlat_SymbolicConverter_StoreInNullable<NULLABLE_TYPE>
        // ------------------------------------------------------------

// CREATORS

template <class NULLABLE_TYPE>
inline
bdlat_SymbolicConverter_StoreInNullable<NULLABLE_TYPE>::
bdlat_SymbolicConverter_StoreInNullable(
                                      NULLABLE_TYPE               *destination,
                                      bdlat_SymbolicConverter_Imp *imp)
: d_destination_p(destination)
, d_imp_p(imp)
{
}

// ACCESSORS

template <class NULLABLE_TYPE>
template <class VALUE_TYPE>
inline
int bdlat_SymbolicConverter_StoreInNullable<NULLABLE_TYPE>::operator()(
                                                 const VALUE_TYPE& value) const
{
    bdlat_NullableValueFunctions::makeValue(d_destination_p);

    // Assign the value.

    bdlat_SymbolicConverter_LoadValue<VALUE_TYPE> loadSourceValue(value,
                                                                  d_imp_p);

    return bdlat_NullableValueFunctions::manipulateValue(d_destination_p,
                                                         loadSourceValue);
}

                       // ------------------------------
                       // struct bdlat_SymbolicConverter
                       // ------------------------------

template <class LHS_TYPE, class RHS_TYPE>
inline
int bdlat_SymbolicConverter::convert(LHS_TYPE        *lhs,
                                     const RHS_TYPE&  rhs)
{
    bsl::ostream nullStream(0);
    return bdlat_SymbolicConverter::convert(lhs, rhs, nullStream);
}

template <class LHS_TYPE, class RHS_TYPE>
inline
int bdlat_SymbolicConverter::convert(LHS_TYPE        *lhs,
                                     const RHS_TYPE&  rhs,
                                     bsl::ostream&    errorStream)
{
    bdlat_ValueTypeFunctions::reset(lhs);

    bdlat_SymbolicConverter_Imp imp(&errorStream);

    return imp.convert(lhs, rhs);
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
