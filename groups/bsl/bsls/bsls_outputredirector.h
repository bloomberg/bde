// bsls_outputredirector.h                                            -*-C++-*-
#ifndef INCLUDED_BSLS_OUTPUTREDIRECTOR
#define INCLUDED_BSLS_OUTPUTREDIRECTOR

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a means for test drivers to redirect and inspect output.
//
//@CLASSES:
//  bsls::OutputRedirector: namespace for low-level logging functions
//
//@MACROS:
//
//@DESCRIPTION: This component provides a mechanism, 'bsls::OutputRedirector',
// to redirect output sent to either of the standard out or error streams and
// capture that output so that it can be read back and parsed, such as to allow
// a test driver to verify the expected output of a streaming operation.
//
// THIS COMPONENT IS IN A PREVIEW STATE AND SHOULD NOT BE USED OUTSIDE OF BDE
// TEST DRIVERS
//
///Usage
///-----
// This section illustrates the intended use of this component, or at least, it
// will once the usage example is written.


#ifndef INCLUDED_BSLS_PLATFORM
#include <bsls_platform.h>
#endif

#ifndef INCLUDED_LIMITS_H
#include <limits.h>
#define INCLUDED_LIMITS_H
#endif

#ifndef INCLUDED_STDDEF_H
#include <stddef.h>
#define INCLUDED_STDDEF_H
#endif

#ifndef INCLUDED_STDIO_H
#include <stdio.h>
#define INCLUDED_STDIO_H
#endif

#ifndef INCLUDED_SYS_STAT_H
#include <sys/stat.h>  // 'struct stat[64]': required on Sun and Windows only
#define INCLUDED_SYS_STAT_H
#endif

#if defined(BSLS_PLATFORM_OS_WINDOWS)
# ifndef INCLUDED_WINDOWS_H
# include <windows.h>  // Required for 'MAX_PATH' plus a few other names
# define INCLUDED_WINDWS_H
# endif
#endif

namespace BloombergLP {
namespace bsls {

                         // ======================
                         // class OutputRedirector
                         // ======================

// Temp file creation and output redirection re-purposed from the pre-existing
// module in 'bsls_bsltestutil.t.cpp'.

class OutputRedirector {
    // This class provides a facility for redirecting 'stdout' and 'stderr' to
    // temporary files, retrieving output from the respective temporary file
    // and comparing the output to user-supplied character buffers.  An
    // 'OutputRedirector' object can be in an un-redirected state or a
    // redirected state.  If the redirector is in a redirected state, it will
    // redirect either 'stdout' or 'stderr', but not both simultaneously.  An
    // 'OutputRedirector' object has the concept of a scratch buffer, where
    // output captured from the process' 'stdout' or 'stderr' stream is stored
    // when the 'OutputRedirector' object is in the redirected state.
    // Throughout this class, the term "captured output" refers to data that
    // has been written to the 'stdout' or 'stderr' stream and is waiting to be
    // loaded into the scratch buffer.  Each time the 'load' method is called,
    // the scratch buffer is truncated, and the captured output is moved into
    // the scratch buffer.  When this is done, there is no longer any captured
    // output.

  public:
    // TYPES
    enum Stream {
        // The 'enum' 'Stream' represents the specific stream that our object
        // is responsible for redirecting.
        e_STDOUT_STREAM,
        e_STDERR_STREAM
    };

#if defined(BSLS_PLATFORM_OS_WINDOWS)
    typedef struct stat StatType;
#else
    typedef struct stat64 StatType;
#endif

    static const size_t k_OUTPUT_REDIRECTOR_BUFFER_SIZE          = 4096;
        // This represents the size of the buffer used by the class
        // 'OutputRedirector' to store the captured values loaded in the
        // 'stdout' and 'stderr' error streams.

#ifdef BSLS_PLATFORM_OS_WINDOWS
    static const size_t k_PATH_BUFFER_SIZE = MAX_PATH + 1;
#else
    static const size_t k_PATH_BUFFER_SIZE = PATH_MAX + 1;
#endif

  private:
    // DATA
    char d_fileName[k_PATH_BUFFER_SIZE];      // name of temporary capture file

    char d_outputBuffer[k_OUTPUT_REDIRECTOR_BUFFER_SIZE];
    // 'd_outputBuffer' is the buffer that will hold the captured output.

    const Stream     d_stream;                // the stream for which this
                                              // object is responsible

    bool             d_isRedirectingFlag;     // Is this object currently
                                              // redirecting?

    bool             d_isFileCreatedFlag;     // has a temp file been created?

    bool             d_isOutputReadyFlag;     // has output been read from temp
                                              // file?

    size_t           d_outputSize;            // size of output loaded into
                                              // 'd_outputBuffer'

    StatType         d_originalStat;          // status information for
                                              // 'stdout' or 'stderr' just
                                              // before redirection

    int              d_duplicatedOriginalFd;  // a file descriptor that is
                                              // associated with a duplicate of
                                              // the original target of the
                                              // redirected stream.  This is
                                              // made by calling 'dup' on the
                                              // original stream before any
                                              // redirection happens

    bool             d_verbose;               // verbose flag (if any) for test
                                              // driver.

    bool             d_veryVerbose;           // very verbose flag (if any) for
                                              // test driver.


