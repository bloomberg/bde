// bcem_aggregateutil.h                                               -*-C++-*-
#ifndef INCLUDED_BCEM_AGGREGATEUTIL
#define INCLUDED_BCEM_AGGREGATEUTIL

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide utilities for manipulating 'bcem_Aggregate' objects.
//
//@CLASSES: bcem_AggregateUtil: Utility functions for 'bcem_Aggregate' objects
//
//@AUTHOR: David Rubin (drubin6)
//
//@SEE_ALSO: bcem_aggregate
//
//@DESCRIPTION:
// This component provides utilities to convert the field(s) of a
// 'bcem_Aggregate' of "List" or "Choice" type to and from values of
// corresponding C++ types.  For a detailed description of 'bcem_Aggregate'
// types see the component-level documentation for 'bcem_aggregate'.
//
///Thread safety
///-------------
// This class is thread-safe.
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

#ifndef INCLUDED_BSLALG_TYPETRAITS
#include <bslalg_typetraits.h>
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

#ifndef INCLUDED_BDEAT_SEQUENCEFUNCTIONS
#include <bdeat_sequencefunctions.h>
#endif

#ifndef INCLUDED_BSLMF_ASSERT
#include <bslmf_assert.h>
#endif

#ifndef INCLUDED_BSLMF_IF
#include <bslmf_if.h>
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

#ifndef INCLUDED_BDEUT_NULLABLEALLOCATEDVALUE
#include <bdeut_nullableallocatedvalue.h>
#endif

