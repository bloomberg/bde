// btes5_credentials.cpp                                              -*-C++-*-
#include <btes5_credentials.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(btes5_credentials_cpp, "$Id$ $CSID$")

#include <bslim_printer.h>
#include <bsls_assert.h>

#include <bsl_ostream.h>

namespace BloombergLP {

                         // -----------------------
                         // class btes5_Credentials
                         // -----------------------

// MANIPULATORS
void btes5_Credentials::set(const bslstl::StringRef& username,
                            const bslstl::StringRef& password)
{
    BSLS_ASSERT(0 < username.length() && username.length() <= 255);
    BSLS_ASSERT(0 < password.length() && password.length() <= 255);

    d_username.assign(username.begin(), username.end());
    d_password.assign(password.begin(), password.end());
}

// ACCESSORS

                                  // Aspects

bsl::ostream& btes5_Credentials::print(bsl::ostream& stream,
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

// FREE OPERATORS
bsl::ostream& operator<<(bsl::ostream&            stream,
                         const btes5_Credentials& object)
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
//      Copyright (C) Bloomberg L.P., 2013
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
