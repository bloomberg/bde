// bdem_choicearrayitem.h                                             -*-C++-*-
#ifndef INCLUDED_BDEM_CHOICEARRAYITEM
#define INCLUDED_BDEM_CHOICEARRAYITEM

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a container for an item in a choice array.
//
//@CLASSES:
// bdem_ChoiceArrayItem: choice container with externally-defined structure
//
//@SEE_ALSO: bdem_choice, bdem_choicearray
//
//@AUTHOR: Pablo Halpern (phalpern)
//
//@DESCRIPTION: This component implements a container class,
// 'bdem_ChoiceArrayItem', capable of holding at most one object of any 'bdem'
// type.  A 'bdem_ChoiceArrayItem' contains a pointer to an externally-managed
// catalog of type descriptors, along with an index into that catalog (the
// "selector") that specifies which type the 'bdem_Choice' object is currently
// holding.  The interface to 'bdem_ChoiceArrayItem' is essentially a subset
// of the interface to 'bdem_Choice'.  Because the descriptor catalog is
// externally managed, there are no manipulators within 'bdem_ChoiceArrayItem'
// to change the descriptor catalog (i.e., the "structure" of the choice).  The
// selector and selection value of the choice *may* be changed, however.
// Because the structure is managed externally to this type,
// 'bdem_ChoiceArrayItem' is ideally suited for use in situations where
// collections of identically-structured choice objects is desired (see
// 'bdem_ChoiceArray').
//
///Terminology
///-----------
// This section will describe the basic terminology used throughout this
// component.
//
// 1. Types catalog: The types catalog is a set of 'bdem_ElemType's that are
// associated with each choice array item and specifies the various types that
// the choice array item can represent.  At any instant a choice array item can
// have exactly one (or none) type selected from this list.
//
// 2. Selector: The index of the currently selected type in the types catalog.
//
// 3. Selection value: The data value corresponding to the currently selected
// type.  'bdem_ChoiceArrayItem' is responsible for the construction,
// management and destruction of this data value.  If the choice array item is
// unset then the data value is null.  The data value could also have a special
// null value even though the choice array item is set.  This is further
// explained in the following section.
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
// To illustrate, consider a 'bdem_ChoiceArrayItem' with a 'BDEM_BOOL'
// selection.  The selection can be in one of three possible states:
//..
//  * null with underlying value 'bdetu_Unset<bool>::unsetValue()'
//  * non-null with underlying value 'false'
//  * non-null with underlying value 'true'
//..
// The underlying value of a null 'bdem' object is a class invariant.  If an
// object is null, that object *also* has the unset value corresponding to its
// type.
//
// For example, suppose we have a 'bdem_ChoiceArrayItem', 'myItem', whose
// current selection is of type 'BDEM_STRING':
//..
//  myItem.theModifiableString() = "HELLO";
//  assert(myItem.selection().isNonNull());
//..
// Making the selection null also makes it have the unset value (which, for
// 'BDEM_STRING', is the empty string):
//..
//  myItem.selection().makeNull();
//  assert(myItem.selection().isNull());
//  assert(myItem.selection().theString().empty());
//..
///Choice Array Item Nullability
///- - - - - - - - - - - - - - -
// Though a choice array item can be unset (selector of -1 and have an empty
// types catalog) and its current selection can be null, the choice array item
// itself can be null only in relation to the choice array in which it is
// contained.  The nullness of the current selection can be queried via a
// 'bdem_ElemRef' returned from 'selection' to determine whether its
// contained object (the current selection) is null, e.g.,
// 'selection().isNull()'.
//
// The preferred way to assign a non-null value to a choice array item is
// through the 'theModifiableTYPE' methods, e.g., 'theModifiableBool' for
// a choice array item that is selected to be of type 'bool'.  The
// 'theModifiableTYPE' methods have the side-effect of making a null value
// non-null, so the 'const' 'theTYPE' methods should be used instead if the
// value is only being accessed (and not modified).
//
///Thread Safety
///-------------
// It is safe to access two 'bdem_ChoiceArrayItem' objects simultaneously,
// each from a separate thread.  It is not safe to access a single
// 'bdem_ChoiceArrayItem' object simultaneously from separate threads.
//
///Usage
///-----
// In this example, we will implement a simple choice array class,
// 'my_ChoiceArray' capable of holding any number of homogeneous choice
// objects.  By "homogeneous," we mean that all of the choices have identical
// type catalogs.  The descriptor catalog for 'my_ChoiceArray' must be supplied
// at construction in the form of an array of type descriptors.  The
// implementation of 'my_ChoiceArray' actually holds 'bdem_ChoiceHeader'
// objects, which are cast to 'bdem_ChoiceArrayItem' on access:
//..
//  class my_ChoiceArray {
//          bdem_ChoiceHeader::DescriptorCatalog d_catalog;
//          bsl::vector<bdem_ChoiceHeader>       d_array;
//      public:
//          // CREATORS
//          my_ChoiceArray(const bdem_Descriptor *descriptors[],
//                         int                    numDescriptors);
//          my_ChoiceArray(const my_ChoiceArray& other);
//          ~my_ChoiceArray();
//
//          // MANIPULATORS
//          my_ChoiceArray& operator=(const my_ChoiceArray& rhs);
//              // Assign this object the value of rhs.
//
//          void appendNullItem();
//              // Append a null choice item to this array.
//
//          bdem_ChoiceArrayItem& operator[](int index);
//              // Return the item at the specified 'index'.
//
//          // ACCESSORS
//          int length() const;
//              // Return the number of items in the array.
//
//          const bdem_ChoiceArrayItem& operator[](int index) const;
//              // Return the item at the specified 'index'.
//  };
//..
// The main constructor must create the types catalog, which will be common to
// all items in the array.  The array remains empty:
//..
//  my_ChoiceArray::my_ChoiceArray(const bdem_Descriptor *descriptors[],
//                                 int                    numDescriptors)
//      : d_catalog(descriptors, descriptors + numDescriptors)
//  {
//  }
//
//  my_ChoiceArray::~my_ChoiceArray()
//  {
//  }
//..
// Now, when we insert a new item, we must ensure that it uses the common
// catalog.  We are still treating the items as 'bdem_ChoiceHeader' objects,
// so we can have access to its constructors:
//..
//  void my_ChoiceArray::appendNullItem() {
//      bdem_ChoiceHeader newItem(&d_catalog);
//      d_array.push_back(newItem);
//  }
//..
// To access an item in the array, we cast the 'bdem_ChoiceHeader' to
// a 'bdem_ChoiceArrayImp' reference:
//..
//  bdem_ChoiceArrayItem& my_ChoiceArray::operator[](int index) {
//      return reinterpret_cast<bdem_ChoiceArrayItem&>(d_array[index]);
//  }
//..
// The two accessors are straight-forward:
//..
//  int my_ChoiceArray::length() const {
//      return d_array.size();
//  }
//
//  const bdem_ChoiceArrayItem& my_ChoiceArray::operator[](int index) const {
//      return reinterpret_cast<const bdem_ChoiceArrayItem&>(d_array[index]);
//  }
//..
// Finally, we implement the copy constructor and assignment for
// 'my_ChoiceArray'.  This requires some care because if we do a
// straight-forward copy of the 'd_array' vector, the items in the copy will
// end up pointing to the descriptor catalog in the original.  Instead, we
// implement the copy by emptying the destination array, copying the catalog,
// then looping through the choice items and using 'makeSelection' to copy the
// data (but not the catalog pointer) for each item.
//..
//  my_ChoiceArray& my_ChoiceArray::operator=(const my_ChoiceArray& rhs) {
//      if (this != &rhs) {
//          d_array.clear();
//          d_catalog = rhs.d_catalog;
//          bdem_ChoiceHeader newItem(&d_catalog);
//          d_array.resize(rhs.length(), newItem);
//          for (int i = 0; i < d_array.size(); ++i) {
//              d_array[i].makeSelection(rhs.d_array[i].selector(),
//                                       rhs.d_array[i].selectionPointer());
//          }
//      }
//
//      return *this;
//  }
//
//  my_ChoiceArray::my_ChoiceArray(const my_ChoiceArray& other) {
//      // Delegate to the assignment operator
//      this->operator=(other);
//  }
//..
// Our main program exercises the facilities of 'my_ChoiceArray'.  We start by
// creating an array of choices, each of which may contain a double or a
// string.  We then create three array items: two strings and a double.  For
// brevity, the definitions of 'my_StringDescriptor' and 'my_DoubleDescriptor'
// are omitted.  Implementations can be found in 'bdem_attributes.cpp'.
//..
//  int main() {
//
//      static const bdem_Descriptor *DESCRIPTORS[] = {
//          &bdem_Properties::d_stringAttr,
//          &bdem_Properties::d_doubleAttr
//      };
//
//      my_ChoiceArray theArray(DESCRIPTORS, 2);
//      assert(0 == theArray.length());
//
//      theArray.appendNullItem();
//      theArray.appendNullItem();
//      theArray.appendNullItem();
//      assert(3 == theArray.length());
//      assert(-1 == theArray[0].selector());
//      assert(-1 == theArray[1].selector());
//      assert(-1 == theArray[2].selector());
//
//      theArray[0].makeSelection(0).theModifiableString() = "Hello";
//      theArray[1].makeSelection(0).theModifiableString() = "World";
//      theArray[2].makeSelection(1).theModifiableDouble() = 3.2;
//
//      assert(0       == theArray[0].selector());
//      assert("Hello" == theArray[0].theString());
//      assert(0       == theArray[1].selector());
//      assert("World" == theArray[1].theString());
//      assert(1       == theArray[2].selector());
//      assert(3.2     == theArray[2].theDouble());
//
//      // Make a copy
//      my_ChoiceArray theCopy(theArray);
//      assert(3       == theCopy.length());
//      assert(0       == theCopy[0].selector());
//      assert("Hello" == theCopy[0].theString());
//      assert(0       == theCopy[1].selector());
//      assert("World" == theCopy[1].theString());
//      assert(1       == theCopy[2].selector());
//      assert(3.2     == theCopy[2].theDouble());
//
//      return 0;
//  }
//..

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BDEM_CHOICEHEADER
#include <bdem_choiceheader.h>
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

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORMUTIL
#include <bsls_platformutil.h>  // @DEPRECATED
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