#ifndef INCLUDED_BDEUT_NULLABLEVALUE
#include <bdeut_nullablevalue.h>
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
    // 'bcem_Aggregate' objects.  The overloaded 'toAggregate' and
    // 'fromAggregate' functions convert between aggregates and primitive
    // types, nullable primitive types, and arrays of primitive types
    // (including 'bsl::string', 'bdes' 64-bit integer types, and a variety of
    // 'bdet' time and date types).

    // PRIVATE CONSTANTS
    enum Type {
        BCEM_PRIMITIVE
      , BCEM_ENUMERATION
      , BCEM_COMPLEX
    };

    // PRIVATE TYPES
    template <typename TYPE>
    struct TypeCategoryOf {
        // Provides a template for deducing the type category of the
        // parameterized 'TYPE' at compile time.

        // TYPES
        typedef
        typename bslmf_If<
          bdeat_SequenceFunctions::IsSequence<TYPE>::VALUE,
          bslmf_MetaInt<BCEM_COMPLEX>,
          typename bslmf_If<
            bdeat_ChoiceFunctions::IsChoice<TYPE>::VALUE,
            bslmf_MetaInt<BCEM_COMPLEX>,
            typename bslmf_If<
              bdeat_CustomizedTypeFunctions::IsCustomizedType<TYPE>::VALUE,
              bslmf_MetaInt<BCEM_COMPLEX>,
              typename bslmf_If<
                bdeat_EnumFunctions::IsEnumeration<TYPE>::VALUE,
                bslmf_MetaInt<BCEM_ENUMERATION>,
                bslmf_MetaInt<BCEM_PRIMITIVE>
              >::Type
            >::Type
          >::Type
        >::Type Result;
    };

    typedef bsls_PlatformUtil::Uint64 Uint64;
    typedef bsls_PlatformUtil::Int64  Int64;
        // Define type aliases for 64-bit integer types.

    // Conversion To and From Primitive Types

    static int fromAggregatePrimitive(bool                       *result,
                                      const bcem_Aggregate&       aggregate,
                                      int                         fieldId);
    static int fromAggregatePrimitive(char                       *result,
                                      const bcem_Aggregate&       aggregate,
                                      int                         fieldId);
    static int fromAggregatePrimitive(short                      *result,
                                      const bcem_Aggregate&       aggregate,
                                      int                         fieldId);
    static int fromAggregatePrimitive(int                        *result,
                                      const bcem_Aggregate&       aggregate,
                                      int                         fieldId);
    static int fromAggregatePrimitive(float                      *result,
                                      const bcem_Aggregate&       aggregate,
                                      int                         fieldId);
    static int fromAggregatePrimitive(double                     *result,
                                      const bcem_Aggregate&       aggregate,
                                      int                         fieldId);
    static int fromAggregatePrimitive(unsigned char              *result,
                                      const bcem_Aggregate&       aggregate,
                                      int                         fieldId);
    static int fromAggregatePrimitive(unsigned short             *result,
                                      const bcem_Aggregate&       aggregate,
                                      int                         fieldId);
    static int fromAggregatePrimitive(unsigned int               *result,
                                      const bcem_Aggregate&       aggregate,
                                      int                         fieldId);
    static int fromAggregatePrimitive(bsl::string                *result,
                                      const bcem_Aggregate&       aggregate,
                                      int                         fieldId);
    static int fromAggregatePrimitive(Int64                      *result,
                                      const bcem_Aggregate&       aggregate,
                                      int                         fieldId);
    static int fromAggregatePrimitive(Uint64                     *result,
                                      const bcem_Aggregate&       aggregate,
                                      int                         fieldId);
    static int fromAggregatePrimitive(bdet_DateTz                *result,
                                      const bcem_Aggregate&       aggregate,
                                      int                         fieldId);
    static int fromAggregatePrimitive(bdet_DatetimeTz            *result,
                                      const bcem_Aggregate&       aggregate,
                                      int                         fieldId);
    static int fromAggregatePrimitive(bdet_TimeTz                *result,
                                      const bcem_Aggregate&       aggregate,
                                      int                         fieldId);
    static int fromAggregatePrimitive(bdet_Date                  *result,
                                      const bcem_Aggregate&       aggregate,
                                      int                         fieldId);
    static int fromAggregatePrimitive(bdet_Datetime              *result,
                                      const bcem_Aggregate&       aggregate,
                                      int                         fieldId);
    static int fromAggregatePrimitive(bdet_Time                  *result,
                                      const bcem_Aggregate&       aggregate,
                                      int                         fieldId);
        // Store into the specified 'result' the value in the field of the
        // specified 'aggregate' selected by the specified 'fieldId'.  Return
        // 0 on success, and a non-zero value otherwise.  Note that any
        // non-zero result can by converted to an 'bsl::string' using the
        // 'errorString' method.

    static int toAggregatePrimitive(bcem_Aggregate            *result,
                                    int                        fieldId,
                                    bool                       value);
    static int toAggregatePrimitive(bcem_Aggregate            *result,
                                    int                        fieldId,
                                    char                       value);
    static int toAggregatePrimitive(bcem_Aggregate            *result,
                                    int                        fieldId,
                                    short                      value);
    static int toAggregatePrimitive(bcem_Aggregate            *result,
                                    int                        fieldId,
                                    int                        value);
    static int toAggregatePrimitive(bcem_Aggregate            *result,
                                    int                        fieldId,
                                    float                      value);
    static int toAggregatePrimitive(bcem_Aggregate            *result,
                                    int                        fieldId,
                                    double                     value);
    static int toAggregatePrimitive(bcem_Aggregate            *result,
                                    int                        fieldId,
                                    unsigned char              value);
    static int toAggregatePrimitive(bcem_Aggregate            *result,
                                    int                        fieldId,
                                    unsigned short             value);
    static int toAggregatePrimitive(bcem_Aggregate            *result,
                                    int                        fieldId,
                                    unsigned int               value);
    static int toAggregatePrimitive(bcem_Aggregate            *result,
                                    int                        fieldId,
                                    const bsl::string&         value);
    static int toAggregatePrimitive(bcem_Aggregate            *result,
                                    int                        fieldId,
                                    Int64                      value);
    static int toAggregatePrimitive(bcem_Aggregate            *result,
                                    int                        fieldId,
                                    Uint64                     value);
    static int toAggregatePrimitive(bcem_Aggregate            *result,
                                    int                        fieldId,
                                    const bdet_DateTz&         value);
    static int toAggregatePrimitive(bcem_Aggregate            *result,
                                    int                        fieldId,
                                    const bdet_DatetimeTz&     value);
    static int toAggregatePrimitive(bcem_Aggregate            *result,
                                    int                        fieldId,
                                    const bdet_TimeTz&         value);
    static int toAggregatePrimitive(bcem_Aggregate            *result,
                                    int                        fieldId,
                                    const bdet_Date&           value);
    static int toAggregatePrimitive(bcem_Aggregate            *result,
                                    int                        fieldId,
                                    const bdet_Datetime&       value);
    static int toAggregatePrimitive(bcem_Aggregate            *result,
                                    int                        fieldId,
                                    const bdet_Time&           value);
        // Store the specified 'value' into the specified 'result' aggregate in
        // the field selected by the specified 'fieldId'.  Return 0 on success,
        // and a non-zero value otherwise.  Note that any non-zero result can
        // by converted to an 'bsl::string' using the 'errorString' method.

    static int setValue(const bcem_Aggregate&   result,
                        const bsl::string&      value);
        // Store the specified 'value' into the specified 'result' aggregate.
        // Return 0 on success, and a non-zero value otherwise.  Note that any
        // non-zero result can by converted to an 'bsl::string' using the
        // 'errorString' method.

    // Conversion To and From Nullable Primitive Types

    static int fromAggregatePrimitive(
            bdeut_NullableValue<bool>                      *result,
            const bcem_Aggregate&                           aggregate,
            int                                             fieldId);
    static int fromAggregatePrimitive(
            bdeut_NullableValue<char>                      *result,
            const bcem_Aggregate&                           aggregate,
            int                                             fieldId);
    static int fromAggregatePrimitive(
            bdeut_NullableValue<short>                     *result,
            const bcem_Aggregate&                           aggregate,
            int                                             fieldId);
    static int fromAggregatePrimitive(
            bdeut_NullableValue<int>                       *result,
            const bcem_Aggregate&                           aggregate,
            int                                             fieldId);
    static int fromAggregatePrimitive(
            bdeut_NullableValue<float>                     *result,
            const bcem_Aggregate&                           aggregate,
            int                                             fieldId);
    static int fromAggregatePrimitive(
            bdeut_NullableValue<double>                    *result,
            const bcem_Aggregate&                           aggregate,
            int                                             fieldId);
    static int fromAggregatePrimitive(
            bdeut_NullableValue<unsigned char>             *result,
            const bcem_Aggregate&                           aggregate,
            int                                             fieldId);
    static int fromAggregatePrimitive(
            bdeut_NullableValue<unsigned short>            *result,
            const bcem_Aggregate&                           aggregate,
            int                                             fieldId);
    static int fromAggregatePrimitive(
            bdeut_NullableValue<unsigned int>              *result,
            const bcem_Aggregate&                           aggregate,
            int                                             fieldId);
    static int fromAggregatePrimitive(
            bdeut_NullableValue<bsl::string>               *result,
            const bcem_Aggregate&                           aggregate,
            int                                             fieldId);
    static int fromAggregatePrimitive(
            bdeut_NullableValue<Int64>                      *result,
            const bcem_Aggregate&                           aggregate,
            int                                             fieldId);
    static int fromAggregatePrimitive(
            bdeut_NullableValue<Uint64>                    *result,
            const bcem_Aggregate&                           aggregate,
            int                                             fieldId);
    static int fromAggregatePrimitive(
            bdeut_NullableValue<bdet_DateTz>               *result,
            const bcem_Aggregate&                           aggregate,
            int                                             fieldId);
    static int fromAggregatePrimitive(
            bdeut_NullableValue<bdet_DatetimeTz>           *result,
            const bcem_Aggregate&                           aggregate,
            int                                             fieldId);
    static int fromAggregatePrimitive(
            bdeut_NullableValue<bdet_TimeTz>               *result,
            const bcem_Aggregate&                           aggregate,
            int                                             fieldId);
    static int fromAggregatePrimitive(
            bdeut_NullableValue<bdet_Date>                 *result,
            const bcem_Aggregate&                           aggregate,
            int                                             fieldId);
    static int fromAggregatePrimitive(
            bdeut_NullableValue<bdet_Datetime>             *result,
            const bcem_Aggregate&                           aggregate,
            int                                             fieldId);
    static int fromAggregatePrimitive(
            bdeut_NullableValue<bdet_Time>                 *result,
            const bcem_Aggregate&                           aggregate,
            int                                             fieldId);
    static int fromAggregatePrimitive(
            bdeut_NullableValue< bsl::vector<char> >       *result,
            const bcem_Aggregate&                           aggregate,
            int                                             fieldId);
        // Store into the specified 'result' the value in the field of the
        // specified 'aggregate' selected by the specified 'fieldId'.  Return
        // 0 on success, and a non-zero value otherwise.  Note that any
        // non-zero result can by converted to an 'bsl::string' using the
        // 'errorString' method.

    static int toAggregatePrimitive(
            bcem_Aggregate                                       *result,
            int                                                   fieldId,
            const bdeut_NullableValue<bool>&                      value);
    static int toAggregatePrimitive(
            bcem_Aggregate                                       *result,
            int                                                   fieldId,
            const bdeut_NullableValue<char>&                      value);
    static int toAggregatePrimitive(
            bcem_Aggregate                                       *result,
            int                                                   fieldId,
            const bdeut_NullableValue<short>&                     value);
    static int toAggregatePrimitive(
            bcem_Aggregate                                       *result,
            int                                                   fieldId,
            const bdeut_NullableValue<int>&                       value);
    static int toAggregatePrimitive(
            bcem_Aggregate                                       *result,
            int                                                   fieldId,
            const bdeut_NullableValue<float>&                     value);
    static int toAggregatePrimitive(
            bcem_Aggregate                                       *result,
            int                                                   fieldId,
            const bdeut_NullableValue<double>&                    value);
    static int toAggregatePrimitive(
            bcem_Aggregate                                       *result,
            int                                                   fieldId,
            const bdeut_NullableValue<unsigned char>&             value);
    static int toAggregatePrimitive(
            bcem_Aggregate                                       *result,
            int                                                   fieldId,
            const bdeut_NullableValue<unsigned short>&            value);
    static int toAggregatePrimitive(
            bcem_Aggregate                                       *result,
            int                                                   fieldId,
            const bdeut_NullableValue<unsigned int>&              value);
    static int toAggregatePrimitive(
            bcem_Aggregate                                       *result,
            int                                                   fieldId,
            const bdeut_NullableValue<bsl::string>&               value);
    static int toAggregatePrimitive(
            bcem_Aggregate                                       *result,
            int                                                   fieldId,
            const bdeut_NullableValue<Int64>&                     value);
    static int toAggregatePrimitive(
            bcem_Aggregate                                       *result,
            int                                                   fieldId,
            const bdeut_NullableValue<Uint64>&                    value);
    static int toAggregatePrimitive(
            bcem_Aggregate                                       *result,
            int                                                   fieldId,
            const bdeut_NullableValue<bdet_DateTz>&               value);
    static int toAggregatePrimitive(
            bcem_Aggregate                                       *result,
            int                                                   fieldId,
            const bdeut_NullableValue<bdet_DatetimeTz>&           value);
    static int toAggregatePrimitive(
            bcem_Aggregate                                       *result,
            int                                                   fieldId,
            const bdeut_NullableValue<bdet_TimeTz>&               value);
    static int toAggregatePrimitive(
            bcem_Aggregate                                       *result,
            int                                                   fieldId,
            const bdeut_NullableValue<bdet_Date>&                 value);
    static int toAggregatePrimitive(
            bcem_Aggregate                                       *result,
            int                                                   fieldId,
            const bdeut_NullableValue<bdet_Datetime>&             value);
    static int toAggregatePrimitive(
            bcem_Aggregate                                       *result,
            int                                                   fieldId,
            const bdeut_NullableValue<bdet_Time>&                 value);
    static int toAggregatePrimitive(
            bcem_Aggregate                                       *result,
            int                                                   fieldId,
            const bdeut_NullableValue< bsl::vector<char> >&       value);
        // Store the specified 'value' into the specified 'result' aggregate in
        // the field selected by the specified 'fieldId'.  Return 0 on success,
        // and a non-zero value otherwise.  Note that any non-zero result can
        // by converted to an 'bsl::string' using the 'errorString' method.

    // Conversion To and From Nullable-Allocated Primitive Types

    static int fromAggregatePrimitive(
            bdeut_NullableAllocatedValue<bool>                *result,
            const bcem_Aggregate&                              aggregate,
            int                                                fieldId);
    static int fromAggregatePrimitive(
            bdeut_NullableAllocatedValue<char>                *result,
            const bcem_Aggregate&                              aggregate,
            int                                                fieldId);
    static int fromAggregatePrimitive(
            bdeut_NullableAllocatedValue<short>               *result,
            const bcem_Aggregate&                              aggregate,
            int                                                fieldId);
    static int fromAggregatePrimitive(
            bdeut_NullableAllocatedValue<int>                 *result,
            const bcem_Aggregate&                              aggregate,
            int                                                fieldId);
    static int fromAggregatePrimitive(
            bdeut_NullableAllocatedValue<float>               *result,
            const bcem_Aggregate&                              aggregate,
            int                                                fieldId);
    static int fromAggregatePrimitive(
            bdeut_NullableAllocatedValue<double>              *result,
            const bcem_Aggregate&                              aggregate,
            int                                                fieldId);
    static int fromAggregatePrimitive(
            bdeut_NullableAllocatedValue<unsigned char>       *result,
            const bcem_Aggregate&                              aggregate,
            int                                                fieldId);
    static int fromAggregatePrimitive(
            bdeut_NullableAllocatedValue<unsigned short>      *result,
            const bcem_Aggregate&                              aggregate,
            int                                                fieldId);
    static int fromAggregatePrimitive(
            bdeut_NullableAllocatedValue<unsigned int>        *result,
            const bcem_Aggregate&                              aggregate,
            int                                                fieldId);
    static int fromAggregatePrimitive(
            bdeut_NullableAllocatedValue<bsl::string>         *result,
            const bcem_Aggregate&                              aggregate,
            int                                                fieldId);
    static int fromAggregatePrimitive(
            bdeut_NullableAllocatedValue<Int64>               *result,
            const bcem_Aggregate&                              aggregate,
            int                                                fieldId);
    static int fromAggregatePrimitive(
            bdeut_NullableAllocatedValue<Uint64>              *result,
            const bcem_Aggregate&                              aggregate,
            int                                                fieldId);
    static int fromAggregatePrimitive(
            bdeut_NullableAllocatedValue<bdet_DateTz>         *result,
            const bcem_Aggregate&                              aggregate,
            int                                                fieldId);
    static int fromAggregatePrimitive(
            bdeut_NullableAllocatedValue<bdet_DatetimeTz>     *result,
            const bcem_Aggregate&                              aggregate,
            int                                                fieldId);
    static int fromAggregatePrimitive(
            bdeut_NullableAllocatedValue<bdet_TimeTz>         *result,
            const bcem_Aggregate&                              aggregate,
            int                                                fieldId);
    static int fromAggregatePrimitive(
            bdeut_NullableAllocatedValue<bdet_Date>           *result,
            const bcem_Aggregate&                              aggregate,
            int                                                fieldId);
    static int fromAggregatePrimitive(
            bdeut_NullableAllocatedValue<bdet_Datetime>       *result,
            const bcem_Aggregate&                              aggregate,
            int                                                fieldId);
    static int fromAggregatePrimitive(
            bdeut_NullableAllocatedValue<bdet_Time>           *result,
            const bcem_Aggregate&                              aggregate,
            int                                                fieldId);
    static int fromAggregatePrimitive(
            bdeut_NullableAllocatedValue< bsl::vector<char> > *result,
            const bcem_Aggregate&                              aggregate,
            int                                                fieldId);
        // Store into the specified 'result' the value in the field of the
        // specified 'aggregate' selected by the specified 'fieldId'.  Return
        // 0 on success, and a non-zero value otherwise.  Note that any
        // non-zero result can by converted to an 'bsl::string' using the
        // 'errorString' method.

    static int toAggregatePrimitive(
            bcem_Aggregate                                           *result,
            int                                                       fieldId,
            const bdeut_NullableAllocatedValue<bool>&                 value);
    static int toAggregatePrimitive(
            bcem_Aggregate                                           *result,
            int                                                       fieldId,
            const bdeut_NullableAllocatedValue<char>&                 value);
    static int toAggregatePrimitive(
            bcem_Aggregate                                           *result,
            int                                                       fieldId,
            const bdeut_NullableAllocatedValue<short>&                value);
    static int toAggregatePrimitive(
            bcem_Aggregate                                           *result,
            int                                                       fieldId,
            const bdeut_NullableAllocatedValue<int>&                  value);
    static int toAggregatePrimitive(
            bcem_Aggregate                                           *result,
            int                                                       fieldId,
            const bdeut_NullableAllocatedValue<float>&                value);
    static int toAggregatePrimitive(
            bcem_Aggregate                                           *result,
            int                                                       fieldId,
            const bdeut_NullableAllocatedValue<double>&               value);
    static int toAggregatePrimitive(
            bcem_Aggregate                                           *result,
            int                                                       fieldId,
            const bdeut_NullableAllocatedValue<unsigned char>&        value);
    static int toAggregatePrimitive(
            bcem_Aggregate                                           *result,
            int                                                       fieldId,
            const bdeut_NullableAllocatedValue<unsigned short>&       value);
    static int toAggregatePrimitive(
            bcem_Aggregate                                           *result,
            int                                                       fieldId,
            const bdeut_NullableAllocatedValue<unsigned int>&         value);
    static int toAggregatePrimitive(
            bcem_Aggregate                                           *result,
            int                                                       fieldId,
            const bdeut_NullableAllocatedValue<bsl::string>&          value);
    static int toAggregatePrimitive(
            bcem_Aggregate                                           *result,
            int                                                       fieldId,
            const bdeut_NullableAllocatedValue<Int64>&                value);
    static int toAggregatePrimitive(
            bcem_Aggregate                                           *result,
            int                                                       fieldId,
            const bdeut_NullableAllocatedValue<Uint64>&               value);
    static int toAggregatePrimitive(
            bcem_Aggregate                                           *result,
            int                                                       fieldId,
            const bdeut_NullableAllocatedValue<bdet_DateTz>&          value);
    static int toAggregatePrimitive(
            bcem_Aggregate                                           *result,
            int                                                       fieldId,
            const bdeut_NullableAllocatedValue<bdet_DatetimeTz>&      value);
    static int toAggregatePrimitive(
            bcem_Aggregate                                           *result,
            int                                                       fieldId,
            const bdeut_NullableAllocatedValue<bdet_TimeTz>&          value);
    static int toAggregatePrimitive(
            bcem_Aggregate                                           *result,
            int                                                       fieldId,
            const bdeut_NullableAllocatedValue<bdet_Date>&            value);
    static int toAggregatePrimitive(
            bcem_Aggregate                                           *result,
            int                                                       fieldId,
            const bdeut_NullableAllocatedValue<bdet_Datetime>&        value);
    static int toAggregatePrimitive(
            bcem_Aggregate                                           *result,
            int                                                       fieldId,
            const bdeut_NullableAllocatedValue<bdet_Time>&            value);
    static int toAggregatePrimitive(
            bcem_Aggregate                                           *result,
            int                                                       fieldId,
            const bdeut_NullableAllocatedValue< bsl::vector<char> >&  value);
        // Store the specified 'value' into the specified 'result' aggregate in
        // the field selected by the specified 'fieldId'.  Return 0 on success,
        // and a non-zero value otherwise.  Note that any non-zero result can
        // by converted to an 'bsl::string' using the 'errorString' method.

    // Conversion To and From Arrays of Primitive Types

    static int fromAggregatePrimitive(
            bsl::vector<bool>                      *result,
            const bcem_Aggregate&                   aggregate,
            int                                     fieldId);
    static int fromAggregatePrimitive(
            bsl::vector<char>                      *result,
            const bcem_Aggregate&                   aggregate,
            int                                     fieldId);
    static int fromAggregatePrimitive(
            bsl::vector<short>                     *result,
            const bcem_Aggregate&                   aggregate,
            int                                     fieldId);
    static int fromAggregatePrimitive(
            bsl::vector<int>                       *result,
            const bcem_Aggregate&                   aggregate,
            int                                     fieldId);
    static int fromAggregatePrimitive(
            bsl::vector<float>                     *result,
            const bcem_Aggregate&                   aggregate,
            int                                     fieldId);
    static int fromAggregatePrimitive(
            bsl::vector<double>                    *result,
            const bcem_Aggregate&                   aggregate,
            int                                     fieldId);
    static int fromAggregatePrimitive(
            bsl::vector<unsigned char>             *result,
            const bcem_Aggregate&                   aggregate,
            int                                     fieldId);
    static int fromAggregatePrimitive(
            bsl::vector<unsigned short>            *result,
            const bcem_Aggregate&                   aggregate,
            int                                     fieldId);
    static int fromAggregatePrimitive(
            bsl::vector<unsigned int>              *result,
            const bcem_Aggregate&                   aggregate,
            int                                     fieldId);
    static int fromAggregatePrimitive(
            bsl::vector<bsl::string>               *result,
            const bcem_Aggregate&                   aggregate,
            int                                     fieldId);
    static int fromAggregatePrimitive(
            bsl::vector<bsls_PlatformUtil::Int64>  *result,
            const bcem_Aggregate&                   aggregate,
            int                                     fieldId);
    static int fromAggregatePrimitive(
            bsl::vector<bsls_PlatformUtil::Uint64> *result,
            const bcem_Aggregate&                   aggregate,
            int                                     fieldId);
    static int fromAggregatePrimitive(
            bsl::vector<bdet_DateTz>               *result,
            const bcem_Aggregate&                   aggregate,
            int                                     fieldId);
    static int fromAggregatePrimitive(
            bsl::vector<bdet_DatetimeTz>           *result,
            const bcem_Aggregate&                   aggregate,
            int                                     fieldId);
    static int fromAggregatePrimitive(
            bsl::vector<bdet_TimeTz>               *result,
            const bcem_Aggregate&                   aggregate,
            int                                     fieldId);
    static int fromAggregatePrimitive(
            bsl::vector<bdet_Date>                 *result,
            const bcem_Aggregate&                   aggregate,
            int                                     fieldId);
    static int fromAggregatePrimitive(
            bsl::vector<bdet_Datetime>             *result,
            const bcem_Aggregate&                   aggregate,
            int                                     fieldId);
    static int fromAggregatePrimitive(
            bsl::vector<bdet_Time>                 *result,
            const bcem_Aggregate&                   aggregate,
            int                                     fieldId);
        // Store into the specified 'result' the value in the field of the
        // specified 'aggregate' selected by the specified 'fieldId'.  Return
        // 0 on success, and a non-zero value otherwise.  Note that any
        // non-zero result can by converted to an 'bsl::string' using the
        // 'errorString' method.

    static int toAggregatePrimitive(
            bcem_Aggregate                               *result,
            int                                           fieldId,
            const bsl::vector<bool>&                      value);
    static int toAggregatePrimitive(
            bcem_Aggregate                               *result,
            int                                           fieldId,
            const bsl::vector<char>&                      value);
    static int toAggregatePrimitive(
            bcem_Aggregate                               *result,
            int                                           fieldId,
            const bsl::vector<short>&                     value);
    static int toAggregatePrimitive(
            bcem_Aggregate                               *result,
            int                                           fieldId,
            const bsl::vector<int>&                       value);
    static int toAggregatePrimitive(
            bcem_Aggregate                               *result,
            int                                           fieldId,
            const bsl::vector<float>&                     value);
    static int toAggregatePrimitive(
            bcem_Aggregate                               *result,
            int                                           fieldId,
            const bsl::vector<double>&                    value);
    static int toAggregatePrimitive(
            bcem_Aggregate                               *result,
            int                                           fieldId,
            const bsl::vector<unsigned char>&             value);
    static int toAggregatePrimitive(
            bcem_Aggregate                               *result,
            int                                           fieldId,
            const bsl::vector<unsigned short>&            value);
    static int toAggregatePrimitive(
            bcem_Aggregate                               *result,
            int                                           fieldId,
            const bsl::vector<unsigned int>&              value);
    static int toAggregatePrimitive(
            bcem_Aggregate                               *result,
            int                                           fieldId,
            const bsl::vector<bsl::string>&               value);
    static int toAggregatePrimitive(
            bcem_Aggregate                               *result,
            int                                           fieldId,
            const bsl::vector<bsls_PlatformUtil::Int64>&  value);
    static int toAggregatePrimitive(
            bcem_Aggregate                               *result,
            int                                           fieldId,
            const bsl::vector<bsls_PlatformUtil::Uint64>& value);
    static int toAggregatePrimitive(
            bcem_Aggregate                               *result,
            int                                           fieldId,
            const bsl::vector<bdet_DateTz>&               value);
    static int toAggregatePrimitive(
            bcem_Aggregate                               *result,
            int                                           fieldId,
            const bsl::vector<bdet_DatetimeTz>&           value);
    static int toAggregatePrimitive(
            bcem_Aggregate                               *result,
            int                                           fieldId,
            const bsl::vector<bdet_TimeTz>&               value);
    static int toAggregatePrimitive(
            bcem_Aggregate                               *result,
            int                                           fieldId,
            const bsl::vector<bdet_Date>&                 value);
    static int toAggregatePrimitive(
            bcem_Aggregate                               *result,
            int                                           fieldId,
            const bsl::vector<bdet_Datetime>&             value);
    static int toAggregatePrimitive(
            bcem_Aggregate                               *result,
            int                                           fieldId,
            const bsl::vector<bdet_Time>&                 value);
        // Store the specified 'value' into the specified 'result' aggregate in
        // the field selected by the specified 'fieldId'.  Return 0 on success,
        // and a non-zero value otherwise.  Note that any non-zero result can
        // by converted to an 'bsl::string' using the 'errorString' method.

    // Conversion To and From Arrays of Nullable Primitive Types

    static int fromAggregatePrimitive(
            bsl::vector< bdeut_NullableValue<bool> >            *result,
            const bcem_Aggregate&                                aggregate,
            int                                                  fieldId);
    static int fromAggregatePrimitive(
            bsl::vector< bdeut_NullableValue<char> >            *result,
            const bcem_Aggregate&                                aggregate,
            int                                                  fieldId);
    static int fromAggregatePrimitive(
            bsl::vector< bdeut_NullableValue<short> >           *result,
            const bcem_Aggregate&                                aggregate,
            int                                                  fieldId);
    static int fromAggregatePrimitive(
            bsl::vector< bdeut_NullableValue<int> >             *result,
            const bcem_Aggregate&                                aggregate,
            int                                                  fieldId);
    static int fromAggregatePrimitive(
            bsl::vector< bdeut_NullableValue<float> >           *result,
            const bcem_Aggregate&                                aggregate,
            int                                                  fieldId);
    static int fromAggregatePrimitive(
            bsl::vector< bdeut_NullableValue<double> >          *result,
            const bcem_Aggregate&                                aggregate,
            int                                                  fieldId);
    static int fromAggregatePrimitive(
            bsl::vector< bdeut_NullableValue<unsigned char> >   *result,
            const bcem_Aggregate&                                aggregate,
            int                                                  fieldId);
    static int fromAggregatePrimitive(
            bsl::vector< bdeut_NullableValue<unsigned short> >  *result,
            const bcem_Aggregate&                                aggregate,
            int                                                  fieldId);
    static int fromAggregatePrimitive(
            bsl::vector< bdeut_NullableValue<unsigned int> >    *result,
            const bcem_Aggregate&                                aggregate,
            int                                                  fieldId);
    static int fromAggregatePrimitive(
            bsl::vector< bdeut_NullableValue<bsl::string> >     *result,
            const bcem_Aggregate&                                aggregate,
            int                                                  fieldId);
    static int fromAggregatePrimitive(
            bsl::vector< bdeut_NullableValue<bsls_PlatformUtil::Int64> >
                                                                *result,
            const bcem_Aggregate&                                aggregate,
            int                                                  fieldId);
    static int fromAggregatePrimitive(
            bsl::vector< bdeut_NullableValue<bsls_PlatformUtil::Uint64> >
                                                                *result,
            const bcem_Aggregate&                                aggregate,
            int                                                  fieldId);
    static int fromAggregatePrimitive(
            bsl::vector< bdeut_NullableValue<bdet_DateTz> >     *result,
            const bcem_Aggregate&                                aggregate,
            int                                                  fieldId);
    static int fromAggregatePrimitive(
            bsl::vector< bdeut_NullableValue<bdet_DatetimeTz> > *result,
            const bcem_Aggregate&                                aggregate,
            int                                                  fieldId);
    static int fromAggregatePrimitive(
            bsl::vector< bdeut_NullableValue<bdet_TimeTz> >     *result,
            const bcem_Aggregate&                                aggregate,
            int                                                  fieldId);
    static int fromAggregatePrimitive(
            bsl::vector< bdeut_NullableValue<bdet_Date> >       *result,
            const bcem_Aggregate&                                aggregate,
            int                                                  fieldId);
    static int fromAggregatePrimitive(
            bsl::vector< bdeut_NullableValue<bdet_Datetime> >   *result,
            const bcem_Aggregate&                                aggregate,
            int                                                  fieldId);
    static int fromAggregatePrimitive(
            bsl::vector< bdeut_NullableValue<bdet_Time> >       *result,
            const bcem_Aggregate&                                aggregate,
            int                                                  fieldId);
        // Store into the specified 'result' the value in the field of the
        // specified 'aggregate' selected by the specified 'fieldId'.  Return
        // 0 on success, and a non-zero value otherwise.  Note that any
        // non-zero result can by converted to an 'bsl::string' using the
        // 'errorString' method.

    static int toAggregatePrimitive(
            bcem_Aggregate                                            *result,
            int                                                        fieldId,
            const bsl::vector< bdeut_NullableValue<bool> >&            value);
    static int toAggregatePrimitive(
            bcem_Aggregate                                            *result,
            int                                                        fieldId,
            const bsl::vector< bdeut_NullableValue<char> >&            value);
    static int toAggregatePrimitive(
            bcem_Aggregate                                            *result,
            int                                                        fieldId,
            const bsl::vector< bdeut_NullableValue<short> >&           value);
    static int toAggregatePrimitive(
            bcem_Aggregate                                            *result,
            int                                                        fieldId,
            const bsl::vector< bdeut_NullableValue<int> >&             value);
    static int toAggregatePrimitive(
            bcem_Aggregate                                            *result,
            int                                                        fieldId,
            const bsl::vector< bdeut_NullableValue<float> >&           value);
    static int toAggregatePrimitive(
            bcem_Aggregate                                            *result,
            int                                                        fieldId,
            const bsl::vector< bdeut_NullableValue<double> >&          value);
    static int toAggregatePrimitive(
            bcem_Aggregate                                            *result,
            int                                                        fieldId,
            const bsl::vector< bdeut_NullableValue<unsigned char> >&   value);
    static int toAggregatePrimitive(
            bcem_Aggregate                                            *result,
            int                                                        fieldId,
            const bsl::vector< bdeut_NullableValue<unsigned short> >&  value);
    static int toAggregatePrimitive(
            bcem_Aggregate                                            *result,
            int                                                        fieldId,
            const bsl::vector< bdeut_NullableValue<unsigned int> >&    value);
    static int toAggregatePrimitive(
            bcem_Aggregate                                            *result,
            int                                                        fieldId,
            const bsl::vector< bdeut_NullableValue<bsl::string> >&     value);
    static int toAggregatePrimitive(
            bcem_Aggregate                                            *result,
            int                                                        fieldId,
            const bsl::vector< bdeut_NullableValue<bsls_PlatformUtil::Int64> >&
                                                                       value);
    static int toAggregatePrimitive(
            bcem_Aggregate                                            *result,
            int                                                        fieldId,
            const bsl::vector<bdeut_NullableValue<bsls_PlatformUtil::Uint64> >&
                                                                       value);
    static int toAggregatePrimitive(
            bcem_Aggregate                                            *result,
            int                                                        fieldId,
            const bsl::vector< bdeut_NullableValue<bdet_DateTz> >&     value);
    static int toAggregatePrimitive(
            bcem_Aggregate                                            *result,
            int                                                        fieldId,
            const bsl::vector< bdeut_NullableValue<bdet_DatetimeTz> >& value);
    static int toAggregatePrimitive(
            bcem_Aggregate                                            *result,
            int                                                        fieldId,
            const bsl::vector< bdeut_NullableValue<bdet_TimeTz> >&     value);
    static int toAggregatePrimitive(
            bcem_Aggregate                                            *result,
            int                                                        fieldId,
            const bsl::vector< bdeut_NullableValue<bdet_Date> >&       value);
    static int toAggregatePrimitive(
            bcem_Aggregate                                            *result,
            int                                                        fieldId,
            const bsl::vector< bdeut_NullableValue<bdet_Datetime> >&   value);
    static int toAggregatePrimitive(
            bcem_Aggregate                                            *result,
            int                                                        fieldId,
            const bsl::vector< bdeut_NullableValue<bdet_Time> >&       value);
        // Store the specified 'value' into the specified 'result' aggregate in
        // the field selected by the specified 'fieldId'.  Return 0 on success,
        // and a non-zero value otherwise.  Note that any non-zero result can
        // by converted to an 'bsl::string' using the 'errorString' method.

    // Conversion To and From Arrays of Nullable-Allocated Primitive Types

    static int fromAggregatePrimitive(
        bsl::vector< bdeut_NullableAllocatedValue<bool> >           *result,
        const bcem_Aggregate&                                        aggregate,
        int                                                          fieldId);
    static int fromAggregatePrimitive(
        bsl::vector< bdeut_NullableAllocatedValue<char> >           *result,
        const bcem_Aggregate&                                        aggregate,
        int                                                          fieldId);
    static int fromAggregatePrimitive(
        bsl::vector< bdeut_NullableAllocatedValue<short> >          *result,
        const bcem_Aggregate&                                        aggregate,
        int                                                          fieldId);
    static int fromAggregatePrimitive(
        bsl::vector< bdeut_NullableAllocatedValue<int> >            *result,
        const bcem_Aggregate&                                        aggregate,
        int                                                          fieldId);
    static int fromAggregatePrimitive(
        bsl::vector< bdeut_NullableAllocatedValue<float> >          *result,
        const bcem_Aggregate&                                        aggregate,
        int                                                          fieldId);
    static int fromAggregatePrimitive(
        bsl::vector< bdeut_NullableAllocatedValue<double> >         *result,
        const bcem_Aggregate&                                        aggregate,
        int                                                          fieldId);
    static int fromAggregatePrimitive(
        bsl::vector< bdeut_NullableAllocatedValue<unsigned char> >  *result,
        const bcem_Aggregate&                                        aggregate,
        int                                                          fieldId);
    static int fromAggregatePrimitive(
        bsl::vector< bdeut_NullableAllocatedValue<unsigned short> > *result,
        const bcem_Aggregate&                                        aggregate,
        int                                                          fieldId);
    static int fromAggregatePrimitive(
        bsl::vector< bdeut_NullableAllocatedValue<unsigned int> >   *result,
        const bcem_Aggregate&                                        aggregate,
        int                                                          fieldId);
    static int fromAggregatePrimitive(
        bsl::vector< bdeut_NullableAllocatedValue<bsl::string> >    *result,
        const bcem_Aggregate&                                        aggregate,
        int                                                          fieldId);
    static int fromAggregatePrimitive(
        bsl::vector<
            bdeut_NullableAllocatedValue<bsls_PlatformUtil::Int64> >*result,
        const bcem_Aggregate&                                        aggregate,
        int                                                          fieldId);
    static int fromAggregatePrimitive(
        bsl::vector< bdeut_NullableAllocatedValue<bsls_PlatformUtil::Uint64> >
                                                                    *result,
        const bcem_Aggregate&                                        aggregate,
        int                                                          fieldId);
    static int fromAggregatePrimitive(
        bsl::vector< bdeut_NullableAllocatedValue<bdet_DateTz> >    *result,
        const bcem_Aggregate&                                        aggregate,
        int                                                          fieldId);
    static int fromAggregatePrimitive(
        bsl::vector< bdeut_NullableAllocatedValue<bdet_DatetimeTz> >*result,
        const bcem_Aggregate&                                        aggregate,
        int                                                          fieldId);
    static int fromAggregatePrimitive(
        bsl::vector< bdeut_NullableAllocatedValue<bdet_TimeTz> >    *result,
        const bcem_Aggregate&                                        aggregate,
        int                                                          fieldId);
    static int fromAggregatePrimitive(
        bsl::vector< bdeut_NullableAllocatedValue<bdet_Date> >      *result,
        const bcem_Aggregate&                                        aggregate,
        int                                                          fieldId);
    static int fromAggregatePrimitive(
        bsl::vector< bdeut_NullableAllocatedValue<bdet_Datetime> >  *result,
        const bcem_Aggregate&                                        aggregate,
        int                                                          fieldId);
    static int fromAggregatePrimitive(
        bsl::vector< bdeut_NullableAllocatedValue<bdet_Time> >      *result,
        const bcem_Aggregate&                                        aggregate,
        int                                                          fieldId);
        // Store into the specified 'result' the value in the field of the
        // specified 'aggregate' selected by the specified 'fieldId'.  Return
        // 0 on success, and a non-zero value otherwise.  Note that any
        // non-zero result can by converted to an 'bsl::string' using the
        // 'errorString' method.

    static int toAggregatePrimitive(
        bcem_Aggregate                                                *result,
        int                                                            fieldId,
        const bsl::vector< bdeut_NullableAllocatedValue<bool> >&       value);
    static int toAggregatePrimitive(
        bcem_Aggregate                                                *result,
        int                                                            fieldId,
        const bsl::vector< bdeut_NullableAllocatedValue<char> >&       value);
    static int toAggregatePrimitive(
        bcem_Aggregate                                                *result,
        int                                                            fieldId,
        const bsl::vector< bdeut_NullableAllocatedValue<short> >&      value);
    static int toAggregatePrimitive(
        bcem_Aggregate                                                *result,
        int                                                            fieldId,
        const bsl::vector< bdeut_NullableAllocatedValue<int> >&        value);
    static int toAggregatePrimitive(
        bcem_Aggregate                                                *result,
        int                                                            fieldId,
        const bsl::vector< bdeut_NullableAllocatedValue<float> >&      value);
    static int toAggregatePrimitive(
        bcem_Aggregate                                                *result,
        int                                                            fieldId,
        const bsl::vector< bdeut_NullableAllocatedValue<double> >&     value);
    static int toAggregatePrimitive(
        bcem_Aggregate                                                *result,
        int                                                            fieldId,
        const bsl::vector<
            bdeut_NullableAllocatedValue<unsigned char> >&             value);
    static int toAggregatePrimitive(
        bcem_Aggregate                                                *result,
        int                                                            fieldId,
        const bsl::vector<
            bdeut_NullableAllocatedValue<unsigned short> >&            value);
    static int toAggregatePrimitive(
        bcem_Aggregate                                                *result,
        int                                                            fieldId,
        const bsl::vector<
            bdeut_NullableAllocatedValue<unsigned int> >&              value);
    static int toAggregatePrimitive(
        bcem_Aggregate                                                *result,
        int                                                            fieldId,
        const bsl::vector<
            bdeut_NullableAllocatedValue<bsl::string> >&               value);
    static int toAggregatePrimitive(
        bcem_Aggregate                                                *result,
        int                                                            fieldId,
        const bsl::vector<
            bdeut_NullableAllocatedValue<bsls_PlatformUtil::Int64> >&  value);
    static int toAggregatePrimitive(
        bcem_Aggregate                                                *result,
        int                                                            fieldId,
        const bsl::vector<
            bdeut_NullableAllocatedValue<bsls_PlatformUtil::Uint64> >& value);
    static int toAggregatePrimitive(
        bcem_Aggregate                                                *result,
        int                                                            fieldId,
        const bsl::vector< bdeut_NullableAllocatedValue<bdet_DateTz> >&
                                                                       value);
    static int toAggregatePrimitive(
        bcem_Aggregate                                                *result,
        int                                                            fieldId,
        const bsl::vector<
            bdeut_NullableAllocatedValue<bdet_DatetimeTz> >&           value);
    static int toAggregatePrimitive(
        bcem_Aggregate                                                *result,
        int                                                            fieldId,
        const bsl::vector<
            bdeut_NullableAllocatedValue<bdet_TimeTz> >&               value);
    static int toAggregatePrimitive(
        bcem_Aggregate                                                *result,
        int                                                            fieldId,
        const bsl::vector< bdeut_NullableAllocatedValue<bdet_Date> >&  value);
    static int toAggregatePrimitive(
        bcem_Aggregate                                                *result,
        int                                                            fieldId,
        const bsl::vector<
            bdeut_NullableAllocatedValue<bdet_Datetime> >&             value);
    static int toAggregatePrimitive(
        bcem_Aggregate                                                *result,
        int                                                            fieldId,
        const bsl::vector< bdeut_NullableAllocatedValue<bdet_Time> >&  value);
        // Store the specified 'value' into the specified 'result' aggregate in
        // the field selected by the specified 'fieldId'.  Return 0 on success,
        // and a non-zero value otherwise.  Note that any non-zero result can
        // by converted to an 'bsl::string' using the 'errorString' method.

    // PRIVATE CLASS TRAIT-SPECIFIC METHODS

    // Conversion To Aggregates

    template <typename TYPE>
    static int toAggregateImp(bcem_Aggregate                   *destination,
                              int                               fieldId,
                              const TYPE&                       source,
                              bslmf_MetaInt<BCEM_COMPLEX>);
        // Convert of the specified 'source' object into the specified
        // 'fieldId' of the specified 'destination' aggregate.  Return 0 on
        // success and a non-zero value otherwise.

    template <typename TYPE>
    static int toAggregateImp(bcem_Aggregate                   *destination,
                              int                               fieldId,
                              const TYPE&                       source,
                              bslmf_MetaInt<BCEM_ENUMERATION>);
        // Convert of the specified 'source' object into the specified
        // 'fieldId' of the specified 'destination' aggregate.  Return 0 on
        // success and a non-zero value otherwise.

    template <typename TYPE>
    static int toAggregateImp(bcem_Aggregate                   *destination,
                              int                               fieldId,
                              const TYPE&                       source,
                              bslmf_MetaInt<BCEM_PRIMITIVE>);
        // Convert of the specified 'source' object into the specified
        // 'fieldId' of the specified 'destination' aggregate.  Return 0 on
        // success and a non-zero value otherwise.

    template <typename TYPE>
    static int toAggregateImp(bcem_Aggregate                   *destination,
                              int                               fieldId,
                              const bdeut_NullableValue<TYPE>&  source,
                              bslmf_MetaInt<BCEM_COMPLEX>);
        // Convert of the specified 'source' object into the specified
        // 'fieldId' of the specified 'destination' aggregate.  Return 0 on
        // success and a non-zero value otherwise.

    template <typename TYPE>
    static int toAggregateImp(bcem_Aggregate                   *destination,
                              int                               fieldId,
                              const bdeut_NullableValue<TYPE>&   source,
                              bslmf_MetaInt<BCEM_ENUMERATION>);
        // Convert of the specified 'source' object into the specified
        // 'fieldId' of the specified 'destination' aggregate.  Return 0 on
        // success and a non-zero value otherwise.

    template <typename TYPE>
    static int toAggregateImp(bcem_Aggregate                   *destination,
                              int                               fieldId,
                              const bdeut_NullableValue<TYPE>&  source,
                              bslmf_MetaInt<BCEM_PRIMITIVE>);
        // Convert of the specified 'source' object into the specified
        // 'fieldId' of the specified 'destination' aggregate.  Return 0 on
        // success and a non-zero value otherwise.

    template <typename TYPE>
    static int toAggregateImp(
                   bcem_Aggregate                              *destination,
                   int                                          fieldId,
                   const bdeut_NullableAllocatedValue<TYPE>&    source,
                   bslmf_MetaInt<BCEM_COMPLEX>);
        // Convert of the specified 'source' object into the specified
        // 'fieldId' of the specified 'destination' aggregate.  Return 0 on
        // success and a non-zero value otherwise.

    template <typename TYPE>
    static int toAggregateImp(
                   bcem_Aggregate                             *destination,
                   int                                         fieldId,
                   const bdeut_NullableAllocatedValue<TYPE>&   source,
                   bslmf_MetaInt<BCEM_ENUMERATION>);
        // Convert of the specified 'source' object into the specified
        // 'fieldId' of the specified 'destination' aggregate.  Return 0 on
        // success and a non-zero value otherwise.

    template <typename TYPE>
    static int toAggregateImp(
                   bcem_Aggregate                              *destination,
                   int                                          fieldId,
                   const bdeut_NullableAllocatedValue<TYPE>&    source,
                   bslmf_MetaInt<BCEM_PRIMITIVE>);
        // Convert of the specified 'source' object into the specified
        // 'fieldId' of the specified 'destination' aggregate.  Return 0 on
        // success and a non-zero value otherwise.

    template <typename TYPE>
    static int toAggregateImp(bcem_Aggregate                   *destination,
                              int                               fieldId,
                              const bsl::vector<TYPE>&          source,
                              bslmf_MetaInt<BCEM_COMPLEX>);
        // Convert of the specified 'source' object into the specified
        // 'fieldId' of the specified 'destination' aggregate.  Return 0 on
        // success and a non-zero value otherwise.

    template <typename TYPE>
    static int toAggregateImp(bcem_Aggregate                   *destination,
                              int                               fieldId,
                              const bsl::vector<TYPE>&          source,
                              bslmf_MetaInt<BCEM_ENUMERATION>);
        // Convert of the specified 'source' object into the specified
        // 'fieldId' of the specified 'destination' aggregate.  Return 0 on
        // success and a non-zero value otherwise.

    template <typename TYPE>
    static int toAggregateImp(bcem_Aggregate                   *destination,
                              int                               fieldId,
                              const bsl::vector<TYPE>&          source,
                              bslmf_MetaInt<BCEM_PRIMITIVE>);
        // Convert of the specified 'source' object into the specified
        // 'fieldId' of the specified 'destination' aggregate.  Return 0 on
        // success and a non-zero value otherwise.

    template <typename TYPE>
    static
    int toAggregateImp(
            bcem_Aggregate                                        *destination,
            int                                                    fieldId,
            const bsl::vector< bdeut_NullableValue<TYPE> >&        source,
            bslmf_MetaInt<BCEM_COMPLEX>);
        // Convert of the specified 'source' object into the specified
        // 'fieldId' of the specified 'destination' aggregate.  Return 0 on
        // success and a non-zero value otherwise.

    template <typename TYPE>
    static
    int toAggregateImp(
            bcem_Aggregate                                        *destination,
            int                                                    fieldId,
            const bsl::vector< bdeut_NullableValue<TYPE> >&        source,
            bslmf_MetaInt<BCEM_ENUMERATION>);
        // Convert of the specified 'source' object into the specified
        // 'fieldId' of the specified 'destination' aggregate.  Return 0 on
        // success and a non-zero value otherwise.

    template <typename TYPE>
    static
    int toAggregateImp(
            bcem_Aggregate                                        *destination,
            int                                                    fieldId,
            const bsl::vector< bdeut_NullableValue<TYPE> >&        source,
            bslmf_MetaInt<BCEM_PRIMITIVE>);
        // Convert of the specified 'source' object into the specified
        // 'fieldId' of the specified 'destination' aggregate.  Return 0 on
        // success and a non-zero value otherwise.

    template <typename TYPE>
    static
    int toAggregateImp(
            bcem_Aggregate                                        *destination,
            int                                                      fieldId,
            const bsl::vector< bdeut_NullableAllocatedValue<TYPE> >& source,
            bslmf_MetaInt<BCEM_COMPLEX>);
        // Convert of the specified 'source' object into the specified
        // 'fieldId' of the specified 'destination' aggregate.  Return 0 on
        // success and a non-zero value otherwise.

    template <typename TYPE>
    static
    int toAggregateImp(
            bcem_Aggregate                                        *destination,
            int                                                      fieldId,
            const bsl::vector< bdeut_NullableAllocatedValue<TYPE> >& source,
            bslmf_MetaInt<BCEM_ENUMERATION>);
        // Convert of the specified 'source' object into the specified
        // 'fieldId' of the specified 'destination' aggregate.  Return 0 on
        // success and a non-zero value otherwise.

    template <typename TYPE>
    static
    int toAggregateImp(
            bcem_Aggregate                                        *destination,
            int                                                      fieldId,
            const bsl::vector< bdeut_NullableAllocatedValue<TYPE> >& source,
            bslmf_MetaInt<BCEM_PRIMITIVE>);
        // Convert of the specified 'source' object into the specified
        // 'fieldId' of the specified 'destination' aggregate.  Return 0 on
        // success and a non-zero value otherwise.

    // Conversion from Aggregates

    template <typename TYPE>
    static int fromAggregateImp(TYPE                           *destination,
                                const bcem_Aggregate&           source,
                                int                             fieldId,
                                bslmf_MetaInt<BCEM_COMPLEX>);
        // Convert the specified 'fieldId' of the specified 'source' aggregate
        // to the specified 'destination' object.  Return 0 on success and a
        // non-zero value otherwise.

    template <typename TYPE>
    static int fromAggregateImp(TYPE                           *destination,
                                const bcem_Aggregate&           source,
                                int                             fieldId,
                                bslmf_MetaInt<BCEM_ENUMERATION>);
        // Convert the specified 'fieldId' of the specified 'source' aggregate
        // to the specified 'destination' object.  Return 0 on success and a
        // non-zero value otherwise.

    template <typename TYPE>
    static int fromAggregateImp(TYPE                           *destination,
                                const bcem_Aggregate&           source,
                                int                             fieldId,
                                bslmf_MetaInt<BCEM_PRIMITIVE>);
        // Convert the specified 'fieldId' of the specified 'source' aggregate
        // to the specified 'destination' object.  Return 0 on success and a
        // non-zero value otherwise.

    template <typename TYPE>
    static int fromAggregateImp(bdeut_NullableValue<TYPE>      *destination,
                                const bcem_Aggregate&           source,
                                int                             fieldId,
                                bslmf_MetaInt<BCEM_COMPLEX>);
        // Convert the specified 'fieldId' of the specified 'source' aggregate
        // to the specified 'destination' object.  Return 0 on success and a
        // non-zero value otherwise.

    template <typename TYPE>
    static int fromAggregateImp(bdeut_NullableValue<TYPE>      *destination,
                                const bcem_Aggregate&           source,
                                int                             fieldId,
                                bslmf_MetaInt<BCEM_ENUMERATION>);
        // Convert the specified 'fieldId' of the specified 'source' aggregate
        // to the specified 'destination' object.  Return 0 on success and a
        // non-zero value otherwise.

    template <typename TYPE>
    static int fromAggregateImp(bdeut_NullableValue<TYPE>      *destination,
                                const bcem_Aggregate&           source,
                                int                             fieldId,
                                bslmf_MetaInt<BCEM_PRIMITIVE>);
        // Convert the specified 'fieldId' of the specified 'source' aggregate
        // to the specified 'destination' object.  Return 0 on success and a
        // non-zero value otherwise.

    template <typename TYPE>
    static
    int fromAggregateImp(
                   bdeut_NullableAllocatedValue<TYPE>          *destination,
                   const bcem_Aggregate&                        source,
                   int                                          fieldId,
                   bslmf_MetaInt<BCEM_COMPLEX>);
        // Convert the specified 'fieldId' of the specified 'source' aggregate
        // to the specified 'destination' object.  Return 0 on success and a
        // non-zero value otherwise.

    template <typename TYPE>
    static
    int fromAggregateImp(
                   bdeut_NullableAllocatedValue<TYPE>          *destination,
                   const bcem_Aggregate&                        source,
                   int                                          fieldId,
                   bslmf_MetaInt<BCEM_ENUMERATION>);
        // Convert the specified 'fieldId' of the specified 'source' aggregate
        // to the specified 'destination' object.  Return 0 on success and a
        // non-zero value otherwise.

    template <typename TYPE>
    static
    int fromAggregateImp(
                   bdeut_NullableAllocatedValue<TYPE>          *destination,
                   const bcem_Aggregate&                        source,
                   int                                          fieldId,
                   bslmf_MetaInt<BCEM_PRIMITIVE>);
        // Convert the specified 'fieldId' of the specified 'source' aggregate
        // to the specified 'destination' object.  Return 0 on success and a
        // non-zero value otherwise.

    template <typename TYPE>
    static
    int fromAggregateImp(
            bsl::vector<TYPE>                                     *destination,
            const bcem_Aggregate&                                  source,
            int                                                    fieldId,
            bslmf_MetaInt<BCEM_COMPLEX>);
        // Convert the specified 'fieldId' of the specified 'source' aggregate
        // to the specified 'destination' object.  Return 0 on success and a
        // non-zero value otherwise.

    template <typename TYPE>
    static
    int fromAggregateImp(
            bsl::vector<TYPE>                                     *destination,
            const bcem_Aggregate&                                  source,
            int                                                    fieldId,
            bslmf_MetaInt<BCEM_ENUMERATION>);
        // Convert the specified 'fieldId' of the specified 'source' aggregate
        // to the specified 'destination' object.  Return 0 on success and a
        // non-zero value otherwise.

    template <typename TYPE>
    static
    int fromAggregateImp(
            bsl::vector<TYPE>                                     *destination,
            const bcem_Aggregate&                                  source,
            int                                                    fieldId,
            bslmf_MetaInt<BCEM_PRIMITIVE>);
        // Convert the specified 'fieldId' of the specified 'source' aggregate
        // to the specified 'destination' object.  Return 0 on success and a
        // non-zero value otherwise.

    template <typename TYPE>
    static
    int fromAggregateImp(
            bsl::vector< bdeut_NullableValue<TYPE> >              *destination,
            const bcem_Aggregate&                                  source,
            int                                                    fieldId,
            bslmf_MetaInt<BCEM_COMPLEX>);
        // Convert the specified 'fieldId' of the specified 'source' aggregate
        // to the specified 'destination' object.  Return 0 on success and a
        // non-zero value otherwise.

    template <typename TYPE>
    static
    int fromAggregateImp(
            bsl::vector< bdeut_NullableValue<TYPE> >              *destination,
            const bcem_Aggregate&                                  source,
            int                                                    fieldId,
            bslmf_MetaInt<BCEM_ENUMERATION>);
        // Convert the specified 'fieldId' of the specified 'source' aggregate
        // to the specified 'destination' object.  Return 0 on success and a
        // non-zero value otherwise.

    template <typename TYPE>
    static
    int fromAggregateImp(
            bsl::vector< bdeut_NullableValue<TYPE> >              *destination,
            const bcem_Aggregate&                                  source,
            int                                                    fieldId,
            bslmf_MetaInt<BCEM_PRIMITIVE>);
        // Convert the specified 'fieldId' of the specified 'source' aggregate
        // to the specified 'destination' object.  Return 0 on success and a
        // non-zero value otherwise.

    template <typename TYPE>
    static
    int fromAggregateImp(
            bsl::vector< bdeut_NullableAllocatedValue<TYPE> >     *destination,
            const bcem_Aggregate&                                  source,
            int                                                    fieldId,
            bslmf_MetaInt<BCEM_COMPLEX>);
        // Convert the specified 'fieldId' of the specified 'source' aggregate
        // to the specified 'destination' object.  Return 0 on success and a
        // non-zero value otherwise.

    template <typename TYPE>
    static
    int fromAggregateImp(
            bsl::vector< bdeut_NullableAllocatedValue<TYPE> >     *destination,
            const bcem_Aggregate&                                  source,
            int                                                    fieldId,
            bslmf_MetaInt<BCEM_ENUMERATION>);
        // Convert the specified 'fieldId' of the specified 'source' aggregate
        // to the specified 'destination' object.  Return 0 on success and a
        // non-zero value otherwise.

    template <typename TYPE>
    static
    int fromAggregateImp(
            bsl::vector< bdeut_NullableAllocatedValue<TYPE> >     *destination,
            const bcem_Aggregate&                                  source,
            int                                                    fieldId,
            bslmf_MetaInt<BCEM_PRIMITIVE>);
        // Convert the specified 'fieldId' of the specified 'source' aggregate
        // to the specified 'destination' object.  Return 0 on success and a
        // non-zero value otherwise.

  public:
    // CLASS METHODS

    template <typename TYPE>
    static
    int toAggregate(
               bcem_Aggregate                                     *destination,
               int                                                 fieldId,
               const TYPE&                                         source);

    template <typename TYPE>
    static
    int toAggregate(
               bcem_Aggregate                                     *destination,
               int                                                 fieldId,
               const bdeut_NullableValue<TYPE>&                    source);

    template <typename TYPE>
    static
    int toAggregate(
               bcem_Aggregate                                     *destination,
               int                                                 fieldId,
               const bdeut_NullableAllocatedValue<TYPE>&           source);

    template <typename TYPE>
    static
    int toAggregate(
               bcem_Aggregate                                     *destination,
               int                                                 fieldId,
               const bsl::vector<TYPE>&                            source);

    template <typename TYPE>
    static
    int toAggregate(
               bcem_Aggregate                                     *destination,
               int                                                 fieldId,
               const bsl::vector< bdeut_NullableValue<TYPE> >&     source);

    template <typename TYPE>
    static
    int toAggregate(
               bcem_Aggregate                                     *destination,
               int                                                 fieldId,
               const bsl::vector< bdeut_NullableAllocatedValue<TYPE> >&
                                                                   source);
        // Convert the specified 'source' object to the specified 'fieldId' of
        // the specified 'destination' aggregate.  Return 0 on success and a
        // non-zero value otherwise.

    template <typename TYPE>
    static
    int fromAggregate(
               TYPE                                               *destination,
               const bcem_Aggregate&                               source,
               int                                                 fieldId);

    template <typename TYPE>
    static
    int fromAggregate(
               bdeut_NullableValue<TYPE>                          *destination,
               const bcem_Aggregate&                               source,
               int                                                 fieldId);

    template <typename TYPE>
    static
    int fromAggregate(
               bdeut_NullableAllocatedValue<TYPE>                 *destination,
               const bcem_Aggregate&                               source,
               int                                                 fieldId);

    template <typename TYPE>
    static
    int fromAggregate(
               bsl::vector<TYPE>                                  *destination,
               const bcem_Aggregate&                               source,
               int                                                 fieldId);

    template <typename TYPE>
    static
    int fromAggregate(
               bsl::vector< bdeut_NullableValue<TYPE> >           *destination,
               const bcem_Aggregate&                               source,
               int                                                 fieldId);

    template <typename TYPE>
    static
    int fromAggregate(
               bsl::vector< bdeut_NullableAllocatedValue<TYPE> >  *destination,
               const bcem_Aggregate&                               source,
               int                                                 fieldId);
        // Convert the specified 'fieldId' of the specified 'source' aggregate
        // to the specified 'destination' object.   Return 0 on success and a
        // non-zero value otherwise.

    static const char *errorString(int errorCode);
       // Return the error string corresponding to the specified 'errorCode',
       // where 'errorCode' is a value returned from any public class method
       // defined in this utility.
};

