// bdls_tempdirectoryguard.cpp                                        -*-C++-*-
#include <bdls_tempdirectoryguard.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdls_tempdirectoryguard_cpp, "$Id$ $CSID$")

#include <bdls_filesystemutil.h>
#include <bdls_pathutil.h>

#include <bsls_assert.h>

namespace BloombergLP {
namespace bdls {

TempDirectoryGuard::TempDirectoryGuard(const bsl::string&  prefix,
                                       bslma::Allocator   *basicAllocator)
: d_dirName(bslma::Default::allocator(basicAllocator))
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    bsl::string tmpPath(d_allocator_p);

    int rc = FilesystemUtil::getSystemTemporaryDirectory(&tmpPath);
    if (0 != rc) {
        // use path relative to current directory
        tmpPath = "";
    }

    rc = bdls::PathUtil::appendIfValid(&tmpPath, prefix);
    if (0 != rc) {
        BSLS_ASSERT_INVOKE("Unable to form directory root name");
    }

    rc = bdls::FilesystemUtil::createTemporaryDirectory(&d_dirName, tmpPath);
    if (0 != rc) {
        BSLS_ASSERT_INVOKE("Unable to create temporary directory");
    }
}

TempDirectoryGuard::~TempDirectoryGuard()
{
    bdls::FilesystemUtil::remove(d_dirName, true);
}

// ACCESSORS
const bsl::string& TempDirectoryGuard::getTempDirName() const
{
    return d_dirName;
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
