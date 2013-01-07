// bdema_testprotectableblockdispenser.cpp                            -*-C++-*-
#include <bdema_testprotectableblockdispenser.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bdema_testprotectableblockdispenser_cpp,"$Id$ $CSID$")

#include <bslma_testallocatorexception.h>

#include <bsls_alignmentutil.h>
#include <bsls_assert.h>
#include <bsls_platform.h>
#include <bsls_platformutil.h>

#include <cstdio>                  // print messages
#include <cstdlib>                 // abort
#include <cstring>                 // memset

namespace BloombergLP {

namespace {

const unsigned int MAGIC_NUMBER = 0xDEADBEEF;        // used to identify memory
                                                     // that has been allocated

const unsigned int DEALLOCATED_MEMORY = 0xDEADF00D;  // used to identify memory
                                                     // that has been
                                                     // deallocated

const unsigned char SCRIBBLED_MEMORY = 0xA5;         // byte used to scribble
                                                     // deallocated memory

unsigned int adler(const unsigned char          *data,
                   bsls_PlatformUtil::size_type  length)
    // A simple implementation of the Adler-32 algorithm.
{
    static const int MOD_ADLER = 65521;

    unsigned int a = 1, b = 0;
    while (length > 0) {
        bsls_PlatformUtil::size_type tlen = length > 5550 ? 5550 : length;
        length -= tlen;
        do {
            a += *data++;
            b += a;
        } while (--tlen);

        a %= MOD_ADLER;
        b %= MOD_ADLER;
    }

    return (b << 16) | a;
}

inline
unsigned int checksum(const unsigned char          *data,
                      bsls_PlatformUtil::size_type  length)
    // Return a simple checksum of the specified 'data' having the specified
    // 'length'.  Implementation Note: levelization rules prevent using 'bdede'
    // components from within this component.  Instead, this component defines
    // a simple implementation of the Adler-32 algorithm.
{
    return adler(data, length);
}

                        // =============
                        // struct Header
                        // =============

struct Header {
    // Data preceding allocated memory.  Note that it is important that the
    // "magic number" is the last data element - this ensures that the test
    // for a magic number will fit within the header for 'malloc', in case
    // an operation is called with memory not allocated by the test dispenser.

    int          d_bytes;
    unsigned int d_checksum;
    bool         d_isProtected;
    unsigned int d_magicNumber;

};

                        // ===========
                        // union Align
                        // ===========

union Align {
    // Maximally-aligned raw buffer big enough for a 'Header'.

    Header                             d_object;
    bsls_AlignmentUtil::MaxAlignedType d_alignment;
};

