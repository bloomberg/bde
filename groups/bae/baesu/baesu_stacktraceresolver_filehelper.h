// baesu_stacktraceresolver_filehelper.h                              -*-C++-*-
#ifndef INCLUDED_BAESU_STACKTRACERESOLVER_FILEHELPER
#define INCLUDED_BAESU_STACKTRACERESOLVER_FILEHELPER

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide platform-independent file input for stack trace resolvers.
//
//@CLASSES:
//   baesu_StackTraceResolver_FileHelper: file input for stack trace resolvers
//
//@SEE_ALSO: baesu_StackTraceResolverImpl_Elf
//           baesu_StackTraceResolverImpl_Xcoff
//
//@AUTHOR: Bill Chapman
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
//  bslma_TestAllocator ta;
//
//  char fileNameBuffer[100];
//  sprintf(fileNameBuffer,
//          "/tmp/baesu_StackTraceResolver_FileHelper.usage.%d.txt",
//          getProcessId());
//..
// Make sure file does not already exist.
//..
//  bdesu_FileUtil::remove(fileNameBuffer);
//..
// Next, Create the file and open a file descriptor to it.  The boolean
// flags indicate that the file is writable, and not previously existing
// (and therefore must be created).
//..
//  FdType fd = FileUtil::open(fileNameBuffer,
//                             true,          // writable
//                             false);        // doesn't already exist
//  assert(FileUtil::INVALID_FD != fd);
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
//      rc = FileUtil::write(fd, testString64, 64);
//      assert(64 == rc);
//  }
//
//  enum { OFFSET_OF_ZERO_BYTE = 7 * 64 };
//
//  rc = (int) FileUtil::seek(fd,
//                            OFFSET_OF_ZERO_BYTE,
//                            FileUtil::BDESU_SEEK_FROM_BEGINNING);
//  assert(OFFSET_OF_ZERO_BYTE == rc);
//
//  rc = FileUtil::write(fd, "", 1);        // write the zero byte
//  assert(1 == rc);
//
//  rc = FileUtil::close(fd);
//  assert(0 == rc);
//
//  {
//      baesu_StackTraceResolver_FileHelper helper(fileNameBuffer);
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
//  bdesu_FileUtil::remove(fileNameBuffer);
//..

#ifndef INCLUDED_BAESCM_VERSION
#include <baescm_version.h>
#endif

#ifndef INCLUDED_BAESU_OBJECTFILEFORMAT
#include <baesu_objectfileformat.h>
#endif
#if defined(BAESU_OBJECTFILEFORMAT_RESOLVER_ELF) || \
    defined(BAESU_OBJECTFILEFORMAT_RESOLVER_XCOFF)

#ifndef INCLUDED_BDESU_FILEUTIL
#include <bdesu_fileutil.h>
#endif

#ifndef INCLUDED_BSLS_TYPES
#include <bsls_types.h>
#endif

#ifndef INCLUDED_BSLFWD_BSLMA_ALLOCATOR
#include <bslfwd_bslma_allocator.h>
#endif

namespace BloombergLP {

                 // =========================================
                 // class baesu_StackTraceResolver_FileHelper
                 // =========================================

class baesu_StackTraceResolver_FileHelper {
    // This class provides a low-level file utility functions for
    // 'baesu_StackTraceResolver<Elf>' and 'baesu_StackTraceResolver<Xcoff>'.
    // This class contains the file descriptor of current object file.  Note
    // that the file is opened readonly, and all reads specify the offset, so
    // the offset of the file descriptor is not considered part of the state of
    // this object.

    // PRIVATE TYPES
    typedef bdesu_FileUtil           FileUtil;  // shorthand for class
                                                // 'bdesu_FileUtil'
    typedef FileUtil::FileDescriptor FdType;    // shorthand for file
                                                // descriptor
    typedef FileUtil::Offset         Offset;
    typedef bsls_Types::UintPtr      UintPtr;
    typedef bsls_Types::IntPtr       IntPtr;

    // DATA
    FdType d_fd;  // file descriptor

  private:
    // NOT IMPLEMENTED
    baesu_StackTraceResolver_FileHelper(
                                   const baesu_StackTraceResolver_FileHelper&);
    baesu_StackTraceResolver_FileHelper& operator=(
                                   const baesu_StackTraceResolver_FileHelper&);
  public:
    // CREATORS
    explicit
    baesu_StackTraceResolver_FileHelper(const char *fileName);
        // Open the file referred to by the specified 'fileName' for read-only
        // access and set 'd_fd' to the file descriptor.  The behavior is
        // undefined if the specified file does not exist or is unreadable.

    ~baesu_StackTraceResolver_FileHelper();
        // Close the file indicated at construction and destroy this object.

    // ACCESSORS
    char *loadString(Offset           offset,
                     char            *scratchBuf,
                     int              scratchBufLength,
                     bslma_Allocator *basicAllocator) const;
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

// ===========================================================================
//                         INLINE FUNCTION DEFINITIONS
// ===========================================================================

                     // -----------------------------------
                     // baesu_StackTraceResolver_FileHelper
                     // -----------------------------------

// ACCESSORS
inline
int baesu_StackTraceResolver_FileHelper::readExact(void    *buf,
                                                   UintPtr  numBytes,
                                                   Offset   offset) const
{
    BSLS_ASSERT_SAFE(buf);
    BSLS_ASSERT_SAFE(offset >= 0);

    UintPtr res = readBytes(buf, numBytes, offset);
    return res != numBytes ? -1 : 0;
}

}  // close namespace BloombergLP

#endif

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2010
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
