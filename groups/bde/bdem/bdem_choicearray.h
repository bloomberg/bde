// bdem_choicearray.h                                                 -*-C++-*-
#ifndef INCLUDED_BDEM_CHOICEARRAY
#define INCLUDED_BDEM_CHOICEARRAY

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide an array of same-structured choice objects.
//
//@CLASSES:
//  bdem_ChoiceArray: homogeneous sequence of choice values
//
//@SEE_ALSO: bdem_choicearrayitem, bdem_choice, bdem_table, bdem_elemref
//
//@AUTHOR: Pablo Halpern (phalpern), Anthony Comerico (acomeric)
//
//@DESCRIPTION: This component implements a value-semantic container class,
// 'bdem_ChoiceArray', capable of holding zero or more items of type
// 'bdem_ChoiceArrayItem'.  Each array item is capable of holding at most one
// value (the "current selection") of any 'bdem' type.  All of the items in a
// choice array have the same list of available selection types (the "selection
// types catalog"), thus producing a regular structure where each item may have
// a different selection, but all selections are from the same list of valid
// types.
//
// Once a choice array is constructed, the only way to change the number or
// types of the valid selections is to discard all of the items and reset the
// selection types catalog.  This is accomplished using the 'reset' method.
// Assignment will also replace the structure of the assigned-to array with
// that of the source array.  Two arrays compare equal only if they have
// identical structure and equal items.  The choice array structure is
// preserved through 'streamIn' and 'streamOut' operations.
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
// To illustrate, consider an item within a 'bdem_ChoiceArray' having a
// 'BDEM_BOOL' selection.  The selection can be in one of three possible
// states:
//..
//  * null with underlying value 'bdetu_Unset<bool>::unsetValue()'
//  * non-null with underlying value 'false'
//  * non-null with underlying value 'true'
//..
// The underlying value of a null 'bdem' object is a class invariant.  If an
// object is null, that object *also* has the unset value corresponding to its
// type.
//
// For example, suppose we have an item at index 1 in a 'bdem_ChoiceArray',
// 'myChoiceArray', whose current selection is of type 'BDEM_STRING':
//..
//  myChoiceArray.theModifiableItem(1).theModifiableString() = "HELLO";
//  assert(myChoiceArray.theModifiableItem(1).selection().isNonNull());
//..
// Making the selection null also makes it have the unset value (which, for
// 'BDEM_STRING', is the empty string):
//..
//  myChoiceArray.theModifiableItem(1).selection().makeNull();
//  assert(myChoiceArray.theModifiableItem(1).selection().isNull());
//  assert(myChoiceArray.theModifiableItem(1).selection().theString().empty());
//..
///Choice Array Nullability
///- - - - - - - - - - - -
// Though a choice array can contain null elements, the array itself can be
// null only in relation to another object (its parent) in which it is a
// member.  The nullness of an element in the array can be queried via a
// 'bdem_ElemRef' returned through 'operator[]' to determine whether its
// contained object is null, e.g.,
// 'array.theModifiableItem(0).selection().isNull()'.
//
// Please see the Nullability sections in the component level documentation for
// 'bdem_ChoiceArrayItem' for specific details about assigning values to 'bdem'
// choice objects.
//
///Thread Safety
///-------------
// It is safe to access separate 'bdem_ChoiceArray' objects simultaneously in
// separate threads.  It is not safe to access a single 'bdem_ChoiceArray'
// object simultaneously in separate threads.
//
///Usage
///-----
// We provide two examples.  The first one is self-contained and shows basic
// usage.  The second one will show how to populate the 'bdem_ChoiceArray' with
// data, output the array to a data stream, read the value from the stream into
// a new object, and access the values individually.
//
///Example 1
///- - - - -
// In the following example, we create a couple of 'bdem_ChoiceArray' objects,
// add items in two different ways, and print the results.
//..
//  int main()
//  {
//..
// The choices within the arrays have selection ID 0 if holding a 'double'
// result and ID 1 if holding a 'bsl::string' error string.  See 'bdem_Choice'
// for a more complete description of how choices work.
//..
//      static const bdem_ElemType::Type CHOICE_TYPES[] = {
//          bdem_ElemType::BDEM_DOUBLE,
//          bdem_ElemType::BDEM_STRING,
//      };
//      static const int NUM_CHOICES =
//          sizeof(CHOICE_TYPES) / sizeof(CHOICE_TYPE[0]);
//      enum ChoiceIndexes { RESULT_VAL, ERROR_STRING };
//..
// Create an empty choice array, 'a', with the structure described in
// 'CHOICE_TYPES':
//..
//      bdem_ChoiceArray a(CHOICE_TYPES, NUM_CHOICES);
//      assert(0 == a.length());
//      assert(NUM_CHOICES == a.numSelections());
//..
// Append one null item to 'a', then set it to hold a 'double':
//..
//      a.appendNullItems(1);
//      a.theModifiableItem(0).makeSelection(RESULT_VAL).
//                                                 theModifiableDouble() = 1.2;
//      assert(1 == a.length());
//      assert(RESULT_VAL == a.theModifiableItem(0).selector());
//      assert(1.2 == a.theModifiableItem(0).theDouble());
//..
// Create a second choice array, 'b', with the same structure as 'a':
//..
//      bdem_ChoiceArray b(CHOICE_TYPES, NUM_CHOICES);
//..
// Append a copy of 'a.theModifiableItem(0)' to 'b':
//..
//      b.appendItem(a.theModifiableItem(0));
//      assert(1 == b.length());
//      assert(RESULT_VAL == b.theModifiableItem(0).selector());
//      assert(1.2 == b.theModifiableItem(0).theDouble());
//..
// Append a null item to 'b', then set it to hold a 'bsl::string':
//..
//      b.appendNullItems(1);
//      b.theModifiableItem(1).makeSelection(ERROR_STRING).
//                                     theModifiableString() = "Error Message";
//      assert(2 == b.length());
//      assert(RESULT_VAL = b.theModifiableItem(0).selector());
//      assert(1.2 == b.theModifiableItem(0).theDouble());
//      assert(ERROR_STRING == b.theModifiableItem(1).selector());
//      assert("Error Message" == b.theModifiableItem(1).theString());
//..
// Now print the resulting array:
//..
//      b.print(bsl::cout);
//
//      return 0;
//  }
//..
//
///Example 2
///- - - - -
// The class bdem_ChoiceArray class can be used to store arrays of choice data,
// where the type of the value in each element can be selected from a catalog
// of types held by the object.
//
// Each element in a ChoiceArray uses the same catalog as the ChoiceArray
// itself.  The catalog is an array of bdem_ElemTypes specifying the what are
// the valid types for the selections held in the ChoiceArray.  Each element in
// the ChoiceArray can specify its chosen selection type and selection
// independently.
//
// In the following example we use an array of data representing the results of
// some computation we wish to display.
//..
//  [ 12  "No Data"  23  52  "Dived by zero"]
//..
// Here each element is either an integer or a string describing why data is
// not available for that element.
//
// The example will show how to populate the ChoiceArray with this data, output
// the array to a data stream, read the value from the stream into a new
// object, and access the values individually.
//..
//  int main()
//  {
//..
// First, we create the catalog of types we will use.
//..
//      bsl::string noData   = "No data.";
//      bsl::string divError = "Divided by zero.";
//
//      vector<bdem_ElemType::Type> CHOICE_TYPES;
//      CHOICE_TYPES.push_back(bdem_ElemType::BDEM_INT);    //result value
//      CHOICE_TYPES.push_back(bdem_ElemType::BDEM_STRING); //error value
//      const int NUM_CHOICES = CHOICE_TYPES.size();
//      enum { RESULT_VAL, ERROR };
//..
// We are now ready to construct our ChoiceArray.
//..
//      bdem_ChoiceArray choiceArray(CHOICE_TYPES);
//..
// Now insert our sample data:
//..
//  [ 12  "No Data"  23  52  "Dived by zero"]
//..
// into the array:
//..
//      // choiceArray.insertNullItems(0, 5);
//      const int VAL0 = 12;
//      const int VAL1 = 23;
//      const int VAL2 = 52;
//..
// A typical usage of 'bdem_ChoiceArray' is to create, say, 5 elements, and
// then assign their 'bdem_Choice' values to the desired value:
//..
//      choiceArray.appendNullItems(5);
//      choiceArray.theModifiableItem(0).makeSelection(RESULT_VAL).
//                                                   theModifiableInt() = VAL0;
//      choiceArray.theModifiableItem(1).makeSelection(ERROR).
//                                              theModifiableString() = noData;
//      choiceArray.theModifiableItem(2).makeSelection(RESULT_VAL).
//                                                   theModifiableInt() = VAL1;
//      choiceArray.theModifiableItem(3).makeSelection(RESULT_VAL).
//                                                   theModifiableInt() = VAL2;
//      choiceArray.theModifiableItem(4).makeSelection(ERROR).
//                                            theModifiableString() = divError;
//..
// For the purposes of this usage example, we do not use
// 'choiceArray.theModifiableItem(i).makeSelection' directly, as this
// introduces a physical dependency on the 'bdem_choicearrayitem' component
// which is at a higher level of physical dependency.  We circumvent this by
// using the method 'bdem_choiceArray::appendItem(bdem_Choice)' instead.
//..
//      bdem_Choice choice;
//
//      choice.makeSelection(RESULT_VAL).theModifiableInt() = VAL0;
//                                              choiceArray.appendItem(choice);
//      choice.makeSelection(ERROR).theModifiableString()   = noData;
//                                              choiceArray.appendItem(choice);
//      choice.makeSelection(RESULT_VAL).theModifiableInt() = VAL1;
//                                              choiceArray.appendItem(choice);
//      choice.makeSelection(RESULT_VAL).theModifiableInt() = VAL2;
//                                              choiceArray.appendItem(choice);
//      choice.makeSelection(ERROR).theModifiableString()   = divError;
//                                              choiceArray.appendItem(choice);
//..
// We can now output our data onto a 'bdex' stream:
//..
//      bdex_ByteOutStream output;
//      choiceArray.bdexStreamOut(output,1);
//..
// then read the stream into a new choice array:
//..
//      bdem_ChoiceArray choiceArray2;
//      bdex_ByteInStream inStream(output.data(),output.length());
//      choiceArray2.bdexStreamIn(inStream, 1 );
//..
// and verify that nothing has changed:
//..
//      assert(choiceArray == choiceArray2);
//..
// We are now finished with our original choice array.  We can reset it to
// clear its values:
//..
//      choiceArray.removeAll();
//
//      assert(0                  == choiceArray.numSelections());
//      assert(0                  == choiceArray.length());
//      assert(bdem_ChoiceArray() == choiceArray);
//
//      return 0;
// }
//..

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BDEM_AGGREGATEOPTION
#include <bdem_aggregateoption.h>
#endif

