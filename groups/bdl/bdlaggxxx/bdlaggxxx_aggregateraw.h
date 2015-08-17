// bdlaggxxx_aggregateraw.h                                           -*-C++-*-
#ifndef INCLUDED_BDLAGGXXX_AGGREGATERAW
#define INCLUDED_BDLAGGXXX_AGGREGATERAW

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a type for self-describing data (without reference count).
//
//@CLASSES:
//   bdlaggxxx::AggregateRaw: pointer to fully-introspective, dynamically-typed data
//
//@SEE_ALSO: bdlaggxxx_aggregate, bdlaggxxx_errorattributes, bdem package
//
//@AUTHOR: David Schumann (dschumann1)
//
//@DESCRIPTION: This component provides a class representing dynamically typed,
// introspective data optionally described by a 'bdlmxxx::Schema'.  The type is
// similar to a "raw" (non-reference-counted) pointer in the sense that
// multiple 'bdlaggxxx::AggregateRaw' objects can refer to the same data and they do
// not dispose of that data when they go out of scope.
//
// Thus 'bdlaggxxx::AggregateRaw' is provided to allow efficient implementation of
// operations such as "visiting" the fields of a large object recursively (that
// may be inefficient on a reference counted aggregate, see 'bdlaggxxx::Aggregate').
// Typically during such operations, a single aggregate object representing the
// top-level value remains in scope, and many sub-aggregate objects
// representing fields in that value are created and destroyed on the stack.
// The atomic operations required to maintain the reference count would be
// wasted in this scenario, as the count never reaches zero and when the
// operation is concluded there is no net change in the reference count.  Using
// 'bdlaggxxx::AggregateRaw' to represent the sub-elements of the object will
// eliminate this expense.
//
///Data Representation
///-------------------
// 'bdlaggxxx::AggregateRaw' has a 3-tuple of pointers: the address of a
// 'bdlmxxx::Schema' describing the structure of the data; a 'void *' pointing at
// the actual data; and the address of a control word holding a "top-level
// nullness bit." When an Aggregate object contains a nullable element, and
// that element is null, that information is stored in the parent element.  But
// for top-level objects, those not contained within other objects, the
// "nullness" of the object must be represented somewhere; the "top-level
// nullness bit" is used for this purpose.  See the 'bdlmxxx::ElemRef' component
// documentation, "Element Reference Nullability", for a discussion of nullness
// control words (described there as "bitmaps").
//
// 'bdlaggxxx::AggregateRaw' also holds information referring to the parent element.
// Like the schema, data, and nullness data, this reference is uncounted and
// thus the parent must remain valid for the lifetime of the
// 'bdlaggxxx::AggregateRaw' object.
//
///Error Handling
///--------------
// 'bdlaggxxx::AggregateRaw' returns descriptive errors when possible.  These consist
// of an enumerated value of 'bdlaggxxx::ErrorCode::Code', combined with a
// human-readable 'string' value.  In general, most methods of
// 'bdlaggxxx::AggregateRaw' will return an arbitrary non-zero value on failure and
// populate a 'bdlaggxxx::ErrorAttributes' (passed by pointer as an output parameter)
// with details.  See the documentation of 'bdlaggxxx::ErrorAttributes' for the
// definition of the enumerated error conditions.
//
///Thread Safety
///-------------
// A 'bdlaggxxx::AggregateRaw' maintains a non-reference-counted handle to
// possibly-shared data.  It is not safe to access or modify this shared data
// concurrently from different threads, therefore 'bdlaggxxx::AggregateRaw' is,
// strictly speaking, *thread* *unsafe* as two aggregates may refer to the same
// shared data.  However, it is safe to concurrently access or modify two
// different 'bdlaggxxx::AggregateRaw' objects refering to different data.
//
///Usage
///-----
// Using 'bdlaggxxx::AggregateRaw' typically involves starting with a
// 'bdlaggxxx::Aggregate' object and extracting 'bdlaggxxx::AggregateRaw' from it,
// then working with that in place of the original aggregate.  In this example,
// we elide the initial population of the 'bdlaggxxx::AggregateRaw' object and
// instead focus on iterating through fields of an array aggregate.
//..
//  void printFields(const bdlaggxxx::AggregateRaw& object)
//  {
//     for (int i = 0; i < object.length(); ++i) {
//        bdlaggxxx::AggregateRaw field;
//        bdlaggxxx::ErrorAttributes error;
//        if (0 == object.fieldByIndex(&field, &error, i)) {
//            field.print(bsl::cout, 0, -1);
//        }
//     }
//  }
//..

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

#ifndef INCLUDED_BDLAGGXXX_ERRORATTRIBUTES
#include <bdlaggxxx_errorattributes.h>
#endif

#ifndef INCLUDED_BDLAGGXXX_ERRORCODE
#include <bdlaggxxx_errorcode.h>
#endif

#ifndef INCLUDED_BDLAGGXXX_FIELDSELECTOR
#include <bdlaggxxx_fieldselector.h>
#endif

#ifndef INCLUDED_BDLAT_ARRAYFUNCTIONS
#include <bdlat_arrayfunctions.h>
#endif

#ifndef INCLUDED_BDLAT_NULLABLEVALUEFUNCTIONS
#include <bdlat_nullablevaluefunctions.h>
#endif

#ifndef INCLUDED_BDLAT_TYPECATEGORY
#include <bdlat_typecategory.h>
#endif

#ifndef INCLUDED_BDLMXXX_CHOICEARRAYITEM
#include <bdlmxxx_choicearrayitem.h>
#endif

#ifndef INCLUDED_BDLMXXX_CONVERT
#include <bdlmxxx_convert.h>
#endif

#ifndef INCLUDED_BDLMXXX_FIELDDEF
#include <bdlmxxx_fielddef.h>
#endif

#ifndef INCLUDED_BDLMXXX_RECORDDEF
#include <bdlmxxx_recorddef.h>
#endif

#ifndef INCLUDED_BDLMXXX_ROW
#include <bdlmxxx_row.h>
#endif

#ifndef INCLUDED_BDLMXXX_SCHEMA
#include <bdlmxxx_schema.h>
#endif

#ifndef INCLUDED_BDLMXXX_SELECTBDEMTYPE
#include <bdlmxxx_selectbdemtype.h>
#endif

#ifndef INCLUDED_BDLMXXX_TABLE
#include <bdlmxxx_table.h>
#endif

#ifndef INCLUDED_BDLTUXXX_UNSET
#include <bdltuxxx_unset.h>
#endif

#ifndef INCLUDED_BSL_SSTREAM
#include <bsl_sstream.h>
#endif

#ifndef INCLUDED_BSL_UTILITY
#include <bsl_utility.h>
#endif

namespace BloombergLP {

namespace bdlaggxxx {
                     // =======================================
                     // class AggregateRaw_ElemDataFetcher
                     // =======================================

struct AggregateRaw_ElemDataFetcher {
    // This class accesses the address of a value held within a 'bdlmxxx::ElemRef'
    // or 'bdlmxxx::ConstElemRef' without affecting the nullness of the referenced
    // value.

    void *d_data_p;

    explicit
    AggregateRaw_ElemDataFetcher(const bdlmxxx::ElemRef& elemRef)
    {
        d_data_p = elemRef.dataRaw();
    }

    explicit
    AggregateRaw_ElemDataFetcher(const bdlmxxx::ConstElemRef& elemRef)
    {
        d_data_p = const_cast<void *>(elemRef.data());
    }
};

                     // ====================================
                     // class AggregateRaw_ArrayIndexer
                     // ====================================

class AggregateRaw_ArrayIndexer {
    // This class defines a function object to access the n'th element in
    // an array.

    // DATA
    int   d_index;   // index of array element to be accessed
    void *d_item_p;  // address of element (initially null)

    // NOT IMPLEMENTED
    AggregateRaw_ArrayIndexer(const AggregateRaw_ArrayIndexer&);
    AggregateRaw_ArrayIndexer& operator=(
                                        const AggregateRaw_ArrayIndexer&);

  public:
    // CREATORS
    explicit
    AggregateRaw_ArrayIndexer(int index)
    : d_index(index)
    , d_item_p(0)
    {
    }

    // MANIPULATORS
    template <class ARRAYTYPE>
    int operator()(ARRAYTYPE *array)
    {
        int rc;
        if ((unsigned)d_index < array->size()) {
            d_item_p = & (*array)[d_index];
            rc = 0;
        }
        else {
            d_item_p = 0;
            rc = -1;
        }
        return rc;
    }

    // ACCESSORS
    void *data() const
    {
        return d_item_p;
    }
};

                         // ==================================
                         // class AggregateRaw_ArraySizer
                         // ==================================

struct AggregateRaw_ArraySizer {
    // This class defines a function object to return the size of a sequence.

    // ACCESSORS
    template <class ARRAYTYPE>
    int operator()(ARRAYTYPE *array) const
    {
        return (int)array->size();
    }
};

                        // =======================
                        // class AggregateRaw
                        // =======================

class AggregateRaw {
    // This type provides a non-counted reference to the data of a
    // 'Aggregate' object.  It can be used to efficiently implement
    // operations, particularly recursive "visiting", where an aggregate
    // remains in scope throughout the operation and there would otherwise be
    // many temporary aggregate objects created and discarded.

    // Data invariants:
    // - If 'd_dataType' is 'bdlmxxx::ElemType::BDEM_VOID', then 'd_value' will be
    //   null or point to an error record.  The remaining invariants need not
    //   hold.
    // - If 'd_schema_p' is null, then both 'd_recordDef_p' and 'd_fieldDef_p'
    //   are null.
    // - If 'd_schema_p' is non-null, then 'd_recordDef_p' and/or
    //   'd_fieldDef_p' are non-null.
    // - 'd_recordDef_p' is either null or points to a record within
    //   'd_schema_p'.  Its memory is not managed separately from the schema's.
    // - 'd_fieldDef_p' is either null or points to a field definition within
    //   'd_schema_p'.  Its memory is not managed separately from the schema's.
    // - If 'd_fieldDef_p' is not null, then 'd_fieldDef_p->elemType()' is
    //   equal to either 'd_dataType' or to
    //   'bdlmxxx::ElemType::toArrayType(d_dataType)'.  The code in this class
    //   always uses 'd_dataType', not 'd_fieldDef_p->elemType()'.
    // - If this is the root object, then 'd_fieldDef_p' will be null, but
    //   'd_recordDef_p' may still have a value.  Otherwise,
    //   'd_recordDef_p'.  The code in this class always uses 'd_recordDef_p',
    //   not 'd_fieldDef_p->recordConstraint()' will always be equal to
    //   'd_fieldDef_p->recordConstraint()'.

    // DATA
    bdlmxxx::ElemType::Type   d_dataType;       // value's type
    const bdlmxxx::Schema    *d_schema_p;       // shared schema (held, not owned)
    const bdlmxxx::RecordDef *d_recordDef_p;    // record constraint (held, not
                                            // owned)
    const bdlmxxx::FieldDef  *d_fieldDef_p;     // data description (held, not
                                            // owned)
    void                 *d_value_p;        // pointer to data (held, not
                                            // owned)
    bdlmxxx::ElemType::Type   d_parentType;     // type of parent of this aggregate
                                            // ('VOID' if top-level)
    void                 *d_parentData_p;   // address of owner; 0 if top-level
                                            // (held, not owned)
    int                   d_indexInParent;  // index into parent of this
                                            // aggregate; -1 if top-level,
                                            // scalar, or vector

    int                  *d_isTopLevelAggregateNull_p;
                                            // nullness indicator for
                                            // top-level aggregate in bit 0

    // TBD: Reorder functions

    // PRIVATE MANIPULATORS
    int descendIntoArrayItem(ErrorAttributes *errorDescription,
                             int                   index,
                             bool                  makeNonNullFlag);
        // Modify this aggregate so that it refers to the item at the specified
        // 'index' within the currently-referenced array (including table and
        // choice array).  If this aggregate references a nillable array, the
        // element at 'index' is null, and the specified 'makeNonNullFlag' is
        // 'true', assign that element its default value; otherwise leave the
        // element unmodified.  Return 0 on success, with no effect on the
        // specified 'errorDescription'; otherwise, load into
        // 'errorDescription' a description of the failure and return a nonzero
        // value.

    int descendIntoField(ErrorAttributes      *errorDescription,
                         const FieldSelector&  fieldOrIdx,
                         bool                       makeNonNullFlag);
        // Modify this aggregate so that it refers to the field with the
        // specified 'fieldOrIdx' within the currently-referenced sequence or
        // choice object.  If this aggregate references a nillable array, the
        // element having 'fieldOrIdx' is null, and the specified
        // 'makeNonNullFlag' is 'true', assign that element its default value;
        // otherwise leave the element unmodified.  Return 0 on success, with
        // no effect on the specified 'errorDescription'; otherwise, load into
        // 'errorDescription' a description of the failure and return a nonzero
        // value.

    int descendIntoFieldById(ErrorAttributes *errorDescription,
                             int                   id);
        // Modify this aggregate so that it refers to the field with the
        // specified 'id' within the currently-referenced sequence or choice
        // object.  Return 0 on success, with no effect on the specified
        // 'errorDescription'; otherwise, load into 'errorDescription' a
        // description of the failure and return a nonzero value.

    int descendIntoFieldByIndex(ErrorAttributes *errorDescription,
                                int                   index);
        // Modify this aggregate so that it refers to the field with the
        // specified 'index' within the currently-referenced sequence or choice
        // object.  Return 0 on success, with no effect on the specified
        // 'errorDescription'; otherwise, load into 'errorDescription' a
        // description of the failure and return a nonzero value.

    int descendIntoFieldByName(ErrorAttributes *errorDescription,
                               const char           *name);
        // Modify this aggregate so that it refers to the field with the
        // specified 'name' within the currently-referenced sequence or choice
        // object.  Return 0 on success, with no effect on the specified
        // 'errorDescription'; otherwise, load into 'errorDescription' a
        // description of the failure and return a nonzero value.

    // PRIVATE ACCESSORS
    template <class TYPE>
    int assignToNillableScalarArrayImp(const TYPE& value) const;
        // Assign the specified 'value' to this aggregate.  Return 0 on
        // success, and a non-zero value otherwise.  The behavior is undefined
        // unless this aggregate refers to a nillable escalar array, and
        // 'value' is a scalar array, or is convertible to one.  If value is
        // null, then make this aggregate null.  Leave this aggregate unchanged
        // if 'value' is not convertible to the type stored in this aggregate.
        // The parameterized 'TYPE' shall be either 'bdlmxxx::ElemRef' or
        // 'bdlmxxx::ConstElemRef'.

