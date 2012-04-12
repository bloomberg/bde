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
//   bcem_AggregateRawNameOrIndex: simple discriminated union of string, index
//
//@SEE_ALSO: bdem package; bcem_Aggregate
//
//@AUTHOR: David Schumann (dschumann1)
//
//@DESCRIPTION: This component provides a class representing dynamically
// typed, introspective data optionally described by a 'bdem_Schema'.  The 
// type is similar to a "raw" (non-reference-counted) pointer in the sense that
// multiple 'bcem_AggregateRaw' objects can refer to the same data and they
// do not dispose of that data when they go out of scope.  
//
// Thus 'bcem_AggregateRaw' is provided to allow efficient implementation
// of operations such as "visiting" the fields of a large object recursively
// (that may be inefficient on a reference counted aggregate, see 
// 'bcem_Aggregate').  Typically during such operations, a single aggregate
// object representing the top-level value remains in scope, and 
// many sub-aggregate objects representing fields in that value are created
// and destroyed on the stack.  The atomic operations required to maintain
// the reference count would be wasted in this scenario, as the count never
// reaches zero and when the operation is concluded there is no net change 
// in the reference count.  Using 'bcem_AggregateRaw' to represent the 
// sub-elements of the object will eliminate this expense.  
//
///Data Representation
///-------------------
// 'bcem_AggregateRaw' has a 3-tuple of pointers: the address of 
// a 'bdem_Schema' describing the structure of the data; a void* pointing
// at the actual data; and the address of a control word holding a "top-level
// nullness bit."  When an Aggregate object contains a nullable element, and
// that element is null, that information is stored in the parent element.  
// But for top-level objects, those not contained within other objects, the 
// "nullness" of the object must be represented somewhere; the 
// "top-level nullness bit" is used for this purpose.  See the 'bdem_ElemRef'
// component documentation, "Element Reference Nullability", for a discussion
// of nullness control words (described there as "bitmaps").  
//
// 'bcem_AggregateRaw' also holds information referring to the parent element.
// Like the schema, data, and nullness data, this reference is uncounted and
// thus the parent must remain valid for the lifetime of the 
// 'bcem_AggregateRaw' object.  
//
///Error Handling
///--------------
// 'bcem_AggregateRaw' returns descriptive errors when possible.  These
// consist of an enumerated value of 'bcem_AggregateError::Code', 
// combined with a human-readable 'string' value.   In general, most 
// methods of 'bcem_AggregateRaw' will return an arbitrary nonzero value 
// on failure and populate a 'bcem_AggregateError' (passed by pointer as an
// output parameter) with details.  See the documentation of
// 'bcem_AggregateError' for the definition of the enumerated error conditions.
//
///Thread Safety
///-------------
// A 'bcem_AggregateRaw' maintains a non-reference-counted handle to
// possibly-shared data. It is not safe to access or modify this shared 
// data concurrently from different threads, therefore 'bcem_AggregateRaw' is, 
// strictly speaking, *thread* *unsafe* as two aggregates may refer to the 
// same shared data.  However, it is safe to concurrently access or modify 
// two different 'bcem_AggregateRaw' objects refering to different data.  
//
///Usage 
///-----
//
// Using 'bcem_AggregateRaw' typically involves starting with a 
// 'bcem_Aggregate' object and extracting 'bcem_AggregateRaw' from it, 
// then working with that in place of the original aggregate.  In this example,
// we elide the initial population of the 'bcem_AggregateRaw' object and 
// instead focus on iterating through fields in the object.
//..
//  void printFields(const bcem_AggregateRaw& object) 
//  {
//     for (int i = 0; i < object.length(); ++i) {
//        bcem_AggregateRaw field;
//        bcem_AggregateError error;
//        if (0 == object.fieldByIndex(&field, &error, i)) {
//            field.print(bsl::cout, 0, -1);
//        }
//     }
//  }
//..

#ifndef INCLUDED_BCESCM_VERSION
#include <bcescm_version.h>
#endif

#ifndef INCLUDED_BCEM_AGGREGATEERROR
#include <bcem_aggregateerror.h>
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

#ifndef INCLUDED_BDEM_SELECTBDEMTYPE
#include <bdem_selectbdemtype.h>
#endif

#ifndef INCLUDED_BDEM_SCHEMA
#include <bdem_schema.h>
#endif

#ifndef INCLUDED_BDEM_SCHEMAUTIL
#include <bdem_schemautil.h>
#endif

#ifndef INCLUDED_BDEM_TABLE
#include <bdem_table.h>
#endif

#ifndef INCLUDED_BDETU_UNSET
#include <bdetu_unset.h>
#endif

#ifndef INCLUDED_BDEF_FUNCTION
#include <bdef_function.h>
#endif

#ifndef INCLUDED_BSL_UTILITY
#include <bsl_utility.h>
#endif

#ifndef INCLUDED_BSL_SSTREAM
#include <bsl_sstream.h>
#endif

namespace BloombergLP {

struct bcem_AggregateRaw_BdeatUtil;

                        // ===================================
                        // class bcem_AggregateRawNameOrIndex
                        // ===================================

class bcem_AggregateRawNameOrIndex {
    // This class holds a *temporary* name string, an integer index, or neither
    // (the "empty" state).  It has conversion constructors
    // from 'bsl::string', 'const char *', and 'int'.  It does not own its
    // string data.  This class has in-core value semantics, except that it
    // lacks printing support.

    enum {
        // Select name, index, or neither ('BCEM_NOI_EMPTY').
        BCEM_NOI_EMPTY,
        BCEM_NOI_NAME,
        BCEM_NOI_INDEX
    } d_state;

    union {
        const char *d_name;   // set if 'BCEM_NOI_NAME' (held, *not* owned)
        int         d_index;  // set if 'BCEM_NOI_INDEX'
    };

  public:
    // CREATORS
    bcem_AggregateRawNameOrIndex();
        // Create an empty name/index having no name or index.

    bcem_AggregateRawNameOrIndex(const char *name);
        // Create a name/index having the specified 'name'.  The behavior is
        // undefined unless the character array pointed to by 'name' is
        // unchanged and remains valid during the lifetime of this object.

    bcem_AggregateRawNameOrIndex(const bsl::string& name);
        // Create a name/index having the specified 'name'.  The behavior is
        // undefined unless 'name' is unchanged during the lifetime of this
        // object.  Note that this method, allowing implicit conversion 
        // of bsl::string to 'bcem_AggregateRawNameOrIndex', is necessary 
        // for the usability of public functions in the interfaces of 
        // 'bcem_Aggregate' and 'bcem_AggregateRaw' that allow the caller
        // to specify any of int, string literal, or bsl::string.  

    bcem_AggregateRawNameOrIndex(int index);
        // Create a name/index having the specified 'index'.  The behavior is
        // undefined unless 'index >= 0'.

    // ACCESSORS
    bool isEmpty() const;
        // Return 'true' if this is an empty name/index, and 'false' otherwise.
        // An empty name/index has neither a name nor an index.

    bool isName() const;
        // Return 'true' if this name/index has a name, and 'false' otherwise.

    bool isIndex() const;
        // Return 'true' if this name/index has an integral index, and 'false'
        // otherwise.

    const char *name() const;
        // Return the name of this name/index if 'isName() == true', and 0
        // otherwise.

    int index() const;
        // Return the integral index of this name/index if 'isIndex() == true',
        // and 'bdem_RecordDef::BDEM_NULL_FIELD_ID' otherwise.
};


                   // ========================================
                   // local class bcem_AggregateRaw_ArrayInserter
                   // ========================================

class bcem_AggregateRaw_ArrayInserter {
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
            IS_SIGNED=1
        };
    };

    // NOT IMPLEMENTED
    bcem_AggregateRaw_ArrayInserter(const bcem_AggregateRaw_ArrayInserter&);
    bcem_AggregateRaw_ArrayInserter& operator=(
                                 const bcem_AggregateRaw_ArrayInserter&);

  public:
    // CREATORS
    bcem_AggregateRaw_ArrayInserter(
                                 int                  index,
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
struct bcem_AggregateRaw_ArrayInserter::SignChecker<unsigned char> {
    // TBD REMOVE
        enum {
            IS_SIGNED=0
        };
};

template <>
struct bcem_AggregateRaw_ArrayInserter::SignChecker<unsigned short> {
    // TBD REMOVE
        enum {
            IS_SIGNED=0
        };
};
template <>
struct bcem_AggregateRaw_ArrayInserter::SignChecker<unsigned int> {
    // TBD REMOVE
        enum {
            IS_SIGNED=0
        };
};
template <>
struct bcem_AggregateRaw_ArrayInserter::SignChecker<bsls_Types::Uint64> {
    // TBD REMOVE
        enum {
            IS_SIGNED=0
        };
};
                      // ============================================
                      // local class bcem_AggregateRaw_ArrayCapacitor
                      // ============================================

class  bcem_AggregateRaw_ArrayCapacitor {
    // This class defines a functor that loads the capacity of a sequence 
    // container into a parameter passed in the constructor.  The capacity of 
    // a sequence container is the number of elements for which memory has
    // already been allocated.

    //DATA

    bsl::size_t *d_capacity_p;  // address of value into which to load capacity

  private:
    // NOT IMPLEMENTED
    bcem_AggregateRaw_ArrayCapacitor(const bcem_AggregateRaw_ArrayCapacitor&);
    bcem_AggregateRaw_ArrayCapacitor& operator=(
                                      const bcem_AggregateRaw_ArrayCapacitor&);
  public:
    // CREATOR
    explicit
    bcem_AggregateRaw_ArrayCapacitor(bsl::size_t *capacity)
    : d_capacity_p(capacity)
    {
    }

    // MANIPULATORS
    template <typename ARRAYTYPE>
    int operator()(ARRAYTYPE *array)
    {
        *d_capacity_p = array->capacity();
        return 0;
    }

};
                     // ==========================================
                     // local class bcem_AggregateRaw_ArrayIndexer
                     // ==========================================

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
        if ((unsigned)d_index < array->size()) {
            d_item_p = & (*array)[d_index];
            return 0;
        }
        else {
            d_item_p = 0;
            return -1;
        }
    }

    // ACCESSORS
    void *data() const
    {
        return d_item_p;
    }
};

                        // ========================================
                        // local class bcem_AggregateRaw_ArrayReserver
                        // ========================================

