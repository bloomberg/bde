// bslstl_error.cpp                                                   -*-C++-*-
#include <bslstl_error.h>

#include <bsls_ident.h>
BSLS_IDENT("$Id$ $CSID$")

#ifndef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY

namespace bsl {
namespace {

                        // ===========================
                        // class generic_category_impl
                        // ===========================

class generic_category_impl : public error_category {
    // The single object of this class represents the generic category.

    // ACCESSORS
    native_std::string message(int value) const BSLS_KEYWORD_OVERRIDE;
        // Return a string describing the specified 'value'.

    const char *name() const BSLS_KEYWORD_NOEXCEPT BSLS_KEYWORD_OVERRIDE;
        // Return a string describing this category.
};

                        // ---------------------------
                        // class generic_category_impl
                        // ---------------------------

// ACCESSORS
native_std::string generic_category_impl::message(int value) const
{
    return error_category::message(value);
}

const char *generic_category_impl::name() const BSLS_KEYWORD_NOEXCEPT
{
    return "generic";
}

}  // close unnamed namespace

// FREE FUNCTIONS
const error_category& generic_category()
{
    static generic_category_impl generic_category_object;
    return generic_category_object;
}

namespace {

                         // ==========================
                         // class system_category_impl
                         // ==========================

class system_category_impl : public error_category {
    // The single object of this class represents the system category.

    // ACCESSORS
    native_std::string message(int value) const BSLS_KEYWORD_OVERRIDE;
        // Return a string describing the specified 'value'.

    const char *name() const BSLS_KEYWORD_NOEXCEPT BSLS_KEYWORD_OVERRIDE;
        // Return a string describing this category.
};

// ACCESSORS
native_std::string system_category_impl::message(int value) const
{
    return error_category::message(value);
}

const char *system_category_impl::name() const BSLS_KEYWORD_NOEXCEPT
{
    return "system";
}

}  // close unnamed namespace

// FREE FUNCTIONS
const error_category& system_category()
{
    static system_category_impl system_category_object;
    return system_category_object;
}

}  // close namespace bsl

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
