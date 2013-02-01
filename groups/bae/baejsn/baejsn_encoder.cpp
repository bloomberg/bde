// baejsn_encoder.cpp                                                 -*-C++-*-
#include <baejsn_encoder.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(baejsn_encoder_cpp,"$Id$ $CSID$")

#include <bdede_base64encoder.h>

namespace BloombergLP {

                        // ------------------------------
                        // class baejsn_Encoder_Formatter
                        // ------------------------------

// CREATORS
baejsn_Encoder_Formatter::baejsn_Encoder_Formatter(
                                          bsl::ostream&                stream,
                                          const baejsn_EncoderOptions& options)
: d_outputStream(stream)
, d_isArrayElement(false)
{
    if (baejsn_EncoderOptions::BAEJSN_PRETTY == options.encodingStyle()) {
        d_usePrettyStyle = true;
        d_indentLevel    = options.initialIndentLevel();
        d_spacesPerLevel = options.spacesPerLevel();
    }
    else {
        d_usePrettyStyle = false;
        d_indentLevel    = 0;
        d_spacesPerLevel = 0;
    }

    if (d_usePrettyStyle) {
        bdeu_Print::indent(d_outputStream, d_indentLevel, d_spacesPerLevel);
    }
}

// MANIPULATORS
void baejsn_Encoder_Formatter::openObject()
{
    indent();

    d_outputStream << '{';

    if (d_usePrettyStyle) {
        d_outputStream << '\n';
    }

    ++d_indentLevel;
}

void baejsn_Encoder_Formatter::closeObject()
{
    --d_indentLevel;

    if (d_usePrettyStyle) {
        d_outputStream << '\n';
        bdeu_Print::indent(d_outputStream, d_indentLevel, d_spacesPerLevel);
    }

    d_outputStream << '}';
}

void baejsn_Encoder_Formatter::openArray()
{
    d_outputStream << '[';

    if (d_usePrettyStyle) {
        d_outputStream << '\n';
    }

    ++d_indentLevel;
}

void baejsn_Encoder_Formatter::closeArray()
{
    --d_indentLevel;

    if (d_usePrettyStyle) {
        d_outputStream << '\n';
        bdeu_Print::indent(d_outputStream, d_indentLevel, d_spacesPerLevel);
    }

    d_outputStream << ']';

}

void baejsn_Encoder_Formatter::indent()
{
    if (d_usePrettyStyle) {
        if (d_isArrayElement) {
            bdeu_Print::indent(d_outputStream,
                               d_indentLevel,
                               d_spacesPerLevel);
        }
    }
}

int baejsn_Encoder_Formatter::openElement(const bsl::string& name)
{
    if (d_usePrettyStyle) {
        bdeu_Print::indent(d_outputStream, d_indentLevel, d_spacesPerLevel);
    }

    const int rc = baejsn_PrintUtil::printValue(d_outputStream, name);
    if (rc) {
        return rc;                                                    // RETURN
    }

    if (d_usePrettyStyle) {
        d_outputStream << " : ";
    }
    else {
        d_outputStream << ':';
    }

    return 0;
}

void baejsn_Encoder_Formatter::closeElement()
{
    d_outputStream << ',';
    if (d_usePrettyStyle) {
        d_outputStream << '\n';
    }
}

                            // -------------------------------
                            // class baejsn_Encoder_EncodeImpl
                            // -------------------------------

// PRIVATE MANIPULATORS
int baejsn_Encoder_EncodeImpl::encodeImp(const bsl::vector<char>& value,
                                         int                      mode,
                                         bdeat_TypeCategory::Array)
{
    bsl::string base64String;
    bdede_Base64Encoder encoder(0);
    base64String.resize(
        bdede_Base64Encoder::encodedLength(static_cast<int>(value.size()), 0));

    // Ensure length is a multiple of 4.

    BSLS_ASSERT(0 == (base64String.length() & 0x03));

    int numOut;
    int numIn;
    int rc = encoder.convert(base64String.begin(),
                             &numOut,
                             &numIn,
                             value.begin(),
                             value.end());

    if (rc < 0) {
        return rc;                                                    // RETURN
    }

    rc = encoder.endConvert(base64String.begin() + numOut);
    if (rc < 0) {
        return rc;                                                    // RETURN
    }

    return encode(base64String, 0);
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