class bcem_AggregateRaw_ArrayReserver {
    // This class defines a function object to reserve memory in a sequence 
    // container for the number of objects indicated at construction.

    // DATA
    bsl::size_t d_numItems; // number of items to reserve

    // NOT IMPLEMENTED
    bcem_AggregateRaw_ArrayReserver(const bcem_AggregateRaw_ArrayReserver&);
    bcem_AggregateRaw_ArrayReserver& operator=(
                                       const bcem_AggregateRaw_ArrayReserver&);

  public:
    // CREATORS
    explicit
    bcem_AggregateRaw_ArrayReserver(bsl::size_t numItems)
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

                       // =====================
                       // class ElemDataFetcher
                       // =====================
struct ElemDataFetcher {
    // This class accesses the address of a value held within a 'bdem_ElemRef'
    // or 'bdem_ConstElemRef' without affecting the nullness of the referenced
    // value.  
    
    void *d_data_p;

    explicit
    ElemDataFetcher(const bdem_ElemRef& elemRef) {
        d_data_p = elemRef.dataRaw();
    }

    explicit
    ElemDataFetcher(const bdem_ConstElemRef& elemRef) {
        d_data_p = const_cast<void*>(elemRef.data());
    }
    
};

                         // ================
                         // class ArraySizer
                         // ================

struct ArraySizer {
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
    // 'bcem_Aggregate' object. It can be used to efficiently implement 
    // operations, particularly recursive "visiting", where an aggregate 
    // remains in scope throughout the operation and there would otherwise be
    // many temporary aggregate objects created and discarded. 

  private:
    // PRIVATE TYPES
    typedef bcem_AggregateRawNameOrIndex NameOrIndex;  // short hand

    // Data invariants:
    // - If 'd_dataType' is 'bdem_ElemType::BDEM_VOID', then 'd_value' will be
    //   null or point to an error record.  The remaining invariants need not
    //   hold.
    // - If 'd_schema' is null, then both 'd_recordDef' and 'd_fieldDef' are
    //   null.
    // - If 'd_schema' is non-null, then 'd_recordDef' and/or 'd_fieldDef'
    //   are non-null.
    // - 'd_recordDef' is either null or points to a record within 'd_schema.'
    //   Its memory is not managed separately from the schema's.
    // - 'd_fieldDef' is either null or points to a field definition within
    //   'd_schema'.  Its memory is not managed separately from the schema's.
    // - If 'd_fieldDef' is not null, then 'd_fieldDef->elemType()' is equal
    //   to either 'd_dataType' or to 'bdem_ElemType::toArrayType(d_dataType)'.
    //   The code in this class always uses 'd_dataType', not
    //   'd_fieldDef->elemType()'.
    // - If this is the root object, then 'd_fieldDef' will be null, but
    //   'd_recordDef' may still have a value.  Otherwise,
    //   'd_fieldDef->recordConstraint()' will always be equal to
    //   'd_recordDef'.  The code in this class always uses 'd_recordDef', not
    //   'd_fieldDef->recordConstraint()'.

    // DATA
    bdem_ElemType::Type   d_dataType;       // value's type
    const bdem_Schema    *d_schema_p;       // shared schema 
    const bdem_RecordDef *d_recordDef;      // record constraint
    const bdem_FieldDef  *d_fieldDef;       // data description
    void                 *d_value_p;        // pointer to data
    bdem_ElemType::Type   d_parentType;     // type of parent of this aggregate
                                            // ('VOID' if top-level)

    void                 *d_parentData;     // address of owner; 0 if top-level
    int                   d_indexInParent;  // index into parent of this 
                                            // aggregate; -1 if top-level,
                                            // scalar, or vector

    int                  *d_isTopLevelAggregateNull_p;  
                                            // nullness indicator for
                                            // top-level aggregate in bit
                                            // 0

    // PRIVATE MANIPULATORS
    template <typename TYPE>
    int assignToNillableScalarArrayImp(const TYPE& value) const;
        // Assign the specified 'value' to this aggregate.  Return 0 on
        // success, and a non-zero value otherwise.  The behavior is undefined
        // unless this aggregate refers to a nillable scalar array, and 'value'
        // is a scalar array, or is convertible to one.  If value is null,
        // then make this aggregate null.  Leave this aggregate unchanged if
        // 'value' is not convertible to the type stored in this aggregate.
        // The parameterized 'TYPE' shall be either bdem_ElemRef or 
        // bdem_ConstElemRef.  

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

    int descendIntoArrayItem(bcem_AggregateError *errorDescription, 
                             int                  index, 
                             bool                 makeNonNullFlag);
        // Modify this aggregate so that it refers to the item at the
        // specified 'index' within the currently-referenced array (including
        // table and choice array).  If this aggregate references a 
        // nillable array, the element at 'index' is null, and the 
        // specified 'makeNonNullFlag' is 'true', assign that element its 
        // default value; otherwise leave the element unmodified.  Return 0 on
        // success, with no effect on the specified 'errorDescription'; 
        // otherwise, load into 'errorDescription' a description of the 
        // failure and return a nonzero value.  

    template <typename VALUETYPE>
    int toEnum(bcem_AggregateError *errorDescription, 
               const VALUETYPE&     value) const;
    template <typename VALUETYPE>
    int toEnum(bcem_AggregateError *errorDescription, 
               const VALUETYPE&     value,
               bslmf_MetaInt<0>     direct) const;
    int toEnum(bcem_AggregateError *errorDescription, 
               const int&, 
               bslmf_MetaInt<0>     direct) const;
    int toEnum(bcem_AggregateError *errorDescription, 
               const char          *value, 
               bslmf_MetaInt<1>     direct) const;
    int toEnum(bcem_AggregateError *errorDescription, 
               const bsl::string&   value,
               bslmf_MetaInt<1>     direct) const;
    int toEnum(bcem_AggregateError      *errorDescription, 
               const bdem_ConstElemRef&  value,
               bslmf_MetaInt<1>          direct) const;
        // Set this enumeration to the specified 'value'.  The 'direct' 
        // argument is to aid in template metaprogramming for overloading for 
        // those types that can be directly processed and those that must
        // first be converted to 'int' using 'bdem_Convert'.  Return 0 on
        // success or a nonzero value with a description loaded into the 
        // specified 'errorDescription' on failure.
    
    // PRIVATE ACCESSORS
    void convertScalarToString(bsl::string *result) const;
        // Convert the scalar value stored in this aggregate to a string, and
        // load the resulting string into the specified 'result', or make
        // 'result' the empty string if this aggregate holds a value that is 
        // not convertible to a string.  

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
        // 'other' remain valid for the lifetime of this object.  

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
        // specified 'rhs' aggregate and return a reference to this
        // aggregate.  This creates a new reference to existing data -- no
        // data is copied.  The behavior is undefined unless the schema and
        // data referred to by 'other' remain valid for the lifetime of this
        // object.  Note that if 'rhs' is an error aggregate, then this 
        // aggregate will be assigned the same error state as 'rhs'.  

    void clearParent();
        // Make this aggregate a top-level aggregate by resetting the 
        // parent information. 
    
    void setDataType(bdem_ElemType::Type dataType);
        // Set the type of data referenced by this "raw" aggregate to the 
        // specified 'dataType'. 

    void setDataPointer(void *data);
        // Set the data pointer for this "raw" aggregate to the specified
        // 'data'.  The behavior is undefined unless 'data' remains valid for
        // the lifetime of this object. 

    void setSchemaPointer(const bdem_Schema *schema);
        // Set the schema pointer for this "raw" aggregate to the specified
        // 'schema'.  The behavior is undefined unless 'schema' remains valid 
        // for the lifetime of this object. 

    void setRecordDefPointer(const bdem_RecordDef* recordDef);
        // Set the record definition poiner for this "raw" aggregate to the
        // specified 'recordDef'.  The behavior is undefined unless 'recordDef'
        // remains valid for the lifetime of this object. 

    void setFieldDefPointer(const bdem_FieldDef* fieldDef);
        // Set the field definition pointer for this "raw" aggregate to the
        // specified 'fieldDef'.  The behavior is undefined unless 'fieldDef'
        // remains valid for the lifetime of this object. 

    void setTopLevelAggregateNullnessPointer(int *nullnessFlag);
        // Set the address of the top-level nullness bit for this
        // "raw" aggregate to the specified 'nullnessFlag'.  See "Data 
        // Representation" in the component-level documentation for a 
        // description of the top-level nullness bit.  

    int reserveRaw(bcem_AggregateError *errorDescription, 
                   bsl::size_t          numItems);
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

    void reset();
        // Return this object to the empty state.  

    // ACCESSORS
    bool isError() const;
        // Return 'true' if this object was returned from a function that
        // detected an error.  If this function returns 'true', then
        // 'dataType()' will return 'bdem_ElemType::BDEM_VOID', 'errorCode()'
        // will return a non-zero value, and 'errorMessage()' will return a
        // non-empty string.

    bool isVoid() const;
        // Return 'true' if 'dataType() == bdem_ElemType::BDEM_VOID'.  

