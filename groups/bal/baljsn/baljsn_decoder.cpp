// baljsn_decoder.cpp                                                 -*-C++-*-
#include <baljsn_decoder.h>

#include <baljsn_encoder.h>         // for testing only
#include <baljsn_encoderoptions.h>  // for testing only
#include <baljsn_printutil.h>       // for testing only

#include <bsls_ident.h>

BSLS_IDENT_RCSID(baljsn_decoder_cpp,"$Id$ $CSID$")

#include <bdlde_utf8util.h>

#include <bsla_fallthrough.h>

#include <bsl_iterator.h>

namespace BloombergLP {
namespace baljsn {

                               // -------------
                               // class Decoder
                               // -------------

// PRIVATE MANIPULATORS
bsl::ostream& Decoder::logTokenizerError(const char *alternateString)
{
    const int sts = d_tokenizer.readStatus();

    if (0 == sts) {
        d_logStream << alternateString;
    }
    else if (sts < 0) {
        d_logStream << "UTF-8 error " << bdlde::Utf8Util::toAscii(sts)
                    << " at offset " << d_tokenizer.currentPosition();
    }
    else {
        d_logStream << "Error: unexpected end of file at offset "
                    << d_tokenizer.currentPosition();
    }

    return d_logStream;
}

int Decoder::skipUnknownElement(const bsl::string_view& elementName)
{
    int rc = d_tokenizer.advanceToNextToken();
    if (rc) {
        logTokenizerError("Error") << " advancing to token after '"
                                   << elementName << "'\n";
        return -1;                                                    // RETURN
    }

    if (Tokenizer::e_ELEMENT_VALUE == d_tokenizer.tokenType()) {
        // 'elementName' is a simple type.  Extract its value and return.

        bslstl::StringRef tmp;
        rc = d_tokenizer.value(&tmp);
        if (rc) {
            d_logStream << "Error reading attribute value for "
                        << elementName << "'\n";
            return rc;                                                // RETURN
        }
    }
    else if (Tokenizer::e_START_OBJECT == d_tokenizer.tokenType()) {
        // 'elementName' is a sequence or choice.  Descend into the element and
        // skip all its sub-elements.

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
                logTokenizerError("Error") << " reading unknown element '"
                            << elementName << "' or after that element\n";
                return -1;                                            // RETURN
            }

            switch (d_tokenizer.tokenType()) {
              case Tokenizer::e_ELEMENT_NAME:                 BSLA_FALLTHROUGH;
              case Tokenizer::e_ELEMENT_VALUE: {
                bslstl::StringRef tmp;
                rc = d_tokenizer.value(&tmp);
                if (rc) {
                    d_logStream << "Error reading attribute name after '{'\n";
                    return -1;                                        // RETURN
                }
              } break;

              case Tokenizer::e_START_OBJECT: {
                if (++d_currentDepth > d_maxDepth) {
                    d_logStream << "Maximum allowed decoding depth reached: "
                                << d_currentDepth << "\n";
                    return -1;                                        // RETURN
                }
                ++skippingDepth;
              } break;

              case Tokenizer::e_END_OBJECT: {
                --d_currentDepth;
                --skippingDepth;
              } break;

              default: {
              } break;
            }
        }
    }
    else if (Tokenizer::e_START_ARRAY == d_tokenizer.tokenType()) {
        // 'elementName' is an array.  Descend into the array element till we
        // encounter the matching end array token (']').

        int skippingDepth = 1;

        // Since we skip all the data associated with this element we can relax
        // our restrictions on array homogeneity.  See {DRQS 171296111} for
        // more details.
        d_tokenizer.setAllowHeterogenousArrays(true);

        while (skippingDepth) {
            // Use 'skippingDepth' to keep track of how we have descended and
            // when to return.

            int rc = d_tokenizer.advanceToNextToken();
            if (rc) {
                logTokenizerError("Error") << " reading unknown element '"
                            << elementName << "' or after that element\n";
                return -1;                                            // RETURN
            }

            switch (d_tokenizer.tokenType()) {
              case Tokenizer::e_ELEMENT_NAME:                 BSLA_FALLTHROUGH;
              case Tokenizer::e_ELEMENT_VALUE: {
                bslstl::StringRef tmp;
                rc = d_tokenizer.value(&tmp);
                if (rc) {
                    d_logStream << "Error reading attribute name after '{'\n";
                    return -1;                                        // RETURN
                }
              } break;

              case Tokenizer::e_START_OBJECT: {
                if (++d_currentDepth > d_maxDepth) {
                    d_logStream << "Maximum allowed decoding depth reached: "
                                << d_currentDepth << "\n";
                    return -1;                                        // RETURN
                }
              } break;

              case Tokenizer::e_END_OBJECT: {
                --d_currentDepth;
              } break;

              case Tokenizer::e_START_ARRAY: {
                ++skippingDepth;
              } break;

              case Tokenizer::e_END_ARRAY: {
                --skippingDepth;
              } break;

              default: {
              } break;
            }
        }
        d_tokenizer.setAllowHeterogenousArrays(false);
    }

    d_numUnknownElementsSkipped++;
    return 0;
}
}  // close package namespace

}  // close enterprise namespace

// ----------------------------------------------------------------------------
// Copyright 2015 Bloomberg Finance L.P.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// ----------------------------- END-OF-FILE ----------------------------------
