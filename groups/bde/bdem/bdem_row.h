// bdem_row.h                                                         -*-C++-*-
#ifndef INCLUDED_BDEM_ROW
#define INCLUDED_BDEM_ROW

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a memory buffer for arbitrary self-describing 'bdem' data.
//
//@CLASSES:
//  bdem_Row: indexable, heterogeneous, streamable sequence of elements
//
//@SEE_ALSO: bdem_list, bdem_table, bdem_elemref, bdem_elemtype
//
//@AUTHOR: Pablo Halpern (phalpern)    (originally by Dan Glaser)
//
//@DESCRIPTION: This component implements a container class, 'bdem_Row', that
// can hold an arbitrary sequence of elements of any of the 'bdem' types
// enumerated in 'bdem_ElemType' (see the table below).
//
///Logical Rows and 'bdem_Row'
///---------------------------
// A 'bdem_Row' object is designed to represent the *common* aspects of the
// data structure of the proper value-semantic 'bdem' aggregate containers,
// 'bdem_List' and 'bdem_Table'.  In the 'bdem' design, a list is exactly one
// *logical* row, and a table is an array of zero or more homogeneous *logical*
// rows -- i.e., rows having (exactly) the same sequence of element types.
// As a result of this constraint on rows in a table, two logical rows having
// different sequences of element types behave as different *logical* types.
// For a variety of reasons, we do not enforce this type difference via the C++
// language.  Instead, we use 'bdem_Row' as a concrete representation of an
// arbitrary logical row, and give to 'bdem_Row' several properties that might
// at first seem unfortunate, but upon closer examination turn out to be
// practical and useful.
//
// Due to its specific intended use, 'bdem_Row' has a relatively limited
// interface.  Individual elements can be accessed by index, the element
// values can be modified, and the entire row can be streamed; however, there
// are no methods for changing the structure (i.e., the sequence of element
// types) of the row.  Moreover, 'bdem_Row' is not a value-semantic type, but
// rather behaves more like a "reference-semantic" type (see below).  For
// example, there are no public constructors (the only way to create a row is
// to cast a pointer to 'bdem_RowData' to a 'bdem_Row *'), nor is there a
// destructor; the assignment operator has also been suppressed to help
// enforce "logical row" type (i.e., structure) safety.  The 'replaceValues'
// method serves as "assignment" for the "logical-type-safe" row.
//
// With this intentionally limited interface, 'bdem_Row' serves as a useful
// proxy for the logical row of either a list or a table; 'bdem_Row' appears
// (in name only) frequently in the signatures of 'bdem_List' and 'bdem_Table',
// enabling efficient interoperability between the two types with respect to
// logical rows.  Note the very strong analogy, in these aggregate interfaces,
// between 'bdem_ElemRef' as a proxy for any one 'bdem' element (type and
// value) and 'bdem_Row' as a proxy for any one *sequence* of 'bdem' elements
// (types and values).  'bdem_Row' may be regarded as a "reference semantic"
// type, except that, for practical reasons, a row supports 'bdex' streaming
// of its "value" as if it were a proper value-semantic container.  See the
// 'bdem_elemref' component for a discussion of reference semantics.
//
// Also note that all of the elements that can be *contained* within a row are
// proper value-semantic types.  The following table shows the 32 element types
// and their respective 'bdem_ElemType' enumerators (with the 'BDEM_' prefix
// elided), along with their convenient partition into (fixed) scalar,
// (homogeneous) array, and (heterogeneous) aggregate types:
//..
//                   ENUMERATOR         C++ TYPE
//                   ----------         --------
//  (fixed)
//          SCALAR:  BOOL               bool
//                   CHAR               char
//                   SHORT              short
//                   INT                int
//                   INT64              int64
//                   FLOAT              float
//                   DOUBLE             double
//                   STRING             bsl::string
//                   DATETIME           bdet_Datetime
//                   DATETIMETZ         bdet_DatetimeTz
//                   DATE               bdet_Date
//                   DATETZ             bdet_DateTz
//                   TIME               bdet_Time
//                   TIMETZ             bdet_TimeTz
//
//  (homogeneous)
//           ARRAY:  BOOL_ARRAY         bsl::vector<bool>
//                   CHAR_ARRAY         bsl::vector<char>
//                   SHORT_ARRAY        bsl::vector<short>
//                   INT_ARRAY          bsl::vector<int>
//                   INT64_ARRAY        bsl::vector<bsls_Types::Int64>
//                   FLOAT_ARRAY        bsl::vector<float>
//                   DOUBLE_ARRAY       bsl::vector<double>
//                   STRING_ARRAY       bsl::vector<bsl::string>
//                   DATETIME_ARRAY     bsl::vector<bdet_Datetime>
//                   DATETIMETZ_ARRAY   bsl::vector<bdet_DatetimeTz>
//                   DATE_ARRAY         bsl::vector<bdet_Date>
//                   DATETZ_ARRAY       bsl::vector<bdet_DateTz>
//                   TIME_ARRAY         bsl::vector<bdet_Time>
//                   TIMETZ_ARRAY       bsl::vector<bdet_TimeTz>
//
//  (heterogeneous)
//       AGGREGATE:  CHOICE             bdem_Choice
//                   CHOICE_ARRAY       bdem_ChoiceArray
//                   LIST               bdem_List
//                   TABLE              bdem_Table
//..
// Both SCALAR and ARRAY types are considered *leaf* as opposed to *aggregate*
// types.  Leaf types are not recursive -- i.e., are not able to contain
// objects of their own type as elements -- and conform to a specific
// fixed (or homogeneous) structure.  AGGREGATE types, on the other hand, are
// recursive, and therefore are able to contain elements of their own, or any
// other element type.  In particular, the list and table types manage (at
// least conceptually) their internal data in terms of one (in the case of a
// list) or a homogeneous sequence (in the case of a table) of 'bdem_Row'
// objects, which in turn, can contain elements of any of the above types,
// including (in addition to leaf types) choices, choice arrays, lists, and
// tables (to any arbitrary degree of nesting).  Note, however, that a row
// cannot be made to contain itself, either directly or indirectly, nor can an
// element belong to more than one row.
//
///'bdem' Null States
///------------------
// The concept of null applies to each 'bdem' type.  In addition to the range
// of values in a given 'bdem' type's domain (e.g., '[ INT_MIN .. INT_MAX ]'
// for 'BDEM_INT'), each type has a null value.  When a 'bdem' element is null,
// it has an underlying (unique) designated unset value (or state) as indicated
// in the following table:
//..
//       'bdem' element type                 "unset" value (or state)
//  ------------------------------    --------------------------------------
//  BDEM_CHOICE and
//  BDEM_CHOICE_ARRAY_ITEM            selector() < 0 && 0 == numSelections()
//
//  BDEM_CHOICE_ARRAY                 0 == length()  && 0 == numSelections()
//
//  BDEM_LIST                         0 == length()
//
//  BDEM_ROW                          All the elements in the row are "null"
//
//  BDEM_TABLE                        0 == numRows() && 0 == numColumns()
//
//  scalar (BDEM_INT, etc.)           bdetu_Unset<TYPE>::unsetValue()
//
//  vector (BDEM_INT_ARRAY, etc.)     0 == size()
//..
// To illustrate, consider a 'BDEM_BOOL' element within a 'bdem_Row'.  The
// element can be in one of three possible states:
//..
//  * null with underlying value 'bdetu_Unset<bool>::unsetValue()'
//  * non-null with underlying value 'false'
//  * non-null with underlying value 'true'
//..
// The underlying value of a null 'bdem' object is a class invariant.  If an
// object is null, that object *also* has the unset value corresponding to its
// type.
//
// For example, suppose we have a 'bdem_Row', 'myRow', whose first element is
// of type 'BDEM_STRING':
//..
//  myRow.theModifiableString(0) = "HELLO";
//  assert(myRow[0].isNonNull());
//..
// Making the element null also makes it have the unset value (which, for
// 'BDEM_STRING', is the empty string):
//..
//  myRow[0].makeNull();
//  assert(myRow[0].isNull());
//  assert(myRow[0].theString().empty());
//..
///Row Manipulation
/// - - - - - - - -
// The preferred way to assign a value to an element within a row is through
// calling the 'theModifiableTYPE' methods (e.g., 'theModifiableBool(index)')
// to first obtain a reference to the modifiable element.  The
// 'theModifiableTYPE' methods have the side-effect of making a null value
// non-null, so the 'const' 'theTYPE' methods should be used instead if the
// value is only being accessed (and *not* modified).
//
// The 'operator[]' manipulator provides an alternate means for assigning a
// value to an element.  In particular, unlike the 'theModifiableTYPE' methods,
// the 'bdem_ElemRef' that is returned by 'operator[]' can be used to assign
// both the null value and non-null values to an element.  The 'makeNull'
// method provides the most direct way to make a specified element within a row
// null.
//
///Usage
///-----
// A row is not typically a first-class object, but is instead created and
// owned by a parent object such as a 'bdem_List' or 'bdem_Table'.  In this
// example we first illustrate how to create a row directly (most users will
// not generally need to follow this example, but rather will obtain rows from
// higher-level 'bdem' components).  Then we will demonstrate more typical row
// usage by getting and setting member elements.
//
// In order to create a row directly, we will need an array of descriptors
// (of type 'bdem_Descriptor'), which is in turn used to characterize the
// salient behavior of each element we plan to instantiate.  For this example,
// we propose to create a row containing a sequence of elements corresponding
// to the 'bdem_ElemType' enumerators 'BDEM_DOUBLE', 'BDEM_INT', 'BDEM_STRING',
// and 'BDEM_STRING_ARRAY'.  First, we create an array, 'ETA', of enumerators:
//..
//  static const bdem_ElemType::Type ETA[] = {
//      bdem_ElemType::BDEM_DOUBLE,
//      bdem_ElemType::BDEM_INT,
//      bdem_ElemType::BDEM_STRING,
//      bdem_ElemType::BDEM_STRING_ARRAY
//  };
//..
// Next, we create an array, 'EDA', of pointers to 'bdem_Descriptor' values.
// We need one appropriate pointer for each *distinct* element type in 'ETA'.
// The array index of that descriptor pointer must match the 'bdem_ElemType'
// enumerator value corresponding to its type:
//..
//  static const bdem_Descriptor *EDA[] = {
//      0, 0,
//      &bdem_Properties::d_intAttr,         /* BDEM_INT    == 2 */
//      0, 0,
//      &bdem_Properties::d_doubleAttr,      /* BDEM_DOUBLE == 5 */
//      &bdem_Properties::d_stringAttr,
//      0, 0, 0, /* arrays: */ 0, 0, 0, 0, 0, 0,
//      &bdem_Properties::d_stringArrayAttr
//  };
//..
// Next we create a 'bdem_RowLayout' object, 'mRL', that defines the layout of
// the 'bdem' elements in the rows that are created later:
//..
//  bdem_RowLayout mRL(ETA,         // array of element type enumerators
//                     4,           // length of row
//                     EDA,         // (sparse) array of element descriptors
//                     bslma_Default::defaultAllocator());
//..
// Given 'mRL', we now instantiate two (modifiable) row definitions, 'mRD1' and
// 'mRD2':
//..
//  bdem_RowData mRD1(&mRL,
//                    bdem_AggregateOption::BDEM_PASS_THROUGH,
//                    bslma_Default::defaultAllocator());
//
//  bdem_RowData mRD2(&mRL,
//                    bdem_AggregateOption::BDEM_PASS_THROUGH,
//                    bslma_Default::defaultAllocator());
//
//..
// By design, a 'bdem_Row' contains exactly one data member of type
// 'bdem_RowData'.  Casting the address of a 'bdem_RowData' to a 'bdem_Row *'
// provides a limited "view" of the row data that is appropriate for typical
// general users (such as clients of lists and tables):
//..
//  bdem_Row &row1 = *reinterpret_cast<bdem_Row *>(&mRD1);
//  bdem_Row &row2 = *reinterpret_cast<bdem_Row *>(&mRD2);
//..
// We observe that these two rows, each of length 4, are initially identical:
//..
//  assert(4    == row1.length());
//  assert(4    == row2.length());
//  assert(row1 == row2);
//..
// Each element of both rows has been initialized to its null value.  Printing
// either row, say, 'row1':
//..
//   bsl::cout << row1 << bsl::endl;
//..
// yields the following output:
//..
//  { DOUBLE NULL INT NULL STRING NULL STRING_ARRAY NULL }
//..
// The next thing we'll do is assign values to the individual elements of the
// rows.  There are two distinct styles of accessing elements.  The first style
// identifies a reference to the specific element directly:
//..
//  double& doubleRef           = row1.theModifiableDouble(0);
//  doubleRef                   = 2.5;    assert(    2.5 == row1.theDouble(0));
//  row1.theModifiableInt(1)    = 34;     assert(     34 == row1.theInt(1));
//  row1.theModifiableString(2) = "hello";
//                                        assert("hello" == row1.theString(2));
//..
// Note that the two rows still have the same length, but not the same value:
//..
//  assert(row1.length() == row2.length());  assert(row1 != row2);
//..
// The second style of access returns a 'bdem_ElemRef' by value.  This "proxy"
// can then be used to access the element:
//..
//  bdem_ElemRef elemRef0          = row2[0];
//  elemRef0.theModifiableDouble() = 2.5;
//
//  bdem_ElemRef elemRef1          = row2[1];
//  elemRef1.theModifiableInt()    = 34;
//
//  bdem_ElemRef elemRef2          = row2[2];
//  elemRef2.theModifiableString() = "hello";
//..
// Note that 'row1' again has the same value as 'row2':
//..
//  assert(row1.length() == row2.length());  assert(row1 == row2);
//..
// which when printed:
//..
//  bsl::cout << row2 << bsl::endl;
//..
// yields:
//..
//  { DOUBLE 2.5 INT 34 STRING hello STRING_ARRAY NULL }
//..
// As another example, we might choose to append a few strings to the empty
// string array at the end of 'row2':
//..
//  row2.theModifiableStringArray(3).push_back("alpha");
//  row2[3].theModifiableStringArray().push_back("beta");
//..
// which when printed:
//..
//  bsl::cout << row2 << bsl::endl;
//..
// yields:
//..
//  { DOUBLE 2.5 INT 34 STRING hello STRING_ARRAY [ alpha beta ] }
//..
// Note that we can also assign directly to the element of an array:
//..
//  row2.theModifiableStringArray(3)[1] = "gamma";
//..
// which when printed:
//..
//  bsl::cout << row2 << bsl::endl;
//..
// yields:
//..
//  { DOUBLE 2.5 INT 34 STRING hello STRING_ARRAY [ alpha gamma ] }
//..
// As a final example, we reset the second element in 'row2' back to null:
//..
//  row2.makeNull(1);
//..
// which when printed:
//..
//  bsl::cout << row2 << bsl::endl;
//..
// now yields:
//..
//  { DOUBLE 2.5 INT NULL STRING hello STRING_ARRAY [ alpha gamma ] }
//..

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BDEM_ELEMATTRLOOKUP
#include <bdem_elemattrlookup.h>
#endif

