// balxml_minireader.t.cpp                                            -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <balxml_minireader.h>

#include <balxml_errorinfo.h>

#include <bslim_testutil.h>

#include <bslma_testallocator.h>
#include <bslma_testallocatorexception.h>

#include <bsl_cstring.h>     // strlen()
#include <bsl_cstdlib.h>     // atoi()
#include <bsl_iostream.h>
#include <bsl_sstream.h>
#include <bsl_fstream.h>
#include <bsl_iomanip.h>
#include <bsl_string.h>

using namespace BloombergLP;
using namespace bsl;

// ============================================================================
//                                 TEST PLAN
// ----------------------------------------------------------------------------
//                                 Overview
//                                 --------
// ----------------------------------------------------------------------------

// ============================================================================
//                     STANDARD BDE ASSERT TEST FUNCTION
// ----------------------------------------------------------------------------

namespace {

int testStatus = 0;

void aSsErT(bool condition, const char *message, int line)
{
    if (condition) {
        cout << "Error " __FILE__ "(" << line << "): " << message
             << "    (failed)" << endl;

        if (0 <= testStatus && testStatus <= 100) {
            ++testStatus;
        }
    }
}

}  // close unnamed namespace

// ============================================================================
//               STANDARD BDE TEST DRIVER MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT       BSLIM_TESTUTIL_ASSERT
#define ASSERTV      BSLIM_TESTUTIL_ASSERTV

#define LOOP_ASSERT  BSLIM_TESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLIM_TESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLIM_TESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLIM_TESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLIM_TESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLIM_TESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLIM_TESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLIM_TESTUTIL_LOOP6_ASSERT

#define Q            BSLIM_TESTUTIL_Q   // Quote identifier literally.
#define P            BSLIM_TESTUTIL_P   // Print identifier and value.
#define P_           BSLIM_TESTUTIL_P_  // P(X) without '\n'.
#define T_           BSLIM_TESTUTIL_T_  // Print a tab (w/o newline).
#define L_           BSLIM_TESTUTIL_L_  // current Line number

#define CHK(X) (X != 0 ? (const char *) X : "(null)")

// ============================================================================
//                   GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------
static int verbose = 0;
static int veryVerbose = 0;
static int veryVeryVerbose = 0;

typedef balxml::MiniReader        Obj;
typedef balxml::ElementAttribute  ElementAttribute;
typedef balxml::NamespaceRegistry Registry;

const bsl::string::size_type npos = bsl::string::npos;

struct XmlEl {
    long                      d_startPosExpected;
    long                      d_endPosExpected;
    balxml::Reader::NodeType  d_typeExpected;
    const char               *d_text;
    const char               *d_textExpected; // 0 if any expected is OK
};

void prepareXmlFromTable(bsl::string& outDoc, XmlEl* table, int count)
{
    outDoc.clear();
    for (int i=0; i < count; ++i) {

        table[i].d_startPosExpected = outDoc.size();

        outDoc.append(table[i].d_text);

        table[i].d_endPosExpected = outDoc.size();
    }
}

void parseAndCompare(const char* name, XmlEl* table, int elementCount)
{
    bsl::string xmlDoc;

    balxml::NamespaceRegistry namespaces;
    balxml::PrefixStack prefixStack(&namespaces);
    Obj reader;

    reader.setPrefixStack(&prefixStack);

    prepareXmlFromTable(xmlDoc, table, elementCount);

    int rc = reader.open(xmlDoc.c_str(),
                         xmlDoc.size(),
                         name);

    ASSERT(rc == 0);

    int i;
    for (i = 0; ; ++i) {
        rc = reader.advanceToNextNode();
        if (rc != 0) {
            break;
        }

        balxml::Reader::NodeType nt = reader.nodeType ();
        long start = reader.nodeStartPosition();
        long end = reader.nodeEndPosition();

        if (veryVeryVerbose) {
            bsl::cout << "Pos start=" << start
                        << " end=" << end << " ";

            reader.dumpNode(bsl::cout);
        }

        LOOP2_ASSERT(table[i].d_typeExpected, nt,
                     table[i].d_typeExpected == nt);

        LOOP2_ASSERT(table[i].d_startPosExpected, start,
                     table[i].d_startPosExpected == start);

        LOOP2_ASSERT(table[i].d_endPosExpected, end,
                     table[i].d_endPosExpected == end);

        if (table[i].d_textExpected != 0) {
            const char *nodeText = reader.nodeValue();

            LOOP2_ASSERT(table[i].d_textExpected, nodeText,
                0 == bsl::strcmp(table[i].d_textExpected, nodeText));
        }
    }

    ASSERT(rc >0);
    ASSERT(i == elementCount);
    reader.close ();
}

// ----------------------------------------------------------------------------
XmlEl table91[] =
{
    {0, 0, balxml::Reader::e_NODE_TYPE_XML_DECLARATION,
          "<?xml version='1.0' encoding='UTF-8'?>" , 0 },
    {0, 0, balxml::Reader::e_NODE_TYPE_WHITESPACE, "\n" , 0},
    {0, 0, balxml::Reader::e_NODE_TYPE_ELEMENT, "<RootElement>", 0 },

    {0, 0, balxml::Reader::e_NODE_TYPE_ELEMENT, "<Elem1>" , 0 },
    {0, 0, balxml::Reader::e_NODE_TYPE_TEXT, "&lt;&amp;&apos;&quot;&gt;",
                                          "<&'\">" },
    {0, 0, balxml::Reader::e_NODE_TYPE_END_ELEMENT, "</Elem1>" , 0 },

    {0, 0, balxml::Reader::e_NODE_TYPE_ELEMENT, "<Elem2>" , 0 },
    {0, 0, balxml::Reader::e_NODE_TYPE_TEXT,
           "&#127;&#x7e;&#128;&#x81;&#2047;&#2048;&#x801;"
           "&#65535;&#x10000;&#x10001;&#x10FFFF;",
           "\x7F\x7e\xC2\x80\xC2\x81\xDF\xBF\xE0\xA0\x80\xE0\xA0\x81"
           "\xEF\xBF\xBF\xF0\x90\x80\x80\xF0\x90\x80\x81\xF4\x8F\xBF\xBF" },
    {0, 0, balxml::Reader::e_NODE_TYPE_END_ELEMENT, "</Elem2>" , 0 },

    {0, 0, balxml::Reader::e_NODE_TYPE_END_ELEMENT, "</RootElement>", 0 },
    {0, 0, balxml::Reader::e_NODE_TYPE_WHITESPACE,   "\n", 0 }
};

XmlEl table81[] =
{
    {0, 0, balxml::Reader::e_NODE_TYPE_XML_DECLARATION,
          "<?xml version='1.0' encoding='UTF-8'?>" , 0 },
    {0, 0, balxml::Reader::e_NODE_TYPE_WHITESPACE, "\n" , 0},
    {0, 0, balxml::Reader::e_NODE_TYPE_ELEMENT,
       "<directory-entry xmlns:dir='http://bloomberg.com/schemas/directory'>",
                                                                           0 },
    {0, 0, balxml::Reader::e_NODE_TYPE_WHITESPACE,   "\n     ", 0 },
    {0, 0, balxml::Reader::e_NODE_TYPE_ELEMENT, "<name>" , 0 },
    {0, 0, balxml::Reader::e_NODE_TYPE_TEXT, "John Smith", 0 },
    {0, 0, balxml::Reader::e_NODE_TYPE_END_ELEMENT, "</name>" , 0 },
    {0, 0, balxml::Reader::e_NODE_TYPE_WHITESPACE,   "\n     ", 0 },
    {0, 0, balxml::Reader::e_NODE_TYPE_ELEMENT,
                                                "<phone dir:phonetype='cell'>",
                                                                           0 },
    {0, 0, balxml::Reader::e_NODE_TYPE_TEXT, "212-318-2000", 0 },
    {0, 0, balxml::Reader::e_NODE_TYPE_END_ELEMENT, "</phone>" , 0 },
    {0, 0, balxml::Reader::e_NODE_TYPE_WHITESPACE,   "\n     ", 0 },
    {0, 0, balxml::Reader::e_NODE_TYPE_ELEMENT, "<address/>" , 0 },
    {0, 0, balxml::Reader::e_NODE_TYPE_WHITESPACE,   "\n", 0 },
    {0, 0, balxml::Reader::e_NODE_TYPE_END_ELEMENT,
           "</directory-entry>", 0 },
    {0, 0, balxml::Reader::e_NODE_TYPE_WHITESPACE,   "\n", 0 }
};

