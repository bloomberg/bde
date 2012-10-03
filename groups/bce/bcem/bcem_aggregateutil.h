// bcem_aggregateutil.h                                               -*-C++-*-
#ifndef INCLUDED_BCEM_AGGREGATEUTIL
#define INCLUDED_BCEM_AGGREGATEUTIL

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide utilities for manipulating 'bcem_Aggregate' objects.
//
//@CLASSES:
//   bcem_AggregateUtil: utility functions for 'bcem_Aggregate' objects
//
//@AUTHOR: David Rubin (drubin6), Rohan Bhindwale (rbhindwa)
//
//@SEE_ALSO: bcem_aggregate
//
//@DESCRIPTION: This component provides utilities to convert the field(s) of a
// 'bcem_Aggregate' of "List" or "Choice" type to and from values of
// corresponding C++ types.  For a detailed description of 'bcem_Aggregate'
// types see the component-level documentation for 'bcem_aggregate'.
//
///Thread-Safety
///-------------
// All methods of 'bcem_AggregateUtil' are 'const' Thread-Safe.
//
///Usage Example
///-------------
//  The following example shows how to convert a 'bcem_Aggregate' to a
//  bdeat-compatible type sharing the same schema.
//..
//
//  <?xml version='1.0' encoding='UTF-8'?>
//  <schema xmlns='http://www.w3.org/2001/XMLSchema'
//          xmlns:bb='http://bloomberg.com/schemas/xyz'
//          targetNamespace='http://bloomberg.com/schemas/xyz'
//          elementFormDefault='qualified'>
//
//    <complexType name='Company'>
//      <sequence>
//        <element name='Name' type='string'/>
//        <element name='AccountNum' type='int'/>
//      </sequence>
//    </complexType>
//
//    <complexType name='Person'>
//      <sequence>
//        <element name='LastName' type='string'/>
//        <element name='FirstName' type='string'/>
//        <element name='Age' type='int'/>
//        <element name='BirthDate' type='date'/>
//      </sequence>
//    </complexType>
//
//    <complexType name='Entity'>
//      <choice>
//        <element name='Corp' type='bb:Company'/>
//        <element name='Human' type='bb:Person'/>
//      </choice>
//    </complexType>
//
//    <element name='Entity' type='bb:Entity'/>
//
//  </schema>
//..
//  Manually create the bdem schema.
//..
//  bslma_Allocator *allocator = bslma_Default::defaultAllocator();
//
//  bcema_SharedPtr<bdem_Schema> schema(new (*allocator) bdem_Schema());
//
//  bdem_RecordDef *companyRecordDef = schema->createRecord("Company");
//  companyRecordDef->appendField(bdem_ElemType::BDEM_STRING, "Name");
//  companyRecordDef->appendField(bdem_ElemType::BDEM_INT,    "AccountNum");
//
//  bdem_RecordDef *personRecordDef = schema->createRecord("Person");
//  personRecordDef->appendField(bdem_ElemType::BDEM_STRING,  "LastName");
//  personRecordDef->appendField(bdem_ElemType::BDEM_STRING,  "FirstName");
//  personRecordDef->appendField(bdem_ElemType::BDEM_INT,     "Age");
//  personRecordDef->appendField(bdem_ElemType::BDEM_DATETZ,  "BirthDate");
//
//  bdem_RecordDef *entityRecordDef =
//         schema->createRecord("Entity", bdem_RecordDef::BDEM_CHOICE_RECORD);
//  entityRecordDef->appendField(bdem_ElemType::BDEM_LIST,
//                               companyRecordDef,
//                               "Corp");
//  entityRecordDef->appendField(bdem_ElemType::BDEM_LIST,
//                               personRecordDef,
//                               "Human");
//
//..
// Initialize the aggregate.
//..
//  bcem_Aggregate entityAgg(schema, "Entity");
//
//  bcem_Aggregate entityAggSelection = entityAgg.makeSelection("Human");
//  entityAggSelection.setField("FirstName", "John");
//  entityAggSelection.setField("LastName", "Doe");
//  entityAggSelection.setField("Age", 27);
//  entityAggSelection.setField("BirthDate",
//                              bdet_DateTz(bdet_Date(1980, 4, 4), -5));
//
//..
// Convert the aggregate to a bdeat-compatible type.
//..
//  test::Entity entity;
//  int rc;
//
//  rc = bcem_AggregateUtil::fromAggregate(&entity.makeHuman(),
//                                         entityAgg,
//                                         test::Entity::SELECTION_ID_HUMAN);
//  BSLS_ASSERT(0 == rc);
//..

#ifndef INCLUDED_BCESCM_VERSION
#include <bcescm_version.h>
#endif

#ifndef INCLUDED_BCEM_AGGREGATE
#include <bcem_aggregate.h>
#endif

#ifndef INCLUDED_BCEM_AGGREGATERAW
#include <bcem_aggregateraw.h>
#endif

#ifndef INCLUDED_BCEM_ERRORATTRIBUTES
#include <bcem_errorattributes.h>
#endif

#ifndef INCLUDED_BDEAT_CHOICEFUNCTIONS
#include <bdeat_choicefunctions.h>
#endif

#ifndef INCLUDED_BDEAT_CUSTOMIZEDTYPEFUNCTIONS
#include <bdeat_customizedtypefunctions.h>
#endif

#ifndef INCLUDED_BDEAT_TYPECATEGORY
#include <bdeat_typecategory.h>
#endif

#ifndef INCLUDED_BDEM_ELEMREF
#include <bdem_elemref.h>
#endif

#ifndef INCLUDED_BDEM_ELEMTYPE
#include <bdem_elemtype.h>
#endif

#ifndef INCLUDED_BDEM_SELECTBDEMTYPE
#include <bdem_selectbdemtype.h>
#endif

#ifndef INCLUDED_BDET_DATE
#include <bdet_date.h>
#endif

#ifndef INCLUDED_BDET_DATETIME
#include <bdet_datetime.h>
#endif

#ifndef INCLUDED_BDET_DATETIMETZ
#include <bdet_datetimetz.h>
#endif

#ifndef INCLUDED_BDET_DATETZ
#include <bdet_datetz.h>
#endif

#ifndef INCLUDED_BDET_TIME
#include <bdet_time.h>
#endif

#ifndef INCLUDED_BDET_TIMETZ
#include <bdet_timetz.h>
#endif

#ifndef INCLUDED_BDETU_UNSET
#include <bdetu_unset.h>
#endif

#ifndef INCLUDED_BDEUT_NULLABLEALLOCATEDVALUE
#include <bdeut_nullableallocatedvalue.h>
#endif

#ifndef INCLUDED_BDEUT_NULLABLEVALUE
#include <bdeut_nullablevalue.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITS
#include <bslalg_typetraits.h>
#endif

#ifndef INCLUDED_BSLS_TYPES
#include <bsls_types.h>
#endif

#ifndef INCLUDED_BSL_STRING
#include <bsl_string.h>
#endif

#ifndef INCLUDED_BSL_VECTOR
#include <bsl_vector.h>
#endif

namespace BloombergLP {

                          // ========================
                          // class bcem_AggregateUtil
                          // ========================

class bcem_AggregateUtil {
    // Provide a namespace for a suite of functions for manipulating
    // 'bcem_Aggregate' objects.  The 'toAggregate' and 'fromAggregate'
    // functions convert between aggregates and primitive types (including
    // 'bsl::string', 'bsls' 64-bit integral types, and a variety of 'bdet'
    // time and date types), nullable primitive types, arrays of primitive
    // types, and arrays of nullable primitive types.

  private:
    // PRIVATE CLASS METHODS
    template <typename TYPE>
    static bsl::vector<TYPE>& theModifiableArray(const bdem_ElemRef& elemRef);
        // Return a reference providing modifiable access to the vector
        // referred by the specified 'elemRef'.  The behavior is undefined
        // unless 'elemRef' refers to an object of the 'bdem' array type
        // corresponding to 'TYPE'.

    template <typename TYPE>
    static int fromAggregateRawToEnumeration(
                                         TYPE                     *destination,
                                         const bcem_AggregateRaw&  aggregate);
        // Load into the specified 'destination' the value of the specified
        // 'aggregate'.  Return 0 on success and a non-zero value otherwise.
        // The behavior is undefined unless 'destination' refers to an object
        // of enumeration type.  Note that a non-zero value is returned if
        // 'aggregate' does not refer to an object of type convertible to a
        // string.

    template <typename TYPE>
    static int fromAggregateRawToSimple(TYPE                     *destination,
                                        const bcem_AggregateRaw&  aggregate);
        // Load into the specified 'destination' the value of the specified
        // 'aggregate'.  Return 0 on success and a non-zero value otherwise.
        // The behavior is undefined unless 'destination' refers to an object
        // of simple type and 'aggregate' refers to an object of scalar type.

    template <typename TYPE>
    static int fromAggregateRawImp(TYPE                     *destination,
                                   const bcem_AggregateRaw&  aggregate,
                                   bdeat_TypeCategory::Enumeration);
        // Load into the specified 'destination' the value of the specified
        // 'aggregate'.  Return 0 on success and a non-zero value otherwise.
        // The behavior is undefined unless 'destination' refers to an object
        // of enumeration type.  Note that a non-zero value is returned if
        // 'aggregate' does not refer to an object of type convertible to a
        // string.

    template <typename TYPE>
    static int fromAggregateRawImp(TYPE                     *destination,
                                   const bcem_AggregateRaw&  aggregate,
                                   bdeat_TypeCategory::Simple);
        // Load into the specified 'destination' the value of the specified
        // 'aggregate'.  Return 0 on success and a non-zero value otherwise.
        // The behavior is undefined unless 'destination' refers to an object
        // of simple type.  Note that a non-zero value is returned if
        // 'aggregate' does not refer to an object of scalar type.

    template <typename TYPE>
    static int fromAggregateRawToNullable(
                                         TYPE                     *destination,
                                         const bcem_AggregateRaw&  aggregate);
        // Load into the specified 'destination' the value of the specified
        // 'aggregate'.  Return 0 on success and a non-zero value otherwise.
        // The behavior is undefined unless 'destination' refers to an object
        // of nullable simple or nullable enumeration type.  Note that a
        // non-zero value is returned if 'aggregate' does not refer to an
        // object of scalar type or a type convertible to a string.

