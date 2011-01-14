// bdem_choiceimp.h                                                   -*-C++-*-
#ifndef INCLUDED_BDEM_CHOICEIMP
#define INCLUDED_BDEM_CHOICEIMP

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide implementation of a 'bdem' choice.
//
//@CLASSES:
// bdem_ChoiceImp: provides implementation of the 'bdem' choice
//
//@SEE_ALSO: bdem_choiceheader, bdem_choice
//
//@AUTHOR: Pablo Halpern (phalpern), Anthony Comerico (acomeric)
//
//@DESCRIPTION: This component defines a class, 'bdem_ChoiceImp', that
// provides the implementation for the 'bdem_Choice' class.  'bdem_ChoiceImp'
// is used exclusively by 'bdem_Choice', and clients should use
// 'bdem_Choice' instead of using this class directly.
//
// A choice imp object stores a list of 'bdem_ElemType::Type' values and the
// data value for one of the types in that list.  'bdem_ChoiceImp' provides
// functions to specify the list of available types, change the selected type
// and the corresponding value.
//
///Terminology
///-----------
// This section describes the basic terminology used throughout this component.
//
// 1. Types Catalog: The set of 'bdem_ElemType::Type' values that are
// associated with each choice imp and that specifies the various types that
// the choice imp can represent.  A choice imp can have at most one type
// selected from this list at a time.
//
// 2. Selector: The index of the currently selected type in the types catalog
// or -1 if no type is currently selected.
//
// 3. Selection value: The data value corresponding to the currently selected
// type.  'bdem_ChoiceImp' is responsible for the construction, management and
// destruction of this data value.
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
// To illustrate, consider a 'bdem_ChoiceImp' with a 'BDEM_BOOL' selection.
// The selection can be in one of three possible states:
//: o  null with underlying value 'bdetu_Unset<bool>::unsetValue()'
//: o  non-null with underlying value 'false'
//: o  non-null with underlying value 'true'
//
// The underlying value of a null 'bdem' object is a class invariant.  If an
// object is null, that object *also* has the unset value corresponding to its
// type.
//
// For example, suppose we have a 'bdem_ChoiceImp', 'myChoiceImp', whose
// current selection is of type 'BDEM_STRING':
//..
//  myChoiceImp.selection().theModifiableString() = "HELLO";
//  assert(myChoiceImp.selection().isNonNull());
//..
// Making the selection null also makes it have the unset value (which, for
// 'BDEM_STRING', is the empty string):
//..
//  myChoiceImp.selection().makeNull();
//  assert(myChoiceImp.selection().isNull());
//  assert(myChoiceImp.selection().theString().empty());
//..
///Choice Imp Nullability
///- - - - - - - - - - - -
// Though a choice imp can be unset (selector of -1 and an empty types catalog)
// and its current selection can be null, the choice imp itself can be null
// only in relation to another object (its parent) in which it is a member.
// The nullness of the current selection can be queried via a 'bdem_ElemRef'
// returned from 'selection' to determine whether its contained object (the
// current selection) is null, e.g., 'selection().isNull()'.
//
// The preferred way to assign a non-null value to a choice imp selection is
// through the 'theModifiableTYPE' methods, e.g., 'theModifiableBool' for
// a choice imp that is selected to be of type 'bool'.  The
// 'theModifiableTYPE' methods have the side-effect of making a null value
// non-null, so the 'const' 'theTYPE' methods should be used instead if the
// value is only being accessed (and not modified).
//
// Note that assigning a value through the pointer returned from
// 'selectionPointer' for a choice imp whose selection value is null results
// in undefined behavior.  The best way to assign a value to a choice imp
// (regardless of whether the selection value is null or not) is via
// the 'setSelectionValue' function.  Alternatively, a call to
// 'clearNullnessBit', can be used to force a choice imp's selection to a
// non-null state, and then to assign a value through the pointer returned by
// 'selectionPointer'.
//
// Note that a 'bdem_ChoiceImp' may contain a sub-object of 'bdem_ChoiceImp',
// though the top level 'bdem_ChoiceImp' does not know its own nullness, the
// 'bdem_ElemRef returned by 'selection' will correctly report the nullness of
// the nested choice sub-object.
//
// The element contained by a choiceimp can be null.  The way to set it null
// is, for a choice imp 'myChoiceImp', to call
// 'myChoiceImp.selection.makeNull()'.  The nullness bit can be cleared without
// affecting the element's value by calling 'mX.clearNullnessBit()'.
//
///Thread Safety
///-------------
// It is safe to access separate 'bdem_ChoiceImp' objects simultaneously in
// separate threads.  It is not safe to access a single 'bdem_ChoiceImp'
// object simultaneously in separate threads.
//
///Usage
///-----
// The 'divide' function reads two 'double' values, divides the first by the
// second, and returns a choice imp representing the result of the division.
// The signature of the function looks as follows:
//..
// bdem_ChoiceImp divide(double dividend, double divisor) {
//..
// The division normally will result in a double value, but will sometimes
// result in an error string.  The latter case can occur because the division
// itself failed because the divisor was zero.  The result of the division is
// therefore packaged in a 'bdem_ChoiceImp' which can store either a 'double'
// or a 'string'.  The current selection is indexed by a value of 0 or 1 for
// the double or string for division error.  We specify the types catalog for
// our choice imp object below:
//..
//      static const bdem_ElemType::Type CHOICE_TYPES[] = {
//          bdem_ElemType::BDEM_DOUBLE,  // result value
//          bdem_ElemType::BDEM_STRING   // division error
//      };
//      static const int NUM_CHOICES =
//                                  sizeof CHOICE_TYPES / sizeof *CHOICE_TYPES;
//..
// Now we will construct our choice imp object providing the types catalog,
// and a descriptor array that was previously constructed.
//..
//      enum { RESULT_VAL, DIVISION_ERROR };
//
//      bdem_ChoiceImp result(CHOICE_TYPES,
//                            NUM_CHOICES,
//                            MY_DESCRIPTORS,
//                            bdem_AggregateOption::BDEM_PASS_THROUGH);
//      ASSERT(-1 == result.selector());
//
//      if (0 == divisor) {
//..
// If the division fails, the choice imp is set to 'DIVISION_ERROR' and the
// corresponding selection is set to an error string.  Note the alternate
// syntax ('selection().theString()') for setting and getting the current
// element.
//..
//          bsl::string errString = "Divided by zero.";
//          result.makeSelection(DIVISION_ERROR);
//          result.selection().theString() = errString;
//          ASSERT(DIVISION_ERROR == result.selector());
//          ASSERT(errString == result.selection().theString());
//      }
//      else {
//..
// If there are no errors, compute the quotient and store it as a 'double'
// with selector, 'RESULT_VAL'.
//..
//          const double quotient = dividend / divisor;
//          result.makeSelection(RESULT_VAL).theDouble() = quotient;
//          ASSERT(RESULT_VAL == result.selector());
//          ASSERT(quotient == result.selection().theDouble());
//      }
//..
// Once the result is calculated, it is returned from the function.
//..
//      return result;
//  }
//..

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BSLMA_ALLOCATOR
#include <bslma_allocator.h>
#endif

