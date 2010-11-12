// bdem_choice.h                                                      -*-C++-*-
#ifndef INCLUDED_BDEM_CHOICE
#define INCLUDED_BDEM_CHOICE

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a container holding one value from a set of possible types.
//
//@CLASSES:
//  bdem_Choice: container holding one value from a set of possible types
//
//@SEE_ALSO: bdem_row, bdem_list, bdem_table, bdem_elemref, bdem_choicearray,
//           bdem_choicearrayitem
//
//@AUTHOR: Pablo Halpern (phalpern), Anthony Comerico (acomeric)
//
//@DESCRIPTION: This component provides a value-semantic container class,
// 'bdem_Choice', capable of holding at most one value (the "current
// selection") of any 'bdem' type.  A 'bdem_Choice' contains the current
// selection object, an array of type IDs (the "types catalog"), and an index
// into the types catalog (the "selector") that specifies the type that the
// 'bdem_Choice' object is currently holding.  The interface closely follows
// that for 'bdem_List' with the major difference being that it holds only one
// value at a time.  Value-semantic operations such as copy construction,
// assignment, streaming, and equality operations are also provided.
//
// This class has methods for setting and appending to the types catalog, for
// setting and retrieving the selector, and for accessing and modifying the
// current selection.  When the selector is changed, the current selection is
// destroyed and a new current selection value is constructed.  A selector of
// '-1' indicates that there is no current selection.  Note that a
// 'bdem_Choice' manages the nullness of its current selection.  The current
// selection's nullability can be queried via a 'bdem_ElemRef' returned from
// 'selection'.  Please refer to the Nullability section below.
//
///'bdem' Null Values
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
// To illustrate, consider a 'bdem_Choice' with a 'BDEM_BOOL' selection.  The
// selection can be in one of three possible states:
//: o null with underlying value 'bdetu_Unset<bool>::unsetValue()'
//: o non-null with underlying value 'false'
//: o non-null with underlying value 'true'
//
// The underlying value of a null 'bdem' object is a class invariant.  If an
// object is null, that object *also* has the unset value corresponding to its
// type.
//
// For example, suppose we have a 'bdem_Choice', 'myChoice', whose current
// selection is of type 'BDEM_STRING':
//..
//  myChoice.theModifiableString() = "HELLO";
//  assert(myChoice.selection().isNonNull());
//..
// Making the selection null also makes it have the unset value (which, for
// 'BDEM_STRING', is the empty string):
//..
//  myChoice.selection().makeNull();
//  assert(myChoice.selection().isNull());
//  assert(myChoice.selection().theString().empty());
//..
///Choice Nullability
/// - - - - - - - - -
// Though a choice can be unset (selector of -1 and an empty types catalog) and
// its current selection can be null, the choice itself can be null only in
// relation to another object (its parent) in which it is a member.  The
// nullness of the current selection can be queried via a 'bdem_ElemRef'
// returned from 'selection' to determine whether its contained object (the
// current selection) is null, e.g., 'selection().isNull()'.
//
// The preferred way to assign a non-null value to a choice selection is
// through the 'theModifiableTYPE' methods, e.g., 'theModifiableBool' for a
// choice that is selected to be of type 'bool'.  The 'theModifiableTYPE'
// methods have the side-effect of making a null value non-null, so the 'const'
// 'theTYPE' methods should be used instead if the value is only being accessed
// (and not modified).
//
///Thread Safety
///-------------
// It is safe to access two 'bdem_Choice' objects simultaneously, each from a
// separate thread.  It is not safe to access a single 'bdem_Choice' object
// simultaneously from separate threads.
//
///Usage
///-----
// The 'divide' function reads two 'double' values from an input stream,
// divides the first by the second, and streams the result to an output stream.
//..
//  void divide(bdex_ByteInStream& is, bdex_ByteOutStream& os) {
//
//      // Read input parameters
//      double dividend, divisor;
//      bdex_InStreamFunctions::streamIn(is, dividend, 0);
//      bdex_InStreamFunctions::streamIn(is, divisor, 0);
//..
// The division normally will result in a double value, but will sometimes
// result in an error string.  The latter case can occur either because the
// input stream is corrupt or because the division itself failed because of the
// divisor was zero.  The result of the division is therefore packaged in a
// 'bdem_Choice' which can store either a 'double' or a 'string'.  The current
// selection is indexed by a value of 0 through 2 for the double result, string
// for input error, or string for division error.  Note that input errors and
// division errors are represented by two separate 'STRING' items in the array
// of types, so that the choice selector will indicate which of the two types
// of error strings is currently being held by the choice.
//..
//      static const bdem_ElemType::Type CHOICE_TYPES[] = {
//          bdem_ElemType::BDEM_DOUBLE,  // result value
//          bdem_ElemType::BDEM_STRING,  // input error
//          bdem_ElemType::BDEM_STRING   // division error
//      };
//      static const int NUM_CHOICES =
//          sizeof CHOICE_TYPES / sizeof CHOICE_TYPES[0];
//      enum { RESULT_VAL, INPUT_ERROR, DIVISION_ERROR };
//
//      bdem_Choice outMessage(CHOICE_TYPES, NUM_CHOICES);
//      assert(-1 == outMessage.selector());
//
//      if (! is) {
//..
// If the input fails, the choice is set to 'INPUT_ERROR' and the corresponding
// selection is set to an error string.
//..
//          bsl::string inputErrString = "Failed to read arguments."
//          outMessage.makeSelection(INPUT_ERROR).theModifiableString() =
//                                                              inputErrString;
//          assert(INPUT_ERROR == outMessage.selector());
//          assert(inputErrString == outMessage.theString());
//      }
//      else if (0 == divisor) {
//..
// If the division fails, the choice is set to 'DIVISION_ERROR' and the
// corresponding selection is set to an error string.  Note the alternate
// syntax for setting ('selection().theModifiableString()') and accessing
// ('selection().theString()') the current element:
//..
//          bsl::string errString = "Divided by zero.";
//          outMessage.makeSelection(DIVISION_ERROR);
//          outMessage.selection().theModifiableString() = errString;
//          assert(DIVISION_ERROR == outMessage.selector());
//          assert(errString == outMessage.selection().theString());
//      }
//      else {
//..
// If there are no errors, compute the quotient and store it as a 'double' with
// selector, 'RESULT_VAL'.
//..
//          const double quotient = dividend / divisor;
//          outMessage.makeSelection(RESULT_VAL).theModifiableDouble() =
//                                                                    quotient;
//          assert(RESULT_VAL == outMessage.selector());
//          assert(quotient == outMessage.theDouble());
//      }
//..
// Once the result is calculated, it is streamed out to be sent back to the
//..
//      bdex_OutStreamFunctions::streamOut(
//                                       os,
//                                       outMessage,
//                                       outMessage.maxSupportedBdexVersion());
//  }
//..

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BDEM_AGGREGATEOPTION
#include <bdem_aggregateoption.h>
#endif