#ifndef INCLUDED_BDEM_CHOICEARRAYIMP
#include <bdem_choicearrayimp.h>
#endif

#ifndef INCLUDED_BDEM_CHOICEARRAYITEM
#include <bdem_choicearrayitem.h>
#endif

#ifndef INCLUDED_BDEM_ELEMATTRLOOKUP
#include <bdem_elemattrlookup.h>
#endif

#ifndef INCLUDED_BDEM_ELEMTYPE
#include <bdem_elemtype.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITS
#include <bslalg_typetraits.h>
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

class bdem_Choice;
class bdem_ChoiceHeader;

                        // ======================
                        // class bdem_ChoiceArray
                        // ======================

class bdem_ChoiceArray {
    // This class represents an autonomous or contained, homogeneous, indexable
    // sequence of 'bdem_ChoiceArrayItem' objects, each containing one scalar,
    // array, and/or aggregate value.  The array of 'bdem_ElemType::Type'
    // selections is the same for all items, but the selected type and value
    // may vary from item to item.  For example, if the choice array is created
    // with selections 'bdem_ElemType::BDEM_STRING', 'bdem_ElemType::BDEM_INT',
    // and 'bdem_ElemType::BDEM_INT_ARRAY', then the first item may contain a
    // string with value "hello", the second item may contain an integer with
    // value 2, the third item may contain an integer with value 0, and the
    // fourth item may contain an empty array of integers, but none of the
    // items may contain a value that is not a string, integer, or array of
    // integer.
    //
    // More generally, this container class supports a complete set of *value*
    // *semantic* operations, including copy construction, assignment, equality
    // comparison, 'ostream' printing, and 'bdex'-compatible serialization.  (A
    // precise operational definition of when two objects have the same value
    // can be found in the description of 'operator==' for the class).  This
    // container is *exception* *neutral* with no guarantee of rollback: if an
    // exception is thrown during the invocation of a method on a pre-existing
    // object, the container is left in a valid state, but its value is
    // undefined.  In no event is memory leaked.  Finally, *aliasing* (e.g.,
    // using all or part of an object as both source and destination) is
    // supported in all cases.

