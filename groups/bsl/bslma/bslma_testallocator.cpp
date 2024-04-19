// bslma_testallocator.cpp                                            -*-C++-*-
#include <bslma_testallocator.h>

#include <bsls_ident.h>
BSLS_IDENT("$Id$ $CSID$")

#include <bslma_mallocfreeallocator.h>

#include <bsls_alignment.h>
#include <bsls_alignmentutil.h>
#include <bsls_assert.h>
#include <bsls_bslexceptionutil.h>
#include <bsls_platform.h>
#include <bsls_bsltestutil.h>

#include <cstdio>    // 'std::printf'
#include <cstdlib>   // 'std::abort'
#include <cstring>   // 'std::memset'

namespace BloombergLP {
namespace bslma {

                    // ================================
                    // struct TestAllocator_BlockHeader
                    // ================================

struct TestAllocator_BlockHeader {
    // This 'struct' holds the attributes for a block of allocated memory and
    // immediately preceeds the block's *user segment* (i.e., the portion of
    // the block that is returned to the user).  The headers for all currently
    // allocated blocks are linked together into a list; the 'TestAllocator'
    // class holds pointers to the head and tail of that list.

    unsigned int  d_magicNumber;  // allocated/deallocated/other identifier

    TestAllocator_BlockHeader
                 *d_prev_p;       // header of previous allocated block
    TestAllocator_BlockHeader
                 *d_next_p;       // header of next allocated block

    bslma::Allocator::size_type
                  d_bytes;        // number of available bytes in this block

    bsls::Types::Int64
                  d_id;           // allocation sequence number: a unique,
                                  // never-reused, ID within this allocator

    bslma::Allocator
                 *d_self_p;       // address of test allocator from which this
                                  // block was allocated

