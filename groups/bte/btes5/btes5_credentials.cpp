// btes5_credentials.cpp                                          -*-C++-*-
#include <btes5_credentials.h>

#include <bsl_ostream.h>
#include <bsls_assert.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(btes5_credentials, "$Id$ $CSID$")

namespace BloombergLP {

                         // ---------------------------
                         // class btes5_Credentials
                         // ---------------------------

// CREATORS
btes5_Credentials::btes5_Credentials(bslma::Allocator *allocator)
: d_username(allocator)
, d_password(allocator)
{
    // 'd_username.length() > 0' is the internal indicator for 'this->isSet()'.
}

btes5_Credentials::btes5_Credentials(
    const btes5_Credentials&  original,
    bslma::Allocator         *allocator)
: d_username(original.d_username, allocator)
, d_password(original.d_password, allocator)
{
}

btes5_Credentials::btes5_Credentials(
    const bslstl::StringRef&  username,
    const bslstl::StringRef&  password,
    bslma::Allocator         *allocator)
: d_username(allocator)
, d_password(allocator)
{
    set(username, password);
}

// MANIPULATORS
void btes5_Credentials::set(const bslstl::StringRef& username,
                            const bslstl::StringRef& password)
{
    BSLS_ASSERT(0 < username.length() && username.length() <= 255);
    BSLS_ASSERT(0 < password.length() && password.length() <= 255);
    d_username.assign(username.begin(), username.end());
    d_password.assign(password.begin(), password.end());
}

void btes5_Credentials::reset()
{
    d_username.clear();
    d_password.clear();
}

// ACCESSORS
bool btes5_Credentials::isSet() const
{
    return d_username.length() > 0;
}

const bsl::string& btes5_Credentials::username() const
{
    return d_username;
}

const bsl::string& btes5_Credentials::password() const
{
    return d_password;
}

// FREE OPERATORS
bool operator==(const btes5_Credentials& lhs,
                const btes5_Credentials& rhs)
{
    return lhs.username() == rhs.username()
        && lhs.password() == rhs.password();
}

bool operator!=(const btes5_Credentials& lhs,
                const btes5_Credentials& rhs)
{
    return lhs.username() != rhs.username()
        || lhs.password() != rhs.password();
}

bsl::ostream& operator<<(bsl::ostream&                output,
                         const btes5_Credentials& object)
{
    output << object.username() << ':' << object.password();
    return output;
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
