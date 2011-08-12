// bdem_choicearrayimp.h                                              -*-C++-*-
#ifndef INCLUDED_BDEM_CHOICEARRAYIMP
#define INCLUDED_BDEM_CHOICEARRAYIMP

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide implementation of a 'bdem' choice array.
//
//@CLASSES:
// bdem_ChoiceArrayImp: provide implementation of a 'bdem' choice array
//
//@SEE_ALSO: bdem_choice, bdem_choicearray, bdem_choiceheader, bdem_elemref
//
//@AUTHOR: Rohan Bhindwale (rbhindwa), Anthony Comerico (acomeric)
//
//@DESCRIPTION: This component implements a 'bdem' choice array object.
// This component is *used* *by* the 'bdem_choicearray' component.  A choice
// array object can hold an array of choice objects where each choice object
// can represent one element from a list of types.  Note that the list of
// types is the same for all choice objects in a choice array.
//
///Terminology
///-----------
// This section will describe the basic terminology used throughout this
// component.
//
// 1. Types catalog: The types catalog is a set of 'bdem_ElemType's that are
// associated with each choice array imp and specifies the various types that
// the choices in a choice array imp can represent.  At any instant a choice
// can have exactly one (or none) type selected from this list.
//
// 2. Item: Item is a short name for choice array item.  A
// 'bdem_ChoiceArrayImp' is a list of items.
//
// 3. Selector: The index of the currently selected type in the types catalog
// for a particular item.
//
// 4. Selection value: The data value corresponding to the currently selected
// type for an item.
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
// To illustrate, consider an item within a 'bdem_ChoiceArrayImp' having a
// 'BDEM_BOOL' selection.  The selection can be in one of three possible
// states:
//
//: o null with underlying value 'bdetu_Unset<bool>::unsetValue()'
//: o non-null with underlying value 'false'
//: o non-null with underlying value 'true'
//
// The underlying value of a null 'bdem' object is a class invariant.  If an
// object is null, that object *also* has the unset value corresponding to its
// type.
//
// For example, suppose we have an item at index 1 in a 'bdem_ChoiceArrayImp',
// 'myChoiceArrayImp', whose current selection is of type 'BDEM_STRING':
//..
//  myChoiceArrayImp.theModifiableItem(1).theModifiableString() = "HELLO";
//  assert(myChoiceArrayImp.theModifiableItem(1).selection().isNonNull());
//..
// Making the selection null also makes it have the unset value (which, for
// 'BDEM_STRING', is the empty string):
//..
//  myChoiceArrayImp.theModifiableItem(1).selection().makeNull();
//  assert(myChoiceArrayImp.theModifiableItem(1).selection().isNull());
//  assert(
//      myChoiceArrayImp.theModifiableItem(1).selection().theString().empty());
//..
///Choice Array Imp Nullability
///- - - - - - - - - - - - - - -
// Though a choice array imp can contain null elements (each element has
// "nullability information" associated with it), the array imp itself can be
// null only in relation to another object (its parent) in which it is a
// member.  The nullness of an element in the array imp can be queried via a
// 'bdem_ElemRef' returned through 'operator[]' to determine whether its
// contained object is null, e.g., 'array.theItem(0).selection().isNull()'.
//
// Please see the Nullability sections in the component level documentation for
// 'bdem_ChoiceArrayItem' for specific details about assigning
// values to bdem choice objects.
//
///Thread Safety
///-------------
// It is safe to access separate 'bdem_ChoiceArrayImp' objects simultaneously
// in separate threads.  It is not safe to access a single
// 'bdem_ChoiceArrayImp' object simultaneously in separate threads.
//
///Usage
///-----
// The class bdem_ChoiceArrayImp class can be used to store arrays of choice
// data, where the type of the value in each element can be selected from a
// catalog of types held by the object.
//
// Each item in a choice array imp uses the same catalog as the choice array
// imp itself and can specify its chosen selection type and selection value
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
// The example will show how to populate the choice array imp with this data.
// The following function will return a choice array imp representing the
// above data:
//..
//  bdem_ChoiceArrayImp computeResults() {
//..
// First, we create the types catalog:
//..
//      vector<bdem_ElemType::Type> CHOICE_TYPES;
//      CHOICE_TYPES.push_back(bdem_ElemType::BDEM_INT); //result value
//      CHOICE_TYPES.push_back(bdem_ElemType::BDEM_STRING); //error value
//      const int NUM_CHOICES = CHOICE_TYPES.size();
//      enum { RESULT_VAL, ERROR };
//
//..
// We are now ready to construct our ChoiceArrayImp using a pre-specified
// descriptor array and the above specified types catalog.
//..
//      bdem_ChoiceArrayImp result(CHOICE_TYPES,
//                                 NUM_CHOICES,
//                                 DESCRIPTORS,
//                                 bdem_AggregateOption::BDEM_PASS_THROUGH);
//..
// Now insert our sample data:
//..
//  [ 12  "No Data"  23  52  "Dived by zero"]
//..
// into the array imp:
//..
//      const int VAL0 = 12;
//      const int VAL1 = 23;
//      const int VAL2 = 52;
//      bsl::string noData   = "No data.";
//      bsl::string divError = "Divided by zero.";
//..
// The normal usage of 'bdem_ChoiceArray' is create 5 elements, and then
// assign the corresponding items to the desired value.
//..
//      result.insertNullItems(0, 5);
//      result.makeSelection(0, RESULT_VAL).theInt() = VAL0;
//      result.makeSelection(1, ERROR).theString()   = noData;
//      result.makeSelection(2, RESULT_VAL).theInt() = VAL1;
//      result.makeSelection(3, RESULT_VAL).theInt() = VAL2;
//      result.makeSelection(4, ERROR).theString()   = divError;
//..
// We can now return the choice array imp:
//..
//      return result;
//  }
//..

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
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

