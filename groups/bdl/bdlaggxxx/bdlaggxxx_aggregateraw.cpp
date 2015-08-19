// bdlaggxxx_aggregateraw.cpp                                         -*-C++-*-
#include <bdlaggxxx_aggregateraw.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlaggxxx_aggregateraw_cpp,"$Id$ $CSID$")

#include <bdlaggxxx_errorattributes.h>
#include <bdlaggxxx_fieldselector.h>

#include <bdlmxxx_convert.h>
#include <bdlmxxx_list.h>
#include <bdlmxxx_schemaaggregateutil.h>
#include <bdlmxxx_schemautil.h>

#include <bslmf_assert.h>

#include <stdio.h>   // for 'snprintf'
#if defined(BSLS_PLATFORM_CMP_MSVC)
#   define snprintf _snprintf // MSVC names snprintf _snprintf
#endif

namespace BloombergLP {

namespace {

static int s_voidNullnessWord = 1;

                        // ====================
                        // class ArrayCapacitor
                        // ====================

class ArrayCapacitor {
    // Functor that loads the capacity of a sequence container into a parameter
    // passed in the constructor.  The capacity of a sequence container is the
    // number of elements for which memory is already allocated.

    //DATA
    bsl::size_t *d_capacity_p;  // pointer to memory where to load the capacity

    // NOT IMPLEMENTED
    ArrayCapacitor(const ArrayCapacitor&);
    ArrayCapacitor& operator=(const ArrayCapacitor&);

  public:
    // CREATORS
    ArrayCapacitor(bsl::size_t *capacity)
    : d_capacity_p(capacity)
    {
    }

    // MANIPULATORS
    template <class ARRAYTYPE>
    int operator()(ARRAYTYPE *array)
    {
        *d_capacity_p = array->capacity();

        // Return 0 because of the constraint on the signature of this functor
        // by the method 'bcem_Aggregate_Util::visitArray'.  The return value
        // should not be used.

        return 0;
    }
};

                   // ===================
                   // class ArrayInserter
                   // ===================

class ArrayInserter {
    // This class defines a function object to insert one or more elements
    // into a sequence container.  The values are either the default value for
    // the contained type of the sequence container, or copies of a single
    // value specified by a 'bdlmxxx::FieldDef' object at construction.

    // DATA
    int                  d_index;          // array index at which to insert

    int                  d_numElements;    // number of elements to insert

    const bdlmxxx::FieldDef *d_fieldDef_p;     // if non-null, provides value to
                                           // be inserted

    int                  d_length;         // pre-insertion length of the
                                           // associated array

    void                *d_data_p;         // address of first element inserted

    bool                 d_areValuesNull;  // are the values being inserted
                                           // null

    // PRIVATE TYPES
    template <class TYPE>
    struct SignChecker {
        // TBD REMOVE

        enum {
            IS_SIGNED = 1
        };
    };

    // NOT IMPLEMENTED
    ArrayInserter(const ArrayInserter&);
    ArrayInserter& operator=(const ArrayInserter&);

  public:
    // CREATORS
    ArrayInserter(int                  index,
                  int                  numElements,
                  const bdlmxxx::FieldDef *fieldDef,
                  bool                 areValuesNull = false);
        // Create an inserter for inserting the specified 'numElements' at the
        // specified 'index' in an array that is subsequently supplied to the
        // 'operator()' method.  If the specified 'fieldDef' is non-null, then
        // 'fieldDef' provides the value for the elements to be inserted;
        // otherwise the default value for the array element type is used.  If
        // 'index < 0', elements are appended to the end of the array.

    // MANIPULATORS
    template <class ARRAYTYPE>
    int operator()(ARRAYTYPE *array);
        // Insert elements into the specified 'array' of parameterized
        // 'ARRAYTYPE' as indicated by the arguments supplied at construction.
        // Return 0 on success and a non-zero value otherwise.  'ARRAYTYPE'
        // shall be a sequence container (e.g., 'vector') that (1) contains
        // elements that can be assigned the value specified by the 'fieldDef'
        // supplied at construction, if any; (2) supports random-access,
        // STL-compatible 'begin' and 'insert' methods, and (3) defines a
        // 'value_type' 'typedef'.

    // ACCESSORS
    void *data() const;
        // Return the address of the modifiable first element inserted into the
        // array supplied to the most recent call to 'operator()' on this
        // inserter, or 0 if 'operator()' has not yet been called.

    int length() const;
        // Return the length of the array supplied to the most recent call to
        // 'operator()' on this inserter, or 0 if 'operator()'
        // has not yet been called.  Note that the length returned is the
        // length prior to the call to 'operator()' (i.e., prior to inserting
        // elements).
};

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
    template <class ARRAYTYPE>
    int operator()(ARRAYTYPE *array)
    {
        if (d_pos + d_numItems > (int)array->size()) {
            return -1;                                                // RETURN
        }

        typename ARRAYTYPE::iterator start = array->begin();
        bsl::advance(start, d_pos);
        typename ARRAYTYPE::iterator finish = start;
        bsl::advance(finish, d_numItems);
        array->erase(start, finish);

        return 0;
    }
};

                        // ===================
                        // class ArrayReserver
                        // ===================

class ArrayReserver {
    // This class defines a function object to reserve memory in a sequence
    // container for the number of objects indicated at construction.

    // DATA
    bsl::size_t d_numItems; // number of items to reserve

    // NOT IMPLEMENTED
    ArrayReserver(const ArrayReserver&);
    ArrayReserver& operator=(const ArrayReserver&);

  public:
    // CREATORS
    explicit
    ArrayReserver(bsl::size_t numItems)
    : d_numItems(numItems)
    {
    }

    // ACCESSORS
    template <class ARRAYTYPE>
    int operator()(ARRAYTYPE *array) const
    {
        array->reserve(d_numItems);
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
    char              d_buffer[128];  // buffer large enough for *most* uses

    char             *d_string_p;     // address of 'd_buffer', or allocated
                                      // string if 128 is not sufficient

    bslma::Allocator *d_allocator_p;  // memory allocator (held, not owned)

    // NOT IMPLEMENTED
    NullTerminatedString(const NullTerminatedString&);
    NullTerminatedString& operator=(const NullTerminatedString&);

  public:
    // CREATORS
    NullTerminatedString(const char       *string,
                         int               length,
                         bslma::Allocator *basicAllocator = 0)
    : d_string_p(d_buffer)
    , d_allocator_p(bslma::Default::allocator(basicAllocator))
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
    operator const char*() const
    {
        return d_string_p;
    }
};