    template <class TYPE>
    int assignToNillableScalarArray(const TYPE& value) const;
    template <class TYPE>
    int assignToNillableScalarArray(const bsl::vector<TYPE>& value) const;
        // Assign the specified 'value' to this aggregate.  Return 0 on
        // success, and a non-zero value otherwise.  The behavior is undefined
        // unless this aggregate refers to a nillable scalar array, and 'value'
        // is a scalar array, or is convertible to one.  If value is null,
        // then make this aggregate null.  Leave this aggregate unchanged if
        // 'value' is not convertible to the type stored in this aggregate.

    int getFieldIndex(int                  *index,
                      ErrorAttributes *errorDescription,
                      const char           *fieldName,
                      const char           *caller) const;
    int getFieldIndex(int                  *index,
                      ErrorAttributes *errorDescription,
                      int                   fieldId,
                      const char           *caller) const;
        // On behalf of the specified 'caller', load into the specified 'index'
        // the index of the field indicated by the specified 'fieldName' or
        // 'fieldId' in the record definition of this aggregate.  Return 0 on
        // success, with no effect on the specified 'errorDescription';
        // otherwise, load into 'errorDescription' a description of the failure
        // (incorporating the value of 'caller') and return a nonzero value.

    bool isNillableScalarArray() const;
        // Return 'true' if this object refers to a nillable scalar array and
        // 'false' otherwise.  Note that an object refers to a nillable scalar
        // array if its record constraint has only one field of a scalar type
        // and having no field name.

    int makeSelectionByIndexRaw(AggregateRaw    *result,
                                ErrorAttributes *errorDescription,
                                int                   index) const;
        // Change the selector in the referenced choice object to the one at
        // the specified 'index', and load a reference to the new selection
        // into the specified 'result' if 'result' is not 0.  The new selection
        // will not be initialized as in 'makeSelection' (so makeValue() will
        // need to be invoked before using the new selection).  If
        // '-1 == index' then the selector value of this object is reset to its
        // default value, with no effect on 'result'.  Return 0 on success, or
        // a nonzero value otherwise with the specified 'errorDescription'
        // loaded with details.

    template <class VALUETYPE>
    int toEnum(ErrorAttributes *errorDescription,
               const VALUETYPE&      value) const;
    template <class VALUETYPE>
    int toEnum(ErrorAttributes *errorDescription,
               const VALUETYPE&      value,
               bslmf::MetaInt<0>     direct) const;
    int toEnum(ErrorAttributes *errorDescription,
               const int&,
               bslmf::MetaInt<0>     direct) const;
    int toEnum(ErrorAttributes *errorDescription,
               const char           *value,
               bslmf::MetaInt<1>     direct) const;
    int toEnum(ErrorAttributes *errorDescription,
               const bsl::string&    value,
               bslmf::MetaInt<1>     direct) const;
    int toEnum(ErrorAttributes     *errorDescription,
               const bdlmxxx::ConstElemRef&  value,
               bslmf::MetaInt<1>         direct) const;
        // Set this enumeration to the specified 'value'.  The 'direct'
        // argument is to aid in template metaprogramming for overloading for
        // those types that can be directly processed and those that must first
        // be converted to 'int' using 'bdlmxxx::Convert'.  Return 0 on success or
        // a nonzero value with a description loaded into the specified
        // 'errorDescription' on failure.

  public:
    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS2(AggregateRaw,
                                  bslalg::TypeTraitBitwiseMoveable,
                                  bdlb::TypeTraitHasPrintMethod);

    // CLASS METHODS
    static int maxSupportedBdexVersion();
        // Return the most current 'bdex' streaming version number supported
        // by this class.  See the BDE package-group-level documentation for
        // more information on 'bdex' streaming of container types.

    static bool areEquivalent(const AggregateRaw& lhs,
                              const AggregateRaw& rhs);
        // Return 'true' if the value of the object referenced by the specified
        // 'lhs' aggregate equals the value of the object referenced by the
        // specified 'rhs' aggregate.  The aggregates need not be identical (as
        // per 'areIdentical'), but must have equivalent record definitions and
        // equal data and nullness values.  This predicate is roughly
        // equivalent to dereferencing two pointers and comparing their
        // pointed-to values for equality.  'areIdentical(lhs, rhs)' implies
        // 'areEquivalent(lhs, rhs)', but not vice versa.

    static bool areIdentical(const AggregateRaw& lhs,
                             const AggregateRaw& rhs);
        // Return 'true' if the specified 'lhs' aggregate refers to the same
        // object as the specified 'rhs' aggregate and 'false' otherwise.
        // Specifically, 'lhs' and 'rhs' are considered identical if 'dataType'
        // and 'dataPtr' return equal values for the respective arguments.
        // When two 'AggregateRaw' objects compare identical,
        // modifications to one of their referred-to object will be visible
        // through the other.  This predicate is roughly equivalent to
        // comparing two pointers for equality.  Note that if
        // 'bdlmxxx::ElemType::BDEM_VOID == lhs.dataType()' or
        // 'bdlmxxx::ElemType::BDEM_VOID == rhs.dataType()', then 'false' is
        // returned.

    template <class TYPE>
    static bdlmxxx::ElemType::Type getBdemType(const TYPE& value);
        // Return the 'bdlmxxx::ElemType::Type' corresponding to the parameterized
        // 'value'.

    // CREATORS
    AggregateRaw();
        // Create an aggregate reference in an empty state.

    AggregateRaw(const AggregateRaw& original);
        // Create a raw aggregate that refers to the same schema and data as
        // that referred to by the specified 'original' raw aggregate.  The
        // behavior is undefined unless the schema and data referred to by
        // 'other' remains valid for the lifetime of this object.

    ~AggregateRaw();
        // Destroy this object.

    // MANIPULATORS
    AggregateRaw& operator=(const AggregateRaw& rhs);
        // Make this aggregate refer to the same data and schema as the
        // specified 'rhs' aggregate and return a reference providing
        // modifiable access to this aggregate.  This creates a new reference
        // to existing data -- no data is copied.  The behavior is undefined
        // unless the schema and data referred to by 'other' remain valid for
        // the lifetime of this object.  Note that if 'rhs' is an error
        // aggregate, then this aggregate will be assigned the same error state
        // as 'rhs'.

    void clearParent();
        // Make this aggregate a top-level aggregate by resetting the
        // parent information.

    void reset();
        // Reset this object to its default-constructed state.

    void setData(void *data);
        // Set the data pointer for this "raw" aggregate to the specified
        // 'data'.  The behavior is undefined unless 'data' remains valid for
        // the lifetime of this object.

    void setDataType(bdlmxxx::ElemType::Type dataType);
        // Set the type of data referenced by this "raw" aggregate to the
        // specified 'dataType'.

    void setFieldDef(const bdlmxxx::FieldDef *fieldDef);
        // Set the field definition pointer for this "raw" aggregate to the
        // specified 'fieldDef'.  The behavior is undefined unless 'fieldDef'
        // remains valid for the lifetime of this object.

    void setRecordDef(const bdlmxxx::RecordDef *recordDef);
        // Set the record definition poiner for this "raw" aggregate to the
        // specified 'recordDef'.  The behavior is undefined unless 'recordDef'
        // remains valid for the lifetime of this object.

    void setSchema(const bdlmxxx::Schema *schema);
        // Set the schema pointer for this "raw" aggregate to the specified
        // 'schema'.  The behavior is undefined unless 'schema' remains valid
        // for the lifetime of this object.

    void setTopLevelAggregateNullness(int *nullnessFlag);
        // Set the address of the top-level nullness bit for this "raw"
        // aggregate to the specified 'nullnessFlag'.  See "Data
        // Representation" in the component-level documentation for a
        // description of the top-level nullness bit.

    template <class STREAM>
    STREAM& bdexStreamIn(STREAM& stream, int version) const;
        // Assign to the object referenced by this aggregate the value read
        // from the specified input 'stream' using the specified 'version'
        // format and return a reference to the modifiable 'stream'.  This
        // aggregate must be initialized with the type and record definition of
        // the expected input before calling this method.  If 'stream' is
        // initially invalid, this operation has no effect.  If 'stream'
        // becomes invalid during this operation, this object is valid, but its
        // value is undefined.  If 'version' is not supported, 'stream' is
        // marked invalid and the object is unaltered.  Note that no version is
        // read from 'stream'.  This operation has the same effect as calling
        // 'bdexStreamIn' on the referenced object (e.g., if 'dataType()' is
        // 'LIST', call 'bdlmxxx::List::bdexStreamIn' on the referenced object).
        // See the 'bdex' package-level documentation for more information on
        // 'bdex' streaming of value-semantic types and containers.

                                  // Aspects

    void swap(AggregateRaw& other);
        // Efficiently exchange the states of this aggregate object and the
        // specified 'other' aggregate object such that value, schema, and
        // nullness information held by each will be what was formerly held by
        // the other.

    // ACCESSORS
    int anonymousField(AggregateRaw    *object,
                       ErrorAttributes *errorDescription) const;
        // If this aggregate contains exactly one field with a null name (i.e.,
        // an anonymous field), then load into the specified 'object' an
        // aggregate representing that field and return 0; otherwise return a
        // non-zero value, and load a descriptive error into
        // 'errorDescription'.  An anonymous field is a field with with a null
        // name.

    int anonymousField(AggregateRaw    *object,
                       ErrorAttributes *errorDescription,
                       int                   index) const;
        // Load into the specified 'object' a representation of the the field
        // with a null name (i.e., an anonymous field) within this object and
        // corresponding to the specified 'index'.  'index' is the index of the
        // field within the anonymous fields of this object, not within all the
        // fields of this object.  If there are not at least 'index + 1'
        // anonymous fields in this object, load a descriptive error into
        // 'errorDescription' and return a nonzero value; otherwise return 0
        // with no effect on 'errorDescription'.

    int getArrayItem(AggregateRaw    *item,
                     ErrorAttributes *errorDescription,
                     int                   index) const;
        // Load into the specified 'item' an aggregate referring to the item at
        // the specified 'index' within the currently-referenced array.  Return
        // 0 on success, with no effect on the specified 'errorDescription';
        // otherwise, load into 'errorDescription' a description of the failure
        // and return a nonzero value.

    bool asBool() const;
    char asChar() const;
    short asShort() const;
    int asInt() const;
    bsls::Types::Int64 asInt64() const;
    float asFloat() const;
    double asDouble() const;
    bdlt::Datetime asDatetime() const;
    bdlt::DatetimeTz asDatetimeTz() const;
    bdlt::Date asDate() const;
    bdlt::DateTz asDateTz() const;
    bdlt::Time asTime() const;
    bdlt::TimeTz asTimeTz() const;
        // Convert the value referenced by this aggregate to the return type
        // using "Extended Type Conversions" as described in the
        // 'Aggregate' component-level documentation (returning the
        // enumerator ID when converting enumeration objects to numeric
        // values).  Return the appropriate "null" value if conversion fails.

    const bdlmxxx::ElemRef asElemRef() const;
        // Return an element reference providing non-modifiable access to the
        // value held by this aggregate.

    bsl::string asString() const;
        // Return a text representation of the value referenced by this
        // aggregate.  For enumeration values, the resulting string is the
        // enumerator name corresponding to the referenced value.  For date and
        // time values, the resulting string will have Iso8601 format.  For
        // other types, the resulting string will have the same format as
        // 'ostream' printing of the underlying data value.

    // TBD: Remove
    template <class TOTYPE>
    TOTYPE convertScalar() const;
        // Return the scalar value stored in this aggregate converted to the
        // parameterized 'TOTYPE'.  Return an "unset" 'TOTYPE' value (see
        // 'bdltuxxx_unset') unless this aggregate holds a scalar value that is
        // convertible to 'TOTYPE'.  'TOTYPE' shall be one of: 'bool', 'char',
        // 'short', 'int', 'bsls::Types::Int64', 'float', 'double',
        // 'bdlt::Datetime', 'bdlt::DatetimeTz', 'bdlt::Date', 'bdlt::DateTz',
        // 'bdlt::Time', 'bdlt::TimeTz'; or, unsigned versions of these.  But
        // note that if TOTYPE is unsigned, and this aggregate is not
        // convertible to 'TOTYPE', then the unset value of the corresponding
        // signed type is returned.

    const void *data() const;
        // Return the address of the non-modifiable data referenced by this
        // aggregate.

    bdlmxxx::ElemType::Type dataType() const;
        // Return the type of data referenced by this aggregate.  Return
        // 'bdlmxxx::ElemType::BDEM_VOID' for a void or error aggregate.

    const bdlmxxx::EnumerationDef *enumerationConstraint() const;
        // Return the address of the non-modifiable enumeration definition that
        // constrains the object referenced by this aggregate, or a null
        // pointer if this aggregate does not reference an enumeration object.

    int errorCode() const;
        // Return a negative error code describing the status of this object if
        // 'isError()' is 'true', or zero if 'isError()' is 'false'.  A set of
        // error code constants with names beginning with 'BCEM_' are
        // described in the 'bdlaggxxx_aggregate' component-level documentation.

    bsl::string errorMessage() const;
        // Return a string describing the error state of this object of
        // 'isError()' is 'true', or an empty string if 'isError()' is 'false'.
        // The contents of the string are intended to be human readable and
        // descriptive.  The exact format of the string may change at any time
        // and should not be relied on in a program (use 'errorCode()',
        // instead).

    int fieldById(AggregateRaw    *field,
                  ErrorAttributes *errorDescription,
                  int                   fieldId) const;
        // Load into the specified 'field' object the field within this
        // aggregate with the specified (zero-based) 'fieldId'.  Return 0 on
        // success, with no effect on the specified 'errorDescription';
        // otherwise, load into 'errorDescription' a description of the failure
        // and return a nonzero value.

    int fieldByIndex(AggregateRaw    *field,
                     ErrorAttributes *errorDescription,
                     int                   index) const;
        // Load into the specified 'field' object the field within this
        // aggregate with the specified (zero-based) 'index'.  Return 0 on
        // success, with no effect on the specified 'errorDescription';
        // otherwise, load into 'errorDescription' a description of the failure
        // and return a nonzero value.

    const bdlmxxx::FieldDef *fieldDef() const;
        // Return a pointer providing non-modifiable access to the field
        // definition for the object referenced by this aggregate, or null if
        // this object does not have a field definition.  An aggregate
        // constructed directly using a record definition will not have a field
        // definition, whereas a sub-aggregate returned by a field-access
        // function (e.g., 'operator[]' or 'field') will.  Note that if this
        // aggregate is an item within an array, table, or choice array, then
        // 'fieldDef()->elemType()' will return the *array* type, not the
        // *item* type (i.e., 'fieldDef()->elemType()' will not match
        // 'dataType()').