                        // ===========================
                        // FILE SCOPE STATIC FUNCTIONS
                        // ===========================

void printAlign(const Align *align)
    // Print on a single line the specified 'align' (in hex) followed by the
    // (hex) value of each byte.
{
    std::printf("%p:\t", (void *)align);

    unsigned char *addr = (unsigned char *) align;

    for (int i = 0; i < (int)(sizeof *align / 4); ++i) {
        if (i > 0) {
            std::printf("  ");
        }
        for (int j = 0; j < 4; ++j) {
            std::printf("%02x ", *addr++);
        }
    }
    std::printf("\n");
}

void print16bytes(void *address)
    // Print on a single line the specified 'address' (in hex) followed by the
    // (hex) value of the block of 16 bytes it addresses.
{
    std::printf("%p:\t", address);

    unsigned char *addr = (unsigned char *) address;

    for (int i = 0; i < 4; ++i) {
        if (i > 0) {
            std::printf("  ");
        }
        for (int j = 0; j < 4; ++j) {
            std::printf("%02x ", *addr++);
        }
    }
    std::printf("\n");
}

void printBadBlock(const Align                        *align,
                   const bdema_MemoryBlockDescriptor&  block)
    // Print an error message that describes the invalid block of memory at
    // the specified 'align' address.  The message includes the (hex) value of
    // the block of 8 bytes addressed by 'align' followed by a line indicating
    // the bad magic number (and/or byte count) and the memory address the
    // client was attempting to access, and then the value of the block of the
    // next 64 bytes addressed by 'align'.  Each line of 16 memory values
    // is labeled with the address of that memory block.  For example:
    //
    // 22254:  20 69 73 20   61 20 76 65
    // Invalid magic number 61207665 at address 22254
    // 2225c:  72 79 20 6c   6f 6e 67 20   73 74 72 69   6e 67 20 74
    // 2226c:  68 61 74 20   73 68 6f 75   6c 64 20 62   65 20 65 6e
    // 2227c:  6f 75 67 68   20 62 79 74   65 73 20 74   6f 20 75 73
    // 2228c:  65 00 00 00   00 00 12 90   00 00 00 00   00 00 00 00
{
    unsigned int magicNumber = align->d_object.d_magicNumber;

    printAlign(align);
    if (MAGIC_NUMBER != magicNumber)  {
        if (DEALLOCATED_MEMORY == magicNumber)  {
            std::printf(
                  "*** Deallocating previously deallocated memory at %p ***\n",
                   (void *)align);
        }
        else {
            std::printf("*** Invalid magic number %08x at address %p ***\n",
            magicNumber, (void *)align);
        }
    }
    else {
        int numBytes = align->d_object.d_bytes;
        if (numBytes <= 0) {
            std::printf(
               "*** Invalid (non-positive) byte count %d at address %p *** \n",
               numBytes, (void *)align);
        }
        if (numBytes != block.size()) {

#ifdef BSLS_PLATFORM_CPU_64_BIT
            std::printf(
  "*** byte count %d doesn't match descriptor count %lld at address %p *** \n",
                        numBytes, (long long) block.size(), (void *)align);
#else
            std::printf(
  "*** byte count %d doesn't match descriptor count %d at address %p *** \n",
                        numBytes, block.size(), (void *)align);
#endif

        }
    }
    unsigned char *address = (unsigned char *) ++align;
    for (int i = 0; i < 4; ++i) {
        print16bytes(address);
        address += 16;
    }
}

inline
bsls_PlatformUtil::size_type allocationSize(
                                    bsls_PlatformUtil::size_type requestedSize,
                                    bsls_PlatformUtil::size_type pageSize)
    // Return the size to allocate for the specified 'requestedSize' based on
    // the specified system 'pageSize'.
{
    int numPages = (requestedSize + pageSize - 1) / pageSize;
    return numPages * pageSize;
}

bool isValid(const Align                        *align,
             const bdema_MemoryBlockDescriptor&  block,
             bool                                quietFlag,
             bool                                noAbortFlag)
{
    bool valid = true;
    if (align->d_object.d_magicNumber != MAGIC_NUMBER
     || align->d_object.d_bytes       <= 0
     || align->d_object.d_bytes       != block.size()) {
        valid = false;
        if (!quietFlag) {
            printBadBlock(align, block);
            if (!noAbortFlag) {
                std::abort();                                          // ABORT
            }
        }
    }
    return valid;
}

}  // close unnamed namespace