                        // -------------------
                        // class ArrayInserter
                        // -------------------

template <>
struct ArrayInserter::SignChecker<unsigned char> {
    // TBD REMOVE
    enum {
        IS_SIGNED = 0
    };
};

template <>
struct ArrayInserter::SignChecker<unsigned short> {
    // TBD REMOVE
    enum {
        IS_SIGNED = 0
    };
};

template <>
struct ArrayInserter::SignChecker<unsigned int> {
    // TBD REMOVE
    enum {
        IS_SIGNED = 0
    };
};

template <>
struct ArrayInserter::SignChecker<bsls::Types::Uint64> {
    // TBD REMOVE
    enum {
        IS_SIGNED = 0
    };
};

// CREATORS
inline
ArrayInserter::ArrayInserter(int                  index,
                             int                  numElements,
                             const bdlmxxx::FieldDef *fieldDef,
                             bool                 areValuesNull)
: d_index(index)
, d_numElements(numElements)
, d_fieldDef_p(fieldDef)
, d_length(0)
, d_data_p(0)
, d_areValuesNull(areValuesNull)
{
}

// MANIPULATORS
template <class ARRAYTYPE>
int ArrayInserter::operator()(ARRAYTYPE *array)
{
    BSLS_ASSERT_SAFE(array);

    d_length = (int)array->size();
    if (d_index < 0) {              // insert at end
        d_index = d_length;
    }
    else if (d_index > d_length) {  // range error
        return -1;                                                    // RETURN
    }

    // Construct value to be inserted.

    typedef typename ARRAYTYPE::value_type value_type;
    if (d_areValuesNull) {
        BSLMF_ASSERT(SignChecker<value_type>::IS_SIGNED == 1);

        value_type nullValue(bdltuxxx::Unset<value_type>::unsetValue());

        // Insert.

        array->insert(array->begin() + d_index, d_numElements, nullValue);
    }
    else {
        value_type defaultValue(d_fieldDef_p
        ? *static_cast<const value_type*>(d_fieldDef_p->defaultValue().data())
        : value_type());

        // Insert.

        array->insert(array->begin() + d_index, d_numElements, defaultValue);
    }

    // Set 'd_data_p' to the address of the first inserted element.

    d_data_p = &(*array)[d_index];

    return 0;
}

// ACCESSORS
inline
void *ArrayInserter::data() const
{
    return d_data_p;
}

inline
int ArrayInserter::length() const
{
    return d_length;
}

}  // close unnamed namespace

namespace bdlaggxxx {
                        // -----------------------
                        // class AggregateRaw
                        // -----------------------

// PRIVATE MANIPULATORS
int AggregateRaw::descendIntoArrayItem(
                                        ErrorAttributes *errorDescription,
                                        int                   index,
                                        bool                  makeNonNullFlag)
{
    BSLS_ASSERT_SAFE(errorDescription);

    void *valuePtr = d_value_p;

    bdlmxxx::ElemType::Type  itemType;
    void                *itemPtr = 0;

    switch (d_dataType) {
      case bdlmxxx::ElemType::BDEM_TABLE: {
        bdlmxxx::Table& table = *(bdlmxxx::Table *) valuePtr;
        if ((unsigned)index >= (unsigned)table.numRows()) {
            break;  // out of bounds
        }

        if (isNillableScalarArray()) {
            bdlmxxx::Row     *row = makeNonNullFlag
                              ? (bdlmxxx::Row *) &table.theModifiableRow(index)
                              : const_cast<bdlmxxx::Row *>(&table.theRow(index));
            bdlmxxx::ElemRef  ref = (*row)[0];

            itemType                   = ref.type();
            itemPtr                    = ref.dataRaw();
            if (d_recordDef_p) {
                const bdlmxxx::FieldDef& field = d_recordDef_p->field(0);
                d_recordDef_p              = field.recordConstraint();
                d_fieldDef_p               = &field;
            }
            d_parentType               = bdlmxxx::ElemType::BDEM_ROW;
            d_parentData_p             = row;
            d_indexInParent            = 0;
            *d_isTopLevelAggregateNull_p = 0;  // don't care

            // d_schema_p is unchanged.
            d_value_p  = itemPtr;
            d_dataType = itemType;

            return 0;                                                 // RETURN
        }
        else if (d_fieldDef_p
              && bdlat_FormattingMode::e_NILLABLE ==
                                              d_fieldDef_p->formattingMode()) {
            itemType = bdlmxxx::ElemType::BDEM_ROW;
            itemPtr  = makeNonNullFlag
                     ? (bdlmxxx::Row *) &table.theModifiableRow(index)
                     : const_cast<bdlmxxx::Row *>(&table.theRow(index));
        }
        else {
            itemType = bdlmxxx::ElemType::BDEM_ROW;
            itemPtr  = &table.theModifiableRow(index);
        }
      } break;
      case bdlmxxx::ElemType::BDEM_CHOICE_ARRAY: {
        bdlmxxx::ChoiceArray& choiceArray = *(bdlmxxx::ChoiceArray*)valuePtr;
        if ((unsigned)index >= (unsigned)choiceArray.length()) {
            break;  // out of bounds
        }

        itemType = bdlmxxx::ElemType::BDEM_CHOICE_ARRAY_ITEM;
        if (d_fieldDef_p
         && bdlat_FormattingMode::e_NILLABLE ==
                                              d_fieldDef_p->formattingMode()) {
            itemPtr = makeNonNullFlag
             ? &choiceArray.theModifiableItem(index)
             : const_cast<bdlmxxx::ChoiceArrayItem *>(&choiceArray.theItem(index));
        }
        else {
            itemPtr  = &choiceArray.theModifiableItem(index);
        }
      } break;
      default: {
        if (bdlmxxx::ElemType::isArrayType(d_dataType)) {  // scalar array type
            AggregateRaw_ArrayIndexer indexer(index);
            if (AggregateRaw_Util::visitArray(valuePtr,
                                                   d_dataType,
                                                   &indexer)) {
                break;  // out of bounds
            }
            itemType = bdlmxxx::ElemType::fromArrayType(d_dataType);
            itemPtr  = indexer.data();
        }
        else {
            errorDescription->setCode(ErrorCode::BCEM_NOT_AN_ARRAY);
            bsl::string description =
                                "Attempt to index a non-array object of type ";
            description += bdlmxxx::ElemType::toAscii(d_dataType);
            errorDescription->setDescription(description);
            return -1;                                                // RETURN
        }
      } break;
    }

    if (! itemPtr) {
        // If got here, then have out-of-bounds index
        bsl::ostringstream oss;
        oss << "Invalid array index " << index
            << " used for " << bdlmxxx::ElemType::toAscii(d_dataType);
        errorDescription->setDescription(oss.str());
        errorDescription->setCode(ErrorCode::BCEM_BAD_ARRAYINDEX);
        return -1;                                                    // RETURN
    }

    // adjust nullness info first

    d_parentType    = d_dataType;
    d_parentData_p  = d_value_p;
    d_indexInParent = index;
    *d_isTopLevelAggregateNull_p = 0;  // don't care

    // d_schema_p, d_recordDef_p, and d_fieldDef_p are unchanged.

    d_value_p  = itemPtr;
    d_dataType = itemType;

    return 0;
}

int AggregateRaw::descendIntoField(
                                   ErrorAttributes      *errorDescription,
                                   const FieldSelector&  fieldSelector,
                                   bool                       makeNonNullFlag)
{
    BSLS_ASSERT_SAFE(errorDescription);

    if (fieldSelector.isEmpty()) {
        return 1;                                                     // RETURN
    }
    else if (fieldSelector.isName()) {
        return descendIntoFieldByName(errorDescription,
                                      fieldSelector.name());          // RETURN
    }
    else {  // 'fieldSelector.isIndex()'
        return descendIntoArrayItem(errorDescription,
                                    fieldSelector.index(),
                                    makeNonNullFlag);                 // RETURN
    }
}

int AggregateRaw::descendIntoFieldById(
                                        ErrorAttributes *errorDescription,
                                        int                   fieldId)
{
    BSLS_ASSERT_SAFE(errorDescription);

    int fieldIndex;
    if (0 != getFieldIndex(&fieldIndex,
                           errorDescription,
                           fieldId,
                           "fieldById or setFieldById")) {
        return -1;                                                    // RETURN
    }

    return descendIntoFieldByIndex(errorDescription, fieldIndex);
}

int
AggregateRaw::descendIntoFieldByIndex(
                                        ErrorAttributes *errorDescription,
                                        int                   fieldIndex)
{
    BSLS_ASSERT_SAFE(errorDescription);

    void *valuePtr = d_value_p;

    switch (d_dataType) {
      case bdlmxxx::ElemType::BDEM_LIST:
        // Extract bdlmxxx::Row from bdlmxxx::Choice, then fall through to ROW case.

        valuePtr = &((bdlmxxx::List*)valuePtr)->row();
                                                                // FALL THROUGH
      case bdlmxxx::ElemType::BDEM_ROW: {
        bdlmxxx::Row& row = *(bdlmxxx::Row *)valuePtr;
        if ((unsigned)fieldIndex >= (unsigned)row.length()) {
            bsl::ostringstream oss;
            oss << "Invalid field index " << fieldIndex
                << " specified for "
                << bdlmxxx::ElemType::toAscii(d_dataType)
                << " \"" << AggregateRaw_Util::recordName(d_recordDef_p)
                << '"';
            errorDescription->setDescription(oss.str());
            errorDescription->setCode(ErrorCode::BCEM_BAD_FIELDINDEX);
            return -1;                                                // RETURN
        }

        // adjust nullness info first
        d_parentType    = d_dataType;
        d_parentData_p  = d_value_p;
        d_indexInParent = fieldIndex;
        *d_isTopLevelAggregateNull_p = 0;  // don't care

        if (d_recordDef_p) {
            const bdlmxxx::FieldDef& field = d_recordDef_p->field(fieldIndex);
            d_recordDef_p = field.recordConstraint();
            d_fieldDef_p  = &field;
        }

        bdlmxxx::ElemRef dataRef = row[fieldIndex];
        d_dataType = dataRef.type();
        d_value_p  = dataRef.dataRaw();
      } break;

      case bdlmxxx::ElemType::BDEM_CHOICE:
        // Extract bdlmxxx::ChoiceArrayItem from bdlmxxx::Choice, then fall through
        // to the CHOICE_ARRAY_ITEM case.

        valuePtr = &((bdlmxxx::Choice*)valuePtr)->item();
                                                                // FALL THROUGH
      case bdlmxxx::ElemType::BDEM_CHOICE_ARRAY_ITEM: {
        bdlmxxx::ChoiceArrayItem& choiceItem = *(bdlmxxx::ChoiceArrayItem*)valuePtr;

        int selectorIndex = choiceItem.selector();
        if (fieldIndex >= 0 && fieldIndex != selectorIndex) {
            bsl::ostringstream oss;

            oss << "Attempt to access field "
                << fieldIndex
                << " in " << bdlmxxx::ElemType::toAscii(d_dataType)
                << " \"" << AggregateRaw_Util::recordName(d_recordDef_p)
                << "\" but field " << selectorIndex
                << " is currently selected";
            errorDescription->setDescription(oss.str());
            errorDescription->setCode(ErrorCode::BCEM_NOT_SELECTED);
            return -1;                                                // RETURN
        }
        else if (-1 == selectorIndex) {
            // No current selection.  Set to a void object.
            reset();
            break;
        }

        // fieldIndex == selectorIndex or should be considered equivalent
        // adjust nullness info first
        d_parentType    = d_dataType;
        d_parentData_p  = d_value_p;
        d_indexInParent = selectorIndex;
        *d_isTopLevelAggregateNull_p = 0;  // don't care

        // Descend into current selection
        if (d_recordDef_p) {
            const bdlmxxx::FieldDef& field = d_recordDef_p->field(selectorIndex);
            d_recordDef_p              = field.recordConstraint();
            d_fieldDef_p               = &field;
        }

        bdlmxxx::ElemRef dataRef = choiceItem.selection();
        d_dataType = dataRef.type();
        d_value_p  = dataRef.dataRaw();
      } break;

      default: {
          bsl::ostringstream oss;
          oss << "Attempt to access field index "
              << fieldIndex << " on non-record type "
              << bdlmxxx::ElemType::toAscii(d_dataType);
          errorDescription->setDescription(oss.str());
          errorDescription->setCode(ErrorCode::BCEM_NOT_A_RECORD);
          return -1;                                                  // RETURN
      }
    }

    return 0;
}

int AggregateRaw::descendIntoFieldByName(
                                        ErrorAttributes *errorDescription,
                                        const char           *fieldName)
{
    BSLS_ASSERT_SAFE(errorDescription);

    int                   fieldIndex   = -1;
    const bdlmxxx::RecordDef *parentRecDef = 0;

    do {
        if ((bdlmxxx::ElemType::BDEM_CHOICE            == d_dataType
          || bdlmxxx::ElemType::BDEM_CHOICE_ARRAY_ITEM == d_dataType)
          && ! (fieldName && fieldName[0])) {
            // Empty field name given to choice.  Descend into current
            // selection.  In order to descend into anonymous choices, we need
            // to get the real field index, not the
            // 'bdltuxxx::Unset<int>::unsetValue()' place holder for "current
            // selection".
            fieldIndex = selectorIndex();
        }
        else {
            if (0 != getFieldIndex(&fieldIndex,
                                   errorDescription,
                                   fieldName,
                                   "field or setField")) {
                return -1;                                            // RETURN
            }
        }

        BSLS_ASSERT(fieldIndex >= -1);

        parentRecDef = d_recordDef_p;
        if (0 != descendIntoFieldByIndex(errorDescription,
                                         fieldIndex)) {
            return -1;                                                // RETURN
        }

        // Repeat the above operations so long as the above lookup finds
        // an unnamed CHOICE or unnamed LIST:
    } while (bdlmxxx::ElemType::isAggregateType(this->d_dataType)
          && 0 == parentRecDef->fieldName(fieldIndex));

    return 0;
}

// PRIVATE ACCESSORS
int AggregateRaw::getFieldIndex(int                  *index,
                                     ErrorAttributes *errorResult,
                                     const char           *fieldName,
                                     const char           *caller) const
{
    BSLS_ASSERT_SAFE(index);
    BSLS_ASSERT_SAFE(errorResult);
    BSLS_ASSERT_SAFE(fieldName);
    BSLS_ASSERT_SAFE(caller);

    if (!bdlmxxx::ElemType::isAggregateType(d_dataType)) {
        bsl::ostringstream oss;
        oss << "Attempt to call " << caller
            << " with field name \"" << fieldName
            << "\" on array type: " << bdlmxxx::ElemType::toAscii(d_dataType);
        errorResult->setDescription(oss.str());
        errorResult->setCode(ErrorCode::BCEM_NOT_A_RECORD);
        return -1;                                                    // RETURN
    }
    else if (! d_recordDef_p) {
        bsl::ostringstream oss;
        oss << "Attempt to call " << caller
            << " with field name \"" << fieldName
            << "\" on unconstrained " << bdlmxxx::ElemType::toAscii(d_dataType);
        errorResult->setDescription(oss.str());
        errorResult->setCode(ErrorCode::BCEM_NOT_A_RECORD);
        return -1;                                                    // RETURN
    }
    else if (bdlmxxx::RecordDef::BDEM_CHOICE_RECORD == d_recordDef_p->recordType()
          && ! (fieldName && fieldName[0])) {
        *index = -1;
        return 0;                                                     // RETURN
    }

    *index = d_recordDef_p->fieldIndexExtended(fieldName);
    if (*index < 0) {
        bsl::ostringstream oss;
        oss << "Invalid field name \"" << fieldName
            << "\" in " << bdlmxxx::ElemType::toAscii(d_dataType)
            << " \"" << AggregateRaw_Util::recordName(d_recordDef_p)
            << "\" passed to " << caller;
        errorResult->setDescription(oss.str());
        errorResult->setCode(ErrorCode::BCEM_BAD_FIELDNAME);
        return -1;                                                    // RETURN
    }

    return 0;
}

int AggregateRaw::getFieldIndex(int                  *index,
                                     ErrorAttributes *errorResult,
                                     int                   fieldId,
                                     const char           *caller) const
{
    BSLS_ASSERT_SAFE(index);
    BSLS_ASSERT_SAFE(errorResult);
    BSLS_ASSERT_SAFE(caller);

    if (!bdlmxxx::ElemType::isAggregateType(d_dataType)) {
        bsl::ostringstream oss;
        oss << "Attempt to call " << caller
            << " with field id " << fieldId
            << " on array type: " << bdlmxxx::ElemType::toAscii(d_dataType);
        errorResult->setDescription(oss.str());
        errorResult->setCode(ErrorCode::BCEM_NOT_A_RECORD);
        return -1;                                                    // RETURN
    }
    else if (! d_recordDef_p) {
        bsl::ostringstream oss;
        oss << "Attempt to call " << caller
            << " with field id " << fieldId
            << " on unconstrained " << bdlmxxx::ElemType::toAscii(d_dataType);
        errorResult->setDescription(oss.str());
        errorResult->setCode(ErrorCode::BCEM_NOT_A_RECORD);
        return -1;                                                    // RETURN
    }
    else if (bdlmxxx::RecordDef::BDEM_CHOICE_RECORD == d_recordDef_p->recordType()
          && bdlmxxx::RecordDef::BDEM_NULL_FIELD_ID == fieldId) {
        // It is legal to pass 'BDEM_NULL_FIELD_ID' to a choice.
        *index = -1;
        return 0;                                                     // RETURN
    }

    *index = d_recordDef_p->fieldIndex(fieldId);
    if (*index < 0) {
        bsl::ostringstream oss;
        oss << "Invalid field ID " << fieldId
            << " in " << bdlmxxx::ElemType::toAscii(d_dataType)
            << " \"" << AggregateRaw_Util::recordName(d_recordDef_p)
            << "\" passed to " << caller;
        errorResult->setDescription(oss.str());
        errorResult->setCode(ErrorCode::BCEM_BAD_FIELDID);
        return -1;                                                    // RETURN
    }

    return 0;
}

bool AggregateRaw::isNillableScalarArray() const
{
    if (bdlmxxx::ElemType::BDEM_TABLE != d_dataType || !d_recordDef_p) {
        return false;                                                 // RETURN
    }

    if (1 == d_recordDef_p->numFields()) {
        if (!d_recordDef_p->fieldName(0)
         && bdlmxxx::ElemType::isScalarType(d_recordDef_p->field(0).elemType())) {
            return true;                                              // RETURN
        }
    }

    return false;
}

int AggregateRaw::makeSelectionByIndexRaw(
                                        AggregateRaw    *field,
                                        ErrorAttributes *errorDescription,
                                        int                   index) const
{
    BSLS_ASSERT_SAFE(field);
    BSLS_ASSERT_SAFE(errorDescription);
    BSLS_ASSERT_SAFE(-1 == index || 0 <= index);

    bdlmxxx::ChoiceArrayItem *choice = 0;

    switch (d_dataType) {
      case bdlmxxx::ElemType::BDEM_CHOICE: {
        choice = &((bdlmxxx::Choice *)d_value_p)->item();
      } break;
      case bdlmxxx::ElemType::BDEM_CHOICE_ARRAY_ITEM: {
        choice = (bdlmxxx::ChoiceArrayItem *)d_value_p;
      } break;
      default: {
        bsl::string description = "makeSelection called on aggregate of type ";
        description += bdlmxxx::ElemType::toAscii(d_dataType);
        errorDescription->setDescription(description);
        errorDescription->setCode(ErrorCode::BCEM_NOT_A_CHOICE);
        return -1;                                                    // RETURN
      }
    }

    if (-1 != index && index >= choice->numSelections()) {
        bsl::ostringstream oss;
        oss << "Invalid selection index "
            << index << " in "
            << bdlmxxx::ElemType::toAscii(d_dataType) << " \""
            << AggregateRaw_Util::recordName(d_recordDef_p)
            << "\" passed to makeSelectionByIndex";
        errorDescription->setDescription(oss.str());
        errorDescription->setCode(ErrorCode::BCEM_BAD_FIELDINDEX);
        return -1;                                                    // RETURN
    }

    choice->makeSelection(index);

    return selection(field, errorDescription);
}

int AggregateRaw::toEnum(ErrorAttributes *errorDescription,
                              const char           *value,
                              bslmf::MetaInt<1>) const
{
    BSLS_ASSERT_SAFE(errorDescription);

    const bdlmxxx::EnumerationDef *enumDef = enumerationConstraint();
    const int enumId = value
                     ? enumDef->lookupId(value)
                     : bdltuxxx::Unset<int>::unsetValue();

    if (bdltuxxx::Unset<int>::isUnset(enumId) && 0 != value && 0 != value[0]) {
        bsl::ostringstream oss;
        oss << "Attempt to set enumerator name \"" << value
            << "\" in enumeration \""
            << AggregateRaw_Util::enumerationName(enumDef) << '\"';
        errorDescription->setDescription(oss.str());
        errorDescription->setCode(ErrorCode::BCEM_BAD_ENUMVALUE);
        return -1;                                                    // RETURN
    }

    // If we got here, we're either a (1) top-level aggregate, (2) CHOICE or
    // CHOICE_ARRAY_ITEM that has been selected (hence, non-null), or (3) an
    // item in a ROW.

    if (bdlmxxx::ElemType::BDEM_INT == dataType()) {
        asElemRef().theModifiableInt() = enumId;
    }
    else {
        asElemRef().theModifiableString() = value ? value : "";
    }

    return 0;
}

int AggregateRaw::toEnum(ErrorAttributes     *errorDescription,
                              const bdlmxxx::ConstElemRef&  value,
                              bslmf::MetaInt<1>) const
{
    BSLS_ASSERT_SAFE(errorDescription);

    int intValue;
    switch (value.type()) {
      case bdlmxxx::ElemType::BDEM_CHAR: {
        intValue = value.theChar();
      } break;
      case bdlmxxx::ElemType::BDEM_SHORT: {
        intValue = value.theShort();
      } break;
      case bdlmxxx::ElemType::BDEM_INT: {
        intValue = value.theInt();
      } break;
      case bdlmxxx::ElemType::BDEM_INT64: {
        intValue = (int) value.theInt64();
      } break;
      case bdlmxxx::ElemType::BDEM_FLOAT: {
        intValue = (int) value.theFloat();
      } break;
      case bdlmxxx::ElemType::BDEM_DOUBLE: {
        intValue = (int) value.theDouble();
      } break;
      case bdlmxxx::ElemType::BDEM_BOOL: {
        intValue = value.theBool();
      } break;
      case bdlmxxx::ElemType::BDEM_STRING: {
        if (value.isNull()) {
            makeNull();
            return 0;                                                 // RETURN
        }
        return toEnum(errorDescription,
                      value.theString().c_str(),
                      bslmf::MetaInt<1>());                           // RETURN
      } break;
      default: {
        bsl::ostringstream oss;
        oss << "Invalid conversion from \""
            << bdlmxxx::ElemType::toAscii(value.type())
            << "\" to enumeration \""
            << AggregateRaw_Util::enumerationName(enumerationConstraint())
            << '\"';
        errorDescription->setDescription(oss.str());
        errorDescription->setCode(ErrorCode::BCEM_BAD_CONVERSION);
        return -1;                                                    // RETURN
      }
    }

    if (value.isNull()) {
        makeNull();
        return 0;                                                     // RETURN
    }

    // Got here if value is numeric and has been converted to int.

    return toEnum(errorDescription, intValue, bslmf::MetaInt<0>());
}

int AggregateRaw::toEnum(ErrorAttributes *errorDescription,
                              const int&            value,
                              bslmf::MetaInt<0>) const
{
    BSLS_ASSERT_SAFE(errorDescription);

    const bdlmxxx::EnumerationDef *enumDef  = enumerationConstraint();
    const char                *enumName = enumDef->lookupName(value);

    if (bdltuxxx::Unset<int>::unsetValue() != value && !enumName) {

        // Failed lookup

        bsl::ostringstream oss;
        oss << "Attempt to set enumerator ID " << value
            << " in enumeration \""
            << AggregateRaw_Util::enumerationName(enumDef);
        errorDescription->setDescription(oss.str());
        errorDescription->setCode(ErrorCode::BCEM_BAD_ENUMVALUE);
        return -1;                                                    // RETURN
    }

    // If we got here, we're either a (1) top-level aggregate, (2) CHOICE or
    // CHOICE_ARRAY_ITEM that has been selected (hence, non-null), or (3) an
    // item in a ROW.

    if (bdlmxxx::ElemType::BDEM_INT == dataType()) {
        asElemRef().theModifiableInt() = value;
    }
    else {
        asElemRef().theModifiableString() = enumName ? enumName : "";
    }

    return 0;
}

// CLASS METHODS
bool AggregateRaw::areEquivalent(const AggregateRaw& lhs,
                                      const AggregateRaw& rhs)
{
    if (areIdentical(lhs, rhs)) {

        // identical aggregates

        return true;                                                  // RETURN
    }
    else if (lhs.dataType() != rhs.dataType()
          || lhs.isNull()   != rhs.isNull()) {

        // different types or nullness mismatch

        return false;                                                 // RETURN
    }
    else if (lhs.recordConstraint() && rhs.recordConstraint()) {
        if (lhs.recordConstraint() != rhs.recordConstraint()
         && ! bdlmxxx::SchemaUtil::areEquivalent(*lhs.recordConstraint(),
                                             *rhs.recordConstraint())) {

            // different record definitions

            return false;                                             // RETURN
        }
    }
    else if (lhs.recordConstraint() || rhs.recordConstraint()) {

        // one has record definition, other doesn't

        return false;                                                 // RETURN
    }

    // Data types, record definitions, and nullness match, so compare values.

    const bdlmxxx::Descriptor *descriptor =
                            bdlmxxx::ElemAttrLookup::lookupTable()[lhs.dataType()];

    return lhs.isNull() || descriptor->areEqual(lhs.data(), rhs.data());
}

bool AggregateRaw::areIdentical(const AggregateRaw& lhs,
                                     const AggregateRaw& rhs)
{
    // If identical in these respects, then there is no need to check their
    // respective nullness attributes.  We need to special case for aggregates
    // of type 'bdlmxxx::ElemType::BDEM_VOID' as those have empty values.

    return bdlmxxx::ElemType::BDEM_VOID != lhs.dataType()
        && lhs.dataType()           == rhs.dataType()
        && lhs.data()               == rhs.data();
}

// CREATORS
AggregateRaw::AggregateRaw()
: d_dataType(bdlmxxx::ElemType::BDEM_VOID)
, d_schema_p(0)
, d_recordDef_p(0)
, d_fieldDef_p(0)
, d_value_p(0)
, d_parentType(bdlmxxx::ElemType::BDEM_VOID)
, d_parentData_p(0)
, d_indexInParent(-1)
, d_isTopLevelAggregateNull_p(0)
{
}

AggregateRaw::AggregateRaw(const AggregateRaw& original)
: d_dataType(original.d_dataType)
, d_schema_p(original.d_schema_p)
, d_recordDef_p(original.d_recordDef_p)
, d_fieldDef_p(original.d_fieldDef_p)
, d_value_p(original.d_value_p)
, d_parentType(original.d_parentType)
, d_parentData_p(original.d_parentData_p)
, d_indexInParent(original.d_indexInParent)
, d_isTopLevelAggregateNull_p(original.d_isTopLevelAggregateNull_p)
{
}

// MANIPULATORS
AggregateRaw& AggregateRaw::operator=(const AggregateRaw& rhs)
{
    if (this != &rhs) {
        d_dataType = rhs.d_dataType;
        d_schema_p = rhs.d_schema_p;
        d_recordDef_p = rhs.d_recordDef_p;
        d_fieldDef_p = rhs.d_fieldDef_p;
        d_value_p = rhs.d_value_p;
        d_parentType = rhs.d_parentType;
        d_parentData_p = rhs.d_parentData_p;
        d_indexInParent = rhs.d_indexInParent;
        d_isTopLevelAggregateNull_p = rhs.d_isTopLevelAggregateNull_p;
    }
    return *this;
}

void AggregateRaw::clearParent()
{
    d_parentType    = bdlmxxx::ElemType::BDEM_VOID;
    d_parentData_p    = 0;
    d_indexInParent = -1;
}

void AggregateRaw::reset()
{
    d_dataType                  = bdlmxxx::ElemType::BDEM_VOID;
    d_schema_p                  = 0;
    d_recordDef_p               = 0;
    d_fieldDef_p                = 0;
    d_value_p                   = 0;
    d_parentType                = bdlmxxx::ElemType::BDEM_VOID;
    d_parentData_p              = 0;
    d_indexInParent             = -1;
    d_isTopLevelAggregateNull_p = 0;
}

