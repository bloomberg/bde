// balcl_optioninfo.cpp                                               -*-C++-*-
#include <balcl_optioninfo.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(balcl_optioninfo_cpp,"$Id$ $CSID$")

#include <bsl_ostream.h>

namespace BloombergLP {

                     // -----------------
                     // struct OptionInfo
                     // -----------------

// FREE OPERATORS
bsl::ostream& balcl::operator<<(bsl::ostream& stream, const OptionInfo& rhs)
{
    stream << "{" << '\n';
    if (rhs.d_tag.empty()) {
        stream << "    NON_OPTION" << '\n';
    } else {
        stream << "    TAG             \"" << rhs.d_tag << "\"" << '\n';
    }
    stream << "    NAME            \""     << rhs.d_name << "\"" << '\n';
    stream << "    DESCRIPTION     \""     << rhs.d_description << "\""
                                                                << '\n';
    stream << "    TYPE_INFO       ";   rhs.d_typeInfo.print(stream, -1);
                                        stream << '\n';
    stream << "    OCCURRENCE_INFO ";   rhs.d_defaultInfo.print(stream, -1);
                                        stream << '\n';
    stream << "}";
    return stream;
}

}  // close enterprise namespace

// ----------------------------------------------------------------------------
// Copyright 2020 Bloomberg Finance L.P.
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
