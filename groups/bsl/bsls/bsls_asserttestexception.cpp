// bsls_asserttestexception.cpp                                       -*-C++-*-
#include <bsls_asserttestexception.h>

#include <bsls_ident.h>
BSLS_IDENT("$Id$ $CSID$")

#include <bsls_bsltestutil.h>        // for testing only

namespace {

// Allocate and return a copy of the string denoted by the optionally specified
// `data`, returning `NULL` if `data` is `NULL`.
const char* copyString(const char* data)
{
    if (NULL == data) {
        return NULL;
    } else {
        std::size_t len = std::strlen(data);
        char* output = new char[len+1];
        std::strncpy(output, data, len+1);
        return output;
    }
}

// Free a string that was previously allocated by `copyString`.
void freeString(const char* data)
{
    if (NULL != data) {
        delete[] data;
    }
}

}

namespace BloombergLP {
namespace bsls {
                     // ================================
                     // class AssertTestException_String
                     // ================================

// CREATORS
AssertTestException_String::AssertTestException_String(const char* data)
: d_data(copyString(data))
{}

AssertTestException_String::AssertTestException_String(
    const AssertTestException_String& original)
: d_data(copyString(original.d_data))
{}

#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
AssertTestException_String::AssertTestException_String(
    AssertTestException_String&& original)
: d_data(original.d_data)
{
    original.d_data = NULL;
}
#endif

AssertTestException_String::~AssertTestException_String()
{
    freeString(d_data);
}

// ACCESSORS
const char* AssertTestException_String::data() const
{
    return static_cast<const char*>(d_data);
}


}  // close package namespace
}  // close enterprise namespace

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
