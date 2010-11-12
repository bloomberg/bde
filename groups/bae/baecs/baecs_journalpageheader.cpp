// baecs_journalpageheader.cpp -*-C++-*-

#include <baecs_journalpageheader.h>

#include <bael_log.h>

#include <bsls_assert.h>

#include <bsl_cstdio.h>

namespace BloombergLP {

namespace {

const char LOG_CATEGORY[] = "BAECS.JOURNALPAGEHEADER";

}  // close unnamed namespace

#define STREAMOUT(x) << " " #x " = " << x

#define J_ASSERT(cond) { if(!(cond)) { \
    BAEL_LOG_SET_CATEGORY(LOG_CATEGORY); \
    BAEL_LOG_ERROR << "[assertion failed: " << #cond << "]" \
                   << BAEL_LOG_END; \
    result = -1; if (!verbose) { return result; } }  }

#define J_ASSERT1(cond, v1) { if(!(cond)) { \
    BAEL_LOG_SET_CATEGORY(LOG_CATEGORY); \
    BAEL_LOG_ERROR << "[assertion failed: " << #cond \
                   << "|" << #v1 << " == " << (v1) << "]" \
                   << BAEL_LOG_END; \
    result = -1; if (!verbose) { return result; } }  }

#define J_ASSERT2(cond, v1, v2) { if(!(cond)) { \
    BAEL_LOG_SET_CATEGORY(LOG_CATEGORY); \
    BAEL_LOG_ERROR << "[assertion failed: " << #cond \
                   << "|" << #v1 << " == " << (v1) << "]" \
                   << "|" << #v2 << " == " << (v2) << "]" \
                   << BAEL_LOG_END; \
    result = -1; if (!verbose) { return result; } }  }

void baecs_JournalPageHeader::print(bsl::ostream& stream) const
{
    stream STREAMOUT(d_transactionId)
           STREAMOUT(d_numBlocks)
           STREAMOUT(d_numAvailableBlocks)
           STREAMOUT(d_numReleasedBlocks)
           STREAMOUT(d_firstAvailableBlock)
           STREAMOUT(d_firstReleasedBlock)
           STREAMOUT(d_lastReleasedBlock)
           STREAMOUT(d_isOnFillList)
           STREAMOUT(d_nextPage);

    stream << "\nBlocks [index: nextRecord\tprevRecord\tnextBlock"
           << "\tAttributes\tFlags]:\n";
    for(unsigned i=0; i<d_numBlocks; ++i) {
        stream << "[" << i << ":"
            << d_blocks[i]
            << "]" << bsl::endl;
    }
    stream << "\n";
}

int baecs_JournalPageHeader::markBlockListForValidation(unsigned index,
                                                        unsigned numBlocks,
                                                        unsigned blockType,
                                                        unsigned *map,
                                                        bool verbose,
                                                        unsigned offset) const
{
    int result = 0;

    unsigned firstBlock = BAECS_INDEX_NONE, lastBlock = BAECS_INDEX_NONE;
    for(unsigned i=0; i<numBlocks; ++i) {
        J_ASSERT1(index < d_numBlocks, index);
        unsigned globalIndex = index + offset;
        J_ASSERT2(map[globalIndex] == (unsigned)-1,
                  map[globalIndex],
                  globalIndex);
        map[globalIndex] = blockType;
        J_ASSERT2(map[globalIndex] != (unsigned)-1,
                  map[globalIndex],
                  globalIndex);

        if (verbose) {
            if (firstBlock == BAECS_INDEX_NONE) {
                firstBlock = globalIndex;
                lastBlock = globalIndex - 1;
            }
            if (globalIndex != lastBlock + 1) {
                if (firstBlock != lastBlock) {
                    bsl::printf(" [%u/%u, %u/%u] ",
                                firstBlock,
                                (firstBlock - offset),
                                lastBlock,
                                (lastBlock - offset));
                    bsl::fflush(stdout);
                }
                else {
                    bsl::printf(" [%u/%u] ",
                                firstBlock,
                                (firstBlock - offset));
                    bsl::fflush(stdout);
                }
                firstBlock = globalIndex;
                lastBlock = firstBlock;
            }
            else {
                lastBlock = globalIndex;
            }

        }
        index = d_blocks[index].nextBlock();
    }

    if (verbose && firstBlock != BAECS_INDEX_NONE) {
        if (firstBlock != lastBlock) {
            bsl::printf(" [%u/%u, %u/%u] ",
                        firstBlock,
                        (firstBlock - offset),
                        lastBlock,
                        (lastBlock - offset));
            bsl::fflush(stdout);
        }
        else {
            bsl::printf(" [%u/%u] ", firstBlock, (firstBlock - offset));
            bsl::fflush(stdout);
        }
        bsl::putchar('\n');
    }
    J_ASSERT1(index == BAECS_INDEX_NONE, index);
    return result;
}