// ============================================================================
//                        INLINE FUNCTION DEFINITIONS
// ============================================================================

// PRIVATE CLASS TRAIT-SPECIFIC METHODS

// Conversion To Aggregates

template <typename TYPE>
inline
int bcem_AggregateUtil::toAggregateImp(bcem_Aggregate *destination,
                                       int             fieldId,
                                       const TYPE&     source,
                                       bslmf_MetaInt<BCEM_COMPLEX>)
{
    bcem_Aggregate field = destination->fieldById(fieldId);
    if (field.isError()) {
        return field.errorCode();
    }

    return source.toAggregate(&field);
}

template <typename TYPE>
int bcem_AggregateUtil::toAggregateImp(bcem_Aggregate *destination,
                                       int             fieldId,
                                       const TYPE&     source,
                                       bslmf_MetaInt<BCEM_ENUMERATION>)
{
    typedef typename bslalg_TypeTraits<TYPE>::Wrapper Wrapper;

    bcem_Aggregate field = destination->fieldById(fieldId);
    if (field.isError()) {
        return field.errorCode();
    }

    return setValue(field, Wrapper::toString(source));
}

template <typename TYPE>
inline
int bcem_AggregateUtil::toAggregateImp(bcem_Aggregate *destination,
                                       int             fieldId,
                                       const TYPE&     source,
                                       bslmf_MetaInt<BCEM_PRIMITIVE>)
{
    return bcem_AggregateUtil::toAggregatePrimitive(destination,
                                                    fieldId,
                                                    source);
}