#ifndef INCLUDED_BDEIMP_BITWISECOPY
#include <bdeimp_bitwisecopy.h>
#endif

#ifndef INCLUDED_BDEU_BITSTRINGUTIL
#include <bdeu_bitstringutil.h>
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

#ifndef INCLUDED_BSLMA_ALLOCATOR
#include <bslma_allocator.h>
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

#ifndef INCLUDED_BSL_CSTRING
#include <bsl_cstring.h>          // for 'bsl::memset'
#endif

namespace BloombergLP {

class bslma_Allocator;

                        // =========================
                        // class bdem_ChoiceArrayImp
                        // =========================

class bdem_ChoiceArrayImp {
    // This class represents a object that can hold an array of choice objects
    // from a catalog of object descriptors ('DescriptorCatalog') implemented
    // in the terms of an array of 'bdem_ChoiceHeader' objects.
    //
    // Additionally, this component provides the externalization functions
    // 'bdexStreamIn' and 'bdexStreamOut'.

    // PRIVATE TYPES
    typedef bdem_ChoiceHeader::DescriptorCatalog DescriptorCatalog;
        // Catalog of object descriptors

    class CatalogContainer {
        // This class does the memory management for the catalog.

        // PRIVATE TYPES
        typedef bdem_ChoiceHeader::DescriptorCatalog DescriptorCatalog;
            // Catalog of object descriptors

        // DATA
        DescriptorCatalog                        *d_catalog_p; // catalog (held
                                                               // not owned)
        bdem_AggregateOption::AllocationStrategy  d_allocationStrategy;

      public:
        // CREATORS
        CatalogContainer(
                 DescriptorCatalog                        *catalog,
                 bdem_AggregateOption::AllocationStrategy  allocationStrategy);
            // Create a catalog container to manage the memory of the specified
            // 'catalog' using the specified 'allocationStrategy'.

        ~CatalogContainer();
            // Destroy this catalog container.

        // MANIPULATORS
        DescriptorCatalog *catalog();
            // Return the address of the modifiable catalog being managed by
            // this catalog container.

        // ACCESSORS
        const DescriptorCatalog *catalog() const;
            // Return the address of the non-modifiable catalog being managed
            // by this catalog container.
    };

    // DATA
    // NOTE: Do *NOT* change the order of the data members.  The implementation
    // relies on them being declared in this order.
    bdem_AllocatorManager          d_allocatorManager;  // allocator manager
    CatalogContainer               d_catalogContainer;  // catalog manager
    bsl::vector<bdem_ChoiceHeader> d_headers;           // choice headers
    bsl::vector<int>               d_nullBits;          // null bits

    // FRIENDS
    friend bool operator==(const bdem_ChoiceArrayImp&,
                           const bdem_ChoiceArrayImp&);

  private:
    // PRIVATE MANIPULATORS
    DescriptorCatalog *catalog();
        // Return the address of the modifiable catalog stored by this choice
        // array imp.

    // PRIVATE ACCESSORS
    const DescriptorCatalog *catalog() const;
        // Return the address of the non-modifiable catalog stored by this
        // choice array imp.

