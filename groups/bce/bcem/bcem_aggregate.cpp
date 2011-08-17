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

#ifdef TEST
// These dependencies will cause the test driver to recompile when the BER
// encoder or decoder change.
#include <bdem_berencoder.h>
#include <bdem_berdecoder.h>
#endif

#include <stdio.h>     // 'v/snprintf' (not always declared in <bsl_cstdio.h>).
#if defined(BSLS_PLATFORM__CMP_MSVC)
#   define snprintf _snprintf // MSVC names snprintf _snprintf
#endif

#include <bsl_cstdarg.h>
#include <bsl_cstdlib.h>         // 'strtol'
#include <bsl_cstring.h>         // 'memcpy'
#include <bsl_iterator.h>

namespace BloombergLP {

namespace {

// DATA
static int s_voidNullnessWord = 1;

// TYPES
typedef bsls_Types::Int64          Int64;
typedef bcem_Aggregate_NameOrIndex NameOrIndex;

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

                      // =====================
                      // class ArrayItemEraser
                      // =====================

class ArrayItemEraser {
    // Function object to erase items from a vector containing any item type.

    // DATA
    int d_pos;       // index position in array at which items are erased
    int d_numItems;  // number of items to remove

    // NOT IMPLEMENTED
    ArrayItemEraser(const ArrayItemEraser&);
    ArrayItemEraser& operator=(const ArrayItemEraser&);

  public:
    // CREATORS
    ArrayItemEraser(int pos, int numItems)
    : d_pos(pos)
    , d_numItems(numItems)
    {
    }

    // MANIPULATORS
    template <typename ARRAYTYPE>
    int operator()(ARRAYTYPE *array)
    {
        if (d_pos + d_numItems > (int)array->size()) {
            return -1;
        }

        typename ARRAYTYPE::iterator start = array->begin();
        bsl::advance(start, d_pos);
        typename ARRAYTYPE::iterator finish = start;
        bsl::advance(finish, d_numItems);
        array->erase(start, finish);

        return 0;
    }
};

                      // ==========================
                      // class NullTerminatedString
                      // ==========================

class NullTerminatedString {
    // This 'class' provides access to a null-terminated string that
    // corresponds to a 'const char *' and a length supplied at construction.
    // Specifically, the 'operator const char *' method returns a pointer to
    // a copy of that string that is guaranteed to be null-terminated.  The
    // returned 'const char *' is valid only for the lifetime of the
    // 'NullTerminatedString' object.  The statically-sized buffer member of
    // this class is expected to be large enough to accommodate most, if not
    // all, practical uses of this class without incurring a memory allocation.

    // DATA
    char             d_buffer[128];  // buffer large enough for *most* uses

    char            *d_string_p;     // address of 'd_buffer', or allocated
                                     // string if 128 is not sufficient

    bslma_Allocator *d_allocator_p;  // memory allocator (held, not owned)

    // NOT IMPLEMENTED
    NullTerminatedString(const NullTerminatedString&);
    NullTerminatedString& operator=(const NullTerminatedString&);

  public:
    // CREATORS
    NullTerminatedString(const char      *string,
                         int              length,
                         bslma_Allocator *basicAllocator = 0)
    : d_string_p(d_buffer)
    , d_allocator_p(bslma_Default::allocator(basicAllocator))
    {
        if (length >= (int)sizeof d_buffer) {
            d_string_p = (char *)d_allocator_p->allocate(length + 1);
        }

        bsl::memcpy(d_string_p, string, length);
        d_string_p[length] = '\0';
    }

    ~NullTerminatedString()
    {
        if (d_string_p != d_buffer) {
            d_allocator_p->deallocate(d_string_p);
        }
    }

    // ACCESSORS
    operator const char*() const {
        return d_string_p;
    }
};

                        //------------------
                        // class ErrorRecord
                        //------------------

class ErrorRecord {
    // This 'class' provides an error record with which to populate an error
    // aggregate with information describing the error.

    // DATA
    int         d_errorCode;     // 'bcem_Aggregate::BCEM_ERR_UNKNOWN_ERROR',
                                 // etc.

    bsl::string d_errorMessage;  // detailed error message text

    // NOT IMPLEMENTED
    ErrorRecord(const ErrorRecord&);

  public:
    // CREATORS
    ErrorRecord(bslma_Allocator *basicAllocator = 0)
    : d_errorCode(0)
    , d_errorMessage(basicAllocator)
    {
    }

    ErrorRecord(int              errorCode,
                const char      *errorMsg,
                bslma_Allocator *basicAllocator = 0)
    : d_errorCode(errorCode)
    , d_errorMessage(errorMsg, basicAllocator)
    {
    }

    ~ErrorRecord()
    {
    }

    // MANIPULATORS
    ErrorRecord& operator=(const ErrorRecord& rhs)
    {
        d_errorCode    = rhs.d_errorCode;
        d_errorMessage = rhs.d_errorMessage;

        return *this;
    }

    // ACCESSORS
    int errorCode() const
    {
        return d_errorCode;
    }

    const bsl::string& errorMessage() const
    {
        return d_errorMessage;
    }
};

}  // close unnamed namespace