template <typename TYPE>
int bcem_AggregateUtil::toAggregateImp(
        bcem_Aggregate                  *destination,
        int                              fieldId,
        const bdeut_NullableValue<TYPE>& source,
        bslmf_MetaInt<BCEM_COMPLEX>)
{
    bcem_Aggregate field = destination->fieldById(fieldId);
    if (field.isError()) {
        return field.errorCode();
    }

    if (source.isNull()) {
        field.makeNull();
        return field.errorCode();
    }

    bcem_Aggregate nonNullField = field.makeValue();
    return source.value().toAggregate(&nonNullField);
}

template <typename TYPE>
int bcem_AggregateUtil::toAggregateImp(
        bcem_Aggregate                  *destination,
        int                              fieldId,
        const bdeut_NullableValue<TYPE>& source,
        bslmf_MetaInt<BCEM_ENUMERATION>)
{
    typedef typename bslalg_TypeTraits<TYPE>::Wrapper Wrapper;

    bcem_Aggregate field = destination->fieldById(fieldId);
    if (field.isError()) {
        return field.errorCode();
    }

    if (source.isNull()) {
        field.makeNull();
        return field.errorCode();
    }

    return setValue(field, Wrapper::toString(source.value()));
}

template <typename TYPE>
inline
int bcem_AggregateUtil::toAggregateImp(
        bcem_Aggregate                  *destination,
        int                              fieldId,
        const bdeut_NullableValue<TYPE>& source,
        bslmf_MetaInt<BCEM_PRIMITIVE>)
{
    return bcem_AggregateUtil::toAggregatePrimitive(destination,
                                                    fieldId,
                                                    source);
}

