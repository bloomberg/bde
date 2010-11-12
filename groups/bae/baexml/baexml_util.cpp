// baexml_util.cpp              -*-C++-*-
#include <baexml_util.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(baexml_util_cpp,"$Id$ $CSID$")

#include <bsls_assert.h>

#include <bsl_iosfwd.h>
#include <bsl_sstream.h>

namespace BloombergLP  {

                        // ------------------
                        // struct baexml_Util
                        // ------------------

// CLASS METHODS
bool
baexml_Util::extractNamespaceFromXsd(const bsl::string&  xsdSource,
                                     bsl::string        *targetNs)
{
    // Note that a valid XSD file must have this attribute for the root element
    // '<schema>' and the only place that the token "targetNamespace" can
    // appear prior to its occurrence as the required attribute is in the
    // comment string(s) before '<schema>'.  And any comments cannot be
    // embedded in other comments or inside a tag.  The extraction algorithm
    // relies on these XML facts to jump over any comments prior to the first
    // occurrence of "targetNamespace" and extract the attribute value.  It
    // leaves all other validation of 'xsdSource' to a parser.

    BSLS_ASSERT(targetNs != 0);

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
                    // bsl::strlen("targetNamespace") == 15
                    // "targetNamespace" is enclosed within '<!-- comment -->'.
                    // So look for next "targetNamespace".
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
                targetNs->append(xsdSource.substr(
                                           startNamespace + 1,
                                           endNamespace - startNamespace - 1));
                return true;
            }  // did not find the close quote after the open quote
        }      // did not find the open quote after "targetNamespace"
    }          // did not find "targetNamespace" as the attribute

    return false;
}

bool
baexml_Util::extractNamespaceFromXsd(bsl::streambuf *xsdSource,
                                     bsl::string    *targetNs)
{

    bsl::streambuf::pos_type pos = xsdSource->pubseekoff(0,
                                                         bsl::ios_base::cur,
                                                         bsl::ios::in );

    bsl::ostringstream oss;

    oss << xsdSource;

    bool rc = extractNamespaceFromXsd(oss.str(), targetNs);

    xsdSource->pubseekpos(pos, bsl::ios::in);

    return rc;
}

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2007
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