#ifndef INCLUDED_BDEM_ELEMREF
#include <bdem_elemref.h>
#endif

#ifndef INCLUDED_BDEM_ELEMTYPE
#include <bdem_elemtype.h>
#endif

#ifndef INCLUDED_BDEM_ROWDATA
#include <bdem_rowdata.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSLS_TYPES
#include <bsls_types.h>
#endif

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif

#ifndef INCLUDED_BSL_STRING
#include <bsl_string.h>
#endif

#ifndef INCLUDED_BSL_VECTOR
#include <bsl_vector.h>
#endif

namespace BloombergLP {

class bdem_Choice;
class bdem_ChoiceArray;
class bdem_List;
class bdem_Table;

class bdem_Datetime;
class bdem_DatetimeTz;
class bdem_Date;
class bdem_DateTz;
class bdem_Time;
class bdem_TimeTz;

                        // ==============
                        // class bdem_Row
                        // ==============

class bdem_Row {
    // This class represents an indexable, heterogeneous sequence of values
    // of arbitrary 'bdem' types.  Although intended for direct use by
    // clients, objects of this type are typically generated by, and as part
    // of, an aggregate container (e.g., 'bdem_List' or 'bdem_Table').
    //
    // Note that this class, by itself, is *not* a value-semantic type.
    // Although 'operator==' and 'bdex'-compatible streaming are supported,
    // all constructors, the destructor, and the copy assignment operator are
    // suppressed.  In particular, the only way to create a 'bdem_Row' object
    // is to cast the address of a 'bdem_RowData' object to 'bdem_Row *'.
    // Consequently, the 'sizeof' operator does not reflect the actual size of
    // of a 'bdem_Row', which may vary with the number of objects contained.
    // This behavior supports the effective use of 'bdem_Row' as a proxy for
    // the logical rows of lists and tables.