    // DATA
    bdem_ChoiceArrayImp d_arrayImp;  // choice array implementation

    // FRIENDS
    friend bool operator==(const bdem_ChoiceArray&, const bdem_ChoiceArray&);

  public:
    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(bdem_ChoiceArray,
                                 bslalg_TypeTraitUsesBslmaAllocator);

    // CLASS METHODS
    static int maxSupportedBdexVersion();
        // Return the most current 'bdex' streaming version number supported by
        // this class.  (See the package-group-level documentation for more
        // information on 'bdex' streaming of container types.)

    // CREATORS
    explicit bdem_ChoiceArray(bslma_Allocator *basicAllocator = 0);
    explicit bdem_ChoiceArray(
                 bdem_AggregateOption::AllocationStrategy  allocMode,
                 bslma_Allocator                          *basicAllocator = 0);
        // Create a choice array with 0 items and having an empty types
        // catalog.  Optionally specify a 'basicAllocator' used to supply
        // memory.  If 'basicAllocator' is 0, the currently installed default
        // allocator is used.  Optionally specify a memory management
        // 'allocMode'.  If 'allocMode' is not specified, then
        // 'BDEM_PASS_THROUGH' is used.  (The meanings of the various
        // 'allocMode' values are described in 'bdem_aggregateoption'.)  Note
        // that the 'reset' method may be used to set the types catalog of a
        // choice array after construction.

