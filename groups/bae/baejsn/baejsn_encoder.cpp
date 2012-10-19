// baejsn_encoder.cpp                                                 -*-C++-*-
#include <baejsn_encoder.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(baetzo_localtimedescriptor_cpp,"$Id$ $CSID$")

#include <bdede_base64encoder.h>

namespace BloombergLP {

namespace {

char getEscapeChar(char value) {
    switch (value) {
      case '"':
      case '\\':
      case '/':
        return value;
      case '\b':
        return 'b';
      case '\f':
        return 'f';
      case '\n':
        return 'n';
      case '\r':
        return 'r';
      case '\t':
        return 't';
      default: {
          if (value < 32) {
              // control characters
              return 'u';
          }
      }
    }
    return 0;
}

}  // close anonymous namespace

                   // ----------------------------------
                   // class baejsn_Encoder::MemOutStream
                   // ----------------------------------

baejsn_Encoder::MemOutStream::~MemOutStream()
{
}

                            // --------------------
                            // class baejsn_Encoder
                            // --------------------

// PRIVATE MANIPULATORS
bsl::ostream& baejsn_Encoder::logStream()
{
    if (0 == d_logStream) {
        d_logStream = new(d_logArea.buffer()) MemOutStream(d_allocator);
    }
    return *d_logStream;
}

// CREATORS
baejsn_Encoder::baejsn_Encoder(bslma::Allocator *basicAllocator)
: d_allocator(bslma_Default::allocator(basicAllocator))
, d_logStream(0)
{
}

baejsn_Encoder::~baejsn_Encoder()
{
    if (d_logStream != 0) {
        d_logStream->~MemOutStream();
    }
}

                            // -------------------------------
                            // class baejsn_Encoder_EncodeImpl
                            // -------------------------------

int baejsn_Encoder_EncodeImpl::encodeSimple(const char value)
{
    bsl::string str = "\"";

    char ch = getEscapeChar(value);
    if (0 != ch) {
        str += '\\';
        str += ch;
        if ('u' == ch) {
            str += "00";
            bsl::ostringstream oss;
            oss << bsl::hex << bsl::setfill('0') << bsl::setw(2)
                << (static_cast<unsigned int>(value) & 0xff);
            str += oss.str();
        }
    }
    else {
        str += value;
    }
    str += '"';

    outputStream() << str;
    return 0;
}

int baejsn_Encoder_EncodeImpl::encodeSimple(const bsl::string & value)
{
    outputStream() << '"';

    for (bsl::string::const_iterator it = value.begin();
         it != value.end();
         ++it)
    {
        bsl::string str;

        char ch = getEscapeChar(*it);
        if (0 != ch) {
            str += '\\';
            str += ch;
            if ('u' == ch) {
                bsl::string::const_iterator next = it;
                ++next;

                bsl::ostringstream oss;

                if (*it == 0 || next == value.end())
                {
                    oss << "00"
                        << bsl::hex << bsl::setfill('0') << bsl::setw(2)
                        << (static_cast<unsigned int>(*it) & 0xff);
                }
                else
                {
                    oss << bsl::hex << bsl::setfill('0') << bsl::setw(2)
                        << (static_cast<unsigned int>(*it) & 0xff)
                        << bsl::hex << bsl::setfill('0') << bsl::setw(2)
                        << (static_cast<unsigned int>(*next) & 0xff);
                    ++it;
                }
                str += oss.str();
            }
        }
        else {
            str += *it;
        }

        outputStream() << str;
    }

    outputStream() << '"';
    return 0;
}

int baejsn_Encoder_EncodeImpl::encodeArray(const bsl::vector<char>& value)
{
    bsl::string base64String;
    bdede_Base64Encoder encoder(0);
    base64String.resize(
                      bdede_Base64Encoder::encodedLength(value.size(), 0));
    int numOut;
    int numIn;
    int rc = encoder.convert(base64String.begin(),
                             &numOut,
                             &numIn,
                             value.begin(),
                             value.end());

    if (rc < 0) {
        return rc;
    }
    rc = encoder.endConvert(base64String.begin() + numOut);

    return encodeSimple(base64String);
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