    // DATA
    bdem_RowData d_rowData;  // holds all items contributing to size and
                             // alignment

    // FRIENDS
    friend bool operator==(const bdem_Row&, const bdem_Row&);

  private:
    // PRIVATE MANIPULATORS
    void *elemData(int index);
        // Return the address of the modifiable element at the specified
        // 'index' position in this row.  The nullness bit corresponding to
        // the element at 'index' is reset to 0.  The behavior is undefined
        // unless '0 <= index < length()'.

    // PRIVATE ACCESSORS
    const void *elemData(int index) const;
        // Return the address of the non-modifiable element at the specified
        // 'index' position in this row.  The nullness bit corresponding to
        // the element at 'index' is unaffected.  The behavior is undefined
        // unless '0 <= index < length()'.

  private:
    // NOT IMPLEMENTED
    bdem_Row();
    bdem_Row(const bdem_Row&);
    ~bdem_Row();
    bdem_Row& operator=(const bdem_Row&);

  public:
    // CLASS METHODS
    static int maxSupportedBdexVersion();
        // Return the most current 'bdex' streaming version number supported by
        // this class.  (See the package-group-level documentation for more
        // information on 'bdex' streaming of container types.)

    // MANIPULATORS
    bdem_ElemRef operator[](int index);
        // Return a 'bdem' element reference to the modifiable element at the
        // specified 'index' position in this row.  The behavior is undefined
        // unless '0 <= index < length()'.  Note that a 'bdem_ElemRef' object
        // does not permit the type of its element to be modified.

