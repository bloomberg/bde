// btes5_usercredentials.cpp                                          -*-C++-*-
#include <btes5_usercredentials.h>

#include <bsl_ostream.h>
#include <bsls_assert.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(btes5_usercredentials, "$Id$ $CSID$")

namespace BloombergLP {

                         // ---------------------------
                         // class btes5_UserCredentials
                         // ---------------------------

// CREATORS
btes5_UserCredentials::btes5_UserCredentials(bslma_Allocator *allocator)
: d_username(bslma::Default::allocator(allocator))
, d_password(bslma::Default::allocator(allocator))
{
    // 'd_username.length() > 0' is the internal indicator for 'this->isSet()'.
}

btes5_UserCredentials::btes5_UserCredentials(
    const btes5_UserCredentials&  original,
    bslma::Allocator             *allocator)
: d_username(original.d_username, bslma::Default::allocator(allocator))
, d_password(original.d_password, bslma::Default::allocator(allocator))
{
}

btes5_UserCredentials::btes5_UserCredentials(
    const bslstl::StringRef&  username,
    const bslstl::StringRef&  password,
    bslma::Allocator         *allocator)
: d_username(bslma::Default::allocator(allocator))
, d_password(bslma::Default::allocator(allocator))
{
    set(username, password);
}

btes5_UserCredentials::~btes5_UserCredentials()
{
}

// MANIPULATORS
void btes5_UserCredentials::set(const bslstl::StringRef& username,
                                const bslstl::StringRef& password)
{
    BSLS_ASSERT(0 < username.length() && username.length() <= 255);
    BSLS_ASSERT(0 < password.length() && password.length() <= 255);
    d_username = username;
    d_password = password;
}

// ACCESSORS
bool btes5_UserCredentials::isSet() const
{
    return d_username.length() > 0;
}

const bsl::string& btes5_UserCredentials::username() const
{
    return d_username;
}

const bsl::string& btes5_UserCredentials::password() const
{
    return d_password;
}

// FREE OPERATORS
bool operator==(const btes5_UserCredentials& lhs,
                const btes5_UserCredentials& rhs)
{
    return lhs.username() == rhs.username()
        && lhs.password() == rhs.password();
}

bool operator!=(const btes5_UserCredentials& lhs,
                const btes5_UserCredentials& rhs)
{
    return lhs.username() != rhs.username()
        || lhs.password() != rhs.password();
}

bsl::ostream& operator<<(bsl::ostream&                output,
                         const btes5_UserCredentials& object)
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
