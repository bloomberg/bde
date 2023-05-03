// bdls_pipeutil.h                                                    -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BDLS_PIPEUTIL
#define INCLUDED_BDLS_PIPEUTIL

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide basic portable named-pipe utilities.
//
//@CLASSES:
//  bdls::PipeUtil: Portable utility methods for naming and accessing pipes
//
//@SEE_ALSO: balb_pipecontrolchannel
//
//@DESCRIPTION: This component, 'bdls::PipeUtil', provides portable utility
// methods for named pipes.
//
///Pipe Atomicity
/// - - - - - - -
// Applications that expect multiple writers to a single pipe must should be
// aware that message content might be corrupted (interleaved) unless:
//
//: 1 Each message is written to the pipe in a single 'write' system call.
//: 2 The length of each message is less than 'PIPE_BUF' (the limit for
//:   guaranteed atomicity).
//
// The value 'PIPE_BUF' depends on the platform:
//..
//   +------------------------------+------------------+
//   | Platform                     | PIPE_BUF (bytes) |
//   +------------------------------+------------------+
//   | POSIX (minimum requirement)) |    512           |
//   | IBM                          | 32,768           |
//   | SUN                          | 32,768           |
//   | Linux                        | 65,536           |
//   | Windows                      | 65,536           |
//   +------------------------------+------------------+
//..
//
// Also note that Linux allows the 'PIPE_BUF' size to be changed via the
// 'fcntl' system call.

#include <bdlscm_version.h>

#include <bsl_string.h>
#include <bsl_string_view.h>

#include <bsls_libraryfeatures.h>

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
#include <memory_resource>  // 'std::pmr::polymorphic_allocator'
#endif

#include <string>           // 'std::string', 'std::pmr::string'

namespace BloombergLP {

namespace bdls {
                              // ===============
                              // struct PipeUtil
                              // ===============
struct PipeUtil {
    // This struct contains utility methods for platform-independent named pipe
    // operations.

    static int makeCanonicalName(bsl::string             *pipeName,
                                 const bsl::string_view&  baseName);
    static int makeCanonicalName(std::string             *pipeName,
                                 const bsl::string_view&  baseName);
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
    static int makeCanonicalName(std::pmr::string        *pipeName,
                                 const bsl::string_view&  baseName);
#endif
        // Load into the specified 'pipeName' the system-dependent canonical
        // pipe name corresponding to the specified 'baseName'.  Return 0 on
        // success, and a nonzero value if 'baseName' cannot be part of a pipe
        // name on this system.
        //
        // On Unix systems, the canonical name is defined by prefixing
        // 'baseName' with the directory specified by the 'SOCKDIR' environment
        // variable if it is set, otherwise with the directory specified by the
        // 'TMPDIR' environment variable if it is set, and otherwise by the
        // current directory.
        //
        // On Windows systems, the canonical name is defined by prefixing
        // 'baseName' with "\\.\pipe\".
        //
        // Finally, any uppercase characters in 'baseName' are converted to
        // lower case in the canonical name.

    static int send(const bsl::string_view& pipeName,
                    const bsl::string_view& message);
        // Send the specified 'message' to the pipe with the specified UTF-8
        // 'pipeName'.  Return 0 on success, and a nonzero value otherwise.
        // 'message is output in a single 'write' operation; consequently,
        // messages that do not exceed the 'PIPE_BUF' value (see {Pipe
        // Atomicity}) will not be interleaved even when multiple concurrent
        // processes are writing to 'pipeName'.  The behavior is undefined
        // unless 'pipeName' is a valid UTF-8 string.

    static bool isOpenForReading(const bsl::string_view& pipeName);
        // Return 'true' if the pipe with the specified UTF-8 'pipeName'
        // exists, the calling process has permission to write to it, and some
        // process is able to read the bytes written to it, and 'false'
        // otherwise.  On Windows, this function may block and may write an
        // "empty message", consisting of a single newline.  The behavior is
        // undefined unless 'pipeName' is a valid UTF-8 string.  Note that even
        // though a process might have the pipe open for reading, this function
        // might still return 'false' because there are not sufficient
        // resources available.
};

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