    bool isNull() const;
        // Return 'true' if the data referenced by this aggregate has a null
        // value, and 'false' otherwise.  

    bool isNullable() const;
        // Return 'true' if the data referenced by this aggregate can be made
        // null, and 'false' otherwise.

    int errorCode() const;
        // Return a negative error code describing the status of this
        // object if 'isError()' is 'true', or zero if 'isError()' is 'false'.
        // A set of error code constants with names beginning with 'BCEM_ERR_'
        // are described in the 'bcem_aggregate' component-level
        // documentation.

    bsl::string errorMessage() const;
        // Return a string describing the error state of this object of
        // 'isError()' is 'true', or an empty string if 'isError()' is 'false'.
        // The contents of the string are intended to be human readable and
        // descriptive.  The exact format of the string may change at any time
        // and should not be relied on in a program (use 'errorCode()',
        // instead).

    bsl::string asString() const;
        // Return a text representation of the value referenced by this
        // aggregate.  For enumeration values, the resulting string is the
        // enumerator name corresponding to the referenced value.  For date
        // and time values, the resulting string will have iso8601 format.
        // For other types, the resulting string will have the same format as
        // 'ostream' printing of the underlying data value.  

    void loadAsString(bsl::string *result) const;
        // Load into the specified 'result' string a text representation of
        // the value referenced by this aggregate, as returned by
        // 'asString()'.

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

    const bdem_ElemRef asElemRef() const;
        // Return a reference to the modifiable element value held by this
        // aggregate.

    bool hasField(const char *fieldName) const;
        // Return 'true' if this aggregate contains a field having the
        // specified 'fieldName' and 'false' otherwise.

    bool hasFieldById(int fieldId) const;
        // Return 'true' if this aggregate contains a field having the
        // specified 'fieldId' and 'false' otherwise.

    bool hasFieldByIndex(int fieldIndex) const;
        // Return 'true' if this aggregate contains a field having the
        // specified 'fieldIndex' and 'false' otherwise.

    int anonymousField(bcem_AggregateRaw    *object, 
                       bcem_AggregateError  *errorDescription, 
                       int                   index) const;
        // Load into the specified 'object' a representation of the 
        // the field with a null name (i.e., an anonymous field) within this 
        // object corresponding to the specified 'index'.  'index' is the 
        // index of the field within the anonymous fields of this object, not
        // within all the fields of this object.  If there are not at least 
        // 'index+1' anonymous fields in this object, load a descriptive error
        // into 'errorDescription' and return a nonzero value; otherwise return
        // 0 with no effect on 'errorDescription'. 

    int anonymousField(bcem_AggregateRaw *object) const;
        // If this aggregate contains exactly one field with a null name
        // (i.e., an anonymous field), then load into the specified 'object' 
        // an aggregate representing that field and return 0; otherwise return
        // a nonzero value.  An anonymous field is a field with with a null
        // name. 

    int descendIntoField(bcem_AggregateError   *errorDescription, 
                         const NameOrIndex&     fieldOrIdx, 
                         bool                   makeNonNullFlag);
        // Modify this aggregate so that it refers to the field with
        // the specified 'fieldOrIdx' within the currently-referenced 
        // sequence or choice object.  If this aggregate references a nillable
        // array, the element having 'fieldOrIdx' is null, and the 
        // specified 'makeNonNullFlag' is 'true', assign that element 
        // its default value; otherwise leave the element unmodified.  Return 0
        // on success, with no effect on the specified 'errorDescription'; 
        // otherwise, load into 'errorDescription' a description of the 
        // failure and return a nonzero value.

    int descendIntoFieldByName(bcem_AggregateError *errorDescription, 
                               const char          *name);
        // Modify this aggregate so that it refers to the field with
        // the specified 'name' within the currently-referenced 
        // sequence or choice object.  Return 0 on success, with no effect on
        // the specified 'errorDescription'; otherwise, load into
        // 'errorDescription' a description of the failure and return a 
        // nonzero value.

    int descendIntoFieldById(bcem_AggregateError *errorDescription, 
                             int                  id);
        // Modify this aggregate so that it refers to the field with
        // the specified 'id' within the currently-referenced 
        // sequence or choice object.  Return 0 on success, with no effect on
        // the specified 'errorDescription'; otherwise, load into
        // 'errorDescription' a description of the failure and return a 
        // nonzero value.

    int descendIntoFieldByIndex(bcem_AggregateError *errorDescription,
                                int                  index);
        // Modify this aggregate so that it refers to the field with
        // the specified 'index' within the currently-referenced 
        // sequence or choice object.  Return 0 on success, with no effect on
        // the specified 'errorDescription'; otherwise, load into
        // 'errorDescription' a description of the failure and return a 
        // nonzero value.

    int getField(bcem_AggregateRaw    *resultField, 
                 bcem_AggregateError  *errorDescription,
                 bool                  makeNonNullFlag,
                 NameOrIndex           fieldOrIdx1,
                 NameOrIndex           fieldOrIdx2 = NameOrIndex(),
                 NameOrIndex           fieldOrIdx3 = NameOrIndex(),
                 NameOrIndex           fieldOrIdx4 = NameOrIndex(),
                 NameOrIndex           fieldOrIdx5 = NameOrIndex(),
                 NameOrIndex           fieldOrIdx6 = NameOrIndex(),
                 NameOrIndex           fieldOrIdx7 = NameOrIndex(),
                 NameOrIndex           fieldOrIdx8 = NameOrIndex(),
                 NameOrIndex           fieldOrIdx9 = NameOrIndex(),
                 NameOrIndex           fieldOrIdx10= NameOrIndex()) const;
        // Navigate to the field within this aggregate reached through the
        // specified chain of two to ten 'fieldOrIdx' arguments, each of which
        // specifies a field name or array index.  If this aggregate references
        // a nillable array, the specified field is null, and the specified 
        // 'makeNonNullFlag' is 'true', assign that field its default value; 
        // otherwise leave the field unmodified.  On success, load into the
        // specified 'resultField' an uncounted reference to the specified 
        // field and return 0 (without any effect on the specified
        // 'errorDescription').  Otherwise, load an error description into
        // the 'errorDescription' and return a nonzero value.  Note that an 
        // empty string can be used for any of the 'fieldOrIdx' arguments to
        // specify the current selection within a choice object.  Note also 
        // that an unused argument results in the construction of a null 
        // 'NameOrIndex', which is treated as the end of the argument list.

    int getFieldIndex(int                 *index,
                      bcem_AggregateError *errorResult,
                      const char          *fieldName,
                      const char          *caller) const;
    int getFieldIndex(int                 *index,
                      bcem_AggregateError *errorResult,
                      int                  fieldId,
                      const char          *caller) const;
        // On behalf of the specified 'caller', load into the specified 'index'
        // the index of the field indicated by the specified 'fieldName' or
        // 'fieldId' in the record definition of this aggregate.  Return 0 on
        // success, with no effect on the specified 'errorDescription'; 
        // otherwise, load into 'errorDescription' a description of the 
        // failure (incorporating the value of 'caller') and return a nonzero 
        // value. 

    int findUnambiguousChoice(bcem_AggregateRaw   *choiceObject, 
                              bcem_AggregateError *errorDescription,
                              const char          *caller = "") const;
        // Load into the specified 'choiceObject' the value of this 
        // aggregate if it is a choice or choice array item, or else descend
        // (recursively) into any anonymous field of this aggregate looking
        // for an anonymous choice field; then load into 'choiceObject'
        // the unambiguous anonymous choice within this aggregate.  On failure
        // -- i.e., multiple or no anonymous choices were found -- load 
        // into the specified 'errorDescription' a description of the error
        // (incorporating the specified 'caller' description) and return a 
        // nonzero value; otherwise, return 0 with no effect on 
        // 'errorDescription'.

    int fieldByIndex(bcem_AggregateRaw   *field, 
                     bcem_AggregateError *errorDescription, 
                     int                  index) const;
        // Load into the specified 'field' object the field within this 
        // aggregate with the specified (zero-based) 'index'.  Return 0 on
        // success, with no effect on the specified 'errorDescription'; 
        // otherwise, load into 'errorDescription' a description of the 
        // failure and return a nonzero value.   

    int arrayItem(bcem_AggregateRaw   *item, 
                  bcem_AggregateError *errorDescription, 
                  int                  index) const;
        // Load into the specified 'item' an aggregate referring to 
        // the item at the specified 'index' within the currently-referenced
        // array.  Return 0 on success, with no effect on the specified
        // 'errorDescription'; otherwise, load into 'errorDescription' a
        // description of the failure and return a nonzero value.
    
    bdem_ElemType::Type
    fieldType(NameOrIndex fieldOrIdx1,
              NameOrIndex fieldOrIdx2  = NameOrIndex(),
              NameOrIndex fieldOrIdx3  = NameOrIndex(),
              NameOrIndex fieldOrIdx4  = NameOrIndex(),
              NameOrIndex fieldOrIdx5  = NameOrIndex(),
              NameOrIndex fieldOrIdx6  = NameOrIndex(),
              NameOrIndex fieldOrIdx7  = NameOrIndex(),
              NameOrIndex fieldOrIdx8  = NameOrIndex(),
              NameOrIndex fieldOrIdx9  = NameOrIndex(),
              NameOrIndex fieldOrIdx10 = NameOrIndex()) const;
        // Navigate to the field within this aggregate reached through the
        // specified chain of one to ten 'fieldOrIdx' arguments, each of which
        // specifies a field name or array index.  Return the type of the
        // field on success, or 'bdem_ElemType::BDEM_VOID' on a navigation
        // error.  An empty string can be used for any of the 'fieldOrIdx'
        // arguments to specify the current selection within a choice object.
        // An unused argument results in the construction of a null
        // 'NameOrIndex', which is treated as the end of the argument list.

