// btls5_credentials.cpp                                              -*-C++-*-
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

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2015
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