// ----------------------------------------------------------------------------
// Start of usage example, extract to the 'balxml::Reader' header file.
//
// Utility function to skip past white space.
//..
int advancePastWhiteSpace(balxml::Reader& reader) {
    static const char whiteSpace[] = "\n\r\t ";
    const char *value = 0;
    int         type = 0;
    int         rc = 0;

    do {
        rc    = reader.advanceToNextNode();
        value = reader.nodeValue();
        type  = reader.nodeType();
    } while ((0 == rc && type == balxml::Reader::e_NODE_TYPE_WHITESPACE) ||
             (type == balxml::Reader::e_NODE_TYPE_TEXT &&
              bsl::strlen(value) == bsl::strspn(value, whiteSpace)));

    ASSERT( reader.nodeType() != balxml::Reader::e_NODE_TYPE_WHITESPACE);

    return rc;
}
//..
// The main program parses an XML string using the TestReader
//..
int usageExample()
{
//..
// The following string describes xml for a very simple user directory.
// The top level element contains one xml namespace attribute, with one
// embedded entry describing a user.
//..
    static const char TEST_XML_STRING[] =
       "<?xml version='1.0' encoding='UTF-8'?>\n"
       "<directory-entry xmlns:dir='http://bloomberg.com/schemas/directory'>\n"
       "    <name>John Smith</name>\n"
       "    <phone dir:phonetype='cell'>212-318-2000</phone>\n"
       "    <address/>\n"
       "</directory-entry>\n";
//..
// In order to read the XML, we first need to construct a
// 'balxml::NamespaceRegistry' object, a 'balxml::PrefixStack' object, and a
// 'TestReader' object, where 'TestReader' is a derived implementation of
// 'balxml_reader'.
//..
    balxml::NamespaceRegistry namespaces;
    balxml::PrefixStack prefixStack(&namespaces);
    balxml::MiniReader miniReader; balxml::Reader& reader = miniReader;

    ASSERT(!reader.isOpen());
//..
// The reader uses a 'balxml::PrefixStack' to manage namespace prefixes so we
// need to set it before we call open.
//..
    reader.setPrefixStack(&prefixStack);
    ASSERT(reader.prefixStack());
    ASSERT(reader.prefixStack() == &prefixStack);
//..
// Now we call the 'open' method to setup the reader for parsing using the data
// contained in the in the XML string.
//..
    reader.open(TEST_XML_STRING, sizeof(TEST_XML_STRING) -1, 0, "UTF-8");
//..
// Confirm that the 'bdem::Reader' has opened properly
//..
    ASSERT( reader.isOpen());
    ASSERT(!bsl::strncmp(reader.documentEncoding(), "UTF-8", 5));
    ASSERT( reader.nodeType() == balxml::Reader::e_NODE_TYPE_NONE);
    ASSERT(!reader.nodeName());
    ASSERT(!reader.nodeHasValue());
    ASSERT(!reader.nodeValue());
    ASSERT(!reader.nodeDepth());
    ASSERT(!reader.numAttributes());
    ASSERT(!reader.isEmptyElement());
//..
// Advance through all the nodes and assert all information contained at each
// node is correct.
//
// Assert the next node's document type is xml.
//..
    int rc = advancePastWhiteSpace(reader);
    ASSERT( 0 == rc);
    ASSERT( reader.nodeType() ==
                              balxml::Reader::e_NODE_TYPE_XML_DECLARATION);
    ASSERT(!bsl::strcmp(reader.nodeName(), "xml"));
    ASSERT( reader.nodeHasValue());
    ASSERT(!bsl::strcmp(reader.nodeValue(), "version='1.0' encoding='UTF-8'"));
    ASSERT( reader.nodeDepth() == 1);
    ASSERT(!reader.numAttributes());
    ASSERT(!reader.isEmptyElement());
    ASSERT( 0 == rc);
    ASSERT( reader.nodeDepth() == 1);
//..
// Advance to the top level element, which has one attribute, the xml
// namespace.  Assert the namespace information has been added correctly to the
// prefix stack.
//..
    rc = advancePastWhiteSpace(reader);
    ASSERT( 0 == rc);
    ASSERT( reader.nodeType() == balxml::Reader::e_NODE_TYPE_ELEMENT);
    ASSERT(!bsl::strcmp(reader.nodeName(), "directory-entry"));
    ASSERT(!reader.nodeHasValue());
    ASSERT( reader.nodeDepth() == 1);
    ASSERT( reader.numAttributes() == 1);
    ASSERT(!reader.isEmptyElement());

    ASSERT(!bsl::strcmp(prefixStack.lookupNamespacePrefix("dir"), "dir"));
    ASSERT(prefixStack.lookupNamespaceId("dir") == 0);
    ASSERT(!bsl::strcmp(prefixStack.lookupNamespaceUri("dir"),
                        "http://bloomberg.com/schemas/directory"));
//..
// The XML being read contains one entry describing a user, advance the users
// name name and assert all information can be read correctly.
//..
    rc = advancePastWhiteSpace(reader);
    ASSERT( 0 == rc);
    ASSERT( reader.nodeType() == balxml::Reader::e_NODE_TYPE_ELEMENT);
    ASSERT(!bsl::strcmp(reader.nodeName(), "name"));
    ASSERT(!reader.nodeHasValue());
    ASSERT( reader.nodeDepth() == 2);
    ASSERT( reader.numAttributes() == 0);
    ASSERT(!reader.isEmptyElement());

    rc = reader.advanceToNextNode();
    ASSERT( 0 == rc);
    ASSERT( reader.nodeType() == balxml::Reader::e_NODE_TYPE_TEXT);
    ASSERT( reader.nodeHasValue());
    ASSERT(!bsl::strcmp(reader.nodeValue(), "John Smith"));
    ASSERT( reader.nodeDepth() == 3);
    ASSERT( reader.numAttributes() == 0);
    ASSERT(!reader.isEmptyElement());

    rc = reader.advanceToNextNode();
    ASSERT( 0 == rc);
    ASSERT( reader.nodeType() == balxml::Reader::e_NODE_TYPE_END_ELEMENT);
    ASSERT(!bsl::strcmp(reader.nodeName(), "name"));
    ASSERT(!reader.nodeHasValue());
    ASSERT( reader.nodeDepth() == 2);
    ASSERT( reader.numAttributes() == 0);
    ASSERT(!reader.isEmptyElement());
//..
// Advance to the user's phone number and assert all information can be read
// correctly.
//..
    rc = advancePastWhiteSpace(reader);
    ASSERT( 0 == rc);
    ASSERT( reader.nodeType() == balxml::Reader::e_NODE_TYPE_ELEMENT);
    ASSERT(!bsl::strcmp(reader.nodeName(), "phone"));
    ASSERT(!reader.nodeHasValue());
    ASSERT( reader.nodeDepth() == 2);
    ASSERT( reader.numAttributes() == 1);
    ASSERT(!reader.isEmptyElement());
//..
// The phone node has one attribute, look it up and assert the
// 'balxml::ElementAttribute' contains valid information and that the prefix
// returns the correct namespace URI from the prefix stack.
//..
    balxml::ElementAttribute elemAttr;

    rc = reader.lookupAttribute(&elemAttr, 0);
    ASSERT( 0 == rc);
    ASSERT(!elemAttr.isNull());
    ASSERT(!bsl::strcmp(elemAttr.qualifiedName(), "dir:phonetype"));
    ASSERT(!bsl::strcmp(elemAttr.value(), "cell"));
    ASSERT(!bsl::strcmp(elemAttr.prefix(), "dir"));
    ASSERT(!bsl::strcmp(elemAttr.localName(), "phonetype"));
    ASSERT(!bsl::strcmp(elemAttr.namespaceUri(),
                        "http://bloomberg.com/schemas/directory"));
    ASSERT( elemAttr.namespaceId() == 0);

    ASSERT(!bsl::strcmp(prefixStack.lookupNamespaceUri(elemAttr.prefix()),
                        elemAttr.namespaceUri()));

    rc = advancePastWhiteSpace(reader);
    ASSERT( 0 == rc);
    ASSERT( reader.nodeType() == balxml::Reader::e_NODE_TYPE_TEXT);
    ASSERT( reader.nodeHasValue());
    ASSERT(!bsl::strcmp(reader.nodeValue(), "212-318-2000"));
    ASSERT( reader.nodeDepth() == 3);
    ASSERT( reader.numAttributes() == 0);
    ASSERT(!reader.isEmptyElement());

    rc = advancePastWhiteSpace(reader);
    ASSERT( 0 == rc);
    ASSERT( reader.nodeType() == balxml::Reader::e_NODE_TYPE_END_ELEMENT);
    ASSERT(!bsl::strcmp(reader.nodeName(), "phone"));
    ASSERT(!reader.nodeHasValue());
    ASSERT( reader.nodeDepth() == 2);
    ASSERT( reader.numAttributes() == 0);
    ASSERT(!reader.isEmptyElement());
//..
// Advance to the user's address and assert all information can be read
// correctly.
//..
    rc = advancePastWhiteSpace(reader);
    ASSERT( 0 == rc);
    ASSERT( reader.nodeType() == balxml::Reader::e_NODE_TYPE_ELEMENT);
    ASSERT(!bsl::strcmp(reader.nodeName(), "address"));
    ASSERT(!reader.nodeHasValue());
    ASSERT( reader.nodeDepth() == 2);
    ASSERT( reader.numAttributes() == 0);
    ASSERT( reader.isEmptyElement());
//..
// Advance to the end element.
//..
    rc = advancePastWhiteSpace(reader);
    ASSERT( 0 == rc);
    ASSERT( reader.nodeType() == balxml::Reader::e_NODE_TYPE_END_ELEMENT);
    ASSERT(!bsl::strcmp(reader.nodeName(), "directory-entry"));
    ASSERT(!reader.nodeHasValue());
    ASSERT( reader.nodeDepth() == 1);
    ASSERT( reader.numAttributes() == 0);
    ASSERT(!reader.isEmptyElement());
//..
// Close the reader.
//..
    reader.close();
    ASSERT(!reader.isOpen());

    return 0;
}
//..
// End of usage example, extract to the 'balxml::Reader' header file.

// ============================================================================
//  Let make numElements and numElements static
//  to be accessible out of scope parse () function for future analysis.
//  Since test is single threaded, no MT-issues are here
// ============================================================================
static int test = 0;

static void  processNode(balxml::Reader *reader)
{
    balxml::Reader::NodeType  nodeType = reader->nodeType ();
    const char  *name = reader->nodeName();
    const char  *value = reader->nodeValue ();

    if (veryVerbose) {
        bsl::cout << reader->getLineNumber()
        << ":"
        << reader->getColumnNumber()
        << " NodeType="
        << nodeType
        << "("
        << balxml::Reader::nodeTypeAsString (nodeType)
        << ") name=" << CHK(name)
        << " value=" << CHK(value)
        << bsl::endl;
    }

    int numAttr = reader->numAttributes();

    for (int i = 0; i < numAttr; ++i)
    {
        balxml::ElementAttribute attr;
        reader->lookupAttribute(&attr, i);

        if (veryVerbose) {
            bsl::cout << "  ATTRIBUTE  "
           << CHK(attr.qualifiedName())
           << "="
           << CHK(attr.value())
           << " uri="
           << CHK(attr.namespaceUri())
           << bsl::endl;
        }
    }
}

static int parseAndProcess(balxml::Reader *reader)
{
    int rc;
    while ((rc=reader->advanceToNextNode()) == 0)
    {
        processNode (reader);
    }

    if (rc < 0)
    {
        const balxml::ErrorInfo& errInfo = reader->errorInfo();

        bsl::cout << errInfo;
    }
    return rc;
}

// XML header information used by ggg function.  'strXmlStart' + 'strXmlEnd' =
// 256 bytes.
const char strXmlStart[] =
    "<?xml version='1.0' encoding='UTF-8'?>\n"
    "<!-- RCSId_bascfg_xsd = \"$Id: $\" -->\n"
    "<xs:schema xmlns:xs='http://www.w3.org/2001/XMLSchema'\n"
    "    elementFormDefault='qualified'\n"
    "    xmlns:bdem='http://bloomberg.com/schemas/bdem'\n"
    "    bdem:package='bascfg'>\n";

const char strXmlEnd[] =
    "</xs:schema>";

void addOpenTag(bsl::string& xmlStr, int indent, const char* tag, int n) {
    bsl::stringstream ss;

    ss << bsl::setw(indent) << "" << "<" << tag << n << ">\n"
       << bsl::setw(indent + 4) << "" << "<Element1>element1</Element1>\n"
       << bsl::setw(indent + 4) << "" << "<Element2>element2</Element2>\n"
       << bsl::setw(indent + 4) << "" << "<Element3>element3</Element3>\n"
       << bsl::setw(indent + 4) << "" << "<![CDATA[&lt;123&#240;&gt;]]>\n";
    // -----------------------------------123456789012345678901234567890

    xmlStr.append(ss.str());
}

void addCloseTag(bsl::string& xmlStr, int indent, const char* tag, int n) {
    bsl::stringstream ss;

    ss << bsl::setw(indent) << "" << "</" << tag << n << ">\n";

    xmlStr.append(ss.str());
}

// Recursively add nested nodes from 'currentDepth' to 'depth' to 'xmlStr'.
void addDepth(bsl::string& xmlStr, int currentDepth, int depth) {
    if (currentDepth == depth) {
        return;                                                       // RETURN
    }
    int indent = (4 * currentDepth) + 4;
    addOpenTag(xmlStr, indent, "Depth", currentDepth % 10);
    addDepth(xmlStr, currentDepth + 1, depth);
    addCloseTag(xmlStr, indent, "Depth", currentDepth % 10);
}

// Recursively add nodes from 'currentNode' to 'numNodes' to 'xmlStr'.  For
// each node added recursively add nested nodes from 'currentDepth' to
// 'depth'.
void addNodes(bsl::string& xmlStr,
              int          currentNode,
              int          numNodes,
              int          currentDepth,
              int          depth) {
    if (currentNode == numNodes) {
        return;                                                       // RETURN
    }
    int indent = 4 * currentDepth;
    addOpenTag(xmlStr, indent, "Node", currentNode % 10);
    addDepth(xmlStr, currentDepth, depth);
    addCloseTag(xmlStr, indent, "Node", currentNode % 10);
    addNodes(xmlStr, currentNode + 1, numNodes, currentDepth, depth);
}

// Add 'numNodes' each having nested nodes of 'depth' to 'xmlStr'.
void ggg(bsl::string& xmlStr, int numNodes, int depth) {
    xmlStr.assign(strXmlStart);
    addNodes(xmlStr, 0, numNodes, 0, depth);
    xmlStr.append(strXmlEnd);
}

void checkNodeName(Obj& reader, const char* tag, int n) {
    bsl::stringstream ss1;
    ss1 << tag << n;
    ASSERT(!bsl::strcmp(reader.nodeName(), ss1.str().c_str()));
}

// Recursively read nested nodes from 'currentDepth' to 'depth' to 'xmlStr'.
void readDepth(Obj& reader, int currentDepth, int depth) {
    if (currentDepth == depth) {
        return;                                                       // RETURN
    }

    // Note that 'currentDepth' does not equal 'reader.nodeDepth()', this is
    // because 'currentDepth' does not account for the depth add by the header
    // information.

    int rc = advancePastWhiteSpace(reader);
    ASSERT( reader.nodeType() == balxml::Reader::e_NODE_TYPE_ELEMENT);
    checkNodeName(reader, "Depth", currentDepth % 10);

    rc = advancePastWhiteSpace(reader);
    ASSERT( reader.nodeType() == balxml::Reader::e_NODE_TYPE_ELEMENT);
    ASSERT(!bsl::strcmp(reader.nodeName(), "Element1"));
    rc = advancePastWhiteSpace(reader);
    ASSERT( reader.nodeType() == balxml::Reader::e_NODE_TYPE_TEXT);
    ASSERT(!bsl::strcmp(reader.nodeValue(), "element1"));
    rc = advancePastWhiteSpace(reader);
    ASSERT( reader.nodeType() == balxml::Reader::e_NODE_TYPE_END_ELEMENT);
    ASSERT(!bsl::strcmp(reader.nodeName(), "Element1"));

    rc = advancePastWhiteSpace(reader);
    ASSERT( reader.nodeType() == balxml::Reader::e_NODE_TYPE_ELEMENT);
    ASSERT(!bsl::strcmp(reader.nodeName(), "Element2"));
    rc = advancePastWhiteSpace(reader);
    ASSERT( reader.nodeType() == balxml::Reader::e_NODE_TYPE_TEXT);
    ASSERT(!bsl::strcmp(reader.nodeValue(), "element2"));
    rc = advancePastWhiteSpace(reader);
    ASSERT( reader.nodeType() == balxml::Reader::e_NODE_TYPE_END_ELEMENT);
    ASSERT(!bsl::strcmp(reader.nodeName(), "Element2"));

    rc = advancePastWhiteSpace(reader);
    ASSERT( reader.nodeType() == balxml::Reader::e_NODE_TYPE_ELEMENT);
    ASSERT(!bsl::strcmp(reader.nodeName(), "Element3"));
    rc = advancePastWhiteSpace(reader);
    ASSERT( reader.nodeType() == balxml::Reader::e_NODE_TYPE_TEXT);
    ASSERT(!bsl::strcmp(reader.nodeValue(), "element3"));
    rc = advancePastWhiteSpace(reader);
    ASSERT( reader.nodeType() == balxml::Reader::e_NODE_TYPE_END_ELEMENT);
    ASSERT(!bsl::strcmp(reader.nodeName(), "Element3"));

    rc = advancePastWhiteSpace(reader);
    ASSERT( reader.nodeType() == balxml::Reader::e_NODE_TYPE_CDATA);
    ASSERT(!bsl::strcmp(reader.nodeValue(), "&lt;123&#240;&gt;"));

    readDepth(reader, currentDepth + 1, depth);

    rc = advancePastWhiteSpace(reader);
    ASSERT( reader.nodeType() == balxml::Reader::e_NODE_TYPE_END_ELEMENT);
    checkNodeName(reader, "Depth", currentDepth % 10);
}

