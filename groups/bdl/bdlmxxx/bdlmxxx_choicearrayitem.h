// bdlmxxx_choicearrayitem.h                                             -*-C++-*-
#ifndef INCLUDED_BDLMXXX_CHOICEARRAYITEM
#define INCLUDED_BDLMXXX_CHOICEARRAYITEM

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a container for an item in a choice array.
//
//@CLASSES:
// bdlmxxx::ChoiceArrayItem: choice container with externally-defined structure
//
//@SEE_ALSO: bdlmxxx_choice, bdlmxxx_choicearray
//
//@AUTHOR: Pablo Halpern (phalpern)
//
//@DESCRIPTION: This component implements a container class,
// 'bdlmxxx::ChoiceArrayItem', capable of holding at most one object of any 'bdem'
// type.  A 'bdlmxxx::ChoiceArrayItem' contains a pointer to an externally-managed
// catalog of type descriptors, along with an index into that catalog (the
// "selector") that specifies which type the 'bdlmxxx::Choice' object is currently
// holding.  The interface to 'bdlmxxx::ChoiceArrayItem' is essentially a subset
// of the interface to 'bdlmxxx::Choice'.  Because the descriptor catalog is
// externally managed, there are no manipulators within 'bdlmxxx::ChoiceArrayItem'
// to change the descriptor catalog (i.e., the "structure" of the choice).  The
// selector and selection value of the choice *may* be changed, however.
// Because the structure is managed externally to this type,
// 'bdlmxxx::ChoiceArrayItem' is ideally suited for use in situations where
// collections of identically-structured choice objects is desired (see
// 'bdlmxxx::ChoiceArray').
//
///Terminology
///-----------
// This section will describe the basic terminology used throughout this
// component.
//
// 1. Types catalog: The types catalog is a set of 'bdlmxxx::ElemType's that are
// associated with each choice array item and specifies the various types that
// the choice array item can represent.  At any instant a choice array item can
// have exactly one (or none) type selected from this list.
//
// 2. Selector: The index of the currently selected type in the types catalog.
//
// 3. Selection value: The data value corresponding to the currently selected
// type.  'bdlmxxx::ChoiceArrayItem' is responsible for the construction,
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
//  scalar (BDEM_INT, etc.)           bdltuxxx::Unset<TYPE>::unsetValue()
//
//  vector (BDEM_INT_ARRAY, etc.)     0 == size()
//..
// To illustrate, consider a 'bdlmxxx::ChoiceArrayItem' with a 'BDEM_BOOL'
// selection.  The selection can be in one of three possible states:
//..
//  * null with underlying value 'bdltuxxx::Unset<bool>::unsetValue()'
//  * non-null with underlying value 'false'
//  * non-null with underlying value 'true'
//..
// The underlying value of a null 'bdem' object is a class invariant.  If an
// object is null, that object *also* has the unset value corresponding to its
// type.
//
// For example, suppose we have a 'bdlmxxx::ChoiceArrayItem', 'myItem', whose
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
// 'bdlmxxx::ElemRef' returned from 'selection' to determine whether its
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
// It is safe to access two 'bdlmxxx::ChoiceArrayItem' objects simultaneously,
// each from a separate thread.  It is not safe to access a single
// 'bdlmxxx::ChoiceArrayItem' object simultaneously from separate threads.
//
///Usage
///-----
// In this example, we will implement a simple choice array class,
// 'my_ChoiceArray' capable of holding any number of homogeneous choice
// objects.  By "homogeneous," we mean that all of the choices have identical
// type catalogs.  The descriptor catalog for 'my_ChoiceArray' must be supplied
// at construction in the form of an array of type descriptors.  The
// implementation of 'my_ChoiceArray' actually holds 'bdlmxxx::ChoiceHeader'
// objects, which are cast to 'bdlmxxx::ChoiceArrayItem' on access:
//..
//  class my_ChoiceArray {
//          bdlmxxx::ChoiceHeader::DescriptorCatalog d_catalog;
//          bsl::vector<bdlmxxx::ChoiceHeader>       d_array;
//      public:
//          // CREATORS
//          my_ChoiceArray(const bdlmxxx::Descriptor *descriptors[],
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
//          bdlmxxx::ChoiceArrayItem& operator[](int index);
//              // Return the item at the specified 'index'.
//
//          // ACCESSORS
//          int length() const;
//              // Return the number of items in the array.
//
//          const bdlmxxx::ChoiceArrayItem& operator[](int index) const;
//              // Return the item at the specified 'index'.
//  };
//..
// The main constructor must create the types catalog, which will be common to
// all items in the array.  The array remains empty:
//..
//  my_ChoiceArray::my_ChoiceArray(const bdlmxxx::Descriptor *descriptors[],
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
// catalog.  We are still treating the items as 'bdlmxxx::ChoiceHeader' objects,
// so we can have access to its constructors:
//..
//  void my_ChoiceArray::appendNullItem() {
//      bdlmxxx::ChoiceHeader newItem(&d_catalog);
//      d_array.push_back(newItem);
//  }
//..
// To access an item in the array, we cast the 'bdlmxxx::ChoiceHeader' to
// a 'bdlmxxx::ChoiceArrayImp' reference:
//..
//  bdlmxxx::ChoiceArrayItem& my_ChoiceArray::operator[](int index) {
//      return reinterpret_cast<bdlmxxx::ChoiceArrayItem&>(d_array[index]);
//  }
//..
// The two accessors are straight-forward:
//..
//  int my_ChoiceArray::length() const {
//      return d_array.size();
//  }
//
//  const bdlmxxx::ChoiceArrayItem& my_ChoiceArray::operator[](int index) const {
//      return reinterpret_cast<const bdlmxxx::ChoiceArrayItem&>(d_array[index]);
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
//          bdlmxxx::ChoiceHeader newItem(&d_catalog);
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
//      static const bdlmxxx::Descriptor *DESCRIPTORS[] = {
//          &bdlmxxx::Properties::s_stringAttr,
//          &bdlmxxx::Properties::s_doubleAttr
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

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

