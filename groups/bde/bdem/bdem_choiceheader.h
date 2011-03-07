// bdem_choiceheader.h                                                -*-C++-*-
#ifndef INCLUDED_BDEM_CHOICEHEADER
#define INCLUDED_BDEM_CHOICEHEADER

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide common implementation machinery for 'bdem' choices.
//
//@CLASSES:
//  bdem_ChoiceHeader: core implementation of 'bdem' choice machinery
//
//@SEE_ALSO:
//  bdem_choiceimp, bdem_choicearrayimp, bdem_choicearrayitem
//
//@AUTHOR: Pablo Halpern (phalpern), Anthony Comerico (acomeric)
//
//@DESCRIPTION: This component defines a class, 'bdem_ChoiceHeader', which
// provides the data and machinery common to 'bdem_ChoiceImp',
// 'bdem_ChoiceArrayImp' and 'bdem_ChoiceArrayItem' (choice objects).  Clients
// should not use this class directly but should use one of the choice classes
// instead.
//
// A choice header object stores a list of 'bdem_Descriptor' values and can
// correspond to at most one descriptor from this list at any time.  In
// addition to storing the index of the descriptor in the list each choice
// header object also stores the data value corresponding to the type of the
// that descriptor.  The choice header object manages the construction,
// management and destruction of its stored value.  'bdem_ChoiceHeader'
// provides functions to change the selected descriptor, thereby also causing
// a change in the stored value.
//
///Terminology
///-----------
// This section will describe the basic terminology used throughout this
// component.
//..
//  Descriptor Catalog - The descriptor catalog is a set of 'bdem_Descriptor'
//  values a choice header object stores.  Each choice header object can
//  represent at most one descriptor value from this list.  Note that the
//  descriptor catalog for a choice header is specified at construction and
//  cannot be changed during the object's lifetime.  Additionally, the choice
//  header object is not responsible for the management of the catalog.  That
//  is done by higher level components.
//
//  Selector - The index of the currently selected descriptor in the descriptor
//  catalog, or -1 if no descriptor is currently selected.
//
//  Selection Value - The data value corresponding to the type of the currently
//  selected descriptor.  'bdem_ChoiceHeader' is responsible for the
//  construction, management and destruction of this data value.
//
//  Null Choice Header - A choice header object is said to be null if it has no
//  descriptor selected, that is, its selector value is -1.  It is also
//  possible that the choice header has a descriptor selected, but the
//  selection value is null.  In this case the selection value corresponds to
//  the null value for the selected type.
//..
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
// To illustrate, consider a 'bdem_ChoiceHeader' with a 'BDEM_BOOL' selection.
// The selection can be in one of three possible states:
//..
//  * null with underlying value 'bdetu_Unset<bool>::unsetValue()'
//  * non-null with underlying value 'false'
//  * non-null with underlying value 'true'
//..
// The underlying value of a null 'bdem' object is a class invariant.  If an
// object is null, that object *also* has the unset value corresponding to its
// type.
//
///Choice Header Nullability
///- - - - - - - - - - - - -
// Though the current selection of a choice header can be be null, the choice
// itself can be null only in relation to another object (its parent) in which
// it is a member.  The nullness of the current selection can be queried via
// the 'isNull' function to determine whether its contained object (the
// current selection) is null.
//
// Note that assigning a value through the pointer returned from
// 'selectionPointer' for a choice having a current selection that is null
// results in undefined behavior.  I.e., if a choice is made null via a
// 'flags', then assigning a value through the pointer returned by
// 'selectionPointer' will result in undefined behavior.  A call to
// 'clearNullnessBit' must be made to force a choice's current selection
// to a non-null state, it is then legal to assign a value through the pointer
// returned from 'selectionPointer'.
//
// Note that a 'bdem_ChoiceHeader' may contain a sub-object of type
// 'bdem_ChoiceHeader', though the top-level 'bdem_ChoiceHeader' does not know
// its own nullness; the 'flags' method will correctly report the nullness of
// the nested choice sub-object.
//
///Thread Safety
///-------------
// It is safe to access separate 'bdem_ChoiceHeader' objects simultaneously in
// separate threads.  It is not safe to access a single 'bdem_ChoiceHeader'
// object simultaneously in separate threads.
//
///Usage
///-----
// The 'divide' function reads two 'double' values from an input stream,
// divides the first by the second, and streams the result to an output
// stream.
//..
//  void divide(bsl::istream& is, bsl::ostream& os) {
//
//      // Read input parameters
//      double dividend, divisor;
//      is >> dividend >> divisor;
//..
// The division normally will result in a double value, but will sometimes
// result in an error string.  The latter case can occur either because the
// input stream is corrupt or because the division itself failed because of
// the divisor was zero.  The result of the division is therefore packaged in
// a 'bdem_ChoiceHeader' which can store either a 'double' or a 'string'.  The
// current selection is indexed by a value of 0 through 2 for the double
// result, string for input error, or string for division error.  Note that
// input errors and division errors are represented by two separate 'STRING'
// items in the array of types, so that the selector will indicate
// which of the two types of error strings is currently being held by the
// choice header.
//..
//      enum { RESULT_VAL, INPUT_ERROR, DIVISION_ERROR };
//
//      bdem_ChoiceHeader::DescriptorCatalog catalog;
//      catalog.push_back(&bdem_Properties::d_doubleAttr);
//      catalog.push_back(&bdem_Properties::d_stringAttr);
//      catalog.push_back(&bdem_Properties::d_stringAttr);
//
//      bdem_ChoiceHeader outMessage(&catalog);
//      assert(-1 == outMessage.selector());
//      assert(3  == outMessage.numSelections());
//
//      if (! is) {
//..
// If the input fails, the choice header is set to 'INPUT_ERROR' and the
// corresponding selection is set to an error string.
//..
//          bsl::string inputErrString = "Failed to read arguments.";
//          outMessage.makeSelection(INPUT_ERROR, &inputErrString);
//          assert(INPUT_ERROR == outMessage.selector());
//          assert(inputErrString ==
//                            *(bsl::string *) outMessage.selectionPointer());
//      }
//      else if (0 == divisor) {
//..
// If the division fails, the header is set to 'DIVISION_ERROR' and the
// corresponding selection is set to an error string.
//..
//          bsl::string errString = "Divided by zero.";
//          outMessage.makeSelection(DIVISION_ERROR, &errString);
//          assert(DIVISION_ERROR == outMessage.selector());
//          assert(errString == *(bsl::string *)outMessage.selectionPointer());
//      }
//      else {
//..
// If there are no errors, compute the quotient and store it as a 'double'
// with selector, 'RESULT_VAL'.
//..
//          const double quotient = dividend / divisor;
//          outMessage.makeSelection(RESULT_VAL, &quotient);
//          assert(RESULT_VAL == outMessage.selector());
//          assert(quotient == *(double *) outMessage.selectionPointer());
//      }
//..
// Once the result is calculated, it is streamed out to be sent back to the
//..
//      os << outMessage;
//  }
//..

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BDEM_AGGREGATEOPTION
#include <bdem_aggregateoption.h>
#endif