    int findUnambiguousChoice(AggregateRaw    *choiceObject,
                              ErrorAttributes *errorDescription,
                              const char           *caller = "") const;
        // Load into the specified 'choiceObject' the value of this aggregate
        // if it is a choice or choice array item, or else descend
        // (recursively) into any anonymous field of this aggregate looking for
        // an anonymous choice field; then load into 'choiceObject' the
        // unambiguous anonymous choice within this aggregate.  On failure --
        // i.e., multiple or no anonymous choices were found -- load into the
        // specified 'errorDescription' a description of the error
        // (incorporating the specified 'caller' description) and return a
        // nonzero value; otherwise, return 0 with no effect on
        // 'errorDescription'.

    int getField(
               AggregateRaw    *resultField,
               ErrorAttributes *errorDescription,
               bool                  makeNonNullFlag,
               FieldSelector    fieldOrIdx1,
               FieldSelector    fieldOrIdx2 = FieldSelector(),
               FieldSelector    fieldOrIdx3 = FieldSelector(),
               FieldSelector    fieldOrIdx4 = FieldSelector(),
               FieldSelector    fieldOrIdx5 = FieldSelector(),
               FieldSelector    fieldOrIdx6 = FieldSelector(),
               FieldSelector    fieldOrIdx7 = FieldSelector(),
               FieldSelector    fieldOrIdx8 = FieldSelector(),
               FieldSelector    fieldOrIdx9 = FieldSelector(),
               FieldSelector    fieldOrIdx10= FieldSelector()) const;
        // Navigate to the field within this aggregate reached through the
        // specified chain of two to ten 'fieldOrIdx' arguments, each of which
        // specifies a field name or array index.  If this aggregate references
        // a nillable array, the specified field is null, and the specified
        // 'makeNonNullFlag' is 'true', assign that field its default value;
        // otherwise leave the field unmodified.  On success, load into the
        // specified 'resultField' an uncounted reference to the specified
        // field and return 0 (without any effect on the specified
        // 'errorDescription').  Otherwise, load an error description into the
        // 'errorDescription' and return a nonzero value.  Note that an empty
        // string can be used for any of the 'fieldOrIdx' arguments to specify
        // the current selection within a choice object.  Note also that an
        // unused argument results in the construction of a null
        // 'FieldSelector', which is treated as the end of the argument
        // list.

    bool hasField(const char *fieldName) const;
        // Return 'true' if this aggregate contains a field having the
        // specified 'fieldName' and 'false' otherwise.

    bool hasFieldById(int fieldId) const;
        // Return 'true' if this aggregate contains a field having the
        // specified 'fieldId' and 'false' otherwise.

    bool hasFieldByIndex(int fieldIndex) const;
        // Return 'true' if this aggregate contains a field having the
        // specified 'fieldIndex' and 'false' otherwise.

    bool isError() const;
        // Return 'true' if this object was returned from a function that
        // detected an error.  If this function returns 'true', then
        // 'dataType()' will return 'bdlmxxx::ElemType::BDEM_VOID', 'errorCode()'
        // will return a non-zero value, and 'errorMessage()' will return a
        // non-empty string.

    bool isNull() const;
        // Return 'true' if the data referenced by this aggregate has a null
        // value, and 'false' otherwise.

    bool isNullable() const;
        // Return 'true' if the data referenced by this aggregate can be made
        // null, and 'false' otherwise.

    bool isVoid() const;
        // Return 'true' if 'dataType() == bdlmxxx::ElemType::BDEM_VOID'.

    int length() const;
        // Return the number of fields or items in the scalar array, list,
        // table, or choice array referred to by this aggregate or
        // 'BCEM_NOT_AN_ARRAY' for other data types.  Note that 0 will be
        // returned if this aggregate refers to a null array.

    void loadAsString(bsl::string *result) const;
        // Load into the specified 'result' string a text representation of the
        // value referenced by this aggregate, as returned by 'asString()'.

    int numSelections() const;
        // If this aggregate refers to choice or choice array item, return the
        // number of available selectors or 0 if 'true == isNull()'.  If this
        // aggregate refers to a list or row, look for an anonymous field
        // within the list or row and recursively look for a choice in the
        // anonymous field (if any), as per the "Anonymous Fields" section of
        // the 'bdlaggxxx_aggregate' component-level documentation.  Otherwise,
        // return an error code.

    const bdlmxxx::RecordDef *recordConstraint() const;
        // Return a pointer providing non-modifiable access to the record
        // definition that describes the structure of the object referenced by
        // this aggregate, or 0 if this aggregate references a scalar, array of
        // scalars, or unconstrained 'bdem' aggregate.

    const bdlmxxx::Schema *schema() const;
        // Return a pointer providing non-modifiable access to schema
        // referenced by this aggregate.

    int selection(AggregateRaw    *field,
                  ErrorAttributes *errorDescription) const;
        // If this aggregate refers to choice or choice array item, load into
        // the specified 'field' the sub-aggregate that refers to the
        // modifiable current selection, or a void aggregate if there is no
        // current selection.  If this aggregate refers to a list or row, look
        // for an anonymous field within the list or row and recursively look
        // for a choice in the anonymous field (if any), as per the "Anonymous
        // Fields" section of the 'bdlaggxxx_aggregate' component-level
        // documentation.  Return 0 on success, with no effect on the specified
        // 'errorDescription'; otherwise, load into 'errorDescription' a
        // description of the failure and return a nonzero value.  Note that if
        // this aggregate does not directly or indirectly refer to a choice or
        // choice array item) or 'true == isNull()' an error is returned.

    const char *selector() const;
        // Return the name for selector of the choice or choice array item
        // referenced by this aggregate or an empty string either if there is
        // no current selection, if the current selector does not have a text
        // name, if this aggregate does not refer to a choice or choice array
        // item, or 'true == isNull()'.  If this aggregate refers to a list or
        // row, look for an anonymous field within the list or row and
        // recursively look for a choice in the anonymous field (if any), as
        // per the "Anonymous Fields" section of the 'bdlaggxxx_aggregate'
        // component-level documentation.  The returned pointer is valid until
        // the the choice object is modified or destroyed.  Note that an error
        // condition is indistinguishable from one of the valid reasons for
        // returning an empty string.  The 'selection' or 'selectorId' methods
        // can be used to distinguish an error condition from a valid selector.

    int selectorId() const;
        // Return the ID for selector of the choice or choice array item
        // referenced by this aggregate, 'bdlmxxx::RecordDef::BDEM_NULL_FIELD_ID'
        // if there is no current selection, the current selection does not
        // have a numeric ID, this aggregate does not refer to a choice or
        // choice array item, or 'true == isNul()'.  If this aggregate refers
        // to a list or row, look for an anonymous field within the list or row
        // and recursively look for a choice in the anonymous field (if any),
        // as per the "Anonymous Fields" section of the 'bdlaggxxx_aggregate'
        // component-level documentation.  Note that the returned ID is the
        // selector's numeric identifier (assigned in its record definition
        // within the schema), not its positional index within the record
        // definition (see 'selectorIndex').

    int selectorIndex() const;
        // Return the index for selector of the choice or choice array item
        // referenced by this aggregate, -1 if there is no current selection,
        // or if 'true == isNull()' or an error code if this aggregate does not
        // refer to a choice or choice array item.  If this aggregate refers to
        // a list or row, look for an anonymous field within the list or row
        // and recursively look for a choice in the anonymous field (if any),
        // as per the "Anonymous Fields" section of the 'bdlaggxxx_aggregate'
        // component-level documentation.

    template <class STREAM>
    STREAM& bdexStreamOut(STREAM& stream, int version) const;
        // Write the value of the object referenced by this aggregate to the
        // specified output 'stream' using the specified 'version' format and
        // return a reference to the modifiable 'stream'.  If 'version' is not
        // supported, 'stream' is unmodified.  Note that 'version' is not
        // written to 'stream'.  This operation has the same effect as calling
        // 'bdexStreamOut' on the referenced object (e.g., if 'dataType()' is
        // 'LIST', call 'bdlmxxx::List::bdexStreamOut' on the referenced object).
        // See the 'bdex' package-level documentation for more information on
        // 'bdex' streaming of value-semantic types and containers.

                                  // Aspects

    bsl::ostream& print(bsl::ostream& stream,
                        int           level = 0,
                        int           spacesPerLevel = 4) const;
        // Format the referenced object to the specified output 'stream' at the
        // (absolute value of) the optionally specified indentation 'level' and
        // return a reference to 'stream'.  If 'level' is specified, optionally
        // specify 'spacesPerLevel', the number of spaces per indentation level
        // for this and all of its nested objects.  If 'level' is negative,
        // suppress indentation of the first line.  If 'spacesPerLevel' is
        // negative, format the entire output on one line.  If 'stream' is not
        // valid on entry, this operation has no effect.  For scalars and
        // arrays of scalars, this 'print' function delegates to the
        // appropriate printing mechanism for referenced object -- the
        // aggregate adds no additional text to the output.  (E.g., the result
        // of printing an aggregate that references a string is
        // indistinguishable from the result of printing the string directly.)
        // For list, row, choice, choice array item, table, and choice array,
        // this print function prepend's each field with the name of the field.

    // REFERENCED-VALUE MANIPULATORS
    template <class VALTYPE>
    int insertItem(AggregateRaw    *newItem,
                   ErrorAttributes *errorDescription,
                   int                   index,
                   const VALTYPE&        value) const;
        // Insert a copy of the specified 'value' before the specified 'index'
        // in the scalar array, table, or choice array referenced by this
        // aggregate.  Return 0 on success and load a reference to the new item
        // into the specified 'newItem'; otherwise, return a nonzero value and
        // load a description into the specified 'errorDescription'.

    int insertItems(ErrorAttributes *errorDescription,
                    int                   index,
                    int                   numItems) const;
        // Insert 'numItems' new elements before the specified 'index' in the
        // scalar array, table, or choice array referenced by this aggregate.
        // The new items are set to their default values, as specified in the
        // schema (recursively for the fields within inserted table rows).  If
        // '0 == pos' and 'isNull()', set the value referenced by this object
        // to its default value (i.e., make it non-null).  Return 0 on success,
        // with no effect on the specified 'errorDescription'; otherwise, load
        // into 'errorDescription' a description of the failure and return a
        // nonzero value.

    int insertNullItem(AggregateRaw    *newItem,
                       ErrorAttributes *errorDescription,
                       int                   index) const;
        // Insert a new element before the specified 'index' in the scalar
        // array, table, or choice array referenced by this aggregate.  The new
        // value is null and it is *not* set to its default value as with the
        // 'insertItems' method.  Return 0 on success and load a reference to
        // the new item into the specified 'newItem'; otherwise, return a
        // nonzero value and load a description into the specified
        // 'errorDescription'.

    int insertNullItems(ErrorAttributes *errorDescription,
                        int                   index,
                        int                   numItems) const;
        // Insert 'numItems' new elements before the specified 'index' in the
        // scalar array, table, or choice array referenced by this aggregate.
        // The new items are set to the null values, as specified in the schema
        // (recursively for the fields within inserted table rows).  Return 0
        // on success, with no effect on the specified 'errorDescription';
        // otherwise, load into 'errorDescription' a description of the failure
        // and return a nonzero value.

    void makeNull() const;
        // Set the object referenced by this aggregate to null.  If the object
        // is null before the call, then do nothing.  The object's type and
        // constraints are unchanged.

    int makeSelection(AggregateRaw    *result,
                      ErrorAttributes *errorDescription,
                      const char           *newSelector) const;
        // Change the selector in the choice object referenced by this
        // aggregate to the one specified by 'newSelector', first making this
        // choice non-null if it is currently null.  The newly selected
        // sub-object is initialized to its default value.  Return 0 on
        // success, loading a sub-aggregate referring to the modifiable
        // selection into the specified 'field' object; otherwise, return a
        // nonzero error and populate the specified 'errorDescription' with no
        // effect on this object.

    template <class VALTYPE>
    int makeSelection(AggregateRaw    *result,
                      ErrorAttributes *errorDescription,
                      const char           *newSelector,
                      const VALTYPE&        value) const;
        // Change the selector in the choice object referenced by this
        // aggregate to the one specified by 'newSelector', first making this
        // choice non-null if it is currently null and initializing the
        // sub-aggregate to the specified 'value' after appropriate conversions
        // (see "Extended Type Conversions" in the 'Aggregate'
        // component-level documentation).  Return 0 on success, loading a
        // sub-aggregate referring to the modifiable selection into the
        // specified 'field' object; otherwise, return a nonzero error and
        // populate the specified 'errorDescription' with no effect on this
        // object.

    int makeSelectionById(AggregateRaw    *field,
                          ErrorAttributes *errorDescription,
                          int                   newSelectorId) const;
        // Change the selector in the choice object referenced by this
        // aggregate to the one specified by 'newSelectorId', first making this
        // choice non-null if it is currently null.  The newly selected
        // sub-object is initialized to its default value.  Return 0 on
        // success, loading a sub-aggregate referring to the modifiable
        // selection into the specified 'field' object; otherwise, return a
        // nonzero error and populate the specified 'errorDescription' with no
        // effect on this object.

    template <class VALTYPE>
    int makeSelectionById(AggregateRaw    *result,
                          ErrorAttributes *errorDescription,
                          int                   newSelectorId,
                          const VALTYPE&        value) const;
        // Change the selector in the choice object referenced by this
        // aggregate to the one specified by 'newSelectorId', first making this
        // choice non-null if it is currently null and initializing the
        // sub-aggregate to the specified 'value' after appropriate conversions
        // (see "Extended Type Conversions" in the 'Aggregate'
        // component-level documentation).  Return 0 on success, loading a
        // sub-aggregate referring to the modifiable selection into the
        // specified 'field' object; otherwise, return a nonzero error and
        // populate the specified 'errorDescription' with no effect on this
        // object.

    int makeSelectionByIndex(AggregateRaw    *field,
                             ErrorAttributes *errorDescription,
                             int                   index) const;
        // Change the selector in the choice object referenced by this
        // aggregate to the one specified by 'index', first making this choice
        // non-null if it is currently null.  The newly selected sub-object is
        // initialized to its default value.  Return 0 on success, loading a
        // sub-aggregate referring to the modifiable selection into the
        // specified 'field' object; otherwise, return a nonzero error and
        // populate the specified 'errorDescription' with no effect on this
        // object.

    template <class VALTYPE>
    int makeSelectionByIndex(AggregateRaw    *field,
                             ErrorAttributes *errorDescription,
                             int                   index,
                             const VALTYPE&        value) const;
        // Change the selector in the choice object referenced by this
        // aggregate to the one specified by 'index', first making this choice
        // non-null if it is currently null and initializing the sub-aggregate
        // to the specified 'value' after appropriate conversions (see
        // "Extended Type Conversions" in the 'Aggregate' component-level
        // documentation).  Return 0 on success, loading a sub-aggregate
        // referring to the modifiable selection into the specified 'field'
        // object; otherwise, return a nonzero error and populate the specified
        // 'errorDescription' with no effect on this object.