    bdem_Row& replaceValues(const bdem_Row& other);
        // Replace the values of the elements in this row with the values of
        // the elements in the specified 'other' row, and return a reference to
        // this modifiable row.  The behavior is undefined unless the length
        // and sequence of element types in 'other' is the same as in this row.

    void makeAllNull();
        // Set the value of each element in this row to null.  Note that, if
        // accessed, the values will be the unset values for their
        // corresponding element types (see 'bdetu_unset').

    void makeNull(int index);
        // Set the value of the element at the specified 'index' in this row
        // to null.  The behavior is undefined unless '0 <= index < length()'.
        // Note that, if accessed, the value will be the corresponding unset
        // value for its element type (see 'bdetu_unset').

    bool& theModifiableBool(int index);
    char& theModifiableChar(int index);
    short& theModifiableShort(int index);
    int& theModifiableInt(int index);
    bsls_Types::Int64& theModifiableInt64(int index);
    float& theModifiableFloat(int index);
    double& theModifiableDouble(int index);
    bsl::string& theModifiableString(int index);
    bdet_Datetime& theModifiableDatetime(int index);
    bdet_DatetimeTz& theModifiableDatetimeTz(int index);
    bdet_Date& theModifiableDate(int index);
    bdet_DateTz& theModifiableDateTz(int index);
    bdet_Time& theModifiableTime(int index);
    bdet_TimeTz& theModifiableTimeTz(int index);
    bsl::vector<bool>& theModifiableBoolArray(int index);
    bsl::vector<char>& theModifiableCharArray(int index);
    bsl::vector<short>& theModifiableShortArray(int index);
    bsl::vector<int>& theModifiableIntArray(int index);
    bsl::vector<bsls_Types::Int64>& theModifiableInt64Array(int index);
    bsl::vector<float>& theModifiableFloatArray(int index);
    bsl::vector<double>& theModifiableDoubleArray(int index);
    bsl::vector<bsl::string>& theModifiableStringArray(int index);
    bsl::vector<bdet_Datetime>& theModifiableDatetimeArray(int index);
    bsl::vector<bdet_DatetimeTz>& theModifiableDatetimeTzArray(int index);
    bsl::vector<bdet_Date>& theModifiableDateArray(int index);
    bsl::vector<bdet_DateTz>& theModifiableDateTzArray(int index);
    bsl::vector<bdet_Time>& theModifiableTimeArray(int index);
    bsl::vector<bdet_TimeTz>& theModifiableTimeTzArray(int index);
    bdem_Choice& theModifiableChoice(int index);
    bdem_ChoiceArray& theModifiableChoiceArray(int index);
    bdem_List& theModifiableList(int index);
    bdem_Table& theModifiableTable(int index);
        // Return a reference to the modifiable 'bdem' element at the specified
        // 'index' position in this row.  If the indexed element is null, it is
        // made non-null before returning, but its value remains the
        // corresponding unset value for that type (see 'bdetu_unset').  The
        // behavior is undefined unless '0 <= index < length()' and the
        // type of the element at 'index' is of the type indicated by the name
        // of the method used.  Note that unless there is an intention of
        // modifying the element, the corresponding 'theTYPE' method should be
        // used instead.  Also note that for a given modifiable row 'mR' and a
        // given 'bdem' 'TYPE', the expression 'mR.theModifiableTYPE(index)'
        // has the same side-effect, and returns a reference to the same
        // element, as 'mR[index].theModifiableTYPE()'.

