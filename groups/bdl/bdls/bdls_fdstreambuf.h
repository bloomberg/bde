// bdls_fdstreambuf.h                                                 -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BDLS_FDSTREAMBUF
#define INCLUDED_BDLS_FDSTREAMBUF

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a stream buffer initialized with a file descriptor.
//
//@CLASSES:
//   bdls::FdStreamBuf: stream buffer constructed with file descriptor
//
//@SEE_ALSO: <bsl::streambuf>
//
//@DESCRIPTION: This component implements a class, 'bdls::FdStreamBuf', derived
// from the C++ standard library's 'bsl::streambuf' that can be associated with
// a file descriptor.  Except for the 'pubimbue' function, all of the actions
// that can be performed on an 'bsl::streambuf' can be performed on a
// 'bdls::FdStreamBuf'.  An 'bsl::streambuf' provides public methods for
// reading from and writing to a stream of data, which are implemented in terms
// of protected virtual functions.  A 'bdls::FdStreamBuf' provides an
// implementation of these protected virtual members such that they operate on
// a given file descriptor.  The file descriptor can represent a file, a pipe,
// or other device, and it can be associated with the 'bdls::FdStreamBuf' at
// construction, or by calling the 'reset' method.  Note that a 'bsl::stream'
// can be initialized with a 'bdls::FdStreamBuf', making it possible to
// associate the stream with a file descriptor.
//
// Note that the 'pubimbue' function may be called, but not with any value
// other than 'bsl::locale()'.  Furthermore, when called with this value, it
// has no effect.
//
// The file descriptor type 'bdls::FilesystemUtil::FileDescriptor' used in this
// component is, on Unix, an 'int' type returned by 'open', and on Windows, a
// 'HANDLE' type returned by 'CreateFile'.  Ideally, a user would open the file
// and obtain the platform-independent 'bdls::FilesystemUtil::FileDescriptor'
// by calling 'bdls::FilesystemUtil::open', which will call the appropriate
// routine for the platform and return a
// 'bdls::FilesystemUtil::FileDescriptor'.  A value of
// 'bdls::FilesystemUtil::k_INVALID_FD' is used to represent an invalid file
// handle on both platforms.
//
// On Windows for a file in text mode, the byte '0x1a' (ctrl-Z) is recognized
// as an end of file marker.  If it is encountered, it is not returned in the
// buffer and subsequent reads will indicate that no more input is available.
// The behavior is undefined if it is not the last byte in the file.  Other
// types of files are not required to end with '0x1a'.  For files on Unix and
// files opened in binary mode on Windows, '0x1a' is treated like any other
// byte.
//
// Note that the public methods of the 'bsl::streambuf' class used in the usage
// example are not described here.  See documentation in
// "The C++ Programming Language, Third Edition", by Bjarne Stroustrup,
// Section 21.6.4, and on the web at:
//..
// http://www.cplusplus.com/reference/iostream/streambuf
//..
// Note that the 'bdls::FdStreamBuf' and 'bdls::FdStreamBuf_FileHandler'
// classes here are based on STLPort's implementation of 'filebuf' and
// '_Filebuf_Base' respectively, with copyright notice as follows:
//..
// ----------------------------------------------------------------------------
// Copyright (c) 1999
// Silicon Graphics Computer Systems, Inc.
//
// Copyright (c) 1999
// Boris Fomitchev
//
// This material is provided "as is", with absolutely no warranty expressed
// or implied.  Any use is at your own risk.
//
// Permission to use or copy this software for any purpose is hereby granted
// without fee, provided the above notices are retained on all copies.
// Permission to modify the code and to distribute modified code is granted,
// provided the above notices are retained, and a notice that the code was
// modified is included with the above copyright notice.
// ----------------------------------------------------------------------------
//..
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Stream Initializaton
///- - - - - - - - - - - - - - - -
// The most common usage of this component is to initialize a stream.  In this
// case, the 'bdls::FdStreamBuf' will be used for either input or output, but
// not both.
//
// First we create a suitable file name, and make sure that no file of that
// name already exists:
//..
//  char fileNameBuffer[100];
//  bsl::sprintf(fileNameBuffer,
//#ifdef BSLS_PLATFORM_OS_UNIX
//               "/tmp/bdls_FdStreamBuf.usage.1.%d.txt",
//#else // windows
//               "C:\\TEMP\\bdls_FdStreamBuf.usage.1.%d.txt";
//#endif
//               bdls::ProcessUtil::getProcessId());
//
//  bdls::FilesystemUtil::remove(fileNameBuffer);
//  assert(0 == bdls::FilesystemUtil::exists(fileNameBuffer));
//..
// Then we create the file and open a file descriptor to it; the boolean
// flags indicate that the file is to be writable, and not previously existing
// (and therefore must be created):
//..
//  typedef bdls::FilesystemUtil::FileDescriptor FdType;
//
//  FdType fd = bdls::FilesystemUtil::open(fileNameBuffer,
//                                         bdls::FilesystemUtil::e_CREATE,
//                                         bdls::FilesystemUtil::e_READ_WRITE);
//  assert(bdls::FilesystemUtil::k_INVALID_FD != fd);
//..
// Next we create a 'bdls::FdStreamBuf' associated with file descriptor
// 'fd'; the 'false' argument indicates that 'streamBuffer' will not assume
// ownership of 'fd', meaning that when 'streamBuffer' is destroyed 'fd' will
// remain open:
//
// Note also that the stream buffer defaults to being in text mode on Windows,
// and binary mode on Unix.
//..
//  {
//      bdls::FdStreamBuf streamBuffer(fd,
//                                     true,    // writable
//                                     false);  // 'fd' won't be closed
//                                              // when 'streamBuffer' is
//                                              // destroyed
//
//      bsl::ostream os(&streamBuffer);
//
//      os << "Five times nine point five = " << 5 * 9.5 << bsl::endl;
//  }
//..
// Note also that the stream buffer defaults to being in text mode on
// Windows, and binary mode on Unix.
//
// Now create a new stream buffer to read the file back, in this case
// using binary mode so we can see exactly what was written.  The new
// stream buf is used to initialize an input stream.
//..
//  {
//      // read it in binary mode
//
//      bdls::FdStreamBuf streamBuffer(fd,
//                                     false,  // not writable
//                                     false,  // 'streamBuffer' does not
//                                             // own 'fd'
//                                     true);  // binary mode
//
//      streamBuffer.pubseekpos(0);
//
//      char buf[100];
//      bsl::memset(buf, 0, sizeof(buf));
//
//      bsl::istream is(&streamBuffer);
//      char *pc = buf;
//      do {
//          is >> bsl::noskipws >> *pc++;
//      } while ('\n' != pc[-1]);
//
//#ifdef BSLS_PLATFORM_OS_UNIX
//      assert(!bsl::strcmp("Five times nine point five = 47.5\n", buf));
//#else
//      //On Windows we see a CRLF ('\r\n') instead of a simple LF '\n'
//      assert(!bsl::strcmp("Five times nine point five = 47.5\r\n", buf));
//#endif
//  }
//..
// Finally, read the file back a second time, this time in text mode.  Note
// how, on Windows, the '\r\n' is translated back to '\n'
//..
//  {
//      // read it back in text mode
//
//      bdls::FdStreamBuf streamBuffer(fd,
//                                     false);  // not writable
//                                              // 'fd' will be closed when
//                                              // streamBuffer is destroyed.
//                                              // Mode will be binary on
//                                              // Unix, text on Dos.
//      streamBuffer.pubseekpos(0);
//
//      char buf[100];
//      bsl::memset(buf, 0, sizeof(buf));
//
//      bsl::istream is(&streamBuffer);
//      char *pc = buf;
//      do {
//          is >> bsl::noskipws >> *pc++;
//      } while ('\n' != pc[-1]);
//
//      assert(!bsl::strcmp("Five times nine point five = 47.5\n", buf));
//  }
//..
// And finally, we clean up:
//..
//  bdls::FilesystemUtil::remove(fileNameBuffer);
//..
//
///Example 2: Streambuf
/// - - - - - - - - - -
// For our second example we will create a 'bdls::FdStreamBuf' associated with
// a temporary file, and then use the public methods of the base class
// interface, including 'sputn', 'sgetn' and 'pubseekpos', to do some I/O and
// seeking on it.
//..
//  const char line1[] = "To be or not to be, that is the question.\n";
//  const char line2[] =
//                     "There are more things in heaven and earth,\n"
//                     "Horatio, than are dreamt of in your philosophy.\n";
//  const char line3[] = "Wherever you go, there you are.  B Banzai\n";
//
//  const int lengthLine1 = sizeof(line1) - 1;
//  const int lengthLine2 = sizeof(line2) - 1;
//  const int lengthLine3 = sizeof(line3) - 1;
//..
// We start by selecting a file name for our (temporary) file.
//..
//  char fileNameBuffer[100];
//  bsl::sprintf(fileNameBuffer,
//#ifdef BSLS_PLATFORM_OS_UNIX
//               "/tmp/bdls_FdStreamBuf.usage.2.%d.txt",
//#else // windows
//               "C:\\TEMP\\bdls_FdStreamBuf.usage.2.%d.txt",
//#endif
//               bdls::ProcessUtil::getProcessId());
//..
// Then, make sure the file does not already exist:
//..
//  bdls::FilesystemUtil::remove(fileNameBuffer);
//  assert(false == bdls::FilesystemUtil::exists(fileNameBuffer));
//..
// Next, Create the file and open a file descriptor to it.  The boolean
// flags indicate that the file is writable, and not previously
// existing (and therefore must be created):
//..
//  typedef bdls::FilesystemUtil::FileDescriptor FdType;
//
//  FdType fd = bdls::FilesystemUtil::open(fileNameBuffer,
//                                         bdls::FilesystemUtil::e_CREATE,
//                                         bdls::FilesystemUtil::e_READ_WRITE);
//  assert(bdls::FilesystemUtil::k_INVALID_FD != fd);
//..
// Now, we create a 'bdls::FdStreamBuf' object named 'streamBuffer'
// associated with the file descriptor 'fd'.  Note that 'streamBuffer'
// defaults to assuming ownership of 'fd', meaning that when
// 'streamBuffer' is cleared, reset, or destroyed, 'fd' will be closed.
// Note that 'FdStreamBuf' implements 'streambuf', which provides the
// public methods used in this example:
//..
//  bdls::FdStreamBuf streamBuffer(fd, true);
//
//  assert(streamBuffer.fileDescriptor() == fd);
//  assert(streamBuffer.isOpened());
//..
// Next we use the 'sputn' method to write two lines to the file:
//..
//  streamBuffer.sputn(line1, lengthLine1);
//  streamBuffer.sputn(line2, lengthLine2);
//..
// Then we seek back to the start of the file.
//..
//  bsl::streamoff status = streamBuffer.pubseekpos(0);
//  assert(0 == status);
//..
// Next, we read the first 'lengthLine1' characters of the file
// into 'buf', with the method 'sgetn'.
//..
//  char buf[1000];
//  bsl::memset(buf, 0, sizeof(buf));
//  status = streamBuffer.sgetn(buf, lengthLine1);
//  assert(lengthLine1 == status);
//  assert(!bsl::strcmp(line1, buf));
//..
// Next we try to read '2 * lengthLine2' characters when only
// 'lengthLine2' characters are available in the file to read, so
// the 'sgetn' method will stop after reading 'lengthLine2' characters.
// The 'sgetn' method will return the number of chars successfully
// read:
//..
//  bsl::memset(buf, 0, sizeof(buf));
//  status =  streamBuffer.sgetn(buf, 2 * lengthLine2);
//  assert(lengthLine2 == status);
//  assert(!bsl::strcmp(line2, buf));
//..
// Trying to read past the end of the file invalidated the current
// cursor position in the file, so we must seek from the end or the
// beginning of the file in order to establish a new cursor position.
// Note the 'pubseekpos' method always seeks relative to the beginning.
// We seek back to the start of the file:
//..
//  status = streamBuffer.pubseekpos(0);
//  assert(0 == status);
//..
// Note that line1 and line3 are the same length:
//..
//  assert(lengthLine1 == lengthLine3);
//..
// Then we write, replacing 'line1' in the file with 'line3':
//..
//  status = streamBuffer.sputn(line3, lengthLine3);
//  assert(lengthLine3 == status);
//..
// Now we seek back to the beginning of the file:
//..
//  status = streamBuffer.pubseekpos(0);
//..
// Next we verify we were returned to the start of the file:
//..
//  assert(0 == status);
//..
// Then we read and verify the first line, which now contains the text
// of 'line3':
//..
//  bsl::memset(buf, 0, sizeof(buf));
//  status = streamBuffer.sgetn(buf, lengthLine3);
//  assert(lengthLine3 == status);
//  assert(!bsl::strcmp(line3, buf));
//..
// Now we read and verify the second line, still 'line2':
//..
//  bsl::memset(buf, 0, sizeof(buf));
//  status = streamBuffer.sgetn(buf, lengthLine2);
//  assert(lengthLine2 == status);
//  assert(!bsl::strcmp(line2, buf));
//..
// Next we close 'fd' and disconnect 'streamBuffer' from 'fd':
//..
//  status = streamBuffer.clear();
//  assert(0 == status);
//..
// Note that 'streamBuffer' is now no longer open, and is not
// associated with a file descriptor:
//..
//  assert(!streamBuffer.isOpened());
//  assert(bdls::FilesystemUtil::k_INVALID_FD ==
//         streamBuffer.fileDescriptor());
//..
// Finally, we clean up the file:
//..
//  bdls::FilesystemUtil::remove(fileNameBuffer);
//..

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