    void makeValue() const;
        // Construct a value for the nullable object referenced by this
        // aggregate by assigning it the default value specified in the schema,
        // or (for 'LIST' aggregates) by giving each scalar fielad its default
        // value and recursively constructing the default value for each
        // non-nullable sublist.  Do nothing if this aggregate is not null.
        // Note that if this aggregate refers to a list with an empty record
        // definition, then the entire list will still be null after this
        // function is called.

    int removeItems(ErrorAttributes *errorDescription,
                    int                   index,
                    int                   numItems) const;
        // Remove the specified 'numItems' items starting at the specified
        // 'index' in the scalar array, choice array, or table referenced by
        // this aggregate.  Return 0 on success, with no effect on the
        // specified 'errorDescription'; otherwise, load into
        // 'errorDescription' a description of the failure and return a nonzero
        // value.

    int reserveRaw(ErrorAttributes *errorDescription,
                   bsl::size_t           numItems) const;
        // Reserve sufficient memory for at least the specified 'numItems' if
        // this aggregate references a scalar or choice array, or reserve
        // sufficient memory for at least the footprint of 'numItems' rows, if
        // this aggregate references a table.  In the latter case, additional
        // memory needed to initialize a new row upon insertion, *may* or may
        // *not* be reserved depending on the allocation mode.  In the future,
        // this method may strengthen its guarantee such that no additional
        // allocation will occur upon row insertion (regardless of allocation
        // mode) unless a data element itself allocates memory.  Return 0 on
        // success or a nonzero value with a description loaded into the
        // specified 'errorDescription' on failure.

    int resize(ErrorAttributes *errorDescription,
               bsl::size_t           newSize) const;
        // Grow or shrink the scalar array, table, or choice array referenced
        // by this aggregate to the specified 'newSize' number of items, first
        // making the referenced value non-null if currently null.  If
        // 'newSize > length()', grow the array or table by inserting new items
        // at the end and set them to their default value, as specified in the
        // schema (recursively for the fields within inserted table rows).  If
        // 'newSize < length()' shrink the array or table by discarding excess
        // items at the end.  Return 0 on success, with no effect on the
        // specified 'errorDescription'; otherwise, load a description of the
        // error into 'errorDescription' and return a nonzero value.

    template <class VALTYPE>
    int setField(AggregateRaw    *field,
                 ErrorAttributes *errorDescription,
                 FieldSelector    fieldSelector1,
                 const VALTYPE&        value) const;
    template <class VALTYPE>
    int setField(AggregateRaw    *field,
                 ErrorAttributes *errorDescription,
                 FieldSelector    fieldSelector1,
                 FieldSelector    fieldSelector2,
                 const VALTYPE&        value) const;
    template <class VALTYPE>
    int setField(AggregateRaw    *field,
                 ErrorAttributes *errorDescription,
                 FieldSelector    fieldSelector1,
                 FieldSelector    fieldSelector2,
                 FieldSelector    fieldSelector3,
                 const VALTYPE&        value) const;
    template <class VALTYPE>
    int setField(AggregateRaw    *field,
                 ErrorAttributes *errorDescription,
                 FieldSelector    fieldSelector1,
                 FieldSelector    fieldSelector2,
                 FieldSelector    fieldSelector3,
                 FieldSelector    fieldSelector4,
                 const VALTYPE&        value) const;
    template <class VALTYPE>
    int setField(AggregateRaw    *field,
                 ErrorAttributes *errorDescription,
                 FieldSelector    fieldSelector1,
                 FieldSelector    fieldSelector2,
                 FieldSelector    fieldSelector3,
                 FieldSelector    fieldSelector4,
                 FieldSelector    fieldSelector5,
                 const VALTYPE&        value) const;
    template <class VALTYPE>
    int setField(AggregateRaw    *field,
                 ErrorAttributes *errorDescription,
                 FieldSelector    fieldSelector1,
                 FieldSelector    fieldSelector2,
                 FieldSelector    fieldSelector3,
                 FieldSelector    fieldSelector4,
                 FieldSelector    fieldSelector5,
                 FieldSelector    fieldSelector6,
                 const VALTYPE&        value) const;
    template <class VALTYPE>
    int setField(AggregateRaw    *field,
                 ErrorAttributes *errorDescription,
                 FieldSelector    fieldSelector1,
                 FieldSelector    fieldSelector2,
                 FieldSelector    fieldSelector3,
                 FieldSelector    fieldSelector4,
                 FieldSelector    fieldSelector5,
                 FieldSelector    fieldSelector6,
                 FieldSelector    fieldSelector7,
                 const VALTYPE&        value) const;
    template <class VALTYPE>
    int setField(AggregateRaw    *field,
                 ErrorAttributes *errorDescription,
                 FieldSelector    fieldSelector1,
                 FieldSelector    fieldSelector2,
                 FieldSelector    fieldSelector3,
                 FieldSelector    fieldSelector4,
                 FieldSelector    fieldSelector5,
                 FieldSelector    fieldSelector6,
                 FieldSelector    fieldSelector7,
                 FieldSelector    fieldSelector8,
                 const VALTYPE&        value) const;
    template <class VALTYPE>
    int setField(AggregateRaw    *field,
                 ErrorAttributes *errorDescription,
                 FieldSelector    fieldSelector1,
                 FieldSelector    fieldSelector2,
                 FieldSelector    fieldSelector3,
                 FieldSelector    fieldSelector4,
                 FieldSelector    fieldSelector5,
                 FieldSelector    fieldSelector6,
                 FieldSelector    fieldSelector7,
                 FieldSelector    fieldSelector8,
                 FieldSelector    fieldSelector9,
                 const VALTYPE&        value) const;
    template <class VALTYPE>
    int setField(AggregateRaw    *field,
                 ErrorAttributes *errorDescription,
                 FieldSelector    fieldSelector1,
                 FieldSelector    fieldSelector2,
                 FieldSelector    fieldSelector3,
                 FieldSelector    fieldSelector4,
                 FieldSelector    fieldSelector5,
                 FieldSelector    fieldSelector6,
                 FieldSelector    fieldSelector7,
                 FieldSelector    fieldSelector8,
                 FieldSelector    fieldSelector9,
                 FieldSelector    fieldSelector10,
                 const VALTYPE&        value) const;
        // Navigate to the field within this aggregate reached through the
        // specified chain of one to ten 'fieldSelector' arguments, each of
        // which specifies a field name or array index, then set that field to
        // the specified 'value', resetting its nullness flag if
        // 'field.isNull()' is 'true', after appropriate conversions (see
        // "Extended Type Conversions" in the 'Aggregate' component-level
        // documentation).  An empty string can be used for any of the
        // 'fieldSelector' arguments to specify the current selection within a
        // choice object.  If value is null then make the field null.  Note
        // that if any field in the chain of fields is null then an error is
        // returned.  Return 0 on success and load a reference to the
        // sub-aggregate into the specified 'field'; otherwise, return a
        // nonzero value and load a description into the specified
        // 'errorDescription'.

    template <class TYPE>
    int setValue(ErrorAttributes *errorDescription,
                 const TYPE&           value) const;
        // Set the value referenced by this aggregate to the specified 'value',
        // converting the specified 'value' as necessary.  If 'value' is null
        // then make this aggregate null.  Return 0 on success, with no effect
        // on the specified 'errorDescription'; otherwise, load into
        // 'errorDescription' a description of the failure and return a nonzero
        // value.

    // REFERENCED-VALUE ACCESSORS
    int capacityRaw(ErrorAttributes *errorDescription,
                    bsl::size_t          *capacity) const;
        // Load, in the specified 'capacity', the total number of items for
        // which sufficient memory is currently allocated if this aggregate
        // references a scalar or choice array, or load the total number of row
        // footprints for which sufficient memory is currently allocated, if
        // this aggregate references a table.  In the latter case, inserting
        // rows that do not exceed this capacity *may* or may *not* result in
        // additional allocations depending on the allocation mode, and whether
        // any row data element itself allocates memory (see the 'reserveRaw'
        // method).  Return 0 on success or a nonzero value with a description
        // loaded into the specified 'errorDescription' on failure.
};

// FREE OPERATORS
inline
bsl::ostream& operator<<(bsl::ostream& stream, const AggregateRaw& rhs);

template <>
bsl::string bdlaggxxx::AggregateRaw::convertScalar<bsl::string>() const;
    // Specialization of 'convertScalar<TOTYPE>' for 'TOTYPE = bsl::string'.

template <>
inline
int bdlaggxxx::AggregateRaw::setValue<bdlaggxxx::AggregateRaw>(
    bdlaggxxx::ErrorAttributes     *errorDescription,
    const bdlaggxxx::AggregateRaw&  value) const;
    // Specialization of 'setValue<VALUETYPE>' for 'VALUETYPE = AggregateRaw'

                   // =============================
                   // struct AggregateRaw_Util
                   // =============================

struct AggregateRaw_Util {
    // This 'struct' provides a namespace for a set of utility methods
    // for working with the 'bdem' data representations of this component.

    static
    const char *enumerationName(const bdlmxxx::EnumerationDef *enumDef);
        // Return the name of the specified 'enumDef', or "(anonymous)" if
        // 'enumDef' has no name, or "(unconstrained)" if 'enumDef' is null.

    static bool isConformant(const bdlmxxx::ConstElemRef *object,
                             const bdlmxxx::RecordDef    *recordDef);
    static bool isConformant(const bdlmxxx::Row       *object,
                             const bdlmxxx::RecordDef *recordDef);
    static bool isConformant(const bdlmxxx::List      *object,
                             const bdlmxxx::RecordDef *recordDef);
    static bool isConformant(const bdlmxxx::Table     *object,
                             const bdlmxxx::RecordDef *recordDef);
    static bool isConformant(const bdlmxxx::ChoiceArrayItem *object,
                             const bdlmxxx::RecordDef       *recordDef);
    static bool isConformant(const bdlmxxx::Choice    *object,
                             const bdlmxxx::RecordDef *recordDef);
    static bool isConformant(const bdlmxxx::ChoiceArray *object,
                             const bdlmxxx::RecordDef   *recordDef);
        // Return 'true' if the specified 'object' is conformant with the
        // specified 'recordDef' as defined by 'bdlmxxx::SchemaAggregateUtil'.

    static bool isConformant(const void           *object,
                             const bdlmxxx::RecordDef *recordDef);
        // Return 'true' if the specified 'recordDef' is null, and 'false'
        // otherwise.

    static bool isConformant(const void           *object,
                             bdlmxxx::ElemType::Type   type,
                             const bdlmxxx::RecordDef *recordDef);
        // Return 'true' if the specified 'object' of the specified 'type' is
        // of aggregate type conforms to the specified 'recordDef' or
        // 'recordDef' is null.  Return 'false' if 'object' is of non-aggregate
        // type and 'recordDef' is non-null.  The behavior is undefined unless
        // 'object' is non-null.

    static
    const char *recordName(const bdlmxxx::RecordDef *recordDef);
        // Return the name of the specified 'recordDef', or "(anonymous)" if
        // 'recordDef' has no name, or "(unconstrained)" if 'recordDef' is
        // null.

    template <class VISITOR>
    static
    int visitArray(void                *array,
                   bdlmxxx::ElemType::Type  arrayType,
                   VISITOR             *visitorPtr);
        // Cast the specified 'array' to a pointer to the vector type indicated
        // by the specified 'arrayType' and load into it the value returned by:
        //..
        //    (*visitorPtr)(array)
        //..
        // The parameterized 'VISITOR' type must have 'int operator()(TYPE*)'
        // overloaded for each array 'TYPE' that may be passed to this
        // function.  The '*visitorPtr' object may store state and, if
        // non-'const', that state may be modified by the visitor.  Return 0
        // on success, or 'ErrorCode::BCEM_NOT_AN_ARRAY' if
        // 'arrayType' is not an array of scalar 'bdem' type.  The behavior
        // is undefined unless 'array' is of 'arrayType'.
};

// ============================================================================
//                      'bdeat' INTEGRATION
// ============================================================================

                // ==================================
                // struct AggregateRaw_BdeatInfo
                // ==================================

struct AggregateRaw_BdeatInfo {
    // This 'struct' provides attribute information used when accessing or
    // manipulating the fields of 'AggregateRaw' within the 'bdeat'
    // framework.

    // DATA
    const bdlmxxx::RecordDef *d_record_p;
    int                   d_fieldIndex;
    mutable char          d_anonFieldNameBuffer[32];  // large enough for
                                                      // "FIELD_<INT_MAX>"
    mutable const char   *d_name_p;                   // lazily evaluated
    mutable int           d_nameLength;               // lazily evaluated
    mutable const char   *d_annotation_p;             // lazily evaluated
    int                   d_id;
    int                   d_formattingMode;
    mutable bool          d_areLazyAttributesSet;     // 'true' if 'd_name_p',
                                                      // etc. have been set

    bool                  d_isNullable;
    bool                  d_hasDefaultValue;

  private:
    // PRIVATE ACCESSORS
    void setLazyAttributes() const;
        // Set the attribute values of this object.

  public:
    // CREATORS
    AggregateRaw_BdeatInfo(const bdlmxxx::RecordDef *record,
                                int                   fieldIndex);
        // Create an info structure representing the field having the specified
        // 'fieldIndex' in the specified 'record'.

    // MANIPULATORS
    const char *& annotation();
        // Return a reference providing modifiable access to the 'annotation'
        // attribute of this object.

    int& formattingMode();
        // Return a reference providing modifiable access to the
        // 'formattingMode' attribute of this object.

    int& id();
        // Return a reference providing modifiable access to the 'id' attribute
        // of this object.

    const char *& name();
        // Return a reference providing modifiable access to the 'name'
        // attribute of this object.

    int& nameLength();
        // Return a reference providing modifiable access to the 'nameLength'
        // attribute of this object.

    bool& hasDefaultValue();
        // Return a reference providing modifiable access to the
        // 'hasDefaultValue' attribute of this object.

    bool& isNullable();
        // Return a reference providing modifiable access to the 'isNullable'
        // attribute of this object.

    // ACCESSORS
    const char *annotation() const;
        // Return a reference providing non-modifiable access to the
        // 'annotation' attribute of this object.

    int formattingMode() const;
        // Return a reference providing non-modifiable access to the
        // 'formattingMode' attribute of this object.

    int id() const;
        // Return a reference providing non-modifiable access to the 'id'
        // attribute of this object.

    const char *name() const;
        // Return a reference providing non-modifiable access to the 'id'
        // attribute of this object.

    int nameLength() const;
        // Return a reference providing non-modifiable access to the
        // 'nameLength' attribute of this object.

    bool hasDefaultValue() const;
        // Return a reference providing non-modifiable access to the
        // 'hasDefaultValue' attribute of this object.

