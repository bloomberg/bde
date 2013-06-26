// bslma_testallocator.cpp                                            -*-C++-*-
#include <bslma_testallocator.h>

#include <bsls_ident.h>
BSLS_IDENT("$Id$ $CSID$")

#include <bslma_mallocfreeallocator.h>
#include <bslma_testallocatorexception.h>

#include <bsls_alignment.h>
#include <bsls_alignmentutil.h>
#include <bsls_assert.h>
#include <bsls_bslexceptionutil.h>
#include <bsls_platform.h>

#include <cstdio>   // print messages
#include <cstdlib>  // abort
#include <cstring>  // memset

namespace BloombergLP {

namespace {

typedef unsigned char              Uchar;

typedef bslma::Allocator::size_type size_type;

const unsigned int ALLOCATED_MEMORY = 0xDEADBEEF;   // magic number identifying
                                                    // memory allocated by this
                                                    // allocator

const unsigned int DEALLOCATED_MEMORY = 0xDEADF00D; // new magic number written
                                                    // over other magic number
                                                    // upon deallocation

const unsigned char SCRIBBLED_MEMORY = 0xA5;        // byte used to scribble
                                                    // deallocated memory

const Uchar PADDED_MEMORY = 0xB1;                   // byte used to write over
                                                    // newly allocated memory
                                                    // and pads.

enum { PADDING_SIZE = sizeof(bsls::AlignmentUtil::MaxAlignedType) };
                                                    // size of the padding
                                                    // before and after the
                                                    // user segment

                        // ===========
                        // struct Link
                        // ===========

struct Link {
    // This 'struct' stores a pointer to the next and preceding allocated
    // memory block in the allocated memory block list.

    bsls::Types::Int64  d_index;   // index of this allocation
    Link               *d_next_p;  // next 'Link' pointer
    Link               *d_prev_p;  // previous 'Link' pointer
};

                        // =============
                        // struct Header
                        // =============

struct Header {
    // This 'struct' defines the data preceding allocated memory.

    unsigned int  d_magicNumber;  // allocated/deallocated/other identifier

    size_type     d_bytes;        // number of available bytes in this block

    bsls::Types::Int64
                  d_index;        // index of this memory allocation

    Link         *d_address_p;    // address of block in linked list

    void         *d_id_p;         // address of current allocator

    bsls::AlignmentUtil::MaxAlignedType
                  d_padding;      // padding -- guaranteed to extend to the
                                  // end of the struct
};

                        // ===========
                        // union Align
                        // ===========

union Align {
    // Maximally-aligned raw buffer big enough for a Header.