#ifndef INCLUDED_BDLS_FILESYSTEMUTIL
#include <bdls_filesystemutil.h>
#endif

#ifndef INCLUDED_BSLMA_ALLOCATOR
#include <bslma_allocator.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSLS_ATOMICOPERATIONS
#include <bsls_atomicoperations.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORM
#include <bsls_platform.h>
#endif

#ifndef INCLUDED_BSLS_TYPES
#include <bsls_types.h>
#endif

#ifndef INCLUDED_BSL_ALGORITHM
#include <bsl_algorithm.h>
#endif

#ifndef INCLUDED_BSL_IOS
#include <bsl_ios.h>
#endif

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif

#ifndef INCLUDED_BSL_LOCALE
#include <bsl_locale.h>
#endif

#ifndef INCLUDED_BSL_STREAMBUF
#include <bsl_streambuf.h>  // char_type, int_type, pos_type, off_type,
                            // traits_type are within the 'bsl::streambuf'
                            // class
#endif

#ifndef INCLUDED_BSL_CSTRING
#include <bsl_cstring.h>          // size_t
#endif

namespace BloombergLP {

namespace bdls {
                    // ====================================
                    // helper class FdStreamBuf_FileHandler
                    // ====================================

class FdStreamBuf_FileHandler {
    // This private helper class isolates direct operations on files from the
    // 'FdStreamBuf' class; it is a thin wrapper around 'FilesystemUtil'.  One
    // service this class provides is converting between an in-process '\n' and
    // its corresponding on-file '\r\n' when writing to or reading from a
    // Windows text file.  On 'reset' an object of this type is associated with
    // a supplied file descriptor, after which it can do simple operations on
    // that file descriptor in the service of a 'FdStreamBuf'.

