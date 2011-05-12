// bslstl_stringargument.cpp                                          -*-C++-*-
#include <bslstl_stringargument.h>

#include <bsls_ident.h>
BSLS_IDENT("$Id$ $CSID$")

#include <algorithm>
#include <ostream>

namespace BloombergLP {

                        // ---------------------------
                        // class bslstl_StringArgument
                        // ---------------------------

// FREE OPERATORS
bool operator<(const bslstl_StringArgument& lhs,
               const bslstl_StringArgument& rhs)
{
    return std::lexicographical_compare(lhs.begin(), lhs.end(),
                                        rhs.begin(), rhs.end());
}

bool operator>(const bslstl_StringArgument& lhs,
               const bslstl_StringArgument& rhs)
{
    return std::lexicographical_compare(rhs.begin(), rhs.end(),
                                        lhs.begin(), lhs.end());
}

bsl::string operator+(const bslstl_StringArgument& lhs,
                      const bslstl_StringArgument& rhs)
{
    bsl::string result;

    result.reserve(lhs.length() + rhs.length());
    result.assign(lhs.begin(), lhs.end());
    result.append(rhs.begin(), rhs.end());

    return result;
}

std::ostream& operator<<(std::ostream&                stream,
                         const bslstl_StringArgument& stringArg)
{
    return stream.write(stringArg.data(), stringArg.length());
}

}  // close namespace BloombergLP

                        // ----------------------------------
                        // struct hash<bslstl_StringArgument>
                        // ----------------------------------

namespace bsl {

// ACCESSORS
std::size_t hash<BloombergLP::bslstl_StringArgument>::operator()(
                     const BloombergLP::bslstl_StringArgument& stringArg) const
{
    const char *string       = stringArg.data();
    std::size_t stringLength = stringArg.length();

    // The following implementation was cloned from bdeu_hashutil.cpp, but
    // without the unneeded modulo operation.

    const unsigned int ADDEND       = 1013904223U;
    const unsigned int MULTIPLICAND =    1664525U;
    const unsigned int MASK         = 4294967295U;

    const char *end = string + stringLength;
    std::size_t r   = 0;

    if (4 == sizeof(int)) {
        while (string != end) {
            r ^= *string++;
            r = r * MULTIPLICAND + ADDEND;
        }
    }
    else {
        while (string != end) {
            r ^= *string++;
            r = (r * MULTIPLICAND + ADDEND) & MASK;
        }
    }

    return r;
}

}  // close namespace bsl

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2011
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
