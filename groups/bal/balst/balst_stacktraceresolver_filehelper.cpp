// balst_stacktraceresolver_filehelper.cpp                            -*-C++-*-
#include <balst_stacktraceresolver_filehelper.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(balst_stacktraceresolver_filehelper_cpp,"$Id$ $CSID$")

#include <balst_objectfileformat.h>

#include <bdlsu_filesystemutil.h>
#include <bdlb_string.h>

#include <bslma_allocator.h>
#include <bsls_assert.h>

namespace BloombergLP {

#if defined(BAESU_OBJECTFILEFORMAT_RESOLVER_ELF) || \
    defined(BAESU_OBJECTFILEFORMAT_RESOLVER_XCOFF)

namespace balst {
                    // -----------------------------------
                    // StackTraceResolver_FileHelper
                    // -----------------------------------

// CREATORS
StackTraceResolver_FileHelper::StackTraceResolver_FileHelper(
                                                          const char *fileName)
{
    BSLS_ASSERT(fileName);

    d_fd = bdlsu::FilesystemUtil::open(
                        fileName,
                        bdlsu::FilesystemUtil::e_OPEN,        // already exists
                        bdlsu::FilesystemUtil::e_READ_ONLY);  // not writable
    BSLS_ASSERT(FilesystemUtil::k_INVALID_FD != d_fd);
}

StackTraceResolver_FileHelper::~StackTraceResolver_FileHelper()
{
    BSLS_ASSERT(FilesystemUtil::k_INVALID_FD != d_fd);

    bdlsu::FilesystemUtil::close(d_fd);
}

// ACCESSORS
char *StackTraceResolver_FileHelper::loadString(
                                        Offset            offset,
                                        char             *scratchBuf,
                                        int               scratchBufLength,
                                        bslma::Allocator *basicAllocator) const
{
    // We do not know the length of the string and we don't want to read a full
    // 'scratchBufLength' (typically > 32000) bytes every time we read a
    // symbol, so first read 256 bytes, then read more and more up to
    // 'scratchBufLength - 1' bytes, until we get the symbol.  If the length of
    // the symbol is greater than or equal to 'scratchBufLength - 1', it will
    // be truncated.  Allocate a buffer of memory just long enough to fit the
    // 0 terminated string, copy the string into it, and return a pointer to
    // the buffer.

    BSLS_ASSERT(scratchBuf);
    BSLS_ASSERT(scratchBufLength > 0);

    enum {
        START_LEN = 256
    };

    const int maxString = scratchBufLength - 1;

    int stringLen;
    for (int readLen = START_LEN; true; readLen *= 4) {
        if (readLen > maxString) {
            readLen = maxString;
        }

        UintPtr bytes = readBytes(scratchBuf, readLen, offset);
        if (0 == bytes) {
            // We can't read.  Return "".

            stringLen = 0;
            break;
        }

        BSLS_ASSERT((int) bytes <= readLen);
        scratchBuf[bytes] = 0;
        stringLen = (int) bsl::strlen(scratchBuf);
        if (stringLen < (int) bytes || (int) bytes < readLen ||
                                                        maxString == readLen) {
            break;
        }
    }

    return bdlb::String::copy(scratchBuf, stringLen, basicAllocator);
}

bsls::Types::UintPtr StackTraceResolver_FileHelper::readBytes(
                                                         void    *buf,
                                                         UintPtr  numBytes,
                                                         Offset   offset) const
{
    BSLS_ASSERT(buf);
    BSLS_ASSERT(offset >= 0);

    Offset seekDest = FilesystemUtil::seek(d_fd,
                                     offset,
                                     FilesystemUtil::e_SEEK_FROM_BEGINNING);
    if (seekDest != offset) {
        return 0;                                                     // RETURN
    }

    int res = FilesystemUtil::read(d_fd, buf, (int) numBytes);
    return (res <= 0 ? 0 : res);
}
}  // close package namespace

#endif

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2010
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
