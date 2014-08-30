// bsls_log.cpp                                                       -*-C++-*-
#include <bsls_log.h>

#include <bsls_ident.h>
BSLS_IDENT("$Id$ $CSID$")

#include <bsls_assert.h>
#include <bsls_atomicoperations.h> // Atomic pointers
#include <bsls_bsltestutil.h>      // for testing only
#include <bsls_platform.h>         // 'BSLS_PLATFORM_OS_WINDOWS'

#include <stdarg.h> // 'va_list', 'va_start', 'va_end', 'va_copy'
#include <stdio.h>  // 'puts', 'snprintf', 'vsnprintf'
#include <stdlib.h> // 'malloc', 'free'
#include <string.h> // 'strlen'

#ifdef BSLS_PLATFORM_OS_WINDOWS
#include <windows.h> // 'GetConsoleWindow'
#endif

#if defined(BSLS_PLATFORM_CMP_MSVC) && !defined(va_copy)
#define va_copy(dest, src) (dest = src)
// Because VS doesn't define 'va_copy' until VS2013, we must define it
// ourselves, knowing how variadic arguments are implemented on Windows.  In
// both the x86 and x64 cases, 'va_list' is implemented as a simple pointer,
// meaning that 'va_copy' can simply be replaced with an assignment.
#endif

namespace BloombergLP {
namespace bsls {
namespace {
                         // =======================
                         // class BufferScopedGuard
                         // =======================
class BufferScopedGuard {
    // This class implements a scoped guard that provides a method, 'allocate',
    // which allocates a buffer of a specified size.  The scoped guard is then
    // responsible for the proper deletion of this allocated buffer upon
    // destruction of the instance or upon a new call to 'allocate'.

  private:
    // DATA
    char *d_buffer_p;
        // Owned by this object.  Lifetime shall not exceed the lifetime of
        // this object.

    // NOT IMPLEMENTED
    BufferScopedGuard(const BufferScopedGuard&);                    // = delete

    BufferScopedGuard& operator=(const BufferScopedGuard&);         // = delete

  public:
    // CREATORS
    BufferScopedGuard();
        // Create a scoped guard that is not responsible for any initial
        // buffer.

    ~BufferScopedGuard();
        // Destroy the last buffer returned by the 'allocate' function, if the
        // 'allocate' function was ever called.  Destroy this scoped guard.