#ifndef INCLUDED_BDEM_DESCRIPTOR
#include <bdem_descriptor.h>
#endif

#ifndef INCLUDED_BDEM_ELEMTYPE
#include <bdem_elemtype.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITBITWISEMOVEABLE
#include <bslalg_typetraitbitwisemoveable.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITS
#include <bslalg_typetraits.h>
#endif

#ifndef INCLUDED_BSLS_ALIGNEDBUFFER
#include <bsls_alignedbuffer.h>
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

                        // =======================
                        // class bdem_ChoiceHeader
                        // =======================

class bdem_ChoiceHeader {
    // This class acts as the lowest level container for holding choice
    // objects.  It holds (does not own) a types catalog that stores the
    // various types that the contained choice object can represent, and the
    // value for one of the selected types.  This container is not responsible
    // for the maintenance of the types catalog (that is done at a higher
    // level) but is responsible for the memory management, construction and
    // destruction of the associated value.  Note that although this class
    // provides all the functions required of a value-semantic container, the
    // assignment operator requires that the types catalog is managed at a
    // higher level.
    //
    // Additionally, this component provides the externalization functions
    // 'bdexStreamIn' and 'bdexStreamOut'.  A 'bdem_ChoiceHeader' can thus be
    // streamed to an older program provided the features being used are
    // compatible with that version of the object.

  public:
    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(bdem_ChoiceHeader,
                                 bslalg_TypeTraitBitwiseMoveable);

    // PUBLIC TYPES
    typedef bsl::vector<const bdem_Descriptor *> DescriptorCatalog;

    // PUBLIC CONSTANTS
    enum {
        // Enumerate valid indices into the 'bdem_ChoiceHeader' flags.