    bsls::AlignmentUtil::MaxAlignedType
                  d_sentinel;     // sentinel before user segment used to
                                  // detect underrun memory corruption --
                                  // guaranteed to extend to the end of this
                                  // 'struct'
};

}  // Close package namespace

namespace {

typedef unsigned char                    Uchar;
typedef bslma::Allocator::size_type      size_type;
typedef bslma::TestAllocator_BlockHeader BlockHeader;

const unsigned int k_ALLOCATED_MEMORY_MAGIC_NUMBER =
                                        0xDEADBEEF; // magic number identifying
                                                    // memory allocated and not
                                                    // yet deallocated

const unsigned int k_DEALLOCATED_MEMORY_MAGIC_NUMBER =
                                        0xDEADF00D; // magic number identifying
                                                    // deallocated memory

const Uchar k_DEALLOCATED_BYTE        = 0xA5;       // byte used to scribble
                                                    // over deallocated memory

const Uchar k_SENTINEL_BYTE           = 0xB1;       // byte used to write over
                                                    // newly-allocated memory
                                                    // and pads

const std::ptrdiff_t k_SENTINEL_SIZE   =
    sizeof(bsls::AlignmentUtil::MaxAlignedType);    // size of the sentinel
                                                    // before and after the
                                                    // user segment

const std::size_t k_MAX_ALIGNMENT = bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT;

#define ZU      BSLS_BSLTESTUTIL_FORMAT_ZU  // printf format str for 'size_t'
#define FMT_I64 BSLS_BSLTESTUTIL_FORMAT_I64 // printf format str for 'Int64'

inline bool isAligned(const void *address, std::size_t alignment)
    // Return 'true' if the specified 'address' is aligned on the specified
    // 'alignment' or 'false' otherwise.
{
    return 0 == bsls::AlignmentUtil::calculateAlignmentOffset(address,
                                                              int(alignment));
}

void formatBlock(const void *address, std::ptrdiff_t length)
    // Format in hex to 'stdout', a block of memory starting at the specified
    // starting 'address' of the specified 'length' (in bytes).  Each line of
    // formatted output will have a maximum of 16 bytes per line, where each
    // line starts with the address of that 16-byte chunk.
{
    const Uchar *addr    = static_cast<const unsigned char *>(address);
    const Uchar *endAddr = addr + length;

    for (int i = 0; addr < endAddr; ++i) {
        if (0 == i % 4) {
            if (i) {
                std::printf("\n");
            }
            std::printf("%p:\t", static_cast<const void *>(addr));
        }
        else {
            std::printf("  ");
        }

        for (int j = 0; j < 4 && addr < endAddr; ++j) {
            std::printf("%02x ", *addr);
            ++addr;
        }
    }

    std::printf("\n");
}

void formatBlockHeader(const BlockHeader *address)
{
    printf("  Magic num: 0x%x, Id: " FMT_I64 ", Self: %p\n",
           address->d_magicNumber, address->d_id, address->d_self_p);
    printf("  User segment addr: %p, num bytes: " ZU "\n",
           address + 1, address->d_bytes);
    printf("  Prefix sentinel:\n    ");
    formatBlock(&address->d_sentinel, k_SENTINEL_SIZE);
}

void formatInvalidMemoryBlock(const BlockHeader    *address,
                              bslma::TestAllocator *allocator,
                              int                   underrunBy,
                              int                   overrunBy)
    // Format the contents of the presumably invalid memory block at the
    // specified 'address' to 'stdout', using the specified 'allocator',
    // 'underrunBy', and 'overrunBy' information.  A suitable error message, if
    // appropriate, is printed first, followed by the fields in the header.
    // Finally, up to the first 64 bytes of memory of the "payload" portion of
    // the allocated memory is printed; if fewer than 64 bytes were requested,
    // the dump will include some of the trailing sentinel.
{
    unsigned int  magicNumber = address->d_magicNumber;
    size_type     numBytes    = address->d_bytes;
    const Uchar  *payload     = reinterpret_cast<const Uchar *>(address + 1);

    if (k_ALLOCATED_MEMORY_MAGIC_NUMBER != magicNumber) {
        if (k_DEALLOCATED_MEMORY_MAGIC_NUMBER == magicNumber) {
            std::printf("*** Deallocating previously deallocated memory at"
                        " %p. ***\n",
                        static_cast<const void *>(payload));
        }
        else {
            std::printf("*** Invalid magic number 0x%08x at address %p. ***\n",
                        magicNumber,
                        static_cast<const void *>(payload));
        }
    }
    else if (numBytes <= 0) {
        std::printf("*** Invalid (non-positive) byte count " ZU " at address"
                    " %p. *** \n",
                    numBytes,
                    static_cast<const void *>(payload));
    }
    else if (allocator != address->d_self_p) {
        std::printf("*** Freeing segment at %p from wrong allocator. ***\n",
                    static_cast<const void *>(payload));
    }
    else if (underrunBy) {
        std::printf("*** Memory corrupted at %d bytes before " ZU " byte"
                    " segment at %p. ***\n",
                    underrunBy,
                    numBytes,
                    static_cast<const void *>(payload));

        std::printf("Pad area before user segment:\n");
        formatBlock(payload - k_SENTINEL_SIZE, k_SENTINEL_SIZE);
    }
    else if (overrunBy) {
        std::printf("*** Memory corrupted at %d bytes after " ZU " byte"
                    " segment at %p. ***\n",
                    overrunBy,
                    numBytes,
                    static_cast<const void *>(payload));

        std::printf("Pad area after user segment:\n");
        formatBlock(payload + numBytes, k_SENTINEL_SIZE);
    }

    std::printf("Header:\n");
    formatBlockHeader(address);
    std::printf("User segment:\n");
    formatBlock(payload, 64);
}

void printIdList(const BlockHeader *blockListHead_p)
    // Traverse the linked list whose head is the specified `blockListHead_p`
    // and print the ID of each block in the list.
{
    const BlockHeader *currBlock_p = blockListHead_p;

    while (currBlock_p) {
        // Print up to 8 IDs on one line, separated by tabs.
        for (int i = 0; i < 8 && currBlock_p; ++i) {
            std::printf(FMT_I64 "\t", currBlock_p->d_id);
            currBlock_p = currBlock_p->d_next_p;
        }

        // Add a newline between every set of 8 IDs.  The space after the '\n'
        // is needed to align these IDs properly with the current output.
        std::printf("\n ");
    }
}

}  // close unnamed namespace

namespace bslma {

