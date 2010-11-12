// bdem_choiceheader.cpp                                              -*-C++-*-
#include <bdem_choiceheader.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bdem_choiceheader_cpp,"$Id$ $CSID$")

#include <bdem_elemtype.h>
#include <bdem_functiontemplates.h>   // for testing only
#include <bdem_properties.h>

#include <bslma_deallocatorproctor.h>

#include <bsls_alignedbuffer.h>
#include <bsls_alignmentfromtype.h>
#include <bsls_assert.h>

#include <bdeu_print.h>

#include <bsl_ostream.h>

namespace BloombergLP {

                    // --------------------------------------
                    // CHOICE_ARRAY_ITEM DESCRIPTOR FUNCTIONS
                    // --------------------------------------

static
void makeChoiceItemUnset(void *obj)
{
    BSLS_ASSERT(obj);

    bdem_ChoiceHeader& header = *static_cast<bdem_ChoiceHeader *>(obj);
    header.reset();
}

static
bool isChoiceItemUnset(const void *obj)
{
    BSLS_ASSERT(obj);

    return static_cast<const bdem_ChoiceHeader *>(obj)->isSelectionNull();
}

static
bool areChoiceItemsEqual(const void *lhs, const void *rhs)
{
    BSLS_ASSERT(lhs);
    BSLS_ASSERT(rhs);

    const bdem_ChoiceHeader& lhsChoiceItem =
                                  *static_cast<const bdem_ChoiceHeader *>(lhs);
    const bdem_ChoiceHeader& rhsChoiceItem =
                                  *static_cast<const bdem_ChoiceHeader *>(rhs);

    return lhsChoiceItem == rhsChoiceItem;
}

static
bsl::ostream& printChoiceItem(const void    *obj,
                              bsl::ostream&  stream,
                              int            level,
                              int            spacesPerLevel)
{
    BSLS_ASSERT(obj);

    const bdem_ChoiceHeader& choiceItem =
                                  *static_cast<const bdem_ChoiceHeader *>(obj);
    return choiceItem.print(stream, level, spacesPerLevel);
}

