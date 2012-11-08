// bcem_aggregateraw.h                                                -*-C++-*-
#ifndef INCLUDED_BCEM_AGGREGATERAW
#define INCLUDED_BCEM_AGGREGATERAW

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a type for self-describing data (without reference count).
//
//@CLASSES:
//   bcem_AggregateRaw: pointer to fully-introspective, dynamically-typed data
//
//@SEE_ALSO: bcem_aggregate, bcem_errorattributes, bdem package
//
//@AUTHOR: David Schumann (dschumann1)
//
//@DESCRIPTION: This component provides a class representing dynamically typed,
// introspective data optionally described by a 'bdem_Schema'.  The type is
// similar to a "raw" (non-reference-counted) pointer in the sense that
// multiple 'bcem_AggregateRaw' objects can refer to the same data and they do
// not dispose of that data when they go out of scope.
//
// Thus 'bcem_AggregateRaw' is provided to allow efficient implementation of
// operations such as "visiting" the fields of a large object recursively (that
// may be inefficient on a reference counted aggregate, see 'bcem_Aggregate').
// Typically during such operations, a single aggregate object representing the
// top-level value remains in scope, and many sub-aggregate objects
// representing fields in that value are created and destroyed on the stack.
// The atomic operations required to maintain the reference count would be
// wasted in this scenario, as the count never reaches zero and when the
// operation is concluded there is no net change in the reference count.  Using
// 'bcem_AggregateRaw' to represent the sub-elements of the object will
// eliminate this expense.
//
///Data Representation
///-------------------
// 'bcem_AggregateRaw' has a 3-tuple of pointers: the address of a
// 'bdem_Schema' describing the structure of the data; a 'void *' pointing at
// the actual data; and the address of a control word holding a "top-level
// nullness bit." When an Aggregate object contains a nullable element, and
// that element is null, that information is stored in the parent element.  But
// for top-level objects, those not contained within other objects, the
// "nullness" of the object must be represented somewhere; the "top-level
// nullness bit" is used for this purpose.  See the 'bdem_ElemRef' component
// documentation, "Element Reference Nullability", for a discussion of nullness
// control words (described there as "bitmaps").
//
// 'bcem_AggregateRaw' also holds information referring to the parent element.
// Like the schema, data, and nullness data, this reference is uncounted and
// thus the parent must remain valid for the lifetime of the
// 'bcem_AggregateRaw' object.
//
///Error Handling
///--------------
// 'bcem_AggregateRaw' returns descriptive errors when possible.  These consist
// of an enumerated value of 'bcem_ErrorCode::Code', combined with a
// human-readable 'string' value.  In general, most methods of
// 'bcem_AggregateRaw' will return an arbitrary non-zero value on failure and
// populate a 'bcem_ErrorAttributes' (passed by pointer as an output parameter)
// with details.  See the documentation of 'bcem_ErrorAttributes' for the
// definition of the enumerated error conditions.
//
///Thread Safety
///-------------
// A 'bcem_AggregateRaw' maintains a non-reference-counted handle to
// possibly-shared data.  It is not safe to access or modify this shared data
// concurrently from different threads, therefore 'bcem_AggregateRaw' is,
// strictly speaking, *thread* *unsafe* as two aggregates may refer to the same
// shared data.  However, it is safe to concurrently access or modify two
// different 'bcem_AggregateRaw' objects refering to different data.
//
///Usage
///-----
// Using 'bcem_AggregateRaw' typically involves starting with a
// 'bcem_Aggregate' object and extracting 'bcem_AggregateRaw' from it,
// then working with that in place of the original aggregate.  In this example,
// we elide the initial population of the 'bcem_AggregateRaw' object and
// instead focus on iterating through fields of an array aggregate.
//..
//  void printFields(const bcem_AggregateRaw& object)
//  {
//     for (int i = 0; i < object.length(); ++i) {
//        bcem_AggregateRaw field;
//        bcem_ErrorAttributes error;
//        if (0 == object.fieldByIndex(&field, &error, i)) {
//            field.print(bsl::cout, 0, -1);
//        }
//     }
//  }
//..

#ifndef INCLUDED_BCESCM_VERSION
#include <bcescm_version.h>
#endif

#ifndef INCLUDED_BCEM_ERRORATTRIBUTES
#include <bcem_errorattributes.h>
#endif

#ifndef INCLUDED_BCEM_FIELDSELECTOR
#include <bcem_fieldselector.h>
#endif

#ifndef INCLUDED_BDEAT_ARRAYFUNCTIONS
#include <bdeat_arrayfunctions.h>
#endif

#ifndef INCLUDED_BDEAT_NULLABLEVALUEFUNCTIONS
#include <bdeat_nullablevaluefunctions.h>
#endif

#ifndef INCLUDED_BDEAT_TYPECATEGORY
#include <bdeat_typecategory.h>
#endif

#ifndef INCLUDED_BDEM_CHOICEARRAYITEM
#include <bdem_choicearrayitem.h>
#endif

#ifndef INCLUDED_BDEM_CONVERT
#include <bdem_convert.h>
#endif

#ifndef INCLUDED_BDEM_FIELDDEF
#include <bdem_fielddef.h>
#endif

#ifndef INCLUDED_BDEM_RECORDDEF
#include <bdem_recorddef.h>
#endif

#ifndef INCLUDED_BDEM_ROW
#include <bdem_row.h>
#endif

#ifndef INCLUDED_BDEM_SCHEMA
#include <bdem_schema.h>
#endif

#ifndef INCLUDED_BDEM_SELECTBDEMTYPE
#include <bdem_selectbdemtype.h>
#endif

#ifndef INCLUDED_BDEM_TABLE
#include <bdem_table.h>
#endif

#ifndef INCLUDED_BDETU_UNSET
#include <bdetu_unset.h>
#endif

#ifndef INCLUDED_BSL_SSTREAM
#include <bsl_sstream.h>
#endif

#ifndef INCLUDED_BSL_UTILITY
#include <bsl_utility.h>
#endif

namespace BloombergLP {

                     // =======================================
                     // class bcem_AggregateRaw_ElemDataFetcher
                     // =======================================

struct bcem_AggregateRaw_ElemDataFetcher {
    // This class accesses the address of a value held within a 'bdem_ElemRef'
    // or 'bdem_ConstElemRef' without affecting the nullness of the referenced
    // value.

    void *d_data_p;

    explicit
    bcem_AggregateRaw_ElemDataFetcher(const bdem_ElemRef& elemRef)
    {
        d_data_p = elemRef.dataRaw();
    }

    explicit
    bcem_AggregateRaw_ElemDataFetcher(const bdem_ConstElemRef& elemRef)
    {
        d_data_p = const_cast<void *>(elemRef.data());
    }
};

                     // ====================================
                     // class bcem_AggregateRaw_ArrayIndexer
                     // ====================================

class bcem_AggregateRaw_ArrayIndexer {
    // This class defines a function object to access the n'th element in
    // an array.

    // DATA
    int   d_index;   // index of array element to be accessed
    void *d_item_p;  // address of element (initially null)

    // NOT IMPLEMENTED
    bcem_AggregateRaw_ArrayIndexer(const bcem_AggregateRaw_ArrayIndexer&);
    bcem_AggregateRaw_ArrayIndexer& operator=(
                                        const bcem_AggregateRaw_ArrayIndexer&);

  public:
    // CREATORS
    explicit
    bcem_AggregateRaw_ArrayIndexer(int index)
    : d_index(index)
    , d_item_p(0)
    {
    }

    // MANIPULATORS
    template <typename ARRAYTYPE>
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
                         // class bcem_AggregateRaw_ArraySizer
                         // ==================================

struct bcem_AggregateRaw_ArraySizer {
    // This class defines a function object to return the size of a sequence.

    // ACCESSORS
    template <typename ARRAYTYPE>
    int operator()(ARRAYTYPE *array) const
    {
        return (int)array->size();
    }
};

                        // =======================
                        // class bcem_AggregateRaw
                        // =======================

class bcem_AggregateRaw {
    // This type provides a non-counted reference to the data of a
    // 'bcem_Aggregate' object.  It can be used to efficiently implement
    // operations, particularly recursive "visiting", where an aggregate
    // remains in scope throughout the operation and there would otherwise be
    // many temporary aggregate objects created and discarded.

    // Data invariants:
    // - If 'd_dataType' is 'bdem_ElemType::BDEM_VOID', then 'd_value' will be
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
    //   'bdem_ElemType::toArrayType(d_dataType)'.  The code in this class
    //   always uses 'd_dataType', not 'd_fieldDef_p->elemType()'.
    // - If this is the root object, then 'd_fieldDef_p' will be null, but
    //   'd_recordDef_p' may still have a value.  Otherwise,
    //   'd_recordDef_p'.  The code in this class always uses 'd_recordDef_p',
    //   not 'd_fieldDef_p->recordConstraint()' will always be equal to
    //   'd_fieldDef_p->recordConstraint()'.