    bdem_ElemType::Type fieldTypeById(int fieldId) const;
        // Return the type of field referenced by the specified 'fieldId' or
        // 'bdem_ElemType::BDEM_VOID' if 'fieldId' is not in this aggregate's
        // record definition.

    bdem_ElemType::Type fieldTypeByIndex(int index) const;
        // Return the type of field at the position specified by the
        // (zero-based) 'index' or 'bdem_ElemType::BDEM_VOID' if 'index < 0' or
        // 'recordDef().length() <= index'.

    bdem_ElemRef fieldRef(NameOrIndex fieldOrIdx1,
                          NameOrIndex fieldOrIdx2  = NameOrIndex(),
                          NameOrIndex fieldOrIdx3  = NameOrIndex(),
                          NameOrIndex fieldOrIdx4  = NameOrIndex(),
                          NameOrIndex fieldOrIdx5  = NameOrIndex(),
                          NameOrIndex fieldOrIdx6  = NameOrIndex(),
                          NameOrIndex fieldOrIdx7  = NameOrIndex(),
                          NameOrIndex fieldOrIdx8  = NameOrIndex(),
                          NameOrIndex fieldOrIdx9  = NameOrIndex(),
                          NameOrIndex fieldOrIdx10 = NameOrIndex()) const;
        // Navigate to the field within this aggregate reached through the
        // specified chain of one to ten 'fieldOrIdx' arguments, each of which
        // specifies a field name or array index.  Return an element reference
        // to the specified modifiable field on success or a 'VOID' element
        // reference on error.  An empty string can be used for any of the
        // arguments to specify the current selection within a choice object.
        // An unused argument results in the construction of a null
        // 'NameOrIndex', which is treated as the end of the argument list.
        // Note that if 'true == isNull()' for any field in the chain of
        // fields then an error is returned.  Modifying the element through
        // the returned reference will modify the original aggregate.  The
        // behavior is undefined if the resulting element ref is used to
        // modify the data such that it no longer conforms to the schema.

    bdem_ElemRef fieldRefById(int fieldId) const;
        // Return an element reference to the modifiable field specified by
        // 'fieldId' within this aggregate or a 'VOID' element reference on
        // error.  If this aggregate refers to a choice value, then a
        // 'fieldId' of 'bdem_RecordDef::BDEM_NULL_FIELD_ID' can be used to
        // specify the current selection.  Modifying the element through the
        // returned reference will modify the original aggregate.  Note that
        // 'fieldId' refers to a field's numeric identifier assigned in its
        // record definition within the schema, not to its positional index
        // within the record definition.  Note that if 'true == isNull()' then
        // an error is returned.  The behavior is undefined if the resulting
        // element ref is used to modify the data such that it no longer
        // conforms to the schema.

    bdem_ElemRef fieldRefByIndex(int index) const;
        // Return an element reference to the modifiable field within this
        // aggregate specified by the (zero-based) 'fieldIndex' or a 'VOID'
        // element reference on error.  If this aggregate refers to a choice
        // value, then a 'fieldIndex' of -1 can be used to specify the current
        // selection.  Modifying the element through the returned reference
        // will modify the original aggregate.  Note that if
        // 'true == isNull()' then an error is returned.  The behavior is
        // undefined if the resulting element ref is used to modify the data
        // such that it no longer conforms to the schema.

    int length() const;
        // Return the number of fields or items in the scalar array, list, 
        // table, or choice array referred to by this aggregate or
        // 'BCEM_ERR_NOT_AN_ARRAY' for other data types.  Note that 0 will be 
        // returned if this aggregate refers to a null array.  

    int size() const;
        // Equivalent to 'length()' (STL-style).

    int numSelections() const;
        // If this aggregate refers to choice or choice array item, return the
        // number of available selectors or 0 if 'true == isNull()'.  If this
        // aggregate refers to a list or row, look for an anonymous field
        // within the list or row and recursively look for a choice in the
        // anonymous field (if any), as per the "Anonymous Fields" section of
        // the 'bcem_aggregate' component-level documentation.  Otherwise,
        // return an error code.

    const char *selector() const;
        // Return the name for selector of the choice or choice array item
        // referenced by this aggregate or an empty string either if there is
        // no current selection, if the current selector does not have a text
        // name, if this aggregate does not refer to a choice or choice array
        // item, or 'true == isNull()'.  If this aggregate refers to a list or
        // row, look for an  anonymous field within the list or row and
        // recursively look for a choice in the anonymous field (if any), as
        // per the "Anonymous Fields" section of the 'bcem_aggregate'
        // component-level documentation.  The returned pointer is valid until
        // the the choice object is modified or destroyed.  Note that an error
        // condition is indistinguishable from one of the valid reasons for
        // returning an empty string.  The 'selection' or 'selectorId' methods
        // can be used to distinguish an error condition from a valid
        // selector.

    int selectorId() const;
        // Return the ID for selector of the choice or choice array item
        // referenced by this aggregate, 'bdem_RecordDef::BDEM_NULL_FIELD_ID'
        // if there is no current selection, the current selection does not
        // have a numeric ID, this aggregate does not refer to a choice or
        // choice array item, or 'true == isNul()'.  If this aggregate refers
        // to a list or row, look for an anonymous field within the list or
        // row and recursively look for a choice in the anonymous field (if
        // any), as per the "Anonymous Fields" section of the 'bcem_aggregate'
        // component-level documentation.  Note that the returned ID is the
        // selector's numeric identifier (assigned in its record definition
        // within the schema), not its positional index within the record
        // definition (see 'selectorIndex').

    int selectorIndex() const;
        // Return the index for selector of the choice or choice array item
        // referenced by this aggregate, -1 if there is no current selection,
        // or if 'true == isNull()' or an error code if this aggregate does
        // not refer to a choice or choice array item.  If this aggregate
        // refers to a list or row, look for an anonymous field within the
        // list or row and recursively look for a choice in the anonymous
        // field (if any), as per the "Anonymous Fields" section of the
        // 'bcem_aggregate' component-level documentation.

    bdem_ElemType::Type dataType() const;
        // Return the type of data referenced by this aggregate.  Return
        // 'bdem_ElemType::BDEM_VOID' for a void or error aggregate.

    const bdem_RecordDef& recordDef() const;
        // Return a reference to the non-modifiable record definition that
        // describes the structure of the object referenced by this aggregate.
        // The behavior is undefined unless this aggregate references a
        // constrained list, constrained table, or constrained choice object.
        // Note that 'recordConstraint' should be preferred if there is any
        // doubt as to whether this aggregate has a record definition.

    const bdem_RecordDef *recordConstraint() const;
        // Return the address of the non-modifiable record definition that
        // describes the structure of the object referenced by this aggregate,
        // or 0 if this aggregate references a scalar, array of scalars, or
        // unconstrained 'bdem' aggregate.

    const bdem_EnumerationDef *enumerationConstraint() const;
        // Return a pointer to the non-modifiable enumeration definition that
        // constrains the object referenced by this aggregate, or a null
        // pointer if this aggregate does not reference an enumeration object.

    const bdem_FieldDef *fieldDef() const;
        // Return a pointer to the non-modifiable field definition for the
        // object referenced by this aggregate, or null if this object does
        // not have a field definition.  An aggregate constructed directly
        // using a record definition will not have a field definition,
        // whereas a sub-aggregate returned by a field-access function (e.g.,
        // 'operator[]' or 'field') will.  Note that, if this aggregate is an
        // item within an array, table, or choice array, then
        // 'fieldDef()->elemType()' will return the *array* type, not the
        // *item* type (i.e., 'fieldDef()->elemType()' will not match
        // 'dataType()').

    const bdem_RecordDef *recordDefPtr() const;
        // Return the address of the record definition for the object
        // referenced by this aggregate, or 0 if this aggregate is empty, 
        // unconstrained, or represents a scalar or scalar array.  

    const void *data() const;
        // Return the address of the non-modifiable data referenced by this
        // aggregate.  

    const bdem_Schema *schema() const;
        // Return the address of the non-modifiable schema referenced by this
        // aggregate.  

    void swap(bcem_AggregateRaw& other);
        // Efficiently exchange the states of this aggregate object and the
        // specified 'other' aggregate object such that value, schema, and
        // nullness information held by each will be what was formerly held by
        // the other.

    template <class STREAM>
    STREAM& bdexStreamIn(STREAM& stream, int version) const;
        // Assign to the object referenced by this aggregate the value read
        // from the specified input 'stream' using the specified 'version'
        // format and return a reference to the modifiable 'stream'.  This
        // aggregate must be initialized with the type and record definition
        // of the expected input before calling this method.  If 'stream' is
        // initially invalid, this operation has no effect.  If 'stream'
        // becomes invalid during this operation, this object is valid, but
        // its value is undefined.  If 'version' is not supported, 'stream' is
        // marked invalid and the object is unaltered.  Note that no version
        // is read from 'stream'.  This operation has the same effect as
        // calling 'bdexStreamIn' on the referenced object (e.g., if
        // 'dataType()' is 'LIST', call 'bdem_List::bdexStreamIn' on the
        // referenced object).  See the 'bdex' package-level documentation for
        // more information on 'bdex' streaming of value-semantic types and
        // containers.

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

