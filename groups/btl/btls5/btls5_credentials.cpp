// btls5_credentials.cpp                                              -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <btls5_credentials.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(btls5_credentials_cpp, "$Id$ $CSID$")

#include <bslim_printer.h>
#include <bsls_assert.h>

#include <bsl_ostream.h>

namespace BloombergLP {

namespace btls5 {

namespace {

static bool areValidCredentials(const bslstl::StringRef& username,
                                const bslstl::StringRef& password)
    // Return 'true' if the specified 'username' and 'password' are both empty
    // or if both of their lengths are from 1 to 255 inclusive; and return
    // 'false' otherwise.
{
    if (username.isEmpty() && password.isEmpty()) {
        return true;                                                  // RETURN
    }
    else if (1 <= username.length() && username.length() <= 255
          && 1 <= password.length() && password.length() <= 255) {
        return true;                                                  // RETURN
    }
    return false;
}

}  // close unnamed namespace

                             // -----------------
                             // class Credentials
                             // -----------------

// MANIPULATORS
void Credentials::set(const bslstl::StringRef& username,
                      const bslstl::StringRef& password)
{
    BSLS_ASSERT(areValidCredentials(username, password));

    d_username.assign(username.begin(), username.end());
    d_password.assign(password.begin(), password.end());
}

// ACCESSORS

                                  // Aspects

bsl::ostream& Credentials::print(bsl::ostream& stream,
                                 int           level,
                                 int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("username", d_username);
    printer.printAttribute("password", d_password);
    printer.end();

    return stream;
}

}  // close package namespace

// FREE OPERATORS
bsl::ostream& btls5::operator<<(bsl::ostream&      stream,
                                const Credentials& object)
{
    bslim::Printer printer(&stream, 0, -1);
    printer.start();
    printer.printValue(object.username());
    printer.printValue(object.password());
    printer.end();

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