#ifndef INCLUDED_BSLS_ALIGNMENT
#include <bsls_alignment.h>
#endif

#ifndef INCLUDED_BDEM_AGGREGATEOPTION
#include <bdem_aggregateoption.h>
#endif

#ifndef INCLUDED_BDEM_ALLOCATORMANAGER
#include <bdem_allocatormanager.h>
#endif

#ifndef INCLUDED_BDEM_CHOICEHEADER
#include <bdem_choiceheader.h>
#endif

#ifndef INCLUDED_BDEM_DESCRIPTOR
#include <bdem_descriptor.h>
#endif

#ifndef INCLUDED_BDEM_ELEMREF
#include <bdem_elemref.h>
#endif

#ifndef INCLUDED_BDEM_ELEMTYPE
#include <bdem_elemtype.h>
#endif

#ifndef INCLUDED_BDEU_PRINTMETHODS
#include <bdeu_printmethods.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITBITWISEMOVEABLE
#include <bslalg_typetraitbitwisemoveable.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITS
#include <bslalg_typetraits.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITUSESBSLMAALLOCATOR
#include <bslalg_typetraitusesbslmaallocator.h>
#endif

#ifndef INCLUDED_BSLS_ALIGNMENT
#include <bsls_alignment.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif

#ifndef INCLUDED_BSL_VECTOR
#include <bsl_vector.h>
#endif