// Recursively read nodes from 'currentNode' to 'numNodes' to 'xmlStr'.  For
// each node read recursively read nested nodes from 'currentDepth' to
// 'depth'.
void readNodes(Obj& reader,
               int  currentNode,
               int  numNodes,
               int  currentDepth,
               int  depth)
{
    if (currentNode == numNodes) {
        return;                                                       // RETURN
    }

    int rc = advancePastWhiteSpace(reader);
    ASSERT( reader.nodeType() == balxml::Reader::e_NODE_TYPE_ELEMENT);
    checkNodeName(reader, "Node", currentNode % 10);

    rc = advancePastWhiteSpace(reader);
    ASSERT( reader.nodeType() == balxml::Reader::e_NODE_TYPE_ELEMENT);
    ASSERT(!bsl::strcmp(reader.nodeName(), "Element1"));
    rc = advancePastWhiteSpace(reader);
    ASSERT( reader.nodeType() == balxml::Reader::e_NODE_TYPE_TEXT);
    ASSERT(!bsl::strcmp(reader.nodeValue(), "element1"));
    rc = advancePastWhiteSpace(reader);
    ASSERT( reader.nodeType() == balxml::Reader::e_NODE_TYPE_END_ELEMENT);
    ASSERT(!bsl::strcmp(reader.nodeName(), "Element1"));

    rc = advancePastWhiteSpace(reader);
    ASSERT( reader.nodeType() == balxml::Reader::e_NODE_TYPE_ELEMENT);
    ASSERT(!bsl::strcmp(reader.nodeName(), "Element2"));
    rc = advancePastWhiteSpace(reader);
    ASSERT( reader.nodeType() == balxml::Reader::e_NODE_TYPE_TEXT);
    ASSERT(!bsl::strcmp(reader.nodeValue(), "element2"));
    rc = advancePastWhiteSpace(reader);
    ASSERT( reader.nodeType() == balxml::Reader::e_NODE_TYPE_END_ELEMENT);
    ASSERT(!bsl::strcmp(reader.nodeName(), "Element2"));

    rc = advancePastWhiteSpace(reader);
    ASSERT( reader.nodeType() == balxml::Reader::e_NODE_TYPE_ELEMENT);
    ASSERT(!bsl::strcmp(reader.nodeName(), "Element3"));
    rc = advancePastWhiteSpace(reader);
    ASSERT( reader.nodeType() == balxml::Reader::e_NODE_TYPE_TEXT);
    ASSERT(!bsl::strcmp(reader.nodeValue(), "element3"));
    rc = advancePastWhiteSpace(reader);
    ASSERT( reader.nodeType() == balxml::Reader::e_NODE_TYPE_END_ELEMENT);
    ASSERT(!bsl::strcmp(reader.nodeName(), "Element3"));

    rc = advancePastWhiteSpace(reader);
    ASSERT( reader.nodeType() == balxml::Reader::e_NODE_TYPE_CDATA);
    ASSERT(!bsl::strcmp(reader.nodeValue(), "&lt;123&#240;&gt;"));

    readDepth(reader, currentDepth, depth);

    rc = advancePastWhiteSpace(reader);
    ASSERT( reader.nodeType() == balxml::Reader::e_NODE_TYPE_END_ELEMENT);
    checkNodeName(reader, "Node", currentNode % 10);

    readNodes(reader, currentNode + 1, numNodes, currentDepth, depth);
}

