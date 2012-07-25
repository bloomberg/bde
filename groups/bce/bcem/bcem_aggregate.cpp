// bcem_aggregate.cpp                                                 -*-C++-*-
#include <bcem_aggregate.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bcem_aggregate_cpp,"$Id$ $CSID$")

#include <bdem_choicearrayitem.h>
#include <bdem_descriptor.h>
#include <bdem_elemattrlookup.h>
#include <bdem_schemaaggregateutil.h>
#include <bdem_schemautil.h>

#include <bslma_default.h>

#include <bsls_platform.h>
#include <bsls_types.h>

#include <bdeu_print.h>

#include <bsls_assert.h>
#include <bsl_sstream.h>

#ifdef TEST
// These dependencies will cause the test driver to recompile when the BER
// encoder or decoder change.
#include <bdem_berencoder.h>
#include <bdem_berdecoder.h>
#endif

#include <stdio.h>     // 'v/snprintf' (not always declared in <bsl_cstdio.h>).

#include <bsl_cstdarg.h>
#include <bsl_cstdlib.h>         // 'strtol'
#include <bsl_cstring.h>         // 'memcpy'
#include <bsl_algorithm.h>      // 'swap'
#include <bsl_iterator.h>

namespace BloombergLP {

namespace {

// TYPES
typedef bsls_Types::Int64            Int64;
typedef bcem_AggregateRawNameOrIndex NameOrIndex;

// HELPER FUNCTIONS
template <typename DATATYPE>
inline
bcema_SharedPtr<DATATYPE> makeValuePtrInplace(bslma_Allocator *basicAllocator)
    // Return a shared pointer to an object of type 'DATATYPE' using the
    // "in-place" construction facility of 'bcema_SharedPtr' to
    // default-construct 'DATATYPE'.  The allocator is used to create the
    // shared pointer, but is not passed to 'DATATYPE's constructor.
{
    bcema_SharedPtr<DATATYPE> result;
    result.createInplace(basicAllocator);
    return result;
}

template <typename DATATYPE>
inline
bcema_SharedPtr<DATATYPE>
makeValuePtrInplaceWithAlloc(bslma_Allocator *basicAllocator)
    // Return a shared pointer to an object of type 'DATATYPE' using the
    // "in-place" construction facility of 'bcema_SharedPtr' to
    // default-construct 'DATATYPE'.  The allocator is used to create the
    // shared pointer, and is also passed to 'DATATYPE's constructor.
{
    bcema_SharedPtr<DATATYPE> result;
    result.createInplace(basicAllocator, basicAllocator);
    return result;
}

const char *recordName(const bdem_RecordDef *recordDef)
{
    if (! recordDef) {
        return "(unconstrained)";                                     // RETURN
    }

    const char *ret = recordDef->schema().recordName(recordDef->recordIndex());
    return ret ? ret : "(anonymous)";
}

}  // close unnamed namespace

bcema_SharedPtr<void>
bcem_Aggregate::makeValuePtr(bdem_ElemType::Type  type,
                             bslma_Allocator     *basicAllocator)
{
    bslma_Allocator *allocator = bslma_Default::allocator(basicAllocator);

    switch (type) {
      case bdem_ElemType::BDEM_CHAR: {
        return makeValuePtrInplace<char>(allocator);
      }
      case bdem_ElemType::BDEM_SHORT: {
        return makeValuePtrInplace<short>(allocator);
      }
      case bdem_ElemType::BDEM_INT: {
        return makeValuePtrInplace<int>(allocator);
      }
      case bdem_ElemType::BDEM_INT64: {
        return makeValuePtrInplace<Int64>(allocator);
      }
      case bdem_ElemType::BDEM_FLOAT: {
        return makeValuePtrInplace<float>(allocator);
      }
      case bdem_ElemType::BDEM_DOUBLE: {
        return makeValuePtrInplace<double>(allocator);
      }
      case bdem_ElemType::BDEM_STRING: {
        return makeValuePtrInplaceWithAlloc<bsl::string>(allocator);
      }
      case bdem_ElemType::BDEM_DATETIME: {
        return makeValuePtrInplace<bdet_Datetime>(allocator);
      }
      case bdem_ElemType::BDEM_DATE: {
        return makeValuePtrInplace<bdet_Date>(allocator);
      }
      case bdem_ElemType::BDEM_TIME: {
        return makeValuePtrInplace<bdet_Time>(allocator);
      }
      case bdem_ElemType::BDEM_BOOL: {
        return makeValuePtrInplace<bool>(allocator);
      }
      case bdem_ElemType::BDEM_DATETIMETZ: {
        return makeValuePtrInplace<bdet_DatetimeTz>(allocator);
      }
      case bdem_ElemType::BDEM_DATETZ: {
        return makeValuePtrInplace<bdet_DateTz>(allocator);
      }
      case bdem_ElemType::BDEM_TIMETZ: {
        return makeValuePtrInplace<bdet_TimeTz>(allocator);
      }
      case bdem_ElemType::BDEM_CHAR_ARRAY: {
        return makeValuePtrInplaceWithAlloc<bsl::vector<char> >(allocator);
      }
      case bdem_ElemType::BDEM_SHORT_ARRAY: {
        return makeValuePtrInplaceWithAlloc<bsl::vector<short> >(allocator);
      }
      case bdem_ElemType::BDEM_INT_ARRAY: {
        return makeValuePtrInplaceWithAlloc<bsl::vector<int> >(allocator);
      }
      case bdem_ElemType::BDEM_INT64_ARRAY: {
        return makeValuePtrInplaceWithAlloc<bsl::vector<Int64> >(allocator);
      }
      case bdem_ElemType::BDEM_FLOAT_ARRAY: {
        return makeValuePtrInplaceWithAlloc<bsl::vector<float> >(allocator);
      }
      case bdem_ElemType::BDEM_DOUBLE_ARRAY: {
        return makeValuePtrInplaceWithAlloc<bsl::vector<double> >(allocator);
      }
      case bdem_ElemType::BDEM_STRING_ARRAY: {
        return makeValuePtrInplaceWithAlloc<bsl::vector<bsl::string> >(
                                                                    allocator);
      }
      case bdem_ElemType::BDEM_DATETIME_ARRAY: {
        return makeValuePtrInplaceWithAlloc<bsl::vector<bdet_Datetime> >(
                                                                    allocator);
      }
      case bdem_ElemType::BDEM_DATE_ARRAY: {
        return makeValuePtrInplaceWithAlloc<bsl::vector<bdet_Date> >(
                                                                    allocator);
      }
      case bdem_ElemType::BDEM_TIME_ARRAY: {
        return makeValuePtrInplaceWithAlloc<bsl::vector<bdet_Time> >(
                                                                    allocator);
      }
      case bdem_ElemType::BDEM_BOOL_ARRAY: {
        return makeValuePtrInplaceWithAlloc<bsl::vector<bool> >(allocator);
      }
      case bdem_ElemType::BDEM_DATETIMETZ_ARRAY: {
        return makeValuePtrInplaceWithAlloc<bsl::vector<bdet_DatetimeTz> >(
                                                                    allocator);
      }
      case bdem_ElemType::BDEM_DATETZ_ARRAY: {
        return makeValuePtrInplaceWithAlloc<bsl::vector<bdet_DateTz> >(
                                                                    allocator);
      }
      case bdem_ElemType::BDEM_TIMETZ_ARRAY: {
        return makeValuePtrInplaceWithAlloc<bsl::vector<bdet_TimeTz> >(
                                                                    allocator);
      }
      case bdem_ElemType::BDEM_LIST: {
        return makeValuePtrInplaceWithAlloc<bdem_List>(allocator);
      }
      case bdem_ElemType::BDEM_TABLE: {
        return makeValuePtrInplaceWithAlloc<bdem_Table>(allocator);
      }
      case bdem_ElemType::BDEM_CHOICE: {
        return makeValuePtrInplaceWithAlloc<bdem_Choice>(allocator);
      }
      case bdem_ElemType::BDEM_CHOICE_ARRAY: {
        return makeValuePtrInplaceWithAlloc<bdem_ChoiceArray>(allocator);
      }
      case bdem_ElemType::BDEM_VOID:
      default: {
        return bcema_SharedPtr<void>();
      }
    }
}