                        // -------------------
                        // class TestAllocator
                        // -------------------

// CREATORS
TestAllocator::TestAllocator(Allocator *basicAllocator)
: d_name_p(0)
, d_noAbortFlag(false)
, d_quietFlag(false)
, d_verboseFlag(false)
, d_allocationLimit(-1)
, d_numAllocations(0)
, d_numDeallocations(0)
, d_numMismatches(0)
, d_numBoundsErrors(0)
, d_numBlocksInUse(0)
, d_numBytesInUse(0)
, d_numBlocksMax(0)
, d_numBytesMax(0)
, d_numBlocksTotal(0)
, d_numBytesTotal(0)
, d_lastAllocatedNumBytes(0)
, d_lastDeallocatedNumBytes(0)
, d_lastAllocatedAddress_p(0)
, d_lastDeallocatedAddress_p(0)
, d_blockListHead_p(0)
, d_blockListTail_p(0)
, d_allocator_p(basicAllocator
                ? basicAllocator
                : &MallocFreeAllocator::singleton())
{
    BSLS_ASSERT(d_allocator_p);
}

TestAllocator::TestAllocator(bool verboseFlag, Allocator *basicAllocator)
: d_name_p(0)
, d_noAbortFlag(false)
, d_quietFlag(false)
, d_verboseFlag(verboseFlag)
, d_allocationLimit(-1)
, d_numAllocations(0)
, d_numDeallocations(0)
, d_numMismatches(0)
, d_numBoundsErrors(0)
, d_numBlocksInUse(0)
, d_numBytesInUse(0)
, d_numBlocksMax(0)
, d_numBytesMax(0)
, d_numBlocksTotal(0)
, d_numBytesTotal(0)
, d_lastAllocatedNumBytes(0)
, d_lastDeallocatedNumBytes(0)
, d_lastAllocatedAddress_p(0)
, d_lastDeallocatedAddress_p(0)
, d_blockListHead_p(0)
, d_blockListTail_p(0)
, d_allocator_p(basicAllocator
                ? basicAllocator
                : &MallocFreeAllocator::singleton())
{
    BSLS_ASSERT(d_allocator_p);
}

TestAllocator::TestAllocator(const char *name, Allocator *basicAllocator)
: d_name_p(name)
, d_noAbortFlag(false)
, d_quietFlag(false)
, d_verboseFlag(false)
, d_allocationLimit(-1)
, d_numAllocations(0)
, d_numDeallocations(0)
, d_numMismatches(0)
, d_numBoundsErrors(0)
, d_numBlocksInUse(0)
, d_numBytesInUse(0)
, d_numBlocksMax(0)
, d_numBytesMax(0)
, d_numBlocksTotal(0)
, d_numBytesTotal(0)
, d_lastAllocatedNumBytes(0)
, d_lastDeallocatedNumBytes(0)
, d_lastAllocatedAddress_p(0)
, d_lastDeallocatedAddress_p(0)
, d_blockListHead_p(0)
, d_blockListTail_p(0)
, d_allocator_p(basicAllocator
                ? basicAllocator
                : &MallocFreeAllocator::singleton())
{
    BSLS_ASSERT(d_allocator_p);
}

TestAllocator::TestAllocator(const char *name,
                             bool        verboseFlag,
                             Allocator  *basicAllocator)
: d_name_p(name)
, d_noAbortFlag(false)
, d_quietFlag(false)
, d_verboseFlag(verboseFlag)
, d_allocationLimit(-1)
, d_numAllocations(0)
, d_numDeallocations(0)
, d_numMismatches(0)
, d_numBoundsErrors(0)
, d_numBlocksInUse(0)
, d_numBytesInUse(0)
, d_numBlocksMax(0)
, d_numBytesMax(0)
, d_numBlocksTotal(0)
, d_numBytesTotal(0)
, d_lastAllocatedNumBytes(0)
, d_lastDeallocatedNumBytes(0)
, d_lastAllocatedAddress_p(0)
, d_lastDeallocatedAddress_p(0)
, d_blockListHead_p(0)
, d_blockListTail_p(0)
, d_allocator_p(basicAllocator
                ? basicAllocator
                : &MallocFreeAllocator::singleton())
{
    BSLS_ASSERT(d_allocator_p);
}

TestAllocator::~TestAllocator()
{
    BSLS_ASSERT(d_allocator_p);

    if (isVerbose()) {
        print();
    }

    // We must not delete outstanding blocks here.  If 'd_blockListHead_p' is
    // not null, the leak might have been caused by memory content corruption.
    // By default, the leak will cause a call to 'abort', allowing for
    // diagnosis in the debugger.

    if (!isQuiet()) {
        if (numBytesInUse() || numBlocksInUse()) {
            std::printf("MEMORY_LEAK");
            if (d_name_p) {
                std::printf(" from %s", d_name_p);
            }
            std::printf(":\n  Number of blocks in use = " FMT_I64 "\n"
                        "   Number of bytes in use = " FMT_I64 "\n",
                        numBlocksInUse(), numBytesInUse());
            std::fflush(stdout);

            if (!isNoAbort()) {
                std::abort();                                         // ABORT
            }
        }
    }
}

// MANIPULATORS
void *TestAllocator::allocate(size_type size)
{
    BlockHeader *header_p = 0;
    if (size > 0) {
        // Perform this allocation before acquiring a mutex, so as to not hold
        // onto the mutex over a potentially expensive operation.
        const std::size_t totalBytes =
            bsls::AlignmentUtil::roundUpToMaximalAlignment(
                                 sizeof(BlockHeader) + size + k_SENTINEL_SIZE);
        header_p =
               static_cast<BlockHeader *>(d_allocator_p->allocate(totalBytes));
        if (!header_p) {
            // We could not satisfy this request.  Throw 'std::bad_alloc'.
            bsls::BslExceptionUtil::throwBadAlloc();
        }
    }

    bsls::BslLockGuard guard(&d_lock);

    // Increment the total number of allocations from this allocator.  Use the
    // result to generate the unique ID for this allocation, which is the
    // number of allocations prior to this one.
    bsls::Types::Int64 allocationId = d_numAllocations.addRelaxed(1) - 1;

    d_lastAllocatedNumBytes.storeRelaxed(
                                        static_cast<bsls::Types::Int64>(size));

#ifdef BDE_BUILD_TARGET_EXC
    if (0 <= allocationLimit()) {
        if (0 > d_allocationLimit.addRelaxed(-1)) {
            throw TestAllocatorException(static_cast<int>(size));
        }
    }
#endif

    if (0 == size) {
        d_lastAllocatedAddress_p.storeRelaxed(0);
        return 0;                                                     // RETURN
    }

    // Ensure 'allocate' returned maximally aligned memory.
    BSLS_ASSERT(isAligned(header_p, k_MAX_ALIGNMENT));

    header_p->d_magicNumber = k_ALLOCATED_MEMORY_MAGIC_NUMBER;

    // Insert into linked list of allocated blocks
    header_p->d_next_p = 0;
    if (d_blockListHead_p) {
        header_p->d_prev_p          = d_blockListTail_p;
        d_blockListTail_p->d_next_p = header_p;
        d_blockListTail_p           = header_p;
    }
    else {
        header_p->d_prev_p                    = 0;
        d_blockListHead_p = d_blockListTail_p = header_p;
    }

    header_p->d_bytes  = size;
    header_p->d_id     = allocationId;
    header_p->d_self_p = this;

    void *address = header_p + 1;

    // Fill sentinel bytes before and after user segment with known values.
    // Note that we don't initialize the user portion of the segment because
    // that would undermine Purify's 'UMR: uninitialized memory read' checking.
    std::memset(static_cast<char*>(address) - k_SENTINEL_SIZE,
                k_SENTINEL_BYTE, k_SENTINEL_SIZE);
    std::memset(static_cast<char*>(address) + size,
                k_SENTINEL_BYTE, k_SENTINEL_SIZE);

    // Update stats. The stats are updated as a group under a mutex lock, but
    // are atomic so that accessors can retrieve individual stats without
    // acquiring the mutex.
    d_numBlocksInUse.addRelaxed(1);
    if (numBlocksMax() < numBlocksInUse()) {
        d_numBlocksMax.storeRelaxed(numBlocksInUse());
    }
    d_numBlocksTotal.addRelaxed(1);

    d_numBytesInUse.addRelaxed(static_cast<bsls::Types::Int64>(size));
    if (numBytesMax() < numBytesInUse()) {
        d_numBytesMax.storeRelaxed(numBytesInUse());
    }
    d_numBytesTotal.addRelaxed(static_cast<bsls::Types::Int64>(size));

    d_lastAllocatedAddress_p.storeRelaxed(reinterpret_cast<int *>(address));

    if (isVerbose()) {

        // In verbose mode, print a message to 'stdout' -- e.g.,
        //..
        //  TestAllocator global [25]: Allocated 128 bytes at 0xc3a281a8.
        //..

        std::printf("TestAllocator%s%s [" FMT_I64 "]: Allocated " ZU
                    " byte%sat %p.\n",
                    d_name_p ? " " : "",
                    d_name_p ? d_name_p : "",
                    allocationId,
                    size,
                    1 == size ? " " : "s ",
                    address);

        std::fflush(stdout);
    }

    return address;
}

void TestAllocator::deallocate(void *address)
{
    bsls::BslLockGuard guard(&d_lock);

    d_lastDeallocatedAddress_p.storeRelaxed(reinterpret_cast<int *>(address));

    if (0 == address) {
        d_numDeallocations.addRelaxed(1);
        d_lastDeallocatedNumBytes.storeRelaxed(0);
        return;                                                       // RETURN
    }

    BlockHeader        *header_p = static_cast<BlockHeader *>(address) - 1;
    bool                miscError    = false;

    size_type           size;
    Uchar              *tail;
    bsls::Types::Int64  allocationId;

    // The following checks are done deliberately in the order shown to avoid a
    // possible bus error when attempting to read a misaligned 64-bit integer,
    // which can happen if an invalid address is passed to this method.  If the
    // address of the memory being deallocated is misaligned, it is very likely
    // that 'd_magicNumber' will not match the expected value, and so we will
    // skip the reading of 'd_bytes' (a 64-bit integer).

    if (! isAligned(header_p, k_MAX_ALIGNMENT)) {
        miscError = true; // Misaligned address
    }
    else if (k_ALLOCATED_MEMORY_MAGIC_NUMBER != header_p->d_magicNumber) {
        // Bad magic number caused by bad address, corruption or duplicate
        // deallocation.
        miscError = true;
    }
    else if (0 >= header_p->d_bytes || this != header_p->d_self_p) {
        // Size is corrupted or memory is being freed from wrong allocator.
        miscError = true;
    }
    else {
        size         = header_p->d_bytes;
        tail         = static_cast<Uchar *>(address) + size;
        allocationId = header_p->d_id;
    }

    // Now check if memory was corrupted.
    int overrunBy  = 0;
    int underrunBy = 0;

    // If there is evidence of corruption, this memory may have already been
    // freed.  On some platforms (but not others), the 'free' function will
    // scribble freed memory.  To get uniform behavior for test drivers, we
    // deliberately don't check over/underruns if 'miscError' is 'true'.
    if (!miscError) {
        // Check the sentinel before the segment.  Go backwards so we will
        // report the trashed byte nearest the segment.

        Uchar *pcBegin = static_cast<Uchar *>(address) - 1;
        Uchar *pcEnd   = static_cast<Uchar *>(address) - k_SENTINEL_SIZE;

        for (Uchar *pc = pcBegin; pcEnd <= pc; --pc) {
            if (k_SENTINEL_BYTE != *pc) {
                underrunBy = static_cast<int>(pcBegin + 1 - pc);
                break;
            }
        }

        if (!underrunBy) {
            // Check the sentinel after the segment.

            pcBegin = tail;
            pcEnd = tail + k_SENTINEL_SIZE;
            for (Uchar *pc = pcBegin; pc < pcEnd; ++pc) {
                if (k_SENTINEL_BYTE != *pc) {
                    overrunBy = static_cast<int>(pc + 1 - pcBegin);
                    break;
                }
            }
        }
    }

    // Report any error detected so far.
    if (miscError || overrunBy || underrunBy) {
        if (miscError) {
            d_numMismatches.addRelaxed(1);
        }
        else {
            d_numBoundsErrors.addRelaxed(1);
        }

        if (isQuiet()) {
            return;                                                   // RETURN
        }
        else {
            formatInvalidMemoryBlock(header_p, this, underrunBy, overrunBy);
            if (isNoAbort()) {
                return;                                               // RETURN
            }
            else {
                std::abort();                                         // ABORT
            }
        }
    }

    // At this point we know (almost) for sure that the memory block is
    // currently allocated from this object.  We now proceed to update our
    // statistics, stamp the block's header as deallocated, scribble over its
    // payload, and give it back to the underlying allocator supplied at
    // construction.  In verbose mode, we also report the deallocation event to
    // 'stdout'.

    d_numDeallocations.addRelaxed(1);
    d_lastDeallocatedNumBytes.storeRelaxed(
                                        static_cast<bsls::Types::Int64>(size));

    d_numBlocksInUse.addRelaxed(-1);

    d_numBytesInUse.addRelaxed(-static_cast<bsls::Types::Int64>(size));

    header_p->d_magicNumber = k_DEALLOCATED_MEMORY_MAGIC_NUMBER;

    if (isVerbose()) {

        // In verbose mode, print a message to 'stdout' -- e.g.,
        //..
        //  TestAllocator local [245]: Deallocated 1 byte at 0x3c1b2740.
        //..

        std::printf("TestAllocator%s%s [" FMT_I64 "]: Deallocated " ZU
                    " byte%sat %p.\n",
                    d_name_p ? " " : "",
                    d_name_p ? d_name_p : "",
                    allocationId,
                    size,
                    1 == size ? " " : "s ",
                    address);

        std::fflush(stdout);
    }

    // Unlink header from linked list of blocks
    if (header_p->d_prev_p) {
        header_p->d_prev_p->d_next_p = header_p->d_next_p;
    }
    else {
        d_blockListHead_p = header_p->d_next_p;
    }

    if (header_p->d_next_p) {
        header_p->d_next_p->d_prev_p = header_p->d_prev_p;
    }
    else {
        d_blockListTail_p = header_p->d_prev_p;
    }

    // Scribble over allocated memory as an aid to debugging.  Note that,
    // because we are immediately deallocating after scribbling, the 'memset'
    // might be optimized away.
    std::memset(address, static_cast<int>(k_DEALLOCATED_BYTE), size);

    // Deallocate block from upstream allocator.  It is safer and more
    // efficient to perform the deallocation outside of the critical section,
    // i.e., we release the lock before delegating to the upstream allocator.
    guard.release();
    d_lock.unlock();
    d_allocator_p->deallocate(header_p);
}

// ACCESSORS
void TestAllocator::print() const
{
    bsls::BslLockGuard guard(&d_lock);

    if (d_name_p) {
        std::printf("\n"
                    "==================================================\n"
                    "                TEST ALLOCATOR %s STATE\n"
                    "--------------------------------------------------\n",
                    d_name_p);
    }
    else {
        std::printf("\n"
                    "==================================================\n"
                    "                TEST ALLOCATOR STATE\n"
                    "--------------------------------------------------\n");
    }

    std::printf("        Category\tBlocks\tBytes\n"
                "        --------\t------\t-----\n"
                "          IN USE\t" FMT_I64 "\t" FMT_I64 "\n"
                "             MAX\t" FMT_I64 "\t" FMT_I64 "\n"
                "           TOTAL\t" FMT_I64 "\t" FMT_I64 "\n"
                "      MISMATCHES\t" FMT_I64 "\n"
                "   BOUNDS ERRORS\t" FMT_I64 "\n"
                "--------------------------------------------------\n",
                numBlocksInUse(), numBytesInUse(),
                numBlocksMax(),   numBytesMax(),
                numBlocksTotal(), numBytesTotal(),
                numMismatches(),  numBoundsErrors());

    if (d_blockListHead_p) {
        std::printf(" Indices of Outstanding Memory Allocations:\n ");
        printIdList(d_blockListHead_p);
    }
    std::fflush(stdout);
}

int TestAllocator::status() const
{
    enum { BSLMA_MEMORY_LEAK = -1, BSLMA_SUCCESS = 0 };

    bsls::BslLockGuard guard(&d_lock);

    const bsls::Types::Int64 numErrors = numMismatches() + numBoundsErrors();

    if (numErrors > 0) {
        return static_cast<int>(numErrors);                           // RETURN
    }
    else if (numBlocksInUse() || numBytesInUse()) {
        return BSLMA_MEMORY_LEAK;                                     // RETURN
    }
    else {
        return BSLMA_SUCCESS;                                         // RETURN
    }
}

}  // close package namespace
}  // close enterprise namespace

// ----------------------------------------------------------------------------
// Copyright 2013 Bloomberg Finance L.P.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// ----------------------------- END-OF-FILE ----------------------------------