namespace BloombergLP {

class bslma_Allocator;

                        // ====================
                        // class bdem_ChoiceImp
                        // ====================

class bdem_ChoiceImp {
    // This class implements the 'bdem_Choice' class.  Each choice imp object
    // can represent at most one type at a time from a types catalog supplied
    // at construction.  A selector index specifies which type from the types
    // catalog the current object refers to.  For each selected object this
    // class creates and manages a data value of the selected type.  Changing
    // the selector causes the current object to be destroyed and a new object
    // to be created.  A selector of -1 indicates that there is no current
    // selection.  Note that the memory management (i.e., allocator usage) for
    // the types catalog is handled by this class.
    //
    // Additionally, this component provides the externalization functions
    // 'bdexStreamInImp' and 'bdexStreamOutImp'.  A 'bdem_ChoiceImp' can thus
    // be streamed to an older program provided the features being used are
    // compatible with that version of the object.

    // PRIVATE TYPES
    typedef bdem_ChoiceHeader::DescriptorCatalog DescriptorCatalog;
        // 'DescriptorCatalog' is an alias for a sequence of
        // 'const bdem_Descriptor *' values.

    // DATA
    // NOTE: Do *NOT* change the order of the data members.  The implementation
    // relies on them being declared in this order.
    bdem_AllocatorManager d_allocatorManager;  // allocator manager
    bdem_ChoiceHeader     d_header;            // choice header

    // FRIENDS
    friend bool operator==(const bdem_ChoiceImp&, const bdem_ChoiceImp&);

  public:
    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS3(bdem_ChoiceImp,
                                  bslalg_TypeTraitBitwiseMoveable,
                                  bslalg_TypeTraitUsesBslmaAllocator,
                                  bdeu_TypeTraitHasPrintMethod);

    // CLASS DATA
    static const bdem_Descriptor d_choiceAttr;
        // Descriptor for the 'bdem_ChoiceImp' class.  Contains pointers to
        // functions for construction, destruction, assignment, equality test,
        // etc.

    // CLASS METHODS
    template <class STREAM>
    static void
    streamInChoice(void                                  *obj,
                   STREAM&                                stream,
                   int                                    version,
                   const bdem_DescriptorStreamIn<STREAM> *strmAttrLookup,
                   const bdem_Descriptor                 *const attrLookup[]);
        // Stream into the choice imp object whose address is specified by
        // 'obj' the data specified in 'stream' of the specified 'version'
        // using the specified 'strmAttrLookup' stream in descriptors and the
        // specified 'attrLookup' table of type descriptors.  Note
        // that the previous value of '*this' is discarded.  The 'version' is
        // mandatory and indicates the expected input stream format.
        // 'strmAttrLookup' maps each known element type to a corresponding
        // function for streaming in that type.  'attrLookup' maps each known
        // element type to a descriptor (see bdem_descriptor).