    Header                              d_object;
    bsls::AlignmentUtil::MaxAlignedType d_alignment;
};

}  // close unnamed namespace

static void formatBlock(void *address, int length)
    // Format in hex to 'stdout', a block of memory starting at the specified
    // starting 'address' of the specified 'length'.  Each line of formatted
    // output will have a maximum of 16 bytes per line, where each line starts
    // with the address of that 16-byte chunk.
{
    unsigned char *addr    = reinterpret_cast<unsigned char *>(address);
    unsigned char *endAddr = addr + length;

    for (int i = 0; addr < endAddr; ++i) {
        if (0 == i % 4) {
            if (i) {
                std::printf("\n");
            }
            std::printf("%p:\t", static_cast<void *>(addr));
        }
        else {
            std::printf("  ");
        }

        for (int j = 0; j < 4 && addr < endAddr; ++j) {
            std::printf("%02x ", *addr++);
        }
    }

    std::printf("\n");
}

static void formatInvalidMemoryBlock(Align                *address,
                                     bslma::TestAllocator *allocator,
                                     int                   underrunBy,
                                     int                   overrunBy)
    // Format the contents of the presumably invalid memory block at the
    // specified 'address' to 'stdout', using of the specified 'allocator',
    // 'underrunBy', and 'overrunBy' information.  An appropriate error,
    // message, if appropriate, is printed first, followed by a block of memory
    // indicating the header and any extra padding appropriate for the current
    // platform.  Finally, the first 64 bytes of memory of the "payload"
    // portion of the allocated memory is is printed (regardless of the amount
    // of memory the user requested).
{
    unsigned int   magicNumber = address->d_object.d_magicNumber;
    size_type      numBytes    = address->d_object.d_bytes;
    unsigned char *payload     =
                                reinterpret_cast<unsigned char *>(address + 1);

    if (ALLOCATED_MEMORY != magicNumber)  {
        if (DEALLOCATED_MEMORY == magicNumber)  {
            std::printf("*** Deallocating previously deallocated memory at"
                        " %p. ***\n",
                        static_cast<void *>(payload));
        }
        else {
            std::printf("*** Invalid magic number 0x%08x at address %p. ***\n",
                        magicNumber,
                        static_cast<void *>(payload));
        }
    }
    else if (numBytes <= 0) {
#ifdef BSLS_PLATFORM_CPU_64_BIT
        std::printf("*** Invalid (non-positive) byte count %lld at address"
                    " %p. *** \n",
                    static_cast<bsls::Types::Int64>(numBytes),
                    static_cast<void *>(payload));
#else
        std::printf("*** Invalid (non-positive) byte count %d at address"
                    " %p. *** \n",
                    numBytes,
                    static_cast<void *>(payload));
#endif
    }
    else if (allocator != address->d_object.d_id_p) {
        std::printf("*** Freeing segment at %p from wrong allocator. ***\n",
                    static_cast<void *>(payload));
    }
    else if (underrunBy) {
#ifdef BSLS_PLATFORM_CPU_64_BIT
        std::printf("*** Memory corrupted at %d bytes before %lld byte"
                    " segment at %p. ***\n",
                    underrunBy,
                    static_cast<bsls::Types::Int64>(numBytes),
                    static_cast<void *>(payload));
#else
        std::printf("*** Memory corrupted at %d bytes before %d byte"
                    " segment at %p. ***\n",
                    underrunBy,
                    numBytes,
                    static_cast<void *>(payload));
#endif

        std::printf("Pad area before user segment:\n");
        formatBlock(payload - PADDING_SIZE, PADDING_SIZE);
    }
    else if (overrunBy) {
#ifdef BSLS_PLATFORM_CPU_64_BIT
        std::printf("*** Memory corrupted at %d bytes after %lld byte"
                    " segment at %p. ***\n",
                    overrunBy,
                    static_cast<bsls::Types::Int64>(numBytes),
                    static_cast<void *>(payload));
#else
        std::printf("*** Memory corrupted at %d bytes after %d byte"
                    " segment at %p. ***\n",
                    overrunBy,
                    numBytes,
                    static_cast<void *>(payload));
#endif

        std::printf("Pad area after user segment:\n");
        formatBlock(payload + numBytes, PADDING_SIZE);
    }

    std::printf("Header:\n");
    formatBlock(address, sizeof *address );
    std::printf("User segment:\n");
    formatBlock(payload, 64);
}

namespace bslma {

                        // =========================
                        // struct TestAllocator_List
                        // =========================

struct TestAllocator_List {
    // This 'struct' stores a head 'Link' and a tail 'Link' for list
    // manipulation.

