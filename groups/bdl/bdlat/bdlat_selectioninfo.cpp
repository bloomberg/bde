// bdlat_selectioninfo.cpp                                            -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <bdlat_selectioninfo.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlat_selectioninfo_cpp,"$Id$ $CSID$")

#include <bsl_ostream.h>

namespace BloombergLP {

                         // --------------------------
                         // struct bdlat_SelectionInfo
                         // --------------------------

// FREE OPERATORS
bsl::ostream& operator<<(bsl::ostream&              stream,
                         const bdlat_SelectionInfo& selectionInfo)
{
    stream << "( " << selectionInfo.formattingMode()
           << ", " << selectionInfo.id()
           << ", ";

    const int nameLen = selectionInfo.nameLength();
    if (nameLen) {
        stream.write(selectionInfo.name(), nameLen);
    }
    else {
        stream << "<null-name>";
    }

    stream << ", " << nameLen
           << ", ";

    const char *annotation = selectionInfo.annotation();
    if (annotation) {
        stream << annotation;
    }
    else {
        stream << "<null-annotation>";
    }

    stream << " )";

    return stream;
}

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