    template <typename TYPE>
    static int fromAggregateToComplex(TYPE                  *destination,
                                      const bcem_Aggregate&  aggregate);
        // Load into the specified 'destination' the value of the specified
        // 'aggregate'.  Return 0 on success and a non-zero value otherwise.
        // The behavior is undefined unless 'destination' refers to an object
        // of sequence, choice, or customized type and the data type of
        // 'aggregate' is the 'bdem' type corresponding to 'TYPE'.

    template <typename TYPE>
    static int fromAggregateImp(TYPE                  *destination,
                                const bcem_Aggregate&  aggregate,
                                bdeat_TypeCategory::Array);
    template <typename TYPE>
    static int fromAggregateImp(TYPE                  *destination,
                                const bcem_Aggregate&  aggregate,
                                bdeat_TypeCategory::Choice);
    template <typename TYPE>
    static int fromAggregateImp(TYPE                  *destination,
                                const bcem_Aggregate&  aggregate,
                                bdeat_TypeCategory::CustomizedType);
    template <typename TYPE>
    static int fromAggregateImp(TYPE                  *destination,
                                const bcem_Aggregate&  aggregate,
                                bdeat_TypeCategory::Enumeration);
    template <typename TYPE>
    static int fromAggregateImp(TYPE                  *destination,
                                const bcem_Aggregate&  aggregate,
                                bdeat_TypeCategory::Sequence);
    template <typename TYPE>
    static int fromAggregateImp(TYPE                  *destination,
                                const bcem_Aggregate&  aggregate,
                                bdeat_TypeCategory::Simple);
        // Load into the specified 'destination' the value of the specified
        // 'aggregate'.  Return 0 on success and a non-zero value otherwise.
        // The behavior is undefined unless 'destination' refers to an object
        // of sequence, array, choice, customized, simple, or enumeration type.
        // Note that a non-zero value is returned if 'aggregate' does not
        // refer to an object of the 'bdem' type corresponding to 'TYPE'.

    template <typename TYPE>
    static int fromAggregateToNullable(TYPE                  *destination,
                                       const bcem_Aggregate&  aggregate);
        // Load into the specified 'destination' the value of the specified
        // 'aggregate'.  Return 0 on success and a non-zero value otherwise.
        // The behavior is undefined unless 'TYPE' refers to a nullable type.
        // Note that a non-zero value is returned if 'aggregate' does not refer
        // to the 'bdem' type corresponding to 'TYPE::ValueType'.

    template <typename TYPE, typename CONVERTER>
    static int fromAggregateRawToVectorOfPrimitive(
                                         bsl::vector<TYPE>        *destination,
                                         const bcem_AggregateRaw&  aggregate,
                                         CONVERTER                 converter);
        // Load into the specified 'destination' the value of the specified
        // 'aggregate' using the specified 'converter' to convert individual
        // elements.  Return 0 on success and a non-zero value otherwise.  The
        // behavior is undefined unless 'TYPE' refers to a scalar, enumeration,
        // nullable scalar, or nullable enumeration type and 'aggregate' refers
        // to the 'bdem' array type corresponding to 'TYPE' or a
        // 'bdem_ElemType::BDEM_TABLE'.

    template <typename TYPE, typename CONVERTER>
    static int fromAggregateToVectorOfComplex(
                                            bsl::vector<TYPE>     *destination,
                                            const bcem_Aggregate&  aggregate,
                                            CONVERTER              converter);
        // Load into the specified 'destination' the value of the specified
        // 'aggregate' using the specified 'converter' to convert individual
        // elements.  Return 0 on success and a non-zero value otherwise.  The
        // behavior is undefined unless 'TYPE' refers to a sequence, choice,
        // customized, nullable sequence, nullable choice, or nullable
        // customized type and 'aggregate' refers to an object of the 'bdem'
        // array type corresponding to 'TYPE' or a 'bdem_ElemType::BDEM_TABLE'.

    template <typename TYPE>
    static int fromAggregateToVectorOfNullable(
                                            bsl::vector<TYPE>     *destination,
                                            const bcem_Aggregate&  aggregate,
                                            int                    fieldId,
                                            bdeat_TypeCategory::Choice);
    template <typename TYPE>
    static int fromAggregateToVectorOfNullable(
                                           bsl::vector<TYPE>     *destination,
                                           const bcem_Aggregate&  aggregate,
                                           int                    fieldId,
                                           bdeat_TypeCategory::CustomizedType);
    template <typename TYPE>
    static int fromAggregateToVectorOfNullable(
                                            bsl::vector<TYPE>     *destination,
                                            const bcem_Aggregate&  aggregate,
                                            int                    fieldId,
                                            bdeat_TypeCategory::Enumeration);
    template <typename TYPE>
    static int fromAggregateToVectorOfNullable(
                                            bsl::vector<TYPE>     *destination,
                                            const bcem_Aggregate&  aggregate,
                                            int                    fieldId,
                                            bdeat_TypeCategory::Sequence);
    template <typename TYPE>
    static int fromAggregateToVectorOfNullable(
                                            bsl::vector<TYPE>     *destination,
                                            const bcem_Aggregate&  aggregate,
                                            int                    fieldId,
                                            bdeat_TypeCategory::Simple);
        // Load into the specified 'destination' the value of the sub-aggregate
        // referred to by the specified 'fieldId' in the specified 'aggregate'.
        // Return 0 on success and a non-zero value otherwise.  The behavior is
        // undefined unless 'destination' refers to an object of vector of a
        // nullable sequence, choice, simple, customized, or enumeration type.
        // Note that a non-zero value is returned if the sub-aggregate does not
        // refer to an object of the 'bdem' array type corresponding to
        // 'TYPE::ValueType' or a 'bdem_ElemType::BDEM_TABLE'.

    template <typename TYPE>
    static int fromAggregateToVector(bsl::vector<TYPE>     *destination,
                                     const bcem_Aggregate&  aggregate,
                                     int                    fieldId,
                                     bdeat_TypeCategory::Choice);
    template <typename TYPE>
    static int fromAggregateToVector(bsl::vector<TYPE>     *destination,
                                     const bcem_Aggregate&  aggregate,
                                     int                    fieldId,
                                     bdeat_TypeCategory::CustomizedType);
    template <typename TYPE>
    static int fromAggregateToVector(bsl::vector<TYPE>     *destination,
                                     const bcem_Aggregate&  aggregate,
                                     int                    fieldId,
                                     bdeat_TypeCategory::Enumeration);
    template <typename TYPE>
    static int fromAggregateToVector(bsl::vector<TYPE>     *destination,
                                     const bcem_Aggregate&  aggregate,
                                     int                    fieldId,
                                     bdeat_TypeCategory::NullableValue);
    template <typename TYPE>
    static int fromAggregateToVector(bsl::vector<TYPE>     *destination,
                                     const bcem_Aggregate&  aggregate,
                                     int                    fieldId,
                                     bdeat_TypeCategory::Sequence);
    template <typename TYPE>
    static int fromAggregateToVector(bsl::vector<TYPE>     *destination,
                                     const bcem_Aggregate&  aggregate,
                                     int                    fieldId,
                                     bdeat_TypeCategory::Simple);
        // Load into the specified 'destination' the value of the sub-aggregate
        // referred to by the specified 'fieldId' in the specified 'aggregate'.
        // Return 0 on success and a non-zero value otherwise.  The behavior is
        // undefined unless 'destination' refers to an object of vector of
        // sequence, choice, customized, simple, or enumeration type, or a
        // vector of nullable sequence, choice, customized, simple, or
        // enumeration type.  Note that a non-zero value is returned if the
        // sub-aggregate does not refer to an object of the 'bdem' array type
        // corresponding to 'TYPE' or a 'bdem_ElemType::BDEM_TABLE'.

    template <typename TYPE>
    static int fromAggregateImp(bsl::vector<TYPE>     *destination,
                                const bcem_Aggregate&  aggregate,
                                int                    fieldId,
                                bdeat_TypeCategory::Array);
    template <typename TYPE>
    static int fromAggregateImp(TYPE                  *destination,
                                const bcem_Aggregate&  aggregate,
                                int                    fieldId,
                                bdeat_TypeCategory::Choice);
    template <typename TYPE>
    static int fromAggregateImp(TYPE                  *destination,
                                const bcem_Aggregate&  aggregate,
                                int                    fieldId,
                                bdeat_TypeCategory::CustomizedType);
    template <typename TYPE>
    static int fromAggregateImp(TYPE                  *destination,
                                const bcem_Aggregate&  aggregate,
                                int                    fieldId,
                                bdeat_TypeCategory::Enumeration);
    template <typename TYPE>
    static int fromAggregateImp(TYPE                  *destination,
                                const bcem_Aggregate&  aggregate,
                                int                    fieldId,
                                bdeat_TypeCategory::NullableValue);
    template <typename TYPE>
    static int fromAggregateImp(TYPE                  *destination,
                                const bcem_Aggregate&  aggregate,
                                int                    fieldId,
                                bdeat_TypeCategory::Sequence);
    template <typename TYPE>
    static int fromAggregateImp(TYPE                  *destination,
                                const bcem_Aggregate&  aggregate,
                                int                    fieldId,
                                bdeat_TypeCategory::Simple);
        // Load into the specified 'destination' the value of the sub-aggregate
        // referred to by the specified 'fieldId' in the specified 'aggregate'.
        // Return 0 on success and a non-zero value otherwise.  The behavior is
        // undefined unless 'destination' refers to an object of sequence,
        // choice, array, nullable, scalar, customized, or enumeration type.
        // Note that a non-zero value is returned if the sub-aggregate does not
        // refer to an object of the 'bdem' type corresponding to 'TYPE'.

    template <typename TYPE>
    static int toAggregateRawFromEnumeration(bcem_AggregateRaw *aggregate,
                                             const TYPE&        source);
        // Load into the specified 'aggregate' the value of the specified
        // 'source'.  Return 0 on success and a non-zero value otherwise.  The
        // behavior is undefined unless 'source' refers to an object of
        // enumeration type.  Note that a non-zero value is returned if
        // 'aggregate' does not refer to a type convertible to a string.

    template <typename TYPE>
    static int toAggregateRawFromSimple(bcem_AggregateRaw *aggregate,
                                        const TYPE&        source);
        // Load into the specified 'aggregate' the value of the specified
        // 'source'.  Return 0 on success and a non-zero value otherwise.  The
        // behavior is undefined unless 'source' refers to an object of simple
        // type and 'aggregate' refers to an object of scalar type.