                    // -----------------------------------------
                    // class bdema_TestProtectableBlockDispenser
                    // -----------------------------------------

// CREATORS
bdema_TestProtectableBlockDispenser::
bdema_TestProtectableBlockDispenser(int pageSize, bool verboseFlag)
: d_pageSize(pageSize)
, d_numBlocksInUse(0)
, d_numBytesInUse(0)
, d_numBlocksMax(0)
, d_numBytesMax(0)
, d_numBlocksTotal(0)
, d_numBytesTotal(0)
, d_numErrors(0)
, d_allocationLimit(-1)
, d_lastAllocateNumBytes(0)
, d_lastDeallocateNumBytes(0)
, d_lastAllocateAddress(0)
, d_lastDeallocateAddress(0)
, d_numAllocation(0)
, d_numDeallocation(0)
, d_noAbortFlag(false)
, d_quietFlag(false)
, d_verboseFlag(verboseFlag)
, d_numBlocksProtected(0)
{
}

bdema_TestProtectableBlockDispenser::~bdema_TestProtectableBlockDispenser()
{
    if (d_verboseFlag) {
        print();
    }

    if (!d_quietFlag) {
        if (d_numBytesInUse || d_numBlocksInUse) {
            std::printf("MEMORY_LEAK:\n"
                        "  Number of blocks in use = %d\n"
                        "   Number of bytes in use = %d\n",
                        d_numBlocksInUse, d_numBytesInUse);

            if (!d_noAbortFlag) {
                std::abort();                                          // ABORT
            }
        }
    }
}

// MANIPULATORS
bdema_MemoryBlockDescriptor
bdema_TestProtectableBlockDispenser::allocate(size_type numBytes)
{
    ++d_numAllocation;

    d_lastAllocateNumBytes = numBytes;
    d_lastAllocateAddress  = 0; // Set to zero in case of premature returns.

#ifdef BDE_BUILD_TARGET_EXC
    if (0 <= d_allocationLimit) {
        --d_allocationLimit;
        if (0 > d_allocationLimit) {
            throw bslma_TestAllocatorException(numBytes);
        }
    }
#endif

    if (numBytes < 0) {
        ++d_numErrors;
        if (!d_quietFlag) {

#ifdef BSLS_PLATFORM_CPU_64_BIT
            std::printf("*** Invalid (negative) allocation size %lld ***\n",
                        (long long) numBytes);
#else
            std::printf("*** Invalid (negative) allocation size %d ***\n",
                        numBytes);
#endif

            if (!d_noAbortFlag) {
                std::abort();                                         // ABORT
            }
        }
        return bdema_MemoryBlockDescriptor();                         // RETURN
    }
    if (numBytes <= 0) {
        return bdema_MemoryBlockDescriptor();                         // RETURN
    }

    int actualNumBytes = allocationSize(numBytes, d_pageSize);
    Align *align = static_cast<Align *>(
                                std::malloc(sizeof *align + actualNumBytes));

    // Ensure malloc returned maximally aligned memory.
    BSLS_ASSERT(0 == bsls_AlignmentUtil::calculateAlignmentOffset(
                                      align,
                                      bsls_AlignmentUtil::BSLS_MAX_ALIGNMENT));

    align->d_object.d_bytes       = actualNumBytes;
    align->d_object.d_magicNumber = MAGIC_NUMBER;
    align->d_object.d_isProtected = false;
    void *addr = ++align;

    d_numBytesInUse += actualNumBytes;
    d_numBytesTotal += actualNumBytes;
    if (d_numBytesInUse > d_numBytesMax) {
        d_numBytesMax = d_numBytesInUse;
    }

    ++d_numBlocksInUse;
    ++d_numBlocksTotal;

    if (d_numBlocksInUse > d_numBlocksMax) {
        d_numBlocksMax = d_numBlocksInUse;
    }

    if (d_verboseFlag) {
        std::printf("bdema_TestProtectableBlockDispenser: "
                    "allocated %d at %p\n",
                    actualNumBytes,
                    addr);
        std::fflush(stdout);
    }

    d_lastAllocateNumBytes = actualNumBytes;
    d_lastAllocateAddress  = addr;

    BSLS_ASSERT(0 == bsls_AlignmentUtil::calculateAlignmentOffset(
                                      addr,
                                      bsls_AlignmentUtil::BSLS_MAX_ALIGNMENT));
    return bdema_MemoryBlockDescriptor(addr, actualNumBytes);
}

void bdema_TestProtectableBlockDispenser::deallocate(
                                      const bdema_MemoryBlockDescriptor& block)
{
    ++d_numDeallocation;
    d_lastDeallocateAddress = block.address();
    d_lastDeallocateNumBytes = 0;  // Set to zero in case of premature returns.

    if (0 == block.address()) {
        return;                                                       // RETURN
    }

    Align *align = static_cast<Align *>(block.address()) - 1;
    if (!isValid(align, block, d_quietFlag, d_noAbortFlag)) {
        d_numErrors++;
        return;                                                       // RETURN
    }
    int numBytes = align->d_object.d_bytes;

    if (align->d_object.d_isProtected) {
        d_numErrors++;
        if (!d_quietFlag) {
            std::printf("Deallocate block protected state: %p",
                        block.address());
            if (!d_noAbortFlag) {
                std::abort();                                         // ABORT
            }
        }
        return;                                                       // RETURN
    }

    d_lastDeallocateNumBytes = numBytes;
    align->d_object.d_magicNumber = DEALLOCATED_MEMORY;

    d_numBytesInUse -= numBytes;
    --d_numBlocksInUse;

    if (d_verboseFlag) {
        std::printf("bdema_TestProtectableBlockDispenser: "
                    "freed %d at %p\n",
                    numBytes,
                    block.address());
        std::fflush(stdout);
    }

    std::memset(block.address(), (int) SCRIBBLED_MEMORY, numBytes);
    std::free(align);
}

int bdema_TestProtectableBlockDispenser::protect(
                                      const bdema_MemoryBlockDescriptor& block)
{
    Align *align = static_cast<Align *>(block.address()) - 1;

    if (!isValid(align, block, d_quietFlag, d_noAbortFlag)) {
        d_numErrors++;
        return -1;                                                    // RETURN
    }
    if (!align->d_object.d_isProtected) {
        align->d_object.d_isProtected = true;
        ++d_numBlocksProtected;
    }

    align->d_object.d_checksum =
               checksum((const unsigned char *)block.address(), block.size());
    return 0;
}

int bdema_TestProtectableBlockDispenser::unprotect(
                                      const bdema_MemoryBlockDescriptor& block)
{
    Align *align = static_cast<Align *>(block.address()) - 1;

    if (!isValid(align, block, d_quietFlag, d_noAbortFlag)) {
        d_numErrors++;
        return -1;                                                    // RETURN
    }
    if (align->d_object.d_isProtected) {
        if (align->d_object.d_checksum !=
            checksum((const unsigned char *)block.address(), block.size())) {
            // The data in the block has been modified since it was protected.

            ++d_numErrors;
            if (!d_quietFlag) {
                std::printf("Modification to a protected block detected: %p\n",
                            block.address());
                if (!d_noAbortFlag) {
                    std::abort();                                     // ABORT
                }
            }
        }
        align->d_object.d_isProtected = false;
        --d_numBlocksProtected;
    }

    return 0;
}

// ACCESSORS
int bdema_TestProtectableBlockDispenser::status() const
{
    enum { BDEMA_MEMORY_LEAK = -1, BDEMA_SUCCESS = 0 };

    return d_numErrors > 0
           ? d_numErrors
           : d_numBlocksInUse || d_numBytesInUse ? BDEMA_MEMORY_LEAK
                                                 : BDEMA_SUCCESS;
}

int bdema_TestProtectableBlockDispenser::minimumBlockSize() const
{
    return d_pageSize;
}

bool bdema_TestProtectableBlockDispenser::isProtected(
                                const bdema_MemoryBlockDescriptor& block) const
{
    Align *align = static_cast<Align *>(block.address()) - 1;
    if (!isValid(align, block, d_quietFlag, d_noAbortFlag)) {
        if (!d_quietFlag) {
            std::printf("*** Invalid block supplied to isProtected ***\n");
            if (!d_noAbortFlag) {
                std::abort();                                         // ABORT
            }
        }
        return false;                                                 // RETURN
    }

    return align->d_object.d_isProtected;
}

void bdema_TestProtectableBlockDispenser::print() const
{
    std::printf("\n"
                "==================================================\n"
                "                TEST ALLOCATOR STATE\n"
                "--------------------------------------------------\n"
                "        Category\tBlocks\tBytes\n"
                "        --------\t------\t-----\n"
                "          IN USE\t%d\t%d\n"
                "             MAX\t%d\t%d\n"
                "           TOTAL\t%d\t%d\n"
                "      NUM ERRORS\t%d\n"
                "--------------------------------------------------\n",
                numBlocksInUse(), numBytesInUse(),
                numBlocksMax(),   numBytesMax(),
                numBlocksTotal(), numBytesTotal(),
                numErrors());

    std::fflush(stdout);
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