    template <class STREAM>
    STREAM& bdexStreamIn(STREAM& stream, int version);
        // Assign the data elements in this row the values read from the
        // specified input 'stream' using the specified 'version' format and
        // return a reference to the modifiable 'stream'.  The row definition
        // (sequence of element types) is not changed and must already match
        // the sequence of elements being read.  If 'stream' is initially
        // invalid, this operation has no effect.  If 'stream' becomes invalid
        // during this operation, this object is valid, but its element values
        // are unspecified.  If the specified 'version' is not supported,
        // 'stream' is marked invalid, but this object is unaltered.  Note
        // that no version is read from 'stream'.
        //
        // (See the package-group-level documentation for more information on
        // 'bdex' streaming of container types.)

    // ACCESSORS
    bdem_ConstElemRef operator[](int index) const;
        // Return a 'bdem' element reference to the non-modifiable element at
        // the specified 'index' position in this row.  The behavior is
        // undefined unless '0 <= index < length()'.

    bdem_ElemType::Type elemType(int index) const;
        // Return the 'bdem' type of the element at the specified 'index'
        // position in this row.  The behavior is undefined unless
        // '0 <= index < length()'.

    int length() const;
        // Return the number of elements in this row.

    bool isAnyNull() const;
        // Return 'true' if the value of any element in this row is null, and
        // 'false' otherwise.  Note that an element that is null is also
        // considered to be unset.

    bool isAnyNonNull() const;
        // Return 'true' if the value of any element in this row is non-null,
        // and 'false' otherwise.

    const bool& theBool(int index) const;
    const char& theChar(int index) const;
    const short& theShort(int index) const;
    const int& theInt(int index) const;
    const bsls_Types::Int64& theInt64(int index) const;
    const float& theFloat(int index) const;
    const double& theDouble(int index) const;
    const bsl::string& theString(int index) const;
    const bdet_Datetime& theDatetime(int index) const;
    const bdet_DatetimeTz& theDatetimeTz(int index) const;
    const bdet_Date& theDate(int index) const;
    const bdet_DateTz& theDateTz(int index) const;
    const bdet_Time& theTime(int index) const;
    const bdet_TimeTz& theTimeTz(int index) const;
    const bsl::vector<bool>& theBoolArray(int index) const;
    const bsl::vector<char>& theCharArray(int index) const;
    const bsl::vector<short>& theShortArray(int index) const;
    const bsl::vector<int>& theIntArray(int index) const;
    const bsl::vector<bsls_Types::Int64>& theInt64Array(int index) const;
    const bsl::vector<float>& theFloatArray(int index) const;
    const bsl::vector<double>& theDoubleArray(int index) const;
    const bsl::vector<bsl::string>& theStringArray(int index) const;
    const bsl::vector<bdet_Datetime>& theDatetimeArray(int index) const;
    const bsl::vector<bdet_DatetimeTz>& theDatetimeTzArray(int index) const;
    const bsl::vector<bdet_Date>& theDateArray(int index) const;
    const bsl::vector<bdet_DateTz>& theDateTzArray(int index) const;
    const bsl::vector<bdet_Time>& theTimeArray(int index) const;
    const bsl::vector<bdet_TimeTz>& theTimeTzArray(int index) const;
    const bdem_Choice& theChoice(int index) const;
    const bdem_ChoiceArray& theChoiceArray(int index) const;
    const bdem_List& theList(int index) const;
    const bdem_Table& theTable(int index) const;
        // Return a reference to the non-modifiable 'bdem' element at the
        // specified 'index' position in this row.  The nullness of the indexed
        // element is not affected.  The behavior is undefined unless
        // '0 <= index < length()' and the type of the element at 'index' is of
        // the type indicated by the name of the method used.  Note that for a
        // given non-modifiable row 'R' and a given 'bdem' 'TYPE', the
        // expression 'R.theTYPE(index)' returns a reference to the same
        // element as 'R[index].theTYPE()'.

    template <class STREAM>
    STREAM& bdexStreamOut(STREAM& stream, int version) const;
        // Write the value of this row's elements to the specified output
        // 'stream' using the streaming format specified by 'version'.  Return
        // a reference to the modifiable 'stream'.  The 'version' is *not*
        // written to 'stream'.  If 'version' is not supported, 'stream' is
        // left unmodified.  Note that only the data elements, not the row
        // definition, is written to the stream.
        //
        // (See the package-group-level documentation for more information on
        // 'bdex' streaming of container types).

    bsl::ostream& print(bsl::ostream& stream,
                        int           level = 0,
                        int           spacesPerLevel = 4) const;
        // Format this row object to the specified output 'stream' at the
        // (absolute value of) the optionally specified indentation 'level' and
        // return a reference to the modifiable 'stream'.  If 'level' is
        // specified, optionally specify 'spacesPerLevel', the number of spaces
        // per indentation level for this and all of its nested objects.  If
        // 'level' is negative, suppress indentation of the first line.  If
        // 'spacesPerLevel' is negative, format the entire output on one line,
        // suppressing all but the initial indentation (as governed by
        // 'level').  If 'stream' is not valid on entry, this operation has no
        // effect.

#ifndef BDE_OMIT_INTERNAL_DEPRECATED

