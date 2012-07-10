// bcem_aggregateraw.cpp                                              -*-C++-*-
#include <bcem_aggregateraw.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bcem_aggregateraw_cpp,"$Id$ $CSID$")

#include <bdem_convert.h>
#include <bdem_list.h>
#include <bdem_schemaaggregateutil.h>
#include <bdem_schemautil.h>

#include <stdio.h>   // for 'snprintf'
#if defined(BSLS_PLATFORM__CMP_MSVC)
#   define snprintf _snprintf // MSVC names snprintf _snprintf
#endif

namespace BloombergLP {

namespace {

static int s_voidNullnessWord = 1;

// TBD TBD TBD TBD TBD
// replace this custom type with bsl::make_signed<T> when that's available.

template <typename TYPE>
struct make_signed {
    typedef TYPE type;
};

template <>
struct make_signed<unsigned char> {
    typedef char type;
};

template <>
struct make_signed<unsigned short> {
    typedef short type;
};

template <>
struct make_signed<unsigned int> {
    typedef int type;
};

template <>
struct make_signed<bsls_Types::Uint64> {
    typedef bsls_Types::Int64 type;
};
// TBD TBD TBD TBD
// TBD replace the code above with bsl::make_signed when available.

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
                   // class ArrayInserter
                   // ===================

class ArrayInserter {
    // This class defines a function object to insert one or more elements
    // into a sequence container.  The values are either the default value for
    // the contained type of the sequence container, or copies of a single
    // value specified by a 'bdem_FieldDef' object at construction.

    // DATA
    int                  d_index;          // array index at which to insert

    int                  d_numElements;    // number of elements to insert

    const bdem_FieldDef *d_fieldDef_p;     // if non-null, provides value to
                                           // be inserted

    int                  d_length;         // pre-insertion length of the
                                           // associated array

    void                *d_data_p;         // address of first element inserted

    bool                 d_areValuesNull;  // are the values being inserted
                                           // null

    // PRIVATE TYPES
    template <typename TYPE>
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
                  const bdem_FieldDef *fieldDef,
                  bool                 areValuesNull = false);
        // Create an inserter for inserting the specified 'numElements' at the
        // specified 'index' in an array that is subsequently supplied to the
        // 'operator()' method.  If the specified 'fieldDef' is non-null, then
        // 'fieldDef' provides the value for the elements to be inserted;
        // otherwise the default value for the array element type is used.  If
        // 'index < 0', elements are appended to the end of the array.

    // MANIPULATORS
    template <typename ARRAYTYPE>
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
struct ArrayInserter::SignChecker<bsls_Types::Uint64> {
    // TBD REMOVE
    enum {
        IS_SIGNED = 0
    };
};

                 // -------------------
                 // class ArrayInserter
                 // -------------------

// CREATORS
inline
ArrayInserter::ArrayInserter(int                  index,
                             int                  numElements,
                             const bdem_FieldDef *fieldDef,
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
template <typename ARRAYTYPE>
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

        value_type nullValue(bdetu_Unset<value_type>::unsetValue());

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
    template <typename ARRAYTYPE>
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
    template <typename ARRAYTYPE>
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
    operator const char*() const
    {
        return d_string_p;
    }
};

} // close unnamed namespace