template <typename TYPE>
int bcem_AggregateUtil::toAggregateImp(
        bcem_Aggregate                            *destination,
        int                                        fieldId,
        const bdeut_NullableAllocatedValue<TYPE>&  source,
        bslmf_MetaInt<BCEM_COMPLEX>)
{
    bcem_Aggregate field = destination->fieldById(fieldId);
    if (field.isError()) {
        return field.errorCode();
    }

    if (source.isNull()) {
        field.makeNull();
        return field.errorCode();
    }

    bcem_Aggregate nonNullField = field.makeValue();
    return source.value().toAggregate(&nonNullField);
}

template <typename TYPE>
int bcem_AggregateUtil::toAggregateImp(
        bcem_Aggregate                            *destination,
        int                                        fieldId,
        const bdeut_NullableAllocatedValue<TYPE>&  source,
        bslmf_MetaInt<BCEM_ENUMERATION>)
{
    typedef typename bslalg_TypeTraits<TYPE>::Wrapper Wrapper;

    bcem_Aggregate field = destination->fieldById(fieldId);
    if (field.isError()) {
        return field.errorCode();
    }

    if (source.isNull()) {
        field.makeNull();
        return field.errorCode();
    }

    return setValue(field, Wrapper::toString(source.value()));
}

template <typename TYPE>
inline
int bcem_AggregateUtil::toAggregateImp(
        bcem_Aggregate                            *destination,
        int                                        fieldId,
        const bdeut_NullableAllocatedValue<TYPE>&  source,
        bslmf_MetaInt<BCEM_PRIMITIVE>)
{
    return bcem_AggregateUtil::toAggregatePrimitive(destination,
                                                    fieldId,
                                                    source);
}

