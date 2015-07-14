// bdlb_xxxstringref.cpp                                                -*-C++-*-
#include <bdlb_xxxstringref.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlb_xxxstringref_cpp,"$Id$ $CSID$")

#include <bsl_algorithm.h>

namespace BloombergLP {

                        // ---------------------
                        // class bdlb::StringRef
                        // ---------------------

// FREE OPERATORS
bool bdlb::operator<(const StringRef& lhs, const StringRef& rhs)
{
    return bsl::lexicographical_compare(lhs.begin(), lhs.end(),
                                        rhs.begin(), rhs.end());
}

bool bdlb::operator>(const StringRef& lhs, const StringRef& rhs)
{
    return bsl::lexicographical_compare(rhs.begin(), rhs.end(),
                                        lhs.begin(), lhs.end());
}

bsl::string bdlb::operator+(const StringRef& lhs, const StringRef& rhs)
{
    bsl::string result;

    result.reserve(lhs.length() + rhs.length());
    result.assign(lhs.begin(), lhs.end());
    result.append(rhs.begin(), rhs.end());

    return result;
}

bsl::ostream& bdlb::operator<<(bsl::ostream&          stream,
                         const StringRef& stringRef)
{
    // Unbound 'StringRef's can be streamed, so test for them.

    if (stringRef.isBound()) {
        stream.write(stringRef.data(), stringRef.length());
    }

    return stream;
}

}  // close namespace BloombergLP

                        // ----------------------------
                        // struct hash<bdlb::StringRef>
                        // ----------------------------

namespace bsl {

// ACCESSORS
bsl::size_t hash<BloombergLP::bdlb::StringRef>::operator()(
                           const BloombergLP::bdlb::StringRef& stringRef) const
{

    const char *string       = stringRef.data();
    bsl::size_t stringLength = stringRef.length();

    // The following implementation was cloned from bdlb_hashutil.cpp, but
    // without the unneeded modulo operation.

    const unsigned int ADDEND       = 1013904223U;
    const unsigned int MULTIPLICAND =    1664525U;
    const unsigned int MASK         = 4294967295U;

    const char *end = string + stringLength;
    bsl::size_t r   = 0;

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

}  // close bsl namespace

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2004
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ----------------------------------
