// bdlaggxxx_aggregate.cpp                                            -*-C++-*-
#include <bdlaggxxx_aggregate.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlaggxxx_aggregate_cpp,"$Id$ $CSID$")

#include <bdlaggxxx_errorattributes.h>
#include <bdlaggxxx_fieldselector.h>

#include <bdlmxxx_choicearrayitem.h>
#include <bdlmxxx_descriptor.h>
#include <bdlmxxx_elemattrlookup.h>
#include <bdlmxxx_schemaaggregateutil.h>
#include <bdlmxxx_schemautil.h>
#include <bdlb_print.h>

#include <bsls_assert.h>
#include <bslma_default.h>
#include <bsls_platform.h>
#include <bsls_types.h>

#include <stdio.h>     // 'v/snprintf' (not always declared in <bsl_cstdio.h>).

#include <bsl_algorithm.h>       // 'swap'
#include <bsl_cstdarg.h>
#include <bsl_cstdlib.h>         // 'strtol'
#include <bsl_cstring.h>         // 'memcpy'
#include <bsl_iterator.h>
#include <bsl_sstream.h>

namespace BloombergLP {

namespace {

// TYPES
typedef bsls::Types::Int64            Int64;

// HELPER FUNCTIONS
template <class DATATYPE>
inline
bsl::shared_ptr<DATATYPE> makeValuePtrInplace(bslma::Allocator *basicAllocator)
    // Return a shared pointer to an object of type 'DATATYPE' using the
    // "in-place" construction facility of 'bsl::shared_ptr' to
    // default-construct 'DATATYPE'.  The allocator is used to create the
    // shared pointer, but is not passed to 'DATATYPE's constructor.
{
    bsl::shared_ptr<DATATYPE> result;
    result.createInplace(basicAllocator);
    return result;
}

template <class DATATYPE>
inline
bsl::shared_ptr<DATATYPE>
makeValuePtrInplaceWithAlloc(bslma::Allocator *basicAllocator)
    // Return a shared pointer to an object of type 'DATATYPE' using the
    // "in-place" construction facility of 'bsl::shared_ptr' to
    // default-construct 'DATATYPE'.  The allocator is used to create the
    // shared pointer, and is also passed to 'DATATYPE's constructor.
{
    bsl::shared_ptr<DATATYPE> result;
    result.createInplace(basicAllocator, basicAllocator);
    return result;
}

const char *recordName(const bdlmxxx::RecordDef *recordDef)
{
    if (! recordDef) {
        return "(unconstrained)";                                     // RETURN
    }

    const char *ret = recordDef->schema().recordName(recordDef->recordIndex());
    return ret ? ret : "(anonymous)";
}

                    // =====================================
                    // local class bcem_Aggregate_RepProctor
                    // =====================================

class bcem_Aggregate_RepProctor {
    // This "component-private" class is a proctor for managing shared
    // pointers and releasing the data from management on destruction.

    // DATA
    bslma::SharedPtrRep *d_rep_p;                     // shared ptr rep

    // NOT IMPLEMENTED
    bcem_Aggregate_RepProctor(const bcem_Aggregate_RepProctor& original);
    bcem_Aggregate_RepProctor& operator=(const bcem_Aggregate_RepProctor& rhs);

  public:
    // CREATORS
    bcem_Aggregate_RepProctor(bslma::SharedPtrRep *rep)
    : d_rep_p(rep)
    {}

    ~bcem_Aggregate_RepProctor()
    {
        if (d_rep_p) {
            d_rep_p->releaseRef();
        }
    }