                        //---------------------------------
                        // local struct bcem_Aggregate_Util
                        //---------------------------------

// CLASS METHODS
const char *bcem_Aggregate_Util::enumerationName(
                                            const bdem_EnumerationDef *enumDef)
{
    if (! enumDef) {
        return "(unconstrained)";
    }

    const char *ret =
                enumDef->schema().enumerationName(enumDef->enumerationIndex());

    return ret ? ret : "(anonymous)";
}

const char *bcem_Aggregate_Util::recordName(const bdem_RecordDef *recordDef)
{
    if (! recordDef) {
        return "(unconstrained)";
    }

    const char *ret = recordDef->schema().recordName(recordDef->recordIndex());
    return ret ? ret : "(anonymous)";
}

// The following 'isConformant' methods are not 'inline' to avoid a
// header dependency on 'bdem_SchemaAggregateUtil'.

bool bcem_Aggregate_Util::isConformant(const bdem_ConstElemRef *object,
                                       const bdem_RecordDef    *recordDef)
{
    bool result = bcem_Aggregate_Util::isConformant(object->data(),
                                                    object->type(),
                                                    recordDef);

    if (!result && object->isNull()) {
        // Conformance fails for aggregate types.

        bdem_ElemType::Type type = object->type();

        if (bdem_RecordDef::BDEM_CHOICE_RECORD == recordDef->recordType()) {
            return bdem_ElemType::isChoiceType(type);                 // RETURN
        }

        return bdem_ElemType::BDEM_ROW   == type
            || bdem_ElemType::BDEM_LIST  == type
            || bdem_ElemType::BDEM_TABLE == type;                     // RETURN
    }

    return result;
}

bool bcem_Aggregate_Util::isConformant(const bdem_Row       *object,
                                       const bdem_RecordDef *recordDef)
{
    return recordDef
         ? bdem_SchemaAggregateUtil::isRowConformant(*object, *recordDef)
         : true;
}

bool bcem_Aggregate_Util::isConformant(const bdem_List      *object,
                                       const bdem_RecordDef *recordDef)
{
    return recordDef
         ? bdem_SchemaAggregateUtil::isListConformant(*object, *recordDef)
         : true;
}

bool bcem_Aggregate_Util::isConformant(const bdem_Table     *object,
                                       const bdem_RecordDef *recordDef)
{
    return recordDef
         ? bdem_SchemaAggregateUtil::isTableConformant(*object, *recordDef)
         : true;
}

bool bcem_Aggregate_Util::isConformant(const bdem_Choice    *object,
                                       const bdem_RecordDef *recordDef)
{
    return recordDef
         ? bdem_SchemaAggregateUtil::isChoiceConformant(*object, *recordDef)
         : true;
}

bool bcem_Aggregate_Util::isConformant(const bdem_ChoiceArrayItem *object,
                                       const bdem_RecordDef       *recordDef)
{
    return recordDef
         ? bdem_SchemaAggregateUtil::isChoiceArrayItemConformant(*object,
                                                                 *recordDef)
         : true;
}

bool bcem_Aggregate_Util::isConformant(const bdem_ChoiceArray *object,
                                       const bdem_RecordDef   *recordDef)
{
    return recordDef
         ? bdem_SchemaAggregateUtil::isChoiceArrayConformant(*object,
                                                             *recordDef)
         : true;
}

bool bcem_Aggregate_Util::isConformant(const bcem_Aggregate *object,
                                       const bdem_RecordDef *recordDef)
{
    if (object->recordConstraint() == recordDef) {
        return true;  // Record definitions are identical.
    }
    else if (! bdem_ElemType::isAggregateType(object->dataType())) {
        // A non-aggregate conforms only if recordDef is null.
        return 0 == recordDef;
    }
    else if (! recordDef) {
        // If recordDef is null, then the target is unconstrained.
        return true;
    }
    else if (object->recordConstraint()) {
        // Both value and 'object' are constrained.  Make sure the object
        // has a compatible schema.
        return bdem_SchemaUtil::areStructurallyEquivalent(
                                                   *object->recordConstraint(),
                                                   *recordDef);
    }

    // If got here, then 'recordDef' is non-null and
    // 'object->recordConstraint()' is null.  Check that (unconstrained) value
    // in 'object' conforms to 'recordDef'.

    return bcem_Aggregate_Util::isConformant(object->data(),
                                             object->dataType(),
                                             recordDef);
}

bool bcem_Aggregate_Util::isConformant(const void           *object,
                                       bdem_ElemType::Type   type,
                                       const bdem_RecordDef *recordDef)
{
    bool result;

    if (recordDef) {
        switch (type) {
          case bdem_ElemType::BDEM_LIST: {
            result = bcem_Aggregate_Util::isConformant((bdem_List*)object,
                                                       recordDef);
          } break;
          case bdem_ElemType::BDEM_ROW: {
            result = bcem_Aggregate_Util::isConformant((bdem_Row*)object,
                                                       recordDef);
          } break;
          case bdem_ElemType::BDEM_TABLE: {
            result = bcem_Aggregate_Util::isConformant((bdem_Table*)object,
                                                       recordDef);
          } break;
          case bdem_ElemType::BDEM_CHOICE: {
            result = bcem_Aggregate_Util::isConformant((bdem_Choice*)object,
                                                       recordDef);
          } break;
          case bdem_ElemType::BDEM_CHOICE_ARRAY_ITEM: {
            result = bcem_Aggregate_Util::isConformant(
                                                 (bdem_ChoiceArrayItem*)object,
                                                 recordDef);
          } break;
          case bdem_ElemType::BDEM_CHOICE_ARRAY: {
            result = bcem_Aggregate_Util::isConformant(
                                                     (bdem_ChoiceArray*)object,
                                                     recordDef);
          } break;
          default: {
            result = false;
          } break;
        }
    }
    else {
        result = true;
    }

    return result;
}

bcema_SharedPtr<void>
bcem_Aggregate_Util::makeValuePtr(bdem_ElemType::Type  type,
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
int
bcem_Aggregate::assignToNillableScalarArrayImp(const bdem_ElemRef& value) const
{
    bdem_ElemType::Type srcType = value.type();

    // Check conformance of value against this aggregate.
    if (bdem_ElemType::BDEM_TABLE == srcType) {
        return assignToNillableScalarArray(value.theTable());         // RETURN
    }

    bdem_ElemType::Type baseType = bdem_ElemType::fromArrayType(srcType);
    if (!bdem_ElemType::isScalarType(baseType)
     || baseType != d_recordDef->field(0).elemType()) {
        return BCEM_ERR_NON_CONFORMANT;                               // RETURN
    }

    if (value.isNull()) {
        makeNull();
        return 0;                                                     // RETURN
    }

    bcem_Aggregate_ArraySizer  sizer;
    void                      *srcData = value.dataRaw();
    const int                  length  = bcem_Aggregate_Util::visitArray(
                                                                       srcData,
                                                                       srcType,
                                                                       &sizer);
    this->resize(length);
    bdem_Table            *dstTable     = (bdem_Table *) d_value.ptr();
    const bdem_Descriptor *baseTypeDesc =
                                  bdem_ElemAttrLookup::lookupTable()[baseType];

    for (int i = 0; i < length; ++i) {
        bcem_Aggregate_ArrayIndexer indexer(i);
        bcem_Aggregate_Util::visitArray(srcData, srcType, &indexer);
        baseTypeDesc->assign(dstTable->theModifiableRow(i)[0].data(),
                             indexer.data());
    }
    return 0;
}

int bcem_Aggregate::assignToNillableScalarArrayImp(
                                          const bdem_ConstElemRef& value) const
{
    bdem_ElemType::Type srcType = value.type();

    // Check conformance of value against this aggregate.
    if (bdem_ElemType::BDEM_TABLE == srcType) {
        return assignToNillableScalarArray(value.theTable());         // RETURN
    }

    bdem_ElemType::Type baseType = bdem_ElemType::fromArrayType(srcType);
    if (!bdem_ElemType::isScalarType(baseType)
     || baseType != d_recordDef->field(0).elemType()) {
        return BCEM_ERR_NON_CONFORMANT;                               // RETURN
    }

    if (value.isNull()) {
        makeNull();
        return 0;                                                     // RETURN
    }

    bcem_Aggregate_ArraySizer  sizer;
    void                      *srcData = const_cast<void *>(value.data());
    const int                  length  = bcem_Aggregate_Util::visitArray(
                                                                       srcData,
                                                                       srcType,
                                                                       &sizer);
    this->resize(length);
    bdem_Table            *dstTable     = (bdem_Table *)d_value.ptr();
    const bdem_Descriptor *baseTypeDesc =
                                  bdem_ElemAttrLookup::lookupTable()[baseType];

    for (int i = 0; i < length; ++i) {
        bcem_Aggregate_ArrayIndexer indexer(i);
        bcem_Aggregate_Util::visitArray(srcData, srcType, &indexer);
        baseTypeDesc->assign(dstTable->theModifiableRow(i)[0].data(),
                             indexer.data());
    }
    return 0;
}

bcem_Aggregate
bcem_Aggregate::toEnum(const int& value, bslmf_MetaInt<0>) const
{
    const bdem_EnumerationDef *enumDef  = enumerationConstraint();
    const char                *enumName = enumDef->lookupName(value);

    if (bdetu_Unset<int>::unsetValue() != value && !enumName) {
        // Failed lookup
        return makeError(BCEM_ERR_BAD_ENUMVALUE, "Attempt to set enumerator "
                         "ID %d in enumeration \"%s\"",
                         value, bcem_Aggregate_Util::enumerationName(enumDef));
    }

    // If we got here, we're either a (1) top-level aggregate, (2) CHOICE or
    // CHOICE_ARRAY_ITEM that has been selected (hence, non-null), or (3) an
    // item in a ROW.

    if (bdem_ElemType::BDEM_INT == d_dataType) {
        asElemRef().theModifiableInt() = value;
    }
    else {
        asElemRef().theModifiableString() = enumName ? enumName : "";
    }

    return *this;
}

bcem_Aggregate
bcem_Aggregate::toEnum(const char *value, bslmf_MetaInt<1>) const
{
    const bdem_EnumerationDef *enumDef = enumerationConstraint();
    const int enumId = value ? enumDef->lookupId(value)
                             : bdetu_Unset<int>::unsetValue();

    if (bdetu_Unset<int>::isUnset(enumId) && 0 != value && 0 != value[0]) {
        return makeError(BCEM_ERR_BAD_ENUMVALUE, "Attempt to set enumerator "
                         "name %s in enumeration \"%s\"",
                         value, bcem_Aggregate_Util::enumerationName(enumDef));
    }

    // If we got here, we're either a (1) top-level aggregate, (2) CHOICE or
    // CHOICE_ARRAY_ITEM that has been selected (hence, non-null), or (3) an
    // item in a ROW.

    if (bdem_ElemType::BDEM_INT == d_dataType) {
        asElemRef().theModifiableInt() = enumId;
    }
    else {
        asElemRef().theModifiableString() = value ? value : "";
    }

    return *this;
}

bcem_Aggregate
bcem_Aggregate::toEnum(const bdem_ConstElemRef& value, bslmf_MetaInt<1>) const
{
    int intValue;
    switch (value.type()) {
      case bdem_ElemType::BDEM_CHAR: {
        intValue = value.theChar();
      } break;
      case bdem_ElemType::BDEM_SHORT: {
        intValue = value.theShort();
      } break;
      case bdem_ElemType::BDEM_INT: {
        intValue = value.theInt();
      } break;
      case bdem_ElemType::BDEM_INT64: {
        intValue = (int) value.theInt64();
      } break;
      case bdem_ElemType::BDEM_FLOAT: {
        intValue = (int) value.theFloat();
      } break;
      case bdem_ElemType::BDEM_DOUBLE: {
        intValue = (int) value.theDouble();
      } break;
      case bdem_ElemType::BDEM_BOOL: {
        intValue = value.theBool();
      } break;
      case bdem_ElemType::BDEM_STRING: {
        return value.isNull()
             ? makeNull()
             : toEnum(value.theString().c_str(), bslmf_MetaInt<1>());
      } break;
      default: {
        return makeError(
                BCEM_ERR_BAD_CONVERSION,
                "Invalid conversion from %s to enumeration \"%s\"",
                bdem_ElemType::toAscii(value.type()),
                bcem_Aggregate_Util::enumerationName(enumerationConstraint()));
      }
    }

    if (value.isNull()) {
        return makeNull();
    }

    // Got here if value is numeric and has been converted to int.

    return toEnum(intValue, bslmf_MetaInt<0>());
}

void bcem_Aggregate::init(
    const bcema_SharedPtr<const bdem_Schema>&  schemaPtr,
    const bdem_RecordDef                      *recordDefPtr,
    bdem_ElemType::Type                        elemType,
    bslma_Allocator                           *basicAllocator)
{
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
            *this = makeError(BCEM_ERR_NOT_A_SEQUENCE,
                              "Attempt to create a LIST aggregate from "
                              "non-SEQUENCE record def \"%s\"",
                              bcem_Aggregate_Util::recordName(recordDefPtr));
            return;
        }
        bcema_SharedPtr<bdem_List> listPtr =
                            makeValuePtrInplaceWithAlloc<bdem_List>(allocator);
        bdem_SchemaAggregateUtil::initListDeep(listPtr.ptr(), *recordDefPtr);
        valuePtr = listPtr;
      } break;
      case bdem_ElemType::BDEM_TABLE: {
        if (recordDefPtr->recordType() !=
            bdem_RecordDef::BDEM_SEQUENCE_RECORD) {
            *this = makeError(BCEM_ERR_NOT_A_SEQUENCE,
                              "Attempt to create a TABLE aggregate from "
                              "non-SEQUENCE record def \"%s\"",
                              bcem_Aggregate_Util::recordName(recordDefPtr));
            return;
        }
        bcema_SharedPtr<bdem_Table> tablePtr =
                           makeValuePtrInplaceWithAlloc<bdem_Table>(allocator);
        bdem_SchemaAggregateUtil::initTable(tablePtr.ptr(), *recordDefPtr);
        valuePtr = tablePtr;
      } break;
      case bdem_ElemType::BDEM_CHOICE: {
        if (recordDefPtr->recordType() !=
            bdem_RecordDef::BDEM_CHOICE_RECORD) {
            *this = makeError(BCEM_ERR_NOT_A_CHOICE,
                              "Attempt to create a CHOICE aggregate from "
                              "non-CHOICE record def \"%s\"",
                              bcem_Aggregate_Util::recordName(recordDefPtr));
            return;
        }
        bcema_SharedPtr<bdem_Choice> choicePtr =
                          makeValuePtrInplaceWithAlloc<bdem_Choice>(allocator);
        bdem_SchemaAggregateUtil::initChoice(choicePtr.ptr(), *recordDefPtr);
        valuePtr = choicePtr;
      } break;
      case bdem_ElemType::BDEM_CHOICE_ARRAY: {
        if (recordDefPtr->recordType() != bdem_RecordDef::BDEM_CHOICE_RECORD) {
            *this = makeError(BCEM_ERR_NOT_A_CHOICE,
                              "Attempt to create a CHOICE_ARRAY aggregate "
                              "from non-CHOICE record def \"%s\"",
                              bcem_Aggregate_Util::recordName(recordDefPtr));
            return;
        }
        bcema_SharedPtr<bdem_ChoiceArray> choiceArrayPtr =
                     makeValuePtrInplaceWithAlloc<bdem_ChoiceArray>(allocator);
        bdem_SchemaAggregateUtil::initChoiceArray(choiceArrayPtr.ptr(),
                                                  *recordDefPtr);
        valuePtr = choiceArrayPtr;
      } break;
      default: {
        *this = makeError(BCEM_ERR_NOT_A_RECORD,
                          "Attempt to specify a record definition when "
                          "constructing an object of non-aggregate type %s",
                          bdem_ElemType::toAscii(elemType));
        return;
      }
    }

    d_dataType  = elemType;
    d_schema    = schemaPtr;
    d_recordDef = recordDefPtr;
    d_fieldDef  = 0;
    d_value     = valuePtr;

    // "nullness" data members are set in the constructors.
    d_isTopLevelAggregateNull.createInplace(allocator, 0);
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
        *this = makeError(BCEM_ERR_NOT_A_RECORD,
                          "Unable to find record \"%s\" in schema", recName);
        return;
    }

    init(schemaPtr, record, elemType, basicAllocator);
}

bool bcem_Aggregate::descendIntoField(bcem_Aggregate_NameOrIndex fieldOrIdx,
                                      bool                       resetNullBit)
{
    if (fieldOrIdx.isEmpty()) {
        return false;
    }
    else if (fieldOrIdx.isName()) {
        return descendIntoFieldByName(fieldOrIdx.name());
    }
    else {  // 'fieldOrIdx.isIndex()'
        return descendIntoArrayItem(fieldOrIdx.index(), resetNullBit);
    }
}

