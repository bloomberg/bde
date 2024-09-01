// balst_resolver_filehelper.h                                        -*-C++-*-
#ifndef INCLUDED_BALST_RESOLVER_FILEHELPER
#define INCLUDED_BALST_RESOLVER_FILEHELPER

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide platform-independent file input for stack trace resolvers.
//
//@CLASSES:
//   balst::Resolver_FileHelper: file input for stack trace resolvers
//
//@SEE_ALSO: balst_resolverimpl_elf, balst_resolverimpl_xcoff
//
//@DESCRIPTION: The one class in this component opens a file in readonly mode
// and then owns the file descriptor, and provides 3 utility functions for
// reading from the file: `readBytes`, which attempts to read a number of bytes
// into a buffer, and does a partial read if it can't read that many;
// `readExact`, which either reads an exact number of bytes or fails, and
// `loadString`, which reads a 0 terminated string from the file, copies it to
// a buffer it allocates, and returns a pointer to the copy.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Basic Usage
/// - - - - - - - - - - -
// First, we prepare the file to be used by this usage example:
// ```
// bslma::TestAllocator ta;
//
// char fileNameBuffer[100];
// sprintf(fileNameBuffer,
//         "/tmp/balst_Resolver_FileHelper.usage.%d.txt",
//         getProcessId());
// ```
// Make sure file does not already exist.
// ```
// bdls::FilesystemUtil::remove(fileNameBuffer);
// ```
// Next, create the file and open a file descriptor to it.  The boolean
// flags indicate that the file is writable, and not previously existing
// (and therefore must be created).
// ```
// FdType fd = FilesystemUtil::open(fileNameBuffer,
//                            true,          // writable
//                            false);        // doesn't already exist
// assert(FilesystemUtil::k_INVALID_FD != fd);
// ```
// 64 char long string
// ```
// const char *testString64 =
//             "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
//                                                     "0123456789+-";
// ```
// Populate the file with known data, with a zero byte at a known offset.
// ```
// int rc;
// for (int i = 0; i < 20; ++i) {
//     rc = FilesystemUtil::write(fd, testString64, 64);
//     assert(64 == rc);
// }
//
// enum { OFFSET_OF_ZERO_BYTE = 7 * 64 };
//
// rc = (int) FilesystemUtil::seek(fd,
//                           OFFSET_OF_ZERO_BYTE,
//                           FilesystemUtil::e_SEEK_FROM_BEGINNING);
// assert(OFFSET_OF_ZERO_BYTE == rc);
//
// rc = FilesystemUtil::write(fd, "", 1);        // write the zero byte
// assert(1 == rc);
//
// rc = FilesystemUtil::close(fd);
// assert(0 == rc);
//
// {
//     balst::Resolver_FileHelper helper(fileNameBuffer);
//
//     char buf[100];
//     memset(buf, 0, sizeof(buf));
//     rc = helper.readExact(buf,
//                           6,                    // # chars to read
//                           128);                 // offset
//     assert(0 == rc);
//     assert(!strcmp(buf, "abcdef"));
// ```
// `readExact` past EOF fails
// ```
//     rc = helper.readExact(buf,
//                           6,                    // # chars to read
//                           64 * 40);             // offset
//     assert(0 != rc);
// ```
// `loadString` will read a zero terminated string at a given offset,
// using a buffer passed in, and allocating memory for a new copy of
// the string.
// ```
//     memset(buf, 'a', sizeof(buf));
//     char *result = helper.loadString(OFFSET_OF_ZERO_BYTE - 12,
//                                      buf,
//                                      sizeof(buf),
//                                      &ta);
//
//     assert(12 == bsl::strlen(result));
//     assert(!bsl::strcmp("0123456789+-", result));
// ```
// clean up
// ```
//     ta.deallocate(result);
// }
// bdls::FilesystemUtil::remove(fileNameBuffer);
// ```

#include <balscm_version.h>

#include <balst_objectfileformat.h>

