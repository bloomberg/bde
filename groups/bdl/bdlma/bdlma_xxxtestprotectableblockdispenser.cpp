// bdlma_xxxtestprotectableblockdispenser.cpp                            -*-C++-*-
#include <bdlma_xxxtestprotectableblockdispenser.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlma_xxxtestprotectableblockdispenser_cpp,"$Id$ $CSID$")

#include <bslma_testallocatorexception.h>

#include <bsls_alignmentutil.h>
#include <bsls_assert.h>
#include <bsls_bsltestutil.h>
#include <bsls_platform.h>
#include <bsls_types.h>

#include <bsl_cstdio.h>                  // print messages
#include <bsl_cstdlib.h>                 // abort
#include <bsl_cstring.h>                 // memset

// Platform-neutral format specifier for size_type
#define ZU BSLS_BSLTESTUTIL_FORMAT_ZU

namespace BloombergLP {

namespace {

const unsigned int MAGIC_NUMBER = 0xDEADBEEF;        // used to identify memory
                                                     // that has been allocated

const unsigned int DEALLOCATED_MEMORY = 0xDEADF00D;  // used to identify memory
                                                     // that has been
                                                     // deallocated

const unsigned char SCRIBBLED_MEMORY = 0xA5;         // byte used to scribble
                                                     // deallocated memory

unsigned int adler(const unsigned char    *data,
                   bsls::Types::size_type  length)
    // A simple implementation of the Adler-32 algorithm.
{
    static const int MOD_ADLER = 65521;

    unsigned int a = 1, b = 0;
    while (length > 0) {
        bsls::Types::size_type tlen = length > 5550 ? 5550 : length;
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
unsigned int checksum(const unsigned char    *data,
                      bsls::Types::size_type  length)
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

    Header                              d_object;
    bsls::AlignmentUtil::MaxAlignedType d_alignment;
};

                        // ===========================
                        // FILE SCOPE STATIC FUNCTIONS
                        // ===========================

void printAlign(const Align *align)
    // Print on a single line the specified 'align' (in hex) followed by the
    // (hex) value of each byte.
{
    bsl::printf("%p:\t", align);

    const unsigned char *addr = (const unsigned char *) align;

    for (int i = 0; i < (int)(sizeof *align / 4); ++i) {
        if (i > 0) {
            bsl::printf("  ");
        }
        for (int j = 0; j < 4; ++j) {
            bsl::printf("%02x ", *addr++);
        }
    }
    bsl::printf("\n");
}

void print16bytes(const void *address)
    // Print on a single line the specified 'address' (in hex) followed by the
    // (hex) value of the block of 16 bytes it addresses.
{
    bsl::printf("%p:\t", address);

    const unsigned char *addr = (const unsigned char *) address;

    for (int i = 0; i < 4; ++i) {
        if (i > 0) {
            bsl::printf("  ");
        }
        for (int j = 0; j < 4; ++j) {
            bsl::printf("%02x ", *addr++);
        }
    }
    bsl::printf("\n");
}

void printBadBlock(const Align                        *align,
                   const bdlma::MemoryBlockDescriptor&  block)
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
            bsl::printf(
                  "*** Deallocating previously deallocated memory at %p ***\n",
                   align);
        }
        else {
            bsl::printf("*** Invalid magic number %08x at address %p ***\n",
            magicNumber, align);
        }
    }
    else {
        int numBytes = align->d_object.d_bytes;
        if (numBytes <= 0) {
            bsl::printf(
               "*** Invalid (non-positive) byte count %d at address %p *** \n",
               numBytes, align);
        }
        if (static_cast<unsigned int>(numBytes) != block.size()) {
            bsl::printf(
"*** byte count %d doesn't match descriptor count " ZU " at address %p *** \n",
                        numBytes, block.size(), align);
        }
    }
    const unsigned char *address = (const unsigned char *) ++align;
    for (int i = 0; i < 4; ++i) {
        print16bytes(address);
        address += 16;
    }
}

inline
bsls::Types::size_type allocationSize(bsls::Types::size_type requestedSize,
                                      bsls::Types::size_type pageSize)
    // Return the size to allocate for the specified 'requestedSize' based on
    // the specified system 'pageSize'.
{
    int numPages = (requestedSize + pageSize - 1) / pageSize;
    return numPages * pageSize;
}

bool isValid(const Align                        *align,
             const bdlma::MemoryBlockDescriptor&  block,
             bool                                quietFlag,
             bool                                noAbortFlag)
{
    bool valid = true;
    if (align->d_object.d_magicNumber != MAGIC_NUMBER
     || align->d_object.d_bytes       <= 0
     || align->d_object.d_bytes       != static_cast<int>(block.size())) {
        valid = false;
        if (!quietFlag) {
            printBadBlock(align, block);
            if (!noAbortFlag) {
                bsl::abort();                                          // ABORT
            }
        }
    }
    return valid;
}

}  // close unnamed namespace