bool bcem_Aggregate::descendIntoFieldByName(const char *fieldName)
{
    bool                  ret          = false;
    int                   fieldIndex   = -1;
    const bdem_RecordDef *parentRecDef = 0;

    do {
        if ((bdem_ElemType::BDEM_CHOICE            == d_dataType
          || bdem_ElemType::BDEM_CHOICE_ARRAY_ITEM == d_dataType)
          && ! (fieldName && fieldName[0])) {
            // Empty field name given to choice.  Descend into current
            // selection.  In order to descend into anonymous choices, we need
            // to get the real field index, not the
            // 'bdetu_Unset<int>::unsetValue()' place holder for "current
            // selection".
            fieldIndex = selectorIndex();
        }
        else {
            bdeut_NullableValue<bcem_Aggregate> errorAggregate;
            if (getFieldIndex(&fieldIndex,
                              &errorAggregate,
                              fieldName,
                              "field or setField")) {
                this->swap(errorAggregate.value());
                return false;
            }
        }

        BSLS_ASSERT(fieldIndex >= -1);

        parentRecDef = d_recordDef;
        ret = descendIntoFieldByIndex(fieldIndex);

        // Repeat the above operations so long as the above lookup finds
        // an unnamed CHOICE or unnamed LIST:
    } while (ret && bdem_ElemType::isAggregateType(this->d_dataType)
          && 0 == parentRecDef->fieldName(fieldIndex));

    return ret;
}

bool bcem_Aggregate::descendIntoFieldById(int fieldId)
{
    int fieldIndex;
    bdeut_NullableValue<bcem_Aggregate> errorAggregate;
    if (getFieldIndex(&fieldIndex,
                      &errorAggregate,
                      fieldId,
                      "fieldById or setFieldById")) {
        this->swap(errorAggregate.value());
        return false;
    }

    return descendIntoFieldByIndex(fieldIndex);
}

bool
bcem_Aggregate::descendIntoFieldByIndex(int fieldIndex)
{
    void *valuePtr = d_value.ptr();

    switch (d_dataType) {
      case bdem_ElemType::BDEM_LIST:
        // Extract bdem_Row from bdem_Choice, then fall through to ROW case.

        valuePtr = &((bdem_List*)valuePtr)->row();
                                                                // FALL THROUGH
      case bdem_ElemType::BDEM_ROW: {
        bdem_Row& row = *(bdem_Row*)valuePtr;
        if ((unsigned)fieldIndex >= (unsigned)row.length()) {
            *this = makeError(BCEM_ERR_BAD_FIELDINDEX,
                              "Invalid field index %d specified for %s \"%s\"",
                              fieldIndex, bdem_ElemType::toAscii(d_dataType),
                              bcem_Aggregate_Util::recordName(d_recordDef));
            return false;
        }

        // adjust nullness info first
        d_parentType               = d_dataType;
        d_parentData               = d_value.ptr();
        d_indexInParent            = fieldIndex;
        *d_isTopLevelAggregateNull = 0;  // don't care

        const bdem_FieldDef& field = d_recordDef->field(fieldIndex);
        d_dataType  = field.elemType();
        d_recordDef = field.recordConstraint();
        d_fieldDef  = &field;
        d_value.loadAlias(d_value, row[fieldIndex].dataRaw());
      } break;

      case bdem_ElemType::BDEM_CHOICE:
        // Extract bdem_ChoiceArrayItem from bdem_Choice, then fall through
        // to the CHOICE_ARRAY_ITEM case.

        valuePtr = &((bdem_Choice*)valuePtr)->item();
                                                                // FALL THROUGH
      case bdem_ElemType::BDEM_CHOICE_ARRAY_ITEM: {
        bdem_ChoiceArrayItem& choiceItem = *(bdem_ChoiceArrayItem*)valuePtr;

        int selectorIndex = choiceItem.selector();
        if (fieldIndex >= 0 && fieldIndex != selectorIndex) {
            // Selector does not match current selection
            *this = makeError(BCEM_ERR_NOT_SELECTED,
                              "Attempt to access field %d in %s \"%s\" but "
                              "field %d is currently selected",
                              fieldIndex, bdem_ElemType::toAscii(d_dataType),
                              bcem_Aggregate_Util::recordName(d_recordDef),
                              selectorIndex);
            return false;
        }
        else if (-1 == selectorIndex) {
            // No current selection.  Set to a void object.
            reset();
            break;
        }

        // fieldIndex == selectorIndex or should be considered equivalent
        // adjust nullness info first
        d_parentType               = d_dataType;
        d_parentData               = d_value.ptr();
        d_indexInParent            = selectorIndex;
        *d_isTopLevelAggregateNull = 0;  // don't care

        // Descend into current selection
        const bdem_FieldDef& field = d_recordDef->field(selectorIndex);
        d_dataType  = field.elemType();
        d_recordDef = field.recordConstraint();
        d_fieldDef  = &field;
        d_value.loadAlias(d_value, choiceItem.selection().dataRaw());
      } break;

      default: {
        *this = makeError(BCEM_ERR_NOT_A_RECORD, "Attempt to access "
                          "field index %d on non-record type %s",
                          fieldIndex, bdem_ElemType::toAscii(d_dataType));
        return false;
      }
    }

    return true;
}

bool bcem_Aggregate::descendIntoArrayItem(int index, bool resetNullBit)
{
    void *valuePtr = d_value.ptr();

    bdem_ElemType::Type  itemType;
    void                *itemPtr  = 0;

    switch (d_dataType) {
      case bdem_ElemType::BDEM_TABLE: {
        bdem_Table& table = *(bdem_Table*)valuePtr;
        if ((unsigned)index >= (unsigned)table.numRows()) {
            break;  // out of bounds
        }

        if (isNillableScalarArray(d_dataType, d_recordDef)) {
            bdem_Row     *row = resetNullBit
                              ? (bdem_Row *) &table.theModifiableRow(index)
                              : const_cast<bdem_Row *>(&table.theRow(index));
            bdem_ElemRef  ref = (*row)[0];

            itemType                   = ref.type();
            itemPtr                    = ref.dataRaw();
            const bdem_FieldDef& field = d_recordDef->field(0);
            d_recordDef                = field.recordConstraint();
            d_fieldDef                 = &field;
            d_parentType               = bdem_ElemType::BDEM_ROW;
            d_parentData               = row;
            d_indexInParent            = 0;
            *d_isTopLevelAggregateNull = 0;  // don't care

            // d_schema is unchanged.
            d_value.loadAlias(d_value, itemPtr);
            d_dataType = itemType;

            return true;                                              // RETURN
        }
        else if (d_fieldDef && bdeat_FormattingMode::BDEAT_NILLABLE ==
                                               d_fieldDef->formattingMode()) {
            itemType = bdem_ElemType::BDEM_ROW;
            itemPtr  = resetNullBit
                     ? (bdem_Row *) &table.theModifiableRow(index)
                     : const_cast<bdem_Row *>(&table.theRow(index));
        }
        else {
            itemType = bdem_ElemType::BDEM_ROW;
            itemPtr  = &table.theModifiableRow(index);
        }
      } break;
      case bdem_ElemType::BDEM_CHOICE_ARRAY: {
        bdem_ChoiceArray& choiceArray = *(bdem_ChoiceArray*)valuePtr;
        if ((unsigned)index >= (unsigned)choiceArray.length()) {
            break;  // out of bounds
        }

        itemType = bdem_ElemType::BDEM_CHOICE_ARRAY_ITEM;
        if (d_fieldDef && bdeat_FormattingMode::BDEAT_NILLABLE ==
                                               d_fieldDef->formattingMode()) {
            itemPtr  = resetNullBit
             ? &choiceArray.theModifiableItem(index)
             : const_cast<bdem_ChoiceArrayItem *>(&choiceArray.theItem(index));
        }
        else {
            itemPtr  = &choiceArray.theModifiableItem(index);
        }
      } break;
      default: {
        if (bdem_ElemType::isArrayType(d_dataType)) {  // scalar array type
            bcem_Aggregate_ArrayIndexer indexer(index);
            if (bcem_Aggregate_Util::visitArray(valuePtr,
                                                d_dataType,
                                                &indexer)) {
                break;  // out of bounds
            }
            itemType = bdem_ElemType::fromArrayType(d_dataType);
            itemPtr  = indexer.data();
        }
        else {
            *this = makeError(BCEM_ERR_NOT_AN_ARRAY,
                              "Attempt to index a non-array object of type %s",
                              bdem_ElemType::toAscii(d_dataType));
            return false;
        }
      } break;
    }

    if (! itemPtr) {
        // If got here, then have out-of-bounds index
        *this = makeError(BCEM_ERR_BAD_ARRAYINDEX,
                          "Invalid array index %d used for %s",
                          index,
                          bdem_ElemType::toAscii(d_dataType));
        return false;
    }

    // adjust nullness info first
    d_parentType               = d_dataType;
    d_parentData               = d_value.ptr();
    d_indexInParent            = index;
    *d_isTopLevelAggregateNull = 0;  // don't care

    // d_schema, d_recordDef, and d_fieldDef are unchanged.
    d_value.loadAlias(d_value, itemPtr);
    d_dataType = itemType;

    return true;
}

// PRIVATE ACCESSORS
bool bcem_Aggregate::isNillableScalarArray(
                                        bdem_ElemType::Type   type,
                                        const bdem_RecordDef *constraint) const
{
    if (bdem_ElemType::BDEM_TABLE != type || !constraint) {
        return false;                                                 // RETURN
    }

    if (1 == constraint->numFields()) {
        if (!constraint->fieldName(0)
         && bdem_ElemType::isScalarType(constraint->field(0).elemType())) {
            return true;                                              // RETURN
        }
    }

    return false;
}

const bcem_Aggregate
bcem_Aggregate::makeError(int errorCode, const char *msg, ...) const
{
    if (0 == errorCode || isError()) {
        // Return this object if success is being returned or this object is
        // already an error.
        return *this;
    }

    enum { MAX_ERROR_STRING = 512 };
    char errorString[MAX_ERROR_STRING];

    using namespace bsl;  // in case 'vsnprintf' is in 'bsl'

    va_list args;
    va_start(args, msg);
    vsnprintf(errorString, MAX_ERROR_STRING, msg, args);
    va_end(args);

    bcema_SharedPtr<ErrorRecord> errPtr;
    errPtr.createInplace(0, errorCode, errorString);

    bcem_Aggregate ret;
    ret.d_value = errPtr;

    return ret;
}

int bcem_Aggregate::getFieldIndex(
                             int                                 *index,
                             bdeut_NullableValue<bcem_Aggregate> *errorResult,
                             const char                          *fieldName,
                             const char                          *caller) const
{
    enum { BCEM_SUCCESS = 0, BCEM_FAILURE = -1 };

    if (!bdem_ElemType::isAggregateType(d_dataType)) {
        errorResult->makeValue(
                makeError(BCEM_ERR_NOT_A_RECORD, "Attempt to call %s with "
                          "field name \"%s\" on array type: %s", caller,
                          fieldName, bdem_ElemType::toAscii(d_dataType)));
        return BCEM_FAILURE;
    }
    else if (! d_recordDef) {
        errorResult->makeValue(
                makeError(BCEM_ERR_NOT_A_RECORD, "Attempt to call %s with "
                          "field name \"%s\" on unconstrained %s", caller,
                          fieldName, bdem_ElemType::toAscii(d_dataType)));
        return BCEM_FAILURE;
    }
    else if (bdem_RecordDef::BDEM_CHOICE_RECORD == d_recordDef->recordType()
          && ! (fieldName && fieldName[0])) {
        *index = -1;
        return BCEM_SUCCESS;
    }

    *index = d_recordDef->fieldIndexExtended(fieldName);
    if (*index < 0) {
        errorResult->makeValue(
               makeError(BCEM_ERR_BAD_FIELDNAME,
                         "Invalid field name \"%s\" in %s \"%s\" passed to %s",
                         fieldName, bdem_ElemType::toAscii(d_dataType),
                         bcem_Aggregate_Util::recordName(d_recordDef),
                         caller));
        return BCEM_FAILURE;
    }

    return BCEM_SUCCESS;
}