    template <class STREAM>
    static void
    streamOutChoice(const void                             *obj,
                    STREAM&                                 stream,
                    int                                     version,
                    const bdem_DescriptorStreamOut<STREAM> *strmAttrLookup);
        // Stream out the data in the choice imp whose address is specified by
        // 'obj' to the specified 'stream' in the specified 'version' using
        // the specified 'strmAttrLookup' stream out descriptors.  The
        // 'version' is mandatory and specifies the format of the output.  The
        // 'version' is *not* written to the stream.  If 'version' is not
        // supported, 'stream' is left unmodified.  Auxiliary information is
        // provided by 'strmAttrLookup', which maps each known element type to
        // a corresponding function for streaming out that type.

    // CREATORS
    explicit bdem_ChoiceImp(
                bslma_Allocator                          *basicAllocator = 0);
    explicit bdem_ChoiceImp(
                bdem_AggregateOption::AllocationStrategy  allocationStrategy,
                bslma_Allocator                          *basicAllocator = 0);
        // Construct a choice imp having an empty types catalog and that is
        // initially unset.  Optionally specify an 'allocationStrategy'.  If
        // 'allocationStrategy' is not specified,
        // 'bdem_AggregateOption::BDEM_PASS_THROUGH' is used.  Optionally
        // specify 'basicAllocator' to supply memory.  If 'basicAllocator' is
        // 0, the currently installed default allocator is used.

    bdem_ChoiceImp(
                 const bdem_ElemType::Type                 selectionTypes[],
                 int                                       numSelectionTypes,
                 const bdem_Descriptor *const              attrLookupTbl[],
                 bdem_AggregateOption::AllocationStrategy  allocationStrategy,
                 bslma_Allocator                          *basicAllocator = 0);
        // Construct a choice imp having a types catalog of the specified
        // 'numSelectionTypes' length and storing the types specified in the
        // 'selectionTypes' array.  Use the specified 'allocationStrategy',
        // and the specified 'attrLookupTbl' table for getting the
        // 'bdem_Descriptor' values corresponding to the various
        // 'bdem_ElemType' values.  Optionally specify a 'basicAllocator' used
        // to supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.  The behavior is undefined unless
        // 'selectionTypes' is an array of size 'numSelectionTypes', for each
        // 'i', such that '0 <= i < numSelectionTypes',
        // '0 <= selectionTypes[i] < bdem_ElemType::BDEM_NUM_TYPE' is true, and
        // 'attrLookupTbl' size is at least the maximum integer among those
        // represented by the 'bdem' element types contained by
        // 'selectionTypes'.

    bdem_ChoiceImp(
                const bdem_ChoiceHeader&                  choiceHeader,
                bdem_AggregateOption::AllocationStrategy  allocationStrategy,
                bslma_Allocator                          *basicAllocator = 0);
        // Construct a choice imp that stores the specified 'choiceHeader'
        // using the specified 'allocationStrategy'.  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.

    bdem_ChoiceImp(const bdem_ChoiceImp&  original,
                   bslma_Allocator       *basicAllocator = 0);
    bdem_ChoiceImp(
                const bdem_ChoiceImp&                     original,
                bdem_AggregateOption::AllocationStrategy  allocationStrategy,
                bslma_Allocator                          *basicAllocator = 0);
        // Create a choice imp initialized to the value of the specified
        // 'original' choice imp.  Optionally specify an 'allocationStrategy'.
        // If 'allocationStrategy' is not specified,
        // 'bdem_AggregateOption::BDEM_PASS_THROUGH' is used.  Optionally
        // specify 'basicAllocator' to supply memory.  If 'basicAllocator' is
        // 0, the currently installed default allocator is used.

    ~bdem_ChoiceImp();
        // Destroy this choice imp destroying the underlying selection value
        // if this object is not null.

    // MANIPULATORS
    bdem_ChoiceImp& operator=(const bdem_ChoiceImp& rhs);
        // Assign to this choice imp the value of the specified 'rhs' choice
        // imp, and return a reference to this modifiable choice imp.  Note
        // that after the assignment, both choice imps will have equal but
        // distinct selection values, or both will be null.

