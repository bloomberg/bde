// bdlmxxx_choice.h                                                      -*-C++-*-
#ifndef INCLUDED_BDLMXXX_CHOICE
#define INCLUDED_BDLMXXX_CHOICE

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a container holding one value from a set of possible types.
//
//@CLASSES:
//  bdlmxxx::Choice: container holding one value from a set of possible types
//
//@SEE_ALSO: bdlmxxx_row, bdlmxxx_list, bdlmxxx_table, bdlmxxx_elemref, bdlmxxx_choicearray,
//           bdlmxxx_choicearrayitem
//
//@AUTHOR: Pablo Halpern (phalpern), Anthony Comerico (acomeric)
//
//@DESCRIPTION: This component provides a value-semantic container class,
// 'bdlmxxx::Choice', capable of holding at most one value (the "current
// selection") of any 'bdem' type.  A 'bdlmxxx::Choice' contains the current
// selection object, an array of type IDs (the "types catalog"), and an index
// into the types catalog (the "selector") that specifies the type that the
// 'bdlmxxx::Choice' object is currently holding.  The interface closely follows
// that for 'bdlmxxx::List' with the major difference being that it holds only one
// value at a time.  Value-semantic operations such as copy construction,
// assignment, streaming, and equality operations are also provided.
//
// This class has methods for setting and appending to the types catalog, for
// setting and retrieving the selector, and for accessing and modifying the
// current selection.  When the selector is changed, the current selection is
// destroyed and a new current selection value is constructed.  A selector of
// '-1' indicates that there is no current selection.  Note that a
// 'bdlmxxx::Choice' manages the nullness of its current selection.  The current
// selection's nullability can be queried via a 'bdlmxxx::ElemRef' returned from
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
//  scalar (BDEM_INT, etc.)           bdltuxxx::Unset<TYPE>::unsetValue()
//
//  vector (BDEM_INT_ARRAY, etc.)     0 == size()
//..
// To illustrate, consider a 'bdlmxxx::Choice' with a 'BDEM_BOOL' selection.  The
// selection can be in one of three possible states:
//: o null with underlying value 'bdltuxxx::Unset<bool>::unsetValue()'
//: o non-null with underlying value 'false'
//: o non-null with underlying value 'true'
//
// The underlying value of a null 'bdem' object is a class invariant.  If an
// object is null, that object *also* has the unset value corresponding to its
// type.
//
// For example, suppose we have a 'bdlmxxx::Choice', 'myChoice', whose current
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
// nullness of the current selection can be queried via a 'bdlmxxx::ElemRef'
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
// It is safe to access two 'bdlmxxx::Choice' objects simultaneously, each from a
// separate thread.  It is not safe to access a single 'bdlmxxx::Choice' object
// simultaneously from separate threads.
//
///Usage
///-----
// The 'divide' function reads two 'double' values from an input stream,
// divides the first by the second, and streams the result to an output stream.
//..
//  void divide(bdlxxxx::ByteInStream& is, bdlxxxx::ByteOutStream& os) {
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
// 'bdlmxxx::Choice' which can store either a 'double' or a 'string'.  The current
// selection is indexed by a value of 0 through 2 for the double result, string
// for input error, or string for division error.  Note that input errors and
// division errors are represented by two separate 'STRING' items in the array
// of types, so that the choice selector will indicate which of the two types
// of error strings is currently being held by the choice.
//..
//      static const bdlmxxx::ElemType::Type CHOICE_TYPES[] = {
//          bdlmxxx::ElemType::BDEM_DOUBLE,  // result value
//          bdlmxxx::ElemType::BDEM_STRING,  // input error
//          bdlmxxx::ElemType::BDEM_STRING   // division error
//      };
//      static const int NUM_CHOICES =
//          sizeof CHOICE_TYPES / sizeof CHOICE_TYPES[0];
//      enum { RESULT_VAL, INPUT_ERROR, DIVISION_ERROR };
//
//      bdlmxxx::Choice outMessage(CHOICE_TYPES, NUM_CHOICES);
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

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

#ifndef INCLUDED_BDLMXXX_AGGREGATEOPTION
#include <bdlmxxx_aggregateoption.h>
#endif

#ifndef INCLUDED_BDLMXXX_CHOICEIMP
#include <bdlmxxx_choiceimp.h>
#endif

#ifndef INCLUDED_BDLMXXX_ELEMATTRLOOKUP
#include <bdlmxxx_elemattrlookup.h>
#endif

