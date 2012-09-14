// bdesu_fdstreambuf.cpp                                              -*-C++-*-
#include <bdesu_fdstreambuf.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bdesu_fdstreambuf_cpp,"$Id$ $CSID$")

///IMPLEMENTATION NOTES
///--------------------
// This code was ported from STLport.  The class 'bdesu_FdStreamBuf' was
// created from 'bsl::filebuf', and the class 'bdesu_FdStreamBuf_FileHandler'
// was created from 'bsl::filebuf_base'.
//
// bdesu_FdStreamBuf_FileHandler: this is a helper class that is a thin layer
// on top of component 'bdesu_FileUtil', the main additional functionality it
// provides is the translation between '\n's and '\r\n' sequences in Windows
// text mode.
//
// bdesu_FdStreamBuf: inherits from 'bsl::streambuf', is meant to be a form
// of streambuf that can be initialized or attached to a file descriptor, that
// will then perform standard streambuf actions on that file descriptor.

#include <bdesu_memoryutil.h>

#include <bslma_allocator.h>
#include <bslma_default.h>

#include <bsls_assert.h>
#include <bsls_types.h>

#include <bsl_algorithm.h>
#include <bsl_locale.h>
#include <bsl_streambuf.h>

#include <bsl_ctime.h>
#include <bsl_cstring.h>              // for memcpy, memchr

# include <sys/stat.h>

#if defined(BSLS_PLATFORM__OS_UNIX)
extern "C" {
# include <unistd.h>
}  // extern "C"
#elif defined(BSLS_PLATFORM__OS_WINDOWS)
# define WIN32_LEAN_AND_MEAN
# include <windows.h>
extern "C" {
# include <fcntl.h>
# include <io.h>
}  // extern "C"
#endif

namespace BloombergLP {

                                // -----------
                                // local types
                                // -----------

typedef BloombergLP::bdesu_FileUtil::FileDescriptor FdType;

                              // ---------------
                              // local functions
                              // ---------------

static
bool getRegularFileInfo(FdType fd)
    // Return 'true' if the specified file descriptor 'fd' refers to a regular
    // file and 'false' otherwise.  Note that a regular file is a file and not
    // a directory, pipe, printer, keyboard or other device.
{
#if defined(BSLS_PLATFORM__OS_UNIX)
    struct stat buf;
    return 0 == fstat(fd, &buf) && S_ISREG(buf.st_mode);
#else
    // The only reason we want to know if it's a regular file is to determine
    // whether it can be 'mmap'ed and 'seek'ed.  Since we may have just created
    // a file of zero length, we may not be able to map a byte of it to see if
    // it's mappable, but we can at least try a seek, which will fail on a pipe
    // and many other non-file device types.

    bdesu_FileUtil::Offset offset =
          bdesu_FileUtil::seek(fd, 0, bdesu_FileUtil::BDESU_SEEK_FROM_CURRENT);
    return 0 <= offset;
#endif
}