namespace bdlma {
                    // -----------------------------------------
                    // class TestProtectableBlockDispenser
                    // -----------------------------------------

// CREATORS
TestProtectableBlockDispenser::
TestProtectableBlockDispenser(int pageSize, bool verboseFlag)
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
, d_numAllocations(0)
, d_numDeallocations(0)
, d_noAbortFlag(false)
, d_quietFlag(false)
, d_verboseFlag(verboseFlag)
, d_numBlocksProtected(0)
{
}

TestProtectableBlockDispenser::~TestProtectableBlockDispenser()
{
    if (d_verboseFlag) {
        print();
    }

    if (!d_quietFlag) {
        if (d_numBytesInUse || d_numBlocksInUse) {
            bsl::printf("MEMORY_LEAK:\n"
                        "  Number of blocks in use = %d\n"
                        "   Number of bytes in use = %d\n",
                        d_numBlocksInUse, d_numBytesInUse);

            if (!d_noAbortFlag) {
                bsl::abort();                                          // ABORT
            }
        }
    }
}

// MANIPULATORS
MemoryBlockDescriptor
TestProtectableBlockDispenser::allocate(size_type numBytes)
{
    ++d_numAllocations;

    d_lastAllocateNumBytes = numBytes;
    d_lastAllocateAddress  = 0; // Set to zero in case of premature returns.

#ifdef BDE_BUILD_TARGET_EXC
    if (0 <= d_allocationLimit) {
        --d_allocationLimit;
        if (0 > d_allocationLimit) {
            throw bslma::TestAllocatorException(numBytes);
        }
    }
#endif

    if (numBytes == 0) {
        return MemoryBlockDescriptor();                         // RETURN
    }

    int actualNumBytes = allocationSize(numBytes, d_pageSize);
    Align *align = static_cast<Align *>(
                                bsl::malloc(sizeof *align + actualNumBytes));

    // Ensure malloc returned maximally aligned memory.
    BSLS_ASSERT(0 == bsls::AlignmentUtil::calculateAlignmentOffset(
                                     align,
                                     bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT));

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
        bsl::printf("TestProtectableBlockDispenser: "
                    "allocated %d at %p\n",
                    actualNumBytes,
                    addr);
        bsl::fflush(stdout);
    }

    d_lastAllocateNumBytes = actualNumBytes;
    d_lastAllocateAddress  = addr;

    BSLS_ASSERT(0 == bsls::AlignmentUtil::calculateAlignmentOffset(
                                     addr,
                                     bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT));
    return MemoryBlockDescriptor(addr, actualNumBytes);
}

void TestProtectableBlockDispenser::deallocate(
                                      const MemoryBlockDescriptor& block)
{
    ++d_numDeallocations;
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
            bsl::printf("Deallocate block protected state: %p",
                        block.address());
            if (!d_noAbortFlag) {
                bsl::abort();                                         // ABORT
            }
        }
        return;                                                       // RETURN
    }

    d_lastDeallocateNumBytes = numBytes;
    align->d_object.d_magicNumber = DEALLOCATED_MEMORY;

    d_numBytesInUse -= numBytes;
    --d_numBlocksInUse;

    if (d_verboseFlag) {
        bsl::printf("TestProtectableBlockDispenser: "
                    "freed %d at %p\n",
                    numBytes,
                    block.address());
        bsl::fflush(stdout);
    }

    bsl::memset(block.address(), (int) SCRIBBLED_MEMORY, numBytes);
    bsl::free(align);
}

int TestProtectableBlockDispenser::protect(
                                      const MemoryBlockDescriptor& block)
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

int TestProtectableBlockDispenser::unprotect(
                                      const MemoryBlockDescriptor& block)
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
                bsl::printf("Modification to a protected block detected: %p\n",
                            block.address());
                if (!d_noAbortFlag) {
                    bsl::abort();                                     // ABORT
                }
            }
        }
        align->d_object.d_isProtected = false;
        --d_numBlocksProtected;
    }

    return 0;
}

// ACCESSORS
int TestProtectableBlockDispenser::status() const
{
    enum { BDEMA_MEMORY_LEAK = -1, BDEMA_SUCCESS = 0 };

    return d_numErrors > 0
           ? d_numErrors
           : d_numBlocksInUse || d_numBytesInUse ? BDEMA_MEMORY_LEAK
                                                 : BDEMA_SUCCESS;
}

int TestProtectableBlockDispenser::minimumBlockSize() const
{
    return d_pageSize;
}

bool TestProtectableBlockDispenser::isProtected(
                                const MemoryBlockDescriptor& block) const
{
    Align *align = static_cast<Align *>(block.address()) - 1;
    if (!isValid(align, block, d_quietFlag, d_noAbortFlag)) {
        if (!d_quietFlag) {
            bsl::printf("*** Invalid block supplied to isProtected ***\n");
            if (!d_noAbortFlag) {
                bsl::abort();                                         // ABORT
            }
        }
        return false;                                                 // RETURN
    }

    return align->d_object.d_isProtected;
}

void TestProtectableBlockDispenser::print() const
{
    bsl::printf("\n"
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

    bsl::fflush(stdout);
}
}  // close package namespace

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2008
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