    bdem_ChoiceArray(
                 const bdem_ElemType::Type                 typesCatalog[],
                 int                                       typesCatalogLen,
                 bslma_Allocator                          *basicAllocator = 0);
    bdem_ChoiceArray(
                 const bdem_ElemType::Type                 typesCatalog[],
                 int                                       typesCatalogLen,
                 bdem_AggregateOption::AllocationStrategy  allocMode,
                 bslma_Allocator                          *basicAllocator = 0);
        // Create a choice array with 0 items and having a types catalog as
        // indicated by the specified initial 'typesCatalogLen' types in the
        // specified 'typesCatalog' array.  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.  Optionally
        // specify a memory management 'allocMode'.  If 'allocMode' is not
        // specified, then 'BDEM_PASS_THROUGH' is used.  (The meanings of the
        // various 'allocMode' values are described in 'bdem_aggregateoption'.)
        // The behavior is undefined unless '0 <= typesCatalogLen' and
        // 'typesCatalog' has at least 'typesCatalogLen' values.

    bdem_ChoiceArray(
                 const bsl::vector<bdem_ElemType::Type>&   typesCatalog,
                 bslma_Allocator                          *basicAllocator = 0);
    bdem_ChoiceArray(
                 const bsl::vector<bdem_ElemType::Type>&   typesCatalog,
                 bdem_AggregateOption::AllocationStrategy  allocMode,
                 bslma_Allocator                          *basicAllocator = 0);
        // Create a choice array with 0 items and having a types catalog as
        // indicated by the specified 'typesCatalog'.  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.  Optionally
        // specify a memory management 'allocMode'.  If 'allocMode' is not
        // specified, then 'BDEM_PASS_THROUGH' is used.  (The meanings of the
        // various 'allocMode' values are described in 'bdem_aggregateoption'.)

    bdem_ChoiceArray(
                 const bdem_ChoiceArray&                   original,
                 bslma_Allocator                          *basicAllocator = 0);
    bdem_ChoiceArray(
                 const bdem_ChoiceArray&                   original,
                 bdem_AggregateOption::AllocationStrategy  allocMode,
                 bslma_Allocator                          *basicAllocator = 0);
        // Create a choice array having the value of the specified 'original'
        // choice array.  Optionally specify a 'basicAllocator' used to supply
        // memory.  If 'basicAllocator' is 0, the currently installed default
        // allocator is used.  Optionally specify a memory management
        // 'allocMode'.  If 'allocMode' is not specified, then
        // 'BDEM_PASS_THROUGH' is used.  (The meanings of the various
        // 'allocMode' values are described in 'bdem_aggregateoption'.)

    ~bdem_ChoiceArray();
        // Destroy this choice array.  If the choice array was constructed with
        // any memory-management mode other than 'BDEM_PASS_THROUGH', then
        // destructors are not called on contained items, since the memory used
        // by the items will be released efficiently all at once when the
        // allocator is destroyed.

    // MANIPULATORS
    bdem_ChoiceArray& operator=(const bdem_ChoiceArray& rhs);
        // Assign to this choice array the value of the specified 'rhs' choice
        // array, and return a reference to this modifiable choice array.  The
        // original choice array items and types catalog of this choice array
        // are discarded.

    void appendItem(const bdem_ChoiceArrayItem& src);
        // Append (a copy of) the specified 'src' choice array item to this
        // choice array.  The behavior is undefined unless the types catalog of
        // 'src' exactly matches the types catalog of this choice array.  Note
        // that this function is logically equivalent to:
        //..
        //  insertItem(length(), src);
        //..
        // DEPRECATED: Use the following method instead:
        //..
        //  appendItem(const bdem_ChoiceArray& src, int srcIndex);
        //..

    void appendItem(const bdem_Choice& src);
        // Append (a copy of) the specified 'src' choice to this choice array.
        // The behavior is undefined unless the types catalog of 'src' exactly
        // matches the types catalog of this choice array.  Note that this
        // function is logically equivalent to:
        //..
        //  insertItem(length(), src);
        //..

    void appendItem(const bdem_ChoiceArray& srcArray, int srcIndex);
        // Append (a copy of) the choice array item referred to by the
        // specified 'srcIndex' in the specified 'srcArray' to this choice
        // array.  The behavior is undefined unless the types catalog of
        // 'srcArray' exactly matches the types catalog of this choice array.
        // Note that this function is logically equivalent to:
        //..
        //  insertItem(length(), srcArray, srcIndex);
        //..

    void appendNullItems(int numItems);
        // Append the specified 'numItems' unselected (null) choice array items
        // to this choice array.  Note that this function is logically
        // equivalent to:
        //..
        //  insertNullItems(length(), numItems);
        //..

    void insertItem(int dstIndex, const bdem_ChoiceArrayItem& src);
        // Insert (a copy of) the specified 'src' choice array item into this
        // choice array at the specified 'dstIndex'.  The behavior is undefined
        // unless '0 <= dstIndex <= length()', and the types catalog of 'src'
        // exactly matches the types catalog of this choice array.
        //
        // DEPRECATED: Use the following method instead:
        //..
        //  insertItem(int dstIdx, const bdem_ChoiceArray& src, int srcIdx);
        //..