#ifndef INCLUDED_BDLMXXX_ELEMREF
#include <bdlmxxx_elemref.h>
#endif

#ifndef INCLUDED_BSLMA_ALLOCATOR
#include <bslma_allocator.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITS
#include <bslalg_typetraits.h>
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



// Updated by 'bde-replace-bdet-forward-declares.py -m bdlt': 2015-02-03
// Updated declarations tagged with '// bdet -> bdlt'.

namespace bdlt { class Date; }                                  // bdet -> bdlt

namespace bdet {typedef ::BloombergLP::bdlt::Date Date;                    // bdet -> bdlt
}  // close package namespace

namespace bdlt { class Datetime; }                              // bdet -> bdlt

namespace bdet {typedef ::BloombergLP::bdlt::Datetime Datetime;            // bdet -> bdlt
}  // close package namespace

namespace bdlt { class DatetimeTz; }                            // bdet -> bdlt

namespace bdet {typedef ::BloombergLP::bdlt::DatetimeTz DatetimeTz;        // bdet -> bdlt
}  // close package namespace

namespace bdlt { class DateTz; }                                // bdet -> bdlt

namespace bdet {typedef ::BloombergLP::bdlt::DateTz DateTz;                // bdet -> bdlt
}  // close package namespace

namespace bdlt { class Time; }                                  // bdet -> bdlt

namespace bdet {typedef ::BloombergLP::bdlt::Time Time;                    // bdet -> bdlt
}  // close package namespace

namespace bdlt { class TimeTz; }                                // bdet -> bdlt

namespace bdet {typedef ::BloombergLP::bdlt::TimeTz TimeTz;                // bdet -> bdlt
}  // close package namespace


namespace bdlmxxx {class ChoiceArray;
class ChoiceArrayItem;
class List;
class Table;

                        // =================
                        // class Choice
                        // =================

class Choice {
    // This class represents a container holding a single value (the "current
    // selection") of scalar, array, or aggregate type.  The 'Choice'
    // contains the current selection object, an array of type IDs (the "types
    // catalog"), and an index into the types catalog (the "selector"), which
    // specifies the type that the 'Choice' object is currently holding.
    // This class has methods for setting and appending to the types catalog,
    // for setting and retrieving the selector, and for accessing and modifying
    // the current selection.  When the selector is changed, the current
    // selection is destroyed and a new current selection value (non-null) is
    // constructed.  A selector of '-1' and an empty types catalog indicates
    // that there is no current selection (the "unset choice").  The current
    // selection's nullability can be queried via a 'ElemRef' returned
    // from 'selection'.  Please refer to the 'Nullability' section in the
    // component-level documentation for this component.
    //
    // Because putting a value into a 'Choice' makes a copy of that value,
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
    ChoiceImp d_choiceImp;  // choice implementation

    // FRIENDS
    friend bool operator==(const Choice&, const Choice&);

  public:
    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(Choice,
                                 bslalg::TypeTraitUsesBslmaAllocator);

    // CLASS METHODS
    static int maxSupportedBdexVersion();
        // Return the most current 'bdex' streaming version number supported by
        // this class.  (See the package-group-level documentation for more
        // information on 'bdex' streaming of container types.)

    // CREATORS
    explicit
    Choice(bslma::Allocator                         *basicAllocator = 0);
    explicit
    Choice(AggregateOption::AllocationStrategy  allocationStrategy,
                bslma::Allocator                         *basicAllocator = 0);
        // Create an unselected choice having an empty types catalog.
        // Optionally specify a memory 'allocationStrategy'.  If
        // 'allocationStrategy' is not specified, then 'BDEM_PASS_THROUGH' is
        // used.  (The meanings of the various 'allocationStrategy' values are
        // described in 'bdlmxxx_aggregateoption'.)  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.

    Choice(const ElemType::Type                 typesCatalog[],
                int                                       numTypes,
                bslma::Allocator                         *basicAllocator = 0);
    Choice(const ElemType::Type                 typesCatalog[],
                int                                       numTypes,
                AggregateOption::AllocationStrategy  allocationStrategy,
                bslma::Allocator                         *basicAllocator = 0);
        // Create an unselected choice having a types catalog of 'numTypes'
        // that are the same as those in the specified sequence 'typesCatalog'.
        // Optionally specify a memory 'allocationStrategy'.  If
        // 'allocationStrategy' is not specified, then 'BDEM_PASS_THROUGH' is
        // used.  (The meanings of the various 'allocationStrategy' values are
        // described in 'bdlmxxx_aggregateoption'.)  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.  The behavior is
        // undefined unless '0 <= numTypes' and 'typesCatalog' has at least
        // 'numTypes' types.