    bool isNullable() const;
        // Return a reference providing non-modifiable access to the
        // 'isNullable' attribute of this object.
};

                // ==================================
                // struct AggregateRaw_BdeatUtil
                // ==================================

struct AggregateRaw_BdeatUtil {
    // This 'struct' provides a namespace for utility functions related to
    // 'bdeat' integration.

    // TYPES
    struct NullableAdapter {
        AggregateRaw *d_element_p;
    };

    // CLASS METHODS
    static
    int fieldIndexFromName(const bdlmxxx::RecordDef&  record,
                           const char            *name,
                           int                    nameLength);
        // Return the field index in the specified 'record' corresponding to
        // the specified 'name' of the specified 'nameLength' or a non-zero
        // value if no such field exists.

    template <class MANIPULATOR>
    static
    int manipulateField(AggregateRaw *parent,
                        MANIPULATOR&       manipulator,
                        int                fieldIndex);
        // Use the specified 'manipulator' to change the value of the field
        // corresponding to the specified 'fieldIndex' in the specified
        // 'parent'.  Return 0 on success and a non-zero value otherwise.

    template <class ACCESSOR>
    static
    int accessField(const AggregateRaw& parent,
                    ACCESSOR&                accessor,
                    int                      fieldIndex);
        // Use the specified 'accessor' to access the value of the field
        // corresponding to the specified 'fieldIndex' in the specified
        // 'parent'.  Return 0 on success and a non-zero value otherwise.
};

// ============================================================================
//                      INLINE AND TEMPLATE FUNCTION DEFINITIONS
// ============================================================================