    // MANIPULATORS
    void release() {
        d_rep_p = 0;
    }
};

}  // close unnamed namespace

namespace bdlaggxxx {
                        //---------------------
                        // class Aggregate
                        //---------------------

bsl::shared_ptr<void>
Aggregate::makeValuePtr(bdlmxxx::ElemType::Type  type,
                             bslma::Allocator    *basicAllocator)
{
    bslma::Allocator *allocator = bslma::Default::allocator(basicAllocator);

    switch (type) {
      case bdlmxxx::ElemType::BDEM_CHAR: {
        return makeValuePtrInplace<char>(allocator);                  // RETURN
      }
      case bdlmxxx::ElemType::BDEM_SHORT: {
        return makeValuePtrInplace<short>(allocator);                 // RETURN
      }
      case bdlmxxx::ElemType::BDEM_INT: {
        return makeValuePtrInplace<int>(allocator);                   // RETURN
      }
      case bdlmxxx::ElemType::BDEM_INT64: {
        return makeValuePtrInplace<Int64>(allocator);                 // RETURN
      }
      case bdlmxxx::ElemType::BDEM_FLOAT: {
        return makeValuePtrInplace<float>(allocator);                 // RETURN
      }
      case bdlmxxx::ElemType::BDEM_DOUBLE: {
        return makeValuePtrInplace<double>(allocator);                // RETURN
      }
      case bdlmxxx::ElemType::BDEM_STRING: {
        return makeValuePtrInplaceWithAlloc<bsl::string>(allocator);  // RETURN
      }
      case bdlmxxx::ElemType::BDEM_DATETIME: {
        return makeValuePtrInplace<bdlt::Datetime>(allocator);        // RETURN
      }
      case bdlmxxx::ElemType::BDEM_DATE: {
        return makeValuePtrInplace<bdlt::Date>(allocator);            // RETURN
      }
      case bdlmxxx::ElemType::BDEM_TIME: {
        return makeValuePtrInplace<bdlt::Time>(allocator);            // RETURN
      }
      case bdlmxxx::ElemType::BDEM_BOOL: {
        return makeValuePtrInplace<bool>(allocator);                  // RETURN
      }
      case bdlmxxx::ElemType::BDEM_DATETIMETZ: {
        return makeValuePtrInplace<bdlt::DatetimeTz>(allocator);      // RETURN
      }
      case bdlmxxx::ElemType::BDEM_DATETZ: {
        return makeValuePtrInplace<bdlt::DateTz>(allocator);          // RETURN
      }
      case bdlmxxx::ElemType::BDEM_TIMETZ: {
        return makeValuePtrInplace<bdlt::TimeTz>(allocator);          // RETURN
      }
      case bdlmxxx::ElemType::BDEM_CHAR_ARRAY: {
        return makeValuePtrInplaceWithAlloc<bsl::vector<char> >(allocator);
                                                                      // RETURN
      }
      case bdlmxxx::ElemType::BDEM_SHORT_ARRAY: {
        return makeValuePtrInplaceWithAlloc<bsl::vector<short> >(allocator);
                                                                      // RETURN
      }
      case bdlmxxx::ElemType::BDEM_INT_ARRAY: {
        return makeValuePtrInplaceWithAlloc<bsl::vector<int> >(allocator);
                                                                      // RETURN
      }
      case bdlmxxx::ElemType::BDEM_INT64_ARRAY: {
        return makeValuePtrInplaceWithAlloc<bsl::vector<Int64> >(allocator);
                                                                      // RETURN
      }
      case bdlmxxx::ElemType::BDEM_FLOAT_ARRAY: {
        return makeValuePtrInplaceWithAlloc<bsl::vector<float> >(allocator);
                                                                      // RETURN
      }
      case bdlmxxx::ElemType::BDEM_DOUBLE_ARRAY: {
        return makeValuePtrInplaceWithAlloc<bsl::vector<double> >(allocator);
                                                                      // RETURN
      }
      case bdlmxxx::ElemType::BDEM_STRING_ARRAY: {
        return makeValuePtrInplaceWithAlloc<bsl::vector<bsl::string> >(
                                                                    allocator);
                                                                      // RETURN
      }
      case bdlmxxx::ElemType::BDEM_DATETIME_ARRAY: {
        return makeValuePtrInplaceWithAlloc<bsl::vector<bdlt::Datetime> >(
                                                                    allocator);
                                                                      // RETURN
      }
      case bdlmxxx::ElemType::BDEM_DATE_ARRAY: {
        return makeValuePtrInplaceWithAlloc<bsl::vector<bdlt::Date> >(
                                                                    allocator);
                                                                      // RETURN
      }
      case bdlmxxx::ElemType::BDEM_TIME_ARRAY: {
        return makeValuePtrInplaceWithAlloc<bsl::vector<bdlt::Time> >(
                                                                    allocator);
                                                                      // RETURN
      }
      case bdlmxxx::ElemType::BDEM_BOOL_ARRAY: {
        return makeValuePtrInplaceWithAlloc<bsl::vector<bool> >(allocator);
                                                                      // RETURN
      }
      case bdlmxxx::ElemType::BDEM_DATETIMETZ_ARRAY: {
        return makeValuePtrInplaceWithAlloc<bsl::vector<bdlt::DatetimeTz> >(
                                                                    allocator);
                                                                      // RETURN
      }
      case bdlmxxx::ElemType::BDEM_DATETZ_ARRAY: {
        return makeValuePtrInplaceWithAlloc<bsl::vector<bdlt::DateTz> >(
                                                                    allocator);
                                                                      // RETURN
      }
      case bdlmxxx::ElemType::BDEM_TIMETZ_ARRAY: {
        return makeValuePtrInplaceWithAlloc<bsl::vector<bdlt::TimeTz> >(
                                                                    allocator);
                                                                      // RETURN
      }
      case bdlmxxx::ElemType::BDEM_LIST: {
        return makeValuePtrInplaceWithAlloc<bdlmxxx::List>(allocator);
                                                                      // RETURN
      }
      case bdlmxxx::ElemType::BDEM_TABLE: {
        return makeValuePtrInplaceWithAlloc<bdlmxxx::Table>(allocator);
                                                                      // RETURN
      }
      case bdlmxxx::ElemType::BDEM_CHOICE: {
        return makeValuePtrInplaceWithAlloc<bdlmxxx::Choice>(allocator);
                                                                      // RETURN
      }
      case bdlmxxx::ElemType::BDEM_CHOICE_ARRAY: {
        return makeValuePtrInplaceWithAlloc<bdlmxxx::ChoiceArray>(allocator);
                                                                      // RETURN
      }
      case bdlmxxx::ElemType::BDEM_VOID:
      default: {
        return bsl::shared_ptr<void>();                               // RETURN
      }
    }
}

// PRIVATE MANIPULATORS
bsl::shared_ptr<const bdlmxxx::Schema> Aggregate::schemaPtr() const
{
    if (0 == d_schemaRep_p) {
        return bsl::shared_ptr<const bdlmxxx::Schema>();              // RETURN
    }
    d_schemaRep_p->acquireRef();
    return bsl::shared_ptr<const bdlmxxx::Schema>(d_aggregateRaw.schema(),
                                              d_schemaRep_p);
}

bsl::shared_ptr<const bdlmxxx::RecordDef> Aggregate::recordDefPtr() const
{
    if (0 == d_schemaRep_p) {
        return bsl::shared_ptr<const bdlmxxx::RecordDef>();           // RETURN
    }
    d_schemaRep_p->acquireRef();
    bsl::shared_ptr<const bdlmxxx::Schema> schema_sp(d_aggregateRaw.schema(),
                                                 d_schemaRep_p);
    return bsl::shared_ptr<const bdlmxxx::RecordDef>(
                                            schema_sp,
                                            d_aggregateRaw.recordConstraint());
}

bsl::shared_ptr<void> Aggregate::dataPtr() const
{
    if (0 == d_valueRep_p) {
        return bsl::shared_ptr<void>();                               // RETURN
    }
    d_valueRep_p->acquireRef();
    return bsl::shared_ptr<void>(const_cast<void *>(d_aggregateRaw.data()),
                                 d_valueRep_p);
}

void Aggregate::init(
                     const bsl::shared_ptr<const bdlmxxx::Schema>&  schemaPtr,
                     const bdlmxxx::RecordDef                      *recordDefPtr,
                     bdlmxxx::ElemType::Type                        elemType,
                     bslma::Allocator                          *basicAllocator)
{
    BSLS_ASSERT_SAFE(0 == d_schemaRep_p);
    BSLS_ASSERT_SAFE(0 == d_valueRep_p);
    BSLS_ASSERT_SAFE(0 == d_isTopLevelAggregateNullRep_p);

    if (bdlmxxx::ElemType::BDEM_VOID == elemType) {

        // Determine aggregate element type from record type

        elemType = bdlmxxx::RecordDef::BDEM_CHOICE_RECORD ==
                                                     recordDefPtr->recordType()
                 ? bdlmxxx::ElemType::BDEM_CHOICE
                 : bdlmxxx::ElemType::BDEM_LIST;
    }

    bslma::Allocator *allocator = bslma::Default::allocator(basicAllocator);

    bsl::shared_ptr<void> valuePtr;
    switch (elemType) {
      case bdlmxxx::ElemType::BDEM_LIST: {
        if (recordDefPtr->recordType() !=
                                        bdlmxxx::RecordDef::BDEM_SEQUENCE_RECORD) {
            *this = makeError(ErrorCode::BCEM_NOT_A_SEQUENCE,
                              "Attempt to create a LIST aggregate from "
                              "non-SEQUENCE record def \"%s\"",
                              recordName(recordDefPtr));
            return;                                                   // RETURN
        }
        bsl::shared_ptr<bdlmxxx::List> listPtr =
                            makeValuePtrInplaceWithAlloc<bdlmxxx::List>(allocator);
        bdlmxxx::SchemaAggregateUtil::initListDeep(listPtr.get(), *recordDefPtr);
        valuePtr = listPtr;
      } break;
      case bdlmxxx::ElemType::BDEM_TABLE: {
        if (recordDefPtr->recordType() !=
                                        bdlmxxx::RecordDef::BDEM_SEQUENCE_RECORD) {
            *this = makeError(ErrorCode::BCEM_NOT_A_SEQUENCE,
                              "Attempt to create a TABLE aggregate from "
                              "non-SEQUENCE record def \"%s\"",
                              recordName(recordDefPtr));
            return;                                                   // RETURN
        }
        bsl::shared_ptr<bdlmxxx::Table> tablePtr =
                           makeValuePtrInplaceWithAlloc<bdlmxxx::Table>(allocator);
        bdlmxxx::SchemaAggregateUtil::initTable(tablePtr.get(), *recordDefPtr);
        valuePtr = tablePtr;
      } break;
      case bdlmxxx::ElemType::BDEM_CHOICE: {
        if (recordDefPtr->recordType() != bdlmxxx::RecordDef::BDEM_CHOICE_RECORD) {
            *this = makeError(ErrorCode::BCEM_NOT_A_CHOICE,
                              "Attempt to create a CHOICE aggregate from "
                              "non-CHOICE record def \"%s\"",
                              recordName(recordDefPtr));
            return;                                                   // RETURN
        }
        bsl::shared_ptr<bdlmxxx::Choice> choicePtr =
                          makeValuePtrInplaceWithAlloc<bdlmxxx::Choice>(allocator);
        bdlmxxx::SchemaAggregateUtil::initChoice(choicePtr.get(), *recordDefPtr);
        valuePtr = choicePtr;
      } break;
      case bdlmxxx::ElemType::BDEM_CHOICE_ARRAY: {
        if (recordDefPtr->recordType() != bdlmxxx::RecordDef::BDEM_CHOICE_RECORD) {
            *this = makeError(ErrorCode::BCEM_NOT_A_CHOICE,
                              "Attempt to create a CHOICE_ARRAY aggregate "
                              "from non-CHOICE record def \"%s\"",
                              recordName(recordDefPtr));
            return;                                                   // RETURN
        }
        bsl::shared_ptr<bdlmxxx::ChoiceArray> choiceArrayPtr =
                     makeValuePtrInplaceWithAlloc<bdlmxxx::ChoiceArray>(allocator);
        bdlmxxx::SchemaAggregateUtil::initChoiceArray(choiceArrayPtr.get(),
                                                  *recordDefPtr);
        valuePtr = choiceArrayPtr;
      } break;
      default: {
          *this = makeError(ErrorCode::BCEM_NOT_A_RECORD,
                          "Attempt to specify a record definition when "
                          "constructing an object of non-aggregate type %s",
                          bdlmxxx::ElemType::toAscii(elemType));
        return;                                                       // RETURN
      }
    }

    d_aggregateRaw.setDataType(elemType);
    d_aggregateRaw.setSchema(schemaPtr.get());
    d_schemaRep_p = schemaPtr.rep();
    if (d_schemaRep_p) {
        d_schemaRep_p->acquireRef();
    }

    bcem_Aggregate_RepProctor schemaRepProctor(d_schemaRep_p);

    BSLS_ASSERT(valuePtr.get());

    d_aggregateRaw.setRecordDef(recordDefPtr);
    d_aggregateRaw.setData(valuePtr.get());
    d_valueRep_p = valuePtr.rep();
    d_valueRep_p->acquireRef();

    bcem_Aggregate_RepProctor valueRepProtctor(d_valueRep_p);

    // "nullness" data members are set in the constructors.
    bsl::shared_ptr<int> isNull_sp;
    isNull_sp.createInplace(allocator, 0);
    d_isTopLevelAggregateNullRep_p = isNull_sp.rep();
    d_isTopLevelAggregateNullRep_p->acquireRef();
    d_aggregateRaw.setTopLevelAggregateNullness(isNull_sp.get());

    valueRepProtctor.release();
    schemaRepProctor.release();
}

void Aggregate::init(
                  const bsl::shared_ptr<const bdlmxxx::RecordDef>&  recordDefPtr,
                  bdlmxxx::ElemType::Type                           elemType,
                  bslma::Allocator                             *basicAllocator)
{
    bsl::shared_ptr<const bdlmxxx::Schema> schemaPtr(recordDefPtr,
                                                 &recordDefPtr->schema());
    init(schemaPtr, recordDefPtr.get(), elemType, basicAllocator);
}

void Aggregate::init(
                     const bsl::shared_ptr<const bdlmxxx::Schema>&  schemaPtr,
                     const char                                *recName,
                     bdlmxxx::ElemType::Type                        elemType,
                     bslma::Allocator                          *basicAllocator)
{
    const bdlmxxx::RecordDef *record = schemaPtr->lookupRecord(recName);
    if (! record) {
        *this = makeError(ErrorCode::BCEM_NOT_A_RECORD,
                          "Unable to find record \"%s\" in schema",
                          recName);
        return;                                                       // RETURN
    }

    init(schemaPtr, record, elemType, basicAllocator);
}

// PRIVATE ACCESSORS

const Aggregate
Aggregate::makeError(ErrorCode::Code  errorCode,
                          const char           *msg, ...) const
{
    if (ErrorCode::BCEM_SUCCESS == errorCode || isError()) {

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

    ErrorAttributes error(errorCode, errorString);
    return makeError(error);
}

const Aggregate
Aggregate::makeError(const ErrorAttributes& errorDescription) const
{
    if (ErrorCode::BCEM_SUCCESS == errorDescription.code() || isError()) {

        // Return this object if success is being returned or this object is
        // already an error.

        return *this;                                                 // RETURN
    }

    bsl::shared_ptr<ErrorAttributes> errPtr;
    errPtr.createInplace(0, errorDescription);

    AggregateRaw errorValue;
    errorValue.setData(errPtr.get());

    return Aggregate(errorValue, 0, errPtr.rep(), 0);
}

// CREATORS
Aggregate::Aggregate()
: d_aggregateRaw()
, d_schemaRep_p(0)
, d_valueRep_p(0)
, d_isTopLevelAggregateNullRep_p(0)
{
}

Aggregate::Aggregate(const Aggregate& original)
: d_aggregateRaw(original.d_aggregateRaw)
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

Aggregate::Aggregate(const AggregateRaw&  aggregateRaw,
                               bslma::SharedPtrRep      *schemaRep,
                               bslma::SharedPtrRep      *valueRep,
                               bslma::SharedPtrRep      *topLevelNullRep)
: d_aggregateRaw(aggregateRaw)
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

Aggregate::~Aggregate()
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
    d_aggregateRaw.reset();
}

// MANIPULATORS
Aggregate& Aggregate::operator=(const Aggregate& rhs)
{
    if (this != &rhs) {
        d_aggregateRaw = rhs.d_aggregateRaw;

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

const Aggregate& Aggregate::reset()
{
    d_aggregateRaw.reset();

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
const Aggregate Aggregate::resize(int newSize) const
{
    ErrorAttributes errorDescription;
    if (0 == d_aggregateRaw.resize(&errorDescription,
                              static_cast<bsl::size_t>(newSize))) {
        return *this;                                                 // RETURN
    }
    else {
        return makeError(errorDescription);                           // RETURN
    }
}

const Aggregate Aggregate::insertItems(int pos, int numItems) const
{
    ErrorAttributes errorDescription;

    if (0 == d_aggregateRaw.insertItems(&errorDescription, pos, numItems)) {
        return *this;                                                 // RETURN
    }
    else {
        return makeError(errorDescription);                           // RETURN
    }
}

const Aggregate Aggregate::insertNullItems(int pos,
                                                     int numItems) const
{
    ErrorAttributes errorDescription;

    if (0 == d_aggregateRaw.insertNullItems(&errorDescription,
                                            pos,
                                            numItems)) {
        return *this;                                                 // RETURN
    }
    else {
        return makeError(errorDescription);                           // RETURN
    }
}

const Aggregate
Aggregate::makeSelectionByIndex(int index) const
{
    AggregateRaw    field;
    ErrorAttributes errorDescription;

    if (0 == d_aggregateRaw.makeSelectionByIndex(&field,
                                            &errorDescription,
                                            index)) {
        return Aggregate(field,
                              d_schemaRep_p,
                              d_valueRep_p,
                              d_isTopLevelAggregateNullRep_p);        // RETURN
    }
    else {
        return makeError(errorDescription);                           // RETURN
    }
}

const Aggregate Aggregate::removeItems(int pos, int numItems) const
{
    ErrorAttributes errorDescription;

    if (0 == d_aggregateRaw.removeItems(&errorDescription, pos, numItems)) {
        return *this;                                                 // RETURN
    }
    else {
        return makeError(errorDescription);                           // RETURN
    }
}

const Aggregate Aggregate::selection() const
{
    AggregateRaw    field;
    ErrorAttributes errorDescription;
    if (0 == d_aggregateRaw.selection(&field, &errorDescription)) {
        return Aggregate(field,
                              d_schemaRep_p,
                              d_valueRep_p,
                              d_isTopLevelAggregateNullRep_p);        // RETURN
    }
    else {
        return makeError(errorDescription);                           // RETURN
    }
}

const Aggregate
Aggregate::makeSelection(const char *newSelector) const
{
    AggregateRaw    field;
    ErrorAttributes errorDescription;

    if (0 == d_aggregateRaw.makeSelection(&field,
                                     &errorDescription,
                                     newSelector)) {
        return Aggregate(field,
                              d_schemaRep_p,
                              d_valueRep_p,
                              d_isTopLevelAggregateNullRep_p);        // RETURN
    }
    else {
        return makeError(errorDescription);                           // RETURN
    }
}

// MANIPULATORS
void Aggregate::swap(Aggregate& rhs)
{
    d_aggregateRaw.swap(rhs.d_aggregateRaw);
    bsl::swap(d_schemaRep_p, rhs.d_schemaRep_p);
    bsl::swap(d_valueRep_p, rhs.d_valueRep_p);
    bsl::swap(d_isTopLevelAggregateNullRep_p,
              rhs.d_isTopLevelAggregateNullRep_p);
}

// ACCESSORS
const Aggregate Aggregate::field(
                                        FieldSelector fieldSelector) const
{
    return fieldImp(false, fieldSelector);
}

const Aggregate
Aggregate::fieldImp(bool               makeNonNullFlag,
                         FieldSelector fieldSelector1,
                         FieldSelector fieldSelector2,
                         FieldSelector fieldSelector3,
                         FieldSelector fieldSelector4,
                         FieldSelector fieldSelector5,
                         FieldSelector fieldSelector6,
                         FieldSelector fieldSelector7,
                         FieldSelector fieldSelector8,
                         FieldSelector fieldSelector9,
                         FieldSelector fieldSelector10) const
{
    AggregateRaw    field;
    ErrorAttributes errorDescription;

    if (0 == d_aggregateRaw.getField(&field,
                                &errorDescription,
                                makeNonNullFlag,
                                fieldSelector1,
                                fieldSelector2,
                                fieldSelector3,
                                fieldSelector4,
                                fieldSelector5,
                                fieldSelector6,
                                fieldSelector7,
                                fieldSelector8,
                                fieldSelector9,
                                fieldSelector10)) {
        return Aggregate(field,
                              d_schemaRep_p,
                              d_valueRep_p,
                              d_isTopLevelAggregateNullRep_p);        // RETURN
    }
    else {
        return makeError(errorDescription);                           // RETURN
    }
}

const Aggregate Aggregate::field(
                                      FieldSelector fieldSelector1,
                                      FieldSelector fieldSelector2,
                                      FieldSelector fieldSelector3,
                                      FieldSelector fieldSelector4,
                                      FieldSelector fieldSelector5,
                                      FieldSelector fieldSelector6,
                                      FieldSelector fieldSelector7,
                                      FieldSelector fieldSelector8,
                                      FieldSelector fieldSelector9,
                                      FieldSelector fieldSelector10) const
{
    return fieldImp(false,
                    fieldSelector1,
                    fieldSelector2,
                    fieldSelector3,
                    fieldSelector4,
                    fieldSelector5,
                    fieldSelector6,
                    fieldSelector7,
                    fieldSelector8,
                    fieldSelector9,
                    fieldSelector10);
}

const Aggregate Aggregate::fieldById(int fieldId) const
{
    AggregateRaw    field;
    ErrorAttributes errorDescription;

    if (0 == d_aggregateRaw.fieldById(&field, &errorDescription, fieldId)) {
        return Aggregate(field,
                              d_schemaRep_p,
                              d_valueRep_p,
                              d_isTopLevelAggregateNullRep_p);        // RETURN
    }
    else {
        return makeError(errorDescription);                           // RETURN
    }
}

const Aggregate Aggregate::fieldByIndex(int fieldIndex) const
{
    AggregateRaw    field;
    ErrorAttributes errorDescription;
    if (0 == d_aggregateRaw.fieldByIndex(&field,
                                         &errorDescription,
                                         fieldIndex)) {
        return Aggregate(field,
                              d_schemaRep_p,
                              d_valueRep_p,
                              d_isTopLevelAggregateNullRep_p);        // RETURN
    }
    else {
        return makeError(errorDescription);                           // RETURN
    }
}

const Aggregate Aggregate::anonymousField(int n) const
{
    AggregateRaw    field;
    ErrorAttributes errorDescription;
    if (0 == d_aggregateRaw.anonymousField(&field, &errorDescription, n)) {
        return Aggregate(field,
                              d_schemaRep_p,
                              d_valueRep_p,
                              d_isTopLevelAggregateNullRep_p);        // RETURN
    }
    else {
        return makeError(errorDescription);                           // RETURN
    }
}

const Aggregate Aggregate::anonymousField() const
{
    if (d_aggregateRaw.recordConstraint()
     && 1 < d_aggregateRaw.recordConstraint()->numAnonymousFields()) {

        // Only report error if there are more than one anonymous fields.  The
        // case where there are zero anonymous fields is already handled by
        // the single-argument call to 'anonymousField', below.

        return makeError(ErrorCode::BCEM_AMBIGUOUS_ANON,
                         "anonymousField() called for object with multiple "
                         "anonymous fields.  Cannot pick one.");      // RETURN
    }

    return anonymousField(0);
}

bdlmxxx::ElemType::Type
Aggregate::fieldType(FieldSelector fieldSelector1,
                          FieldSelector fieldSelector2,
                          FieldSelector fieldSelector3,
                          FieldSelector fieldSelector4,
                          FieldSelector fieldSelector5,
                          FieldSelector fieldSelector6,
                          FieldSelector fieldSelector7,
                          FieldSelector fieldSelector8,
                          FieldSelector fieldSelector9,
                          FieldSelector fieldSelector10) const
{
    return fieldImp(false,
                    fieldSelector1,
                    fieldSelector2,
                    fieldSelector3,
                    fieldSelector4,
                    fieldSelector5,
                    fieldSelector6,
                    fieldSelector7,
                    fieldSelector8,
                    fieldSelector9,
                    fieldSelector10).dataType();
}

bdlmxxx::ElemType::Type Aggregate::fieldTypeById(int fieldId) const
{
    return fieldById(fieldId).dataType();
}

bdlmxxx::ElemType::Type Aggregate::fieldTypeByIndex(int index) const
{
    return fieldByIndex(index).dataType();
}

bdlmxxx::ElemRef Aggregate::fieldRef(FieldSelector fieldSelector1,
                                      FieldSelector fieldSelector2,
                                      FieldSelector fieldSelector3,
                                      FieldSelector fieldSelector4,
                                      FieldSelector fieldSelector5,
                                      FieldSelector fieldSelector6,
                                      FieldSelector fieldSelector7,
                                      FieldSelector fieldSelector8,
                                      FieldSelector fieldSelector9,
                                      FieldSelector fieldSelector10) const
{
    return fieldImp(true,
                    fieldSelector1,
                    fieldSelector2,
                    fieldSelector3,
                    fieldSelector4,
                    fieldSelector5,
                    fieldSelector6,
                    fieldSelector7,
                    fieldSelector8,
                    fieldSelector9,
                    fieldSelector10).asElemRef();
}

bdlmxxx::ElemRef Aggregate::fieldRefById(int fieldId) const
{
    return fieldById(fieldId).asElemRef();
}

bdlmxxx::ElemRef Aggregate::fieldRefByIndex(int index) const
{
    return fieldByIndex(index).asElemRef();
}

const Aggregate Aggregate::operator[](int index) const
{
    return fieldImp(false, index);  // TBD pass true?
}

const Aggregate Aggregate::makeSelectionById(int id) const
{
    AggregateRaw    field;
    ErrorAttributes errorDescription;

    if (0 == d_aggregateRaw.makeSelectionById(&field,
                                         &errorDescription,
                                         id)) {
        return Aggregate(field,
                              d_schemaRep_p,
                              d_valueRep_p,
                              d_isTopLevelAggregateNullRep_p);        // RETURN
    }
    else {
        return makeError(errorDescription);                           // RETURN
    }
}
}  // close package namespace

#ifndef BDE_OMIT_INTERNAL_DEPRECATED

namespace bdlaggxxx {bool Aggregate::isUnset() const
{
    bool isUnsetFlag;
    switch (dataType()) {
      case bdlmxxx::ElemType::BDEM_VOID: {
        isUnsetFlag = true;
      } break;
      case bdlmxxx::ElemType::BDEM_TABLE: {
        const bdlmxxx::Table *table = (const bdlmxxx::Table *) data();
        if (recordConstraint()) {
            isUnsetFlag = 0 == table->numRows();
        }
        else {
            isUnsetFlag = 0 == table->numRows() && 0 == table->numColumns();
        }
      } break;
      case bdlmxxx::ElemType::BDEM_CHOICE: {
        const bdlmxxx::Choice *choice = (const bdlmxxx::Choice *) data();
        if (recordConstraint()) {
            isUnsetFlag = choice->selector() < 0;
        }
        else {
            isUnsetFlag = choice->selector() < 0
                       && 0 == choice->numSelections();
        }
      } break;
      case bdlmxxx::ElemType::BDEM_CHOICE_ARRAY_ITEM: {
        const bdlmxxx::ChoiceArrayItem *item = (const bdlmxxx::ChoiceArrayItem *) data();
        isUnsetFlag = item->selector() < 0;
      } break;
      case bdlmxxx::ElemType::BDEM_CHOICE_ARRAY: {
        const bdlmxxx::ChoiceArray *choiceArray = (const bdlmxxx::ChoiceArray *) data();
        if (recordConstraint()) {
            isUnsetFlag = 0 == choiceArray->length();
        }
        else {
            isUnsetFlag = 0 == choiceArray->length()
                       && 0 == choiceArray->numSelections();
        }
      } break;
      case bdlmxxx::ElemType::BDEM_ROW:
      case bdlmxxx::ElemType::BDEM_LIST:
      default: {
        const bdlmxxx::Descriptor *descriptor =
                                bdlmxxx::ElemAttrLookup::lookupTable()[dataType()];
        isUnsetFlag = descriptor->isUnset(data());
      }
    }
    return isUnsetFlag;
}
}  // close package namespace
#endif  // BDE_OMIT_INTERNAL_DEPRECATED

namespace bdlaggxxx {
const Aggregate
Aggregate::clone(bslma::Allocator *basicAllocator) const
{
    Aggregate returnVal(this->cloneData(basicAllocator));

    if (! d_aggregateRaw.schema()) {
        return returnVal;                                             // RETURN
    }

    // Clone the schema

    bsl::shared_ptr<bdlmxxx::Schema> schemaClone;
    schemaClone.createInplace(basicAllocator,
                              *d_aggregateRaw.schema(),
                              basicAllocator);

    returnVal.d_aggregateRaw.setSchema(schemaClone.get());

    bslma::SharedPtrRep *schemaCloneRep = schemaClone.rep();
    bsl::swap(returnVal.d_schemaRep_p, schemaCloneRep);
    schemaCloneRep->releaseRef();

    returnVal.d_schemaRep_p->acquireRef();

    if (d_aggregateRaw.recordConstraint()) {

        // Set the clone's record pointer to point into the cloned schema

        int recordIndex = d_aggregateRaw.recordConstraint()->recordIndex();
        returnVal.d_aggregateRaw.setRecordDef(
                                           &schemaClone->record(recordIndex));
    }

    if (d_aggregateRaw.fieldDef()) {

        // The field spec is not null -- find the field spec in the original
        // schema.
        // TBD: The only way to find the field spec in the schema is to do a
        // linear search through all of the fields of all of the recordDefs.
        // Can we find a more efficient way to do this?

        for (int recIndex = 0;
             recIndex < d_aggregateRaw.schema()->numRecords();
             ++recIndex) {
            const bdlmxxx::RecordDef& rec =
                                     d_aggregateRaw.schema()->record(recIndex);
            for (int fieldIndex = 0;
                 fieldIndex < rec.numFields();
                 ++fieldIndex) {
                const bdlmxxx::FieldDef& field = rec.field(fieldIndex);
                if (&field == d_aggregateRaw.fieldDef()) {

                    // Point the field spec in the clone to the corresponding
                    // fieldspec within the cloned schema.

                    returnVal.d_aggregateRaw.setFieldDef(
                          &(returnVal.d_aggregateRaw.schema()->
                               record(recIndex).field(fieldIndex)));
                    return returnVal;                                 // RETURN
                } // end if (match)
            } // end for (each field in record)
        } // end for (each record in original schema)
    } // end if (field spec is not static)

    return returnVal;
}

const Aggregate
Aggregate::cloneData(bslma::Allocator *basicAllocator) const
{
    bsl::shared_ptr<void> valuePtr;
    bslma::Allocator *allocator = bslma::Default::allocator(basicAllocator);

    switch (dataType()) {
      case bdlmxxx::ElemType::BDEM_ROW: {

        // ROW is a special case.  Because a row has no copy constructor, it
        // is necessary to make a list containing a copy of the row, then
        // return an aggregate that refers to the copy within the list.

        // Construct a parent list

        bsl::shared_ptr<void> parent =
                            makeValuePtrInplaceWithAlloc<bdlmxxx::List>(allocator);
        bdlmxxx::List& parentList = *(bdlmxxx::List *)parent.get();

        // Perform a row-to-list assignment to make a copy of this row.

        parentList = *(const bdlmxxx::Row *)data();

        // Get a shared pointer to the row within the parent list

        valuePtr.loadAlias(parent, &parentList.row());
      } break;
      case bdlmxxx::ElemType::BDEM_CHOICE_ARRAY_ITEM: {

        // CHOICE_ARRAY_ITEM is a special case.  Because a choice array item
        // has no copy constructor, it is necessary to make a choice
        // containing a copy of the item, then return an aggregate that
        // refers to the copy within the choice.

        // Construct a parent choice

        bsl::shared_ptr<void> parent =
                          makeValuePtrInplaceWithAlloc<bdlmxxx::Choice>(allocator);
        bdlmxxx::Choice& parentChoice = *(bdlmxxx::Choice *)parent.get();

        // Perform an item-to-choice assignment to make a copy of this item.

        parentChoice = *(const bdlmxxx::ChoiceArrayItem *)data();

        // Get a shared pointer to the item within the parent choice

        valuePtr.loadAlias(parent, &parentChoice.item());
      } break;
      case bdlmxxx::ElemType::BDEM_VOID: {
        if (isError()) {
            valuePtr = makeValuePtrInplaceWithAlloc<ErrorAttributes>(
                                                                    allocator);
            *static_cast<ErrorAttributes *>(valuePtr.get()) =
                            *static_cast<const ErrorAttributes *>(data());
        }
        else {

            // Return an empty aggregate

            return Aggregate();                                       // RETURN
        }
      } break;
      default: {

        // Make a copy of the value.

        const bdlmxxx::Descriptor *descriptor =
                                bdlmxxx::ElemAttrLookup::lookupTable()[dataType()];

        valuePtr = makeValuePtr(dataType(), allocator);
        descriptor->assign(valuePtr.get(), data());
      } break;
    }

    Aggregate returnVal(*this);
    returnVal.d_aggregateRaw.setData(valuePtr.get());

    bslma::SharedPtrRep *valuePtrRep = valuePtr.rep();
    bsl::swap(returnVal.d_valueRep_p, valuePtrRep);
    if (valuePtrRep) {
        valuePtrRep->releaseRef();
    }
    returnVal.d_valueRep_p->acquireRef();

    // Clone is a top-level aggregate.

    returnVal.d_aggregateRaw.clearParent();

    if (bdlmxxx::ElemType::BDEM_VOID != dataType()) {
        bsl::shared_ptr<int> isNull_sp;
        isNull_sp.createInplace(allocator, isNul2());

        returnVal.d_aggregateRaw.setTopLevelAggregateNullness(isNull_sp.get());

        bslma::SharedPtrRep *isNullRep = isNull_sp.rep();
        bsl::swap(returnVal.d_isTopLevelAggregateNullRep_p, isNullRep);
        if (isNullRep) {
            isNullRep->releaseRef();
        }
        returnVal.d_isTopLevelAggregateNullRep_p->acquireRef();
    }

    return returnVal;
}
}  // close package namespace

}  // close enterprise namespace

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2006, 2011
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ----------------------------------