  private:
    // CLASS DATA
    static bsls::AtomicOperations::AtomicTypes::Int
           s_pageSize;             // page size associated with this operating
                                   // system

    // DATA
    FilesystemUtil::FileDescriptor
           d_fileId;               // file descriptor, which is owned if
                                   // 'd_willCloseOnResetFlag' is 'true',
                                   // otherwise not owned

    bool   d_openedFlag;           // 'true' if this object is associated with
                                   // a valid file descriptor, and 'false'
                                   // otherwise

    bool   d_regularFileFlag;      // 'true' if the file descriptor represents
                                   // a plain file (and not a directory or
                                   // other device), and 'false' otherwise

    bsl::ios_base::openmode
           d_openModeFlags;        // 'ios_base'-style flags with which the
                                   // file or device was opened

    bool   d_willCloseOnResetFlag; // 'true' if the file descriptor should be
                                   // closed when this file handler is reset,
                                   // cleared or destroyed, and 'false'
                                   // otherwise

    char   d_peekBuffer;           // buffer used when looking one byte ahead
                                   // to complete a '\r\n' in text mode

    bool   d_peekBufferFlag;       // 'true' if peek buffer contains a
                                   // character, 'false' otherwise.  Note this
                                   // is never true on Unix or in binary mode
                                   // on Windows.

  private:
    // PRIVATE MANIPULATORS
#ifdef BSLS_PLATFORM_OS_WINDOWS
    int windowsWriteText(const char *buffer, int numChars);
        // Write the specified 'numChars' characters from the specified
        // 'buffer' to this object's file descriptor.  Return the number of
        // characters successfully written on success, and a negative value
        // otherwise.  Note that '\n's in the specified 'buffer' will be
        // translated to '\r\n' sequences on output.  Also note that this
        // method does not exist and is not called except on Windows.
#endif