    template <typename TYPE>
    static int toAggregateRawImp(bcem_AggregateRaw *aggregate,
                                 const TYPE&        source,
                                 bdeat_TypeCategory::Enumeration);
        // Load into the specified 'aggregate' the value of the specified
        // 'source'.  Return 0 on success and a non-zero value otherwise.  The
        // behavior is undefined unless 'source' refers to an object of
        // enumeration type.  Note that a non-zero value is returned if
        // 'aggregate' does not refer to a type convertible to a string.

    template <typename TYPE>
    static int toAggregateRawImp(bcem_AggregateRaw *aggregate,
                                 const TYPE&        source,
                                 bdeat_TypeCategory::Simple);
        // Load into the specified 'aggregate' the value of the specified
        // 'source'.  Return 0 on success and a non-zero value otherwise.  The
        // behavior is undefined unless 'source' refers to an object of simple
        // type.  Note that a non-zero value is returned if 'aggregate' does
        // not refer to an object of scalar type.

    template <typename TYPE>
    static int toAggregateRawFromNullable(bcem_AggregateRaw *aggregate,
                                          const TYPE&        source);
        // Load into the specified 'aggregate' the value of the specified
        // 'source'.  Return 0 on success and a non-zero value otherwise.  The
        // behavior is undefined unless 'source' refers to an object of
        // nullable simple or nullable enumeration type.  Note that a non-zero
        // value is returned if 'aggregate' does not refer to an object of
        // scalar type or a type convertible to a string.

    template <typename TYPE>
    static int toAggregateFromComplex(bcem_Aggregate *aggregate,
                                      const TYPE&     source);
        // Load into the specified 'aggregate' the value of the specified
        // 'source'.  Return 0 on success and a non-zero value otherwise.  The
        // behavior is undefined unless 'source' refers to an object of
        // sequence, choice, or customized type and the data type of
        // 'aggregate' is the 'bdem' type corresponding to 'TYPE'.

    template <typename TYPE>
    static int toAggregateImp(bcem_Aggregate *aggregate,
                              const TYPE&     source,
                              bdeat_TypeCategory::Array);
    template <typename TYPE>
    static int toAggregateImp(bcem_Aggregate *aggregate,
                              const TYPE&     source,
                              bdeat_TypeCategory::CustomizedType);
    template <typename TYPE>
    static int toAggregateImp(bcem_Aggregate *aggregate,
                              const TYPE&     source,
                              bdeat_TypeCategory::Choice);
    template <typename TYPE>
    static int toAggregateImp(bcem_Aggregate *aggregate,
                              const TYPE&     source,
                              bdeat_TypeCategory::Enumeration);
    template <typename TYPE>
    static int toAggregateImp(bcem_Aggregate *aggregate,
                              const TYPE&     source,
                              bdeat_TypeCategory::Sequence);
    template <typename TYPE>
    static int toAggregateImp(bcem_Aggregate *aggregate,
                              const TYPE&     source,
                              bdeat_TypeCategory::Simple);
        // Load into the specified 'aggregate' the value of the specified
        // 'source'.  Return 0 on success and a non-zero value otherwise.  The
        // behavior is undefined unless 'source' refers to an object of
        // sequence, array, choice, customized, simple, or enumeration type.
        // Note that a non-zero value is returned if 'aggregate' does not
        // refer to an object of the 'bdem' type corresponding to 'TYPE'.

    template <typename TYPE>
    static int toAggregateFromNullable(bcem_Aggregate *aggregate,
                                       const TYPE&     source);
        // Load into the specified 'aggregate' the value of the specified
        // 'source'.  Return 0 on success and a non-zero value otherwise.  The
        // behavior is undefined unless 'TYPE' refers to a nullable type.  Note
        // that a non-zero value is returned if 'aggregate' does not refer to
        // an object of the 'bdem' type corresponding to 'TYPE::ValueType'.

    template <typename TYPE, typename CONVERTER>
    static int toAggregateRawFromVectorOfPrimitive(
                                          bcem_AggregateRaw        *aggregate,
                                          const bsl::vector<TYPE>&  source,
                                          CONVERTER                 converter);
        // Load into the specified 'aggregate' the value of the specified
        // 'source' using the specified 'converter' to convert individual
        // elements.  Return 0 on success and a non-zero value otherwise.  The
        // behavior is undefined unless 'TYPE' refers to a scalar, enumeration,
        // nullable scalar, or nullable enumeration type and 'aggregate' refers
        // to the 'bdem' array type corresponding to 'TYPE' or a
        // 'bdem_ElemType::BDEM_TABLE'.

    template <typename TYPE, typename CONVERTER>
    static int toAggregateFromVectorOfComplex(
                                          bcem_Aggregate           *aggregate,
                                          const bsl::vector<TYPE>&  source,
                                          CONVERTER                 converter);
        // Load into the specified 'aggregate' the value of the specified
        // 'source' using the specified 'converter' to convert individual
        // elements.  Return 0 on success and a non-zero value otherwise.  The
        // behavior is undefined unless 'TYPE' refers to a sequence, choice,
        // customized, nullable sequence, nullable choice, or nullable
        // customized type and 'aggregate' refers to an object of the 'bdem'
        // array type corresponding to 'TYPE' or a 'bdem_ElemType::BDEM_TABLE'.

    template <typename TYPE>
    static int toAggregateFromVectorOfNullable(
                                           bcem_Aggregate           *aggregate,
                                           int                       fieldId,
                                           const bsl::vector<TYPE>&  source,
                                           bdeat_TypeCategory::Choice);
    template <typename TYPE>
    static int toAggregateFromVectorOfNullable(
                                           bcem_Aggregate           *aggregate,
                                           int                       fieldId,
                                           const bsl::vector<TYPE>&  source,
                                           bdeat_TypeCategory::CustomizedType);
    template <typename TYPE>
    static int toAggregateFromVectorOfNullable(
                                           bcem_Aggregate           *aggregate,
                                           int                       fieldId,
                                           const bsl::vector<TYPE>&  source,
                                           bdeat_TypeCategory::Enumeration);
    template <typename TYPE>
    static int toAggregateFromVectorOfNullable(
                                           bcem_Aggregate           *aggregate,
                                           int                       fieldId,
                                           const bsl::vector<TYPE>&  source,
                                           bdeat_TypeCategory::Sequence);
    template <typename TYPE>
    static int toAggregateFromVectorOfNullable(
                                           bcem_Aggregate           *aggregate,
                                           int                       fieldId,
                                           const bsl::vector<TYPE>&  source,
                                           bdeat_TypeCategory::Simple);
        // Load into the sub-aggregate referred to by the specified 'fieldId'
        // in the specified 'aggregate' the value of the specified 'source'.
        // Return 0 on success and a non-zero value otherwise.  The behavior is
        // undefined unless 'source' refers to an object of vector of nullable
        // sequence, choice, simple, customized, or enumeration type.  Note
        // that a non-zero value is returned if the sub-aggregate does not
        // refer to an object of the 'bdem' array type corresponding to
        // 'TYPE::ValueType' or a 'bdem_ElemType::BDEM_TABLE'.

    template <typename TYPE>
    static int toAggregateFromVector(bcem_Aggregate           *aggregate,
                                     int                       fieldId,
                                     const bsl::vector<TYPE>&  source,
                                     bdeat_TypeCategory::Choice);
    template <typename TYPE>
    static int toAggregateFromVector(bcem_Aggregate           *aggregate,
                                     int                       fieldId,
                                     const bsl::vector<TYPE>&  source,
                                     bdeat_TypeCategory::CustomizedType);
    template <typename TYPE>
    static int toAggregateFromVector(bcem_Aggregate           *aggregate,
                                     int                       fieldId,
                                     const bsl::vector<TYPE>&  source,
                                     bdeat_TypeCategory::Enumeration);
    template <typename TYPE>
    static int toAggregateFromVector(bcem_Aggregate           *aggregate,
                                     int                       fieldId,
                                     const bsl::vector<TYPE>&  source,
                                     bdeat_TypeCategory::NullableValue);
    template <typename TYPE>
    static int toAggregateFromVector(bcem_Aggregate           *aggregate,
                                     int                       fieldId,
                                     const bsl::vector<TYPE>&  source,
                                     bdeat_TypeCategory::Sequence);
    template <typename TYPE>
    static int toAggregateFromVector(bcem_Aggregate           *aggregate,
                                     int                       fieldId,
                                     const bsl::vector<TYPE>&  source,
                                     bdeat_TypeCategory::Simple);
        // Load into the sub-aggregate referred to by the specified 'fieldId'
        // in the specified 'aggregate' the value of the specified 'source'.
        // Return 0 on success and a non-zero value otherwise.  The behavior is
        // undefined unless 'source' refers to an object of vector of sequence,
        // choice, customized, simple, or enumeration type, or a vector of
        // nullable sequence, choice, customized, simple, or enumeration type.
        // Note that a non-zero value is returned if the sub-aggregate does not
        // refer to an object of the 'bdem' array type corresponding to 'TYPE'
        // or a 'bdem_ElemType::BDEM_TABLE'.

    template <typename TYPE>
    static int toAggregateImp(bcem_Aggregate           *aggregate,
                              int                       fieldId,
                              const bsl::vector<TYPE>&  source,
                              bdeat_TypeCategory::Array);
    template <typename TYPE>
    static int toAggregateImp(bcem_Aggregate *aggregate,
                              int             fieldId,
                              const TYPE&     source,
                              bdeat_TypeCategory::Choice);
    template <typename TYPE>
    static int toAggregateImp(bcem_Aggregate *aggregate,
                              int             fieldId,
                              const TYPE&     source,
                              bdeat_TypeCategory::CustomizedType);
    template <typename TYPE>
    static int toAggregateImp(bcem_Aggregate *aggregate,
                              int             fieldId,
                              const TYPE&     source,
                              bdeat_TypeCategory::Enumeration);
    template <typename TYPE>
    static int toAggregateImp(bcem_Aggregate *aggregate,
                              int             fieldId,
                              const TYPE&     source,
                              bdeat_TypeCategory::NullableValue);
    template <typename TYPE>
    static int toAggregateImp(bcem_Aggregate *aggregate,
                              int             fieldId,
                              const TYPE&     source,
                              bdeat_TypeCategory::Sequence);
    template <typename TYPE>
    static int toAggregateImp(bcem_Aggregate *aggregate,
                              int             fieldId,
                              const TYPE&     source,
                              bdeat_TypeCategory::Simple);
        // Load into the sub-aggregate referred to by the specified 'fieldId'
        // in the specified 'aggregate' the value of the specified 'source'.
        // Return 0 on success and a non-zero value otherwise.  The behavior is
        // undefined unless 'destination' refers to an object of sequence,
        // choice, array, nullable, scalar, customized, or enumeration type.
        // Note that a non-zero value is returned if the sub-aggregate does not
        // refer to an object of the 'bdem' type corresponding to 'TYPE'.