    void addSelection(const bdem_Descriptor *elemAttr);
        // Append the type corresponding to the specified 'elemAttr' descriptor
        // to the types catalog of this choice imp.  Note that this operation
        // does not affect the current selection.

    bdem_ChoiceHeader& choiceHeader();
        // Return a reference to the modifiable choice header owned by this
        // choice imp object.

    void clear();
        // Reset this choice imp to its default constructed state, destroying
        // the underlying selection value if an element type is selected.
        // Note that this method is needed for 'bdem_functiontemplates'.

    void clearNullnessBit();
        // Clear the nullness bit associated with this choice object without
        // affecting the element's value.

    bdem_ElemRef makeSelection(int index);
        // Set the selector of this choice imp to the specified 'index' and the
        // selection value to a null value of the type corresponding to
        // 'index' in the types catalog.  Return an element reference to the
        // modifiable selection value if '-1 != index' and an element reference
        // of type 'bdem_ElemType::BDEM_VOID' otherwise.  The behavior is
        // undefined unless  '-1 <= index < numSelections()'.  Note that if
        // 'selector() == index' then this choice imp is left unchanged.  Also
        // note that calling this function might entail the destruction of the
        // current selection value thereby invalidating previously returned
        // element references.

    bdem_ElemRef makeSelection(int index, const void *value);
        // Set the selector of this choice imp to the specified 'index' and
        // the selection value to the specified 'value' of the type
        // corresponding to 'index' in the types catalog.  Return an element
        // reference to the modifiable selection value if '-1 != index' and an
        // element reference of type 'bdem_ElemType::BDEM_VOID' otherwise.  The
        // behavior is undefined unless '-1 <= index < numSelections()' and
        // 'value' is the address of a data value of the type corresponding to
        // 'index' in the types catalog.  Note that calling this function
        // might entail the destruction of the current selection value thereby
        // invalidating previously returned element references.

    bdem_ElemRef selection();
        // Return an element reference to the modifiable selection value of
        // this choice imp if '-1 != selector()', and an element reference of
        // type 'bdem_ElemType::BDEM_VOID' otherwise.

    void reset();
        // Reset this choice imp to its default constructed state, destroying
        // the underlying selection value if an element type is selected.
        // Note that this function might invalidate previously returned
        // element references.

    void reset(const bdem_ElemType::Type    selectionTypes[],
               int                          numSelectionTypes,
               const bdem_Descriptor *const attrLookupTbl[]);
        // Reset this choice imp to have a types catalog of the specified
        // 'numSelectionTypes' length and storing the types specified in the
        // 'selectionTypes' array.  Use the specified 'attrLookupTbl' table for
        // getting the 'bdem_Descriptor' values corresponding to the various
        // 'bdem_ElemType' values.  The behavior is undefined unless
        // 'selectionTypes' is an array of size 'numSelectionTypes', and for
        // '0 <= i < numSelectionTypes',
        // '0 <= selectionTypes[i] < bdem_ElemType::BDEM_NUM_TYPES'.  Note that
        // calling this function might entail the destruction of the current
        // selection value thereby invalidating previously returned element
        // references.

    void *selectionPointer();
        // Return the address of the modifiable selection value stored in this
        // choice imp if '-1 != selector()' and 0 otherwise.

    void *setSelectionValue(const void *value);
        // Set the current selection value to the specified 'value'.  Return
        // the address of the modifiable selection value if '-1 != index' and
        // 0 otherwise.  The behavior is undefined unless 'value' is the
        // address of a data value of the type corresponding to that returned
        // by 'selector'.  Note that calling this function might entail
        // the destruction of the current selection value thereby invalidating
        // previously returned element references.  Also note that if the
        // '-1 == selector()' then the choice imp will be left unmodified.