    static int maxSupportedVersion();
        // Return the most current 'bdex' streaming version number supported by
        // this class.  (See the package-group-level documentation for more
        // information on 'bdex' streaming of container types.)
        //
        // DEPRECATED: Use 'maxSupportedBdexVersion' instead.

#endif
};

// FREE OPERATORS
bool operator==(const bdem_Row& lhs, const bdem_Row& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' rows have the same value,
    // and 'false' otherwise.  Two rows have the same value if they have the
    // same number of elements and corresponding elements at each index
    // position have the same type and value.

bool operator!=(const bdem_Row& lhs, const bdem_Row& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' rows do not have the same
    // value, and 'false' otherwise.  Two rows do not have the same value if
    // they do not have the same number of elements, or there are corresponding
    // elements at some index position that do not have the same type or value.

bsl::ostream& operator<<(bsl::ostream& stream, const bdem_Row& row);
    // Format the value of the specified 'row', unindented on a single line,
    // to the specified output 'stream', and return a reference to 'stream'.
    // If stream is not valid on entry, this operation has no effect.  Note
    // that this human-readable format is the same as what would result from
    // invoking:
    //..
    //  row.print(stream, 0, -1);
    //..
    // which is not fully specified, and subject to change without notice.

// ===========================================================================
//                      INLINE FUNCTION DEFINITIONS
// ===========================================================================

                        // --------------
                        // class bdem_Row
                        // --------------

// PRIVATE MANIPULATORS
inline
void *bdem_Row::elemData(int index)
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(index < d_rowData.length());

    return d_rowData.elemData(index);
}

// PRIVATE ACCESSORS
inline
const void *bdem_Row::elemData(int index) const
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(index < d_rowData.length());

    return d_rowData.elemData(index);
}

// CLASS METHODS
inline
int bdem_Row::maxSupportedBdexVersion()
{
    return 3;
}

// MANIPULATORS
inline
bdem_Row& bdem_Row::replaceValues(const bdem_Row& other)
{
    if (this != &other) {
        const bdem_RowData& srcRowData = (const bdem_RowData&) other;

        d_rowData.replaceValues(srcRowData);
    }
    return *this;
}

inline
void bdem_Row::makeAllNull()
{
    d_rowData.makeAllNull();
}

inline
void bdem_Row::makeNull(int index)
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(index < d_rowData.length());

    d_rowData.makeNull(index);
}

template <class STREAM>
inline
STREAM& bdem_Row::bdexStreamIn(STREAM& stream, int version)
{
    // Stream in only the data, not the structure of the row.

    return d_rowData.bdexStreamInImp(
                            stream,
                            version,
                            bdem_ElemStreamInAttrLookup<STREAM>::lookupTable(),
                            bdem_ElemAttrLookup::lookupTable());
}

inline
bdem_ElemRef bdem_Row::operator[](int index)
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(index < d_rowData.length());

    return d_rowData.elemRef(index);
}

inline
bool& bdem_Row::theModifiableBool(int index)
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(index < d_rowData.length());

    return *static_cast<bool *>(elemData(index));
}

inline
char& bdem_Row::theModifiableChar(int index)
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(index < d_rowData.length());

    return *static_cast<char *>(elemData(index));
}

inline
short& bdem_Row::theModifiableShort(int index)
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(index < d_rowData.length());

    return *static_cast<short *>(elemData(index));
}

inline
int& bdem_Row::theModifiableInt(int index)
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(index < d_rowData.length());

    return *static_cast<int *>(elemData(index));
}

inline
bsls_Types::Int64& bdem_Row::theModifiableInt64(int index)
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(index < d_rowData.length());

    return *static_cast<bsls_Types::Int64 *>(elemData(index));
}

inline
float& bdem_Row::theModifiableFloat(int index)
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(index < d_rowData.length());

    return *static_cast<float *>(elemData(index));
}

inline
double& bdem_Row::theModifiableDouble(int index)
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(index < d_rowData.length());

    return *static_cast<double *>(elemData(index));
}

inline
bsl::string& bdem_Row::theModifiableString(int index)
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(index < d_rowData.length());

    return *static_cast<bsl::string *>(elemData(index));
}

inline
bdet_Datetime& bdem_Row::theModifiableDatetime(int index)
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(index < d_rowData.length());

    return *static_cast<bdet_Datetime *>(elemData(index));
}

inline
bdet_DatetimeTz& bdem_Row::theModifiableDatetimeTz(int index)
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(index < d_rowData.length());

    return *static_cast<bdet_DatetimeTz *>(elemData(index));
}

inline
bdet_Date& bdem_Row::theModifiableDate(int index)
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(index < d_rowData.length());

    return *static_cast<bdet_Date *>(elemData(index));
}

inline
bdet_DateTz& bdem_Row::theModifiableDateTz(int index)
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(index < d_rowData.length());

    return *static_cast<bdet_DateTz *>(elemData(index));
}

inline
bdet_Time& bdem_Row::theModifiableTime(int index)
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(index < d_rowData.length());

    return *static_cast<bdet_Time *>(elemData(index));
}

inline
bdet_TimeTz& bdem_Row::theModifiableTimeTz(int index)
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(index < d_rowData.length());

    return *static_cast<bdet_TimeTz *>(elemData(index));
}

inline
bsl::vector<bool>& bdem_Row::theModifiableBoolArray(int index)
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(index < d_rowData.length());

    return *static_cast<bsl::vector<bool> *>(elemData(index));
}

inline
bsl::vector<char>& bdem_Row::theModifiableCharArray(int index)
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(index < d_rowData.length());

    return *static_cast<bsl::vector<char> *>(elemData(index));
}

inline
bsl::vector<short>& bdem_Row::theModifiableShortArray(int index)
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(index < d_rowData.length());

    return *static_cast<bsl::vector<short> *>(elemData(index));
}