#ifndef INCLUDED_BDEM_CHOICEIMP
#include <bdem_choiceimp.h>
#endif

#ifndef INCLUDED_BDEM_ELEMATTRLOOKUP
#include <bdem_elemattrlookup.h>
#endif

#ifndef INCLUDED_BDEM_ELEMREF
#include <bdem_elemref.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITS
#include <bslalg_typetraits.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORMUTIL
#include <bsls_platformutil.h>    // @DEPRECATED
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

class bslma_Allocator;

class bdet_Date;
class bdet_Datetime;
class bdet_DatetimeTz;
class bdet_DateTz;
class bdet_Time;
class bdet_TimeTz;

class bdem_ChoiceArray;
class bdem_ChoiceArrayItem;
class bdem_List;
class bdem_Table;

                        // =================
                        // class bdem_Choice
                        // =================

class bdem_Choice {
    // This class represents a container holding a single value (the "current
    // selection") of scalar, array, or aggregate type.  The 'bdem_Choice'
    // contains the current selection object, an array of type IDs (the "types
    // catalog"), and an index into the types catalog (the "selector"), which
    // specifies the type that the 'bdem_Choice' object is currently holding.
    // This class has methods for setting and appending to the types catalog,
    // for setting and retrieving the selector, and for accessing and modifying
    // the current selection.  When the selector is changed, the current
    // selection is destroyed and a new current selection value (non-null) is
    // constructed.  A selector of '-1' and an empty types catalog indicates
    // that there is no current selection (the "unset choice").  The current
    // selection's nullability can be queried via a 'bdem_ElemRef' returned
    // from 'selection'.  Please refer to the 'Nullability' section in the
    // component-level documentation for this component.
    //
    // Because putting a value into a 'bdem_Choice' makes a copy of that value,
    // assigning array or aggregate values may be prohibitively expensive in
    // terms of runtime.  Optimum performance may be achieved by building up
    // such (recursively) contained complex values in place.
    //
    // More generally, this container class supports a complete set of
    // *value-semantic* operations, including copy construction, assignment,
    // equality comparison, 'ostream' printing, and 'bdex'-compatible
    // serialization.  (A precise operational definition of when two objects
    // have the same value can be found in the description of 'operator==' for
    // the class.)  This container is *exception* *neutral* and, unless
    // otherwise stated, provides no guarantee of rollback: If an exception is
    // thrown during the invocation of a method on a pre-existing object, the
    // container is left in a valid state, but its value is undefined.  In no
    // event is memory leaked.  Finally, *aliasing* (e.g., using all or part of
    // an object as both source and destination) is supported in all cases.

    // DATA
    bdem_ChoiceImp d_choiceImp;  // choice implementation

    // FRIENDS
    friend bool operator==(const bdem_Choice&, const bdem_Choice&);

  public:
    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(bdem_Choice,
                                 bslalg_TypeTraitUsesBslmaAllocator);

    // CLASS METHODS
    static int maxSupportedBdexVersion();
        // Return the most current 'bdex' streaming version number supported by
        // this class.  (See the package-group-level documentation for more
        // information on 'bdex' streaming of container types.)

    // CREATORS
    explicit
    bdem_Choice(bslma_Allocator                          *basicAllocator = 0);
    explicit
    bdem_Choice(bdem_AggregateOption::AllocationStrategy  allocationStrategy,
                bslma_Allocator                          *basicAllocator = 0);
        // Create an unselected choice having an empty types catalog.
        // Optionally specify a memory 'allocationStrategy'.  If
        // 'allocationStrategy' is not specified, then 'BDEM_PASS_THROUGH' is
        // used.  (The meanings of the various 'allocationStrategy' values are
        // described in 'bdem_aggregateoption'.)  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.

