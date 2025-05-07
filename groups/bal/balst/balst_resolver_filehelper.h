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
//@SEE_ALSO: balst_resolverimpl_elf
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
//  bslma::TestAllocator ta;
//
//  char fileNameBuffer[100];
//  sprintf(fileNameBuffer,
//          "/tmp/balst_Resolver_FileHelper.usage.%d.txt",
//          getProcessId());
// ```
// Make sure file does not already exist.
// ```
//  bdls::FilesystemUtil::remove(fileNameBuffer);
// ```
// Next, Create the file and open a file descriptor to it.  The boolean
// flags indicate that the file is writable, and not previously existing
// (and therefore must be created).
// ```
//  FdType fd = FilesystemUtil::open(
//                  fileNameBuffer,
//                  FilesystemUtil::e_OPEN_OR_CREATE,  // doesn't already exist
//                  FilesystemUtil::e_READ_WRITE);     // writable
//  assert(FilesystemUtil::k_INVALID_FD != fd);
// ```
// 64 char long string
// ```
//  const char *testString64 =
//              "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
//                                                      "0123456789+-";
// ```
// Populate the file with known data, with a zero byte at a known offset.
// ```
//  int rc;
//  for (int i = 0; i < 20; ++i) {
//      rc = FilesystemUtil::write(fd, testString64, 64);
//      assert(64 == rc);
//  }
//
//  enum { OFFSET_OF_ZERO_BYTE = 7 * 64 };
//
//  rc = (int) FilesystemUtil::seek(fd,
//                                  OFFSET_OF_ZERO_BYTE,
//                                  FilesystemUtil::e_SEEK_FROM_BEGINNING);
//  assert(OFFSET_OF_ZERO_BYTE == rc);
//
//  rc = FilesystemUtil::write(fd, "", 1);        // write the zero byte
//  assert(1 == rc);
//
//  rc = FilesystemUtil::close(fd);
//  assert(0 == rc);
//
//  {
//      balst::Resolver_FileHelper helper;
//      rc = helper.openFile(fileNameBuffer);
//      assert(0 == rc);
//
//      char buf[100];    Span bufSpan(buf);
//      memset(buf, 0, sizeof(buf));
//      rc = helper.readExact(Span(buf, 6),
//                            128);                 // offset
//      assert(0 == rc);
//      assert(!strcmp(buf, "abcdef"));
// ```
// `readExact` past EOF fails
// ```
//      rc = helper.readExact(Span(buf, 6),
//                            64 * 40);             // offset
//      assert(0 != rc);
// ```
// `loadString` will read a zero terminated string at a given offset,
// using a buffer passed in, and allocating memory for a new copy of
// the string.
// ```
//      memset(buf, 'a', sizeof(buf));
//      bsl::string_view result = helper.loadString(buf,
//                                                  OFFSET_OF_ZERO_BYTE - 12);
//      assert(12 == result.length());
//      assert("0123456789+-" == result);
//  }
//  bdls::FilesystemUtil::remove(fileNameBuffer);
// ```

#include <balscm_version.h>

#include <balst_objectfileformat.h>

#if defined(BALST_OBJECTFILEFORMAT_RESOLVER_ELF)
#include <bdls_filesystemutil.h>

#include <bslma_allocator.h>

#include <bsls_assert.h>
#include <bsls_review.h>
#include <bsls_types.h>

#include <bsl_span.h>
#include <bsl_variant.h>

namespace BloombergLP {

namespace balst {
                            // =========================
                            // class Resolver_FileHelper
                            // =========================

/// This class provides a low-level file utility functions for `Resolver<Elf>`.
/// This class contains the file descriptor of current object file.  Note that
/// the file is opened readonly, and all reads specify the offset, so the
/// offset of the file descriptor is not considered part of the state of this
/// object.
class Resolver_FileHelper {

    // PRIVATE TYPES
    typedef bdls::FilesystemUtil             FilesystemUtil;
    typedef FilesystemUtil::FileDescriptor   FileDescriptor;
    typedef bsls::Types::UintPtr             UintPtr;
    typedef bsls::Types::IntPtr              IntPtr;
    typedef bsl::span<char>                  Span;
    typedef bsl::span<const char>            CSpan;
    typedef bsl::ptrdiff_t                   ptrdiff_t;

  public:
    // PUBLIC CONSTANTS

    /// length in bytes of d_buffer_p; 32K minus a little so we don't waste a
    /// page
    enum { k_DEFAULT_SCRATCH_BUF_LEN = (1 << 15) - 64 };

    // PUBLIC TYPES
    typedef FilesystemUtil::Offset           Offset;

  private:
    // PRIVATE TYPES
    class FileReader {
        // DATA
        FileDescriptor d_fd;

      public:
        // CREATORS

        /// Bind this file reader to the specified `fd`.  The behavior is
        /// undefined if `fd` is invalid.
        FileReader(FileDescriptor fd);

        /// Close the file to which this `FileReader` is bound.
        ~FileReader();

        // ACCESSORS

        /// The size of the open file.
        Offset fileSize() const;

