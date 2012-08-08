// bdeut_stringref.cpp      -*-C++-*-
#include <bdeut_stringref.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bdeut_stringref_cpp,"$Id$ $CSID$")


#include <bsl_algorithm.h>
#include <bsl_cstddef.h>
#include <bsl_ostream.h>
#include <bsl_string.h>

namespace BloombergLP {

                        // ---------------------
                        // class bdeut_StringRef
                        // ---------------------

// FREE OPERATORS
bool operator<(const bdeut_StringRef& lhs, const bdeut_StringRef& rhs)
{
    return bsl::lexicographical_compare(lhs.begin(), lhs.end(),
                                        rhs.begin(), rhs.end());
}

bool operator>(const bdeut_StringRef& lhs, const bdeut_StringRef& rhs)
{
    return bsl::lexicographical_compare(rhs.begin(), rhs.end(),
                                        lhs.begin(), lhs.end());
}

bsl::string operator+(const bdeut_StringRef& lhs, const bdeut_StringRef& rhs)
{
    bsl::string result;

    result.reserve(lhs.length() + rhs.length());
    result.assign(lhs.begin(), lhs.end());
    result.append(rhs.begin(), rhs.end());

    return result;
}

bsl::ostream& operator<<(bsl::ostream&          stream,
                         const bdeut_StringRef& stringRef)
{
    // Unbound 'StringRef's can be streamed, so test for them.

    if (stringRef.isBound()) {
        stream.write(stringRef.data(), stringRef.length());
    }

    return stream;
}

}  // close namespace BloombergLP

                        // ----------------------------
                        // struct hash<bdeut_StringRef>
                        // ----------------------------

namespace bsl {

// ACCESSORS
bsl::size_t hash<BloombergLP::bdeut_StringRef>::operator()(
                           const BloombergLP::bdeut_StringRef& stringRef) const
{
    const char *string       = stringRef.data();
    bsl::size_t stringLength = stringRef.length();

    // The following implementation was cloned from bdeu_hashutil.cpp, but
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

}  // close namespace bsl

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2004
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
