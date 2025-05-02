// balst_resolver_filehelper.cpp                                      -*-C++-*-
#include <balst_resolver_filehelper.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(balst_resolver_filehelper_cpp,"$Id$ $CSID$")

#include <balst_objectfileformat.h>

#include <bdls_filesystemutil.h>
#include <bdlb_string.h>

#include <bslma_allocator.h>
#include <bsls_assert.h>

#include <bsl_cstddef.h>
#include <bsl_cstring.h>
#include <bsl_limits.h>

#if defined(BALST_OBJECTFILEFORMAT_RESOLVER_ELF)

namespace BloombergLP {
namespace balst {

                                    // ----------
                                    // FileReader
                                    // ----------

// CREATORS
Resolver_FileHelper::FileReader::FileReader(FileDescriptor fd)
: d_fd(fd)
{
    BSLS_ASSERT(FilesystemUtil::k_INVALID_FD != fd);
}

Resolver_FileHelper::FileReader::~FileReader()
{
    FilesystemUtil::close(d_fd);
}

// ACCESSORS
bsl::string_view Resolver_FileHelper::FileReader::loadString(
                                                      const Span& writeSpan,
                                                      Offset      offset) const
{
    BSLS_ASSERT(0 <= offset);

    using bsl::string_view;

    enum { k_START_READ_LEN = 256 };
    static const size_t npos = string_view::npos;

    if (writeSpan.empty()) {
        return string_view(writeSpan.data(), 0);                      // RETURN
    }

    if (FilesystemUtil::seek(
                            d_fd,
                            offset,
                            FilesystemUtil::e_SEEK_FROM_BEGINNING) != offset) {
        // past EOF

        return string_view(writeSpan.data(), 0);                      // RETURN
    }

    Span spanRemaining(writeSpan);
    ptrdiff_t readLen = k_START_READ_LEN;
    do {
        readLen = bsl::min(readLen, bsl::ssize(spanRemaining));
        BSLS_ASSERT(0 < readLen);
        int rc = FilesystemUtil::read(d_fd,
                                      spanRemaining.data(),
                                      static_cast<int>(readLen));
        if (rc <= 0) {
            break;
        }
        BSLS_ASSERT(rc <= bsl::ssize(spanRemaining));

        const string_view newData(spanRemaining.data(), rc);
        const size_t      zero = newData.find('\0');
        spanRemaining = npos != zero
                      ? spanRemaining.subspan(zero, 0)    // empty --
                                                          // terminates loop
                      : spanRemaining.subspan(rc);

        readLen *= 4;
    } while (!spanRemaining.empty());

    // found `\0`, end of `writeSpan`, end of input, or error on input

    return string_view(writeSpan.data(), spanRemaining.data());
}

bsl::span<const char> Resolver_FileHelper::FileReader::readBytes(
                                                      const Span& outSpan,
                                                      Offset      offset) const
{
    BSLS_ASSERT(offset >= 0);

    if (outSpan.empty()) {
        return outSpan.first(0);                                      // RETURN
    }

    Offset seekDest = FilesystemUtil::seek(
                                        d_fd,
                                        offset,
                                        FilesystemUtil::e_SEEK_FROM_BEGINNING);
    if (seekDest != offset) {
        return outSpan.first(0);                                      // RETURN
    }

    ptrdiff_t res = FilesystemUtil::read(d_fd,
                                         outSpan.data(),
                                         static_cast<int>(outSpan.size()));
    return outSpan.first(res <= 0 ? 0 : res);
}

