// bdls_tempdirectoryguard.h                                          -*-C++-*-
#ifndef INCLUDED_BDLS_TEMPDIRECTORYGUARD
#define INCLUDED_BDLS_TEMPDIRECTORYGUARD

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a scoped guard that creates a unique temporary directory.
//
//@CLASSES:
//  bdls::TempDirectoryGuard: guard for creating and removing a temp directory
//
//@DESCRIPTION: This component provides a scoped guard
// `bdls::TempDirectoryGuard`, intended primarily for testing, which creates a
// uniquely named temporary directory.  If possible, this is located in the
// system temp directory, otherwise it is created within the current working
// directory.
//
// As this class is primarily intended for testing, any failures to build the
// directory name or create the temporary directory will be fatal.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Basic Usage
/// - - - - - - - - - - -
// Suppose an algorithm requires writing data to a temporary file on disk
// during processing:
// ```
// /// Do "algorithm" using the specified `testFileName` for intermidiate state
// /// storage.
// void testAlgorithm(const bsl::string &testFileName);
// ```
// A function looking to use this algorithm can obtain a directory in which to
// put this file, guaranteed to not be used by other processes and to be
// cleaned up on normal exit, using an instance of `bdls::TempDirectoryGuard`:
// ```
// void usesTestAlgorithm()
// {
//     bdls::TempDirectoryGuard tempDirGuard("my_algo_");
//
//     bsl::string tmpFileName(tempDirGuard.getTempDirName());
//     bdls::PathUtil::appendRaw(&tmpFileName,"algorithm.scratch");
//
//     testAlgorithm(tmpFileName);
// }
// ```
// After exiting, the scratch file (named "algorithm.scratch") and the
// temporary directory (with an unspecified name starting with "my_algo_"),
// possibly in the system temp directory or the current working directory, will
// be removed.

#include <bslma_allocator.h>
#include <bslma_usesbslmaallocator.h>

#include <bslmf_nestedtraitdeclaration.h>

#include <bsl_string.h>

namespace BloombergLP {
namespace bdls {

                              // ========================
                              // class TempDirectoryGuard
                              // ========================

/// This class implements a scoped temporary directory guard.  The guard tries
/// to create a temporary directory in the system-wide temp directory and falls
/// back to the current directory.
class TempDirectoryGuard {

    // DATA
    bsl::string       d_dirName;      // path to the created directory
    bslma::Allocator *d_allocator_p;  // memory allocator (held, not owned)

    // NOT IMPLEMENTED
    TempDirectoryGuard(const TempDirectoryGuard&);
    TempDirectoryGuard& operator=(const TempDirectoryGuard&);

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(TempDirectoryGuard,
                                   bslma::UsesBslmaAllocator);

    // CREATORS

    /// Create temporary directory with the specified `prefix` in the
    /// system-wide temp or current directory.  Optionally specify a
    /// `basicAllocator` used to supply memory.  If `basicAllocator` is 0, the
    /// currently installed default allocator is used.
    explicit TempDirectoryGuard(const bsl::string&  prefix,
                                bslma::Allocator   *basicAllocator = 0);

    /// Destroy this object and remove the temporary directory (recursively)
    /// created at construction.
    ~TempDirectoryGuard();

    // ACCESSORS

    /// Return a `const` reference to the name of the created temporary
    /// directory.
    const bsl::string& getTempDirName() const;
};

}  // close package namespace
}  // close enterprise namespace
#endif

// ----------------------------------------------------------------------------
// Copyright 2019 Bloomberg Finance L.P.
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