    bdem_Choice(const bdem_ElemType::Type                 typesCatalog[],
                int                                       numTypes,
                bslma_Allocator                          *basicAllocator = 0);
    bdem_Choice(const bdem_ElemType::Type                 typesCatalog[],
                int                                       numTypes,
                bdem_AggregateOption::AllocationStrategy  allocationStrategy,
                bslma_Allocator                          *basicAllocator = 0);
        // Create an unselected choice having a types catalog of 'numTypes'
        // that are the same as those in the specified sequence 'typesCatalog'.
        // Optionally specify a memory 'allocationStrategy'.  If
        // 'allocationStrategy' is not specified, then 'BDEM_PASS_THROUGH' is
        // used.  (The meanings of the various 'allocationStrategy' values are
        // described in 'bdem_aggregateoption'.)  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.  The behavior is
        // undefined unless '0 <= numTypes' and 'typesCatalog' has at least
        // 'numTypes' types.

    bdem_Choice(const bsl::vector<bdem_ElemType::Type>&   typesCatalog,
                bslma_Allocator                          *basicAllocator = 0);
    bdem_Choice(const bsl::vector<bdem_ElemType::Type>&   typesCatalog,
                bdem_AggregateOption::AllocationStrategy  allocationStrategy,
                bslma_Allocator                          *basicAllocator = 0);
        // Create an unselected choice having a types catalog whose types are
        // the same as those in the specified 'typesCatalog'.  Optionally
        // specify a memory 'allocationStrategy'.  If 'allocationStrategy' is
        // not specified, then 'BDEM_PASS_THROUGH' is used.  (The meanings of
        // the various 'allocationStrategy' values are described in
        // 'bdem_aggregateoption'.)  Optionally specify a 'basicAllocator' used
        // to supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.

    bdem_Choice(const bdem_Choice&                       original,
                bslma_Allocator                          *basicAllocator = 0);
    bdem_Choice(const bdem_Choice&                        original,
                bdem_AggregateOption::AllocationStrategy  allocationStrategy,
                bslma_Allocator                          *basicAllocator = 0);
    explicit bdem_Choice(const bdem_ChoiceArrayItem&      original,
                         bslma_Allocator                 *basicAllocator = 0);
    bdem_Choice(const bdem_ChoiceArrayItem&               original,
                bdem_AggregateOption::AllocationStrategy  allocationStrategy,
                bslma_Allocator                          *basicAllocator = 0);
        // Create a choice having the value of the specified 'original' object
        // (choice or choice array item).  Optionally specify a memory
        // 'allocationStrategy'.  If 'allocationStrategy' is not specified,
        // then 'BDEM_PASS_THROUGH' is used.  (The meanings of the various
        // 'allocationStrategy' values are described in
        // 'bdem_aggregateoption'.)  Optionally specify a 'basicAllocator' used
        // to supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.

    ~bdem_Choice();
        // Destroy this choice object.  If the choice was constructed with any
        // memory allocation strategy other than 'BDEM_PASS_THROUGH', then
        // destructors are not called on the contained selection.  The memory
        // used by those selections will be released efficiently (all at once)
        // when the internal allocator is destroyed.

    // MANIPULATORS
    bdem_Choice& operator=(const bdem_ChoiceArrayItem& rhs);
    bdem_Choice& operator=(const bdem_Choice& rhs);
        // Assign to this choice the types catalog, selector, and value
        // contained in the specified 'rhs' choice or choice array item, and
        // return a reference to this modifiable choice.

    bdem_ChoiceArrayItem& item();
        // Return a reference to a modifiable choice array item proxy for this
        // choice.

    bdem_ElemRef makeSelection(int index);
        // If the specified 'index' is '-1', destroy the current selection of
        // this choice (if any), and set this choice to the unselected state.
        // Otherwise, if 'index == selector()', reset the current selection to
        // null.  Otherwise ('-1 != index' and 'index != selector()'), destroy
        // the current selection (if any), and set the selection of this choice
        // to the null value corresponding to the entry in the types catalog
        // given by 'selectionType(index)'.  Return an element reference to the
        // modifiable current selection if '-1 != index', and a void element
        // reference otherwise.  The behavior is undefined unless
        // '-1 <= index < numSelections()'.  Note that after calling this
        // method, 'selector' will return 'index'.

    bdem_ElemRef selection();
        // Return an element reference to the modifiable current selection of
        // this choice if '-1 != selector()', and a void element reference
        // otherwise.

    bool& theModifiableBool();
    char& theModifiableChar();
    short& theModifiableShort();
    int& theModifiableInt();
    bsls_Types::Int64& theModifiableInt64();
    float& theModifiableFloat();
    double& theModifiableDouble();
    bsl::string& theModifiableString();
    bdet_Datetime& theModifiableDatetime();
    bdet_DatetimeTz& theModifiableDatetimeTz();
    bdet_Date& theModifiableDate();
    bdet_DateTz& theModifiableDateTz();
    bdet_Time& theModifiableTime();
    bdet_TimeTz& theModifiableTimeTz();
    bsl::vector<bool>& theModifiableBoolArray();
    bsl::vector<char>& theModifiableCharArray();
    bsl::vector<short>& theModifiableShortArray();
    bsl::vector<int>& theModifiableIntArray();
    bsl::vector<bsls_Types::Int64>& theModifiableInt64Array();
    bsl::vector<float>& theModifiableFloatArray();
    bsl::vector<double>& theModifiableDoubleArray();
    bsl::vector<bsl::string>& theModifiableStringArray();
    bsl::vector<bdet_Datetime>& theModifiableDatetimeArray();
    bsl::vector<bdet_DatetimeTz>& theModifiableDatetimeTzArray();
    bsl::vector<bdet_Date>& theModifiableDateArray();
    bsl::vector<bdet_DateTz>& theModifiableDateTzArray();
    bsl::vector<bdet_Time>& theModifiableTimeArray();
    bsl::vector<bdet_TimeTz>& theModifiableTimeTzArray();
    bdem_Choice& theModifiableChoice();
    bdem_ChoiceArray& theModifiableChoiceArray();
    bdem_List& theModifiableList();
    bdem_Table& theModifiableTable();
        // Return a reference to the modifiable selection of this choice.  If
        // the selection is null, it is made non-null before returning, but its
        // value is not otherwise modified.  The behavior is undefined unless
        // the type of the current selection is of the type indicated by the
        // name of the method used.  Note that unless there is an intention of
        // modifying the selection, the corresponding 'theTYPE' method should
        // be used instead.  Also note that for a given choice 'C' and a given
        // 'bdem' 'TYPE', the expression 'C.theModifiableTYPE()' has the same
        // side-effect and returns a reference to the same element as
        // 'C.selection().theModifiableTYPE()'.