    // MANIPULATORS
    char* allocate(size_t numBytes);
        // Return a pointer to a buffer guaranteed to be large enough to store
        // the specified 'numBytes' bytes, or 'NULL' if memory was not
        // available.  Destroy the last buffer returned by this method, if this
        // method was ever called before.  The returned pointer is owned by
        // this object and will remain valid until this object is destroyed or
        // until this method ('allocate') is called again.

};

// CREATORS
BufferScopedGuard::BufferScopedGuard()
: d_buffer_p(NULL)
{
}

BufferScopedGuard::~BufferScopedGuard()
{
    if(d_buffer_p) {
        free(d_buffer_p);
        d_buffer_p = NULL;
    }
}

// MANIPULATORS
char* BufferScopedGuard::allocate(size_t numBytes) {
    // We don't want to use realloc, because it guarantees that the contents of
    // the buffer are copied to the new block (in the case that the block is
    // moved).  The code itself does not rely on this ability, so we do not
    // want the inefficiency of the copy if it does not need to be done.

    if(d_buffer_p) {
        free(d_buffer_p);
        d_buffer_p = NULL;
    }

    d_buffer_p = static_cast<char*>(malloc(numBytes));

    return d_buffer_p;
}

// ============================================================================
//                      LOCAL FUNCTION DEFINITIONS
// ============================================================================

static
int vsnprintf_alwaysCount(char * const       buffer,
                          const size_t       size,
                          const char * const format,
                          va_list            arguments)
    // Return the number of characters that would be generated if the string
    // formed by applying the 'printf'-style formatting rules to the specified
    // 'format' string with the specified 'arguments' as substitutions were
    // written to a buffer that is sufficiently large, not including the null
    // byte.  In the case of an error, return a negative value.  If the
    // specified 'size' indicates that the specified 'buffer' is large enough
    // to store the entire formatted string in addition to a null byte, write
    // the entire formatted string followed by a zero byte to 'buffer'.
    // Otherwise, the bytes in 'buffer' up to 'size' number of bytes may be
    // written to any value.  The behavior is undefined unless 'buffer'
    // contains sufficient space to store 'size' bytes and 'format' is a valid
    // 'printf'-style format string with all expected substitutions present in
    // 'arguments'.
{

    // Unfortunately, 'vsnprintf' has observably different behavior on
    // different systems.  From the documentation for the various systems, we
    // have the following table of behaviors:
    //..
    //  =======================================================================
    //               'vsnprintf' behavior in case of overflow
    //  -----------------------------------------------------------------------
    //  System                   Return Value                   Null Written?
    //  ---------   --------------------------------------    -----------------
    //  Microsoft   # Written Chars (not incl. '\0') or -1    No
    //  BSD         # Expected Chars (not incl. '\0')         Yes
    //  GCC         # Expected Chars (not incl. '\0')         No
    //  Solaris     # Expected Chars (not incl. '\0')         No
    //  AIX         # Expected Chars (not incl. '\0')         No
    //  =======================================================================
    //..
    // The solution?  On Microsoft, we simply use a different function to
    // generate the count.  To solve the BSD / other inconsistency, we simply
    // specify in our contract that the output string is undefined unless there
    // is enough room to hold the additional zero byte.
    int count;

#ifdef BSLS_PLATFORM_OS_WINDOWS
    // Because we are making *two* calls using 'arguments', we must make a copy
    // so the first call does not ruin the 'va_list'.
    va_list arguments_copy;
    va_copy(arguments_copy, arguments);
    count = _vscprintf(format, arguments_copy);
    va_end(arguments_copy);

    if(count >= 0) {
        // Because 'count >= 0', the cast to 'size_t' is safe.
        const size_t countCasted = static_cast<size_t>(count);
        if(size > countCasted) {
            count = vsnprintf(buffer, size, format, arguments);
        }
    }
#else
    count = vsnprintf(buffer, size, format, arguments);
#endif

    return count;
}

static
int vsnprintf_allocate(char * const        originalBuffer,
                       const size_t        originalBufferSize,
                       BufferScopedGuard&  guard,
                       char ** const       outputBuffer,
                       size_t * const      outputBufferSize,
                       const char * const  format,
                       va_list             arguments)
    // If no error occurs, load into the specified 'outputBuffer' a pointer to
    // the beginning of a buffer containing a null-terminated C-style string
    // equivalent to the formatted string generated from the 'printf'-style
    // format specifiers in the specified 'format' string, using the specified
    // 'arguments' as the substitutions.  The pointer loaded into
    // 'outputBuffer' may be equal to the specified 'originalBuffer', or may
    // reference a buffer owned and allocated by the specified 'guard'
    // reference.  If the pointer loaded into 'outputBuffer' is not equal to
    // 'originalBuffer', the values of the bytes referenced by 'originalBuffer'
    // may be changed to any value (in other words, the only instance where the
    // bytes in the original buffer are guaranteed to be equal to any specific
    // value is the instance in which the original buffer is the output
    // buffer).  Load a number into the specified 'outputBufferSize' that is
    // greater than or equal to the number of bytes written into the output
    // buffer (including the null byte) and less than or equal to the number of
    // total available bytes in the output buffer.  Return the number of
    // characters written to the buffer referenced by the pointer loaded into
    // 'outputBuffer', not including the null byte.  If an error occurs with
    // memory allocation or with the C runtime implementation of 'vsnprintf',
    // return a negative value.  In the case of an error, any values may be
    // loaded into 'outputBuffer' and 'outputBufferSize'.  (In other words,
    // 'outputBuffer' and 'outputBufferSize' will refer to valid values only if
    // this function returns a non-negative value).  The behavior is undefined
    // unless 'originalBuffer' is not null and refers to a buffer large enough
    // to hold at least the specified 'originalBufferSize' bytes,
    // 'outputBuffer' is not null, 'outputBufferSize' is not null, 'format' is
    // a null-terminated C-style string, and the various 'printf'-style format
    // specifiers in 'format' correspond to valid substitution values in
    // 'arguments'.
{
    size_t  bufferSize = originalBufferSize;
    char   *buffer     = originalBuffer;

    // Because we are making *two* calls using 'arguments', we must make a copy
    // so the first call does not ruin the 'va_list'.
    va_list arguments_copy;
    va_copy(arguments_copy, arguments);
    int status = vsnprintf_alwaysCount(buffer,
                                       bufferSize,
                                       format,
                                       arguments_copy);
    va_end(arguments_copy);

    if(status >= 0) {
        // Cast is safe because status is nonnegative
        const size_t statusCasted = static_cast<size_t>(status);
        if(statusCasted + 1 > originalBufferSize) {
            // The number of needed characters did not fit in the buffer, so we
            // must allocate and then call 'vsnprintf' again (this time, we do
            // not need the alwaysCount variant).
            bufferSize = statusCasted + 1;
            buffer = guard.allocate(bufferSize);

            if(buffer) {
                const int newStatus = vsnprintf(buffer,
                                                bufferSize,
                                                format,
                                                arguments);
                if(newStatus != status) {
                    // Some weird error.
                    if(newStatus < 0) {
                        // If the new status was negative then we should return
                        // the new status so the user can get better error
                        // information:
                        status = newStatus;
                    } else {
                        // Otherwise, if it was nonnegative but not the
                        // expected value, we should let the user know that we
                        // failed, so we will set the status to a negative
                        // value:
                        status = -2;
                    }
                }
            } else {
                // Allocation error.  Just set the status to a negative value:
                status = -1;
            }
        }
    }

    *outputBufferSize = bufferSize;
    *outputBuffer = buffer;
    return status;
}

static
int snprintf_allocate(char * const        originalBuffer,
                      const size_t        originalBufferSize,
                      BufferScopedGuard&  guard,
                      char ** const       outputBuffer,
                      size_t * const      outputBufferSize,
                      const char * const  format,
                      ...)
    // If no error occurs, load into the specified 'outputBuffer' a pointer to
    // the beginning of a buffer containing a null-terminated C-style string
    // equivalent to the formatted string generated from the 'printf'-style
    // format specifiers in the specified 'format' string, using the specified
    // variadic arguments '...' as the substitutions.  The pointer loaded into
    // 'outputBuffer' may be equal to the specified 'originalBuffer', or may
    // reference a buffer owned and allocated by the specified 'guard'
    // reference.  If the pointer loaded into 'outputBuffer' is not equal to
    // 'originalBuffer', the values of the bytes referenced by 'originalBuffer'
    // may be changed to any values (in other words, the only instance where
    // the bytes in the original buffer are guaranteed to be equal to any
    // specific values is the instance in which the original buffer is the
    // output buffer).  Load a number into the specified 'outputBufferSize'
    // that is greater than or equal to the number of bytes written into the
    // output buffer (including the null byte) and less than or equal to the
    // number of total available bytes in the output buffer.  Return the number
    // of characters written to the buffer referenced by the pointer loaded
    // into 'outputBuffer', not including the null byte.  If an error occurs
    // with memory allocation or with the C runtime implementation of
    // 'vsnprintf', return a negative value.  In the case of an error, any
    // arbitrary values may be loaded into 'outputBuffer' and
    // 'outputBufferSize'.  (In other words, the pointers 'outputBuffer' and
    // 'outputBufferSize' will refer to meaningful values only if this function
    // returns a non-negative value).  The behavior is undefined unless
    // 'originalBuffer' is not null and refers to a buffer large enough to hold
    // at least the specified 'originalBufferSize' bytes, 'format' is a
    // null-terminated C-style string, and the various 'printf'-style format
    // specifiers in 'format' correspond to valid substitution values in the
    // variadic argument list ('...').
{
    va_list arguments;
    va_start(arguments, format);
    const int status = vsnprintf_allocate(originalBuffer,
                                          originalBufferSize,
                                          guard,
                                          outputBuffer,
                                          outputBufferSize,
                                          format,
                                          arguments);
    va_end(arguments);
    return status;
}

}  // close unnamed namespace