  public:
    // CLASS METHODS
    static const char *errorString(int errorCode);
       // Return the error string corresponding to the specified 'errorCode',
       // where 'errorCode' is a value returned from any public class method
       // defined in this utility.

    template <typename TYPE>
    static int fromAggregate(TYPE                  *destination,
                             const bcem_Aggregate&  aggregate,
                             int                    fieldId);
        // Load into the specified 'destination' the value of the sub-aggregate
        // referred to by the specified 'fieldId' in the specified 'aggregate'.
        // Return 0 on success and a non-zero value otherwise.  The behavior is
        // undefined unless 'destination' refers to an object of sequence,
        // choice, array, nullable, scalar, customized, or enumeration type.
        // Note that a non-zero value is returned if the sub-aggregate does not
        // refer to an object of the 'bdem' type corresponding to 'TYPE'.

    template <typename TYPE>
    static int toAggregate(bcem_Aggregate *aggregate,
                           int             fieldId,
                           const TYPE&     source);
        // Load into the sub-aggregate referred to by the specified 'fieldId'
        // in the specified 'aggregate' the value of the specified 'source'.
        // Return 0 on success and a non-zero value otherwise.  The behavior is
        // undefined unless 'source' refers to an object of sequence, choice,
        // array, nullable, scalar, customized, or enumeration type.  Note that
        // a non-zero value is returned if the sub-aggregate does not refer to
        // an object of the 'bdem' type corresponding to 'TYPE'.
};

                          // ===================================
                          // class bcem_AggregateUtil_MakeSigned
                          // ===================================

template <typename TYPE>
struct bcem_AggregateUtil_MakeSigned {
    // This 'struct' defines a signed type, 'Type', corresponding to 'TYPE'.
    //
    // TBD Replace this custom type with bsl::make_signed<T> when it is
    // available.

    typedef TYPE Type;
};

template <>
struct bcem_AggregateUtil_MakeSigned<unsigned char> {
    typedef char Type;
};

template <>
struct bcem_AggregateUtil_MakeSigned<unsigned short> {
    typedef short Type;
};

template <>
struct bcem_AggregateUtil_MakeSigned<unsigned int> {
    typedef int Type;
};

template <>
struct bcem_AggregateUtil_MakeSigned<bsls::Types::Uint64> {
    typedef bsls::Types::Int64 Type;
};

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

                        // ------------------------
                        // class bcem_AggregateUtil
                        // ------------------------

// PRIVATE CLASS METHODS
template <typename TYPE>
bsl::vector<TYPE>&
bcem_AggregateUtil::theModifiableArray(const bdem_ElemRef& elemRef)
{
    BSLS_ASSERT_OPT(!"Unreachable");
    return *(bsl::vector<TYPE>*)0;
}

template <>
inline
bsl::vector<bool>&
bcem_AggregateUtil::theModifiableArray<bool>(const bdem_ElemRef& elemRef)
{
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_BOOL_ARRAY == elemRef.type());

    return elemRef.theModifiableBoolArray();
}

template <>
inline
bsl::vector<char>&
bcem_AggregateUtil::theModifiableArray<char>(const bdem_ElemRef& elemRef)
{
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_CHAR_ARRAY == elemRef.type());

    return elemRef.theModifiableCharArray();
}

template <>
inline
bsl::vector<short>&
bcem_AggregateUtil::theModifiableArray<short>(const bdem_ElemRef& elemRef)
{
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_SHORT_ARRAY == elemRef.type());

    return elemRef.theModifiableShortArray();
}

template <>
inline
bsl::vector<int>&
bcem_AggregateUtil::theModifiableArray<int>(const bdem_ElemRef& elemRef)
{
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_INT_ARRAY == elemRef.type());

    return elemRef.theModifiableIntArray();
}

template <>
inline
bsl::vector<bsls::Types::Int64>&
bcem_AggregateUtil::theModifiableArray<bsls::Types::Int64>(
                                                   const bdem_ElemRef& elemRef)
{
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_INT64_ARRAY == elemRef.type());

    return elemRef.theModifiableInt64Array();
}

template <>
inline
bsl::vector<float>&
bcem_AggregateUtil::theModifiableArray<float>(const bdem_ElemRef& elemRef)
{
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_FLOAT_ARRAY == elemRef.type());

    return elemRef.theModifiableFloatArray();
}

template <>
inline
bsl::vector<double>&
bcem_AggregateUtil::theModifiableArray<double>(const bdem_ElemRef& elemRef)
{
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_DOUBLE_ARRAY == elemRef.type());

    return elemRef.theModifiableDoubleArray();
}

template <>
inline
bsl::vector<bsl::string>&
bcem_AggregateUtil::theModifiableArray<bsl::string>(
                                                   const bdem_ElemRef& elemRef)
{
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_STRING_ARRAY == elemRef.type());

    return elemRef.theModifiableStringArray();
}

template <>
inline
bsl::vector<bdet_DateTz>&
bcem_AggregateUtil::theModifiableArray<bdet_DateTz>(
                                                   const bdem_ElemRef& elemRef)
{
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_DATETZ_ARRAY == elemRef.type());

    return elemRef.theModifiableDateTzArray();
}

template <>
inline
bsl::vector<bdet_DatetimeTz>&
bcem_AggregateUtil::theModifiableArray<bdet_DatetimeTz>(
                                                   const bdem_ElemRef& elemRef)
{
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_DATETIMETZ_ARRAY == elemRef.type());

    return elemRef.theModifiableDatetimeTzArray();
}

template <>
inline
bsl::vector<bdet_TimeTz>&
bcem_AggregateUtil::theModifiableArray<bdet_TimeTz>(
                                                   const bdem_ElemRef& elemRef)
{
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_TIMETZ_ARRAY == elemRef.type());

    return elemRef.theModifiableTimeTzArray();
}

template <>
inline
bsl::vector<bdet_Date>&
bcem_AggregateUtil::theModifiableArray<bdet_Date>(const bdem_ElemRef& elemRef)
{
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_DATE_ARRAY == elemRef.type());

    return elemRef.theModifiableDateArray();
}

template <>
inline
bsl::vector<bdet_Datetime>&
bcem_AggregateUtil::theModifiableArray<bdet_Datetime>(
                                                   const bdem_ElemRef& elemRef)
{
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_DATETIME_ARRAY == elemRef.type());

    return elemRef.theModifiableDatetimeArray();
}

template <>
inline
bsl::vector<bdet_Time>&
bcem_AggregateUtil::theModifiableArray<bdet_Time>(const bdem_ElemRef& elemRef)
{
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_TIME_ARRAY == elemRef.type());

    return elemRef.theModifiableTimeArray();
}

template <typename TYPE>
inline
int bcem_AggregateUtil::fromAggregateRawToEnumeration(
                                         TYPE                     *destination,
                                         const bcem_AggregateRaw&  aggregate)
{
    typedef typename bslalg_TypeTraits<TYPE>::Wrapper Wrapper;

    return Wrapper::fromString(destination, aggregate.asString());
}

template <typename TYPE>
int bcem_AggregateUtil::fromAggregateRawToSimple(
                                         TYPE                     *destination,
                                         const bcem_AggregateRaw&  aggregate)
{
    typedef typename bcem_AggregateUtil_MakeSigned<TYPE>::Type SIGNED_TYPE;

    if (aggregate.isNull()) {
        *destination = static_cast<TYPE>(
                                       bdetu_Unset<SIGNED_TYPE>::unsetValue());
    }
    else {
        *destination = static_cast<TYPE>(
                                       aggregate.convertScalar<SIGNED_TYPE>());
    }
    return 0;
}

template <typename TYPE>
inline
int bcem_AggregateUtil::fromAggregateRawImp(
                                         TYPE                     *destination,
                                         const bcem_AggregateRaw&  aggregate,
                                         bdeat_TypeCategory::Enumeration)
{
    return fromAggregateRawToEnumeration(destination, aggregate);
}

template <typename TYPE>
int bcem_AggregateUtil::fromAggregateRawImp(
                                         TYPE                     *destination,
                                         const bcem_AggregateRaw&  aggregate,
                                         bdeat_TypeCategory::Simple)
{
    if (!bdem_ElemType::isScalarType(aggregate.dataType())) {
        return -1;                                                    // RETURN
    }

    return fromAggregateRawToSimple(destination, aggregate);
}

template <typename TYPE>
int bcem_AggregateUtil::fromAggregateRawToNullable(
                                         TYPE                     *destination,
                                         const bcem_AggregateRaw&  aggregate)
{
    if (aggregate.isNull()) {
        destination->reset();
    }
    else {
        destination->makeValue();

        typedef typename
        bdeat_TypeCategory::Select<typename TYPE::ValueType>::Type TypeTag;

        return fromAggregateRawImp(&destination->value(),
                                   aggregate,
                                   TypeTag());                        // RETURN
    }
    return 0;
}

template <typename TYPE>
inline
int bcem_AggregateUtil::fromAggregateToComplex(
                                            TYPE                  *destination,
                                            const bcem_Aggregate&  aggregate)
{
    return destination->fromAggregate(aggregate);
}

template <typename TYPE>
int bcem_AggregateUtil::fromAggregateImp(TYPE                  *destination,
                                         const bcem_Aggregate&  aggregate,
                                         bdeat_TypeCategory::Array)
{
    // This case is only for handling 'bsl::vector<char>'.

    if (bdem_ElemType::BDEM_CHAR_ARRAY == aggregate.dataType()) {
        *destination = aggregate.asElemRef().theCharArray();
        return 0;                                                     // RETURN
    }
    return -1;
}

template <typename TYPE>
int bcem_AggregateUtil::fromAggregateImp(TYPE                  *destination,
                                         const bcem_Aggregate&  aggregate,
                                         bdeat_TypeCategory::Choice)
{
    if (bdem_ElemType::BDEM_CHOICE            == aggregate.dataType()
     || bdem_ElemType::BDEM_CHOICE_ARRAY_ITEM == aggregate.dataType()) {
        return fromAggregateToComplex(destination, aggregate);        // RETURN
    }
    return -1;
}