  public:
    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS3(bdem_ChoiceArrayImp,
                                  bslalg_TypeTraitBitwiseMoveable,
                                  bslalg_TypeTraitUsesBslmaAllocator,
                                  bdeu_TypeTraitHasPrintMethod);

    // CLASS DATA
    static const bdem_Descriptor d_choiceArrayAttr;
        // Descriptor for the 'bdem_ChoiceArrayImp' class.  Contains pointers
        // to functions for construction, destruction, assignment, equality
        // test, etc.

    // CLASS METHODS
    template <class STREAM>
    static
    void streamInChoiceArray(
                 void                                  *obj,
                 STREAM&                                stream,
                 int                                    version,
                 const bdem_DescriptorStreamIn<STREAM> *strmAttrLookupTbl,
                 const bdem_Descriptor                 *const attrLookupTbl[]);
        // Call the 'bdexStreamIn' method through a generic interface.  A
        // pointer to an instantiation of this function is stored in the
        // streaming function lookup tables for each stream type.

    template <class STREAM>
    static
    void streamOutChoiceArray(
                    const void                             *obj,
                    STREAM&                                 stream,
                    int                                     version,
                    const bdem_DescriptorStreamOut<STREAM> *strmAttrLookupTbl);
        // Call the 'bdexStreamOut' method through a generic interface.  A
        // pointer to an instantiation of this function is stored in the
        // streaming function lookup tables for each stream type.

    // CREATORS
    explicit bdem_ChoiceArrayImp(
                 bslma_Allocator                          *basicAllocator = 0);
    explicit bdem_ChoiceArrayImp(
                 bdem_AggregateOption::AllocationStrategy  allocationStrategy,
                 bslma_Allocator                          *basicAllocator = 0);
        // Construct a default choice array imp.  Optionally specify an
        // 'allocationStrategy'.  If 'allocationStrategy' is not specified,
        // 'bdem_AggregateOption::BDEM_PASS_THROUGH' is used.  Optionally
        // specify a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.  Note that the choice array imp is in the unset state after
        // construction.

    bdem_ChoiceArrayImp(
                const bdem_ElemType::Type                *selectionTypes,
                int                                       numSelectionTypes,
                const bdem_Descriptor *const             *attrLookupTbl,
                bdem_AggregateOption::AllocationStrategy  allocationStrategy,
                bslma_Allocator                          *basicAllocator = 0);
        // Construct a choice array imp, using the specified
        // 'allocationStrategy', that represents the specified 'selectionTypes'
        // array of types of the specified 'numSelectionTypes' length where the
        // specified 'attrLookupTbl' array stores the descriptors
        // corresponding to 'selectionTypes'.  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.  The behavior is
        // undefined unless 'selectionTypes' is an array of size
        // 'numSelectionTypes', and for '0 <= i < numSelectionTypes',
        // '0 <= selectionTypes[i] < bdem_ElemType::BDEM_NUM_TYPES'.

    bdem_ChoiceArrayImp(const bdem_ChoiceArrayImp&  original,
                        bslma_Allocator            *basicAllocator = 0);
    bdem_ChoiceArrayImp(
                 const bdem_ChoiceArrayImp&                original,
                 bdem_AggregateOption::AllocationStrategy  allocationStrategy,
                 bslma_Allocator                          *basicAllocator = 0);
        // Copy construct a choice array imp using the specified 'original'
        // choice array imp.  Optionally specify an 'allocationStrategy'.  If
        // 'allocationStrategy' is not specified,
        // 'bdem_AggregateOption::BDEM_PASS_THROUGH' is used.  Optionally
        // specify a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.

    ~bdem_ChoiceArrayImp();
        // Destroy this choice array imp.

    // MANIPULATORS
    bdem_ChoiceArrayImp& operator=(const bdem_ChoiceArrayImp& rhs);
        // Assign to this choice array imp the value of the specified 'rhs'
        // choice array imp, and return a reference to this choice array imp.

    bdem_ChoiceHeader& theModifiableItem(int itemIndex);
        // Return a reference to the modifiable choice header corresponding to
        // the item at the specified 'itemIndex' in this choice array imp.  If
        // the referenced item is null, it is made non-null before returning,
        // but its value is not otherwise modified.  The behavior is undefined
        // unless '0 <= itemIndex < length()'.

    void clear();
        // Reset this choice array imp to the default-constructed state.  Note
        // that this method is needed for 'bdem_functiontemplates'.

