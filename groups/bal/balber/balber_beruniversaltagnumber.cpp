// balber_beruniversaltagnumber.cpp                                     -*-C++-*-
#include <balber_beruniversaltagnumber.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(balber_beruniversaltagnumber_cpp,"$Id$ $CSID$")

#include <bdlb_string.h>

namespace BloombergLP {

namespace balber {
                     // ---------------------------------
                     // struct BerUniversalTagNumber
                     // ---------------------------------

// CLASS METHODS
const char *BerUniversalTagNumber::toString(
                                       BerUniversalTagNumber::Value value)
{
#ifdef CASE
#undef CASE
#endif

#define CASE(VALUE, STRING) case(VALUE): return #STRING;

    switch (value) {
      CASE(e_BER_BOOL,           BOOL)
      CASE(e_BER_INT,            INT)
      CASE(e_BER_OCTET_STRING,   OCTET_STRING)
      CASE(e_BER_REAL,           REAL)
      CASE(e_BER_ENUMERATION,    ENUMERATION)
      CASE(e_BER_UTF8_STRING,    UTF8_STRING)
      CASE(e_BER_SEQUENCE,       SEQUENCE)
      CASE(e_BER_VISIBLE_STRING, VISIBLE_STRING)
      default: {
      } break;
    }

    return "(* UNKNOWN *)";
}

int BerUniversalTagNumber::fromString(
                               BerUniversalTagNumber::Value *result,
                               const char                        *string,
                               int                                stringLength)
{
    enum { BDEM_SUCCESS = 0, BDEM_NOT_FOUND = 1 };

#ifdef CHECK
#undef CHECK
#endif

#define CHECK(STRING, VALUE)                                                 \
        if (bdlb::String::areEqualCaseless(string, stringLength, #STRING)) {  \
            *result = VALUE;                                                 \
            return BDEM_SUCCESS;                                             \
        }

    CHECK(BOOL,           e_BER_BOOL)
    CHECK(INT,            e_BER_INT)
    CHECK(OCTET_STRING,   e_BER_OCTET_STRING)
    CHECK(REAL,           e_BER_REAL)
    CHECK(ENUMERATION,    e_BER_ENUMERATION)
    CHECK(UTF8_STRING,    e_BER_UTF8_STRING)
    CHECK(SEQUENCE,       e_BER_SEQUENCE)
    CHECK(VISIBLE_STRING, e_BER_VISIBLE_STRING)

    return BDEM_NOT_FOUND;
}
}  // close package namespace

}  // close namespace BloombergLP;

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2005
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