    bsl::ostream& print(bsl::ostream& stream,
                        int           level = 0,
                        int           spacesPerLevel = 4) const;
        // Format the referenced object to the specified output 'stream' at
        // the (absolute value of) the optionally specified indentation
        // 'level' and return a reference to 'stream'.  If 'level' is
        // specified, optionally specify 'spacesPerLevel', the number of
        // spaces per indentation level for this and all of its nested
        // objects.  If 'level' is negative, suppress indentation of the first
        // line.  If 'spacesPerLevel' is negative, format the entire output on
        // one line.  If 'stream' is not valid on entry, this operation has no
        // effect.  For scalars and arrays of scalars, this 'print' function
        // delegates to the appropriate printing mechanism for referenced
        // object -- the aggregate adds no additional text to the
        // output.  (E.g., the result of printing an aggregate that references
        // a string is indistinguishable from the result of printing the
        // string directly.)  For list, row, choice, choice array item, table,
        // and choice array, this print function prepend's each field with the
        // name of the field.

    template <typename VALTYPE>
    int insertItem(bcem_AggregateRaw   *newItem, 
                   bcem_AggregateError *description, 
                   int                  index, 
                   const VALTYPE&       value) const;
        // Insert a copy of the specified 'value' before the specified 
        // 'index' in the scalar array, table, or choice array referenced by
        // this aggregate.  Return 0 on success and load a reference to the 
        // new item into the specified 'newItem'; otherwise, return a nonzero 
        // value and load a description into the specified 'errorDescription'.  

    int insertItemRaw(bcem_AggregateRaw   *newItem, 
                      bcem_AggregateError *errorDescription, 
                      int                  index) const;
        // Insert a new element before the specified 'index' in the 
        // scalar array, table, or choice array referenced by this 
        // aggregate.  The new value is uninitialized, meaning it is *not* 
        // set to its default value as with the 'insertItems' method.  Return 
        // 0 on success and load a reference to the new item into the
        // specified 'newItem'; otherwise, return a nonzero value and load
        // a description into the specified 'errorDescription'.  

    int insertItems(bcem_AggregateError *errorDescription, 
                    int                  index, 
                    int                  numItems) const;
        // Insert 'numItems' new elements before the specified 'index' in
        // the scalar array, table, or choice array referenced by this
        // aggregate.  The new items are set to their default values, as
        // specified in the schema (recursively for the fields within inserted
        // table rows).  If '0 == pos' and 'isNull()', set the value referenced
        // by this object to its default value (i.e., make it non-null).  
        // Return 0 on success, with no effect on the specified 
        // 'errorDescription'; otherwise, load into 'errorDescription' a
        // description of the failure and return a nonzero value.

    int removeItems(bcem_AggregateError *errorDescription, 
                    int                  index, 
                    int                  numItems) const;
        // Remove the specified 'numItems' items starting at the specified
        // 'index' in the scalar array, choice array, or table referenced
        // by this aggregate.  Return 0 on success, with no effect on the
        // specified 'errorDescription'; otherwise, load into 
        // 'errorDescription' a description of the failure and return a
        // nonzero value.

    int makeSelectionByIndexRaw(bcem_AggregateRaw   *result, 
                                bcem_AggregateError *errorDescription,
                                int                  index) const;
        // Change the selector in the referenced choice object to the one at
        // the specified 'index', and load a reference to the new selection 
        // into the specified 'result' if 'result' is not 0. The new
        // selection will not be initialized as in 'makeSelection' (so
        // makeValue() will need to be invoked before using the new selection). 
        // If '-1 == index' then the selector value of this object is reset
        // to its default value, with no effect on 'result'.  Return 0 on
        // success, or a nonzero value otherwise with the specified
        // 'errorDescription' loaded with details.  

    int makeSelection(bcem_AggregateRaw   *result, 
                      bcem_AggregateError *errorDescription, 
                      const char          *newSelector) const;
        // Change the selector in the choice object referenced by this
        // aggregate to the one specified by 'newSelector', first making this
        // choice non-null if it is currently null.  If 'newSelector'
        // is null or the empty string then the selection is reset to its
        // default value. The newly selected sub-object is initialized to its
        // default value.  Return 0 on success, loading a sub-aggregate
        // referring to the modifiable selection into the specified 'result' 
        // object if 'result' is not 0; otherwise, return a nonzero error and
        // populate the specified 'errorDescription' with no effect on this
        // object.

    int makeSelectionById(bcem_AggregateError *errorDescription, 
                          int                  newSelectorId) const;
        // Change the selector in the choice object referenced by this
        // aggregate to the one specified by 'newSelectorId', first making
        // this choice non-null if it is currently null.  If 'newSelector'
        // is null or the empty string then the selection is reset to its
        // default value. The newly selected sub-object is initialized to its
        // default value.  Return 0 on success; otherwise, return a nonzero 
        // error and populate the specified 'errorDescription' with no effect
        // on this object.

    void makeNull() const;
        // Set the object referenced by this aggregate to null.
        // If the object is null before the call, then do nothing.  The
        // object's type and constraints are unchanged.

    void makeValue() const;
        // Construct a value for the nullable object referenced by this
        // aggregate by assigning it the default value specified in the schema,
        // or (for 'LIST' aggregates) by giving each scalar fielad its default
        // value and recursively constructing the default value for each
        // non-nullable sublist.  Do nothing if this aggregate is not
        // null.  Note that if this aggregate refers to a list with an
        // empty record definition, then the entire list will still be null
        // after this function is called.

    template <typename TYPE>
    int setValue(bcem_AggregateError *errorDescription, 
                 const TYPE&          value) const;
        // Set the value referenced by this aggregate to the specified
        // 'value', converting the specified 'value' as necessary.  If
        // 'value' is null then make this aggregate null.  Return 0 on 
        // success, with no effect on the specified 'errorDescription'; 
        // otherwise, load into 'errorDescription' a description of the 
        // failure and return a nonzero value.  

    int resize(bcem_AggregateError *errorDescription, 
               bsl::size_t          newSize) const;
        // Grow or shrink the scalar array, table, or choice array referenced
        // by this aggregate to the specified 'newSize' number of items, first
        // making the referenced value non-null if currently null.  If
        // 'newSize > length()', grow the array or table by inserting new
        // items at the end and set them to their default value, as specified
        // in the schema (recursively for the fields within inserted table
        // rows).  If 'newSize < length()' shrink the array or table by
        // discarding excess items at the end.  Return 0 on success, with 
        // no effect on the specified 'errorDescription'; otherwise, load a 
        // description of the error into 'errorDescription' and return a
        // nonzero value.  
};

template <>
inline
int bcem_AggregateRaw::setValue<bcem_AggregateRaw>(
                                        bcem_AggregateError *errorDescription, 
                                        const bcem_AggregateRaw& value) const;
    // Specialization of 'setValue<VALUETYPE>' for 
    // 'VALUETYPE = bcem_AggregateRaw'

                   // ============================
                   // struct bcem_AggregateRawUtil
                   // ============================

struct bcem_AggregateRawUtil {
    // This 'struct' provides a namespace for a set of utility methods
    // for working with the 'bdem' data representations of this component.  

    static
    const char *enumerationName(const bdem_EnumerationDef *enumDef);
        // Return the name of the specified 'enumDef', or "(anonymous)" if
        // 'enumDef' has no name, or "(unconstrained)" if 'enumDef' is null.

    static
    const char *recordName(const bdem_RecordDef *recordDef);
        // Return the name of the specified 'recordDef', or "(anonymous)" if
        // 'recordDef' has no name, or "(unconstrained)" if 'recordDef' is
        // null.

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
        // If the specified 'object' of the specified 'type' is of aggregate
        // type and the specified 'recordDef' is non-null, return 'true' if
        // 'object' conforms to 'recordDef', and 'false' otherwise.  Return
        // 'false' if 'object' is of non-aggregate type and 'recordDef' is
        // non-null.  Return 'true' if 'recordDef' is null.  The behavior is
        // undefined unless 'object' is non-null.

    template <typename VISITOR>
    static
    int visitArray(void                *array,
                   bdem_ElemType::Type  arrayType,
                   VISITOR             *visitorPtr);
        // Cast the specified 'array' to a pointer to the vector type indicated
        // by the specified 'arrayType' and return the value returned by:
        //..
        //    (*visitorPtr)(array)
        //..
        // The parameterized 'VISITOR' type must have 'int operator()(TYPE*)'
        // overloaded for each array 'TYPE' that may be passed to this
        // function.  The '*visitorPtr' object may store state and, if
        // non-'const', that state may be modified by the visitor.  Return 0
        // on success, or bcem_AggregateError::BCEM_ERR_NOT_AN_ARRAY if
        // 'arrayType' is not an array of scalar 'bdem' type.  The behavior
        // is undefined unless 'array' is of 'arrayType'.
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

// ===========================================================================
//                      'bdeat' INTEGRATION
// ===========================================================================


                // ========================================
                // local struct bcem_AggregateRaw_BdeatInfo
                // ========================================

struct bcem_AggregateRaw_BdeatInfo {
    // This 'struct' provides attribute information used when 
    // accessing or manipulating the fields of 'bcem_AggregateRaw'
    // within the 'bdeat' framework.

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

  public:
    // CREATORS
    bcem_AggregateRaw_BdeatInfo(const bdem_RecordDef     *record,
                                int                       fieldIndex);
        // Create an info structure representing the field having the
        // specified 'fieldIndex' in the specified 'record'.  

    // MANIPULATORS
    const char *& annotation()
    {
        if (!d_areLazyAttributesSet) {
            setLazyAttributes();
        }
        return d_annotation_p;
    }

    int& formattingMode()
    {
        return d_formattingMode;
    }

    int& id()
    {
        return d_id;
    }

    const char *& name()
    {
        if (!d_areLazyAttributesSet) {
            setLazyAttributes();
        }
        return d_name_p;
    }

    int& nameLength()
    {
        if (!d_areLazyAttributesSet) {
            setLazyAttributes();
        }
        return d_nameLength;
    }

    bool& hasDefaultValue()
    {
        return d_hasDefaultValue;
    }

    bool& isNullable()
    {
        return d_isNullable;
    }