#if defined(BALST_OBJECTFILEFORMAT_RESOLVER_ELF) || \
    defined(BALST_OBJECTFILEFORMAT_RESOLVER_XCOFF)
#include <bdls_filesystemutil.h>

#include <bslma_allocator.h>

#include <bsls_assert.h>
#include <bsls_review.h>
#include <bsls_types.h>

namespace BloombergLP {

namespace balst {
                            // =========================
                            // class Resolver_FileHelper
                            // =========================

/// This class provides a low-level file utility functions for
/// `Resolver<Elf>` and `Resolver<Xcoff>`.  This class contains the file
/// descriptor of current object file.  Note that the file is opened
/// readonly, and all reads specify the offset, so the offset of the file
/// descriptor is not considered part of the state of this object.
class Resolver_FileHelper {

    // PRIVATE TYPES
    typedef bdls::FilesystemUtil    FilesystemUtil;
    typedef FilesystemUtil::Offset  Offset;
    typedef bsls::Types::UintPtr    UintPtr;
    typedef bsls::Types::IntPtr     IntPtr;

    // DATA
    FilesystemUtil::FileDescriptor d_fd;  // file descriptor

  private:
    // NOT IMPLEMENTED
    Resolver_FileHelper(const Resolver_FileHelper&);
    Resolver_FileHelper& operator=(
                                         const Resolver_FileHelper&);
  public:
    // CREATORS

    /// Create a file helper object in an invalid state.
    explicit
    Resolver_FileHelper();

    /// Close any file currently opened by this object and destroy it.
    ~Resolver_FileHelper();

    // MANIPULATOR

    /// Open the file referred to by the specified `fileName` for read-only
    /// access and set `d_fd` to the file descriptor.  Return 0 on success
    /// and a non-zero value otherwise.  If this object already had a file
    /// descriptor open, close it before opening `fileName`.  If the `open`
    /// call fails, `d_fd` will be set to `FilesystemUtil::k_INVALID_FD`.
    int initialize(const char *fileName);

    // ACCESSORS

    /// Load into memory newly allocated from the specified `basicAllocator`
    /// a zero-terminated string from the specified absolute file `offset`,
    /// using the specified `scratchBuf` of specified length
    /// `scratchBufLength` as temporary storage before the new string is
    /// allocated.  Return the pointer to the newly allocated string.  The
    /// behavior is undefined unless `scratchBuf != 0` and
    /// `scratchBufLength >= 1`.  Note that if the string is longer than
    /// `scratchBufLength - 1`, it is truncated.
    char *loadString(Offset            offset,
                     char             *scratchBuf,
                     UintPtr           scratchBufLength,
                     bslma::Allocator *basicAllocator) const;

    /// Read into the specified `buf` up to the specified `numBytes` of data
    /// starting at the specified `offset` in the current ELF or XCOFF file.
    /// Return the number of bytes read, which can be zero.  The behavior is
    /// undefined unless `buf != 0` and `offset >= 0`.
    UintPtr readBytes(void *buf, UintPtr numBytes, Offset offset) const;

    /// Read into the specified `buf` exactly the specified `numBytes` of
    /// data starting at the specified `offset` in the current ELF or XCOFF
    /// file.  Return 0 on success, or a negative value otherwise.  The
    /// behavior is undefined unless `buf != 0` and `offset >= 0`.
    int readExact(void *buf, UintPtr numBytes, Offset offset) const;
};

// ============================================================================
//                        INLINE FUNCTION DEFINITIONS
// ============================================================================

                     // -----------------------------------
                     // Resolver_FileHelper
                     // -----------------------------------

// ACCESSORS
inline
int Resolver_FileHelper::readExact(void    *buf,
                                   UintPtr  numBytes,
                                   Offset   offset) const
{
    BSLS_ASSERT(buf);
    BSLS_ASSERT(offset >= 0);

    UintPtr res = readBytes(buf, numBytes, offset);
    return res != numBytes ? -1 : 0;
}

}  // close package namespace
}  // close enterprise namespace

#endif

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