    void addSelection(bdem_ElemType::Type elemType);
        // Append the specified 'elemType' to the types catalog of this choice.
        // Note that 'elemType' need not be unique in the types catalog.

    void reset();
        // Destroy the current selection of this choice (if any), reset the
        // choice to the unselected state, and reset the types catalog to be
        // empty.

    void reset(const bdem_ElemType::Type typesCatalog[],
               int                       numTypes);
        // Destroy the current selection of this choice (if any), reset the
        // choice to the unselected state, and reset the types catalog to have
        // the specified initial 'numTypes' types in the specified
        // 'typesCatalog' array.  The behavior is undefined unless
        // '0 <= numTypes' and 'typesCatalog' has at least 'numTypes' types.

    void reset(const bsl::vector<bdem_ElemType::Type>& typesCatalog);
        // Destroy the current selection of this choice (if any), reset the
        // choice to the unselected state, and reset the types catalog to
        // contain the types in the specified 'typesCatalog'.

    template <class STREAM>
    STREAM& bdexStreamIn(STREAM& stream, int version);
        // Assign to this object the value read from the specified input
        // 'stream' using the specified 'version' format, and return a
        // reference to the modifiable 'stream'.  If 'stream' is initially
        // invalid, this operation has no effect.  If 'stream' becomes invalid
        // during this operation, this object is valid, but its value is not
        // specified.  If 'version' is not supported, 'stream' is marked
        // invalid and this object is unaltered.  Note that no version is read
        // from 'stream'.  See the 'bdex' package-level documentation for more
        // information on 'bdex' streaming of value-semantic types and
        // containers.

    // ACCESSORS
    int numSelections() const;
        // Return the number of selections available for this choice.  Note
        // that the number of selections is the same as the length of the types
        // catalog of this choice.

    int selector() const;
        // Return the current selector of this choice (i.e., the index of the
        // currently selected type within the types catalog, or -1 if this
        // choice is unselected).

    const bdem_ChoiceArrayItem& item() const;
        // Return a reference to a non-modifiable choice array item proxy for
        // this choice.

    bdem_ElemType::Type selectionType() const;
        // Return the type of the current selection of this choice if it is
        // currently selected, and 'bdem_ElemType::BDEM_VOID' otherwise.

    bdem_ElemType::Type selectionType(int index) const;
        // Return the type at the specified 'index' in the types catalog of
        // this choice if '-1 != index', and 'bdem_ElemType::BDEM_VOID'
        // otherwise.  The behavior is undefined unless
        // '-1 <= index < numSelections()'.

    void selectionTypes(bsl::vector<bdem_ElemType::Type> *result) const;
        // Replace the contents of the specified 'result' with the sequence of
        // types in the types catalog of this choice.

    bdem_ConstElemRef selection() const;
        // Return an element reference to the non-modifiable current selection
        // of this choice if '-1 != selector()', and a void element reference
        // otherwise.

    const bool& theBool() const;
    const char& theChar() const;
    const short& theShort() const;
    const int& theInt() const;
    const bsls_Types::Int64& theInt64() const;
    const float& theFloat() const;
    const double& theDouble() const;
    const bsl::string& theString() const;
    const bdet_Datetime& theDatetime() const;
    const bdet_DatetimeTz& theDatetimeTz() const;
    const bdet_Date& theDate() const;
    const bdet_DateTz& theDateTz() const;
    const bdet_Time& theTime() const;
    const bdet_TimeTz& theTimeTz() const;
    const bsl::vector<bool>& theBoolArray() const;
    const bsl::vector<char>& theCharArray() const;
    const bsl::vector<short>& theShortArray() const;
    const bsl::vector<int>& theIntArray() const;
    const bsl::vector<bsls_Types::Int64>& theInt64Array() const;
    const bsl::vector<float>& theFloatArray() const;
    const bsl::vector<double>& theDoubleArray() const;
    const bsl::vector<bsl::string>& theStringArray() const;
    const bsl::vector<bdet_Datetime>& theDatetimeArray() const;
    const bsl::vector<bdet_DatetimeTz>& theDatetimeTzArray() const;
    const bsl::vector<bdet_Date>& theDateArray() const;
    const bsl::vector<bdet_DateTz>& theDateTzArray() const;
    const bsl::vector<bdet_Time>& theTimeArray() const;
    const bsl::vector<bdet_TimeTz>& theTimeTzArray() const;
    const bdem_Choice& theChoice() const;
    const bdem_ChoiceArray& theChoiceArray() const;
    const bdem_List& theList() const;
    const bdem_Table& theTable() const;
        // Return a reference to the non-modifiable selection of this choice.
        // The nullness of the selection is not affected.  The behavior is
        // undefined unless the type of the current selection is of the type
        // indicated by the name of the method used.  Note that for a given
        // choice 'C' and a given 'bdem' 'TYPE', the expression 'C.theTYPE()'
        // returns a reference to the same element as
        // 'C.selection().theTYPE()'.