void readHeader(Obj& reader)
{
    int rc = advancePastWhiteSpace(reader);
    //TBD: Mini Reader needs to be fix to pass back the correct encoding.
    //ASSERT(!bsl::strncmp(reader.documentEncoding(), "UTF-8", 5));
    ASSERT( reader.nodeType() ==
                              balxml::Reader::e_NODE_TYPE_XML_DECLARATION);
    ASSERT(!bsl::strcmp(reader.nodeName(), "xml"));
    ASSERT( reader.nodeHasValue());
    ASSERT(!bsl::strcmp(reader.nodeValue(), "version='1.0' encoding='UTF-8'"));

    rc = advancePastWhiteSpace(reader);
    ASSERT( reader.nodeType() == balxml::Reader::e_NODE_TYPE_COMMENT);
    ASSERT(!bsl::strcmp(reader.nodeValue(),
                        " RCSId_bascfg_xsd = \"$Id: $\" "));

    rc = advancePastWhiteSpace(reader);
    ASSERT( reader.nodeType() == balxml::Reader::e_NODE_TYPE_ELEMENT);
    ASSERT(!bsl::strcmp(reader.nodeName(), "xs:schema"));
}
// ============================================================================
//                               MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    test = argc > 1 ? bsl::atoi(argv[1]) : 0;
    verbose = argc > 2;
    veryVerbose = argc > 3;
    veryVeryVerbose = argc > 4;

    bslma::TestAllocator testAllocator(veryVeryVerbose);

    bsl::cout << "TEST " << __FILE__ << " CASE " << test << bsl::endl;;

    switch (test)
    {
      case 0:  // Zero is always the leading case.
      case 12: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //
        // This is a implementation of the 'balxml::Reader' interface.
        //
        // Concerns:
        //   That the usage example compiles and runs correctly.
        //
        // Plan:
        //   Copy the usage example from the component-level documentation.
        //   Replace 'assert' with 'ASSERT'.
        //
        // Testing:
        //   balxml::MiniReader();  // Constructor
        //   ~balxml::MiniReader(); // Destructor
        //   setPrefixStack(balxml::PrefixStack *prefixes);
        //   prefixStack();
        //   isOpen();
        //   documentEncoding();
        //   nodeType();
        //   nodeName();
        //   nodeHasValue());
        //   nodeValue());
        //   nodeDepth());
        //   numAttributes());
        //   isEmptyElement());
        //   advanceToNextNode();
        //   lookupAttribute(ElemAtt a, int index);
        //   lookupAttribute(ElemAtt a, char *qname);
        //   lookupAttribute(ElemAtt a, char *localname, char *nsUri);
        //   lookupAttribute(ElemAtt a, char *localname, int nsId);
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nUSAGE EXAMPLE"
                               << "\n=============" << bsl::endl;

        usageExample();

      } break;

      case 11: {
        // --------------------------------------------------------------------
        // TESTING 'xsi:nil' attribute
        //
        // Plan:
        //
        // Testing:
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << bsl::endl
                               << "Testing 'xsi:nil' attribute"   << bsl::endl
                               << "===========================\n" << bsl::endl;

        {
            static const string xmlStr =
                "<?xml version='1.0' encoding='UTF-8'?>\n"
                "<!-- RCSId_bascfg_xsd = \"$Id: $\" -->\n"
                "<xs:schema xmlns:xs='http://www.w3.org/2001/XMLSchema'\n"
                "    elementFormDefault='qualified'\n"
                "    xmlns:bdem='http://bloomberg.com/schemas/bdem'\n"
                "    bdem:package='bascfg'>\n"
                "<Node1 xsi:nil='true'/>\n"
                "<Node2 xsi:nil='false'/>\n"
                "<Node3/>\n"
                "</xs:schema>";

          if (veryVerbose) { P(xmlStr); }

          balxml::NamespaceRegistry namespaces;
          balxml::PrefixStack prefixStack(&namespaces);
          Obj miniReader; Obj& reader = miniReader;

          reader.setPrefixStack(&prefixStack);

          int rc = reader.open(&xmlStr[0], xmlStr.size());
          ASSERT(-1 < rc);

          ASSERT( reader.isOpen());
          ASSERT( reader.nodeType() == balxml::Reader::e_NODE_TYPE_NONE);

          readHeader(reader);

          rc = advancePastWhiteSpace(reader);
          ASSERT(reader.nodeType() == balxml::Reader::e_NODE_TYPE_ELEMENT);
          ASSERT(!bsl::strcmp(reader.nodeName(), "Node1"));
          ASSERT(1 == reader.numAttributes());

          balxml::ElementAttribute attr;
          ASSERT(0 == reader.lookupAttribute(&attr, 0));
          ASSERT(ElementAttribute::k_ATTR_IS_XSIDECL == attr.flags());
          ASSERT(0 == strcmp("xsi:nil", attr.qualifiedName()));
          ASSERT(0 == strcmp("xsi", attr.prefix()));
          ASSERT(0 == strcmp("nil", attr.localName()));
          ASSERT(0 == strcmp("true", attr.value()));
          ASSERT(0 == strcmp("http://www.w3.org/2001/XMLSchema-instance",
                             attr.namespaceUri()));
          ASSERT(Registry::e_XMLSCHEMA_INSTANCE == attr.namespaceId());

          rc = advancePastWhiteSpace(reader);
          ASSERT(reader.nodeType() == balxml::Reader::e_NODE_TYPE_ELEMENT);
          ASSERT(!bsl::strcmp(reader.nodeName(), "Node2"));
          ASSERT(1 == reader.numAttributes());
          attr.reset();
          ASSERT(0 == reader.lookupAttribute(&attr, 0));
          ASSERT(ElementAttribute::k_ATTR_IS_XSIDECL == attr.flags());
          ASSERT(0 == strcmp("xsi:nil", attr.qualifiedName()));
          ASSERT(0 == strcmp("xsi", attr.prefix()));
          ASSERT(0 == strcmp("nil", attr.localName()));
          ASSERT(0 == strcmp("false", attr.value()));
          ASSERT(0 == strcmp("http://www.w3.org/2001/XMLSchema-instance",
                             attr.namespaceUri()));
          ASSERT(Registry::e_XMLSCHEMA_INSTANCE == attr.namespaceId());

          rc = advancePastWhiteSpace(reader);
          ASSERT(reader.nodeType() == balxml::Reader::e_NODE_TYPE_ELEMENT);
          ASSERT(!bsl::strcmp(reader.nodeName(), "Node3"));
          ASSERT(0 == reader.numAttributes());
          ASSERT(reader.isEmptyElement());

          reader.close();
        }
      } break;
      case 10: {
        // --------------------------------------------------------------------
        // TESTING decoding 'CDATA'
        //
        // Plan:
        //
        // Testing:
        // --------------------------------------------------------------------
        if (verbose) bsl::cout << bsl::endl
                               << "Testing 'CDATA'"   << bsl::endl
                               << "===============\n" << bsl::endl;

        {
            const char *CDATA_STR = "<![CDATA[S & P]]>";

            static const string xmlStr =
                "<?xml version='1.0' encoding='UTF-8'?>\n"
                "<!-- RCSId_bascfg_xsd = \"$Id: $\" -->\n"
                "<xs:schema xmlns:xs='http://www.w3.org/2001/XMLSchema'\n"
                "    elementFormDefault='qualified'\n"
                "    xmlns:bdem='http://bloomberg.com/schemas/bdem'\n"
                "    bdem:package='bascfg'>\n"
                "<Node>\n"
                "    <Element>&lt;![CDATA[S &amp; P]]></Element>\n"
                "    \n"
                "</Node>\n"
                "</xs:schema>";

          if (veryVerbose) { P(xmlStr); }

          balxml::NamespaceRegistry namespaces;
          balxml::PrefixStack prefixStack(&namespaces);
          Obj miniReader; Obj& reader = miniReader;

          reader.setPrefixStack(&prefixStack);

          int rc = reader.open(&xmlStr[0], xmlStr.size());
          ASSERT(-1 < rc);

          ASSERT( reader.isOpen());
          ASSERT( reader.nodeType() == balxml::Reader::e_NODE_TYPE_NONE);

          readHeader(reader);

          rc = advancePastWhiteSpace(reader);
          ASSERT(reader.nodeType() == balxml::Reader::e_NODE_TYPE_ELEMENT);
          ASSERT(!bsl::strcmp(reader.nodeName(), "Node"));

          rc = advancePastWhiteSpace(reader);
          ASSERT(reader.nodeType() == balxml::Reader::e_NODE_TYPE_ELEMENT);
          ASSERT(!bsl::strcmp(reader.nodeName(), "Element"));

          rc = advancePastWhiteSpace(reader);
          ASSERT( reader.nodeType() == balxml::Reader::e_NODE_TYPE_TEXT);
          LOOP_ASSERT(reader.nodeValue(),
                      !bsl::strcmp(reader.nodeValue(), CDATA_STR));

          rc = advancePastWhiteSpace(reader);
          ASSERT(reader.nodeType()
                               == balxml::Reader::e_NODE_TYPE_END_ELEMENT);
          ASSERT(!bsl::strcmp(reader.nodeName(), "Element"));

          rc = advancePastWhiteSpace(reader);
          ASSERT(reader.nodeType()
                               == balxml::Reader::e_NODE_TYPE_END_ELEMENT);
          ASSERT(!bsl::strcmp(reader.nodeName(), "Node"));

          reader.close();
        }
      } break;
      case 9: {
        // --------------------------------------------------------------------
        // REFERENCE SUBSTITUTION TEST
        //
        // Concerns:
        //   That balxml::MiniReader performs properly reference processing
        //
        // Plan:
        //   Create an Xml document from array of XmlEl elements.
        //   Each XmlEl contains a string with single xml construct,
        //   representing a node and expected string as node value.
        //   Concatenation of all XmlEl string into single string produces
        //   the whole xml document.  During parsing compare each node's
        //   characteristics, including expected text value.
        // --------------------------------------------------------------------
        if (verbose) bsl::cout << "\nREFERENCE SUBSTITUTION TEST"
                               << "\n==========================="
                               << bsl::endl;

        int numEl = sizeof(table91)/sizeof(table91[0]);

        parseAndCompare("table91", table91, numEl);

      } break;

      case 8: {
        // --------------------------------------------------------------------
        // NODE POSITIONS TEST
        //
        // Concerns:
        //   That balxml::MiniReader reports correct start and end positions
        //   each node.
        //
        // Plan:
        //   Create an Xml document from array of XmlEl elements.
        //   Each XmlEl contains a string with single xml construct,
        //   representing a node.  Concatenation of all XmlEl string
        //   into single string produces the whole xml document.  During
        //   the concatenation process we can calculate the offset of the
        //   start and end of each xml construct from the beginning of document
        //   and save these offsets in the corresponding XmlEl element.
        //   So we know the expected positions for each node.
        //   We have to compare table positions with the positions reported
        //   by the reader during parsing.
        //
        // --------------------------------------------------------------------
        if (verbose) bsl::cout << "\nNODE POSITIONS TEST"
                               << "\n==================="
                               << bsl::endl;

        int numEl = sizeof(table81)/sizeof(table81[0]);

        parseAndCompare("table81", table81, numEl);

      } break;
      case 7: {
        // --------------------------------------------------------------------
        // LOTS OF ATTRIBUTES (STRESS TEST)
        //
        // Concerns:
        //   That balxml::MiniReader operate correctly with xml strings
        //   (buffers) with many attributes.  The strings in the test vectors
        //   all contain a *single element* with different number of
        //   attributes.
        //
        // Plan:
        //   Construct a 'balxml::MiniReader' reader capable of buffering
        //   'bufferSize' bytes of an XML document created with many
        //   attributes.  For each test vector generate the XML input starting
        //   with the 'strXmlStart' template, add attributes to a single node
        //   to get the desired input size and then close the input with the
        //   'strXmlEnd' template.  Use the reader to parse the input and
        //   verify that the node and all its attributes are intact.  The
        //   'bufferSize' is used to perturb the testing with different
        //   initial buffer sizes, this will force the reader to operate
        //   differently.  I.e., the reader will need to stop more frequently
        //   to read in input with smaller buffer size while using less
        //   memory, while the exact opposite will be true with larger
        //   'bufferSize's'.
        //
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nLOTS OF ATTRIBUTES (STRESS TEST)"
                               << "\n================================"
                               << bsl::endl;

        static const struct {
            int d_lineNum;
            int d_numAttributes;
            int d_bufferSize;
        } DATA[] = {
            //Line    , nAtts , bufferSize
            { L_      , 1     , 0         },
            { L_      , 10    , 0         },
            { L_      , 95    , 0         }, // 256 + 8 + 8(95) = 1024
            { L_      , 96    , 0         },
            { L_      , 990   , 0         },
            { L_      , 991   , 0         }, // 256 + 8 + 8(991) = 8192
            { L_      , 992   , 0         },
            { L_      , 16350 , 0         },
            { L_      , 16351 , 0         }, // 256 + 8 + 8(16351) = 131072
            { L_      , 16352 , 0         },
            { L_      , 1     , 1024      },
            { L_      , 94    , 1024      },
            { L_      , 95    , 1024      }, // 256 + 8 + 8(95) = 1024
            { L_      , 96    , 1024      },
            { L_      , 990   , 1024      },
            { L_      , 991   , 1024      }, // 256 + 8 + 8(991) = 8192
            { L_      , 992   , 1024      },
            { L_      , 16350 , 1024      },
            { L_      , 16351 , 1024      }, // 256 + 8 + 8(16351) = 131072
            { L_      , 16352 , 1024      },
            { L_      , 1     , 131072    },
            { L_      , 94    , 131072    },
            { L_      , 95    , 131072    }, // 256 + 8 + 8(95) = 1024
            { L_      , 96    , 131072    },
            { L_      , 990   , 131072    },
            { L_      , 991   , 131072    }, // 256 + 8 + 8(991) = 8192
            { L_      , 992   , 131072    },
            { L_      , 16350 , 131072    },
            { L_      , 16351 , 131072    }, // 256 + 8 + 8(16351) = 131072
            { L_      , 16352 , 131072    }
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int idx = 0; idx < NUM_DATA; ++idx) {
          const int LINE       = DATA[idx].d_lineNum;
          const int ATTRIBUTES = DATA[idx].d_numAttributes;
          const int BUFFER     = DATA[idx].d_bufferSize;

          balxml::NamespaceRegistry namespaces;
          balxml::PrefixStack prefixStack(&namespaces);
          Obj miniReader(BUFFER); Obj& reader = miniReader;

          reader.setPrefixStack(&prefixStack);

          // Generate a XML input string with one element that contains
          // 'ATTRIBUTES' attributes.  For example if 'ATTRIBUTES' == 2, then
          // the XML input will look like this:
          //
          //  <?xml version='1.0' encoding='UTF-8'?>
          //  <!-- RCSId_bascfg_xsd = $Id: $ -->
          //  <xs:schema xmlns:xs='http://www.w3.org/2001/XMLSchema'
          //      elementFormDefault='qualified'
          //      xmlns:bdem='http://bloomberg.com/schemas/bdem'
          //      bdem:package='bascfg'>
          //      <N att='0' att='1'/>
          //  </xs:schema>

          bsl::string xmlStr;
          xmlStr.assign(strXmlStart);
          bsl::string nodeName;
          xmlStr.append("  <N ");
          for (int ll = 0; ll < ATTRIBUTES; ++ll) {
            bsl::stringstream ss;
            ss << "att='" << ll % 10 << "' ";
            nodeName.append(ss.str());
          }
          xmlStr.append(nodeName);
          xmlStr.append("/>\n");
          xmlStr.append(strXmlEnd);

          if (veryVerbose)
              bsl::cout << "\nlength: " << xmlStr.length()
                        << bsl::endl;

          if (veryVeryVerbose) bsl::cout << xmlStr << bsl::endl;

          int rc = reader.open(xmlStr.c_str(), xmlStr.length());
          LOOP_ASSERT(LINE, -1 < rc);

          LOOP_ASSERT(LINE, reader.isOpen());
          LOOP_ASSERT(LINE,
                      reader.nodeType() == balxml::Reader::e_NODE_TYPE_NONE);

          readHeader(reader);
          rc = advancePastWhiteSpace(reader);
          LOOP_ASSERT(LINE, !bsl::strcmp(reader.nodeName(), "N"));
          LOOP_ASSERT(LINE,  reader.numAttributes() == ATTRIBUTES);

          balxml::ElementAttribute elemAttr;

          for (int ll = 0; ll < ATTRIBUTES; ++ll) {
              rc = reader.lookupAttribute(&elemAttr, ll);
              LOOP_ASSERT(LINE,  0 == rc);
              LOOP_ASSERT(LINE, !elemAttr.isNull());
              LOOP_ASSERT(LINE, !bsl::strcmp(elemAttr.localName(), "att"));
              LOOP_ASSERT(LINE,  bsl::atoi(elemAttr.value()) == ll % 10);
          }

          rc = reader.lookupAttribute(&elemAttr, -1);
          LOOP_ASSERT(LINE,  1 == rc);

          rc = reader.lookupAttribute(&elemAttr, ATTRIBUTES);
          LOOP_ASSERT(LINE,  1 == rc);

          rc = reader.lookupAttribute(&elemAttr, ATTRIBUTES+1);
          LOOP_ASSERT(LINE,  1 == rc);

          rc = advancePastWhiteSpace(reader);
          LOOP_ASSERT(LINE,
                      reader.nodeType() ==
                                  balxml::Reader::e_NODE_TYPE_END_ELEMENT);
          LOOP_ASSERT(LINE, !bsl::strcmp(reader.nodeName(), "xs:schema"));

          reader.close();
        }
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // LONG ELEMENT NAMES (STRESS TEST)
        //
        // Concerns:
        //   That balxml::MiniReader operate correctly with xml strings
        //   (buffers) with long node names.  The strings in the test vectors
        //   all contain a *single element* with a name of different lengths.
        //
        // Plan:
        //   Construct a 'balxml::MiniReader' reader capable of buffering
        //   'bufferSize' bytes of an XML document created with long node
        //   names.  For each test vector generate the XML input starting with
        //   the 'strXmlStart' template, add a single node with a name of
        //   'length' to get the desired input size and then close the input
        //   with the 'strXmlEnd' template.  Use the reader to parse the input
        //   and verify that the node and its name are intact.  The
        //   'bufferSize' is used to perturb the testing with different
        //   initial buffer sizes, this will force the reader to operate
        //   differently.  I.e., the reader will need to stop more frequently
        //   to read in input with smaller buffer size while using less
        //   memory, while the exact opposite will be true with larger
        //   'bufferSize's'.
        //
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nLONG ELEMENT NAMES (STRESS TEST)"
                               << "\n================================"
                               << bsl::endl;

        static const struct {
            int d_lineNum;
            int d_length;
            int d_bufferSize;
        } DATA[] = {
            //Line    , length, bufferSize
            { L_      , 1     , 0         },
            { L_      , 94    , 0         },
            { L_      , 95    , 0         }, // 256 + 8 + 8(95) = 1024
            { L_      , 96    , 0         },
            { L_      , 990   , 0         },
            { L_      , 991   , 0         }, // 256 + 8 + 8(991) = 8192
            { L_      , 992   , 0         },
            { L_      , 16350 , 0         },
            { L_      , 16351 , 0         }, // 256 + 8 + 8(16351) = 131072
            { L_      , 16352 , 0         },
            { L_      , 1     , 1024      },
            { L_      , 94    , 1024      },
            { L_      , 95    , 1024      }, // 256 + 8 + 8(95) = 1024
            { L_      , 96    , 1024      },
            { L_      , 990   , 1024      },
            { L_      , 991   , 1024      }, // 256 + 8 + 8(991) = 8192
            { L_      , 992   , 1024      },
            { L_      , 16350 , 1024      },
            { L_      , 16351 , 1024      }, // 256 + 8 + 8(16351) = 131072
            { L_      , 16352 , 1024      },
            { L_      , 1     , 131072    },
            { L_      , 94    , 131072    },
            { L_      , 95    , 131072    }, // 256 + 8 + 8(95) = 1024
            { L_      , 96    , 131072    },
            { L_      , 990   , 131072    },
            { L_      , 991   , 131072    }, // 256 + 8 + 8(991) = 8192
            { L_      , 992   , 131072    },
            { L_      , 16350 , 131072    },
            { L_      , 16351 , 131072    }, // 256 + 8 + 8(16351) = 131072
            { L_      , 16352 , 131072    }
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int idx = 0; idx < NUM_DATA; ++idx) {
          const int LINE   = DATA[idx].d_lineNum;
          const int LENGTH = DATA[idx].d_length;
          const int BUFFER = DATA[idx].d_bufferSize;

          balxml::NamespaceRegistry namespaces;
          balxml::PrefixStack prefixStack(&namespaces);
          Obj miniReader(BUFFER); Obj& reader = miniReader;

          reader.setPrefixStack(&prefixStack);

          // Generate a XML input string with one element where the element's
          // name has 'LENGTH' length.  For example if 'LENGTH' == 2, then the
          // XML input will look like this:
          //
          //  <?xml version='1.0' encoding='UTF-8'?>
          //  <!-- RCSId_bascfg_xsd = $Id: $ -->
          //  <xs:schema xmlns:xs='http://www.w3.org/2001/XMLSchema'
          //      elementFormDefault='qualified'
          //      xmlns:bdem='http://bloomberg.com/schemas/bdem'
          //      bdem:package='bascfg'>
          //      <longNamelongName/>
          // < /xs:schema>

          bsl::string xmlStr;
          xmlStr.assign(strXmlStart);
          bsl::string nodeName;
          xmlStr.append("    <");
          for (int ll = 0; ll < LENGTH; ++ll) {
            nodeName.append("longName");
          }
          xmlStr.append(nodeName);
          xmlStr.append("/>\n");
          xmlStr.append(strXmlEnd);

          if (veryVerbose)
              bsl::cout << "\nlength: " << xmlStr.length()
                        << bsl::endl;

          if (veryVeryVerbose) bsl::cout << xmlStr << bsl::endl;

          int rc = reader.open(xmlStr.c_str(), xmlStr.length());
          LOOP_ASSERT(LINE, -1 < rc);

          LOOP_ASSERT(LINE,  reader.isOpen());
          LOOP_ASSERT(LINE,  reader.nodeType()
                             == balxml::Reader::e_NODE_TYPE_NONE);

          readHeader(reader);
          rc = advancePastWhiteSpace(reader);
          LOOP_ASSERT(LINE, reader.nodeName() == nodeName);

          rc = advancePastWhiteSpace(reader);
          LOOP_ASSERT(LINE,  reader.nodeType()
                             == balxml::Reader::e_NODE_TYPE_END_ELEMENT);
          LOOP_ASSERT(LINE, !bsl::strcmp(reader.nodeName(), "xs:schema"));

          reader.close();
        }
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // MANY ELEMENTS, BREADTH AND DEPTH (STRESS TEST)
        //
        // Concerns:
        //   That balxml::MiniReader operate correctly with xml strings
        //   (buffers) of different lengths.  The strings in the test vectors
        //   will contain elements of different breadth and depth.
        //
        // Plan:
        //   Construct a 'balxml::MiniReader' reader capable of buffering
        //   'bufferSize' bytes of an XML document created with the 'ggg'
        //   function.  The XML documents created with the 'ggg' function will
        //   be of lengths and depths that force 'pointer rebase' within the
        //   'balxml::MiniReader'.  The 'bufferSize' is used to perturb the
        //   testing with different initial buffer sizes, this will force the
        //   reader to operate differently.  I.e., the reader will need to stop
        //   more frequently to read in input with smaller buffer size while
        //   using less memory, while the exact opposite will be true with
        //   larger 'bufferSize's'.
        //
        // --------------------------------------------------------------------

        if (verbose)
            bsl::cout << "\nMANY ELEMENTS, BREADTH AND DEPTH (STRESS TEST)"
                      << "\n=============================================="
                      << bsl::endl;

        static const struct {
            int d_lineNum;
            int d_numNodes;
            int d_depth;
            int d_numBytes;
            int d_bufferSize;
        } DATA[] = {
            //           1
            //          ___
            //          \                                                  .
            // bytes = (/__  24*depth) + (153*nodes) + (155*depth) + 256
            //         depth
            //
            //Line    , nodes    , depth    , bytes   , bufferSize
            { L_      , 0        , 0        , 256     , 0         },
            { L_      , 1        , 0        , 409     , 0         },
            { L_      , 6        , 0        , 1174    , 0         },
            { L_      , 7        , 0        , 1327    , 0         },
            { L_      , 66       , 0        , 10354   , 0         },
            { L_      , 67       , 0        , 10507   , 0         },
            { L_      , 1099     , 0        , 168403  , 0         },
            { L_      , 1100     , 0        , 168556  , 0         },
            { L_      , 2200     , 0        , 336856  , 0         },
            { L_      , 1        , 1        , 588     , 0         },
            { L_      , 1        , 5        , 1544    , 0         },
            { L_      , 1        , 22       , 9891    , 0         },
            { L_      , 1        , 23       , 10598   , 0         },
            { L_      , 1        , 107      , 155666  , 0         },
            { L_      , 1        , 108      , 158413  , 0         },
            { L_      , 1        , 1024     , 12754329, 0         },
            { L_      , 2        , 1        , 920     , 0         },
            { L_      , 2        , 2        , 1326    , 0         },
            { L_      , 6        , 6        , 9778    , 0         },
            { L_      , 7        , 7        , 13626   , 0         },
            { L_      , 19       , 19       , 145758  , 0         },
            { L_      , 20       , 20       , 166116  , 0         },
            { L_      , 0        , 0        , 256     , 1024      },
            { L_      , 1        , 0        , 409     , 1024      },
            { L_      , 6        , 0        , 1174    , 1024      },
            { L_      , 7        , 0        , 1327    , 1024      },
            { L_      , 66       , 0        , 10354   , 1024      },
            { L_      , 67       , 0        , 10507   , 1024      },
            { L_      , 1099     , 0        , 168403  , 1024      },
            { L_      , 1100     , 0        , 168556  , 1024      },
            { L_      , 2200     , 0        , 336856  , 1024      },
            { L_      , 1        , 1        , 588     , 1024      },
            { L_      , 1        , 5        , 1544    , 1024      },
            { L_      , 1        , 22       , 9891    , 1024      },
            { L_      , 1        , 23       , 10598   , 1024      },
            { L_      , 1        , 107      , 155666  , 1024      },
            { L_      , 1        , 108      , 158413  , 1024      },
            { L_      , 1        , 1024     , 12754329, 1024      },
            { L_      , 2        , 1        , 920     , 1024      },
            { L_      , 2        , 2        , 1326    , 1024      },
            { L_      , 6        , 6        , 9778    , 1024      },
            { L_      , 7        , 7        , 13626   , 1024      },
            { L_      , 19       , 19       , 145758  , 1024      },
            { L_      , 20       , 20       , 166116  , 1024      },
            { L_      , 0        , 0        , 256     , 131072    },
            { L_      , 1        , 0        , 409     , 131072    },
            { L_      , 6        , 0        , 1174    , 131072    },
            { L_      , 7        , 0        , 1327    , 131072    },
            { L_      , 66       , 0        , 10354   , 131072    },
            { L_      , 67       , 0        , 10507   , 131072    },
            { L_      , 1099     , 0        , 168403  , 131072    },
            { L_      , 1100     , 0        , 168556  , 131072    },
            { L_      , 2200     , 0        , 336856  , 131072    },
            { L_      , 1        , 1        , 588     , 131072    },
            { L_      , 1        , 5        , 1544    , 131072    },
            { L_      , 1        , 22       , 9891    , 131072    },
            { L_      , 1        , 23       , 10598   , 131072    },
            { L_      , 1        , 107      , 155666  , 131072    },
            { L_      , 1        , 108      , 158413  , 131072    },
            { L_      , 1        , 1024     , 12754329, 131072    },
            { L_      , 2        , 1        , 920     , 131072    },
            { L_      , 2        , 2        , 1326    , 131072    },
            { L_      , 6        , 6        , 9778    , 131072    },
            { L_      , 7        , 7        , 13626   , 131072    },
            { L_      , 19       , 19       , 145758  , 131072    },
            { L_      , 20       , 20       , 166116  , 131072    }
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int idx = 0; idx < NUM_DATA; ++idx) {
          const int LINE = DATA[idx].d_lineNum;
          const int NODES = DATA[idx].d_numNodes;
          const int DEPTH = DATA[idx].d_depth;
          const int BYTES = DATA[idx].d_numBytes;
          const int BUFFER = DATA[idx].d_bufferSize;

          balxml::NamespaceRegistry namespaces;
          balxml::PrefixStack prefixStack(&namespaces);
          Obj miniReader(BUFFER); Obj& reader = miniReader;

          reader.setPrefixStack(&prefixStack);

          bsl::string xmlStr;
          ggg(xmlStr, NODES, DEPTH);

          // Testing to see if the 'BYTES' from the test vector match the
          // calculation described on the comment above.
          int bytes = 0;
          for (int sumIndex = 1; sumIndex <= DEPTH; ++sumIndex) {
            bytes += 24 * sumIndex;
          }
          bytes *= NODES;
          bytes += 153 * NODES;
          bytes += (155 * DEPTH) * NODES;
          bytes += 256;

          if (veryVerbose)
              bsl::cout << "\nnodes: " << NODES
                        << " depth: " << DEPTH
                        << " length: " << xmlStr.length()
                        << " bytes: " << bytes
                        << bsl::endl;

          int len = (int) xmlStr.length();
          //LOOP3_ASSERT(LINE, bytes, BYTES, bytes == BYTES);
          LOOP3_ASSERT(LINE, len, bytes, len == bytes);
          LOOP3_ASSERT(LINE, len, BYTES, len == BYTES);

          if (veryVeryVerbose) bsl::cout << xmlStr << bsl::endl;

          int rc = reader.open(xmlStr.c_str(), xmlStr.length());
          LOOP_ASSERT(LINE, -1 < rc);

          LOOP_ASSERT(LINE,  reader.isOpen());
          LOOP_ASSERT(LINE,
                      reader.nodeType() == balxml::Reader::e_NODE_TYPE_NONE);

          readHeader(reader);
          readNodes(reader, 0, NODES, 0, DEPTH);

          rc = advancePastWhiteSpace(reader);
          LOOP_ASSERT(LINE,
               reader.nodeType()==balxml::Reader::e_NODE_TYPE_END_ELEMENT);
          LOOP_ASSERT(LINE, !bsl::strcmp(reader.nodeName(), "xs:schema"));

          reader.close();
        }
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING 'advanceToNextNode' with bad inputs
        //
        // Plan:
        //  Create and open a 'balxml::MiniReader' with strings containing
        //  invalid inputs.  Assert that calls to advanceToNextNode fail with
        //  the proper errors.
        //
        // Testing:
        //  int advanceToNextNode();
        //  balxml::ErrorInfo& errorInfo();
        // --------------------------------------------------------------------
        if (verbose) bsl::cout << bsl::endl
            << "Testing 'bad inputs'"   << bsl::endl
            << "====================\n" << bsl::endl;

        if (verbose)
          bsl::cout << "Testing 'advanceToNextNode'"   << bsl::endl
                    << "- - - - - - - - - - - - - -\n" << bsl::endl;

        if (verbose) bsl::cout << "Bad closing tag."   << bsl::endl
                               << "-  -  -  -  -  -\n" << bsl::endl;
        {
          static const char xmlStr[] =
            "<?xml version='1.0' encoding='UTF-8'?>\n"
            "<!-- RCSId_bascfg_xsd = \"$Id: $\" -->\n"
            "<xs:schema xmlns:xs='http://www.w3.org/2001/XMLSchema'\n"
            "    elementFormDefault='qualified'\n"
            "    xmlns:bdem='http://bloomberg.com/schemas/bdem'\n"
            "    bdem:package='bascfg'>\n"
            "<Node0>\n"
            "    <Element1>element1</Element1>\n"
            "    <Element2>element2</Element2>\n"
            "    <Element3>element3</Element3>\n"
            "    <![CDATA[&lt;123&#240;&gt;]]>\n"
            "</Node0>\n"
            "</xs:schemxxxxxxx>";

          if (veryVerbose) { P(xmlStr); }

          balxml::NamespaceRegistry namespaces;
          balxml::PrefixStack prefixStack(&namespaces);
          Obj miniReader; Obj& reader = miniReader;

          reader.setPrefixStack(&prefixStack);

          int rc = reader.open(xmlStr, bsl::strlen(xmlStr));
          ASSERT(-1 < rc);

          ASSERT( reader.isOpen());
          ASSERT( reader.nodeType() == balxml::Reader::e_NODE_TYPE_NONE);

          readHeader(reader);
          readNodes(reader, 0, 1, 0, 0);

          rc = advancePastWhiteSpace(reader);
          ASSERT(0 > rc);
          const balxml::ErrorInfo& errorInfo = reader.errorInfo();
          ASSERT(errorInfo.isError());
          ASSERT(13 == errorInfo.lineNumber());
          ASSERT(19 == errorInfo.columnNumber());
          ASSERT(bsl::strstr(errorInfo.message().c_str(), "tag mismatch"));

          reader.close();
        }

        if (verbose) bsl::cout << "No closing tag."    << bsl::endl
                               << "-  -  -  -  -  -\n" << bsl::endl;
        {
          static const char xmlStr[] =
            "<?xml version='1.0' encoding='UTF-8'?>\n"
            "<!-- RCSId_bascfg_xsd = \"$Id: $\" -->\n"
            "<xs:schema xmlns:xs='http://www.w3.org/2001/XMLSchema'\n"
            "    elementFormDefault='qualified'\n"
            "    xmlns:bdem='http://bloomberg.com/schemas/bdem'\n"
            "    bdem:package='bascfg'>\n"
            "<Node0>\n"
            "    <Element1>element1</Element1>\n"
            "    <Element2>element2</Element2>\n"
            "    <Element3>element3</Element3>\n"
            "    <![CDATA[&lt;123&#240;&gt;]]>\n"
            "</Node0>\n";

          if (veryVerbose) { P(xmlStr); }

          balxml::NamespaceRegistry namespaces;
          balxml::PrefixStack prefixStack(&namespaces);
          Obj miniReader; Obj& reader = miniReader;

          reader.setPrefixStack(&prefixStack);

          int rc = reader.open(xmlStr, bsl::strlen(xmlStr));
          ASSERT(-1 < rc);

          ASSERT( reader.isOpen());
          ASSERT( reader.nodeType() == balxml::Reader::e_NODE_TYPE_NONE);

          readHeader(reader);
          readNodes(reader, 0, 1, 0, 0);

          rc = advancePastWhiteSpace(reader);
          ASSERT(0 > rc);
          const balxml::ErrorInfo& errorInfo = reader.errorInfo();
          ASSERT(errorInfo.isError());
          ASSERT(13 == errorInfo.lineNumber());
          ASSERT(1 == errorInfo.columnNumber());
          ASSERT(bsl::strstr(errorInfo.message().c_str(), "No End Element"));

          reader.close();
        }

        if (verbose) bsl::cout << "Missing closing quote on attributes."
                               << bsl::endl
                               << "-  -  -  -  -  -  -  -  -  -  -  -  -\n"
                               << bsl::endl;
        {
          // closing quote is missing on the xmlns:bdem attribute within the
          // xs:schema element
          static const char xmlStr[] =
            "<?xml version='1.0' encoding='UTF-8'?>\n"
            "<!-- RCSId_bascfg_xsd = \"$Id: $\" -->\n"
            "<xs:schema xmlns:xs='http://www.w3.org/2001/XMLSchema'\n"
            "    elementFormDefault='qualified'\n"
            "    xmlns:bdem='http://bloomberg.com/schemas/bdem\n"
            "    bdem:package='bascfg'>\n"
            "<Node0>\n"
            "    <Element1>element1</Element1>\n"
            "    <Element2>element2</Element2>\n"
            "    <Element3>element3</Element3>\n"
            "    <![CDATA[&lt;123&#240;&gt;]]>\n"
            "</Node0>\n"
            "</xs:schema>";

          if (veryVerbose) { P(xmlStr); }

          balxml::NamespaceRegistry namespaces;
          balxml::PrefixStack prefixStack(&namespaces);
          Obj miniReader; Obj& reader = miniReader;

          reader.setPrefixStack(&prefixStack);

          int rc = reader.open(xmlStr, bsl::strlen(xmlStr));
          ASSERT(-1 < rc);

          ASSERT( reader.isOpen());
          ASSERT( reader.nodeType() == balxml::Reader::e_NODE_TYPE_NONE);

          rc = advancePastWhiteSpace(reader);
          LOOP_ASSERT(reader.nodeType(),
                      reader.nodeType() ==
                      balxml::Reader::e_NODE_TYPE_XML_DECLARATION);
          LOOP_ASSERT(reader.nodeName(),
                      !bsl::strcmp(reader.nodeName(), "xml"));
          ASSERT( reader.nodeHasValue());
          ASSERT(!bsl::strcmp(reader.nodeValue(),
                              "version='1.0' encoding='UTF-8'"));

          rc = advancePastWhiteSpace(reader);
          ASSERT( reader.nodeType() ==
                                      balxml::Reader::e_NODE_TYPE_COMMENT);
          ASSERT(!bsl::strcmp(reader.nodeValue(),
                      " RCSId_bascfg_xsd = \"$Id: $\" "));

          rc = advancePastWhiteSpace(reader);
          ASSERT( reader.nodeType() ==
                                      balxml::Reader::e_NODE_TYPE_ELEMENT);
          ASSERT(!bsl::strcmp(reader.nodeName(), "xs:schema"));
          ASSERT(0 > rc);
          const balxml::ErrorInfo& errorInfo = reader.errorInfo();
          LOOP_ASSERT(errorInfo.lineNumber(), 6 == errorInfo.lineNumber());
          LOOP_ASSERT(errorInfo.columnNumber(),
                      26 == errorInfo.columnNumber());
          LOOP_ASSERT(errorInfo.message(),
                      npos != errorInfo.message().find("No space"));

          reader.close();
        }

        if (verbose) bsl::cout << "Missing '=' attributes."
                               << bsl::endl
                               << "-  -  -  -  -  -  -  -  -  -  -  -  -\n"
                               << bsl::endl;
        {
          // closing quote is missing on the xmlns:bdem attribute within the
          // xs:schema element
          static const char xmlStr[] =
            "<xs:schema xmlns:xs='http://www.w3.org/2001/XMLSchema'\n"
            "    elementFormDefault='qualified'\n"
            "    xmlns:bdem 'http://bloomberg.com/schemas/bdem'\n"
            "    bdem:package='bascfg'>\n"
            "<Node0>\n"
            "    <Element1>element1</Element1>\n"
            "    <Element2>element2</Element2>\n"
            "    <Element3>element3</Element3>\n"
            "    <![CDATA[&lt;123&#240;&gt;]]>\n"
            "</Node0>\n"
            "</xs:schema>";

          if (veryVerbose) { P(xmlStr); }

          balxml::NamespaceRegistry namespaces;
          balxml::PrefixStack prefixStack(&namespaces);
          Obj miniReader; Obj& reader = miniReader;

          reader.setPrefixStack(&prefixStack);

          int rc = reader.open(xmlStr, bsl::strlen(xmlStr));
          ASSERT(-1 < rc);

          ASSERT( reader.isOpen());
          ASSERT( reader.nodeType() == balxml::Reader::e_NODE_TYPE_NONE);

          rc = advancePastWhiteSpace(reader);
          ASSERT( reader.nodeType() ==
                                      balxml::Reader::e_NODE_TYPE_ELEMENT);
          ASSERT(!bsl::strcmp(reader.nodeName(), "xs:schema"));
          LOOP_ASSERT(rc, 0 > rc);
          const balxml::ErrorInfo& errorInfo = reader.errorInfo();
          LOOP_ASSERT(errorInfo.lineNumber(), 3  == errorInfo.lineNumber());
          LOOP_ASSERT(errorInfo.columnNumber(),
                      16 == errorInfo.columnNumber());
          LOOP_ASSERT(errorInfo.message(),
                      npos != errorInfo.message().find("No '='"));

          reader.close();
        }

        if (verbose) bsl::cout << "Mismatched Closing Quote on Attributes"
                               << bsl::endl
                               << "-  -  -  -  -  -  -  -  -  -  -  -  -  -\n"
                               << bsl::endl;
        if (verbose) bsl::cout << "Attribute starts with ' and ends with \""
                               << bsl::endl
                               << "-   -   -   -   -   -   -   -   -   -   -\n"
                               << bsl::endl;
        {
          // Closing quote is missing on the xmlns:bdem attribute within the
          // xs:schema element.  Start with ' end with ".
          static const char xmlStr[] =
            "<?xml version='1.0' encoding='UTF-8'?>\n"
            "<!-- RCSId_bascfg_xsd = \"$Id: $\" -->\n"
            "<xs:schema xmlns:xs='http://www.w3.org/2001/XMLSchema'\n"
            "    elementFormDefault='qualified'\n"
            "    xmlns:bdem='http://bloomberg.com/schemas/bdem\"\n"
            "    bdem:package='bascfg'>\n"
            "<Node0>\n"
            "    <Element1>element1</Element1>\n"
            "    <Element2>element2</Element2>\n"
            "    <Element3>element3</Element3>\n"
            "    <![CDATA[&lt;123&#240;&gt;]]>\n"
            "</Node0>\n"
            "</xs:schema>";

          if (veryVerbose) { P(xmlStr); }

          balxml::NamespaceRegistry namespaces;
          balxml::PrefixStack prefixStack(&namespaces);
          Obj miniReader; Obj& reader = miniReader;

          reader.setPrefixStack(&prefixStack);

          int rc = reader.open(xmlStr, bsl::strlen(xmlStr));
          ASSERT(-1 < rc);

          ASSERT( reader.isOpen());
          ASSERT( reader.nodeType() == balxml::Reader::e_NODE_TYPE_NONE);

          rc = advancePastWhiteSpace(reader);
          ASSERT( reader.nodeType()
                  == balxml::Reader::e_NODE_TYPE_XML_DECLARATION);
          ASSERT(!bsl::strcmp(reader.nodeName(), "xml"));
          ASSERT( reader.nodeHasValue());
          ASSERT(!bsl::strcmp(reader.nodeValue(),
                              "version='1.0' encoding='UTF-8'"));

          rc = advancePastWhiteSpace(reader);
          ASSERT( reader.nodeType() ==
                                      balxml::Reader::e_NODE_TYPE_COMMENT);
          ASSERT(!bsl::strcmp(reader.nodeValue(),
                              " RCSId_bascfg_xsd = \"$Id: $\" "));

          rc = advancePastWhiteSpace(reader);
          ASSERT( reader.nodeType() ==
                                      balxml::Reader::e_NODE_TYPE_ELEMENT);
          ASSERT(!bsl::strcmp(reader.nodeName(), "xs:schema"));
          ASSERT(0 > rc);
          const balxml::ErrorInfo& errorInfo = reader.errorInfo();
          ASSERT(6  == errorInfo.lineNumber());
          ASSERT(26 == errorInfo.columnNumber());
          LOOP_ASSERT(errorInfo.message(),
                      npos != errorInfo.message().find("No space"));

          reader.close();
        }

        if (verbose) bsl::cout << "Attribute starts with \" and ends with '"
                               << bsl::endl
                               << "-   -   -   -   -   -   -   -   -   -   -\n"
                               << bsl::endl;
        {
          // Closing quote is missing on the xmlns:bdem attribute within the
          // xs:schema element.  Start with ' end with ".
          static const char xmlStr[] =
            "<?xml version='1.0' encoding='UTF-8'?>\n"
            "<!-- RCSId_bascfg_xsd = \"$Id: $\" -->\n"
            "<xs:schema xmlns:xs='http://www.w3.org/2001/XMLSchema'\n"
            "    elementFormDefault='qualified'\n"
            "    xmlns:bdem='http://bloomberg.com/schemas/bdem\"\n"
            "    bdem:package='bascfg'>\n"
            "<Node0>\n"
            "    <Element1>element1</Element1>\n"
            "    <Element2>element2</Element2>\n"
            "    <Element3>element3</Element3>\n"
            "    <![CDATA[&lt;123&#240;&gt;]]>\n"
            "</Node0>\n"
            "</xs:schema>";

          if (veryVerbose) { P(xmlStr); }

          balxml::NamespaceRegistry namespaces;
          balxml::PrefixStack prefixStack(&namespaces);
          Obj miniReader; Obj& reader = miniReader;

          reader.setPrefixStack(&prefixStack);

          int rc = reader.open(xmlStr, bsl::strlen(xmlStr));
          ASSERT(-1 < rc);

          ASSERT( reader.isOpen());
          ASSERT( reader.nodeType() == balxml::Reader::e_NODE_TYPE_NONE);

          rc = advancePastWhiteSpace(reader);
          ASSERT( reader.nodeType()
                  == balxml::Reader::e_NODE_TYPE_XML_DECLARATION);
          ASSERT(!bsl::strcmp(reader.nodeName(), "xml"));
          ASSERT( reader.nodeHasValue());
          ASSERT(!bsl::strcmp(reader.nodeValue(),
                              "version='1.0' encoding='UTF-8'"));

          rc = advancePastWhiteSpace(reader);
          ASSERT( reader.nodeType() ==
                                      balxml::Reader::e_NODE_TYPE_COMMENT);
          ASSERT(!bsl::strcmp(reader.nodeValue(),
                      " RCSId_bascfg_xsd = \"$Id: $\" "));

          rc = advancePastWhiteSpace(reader);
          ASSERT( reader.nodeType() ==
                                      balxml::Reader::e_NODE_TYPE_ELEMENT);
          ASSERT(!bsl::strcmp(reader.nodeName(), "xs:schema"));
          ASSERT(0 > rc);
          const balxml::ErrorInfo& errorInfo = reader.errorInfo();
          ASSERT(6  == errorInfo.lineNumber());
          ASSERT(26 == errorInfo.columnNumber());
          LOOP_ASSERT(errorInfo.message(),
                      npos != errorInfo.message().find("No space"));

          reader.close();
        }

        if (verbose)
          bsl::cout << "Illegal Attribute Name"
                    << bsl::endl
                    << "-  -  -  -  -  -  -  -  -\n"
                    << bsl::endl;
        {
          // The bdem:pac=kage [sic] attribute with in the xs:schema element
          // is invalid.
          static const char xmlStr[] =
            "<?xml version='1.0' encoding='UTF-8'?>\n"
            "<!-- RCSId_bascfg_xsd = \"$Id: $\" -->\n"
            "<xs:schema xmlns:xs='http://www.w3.org/2001/XMLSchema'\n"
            "    elementFormDefault='qualified'\n"
            "    xmlns:bdem='http://bloomberg.com/schemas/bdem'\n"
            "    bdem:pac=kage='bascfg'>\n"
            "<Node0>\n"
            "    <Element1>element1</Element1>\n"
            "    <Element2>element2</Element2>\n"
            "    <Element3>element3</Element3>\n"
            "    <![CDATA[&lt;123&#240;&gt;]]>\n"
            "</Node0>\n"
            "</xs:schema>";

          if (veryVerbose) { P(xmlStr); }

          balxml::NamespaceRegistry namespaces;
          balxml::PrefixStack prefixStack(&namespaces);
          Obj miniReader; Obj& reader = miniReader;

          reader.setPrefixStack(&prefixStack);

          int rc = reader.open(xmlStr, bsl::strlen(xmlStr));
          ASSERT(-1 < rc);

          ASSERT( reader.isOpen());
          ASSERT( reader.nodeType() == balxml::Reader::e_NODE_TYPE_NONE);

          rc = advancePastWhiteSpace(reader);
          ASSERT( reader.nodeType()
                  == balxml::Reader::e_NODE_TYPE_XML_DECLARATION);
          ASSERT(!bsl::strcmp(reader.nodeName(), "xml"));
          ASSERT( reader.nodeHasValue());
          ASSERT(!bsl::strcmp(reader.nodeValue(),
                              "version='1.0' encoding='UTF-8'"));

          rc = advancePastWhiteSpace(reader);
          ASSERT( reader.nodeType() ==
                                      balxml::Reader::e_NODE_TYPE_COMMENT);
          ASSERT(!bsl::strcmp(reader.nodeValue(),
                      " RCSId_bascfg_xsd = \"$Id: $\" "));

          rc = advancePastWhiteSpace(reader);
          ASSERT( reader.nodeType() ==
                                      balxml::Reader::e_NODE_TYPE_ELEMENT);
          ASSERT(!bsl::strcmp(reader.nodeName(), "xs:schema"));
          ASSERT(0 > rc);
          const balxml::ErrorInfo& errorInfo = reader.errorInfo();
          ASSERT(6  == errorInfo.lineNumber());
          ASSERT(15 == errorInfo.columnNumber());
          ASSERT(bsl::strstr(errorInfo.message().c_str(), "Attribute value"));

          reader.close();
        }

        if (verbose)
          bsl::cout << "Illegal Attribute Value"
                    << bsl::endl
                    << "-  -  -  -  -  -  -  -  -\n"
                    << bsl::endl;
        {
          // The bdem:package attribute with in the xs:schema element has an
          // invalid value.
          static const char xmlStr[] =
            "<?xml version='1.0' encoding='UTF-8'?>\n"
            "<!-- RCSId_bascfg_xsd = \"$Id: $\" -->\n"
            "<xs:schema xmlns:xs='http://www.w3.org/2001/XMLSchema'\n"
            "    elementFormDefault='qualified'\n"
            "    xmlns:bdem='http://bloomberg.com/schemas/bdem'\n"
            "    bdem:package='bas'cfg'>\n"
            "<Node0>\n"
            "    <Element1>element1</Element1>\n"
            "    <Element2>element2</Element2>\n"
            "    <Element3>element3</Element3>\n"
            "    <![CDATA[&lt;123&#240;&gt;]]>\n"
            "</Node0>\n"
            "</xs:schema>";

          if (veryVerbose) { P(xmlStr); }

          balxml::NamespaceRegistry namespaces;
          balxml::PrefixStack prefixStack(&namespaces);
          Obj miniReader; Obj& reader = miniReader;

          reader.setPrefixStack(&prefixStack);

          int rc = reader.open(xmlStr, bsl::strlen(xmlStr));
          ASSERT(-1 < rc);

          ASSERT( reader.isOpen());
          ASSERT( reader.nodeType() == balxml::Reader::e_NODE_TYPE_NONE);

          rc = advancePastWhiteSpace(reader);
          ASSERT( reader.nodeType()
                  == balxml::Reader::e_NODE_TYPE_XML_DECLARATION);
          ASSERT(!bsl::strcmp(reader.nodeName(), "xml"));
          ASSERT( reader.nodeHasValue());
          ASSERT(!bsl::strcmp(reader.nodeValue(),
                              "version='1.0' encoding='UTF-8'"));

          rc = advancePastWhiteSpace(reader);
          ASSERT( reader.nodeType() ==
                                      balxml::Reader::e_NODE_TYPE_COMMENT);
          ASSERT(!bsl::strcmp(reader.nodeValue(),
                      " RCSId_bascfg_xsd = \"$Id: $\" "));

          rc = advancePastWhiteSpace(reader);
          ASSERT( reader.nodeType() ==
                                      balxml::Reader::e_NODE_TYPE_ELEMENT);
          ASSERT(!bsl::strcmp(reader.nodeName(), "xs:schema"));
          ASSERT(0 > rc);
          const balxml::ErrorInfo& errorInfo = reader.errorInfo();
          LOOP_ASSERT(errorInfo.lineNumber(), 6  == errorInfo.lineNumber());
          LOOP_ASSERT(errorInfo.columnNumber(),
                      27 == errorInfo.columnNumber());
          LOOP_ASSERT(errorInfo.message(),
                      npos != errorInfo.message().find("No space"));

          reader.close();
        }

        if (verbose)
          bsl::cout << "Illegal Element Name"
                    << bsl::endl
                    << "-  -  -  -  -  -  -  -\n"
                    << bsl::endl;
        {
          // The <xs:sc>hema [sic] element is invalid.
          static const char xmlStr[] =
            "<?xml version='1.0' encoding='UTF-8'?>\n"
            "<!-- RCSId_bascfg_xsd = \"$Id: $\" -->\n"
            "<xs:sc>hema xmlns:xs='http://www.w3.org/2001/XMLSchema'\n"
            "    elementFormDefault='qualified'\n"
            "    xmlns:bdem='http://bloomberg.com/schemas/bdem'\n"
            "    bdem:package='bascfg'>\n"
            "<Node0>\n"
            "    <Element1>element1</Element1>\n"
            "    <Element2>element2</Element2>\n"
            "    <Element3>element3</Element3>\n"
            "    <![CDATA[&lt;123&#240;&gt;]]>\n"
            "</Node0>\n"
            "</xs:schema>";

          if (veryVerbose) { P(xmlStr); }

          balxml::NamespaceRegistry namespaces;
          balxml::PrefixStack prefixStack(&namespaces);
          Obj miniReader; Obj& reader = miniReader;

          reader.setPrefixStack(&prefixStack);

          int rc = reader.open(xmlStr, bsl::strlen(xmlStr));
          ASSERT(-1 < rc);

          ASSERT( reader.isOpen());
          ASSERT( reader.nodeType() == balxml::Reader::e_NODE_TYPE_NONE);

          rc = advancePastWhiteSpace(reader);
          ASSERT( reader.nodeType()
                  == balxml::Reader::e_NODE_TYPE_XML_DECLARATION);
          ASSERT(!bsl::strcmp(reader.nodeName(), "xml"));
          ASSERT( reader.nodeHasValue());
          ASSERT(!bsl::strcmp(reader.nodeValue(),
                              "version='1.0' encoding='UTF-8'"));

          rc = advancePastWhiteSpace(reader);
          ASSERT( reader.nodeType() ==
                                      balxml::Reader::e_NODE_TYPE_COMMENT);
          ASSERT(!bsl::strcmp(reader.nodeValue(),
                      " RCSId_bascfg_xsd = \"$Id: $\" "));

          rc = advancePastWhiteSpace(reader);
          ASSERT( reader.nodeType() ==
                                      balxml::Reader::e_NODE_TYPE_ELEMENT);
          ASSERT(0 > rc);
          const balxml::ErrorInfo& errorInfo = reader.errorInfo();
          ASSERT(3 == errorInfo.lineNumber());
          ASSERT(8 == errorInfo.columnNumber());
          ASSERT(bsl::strstr(errorInfo.message().c_str(),
                             "Undefined namespace"));

          reader.close();
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING GENERATOR FUNCTION 'GGG'
        //
        // Plan:
        //   Test the behavior of 'ggg'.
        //   * Create local strings representing the expected xml "header" and
        //     "footer", assert that the local strings equal the global
        //     'strXmlStart' and 'strXmlEnd'.
        //   * Verify that the string passed to the ggg function is filled in
        //     correctly (implicitly testing the addNodes and addDepth
        //     functions).  Append the local xml header, a "node" and the xml
        //     footer to a local string and compare it to the string passed
        //     into the ggg function.
        //   * Open an 'balxml::MiniReader' with the xml string and pass the
        //     reader to 'readHeader' and 'readNodes'.
        //
        // Testing:
        //   void ggg(bsl::string& xmlStr, int numNodes, int depth);
        //   void addDepth(bsl::string& xmlStr, int currentDepth, int depth);
        //   void addNodes(bsl::string& xmlStr,
        //                 int          currentNode,
        //                 int          numNodes,
        //                 int          currentDepth,
        //                 int          depth);
        //   void readDepth(Obj& reader, int currentDepth, int depth);
        //   void readNodes(Obj& reader,
        //                  int  currentNode,
        //                  int  numNodes,
        //                  int  currentDepth,
        //                  int  depth);
        // --------------------------------------------------------------------
        if (verbose) bsl::cout << bsl::endl
            << "Testing 'ggg' generator functions" << bsl::endl
            << "=================================" << bsl::endl;

        const char *xmlStart =
          "<?xml version='1.0' encoding='UTF-8'?>\n"
          "<!-- RCSId_bascfg_xsd = \"$Id: $\" -->\n"
          "<xs:schema xmlns:xs='http://www.w3.org/2001/XMLSchema'\n"
          "    elementFormDefault='qualified'\n"
          "    xmlns:bdem='http://bloomberg.com/schemas/bdem'\n"
          "    bdem:package='bascfg'>\n";

        const char *xmlNode =
          "<Node0>\n"
          "    <Element1>element1</Element1>\n"
          "    <Element2>element2</Element2>\n"
          "    <Element3>element3</Element3>\n"
          "    <![CDATA[&lt;123&#240;&gt;]]>\n"
          "</Node0>\n";

        const char *xmlEnd = "</xs:schema>";

        // Assert that xml string are valid
        ASSERT(!bsl::strcmp(strXmlStart, xmlStart));
        ASSERT(!bsl::strcmp(strXmlEnd, xmlEnd));

        bsl::string xmlStr(xmlStart);
        xmlStr.append(xmlNode);
        xmlStr.append(xmlEnd);

        bsl::string xmlStrggg;
        ggg(xmlStrggg, 1, 0);

        if (veryVerbose) { P(xmlStr);  P(xmlStrggg); }

        // Assert that 'hard-coded' string and string produced by the ggg
        // function are the same.
        ASSERT(xmlStr == xmlStrggg);

        balxml::NamespaceRegistry namespaces;
        balxml::PrefixStack prefixStack(&namespaces);
        Obj miniReader; Obj& reader = miniReader;

        reader.setPrefixStack(&prefixStack);

        int rc = reader.open(xmlStr.c_str(), xmlStr.length());
        ASSERT(-1 < rc);

        ASSERT( reader.isOpen());
        ASSERT( reader.nodeType() == balxml::Reader::e_NODE_TYPE_NONE);

        // Test read header and readNodes functions.
        readHeader(reader);
        readNodes(reader, 0, 1, 0, 0);

        rc = advancePastWhiteSpace(reader);
        ASSERT( reader.nodeType() ==
                                  balxml::Reader::e_NODE_TYPE_END_ELEMENT);
        ASSERT(!bsl::strcmp(reader.nodeName(), "xs:schema"));

        reader.close();
      } break;
      case 2: {
        //--------------------------------------------------------------------
        // BOOTSTRAP TEST
        //
        // Concerns:
        //   1. The 'balxml::MiniReader's' constructors work properly:
        //      a. The initial value is correct.
        //      b. The constructor is exception neutral w.r.t. memory
        //         allocation.
        //      c. The internal memory management system is hooked up properly
        //         so that *all* internally allocated memory draws from a
        //         user-supplied allocator whenever one is specified.
        //   2. The destructor works properly as implicitly tested in the
        //      various scopes of this test and in the presence of exceptions.
        //
        // Plan:
        //   Create a test object using the constructors: 1) without
        //   exceptions and 2) in the presence of exceptions during memory
        //   allocations using a 'bslma::TestAllocator' and varying its
        //   *allocation* *limit*.
        //
        // Testing:
        //   balxml::MiniReader(bslma::Allocator *bA);
        //   balxml::MiniReader(int bufSize, bslma::Allocator *bA);
        //   ~balxml::MiniReader();
        //--------------------------------------------------------------------

        if (verbose)
          bsl::cout << bsl::endl
                    << "Testing Primary Manipulators" << bsl::endl
                    << "============================" << bsl::endl;

        if (verbose)
          bsl::cout << "\nTesting 'balxml::MiniReader(*bA)' ctor" << bsl::endl;

        if (verbose) bsl::cout << "\tPassing in an allocator." << bsl::endl;
        if (verbose) bsl::cout << "\t\tWith no exceptions." << bsl::endl;
        {
          const int NUM_BLOCKS = testAllocator.numBlocksInUse();
          const int NUM_BYTES  = testAllocator.numBytesInUse();
          {
            Obj mX(&testAllocator);

            ASSERT(NUM_BLOCKS < testAllocator.numBlocksInUse());
            ASSERT(NUM_BYTES  < testAllocator.numBytesInUse());
          }
          ASSERT(NUM_BLOCKS == testAllocator.numBlocksInUse());
          ASSERT(NUM_BYTES  == testAllocator.numBytesInUse());
        }

        if (verbose) bsl::cout << "\t\tWith exceptions." << bsl::endl;
        {
          const int NUM_BLOCKS = testAllocator.numBlocksInUse();
          const int NUM_BYTES  = testAllocator.numBytesInUse();
          BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
            Obj mX(&testAllocator);

            ASSERT(NUM_BLOCKS < testAllocator.numBlocksInUse());
            ASSERT(NUM_BYTES  < testAllocator.numBytesInUse());
          } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
          ASSERT(NUM_BLOCKS == testAllocator.numBlocksInUse());
          ASSERT(NUM_BYTES  == testAllocator.numBytesInUse());
        }

        if (verbose)
          bsl::cout << "\nTesting 'balxml::MiniReader(bufSize, *bA)' ctor"
                    << bsl::endl;

        if (verbose) bsl::cout << "\tPassing in an allocator." << bsl::endl;
        if (verbose) bsl::cout << "\t\tWith no exceptions." << bsl::endl;
        {
          const int NUM_BLOCKS = testAllocator.numBlocksInUse();
          const int NUM_BYTES  = testAllocator.numBytesInUse();
          {
            Obj mX(1024, &testAllocator);

            ASSERT(NUM_BLOCKS < testAllocator.numBlocksInUse());
            ASSERT(NUM_BYTES  < testAllocator.numBytesInUse());
          }
          ASSERT(NUM_BLOCKS == testAllocator.numBlocksInUse());
          ASSERT(NUM_BYTES  == testAllocator.numBytesInUse());
          {
            Obj mX(131072, &testAllocator);

            ASSERT(NUM_BLOCKS < testAllocator.numBlocksInUse());
            ASSERT(NUM_BYTES  < testAllocator.numBytesInUse());
          }
          ASSERT(NUM_BLOCKS == testAllocator.numBlocksInUse());
          ASSERT(NUM_BYTES  == testAllocator.numBytesInUse());
        }

        if (verbose) bsl::cout << "\t\tWith exceptions." << bsl::endl;
        {
          const int NUM_BLOCKS = testAllocator.numBlocksInUse();
          const int NUM_BYTES  = testAllocator.numBytesInUse();
          BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
            Obj mX(1024, &testAllocator);

            ASSERT(NUM_BLOCKS < testAllocator.numBlocksInUse());
            ASSERT(NUM_BYTES  < testAllocator.numBytesInUse());
          } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

          ASSERT(NUM_BLOCKS == testAllocator.numBlocksInUse());
          ASSERT(NUM_BYTES  == testAllocator.numBytesInUse());
          BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
            Obj mX(131072, &testAllocator);

            ASSERT(NUM_BLOCKS < testAllocator.numBlocksInUse());
            ASSERT(NUM_BYTES  < testAllocator.numBytesInUse());
          } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
          ASSERT(NUM_BLOCKS == testAllocator.numBlocksInUse());
          ASSERT(NUM_BYTES  == testAllocator.numBytesInUse());
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //
        // Concerns:
        //   Exercise a broad cross-section of functionality before beginning
        //   testing in earnest.  Probe that functionality systematically and
        //   incrementally to discover basic errors in isolation.
        //
        // Testing:
        //   This test case exercises basic functionality of the
        //   'balxml::MiniReader' component.
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nBREATHING TEST"
                               << "\n==============" << bsl::endl;

        static const char xmlStr[] =
          "<?xml version='1.0' encoding='UTF-8'?>\n"
          "<!-- RCSId_bascfg_xsd = \"$Id: $\" -->\n"
          "<xs:schema xmlns:xs='http://www.w3.org/2001/XMLSchema'\n"
          "    elementFormDefault='qualified'\n"
          "    xmlns:bdem='http://bloomberg.com/schemas/bdem'\n"
          "    bdem:package='bascfg'>\n"
          "<Node0>\n"
          "    <Element1>element1</Element1>\n"
          "    <Element2>element2</Element2>\n"
          "    <Element3>element3</Element3>\n"
          "    <![CDATA[&lt;123&#240;&gt;]]>\n"
          "</Node0>\n"
          "</xs:schema>";

        if (veryVerbose) { P(xmlStr); }

        balxml::NamespaceRegistry namespaces;
        balxml::PrefixStack prefixStack(&namespaces);
        Obj miniReader; Obj& reader = miniReader;

        reader.setPrefixStack(&prefixStack);

        int rc = reader.open(xmlStr, bsl::strlen(xmlStr));
        ASSERT(-1 < rc);

        ASSERT( reader.isOpen());
        ASSERT( reader.nodeType() == balxml::Reader::e_NODE_TYPE_NONE);

        rc = advancePastWhiteSpace(reader);
        //TBD: Mini Reader needs to be fix to pass back the correct encoding.
        //ASSERT(!bsl::strncmp(reader.documentEncoding(), "UTF-8", 5));
        ASSERT( reader.nodeType() ==
                balxml::Reader::e_NODE_TYPE_XML_DECLARATION);
        ASSERT(!bsl::strcmp(reader.nodeName(), "xml"));
        ASSERT( reader.nodeHasValue());
        ASSERT(!bsl::strcmp(reader.nodeValue(),
                    "version='1.0' encoding='UTF-8'"));

        rc = advancePastWhiteSpace(reader);
        ASSERT( reader.nodeType() == balxml::Reader::e_NODE_TYPE_COMMENT);
        ASSERT(!bsl::strcmp(reader.nodeValue(),
                    " RCSId_bascfg_xsd = \"$Id: $\" "));

        rc = advancePastWhiteSpace(reader);
        ASSERT( reader.nodeType() == balxml::Reader::e_NODE_TYPE_ELEMENT);
        ASSERT(!bsl::strcmp(reader.nodeName(), "xs:schema"));

        rc = advancePastWhiteSpace(reader);
        ASSERT( reader.nodeType() == balxml::Reader::e_NODE_TYPE_ELEMENT);
        ASSERT(!bsl::strcmp(reader.nodeName(), "Node0"));

        rc = advancePastWhiteSpace(reader);
        ASSERT( reader.nodeType() == balxml::Reader::e_NODE_TYPE_ELEMENT);
        ASSERT(!bsl::strcmp(reader.nodeName(), "Element1"));
        rc = advancePastWhiteSpace(reader);
        ASSERT( reader.nodeType() == balxml::Reader::e_NODE_TYPE_TEXT);
        ASSERT(!bsl::strcmp(reader.nodeValue(), "element1"));
        rc = advancePastWhiteSpace(reader);
        ASSERT( reader.nodeType() ==
                                  balxml::Reader::e_NODE_TYPE_END_ELEMENT);
        ASSERT(!bsl::strcmp(reader.nodeName(), "Element1"));

        rc = advancePastWhiteSpace(reader);
        ASSERT( reader.nodeType() == balxml::Reader::e_NODE_TYPE_ELEMENT);
        ASSERT(!bsl::strcmp(reader.nodeName(), "Element2"));
        rc = advancePastWhiteSpace(reader);
        ASSERT( reader.nodeType() == balxml::Reader::e_NODE_TYPE_TEXT);
        ASSERT(!bsl::strcmp(reader.nodeValue(), "element2"));
        rc = advancePastWhiteSpace(reader);
        ASSERT( reader.nodeType() ==
                                  balxml::Reader::e_NODE_TYPE_END_ELEMENT);
        ASSERT(!bsl::strcmp(reader.nodeName(), "Element2"));

        rc = advancePastWhiteSpace(reader);
        ASSERT( reader.nodeType() == balxml::Reader::e_NODE_TYPE_ELEMENT);
        ASSERT(!bsl::strcmp(reader.nodeName(), "Element3"));
        rc = advancePastWhiteSpace(reader);
        ASSERT( reader.nodeType() == balxml::Reader::e_NODE_TYPE_TEXT);
        ASSERT(!bsl::strcmp(reader.nodeValue(), "element3"));
        rc = advancePastWhiteSpace(reader);
        ASSERT( reader.nodeType() ==
                                  balxml::Reader::e_NODE_TYPE_END_ELEMENT);
        ASSERT(!bsl::strcmp(reader.nodeName(), "Element3"));

        rc = advancePastWhiteSpace(reader);
        ASSERT( reader.nodeType() == balxml::Reader::e_NODE_TYPE_CDATA);
        ASSERT(!bsl::strcmp(reader.nodeValue(), "&lt;123&#240;&gt;"));

        rc = advancePastWhiteSpace(reader);
        ASSERT( reader.nodeType() ==
                                  balxml::Reader::e_NODE_TYPE_END_ELEMENT);
        ASSERT(!bsl::strcmp(reader.nodeName(), "Node0"));

        rc = advancePastWhiteSpace(reader);
        ASSERT( reader.nodeType() ==
                                  balxml::Reader::e_NODE_TYPE_END_ELEMENT);
        ASSERT(!bsl::strcmp(reader.nodeName(), "xs:schema"));

        reader.close();
      } break;
      case -1: {
        // --------------------------------------------------------------------
        // balxml::MiniReader INTERACTIVE TEST
        //
        // Invoke this test program with argument -1 and redirect input to an
        // xml schema file.  The program will read the xml schema file and
        // print it out.
        //
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nINTERACTIVE TEST"
                               << "\n================" << bsl::endl;

        balxml::NamespaceRegistry namespaces;
        balxml::PrefixStack prefixStack(&namespaces);
        Obj miniReader; Obj& reader = miniReader;

        reader.setPrefixStack(&prefixStack);

        int rc = reader.open(bsl::cin.rdbuf(), "UTF-8");
        ASSERT(-1 < rc);

        rc = parseAndProcess(&reader);
        if (veryVerbose) bsl::cout << "Reader parse: " << rc << bsl::endl;
        reader.close();

      } break;
      default: {
        bsl::cerr << "WARNING: CASE `" << test << "' NOT FOUND." << bsl::endl;
        testStatus = -1;
      }
    }

    if (testStatus > 0)
    {
        bsl::cerr << "Error, non-zero test status = " << testStatus << "."
                  << bsl::endl;
    }
    return testStatus;
}

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