template <typename TYPE>
int bcem_AggregateUtil::fromAggregateImp(TYPE                  *destination,
                                         const bcem_Aggregate&  aggregate,
                                         bdeat_TypeCategory::CustomizedType)
{
    bdem_ElemType::Type expectedType = static_cast<bdem_ElemType::Type>(
                          bdem_SelectBdemType<typename TYPE::BaseType>::VALUE);

    if (expectedType == aggregate.dataType()) {
        return fromAggregateToComplex(destination, aggregate);        // RETURN
    }
    return -1;
}

template <typename TYPE>
inline
int bcem_AggregateUtil::fromAggregateImp(TYPE                  *destination,
                                         const bcem_Aggregate&  aggregate,
                                         bdeat_TypeCategory::Enumeration)
{
    return fromAggregateRawToEnumeration(destination,
                                         aggregate.aggregateRaw());
}

template <typename TYPE>
int bcem_AggregateUtil::fromAggregateImp(TYPE                  *destination,
                                         const bcem_Aggregate&  aggregate,
                                         bdeat_TypeCategory::Sequence)
{
    if (bdem_ElemType::BDEM_LIST == aggregate.dataType()
     || bdem_ElemType::BDEM_ROW  == aggregate.dataType()) {
        return fromAggregateToComplex(destination, aggregate);        // RETURN
    }
    return -1;
}

template <typename TYPE>
int bcem_AggregateUtil::fromAggregateImp(TYPE                  *destination,
                                         const bcem_Aggregate&  aggregate,
                                         bdeat_TypeCategory::Simple)
{
    if (!bdem_ElemType::isScalarType(aggregate.dataType())) {
        return -1;                                                    // RETURN
    }

    return fromAggregateRawToSimple(destination, aggregate.aggregateRaw());
}

template <typename TYPE>
int bcem_AggregateUtil::fromAggregateToNullable(
                                            TYPE                  *destination,
                                            const bcem_Aggregate&  aggregate)
{
    if (aggregate.isNul2()) {
        destination->reset();
    }
    else {
        destination->makeValue();

        typedef typename
        bdeat_TypeCategory::Select<typename TYPE::ValueType>::Type TypeTag;

        return fromAggregateImp(&destination->value(), aggregate, TypeTag());
                                                                      // RETURN
    }
    return 0;
}

template <typename TYPE, typename CONVERTER>
int bcem_AggregateUtil::fromAggregateRawToVectorOfPrimitive(
                                         bsl::vector<TYPE>        *destination,
                                         const bcem_AggregateRaw&  aggregate,
                                         CONVERTER                 converter)
{
    const int length = aggregate.length();
    destination->resize(length);

    bcem_AggregateRaw    item;
    bcem_ErrorAttributes error;
    for (int i = 0; i < length; ++i) {
        TYPE& element = (*destination)[i];
        aggregate.getArrayItem(&item, &error, i);

        const int rc = converter(&element, item);
        if (rc) {
            return rc;                                                // RETURN
        }
    }
    return 0;
}

template <typename TYPE, typename CONVERTER>
int bcem_AggregateUtil::fromAggregateToVectorOfComplex(
                                            bsl::vector<TYPE>     *destination,
                                            const bcem_Aggregate&  aggregate,
                                            CONVERTER              converter)
{
    const int length = aggregate.length();
    destination->resize(length);

    for (int i = 0; i < length; ++i) {
        TYPE& element = (*destination)[i];
        const int rc = converter(&element, aggregate[i]);
        if (rc) {
            return rc;                                                // RETURN
        }
    }
    return 0;
}

template <typename TYPE>
int bcem_AggregateUtil::fromAggregateToVectorOfNullable(
                                            bsl::vector<TYPE>     *destination,
                                            const bcem_Aggregate&  aggregate,
                                            int                    fieldId,
                                            bdeat_TypeCategory::Choice)
{
    const bcem_Aggregate& field = aggregate.fieldById(fieldId);
    if (field.isError()) {
        return field.errorCode();                                     // RETURN
    }

    if (bdem_ElemType::BDEM_CHOICE_ARRAY == field.dataType()) {
        return fromAggregateToVectorOfComplex(destination,
                                              field,
                                              fromAggregateToNullable<TYPE>);
                                                                      // RETURN
    }
    return -1;
}

template <typename TYPE>
int bcem_AggregateUtil::fromAggregateToVectorOfNullable(
                                            bsl::vector<TYPE>     *destination,
                                            const bcem_Aggregate&  aggregate,
                                            int                    fieldId,
                                            bdeat_TypeCategory::CustomizedType)
{
    const bcem_Aggregate& field = aggregate.fieldById(fieldId);
    if (field.isError()) {
        return field.errorCode();                                     // RETURN
    }

    bdem_ElemType::Type expectedType = bdem_ElemType::toArrayType(
              static_cast<bdem_ElemType::Type>(bdem_SelectBdemType<
                                  typename TYPE::ValueType::BaseType>::VALUE));

    if (bdem_ElemType::BDEM_TABLE == field.dataType()
     || expectedType              == field.dataType()) {
        return fromAggregateToVectorOfComplex(destination,
                                              field,
                                              fromAggregateToNullable<TYPE>);
                                                                      // RETURN
    }
    return -1;
}

template <typename TYPE>
int bcem_AggregateUtil::fromAggregateToVectorOfNullable(
                                            bsl::vector<TYPE>     *destination,
                                            const bcem_Aggregate&  aggregate,
                                            int                    fieldId,
                                            bdeat_TypeCategory::Enumeration)
{
    bcem_AggregateRaw    aggregateRaw = aggregate.aggregateRaw();
    bcem_AggregateRaw    field;
    bcem_ErrorAttributes error;
    if (0 != aggregateRaw.fieldById(&field, &error, fieldId)) {
        return error.code();                                          // RETURN
    }

    if (bdem_ElemType::BDEM_TABLE        == field.dataType()
     || bdem_ElemType::BDEM_INT_ARRAY    == field.dataType()
     || bdem_ElemType::BDEM_STRING_ARRAY == field.dataType()) {
        return fromAggregateRawToVectorOfPrimitive(
                                             destination,
                                             field,
                                             fromAggregateRawToNullable<TYPE>);
                                                                      // RETURN
    }
    return -1;
}

template <typename TYPE>
int bcem_AggregateUtil::fromAggregateToVectorOfNullable(
                                            bsl::vector<TYPE>     *destination,
                                            const bcem_Aggregate&  aggregate,
                                            int                    fieldId,
                                            bdeat_TypeCategory::Sequence)
{
    const bcem_Aggregate& field = aggregate.fieldById(fieldId);
    if (field.isError()) {
        return field.errorCode();                                     // RETURN
    }

    if (bdem_ElemType::BDEM_TABLE == field.dataType()) {
        return fromAggregateToVectorOfComplex(destination,
                                              field,
                                              fromAggregateToNullable<TYPE>);
                                                                      // RETURN
    }
    return -1;
}

template <typename TYPE>
int bcem_AggregateUtil::fromAggregateToVectorOfNullable(
                                            bsl::vector<TYPE>     *destination,
                                            const bcem_Aggregate&  aggregate,
                                            int                    fieldId,
                                            bdeat_TypeCategory::Simple)
{
    bcem_AggregateRaw    aggregateRaw = aggregate.aggregateRaw();
    bcem_AggregateRaw    field;
    bcem_ErrorAttributes error;
    if (0 != aggregateRaw.fieldById(&field, &error, fieldId)) {
        return error.code();                                          // RETURN
    }

    typedef typename
    bcem_AggregateUtil_MakeSigned<typename TYPE::ValueType>::Type SIGNED_TYPE;

    bdem_ElemType::Type expType = bdem_ElemType::toArrayType(
    static_cast<bdem_ElemType::Type>(bdem_SelectBdemType<SIGNED_TYPE>::VALUE));

    if (bdem_ElemType::BDEM_TABLE       == field.dataType()
     || expType                         == field.dataType()
     || bdem_ElemType::BDEM_SHORT_ARRAY == field.dataType()
     || bdem_ElemType::BDEM_INT_ARRAY   == field.dataType()
     || bdem_ElemType::BDEM_INT64_ARRAY == field.dataType()) {
        // When 'TYPE' refers to 'unsigned' types ('unsigned char',
        // 'unsigned short', and 'unsigned int') the corresponding 'bdem' types
        // are, respectively, 'short', 'int', and 'Int64'.  Due to this
        // mismatch the expected array types for 'vector<unsigned char>',
        // 'vector<unsigned short>', and 'vector<unsigned int>' are
        // 'BDEM_SHORT_ARRAY', 'BDEM_INT_ARRAY', and 'BDEM_INT64_ARRAY',
        // respectively.

        return fromAggregateRawToVectorOfPrimitive(
                                             destination,
                                             field,
                                             fromAggregateRawToNullable<TYPE>);
                                                                      // RETURN
    }
    return -1;
}

template <typename TYPE>
int bcem_AggregateUtil::fromAggregateToVector(
                                            bsl::vector<TYPE>     *destination,
                                            const bcem_Aggregate&  aggregate,
                                            int                    fieldId,
                                            bdeat_TypeCategory::Choice)
{
    bcem_Aggregate field  = aggregate.fieldById(fieldId);
    if (field.isError()) {
        return field.errorCode();                                     // RETURN
    }

    if (bdem_ElemType::BDEM_CHOICE_ARRAY == field.dataType()) {
        return fromAggregateToVectorOfComplex(destination,
                                              field,
                                              fromAggregateToComplex<TYPE>);
                                                                      // RETURN
    }
    return -1;
}

template <typename TYPE>
int bcem_AggregateUtil::fromAggregateToVector(
                                            bsl::vector<TYPE>     *destination,
                                            const bcem_Aggregate&  aggregate,
                                            int                    fieldId,
                                            bdeat_TypeCategory::CustomizedType)
{
    bcem_Aggregate field  = aggregate.fieldById(fieldId);
    if (field.isError()) {
        return field.errorCode();                                     // RETURN
    }

    bdem_ElemType::Type expectedType = bdem_ElemType::toArrayType(
                                              static_cast<bdem_ElemType::Type>(
                         bdem_SelectBdemType<typename TYPE::BaseType>::VALUE));

    if (expectedType == field.dataType()) {
        return fromAggregateToVectorOfComplex(destination,
                                              field,
                                              fromAggregateToComplex<TYPE>);
                                                                     // RETURN
    }
    return -1;
}