template <typename TYPE>
int bcem_AggregateUtil::toAggregateImp(
        bcem_Aggregate           *destination,
        int                       fieldId,
        const bsl::vector<TYPE>&  source,
        bslmf_MetaInt<BCEM_COMPLEX>)
{
    bsl::size_t    size  = source.size();
    bcem_Aggregate field = destination->fieldById(fieldId);
    int            rc    = 0;

    field.removeAllItems();
    field.resize(size);

    for (bsl::size_t i = 0; i < size; ++i) {
        bcem_Aggregate fieldElement = field[i];
        if (0 != (rc = source[i].toAggregate(&fieldElement))) {
            return rc;
        }
    }

    return 0;
}

template <typename TYPE>
int bcem_AggregateUtil::toAggregateImp(
        bcem_Aggregate           *destination,
        int                       fieldId,
        const bsl::vector<TYPE>&  source,
        bslmf_MetaInt<BCEM_ENUMERATION>)
{
    typedef typename bslalg_TypeTraits<TYPE>::Wrapper Wrapper;

    bsl::size_t    size  = source.size();
    bcem_Aggregate field = destination->fieldById(fieldId);

    field.removeAllItems();
    field.resize(size);

    for (bsl::size_t i = 0; i < size; ++i) {
        bcem_Aggregate fieldElement = field[i];
        int rc = setValue(fieldElement, Wrapper::toString(source[i]));
        if (0 != rc) {
            return rc;
        }
    }

    return 0;
}

