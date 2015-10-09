// balst_stacktraceresolver_filehelper.h                              -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BALST_STACKTRACERESOLVER_FILEHELPER
#define INCLUDED_BALST_STACKTRACERESOLVER_FILEHELPER

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide platform-independent file input for stack trace resolvers.
//
//@CLASSES:
//   balst::StackTraceResolver_FileHelper: file input for stack trace resolvers
//
//@SEE_ALSO: balst::StackTraceResolverImpl_Elf
//           balst::StackTraceResolverImpl_Xcoff
//
//@DESCRIPTION: The one class in this component opens a file in readonly mode
// and then owns the file descriptor, and provides 3 utility functions for
// reading from the file: 'readBytes', which attempts to read a number of bytes
// into a buffer, and does a partial read if it can't read that many;
// 'readExact', which either reads an exact number of bytes or fails, and
// 'loadString', which reads a 0 terminated string from the file, copies it to
// a buffer it allocates, and returns a pointer to the copy.
//
///Usage
///-----
//..
//  bslma::TestAllocator ta;
//
//  char fileNameBuffer[100];
//  sprintf(fileNameBuffer,
//          "/tmp/balst_StackTraceResolver_FileHelper.usage.%d.txt",
//          getProcessId());
//..
// Make sure file does not already exist.
//..
//  bdls::FilesystemUtil::remove(fileNameBuffer);
//..
// Next, Create the file and open a file descriptor to it.  The boolean
// flags indicate that the file is writable, and not previously existing
// (and therefore must be created).
//..
//  FdType fd = FilesystemUtil::open(fileNameBuffer,
//                             true,          // writable
//                             false);        // doesn't already exist
//  assert(FilesystemUtil::k_INVALID_FD != fd);
//..
// 64 char long string
//..
//  const char *testString64 =
//              "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
//                                                      "0123456789+-";
//..
// Populate the file with known data, with a zero byte at a known offset.
//..
//  int rc;
//  for (int i = 0; i < 20; ++i) {
//      rc = FilesystemUtil::write(fd, testString64, 64);
//      assert(64 == rc);
//  }
//
//  enum { OFFSET_OF_ZERO_BYTE = 7 * 64 };
//
//  rc = (int) FilesystemUtil::seek(fd,
//                            OFFSET_OF_ZERO_BYTE,
//                            FilesystemUtil::e_SEEK_FROM_BEGINNING);
//  assert(OFFSET_OF_ZERO_BYTE == rc);
//
//  rc = FilesystemUtil::write(fd, "", 1);        // write the zero byte
//  assert(1 == rc);
//
//  rc = FilesystemUtil::close(fd);
//  assert(0 == rc);
//
//  {
//      balst::StackTraceResolver_FileHelper helper(fileNameBuffer);
//
//      char buf[100];
//      memset(buf, 0, sizeof(buf));
//      rc = helper.readExact(buf,
//                            6,                    // # chars to read
//                            128);                 // offset
//      assert(0 == rc);
//      assert(!strcmp(buf, "abcdef"));
//..
// 'readExact' past EOF fails
//..
//      rc = helper.readExact(buf,
//                            6,                    // # chars to read
//                            64 * 40);             // offset
//      assert(0 != rc);
//..
// 'loadString' will read a zero terminated string at a given offset,
// using a buffer passed in, and allocating memory for a new copy of
// the string.
//..
//      memset(buf, 'a', sizeof(buf));
//      char *result = helper.loadString(OFFSET_OF_ZERO_BYTE - 12,
//                                       buf,
//                                       sizeof(buf),
//                                       &ta);
//
//      assert(12 == bsl::strlen(result));
//      assert(!bsl::strcmp("0123456789+-", result));
//..
// clean up
//..
//      ta.deallocate(result);
//  }
//  bdls::FilesystemUtil::remove(fileNameBuffer);
//..

#ifndef INCLUDED_BALSCM_VERSION
#include <balscm_version.h>
#endif

#ifndef INCLUDED_BALST_OBJECTFILEFORMAT
#include <balst_objectfileformat.h>
#endif
#if defined(BALST_OBJECTFILEFORMAT_RESOLVER_ELF) || \
    defined(BALST_OBJECTFILEFORMAT_RESOLVER_XCOFF)

