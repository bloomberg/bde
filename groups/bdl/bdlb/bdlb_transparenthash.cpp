// bdlb_transparenthash.cpp                                           -*-C++-*-
#include <bdlb_transparenthash.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlb_transparenthash_cpp,"$Id$ $CSID$")

namespace BloombergLP {
namespace bdlb {

std::size_t TransparentStringHash::operator()(const char *str) const
{
    bsl::string_view sv(str);
    return this->operator()(sv);
}

std::size_t TransparentStringHash::operator()(const bsl::string& str) const
{
    return bsl::hash<bsl::string>().operator()(str);
}

std::size_t TransparentStringHash::operator()(
                                             const bsl::string_view& str) const
{
    return bsl::hash<bsl::string_view>().operator()(str);
}



}  // close package namespace
}  // close enterprise namespace

// ----------------------------------------------------------------------------
// Copyright 2021 Bloomberg Finance L.P.
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