class bdem_Choice;
class bdem_ChoiceArray;
class bdem_List;
class bdem_Table;

                        // ==========================
                        // class bdem_ChoiceArrayItem
                        // ==========================

class bdem_ChoiceArrayItem {
    // This class represents a container holding a single value of scalar,
    // array or aggregate type.  The 'bdem_ChoiceArrayItem' contains a
    // catalog of type IDs and an index into that catalog ("the selector")
    // that specifies which type the 'bdem_ChoiceArrayItem' object is
    // currently holding.  This class has methods for setting and retrieving
    // the selector, as well as accessing and modifying the object currently
    // stored within the choice (the "current selection"), but not for
    // modifying the set of available choices.  When the selector is changed,
    // the current selection is destroyed and a new current selection is
    // constructed.  A selector of '-1' indicates that there is no current
    // selection.  The current selection's nullability can be queried via a
    // 'bdem_ElemRef' returned from 'selection'.  Please refer to the
    // 'Nullability' section in the component-level documentation.
    //
    // Because putting a value into a 'bdem_ChoiceArrayItem' makes an
    // actual copy of that value, assigning array or aggregate values may be
    // prohibitively expensive in terms of runtime.  Optimum performance may
    // be achieved by building up such (recursively) contained complex values
    // in place.
    //
    // This container is *not* *fully* *value* *semantic*.  It has no public
    // constructors and no assignment operators.  (The only way to create one
    // is by casting the address of a 'bdem_ChoiceHeader' object to
    // 'bdem_ChoiceArrayItem*'.)  However, it does have 'operator==',
    // which defines the notion of a *value*, as well as 'ostream' printing
    // and 'bdex'-compatible serialization.
    //
    // This container is *exception* *neutral* with no guarantee of rollback:
    // if an exception is thrown during the invocation of a method on a
    // pre-existing object, the container is left in a valid state, but its
    // value is undefined.  In no event is memory leaked.  Finally, *aliasing*
    // (e.g., using all or part of an object as both source and destination)
    // is supported in all cases.