template <typename TYPE>
int bcem_AggregateUtil::fromAggregateToVector(
                                            bsl::vector<TYPE>     *destination,
                                            const bcem_Aggregate&  aggregate,
                                            int                    fieldId,
                                            bdeat_TypeCategory::Enumeration)
{
    bcem_AggregateRaw    aggregateRaw = aggregate.aggregateRaw();
    bcem_AggregateRaw    field;
    bcem_ErrorAttributes error;
    if (0 != aggregateRaw.fieldById(&field, &error, fieldId)) {
        return error.code();                                          // RETURN
    }

    if (bdem_ElemType::BDEM_INT_ARRAY    == field.dataType()
     || bdem_ElemType::BDEM_STRING_ARRAY == field.dataType()) {
        return fromAggregateRawToVectorOfPrimitive(
                                          destination,
                                          field,
                                          fromAggregateRawToEnumeration<TYPE>);
                                                                      // RETURN
    }
    return -1;
}

template <typename TYPE>
inline
int bcem_AggregateUtil::fromAggregateToVector(
                                            bsl::vector<TYPE>     *destination,
                                            const bcem_Aggregate&  aggregate,
                                            int                    fieldId,
                                            bdeat_TypeCategory::NullableValue)
{
    typedef typename TYPE::ValueType                              VALUE_TYPE;
    typedef typename bdeat_TypeCategory::Select<VALUE_TYPE>::Type TypeTag;

    return fromAggregateToVectorOfNullable(destination,
                                           aggregate,
                                           fieldId,
                                           TypeTag());
}

template <typename TYPE>
int bcem_AggregateUtil::fromAggregateToVector(
                                            bsl::vector<TYPE>     *destination,
                                            const bcem_Aggregate&  aggregate,
                                            int                    fieldId,
                                            bdeat_TypeCategory::Sequence)
{
    bcem_Aggregate field  = aggregate.fieldById(fieldId);
    if (field.isError()) {
        return field.errorCode();                                     // RETURN
    }

    if (bdem_ElemType::BDEM_TABLE == field.dataType()) {
        return fromAggregateToVectorOfComplex(destination,
                                              field,
                                              fromAggregateToComplex<TYPE>);
                                                                      // RETURN
    }
    return -1;
}

template <typename TYPE>
int bcem_AggregateUtil::fromAggregateToVector(
                                            bsl::vector<TYPE>     *destination,
                                            const bcem_Aggregate&  aggregate,
                                            int                    fieldId,
                                            bdeat_TypeCategory::Simple)
{
    bcem_AggregateRaw    aggregateRaw = aggregate.aggregateRaw();
    bcem_AggregateRaw    field;
    bcem_ErrorAttributes error;
    if (0 != aggregateRaw.fieldById(&field, &error, fieldId)) {
        return error.code();                                          // RETURN
    }

    typedef typename bcem_AggregateUtil_MakeSigned<TYPE>::Type SIGNED_TYPE;

    bdem_ElemType::Type expType = bdem_ElemType::toArrayType(
    static_cast<bdem_ElemType::Type>(bdem_SelectBdemType<SIGNED_TYPE>::VALUE));

    if (expType == field.dataType()) {
        const bsl::vector<SIGNED_TYPE>& array =
                            theModifiableArray<SIGNED_TYPE>(field.asElemRef());

        *destination = reinterpret_cast<const bsl::vector<TYPE>&>(array);
        return 0;                                                     // RETURN
    }
    else if (bdem_ElemType::BDEM_SHORT_ARRAY == field.dataType()
          || bdem_ElemType::BDEM_INT_ARRAY   == field.dataType()
          || bdem_ElemType::BDEM_INT64_ARRAY == field.dataType()) {
        // This is the case when 'TYPE' refers to 'unsigned' types
        // ('unsigned char', 'unsigned short', and 'unsigned int') and the
        // corresponding 'bdem' types are, respectively, 'short', 'int', and
        // 'Int64'.  Due to this mismatch the expected array types for
        // 'vector<unsigned char>', 'vector<unsigned short>', and
        // 'vector<unsigned int>' are 'BDEM_SHORT_ARRAY', 'BDEM_INT_ARRAY', and
        // 'BDEM_INT64_ARRAY', respectively.

        return fromAggregateRawToVectorOfPrimitive(
                                               destination,
                                               field,
                                               fromAggregateRawToSimple<TYPE>);
                                                                      // RETURN
    }
    return -1;
}

template <typename TYPE>
inline
int bcem_AggregateUtil::fromAggregateImp(bsl::vector<TYPE>     *destination,
                                         const bcem_Aggregate&  aggregate,
                                         int                    fieldId,
                                         bdeat_TypeCategory::Array)
{
    typedef typename bdeat_TypeCategory::Select<TYPE>::Type TypeTag;
    return fromAggregateToVector(destination, aggregate, fieldId, TypeTag());
}

template <typename TYPE>
int bcem_AggregateUtil::fromAggregateImp(TYPE                  *destination,
                                         const bcem_Aggregate&  aggregate,
                                         int                    fieldId,
                                         bdeat_TypeCategory::Choice)
{
    bcem_Aggregate field = aggregate.fieldById(fieldId);
    if (field.isError()) {
        return field.errorCode();                                     // RETURN
    }

    if (bdem_ElemType::BDEM_CHOICE == field.dataType()) {
        return fromAggregateToComplex(destination, field);            // RETURN
    }
    return -1;
}

template <typename TYPE>
int bcem_AggregateUtil::fromAggregateImp(TYPE                  *destination,
                                         const bcem_Aggregate&  aggregate,
                                         int                    fieldId,
                                         bdeat_TypeCategory::CustomizedType)
{
    bcem_Aggregate field = aggregate.fieldById(fieldId);
    if (field.isError()) {
        return field.errorCode();                                     // RETURN
    }

    bdem_ElemType::Type expectedType = static_cast<bdem_ElemType::Type>(
                          bdem_SelectBdemType<typename TYPE::BaseType>::VALUE);

    if (expectedType == field.dataType()) {
        return fromAggregateToComplex(destination, field);            // RETURN
    }
    return -1;
}

template <typename TYPE>
int bcem_AggregateUtil::fromAggregateImp(TYPE                  *destination,
                                         const bcem_Aggregate&  aggregate,
                                         int                    fieldId,
                                         bdeat_TypeCategory::Enumeration)
{
    bcem_AggregateRaw    aggregateRaw = aggregate.aggregateRaw();
    bcem_AggregateRaw    field;
    bcem_ErrorAttributes error;
    if (0 != aggregateRaw.fieldById(&field, &error, fieldId)) {
        return error.code();                                          // RETURN
    }

    return fromAggregateRawToEnumeration(destination, field);
}

template <typename TYPE>
int bcem_AggregateUtil::fromAggregateImp(TYPE                  *destination,
                                         const bcem_Aggregate&  aggregate,
                                         int                    fieldId,
                                         bdeat_TypeCategory::NullableValue)
{
    const bcem_Aggregate& field = aggregate.fieldById(fieldId);
    if (field.isError()) {
        return field.errorCode();                                     // RETURN
    }

    return fromAggregateToNullable(destination, field);
}

template <typename TYPE>
int bcem_AggregateUtil::fromAggregateImp(TYPE                  *destination,
                                         const bcem_Aggregate&  aggregate,
                                         int                    fieldId,
                                         bdeat_TypeCategory::Sequence)
{
    bcem_Aggregate field = aggregate.fieldById(fieldId);
    if (field.isError()) {
        return field.errorCode();                                     // RETURN
    }

    if (bdem_ElemType::BDEM_LIST == field.dataType()) {
        return fromAggregateToComplex(destination, field);            // RETURN
    }
    return -1;
}

template <typename TYPE>
int bcem_AggregateUtil::fromAggregateImp(TYPE                  *destination,
                                         const bcem_Aggregate&  aggregate,
                                         int                    fieldId,
                                         bdeat_TypeCategory::Simple)
{
    bcem_AggregateRaw    aggregateRaw = aggregate.aggregateRaw();
    bcem_AggregateRaw    field;
    bcem_ErrorAttributes error;
    if (0 != aggregateRaw.fieldById(&field, &error, fieldId)) {
        return error.code();                                          // RETURN
    }

    if (!bdem_ElemType::isScalarType(field.dataType())) {
        return -1;                                                    // RETURN
    }

    return fromAggregateRawToSimple(destination, field);
}

template <typename TYPE>
inline
int bcem_AggregateUtil::toAggregateRawFromEnumeration(
                                                bcem_AggregateRaw *aggregate,
                                                const TYPE&        source)
{
    typedef typename bslalg_TypeTraits<TYPE>::Wrapper Wrapper;

    bcem_ErrorAttributes errorDescription;
    aggregate->setValue(&errorDescription, Wrapper::toString(source));
    return errorDescription.code();
}

template <typename TYPE>
inline
int bcem_AggregateUtil::toAggregateRawFromSimple(bcem_AggregateRaw *aggregate,
                                                 const TYPE&        source)
{
    // 'setValue' works for objects of both signed and unsigned types as
    // 'bdem_Convert' converts from both signed and unsigned primitive
    // types.

    bcem_ErrorAttributes error;
    aggregate->setValue(&error, source);
    return error.code();
}

template <typename TYPE>
inline
int bcem_AggregateUtil::toAggregateFromComplex(bcem_Aggregate *aggregate,
                                               const TYPE&     source)
{
    return source.toAggregate(aggregate);
}

template <typename TYPE>
inline
int bcem_AggregateUtil::toAggregateRawImp(bcem_AggregateRaw *aggregate,
                                          const TYPE&        source,
                                          bdeat_TypeCategory::Enumeration)
{
    return toAggregateRawFromEnumeration(aggregate, source);
}

template <typename TYPE>
int bcem_AggregateUtil::toAggregateRawImp(bcem_AggregateRaw *aggregate,
                                          const TYPE&        source,
                                          bdeat_TypeCategory::Simple)
{
    if (!bdem_ElemType::isScalarType(aggregate->dataType())) {
        return -1;                                                    // RETURN
    }

    return toAggregateRawFromSimple(aggregate, source);
}

