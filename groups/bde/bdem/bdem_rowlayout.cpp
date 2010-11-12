// bdem_rowlayout.cpp                                                 -*-C++-*-
#include <bdem_rowlayout.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bdem_rowlayout_cpp,"$Id$ $CSID$")

#include <bsls_alignmentutil.h>
#include <bsls_assert.h>

#include <bdeu_print.h>

#include <bsl_algorithm.h>
#include <bsl_ostream.h>
#include <bsl_vector.h>

namespace BloombergLP {

                        // --------------------
                        // class bdem_RowLayout
                        // --------------------

// CREATORS
bdem_RowLayout::bdem_RowLayout(const InitialCapacity&  numElements,
                               bslma_Allocator        *basicAllocator)
: d_entries(basicAllocator)
, d_totalOffset(0)
{
    BSLS_ASSERT(0 <= numElements.d_i);

    d_entries.reserve(numElements.d_i);
}

bdem_RowLayout::bdem_RowLayout(const bdem_ElemType::Type     elementTypes[],
                               int                           numElements,
                               const bdem_Descriptor *const  attributesTable[],
                               bslma_Allocator              *basicAllocator)
: d_entries(basicAllocator)
, d_totalOffset(0)
{
    BSLS_ASSERT(0 <= numElements);

    d_entries.reserve(numElements);
    resetElemTypes(elementTypes, numElements, attributesTable);

    // Note that 'd_offset's and 'd_totalOffset' are set by 'compact'.
}

bdem_RowLayout::bdem_RowLayout(const bdem_RowLayout&  srcRowLayout,
                               int                    srcIndex,
                               int                    numElements,
                               bslma_Allocator       *basicAllocator)
: d_entries(basicAllocator)
, d_totalOffset(srcRowLayout.d_totalOffset)
{
    BSLS_ASSERT(0 <= srcIndex);
    BSLS_ASSERT(0 <= numElements);
    BSLS_ASSERT(srcIndex + numElements <= srcRowLayout.length());

    d_entries.reserve(numElements);
    d_entries.insert(d_entries.end(),
                     srcRowLayout.d_entries.begin() + srcIndex,
                     srcRowLayout.d_entries.begin() + srcIndex + numElements);
}

// MANIPULATORS
bdem_RowLayout& bdem_RowLayout::operator=(const bdem_RowLayout& rhs)
{
    if (&rhs != this) {
        d_entries.reserve(rhs.d_entries.size());
        d_entries     = rhs.d_entries;
        d_totalOffset = rhs.d_totalOffset;
    }

    return *this;
}

void bdem_RowLayout::append(const bdem_Descriptor *elementAttributes)
{
    BSLS_ASSERT(elementAttributes);

    // Calculate the offset for this element.

    const int offset = insertionOffset(elementAttributes);

    d_entries.push_back(bdem_RowLayoutEntry(elementAttributes, offset));

    d_totalOffset = offset + elementAttributes->d_size;
}

void bdem_RowLayout::append(const bdem_ElemType::Type    elementTypes[],
                            int                          numElements,
                            const bdem_Descriptor *const attributesTable[])
{
    BSLS_ASSERT(0 <= numElements);

    const int dstLength   = length();
    const int totalLength = numElements + dstLength;

    d_entries.reserve(totalLength);
    for (int i = 0; i < numElements; ++i) {
        const bdem_Descriptor *desc = attributesTable[elementTypes[i]];
        const int offset = insertionOffset(desc);
        d_entries.push_back(bdem_RowLayoutEntry(desc, offset));
        d_totalOffset = offset + desc->d_size;
    }
}

void bdem_RowLayout::compact()
{
    enum {
        BDEM_MAX_ALIGN = bsls_AlignmentUtil::BSLS_MAX_ALIGNMENT
    };

    const int len = length();

    int listHeads[BDEM_MAX_ALIGN + 1];

    // Create a set of linked lists of elements with identical alignment
    // requirements.  At the end of this loop, 'listHeads[m]' will contain the
    // index within 'd_entries' of the last element with alignment 'm', and
    // 'd_entries[n].offset()' will contain the index of the previous element
    // with the same alignment as element 'n'.

    for (int align = BDEM_MAX_ALIGN; align > 0; align /= 2) {
        listHeads[align] = -1;
    }

    for (int i = 0; i < len; ++i) {
        const int align = d_entries[i].attributes()->d_alignment;
        d_entries[i].offset() = listHeads[align];  // 'offset' temporarily
                                                   // changes its meaning
        listHeads[align] = i;
    }

    // Now iterate through the elements, starting with the most-strictly
    // aligned members and compute new offsets for each one.  Since all
    // alignments are powers of 2, we need to iterate through only the powers
    // of 2 in the 'listHeads' array.

    d_totalOffset = 0;
    for (int align = BDEM_MAX_ALIGN; align > 0; align /= 2) {
        // The items are in reverse order in the list; reverse them so that
        // in the end the offsets of items of a given alignment requirement
        // will be sorted in the same order as their 'listHeads' in the
        // row layout.

        int newHead = -1;
        int next;
        for (int j = listHeads[align]; -1 != j; j = next) {
            next = d_entries[j].offset();
            d_entries[j].offset() = newHead;
            newHead = j;
        }
        listHeads[align] = newHead;

        // Iterate through the linked list of elements with alignment 'align'.

        const int alignMinus1 = align - 1;
        for (int j = listHeads[align]; -1 != j; j = next) {
            next = d_entries[j].offset();
            d_entries[j].offset() = d_totalOffset;  // restored to original
                                                    // meaning
            d_totalOffset += d_entries[j].attributes()->d_size;

            BSLS_ASSERT(0 == (d_totalOffset & alignMinus1));
        }
    }
}

void bdem_RowLayout::insert(int                    dstIndex,
                            const bdem_Descriptor *elementAttributes)
{
    BSLS_ASSERT(0 <= dstIndex);
    BSLS_ASSERT(dstIndex <= static_cast<int>(d_entries.size()));
    BSLS_ASSERT(elementAttributes);

    // Calculate the offset for this element.

    const int offset = insertionOffset(elementAttributes);

    d_entries.insert(d_entries.begin() + dstIndex,
                     bdem_RowLayoutEntry(elementAttributes, offset));

    d_totalOffset = offset + elementAttributes->d_size;
}

void bdem_RowLayout::insert(int                   dstIndex,
                            const bdem_RowLayout& srcRowLayout,
                            int                   srcIndex,
                            int                   numElements)
{
    BSLS_ASSERT(0 <= dstIndex);
    BSLS_ASSERT(dstIndex <= static_cast<int>(d_entries.size()));
    BSLS_ASSERT(0 <= srcIndex);
    BSLS_ASSERT(0 <= numElements);
    BSLS_ASSERT(srcIndex + numElements <= srcRowLayout.length());

    if (0 == numElements) {
        return;
    }

    const int dstLength   = length();
    const int dstEndIndex = dstIndex + numElements;
    const int totalLength = numElements + dstLength;

    bdem_RowLayoutEntry dummy(srcRowLayout[0].attributes(), 0);

    d_entries.resize(totalLength, dummy);
    bsl::copy_backward(d_entries.begin() + dstIndex,
                       d_entries.begin() + dstLength,
                       d_entries.end());

    for (int i = 0; i < numElements; ++i) {
        const bdem_Descriptor *desc = srcRowLayout[srcIndex + i].attributes();

        // Calculate the appropriate offset for the next element.

        const int offset = insertionOffset(desc);

        d_entries[dstIndex + i] = bdem_RowLayoutEntry(desc, offset);
        d_totalOffset           = offset + desc->d_size;
    }
}

void bdem_RowLayout::insert(int                          dstIndex,
                            const bdem_ElemType::Type    elementTypes[],
                            int                          numElements,
                            const bdem_Descriptor *const attributesTable[])
{
    BSLS_ASSERT(0 <= dstIndex);
    BSLS_ASSERT(dstIndex <= static_cast<int>(d_entries.size()));
    BSLS_ASSERT(0 <= numElements);

    if (0 == numElements) {
        return;
    }

    const int dstLength   = length();
    const int dstEndIndex = dstIndex + numElements;
    const int totalLength = numElements + dstLength;

    bdem_RowLayoutEntry dummy(attributesTable[elementTypes[0]], 0);

    d_entries.resize(totalLength, dummy);
    bsl::copy_backward(d_entries.begin() + dstIndex,
                       d_entries.begin() + dstLength,
                       d_entries.end());

    for (int i = dstIndex; i < dstEndIndex; ++i) {
        // Calculate the appropriate offset for the next element.

        const bdem_Descriptor *desc =
                                   attributesTable[elementTypes[i - dstIndex]];
        const int offset = insertionOffset(desc);

        d_entries[i]  = bdem_RowLayoutEntry(desc, offset);
        d_totalOffset = offset + desc->d_size;
    }
}

void bdem_RowLayout::replace(int                    dstIndex,
                             const bdem_Descriptor *elementAttributes)
{
    BSLS_ASSERT(0 <= dstIndex);
    BSLS_ASSERT(dstIndex < static_cast<int>(d_entries.size()));
    BSLS_ASSERT(elementAttributes);

    // Calculate the offset for this element.

    const int offset = insertionOffset(elementAttributes);

    d_entries[dstIndex] = bdem_RowLayoutEntry(elementAttributes, offset);
    d_totalOffset       = offset + elementAttributes->d_size;
}

void bdem_RowLayout::replace(int                          dstIndex,
                             const bdem_ElemType::Type    elementTypes[],
                             int                          numElements,
                             const bdem_Descriptor *const attributesTable[])
{
    BSLS_ASSERT(0 <= dstIndex);
    BSLS_ASSERT(0 <= numElements);
    BSLS_ASSERT(dstIndex + numElements <= static_cast<int>(d_entries.size()));

    for (int i = dstIndex; i < dstIndex + numElements; ++i) {
        // Calculate the appropriate offset for the next element.

        const bdem_Descriptor *desc =
                                   attributesTable[elementTypes[i - dstIndex]];
        const int offset = insertionOffset(desc);

        d_entries[i]  = bdem_RowLayoutEntry(desc, offset);
        d_totalOffset = offset + desc->d_size;
    }
}

void bdem_RowLayout::replace(
                        int                                        dstIndex,
                        const bsl::vector<const bdem_Descriptor*>& srcArray,
                        int                                        srcIndex,
                        int                                        numElements)
{
    BSLS_ASSERT(0 <= dstIndex);
    BSLS_ASSERT(0 <= srcIndex);
    BSLS_ASSERT(0 <= numElements);
    BSLS_ASSERT(dstIndex + numElements <= static_cast<int>(d_entries.size()));
    BSLS_ASSERT(srcIndex + numElements <= static_cast<int>(srcArray.size()));

    for (int i = dstIndex; i < dstIndex + numElements; ++i) {
        // Calculate the appropriate offset for the next element.

        const bdem_Descriptor *desc = srcArray[i - dstIndex + srcIndex];
        const int offset = insertionOffset(desc);

        d_entries[i]  = bdem_RowLayoutEntry(desc, offset);
        d_totalOffset = offset + desc->d_size;
    }
}

void bdem_RowLayout::resetElemTypes(
                                const bdem_ElemType::Type    elementTypes[],
                                int                          numElements,
                                const bdem_Descriptor *const attributesTable[])
{
    BSLS_ASSERT(0 <= numElements);

    d_entries.clear();
    d_entries.reserve(numElements);
    d_totalOffset = 0;

    for (int i = 0; i < numElements; ++i) {
        // Calculate the appropriate offset for the next element.

        const bdem_Descriptor *desc = attributesTable[elementTypes[i]];
        const int offset = insertionOffset(desc);

        d_entries.push_back(bdem_RowLayoutEntry(desc, offset));
        d_totalOffset = offset + desc->d_size;
    }

    compact();  // 'd_offset's and 'd_totalOffset' are set by 'compact'.
}

// ACCESSORS
int bdem_RowLayout::adjustedTotalOffset(
                          const bdem_ElemType::Type    elementTypes[],
                          int                          numElements,
                          const bdem_Descriptor *const attributesTable[]) const
{
    BSLS_ASSERT(0 <= numElements);

    int totalOffset = d_totalOffset;

    for (int i = 0; i < numElements; ++i) {
        // Calculate the appropriate offset for the next element.

        const bdem_Descriptor *desc = attributesTable[elementTypes[i]];
        const int alignMinus1 = desc->d_alignment - 1;
        const int offset      = (totalOffset + alignMinus1) & ~alignMinus1;

        totalOffset = offset + desc->d_size;
    }

    return totalOffset;
}

int bdem_RowLayout::adjustedTotalOffset(
                                       const bdem_RowLayout& srcRowLayout,
                                       int                   srcIndex,
                                       int                   numElements) const
{
    BSLS_ASSERT(0 <= srcIndex);
    BSLS_ASSERT(0 <= numElements);
    BSLS_ASSERT(srcIndex + numElements <= srcRowLayout.length());

    int totalOffset = d_totalOffset;
    for (int i = 0; i < numElements; ++i) {
        // Calculate the appropriate offset for the next element.

        const bdem_Descriptor *desc =
                             srcRowLayout.d_entries[srcIndex + i].attributes();
        const int alignMinus1 = desc->d_alignment - 1;
        const int offset      = (totalOffset + alignMinus1) & ~alignMinus1;

        totalOffset = offset + desc->d_size;
    }

    return totalOffset;
}

bool bdem_RowLayout::areElemTypesEqual(const bdem_RowLayout& other) const
{
    const int len = length();

    if (len != other.length()) {
        return false;                                                 // RETURN
    }

    for (int i = 0; i < len; ++i) {
        if (d_entries[i].attributes()->d_elemEnum !=
                                 other.d_entries[i].attributes()->d_elemEnum) {
            return false;                                             // RETURN
        }
    }

    return true;
}

bsl::ostream& bdem_RowLayout::print(bsl::ostream& stream,
                                    int           level,
                                    int           spacesPerLevel) const
{
    char separator = '\n';
    if (spacesPerLevel < 0) {
        separator = ' ';
        spacesPerLevel = 0;
    }

    bdeu_Print::indent(stream, level, spacesPerLevel);
    stream << '{' << separator;

    if (level < 0) {
        level = -level;
    }

    const int levelPlus1 = level + 1;
    const int len        = length();

    for (int i = 0; i < len; ++i) {
        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        bdem_ElemType::Type elemEnum =
                    (bdem_ElemType::Type)d_entries[i].attributes()->d_elemEnum;
        stream << elemEnum << ' ' << d_entries[i].offset() << separator;
    }

    bdeu_Print::indent(stream, level, spacesPerLevel);
    stream << '}';
    if ('\n' == separator) {
        stream << '\n';
    }
    return stream;
}

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2010
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