    // DATA
    bdem_ChoiceHeader d_header;  // choice header

    // FRIENDS
    friend bool operator==(const bdem_ChoiceArrayItem&,
                           const bdem_ChoiceArrayItem&);

    // NOT IMPLEMENTED
    bdem_ChoiceArrayItem();
    bdem_ChoiceArrayItem(const bdem_ChoiceArrayItem&);
    ~bdem_ChoiceArrayItem();
    bdem_ChoiceArrayItem& operator=(const bdem_ChoiceArrayItem&);

  public:
    // CLASS METHODS
    static int maxSupportedBdexVersion();
        // Return the most current 'bdex' streaming version number supported by
        // this class.  (See the package-group-level documentation for more
        // information on 'bdex' streaming of container types.)

    // MANIPULATORS
    bdem_ElemRef makeSelection(int index);
        // If the specified 'index' is '-1', destroy the current selection of
        // this choice array item (if any), and set this choice array item to
        // the unselected state.  Otherwise, if 'index == selector()', reset
        // the current selection to null.  Otherwise ('-1 != index' and
        // 'index != selector()'), destroy the current selection (if any), and
        // set the selection of this choice array item to the null value
        // corresponding to the entry in the types catalog given by
        // 'selectionType(index)'.  Return an element reference to the
        // modifiable current selection if '-1 != index', and a void element
        // reference otherwise.  The behavior is undefined unless
        // '-1 <= index < numSelections()'.  Note that after calling this
        // method, 'selector' will return 'index'.