    // ACCESSORS
    const char *annotation() const
    {
        if (!d_areLazyAttributesSet) {
            setLazyAttributes();
        }
        return d_annotation_p;
    }

    int formattingMode() const
    {
        return d_formattingMode;
    }

    int id() const
    {
        return d_id;
    }

    const char *name() const
    {
        if (!d_areLazyAttributesSet) {
            setLazyAttributes();
        }
        return d_name_p;
    }

    int nameLength() const
    {
        if (!d_areLazyAttributesSet) {
            setLazyAttributes();
        }
        return d_nameLength;
    }

    bool hasDefaultValue() const
    {
        return d_hasDefaultValue;
    }

    bool isNullable() const
    {
        return d_isNullable;
    }
};

                // ========================================
                // local struct bcem_AggregateRaw_BdeatUtil
                // ========================================

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

    static
    int manipulateField(bcem_AggregateRaw *parent,
                        bdef_Function<int(*)(
                                          bcem_AggregateRaw*, 
                                          const bcem_AggregateRaw_BdeatInfo&)>&
                                              manipulator,
                        int                   fieldIndex);
    template <typename MANIPULATOR>
    static
    int manipulateField(bcem_AggregateRaw    *parent,
                        MANIPULATOR&          manipulator,
                        int                   fieldIndex)
        // Used for list/row/choice/choiceArrayItem.
    {
        if (! parent->recordDefPtr()) {
            return -1;
        }

        bcem_AggregateRaw field;
        bcem_AggregateError dummy;
        if (0 != parent->fieldByIndex(&field, &dummy, fieldIndex)) {
            return -1;
        }

        bcem_AggregateRaw_BdeatInfo info(parent->recordDefPtr(), fieldIndex);

        if (info.isNullable()) {
            NullableAdapter adapter = { &field };
            return manipulator(&adapter, info);
        }
        else {
            return manipulator(&field, info);
        }
    }

    template <typename ACCESSOR>
    static
    int accessField(const bcem_AggregateRaw& parent,
                    ACCESSOR&             accessor,
                    int                   fieldIndex)
        // Used for list/row/choice/choiceArrayItem.
    {
        if (! parent.recordDefPtr()) {
            return -1;
        }

        bcem_AggregateRaw field;
        bcem_AggregateError dummy;
        if (0 != parent.fieldByIndex(&field, &dummy, fieldIndex)) {
            return -1;
        }

        bcem_AggregateRaw_BdeatInfo info(parent.recordDefPtr(), fieldIndex);

        // If a field is nullable but also has a default value, then we treat
        // it as non-nullable to avoid suppressing access to special (null)
        // values that are not the default.  For example a 'char' with a
        // default value of '127' should not be considered nullable or else
        // the accessor will not be called on a '0' value (which bdem
        // considers to be null but the rest of the world considers normal).

        if (info.isNullable() && !info.hasDefaultValue()) {
            NullableAdapter adapter = { &field };
            return accessor(adapter, info);
        }
        else {
            return accessor(field, info);
        }
    }
};

// ============================================================================
//           'bdeat_choicefunctions' overloads and specializations
// ============================================================================

namespace bdeat_ChoiceFunctions {
    // META-FUNCTIONS
    bslmf_MetaInt<1> isChoiceMetaFunction(const bcem_AggregateRaw&);
        // This function can be overloaded to support partial specialization
        // (Sun5.2 compiler is unable to partially specialize the 'struct'
        // below).  Note that this function is has no definition and should not
        // be called at runtime.

    template <>
    struct IsChoice<bcem_AggregateRaw> {
        enum { VALUE = 1 };
    };

}  // close namespace bdeat_ChoiceFunctions

inline
bool bdeat_choiceHasSelection(const bcem_AggregateRaw& object, 
                              int selectionId)
{
    return object.hasFieldById(selectionId);
}


bool bdeat_choiceHasSelection(const bcem_AggregateRaw&  object,
                              const char               *selectionName,
                              int                       selectionNameLength);

int bdeat_choiceMakeSelection(bcem_AggregateRaw *object, 
                              int                selectionId);


int bdeat_choiceMakeSelection(bcem_AggregateRaw  *object,
                              const char         *selectionName,
                              int                 selectionNameLength);

template <typename MANIPULATOR>
inline
int bdeat_choiceManipulateSelection(bcem_AggregateRaw *object,
                                    MANIPULATOR&    manipulator)
{
    int fieldIndex = object->selectorIndex();
    if (fieldIndex < 0) {
        return -1;
    }

    return bcem_AggregateRaw_BdeatUtil::manipulateField(object,
                                                        manipulator,
                                                        fieldIndex);
}
template <typename ACCESSOR>
inline
int bdeat_choiceAccessSelection(const bcem_AggregateRaw& object,
                                ACCESSOR&             accessor)
{
    int fieldIndex = object.selectorIndex();
    if (fieldIndex < 0) {
        return -1;
    }

    return bcem_AggregateRaw_BdeatUtil::accessField(object, 
                                                    accessor, 
                                                    fieldIndex);
}

inline
int bdeat_choiceSelectionId(const bcem_AggregateRaw& object)
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

    // META-FUNCTIONS
    bslmf_MetaInt<1> isArrayMetaFunction(const bcem_AggregateRaw&);
        // This function can be overloaded to support partial specialization
        // (Sun5.2 compiler is unable to partially specialize the 'struct'
        // below).  Note that this function is has no definition and should not
        // be called at runtime.

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
{
    bcem_AggregateError dummy;
    int rc = array->resize(&dummy, newSize);
    BSLS_ASSERT_SAFE(0 == rc);
    (void) rc;
}

inline
bsl::size_t bdeat_arraySize(const bcem_AggregateRaw& array)
{
    return array.size();
}

template <typename ACCESSOR>
int bdeat_arrayAccessElement(const bcem_AggregateRaw&    array,
                             ACCESSOR&                   accessor,
                             int                         index)
{
    bcem_AggregateRaw element;
    bcem_AggregateError dummy;
    if (0 != array.arrayItem(&element, &dummy, index)) {
        return -1;
    }

    const bdem_FieldDef *fieldDef = array.fieldDef();
    if (fieldDef
     && fieldDef->formattingMode() & bdeat_FormattingMode::BDEAT_NILLABLE) {
        bcem_AggregateRaw_BdeatUtil::NullableAdapter adapter = { &element };
        return accessor(adapter);
    }
    else {
        return accessor(element);
    }
}

int bdeat_arrayManipulateElement(bcem_AggregateRaw   *array, 
                                 bdef_Function<int(*)(bcem_AggregateRaw*)>&,
                                 int                  index);
template <typename MANIPULATOR>
int bdeat_arrayManipulateElement(bcem_AggregateRaw    *array,
                                 MANIPULATOR&          manipulator,
                                 int                   index)
    // If there is an element with the specified 'index' within the specified
    // 'array', invoke the specified 'manipulator' on that element and return
    // the result of the invocation; otherwise return a nonzero value.  
{
    bcem_AggregateRaw element;
    bcem_AggregateError dummy;
    if (0 != array->arrayItem(&element, &dummy, index)) {
        return -1;
    }

    const bdem_FieldDef *fieldDef = array->fieldDef();
    if (fieldDef
     && fieldDef->formattingMode() & bdeat_FormattingMode::BDEAT_NILLABLE) {
        bcem_AggregateRaw_BdeatUtil::NullableAdapter adapter = { &element };
        return manipulator(&adapter);
    }
    else {
        return manipulator(&element);
    }
}

// ============================================================================
//           'bdeat_enumfunctions' overloads and specializations
// ============================================================================

namespace bdeat_EnumFunctions {

    // META-FUNCTIONS
    bslmf_MetaInt<1> isEnumerationMetaFunction(const bcem_AggregateRaw&);
        // This function can be overloaded to support partial specialization
        // (Sun5.2 compiler is unable to partially specialize the 'struct'
        // below).  Note that this function is has no definition and should not
        // be called at runtime.

    template <>
    struct IsEnumeration<bcem_AggregateRaw> {
        enum { VALUE = 1 };
    };
}  // close namespace bdeat_EnumFunctions

int bdeat_enumFromString(bcem_AggregateRaw *result,
                         const char        *string,
                         int                stringLength);

int bdeat_enumFromInt(bcem_AggregateRaw *result, int enumId);

inline
void bdeat_enumToInt(int *result, const bcem_AggregateRaw& value)
{
    *result = value.asInt();
}

inline
void bdeat_enumToString(bsl::string *result, const bcem_AggregateRaw& value)
{
    *result = value.asString();
}


// ============================================================================
//                       'bdeat_valuetype' overloads
// ============================================================================

// This function is not available nor easily implemented
// int bdeat_valueTypeAssign(bcem_AggregateRaw *lhs, 
//                           const bcem_AggregateRaw& rhs)

void bdeat_valueTypeReset(bcem_AggregateRaw *object);

inline
void bdeat_valueTypeReset(bcem_AggregateRaw_BdeatUtil::NullableAdapter *object)
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
                                      MANIPULATOR&    manipulator,
                                      const char     *attributeName,
                                      int             attributeNameLength)
{
    if (! object->recordConstraint()) {
        return -1;
    }

    int fieldIndex = bcem_AggregateRaw_BdeatUtil::fieldIndexFromName(
                                                          object->recordDef(),
                                                          attributeName,
                                                          attributeNameLength);

    if (0 > fieldIndex) {
        return -1;
    }

    return bcem_AggregateRaw_BdeatUtil::manipulateField(object,
                                                        manipulator,
                                                        fieldIndex);
}

