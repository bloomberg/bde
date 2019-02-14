// bslstl_systemerror.cpp                                             -*-C++-*-
#include <bslstl_systemerror.h>

#include <bsls_ident.h>
BSLS_IDENT("$Id$ $CSID$")

#ifndef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY

                             // ------------------
                             // class system_error
                             // ------------------

// CREATORS
bsl::system_error::system_error(error_code code, const std::string& what)
: std::runtime_error(what + std::string(": ") + code.message())
, d_code(code)
{
}

bsl::system_error::system_error(error_code code, const char *what)
: std::runtime_error(what + std::string(": ") + code.message())
, d_code(code)
{
}

bsl::system_error::system_error(error_code code)
: std::runtime_error(code.message())
, d_code(code)
{
}

bsl::system_error::system_error(int                   value,
                                const error_category& category,
                                const std::string&    what)
: std::runtime_error(what + std::string(": ") + category.message(value))
, d_code(value, category)
{
}

bsl::system_error::system_error(int                    value,
                                const error_category&  category,
                                const char            *what)
: std::runtime_error(what + std::string(": ") + category.message(value))
, d_code(value, category)
{
}

bsl::system_error::system_error(int value, const error_category& category)
: std::runtime_error(category.message(value))
, d_code(value, category)
{
}

// ACCESSORS
const bsl::error_code& bsl::system_error::code() const
{
    return d_code;
}

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
