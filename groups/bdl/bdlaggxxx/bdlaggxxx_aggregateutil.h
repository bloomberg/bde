// bdlaggxxx_aggregateutil.h                                          -*-C++-*-
#ifndef INCLUDED_BDLAGGXXX_AGGREGATEUTIL
#define INCLUDED_BDLAGGXXX_AGGREGATEUTIL

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide utilities for manipulating 'bdlaggxxx::Aggregate' objects.
//
//@CLASSES:
//   bdlaggxxx::AggregateUtil: utility functions for 'bdlaggxxx::Aggregate' objects
//
//@AUTHOR: David Rubin (drubin6), Rohan Bhindwale (rbhindwa)
//
//@SEE_ALSO: bdlaggxxx_aggregate
//
//@DESCRIPTION: This component provides utilities to convert the field(s) of a
// 'bdlaggxxx::Aggregate' of "List" or "Choice" type to and from values of
// corresponding C++ types.  For a detailed description of 'bdlaggxxx::Aggregate'
// types see the component-level documentation for 'bdlaggxxx_aggregate'.
//
///Thread-Safety
///-------------
// All methods of 'bdlaggxxx::AggregateUtil' are thread-safe.
//
///Usage Example
///-------------
//  The following example shows how to convert a 'bdlaggxxx::Aggregate' to a
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
//  bslma::Allocator *allocator = bslma::Default::defaultAllocator();
//
//  bsl::shared_ptr<bdlmxxx::Schema> schema(new (*allocator) bdlmxxx::Schema());
//
//  bdlmxxx::RecordDef *companyRecordDef = schema->createRecord("Company");
//  companyRecordDef->appendField(bdlmxxx::ElemType::BDEM_STRING, "Name");
//  companyRecordDef->appendField(bdlmxxx::ElemType::BDEM_INT,    "AccountNum");
//
//  bdlmxxx::RecordDef *personRecordDef = schema->createRecord("Person");
//  personRecordDef->appendField(bdlmxxx::ElemType::BDEM_STRING,  "LastName");
//  personRecordDef->appendField(bdlmxxx::ElemType::BDEM_STRING,  "FirstName");
//  personRecordDef->appendField(bdlmxxx::ElemType::BDEM_INT,     "Age");
//  personRecordDef->appendField(bdlmxxx::ElemType::BDEM_DATETZ,  "BirthDate");
//
//  bdlmxxx::RecordDef *entityRecordDef =
//         schema->createRecord("Entity", bdlmxxx::RecordDef::BDEM_CHOICE_RECORD);
//  entityRecordDef->appendField(bdlmxxx::ElemType::BDEM_LIST,
//                               companyRecordDef,
//                               "Corp");
//  entityRecordDef->appendField(bdlmxxx::ElemType::BDEM_LIST,
//                               personRecordDef,
//                               "Human");
//
//..
// Initialize the aggregate.
//..
//  bdlaggxxx::Aggregate entityAgg(schema, "Entity");
//
//  bdlaggxxx::Aggregate entityAggSelection = entityAgg.makeSelection("Human");
//  entityAggSelection.setField("FirstName", "John");
//  entityAggSelection.setField("LastName", "Doe");
//  entityAggSelection.setField("Age", 27);
//  entityAggSelection.setField("BirthDate",
//                              bdlt::DateTz(bdlt::Date(1980, 4, 4), -5));
//
//..
// Convert the aggregate to a bdeat-compatible type.
//..
//  test::Entity entity;
//  int rc;
//
//  rc = bdlaggxxx::AggregateUtil::fromAggregate(&entity.makeHuman(),
//                                         entityAgg,
//                                         test::Entity::SELECTION_ID_HUMAN);
//  BSLS_ASSERT(0 == rc);
//..

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

#ifndef INCLUDED_BDLAGGXXX_AGGREGATE
#include <bdlaggxxx_aggregate.h>
#endif

#ifndef INCLUDED_BDLAGGXXX_AGGREGATERAW
#include <bdlaggxxx_aggregateraw.h>
#endif

#ifndef INCLUDED_BDLAGGXXX_ERRORATTRIBUTES
#include <bdlaggxxx_errorattributes.h>
#endif

#ifndef INCLUDED_BDLAT_CHOICEFUNCTIONS
#include <bdlat_choicefunctions.h>
#endif

#ifndef INCLUDED_BDLAT_CUSTOMIZEDTYPEFUNCTIONS
#include <bdlat_customizedtypefunctions.h>
#endif

#ifndef INCLUDED_BDLAT_TYPECATEGORY
#include <bdlat_typecategory.h>
#endif

#ifndef INCLUDED_BDLMXXX_ELEMREF
#include <bdlmxxx_elemref.h>
#endif

#ifndef INCLUDED_BDLMXXX_ELEMTYPE
#include <bdlmxxx_elemtype.h>
#endif

#ifndef INCLUDED_BDLMXXX_SELECTBDEMTYPE
#include <bdlmxxx_selectbdemtype.h>
#endif

#ifndef INCLUDED_BDLT_DATE
#include <bdlt_date.h>
#endif

#ifndef INCLUDED_BDLT_DATETIME
#include <bdlt_datetime.h>
#endif

#ifndef INCLUDED_BDLT_DATETIMETZ
#include <bdlt_datetimetz.h>
#endif

#ifndef INCLUDED_BDLT_DATETZ
#include <bdlt_datetz.h>
#endif

#ifndef INCLUDED_BDLT_TIME
#include <bdlt_time.h>
#endif

#ifndef INCLUDED_BDLT_TIMETZ
#include <bdlt_timetz.h>
#endif

#ifndef INCLUDED_BDLTUXXX_UNSET
#include <bdltuxxx_unset.h>
#endif

#ifndef INCLUDED_BDLB_NULLABLEALLOCATEDVALUE
#include <bdlb_nullableallocatedvalue.h>
#endif

#ifndef INCLUDED_BDLB_NULLABLEVALUE
#include <bdlb_nullablevalue.h>
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

namespace bdlaggxxx {
                          // ========================
                          // class AggregateUtil
                          // ========================

class AggregateUtil {
    // Provide a namespace for a suite of functions for manipulating
    // 'Aggregate' objects.  The 'toAggregate' and 'fromAggregate'
    // functions convert between aggregates and primitive types (including
    // 'bsl::string', 'bsls' 64-bit integral types, and a variety of 'bdet'
    // time and date types), nullable primitive types, arrays of primitive
    // types, and arrays of nullable primitive types.

  private:
    // PRIVATE CLASS METHODS
    template <class TYPE>
    static bsl::vector<TYPE>& theModifiableArray(const bdlmxxx::ElemRef& elemRef);
        // Return a reference providing modifiable access to the vector
        // referred by the specified 'elemRef'.  The behavior is undefined
        // unless 'elemRef' refers to an object of the 'bdem' array type
        // corresponding to 'TYPE'.

    template <class TYPE>
    static int fromAggregateRawToEnumeration(
                                         TYPE                     *destination,
                                         const AggregateRaw&  aggregate);
        // Load into the specified 'destination' the value of the specified
        // 'aggregate'.  Return 0 on success and a non-zero value otherwise.
        // The behavior is undefined unless 'destination' refers to an object
        // of enumeration type.  Note that a non-zero value is returned if
        // 'aggregate' does not refer to an object of type convertible to a
        // string.

    template <class TYPE>
    static int fromAggregateRawToSimple(TYPE                     *destination,
                                        const AggregateRaw&  aggregate);
        // Load into the specified 'destination' the value of the specified
        // 'aggregate'.  Return 0 on success and a non-zero value otherwise.
        // The behavior is undefined unless 'destination' refers to an object
        // of simple type and 'aggregate' refers to an object of scalar type.

    template <class TYPE>
    static int fromAggregateRawImp(TYPE                     *destination,
                                   const AggregateRaw&  aggregate,
                                   bdeat_TypeCategory::Enumeration);
        // Load into the specified 'destination' the value of the specified
        // 'aggregate'.  Return 0 on success and a non-zero value otherwise.
        // The behavior is undefined unless 'destination' refers to an object
        // of enumeration type.  Note that a non-zero value is returned if
        // 'aggregate' does not refer to an object of type convertible to a
        // string.

    template <class TYPE>
    static int fromAggregateRawImp(TYPE                     *destination,
                                   const AggregateRaw&  aggregate,
                                   bdeat_TypeCategory::Simple);
        // Load into the specified 'destination' the value of the specified
        // 'aggregate'.  Return 0 on success and a non-zero value otherwise.
        // The behavior is undefined unless 'destination' refers to an object
        // of simple type.  Note that a non-zero value is returned if
        // 'aggregate' does not refer to an object of scalar type.

    template <class TYPE>
    static int fromAggregateRawToNullable(
                                         TYPE                     *destination,
                                         const AggregateRaw&  aggregate);
        // Load into the specified 'destination' the value of the specified
        // 'aggregate'.  Return 0 on success and a non-zero value otherwise.
        // The behavior is undefined unless 'destination' refers to an object
        // of nullable simple or nullable enumeration type.  Note that a
        // non-zero value is returned if 'aggregate' does not refer to an
        // object of scalar type or a type convertible to a string.

