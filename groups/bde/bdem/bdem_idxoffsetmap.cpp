// bdem_idxoffsetmap.cpp     -*-C++-*-

#include <bdem_idxoffsetmap.h>

#include <bdes_platformutil.h>
#include <bdeu_print.h>

#include <algorithm>
#include <ostream>
#include <vector>

namespace BloombergLP {

                             // ------------
                             // MANIPULATORS
                             // ------------

void bdem_IdxOffsetMap::adjustedTotalOffset(
                           int                          *total,
                           const bdem_ElemType::Type     elementTypes[],
                           int                           numElements,
                           const bdem_Descriptor *const  attrLookupTbl[]) const
{
    int totalOffset = d_totalOffset;

    for (int i = 0; i < numElements; ++i) {
        const bdem_Descriptor *elemAttr = attrLookupTbl[elementTypes[i]];
        int align  = elemAttr->d_alignment;
        int offset = (totalOffset + align - 1) & ~(align - 1);
        totalOffset = offset + elemAttr->d_size;
    }

    *total = totalOffset;
}

void bdem_IdxOffsetMap::adjustedTotalOffset(
                                   int                      *total,
                                   const bdem_IdxOffsetMap&  original,
                                   int                       srcIndex,
                                   int                       numElements) const
{
    int totalOffset = d_totalOffset;
    for (int i = 0; i < numElements; ++i) {
        const bdem_Descriptor *elemAttr =
                                       original.d_map[srcIndex + i].d_elemAttr;
        int align  = elemAttr->d_alignment;
        int offset = (totalOffset + align - 1) & ~(align - 1);
        totalOffset = offset + elemAttr->d_size;
    }

    *total = totalOffset;
}

void bdem_IdxOffsetMap::append(const bdem_ElemType::Type    elementTypes[],
                               int                          numElements,
                               const bdem_Descriptor *const attrLookupTbl[])
{
    int dstLength   = length();
    int totalLength = numElements + dstLength;

    d_map.resize(totalLength);
    for (int i = 0; i < numElements; ++i) {
        const bdem_Descriptor *elemAttr = attrLookupTbl[elementTypes[i]];
        int align  = elemAttr->d_alignment;
        int offset = (d_totalOffset + align - 1) & ~(align - 1);
        d_map[dstLength + i].d_elemAttr = elemAttr;
        d_map[dstLength + i].d_offset = offset;
        d_totalOffset = offset + elemAttr->d_size;
    }
}

void bdem_IdxOffsetMap::compact()
{
    static const int maxAlign = bdes_Alignment::MAX_ALIGNMENT;

    int myLength = length();
    int indices[maxAlign + 1];

    for (int i = 0; i <= maxAlign; ++i) {
        indices[i] = -1;
    }

    // Create a set of linked lists of elements with identical alignment
    // requirements.  At the end of this loop, indices[m] will contain the
    // index within d_map of the last element with alignment m, and
    // d_map[n].d_offset will contain the index of the previous element with
    // the same alignment as element n.  This is a temporary use of d_offset
    // which is unrelated to memory offsets.
    for (int i = 0; i < myLength; ++i) {
        int align = d_map[i].d_elemAttr->d_alignment;
        d_map[i].d_offset = indices[align];
        indices[align] = i;
    }

    // Now iterate through the elements, starting with the most-strictly
    // aligned members and compute new offsets for each one.  Since all
    // alignments are powers of 2, we only need to iterate through the powers
    // of 2 in the indices array.
    d_totalOffset = 0;
    for (int align = maxAlign; align > 0; align /= 2) {
        int j = indices[align];
        if (-1 == j) {
            continue;
        }

        // Round offset up to alignment value
        d_totalOffset += align - 1;
        d_totalOffset &= ~(align - 1);

        // Iterate through linked list of elements with alignment 'align'.
        do {
            int next = d_map[j].d_offset;
            d_map[j].d_offset = d_totalOffset;
            d_totalOffset += d_map[j].d_elemAttr->d_size;

            // assert(0 == d_totalOffset % align);
            j = next;
        } while (-1 != j);
    }
}

void bdem_IdxOffsetMap::insert(int                          dstIndex,
                               const bdem_ElemType::Type    elementTypes[],
                               int                          numElements,
                               const bdem_Descriptor *const attrLookupTbl[])
{
    int dstLength   = length();
    int dstEndIndex = dstIndex + numElements;
    int totalLength = numElements + dstLength;

    d_map.resize(totalLength);
    std::copy_backward(d_map.begin() + dstIndex,
                       d_map.begin() + dstLength,
                       d_map.end());

    for (int i = dstIndex; i < dstEndIndex; ++i) {
        const bdem_Descriptor *elemAttr =
                                     attrLookupTbl[elementTypes[i - dstIndex]];
        int align  = elemAttr->d_alignment;
        int offset = (d_totalOffset + align - 1) & ~(align - 1);
        d_map[i].d_elemAttr = elemAttr;
        d_map[i].d_offset   = offset;
        d_totalOffset = offset + elemAttr->d_size;
    }
}

void bdem_IdxOffsetMap::replace(int                          dstIndex,
                                const bdem_ElemType::Type    elementTypes[],
                                int                          numElements,
                                const bdem_Descriptor *const attrLookupTbl[])
{
    for (int i = dstIndex; i < dstIndex + numElements; ++i) {
        const bdem_Descriptor *elemAttr =
                                  attrLookupTbl[elementTypes[i-dstIndex]];
        int align  = elemAttr->d_alignment;
        int offset = (d_totalOffset + align - 1) & ~(align - 1);
        d_map[i].d_elemAttr = elemAttr;
        d_map[i].d_offset  = offset;
        d_totalOffset = offset + elemAttr->d_size;
    }
}

void bdem_IdxOffsetMap::replace(
                        int                                        dstIndex,
                        const std::vector<const bdem_Descriptor*>& srcArray,
                        int                                        srcIndex,
                        int                                        numElements)
{
    for (int i = dstIndex; i < dstIndex + numElements; ++i) {
        const bdem_Descriptor *elemAttr = srcArray[i - dstIndex + srcIndex];
        int align  = elemAttr->d_alignment;
        int offset = (d_totalOffset + align - 1) & ~(align - 1);
        d_map[i].d_elemAttr = elemAttr;
        d_map[i].d_offset  = offset;
        d_totalOffset = offset + elemAttr->d_size;
    }
}

void bdem_IdxOffsetMap::resetElemTypes(
                                  const bdem_ElemType::Type    elementTypes[],
                                  int                          numElements,
                                  const bdem_Descriptor *const attrLookupTbl[])
{
    d_map.resize(numElements);
    d_totalOffset = 0;

    for (int i = 0; i < numElements; ++i) {
        const bdem_Descriptor *elemAttr = attrLookupTbl[elementTypes[i]];
        int align  = elemAttr->d_alignment;
        int offset = (d_totalOffset + align - 1) & ~(align - 1);
        d_map[i].d_elemAttr = elemAttr;
        d_map[i].d_offset = offset;
        d_totalOffset = offset + elemAttr->d_size;
    }

    compact();  // d_offsets, d_totalOffset set by compact()
}

void bdem_IdxOffsetMap::updateTotalOffset()
{
    int  myLength       = length();
    int  newTotalOffset = 0;

    for (int i = 0; i < myLength; ++i) {
        int offset = d_map[i].d_offset;
        if (offset >= newTotalOffset) {
            const bdem_Descriptor *elemAttr = d_map[i].d_elemAttr;
            newTotalOffset = offset + elemAttr->d_size;
        }
    }
    d_totalOffset = newTotalOffset;
}

                             // ---------
                             // ACCESSORS
                             // ---------

int bdem_IdxOffsetMap::areElemTypesEqual(const bdem_IdxOffsetMap& other) const
{
    int myLength = length();

    if (myLength != other.length()) {
        return 0;
    }

    for (int i = 0; i < myLength; ++i) {
        if (d_map[i].d_elemAttr->d_elemEnum !=
                                       other.d_map[i].d_elemAttr->d_elemEnum) {
            return 0;
        }
    }

    return 1;
}

std::ostream& bdem_IdxOffsetMap::print(std::ostream& stream,
                                       int           level,
                                       int           spacesPerLevel) const
{
    bdeu_Print::indent(stream, level, spacesPerLevel);
    stream << "{\n";

    if (level < 0) {
        level = -level;
    }

    int levelPlus1 = level + 1;
    int myLength = length();
    for (int i = 0; i < myLength; ++i) {
        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        bdem_ElemType::Type elemEnum =
            (bdem_ElemType::Type) d_map[i].d_elemAttr->d_elemEnum;
        stream << elemEnum << ' ' << d_map[i].d_offset << '\n';
    }

    bdeu_Print::indent(stream, level, spacesPerLevel);
    return stream << "}\n";
}

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2002
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