                        // -----------------------
                        // class bdem_ChoiceHeader
                        // -----------------------

// CLASS DATA
const bdem_Descriptor bdem_ChoiceHeader::d_choiceItemAttr =
{
    bdem_ElemType::BDEM_CHOICE_ARRAY_ITEM,             // element enumeration
    sizeof(bdem_ChoiceHeader),                         // size
    bsls_AlignmentFromType<bdem_ChoiceHeader>::VALUE,  // alignment
    0,                                                 // unsetConstruct,
    0,                                                 // copyConstruct
    0,                                                 // destroy
    0,                                                 // assign
    0,                                                 // move
    &makeChoiceItemUnset,                              // makeUnset
    &isChoiceItemUnset,                                // isUnset
    &areChoiceItemsEqual,                              // areEqual
    &printChoiceItem                                   // print
};

// CREATORS
bdem_ChoiceHeader::bdem_ChoiceHeader(
                         const DescriptorCatalog                  *catalogPtr,
                         bdem_AggregateOption::AllocationStrategy  allocMode)
: d_catalog_p(catalogPtr)
, d_selector(-1)
, d_flags(1)
, d_allocMode(bdem_AggregateOption::AllocationStrategy(
                   allocMode & ~bdem_AggregateOption::BDEM_OWN_ALLOCATOR_FLAG))
{
    d_selection_p = 0;  // eases debugging
}

bdem_ChoiceHeader::bdem_ChoiceHeader(
                           const bdem_ChoiceHeader&                 original,
                           bdem_AggregateOption::AllocationStrategy allocMode)
: d_catalog_p(original.d_catalog_p)
, d_selector(-1)
, d_flags(1)
, d_allocMode(bdem_AggregateOption::AllocationStrategy(
                   allocMode & ~bdem_AggregateOption::BDEM_OWN_ALLOCATOR_FLAG))
{
    d_selection_p = 0;           // eases debugging
    makeSelection(original.d_selector, original.selectionPointer());
    d_flags = original.d_flags;  // 'makeSelection' set as non-null
}

bdem_ChoiceHeader::~bdem_ChoiceHeader()
{
    reset();
}

// MANIPULATORS
bdem_ChoiceHeader& bdem_ChoiceHeader::operator=(const bdem_ChoiceHeader& rhs)
{
    if (this != &rhs) {
        // Assume that catalog management is done at a higher level.

        makeSelection(rhs.selector(), rhs.selectionPointer());
        d_flags = rhs.d_flags;  // 'makeSelection' set as non-null
    }
    return *this;
}

void *bdem_ChoiceHeader::makeSelection(int index)
{
    BSLS_ASSERT(-1 <= index);
    BSLS_ASSERT(      index < numSelections());

    const bdem_Descriptor *descriptorPtr = selectionDescriptor(index);
    void *dataPtr = 0;

    if (-1 == index) {
        reset();
    }
    else if (descriptorPtr == selectionDescriptor(selector())) {
        // Data type has not changed, use 'makeUnset' instead of 'reset'.

        dataPtr = selectionPointer();
        descriptorPtr->makeUnset(dataPtr);
        setNullnessBit();
    }
    else if (static_cast<unsigned>(descriptorPtr->d_size)
                                                    <= sizeof d_selectionBuf) {
        // Data type has changed and new data type fits in 'd_selectionBuf'.
        // Call 'reset' to set 'd_selector' to -1.  If an exception occurs
        // during 'unsetConstruct', 'd_selector' will safely remain -1.

        reset();
        dataPtr = &d_selectionBuf;
        descriptorPtr->unsetConstruct(dataPtr, d_allocMode, allocator());
    }
    else {
        // Data type has changed and new data type is larger than
        // 'd_selectionBuf'.  We must allocate space for the new value.
        // Allocate new memory and protect it from leaking on exception.

        dataPtr = allocator()->allocate(descriptorPtr->d_size);
        bslma_DeallocatorProctor<bslma_Allocator> autoDealloc(dataPtr,
                                                              allocator());

        // Construct an unset value, which *can* throw.

        descriptorPtr->unsetConstruct(dataPtr, d_allocMode, allocator());
        autoDealloc.release();

        reset();  // clear old 'd_selection_p'
        d_selection_p = dataPtr;
    }

    d_selector = index;

    return dataPtr;
}

void *bdem_ChoiceHeader::makeSelection(int index, const void *value)
{
    BSLS_ASSERT(-1 <= index);
    BSLS_ASSERT(      index < numSelections());

    const bdem_Descriptor *descriptorPtr = selectionDescriptor(index);
    void *dataPtr = 0;

    // Verify that descriptor at index matches 'descriptorPtr'.

    if (-1 == index) {
        reset();
    }
    else if (descriptorPtr == selectionDescriptor(selector())) {
        // Data type has not changed, so use assignment.

        dataPtr = selectionPointer();
        descriptorPtr->assign(dataPtr, value);
        clearNullnessBit();
    }
    else if (static_cast<unsigned>(descriptorPtr->d_size)
                                                    <= sizeof d_selectionBuf) {
        // Data type has changed and new data type fits in 'd_selectionBuf'.
        // There is a chance that 'value' is a sub-part of the current
        // selection ("aliasing").  Thus, we must make a copy of 'value' BEFORE
        // destroying the current selection.  For efficiency and exception
        // safety, we construct a new object in a separate memory space then
        // bit-wise copy it into the current selection.  This works because all
        // 'bdem' types are bit-wise moveable.  If an exception is thrown, the
        // original contents of 'd_selectionBuf' is left unchanged.

        bsls_AlignedBuffer<sizeof d_selectionBuf> temp;
        descriptorPtr->copyConstruct(&temp,
                                     value,
                                     d_allocMode,
                                     allocator());

        reset();                // Destroy old contents of 'd_selectionBuf'.
        d_selectionBuf = temp;  // Bit-wise copy 'temp' to 'd_selectionBuf'.

        dataPtr = &d_selectionBuf;
        clearNullnessBit();

        // Destructor for 'temp' is a no-op.  This is good, since the
        // contents of 'temp' has been "moved" to 'd_selectionBuf'.
    }
    else {
        // Data type had changed and new data type is larger than
        // 'd_selectionBuf'.  We must allocate space for the new value.

        // There is a chance that 'value' is a sub-part of the current
        // selection ("aliasing").  For this reason, as well as for
        // exception safety, we must allocate and construct a copy of
        // 'value' BEFORE destroying the current selection.

        // Allocate new memory and protect it from leaking on exception.

        dataPtr = allocator()->allocate(descriptorPtr->d_size);
        bslma_DeallocatorProctor<bslma_Allocator> autoDealloc(dataPtr,
                                                              allocator());

        // Construct a copy of 'value', which *can* throw.

        descriptorPtr->copyConstruct(dataPtr,
                                     value,
                                     d_allocMode,
                                     allocator());
        autoDealloc.release();

        reset();  // clear old 'd_selection_p'
        d_selection_p = dataPtr;
        clearNullnessBit();
    }

    d_selector = index;

    return dataPtr;
}

void *bdem_ChoiceHeader::selectionPointer()
{
    if (-1 == d_selector) {
        return 0;                                                     // RETURN
    }

    const bdem_Descriptor *descriptorPtr = (*d_catalog_p)[d_selector];
    return (static_cast<unsigned>(descriptorPtr->d_size)
                                    > sizeof d_selectionBuf) ? d_selection_p
                                                             : &d_selectionBuf;
}

void bdem_ChoiceHeader::reset()
{
    if (d_selector >= 0) {
        const bdem_Descriptor *descriptorPtr = (*d_catalog_p)[d_selector];
        if (static_cast<unsigned>(descriptorPtr->d_size)
                                                     > sizeof d_selectionBuf) {
            // Destroy and deallocate external value.

            if (!(d_allocMode & bdem_AggregateOption::BDEM_NODESTRUCT_FLAG)) {
                descriptorPtr->destroy(d_selection_p);
            }
            allocator()->deallocate(d_selection_p);
        }
        else if (!(d_allocMode & bdem_AggregateOption::BDEM_NODESTRUCT_FLAG)) {
            // Destroy in-place value.

            descriptorPtr->destroy(&d_selectionBuf);
        }

        d_selector    = -1;
        d_selection_p = 0;   // eases debugging
    }
    setNullnessBit();  // make null
}

// ACCESSORS
const bdem_Descriptor *bdem_ChoiceHeader::selectionDescriptor(int index) const
{
    BSLS_ASSERT(-1 <= index);
    BSLS_ASSERT(      index < numSelections());

    return -1 == index ? &bdem_Properties::d_voidAttr : (*d_catalog_p)[index];
}

bsl::ostream& bdem_ChoiceHeader::print(bsl::ostream& stream,
                                       int           level,
                                       int           spacesPerLevel) const
{
    int nestedLevel, nestedSpacesPerLevel;
    bdeu_Print::indent(stream, level, spacesPerLevel);

    stream << '{';
    if (level < 0) {
        level = -level;
    }

    if (0 <= spacesPerLevel) {
        // Output newlines only if 'spacesPerLevel' is non-negative.

        stream << '\n';
        nestedLevel = level + 1;
        nestedSpacesPerLevel = spacesPerLevel;
    }
    else {
        // If 'spacesPerLevel' is negative, just put one space between fields
        // and suppress newlines when formatting each field.

        nestedLevel = 1;
        nestedSpacesPerLevel = -1;
    }

    bdeu_Print::indent(stream, nestedLevel, nestedSpacesPerLevel);

    // Output element type name followed by value.

    const bdem_Descriptor *descriptorPtr = selectionDescriptor(selector());
    if (d_selector < 0) {
        stream << "VOID";
    }
    else if (static_cast<unsigned>(descriptorPtr->d_elemEnum)
                      < static_cast<unsigned>(bdem_ElemType::BDEM_NUM_TYPES)) {
        bdem_ElemType::Type elemTypeEnum =
                   static_cast<bdem_ElemType::Type>(descriptorPtr->d_elemEnum);
        stream << elemTypeEnum;
    }
    else {
        // Out-of-range.  Print type as integer.

        stream << descriptorPtr->d_elemEnum;
    }

    stream << ' ';

    if (isSelectionNull()) {
        stream << "NULL";
        if (0 <= spacesPerLevel) {
            stream << "\n";
        }
    }
    else {
        descriptorPtr->print(selectionPointer(),
                             stream,
                             -nestedLevel,
                             nestedSpacesPerLevel);
    }

    if (0 <= spacesPerLevel) {
        bdeu_Print::indent(stream, level, spacesPerLevel);
        stream << "}\n";
    }
    else {
        stream << " }";
    }

    return stream << bsl::flush;
}

// FREE OPERATORS
bool operator==(const bdem_ChoiceHeader& lhs, const bdem_ChoiceHeader& rhs)
{
    return lhs.selector() == rhs.selector()
        && *lhs.catalog() == *rhs.catalog()
        && lhs.flags()    == rhs.flags()
        && lhs.selectionDescriptor(lhs.selector())->areEqual(
                                                       lhs.selectionPointer(),
                                                       rhs.selectionPointer());
}

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2006
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