    template <class STREAM>
    STREAM& bdexStreamOut(STREAM& stream, int version) const;
        // Write this value to the specified output 'stream' using the
        // specified 'version' format and return a reference to the modifiable
        // 'stream'.  If 'version' is not supported, 'stream' is unmodified.
        // Note that 'version' is not written to 'stream'.  See the 'bdex'
        // package-level documentation for more information on 'bdex' streaming
        // of value-semantic types and containers.

    bsl::ostream& print(bsl::ostream& stream,
                        int           level = 0,
                        int           spacesPerLevel = 4) const;
        // Format this object to the specified output 'stream' at the absolute
        // value of the optionally specified indentation 'level' and return a
        // reference to 'stream'.  If 'level' is specified, optionally specify
        // 'spacesPerLevel', the number of spaces per indentation level for
        // this and all of its nested objects.  If 'level' is negative,
        // suppress indentation of the first line.  If 'spacesPerLevel' is
        // negative, format the entire output on one line, suppressing all but
        // the initial indentation (as governed by 'level').  If 'stream' is
        // not valid on entry, this operation has no effect.
};

// FREE OPERATORS
inline
bool operator==(const bdem_Choice& lhs, const bdem_Choice& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' choice objects have the
    // same value, and 'false' otherwise.  Two choices have the same value if
    // they have the same types catalog, the same selector, and the same
    // selection value.

inline
bool operator!=(const bdem_Choice& lhs, const bdem_Choice& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' choice objects do not
    // have the same value, and 'false' otherwise.  Two choices do not have the
    // same value if they have different types catalogs, different selectors,
    // or different current selection values.

inline
bsl::ostream& operator<<(bsl::ostream& stream, const bdem_Choice& rhs);
    // Format the value of the specified 'rhs', unindented on a single line, to
    // the specified output 'stream', and return a reference to 'stream'.  If
    // stream is not valid on entry, this operation has no effect.  Note that
    // this human-readable format is the same as what would result from
    // invoking:
    //..
    //  rhs.print(stream, 0, -1);
    //..
    // which is not fully specified, and subject to change without notice.

// ===========================================================================
//                      INLINE FUNCTION DEFINITIONS
// ===========================================================================

                        // -----------------
                        // class bdem_Choice
                        // -----------------

// CLASS METHODS
inline
int bdem_Choice::maxSupportedBdexVersion()
{
    return 3;
}

// CREATORS
inline
bdem_Choice::bdem_Choice(bslma_Allocator *basicAllocator)
: d_choiceImp(basicAllocator)
{
}

inline
bdem_Choice::bdem_Choice(
                  bdem_AggregateOption::AllocationStrategy  allocationStrategy,
                  bslma_Allocator                          *basicAllocator)
: d_choiceImp(allocationStrategy, basicAllocator)
{
}

inline
bdem_Choice::bdem_Choice(const bdem_ElemType::Type  typesCatalog[],
                         int                        numTypes,
                         bslma_Allocator           *basicAllocator)
: d_choiceImp(typesCatalog,
              numTypes,
              bdem_ElemAttrLookup::lookupTable(),
              bdem_AggregateOption::BDEM_PASS_THROUGH,
              basicAllocator)
{
    BSLS_ASSERT_SAFE(0 <= numTypes);
}

inline
bdem_Choice::bdem_Choice(
                  const bdem_ElemType::Type                 typesCatalog[],
                  int                                       numTypes,
                  bdem_AggregateOption::AllocationStrategy  allocationStrategy,
                  bslma_Allocator                          *basicAllocator)
: d_choiceImp(typesCatalog,
              numTypes,
              bdem_ElemAttrLookup::lookupTable(),
              allocationStrategy,
              basicAllocator)
{
    BSLS_ASSERT_SAFE(0 <= numTypes);
}

inline
bdem_Choice::bdem_Choice(
                      const bsl::vector<bdem_ElemType::Type>&  typesCatalog,
                      bslma_Allocator                         *basicAllocator)
: d_choiceImp(0 == typesCatalog.size() ? 0 : &typesCatalog[0],
              (int)typesCatalog.size(),
              bdem_ElemAttrLookup::lookupTable(),
              bdem_AggregateOption::BDEM_PASS_THROUGH,
              basicAllocator)
{
}

inline
bdem_Choice::bdem_Choice(
                  const bsl::vector<bdem_ElemType::Type>&   typesCatalog,
                  bdem_AggregateOption::AllocationStrategy  allocationStrategy,
                  bslma_Allocator                          *basicAllocator)
: d_choiceImp(0 == typesCatalog.size() ? 0 : &typesCatalog[0],
              (int)typesCatalog.size(),
              bdem_ElemAttrLookup::lookupTable(),
              allocationStrategy,
              basicAllocator)
{
}

inline
bdem_Choice::bdem_Choice(const bdem_Choice&  original,
                         bslma_Allocator    *basicAllocator)
: d_choiceImp(original.d_choiceImp, basicAllocator)
{
}

inline
bdem_Choice::bdem_Choice(
                  const bdem_Choice&                        original,
                  bdem_AggregateOption::AllocationStrategy  allocationStrategy,
                  bslma_Allocator                          *basicAllocator)
: d_choiceImp(original.d_choiceImp, allocationStrategy, basicAllocator)
{
}

inline
bdem_Choice::bdem_Choice(const bdem_ChoiceArrayItem&  original,
                         bslma_Allocator             *basicAllocator)
: d_choiceImp((const bdem_ChoiceHeader &) original,
              bdem_AggregateOption::BDEM_PASS_THROUGH,
              basicAllocator)
{
}

inline
bdem_Choice::bdem_Choice(
                  const bdem_ChoiceArrayItem&               original,
                  bdem_AggregateOption::AllocationStrategy  allocationStrategy,
                  bslma_Allocator                          *basicAllocator)
: d_choiceImp((const bdem_ChoiceHeader &) original,
              allocationStrategy,
              basicAllocator)
{
}

inline
bdem_Choice::~bdem_Choice()
{
}

// MANIPULATORS
inline
bdem_Choice& bdem_Choice::operator=(const bdem_Choice& rhs)
{
    if (this != &rhs) {
        d_choiceImp = rhs.d_choiceImp;
    }
    return *this;
}

inline
bdem_ChoiceArrayItem& bdem_Choice::item()
{
    return (bdem_ChoiceArrayItem &)d_choiceImp.choiceHeader();
}

inline
bdem_ElemRef bdem_Choice::makeSelection(int index)
{
    BSLS_ASSERT_SAFE(-1 <= index);
    BSLS_ASSERT_SAFE(      index < d_choiceImp.numSelections());

    return d_choiceImp.makeSelection(index);
}

inline
bdem_ElemRef bdem_Choice::selection()
{
    return d_choiceImp.selection();
}

inline
bool& bdem_Choice::theModifiableBool()
{
    d_choiceImp.clearNullnessBit();
    return *(bool *)d_choiceImp.selectionPointer();
}

inline
char& bdem_Choice::theModifiableChar()
{
    d_choiceImp.clearNullnessBit();
    return *(char *)d_choiceImp.selectionPointer();
}

inline
short& bdem_Choice::theModifiableShort()
{
    d_choiceImp.clearNullnessBit();
    return *(short *)d_choiceImp.selectionPointer();
}

inline
int& bdem_Choice::theModifiableInt()
{
    d_choiceImp.clearNullnessBit();
    return *(int *)d_choiceImp.selectionPointer();
}

inline
bsls_Types::Int64& bdem_Choice::theModifiableInt64()
{
    d_choiceImp.clearNullnessBit();
    return *(bsls_Types::Int64 *)d_choiceImp.selectionPointer();
}

inline
float& bdem_Choice::theModifiableFloat()
{
    d_choiceImp.clearNullnessBit();
    return *(float *)d_choiceImp.selectionPointer();
}

inline
double& bdem_Choice::theModifiableDouble()
{
    d_choiceImp.clearNullnessBit();
    return *(double *)d_choiceImp.selectionPointer();
}

inline
bsl::string& bdem_Choice::theModifiableString()
{
    d_choiceImp.clearNullnessBit();
    return *(bsl::string *)d_choiceImp.selectionPointer();
}

inline
bdet_Datetime& bdem_Choice::theModifiableDatetime()
{
    d_choiceImp.clearNullnessBit();
    return *(bdet_Datetime *)d_choiceImp.selectionPointer();
}

inline
bdet_DatetimeTz& bdem_Choice::theModifiableDatetimeTz()
{
    d_choiceImp.clearNullnessBit();
    return *(bdet_DatetimeTz *)d_choiceImp.selectionPointer();
}

inline
bdet_Date& bdem_Choice::theModifiableDate()
{
    d_choiceImp.clearNullnessBit();
    return *(bdet_Date *)d_choiceImp.selectionPointer();
}

inline
bdet_DateTz& bdem_Choice::theModifiableDateTz()
{
    d_choiceImp.clearNullnessBit();
    return *(bdet_DateTz *)d_choiceImp.selectionPointer();
}

inline
bdet_Time& bdem_Choice::theModifiableTime()
{
    d_choiceImp.clearNullnessBit();
    return *(bdet_Time *)d_choiceImp.selectionPointer();
}

inline
bdet_TimeTz& bdem_Choice::theModifiableTimeTz()
{
    d_choiceImp.clearNullnessBit();
    return *(bdet_TimeTz *)d_choiceImp.selectionPointer();
}

inline
bsl::vector<bool>& bdem_Choice::theModifiableBoolArray()
{
    d_choiceImp.clearNullnessBit();
    return *(bsl::vector<bool> *)d_choiceImp.selectionPointer();
}

inline
bsl::vector<char>& bdem_Choice::theModifiableCharArray()
{
    d_choiceImp.clearNullnessBit();
    return *(bsl::vector<char> *)d_choiceImp.selectionPointer();
}

inline
bsl::vector<short>& bdem_Choice::theModifiableShortArray()
{
    d_choiceImp.clearNullnessBit();
    return *(bsl::vector<short> *)d_choiceImp.selectionPointer();
}

inline
bsl::vector<int>& bdem_Choice::theModifiableIntArray()
{
    d_choiceImp.clearNullnessBit();
    return *(bsl::vector<int> *)d_choiceImp.selectionPointer();
}

inline
bsl::vector<bsls_Types::Int64>& bdem_Choice::theModifiableInt64Array()
{
    d_choiceImp.clearNullnessBit();
    return *(bsl::vector<bsls_Types::Int64> *)d_choiceImp.selectionPointer();
}

inline
bsl::vector<float>& bdem_Choice::theModifiableFloatArray()
{
    d_choiceImp.clearNullnessBit();
    return *(bsl::vector<float> *)d_choiceImp.selectionPointer();
}

inline
bsl::vector<double>& bdem_Choice::theModifiableDoubleArray()
{
    d_choiceImp.clearNullnessBit();
    return *(bsl::vector<double> *)d_choiceImp.selectionPointer();
}

inline
bsl::vector<bsl::string>& bdem_Choice::theModifiableStringArray()
{
    d_choiceImp.clearNullnessBit();
    return *(bsl::vector<bsl::string> *)d_choiceImp.selectionPointer();
}

inline
bsl::vector<bdet_Datetime>& bdem_Choice::theModifiableDatetimeArray()
{
    d_choiceImp.clearNullnessBit();
    return *(bsl::vector<bdet_Datetime> *)d_choiceImp.selectionPointer();
}

inline
bsl::vector<bdet_DatetimeTz>& bdem_Choice::theModifiableDatetimeTzArray()
{
    d_choiceImp.clearNullnessBit();
    return *(bsl::vector<bdet_DatetimeTz> *)d_choiceImp.selectionPointer();
}

inline
bsl::vector<bdet_Date>& bdem_Choice::theModifiableDateArray()
{
    d_choiceImp.clearNullnessBit();
    return *(bsl::vector<bdet_Date> *)d_choiceImp.selectionPointer();
}

inline
bsl::vector<bdet_DateTz>& bdem_Choice::theModifiableDateTzArray()
{
    d_choiceImp.clearNullnessBit();
    return *(bsl::vector<bdet_DateTz> *)d_choiceImp.selectionPointer();
}

inline
bsl::vector<bdet_Time>& bdem_Choice::theModifiableTimeArray()
{
    d_choiceImp.clearNullnessBit();
    return *(bsl::vector<bdet_Time> *)d_choiceImp.selectionPointer();
}

inline
bsl::vector<bdet_TimeTz>& bdem_Choice::theModifiableTimeTzArray()
{
    d_choiceImp.clearNullnessBit();
    return *(bsl::vector<bdet_TimeTz> *)d_choiceImp.selectionPointer();
}

inline
bdem_List& bdem_Choice::theModifiableList()
{
    d_choiceImp.clearNullnessBit();
    return *(bdem_List *)d_choiceImp.selectionPointer();
}

inline
bdem_Table& bdem_Choice::theModifiableTable()
{
    d_choiceImp.clearNullnessBit();
    return *(bdem_Table *)d_choiceImp.selectionPointer();
}

inline
bdem_Choice& bdem_Choice::theModifiableChoice()
{
    d_choiceImp.clearNullnessBit();
    return *(bdem_Choice *)d_choiceImp.selectionPointer();
}

inline
bdem_ChoiceArray& bdem_Choice::theModifiableChoiceArray()
{
    d_choiceImp.clearNullnessBit();
    return *(bdem_ChoiceArray *)d_choiceImp.selectionPointer();
}

inline
void bdem_Choice::addSelection(bdem_ElemType::Type elemType)
{
    d_choiceImp.addSelection(bdem_ElemAttrLookup::lookupTable()
                                                [static_cast<int>(elemType)]);
}

inline
void bdem_Choice::reset()
{
    d_choiceImp.reset();
}

inline
void bdem_Choice::reset(const bdem_ElemType::Type typesCatalog[], int numTypes)
{
    BSLS_ASSERT_SAFE(0 <= numTypes);

    d_choiceImp.reset(typesCatalog,
                      numTypes,
                      bdem_ElemAttrLookup::lookupTable());
}

inline
void bdem_Choice::reset(const bsl::vector<bdem_ElemType::Type>& typesCatalog)
{
    const bdem_ElemType::Type *begin = 0 == typesCatalog.size()
                                     ? 0
                                     : &typesCatalog[0];
    return reset(begin, (int)typesCatalog.size());
}

template <class STREAM>
inline
STREAM& bdem_Choice::bdexStreamIn(STREAM& stream, int version)
{
    return d_choiceImp.bdexStreamInImp(
                            stream,
                            version,
                            bdem_ElemStreamInAttrLookup<STREAM>::lookupTable(),
                            bdem_ElemAttrLookup::lookupTable());
}

// ACCESSORS
inline
int bdem_Choice::numSelections() const
{
    return d_choiceImp.numSelections();
}

inline
int bdem_Choice::selector() const
{
    return d_choiceImp.selector();
}

inline
const bdem_ChoiceArrayItem& bdem_Choice::item() const
{
    return (const bdem_ChoiceArrayItem &) d_choiceImp.choiceHeader();
}

inline
bdem_ElemType::Type bdem_Choice::selectionType() const
{
    return selectionType(selector());
}

inline
bdem_ElemType::Type bdem_Choice::selectionType(int index) const
{
    BSLS_ASSERT_SAFE(-1 <= index);
    BSLS_ASSERT_SAFE(      index < numSelections());

    return d_choiceImp.selectionType(index);
}

inline
bdem_ConstElemRef bdem_Choice::selection() const
{
    return d_choiceImp.selection();
}

inline
const bool& bdem_Choice::theBool() const
{
    return *(const bool *)d_choiceImp.selectionPointer();
}

inline
const char& bdem_Choice::theChar() const
{
    return *(const char *)d_choiceImp.selectionPointer();
}

inline
const short& bdem_Choice::theShort() const
{
    return *(const short *)d_choiceImp.selectionPointer();
}

inline
const int& bdem_Choice::theInt() const
{
    return *(const int *)d_choiceImp.selectionPointer();
}

inline
const bsls_Types::Int64& bdem_Choice::theInt64() const
{
    return *(const bsls_Types::Int64 *)d_choiceImp.selectionPointer();
}

inline
const float& bdem_Choice::theFloat() const
{
    return *(const float *)d_choiceImp.selectionPointer();
}

inline
const double& bdem_Choice::theDouble() const
{
    return *(const double *)d_choiceImp.selectionPointer();
}

inline
const bsl::string& bdem_Choice::theString() const
{
    return *(const bsl::string *)d_choiceImp.selectionPointer();
}

inline
const bdet_Datetime& bdem_Choice::theDatetime() const
{
    return *(const bdet_Datetime *)d_choiceImp.selectionPointer();
}

inline
const bdet_DatetimeTz& bdem_Choice::theDatetimeTz() const
{
    return *(const bdet_DatetimeTz *)d_choiceImp.selectionPointer();
}

inline
const bdet_Date& bdem_Choice::theDate() const
{
    return *(const bdet_Date *)d_choiceImp.selectionPointer();
}

inline
const bdet_DateTz& bdem_Choice::theDateTz() const
{
    return *(const bdet_DateTz *)d_choiceImp.selectionPointer();
}

inline
const bdet_Time& bdem_Choice::theTime() const
{
    return *(const bdet_Time *)d_choiceImp.selectionPointer();
}

inline
const bdet_TimeTz& bdem_Choice::theTimeTz() const
{
    return *(const bdet_TimeTz *)d_choiceImp.selectionPointer();
}

inline
const bsl::vector<bool>& bdem_Choice::theBoolArray() const
{
    return *(const bsl::vector<bool> *)d_choiceImp.selectionPointer();
}

inline
const bsl::vector<char>& bdem_Choice::theCharArray() const
{
    return *(const bsl::vector<char> *)d_choiceImp.selectionPointer();
}

inline
const bsl::vector<short>& bdem_Choice::theShortArray() const
{
    return *(const bsl::vector<short> *)d_choiceImp.selectionPointer();
}

inline
const bsl::vector<int>& bdem_Choice::theIntArray() const
{
    return *(const bsl::vector<int> *)d_choiceImp.selectionPointer();
}

inline
const bsl::vector<bsls_Types::Int64>&
bdem_Choice::theInt64Array() const
{
    return *(const bsl::vector<bsls_Types::Int64> *)
                                                d_choiceImp.selectionPointer();
}

inline
const bsl::vector<float>& bdem_Choice::theFloatArray() const
{
    return *(const bsl::vector<float> *)d_choiceImp.selectionPointer();
}

inline
const bsl::vector<double>& bdem_Choice::theDoubleArray() const
{
    return *(const bsl::vector<double> *)d_choiceImp.selectionPointer();
}

inline
const bsl::vector<bsl::string>& bdem_Choice::theStringArray() const
{
    return *(const bsl::vector<bsl::string> *)d_choiceImp.selectionPointer();
}

inline
const bsl::vector<bdet_Datetime>& bdem_Choice::theDatetimeArray() const
{
    return *(const bsl::vector<bdet_Datetime> *)
                                               d_choiceImp.selectionPointer();
}

inline
const bsl::vector<bdet_DatetimeTz>& bdem_Choice::theDatetimeTzArray() const
{
    return *(const bsl::vector<bdet_DatetimeTz> *)
                                               d_choiceImp.selectionPointer();
}

inline
const bsl::vector<bdet_Date>& bdem_Choice::theDateArray() const
{
    return *(const bsl::vector<bdet_Date> *)d_choiceImp.selectionPointer();
}

inline
const bsl::vector<bdet_DateTz>& bdem_Choice::theDateTzArray() const
{
    return *(const bsl::vector<bdet_DateTz> *)d_choiceImp.selectionPointer();
}

inline
const bsl::vector<bdet_Time>& bdem_Choice::theTimeArray() const
{
    return *(const bsl::vector<bdet_Time> *)d_choiceImp.selectionPointer();
}

inline
const bsl::vector<bdet_TimeTz>& bdem_Choice::theTimeTzArray() const
{
    return *(const bsl::vector<bdet_TimeTz> *)d_choiceImp.selectionPointer();
}

inline
const bdem_List& bdem_Choice::theList() const
{
    return *(const bdem_List *)d_choiceImp.selectionPointer();
}

inline
const bdem_Table& bdem_Choice::theTable() const
{
    return *(const bdem_Table *)d_choiceImp.selectionPointer();
}

inline
const bdem_Choice& bdem_Choice::theChoice() const
{
    return *(const bdem_Choice *)d_choiceImp.selectionPointer();
}

inline
const bdem_ChoiceArray& bdem_Choice::theChoiceArray() const
{
    return *(const bdem_ChoiceArray *)d_choiceImp.selectionPointer();
}

template <class STREAM>
inline
STREAM& bdem_Choice::bdexStreamOut(STREAM& stream, int version) const
{
    return d_choiceImp.bdexStreamOutImp(
                          stream,
                          version,
                          bdem_ElemStreamOutAttrLookup<STREAM>::lookupTable());
}

// FREE OPERATORS
inline
bool operator==(const bdem_Choice& lhs, const bdem_Choice& rhs)
{
    return lhs.d_choiceImp == rhs.d_choiceImp;
}

inline
bool operator!=(const bdem_Choice& lhs, const bdem_Choice& rhs)
{
    return !(lhs == rhs);
}

inline
bsl::ostream& operator<<(bsl::ostream& stream, const bdem_Choice& rhs)
{
    return rhs.print(stream, 0, -1);
}

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2006
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
