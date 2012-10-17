// baejsn_decoder.cpp                                                 -*-C++-*-
#include <baejsn_decoder.h>
#include <bcemt_once.h>
#include <bdem_elemtype.h>
#include <bdede_base64decoder.h>

namespace BloombergLP {

namespace {

// BDEM_DATETIME
// BDEM_DATE
// BDEM_TIME
// BDEM_TABLE    // todo need this, its basically an array of sequences
// BDEM_DATETIMETZ
// BDEM_DATETZ
// BDEM_TIMETZ
// BDEM_ROW
// BDEM_CHOICE_ARRAY_ITEM

}

// PRIVATE MANIPULATORS
bsl::ostream& baejsn_Decoder::logStream()
{
    if (0 == d_logStream) {
        d_logStream = new(d_logArea.buffer()) bsl::ostringstream;
    }
    return *d_logStream;
}


int baejsn_Decoder::decodeArray(std::vector<char> *value)
{
    baejsn_ParserUtil::skipSpaces(d_streamBuf);

    std::string hexString;
    if (0 != baejsn_ParserUtil::getString(d_streamBuf, &hexString)) {
        BAEJSN_THROW(mobcmn::ExBadArg, "Expected string containing base64");
        return 1;
    }

    if (0 != (hexString.length() & 0x03)) {
        BAEJSN_THROW(mobcmn::ExBadArg, "Expected string containing base64, but string length is not a multiple of 4");
        return 1;
    }

    bdede_Base64Decoder decoder(true);
    value->resize((hexString.length() * 3 + 3) / 4);
    int numOut;
    int numIn;
    int rc = decoder.convert(value->begin(), &numOut, &numIn, hexString.begin(), hexString.end());
    value->resize(numOut);
    return rc;
}

bsl::string baejsn_Decoder::loggedMessages() const
{
    if (d_logStream) {
        return d_logStream->str();
    }
    return bsl::string();
}

}  // close namespace BloombergLP

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2012
//      All Rights Reserved.
//      Property of Bloomberg L.P.  (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ----------------------------------