    void insertNullItems(int dstIndex, int numItems);
        // Insert the specified 'numItems' null items at the specified
        // 'dstIndex' in this choice array imp.  The behavior is undefined
        // unless '0 <= numItems' and '0 <= dstIndex <= length()'.

    void insertItem(int dstIndex, const bdem_ChoiceHeader& src);
        // Insert the specified 'src' choice header at the specified 'dstIndex'
        // in this choice array imp.  The behavior is undefined unless
        // '0 <= dstIndex <= length()' and both this choice array imp and
        // 'src' refer to the same types catalog.

    void insertItem(int                        dstIndex,
                    const bdem_ChoiceArrayImp& srcArray,
                    int                        srcIndex);
        // Insert (a copy of) the choice array item at the specified 'srcIndex'
        // in the specified 'srcArray' at the specified 'dstIndex' in this
        // choice array imp.  The behavior is undefined unless
        // '0 <= dstIndex <= length()', '0 <= srcIndex < srcIndex.length()',
        // and the types catalog of 'srcArray' exactly matches the types
        // catalog of this choice array imp.

    bdem_ElemRef itemElemRef(int itemIndex);
        // Return an element reference to the modifiable item at the specified
        // 'itemIndex' in this choice array imp.  The behavior is undefined
        // unless '0 <= itemIndex < length()'.

    void makeItemsNull(int index, int numItems);
        // Set the specified 'numItems' starting at the specified 'index' to
        // null.  The behavior is undefined unless '0 <= index',
        // '0 <= numItems', and 'index + numItems <= length()'.

    bdem_ElemRef makeSelection(int itemIndex, int selection);
        // Set the selector of the item at the specified 'itemIndex' in this
        // choice array imp to the specified 'selection' and change its
        // selection value to the null value of the type corresponding to
        // 'itemIndex' in the types catalog.  Return an element reference to
        // the modifiable selection value if '-1 != itemIndex' and an element
        // reference to 'VOID' otherwise.  The behavior is undefined unless
        // '0 <= itemIndex < length()' and '-1 <= selection < numSelections()'.
        // Note that if '-1 == selection' the item at 'itemIndex' is set to the
        // null value, and if 'selector() == itemIndex' then it is left
        // unchanged.

    void removeItems(int dstIndex, int numItems);
        // Remove the specified 'numItems' items starting at the specified
        // 'dstIndex' from this choice array imp.  The behavior is undefined
        // unless '0 <= numItems <= dstIndex' and
        // 'dstIndex + numItems <= length()'.

    void removeItem(int dstIndex);
        // Remove the item stored at the specified 'dstIndex' in this choice
        // array imp.  The behavior is undefined unless
        // '0 <= dstIndex < length()'.
    
    void reserveRaw(bsl::size_t numItems);
        // Reserve sufficient memory to satisfy allocation requests for at
        // least the specified 'numItems' without replenishment (i.e., without
        // internal allocation).

    void reset();
        // Reset this choice array imp to the default-constructed state.

    void reset(const bdem_ElemType::Type *selectionTypes,
               int                        numSelections,
               const bdem_Descriptor     *const *attrLookupTbl);
        // Reset this choice array imp to represent the specified
        // 'selectionTypes' array of types of the specified
        // 'numSelectionTypes' length where the specified 'attrLookupTbl'
        // array stores the descriptors corresponding to those types.  The
        // behavior is undefined unless 'selectionTypes' is an array of size
        // 'numSelectionTypes', and for '0 <= i < numSelectionTypes',
        // '0 <= selectionTypes[i] < bdem_ElemType::BDEM_NUM_TYPES'.

    template <class STREAM>
    STREAM&
    bdexStreamIn(STREAM&                                stream,
                 int                                    version,
                 const bdem_DescriptorStreamIn<STREAM> *strmAttrLookupTbl,
                 const bdem_Descriptor                 *const *attrLookupTbl);
        // Read the flags, selection types, selection index, selected object
        // for this choice object from the specified input 'stream' and return
        // a reference to the modifiable 'stream'.  The previous value of
        // '*this' is discarded.  The 'version' is mandatory and indicates the
        // expected input stream format.  'strmAttrLookupTbl' maps each known
        // element type to a corresponding function for streaming in that type.
        // 'attrLookupTbl' maps each known element type to a descriptor (see
        // bdem_descriptor).
        //
        // If 'version' is not supported, 'stream' is marked invalid and
        // choice is left unchanged.  If 'stream' is initially invalid, the
        // choice is left unchanged.  If 'stream' becomes invalid during this
        // operation, 'stream' is marked invalid; choice is valid, but its
        // value is unspecified.  Decoding will fail (incorrect data will be
        // read and/or the stream will become invalid) unless the type of
        // object encoded in 'stream' matches the element type encoded in
        // 'stream' and 'stream's version matches 'version'.
        //
        // Note that the type 'STREAM' must adhere to the protocol specified
        // by 'bdex_InStream' (see the package-group-level documentation for
        // more information on 'bdex' streaming of container types).