template <typename TYPE>
int bcem_AggregateUtil::toAggregateRawFromNullable(
                                                  bcem_AggregateRaw *aggregate,
                                                  const TYPE&        source)
{
    if (source.isNull()) {
        aggregate->makeNull();
    }
    else {
        aggregate->makeValue();

        typedef typename
        bdeat_TypeCategory::Select<typename TYPE::ValueType>::Type TypeTag;

        return toAggregateRawImp(aggregate, source.value(), TypeTag());
                                                                      // RETURN
    }
    return 0;
}

template <typename TYPE, typename CONVERTER>
int bcem_AggregateUtil::toAggregateRawFromVectorOfPrimitive(
                                           bcem_AggregateRaw        *aggregate,
                                           const bsl::vector<TYPE>&  source,
                                           CONVERTER                 converter)
{
    const int length = static_cast<int>(source.size());

    bcem_ErrorAttributes error;
    aggregate->resize(&error, length);

    bcem_AggregateRaw item;
    for (int i = 0; i < length; ++i) {
        aggregate->getArrayItem(&item, &error, i);

        const int rc = converter(&item, source[i]);
        if (rc) {
            return rc;                                                // RETURN
        }
    }
    return 0;
}

template <typename TYPE, typename CONVERTER>
int bcem_AggregateUtil::toAggregateFromVectorOfComplex(
                                           bcem_Aggregate           *aggregate,
                                           const bsl::vector<TYPE>&  source,
                                           CONVERTER                 converter)
{
    const int length = static_cast<int>(source.size());
    aggregate->resize(length);

    for (int i = 0; i < length; ++i) {
        bcem_Aggregate item = (*aggregate)[i];
        const int rc = converter(&item, source[i]);
        if (rc) {
            return rc;                                                // RETURN
        }
    }
    return 0;
}

template <typename TYPE>
int bcem_AggregateUtil::toAggregateFromVectorOfNullable(
                                           bcem_Aggregate           *aggregate,
                                           int                       fieldId,
                                           const bsl::vector<TYPE>&  source,
                                           bdeat_TypeCategory::Choice)
{
    bcem_Aggregate field = aggregate->fieldById(fieldId);
    if (field.isError()) {
        return field.errorCode();                                     // RETURN
    }

    if (bdem_ElemType::BDEM_CHOICE_ARRAY == field.dataType()) {
        return toAggregateFromVectorOfComplex(&field,
                                              source,
                                              toAggregateFromNullable<TYPE>);
                                                                      // RETURN
    }
    return -1;
}

template <typename TYPE>
int bcem_AggregateUtil::toAggregateFromVectorOfNullable(
                                           bcem_Aggregate           *aggregate,
                                           int                       fieldId,
                                           const bsl::vector<TYPE>&  source,
                                           bdeat_TypeCategory::CustomizedType)
{
    bcem_Aggregate field = aggregate->fieldById(fieldId);
    if (field.isError()) {
        return field.errorCode();                                     // RETURN
    }

    bdem_ElemType::Type expectedType = bdem_ElemType::toArrayType(
              static_cast<bdem_ElemType::Type>(bdem_SelectBdemType<
                                  typename TYPE::ValueType::BaseType>::VALUE));

    if (bdem_ElemType::BDEM_TABLE == field.dataType()
     || expectedType              == field.dataType()) {
        return toAggregateFromVectorOfComplex(&field,
                                              source,
                                              toAggregateFromNullable<TYPE>);
                                                                      // RETURN
    }
    return -1;
}

template <typename TYPE>
int bcem_AggregateUtil::toAggregateFromVectorOfNullable(
                                           bcem_Aggregate           *aggregate,
                                           int                       fieldId,
                                           const bsl::vector<TYPE>&  source,
                                           bdeat_TypeCategory::Enumeration)
{
    bcem_AggregateRaw    aggregateRaw = aggregate->aggregateRaw();
    bcem_AggregateRaw    field;
    bcem_ErrorAttributes error;
    if (0 != aggregateRaw.fieldById(&field, &error, fieldId)) {
        return error.code();                                          // RETURN
    }

    if (bdem_ElemType::BDEM_TABLE        == field.dataType()
     || bdem_ElemType::BDEM_INT_ARRAY    == field.dataType()
     || bdem_ElemType::BDEM_STRING_ARRAY == field.dataType()) {
        return toAggregateRawFromVectorOfPrimitive(
                                             &field,
                                             source,
                                             toAggregateRawFromNullable<TYPE>);
                                                                      // RETURN
    }
    return -1;
}

template <typename TYPE>
int bcem_AggregateUtil::toAggregateFromVectorOfNullable(
                                           bcem_Aggregate           *aggregate,
                                           int                       fieldId,
                                           const bsl::vector<TYPE>&  source,
                                           bdeat_TypeCategory::Sequence)
{
    bcem_Aggregate field  = aggregate->fieldById(fieldId);
    if (field.isError()) {
        return field.errorCode();                                     // RETURN
    }

    if (bdem_ElemType::BDEM_TABLE == field.dataType()) {
        return toAggregateFromVectorOfComplex(&field,
                                              source,
                                              toAggregateFromNullable<TYPE>);
                                                                      // RETURN
    }
    return -1;
}

template <typename TYPE>
int bcem_AggregateUtil::toAggregateFromVectorOfNullable(
                                           bcem_Aggregate           *aggregate,
                                           int                       fieldId,
                                           const bsl::vector<TYPE>&  source,
                                           bdeat_TypeCategory::Simple)
{
    bcem_AggregateRaw    aggregateRaw = aggregate->aggregateRaw();
    bcem_AggregateRaw    field;
    bcem_ErrorAttributes error;
    if (0 != aggregateRaw.fieldById(&field, &error, fieldId)) {
        return error.code();                                          // RETURN
    }

    typedef typename
    bcem_AggregateUtil_MakeSigned<typename TYPE::ValueType>::Type SIGNED_TYPE;

    bdem_ElemType::Type expectedType = bdem_ElemType::toArrayType(
    static_cast<bdem_ElemType::Type>(bdem_SelectBdemType<SIGNED_TYPE>::VALUE));

    if (bdem_ElemType::BDEM_TABLE       == field.dataType()
     || expectedType                    == field.dataType()
     || bdem_ElemType::BDEM_SHORT_ARRAY == field.dataType()
     || bdem_ElemType::BDEM_INT_ARRAY   == field.dataType()
     || bdem_ElemType::BDEM_INT64_ARRAY == field.dataType()) {
        // When 'TYPE' refers to 'unsigned' types ('unsigned char',
        // 'unsigned short', and 'unsigned int') the corresponding 'bdem' types
        // are, respectively, 'short', 'int', and 'Int64'.  Due to this
        // mismatch the expected array types for 'vector<unsigned char>',
        // 'vector<unsigned short>', and 'vector<unsigned int>' are
        // 'BDEM_SHORT_ARRAY', 'BDEM_INT_ARRAY', and 'BDEM_INT64_ARRAY'
        //, respectively.

        return toAggregateRawFromVectorOfPrimitive(
                                             &field,
                                             source,
                                             toAggregateRawFromNullable<TYPE>);
                                                                      // RETURN
    }
    return -1;
}

template <typename TYPE>
int bcem_AggregateUtil::toAggregateImp(bcem_Aggregate *aggregate,
                                       const TYPE&     source,
                                       bdeat_TypeCategory::Array)
{
    // This case is only for handling 'bsl::vector<char>'.

    if (bdem_ElemType::BDEM_CHAR_ARRAY == aggregate->dataType()) {
        aggregate->asElemRef().theModifiableCharArray() = source;
        return 0;                                                     // RETURN
    }
    return -1;
}

template <typename TYPE>
int bcem_AggregateUtil::toAggregateImp(bcem_Aggregate *aggregate,
                                       const TYPE&     source,
                                       bdeat_TypeCategory::Choice)
{
    if (bdem_ElemType::BDEM_CHOICE            == aggregate->dataType()
     || bdem_ElemType::BDEM_CHOICE_ARRAY_ITEM == aggregate->dataType()) {
        return toAggregateFromComplex(aggregate, source);             // RETURN
    }
    return -1;
}

template <typename TYPE>
int bcem_AggregateUtil::toAggregateImp(bcem_Aggregate *aggregate,
                                       const TYPE&     source,
                                       bdeat_TypeCategory::CustomizedType)
{
    bdem_ElemType::Type expectedType = static_cast<bdem_ElemType::Type>(
                          bdem_SelectBdemType<typename TYPE::BaseType>::VALUE);
    if (expectedType == aggregate->dataType()) {
        return toAggregateFromComplex(aggregate, source);             // RETURN
    }
    return -1;
}

template <typename TYPE>
inline
int bcem_AggregateUtil::toAggregateImp(bcem_Aggregate *aggregate,
                                       const TYPE&     source,
                                       bdeat_TypeCategory::Enumeration)
{
    return toAggregateRawFromEnumeration(&aggregate->aggregateRaw(), source);
}

template <typename TYPE>
int bcem_AggregateUtil::toAggregateImp(bcem_Aggregate *aggregate,
                                       const TYPE&     source,
                                       bdeat_TypeCategory::Sequence)
{
    if (bdem_ElemType::BDEM_LIST == aggregate->dataType()
     || bdem_ElemType::BDEM_ROW  == aggregate->dataType()) {
        return toAggregateFromComplex(aggregate, source);             // RETURN
    }
    return -1;
}

template <typename TYPE>
int bcem_AggregateUtil::toAggregateImp(bcem_Aggregate *aggregate,
                                       const TYPE&     source,
                                       bdeat_TypeCategory::Simple)
{
    if (!bdem_ElemType::isScalarType(aggregate->dataType())) {
        return -1;                                                    // RETURN
    }

    return toAggregateRawFromSimple(&aggregate->aggregateRaw(), source);
}

template <typename TYPE>
int bcem_AggregateUtil::toAggregateFromNullable(bcem_Aggregate *aggregate,
                                                const TYPE&     source)
{
    if (source.isNull()) {
        aggregate->makeNull();
    }
    else {
        aggregate->makeValue();

        typedef typename
        bdeat_TypeCategory::Select<typename TYPE::ValueType>::Type TypeTag;

        return toAggregateImp(aggregate, source.value(), TypeTag());  // RETURN
    }
    return 0;
}