        /// Load a string, terminated by either a null, by EOF, or by the end
        /// of the file, into the specified `writeSpan` and return a
        /// `string_view` referring to it.  The resulting `string_view` will
        /// refer to memory copied to `writeSpan`.  The behavior is undefined
        /// unless `offset >= 0`.
        bsl::string_view loadString(const Span& writeSpan,
                                    Offset      offset) const;

        /// Read into the specified `writeSpan` data starting at the specified
        /// `offset` in the current ELF file.  Terminate reading either when
        /// the end of `writeSpan` is reached, or EOF.  Return a span of the
        /// bytes read into `writeSpan`.  The behavior is undefined unless
        /// `offset >= 0`.
        CSpan readBytes(const Span& writeSpan, Offset offset) const;
    };

    class MappedFileReader {
        // DATA
        const CSpan d_mappedFile;

      public:
        // CREATORS

        /// Map this reader to the specified `mappedFile`.  The behavior is
        // undefined if `mappedFile` is empty.
        MappedFileReader(const CSpan& mappedFile);

        // ACCESSORS

        /// The size of the mapped segment.
        Offset fileSize() const;

        /// Return a `string_view` referring to a null-terminated string from
        /// `d_mappedFile` beginning at the specified `offset`.  If no `\0` is
        /// found, return a `string_view` ranging from `offset` to the end of
        /// `d_mappedFile`.  The behavior is undefined unless `offset >= 0`.
        /// Note that the string will never be longer than `writeSpan` and the
        /// result will be copied into `writeSpan`.
        bsl::string_view loadString(const Span& writeSpan,
                                    Offset      offset) const;

        /// Read into the specified `writeSpan` data starting at the specified
        /// `offset` in the current ELF file.  Terminate reading either when
        /// the end of `writeSpan` is reached, or EOF.  Return a span of the
        /// bytes read into `writeSpan`.  The behavior is undefined unless
        /// `offset >= 0`.
        CSpan readBytes(const Span& writeSpan, Offset offset) const;
    };

    typedef bsl::variant<bsl::monostate,
                         FileReader,
                         MappedFileReader>   ReaderType;

    enum ReaderIndex { e_MONO_STATE,
                       e_OPEN_FILE,
                       e_MAPPED_FILE };

  private:
    // DATA
    ReaderType d_reader;        // Either null, or a file descriptor of an open
                                // file, or a string_view of a file image in
                                // memory

  private:
    // NOT IMPLEMENTED
    Resolver_FileHelper(const Resolver_FileHelper&);
    Resolver_FileHelper& operator=(const Resolver_FileHelper&);

  public:
    // CREATORS

    /// Create a file helper object in an invalid state.
    Resolver_FileHelper();

    // MANIPULATOR

    /// Open the file referred to by the specified `fileName` for read-only
    /// access, and if it opens successfully, set `d_reader` to a `FileReader`
    /// type and bind it to the opened file.  Return 0 on success and a
    /// non-zero value otherwise.
    int openFile(const char *fileName);

    /// Open this object to access the memory referred to by the specified
    /// `mappedFile` as if it were a disk file.  Return 0 on success and a
    /// non-zero value otherwise.
    int openMappedFile(const CSpan& mappedFile);

    // ACCESSORS

    /// The size of the open file or mapped segment.
    Offset fileSize() const;

    /// Read a null-terminated string from `offset` in the file or mapped file
    /// into `writeSpan` and return a `string_view` to the string.  If we run
    /// out of room in `writeSpan`, reach end of file, or reach end of mapped
    /// area, return a non-null-terminated `string_view`.  The behavior is
    /// undefined unless `!outSpan.empty()` and `offset >= 0`.  Note that the
    /// returned `string_view` never contains the terminating `\0`.
    bsl::string_view loadString(const Span& writeSpan,
                                Offset      offset) const;

    /// Read into the specified `outSpan` up to the specified `outSpan.size()`
    /// of data starting at the specified `offset` in the current ELF file.
    /// Return a span referring to the section of `outSpan` containing, which
    /// can be empty.  The behavior is undefined unless `!outSpan.empty()` and
    /// `offset >= 0`.
    CSpan readBytes(const Span& outSpan, Offset offset) const;

    /// Read into the specified `outSpan` exactly the specified
    /// `outSpan.size()` bytes of data starting at the specified `offset` in
    /// the current ELF file.  Return 0 on success, or a negative value
    /// otherwise.  The behavior is undefined unless `!outSpan.empty()` and
    /// `offset >= 0`.
    int readExact(const Span& outSpan, Offset offset) const;
};

// ============================================================================
//                        INLINE FUNCTION DEFINITIONS
// ============================================================================

                             // -------------------
                             // Resolver_FileHelper
                             // -------------------

// ACCESSORS
inline
int Resolver_FileHelper::readExact(const Span& outSpan, Offset offset) const
{
    BSLS_ASSERT(offset >= 0);

    CSpan res = readBytes(outSpan, offset);
    if (res.size() != outSpan.size() || res.data() != outSpan.data()) {
        return -1;                                                    // RETURN
    }

    return 0;
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