        BDEM_NULLNESS_FLAG = 0  // offset of nullness flag
    };

  private:
    // DATA
    const DescriptorCatalog                  *d_catalog_p;  // held, not owned
    int                                       d_selector;   // selector index
    int                                       d_flags;      // null bit and
                                                            // future use.
    bdem_AggregateOption::AllocationStrategy  d_allocMode;  // alloc mode

    union {
        // This anonymous 'union' stores the data value corresponding to this
        // choice header object.  'd_selectionBuf' is an aligned buffer large
        // enough to hold *most* 'bdem' types in-place.  The exceptions are
        // 'bdem_Choice', 'bdem_ChoiceArray', and 'bdem_Table', which are
        // allocated out-of-place and addressed by 'd_selection_p'.

        bsls_AlignedBuffer<8 * sizeof(void *)> d_selectionBuf;

        void *d_selection_p;  // allocated object if 'd_selectionBuf' too small
    };

    // PRIVATE MANIPULATORS
    template <class STREAM>
    void streamInSelection(
                    STREAM&                                stream,
                    int                                    version,
                    const bdem_DescriptorStreamIn<STREAM> *strmAttrLookup,
                    const bdem_Descriptor                 *const attrLookup[]);
        // Stream in the selection into this choice header from the specified
        // input 'stream', and return a reference to the modifiable 'stream'.
        // The previous value of '*this' is discarded.  The 'version' is
        // mandatory and indicates the expected input stream format.
        // 'strmAttrLookup' maps each known element type to a corresponding
        // function for streaming in that type.  'attrLookup' maps each known
        // element type to a descriptor (see bdem_descriptor).
        //
        // If 'version' is not supported, 'stream' is marked invalid and this
        // choice header is left unchanged.  If 'stream' is initially invalid,
        // this choice header is left unchanged.  If 'stream' becomes invalid
        // during this operation, 'stream' is marked invalid, the selector is
        // not changed, and the choice header is valid, but its value is
        // unspecified.  Decoding will fail (incorrect data will be read
        // and/or the stream will become invalid) unless the types of choice
        // headers encoded in 'stream' match the sequence of element types in
        // the choice header's types catalog and 'stream's version matches
        // 'version'.

    // PRIVATE ACCESSORS
    template <class STREAM>
    void streamOutSelection(
                 STREAM&                                 stream,
                 int                                     version,
                 const bdem_DescriptorStreamOut<STREAM> *strmAttrLookup) const;
        // Stream out the selection stored by this choice header to the
        // specified output 'stream' and return a reference to the modifiable
        // 'stream'.  The 'version' is mandatory and specifies the format of
        // the output.  The 'version' is *not* written to the stream.  If
        // 'version' is not supported, 'stream' is left unmodified.  Auxiliary
        // information is provided by 'strmAttrLookup', which maps each known
        // element type to a corresponding function for streaming in that type.

  public:
    // PUBLIC CLASS CONSTANTS
    static const bdem_Descriptor d_choiceItemAttr;
        // Attributes of the 'bdem_ChoiceHeader' class.  Contains null pointers
        // for construction, destruction, and assignment functions, but has
        // functionality for the equality test, printing, etc.

    // CREATORS
    bdem_ChoiceHeader(const DescriptorCatalog                  *catalogPtr,
                      bdem_AggregateOption::AllocationStrategy  allocMode
                                    = bdem_AggregateOption::BDEM_PASS_THROUGH);
        // Construct this choice header from the specified 'catalogPtr'.
        // Optionally specify the 'allocMode' allocation strategy.  Note that
        // the 'catalogPtr' must remain valid for the life of this choice
        // header.  Also note that this choice header will use the allocator
        // within the descriptor catalog for memory allocations.

    bdem_ChoiceHeader(const bdem_ChoiceHeader&                 original,
                      bdem_AggregateOption::AllocationStrategy allocMode
                                    = bdem_AggregateOption::BDEM_PASS_THROUGH);
        // Construct this choice header from the specified 'original' choice
        // header.  Optionally specify the 'allocMode' allocation strategy.
        // Note that the 'catalogPtr' must remain valid for the life of this
        // choice header.  Also note that this choice header will use the
        // allocator within the descriptor catalog for memory allocations.

    ~bdem_ChoiceHeader();
        // Destroy this choice header.  If this choice header is currently
        // null then this destructor is guaranteed not to access the stored
        // descriptor catalog or allocator.