    Choice(const bsl::vector<ElemType::Type>&   typesCatalog,
                bslma::Allocator                         *basicAllocator = 0);
    Choice(const bsl::vector<ElemType::Type>&   typesCatalog,
                AggregateOption::AllocationStrategy  allocationStrategy,
                bslma::Allocator                         *basicAllocator = 0);
        // Create an unselected choice having a types catalog whose types are
        // the same as those in the specified 'typesCatalog'.  Optionally
        // specify a memory 'allocationStrategy'.  If 'allocationStrategy' is
        // not specified, then 'BDEM_PASS_THROUGH' is used.  (The meanings of
        // the various 'allocationStrategy' values are described in
        // 'bdlmxxx_aggregateoption'.)  Optionally specify a 'basicAllocator' used
        // to supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.

    Choice(const Choice&                        original,
                bslma::Allocator                         *basicAllocator = 0);
    Choice(const Choice&                        original,
                AggregateOption::AllocationStrategy  allocationStrategy,
                bslma::Allocator                         *basicAllocator = 0);
    explicit Choice(const ChoiceArrayItem&      original,
                         bslma::Allocator                *basicAllocator = 0);
    Choice(const ChoiceArrayItem&               original,
                AggregateOption::AllocationStrategy  allocationStrategy,
                bslma::Allocator                         *basicAllocator = 0);
        // Create a choice having the value of the specified 'original' object
        // (choice or choice array item).  Optionally specify a memory
        // 'allocationStrategy'.  If 'allocationStrategy' is not specified,
        // then 'BDEM_PASS_THROUGH' is used.  (The meanings of the various
        // 'allocationStrategy' values are described in
        // 'bdlmxxx_aggregateoption'.)  Optionally specify a 'basicAllocator' used
        // to supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.

    ~Choice();
        // Destroy this choice object.  If the choice was constructed with any
        // memory allocation strategy other than 'BDEM_PASS_THROUGH', then
        // destructors are not called on the contained selection.  The memory
        // used by those selections will be released efficiently (all at once)
        // when the internal allocator is destroyed.

    // MANIPULATORS
    Choice& operator=(const ChoiceArrayItem& rhs);
    Choice& operator=(const Choice& rhs);
        // Assign to this choice the types catalog, selector, and value
        // contained in the specified 'rhs' choice or choice array item, and
        // return a reference to this modifiable choice.

    ChoiceArrayItem& item();
        // Return a reference to a modifiable choice array item proxy for this
        // choice.

    ElemRef makeSelection(int index);
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

    ElemRef selection();
        // Return an element reference to the modifiable current selection of
        // this choice if '-1 != selector()', and a void element reference
        // otherwise.

    bool& theModifiableBool();
    char& theModifiableChar();
    short& theModifiableShort();
    int& theModifiableInt();
    bsls::Types::Int64& theModifiableInt64();
    float& theModifiableFloat();
    double& theModifiableDouble();
    bsl::string& theModifiableString();
    bdlt::Datetime& theModifiableDatetime();
    bdlt::DatetimeTz& theModifiableDatetimeTz();
    bdlt::Date& theModifiableDate();
    bdlt::DateTz& theModifiableDateTz();
    bdlt::Time& theModifiableTime();
    bdlt::TimeTz& theModifiableTimeTz();
    bsl::vector<bool>& theModifiableBoolArray();
    bsl::vector<char>& theModifiableCharArray();
    bsl::vector<short>& theModifiableShortArray();
    bsl::vector<int>& theModifiableIntArray();
    bsl::vector<bsls::Types::Int64>& theModifiableInt64Array();
    bsl::vector<float>& theModifiableFloatArray();
    bsl::vector<double>& theModifiableDoubleArray();
    bsl::vector<bsl::string>& theModifiableStringArray();
    bsl::vector<bdlt::Datetime>& theModifiableDatetimeArray();
    bsl::vector<bdlt::DatetimeTz>& theModifiableDatetimeTzArray();
    bsl::vector<bdlt::Date>& theModifiableDateArray();
    bsl::vector<bdlt::DateTz>& theModifiableDateTzArray();
    bsl::vector<bdlt::Time>& theModifiableTimeArray();
    bsl::vector<bdlt::TimeTz>& theModifiableTimeTzArray();
    Choice& theModifiableChoice();
    ChoiceArray& theModifiableChoiceArray();
    List& theModifiableList();
    Table& theModifiableTable();
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

