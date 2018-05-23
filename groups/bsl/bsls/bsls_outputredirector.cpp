// bsls_outputredirector.cpp                                          -*-C++-*-
#include <bsls_outputredirector.h>

#include <bsls_ident.h>
BSLS_IDENT("$Id$ $CSID$")

#include <bsls_assert.h>
#include <bsls_bsltestutil.h>   // for testing only

#include <stdlib.h>
#include <string.h>

#include <sys/types.h> // 'struct stat': required on Sun and Windows only

#if defined(BSLS_PLATFORM_OS_WINDOWS)
# include <fcntl.h>     // '_O_BINARY'
# include <io.h>        // '_dup2', '_dup', '_close'
# define snprintf _snprintf
#else
# include <unistd.h>
# include <stdint.h>    // 'SIZE_MAX', cannot include on all Windows platforms
# include <stdlib.h>    // 'mkstemp'
#endif

#ifndef SIZE_MAX
#define SIZE_MAX (static_cast<size_t>(-1))
    // 'SIZE_MAX' is only defined as part of C99, so it may not exist in some
    // pre-C++11 compilers.
#endif

namespace BloombergLP {
namespace bsls {

// PRIVATE MANIPULATORS
void OutputRedirector::cleanup()
{
    disable();
    cleanupFiles();
}

void OutputRedirector::cleanupFiles()
{
#ifdef BSLS_PLATFORM_OS_WINDOWS
    _close(d_duplicatedOriginalFd);
#else
    close(d_duplicatedOriginalFd);
#endif

    d_duplicatedOriginalFd = -1;

    if (d_isFileCreatedFlag) {
        unlink(d_fileName);
        d_isFileCreatedFlag = false;
    }
}

bool OutputRedirector::generateTempFileName()
{
#ifdef BSLS_PLATFORM_OS_WINDOWS
    char tmpPathBuf[k_PATH_BUFFER_SIZE];
    if (! GetTempPathA(k_PATH_BUFFER_SIZE, tmpPathBuf) ||
        ! GetTempFileNameA(tmpPathBuf, "bsls", 0, d_fileName)) {
        return false;                                                 // RETURN
    }
#else
    // Copy in the pattern.

    ::strncpy(d_fileName,
              "/tmp/bsls_outputredirector_XXXXXX",
              sizeof(d_fileName));
    if ('\0' != d_fileName[sizeof(d_fileName) - 1]) {
        return false;                                                 // RETURN
    }

    const int fd = ::mkstemp(d_fileName);
    if (fd < 0) {
        // 'mkstemp' failed.

        return false;                                                 // RETURN
    }
    int rc = ::close(fd);
    if (0 != rc) {
        return false;                                                 // RETURN
    }
#endif
    if (d_veryVerbose) {
        fprintf(nonRedirectedStream(),
                "\tUsing '%s' as a base filename.\n",
                d_fileName);
    }
    return '\0' != d_fileName[0]; // Ensure that 'd_fileName' is not empty
}

// CREATORS
OutputRedirector::OutputRedirector(Stream which,
                                   bool   verbose,
                                   bool   veryVerbose)
: d_stream(which)
, d_isRedirectingFlag(false)
, d_isFileCreatedFlag(false)
, d_isOutputReadyFlag(false)
, d_outputSize(0)
, d_duplicatedOriginalFd(-1)
, d_verbose(verbose)
, d_veryVerbose(veryVerbose)
{
    BSLS_ASSERT(which == e_STDOUT_STREAM || which == e_STDERR_STREAM);

    d_fileName[0] = '\0';
    memset(&d_originalStat, 0, sizeof(struct stat));
}

OutputRedirector::~OutputRedirector()
{
    cleanup();
}

// MANIPULATORS
void OutputRedirector::disable()
{
    if (!d_isRedirectingFlag) {
        return;                                                       // RETURN
    }

    d_isRedirectingFlag = false;

    fflush(redirectedStream());

#ifdef BSLS_PLATFORM_OS_WINDOWS
    const int status = _dup2(d_duplicatedOriginalFd,
                             _fileno(redirectedStream()));
#else
    const int status = dup2(d_duplicatedOriginalFd,
                            fileno(redirectedStream()));
#endif

    if (status < 0) {
        if (d_veryVerbose) {
            fprintf(nonRedirectedStream(),
                    "Error " __FILE__ "(%d): Bad 'dup2' status.\n",
                    __LINE__);
        }
        cleanupFiles();
        abort();
    }
}

void OutputRedirector::enable()
{
    if (d_isRedirectingFlag) {
        if (d_veryVerbose) {
            fprintf(nonRedirectedStream(),
                    "Warning " __FILE__ "(%d): Output already redirected\n",
                    __LINE__);
        }

        return;                                                       // RETURN
    }

    // Retain information about original file descriptor for use in later
    // tests.

    const int originalFD = fileno(redirectedStream());

    BSLS_ASSERT(-1 != originalFD);
    BSLS_ASSERT(0 == fstat(originalFD, &d_originalStat));

    if (d_duplicatedOriginalFd == -1) {
#ifdef BSLS_PLATFORM_OS_WINDOWS
        d_duplicatedOriginalFd = _dup(originalFD);
#else
        d_duplicatedOriginalFd = dup(originalFD);
#endif
    }

    if (d_duplicatedOriginalFd < 0) {
        if (d_veryVerbose) {
                fprintf(nonRedirectedStream(),
                        "Error " __FILE__ "(%d): Bad 'dup' value.\n",
                        __LINE__);
            }
        cleanup();
        abort();
    }

    if (!d_isFileCreatedFlag) {
        if (!generateTempFileName()) {

            // Get temp file name

            if (d_veryVerbose) {
                fprintf(nonRedirectedStream(),
                        "Error "
                        __FILE__
                        "(%d): Failed to get temp file name for capture\n",
                        __LINE__);
            }
            cleanup();
            abort();
        }

        d_isFileCreatedFlag = true;
    }

    if (! freopen(d_fileName, "w+", redirectedStream())) {
        if (d_veryVerbose) {
            fprintf(nonRedirectedStream(),
                    "Error " __FILE__ "(%d): Failed to redirect stdout to"
                    " temp file '%s'\n",
                    __LINE__, d_fileName);
        }
        cleanup();
        abort();
    }

#ifdef BSLS_PLATFORM_OS_WINDOWS
    // In Windows, we need to convert the stream to binary mode because Windows
    // will automatically convert '\n' to '\r\n' in text mode output.  Normally
    // this would not be a problem, since the '\r\n' would be converted back to
    // '\n' when we read the file.  However, since we are using the size of the
    // written file to know how much to read, having extra characters will lead
    // to a faulty size reading.

    if (_setmode(_fileno(redirectedStream()), _O_BINARY) < 0) {
        if (d_veryVerbose) {
            fprintf(nonRedirectedStream(),
                    "Error " __FILE__ "(%d): Binary mode change failed.\n",
                    __LINE__);
        }
        cleanup();
        abort();
    }
#endif

    if (EOF == fflush(redirectedStream())) {
        if (d_veryVerbose) {
            fprintf(nonRedirectedStream(),
                    "Warning " __FILE__ "(%d): Error flushing stream\n",
                    __LINE__);
        }
    }

    d_isRedirectingFlag = true;
}

bool OutputRedirector::load()
{
    BSLS_ASSERT(d_isRedirectingFlag);

    if (ferror(redirectedStream()) != 0) {
        if (d_veryVerbose) {
            fprintf(nonRedirectedStream(),
                    "Error " __FILE__ "(%d): Stream has an error\n",
                    __LINE__);
        }
        return false;                                                 // RETURN
    }

    const long tempOutputSize = ftell(redirectedStream());
    const long incremented    = tempOutputSize + 1;

    if (tempOutputSize < 0 || incremented < 0) {
        // Protect against overflow or negative value

        if (d_veryVerbose) {
            fprintf(nonRedirectedStream(),
                    "Bad value from 'ftell': %ld",
                    incremented);
        }
        return false;                                                 // RETURN
    }

    // Conversion to 'unsigned long' is safe because 'incremented' > 0

    if (static_cast<unsigned long>(incremented) > SIZE_MAX) {
        // Our 'incremented' will not fit in a size_t, so it is too big for our
        // buffer.

        if (d_veryVerbose) {
            fprintf(nonRedirectedStream(),
                    "Bad value from 'ftell': %ld",
                    incremented);
        }
        return false;                                                 // RETURN
    }

    // Conversion to 'size_t' is safe because 'tempOutputSize' is nonnegative
    // and 'tempOutputSize'+1 is no larger than 'SIZE_MAX'.

    d_outputSize = static_cast<size_t>(tempOutputSize);

    if (static_cast<size_t>(incremented) > k_OUTPUT_REDIRECTOR_BUFFER_SIZE) {
        // Refuse to load output if it will not all fit in the scratch buffer,
        // INCLUDING the final null byte.

        if (d_veryVerbose) {
            fprintf(nonRedirectedStream(),
                    "Error "
                        __FILE__
                        "(%d): Captured output exceeds read buffer size\n",
                    __LINE__);
        }
        d_outputSize = 0u;
        return false;                                                 // RETURN
    }

    rewind(redirectedStream());

    const long charsRead = fread(d_outputBuffer,
                                 sizeof(char),
                                 d_outputSize,
                                 redirectedStream());

    if (charsRead < 0 || static_cast<unsigned long>(charsRead)!=d_outputSize) {
        // We failed to read all output from the capture file.

        if (d_veryVerbose) {
            if (ferror(redirectedStream())) {
                // We encountered a file error, not 'EOF'.

                fprintf(nonRedirectedStream(),
                        "Error "
                            __FILE__
                            "(%d): Non-EOF error with file\n",
                        __LINE__);
            }
            fprintf(nonRedirectedStream(),
                    "Error "
                        __FILE__
                        "(%d): Could not read all captured output\n",
                    __LINE__);
        }

        if (charsRead < 0) {
            d_outputBuffer[0] = '\0';
        } else if (static_cast<unsigned long>(charsRead) >= d_outputSize) {
            // This case should never happen.  This assignment is safe because
            // the total buffer size is enough to hold 'd_outputSize' + 1.

            d_outputBuffer[d_outputSize] = '\0';
        } else {
            d_outputBuffer[charsRead] = '\0';
        }
            // ...to ensure that direct inspection of buffer does not overflow

        return false;                                                 // RETURN

    } else {
        // We have read all output from the capture file.

        d_outputBuffer[d_outputSize] = '\0';
    }

    d_isOutputReadyFlag = true;

    return true;
}

void OutputRedirector::clear()
{
    BSLS_ASSERT(d_isRedirectingFlag);

    d_outputSize = 0u;
    d_isOutputReadyFlag = false;
    d_outputBuffer[0] = '\0';
    rewind(redirectedStream());
}

// ACCESSORS
int OutputRedirector::compare(const char *expected) const
{
    BSLS_ASSERT(expected);

    return compare(expected, strlen(expected));
}

int
OutputRedirector::compare(const char *expected, size_t expectedLength) const
{
    BSLS_ASSERT(expected || ! expectedLength);

    if (!d_isOutputReadyFlag) {
        if (d_veryVerbose) {
            fprintf(nonRedirectedStream(),
                    "Error " __FILE__ "(%d): No captured output available\n",
                    __LINE__);
        }
        return -1;                                                    // RETURN
    }

    // Use 'memcmp' instead of 'strncmp' to compare 'd_outputBuffer' to
    // 'expected', because 'expected' is allowed to contain embedded nulls.

    return static_cast<size_t>(d_outputSize) != expectedLength ||
           memcmp(d_outputBuffer, expected, expectedLength);
}

const char *OutputRedirector::getOutput() const
{
    return d_outputBuffer;
}

bool OutputRedirector::isOutputReady() const
{
    return d_isOutputReadyFlag;
}

bool OutputRedirector::isRedirecting() const
{
    return d_isRedirectingFlag;
}

FILE *OutputRedirector::nonRedirectedStream() const
{
    // This should return the opposite values

    if (d_stream == e_STDOUT_STREAM) {
        return stderr;                                                // RETURN
    } else {
        return stdout;                                                // RETURN
    }
}

const struct stat& OutputRedirector::originalStat() const
{
    return d_originalStat;
}

size_t OutputRedirector::outputSize() const
{
    return d_outputSize;
}

FILE *OutputRedirector::redirectedStream() const
{
    if (d_stream == e_STDOUT_STREAM) {
        return stdout;                                                // RETURN
    } else {
        return stderr;                                                // RETURN
    }
}

OutputRedirector::Stream OutputRedirector::redirectedStreamId() const
{
    return d_stream;
}

}  // close package namespace
}  // close enterprise namespace

// ----------------------------------------------------------------------------
// Copyright 2013 Bloomberg Finance L.P.
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