    template <class STREAM>
    STREAM&
    bdexStreamInImp(STREAM&                                stream,
                    int                                    version,
                    const bdem_DescriptorStreamIn<STREAM> *strmAttrLookup,
                    const bdem_Descriptor                 *const attrLookup[]);
        // Stream in the data specified in 'stream' of the specified 'version'
        // and populate this choice imp using the specified 'strmAttrLookup'
        // stream in descriptors and the specified 'attrLookup' table of type
        // descriptors.  Return a reference to the modifiable 'stream'.  Note
        // that the previous value of '*this' is discarded.  The 'version' is
        // mandatory and indicates the expected input stream format.
        // 'strmAttrLookup' maps each known element type to a corresponding
        // function for streaming in that type.  'attrLookup' maps each known
        // element type to a descriptor (see bdem_descriptor).
        //
        // If 'version' is not supported, 'stream' is marked invalid and the
        // choice imp is left unchanged.  If 'stream' is initially invalid, the
        // choice imp is left unchanged.  If 'stream' becomes invalid during
        // this operation, 'stream' is marked invalid; choice imp is valid,
        // but its value is unspecified.  Decoding will fail (incorrect data
        // will be read and/or the stream will become invalid) unless the type
        // of object encoded in 'stream' matches the element type encoded in
        // 'stream' and 'stream's version matches 'version'.
        //
        // Note that the type 'STREAM' must adhere to the protocol specified
        // by 'bdex_InStream' (see the package-group-level documentation for
        // more information on 'bdex' streaming of container types).

    // ACCESSORS
    const bdem_ChoiceHeader& choiceHeader() const;
        // Return a reference to the non-modifiable choice header owned by this
        // choice imp object.

    int numSelections() const;
        // Return the number of type selections available for this choice
        // imp.  Note that this is also the size of the types catalog.

    int size() const;
        // Return the number of type selections available for this choice imp.
        // This function is needed for 'bdem_functiontemplates'.

    int selector() const;
        // Return the index of the current selected type in the types catalog
        // of this choice imp, or -1 if no type is currently selected.

    bdem_ConstElemRef selection() const;
        // Return an element reference to the modifiable selection value of
        // this choice imp if '-1 != selector()' and an element reference of
        // type 'bdem_ElemType::BDEM_VOID' otherwise.

    bdem_ElemType::Type selectionType(int index) const;
        // Return the type of the element at the specified 'index' in the
        // types catalog of this choice imp if '-1 != selector()', and
        // 'bdem_ElemType::BDEM_VOID' otherwise.  The behavior is undefined
        // unless '-1 <= index < numSelections()'.

    const void *selectionPointer() const;
        // Return the address of the non-modifiable selection value of this
        // choice imp if '-1 != selector()' and 0 otherwise.

    template <class STREAM>
    STREAM&
    bdexStreamOutImp(
                 STREAM&                                 stream,
                 int                                     version,
                 const bdem_DescriptorStreamOut<STREAM> *strmAttrLookup) const;
        // Stream out the data in this choice imp to the specified 'stream' in
        // the specified 'version' using the specified 'strmAttrLookup'
        // stream out descriptors.  Return a reference to the modifiable
        // 'stream'.  The 'version' is mandatory and specifies the format of
        // the output.  The 'version' is *not* written to the stream.  If
        // 'version' is not supported, 'stream' is left unmodified.  Auxiliary
        // information is provided by 'strmAttrLookup', which maps each known
        // element type to a corresponding function for streaming out that
        // type.
        //
        // The type 'STREAM' must adhere to the 'bdex_OutStream' protocol (see
        // the package-group-level documentation for more information on
        // 'bdex' streaming of container types).

