// baejsn_decoder.cpp                                                 -*-C++-*-
#include <baejsn_decoder.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(baejsn_decoder_cpp,"$Id$ $CSID$")

#include <bdema_bufferedsequentialallocator.h>
#include <bdede_base64decoder.h>

#include <bsl_iterator.h>

namespace BloombergLP {

                   // --------------------
                   // class baejsn_Decoder
                   // --------------------

// PRIVATE MANIPULATORS
int baejsn_Decoder::decodeBinaryArray(bsl::vector<char> *value)
{
    if (baejsn_Tokenizer::BAEJSN_ELEMENT_VALUE == d_tokenizer.tokenType()) {
        bslstl::StringRef dataValue;
        int rc = d_tokenizer.value(&dataValue);

        const int MAX_LENGTH = 255;
        char      buffer[MAX_LENGTH + 1];

        bdema_BufferedSequentialAllocator allocator(buffer, MAX_LENGTH + 1);
        bsl::string base64String(&allocator);

        rc = baejsn_TokenizerUtil::getValue(&base64String, dataValue);

        bdede_Base64Decoder base64Decoder(true);
        bsl::back_insert_iterator<bsl::vector<char> > outputIterator(*value);

        rc = base64Decoder.convert(outputIterator,
                                   base64String.begin(),
                                   base64String.end());

        if (rc) {
            return -1;                                                // RETURN
        }

        rc = base64Decoder.endConvert(outputIterator);

        if (rc) {
            return -1;                                                // RETURN
        }
    }
    return 0;
}

int baejsn_Decoder::skipUnknownElement(const bslstl::StringRef& elementName)
{
    int rc = d_tokenizer.advanceToNextToken();
    if (rc) {
        d_logStream << "Error advancing to token after '"
                    << elementName << "'\n";
        return -1;                                                    // RETURN
    }

    if (baejsn_Tokenizer::BAEJSN_ELEMENT_VALUE == d_tokenizer.tokenType()) {
        // 'elementName' is a simple type.  Extract its value and return.

        bslstl::StringRef tmp;
        rc = d_tokenizer.value(&tmp);
        if (rc) {
            d_logStream << "Error reading attribute value for "
                        << elementName << "'\n";
        }
        return rc;                                                    // RETURN
    }
    else if (baejsn_Tokenizer::BAEJSN_START_OBJECT ==
                                                     d_tokenizer.tokenType()) {
        // 'elementName' is a sequence or choice.  Descend into the element
        // and skip all its sub-elements.

        if (++d_currentDepth > d_maxDepth) {
            d_logStream << "Maximum allowed decoding depth reached: "
                        << d_currentDepth << "\n";
            return -1;                                                // RETURN
        }

        int skippingDepth = 1;
        while (skippingDepth) {
            // Use 'skippingDepth' to keep track of how we have descended and
            // when to return.

            int rc = d_tokenizer.advanceToNextToken();
            if (rc) {
                d_logStream << "Error reading unknown element '"
                            << elementName << "' or after that element\n";
                return -1;                                            // RETURN
            }

            switch (d_tokenizer.tokenType()) {
              case baejsn_Tokenizer::BAEJSN_ELEMENT_NAME:
              case baejsn_Tokenizer::BAEJSN_ELEMENT_VALUE: {    // FALL THROUGH
                bslstl::StringRef tmp;
                rc = d_tokenizer.value(&tmp);
                if (rc) {
                    d_logStream << "Error reading attribute name after '{'\n";
                    return -1;                                        // RETURN
                }
              } break;

              case baejsn_Tokenizer::BAEJSN_START_OBJECT: {
                if (++d_currentDepth > d_maxDepth) {
                    d_logStream << "Maximum allowed decoding depth reached: "
                                << d_currentDepth << "\n";
                    return -1;                                        // RETURN
                }
                ++skippingDepth;
              } break;

              case baejsn_Tokenizer::BAEJSN_END_OBJECT: {
                --d_currentDepth;
                --skippingDepth;
              } break;

              default: {
              } break;
            }
        }
    }
    else if (baejsn_Tokenizer::BAEJSN_START_ARRAY == d_tokenizer.tokenType()) {
        // 'elementName' is an array.  Descend into the array element till we
        // encounter the matching end array token (']').

        int skippingDepth = 1;
        while (skippingDepth) {
            // Use 'skippingDepth' to keep track of how we have descended and
            // when to return.

            int rc = d_tokenizer.advanceToNextToken();
            if (rc) {
                d_logStream << "Error reading unknown element '"
                            << elementName << "' or after that element\n";
                return -1;                                            // RETURN
            }

            switch (d_tokenizer.tokenType()) {
              case baejsn_Tokenizer::BAEJSN_ELEMENT_NAME:
              case baejsn_Tokenizer::BAEJSN_ELEMENT_VALUE: {    // FALL THROUGH
                bslstl::StringRef tmp;
                rc = d_tokenizer.value(&tmp);
                if (rc) {
                    d_logStream << "Error reading attribute name after '{'\n";
                    return -1;                                        // RETURN
                }
              } break;

              case baejsn_Tokenizer::BAEJSN_START_OBJECT: {
                if (++d_currentDepth > d_maxDepth) {
                    d_logStream << "Maximum allowed decoding depth reached: "
                                << d_currentDepth << "\n";
                    return -1;                                        // RETURN
                }
              } break;

              case baejsn_Tokenizer::BAEJSN_END_OBJECT: {
                --d_currentDepth;
              } break;

              case baejsn_Tokenizer::BAEJSN_START_ARRAY: {
                ++skippingDepth;
              } break;

              case baejsn_Tokenizer::BAEJSN_END_ARRAY: {
                --skippingDepth;
              } break;

              default: {
              } break;
            }
        }
    }

    return 0;
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