int bcem_Aggregate::getFieldIndex(
                             int                                 *index,
                             bdeut_NullableValue<bcem_Aggregate> *errorResult,
                             int                                  fieldId,
                             const char                          *caller) const
{
    enum { BCEM_SUCCESS = 0, BCEM_FAILURE = -1 };

    if (!bdem_ElemType::isAggregateType(d_dataType)) {
        errorResult->makeValue(
                makeError(BCEM_ERR_NOT_A_RECORD, "Attempt to call %s with "
                          "field id \"%d\" on array type: %s", caller,
                          fieldId, bdem_ElemType::toAscii(d_dataType)));
        return BCEM_FAILURE;
    }
    else if (! d_recordDef) {
        errorResult->makeValue(
                makeError(BCEM_ERR_NOT_A_RECORD, "Attempt to call %s with "
                          "field ID %d on unconstrained %s", caller, fieldId,
                          bdem_ElemType::toAscii(d_dataType)));
        return BCEM_FAILURE;
    }
    else if (bdem_RecordDef::BDEM_CHOICE_RECORD == d_recordDef->recordType()
          && bdem_RecordDef::BDEM_NULL_FIELD_ID == fieldId) {
        // It is legal to pass 'BDEM_NULL_FIELD_ID' to a choice.
        *index = -1;
        return BCEM_SUCCESS;
    }

    *index = d_recordDef->fieldIndex(fieldId);
    if (*index < 0) {
        errorResult->makeValue(
                makeError(BCEM_ERR_BAD_FIELDID,
                          "Invalid field ID %d in %s %s passed to %s",
                          fieldId, bdem_ElemType::toAscii(d_dataType),
                          bcem_Aggregate_Util::recordName(d_recordDef),
                          caller));
        return BCEM_FAILURE;
    }

    return BCEM_SUCCESS;
}

const bcem_Aggregate
bcem_Aggregate::findUnambiguousChoice(const char *caller) const
{
    if (bdem_ElemType::BDEM_CHOICE            == d_dataType
     || bdem_ElemType::BDEM_CHOICE_ARRAY_ITEM == d_dataType) {
        // This object is a choice (unambiguously)
        return *this;
    }

    bcem_Aggregate ret(*this);

    do {
        ret = ret.anonymousField();
        // Loop until choice is found or error is encountered.  Since we do
        // not descend into arrays, it is not possible for ret.dataType() to
        // be a 'CHOICE_ARRAY_ITEM'.
    } while (bdem_ElemType::BDEM_CHOICE != ret.dataType() && ! ret.isError());

    if (ret.isError()) {
        // Discard error and replace by a more apropos error message.
        if (BCEM_ERR_AMBIGUOUS_ANON == ret.errorCode()) {
            ret = makeError(BCEM_ERR_AMBIGUOUS_ANON, "%s called "
                            "for object with multiple anonymous fields.  "
                            "Cannot pick one.", caller);
        }
        else {
            ret = makeError(BCEM_ERR_NOT_A_CHOICE,
                            "%s called on aggregate of type %s",
                            caller, bdem_ElemType::toAscii(d_dataType));
        }
    }

    return ret;
}

const bcem_Aggregate
bcem_Aggregate::makeSelectionByIndexRaw(int index) const
{
    bdem_ChoiceArrayItem *choice = 0;

    switch (d_dataType) {
      case bdem_ElemType::BDEM_CHOICE: {
        choice = &((bdem_Choice*)d_value.ptr())->item();
      } break;
      case bdem_ElemType::BDEM_CHOICE_ARRAY_ITEM: {
        choice = (bdem_ChoiceArrayItem*)d_value.ptr();
      } break;
      default: {
        return makeError(BCEM_ERR_NOT_A_CHOICE,
                         "makeSelection called on aggregate of type %s",
                         bdem_ElemType::toAscii(d_dataType));
      }
    }

    if (-1 != index && index >= choice->numSelections()) {
        return makeError(BCEM_ERR_BAD_FIELDINDEX,
                         "Invalid selection index %d in %s \"%s\" passed to "
                         "makeSelectionByIndex",
                         index, bdem_ElemType::toAscii(d_dataType),
                         bcem_Aggregate_Util::recordName(d_recordDef));
    }

    choice->makeSelection(index);

    return selection();
}

template <typename TOTYPE>
TOTYPE bcem_Aggregate::convertScalar() const
{
    TOTYPE result;
    int status = -1;
    const bdem_EnumerationDef *enumDef = enumerationConstraint();
    if (enumDef) {
        int enumId;
        if (bdem_ElemType::BDEM_INT == d_dataType) {
            enumId = *static_cast<int*>(d_value.ptr());
            status = 0;
        }
        else if (bdem_ElemType::BDEM_STRING == d_dataType) {
            const bsl::string& enumName =
                                     *static_cast<bsl::string*>(d_value.ptr());
            enumId = enumDef->lookupId(enumName.c_str());
            if (bdetu_Unset<int>::unsetValue() != enumId
             || bdetu_Unset<bsl::string>::isUnset(enumName)) {
                status = 0;
            }
        }

        if (0 == status) {
            status = bdem_Convert::convert(&result, enumId);
        }
    }

    if (0 != status) {
        // If not an enumeration, or if enum-conversion failed, then do normal
        // conversion.
        status = bdem_Convert::fromBdemType(&result,
                                            d_value.ptr(),
                                            d_dataType);
    }

    if (0 != status) {
        // Conversion failed.
        return bdetu_Unset<TOTYPE>::unsetValue();
    }

    return result;
}

template <>
bsl::string bcem_Aggregate::convertScalar<bsl::string>() const
{
    bsl::string result;
    int status = -1;
    const bdem_EnumerationDef *enumDef = enumerationConstraint();
    if (enumDef) {
        if (bdem_ElemType::BDEM_INT == d_dataType) {
            int enumId = *static_cast<int*>(d_value.ptr());
            if (bdetu_Unset<int>::isUnset(enumId)) {
                status = 0;
            }
            else {
                const char *enumName = enumDef->lookupName(enumId);
                if (enumName) {
                    result = enumName;
                    status = 0;
                }
            }
        }
        else if (bdem_ElemType::BDEM_STRING == d_dataType) {
            result = *static_cast<bsl::string*>(d_value.ptr());
            status = 0;
        }
    }

    if (0 != status) {
        // If not an enumeration, or if enum-conversion failed, then do normal
        // conversion.
        status = bdem_Convert::fromBdemType(&result,
                                            d_value.ptr(),
                                            d_dataType);
    }

    if (0 != status) {
        // Conversion failed.
        return "";
    }

    return result;
}

void bcem_Aggregate::convertScalarToString(bsl::string *result) const
{
    int status = -1;
    const bdem_EnumerationDef *enumDef = enumerationConstraint();
    if (enumDef) {
        if (bdem_ElemType::BDEM_INT == d_dataType) {
            int enumId = *static_cast<int*>(d_value.ptr());
            if (bdetu_Unset<int>::isUnset(enumId)) {
                status = 0;
            }
            else {
                const char *enumName = enumDef->lookupName(enumId);
                if (enumName) {
                    *result = enumName;
                    status = 0;
                }
            }
        }
        else if (bdem_ElemType::BDEM_STRING == d_dataType) {
            *result = *static_cast<bsl::string*>(d_value.ptr());
            status = 0;
        }
    }

    if (0 != status) {
        // If not an enumeration, or if enum-conversion failed, then do normal
        // conversion.
        status = bdem_Convert::fromBdemType(result, d_value.ptr(), d_dataType);
    }

    if (0 != status) {
        // Conversion failed.
        result->clear();
    }
}

// CLASS METHODS
bool bcem_Aggregate::areEquivalent(const bcem_Aggregate& lhs,
                                   const bcem_Aggregate& rhs)
{
    if (areIdentical(lhs, rhs)) {
        return true;       // identical aggregates
    }
    else if (lhs.d_dataType != rhs.d_dataType
          || lhs.isNul2()   != rhs.isNul2()) {
        return false;      // different types or nullness mismatch
    }
    else if (lhs.d_recordDef && rhs.d_recordDef) {
        if (lhs.d_recordDef != rhs.d_recordDef
         && ! bdem_SchemaUtil::areEquivalent(*lhs.d_recordDef,
                                             *rhs.d_recordDef)) {
            return false;  // different record definitions
        }
    }
    else if (lhs.d_recordDef || rhs.d_recordDef) {
        return false;      // one has record definition, other doesn't
    }

    // Data types, record definitions, and nullness match, so compare values.

    const bdem_Descriptor *descriptor =
                           bdem_ElemAttrLookup::lookupTable()[lhs.d_dataType];
    return lhs.isNul2()
        || descriptor->areEqual(lhs.d_value.ptr(), rhs.d_value.ptr());
}

// CREATORS
bcem_Aggregate::bcem_Aggregate()
: d_dataType(bdem_ElemType::BDEM_VOID)
, d_recordDef(0)
, d_fieldDef(0)
, d_parentType(bdem_ElemType::BDEM_VOID)
, d_parentData(0)
, d_indexInParent(-1)
{
}

bcem_Aggregate::bcem_Aggregate(const bcem_Aggregate& original)
: d_dataType(original.d_dataType)
, d_schema(original.d_schema)
, d_recordDef(original.d_recordDef)
, d_fieldDef(original.d_fieldDef)
, d_value(original.d_value)
, d_parentType(original.d_parentType)
, d_parentData(original.d_parentData)
, d_indexInParent(original.d_indexInParent)
, d_isTopLevelAggregateNull(original.d_isTopLevelAggregateNull)
{
}

bcem_Aggregate::~bcem_Aggregate()
{
    // Assert invariants (see member variable description in class definition)
#ifdef BDE_BUILD_TARGET_SAFE
    if (d_dataType != bdem_ElemType::BDEM_VOID) {
        BSLS_ASSERT(d_schema || (!d_recordDef && !d_fieldDef));
        BSLS_ASSERT(!d_schema || (d_recordDef || d_fieldDef));
        BSLS_ASSERT(! d_recordDef
                    || &d_recordDef->schema() == d_schema.ptr());
        // Cannot easily test that 'd_fieldDef' is within 'd_schema'
        BSLS_ASSERT(! d_fieldDef || d_fieldDef->elemType() == d_dataType
                    || d_fieldDef->elemType() ==
                                       bdem_ElemType::toArrayType(d_dataType));
        BSLS_ASSERT(! d_fieldDef
                    || d_recordDef  == d_fieldDef->recordConstraint());
    }
#endif
}