                    // ===================================
                    // class bdesu_FdStreamBuf_FileHandler
                    // ===================================

bsls::AtomicOperations::AtomicTypes::Int
                               bdesu_FdStreamBuf_FileHandler::s_pageSize = {0};

// CREATORS
bdesu_FdStreamBuf_FileHandler::bdesu_FdStreamBuf_FileHandler()
: d_fileId(bdesu_FileUtil::INVALID_FD)
, d_openedFlag(false)
, d_regularFileFlag(false)
, d_openModeFlags(static_cast<bsl::ios_base::openmode>(0))
, d_willCloseOnResetFlag(false)
, d_peekBufferFlag(false)
{
    if (bsls::AtomicOperations::getIntRelaxed(&s_pageSize) <= 0) {
        bsls::AtomicOperations::setIntRelaxed(&s_pageSize,
                                              bdesu_MemoryUtil::pageSize());
    }
}

bdesu_FdStreamBuf_FileHandler::~bdesu_FdStreamBuf_FileHandler()
{
    clear();
}

// MANIPULATORS
int bdesu_FdStreamBuf_FileHandler::reset(
                           bdesu_FileUtil::FileDescriptor fileDescriptor,
                           bool                           writableFlag,
                           bool                           willCloseOnResetFlag,
                           bool                           binaryFile)
{
    const bsl::ios_base::openmode iosBaseZero = (bsl::ios_base::openmode) 0;

    if (isOpened() && willCloseOnReset()) {
        if (0 != FileUtil::close(d_fileId)) {
            return -1;                                                // RETURN
        }
    }

    d_fileId               = bdesu_FileUtil::INVALID_FD;
    d_openedFlag           = false;
    d_regularFileFlag      = false;
    d_openModeFlags        = iosBaseZero;
    d_willCloseOnResetFlag = false;
    d_peekBufferFlag       = false;

    if (bdesu_FileUtil::INVALID_FD == fileDescriptor) {
        return 0;                                                     // RETURN
    }

    d_fileId           = fileDescriptor;
    d_openedFlag       = true;
    d_regularFileFlag  = getRegularFileInfo(d_fileId);

    d_openModeFlags    = bsl::ios_base::in;
    d_openModeFlags   |= writableFlag ? bsl::ios_base::out : iosBaseZero;
#if defined(BSLS_PLATFORM__OS_UNIX)
    (void) binaryFile;    // suppress unused warning

    d_openModeFlags   |= bsl::ios_base::binary;
# else
    // Windows

    d_openModeFlags   |= binaryFile ? bsl::ios_base::binary : iosBaseZero;
# endif

    d_willCloseOnResetFlag = willCloseOnResetFlag;

    return 0;
}

int bdesu_FdStreamBuf_FileHandler::read(char *buffer, int numBytes)
{
    BSLS_ASSERT_OPT(0 <= numBytes);

#ifdef BSLS_PLATFORM__OS_WINDOWS
    enum { CTRLZ = 26 };    // ^Z means EOF in Windows.  A ^Z character is
                            // allowed to occur at most once in a Windows text
                            // file, and then it must be the last character in
                            // the file.  Do not return a ^Z as data read, but
                            // act like we're already at end of file.

    char *remainingBuf = buffer;
    int bytesRead = 0;
    if (d_peekBufferFlag) {
        if (CTRLZ == d_peekBuffer || 0 == numBytes) {
            // if we have hit a ^Z, behave as if we're at the end of the file
            // if 0 bytes were requested, 0 bytes are returned

            return 0;                                                 // RETURN
        }
        BSLS_ASSERT(numBytes >= 1);

        buffer[0] = d_peekBuffer;
        ++bytesRead;
        d_peekBufferFlag = false;
        ++remainingBuf;
        --numBytes;
    }

    BSLS_ASSERT(! d_peekBufferFlag);

    bytesRead += FileUtil::read(d_fileId, remainingBuf, numBytes);

    if (bytesRead && !(d_openModeFlags & bsl::ios_base::binary)) {
        // Iterate over buffer, translating '\r\n's to '\n's (in-place).

        char       *from = buffer;
        char       *to   = buffer;
        const char *last = buffer + bytesRead - 1;
        for (; from <= last && *from != CTRLZ; ++from) {
            if (*from != '\r') {
                *to++ = *from;
            }
            else {
                if (from < last) { // not at buffer end
                    if (*(from + 1) != '\n') {
                        *to++ = '\r';
                    }
                }
                else {
                    unsigned numberOfBytesPeeked;
                    numberOfBytesPeeked = FileUtil::read(
                                          d_fileId, (void *) &d_peekBuffer, 1);
                    if (numberOfBytesPeeked) {
                        if (d_peekBuffer != '\n') {
                            // not a '\r\n'

                            *to++ = '\r';
                            d_peekBufferFlag = true;
                        }
                        else {   // A '\r\n', convert to '\n'
                            *to++ = '\n';
                        }
                    }
                    else { // An eof after '\r'
                        *to++ = '\r';
                    }
                }
            }
        }

        // pretend we're at end of file if hit ^Z

        if (CTRLZ == *from) {
            d_peekBuffer     = CTRLZ;
            d_peekBufferFlag = true;
        }
        bytesRead = to - buffer;
    }

    return bytesRead;
#else
    return FileUtil::read(d_fileId, buffer, numBytes);
#endif
}

#ifdef BSLS_PLATFORM__OS_WINDOWS
int bdesu_FdStreamBuf_FileHandler::windowsWriteText(const char *buffer,
                                                    int         numChars)
{
    BSLS_ASSERT(0 <= numChars);

    // This method in only called for a Windows text file, '\n's are
    // translated to '\r\n's.

    enum { OUTBUF_SIZE = 0x1000 };

    char outBuf[OUTBUF_SIZE + 1]; // A temporary buffer for holding the block
                                  // of translated text.  There is one extra
                                  // byte in case the block ends with '\n'
    const char *endOutBuf = outBuf + OUTBUF_SIZE;
    char *ptrOutBuf       = outBuf;

    const char *ptrInBuf = buffer;
    const char *endInBuf = buffer + numChars;

    int nextCopySize = bsl::min(numChars, (int) OUTBUF_SIZE);
    char *nextInLF;

    // while the remaining bytes to copy contain an LF ('\n')

    while (nextCopySize > 0 && 0 !=
             (nextInLF = (char *) bsl::memchr(ptrInBuf, '\n', nextCopySize))) {
        int lineLength = nextInLF - ptrInBuf;
        bsl::memcpy(ptrOutBuf, ptrInBuf, lineLength);
        ptrInBuf     += lineLength + 1;
        ptrOutBuf    += lineLength;
        *ptrOutBuf++ = '\r';
        *ptrOutBuf++ = '\n';

        nextCopySize = bsl::min(endInBuf - ptrInBuf,
                                bsl::max((bsl::ptrdiff_t)0,
                                         endOutBuf - ptrOutBuf));
    }

    // There are no LF's within next 'nextCopySize' bytes.

    if (nextCopySize > 0) {
        bsl::memcpy(ptrOutBuf, ptrInBuf, nextCopySize);
        ptrOutBuf += nextCopySize;
        ptrInBuf  += nextCopySize;
    }

    BSLS_ASSERT(ptrOutBuf <= outBuf + OUTBUF_SIZE + 1);

    // now write out the translated portion of the buffer

    char *writeOutBuf   = outBuf;
    int numBytesToWrite = ptrOutBuf - outBuf;
    while (numBytesToWrite) {
        int bytesWritten = FileUtil::write(d_fileId,
                                           writeOutBuf, numBytesToWrite);
        if (0 == bytesWritten) {
            // error - write shortfall

            return -1;                                                // RETURN
        }
        writeOutBuf     += bytesWritten;
        numBytesToWrite -= bytesWritten;
    }

    // Return the number of pre-translated characters written.

    return ptrInBuf - buffer;
}
#endif

int bdesu_FdStreamBuf_FileHandler::write(const char *buffer, int numBytes)
{
    BSLS_ASSERT_OPT(0 <= numBytes);

#ifdef BSLS_PLATFORM__OS_WINDOWS
    if (d_peekBufferFlag) {
        const int status = this->seek(0, FileUtil::BDESU_SEEK_FROM_CURRENT);
        if (status < 0) {
            // error: non-seekable device

            return -1;                                                // RETURN
        }
        d_peekBufferFlag = false;
    }
#endif

    while (true) {
        int written;

#ifdef BSLS_PLATFORM__OS_WINDOWS
        if (d_openModeFlags & bsl::ios_base::binary) {
            written = FileUtil::write(d_fileId, buffer, numBytes);
        }
        else {
            written = windowsWriteText(buffer, numBytes);
        }
#else
        // Unix
        written = FileUtil::write(d_fileId, buffer, numBytes);
#endif

        if (numBytes == written) {
            return 0;                                                 // RETURN
        }
        else if (written > 0 && written < numBytes) {
            numBytes -= written;
            buffer += written;
        }
        else {
            // error: write returned 0 or negative, meaning it failed

            return -1;                                                // RETURN
        }
    }
}

bsl::streampos bdesu_FdStreamBuf_FileHandler::seek(
                                                 bsl::streamoff         offset,
                                                 bdesu_FileUtil::Whence dir)
{
#ifdef BSLS_PLATFORM__OS_WINDOWS
    if (d_peekBufferFlag) {
        if (FileUtil::BDESU_SEEK_FROM_CURRENT == dir) {
            --offset;
        }
        d_peekBufferFlag = false;
    }
#endif

    switch (dir) {
      case FileUtil::BDESU_SEEK_FROM_BEGINNING: {
        if (offset < 0) {
            return (bsl::streamoff) -1;                               // RETURN
        }
      } break;
      case FileUtil::BDESU_SEEK_FROM_CURRENT: {
      } break;
      case FileUtil::BDESU_SEEK_FROM_END: {
        if (-offset > fileSize() ) {
            return (bsl::streamoff) -1;                               // RETURN
        }
      } break;
      default: {
        return (bsl::streamoff) -1;                                   // RETURN
      }
    }

    return FileUtil::seek(d_fileId, offset, dir);
}

void *bdesu_FdStreamBuf_FileHandler::mmap(bsl::streamoff offset,
                                          bsl::streamoff len)
{
    BSLS_ASSERT(0 <= len);

    void *ret;

    d_peekBufferFlag = false;

    bdesu_FileUtil::Offset cur = seek(0, FileUtil::BDESU_SEEK_FROM_CURRENT);
    if (0 > cur) {
        // not seekable, won't be mappable

        return 0;                                                     // RETURN
    }

    if (0 != FileUtil::map(d_fileId, &ret, offset,
                           len, bdesu_MemoryUtil::BDESU_ACCESS_READ)) {
        // error -- device is not mappable

        this->seek(cur, FileUtil::BDESU_SEEK_FROM_BEGINNING);
        return 0;                                                     // RETURN
    }

    // Move the cursor at the end of the mapped area; 'FileUtil::map()' was
    // leaving it at 0.

    if (0 > this->seek(offset + len, FileUtil::BDESU_SEEK_FROM_BEGINNING)) {
        // error -- device not seekable
        // this really should not happen -- the first seek succeeded

        FileUtil::unmap(ret, len);
        return 0;                                                     // RETURN
    }

    return ret;
}

void bdesu_FdStreamBuf_FileHandler::unmap(void *base, bsl::streamoff len)
{
    // 'base' must have been previously mmapped with length 'len'.

    BSLS_ASSERT(base);
    BSLS_ASSERT(0 <= len);

    FileUtil::unmap(base, len);
}

// ACCESSORS
bsl::streamoff
bdesu_FdStreamBuf_FileHandler::fileSize() const
{
    bsl::streamoff ret = 0;

#ifdef BSLS_PLATFORM__OS_UNIX
    struct stat buf;
    if (fstat(d_fileId, &buf) == 0 && S_ISREG(buf.st_mode)) {
        ret = buf.st_size > 0 ? buf.st_size : 0;
    }
#else
    // Windows

    LARGE_INTEGER li;
    li.LowPart = GetFileSize(d_fileId, (unsigned long *) &li.HighPart);
    if (li.LowPart == 0xFFFFFFFF && GetLastError() != NO_ERROR) {
        ret = 0;
    } else {
        ret = li.QuadPart;
    }
#endif

    return ret;
}