    // DATA
    bdem_ElemType::Type   d_dataType;       // value's type
    const bdem_Schema    *d_schema_p;       // shared schema (held, not owned)
    const bdem_RecordDef *d_recordDef_p;    // record constraint (held, not
                                            // owned)
    const bdem_FieldDef  *d_fieldDef_p;     // data description (held, not
                                            // owned)
    void                 *d_value_p;        // pointer to data (held, not
                                            // owned)
    bdem_ElemType::Type   d_parentType;     // type of parent of this aggregate
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
    int descendIntoArrayItem(bcem_ErrorAttributes *errorDescription,
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

    int descendIntoField(bcem_ErrorAttributes      *errorDescription,
                         const bcem_FieldSelector&  fieldOrIdx,
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

    int descendIntoFieldById(bcem_ErrorAttributes *errorDescription,
                             int                   id);
        // Modify this aggregate so that it refers to the field with the
        // specified 'id' within the currently-referenced sequence or choice
        // object.  Return 0 on success, with no effect on the specified
        // 'errorDescription'; otherwise, load into 'errorDescription' a
        // description of the failure and return a nonzero value.

    int descendIntoFieldByIndex(bcem_ErrorAttributes *errorDescription,
                                int                   index);
        // Modify this aggregate so that it refers to the field with the
        // specified 'index' within the currently-referenced sequence or choice
        // object.  Return 0 on success, with no effect on the specified
        // 'errorDescription'; otherwise, load into 'errorDescription' a
        // description of the failure and return a nonzero value.

    int descendIntoFieldByName(bcem_ErrorAttributes *errorDescription,
                               const char           *name);
        // Modify this aggregate so that it refers to the field with the
        // specified 'name' within the currently-referenced sequence or choice
        // object.  Return 0 on success, with no effect on the specified
        // 'errorDescription'; otherwise, load into 'errorDescription' a
        // description of the failure and return a nonzero value.

    // PRIVATE ACCESSORS
    template <typename TYPE>
    int assignToNillableScalarArrayImp(const TYPE& value) const;
        // Assign the specified 'value' to this aggregate.  Return 0 on
        // success, and a non-zero value otherwise.  The behavior is undefined
        // unless this aggregate refers to a nillable escalar array, and
        // 'value' is a scalar array, or is convertible to one.  If value is
        // null, then make this aggregate null.  Leave this aggregate unchanged
        // if 'value' is not convertible to the type stored in this aggregate.
        // The parameterized 'TYPE' shall be either 'bdem_ElemRef' or
        // 'bdem_ConstElemRef'.

    template <typename TYPE>
    int assignToNillableScalarArray(const TYPE& value) const;
    template <typename TYPE>
    int assignToNillableScalarArray(const bsl::vector<TYPE>& value) const;
        // Assign the specified 'value' to this aggregate.  Return 0 on
        // success, and a non-zero value otherwise.  The behavior is undefined
        // unless this aggregate refers to a nillable scalar array, and 'value'
        // is a scalar array, or is convertible to one.  If value is null,
        // then make this aggregate null.  Leave this aggregate unchanged if
        // 'value' is not convertible to the type stored in this aggregate.

    int getFieldIndex(int                  *index,
                      bcem_ErrorAttributes *errorDescription,
                      const char           *fieldName,
                      const char           *caller) const;
    int getFieldIndex(int                  *index,
                      bcem_ErrorAttributes *errorDescription,
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

    int makeSelectionByIndexRaw(bcem_AggregateRaw    *result,
                                bcem_ErrorAttributes *errorDescription,
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

    template <typename VALUETYPE>
    int toEnum(bcem_ErrorAttributes *errorDescription,
               const VALUETYPE&      value) const;
    template <typename VALUETYPE>
    int toEnum(bcem_ErrorAttributes *errorDescription,
               const VALUETYPE&      value,
               bslmf_MetaInt<0>      direct) const;
    int toEnum(bcem_ErrorAttributes *errorDescription,
               const int&,
               bslmf_MetaInt<0>      direct) const;
    int toEnum(bcem_ErrorAttributes *errorDescription,
               const char           *value,
               bslmf_MetaInt<1>      direct) const;
    int toEnum(bcem_ErrorAttributes *errorDescription,
               const bsl::string&    value,
               bslmf_MetaInt<1>      direct) const;
    int toEnum(bcem_ErrorAttributes     *errorDescription,
               const bdem_ConstElemRef&  value,
               bslmf_MetaInt<1>          direct) const;
        // Set this enumeration to the specified 'value'.  The 'direct'
        // argument is to aid in template metaprogramming for overloading for
        // those types that can be directly processed and those that must first
        // be converted to 'int' using 'bdem_Convert'.  Return 0 on success or
        // a nonzero value with a description loaded into the specified
        // 'errorDescription' on failure.

  public:
    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS2(bcem_AggregateRaw,
                                  bslalg_TypeTraitBitwiseMoveable,
                                  bdeu_TypeTraitHasPrintMethod);

    // CLASS METHODS
    static int maxSupportedBdexVersion();
        // Return the most current 'bdex' streaming version number supported
        // by this class.  See the BDE package-group-level documentation for
        // more information on 'bdex' streaming of container types.

    static bool areEquivalent(const bcem_AggregateRaw& lhs,
                              const bcem_AggregateRaw& rhs);
        // Return 'true' if the value of the object referenced by the specified
        // 'lhs' aggregate equals the value of the object referenced by the
        // specified 'rhs' aggregate.  The aggregates need not be identical (as
        // per 'areIdentical'), but must have equivalent record definitions and
        // equal data and nullness values.  This predicate is roughly
        // equivalent to dereferencing two pointers and comparing their
        // pointed-to values for equality.  'areIdentical(lhs, rhs)' implies
        // 'areEquivalent(lhs, rhs)', but not vice versa.

    static bool areIdentical(const bcem_AggregateRaw& lhs,
                             const bcem_AggregateRaw& rhs);
        // Return 'true' if the specified 'lhs' aggregate refers to the same
        // object as the specified 'rhs' aggregate and 'false' otherwise.
        // Specifically, 'lhs' and 'rhs' are considered identical if 'dataType'
        // and 'dataPtr' return equal values for the respective arguments.
        // When two 'bcem_AggregateRaw' objects compare identical,
        // modifications to one of their referred-to object will be visible
        // through the other.  This predicate is roughly equivalent to
        // comparing two pointers for equality.  Note that if
        // 'bdem_ElemType::BDEM_VOID == lhs.dataType()' or
        // 'bdem_ElemType::BDEM_VOID == rhs.dataType()', then 'false' is
        // returned.

    template <typename TYPE>
    static bdem_ElemType::Type getBdemType(const TYPE& value);
        // Return the 'bdem_ElemType::Type' corresponding to the parameterized
        // 'value'.

    // CREATORS
    bcem_AggregateRaw();
        // Create an aggregate reference in an empty state.

    bcem_AggregateRaw(const bcem_AggregateRaw& original);
        // Create a raw aggregate that refers to the same schema and data as
        // that referred to by the specified 'original' raw aggregate.  The
        // behavior is undefined unless the schema and data referred to by
        // 'other' remains valid for the lifetime of this object.

#ifdef BDE_BUILD_TARGET_SAFE
    ~bcem_AggregateRaw();
        // Destroy this object.
#else
    //~bcem_AggregateRaw() = default;
        // Destroy this object.  Note that the compiler-generated default
        // is used.
#endif

    // MANIPULATORS
    bcem_AggregateRaw& operator=(const bcem_AggregateRaw& rhs);
        // Make this aggregate refer to the same data and schema as the
        // specified 'rhs' aggregate and return a reference to this aggregate.
        // This creates a new reference to existing data -- no data is copied.
        // The behavior is undefined unless the schema and data referred to by
        // 'other' remain valid for the lifetime of this object.  Note that if
        // 'rhs' is an error aggregate, then this aggregate will be assigned
        // the same error state as 'rhs'.

    void clearParent();
        // Make this aggregate a top-level aggregate by resetting the
        // parent information.

    void reset();
        // Reset this object to its default-constructed state.

    void setData(void *data);
        // Set the data pointer for this "raw" aggregate to the specified
        // 'data'.  The behavior is undefined unless 'data' remains valid for
        // the lifetime of this object.

    void setDataType(bdem_ElemType::Type dataType);
        // Set the type of data referenced by this "raw" aggregate to the
        // specified 'dataType'.

    void setFieldDef(const bdem_FieldDef *fieldDef);
        // Set the field definition pointer for this "raw" aggregate to the
        // specified 'fieldDef'.  The behavior is undefined unless 'fieldDef'
        // remains valid for the lifetime of this object.

    void setRecordDef(const bdem_RecordDef *recordDef);
        // Set the record definition poiner for this "raw" aggregate to the
        // specified 'recordDef'.  The behavior is undefined unless 'recordDef'
        // remains valid for the lifetime of this object.

    void setSchema(const bdem_Schema *schema);
        // Set the schema pointer for this "raw" aggregate to the specified
        // 'schema'.  The behavior is undefined unless 'schema' remains valid
        // for the lifetime of this object.

    void setTopLevelAggregateNullnessPointer(int *nullnessFlag);
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
        // 'LIST', call 'bdem_List::bdexStreamIn' on the referenced object).
        // See the 'bdex' package-level documentation for more information on
        // 'bdex' streaming of value-semantic types and containers.

                                  // Aspects

    void swap(bcem_AggregateRaw& other);
        // Efficiently exchange the states of this aggregate object and the
        // specified 'other' aggregate object such that value, schema, and
        // nullness information held by each will be what was formerly held by
        // the other.

    // ACCESSORS
    int anonymousField(bcem_AggregateRaw    *object,
                       bcem_ErrorAttributes *errorDescription) const;
        // If this aggregate contains exactly one field with a null name (i.e.,
        // an anonymous field), then load into the specified 'object' an
        // aggregate representing that field and return 0; otherwise return a
        // non-zero value, and load a descriptive error into
        // 'errorDescription'.  An anonymous field is a field with with a null
        // name.

    int anonymousField(bcem_AggregateRaw    *object,
                       bcem_ErrorAttributes *errorDescription,
                       int                   index) const;
        // Load into the specified 'object' a representation of the the field
        // with a null name (i.e., an anonymous field) within this object and
        // corresponding to the specified 'index'.  'index' is the index of the
        // field within the anonymous fields of this object, not within all the
        // fields of this object.  If there are not at least 'index + 1'
        // anonymous fields in this object, load a descriptive error into
        // 'errorDescription' and return a nonzero value; otherwise return 0
        // with no effect on 'errorDescription'.

    int getArrayItem(bcem_AggregateRaw    *item,
                     bcem_ErrorAttributes *errorDescription,
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
    bsls_Types::Int64 asInt64() const;
    float asFloat() const;
    double asDouble() const;
    bdet_Datetime asDatetime() const;
    bdet_DatetimeTz asDatetimeTz() const;
    bdet_Date asDate() const;
    bdet_DateTz asDateTz() const;
    bdet_Time asTime() const;
    bdet_TimeTz asTimeTz() const;
        // Convert the value referenced by this aggregate to the return type
        // using "Extended Type Conversions" as described in the
        // 'bcem_Aggregate' component-level documentation (returning the
        // enumerator ID when converting enumeration objects to numeric
        // values).  Return the appropriate "null" value if conversion fails.

    const bdem_ElemRef asElemRef() const;
        // Return an element reference providing modifiable access to the value
        // held by this aggregate.

    bsl::string asString() const;
        // Return a text representation of the value referenced by this
        // aggregate.  For enumeration values, the resulting string is the
        // enumerator name corresponding to the referenced value.  For date and
        // time values, the resulting string will have Iso8601 format.  For
        // other types, the resulting string will have the same format as
        // 'ostream' printing of the underlying data value.

    // TBD: Remove
    template <typename TOTYPE>
    TOTYPE convertScalar() const;
        // Return the scalar value stored in this aggregate converted to the
        // parameterized 'TOTYPE'.  Return an "unset" 'TOTYPE' value (see
        // 'bdetu_unset') unless this aggregate holds a scalar value that is
        // convertible to 'TOTYPE'.  'TOTYPE' shall be one of: 'bool', 'char',
        // 'short', 'int', 'bsls_Types::Int64', 'float', 'double',
        // 'bdet_Datetime', 'bdet_DatetimeTz', 'bdet_Date', 'bdet_DateTz',
        // 'bdet_Time', 'bdet_TimeTz'; or, unsigned versions of these.  But
        // note that if TOTYPE is unsigned, and this aggregate is not
        // convertible to 'TOTYPE', then the unset value of the corresponding
        // signed type is returned.

    const void *data() const;
        // Return the address of the non-modifiable data referenced by this
        // aggregate.

    bdem_ElemType::Type dataType() const;
        // Return the type of data referenced by this aggregate.  Return
        // 'bdem_ElemType::BDEM_VOID' for a void or error aggregate.

    const bdem_EnumerationDef *enumerationConstraint() const;
        // Return the address of the non-modifiable enumeration definition that
        // constrains the object referenced by this aggregate, or a null
        // pointer if this aggregate does not reference an enumeration object.

    int errorCode() const;
        // Return a negative error code describing the status of this object if
        // 'isError()' is 'true', or zero if 'isError()' is 'false'.  A set of
        // error code constants with names beginning with 'BCEM_' are
        // described in the 'bcem_aggregate' component-level documentation.

    bsl::string errorMessage() const;
        // Return a string describing the error state of this object of
        // 'isError()' is 'true', or an empty string if 'isError()' is 'false'.
        // The contents of the string are intended to be human readable and
        // descriptive.  The exact format of the string may change at any time
        // and should not be relied on in a program (use 'errorCode()',
        // instead).

    int fieldById(bcem_AggregateRaw    *field,
                  bcem_ErrorAttributes *errorDescription,
                  int                   fieldId) const;
        // Load into the specified 'field' object the field within this
        // aggregate with the specified (zero-based) 'fieldId'.  Return 0 on
        // success, with no effect on the specified 'errorDescription';
        // otherwise, load into 'errorDescription' a description of the failure
        // and return a nonzero value.

    int fieldByIndex(bcem_AggregateRaw    *field,
                     bcem_ErrorAttributes *errorDescription,
                     int                   index) const;
        // Load into the specified 'field' object the field within this
        // aggregate with the specified (zero-based) 'index'.  Return 0 on
        // success, with no effect on the specified 'errorDescription';
        // otherwise, load into 'errorDescription' a description of the failure
        // and return a nonzero value.

    const bdem_FieldDef *fieldDef() const;
        // Return the address of the non-modifiable field definition for the
        // object referenced by this aggregate, or null if this object does not
        // have a field definition.  An aggregate constructed directly using a
        // record definition will not have a field definition, whereas a
        // sub-aggregate returned by a field-access function (e.g.,
        // 'operator[]' or 'field') will.  Note that, if this aggregate is an
        // item within an array, table, or choice array, then
        // 'fieldDef()->elemType()' will return the *array* type, not the
        // *item* type (i.e., 'fieldDef()->elemType()' will not match
        // 'dataType()').

    int findUnambiguousChoice(bcem_AggregateRaw    *choiceObject,
                              bcem_ErrorAttributes *errorDescription,
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
               bcem_AggregateRaw    *resultField,
               bcem_ErrorAttributes *errorDescription,
               bool                  makeNonNullFlag,
               bcem_FieldSelector    fieldOrIdx1,
               bcem_FieldSelector    fieldOrIdx2 = bcem_FieldSelector(),
               bcem_FieldSelector    fieldOrIdx3 = bcem_FieldSelector(),
               bcem_FieldSelector    fieldOrIdx4 = bcem_FieldSelector(),
               bcem_FieldSelector    fieldOrIdx5 = bcem_FieldSelector(),
               bcem_FieldSelector    fieldOrIdx6 = bcem_FieldSelector(),
               bcem_FieldSelector    fieldOrIdx7 = bcem_FieldSelector(),
               bcem_FieldSelector    fieldOrIdx8 = bcem_FieldSelector(),
               bcem_FieldSelector    fieldOrIdx9 = bcem_FieldSelector(),
               bcem_FieldSelector    fieldOrIdx10= bcem_FieldSelector()) const;
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
        // 'bcem_FieldSelector', which is treated as the end of the argument
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
        // 'dataType()' will return 'bdem_ElemType::BDEM_VOID', 'errorCode()'
        // will return a non-zero value, and 'errorMessage()' will return a
        // non-empty string.

    bool isNull() const;
        // Return 'true' if the data referenced by this aggregate has a null
        // value, and 'false' otherwise.

    bool isNullable() const;
        // Return 'true' if the data referenced by this aggregate can be made
        // null, and 'false' otherwise.

    bool isVoid() const;
        // Return 'true' if 'dataType() == bdem_ElemType::BDEM_VOID'.

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
        // the 'bcem_aggregate' component-level documentation.  Otherwise,
        // return an error code.

    const bdem_RecordDef *recordConstraint() const;
        // Return the address of the non-modifiable record definition that
        // describes the structure of the object referenced by this aggregate,
        // or 0 if this aggregate references a scalar, array of scalars, or
        // unconstrained 'bdem' aggregate.

    const bdem_Schema *schema() const;
        // Return the address of the non-modifiable schema referenced by this
        // aggregate.

    int selection(bcem_AggregateRaw    *field,
                  bcem_ErrorAttributes *errorDescription) const;
        // If this aggregate refers to choice or choice array item, load into
        // the specified 'field' the sub-aggregate that refers to the
        // modifiable current selection, or a void aggregate if there is no
        // current selection.  If this aggregate refers to a list or row, look
        // for an anonymous field within the list or row and recursively look
        // for a choice in the anonymous field (if any), as per the "Anonymous
        // Fields" section of the 'bcem_aggregate' component-level
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
        // per the "Anonymous Fields" section of the 'bcem_aggregate'
        // component-level documentation.  The returned pointer is valid until
        // the the choice object is modified or destroyed.  Note that an error
        // condition is indistinguishable from one of the valid reasons for
        // returning an empty string.  The 'selection' or 'selectorId' methods
        // can be used to distinguish an error condition from a valid selector.

    int selectorId() const;
        // Return the ID for selector of the choice or choice array item
        // referenced by this aggregate, 'bdem_RecordDef::BDEM_NULL_FIELD_ID'
        // if there is no current selection, the current selection does not
        // have a numeric ID, this aggregate does not refer to a choice or
        // choice array item, or 'true == isNul()'.  If this aggregate refers
        // to a list or row, look for an anonymous field within the list or row
        // and recursively look for a choice in the anonymous field (if any),
        // as per the "Anonymous Fields" section of the 'bcem_aggregate'
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
        // as per the "Anonymous Fields" section of the 'bcem_aggregate'
        // component-level documentation.

    template <class STREAM>
    STREAM& bdexStreamOut(STREAM& stream, int version) const;
        // Write the value of the object referenced by this aggregate to the
        // specified output 'stream' using the specified 'version' format and
        // return a reference to the modifiable 'stream'.  If 'version' is not
        // supported, 'stream' is unmodified.  Note that 'version' is not
        // written to 'stream'.  This operation has the same effect as calling
        // 'bdexStreamOut' on the referenced object (e.g., if 'dataType()' is
        // 'LIST', call 'bdem_List::bdexStreamOut' on the referenced object).
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
    template <typename VALTYPE>
    int insertItem(bcem_AggregateRaw    *newItem,
                   bcem_ErrorAttributes *errorDescription,
                   int                   index,
                   const VALTYPE&        value) const;
        // Insert a copy of the specified 'value' before the specified 'index'
        // in the scalar array, table, or choice array referenced by this
        // aggregate.  Return 0 on success and load a reference to the new item
        // into the specified 'newItem'; otherwise, return a nonzero value and
        // load a description into the specified 'errorDescription'.

    int insertItems(bcem_ErrorAttributes *errorDescription,
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

    int insertNullItem(bcem_AggregateRaw    *newItem,
                       bcem_ErrorAttributes *errorDescription,
                       int                   index) const;
        // Insert a new element before the specified 'index' in the scalar
        // array, table, or choice array referenced by this aggregate.  The new
        // value is null and it is *not* set to its default value as with the
        // 'insertItems' method.  Return 0 on success and load a reference to
        // the new item into the specified 'newItem'; otherwise, return a
        // nonzero value and load a description into the specified
        // 'errorDescription'.

    int insertNullItems(bcem_ErrorAttributes *errorDescription,
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

    int makeSelection(bcem_AggregateRaw    *result,
                      bcem_ErrorAttributes *errorDescription,
                      const char           *newSelector) const;
        // Change the selector in the choice object referenced by this
        // aggregate to the one specified by 'newSelector', first making this
        // choice non-null if it is currently null.  The newly selected
        // sub-object is initialized to its default value.  Return 0 on
        // success, loading a sub-aggregate referring to the modifiable
        // selection into the specified 'field' object; otherwise, return a
        // nonzero error and populate the specified 'errorDescription' with no
        // effect on this object.

    template <typename VALTYPE>
    int makeSelection(bcem_AggregateRaw    *result,
                      bcem_ErrorAttributes *errorDescription,
                      const char           *newSelector,
                      const VALTYPE&        value) const;
        // Change the selector in the choice object referenced by this
        // aggregate to the one specified by 'newSelector', first making this
        // choice non-null if it is currently null and initializing the
        // sub-aggregate to the specified 'value' after appropriate conversions
        // (see "Extended Type Conversions" in the 'bcem_Aggregate'
        // component-level documentation).  Return 0 on success, loading a
        // sub-aggregate referring to the modifiable selection into the
        // specified 'field' object; otherwise, return a nonzero error and
        // populate the specified 'errorDescription' with no effect on this
        // object.

    int makeSelectionById(bcem_AggregateRaw    *field,
                          bcem_ErrorAttributes *errorDescription,
                          int                   newSelectorId) const;
        // Change the selector in the choice object referenced by this
        // aggregate to the one specified by 'newSelectorId', first making this
        // choice non-null if it is currently null.  The newly selected
        // sub-object is initialized to its default value.  Return 0 on
        // success, loading a sub-aggregate referring to the modifiable
        // selection into the specified 'field' object; otherwise, return a
        // nonzero error and populate the specified 'errorDescription' with no
        // effect on this object.

    template <typename VALTYPE>
    int makeSelectionById(bcem_AggregateRaw    *result,
                          bcem_ErrorAttributes *errorDescription,
                          int                   newSelectorId,
                          const VALTYPE&        value) const;
        // Change the selector in the choice object referenced by this
        // aggregate to the one specified by 'newSelectorId', first making this
        // choice non-null if it is currently null and initializing the
        // sub-aggregate to the specified 'value' after appropriate conversions
        // (see "Extended Type Conversions" in the 'bcem_Aggregate'
        // component-level documentation).  Return 0 on success, loading a
        // sub-aggregate referring to the modifiable selection into the
        // specified 'field' object; otherwise, return a nonzero error and
        // populate the specified 'errorDescription' with no effect on this
        // object.

    int makeSelectionByIndex(bcem_AggregateRaw    *field,
                             bcem_ErrorAttributes *errorDescription,
                             int                   index) const;
        // Change the selector in the choice object referenced by this
        // aggregate to the one specified by 'index', first making this choice
        // non-null if it is currently null.  The newly selected sub-object is
        // initialized to its default value.  Return 0 on success, loading a
        // sub-aggregate referring to the modifiable selection into the
        // specified 'field' object; otherwise, return a nonzero error and
        // populate the specified 'errorDescription' with no effect on this
        // object.

    template <typename VALTYPE>
    int makeSelectionByIndex(bcem_AggregateRaw    *field,
                             bcem_ErrorAttributes *errorDescription,
                             int                   index,
                             const VALTYPE&        value) const;
        // Change the selector in the choice object referenced by this
        // aggregate to the one specified by 'index', first making this choice
        // non-null if it is currently null and initializing the sub-aggregate
        // to the specified 'value' after appropriate conversions (see
        // "Extended Type Conversions" in the 'bcem_Aggregate' component-level
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

    int removeItems(bcem_ErrorAttributes *errorDescription,
                    int                   index,
                    int                   numItems) const;
        // Remove the specified 'numItems' items starting at the specified
        // 'index' in the scalar array, choice array, or table referenced by
        // this aggregate.  Return 0 on success, with no effect on the
        // specified 'errorDescription'; otherwise, load into
        // 'errorDescription' a description of the failure and return a nonzero
        // value.

    int reserveRaw(bcem_ErrorAttributes *errorDescription,
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

    int resize(bcem_ErrorAttributes *errorDescription,
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

    template <typename VALTYPE>
    int setField(bcem_AggregateRaw    *field,
                 bcem_ErrorAttributes *errorDescription,
                 bcem_FieldSelector    fieldSelector1,
                 const VALTYPE&        value) const;
    template <typename VALTYPE>
    int setField(bcem_AggregateRaw    *field,
                 bcem_ErrorAttributes *errorDescription,
                 bcem_FieldSelector    fieldSelector1,
                 bcem_FieldSelector    fieldSelector2,
                 const VALTYPE&        value) const;
    template <typename VALTYPE>
    int setField(bcem_AggregateRaw    *field,
                 bcem_ErrorAttributes *errorDescription,
                 bcem_FieldSelector    fieldSelector1,
                 bcem_FieldSelector    fieldSelector2,
                 bcem_FieldSelector    fieldSelector3,
                 const VALTYPE&        value) const;
    template <typename VALTYPE>
    int setField(bcem_AggregateRaw    *field,
                 bcem_ErrorAttributes *errorDescription,
                 bcem_FieldSelector    fieldSelector1,
                 bcem_FieldSelector    fieldSelector2,
                 bcem_FieldSelector    fieldSelector3,
                 bcem_FieldSelector    fieldSelector4,
                 const VALTYPE&        value) const;
    template <typename VALTYPE>
    int setField(bcem_AggregateRaw    *field,
                 bcem_ErrorAttributes *errorDescription,
                 bcem_FieldSelector    fieldSelector1,
                 bcem_FieldSelector    fieldSelector2,
                 bcem_FieldSelector    fieldSelector3,
                 bcem_FieldSelector    fieldSelector4,
                 bcem_FieldSelector    fieldSelector5,
                 const VALTYPE&        value) const;
    template <typename VALTYPE>
    int setField(bcem_AggregateRaw    *field,
                 bcem_ErrorAttributes *errorDescription,
                 bcem_FieldSelector    fieldSelector1,
                 bcem_FieldSelector    fieldSelector2,
                 bcem_FieldSelector    fieldSelector3,
                 bcem_FieldSelector    fieldSelector4,
                 bcem_FieldSelector    fieldSelector5,
                 bcem_FieldSelector    fieldSelector6,
                 const VALTYPE&        value) const;
    template <typename VALTYPE>
    int setField(bcem_AggregateRaw    *field,
                 bcem_ErrorAttributes *errorDescription,
                 bcem_FieldSelector    fieldSelector1,
                 bcem_FieldSelector    fieldSelector2,
                 bcem_FieldSelector    fieldSelector3,
                 bcem_FieldSelector    fieldSelector4,
                 bcem_FieldSelector    fieldSelector5,
                 bcem_FieldSelector    fieldSelector6,
                 bcem_FieldSelector    fieldSelector7,
                 const VALTYPE&        value) const;
    template <typename VALTYPE>
    int setField(bcem_AggregateRaw    *field,
                 bcem_ErrorAttributes *errorDescription,
                 bcem_FieldSelector    fieldSelector1,
                 bcem_FieldSelector    fieldSelector2,
                 bcem_FieldSelector    fieldSelector3,
                 bcem_FieldSelector    fieldSelector4,
                 bcem_FieldSelector    fieldSelector5,
                 bcem_FieldSelector    fieldSelector6,
                 bcem_FieldSelector    fieldSelector7,
                 bcem_FieldSelector    fieldSelector8,
                 const VALTYPE&        value) const;
    template <typename VALTYPE>
    int setField(bcem_AggregateRaw    *field,
                 bcem_ErrorAttributes *errorDescription,
                 bcem_FieldSelector    fieldSelector1,
                 bcem_FieldSelector    fieldSelector2,
                 bcem_FieldSelector    fieldSelector3,
                 bcem_FieldSelector    fieldSelector4,
                 bcem_FieldSelector    fieldSelector5,
                 bcem_FieldSelector    fieldSelector6,
                 bcem_FieldSelector    fieldSelector7,
                 bcem_FieldSelector    fieldSelector8,
                 bcem_FieldSelector    fieldSelector9,
                 const VALTYPE&        value) const;
    template <typename VALTYPE>
    int setField(bcem_AggregateRaw    *field,
                 bcem_ErrorAttributes *errorDescription,
                 bcem_FieldSelector    fieldSelector1,
                 bcem_FieldSelector    fieldSelector2,
                 bcem_FieldSelector    fieldSelector3,
                 bcem_FieldSelector    fieldSelector4,
                 bcem_FieldSelector    fieldSelector5,
                 bcem_FieldSelector    fieldSelector6,
                 bcem_FieldSelector    fieldSelector7,
                 bcem_FieldSelector    fieldSelector8,
                 bcem_FieldSelector    fieldSelector9,
                 bcem_FieldSelector    fieldSelector10,
                 const VALTYPE&        value) const;
        // Navigate to the field within this aggregate reached through the
        // specified chain of one to ten 'fieldSelector' arguments, each of
        // which specifies a field name or array index, then set that field to
        // the specified 'value', resetting its nullness flag if
        // 'field.isNull()' is 'true', after appropriate conversions (see
        // "Extended Type Conversions" in the 'bcem_Aggregate' component-level
        // documentation).  An empty string can be used for any of the
        // 'fieldSelector' arguments to specify the current selection within a
        // choice object.  If value is null then make the field null.  Note
        // that if any field in the chain of fields is null then an error is
        // returned.  Return 0 on success and load a reference to the
        // sub-aggregate into the specified 'field'; otherwise, return a
        // nonzero value and load a description into the specified
        // 'errorDescription'.

    template <typename TYPE>
    int setValue(bcem_ErrorAttributes *errorDescription,
                 const TYPE&           value) const;
        // Set the value referenced by this aggregate to the specified 'value',
        // converting the specified 'value' as necessary.  If 'value' is null
        // then make this aggregate null.  Return 0 on success, with no effect
        // on the specified 'errorDescription'; otherwise, load into
        // 'errorDescription' a description of the failure and return a nonzero
        // value.

    // REFERENCED-VALUE ACCESSORS
    int capacityRaw(bcem_ErrorAttributes *errorDescription,
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
bsl::ostream& operator<<(bsl::ostream& stream, const bcem_AggregateRaw& rhs);
    // Format 'rhs' in human-readable form (same format as
    // 'rhs.print(stream, 0, -1)') and return a modifiable reference to
    // 'stream'.

// ===========================================================================
//                      TEMPLATE SPECIALIZATIONS
// ===========================================================================

template <>
bsl::string bcem_AggregateRaw::convertScalar<bsl::string>() const;
    // Specialization of 'convertScalar<TOTYPE>' for 'TOTYPE = bsl::string'.

template <>
inline
int bcem_AggregateRaw::setValue<bcem_AggregateRaw>(
                                    bcem_ErrorAttributes     *errorDescription,
                                    const bcem_AggregateRaw&  value) const;
    // Specialization of 'setValue<VALUETYPE>' for
    // 'VALUETYPE = bcem_AggregateRaw'

                   // =============================
                   // struct bcem_AggregateRaw_Util
                   // =============================

struct bcem_AggregateRaw_Util {
    // This 'struct' provides a namespace for a set of utility methods
    // for working with the 'bdem' data representations of this component.

    static
    const char *enumerationName(const bdem_EnumerationDef *enumDef);
        // Return the name of the specified 'enumDef', or "(anonymous)" if
        // 'enumDef' has no name, or "(unconstrained)" if 'enumDef' is null.

    static bool isConformant(const bdem_ConstElemRef *object,
                             const bdem_RecordDef    *recordDef);
    static bool isConformant(const bdem_Row       *object,
                             const bdem_RecordDef *recordDef);
    static bool isConformant(const bdem_List      *object,
                             const bdem_RecordDef *recordDef);
    static bool isConformant(const bdem_Table     *object,
                             const bdem_RecordDef *recordDef);
    static bool isConformant(const bdem_ChoiceArrayItem *object,
                             const bdem_RecordDef       *recordDef);
    static bool isConformant(const bdem_Choice    *object,
                             const bdem_RecordDef *recordDef);
    static bool isConformant(const bdem_ChoiceArray *object,
                             const bdem_RecordDef   *recordDef);
        // Return 'true' if the specified 'object' is conformant with the
        // specified 'recordDef' as defined by 'bdem_SchemaAggregateUtil'.

    static bool isConformant(const void           *object,
                             const bdem_RecordDef *recordDef);
        // Return 'true' if the specified 'recordDef' is null, and 'false'
        // otherwise.

    static bool isConformant(const void           *object,
                             bdem_ElemType::Type   type,
                             const bdem_RecordDef *recordDef);
        // Return 'true' if the specified 'object' of the specified 'type' is
        // of aggregate type conforms to the specified 'recordDef' or
        // 'recordDef' is null.  Return 'false' if 'object' is of non-aggregate
        // type and 'recordDef' is non-null.  The behavior is undefined unless
        // 'object' is non-null.

    static
    const char *recordName(const bdem_RecordDef *recordDef);
        // Return the name of the specified 'recordDef', or "(anonymous)" if
        // 'recordDef' has no name, or "(unconstrained)" if 'recordDef' is
        // null.

    template <typename VISITOR>
    static
    int visitArray(void                *array,
                   bdem_ElemType::Type  arrayType,
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
        // on success, or 'bcem_ErrorCode::BCEM_NOT_AN_ARRAY' if
        // 'arrayType' is not an array of scalar 'bdem' type.  The behavior
        // is undefined unless 'array' is of 'arrayType'.
};

// ===========================================================================
//                      'bdeat' INTEGRATION
// ===========================================================================

                // ==================================
                // struct bcem_AggregateRaw_BdeatInfo
                // ==================================

struct bcem_AggregateRaw_BdeatInfo {
    // This 'struct' provides attribute information used when accessing or
    // manipulating the fields of 'bcem_AggregateRaw' within the 'bdeat'
    // framework.

    // DATA
    const bdem_RecordDef *d_record_p;
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
    bcem_AggregateRaw_BdeatInfo(const bdem_RecordDef *record,
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
                // struct bcem_AggregateRaw_BdeatUtil
                // ==================================

struct bcem_AggregateRaw_BdeatUtil {
    // This 'struct' provides a namespace for utility functions related to
    // 'bdeat' integration.

    // TYPES
    struct NullableAdapter {
        bcem_AggregateRaw *d_element_p;
    };

    // CLASS METHODS
    static
    int fieldIndexFromName(const bdem_RecordDef&  record,
                           const char            *name,
                           int                    nameLength);
        // Return the field index in the specified 'record' corresponding to
        // the specified 'name' of the specified 'nameLength' or a non-zero
        // value if no such field exists.

    template <typename MANIPULATOR>
    static
    int manipulateField(bcem_AggregateRaw *parent,
                        MANIPULATOR&       manipulator,
                        int                fieldIndex);
        // Use the specified 'manipulator' to change the value of the field
        // corresponding to the specified 'fieldIndex' in the specified
        // 'parent'.  Return 0 on success and a non-zero value otherwise.

    template <typename ACCESSOR>
    static
    int accessField(const bcem_AggregateRaw& parent,
                    ACCESSOR&                accessor,
                    int                      fieldIndex);
        // Use the specified 'accessor' to access the value of the field
        // corresponding to the specified 'fieldIndex' in the specified
        // 'parent'.  Return 0 on success and a non-zero value otherwise.
};

// ===========================================================================
//                      INLINE AND TEMPLATE FUNCTION DEFINITIONS
// ===========================================================================

                           //------------------------
                           // class bcem_AggregateRaw
                           //------------------------

// PRIVATE ACCESSORS
template <typename TYPE>
int bcem_AggregateRaw::assignToNillableScalarArrayImp(const TYPE& value) const
{
    bdem_ElemType::Type srcType = value.type();

    // Check conformance of value against this aggregate.

    if (bdem_ElemType::BDEM_TABLE == srcType) {
        return assignToNillableScalarArray(value.theTable());         // RETURN
    }

    bdem_ElemType::Type baseType = bdem_ElemType::fromArrayType(srcType);
    if (!bdem_ElemType::isScalarType(baseType)
      || baseType != recordConstraint()->field(0).elemType()) {
        return bcem_ErrorCode::BCEM_NON_CONFORMANT;                   // RETURN
    }

    if (value.isNull()) {
        makeNull();
        return 0;                                                     // RETURN
    }

    bcem_AggregateRaw_ElemDataFetcher fetcher(value);
    void *srcData = fetcher.d_data_p;
    bcem_AggregateRaw_ArraySizer sizer;
    const int  length  = bcem_AggregateRaw_Util::visitArray(srcData,
                                                            srcType,
                                                            &sizer);

    bcem_ErrorAttributes error;
    if (0 != resize(&error, length)) {
        return error.code();                                          // RETURN
    }

    bdem_Table            *dstTable     = (bdem_Table *)data();
    const bdem_Descriptor *baseTypeDesc =
                                  bdem_ElemAttrLookup::lookupTable()[baseType];

    for (int i = 0; i < length; ++i) {
        bcem_AggregateRaw_ArrayIndexer indexer(i);
        bcem_AggregateRaw_Util::visitArray(srcData, srcType, &indexer);
        baseTypeDesc->assign(dstTable->theModifiableRow(i)[0].data(),
                             indexer.data());
    }
    return 0;
}

template <typename TYPE>
inline
int bcem_AggregateRaw::assignToNillableScalarArray(const TYPE&) const
{
    BSLS_ASSERT_OPT("Invalid Type for Nillable Type" && 0);
    return -1;
}

template <>
inline
int bcem_AggregateRaw::assignToNillableScalarArray(
                                                 const bdem_Table& value) const
{
    if (!bcem_AggregateRaw_Util::isConformant(&value, recordConstraint())) {
        return bcem_ErrorCode::BCEM_NON_CONFORMANT;                   // RETURN
    }

    *(bdem_Table *)data() = value;
    return 0;
}

template <>
inline
int bcem_AggregateRaw::assignToNillableScalarArray(
                                          const bdem_ConstElemRef& value) const
{
    return assignToNillableScalarArrayImp(value);
}

template <>
inline
int bcem_AggregateRaw::assignToNillableScalarArray(
                                               const bdem_ElemRef& value) const
{
    return assignToNillableScalarArrayImp(value);
}

template <typename TYPE>
int bcem_AggregateRaw::assignToNillableScalarArray(
                                          const bsl::vector<TYPE>& value) const
{
    bdem_ElemType::Type baseType  =
                        (bdem_ElemType::Type) bdem_SelectBdemType<TYPE>::VALUE;

    if (baseType != recordConstraint()->field(0).elemType()) {
        return bcem_ErrorCode::BCEM_NON_CONFORMANT;                   // RETURN
    }

    const int length = static_cast<int>(value.size());
    bcem_ErrorAttributes errorDescription;
    if (0 != resize(&errorDescription, length)) {
        return errorDescription.code();                               // RETURN
    }
    if (0 == length) {
        return 0;                                                     // RETURN
    }

    bdem_Table            *dstTable     = (bdem_Table *)data();
    const bdem_Descriptor *baseTypeDesc =
                                  bdem_ElemAttrLookup::lookupTable()[baseType];
    typename bsl::vector<TYPE>::const_iterator iter = value.begin();
    for (int i = 0; i < length; ++i, ++iter) {
        baseTypeDesc->assign(dstTable->theModifiableRow(i)[0].data(),
                             (const void *) &(*iter));
    }
    return 0;
}

template <typename VALUETYPE>
inline
int bcem_AggregateRaw::toEnum(bcem_ErrorAttributes *errorDescription,
                              const VALUETYPE&      value) const
{
    static const int IS_DIRECT =
               bslmf_IsConvertible<VALUETYPE, const char*>::VALUE
            || bslmf_IsConvertible<VALUETYPE, bsl::string>::VALUE
            || bslmf_IsConvertible<VALUETYPE, const bdem_ConstElemRef&>::VALUE;

    return toEnum(errorDescription, value, bslmf_MetaInt<IS_DIRECT>());
}

template <typename VALUETYPE>
int bcem_AggregateRaw::toEnum(bcem_ErrorAttributes *errorDescription,
                              const VALUETYPE&      value,
                              bslmf_MetaInt<0>      direct) const
{
    int intVal;
    if (0 != bdem_Convert::convert(&intVal, value)) {
        bsl::ostringstream oss;
        oss << "Invalid conversion from \""
            << bdem_ElemType::toAscii(getBdemType(value))
            << "\" to enumeration \""
            << bcem_AggregateRaw_Util::enumerationName(enumerationConstraint())
            << '\"';
        errorDescription->setDescription(oss.str());
        errorDescription->setCode(bcem_ErrorCode::BCEM_BAD_CONVERSION);
        return -1;                                                    // RETURN
    }
    return toEnum(errorDescription, intVal, direct);
}

inline
int bcem_AggregateRaw::toEnum(bcem_ErrorAttributes *errorDescription,
                              const bsl::string&    value,
                              bslmf_MetaInt<1>      direct) const
{
    return toEnum(errorDescription, value.c_str(), direct);
}

// CLASS METHODS
inline
int bcem_AggregateRaw::maxSupportedBdexVersion()
{
    return 3;
}

template <typename TYPE>
inline
bdem_ElemType::Type bcem_AggregateRaw::getBdemType(const TYPE&)
{
    return (bdem_ElemType::Type) bdem_SelectBdemType<TYPE>::VALUE;
}

template <>
inline
bdem_ElemType::Type bcem_AggregateRaw::getBdemType(
                                                const bdem_ConstElemRef& value)
{
    return value.type();
}

template <>
inline
bdem_ElemType::Type bcem_AggregateRaw::getBdemType(const bdem_ElemRef& value)
{
    return value.type();
}

template <>
inline
bdem_ElemType::Type bcem_AggregateRaw::getBdemType(
                                                const bcem_AggregateRaw& value)
{
    return value.dataType();
}

// MANIPULATORS
inline
void bcem_AggregateRaw::setData(void *data)
{
    BSLS_ASSERT_SAFE(data);

    d_value_p = data;
}

inline
void bcem_AggregateRaw::setDataType(bdem_ElemType::Type dataType)
{
    d_dataType = dataType;
}

inline
void bcem_AggregateRaw::setFieldDef(const bdem_FieldDef *fieldDef)
{
    BSLS_ASSERT_SAFE(fieldDef);

    d_fieldDef_p = fieldDef;
}

inline
void bcem_AggregateRaw::setRecordDef(const bdem_RecordDef *recordDef)
{
    BSLS_ASSERT_SAFE(recordDef);

    d_recordDef_p = recordDef;
}

inline
void bcem_AggregateRaw::setSchema(const bdem_Schema *schema)
{
    BSLS_ASSERT_SAFE(schema);

    d_schema_p = schema;
}

inline
void bcem_AggregateRaw::setTopLevelAggregateNullnessPointer(int *nullnessFlag)
{
    BSLS_ASSERT_SAFE(nullnessFlag);

    d_isTopLevelAggregateNull_p = nullnessFlag;
}

template <class STREAM>
STREAM& bcem_AggregateRaw::bdexStreamIn(STREAM& stream, int version) const
{
    switch (version) {
      case 3: {
        char isNull;
        stream.getUint8(isNull);
        if (!stream) {
            return stream;                                            // RETURN
        }

        if (isNull) {
            if (bdem_ElemType::BDEM_VOID != d_dataType) {
                makeNull();
            }
            return stream;                                            // RETURN
        }
      }                                                         // FALL THROUGH
      case 2:                                                   // FALL THROUGH
      case 1: {
          switch (dataType()) {
          case bdem_ElemType::BDEM_VOID: {
            return stream;                                            // RETURN
          }
          case bdem_ElemType::BDEM_ROW: {
              bdem_Row *row = (bdem_Row*)d_value_p;
              return row->bdexStreamIn(stream, version);              // RETURN
          }
          case bdem_ElemType::BDEM_CHOICE_ARRAY_ITEM: {
            bdem_ChoiceArrayItem *item = (bdem_ChoiceArrayItem*)d_value_p;
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

    const bdem_DescriptorStreamIn<STREAM> *strmAttrLookup =
                            bdem_ElemStreamInAttrLookup<STREAM>::lookupTable();

    const bdem_Descriptor *const *elemAttrLookup =
                                            bdem_ElemAttrLookup::lookupTable();

    version = bdem_ElemType::isAggregateType(d_dataType) ? version : 1;

    strmAttrLookup[dataType()].streamIn((void*)d_value_p,
                                        stream,
                                        version,
                                        strmAttrLookup,
                                        elemAttrLookup);

    if (recordConstraint() && !bcem_AggregateRaw_Util::isConformant(
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
int bcem_AggregateRaw::getArrayItem(bcem_AggregateRaw    *item,
                                    bcem_ErrorAttributes *errorDescription,
                                    int                   index) const
{
    *item = *this;
    return item->descendIntoArrayItem(errorDescription, index, false);
}

template <typename TOTYPE>
inline
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

        return bdetu_Unset<TOTYPE>::unsetValue();                     // RETURN
    }

    return result;
}

inline
const void *bcem_AggregateRaw::data() const
{
    return d_value_p;
}

inline
bdem_ElemType::Type bcem_AggregateRaw::dataType() const
{
    return d_dataType;
}

inline
const bdem_EnumerationDef *bcem_AggregateRaw::enumerationConstraint() const
{
    return d_fieldDef_p ? d_fieldDef_p->enumerationConstraint() : 0;
}

inline
const bdem_FieldDef *bcem_AggregateRaw::fieldDef() const
{
    return d_fieldDef_p;
}

inline
bool bcem_AggregateRaw::isError() const
{
    // An error state is detectable as a void object with a non-null 'd_value'.
    // The 'd_value' holds the error record.

    return bdem_ElemType::BDEM_VOID == d_dataType && d_value_p;
}

inline
bool bcem_AggregateRaw::isVoid() const
{
    return bdem_ElemType::BDEM_VOID == d_dataType;
}

inline
const bdem_RecordDef *bcem_AggregateRaw::recordConstraint() const
{
    return d_recordDef_p;
}

inline
const bdem_Schema *bcem_AggregateRaw::schema() const
{
    return d_schema_p;
}

inline
int bcem_AggregateRaw::selection(bcem_AggregateRaw    *field,
                                 bcem_ErrorAttributes *errorDescription) const
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
STREAM& bcem_AggregateRaw::bdexStreamOut(STREAM& stream, int version) const
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
          case bdem_ElemType::BDEM_VOID: {
            return stream;                                            // RETURN
          }
          case bdem_ElemType::BDEM_ROW: {
            const bdem_Row *row = static_cast<const bdem_Row *>(d_value_p);
            return row->bdexStreamOut(stream, version);               // RETURN
          }
          case bdem_ElemType::BDEM_CHOICE_ARRAY_ITEM: {
            const bdem_ChoiceArrayItem *item =
                          static_cast<const bdem_ChoiceArrayItem *>(d_value_p);
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

    const bdem_DescriptorStreamOut<STREAM> *strmAttrLookup =
                           bdem_ElemStreamOutAttrLookup<STREAM>::lookupTable();

    const bdem_DescriptorStreamOut<STREAM> *elemStrmAttr =
                                                   &strmAttrLookup[d_dataType];

    version = bdem_ElemType::isAggregateType(d_dataType) ? version : 1;

    elemStrmAttr->streamOut(data(), stream, version, strmAttrLookup);

    return stream;
}

// REFERENCED-VALUE MANIPULATORS
template <typename VALTYPE>
int bcem_AggregateRaw::insertItem(bcem_AggregateRaw    *newItem,
                                  bcem_ErrorAttributes *description,
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

template <typename VALTYPE>
inline
int bcem_AggregateRaw::makeSelection(bcem_AggregateRaw    *field,
                                     bcem_ErrorAttributes *errorDescription,
                                     const char           *newSelector,
                                     const VALTYPE&        value) const
{
    int rc = makeSelection(field, errorDescription, newSelector);

    if (!rc) {
        rc = field->setValue(errorDescription, value);
    }
    return rc;
}

template <typename VALTYPE>
inline
int bcem_AggregateRaw::makeSelectionById(
                                        bcem_AggregateRaw    *field,
                                        bcem_ErrorAttributes *errorDescription,
                                        int                   id,
                                        const VALTYPE&        value) const
{
    int rc = makeSelectionById(field, errorDescription, id);

    if (!rc) {
        rc = field->setValue(errorDescription, value);
    }
    return rc;
}

template <typename VALTYPE>
inline
int bcem_AggregateRaw::makeSelectionByIndex(
                                        bcem_AggregateRaw    *field,
                                        bcem_ErrorAttributes *errorDescription,
                                        int                   index,
                                        const VALTYPE&        value) const
{
    int rc = makeSelectionByIndex(field, errorDescription, index);

    if (!rc) {
        rc = field->setValue(errorDescription, value);
    }
    return rc;
}

template <typename VALTYPE>
inline
int bcem_AggregateRaw::setField(bcem_AggregateRaw    *field,
                                bcem_ErrorAttributes *errorDescription,
                                bcem_FieldSelector    fieldSelector1,
                                const VALTYPE&        value) const
{
    int rc = getField(field, errorDescription, true, fieldSelector1);
    if (!rc) {
        field->setValue(errorDescription, value);
    }
    return rc;
}

template <typename VALTYPE>
inline
int bcem_AggregateRaw::setField(bcem_AggregateRaw    *field,
                                bcem_ErrorAttributes *errorDescription,
                                bcem_FieldSelector    fieldSelector1,
                                bcem_FieldSelector    fieldSelector2,
                                const VALTYPE&        value) const
{
    int rc = getField(field,
                      errorDescription,
                      true,
                      fieldSelector1,
                      fieldSelector2);
    if (!rc) {
        field->setValue(errorDescription, value);
    }
    return rc;
}

template <typename VALTYPE>
inline
int bcem_AggregateRaw::setField(bcem_AggregateRaw    *field,
                                bcem_ErrorAttributes *errorDescription,
                                bcem_FieldSelector    fieldSelector1,
                                bcem_FieldSelector    fieldSelector2,
                                bcem_FieldSelector    fieldSelector3,
                                const VALTYPE&        value) const
{
    int rc = getField(field,
                      errorDescription,
                      true,
                      fieldSelector1,
                      fieldSelector2,
                      fieldSelector3);
    if (!rc) {
        field->setValue(errorDescription, value);
    }
    return rc;
}

template <typename VALTYPE>
inline
int bcem_AggregateRaw::setField(bcem_AggregateRaw    *field,
                                bcem_ErrorAttributes *errorDescription,
                                bcem_FieldSelector    fieldSelector1,
                                bcem_FieldSelector    fieldSelector2,
                                bcem_FieldSelector    fieldSelector3,
                                bcem_FieldSelector    fieldSelector4,
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
        field->setValue(errorDescription, value);
    }
    return rc;
}

template <typename VALTYPE>
inline
int bcem_AggregateRaw::setField(bcem_AggregateRaw    *field,
                                bcem_ErrorAttributes *errorDescription,
                                bcem_FieldSelector    fieldSelector1,
                                bcem_FieldSelector    fieldSelector2,
                                bcem_FieldSelector    fieldSelector3,
                                bcem_FieldSelector    fieldSelector4,
                                bcem_FieldSelector    fieldSelector5,
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
        field->setValue(errorDescription, value);
    }
    return rc;
}

template <typename VALTYPE>
inline
int bcem_AggregateRaw::setField(bcem_AggregateRaw    *field,
                                bcem_ErrorAttributes *errorDescription,
                                bcem_FieldSelector    fieldSelector1,
                                bcem_FieldSelector    fieldSelector2,
                                bcem_FieldSelector    fieldSelector3,
                                bcem_FieldSelector    fieldSelector4,
                                bcem_FieldSelector    fieldSelector5,
                                bcem_FieldSelector    fieldSelector6,
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
        field->setValue(errorDescription, value);
    }
    return rc;
}

template <typename VALTYPE>
inline
int bcem_AggregateRaw::setField(bcem_AggregateRaw    *field,
                                bcem_ErrorAttributes *errorDescription,
                                bcem_FieldSelector    fieldSelector1,
                                bcem_FieldSelector    fieldSelector2,
                                bcem_FieldSelector    fieldSelector3,
                                bcem_FieldSelector    fieldSelector4,
                                bcem_FieldSelector    fieldSelector5,
                                bcem_FieldSelector    fieldSelector6,
                                bcem_FieldSelector    fieldSelector7,
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
        field->setValue(errorDescription, value);
    }
    return rc;
}

template <typename VALTYPE>
inline
int bcem_AggregateRaw::setField(bcem_AggregateRaw    *field,
                                bcem_ErrorAttributes *errorDescription,
                                bcem_FieldSelector    fieldSelector1,
                                bcem_FieldSelector    fieldSelector2,
                                bcem_FieldSelector    fieldSelector3,
                                bcem_FieldSelector    fieldSelector4,
                                bcem_FieldSelector    fieldSelector5,
                                bcem_FieldSelector    fieldSelector6,
                                bcem_FieldSelector    fieldSelector7,
                                bcem_FieldSelector    fieldSelector8,
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
        field->setValue(errorDescription, value);
    }
    return rc;
}

template <typename VALTYPE>
inline
int bcem_AggregateRaw::setField(bcem_AggregateRaw    *field,
                                bcem_ErrorAttributes *errorDescription,
                                bcem_FieldSelector    fieldSelector1,
                                bcem_FieldSelector    fieldSelector2,
                                bcem_FieldSelector    fieldSelector3,
                                bcem_FieldSelector    fieldSelector4,
                                bcem_FieldSelector    fieldSelector5,
                                bcem_FieldSelector    fieldSelector6,
                                bcem_FieldSelector    fieldSelector7,
                                bcem_FieldSelector    fieldSelector8,
                                bcem_FieldSelector    fieldSelector9,
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
        field->setValue(errorDescription, value);
    }
    return rc;
}

template <typename VALTYPE>
inline
int bcem_AggregateRaw::setField(bcem_AggregateRaw    *field,
                                bcem_ErrorAttributes *errorDescription,
                                bcem_FieldSelector    fieldSelector1,
                                bcem_FieldSelector    fieldSelector2,
                                bcem_FieldSelector    fieldSelector3,
                                bcem_FieldSelector    fieldSelector4,
                                bcem_FieldSelector    fieldSelector5,
                                bcem_FieldSelector    fieldSelector6,
                                bcem_FieldSelector    fieldSelector7,
                                bcem_FieldSelector    fieldSelector8,
                                bcem_FieldSelector    fieldSelector9,
                                bcem_FieldSelector    fieldSelector10,
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
        field->setValue(errorDescription, value);
    }
    return rc;
}

template <typename TYPE>
int bcem_AggregateRaw::setValue(bcem_ErrorAttributes *errorDescription,
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
                << bcem_AggregateRaw_Util::recordName(recordConstraint())
                << "\" in schema";
            errorDescription->setDescription(oss.str());
            errorDescription->setCode(bcem_ErrorCode::BCEM_NON_CONFORMANT);
            return -1;                                                // RETURN
        }
        return 0;                                                     // RETURN
    }

    if (! bcem_AggregateRaw_Util::isConformant(&value, recordConstraint())) {
        bsl::ostringstream oss;
        oss << "Value does not conform to record \""
            << bcem_AggregateRaw_Util::recordName(recordConstraint())
            << "\" in schema";
        errorDescription->setDescription(oss.str());
        errorDescription->setCode(bcem_ErrorCode::BCEM_NON_CONFORMANT);
        return -1;                                                    // RETURN
    }

    if (enumerationConstraint() && bdem_ElemType::isScalarType(dataType())) {
        return toEnum(errorDescription, value);                       // RETURN
    }
    else {
        bdem_ElemRef elemRef = asElemRef();
        if (0 != bdem_Convert::convert(&elemRef, value)) {
            bsl::ostringstream oss;
            oss << "Invalid conversion when setting "
                << bdem_ElemType::toAscii(dataType())
                << " value from "
                << bdem_ElemType::toAscii(getBdemType(value))
                << " value";
            errorDescription->setDescription(oss.str());
            errorDescription->setCode(bcem_ErrorCode::BCEM_BAD_CONVERSION);
            return -1;                                                // RETURN
        }
    }

    return 0;
}

template <>
inline
int bcem_AggregateRaw::setValue<bcem_AggregateRaw>(
                                    bcem_ErrorAttributes     *errorDescription,
                                    const bcem_AggregateRaw&  value) const
{
    // Specialization for 'VALUETYPE = bcem_AggregateRaw'.

    return setValue(errorDescription, value.asElemRef());
}

// FREE OPERATORS
inline
bsl::ostream& operator<<(bsl::ostream& stream, const bcem_AggregateRaw& rhs)
{
    return rhs.print(stream, 0, -1);
}

                // ----------------------------------
                // struct bcem_AggregateRaw_BdeatInfo
                // ----------------------------------

// MANIPULATORS
inline
const char *& bcem_AggregateRaw_BdeatInfo::annotation()
{
    if (!d_areLazyAttributesSet) {
        setLazyAttributes();
    }
    return d_annotation_p;
}

inline
int& bcem_AggregateRaw_BdeatInfo::formattingMode()
{
    return d_formattingMode;
}

inline
int& bcem_AggregateRaw_BdeatInfo::id()
{
    return d_id;
}

inline
const char *& bcem_AggregateRaw_BdeatInfo::name()
{
    if (!d_areLazyAttributesSet) {
        setLazyAttributes();
    }
    return d_name_p;
}

inline
int& bcem_AggregateRaw_BdeatInfo::nameLength()
{
    if (!d_areLazyAttributesSet) {
        setLazyAttributes();
    }
    return d_nameLength;
}

inline
bool& bcem_AggregateRaw_BdeatInfo::hasDefaultValue()
{
    return d_hasDefaultValue;
}

inline
bool& bcem_AggregateRaw_BdeatInfo::isNullable()
{
    return d_isNullable;
}

// ACCESSORS
inline
const char *bcem_AggregateRaw_BdeatInfo::annotation() const
{
    if (!d_areLazyAttributesSet) {
        setLazyAttributes();
    }
    return d_annotation_p;
}

inline
int bcem_AggregateRaw_BdeatInfo::formattingMode() const
{
    return d_formattingMode;
}

inline
int bcem_AggregateRaw_BdeatInfo::id() const
{
    return d_id;
}

inline
const char *bcem_AggregateRaw_BdeatInfo::name() const
{
    if (!d_areLazyAttributesSet) {
        setLazyAttributes();
    }
    return d_name_p;
}

inline
int bcem_AggregateRaw_BdeatInfo::nameLength() const
{
    if (!d_areLazyAttributesSet) {
        setLazyAttributes();
    }
    return d_nameLength;
}

inline
bool bcem_AggregateRaw_BdeatInfo::hasDefaultValue() const
{
    return d_hasDefaultValue;
}

inline
bool bcem_AggregateRaw_BdeatInfo::isNullable() const
{
    return d_isNullable;
}

                // ----------------------------------
                // struct bcem_AggregateRaw_BdeatUtil
                // ----------------------------------

// MANIPULATORS
template <typename MANIPULATOR>
int bcem_AggregateRaw_BdeatUtil::manipulateField(
                                                bcem_AggregateRaw *parent,
                                                MANIPULATOR&       manipulator,
                                                int                fieldIndex)
{
    if (! parent->recordConstraint()) {
        return -1;                                                    // RETURN
    }

    bcem_AggregateRaw   field;
    bcem_ErrorAttributes dummy;
    if (0 != parent->fieldByIndex(&field, &dummy, fieldIndex)) {
        return -1;                                                    // RETURN
    }

    bcem_AggregateRaw_BdeatInfo info(parent->recordConstraint(), fieldIndex);

    if (info.isNullable()) {
        NullableAdapter adapter = { &field };
        return manipulator(&adapter, info);                           // RETURN
    }
    else {
        return manipulator(&field, info);                             // RETURN
    }
}

// ACCESSORS
template <typename ACCESSOR>
int bcem_AggregateRaw_BdeatUtil::accessField(
                                           const bcem_AggregateRaw& parent,
                                           ACCESSOR&                accessor,
                                           int                      fieldIndex)
{
    if (! parent.recordConstraint()) {
        return -1;                                                    // RETURN
    }

    bcem_AggregateRaw   field;
    bcem_ErrorAttributes dummy;
    if (0 != parent.fieldByIndex(&field, &dummy, fieldIndex)) {
        return -1;                                                    // RETURN
    }

    bcem_AggregateRaw_BdeatInfo info(parent.recordConstraint(), fieldIndex);

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
                // struct bcem_AggregateRaw_Util
                // -----------------------------

// CLASS METHODS
inline
bool bcem_AggregateRaw_Util::isConformant(const void           *,
                                          const bdem_RecordDef *recordDef)
{
    // A non-aggregate conforms only if 'recordDef' is null.

    return 0 == recordDef;
}

template <typename VISITOR>
int
bcem_AggregateRaw_Util::visitArray(void                *array,
                                   bdem_ElemType::Type  arrayType,
                                   VISITOR             *visitorPtr)
{
    VISITOR& visitorObj = *visitorPtr;
    switch (arrayType) {
      case bdem_ElemType::BDEM_CHAR_ARRAY:
        return visitorObj((bsl::vector<char> *) array);
      case bdem_ElemType::BDEM_SHORT_ARRAY:
        return visitorObj((bsl::vector<short> *) array);
      case bdem_ElemType::BDEM_INT_ARRAY:
        return visitorObj((bsl::vector<int> *) array);
      case bdem_ElemType::BDEM_INT64_ARRAY:
          return visitorObj((bsl::vector<bsls_Types::Int64> *) array);
      case bdem_ElemType::BDEM_FLOAT_ARRAY:
        return visitorObj((bsl::vector<float> *) array);
      case bdem_ElemType::BDEM_DOUBLE_ARRAY:
        return visitorObj((bsl::vector<double> *) array);
      case bdem_ElemType::BDEM_STRING_ARRAY:
        return visitorObj((bsl::vector<bsl::string> *) array);
      case bdem_ElemType::BDEM_DATETIME_ARRAY:
        return visitorObj((bsl::vector<bdet_Datetime> *) array);
      case bdem_ElemType::BDEM_DATE_ARRAY:
        return visitorObj((bsl::vector<bdet_Date> *) array);
      case bdem_ElemType::BDEM_TIME_ARRAY:
        return visitorObj((bsl::vector<bdet_Time> *) array);
      case bdem_ElemType::BDEM_BOOL_ARRAY:
        return visitorObj((bsl::vector<bool> *) array);
      case bdem_ElemType::BDEM_DATETIMETZ_ARRAY:
        return visitorObj((bsl::vector<bdet_DatetimeTz> *) array);
      case bdem_ElemType::BDEM_DATETZ_ARRAY:
        return visitorObj((bsl::vector<bdet_DateTz> *) array);
      case bdem_ElemType::BDEM_TIMETZ_ARRAY:
        return visitorObj((bsl::vector<bdet_TimeTz> *) array);
      default:
        return bcem_ErrorCode::BCEM_NOT_AN_ARRAY;
    }
}


// ============================================================================
//           'bdeat_choicefunctions' overloads and specializations
// ============================================================================

namespace bdeat_ChoiceFunctions {

    // TYPES
    template <>
    struct IsChoice<bcem_AggregateRaw> {
        enum { VALUE = 1 };
    };

}  // close namespace bdeat_ChoiceFunctions

inline
bool bdeat_choiceHasSelection(const bcem_AggregateRaw& object,
                              int                      selectionId)
{
    return object.hasFieldById(selectionId);
}

bool bdeat_choiceHasSelection(const bcem_AggregateRaw&  object,
                              const char               *selectionName,
                              int                       selectionNameLength);
    // Return 'true' if the specified 'object' has a field having the specified
    // 'selectionName' of the specified 'selectionNameLength' and 'false'
    // otherwise.  The behavior is undefined unless '0 < selectionNameLength'
    // and 'selectionName'

int bdeat_choiceMakeSelection(bcem_AggregateRaw *object,
                              int                selectionId);
    // Make the field specified by 'selectionId' as the selection in the
    // specified 'object'.  Return 0 on success and a non-zero value otherwise.

int bdeat_choiceMakeSelection(bcem_AggregateRaw  *object,
                              const char         *selectionName,
                              int                 selectionNameLength);
    // Make the field specified by 'selectionName' of the specified
    // 'selectionNameLength' as the selection in the specified 'object'.
    // Return 0 on success and a non-zero value otherwise.

template <typename MANIPULATOR>
inline
int bdeat_choiceManipulateSelection(bcem_AggregateRaw *object,
                                    MANIPULATOR&       manipulator)
    // Use the specified 'manipulator' to manipulate the selection in the
    // specified 'object'.  Return 0 on success and a non-zero value otherwise.
{
    int fieldIndex = object->selectorIndex();
    if (fieldIndex < 0) {
        return -1;                                                    // RETURN
    }

    return bcem_AggregateRaw_BdeatUtil::manipulateField(object,
                                                        manipulator,
                                                        fieldIndex);
}

template <typename ACCESSOR>
inline
int bdeat_choiceAccessSelection(const bcem_AggregateRaw& object,
                                ACCESSOR&             accessor)
    // Use the specified 'accessor' to access the selection in the specified
    // 'object'.  Return 0 on success and a non-zero value otherwise.
{
    int fieldIndex = object.selectorIndex();
    if (fieldIndex < 0) {
        return -1;                                                    // RETURN
    }

    return bcem_AggregateRaw_BdeatUtil::accessField(object,
                                                    accessor,
                                                    fieldIndex);
}

inline
int bdeat_choiceSelectionId(const bcem_AggregateRaw& object)
    // Return the selection id corresponding to the selection of the specified
    // 'object' or 'bdeat_ChoiceFunctions::BDEAT_UNDEFINED_SELECTION_ID' if an
    // element is not selected.
{
    int selectorId = object.selectorId();

    return bdem_RecordDef::BDEM_NULL_FIELD_ID == selectorId
         ? bdeat_ChoiceFunctions::BDEAT_UNDEFINED_SELECTION_ID
         : selectorId;
}

// ============================================================================
//           'bdeat_arrayfunctions' overloads and specializations
// ============================================================================

namespace bdeat_ArrayFunctions {

    // TYPES
    template <>
    struct IsArray<bcem_AggregateRaw> {
        enum { VALUE = 1 };
    };

    template <>
    struct ElementType<bcem_AggregateRaw> {
        typedef bcem_AggregateRaw Type;
    };

}  // close namespace bdeat_ArrayFunctions

inline
void bdeat_arrayResize(bcem_AggregateRaw *array, int newSize)
    // Resize the specified 'array' to have the specified 'newSize' elements.
    // The behavior is undefined unless 'array' refers to an array type.
{
    bcem_ErrorAttributes dummy;
    int rc = array->resize(&dummy, newSize);
    (void) rc; // avoid compiler warning in non-safe mode
}

inline
bsl::size_t bdeat_arraySize(const bcem_AggregateRaw& array)
    // Return the number of elements in the specified 'array'.  The behavior is
    // undefined unless 'array' refers to an array type.
{
    return array.length();
}

template <typename ACCESSOR>
int bdeat_arrayAccessElement(const bcem_AggregateRaw& array,
                             ACCESSOR&                accessor,
                             int                      index)
    // Use the specified 'accessor' to access the element in the specified
    // 'array' at the specified 'index'.  Return 0 on success and a non-zero
    // value otherwise.
{
    bcem_AggregateRaw   element;
    bcem_ErrorAttributes dummy;
    if (0 != array.getArrayItem(&element, &dummy, index)) {
        return -1;                                                    // RETURN
    }

    const bdem_FieldDef *fieldDef = array.fieldDef();
    if (fieldDef
     && fieldDef->formattingMode() & bdeat_FormattingMode::BDEAT_NILLABLE) {
        bcem_AggregateRaw_BdeatUtil::NullableAdapter adapter = { &element };
        return accessor(adapter);                                     // RETURN
    }
    else {
        return accessor(element);                                     // RETURN
    }
}

template <typename MANIPULATOR>
int bdeat_arrayManipulateElement(bcem_AggregateRaw *array,
                                 MANIPULATOR&       manipulator,
                                 int                index)
    // Use the specified 'manipulator' to manipulate the element in the
    // specified 'array' at the specified 'index'.  Return 0 on success and a
    // non-zero value otherwise.
{
    bcem_AggregateRaw    element;
    bcem_ErrorAttributes dummy;
    if (0 != array->getArrayItem(&element, &dummy, index)) {
        return -1;                                                    // RETURN
    }

    const bdem_FieldDef *fieldDef = array->fieldDef();
    if (fieldDef
     && fieldDef->formattingMode() & bdeat_FormattingMode::BDEAT_NILLABLE) {
        bcem_AggregateRaw_BdeatUtil::NullableAdapter adapter = { &element };
        return manipulator(&adapter);                                 // RETURN
    }
    else {
        return manipulator(&element);                                 // RETURN
    }
}

// ============================================================================
//           'bdeat_enumfunctions' overloads and specializations
// ============================================================================

namespace bdeat_EnumFunctions {

    // TYPES
    template <>
    struct IsEnumeration<bcem_AggregateRaw> {
        enum { VALUE = 1 };
    };
}  // close namespace bdeat_EnumFunctions

int bdeat_enumFromString(bcem_AggregateRaw *result,
                         const char        *string,
                         int                stringLength);
    // Load into the specified 'result' the enumerator value corresponding to
    // the specified 'string' of the specified 'stringLength'.  Return 0 on
    // success and a non-zero value otherwise.  Note that if 'result' refers to
    // a 'bdem_ElemType::BDEM_STRING' then the value of 'result' would be equal
    // to the string having the value 'string' of 'stringLength' and if
    // 'result' refers to a 'bdem_ElemType::BDEM_INT' then the value of
    // 'result' would be equal to the integer enumerator value corresponding to
    // 'string'.

int bdeat_enumFromInt(bcem_AggregateRaw *result, int enumId);
    // Load into the specified 'result' the enumerator value corresponding to
    // the specified 'enumId'.  Return 0 on success and a non-zero value
    // otherwise.  Note that if 'result' refers to a
    // 'bdem_ElemType::BDEM_STRING' then the value of 'result' would be equal
    // to the string enumerator value corresponding to 'enumId' and if 'result'
    // refers to a 'bdem_ElemType::BDEM_INT' then the value of 'result' would
    // be equal to 'enumId'.

inline
void bdeat_enumToInt(int *result, const bcem_AggregateRaw& value)
    // Load into the specified 'result' the integer enumerator value
    // corresponding to the specified 'value' or an error aggregate on error.
{
    const bdem_EnumerationDef *enumDef = value.enumerationConstraint();
    if (! enumDef) {
        BSLS_ASSERT_OPT(!"Schema Error");
        *result = bdetu_Unset<int>::unsetValue();
        return;                                                       // RETURN
    }

    *result = value.asInt();
}

inline
void bdeat_enumToString(bsl::string *result, const bcem_AggregateRaw& value)
    // Load into the specified 'result' the string enumerator value
    // corresponding to the specified 'value' or an error aggregate on error.
{
    const bdem_EnumerationDef *enumDef = value.enumerationConstraint();
    if (! enumDef) {
        return;                                                       // RETURN
    }

    *result = value.asString();
}

// ============================================================================
//                       'bdeat_typename' overloads
// ============================================================================

const char *bdeat_TypeName_className(const bcem_AggregateRaw& object);
     // Return the name of the record or enumeration definition for the
     // specified 'object' aggregate or a null pointer of 'object' does not
     // have a named record or enumeration definition.

// ============================================================================
//                       'bdeat_valuetype' overloads
// ============================================================================

void bdeat_valueTypeReset(bcem_AggregateRaw *object);
    // Reset the specified 'object' to its default-constructed state.

inline
void bdeat_valueTypeReset(bcem_AggregateRaw_BdeatUtil::NullableAdapter *object)
    // Reset the specified 'object' to its default-constructed (null) state.
{
    BSLS_ASSERT_SAFE(object);
    BSLS_ASSERT_SAFE(object->d_element_p);

    object->d_element_p->makeNull();
}

// ============================================================================
//           'bdeat_sequencefunctions' overloads and specializations
// ============================================================================

namespace bdeat_SequenceFunctions {
    // META-FUNCTIONS
    bslmf_MetaInt<1> isSequenceMetaFunction(const bcem_AggregateRaw&);

    template <>
    struct IsSequence<bcem_AggregateRaw> {
        enum { VALUE = 1 };
    };
}  // close namespace bdeat_SequenceFunctions

template <typename MANIPULATOR>
int bdeat_sequenceManipulateAttribute(bcem_AggregateRaw *object,
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

    int fieldIndex = bcem_AggregateRaw_BdeatUtil::fieldIndexFromName(
                                                   *object->recordConstraint(),
                                                   attributeName,
                                                   attributeNameLength);

    if (fieldIndex < 0) {
        return -1;                                                    // RETURN
    }

    return bcem_AggregateRaw_BdeatUtil::manipulateField(object,
                                                        manipulator,
                                                        fieldIndex);
}

template <typename MANIPULATOR>
int bdeat_sequenceManipulateAttribute(bcem_AggregateRaw *object,
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

    return bcem_AggregateRaw_BdeatUtil::manipulateField(object,
                                                        manipulator,
                                                        fieldIndex);
}

template <typename MANIPULATOR>
int bdeat_sequenceManipulateAttributes(bcem_AggregateRaw *object,
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
        ret = bcem_AggregateRaw_BdeatUtil::manipulateField(object,
                                                           manipulator,
                                                           index);
    }

    return ret;
}

template <typename ACCESSOR>
int bdeat_sequenceAccessAttribute(
                                 const bcem_AggregateRaw&  object,
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

    int fieldIndex = bcem_AggregateRaw_BdeatUtil::fieldIndexFromName(
                                                    *object.recordConstraint(),
                                                    attributeName,
                                                    attributeNameLength);

    if (fieldIndex < 0) {
        return -1;                                                    // RETURN
    }

    return bcem_AggregateRaw_BdeatUtil::accessField(object,
                                                    accessor,
                                                    fieldIndex);
}

template <typename ACCESSOR>
int bdeat_sequenceAccessAttribute(const bcem_AggregateRaw& object,
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

    return bcem_AggregateRaw_BdeatUtil::accessField(object,
                                                    accessor,
                                                    fieldIndex);
}

template <typename ACCESSOR>
int bdeat_sequenceAccessAttributes(const bcem_AggregateRaw& object,
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
        ret = bcem_AggregateRaw_BdeatUtil::accessField(object,
                                                       accessor,
                                                       index);
    }

    return ret;
}

inline
bool bdeat_sequenceHasAttribute(const bcem_AggregateRaw&  object,
                                const char               *attributeName,
                                int                       attributeNameLength)
    // Return 'true' if the specified 'object' has a field having the specified
    // 'attributeName' of the specified 'attributeNameLength' and 'false'
    // otherwise.
{
    if (! object.recordConstraint()) {
        return false;                                                 // RETURN
    }

    return 0 <= bcem_AggregateRaw_BdeatUtil::fieldIndexFromName(
                                                    *object.recordConstraint(),
                                                    attributeName,
                                                    attributeNameLength);
}

inline
bool bdeat_sequenceHasAttribute(const bcem_AggregateRaw& object,
                                int                      attributeId)
    // Return 'true' if the specified 'object' has a field having the specified
    // 'attributeId' and 'false' otherwise.
{
    return object.hasFieldById(attributeId);
}


// ============================================================================
//           'bdeat_nullablevaluefunctions' overloads and specializations
// ============================================================================

namespace bdeat_NullableValueFunctions {

    // TYPES
    template <>
    struct IsNullableValue<bcem_AggregateRaw_BdeatUtil::NullableAdapter> {
        enum { VALUE = 1 };
    };

    template <>
    struct ValueType<bcem_AggregateRaw_BdeatUtil::NullableAdapter> {
        typedef bcem_AggregateRaw Type;
    };

}  // close namespace bdeat_NullableValueFunctions

inline
bool bdeat_nullableValueIsNull(
                    const bcem_AggregateRaw_BdeatUtil::NullableAdapter& object)
    // Return 'true' if the specified 'object' is null and 'false' otherwise.
{
    return object.d_element_p->isNull();
}

inline
void bdeat_nullableValueMakeValue(
                          bcem_AggregateRaw_BdeatUtil::NullableAdapter *object)
    // Make the value of the specified 'object' by resetting its null bit and
    // setting its value to its unset value.
{
    object->d_element_p->makeValue();
}

template <typename MANIPULATOR>
inline
int bdeat_nullableValueManipulateValue(
                     bcem_AggregateRaw_BdeatUtil::NullableAdapter *object,
                     MANIPULATOR&                                  manipulator)
    // Use the specified 'manipulator' to change the value of the specified
    // nullable 'object'.  Return 0 on success and a non-zero value otherwise.
{
    return manipulator(object->d_element_p);
}

template <typename ACCESSOR>
inline
int bdeat_nullableValueAccessValue(
                  const bcem_AggregateRaw_BdeatUtil::NullableAdapter& object,
                  ACCESSOR&                                           accessor)
    // Use the specified 'accessor' to access the value of the specified
    // nullable 'object'.  Return 0 on success and a non-zero value otherwise.
{
    return accessor(*object.d_element_p);
}

// ============================================================================
//                     'bdeat_typecategory' overloads
// ============================================================================

bdeat_TypeCategory::Value
bdeat_typeCategorySelect(const bcem_AggregateRaw& object);
    // Return the actual run-time category for the specified 'object'.  For
    // example, if 'object' contains an 'INT', return 'SIMPLE_CATEGORY'; if
    // 'object' contains a 'LIST', return 'SEQUENCE_CATEGORY', etc.

template <typename ACCESSOR>
int bdeat_typeCategoryAccessArray(const bcem_AggregateRaw& object,
                                  ACCESSOR&                accessor)
    // Use the specified 'accessor' to access the value of the specified array
    // 'object'.  Return 0 on success and a non-zero value otherwise.
{
    // The 'bdeat' framework treats 'bsl::vector<char>' as a native type
    // for 'base64Binary' and 'hexBinary' schema types.

    typedef bdeat_TypeCategory::Array Tag;

    if (bdem_ElemType::BDEM_CHAR_ARRAY == object.dataType()) {
        return accessor(object.asElemRef().theCharArray(), Tag());
    }
    if (bdem_ElemType::isArrayType(object.dataType())) {
        return accessor(object, Tag());
    }
    return accessor(object, bslmf_Nil());
}

template <typename MANIPULATOR>
int bdeat_typeCategoryManipulateArray(bcem_AggregateRaw *object,
                                      MANIPULATOR&       manipulator)
    // Use the specified 'manipulator' to change the value of the specified
    // array 'object'.  Return 0 on success and a non-zero value otherwise.
{
    // The 'bdeat' framework treats 'bsl::vector<char>' as a native type
    // for 'base64Binary' and 'hexBinary' schema types.

    typedef bdeat_TypeCategory::Array Tag;

    if (bdem_ElemType::BDEM_CHAR_ARRAY == object->dataType()) {
        return manipulator(&object->asElemRef().theModifiableCharArray(),
                           Tag());
    }
    if (bdem_ElemType::isArrayType(object->dataType())) {
        return manipulator(object, Tag());
    }

    return manipulator(object, bslmf_Nil());
}

template <typename MANIPULATOR>
int bdeat_typeCategoryManipulateSimple(bcem_AggregateRaw *object,
                                       MANIPULATOR&       manipulator)
    // Use the specified 'manipulator' to change the value of the specified
    // simple 'object'.  Return 0 on success and a non-zero value otherwise.
{
    typedef bdeat_TypeCategory::Simple Tag;

    int result;

    switch (object->dataType()) {
      case bdem_ElemType::BDEM_CHAR: {
        result = manipulator(&object->asElemRef().theModifiableChar(), Tag());
      } break;
      case bdem_ElemType::BDEM_SHORT: {
        result = manipulator(&object->asElemRef().theModifiableShort(), Tag());
      } break;
      case bdem_ElemType::BDEM_INT: {
        result = manipulator(&object->asElemRef().theModifiableInt(), Tag());
      } break;
      case bdem_ElemType::BDEM_INT64: {
        result = manipulator(&object->asElemRef().theModifiableInt64(), Tag());
      } break;
      case bdem_ElemType::BDEM_FLOAT: {
        result = manipulator(&object->asElemRef().theModifiableFloat(), Tag());
      } break;
      case bdem_ElemType::BDEM_DOUBLE: {
        result = manipulator(&object->asElemRef().theModifiableDouble(),
                             Tag());
      } break;
      case bdem_ElemType::BDEM_STRING: {
        result = manipulator(&object->asElemRef().theModifiableString(),
                             Tag());
      } break;
      case bdem_ElemType::BDEM_DATETIME: {
        result = manipulator(&object->asElemRef().theModifiableDatetime(),
                             Tag());
      } break;
      case bdem_ElemType::BDEM_DATE: {
        result = manipulator(&object->asElemRef().theModifiableDate(), Tag());
      } break;
      case bdem_ElemType::BDEM_TIME: {
        result = manipulator(&object->asElemRef().theModifiableTime(), Tag());
      } break;
      case bdem_ElemType::BDEM_BOOL: {
        result = manipulator(&object->asElemRef().theModifiableBool(), Tag());
      } break;
      case bdem_ElemType::BDEM_DATETIMETZ: {
        result = manipulator(&object->asElemRef().theModifiableDatetimeTz(),
                             Tag());
      } break;
      case bdem_ElemType::BDEM_DATETZ: {
        result = manipulator(&object->asElemRef().theModifiableDateTz(),
                             Tag());
      } break;
      case bdem_ElemType::BDEM_TIMETZ: {
        result = manipulator(&object->asElemRef().theModifiableTimeTz(),
                             Tag());
      } break;
      default: {
        result = manipulator(object, bslmf_Nil());
      } break;
    }

    return result;
}

template <typename ACCESSOR>
int bdeat_typeCategoryAccessSimple(const bcem_AggregateRaw& object,
                                   ACCESSOR&                accessor)
    // Use the specified 'accessor' to access the value of the specified simple
    // 'object'.  Return 0 on success and a non-zero value otherwise.
{
    typedef bdeat_TypeCategory::Simple Tag;

    int result;

    switch (object.dataType()) {
      case bdem_ElemType::BDEM_CHAR: {
        result = accessor(object.asElemRef().theChar(), Tag());
      } break;
      case bdem_ElemType::BDEM_SHORT: {
        result = accessor(object.asElemRef().theShort(), Tag());
      } break;
      case bdem_ElemType::BDEM_INT: {
        result = accessor(object.asElemRef().theInt(), Tag());
      } break;
      case bdem_ElemType::BDEM_INT64: {
        result = accessor(object.asElemRef().theInt64(), Tag());
      } break;
      case bdem_ElemType::BDEM_FLOAT: {
        result = accessor(object.asElemRef().theFloat(), Tag());
      } break;
      case bdem_ElemType::BDEM_DOUBLE: {
        result = accessor(object.asElemRef().theDouble(), Tag());
      } break;
      case bdem_ElemType::BDEM_STRING: {
        result = accessor(object.asElemRef().theString(), Tag());
      } break;
      case bdem_ElemType::BDEM_DATETIME: {
        result = accessor(object.asElemRef().theDatetime(), Tag());
      } break;
      case bdem_ElemType::BDEM_DATE: {
        result = accessor(object.asElemRef().theDate(), Tag());
      } break;
      case bdem_ElemType::BDEM_TIME: {
        result = accessor(object.asElemRef().theTime(), Tag());
      } break;
      case bdem_ElemType::BDEM_BOOL: {
        result = accessor(object.asElemRef().theBool(), Tag());
      } break;
      case bdem_ElemType::BDEM_DATETIMETZ: {
        result = accessor(object.asElemRef().theDatetimeTz(), Tag());
      } break;
      case bdem_ElemType::BDEM_DATETZ: {
        result = accessor(object.asElemRef().theDateTz(), Tag());
      } break;
      case bdem_ElemType::BDEM_TIMETZ: {
        result = accessor(object.asElemRef().theTimeTz(), Tag());
      } break;
      default: {
        result = accessor(object, bslmf_Nil());
      } break;
    }

    return result;
}

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2011
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