                  // -------------------------------------
                  // local struct bcem_Aggregate_BdeatInfo
                  // -------------------------------------
// CREATORS
bcem_AggregateRaw_BdeatInfo::bcem_AggregateRaw_BdeatInfo(
                                              const bdem_RecordDef *record,
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
void bcem_AggregateRaw_BdeatInfo::setLazyAttributes() const
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
                 // struct bcem_AggregateRaw_BdeatUtil
                 // ----------------------------------

// CLASS METHODS
int bcem_AggregateRaw_BdeatUtil::manipulateField(
                     bcem_AggregateRaw *parent,
                     bdef_Function<int(*)(bcem_AggregateRaw *,
                                          const bcem_AggregateRaw_BdeatInfo&)>&
                                        manipulator,
                     int                fieldIndex)
{
    BSLS_ASSERT_SAFE(parent);
    BSLS_ASSERT_SAFE(0 <= fieldIndex);

    if (! parent->recordConstraint()) {
        return -1;                                                    // RETURN
    }

    bcem_AggregateRaw   field;
    bcem_AggregateError dummy;
    if (0 != parent->fieldByIndex(&field, &dummy, fieldIndex)) {
        return -1;                                                    // RETURN
    }

    bcem_AggregateRaw_BdeatInfo info(parent->recordConstraint(), fieldIndex);

    return manipulator(&field, info);
}

int bcem_AggregateRaw_BdeatUtil::fieldIndexFromName(
                                             const bdem_RecordDef&  record,
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

                        // ----------------------------
                        // struct bcem_AggregateRawUtil
                        // ----------------------------

// CLASS METHODS
const char *bcem_AggregateRawUtil::enumerationName(
                                            const bdem_EnumerationDef *enumDef)
{
    if (! enumDef) {
        return "(unconstrained)";                                     // RETURN
    }

    const char *ret =
                enumDef->schema().enumerationName(enumDef->enumerationIndex());

    return ret ? ret : "(anonymous)";
}

const char *bcem_AggregateRawUtil::recordName(const bdem_RecordDef *recordDef)
{
    if (! recordDef) {
        return "(unconstrained)";                                     // RETURN
    }

    const char *ret = recordDef->schema().recordName(recordDef->recordIndex());
    return ret ? ret : "(anonymous)";
}

// The following 'isConformant' methods are not 'inline' to avoid a
// header dependency on 'bdem_SchemaAggregateUtil'.

bool bcem_AggregateRawUtil::isConformant(const bdem_ConstElemRef *object,
                                         const bdem_RecordDef    *recordDef)
{
    BSLS_ASSERT_SAFE(object);

    bool result = bcem_AggregateRawUtil::isConformant(object->data(),
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

bool bcem_AggregateRawUtil::isConformant(const bdem_Row       *object,
                                         const bdem_RecordDef *recordDef)
{
    BSLS_ASSERT_SAFE(object);

    return recordDef
         ? bdem_SchemaAggregateUtil::isRowConformant(*object, *recordDef)
         : true;
}

bool bcem_AggregateRawUtil::isConformant(const bdem_List      *object,
                                         const bdem_RecordDef *recordDef)
{
    BSLS_ASSERT_SAFE(object);

    return recordDef
         ? bdem_SchemaAggregateUtil::isListConformant(*object, *recordDef)
         : true;
}

bool bcem_AggregateRawUtil::isConformant(const bdem_Table     *object,
                                         const bdem_RecordDef *recordDef)
{
    BSLS_ASSERT_SAFE(object);

    return recordDef
         ? bdem_SchemaAggregateUtil::isTableConformant(*object, *recordDef)
         : true;
}

bool bcem_AggregateRawUtil::isConformant(const bdem_Choice    *object,
                                         const bdem_RecordDef *recordDef)
{
    BSLS_ASSERT_SAFE(object);

    return recordDef
         ? bdem_SchemaAggregateUtil::isChoiceConformant(*object, *recordDef)
         : true;
}

bool bcem_AggregateRawUtil::isConformant(const bdem_ChoiceArrayItem *object,
                                         const bdem_RecordDef       *recordDef)
{
    BSLS_ASSERT_SAFE(object);

    return recordDef
         ? bdem_SchemaAggregateUtil::isChoiceArrayItemConformant(*object,
                                                                 *recordDef)
         : true;
}

bool bcem_AggregateRawUtil::isConformant(const bdem_ChoiceArray *object,
                                         const bdem_RecordDef   *recordDef)
{
    BSLS_ASSERT_SAFE(object);

    return recordDef
         ? bdem_SchemaAggregateUtil::isChoiceArrayConformant(*object,
                                                             *recordDef)
         : true;
}

bool bcem_AggregateRawUtil::isConformant(const void           *object,
                                         bdem_ElemType::Type   type,
                                         const bdem_RecordDef *recordDef)
{
    bool result;

    if (recordDef) {
        switch (type) {
          case bdem_ElemType::BDEM_LIST: {
            result = bcem_AggregateRawUtil::isConformant((bdem_List*)object,
                                                         recordDef);
          } break;
          case bdem_ElemType::BDEM_ROW: {
            result = bcem_AggregateRawUtil::isConformant((bdem_Row*)object,
                                                         recordDef);
          } break;
          case bdem_ElemType::BDEM_TABLE: {
            result = bcem_AggregateRawUtil::isConformant((bdem_Table*)object,
                                                         recordDef);
          } break;
          case bdem_ElemType::BDEM_CHOICE: {
            result = bcem_AggregateRawUtil::isConformant((bdem_Choice*)object,
                                                         recordDef);
          } break;
          case bdem_ElemType::BDEM_CHOICE_ARRAY_ITEM: {
            result = bcem_AggregateRawUtil::isConformant(
                                                 (bdem_ChoiceArrayItem*)object,
                                                 recordDef);
          } break;
          case bdem_ElemType::BDEM_CHOICE_ARRAY: {
            result = bcem_AggregateRawUtil::isConformant(
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

#ifdef BDE_BUILD_TARGET_SAFE
bcem_AggregateRaw::~bcem_AggregateRaw()
{
    // Assert invariants (see member variable description in class definition)
    if (d_dataType != bdem_ElemType::BDEM_VOID) {
        BSLS_ASSERT(d_schema_p || (!d_recordDef && !d_fieldDef));

        BSLS_ASSERT(!d_schema_p || (d_recordDef || d_fieldDef));

        BSLS_ASSERT(! d_recordDef || &d_recordDef->schema() == d_schema_p);

        // Cannot easily test that 'd_fieldDef' is within 'd_schema'
        BSLS_ASSERT(! d_fieldDef
                    || d_fieldDef->elemType() == d_dataType
                    || d_fieldDef->elemType() ==
                            bdem_ElemType::toArrayType(d_dataType));
        BSLS_ASSERT(! d_fieldDef
                    || d_recordDef  == d_fieldDef->recordConstraint());
    }
}
#endif

int bcem_AggregateRaw::toEnum(bcem_AggregateError *errorDescription,
                              const char          *value,
                              bslmf_MetaInt<1>) const
{
    BSLS_ASSERT_SAFE(errorDescription);

    const bdem_EnumerationDef *enumDef = enumerationConstraint();
    const int enumId = value
                     ? enumDef->lookupId(value)
                     : bdetu_Unset<int>::unsetValue();

    if (bdetu_Unset<int>::isUnset(enumId) && 0 != value && 0 != value[0]) {
        bsl::ostringstream oss;
        oss << "Attempt to set enumerator name \"" << value
            << "\" in enumeration \""
            << bcem_AggregateRawUtil::enumerationName(enumDef) << '\"';
        errorDescription->description() = oss.str();
        errorDescription->code() = bcem_AggregateError::BCEM_ERR_BAD_ENUMVALUE;
        return -1;                                                    // RETURN
    }

    // If we got here, we're either a (1) top-level aggregate, (2) CHOICE or
    // CHOICE_ARRAY_ITEM that has been selected (hence, non-null), or (3) an
    // item in a ROW.

    if (bdem_ElemType::BDEM_INT == dataType()) {
        asElemRef().theModifiableInt() = enumId;
    }
    else {
        asElemRef().theModifiableString() = value ? value : "";
    }

    return 0;
}

int bcem_AggregateRaw::toEnum(bcem_AggregateError      *errorDescription,
                              const bdem_ConstElemRef&  value,
                              bslmf_MetaInt<1>) const
{
    BSLS_ASSERT_SAFE(errorDescription);

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
          if (value.isNull()) {
              makeNull();
              return 0;                                               // RETURN
          }
          return toEnum(errorDescription, value.theString().c_str(),
                        bslmf_MetaInt<1>());                          // RETURN
      } break;
      default: {
        bsl::ostringstream oss;
        oss << "Invalid conversion from \""
            << bdem_ElemType::toAscii(value.type())
            << "\" to enumeration \""
            << bcem_AggregateRawUtil::enumerationName(enumerationConstraint())
            << '\"';
        errorDescription->description() = oss.str();
        errorDescription->code() =
                                  bcem_AggregateError::BCEM_ERR_BAD_CONVERSION;
        return -1;                                                    // RETURN
      }
    }

    if (value.isNull()) {
        makeNull();
        return 0;                                                     // RETURN
    }

    // Got here if value is numeric and has been converted to int.

    return toEnum(errorDescription, intValue, bslmf_MetaInt<0>());
}

int bcem_AggregateRaw::toEnum(bcem_AggregateError *errorDescription,
                              const int&           value,
                              bslmf_MetaInt<0>) const
{
    BSLS_ASSERT_SAFE(errorDescription);

    const bdem_EnumerationDef *enumDef  = enumerationConstraint();
    const char                *enumName = enumDef->lookupName(value);

    if (bdetu_Unset<int>::unsetValue() != value && !enumName) {

        // Failed lookup

        bsl::ostringstream oss;
        oss << "Attempt to set enumerator ID " << value
            << " in enumeration \""
            << bcem_AggregateRawUtil::enumerationName(enumDef);
        errorDescription->description().assign(oss.str());
        errorDescription->code() = bcem_AggregateError::BCEM_ERR_BAD_ENUMVALUE;
        return -1;                                                    // RETURN
    }

    // If we got here, we're either a (1) top-level aggregate, (2) CHOICE or
    // CHOICE_ARRAY_ITEM that has been selected (hence, non-null), or (3) an
    // item in a ROW.

    if (bdem_ElemType::BDEM_INT == dataType()) {
        asElemRef().theModifiableInt() = value;
    }
    else {
        asElemRef().theModifiableString() = enumName ? enumName : "";
    }

    return 0;
}

template <typename TOTYPE>
TOTYPE bcem_AggregateRaw::convertScalar() const
{
    TOTYPE result;
    int    status = -1;
    const bdem_EnumerationDef *enumDef = enumerationConstraint();
    if (enumDef) {
        int enumId;
        if (bdem_ElemType::BDEM_INT == d_dataType) {
            enumId = *static_cast<int*>(d_value_p);
            status = 0;
        }
        else if (bdem_ElemType::BDEM_STRING == d_dataType) {
            const bsl::string& enumName =
                                         *static_cast<bsl::string*>(d_value_p);
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
                                            d_value_p,
                                            d_dataType);
    }

    if (0 != status) {

        // Conversion failed.

        return static_cast<TOTYPE>(
                bdetu_Unset<typename make_signed<TOTYPE>::type>::unsetValue());
                                                                      // RETURN
    }

    return result;
}

template <>
bsl::string bcem_AggregateRaw::convertScalar<bsl::string>() const
{
    bsl::string result;
    int status = -1;
    const bdem_EnumerationDef *enumDef = enumerationConstraint();
    if (enumDef) {
        if (bdem_ElemType::BDEM_INT == d_dataType) {
            int enumId = *static_cast<int*>(d_value_p);
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
            result = *static_cast<bsl::string*>(d_value_p);
            status = 0;
        }
    }

    if (0 != status) {
        // If not an enumeration, or if enum-conversion failed, then do normal
        // conversion.

        status = bdem_Convert::fromBdemType(&result,
                                            d_value_p,
                                            d_dataType);
    }

    if (0 != status) {
        // Conversion failed.
        return "";
    }

    return result;
}

int bcem_AggregateRaw::descendIntoFieldByName(
                                         bcem_AggregateError *errorDescription,
                                         const char          *fieldName)
{
    BSLS_ASSERT_SAFE(errorDescription);

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
            if (0 != getFieldIndex(&fieldIndex,
                                   errorDescription,
                                   fieldName,
                                   "field or setField")) {
                return -1;                                            // RETURN
            }
        }

        BSLS_ASSERT(fieldIndex >= -1);

        parentRecDef = d_recordDef;
        if (0 != descendIntoFieldByIndex(errorDescription,
                                         fieldIndex)) {
            return -1;                                                // RETURN
        }

        // Repeat the above operations so long as the above lookup finds
        // an unnamed CHOICE or unnamed LIST:
    } while (bdem_ElemType::isAggregateType(this->d_dataType)
          && 0 == parentRecDef->fieldName(fieldIndex));

    return 0;
}

int bcem_AggregateRaw::descendIntoFieldById(
                                         bcem_AggregateError *errorDescription,
                                         int                  fieldId)
{
    BSLS_ASSERT_SAFE(errorDescription);

    int fieldIndex;
    if (0 != getFieldIndex(&fieldIndex,
                           errorDescription,
                           fieldId,
                           "fieldById or setFieldById")) {
        return 1;                                                     // RETURN
    }

    return descendIntoFieldByIndex(errorDescription, fieldIndex);
}

int
bcem_AggregateRaw::descendIntoFieldByIndex(
                                         bcem_AggregateError *errorDescription,
                                         int                  fieldIndex)
{
    BSLS_ASSERT_SAFE(errorDescription);

    void *valuePtr = d_value_p;

    switch (d_dataType) {
      case bdem_ElemType::BDEM_LIST:
        // Extract bdem_Row from bdem_Choice, then fall through to ROW case.

        valuePtr = &((bdem_List*)valuePtr)->row();
                                                                // FALL THROUGH
      case bdem_ElemType::BDEM_ROW: {
        bdem_Row& row = *(bdem_Row *)valuePtr;
        if ((unsigned)fieldIndex >= (unsigned)row.length()) {
            bsl::ostringstream oss;
            oss << "Invalid field index " << fieldIndex
                << " specified for "
                << bdem_ElemType::toAscii(d_dataType)
                << " \"" << bcem_AggregateRawUtil::recordName(d_recordDef)
                << '"';
            errorDescription->description() = oss.str();
            errorDescription->code() =
                                  bcem_AggregateError::BCEM_ERR_BAD_FIELDINDEX;
            return -1;                                                // RETURN
        }

        // adjust nullness info first
        d_parentType    = d_dataType;
        d_parentData    = d_value_p;
        d_indexInParent = fieldIndex;

        const bdem_FieldDef& field = d_recordDef->field(fieldIndex);
        d_dataType  = field.elemType();
        d_recordDef = field.recordConstraint();
        d_fieldDef  = &field;
        d_value_p   = row[fieldIndex].dataRaw();
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
            bsl::ostringstream oss;

            oss << "Attempt to access field "
                << fieldIndex
                << " in " << bdem_ElemType::toAscii(d_dataType)
                << " \"" << bcem_AggregateRawUtil::recordName(d_recordDef)
                << "\" but field " << selectorIndex
                << " is currently selected";
            errorDescription->description() = oss.str();
            errorDescription->code() =
                                    bcem_AggregateError::BCEM_ERR_NOT_SELECTED;
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
        d_parentData    = d_value_p;
        d_indexInParent = selectorIndex;

        // Descend into current selection
        const bdem_FieldDef& field = d_recordDef->field(selectorIndex);
        d_dataType  = field.elemType();
        d_recordDef = field.recordConstraint();
        d_fieldDef  = &field;
        d_value_p   = choiceItem.selection().dataRaw();
      } break;

      default: {
          bsl::ostringstream oss;
          oss << "Attempt to access field index "
              << fieldIndex << " on non-record type "
              << bdem_ElemType::toAscii(d_dataType);
          errorDescription->description() = oss.str();
          errorDescription->code() =
                                    bcem_AggregateError::BCEM_ERR_NOT_A_RECORD;
          return -1;                                                  // RETURN
      }
    }

    return 0;
}

int bcem_AggregateRaw::descendIntoField(
                         bcem_AggregateError                 *errorDescription,
                         const bcem_AggregateRawNameOrIndex&  fieldOrIdx,
                         bool                                 makeNonNullFlag)
{
    BSLS_ASSERT_SAFE(errorDescription);

    if (fieldOrIdx.isEmpty()) {
        return 1;                                                     // RETURN
    }
    else if (fieldOrIdx.isName()) {
        return descendIntoFieldByName(errorDescription,
                                      fieldOrIdx.name());             // RETURN
    }
    else {  // 'fieldOrIdx.isIndex()'
        return descendIntoArrayItem(errorDescription,
                                    fieldOrIdx.index(),
                                    makeNonNullFlag);                 // RETURN
    }
}

int bcem_AggregateRaw::descendIntoArrayItem(
                                         bcem_AggregateError *errorDescription,
                                         int                  index,
                                         bool                 makeNonNullFlag)
{
    BSLS_ASSERT_SAFE(errorDescription);

    void *valuePtr = d_value_p;

    bdem_ElemType::Type  itemType;
    void                *itemPtr = 0;

    switch (d_dataType) {
      case bdem_ElemType::BDEM_TABLE: {
        bdem_Table& table = *(bdem_Table *) valuePtr;
        if ((unsigned)index >= (unsigned)table.numRows()) {
            break;  // out of bounds
        }

        if (bdem_SchemaUtil::isNillableScalarArrayRecordDef(d_dataType,
                                                            d_recordDef)) {
            bdem_Row     *row = makeNonNullFlag
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

            // d_schema is unchanged.
            d_value_p  = itemPtr;
            d_dataType = itemType;

            return 0;                                                 // RETURN
        }
        else if (d_fieldDef && bdeat_FormattingMode::BDEAT_NILLABLE ==
                                               d_fieldDef->formattingMode()) {
            itemType = bdem_ElemType::BDEM_ROW;
            itemPtr  = makeNonNullFlag
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
            itemPtr = makeNonNullFlag
                    ? &choiceArray.theModifiableItem(index)
             : const_cast<bdem_ChoiceArrayItem *>(&choiceArray.theItem(index));
        }
        else {
            itemPtr  = &choiceArray.theModifiableItem(index);
        }
      } break;
      default: {
        if (bdem_ElemType::isArrayType(d_dataType)) {  // scalar array type
            bcem_AggregateRaw_ArrayIndexer indexer(index);
            if (bcem_AggregateRawUtil::visitArray(valuePtr,
                                                  d_dataType,
                                                  &indexer)) {
                break;  // out of bounds
            }
            itemType = bdem_ElemType::fromArrayType(d_dataType);
            itemPtr  = indexer.data();
        }
        else {
            errorDescription->code() =
                bcem_AggregateError::BCEM_ERR_NOT_AN_ARRAY;
            errorDescription->description() =
                "Attempt to index a non-array object of type ";
            errorDescription->description() +=
                bdem_ElemType::toAscii(d_dataType);
            return -1;                                                // RETURN
        }
      } break;
    }

    if (! itemPtr) {
        // If got here, then have out-of-bounds index
        bsl::ostringstream oss;
        oss << "Invalid array index " << index
            << " used for " << bdem_ElemType::toAscii(d_dataType);
        errorDescription->description() = oss.str();
        errorDescription->code() =
                                  bcem_AggregateError::BCEM_ERR_BAD_ARRAYINDEX;
        return -1;                                                    // RETURN
    }

    // adjust nullness info first

    d_parentType    = d_dataType;
    d_parentData    = d_value_p;
    d_indexInParent = index;

    // d_schema, d_recordDef, and d_fieldDef are unchanged.

    d_value_p  = itemPtr;
    d_dataType = itemType;

    return 0;
}

bsl::ostream& bcem_AggregateRaw::print(bsl::ostream& stream,
                                       int           level,
                                       int           spacesPerLevel) const
{
    if (isError()) {
        bdeu_Print::indent(stream, level, spacesPerLevel);
        stream << "<ERR> "
               << static_cast<bcem_AggregateError*>(d_value_p)->description();
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
                                        d_recordDef,
                                        level,
                                        spacesPerLevel);
    }

    return stream;
}

int bcem_AggregateRaw::getFieldIndex(int                 *index,
                                     bcem_AggregateError *errorResult,
                                     const char          *fieldName,
                                     const char          *caller) const
{
    BSLS_ASSERT_SAFE(index);
    BSLS_ASSERT_SAFE(errorResult);
    BSLS_ASSERT_SAFE(fieldName);
    BSLS_ASSERT_SAFE(caller);

    if (!bdem_ElemType::isAggregateType(d_dataType)) {
        bsl::ostringstream oss;
        oss << "Attempt to call " << caller
            << " with field name \"" << fieldName
            << "\" on array type: " << bdem_ElemType::toAscii(d_dataType);
        errorResult->description() = oss.str();
        errorResult->code() = bcem_AggregateError::BCEM_ERR_NOT_A_RECORD;
        return -1;                                                    // RETURN
    }
    else if (! d_recordDef) {
        bsl::ostringstream oss;
        oss << "Attempt to call " << caller
            << " with field name \"" << fieldName
            << "\" on unconstrained " << bdem_ElemType::toAscii(d_dataType);
        errorResult->description() = oss.str();
        errorResult->code() = bcem_AggregateError::BCEM_ERR_NOT_A_RECORD;
        return -1;                                                    // RETURN
    }
    else if (bdem_RecordDef::BDEM_CHOICE_RECORD == d_recordDef->recordType()
          && ! (fieldName && fieldName[0])) {
        *index = -1;
        return 0;                                                     // RETURN
    }

    *index = d_recordDef->fieldIndexExtended(fieldName);
    if (*index < 0) {
        bsl::ostringstream oss;
        oss << "Invalid field name \"" << fieldName
            << "\" in " << bdem_ElemType::toAscii(d_dataType)
            << " \"" << bcem_AggregateRawUtil::recordName(d_recordDef)
            << "\" passed to " << caller;
        errorResult->description() = oss.str();
        errorResult->code() = bcem_AggregateError::BCEM_ERR_BAD_FIELDNAME;
        return -1;                                                    // RETURN
    }

    return 0;
}

int bcem_AggregateRaw::getFieldIndex(int                 *index,
                                     bcem_AggregateError *errorResult,
                                     int                  fieldId,
                                     const char          *caller) const
{
    BSLS_ASSERT_SAFE(index);
    BSLS_ASSERT_SAFE(errorResult);
    BSLS_ASSERT_SAFE(caller);

    if (!bdem_ElemType::isAggregateType(d_dataType)) {
        bsl::ostringstream oss;
        oss << "Attempt to call " << caller
            << " with field id " << fieldId
            << " on array type: " << bdem_ElemType::toAscii(d_dataType);
        errorResult->description() = oss.str();
        errorResult->code() = bcem_AggregateError::BCEM_ERR_NOT_A_RECORD;
        return -1;                                                    // RETURN
    }
    else if (! d_recordDef) {
        bsl::ostringstream oss;
        oss << "Attempt to call " << caller
            << " with field id " << fieldId
            << " on unconstrained " << bdem_ElemType::toAscii(d_dataType);
        errorResult->description() = oss.str();
        errorResult->code() = bcem_AggregateError::BCEM_ERR_NOT_A_RECORD;
        return -1;                                                    // RETURN
    }
    else if (bdem_RecordDef::BDEM_CHOICE_RECORD == d_recordDef->recordType()
          && bdem_RecordDef::BDEM_NULL_FIELD_ID == fieldId) {
        // It is legal to pass 'BDEM_NULL_FIELD_ID' to a choice.
        *index = -1;
        return 0;                                                     // RETURN
    }

    *index = d_recordDef->fieldIndex(fieldId);
    if (*index < 0) {
        bsl::ostringstream oss;
        oss << "Invalid field ID " << fieldId
            << " in " << bdem_ElemType::toAscii(d_dataType)
            << " \"" << bcem_AggregateRawUtil::recordName(d_recordDef)
            << "\" passed to " << caller;
        errorResult->description() = oss.str();
        errorResult->code() = bcem_AggregateError::BCEM_ERR_BAD_FIELDID;
        return -1;                                                    // RETURN
    }

    return 0;
}

int bcem_AggregateRaw::makeSelectionByIndex(
                                         bcem_AggregateRaw   *field,
                                         bcem_AggregateError *errorDescription,
                                         int                  index) const
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

int bcem_AggregateRaw::makeSelectionByIndexRaw(
                                         bcem_AggregateRaw   *selection,
                                         bcem_AggregateError *errorDescription,
                                         int                  index) const
{
    BSLS_ASSERT_SAFE(selection);
    BSLS_ASSERT_SAFE(errorDescription);
    BSLS_ASSERT_SAFE(-1 == index || 0 <= index);

    bdem_ChoiceArrayItem *choice = 0;

    switch (d_dataType) {
      case bdem_ElemType::BDEM_CHOICE: {
        choice = &((bdem_Choice *)d_value_p)->item();
      } break;
      case bdem_ElemType::BDEM_CHOICE_ARRAY_ITEM: {
        choice = (bdem_ChoiceArrayItem *)d_value_p;
      } break;
      default: {
        errorDescription->description() =
                                  "makeSelection called on aggregate of type ";
        errorDescription->description() += bdem_ElemType::toAscii(d_dataType);
        errorDescription->code() = bcem_AggregateError::BCEM_ERR_NOT_A_CHOICE;
        return -1;                                                  // RETURN
      }
    }

    if (-1 != index && index >= choice->numSelections()) {
        bsl::ostringstream oss;
        oss << "Invalid selection index "
            << index << " in "
            << bdem_ElemType::toAscii(d_dataType) << " \""
            << bcem_AggregateRawUtil::recordName(d_recordDef)
            << "\" passed to makeSelectionByIndex";
        errorDescription->description() = oss.str();
        errorDescription->code() =
                                  bcem_AggregateError::BCEM_ERR_BAD_FIELDINDEX;
        return -1;                                                    // RETURN
    }

    choice->makeSelection(index);

    if (index >= 0) {
        return fieldByIndex(selection, errorDescription, index);  // RETURN
    }
    else {

        // No current selection.  Set to a void object.

        selection->reset();
    }

    return 0;
}

int
bcem_AggregateRaw::findUnambiguousChoice(bcem_AggregateRaw   *choiceObject,
                                         bcem_AggregateError *errorDescription,
                                         const char          *caller) const
{
    BSLS_ASSERT_SAFE(choiceObject);
    BSLS_ASSERT_SAFE(errorDescription);
    BSLS_ASSERT_SAFE(caller);

    *choiceObject = *this;
    if (bdem_ElemType::BDEM_CHOICE            == d_dataType
     || bdem_ElemType::BDEM_CHOICE_ARRAY_ITEM == d_dataType) {

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
             bdem_ElemType::BDEM_CHOICE != choiceObject->dataType());

    switch (rc) {
      case 0: {
        return 0;                                                     // RETURN
      } break;
      case bcem_AggregateError::BCEM_ERR_AMBIGUOUS_ANON: {
        bsl::ostringstream oss;
        oss << caller
            << " called for object with multiple anonymous fields.  "
            << "Cannot pick one.";
        errorDescription->description() = oss.str();
        errorDescription->code() =
                                  bcem_AggregateError::BCEM_ERR_AMBIGUOUS_ANON;
      } break;
      default: {
        bsl::ostringstream oss;
        oss << caller <<  " called on aggregate of type"
            << bdem_ElemType::toAscii(d_dataType);
        errorDescription->description() = oss.str();
        errorDescription->code() = bcem_AggregateError::BCEM_ERR_NOT_A_CHOICE;
      } break;
    }
    return -1;
}

int bcem_AggregateRaw::reserveRaw(bcem_AggregateError *errorDescription,
                                  bsl::size_t          numItems) const
{
    BSLS_ASSERT_SAFE(errorDescription);

    if (!bdem_ElemType::isArrayType(d_dataType)) {
        errorDescription->description() = bdem_ElemType::toAscii(d_dataType);
        errorDescription->description() += " is not an array type";
        errorDescription->code() = bcem_AggregateError::BCEM_ERR_NOT_AN_ARRAY;
        return -1;                                                    // RETURN
    }

    void *valuePtr = d_value_p;
    switch (d_dataType) {
      case bdem_ElemType::BDEM_TABLE: {
        bdem_Table& table = *(bdem_Table *)valuePtr;
        table.reserveRaw(numItems);
      } break;
      case bdem_ElemType::BDEM_CHOICE_ARRAY: {
        bdem_ChoiceArray& array = *(bdem_ChoiceArray *)valuePtr;
        array.reserveRaw(numItems);
      } break;
      default: {
        ArrayReserver reserver(numItems);

        // ArrayReserver always returns 0.

        bcem_AggregateRawUtil::visitArray(valuePtr,
                                          d_dataType,
                                          &reserver);
      } break;
    }
    return 0;
}

int bcem_AggregateRaw::resize(bcem_AggregateError *errorDescription,
                              bsl::size_t          newSize) const
{
    BSLS_ASSERT_SAFE(errorDescription);

    if (! bdem_ElemType::isArrayType(d_dataType)) {
        errorDescription->description() =
                              "Attempt to resize non-array aggregate of type ";
        errorDescription->description() += bdem_ElemType::toAscii(dataType());
        errorDescription->code() = bcem_AggregateError::BCEM_ERR_NOT_AN_ARRAY;
        return -1;                                                    // RETURN
    }

    int currentSize = length();
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

int bcem_AggregateRaw::insertItems(bcem_AggregateError* errorDescription,
                                   int                  index,
                                   int                  numItems) const
{
    BSLS_ASSERT_SAFE(errorDescription);
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(0 <= numItems);

    if (! bdem_ElemType::isArrayType(d_dataType)) {
        errorDescription->description() =
                   "Attempt to insert items into non-array aggregate of type ";
        errorDescription->description() += bdem_ElemType::toAscii(d_dataType);
        errorDescription->code() = bcem_AggregateError::BCEM_ERR_NOT_AN_ARRAY;
        return 3;                                                     // RETURN
    }

    bool isAggNull = isNull();

    bcem_AggregateError::Code status = bcem_AggregateError::BCEM_SUCCESS;
    int arrayLen;
    switch (d_dataType) {
      case bdem_ElemType::BDEM_TABLE: {
        bdem_Table& theTable = *(bdem_Table *)d_value_p;
        arrayLen = theTable.numRows();
        if (index > arrayLen) {
            status = bcem_AggregateError::BCEM_ERR_BAD_ARRAYINDEX;
            break;
        }

        if (isAggNull) {
            makeValue();
        }

        theTable.insertNullRows(index, numItems);
        if (d_recordDef
         && (!d_fieldDef || bdeat_FormattingMode::BDEAT_NILLABLE !=
                                               d_fieldDef->formattingMode())) {
            for (int i = index; i < index + numItems; ++i) {
                bdem_SchemaAggregateUtil::initRowDeep(
                                                 &theTable.theModifiableRow(i),
                                                 *d_recordDef);
            }
        }
      } break;
      case bdem_ElemType::BDEM_CHOICE_ARRAY: {
        bdem_ChoiceArray& theChoiceArray = *(bdem_ChoiceArray*)d_value_p;
        arrayLen = theChoiceArray.length();
        if (index > arrayLen) {
            status = bcem_AggregateError::BCEM_ERR_BAD_ARRAYINDEX;
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
            ArrayInserter inserter(index, numItems, d_fieldDef);
            int rc = bcem_AggregateRawUtil::visitArray(d_value_p,
                                                       d_dataType,
                                                       &inserter);
            if (0 != rc) {
                if (isAggNull) {
                    makeNull();
                }
                status = bcem_AggregateError::BCEM_ERR_BAD_ARRAYINDEX;
            }
            arrayLen = inserter.length();
        }
      } break;
    }

    if (bcem_AggregateError::BCEM_SUCCESS != status) {
        bsl::ostringstream oss;
        oss << "Attempt to insert items at index " << index
            << " into " << bdem_ElemType::toAscii(d_dataType)
            << " of length " << arrayLen;
        errorDescription->description() = oss.str();
        errorDescription->code() = status;
        return status;                                                // RETURN
    }

    return 0;
}

int bcem_AggregateRaw::insertNullItems(bcem_AggregateError* errorDescription,
                                       int                  index,
                                       int                  numItems) const
{
    BSLS_ASSERT_SAFE(errorDescription);
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(0 <= numItems);

    if (! bdem_ElemType::isArrayType(d_dataType)) {
        errorDescription->description() =
                   "Attempt to insert items into non-array aggregate of type ";
        errorDescription->description() += bdem_ElemType::toAscii(d_dataType);
        errorDescription->code() = bcem_AggregateError::BCEM_ERR_NOT_AN_ARRAY;
        return 3;                                                     // RETURN
    }

    bool isAggNull = isNull();

    bcem_AggregateError::Code status = bcem_AggregateError::BCEM_SUCCESS;
    int arrayLen;
    switch (d_dataType) {
      case bdem_ElemType::BDEM_TABLE: {
        bdem_Table& theTable = *(bdem_Table *)d_value_p;
        arrayLen = theTable.numRows();
        if (index > arrayLen) {
            status = bcem_AggregateError::BCEM_ERR_BAD_ARRAYINDEX;
            break;
        }

        if (isAggNull) {
            makeValue();
        }

        theTable.insertNullRows(index, numItems);
      } break;
      case bdem_ElemType::BDEM_CHOICE_ARRAY: {
        bdem_ChoiceArray& theChoiceArray = *(bdem_ChoiceArray*)d_value_p;
        arrayLen = theChoiceArray.length();
        if (index > arrayLen) {
            status = bcem_AggregateError::BCEM_ERR_BAD_ARRAYINDEX;
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
            ArrayInserter inserter(index, numItems, d_fieldDef, true);
            int rc = bcem_AggregateRawUtil::visitArray(d_value_p,
                                                       d_dataType,
                                                       &inserter);
            if (0 != rc) {
                if (isAggNull) {
                    makeNull();
                }
                status = bcem_AggregateError::BCEM_ERR_BAD_ARRAYINDEX;
            }
            arrayLen = inserter.length();
        }
      } break;
    }

    if (bcem_AggregateError::BCEM_SUCCESS != status) {
        bsl::ostringstream oss;
        oss << "Attempt to insert items at index " << index
            << " into " << bdem_ElemType::toAscii(d_dataType)
            << " of length " << arrayLen;
        errorDescription->description() = oss.str();
        errorDescription->code() = status;
        return status;                                                // RETURN
    }

    return 0;
}

int bcem_AggregateRaw::removeItems(bcem_AggregateError *errorDescription,
                                   int                  index,
                                   int                  numItems) const
{
    BSLS_ASSERT_SAFE(errorDescription);
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(0 <= numItems);

    if (! bdem_ElemType::isArrayType(d_dataType)) {
        errorDescription->description() =
                   "Attempt to remove items from non-array aggregate of type ";
        errorDescription->description() += bdem_ElemType::toAscii(d_dataType);
        errorDescription->code() = bcem_AggregateError::BCEM_ERR_NOT_AN_ARRAY;
        return 3;                                                     // RETURN
    }
    else if (index + numItems > length()) {
        bsl::ostringstream oss;
        oss << "Attempt to remove at index " << index
            << " in " << bdem_ElemType::toAscii(d_dataType)
            << " of length " << length();
        errorDescription->description() = oss.str();
        errorDescription->code() =
                                  bcem_AggregateError::BCEM_ERR_BAD_ARRAYINDEX;
        return 4;                                                     // RETURN
    }

    switch (d_dataType) {
      case bdem_ElemType::BDEM_TABLE: {
        bdem_Table& theTable = *(bdem_Table *)d_value_p;
        theTable.removeRows(index, numItems);
      } break;
      case bdem_ElemType::BDEM_CHOICE_ARRAY: {
        bdem_ChoiceArray& theChoiceArray = *(bdem_ChoiceArray *)d_value_p;
        theChoiceArray.removeItems(index, numItems);
      } break;
      default: {
        ArrayItemEraser itemEraser(index, numItems);
        bcem_AggregateRawUtil::visitArray(d_value_p,
                                          d_dataType,
                                          &itemEraser);
      } break;
    }

    return 0;
}

int
bcem_AggregateRaw::makeSelection(bcem_AggregateRaw   *selection,
                                 bcem_AggregateError *errorDescription,
                                 const char          *newSelector) const
{
    BSLS_ASSERT_SAFE(selection);
    BSLS_ASSERT_SAFE(errorDescription);

    int                   newSelectorIndex = -1;
    bool                  foundChoice      = false;
    bcem_AggregateRaw     obj              = *this;
    const bdem_RecordDef *parentRecordDef  = 0;

    // Descend into unnamed choices and sequences until a leaf is found
    do {
        if (0 != obj.getFieldIndex(&newSelectorIndex,
                                   errorDescription,
                                   newSelector,
                                   "makeSelection")) {
            return 4;                                                 // RETURN
        }

        // TBD: We don't reset nullness of top level aggregate if we descend
        // into an aggregate and we face an error making the selection.

        if (isNull()) {
            makeValue();
        }

        parentRecordDef = obj.d_recordDef;
        if (bdem_RecordDef::BDEM_CHOICE_RECORD ==
                                               obj.d_recordDef->recordType()) {
            // CHOICE or CHOICE_ARRAY_ITEM

            if (0 == obj.makeSelectionByIndexRaw(&obj,
                                                 errorDescription,
                                                 newSelectorIndex)) {
                obj.makeValue();
                foundChoice = true;
            }
            else {
                return 5;                                             // RETURN
            }
        }
        else {
            // Even though object is not a choice, it might contain an
            // unnamed choice field or an unnamed sequence that indirectly
            // holds an unnamed choice.  Descend into field:

            if (0 != obj.descendIntoFieldByIndex(errorDescription,
                                                 newSelectorIndex)) {
                return 6;                                             // RETURN
            }
        }

        // Loop so long as we are looking at an unnamed constrained aggregate.
    } while (obj.d_recordDef
          && 0 == parentRecordDef->fieldName(newSelectorIndex));

    if (! foundChoice) {
        // We may have descended through zero or more unnamed aggregates, but
        // none of them were choice aggregates.  'makeSelection' is not
        // appropriate.

        errorDescription->description() = "Called makeSelection "
                                          "on non-choice object of type ";
        errorDescription->description() +=
                                      bdem_ElemType::toAscii(this->d_dataType);
        errorDescription->code() = bcem_AggregateError::BCEM_ERR_NOT_A_CHOICE;
        return 7;                                                     // RETURN
    }

    *selection = obj;

    return 0;
}

void bcem_AggregateRaw::makeValue() const
{
    if (bdem_ElemType::BDEM_VOID == d_dataType) {
        return;                                                       // RETURN
    }

    bdem_ElemRef elemRef = asElemRef();

    if (!elemRef.isNull()) {
        return;                                                       // RETURN
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
            bdem_SchemaAggregateUtil::initRowDeep((bdem_Row *) d_value_p,
                                                  *d_recordDef);
          } break;
          case bdem_ElemType::BDEM_LIST: {
            bdem_SchemaAggregateUtil::initListDeep((bdem_List *) d_value_p,
                                                   *d_recordDef);
          } break;
          case bdem_ElemType::BDEM_CHOICE: {
            bdem_SchemaAggregateUtil::initChoice((bdem_Choice *) d_value_p,
                                                 *d_recordDef);
          } break;
          case bdem_ElemType::BDEM_TABLE: {
            bdem_SchemaAggregateUtil::initTable((bdem_Table *) d_value_p,
                                                *d_recordDef);
          } break;
          case bdem_ElemType::BDEM_CHOICE_ARRAY: {
            bdem_SchemaAggregateUtil::initChoiceArray(
                                                (bdem_ChoiceArray *) d_value_p,
                                                *d_recordDef);
          } break;
          default: {
          } break;
        }
    }
}

int bcem_AggregateRaw::errorCode() const
{
    return isError()
         ? static_cast<bcem_AggregateError*>(d_value_p)->code()
         : 0;
}

bsl::string bcem_AggregateRaw::errorMessage() const
{
    if (! isError()) {
        return "";                                                    // RETURN
    }

    return ((bcem_AggregateError *)data())->description();
}

bsl::string bcem_AggregateRaw::asString() const
{
    if (bdem_ElemType::BDEM_VOID == d_dataType) {

        // Special case: return empty string for 'BDEM_VOID' type.

        return "";                                                    // RETURN
    }

    return convertScalar<bsl::string>();
}

void bcem_AggregateRaw::loadAsString(bsl::string *result) const
{
    if (bdem_ElemType::BDEM_VOID == d_dataType) {

        // Special case: load empty string for 'BDEM_VOID' type.

        result->clear();
    }
    else {
        *result = convertScalar<bsl::string>();
    }
}

bool bcem_AggregateRaw::asBool() const
{
    return convertScalar<bool>();
}

char bcem_AggregateRaw::asChar() const
{
    return convertScalar<char>();
}

short bcem_AggregateRaw::asShort() const
{
    return convertScalar<short>();
}

int bcem_AggregateRaw::asInt() const
{
    return convertScalar<int>();
}

bsls_Types::Int64 bcem_AggregateRaw::asInt64() const
{
    return convertScalar<bsls_Types::Int64>();
}

float bcem_AggregateRaw::asFloat() const
{
    return convertScalar<float>();
}

double bcem_AggregateRaw::asDouble() const
{
    return convertScalar<double>();
}

bdet_Datetime bcem_AggregateRaw::asDatetime() const
{
    return convertScalar<bdet_Datetime>();
}

bdet_DatetimeTz bcem_AggregateRaw::asDatetimeTz() const
{
    return convertScalar<bdet_DatetimeTz>();
}

bdet_Date bcem_AggregateRaw::asDate() const
{
    return convertScalar<bdet_Date>();
}

bdet_DateTz bcem_AggregateRaw::asDateTz() const
{
    return convertScalar<bdet_DateTz>();
}

bdet_Time bcem_AggregateRaw::asTime() const
{
    return convertScalar<bdet_Time>();
}

bdet_TimeTz bcem_AggregateRaw::asTimeTz() const
{
    return convertScalar<bdet_TimeTz>();
}

const bdem_ElemRef bcem_AggregateRaw::asElemRef() const
{
    if (!d_parentData) {

        // top-level aggregate

        const bdem_Descriptor *descriptor =
                                bdem_ElemAttrLookup::lookupTable()[d_dataType];
        int *nullnessWord = bdem_ElemType::BDEM_VOID == d_dataType
                          ? &s_voidNullnessWord
                          : d_isTopLevelAggregateNull_p;

        return bdem_ElemRef(d_value_p, descriptor, nullnessWord, 0);  // RETURN
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
        bdem_ChoiceArrayItem& choiceItem = *(bdem_ChoiceArrayItem *)valuePtr;
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
        return bdem_ElemRef(d_value_p, descriptor);                   // RETURN
      }
    }
}

int bcem_AggregateRaw::getField(bcem_AggregateRaw   *fieldPointer,
                                bcem_AggregateError *errorDescription,
                                bool                 makeNonNullFlag,
                                NameOrIndex          fieldOrIdx1,
                                NameOrIndex          fieldOrIdx2,
                                NameOrIndex          fieldOrIdx3,
                                NameOrIndex          fieldOrIdx4,
                                NameOrIndex          fieldOrIdx5,
                                NameOrIndex          fieldOrIdx6,
                                NameOrIndex          fieldOrIdx7,
                                NameOrIndex          fieldOrIdx8,
                                NameOrIndex          fieldOrIdx9,
                                NameOrIndex          fieldOrIdx10) const
{
    BSLS_ASSERT_SAFE(fieldPointer);
    BSLS_ASSERT_SAFE(errorDescription);

    *fieldPointer = *this;

    int rc = 1;
    do {
        // this "loop" is just to enable exiting from the block of
        // statements easily once we find an "empty" argument

        if (fieldOrIdx1.isEmpty()) break;
        rc = fieldPointer->descendIntoField(errorDescription,
                                            fieldOrIdx1,
                                            makeNonNullFlag);

        if (rc || fieldOrIdx2.isEmpty()) break;
        rc = fieldPointer->descendIntoField(errorDescription,
                                            fieldOrIdx2,
                                            makeNonNullFlag);

        if (rc || fieldOrIdx3.isEmpty()) break;
        rc = fieldPointer->descendIntoField(errorDescription,
                                            fieldOrIdx3,
                                            makeNonNullFlag);

        if (rc || fieldOrIdx4.isEmpty()) break;
        rc = fieldPointer->descendIntoField(errorDescription,
                                            fieldOrIdx4,
                                            makeNonNullFlag);

        if (rc || fieldOrIdx5.isEmpty()) break;
        rc = fieldPointer->descendIntoField(errorDescription,
                                            fieldOrIdx5,
                                            makeNonNullFlag);

        if (rc || fieldOrIdx6.isEmpty()) break;
        rc = fieldPointer->descendIntoField(errorDescription,
                                            fieldOrIdx6,
                                            makeNonNullFlag);

        if (rc || fieldOrIdx7.isEmpty()) break;
        rc = fieldPointer->descendIntoField(errorDescription,
                                            fieldOrIdx7,
                                            makeNonNullFlag);

        if (rc || fieldOrIdx8.isEmpty()) break;
        rc = fieldPointer->descendIntoField(errorDescription,
                                            fieldOrIdx8,
                                            makeNonNullFlag);

        if (rc || fieldOrIdx9.isEmpty()) break;
        rc = fieldPointer->descendIntoField(errorDescription,
                                            fieldOrIdx9,
                                            makeNonNullFlag);

        if (rc || fieldOrIdx10.isEmpty()) break;
        rc = fieldPointer->descendIntoField(errorDescription,
                                            fieldOrIdx10,
                                            makeNonNullFlag);
    } while (false);

    return rc;
}

int bcem_AggregateRaw::fieldById(bcem_AggregateRaw    *field,
                                 bcem_AggregateError  *errorDescription,
                                 int                   fieldId) const
{
    BSLS_ASSERT_SAFE(field);
    BSLS_ASSERT_SAFE(errorDescription);

    *field = *this;
    return field->descendIntoFieldById(errorDescription, fieldId);
}

int bcem_AggregateRaw::fieldByIndex(bcem_AggregateRaw    *field,
                                    bcem_AggregateError  *errorDescription,
                                    int                   index) const
{
    BSLS_ASSERT_SAFE(field);
    BSLS_ASSERT_SAFE(errorDescription);

    *field = *this;
    return field->descendIntoFieldByIndex(errorDescription, index);
}

int bcem_AggregateRaw::anonymousField(bcem_AggregateRaw   *object,
                                      bcem_AggregateError *errorDescription,
                                      int                  index) const
{
    BSLS_ASSERT_SAFE(object);
    BSLS_ASSERT_SAFE(errorDescription);

    if (! d_recordDef) {
        bsl::ostringstream oss;
        oss << "Called anonymousField on unconstrained "
            << bdem_ElemType::toAscii(d_dataType)
            << " object";
        errorDescription->description() = oss.str();
        errorDescription->code() = bcem_AggregateError::BCEM_ERR_NOT_A_RECORD;
        return -1;                                                    // RETURN
    }

    int numAnonFields = d_recordDef->numAnonymousFields();
    if (0 == numAnonFields) {
        bsl::ostringstream oss;
        oss << "Called anonymousField for "
            << bdem_ElemType::toAscii(d_dataType)
            << " \"" << bcem_AggregateRawUtil::recordName(d_recordDef)
            << "\" that contains no anonymous fields";
        errorDescription->description() = oss.str();
        errorDescription->code() =
                                  bcem_AggregateError::BCEM_ERR_BAD_FIELDINDEX;
        return -1;                                                    // RETURN
    }
    else if ((unsigned)index >= (unsigned)numAnonFields) {
        bsl::ostringstream oss;
        oss << "Invalid index " << index << "passed to anonymousField for "
            << bdem_ElemType::toAscii(d_dataType) << " \""
            << bcem_AggregateRawUtil::recordName(d_recordDef) << '"';
        errorDescription->description() = oss.str();
        errorDescription->code() =
                                  bcem_AggregateError::BCEM_ERR_BAD_FIELDINDEX;
        return -1;                                                    // RETURN
    }

    int fldIdx = 0;
    for (int anonIdx = -1; anonIdx < index; ++fldIdx) {
        if (0 == d_recordDef->fieldName(fldIdx)) {
            ++anonIdx;
            if (anonIdx == index) {
                break;  // break loop without incrementing fldIdx
            }
        }
    }

    return fieldByIndex(object, errorDescription, fldIdx);
}

int bcem_AggregateRaw::insertNullItem(bcem_AggregateRaw   *newItem,
                                      bcem_AggregateError *errorDescription,
                                      int                  index) const
{
    BSLS_ASSERT_SAFE(newItem);
    BSLS_ASSERT_SAFE(errorDescription);
    BSLS_ASSERT_SAFE(index >= 0);

    if (! bdem_ElemType::isArrayType(d_dataType)) {
        errorDescription->description() =
                         "Attempt to insert into non-array aggregate of type ";
        errorDescription->description() += bdem_ElemType::toAscii(d_dataType);
        errorDescription->code() = bcem_AggregateError::BCEM_ERR_NOT_AN_ARRAY;
        return 8;                                                     // RETURN
    }

    bool isAggNull = isNull();

    switch (d_dataType) {
      case bdem_ElemType::BDEM_TABLE: {
        bdem_Table& theTable = *(bdem_Table *)d_value_p;
        if (index > theTable.numRows()) {
            bsl::ostringstream oss;
            oss << "Attempt to insert at index " << index
                << " into TABLE of length " << theTable.numRows();
            errorDescription->description() = oss.str();
            errorDescription->code() =
                                  bcem_AggregateError::BCEM_ERR_BAD_ARRAYINDEX;
            return 9;                                                 // RETURN
        }

        if (isAggNull) {
            makeValue();
        }

        theTable.insertNullRow(index);

        return getField(newItem, errorDescription, false, index);     // RETURN
      } break;
      case bdem_ElemType::BDEM_CHOICE_ARRAY: {
        bdem_ChoiceArray& theChoiceArray = *(bdem_ChoiceArray*)d_value_p;
        if (index > theChoiceArray.length()) {
            bsl::ostringstream oss;
            oss << "Attempt to insert at index " << index
                << " into CHOICE_ARRAY of length " << theChoiceArray.length();
            errorDescription->description() = oss.str();
            errorDescription->code() =
                                  bcem_AggregateError::BCEM_ERR_BAD_ARRAYINDEX;
            return 10;                                                // RETURN
        }

        if (isAggNull) {
            makeValue();
        }

        theChoiceArray.insertNullItems(index, 1);

        *newItem                 = *this;
        newItem->d_parentType    = d_dataType;
        newItem->d_parentData    = d_value_p;
        newItem->d_indexInParent = index;

        newItem->d_value_p  = &theChoiceArray.theModifiableItem(index);
        newItem->d_dataType = bdem_ElemType::BDEM_CHOICE_ARRAY_ITEM;

        return 0;                                                     // RETURN
      } break;
      default: {
        if (isAggNull) {
            makeValue();
        }

        ArrayInserter inserter(index, 1, d_fieldDef);
        int status = bcem_AggregateRawUtil::visitArray(d_value_p,
                                                     d_dataType,
                                                     &inserter);
        if (status < 0) {
            bsl::ostringstream oss;
            oss << "Attempt to insert at index " << index
                << " into " << bdem_ElemType::toAscii(d_dataType)
                << " of length " << inserter.length();
            errorDescription->description() = oss.str();
            errorDescription->code() =
                                  bcem_AggregateError::BCEM_ERR_BAD_ARRAYINDEX;
            return 11;                                                // RETURN
        }

        // Return sub-aggregate that refers to newly-inserted array element.

        *newItem = *this;

        newItem->d_parentType    = d_dataType;
        newItem->d_parentData    = d_value_p;
        newItem->d_indexInParent = index;

        newItem->d_value_p  = inserter.data();
        newItem->d_dataType = bdem_ElemType::fromArrayType(d_dataType);

        return 0;                                                     // RETURN
      }
    }
}

void bcem_AggregateRaw::swap(bcem_AggregateRaw& rhs)
{
    bsl::swap(d_dataType, rhs.d_dataType);
    bsl::swap(d_recordDef, rhs.d_recordDef);
    bsl::swap(d_fieldDef, rhs.d_fieldDef);
    bsl::swap(d_schema_p, rhs.d_schema_p);
    bsl::swap(d_value_p, rhs.d_value_p);
    bsl::swap(d_isTopLevelAggregateNull_p, rhs.d_isTopLevelAggregateNull_p);
    bsl::swap(d_parentType, rhs.d_parentType);
    bsl::swap(d_parentData, rhs.d_parentData);
    bsl::swap(d_indexInParent, rhs.d_indexInParent);
}

void bcem_AggregateRaw::clearParent()
{
    d_parentType    = bdem_ElemType::BDEM_VOID;
    d_parentData    = 0;
    d_indexInParent = -1;
}

bool bcem_AggregateRaw::areEquivalent(const bcem_AggregateRaw& lhs,
                                      const bcem_AggregateRaw& rhs)
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
         && ! bdem_SchemaUtil::areEquivalent(*lhs.recordConstraint(),
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

    const bdem_Descriptor *descriptor =
                            bdem_ElemAttrLookup::lookupTable()[lhs.dataType()];

    return lhs.isNull() || descriptor->areEqual(lhs.data(), rhs.data());
}

bool bcem_AggregateRaw::areIdentical(const bcem_AggregateRaw& lhs,
                                     const bcem_AggregateRaw& rhs)
{
    // If identical in these respects, then there is no need to check their
    // respective nullness attributes.  We need to special case for aggregates
    // of type 'bdem_ElemType::BDEM_VOID' as those have empty values.

    return bdem_ElemType::BDEM_VOID != lhs.dataType()
        && lhs.dataType()           == rhs.dataType()
        && lhs.data()               == rhs.data();
}

int bcem_AggregateRaw::length() const
{
    int result;

    switch (d_dataType) {
      case bdem_ElemType::BDEM_LIST: {
        result = ((bdem_List *)d_value_p)->length();
      } break;
      case bdem_ElemType::BDEM_ROW: {
        result = ((bdem_Row *)d_value_p)->length();
      } break;
      case bdem_ElemType::BDEM_TABLE: {
        result = ((bdem_Table *)d_value_p)->numRows();
      } break;
      case bdem_ElemType::BDEM_CHOICE_ARRAY: {
        result = ((bdem_ChoiceArray *)d_value_p)->length();
      } break;
      default: {
        BSLS_ASSERT_SAFE(bdem_ElemType::isArrayType(d_dataType));

        bcem_AggregateRaw_ArraySizer sizer;
        result = bcem_AggregateRawUtil::visitArray(d_value_p,
                                                   d_dataType,
                                                   &sizer);
      } break;
    }

    return result;
}

bool bcem_AggregateRaw::isNull() const
{
    if (!isNullable()) {
        return false;                                                 // RETURN
    }

    bool isElemNull;
    if (bdem_ElemType::isArrayType(d_parentType)) {
        if (bdem_ElemType::BDEM_TABLE == d_parentType) {
            bdem_Table& table = *(bdem_Table *)d_parentData;
            isElemNull = table.isRowNull(d_indexInParent);
        }
        else {
            BSLS_ASSERT(bdem_ElemType::BDEM_CHOICE_ARRAY == d_parentType);

            bdem_ChoiceArray& choiceArray = *(bdem_ChoiceArray *)d_parentData;
            isElemNull = choiceArray.isItemNull(d_indexInParent);
        }
    }
    else {
        isElemNull = isVoid() || asElemRef().isNull();
    }

    return isElemNull;
}

bool bcem_AggregateRaw::isNullable() const
{
    return bdem_ElemType::isArrayType(d_parentType)
        && !bdem_ElemType::isAggregateType(d_parentType)
         ? false
         : true;
}

int bcem_AggregateRaw::makeSelectionById(bcem_AggregateRaw   *field,
                                         bcem_AggregateError *errorDescription,
                                         int                  id) const
{
    BSLS_ASSERT_SAFE(field);
    BSLS_ASSERT_SAFE(errorDescription);

    int index = -1;
    if (0 != getFieldIndex(&index,
                           errorDescription,
                           id,
                           "makeSelectionById")) {
        return -1;                                                    // RETURN
    }

    return makeSelectionByIndex(field, errorDescription, index);
}

int bcem_AggregateRaw::numSelections() const
{
    bcem_AggregateError error;
    bcem_AggregateRaw   choiceObj;

    if (0 != findUnambiguousChoice(&choiceObj, &error, "numSelections")) {
        return error.code();                                          // RETURN
    }

    int retValue;
    switch (choiceObj.d_dataType) {
      case bdem_ElemType::BDEM_CHOICE: {
        retValue = ((bdem_Choice *)choiceObj.d_value_p)->numSelections();
      } break;
      case bdem_ElemType::BDEM_CHOICE_ARRAY_ITEM: {
        retValue =
                ((bdem_ChoiceArrayItem *)choiceObj.d_value_p)->numSelections();
      } break;
      case bdem_ElemType::BDEM_CHOICE_ARRAY: {
        retValue = ((bdem_ChoiceArray *)choiceObj.d_value_p)->numSelections();
      } break;
      default: {
        retValue = bcem_AggregateError::BCEM_ERR_NOT_A_CHOICE;
      } break;
    }
    return retValue;
}

const char *bcem_AggregateRaw::selector() const
{
    bcem_AggregateError error;
    bcem_AggregateRaw   choiceObj;

    if (0 != findUnambiguousChoice(&choiceObj, &error, "selector")) {
        return "";                                                    // RETURN
    }

    int index = choiceObj.selectorIndex();
    return index < 0 ? "" : choiceObj.d_recordDef->fieldName(index);
}

bool bcem_AggregateRaw::hasField(const char *fieldName) const
{
    if (bdem_ElemType::isAggregateType(d_dataType)
     && !bdem_ElemType::isArrayType(d_dataType)
     && d_recordDef) {

        // constrained list or choice aggregate

        return 0 <= d_recordDef->fieldIndexExtended(fieldName);       // RETURN
    }

    return false;
}

bool bcem_AggregateRaw::hasFieldById(int fieldId) const
{
    if (bdem_ElemType::isAggregateType(d_dataType)
     && !bdem_ElemType::isArrayType(d_dataType)
     && d_recordDef) {

        // constrained list or choice aggregate

        return 0 != d_recordDef->lookupField(fieldId);                // RETURN
    }

    return false;
}

bool bcem_AggregateRaw::hasFieldByIndex(int fieldIndex) const
{
    if (bdem_ElemType::isAggregateType(d_dataType)
     && !bdem_ElemType::isArrayType(d_dataType)
     && d_recordDef) {

        // constrained list or choice aggregate

        return (unsigned)fieldIndex < (unsigned)d_recordDef->numFields();
                                                                      // RETURN
    }

    return false;
}

int bcem_AggregateRaw::selectorIndex() const
{
    int index;
    switch (d_dataType) {
      case bdem_ElemType::BDEM_CHOICE: {
        index = ((bdem_Choice *)d_value_p)->selector();
      } break;
      case bdem_ElemType::BDEM_CHOICE_ARRAY_ITEM: {
        index = ((bdem_ChoiceArrayItem *)d_value_p)->selector();
      } break;
      case bdem_ElemType::BDEM_LIST:                            // FALL THROUGH
      case bdem_ElemType::BDEM_ROW: {

        // For non-choice records, find the unambiguous anonymous choice
        // within the record and return the selection index for that, or
        // the error code if there is no unambiguous anonymous choice.

        bcem_AggregateError errorDescription;
        bcem_AggregateRaw   choiceObject;
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
            index = bcem_AggregateError::BCEM_ERR_NOT_A_CHOICE;
        }
      } break;
    }

    return index;
}

int bcem_AggregateRaw::selectorId() const
{
    bcem_AggregateRaw   choiceObj;
    bcem_AggregateError dummy;
    if (0 != findUnambiguousChoice(&choiceObj, &dummy, "selectorId")) {
        return bdem_RecordDef::BDEM_NULL_FIELD_ID;                    // RETURN
    }

    int index = choiceObj.selectorIndex();
    return index < 0
         ? bdem_RecordDef::BDEM_NULL_FIELD_ID
         : choiceObj.d_recordDef->fieldId(index);
}

int bcem_AggregateRaw::anonymousField(
                                   bcem_AggregateRaw   *object,
                                   bcem_AggregateError *errorDescription) const
{
    // initialize to error value

    if (d_recordDef && 1 < d_recordDef->numAnonymousFields()) {
        bsl::ostringstream oss;
        oss << "Called anonymousField for "
            << " \"" << bcem_AggregateRawUtil::recordName(d_recordDef)
            << "\" that contains ambiguous anonymous fields";
        errorDescription->description() = oss.str();
        errorDescription->code() =
                                  bcem_AggregateError::BCEM_ERR_AMBIGUOUS_ANON;
        return -1;                                                    // RETURN
    }

    return anonymousField(object, errorDescription, 0);
}

void bcem_AggregateRaw::makeNull() const
{
    if (bdem_ElemType::BDEM_VOID != d_dataType) {
        if (bdem_ElemType::isArrayType(d_parentType)) {
            if (!bdem_ElemType::isAggregateType(d_parentType)) {
                const bdem_Descriptor *descriptor =
                                bdem_ElemAttrLookup::lookupTable()[d_dataType];
                descriptor->makeUnset(d_value_p);
            }
            else if (bdem_ElemType::BDEM_TABLE == d_parentType) {
                bdem_Table& table = *(bdem_Table *)d_parentData;
                table.makeRowsNull(d_indexInParent, 1);
            }
            else {
                BSLS_ASSERT(bdem_ElemType::BDEM_CHOICE_ARRAY == d_parentType);
                bdem_ChoiceArray& choiceArray =
                                             *(bdem_ChoiceArray *)d_parentData;
                choiceArray.makeItemsNull(d_indexInParent, 1);
            }
        }
        else {
            asElemRef().makeNull();
        }
    }
}

void bcem_AggregateRaw::reset()
{
    d_dataType = bdem_ElemType::BDEM_VOID;
    d_schema_p = 0;
    d_recordDef = 0;
    d_fieldDef  = 0;
    d_value_p = 0;
    d_parentType = bdem_ElemType::BDEM_VOID;
    d_parentData = 0;
    d_indexInParent = -1;
    d_isTopLevelAggregateNull_p = 0;
}

int bcem_AggregateRaw::capacityRaw(bcem_AggregateError *errorDescription,
                                   bsl::size_t         *capacity) const
{
    BSLS_ASSERT_SAFE(errorDescription);
    BSLS_ASSERT_SAFE(capacity);

    if (!bdem_ElemType::isArrayType(d_dataType)) {
        errorDescription->description() = bdem_ElemType::toAscii(d_dataType);
        errorDescription->description() += " is not an array type";
        errorDescription->code() = bcem_AggregateError::BCEM_ERR_NOT_AN_ARRAY;
        return -1;                                                    // RETURN
    }

    void *valuePtr = d_value_p;
    switch (d_dataType) {
      case bdem_ElemType::BDEM_TABLE: {
        bdem_Table& table = *(bdem_Table *)valuePtr;
        *capacity = table.capacityRaw();
      } break;
      case bdem_ElemType::BDEM_CHOICE_ARRAY: {
        bdem_ChoiceArray& array = *(bdem_ChoiceArray *)valuePtr;
        *capacity = array.capacityRaw();
      } break;
      default: {
        ArrayCapacitor capacitor(capacity);
        bcem_AggregateRawUtil::visitArray(valuePtr,
                                          d_dataType,
                                          &capacitor);
      }
    }
    return 0;
}

// CREATORS
bcem_AggregateRaw::bcem_AggregateRaw()
: d_dataType(bdem_ElemType::BDEM_VOID)
, d_schema_p(0)
, d_recordDef(0)
, d_fieldDef(0)
, d_value_p(0)
, d_parentType(bdem_ElemType::BDEM_VOID)
, d_parentData(0)
, d_indexInParent(-1)
, d_isTopLevelAggregateNull_p(0)
{
}

bcem_AggregateRaw::bcem_AggregateRaw(const bcem_AggregateRaw& original)
: d_dataType(original.d_dataType)
, d_schema_p(original.d_schema_p)
, d_recordDef(original.d_recordDef)
, d_fieldDef(original.d_fieldDef)
, d_value_p(original.d_value_p)
, d_parentType(original.d_parentType)
, d_parentData(original.d_parentData)
, d_indexInParent(original.d_indexInParent)
, d_isTopLevelAggregateNull_p(original.d_isTopLevelAggregateNull_p)
{
}

bcem_AggregateRaw& bcem_AggregateRaw::operator=(const bcem_AggregateRaw& rhs)
{
    if (this != &rhs) {
        d_dataType = rhs.d_dataType;
        d_schema_p = rhs.d_schema_p;
        d_recordDef = rhs.d_recordDef;
        d_fieldDef = rhs.d_fieldDef;
        d_value_p = rhs.d_value_p;
        d_parentType = rhs.d_parentType;
        d_parentData = rhs.d_parentData;
        d_indexInParent = rhs.d_indexInParent;
        d_isTopLevelAggregateNull_p = rhs.d_isTopLevelAggregateNull_p;
    }
    return *this;
}

// ============================================================================
//                     'bdeat_arrayfunctions' overloads
// ============================================================================

int bdeat_arrayManipulateElement(bcem_AggregateRaw* array,
                                 bdef_Function<int(*)(bcem_AggregateRaw*)>&
                                                    manipulator,
                                 int                index)
{
    bcem_AggregateRaw   element;
    bcem_AggregateError dummy;
    if (0 != array->arrayItem(&element, &dummy, index)) {
        return -1;                                                    // RETURN
    }

    return manipulator(&element);
}

// ============================================================================
//                         'bdeat_typename' overloads
// ============================================================================

const char *bdeat_TypeName_className(const bcem_AggregateRaw& object)
{
    const char           *name = 0;
    const bdem_RecordDef *recordDef = object.recordConstraint();
    if (recordDef) {
        name = recordDef->recordName();
    }

    const bdem_EnumerationDef *enumerationDef = object.enumerationConstraint();
    if (enumerationDef) {
        name = enumerationDef->enumerationName();
    }

    return name;
}

// ============================================================================
//                       'bdeat_valuetype' overloads
// ============================================================================

void bdeat_valueTypeReset(bcem_AggregateRaw *object)
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
//                      'bdeat_choicefunctions' overloads
// ============================================================================


bool bdeat_choiceHasSelection(const bcem_AggregateRaw&  object,
                              const char               *selectionName,
                              int                       selectionNameLength)
{
    NullTerminatedString name(selectionName, selectionNameLength);

    return object.hasField(name);
}

int bdeat_choiceMakeSelection(bcem_AggregateRaw *object,
                              int                selectionId)
{
    bcem_AggregateRaw   dummyField;
    bcem_AggregateError dummyError;
    if (bdeat_ChoiceFunctions::BDEAT_UNDEFINED_SELECTION_ID == selectionId) {
        return object->makeSelectionById(&dummyField,
                                         &dummyError,
                                         bdem_RecordDef::BDEM_NULL_FIELD_ID);
                                                                      // RETURN
    }

    if (! object->hasFieldById(selectionId)) {
        return -1;                                                    // RETURN
    }

    return object->makeSelectionById(&dummyField, &dummyError, selectionId);
}

int bdeat_choiceMakeSelection(bcem_AggregateRaw *object,
                              const char        *selectionName,
                              int                selectionNameLength)
{
    NullTerminatedString name(selectionName, selectionNameLength);

    if (0 == object->hasField(name)) {
        return -1;                                                    // RETURN
    }

    bcem_AggregateRaw   field;
    bcem_AggregateError dummy;
    return object->makeSelection(&field, &dummy, name);
}


// ============================================================================
//                     'bdeat_enumfunctions' overloads
// ============================================================================
int bdeat_enumFromInt(bcem_AggregateRaw *result, int enumId)
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

int bdeat_enumFromString(bcem_AggregateRaw *result,
                         const char        *string,
                         int                stringLength)
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

// ============================================================================
//                     'bdeat_typecategory' overloads
// ============================================================================

bdeat_TypeCategory::Value
bdeat_typeCategorySelect(const bcem_AggregateRaw& object)
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

} // namespace BloombegLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2011
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