#ifndef INCLUDED_BDLS_FILESYSTEMUTIL
#include <bdls_filesystemutil.h>
#endif

#ifndef INCLUDED_BSLMA_ALLOCATOR
#include <bslma_allocator.h>
#endif

#ifndef INCLUDED_BSLS_TYPES
#include <bsls_types.h>
#endif

namespace BloombergLP {

namespace balst {
                    // ===================================
                    // class StackTraceResolver_FileHelper
                    // ===================================

class StackTraceResolver_FileHelper {
    // This class provides a low-level file utility functions for
    // 'StackTraceResolver<Elf>' and 'StackTraceResolver<Xcoff>'.
    // This class contains the file descriptor of current object file.  Note
    // that the file is opened readonly, and all reads specify the offset, so
    // the offset of the file descriptor is not considered part of the state of
    // this object.

    // PRIVATE TYPES
    typedef bdls::FilesystemUtil           FilesystemUtil;
                                                // shorthand for class
                                                // 'bdls::FilesystemUtil'
    typedef FilesystemUtil::FileDescriptor FdType;    // shorthand for file
                                                      // descriptor
    typedef FilesystemUtil::Offset         Offset;
    typedef bsls::Types::UintPtr           UintPtr;
    typedef bsls::Types::IntPtr            IntPtr;

    // DATA
    FdType d_fd;  // file descriptor

  private:
    // NOT IMPLEMENTED
    StackTraceResolver_FileHelper(
                                   const StackTraceResolver_FileHelper&);
    StackTraceResolver_FileHelper& operator=(
                                   const StackTraceResolver_FileHelper&);
  public:
    // CREATORS
    explicit
    StackTraceResolver_FileHelper(const char *fileName);
        // Open the file referred to by the specified 'fileName' for read-only
        // access and set 'd_fd' to the file descriptor.  The behavior is
        // undefined if the specified file does not exist or is unreadable.

    ~StackTraceResolver_FileHelper();
        // Close the file indicated at construction and destroy this object.

    // ACCESSORS
    char *loadString(Offset            offset,
                     char             *scratchBuf,
                     int               scratchBufLength,
                     bslma::Allocator *basicAllocator) const;
        // Load into memory newly allocated from the specified 'basicAllocator'
        // a zero-terminated string from the specified absolute file 'offset',
        // using the specified 'scratchBuf' of specified length
        // 'scratchBufLength' as temporary storage before the new string is
        // allocated.  Return the pointer to the newly allocated string.  The
        // behavior is undefined unless 'scratchBuf != 0' and
        // 'scratchBufLength >= 1'.  Note that if the string is longer than
        // 'scratchBufLength - 1', it is truncated.

    UintPtr readBytes(void *buf, UintPtr numBytes, Offset offset) const;
        // Read into the specified 'buf' up to the specified 'numBytes' of data
        // starting at the specified 'offset' in the current ELF or XCOFF file.
        // Return the number of bytes read, which can be zero.  The behavior is
        // undefined unless 'buf != 0' and 'offset >= 0'.

    int readExact(void *buf, UintPtr numBytes, Offset offset) const;
        // Read into the specified 'buf' exactly the spacified 'numBytes' of
        // data starting at the specified 'offset' in the current ELF or XCOFF
        // file.  Return 0 on success, or a negative value otherwise.  The
        // behavior is undefined unless 'buf != 0' and 'offset >= 0'.
};

// ============================================================================
//                        INLINE FUNCTION DEFINITIONS
// ============================================================================

                     // -----------------------------------
                     // StackTraceResolver_FileHelper
                     // -----------------------------------

// ACCESSORS
inline
int StackTraceResolver_FileHelper::readExact(void    *buf,
                                             UintPtr  numBytes,
                                             Offset   offset) const
{
    BSLS_ASSERT_SAFE(buf);
    BSLS_ASSERT_SAFE(offset >= 0);

    UintPtr res = readBytes(buf, numBytes, offset);
    return res != numBytes ? -1 : 0;
}

}  // close package namespace
}  // close enterprise namespace

#endif

#endif

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