#ifndef INCLUDED_BDLMXXX_CHOICEHEADER
#include <bdlmxxx_choiceheader.h>
#endif

#ifndef INCLUDED_BDLMXXX_ELEMATTRLOOKUP
#include <bdlmxxx_elemattrlookup.h>
#endif

#ifndef INCLUDED_BDLMXXX_ELEMREF
#include <bdlmxxx_elemref.h>
#endif

#ifndef INCLUDED_BDLMXXX_ELEMTYPE
#include <bdlmxxx_elemtype.h>
#endif

#ifndef INCLUDED_BSLMA_ALLOCATOR
#include <bslma_allocator.h>
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


namespace bdlmxxx {class Choice;
class ChoiceArray;
class List;
class Table;

                        // ==========================
                        // class ChoiceArrayItem
                        // ==========================

class ChoiceArrayItem {
    // This class represents a container holding a single value of scalar,
    // array or aggregate type.  The 'ChoiceArrayItem' contains a
    // catalog of type IDs and an index into that catalog ("the selector")
    // that specifies which type the 'ChoiceArrayItem' object is
    // currently holding.  This class has methods for setting and retrieving
    // the selector, as well as accessing and modifying the object currently
    // stored within the choice (the "current selection"), but not for
    // modifying the set of available choices.  When the selector is changed,
    // the current selection is destroyed and a new current selection is
    // constructed.  A selector of '-1' indicates that there is no current
    // selection.  The current selection's nullability can be queried via a
    // 'ElemRef' returned from 'selection'.  Please refer to the
    // 'Nullability' section in the component-level documentation.
    //
    // Because putting a value into a 'ChoiceArrayItem' makes an
    // actual copy of that value, assigning array or aggregate values may be
    // prohibitively expensive in terms of runtime.  Optimum performance may
    // be achieved by building up such (recursively) contained complex values
    // in place.
    //
    // This container is *not* *fully* *value* *semantic*.  It has no public
    // constructors and no assignment operators.  (The only way to create one
    // is by casting the address of a 'ChoiceHeader' object to
    // 'ChoiceArrayItem*'.)  However, it does have 'operator==',
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
    ChoiceHeader d_header;  // choice header

    // FRIENDS
    friend bool operator==(const ChoiceArrayItem&,
                           const ChoiceArrayItem&);

    // NOT IMPLEMENTED
    ChoiceArrayItem();
    ChoiceArrayItem(const ChoiceArrayItem&);
    ~ChoiceArrayItem();
    ChoiceArrayItem& operator=(const ChoiceArrayItem&);

  public:
    // CLASS METHODS
    static int maxSupportedBdexVersion();
        // Return the most current 'bdex' streaming version number supported by
        // this class.  (See the package-group-level documentation for more
        // information on 'bdex' streaming of container types.)

    // MANIPULATORS
    ElemRef makeSelection(int index);
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