    void addSelection(ElemType::Type elemType);
        // Append the specified 'elemType' to the types catalog of this choice.
        // Note that 'elemType' need not be unique in the types catalog.

    void reset();
        // Destroy the current selection of this choice (if any), reset the
        // choice to the unselected state, and reset the types catalog to be
        // empty.

    void reset(const ElemType::Type typesCatalog[],
               int                       numTypes);
        // Destroy the current selection of this choice (if any), reset the
        // choice to the unselected state, and reset the types catalog to have
        // the specified initial 'numTypes' types in the specified
        // 'typesCatalog' array.  The behavior is undefined unless
        // '0 <= numTypes' and 'typesCatalog' has at least 'numTypes' types.

    void reset(const bsl::vector<ElemType::Type>& typesCatalog);
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

    const ChoiceArrayItem& item() const;
        // Return a reference to a non-modifiable choice array item proxy for
        // this choice.

    ElemType::Type selectionType() const;
        // Return the type of the current selection of this choice if it is
        // currently selected, and 'ElemType::BDEM_VOID' otherwise.

    ElemType::Type selectionType(int index) const;
        // Return the type at the specified 'index' in the types catalog of
        // this choice if '-1 != index', and 'ElemType::BDEM_VOID'
        // otherwise.  The behavior is undefined unless
        // '-1 <= index < numSelections()'.

    void selectionTypes(bsl::vector<ElemType::Type> *result) const;
        // Replace the contents of the specified 'result' with the sequence of
        // types in the types catalog of this choice.

    ConstElemRef selection() const;
        // Return an element reference to the non-modifiable current selection
        // of this choice if '-1 != selector()', and a void element reference
        // otherwise.