    // MANIPULATORS
    bdem_ChoiceHeader& operator=(const bdem_ChoiceHeader& rhs);
        // Assign to this choice header the value of the specified 'rhs' choice
        // header, and return a reference to this choice header.  The behavior
        // is undefined unless the type catalogs of both choice headers are
        // identical.

    template <class STREAM>
    STREAM&
    bdexStreamIn(STREAM&                                stream,
                 int                                    version,
                 const bdem_DescriptorStreamIn<STREAM> *strmAttrLookup,
                 const bdem_Descriptor                 *const attrLookup[]);
        // Populate this choice header from the data in the specified input
        // 'stream' and return a reference to the modifiable 'stream'.  The
        // previous value of '*this' is discarded.  The 'version' is mandatory
        // and indicates the expected input stream format.  'strmAttrLookup'
        // maps each known element type to a corresponding function for
        // streaming in that type.  'attrLookup' maps each known element type
        // to a descriptor (see bdem_descriptor).
        //
        // If 'version' is not supported, 'stream' is marked invalid and this
        // choice header is left unchanged.  If 'stream' is initially invalid,
        // this choice header is left unchanged.  If 'stream' becomes invalid
        // during this operation, 'stream' is marked invalid; choice header is
        // valid, but its value is unspecified.  Decoding will fail (incorrect
        // data will be read and/or the stream will become invalid) unless the
        // types of choice headers encoded in 'stream' match the sequence of
        // element types in the list's row definition and 'stream's version
        // matches 'version'.
        //
        // Note that the type 'STREAM' must adhere to the protocol specified
        // by 'bdex_InStream' (see the package-group-level documentation for
        // more information on 'bdex' streaming of container types).

    void *makeSelection(int index);
        // Set the selector of this choice header to the specified 'index' and
        // the selection value to be the null value for the type specified by
        // the descriptor at 'index' in the descriptor catalog, destroying the
        // current selection value.  Return the address of the modifiable
        // selection value if '-1 != index', and 0 otherwise.  The behavior is
        // undefined unless '-1 <= index < numSelections()'.  Note that if
        // '-1 == index' this choice is set to the null state.  Also note
        // that if 'selector() == index', then this choice header is left
        // unchanged.

    void *makeSelection(int index, const void *value);
        // Set the selector of this choice header to the specified 'index' and
        // the selection value to the specified 'value' of the type specified
        // by the descriptor at 'index' in the descriptor catalog, destroying
        // the current selection value.  Return the address of the modifiable
        // selection value if '-1 != index', and 0 otherwise.  The behavior is
        // undefined unless '-1 <= index < numSelections()'.  Note that if
        // '-1 == index' this choice is set to the null state and 'value' is
        // ignored.

    void *selectionPointer();
        // Return the address of the modifiable selection value stored in this
        // choice header if '-1 != selector()', and 0 otherwise.

    int& flags();
        // Return a reference to the modifiable flags of this choice header.

    void clearNullnessBit();
        // Clear the nullness bit of this choice header.  The selector value is
        // uneffected.

    void setNullnessBit();
        // Set the nullness bit of this choice header.  The selector value is
        // uneffected.

    void reset();
        // Reset this choice header to its default constructed state.

    // ACCESSORS
    bool isSelectionNull() const;
        // Return 'true' if the selection value stored by this choice header
        // is null, and 'false' otherwise.

    int numSelections() const;
        // Return the number of descriptors in the descriptor catalog held by
        // this choice header.

    int selector() const;
        // Return the index of the current selection in the descriptor catalog
        // or -1 if no descriptor is currently selected.

    const bdem_Descriptor *selectionDescriptor(int index) const;
        // Return the address of the non-modifiable descriptor corresponding
        // to the specified 'index' in the descriptor catalog if '-1 != index',
        // and the descriptor corresponding to 'bdem_ElemType::BDEM_VOID'
        // otherwise.  The behavior is undefined unless
        // '-1 <= index < numSelections()'.

    const void *selectionPointer() const;
        // Return the address of the non-modifiable selection value stored in
        // this choice header if '-1 != selector()', and 0 otherwise.

    const int& flags() const;
        // Return a reference to the non-modifiable flags stored by this
        // choice header.

    const DescriptorCatalog *catalog() const;
        // Return the address of the non-modifiable descriptor catalog stored
        // by this choice header.

    bslma_Allocator *allocator() const;
        // Return the address of the modifiable allocator used by this choice
        // header.