                         // =========
                         // class Log
                         // =========

// CLASS DATA
bsls::AtomicOperations::AtomicTypes::Pointer Log::s_logMessageHandler =
    {reinterpret_cast<void*>(&Log::platformDefaultMessageHandler)};
    // Static initialization of the Log::s_logMessageHandler function pointer
    // which holds the log handler function.  The pointer is initialized to the
    // address of the 'static' function 'platformDefaultMessageHandler'.

// CLASS METHODS

                         // Dispatcher Method

void Log::logFormatted(const char *file,
                       int         line,
                       const char *format,
                       ...)
{

    // Scoped guard to handle the buffer if it needs to be dynamically
    // allocated.
    BufferScopedGuard guard;

    // This is the initial stack-allocated buffer which we will use to store
    // the formatted string if it can fit.
    const size_t originalBufferSize = 1024;
    char         originalBuffer[originalBufferSize];

    // This is the "final buffer" we will use when outputting our value to the
    // logging function.  We will let 'vsnprintf_allocate' set this value.
    size_t  bufferSize;
    char   *buffer;

    va_list arguments;
    va_start(arguments, format);
    const int status = vsnprintf_allocate(originalBuffer,
                                          originalBufferSize,
                                          guard,
                                          &buffer,
                                          &bufferSize,
                                          format,
                                          arguments);
    va_end(arguments);

    if(status < 0) {
        BSLS_LOG_SIMPLE("Low-level log failure.");
        // Weird error.  Could be a memory allocation failure.  Just quit.
        return;                                                       // RETURN
    }

    bsls::Log::logMessage(file, line, buffer);
}

