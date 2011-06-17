// baesu_stacktraceresolver_filehelper.cpp                            -*-C++-*-
#include <baesu_stacktraceresolver_filehelper.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(baesu_stacktraceresolver_filehelper_cpp,"$Id$ $CSID$")

#include <baesu_objectfileformat.h>

#include <bdesu_fileutil.h>
#include <bdeu_string.h>

#include <bslma_allocator.h>
#include <bsls_assert.h>

namespace BloombergLP {

#if defined(BAESU_OBJECTFILEFORMAT_RESOLVER_ELF) || \
    defined(BAESU_OBJECTFILEFORMAT_RESOLVER_XCOFF)

                    // -----------------------------------
                    // baesu_StackTraceResolver_FileHelper
                    // -----------------------------------

// CREATORS
baesu_StackTraceResolver_FileHelper::baesu_StackTraceResolver_FileHelper(
                                                          const char *fileName)
{
    d_fd = bdesu_FileUtil::open(fileName,
                                false,    // not writable
                                true);    // already exists
    BSLS_ASSERT(FileUtil::INVALID_FD != d_fd);
}

baesu_StackTraceResolver_FileHelper::~baesu_StackTraceResolver_FileHelper()
{
    BSLS_ASSERT(FileUtil::INVALID_FD != d_fd);

    bdesu_FileUtil::close(d_fd);
}

// ACCESSORS
char *baesu_StackTraceResolver_FileHelper::loadString(
                                         Offset           offset,
                                         char            *scratchBuf,
                                         int              scratchBufLength,
                                         bslma_Allocator *basicAllocator) const
{
    // We do not know the length of the string and we don't want to read a full
    // 'scratchBufLength' (typically > 32000) bytes every time we read a
    // symbol, so first read 256 bytes, then read more and more up to
    // 'scratchBufLength - 1' bytes, until we get the symbol.  If the length of
    // the symbol is greater than or equal to 'scratchBufLength - 1', it will
    // be truncated.  Allocate a buffer of memory just long enough to fit the
    // 0 terminated string, copy the string into it, and return a pointer to
    // the buffer.

    BSLS_ASSERT_SAFE(scratchBuf);
    BSLS_ASSERT_SAFE(scratchBufLength > 0);

    enum {
        START_LEN = 256
    };

    const int maxString = scratchBufLength - 1;

    int stringLen;
    for (int readLen = START_LEN; true; readLen *= 4) {
        if (readLen > maxString) {
            readLen = maxString;
        }

        int bytes = readBytes(scratchBuf, readLen, offset);
        if (bytes <= 0) {
            // We can't read.  Return "".

            stringLen = 0;
            break;
        }

        BSLS_ASSERT(bytes <= readLen);
        scratchBuf[bytes] = 0;
        stringLen = bsl::strlen(scratchBuf);
        if (stringLen < bytes || bytes < readLen || maxString == readLen) {
            break;
        }
    }

    return bdeu_String::copy(scratchBuf, stringLen, basicAllocator);
}

bsls_Types::UintPtr baesu_StackTraceResolver_FileHelper::readBytes(
                                                         void    *buf,
                                                         UintPtr  numBytes,
                                                         Offset   offset) const
{
    BSLS_ASSERT_SAFE(buf);
    BSLS_ASSERT_SAFE(offset >= 0);

    Offset seekDest = FileUtil::seek(d_fd,
                                     offset,
                                     FileUtil::BDESU_SEEK_FROM_BEGINNING);
    if (seekDest != offset) {
        return 0;                                                     // RETURN
    }

    IntPtr res = FileUtil::read(d_fd, buf, numBytes);
    return (res <= 0 ? 0 : res);
}

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