    // ACCESSORS
    int numSelections() const;
        // Return the number of selections available for this choice array imp.

    bsl::size_t getCapacityRaw() const;
        // Return the number of items for which memory has already been
        // allocated (whether inserted or not).  Note that
        // 'getCapacityRaw() - length()' represents the number of items that
        // can be inserted with the guarantee of no replenishment (no internal
        // allocation).
    
    int length() const;
        // Return the number of items in this choice array imp.

    const bdem_ChoiceHeader& theItem(int itemIndex) const;
        // Return a reference to the non-modifiable choice header corresponding
        // to the item at the specified 'itemIndex' in this choice array imp.
        // The behavior is undefined unless '0 <= itemIndex < length()'.

    bdem_ConstElemRef itemElemRef(int itemIndex) const;
        // Return an element reference to the non-modifiable item at the
        // specified 'itemIndex' in this choice array imp.  The behavior is
        // undefined unless '0 <= itemIndex < length()'.

    int selector(int itemIndex) const;
        // Return the selector value of the item stored at the specified
        // 'itemIndex' in the types catalog of this choice array imp or -1 if
        // no type is currently selected.  The behavior is undefined unless
        // '0 <= itemIndex < length()'.

    bdem_ElemType::Type selectionType(int selection) const;
        // Return the element type corresponding to the specified 'selection'
        // in the types catalog or 'bdem_ElemType::BDEM_VOID' if 'selection'
        // is -1.  Note that all items in the choice array have the same types
        // catalog.  The behavior is undefined unless
        // '-1 <= selection < numSelections()'.

    template <class STREAM>
    STREAM&
    bdexStreamOut(
              STREAM&                                 stream,
              int                                     version,
              const bdem_DescriptorStreamOut<STREAM> *strmAttrLookupTbl) const;
        // Write the flags, current value's element type (as stored in its
        // value descriptor), the current value, and the client selection ID to
        // the specified output 'stream' and return a reference to the
        // modifiable 'stream'.  The 'version' is mandatory and specifies the
        // format of the output.  The 'version' is *not* written to the stream.
        // If 'version' is not supported, 'stream' is left unmodified.
        // Auxiliary information is provided by 'strmAttrLookupTbl', which
        // maps each known element type to a corresponding function for
        // streaming out that type.
        //
        // The type 'STREAM' must adhere to the 'bdex_OutStream' protocol (see
        // the package-group-level documentation for more information on
        // 'bdex' streaming of container types).