template <typename MANIPULATOR>
int bdeat_sequenceManipulateAttribute(bcem_AggregateRaw *object,
                                      MANIPULATOR&    manipulator,
                                      int             attributeId)
{
    if (! object->recordConstraint()) {
        return -1;
    }

    int fieldIndex = object->recordDef().fieldIndex(attributeId);

    if (0 > fieldIndex) {
        return -1;
    }

    return bcem_AggregateRaw_BdeatUtil::manipulateField(object,
                                                        manipulator,
                                                        fieldIndex);
}

template <typename MANIPULATOR>
int bdeat_sequenceManipulateAttributes(bcem_AggregateRaw *object,
                                       MANIPULATOR&          manipulator)
{
    if (! object->recordConstraint()) {
        return -1;
    }

    const int numAttr = object->recordDef().numFields();
    int ret = 0;
    for (int index = 0; 0 == ret && index < numAttr; ++index) {
        ret = bcem_AggregateRaw_BdeatUtil::manipulateField(object,
                                                           manipulator,
                                                           index);
    }

    return ret;
}

template <typename ACCESSOR>
int bdeat_sequenceAccessAttribute(const bcem_AggregateRaw&  object,
                                  ACCESSOR&              accessor,
                                  const char            *attributeName,
                                  int                    attributeNameLength)
{
    if (! object.recordConstraint()) {
        return -1;
    }

    int fieldIndex = bcem_AggregateRaw_BdeatUtil::fieldIndexFromName(
                                                          object.recordDef(),
                                                          attributeName,
                                                          attributeNameLength);

    if (0 > fieldIndex) {
        return -1;
    }

    return bcem_AggregateRaw_BdeatUtil::accessField(object, 
                                                    accessor, 
                                                    fieldIndex);
}

template <typename ACCESSOR>
int bdeat_sequenceAccessAttribute(const bcem_AggregateRaw& object,
                                  ACCESSOR&                   accessor,
                                  int                         attributeId)
{
    if (! object.recordConstraint()) {
        return -1;
    }

    int fieldIndex = object.recordDef().fieldIndex(attributeId);

    if (0 > fieldIndex) {
        return -1;
    }

    return bcem_AggregateRaw_BdeatUtil::accessField(object, 
                                                    accessor, 
                                                    fieldIndex);
}

template <typename ACCESSOR>
int bdeat_sequenceAccessAttributes(const bcem_AggregateRaw& object,
                                   ACCESSOR&                   accessor)
{
    if (! object.recordConstraint()) {
        return -1;
    }

    const int numAttr = object.recordDef().numFields();
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
                                const char            *attributeName,
                                int                    attributeNameLength)
{
    if (! object.recordConstraint()) {
        return false;
    }

    return 0 <= bcem_AggregateRaw_BdeatUtil::fieldIndexFromName(
                                                          object.recordDef(),
                                                          attributeName,
                                                          attributeNameLength);
}

inline
bool bdeat_sequenceHasAttribute(const bcem_AggregateRaw& object, 
                                int attributeId)
{
    return object.hasFieldById(attributeId);
}


// ============================================================================
//           'bdeat_nullablevaluefunctions' overloads and specializations
// ============================================================================

namespace bdeat_NullableValueFunctions {

    bslmf_MetaInt<1> isNullableValueMetaFunction(
                        const bcem_AggregateRaw_BdeatUtil::NullableAdapter&);
        // This function can be overloaded to support partial specialization
        // (Sun5.2 compiler is unable to partially specialize the 'struct'
        // below).  Note that this function is has no definition and should not
        // be called at runtime.

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
{
    return object.d_element_p->isNull();
}

inline
void bdeat_nullableValueMakeValue(
                       bcem_AggregateRaw_BdeatUtil::NullableAdapter *object)
{
    object->d_element_p->makeValue();
}

template <typename MANIPULATOR>
inline
int bdeat_nullableValueManipulateValue(
                  bcem_AggregateRaw_BdeatUtil::NullableAdapter *object,
                  MANIPULATOR&                                  manipulator)
{
    return manipulator(object->d_element_p);
}

template <typename ACCESSOR>
inline
int bdeat_nullableValueAccessValue(
               const bcem_AggregateRaw_BdeatUtil::NullableAdapter& object,
               ACCESSOR&                                           accessor)
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
                                  ACCESSOR&             accessor)
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
                                      MANIPULATOR&    manipulator)
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
                                       MANIPULATOR&    manipulator)
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
                                   ACCESSOR&             accessor)
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


//
// ===========================================================================
//                      INLINE AND TEMPLATE FUNCTION DEFINITIONS
// ===========================================================================

                   // ----------------------------------
                   // local struct bcem_AggregateRawUtil
                   // ----------------------------------

inline
bool bcem_AggregateRawUtil::isConformant(const void           *,
                                         const bdem_RecordDef *recordDef)
{
    // A non-aggregate conforms only if 'recordDef' is null.

    return 0 == recordDef;
}

template <typename VISITOR>
int
bcem_AggregateRawUtil::visitArray(void                *array,
                                bdem_ElemType::Type  arrayType,
                                VISITOR             *visitorPtr)
{
    VISITOR& visitorObj = *visitorPtr;
    switch (arrayType) {
      case bdem_ElemType::BDEM_CHAR_ARRAY: 
        return visitorObj((bsl::vector<char>*) array);
      case bdem_ElemType::BDEM_SHORT_ARRAY: 
        return visitorObj((bsl::vector<short>*) array);
      case bdem_ElemType::BDEM_INT_ARRAY: 
        return visitorObj((bsl::vector<int>*) array);
      case bdem_ElemType::BDEM_INT64_ARRAY: 
          return visitorObj((bsl::vector<bsls_Types::Int64>*) array);
      case bdem_ElemType::BDEM_FLOAT_ARRAY: 
        return visitorObj((bsl::vector<float>*) array);
      case bdem_ElemType::BDEM_DOUBLE_ARRAY: 
        return visitorObj((bsl::vector<double>*) array);
      case bdem_ElemType::BDEM_STRING_ARRAY: 
        return visitorObj((bsl::vector<bsl::string>*) array);
      case bdem_ElemType::BDEM_DATETIME_ARRAY: 
        return visitorObj((bsl::vector<bdet_Datetime>*) array);
      case bdem_ElemType::BDEM_DATE_ARRAY: 
        return visitorObj((bsl::vector<bdet_Date>*) array);
      case bdem_ElemType::BDEM_TIME_ARRAY: 
        return visitorObj((bsl::vector<bdet_Time>*) array);
      case bdem_ElemType::BDEM_BOOL_ARRAY: 
        return visitorObj((bsl::vector<bool>*) array);
      case bdem_ElemType::BDEM_DATETIMETZ_ARRAY: 
        return visitorObj((bsl::vector<bdet_DatetimeTz>*) array);
      case bdem_ElemType::BDEM_DATETZ_ARRAY: 
        return visitorObj((bsl::vector<bdet_DateTz>*) array);
      case bdem_ElemType::BDEM_TIMETZ_ARRAY: 
        return visitorObj((bsl::vector<bdet_TimeTz>*) array);
      default: 
        return bcem_AggregateError::BCEM_ERR_NOT_AN_ARRAY;
    }
}

                     //---------------------------------------
                     // local class bcem_Aggregate_NameOrIndex
                     //---------------------------------------

// CREATORS
inline
bcem_AggregateRawNameOrIndex::bcem_AggregateRawNameOrIndex()
: d_state(BCEM_NOI_EMPTY)
, d_name(0)
{
}

inline
bcem_AggregateRawNameOrIndex::bcem_AggregateRawNameOrIndex(const char *name)
: d_state(BCEM_NOI_NAME)
, d_name(name)
{
}

inline
bcem_AggregateRawNameOrIndex::bcem_AggregateRawNameOrIndex(
                                                       const bsl::string& name)
: d_state(BCEM_NOI_NAME)
, d_name(name.c_str())
{
}

inline
bcem_AggregateRawNameOrIndex::bcem_AggregateRawNameOrIndex(int index)
: d_state(BCEM_NOI_INDEX)
, d_index(index)
{
}

// ACCESSORS
inline
bool bcem_AggregateRawNameOrIndex::isEmpty() const
{
    return BCEM_NOI_EMPTY == d_state;
}

inline
bool bcem_AggregateRawNameOrIndex::isName() const
{
    return BCEM_NOI_NAME == d_state;
}

inline
bool bcem_AggregateRawNameOrIndex::isIndex() const
{
    return BCEM_NOI_INDEX == d_state;
}

inline
const char *bcem_AggregateRawNameOrIndex::name() const
{
    return BCEM_NOI_NAME == d_state ? d_name : 0;
}

inline
int bcem_AggregateRawNameOrIndex::index() const
{
    return BCEM_NOI_INDEX == d_state
                             ? d_index
                             : bdem_RecordDef::BDEM_NULL_FIELD_ID;
}


                 // -------------------------------------------
                 // local class bcem_AggregateRaw_ArrayInserter
                 // -------------------------------------------