inline
bsl::vector<int>& bdem_Row::theModifiableIntArray(int index)
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(index < d_rowData.length());

    return *static_cast<bsl::vector<int> *>(elemData(index));
}

inline
bsl::vector<bsls_Types::Int64>& bdem_Row::theModifiableInt64Array(int index)
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(index < d_rowData.length());

    return *static_cast<bsl::vector<bsls_Types::Int64> *>(elemData(index));
}

inline
bsl::vector<float>& bdem_Row::theModifiableFloatArray(int index)
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(index < d_rowData.length());

    return *static_cast<bsl::vector<float> *>(elemData(index));
}

inline
bsl::vector<double>& bdem_Row::theModifiableDoubleArray(int index)
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(index < d_rowData.length());

    return *static_cast<bsl::vector<double> *>(elemData(index));
}

inline
bsl::vector<bsl::string>& bdem_Row::theModifiableStringArray(int index)
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(index < d_rowData.length());

    return *static_cast<bsl::vector<bsl::string> *>(elemData(index));
}

inline
bsl::vector<bdet_Datetime>& bdem_Row::theModifiableDatetimeArray(int index)
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(index < d_rowData.length());

    return *static_cast<bsl::vector<bdet_Datetime> *>(elemData(index));
}

inline
bsl::vector<bdet_DatetimeTz>& bdem_Row::theModifiableDatetimeTzArray(int index)
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(index < d_rowData.length());

    return *static_cast<bsl::vector<bdet_DatetimeTz> *>(elemData(index));
}

inline
bsl::vector<bdet_Date>& bdem_Row::theModifiableDateArray(int index)
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(index < d_rowData.length());

    return *static_cast<bsl::vector<bdet_Date> *>(elemData(index));
}

inline
bsl::vector<bdet_DateTz>& bdem_Row::theModifiableDateTzArray(int index)
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(index < d_rowData.length());

    return *static_cast<bsl::vector<bdet_DateTz> *>(elemData(index));
}

inline
bsl::vector<bdet_Time>& bdem_Row::theModifiableTimeArray(int index)
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(index < d_rowData.length());

    return *static_cast<bsl::vector<bdet_Time> *>(elemData(index));
}

inline
bsl::vector<bdet_TimeTz>& bdem_Row::theModifiableTimeTzArray(int index)
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(index < d_rowData.length());

    return *static_cast<bsl::vector<bdet_TimeTz> *>(elemData(index));
}

inline
bdem_Choice& bdem_Row::theModifiableChoice(int index)
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(index < d_rowData.length());

    return *static_cast<bdem_Choice *>(elemData(index));
}

inline
bdem_ChoiceArray& bdem_Row::theModifiableChoiceArray(int index)
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(index < d_rowData.length());

    return *static_cast<bdem_ChoiceArray *>(elemData(index));
}

inline
bdem_List& bdem_Row::theModifiableList(int index)
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(index < d_rowData.length());

    return *static_cast<bdem_List *>(elemData(index));
}

inline
bdem_Table& bdem_Row::theModifiableTable(int index)
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(index < d_rowData.length());

    return *static_cast<bdem_Table *>(elemData(index));
}

// ACCESSORS
inline
int bdem_Row::length() const
{
    return d_rowData.length();
}

inline
bdem_ElemType::Type bdem_Row::elemType(int index) const
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(index < d_rowData.length());

    return (bdem_ElemType::Type)
                      (*d_rowData.rowLayout())[index].attributes()->d_elemEnum;
}

inline
bool bdem_Row::isAnyNull() const
{
    return d_rowData.isAnyInRangeNull(0, length());
}

inline
bool bdem_Row::isAnyNonNull() const
{
    return d_rowData.isAnyInRangeNonNull(0, length());
}

inline
bsl::ostream&
bdem_Row::print(bsl::ostream& stream, int level, int spacesPerLevel) const
{
    return d_rowData.printRow(stream, level, spacesPerLevel);
}

template <class STREAM>
inline
STREAM& bdem_Row::bdexStreamOut(STREAM& stream, int version) const
{
    // Stream just the data elements of this row, not the row definition.

    return d_rowData.bdexStreamOutImp(
                          stream,
                          version,
                          bdem_ElemStreamOutAttrLookup<STREAM>::lookupTable());
}

inline
bdem_ConstElemRef bdem_Row::operator[](int index) const
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(index < d_rowData.length());

    return d_rowData.elemRef(index);
}

inline
const bool& bdem_Row::theBool(int index) const
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(index < d_rowData.length());

    return *static_cast<const bool *>(elemData(index));
}

inline
const char& bdem_Row::theChar(int index) const
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(index < d_rowData.length());

    return *static_cast<const char *>(elemData(index));
}

inline
const short& bdem_Row::theShort(int index) const
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(index < d_rowData.length());

    return *static_cast<const short *>(elemData(index));
}

inline
const int& bdem_Row::theInt(int index) const
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(index < d_rowData.length());

    return *static_cast<const int *>(elemData(index));
}

inline
const bsls_Types::Int64& bdem_Row::theInt64(int index) const
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(index < d_rowData.length());

    return *static_cast<const bsls_Types::Int64 *>(elemData(index));
}

inline
const float& bdem_Row::theFloat(int index) const
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(index < d_rowData.length());

    return *static_cast<const float *>(elemData(index));
}

inline
const double& bdem_Row::theDouble(int index) const
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(index < d_rowData.length());

    return *static_cast<const double *>(elemData(index));
}

inline
const bsl::string& bdem_Row::theString(int index) const
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(index < d_rowData.length());

    return *static_cast<const bsl::string *>(elemData(index));
}

