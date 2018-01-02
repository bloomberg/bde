// bslstl_ostringstream.cpp                                           -*-C++-*-
#include <bslstl_ostringstream.h>

#include <bsls_ident.h>
BSLS_IDENT("$Id$ $CSID$")

namespace bsl {

// Force instantiation of 'ostringstream' classes.

template class basic_ostringstream<
    char, char_traits<char>, allocator<char> >;

template class basic_ostringstream<
    wchar_t, char_traits<wchar_t>, allocator<wchar_t> >;

}  // close namespace bsl

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