    bsl::ostream& print(bsl::ostream& stream,
                        int           level = 0,
                        int           spacesPerLevel = 4) const;
        // Format this choice imp to the specified output 'stream' at the
        // (absolute value of) the optionally specified indentation 'level' and
        // return a reference to 'stream'.  If 'level' is specified, optionally
        // specify 'spacesPerLevel', the non-negative number of spaces per
        // indentation level for this and all of its nested objects.  Making
        // 'level' negative suppresses indentation for the first line only.
        // Making 'spacesPerLevel' negative suppresses all indentation AND
        // formats the entire output on one line.  Note that if 'stream' is not
        // valid this operation has no effect.
};

// FREE OPERATORS
bool operator==(const bdem_ChoiceImp& lhs, const bdem_ChoiceImp& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' choice imps have the
    // same value and 'false' otherwise.  Two choice imps have the same value
    // if they have identical types catalogs, the same selector and equal
    // selection values.

bool operator!=(const bdem_ChoiceImp& lhs, const bdem_ChoiceImp& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' choice imps do not
    // have the same value and 'false' otherwise.  Two choice imps have
    // differing values if they have non-equal descriptor catalogs, or have
    // different selectors, or unequal selection values.

bsl::ostream& operator<<(bsl::ostream& stream, const bdem_ChoiceImp& rhs);
    // Write the specified 'rhs' choice imp to the specified output 'stream'
    // in some reasonable (human-readable) format and return a modifiable
    // reference to 'stream'.

// ===========================================================================
//                      INLINE FUNCTION DEFINITIONS
// ===========================================================================

                        // --------------------
                        // class bdem_ChoiceImp
                        // --------------------

// CLASS METHODS
template <class STREAM>
inline
void bdem_ChoiceImp::streamInChoice(
                     void                                  *obj,
                     STREAM&                                stream,
                     int                                    version,
                     const bdem_DescriptorStreamIn<STREAM> *strmAttrLookup,
                     const bdem_Descriptor                 *const attrLookup[])
{
    BSLS_ASSERT_SAFE(obj);

    static_cast<bdem_ChoiceImp *>(obj)->bdexStreamInImp(stream,
                                                        version,
                                                        strmAttrLookup,
                                                        attrLookup);
}

template <class STREAM>
inline
void bdem_ChoiceImp::streamOutChoice(
                        const void                             *obj,
                        STREAM&                                 stream,
                        int                                     version,
                        const bdem_DescriptorStreamOut<STREAM> *strmAttrLookup)
{
    BSLS_ASSERT_SAFE(obj);

    static_cast<const bdem_ChoiceImp *>(obj)->bdexStreamOutImp(stream,
                                                               version,
                                                               strmAttrLookup);
}

// MANIPULATORS
inline
void bdem_ChoiceImp::addSelection(const bdem_Descriptor *elemAttr)
{
    BSLS_ASSERT_SAFE(elemAttr);

    const_cast<DescriptorCatalog *>(d_header.catalog())->push_back(elemAttr);
}

inline
bdem_ChoiceHeader& bdem_ChoiceImp::choiceHeader()
{
    return d_header;
}

inline
bdem_ElemRef bdem_ChoiceImp::selection()
{
    return bdem_ElemRef(d_header.selectionPointer(),
                        d_header.selectionDescriptor(selector()),
                        &d_header.flags(),
                        (int)bdem_ChoiceHeader::BDEM_NULLNESS_FLAG);
}

inline
void bdem_ChoiceImp::clear()
{
    reset();
}

inline
void bdem_ChoiceImp::clearNullnessBit()
{
    d_header.clearNullnessBit();
}

inline
void bdem_ChoiceImp::reset()
{
    d_header.reset();

    const_cast<DescriptorCatalog *>(d_header.catalog())->clear();
}

inline
void *bdem_ChoiceImp::selectionPointer()
{
    return d_header.selectionPointer();
}

template <class STREAM>
STREAM&
bdem_ChoiceImp::bdexStreamInImp(
                     STREAM&                                stream,
                     int                                    version,
                     const bdem_DescriptorStreamIn<STREAM> *strmAttrLookup,
                     const bdem_Descriptor                 *const attrLookup[])
{
    switch (version) {  // Switch on the version (starting with 1).
      case 3:                                                   // FALL THROUGH
      case 2:                                                   // FALL THROUGH
      // 'case 2' falls through to bring the choice components to the same
      // version as the list components.
      case 1: {
        int numSelections;
        stream.getLength(numSelections);
        if (!stream) {
            return stream;                                            // RETURN
        }

        reset();

        if (0 == numSelections) {
            return stream;                                            // RETURN
        }

        DescriptorCatalog *catalog =
                           const_cast<DescriptorCatalog *>(d_header.catalog());
        catalog->resize(numSelections);
        for (int i = 0; i < numSelections; ++i) {
            bdem_ElemType::Type type = bdem_ElemType::BDEM_VOID;
            bdem_ElemType::bdexStreamIn(stream, type, 1);
            if (!stream
             || (unsigned)type >= (unsigned)bdem_ElemType::BDEM_NUM_TYPES) {
                stream.invalidate();
                return stream;                                        // RETURN
            }
            (*catalog)[i] = attrLookup[type];
        }
        d_header.bdexStreamIn(stream, version, strmAttrLookup, attrLookup);
      } break;
      default: {
        stream.invalidate();
      }
    }

    return stream;
}

// ACCESSORS
inline
const bdem_ChoiceHeader& bdem_ChoiceImp::choiceHeader() const
{
    return d_header;
}

inline
int bdem_ChoiceImp::numSelections() const
{
    return (int)d_header.catalog()->size();
}

inline
int bdem_ChoiceImp::size() const
{
    return numSelections();
}

inline
int bdem_ChoiceImp::selector() const
{
    return d_header.selector();
}

inline
bdem_ConstElemRef bdem_ChoiceImp::selection() const
{
    return bdem_ConstElemRef(d_header.selectionPointer(),
                             d_header.selectionDescriptor(selector()),
                             &d_header.flags(),
                             (int)bdem_ChoiceHeader::BDEM_NULLNESS_FLAG);
}

inline
const void *bdem_ChoiceImp::selectionPointer() const
{
    return d_header.selectionPointer();
}

inline
bdem_ElemType::Type bdem_ChoiceImp::selectionType(int index) const
{
    BSLS_ASSERT_SAFE(-1 <= index);
    BSLS_ASSERT_SAFE(      index < numSelections());

    return static_cast<bdem_ElemType::Type>(
                              d_header.selectionDescriptor(index)->d_elemEnum);
}

template <class STREAM>
STREAM&
bdem_ChoiceImp::bdexStreamOutImp(
                  STREAM&                                 stream,
                  int                                     version,
                  const bdem_DescriptorStreamOut<STREAM> *strmAttrLookup) const
{
    switch (version) {  // Switch on the version (starting with 1).
      case 3:                                                   // FALL THROUGH
      case 2:                                                   // FALL THROUGH
      // 'case 2' falls through to bring the choice components to the same
      // version as the list components.
      case 1: {
        const DescriptorCatalog *catalog = d_header.catalog();
        const int numSelections = static_cast<int>(catalog->size());
        stream.putLength(numSelections);

        if (0 == numSelections) {
            return stream;                                            // RETURN
        }

        for (int i = 0; i < numSelections; ++i) {
            bdem_ElemType::bdexStreamOut(
                                stream,
                                (bdem_ElemType::Type)(*catalog)[i]->d_elemEnum,
                                1);
        }

        d_header.bdexStreamOut(stream, version, strmAttrLookup);
      } break;
      default: {
        stream.invalidate();
      }
    }

    return stream;
}

// FREE OPERATORS
inline
bool operator==(const bdem_ChoiceImp& lhs, const bdem_ChoiceImp& rhs)
{
    return lhs.d_header == rhs.d_header;
}

inline
bool operator!=(const bdem_ChoiceImp& lhs, const bdem_ChoiceImp& rhs)
{
    return !(lhs == rhs);
}

inline
bsl::ostream& operator<<(bsl::ostream& stream, const bdem_ChoiceImp& rhs)
{
    return rhs.print(stream, 0, -1);
}

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2005
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