    template <class TYPE>
    static int fromAggregateToComplex(TYPE                  *destination,
                                      const Aggregate&  aggregate);
        // Load into the specified 'destination' the value of the specified
        // 'aggregate'.  Return 0 on success and a non-zero value otherwise.
        // The behavior is undefined unless 'destination' refers to an object
        // of sequence, choice, or customized type and the data type of
        // 'aggregate' is the 'bdem' type corresponding to 'TYPE'.

    template <class TYPE>
    static int fromAggregateImp(TYPE                  *destination,
                                const Aggregate&  aggregate,
                                bdeat_TypeCategory::Array);
    template <class TYPE>
    static int fromAggregateImp(TYPE                  *destination,
                                const Aggregate&  aggregate,
                                bdeat_TypeCategory::Choice);
    template <class TYPE>
    static int fromAggregateImp(TYPE                  *destination,
                                const Aggregate&  aggregate,
                                bdeat_TypeCategory::CustomizedType);
    template <class TYPE>
    static int fromAggregateImp(TYPE                  *destination,
                                const Aggregate&  aggregate,
                                bdeat_TypeCategory::Enumeration);
    template <class TYPE>
    static int fromAggregateImp(TYPE                  *destination,
                                const Aggregate&  aggregate,
                                bdeat_TypeCategory::Sequence);
    template <class TYPE>
    static int fromAggregateImp(TYPE                  *destination,
                                const Aggregate&  aggregate,
                                bdeat_TypeCategory::Simple);
        // Load into the specified 'destination' the value of the specified
        // 'aggregate'.  Return 0 on success and a non-zero value otherwise.
        // The behavior is undefined unless 'destination' refers to an object
        // of sequence, array, choice, customized, simple, or enumeration type.
        // Note that a non-zero value is returned if 'aggregate' does not
        // refer to an object of the 'bdem' type corresponding to 'TYPE'.

    template <class TYPE>
    static int fromAggregateToNullable(TYPE                  *destination,
                                       const Aggregate&  aggregate);
        // Load into the specified 'destination' the value of the specified
        // 'aggregate'.  Return 0 on success and a non-zero value otherwise.
        // The behavior is undefined unless 'TYPE' refers to a nullable type.
        // Note that a non-zero value is returned if 'aggregate' does not refer
        // to the 'bdem' type corresponding to 'TYPE::ValueType'.

    template <class TYPE, class CONVERTER>
    static int fromAggregateRawToVectorOfPrimitive(
                                         bsl::vector<TYPE>        *destination,
                                         const AggregateRaw&  aggregate,
                                         CONVERTER                 converter);
        // Load into the specified 'destination' the value of the specified
        // 'aggregate' using the specified 'converter' to convert individual
        // elements.  Return 0 on success and a non-zero value otherwise.  The
        // behavior is undefined unless 'TYPE' refers to a scalar, enumeration,
        // nullable scalar, or nullable enumeration type and 'aggregate' refers
        // to the 'bdem' array type corresponding to 'TYPE' or a
        // 'bdlmxxx::ElemType::BDEM_TABLE'.

    template <class TYPE, class CONVERTER>
    static int fromAggregateToVectorOfComplex(
                                            bsl::vector<TYPE>     *destination,
                                            const Aggregate&  aggregate,
                                            CONVERTER              converter);
        // Load into the specified 'destination' the value of the specified
        // 'aggregate' using the specified 'converter' to convert individual
        // elements.  Return 0 on success and a non-zero value otherwise.  The
        // behavior is undefined unless 'TYPE' refers to a sequence, choice,
        // customized, nullable sequence, nullable choice, or nullable
        // customized type and 'aggregate' refers to an object of the 'bdem'
        // array type corresponding to 'TYPE' or a 'bdlmxxx::ElemType::BDEM_TABLE'.

    template <class TYPE>
    static int fromAggregateToVectorOfNullable(
                                            bsl::vector<TYPE>     *destination,
                                            const Aggregate&  aggregate,
                                            int                    fieldId,
                                            bdeat_TypeCategory::Choice);
    template <class TYPE>
    static int fromAggregateToVectorOfNullable(
                                           bsl::vector<TYPE>     *destination,
                                           const Aggregate&  aggregate,
                                           int                    fieldId,
                                           bdeat_TypeCategory::CustomizedType);
    template <class TYPE>
    static int fromAggregateToVectorOfNullable(
                                            bsl::vector<TYPE>     *destination,
                                            const Aggregate&  aggregate,
                                            int                    fieldId,
                                            bdeat_TypeCategory::Enumeration);
    template <class TYPE>
    static int fromAggregateToVectorOfNullable(
                                            bsl::vector<TYPE>     *destination,
                                            const Aggregate&  aggregate,
                                            int                    fieldId,
                                            bdeat_TypeCategory::Sequence);
    template <class TYPE>
    static int fromAggregateToVectorOfNullable(
                                            bsl::vector<TYPE>     *destination,
                                            const Aggregate&  aggregate,
                                            int                    fieldId,
                                            bdeat_TypeCategory::Simple);
        // Load into the specified 'destination' the value of the sub-aggregate
        // referred to by the specified 'fieldId' in the specified 'aggregate'.
        // Return 0 on success and a non-zero value otherwise.  The behavior is
        // undefined unless 'destination' refers to an object of vector of a
        // nullable sequence, choice, simple, customized, or enumeration type.
        // Note that a non-zero value is returned if the sub-aggregate does not
        // refer to an object of the 'bdem' array type corresponding to
        // 'TYPE::ValueType' or a 'bdlmxxx::ElemType::BDEM_TABLE'.