template <typename TYPE>
inline
int bcem_AggregateUtil::toAggregateImp(
        bcem_Aggregate           *destination,
        int                       fieldId,
        const bsl::vector<TYPE>&  source,
        bslmf_MetaInt<BCEM_PRIMITIVE>)
{
    return bcem_AggregateUtil::toAggregatePrimitive(destination,
                                                    fieldId,
                                                    source);
}

template <typename TYPE>
int bcem_AggregateUtil::toAggregateImp(
        bcem_Aggregate                                  *destination,
        int                                              fieldId,
        const bsl::vector <bdeut_NullableValue<TYPE> >&  source,
        bslmf_MetaInt<BCEM_COMPLEX>)
{
    bsl::size_t    size  = source.size();
    bcem_Aggregate field = destination->fieldById(fieldId);
    int            rc    = 0;

    field.removeAllItems();
    field.resize(size);

    for (bsl::size_t i = 0; i < size; ++i) {
        bcem_Aggregate fieldElement = field[i];
        if (source[i].isNull()) {
            fieldElement.makeNull();
        }
        else {
            bcem_Aggregate nonNullField = fieldElement.makeValue();
            if (0 != (rc = source[i].value().toAggregate(&nonNullField))) {
                return rc;
            }
        }
    }

    return 0;
}

template <typename TYPE>
int bcem_AggregateUtil::toAggregateImp(
        bcem_Aggregate                                  *destination,
        int                                              fieldId,
        const bsl::vector< bdeut_NullableValue<TYPE> >&  source,
        bslmf_MetaInt<BCEM_ENUMERATION>)
{
    typedef typename bslalg_TypeTraits<TYPE>::Wrapper Wrapper;

    bsl::size_t    size  = source.size();
    bcem_Aggregate field = destination->fieldById(fieldId);

    field.removeAllItems();
    field.resize(size);

    for (bsl::size_t i = 0; i < size; ++i) {
        bcem_Aggregate fieldElement = field[i];
        if (source[i].isNull()) {
            fieldElement.makeNull();
        }
        else {
            bcem_Aggregate nonNullField = fieldElement.makeValue();
            int rc = setValue(nonNullField,
                              Wrapper::toString(source[i].value()));
            if (0 != rc) {
                return rc;
            }
        }
    }

    return 0;
}

template <typename TYPE>
int bcem_AggregateUtil::toAggregateImp(
        bcem_Aggregate                                  *destination,
        int                                              fieldId,
        const bsl::vector< bdeut_NullableValue<TYPE> >&  source,
        bslmf_MetaInt<BCEM_PRIMITIVE>)
{
    return bcem_AggregateUtil::toAggregatePrimitive(destination,
                                                    fieldId,
                                                    source);
}

template <typename TYPE>
int bcem_AggregateUtil::toAggregateImp(
        bcem_Aggregate                                           *destination,
        int                                                       fieldId,
        const bsl::vector<bdeut_NullableAllocatedValue<TYPE> >&  source,
        bslmf_MetaInt<BCEM_COMPLEX>)
{
    bsl::size_t    size  = source.size();
    bcem_Aggregate field = destination->fieldById(fieldId);
    int            rc    = 0;

    field.removeAllItems();
    field.resize(size);

    for (bsl::size_t i = 0; i < size; ++i) {
        bcem_Aggregate fieldElement = field[i];
        if (source[i].isNull()) {
            fieldElement.makeNull();
        }
        else {
            bcem_Aggregate nonNullField = fieldElement.makeValue();
            if (0 != (rc = source[i].value().toAggregate(&nonNullField))) {
                return rc;
            }
        }
    }

    return 0;
}

template <typename TYPE>
int bcem_AggregateUtil::toAggregateImp(
        bcem_Aggregate                                           *destination,
        int                                                       fieldId,
        const bsl::vector< bdeut_NullableAllocatedValue<TYPE> >&  source,
        bslmf_MetaInt<BCEM_ENUMERATION>)
{
    typedef typename bslalg_TypeTraits<TYPE>::Wrapper Wrapper;

    bsl::size_t    size  = source.size();
    bcem_Aggregate field = destination->fieldById(fieldId);

    field.removeAllItems();
    field.resize(size);

    for (bsl::size_t i = 0; i < size; ++i) {
        bcem_Aggregate fieldElement = field[i];
        if (source[i].isNull()) {
            fieldElement.makeNull();
        }
        else {
            bcem_Aggregate nonNullField = fieldElement.makeValue();
            int rc = setValue(nonNullField,
                              Wrapper::toString(source[i].value()));
            if (0 != rc) {
                return rc;
            }
        }
    }

    return 0;
}

template <typename TYPE>
int bcem_AggregateUtil::toAggregateImp(
        bcem_Aggregate                                           *destination,
        int                                                       fieldId,
        const bsl::vector< bdeut_NullableAllocatedValue<TYPE> >&  source,
        bslmf_MetaInt<BCEM_PRIMITIVE>)
{
    return bcem_AggregateUtil::toAggregatePrimitive(destination,
                                                    fieldId,
                                                    source);
}

// Conversion from Aggregates

template <typename TYPE>
inline
int bcem_AggregateUtil::fromAggregateImp(TYPE                  *destination,
                                         const bcem_Aggregate&  source,
                                         int                    fieldId,
                                         bslmf_MetaInt<BCEM_COMPLEX>)
{
    bcem_Aggregate field = source.fieldById(fieldId);
    if (field.isError()) {
        return field.errorCode();
    }

    return destination->fromAggregate(field);
}

template <typename TYPE>
int bcem_AggregateUtil::fromAggregateImp(TYPE                  *destination,
                                         const bcem_Aggregate&  source,
                                         int                    fieldId,
                                         bslmf_MetaInt<BCEM_ENUMERATION>)
{
    typedef typename bslalg_TypeTraits<TYPE>::Wrapper Wrapper;

    bcem_Aggregate field = source.fieldById(fieldId);
    if (field.isError()) {
        return field.errorCode();
    }

    if (0 != Wrapper::fromString(destination, field.asString())) {
        return bcem_Aggregate::BCEM_ERR_BAD_ENUMVALUE;
    }

    return 0;
}

template <typename TYPE>
inline
int bcem_AggregateUtil::fromAggregateImp(TYPE                  *destination,
                                         const bcem_Aggregate&  source,
                                         int                    fieldId,
                                         bslmf_MetaInt<BCEM_PRIMITIVE>)
{
    return bcem_AggregateUtil::fromAggregatePrimitive(destination,
                                                      source,
                                                      fieldId);
}

template <typename TYPE>
int bcem_AggregateUtil::fromAggregateImp(
        bdeut_NullableValue<TYPE> *destination,
        const bcem_Aggregate&      source,
        int                        fieldId,
        bslmf_MetaInt<BCEM_COMPLEX>)
{
    bcem_Aggregate field = source.fieldById(fieldId);
    int            rc;

    if (field.isError()) {
        return field.errorCode();
    }

    if (field.isNul2()) {
        destination->reset();
    }
    else if (0 != (rc = destination->makeValue().fromAggregate(field))) {
        return rc;
    }

    return 0;
}

template <typename TYPE>
int bcem_AggregateUtil::fromAggregateImp(
        bdeut_NullableValue<TYPE> *destination,
        const bcem_Aggregate&      source,
        int                        fieldId,
        bslmf_MetaInt<BCEM_ENUMERATION>)
{
    typedef typename bslalg_TypeTraits<TYPE>::Wrapper Wrapper;

    bcem_Aggregate field = source.fieldById(fieldId);
    if (field.isError()) {
        return field.errorCode();
    }

    if (field.isNul2()) {
        destination->reset();
    }
    else if (0 != (Wrapper::fromString(&destination->makeValue(),
                                       field.asString())))
    {
        return bcem_Aggregate::BCEM_ERR_BAD_ENUMVALUE;
    }

    return 0;
}

template <typename TYPE>
inline
int bcem_AggregateUtil::fromAggregateImp(
        bdeut_NullableValue<TYPE> *destination,
        const bcem_Aggregate&      source,
        int                        fieldId,
        bslmf_MetaInt<BCEM_PRIMITIVE>)
{
    return bcem_AggregateUtil::fromAggregatePrimitive(destination,
                                                      source,
                                                      fieldId);
}

template <typename TYPE>
int bcem_AggregateUtil::fromAggregateImp(
        bdeut_NullableAllocatedValue<TYPE> *destination,
        const bcem_Aggregate&               source,
        int                                 fieldId,
        bslmf_MetaInt<BCEM_COMPLEX>)
{
    bcem_Aggregate field = source.fieldById(fieldId);
    int            rc;

    if (field.isError()) {
        return field.errorCode();
    }

    if (field.isNul2()) {
        destination->reset();
    }
    else if (0 != (rc = destination->makeValue().fromAggregate(field))) {
        return rc;
    }

    return 0;
}

template <typename TYPE>
int bcem_AggregateUtil::fromAggregateImp(
        bdeut_NullableAllocatedValue<TYPE> *destination,
        const bcem_Aggregate&               source,
        int                                 fieldId,
        bslmf_MetaInt<BCEM_ENUMERATION>)
{
    typedef typename bslalg_TypeTraits<TYPE>::Wrapper Wrapper;

    bcem_Aggregate field = source.fieldById(fieldId);
    if (field.isError()) {
        return field.errorCode();
    }

    if (field.isNul2()) {
        destination->reset();
    }
    else if (0 != (Wrapper::fromString(&destination->makeValue(),
                                       field.asString())))
    {
        return bcem_Aggregate::BCEM_ERR_BAD_ENUMVALUE;
    }

    return 0;
}