int baecs_JournalPageHeader::markFreeListsForValidation(unsigned *map,
                                                        bool verbose,
                                                        unsigned offset) const
{
    BAEL_LOG_SET_CATEGORY(LOG_CATEGORY);
    int result = 0;

    if (d_numAvailableBlocks == 0 && d_numReleasedBlocks == 0)
    {
        return result;
    }

    if (verbose) {
        bsl::printf("Offset %d", offset);
        if (d_numAvailableBlocks) {
            bsl::printf("\tAvailable blocks (%u):",
                        (unsigned int) d_numAvailableBlocks);
        }
    }

    markBlockListForValidation(d_firstAvailableBlock, d_numAvailableBlocks,
                               (unsigned) -2, map, verbose, offset);

    if (verbose) {
        if (d_numReleasedBlocks) {
            bsl::printf("\t\tReleased blocks (%u):",
                        (unsigned int) d_numReleasedBlocks);
        }
    }

    markBlockListForValidation(d_firstReleasedBlock, d_numReleasedBlocks,
                               (unsigned) -3, map, verbose, offset);

    return result;
}

void baecs_JournalPageHeader::copyFrom(
                                 const baecs_JournalPageHeader &other,
                                 const bsls_PlatformUtil::Int64 &transactionId)
{
    BSLS_ASSERT(d_numBlocks == other.d_numBlocks);
    d_transactionId.setValue(transactionId);
    d_numAvailableBlocks    = other.d_numAvailableBlocks;
    d_numReleasedBlocks     = other.d_numReleasedBlocks;
    d_firstAvailableBlock   = other.d_firstAvailableBlock;
    d_firstReleasedBlock    = other.d_firstReleasedBlock;
    d_lastReleasedBlock     = other.d_lastReleasedBlock;
    d_isOnFillList          = other.d_isOnFillList;
    d_nextPage              = other.d_nextPage;
    bsl::memcpy(d_blocks, other.d_blocks, sizeof(BlockHeader)*d_numBlocks);

}

void baecs_JournalPageHeader::init(
                                unsigned numBlocks,
                                const bsls_PlatformUtil::Int64 &transactionId)
{
    d_transactionId.setValue(transactionId);
    d_firstAvailableBlock = 0;
    d_numAvailableBlocks  = numBlocks;
    d_numBlocks = numBlocks;
    d_firstReleasedBlock = BAECS_INDEX_NONE;
    d_lastReleasedBlock = BAECS_INDEX_NONE;
    d_numReleasedBlocks = 0;
    d_isOnFillList = false;
    d_nextPage = BAECS_INVALID_RECORD_HANDLE;

    for(unsigned i=0; i<d_numBlocks; i++) {
        d_blocks[i].init();
        d_blocks[i].setNextBlock(i+1);
    }
    d_blocks[d_numBlocks-1].setNextBlock(BAECS_INDEX_NONE);
}

unsigned baecs_JournalPageHeader::getSize(unsigned numBlocks)
{
    BSLS_ASSERT(numBlocks > 0);
    return sizeof(baecs_JournalPageHeader)
        + sizeof(BlockHeader)*(numBlocks-1);
}