    bdem_ElemRef selection();
        // Return an element reference to the modifiable current selection of
        // this choice array item if '-1 != selector()', and a void element
        // reference otherwise.

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
        // Return a reference to the modifiable selection of this choice array
        // item.  If the selection is null, it is made non-null before
        // returning, but its value is not otherwise modified.  The behavior is
        // undefined unless the type of the current selection is of the type
        // indicated by the name of the method used.  Note that unless there is
        // an intention of modifying the selection, the corresponding 'theTYPE'
        // method should be used instead.  Also note that for a given choice
        // array item 'CAI' and a given 'bdem' 'TYPE', the expression
        // 'CAI.theModifiableTYPE()' has the same side-effect and returns a
        // reference to the same element as
        // 'CAI.selection().theModifiableTYPE()'.

    void reset();
        // Destroy the current selection of this choice array item (if any) and
        // reset the item to the unselected state.

    template <class STREAM>
    STREAM& bdexStreamIn(STREAM& stream, int version);
        // Assign the flags , selector and selection the values read from the
        // specified input 'stream' using the specified 'version' format and
        // return a reference to the modifiable 'stream'.  The types catalog is
        // not changed and must already match the types catalog being read.  If
        // 'stream' is initially invalid, this operation has no effect.  If
        // 'stream' becomes invalid during this operation, this object is
        // valid, but its value is unspecified.  If 'version' is not supported,
        // 'stream' is marked invalid and this object is unaltered.  Note that
        // no version is read from 'stream'.
        //
        // See the 'bdex' package-level documentation for more information on
        // 'bdex' streaming of value-semantic types and containers.

    // ACCESSORS
    int numSelections() const;
        // Return the number of selections available for this choice array
        // item.  Note that the number of selections is the same as the length
        // of the types catalog of this choice array item.

    int selector() const;
        // Return the current selector of this choice array item (i.e., the
        // index of the currently selected type within the types catalog, or -1
        // if this choice array item is unselected).

    bdem_ElemType::Type selectionType() const;
        // Return the type of the current selection of this choice array item
        // if it is currently selected, and 'bdem_ElemType::BDEM_VOID'
        // otherwise.

    bdem_ElemType::Type selectionType(int index) const;
        // Return the type at the specified 'index' in the types catalog of
        // this choice array item if '-1 != index', and
        // 'bdem_ElemType::BDEM_VOID' otherwise.  The behavior is undefined
        // unless '-1 <= index < numSelections()'.

    void selectionTypes(bsl::vector<bdem_ElemType::Type> *result) const;
        // Replace the contents of the specified 'result' with the sequence of
        // types in the types catalog of this choice array item.

    bdem_ConstElemRef selection() const;
        // Return an element reference to the non-modifiable current selection
        // of this choice array item if '-1 != selector()', and a void element
        // reference otherwise.

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
        // Return a reference to the non-modifiable selection of this choice
        // array item.  The nullness of the selection is not affected.  The
        // behavior is undefined unless the type of the current selection is of
        // the type indicated by the name of the method used.  Note that for a
        // given choice array item 'CAI' and a given 'bdem' 'TYPE', the
        // expression 'CAI.theTYPE()' returns a reference to the same element
        // as 'CAI.selection().theTYPE()'.

    template <class STREAM>
    STREAM& bdexStreamOut(STREAM& stream, int version) const;
        // Write the flags , selector, and the current selection value to the
        // specified output 'stream' using the specified 'version' format and
        // return a reference to the modifiable 'stream'.  The 'version' is
        // *not* written to 'stream'.  If 'version' is not supported, 'stream'
        // is unmodified.  Note that only the selector and value and not the
        // types catalog is written to 'stream'.
        //
        // See the 'bdex' package-level documentation for more information on
        // 'bdex' streaming of value-semantic types and containers.