// MANIPULATORS
bcem_Aggregate& bcem_Aggregate::operator=(const bcem_Aggregate& rhs)
{
    if (this != &rhs) {
        d_dataType                = rhs.d_dataType;
        d_schema                  = rhs.d_schema;
        d_recordDef               = rhs.d_recordDef;
        d_fieldDef                = rhs.d_fieldDef;
        d_value                   = rhs.d_value;
        d_parentType              = rhs.d_parentType;
        d_parentData              = rhs.d_parentData;
        d_indexInParent           = rhs.d_indexInParent;
        d_isTopLevelAggregateNull = rhs.d_isTopLevelAggregateNull;
    }

    return *this;
}

const bcem_Aggregate  bcem_Aggregate::reserveRaw(bsl::size_t numItems)
{
    if(!bdem_ElemType::isArrayType(d_dataType)) {
        return makeError(BCEM_ERR_NOT_AN_ARRAY,
                         "Attempt to reserve on non-array aggregate of type %s",
                         bdem_ElemType::toAscii(d_dataType));
    }

    int status = 0;

    void *valuePtr = d_value.ptr();
    switch (d_dataType) {
      case bdem_ElemType::BDEM_TABLE: {
        bdem_Table& table = *(bdem_Table*)valuePtr;
        table.reserveRaw(numItems);
      } break;
      case bdem_ElemType::BDEM_CHOICE_ARRAY: {
        bdem_ChoiceArray& array = *(bdem_ChoiceArray*)valuePtr;
        array.reserveRaw(numItems);
      } break;
      default: {
        bcem_Aggregate_ArrayReserver reserver(numItems);
        status = bcem_Aggregate_Util::visitArray(valuePtr,
                                                 d_dataType,
                                                 &reserver);
      }
    }

    if (status < 0) {
        return makeError(status, "Attempt to reserve %Zu items into %s ",
                         numItems, bdem_ElemType::toAscii(d_dataType));
    }
    return *this;
}

const bcem_Aggregate& bcem_Aggregate::reset()
{
    d_dataType = bdem_ElemType::BDEM_VOID;
    d_schema.clear();
    d_recordDef = 0;
    d_fieldDef  = 0;
    d_value.clear();
    d_parentType = bdem_ElemType::BDEM_VOID;
    d_parentData = 0;
    d_indexInParent = -1;
    if (d_isTopLevelAggregateNull.ptr()) {
        *d_isTopLevelAggregateNull = 0;
    }

    return *this;
}

// ACCESSORS THAT MANIPULATE DATA
const bcem_Aggregate& bcem_Aggregate::makeNull()  const
{
    if (bdem_ElemType::BDEM_VOID != d_dataType) {
        if (bdem_ElemType::isArrayType(d_parentType)) {
            if (!bdem_ElemType::isAggregateType(d_parentType)) {
                const bdem_Descriptor *descriptor =
                                bdem_ElemAttrLookup::lookupTable()[d_dataType];
                descriptor->makeUnset(d_value.ptr());
            }
            else if (bdem_ElemType::BDEM_TABLE == d_parentType) {
                bdem_Table& table = *(bdem_Table *)d_parentData;
                table.makeRowsNull(d_indexInParent, 1);
            }
            else {
                BSLS_ASSERT(bdem_ElemType::BDEM_CHOICE_ARRAY
                                                              == d_parentType);
                bdem_ChoiceArray& choiceArray =
                                             *(bdem_ChoiceArray *)d_parentData;
                choiceArray.makeItemsNull(d_indexInParent, 1);
            }
        }
        else {
            asElemRef().makeNull();
        }
    }

    return *this;
}

const bcem_Aggregate bcem_Aggregate::resize(int newSize) const
{
    if (! bdem_ElemType::isArrayType(d_dataType)) {
        return makeError(BCEM_ERR_NOT_AN_ARRAY,
                         "Attempt to resize non-array aggregate of type %s",
                         bdem_ElemType::toAscii(d_dataType));
    }
    else if (newSize < 0) {
        return makeError(BCEM_ERR_BAD_ARRAYINDEX,
                         "Attempt to resize %s to negative length",
                         bdem_ElemType::toAscii(d_dataType));
    }

    int currentSize = length();
    if (newSize > currentSize) {
        insertItems(currentSize, newSize - currentSize);
    }
    else if (newSize < currentSize) {
        removeItems(newSize, currentSize - newSize);
    }

    return *this;
}

const bcem_Aggregate bcem_Aggregate::insertItems(int pos, int numItems) const
{
    if (! bdem_ElemType::isArrayType(d_dataType)) {
        return makeError(BCEM_ERR_NOT_AN_ARRAY,
                         "Attempt to insert items into non-array aggregate "
                         "of type %s", bdem_ElemType::toAscii(d_dataType));
    }

    bool isAggNull = isNul2();

    int status = 0;
    int arrayLen;
    switch (d_dataType) {
      case bdem_ElemType::BDEM_TABLE: {
        bdem_Table& theTable = *(bdem_Table *)d_value.ptr();
        arrayLen = theTable.numRows();
        if (pos > arrayLen) {
            status = BCEM_ERR_BAD_ARRAYINDEX;
            break;
        }

        if (isAggNull) {
            makeValue();
        }

        theTable.insertNullRows(pos, numItems);
        if (d_recordDef
         && (!d_fieldDef || bdeat_FormattingMode::BDEAT_NILLABLE !=
                                              d_fieldDef->formattingMode())) {
            for (int i = pos; i < pos + numItems; ++i) {
                bdem_SchemaAggregateUtil::initRowDeep(
                                                 &theTable.theModifiableRow(i),
                                                 *d_recordDef);
            }
        }
      } break;
      case bdem_ElemType::BDEM_CHOICE_ARRAY: {
        bdem_ChoiceArray& theChoiceArray = *(bdem_ChoiceArray*)d_value.ptr();
        arrayLen = theChoiceArray.length();
        if (pos > arrayLen) {
            status = BCEM_ERR_BAD_ARRAYINDEX;
            break;
        }

        if (isAggNull) {
            makeValue();
        }

        theChoiceArray.insertNullItems(pos, numItems);
      } break;
      default: {
        if (isAggNull) {
            makeValue();
        }

        if (numItems > 0) {
            bcem_Aggregate_ArrayInserter inserter(pos, numItems, d_fieldDef);
            status = bcem_Aggregate_Util::visitArray(d_value.ptr(),
                                                     d_dataType,
                                                     &inserter);
            if (status) {
                if (isAggNull) {
                    makeNull();
                }
                status = BCEM_ERR_BAD_ARRAYINDEX;
            }

            arrayLen = inserter.length();
        }
      } break;
    }

    if (status < 0) {
        return makeError(status, "Attempt to insert items at index %d into %s "
                         "of length %d", pos,
                         bdem_ElemType::toAscii(d_dataType), arrayLen);
    }

    return *this;
}

const bcem_Aggregate
bcem_Aggregate::insertNullItems(int pos, int numItems) const
{
    if (! bdem_ElemType::isArrayType(d_dataType)) {
        return makeError(BCEM_ERR_NOT_AN_ARRAY,
                         "Attempt to insert null items into non-array "
                         "aggregate of type %s",
                         bdem_ElemType::toAscii(d_dataType));
    }

    bool isAggNull = isNul2();
    int status = 0;
    int arrayLen;
    switch (d_dataType) {
      case bdem_ElemType::BDEM_TABLE: {
        bdem_Table& theTable = *(bdem_Table*)d_value.ptr();
        arrayLen = theTable.numRows();
        if (pos > arrayLen) {
            status = BCEM_ERR_BAD_ARRAYINDEX;
            break;
        }

        if (isAggNull) {
            makeValue();
        }

        theTable.insertNullRows(pos, numItems);
      } break;
      case bdem_ElemType::BDEM_CHOICE_ARRAY: {
        bdem_ChoiceArray& theChoiceArray = *(bdem_ChoiceArray*)d_value.ptr();
        arrayLen = theChoiceArray.length();
        if (pos > arrayLen) {
            status = BCEM_ERR_BAD_ARRAYINDEX;
            break;
        }

        if (isAggNull) {
            makeValue();
        }

        theChoiceArray.insertNullItems(pos, numItems);
      } break;
      default: {
        if (isAggNull) {
            makeValue();
        }

        if (numItems > 0) {
            bcem_Aggregate_ArrayInserter inserter(pos,
                                                  numItems,
                                                  d_fieldDef,
                                                  true);
            status = bcem_Aggregate_Util::visitArray(d_value.ptr(),
                                                     d_dataType,
                                                     &inserter);
            if (status) {
                if (isAggNull) {
                    makeNull();
                }
                status = BCEM_ERR_BAD_ARRAYINDEX;
            }
            arrayLen = inserter.length();
        }
      } break;
    }

    if (status < 0) {
        return makeError(status, "Attempt to insert null items at index %d "
                         "into %s of length %d", pos,
                         bdem_ElemType::toAscii(d_dataType), arrayLen);
    }

    return *this;
}

const bcem_Aggregate bcem_Aggregate::removeItems(int pos, int numItems) const
{
    if (! bdem_ElemType::isArrayType(d_dataType)) {
        return makeError(BCEM_ERR_NOT_AN_ARRAY,
                         "Attempt to remove items from non-array aggregate "
                         "of type %s", bdem_ElemType::toAscii(d_dataType));
    }
    else if (pos + numItems > length()) {
        return makeError(BCEM_ERR_BAD_ARRAYINDEX, "Attempt to remove items "
                         "at index %d in %s of length %d",
                         pos, bdem_ElemType::toAscii(d_dataType), length());
    }

    switch (d_dataType) {
      case bdem_ElemType::BDEM_TABLE: {
        bdem_Table& theTable = *(bdem_Table*)d_value.ptr();
        theTable.removeRows(pos, numItems);
      } break;
      case bdem_ElemType::BDEM_CHOICE_ARRAY: {
        bdem_ChoiceArray& theChoiceArray = *(bdem_ChoiceArray*)d_value.ptr();
        theChoiceArray.removeItems(pos, numItems);
      } break;
      default: {
        ArrayItemEraser itemEraser(pos, numItems);
        bcem_Aggregate_Util::visitArray(d_value.ptr(),
                                        d_dataType,
                                        &itemEraser);
      } break;
    }

    return *this;
}

const bcem_Aggregate bcem_Aggregate::selection() const
{
    bcem_Aggregate obj = findUnambiguousChoice("selection");
    if (obj.isError()) {
        return obj;
    }

    return obj.fieldByIndex(selectorIndex());
}