    Link *d_head_p;    // address of first link in list (or 0)
    Link *d_tail_p;    // address of last link in list (or 0)
};

}  // close package namespace

static Link *removeLink(bslma::TestAllocator_List *allocatedList, Link *link)
    // Remove the specified 'link' from the specified 'allocatedList'.  Return
    // the address of the removed link if 'index' is found, or 0 otherwise.
    // The behavior is undefined unless 'allocatedList' is not 0.  Note that
    // the tail pointer of the 'allocatedList' will be updated if the 'Link'
    // removed is the tail itself.  Also note that the head pointer of the
    // 'allocatedList' will be updated if the link removed is the head itself.
{
    BSLS_ASSERT(allocatedList);
    BSLS_ASSERT(link);

    if (link == allocatedList->d_tail_p) {
        allocatedList->d_tail_p = link->d_prev_p;
    }
    else {
        link->d_next_p->d_prev_p = link->d_prev_p;
    }

    if (link == allocatedList->d_head_p) {
        allocatedList->d_head_p = link->d_next_p;
    }
    else {
        link->d_prev_p->d_next_p = link->d_next_p;
    }

    return link;
}

static Link *addLink(bslma::TestAllocator_List *allocatedList,
                     bsls::Types::Int64         index,
                     bslma::Allocator          *basicAllocator)
    // Add to the specified 'allocatedList' a 'Link' having the specified
    // 'index', and using the specified 'basicAllocator' to supply memory; also
    // update the tail pointer of the 'allocatedList'.  Return the address of
    // the allocated 'Link'.  Note that this operation has no effect if
    // 'allocatedList' is 0.  Also note that the head pointer of the
    // 'allocatedList' will be updated if the 'allocatedList' is initially
    // empty.
{
    BSLS_ASSERT(allocatedList);

    Link * link = (Link*) basicAllocator->allocate(sizeof(Link));

    // Ensure 'allocate' returned memory.

    BSLS_ASSERT(0 != link);

    link->d_next_p = 0;
    link->d_index  = index;

    if (!allocatedList->d_head_p) {
        allocatedList->d_head_p = link;
        allocatedList->d_tail_p = link;
        link->d_prev_p = 0;
    }
    else {
        allocatedList->d_tail_p->d_next_p = link;
        link->d_prev_p = allocatedList->d_tail_p;
        allocatedList->d_tail_p = link;
    }

    return link;
}

static void printList(const bslma::TestAllocator_List& allocatedList)
    // Print the indices of all 'Link' objects currently in the specified
    // 'allocatedList'.
{
    const Link *link_p = allocatedList.d_head_p;

    while (link_p) {
        for (int i = 0; i < 8 && link_p; ++i) {
            std::printf("%lld\t", link_p->d_index);
            link_p = link_p->d_next_p;
        }

        // The space after the '\n' is needed to align these indices properly
        // with the current output.

        std::printf("\n ");
    }
}

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
, d_allocator_p(basicAllocator
                ? basicAllocator
                : &MallocFreeAllocator::singleton())
{
    BSLS_ASSERT(d_allocator_p);

    d_list_p = (TestAllocator_List *)d_allocator_p->allocate(
                                                   sizeof(TestAllocator_List));
    d_list_p->d_head_p = 0;
    d_list_p->d_tail_p = 0;
}

TestAllocator::TestAllocator(bool       verboseFlag,
                             Allocator *basicAllocator)
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
, d_allocator_p(basicAllocator
                ? basicAllocator
                : &MallocFreeAllocator::singleton())
{
    BSLS_ASSERT(d_allocator_p);

    d_list_p = (TestAllocator_List *)d_allocator_p->allocate(
                                                   sizeof(TestAllocator_List));
    d_list_p->d_head_p = 0;
    d_list_p->d_tail_p = 0;
}

TestAllocator::TestAllocator(const char *name,
                             Allocator  *basicAllocator)
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
, d_allocator_p(basicAllocator
                ? basicAllocator
                : &MallocFreeAllocator::singleton())
{
    BSLS_ASSERT(d_allocator_p);

    d_list_p = (TestAllocator_List *)d_allocator_p->allocate(
                                                   sizeof(TestAllocator_List));
    d_list_p->d_head_p = 0;
    d_list_p->d_tail_p = 0;
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
, d_allocator_p(basicAllocator
                ? basicAllocator
                : &MallocFreeAllocator::singleton())
{
    BSLS_ASSERT(d_allocator_p);

    d_list_p = (TestAllocator_List *)d_allocator_p->allocate(
                                                   sizeof(TestAllocator_List));
    d_list_p->d_head_p = 0;
    d_list_p->d_tail_p = 0;
}

TestAllocator::~TestAllocator()
{
    BSLS_ASSERT(d_allocator_p);
    BSLS_ASSERT(d_list_p);

    if (d_verboseFlag) {
        print();
    }

    Link *link_p = d_list_p->d_head_p;
    while (link_p) {
        Link *linkToFree = link_p;
        link_p = link_p->d_next_p;
        d_allocator_p->deallocate(linkToFree);
    }
    d_list_p->d_head_p = 0;
    d_list_p->d_tail_p = 0;
    d_allocator_p->deallocate(d_list_p);

    if (!d_quietFlag) {
        if (d_numBytesInUse || d_numBlocksInUse) {
            std::printf("MEMORY_LEAK");
            if (d_name_p) {
                std::printf(" from %s", d_name_p);
            }
            std::printf(":\n  Number of blocks in use = %lld\n"
                        "   Number of bytes in use = %lld\n",
                        d_numBlocksInUse, d_numBytesInUse);

            if (!d_noAbortFlag) {
                std::abort();                                         // ABORT
            }
        }
    }
}

// MANIPULATORS
void *TestAllocator::allocate(size_type size)
{
    bsls::Types::Int64 allocationIndex = d_numAllocations++;  // Note: Postfix
                                                              // is important!
    d_lastAllocatedNumBytes           = size;

    // Set to zero in case of premature returns.

    d_lastAllocatedAddress_p = 0;

#ifdef BDE_BUILD_TARGET_EXC
    if (0 <= d_allocationLimit) {
        --d_allocationLimit;
        if (0 > d_allocationLimit) {
            throw TestAllocatorException(static_cast<int>(size));
        }
    }
#endif

    if (size == 0) {
        return 0;                                                     // RETURN
    }

    Align *align = (Align *)d_allocator_p->allocate(
                                          sizeof(Align) + size + PADDING_SIZE);
    if (! align) {
        // We cannot satisfy this request.  Throw 'std::bad_alloc'.

        bsls::BslExceptionUtil::throwBadAlloc();
    }

    // Note that we don't initialize the user portion of the segment because
    // that would undermine purify's 'UMR: uninitialized memory read' checking.

    std::memset((char *)(align + 1) - PADDING_SIZE,
                PADDED_MEMORY, PADDING_SIZE);
    std::memset((char *)(align + 1) + size, PADDED_MEMORY, PADDING_SIZE);

    // Ensure 'allocate' returned maximally aligned memory.

    BSLS_ASSERT(0 == bsls::AlignmentUtil::calculateAlignmentOffset(
                                     align,
                                     bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT));

    align->d_object.d_bytes       = size;
    align->d_object.d_magicNumber = ALLOCATED_MEMORY;
    align->d_object.d_index       = allocationIndex;

    ++d_numBlocksInUse;
    if (d_numBlocksMax < d_numBlocksInUse) {
        d_numBlocksMax = d_numBlocksInUse;
    }
    ++d_numBlocksTotal;

    d_numBytesInUse += size;
    if (d_numBytesMax < d_numBytesInUse) {
        d_numBytesMax = d_numBytesInUse;
    }
    d_numBytesTotal += size;

    Link *link = addLink(d_list_p, allocationIndex, d_allocator_p);
    align->d_object.d_address_p = link;
    align->d_object.d_id_p      = this;

    void *address = ++align;

    if (d_verboseFlag) {

        // In verbose mode, print a message to 'stdout' -- e.g.,
        //..
        //  TestAllocator global [25]: Allocated 128 bytes at 0xc3a281a8.
        //..

        std::printf("TestAllocator");

        if (d_name_p) {
            std::printf(" %s", d_name_p);
        }

#ifdef BSLS_PLATFORM_CPU_64_BIT
        std::printf(" [%lld]: Allocated %lld byte%sat %p.\n",
                    allocationIndex,
                    static_cast<bsls::Types::Int64>(size),
                    1 == size ? " " : "s ",
                    address);
#else
        std::printf(" [%lld]: Allocated %d byte%sat %p.\n",
                    allocationIndex,
                    size,
                    1 == size ? " " : "s ",
                    address);
#endif
        std::fflush(stdout);
    }

    d_lastAllocatedAddress_p = address;
    BSLS_ASSERT(0 == bsls::AlignmentUtil::calculateAlignmentOffset(
                                     address,
                                     bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT));
    return address;
}

void TestAllocator::deallocate(void *address)
{
    ++d_numDeallocations;
    d_lastDeallocatedAddress_p = address;

    // Set to zero in case of premature returns.

    d_lastDeallocatedNumBytes  = 0;

    if (0 == address) {
        return;                                                       // RETURN
    }

    Align *align     = ((Align *) address) - 1;
    bool   miscError = false;

    size_type           size;
    Uchar              *tail;
    bsls::Types::Int64  allocationIndex;

    // The following checks are done deliberately in the order shown to avoid a
    // possible bus error when attempting to read a 64-bit integer on
    // misaligned memory (which can happen if the client passes an invalid
    // address to this method).  If the address of the memory being deallocated
    // is misaligned, it is very likely that 'd_magicNumber' will not match the
    // expected value, and so we will skip the read of 'd_bytes' (a 64-bit
    // integer).

    if (ALLOCATED_MEMORY != align->d_object.d_magicNumber) {
        miscError = true;
    }
    else if (0 >= align->d_object.d_bytes || this != align->d_object.d_id_p) {
        miscError = true;
    }
    else {
        size            = align->d_object.d_bytes;
        tail            = (Uchar *)address + size;
        allocationIndex = align->d_object.d_index;
    }

    // If there is evidence of corruption, this memory may have already been
    // freed.  On some platforms (but not others), the 'free' function will
    // scribble freed memory.  To get uniform behavior for test drivers, we
    // deliberately don't check over/underruns if 'miscError' is 'true'.

    int overrunBy  = 0;
    int underrunBy = 0;

    if (!miscError) {
        Uchar *pcBegin;
        Uchar *pcEnd;

        // Check the padding before the segment.  Go backwards so we will
        // report the trashed byte nearest the segment.

        pcBegin = (Uchar *) address - 1;
        pcEnd   = (Uchar *) &align->d_object.d_padding;

        for (Uchar *pc = pcBegin; pcEnd <= pc; --pc) {
            if (PADDED_MEMORY != *pc) {
                underrunBy = static_cast<int>(pcBegin + 1 - pc);
                break;
            }
        }

        if (!underrunBy) {
            // Check the padding after the segment.

            pcBegin = tail;
            pcEnd = tail + PADDING_SIZE;
            for (Uchar *pc = pcBegin; pc < pcEnd; ++pc) {
                if (PADDED_MEMORY != *pc) {
                    overrunBy = static_cast<int>(pc + 1 - pcBegin);
                    break;
                }
            }
        }
    }

    // Now check for corrupted memory block and cross allocation.

    if (!miscError && !overrunBy && !underrunBy) {
        d_allocator_p->deallocate(removeLink(d_list_p,
                                             align->d_object.d_address_p));
    }
    else {
        if (miscError) {
            ++d_numMismatches;
        } else if (overrunBy || underrunBy) {
            ++d_numBoundsErrors;
        }

        if (d_quietFlag) {
            return;                                                   // RETURN
        }
        else {
            formatInvalidMemoryBlock(align, this, underrunBy, overrunBy);
            if (d_noAbortFlag) {
                return;                                               // RETURN
            }
            else {
                std::abort();                                         // ABORT
            }
        }
    }

    // At this point we know (almost) for sure that the memory block is
    // currently allocated from this object.  We now proceed to update our
    // statistics, stamp the blocks header as deallocated, scribble all over
    // its payload, and give it back to the underlying allocator supplied at
    // construction.  In verbose mode, we also report the deallocation event to
    // 'stdout'.

    d_lastDeallocatedNumBytes = size;

    --d_numBlocksInUse;

    d_numBytesInUse -= size;

    align->d_object.d_magicNumber = DEALLOCATED_MEMORY;

    if (d_verboseFlag) {
        // In verbose mode, print a message to 'stdout' -- e.g.,
        //..
        //  TestAllocator local [245]: Deallocated 1 byte at 0x3c1b2740.
        //..

        std::printf("TestAllocator");
        if (d_name_p) {
            std::printf(" %s", d_name_p);
        }

#ifdef BSLS_PLATFORM_CPU_64_BIT
        std::printf(" [%lld]: Deallocated %lld byte%sat %p.\n",
                    allocationIndex,
                    static_cast<bsls::Types::Int64>(size),
                    1 == size ? " " : "s ",
                    address);
#else
        std::printf(" [%lld]: Deallocated %d byte%sat %p.\n",
                    allocationIndex,
                    size,
                    1 == size ? " " : "s ",
                    address);
#endif
        std::fflush(stdout);
    }

    std::memset(address, static_cast<int>(SCRIBBLED_MEMORY), size);
    d_allocator_p->deallocate(align);
}

// ACCESSORS
int TestAllocator::status() const
{
    enum { BSLMA_MEMORY_LEAK = -1, BSLMA_SUCCESS = 0 };

    bsls::Types::Int64 numErrors = d_numMismatches + d_numBoundsErrors;

    if (numErrors > 0) {
        return static_cast<int>(numErrors);
    }
    else if (d_numBlocksInUse || d_numBytesInUse) {
        return BSLMA_MEMORY_LEAK;
    }
    else {
        return BSLMA_SUCCESS;
    }
}

// FREE OPERATORS
void TestAllocator::print() const
{
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
                "          IN USE\t%lld\t%lld\n"
                "             MAX\t%lld\t%lld\n"
                "           TOTAL\t%lld\t%lld\n"
                "  NUM MISMATCHES\t%lld\n"
                "   BOUNDS ERRORS\t%lld\n"
                "--------------------------------------------------\n",
                numBlocksInUse(), numBytesInUse(),
                numBlocksMax(),   numBytesMax(),
                numBlocksTotal(), numBytesTotal(),
                numMismatches(),  numBoundsErrors());

    if (d_list_p->d_head_p) {
        std::printf(" Indices of Outstanding Memory Allocation:\n ");
        printList(*d_list_p);
    }
    std::fflush(stdout);
}

}  // close package namespace

}  // close enterprise namespace

// ----------------------------------------------------------------------------
// Copyright (C) 2013 Bloomberg L.P.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
// ----------------------------- END-OF-FILE ----------------------------------