    template <class TYPE>
    static int fromAggregateToVector(bsl::vector<TYPE>     *destination,
                                     const Aggregate&  aggregate,
                                     int                    fieldId,
                                     bdeat_TypeCategory::Choice);
    template <class TYPE>
    static int fromAggregateToVector(bsl::vector<TYPE>     *destination,
                                     const Aggregate&  aggregate,
                                     int                    fieldId,
                                     bdeat_TypeCategory::CustomizedType);
    template <class TYPE>
    static int fromAggregateToVector(bsl::vector<TYPE>     *destination,
                                     const Aggregate&  aggregate,
                                     int                    fieldId,
                                     bdeat_TypeCategory::Enumeration);
    template <class TYPE>
    static int fromAggregateToVector(bsl::vector<TYPE>     *destination,
                                     const Aggregate&  aggregate,
                                     int                    fieldId,
                                     bdeat_TypeCategory::NullableValue);
    template <class TYPE>
    static int fromAggregateToVector(bsl::vector<TYPE>     *destination,
                                     const Aggregate&  aggregate,
                                     int                    fieldId,
                                     bdeat_TypeCategory::Sequence);
    template <class TYPE>
    static int fromAggregateToVector(bsl::vector<TYPE>     *destination,
                                     const Aggregate&  aggregate,
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
        // corresponding to 'TYPE' or a 'bdlmxxx::ElemType::BDEM_TABLE'.

    template <class TYPE>
    static int fromAggregateImp(bsl::vector<TYPE>     *destination,
                                const Aggregate&  aggregate,
                                int                    fieldId,
                                bdeat_TypeCategory::Array);
    template <class TYPE>
    static int fromAggregateImp(TYPE                  *destination,
                                const Aggregate&  aggregate,
                                int                    fieldId,
                                bdeat_TypeCategory::Choice);
    template <class TYPE>
    static int fromAggregateImp(TYPE                  *destination,
                                const Aggregate&  aggregate,
                                int                    fieldId,
                                bdeat_TypeCategory::CustomizedType);
    template <class TYPE>
    static int fromAggregateImp(TYPE                  *destination,
                                const Aggregate&  aggregate,
                                int                    fieldId,
                                bdeat_TypeCategory::Enumeration);
    template <class TYPE>
    static int fromAggregateImp(TYPE                  *destination,
                                const Aggregate&  aggregate,
                                int                    fieldId,
                                bdeat_TypeCategory::NullableValue);
    template <class TYPE>
    static int fromAggregateImp(TYPE                  *destination,
                                const Aggregate&  aggregate,
                                int                    fieldId,
                                bdeat_TypeCategory::Sequence);
    template <class TYPE>
    static int fromAggregateImp(TYPE                  *destination,
                                const Aggregate&  aggregate,
                                int                    fieldId,
                                bdeat_TypeCategory::Simple);
        // Load into the specified 'destination' the value of the sub-aggregate
        // referred to by the specified 'fieldId' in the specified 'aggregate'.
        // Return 0 on success and a non-zero value otherwise.  The behavior is
        // undefined unless 'destination' refers to an object of sequence,
        // choice, array, nullable, scalar, customized, or enumeration type.
        // Note that a non-zero value is returned if the sub-aggregate does not
        // refer to an object of the 'bdem' type corresponding to 'TYPE'.

    template <class TYPE>
    static int toAggregateRawFromEnumeration(AggregateRaw *aggregate,
                                             const TYPE&        source);
        // Load into the specified 'aggregate' the value of the specified
        // 'source'.  Return 0 on success and a non-zero value otherwise.  The
        // behavior is undefined unless 'source' refers to an object of
        // enumeration type.  Note that a non-zero value is returned if
        // 'aggregate' does not refer to a type convertible to a string.

    template <class TYPE>
    static int toAggregateRawFromSimple(AggregateRaw *aggregate,
                                        const TYPE&        source);
        // Load into the specified 'aggregate' the value of the specified
        // 'source'.  Return 0 on success and a non-zero value otherwise.  The
        // behavior is undefined unless 'source' refers to an object of simple
        // type and 'aggregate' refers to an object of scalar type.

    template <class TYPE>
    static int toAggregateRawImp(AggregateRaw *aggregate,
                                 const TYPE&        source,
                                 bdeat_TypeCategory::Enumeration);
        // Load into the specified 'aggregate' the value of the specified
        // 'source'.  Return 0 on success and a non-zero value otherwise.  The
        // behavior is undefined unless 'source' refers to an object of
        // enumeration type.  Note that a non-zero value is returned if
        // 'aggregate' does not refer to a type convertible to a string.

    template <class TYPE>
    static int toAggregateRawImp(AggregateRaw *aggregate,
                                 const TYPE&        source,
                                 bdeat_TypeCategory::Simple);
        // Load into the specified 'aggregate' the value of the specified
        // 'source'.  Return 0 on success and a non-zero value otherwise.  The
        // behavior is undefined unless 'source' refers to an object of simple
        // type.  Note that a non-zero value is returned if 'aggregate' does
        // not refer to an object of scalar type.

    template <class TYPE>
    static int toAggregateRawFromNullable(AggregateRaw *aggregate,
                                          const TYPE&        source);
        // Load into the specified 'aggregate' the value of the specified
        // 'source'.  Return 0 on success and a non-zero value otherwise.  The
        // behavior is undefined unless 'source' refers to an object of
        // nullable simple or nullable enumeration type.  Note that a non-zero
        // value is returned if 'aggregate' does not refer to an object of
        // scalar type or a type convertible to a string.

    template <class TYPE>
    static int toAggregateFromComplex(Aggregate *aggregate,
                                      const TYPE&     source);
        // Load into the specified 'aggregate' the value of the specified
        // 'source'.  Return 0 on success and a non-zero value otherwise.  The
        // behavior is undefined unless 'source' refers to an object of
        // sequence, choice, or customized type and the data type of
        // 'aggregate' is the 'bdem' type corresponding to 'TYPE'.

    template <class TYPE>
    static int toAggregateImp(Aggregate *aggregate,
                              const TYPE&     source,
                              bdeat_TypeCategory::Array);
    template <class TYPE>
    static int toAggregateImp(Aggregate *aggregate,
                              const TYPE&     source,
                              bdeat_TypeCategory::CustomizedType);
    template <class TYPE>
    static int toAggregateImp(Aggregate *aggregate,
                              const TYPE&     source,
                              bdeat_TypeCategory::Choice);
    template <class TYPE>
    static int toAggregateImp(Aggregate *aggregate,
                              const TYPE&     source,
                              bdeat_TypeCategory::Enumeration);
    template <class TYPE>
    static int toAggregateImp(Aggregate *aggregate,
                              const TYPE&     source,
                              bdeat_TypeCategory::Sequence);
    template <class TYPE>
    static int toAggregateImp(Aggregate *aggregate,
                              const TYPE&     source,
                              bdeat_TypeCategory::Simple);
        // Load into the specified 'aggregate' the value of the specified
        // 'source'.  Return 0 on success and a non-zero value otherwise.  The
        // behavior is undefined unless 'source' refers to an object of
        // sequence, array, choice, customized, simple, or enumeration type.
        // Note that a non-zero value is returned if 'aggregate' does not
        // refer to an object of the 'bdem' type corresponding to 'TYPE'.

    template <class TYPE>
    static int toAggregateFromNullable(Aggregate *aggregate,
                                       const TYPE&     source);
        // Load into the specified 'aggregate' the value of the specified
        // 'source'.  Return 0 on success and a non-zero value otherwise.  The
        // behavior is undefined unless 'TYPE' refers to a nullable type.  Note
        // that a non-zero value is returned if 'aggregate' does not refer to
        // an object of the 'bdem' type corresponding to 'TYPE::ValueType'.

    template <class TYPE, class CONVERTER>
    static int toAggregateRawFromVectorOfPrimitive(
                                          AggregateRaw        *aggregate,
                                          const bsl::vector<TYPE>&  source,
                                          CONVERTER                 converter);
        // Load into the specified 'aggregate' the value of the specified
        // 'source' using the specified 'converter' to convert individual
        // elements.  Return 0 on success and a non-zero value otherwise.  The
        // behavior is undefined unless 'TYPE' refers to a scalar, enumeration,
        // nullable scalar, or nullable enumeration type and 'aggregate' refers
        // to the 'bdem' array type corresponding to 'TYPE' or a
        // 'bdlmxxx::ElemType::BDEM_TABLE'.

    template <class TYPE, class CONVERTER>
    static int toAggregateFromVectorOfComplex(
                                          Aggregate           *aggregate,
                                          const bsl::vector<TYPE>&  source,
                                          CONVERTER                 converter);
        // Load into the specified 'aggregate' the value of the specified
        // 'source' using the specified 'converter' to convert individual
        // elements.  Return 0 on success and a non-zero value otherwise.  The
        // behavior is undefined unless 'TYPE' refers to a sequence, choice,
        // customized, nullable sequence, nullable choice, or nullable
        // customized type and 'aggregate' refers to an object of the 'bdem'
        // array type corresponding to 'TYPE' or a 'bdlmxxx::ElemType::BDEM_TABLE'.

    template <class TYPE>
    static int toAggregateFromVectorOfNullable(
                                           Aggregate           *aggregate,
                                           int                       fieldId,
                                           const bsl::vector<TYPE>&  source,
                                           bdeat_TypeCategory::Choice);
    template <class TYPE>
    static int toAggregateFromVectorOfNullable(
                                           Aggregate           *aggregate,
                                           int                       fieldId,
                                           const bsl::vector<TYPE>&  source,
                                           bdeat_TypeCategory::CustomizedType);
    template <class TYPE>
    static int toAggregateFromVectorOfNullable(
                                           Aggregate           *aggregate,
                                           int                       fieldId,
                                           const bsl::vector<TYPE>&  source,
                                           bdeat_TypeCategory::Enumeration);
    template <class TYPE>
    static int toAggregateFromVectorOfNullable(
                                           Aggregate           *aggregate,
                                           int                       fieldId,
                                           const bsl::vector<TYPE>&  source,
                                           bdeat_TypeCategory::Sequence);
    template <class TYPE>
    static int toAggregateFromVectorOfNullable(
                                           Aggregate           *aggregate,
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
        // 'TYPE::ValueType' or a 'bdlmxxx::ElemType::BDEM_TABLE'.

    template <class TYPE>
    static int toAggregateFromVector(Aggregate           *aggregate,
                                     int                       fieldId,
                                     const bsl::vector<TYPE>&  source,
                                     bdeat_TypeCategory::Choice);
    template <class TYPE>
    static int toAggregateFromVector(Aggregate           *aggregate,
                                     int                       fieldId,
                                     const bsl::vector<TYPE>&  source,
                                     bdeat_TypeCategory::CustomizedType);
    template <class TYPE>
    static int toAggregateFromVector(Aggregate           *aggregate,
                                     int                       fieldId,
                                     const bsl::vector<TYPE>&  source,
                                     bdeat_TypeCategory::Enumeration);
    template <class TYPE>
    static int toAggregateFromVector(Aggregate           *aggregate,
                                     int                       fieldId,
                                     const bsl::vector<TYPE>&  source,
                                     bdeat_TypeCategory::NullableValue);
    template <class TYPE>
    static int toAggregateFromVector(Aggregate           *aggregate,
                                     int                       fieldId,
                                     const bsl::vector<TYPE>&  source,
                                     bdeat_TypeCategory::Sequence);
    template <class TYPE>
    static int toAggregateFromVector(Aggregate           *aggregate,
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
        // or a 'bdlmxxx::ElemType::BDEM_TABLE'.

    template <class TYPE>
    static int toAggregateImp(Aggregate           *aggregate,
                              int                       fieldId,
                              const bsl::vector<TYPE>&  source,
                              bdeat_TypeCategory::Array);
    template <class TYPE>
    static int toAggregateImp(Aggregate *aggregate,
                              int             fieldId,
                              const TYPE&     source,
                              bdeat_TypeCategory::Choice);
    template <class TYPE>
    static int toAggregateImp(Aggregate *aggregate,
                              int             fieldId,
                              const TYPE&     source,
                              bdeat_TypeCategory::CustomizedType);
    template <class TYPE>
    static int toAggregateImp(Aggregate *aggregate,
                              int             fieldId,
                              const TYPE&     source,
                              bdeat_TypeCategory::Enumeration);
    template <class TYPE>
    static int toAggregateImp(Aggregate *aggregate,
                              int             fieldId,
                              const TYPE&     source,
                              bdeat_TypeCategory::NullableValue);
    template <class TYPE>
    static int toAggregateImp(Aggregate *aggregate,
                              int             fieldId,
                              const TYPE&     source,
                              bdeat_TypeCategory::Sequence);
    template <class TYPE>
    static int toAggregateImp(Aggregate *aggregate,
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

    template <class TYPE>
    static int fromAggregate(TYPE                  *destination,
                             const Aggregate&  aggregate,
                             int                    fieldId);
        // Load into the specified 'destination' the value of the sub-aggregate
        // referred to by the specified 'fieldId' in the specified 'aggregate'.
        // Return 0 on success and a non-zero value otherwise.  The behavior is
        // undefined unless 'destination' refers to an object of sequence,
        // choice, array, nullable, scalar, customized, or enumeration type.
        // Note that a non-zero value is returned if the sub-aggregate does not
        // refer to an object of the 'bdem' type corresponding to 'TYPE'.

    template <class TYPE>
    static int toAggregate(Aggregate *aggregate,
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
                          // class AggregateUtil_MakeSigned
                          // ===================================

template <class TYPE>
struct AggregateUtil_MakeSigned {
    // This 'struct' defines a signed type, 'Type', corresponding to 'TYPE'.
    //
    // TBD Replace this custom type with bsl::make_signed<T> when it is
    // available.

    typedef TYPE Type;
};

template <>
struct AggregateUtil_MakeSigned<unsigned char> {
    typedef char Type;
};

template <>
struct AggregateUtil_MakeSigned<unsigned short> {
    typedef short Type;
};

template <>
struct AggregateUtil_MakeSigned<unsigned int> {
    typedef int Type;
};

template <>
struct AggregateUtil_MakeSigned<bsls::Types::Uint64> {
    typedef bsls::Types::Int64 Type;
};

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

                        // ------------------------
                        // class AggregateUtil
                        // ------------------------

// PRIVATE CLASS METHODS
template <class TYPE>
bsl::vector<TYPE>&
AggregateUtil::theModifiableArray(const bdlmxxx::ElemRef& /*elemRef*/)
{
    BSLS_ASSERT_OPT(!"Unreachable");
    return *(bsl::vector<TYPE>*)0;
}

template <>
inline
bsl::vector<bool>&
AggregateUtil::theModifiableArray<bool>(const bdlmxxx::ElemRef& elemRef)
{
    BSLS_ASSERT_SAFE(bdlmxxx::ElemType::BDEM_BOOL_ARRAY == elemRef.type());

    return elemRef.theModifiableBoolArray();
}

template <>
inline
bsl::vector<char>&
AggregateUtil::theModifiableArray<char>(const bdlmxxx::ElemRef& elemRef)
{
    BSLS_ASSERT_SAFE(bdlmxxx::ElemType::BDEM_CHAR_ARRAY == elemRef.type());

    return elemRef.theModifiableCharArray();
}

template <>
inline
bsl::vector<short>&
AggregateUtil::theModifiableArray<short>(const bdlmxxx::ElemRef& elemRef)
{
    BSLS_ASSERT_SAFE(bdlmxxx::ElemType::BDEM_SHORT_ARRAY == elemRef.type());

    return elemRef.theModifiableShortArray();
}

template <>
inline
bsl::vector<int>&
AggregateUtil::theModifiableArray<int>(const bdlmxxx::ElemRef& elemRef)
{
    BSLS_ASSERT_SAFE(bdlmxxx::ElemType::BDEM_INT_ARRAY == elemRef.type());

    return elemRef.theModifiableIntArray();
}

template <>
inline
bsl::vector<bsls::Types::Int64>&
AggregateUtil::theModifiableArray<bsls::Types::Int64>(
                                                   const bdlmxxx::ElemRef& elemRef)
{
    BSLS_ASSERT_SAFE(bdlmxxx::ElemType::BDEM_INT64_ARRAY == elemRef.type());

    return elemRef.theModifiableInt64Array();
}

template <>
inline
bsl::vector<float>&
AggregateUtil::theModifiableArray<float>(const bdlmxxx::ElemRef& elemRef)
{
    BSLS_ASSERT_SAFE(bdlmxxx::ElemType::BDEM_FLOAT_ARRAY == elemRef.type());

    return elemRef.theModifiableFloatArray();
}

template <>
inline
bsl::vector<double>&
AggregateUtil::theModifiableArray<double>(const bdlmxxx::ElemRef& elemRef)
{
    BSLS_ASSERT_SAFE(bdlmxxx::ElemType::BDEM_DOUBLE_ARRAY == elemRef.type());

    return elemRef.theModifiableDoubleArray();
}

template <>
inline
bsl::vector<bsl::string>&
AggregateUtil::theModifiableArray<bsl::string>(
                                                   const bdlmxxx::ElemRef& elemRef)
{
    BSLS_ASSERT_SAFE(bdlmxxx::ElemType::BDEM_STRING_ARRAY == elemRef.type());

    return elemRef.theModifiableStringArray();
}

template <>
inline
bsl::vector<bdlt::DateTz>&
AggregateUtil::theModifiableArray<bdlt::DateTz>(
                                                   const bdlmxxx::ElemRef& elemRef)
{
    BSLS_ASSERT_SAFE(bdlmxxx::ElemType::BDEM_DATETZ_ARRAY == elemRef.type());

    return elemRef.theModifiableDateTzArray();
}

template <>
inline
bsl::vector<bdlt::DatetimeTz>&
AggregateUtil::theModifiableArray<bdlt::DatetimeTz>(
                                                   const bdlmxxx::ElemRef& elemRef)
{
    BSLS_ASSERT_SAFE(bdlmxxx::ElemType::BDEM_DATETIMETZ_ARRAY == elemRef.type());

    return elemRef.theModifiableDatetimeTzArray();
}

template <>
inline
bsl::vector<bdlt::TimeTz>&
AggregateUtil::theModifiableArray<bdlt::TimeTz>(
                                                   const bdlmxxx::ElemRef& elemRef)
{
    BSLS_ASSERT_SAFE(bdlmxxx::ElemType::BDEM_TIMETZ_ARRAY == elemRef.type());

    return elemRef.theModifiableTimeTzArray();
}

template <>
inline
bsl::vector<bdlt::Date>&
AggregateUtil::theModifiableArray<bdlt::Date>(const bdlmxxx::ElemRef& elemRef)
{
    BSLS_ASSERT_SAFE(bdlmxxx::ElemType::BDEM_DATE_ARRAY == elemRef.type());

    return elemRef.theModifiableDateArray();
}

template <>
inline
bsl::vector<bdlt::Datetime>&
AggregateUtil::theModifiableArray<bdlt::Datetime>(
                                                   const bdlmxxx::ElemRef& elemRef)
{
    BSLS_ASSERT_SAFE(bdlmxxx::ElemType::BDEM_DATETIME_ARRAY == elemRef.type());

    return elemRef.theModifiableDatetimeArray();
}

template <>
inline
bsl::vector<bdlt::Time>&
AggregateUtil::theModifiableArray<bdlt::Time>(const bdlmxxx::ElemRef& elemRef)
{
    BSLS_ASSERT_SAFE(bdlmxxx::ElemType::BDEM_TIME_ARRAY == elemRef.type());

    return elemRef.theModifiableTimeArray();
}

template <class TYPE>
inline
int AggregateUtil::fromAggregateRawToEnumeration(
                                         TYPE                     *destination,
                                         const AggregateRaw&  aggregate)
{
    typedef typename bdeat_BasicEnumerationWrapper<TYPE>::Wrapper Wrapper;

    return Wrapper::fromString(destination, aggregate.asString());
}

template <class TYPE>
int AggregateUtil::fromAggregateRawToSimple(
                                         TYPE                     *destination,
                                         const AggregateRaw&  aggregate)
{
    typedef typename AggregateUtil_MakeSigned<TYPE>::Type SIGNED_TYPE;

    if (aggregate.isNull()) {
        *destination = static_cast<TYPE>(
                                       bdltuxxx::Unset<SIGNED_TYPE>::unsetValue());
    }
    else {
        *destination = static_cast<TYPE>(
                                       aggregate.convertScalar<SIGNED_TYPE>());
    }
    return 0;
}

template <class TYPE>
inline
int AggregateUtil::fromAggregateRawImp(
                                         TYPE                     *destination,
                                         const AggregateRaw&  aggregate,
                                         bdeat_TypeCategory::Enumeration)
{
    return fromAggregateRawToEnumeration(destination, aggregate);
}

template <class TYPE>
int AggregateUtil::fromAggregateRawImp(
                                         TYPE                     *destination,
                                         const AggregateRaw&  aggregate,
                                         bdeat_TypeCategory::Simple)
{
    if (!bdlmxxx::ElemType::isScalarType(aggregate.dataType())) {
        return -1;                                                    // RETURN
    }

    return fromAggregateRawToSimple(destination, aggregate);
}

template <class TYPE>
int AggregateUtil::fromAggregateRawToNullable(
                                         TYPE                     *destination,
                                         const AggregateRaw&  aggregate)
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

template <class TYPE>
inline
int AggregateUtil::fromAggregateToComplex(
                                            TYPE                  *destination,
                                            const Aggregate&  aggregate)
{
    return destination->fromAggregate(aggregate);
}

template <class TYPE>
int AggregateUtil::fromAggregateImp(TYPE                  *destination,
                                         const Aggregate&  aggregate,
                                         bdeat_TypeCategory::Array)
{
    // This case is only for handling 'bsl::vector<char>'.

    if (bdlmxxx::ElemType::BDEM_CHAR_ARRAY == aggregate.dataType()) {
        *destination = aggregate.asElemRef().theCharArray();
        return 0;                                                     // RETURN
    }
    return -1;
}

template <class TYPE>
int AggregateUtil::fromAggregateImp(TYPE                  *destination,
                                         const Aggregate&  aggregate,
                                         bdeat_TypeCategory::Choice)
{
    if (bdlmxxx::ElemType::BDEM_CHOICE            == aggregate.dataType()
     || bdlmxxx::ElemType::BDEM_CHOICE_ARRAY_ITEM == aggregate.dataType()) {
        return fromAggregateToComplex(destination, aggregate);        // RETURN
    }
    return -1;
}

template <class TYPE>
int AggregateUtil::fromAggregateImp(TYPE                  *destination,
                                         const Aggregate&  aggregate,
                                         bdeat_TypeCategory::CustomizedType)
{
    bdlmxxx::ElemType::Type expectedType = static_cast<bdlmxxx::ElemType::Type>(
                          bdlmxxx::SelectBdemType<typename TYPE::BaseType>::VALUE);

    if (expectedType == aggregate.dataType()) {
        return fromAggregateToComplex(destination, aggregate);        // RETURN
    }
    return -1;
}

template <class TYPE>
inline
int AggregateUtil::fromAggregateImp(TYPE                  *destination,
                                         const Aggregate&  aggregate,
                                         bdeat_TypeCategory::Enumeration)
{
    return fromAggregateRawToEnumeration(destination,
                                         aggregate.aggregateRaw());
}

template <class TYPE>
int AggregateUtil::fromAggregateImp(TYPE                  *destination,
                                         const Aggregate&  aggregate,
                                         bdeat_TypeCategory::Sequence)
{
    if (bdlmxxx::ElemType::BDEM_LIST == aggregate.dataType()
     || bdlmxxx::ElemType::BDEM_ROW  == aggregate.dataType()) {
        return fromAggregateToComplex(destination, aggregate);        // RETURN
    }
    return -1;
}

template <class TYPE>
int AggregateUtil::fromAggregateImp(TYPE                  *destination,
                                         const Aggregate&  aggregate,
                                         bdeat_TypeCategory::Simple)
{
    if (!bdlmxxx::ElemType::isScalarType(aggregate.dataType())) {
        return -1;                                                    // RETURN
    }

    return fromAggregateRawToSimple(destination, aggregate.aggregateRaw());
}

template <class TYPE>
int AggregateUtil::fromAggregateToNullable(
                                            TYPE                  *destination,
                                            const Aggregate&  aggregate)
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

template <class TYPE, class CONVERTER>
int AggregateUtil::fromAggregateRawToVectorOfPrimitive(
                                         bsl::vector<TYPE>        *destination,
                                         const AggregateRaw&  aggregate,
                                         CONVERTER                 converter)
{
    const int length = aggregate.length();
    destination->resize(length);

    AggregateRaw    item;
    ErrorAttributes error;
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

template <class TYPE, class CONVERTER>
int AggregateUtil::fromAggregateToVectorOfComplex(
                                            bsl::vector<TYPE>     *destination,
                                            const Aggregate&  aggregate,
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

template <class TYPE>
int AggregateUtil::fromAggregateToVectorOfNullable(
                                            bsl::vector<TYPE>     *destination,
                                            const Aggregate&  aggregate,
                                            int                    fieldId,
                                            bdeat_TypeCategory::Choice)
{
    const Aggregate& field = aggregate.fieldById(fieldId);
    if (field.isError()) {
        return field.errorCode();                                     // RETURN
    }

    if (bdlmxxx::ElemType::BDEM_CHOICE_ARRAY == field.dataType()) {
        return fromAggregateToVectorOfComplex(destination,
                                              field,
                                              fromAggregateToNullable<TYPE>);
                                                                      // RETURN
    }
    return -1;
}

template <class TYPE>
int AggregateUtil::fromAggregateToVectorOfNullable(
                                            bsl::vector<TYPE>     *destination,
                                            const Aggregate&  aggregate,
                                            int                    fieldId,
                                            bdeat_TypeCategory::CustomizedType)
{
    const Aggregate& field = aggregate.fieldById(fieldId);
    if (field.isError()) {
        return field.errorCode();                                     // RETURN
    }

    bdlmxxx::ElemType::Type expectedType = bdlmxxx::ElemType::toArrayType(
              static_cast<bdlmxxx::ElemType::Type>(bdlmxxx::SelectBdemType<
                                  typename TYPE::ValueType::BaseType>::VALUE));

    if (bdlmxxx::ElemType::BDEM_TABLE == field.dataType()
     || expectedType              == field.dataType()) {
        return fromAggregateToVectorOfComplex(destination,
                                              field,
                                              fromAggregateToNullable<TYPE>);
                                                                      // RETURN
    }
    return -1;
}

template <class TYPE>
int AggregateUtil::fromAggregateToVectorOfNullable(
                                            bsl::vector<TYPE>     *destination,
                                            const Aggregate&  aggregate,
                                            int                    fieldId,
                                            bdeat_TypeCategory::Enumeration)
{
    AggregateRaw    aggregateRaw = aggregate.aggregateRaw();
    AggregateRaw    field;
    ErrorAttributes error;
    if (0 != aggregateRaw.fieldById(&field, &error, fieldId)) {
        return error.code();                                          // RETURN
    }

    if (bdlmxxx::ElemType::BDEM_TABLE        == field.dataType()
     || bdlmxxx::ElemType::BDEM_INT_ARRAY    == field.dataType()
     || bdlmxxx::ElemType::BDEM_STRING_ARRAY == field.dataType()) {
        return fromAggregateRawToVectorOfPrimitive(
                                             destination,
                                             field,
                                             fromAggregateRawToNullable<TYPE>);
                                                                      // RETURN
    }
    return -1;
}

template <class TYPE>
int AggregateUtil::fromAggregateToVectorOfNullable(
                                            bsl::vector<TYPE>     *destination,
                                            const Aggregate&  aggregate,
                                            int                    fieldId,
                                            bdeat_TypeCategory::Sequence)
{
    const Aggregate& field = aggregate.fieldById(fieldId);
    if (field.isError()) {
        return field.errorCode();                                     // RETURN
    }

    if (bdlmxxx::ElemType::BDEM_TABLE == field.dataType()) {
        return fromAggregateToVectorOfComplex(destination,
                                              field,
                                              fromAggregateToNullable<TYPE>);
                                                                      // RETURN
    }
    return -1;
}

template <class TYPE>
int AggregateUtil::fromAggregateToVectorOfNullable(
                                            bsl::vector<TYPE>     *destination,
                                            const Aggregate&  aggregate,
                                            int                    fieldId,
                                            bdeat_TypeCategory::Simple)
{
    AggregateRaw    aggregateRaw = aggregate.aggregateRaw();
    AggregateRaw    field;
    ErrorAttributes error;
    if (0 != aggregateRaw.fieldById(&field, &error, fieldId)) {
        return error.code();                                          // RETURN
    }

    typedef typename
    AggregateUtil_MakeSigned<typename TYPE::ValueType>::Type SIGNED_TYPE;

    bdlmxxx::ElemType::Type expType = bdlmxxx::ElemType::toArrayType(
    static_cast<bdlmxxx::ElemType::Type>(bdlmxxx::SelectBdemType<SIGNED_TYPE>::VALUE));

    if (bdlmxxx::ElemType::BDEM_TABLE       == field.dataType()
     || expType                         == field.dataType()
     || bdlmxxx::ElemType::BDEM_SHORT_ARRAY == field.dataType()
     || bdlmxxx::ElemType::BDEM_INT_ARRAY   == field.dataType()
     || bdlmxxx::ElemType::BDEM_INT64_ARRAY == field.dataType()) {
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

template <class TYPE>
int AggregateUtil::fromAggregateToVector(
                                            bsl::vector<TYPE>     *destination,
                                            const Aggregate&  aggregate,
                                            int                    fieldId,
                                            bdeat_TypeCategory::Choice)
{
    Aggregate field  = aggregate.fieldById(fieldId);
    if (field.isError()) {
        return field.errorCode();                                     // RETURN
    }

    if (bdlmxxx::ElemType::BDEM_CHOICE_ARRAY == field.dataType()) {
        return fromAggregateToVectorOfComplex(destination,
                                              field,
                                              fromAggregateToComplex<TYPE>);
                                                                      // RETURN
    }
    return -1;
}

template <class TYPE>
int AggregateUtil::fromAggregateToVector(
                                            bsl::vector<TYPE>     *destination,
                                            const Aggregate&  aggregate,
                                            int                    fieldId,
                                            bdeat_TypeCategory::CustomizedType)
{
    Aggregate field  = aggregate.fieldById(fieldId);
    if (field.isError()) {
        return field.errorCode();                                     // RETURN
    }

    bdlmxxx::ElemType::Type expectedType = bdlmxxx::ElemType::toArrayType(
                                              static_cast<bdlmxxx::ElemType::Type>(
                         bdlmxxx::SelectBdemType<typename TYPE::BaseType>::VALUE));

    if (expectedType == field.dataType()) {
        return fromAggregateToVectorOfComplex(destination,
                                              field,
                                              fromAggregateToComplex<TYPE>);
                                                                      // RETURN
    }
    return -1;
}

template <class TYPE>
int AggregateUtil::fromAggregateToVector(
                                            bsl::vector<TYPE>     *destination,
                                            const Aggregate&  aggregate,
                                            int                    fieldId,
                                            bdeat_TypeCategory::Enumeration)
{
    AggregateRaw    aggregateRaw = aggregate.aggregateRaw();
    AggregateRaw    field;
    ErrorAttributes error;
    if (0 != aggregateRaw.fieldById(&field, &error, fieldId)) {
        return error.code();                                          // RETURN
    }

    if (bdlmxxx::ElemType::BDEM_INT_ARRAY    == field.dataType()
     || bdlmxxx::ElemType::BDEM_STRING_ARRAY == field.dataType()) {
        return fromAggregateRawToVectorOfPrimitive(
                                          destination,
                                          field,
                                          fromAggregateRawToEnumeration<TYPE>);
                                                                      // RETURN
    }
    return -1;
}

template <class TYPE>
inline
int AggregateUtil::fromAggregateToVector(
                                            bsl::vector<TYPE>     *destination,
                                            const Aggregate&  aggregate,
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

template <class TYPE>
int AggregateUtil::fromAggregateToVector(
                                            bsl::vector<TYPE>     *destination,
                                            const Aggregate&  aggregate,
                                            int                    fieldId,
                                            bdeat_TypeCategory::Sequence)
{
    Aggregate field  = aggregate.fieldById(fieldId);
    if (field.isError()) {
        return field.errorCode();                                     // RETURN
    }

    if (bdlmxxx::ElemType::BDEM_TABLE == field.dataType()) {
        return fromAggregateToVectorOfComplex(destination,
                                              field,
                                              fromAggregateToComplex<TYPE>);
                                                                      // RETURN
    }
    return -1;
}

template <class TYPE>
int AggregateUtil::fromAggregateToVector(
                                            bsl::vector<TYPE>     *destination,
                                            const Aggregate&  aggregate,
                                            int                    fieldId,
                                            bdeat_TypeCategory::Simple)
{
    AggregateRaw    aggregateRaw = aggregate.aggregateRaw();
    AggregateRaw    field;
    ErrorAttributes error;
    if (0 != aggregateRaw.fieldById(&field, &error, fieldId)) {
        return error.code();                                          // RETURN
    }

    typedef typename AggregateUtil_MakeSigned<TYPE>::Type SIGNED_TYPE;

    bdlmxxx::ElemType::Type expType = bdlmxxx::ElemType::toArrayType(
    static_cast<bdlmxxx::ElemType::Type>(bdlmxxx::SelectBdemType<SIGNED_TYPE>::VALUE));

    if (expType == field.dataType()) {
        const bsl::vector<SIGNED_TYPE>& array =
                            theModifiableArray<SIGNED_TYPE>(field.asElemRef());

        *destination = reinterpret_cast<const bsl::vector<TYPE>&>(array);
        return 0;                                                     // RETURN
    }
    else if (bdlmxxx::ElemType::BDEM_SHORT_ARRAY == field.dataType()
          || bdlmxxx::ElemType::BDEM_INT_ARRAY   == field.dataType()
          || bdlmxxx::ElemType::BDEM_INT64_ARRAY == field.dataType()) {
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

template <class TYPE>
inline
int AggregateUtil::fromAggregateImp(bsl::vector<TYPE>     *destination,
                                         const Aggregate&  aggregate,
                                         int                    fieldId,
                                         bdeat_TypeCategory::Array)
{
    typedef typename bdeat_TypeCategory::Select<TYPE>::Type TypeTag;
    return fromAggregateToVector(destination, aggregate, fieldId, TypeTag());
}

template <class TYPE>
int AggregateUtil::fromAggregateImp(TYPE                  *destination,
                                         const Aggregate&  aggregate,
                                         int                    fieldId,
                                         bdeat_TypeCategory::Choice)
{
    Aggregate field = aggregate.fieldById(fieldId);
    if (field.isError()) {
        return field.errorCode();                                     // RETURN
    }

    if (bdlmxxx::ElemType::BDEM_CHOICE == field.dataType()) {
        return fromAggregateToComplex(destination, field);            // RETURN
    }
    return -1;
}

template <class TYPE>
int AggregateUtil::fromAggregateImp(TYPE                  *destination,
                                         const Aggregate&  aggregate,
                                         int                    fieldId,
                                         bdeat_TypeCategory::CustomizedType)
{
    Aggregate field = aggregate.fieldById(fieldId);
    if (field.isError()) {
        return field.errorCode();                                     // RETURN
    }

    bdlmxxx::ElemType::Type expectedType = static_cast<bdlmxxx::ElemType::Type>(
                          bdlmxxx::SelectBdemType<typename TYPE::BaseType>::VALUE);

    if (expectedType == field.dataType()) {
        return fromAggregateToComplex(destination, field);            // RETURN
    }
    return -1;
}

template <class TYPE>
int AggregateUtil::fromAggregateImp(TYPE                  *destination,
                                         const Aggregate&  aggregate,
                                         int                    fieldId,
                                         bdeat_TypeCategory::Enumeration)
{
    AggregateRaw    aggregateRaw = aggregate.aggregateRaw();
    AggregateRaw    field;
    ErrorAttributes error;
    if (0 != aggregateRaw.fieldById(&field, &error, fieldId)) {
        return error.code();                                          // RETURN
    }

    return fromAggregateRawToEnumeration(destination, field);
}

template <class TYPE>
int AggregateUtil::fromAggregateImp(TYPE                  *destination,
                                         const Aggregate&  aggregate,
                                         int                    fieldId,
                                         bdeat_TypeCategory::NullableValue)
{
    const Aggregate& field = aggregate.fieldById(fieldId);
    if (field.isError()) {
        return field.errorCode();                                     // RETURN
    }

    return fromAggregateToNullable(destination, field);
}

template <class TYPE>
int AggregateUtil::fromAggregateImp(TYPE                  *destination,
                                         const Aggregate&  aggregate,
                                         int                    fieldId,
                                         bdeat_TypeCategory::Sequence)
{
    Aggregate field = aggregate.fieldById(fieldId);
    if (field.isError()) {
        return field.errorCode();                                     // RETURN
    }

    if (bdlmxxx::ElemType::BDEM_LIST == field.dataType()) {
        return fromAggregateToComplex(destination, field);            // RETURN
    }
    return -1;
}

template <class TYPE>
int AggregateUtil::fromAggregateImp(TYPE                  *destination,
                                         const Aggregate&  aggregate,
                                         int                    fieldId,
                                         bdeat_TypeCategory::Simple)
{
    AggregateRaw    aggregateRaw = aggregate.aggregateRaw();
    AggregateRaw    field;
    ErrorAttributes error;
    if (0 != aggregateRaw.fieldById(&field, &error, fieldId)) {
        return error.code();                                          // RETURN
    }

    if (!bdlmxxx::ElemType::isScalarType(field.dataType())) {
        return -1;                                                    // RETURN
    }

    return fromAggregateRawToSimple(destination, field);
}

template <class TYPE>
inline
int AggregateUtil::toAggregateRawFromEnumeration(
                                                AggregateRaw *aggregate,
                                                const TYPE&        source)
{
    typedef typename bdeat_BasicEnumerationWrapper<TYPE>::Wrapper Wrapper;

    ErrorAttributes errorDescription;
    aggregate->setValue(&errorDescription, Wrapper::toString(source));
    return errorDescription.code();
}

template <class TYPE>
inline
int AggregateUtil::toAggregateRawFromSimple(AggregateRaw *aggregate,
                                                 const TYPE&        source)
{
    // 'setValue' works for objects of both signed and unsigned types as
    // 'bdlmxxx::Convert' converts from both signed and unsigned primitive
    // types.

    ErrorAttributes error;
    aggregate->setValue(&error, source);
    return error.code();
}

template <class TYPE>
inline
int AggregateUtil::toAggregateFromComplex(Aggregate *aggregate,
                                               const TYPE&     source)
{
    return source.toAggregate(aggregate);
}

template <class TYPE>
inline
int AggregateUtil::toAggregateRawImp(AggregateRaw *aggregate,
                                          const TYPE&        source,
                                          bdeat_TypeCategory::Enumeration)
{
    return toAggregateRawFromEnumeration(aggregate, source);
}

template <class TYPE>
int AggregateUtil::toAggregateRawImp(AggregateRaw *aggregate,
                                          const TYPE&        source,
                                          bdeat_TypeCategory::Simple)
{
    if (!bdlmxxx::ElemType::isScalarType(aggregate->dataType())) {
        return -1;                                                    // RETURN
    }

    return toAggregateRawFromSimple(aggregate, source);
}

template <class TYPE>
int AggregateUtil::toAggregateRawFromNullable(
                                                  AggregateRaw *aggregate,
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

template <class TYPE, class CONVERTER>
int AggregateUtil::toAggregateRawFromVectorOfPrimitive(
                                           AggregateRaw        *aggregate,
                                           const bsl::vector<TYPE>&  source,
                                           CONVERTER                 converter)
{
    const int length = static_cast<int>(source.size());

    ErrorAttributes error;
    aggregate->resize(&error, length);

    AggregateRaw item;
    for (int i = 0; i < length; ++i) {
        aggregate->getArrayItem(&item, &error, i);

        const int rc = converter(&item, source[i]);
        if (rc) {
            return rc;                                                // RETURN
        }
    }
    return 0;
}

template <class TYPE, class CONVERTER>
int AggregateUtil::toAggregateFromVectorOfComplex(
                                           Aggregate           *aggregate,
                                           const bsl::vector<TYPE>&  source,
                                           CONVERTER                 converter)
{
    const int length = static_cast<int>(source.size());
    aggregate->resize(length);

    for (int i = 0; i < length; ++i) {
        Aggregate item = (*aggregate)[i];
        const int rc = converter(&item, source[i]);
        if (rc) {
            return rc;                                                // RETURN
        }
    }
    return 0;
}

template <class TYPE>
int AggregateUtil::toAggregateFromVectorOfNullable(
                                           Aggregate           *aggregate,
                                           int                       fieldId,
                                           const bsl::vector<TYPE>&  source,
                                           bdeat_TypeCategory::Choice)
{
    Aggregate field = aggregate->fieldById(fieldId);
    if (field.isError()) {
        return field.errorCode();                                     // RETURN
    }

    if (bdlmxxx::ElemType::BDEM_CHOICE_ARRAY == field.dataType()) {
        return toAggregateFromVectorOfComplex(&field,
                                              source,
                                              toAggregateFromNullable<TYPE>);
                                                                      // RETURN
    }
    return -1;
}

template <class TYPE>
int AggregateUtil::toAggregateFromVectorOfNullable(
                                           Aggregate           *aggregate,
                                           int                       fieldId,
                                           const bsl::vector<TYPE>&  source,
                                           bdeat_TypeCategory::CustomizedType)
{
    Aggregate field = aggregate->fieldById(fieldId);
    if (field.isError()) {
        return field.errorCode();                                     // RETURN
    }

    bdlmxxx::ElemType::Type expectedType = bdlmxxx::ElemType::toArrayType(
              static_cast<bdlmxxx::ElemType::Type>(bdlmxxx::SelectBdemType<
                                  typename TYPE::ValueType::BaseType>::VALUE));

    if (bdlmxxx::ElemType::BDEM_TABLE == field.dataType()
     || expectedType              == field.dataType()) {
        return toAggregateFromVectorOfComplex(&field,
                                              source,
                                              toAggregateFromNullable<TYPE>);
                                                                      // RETURN
    }
    return -1;
}

template <class TYPE>
int AggregateUtil::toAggregateFromVectorOfNullable(
                                           Aggregate           *aggregate,
                                           int                       fieldId,
                                           const bsl::vector<TYPE>&  source,
                                           bdeat_TypeCategory::Enumeration)
{
    AggregateRaw    aggregateRaw = aggregate->aggregateRaw();
    AggregateRaw    field;
    ErrorAttributes error;
    if (0 != aggregateRaw.fieldById(&field, &error, fieldId)) {
        return error.code();                                          // RETURN
    }

    if (bdlmxxx::ElemType::BDEM_TABLE        == field.dataType()
     || bdlmxxx::ElemType::BDEM_INT_ARRAY    == field.dataType()
     || bdlmxxx::ElemType::BDEM_STRING_ARRAY == field.dataType()) {
        return toAggregateRawFromVectorOfPrimitive(
                                             &field,
                                             source,
                                             toAggregateRawFromNullable<TYPE>);
                                                                      // RETURN
    }
    return -1;
}

template <class TYPE>
int AggregateUtil::toAggregateFromVectorOfNullable(
                                           Aggregate           *aggregate,
                                           int                       fieldId,
                                           const bsl::vector<TYPE>&  source,
                                           bdeat_TypeCategory::Sequence)
{
    Aggregate field  = aggregate->fieldById(fieldId);
    if (field.isError()) {
        return field.errorCode();                                     // RETURN
    }

    if (bdlmxxx::ElemType::BDEM_TABLE == field.dataType()) {
        return toAggregateFromVectorOfComplex(&field,
                                              source,
                                              toAggregateFromNullable<TYPE>);
                                                                      // RETURN
    }
    return -1;
}

template <class TYPE>
int AggregateUtil::toAggregateFromVectorOfNullable(
                                           Aggregate           *aggregate,
                                           int                       fieldId,
                                           const bsl::vector<TYPE>&  source,
                                           bdeat_TypeCategory::Simple)
{
    AggregateRaw    aggregateRaw = aggregate->aggregateRaw();
    AggregateRaw    field;
    ErrorAttributes error;
    if (0 != aggregateRaw.fieldById(&field, &error, fieldId)) {
        return error.code();                                          // RETURN
    }

    typedef typename
    AggregateUtil_MakeSigned<typename TYPE::ValueType>::Type SIGNED_TYPE;

    bdlmxxx::ElemType::Type expectedType = bdlmxxx::ElemType::toArrayType(
    static_cast<bdlmxxx::ElemType::Type>(bdlmxxx::SelectBdemType<SIGNED_TYPE>::VALUE));

    if (bdlmxxx::ElemType::BDEM_TABLE       == field.dataType()
     || expectedType                    == field.dataType()
     || bdlmxxx::ElemType::BDEM_SHORT_ARRAY == field.dataType()
     || bdlmxxx::ElemType::BDEM_INT_ARRAY   == field.dataType()
     || bdlmxxx::ElemType::BDEM_INT64_ARRAY == field.dataType()) {
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

template <class TYPE>
int AggregateUtil::toAggregateImp(Aggregate *aggregate,
                                       const TYPE&     source,
                                       bdeat_TypeCategory::Array)
{
    // This case is only for handling 'bsl::vector<char>'.

    if (bdlmxxx::ElemType::BDEM_CHAR_ARRAY == aggregate->dataType()) {
        aggregate->asElemRef().theModifiableCharArray() = source;
        return 0;                                                     // RETURN
    }
    return -1;
}

template <class TYPE>
int AggregateUtil::toAggregateImp(Aggregate *aggregate,
                                       const TYPE&     source,
                                       bdeat_TypeCategory::Choice)
{
    if (bdlmxxx::ElemType::BDEM_CHOICE            == aggregate->dataType()
     || bdlmxxx::ElemType::BDEM_CHOICE_ARRAY_ITEM == aggregate->dataType()) {
        return toAggregateFromComplex(aggregate, source);             // RETURN
    }
    return -1;
}

template <class TYPE>
int AggregateUtil::toAggregateImp(Aggregate *aggregate,
                                       const TYPE&     source,
                                       bdeat_TypeCategory::CustomizedType)
{
    bdlmxxx::ElemType::Type expectedType = static_cast<bdlmxxx::ElemType::Type>(
                          bdlmxxx::SelectBdemType<typename TYPE::BaseType>::VALUE);
    if (expectedType == aggregate->dataType()) {
        return toAggregateFromComplex(aggregate, source);             // RETURN
    }
    return -1;
}

template <class TYPE>
inline
int AggregateUtil::toAggregateImp(Aggregate *aggregate,
                                       const TYPE&     source,
                                       bdeat_TypeCategory::Enumeration)
{
    return toAggregateRawFromEnumeration(&aggregate->aggregateRaw(), source);
}

template <class TYPE>
int AggregateUtil::toAggregateImp(Aggregate *aggregate,
                                       const TYPE&     source,
                                       bdeat_TypeCategory::Sequence)
{
    if (bdlmxxx::ElemType::BDEM_LIST == aggregate->dataType()
     || bdlmxxx::ElemType::BDEM_ROW  == aggregate->dataType()) {
        return toAggregateFromComplex(aggregate, source);             // RETURN
    }
    return -1;
}

template <class TYPE>
int AggregateUtil::toAggregateImp(Aggregate *aggregate,
                                       const TYPE&     source,
                                       bdeat_TypeCategory::Simple)
{
    if (!bdlmxxx::ElemType::isScalarType(aggregate->dataType())) {
        return -1;                                                    // RETURN
    }

    return toAggregateRawFromSimple(&aggregate->aggregateRaw(), source);
}

template <class TYPE>
int AggregateUtil::toAggregateFromNullable(Aggregate *aggregate,
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

template <class TYPE>
int AggregateUtil::toAggregateFromVector(
                                           Aggregate           *aggregate,
                                           int                       fieldId,
                                           const bsl::vector<TYPE>&  source,
                                           bdeat_TypeCategory::Enumeration)
{
    AggregateRaw    aggregateRaw = aggregate->aggregateRaw();
    AggregateRaw    field;
    ErrorAttributes error;
    if (0 != aggregateRaw.fieldById(&field, &error, fieldId)) {
        return error.code();                                          // RETURN
    }

    if (bdlmxxx::ElemType::BDEM_INT_ARRAY    == field.dataType()
     || bdlmxxx::ElemType::BDEM_STRING_ARRAY == field.dataType()) {
        return toAggregateRawFromVectorOfPrimitive(
                                          &field,
                                          source,
                                          toAggregateRawFromEnumeration<TYPE>);
                                                                      // RETURN
    }
    return -1;
}

template <class TYPE>
inline
int AggregateUtil::toAggregateFromVector(
                                           Aggregate           *aggregate,
                                           int                       fieldId,
                                           const bsl::vector<TYPE>&  source,
                                           bdeat_TypeCategory::NullableValue)
{
    typedef typename
    bdeat_TypeCategory::Select<typename TYPE::ValueType>::Type TypeTag;

    return toAggregateFromVectorOfNullable(aggregate,
                                           fieldId,
                                           source,
                                           TypeTag());
}

template <class TYPE>
int AggregateUtil::toAggregateFromVector(
                                           Aggregate           *aggregate,
                                           int                       fieldId,
                                           const bsl::vector<TYPE>&  source,
                                           bdeat_TypeCategory::Choice)
{
    Aggregate field = aggregate->fieldById(fieldId);
    if (field.isError()) {
        return field.errorCode();                                     // RETURN
    }

    if (bdlmxxx::ElemType::BDEM_CHOICE_ARRAY == field.dataType()) {
        return toAggregateFromVectorOfComplex(&field,
                                              source,
                                              toAggregateFromComplex<TYPE>);
                                                                      // RETURN
    }
    return -1;
}

template <class TYPE>
int AggregateUtil::toAggregateFromVector(
                                           Aggregate           *aggregate,
                                           int                       fieldId,
                                           const bsl::vector<TYPE>&  source,
                                           bdeat_TypeCategory::CustomizedType)
{
    Aggregate field = aggregate->fieldById(fieldId);
    if (field.isError()) {
        return field.errorCode();                                     // RETURN
    }

    bdlmxxx::ElemType::Type expectedType = bdlmxxx::ElemType::toArrayType(
                                              static_cast<bdlmxxx::ElemType::Type>(
                         bdlmxxx::SelectBdemType<typename TYPE::BaseType>::VALUE));

    if (expectedType == field.dataType()) {
        return toAggregateFromVectorOfComplex(&field,
                                              source,
                                              toAggregateFromComplex<TYPE>);
                                                                      // RETURN
    }
    return -1;
}

template <class TYPE>
int AggregateUtil::toAggregateFromVector(
                                           Aggregate           *aggregate,
                                           int                       fieldId,
                                           const bsl::vector<TYPE>&  source,
                                           bdeat_TypeCategory::Sequence)
{
    Aggregate field = aggregate->fieldById(fieldId);
    if (field.isError()) {
        return field.errorCode();                                     // RETURN
    }

    if (bdlmxxx::ElemType::BDEM_TABLE == field.dataType()) {
        return toAggregateFromVectorOfComplex(&field,
                                              source,
                                              toAggregateFromComplex<TYPE>);
                                                                      // RETURN
    }
    return -1;
}

template <class TYPE>
int AggregateUtil::toAggregateFromVector(
                                           Aggregate           *aggregate,
                                           int                       fieldId,
                                           const bsl::vector<TYPE>&  source,
                                           bdeat_TypeCategory::Simple)
{
    AggregateRaw    aggregateRaw = aggregate->aggregateRaw();
    AggregateRaw    field;
    ErrorAttributes error;
    if (0 != aggregateRaw.fieldById(&field, &error, fieldId)) {
        return error.code();                                          // RETURN
    }

    typedef typename AggregateUtil_MakeSigned<TYPE>::Type SIGNED_TYPE;

    bdlmxxx::ElemType::Type expType = bdlmxxx::ElemType::toArrayType(
    static_cast<bdlmxxx::ElemType::Type>(bdlmxxx::SelectBdemType<SIGNED_TYPE>::VALUE));

    if (expType == field.dataType()) {
        bsl::vector<SIGNED_TYPE>& array = theModifiableArray<SIGNED_TYPE>(
                     static_cast<const AggregateRaw&>(field).asElemRef());

        array = reinterpret_cast<const bsl::vector<SIGNED_TYPE>&>(source);

        return 0;                                                     // RETURN
    }
    else if (bdlmxxx::ElemType::BDEM_SHORT_ARRAY == field.dataType()
          || bdlmxxx::ElemType::BDEM_INT_ARRAY   == field.dataType()
          || bdlmxxx::ElemType::BDEM_INT64_ARRAY == field.dataType()) {
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

template <class TYPE>
inline
int AggregateUtil::toAggregateImp(Aggregate           *aggregate,
                                       int                       fieldId,
                                       const bsl::vector<TYPE>&  source,
                                       bdeat_TypeCategory::Array)
{
    typedef typename bdeat_TypeCategory::Select<TYPE>::Type TypeTag;
    return toAggregateFromVector(aggregate, fieldId, source, TypeTag());
}

template <class TYPE>
int AggregateUtil::toAggregateImp(Aggregate *aggregate,
                                       int             fieldId,
                                       const TYPE&     source,
                                       bdeat_TypeCategory::Enumeration)
{
    AggregateRaw    aggregateRaw = aggregate->aggregateRaw();
    AggregateRaw    field;
    ErrorAttributes error;
    if (0 != aggregateRaw.fieldById(&field, &error, fieldId)) {
        return error.code();                                          // RETURN
    }

    return toAggregateRawFromEnumeration(&field, source);
}

template <class TYPE>
int AggregateUtil::toAggregateImp(Aggregate *aggregate,
                                       int             fieldId,
                                       const TYPE&     source,
                                       bdeat_TypeCategory::NullableValue)
{
    Aggregate field = aggregate->fieldById(fieldId);
    if (field.isError()) {
        return field.errorCode();                                     // RETURN
    }

    return toAggregateFromNullable(&field, source);
}

template <class TYPE>
int AggregateUtil::toAggregateImp(Aggregate *aggregate,
                                       int             fieldId,
                                       const TYPE&     source,
                                       bdeat_TypeCategory::CustomizedType)
{
    Aggregate field = aggregate->fieldById(fieldId);
    if (field.isError()) {
        return field.errorCode();                                     // RETURN
    }

    bdlmxxx::ElemType::Type expectedType = static_cast<bdlmxxx::ElemType::Type>(
                          bdlmxxx::SelectBdemType<typename TYPE::BaseType>::VALUE);

    if (expectedType == field.dataType()) {
        return toAggregateFromComplex(&field, source);                // RETURN
    }
    return -1;
}

template <class TYPE>
int AggregateUtil::toAggregateImp(Aggregate *aggregate,
                                       int             fieldId,
                                       const TYPE&     source,
                                       bdeat_TypeCategory::Choice)
{
    Aggregate field = aggregate->fieldById(fieldId);
    if (field.isError()) {
        return field.errorCode();                                     // RETURN
    }

    if (bdlmxxx::ElemType::BDEM_CHOICE == field.dataType()) {
        return toAggregateFromComplex(&field, source);                // RETURN
    }
    return -1;
}

template <class TYPE>
int AggregateUtil::toAggregateImp(Aggregate *aggregate,
                                       int             fieldId,
                                       const TYPE&     source,
                                       bdeat_TypeCategory::Sequence)
{
    Aggregate field = aggregate->fieldById(fieldId);
    if (field.isError()) {
        return field.errorCode();                                     // RETURN
    }

    if (bdlmxxx::ElemType::BDEM_LIST == field.dataType()) {
        return toAggregateFromComplex(&field, source);                // RETURN
    }
    return -1;
}

template <class TYPE>
int AggregateUtil::toAggregateImp(Aggregate *aggregate,
                                       int             fieldId,
                                       const TYPE&     source,
                                       bdeat_TypeCategory::Simple)
{
    AggregateRaw    aggregateRaw = aggregate->aggregateRaw();
    AggregateRaw    field;
    ErrorAttributes error;
    if (0 != aggregateRaw.fieldById(&field, &error, fieldId)) {
        return error.code();                                          // RETURN
    }

    if (!bdlmxxx::ElemType::isScalarType(field.dataType())) {
        return -1;                                                    // RETURN
    }

    return toAggregateRawFromSimple(&field, source);
}

// CLASS METHODS
template <class TYPE>
inline
int AggregateUtil::fromAggregate(TYPE                  *destination,
                                      const Aggregate&  aggregate,
                                      int                    fieldId)
{
    typedef typename bdeat_TypeCategory::Select<TYPE>::Type TypeTag;
    return fromAggregateImp(destination, aggregate, fieldId, TypeTag());
}

template <class TYPE>
inline
int AggregateUtil::toAggregate(Aggregate *aggregate,
                                    int             fieldId,
                                    const TYPE&     source)
{
    typedef typename bdeat_TypeCategory::Select<TYPE>::Type TypeTag;
    return toAggregateImp(aggregate, fieldId, source, TypeTag());
}
}  // close package namespace

}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2008
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ------------------------------ END-OF-FILE ---------------------------------
