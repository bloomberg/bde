// balxml_util.cpp                                                    -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <balxml_util.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(balxml_util_cpp,"$Id$ $CSID$")

#include <bsls_assert.h>

#include <bsl_iosfwd.h>
#include <bsl_sstream.h>

namespace BloombergLP  {

namespace balxml {
                                // -----------
                                // struct Util
                                // -----------

// CLASS METHODS
bool
Util::extractNamespaceFromXsd(const bsl::string&  xsdSource,
                              bsl::string        *targetNamespace)
{
    // Note that a valid XSD file must have this attribute for the root element
    // '<schema>' and the only place that the token "targetNamespace" can
    // appear prior to its occurrence as the required attribute is in the
    // comment string(s) before '<schema>'.  And any comments cannot be
    // embedded in other comments or inside a tag.  The extraction algorithm
    // relies on these XML facts to jump over any comments prior to the first
    // occurrence of "targetNamespace" and extract the attribute value.  It
    // leaves all other validation of 'xsdSource' to a parser.

    BSLS_ASSERT(targetNamespace != 0);

    typedef bsl::string::size_type size_type;

    size_type lookHere             = 0;
    size_type startTargetNamespace = bsl::string::npos;

    for (;;) {
        // Find "targetNamespace" token.
        startTargetNamespace = xsdSource.find("targetNamespace", lookHere);

        if (bsl::string::npos != startTargetNamespace) {
            // Find whether there is a '<!-- comment -->' before this
            // "targetNamespace".

            size_type startOpenComment = xsdSource.rfind("<!--",
                                                         startTargetNamespace);

            if (bsl::string::npos != startOpenComment) {
                // There is a '<!--'; find whether there is '-->' before this
                // "targetNamespace".
                size_type startCloseComment = xsdSource.find(
                                                         "-->",
                                                         startOpenComment + 4);
                                                        // 4 is strlen("<!--")
                if (bsl::string::npos == startCloseComment) {
                    // no ending comment, the doc is not formatted right
                    startTargetNamespace = bsl::string::npos;
                    break;
                }
                else if (startCloseComment >= startTargetNamespace + 15) {
                    // "targetNamespace" is enclosed within '<!-- comment -->'.
                    // 'bsl::strlen("targetNamespace") == 15' So look for next
                    // "targetNamespace".
                    lookHere = startCloseComment + 3; // 3 is strlen("-->")
                }
                else {
                    BSLS_ASSERT(startCloseComment + 3 <= startTargetNamespace);
                    // Found the "targetNamespace" we are looking for.
                    break;
                }
            }
            else {
                // There is no '<!--' before this "targetNamespace", so found
                // the "targetNamespace" we are looking for.
                break;
            }
        }
        else {     // can not find more token of "targetNamespace"
            break;
        }
    }

    // By now, 'startTargetNamespace' is determined.
    if (bsl::string::npos != startTargetNamespace) {
        // Find whatever content is in the quotation after "targetNamespace".
        size_type startNamespace = xsdSource.find_first_of(
                                                    "'\"",
                                                    startTargetNamespace + 16);
                            // bsl::strlen("targetNamespace=") == 16
        if (bsl::string::npos != startNamespace) {
            char quote = xsdSource[startNamespace];
            size_type endNamespace = xsdSource.find(quote, startNamespace + 1);
            if (bsl::string::npos != endNamespace) {
                targetNamespace->append(xsdSource.substr(
                                           startNamespace + 1,
                                           endNamespace - startNamespace - 1));
                return true;                                          // RETURN
            }  // did not find the close quote after the open quote
        }      // did not find the open quote after "targetNamespace"
    }          // did not find "targetNamespace" as the attribute

    return false;
}

bool
Util::extractNamespaceFromXsd(bsl::streambuf *xsdSource,
                              bsl::string    *targetNamespace)
{

    bsl::streambuf::pos_type pos = xsdSource->pubseekoff(0,
                                                         bsl::ios_base::cur,
                                                         bsl::ios::in);

    bsl::ostringstream oss;

    oss << xsdSource;

    bool rc = extractNamespaceFromXsd(oss.str(), targetNamespace);

    xsdSource->pubseekpos(pos, bsl::ios::in);

    return rc;
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