    bdem_AggregateOption::AllocationStrategy allocMode() const;
        // Return the allocation mode used by this choice header.

    template <class STREAM>
    STREAM&
    bdexStreamOut(
                 STREAM&                                 stream,
                 int                                     version,
                 const bdem_DescriptorStreamOut<STREAM> *strmAttrLookup) const;
        // Write this choice header's flags type catalog, selector value and
        // selected element value to the specified output 'stream' and return
        // a reference to the modifiable 'stream'.  If no type is currently
        // selected then the selected element value is not written.  The
        // 'version' is mandatory and specifies the format of the output.  The
        // 'version' is *not* written to the stream.  If 'version' is not
        // supported, 'stream' is left unmodified.  Auxiliary information is
        // provided by 'strmAttrLookup', which maps each known element type to
        // a corresponding function for streaming in that type.
        //
        // The type 'STREAM' must adhere to the 'bdex_OutStream' protocol (see
        // the package-group-level documentation for more information on
        // 'bdex' streaming of container types).

    bsl::ostream& print(bsl::ostream& stream,
                        int           level          = 0,
                        int           spacesPerLevel = 4) const;
        // Format this choice header to the specified output 'stream' at the
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
bool operator==(const bdem_ChoiceHeader& lhs, const bdem_ChoiceHeader& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' choice headers have the
    // same value, and 'false' otherwise.  Two choice headers have the same
    // value if they store identical descriptor catalogs, and have the same
    // selector value and equal selection values.

inline
bool operator!=(const bdem_ChoiceHeader& lhs, const bdem_ChoiceHeader& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' choice headers do not
    // have the same value, and 'false' otherwise.  Two choice headers have
    // differing values if they have non-equal descriptor catalogs, or have
    // different selector values or unequal selection values.

inline
bsl::ostream& operator<<(bsl::ostream& stream, const bdem_ChoiceHeader& rhs);
    // Write the specified 'rhs' choice header to the specified output
    // 'stream' in some reasonable (human-readable) format, and return a
    // reference to the modifiable 'stream'.

// ===========================================================================
//                      INLINE FUNCTION DEFINITIONS
// ===========================================================================

                        // -----------------------
                        // class bdem_ChoiceHeader
                        // -----------------------

// PRIVATE MANIPULATORS
template <class STREAM>
void bdem_ChoiceHeader::streamInSelection(
                     STREAM&                                stream,
                     int                                    version,
                     const bdem_DescriptorStreamIn<STREAM> *strmAttrLookup,
                     const bdem_Descriptor                 *const attrLookup[])
{
    const bdem_Descriptor *descriptor = (*d_catalog_p)[d_selector];
    bdem_ElemType::Type    type = (bdem_ElemType::Type) descriptor->d_elemEnum;
    const bdem_DescriptorStreamIn<STREAM> *elemStrmAttr =
                                                   &strmAttrLookup[(int) type];

    version = bdem_ElemType::isAggregateType(type) ? version : 1;
    elemStrmAttr->streamIn(selectionPointer(),
                           stream,
                           version,
                           strmAttrLookup,
                           attrLookup);
}

// PRIVATE ACCESSORS
template <class STREAM>
void bdem_ChoiceHeader::streamOutSelection(
                  STREAM&                                 stream,
                  int                                     version,
                  const bdem_DescriptorStreamOut<STREAM> *strmAttrLookup) const
{
    const bdem_Descriptor *descriptor = (*d_catalog_p)[d_selector];
    bdem_ElemType::Type    type = (bdem_ElemType::Type) descriptor->d_elemEnum;
    const bdem_DescriptorStreamOut<STREAM> *elemStrmAttr =
                                                   &strmAttrLookup[(int) type];

    version = bdem_ElemType::isAggregateType(type) ? version : 1;
    elemStrmAttr->streamOut(selectionPointer(),
                            stream,
                            version,
                            strmAttrLookup);
}

// MANIPULATORS
template <class STREAM>
STREAM&
bdem_ChoiceHeader::bdexStreamIn(
                     STREAM&                                stream,
                     int                                    version,
                     const bdem_DescriptorStreamIn<STREAM> *strmAttrLookup,
                     const bdem_Descriptor                 *const attrLookup[])
{
    switch (version) {  // Switch on the schema version (starting with 1).
      case 3: {
        // Use a temporary selector value, so that the choice
        // header is considered completely constructed only if no exception
        // is thrown in the streaming in operation.
        int selector;
        int flags = 0;

        stream.getInt32(selector);
        if (!stream) {
            return stream;                                            // RETURN
        }

        if (-1 == selector) {
            reset();
            return stream;                                            // RETURN
        }

        if (static_cast<unsigned>(selector) >= d_catalog_p->size()) {
            stream.invalidate();
            return stream;
        }

        stream.getInt32(flags);
        if (!stream) {
            return stream;                                            // RETURN
        }

        makeSelection(selector);

        if (flags & (1 << bdem_ChoiceHeader::BDEM_NULLNESS_FLAG)) {
            return stream;
        }

        streamInSelection(stream, version, strmAttrLookup, attrLookup);
        if (stream) {
            clearNullnessBit();
        }
      } break;
      case 2:                                                   // FALL THROUGH
        // 'case 2' falls through to bring the choice components to the same
        // version as the list components.
      case 1: {
        // Use a temporary selector value, so that the choice header is
        // considered completely constructed only if no exception is thrown
        // during streaming.
        int selector;

        stream.getInt32(selector);
        if (!stream) {
            return stream;                                            // RETURN
        }

        if (-1 == selector) {
            reset();
            return stream;                                            // RETURN
        }

        if (static_cast<unsigned>(selector) >= d_catalog_p->size()) {
            stream.invalidate();
            return stream;
        }

        makeSelection(selector);
        streamInSelection(stream, version, strmAttrLookup, attrLookup);
        if (stream) {
            clearNullnessBit();
        }
      } break;
      default: {
        stream.invalidate();
      }
    }

    return stream;
}

inline
void bdem_ChoiceHeader::clearNullnessBit()
{
    d_flags &= ~(1 << bdem_ChoiceHeader::BDEM_NULLNESS_FLAG);
}

inline
void bdem_ChoiceHeader::setNullnessBit()
{
    d_flags |= (1 << bdem_ChoiceHeader::BDEM_NULLNESS_FLAG);
}

inline
int& bdem_ChoiceHeader::flags()
{
    return d_flags;
}

// ACCESSORS
inline
bool bdem_ChoiceHeader::isSelectionNull() const
{
    return d_flags & (1 << bdem_ChoiceHeader::BDEM_NULLNESS_FLAG);
}

inline
int bdem_ChoiceHeader::numSelections() const
{
    return (int)d_catalog_p->size();
}

inline
int bdem_ChoiceHeader::selector() const
{
    return d_selector;
}

inline
const void *bdem_ChoiceHeader::selectionPointer() const
{
    return const_cast<bdem_ChoiceHeader *>(this)->selectionPointer();
}

inline
const int& bdem_ChoiceHeader::flags() const
{
    return d_flags;
}

inline
const bdem_ChoiceHeader::DescriptorCatalog *
bdem_ChoiceHeader::catalog() const
{
    return d_catalog_p;
}

inline
bslma_Allocator *bdem_ChoiceHeader::allocator() const
{
    return d_catalog_p->get_allocator().mechanism();
}

inline
bdem_AggregateOption::AllocationStrategy bdem_ChoiceHeader::allocMode() const
{
    return d_allocMode;
}

template <class STREAM>
STREAM&
bdem_ChoiceHeader::bdexStreamOut(
                  STREAM&                                 stream,
                  int                                     version,
                  const bdem_DescriptorStreamOut<STREAM> *strmAttrLookup) const
{
    switch (version) {  // Switch on the version (starting with 1).
      case 3: {
        stream.putInt32(d_selector);
        if (!stream) {
            return stream;                                            // RETURN
        }

        if (-1 != d_selector) {
            stream.putInt32(d_flags);
            if (!isSelectionNull()) {
                streamOutSelection(stream, version, strmAttrLookup);
            }
        }
      } break;
      case 2:                                                   // FALL THROUGH
        // 'case 2' falls through to bring the choice components to the same
        // version as the list components.
      case 1: {
        stream.putInt32(d_selector);
        if (!stream) {
            return stream;                                            // RETURN
        }

        if (-1 != d_selector) {
            streamOutSelection(stream, version, strmAttrLookup);
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
bool operator!=(const bdem_ChoiceHeader& lhs, const bdem_ChoiceHeader& rhs)
{
    return !(lhs == rhs);
}

inline
bsl::ostream& operator<<(bsl::ostream& stream, const bdem_ChoiceHeader& rhs)
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