const bcem_Aggregate
bcem_Aggregate::makeSelection(const char *newSelector) const
{
    int                   newSelectorIndex = -1;
    bool                  foundChoice      = false;
    bcem_Aggregate        obj              = *this;
    const bdem_RecordDef *parentRecordDef  = 0;

    // Descend into unnamed choices and sequences until a leaf is found
    do {
        bdeut_NullableValue<bcem_Aggregate> errorAggregate;
        if (obj.getFieldIndex(&newSelectorIndex,
                              &errorAggregate,
                              newSelector,
                              "makeSelection")) {
            return errorAggregate.value();
        }

        // TBD: We don't reset nullness of top level aggregate if we descend
        // into an aggregate and we face an error making the selection.
        if (isNul2()) {
            makeValue();
        }

        parentRecordDef = obj.d_recordDef;
        if (bdem_RecordDef::BDEM_CHOICE_RECORD ==
                                               obj.d_recordDef->recordType()) {
            // CHOICE or CHOICE_ARRAY_ITEM
            obj = obj.makeSelectionByIndex(newSelectorIndex);
            foundChoice = true;
        }
        else {
            // Even though object is not a choice, it might contain an
            // unnamed choice field or an unnamed sequence that indirectly
            // holds an unnamed choice.  Descend into field:

            obj.descendIntoFieldByIndex(newSelectorIndex);
        }

        // Loop so long as we are looking at an unnamed constrained aggregate.
    } while (obj.d_recordDef
          && 0 == parentRecordDef->fieldName(newSelectorIndex));

    if (! foundChoice) {
        // We may have descended through zero or more unnamed aggregates, but
        // none of them were choice aggregates.  'makeSelection' is not
        // appropriate.

        return obj.makeError(BCEM_ERR_NOT_A_CHOICE, "Called makeSelection "
                             "on non-choice object of type %s",
                             bdem_ElemType::toAscii(this->d_dataType));
    }

    return obj;  // holds the desired result or an error
}

const bcem_Aggregate bcem_Aggregate::makeValue() const
{
    if (bdem_ElemType::BDEM_VOID == d_dataType) {
        return *this;                                                 // RETURN
    }

    bdem_ElemRef elemRef = asElemRef();

    if (!elemRef.isNull()) {
        return *this;                                                 // RETURN
    }

    elemRef.data();  // clear nullness bit

    if (bdem_ElemType::isScalarType(d_dataType)) {
        if (d_fieldDef && d_fieldDef->hasDefaultValue()) {
            asElemRef().replaceValue(d_fieldDef->defaultValue());
        }
    }
    else if (d_recordDef) {
        switch (d_dataType) {
          case bdem_ElemType::BDEM_ROW: {
            bdem_SchemaAggregateUtil::initRowDeep((bdem_Row *) d_value.ptr(),
                                                  *d_recordDef);
          } break;
          case bdem_ElemType::BDEM_LIST: {
            bdem_SchemaAggregateUtil::initListDeep((bdem_List *) d_value.ptr(),
                                                   *d_recordDef);
          } break;
          case bdem_ElemType::BDEM_CHOICE: {
            bdem_SchemaAggregateUtil::initChoice((bdem_Choice *) d_value.ptr(),
                                                 *d_recordDef);
          } break;
          case bdem_ElemType::BDEM_TABLE: {
            bdem_SchemaAggregateUtil::initTable((bdem_Table *) d_value.ptr(),
                                                *d_recordDef);
          } break;
          case bdem_ElemType::BDEM_CHOICE_ARRAY: {
            bdem_SchemaAggregateUtil::initChoiceArray(
                                            (bdem_ChoiceArray *) d_value.ptr(),
                                            *d_recordDef);
          } break;
          default: {
          } break;
        }
    }

    return *this;
}

// MANIPULATORS
void bcem_Aggregate::swap(bcem_Aggregate& rhs)
{
    bsl::swap(d_dataType, rhs.d_dataType);
    bsl::swap(d_recordDef, rhs.d_recordDef);
    bsl::swap(d_fieldDef, rhs.d_fieldDef);
    d_schema.swap(rhs.d_schema);
    d_value.swap(rhs.d_value);
    bsl::swap(d_parentType, rhs.d_parentType);
    bsl::swap(d_parentData, rhs.d_parentData);
    bsl::swap(d_indexInParent, rhs.d_indexInParent);
    d_isTopLevelAggregateNull.swap(rhs.d_isTopLevelAggregateNull);
}

// ACCESSORS
int bcem_Aggregate::errorCode() const
{
    return isError() ? static_cast<ErrorRecord*>(d_value.ptr())->errorCode()
                     : 0;
}

bsl::string bcem_Aggregate::errorMessage() const
{
    if (! isError()) {
        return "";
    }

    return static_cast<ErrorRecord*>(d_value.ptr())->errorMessage();
}

bsl::string bcem_Aggregate::asString() const
{
    if (bdem_ElemType::BDEM_VOID == d_dataType) {
        // Special case: return empty string for 'BDEM_VOID' type.
        return "";
    }

    return convertScalar<bsl::string>();
}

void bcem_Aggregate::loadAsString(bsl::string *result) const
{
    if (bdem_ElemType::BDEM_VOID == d_dataType) {
        // Special case: load empty string for 'BDEM_VOID' type.
        result->clear();
    } else {
        convertScalarToString(result);
    }
}

bool bcem_Aggregate::asBool() const
{
    return convertScalar<bool>();
}

char bcem_Aggregate::asChar() const
{
    return convertScalar<char>();
}

short bcem_Aggregate::asShort() const
{
    return convertScalar<short>();
}

int bcem_Aggregate::asInt() const
{
    return convertScalar<int>();
}

Int64 bcem_Aggregate::asInt64() const
{
    return convertScalar<Int64>();
}

float bcem_Aggregate::asFloat() const
{
    return convertScalar<float>();
}

double bcem_Aggregate::asDouble() const
{
    return convertScalar<double>();
}

bdet_Datetime bcem_Aggregate::asDatetime() const
{
    return convertScalar<bdet_Datetime>();
}

bdet_DatetimeTz bcem_Aggregate::asDatetimeTz() const
{
    return convertScalar<bdet_DatetimeTz>();
}

bdet_Date bcem_Aggregate::asDate() const
{
    return convertScalar<bdet_Date>();
}

bdet_DateTz bcem_Aggregate::asDateTz() const
{
    return convertScalar<bdet_DateTz>();
}

bdet_Time bcem_Aggregate::asTime() const
{
    return convertScalar<bdet_Time>();
}

bdet_TimeTz bcem_Aggregate::asTimeTz() const
{
    return convertScalar<bdet_TimeTz>();
}

const bdem_ElemRef bcem_Aggregate::asElemRef() const
{
    if (!d_parentData) {
        // top-level aggregate
        const bdem_Descriptor *descriptor =
                                bdem_ElemAttrLookup::lookupTable()[d_dataType];
        int *nullnessWord = bdem_ElemType::BDEM_VOID == d_dataType
                          ? &s_voidNullnessWord
                          : d_isTopLevelAggregateNull.ptr();

        return bdem_ElemRef(d_value.ptr(), descriptor, nullnessWord, 0);
                                                                      // RETURN
    }

    void *valuePtr = d_parentData;

    switch (d_parentType) {
      case bdem_ElemType::BDEM_LIST: {
        // Extract bdem_Row from bdem_Choice, then fall through to ROW case.

        valuePtr = &((bdem_List *) valuePtr)->row();            // FALL THROUGH
      case bdem_ElemType::BDEM_ROW:

        bdem_Row& row = *(bdem_Row *) valuePtr;
        return row[d_indexInParent];                                  // RETURN
      } break;
      case bdem_ElemType::BDEM_CHOICE:
        // Extract bdem_ChoiceArrayItem from bdem_Choice, then fall through
        // to the CHOICE_ARRAY_ITEM case.

        valuePtr = &((bdem_Choice *) valuePtr)->item();         // FALL THROUGH
      case bdem_ElemType::BDEM_CHOICE_ARRAY_ITEM: {
        bdem_ChoiceArrayItem& choiceItem = *(bdem_ChoiceArrayItem*)valuePtr;
        return choiceItem.selection();                                // RETURN
      } break;
      case bdem_ElemType::BDEM_TABLE: {
        bdem_Table& table = *(bdem_Table *) valuePtr;
        return table.rowElemRef(d_indexInParent);                     // RETURN
      } break;
      case bdem_ElemType::BDEM_CHOICE_ARRAY: {
        return ((bdem_ChoiceArray *)valuePtr)->itemElemRef(d_indexInParent);
                                                                      // RETURN
      } break;
      default: {
        BSLS_ASSERT(bdem_ElemType::isArrayType(d_parentType));

        const bdem_Descriptor *descriptor =
                                bdem_ElemAttrLookup::lookupTable()[d_dataType];
        return bdem_ElemRef(d_value.ptr(), descriptor);               // RETURN
      }
    }
}

const bcem_Aggregate bcem_Aggregate::field(NameOrIndex fieldOrIdx) const
{
    return fieldImp(false, fieldOrIdx);
}

const bcem_Aggregate bcem_Aggregate::fieldImp(bool        resetNullBit,
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
    bcem_Aggregate fldObj(*this);
    // Descend into each field, stopping at the first one to return false.
    (void) (fldObj.descendIntoField(fieldOrIdx1, resetNullBit)
         && fldObj.descendIntoField(fieldOrIdx2, resetNullBit)
         && fldObj.descendIntoField(fieldOrIdx3, resetNullBit)
         && fldObj.descendIntoField(fieldOrIdx4, resetNullBit)
         && fldObj.descendIntoField(fieldOrIdx5, resetNullBit)
         && fldObj.descendIntoField(fieldOrIdx6, resetNullBit)
         && fldObj.descendIntoField(fieldOrIdx7, resetNullBit)
         && fldObj.descendIntoField(fieldOrIdx8, resetNullBit)
         && fldObj.descendIntoField(fieldOrIdx9, resetNullBit)
         && fldObj.descendIntoField(fieldOrIdx10, resetNullBit));

    return fldObj;
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
    bcem_Aggregate fldObj(*this);
    fldObj.descendIntoFieldById(fieldId);
    return fldObj;
}

const bcem_Aggregate bcem_Aggregate::fieldByIndex(int index) const
{
    bcem_Aggregate fldObj(*this);
    fldObj.descendIntoFieldByIndex(index);
    return fldObj;
}

const bcem_Aggregate bcem_Aggregate::anonymousField(int n) const
{
    if (! d_recordDef) {
        return makeError(BCEM_ERR_NOT_A_RECORD, "Called anonymousField on "
                         "unconstrained %s object",
                         bdem_ElemType::toAscii(d_dataType));
    }

    int numAnonFields = d_recordDef->numAnonymousFields();
    if (0 == numAnonFields) {
        return makeError(BCEM_ERR_BAD_FIELDINDEX, "Called anonymousField "
                         "for %s \"%s\" that contains no anonymous fields",
                         bdem_ElemType::toAscii(d_dataType),
                         bcem_Aggregate_Util::recordName(d_recordDef));
    }
    else if ((unsigned)n >= (unsigned)numAnonFields) {
        return makeError(BCEM_ERR_BAD_FIELDINDEX,
                         "Invalid index %d passed to anonymousField "
                         "for %s \"%s\".",
                         n, bdem_ElemType::toAscii(d_dataType),
                         bcem_Aggregate_Util::recordName(d_recordDef));
    }

    int fldIdx = 0;
    for (int anonIdx = -1; anonIdx < n; ++fldIdx) {
        if (0 == d_recordDef->fieldName(fldIdx)) {
            ++anonIdx;
            if (anonIdx == n) {
                break;  // break loop without incrementing fldIdx
            }
        }
    }

    return fieldByIndex(fldIdx);
}