  private:
    // NOT IMPLEMENTED
    FdStreamBuf_FileHandler(const FdStreamBuf_FileHandler &);
    FdStreamBuf_FileHandler &operator=(const FdStreamBuf_FileHandler &);

  public:
    // CLASS METHODS
    static bsl::size_t pageSize();
        // Return the operating system's page size.

    // CREATORS
    FdStreamBuf_FileHandler();
        // Create a file handler that is not associated with any file
        // descriptor.  Note that 'isOpened' will be 'false' on the newly
        // created object.

    ~FdStreamBuf_FileHandler();
        // Destroy this file handler.  If 'willCloseOnReset' is 'true', close
        // any file descriptor associated with this object.

    // MANIPULATORS
    int reset(FilesystemUtil::FileDescriptor fileDescriptor,
              bool                           writableFlag,
              bool                           willCloseOnResetFlag = true,
              bool                           binaryModeFlag = false);
        // Associate this object with the specified 'fileDescriptor', and
        // record the state of the specified 'writableFlag' which, if 'true',
        // indicates that the 'fileDescriptor' is writable, otherwise it is
        // not.  Before making this association, if, prior to this call,
        // 'willCloseOnReset' is true, close any file descriptor previously
        // associated with this object, otherwise leave it open but
        // disassociate this object from it.  The optionally specified
        // 'willCloseOnResetFlag' will set 'willCloseOnReset', which, if
        // 'true', indicates that 'fileDescriptor' is to be closed when this
        // object is cleared, reset, or destroyed, otherwise no action will be
        // taken on 'fileDescriptor' at that time.  Optionally specify a
        // 'binaryModeFlag', which is ignored on Unix; if 'false' on Windows,
        // it indicates that '\n's internally are to be translated to and from
        // '\r\n' sequences on the device; on Unix or if 'binaryModeFlag' is
        // 'true' no such translation is to occur.  Return 0 on success, and a
        // non-zero value otherwise.  Note that if
        // 'FilesystemUtil::k_INVALID_FD' is passed as 'fileDescriptor', no
        // file descriptor is to be associated with this object.  Also note
        // that the state of 'fileDescriptor' is unchanged by this call, there
        // is no implicit seek.

    void release();
        // Disassociate this file handler from any file descriptor with which
        // it may be associated without closing that file descriptor.  This
        // method succeeds with no effect if 'isOpened' was 'false'.  Note that
        // 'fileDescriptor' is 'FilesystemUtil::k_INVALID_FD' after this call.

    int clear();
        // Release any file descriptor that may be associated with this file
        // handler.  If 'isOpened' and 'willCloseOnReset' are both 'true', the
        // file descriptor will be closed, otherwise it will be left unchanged.
        // Return 0 on success and a non-zero value if the close fails.  This
        // method succeeds with no effect if 'isOpened' was 'false'.  Note that
        // 'fileDescriptor' is always 'FilesystemUtil::k_INVALID_FD' after this
        // call.

    int read(char *buffer, int numBytes);
        // Read the specified 'numBytes' bytes from the current position of the
        // file descriptor into the specified 'buffer'.  Return the number of
        // characters successfully read.  The behavior is undefined unless
        // '0 <= numBytes' and 'buffer' is at least 'numBytes' long.  Note that
        // on Windows in text mode, '\r\n' is read as a single character and
        // stored in the buffer as '\n'.

    int write(const char *buffer, int numBytes);
        // Write the specified 'buffer', containing the specified 'numBytes',
        // to the file descriptor starting at the current position.  Return 0
        // on success, and a non-zero value otherwise.  The behavior is
        // undefined unless '0 <= numBytes'.  Note that on Windows in text
        // mode, a '\n' is written as '\r\n' and counts as one character.

    bsl::streampos seek(bsl::streamoff offset, FilesystemUtil::Whence dir);
        // Set the file position associated with this object according to the
        // specified 'offset' and 'dir' behavior.
        //..
        //   * If 'dir' is 'FilesystemUtil::e_SEEK_FROM_BEGINNING', set the
        //     position to 'offset' bytes from the beginning of the file.
        //   * If 'dir' is 'FilesystemUtil::e_SEEK_FROM_CURRENT', advance the
        //     position by 'offset' bytes
        //   * If 'dir' is 'FilesystemUtil::e_SEEK_FROM_END', set the position
        //     to 'offset' bytes beyond the end of the file.
        //..
        // Return the new location of the file position, in bytes from the
        // beginning of the file on success; and 'FilesystemUtil::k_INVALID_FD'
        // otherwise.  The effect on the file position is undefined unless the
        // file descriptor represents a device capable of seeking.  Note that
        // 'seek' does not change the size of the file if the position advances
        // beyond the end of the file; instead, the next write at the pointer
        // will increase the file size.  Also note that on Windows in text
        // mode, 'offset' will be the number of bytes on disk passed over,
        // including '\r's in '\r\n' sequences.

    void *mmap(bsl::streamoff offset, bsl::streamoff length);
        // Map to memory a section of the file starting at the specified
        // 'offset' from the start of the file and return a pointer to that
        // memory.  The section mapped is to be of the specified 'length'.
        // The behavior is undefined unless 'offset' is a multiple of
        // 'pageSize'.  Note that the memory is mapped for readonly access.

    void unmap(void *mappedMemory, bsl::streamoff length);
        // Unmap the section of memory beginning at the specified
        // 'mappedMemory', having the specified 'length'.  The behavior is
        // undefined unless 'mappedMemory' is an address returned by a previous
        // call to the 'mmap' method and 'length' was the 'length' specified in
        // that call.

    void setWillCloseOnReset(bool booleanValue);
        // Set 'willCloseOnReset' (the flag determining whether this file
        // handler will close the file descriptor on the the next reset, clear,
        // or destruction) to the specified 'booleanValue'.  If
        // 'willCloseOnReset' is 'true', the next reset, clear, or destruction
        // will result in the file descriptor being closed, otherwise, it will
        // remain open.