inline
const bdet_Datetime& bdem_Row::theDatetime(int index) const
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(index < d_rowData.length());

    return *static_cast<const bdet_Datetime *>(elemData(index));
}

inline
const bdet_DatetimeTz& bdem_Row::theDatetimeTz(int index) const
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(index < d_rowData.length());

    return *static_cast<const bdet_DatetimeTz *>(elemData(index));
}

inline
const bdet_Date& bdem_Row::theDate(int index) const
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(index < d_rowData.length());

    return *static_cast<const bdet_Date *>(elemData(index));
}

inline
const bdet_DateTz& bdem_Row::theDateTz(int index) const
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(index < d_rowData.length());

    return *static_cast<const bdet_DateTz *>(elemData(index));
}

inline
const bdet_Time& bdem_Row::theTime(int index) const
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(index < d_rowData.length());

    return *static_cast<const bdet_Time *>(elemData(index));
}

inline
const bdet_TimeTz& bdem_Row::theTimeTz(int index) const
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(index < d_rowData.length());

    return *static_cast<const bdet_TimeTz *>(elemData(index));
}

inline
const bsl::vector<bool>& bdem_Row::theBoolArray(int index) const
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(index < d_rowData.length());

    return *static_cast<const bsl::vector<bool> *>(elemData(index));
}

inline
const bsl::vector<char>& bdem_Row::theCharArray(int index) const
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(index < d_rowData.length());

    return *static_cast<const bsl::vector<char> *>(elemData(index));
}

inline
const bsl::vector<short>& bdem_Row::theShortArray(int index) const
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(index < d_rowData.length());

    return *static_cast<const bsl::vector<short> *>(elemData(index));
}

inline
const bsl::vector<int>& bdem_Row::theIntArray(int index) const
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(index < d_rowData.length());

    return *static_cast<const bsl::vector<int> *>(elemData(index));
}

inline
const bsl::vector<bsls_Types::Int64>& bdem_Row::theInt64Array(int index) const
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(index < d_rowData.length());

    return *static_cast<const bsl::vector<bsls_Types::Int64> *>
                                                             (elemData(index));
}

inline
const bsl::vector<float>& bdem_Row::theFloatArray(int index) const
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(index < d_rowData.length());

    return *static_cast<const bsl::vector<float> *>(elemData(index));
}

inline
const bsl::vector<double>& bdem_Row::theDoubleArray(int index) const
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(index < d_rowData.length());

    return *static_cast<const bsl::vector<double> *>(elemData(index));
}

inline
const bsl::vector<bsl::string>& bdem_Row::theStringArray(int index) const
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(index < d_rowData.length());

    return *static_cast<const bsl::vector<bsl::string> *>(elemData(index));
}

inline
const bsl::vector<bdet_Datetime>& bdem_Row::theDatetimeArray(int index) const
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(index < d_rowData.length());

    return *static_cast<const bsl::vector<bdet_Datetime> *>(elemData(index));
}

inline
const bsl::vector<bdet_DatetimeTz>&
bdem_Row::theDatetimeTzArray(int index) const
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(index < d_rowData.length());

    return *static_cast<const bsl::vector<bdet_DatetimeTz> *>(
                                                              elemData(index));
}

inline
const bsl::vector<bdet_Date>& bdem_Row::theDateArray(int index) const
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(index < d_rowData.length());

    return *static_cast<const bsl::vector<bdet_Date> *>(elemData(index));
}

inline
const bsl::vector<bdet_DateTz>& bdem_Row::theDateTzArray(int index) const
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(index < d_rowData.length());

    return *static_cast<const bsl::vector<bdet_DateTz> *>(elemData(index));
}

inline
const bsl::vector<bdet_Time>& bdem_Row::theTimeArray(int index) const
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(index < d_rowData.length());

    return *static_cast<const bsl::vector<bdet_Time> *>(elemData(index));
}

inline
const bsl::vector<bdet_TimeTz>& bdem_Row::theTimeTzArray(int index) const
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(index < d_rowData.length());

    return *static_cast<const bsl::vector<bdet_TimeTz> *>(elemData(index));
}

inline
const bdem_Choice& bdem_Row::theChoice(int index) const
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(index < d_rowData.length());

    return *static_cast<const bdem_Choice *>(elemData(index));
}

inline
const bdem_ChoiceArray& bdem_Row::theChoiceArray(int index) const
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(index < d_rowData.length());

    return *static_cast<const bdem_ChoiceArray *>(elemData(index));
}

inline
const bdem_List& bdem_Row::theList(int index) const
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(index < d_rowData.length());

    return *static_cast<const bdem_List *>(elemData(index));
}

inline
const bdem_Table& bdem_Row::theTable(int index) const
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(index < d_rowData.length());

    return *static_cast<const bdem_Table *>(elemData(index));
}

#ifndef BDE_OMIT_INTERNAL_DEPRECATED

inline
int bdem_Row::maxSupportedVersion()
{
    return maxSupportedBdexVersion();
}

#endif

// FREE OPERATORS
inline
bool operator==(const bdem_Row& lhs, const bdem_Row& rhs)
{
    return lhs.d_rowData == rhs.d_rowData;
}

inline
bool operator!=(const bdem_Row& lhs, const bdem_Row& rhs)
{
     return !(lhs == rhs);
}

inline
bsl::ostream& operator<<(bsl::ostream& stream, const bdem_Row& row)
{
    return row.print(stream, 0, -1);
}

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2010
//      All Rights Reserved.
//      Property of Bloomberg L.P.  (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