    bsl::ostream& print(bsl::ostream& stream,
                        int           level = 0,
                        int           spacesPerLevel = 4) const;
        // Format this choice object to the specified output 'stream' at the
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
bool operator==(const bdem_ChoiceArrayImp& lhs,
                const bdem_ChoiceArrayImp& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' choice array imps have
    // the same value and 'false' otherwise.  Two choice arrays imps have the
    // same value if they store identical type catalogs, have the same length,
    // and the corresponding items at each index are equal.

inline
bool operator!=(const bdem_ChoiceArrayImp& lhs,
                const bdem_ChoiceArrayImp& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' choice array imps do not
    // have the same value and 'false' otherwise.  Two choice arrays imps do
    // not have the same value if they store different type catalogs, have the
    // different length, or the corresponding items at each index are
    // different.

inline
bsl::ostream& operator<<(bsl::ostream& stream, const bdem_ChoiceArrayImp& rhs);
    // Write the specified 'rhs' choice array imp to the specified output
    // 'stream' in some reasonable (human-readable) format, and return a
    // reference to the modifiable 'stream'.

// ===========================================================================
//                      INLINE FUNCTION DEFINITIONS
// ===========================================================================

                        // -------------------------------------------
                        // class bdem_ChoiceArrayImp::CatalogContainer
                        // -------------------------------------------

// CREATORS
inline
bdem_ChoiceArrayImp::CatalogContainer::CatalogContainer(
                  DescriptorCatalog                        *catalog,
                  bdem_AggregateOption::AllocationStrategy  allocationStrategy)
: d_catalog_p(catalog)
, d_allocationStrategy(allocationStrategy)
{
}

// MANIPULATORS
inline
bdem_ChoiceHeader::DescriptorCatalog *
bdem_ChoiceArrayImp::CatalogContainer::catalog()
{
    return d_catalog_p;
}

// ACCESSORS
inline
const bdem_ChoiceHeader::DescriptorCatalog *
bdem_ChoiceArrayImp::CatalogContainer::catalog() const
{
    return d_catalog_p;
}

                        // -------------------------
                        // class bdem_ChoiceArrayImp
                        // -------------------------

                        // -----------------
                        // Level-0 Functions
                        // -----------------

// ACCESSORS
inline 
bsl::size_t bdem_ChoiceArrayImp::getCapacityRaw() const
{
    return d_headers.capacity();
}

inline
int bdem_ChoiceArrayImp::length() const
{
    return (int)d_headers.size();
}

                        // -------------------
                        // All Other Functions
                        // -------------------

// PRIVATE MANIPULATORS
inline
bdem_ChoiceHeader::DescriptorCatalog *bdem_ChoiceArrayImp::catalog()
{
    return d_catalogContainer.catalog();
}

// PRIVATE ACCESSORS
inline
const bdem_ChoiceHeader::DescriptorCatalog *
bdem_ChoiceArrayImp::catalog() const
{
    return d_catalogContainer.catalog();
}

// CLASS METHODS
template <class STREAM>
inline
void bdem_ChoiceArrayImp::streamInChoiceArray(
                  void                                  *obj,
                  STREAM&                                stream,
                  int                                    version,
                  const bdem_DescriptorStreamIn<STREAM> *strmAttrLookupTbl,
                  const bdem_Descriptor                 *const *attrLookupTbl)
{
    BSLS_ASSERT_SAFE(obj);

    static_cast<bdem_ChoiceArrayImp *>(obj)->bdexStreamIn(stream,
                                                          version,
                                                          strmAttrLookupTbl,
                                                          attrLookupTbl);
}

template <class STREAM>
inline
void bdem_ChoiceArrayImp::streamOutChoiceArray(
                     const void                             *obj,
                     STREAM&                                 stream,
                     int                                     version,
                     const bdem_DescriptorStreamOut<STREAM> *strmAttrLookupTbl)
{
    BSLS_ASSERT_SAFE(obj);

    static_cast<const bdem_ChoiceArrayImp *>(obj)->bdexStreamOut(
                                                            stream,
                                                            version,
                                                            strmAttrLookupTbl);
}

// MANIPULATORS
inline
bdem_ChoiceHeader& bdem_ChoiceArrayImp::theModifiableItem(int itemIndex)
{
    BSLS_ASSERT_SAFE(0 <= itemIndex);
    BSLS_ASSERT_SAFE(     itemIndex < length());

    bdeu_BitstringUtil::set(&d_nullBits.front(), itemIndex, false);
    return d_headers[itemIndex];
}

inline
void bdem_ChoiceArrayImp::clear()
{
    reset();
}

inline
void bdem_ChoiceArrayImp::removeItems(int dstIndex, int numItems)
{
    BSLS_ASSERT_SAFE(0 <= dstIndex);
    BSLS_ASSERT_SAFE(0 <= numItems);
    BSLS_ASSERT_SAFE(dstIndex + numItems <= length());

    d_headers.erase(d_headers.begin() + dstIndex,
                    d_headers.begin() + dstIndex + numItems);
    bdeu_BitstringUtil::removeAndFill0(
                                 &d_nullBits.front(),
                                 static_cast<int>(d_headers.size()) + numItems,
                                 dstIndex,
                                 numItems);
}

inline
void bdem_ChoiceArrayImp::removeItem(int dstIndex)
{
    BSLS_ASSERT_SAFE(0 <= dstIndex);
    BSLS_ASSERT_SAFE(     dstIndex < length());

    d_headers.erase(d_headers.begin() + dstIndex);
    bdeu_BitstringUtil::removeAndFill0(&d_nullBits.front(),
                                       static_cast<int>(d_headers.size()) + 1,
                                       dstIndex,
                                       1);
}

inline
void bdem_ChoiceArrayImp::reset()
{
    d_headers.clear();  // Calls the destructors for individual headers,
                        // which will call the 'reset' method on each of them.
    d_nullBits.resize(1, 0);
    catalog()->clear();
}

template <class STREAM>
STREAM&
bdem_ChoiceArrayImp::bdexStreamIn(
                  STREAM&                                stream,
                  int                                    version,
                  const bdem_DescriptorStreamIn<STREAM> *strmAttrLookupTbl,
                  const bdem_Descriptor                 *const *attrLookupTbl)
{
    enum { BDEM_BITS_PER_INT = 32 };

    switch (version) {  // Switch on the version (starting with 1).
      case 3: {
        int numSelections;
        stream.getLength(numSelections);
        if (!stream) {
            return stream;                                            // RETURN
        }

        reset();

        if (0 == numSelections) {
            return stream;                                            // RETURN
        }

        DescriptorCatalog *catalogPtr = catalog();
        catalogPtr->resize(numSelections);

        for (int i = 0; i < numSelections; ++i) {
            bdem_ElemType::Type elemType = bdem_ElemType::BDEM_VOID;
            bdem_ElemType::bdexStreamIn(stream, elemType, 1);
            if (!stream
             || (unsigned)elemType >= (unsigned)bdem_ElemType::BDEM_NUM_TYPES)
                                                                              {
                stream.invalidate();
                return stream;                                        // RETURN
            }
            (*catalogPtr)[i] = attrLookupTbl[elemType];
        }

        int numItems;
        stream.getLength(numItems);
        if (!stream || 0 == numItems) {
            return stream;                                            // RETURN
        }

        const int arraySize = (numItems + BDEM_BITS_PER_INT - 1)
                                                           / BDEM_BITS_PER_INT;
        d_nullBits.resize(arraySize);
        stream.getArrayInt32(&d_nullBits.front(),
                             static_cast<int>(d_nullBits.size()));
        if (!stream) {
            return stream;                                            // RETURN
        }

        d_headers.resize(numItems,
                         bdem_ChoiceHeader(
                                     catalogPtr,
                                     d_allocatorManager.allocationStrategy()));

        for (int i = 0; i < numItems; ++i) {
            if (!bdeu_BitstringUtil::get(&d_nullBits.front(), i)) {
                d_headers[i].bdexStreamIn(stream,
                                          version,
                                          strmAttrLookupTbl,
                                          attrLookupTbl);
                if (!stream) {
                    return stream;                                    // RETURN
                }
            }
        }
      } break;
      case 2:                                                   // FALL THROUGH
      // case 2 falls through to bring the choice components at same version
      // as the list components.
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

        DescriptorCatalog *catalogPtr = catalog();
        catalogPtr->resize(numSelections);
        for (int i = 0; i < numSelections; ++i) {
            bdem_ElemType::Type elemType = bdem_ElemType::BDEM_VOID;
            bdem_ElemType::bdexStreamIn(stream, elemType, 1);
            if (!stream
             || (unsigned)elemType >= (unsigned)bdem_ElemType::BDEM_NUM_TYPES)
                                                                              {
                stream.invalidate();
                return stream;                                        // RETURN
            }
            (*catalogPtr)[i] = attrLookupTbl[elemType];
        }

        int numItems;
        stream.getLength(numItems);
        if (!stream || 0 == numItems) {
            return stream;                                            // RETURN
        }

        const int arraySize = (numItems + BDEM_BITS_PER_INT - 1)
                                                           / BDEM_BITS_PER_INT;
        d_nullBits.resize(arraySize);
        bsl::memset((void *)&d_nullBits.front(), 0, arraySize * sizeof(int));

        d_headers.resize(numItems,
                         bdem_ChoiceHeader(
                                     catalogPtr,
                                     d_allocatorManager.allocationStrategy()));

        for (int i = 0; i < numItems; ++i) {
            d_headers[i].bdexStreamIn(stream,
                                      version,
                                      strmAttrLookupTbl,
                                      attrLookupTbl);
            if (!stream) {
                return stream;                                        // RETURN
            }
        }
      } break;
      default: {
        stream.invalidate();
      }
    }

    return stream;
}

// ACCESSORS
inline
int bdem_ChoiceArrayImp::numSelections() const
{
    return (int)catalog()->size();
}

inline
const bdem_ChoiceHeader& bdem_ChoiceArrayImp::theItem(int itemIndex) const
{
    BSLS_ASSERT_SAFE(0 <= itemIndex);
    BSLS_ASSERT_SAFE(     itemIndex < length());

    return d_headers[itemIndex];
}

inline
int bdem_ChoiceArrayImp::selector(int itemIndex) const
{
    BSLS_ASSERT_SAFE(0 <= itemIndex);
    BSLS_ASSERT_SAFE(     itemIndex < length());

    return d_headers[itemIndex].selector();
}

inline
bdem_ElemType::Type bdem_ChoiceArrayImp::selectionType(int selection) const
{
    BSLS_ASSERT_SAFE(-1 <= selection);
    BSLS_ASSERT_SAFE(      selection < numSelections());

    const DescriptorCatalog *catalogPtr = catalog();
    return -1 == selection ? bdem_ElemType::BDEM_VOID
                           : static_cast<bdem_ElemType::Type>(
                                         (*catalogPtr)[selection]->d_elemEnum);
}

template <class STREAM>
STREAM&
bdem_ChoiceArrayImp::bdexStreamOut(
               STREAM&                                 stream,
               int                                     version,
               const bdem_DescriptorStreamOut<STREAM> *strmAttrLookupTbl) const
{
    enum { BDEM_BITS_PER_INT = 32 };

    switch (version) {  // Switch on the version (starting with 1).
      case 3: {
        const DescriptorCatalog *catalogPtr = catalog();
        const int numSelections = static_cast<int>(catalogPtr->size());
        stream.putLength(numSelections);

        // Although a choice array with an empty types catalog *can* contain
        // items, it is not terribly useful.  Moreover, streaming version 1 did
        // not attempt to stream out items if '0 == numSelections', so that
        // behavior is preserved for all versions (it is not worth
        // distinguishing version 1 from 2&3 on this corner case).

        if (0 == numSelections) {
            return stream;                                            // RETURN
        }

        for (int i = 0; i < numSelections; ++i) {
            bdem_ElemType::bdexStreamOut(
                            stream,
                            (bdem_ElemType::Type) (*catalogPtr)[i]->d_elemEnum,
                            1);
        }

        int numItems = static_cast<int>(d_headers.size());
        stream.putLength(numItems);
        if (0 == numItems) {
            return stream;                                            // RETURN
        }

        stream.putArrayInt32(&d_nullBits.front(),
                             (numItems + BDEM_BITS_PER_INT - 1)
                                                          / BDEM_BITS_PER_INT);

        // Only stream out non-null items.

        int arrayIndex = 0;
        int itemIndex  = 0;
        while (numItems) {
            unsigned int bit    = 1;
            const    int bitmap = d_nullBits[arrayIndex];
            const    int stride = numItems >= BDEM_BITS_PER_INT
                                                            ? BDEM_BITS_PER_INT
                                                            : numItems;
            for (int i = 0; i < stride; ++i) {
                if (!(bitmap & bit)) {
                    d_headers[itemIndex].bdexStreamOut(stream,
                                                       version,
                                                       strmAttrLookupTbl);
                }
                ++itemIndex;
                bit <<= 1;
            }
            ++arrayIndex;
            numItems -= stride;
        }
      } break;
      case 2:                                                   // FALL THROUGH
      // case 2 falls through to bring the choice components at same version
      // as the list components.
      case 1: {
        const DescriptorCatalog *catalogPtr = catalog();
        const int numSelections = static_cast<int>(catalogPtr->size());
        stream.putLength(numSelections);

        // Although a choice array with an empty types catalog *can* contain
        // items, it is not terribly useful.  Moreover, streaming version 1 did
        // not attempt to stream out items if '0 == numSelections', so that
        // behavior is preserved for all versions (it is not worth
        // distinguishing version 1 from 2&3 on this corner case).

        if (0 == numSelections) {
            return stream;                                            // RETURN
        }

        for (int i = 0; i < numSelections; ++i) {
            bdem_ElemType::bdexStreamOut(
                            stream,
                            (bdem_ElemType::Type) (*catalogPtr)[i]->d_elemEnum,
                            1);
        }

        const int numItems = static_cast<int>(d_headers.size());
        stream.putLength(numItems);

        for (int i = 0; i < numItems; ++i) {
            d_headers[i].bdexStreamOut(stream, version, strmAttrLookupTbl);
        }
      } break;
      default: {
        stream.invalidate();
      }
    }

    return stream;
}

// FREE OPERATORS
inline
bool operator!=(const bdem_ChoiceArrayImp& lhs, const bdem_ChoiceArrayImp& rhs)
{
    return !(lhs == rhs);
}

inline
bsl::ostream& operator<<(bsl::ostream& stream, const bdem_ChoiceArrayImp& rhs)
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