    // ACCESSORS
    FilesystemUtil::FileDescriptor fileDescriptor() const;
        // Return the file descriptor associated with this object, if
        // 'isOpened' is 'true', and -1 otherwise.

    bsl::streamoff fileSize() const;
        // Return the size of the file associated with this file handler, or
        // 0 if it is associated with a device other than a regular file (e.g.,
        // a device or directory).

    bsl::streamoff getOffset(char *first, char *last) const;
        // Return the number of bytes that the data in the range specified by
        // '[first, last)' will fill when written to the file descriptor.  Note
        // that on Unix, or for a binary file on Windows, this value will be
        // 'last - first', but for on Windows in text mode, extra bytes are
        // added when '\n' would be written to the file descriptor as '\r\n'.

    bool isInBinaryMode() const;
        // Return 'false' if on Windows and the file is opened in text mode,
        // and 'true' otherwise.

    bool isOpened() const;
        // Return 'true' if this file handler is currently associated with a
        // file descriptor, and 'false' otherwise.

    bool isRegularFile() const;
        // Return 'true' if the file descriptor associated with this file
        // handler is associated with a regular file and 'false' otherwise.
        // Note that directories and pipes are not regular files.

    int openMode() const;
        // Return the 'bsl::ios_base' mode bits corresponding to this file
        // handler.  Note that this will be a union (bitwise-OR) of a subset of
        // the 'bsl::ios_base' constants 'in', 'out', and 'binary'.

    bool willCloseOnReset() const;
        // Return 'true' if the associated file descriptor will be closed the
        // next time this file handler is reset, cleared, or destroyed, and
        // 'false' otherwise.  Note that this value is determined by the value
        // of 'willCloseOnResetFlag' that was passed to the most recent call to
        // 'reset' or 'setWillCloseOnReset'.
};

                             // =================
                             // class FdStreamBuf
                             // =================

class FdStreamBuf : public bsl::streambuf {
    // This class, derived from the C++ standard library class
    // 'bsl::streambuf', is a mechanism that can be associated with an opened
    // file descriptor, and, except for changing the locale, enables the caller
    // to invoke all the standard 'bsl::streambuf' operations on that file
    // descriptor.  Note that objects of this class are always in exactly one
    // of the 5 modes outlined in the enum 'FdStreamBuf::FdStreamBufMode'.

  private:
    // PRIVATE TYPES
    enum { k_PBACK_BUF_SIZE = 8 }; // size of d_pBackBuf

    enum FdStreamBufMode  {
        e_NULL_MODE          = 0,  // empty state, when not in any other mode;
                                   // the object is constructed in this state

        e_INPUT_MODE         = 1,  // doing input

        e_INPUT_PUTBACK_MODE = 2,  // input putback mode is a form of input
                                   // mode where chars that have been stuffed
                                   // back into the input buffer are kept in
                                   // 'd_pBackBuf'.

        e_OUTPUT_MODE        = 3,  // doing output

        e_ERROR_MODE         = 4   // An error has occured.  Note that error
                                   // mode is sticky -- subsequent I/O won't
                                   // work until error mode is cleared by a
                                   // 'reset' or a seek.
    };

  private:
    // DATA
                        // data members used in all modes

    FdStreamBuf_FileHandler
                      d_fileHandler;      // file handler, holds the file
                                          // descriptor, used for doing low
                                          // level operations on the file
                                          // descriptor

                        // mode information

    FdStreamBufMode   d_mode;

    bool              d_dynamicBufferFlag;// 'true' if the buffer 'd_buf_p' is
                                          // heap allocated, 'false' if it was
                                          // supplied by the user.

                        // putback buffer

    char              d_pBackBuf[k_PBACK_BUF_SIZE];
                                          // for putback mode (see above)

                        // input/output buffer

    char             *d_buf_p;            // buffer
    char             *d_bufEOS_p;         // end of buffer space, allocated or
                                          // otherwise
    char             *d_bufEnd_p;         // end of data that's been read in
                                          // input mode, not used in output
                                          // mode.

                        // data members saved when entering putback mode --
                        // these elements are for for saving fields from the
                        // base class while we are in putback mode

    char             *d_savedEback_p;     // saved value of 'eback'
    char             *d_savedGptr_p;      // saved value of 'gptr'
    char             *d_savedEgptr_p;     // saved value of 'egptr'

                        // fields relevant to mapping the file while in input
                        // mode

    char             *d_mmapBase_p;       // pointer to the 'mmap'ed input
                                          // area, 0 if we are not in 'mmap'
                                          // input mode

    bsl::streamoff    d_mmapLen;          // length of mapped area

                        // memory allocator

    bslma::Allocator *d_allocator_p;      // allocator (held, not owned)

  private:
    // PRIVATE MANIPULATORS
    void exitPutbackMode();
        // Exit putback mode (leaving this object in input mode) and restore
        // the get buffer to the state it was in just prior to entering putback
        // mode.  The behavior is undefined unless this object is in putback
        // mode.

    int switchToInputMode();
        // Switch this object to input mode.  Return 0 on success, and a
        // non-zero value otherwise.  If this method is called while in input
        // putback mode, exit input putback mode.  Note that this function is
        // called when doing the first input, after a seek, or after writing.
        // Also note that this method has no effect if called when this object
        // is in input mode.

    int exitInputMode(bool correctSeek);
        // Change from input mode to null mode.  If the specified 'correctSeek'
        // is 'true', seek to position the file pointer at the point from
        // which the next input would have come; otherwise don't do the seek.
        // If the input file is currently mapped, unmap it.  Return 0 on
        // success, and non-zero otherwise.  The behavior is undefined unless
        // this object is in input or input_putback mode.  Note that
        // performing a corrective seek corrects the discrepancy between the
        // client's perception of the file pointer location and the actual
        // file pointer location, caused by buffering.