const bcem_Aggregate bcem_Aggregate::anonymousField() const
{
    if (d_recordDef && 1 < d_recordDef->numAnonymousFields()) {
        // Only report error if there are more than one anonymous fields.  The
        // case where there are zero anonymous fields is already handled by
        // the single-argument call to 'anonymousField', below.
        return makeError(BCEM_ERR_AMBIGUOUS_ANON, "anonymousField() called "
                         "for object with multiple anonymous fields.  "
                         "Cannot pick one.");
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
    return fieldImp(false, fieldOrIdx1, fieldOrIdx2, fieldOrIdx3, fieldOrIdx4,
                    fieldOrIdx5, fieldOrIdx6, fieldOrIdx7, fieldOrIdx8,
                    fieldOrIdx9, fieldOrIdx10).d_dataType;
}

bdem_ElemType::Type bcem_Aggregate::fieldTypeById(int fieldId) const
{
    return fieldById(fieldId).d_dataType;
}

bdem_ElemType::Type bcem_Aggregate::fieldTypeByIndex(int index) const
{
    return fieldByIndex(index).d_dataType;
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
    return fieldImp(true,
                    fieldOrIdx1, fieldOrIdx2, fieldOrIdx3, fieldOrIdx4,
                    fieldOrIdx5, fieldOrIdx6, fieldOrIdx7, fieldOrIdx8,
                    fieldOrIdx9, fieldOrIdx10).asElemRef();
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

const bcem_Aggregate bcem_Aggregate::capacityRaw(bsl::size_t *capacity) const
{
    BSLS_ASSERT(0 != capacity);

    if (!bdem_ElemType::isArrayType(d_dataType)) {
        return makeError(BCEM_ERR_NOT_AN_ARRAY,                       // RETURN
                         "Attempt to get capacity on non-array aggregate of"
                         "  type %s", bdem_ElemType::toAscii(d_dataType));
    }

    void *valuePtr = d_value.ptr();

    switch (d_dataType) {
      case bdem_ElemType::BDEM_TABLE: {
        bdem_Table& table = *(bdem_Table*)valuePtr;
        *capacity = table.capacityRaw();
      } break;
      case bdem_ElemType::BDEM_CHOICE_ARRAY: {
        bdem_ChoiceArray& array = *(bdem_ChoiceArray*)valuePtr;
        *capacity = array.capacityRaw();
      } break;
      default: {
        bcem_Aggregate_ArrayCapacitor capacitor;
        *capacity = bcem_Aggregate_Util::visitArray(
                                             valuePtr,
                                             d_dataType,
                                             &capacitor);
      }
    }
    return *this;
}

int bcem_Aggregate::length() const
{
    int result;

    switch (d_dataType) {
      case bdem_ElemType::BDEM_LIST: {
        result = ((bdem_List*)d_value.ptr())->length();
      } break;
      case bdem_ElemType::BDEM_ROW: {
        result = ((bdem_Row*)d_value.ptr())->length();
      } break;
      case bdem_ElemType::BDEM_TABLE: {
        result = ((bdem_Table*)d_value.ptr())->numRows();
      } break;
      case bdem_ElemType::BDEM_CHOICE_ARRAY: {
        result = ((bdem_ChoiceArray*)d_value.ptr())->length();
      } break;
      default: {
        bcem_Aggregate_ArraySizer sizer;
        result = bcem_Aggregate_Util::visitArray(d_value.ptr(),
                                                 d_dataType,
                                                 &sizer);
      } break;
    }

    return result;
}

bool bcem_Aggregate::isNul2() const
{
    if (!isNullable()) {
        return false;
    }

    if (bdem_ElemType::isArrayType(d_parentType)) {
        if (bdem_ElemType::BDEM_TABLE == d_parentType) {
            bdem_Table& table = *(bdem_Table *)d_parentData;
            return table.isRowNull(d_indexInParent);                  // RETURN
        }
        else {
            BSLS_ASSERT(bdem_ElemType::BDEM_CHOICE_ARRAY == d_parentType);

            bdem_ChoiceArray& choiceArray = *(bdem_ChoiceArray *)d_parentData;
            return choiceArray.isItemNull(d_indexInParent);           // RETURN
        }
    }

    return isVoid() || asElemRef().isNull();
}

bool bcem_Aggregate::isNullable() const
{
    return bdem_ElemType::isArrayType(d_parentType)
        && !bdem_ElemType::isAggregateType(d_parentType)
         ? false
         : true;
}

#if !defined(BSL_LEGACY) || 1 == BSL_LEGACY
bool bcem_Aggregate::isUnset() const
{
    switch (d_dataType) {
      case bdem_ElemType::BDEM_VOID: {
        return true;
      }
      case bdem_ElemType::BDEM_TABLE: {
        const bdem_Table *table = (bdem_Table *) d_value.ptr();
        if (d_recordDef) {
            return 0 == table->numRows();
        }
        else {
            return 0 == table->numRows() && 0 == table->numColumns();
        }
      } break;
      case bdem_ElemType::BDEM_CHOICE: {
        const bdem_Choice *choice = (bdem_Choice *) d_value.ptr();
        if (d_recordDef) {
            return choice->selector() < 0;
        }
        else {
            return choice->selector() < 0 && 0 == choice->numSelections();
        }
      } break;
      case bdem_ElemType::BDEM_CHOICE_ARRAY_ITEM: {
        const bdem_ChoiceArrayItem *item =
                                        (bdem_ChoiceArrayItem *) d_value.ptr();
        return item->selector() < 0;
      } break;
      case bdem_ElemType::BDEM_CHOICE_ARRAY: {
        const bdem_ChoiceArray *choiceArray
                                          = (bdem_ChoiceArray *) d_value.ptr();
        if (d_recordDef) {
            return 0 == choiceArray->length();
        }
        else {
            return 0 == choiceArray->length()
                && 0 == choiceArray->numSelections();
        }
      } break;
      case bdem_ElemType::BDEM_ROW:
      case bdem_ElemType::BDEM_LIST:
      default: {
        const bdem_Descriptor *descriptor =
            bdem_ElemAttrLookup::lookupTable()[d_dataType];
        return descriptor->isUnset(d_value.ptr());
      }
    }
}
#endif

int bcem_Aggregate::numSelections() const
{
    int result;

    bcem_Aggregate choiceObj = findUnambiguousChoice("numSelections");

    if (choiceObj.isError()) {
        result = choiceObj.errorCode();
    }
    else {
        switch (choiceObj.d_dataType) {
          case bdem_ElemType::BDEM_CHOICE: {
            result = ((bdem_Choice*)choiceObj.d_value.ptr())->numSelections();
          } break;
          case bdem_ElemType::BDEM_CHOICE_ARRAY_ITEM: {
            result =
             ((bdem_ChoiceArrayItem*)choiceObj.d_value.ptr())->numSelections();
          } break;
          case bdem_ElemType::BDEM_CHOICE_ARRAY: {
            result =
                 ((bdem_ChoiceArray*)choiceObj.d_value.ptr())->numSelections();
          } break;
          default: {
            result = BCEM_ERR_NOT_A_CHOICE;
          } break;
        }
    }

    return result;
}

const char *bcem_Aggregate::selector() const
{
    bcem_Aggregate choiceObj = findUnambiguousChoice("selector");
    if (choiceObj.isError()) {
        return "";
    }

    int index = choiceObj.selectorIndex();
    return index < 0 ? "" : choiceObj.d_recordDef->fieldName(index);
}

int bcem_Aggregate::selectorIndex() const
{
    int index;
    switch (d_dataType) {
      case bdem_ElemType::BDEM_CHOICE: {
        index = ((bdem_Choice*)d_value.ptr())->selector();
      } break;
      case bdem_ElemType::BDEM_CHOICE_ARRAY_ITEM: {
        index = ((bdem_ChoiceArrayItem*)d_value.ptr())->selector();
      } break;
      case bdem_ElemType::BDEM_LIST:
      case bdem_ElemType::BDEM_ROW: {
        // For non-choice records, find the unambiguous anonymous choice
        // within the record and return the selection index for that.  If
        // there is no unambiguous anonymous choice 'findUnambiguousChoice'
        // will return an error object and the recursive call to
        // 'selectorIndex' will return the error code.
        index = findUnambiguousChoice("selectorIndex").selectorIndex();
      } break;
      default: {
        if (isError()) {
            index = errorCode();
        }
        else {
            index = BCEM_ERR_NOT_A_CHOICE;
        }
      } break;
    }

    return index;
}

int bcem_Aggregate::selectorId() const
{
    bcem_Aggregate choiceObj = findUnambiguousChoice("selectorId");
    if (choiceObj.isError()) {
        return bdem_RecordDef::BDEM_NULL_FIELD_ID;
    }

    int index = choiceObj.selectorIndex();
    return index < 0 ? bdem_RecordDef::BDEM_NULL_FIELD_ID
                     : choiceObj.d_recordDef->fieldId(index);
}

const bcem_Aggregate
bcem_Aggregate::clone(bslma_Allocator *basicAllocator) const
{
    bcem_Aggregate returnVal(this->cloneData(basicAllocator));

    if (! d_schema) {
        return returnVal;
    }

    // Clone the schema:
    bcema_SharedPtr<bdem_Schema> schemaClone;
    schemaClone.createInplace(basicAllocator, *d_schema, basicAllocator);
    returnVal.d_schema = schemaClone;

    if (d_recordDef) {
        // Set the clone's record pointer to point into the cloned schema
        int recordIndex = d_recordDef->recordIndex();
        returnVal.d_recordDef = &schemaClone->record(recordIndex);
    }

    if (d_fieldDef) {
        // The field spec is not null -- find the field spec in the original
        // schema.
        // TBD: The only way to find the field spec in the schema is to do a
        // linear search through all of the fields of all of the recordDefs.
        // Can we find a more efficient way to do this?
        for (int recIndex = 0; recIndex < d_schema->numRecords(); ++recIndex) {
            const bdem_RecordDef& rec = d_schema->record(recIndex);
            for (int fieldIndex = 0;
                 fieldIndex < rec.numFields();
                 ++fieldIndex) {
                const bdem_FieldDef& field = rec.field(fieldIndex);
                if (&field == d_fieldDef) {
                    // Point the field spec in the clone to the corresponding
                    // fieldspec within the cloned schema.
                    returnVal.d_fieldDef = &(returnVal.d_schema->
                                             record(recIndex).
                                             field(fieldIndex));
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

    switch (d_dataType) {
      case bdem_ElemType::BDEM_ROW: {
        // ROW is a special case.  Because a row has no copy constructor, it
        // is necessary to make a list containing a copy of the row, then
        // return an aggregate that refers to the copy within the list.

        // Construct a parent list
        bcema_SharedPtr<void> parent =
                            makeValuePtrInplaceWithAlloc<bdem_List>(allocator);
        bdem_List& parentList = *(bdem_List*)parent.ptr();

        // Perform a row-to-list assignment to make a copy of this row.
        parentList = *(bdem_Row*)d_value.ptr();

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
        bdem_Choice& parentChoice = *(bdem_Choice*)parent.ptr();

        // Perform an item-to-choice assignment to make a copy of this item.
        parentChoice = *(bdem_ChoiceArrayItem*)d_value.ptr();

        // Get a shared pointer to the item within the parent choice
        valuePtr.loadAlias(parent, &parentChoice.item());
      } break;
      case bdem_ElemType::BDEM_VOID: {
        if (isError()) {
            valuePtr = makeValuePtrInplaceWithAlloc<ErrorRecord>(allocator);
            *static_cast<ErrorRecord*>(valuePtr.ptr()) =
                               *static_cast<const ErrorRecord*>(d_value.ptr());
        }
      } break;
      default: {
        // Make a copy of the value.
        const bdem_Descriptor *descriptor =
            bdem_ElemAttrLookup::lookupTable()[d_dataType];

        valuePtr = bcem_Aggregate_Util::makeValuePtr(d_dataType, allocator);
        descriptor->assign(valuePtr.ptr(), d_value.ptr());
      } break;
    }

    bcem_Aggregate returnVal(*this);
    returnVal.d_value = valuePtr;

    // Clone is a top-level aggregate.

    returnVal.d_parentType               = bdem_ElemType::BDEM_VOID;
    returnVal.d_parentData               = 0;
    returnVal.d_indexInParent            = -1;
    if (bdem_ElemType::BDEM_VOID != d_dataType) {
        returnVal.d_isTopLevelAggregateNull.createInplace(allocator,
                                                          this->isNul2());
    }

    return returnVal;
}

bsl::ostream& bcem_Aggregate::print(bsl::ostream& stream,
                                    int           level,
                                    int           spacesPerLevel) const
{

    if (isError()) {
        bdeu_Print::indent(stream, level, spacesPerLevel);
        stream << "<ERR> "
               << static_cast<ErrorRecord*>(d_value.ptr())->errorMessage();
        if (spacesPerLevel >= 0) {
             stream << bsl::endl;
        }
    }
    else if (bdem_ElemType::BDEM_VOID == d_dataType) {
        bdeu_Print::indent(stream, level, spacesPerLevel);
        stream << "<null aggregate>";
        if (spacesPerLevel >= 0) {
            stream << bsl::endl;
        }
    }
    else {
        bdem_SchemaAggregateUtil::print(stream,
                                        asElemRef(),
                                        recordDefPtr().ptr(),
                                        level,
                                        spacesPerLevel);
    }

    return stream;
}

// ============================================================================
// Note: All of the remaining code pertains to 'bdeat'.
// ============================================================================

                  // -------------------------------------
                  // local struct bcem_Aggregate_BdeatInfo
                  // -------------------------------------

// PRIVATE ACCESSORS
void bcem_Aggregate_BdeatInfo::setLazyAttributes() const
{
    const char *fieldName = d_record_p->fieldName(d_fieldIndex);

    if (fieldName) {
        d_name_p       = fieldName;
        d_nameLength   = bsl::strlen(fieldName);
        d_annotation_p = fieldName;
    }
    else {
        using namespace bsl;  // in case 'snprintf' is in 'bsl'

        const int BUFFER_SIZE = sizeof d_anonFieldNameBuffer;

        int formattedBytes = snprintf(d_anonFieldNameBuffer,
                                      BUFFER_SIZE,
                                      "FIELD_%d",
                                      d_fieldIndex);

        if (formattedBytes >= BUFFER_SIZE) {
            formattedBytes = BUFFER_SIZE - 1;
        }

        d_name_p       = d_anonFieldNameBuffer;
        d_nameLength   = formattedBytes;
        d_annotation_p = d_anonFieldNameBuffer;
    }
    d_areLazyAttributesSet = true;
}

                  // -------------------------------------
                  // local struct bcem_Aggregate_BdeatUtil
                  // -------------------------------------

// CLASS METHODS
void bcem_Aggregate_BdeatUtil::initInfo(bcem_Aggregate_BdeatInfo *info,
                                        const bdem_RecordDef     *record,
                                        int                       fieldIndex)
{
    BSLS_ASSERT((unsigned)fieldIndex < (unsigned)record->numFields());

    info->d_record_p             = record;
    info->d_fieldIndex           = fieldIndex;
    info->d_areLazyAttributesSet = false;

    const bdem_FieldDef& fieldDef = record->field(fieldIndex);

    info->d_id              = record->fieldId(fieldIndex);
    info->d_formattingMode  = fieldDef.formattingMode();
    info->d_isNullable      = fieldDef.isNullable();
    info->d_hasDefaultValue = fieldDef.hasDefaultValue();
}

int bcem_Aggregate_BdeatUtil::fieldIndexFromName(
                                             const bdem_RecordDef&  record,
                                             const char            *name,
                                             int                    nameLength)
{
    NullTerminatedString fieldName(name, nameLength);

    int fieldIndex = record.fieldIndexExtended(fieldName);

    if (0 > fieldIndex) {
        // Name does not match any field name.  If it matches the pattern
        // "FIELD_n", then return n (where n is a decimal number).

        if (nameLength <= 6 || 0 != bsl::strncmp(fieldName, "FIELD_", 6)) {
            return -1;
        }

        char *endPos = 0;
        fieldIndex = (int)bsl::strtol(fieldName + 6, &endPos, 10);
        if (*endPos || (unsigned)fieldIndex >= (unsigned)record.numFields()) {
            return -1;
        }
    }

    return fieldIndex;
}

// ============================================================================
//           'bdeat_nullablevaluefunctions' overloads and specializations
// ============================================================================

bool bdeat_nullableValueIsNull(
                       const bcem_Aggregate_BdeatUtil::NullableAdapter& object)
{
    return object.d_element_p->isNul2();
}

// ============================================================================
//                      'bdeat_choicefunctions' overloads
// ============================================================================

bool bdeat_choiceHasSelection(const bcem_Aggregate&  object,
                              const char            *selectionName,
                              int                    selectionNameLength)
{
    NullTerminatedString name(selectionName, selectionNameLength);

    return object.hasField(name);
}

int bdeat_choiceMakeSelection(bcem_Aggregate *object,
                              int             selectionId)
{
    enum { BCEM_SUCCESS = 0, BCEM_FAILURE = -1 };

    if (bdeat_ChoiceFunctions::BDEAT_UNDEFINED_SELECTION_ID == selectionId) {
        object->makeSelectionById(bdem_RecordDef::BDEM_NULL_FIELD_ID);
        return BCEM_SUCCESS;
    }

    if (! object->hasFieldById(selectionId)) {
        return BCEM_FAILURE;
    }

    bcem_Aggregate result = object->makeSelectionById(selectionId);

    return bdem_ElemType::BDEM_VOID != result.dataType()
           ? BCEM_SUCCESS
           : BCEM_FAILURE;
}

int bdeat_choiceMakeSelection(bcem_Aggregate *object,
                              const char     *selectionName,
                              int             selectionNameLength)
{
    enum { BCEM_SUCCESS = 0, BCEM_FAILURE = -1 };

    NullTerminatedString name(selectionName, selectionNameLength);

    if (0 == object->hasField(name)) {
        return BCEM_FAILURE;
    }

    bcem_Aggregate result = object->makeSelection(name);

    return bdem_ElemType::BDEM_VOID != result.dataType()
           ? BCEM_SUCCESS
           : BCEM_FAILURE;
}

// ============================================================================
//                     'bdeat_enumfunctions' overloads
// ============================================================================

int bdeat_enumFromInt(bcem_Aggregate *result, int enumId)
{
    const bdem_EnumerationDef *enumDef = result->enumerationConstraint();
    if (! enumDef) {
        return -1;                                                    // RETURN
    }

    const char *enumName = enumDef->lookupName(enumId);
    if (! enumName) {
        return -1;                                                    // RETURN
    }

    if (bdem_ElemType::BDEM_STRING == result->dataType()) {
        result->asElemRef().theModifiableString() = enumName;
                                                           // set string value
    }
    else {
        BSLS_ASSERT(bdem_ElemType::BDEM_INT == result->dataType());
        result->asElemRef().theModifiableInt() = enumId;   // set integer value
    }

    return 0;
}

int bdeat_enumFromString(bcem_Aggregate *result,
                         const char     *string,
                         int             stringLength)
{
    const bdem_EnumerationDef *enumDef = result->enumerationConstraint();
    if (! enumDef) {
        return -1;                                                    // RETURN
    }

    const bsl::string enumName(string, stringLength);
    const int enumId = enumDef->lookupId(enumName.c_str());
    if (bdetu_Unset<int>::unsetValue() == enumId) {
        return -1;                                                    // RETURN
    }

    if (bdem_ElemType::BDEM_STRING == result->dataType()) {
        result->asElemRef().theModifiableString() = enumName;
                                                           // set string value
    }
    else {
        BSLS_ASSERT(bdem_ElemType::BDEM_INT == result->dataType());
        result->asElemRef().theModifiableInt() = enumId;   // set integer value
    }

    return 0;
}

void bdeat_enumToInt(int *result, const bcem_Aggregate& value)
{
    const bdem_EnumerationDef *enumDef = value.enumerationConstraint();
    if (! enumDef) {
        return;
    }

    *result = value.asInt();  // get as integer value
}

void bdeat_enumToString(bsl::string *result, const bcem_Aggregate& value)
{
    const bdem_EnumerationDef *enumDef = value.enumerationConstraint();
    if (! enumDef) {
        return;
    }

    *result = value.asString();  // get as string value
}

// ============================================================================
//                     'bdeat_typecategory' overloads
// ============================================================================

bdeat_TypeCategory::Value
bdeat_typeCategorySelect(const bcem_Aggregate& object)
{
    bdem_ElemType::Type dataType = object.dataType();

    bdeat_TypeCategory::Value result;

    if (bdem_ElemType::isArrayType(dataType)) {
        result = bdeat_TypeCategory::BDEAT_ARRAY_CATEGORY;
    }
    else if (object.enumerationConstraint()) {
        result = bdeat_TypeCategory::BDEAT_ENUMERATION_CATEGORY;
    }
    else if (bdem_ElemType::isScalarType(dataType)) {
        result = bdeat_TypeCategory::BDEAT_SIMPLE_CATEGORY;
    }
    else {
        switch (dataType) {
          case bdem_ElemType::BDEM_LIST:                        // FALL THROUGH
          case bdem_ElemType::BDEM_ROW: {
            result = bdeat_TypeCategory::BDEAT_SEQUENCE_CATEGORY;
          } break;
          case bdem_ElemType::BDEM_CHOICE:
          case bdem_ElemType::BDEM_CHOICE_ARRAY_ITEM: {
            result = bdeat_TypeCategory::BDEAT_CHOICE_CATEGORY;
          } break;
          case bdem_ElemType::BDEM_VOID: {
            // Treat void as a simple type (an error will occur later, where
            // it can be more easily detected).

            result = bdeat_TypeCategory::BDEAT_SIMPLE_CATEGORY;
          } break;
          default: {
            BSLS_ASSERT("Category error" && 0);
            result = static_cast<bdeat_TypeCategory::Value>(-1);
          } break;
        }
    }

    return result;
}

// ============================================================================
//                         'bdeat_typename' overloads
// ============================================================================

const char *bdeat_TypeName_className(const bcem_Aggregate& object)
{
    const bdem_RecordDef *recordDef = object.recordConstraint();
    if (recordDef) {
        return recordDef->recordName();
    }

    const bdem_EnumerationDef *enumerationDef = object.enumerationConstraint();
    if (enumerationDef) {
        return enumerationDef->enumerationName();
    }

    return 0;
}

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2006
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