                           //------------------------
                           // class AggregateRaw
                           //------------------------

// PRIVATE ACCESSORS
template <class TYPE>
int AggregateRaw::assignToNillableScalarArrayImp(const TYPE& value) const
{
    bdlmxxx::ElemType::Type srcType = value.type();

    // Check conformance of value against this aggregate.

    if (bdlmxxx::ElemType::BDEM_TABLE == srcType) {
        return assignToNillableScalarArray(value.theTable());         // RETURN
    }

    bdlmxxx::ElemType::Type baseType = bdlmxxx::ElemType::fromArrayType(srcType);
    if (!bdlmxxx::ElemType::isScalarType(baseType)
      || baseType != recordConstraint()->field(0).elemType()) {
        return ErrorCode::BCEM_NON_CONFORMANT;                        // RETURN
    }

    if (value.isNull()) {
        makeNull();
        return 0;                                                     // RETURN
    }

    AggregateRaw_ElemDataFetcher fetcher(value);
    void *srcData = fetcher.d_data_p;
    AggregateRaw_ArraySizer sizer;
    const int  length  = AggregateRaw_Util::visitArray(srcData,
                                                            srcType,
                                                            &sizer);

    ErrorAttributes error;
    if (0 != resize(&error, length)) {
        return error.code();                                          // RETURN
    }

    bdlmxxx::Table            *dstTable     = (bdlmxxx::Table *)d_value_p;
    const bdlmxxx::Descriptor *baseTypeDesc =
                                  bdlmxxx::ElemAttrLookup::lookupTable()[baseType];

    for (int i = 0; i < length; ++i) {
        AggregateRaw_ArrayIndexer indexer(i);
        AggregateRaw_Util::visitArray(srcData, srcType, &indexer);
        baseTypeDesc->assign(dstTable->theModifiableRow(i)[0].data(),
                             indexer.data());
    }
    return 0;
}

template <class TYPE>
int AggregateRaw::assignToNillableScalarArray(const TYPE&) const
{
    BSLS_ASSERT_OPT("Invalid Type for Nillable Type" && 0);
    return -1;
}

template <>
inline
int AggregateRaw::assignToNillableScalarArray(
                                                 const bdlmxxx::Table& value) const
{
    if (!AggregateRaw_Util::isConformant(&value, recordConstraint())) {
        return ErrorCode::BCEM_NON_CONFORMANT;                        // RETURN
    }

    *(bdlmxxx::Table *)d_value_p = value;
    return 0;
}

template <>
inline
int AggregateRaw::assignToNillableScalarArray(
                                          const bdlmxxx::ConstElemRef& value) const
{
    return assignToNillableScalarArrayImp(value);
}

template <>
inline
int AggregateRaw::assignToNillableScalarArray(
                                               const bdlmxxx::ElemRef& value) const
{
    return assignToNillableScalarArrayImp(value);
}

template <class TYPE>
int AggregateRaw::assignToNillableScalarArray(
                                          const bsl::vector<TYPE>& value) const
{
    bdlmxxx::ElemType::Type baseType  =
                        (bdlmxxx::ElemType::Type) bdlmxxx::SelectBdemType<TYPE>::VALUE;

    if (baseType != recordConstraint()->field(0).elemType()) {
        return ErrorCode::BCEM_NON_CONFORMANT;                        // RETURN
    }

    const int length = static_cast<int>(value.size());
    ErrorAttributes errorDescription;
    if (0 != resize(&errorDescription, length)) {
        return errorDescription.code();                               // RETURN
    }
    if (0 == length) {
        return 0;                                                     // RETURN
    }

    bdlmxxx::Table            *dstTable     = (bdlmxxx::Table *)d_value_p;
    const bdlmxxx::Descriptor *baseTypeDesc =
                                  bdlmxxx::ElemAttrLookup::lookupTable()[baseType];
    typename bsl::vector<TYPE>::const_iterator iter = value.begin();
    for (int i = 0; i < length; ++i, ++iter) {
        baseTypeDesc->assign(dstTable->theModifiableRow(i)[0].data(),
                             (const void *) &(*iter));
    }
    return 0;
}

template <class VALUETYPE>
inline
int AggregateRaw::toEnum(ErrorAttributes *errorDescription,
                              const VALUETYPE&      value) const
{
    static const int IS_DIRECT =
              bslmf::IsConvertible<VALUETYPE, const char*>::VALUE
           || bslmf::IsConvertible<VALUETYPE, bsl::string>::VALUE
           || bslmf::IsConvertible<VALUETYPE, const bdlmxxx::ConstElemRef&>::VALUE;

    return toEnum(errorDescription, value, bslmf::MetaInt<IS_DIRECT>());
}

template <class VALUETYPE>
int AggregateRaw::toEnum(ErrorAttributes *errorDescription,
                              const VALUETYPE&      value,
                              bslmf::MetaInt<0>     direct) const
{
    int intVal;
    if (0 != bdlmxxx::Convert::convert(&intVal, value)) {
        bsl::ostringstream oss;
        oss << "Invalid conversion from \""
            << bdlmxxx::ElemType::toAscii(getBdemType(value))
            << "\" to enumeration \""
            << AggregateRaw_Util::enumerationName(enumerationConstraint())
            << '\"';
        errorDescription->setDescription(oss.str());
        errorDescription->setCode(ErrorCode::BCEM_BAD_CONVERSION);
        return -1;                                                    // RETURN
    }
    return toEnum(errorDescription, intVal, direct);
}

inline
int AggregateRaw::toEnum(ErrorAttributes *errorDescription,
                              const bsl::string&    value,
                              bslmf::MetaInt<1>     direct) const
{
    return toEnum(errorDescription, value.c_str(), direct);
}

// CLASS METHODS
inline
int AggregateRaw::maxSupportedBdexVersion()
{
    return 3;
}

template <class TYPE>
inline
bdlmxxx::ElemType::Type AggregateRaw::getBdemType(const TYPE&)
{
    return (bdlmxxx::ElemType::Type) bdlmxxx::SelectBdemType<TYPE>::VALUE;
}

template <>
inline
bdlmxxx::ElemType::Type AggregateRaw::getBdemType(
                                                const bdlmxxx::ConstElemRef& value)
{
    return value.type();
}

template <>
inline
bdlmxxx::ElemType::Type AggregateRaw::getBdemType(const bdlmxxx::ElemRef& value)
{
    return value.type();
}

template <>
inline
bdlmxxx::ElemType::Type AggregateRaw::getBdemType(
                                                const AggregateRaw& value)
{
    return value.dataType();
}

// CREATORS
inline
AggregateRaw::~AggregateRaw()
{
    // Assert invariants (see member variable description in class definition)
    if (d_dataType != bdlmxxx::ElemType::BDEM_VOID) {
        BSLS_ASSERT(d_schema_p || (!d_recordDef_p && !d_fieldDef_p));

        BSLS_ASSERT(!d_schema_p || (d_recordDef_p || d_fieldDef_p));

        BSLS_ASSERT(! d_recordDef_p || &d_recordDef_p->schema() == d_schema_p);

        // Cannot easily test that 'd_fieldDef_p' is within 'd_schema_p'
        BSLS_ASSERT(! d_fieldDef_p
                    || d_fieldDef_p->elemType() == d_dataType
                    || d_fieldDef_p->elemType() ==
                            bdlmxxx::ElemType::toArrayType(d_dataType));
        BSLS_ASSERT(! d_fieldDef_p
                    || d_recordDef_p  == d_fieldDef_p->recordConstraint());
    }
}

// MANIPULATORS
inline
void AggregateRaw::setData(void *data)
{
    BSLS_ASSERT_SAFE(data);

    d_value_p = data;
}

inline
void AggregateRaw::setDataType(bdlmxxx::ElemType::Type dataType)
{
    d_dataType = dataType;
}

inline
void AggregateRaw::setFieldDef(const bdlmxxx::FieldDef *fieldDef)
{
    BSLS_ASSERT_SAFE(fieldDef);

    d_fieldDef_p = fieldDef;
}

inline
void AggregateRaw::setRecordDef(const bdlmxxx::RecordDef *recordDef)
{
    BSLS_ASSERT_SAFE(recordDef);

    d_recordDef_p = recordDef;
}

inline
void AggregateRaw::setSchema(const bdlmxxx::Schema *schema)
{
    BSLS_ASSERT_SAFE(schema);

    d_schema_p = schema;
}

inline
void AggregateRaw::setTopLevelAggregateNullness(int *nullnessFlag)
{
    BSLS_ASSERT_SAFE(nullnessFlag);

    d_isTopLevelAggregateNull_p = nullnessFlag;
}

template <class STREAM>
STREAM& AggregateRaw::bdexStreamIn(STREAM& stream, int version) const
{
    switch (version) {
      case 3: {
        char isNull;
        stream.getUint8(isNull);
        if (!stream) {
            return stream;                                            // RETURN
        }

        if (isNull) {
            if (bdlmxxx::ElemType::BDEM_VOID != d_dataType) {
                makeNull();
            }
            return stream;                                            // RETURN
        }
      }                                                         // FALL THROUGH
      case 2:                                                   // FALL THROUGH
      case 1: {
          switch (dataType()) {
          case bdlmxxx::ElemType::BDEM_VOID: {
            return stream;                                            // RETURN
          }
          case bdlmxxx::ElemType::BDEM_ROW: {
              bdlmxxx::Row *row = (bdlmxxx::Row*)d_value_p;
              return row->bdexStreamIn(stream, version);              // RETURN
          }
          case bdlmxxx::ElemType::BDEM_CHOICE_ARRAY_ITEM: {
            bdlmxxx::ChoiceArrayItem *item = (bdlmxxx::ChoiceArrayItem*)d_value_p;
            return item->bdexStreamIn(stream, version);               // RETURN
          }
          default: {
          } break;
        }
      } break;
      default: {
        stream.invalidate();
        return stream;                                                // RETURN
      }
    }

    const bdlmxxx::DescriptorStreamIn<STREAM> *strmAttrLookup =
                            bdlmxxx::ElemStreamInAttrLookup<STREAM>::lookupTable();

    const bdlmxxx::Descriptor *const *elemAttrLookup =
                                            bdlmxxx::ElemAttrLookup::lookupTable();

    version = bdlmxxx::ElemType::isAggregateType(d_dataType) ? version : 1;

    strmAttrLookup[dataType()].streamIn((void*)d_value_p,
                                        stream,
                                        version,
                                        strmAttrLookup,
                                        elemAttrLookup);

    if (recordConstraint() && !AggregateRaw_Util::isConformant(
                                                              d_value_p,
                                                              d_dataType,
                                                              d_recordDef_p)) {
        makeNull();  // discard data
        stream.invalidate();
    }

    return stream;
}

// ACCESSORS
inline
int AggregateRaw::getArrayItem(AggregateRaw    *item,
                                    ErrorAttributes *errorDescription,
                                    int                   index) const
{
    *item = *this;
    return item->descendIntoArrayItem(errorDescription, index, false);
}

template <class TOTYPE>
TOTYPE AggregateRaw::convertScalar() const
{
    TOTYPE result;
    int    status = -1;
    const bdlmxxx::EnumerationDef *enumDef = enumerationConstraint();
    if (enumDef) {
        int enumId;
        if (bdlmxxx::ElemType::BDEM_INT == d_dataType) {
            enumId = *static_cast<int*>(d_value_p);
            status = 0;
        }
        else if (bdlmxxx::ElemType::BDEM_STRING == d_dataType) {
            const bsl::string& enumName =
                                         *static_cast<bsl::string*>(d_value_p);
            enumId = enumDef->lookupId(enumName.c_str());
            if (bdltuxxx::Unset<int>::unsetValue() != enumId
             || bdltuxxx::Unset<bsl::string>::isUnset(enumName)) {
                status = 0;
            }
        }

        if (0 == status) {
            status = bdlmxxx::Convert::convert(&result, enumId);
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

        return bdltuxxx::Unset<TOTYPE>::unsetValue();                 // RETURN
    }

    return result;
}

inline
const void *AggregateRaw::data() const
{
    return d_value_p;
}

inline
bdlmxxx::ElemType::Type AggregateRaw::dataType() const
{
    return d_dataType;
}

inline
const bdlmxxx::EnumerationDef *AggregateRaw::enumerationConstraint() const
{
    return d_fieldDef_p ? d_fieldDef_p->enumerationConstraint() : 0;
}

inline
const bdlmxxx::FieldDef *AggregateRaw::fieldDef() const
{
    return d_fieldDef_p;
}

inline
bool AggregateRaw::isError() const
{
    // An error state is detectable as a void object with a non-null 'd_value'.
    // The 'd_value' holds the error record.

    return bdlmxxx::ElemType::BDEM_VOID == d_dataType && d_value_p;
}

inline
bool AggregateRaw::isVoid() const
{
    return bdlmxxx::ElemType::BDEM_VOID == d_dataType;
}

inline
const bdlmxxx::RecordDef *AggregateRaw::recordConstraint() const
{
    return d_recordDef_p;
}

inline
const bdlmxxx::Schema *AggregateRaw::schema() const
{
    return d_schema_p;
}

inline
int AggregateRaw::selection(AggregateRaw    *field,
                                 ErrorAttributes *errorDescription) const
{
    int rc = findUnambiguousChoice(field, errorDescription, "selection");
    if (!rc) {
        return field->fieldByIndex(field,
                                   errorDescription,
                                   selectorIndex());                  // RETURN
    }
    return rc;
}

template <class STREAM>
STREAM& AggregateRaw::bdexStreamOut(STREAM& stream, int version) const
{
    switch (version) {
      case 3: {
        if (isNull()) {
            stream.putUint8(1);
            return stream;                                            // RETURN
        }
        else {
            stream.putUint8(0);
        }
      }                                                         // FALL THROUGH
      case 2:                                                   // FALL THROUGH
      case 1: {
        switch (d_dataType) {
          case bdlmxxx::ElemType::BDEM_VOID: {
            return stream;                                            // RETURN
          }
          case bdlmxxx::ElemType::BDEM_ROW: {
            const bdlmxxx::Row *row = static_cast<const bdlmxxx::Row *>(d_value_p);
            return row->bdexStreamOut(stream, version);               // RETURN
          }
          case bdlmxxx::ElemType::BDEM_CHOICE_ARRAY_ITEM: {
            const bdlmxxx::ChoiceArrayItem *item =
                          static_cast<const bdlmxxx::ChoiceArrayItem *>(d_value_p);
            return item->bdexStreamOut(stream, version);              // RETURN
          }
          default: {
          } break;
        }
      } break;
      default: {
        stream.invalidate();
        return stream;                                                // RETURN
      }
    }

    const bdlmxxx::DescriptorStreamOut<STREAM> *strmAttrLookup =
                           bdlmxxx::ElemStreamOutAttrLookup<STREAM>::lookupTable();

    const bdlmxxx::DescriptorStreamOut<STREAM> *elemStrmAttr =
                                                   &strmAttrLookup[d_dataType];

    version = bdlmxxx::ElemType::isAggregateType(d_dataType) ? version : 1;

    elemStrmAttr->streamOut(data(), stream, version, strmAttrLookup);

    return stream;
}

// REFERENCED-VALUE MANIPULATORS
template <class VALTYPE>
int AggregateRaw::insertItem(AggregateRaw    *newItem,
                                  ErrorAttributes *description,
                                  int                   index,
                                  const VALTYPE&        value) const
{
    bool wasNull = isNull();
    if (0 != insertNullItem(newItem, description, index)) {
        return -1;                                                    // RETURN
    }

    int rc = newItem->setValue(description, value);
    if (0 != rc && wasNull) {
        makeNull();
    }
    return rc;
}

template <class VALTYPE>
inline
int AggregateRaw::makeSelection(AggregateRaw    *field,
                                     ErrorAttributes *errorDescription,
                                     const char           *newSelector,
                                     const VALTYPE&        value) const
{
    int rc = makeSelection(field, errorDescription, newSelector);

    if (!rc) {
        rc = field->setValue(errorDescription, value);
    }
    return rc;
}

template <class VALTYPE>
inline
int AggregateRaw::makeSelectionById(
                                        AggregateRaw    *field,
                                        ErrorAttributes *errorDescription,
                                        int                   id,
                                        const VALTYPE&        value) const
{
    int rc = makeSelectionById(field, errorDescription, id);

    if (!rc) {
        rc = field->setValue(errorDescription, value);
    }
    return rc;
}

template <class VALTYPE>
inline
int AggregateRaw::makeSelectionByIndex(
                                        AggregateRaw    *field,
                                        ErrorAttributes *errorDescription,
                                        int                   index,
                                        const VALTYPE&        value) const
{
    int rc = makeSelectionByIndex(field, errorDescription, index);

    if (!rc) {
        rc = field->setValue(errorDescription, value);
    }
    return rc;
}

template <class VALTYPE>
inline
int AggregateRaw::setField(AggregateRaw    *field,
                                ErrorAttributes *errorDescription,
                                FieldSelector    fieldSelector1,
                                const VALTYPE&        value) const
{
    int rc = getField(field, errorDescription, true, fieldSelector1);
    if (!rc) {
        rc = field->setValue(errorDescription, value);
    }
    return rc;
}

template <class VALTYPE>
inline
int AggregateRaw::setField(AggregateRaw    *field,
                                ErrorAttributes *errorDescription,
                                FieldSelector    fieldSelector1,
                                FieldSelector    fieldSelector2,
                                const VALTYPE&        value) const
{
    int rc = getField(field,
                      errorDescription,
                      true,
                      fieldSelector1,
                      fieldSelector2);
    if (!rc) {
        rc = field->setValue(errorDescription, value);
    }
    return rc;
}

template <class VALTYPE>
inline
int AggregateRaw::setField(AggregateRaw    *field,
                                ErrorAttributes *errorDescription,
                                FieldSelector    fieldSelector1,
                                FieldSelector    fieldSelector2,
                                FieldSelector    fieldSelector3,
                                const VALTYPE&        value) const
{
    int rc = getField(field,
                      errorDescription,
                      true,
                      fieldSelector1,
                      fieldSelector2,
                      fieldSelector3);
    if (!rc) {
        rc = field->setValue(errorDescription, value);
    }
    return rc;
}

template <class VALTYPE>
inline
int AggregateRaw::setField(AggregateRaw    *field,
                                ErrorAttributes *errorDescription,
                                FieldSelector    fieldSelector1,
                                FieldSelector    fieldSelector2,
                                FieldSelector    fieldSelector3,
                                FieldSelector    fieldSelector4,
                                const VALTYPE&        value) const
{
    int rc = getField(field,
                      errorDescription,
                      true,
                      fieldSelector1,
                      fieldSelector2,
                      fieldSelector3,
                      fieldSelector4);
    if (!rc) {
        rc = field->setValue(errorDescription, value);
    }
    return rc;
}

template <class VALTYPE>
inline
int AggregateRaw::setField(AggregateRaw    *field,
                                ErrorAttributes *errorDescription,
                                FieldSelector    fieldSelector1,
                                FieldSelector    fieldSelector2,
                                FieldSelector    fieldSelector3,
                                FieldSelector    fieldSelector4,
                                FieldSelector    fieldSelector5,
                                const VALTYPE&        value) const
{
    int rc = getField(field,
                      errorDescription,
                      true,
                      fieldSelector1,
                      fieldSelector2,
                      fieldSelector3,
                      fieldSelector4,
                      fieldSelector5);
    if (!rc) {
        rc = field->setValue(errorDescription, value);
    }
    return rc;
}

template <class VALTYPE>
inline
int AggregateRaw::setField(AggregateRaw    *field,
                                ErrorAttributes *errorDescription,
                                FieldSelector    fieldSelector1,
                                FieldSelector    fieldSelector2,
                                FieldSelector    fieldSelector3,
                                FieldSelector    fieldSelector4,
                                FieldSelector    fieldSelector5,
                                FieldSelector    fieldSelector6,
                                const VALTYPE&        value) const
{
    int rc = getField(field,
                      errorDescription,
                      true,
                      fieldSelector1,
                      fieldSelector2,
                      fieldSelector3,
                      fieldSelector4,
                      fieldSelector5,
                      fieldSelector6);
    if (!rc) {
        rc = field->setValue(errorDescription, value);
    }
    return rc;
}

template <class VALTYPE>
inline
int AggregateRaw::setField(AggregateRaw    *field,
                                ErrorAttributes *errorDescription,
                                FieldSelector    fieldSelector1,
                                FieldSelector    fieldSelector2,
                                FieldSelector    fieldSelector3,
                                FieldSelector    fieldSelector4,
                                FieldSelector    fieldSelector5,
                                FieldSelector    fieldSelector6,
                                FieldSelector    fieldSelector7,
                                const VALTYPE&        value) const
{
    int rc = getField(field,
                      errorDescription,
                      true,
                      fieldSelector1,
                      fieldSelector2,
                      fieldSelector3,
                      fieldSelector4,
                      fieldSelector5,
                      fieldSelector6,
                      fieldSelector7);
    if (!rc) {
        rc = field->setValue(errorDescription, value);
    }
    return rc;
}

template <class VALTYPE>
inline
int AggregateRaw::setField(AggregateRaw    *field,
                                ErrorAttributes *errorDescription,
                                FieldSelector    fieldSelector1,
                                FieldSelector    fieldSelector2,
                                FieldSelector    fieldSelector3,
                                FieldSelector    fieldSelector4,
                                FieldSelector    fieldSelector5,
                                FieldSelector    fieldSelector6,
                                FieldSelector    fieldSelector7,
                                FieldSelector    fieldSelector8,
                                const VALTYPE&        value) const
{
    int rc = getField(field,
                      errorDescription,
                      true,
                      fieldSelector1,
                      fieldSelector2,
                      fieldSelector3,
                      fieldSelector4,
                      fieldSelector5,
                      fieldSelector6,
                      fieldSelector7,
                      fieldSelector8);
    if (!rc) {
        rc = field->setValue(errorDescription, value);
    }
    return rc;
}

template <class VALTYPE>
inline
int AggregateRaw::setField(AggregateRaw    *field,
                                ErrorAttributes *errorDescription,
                                FieldSelector    fieldSelector1,
                                FieldSelector    fieldSelector2,
                                FieldSelector    fieldSelector3,
                                FieldSelector    fieldSelector4,
                                FieldSelector    fieldSelector5,
                                FieldSelector    fieldSelector6,
                                FieldSelector    fieldSelector7,
                                FieldSelector    fieldSelector8,
                                FieldSelector    fieldSelector9,
                                const VALTYPE&        value) const
{
    int rc = getField(field,
                      errorDescription,
                      true,
                      fieldSelector1,
                      fieldSelector2,
                      fieldSelector3,
                      fieldSelector4,
                      fieldSelector5,
                      fieldSelector6,
                      fieldSelector7,
                      fieldSelector8,
                      fieldSelector9);
    if (!rc) {
        rc = field->setValue(errorDescription, value);
    }
    return rc;
}

template <class VALTYPE>
inline
int AggregateRaw::setField(AggregateRaw    *field,
                                ErrorAttributes *errorDescription,
                                FieldSelector    fieldSelector1,
                                FieldSelector    fieldSelector2,
                                FieldSelector    fieldSelector3,
                                FieldSelector    fieldSelector4,
                                FieldSelector    fieldSelector5,
                                FieldSelector    fieldSelector6,
                                FieldSelector    fieldSelector7,
                                FieldSelector    fieldSelector8,
                                FieldSelector    fieldSelector9,
                                FieldSelector    fieldSelector10,
                                const VALTYPE&        value) const
{
    int rc = getField(field,
                      errorDescription,
                      true,
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
    if (!rc) {
        rc = field->setValue(errorDescription, value);
    }
    return rc;
}

template <class TYPE>
int AggregateRaw::setValue(ErrorAttributes *errorDescription,
                                const TYPE&           value) const
{
    if (isError()) {
        return 0;                                                     // RETURN
    }

    if (isNillableScalarArray()) {
        if (isNull()) {
            makeValue();
        }

        if (0 != assignToNillableScalarArray(value)) {
            bsl::ostringstream oss;
            oss << "Value does not conform to record \""
                << AggregateRaw_Util::recordName(recordConstraint())
                << "\" in schema";
            errorDescription->setDescription(oss.str());
            errorDescription->setCode(ErrorCode::BCEM_NON_CONFORMANT);
            return -1;                                                // RETURN
        }
        return 0;                                                     // RETURN
    }

    if (! AggregateRaw_Util::isConformant(&value, recordConstraint())) {
        bsl::ostringstream oss;
        oss << "Value does not conform to record \""
            << AggregateRaw_Util::recordName(recordConstraint())
            << "\" in schema";
        errorDescription->setDescription(oss.str());
        errorDescription->setCode(ErrorCode::BCEM_NON_CONFORMANT);
        return -1;                                                    // RETURN
    }

    if (enumerationConstraint() && bdlmxxx::ElemType::isScalarType(dataType())) {
        return toEnum(errorDescription, value);                       // RETURN
    }
    else {
        bdlmxxx::ElemRef elemRef = asElemRef();
        if (0 != bdlmxxx::Convert::convert(&elemRef, value)) {
            bsl::ostringstream oss;
            oss << "Invalid conversion when setting "
                << bdlmxxx::ElemType::toAscii(dataType())
                << " value from "
                << bdlmxxx::ElemType::toAscii(getBdemType(value))
                << " value";
            errorDescription->setDescription(oss.str());
            errorDescription->setCode(ErrorCode::BCEM_BAD_CONVERSION);
            return -1;                                                // RETURN
        }
    }

    return 0;
}

template <>
inline
int AggregateRaw::setValue<AggregateRaw>(
                                    ErrorAttributes     *errorDescription,
                                    const AggregateRaw&  value) const
{
    // Specialization for 'VALUETYPE = AggregateRaw'.

    return setValue(errorDescription, value.asElemRef());
}
}  // close package namespace

// FREE OPERATORS
inline
bsl::ostream& bdlaggxxx::operator<<(bsl::ostream& stream, const AggregateRaw& rhs)
{
    return rhs.print(stream, 0, -1);
}

namespace bdlaggxxx {
                // ----------------------------------
                // struct AggregateRaw_BdeatInfo
                // ----------------------------------

// MANIPULATORS
inline
const char *& AggregateRaw_BdeatInfo::annotation()
{
    if (!d_areLazyAttributesSet) {
        setLazyAttributes();
    }
    return d_annotation_p;
}

inline
int& AggregateRaw_BdeatInfo::formattingMode()
{
    return d_formattingMode;
}

inline
int& AggregateRaw_BdeatInfo::id()
{
    return d_id;
}

inline
const char *& AggregateRaw_BdeatInfo::name()
{
    if (!d_areLazyAttributesSet) {
        setLazyAttributes();
    }
    return d_name_p;
}

inline
int& AggregateRaw_BdeatInfo::nameLength()
{
    if (!d_areLazyAttributesSet) {
        setLazyAttributes();
    }
    return d_nameLength;
}

inline
bool& AggregateRaw_BdeatInfo::hasDefaultValue()
{
    return d_hasDefaultValue;
}

inline
bool& AggregateRaw_BdeatInfo::isNullable()
{
    return d_isNullable;
}

// ACCESSORS
inline
const char *AggregateRaw_BdeatInfo::annotation() const
{
    if (!d_areLazyAttributesSet) {
        setLazyAttributes();
    }
    return d_annotation_p;
}

inline
int AggregateRaw_BdeatInfo::formattingMode() const
{
    return d_formattingMode;
}

inline
int AggregateRaw_BdeatInfo::id() const
{
    return d_id;
}

inline
const char *AggregateRaw_BdeatInfo::name() const
{
    if (!d_areLazyAttributesSet) {
        setLazyAttributes();
    }
    return d_name_p;
}

inline
int AggregateRaw_BdeatInfo::nameLength() const
{
    if (!d_areLazyAttributesSet) {
        setLazyAttributes();
    }
    return d_nameLength;
}

inline
bool AggregateRaw_BdeatInfo::hasDefaultValue() const
{
    return d_hasDefaultValue;
}

inline
bool AggregateRaw_BdeatInfo::isNullable() const
{
    return d_isNullable;
}

                // ----------------------------------
                // struct AggregateRaw_BdeatUtil
                // ----------------------------------

// MANIPULATORS
template <class MANIPULATOR>
int AggregateRaw_BdeatUtil::manipulateField(
                                                AggregateRaw *parent,
                                                MANIPULATOR&       manipulator,
                                                int                fieldIndex)
{
    if (! parent->recordConstraint()) {
        return -1;                                                    // RETURN
    }

    AggregateRaw   field;
    ErrorAttributes dummy;
    if (0 != parent->fieldByIndex(&field, &dummy, fieldIndex)) {
        return -1;                                                    // RETURN
    }

    AggregateRaw_BdeatInfo info(parent->recordConstraint(), fieldIndex);

    if (info.isNullable()) {
        NullableAdapter adapter = { &field };
        return manipulator(&adapter, info);                           // RETURN
    }
    else {
        return manipulator(&field, info);                             // RETURN
    }
}

// ACCESSORS
template <class ACCESSOR>
int AggregateRaw_BdeatUtil::accessField(
                                           const AggregateRaw& parent,
                                           ACCESSOR&                accessor,
                                           int                      fieldIndex)
{
    if (! parent.recordConstraint()) {
        return -1;                                                    // RETURN
    }

    AggregateRaw   field;
    ErrorAttributes dummy;
    if (0 != parent.fieldByIndex(&field, &dummy, fieldIndex)) {
        return -1;                                                    // RETURN
    }

    AggregateRaw_BdeatInfo info(parent.recordConstraint(), fieldIndex);

    // If a field is nullable but also has a default value, then we treat
    // it as non-nullable to avoid suppressing access to special (null)
    // values that are not the default.  For example a 'char' with a
    // default value of '127' should not be considered nullable or else
    // the accessor will not be called on a '0' value (which bdem
    // considers to be null but the rest of the world considers normal).

    if (info.isNullable() && !info.hasDefaultValue()) {
        NullableAdapter adapter = { &field };
        return accessor(adapter, info);                               // RETURN
    }
    else {
        return accessor(field, info);                                 // RETURN
    }
}

                // -----------------------------
                // struct AggregateRaw_Util
                // -----------------------------

// CLASS METHODS
inline
bool AggregateRaw_Util::isConformant(const void           *,
                                          const bdlmxxx::RecordDef *recordDef)
{
    // A non-aggregate conforms only if 'recordDef' is null.

    return 0 == recordDef;
}

template <class VISITOR>
int
AggregateRaw_Util::visitArray(void                *array,
                                   bdlmxxx::ElemType::Type  arrayType,
                                   VISITOR             *visitorPtr)
{
    VISITOR& visitorObj = *visitorPtr;
    switch (arrayType) {
      case bdlmxxx::ElemType::BDEM_CHAR_ARRAY:
        return visitorObj((bsl::vector<char> *) array);
      case bdlmxxx::ElemType::BDEM_SHORT_ARRAY:
        return visitorObj((bsl::vector<short> *) array);
      case bdlmxxx::ElemType::BDEM_INT_ARRAY:
        return visitorObj((bsl::vector<int> *) array);
      case bdlmxxx::ElemType::BDEM_INT64_ARRAY:
        return visitorObj((bsl::vector<bsls::Types::Int64> *) array);
      case bdlmxxx::ElemType::BDEM_FLOAT_ARRAY:
        return visitorObj((bsl::vector<float> *) array);
      case bdlmxxx::ElemType::BDEM_DOUBLE_ARRAY:
        return visitorObj((bsl::vector<double> *) array);
      case bdlmxxx::ElemType::BDEM_STRING_ARRAY:
        return visitorObj((bsl::vector<bsl::string> *) array);
      case bdlmxxx::ElemType::BDEM_DATETIME_ARRAY:
        return visitorObj((bsl::vector<bdlt::Datetime> *) array);
      case bdlmxxx::ElemType::BDEM_DATE_ARRAY:
        return visitorObj((bsl::vector<bdlt::Date> *) array);
      case bdlmxxx::ElemType::BDEM_TIME_ARRAY:
        return visitorObj((bsl::vector<bdlt::Time> *) array);
      case bdlmxxx::ElemType::BDEM_BOOL_ARRAY:
        return visitorObj((bsl::vector<bool> *) array);
      case bdlmxxx::ElemType::BDEM_DATETIMETZ_ARRAY:
        return visitorObj((bsl::vector<bdlt::DatetimeTz> *) array);
      case bdlmxxx::ElemType::BDEM_DATETZ_ARRAY:
        return visitorObj((bsl::vector<bdlt::DateTz> *) array);
      case bdlmxxx::ElemType::BDEM_TIMETZ_ARRAY:
        return visitorObj((bsl::vector<bdlt::TimeTz> *) array);
      default:
        return ErrorCode::BCEM_NOT_AN_ARRAY;
    }
}
}  // close package namespace


// ============================================================================
//           'bdlat_choicefunctions' overloads and specializations
// ============================================================================

namespace bdlat_ChoiceFunctions {

    // TYPES
    template <>
    struct IsChoice<bdlaggxxx::AggregateRaw> {
        enum { VALUE = 1 };
    };

}  // close namespace bdlat_ChoiceFunctions

namespace bdlaggxxx {
inline
bool bdlat_choiceHasSelection(const AggregateRaw& object,
                              int                      selectionId)
{
    return object.hasFieldById(selectionId);
}

bool bdlat_choiceHasSelection(const AggregateRaw&  object,
                              const char               *selectionName,
                              int                       selectionNameLength);
    // Return 'true' if the specified 'object' has a field having the specified
    // 'selectionName' of the specified 'selectionNameLength' and 'false'
    // otherwise.  The behavior is undefined unless '0 < selectionNameLength'
    // and 'selectionName'

int bdlat_choiceMakeSelection(AggregateRaw *object,
                              int                selectionId);
    // Make the field specified by 'selectionId' as the selection in the
    // specified 'object'.  Return 0 on success and a non-zero value otherwise.

int bdlat_choiceMakeSelection(AggregateRaw  *object,
                              const char         *selectionName,
                              int                 selectionNameLength);
    // Make the field specified by 'selectionName' of the specified
    // 'selectionNameLength' as the selection in the specified 'object'.
    // Return 0 on success and a non-zero value otherwise.

template <class MANIPULATOR>
inline
int bdlat_choiceManipulateSelection(AggregateRaw *object,
                                    MANIPULATOR&       manipulator)
    // Use the specified 'manipulator' to manipulate the selection in the
    // specified 'object'.  Return 0 on success and a non-zero value otherwise.
{
    int fieldIndex = object->selectorIndex();
    if (fieldIndex < 0) {
        return -1;                                                    // RETURN
    }

    return AggregateRaw_BdeatUtil::manipulateField(object,
                                                        manipulator,
                                                        fieldIndex);
}

template <class ACCESSOR>
inline
int bdlat_choiceAccessSelection(const AggregateRaw& object,
                                ACCESSOR&             accessor)
    // Use the specified 'accessor' to access the selection in the specified
    // 'object'.  Return 0 on success and a non-zero value otherwise.
{
    int fieldIndex = object.selectorIndex();
    if (fieldIndex < 0) {
        return -1;                                                    // RETURN
    }

    return AggregateRaw_BdeatUtil::accessField(object,
                                                    accessor,
                                                    fieldIndex);
}

inline
int bdlat_choiceSelectionId(const AggregateRaw& object)
    // Return the selection id corresponding to the selection of the specified
    // 'object' or 'bdlat_ChoiceFunctions::k_UNDEFINED_SELECTION_ID' if an
    // element is not selected.
{
    int selectorId = object.selectorId();

    return bdlmxxx::RecordDef::BDEM_NULL_FIELD_ID == selectorId
         ? bdlat_ChoiceFunctions::k_UNDEFINED_SELECTION_ID
         : selectorId;
}
}  // close package namespace

// ============================================================================
//           'bdlat_arrayfunctions' overloads and specializations
// ============================================================================

namespace bdlat_ArrayFunctions {

    // TYPES
    template <>
    struct IsArray<bdlaggxxx::AggregateRaw> {
        enum { VALUE = 1 };
    };

    template <>
    struct ElementType<bdlaggxxx::AggregateRaw> {
        typedef bdlaggxxx::AggregateRaw Type;
    };

}  // close namespace bdlat_ArrayFunctions

namespace bdlaggxxx {
inline
void bdlat_arrayResize(AggregateRaw *array, int newSize)
    // Resize the specified 'array' to have the specified 'newSize' elements.
    // The behavior is undefined unless 'array' refers to an array type.
{
    ErrorAttributes dummy;
    int rc = array->resize(&dummy, newSize);
    (void) rc; // avoid compiler warning in non-safe mode
}

inline
bsl::size_t bdlat_arraySize(const AggregateRaw& array)
    // Return the number of elements in the specified 'array'.  The behavior is
    // undefined unless 'array' refers to an array type.
{
    return array.length();
}

template <class ACCESSOR>
int bdlat_arrayAccessElement(const AggregateRaw& array,
                             ACCESSOR&                accessor,
                             int                      index)
    // Use the specified 'accessor' to access the element in the specified
    // 'array' at the specified 'index'.  Return 0 on success and a non-zero
    // value otherwise.
{
    AggregateRaw   element;
    ErrorAttributes dummy;
    if (0 != array.getArrayItem(&element, &dummy, index)) {
        return -1;                                                    // RETURN
    }

    const bdlmxxx::FieldDef *fieldDef = array.fieldDef();
    if (fieldDef
     && fieldDef->formattingMode() & bdlat_FormattingMode::e_NILLABLE) {
        AggregateRaw_BdeatUtil::NullableAdapter adapter = { &element };
        return accessor(adapter);                                     // RETURN
    }
    else {
        return accessor(element);                                     // RETURN
    }
}

template <class MANIPULATOR>
int bdlat_arrayManipulateElement(AggregateRaw *array,
                                 MANIPULATOR&       manipulator,
                                 int                index)
    // Use the specified 'manipulator' to manipulate the element in the
    // specified 'array' at the specified 'index'.  Return 0 on success and a
    // non-zero value otherwise.
{
    AggregateRaw    element;
    ErrorAttributes dummy;
    if (0 != array->getArrayItem(&element, &dummy, index)) {
        return -1;                                                    // RETURN
    }

    const bdlmxxx::FieldDef *fieldDef = array->fieldDef();
    if (fieldDef
     && fieldDef->formattingMode() & bdlat_FormattingMode::e_NILLABLE) {
        AggregateRaw_BdeatUtil::NullableAdapter adapter = { &element };
        return manipulator(&adapter);                                 // RETURN
    }
    else {
        return manipulator(&element);                                 // RETURN
    }
}
}  // close package namespace

// ============================================================================
//           'bdlat_enumfunctions' overloads and specializations
// ============================================================================

namespace bdlat_EnumFunctions {

    // TYPES
    template <>
    struct IsEnumeration<bdlaggxxx::AggregateRaw> {
        enum { VALUE = 1 };
    };
}  // close namespace bdlat_EnumFunctions

namespace bdlaggxxx {
int bdlat_enumFromString(AggregateRaw *result,
                         const char        *string,
                         int                stringLength);
    // Load into the specified 'result' the enumerator value corresponding to
    // the specified 'string' of the specified 'stringLength'.  Return 0 on
    // success and a non-zero value otherwise.  Note that if 'result' refers to
    // a 'bdlmxxx::ElemType::BDEM_STRING' then the value of 'result' would be equal
    // to the string having the value 'string' of 'stringLength' and if
    // 'result' refers to a 'bdlmxxx::ElemType::BDEM_INT' then the value of
    // 'result' would be equal to the integer enumerator value corresponding to
    // 'string'.

int bdlat_enumFromInt(AggregateRaw *result, int enumId);
    // Load into the specified 'result' the enumerator value corresponding to
    // the specified 'enumId'.  Return 0 on success and a non-zero value
    // otherwise.  Note that if 'result' refers to a
    // 'bdlmxxx::ElemType::BDEM_STRING' then the value of 'result' would be equal
    // to the string enumerator value corresponding to 'enumId' and if 'result'
    // refers to a 'bdlmxxx::ElemType::BDEM_INT' then the value of 'result' would
    // be equal to 'enumId'.

inline
void bdlat_enumToInt(int *result, const AggregateRaw& value)
    // Load into the specified 'result' the integer enumerator value
    // corresponding to the specified 'value' or an error aggregate on error.
{
    const bdlmxxx::EnumerationDef *enumDef = value.enumerationConstraint();
    if (! enumDef) {
        BSLS_ASSERT_OPT(!"Schema Error");
        *result = bdltuxxx::Unset<int>::unsetValue();
        return;                                                       // RETURN
    }

    *result = value.asInt();
}

inline
void bdlat_enumToString(bsl::string *result, const AggregateRaw& value)
    // Load into the specified 'result' the string enumerator value
    // corresponding to the specified 'value' or an error aggregate on error.
{
    const bdlmxxx::EnumerationDef *enumDef = value.enumerationConstraint();
    if (! enumDef) {
        return;                                                       // RETURN
    }

    *result = value.asString();
}

// ============================================================================
//                       'bdlat_typename' overloads
// ============================================================================

const char *bdlat_TypeName_className(const AggregateRaw& object);
     // Return the name of the record or enumeration definition for the
     // specified 'object' aggregate or a null pointer of 'object' does not
     // have a named record or enumeration definition.

// ============================================================================
//                       'bdlat_valuetype' overloads
// ============================================================================

void bdlat_valueTypeReset(AggregateRaw *object);
    // Reset the specified 'object' to its default-constructed state.

inline
void bdlat_valueTypeReset(AggregateRaw_BdeatUtil::NullableAdapter *object)
    // Reset the specified 'object' to its default-constructed (null) state.
{
    BSLS_ASSERT_SAFE(object);
    BSLS_ASSERT_SAFE(object->d_element_p);

    object->d_element_p->makeNull();
}
}  // close package namespace

// ============================================================================
//           'bdlat_sequencefunctions' overloads and specializations
// ============================================================================

namespace bdlat_SequenceFunctions {
    // META-FUNCTIONS
    bslmf::MetaInt<1> isSequenceMetaFunction(const bdlaggxxx::AggregateRaw&);

    template <>
    struct IsSequence<bdlaggxxx::AggregateRaw> {
        enum { VALUE = 1 };
    };
}  // close namespace bdlat_SequenceFunctions

namespace bdlaggxxx {
template <class MANIPULATOR>
int bdlat_sequenceManipulateAttribute(AggregateRaw *object,
                                      MANIPULATOR&       manipulator,
                                      const char        *attributeName,
                                      int                attributeNameLength)
    // Use the specified 'manipulator' to change the value of the field
    // corresponding to the specified 'attributeName' of the specified
    // 'attributeNameLength' in the specified 'object'.  Return 0 on success
    // and a non-zero value otherwise.
{
    if (! object->recordConstraint()) {
        return -1;                                                    // RETURN
    }

    int fieldIndex = AggregateRaw_BdeatUtil::fieldIndexFromName(
                                                   *object->recordConstraint(),
                                                   attributeName,
                                                   attributeNameLength);

    if (fieldIndex < 0) {
        return -1;                                                    // RETURN
    }

    return AggregateRaw_BdeatUtil::manipulateField(object,
                                                        manipulator,
                                                        fieldIndex);
}

template <class MANIPULATOR>
int bdlat_sequenceManipulateAttribute(AggregateRaw *object,
                                      MANIPULATOR&       manipulator,
                                      int                attributeId)
    // Use the specified 'manipulator' to change the value of the field
    // corresponding to the specified 'attributeId' in the specified 'object'.
    // Return 0 on success and a non-zero value otherwise.
{
    if (! object->recordConstraint()) {
        return -1;                                                    // RETURN
    }

    int fieldIndex = object->recordConstraint()->fieldIndex(attributeId);

    if (fieldIndex < 0) {
        return -1;                                                    // RETURN
    }

    return AggregateRaw_BdeatUtil::manipulateField(object,
                                                        manipulator,
                                                        fieldIndex);
}

template <class MANIPULATOR>
int bdlat_sequenceManipulateAttributes(AggregateRaw *object,
                                       MANIPULATOR&       manipulator)
    // Use the specified 'manipulator' to change the value of all fields in the
    // specified 'object'.  Return 0 on success and a non-zero value otherwise.
{
    if (! object->recordConstraint()) {
        return -1;                                                    // RETURN
    }

    const int numAttr = object->recordConstraint()->numFields();
    int ret = 0;
    for (int index = 0; 0 == ret && index < numAttr; ++index) {
        ret = AggregateRaw_BdeatUtil::manipulateField(object,
                                                           manipulator,
                                                           index);
    }

    return ret;
}

template <class ACCESSOR>
int bdlat_sequenceAccessAttribute(
                                 const AggregateRaw&  object,
                                 ACCESSOR&                 accessor,
                                 const char               *attributeName,
                                 int                       attributeNameLength)
    // Use the specified 'accessor' to access the value of the field
    // corresponding to the specified 'attributeName' of the specified
    // 'attributeNameLength' in the specified 'object'.  Return 0 on success
    // and a non-zero value otherwise.
{
    if (! object.recordConstraint()) {
        return -1;                                                    // RETURN
    }

    int fieldIndex = AggregateRaw_BdeatUtil::fieldIndexFromName(
                                                    *object.recordConstraint(),
                                                    attributeName,
                                                    attributeNameLength);

    if (fieldIndex < 0) {
        return -1;                                                    // RETURN
    }

    return AggregateRaw_BdeatUtil::accessField(object,
                                                    accessor,
                                                    fieldIndex);
}

template <class ACCESSOR>
int bdlat_sequenceAccessAttribute(const AggregateRaw& object,
                                  ACCESSOR&                accessor,
                                  int                      attributeId)
    // Use the specified 'accessor' to access the value of the field
    // corresponding to the specified 'attributeId' in the specified 'object'.
    // Return 0 on success and a non-zero value otherwise.
{
    if (! object.recordConstraint()) {
        return -1;                                                    // RETURN
    }

    int fieldIndex = object.recordConstraint()->fieldIndex(attributeId);

    if (fieldIndex < 0) {
        return -1;                                                    // RETURN
    }

    return AggregateRaw_BdeatUtil::accessField(object,
                                                    accessor,
                                                    fieldIndex);
}

template <class ACCESSOR>
int bdlat_sequenceAccessAttributes(const AggregateRaw& object,
                                   ACCESSOR&                accessor)
    // Use the specified 'accessor' to access the value of all fields in the
    // specified 'object'.  Return 0 on success and a non-zero value otherwise.
{
    if (! object.recordConstraint()) {
        return -1;                                                    // RETURN
    }

    const int numAttr = object.recordConstraint()->numFields();
    int ret = 0;
    for (int index = 0; 0 == ret && index < numAttr; ++index) {
        ret = AggregateRaw_BdeatUtil::accessField(object,
                                                       accessor,
                                                       index);
    }

    return ret;
}

inline
bool bdlat_sequenceHasAttribute(const AggregateRaw&  object,
                                const char               *attributeName,
                                int                       attributeNameLength)
    // Return 'true' if the specified 'object' has a field having the specified
    // 'attributeName' of the specified 'attributeNameLength' and 'false'
    // otherwise.
{
    if (! object.recordConstraint()) {
        return false;                                                 // RETURN
    }

    return 0 <= AggregateRaw_BdeatUtil::fieldIndexFromName(
                                                    *object.recordConstraint(),
                                                    attributeName,
                                                    attributeNameLength);
}

inline
bool bdlat_sequenceHasAttribute(const AggregateRaw& object,
                                int                      attributeId)
    // Return 'true' if the specified 'object' has a field having the specified
    // 'attributeId' and 'false' otherwise.
{
    return object.hasFieldById(attributeId);
}
}  // close package namespace


// ============================================================================
//           'bdlat_nullablevaluefunctions' overloads and specializations
// ============================================================================

namespace bdlat_NullableValueFunctions {

    // TYPES
    template <>
    struct IsNullableValue<bdlaggxxx::AggregateRaw_BdeatUtil::NullableAdapter> {
        enum { VALUE = 1 };
    };

    template <>
    struct ValueType<bdlaggxxx::AggregateRaw_BdeatUtil::NullableAdapter> {
        typedef bdlaggxxx::AggregateRaw Type;
    };

}  // close namespace bdlat_NullableValueFunctions

namespace bdlaggxxx {
inline
bool bdlat_nullableValueIsNull(
                    const AggregateRaw_BdeatUtil::NullableAdapter& object)
    // Return 'true' if the specified 'object' is null and 'false' otherwise.
{
    return object.d_element_p->isNull();
}

inline
void bdlat_nullableValueMakeValue(
                          AggregateRaw_BdeatUtil::NullableAdapter *object)
    // Make the value of the specified 'object' by resetting its null bit and
    // setting its value to its unset value.
{
    object->d_element_p->makeValue();
}

template <class MANIPULATOR>
inline
int bdlat_nullableValueManipulateValue(
                     AggregateRaw_BdeatUtil::NullableAdapter *object,
                     MANIPULATOR&                                  manipulator)
    // Use the specified 'manipulator' to change the value of the specified
    // nullable 'object'.  Return 0 on success and a non-zero value otherwise.
{
    return manipulator(object->d_element_p);
}

template <class ACCESSOR>
inline
int bdlat_nullableValueAccessValue(
                  const AggregateRaw_BdeatUtil::NullableAdapter& object,
                  ACCESSOR&                                           accessor)
    // Use the specified 'accessor' to access the value of the specified
    // nullable 'object'.  Return 0 on success and a non-zero value otherwise.
{
    return accessor(*object.d_element_p);
}

// ============================================================================
//                     'bdlat_typecategory' overloads
// ============================================================================

bdlat_TypeCategory::Value
bdlat_typeCategorySelect(const AggregateRaw& object);
    // Return the actual run-time category for the specified 'object'.  For
    // example, if 'object' contains an 'INT', return 'SIMPLE_CATEGORY'; if
    // 'object' contains a 'LIST', return 'SEQUENCE_CATEGORY', etc.

template <class ACCESSOR>
int bdlat_typeCategoryAccessArray(const AggregateRaw& object,
                                  ACCESSOR&                accessor)
    // Use the specified 'accessor' to access the value of the specified array
    // 'object'.  Return 0 on success and a non-zero value otherwise.
{
    // The 'bdeat' framework treats 'bsl::vector<char>' as a native type
    // for 'base64Binary' and 'hexBinary' schema types.

    typedef bdlat_TypeCategory::Array Tag;

    if (bdlmxxx::ElemType::BDEM_CHAR_ARRAY == object.dataType()) {
        return accessor(object.asElemRef().theCharArray(), Tag());    // RETURN
    }
    if (bdlmxxx::ElemType::isArrayType(object.dataType())) {
        return accessor(object, Tag());                               // RETURN
    }
    return accessor(object, bslmf::Nil());
}

template <class MANIPULATOR>
int bdlat_typeCategoryManipulateArray(AggregateRaw *object,
                                      MANIPULATOR&       manipulator)
    // Use the specified 'manipulator' to change the value of the specified
    // array 'object'.  Return 0 on success and a non-zero value otherwise.
{
    // The 'bdeat' framework treats 'bsl::vector<char>' as a native type
    // for 'base64Binary' and 'hexBinary' schema types.

    typedef bdlat_TypeCategory::Array Tag;

    if (bdlmxxx::ElemType::BDEM_CHAR_ARRAY == object->dataType()) {
        return manipulator(&object->asElemRef().theModifiableCharArray(),
                           Tag());                                    // RETURN
    }
    if (bdlmxxx::ElemType::isArrayType(object->dataType())) {
        return manipulator(object, Tag());                            // RETURN
    }

    return manipulator(object, bslmf::Nil());
}

template <class MANIPULATOR>
int bdlat_typeCategoryManipulateSimple(AggregateRaw *object,
                                       MANIPULATOR&       manipulator)
    // Use the specified 'manipulator' to change the value of the specified
    // simple 'object'.  Return 0 on success and a non-zero value otherwise.
{
    typedef bdlat_TypeCategory::Simple Tag;

    int result;

    switch (object->dataType()) {
      case bdlmxxx::ElemType::BDEM_CHAR: {
        result = manipulator(&object->asElemRef().theModifiableChar(), Tag());
      } break;
      case bdlmxxx::ElemType::BDEM_SHORT: {
        result = manipulator(&object->asElemRef().theModifiableShort(), Tag());
      } break;
      case bdlmxxx::ElemType::BDEM_INT: {
        result = manipulator(&object->asElemRef().theModifiableInt(), Tag());
      } break;
      case bdlmxxx::ElemType::BDEM_INT64: {
        result = manipulator(&object->asElemRef().theModifiableInt64(), Tag());
      } break;
      case bdlmxxx::ElemType::BDEM_FLOAT: {
        result = manipulator(&object->asElemRef().theModifiableFloat(), Tag());
      } break;
      case bdlmxxx::ElemType::BDEM_DOUBLE: {
        result = manipulator(&object->asElemRef().theModifiableDouble(),
                             Tag());
      } break;
      case bdlmxxx::ElemType::BDEM_STRING: {
        result = manipulator(&object->asElemRef().theModifiableString(),
                             Tag());
      } break;
      case bdlmxxx::ElemType::BDEM_DATETIME: {
        result = manipulator(&object->asElemRef().theModifiableDatetime(),
                             Tag());
      } break;
      case bdlmxxx::ElemType::BDEM_DATE: {
        result = manipulator(&object->asElemRef().theModifiableDate(), Tag());
      } break;
      case bdlmxxx::ElemType::BDEM_TIME: {
        result = manipulator(&object->asElemRef().theModifiableTime(), Tag());
      } break;
      case bdlmxxx::ElemType::BDEM_BOOL: {
        result = manipulator(&object->asElemRef().theModifiableBool(), Tag());
      } break;
      case bdlmxxx::ElemType::BDEM_DATETIMETZ: {
        result = manipulator(&object->asElemRef().theModifiableDatetimeTz(),
                             Tag());
      } break;
      case bdlmxxx::ElemType::BDEM_DATETZ: {
        result = manipulator(&object->asElemRef().theModifiableDateTz(),
                             Tag());
      } break;
      case bdlmxxx::ElemType::BDEM_TIMETZ: {
        result = manipulator(&object->asElemRef().theModifiableTimeTz(),
                             Tag());
      } break;
      default: {
        result = manipulator(object, bslmf::Nil());
      } break;
    }

    return result;
}

template <class ACCESSOR>
int bdlat_typeCategoryAccessSimple(const AggregateRaw& object,
                                   ACCESSOR&                accessor)
    // Use the specified 'accessor' to access the value of the specified simple
    // 'object'.  Return 0 on success and a non-zero value otherwise.
{
    typedef bdlat_TypeCategory::Simple Tag;

    int result;

    switch (object.dataType()) {
      case bdlmxxx::ElemType::BDEM_CHAR: {
        result = accessor(object.asElemRef().theChar(), Tag());
      } break;
      case bdlmxxx::ElemType::BDEM_SHORT: {
        result = accessor(object.asElemRef().theShort(), Tag());
      } break;
      case bdlmxxx::ElemType::BDEM_INT: {
        result = accessor(object.asElemRef().theInt(), Tag());
      } break;
      case bdlmxxx::ElemType::BDEM_INT64: {
        result = accessor(object.asElemRef().theInt64(), Tag());
      } break;
      case bdlmxxx::ElemType::BDEM_FLOAT: {
        result = accessor(object.asElemRef().theFloat(), Tag());
      } break;
      case bdlmxxx::ElemType::BDEM_DOUBLE: {
        result = accessor(object.asElemRef().theDouble(), Tag());
      } break;
      case bdlmxxx::ElemType::BDEM_STRING: {
        result = accessor(object.asElemRef().theString(), Tag());
      } break;
      case bdlmxxx::ElemType::BDEM_DATETIME: {
        result = accessor(object.asElemRef().theDatetime(), Tag());
      } break;
      case bdlmxxx::ElemType::BDEM_DATE: {
        result = accessor(object.asElemRef().theDate(), Tag());
      } break;
      case bdlmxxx::ElemType::BDEM_TIME: {
        result = accessor(object.asElemRef().theTime(), Tag());
      } break;
      case bdlmxxx::ElemType::BDEM_BOOL: {
        result = accessor(object.asElemRef().theBool(), Tag());
      } break;
      case bdlmxxx::ElemType::BDEM_DATETIMETZ: {
        result = accessor(object.asElemRef().theDatetimeTz(), Tag());
      } break;
      case bdlmxxx::ElemType::BDEM_DATETZ: {
        result = accessor(object.asElemRef().theDateTz(), Tag());
      } break;
      case bdlmxxx::ElemType::BDEM_TIMETZ: {
        result = accessor(object.asElemRef().theTimeTz(), Tag());
      } break;
      default: {
        result = accessor(object, bslmf::Nil());
      } break;
    }

    return result;
}
}  // close package namespace

}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2011
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ----------------------------------