    int switchToOutputMode();
        // Switch this object to output mode.  Return 0 on success, and a
        // non-zero value otherwise.  Note that this method has no effect if
        // this object is already in output mode.  Also note that this method
        // is called when performing the first output, or when performing the
        // first output after a seek or read.

    int underflowRead();
        // Use 'read' to get some data from the file descriptor, and add it to
        // the input buffer.  Return the first character of input.  Note that
        // this method is called only by 'underflow', and only as a last
        // resort, when additional data can't be provided by mapping.

    int inputError();
        // Put this object into error mode, clearing the get area.  Always
        // return 'traits_type::eof()'.  Note that error mode is sticky and is
        // cleared only by a 'reset', 'clear' or seek.

    int outputError();
        // Put this object into error mode, clearing the put area.  Always
        // return 'traits_type::eof()'.  Note that error mode is sticky and is
        // cleared only by a 'reset', 'clear', 'seekoff', or 'seekpos'.

    int allocateBuffer(char *buffer, int numBytes);
        // Set the buffer to be used by this object.  If the specified 'buffer'
        // is 0, dynamically allocate a buffer of specified 'numBytes' length,
        // or if 'buffer' is a non-zero value, use the first 'numBytes' bytes
        // of 'buffer'.  Return 0 on success, and a non-zero value otherwise.
        // The behavior is undefined unless '1 <= numBytes', 'buffer' (if
        // non-zero) is at least 'numBytes' long, a buffer has not previously
        // been allocated or provided, and no I/O has occurred prior to this
        // call.

    int allocateBuffer();
        // Dynamically allocate an input/output buffer of a default size.
        // Return 0 on success, and a non-zero value otherwise.  The behavior
        // is undefined unless no buffer has previously been allocated or
        // provided, and no I/O has occurred prior to this call.

    void deallocateBuffer();
        // If the buffer is dynamically allocated by 'allocateBuffer', free
        // it.  The behavior is undefined unless the buffer was previously
        // allocated or provided.

    int seekInit();
        // Prepare this 'FdStreamBuf' for a subsequent seek operation by
        // setting various mode information into an appropriate state.  If in
        // output mode, flush the buffer.  If in putback input mode or error
        // mode, exit that mode.  If in regular input mode, leave that mode
        // unaffected.  Return 0 on success, and a non-zero value otherwise.

    pos_type seekReturn(pos_type offset);
        // Finish a seek by putting this object into null mode and nulling out
        // pointers to the buffer to reflect the fact that a seek has occurred.
        // Return the specified 'offset' on success, and a negative value
        // otherwise.

    int flush();
        // Flush any output data to the file descriptor, and reset the state of
        // this object.  Return 0 on success, and a non-zero value otherwise.

  protected:
    // PROTECTED MEMBER FUNCTIONS

    // The following member functions override protected virtual functions
    // inherited from the base class, and are specified to be protected as part
    // of the standard library 'bsl::streambuf' interface.

    // PROTECTED MANIPULATORS
    virtual int_type underflow();
        // Replenish the input buffer with data obtained from the file
        // descriptor, and return the next character of input (or eof if no
        // input is available).  Note that in windows text mode, '\r\n'
        // sequences on the device will be translated to '\n's.

    virtual int_type pbackfail(int_type c = traits_type::eof());
        // If the optionally specified 'c' is not given, move the current input
        // position back one character and return the character at that
        // position.  Otherwise specify a value for 'c' other than
        // 'traits_type::eof'.  If 'c' is equal to the previous character in
        // the read buffer, the behavior is the same as if 'eof()' was passed.
        // If 'c' is not eof and is not equal to the previous character in the
        // putback buffer push the character 'c' is back into the input buffer,
        // if possible.  Return the backed up character on success and
        // 'traits_type::eof()' otherwise.  If the input buffer is readonly, or
        // 'gptr()' is already at the beginning of the input buffer, this
        // object enters 'INPUT_PUTBACK_MODE' and 'c' is stuffed back into the
        // putback buffer.  Note that only 'PBACK_BUF_SIZE' characters can be
        // backed up into the putback buffer, if this limit is exceeded,
        // 'traits_type::eof()' will be returned.  Also note that this method
        // is called by public methods 'sputbackc' or 'sungetc' in the base
        // class, and only when simply decrementing the current position in the
        // input buffer won't satisfy the request, either because 'c' doesn't
        // match the previously input character, or because the input position
        // is already at the beginning of the input buffer.

    virtual int_type overflow(int_type c = traits_type::eof());
        // If in output mode, write the contents of the buffer to output.
        // Return 'traits_type::eof()' on failure, and any other value on
        // success.  Optionally specify a character 'c' to be appended to the
        // buffer prior to the flush.  If no character is specified, no
        // character is appended to the buffer.  If not in output mode, switch
        // to output mode.  Note that the write will translate '\n's to
        // '\r\n's.

    virtual FdStreamBuf *setbuf(char_type *buffer, bsl::streamsize numBytes);
        // Use the specified 'buffer' of the specified 'numBytes' capacity as
        // the input/output buffer for this 'streambuf'.  If 'buffer == 0', the
        // buffer is dynamically allocated with a default size.  If both
        // 'buffer' and 'numBytes' are zero a 1-byte buffer is dynamically
        // allocated.  The behavior is undefined if any I/O has preceded this
        // call, and unless the buffer is uninitialized before this call.