    void insertItem(int dstIndex, const bdem_Choice& src);
        // Insert (a copy of) the specified 'src' choice or choice array item
        // into this choice array at the specified 'dstIndex'.  The behavior is
        // undefined unless '0 <= dstIndex <= length()', and the types catalog
        // of 'src' exactly matches the types catalog of this choice array.

    void insertItem(int                     dstIndex,
                    const bdem_ChoiceArray& srcArray,
                    int                     srcIndex);
        // Insert at the specified 'dstIndex' (a copy of) the choice array item
        // referred to by the specified 'srcIndex' in the specified 'srcArray'
        // to this choice array.  The behavior is undefined unless
        // '0 <= dstIndex <= length()', '0 <= srcIndex < srcArray.length()',
        // and the types catalog of 'srcArray' exactly matches the types
        // catalog of this choice array.

    void insertNullItems(int dstIndex, int numItems);
        // Insert the specified 'numItems' null (unselected) choice array items
        // at the specified 'dstIndex' in this choice array.  The behavior is
        // undefined unless '0 <= dstIndex <= length()' and '0 <= numItems'.

    bdem_ElemRef itemElemRef(int itemIndex);
        // Return an element reference to the modifiable item at the specified
        // 'itemIndex' in this choice array.  The behavior is undefined unless
        // '0 <= itemIndex < length()'.

    void makeItemsNull(int index, int numItems);
        // Set the specified 'numItems' choice array items starting at the
        // specified 'index' to the null (unselected) state.  The behavior is
        // undefined unless '0 <= index', '0 <= numItems', and
        // 'index + numItems <= length()'.

    void reset(const bdem_ElemType::Type typesCatalog[],
               int                       typesCatalogLen);
        // Destroy all choice array items in this choice array, remove them
        // from the array, and reset the types catalog to have the specified
        // initial 'typesCatalogLen' types in the specified 'typesCatalog'
        // array.  The behavior is undefined unless '0 <= typesCatalogLen' and
        // 'typesCatalog' has at least 'typesCatalogLen' values.

    void reserveRaw(bsl::size_t numItems);
        // Reserve sufficient memory to satisfy allocation requests for at
        // least the specified 'numItems' without replenishment (i.e., without
        // internal allocation).

    void reset(const bsl::vector<bdem_ElemType::Type>& typesCatalog);
        // Destroy all choice array items in this choice array, remove them
        // from the array, and reset the types catalog to contain the types in
        // the specified 'typesCatalog'.

    void removeItem(int index);
        // Destroy the choice array item at the specified 'index' in this
        // choice array and remove it from the array.  The behavior is
        // undefined unless '0 <= index < length()'.

    void removeItems(int index, int numItems);
        // Destroy the specified 'numItems' choice array items starting at the
        // specified 'index' in this choice array, and remove the 'numItems'
        // items from the array.  The behavior is undefined unless
        // '0 <= index', '0 <= numItems', and 'index + numItems <= length()'.

    void removeAllItems();
        // Destroy all choice array items in this choice array, and remove them
        // from the array.

    void removeAll();
        // Destroy all choice array items in this choice array, remove them
        // from the array, and reset the types catalog to be empty.

    bdem_ChoiceArrayItem& theModifiableItem(int itemIndex);
        // Return a reference to the modifiable choice array item at the
        // specified 'itemIndex' in this choice array.  If the referenced item
        // is null, it is made non-null before returning, but its value is not
        // otherwise modified.  The behavior is undefined unless
        // '0 <= itemIndex < length()'.

    template <class STREAM>
    STREAM& bdexStreamIn(STREAM& stream, int version);
        // Assign to this object the value read from the specified input
        // 'stream' using the specified 'version' format and return a reference
        // to the modifiable 'stream'.  If 'stream' is initially invalid, this
        // operation has no effect.  If 'stream' becomes invalid during this
        // operation, this object is valid, but its value is undefined.  If
        // 'version' is not supported, 'stream' is marked invalid and this
        // object is unaltered.  Note that no version is read from 'stream'.
        // See the 'bdex' package-level documentation for more information on
        // 'bdex' streaming of value-semantic types and containers.

    // ACCESSORS
    const bdem_ChoiceArrayItem& operator[](int itemIndex) const;
        // Return a reference to the non-modifiable choice array item at the
        // specified 'itemIndex' in this choice array.  The behavior is
        // undefined unless '0 <= itemIndex < length()'.

    bsl::size_t capacityRaw() const;
        // Return the number of items for which memory was previously allocated
        // upon insertion or via a call to 'reserveRaw'.
        // Note that it is always true: 'lenght() <= capacityRaw()'.

    int length() const;
        // Return the number of choice array items in this choice array.

    int numSelections() const;
        // Return the number of selections available for each choice array item
        // in this choice array.  Note that the number of selections is the
        // same as the length of the types catalog of this choice array.

    bdem_ElemType::Type selectionType(int selectionIndex) const;
        // Return the type at the specified 'selectionIndex' in the types
        // catalog of this choice array if '-1 != selectionIndex', and
        // 'bdem_ElemType::BDEM_VOID' otherwise.  The behavior is undefined
        // unless '-1 <= selectionIndex < numSelections()'.

