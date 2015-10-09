// balxml_reader.cpp                                                  -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <balxml_reader.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(balxml_reader_cpp,"$Id$ $CSID$")

#ifndef INCLUDED_BALXML_ELEMENTATTRIBUTE
#include <balxml_elementattribute.h>
#endif

#include <bsl_string.h>

namespace BloombergLP {

namespace balxml {
                                // ------------
                                // class Reader
                                // ------------

// PUBLIC CREATORS
Reader::~Reader(void)
{
}

// CLASS METHODS
const char *
Reader::nodeTypeAsString(NodeType nodeType)
{
    switch (nodeType) {
      case e_NODE_TYPE_NONE :
        return "NODE_TYPE_NONE";
      case e_NODE_TYPE_ELEMENT :
        return "NODE_TYPE_ELEMENT";
      case e_NODE_TYPE_TEXT :
        return "NODE_TYPE_TEXT";
      case e_NODE_TYPE_CDATA :
        return "NODE_TYPE_CDATA";
      case e_NODE_TYPE_ENTITY_REFERENCE :
        return "NODE_TYPE_ENTITY_REFERENCE";
      case e_NODE_TYPE_ENTITY :
        return "NODE_TYPE_ENTITY";
      case e_NODE_TYPE_PROCESSING_INSTRUCTION:
        return "NODE_TYPE_PROCESSING_INSTRUCTION";
      case e_NODE_TYPE_COMMENT :
        return "NODE_TYPE_COMMENT";
      case e_NODE_TYPE_DOCUMENT :
        return "NODE_TYPE_DOCUMENT";
      case e_NODE_TYPE_DOCUMENT_TYPE :
        return "NODE_TYPE_DOCUMENT_TYPE";
      case e_NODE_TYPE_DOCUMENT_FRAGMENT :
        return "NODE_TYPE_DOCUMENT_FRAGMENT";
      case e_NODE_TYPE_NOTATION :
        return "NODE_TYPE_NOTATION";
      case e_NODE_TYPE_WHITESPACE :
        return "NODE_TYPE_WHITESPACE";
      case e_NODE_TYPE_SIGNIFICANT_WHITESPACE:
        return "NODE_TYPE_SIGNIFICANT_WHITESPACE";
      case e_NODE_TYPE_END_ELEMENT :
        return "NODE_TYPE_END_ELEMENT";
      case e_NODE_TYPE_END_ENTITY :
        return "NODE_TYPE_END_ENTITY";
      case e_NODE_TYPE_XML_DECLARATION :
        return "NODE_TYPE_XML_DECLARATION";
    }
    return "(* UNKNOWN NODE TYPE *)";
}
}  // close package namespace

// NON-VIRTUAL ACCESSORS (implemented in this base class)
#define CHK(X) (X != 0 ? (const char *) X : "(null)")

namespace balxml {
void
Reader::dumpNode(bsl::ostream & os) const
{
    const char *name  = nodeName();
    const char *value = nodeValue();
    const char *nsUri = nodeNamespaceUri();

    int      line   = getLineNumber();
    int      column = getColumnNumber();
    int      depth  = nodeDepth();
    NodeType type   = nodeType();

    bsl::string strPad((bsl::string::size_type)depth*2, ' ');

    os << strPad
       << "Node pos="  << line  << ":" << column
       << " type=" << type
       << "(" << nodeTypeAsString(type)
       << ") empty=" << isEmptyElement()
       << " hasValue=" << nodeHasValue()
       << " name=" << CHK(name)
       << " value=" <<  CHK(value)
       << " uri=" << CHK(nsUri)
       << bsl::endl;

    int numAttr  = numAttributes();

    for (int i = 0; i < numAttr; ++i)
    {
        ElementAttribute attr;
        lookupAttribute(&attr, i);

        os << strPad
           << "  ATTRIBUTE  "
           << CHK(attr.qualifiedName())
           << "="
           << CHK(attr.value())
           << " uri="
           << CHK(attr.namespaceUri())
           << bsl::endl;
    }
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