    virtual pos_type seekoff(
        off_type                offset,
        bsl::ios_base::seekdir  whence,
        bsl::ios_base::openmode mode = bsl::ios_base::in | bsl::ios_base::out);
        // Set the file pointer associated with the file descriptor according
        // to the specified 'offset' and 'whence':
        //..
        // * If 'whence' is 'bsl::ios_base::beg', set the pointer to 'offset'
        //   bytes from the beginning of the file.
        // * If 'whence' is 'bsl::ios_base::cur', advance the pointer by
        //   'offset' bytes
        // * If 'whence' is 'bsl::ios_base::end', set the pointer to 'offset'
        //   bytes beyond the end of the file.
        //..
        // Optionally specify 'mode', which is ignored.  Return the new
        // location of the file position, in bytes from the beginning of the
        // file, on success, and -1 otherwise.  The behavior is undefined
        // unless the file descriptor is on a device capable of seeking.  Note
        // that seeking does not change the size of the file if the pointer
        // advances beyond the end of the file; instead, the next write at the
        // pointer will increase the file size.  Also note that seeks are
        // always in terms of bytes on the device, meaning that in Windows
        // text mode, seeking past a '\n' perceived by the caller will count as
        // 2 bytes since it has to seek over a '\r\n' sequence on the device.

    virtual pos_type seekpos(
        pos_type                offset,
        bsl::ios_base::openmode mode = bsl::ios_base::in | bsl::ios_base::out);
        // Seek to the specified 'offset' relative to the beginning of the
        // file.  Return the resulting absolute position in the file relative
        // to the beginning.  Optionally specify 'mode' which is ignored.  Also
        // note that seeks are always in terms of bytes on the device, meaning
        // that on a Windows text file, seeking past a '\r\n' sequence on the
        // disk will count as two bytes, though if it is read in it will be a
        // single '\n' byte.

    virtual int sync();
        // If in output mode, flush the buffer to the associated file
        // descriptor; otherwise do nothing.  Return 0 on success, -1
        // otherwise.

    virtual void imbue(const bsl::locale& locale);
        // Set the locale for this object.  This method has no effect.  The
        // behavior is undefined unless the specified 'locale' is the same as
        // 'bsl::locale()'.

    virtual bsl::streamsize showmanyc();
        // If this object is in putback mode, return the number of characters
        // remaining to be read in the putback buffer, and otherwise the
        // number of characters remaining in the file to be read.  Return a
        // non-negative number of characters on success and a negative value
        // otherwise.  The behavior is undefined unless this object is in input
        // mode and the file descriptor is associated with a regular file.

    virtual bsl::streamsize xsgetn(char *buffer, bsl::streamsize numBytes);
        // Read up to the specified 'numBytes' characters from the file
        // descriptor into the specified 'buffer' and return the number of
        // characters successfully read.  The behavior is undefined unless
        // 'buffer' is at least 'numBytes' bytes long.  Note that on a Windows
        // text file, a '\r\n' in the file will be read as '\n' (counting as a
        // single character).

    virtual bsl::streamsize xsputn(const char      *buffer,
                                   bsl::streamsize  numBytes);
        // Write up to the specified 'numBytes' characters from the specified
        // 'buffer' and return the number of characters successfully written.
        // Note that this method does not necessarily modify the file: this
        // method may simply write the characters to a buffer to be flushed to
        // the file at a later time.  Also note that on a Windows text file, a
        // '\n' will be written to the file as '\r\n' (counted as a single
        // character).

  private:
    // NOT IMPLEMENTED
    FdStreamBuf(           const FdStreamBuf&);
    FdStreamBuf& operator=(const FdStreamBuf&);

  public:
    // CREATORS
    explicit FdStreamBuf(
                   FilesystemUtil::FileDescriptor  fileDescriptor,
                   bool                            writableFlag,
                   bool                            willCloseOnResetFlag = true,
                   bool                            binaryModeFlag = false,
                   bslma::Allocator               *basicAllocator = 0);
        // Create a 'FdStreamBuf' associated with the specified
        // 'fileDescriptor' that refers to an already opened file or device,
        // and specify 'writableFlag' which, if 'true', indicates that
        // 'fileDescriptor' is writable, otherwise it is not.  The optionally
        // specified 'willCloseOnResetFlag', if 'true', indicates that
        // 'fileDescriptor' is to be closed the next time this object is reset,
        // cleared or destroyed, or if 'false' the file descriptor is to be
        // left open.  Optionally specify a 'binaryModeFlag' which is ignored
        // on Unix; if 'false' on Windows, it indicates that '\n's are to be
        // translated to and from '\r\n' sequences on the device.  Optionally
        // specify a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.  Note that if 'FilesystemUtil::k_INVALID_FD' is passed to
        // 'fileDescriptor', no file descriptor is to be associated with this
        // object.  Also note that the state of the 'fileDescriptor' is
        // unchanged by this call (i.e., there is no implicit seek).

    ~FdStreamBuf();
        // Destroy this object and, if 'willCloseOnReset' is 'true', close the
        // file descriptor associated with this object, if any.

    // MANIPULATORS
    int reset(FilesystemUtil::FileDescriptor fileDescriptor,
              bool                           writableFlag,
              bool                           willCloseOnResetFlag = true,
              bool                           binaryModeFlag = false);
        // Associate this object with the specified 'fileDescriptor', and
        // record the state of the specified 'writableFlag' which, if 'true',
        // indicates that 'fileDescriptor' is writable, otherwise it is not.
        // Before making this association, if, prior to this call,
        // 'willCloseOnReset' is true, close any file descriptor previously
        // associated with this object, otherwise leave it open but
        // disassociate this object from it.  The Optionally specified
        // 'willCloseOnResetFlag' which will set 'willCloseOnReset', which, if
        // 'true', indicates that the specified file descriptor is to be closed
        // when this object is cleared, reset, or destroyed, otherwise no
        // action will be taken on 'fileDescriptor' at that time.  Optionally
        // specify a 'binaryModeFlag', which is ignored on Unix; if 'false' on
        // Windows, it indicates that '\n's internally are to be translated to
        // and from '\r\n' sequences on the device; on Unix or if
        // 'binaryModeFlag' is 'true' no such translation is to occur.  Return
        // 0 on success, and a non-zero value otherwise.  Note that if
        // 'FilesystemUtil::k_INVALID_FD' is passed as 'fileDescriptor', no
        // file descriptor is to be associated with this object.  Also note
        // that the state of the 'fileDescriptor' is unchanged by this call,
        // there is no implicit seek.

