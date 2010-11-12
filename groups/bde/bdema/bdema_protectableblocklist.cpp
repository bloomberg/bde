// bdema_protectableblocklist.cpp    -*-C++-*-
#include <bdema_protectableblocklist.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bdema_protectableblocklist_cpp,"$Id$ $CSID$")

#ifdef TEST
#include <bslma_testallocator.h>                  // for testing only
#include <bslma_testallocatorexception.h>         // for testing only
#include <bdema_testprotectableblockdispenser.h>  // for testing only
#endif

#include <bsls_assert.h>

namespace BloombergLP {

                   // --------------------------------
                   // class bdema_ProtectableBlockList
                   // --------------------------------

// CLASS METHODS
int bdema_ProtectableBlockList::blockHeaderSize()
{
    return BLOCK_HEADER_SIZE;
}

// CREATORS
bdema_ProtectableBlockList::~bdema_ProtectableBlockList()
{
    release();
}

// MANIPULATORS
bdema_MemoryBlockDescriptor bdema_ProtectableBlockList::allocate(int numBytes)
{
    BSLS_ASSERT(0 <= numBytes);

    bdema_MemoryBlockDescriptor desc;

    if (0 == numBytes) {
        return desc;
    }

    // Add to the requested size to account for the size of the block header.

    numBytes += BLOCK_HEADER_SIZE;

    // Allocate and initialize the block header information.

    desc = d_dispenser_p->allocate(numBytes);
    BSLS_ASSERT(!desc.isNull());

    Block *block = (Block *)desc.address();
    block->d_size = desc.size();

    // Point the next pointer to the current head.

    block->d_next_p = d_head_p;

    // The soon-to-be head has no previous block.

    block->d_prev_p = 0;

    // Point the previous pointer of the existing head to our new block.

    if (d_head_p) {
        d_head_p->d_prev_p = block;
    }

    // Point the head to the new block.

    d_head_p = block;

    // The actual allocated memory is the allocated size minus the size of the
    // block header.

    desc.setAddressAndSize(static_cast<void *>(&block->d_memory),
                           block->d_size - BLOCK_HEADER_SIZE);
    return desc;
}

void bdema_ProtectableBlockList::deallocate(void *address)
{
    if (address) {
        Block *block = (Block *)(void *)((char *)address - BLOCK_HEADER_SIZE);

        // Point the previous block's next pointer to the deleted block's next
        // pointer.

        if (block->d_prev_p) {
            block->d_prev_p->d_next_p = block->d_next_p;
        } else {
            d_head_p = block->d_next_p;
        }

        // Point the next block's previous pointer to this block's previous
        // pointer.

        if (block->d_next_p) {
            block->d_next_p->d_prev_p = block->d_prev_p;
        }

        // Free the block.

        d_dispenser_p->deallocate(descriptor(block));
    }
}

void bdema_ProtectableBlockList::release()
{
    unprotect();
    while (d_head_p) {
        Block *lastBlock = d_head_p;
        d_head_p = d_head_p->d_next_p;
        d_dispenser_p->deallocate(descriptor(lastBlock));
    }
}

void bdema_ProtectableBlockList::protect()
{
    if (!d_isProtected) {
        Block *block = d_head_p;
        while (block) {
            const int ret = d_dispenser_p->protect(descriptor(block));
            BSLS_ASSERT(0 == ret);
            block = block->d_next_p;
        }
        d_isProtected = true;
    }
}

void bdema_ProtectableBlockList::unprotect()
{
    if (d_isProtected) {
        Block *block = d_head_p;
        while (block) {
            const int ret = d_dispenser_p->unprotect(descriptor(block));
            BSLS_ASSERT(0 == ret);
            block = block->d_next_p;
        }
        d_isProtected = false;
    }
}

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2008
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