    void selectionTypes(bsl::vector<bdem_ElemType::Type> *result) const;
        // Replace the contents of the specified 'result' with the sequence of
        // types in the types catalog of this choice array.

    bool isEmpty() const;
        // Return 'true' if this choice array contains no choice array items
        // (i.e., '0 == length()'), and 'false' otherwise.

    bool isItemNull(int itemIndex) const;
        // Return 'true' if the choice array item at the specified 'itemIndex'
        // in this choice array is null, and 'false' otherwise.  The behavior
        // is undefined unless '0 <= itemIndex < length()'.

    bool isAnyNull() const;
        // Return 'true' if any choice array item in this choice array is null,
        // and 'false' otherwise.

    bool isAnyNonNull() const;
        // Return 'true' if any choice array item in this choice array is
        // non-null, and 'false' otherwise.

    bdem_ConstElemRef itemElemRef(int itemIndex) const;
        // Return an element reference to the non-modifiable item at the
        // specified 'itemIndex' in this choice array.  The behavior is
        // undefined unless '0 <= itemIndex < length()'.

    const bdem_ChoiceArrayItem& theItem(int itemIndex) const;
        // Return a reference to the non-modifiable choice array item at the
        // specified 'itemIndex' in this choice array.  The behavior is
        // undefined unless '0 <= itemIndex < length()'.

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
        // Format this table object to the specified output 'stream' at the
        // (absolute value of) the optionally specified indentation 'level' and
        // return a reference to 'stream'.  If 'level' is specified, optionally
        // specify 'spacesPerLevel', the number of spaces per indentation level
        // for this and all of its nested objects.  If 'level' is negative,
        // suppress indentation of the first line.  If 'spacesPerLevel' is
        // negative, suppress all indentation AND format the entire output on
        // one line.  If 'stream' is not valid on entry, this operation has no
        // effect.
};