    void release();
        // Disassociate this file handler from any file descriptor with which
        // it may be associated without closing that file descriptor.  This
        // method succeeds with no effect is 'isOpened' was false.  Note that
        // 'fileDescriptor' is 'FilesystemUtil::k_INVALID_FD' after this call.

    int clear();
        // Release any file descriptor that may be associated with this file
        // handler.  If 'isOpened' and 'willCloseOnReset' are both 'true', the
        // file descriptor will be closed, otherwise it will not.  Return 0 on
        // success, and a non-zero value if the close fails.  This method
        // succeeds with no effect if 'isOpened' was false.  Note that
        // 'fileDescriptor' is 'FilesystemUtil::k_INVALID_FD' after this call.

    // ACCESSORS
    FilesystemUtil::FileDescriptor fileDescriptor() const;
        // Return the file descriptor associated with this object, or
        // 'FilesystemUtil::k_INVALID_FD' if this object is not currently
        // associated with a file descriptor.

    bool isOpened() const;
        // Return 'true' if this object is currently associated with a file
        // descriptor, and 'false' otherwise.

    bool willCloseOnReset() const;
        // Return 'true' if this object will close the associated file
        // descriptor the next time it is reset, cleared, or destroyed, and
        // 'false' otherwise.
};

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

                       // -----------------------------
                       // class FdStreamBuf_FileHandler
                       // -----------------------------

// CLASS METHODS
inline
size_t FdStreamBuf_FileHandler::pageSize()
{
    return bsls::AtomicOperations::getIntRelaxed(&s_pageSize);
}

// MANIPULATORS
inline
void FdStreamBuf_FileHandler::release()
{
    d_willCloseOnResetFlag = false;
    reset(FilesystemUtil::k_INVALID_FD, false);
}

inline
int FdStreamBuf_FileHandler::clear()
{
    return reset(FilesystemUtil::k_INVALID_FD, false);
}

inline
void FdStreamBuf_FileHandler::setWillCloseOnReset(bool booleanValue)
{
    d_willCloseOnResetFlag = booleanValue;
}

inline
bsl::streamoff
FdStreamBuf_FileHandler::getOffset(char *first, char *last) const
{
    BSLS_ASSERT_SAFE(first <= last);

    return d_openModeFlags & bsl::ios_base::binary
           ? last - first
           : bsl::count(first, last, '\n') + last - first;
}

inline
bool FdStreamBuf_FileHandler::isInBinaryMode() const
{
#if defined(BSLS_PLATFORM_OS_UNIX)
    return true;
# else
    // Windows

    return (d_openModeFlags & bsl::ios_base::binary) != 0;
# endif
}

inline
bool FdStreamBuf_FileHandler::isOpened() const
{
    return d_openedFlag;
}

inline
bool FdStreamBuf_FileHandler::isRegularFile() const
{
    return d_regularFileFlag;
}

inline
int FdStreamBuf_FileHandler::openMode() const
{
    return (int) d_openModeFlags;
}

inline
bool FdStreamBuf_FileHandler::willCloseOnReset() const
{
    return d_willCloseOnResetFlag;
}

inline
FilesystemUtil::FileDescriptor
FdStreamBuf_FileHandler::fileDescriptor() const
{
    return d_fileId;
}

                             // -----------------
                             // class FdStreamBuf
                             // -----------------

// PRIVATE MANIPULATORS
inline
void FdStreamBuf::exitPutbackMode()
{
    setg(d_savedEback_p, d_savedGptr_p, d_savedEgptr_p);
    d_mode = e_INPUT_MODE;
}

inline
FdStreamBuf::pos_type
FdStreamBuf::seekReturn(pos_type offset)
    // Only called by 'seekoff' and 'seekpos', returns the value about to be
    // returned by the calling routine.
{
    if (e_INPUT_MODE == d_mode || e_INPUT_PUTBACK_MODE == d_mode) {
        if (0 != exitInputMode(false)) {
            // error

            return (pos_type) - 1;                                    // RETURN
        }
    }
    setg(0, 0, 0);
    setp(0, 0);

    d_mode = e_NULL_MODE;

    return offset;
}

// MANIPULATORS
inline
int FdStreamBuf::reset(FilesystemUtil::FileDescriptor fileDescriptor,
                       bool                           writableFlag,
                       bool                           willCloseOnResetFlag,
                       bool                           binaryModeFlag)
{
    bool ok = 0 == flush();

    if (ok || FilesystemUtil::k_INVALID_FD == fileDescriptor) {
        // note we reset() whether flush succeeded or not

        ok &= (0 == d_fileHandler.reset(fileDescriptor,
                                        writableFlag,
                                        willCloseOnResetFlag,
                                        binaryModeFlag));
    }

    return ok ? 0 : -1;
}

inline
void FdStreamBuf::release()
{
    d_fileHandler.setWillCloseOnReset(false);
    reset(FilesystemUtil::k_INVALID_FD, false);
}

inline
int FdStreamBuf::clear()
{
    return reset(FilesystemUtil::k_INVALID_FD, false);
}

// ACCESSORS
inline
FilesystemUtil::FileDescriptor FdStreamBuf::fileDescriptor() const
{
    return d_fileHandler.fileDescriptor();
}

inline
bool FdStreamBuf::isOpened() const
{
    return d_fileHandler.isOpened();
}

inline
bool FdStreamBuf::willCloseOnReset() const
{
    return d_fileHandler.willCloseOnReset();
}
}  // close package namespace

}  // close enterprise namespace

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