    // PRIVATE MANIPULATORS
    void cleanup();
        // If the redirector is in a redirected state, restore the original
        // target of the redirected stream.  If the temporary file has been
        // created, delete it.

    void cleanupFiles();
        // Delete the temporary file, if it has been created.

    bool generateTempFileName();
        // Load into 'd_fileName' a file name string corresponding to the name
        // of a valid temp file on the system.  Return 'true' if the name was
        // successfully loaded, or 'false' otherwise.

  private:
    // NOT IMPLEMENTED
    OutputRedirector(const OutputRedirector&);                     // = delete;
    OutputRedirector& operator=(const OutputRedirector&);          // = delete;

  public:
    // CREATORS
    explicit OutputRedirector(Stream which,
                              bool   verbose     = false,
                              bool   veryVerbose = false);
        // Create an 'OutputRedirector' in an un-redirected state, and with an
        // empty scratch buffer.  Upon a call to 'enable', this redirector will
        // be responsible for redirecting the stream associated with the
        // specified 'which' to a temporary file.  The behavior is undefined
        // unless 'which' is equal to 'OutputRedirector::e_STDOUT_STREAM' or
        // 'OutputRedirector::e_STDERR_STREAM'.

    ~OutputRedirector();
        // Destroy this 'OutputRedirector' object.  If the object is in a
        // redirected state, the original stream will be restored to its
        // initial target and the temporary file to which the stream was
        // redirected will be deleted.

    // MANIPULATORS
    void disable();
        // If the redirector is in a redirected state, restore the original
        // target of the redirected stream and close the temporary buffer.  If
        // the redirector is not in a redirected state, this method is a no-op.
        // Calling this method invalidates all output in the temporary file, so
        // a call to 'load' after the next successful 'enable' call will not
        // load any output that was previously written to the file.  This
        // method does not clear the scratch buffer, so one may call 'load'
        // before calling 'disable', and the contents will be available after
        // 'disable' is called.  If 'disable' fails to disable the redirection,
        // it will end the program by calling 'std::abort'.

    void enable();
        // If the 'Stream' specified at construction was 'e_STDOUT_STREAM',
        // redirect 'stdout' to a temporary file.  If the 'Stream' specified at
        // construction was 'e_STDERR_STREAM', redirect 'stderr' to a temporary
        // file.  The temporary file to which the stream is redirected will be
        // created the first time 'enable' is called, and will be deleted when
        // this object is destroyed.  If 'enable' fails to redirect either
        // 'stdout' or 'stderr' it will end the program by calling
        // 'std::abort'.

    bool load();
        // Read captured output into the scratch buffer.  Return 'true' if all
        // captured output was successfully loaded, and 'false' otherwise.
        // Note that captured output is allowed to have zero length.  The
        // behavior is undefined unless 'enable' has been previously called
        // successfully (after the latest call to 'disable', if 'disable' has
        // been called successfully).

    void clear();
        // Reset the scratch buffer to empty.  The behavior is undefined unless
        // 'enable' has been previously called successfully (after the latest
        // call to 'disable' if 'disable' has been called successfully).

    // ACCESSORS
    int compare(const char *expected, size_t expectedLength) const;
        // Compare the character buffer pointed to by the specified pointer
        // 'expected' with any output that has been loaded into the scratch
        // buffer.  The length of the 'expected' buffer is supplied in the
        // specified 'expectedLength'.  Return 0 if the 'expected' buffer has
        // the same length and contents as the scratch buffer, and non-zero
        // otherwise.  Note that the 'expected' buffer is allowed to contain
        // embedded nulls.  The behavior is undefined unless 'enable' has
        // previously been called successfully.

    int compare(const char *expected) const;
        // Compare the character buffer pointed to by the specified pointer
        // 'expected' with any output that has been loaded into the scratch
        // buffer.  The 'expected' buffer is assumed to be a NTBS, and its
        // length is taken to be the string length of the NTBS.  Return 0 if
        // the 'expected' buffer has the same length and contents as the
        // scratch buffer, and non-zero otherwise.  The behavior is undefined
        // unless 'enable' has previously been called successfully.

    const char *getOutput() const;
        // Return the address of the scratch buffer.

    bool isOutputReady() const;
        // Return 'true' if the captured output has been loaded into the
        // scratch buffer, and 'false' otherwise.

    bool isRedirecting() const;
        // Return 'true' if 'stdout' or 'stderr' has been successfully
        // redirected, and 'false' otherwise.

    FILE *nonRedirectedStream() const;
        // Return the value of the global 'stdout' or 'stderr' corresponding to
        // the stream that is not intended to be redirected by this object.

    const StatType& originalStat() const;
        // Return a reference to the status information for 'stdout' collected
        // just before redirection.  This method is used only to test the
        // correctness of 'OutputRedirector'.

    size_t outputSize() const;
        // Return the number of bytes currently loaded into the scratch buffer.

    FILE *redirectedStream() const;
        // Return the value of the global 'stdout' or 'stderr' corresponding to
        // the stream that is intended to be redirected by this object.

    OutputRedirector::Stream redirectedStreamId() const;
        // Return 'OutputRedirector::e_STDOUT_STREAM' if this object is
        // responsible for redirecting 'stdout', and
        // 'OutputRedirector::e_STDERR_STREAM' if this object is responsible
        // for redirecting 'stderr'.
};


}  // close package namespace
}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2017 Bloomberg Finance L.P.
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