unsigned baecs_JournalPageHeader::allocate(unsigned numBlocks,
                                            unsigned attributes)
{
    BSLS_ASSERT(numBlocks <= d_numAvailableBlocks);
    BSLS_ASSERT(numBlocks > 0);

    // get the first free block (will be the head of segment)
    unsigned first = d_firstAvailableBlock;
    // iterate to the last block of the segment we want to allocate
    unsigned last = first;
    for(unsigned i=0; i<numBlocks-1; ++i) {
        BSLS_ASSERT(last < d_numBlocks);
        last = d_blocks[last].nextBlock();
    }
    BSLS_ASSERT(last < d_numBlocks);
    // in the head, set up the pointer to last block and flags
    d_blocks[first].setHead(false);
    d_blocks[first].setExtended(false);
    d_blocks[first].setConfirmed(false);
    d_blocks[first].setAttributes(attributes);
    // chop off the blocks from the beginning of free block list
    d_firstAvailableBlock = d_blocks[last].nextBlock();
    d_blocks[last].setNextBlock(BAECS_INDEX_NONE);
    // decrement the free block counter
    BSLS_ASSERT(d_numAvailableBlocks >= numBlocks);
    d_numAvailableBlocks = d_numAvailableBlocks - numBlocks;
    if (!d_numAvailableBlocks) d_firstAvailableBlock = BAECS_INDEX_NONE;
    return first;
}

int baecs_JournalPageHeader::release(unsigned first)
{
    BAEL_LOG_SET_CATEGORY(LOG_CATEGORY);

    unsigned last = first;
    unsigned numBlocks = 1;
    while(d_blocks[last].nextBlock() != BAECS_INDEX_NONE) {
        ++numBlocks;
        last = d_blocks[last].nextBlock();
    }
//    BSLS_ASSERT(numFreeBlocks() + numBlocks <= d_numBlocks);
    if (numFreeBlocks() + numBlocks > d_numBlocks) {
        return numBlocks;
    }

    d_blocks[first].setNextRecord(BAECS_INVALID_RECORD_HANDLE); // TBD: ????
    d_blocks[first].setPrevRecord(BAECS_INVALID_RECORD_HANDLE);

    d_numReleasedBlocks = d_numReleasedBlocks + numBlocks;

    BAEL_LOG_TRACE << "Release @" << this << " nrb="
        << d_numReleasedBlocks << BAEL_LOG_END;

    if (d_lastReleasedBlock == BAECS_INDEX_NONE) d_lastReleasedBlock = last;
    d_blocks[last].setNextBlock(d_firstReleasedBlock);
    d_firstReleasedBlock = first;
    return 0;
}

int baecs_JournalPageHeader::deallocate(unsigned first) {
    unsigned last = first;
    unsigned numBlocks = 1;
    while(d_blocks[last].nextBlock() != BAECS_INDEX_NONE) {
        ++numBlocks;
        last = d_blocks[last].nextBlock();
    }
//    BSLS_ASSERT(numFreeBlocks() + numBlocks <= d_numBlocks);
    if (numFreeBlocks() + numBlocks > d_numBlocks) {
        return numBlocks;
    }
    d_blocks[first].setNextRecord(BAECS_INVALID_RECORD_HANDLE); // TBD: ???
    d_blocks[first].setPrevRecord(BAECS_INVALID_RECORD_HANDLE);
    d_numAvailableBlocks = d_numAvailableBlocks + numBlocks;
    d_blocks[last].setNextBlock(d_firstAvailableBlock);
    d_firstAvailableBlock = first;
    return 0;
}

int baecs_JournalPageHeader::purge() {
    BAEL_LOG_SET_CATEGORY(LOG_CATEGORY);

    BAEL_LOG_TRACE << "Purge @" << this << " nrb="
        << d_numReleasedBlocks <<  BAEL_LOG_END;

    if (d_firstReleasedBlock == BAECS_INDEX_NONE) {
        BSLS_ASSERT(d_numReleasedBlocks == 0);
        BSLS_ASSERT(d_lastReleasedBlock == BAECS_INDEX_NONE);
    } else {
        BSLS_ASSERT(d_lastReleasedBlock != BAECS_INDEX_NONE);
        BSLS_ASSERT(d_numReleasedBlocks > 0);
        d_blocks[d_lastReleasedBlock].setNextBlock(d_firstAvailableBlock);
        d_firstAvailableBlock = d_firstReleasedBlock;
        d_firstReleasedBlock = BAECS_INDEX_NONE;
        d_lastReleasedBlock = BAECS_INDEX_NONE;
        d_numAvailableBlocks = (unsigned)d_numAvailableBlocks
                             + d_numReleasedBlocks;
        d_numReleasedBlocks = 0;
    }
    return 0;
}

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2007
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
