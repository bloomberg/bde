// bdls_filedescriptorguard.cpp                                       -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <bdls_filedescriptorguard.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdls_filedescriptorguard_cpp,"$Id$ $CSID$")

namespace BloombergLP {

namespace bdls {
// MANIPULATORS
void FileDescriptorGuard::closeAndRelease()
{
    BSLS_ASSERT(FilesystemUtil::k_INVALID_FD != d_descriptor);

    int rc = FilesystemUtil::close(d_descriptor);
    BSLS_ASSERT(0 == rc && "close failed");
    (void) rc;    // suppress unuwed warning in opt build

    d_descriptor = FilesystemUtil::k_INVALID_FD;
}
}  // close package namespace

}  // close enterprise namespace

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
