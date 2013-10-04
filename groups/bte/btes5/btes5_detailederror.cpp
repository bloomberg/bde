// btes5_detailederror.cpp                                            -*-C++-*-
#include <btes5_detailederror.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(btes5_detailederror_cpp, "$Id$ $CSID$")

#include <bdes_ident.h>
BDES_IDENT_RCSID(btes5_detailederror_cpp, "$Id$ $CSID$")

namespace BloombergLP {

                        // -------------------------
                        // class btes5_DetailedError
                        // -------------------------

// CREATORS
btes5_DetailedError::btes5_DetailedError(const bslstl::StringRef&  description,
                                         const bteso_Endpoint&     address,
                                         bslma::Allocator         *allocator)
: d_description(description, allocator)
, d_address(address, allocator)
{
}

btes5_DetailedError::btes5_DetailedError(const bslstl::StringRef&  description,
                                         bslma::Allocator         *allocator)
: d_description(description, allocator)
, d_address(allocator)
{
}

btes5_DetailedError::btes5_DetailedError(
    const btes5_DetailedError&  original,
    bslma::Allocator           *allocator)
: d_description(original.d_description, allocator)
, d_address(original.d_address, allocator)
{
}

// MANIPULATORS
void btes5_DetailedError::setDescription(const bslstl::StringRef& value)
{
    d_description = value;
}

void btes5_DetailedError::setAddress(const bteso_Endpoint& value)
{
    d_address = value;
}

// ACCESSORS
const bsl::string& btes5_DetailedError::description() const
{
    return d_description;
}

const bteso_Endpoint& btes5_DetailedError::address() const
{
    return d_address;
}

// FREE OPERATORS
bool operator==(const btes5_DetailedError& lhs,
                const btes5_DetailedError& rhs)
{
    return lhs.description() == rhs.description()
            && lhs.address() == rhs.address();
}

bool operator!=(const btes5_DetailedError& lhs,
                const btes5_DetailedError& rhs)
{
    return lhs.description() != rhs.description()
            || lhs.address() != rhs.address();
}

bsl::ostream& operator<<(bsl::ostream&              output,
                         const btes5_DetailedError& error)
{
    output << error.description();
    if (error.address().port()) {
        output << " (" << error.address() << ")";
    }
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