    const bool& theBool() const;
    const char& theChar() const;
    const short& theShort() const;
    const int& theInt() const;
    const bsls::Types::Int64& theInt64() const;
    const float& theFloat() const;
    const double& theDouble() const;
    const bsl::string& theString() const;
    const bdlt::Datetime& theDatetime() const;
    const bdlt::DatetimeTz& theDatetimeTz() const;
    const bdlt::Date& theDate() const;
    const bdlt::DateTz& theDateTz() const;
    const bdlt::Time& theTime() const;
    const bdlt::TimeTz& theTimeTz() const;
    const bsl::vector<bool>& theBoolArray() const;
    const bsl::vector<char>& theCharArray() const;
    const bsl::vector<short>& theShortArray() const;
    const bsl::vector<int>& theIntArray() const;
    const bsl::vector<bsls::Types::Int64>& theInt64Array() const;
    const bsl::vector<float>& theFloatArray() const;
    const bsl::vector<double>& theDoubleArray() const;
    const bsl::vector<bsl::string>& theStringArray() const;
    const bsl::vector<bdlt::Datetime>& theDatetimeArray() const;
    const bsl::vector<bdlt::DatetimeTz>& theDatetimeTzArray() const;
    const bsl::vector<bdlt::Date>& theDateArray() const;
    const bsl::vector<bdlt::DateTz>& theDateTzArray() const;
    const bsl::vector<bdlt::Time>& theTimeArray() const;
    const bsl::vector<bdlt::TimeTz>& theTimeTzArray() const;
    const Choice& theChoice() const;
    const ChoiceArray& theChoiceArray() const;
    const List& theList() const;
    const Table& theTable() const;
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
bool operator==(const Choice& lhs, const Choice& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' choice objects have the
    // same value, and 'false' otherwise.  Two choices have the same value if
    // they have the same types catalog, the same selector, and the same
    // selection value.

inline
bool operator!=(const Choice& lhs, const Choice& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' choice objects do not
    // have the same value, and 'false' otherwise.  Two choices do not have the
    // same value if they have different types catalogs, different selectors,
    // or different current selection values.

inline
bsl::ostream& operator<<(bsl::ostream& stream, const Choice& rhs);
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
                        // class Choice
                        // -----------------

// CLASS METHODS
inline
int Choice::maxSupportedBdexVersion()
{
    return 3;
}

// CREATORS
inline
Choice::Choice(bslma::Allocator *basicAllocator)
: d_choiceImp(basicAllocator)
{
}

inline
Choice::Choice(
                  AggregateOption::AllocationStrategy  allocationStrategy,
                  bslma::Allocator                         *basicAllocator)
: d_choiceImp(allocationStrategy, basicAllocator)
{
}

inline
Choice::Choice(const ElemType::Type  typesCatalog[],
                         int                        numTypes,
                         bslma::Allocator          *basicAllocator)
: d_choiceImp(typesCatalog,
              numTypes,
              ElemAttrLookup::lookupTable(),
              AggregateOption::BDEM_PASS_THROUGH,
              basicAllocator)
{
    BSLS_ASSERT_SAFE(0 <= numTypes);
}

inline
Choice::Choice(
                  const ElemType::Type                 typesCatalog[],
                  int                                       numTypes,
                  AggregateOption::AllocationStrategy  allocationStrategy,
                  bslma::Allocator                         *basicAllocator)
: d_choiceImp(typesCatalog,
              numTypes,
              ElemAttrLookup::lookupTable(),
              allocationStrategy,
              basicAllocator)
{
    BSLS_ASSERT_SAFE(0 <= numTypes);
}

inline
Choice::Choice(
                      const bsl::vector<ElemType::Type>&  typesCatalog,
                      bslma::Allocator                        *basicAllocator)
: d_choiceImp(0 == typesCatalog.size() ? 0 : &typesCatalog[0],
              (int)typesCatalog.size(),
              ElemAttrLookup::lookupTable(),
              AggregateOption::BDEM_PASS_THROUGH,
              basicAllocator)
{
}

inline
Choice::Choice(
                  const bsl::vector<ElemType::Type>&   typesCatalog,
                  AggregateOption::AllocationStrategy  allocationStrategy,
                  bslma::Allocator                         *basicAllocator)
: d_choiceImp(0 == typesCatalog.size() ? 0 : &typesCatalog[0],
              (int)typesCatalog.size(),
              ElemAttrLookup::lookupTable(),
              allocationStrategy,
              basicAllocator)
{
}

inline
Choice::Choice(const Choice&  original,
                         bslma::Allocator   *basicAllocator)
: d_choiceImp(original.d_choiceImp, basicAllocator)
{
}

inline
Choice::Choice(
                  const Choice&                        original,
                  AggregateOption::AllocationStrategy  allocationStrategy,
                  bslma::Allocator                         *basicAllocator)
: d_choiceImp(original.d_choiceImp, allocationStrategy, basicAllocator)
{
}

inline
Choice::Choice(const ChoiceArrayItem&  original,
                         bslma::Allocator            *basicAllocator)
: d_choiceImp((const ChoiceHeader &) original,
              AggregateOption::BDEM_PASS_THROUGH,
              basicAllocator)
{
}

inline
Choice::Choice(
                  const ChoiceArrayItem&               original,
                  AggregateOption::AllocationStrategy  allocationStrategy,
                  bslma::Allocator                         *basicAllocator)
: d_choiceImp((const ChoiceHeader &) original,
              allocationStrategy,
              basicAllocator)
{
}

inline
Choice::~Choice()
{
}

// MANIPULATORS
inline
Choice& Choice::operator=(const Choice& rhs)
{
    if (this != &rhs) {
        d_choiceImp = rhs.d_choiceImp;
    }
    return *this;
}

inline
ChoiceArrayItem& Choice::item()
{
    return (ChoiceArrayItem &)d_choiceImp.choiceHeader();
}

inline
ElemRef Choice::makeSelection(int index)
{
    BSLS_ASSERT_SAFE(-1 <= index);
    BSLS_ASSERT_SAFE(      index < d_choiceImp.numSelections());

    return d_choiceImp.makeSelection(index);
}

inline
ElemRef Choice::selection()
{
    return d_choiceImp.selection();
}

inline
bool& Choice::theModifiableBool()
{
    d_choiceImp.clearNullnessBit();
    return *(bool *)d_choiceImp.selectionPointer();
}

inline
char& Choice::theModifiableChar()
{
    d_choiceImp.clearNullnessBit();
    return *(char *)d_choiceImp.selectionPointer();
}

inline
short& Choice::theModifiableShort()
{
    d_choiceImp.clearNullnessBit();
    return *(short *)d_choiceImp.selectionPointer();
}

inline
int& Choice::theModifiableInt()
{
    d_choiceImp.clearNullnessBit();
    return *(int *)d_choiceImp.selectionPointer();
}

inline
bsls::Types::Int64& Choice::theModifiableInt64()
{
    d_choiceImp.clearNullnessBit();
    return *(bsls::Types::Int64 *)d_choiceImp.selectionPointer();
}

inline
float& Choice::theModifiableFloat()
{
    d_choiceImp.clearNullnessBit();
    return *(float *)d_choiceImp.selectionPointer();
}

inline
double& Choice::theModifiableDouble()
{
    d_choiceImp.clearNullnessBit();
    return *(double *)d_choiceImp.selectionPointer();
}

inline
bsl::string& Choice::theModifiableString()
{
    d_choiceImp.clearNullnessBit();
    return *(bsl::string *)d_choiceImp.selectionPointer();
}

inline
bdlt::Datetime& Choice::theModifiableDatetime()
{
    d_choiceImp.clearNullnessBit();
    return *(bdlt::Datetime *)d_choiceImp.selectionPointer();
}

inline
bdlt::DatetimeTz& Choice::theModifiableDatetimeTz()
{
    d_choiceImp.clearNullnessBit();
    return *(bdlt::DatetimeTz *)d_choiceImp.selectionPointer();
}

inline
bdlt::Date& Choice::theModifiableDate()
{
    d_choiceImp.clearNullnessBit();
    return *(bdlt::Date *)d_choiceImp.selectionPointer();
}

inline
bdlt::DateTz& Choice::theModifiableDateTz()
{
    d_choiceImp.clearNullnessBit();
    return *(bdlt::DateTz *)d_choiceImp.selectionPointer();
}

inline
bdlt::Time& Choice::theModifiableTime()
{
    d_choiceImp.clearNullnessBit();
    return *(bdlt::Time *)d_choiceImp.selectionPointer();
}

inline
bdlt::TimeTz& Choice::theModifiableTimeTz()
{
    d_choiceImp.clearNullnessBit();
    return *(bdlt::TimeTz *)d_choiceImp.selectionPointer();
}

inline
bsl::vector<bool>& Choice::theModifiableBoolArray()
{
    d_choiceImp.clearNullnessBit();
    return *(bsl::vector<bool> *)d_choiceImp.selectionPointer();
}

inline
bsl::vector<char>& Choice::theModifiableCharArray()
{
    d_choiceImp.clearNullnessBit();
    return *(bsl::vector<char> *)d_choiceImp.selectionPointer();
}

inline
bsl::vector<short>& Choice::theModifiableShortArray()
{
    d_choiceImp.clearNullnessBit();
    return *(bsl::vector<short> *)d_choiceImp.selectionPointer();
}

inline
bsl::vector<int>& Choice::theModifiableIntArray()
{
    d_choiceImp.clearNullnessBit();
    return *(bsl::vector<int> *)d_choiceImp.selectionPointer();
}

inline
bsl::vector<bsls::Types::Int64>& Choice::theModifiableInt64Array()
{
    d_choiceImp.clearNullnessBit();
    return *(bsl::vector<bsls::Types::Int64> *)d_choiceImp.selectionPointer();
}

inline
bsl::vector<float>& Choice::theModifiableFloatArray()
{
    d_choiceImp.clearNullnessBit();
    return *(bsl::vector<float> *)d_choiceImp.selectionPointer();
}

inline
bsl::vector<double>& Choice::theModifiableDoubleArray()
{
    d_choiceImp.clearNullnessBit();
    return *(bsl::vector<double> *)d_choiceImp.selectionPointer();
}

inline
bsl::vector<bsl::string>& Choice::theModifiableStringArray()
{
    d_choiceImp.clearNullnessBit();
    return *(bsl::vector<bsl::string> *)d_choiceImp.selectionPointer();
}

inline
bsl::vector<bdlt::Datetime>& Choice::theModifiableDatetimeArray()
{
    d_choiceImp.clearNullnessBit();
    return *(bsl::vector<bdlt::Datetime> *)d_choiceImp.selectionPointer();
}

inline
bsl::vector<bdlt::DatetimeTz>& Choice::theModifiableDatetimeTzArray()
{
    d_choiceImp.clearNullnessBit();
    return *(bsl::vector<bdlt::DatetimeTz> *)d_choiceImp.selectionPointer();
}

inline
bsl::vector<bdlt::Date>& Choice::theModifiableDateArray()
{
    d_choiceImp.clearNullnessBit();
    return *(bsl::vector<bdlt::Date> *)d_choiceImp.selectionPointer();
}

inline
bsl::vector<bdlt::DateTz>& Choice::theModifiableDateTzArray()
{
    d_choiceImp.clearNullnessBit();
    return *(bsl::vector<bdlt::DateTz> *)d_choiceImp.selectionPointer();
}

inline
bsl::vector<bdlt::Time>& Choice::theModifiableTimeArray()
{
    d_choiceImp.clearNullnessBit();
    return *(bsl::vector<bdlt::Time> *)d_choiceImp.selectionPointer();
}

inline
bsl::vector<bdlt::TimeTz>& Choice::theModifiableTimeTzArray()
{
    d_choiceImp.clearNullnessBit();
    return *(bsl::vector<bdlt::TimeTz> *)d_choiceImp.selectionPointer();
}

inline
List& Choice::theModifiableList()
{
    d_choiceImp.clearNullnessBit();
    return *(List *)d_choiceImp.selectionPointer();
}

inline
Table& Choice::theModifiableTable()
{
    d_choiceImp.clearNullnessBit();
    return *(Table *)d_choiceImp.selectionPointer();
}

inline
Choice& Choice::theModifiableChoice()
{
    d_choiceImp.clearNullnessBit();
    return *(Choice *)d_choiceImp.selectionPointer();
}

inline
ChoiceArray& Choice::theModifiableChoiceArray()
{
    d_choiceImp.clearNullnessBit();
    return *(ChoiceArray *)d_choiceImp.selectionPointer();
}

inline
void Choice::addSelection(ElemType::Type elemType)
{
    d_choiceImp.addSelection(ElemAttrLookup::lookupTable()
                                                [static_cast<int>(elemType)]);
}

inline
void Choice::reset()
{
    d_choiceImp.reset();
}

inline
void Choice::reset(const ElemType::Type typesCatalog[], int numTypes)
{
    BSLS_ASSERT_SAFE(0 <= numTypes);

    d_choiceImp.reset(typesCatalog,
                      numTypes,
                      ElemAttrLookup::lookupTable());
}

inline
void Choice::reset(const bsl::vector<ElemType::Type>& typesCatalog)
{
    const ElemType::Type *begin = 0 == typesCatalog.size()
                                     ? 0
                                     : &typesCatalog[0];
    return reset(begin, (int)typesCatalog.size());
}

template <class STREAM>
inline
STREAM& Choice::bdexStreamIn(STREAM& stream, int version)
{
    return d_choiceImp.bdexStreamInImp(
                            stream,
                            version,
                            ElemStreamInAttrLookup<STREAM>::lookupTable(),
                            ElemAttrLookup::lookupTable());
}

// ACCESSORS
inline
int Choice::numSelections() const
{
    return d_choiceImp.numSelections();
}

inline
int Choice::selector() const
{
    return d_choiceImp.selector();
}

inline
const ChoiceArrayItem& Choice::item() const
{
    return (const ChoiceArrayItem &) d_choiceImp.choiceHeader();
}

inline
ElemType::Type Choice::selectionType() const
{
    return selectionType(selector());
}

inline
ElemType::Type Choice::selectionType(int index) const
{
    BSLS_ASSERT_SAFE(-1 <= index);
    BSLS_ASSERT_SAFE(      index < numSelections());

    return d_choiceImp.selectionType(index);
}

inline
ConstElemRef Choice::selection() const
{
    return d_choiceImp.selection();
}

inline
const bool& Choice::theBool() const
{
    return *(const bool *)d_choiceImp.selectionPointer();
}

inline
const char& Choice::theChar() const
{
    return *(const char *)d_choiceImp.selectionPointer();
}

inline
const short& Choice::theShort() const
{
    return *(const short *)d_choiceImp.selectionPointer();
}

inline
const int& Choice::theInt() const
{
    return *(const int *)d_choiceImp.selectionPointer();
}

inline
const bsls::Types::Int64& Choice::theInt64() const
{
    return *(const bsls::Types::Int64 *)d_choiceImp.selectionPointer();
}

inline
const float& Choice::theFloat() const
{
    return *(const float *)d_choiceImp.selectionPointer();
}

inline
const double& Choice::theDouble() const
{
    return *(const double *)d_choiceImp.selectionPointer();
}

inline
const bsl::string& Choice::theString() const
{
    return *(const bsl::string *)d_choiceImp.selectionPointer();
}

inline
const bdlt::Datetime& Choice::theDatetime() const
{
    return *(const bdlt::Datetime *)d_choiceImp.selectionPointer();
}

inline
const bdlt::DatetimeTz& Choice::theDatetimeTz() const
{
    return *(const bdlt::DatetimeTz *)d_choiceImp.selectionPointer();
}

inline
const bdlt::Date& Choice::theDate() const
{
    return *(const bdlt::Date *)d_choiceImp.selectionPointer();
}

inline
const bdlt::DateTz& Choice::theDateTz() const
{
    return *(const bdlt::DateTz *)d_choiceImp.selectionPointer();
}

inline
const bdlt::Time& Choice::theTime() const
{
    return *(const bdlt::Time *)d_choiceImp.selectionPointer();
}

inline
const bdlt::TimeTz& Choice::theTimeTz() const
{
    return *(const bdlt::TimeTz *)d_choiceImp.selectionPointer();
}

inline
const bsl::vector<bool>& Choice::theBoolArray() const
{
    return *(const bsl::vector<bool> *)d_choiceImp.selectionPointer();
}

inline
const bsl::vector<char>& Choice::theCharArray() const
{
    return *(const bsl::vector<char> *)d_choiceImp.selectionPointer();
}

inline
const bsl::vector<short>& Choice::theShortArray() const
{
    return *(const bsl::vector<short> *)d_choiceImp.selectionPointer();
}

inline
const bsl::vector<int>& Choice::theIntArray() const
{
    return *(const bsl::vector<int> *)d_choiceImp.selectionPointer();
}

inline
const bsl::vector<bsls::Types::Int64>&
Choice::theInt64Array() const
{
    return *(const bsl::vector<bsls::Types::Int64> *)
                                                d_choiceImp.selectionPointer();
}

inline
const bsl::vector<float>& Choice::theFloatArray() const
{
    return *(const bsl::vector<float> *)d_choiceImp.selectionPointer();
}

inline
const bsl::vector<double>& Choice::theDoubleArray() const
{
    return *(const bsl::vector<double> *)d_choiceImp.selectionPointer();
}

inline
const bsl::vector<bsl::string>& Choice::theStringArray() const
{
    return *(const bsl::vector<bsl::string> *)d_choiceImp.selectionPointer();
}

inline
const bsl::vector<bdlt::Datetime>& Choice::theDatetimeArray() const
{
    return *(const bsl::vector<bdlt::Datetime> *)
                                               d_choiceImp.selectionPointer();
}

inline
const bsl::vector<bdlt::DatetimeTz>& Choice::theDatetimeTzArray() const
{
    return *(const bsl::vector<bdlt::DatetimeTz> *)
                                               d_choiceImp.selectionPointer();
}

inline
const bsl::vector<bdlt::Date>& Choice::theDateArray() const
{
    return *(const bsl::vector<bdlt::Date> *)d_choiceImp.selectionPointer();
}

inline
const bsl::vector<bdlt::DateTz>& Choice::theDateTzArray() const
{
    return *(const bsl::vector<bdlt::DateTz> *)d_choiceImp.selectionPointer();
}

inline
const bsl::vector<bdlt::Time>& Choice::theTimeArray() const
{
    return *(const bsl::vector<bdlt::Time> *)d_choiceImp.selectionPointer();
}

inline
const bsl::vector<bdlt::TimeTz>& Choice::theTimeTzArray() const
{
    return *(const bsl::vector<bdlt::TimeTz> *)d_choiceImp.selectionPointer();
}

inline
const List& Choice::theList() const
{
    return *(const List *)d_choiceImp.selectionPointer();
}

inline
const Table& Choice::theTable() const
{
    return *(const Table *)d_choiceImp.selectionPointer();
}

inline
const Choice& Choice::theChoice() const
{
    return *(const Choice *)d_choiceImp.selectionPointer();
}

inline
const ChoiceArray& Choice::theChoiceArray() const
{
    return *(const ChoiceArray *)d_choiceImp.selectionPointer();
}

template <class STREAM>
inline
STREAM& Choice::bdexStreamOut(STREAM& stream, int version) const
{
    return d_choiceImp.bdexStreamOutImp(
                          stream,
                          version,
                          ElemStreamOutAttrLookup<STREAM>::lookupTable());
}
}  // close package namespace

// FREE OPERATORS
inline
bool bdlmxxx::operator==(const Choice& lhs, const Choice& rhs)
{
    return lhs.d_choiceImp == rhs.d_choiceImp;
}

inline
bool bdlmxxx::operator!=(const Choice& lhs, const Choice& rhs)
{
    return !(lhs == rhs);
}

inline
bsl::ostream& bdlmxxx::operator<<(bsl::ostream& stream, const Choice& rhs)
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
