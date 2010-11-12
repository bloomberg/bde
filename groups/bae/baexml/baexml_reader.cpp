// baexml_reader.cpp              -*-C++-*-
#include <baexml_reader.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(baexml_reader_cpp,"$Id$ $CSID$")

#ifndef INCLUDED_BAEXML_ELEMENTATTRIBUTE
#include <baexml_elementattribute.h>
#endif

namespace BloombergLP {

                        // -------------------
                        // class baexml_Reader
                        // -------------------

// PUBLIC CREATORS
baexml_Reader::~baexml_Reader(void)
{
}

// CLASS METHODS
const char *
baexml_Reader::nodeTypeAsString(NodeType nodeType)
{
    switch (nodeType) {
      case BAEXML_NODE_TYPE_NONE :
        return "NODE_TYPE_NONE";
      case BAEXML_NODE_TYPE_ELEMENT :
        return "NODE_TYPE_ELEMENT";
      case BAEXML_NODE_TYPE_TEXT :
        return "NODE_TYPE_TEXT";
      case BAEXML_NODE_TYPE_CDATA :
        return "NODE_TYPE_CDATA";
      case BAEXML_NODE_TYPE_ENTITY_REFERENCE :
        return "NODE_TYPE_ENTITY_REFERENCE";
      case BAEXML_NODE_TYPE_ENTITY :
        return "NODE_TYPE_ENTITY";
      case BAEXML_NODE_TYPE_PROCESSING_INSTRUCTION:
        return "NODE_TYPE_PROCESSING_INSTRUCTION";
      case BAEXML_NODE_TYPE_COMMENT :
        return "NODE_TYPE_COMMENT";
      case BAEXML_NODE_TYPE_DOCUMENT :
        return "NODE_TYPE_DOCUMENT";
      case BAEXML_NODE_TYPE_DOCUMENT_TYPE :
        return "NODE_TYPE_DOCUMENT_TYPE";
      case BAEXML_NODE_TYPE_DOCUMENT_FRAGMENT :
        return "NODE_TYPE_DOCUMENT_FRAGMENT";
      case BAEXML_NODE_TYPE_NOTATION :
        return "NODE_TYPE_NOTATION";
      case BAEXML_NODE_TYPE_WHITESPACE :
        return "NODE_TYPE_WHITESPACE";
      case BAEXML_NODE_TYPE_SIGNIFICANT_WHITESPACE:
        return "NODE_TYPE_SIGNIFICANT_WHITESPACE";
      case BAEXML_NODE_TYPE_END_ELEMENT :
        return "NODE_TYPE_END_ELEMENT";
      case BAEXML_NODE_TYPE_END_ENTITY :
        return "NODE_TYPE_END_ENTITY";
      case BAEXML_NODE_TYPE_XML_DECLARATION :
        return "NODE_TYPE_XML_DECLARATION";
    }
    return "(* UNKNOWN NODE TYPE *)";
}

// NON-VIRTUAL ACCESSORS (implemented in this base class)
#define CHK(X) (X != 0 ? (const char *) X : "(null)")

void
baexml_Reader::dumpNode(bsl::ostream & os) const
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
        baexml_ElementAttribute attr;
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

} // namespace BloombergLP
// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2007
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