                                  // Aspects

void AggregateRaw::swap(AggregateRaw& rhs)
{
    bsl::swap(d_dataType, rhs.d_dataType);
    bsl::swap(d_schema_p, rhs.d_schema_p);
    bsl::swap(d_recordDef_p, rhs.d_recordDef_p);
    bsl::swap(d_fieldDef_p, rhs.d_fieldDef_p);
    bsl::swap(d_value_p, rhs.d_value_p);
    bsl::swap(d_parentType, rhs.d_parentType);
    bsl::swap(d_parentData_p, rhs.d_parentData_p);
    bsl::swap(d_indexInParent, rhs.d_indexInParent);
    bsl::swap(d_isTopLevelAggregateNull_p, rhs.d_isTopLevelAggregateNull_p);
}

// ACCESSORS
int AggregateRaw::anonymousField(
                                  AggregateRaw    *object,
                                  ErrorAttributes *errorDescription) const
{
    // initialize to error value

    if (d_recordDef_p && 1 < d_recordDef_p->numAnonymousFields()) {
        bsl::ostringstream oss;
        oss << "Called anonymousField for "
            << " \"" << AggregateRaw_Util::recordName(d_recordDef_p)
            << "\" that contains ambiguous anonymous fields";
        errorDescription->setDescription(oss.str());
        errorDescription->setCode(ErrorCode::BCEM_AMBIGUOUS_ANON);
        return -1;                                                    // RETURN
    }

    return anonymousField(object, errorDescription, 0);
}

int AggregateRaw::anonymousField(AggregateRaw    *object,
                                      ErrorAttributes *errorDescription,
                                      int                   index) const
{
    BSLS_ASSERT_SAFE(object);
    BSLS_ASSERT_SAFE(errorDescription);

    if (! d_recordDef_p) {
        bsl::ostringstream oss;
        oss << "Called anonymousField on unconstrained "
            << bdlmxxx::ElemType::toAscii(d_dataType)
            << " object";
        errorDescription->setDescription(oss.str());
        errorDescription->setCode(ErrorCode::BCEM_NOT_A_RECORD);
        return -1;                                                    // RETURN
    }

    int numAnonFields = d_recordDef_p->numAnonymousFields();
    if (0 == numAnonFields) {
        bsl::ostringstream oss;
        oss << "Called anonymousField for "
            << bdlmxxx::ElemType::toAscii(d_dataType)
            << " \"" << AggregateRaw_Util::recordName(d_recordDef_p)
            << "\" that contains no anonymous fields";
        errorDescription->setDescription(oss.str());
        errorDescription->setCode(ErrorCode::BCEM_BAD_FIELDINDEX);
        return -1;                                                    // RETURN
    }
    else if ((unsigned)index >= (unsigned)numAnonFields) {
        bsl::ostringstream oss;
        oss << "Invalid index " << index << "passed to anonymousField for "
            << bdlmxxx::ElemType::toAscii(d_dataType) << " \""
            << AggregateRaw_Util::recordName(d_recordDef_p) << '"';
        errorDescription->setDescription(oss.str());
        errorDescription->setCode(ErrorCode::BCEM_BAD_FIELDINDEX);
        return -1;                                                    // RETURN
    }

    int fldIdx = 0;
    for (int anonIdx = -1; anonIdx < index; ++fldIdx) {
        if (0 == d_recordDef_p->fieldName(fldIdx)) {
            ++anonIdx;
            if (anonIdx == index) {
                break;  // break loop without incrementing fldIdx
            }
        }
    }

    return fieldByIndex(object, errorDescription, fldIdx);
}

bool AggregateRaw::asBool() const
{
    return convertScalar<bool>();
}

char AggregateRaw::asChar() const
{
    return convertScalar<char>();
}

short AggregateRaw::asShort() const
{
    return convertScalar<short>();
}

int AggregateRaw::asInt() const
{
    return convertScalar<int>();
}

bsls::Types::Int64 AggregateRaw::asInt64() const
{
    return convertScalar<bsls::Types::Int64>();
}

float AggregateRaw::asFloat() const
{
    return convertScalar<float>();
}

double AggregateRaw::asDouble() const
{
    return convertScalar<double>();
}

bdlt::Datetime AggregateRaw::asDatetime() const
{
    return convertScalar<bdlt::Datetime>();
}

bdlt::DatetimeTz AggregateRaw::asDatetimeTz() const
{
    return convertScalar<bdlt::DatetimeTz>();
}

bdlt::Date AggregateRaw::asDate() const
{
    return convertScalar<bdlt::Date>();
}

bdlt::DateTz AggregateRaw::asDateTz() const
{
    return convertScalar<bdlt::DateTz>();
}

bdlt::Time AggregateRaw::asTime() const
{
    return convertScalar<bdlt::Time>();
}

bdlt::TimeTz AggregateRaw::asTimeTz() const
{
    return convertScalar<bdlt::TimeTz>();
}

const bdlmxxx::ElemRef AggregateRaw::asElemRef() const
{
    if (!d_parentData_p) {

        // top-level aggregate

        const bdlmxxx::Descriptor *descriptor =
                                bdlmxxx::ElemAttrLookup::lookupTable()[d_dataType];
        int *nullnessWord = bdlmxxx::ElemType::BDEM_VOID == d_dataType
                          ? &s_voidNullnessWord
                          : d_isTopLevelAggregateNull_p;

        return bdlmxxx::ElemRef(d_value_p, descriptor, nullnessWord, 0);
                                                                      // RETURN
    }

    void *valuePtr = d_parentData_p;

    switch (d_parentType) {
      case bdlmxxx::ElemType::BDEM_LIST: {

        // Extract bdlmxxx::Row from bdlmxxx::Choice, then fall through to ROW case.

        valuePtr = &((bdlmxxx::List *) valuePtr)->row();            // FALL THROUGH
      case bdlmxxx::ElemType::BDEM_ROW:

        bdlmxxx::Row& row = *(bdlmxxx::Row *) valuePtr;
        return row[d_indexInParent];                                  // RETURN
      } break;
      case bdlmxxx::ElemType::BDEM_CHOICE:

        // Extract bdlmxxx::ChoiceArrayItem from bdlmxxx::Choice, then fall through
        // to the CHOICE_ARRAY_ITEM case.

        valuePtr = &((bdlmxxx::Choice *) valuePtr)->item();         // FALL THROUGH
      case bdlmxxx::ElemType::BDEM_CHOICE_ARRAY_ITEM: {
        bdlmxxx::ChoiceArrayItem& choiceItem = *(bdlmxxx::ChoiceArrayItem *)valuePtr;
        return choiceItem.selection();                                // RETURN
      } break;
      case bdlmxxx::ElemType::BDEM_TABLE: {
        bdlmxxx::Table& table = *(bdlmxxx::Table *) valuePtr;
        return table.rowElemRef(d_indexInParent);                     // RETURN
      } break;
      case bdlmxxx::ElemType::BDEM_CHOICE_ARRAY: {
        return ((bdlmxxx::ChoiceArray *)valuePtr)->itemElemRef(d_indexInParent);
                                                                      // RETURN
      } break;
      default: {
        BSLS_ASSERT(bdlmxxx::ElemType::isArrayType(d_parentType));

        const bdlmxxx::Descriptor *descriptor =
                                bdlmxxx::ElemAttrLookup::lookupTable()[d_dataType];
        return bdlmxxx::ElemRef(d_value_p, descriptor);               // RETURN
      }
    }
}

bsl::string AggregateRaw::asString() const
{
    if (bdlmxxx::ElemType::BDEM_VOID == d_dataType) {

        // Special case: return empty string for 'BDEM_VOID' type.

        return "";                                                    // RETURN
    }

    return convertScalar<bsl::string>();
}

template <>
bsl::string AggregateRaw::convertScalar<bsl::string>() const
{
    bsl::string result;
    int status = -1;
    const bdlmxxx::EnumerationDef *enumDef = enumerationConstraint();
    if (enumDef) {
        if (bdlmxxx::ElemType::BDEM_INT == d_dataType) {
            int enumId = *static_cast<int*>(d_value_p);
            if (bdltuxxx::Unset<int>::isUnset(enumId)) {
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
        else if (bdlmxxx::ElemType::BDEM_STRING == d_dataType) {
            result = *static_cast<bsl::string*>(d_value_p);
            status = 0;
        }
    }

    if (0 != status) {
        // If not an enumeration, or if enum-conversion failed, then do normal
        // conversion.

        status = bdlmxxx::Convert::fromBdemType(&result,
                                            d_value_p,
                                            d_dataType);
    }

    if (0 != status) {
        // Conversion failed.
        return "";                                                    // RETURN
    }

    return result;
}

int AggregateRaw::errorCode() const
{
    return isError()
         ? (int) static_cast<const ErrorAttributes *>(data())->code()
         : 0;
}

bsl::string AggregateRaw::errorMessage() const
{
    if (! isError()) {
        return "";                                                    // RETURN
    }

    return static_cast<const ErrorAttributes *>(data())->description();
}

int AggregateRaw::fieldById(AggregateRaw    *field,
                                 ErrorAttributes *errorDescription,
                                 int                   fieldId) const
{
    BSLS_ASSERT_SAFE(field);
    BSLS_ASSERT_SAFE(errorDescription);

    *field = *this;
    return field->descendIntoFieldById(errorDescription, fieldId);
}

int AggregateRaw::fieldByIndex(AggregateRaw    *field,
                                    ErrorAttributes *errorDescription,
                                    int                   index) const
{
    BSLS_ASSERT_SAFE(field);
    BSLS_ASSERT_SAFE(errorDescription);

    *field = *this;
    return field->descendIntoFieldByIndex(errorDescription, index);
}

int
AggregateRaw::findUnambiguousChoice(
                                        AggregateRaw    *choiceObject,
                                        ErrorAttributes *errorDescription,
                                        const char           *caller) const
{
    BSLS_ASSERT_SAFE(choiceObject);
    BSLS_ASSERT_SAFE(errorDescription);
    BSLS_ASSERT_SAFE(caller);

    *choiceObject = *this;
    if (bdlmxxx::ElemType::BDEM_CHOICE            == d_dataType
     || bdlmxxx::ElemType::BDEM_CHOICE_ARRAY_ITEM == d_dataType) {

        // This object is a choice (unambiguously)

        return 0;                                                     // RETURN
    }

    int rc;
    do {
        rc = choiceObject->anonymousField(choiceObject, errorDescription);

        // Loop until choice is found or error is encountered.  Since we do
        // not descend into arrays, it is not possible for
        // choiceObject->dataType() to be a 'CHOICE_ARRAY_ITEM'.

    } while (0 == rc &&
             bdlmxxx::ElemType::BDEM_CHOICE != choiceObject->dataType());

    if (!rc) {
        return 0;                                                     // RETURN
    }

    switch (errorDescription->code()) {
      case ErrorCode::BCEM_AMBIGUOUS_ANON: {
        bsl::ostringstream oss;
        oss << caller
            << " called for object with multiple anonymous fields.  "
            << "Cannot pick one.";
        errorDescription->setDescription(oss.str());
      } break;
      default: {
        bsl::ostringstream oss;
        oss << caller <<  " called on aggregate of type"
            << bdlmxxx::ElemType::toAscii(d_dataType);
        errorDescription->setDescription(oss.str());
        errorDescription->setCode(ErrorCode::BCEM_NOT_A_CHOICE);
      } break;
    }
    return -1;
}

int AggregateRaw::getField(AggregateRaw    *fieldPointer,
                                ErrorAttributes *errorDescription,
                                bool                  makeNonNullFlag,
                                FieldSelector    fieldSelector1,
                                FieldSelector    fieldSelector2,
                                FieldSelector    fieldSelector3,
                                FieldSelector    fieldSelector4,
                                FieldSelector    fieldSelector5,
                                FieldSelector    fieldSelector6,
                                FieldSelector    fieldSelector7,
                                FieldSelector    fieldSelector8,
                                FieldSelector    fieldSelector9,
                                FieldSelector    fieldSelector10) const
{
    BSLS_ASSERT_SAFE(fieldPointer);
    BSLS_ASSERT_SAFE(errorDescription);

    *fieldPointer = *this;

    int rc = 1;
    do {
        // this "loop" is just to enable exiting from the block of
        // statements easily once we find an "empty" argument

        if (fieldSelector1.isEmpty()) {
            break;
        }

        rc = fieldPointer->descendIntoField(errorDescription,
                                            fieldSelector1,
                                            makeNonNullFlag);

        if (rc || fieldSelector2.isEmpty()) {
            break;
        }

        rc = fieldPointer->descendIntoField(errorDescription,
                                            fieldSelector2,
                                            makeNonNullFlag);

        if (rc || fieldSelector3.isEmpty()) {
            break;
        }

        rc = fieldPointer->descendIntoField(errorDescription,
                                            fieldSelector3,
                                            makeNonNullFlag);

        if (rc || fieldSelector4.isEmpty()) {
            break;
        }

        rc = fieldPointer->descendIntoField(errorDescription,
                                            fieldSelector4,
                                            makeNonNullFlag);

        if (rc || fieldSelector5.isEmpty()) {
            break;
        }

        rc = fieldPointer->descendIntoField(errorDescription,
                                            fieldSelector5,
                                            makeNonNullFlag);

        if (rc || fieldSelector6.isEmpty()) {
            break;
        }

        rc = fieldPointer->descendIntoField(errorDescription,
                                            fieldSelector6,
                                            makeNonNullFlag);

        if (rc || fieldSelector7.isEmpty()) {
            break;
        }

        rc = fieldPointer->descendIntoField(errorDescription,
                                            fieldSelector7,
                                            makeNonNullFlag);

        if (rc || fieldSelector8.isEmpty()) {
            break;
        }

        rc = fieldPointer->descendIntoField(errorDescription,
                                            fieldSelector8,
                                            makeNonNullFlag);

        if (rc || fieldSelector9.isEmpty()) {
            break;
        }

        rc = fieldPointer->descendIntoField(errorDescription,
                                            fieldSelector9,
                                            makeNonNullFlag);

        if (rc || fieldSelector10.isEmpty()) {
            break;
        }

        rc = fieldPointer->descendIntoField(errorDescription,
                                            fieldSelector10,
                                            makeNonNullFlag);
    } while (false);

    return rc;
}

bool AggregateRaw::hasField(const char *fieldName) const
{
    if (bdlmxxx::ElemType::isAggregateType(d_dataType)
     && !bdlmxxx::ElemType::isArrayType(d_dataType)
     && d_recordDef_p) {

        // constrained list or choice aggregate

        return 0 <= d_recordDef_p->fieldIndexExtended(fieldName);     // RETURN
    }

    return false;
}

bool AggregateRaw::hasFieldById(int fieldId) const
{
    if (bdlmxxx::ElemType::isAggregateType(d_dataType)
     && !bdlmxxx::ElemType::isArrayType(d_dataType)
     && d_recordDef_p) {

        // constrained list or choice aggregate

        return 0 != d_recordDef_p->lookupField(fieldId);              // RETURN
    }

    return false;
}

bool AggregateRaw::hasFieldByIndex(int fieldIndex) const
{
    if (bdlmxxx::ElemType::isAggregateType(d_dataType)
     && !bdlmxxx::ElemType::isArrayType(d_dataType)
     && d_recordDef_p) {

        // constrained list or choice aggregate

        return (unsigned)fieldIndex < (unsigned)d_recordDef_p->numFields();
                                                                      // RETURN
    }

    return false;
}

bool AggregateRaw::isNull() const
{
    if (!isNullable()) {
        return false;                                                 // RETURN
    }

    bool isElemNull;
    if (bdlmxxx::ElemType::isArrayType(d_parentType)) {
        if (bdlmxxx::ElemType::BDEM_TABLE == d_parentType) {
            bdlmxxx::Table& table = *(bdlmxxx::Table *)d_parentData_p;
            isElemNull = table.isRowNull(d_indexInParent);
        }
        else {
            BSLS_ASSERT(bdlmxxx::ElemType::BDEM_CHOICE_ARRAY == d_parentType);

            bdlmxxx::ChoiceArray& choiceArray =
                                           *(bdlmxxx::ChoiceArray *)d_parentData_p;
            isElemNull = choiceArray.isItemNull(d_indexInParent);
        }
    }
    else {
        isElemNull = isVoid() || asElemRef().isNull();
    }

    return isElemNull;
}

bool AggregateRaw::isNullable() const
{
    return bdlmxxx::ElemType::isArrayType(d_parentType)
        && !bdlmxxx::ElemType::isAggregateType(d_parentType)
         ? false
         : true;
}

int AggregateRaw::length() const
{
    int result;

    switch (d_dataType) {
      case bdlmxxx::ElemType::BDEM_LIST: {
        result = ((bdlmxxx::List *)d_value_p)->length();
      } break;
      case bdlmxxx::ElemType::BDEM_ROW: {
        result = ((bdlmxxx::Row *)d_value_p)->length();
      } break;
      case bdlmxxx::ElemType::BDEM_TABLE: {
        result = ((bdlmxxx::Table *)d_value_p)->numRows();
      } break;
      case bdlmxxx::ElemType::BDEM_CHOICE_ARRAY: {
        result = ((bdlmxxx::ChoiceArray *)d_value_p)->length();
      } break;
      default: {
        AggregateRaw_ArraySizer sizer;
        result = AggregateRaw_Util::visitArray(d_value_p,
                                                    d_dataType,
                                                    &sizer);
      } break;
    }

    return result;
}

void AggregateRaw::loadAsString(bsl::string *result) const
{
    if (bdlmxxx::ElemType::BDEM_VOID == d_dataType) {

        // Special case: load empty string for 'BDEM_VOID' type.

        result->clear();
    }
    else {
        *result = convertScalar<bsl::string>();
    }
}

int AggregateRaw::numSelections() const
{
    ErrorAttributes error;
    AggregateRaw    choiceObj;

    if (0 != findUnambiguousChoice(&choiceObj, &error, "numSelections")) {
        return error.code();                                          // RETURN
    }

    int retValue;
    switch (choiceObj.d_dataType) {
      case bdlmxxx::ElemType::BDEM_CHOICE: {
        retValue = ((bdlmxxx::Choice *)choiceObj.d_value_p)->numSelections();
      } break;
      case bdlmxxx::ElemType::BDEM_CHOICE_ARRAY_ITEM: {
        retValue =
                ((bdlmxxx::ChoiceArrayItem *)choiceObj.d_value_p)->numSelections();
      } break;
      case bdlmxxx::ElemType::BDEM_CHOICE_ARRAY: {
        retValue = ((bdlmxxx::ChoiceArray *)choiceObj.d_value_p)->numSelections();
      } break;
      default: {
        retValue = ErrorCode::BCEM_NOT_A_CHOICE;
      } break;
    }
    return retValue;
}

const char *AggregateRaw::selector() const
{
    ErrorAttributes error;
    AggregateRaw    choiceObj;

    if (0 != findUnambiguousChoice(&choiceObj, &error, "selector")) {
        return "";                                                    // RETURN
    }

    int index = choiceObj.selectorIndex();
    return index < 0 ? "" : choiceObj.d_recordDef_p->fieldName(index);
}

int AggregateRaw::selectorId() const
{
    AggregateRaw    choiceObj;
    ErrorAttributes dummy;
    if (0 != findUnambiguousChoice(&choiceObj, &dummy, "selectorId")) {
        return bdlmxxx::RecordDef::BDEM_NULL_FIELD_ID;                // RETURN
    }

    int index = choiceObj.selectorIndex();
    return index < 0
         ? bdlmxxx::RecordDef::BDEM_NULL_FIELD_ID
         : choiceObj.d_recordDef_p->fieldId(index);
}

int AggregateRaw::selectorIndex() const
{
    int index;
    switch (d_dataType) {
      case bdlmxxx::ElemType::BDEM_CHOICE: {
        index = ((bdlmxxx::Choice *)d_value_p)->selector();
      } break;
      case bdlmxxx::ElemType::BDEM_CHOICE_ARRAY_ITEM: {
        index = ((bdlmxxx::ChoiceArrayItem *)d_value_p)->selector();
      } break;
      case bdlmxxx::ElemType::BDEM_LIST:                            // FALL THROUGH
      case bdlmxxx::ElemType::BDEM_ROW: {

        // For non-choice records, find the unambiguous anonymous choice
        // within the record and return the selection index for that, or
        // the error code if there is no unambiguous anonymous choice.

        ErrorAttributes errorDescription;
        AggregateRaw    choiceObject;
        if (0 == findUnambiguousChoice(&choiceObject,
                                       &errorDescription,
                                       "selectorIndex")) {
            index = choiceObject.selectorIndex();
        }
        else {
            index = errorDescription.code();
        }
      } break;
      default: {
        if (isError()) {
            index = errorCode();
        }
        else {
            index = ErrorCode::BCEM_NOT_A_CHOICE;
        }
      } break;
    }

    return index;
}

                                  // Aspects

bsl::ostream& AggregateRaw::print(bsl::ostream& stream,
                                       int           level,
                                       int           spacesPerLevel) const
{
    if (isError()) {
        bdlb::Print::indent(stream, level, spacesPerLevel);
        stream << "<ERR> "
               << static_cast<const ErrorAttributes *>(
                                                     d_value_p)->description();
        if (spacesPerLevel >= 0) {
            stream << bsl::endl;
        }
    }
    else if (bdlmxxx::ElemType::BDEM_VOID == d_dataType) {
        bdlb::Print::indent(stream, level, spacesPerLevel);
        stream << "<null aggregate>";
        if (spacesPerLevel >= 0) {
            stream << bsl::endl;
        }
    }
    else {
        bdlmxxx::SchemaAggregateUtil::print(stream,
                                        asElemRef(),
                                        d_recordDef_p,
                                        level,
                                        spacesPerLevel);
    }

    return stream;
}

// REFERENCED-VALUE MANIPULATORS
int AggregateRaw::insertItems(ErrorAttributes *errorDescription,
                                   int                   index,
                                   int                   numItems) const
{
    BSLS_ASSERT_SAFE(errorDescription);
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(0 <= numItems);

    if (! bdlmxxx::ElemType::isArrayType(d_dataType)) {
        bsl::string description =
                   "Attempt to insert items into non-array aggregate of type ";
        description += bdlmxxx::ElemType::toAscii(d_dataType);
        errorDescription->setDescription(description);
        errorDescription->setCode(ErrorCode::BCEM_NOT_AN_ARRAY);
        return -1;                                                    // RETURN
    }

    bool isAggNull = isNull();

    ErrorCode::Code status = ErrorCode::BCEM_SUCCESS;
    int arrayLen;
    switch (d_dataType) {
      case bdlmxxx::ElemType::BDEM_TABLE: {
          bdlmxxx::Table& theTable = *static_cast<bdlmxxx::Table *>(d_value_p);
        arrayLen = theTable.numRows();
        if (index > arrayLen) {
            status = ErrorCode::BCEM_BAD_ARRAYINDEX;
            break;
        }

        if (isAggNull) {
            makeValue();
        }

        theTable.insertNullRows(index, numItems);
        if (d_recordDef_p
         && (!d_fieldDef_p
          || bdlat_FormattingMode::e_NILLABLE !=
                                             d_fieldDef_p->formattingMode())) {
            for (int i = index; i < index + numItems; ++i) {
                bdlmxxx::SchemaAggregateUtil::initRowDeep(
                                                 &theTable.theModifiableRow(i),
                                                 *d_recordDef_p);
            }
        }
      } break;
      case bdlmxxx::ElemType::BDEM_CHOICE_ARRAY: {
        bdlmxxx::ChoiceArray& theChoiceArray =
                                   *static_cast<bdlmxxx::ChoiceArray *>(d_value_p);
        arrayLen = theChoiceArray.length();
        if (index > arrayLen) {
            status = ErrorCode::BCEM_BAD_ARRAYINDEX;
            break;
        }

        if (isAggNull) {
            makeValue();
        }

        theChoiceArray.insertNullItems(index, numItems);
      } break;
      default: {
        if (isAggNull) {
            makeValue();
        }

        if (numItems > 0) {
            ArrayInserter inserter(index, numItems, d_fieldDef_p);
            int rc = AggregateRaw_Util::visitArray(d_value_p,
                                                        d_dataType,
                                                        &inserter);
            if (rc) {
                if (isAggNull) {
                    makeNull();
                }
                status = ErrorCode::BCEM_BAD_ARRAYINDEX;
            }
            arrayLen = inserter.length();
        }
      } break;
    }

    if (ErrorCode::BCEM_SUCCESS != status) {
        bsl::ostringstream oss;
        oss << "Attempt to insert items at index " << index
            << " into " << bdlmxxx::ElemType::toAscii(d_dataType)
            << " of length " << arrayLen;
        errorDescription->setDescription(oss.str());
        errorDescription->setCode(status);
        return status;                                                // RETURN
    }

    return 0;
}

int AggregateRaw::insertNullItem(AggregateRaw    *newItem,
                                      ErrorAttributes *errorDescription,
                                      int                   index) const
{
    BSLS_ASSERT_SAFE(newItem);
    BSLS_ASSERT_SAFE(errorDescription);
    BSLS_ASSERT_SAFE(index >= 0);

    if (! bdlmxxx::ElemType::isArrayType(d_dataType)) {
        bsl::string description =
                         "Attempt to insert into non-array aggregate of type ";
        description += bdlmxxx::ElemType::toAscii(d_dataType);
        errorDescription->setDescription(description);
        errorDescription->setCode(ErrorCode::BCEM_NOT_AN_ARRAY);
        return -1;                                                    // RETURN
    }

    bool isAggNull = isNull();

    switch (d_dataType) {
      case bdlmxxx::ElemType::BDEM_TABLE: {
        bdlmxxx::Table& theTable = *(bdlmxxx::Table *)d_value_p;
        if (index > theTable.numRows()) {
            bsl::ostringstream oss;
            oss << "Attempt to insert at index " << index
                << " into TABLE of length " << theTable.numRows();
            errorDescription->setDescription(oss.str());
            errorDescription->setCode(ErrorCode::BCEM_BAD_ARRAYINDEX);
            return -1;                                                // RETURN
        }

        if (isAggNull) {
            makeValue();
        }

        theTable.insertNullRow(index);
      } break;
      case bdlmxxx::ElemType::BDEM_CHOICE_ARRAY: {
        bdlmxxx::ChoiceArray& theChoiceArray = *(bdlmxxx::ChoiceArray*)d_value_p;
        if (index > theChoiceArray.length()) {
            bsl::ostringstream oss;
            oss << "Attempt to insert at index " << index
                << " into CHOICE_ARRAY of length " << theChoiceArray.length();
            errorDescription->setDescription(oss.str());
            errorDescription->setCode(ErrorCode::BCEM_BAD_ARRAYINDEX);
            return -1;                                                // RETURN
        }

        if (isAggNull) {
            makeValue();
        }

        theChoiceArray.insertNullItems(index, 1);
      } break;
      default: {
        if (isAggNull) {
            makeValue();
        }

        ArrayInserter inserter(index, 1, d_fieldDef_p, true);
        int status = AggregateRaw_Util::visitArray(d_value_p,
                                                        d_dataType,
                                                        &inserter);
        if (status < 0) {
            bsl::ostringstream oss;
            oss << "Attempt to insert at index " << index
                << " into " << bdlmxxx::ElemType::toAscii(d_dataType)
                << " of length " << inserter.length();
            errorDescription->setDescription(oss.str());
            errorDescription->setCode(ErrorCode::BCEM_BAD_ARRAYINDEX);
            return -1;                                                // RETURN
        }
      }
    }

    return getField(newItem, errorDescription, false, index);
}

int AggregateRaw::insertNullItems(ErrorAttributes *errorDescription,
                                       int                   index,
                                       int                   numItems) const
{
    BSLS_ASSERT_SAFE(errorDescription);
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(0 <= numItems);

    if (! bdlmxxx::ElemType::isArrayType(d_dataType)) {
        bsl::string description =
                   "Attempt to insert items into non-array aggregate of type ";
        description += bdlmxxx::ElemType::toAscii(d_dataType);
        errorDescription->setDescription(description);
        errorDescription->setCode(ErrorCode::BCEM_NOT_AN_ARRAY);
        return -1;                                                    // RETURN
    }

    bool isAggNull = isNull();

    ErrorCode::Code status = ErrorCode::BCEM_SUCCESS;
    int arrayLen;
    switch (d_dataType) {
      case bdlmxxx::ElemType::BDEM_TABLE: {
        bdlmxxx::Table& theTable = *(bdlmxxx::Table *)d_value_p;
        arrayLen = theTable.numRows();
        if (index > arrayLen) {
            status = ErrorCode::BCEM_BAD_ARRAYINDEX;
            break;
        }

        if (isAggNull) {
            makeValue();
        }

        theTable.insertNullRows(index, numItems);
      } break;
      case bdlmxxx::ElemType::BDEM_CHOICE_ARRAY: {
        bdlmxxx::ChoiceArray& theChoiceArray = *(bdlmxxx::ChoiceArray*)d_value_p;
        arrayLen = theChoiceArray.length();
        if (index > arrayLen) {
            status = ErrorCode::BCEM_BAD_ARRAYINDEX;
            break;
        }

        if (isAggNull) {
            makeValue();
        }

        theChoiceArray.insertNullItems(index, numItems);
      } break;
      default: {
        if (isAggNull) {
            makeValue();
        }

        if (numItems > 0) {
            ArrayInserter inserter(index, numItems, d_fieldDef_p, true);
            int rc = AggregateRaw_Util::visitArray(d_value_p,
                                                        d_dataType,
                                                        &inserter);
            if (rc) {
                if (isAggNull) {
                    makeNull();
                }
                status = ErrorCode::BCEM_BAD_ARRAYINDEX;
            }
            arrayLen = inserter.length();
        }
      } break;
    }

    if (ErrorCode::BCEM_SUCCESS != status) {
        bsl::ostringstream oss;
        oss << "Attempt to insert items at index " << index
            << " into " << bdlmxxx::ElemType::toAscii(d_dataType)
            << " of length " << arrayLen;
        errorDescription->setDescription(oss.str());
        errorDescription->setCode(status);
        return status;                                                // RETURN
    }

    return 0;
}

void AggregateRaw::makeNull() const
{
    if (bdlmxxx::ElemType::BDEM_VOID != d_dataType) {
        if (bdlmxxx::ElemType::isArrayType(d_parentType)) {
            if (!bdlmxxx::ElemType::isAggregateType(d_parentType)) {
                const bdlmxxx::Descriptor *descriptor =
                                bdlmxxx::ElemAttrLookup::lookupTable()[d_dataType];
                descriptor->makeUnset(d_value_p);
            }
            else if (bdlmxxx::ElemType::BDEM_TABLE == d_parentType) {
                bdlmxxx::Table& table = *(bdlmxxx::Table *)d_parentData_p;
                table.makeRowsNull(d_indexInParent, 1);
            }
            else {
                BSLS_ASSERT(bdlmxxx::ElemType::BDEM_CHOICE_ARRAY == d_parentType);
                bdlmxxx::ChoiceArray& choiceArray =
                                           *(bdlmxxx::ChoiceArray *)d_parentData_p;
                choiceArray.makeItemsNull(d_indexInParent, 1);
            }
        }
        else {
            asElemRef().makeNull();
        }
    }
}

int
AggregateRaw::makeSelection(AggregateRaw    *selection,
                                 ErrorAttributes *errorDescription,
                                 const char           *newSelector) const
{
    BSLS_ASSERT_SAFE(selection);
    BSLS_ASSERT_SAFE(errorDescription);

    int                   newSelectorIndex = -1;
    bool                  foundChoice      = false;
    AggregateRaw     obj              = *this;
    const bdlmxxx::RecordDef *parentRecordDef  = 0;

    // Descend into unnamed choices and sequences until a leaf is found
    do {
        if (0 != obj.getFieldIndex(&newSelectorIndex,
                                   errorDescription,
                                   newSelector,
                                   "makeSelection")) {
            return -1;                                                // RETURN
        }

        // TBD: We don't reset nullness of top level aggregate if we descend
        // into an aggregate and we face an error making the selection.

        if (isNull()) {
            makeValue();
        }

        parentRecordDef = obj.d_recordDef_p;
        if (bdlmxxx::RecordDef::BDEM_CHOICE_RECORD ==
                                             obj.d_recordDef_p->recordType()) {
            // CHOICE or CHOICE_ARRAY_ITEM

            int rc = obj.makeSelectionByIndex(&obj,
                                              errorDescription,
                                              newSelectorIndex);
            if (!rc) {
                foundChoice = true;
            }
            else {
                return -1;                                            // RETURN
            }
        }
        else {
            // Even though object is not a choice, it might contain an
            // unnamed choice field or an unnamed sequence that indirectly
            // holds an unnamed choice.  Descend into field:

            if (0 != obj.descendIntoFieldByIndex(errorDescription,
                                                 newSelectorIndex)) {
                return -1;                                            // RETURN
            }
        }

        // Loop so long as we are looking at an unnamed constrained aggregate.
    } while (obj.d_recordDef_p
          && 0 == parentRecordDef->fieldName(newSelectorIndex));

    if (! foundChoice) {
        // We may have descended through zero or more unnamed aggregates, but
        // none of them were choice aggregates.  'makeSelection' is not
        // appropriate.

        bsl::string description =
                          "Called makeSelection on non-choice object of type ";
        description += bdlmxxx::ElemType::toAscii(d_dataType);
        errorDescription->setDescription(description);
        errorDescription->setCode(ErrorCode::BCEM_NOT_A_CHOICE);
        return -1;                                                    // RETURN
    }

    *selection = obj;

    return 0;
}

int AggregateRaw::makeSelectionById(
                                        AggregateRaw    *field,
                                        ErrorAttributes *errorDescription,
                                        int                   id) const
{
    BSLS_ASSERT_SAFE(field);
    BSLS_ASSERT_SAFE(errorDescription);

    int index = -1;
    if (getFieldIndex(&index,
                      errorDescription,
                      id,
                      "makeSelectionById")) {
        return -1;                                                    // RETURN
    }

    return makeSelectionByIndex(field, errorDescription, index);
}

int AggregateRaw::makeSelectionByIndex(
                                        AggregateRaw    *field,
                                        ErrorAttributes *errorDescription,
                                        int                   index) const
{
    BSLS_ASSERT_SAFE(field);
    BSLS_ASSERT_SAFE(errorDescription);
    BSLS_ASSERT_SAFE(-1 == index || 0 <= index);
    BSLS_ASSERT_SAFE(index < numSelections());

    bool isAggNull = isNull();
    if (isAggNull) {
        makeValue();
    }

    if (0 != makeSelectionByIndexRaw(field, errorDescription, index)) {
        if (isAggNull) {
            makeNull();
        }
        return -1;                                                    // RETURN
    }

    field->makeValue();

    return 0;
}

void AggregateRaw::makeValue() const
{
    if (bdlmxxx::ElemType::BDEM_VOID == d_dataType) {
        return;                                                       // RETURN
    }

    bdlmxxx::ElemRef elemRef = asElemRef();

    if (!elemRef.isNull()) {
        return;                                                       // RETURN
    }

    elemRef.data();  // clear nullness bit

    if (bdlmxxx::ElemType::isScalarType(d_dataType)) {
        if (d_fieldDef_p && d_fieldDef_p->hasDefaultValue()) {
            asElemRef().replaceValue(d_fieldDef_p->defaultValue());
        }
    }
    else if (d_recordDef_p) {
        switch (d_dataType) {
          case bdlmxxx::ElemType::BDEM_ROW: {
            bdlmxxx::SchemaAggregateUtil::initRowDeep((bdlmxxx::Row *) d_value_p,
                                                  *d_recordDef_p);
          } break;
          case bdlmxxx::ElemType::BDEM_LIST: {
            bdlmxxx::SchemaAggregateUtil::initListDeep((bdlmxxx::List *) d_value_p,
                                                   *d_recordDef_p);
          } break;
          case bdlmxxx::ElemType::BDEM_CHOICE: {
            bdlmxxx::SchemaAggregateUtil::initChoice((bdlmxxx::Choice *) d_value_p,
                                                 *d_recordDef_p);
          } break;
          case bdlmxxx::ElemType::BDEM_TABLE: {
            bdlmxxx::SchemaAggregateUtil::initTable((bdlmxxx::Table *) d_value_p,
                                                *d_recordDef_p);
          } break;
          case bdlmxxx::ElemType::BDEM_CHOICE_ARRAY: {
            bdlmxxx::SchemaAggregateUtil::initChoiceArray(
                                                (bdlmxxx::ChoiceArray *) d_value_p,
                                                *d_recordDef_p);
          } break;
          default: {
          } break;
        }
    }
}

int AggregateRaw::removeItems(ErrorAttributes *errorDescription,
                                   int                   index,
                                   int                   numItems) const
{
    BSLS_ASSERT_SAFE(errorDescription);
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(0 <= numItems);

    if (! bdlmxxx::ElemType::isArrayType(d_dataType)) {
        bsl::string description =
                   "Attempt to remove items from non-array aggregate of type ";
        description += bdlmxxx::ElemType::toAscii(d_dataType);
        errorDescription->setDescription(description);
        errorDescription->setCode(ErrorCode::BCEM_NOT_AN_ARRAY);
        return -1;                                                    // RETURN
    }
    else if (index + numItems > length()) {
        bsl::ostringstream oss;
        oss << "Attempt to remove at index " << index
            << " in " << bdlmxxx::ElemType::toAscii(d_dataType)
            << " of length " << length();
        errorDescription->setDescription(oss.str());
        errorDescription->setCode(ErrorCode::BCEM_BAD_ARRAYINDEX);
        return -1;                                                    // RETURN
    }

    switch (d_dataType) {
      case bdlmxxx::ElemType::BDEM_TABLE: {
        bdlmxxx::Table& theTable = *(bdlmxxx::Table *)d_value_p;
        theTable.removeRows(index, numItems);
      } break;
      case bdlmxxx::ElemType::BDEM_CHOICE_ARRAY: {
        bdlmxxx::ChoiceArray& theChoiceArray = *(bdlmxxx::ChoiceArray *)d_value_p;
        theChoiceArray.removeItems(index, numItems);
      } break;
      default: {
        ArrayItemEraser itemEraser(index, numItems);
        AggregateRaw_Util::visitArray(d_value_p,
                                           d_dataType,
                                           &itemEraser);
      } break;
    }

    return 0;
}

int AggregateRaw::reserveRaw(ErrorAttributes *errorDescription,
                                  bsl::size_t           numItems) const
{
    BSLS_ASSERT_SAFE(errorDescription);

    if (!bdlmxxx::ElemType::isArrayType(d_dataType)) {
        bsl::string description = bdlmxxx::ElemType::toAscii(d_dataType);
        description += " is not an array type";
        errorDescription->setDescription(description);
        errorDescription->setCode(ErrorCode::BCEM_NOT_AN_ARRAY);
        return -1;                                                    // RETURN
    }

    void *valuePtr = d_value_p;
    switch (d_dataType) {
      case bdlmxxx::ElemType::BDEM_TABLE: {
        bdlmxxx::Table& table = *(bdlmxxx::Table *)valuePtr;
        table.reserveRaw(numItems);
      } break;
      case bdlmxxx::ElemType::BDEM_CHOICE_ARRAY: {
        bdlmxxx::ChoiceArray& array = *(bdlmxxx::ChoiceArray *)valuePtr;
        array.reserveRaw(numItems);
      } break;
      default: {
        ArrayReserver reserver(numItems);

        // ArrayReserver always returns 0.

        AggregateRaw_Util::visitArray(valuePtr,
                                           d_dataType,
                                           &reserver);
      } break;
    }
    return 0;
}

int AggregateRaw::resize(ErrorAttributes *errorDescription,
                              bsl::size_t           newSize) const
{
    BSLS_ASSERT_SAFE(errorDescription);

    if (! bdlmxxx::ElemType::isArrayType(d_dataType)) {
        bsl::string description =
                              "Attempt to resize non-array aggregate of type ";
        description += bdlmxxx::ElemType::toAscii(d_dataType);
        errorDescription->setDescription(description);
        errorDescription->setCode(ErrorCode::BCEM_NOT_AN_ARRAY);
        return -1;                                                    // RETURN
    }

    bsl::size_t currentSize = length();
    if (newSize > currentSize) {
        return insertItems(errorDescription,
                           currentSize,
                           newSize - currentSize);                    // RETURN
    }
    else if (newSize < currentSize) {
        return removeItems(errorDescription,
                           newSize,
                           currentSize - newSize);                    // RETURN
    }

    return 0;
}

// REFERENCED-VALUE ACCESSORS
int AggregateRaw::capacityRaw(ErrorAttributes *errorDescription,
                                   bsl::size_t          *capacity) const
{
    BSLS_ASSERT_SAFE(errorDescription);
    BSLS_ASSERT_SAFE(capacity);

    if (!bdlmxxx::ElemType::isArrayType(d_dataType)) {
        bsl::string description = bdlmxxx::ElemType::toAscii(d_dataType);
        description += " is not an array type";
        errorDescription->setDescription(description);
        errorDescription->setCode(ErrorCode::BCEM_NOT_AN_ARRAY);
        return -1;                                                    // RETURN
    }

    void *valuePtr = d_value_p;
    switch (d_dataType) {
      case bdlmxxx::ElemType::BDEM_TABLE: {
        bdlmxxx::Table& table = *(bdlmxxx::Table *)valuePtr;
        *capacity = table.capacityRaw();
      } break;
      case bdlmxxx::ElemType::BDEM_CHOICE_ARRAY: {
        bdlmxxx::ChoiceArray& array = *(bdlmxxx::ChoiceArray *)valuePtr;
        *capacity = array.capacityRaw();
      } break;
      default: {
        ArrayCapacitor capacitor(capacity);
        AggregateRaw_Util::visitArray(valuePtr,
                                           d_dataType,
                                           &capacitor);
      }
    }
    return 0;
}

                  // -------------------------------------
                  // local struct bcem_Aggregate_BdeatInfo
                  // -------------------------------------
// CREATORS
AggregateRaw_BdeatInfo::AggregateRaw_BdeatInfo(
                                              const bdlmxxx::RecordDef *record,
                                              int                   fieldIndex)
: d_record_p(record)
, d_fieldIndex(fieldIndex)
, d_id(record->fieldId(fieldIndex))
, d_formattingMode(record->field(fieldIndex).formattingMode())
, d_areLazyAttributesSet(false)
, d_isNullable(record->field(fieldIndex).isNullable())
, d_hasDefaultValue(record->field(fieldIndex).hasDefaultValue())
{
    BSLS_ASSERT((unsigned)fieldIndex < (unsigned)record->numFields());
}

// PRIVATE ACCESSORS
void AggregateRaw_BdeatInfo::setLazyAttributes() const
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

                 // ----------------------------------
                 // struct AggregateRaw_BdeatUtil
                 // ----------------------------------

// CLASS METHODS
int AggregateRaw_BdeatUtil::fieldIndexFromName(
                                             const bdlmxxx::RecordDef&  record,
                                             const char            *name,
                                             int                    nameLength)
{
    BSLS_ASSERT_SAFE(name);
    BSLS_ASSERT_SAFE(0 <= nameLength);

    NullTerminatedString fieldName(name, nameLength);

    int fieldIndex = record.fieldIndexExtended(fieldName);

    if (fieldIndex < 0) {
        // Name does not match any field name.  If it matches the pattern
        // "FIELD_n", then return n (where n is a decimal number).

        if (nameLength <= 6 || 0 != bsl::strncmp(fieldName, "FIELD_", 6)) {
            return -1;                                                // RETURN
        }

        char *endPos = 0;
        fieldIndex = (int)bsl::strtol(fieldName + 6, &endPos, 10);
        if (*endPos || (unsigned)fieldIndex >= (unsigned)record.numFields()) {
            return -1;                                                // RETURN
        }
    }

    return fieldIndex;
}

                        // -----------------------------
                        // struct AggregateRaw_Util
                        // -----------------------------

// CLASS METHODS
const char *AggregateRaw_Util::enumerationName(
                                            const bdlmxxx::EnumerationDef *enumDef)
{
    if (! enumDef) {
        return "(unconstrained)";                                     // RETURN
    }

    const char *ret =
                enumDef->schema().enumerationName(enumDef->enumerationIndex());

    return ret ? ret : "(anonymous)";
}

const char *AggregateRaw_Util::recordName(const bdlmxxx::RecordDef *recordDef)
{
    if (! recordDef) {
        return "(unconstrained)";                                     // RETURN
    }

    const char *ret = recordDef->schema().recordName(recordDef->recordIndex());
    return ret ? ret : "(anonymous)";
}

// The following 'isConformant' methods are not 'inline' to avoid a
// header dependency on 'bdlmxxx::SchemaAggregateUtil'.

bool AggregateRaw_Util::isConformant(const bdlmxxx::ConstElemRef *object,
                                          const bdlmxxx::RecordDef    *recordDef)
{
    BSLS_ASSERT_SAFE(object);

    bool result = AggregateRaw_Util::isConformant(object->data(),
                                                       object->type(),
                                                       recordDef);

    if (!result && object->isNull()) {
        // Conformance fails for aggregate types.

        bdlmxxx::ElemType::Type type = object->type();

        if (bdlmxxx::RecordDef::BDEM_CHOICE_RECORD == recordDef->recordType()) {
            return bdlmxxx::ElemType::isChoiceType(type);             // RETURN
        }

        return bdlmxxx::ElemType::BDEM_ROW   == type
            || bdlmxxx::ElemType::BDEM_LIST  == type
            || bdlmxxx::ElemType::BDEM_TABLE == type;                 // RETURN
    }

    return result;
}

bool AggregateRaw_Util::isConformant(const bdlmxxx::Row       *object,
                                          const bdlmxxx::RecordDef *recordDef)
{
    BSLS_ASSERT_SAFE(object);

    return recordDef
         ? bdlmxxx::SchemaAggregateUtil::isRowConformant(*object, *recordDef)
         : true;
}

bool AggregateRaw_Util::isConformant(const bdlmxxx::List      *object,
                                          const bdlmxxx::RecordDef *recordDef)
{
    BSLS_ASSERT_SAFE(object);

    return recordDef
         ? bdlmxxx::SchemaAggregateUtil::isListConformant(*object, *recordDef)
         : true;
}

bool AggregateRaw_Util::isConformant(const bdlmxxx::Table     *object,
                                          const bdlmxxx::RecordDef *recordDef)
{
    BSLS_ASSERT_SAFE(object);

    return recordDef
         ? bdlmxxx::SchemaAggregateUtil::isTableConformant(*object, *recordDef)
         : true;
}

bool AggregateRaw_Util::isConformant(const bdlmxxx::Choice    *object,
                                          const bdlmxxx::RecordDef *recordDef)
{
    BSLS_ASSERT_SAFE(object);

    return recordDef
         ? bdlmxxx::SchemaAggregateUtil::isChoiceConformant(*object, *recordDef)
         : true;
}

bool AggregateRaw_Util::isConformant(
                                         const bdlmxxx::ChoiceArrayItem *object,
                                         const bdlmxxx::RecordDef       *recordDef)
{
    BSLS_ASSERT_SAFE(object);

    return recordDef
         ? bdlmxxx::SchemaAggregateUtil::isChoiceArrayItemConformant(*object,
                                                                 *recordDef)
         : true;
}

bool AggregateRaw_Util::isConformant(const bdlmxxx::ChoiceArray *object,
                                          const bdlmxxx::RecordDef   *recordDef)
{
    BSLS_ASSERT_SAFE(object);

    return recordDef
         ? bdlmxxx::SchemaAggregateUtil::isChoiceArrayConformant(*object,
                                                             *recordDef)
         : true;
}

bool AggregateRaw_Util::isConformant(const void           *object,
                                          bdlmxxx::ElemType::Type   type,
                                          const bdlmxxx::RecordDef *recordDef)
{
    bool result;

    if (recordDef) {
        switch (type) {
          case bdlmxxx::ElemType::BDEM_LIST: {
            result = AggregateRaw_Util::isConformant(
                    (const bdlmxxx::List*)object,
                    recordDef);
          } break;
          case bdlmxxx::ElemType::BDEM_ROW: {
            result = AggregateRaw_Util::isConformant(
                    (const bdlmxxx::Row*)object,
                    recordDef);
          } break;
          case bdlmxxx::ElemType::BDEM_TABLE: {
            result = AggregateRaw_Util::isConformant(
                    (const bdlmxxx::Table*)object,
                    recordDef);
          } break;
          case bdlmxxx::ElemType::BDEM_CHOICE: {
            result = AggregateRaw_Util::isConformant(
                    (const bdlmxxx::Choice*)object,
                    recordDef);
          } break;
          case bdlmxxx::ElemType::BDEM_CHOICE_ARRAY_ITEM: {
            result = AggregateRaw_Util::isConformant(
                    (const bdlmxxx::ChoiceArrayItem*)object,
                    recordDef);
          } break;
          case bdlmxxx::ElemType::BDEM_CHOICE_ARRAY: {
            result = AggregateRaw_Util::isConformant(
                    (const bdlmxxx::ChoiceArray*)object,
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

// ============================================================================
//                         'bdlat_typename' overloads
// ============================================================================

const char *bdlat_TypeName_className(const AggregateRaw& object)
{
    const char           *name = 0;
    const bdlmxxx::RecordDef *recordDef = object.recordConstraint();
    if (recordDef) {
        name = recordDef->recordName();
    }

    const bdlmxxx::EnumerationDef *enumerationDef = object.enumerationConstraint();
    if (enumerationDef) {
        name = enumerationDef->enumerationName();
    }

    return name;
}

// ============================================================================
//                       'bdlat_valuetype' overloads
// ============================================================================

void bdlat_valueTypeReset(AggregateRaw *object)
{
    BSLS_ASSERT_SAFE(object);

    // Do not use 'object->reset()' as that would set 'object' to VOID type.
    // Instead, reset the current object to its default state.

    object->makeNull();
    if (!object->fieldDef() || !object->fieldDef()->isNullable()) {
        object->makeValue();
    }
}

// ============================================================================
//                      'bdlat_choicefunctions' overloads
// ============================================================================


bool bdlat_choiceHasSelection(const AggregateRaw&  object,
                              const char               *selectionName,
                              int                       selectionNameLength)
{
    NullTerminatedString name(selectionName, selectionNameLength);

    return object.hasField(name);
}

int bdlat_choiceMakeSelection(AggregateRaw *object,
                              int                selectionId)
{
    AggregateRaw    dummyField;
    ErrorAttributes dummyError;
    if (bdlat_ChoiceFunctions::k_UNDEFINED_SELECTION_ID == selectionId) {
        object->makeSelectionById(&dummyField,
                                  &dummyError,
                                  bdlmxxx::RecordDef::BDEM_NULL_FIELD_ID);
        return 0;                                                     // RETURN
    }

    if (! object->hasFieldById(selectionId)) {
        return -1;                                                    // RETURN
    }

    return object->makeSelectionById(&dummyField, &dummyError, selectionId);
}

int bdlat_choiceMakeSelection(AggregateRaw *object,
                              const char        *selectionName,
                              int                selectionNameLength)
{
    NullTerminatedString name(selectionName, selectionNameLength);

    if (0 == object->hasField(name)) {
        return -1;                                                    // RETURN
    }

    AggregateRaw    field;
    ErrorAttributes dummy;
    return object->makeSelection(&field, &dummy, name);
}


// ============================================================================
//                     'bdlat_enumfunctions' overloads
// ============================================================================
int bdlat_enumFromInt(AggregateRaw *result, int enumId)
{
    const bdlmxxx::EnumerationDef *enumDef = result->enumerationConstraint();
    if (! enumDef) {
        return -1;                                                    // RETURN
    }

    const char *enumName = enumDef->lookupName(enumId);
    if (! enumName) {
        return -1;                                                    // RETURN
    }

    if (bdlmxxx::ElemType::BDEM_STRING == result->dataType()) {
        result->asElemRef().theModifiableString() = enumName;
                                                           // set string value
    }
    else {
        BSLS_ASSERT(bdlmxxx::ElemType::BDEM_INT == result->dataType());
        result->asElemRef().theModifiableInt() = enumId;   // set integer value
    }

    return 0;
}

int bdlat_enumFromString(AggregateRaw *result,
                         const char        *string,
                         int                stringLength)
{
    const bdlmxxx::EnumerationDef *enumDef = result->enumerationConstraint();
    if (! enumDef) {
        return -1;                                                    // RETURN
    }

    const bsl::string enumName(string, stringLength);
    const int enumId = enumDef->lookupId(enumName.c_str());
    if (bdltuxxx::Unset<int>::unsetValue() == enumId) {
        return -1;                                                    // RETURN
    }

    if (bdlmxxx::ElemType::BDEM_STRING == result->dataType()) {
        result->asElemRef().theModifiableString() = enumName;
                                                            // set string value
    }
    else {
        BSLS_ASSERT(bdlmxxx::ElemType::BDEM_INT == result->dataType());
        result->asElemRef().theModifiableInt() = enumId;   // set integer value
    }

    return 0;
}

// ============================================================================
//                     'bdlat_typecategory' overloads
// ============================================================================

bdlat_TypeCategory::Value
bdlat_typeCategorySelect(const AggregateRaw& object)
{
    bdlmxxx::ElemType::Type dataType = object.dataType();

    bdlat_TypeCategory::Value result;

    if (bdlmxxx::ElemType::isArrayType(dataType)) {
        result = bdlat_TypeCategory::e_ARRAY_CATEGORY;
    }
    else if (object.enumerationConstraint()) {
        result = bdlat_TypeCategory::e_ENUMERATION_CATEGORY;
    }
    else if (bdlmxxx::ElemType::isScalarType(dataType)) {
        result = bdlat_TypeCategory::e_SIMPLE_CATEGORY;
    }
    else {
        switch (dataType) {
          case bdlmxxx::ElemType::BDEM_LIST:                        // FALL THROUGH
          case bdlmxxx::ElemType::BDEM_ROW: {
            result = bdlat_TypeCategory::e_SEQUENCE_CATEGORY;
          } break;
          case bdlmxxx::ElemType::BDEM_CHOICE:
          case bdlmxxx::ElemType::BDEM_CHOICE_ARRAY_ITEM: {
            result = bdlat_TypeCategory::e_CHOICE_CATEGORY;
          } break;
          case bdlmxxx::ElemType::BDEM_VOID: {
            // Treat void as a simple type (an error will occur later, where
            // it can be more easily detected).

            result = bdlat_TypeCategory::e_SIMPLE_CATEGORY;
          } break;
          default: {
            BSLS_ASSERT_OPT("Category error" && 0);
            result = bdlat_TypeCategory::e_SIMPLE_CATEGORY;
          } break;
        }
    }

    return result;
}
}  // close package namespace

}  // close enterprise namespace

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2011
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ----------------------------------
