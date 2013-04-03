// bcema_protectablesequentialallocator.cpp                           -*-C++-*-
#include <bcema_protectablesequentialallocator.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bcema_protectablesequentialallocator_cpp,"$Id$ $CSID$")

#include <bcema_testallocator.h>     // for testing only

#include <bdema_bufferimputil.h>
#include <bdema_memoryblockdescriptor.h>
#include <bdema_nativeprotectableblockdispenser.h>
#include <bdema_protectableblockdispenser.h>
#include <bdema_protectableblocklist.h>

#include <bsls_assert.h>
#include <bsls_types.h>

#include <bsl_cstdlib.h>      // for 'bsl::abs'
#include <bsl_limits.h>       // for 'bsl::numeric_limits'

namespace BloombergLP {

namespace {

// STATIC HELPER FUNCTIONS
static inline
bcema_ProtectableSequentialAllocator::size_type roundUp(
                      bcema_ProtectableSequentialAllocator::size_type value,
                      bcema_ProtectableSequentialAllocator::size_type multiple)
    // Return the specified 'value' rounded up to the next highest multiple of
    // the specified 'multiple'
{
    return value == 0 ? 0 : multiple * (1 + ((value - 1) / multiple));
}

}  // close unnamed namespace

                  // ------------------------------------------
                  // class bcema_ProtectableSequentialAllocator
                  // ------------------------------------------

// PRIVATE MANIPULATORS
void *bcema_ProtectableSequentialAllocator::allocateWithoutLock(size_type size)
{
    if (0 == size) {
        return 0;                                                     // RETURN
    }

    // Attempt to allocate memory from the current open buffer 'd_buffer'.

    void *memory = d_buffer
                 ? bdema_BufferImpUtil::allocateFromBuffer(&d_cursor,
                                                           d_buffer,
                                                           d_bufSize,
                                                           size,
                                                           d_strategy) : 0;

    if (memory) {  // Current buffer sufficient.
        return memory;                                                // RETURN
    }

    // Accounting for the block size and the block header size ensures that
    // allocations occur on regular page boundaries, enabling predictable
    // memory usage for white-box testing.

    const int HEADER   = bdema_ProtectableBlockList::blockHeaderSize();
    const int BLK_SIZE = d_dispenser_p->minimumBlockSize();

    BSLS_ASSERT(HEADER <= BLK_SIZE);

    size_type allocSize = (0 == d_size ? BLK_SIZE : d_size) - HEADER;

    if (d_growthStrategy == bsls::BlockGrowth::BSLS_GEOMETRIC) {
        while (allocSize < size && allocSize <= d_growthLimit) {
            allocSize += allocSize + HEADER;  // Double allocation while still
                                              // accounting for the header
                                              // size.
        }
    }

    if (d_growthStrategy == bsls::BlockGrowth::BSLS_CONSTANT ||
                                                   allocSize > d_growthLimit) {

        // If the allocation's size is greater than the max buffer size, but
        // still not enough to satisfy the request, allocate the requested
        // size; otherwise, allocate the max buffer size.

        size_type bufferLimit = d_growthLimit;
        const int allocEstimate = size > bufferLimit ? size : bufferLimit;
        allocSize = roundUp(allocEstimate + HEADER, BLK_SIZE) - HEADER;
    }

    BSLS_ASSERT(0 == (allocSize + HEADER) % BLK_SIZE);

    // Keep track of the actual size (w/ header).

    d_size += allocSize + HEADER;

    // Use the block list to allocate the memory.  Set 'd_bufSize' to the
    // actual amount of memory allocated.

    bdema_MemoryBlockDescriptor block = d_blockList.allocate(allocSize);
    d_bufSize = block.size();
    d_buffer  = static_cast<char *>(block.address());
    d_cursor  = 0;

    return bdema_BufferImpUtil::allocateFromBuffer(&d_cursor,
                                                   d_buffer,
                                                   d_bufSize,
                                                   size,
                                                   d_strategy);
}

// CREATORS
bcema_ProtectableSequentialAllocator::
bcema_ProtectableSequentialAllocator(
                               bdema_ProtectableBlockDispenser *blockDispenser)
: d_cursor(0)
, d_buffer(0)
, d_bufSize(0)
, d_growthLimit(bsl::numeric_limits<size_type>::max())
, d_growthStrategy(bsls::BlockGrowth::BSLS_GEOMETRIC)
, d_size(0)
, d_strategy(bsls::Alignment::BSLS_NATURAL)
, d_blockList(
          blockDispenser ? blockDispenser
                         : &bdema_NativeProtectableBlockDispenser::singleton())
, d_dispenser_p(
          blockDispenser ? blockDispenser
                         : &bdema_NativeProtectableBlockDispenser::singleton())
{
}

bcema_ProtectableSequentialAllocator::
bcema_ProtectableSequentialAllocator(
                               bsls::Alignment::Strategy        strategy,
                               bdema_ProtectableBlockDispenser *blockDispenser)
: d_cursor(0)
, d_buffer(0)
, d_bufSize(0)
, d_growthLimit(bsl::numeric_limits<size_type>::max())
, d_growthStrategy(bsls::BlockGrowth::BSLS_GEOMETRIC)
, d_size(0)
, d_strategy(strategy)
, d_blockList(
          blockDispenser ? blockDispenser
                         : &bdema_NativeProtectableBlockDispenser::singleton())
, d_dispenser_p(
          blockDispenser ? blockDispenser
                         : &bdema_NativeProtectableBlockDispenser::singleton())
{
}

bcema_ProtectableSequentialAllocator::
bcema_ProtectableSequentialAllocator(
                          bsls::Alignment::Strategy        strategy,
                          bsls::BlockGrowth::Strategy      growthStrategy,
                          size_type                        bufferExpansionSize,
                          bdema_ProtectableBlockDispenser *blockDispenser)
: d_cursor(0)
, d_buffer(0)
, d_bufSize(0)
, d_growthLimit(bufferExpansionSize ? bufferExpansionSize
                                    : bsl::numeric_limits<size_type>::max())
, d_growthStrategy(growthStrategy)
, d_size(0)
, d_strategy(strategy)
, d_blockList(
          blockDispenser ? blockDispenser
                         : &bdema_NativeProtectableBlockDispenser::singleton())
, d_dispenser_p(
          blockDispenser ? blockDispenser
                         : &bdema_NativeProtectableBlockDispenser::singleton())
{
}

bcema_ProtectableSequentialAllocator::~bcema_ProtectableSequentialAllocator()
{
}

// MANIPULATORS
void *bcema_ProtectableSequentialAllocator::allocate(size_type numBytes)
{
    bcemt_LockGuard<bcemt_Mutex> guard(&d_mutex);
    return allocateWithoutLock(numBytes);
}

void bcema_ProtectableSequentialAllocator::deallocate(void *)
{
}

void bcema_ProtectableSequentialAllocator::release()
{
    bcemt_LockGuard<bcemt_Mutex> guard(&d_mutex);
    d_blockList.release();
    d_cursor  = 0;
    d_buffer  = 0;
    d_bufSize = 0;
    d_size    = 0;
}

bsls::Types::size_type bcema_ProtectableSequentialAllocator::expand(
                                      void                   *address,
                                      bsls::Types::size_type  originalNumBytes,
                                      bsls::Types::size_type  maxNumBytes)
{
    bcemt_LockGuard<bcemt_Mutex> guard(&d_mutex);

    size_type size = originalNumBytes;
    if ((char *)address + originalNumBytes == d_buffer + d_cursor) {

        BSLS_ASSERT(d_cursor < 0 ||
                    (size_type) d_cursor <= originalNumBytes + d_bufSize);

        size_type newNumBytes = originalNumBytes + d_bufSize - d_cursor;
        if (maxNumBytes && maxNumBytes < newNumBytes) {
            newNumBytes = maxNumBytes;
        }
        d_cursor = d_cursor + newNumBytes - originalNumBytes;
        size = newNumBytes;
    }

    return size;
}

void bcema_ProtectableSequentialAllocator::reserveCapacity(size_type numBytes)
{
    bcemt_LockGuard<bcemt_Mutex> guard(&d_mutex);

    if (numBytes) {
        // Test if the current buffer has enough space for 'numBytes'.

        int cursor = d_cursor;
        void *memory = d_buffer
                     ? bdema_BufferImpUtil::allocateFromBuffer(&cursor,
                                                               d_buffer,
                                                               d_bufSize,
                                                               numBytes,
                                                               d_strategy) : 0;
        if (0 == memory) {
            // If it does not, perform an allocation and then reset the cursor
            // to 0.

            allocateWithoutLock(numBytes);
            d_cursor = 0;
        }
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