    bsl::ostream& print(bsl::ostream& stream,
                        int           level = 0,
                        int           spacesPerLevel = 4) const;
        // Format this choice object to the specified output 'stream' at the
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
bool operator==(const bdem_ChoiceArrayItem& lhs,
                const bdem_ChoiceArrayItem& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' choices array items have
    // the same value, and 'false' otherwise.  Two choices array items have
    // the same value if they have the same types catalog, the same selector,
    // and the same selection value.

bool operator!=(const bdem_ChoiceArrayItem& lhs,
                const bdem_ChoiceArrayItem& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' choices array items do
    // not have the same value, and 'false' otherwise.  Two choices array items
    // do not have the same value if they have different types catalog, a
    // different selector value, or a different selection value.

bsl::ostream& operator<<(bsl::ostream&               stream,
                         const bdem_ChoiceArrayItem& rhs);
    // Format 'rhs' in human-readable form (same format as
    // 'rhs.print(stream, 0, -1)'), and return a modifiable reference to
    // 'stream'.

// ===========================================================================
//                      INLINE FUNCTION DEFINITIONS
// ===========================================================================

                        // --------------------------
                        // class bdem_ChoiceArrayItem
                        // --------------------------

// CLASS METHODS
inline
int bdem_ChoiceArrayItem::maxSupportedBdexVersion()
{
    return 3;
}

// MANIPULATORS
inline
bdem_ElemRef bdem_ChoiceArrayItem::makeSelection(int index)
{
    BSLS_ASSERT_SAFE(-1 <= index);
    BSLS_ASSERT_SAFE(      index < d_header.numSelections());

    return bdem_ElemRef(d_header.makeSelection(index),
                        d_header.selectionDescriptor(index),
                        &d_header.flags(),
                        (int)bdem_ChoiceHeader::BDEM_NULLNESS_FLAG);
}

inline
bdem_ElemRef bdem_ChoiceArrayItem::selection()
{
    return bdem_ElemRef(d_header.selectionPointer(),
                        d_header.selectionDescriptor(selector()),
                        &d_header.flags(),
                        (int)bdem_ChoiceHeader::BDEM_NULLNESS_FLAG);
}

inline
bool& bdem_ChoiceArrayItem::theModifiableBool()
{
    d_header.clearNullnessBit();
    return *static_cast<bool *>(d_header.selectionPointer());
}

inline
char& bdem_ChoiceArrayItem::theModifiableChar()
{
    d_header.clearNullnessBit();
    return *static_cast<char *>(d_header.selectionPointer());
}

inline
short& bdem_ChoiceArrayItem::theModifiableShort()
{
    d_header.clearNullnessBit();
    return *static_cast<short *>(d_header.selectionPointer());
}

inline
int& bdem_ChoiceArrayItem::theModifiableInt()
{
    d_header.clearNullnessBit();
    return *static_cast<int *>(d_header.selectionPointer());
}

inline
bsls_Types::Int64& bdem_ChoiceArrayItem::theModifiableInt64()
{
    d_header.clearNullnessBit();
    return *static_cast<bsls_Types::Int64 *>(d_header.selectionPointer());
}

inline
float& bdem_ChoiceArrayItem::theModifiableFloat()
{
    d_header.clearNullnessBit();
    return *static_cast<float *>(d_header.selectionPointer());
}

inline
double& bdem_ChoiceArrayItem::theModifiableDouble()
{
    d_header.clearNullnessBit();
    return *static_cast<double *>(d_header.selectionPointer());
}

inline
bsl::string& bdem_ChoiceArrayItem::theModifiableString()
{
    d_header.clearNullnessBit();
    return *static_cast<bsl::string *>(d_header.selectionPointer());
}

inline
bdet_Datetime& bdem_ChoiceArrayItem::theModifiableDatetime()
{
    d_header.clearNullnessBit();
    return *static_cast<bdet_Datetime *>(d_header.selectionPointer());
}

inline
bdet_DatetimeTz& bdem_ChoiceArrayItem::theModifiableDatetimeTz()
{
    d_header.clearNullnessBit();
    return *static_cast<bdet_DatetimeTz *>(d_header.selectionPointer());
}

inline
bdet_Date& bdem_ChoiceArrayItem::theModifiableDate()
{
    d_header.clearNullnessBit();
    return *static_cast<bdet_Date *>(d_header.selectionPointer());
}

inline
bdet_DateTz& bdem_ChoiceArrayItem::theModifiableDateTz()
{
    d_header.clearNullnessBit();
    return *static_cast<bdet_DateTz *>(d_header.selectionPointer());
}

inline
bdet_Time& bdem_ChoiceArrayItem::theModifiableTime()
{
    d_header.clearNullnessBit();
    return *static_cast<bdet_Time *>(d_header.selectionPointer());
}

inline
bdet_TimeTz& bdem_ChoiceArrayItem::theModifiableTimeTz()
{
    d_header.clearNullnessBit();
    return *static_cast<bdet_TimeTz *>(d_header.selectionPointer());
}

inline
bsl::vector<bool>& bdem_ChoiceArrayItem::theModifiableBoolArray()
{
    d_header.clearNullnessBit();
    return *static_cast<bsl::vector<bool> *>(d_header.selectionPointer());
}

inline
bsl::vector<char>& bdem_ChoiceArrayItem::theModifiableCharArray()
{
    d_header.clearNullnessBit();
    return *static_cast<bsl::vector<char> *>(d_header.selectionPointer());
}

inline
bsl::vector<short>& bdem_ChoiceArrayItem::theModifiableShortArray()
{
    d_header.clearNullnessBit();
    return *static_cast<bsl::vector<short> *>(d_header.selectionPointer());
}

inline
bsl::vector<int>& bdem_ChoiceArrayItem::theModifiableIntArray()
{
    d_header.clearNullnessBit();
    return *static_cast<bsl::vector<int> *>(d_header.selectionPointer());
}

inline
bsl::vector<bsls_Types::Int64>& bdem_ChoiceArrayItem::theModifiableInt64Array()
{
    d_header.clearNullnessBit();
    return *static_cast<bsl::vector<bsls_Types::Int64> *>(
                                                  d_header.selectionPointer());
}

inline
bsl::vector<float>& bdem_ChoiceArrayItem::theModifiableFloatArray()
{
    d_header.clearNullnessBit();
    return *static_cast<bsl::vector<float> *>(d_header.selectionPointer());
}

inline
bsl::vector<double>& bdem_ChoiceArrayItem::theModifiableDoubleArray()
{
    d_header.clearNullnessBit();
    return *static_cast<bsl::vector<double> *>(d_header.selectionPointer());
}

inline
bsl::vector<bsl::string>& bdem_ChoiceArrayItem::theModifiableStringArray()
{
    d_header.clearNullnessBit();
    return *static_cast<bsl::vector<bsl::string> *>(
                                                  d_header.selectionPointer());
}

inline
bsl::vector<bdet_Datetime>& bdem_ChoiceArrayItem::theModifiableDatetimeArray()
{
    d_header.clearNullnessBit();
    return *static_cast<bsl::vector<bdet_Datetime> *>(
                                                  d_header.selectionPointer());
}

inline
bsl::vector<bdet_DatetimeTz>&
                           bdem_ChoiceArrayItem::theModifiableDatetimeTzArray()
{
    d_header.clearNullnessBit();
    return *static_cast<bsl::vector<bdet_DatetimeTz> *>(
                                                  d_header.selectionPointer());
}

inline
bsl::vector<bdet_Date>& bdem_ChoiceArrayItem::theModifiableDateArray()
{
    d_header.clearNullnessBit();
    return *static_cast<bsl::vector<bdet_Date> *>(d_header.selectionPointer());
}

inline
bsl::vector<bdet_DateTz>& bdem_ChoiceArrayItem::theModifiableDateTzArray()
{
    d_header.clearNullnessBit();
    return *static_cast<bsl::vector<bdet_DateTz> *>(
                                                  d_header.selectionPointer());
}

inline
bsl::vector<bdet_Time>& bdem_ChoiceArrayItem::theModifiableTimeArray()
{
    d_header.clearNullnessBit();
    return *static_cast<bsl::vector<bdet_Time> *>(d_header.selectionPointer());
}

inline
bsl::vector<bdet_TimeTz>& bdem_ChoiceArrayItem::theModifiableTimeTzArray()
{
    d_header.clearNullnessBit();
    return *static_cast<bsl::vector<bdet_TimeTz> *>(
                                                  d_header.selectionPointer());
}

inline
bdem_List& bdem_ChoiceArrayItem::theModifiableList()
{
    d_header.clearNullnessBit();
    return *static_cast<bdem_List *>(d_header.selectionPointer());
}

inline
bdem_Table& bdem_ChoiceArrayItem::theModifiableTable()
{
    d_header.clearNullnessBit();
    return *static_cast<bdem_Table *>(d_header.selectionPointer());
}

inline
bdem_Choice& bdem_ChoiceArrayItem::theModifiableChoice()
{
    d_header.clearNullnessBit();
    return *static_cast<bdem_Choice *>(d_header.selectionPointer());
}

inline
bdem_ChoiceArray& bdem_ChoiceArrayItem::theModifiableChoiceArray()
{
    d_header.clearNullnessBit();
    return *static_cast<bdem_ChoiceArray *>(d_header.selectionPointer());
}

inline
void bdem_ChoiceArrayItem::reset()
{
    makeSelection(-1);
}

template <class STREAM>
inline
STREAM& bdem_ChoiceArrayItem::bdexStreamIn(STREAM& stream, int version)
{
    return d_header.bdexStreamIn(
                            stream,
                            version,
                            bdem_ElemStreamInAttrLookup<STREAM>::lookupTable(),
                            bdem_ElemAttrLookup::lookupTable());
}

// ACCESSORS
inline
int bdem_ChoiceArrayItem::numSelections() const
{
    return d_header.numSelections();
}

inline
int bdem_ChoiceArrayItem::selector() const
{
    return d_header.selector();
}

inline
bdem_ElemType::Type bdem_ChoiceArrayItem::selectionType() const
{
    return static_cast<bdem_ElemType::Type>(
                         d_header.selectionDescriptor(selector())->d_elemEnum);
}

inline
bdem_ElemType::Type bdem_ChoiceArrayItem::selectionType(int index) const
{
    BSLS_ASSERT_SAFE(-1 <= index);
    BSLS_ASSERT_SAFE(      index < numSelections());

    return static_cast<bdem_ElemType::Type>(
                              d_header.selectionDescriptor(index)->d_elemEnum);
}

inline
bdem_ConstElemRef bdem_ChoiceArrayItem::selection() const
{
    return bdem_ConstElemRef(d_header.selectionPointer(),
                             d_header.selectionDescriptor(selector()),
                             &d_header.flags(),
                             (int)bdem_ChoiceHeader::BDEM_NULLNESS_FLAG);
}

inline
const bool& bdem_ChoiceArrayItem::theBool() const
{
    return *static_cast<const bool *>(d_header.selectionPointer());
}

inline
const char& bdem_ChoiceArrayItem::theChar() const
{
    return *static_cast<const char *>(d_header.selectionPointer());
}

inline
const short& bdem_ChoiceArrayItem::theShort() const
{
    return *static_cast<const short *>(d_header.selectionPointer());
}

inline
const int& bdem_ChoiceArrayItem::theInt() const
{
    return *static_cast<const int *>(d_header.selectionPointer());
}

inline
const bsls_Types::Int64& bdem_ChoiceArrayItem::theInt64() const
{
    return *static_cast<const bsls_Types::Int64 *>(
                                                  d_header.selectionPointer());
}

inline
const float& bdem_ChoiceArrayItem::theFloat() const
{
    return *static_cast<const float *>(d_header.selectionPointer());
}

inline
const double& bdem_ChoiceArrayItem::theDouble() const
{
    return *static_cast<const double *>(d_header.selectionPointer());
}

inline
const bsl::string& bdem_ChoiceArrayItem::theString() const
{
    return *static_cast<const bsl::string *>(d_header.selectionPointer());
}

inline
const bdet_Datetime& bdem_ChoiceArrayItem::theDatetime() const
{
    return *static_cast<const bdet_Datetime *>(d_header.selectionPointer());
}

inline
const bdet_DatetimeTz& bdem_ChoiceArrayItem::theDatetimeTz() const
{
    return *static_cast<const bdet_DatetimeTz *>(d_header.selectionPointer());
}

inline
const bdet_Date& bdem_ChoiceArrayItem::theDate() const
{
    return *static_cast<const bdet_Date *>(d_header.selectionPointer());
}

inline
const bdet_DateTz& bdem_ChoiceArrayItem::theDateTz() const
{
    return *static_cast<const bdet_DateTz *>(d_header.selectionPointer());
}

inline
const bdet_Time& bdem_ChoiceArrayItem::theTime() const
{
    return *static_cast<const bdet_Time *>(d_header.selectionPointer());
}

inline
const bdet_TimeTz& bdem_ChoiceArrayItem::theTimeTz() const
{
    return *static_cast<const bdet_TimeTz *>(d_header.selectionPointer());
}

inline
const bsl::vector<bool>& bdem_ChoiceArrayItem::theBoolArray() const
{
    return *static_cast<const bsl::vector<bool> *>(
                                                  d_header.selectionPointer());
}

inline
const bsl::vector<char>& bdem_ChoiceArrayItem::theCharArray() const
{
    return *static_cast<const bsl::vector<char> *>(
                                                  d_header.selectionPointer());
}

inline
const bsl::vector<short>& bdem_ChoiceArrayItem::theShortArray() const
{
    return *static_cast<const bsl::vector<short> *>(
                                                  d_header.selectionPointer());
}

inline
const bsl::vector<int>& bdem_ChoiceArrayItem::theIntArray() const
{
    return *static_cast<const bsl::vector<int> *>(d_header.selectionPointer());
}

inline
const bsl::vector<bsls_Types::Int64>&
bdem_ChoiceArrayItem::theInt64Array() const
{
    return *static_cast<const bsl::vector<bsls_Types::Int64> *>(
                                                  d_header.selectionPointer());
}

inline
const bsl::vector<float>& bdem_ChoiceArrayItem::theFloatArray() const
{
    return *static_cast<const bsl::vector<float> *>(
                                                  d_header.selectionPointer());
}

inline
const bsl::vector<double>& bdem_ChoiceArrayItem::theDoubleArray() const
{
    return *static_cast<const bsl::vector<double> *>(
                                                  d_header.selectionPointer());
}

inline
const bsl::vector<bsl::string>&
bdem_ChoiceArrayItem::theStringArray() const
{
    return *static_cast<const bsl::vector<bsl::string> *>(
                                                  d_header.selectionPointer());
}

inline
const bsl::vector<bdet_Datetime>&
bdem_ChoiceArrayItem::theDatetimeArray() const
{
    return *static_cast<const bsl::vector<bdet_Datetime> *>(
                                                  d_header.selectionPointer());
}

inline
const bsl::vector<bdet_DatetimeTz>&
bdem_ChoiceArrayItem::theDatetimeTzArray() const
{
    return *static_cast<const bsl::vector<bdet_DatetimeTz> *>(
                                                  d_header.selectionPointer());
}

inline
const bsl::vector<bdet_Date>&
bdem_ChoiceArrayItem::theDateArray() const
{
    return *static_cast<const bsl::vector<bdet_Date> *>(
                                                  d_header.selectionPointer());
}

inline
const bsl::vector<bdet_DateTz>&
bdem_ChoiceArrayItem::theDateTzArray() const
{
    return *static_cast<const bsl::vector<bdet_DateTz> *>(
                                                  d_header.selectionPointer());
}

inline
const bsl::vector<bdet_Time>&
bdem_ChoiceArrayItem::theTimeArray() const
{
    return *static_cast<const bsl::vector<bdet_Time> *>(
                                                  d_header.selectionPointer());
}

inline
const bsl::vector<bdet_TimeTz>&
bdem_ChoiceArrayItem::theTimeTzArray() const
{
    return *static_cast<const bsl::vector<bdet_TimeTz> *>(
                                                  d_header.selectionPointer());
}

inline
const bdem_List& bdem_ChoiceArrayItem::theList() const
{
    return *static_cast<const bdem_List *>(d_header.selectionPointer());
}

inline
const bdem_Table& bdem_ChoiceArrayItem::theTable() const
{
    return *static_cast<const bdem_Table *>(d_header.selectionPointer());
}

inline
const bdem_Choice& bdem_ChoiceArrayItem::theChoice() const
{
    return *static_cast<const bdem_Choice *>(d_header.selectionPointer());
}

inline
const bdem_ChoiceArray& bdem_ChoiceArrayItem::theChoiceArray() const
{
    return *static_cast<const bdem_ChoiceArray *>(d_header.selectionPointer());
}

template <class STREAM>
inline
STREAM& bdem_ChoiceArrayItem::bdexStreamOut(STREAM& stream, int version) const
{
    return d_header.bdexStreamOut(
                          stream,
                          version,
                          bdem_ElemStreamOutAttrLookup<STREAM>::lookupTable());
}

// FREE OPERATORS
inline
bool operator==(const bdem_ChoiceArrayItem& lhs,
                const bdem_ChoiceArrayItem& rhs)
{
    return lhs.d_header == rhs.d_header;
}

inline
bool operator!=(const bdem_ChoiceArrayItem& lhs,
                const bdem_ChoiceArrayItem& rhs)
{
    return !(lhs == rhs);
}

inline
bsl::ostream& operator<<(bsl::ostream& stream, const bdem_ChoiceArrayItem& rhs)
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