                                // ----------------
                                // MappedFileReader
                                // ----------------

Resolver_FileHelper::MappedFileReader::MappedFileReader(
                                                       const CSpan& mappedFile)
: d_mappedFile(mappedFile)
{
    BSLS_ASSERT(!mappedFile.empty());
}

bsl::string_view Resolver_FileHelper::MappedFileReader::loadString(
                                                      const Span& writeSpan,
                                                      Offset      offset) const
{
    BSLS_ASSERT(0 <= offset);

    if (bsl::ssize(d_mappedFile) <= offset) {
        // past or at end of mapped file

        return bsl::string_view(writeSpan.data(), 0);                 // RETURN
    }

    typedef CSpan::iterator It;

    const It start    =  d_mappedFile.begin() + static_cast<ptrdiff_t>(offset);
    BSLS_ASSERT(start <= d_mappedFile.end());
    size_t minSize    = bsl::min(
                               d_mappedFile.size(),
                               static_cast<size_t>(offset) + writeSpan.size());
    const It end      = start + minSize;
    const It endFound = bsl::find(start, end, '\0');

    // Copy the result to `writeSpan`.

    bsl::copy(start, endFound, writeSpan.data());
    return bsl::string_view(writeSpan.data(), endFound - start);
}

bsl::span<const char> Resolver_FileHelper::MappedFileReader::readBytes(
                                                    const Span& writeSpan,
                                                    Offset      offsetIn) const
{
    BSLS_ASSERT(offsetIn >= 0);

    if (bsl::ssize(d_mappedFile) <= offsetIn || writeSpan.empty()) {
        return writeSpan.first(0);                                  // RETURN
    }

    const ptrdiff_t  offset   = static_cast<ptrdiff_t>(offsetIn);
    const ptrdiff_t  numBytes = bsl::min(bsl::ssize(writeSpan),
                                         bsl::ssize(d_mappedFile) - offset);
    BSLS_ASSERT(0 < numBytes);
    const char      *start    = d_mappedFile.data() + offset;
    bsl::copy(start, start + numBytes, writeSpan.data());
    return writeSpan.first(numBytes);
}

                                // -------------------
                                // Resolver_FileHelper
                                // -------------------

// CREATORS
Resolver_FileHelper::Resolver_FileHelper()
: d_reader()
{}

// MANIPULATORS
int Resolver_FileHelper::openFile(const char *fileName)
{
    if (!fileName || !*fileName) {
        d_reader.emplace<bsl::monostate>();
        return -1;                                                    // RETURN
    }

    FileDescriptor fd = FilesystemUtil::open(
                                fileName,
                                FilesystemUtil::e_OPEN,       // already exists
                                FilesystemUtil::e_READ_ONLY); // not writable
    if (FilesystemUtil::k_INVALID_FD == fd) {
        d_reader.emplace<bsl::monostate>();
        return -1;
    }

    d_reader.emplace<FileReader>(fd);
    d_fileSize = FilesystemUtil::getFileSize(fd);

    return 0;
}

int Resolver_FileHelper::openMappedFile(const CSpan& mappedFile)
{
    if (mappedFile.empty()) {
        d_reader.emplace<bsl::monostate>();
        return -1;                                                    // RETURN
    }

    d_reader.emplace<MappedFileReader>(mappedFile);
    d_fileSize = mappedFile.size();

    return 0;
}

// ACCESSORS
bsl::string_view Resolver_FileHelper::loadString(const Span& writeSpan,
                                                 Offset      offset) const
{
    BSLS_ASSERT(writeSpan.size() < static_cast<unsigned>(INT_MAX));
    BSLS_ASSERT(0 <= offset);

    if (writeSpan.empty()) {
        return bsl::string_view();                                    // RETURN
    }

    switch (d_reader.index()) {
      case e_OPEN_FILE: {
        const FileReader& fileReader = bsl::get<FileReader>(d_reader);
        return fileReader.loadString(writeSpan, offset);              // RETURN
      } break;
      case e_MAPPED_FILE: {
        const MappedFileReader& mappedFileReader =
                                          bsl::get<MappedFileReader>(d_reader);
        return mappedFileReader.loadString(writeSpan, offset);        // RETURN
      } break;
      default: {
        BSLS_ASSERT_INVOKE_NORETURN(
                               "`loadString` called on unopened `FileHelper`");
      }
    }
}

bsl::span<const char> Resolver_FileHelper::readBytes(const Span& outSpan,
                                                     Offset      offset) const
{
    BSLS_ASSERT(offset >= 0);

    if (outSpan.empty()) {
        return outSpan;                                               // RETURN
    }

    switch (d_reader.index()) {
      case e_OPEN_FILE: {
        const FileReader& fileReader = bsl::get<FileReader>(d_reader);
        return fileReader.      readBytes(outSpan, offset);           // RETURN
      } break;
      case e_MAPPED_FILE: {
        const MappedFileReader& mappedFileReader =
                                          bsl::get<MappedFileReader>(d_reader);
        return mappedFileReader.readBytes(outSpan, offset);           // RETURN
      } break;
      default: {
        BSLS_ASSERT_INVOKE_NORETURN(
                                "`readBytes` called on unopened `FileHelper`");
      }
    }
}

}  // close package namespace
}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2018 Bloomberg Finance L.P.
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

