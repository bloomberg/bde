// bslstl_systemerror.cpp                                             -*-C++-*-
#include <bslstl_systemerror.h>

#include <bsls_ident.h>
BSLS_IDENT("$Id$ $CSID$")

                            // --------------------
                            // class error_category
                            // --------------------

//CREATORS
bsl::error_category::error_category()
{
}

bsl::error_category::~error_category()
{
}

// ACCESSORS
bsl::error_condition bsl::error_category::default_error_condition(
                                                               int value) const
{
    return error_condition(value, *this);
}

bool bsl::error_category::equivalent(int                    code,
                                     const error_condition& condition) const
{
    return default_error_condition(code) == condition;
}

bool bsl::error_category::equivalent(const error_code& code,
                                     int               condition) const
{
    return *this == code.category() && code.value() == condition;
}

std::string bsl::error_category::message(int value) const
{
    return std::strerror(value);
}

const char *bsl::error_category::name() const
{
    return "error_category";
}

bool bsl::error_category::operator==(const error_category& other) const
{
    return this == &other;
}

bool bsl::error_category::operator!=(const error_category& other) const
{
    return !(*this == other);
}

bool bsl::error_category::operator<(const error_category& other) const
{
    return std::less<const error_category *>()(this, &other);
}

                              // ----------------
                              // class error_code
                              // ----------------

// CREATORS
bsl::error_code::error_code()
: d_value(0)
, d_category_p(&system_category())
{
}

bsl::error_code::error_code(int value, const error_category& category)
: d_value(value)
, d_category_p(&category)
{
}

// MANIPULATORS
void bsl::error_code::assign(int value, const error_category& category)
{
    d_value = value;
    d_category_p = &category;
}

void bsl::error_code::clear()
{
    d_value = 0;
    d_category_p = &system_category();
}

// ACCESSORS
const bsl::error_category& bsl::error_code::category() const
{
    return *d_category_p;
}

bsl::error_condition bsl::error_code::default_error_condition() const
{
    return category().default_error_condition(value());
}

std::string bsl::error_code::message() const
{
    return category().message(value());
}

int bsl::error_code::value() const
{
    return d_value;
}

bsl::error_code::operator BoolType() const
{
    return UnspecifiedBool::makeValue(value());
}

// FREE FUNCTIONS
bsl::error_code bsl::make_error_code(errc::errc value)
{
    return error_code(static_cast<int>(value), generic_category());
}

                           // ---------------------
                           // class error_condition
                           // ---------------------

// CREATORS
bsl::error_condition::error_condition()
: d_value(0)
, d_category_p(&generic_category())
{
}

bsl::error_condition::error_condition(int                   value,
                                      const error_category& category)
: d_value(value)
, d_category_p(&category)
{
}

// MANIPULATORS
void bsl::error_condition::assign(int value, const error_category& category)
{
    d_value = value;
    d_category_p = &category;
}

void bsl::error_condition::clear()
{
    d_value = 0;
    d_category_p = &generic_category();
}

// ACCESSORS
const bsl::error_category& bsl::error_condition::category() const
{
    return *d_category_p;
}

std::string bsl::error_condition::message() const
{
    return category().message(value());
}

int bsl::error_condition::value() const
{
    return d_value;
}

bsl::error_condition::operator BoolType() const
{
    return UnspecifiedBool::makeValue(value());
}

// FREE FUNCTIONS
bsl::error_condition bsl::make_error_condition(errc::errc value)
{
    return error_condition(static_cast<int>(value), generic_category());
}

// FREE OPERATORS
bool bsl::operator==(const error_code& lhs, const error_code& rhs)
{
    return lhs.category() == rhs.category() && lhs.value() == rhs.value();
}

bool bsl::operator==(const error_code& lhs, const error_condition& rhs)
{
    return lhs.category().equivalent(lhs.value(), rhs) ||
           rhs.category().equivalent(lhs, rhs.value());
}

bool bsl::operator==(const error_condition& lhs, const error_code& rhs)
{
    return rhs.category().equivalent(rhs.value(), lhs) ||
           lhs.category().equivalent(rhs, lhs.value());
}

bool bsl::operator==(const error_condition& lhs, const error_condition& rhs)
{
    return lhs.category() == rhs.category() && lhs.value() == rhs.value();
}

bool bsl::operator!=(const error_code& lhs, const error_code& rhs)
{
    return !(lhs == rhs);
}

bool bsl::operator!=(const error_code& lhs, const error_condition& rhs)
{
    return !(lhs == rhs);
}

bool bsl::operator!=(const error_condition& lhs, const error_code& rhs)
{
    return !(lhs == rhs);
}

bool bsl::operator!=(const error_condition& lhs, const error_condition& rhs)
{
    return !(lhs == rhs);
}

bool bsl::operator<(const error_code& lhs, const error_code& rhs)
{
    return lhs.category() < rhs.category() ||
           (lhs.category() == rhs.category() && lhs.value() < rhs.value());
}

bool bsl::operator<(const error_condition& lhs, const error_condition& rhs)
{
    return lhs.category() < rhs.category() ||
           (lhs.category() == rhs.category() && lhs.value() < rhs.value());
}

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

namespace {

                        // ===========================
                        // class generic_category_impl
                        // ===========================

class generic_category_impl : public bsl::error_category {
    // The single object of this class represents the generic category.

    // ACCESSORS
    std::string message(int value) const;
        // Return a string describing the specified 'value'.

    const char *name() const;
        // Return a string describing this category.
};

                        // ---------------------------
                        // class generic_category_impl
                        // ---------------------------

// ACCESSORS
std::string generic_category_impl::message(int value) const
{
    return bsl::error_category::message(value);
}

const char *generic_category_impl::name() const
{
    return "generic";
}

}  // close unnamed namespace

// FREE FUNCTIONS
const bsl::error_category& bsl::generic_category()
{
    static generic_category_impl generic_category_object;
    return generic_category_object;
}

namespace {

                         // ==========================
                         // class system_category_impl
                         // ==========================

class system_category_impl : public bsl::error_category {
    // The single object of this class represents the system category.

    // ACCESSORS
    std::string message(int value) const;
        // Return a string describing the specified 'value'.

    const char *name() const;
        // Return a string describing this category.
};

// ACCESSORS
std::string system_category_impl::message(int value) const
{
    return bsl::error_category::message(value);
}

const char *system_category_impl::name() const
{
    return "system";
}

}  // close unnamed namespace

// FREE FUNCTIONS
const bsl::error_category& bsl::system_category()
{
    static system_category_impl system_category_object;
    return system_category_object;
}

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