                        // -----------------------
                        // class bdesu_FdStreamBuf
                        // -----------------------

// CREATORS
bdesu_FdStreamBuf::bdesu_FdStreamBuf(
                          bdesu_FileUtil::FileDescriptor  fileDescriptor,
                          bool                            writableFlag,
                          bool                            willCloseOnResetFlag,
                          bool                            binaryModeFlag,
                          bslma_Allocator                *basicAllocator)
: bsl::streambuf()
, d_fileHandler()
, d_mode(BDESU_NULL_MODE)
, d_dynamicBufferFlag(false)
, d_buf_p(0)
, d_bufEOS_p(0)
, d_bufEnd_p(0)
, d_savedEback_p(0)
, d_savedGptr_p(0)
, d_savedEgptr_p(0)
, d_mmapBase_p(0)
, d_mmapLen(0)
, d_allocator_p(bslma_Default::allocator(basicAllocator))
{
    reset(fileDescriptor, writableFlag, willCloseOnResetFlag, binaryModeFlag);
}

bdesu_FdStreamBuf::~bdesu_FdStreamBuf()
{
    clear();
    deallocateBuffer();
}

// PRIVATE MANIPULATORS
int bdesu_FdStreamBuf::switchToInputMode()
{
    switch (d_mode) {
      case BDESU_INPUT_MODE: {
        return 0;                                                     // RETURN
      } break;
      case BDESU_INPUT_PUTBACK_MODE: {
        exitPutbackMode();
        return 0;                                                     // RETURN
      } break;
      case BDESU_ERROR_MODE: {
        // error mode is sticky

        return -1;                                                    // RETURN
      } break;
      case BDESU_OUTPUT_MODE: {
        // flush the output buffer

        if (traits_type::eq_int_type(overflow(traits_type::eof()),
                                     traits_type::eof())) {
            return -1;                                                // RETURN
        }
      } break;
      case BDESU_NULL_MODE: {
        // do nothing
      } break;
    }

    // mode is now 'BDESU_NULL_MODE' or 'BDESU_OUTPUT_MODE'

    if (!isOpened()
       || 0 == ((int) d_fileHandler.openMode() & (int) bsl::ios_base::in)) {
        return -1;                                                    // RETURN
    }

    if (!d_buf_p && 0 != allocateBuffer()) {
        return -1;                                                    // RETURN
    }
    d_bufEnd_p = d_buf_p;

    setg(0, 0, 0);
    setp(0, 0);

    d_mode = BDESU_INPUT_MODE;

    return 0;
}

int bdesu_FdStreamBuf::exitInputMode(bool correctSeek)
{
    if (BDESU_INPUT_PUTBACK_MODE == d_mode) {
        exitPutbackMode();
    }

    BSLS_ASSERT(BDESU_INPUT_MODE == d_mode);

    bsl::streamoff adjust = 0;

    // The file pointer located one byte past the final byte read into the
    // input buffer.  Adjust it back to where the client perceives the next
    // read should occur from.

    if (correctSeek && gptr() != egptr()) {
        // Use 'getOffset' which, if it's a Windows text file, will account for
        // the fact that where we see a '\n' in the buffer there will be a
        // corresponding '\r\n' on the device.

        adjust = d_fileHandler.getOffset(gptr(), egptr());
    }

    if (d_mmapBase_p != 0) {
        d_fileHandler.unmap(d_mmapBase_p, d_mmapLen);
    }
    d_mmapBase_p = 0;

    if (adjust) {
        if (0 > d_fileHandler.seek(-adjust,
                                   FileUtil::BDESU_SEEK_FROM_CURRENT)) {
            // non-seekable device

            return -1;                                                // RETURN
        }
    }

    setg(0, 0, 0);
    setp(0, 0);

    d_mode = BDESU_NULL_MODE;

    return 0;
}

int bdesu_FdStreamBuf::switchToOutputMode()
{
    switch (d_mode) {
      case BDESU_OUTPUT_MODE: {
        return 0;                                                     // RETURN
      } break;
      case BDESU_INPUT_MODE:
      case BDESU_INPUT_PUTBACK_MODE: {
        if (0 != exitInputMode(true)) {
            return -1;                                                // RETURN
        }
      } break;
      case BDESU_ERROR_MODE: {
        // error mode is sticky

        return -1;                                                    // RETURN
      } break;
      case BDESU_NULL_MODE: {
        // do nothing
      } break;
    }

    BSLS_ASSERT(BDESU_NULL_MODE == d_mode);

    if (!isOpened()
     || !((int) d_fileHandler.openMode() & (int) bsl::ios_base::out)) {
        return -1;                                                    // RETURN
    }

    if (!d_buf_p && 0 != allocateBuffer()) {
        return -1;                                                    // RETURN
    }

    setg(0, 0, 0);
    setp(d_buf_p, d_bufEOS_p - 1);

    d_mode = BDESU_OUTPUT_MODE;

    return 0;
}

int bdesu_FdStreamBuf::underflowRead()
{
    // This routine is called after it has been determined that 'mmap' is not
    // an appropriate way to read the file.  This routine's purpose is to
    // replenish the buffer 'd_buf_p' from the file descriptor.

    // Note that 'FileHandler::read' handles the translation of '\r\n' to '\n',
    // if appropriate.

    bsl::ptrdiff_t bytesRead = d_fileHandler.read(d_buf_p,
                                                  d_bufEOS_p - d_buf_p);

    // Don't enter error mode for a failed read.  Error mode is sticky,
    // and we might succeed if we try again.

    if (bytesRead <= 0) {
        return traits_type::eof();                                    // RETURN
    }

    // Convert the buffer to internal characters.

    d_bufEnd_p = d_buf_p + bytesRead;
    setg((char_type *) (void *) d_buf_p,
         (char_type *) (void *) d_buf_p,
         (char_type *) (void *) d_bufEnd_p);
    return traits_type::to_int_type(* d_buf_p);
}

int bdesu_FdStreamBuf::inputError()
{
    d_mode = BDESU_ERROR_MODE;
    setg(0, 0, 0);

    return traits_type::eof();
}

int bdesu_FdStreamBuf::outputError()
{
    d_mode = BDESU_ERROR_MODE;
    setp(0, 0);

    return traits_type::eof();
}

int bdesu_FdStreamBuf::allocateBuffer(char *buffer, int numBytes)
{
    BSLS_ASSERT(0 <= numBytes);

    // 'overflow' needs the buffer to be at least 1 byte long, and we also need
    // to be able to read at least a byte at a time.

    if (0 == numBytes) {
        buffer = 0;
    }
    numBytes = bsl::max(1, numBytes);

    if (0 == buffer) {
        d_buf_p = static_cast<char *>(d_allocator_p->allocate(numBytes));
        if (!d_buf_p) {
            return -1;                                                // RETURN
        }
        d_dynamicBufferFlag = true;
    }
    else {
        d_buf_p             = buffer;
        d_dynamicBufferFlag = false;
    }

    d_bufEOS_p = d_buf_p + numBytes;

    return 0;
}

int bdesu_FdStreamBuf::allocateBuffer()
{
    // Choose a buffer that's at least 4096 characters long and that's a
    // multiple of the page size.

    const int pageSize       = d_fileHandler.pageSize();
    const int defaultBufsize = ((pageSize + 4095UL) / pageSize) * pageSize;

    return allocateBuffer(0, defaultBufsize);
}

void bdesu_FdStreamBuf::deallocateBuffer()
{
    if (d_dynamicBufferFlag && d_buf_p) {
        d_allocator_p->deallocate(d_buf_p);
    }

    d_buf_p    = 0;
    d_bufEOS_p = 0;
}

int bdesu_FdStreamBuf::seekInit()
{
    // Flush the output buffer if we're in output mode.

    if (BDESU_OUTPUT_MODE == d_mode) {
        bool ok = !traits_type::eq_int_type(overflow(traits_type::eof()),
                                            traits_type::eof());

        if (!ok) {
            d_mode = BDESU_ERROR_MODE;
            setp(0, 0);
            return -1;                                                // RETURN
        }
    }

    // Discard putback characters, if any.

    if (BDESU_INPUT_PUTBACK_MODE == d_mode) {
        exitPutbackMode();
    }

    if (BDESU_INPUT_MODE != d_mode) {
        d_mode = BDESU_NULL_MODE;
    }

    return 0;
}

int bdesu_FdStreamBuf::flush()
{
    if (!isOpened()) {
        return 0;                                                     // RETURN
    }

    bool ok = true;

    if (BDESU_OUTPUT_MODE == d_mode) {
        ok &= !traits_type::eq_int_type(overflow(traits_type::eof()),
                                        traits_type::eof());
    }
    else if (BDESU_INPUT_MODE == d_mode) {
        if (0 != exitInputMode(true)) {
            return -1;                                                // RETURN
        }
    }

    d_mmapBase_p = 0;
    d_mmapLen    = 0;

    setg(0, 0, 0);
    setp(0, 0);

    d_savedEback_p = d_savedGptr_p = d_savedEgptr_p = 0;

    d_mode = BDESU_NULL_MODE;

    return ok ? 0 : -1;
}

// PROTECTED MANIPULATORS
bsl::streambuf::int_type
bdesu_FdStreamBuf::underflow()
{
    //  Note this method is called when the input buffer is exhausted.

    const unsigned int MMAP_CHUNK = 0x100000;    // Map 1MB at a time.

    if (BDESU_INPUT_PUTBACK_MODE == d_mode) {
        exitPutbackMode();
        if (gptr() < egptr()) {
            int c = traits_type::to_int_type(*gptr());
            return c;                                                 // RETURN
        }
    }
    else if (BDESU_INPUT_MODE != d_mode && 0 != switchToInputMode()) {
        return traits_type::eof();                                    // RETURN
    }

    // If it's a disk file, and if the internal and external character
    // sequences are guaranteed to be identical, then try to use memory
    // mapped I/O.  Otherwise, revert to ordinary read.

    if (d_fileHandler.isRegularFile() && d_fileHandler.isInBinaryMode()) {
        // If we have mapped part of the file already, then unmap it.

        if (d_mmapBase_p) {
            d_fileHandler.unmap(d_mmapBase_p, d_mmapLen);
        }
        d_mmapBase_p = 0;
        d_mmapLen    = 0;

        // Determine the position where we start mapping.  It has to be
        // a multiple of the page size.

        bsl::streamoff cur =
                d_fileHandler.seek(0, bdesu_FileUtil::BDESU_SEEK_FROM_CURRENT);

        bsl::streamoff sz = d_fileHandler.fileSize();
        if (sz > 0 && cur >= 0 && cur < sz) {
            // We were able to seek and take the file size, so it's probably
            // mappable.

            bsl::streamoff offset = (cur / d_fileHandler.pageSize())
                                                    * d_fileHandler.pageSize();
            bsl::streamoff remainder = cur - offset;

            d_mmapLen = sz - offset;

            if (d_mmapLen > (bsl::streamoff) MMAP_CHUNK) {
                d_mmapLen = MMAP_CHUNK;
            }

            d_mmapBase_p = (char *) d_fileHandler.mmap(offset, d_mmapLen);
            if (d_mmapBase_p) {
                setg(d_mmapBase_p,
                     d_mmapBase_p + remainder,
                     d_mmapBase_p + d_mmapLen);
                return traits_type::to_int_type(*gptr());             // RETURN
            }

            // mmap or subsequent seek failed, we may have lost our
            // position in the file -- recover it before falling through to
            // underflowRead

            if (cur != d_fileHandler.seek(
                             cur, bdesu_FileUtil::BDESU_SEEK_FROM_BEGINNING)) {
                // we're beyond recovering

                return inputError();                                  // RETURN
            }
        }
    }

    d_mmapBase_p = 0;
    d_mmapLen    = 0;

    // could not mmap it, use 'read()'

    return underflowRead();
}

bsl::streambuf::int_type
bdesu_FdStreamBuf::pbackfail(int_type c)
{
    // Push a character into the end of the input buffer.  When in
    // 'INPUT_PUTBACK_MODE', the end of the buffer is always
    // 'd_pBackBuf + d_pbackBufSize'; the beginning of the buffer moves
    // backward toward 'd_pBackBuf' as more characters are stuffed.

    const int_type eof = traits_type::eof();

    // If we are not already in input mode, pushback is impossible.

    if (BDESU_INPUT_MODE != d_mode && BDESU_INPUT_PUTBACK_MODE != d_mode) {
        return eof;                                                   // RETURN
    }

    // We can shortcut this if there was already an equal character in the
    // ordinary buffer in the last position read, we just have to back up.
    // Note that we must assume the ordinary buffer is readonly.

    if (gptr() != eback()
       && (traits_type::eq_int_type(c, eof)
                   || traits_type::eq(traits_type::to_char_type(c), gptr()[-1])
                                                           || !d_mmapBase_p)) {
        gbump(-1);
        if (!d_mmapBase_p && !traits_type::eq_int_type(c, eof)) {
            // input buffer is writable

            *gptr() = traits_type::to_char_type(c);
        }
        return traits_type::to_int_type(*gptr());                     // RETURN
    }
    else if (!traits_type::eq_int_type(c, eof)) {
        // No room in the ordinary buffer, have to use putback mode

        // Are we in the putback buffer already?

        char *pBackEnd = d_pBackBuf + BDESU_PBACK_BUF_SIZE;
        if (BDESU_INPUT_PUTBACK_MODE == d_mode) {
            // We're already in putback mode.  Do we have more room in the
            // putback buffer?

            if (eback() != d_pBackBuf) {
                setg(gptr() - 1, gptr() - 1, pBackEnd);
            }
            else {
                // No more room in the buffer, so fail.

                return eof;                                           // RETURN
            }
        }
        else {
            // We're not yet in the putback buffer.  Enter putback mode.

            d_savedEback_p = eback();
            d_savedGptr_p  = gptr();
            d_savedEgptr_p = egptr();
            setg(pBackEnd - 1, pBackEnd - 1, pBackEnd);
            d_mode = BDESU_INPUT_PUTBACK_MODE;
        }
    }
    else {
        return eof;                                                   // RETURN
    }

    // We have made a putback position available.  Assign to it, and return.

    *gptr() = traits_type::to_char_type(c);
    return c;
}

bsl::streambuf::int_type
bdesu_FdStreamBuf::overflow(int_type c)
{
    // Invariant: we always leave room in the buffer for one character
    // more than the base class knows about, this is so that there is always
    // room for this method to stuff a character onto the end of the buffer.
    // This method sees the buffer as '[d_buf_p, d_bufEOS_p)', but the rest of
    // this class, during output, only sees '[d_buf_p, d_bufEOS_p - 1)'.

    // Switch to output mode, if necessary.

    if (BDESU_OUTPUT_MODE != d_mode && 0 != switchToOutputMode()) {
        return traits_type::eof();                                    // RETURN
    }

    char *iend = pptr();

    // Put c at the end of the internal buffer.

    if (!traits_type::eq_int_type(c, traits_type::eof())) {
        *iend++ = c;
    }

    const int_type ret = d_fileHandler.write(d_buf_p, iend - d_buf_p)
                         ? outputError()
                         : traits_type::not_eof(c);
    setp(d_buf_p, d_bufEOS_p - 1);

    return ret;
}

bdesu_FdStreamBuf *bdesu_FdStreamBuf::setbuf(char            *buffer,
                                             bsl::streamsize  numBytes)
    // 'buffer == 0 && n == 0' means to make this object have a 1 byte buffer.
    // 'buffer != 0 && n > 0' means to use 'buffer' as this object's internal
    // buffer, rather than the buffer that would otherwise be allocated
    // automatically.  'buffer' must be a pointer to an array of 'char' whose
    // size is at least 'numBytes'.  This member function, if called, must be
    // called before any I/O has been performed on the stream, otherwise it has
    // no effect.
{
    if (BDESU_NULL_MODE == d_mode && 0 == d_buf_p) {
        allocateBuffer(buffer, numBytes);
    }

    return this;
}

bsl::streambuf::pos_type
bdesu_FdStreamBuf::seekoff(off_type               offset,
                           bsl::ios_base::seekdir whence,
                           bsl::ios_base::openmode)
{
    const bdesu_FileUtil::Whence CUR = FileUtil::BDESU_SEEK_FROM_CURRENT;

    bdesu_FileUtil::Whence dir;
    switch (whence) {
      case bsl::ios_base::beg: {
        dir = FileUtil::BDESU_SEEK_FROM_BEGINNING;
      } break;
      case bsl::ios_base::cur: {
        dir = FileUtil::BDESU_SEEK_FROM_CURRENT;
      } break;
      case bsl::ios_base::end: {
        dir = FileUtil::BDESU_SEEK_FROM_END;
      } break;
      default: {
        return pos_type(-1);                                          // RETURN
      }
    }

    if (!isOpened()) {
        return pos_type(-1);                                          // RETURN
    }

    // Note that 'seekInit' will flush the output buffer if we're in output
    // mode, and exit putback mode if we're in it, leaving us in either input
    // mode or null mode.

    if (0 != seekInit()) {
        return pos_type(-1);                                          // RETURN
    }

    BSLS_ASSERT((BDESU_INPUT_MODE == d_mode) | (BDESU_NULL_MODE == d_mode));

    // Note that 'seekReturn' will put us into 'BDESU_NULL_MODE' and release
    // our pointers into the buffer.  We need to call it if we're  changing our
    // file position.

    // Seek relative to beginning or end, regardless of whether we're in input
    // mode.

    if (CUR != dir) {
        return seekReturn(d_fileHandler.seek(offset, dir));           // RETURN
    }

    // Seek relative to current position.  Simple if we're not in input mode.

    if (BDESU_INPUT_MODE != d_mode) {
        return seekReturn(d_fileHandler.seek(offset, CUR));           // RETURN
    }

    // We're in input mode.  See if we're in mapped mode.

    if (0 != d_mmapBase_p) {
        // Offset is relative to 'gptr()'.  We need to do a bit of arithmetic
        // to get an offset relative to the external file pointer.  Note that
        // mapping the file left the file pointer at the end of the mapped
        // area.  Note that since we're in mapped mode, we know no translation
        // between 'n' and '\r\n' is taking place.

        bsl::streamoff adjust = egptr() - gptr();

        if (0 == offset) {
            // We're just returning our current position, so we don't want to
            // exit input mode or shift the file pointer.

            return pos_type(d_fileHandler.seek(0, CUR) - adjust);     // RETURN
        }

        return seekReturn(d_fileHandler.seek(offset - adjust, CUR));  // RETURN
    }

    // This object is not in mmap mode, it's in read mode.  This gets tricky,
    // because the file position is at the end of the read buffer.  We need to
    // calculate the disk distance 'diskAdjust' from the current location to
    // the end of the input buffer, which takes the translation of '\n's to
    // '\r\n's is into account.

    bsl::streamoff diskAdjust = d_fileHandler.getOffset(gptr(), egptr());
    if (0 == offset) {
        // We have only to return our current position, so it's not necessary
        // to exit input mode or change our file position.

        return pos_type(d_fileHandler.seek(0, CUR) - diskAdjust);     // RETURN
    }

    return seekReturn(d_fileHandler.seek(offset - diskAdjust, CUR));
}

bsl::streambuf::pos_type
bdesu_FdStreamBuf::seekpos(pos_type pos, bsl::ios_base::openmode)
{
    bsl::streamoff offset = off_type(pos);

    if (offset >= 0 && isOpened() && 0 == seekInit()) {
        return seekReturn(d_fileHandler.seek(offset,                  // RETURN
                                         FileUtil::BDESU_SEEK_FROM_BEGINNING));
    }

    return pos_type(-1);
}

int bdesu_FdStreamBuf::sync()
{
    if (BDESU_OUTPUT_MODE == d_mode) {
        return traits_type::eq_int_type(overflow(traits_type::eof()), // RETURN
                                        traits_type::eof()) ? -1 : 0;
    }

    return 0;
}

void bdesu_FdStreamBuf::imbue(const bsl::locale& locale)
{
    if (bsl::locale() != locale) {
        BSLS_ASSERT_OPT(0 && "Changing locales not supported\n");
    }

#if 0
    // TBD save this code until we've verified that the above works on all
    // platforms

    typedef bsl::codecvt<char, char, traits_type::state_type> Codecvt;

    const Codecvt *pCvt = &bsl::use_facet<Codecvt>(locale);

    if (1 != pCvt->encoding() || 1 != pCvt->max_length()
       || !pCvt->always_noconv()) {
        BSLS_ASSERT_OPT(0 && "Changing locales not supported\n");
    }
#endif
}

bsl::streamsize bdesu_FdStreamBuf::showmanyc()
{
    // Is there any possibility that reads can succeed?

    if (!isOpened()
     || BDESU_OUTPUT_MODE == d_mode || BDESU_ERROR_MODE == d_mode) {
        return -1;                                                    // RETURN
    }
    else if (BDESU_INPUT_PUTBACK_MODE == d_mode) {
        // return the number of characters in the putback buffer

        return egptr() - gptr();                                      // RETURN
    }

    // We are assuming here that 'showmanyc()' is called by 'in_avail()'
    // only when 'gptr() >= egptr()', meaning that the position of the file
    // descriptor matches the position the caller perceives this object as
    // being at.  We are also assuming that if the file descriptor is not
    // associated with a file, the seek will return -1.

    bsl::streamoff pos = d_fileHandler.seek(0,
                                            FileUtil::BDESU_SEEK_FROM_CURRENT);
    bsl::streamoff sz  = d_fileHandler.fileSize();

    return pos >= 0 && sz > pos ? sz - pos : 0;
}

bsl::streamsize bdesu_FdStreamBuf::xsgetn(char            *buffer,
                                          bsl::streamsize  numBytes)
{
    BSLS_ASSERT(0 <= numBytes);

    if (0 == buffer) {
        return 0;                                                     // RETURN
    }

    char      *start = buffer;
    char      *end   = buffer + numBytes;
    const int  eof   = traits_type::eof();

    while (buffer < end) {
        if (gptr() < egptr()) {
            const int chunk = bsl::min(egptr() - gptr(), end - buffer);
            traits_type::copy(buffer, gptr(), chunk);
            buffer += chunk;
            gbump(chunk);
        }
        else {
            int c = sbumpc();
            if (eof != c) {
                *buffer = c;
                ++buffer;
            }
            else {
                break;
            }
        }
    }

    return buffer - start;
}

bsl::streamsize bdesu_FdStreamBuf::xsputn(const char      *buffer,
                                          bsl::streamsize  numBytes)
{
    BSLS_ASSERT(0 <= numBytes);

    if (0 == buffer) {
        return 0;                                                     // RETURN
    }

    const char *start = buffer;
    const char *end   = buffer + numBytes;
    const int   eof   = traits_type::eof();

    if (BDESU_OUTPUT_MODE != d_mode && 0 != switchToOutputMode()) {
        return 0;                                                     // RETURN
    }

    while (buffer < end) {
        if (pptr() < epptr()) {
            const int chunk = bsl::min(epptr() - pptr(), end - buffer);
            traits_type::copy(pptr(), buffer, chunk);
            buffer += chunk;
            pbump(chunk);
        }
        else {
            if (eof == sputc(*buffer)) {
                break;
            }
            ++buffer;
        }
    }

    return buffer - start;
}

}  // close namespace BloombergLP

//-----------------------------------------------------------------------------
// Adapted to bde from STLport, 2009
//     'bdesu_FdStreamBuf' from 'bsl::filebuf'
//     'bdesu_FdStreamBuf_FileHandler' from 'bsl::_Filebuf_base'
//
// Copyright (c) 1996,1997,1999
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
//-----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2009
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ------------------------------ END-OF-FILE ---------------------------------