    ElemRef selection();
        // Return an element reference to the modifiable current selection of
        // this choice array item if '-1 != selector()', and a void element
        // reference otherwise.

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

    ElemType::Type selectionType() const;
        // Return the type of the current selection of this choice array item
        // if it is currently selected, and 'ElemType::BDEM_VOID'
        // otherwise.

    ElemType::Type selectionType(int index) const;
        // Return the type at the specified 'index' in the types catalog of
        // this choice array item if '-1 != index', and
        // 'ElemType::BDEM_VOID' otherwise.  The behavior is undefined
        // unless '-1 <= index < numSelections()'.

    void selectionTypes(bsl::vector<ElemType::Type> *result) const;
        // Replace the contents of the specified 'result' with the sequence of
        // types in the types catalog of this choice array item.

    ConstElemRef selection() const;
        // Return an element reference to the non-modifiable current selection
        // of this choice array item if '-1 != selector()', and a void element
        // reference otherwise.

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
bool operator==(const ChoiceArrayItem& lhs,
                const ChoiceArrayItem& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' choices array items have
    // the same value, and 'false' otherwise.  Two choices array items have
    // the same value if they have the same types catalog, the same selector,
    // and the same selection value.

bool operator!=(const ChoiceArrayItem& lhs,
                const ChoiceArrayItem& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' choices array items do
    // not have the same value, and 'false' otherwise.  Two choices array items
    // do not have the same value if they have different types catalog, a
    // different selector value, or a different selection value.

bsl::ostream& operator<<(bsl::ostream&               stream,
                         const ChoiceArrayItem& rhs);
    // Format 'rhs' in human-readable form (same format as
    // 'rhs.print(stream, 0, -1)'), and return a modifiable reference to
    // 'stream'.

// ===========================================================================
//                      INLINE FUNCTION DEFINITIONS
// ===========================================================================

                        // --------------------------
                        // class ChoiceArrayItem
                        // --------------------------

// CLASS METHODS
inline
int ChoiceArrayItem::maxSupportedBdexVersion()
{
    return 3;
}

// MANIPULATORS
inline
ElemRef ChoiceArrayItem::makeSelection(int index)
{
    BSLS_ASSERT_SAFE(-1 <= index);
    BSLS_ASSERT_SAFE(      index < d_header.numSelections());

    return ElemRef(d_header.makeSelection(index),
                        d_header.selectionDescriptor(index),
                        &d_header.flags(),
                        (int)ChoiceHeader::BDEM_NULLNESS_FLAG);
}

inline
ElemRef ChoiceArrayItem::selection()
{
    return ElemRef(d_header.selectionPointer(),
                        d_header.selectionDescriptor(selector()),
                        &d_header.flags(),
                        (int)ChoiceHeader::BDEM_NULLNESS_FLAG);
}

inline
bool& ChoiceArrayItem::theModifiableBool()
{
    d_header.clearNullnessBit();
    return *static_cast<bool *>(d_header.selectionPointer());
}

inline
char& ChoiceArrayItem::theModifiableChar()
{
    d_header.clearNullnessBit();
    return *static_cast<char *>(d_header.selectionPointer());
}

inline
short& ChoiceArrayItem::theModifiableShort()
{
    d_header.clearNullnessBit();
    return *static_cast<short *>(d_header.selectionPointer());
}

inline
int& ChoiceArrayItem::theModifiableInt()
{
    d_header.clearNullnessBit();
    return *static_cast<int *>(d_header.selectionPointer());
}

inline
bsls::Types::Int64& ChoiceArrayItem::theModifiableInt64()
{
    d_header.clearNullnessBit();
    return *static_cast<bsls::Types::Int64 *>(d_header.selectionPointer());
}

inline
float& ChoiceArrayItem::theModifiableFloat()
{
    d_header.clearNullnessBit();
    return *static_cast<float *>(d_header.selectionPointer());
}

inline
double& ChoiceArrayItem::theModifiableDouble()
{
    d_header.clearNullnessBit();
    return *static_cast<double *>(d_header.selectionPointer());
}

inline
bsl::string& ChoiceArrayItem::theModifiableString()
{
    d_header.clearNullnessBit();
    return *static_cast<bsl::string *>(d_header.selectionPointer());
}

inline
bdlt::Datetime& ChoiceArrayItem::theModifiableDatetime()
{
    d_header.clearNullnessBit();
    return *static_cast<bdlt::Datetime *>(d_header.selectionPointer());
}

inline
bdlt::DatetimeTz& ChoiceArrayItem::theModifiableDatetimeTz()
{
    d_header.clearNullnessBit();
    return *static_cast<bdlt::DatetimeTz *>(d_header.selectionPointer());
}

inline
bdlt::Date& ChoiceArrayItem::theModifiableDate()
{
    d_header.clearNullnessBit();
    return *static_cast<bdlt::Date *>(d_header.selectionPointer());
}

inline
bdlt::DateTz& ChoiceArrayItem::theModifiableDateTz()
{
    d_header.clearNullnessBit();
    return *static_cast<bdlt::DateTz *>(d_header.selectionPointer());
}

inline
bdlt::Time& ChoiceArrayItem::theModifiableTime()
{
    d_header.clearNullnessBit();
    return *static_cast<bdlt::Time *>(d_header.selectionPointer());
}

inline
bdlt::TimeTz& ChoiceArrayItem::theModifiableTimeTz()
{
    d_header.clearNullnessBit();
    return *static_cast<bdlt::TimeTz *>(d_header.selectionPointer());
}

inline
bsl::vector<bool>& ChoiceArrayItem::theModifiableBoolArray()
{
    d_header.clearNullnessBit();
    return *static_cast<bsl::vector<bool> *>(d_header.selectionPointer());
}

inline
bsl::vector<char>& ChoiceArrayItem::theModifiableCharArray()
{
    d_header.clearNullnessBit();
    return *static_cast<bsl::vector<char> *>(d_header.selectionPointer());
}

inline
bsl::vector<short>& ChoiceArrayItem::theModifiableShortArray()
{
    d_header.clearNullnessBit();
    return *static_cast<bsl::vector<short> *>(d_header.selectionPointer());
}

inline
bsl::vector<int>& ChoiceArrayItem::theModifiableIntArray()
{
    d_header.clearNullnessBit();
    return *static_cast<bsl::vector<int> *>(d_header.selectionPointer());
}

inline
bsl::vector<bsls::Types::Int64>&
ChoiceArrayItem::theModifiableInt64Array()
{
    d_header.clearNullnessBit();
    return *static_cast<bsl::vector<bsls::Types::Int64> *>(
                                                  d_header.selectionPointer());
}

inline
bsl::vector<float>& ChoiceArrayItem::theModifiableFloatArray()
{
    d_header.clearNullnessBit();
    return *static_cast<bsl::vector<float> *>(d_header.selectionPointer());
}

inline
bsl::vector<double>& ChoiceArrayItem::theModifiableDoubleArray()
{
    d_header.clearNullnessBit();
    return *static_cast<bsl::vector<double> *>(d_header.selectionPointer());
}

inline
bsl::vector<bsl::string>& ChoiceArrayItem::theModifiableStringArray()
{
    d_header.clearNullnessBit();
    return *static_cast<bsl::vector<bsl::string> *>(
                                                  d_header.selectionPointer());
}

inline
bsl::vector<bdlt::Datetime>& ChoiceArrayItem::theModifiableDatetimeArray()
{
    d_header.clearNullnessBit();
    return *static_cast<bsl::vector<bdlt::Datetime> *>(
                                                  d_header.selectionPointer());
}

inline
bsl::vector<bdlt::DatetimeTz>&
                           ChoiceArrayItem::theModifiableDatetimeTzArray()
{
    d_header.clearNullnessBit();
    return *static_cast<bsl::vector<bdlt::DatetimeTz> *>(
                                                  d_header.selectionPointer());
}

inline
bsl::vector<bdlt::Date>& ChoiceArrayItem::theModifiableDateArray()
{
    d_header.clearNullnessBit();
    return *static_cast<bsl::vector<bdlt::Date> *>(d_header.selectionPointer());
}

inline
bsl::vector<bdlt::DateTz>& ChoiceArrayItem::theModifiableDateTzArray()
{
    d_header.clearNullnessBit();
    return *static_cast<bsl::vector<bdlt::DateTz> *>(
                                                  d_header.selectionPointer());
}

inline
bsl::vector<bdlt::Time>& ChoiceArrayItem::theModifiableTimeArray()
{
    d_header.clearNullnessBit();
    return *static_cast<bsl::vector<bdlt::Time> *>(d_header.selectionPointer());
}

inline
bsl::vector<bdlt::TimeTz>& ChoiceArrayItem::theModifiableTimeTzArray()
{
    d_header.clearNullnessBit();
    return *static_cast<bsl::vector<bdlt::TimeTz> *>(
                                                  d_header.selectionPointer());
}

inline
List& ChoiceArrayItem::theModifiableList()
{
    d_header.clearNullnessBit();
    return *static_cast<List *>(d_header.selectionPointer());
}

inline
Table& ChoiceArrayItem::theModifiableTable()
{
    d_header.clearNullnessBit();
    return *static_cast<Table *>(d_header.selectionPointer());
}

inline
Choice& ChoiceArrayItem::theModifiableChoice()
{
    d_header.clearNullnessBit();
    return *static_cast<Choice *>(d_header.selectionPointer());
}

inline
ChoiceArray& ChoiceArrayItem::theModifiableChoiceArray()
{
    d_header.clearNullnessBit();
    return *static_cast<ChoiceArray *>(d_header.selectionPointer());
}

inline
void ChoiceArrayItem::reset()
{
    makeSelection(-1);
}

template <class STREAM>
inline
STREAM& ChoiceArrayItem::bdexStreamIn(STREAM& stream, int version)
{
    return d_header.bdexStreamIn(
                            stream,
                            version,
                            ElemStreamInAttrLookup<STREAM>::lookupTable(),
                            ElemAttrLookup::lookupTable());
}

// ACCESSORS
inline
int ChoiceArrayItem::numSelections() const
{
    return d_header.numSelections();
}

inline
int ChoiceArrayItem::selector() const
{
    return d_header.selector();
}

inline
ElemType::Type ChoiceArrayItem::selectionType() const
{
    return static_cast<ElemType::Type>(
                         d_header.selectionDescriptor(selector())->d_elemEnum);
}

inline
ElemType::Type ChoiceArrayItem::selectionType(int index) const
{
    BSLS_ASSERT_SAFE(-1 <= index);
    BSLS_ASSERT_SAFE(      index < numSelections());

    return static_cast<ElemType::Type>(
                              d_header.selectionDescriptor(index)->d_elemEnum);
}

inline
ConstElemRef ChoiceArrayItem::selection() const
{
    return ConstElemRef(d_header.selectionPointer(),
                             d_header.selectionDescriptor(selector()),
                             &d_header.flags(),
                             (int)ChoiceHeader::BDEM_NULLNESS_FLAG);
}

inline
const bool& ChoiceArrayItem::theBool() const
{
    return *static_cast<const bool *>(d_header.selectionPointer());
}

inline
const char& ChoiceArrayItem::theChar() const
{
    return *static_cast<const char *>(d_header.selectionPointer());
}

inline
const short& ChoiceArrayItem::theShort() const
{
    return *static_cast<const short *>(d_header.selectionPointer());
}

inline
const int& ChoiceArrayItem::theInt() const
{
    return *static_cast<const int *>(d_header.selectionPointer());
}

inline
const bsls::Types::Int64& ChoiceArrayItem::theInt64() const
{
    return *static_cast<const bsls::Types::Int64 *>(
                                                  d_header.selectionPointer());
}

inline
const float& ChoiceArrayItem::theFloat() const
{
    return *static_cast<const float *>(d_header.selectionPointer());
}

inline
const double& ChoiceArrayItem::theDouble() const
{
    return *static_cast<const double *>(d_header.selectionPointer());
}

inline
const bsl::string& ChoiceArrayItem::theString() const
{
    return *static_cast<const bsl::string *>(d_header.selectionPointer());
}

inline
const bdlt::Datetime& ChoiceArrayItem::theDatetime() const
{
    return *static_cast<const bdlt::Datetime *>(d_header.selectionPointer());
}

inline
const bdlt::DatetimeTz& ChoiceArrayItem::theDatetimeTz() const
{
    return *static_cast<const bdlt::DatetimeTz *>(d_header.selectionPointer());
}

inline
const bdlt::Date& ChoiceArrayItem::theDate() const
{
    return *static_cast<const bdlt::Date *>(d_header.selectionPointer());
}

inline
const bdlt::DateTz& ChoiceArrayItem::theDateTz() const
{
    return *static_cast<const bdlt::DateTz *>(d_header.selectionPointer());
}

inline
const bdlt::Time& ChoiceArrayItem::theTime() const
{
    return *static_cast<const bdlt::Time *>(d_header.selectionPointer());
}

inline
const bdlt::TimeTz& ChoiceArrayItem::theTimeTz() const
{
    return *static_cast<const bdlt::TimeTz *>(d_header.selectionPointer());
}

inline
const bsl::vector<bool>& ChoiceArrayItem::theBoolArray() const
{
    return *static_cast<const bsl::vector<bool> *>(
                                                  d_header.selectionPointer());
}

inline
const bsl::vector<char>& ChoiceArrayItem::theCharArray() const
{
    return *static_cast<const bsl::vector<char> *>(
                                                  d_header.selectionPointer());
}

inline
const bsl::vector<short>& ChoiceArrayItem::theShortArray() const
{
    return *static_cast<const bsl::vector<short> *>(
                                                  d_header.selectionPointer());
}

inline
const bsl::vector<int>& ChoiceArrayItem::theIntArray() const
{
    return *static_cast<const bsl::vector<int> *>(d_header.selectionPointer());
}

inline
const bsl::vector<bsls::Types::Int64>&
ChoiceArrayItem::theInt64Array() const
{
    return *static_cast<const bsl::vector<bsls::Types::Int64> *>(
                                                  d_header.selectionPointer());
}

inline
const bsl::vector<float>& ChoiceArrayItem::theFloatArray() const
{
    return *static_cast<const bsl::vector<float> *>(
                                                  d_header.selectionPointer());
}

inline
const bsl::vector<double>& ChoiceArrayItem::theDoubleArray() const
{
    return *static_cast<const bsl::vector<double> *>(
                                                  d_header.selectionPointer());
}

inline
const bsl::vector<bsl::string>&
ChoiceArrayItem::theStringArray() const
{
    return *static_cast<const bsl::vector<bsl::string> *>(
                                                  d_header.selectionPointer());
}

inline
const bsl::vector<bdlt::Datetime>&
ChoiceArrayItem::theDatetimeArray() const
{
    return *static_cast<const bsl::vector<bdlt::Datetime> *>(
                                                  d_header.selectionPointer());
}

inline
const bsl::vector<bdlt::DatetimeTz>&
ChoiceArrayItem::theDatetimeTzArray() const
{
    return *static_cast<const bsl::vector<bdlt::DatetimeTz> *>(
                                                  d_header.selectionPointer());
}

inline
const bsl::vector<bdlt::Date>&
ChoiceArrayItem::theDateArray() const
{
    return *static_cast<const bsl::vector<bdlt::Date> *>(
                                                  d_header.selectionPointer());
}

inline
const bsl::vector<bdlt::DateTz>&
ChoiceArrayItem::theDateTzArray() const
{
    return *static_cast<const bsl::vector<bdlt::DateTz> *>(
                                                  d_header.selectionPointer());
}

inline
const bsl::vector<bdlt::Time>&
ChoiceArrayItem::theTimeArray() const
{
    return *static_cast<const bsl::vector<bdlt::Time> *>(
                                                  d_header.selectionPointer());
}

inline
const bsl::vector<bdlt::TimeTz>&
ChoiceArrayItem::theTimeTzArray() const
{
    return *static_cast<const bsl::vector<bdlt::TimeTz> *>(
                                                  d_header.selectionPointer());
}

inline
const List& ChoiceArrayItem::theList() const
{
    return *static_cast<const List *>(d_header.selectionPointer());
}

inline
const Table& ChoiceArrayItem::theTable() const
{
    return *static_cast<const Table *>(d_header.selectionPointer());
}

inline
const Choice& ChoiceArrayItem::theChoice() const
{
    return *static_cast<const Choice *>(d_header.selectionPointer());
}

inline
const ChoiceArray& ChoiceArrayItem::theChoiceArray() const
{
    return *static_cast<const ChoiceArray *>(d_header.selectionPointer());
}

template <class STREAM>
inline
STREAM& ChoiceArrayItem::bdexStreamOut(STREAM& stream, int version) const
{
    return d_header.bdexStreamOut(
                          stream,
                          version,
                          ElemStreamOutAttrLookup<STREAM>::lookupTable());
}
}  // close package namespace

// FREE OPERATORS
inline
bool bdlmxxx::operator==(const ChoiceArrayItem& lhs,
                const ChoiceArrayItem& rhs)
{
    return lhs.d_header == rhs.d_header;
}

inline
bool bdlmxxx::operator!=(const ChoiceArrayItem& lhs,
                const ChoiceArrayItem& rhs)
{
    return !(lhs == rhs);
}

inline
bsl::ostream& bdlmxxx::operator<<(bsl::ostream& stream, const ChoiceArrayItem& rhs)
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