// FREE OPERATORS
inline
bool operator==(const bdem_ChoiceArray& lhs, const bdem_ChoiceArray& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' choice arrays have the
    // same value, and 'false' otherwise.  Two choice arrays have the same
    // value if they have the same number of items and every item from one
    // choice array compares equal to the corresponding item in the other
    // choice array.

inline
bool operator!=(const bdem_ChoiceArray& lhs, const bdem_ChoiceArray& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' choice arrays have
    // different values, and 'false' otherwise.  Two choice arrays have
    // different values if they have different numbers of items or at least one
    // item from one choice compares not equal to the corresponding item in the
    // other choice array.

inline
bsl::ostream& operator<<(bsl::ostream& stream, const bdem_ChoiceArray& rhs);
    // Format 'rhs' in human-readable form and return a modifiable reference to
    // 'stream'.  Note that this produces the same output as:
    //..
    //  rhs.print(stream, 0, -1)
    //..

// ===========================================================================
//                      INLINE FUNCTION DEFINITIONS
// ===========================================================================

                        // ----------------------
                        // class bdem_ChoiceArray
                        // ----------------------

                        // -----------------
                        // Level-0 Functions
                        // -----------------

// ACCESSORS
inline
bsl::size_t bdem_ChoiceArray::capacityRaw() const
{
    return d_arrayImp.capacityRaw();
}

inline
int bdem_ChoiceArray::length() const
{
    return d_arrayImp.length();
}

                        // -------------------
                        // All Other Functions
                        // -------------------

// CLASS METHODS
inline
int bdem_ChoiceArray::maxSupportedBdexVersion()
{
    return 3;
}

// CREATORS
inline
bdem_ChoiceArray::bdem_ChoiceArray(bslma_Allocator *basicAllocator)
: d_arrayImp(basicAllocator)
{
}

inline
bdem_ChoiceArray::bdem_ChoiceArray(
                      bdem_AggregateOption::AllocationStrategy  allocMode,
                      bslma_Allocator                          *basicAllocator)
: d_arrayImp(allocMode, basicAllocator)
{
}

inline
bdem_ChoiceArray::bdem_ChoiceArray(const bdem_ElemType::Type  typesCatalog[],
                                   int                        typesCatalogLen,
                                   bslma_Allocator           *basicAllocator)
: d_arrayImp(typesCatalog,
             typesCatalogLen,
             bdem_ElemAttrLookup::lookupTable(),
             bdem_AggregateOption::BDEM_PASS_THROUGH,
             basicAllocator)
{
    BSLS_ASSERT_SAFE(0 <= typesCatalogLen);
}

inline
bdem_ChoiceArray::bdem_ChoiceArray(
                     const bdem_ElemType::Type                 typesCatalog[],
                     int                                       typesCatalogLen,
                     bdem_AggregateOption::AllocationStrategy  allocMode,
                     bslma_Allocator                          *basicAllocator)
: d_arrayImp(typesCatalog,
             typesCatalogLen,
             bdem_ElemAttrLookup::lookupTable(),
             allocMode,
             basicAllocator)
{
    BSLS_ASSERT_SAFE(0 <= typesCatalogLen);
}

inline
bdem_ChoiceArray::bdem_ChoiceArray(
                       const bsl::vector<bdem_ElemType::Type>&  typesCatalog,
                       bslma_Allocator                         *basicAllocator)
: d_arrayImp(0 == typesCatalog.size() ? 0 : &typesCatalog[0],
             static_cast<int>(typesCatalog.size()),
             bdem_ElemAttrLookup::lookupTable(),
             bdem_AggregateOption::BDEM_PASS_THROUGH,
             basicAllocator)
{
}

inline
bdem_ChoiceArray::bdem_ChoiceArray(
                      const bsl::vector<bdem_ElemType::Type>&   typesCatalog,
                      bdem_AggregateOption::AllocationStrategy  allocMode,
                      bslma_Allocator                          *basicAllocator)
: d_arrayImp(0 == typesCatalog.size() ? 0 : &typesCatalog[0],
             static_cast<int>(typesCatalog.size()),
             bdem_ElemAttrLookup::lookupTable(),
             allocMode,
             basicAllocator)
{
}

inline
bdem_ChoiceArray::bdem_ChoiceArray(const bdem_ChoiceArray&  original,
                                   bslma_Allocator         *basicAllocator)
: d_arrayImp(original.d_arrayImp, basicAllocator)
{
}

inline
bdem_ChoiceArray::bdem_ChoiceArray(
                      const bdem_ChoiceArray&                   original,
                      bdem_AggregateOption::AllocationStrategy  allocMode,
                      bslma_Allocator                          *basicAllocator)
: d_arrayImp(original.d_arrayImp, allocMode, basicAllocator)
{
}

inline
bdem_ChoiceArray::~bdem_ChoiceArray()
{
}

// MANIPULATORS
inline
bdem_ChoiceArray& bdem_ChoiceArray::operator=(const bdem_ChoiceArray& rhs)
{
    if (this != &rhs) {
        d_arrayImp = rhs.d_arrayImp;
    }
    return *this;
}

inline
void bdem_ChoiceArray::appendNullItems(int numItems)
{
    BSLS_ASSERT_SAFE(0 <= numItems);

    d_arrayImp.insertNullItems(length(), numItems);
}

inline
void bdem_ChoiceArray::appendItem(const bdem_ChoiceArrayItem& src)
{
    d_arrayImp.insertItem(length(), (const bdem_ChoiceHeader&) src);
}

inline
void bdem_ChoiceArray::appendItem(const bdem_ChoiceArray& srcArray,
                                  int                     srcIndex)
{
    BSLS_ASSERT_SAFE(0 <= srcIndex);
    BSLS_ASSERT_SAFE(     srcIndex < srcArray.length());

    d_arrayImp.insertItem(length(), srcArray.d_arrayImp, srcIndex);
}

inline
void bdem_ChoiceArray::insertNullItems(int dstIndex, int numItems)
{
    BSLS_ASSERT_SAFE(0 <= dstIndex);
    BSLS_ASSERT_SAFE(     dstIndex <= length());
    BSLS_ASSERT_SAFE(0 <= numItems);

    d_arrayImp.insertNullItems(dstIndex, numItems);
}

inline
void bdem_ChoiceArray::insertItem(int                         dstIndex,
                                  const bdem_ChoiceArrayItem& src)
{
    BSLS_ASSERT_SAFE(0 <= dstIndex);
    BSLS_ASSERT_SAFE(     dstIndex <= length());

    d_arrayImp.insertItem(dstIndex, (const bdem_ChoiceHeader&) src);
}

inline
void bdem_ChoiceArray::insertItem(int                     dstIndex,
                                  const bdem_ChoiceArray& srcArray,
                                  int                     srcIndex)
{
    BSLS_ASSERT_SAFE(0 <= dstIndex);
    BSLS_ASSERT_SAFE(     dstIndex <= length());
    BSLS_ASSERT_SAFE(0 <= srcIndex);
    BSLS_ASSERT_SAFE(     srcIndex < srcArray.length());

    d_arrayImp.insertItem(dstIndex, srcArray.d_arrayImp, srcIndex);
}

inline
bdem_ElemRef bdem_ChoiceArray::itemElemRef(int itemIndex)
{
    BSLS_ASSERT_SAFE(0 <= itemIndex);
    BSLS_ASSERT_SAFE(     itemIndex < length());

    return d_arrayImp.itemElemRef(itemIndex);
}

inline
void bdem_ChoiceArray::makeItemsNull(int index, int numItems)
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(0 <= numItems);
    BSLS_ASSERT_SAFE(index + numItems <= length());

    d_arrayImp.makeItemsNull(index, numItems);
}

inline
void bdem_ChoiceArray::reserveRaw(bsl::size_t numItems)
{
    d_arrayImp.reserveRaw(numItems);
}

inline
void bdem_ChoiceArray::reset(const bdem_ElemType::Type typesCatalog[],
                             int                       typesCatalogLen)
{
    BSLS_ASSERT_SAFE(0 <= typesCatalogLen);

    d_arrayImp.reset(typesCatalog,
                     typesCatalogLen,
                     bdem_ElemAttrLookup::lookupTable());
}

inline
void bdem_ChoiceArray::reset(
                          const bsl::vector<bdem_ElemType::Type>& typesCatalog)
{
    const bdem_ElemType::Type *begin = 0 == typesCatalog.size()
                                       ? 0
                                       : &typesCatalog[0];
    reset(begin, static_cast<int>(typesCatalog.size()));
}

inline
void bdem_ChoiceArray::removeItem(int index)
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(     index < length());

    d_arrayImp.removeItem(index);
}

