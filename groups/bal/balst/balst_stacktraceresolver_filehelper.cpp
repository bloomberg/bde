// balst_stacktraceresolver_filehelper.cpp                            -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <balst_stacktraceresolver_filehelper.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(balst_stacktraceresolver_filehelper_cpp,"$Id$ $CSID$")

#include <balst_objectfileformat.h>

#include <bdls_filesystemutil.h>
#include <bdlb_string.h>

#include <bslma_allocator.h>
#include <bsls_assert.h>

#include <bsl_cstring.h>

namespace BloombergLP {

#if defined(BALST_OBJECTFILEFORMAT_RESOLVER_ELF) || \
    defined(BALST_OBJECTFILEFORMAT_RESOLVER_XCOFF)

namespace balst {
                    // -----------------------------------
                    // StackTraceResolver_FileHelper
                    // -----------------------------------

// CREATORS
StackTraceResolver_FileHelper::StackTraceResolver_FileHelper(
                                                          const char *fileName)
{
    BSLS_ASSERT(fileName);

    d_fd = bdls::FilesystemUtil::open(
                        fileName,
                        bdls::FilesystemUtil::e_OPEN,        // already exists
                        bdls::FilesystemUtil::e_READ_ONLY);  // not writable
    BSLS_ASSERT(FilesystemUtil::k_INVALID_FD != d_fd);
}

StackTraceResolver_FileHelper::~StackTraceResolver_FileHelper()
{
    BSLS_ASSERT(FilesystemUtil::k_INVALID_FD != d_fd);

    bdls::FilesystemUtil::close(d_fd);
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
        k_START_LEN = 256
    };

    const int maxString = scratchBufLength - 1;

    int stringLen;
    for (int readLen = k_START_LEN; true; readLen *= 4) {
        if (readLen > maxString) {
            readLen = maxString;
        }

        int bytes = static_cast<int>(readBytes(scratchBuf, readLen, offset));
        if (0 == bytes) {
            // We can't read.  Return "".

            stringLen = 0;
            break;
        }

        BSLS_ASSERT(bytes <= readLen);
        scratchBuf[bytes] = 0;
        stringLen = static_cast<int>(bsl::strlen(scratchBuf));
        if (stringLen < bytes || bytes < readLen || maxString == readLen) {
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

    Offset seekDest = FilesystemUtil::seek(
                                        d_fd,
                                        offset,
                                        FilesystemUtil::e_SEEK_FROM_BEGINNING);
    if (seekDest != offset) {
        return 0;                                                     // RETURN
    }

    int res = FilesystemUtil::read(d_fd, buf, static_cast<int>(numBytes));
    return (res <= 0 ? 0 : res);
}
}  // close package namespace

#endif

}  // close enterprise namespace

// ----------------------------------------------------------------------------
// Copyright 2015 Bloomberg Finance L.P.
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