template <typename TYPE>
inline
int bcem_AggregateUtil::fromAggregateImp(
        bdeut_NullableAllocatedValue<TYPE> *destination,
        const bcem_Aggregate&               source,
        int                                 fieldId,
        bslmf_MetaInt<BCEM_PRIMITIVE>)
{
    return bcem_AggregateUtil::fromAggregatePrimitive(destination,
                                                      source,
                                                      fieldId);
}

template <typename TYPE>
int bcem_AggregateUtil::fromAggregateImp(
        bsl::vector<TYPE>     *destination,
        const bcem_Aggregate&  source,
        int                    fieldId,
        bslmf_MetaInt<BCEM_COMPLEX>)
{
    bcem_Aggregate field = source.fieldById(fieldId);
    int            rc    = 0;

    if (field.isError()) {
        return field.errorCode();
    }

    int size = field.length();

    destination->clear();
    destination->resize(size);

    for (int i = 0; i < size; ++i)
    {
        if (0 != (rc = (*destination)[i].fromAggregate(field[i]))) {
            return rc;
        }
    }

    return 0;
}

template <typename TYPE>
int bcem_AggregateUtil::fromAggregateImp(
        bsl::vector<TYPE>     *destination,
        const bcem_Aggregate&  source,
        int                    fieldId,
        bslmf_MetaInt<BCEM_ENUMERATION>)
{
    typedef typename bslalg_TypeTraits<TYPE>::Wrapper Wrapper;

    bcem_Aggregate field = source.fieldById(fieldId);
    int            rc    = 0;

    if (field.isError()) {
        return field.errorCode();
    }

    int size = field.length();

    destination->clear();
    destination->resize(size);

    for (int i = 0; i < size; ++i)
    {
        if (0 != (Wrapper::fromString(&(*destination)[i],
                                      field[i].asString())))
        {
            return bcem_Aggregate::BCEM_ERR_BAD_ENUMVALUE;
        }
    }

    return 0;
}

template <typename TYPE>
inline
int bcem_AggregateUtil::fromAggregateImp(
        bsl::vector<TYPE>     *destination,
        const bcem_Aggregate&  source,
        int                    fieldId,
        bslmf_MetaInt<BCEM_PRIMITIVE>)
{
    return bcem_AggregateUtil::fromAggregatePrimitive(destination,
                                                      source,
                                                      fieldId);
}

template <typename TYPE>
int bcem_AggregateUtil::fromAggregateImp(
        bsl::vector< bdeut_NullableValue<TYPE> > *destination,
        const bcem_Aggregate&                     source,
        int                                       fieldId,
        bslmf_MetaInt<BCEM_COMPLEX>)
{
    bcem_Aggregate field = source.fieldById(fieldId);
    int            rc    = 0;

    if (field.isError()) {
        return field.errorCode();
    }

    int size = field.length();

    destination->clear();
    destination->resize(size);

    for (int i = 0; i < size; ++i) {
        bcem_Aggregate fieldElement = field[i];
        if (fieldElement.isNul2()) {
            (*destination)[i].reset();
        }
        else {
            rc = (*destination)[i].makeValue().fromAggregate(fieldElement);
            if (0 != rc) {
                return rc;
            }
        }
    }

    return 0;
}

template <typename TYPE>
int bcem_AggregateUtil::fromAggregateImp(
        bsl::vector< bdeut_NullableValue<TYPE> > *destination,
        const bcem_Aggregate&                     source,
        int                                       fieldId,
        bslmf_MetaInt<BCEM_ENUMERATION>)
{
    typedef typename bslalg_TypeTraits<TYPE>::Wrapper Wrapper;

    bcem_Aggregate field = source.fieldById(fieldId);
    int            rc    = 0;

    if (field.isError()) {
        return field.errorCode();
    }

    int size = field.length();

    destination->clear();
    destination->resize(size);

    for (int i = 0; i < size; ++i)
    {
        bcem_Aggregate fieldElement = field[i];
        if (fieldElement.isNul2()) {
            (*destination)[i].reset();
        }
        else if (0 != (Wrapper::fromString(&(*destination)[i].makeValue(),
                                           fieldElement.asString())))
        {
            return bcem_Aggregate::BCEM_ERR_BAD_ENUMVALUE;
        }
    }

    return 0;
}

template <typename TYPE>
int bcem_AggregateUtil::fromAggregateImp(
        bsl::vector< bdeut_NullableValue<TYPE> > *destination,
        const bcem_Aggregate&                     source,
        int                                       fieldId,
        bslmf_MetaInt<BCEM_PRIMITIVE>)
{
    return bcem_AggregateUtil::fromAggregatePrimitive(destination,
                                                      source,
                                                      fieldId);
}

template <typename TYPE>
int bcem_AggregateUtil::fromAggregateImp(
        bsl::vector< bdeut_NullableAllocatedValue<TYPE> > *destination,
        const bcem_Aggregate&                              source,
        int                                                fieldId,
        bslmf_MetaInt<BCEM_COMPLEX>)
{
    bcem_Aggregate field = source.fieldById(fieldId);
    int            rc    = 0;

    if (field.isError()) {
        return field.errorCode();
    }

    int size = field.length();

    destination->clear();
    destination->resize(size);

    for (int i = 0; i < size; ++i) {
        bcem_Aggregate fieldElement = field[i];
        if (fieldElement.isNul2()) {
            (*destination)[i].reset();
        }
        else {
            rc = (*destination)[i].makeValue().fromAggregate(fieldElement);
            if (0 != rc) {
                return rc;
            }
        }
    }

    return 0;
}

template <typename TYPE>
int bcem_AggregateUtil::fromAggregateImp(
        bsl::vector< bdeut_NullableAllocatedValue<TYPE> > *destination,
        const bcem_Aggregate&                              source,
        int                                                fieldId,
        bslmf_MetaInt<BCEM_ENUMERATION>)
{
    typedef typename bslalg_TypeTraits<TYPE>::Wrapper Wrapper;

    bcem_Aggregate field = source.fieldById(fieldId);
    int            rc    = 0;

    if (field.isError()) {
        return field.errorCode();
    }

    int size = field.length();

    destination->clear();
    destination->resize(size);

    for (int i = 0; i < size; ++i)
    {
        bcem_Aggregate fieldElement = field[i];
        if (fieldElement.isNul2()) {
            (*destination)[i].reset();
        }
        else if (0 != (Wrapper::fromString(&(*destination)[i].makeValue(),
                                           fieldElement.asString())))
        {
            return bcem_Aggregate::BCEM_ERR_BAD_ENUMVALUE;
        }
    }

    return 0;
}

template <typename TYPE>
int bcem_AggregateUtil::fromAggregateImp(
        bsl::vector< bdeut_NullableAllocatedValue<TYPE> > *destination,
        const bcem_Aggregate&                              source,
        int                                                fieldId,
        bslmf_MetaInt<BCEM_PRIMITIVE>)
{
    return bcem_AggregateUtil::fromAggregatePrimitive(destination,
                                                      source,
                                                      fieldId);
}

// CLASS METHODS

template <typename TYPE>
inline int
bcem_AggregateUtil::toAggregate(
        bcem_Aggregate *destination,
        int             fieldId,
        const TYPE&     source)
{
    typedef typename TypeCategoryOf<TYPE>::Result TypeTag;
    return toAggregateImp(destination, fieldId, source, TypeTag());
}

template <typename TYPE>
inline int
bcem_AggregateUtil::toAggregate(
        bcem_Aggregate                   *destination,
        int                               fieldId,
        const bdeut_NullableValue<TYPE>&  source)
{
    typedef typename TypeCategoryOf<TYPE>::Result TypeTag;
    return toAggregateImp(destination, fieldId, source, TypeTag());
}

template <typename TYPE>
inline int
bcem_AggregateUtil::toAggregate(
        bcem_Aggregate                            *destination,
        int                                        fieldId,
        const bdeut_NullableAllocatedValue<TYPE>&  source)
{
    typedef typename TypeCategoryOf<TYPE>::Result TypeTag;
    return toAggregateImp(destination, fieldId, source, TypeTag());
}

template <typename TYPE>
inline int
bcem_AggregateUtil::toAggregate(
        bcem_Aggregate           *destination,
        int                       fieldId,
        const bsl::vector<TYPE>&  source)
{
    typedef typename TypeCategoryOf<TYPE>::Result TypeTag;
    return toAggregateImp(destination, fieldId, source, TypeTag());
}

template <typename TYPE>
inline int
bcem_AggregateUtil::toAggregate(
        bcem_Aggregate                                  *destination,
        int                                              fieldId,
        const bsl::vector< bdeut_NullableValue<TYPE> >&  source)
{
    typedef typename TypeCategoryOf<TYPE>::Result TypeTag;
    return toAggregateImp(destination, fieldId, source, TypeTag());
}

template <typename TYPE>
inline int
bcem_AggregateUtil::toAggregate(
        bcem_Aggregate                                           *destination,
        int                                                       fieldId,
        const bsl::vector< bdeut_NullableAllocatedValue<TYPE> >&  source)
{
    typedef typename TypeCategoryOf<TYPE>::Result TypeTag;
    return toAggregateImp(destination, fieldId, source, TypeTag());
}

template <typename TYPE>
inline int
bcem_AggregateUtil::fromAggregate(
        TYPE                  *destination,
        const bcem_Aggregate&  source,
        int                    fieldId)
{
    typedef typename TypeCategoryOf<TYPE>::Result TypeTag;
    return fromAggregateImp(destination, source, fieldId, TypeTag());
}

template <typename TYPE>
inline int
bcem_AggregateUtil::fromAggregate(
        bdeut_NullableValue<TYPE> *destination,
        const bcem_Aggregate&      source,
        int                        fieldId)
{
    typedef typename TypeCategoryOf<TYPE>::Result TypeTag;
    return fromAggregateImp(destination, source, fieldId, TypeTag());
}

template <typename TYPE>
inline int
bcem_AggregateUtil::fromAggregate(
        bdeut_NullableAllocatedValue<TYPE> *destination,
        const bcem_Aggregate&               source,
        int                                 fieldId)
{
    typedef typename TypeCategoryOf<TYPE>::Result TypeTag;
    return fromAggregateImp(destination, source, fieldId, TypeTag());
}

template <typename TYPE>
inline int
bcem_AggregateUtil::fromAggregate(
        bsl::vector<TYPE>     *destination,
        const bcem_Aggregate&  source,
        int                    fieldId)
{
    typedef typename TypeCategoryOf<TYPE>::Result TypeTag;
    return fromAggregateImp(destination, source, fieldId, TypeTag());
}

template <typename TYPE>
inline int
bcem_AggregateUtil::fromAggregate(
        bsl::vector< bdeut_NullableValue<TYPE> > *destination,
        const bcem_Aggregate&                     source,
        int                                       fieldId)
{
    typedef typename TypeCategoryOf<TYPE>::Result TypeTag;
    return fromAggregateImp(destination, source, fieldId, TypeTag());
}

template <typename TYPE>
inline int
bcem_AggregateUtil::fromAggregate(
        bsl::vector< bdeut_NullableAllocatedValue<TYPE> > *destination,
        const bcem_Aggregate&                              source,
        int                                                fieldId)
{
    typedef typename TypeCategoryOf<TYPE>::Result TypeTag;
    return fromAggregateImp(destination, source, fieldId, TypeTag());
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
