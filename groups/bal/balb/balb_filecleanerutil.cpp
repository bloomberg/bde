// balb_filecleanerutil.cpp                                           -*-C++-*-
#include <balb_filecleanerutil.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(balb_filecleanerutil_cpp,"$Id$ $CSID$")

#include <bdlma_localsequentialallocator.h>

#include <bdls_filesystemutil.h>

#include <bdlt_currenttime.h>
#include <bdlt_datetime.h>
#include <bdlt_epochutil.h>

#include <bsls_timeinterval.h>

#include <bsl_cstddef.h>
#include <bsl_functional.h>
#include <bsl_iterator.h>
#include <bsl_map.h>
#include <bsl_string.h>
#include <bsl_vector.h>

namespace BloombergLP {
namespace balb {

                           // ----------------------
                           // struct FileCleanerUtil
                           // ----------------------

// CLASS METHODS
void FileCleanerUtil::removeFiles(const FileCleanerConfiguration& config)
{
    typedef bsl::multimap<bdlt::Datetime, bsl::string> RemoveMap;
    typedef bsl::vector<bsl::string>                   FileVec;
    typedef RemoveMap::const_iterator                  MCIter;
    typedef FileVec::const_iterator                    VCIter;
    typedef bsl::ptrdiff_t                             ptrdiff_t;

    const bdlt::Datetime thresholdDatetime =
        bdlt::EpochUtil::convertFromTimeInterval(
                               bdlt::CurrentTime::now() - config.maxFileAge());

    bdlma::LocalSequentialAllocator<4 * 1024> localAllocator;

    FileVec files(&localAllocator);
    bdls::FilesystemUtil::findMatchingPaths(&files,
                                            config.filePattern().data());

    if (files.empty()) {
        return;                                                       // RETURN
    }

    bdlt::Datetime modificationTime;
    RemoveMap      removeMap(&localAllocator);

    for (VCIter it = files.cbegin(); files.cend() != it; ++it) {
        if (bdls::FilesystemUtil::isRegularFile(*it) &&
            0 == bdls::FilesystemUtil::getLastModificationTime(
                                                             &modificationTime,
                                                             *it)) {
            removeMap.emplace(modificationTime, *it);
        }
    }

    // Iterate through the files, oldest first.

    ptrdiff_t       numFiles    = removeMap.size();
    const ptrdiff_t minNumFiles = config.minNumFiles();
    const ptrdiff_t maxNumFiles = config.maxNumFiles();

    BSLS_ASSERT_OPT(0 <= minNumFiles);
    BSLS_ASSERT_OPT(minNumFiles <= maxNumFiles);

    for (MCIter it = removeMap.cbegin();
         removeMap.cend() != it && (maxNumFiles < numFiles ||
                    (minNumFiles < numFiles && it->first < thresholdDatetime));
         ++it, --numFiles) {
        bdls::FilesystemUtil::remove(it->second);
    }
}

}  // close package namespace
}  // close enterprise namespace

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
