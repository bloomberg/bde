// baljsn_decoder.cpp                                                 -*-C++-*-
#include <baljsn_decoder.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(baljsn_decoder_cpp,"$Id$ $CSID$")

#include <bsl_iterator.h>

namespace BloombergLP {

namespace baljsn {
                   // --------------------
                   // class Decoder
                   // --------------------

// PRIVATE MANIPULATORS
int Decoder::skipUnknownElement(const bslstl::StringRef& elementName)
{
    int rc = d_tokenizer.advanceToNextToken();
    if (rc) {
        d_logStream << "Error advancing to token after '"
                    << elementName << "'\n";
        return -1;                                                    // RETURN
    }

    if (Tokenizer::BAEJSN_ELEMENT_VALUE == d_tokenizer.tokenType()) {
        // 'elementName' is a simple type.  Extract its value and return.

        bslstl::StringRef tmp;
        rc = d_tokenizer.value(&tmp);
        if (rc) {
            d_logStream << "Error reading attribute value for "
                        << elementName << "'\n";
        }
        return rc;                                                    // RETURN
    }
    else if (Tokenizer::BAEJSN_START_OBJECT ==
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
              case Tokenizer::BAEJSN_ELEMENT_NAME:
              case Tokenizer::BAEJSN_ELEMENT_VALUE: {    // FALL THROUGH
                bslstl::StringRef tmp;
                rc = d_tokenizer.value(&tmp);
                if (rc) {
                    d_logStream << "Error reading attribute name after '{'\n";
                    return -1;                                        // RETURN
                }
              } break;

              case Tokenizer::BAEJSN_START_OBJECT: {
                if (++d_currentDepth > d_maxDepth) {
                    d_logStream << "Maximum allowed decoding depth reached: "
                                << d_currentDepth << "\n";
                    return -1;                                        // RETURN
                }
                ++skippingDepth;
              } break;

              case Tokenizer::BAEJSN_END_OBJECT: {
                --d_currentDepth;
                --skippingDepth;
              } break;

              default: {
              } break;
            }
        }
    }
    else if (Tokenizer::BAEJSN_START_ARRAY == d_tokenizer.tokenType()) {
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
              case Tokenizer::BAEJSN_ELEMENT_NAME:
              case Tokenizer::BAEJSN_ELEMENT_VALUE: {    // FALL THROUGH
                bslstl::StringRef tmp;
                rc = d_tokenizer.value(&tmp);
                if (rc) {
                    d_logStream << "Error reading attribute name after '{'\n";
                    return -1;                                        // RETURN
                }
              } break;

              case Tokenizer::BAEJSN_START_OBJECT: {
                if (++d_currentDepth > d_maxDepth) {
                    d_logStream << "Maximum allowed decoding depth reached: "
                                << d_currentDepth << "\n";
                    return -1;                                        // RETURN
                }
              } break;

              case Tokenizer::BAEJSN_END_OBJECT: {
                --d_currentDepth;
              } break;

              case Tokenizer::BAEJSN_START_ARRAY: {
                ++skippingDepth;
              } break;

              case Tokenizer::BAEJSN_END_ARRAY: {
                --skippingDepth;
              } break;

              default: {
              } break;
            }
        }
    }

    return 0;
}
}  // close package namespace

}  // close namespace BloombergLP

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2012
//      All Rights Reserved.
//      Property of Bloomberg L.P.  (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ----------------------------------