// CREATORS
inline
bcem_AggregateRaw_ArrayInserter::bcem_AggregateRaw_ArrayInserter(
                                           int                  index,
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
int bcem_AggregateRaw_ArrayInserter::operator()(ARRAYTYPE *array)
{
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
void *bcem_AggregateRaw_ArrayInserter::data() const
{
    return d_data_p;
}

inline
int bcem_AggregateRaw_ArrayInserter::length() const
{
    return d_length;
}
                           //-------------------------
                           // class bcem_AggregateRaw
                           //-------------------------

// CLASS METHODS
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

// PRIVATE MANIPULATORS
template <typename VALUETYPE>
inline
int bcem_AggregateRaw::toEnum(bcem_AggregateError *errorDescription,
                              const VALUETYPE&     value) const
{
    static const int IS_DIRECT =
              bslmf_IsConvertible<VALUETYPE, const char*>::VALUE
           || bslmf_IsConvertible<VALUETYPE, bsl::string>::VALUE
           || bslmf_IsConvertible<VALUETYPE, const bdem_ConstElemRef&>::VALUE;

    return toEnum(errorDescription, value, bslmf_MetaInt<IS_DIRECT>());
}

template <typename VALUETYPE>
int bcem_AggregateRaw::toEnum(bcem_AggregateError *errorDescription, 
                              const VALUETYPE&     value, 
                              bslmf_MetaInt<0>     direct) const
{
    int intVal;
    if (0 != bdem_Convert::convert(&intVal, value)) {
        errorDescription->description().assign(
                                       "Invalid conversion to enumeration \"");
        errorDescription->description() += 
            bcem_AggregateRawUtil::enumerationName(enumerationConstraint());
        errorDescription->description() += "\" from \"";
        errorDescription->description() += 
            bdem_ElemType::toAscii(getBdemType(value));
        errorDescription->description() += '\"';
        errorDescription->code() = bcem_AggregateError::BCEM_ERR_BAD_CONVERSION;
        return 1;
    }
    return toEnum(errorDescription, intVal, direct);
}

inline
int bcem_AggregateRaw::toEnum(bcem_AggregateError *errorDescription, 
                              const bsl::string&   value, 
                              bslmf_MetaInt<1>     direct) const
{
    return toEnum(errorDescription, value.c_str(), direct);
}

template <typename TYPE> 
int bcem_AggregateRaw::assignToNillableScalarArrayImp(
                                              const TYPE& value) const
{
    bdem_ElemType::Type srcType = value.type();

    // Check conformance of value against this aggregate.
    if (bdem_ElemType::BDEM_TABLE == srcType) {
        return assignToNillableScalarArray(value.theTable());
    }

    bdem_ElemType::Type baseType = bdem_ElemType::fromArrayType(srcType);
    if (!bdem_ElemType::isScalarType(baseType)
        || baseType != recordConstraint()->field(0).elemType()) {
        return bcem_AggregateError::BCEM_ERR_NON_CONFORMANT;
    }

    if (value.isNull()) {
        makeNull();
        return 0;
    }

    ElemDataFetcher fetcher(value);
    void *srcData = fetcher.d_data_p;
    ArraySizer sizer;
    const int  length  = bcem_AggregateRawUtil::visitArray(srcData, srcType,
                                                           &sizer);
    
    bcem_AggregateError error;
    if (0 != resize(&error, length)) {
        return error.code();
    }

    bdem_Table            *dstTable     = (bdem_Table *)data();
    const bdem_Descriptor *baseTypeDesc =
                                  bdem_ElemAttrLookup::lookupTable()[baseType];

    for (int i = 0; i < length; ++i) {
        bcem_AggregateRaw_ArrayIndexer indexer(i);
        bcem_AggregateRawUtil::visitArray(srcData, srcType, &indexer);
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
    if (!bcem_AggregateRawUtil::isConformant(&value, recordConstraint())) {
        return bcem_AggregateError::BCEM_ERR_NON_CONFORMANT; 
    }

    *(bdem_Table *)data() = value;
    return 0;
}

template <>
inline
int bcem_AggregateRaw::assignToNillableScalarArray(
                                              const bdem_ElemRef& value) const
{
    return assignToNillableScalarArrayImp(value);
}

template <>
inline
int bcem_AggregateRaw::assignToNillableScalarArray(
                                          const bdem_ConstElemRef& value) const
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
        return bcem_AggregateError::BCEM_ERR_NON_CONFORMANT;
    }

    const int length = static_cast<int>(value.size());
    bcem_AggregateError errorDescription;
    if (0 != resize(&errorDescription, length)) {
        return errorDescription.code();
    }
    if (0 == length) {
        return 0;
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

// MANIPULATORS
inline
void bcem_AggregateRaw::setDataType(bdem_ElemType::Type dataType)
{
    d_dataType = dataType;
}

inline
void bcem_AggregateRaw::setDataPointer(void *data)
{
    d_value_p = data;
}

inline
void bcem_AggregateRaw::setSchemaPointer(const bdem_Schema *data)
{
    d_schema_p = data;
}

inline
void bcem_AggregateRaw::setRecordDefPointer(const bdem_RecordDef *recordDef)
{
    d_recordDef = recordDef;
}

inline
void bcem_AggregateRaw::setFieldDefPointer(const bdem_FieldDef *fieldDef)
{
    d_fieldDef = fieldDef;
}

inline
void bcem_AggregateRaw::setTopLevelAggregateNullnessPointer(int *pointer)
{
    d_isTopLevelAggregateNull_p = pointer;
}

template <typename TYPE>
int bcem_AggregateRaw::setValue(bcem_AggregateError* errorDescription, 
                                const TYPE& value) const
{
    if (bdem_SchemaUtil::isNillableScalarArrayDescription(dataType(), 
                                                          recordConstraint())) 
    {
        if (isNull()) {
            makeValue();
        }

        if (0 != assignToNillableScalarArray(value)) {
            errorDescription->description().assign(
                                        "Value does not conform to record \"");
            errorDescription->description() += 
                bcem_AggregateRawUtil::recordName(recordConstraint());
            errorDescription->description() += "\" in schema";
            errorDescription->code() = 
                bcem_AggregateError::BCEM_ERR_NON_CONFORMANT;
            return 1;
        }
        return 0;
    }

    if (! bcem_AggregateRawUtil::isConformant(&value, recordConstraint())) {
        errorDescription->description().assign(
                                        "Value does not conform to record \"");
        errorDescription->description() += 
            bcem_AggregateRawUtil::recordName(recordConstraint());
        errorDescription->description() += "\" in schema";
        errorDescription->code() = 
            bcem_AggregateError::BCEM_ERR_NON_CONFORMANT;
        return 1;
    }

    if (enumerationConstraint() && bdem_ElemType::isScalarType(dataType())) {
        return toEnum(errorDescription, value);
    }
    else {
        bdem_ElemRef elemRef = asElemRef();
        if (0 != bdem_Convert::convert(&elemRef, value)) {
            errorDescription->description().assign(
                                  "Invalid conversion when setting ");
            errorDescription->description() += 
                                  bdem_ElemType::toAscii(dataType());
            errorDescription->description() += " value from ";
            errorDescription->description() +=
                                  bdem_ElemType::toAscii(getBdemType(value));
            errorDescription->description() += " value";
            errorDescription->code() = 
                                  bcem_AggregateError::BCEM_ERR_BAD_CONVERSION;
            return 1;
        }
    }

    return 0;
}

template <>
inline
int bcem_AggregateRaw::setValue<bcem_AggregateRaw>(
                                   bcem_AggregateError *errorDescription, 
                                   const bcem_AggregateRaw& value) const
{
    // Specialization for 'VALUETYPE = bcem_AggregateRaw'.
    return setValue(errorDescription, value.asElemRef());
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

    if (recordConstraint() && !bcem_AggregateRawUtil::isConformant(
                                                         d_value_p,
                                                         d_dataType,
                                                         d_recordDef)) {
        makeNull();  // discard data
        stream.invalidate();
    }

    return stream;
}

template <typename VALTYPE>
int bcem_AggregateRaw::insertItem(bcem_AggregateRaw   *newItem, 
                                  bcem_AggregateError *description, 
                                  int                  index, 
                                  const VALTYPE&       value) const
{
    bool wasNull = isNull();
    if (0 != insertItemRaw(newItem, description, index)) {
        return 1;
    }

    int rc = newItem->setValue(description, value);
    if (0 != rc && wasNull) {
        makeNull();
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
                       static_cast<const bdem_ChoiceArrayItem*>(d_value_p);
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

inline
bool bcem_AggregateRaw::isVoid() const
{
    return bdem_ElemType::BDEM_VOID == d_dataType;
}

inline
const void *bcem_AggregateRaw::data() const
{
    return d_value_p;
}

inline
const bdem_Schema *bcem_AggregateRaw::schema() const
{
    return d_schema_p;
}

inline
const bdem_RecordDef& bcem_AggregateRaw::recordDef() const
{
    return *d_recordDef;
}

inline
const bdem_RecordDef *bcem_AggregateRaw::recordDefPtr() const 
{
    if (0 == d_schema_p) {
        return 0;
    }
    return d_recordDef;
}

inline
const bdem_RecordDef *bcem_AggregateRaw::recordConstraint() const
{
    return d_recordDef;
}

inline
const bdem_EnumerationDef *bcem_AggregateRaw::enumerationConstraint() const
{
    return d_fieldDef ? d_fieldDef->enumerationConstraint() : 0;
}

inline
bsl::ostream& operator<<(bsl::ostream& stream, const bcem_AggregateRaw& rhs)
{
    return rhs.print(stream, 0, -1);
}

inline
const bdem_FieldDef *bcem_AggregateRaw::fieldDef() const
{
    return d_fieldDef;
}

inline
int bcem_AggregateRaw::maxSupportedBdexVersion()
{
    return 3;
}

inline
int bcem_AggregateRaw::size() const 
{
    return length();
}

inline
int bcem_AggregateRaw::arrayItem(bcem_AggregateRaw *item, 
                                    bcem_AggregateError     *errorDescription, 
                                    int                   index) const
{
    *item = *this;
    return item->descendIntoArrayItem(errorDescription, index, false);
}

inline
bool bcem_AggregateRaw::isError() const
{
    // An error state is detectable as a void object with a non-null 'd_value'.
    // The 'd_value' holds the error record.

    return bdem_ElemType::BDEM_VOID == d_dataType && d_value_p;
}

inline
bdem_ElemType::Type bcem_AggregateRaw::dataType() const
{
    return d_dataType;
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