                        //---------------------
                        // class bcem_Aggregate
                        //---------------------

// PRIVATE MANIPULATORS
bcema_SharedPtr<const bdem_Schema> bcem_Aggregate::schemaPtr() const
{
    if (0 == d_schemaRep_p) {
        return bcema_SharedPtr<const bdem_Schema>();                  // RETURN
    }
    d_schemaRep_p->acquireRef();
    return bcema_SharedPtr<const bdem_Schema>(d_rawData.schema(),
                                              d_schemaRep_p);
}

bcema_SharedPtr<const bdem_RecordDef> bcem_Aggregate::recordDefPtr() const
{
    if (0 == d_schemaRep_p) {
        return bcema_SharedPtr<const bdem_RecordDef>();               // RETURN
    }
    d_schemaRep_p->acquireRef();
    bcema_SharedPtr<const bdem_Schema> schema_sp(d_rawData.schema(),
                                                 d_schemaRep_p);
    return bcema_SharedPtr<const bdem_RecordDef>(schema_sp,
                                                 d_rawData.recordConstraint());
}

bcema_SharedPtr<void> bcem_Aggregate::dataPtr() const
{
    if (0 == d_valueRep_p) {
        return bcema_SharedPtr<void>();                               // RETURN
    }
    d_valueRep_p->acquireRef();
    return bcema_SharedPtr<void>((void *)d_rawData.data(), d_valueRep_p);
}

void bcem_Aggregate::init(
                     const bcema_SharedPtr<const bdem_Schema>&  schemaPtr,
                     const bdem_RecordDef                      *recordDefPtr,
                     bdem_ElemType::Type                        elemType,
                     bslma_Allocator                           *basicAllocator)
{
    BSLS_ASSERT_SAFE(0 == d_schemaRep_p);
    BSLS_ASSERT_SAFE(0 == d_valueRep_p);
    BSLS_ASSERT_SAFE(0 == d_isTopLevelAggregateNullRep_p);

    if (bdem_ElemType::BDEM_VOID == elemType) {

        // Determine aggregate element type from record type

        elemType = bdem_RecordDef::BDEM_CHOICE_RECORD ==
                                                     recordDefPtr->recordType()
                 ? bdem_ElemType::BDEM_CHOICE
                 : bdem_ElemType::BDEM_LIST;
    }

    bslma_Allocator *allocator = bslma_Default::allocator(basicAllocator);

    bcema_SharedPtr<void> valuePtr;
    switch (elemType) {
      case bdem_ElemType::BDEM_LIST: {
        if (recordDefPtr->recordType() !=
                                        bdem_RecordDef::BDEM_SEQUENCE_RECORD) {
            *this = makeError(bcem_ErrorCode::BCEM_NOT_A_SEQUENCE,
                              "Attempt to create a LIST aggregate from "
                              "non-SEQUENCE record def \"%s\"",
                              recordName(recordDefPtr));
            return;                                                   // RETURN
        }
        bcema_SharedPtr<bdem_List> listPtr =
                            makeValuePtrInplaceWithAlloc<bdem_List>(allocator);
        bdem_SchemaAggregateUtil::initListDeep(listPtr.ptr(), *recordDefPtr);
        valuePtr = listPtr;
      } break;
      case bdem_ElemType::BDEM_TABLE: {
        if (recordDefPtr->recordType() !=
                                        bdem_RecordDef::BDEM_SEQUENCE_RECORD) {
            *this = makeError(bcem_ErrorCode::BCEM_NOT_A_SEQUENCE,
                              "Attempt to create a TABLE aggregate from "
                              "non-SEQUENCE record def \"%s\"",
                              recordName(recordDefPtr));
            return;                                                   // RETURN
        }
        bcema_SharedPtr<bdem_Table> tablePtr =
                           makeValuePtrInplaceWithAlloc<bdem_Table>(allocator);
        valuePtr = tablePtr;
        bdem_SchemaAggregateUtil::initTable(tablePtr.ptr(), *recordDefPtr);
      } break;
      case bdem_ElemType::BDEM_CHOICE: {
        if (recordDefPtr->recordType() != bdem_RecordDef::BDEM_CHOICE_RECORD) {
            *this = makeError(bcem_ErrorCode::BCEM_NOT_A_CHOICE,
                              "Attempt to create a CHOICE aggregate from "
                              "non-CHOICE record def \"%s\"",
                              recordName(recordDefPtr));
            return;                                                   // RETURN
        }
        bcema_SharedPtr<bdem_Choice> choicePtr =
                          makeValuePtrInplaceWithAlloc<bdem_Choice>(allocator);
        bdem_SchemaAggregateUtil::initChoice(choicePtr.ptr(), *recordDefPtr);
        valuePtr = choicePtr;
      } break;
      case bdem_ElemType::BDEM_CHOICE_ARRAY: {
        if (recordDefPtr->recordType() != bdem_RecordDef::BDEM_CHOICE_RECORD) {
            *this = makeError(bcem_ErrorCode::BCEM_NOT_A_CHOICE,
                              "Attempt to create a CHOICE_ARRAY aggregate "
                              "from non-CHOICE record def \"%s\"",
                              recordName(recordDefPtr));
            return;                                                   // RETURN
        }
        bcema_SharedPtr<bdem_ChoiceArray> choiceArrayPtr =
                     makeValuePtrInplaceWithAlloc<bdem_ChoiceArray>(allocator);
        bdem_SchemaAggregateUtil::initChoiceArray(choiceArrayPtr.ptr(),
                                                  *recordDefPtr);
        valuePtr = choiceArrayPtr;
      } break;
      default: {
          *this = makeError(bcem_ErrorCode::BCEM_NOT_A_RECORD,
                          "Attempt to specify a record definition when "
                          "constructing an object of non-aggregate type %s",
                          bdem_ElemType::toAscii(elemType));
        return;                                                       // RETURN
      }
    }

    d_rawData.setDataType(elemType);
    d_rawData.setSchemaPointer(schemaPtr.ptr());
    d_schemaRep_p = schemaPtr.rep();
    if (d_schemaRep_p) {
        d_schemaRep_p->acquireRef();
    }

    bcem_Aggregate_RepProctor schemaRepProctor(d_schemaRep_p);

    BSLS_ASSERT(valuePtr.ptr());

    d_rawData.setRecordDefPointer(recordDefPtr);
    d_rawData.setDataPointer(valuePtr.ptr());
    d_valueRep_p = valuePtr.rep();
    d_valueRep_p->acquireRef();

    bcem_Aggregate_RepProctor valueRepProtctor(d_valueRep_p);

    // "nullness" data members are set in the constructors.
    bcema_SharedPtr<int> isNull_sp;
    isNull_sp.createInplace(allocator, 0);
    d_isTopLevelAggregateNullRep_p = isNull_sp.rep();
    d_isTopLevelAggregateNullRep_p->acquireRef();
    d_rawData.setTopLevelAggregateNullnessPointer(isNull_sp.ptr());

    valueRepProtctor.release();
    schemaRepProctor.release();
}

void bcem_Aggregate::init(
                  const bcema_SharedPtr<const bdem_RecordDef>&  recordDefPtr,
                  bdem_ElemType::Type                           elemType,
                  bslma_Allocator                              *basicAllocator)
{
    bcema_SharedPtr<const bdem_Schema> schemaPtr(recordDefPtr,
                                                 &recordDefPtr->schema());
    init(schemaPtr, recordDefPtr.ptr(), elemType, basicAllocator);
}

void bcem_Aggregate::init(
                     const bcema_SharedPtr<const bdem_Schema>&  schemaPtr,
                     const char                                *recName,
                     bdem_ElemType::Type                        elemType,
                     bslma_Allocator                           *basicAllocator)
{
    const bdem_RecordDef *record = schemaPtr->lookupRecord(recName);
    if (! record) {
        *this = makeError(bcem_ErrorCode::BCEM_NOT_A_RECORD,
                          "Unable to find record \"%s\" in schema",
                          recName);
        return;                                                       // RETURN
    }

    init(schemaPtr, record, elemType, basicAllocator);
}

// PRIVATE ACCESSORS

const bcem_Aggregate
bcem_Aggregate::makeError(bcem_ErrorCode::Code  errorCode,
                          const char                *msg, ...) const
{
    if (0 == errorCode || isError()) {

        // Return this object if success is being returned or this object is
        // already an error.

        return *this;                                                 // RETURN
    }

    enum { MAX_ERROR_STRING = 512 };
    char errorString[MAX_ERROR_STRING];

    using namespace bsl;  // in case 'vsnprintf' is in 'bsl'

    va_list args;
    va_start(args, msg);
    vsnprintf(errorString, MAX_ERROR_STRING, msg, args);
    va_end(args);

    bcem_AggregateError error(errorCode, errorString);
    return makeError(error);
}

const bcem_Aggregate
bcem_Aggregate::makeError(const bcem_AggregateError& errorDescription) const
{
    if (0 == errorDescription.code() || isError()) {

        // Return this object if success is being returned or this object is
        // already an error.

        return *this;                                                 // RETURN
    }

    bcema_SharedPtr<bcem_AggregateError> errPtr;
    errPtr.createInplace(0, errorDescription);

    bcem_AggregateRaw errorValue;
    errorValue.setDataPointer(errPtr.ptr());

    return bcem_Aggregate(errorValue, 0, errPtr.rep(), 0);
}

// CREATORS
bcem_Aggregate::bcem_Aggregate()
: d_rawData()
, d_schemaRep_p(0)
, d_valueRep_p(0)
, d_isTopLevelAggregateNullRep_p(0)
{
}

bcem_Aggregate::bcem_Aggregate(const bcem_Aggregate& original)
: d_rawData(original.d_rawData)
, d_schemaRep_p(original.d_schemaRep_p)
, d_valueRep_p(original.d_valueRep_p)
, d_isTopLevelAggregateNullRep_p(original.d_isTopLevelAggregateNullRep_p)
{
    if (d_schemaRep_p) {
        d_schemaRep_p->acquireRef();
    }
    if (d_valueRep_p) {
        d_valueRep_p->acquireRef();
    }
    if (d_isTopLevelAggregateNullRep_p) {
        d_isTopLevelAggregateNullRep_p->acquireRef();
    }
}

bcem_Aggregate::bcem_Aggregate(const bcem_AggregateRaw&  rawData,
                               bcema_SharedPtrRep       *schemaRep,
                               bcema_SharedPtrRep       *valueRep,
                               bcema_SharedPtrRep       *topLevelNullRep)
: d_rawData(rawData)
, d_schemaRep_p(schemaRep)
, d_valueRep_p(valueRep)
, d_isTopLevelAggregateNullRep_p(topLevelNullRep)
{
    if (d_schemaRep_p) {
        d_schemaRep_p->acquireRef();
    }
    if (d_valueRep_p) {
        d_valueRep_p->acquireRef();
    }
    if (d_isTopLevelAggregateNullRep_p) {
        d_isTopLevelAggregateNullRep_p->acquireRef();
    }
}

bcem_Aggregate::~bcem_Aggregate()
{
    if (d_isTopLevelAggregateNullRep_p) {
        d_isTopLevelAggregateNullRep_p->releaseRef();
    }
    if (d_valueRep_p) {
        d_valueRep_p->releaseRef();
    }
    if (d_schemaRep_p) {
        d_schemaRep_p->releaseRef();
    }
    d_rawData.reset();
}

// MANIPULATORS
bcem_Aggregate& bcem_Aggregate::operator=(const bcem_Aggregate& rhs)
{
    if (this != &rhs) {
        d_rawData = rhs.d_rawData;

        if (d_isTopLevelAggregateNullRep_p) {
            d_isTopLevelAggregateNullRep_p->releaseRef();
        }
        if (d_valueRep_p) {
            d_valueRep_p->releaseRef();
        }
        if (d_schemaRep_p) {
            d_schemaRep_p->releaseRef();
        }

        d_schemaRep_p = rhs.d_schemaRep_p;
        if (d_schemaRep_p) {
            d_schemaRep_p->acquireRef();
        }

        d_valueRep_p = rhs.d_valueRep_p;
        if (d_valueRep_p) {
            d_valueRep_p->acquireRef();
        }

        d_isTopLevelAggregateNullRep_p = rhs.d_isTopLevelAggregateNullRep_p;
        if (d_isTopLevelAggregateNullRep_p) {
            d_isTopLevelAggregateNullRep_p->acquireRef();
        }
    }

    return *this;
}

const bcem_Aggregate& bcem_Aggregate::reset()
{
    d_rawData.reset();

    if (d_isTopLevelAggregateNullRep_p) {
        d_isTopLevelAggregateNullRep_p->releaseRef();
        d_isTopLevelAggregateNullRep_p = 0;
    }
    if (d_valueRep_p) {
        d_valueRep_p->releaseRef();
        d_valueRep_p = 0;
    }
    if (d_schemaRep_p) {
        d_schemaRep_p->releaseRef();
        d_schemaRep_p = 0;
    }
    return *this;
}

// ACCESSORS THAT MANIPULATE DATA
const bcem_Aggregate bcem_Aggregate::resize(int newSize) const
{
    bcem_AggregateError errorDescription;
    if (0 == d_rawData.resize(&errorDescription,
                              static_cast<bsl::size_t>(newSize))) {
        return *this;                                                 // RETURN
    }
    else {
        return makeError(errorDescription);                           // RETURN
    }
}

const bcem_Aggregate bcem_Aggregate::insertItems(int pos, int numItems) const
{
    bcem_AggregateError errorDescription;

    if (0 == d_rawData.insertItems(&errorDescription, pos, numItems)) {
        return *this;                                                 // RETURN
    }
    else {
        return makeError(errorDescription);                           // RETURN
    }
}

const bcem_Aggregate bcem_Aggregate::insertNullItems(int pos,
                                                     int numItems) const
{
    bcem_AggregateError errorDescription;

    if (0 == d_rawData.insertNullItems(&errorDescription, pos, numItems)) {
        return *this;                                                 // RETURN
    }
    else {
        return makeError(errorDescription);                           // RETURN
    }
}

const bcem_Aggregate
bcem_Aggregate::makeSelectionByIndex(int index) const
{
    bcem_AggregateRaw   field;
    bcem_AggregateError errorDescription;

    if (0 == d_rawData.makeSelectionByIndex(&field,
                                            &errorDescription,
                                            index)) {
        return bcem_Aggregate(field,
                              d_schemaRep_p,
                              d_valueRep_p,
                              d_isTopLevelAggregateNullRep_p);        // RETURN
    }
    else {
        return makeError(errorDescription);                           // RETURN
    }
}

const bcem_Aggregate bcem_Aggregate::removeItems(int pos, int numItems) const
{
    bcem_AggregateError errorDescription;

    if (0 == d_rawData.removeItems(&errorDescription, pos, numItems)) {
        return *this;                                                 // RETURN
    }
    else {
        return makeError(errorDescription);                           // RETURN
    }
}

const bcem_Aggregate bcem_Aggregate::selection() const
{
    bcem_AggregateRaw   field;
    bcem_AggregateError errorDescription;
    if (0 == d_rawData.selection(&field, &errorDescription)) {
        return bcem_Aggregate(field,
                              d_schemaRep_p,
                              d_valueRep_p,
                              d_isTopLevelAggregateNullRep_p);        // RETURN
    }
    else {
        return makeError(errorDescription);
    }
}

const bcem_Aggregate
bcem_Aggregate::makeSelection(const char *newSelector) const
{
    bcem_AggregateRaw   field;
    bcem_AggregateError errorDescription;

    if (0 == d_rawData.makeSelection(&field,
                                     &errorDescription,
                                     newSelector)) {
        return bcem_Aggregate(field,
                              d_schemaRep_p,
                              d_valueRep_p,
                              d_isTopLevelAggregateNullRep_p);        // RETURN
    }
    else {
        return makeError(errorDescription);                           // RETURN
    }
}

// MANIPULATORS
void bcem_Aggregate::swap(bcem_Aggregate& rhs)
{
    d_rawData.swap(rhs.d_rawData);
    bsl::swap(d_schemaRep_p, rhs.d_schemaRep_p);
    bsl::swap(d_valueRep_p, rhs.d_valueRep_p);
    bsl::swap(d_isTopLevelAggregateNullRep_p,
              rhs.d_isTopLevelAggregateNullRep_p);
}

// ACCESSORS
bsl::string bcem_Aggregate::errorMessage() const
{
    if (! isError()) {
        return "";                                                    // RETURN
    }

    return ((bcem_AggregateError *)data())->description();
}

const bcem_Aggregate bcem_Aggregate::field(NameOrIndex fieldOrIdx) const
{
    return fieldImp(false, fieldOrIdx);
}

const bcem_Aggregate
bcem_Aggregate::fieldImp(bool        makeNonNullFlag,
                         NameOrIndex fieldOrIdx1,
                         NameOrIndex fieldOrIdx2,
                         NameOrIndex fieldOrIdx3,
                         NameOrIndex fieldOrIdx4,
                         NameOrIndex fieldOrIdx5,
                         NameOrIndex fieldOrIdx6,
                         NameOrIndex fieldOrIdx7,
                         NameOrIndex fieldOrIdx8,
                         NameOrIndex fieldOrIdx9,
                         NameOrIndex fieldOrIdx10) const
{
    bcem_AggregateRaw   field;
    bcem_AggregateError errorDescription;

    if (0 == d_rawData.getField(&field,
                                &errorDescription,
                                makeNonNullFlag,
                                fieldOrIdx1,
                                fieldOrIdx2,
                                fieldOrIdx3,
                                fieldOrIdx4,
                                fieldOrIdx5,
                                fieldOrIdx6,
                                fieldOrIdx7,
                                fieldOrIdx8,
                                fieldOrIdx9,
                                fieldOrIdx10)) {
        return bcem_Aggregate(field,
                              d_schemaRep_p,
                              d_valueRep_p,
                              d_isTopLevelAggregateNullRep_p);        // RETURN
    }
    else {
        return makeError(errorDescription);                           // RETURN
    }
}

const bcem_Aggregate bcem_Aggregate::field(NameOrIndex fieldOrIdx1,
                                           NameOrIndex fieldOrIdx2,
                                           NameOrIndex fieldOrIdx3,
                                           NameOrIndex fieldOrIdx4,
                                           NameOrIndex fieldOrIdx5,
                                           NameOrIndex fieldOrIdx6,
                                           NameOrIndex fieldOrIdx7,
                                           NameOrIndex fieldOrIdx8,
                                           NameOrIndex fieldOrIdx9,
                                           NameOrIndex fieldOrIdx10) const
{
    return fieldImp(false,
                    fieldOrIdx1,
                    fieldOrIdx2,
                    fieldOrIdx3,
                    fieldOrIdx4,
                    fieldOrIdx5,
                    fieldOrIdx6,
                    fieldOrIdx7,
                    fieldOrIdx8,
                    fieldOrIdx9,
                    fieldOrIdx10);
}

const bcem_Aggregate bcem_Aggregate::fieldById(int fieldId) const
{
    bcem_AggregateRaw   field;
    bcem_AggregateError errorDescription;

    if (0 == d_rawData.fieldById(&field, &errorDescription, fieldId)) {
        return bcem_Aggregate(field,
                              d_schemaRep_p,
                              d_valueRep_p,
                              d_isTopLevelAggregateNullRep_p);        // RETURN
    }
    else {
        return makeError(errorDescription);                           // RETURN
    }
}

const bcem_Aggregate bcem_Aggregate::fieldByIndex(int index) const
{
    bcem_AggregateRaw   field;
    bcem_AggregateError errorDescription;
    if (0 == d_rawData.fieldByIndex(&field, &errorDescription, index)) {
        return bcem_Aggregate(field,
                              d_schemaRep_p,
                              d_valueRep_p,
                              d_isTopLevelAggregateNullRep_p);        // RETURN
    }
    else {
        return makeError(errorDescription);                           // RETURN
    }
}


const bcem_Aggregate bcem_Aggregate::anonymousField(int n) const
{
    bcem_AggregateRaw   field;
    bcem_AggregateError errorDescription;
    if (0 == d_rawData.anonymousField(&field, &errorDescription, n)) {
        return bcem_Aggregate(field,
                              d_schemaRep_p,
                              d_valueRep_p,
                              d_isTopLevelAggregateNullRep_p);        // RETURN
    }
    else {
        return makeError(errorDescription);                           // RETURN
    }
}

const bcem_Aggregate bcem_Aggregate::anonymousField() const
{
    if (d_rawData.recordConstraint()
     && 1 < d_rawData.recordConstraint()->numAnonymousFields()) {

        // Only report error if there are more than one anonymous fields.  The
        // case where there are zero anonymous fields is already handled by
        // the single-argument call to 'anonymousField', below.

        return makeError(bcem_ErrorCode::BCEM_AMBIGUOUS_ANON,
                         "anonymousField() called for object with multiple "
                         "anonymous fields.  Cannot pick one.");      // RETURN
    }

    return anonymousField(0);
}

bdem_ElemType::Type
bcem_Aggregate::fieldType(NameOrIndex fieldOrIdx1,
                          NameOrIndex fieldOrIdx2,
                          NameOrIndex fieldOrIdx3,
                          NameOrIndex fieldOrIdx4,
                          NameOrIndex fieldOrIdx5,
                          NameOrIndex fieldOrIdx6,
                          NameOrIndex fieldOrIdx7,
                          NameOrIndex fieldOrIdx8,
                          NameOrIndex fieldOrIdx9,
                          NameOrIndex fieldOrIdx10) const
{
    return fieldImp(false,
                    fieldOrIdx1,
                    fieldOrIdx2,
                    fieldOrIdx3,
                    fieldOrIdx4,
                    fieldOrIdx5,
                    fieldOrIdx6,
                    fieldOrIdx7,
                    fieldOrIdx8,
                    fieldOrIdx9,
                    fieldOrIdx10).dataType();
}

bdem_ElemType::Type bcem_Aggregate::fieldTypeById(int fieldId) const
{
    return fieldById(fieldId).dataType();
}

bdem_ElemType::Type bcem_Aggregate::fieldTypeByIndex(int index) const
{
    return fieldByIndex(index).dataType();
}

bdem_ElemRef bcem_Aggregate::fieldRef(NameOrIndex fieldOrIdx1,
                                      NameOrIndex fieldOrIdx2,
                                      NameOrIndex fieldOrIdx3,
                                      NameOrIndex fieldOrIdx4,
                                      NameOrIndex fieldOrIdx5,
                                      NameOrIndex fieldOrIdx6,
                                      NameOrIndex fieldOrIdx7,
                                      NameOrIndex fieldOrIdx8,
                                      NameOrIndex fieldOrIdx9,
                                      NameOrIndex fieldOrIdx10) const
{
    return fieldImp(false,
                    fieldOrIdx1,
                    fieldOrIdx2,
                    fieldOrIdx3,
                    fieldOrIdx4,
                    fieldOrIdx5,
                    fieldOrIdx6,
                    fieldOrIdx7,
                    fieldOrIdx8,
                    fieldOrIdx9,
                    fieldOrIdx10).asElemRef();
}

bdem_ElemRef bcem_Aggregate::fieldRefById(int fieldId) const
{
    return fieldById(fieldId).asElemRef();
}

bdem_ElemRef bcem_Aggregate::fieldRefByIndex(int index) const
{
    return fieldByIndex(index).asElemRef();
}

const bcem_Aggregate bcem_Aggregate::operator[](int index) const
{
    return fieldImp(false, index);  // TBD pass true?
}

const bcem_Aggregate bcem_Aggregate::makeSelectionById(int id) const
{
    bcem_AggregateRaw   field;
    bcem_AggregateError errorDescription;

    if (0 == d_rawData.makeSelectionById(&field,
                                         &errorDescription,
                                         id)) {
        return bcem_Aggregate(field,
                              d_schemaRep_p,
                              d_valueRep_p,
                              d_isTopLevelAggregateNullRep_p);        // RETURN
    }
    else {
        return makeError(errorDescription);                           // RETURN
    }
}

#if !defined(BSL_LEGACY) || 1 == BSL_LEGACY
bool bcem_Aggregate::isUnset() const
{
    bool isUnsetFlag;
    switch (dataType()) {
      case bdem_ElemType::BDEM_VOID: {
        isUnsetFlag = true;
      } break;
      case bdem_ElemType::BDEM_TABLE: {
        const bdem_Table *table = (bdem_Table *) data();
        if (recordConstraint()) {
            isUnsetFlag = 0 == table->numRows();
        }
        else {
            isUnsetFlag = 0 == table->numRows() && 0 == table->numColumns();
        }
      } break;
      case bdem_ElemType::BDEM_CHOICE: {
        const bdem_Choice *choice = (bdem_Choice *) data();
        if (recordConstraint()) {
            isUnsetFlag = choice->selector() < 0;
        }
        else {
            isUnsetFlag = choice->selector() < 0
                       && 0 == choice->numSelections();
        }
      } break;
      case bdem_ElemType::BDEM_CHOICE_ARRAY_ITEM: {
        const bdem_ChoiceArrayItem *item = (bdem_ChoiceArrayItem *) data();
        isUnsetFlag = item->selector() < 0;
      } break;
      case bdem_ElemType::BDEM_CHOICE_ARRAY: {
        const bdem_ChoiceArray *choiceArray = (bdem_ChoiceArray *) data();
        if (recordConstraint()) {
            isUnsetFlag = 0 == choiceArray->length();
        }
        else {
            isUnsetFlag = 0 == choiceArray->length()
                       && 0 == choiceArray->numSelections();
        }
      } break;
      case bdem_ElemType::BDEM_ROW:
      case bdem_ElemType::BDEM_LIST:
      default: {
        const bdem_Descriptor *descriptor =
                                bdem_ElemAttrLookup::lookupTable()[dataType()];
        isUnsetFlag = descriptor->isUnset(data());
      }
    }
    return isUnsetFlag;
}
#endif // !defined(BSL_LEGACY) || 1 == BSL_LEGACY

const bcem_Aggregate
bcem_Aggregate::clone(bslma_Allocator *basicAllocator) const
{
    bcem_Aggregate returnVal(this->cloneData(basicAllocator));

    if (! d_rawData.schema()) {
        return returnVal;                                             // RETURN
    }

    // Clone the schema

    bcema_SharedPtr<bdem_Schema> schemaClone;
    schemaClone.createInplace(basicAllocator,
                              *d_rawData.schema(),
                              basicAllocator);

    returnVal.d_rawData.setSchemaPointer(schemaClone.ptr());

    bcema_SharedPtrRep *schemaCloneRep = schemaClone.rep();
    bsl::swap(returnVal.d_schemaRep_p, schemaCloneRep);
    if (schemaCloneRep) {
        schemaCloneRep->releaseRef();
    }
    returnVal.d_schemaRep_p->acquireRef();

    if (d_rawData.recordConstraint()) {

        // Set the clone's record pointer to point into the cloned schema

        int recordIndex = d_rawData.recordConstraint()->recordIndex();
        returnVal.d_rawData.setRecordDefPointer(
                                           &schemaClone->record(recordIndex));
    }

    if (d_rawData.fieldDef()) {

        // The field spec is not null -- find the field spec in the original
        // schema.
        // TBD: The only way to find the field spec in the schema is to do a
        // linear search through all of the fields of all of the recordDefs.
        // Can we find a more efficient way to do this?

        for (int recIndex = 0;
             recIndex < d_rawData.schema()->numRecords();
             ++recIndex) {
            const bdem_RecordDef& rec = d_rawData.schema()->record(recIndex);
            for (int fieldIndex = 0;
                 fieldIndex < rec.numFields();
                 ++fieldIndex) {
                const bdem_FieldDef& field = rec.field(fieldIndex);
                if (&field == d_rawData.fieldDef()) {

                    // Point the field spec in the clone to the corresponding
                    // fieldspec within the cloned schema.

                    returnVal.d_rawData.setFieldDefPointer(
                          &(returnVal.d_rawData.schema()->
                               record(recIndex).field(fieldIndex)));
                    return returnVal;
                } // end if (match)
            } // end for (each field in record)
        } // end for (each record in original schema)
    } // end if (field spec is not static)

    return returnVal;
}

const bcem_Aggregate
bcem_Aggregate::cloneData(bslma_Allocator *basicAllocator) const
{
    bcema_SharedPtr<void> valuePtr;
    bslma_Allocator *allocator = bslma_Default::allocator(basicAllocator);

    switch (dataType()) {
      case bdem_ElemType::BDEM_ROW: {

        // ROW is a special case.  Because a row has no copy constructor, it
        // is necessary to make a list containing a copy of the row, then
        // return an aggregate that refers to the copy within the list.

        // Construct a parent list

        bcema_SharedPtr<void> parent =
                            makeValuePtrInplaceWithAlloc<bdem_List>(allocator);
        bdem_List& parentList = *(bdem_List *)parent.ptr();

        // Perform a row-to-list assignment to make a copy of this row.

        parentList = *(bdem_Row *)data();

        // Get a shared pointer to the row within the parent list

        valuePtr.loadAlias(parent, &parentList.row());
      } break;
      case bdem_ElemType::BDEM_CHOICE_ARRAY_ITEM: {

        // CHOICE_ARRAY_ITEM is a special case.  Because a choice array item
        // has no copy constructor, it is necessary to make a choice
        // containing a copy of the item, then return an aggregate that
        // refers to the copy within the choice.

        // Construct a parent choice

        bcema_SharedPtr<void> parent =
                          makeValuePtrInplaceWithAlloc<bdem_Choice>(allocator);
        bdem_Choice& parentChoice = *(bdem_Choice *)parent.ptr();

        // Perform an item-to-choice assignment to make a copy of this item.

        parentChoice = *(bdem_ChoiceArrayItem *)data();

        // Get a shared pointer to the item within the parent choice

        valuePtr.loadAlias(parent, &parentChoice.item());
      } break;
      case bdem_ElemType::BDEM_VOID: {
        if (isError()) {
            valuePtr = makeValuePtrInplaceWithAlloc<bcem_AggregateError>(
                                                                    allocator);
            *static_cast<bcem_AggregateError *>(valuePtr.ptr()) =
                             *static_cast<const bcem_AggregateError *>(data());
        }
        else {

            // Return an empty aggregate

            return bcem_Aggregate();                                  // RETURN
        }
      } break;
      default: {

        // Make a copy of the value.

        const bdem_Descriptor *descriptor =
                                bdem_ElemAttrLookup::lookupTable()[dataType()];

        valuePtr = makeValuePtr(dataType(), allocator);
        descriptor->assign(valuePtr.ptr(), data());
      } break;
    }

    bcem_Aggregate returnVal(*this);
    returnVal.d_rawData.setDataPointer(valuePtr.ptr());

    bcema_SharedPtrRep *valuePtrRep = valuePtr.rep();
    bsl::swap(returnVal.d_valueRep_p, valuePtrRep);
    if (valuePtrRep) {
        valuePtrRep->releaseRef();
    }
    returnVal.d_valueRep_p->acquireRef();

    // Clone is a top-level aggregate.

    returnVal.d_rawData.clearParent();

    if (bdem_ElemType::BDEM_VOID != dataType()) {
        bcema_SharedPtr<int> isNull_sp;
        isNull_sp.createInplace(allocator, isNul2());

        returnVal.d_rawData.setTopLevelAggregateNullnessPointer(
                                                              isNull_sp.ptr());

        bcema_SharedPtrRep *isNullRep = isNull_sp.rep();
        bsl::swap(returnVal.d_isTopLevelAggregateNullRep_p, isNullRep);
        if (isNullRep) {
            isNullRep->releaseRef();
        }
        returnVal.d_isTopLevelAggregateNullRep_p->acquireRef();
    }

    return returnVal;
}

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2006, 2011
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