                         // Standard Log Message Handlers

void Log::platformDefaultMessageHandler(const char *file,
                                        const int   line,
                                        const char *message)
{

#ifdef BSLS_PLATFORM_OS_WINDOWS
    // In Windows we must check for console mode
    if(GetConsoleWindow() == NULL) {
        BSLS_ASSERT_OPT(file);
        BSLS_ASSERT(line >= 0);
        BSLS_ASSERT_OPT(message);
        // To avoid the multi-threaded interlacing of messages, we need to
        // pre-format a buffer and then pass that to 'OutputDebugStringA'.

        // This is the initial stack-allocated buffer which we will use to
        // store the formatted string if it can fit:
        const size_t originalBufferSize = 1024;
        char originalBuffer[originalBufferSize];

        // This is the "final buffer" we will use when outputting our value to
        // the logging function.  We will let 'snprintf_allocate' set this
        // value:
        size_t bufferSize;
        char *buffer;

        BufferScopedGuard guard;

        va_list arguments;
        va_start(arguments, format);
        const int status = snprintf_allocate(originalBuffer,
                                             originalBufferSize,
                                             guard,
                                             &buffer,
                                             &bufferSize,
                                             "%s:%d %s\n",
                                             file,
                                             line,
                                             message);
        va_end(arguments);

        if(status >= 4) {
            // Ensure no weird errors happened.  At least four characters must
            // have been written.  Note that checking for 'status >= 4' and not
            // 'status >= 0' is simply a convenience, and should not make a
            // difference in practice, unless one of the functions is somehow
            // exhibiting undefined behavior due to bad user input.
            OutputDebugStringA(buffer);
        } else {
            // There are legitimate reasons for 'status' to be negative, such
            // as a memory allocation failure.  Therefore, we will simply log
            // some simple error so that we don't fail quietly:
            OutputDebugStringA("Low-level log failure.\n");
        }
    } else {
        stderrMessageHandler(file, line, message);
    }
#else
    // In non-Windows, we will just use 'stderr'.
    stderrMessageHandler(file, line, message);
#endif

}

void Log::stderrMessageHandler(const char * file,
                               int          line,
                               const char * message)
{
    BSLS_ASSERT_OPT(file);
    BSLS_ASSERT(line >= 0);
    BSLS_ASSERT_OPT(message);

    fprintf(stderr, "%s:%d %s\n", file, line, message);
    fflush(stderr);
}

void Log::stdoutMessageHandler(const char *file,
                               int         line,
                               const char *message)
{
    BSLS_ASSERT_OPT(file);
    BSLS_ASSERT(line >= 0);
    BSLS_ASSERT_OPT(message);

    fprintf(stdout, "%s:%d %s\n", file, line, message);
    fflush(stdout);
}

}  // close package namespace
}  // close enterprise namespace

// ----------------------------------------------------------------------------
// Copyright (C) 2014 Bloomberg Finance L.P.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
// ----------------------------- END-OF-FILE ----------------------------------