inline
void bdem_ChoiceArray::removeItems(int index, int numItems)
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(0 <= numItems);
    BSLS_ASSERT_SAFE(index + numItems <= length());

    d_arrayImp.removeItems(index, numItems);
}

inline
void bdem_ChoiceArray::removeAllItems()
{
    d_arrayImp.removeItems(0, length());
}

inline
void bdem_ChoiceArray::removeAll()
{
    removeAllItems();
    reset(0, 0);
}

inline
bdem_ChoiceArrayItem& bdem_ChoiceArray::theModifiableItem(int itemIndex)
{
    BSLS_ASSERT_SAFE(0 <= itemIndex);
    BSLS_ASSERT_SAFE(     itemIndex < length());

    return (bdem_ChoiceArrayItem&) d_arrayImp.theModifiableItem(itemIndex);
}

template <class STREAM>
inline
STREAM& bdem_ChoiceArray::bdexStreamIn(STREAM& stream, int version)
{
    return d_arrayImp.bdexStreamIn(
                            stream,
                            version,
                            bdem_ElemStreamInAttrLookup<STREAM>::lookupTable(),
                            bdem_ElemAttrLookup::lookupTable());
}

// ACCESSORS
inline
const bdem_ChoiceArrayItem& bdem_ChoiceArray::operator[](int itemIndex) const
{
    BSLS_ASSERT_SAFE(0 <= itemIndex);
    BSLS_ASSERT_SAFE(     itemIndex < length());

    return (const bdem_ChoiceArrayItem &) d_arrayImp.theItem(itemIndex);
}

inline
int bdem_ChoiceArray::numSelections() const
{
    return d_arrayImp.numSelections();
}

inline
bdem_ElemType::Type bdem_ChoiceArray::selectionType(int selectionIndex) const
{
    BSLS_ASSERT_SAFE(-1 <= selectionIndex);
    BSLS_ASSERT_SAFE(      selectionIndex < numSelections());

    return d_arrayImp.selectionType(selectionIndex);
}

inline
bool bdem_ChoiceArray::isEmpty() const
{
    return 0 == length();
}

inline
bool bdem_ChoiceArray::isItemNull(int itemIndex) const
{
    BSLS_ASSERT_SAFE(0 <= itemIndex);
    BSLS_ASSERT_SAFE(     itemIndex < length());

    return d_arrayImp.theItem(itemIndex).isSelectionNull();
}

inline
bdem_ConstElemRef bdem_ChoiceArray::itemElemRef(int itemIndex) const
{
    BSLS_ASSERT_SAFE(0 <= itemIndex);
    BSLS_ASSERT_SAFE(     itemIndex < length());

    return d_arrayImp.itemElemRef(itemIndex);
}

inline
const bdem_ChoiceArrayItem& bdem_ChoiceArray::theItem(int itemIndex) const
{
    BSLS_ASSERT_SAFE(0 <= itemIndex);
    BSLS_ASSERT_SAFE(     itemIndex < length());

    return (const bdem_ChoiceArrayItem &) d_arrayImp.theItem(itemIndex);
}

template <class STREAM>
inline
STREAM& bdem_ChoiceArray::bdexStreamOut(STREAM& stream, int version) const
{
    return d_arrayImp.bdexStreamOut(
                          stream,
                          version,
                          bdem_ElemStreamOutAttrLookup<STREAM>::lookupTable());
}

inline
bsl::ostream& bdem_ChoiceArray::print(bsl::ostream& stream,
                                      int           level,
                                      int           spacesPerLevel) const
{
    return d_arrayImp.print(stream, level, spacesPerLevel);
}

// FREE OPERATORS
inline
bool operator==(const bdem_ChoiceArray& lhs, const bdem_ChoiceArray& rhs)
{
    return lhs.d_arrayImp == rhs.d_arrayImp;
}

inline
bool operator!=(const bdem_ChoiceArray& lhs, const bdem_ChoiceArray& rhs)
{
    return !(lhs == rhs);
}

inline
bsl::ostream& operator<<(bsl::ostream& stream, const bdem_ChoiceArray& rhs)
{
    return rhs.print(stream, 0, -1);
}

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2006
//      All Rights Reserved.
//      Property of Bloomberg L.P.  (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