template <typename TYPE>
int bcem_AggregateUtil::toAggregateFromVector(
                                           bcem_Aggregate           *aggregate,
                                           int                       fieldId,
                                           const bsl::vector<TYPE>&  source,
                                           bdeat_TypeCategory::Enumeration)
{
    bcem_AggregateRaw    aggregateRaw = aggregate->aggregateRaw();
    bcem_AggregateRaw    field;
    bcem_ErrorAttributes error;
    if (0 != aggregateRaw.fieldById(&field, &error, fieldId)) {
        return error.code();                                          // RETURN
    }

    if (bdem_ElemType::BDEM_INT_ARRAY    == field.dataType()
     || bdem_ElemType::BDEM_STRING_ARRAY == field.dataType()) {
        return toAggregateRawFromVectorOfPrimitive(
                                          &field,
                                          source,
                                          toAggregateRawFromEnumeration<TYPE>);
                                                                      // RETURN
    }
    return -1;
}

template <typename TYPE>
inline
int bcem_AggregateUtil::toAggregateFromVector(
                                           bcem_Aggregate           *aggregate,
                                           int                       fieldId,
                                           const bsl::vector<TYPE>&  source,
                                           bdeat_TypeCategory::NullableValue)
{
    typedef typename TYPE::ValueType                           VALUE_TYPE;
    typedef typename
    bdeat_TypeCategory::Select<typename TYPE::ValueType>::Type TypeTag;

    return toAggregateFromVectorOfNullable(aggregate,
                                           fieldId,
                                           source,
                                           TypeTag());
}

template <typename TYPE>
int bcem_AggregateUtil::toAggregateFromVector(
                                           bcem_Aggregate           *aggregate,
                                           int                       fieldId,
                                           const bsl::vector<TYPE>&  source,
                                           bdeat_TypeCategory::Choice)
{
    bcem_Aggregate field = aggregate->fieldById(fieldId);
    if (field.isError()) {
        return field.errorCode();                                     // RETURN
    }

    if (bdem_ElemType::BDEM_CHOICE_ARRAY == field.dataType()) {
        return toAggregateFromVectorOfComplex(&field,
                                              source,
                                              toAggregateFromComplex<TYPE>);
                                                                      // RETURN
    }
    return -1;
}

template <typename TYPE>
int bcem_AggregateUtil::toAggregateFromVector(
                                           bcem_Aggregate           *aggregate,
                                           int                       fieldId,
                                           const bsl::vector<TYPE>&  source,
                                           bdeat_TypeCategory::CustomizedType)
{
    bcem_Aggregate field = aggregate->fieldById(fieldId);
    if (field.isError()) {
        return field.errorCode();                                     // RETURN
    }

    bdem_ElemType::Type expectedType = bdem_ElemType::toArrayType(
                                              static_cast<bdem_ElemType::Type>(
                         bdem_SelectBdemType<typename TYPE::BaseType>::VALUE));

    if (expectedType == field.dataType()) {
        return toAggregateFromVectorOfComplex(&field,
                                              source,
                                              toAggregateFromComplex<TYPE>);
                                                                      // RETURN
    }
    return -1;
}

template <typename TYPE>
int bcem_AggregateUtil::toAggregateFromVector(
                                           bcem_Aggregate           *aggregate,
                                           int                       fieldId,
                                           const bsl::vector<TYPE>&  source,
                                           bdeat_TypeCategory::Sequence)
{
    bcem_Aggregate field = aggregate->fieldById(fieldId);
    if (field.isError()) {
        return field.errorCode();                                     // RETURN
    }

    if (bdem_ElemType::BDEM_TABLE == field.dataType()) {
        return toAggregateFromVectorOfComplex(&field,
                                              source,
                                              toAggregateFromComplex<TYPE>);
                                                                      // RETURN
    }
    return -1;
}

template <typename TYPE>
int bcem_AggregateUtil::toAggregateFromVector(
                                           bcem_Aggregate           *aggregate,
                                           int                       fieldId,
                                           const bsl::vector<TYPE>&  source,
                                           bdeat_TypeCategory::Simple)
{
    bcem_AggregateRaw    aggregateRaw = aggregate->aggregateRaw();
    bcem_AggregateRaw    field;
    bcem_ErrorAttributes error;
    if (0 != aggregateRaw.fieldById(&field, &error, fieldId)) {
        return error.code();                                          // RETURN
    }

    typedef typename bcem_AggregateUtil_MakeSigned<TYPE>::Type SIGNED_TYPE;

    bdem_ElemType::Type expType = bdem_ElemType::toArrayType(
    static_cast<bdem_ElemType::Type>(bdem_SelectBdemType<SIGNED_TYPE>::VALUE));

    if (expType == field.dataType()) {
        bsl::vector<SIGNED_TYPE>& array = theModifiableArray<SIGNED_TYPE>(
                     static_cast<const bcem_AggregateRaw&>(field).asElemRef());

        array = reinterpret_cast<const bsl::vector<SIGNED_TYPE>&>(source);

        return 0;                                                     // RETURN
    }
    else if (bdem_ElemType::BDEM_SHORT_ARRAY == field.dataType()
          || bdem_ElemType::BDEM_INT_ARRAY   == field.dataType()
          || bdem_ElemType::BDEM_INT64_ARRAY == field.dataType()) {
        // This is the case when 'TYPE' refers to 'unsigned' types
        // ('unsigned char', 'unsigned short', and 'unsigned int') and the
        // corresponding 'bdem' types are, respectively, 'short', 'int', and
        // 'Int64'.  Due to this mismatch the expected array types for
        // 'vector<unsigned char>', 'vector<unsigned short>', and
        // 'vector<unsigned int>' are 'BDEM_SHORT_ARRAY', 'BDEM_INT_ARRAY', and
        // 'BDEM_INT64_ARRAY', respectively.

        return toAggregateRawFromVectorOfPrimitive(
                                               &field,
                                               source,
                                               toAggregateRawFromSimple<TYPE>);
                                                                      // RETURN
    }
    return -1;
}

template <typename TYPE>
inline
int bcem_AggregateUtil::toAggregateImp(bcem_Aggregate           *aggregate,
                                       int                       fieldId,
                                       const bsl::vector<TYPE>&  source,
                                       bdeat_TypeCategory::Array)
{
    typedef typename bdeat_TypeCategory::Select<TYPE>::Type TypeTag;
    return toAggregateFromVector(aggregate, fieldId, source, TypeTag());
}

template <typename TYPE>
int bcem_AggregateUtil::toAggregateImp(bcem_Aggregate *aggregate,
                                       int             fieldId,
                                       const TYPE&     source,
                                       bdeat_TypeCategory::Enumeration)
{
    bcem_AggregateRaw    aggregateRaw = aggregate->aggregateRaw();
    bcem_AggregateRaw    field;
    bcem_ErrorAttributes error;
    if (0 != aggregateRaw.fieldById(&field, &error, fieldId)) {
        return error.code();                                          // RETURN
    }

    return toAggregateRawFromEnumeration(&field, source);
}

template <typename TYPE>
int bcem_AggregateUtil::toAggregateImp(bcem_Aggregate *aggregate,
                                       int             fieldId,
                                       const TYPE&     source,
                                       bdeat_TypeCategory::NullableValue)
{
    bcem_Aggregate field = aggregate->fieldById(fieldId);
    if (field.isError()) {
        return field.errorCode();                                     // RETURN
    }

    return toAggregateFromNullable(&field, source);
}

template <typename TYPE>
int bcem_AggregateUtil::toAggregateImp(bcem_Aggregate *aggregate,
                                       int             fieldId,
                                       const TYPE&     source,
                                       bdeat_TypeCategory::CustomizedType)
{
    bcem_Aggregate field = aggregate->fieldById(fieldId);
    if (field.isError()) {
        return field.errorCode();                                     // RETURN
    }

    bdem_ElemType::Type expectedType = static_cast<bdem_ElemType::Type>(
                          bdem_SelectBdemType<typename TYPE::BaseType>::VALUE);

    if (expectedType == field.dataType()) {
        return toAggregateFromComplex(&field, source);                // RETURN
    }
    return -1;
}

template <typename TYPE>
int bcem_AggregateUtil::toAggregateImp(bcem_Aggregate *aggregate,
                                       int             fieldId,
                                       const TYPE&     source,
                                       bdeat_TypeCategory::Choice)
{
    bcem_Aggregate field = aggregate->fieldById(fieldId);
    if (field.isError()) {
        return field.errorCode();                                     // RETURN
    }

    if (bdem_ElemType::BDEM_CHOICE == field.dataType()) {
        return toAggregateFromComplex(&field, source);                // RETURN
    }
    return -1;
}

template <typename TYPE>
int bcem_AggregateUtil::toAggregateImp(bcem_Aggregate *aggregate,
                                       int             fieldId,
                                       const TYPE&     source,
                                       bdeat_TypeCategory::Sequence)
{
    bcem_Aggregate field = aggregate->fieldById(fieldId);
    if (field.isError()) {
        return field.errorCode();                                     // RETURN
    }

    if (bdem_ElemType::BDEM_LIST == field.dataType()) {
        return toAggregateFromComplex(&field, source);                // RETURN
    }
    return -1;
}

template <typename TYPE>
int bcem_AggregateUtil::toAggregateImp(bcem_Aggregate *aggregate,
                                       int             fieldId,
                                       const TYPE&     source,
                                       bdeat_TypeCategory::Simple)
{
    bcem_AggregateRaw    aggregateRaw = aggregate->aggregateRaw();
    bcem_AggregateRaw    field;
    bcem_ErrorAttributes error;
    if (0 != aggregateRaw.fieldById(&field, &error, fieldId)) {
        return error.code();                                          // RETURN
    }

    if (!bdem_ElemType::isScalarType(field.dataType())) {
        return -1;                                                    // RETURN
    }

    return toAggregateRawFromSimple(&field, source);
}

// CLASS METHODS
template <typename TYPE>
inline
int bcem_AggregateUtil::fromAggregate(TYPE                  *destination,
                                      const bcem_Aggregate&  aggregate,
                                      int                    fieldId)
{
    typedef typename bdeat_TypeCategory::Select<TYPE>::Type TypeTag;
    return fromAggregateImp(destination, aggregate, fieldId, TypeTag());
}

template <typename TYPE>
inline
int bcem_AggregateUtil::toAggregate(bcem_Aggregate *aggregate,
                                    int             fieldId,
                                    const TYPE&     source)
{
    typedef typename bdeat_TypeCategory::Select<TYPE>::Type TypeTag;
    return toAggregateImp(aggregate, fieldId, source, TypeTag());
}

}  // close namespace BloombergLP

#endif

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2008
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ------------------------------ END-OF-FILE ---------------------------------
