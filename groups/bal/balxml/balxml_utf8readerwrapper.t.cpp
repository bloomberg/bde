// balxml_utf8readerwrapper.t.cpp                                     -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <balxml_utf8readerwrapper.h>

#include <balxml_errorinfo.h>
#include <balxml_minireader.h>

#include <bdlde_utf8util.h>
#include <bdls_filesystemutil.h>
#include <bdls_osutil.h>
#include <bdls_processutil.h>
#include <bdlsb_fixedmeminstreambuf.h>
#include <bdlsb_fixedmemoutstreambuf.h>

#include <bslim_testutil.h>

#include <bsla_fallthrough.h>
#include <bslma_testallocator.h>
#include <bslma_testallocatorexception.h>
#include <bslmf_assert.h>
#include <bsls_platform.h>
#include <bsls_types.h>

#include <bsl_algorithm.h>   // min
#include <bsl_cstring.h>     // strlen()
#include <bsl_cstdlib.h>     // atoi()
#include <bsl_iostream.h>
#include <bsl_sstream.h>
#include <bsl_fstream.h>
#include <bsl_iomanip.h>
#include <bsl_string.h>

using namespace BloombergLP;
using bsl::cout;
using bsl::cerr;
using bsl::endl;
using bsl::flush;

//=============================================================================
//                                 TEST PLAN
//-----------------------------------------------------------------------------
//                                 Overview
//                                 --------
// The component under test is a mechanism for parsing industry standard XML
// files.
//
//-----------------------------------------------------------------------------
//
// MANIPULATORS
// [ 8] nodeEndPosition()
// [ 8] nodeStartPosition()
//
// [11] numAttributes()
//
// [11] isEmptyElement()
//
// [12] advanceToEndNode()
//
// [13] advanceToEndNodeRaw()
//
// [14] advanceToEndNodeRawBare()
//
// [15] bslma::UsesBslmaAllocator<Obj>
// [17] MiniReader(basicAllocator)
// [17] MiniReader(bufSize, basicAllocator)
// [17] ~MiniReader()
// [17] setPrefixStack(balxml::PrefixStack *prefixes)
// [17] prefixStack()
// [17] open()
// [17] isOpen()
// [17] documentEncoding()
// [17] nodeType()
// [17] nodeName()
// [17] nodeHasValue()
// [17] nodeValue()
// [17] nodeDepth()
// [17] numAttributes()
// [17] isEmptyElement()
// [17] advanceToNextNode()
// [17] lookupAttribute(ElemAtt a, int index)
// [17] lookupAttribute(ElemAtt a, char *qname)
// [17] lookupAttribute(ElemAtt a, char *localname, char *nsUri)
// [17] lookupAttribute(ElemAtt a, char *localname, int nsId)
// [16] open(const char *filename, const char *encoding);
// [16] open(const char *buffer, size_t size);
// [16] open(bsl::streambuf *stream);
// [ 4] close();
//-----------------------------------------------------------------------------
// [-1] INTERACTIVE TEST
// [ 1] BREATHING TEST
// [16] VALID AND INVALID UTF-8 TEST
// [17] USAGE EXAMPLE
//-----------------------------------------------------------------------------

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

#define T2_          (cout << "  ");

#define CHK(X) (X != 0 ? (const char *) X : "(null)")

// ============================================================================
//                   GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------
static int verbose = 0;
static int veryVerbose = 0;
static int veryVeryVerbose = 0;

typedef balxml::Utf8ReaderWrapper  Obj;
typedef balxml::MiniReader         Mini;
typedef bdlde::Utf8Util            Utf8Util;
typedef balxml::ElementAttribute   ElementAttribute;
typedef balxml::NamespaceRegistry  Registry;
typedef bsls::Types::Uint64        Uint64;

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
    Mini mini;
    Obj reader(&mini);

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
        long start = mini.nodeStartPosition();
        long end = mini.nodeEndPosition();

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
XmlEl table91[] = {
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

XmlEl table81[] = {
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


                                // =============
                                // class RandGen
                                // =============

class RandGen {
    // Random number generator using the high-order 32 bits of Donald Knuth's
    // MMIX algorithm.

    bsls::Types::Uint64 d_seed;

  public:
    explicit
    RandGen(int startSeed = 0);
        // Initialize the generator with the specified 'startSeed'.

    unsigned operator()();
        // Return the next random number in the series;
};

// CREATOR
inline
RandGen::RandGen(int startSeed)
: d_seed(startSeed)
{
    (void) (*this)();
    (void) (*this)();
    (void) (*this)();
}

// MANIPULATOR
inline
unsigned RandGen::operator()()
{
    d_seed = d_seed * 6364136223846793005ULL + 1442695040888963407ULL;
    return static_cast<unsigned>(d_seed >> 32);
}

// ----------------------------------------------------------------------------
// Start of usage example, extract to the 'balxml::Utf8ReaderWrapper' header
// file.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Routine Parsing:
/// - - - - - - - - - - - - -
// Utility function to skip past white space.
//..
    int advancePastWhiteSpace(balxml::Reader& reader)
    {
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
// Then, in 'main', we parse an XML string using the UTF-8 reader wrapper:
int usageExample()
{
//
// The following string describes xml for a very simple user directory.  The
// top level element contains one xml namespace attribute, with one embedded
// entry describing a user.  The person's name contains some non-ascii UTF-8.
//..
    static const char TEST_XML_STRING[] =
       "<?xml version='1.0' encoding='UTF-8'?>\n"
       "<directory-entry xmlns:dir='http://bloomberg.com/schemas/directory'>\n"
       "    <name>John Smith\xe7\x8f\x8f</name>\n"
       "    <phone dir:phonetype='cell'>212-318-2000</phone>\n"
       "    <address/>\n"
       "</directory-entry>\n";
//..
// In order to read the XML, we first need to construct a
// 'balxml::NamespaceRegistry' object, a 'balxml::PrefixStack' object, and a
// 'Utf8ReaderWrapper' object.
//..
    balxml::NamespaceRegistry namespaces;
    balxml::PrefixStack prefixStack(&namespaces);
    balxml::MiniReader miniReader;
    balxml::Utf8ReaderWrapper reader(&miniReader);

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
    ASSERT(!bsl::strcmp(reader.nodeValue(), "John Smith\xe7\x8f\x8f"));
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
//..
}

// ============================================================================
//  Let make numElements and numElements static
//  to be accessible out of scope parse () function for future analysis.
//  Since test is single threaded, no MT-issues are here
// ============================================================================
static int test = 0;

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

void addOpenTag(bsl::string& xmlStr, int indent, const char* tag, int n)
{
    bsl::stringstream ss;

    ss << bsl::setw(indent) << "" << "<" << tag << n << ">\n"
       << bsl::setw(indent + 4) << "" << "<Element1>element1</Element1>\n"
       << bsl::setw(indent + 4) << "" << "<Element2>element2</Element2>\n"
       << bsl::setw(indent + 4) << "" << "<Element3>element3</Element3>\n"
       << bsl::setw(indent + 4) << "" << "<![CDATA[&lt;123&#240;&gt;]]>\n";
    // -----------------------------------123456789012345678901234567890

    xmlStr.append(ss.str());
}

void addCloseTag(bsl::string& xmlStr, int indent, const char* tag, int n)
{
    bsl::stringstream ss;

    ss << bsl::setw(indent) << "" << "</" << tag << n << ">\n";

    xmlStr.append(ss.str());
}

// Recursively add nested nodes from 'currentDepth' to 'depth' to 'xmlStr'.
void addDepth(bsl::string& xmlStr, int currentDepth, int depth)
{
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
              int          depth)
{
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
void ggg(bsl::string& xmlStr, int numNodes, int depth)
{
    xmlStr.assign(strXmlStart);
    addNodes(xmlStr, 0, numNodes, 0, depth);
    xmlStr.append(strXmlEnd);
}

void nonAsciiUtf8(bsl::string *str)
    // Translate boring ASCII strings in the specified 'str' into strings
    // containing non-ascii UTF-8.
{
    static struct Record {
        const char *d_substitute;
        bsl::size_t d_substituteLen;
        const char *d_original;
        bsl::size_t d_originalLen;
    } TABLE[] = {
        { "Element1", 0, "\xc7\x8f" "Element1\xeb\xac\x8f",         0 },
        { "Element2", 0, "\xeb\xac\x8f" "Element2\xca\xbf",         0 },
        { "Element3", 0, "\xf1\xbf\xa3\x92" "Element3\xe7\xa4\x8a", 0 },
        { "Element4", 0, "\xef\xbf\xbf" "Element4\xef\xbf\xbf",     0 },

        { "element1", 0, "\xe0\xbf\x80" "element1\xc4\xac",         0 },
        { "element2", 0, "\xec\xa2\x88" "element2\xea\x8f\xa5",     0 },
        { "element3", 0, "\xc3\xa4" "element3\xf2\xbe\xa0\xb7",     0 },
        { "element4", 0, "\xdf\x80" "element4\xe3\xb2\x90",         0 },

        { "Node"    , 0, "\xc6\x92Node\xf3\x9a\x8f\x8e",            0 } };
    enum { k_NUM_TABLE = sizeof TABLE / sizeof *TABLE };

    if (0 == TABLE[0].d_originalLen) {
        for (int ii = 0; ii < k_NUM_TABLE; ++ii) {
            Record& record = TABLE[ii];
            record.d_originalLen   = bsl::strlen(record.d_original);
            record.d_substituteLen = bsl::strlen(record.d_substitute);
        }
    }

    const bsl::size_t npos = bsl::string::npos;

    for (int ii = 0; ii < k_NUM_TABLE; ++ii) {
        const Record& record = TABLE[ii];

        bsl::size_t pos = 0;
        while (npos != (pos = str->find(record.d_original, pos))) {
            str->erase( pos, record.d_originalLen);
            str->insert(pos, record.d_substitute);
            pos += record.d_substituteLen;
        }
    }
}

void checkNodeName(Obj& reader, const char* tag, int n)
{
    bsl::stringstream ss1;
    ss1 << tag << n;
    ASSERT(!bsl::strcmp(reader.nodeName(), ss1.str().c_str()));
}

// Recursively read nested nodes from 'currentDepth' to 'depth' to 'xmlStr'.
void readDepth(Obj& reader, int currentDepth, int depth)
{
    if (currentDepth == depth) {
        return;                                                       // RETURN
    }

    // Note that 'currentDepth' does not equal 'reader.nodeDepth()', this is
    // because 'currentDepth' does not account for the depth add by the header
    // information.

    int rc = advancePastWhiteSpace(reader);
    (void)rc;

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
    (void)rc;

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
    int rc = advancePastWhiteSpace(reader); (void)rc;
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

void advanceN(Obj& reader, bsl::size_t n)
    // Advance the specified 'n' number of nodes in the specified 'reader'.
{
    while (n--) {
        advancePastWhiteSpace(reader);
    }
}

namespace Utf8Test {

const Utf8Util::ErrorStatus EIT = Utf8Util::k_END_OF_INPUT_TRUNCATION;
const Utf8Util::ErrorStatus UCO = Utf8Util::k_UNEXPECTED_CONTINUATION_OCTET;
const Utf8Util::ErrorStatus NCO = Utf8Util::k_NON_CONTINUATION_OCTET;
const Utf8Util::ErrorStatus OLE = Utf8Util::k_OVERLONG_ENCODING;
const Utf8Util::ErrorStatus IIO = Utf8Util::k_INVALID_INITIAL_OCTET;
const Utf8Util::ErrorStatus VTL = Utf8Util::k_VALUE_LARGER_THAN_0X10FFFF;
const Utf8Util::ErrorStatus SUR = Utf8Util::k_SURROGATE;

static const struct Data {
    int                  d_lineNum;    // source line number

    const char           *d_utf8_p;     // UTF-8 input string

    int                  d_numBytes;   // length of spec (in bytes), not
                                       // including null-terminator

    int                  d_numCodePoints;
                                       // +ve number of UTF-8 code points if
                                       // valid, -ve Utf8Util::ErrorStatus is
                                       // invalid.

    int                  d_errOffset;  // byte offset to first invalid
                                       // sequence; -1 if valid

    int                  d_isValid;    // 1 if valid UTF-8; 0 otherwise
} DATA[] = {
    //L#  input                          #b   #c  eo  result
    //--  -----                          --  ---  --  ------
    { L_, "",                             0,   0, -1,   1   },

    { L_, "H",                            1,   1, -1,   1   },
    { L_, "He",                           2,   2, -1,   1   },
    { L_, "Hel",                          3,   3, -1,   1   },
    { L_, "Hell",                         4,   4, -1,   1   },
    { L_, "Hello",                        5,   5, -1,   1   },

    // Check the boundary between 1-octet and 2-octet code points.

    { L_, "\x7f",                         1,   1, -1,   1   },
    { L_, "\xc2\x80",                     2,   1, -1,   1   },

    // Check the boundary between 2-octet and 3-octet code points.

    { L_, "\xdf\xbf",                     2,   1, -1,   1   },
    { L_, "\xe0\xa0\x80",                 3,   1, -1,   1   },

    // Check the maximal 3-octet code point.

    { L_, "\xef\xbf\xbf",                 3,   1, -1,   1   },

    // Make sure 4-octet code points are handled correctly.

    { L_, "\xf0\x90\x80\x80",             4,   1, -1,   1   },
    { L_, "\xf0\x90\x80\x80g",            5,   2, -1,   1   },
    { L_, "a\xf0\x90\x80\x81g",           6,   3, -1,   1   },
    { L_, "\xf4\x8f\xbf\xbe",             4,   1, -1,   1   },
    { L_, "\xf4\x8f\xbf\xbeg",            5,   2, -1,   1   },
    { L_, "a\xf4\x8f\xbf\xbfg",           6,   3, -1,   1   },

    // unexpected continuation octets

    { L_, "\x80",                         1, UCO,  0,   0   },
    { L_, "\x85p",                        2, UCO,  0,   0   },
    { L_, "a\x85",                        2, UCO,  1,   0   },
    { L_, "\x90",                         1, UCO,  0,   0   },
    { L_, "a\x91",                        2, UCO,  1,   0   },
    { L_, "\x9f",                         1, UCO,  0,   0   },
    { L_, "a\x9f",                        2, UCO,  1,   0   },
    { L_, "a\xa0",                        2, UCO,  1,   0   },
    { L_, "\xa1",                         1, UCO,  0,   0   },
    { L_, "7\xaf",                        2, UCO,  1,   0   },
    { L_, "\xaf",                         1, UCO,  0,   0   },
    { L_, "a\xb0",                        2, UCO,  1,   0   },
    { L_, "\xb1",                         1, UCO,  0,   0   },
    { L_, "\xbf",                         1, UCO,  0,   0   },
    { L_, "p\xbf",                        2, UCO,  1,   0   },

    // Make sure partial 4-octet code points are handled correctly (with a
    // single error).

    { L_, "\xf0",                         1, EIT,  0,   0   },
    { L_, "\xf0\x80",                     2, EIT,  0,   0   },
    { L_, "\xf0\x80\x80",                 3, EIT,  0,   0   },
    { L_, "\xf0g",                        2, NCO,  0,   0   },
    { L_, "\xf0\x80g",                    3, NCO,  0,   0   },
    { L_, "\xf0\x80\x80g",                4, NCO,  0,   0   },

    // Make sure partial 4-octet code points are handled correctly (with a
    // single error).

    { L_, "\xe0\x80",                     2, EIT,  0,   0   },
    { L_, "\xe0",                         1, EIT,  0,   0   },
    { L_, "\xe0\x80g",                    3, NCO,  0,   0   },
    { L_, "\xe0g",                        2, NCO,  0,   0   },

    // Make sure the "illegal" UTF-8 octets are handled correctly:
    //   o The octet values C0, C1, F5 to FF never appear.

    { L_, "\xc0",                         1, EIT,  0,   0   },
    { L_, "\xc1",                         1, EIT,  0,   0   },
    { L_, "\xf0",                         1, EIT,  0,   0   },
    { L_, "\xf5",                         1, EIT,  0,   0   },
    { L_, "\xf6",                         1, EIT,  0,   0   },
    { L_, "\xf7",                         1, EIT,  0,   0   },
    { L_, "\xf8",                         1, IIO,  0,   0   },
    { L_, "\xf8\xaf\xaf\xaf",             4, IIO,  0,   0   },
    { L_, "\xf8\x80\x80\x80",             4, IIO,  0,   0   },
    { L_, "\xf8",                         1, IIO,  0,   0   },
    { L_, "\xf9",                         1, IIO,  0,   0   },
    { L_, "\xfa",                         1, IIO,  0,   0   },
    { L_, "\xfb",                         1, IIO,  0,   0   },
    { L_, "\xfc",                         1, IIO,  0,   0   },
    { L_, "\xfd",                         1, IIO,  0,   0   },
    { L_, "\xfe",                         1, IIO,  0,   0   },
    { L_, "\xff",                         1, IIO,  0,   0   },

    // Make sure that the "illegal" UTF-8 octets are handled correctly
    // mid-string:
    //   o The octet values C0, C1, F5 to FF never appear.

    { L_, "a\xc0g",                       3, NCO,  1,   0   },
    { L_, "a\xc1g",                       3, NCO,  1,   0   },
    { L_, "a\xf5g",                       3, NCO,  1,   0   },
    { L_, "a\xf6g",                       3, NCO,  1,   0   },
    { L_, "a\xf7g",                       3, NCO,  1,   0   },
    { L_, "a\xf8g",                       3, IIO,  1,   0   },
    { L_, "a\xf9g",                       3, IIO,  1,   0   },
    { L_, "a\xfag",                       3, IIO,  1,   0   },
    { L_, "a\xfbg",                       3, IIO,  1,   0   },
    { L_, "a\xfcg",                       3, IIO,  1,   0   },
    { L_, "a\xfdg",                       3, IIO,  1,   0   },
    { L_, "a\xfeg",                       3, IIO,  1,   0   },
    { L_, "a\xffg",                       3, IIO,  1,   0   },

    { L_, "\xc2\x80",                     2,   1, -1,   1   },
    { L_, "\xc2",                         1, EIT,  0,   0   },
    { L_, "\xc2\x80g",                    3,   2, -1,   1   },
    { L_, "\xc3\xbf",                     2,   1, -1,   1   },
    { L_, "\x01z\x7f\xc3\xbf\xdf\xbf\xe0\xa0\x80\xef\xbf\xbf",
                                         13,   7, -1,   1   },

    { L_, "a\xef",                        2, EIT,  1,   0   },
    { L_, "a\xef\xbf",                    2, EIT,  1,   0   },

    { L_, "\xef\xbf\xbf\xe0\xa0\x80\xdf\xbf\xc3\xbf\x7fz\x01",
                                         13,   7, -1,   1   },

    // Make sure illegal overlong encodings are not accepted.  These code
    // points are mathematically correctly encoded, but since there are
    // equivalent encodings with fewer octets, the UTF-8 standard disallows
    // them.

    { L_, "\xf0\x80\x80\x80",             4, OLE,  0,   0   },
    { L_, "\xf0\x8a\xaa\xa0",             4, OLE,  0,   0   },
    { L_, "\xf0\x8f\xbf\xbf",             4, OLE,  0,   0   },    // max OLE
    { L_, "\xf0\x90\x80\x80",             4,   1, -1,   1   },    // min legal
    { L_, "\xf1\x80\x80\x80",             4,   1, -1,   1   },    // norm legal
    { L_, "\xf1\xaa\xaa\xaa",             4,   1, -1,   1   },    // norm legal
    { L_, "\xf4\x8f\xbf\xbf",             4,   1, -1,   1   },    // max legal
    { L_, "\xf4\x90\x80\x80",             4, VTL,  0,   0   },    // min VTL
    { L_, "\xf4\x90\xbf\xbf",             4, VTL,  0,   0   },    //     VTL
    { L_, "\xf4\xa0\x80\x80",             4, VTL,  0,   0   },    //     VTL
    { L_, "\xf7\xbf\xbf\xbf",             4, VTL,  0,   0   },    // max VTL

    { L_, "\xe0\x80\x80",                 3, OLE,  0,   0   },
    { L_, "\xe0\x9a\xaf",                 3, OLE,  0,   0   },
    { L_, "\xe0\x9f\xbf",                 3, OLE,  0,   0   },    // max OLE
    { L_, "\xe0\xa0\x80",                 3,   1, -1,   1   },    // min legal

    { L_, "\xc0\x80",                     2, OLE,  0,   0   },
    { L_, "\xc0\xaf",                     2, OLE,  0,   0   },
    { L_, "\xc0\xbf",                     2, OLE,  0,   0   },
    { L_, "\xc1\x81",                     2, OLE,  0,   0   },
    { L_, "\xc1\xbf",                     2, OLE,  0,   0   },    // max OLE
    { L_, "\xc2\x80",                     2,   1,  0,   1   },    // min legal

    // Corrupted 2-octet code point:

    { L_, "\xc2",                         1, EIT,  0,   0   },
    { L_, "a\xc2",                        2, EIT,  1,   0   },
    { L_, "\xc2g",                        2, NCO,  0,   0   },
    { L_, "\xc2\xc2",                     2, NCO,  0,   0   },
    { L_, "\xc2\xef",                     2, NCO,  0,   0   },

    // Corrupted 2-octet code point followed by an invalid code point:

    { L_, "\xc2\xff",                     2, NCO,  0,   0   },
    { L_, "\xc2\xff",                     2, NCO,  0,   0   },

    // 3-octet code points corrupted after octet 1:

    { L_, "\xef",                         1, EIT,  0,   0   },
    { L_, "a\xef",                        2, EIT,  1,   0   },
    { L_, "\xefg",                        2, NCO,  0,   0   },
    { L_, "\xef\xefg",                    3, NCO,  0,   0   },
    { L_, "\xefg\xef",                    3, NCO,  0,   0   },
    { L_, "\xef" "\xc2\x80",              3, NCO,  0,   0   },

    // 3-octet code points corrupted after octet 2:

    { L_, "\xef\xbf",                     2, EIT,  0,   0   },
    { L_, "\xef\xbf",                     2, EIT,  0,   0   },
    { L_, "a\xef\xbf@",                   4, NCO,  1,   0   },
    { L_, "a\xef\xbf@",                   4, NCO,  1,   0   },
    { L_, "\xef\xbfg",                    3, NCO,  0,   0   },
    { L_, "\xef\xbf\xef",                 3, NCO,  0,   0   },

    { L_, "\xed\xa0\x80",                 3, SUR,  0,   0   },
    { L_, "\xed\xb0\x85g",                4, SUR,  0,   0   },
    { L_, "\xed\xbf\xbf",                 3, SUR,  0,   0   },
};
enum { k_NUM_DATA = sizeof DATA / sizeof *DATA };

enum Mode { e_FILE, e_STRING, e_STREAMBUF, e_END };

bsl::ostream& operator<<(bsl::ostream& stream, Mode mode)
{
#undef  CASE
#define CASE(value)    case value: { stream << #value; } break

    switch (mode) {
      CASE(e_FILE);
      CASE(e_STRING);
      CASE(e_STREAMBUF);
      CASE(e_END);
      default: {
        stream << "Unknown mode: " << static_cast<int>(mode);
      } break;
    }
#undef  CASE

    return stream;
}

int findLoc(int                *line,
            int                *column,
            const bsl::string&  pattern,
            bsl::size_t         offset)
{
    if (pattern.length() < offset) {
        return -1;                                                    // RETURN
    }

    *line = 1;
    *column = 1;

    for (unsigned pos = 0; pos < offset; ++pos) {
        const char c = pattern[pos];

        if ('\n' == c) {
            ++*line;
            *column = 1;
        }
        else {
            ++*column;
        }
    }

    return 0;
}

bsl::string utf8Dump(const char *utf8)
{
    bsl::string ret;

    for (const char *pc = utf8; *pc; ++pc) {
        unsigned char uc = *pc;

        ret += "\\x";
        for (int shift = 4; 0 <= shift; shift -= 4) {
            const int nybble = (uc >> shift) & 0xf;

            ret += static_cast<char>(nybble < 10 ? '0' + nybble
                                                 : 'a' + nybble - 10);
        }
    }

    return ret;
}

void writeStringToFile(const char         *fileName,
                       const bsl::string&  str)
{
    typedef bdls::FilesystemUtil FUtil;

    while (true) {
        FUtil::remove(fileName);

        bsl::ofstream of(fileName, bsl::ios_base::out | bsl::ios_base::binary);
        of << str << bsl::flush;
        of.close();

#if defined(BSLS_PLATFORM_OS_WINDOWS)
        // The above write to file sometimes fails on Windows, so we have to
        // verify that it succeeded.

        if (! FUtil::exists(fileName)) {
            continue;
        }

        Uint64 fileSize = FUtil::getFileSize(fileName);
        if (fileSize != str.length()) {
            continue;
        }

        bsl::ifstream ifs(fileName, bsl::ios_base::in | bsl::ios_base::binary);
        bsl::string readStr;
        char readBuf[10 * 1024];
        ifs.read(readBuf, sizeof(readBuf));
        readBuf[ifs.gcount()] = 0;

        if (! ifs.eof() || str != readBuf) {
            continue;
        }
#endif
        return;                                                       // RETURN
    }
}

}  // close namespace Utf8Test

// ============================================================================
//                               MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    test            = argc > 1 ? bsl::atoi(argv[1]) : 0;
    verbose         = argc > 2;
    veryVerbose     = argc > 3;
    veryVeryVerbose = argc > 4;

    bslma::TestAllocator testAllocator(veryVeryVerbose);

    bsl::cout << "TEST " << __FILE__ << " CASE " << test << bsl::endl;;

    switch (test) { case 0:  // Zero is always the leading case.
      case 17: {
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
        //   USAGE EXAMPLE
        //   MiniReader(basicAllocator)
        //   MiniReader(bufSize, basicAllocator)
        //   ~MiniReader()
        //   setPrefixStack(balxml::PrefixStack *prefixes)
        //   prefixStack()
        //   open()
        //   isOpen()
        //   documentEncoding()
        //   nodeType()
        //   nodeName()
        //   nodeHasValue()
        //   nodeValue()
        //   nodeDepth()
        //   numAttributes()
        //   isEmptyElement()
        //   advanceToNextNode()
        //   lookupAttribute(ElemAtt a, int index)
        //   lookupAttribute(ElemAtt a, char *qname)
        //   lookupAttribute(ElemAtt a, char *localname, char *nsUri)
        //   lookupAttribute(ElemAtt a, char *localname, int nsId)
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nUSAGE EXAMPLE"
                               << "\n=============" << bsl::endl;

        usageExample();

      } break;

      case 16: {
        // --------------------------------------------------------------------
        // VALID AND INVALID UTF-8 TEST
        //
        // Concern:
        //: 1 That the object functions correctly on input containing non-ascii
        //:   UTF-8.
        //:
        //: 2 That the object detects invalid UTF-8.
        //:   o It correctly diagnoses the nature of the UTF-8 error.
        //:
        //:   o It correctly reports the location of the invalid UTF-8 code
        //:     point.
        //
        // Plan:
        //: 1 Repeat the following tests reading from a string, a file, and a
        //:   'bsl::streambuf'.
        //:
        //: 2 Start with a correct, all-ascii pattern, and insert various
        //:   correct non-ascii UTF-8 snippets into places that will not cause
        //:   a syntax error, and observe that we can read the whole pattern
        //:   without an error.
        //:
        //: 3 Start with the same correct all-ascii pattern, and insert various
        //:   snippets of invalid UTF-8 into it at different random positions
        //:   in the pattern.
        //:   o Observe that either 'open' or 'advanceToNextNode' return a
        //:     negative status, and observe:
        //:     1 that that negative status is the
        //:       'bdlde::Utf8Util::ErrorStatus' 'enum' appropriate to describe
        //:       the nature of the UTF-8 error.
        //:
        //:     2 that the line number and column indicated by 'errorInfo()'
        //:       correctly indicate the location of the start of the invalid
        //:       UTF-8 code point.
        //:
        //:     3 that the 'msg' field of the 'errorInfo()' object contains a
        //:       description of the UTF-8 error.
        //
        // Testing:
        //   VALID AND INVALID UTF-8 TEST
        //   open(const char *filename, const char *encoding);
        //   open(const char *buffer, size_t size);
        //   open(bsl::streambuf *stream);
        // --------------------------------------------------------------------

        if (verbose) cout << "VALID AND INVALID UTF-8 TEST\n"
                             "============================\n";

        namespace TC = Utf8Test;

        enum { k_BUF_LEN = 1024 };
        char fileName[k_BUF_LEN];
        {
            bsl::string osName, osVersion, osPatch;
            const char *hostName = bsl::getenv("HOSTNAME");
            if (!hostName) {
                ASSERT(0 == bdls::OsUtil::getOsInfo(&osName,
                                                    &osVersion,
                                                    &osPatch));
                osName += '.' + osVersion + '.' + osPatch;
                hostName = osName.c_str();
            }

            bdlsb::FixedMemOutStreamBuf fileSb(fileName, sizeof(fileName));
            bsl::ostream                fileStream(&fileSb);
            fileStream << "tmp.balxml_utf8readerwrapper.16." << hostName <<
                          '.' << bdls::ProcessUtil::getProcessId() <<
                                                           ".xml" << bsl::ends;

            BSLS_ASSERT(bsl::strlen(fileName) < sizeof(fileName));
        }

        if (verbose) P(fileName);

#define abcd " a='/Element1' b='/Element2' c='/Element3' d='/Node0' "
        const bsl::string xmlRaw =
          "<?xml version='1.0' encoding='UTF-8'?>\n"
          "<xs:schema xmlns:xs='http://www.w3.org/2001/XMLSchema'\n"
          "    elementFormDefault='qualified'\n"
          "    xmlns:bdem='http://bloomberg.com/schemas/bdem'\n"
          "    bdem:package='bascfg'>\n"
          "<Node0" abcd ">\n"
          "    <Element1" abcd ">element1</Element1>\n"
          "    <Element2" abcd ">element2</Element2>\n"
          "    <Element3" abcd ">element3</Element3>\n"
          "</Node0>\n"
          "</xs:schema>";
#undef abcd

        for (int di = 0; di < TC::k_NUM_DATA; ++di) {
            const TC::Data&    data       = TC::DATA[di];
            const int          LINE       = data.d_lineNum;
            const char        *UTF8       = data.d_utf8_p;
            const bsl::size_t  NUM_BYTES  = data.d_numBytes;
            const bool         IS_VALID   = data.d_isValid;

            if (!IS_VALID) {
                continue;
            }

            if (veryVerbose) { P_(LINE);    P(TC::utf8Dump(UTF8)); }

            for (int mi = TC::e_FILE; mi < TC::e_END; ++mi) {
                const TC::Mode mode = static_cast<TC::Mode>(mi);

                static const char *insertAfter[] = {
                                                "Node", "Element", "element" };
                enum { k_NUM_INSERT_AFTER =
                                    sizeof insertAfter / sizeof *insertAfter };

                if (veryVeryVerbose) { T2_ P_(LINE);    P(mode); }

                for (int ai = 0; ai < k_NUM_INSERT_AFTER; ++ai) {
                    const bsl::string AFTER = insertAfter[ai];

                    bsl::string xmlStr = xmlRaw;

                    int numFound = 0;
                    bsl::size_t insertPos = 0;
                    while (bsl::string::npos !=
                             (insertPos = xmlStr.find(AFTER, insertPos + 1))) {
                        ++numFound;
                        xmlStr.insert(insertPos + AFTER.length(),
                                      UTF8,
                                      NUM_BYTES);
                    }
                    ASSERT(0 < numFound);

                    if (veryVeryVerbose) {
                        T2_ T2_ P_(LINE); P_(AFTER); P(numFound);
                    }

                    Mini miniReader;
                    Obj  reader(&miniReader);

                    balxml::NamespaceRegistry namespaces;
                    balxml::PrefixStack prefixStack(&namespaces);
                    reader.setPrefixStack(&prefixStack);

                    bdls::FilesystemUtil::remove(fileName);

                    bdlsb::FixedMemInStreamBuf sb("", 0);

                    switch (mode) {
                      case TC::e_FILE: {
                        TC::writeStringToFile(fileName, xmlStr);

                        ASSERT(0 == reader.open(fileName));
                      } break;
                      case TC::e_STRING: {
                        ASSERT(0 ==
                                 reader.open(xmlStr.c_str(), xmlStr.length()));
                      } break;
                      case TC::e_STREAMBUF: {
                        sb.pubsetbuf(&xmlStr[0], xmlStr.length());

                        ASSERT(0 == reader.open(&sb));
                      } break;
                      case TC::e_END: BSLA_FALLTHROUGH;
                      default: {
                        BSLS_ASSERT(0 && "impossible mode");
                      }
                    }

                    int rc;
                    while (0 == (rc = reader.advanceToNextNode())) {
                        ;    // do nothing
                    }

                    ASSERTV(LINE, AFTER, rc, reader.errorInfo(), mode,
                                                              xmlStr, 1 == rc);

                    bdls::FilesystemUtil::remove(fileName);
                }
            }
        }

        const bool exhaustive = verbose ? bsl::atoi(argv[2]) : false;
        if (verbose) P(exhaustive);

        int colPass = 0, colFail = 0;
        RandGen rand;

        for (int di = 0; di < TC::k_NUM_DATA; ++di) {
            const TC::Data&    data       = TC::DATA[di];
            const int          LINE       = data.d_lineNum;
            const char        *UTF8       = data.d_utf8_p;
            const bsl::size_t  NUM_BYTES  = data.d_numBytes;
            const unsigned     ERR_OFFSET = data.d_errOffset;
            const int          ERR_TYPE   = data.d_numCodePoints;
            const bool         IS_VALID   = data.d_isValid;

            if (IS_VALID) {
                continue;
            }

            ASSERT(ERR_TYPE < 0);

            if (veryVerbose) { P_(LINE);    P(TC::utf8Dump(UTF8)); }

            for (int mi = TC::e_FILE; mi < TC::e_END; ++mi) {
                const TC::Mode mode = static_cast<TC::Mode>(mi);

                const int mod = TC::e_FILE == mode
                              ? (exhaustive ? 20 : 500)
                              : (exhaustive ?  1 :  50);

                if (veryVerbose) { T2_    P_(mode);    P(mod); }

                for (bsl::size_t badPos = 0; true; badPos += 1 + rand()%mod) {
                    badPos = bsl::min(badPos, xmlRaw.length());

                    if (0 < ERR_OFFSET && 0 == badPos) {
                        // Garbage that is valid UTF-8 injected at the first
                        // byte is liable to be interpreted as a syntax error
                        // before we get to the invalid UTF-8, which just
                        // confuses the test.

                        continue;
                    }

                    bsl::string xmlStr = xmlRaw;

                    if (TC::EIT == ERR_TYPE) {
                        xmlStr.resize(badPos);
                    }
                    const bdlde::Utf8Util::ErrorStatus errType =
                           static_cast<bdlde::Utf8Util::ErrorStatus>(ERR_TYPE);
                    const bsl::string expMsg = bdlde::Utf8Util::toAscii(
                                                                      errType);

                    xmlStr.insert(badPos, UTF8, NUM_BYTES);

                    if (veryVeryVerbose) {
                        if (!exhaustive || 0 == badPos % 20) {
                            T2_    T2_    P_(badPos);    P(expMsg);
                        }
                    }

                    Mini miniReader;
                    Obj  reader(&miniReader);

                    balxml::NamespaceRegistry namespaces;
                    balxml::PrefixStack prefixStack(&namespaces);
                    reader.setPrefixStack(&prefixStack);

                    if (TC::e_FILE == mode) {
                        bdls::FilesystemUtil::remove(fileName);
                    }

                    bdlsb::FixedMemInStreamBuf sb("", 0);

                    int rc = 0;
                    switch (mode) {
                      case TC::e_FILE: {
                        TC::writeStringToFile(fileName, xmlStr);

                        rc = reader.open(fileName);
                      } break;
                      case TC::e_STRING: {
                        rc = reader.open(xmlStr.c_str(), xmlStr.length());
                      } break;
                      case TC::e_STREAMBUF: {
                        sb.pubsetbuf(&xmlStr[0], xmlStr.length());

                        rc = reader.open(&sb);
                      } break;
                      case TC::e_END: BSLA_FALLTHROUGH;
                      default: {
                        BSLS_ASSERT(0 && "impossible mode");
                      }
                    }

                    // Note that 'MiniReader::open' calls
                    // 'MiniReader::readInput', which will fail if the first
                    // byte is invalid UTF-8.

                    ASSERTV(LINE, mode, rc, badPos, 0 == rc || 0 == badPos);

                    while (0 == rc) {
                        rc = reader.advanceToNextNode();
                    }

                    ASSERTV(LINE, rc, reader.errorInfo(), mode, xmlStr,
                                                                       rc < 0);
                    ASSERTV(errType, rc, errType == rc);
                    const Uint64 pos = miniReader.getCurrentPosition();
                    ASSERTV(LINE, badPos, ERR_OFFSET, pos,
                                                   badPos + ERR_OFFSET == pos);

                    int expLine, expCol;
                    ASSERT(0 == TC::findLoc(&expLine,
                                            &expCol,
                                            xmlStr,
                                            badPos + ERR_OFFSET));

                    const balxml::ErrorInfo& errorInfo = reader.errorInfo();

                    ASSERTV(errorInfo.lineNumber(), expLine,
                                            errorInfo.lineNumber() == expLine);
                    if (errorInfo.columnNumber() == expCol) {
                        ++colPass;
                    }
                    else {
                        ++colFail;
                        ASSERTV(LINE, errorInfo.columnNumber(), expCol, badPos,
                         colPass, colFail, errorInfo.columnNumber() == expCol);
                        ASSERTV(LINE, badPos, xmlStr,
                                           errorInfo.columnNumber() == expCol);
                    }
                    ASSERT(bsl::string::npos != errorInfo.message().find(
                                                                      expMsg));

                    if (xmlRaw.length() <= badPos) {
                        break;
                    }
                }
            }
        }

        bdls::FilesystemUtil::remove(fileName);
      } break;

      case 15: {
        // --------------------------------------------------------------------
        // TRAITS TEST
        //
        // Concern:
        //: 1 That the reader wrapper has the correct memory trait.
        //
        // Plan:
        //: 1 Use 'BSLMF_ASSERT' to test the trait.
        //
        // Testing:
        //   bslma::UsesBslmaAllocator<Obj>
        // --------------------------------------------------------------------

        BSLMF_ASSERT(bslma::UsesBslmaAllocator<Obj>::value);
      } break;

      case 14: {
        // --------------------------------------------------------------------
        // ADVANCE TO END NODE RAW BARE TEST
        //
        // Concerns:
        //: 1 'advanceToEndNodeRawBare' function skips all (text, whitespace,
        //:    start element, end element for skipped elements) node types
        //:    (without XML validation) until it finds the end element for
        //:   the element it was called for.
        //:
        //: 2 Calling 'advanceToEndNodeRawBare' in states 'ST_EOF', 'ST_CLOSE'
        //:   or 'ST_ERROR' is a no-op and it returns an error.
        //:
        //: 3 Calling 'advanceToEndNodeRawBare' on an empty element is a no-op
        //:   and returns success.
        //:
        //: 4 Node names crossing buffer boundaries are found properly.
        //
        // Plan:
        //: 1 Create input XML strings and call 'advanceToEndNodeRawBare' in
        //:   several states of them:
        //:   1 Verify skipping the deepest element
        //:   2 Verify skipping mid elements
        //:   3 Verify skipping outer element
        //:   4 Verify skipping in EOF and CLOSE state (error)
        //:   5 Verify skipping in ERROR state (error)
        //:   6 Verify skipping an empty element '<Node0/>'
        //:   7 Verify skipping in larger XML, larger than the read buffer
        //:   8 Verify skipping in ill-formed large XML (error)
        //:   9 Verify skipping in partial large XML (error)
        //:  10 Verify skipping nested elements with the same name
        //
        // Testing:
        //   advanceToEndNodeRawBare()
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nADVANCE TO END NODE RAW BARE TEST"
                               << "\n================================="
                               << bsl::endl;

#define abcd " a='/Element1' b='/Element2' c='/Element3' d='/Node0' "
        static const char *xmlRaw =
          "<?xml version='1.0' encoding='UTF-8'?>\n"
          "<xs:schema xmlns:xs='http://www.w3.org/2001/XMLSchema'\n"
          "    elementFormDefault='qualified'\n"
          "    xmlns:bdem='http://bloomberg.com/schemas/bdem'\n"
          "    bdem:package='bascfg'>\n"
          "<Node0" abcd ">\n"
          "    <Element1" abcd ">element1</Element1>\n"
          "    <Element2" abcd ">element2</Element2>\n"
          "    <Element3" abcd ">element3</Element3>\n"
          "</Node0>\n"
          "</xs:schema>";
#undef abcd

        bsl::string xmlStr(xmlRaw);
        nonAsciiUtf8(&xmlStr);

        if (veryVerbose) P(xmlStr);

        balxml::NamespaceRegistry namespaces;
        balxml::PrefixStack prefixStack(&namespaces);
        Mini miniReader;
        Obj  reader(&miniReader);

        reader.setPrefixStack(&prefixStack);

        if (veryVerbose) bsl::cout << "\nSkip deepest element, Element3."
                                      "\n- - - - - - - - - - - - - - - -"
                                   << bsl::endl;
        {
            int rc = reader.open(xmlStr.c_str(), xmlStr.length());
            ASSERT(-1 < rc);

            ASSERT( reader.isOpen());
            ASSERT( reader.nodeType() == balxml::Reader::e_NODE_TYPE_NONE);

            advanceN(reader, 10);  // XML declaration --> <Element3>

            LOOP_ASSERT(rc,
                        (rc = miniReader.advanceToEndNodeRawBare()) == 0); //<<

            ASSERT(reader.nodeType() ==
                                      balxml::Reader::e_NODE_TYPE_END_ELEMENT);
            LOOP_ASSERT((reader.nodeName() ? reader.nodeName() : "(null)"),
                        !bsl::strcmp(reader.nodeName(), "Element3"));

            rc = advancePastWhiteSpace(reader);
            ASSERT( reader.nodeType() ==
                                      balxml::Reader::e_NODE_TYPE_END_ELEMENT);
            ASSERT(!bsl::strcmp(reader.nodeName(), "Node0"));

            rc = advancePastWhiteSpace(reader);
            ASSERT( reader.nodeType() ==
                                      balxml::Reader::e_NODE_TYPE_END_ELEMENT);
            ASSERT(!bsl::strcmp(reader.nodeName(), "xs:schema"));

            reader.close();
        }

        if (veryVerbose) bsl::cout << "\nSkip Element1."
                                      "\n - - - - - - -"
                                   << bsl::endl;
        {
            int rc = reader.open(xmlStr.c_str(), xmlStr.length());
            ASSERT(-1 < rc);

            ASSERT( reader.isOpen());
            ASSERT( reader.nodeType() == balxml::Reader::e_NODE_TYPE_NONE);

            advanceN(reader, 4);  // XML declaration --> <Element1>

            LOOP_ASSERT(rc,
                        (rc = miniReader.advanceToEndNodeRawBare()) == 0); //<<

            ASSERT( reader.nodeType() ==
                                      balxml::Reader::e_NODE_TYPE_END_ELEMENT);
            ASSERT(!bsl::strcmp(reader.nodeName(), "Element1"));

            advanceN(reader, 8);
            ASSERT( reader.nodeType() ==
                                      balxml::Reader::e_NODE_TYPE_END_ELEMENT);
            ASSERT(!bsl::strcmp(reader.nodeName(), "xs:schema"));

            reader.close();
        }

        if (veryVerbose) bsl::cout << "\nSkip inner element, Node0."
                                      "\n- - - - - - - - - - - - - "
                                   << bsl::endl;
        {
            int rc = reader.open(xmlStr.c_str(), xmlStr.length());
            ASSERT(-1 < rc);

            ASSERT( reader.isOpen());
            ASSERT( reader.nodeType() == balxml::Reader::e_NODE_TYPE_NONE);

            advanceN(reader, 3);  // XML declaration --> <Node0>

            LOOP_ASSERT(rc,
                        (rc = miniReader.advanceToEndNodeRawBare()) == 0); //<<

            ASSERT( reader.nodeType() ==
                                      balxml::Reader::e_NODE_TYPE_END_ELEMENT);
            ASSERT(!bsl::strcmp(reader.nodeName(), "Node0"));

            rc = advancePastWhiteSpace(reader);
            ASSERT( reader.nodeType() ==
                                      balxml::Reader::e_NODE_TYPE_END_ELEMENT);
            ASSERT(!bsl::strcmp(reader.nodeName(), "xs:schema"));

            reader.close();
        }

        if (veryVerbose) bsl::cout << "\nSkip outer element, xs:schema."
                                      "\n- - - - - - - - - - - - - - - "
                                   << bsl::endl;
        {
            int rc = reader.open(xmlStr.c_str(), xmlStr.length());
            ASSERT(-1 < rc);

            ASSERT( reader.isOpen());
            ASSERT( reader.nodeType() == balxml::Reader::e_NODE_TYPE_NONE);

            advanceN(reader, 2);  // XML declaration --> <xs:schema>

            LOOP_ASSERT(rc,
                        (rc = miniReader.advanceToEndNodeRawBare()) == 0); //<<

            ASSERT( reader.nodeType() ==
                                      balxml::Reader::e_NODE_TYPE_END_ELEMENT);
            ASSERT(!bsl::strcmp(reader.nodeName(), "xs:schema"));

            reader.close();
        }

        if (veryVerbose) bsl::cout << "\nTests in ST_EOF and ST_CLOSED state."
                                      "\n- - - - - - - - - - - - - - - - - - -"
                                   << bsl::endl;
        {
            static const char *xmlRaw =
              "<?xml version='1.0' encoding='UTF-8'?>\n"
              "<xs:schema xmlns:xs='http://www.w3.org/2001/XMLSchema'\n"
              "    elementFormDefault='qualified'\n"
              "    xmlns:bdem='http://bloomberg.com/schemas/bdem'\n"
              "    bdem:package='bascfg'>\n"
              "    <Node0>text</Node0>\n"
              "</xs:schema>";

            bsl::string xmlStr(xmlRaw);
            nonAsciiUtf8(&xmlStr);

            int rc = reader.open(xmlStr.c_str(), xmlStr.length());
            ASSERT(-1 < rc);
            ASSERT(reader.isOpen());
            ASSERT(reader.nodeType() == balxml::Reader::e_NODE_TYPE_NONE);

            advanceN(reader, 2);  // XML declaration --> <xs:schema>

            LOOP_ASSERT(rc,
                        (rc = miniReader.advanceToEndNodeRawBare()) == 0); //<<

            ASSERT( reader.nodeType() ==
                                      balxml::Reader::e_NODE_TYPE_END_ELEMENT);
            ASSERT(!bsl::strcmp(reader.nodeName(), "xs:schema"));

            rc = advancePastWhiteSpace(reader); // Hit EOF here

            // Skip to end node fails, the parser is in the 'ST_EOF' state:

            LOOP_ASSERT(rc, (rc = miniReader.advanceToEndNodeRaw()) == -1);

            reader.close();

            // Skip to end node fails, the parser is in the 'ST_CLOSED' state:

            LOOP_ASSERT(rc, (rc = miniReader.advanceToEndNodeRaw()) == -1);
        }

        if (veryVerbose) bsl::cout << "\nTest in ST_ERROR state."
                                      "\n- - - - - - - - - - - -" << bsl::endl;
        {
            static const char xmlRaw[] =
              "<?xml version='1.0' encoding='UTF-8'?>\n"
              "<xs:schema xmlns:xs='http://www.w3.org/2001/XMLSchema'\n"
              "    elementFormDefault='qualified'\n"
              "    xmlns:bdem='http://bloomberg.com/schemas/bdem'\n"
              "    bdem:package='bascfg'>\n"
              "    <Node0></Node1>\n"
              "</xs:schema>";

            bsl::string xmlStr(xmlRaw);
            nonAsciiUtf8(&xmlStr);

            int rc = reader.open(xmlStr.c_str(), xmlStr.length());
            ASSERT(-1 < rc);
            ASSERT(reader.isOpen());
            ASSERT(reader.nodeType() == balxml::Reader::e_NODE_TYPE_NONE);

            advanceN(reader, 3);  // XML declaration --> <Node0>

            // Wrong end-element name, going into error state:

            LOOP_ASSERT(rc, (rc = advancePastWhiteSpace(reader)) < 0);

            // Skip to end node fails, the parser is in the 'ST_ERROR' state:

            LOOP_ASSERT(rc, (rc = miniReader.advanceToEndNodeRawBare()) == -1);

            reader.close();
        }

        if (veryVerbose) bsl::cout << "\nTest empty element."
                                      "\n- - - - - - - - - -" << bsl::endl;
        {
            static const char xmlRaw[] =
              "<?xml version='1.0' encoding='UTF-8'?>\n"
              "<xs:schema xmlns:xs='http://www.w3.org/2001/XMLSchema'\n"
              "    elementFormDefault='qualified'\n"
              "    xmlns:bdem='http://bloomberg.com/schemas/bdem'\n"
              "    bdem:package='bascfg'>\n"
              "    <Node0/>\n"
              "</xs:schema>";

            bsl::string xmlStr(xmlRaw);
            nonAsciiUtf8(&xmlStr);

            int rc = reader.open(xmlStr.c_str(), xmlStr.length());
            ASSERT(-1 < rc);
            ASSERT(reader.isOpen());
            ASSERT(reader.nodeType() == balxml::Reader::e_NODE_TYPE_NONE);

            advanceN(reader, 3);  // XML declaration --> <Node0>

            LOOP_ASSERT(rc,
                        (rc = miniReader.advanceToEndNodeRawBare()) == 0); //<<

            ASSERT(reader.nodeType() == balxml::Reader::e_NODE_TYPE_ELEMENT);
            ASSERT(!bsl::strcmp(reader.nodeName(), "Node0"));

            reader.close();
        }

        if (veryVerbose) bsl::cout << "\nTests large xml (*8K+)."
                                      "\n- - - - - - - - - - - -"
                                   << bsl::endl;
        {
            bsl::string xmlStr =
              "<?xml version='1.0' encoding='UTF-8'?>\n"
              "<xs:schema xmlns:xs='http://www.w3.org/2001/XMLSchema'\n"
              "    elementFormDefault='qualified'\n"
              "    xmlns:bdem='http://bloomberg.com/schemas/bdem'\n"
              "    bdem:package='bascfg'>\n"
              "<Node0>\n"
              "    <Element1>";

            xmlStr += bsl::string(8 * 1024, 'b');

            xmlStr += "</Element1>\n"
              "    <Element2>element2</Element2>\n"
              "    <Element3>element3</Element3>\n"
              "</Node0>\n"
              "</xs:schema>";

            nonAsciiUtf8(&xmlStr);

            int rc = reader.open(xmlStr.c_str(), xmlStr.length());
            ASSERT(-1 < rc);
            ASSERT(reader.isOpen());
            ASSERT(reader.nodeType() == balxml::Reader::e_NODE_TYPE_NONE);

            advanceN(reader, 2);  // XML declaration --> <xs:schema>

            // Skip to end node

            LOOP_ASSERT(rc,
                        (rc = miniReader.advanceToEndNodeRawBare()) == 0); //<<

            ASSERT(reader.nodeType() ==
                                      balxml::Reader::e_NODE_TYPE_END_ELEMENT);
            ASSERT(!bsl::strcmp(reader.nodeName(), "xs:schema"));

            reader.close();
        }


        if (veryVerbose) bsl::cout << "\nTests bad large xml (*8K+)."
                                      "\n- - - - - - - - - - - - - -"
                                   << bsl::endl;
        {
            bsl::string xmlStr =
              "<?xml version='1.0' encoding='UTF-8'?>\n"
              "<xs:schema xmlns:xs='http://www.w3.org/2001/XMLSchema'\n"
              "    elementFormDefault='qualified'\n"
              "    xmlns:bdem='http://bloomberg.com/schemas/bdem'\n"
              "    bdem:package='bascfg'>\n"
              "<Node0>\n"
              "    <Element1>";

            xmlStr += bsl::string(8 * 1024 - xmlStr.length() - 7, 'b');

            xmlStr += "</foobar</Element1>\n"
              "    <Element2>element2</Element2>\n"
              "    <Element3>element3</Element3>\n"
              "</Node0>\n"
              "</xs:schema>";

            nonAsciiUtf8(&xmlStr);

            int rc = reader.open(xmlStr.c_str(), xmlStr.length());
            ASSERT(-1 < rc);
            ASSERT(reader.isOpen());
            ASSERT(reader.nodeType() == balxml::Reader::e_NODE_TYPE_NONE);

            advanceN(reader, 2);  // XML declaration --> <xs:schema>

            // Skip to end node

            LOOP_ASSERT(rc,
                        (rc = miniReader.advanceToEndNodeRawBare()) == 0); //<<

            ASSERT(reader.nodeType() ==
                balxml::Reader::e_NODE_TYPE_END_ELEMENT);
            ASSERT(!bsl::strcmp(reader.nodeName(), "xs:schema"));

            reader.close();
        }

        if (veryVerbose) bsl::cout << "\nTests bad unended large xml (*8K+)."
                                      "\n- - - - - - - - - - - - - - - - - -"
                                   << bsl::endl;
        {
            bsl::string xmlStr =
              "<?xml version='1.0' encoding='UTF-8'?>\n"
              "<xs:schema xmlns:xs='http://www.w3.org/2001/XMLSchema'\n"
              "    elementFormDefault='qualified'\n"
              "    xmlns:bdem='http://bloomberg.com/schemas/bdem'\n"
              "    bdem:package='bascfg'>\n"
              "<Node0>\n"
              "    <Element1>";

            xmlStr += bsl::string(8 * 1024 - xmlStr.length() - 7, 'b');

            xmlStr += "</foobar";

            nonAsciiUtf8(&xmlStr);

            int rc = reader.open(xmlStr.c_str(), xmlStr.length());
            ASSERT(-1 < rc);
            ASSERT(reader.isOpen());
            ASSERT(reader.nodeType() == balxml::Reader::e_NODE_TYPE_NONE);

            advanceN(reader, 2);  // XML declaration --> <xs:schema>

            // Skip to end node

            LOOP_ASSERT(rc, (rc = miniReader.advanceToEndNodeRawBare()) == -2);

            reader.close();
        }

        if (veryVerbose) bsl::cout << "\nTest elements with the same name."
                                      "\n- - - - - - - - - - - - - - - - -"
                                   << bsl::endl;
        {
            static const char xmlRaw[] =
              "<?xml version='1.0' encoding='UTF-8'?>\n"
              "<xs:schema xmlns:xs='http://www.w3.org/2001/XMLSchema'\n"
              "    elementFormDefault='qualified'\n"
              "    xmlns:bdem='http://bloomberg.com/schemas/bdem'\n"
              "    bdem:package='bascfg'>\n"
              "    <Node0>"
              "        <Node0>"
              "            <Node0>"
              "            </Node0>"
              "        </Node0>"
              "    </Node0>\n"
              "</xs:schema>";

            bsl::string xmlStr(xmlRaw);
            nonAsciiUtf8(&xmlStr);

            int rc = reader.open(xmlStr.c_str(), xmlStr.length());
            ASSERT(-1 < rc);
            ASSERT(reader.isOpen());
            ASSERT(reader.nodeType() == balxml::Reader::e_NODE_TYPE_NONE);

            advanceN(reader, 3);  // XML declaration --> <Node0>

            LOOP_ASSERT(rc, (rc = miniReader.advanceToEndNodeRawBare()) == 0);

            ASSERT(reader.nodeType() ==
                                      balxml::Reader::e_NODE_TYPE_END_ELEMENT);
            ASSERT(!bsl::strcmp(reader.nodeName(), "Node0"));

            LOOP2_ASSERT(rc, reader.errorInfo(),
                         (rc = advancePastWhiteSpace(reader)) == 0);

            ASSERT( reader.nodeType() ==
                                      balxml::Reader::e_NODE_TYPE_END_ELEMENT);
            LOOP_ASSERT(reader.nodeName(),
                        !bsl::strcmp(reader.nodeName(), "xs:schema"));

            reader.close();
        }

        if (veryVerbose) bsl::cout << "\nTest elements with longer name."
                                      "\n- - - - - - - - - - - - - - - -"
                                   << bsl::endl;
        {
            static const char xmlRaw[] =
              "<?xml version='1.0' encoding='UTF-8'?>\n"
              "<xs:schema xmlns:xs='http://www.w3.org/2001/XMLSchema'\n"
              "    elementFormDefault='qualified'\n"
              "    xmlns:bdem='http://bloomberg.com/schemas/bdem'\n"
              "    bdem:package='bascfg'>\n"
              "    <Node0>"
              "        <Node0>"
              "            <Node01>"
              "            </Node01>"
              "        </Node0>"
              "    </Node0>\n"
              "</xs:schema>";

            bsl::string xmlStr(xmlRaw);
            nonAsciiUtf8(&xmlStr);

            int rc = reader.open(xmlStr.c_str(), xmlStr.length());
            ASSERT(-1 < rc);
            ASSERT(reader.isOpen());
            ASSERT(reader.nodeType() == balxml::Reader::e_NODE_TYPE_NONE);

            advanceN(reader, 3);  // XML declaration --> <Node0>

            LOOP_ASSERT(rc, (rc = miniReader.advanceToEndNodeRawBare()) == 0);

            ASSERT(reader.nodeType() ==
                                      balxml::Reader::e_NODE_TYPE_END_ELEMENT);
            ASSERT(!bsl::strcmp(reader.nodeName(), "Node0"));

            LOOP2_ASSERT(rc, reader.errorInfo(),
                         (rc = advancePastWhiteSpace(reader)) == 0);

            ASSERT( reader.nodeType() ==
                                      balxml::Reader::e_NODE_TYPE_END_ELEMENT);
            LOOP_ASSERT(reader.nodeName(),
                        !bsl::strcmp(reader.nodeName(), "xs:schema"));

            reader.close();
        }

        if (veryVerbose) bsl::cout << "\nTest elements with longer name."
                                      "\n- - - - - - - - - - - - - - - -"
                                   << bsl::endl;
        {
            static const char xmlRaw[] =
              "<?xml version='1.0' encoding='UTF-8'?>\n"
              "<xs:schema xmlns:xs='http://www.w3.org/2001/XMLSchema'\n"
              "    elementFormDefault='qualified'\n"
              "    xmlns:bdem='http://bloomberg.com/schemas/bdem'\n"
              "    bdem:package='bascfg'>\n"
              "    <Node0>"
              "        <Node0>"
              "            <LongNode0>"
              "            </LongNode0>"
              "        </Node0>"
              "    </Node0>\n"
              "</xs:schema>";

            bsl::string xmlStr(xmlRaw);
            nonAsciiUtf8(&xmlStr);

            int rc = reader.open(xmlStr.c_str(), xmlStr.length());
            ASSERT(-1 < rc);
            ASSERT(reader.isOpen());
            ASSERT(reader.nodeType() == balxml::Reader::e_NODE_TYPE_NONE);

            advanceN(reader, 3);  // XML declaration --> <Node0>

            LOOP_ASSERT(rc, (rc = miniReader.advanceToEndNodeRawBare()) == 0);

            ASSERT(reader.nodeType() ==
                                      balxml::Reader::e_NODE_TYPE_END_ELEMENT);
            ASSERT(!bsl::strcmp(reader.nodeName(), "Node0"));

            LOOP2_ASSERT(rc, reader.errorInfo(),
                         (rc = advancePastWhiteSpace(reader)) == 0);

            ASSERT( reader.nodeType() ==
                                      balxml::Reader::e_NODE_TYPE_END_ELEMENT);
            LOOP_ASSERT(reader.nodeName(),
                        !bsl::strcmp(reader.nodeName(), "xs:schema"));

            reader.close();
        }
      } break;

      case 13: {
        // --------------------------------------------------------------------
        // ADVANCE TO END NODE RAW TEST
        //
        // Concerns:
        //: 1 'advanceToEndNodeRaw' function skips all (text, CDATA, comment,
        //:   whitespace, start element, end element for skipped elements) node
        //:   types (without XML validation) until it finds the end element for
        //:   the element it was called for.
        //:
        //: 2 Calling 'advanceToEndNodeRaw' in states 'ST_EOF', 'ST_CLOSE' or
        //:   'ST_ERROR' is a no-op and it returns an error.
        //:
        //: 3 Calling 'advanceToEndNodeRaw' on an empty element is a no-op and
        //:   returns success.
        //:
        //: 4 Attempt to skip unclosed comments returns an error and leaves the
        //:   parser in an error state.
        //:
        //: 5 Attempt to skip unclosed CDATA section returns an error and
        //:   leaves the parser in an error state.
        //:
        //: 6 Node names crosses buffer boundaries are found properly.
        //
        // Plan:
        //: 1 Create input XML strings and call 'advanceToEndNodeRawBare' in
        //:   several states of them:
        //:   1 Verify skipping the deepest element
        //:   2 Verify skipping mid elements
        //:   3 Verify skipping outer element
        //:   4 Verify skipping in EOF and CLOSE state (error)
        //:   5 Verify skipping in ERROR state (error)
        //:   6 Verify skipping an empty element '<Node0/>'
        //:   7 Verify skipping in larger XML, larger than the read buffer
        //:   8 Verify skipping in ill-formed large XML (error)
        //:   9 Verify skipping in partial large XML (error)
        //:  10 Verify skipping nested elements with the same name
        //
        // Testing:
        //   advanceToEndNodeRaw()
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nADVANCE TO END NODE RAW TEST"
                               << "\n============================"
                               << bsl::endl;

#define abcd " a='/Element1' b='/Element2' c='/Element3' d='/Node0' "
        static const char xmlRaw[] =
            "<?xml version='1.0' encoding='UTF-8'?>\n"
            "<!-- RCSId_bascfg_xsd = \"$Id: $\" -->\n"
            "<xs:schema xmlns:xs='http://www.w3.org/2001/XMLSchema'\n"
            "    elementFormDefault='qualified'\n"
            "    xmlns:bdem='http://bloomberg.com/schemas/bdem'\n"
            "    bdem:package='bascfg'>\n"
            "<Node0" abcd ">\n"
            "    <Element1" abcd ">element1"
            "<!-- The 1st element has a comment -->"
            "</Element1>\n"
            "    <Element2" abcd ">element2</Element2>\n"
            "    <Element3" abcd ">element3</Element3>\n"
            "    <![CDATA[&lt;123&#240;&gt;]]>\n"
            "</Node0>\n"
            "</xs:schema>";
#undef abcd

        bsl::string xmlStr(xmlRaw);
        nonAsciiUtf8(&xmlStr);

        if (veryVerbose) P(xmlStr);

        balxml::NamespaceRegistry namespaces;
        balxml::PrefixStack prefixStack(&namespaces);
        Mini miniReader; Obj reader(&miniReader);

        reader.setPrefixStack(&prefixStack);

        if (veryVerbose) bsl::cout << "\nSkip deepest element, Element3."
                                      "\n- - - - - - - - - - - - - - - -"
                                   << bsl::endl;
        {
            int rc = reader.open(xmlStr.c_str(), xmlStr.length());
            ASSERT(-1 < rc);

            ASSERT( reader.isOpen());
            ASSERT( reader.nodeType() == balxml::Reader::e_NODE_TYPE_NONE);

            advanceN(reader, 12);  // XML declaration --> <Element3>

            LOOP_ASSERT(rc,
                        (rc = miniReader.advanceToEndNodeRaw()) == 0);  // SKIP

            ASSERT(reader.nodeType() ==
                                      balxml::Reader::e_NODE_TYPE_END_ELEMENT);
            LOOP_ASSERT(reader.nodeName(),
                        !bsl::strcmp(reader.nodeName(), "Element3"));

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
        }

        if (veryVerbose) bsl::cout << "\nSkip Element1 (with comment)."
                                      "\n- - - - - - - - - - - - - - -"
                                   << bsl::endl;
        {
            int rc = reader.open(xmlStr.c_str(), xmlStr.length());
            ASSERT(-1 < rc);

            ASSERT( reader.isOpen());
            ASSERT( reader.nodeType() == balxml::Reader::e_NODE_TYPE_NONE);

            advanceN(reader, 5);  // XML declaration --> <Element1>

            LOOP_ASSERT(rc,
                        (rc = miniReader.advanceToEndNodeRaw()) == 0);  // SKIP

            ASSERT( reader.nodeType() ==
                                      balxml::Reader::e_NODE_TYPE_END_ELEMENT);
            ASSERT(!bsl::strcmp(reader.nodeName(), "Element1"));

            advanceN(reader, 9); // <Element2> --> </xs:schema>
            ASSERT( reader.nodeType() ==
                                      balxml::Reader::e_NODE_TYPE_END_ELEMENT);
            ASSERT(!bsl::strcmp(reader.nodeName(), "xs:schema"));

            reader.close();
        }

        if (veryVerbose) bsl::cout << "\nSkip inner element, Node0."
                                      "\n- - - - - - - - - - - - - "
                                   << bsl::endl;
        {
            int rc = reader.open(xmlStr.c_str(), xmlStr.length());
            ASSERT(-1 < rc);

            ASSERT( reader.isOpen());
            ASSERT( reader.nodeType() == balxml::Reader::e_NODE_TYPE_NONE);

            advanceN(reader, 4);  // XML declaration --> <Node0>

            LOOP_ASSERT(rc,
                        (rc = miniReader.advanceToEndNodeRaw()) == 0);  // SKIP

            ASSERT( reader.nodeType() ==
                                      balxml::Reader::e_NODE_TYPE_END_ELEMENT);
            LOOP_ASSERT(reader.nodeName(),
                        !bsl::strcmp(reader.nodeName(), "Node0"));

            rc = advancePastWhiteSpace(reader);
            ASSERT( reader.nodeType() ==
                                      balxml::Reader::e_NODE_TYPE_END_ELEMENT);
            LOOP_ASSERT(reader.nodeName(),
                        !bsl::strcmp(reader.nodeName(), "xs:schema"));

            reader.close();
        }

        if (veryVerbose) bsl::cout << "\nSkip outer element, xs:schema."
                                      "\n- - - - - - - - - - - - - - - "
                                   << bsl::endl;
        {
            int rc = reader.open(xmlStr.c_str(), xmlStr.length());
            ASSERT(-1 < rc);

            ASSERT( reader.isOpen());
            ASSERT( reader.nodeType() == balxml::Reader::e_NODE_TYPE_NONE);

            advanceN(reader, 3);  // XML declaration --> <xs:schema>

            LOOP_ASSERT(rc,
                        (rc = miniReader.advanceToEndNodeRaw()) == 0);  // SKIP

            ASSERT( reader.nodeType() ==
                                      balxml::Reader::e_NODE_TYPE_END_ELEMENT);
            ASSERT(!bsl::strcmp(reader.nodeName(), "xs:schema"));

            reader.close();
        }

        if (veryVerbose) bsl::cout << "\nTests in ST_EOF and ST_CLOSED state."
                                      "\n- - - - - - - - - - - - - - - - - - -"
                                   << bsl::endl;
        {
            static const char xmlRaw[] =
              "<?xml version='1.0' encoding='UTF-8'?>\n"
              "<xs:schema xmlns:xs='http://www.w3.org/2001/XMLSchema'\n"
              "    elementFormDefault='qualified'\n"
              "    xmlns:bdem='http://bloomberg.com/schemas/bdem'\n"
              "    bdem:package='bascfg'>\n"
              "    <Node0>text</Node0>\n"
              "</xs:schema>";

            bsl::string xmlStr(xmlRaw);
            nonAsciiUtf8(&xmlStr);

            int rc = reader.open(xmlStr.c_str(), xmlStr.length());
            ASSERT(-1 < rc);
            ASSERT(reader.isOpen());
            ASSERT(reader.nodeType() == balxml::Reader::e_NODE_TYPE_NONE);

            advanceN(reader, 2);  // XML declaration --> <xs:schema>

            LOOP_ASSERT(rc,
                        (rc = miniReader.advanceToEndNodeRaw()) == 0);  // SKIP

            ASSERT( reader.nodeType() ==
                                      balxml::Reader::e_NODE_TYPE_END_ELEMENT);
            ASSERT(!bsl::strcmp(reader.nodeName(), "xs:schema"));

            rc = advancePastWhiteSpace(reader); // Hit EOF here

            // Skip to end node fails, the parser is in the 'ST_EOF' state:

            LOOP_ASSERT(rc, (rc = miniReader.advanceToEndNodeRaw()) == -1);

            reader.close();

            // Skip to end node fails, the parser is in the 'ST_CLOSED' state:

            LOOP_ASSERT(rc, (rc = miniReader.advanceToEndNodeRaw()) == -1);
        }

        if (veryVerbose) bsl::cout << "\nTest in ST_ERROR state."
                                      "\n- - - - - - - - - - - -" << bsl::endl;
        {
            static const char xmlRaw[] =
              "<?xml version='1.0' encoding='UTF-8'?>\n"
              "<xs:schema xmlns:xs='http://www.w3.org/2001/XMLSchema'\n"
              "    elementFormDefault='qualified'\n"
              "    xmlns:bdem='http://bloomberg.com/schemas/bdem'\n"
              "    bdem:package='bascfg'>\n"
              "    <Node0></Node1>\n"
              "</xs:schema>";

            bsl::string xmlStr(xmlRaw);
            nonAsciiUtf8(&xmlStr);

            int rc = reader.open(xmlStr.c_str(), xmlStr.length());
            ASSERT(-1 < rc);
            ASSERT(reader.isOpen());
            ASSERT(reader.nodeType() == balxml::Reader::e_NODE_TYPE_NONE);

            advanceN(reader, 3);  // XML declaration --> <Node0>

            // Wrong end-element name, going into error state:

            LOOP_ASSERT(rc, (rc = advancePastWhiteSpace(reader)) < 0);

            // Skip to end node fails, the parser is in the 'ST_ERROR' state:

            LOOP_ASSERT(rc, (rc = miniReader.advanceToEndNodeRaw()) == -1);

            reader.close();
        }

        if (veryVerbose) bsl::cout << "\nTest empty element."
                                      "\n- - - - - - - - - -" << bsl::endl;
        {
            static const char xmlRaw[] =
              "<?xml version='1.0' encoding='UTF-8'?>\n"
              "<xs:schema xmlns:xs='http://www.w3.org/2001/XMLSchema'\n"
              "    elementFormDefault='qualified'\n"
              "    xmlns:bdem='http://bloomberg.com/schemas/bdem'\n"
              "    bdem:package='bascfg'>\n"
              "    <Node0/>\n"
              "</xs:schema>";

            bsl::string xmlStr(xmlRaw);
            nonAsciiUtf8(&xmlStr);

            int rc = reader.open(xmlStr.c_str(), xmlStr.length());
            ASSERT(-1 < rc);
            ASSERT(reader.isOpen());
            ASSERT(reader.nodeType() == balxml::Reader::e_NODE_TYPE_NONE);

            advanceN(reader, 3);  // XML declaration --> <Node0>

            LOOP_ASSERT(rc,
                        (rc = miniReader.advanceToEndNodeRaw()) == 0);  // SKIP

            ASSERT(reader.nodeType() == balxml::Reader::e_NODE_TYPE_ELEMENT);
            ASSERT(!bsl::strcmp(reader.nodeName(), "Node0"));

            reader.close();
        }

        if (veryVerbose) bsl::cout << "\nTest unclosed comment in skip area."
                                      "\n- - - - - - - - - - - - - - - - - -"
                                   << bsl::endl;
        {
            static const char xmlRaw[] =
              "<?xml version='1.0' encoding='UTF-8'?>\n"
              "<xs:schema xmlns:xs='http://www.w3.org/2001/XMLSchema'\n"
              "    elementFormDefault='qualified'\n"
              "    xmlns:bdem='http://bloomberg.com/schemas/bdem'\n"
              "    bdem:package='bascfg'>\n"
              "    <Node0><!--comment</Node0>\n"
              "</xs:schema>";

            bsl::string xmlStr(xmlRaw);
            nonAsciiUtf8(&xmlStr);

            int rc = reader.open(xmlStr.c_str(), xmlStr.length());
            ASSERT(-1 < rc);
            ASSERT(reader.isOpen());
            ASSERT(reader.nodeType() == balxml::Reader::e_NODE_TYPE_NONE);

            advanceN(reader, 2);  // XML declaration --> <xs:schema>

            // Skip to end node fails, cannot find the end of the comment:

            LOOP_ASSERT(rc, (rc = miniReader.advanceToEndNodeRaw()) == -1);
            ASSERT(reader.isError());
            LOOP_ASSERT(reader.errorInfo().message(),
                        reader.errorInfo().message() == "Unclosed comment.");

            reader.close();
        }

        if (veryVerbose) bsl::cout << "\nTest unclosed CDATA in skip area."
                                      "\n- - - - - - - - - - - - - - - - -"
                                   << bsl::endl;
        {
            static const char xmlRaw[] =
              "<?xml version='1.0' encoding='UTF-8'?>\n"
              "<xs:schema xmlns:xs='http://www.w3.org/2001/XMLSchema'\n"
              "    elementFormDefault='qualified'\n"
              "    xmlns:bdem='http://bloomberg.com/schemas/bdem'\n"
              "    bdem:package='bascfg'>\n"
              "    <Node0><![CDATA[cdata text</Node0>\n"
              "</xs:schema>";

            bsl::string xmlStr(xmlRaw);
            nonAsciiUtf8(&xmlStr);

            int rc = reader.open(xmlStr.c_str(), xmlStr.length());
            ASSERT(-1 < rc);
            ASSERT(reader.isOpen());
            ASSERT(reader.nodeType() == balxml::Reader::e_NODE_TYPE_NONE);

            advanceN(reader, 2);  // XML declaration --> <xs:schema>

            // Skip to end node fails, cannot find the end of the CDATA:

            LOOP_ASSERT(rc, (rc = miniReader.advanceToEndNodeRaw()) == -1);
            ASSERT(reader.isError());
            LOOP_ASSERT(reader.errorInfo().message(),
                        reader.errorInfo().message() == "Unclosed CDATA.");

            reader.close();
        }

        if (veryVerbose) bsl::cout << "\nTests large xml (*8K+)."
                                      "\n- - - - - - - - - - - -"
                                   << bsl::endl;
        {
            bsl::string xmlStr =
              "<?xml version='1.0' encoding='UTF-8'?>\n"
              "<xs:schema xmlns:xs='http://www.w3.org/2001/XMLSchema'\n"
              "    elementFormDefault='qualified'\n"
              "    xmlns:bdem='http://bloomberg.com/schemas/bdem'\n"
              "    bdem:package='bascfg'>\n"
              "<Node0>\n"
              "    <Element1>";

            xmlStr += bsl::string(8 * 1024, 'b');

            xmlStr += "</Element1>\n"
              "    <Element2>element2</Element2>\n"
              "    <Element3>element3</Element3>\n"
              "    <![CDATA[&lt;123&#240;&gt;]]>\n"
              "</Node0>\n"
              "</xs:schema>";

            nonAsciiUtf8(&xmlStr);

            int rc = reader.open(xmlStr.c_str(), xmlStr.length());
            ASSERT(-1 < rc);
            ASSERT(reader.isOpen());
            ASSERT(reader.nodeType() == balxml::Reader::e_NODE_TYPE_NONE);

            advanceN(reader, 2);  // XML declaration --> <xs:schema>

            // Skip to end node

            LOOP_ASSERT(rc,
                        (rc = miniReader.advanceToEndNodeRaw()) == 0);  // SKIP

            ASSERT(reader.nodeType() ==
                                      balxml::Reader::e_NODE_TYPE_END_ELEMENT);
            ASSERT(!bsl::strcmp(reader.nodeName(), "xs:schema"));

            reader.close();
        }

        if (veryVerbose) bsl::cout << "\nTests large xml (*8K+) with comment."
                                      "\n- - - - - - - - - - - - - - - - - - -"
                                   << bsl::endl;
        {
            bsl::string xmlStr =
              "<?xml version='1.0' encoding='UTF-8'?>\n"
              "<xs:schema xmlns:xs='http://www.w3.org/2001/XMLSchema'\n"
              "    elementFormDefault='qualified'\n"
              "    xmlns:bdem='http://bloomberg.com/schemas/bdem'\n"
              "    bdem:package='bascfg'>\n"
              "<Node0>\n"
              "    <Element1>";

            xmlStr += bsl::string(8 * 1024 - xmlStr.size() - 2, 'b');

            xmlStr += "<!--foobar--></Element1>\n"
              "    <Element2>element2</Element2>\n"
              "    <Element3>element3</Element3>\n"
              "    <![CDATA[&lt;123&#240;&gt;]]>\n"
              "</Node0>\n"
              "</xs:schema>";

            nonAsciiUtf8(&xmlStr);

            int rc = reader.open(xmlStr.c_str(), xmlStr.length());
            ASSERT(-1 < rc);
            ASSERT(reader.isOpen());
            ASSERT(reader.nodeType() == balxml::Reader::e_NODE_TYPE_NONE);

            advanceN(reader, 2);  // XML declaration --> <xs:schema>

            // Skip to end node

            LOOP_ASSERT(rc,
                        (rc = miniReader.advanceToEndNodeRaw()) == 0);  // SKIP

            ASSERT(reader.nodeType() ==
                                      balxml::Reader::e_NODE_TYPE_END_ELEMENT);
            ASSERT(!bsl::strcmp(reader.nodeName(), "xs:schema"));

            reader.close();
        }

        if (veryVerbose) bsl::cout << "\nTests large xml (*8K+) with CDATA."
                                      "\n- - - - - - - - - - - - - - - - - -"
                                   << bsl::endl;
        {
            bsl::string xmlStr =
              "<?xml version='1.0' encoding='UTF-8'?>\n"
              "<xs:schema xmlns:xs='http://www.w3.org/2001/XMLSchema'\n"
              "    elementFormDefault='qualified'\n"
              "    xmlns:bdem='http://bloomberg.com/schemas/bdem'\n"
              "    bdem:package='bascfg'>\n"
              "<Node0>\n"
              "    <Element1>";

            xmlStr += bsl::string(8 * 1024 - xmlStr.size() - 5, 'b');

            xmlStr += "<![CDATA[&lt;123&#240;&gt;]]></Element1>\n"
              "    <Element2>element2</Element2>\n"
              "    <Element3>element3</Element3>\n"
              "</Node0>\n"
              "</xs:schema>";

            nonAsciiUtf8(&xmlStr);

            int rc = reader.open(xmlStr.c_str(), xmlStr.length());
            ASSERT(-1 < rc);
            ASSERT(reader.isOpen());
            ASSERT(reader.nodeType() == balxml::Reader::e_NODE_TYPE_NONE);

            advanceN(reader, 2);  // XML declaration --> <xs:schema>

            // Skip to end node

            LOOP_ASSERT(rc,
                        (rc = miniReader.advanceToEndNodeRaw()) == 0);  // SKIP

            ASSERT(reader.nodeType() ==
                                      balxml::Reader::e_NODE_TYPE_END_ELEMENT);
            ASSERT(!bsl::strcmp(reader.nodeName(), "xs:schema"));

            reader.close();
        }

        if (veryVerbose) bsl::cout << "\nTests bad large xml (*8K+)."
                                      "\n- - - - - - - - - - - - - -"
                                   << bsl::endl;
        {
            bsl::string xmlStr =
              "<?xml version='1.0' encoding='UTF-8'?>\n"
              "<xs:schema xmlns:xs='http://www.w3.org/2001/XMLSchema'\n"
              "    elementFormDefault='qualified'\n"
              "    xmlns:bdem='http://bloomberg.com/schemas/bdem'\n"
              "    bdem:package='bascfg'>\n"
              "<Node0>\n"
              "    <Element1>";

            xmlStr += bsl::string(8 * 1024 - xmlStr.length() - 7, 'b');

            xmlStr += "</foobar</Element1>\n"
              "    <Element2>element2</Element2>\n"
              "    <Element3>element3</Element3>\n"
              "</Node0>\n"
              "</xs:schema>";

            nonAsciiUtf8(&xmlStr);

            int rc = reader.open(xmlStr.data(), xmlStr.size());
            ASSERT(-1 < rc);
            ASSERT(reader.isOpen());
            ASSERT(reader.nodeType() == balxml::Reader::e_NODE_TYPE_NONE);

            advanceN(reader, 2);  // XML declaration --> <xs:schema>

            // Skip to end node

            LOOP_ASSERT(rc,
                        (rc = miniReader.advanceToEndNodeRaw()) == 0);  // SKIP

            ASSERT(reader.nodeType() ==
                balxml::Reader::e_NODE_TYPE_END_ELEMENT);
            ASSERT(!bsl::strcmp(reader.nodeName(), "xs:schema"));

            reader.close();
        }

        if (veryVerbose) bsl::cout << "\nTests bad unended large xml (*8K+)."
                                      "\n- - - - - - - - - - - - - - - - - -"
                                   << bsl::endl;
        {
            bsl::string xmlStr =
              "<?xml version='1.0' encoding='UTF-8'?>\n"
              "<xs:schema xmlns:xs='http://www.w3.org/2001/XMLSchema'\n"
              "    elementFormDefault='qualified'\n"
              "    xmlns:bdem='http://bloomberg.com/schemas/bdem'\n"
              "    bdem:package='bascfg'>\n"
              "<Node0>\n"
              "    <Element1>";

            xmlStr += bsl::string(8 * 1024 - xmlStr.length() - 7, 'b');

            xmlStr += "</foobar";

            nonAsciiUtf8(&xmlStr);

            int rc = reader.open(xmlStr.data(), xmlStr.size());
            ASSERT(-1 < rc);
            ASSERT(reader.isOpen());
            ASSERT(reader.nodeType() == balxml::Reader::e_NODE_TYPE_NONE);

            advanceN(reader, 2);  // XML declaration --> <xs:schema>

            // Skip to end node

            LOOP_ASSERT(rc,
                        (rc = miniReader.advanceToEndNodeRaw()) == -2); // SKIP

            reader.close();
        }

        if (veryVerbose) bsl::cout << "\nTests unended large xml (*8K+)."
                                      "\n- - - - - - - - - - - - - - - -"
                                   << bsl::endl;
        {
            bsl::string xmlStr =
              "<?xml version='1.0' encoding='UTF-8'?>\n"
              "<xs:schema xmlns:xs='http://www.w3.org/2001/XMLSchema'\n"
              "    elementFormDefault='qualified'\n"
              "    xmlns:bdem='http://bloomberg.com/schemas/bdem'\n"
              "    bdem:package='bascfg'>\n"
              "<Node0>\n"
              "    <Element1>";

            xmlStr += bsl::string(8 * 1024, 'b');

            nonAsciiUtf8(&xmlStr);

            int rc = reader.open(xmlStr.data(), xmlStr.size());
            ASSERT(-1 < rc);
            ASSERT(reader.isOpen());
            ASSERT(reader.nodeType() == balxml::Reader::e_NODE_TYPE_NONE);

            advanceN(reader, 2);  // XML declaration --> <xs:schema>

            // Skip to end node

            LOOP_ASSERT(rc,
                        (rc = miniReader.advanceToEndNodeRaw()) == -2); // SKIP

            reader.close();
        }

        if (veryVerbose) bsl::cout << "\nTests large xml (*8K+) bad comment."
                                      "\n- - - - - - - - - - - - - - - - - -"
                                   << bsl::endl;
        {
            bsl::string xmlStr =
              "<?xml version='1.0' encoding='UTF-8'?>\n"
              "<xs:schema xmlns:xs='http://www.w3.org/2001/XMLSchema'\n"
              "    elementFormDefault='qualified'\n"
              "    xmlns:bdem='http://bloomberg.com/schemas/bdem'\n"
              "    bdem:package='bascfg'>\n"
              "<Node0>\n"
              "    <Element1>";

            xmlStr += bsl::string(8 * 1024 - xmlStr.size() - 2, 'b');

            xmlStr += "<!-";

            nonAsciiUtf8(&xmlStr);

            int rc = reader.open(xmlStr.data(), xmlStr.size());
            ASSERT(-1 < rc);
            ASSERT(reader.isOpen());
            ASSERT(reader.nodeType() == balxml::Reader::e_NODE_TYPE_NONE);

            advanceN(reader, 2);  // XML declaration --> <xs:schema>

            // Skip to end node

            LOOP_ASSERT(rc,
                        (rc = miniReader.advanceToEndNodeRaw()) == -2); // SKIP

            reader.close();
        }

        if (veryVerbose) bsl::cout << "\nTests large xml (*8K+) bad CDATA."
                                      "\n- - - - - - - - - - - - - - - - -"
                                   << bsl::endl;
        {
            bsl::string xmlStr =
              "<?xml version='1.0' encoding='UTF-8'?>\n"
              "<xs:schema xmlns:xs='http://www.w3.org/2001/XMLSchema'\n"
              "    elementFormDefault='qualified'\n"
              "    xmlns:bdem='http://bloomberg.com/schemas/bdem'\n"
              "    bdem:package='bascfg'>\n"
              "<Node0>\n"
              "    <Element1>";

            xmlStr += bsl::string(8 * 1024 - xmlStr.size() - 5, 'b');

            xmlStr += "<![CDA";

            nonAsciiUtf8(&xmlStr);

            int rc = reader.open(xmlStr.data(), xmlStr.size());
            ASSERT(-1 < rc);
            ASSERT(reader.isOpen());
            ASSERT(reader.nodeType() == balxml::Reader::e_NODE_TYPE_NONE);

            advanceN(reader, 2);  // XML declaration --> <xs:schema>

            // Skip to end node

            LOOP_ASSERT(rc,
                        (rc = miniReader.advanceToEndNodeRaw()) == -2); // SKIP

            reader.close();
        }

        if (veryVerbose) {
            bsl::cout << "\nTest nested elements with the same name."
                         "\n - - - - - - - - - - - - - - - - - - - -"
                      << bsl::endl;
        }
        {
            static const char xmlRaw[] =
              "<?xml version='1.0' encoding='UTF-8'?>\n"
              "<xs:schema xmlns:xs='http://www.w3.org/2001/XMLSchema'\n"
              "    elementFormDefault='qualified'\n"
              "    xmlns:bdem='http://bloomberg.com/schemas/bdem'\n"
              "    bdem:package='bascfg'>\n"
              "    <Node0>"
              "        <Node0>"
              "            <Node0>"
              "            </Node0>"
              "        </Node0>"
              "    </Node0>\n"
              "</xs:schema>";

            bsl::string xmlStr(xmlRaw);
            nonAsciiUtf8(&xmlStr);

            int rc = reader.open(xmlStr.c_str(), xmlStr.length());
            ASSERT(-1 < rc);
            ASSERT(reader.isOpen());
            ASSERT(reader.nodeType() == balxml::Reader::e_NODE_TYPE_NONE);

            advanceN(reader, 3);  // XML declaration --> <Node0>
            //advanceN(reader, 3);  // XML declaration --> <Node0>

            LOOP_ASSERT(rc,
                        (rc = miniReader.advanceToEndNodeRaw()) == 0);  // SKIP

            LOOP_ASSERT(reader.nodeType(),
                        reader.nodeType() ==
                                      balxml::Reader::e_NODE_TYPE_END_ELEMENT);
            LOOP_ASSERT(reader.nodeName(),
                        !bsl::strcmp(reader.nodeName(), "Node0"));

            LOOP2_ASSERT(rc, reader.errorInfo(),
                         (rc = advancePastWhiteSpace(reader)) == 0);

            ASSERT( reader.nodeType() ==
                                      balxml::Reader::e_NODE_TYPE_END_ELEMENT);
            LOOP_ASSERT(reader.nodeName(),
                        !bsl::strcmp(reader.nodeName(), "xs:schema"));

            reader.close();
        }

        if (veryVerbose) {
            bsl::cout << "\nTest nested elements with longer name."
                         "\n - - - - - - - - - - - - - - - - - - -"
                      << bsl::endl;
        }
        {
            static const char xmlRaw[] =
              "<?xml version='1.0' encoding='UTF-8'?>\n"
              "<xs:schema xmlns:xs='http://www.w3.org/2001/XMLSchema'\n"
              "    elementFormDefault='qualified'\n"
              "    xmlns:bdem='http://bloomberg.com/schemas/bdem'\n"
              "    bdem:package='bascfg'>\n"
              "    <Node0>"
              "        <Node0>"
              "            <Node01>"
              "            </Node01>"
              "        </Node0>"
              "    </Node0>\n"
              "</xs:schema>";

            bsl::string xmlStr(xmlRaw);
            nonAsciiUtf8(&xmlStr);

            int rc = reader.open(xmlStr.c_str(), xmlStr.length());
            ASSERT(-1 < rc);
            ASSERT(reader.isOpen());
            ASSERT(reader.nodeType() == balxml::Reader::e_NODE_TYPE_NONE);

            advanceN(reader, 3);  // XML declaration --> <Node0>
            //advanceN(reader, 3);  // XML declaration --> <Node0>

            LOOP_ASSERT(rc,
                        (rc = miniReader.advanceToEndNodeRaw()) == 0);  // SKIP

            LOOP_ASSERT(reader.nodeType(),
                        reader.nodeType() ==
                                      balxml::Reader::e_NODE_TYPE_END_ELEMENT);
            LOOP_ASSERT(reader.nodeName(),
                        !bsl::strcmp(reader.nodeName(), "Node0"));

            LOOP2_ASSERT(rc, reader.errorInfo(),
                         (rc = advancePastWhiteSpace(reader)) == 0);

            ASSERT( reader.nodeType() ==
                                      balxml::Reader::e_NODE_TYPE_END_ELEMENT);
            LOOP_ASSERT(reader.nodeName(),
                        !bsl::strcmp(reader.nodeName(), "xs:schema"));

            reader.close();
        }

        if (veryVerbose) {
            bsl::cout << "\nTest nested elements with longer name."
                         "\n - - - - - - - - - - - - - - - - - - -"
                      << bsl::endl;
        }
        {
            static const char xmlRaw[] =
              "<?xml version='1.0' encoding='UTF-8'?>\n"
              "<xs:schema xmlns:xs='http://www.w3.org/2001/XMLSchema'\n"
              "    elementFormDefault='qualified'\n"
              "    xmlns:bdem='http://bloomberg.com/schemas/bdem'\n"
              "    bdem:package='bascfg'>\n"
              "    <Node0>"
              "        <Node0>"
              "            <LongNode0>"
              "            </LongNode0>"
              "        </Node0>"
              "    </Node0>\n"
              "</xs:schema>";

            bsl::string xmlStr(xmlRaw);
            nonAsciiUtf8(&xmlStr);

            int rc = reader.open(xmlStr.c_str(), xmlStr.length());
            ASSERT(-1 < rc);
            ASSERT(reader.isOpen());
            ASSERT(reader.nodeType() == balxml::Reader::e_NODE_TYPE_NONE);

            advanceN(reader, 3);  // XML declaration --> <Node0>

            LOOP_ASSERT(rc,
                        (rc = miniReader.advanceToEndNodeRaw()) == 0);  // SKIP

            LOOP_ASSERT(reader.nodeType(),
                        reader.nodeType() ==
                                      balxml::Reader::e_NODE_TYPE_END_ELEMENT);
            LOOP_ASSERT(reader.nodeName(),
                        !bsl::strcmp(reader.nodeName(), "Node0"));

            LOOP2_ASSERT(rc, reader.errorInfo(),
                         (rc = advancePastWhiteSpace(reader)) == 0);

            ASSERT( reader.nodeType() ==
                                      balxml::Reader::e_NODE_TYPE_END_ELEMENT);
            LOOP_ASSERT(reader.nodeName(),
                        !bsl::strcmp(reader.nodeName(), "xs:schema"));

            reader.close();
        }

        if (veryVerbose) {
            bsl::cout << "\nDRQS 118964602 - Failure due to attributes"
                         "\n - - - - - - - - - - - - - - - - - - - - -"
                      << bsl::endl;
        }
        {
            static const char xmlStr[] =
              "<aaa><bbb attr=\"1\"><bbb attr=\"2\"></bbb></bbb></aaa>";

            int rc = reader.open(xmlStr, bsl::strlen(xmlStr));
            ASSERT(-1 < rc);
            ASSERT(reader.isOpen());
            ASSERT(reader.nodeType() == balxml::Reader::e_NODE_TYPE_NONE);

            advanceN(reader, 2);  // aaa --> first bbb

            LOOP_ASSERT(reader.nodeType(),
                        reader.nodeType() ==
                                          balxml::Reader::e_NODE_TYPE_ELEMENT);
            LOOP_ASSERT(reader.nodeName(),
                        !bsl::strcmp(reader.nodeName(), "bbb"));

            LOOP_ASSERT(rc,
                        (rc = miniReader.advanceToEndNodeRaw()) == 0);  // SKIP

            LOOP_ASSERT(reader.nodeType(),
                        reader.nodeType() ==
                                      balxml::Reader::e_NODE_TYPE_END_ELEMENT);
            LOOP_ASSERT(reader.nodeName(),
                        !bsl::strcmp(reader.nodeName(), "bbb"));

            LOOP_ASSERT(rc, (rc = reader.advanceToNextNode()) == 0); // to /aaa

            LOOP_ASSERT(reader.nodeType(),
                        reader.nodeType() ==
                                      balxml::Reader::e_NODE_TYPE_END_ELEMENT);
            LOOP_ASSERT(reader.nodeName(),
                        !bsl::strcmp(reader.nodeName(), "aaa"));

            reader.close();
        }
        { // Bad xml, second bbb has attribute but no closing '>'
            static const char xmlStr[] =
                                        "<aaa><bbb attr=\"1\"><bbb attr=\"2\"";

            int rc = reader.open(xmlStr, bsl::strlen(xmlStr));
            ASSERT(-1 < rc);
            ASSERT(reader.isOpen());
            ASSERT(reader.nodeType() == balxml::Reader::e_NODE_TYPE_NONE);

            advanceN(reader, 2);  // aaa --> first bbb

            LOOP_ASSERT(reader.nodeType(),
                        reader.nodeType() ==
                                          balxml::Reader::e_NODE_TYPE_ELEMENT);
            LOOP_ASSERT(reader.nodeName(),
                        !bsl::strcmp(reader.nodeName(), "bbb"));

            LOOP_ASSERT(rc, (rc = miniReader.advanceToEndNodeRaw()) == -2);

            reader.close();
        }
      } break;

      case 12: {
        // --------------------------------------------------------------------
        // ADVANCE TO END NODE TEST
        //
        // Concerns:
        //: 1 'advanceToEndNode' function skips all (text, CDATA, comment,
        //:   whitespace, start element, end element for skipped elements) node
        //:   types until it finds the end element for the element it was
        //:   called for.
        //:
        //: 2 Calling 'advanceToEndNodeRaw' in states 'ST_EOF', 'ST_CLOSE' or
        //:   'ST_ERROR' is a no-op and it returns an error.
        //:
        //: 3 Calling 'advanceToEndNodeRaw' on an empty element is a no-op and
        //:   returns success.
        //:
        //: 4 Node names crosses buffer boundaries are found properly.
        //:
        //: 5 Function returns error for bad XML.
        //
        // Plan:
        //: 1 Create input XML strings and call 'advanceToEndNodeRawBare' in
        //:   several states of them:
        //:   1 Verify skipping the deepest element
        //:   2 Verify skipping mid elements
        //:   3 Verify skipping outer element
        //:   4 Verify skipping in EOF and CLOSE state (error)
        //:   5 Verify skipping in ERROR state (error)
        //:   6 Verify skipping an empty element '<Node0/>'
        //:   7 Verify skipping in larger XML, larger than the read buffer
        //:   8 Verify skipping in ill-formed large XML (error)
        //:   9 Verify skipping in partial large XML (error)
        //:  10 Verify skipping nested elements with the same name
        //
        // Testing:
        //   advanceToEndNode()
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nADVANCE TO END NODE TEST"
                               << "\n========================" << bsl::endl;

#define abcd " a='/Element1' b='/Element2' c='/Element3' d='/Node0' "
        static const char xmlRaw[] =
          "<?xml version='1.0' encoding='UTF-8'?>\n"
          "<!-- RCSId_bascfg_xsd = \"$Id: $\" -->\n"
          "<xs:schema xmlns:xs='http://www.w3.org/2001/XMLSchema'\n"
          "    elementFormDefault='qualified'\n"
          "    xmlns:bdem='http://bloomberg.com/schemas/bdem'\n"
          "    bdem:package='bascfg'>\n"
          "<Node0" abcd ">\n"
          "    <Element1" abcd ">element1<!-- The 1st elem has a comment -->"
          "</Element1>\n"
          "    <Element2" abcd ">element2</Element2>\n"
          "    <Element3" abcd ">element3</Element3>\n"
          "    <![CDATA[&lt;123&#240;&gt;]]>\n"
          "</Node0>\n"
          "</xs:schema>";
#undef abcd

        bsl::string xmlStr(xmlRaw);
        nonAsciiUtf8(&xmlStr);

        if (veryVerbose) P(xmlStr);

        balxml::NamespaceRegistry namespaces;
        balxml::PrefixStack prefixStack(&namespaces);
        Mini miniReader; Obj reader(&miniReader);

        reader.setPrefixStack(&prefixStack);

        if (veryVerbose) bsl::cout << "\nSkip deepest element, Element3."
                                      "\n- - - - - - - - - - - - - - - -"
                                   << bsl::endl;
        {
            int rc = reader.open(xmlStr.c_str(), xmlStr.length());
            ASSERT(-1 < rc);

            ASSERT( reader.isOpen());
            ASSERT( reader.nodeType() == balxml::Reader::e_NODE_TYPE_NONE);

            advanceN(reader, 12);  // XML declaration --> <Element3>

            LOOP_ASSERT(rc,
                        (rc = miniReader.advanceToEndNode()) == 0);     // SKIP

            ASSERT(reader.nodeType() ==
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
        }

        if (veryVerbose) bsl::cout << "\nSkip Element1 (with comment)."
                                      "\n- - - - - - - - - - - - - - -"
                                   << bsl::endl;
        {
            int rc = reader.open(xmlStr.c_str(), xmlStr.length());
            ASSERT(-1 < rc);

            ASSERT( reader.isOpen());
            ASSERT( reader.nodeType() == balxml::Reader::e_NODE_TYPE_NONE);

            advanceN(reader, 5);  // XML declaration --> <Element1>

            LOOP_ASSERT(rc,
                        (rc = miniReader.advanceToEndNode()) == 0);     // SKIP

            ASSERT( reader.nodeType() ==
                                      balxml::Reader::e_NODE_TYPE_END_ELEMENT);
            ASSERT(!bsl::strcmp(reader.nodeName(), "Element1"));

            advanceN(reader, 9);  // </Element1> --> <xs:schema>
            ASSERT( reader.nodeType() ==
                                      balxml::Reader::e_NODE_TYPE_END_ELEMENT);
            ASSERT(!bsl::strcmp(reader.nodeName(), "xs:schema"));

            reader.close();
        }

        if (veryVerbose) bsl::cout << "\nSkip inner element, Node0."
                                      "\n- - - - - - - - - - - - - "
                                   << bsl::endl;
        {
            int rc = reader.open(xmlStr.c_str(), xmlStr.length());
            ASSERT(-1 < rc);

            ASSERT( reader.isOpen());
            ASSERT( reader.nodeType() == balxml::Reader::e_NODE_TYPE_NONE);

            advanceN(reader, 4);  // XML declaration --> <Node0>

            LOOP_ASSERT(rc,
                        (rc = miniReader.advanceToEndNode()) == 0);     // SKIP

            ASSERT( reader.nodeType() ==
                                      balxml::Reader::e_NODE_TYPE_END_ELEMENT);
            ASSERT(!bsl::strcmp(reader.nodeName(), "Node0"));

            rc = advancePastWhiteSpace(reader);
            ASSERT( reader.nodeType() ==
                                      balxml::Reader::e_NODE_TYPE_END_ELEMENT);
            ASSERT(!bsl::strcmp(reader.nodeName(), "xs:schema"));

            reader.close();
        }

        if (veryVerbose) bsl::cout << "\nSkip outer element, xs:schema."
                                      "\n- - - - - - - - - - - - - - - "
                                   << bsl::endl;
        {
            int rc = reader.open(xmlStr.c_str(), xmlStr.length());
            ASSERT(-1 < rc);

            ASSERT( reader.isOpen());
            ASSERT( reader.nodeType() == balxml::Reader::e_NODE_TYPE_NONE);

            advanceN(reader, 3);  // XML declaration --> <xs:schema>

            LOOP_ASSERT(rc,
                        (rc = miniReader.advanceToEndNode()) == 0);     // SKIP

            ASSERT( reader.nodeType() ==
                                      balxml::Reader::e_NODE_TYPE_END_ELEMENT);
            ASSERT(!bsl::strcmp(reader.nodeName(), "xs:schema"));

            reader.close();
        }

        if (veryVerbose) bsl::cout << "\nTests in ST_EOF and ST_CLOSED state."
                                      "\n- - - - - - - - - - - - - - - - - - -"
                                   << bsl::endl;
        {
            static const char xmlRaw[] =
              "<?xml version='1.0' encoding='UTF-8'?>\n"
              "<xs:schema xmlns:xs='http://www.w3.org/2001/XMLSchema'\n"
              "    elementFormDefault='qualified'\n"
              "    xmlns:bdem='http://bloomberg.com/schemas/bdem'\n"
              "    bdem:package='bascfg'>\n"
              "    <Node0>text</Node0>\n"
              "</xs:schema>";

            bsl::string xmlStr(xmlRaw);
            nonAsciiUtf8(&xmlStr);

            int rc = reader.open(xmlStr.c_str(), xmlStr.length());
            ASSERT(-1 < rc);
            ASSERT(reader.isOpen());
            ASSERT(reader.nodeType() == balxml::Reader::e_NODE_TYPE_NONE);

            advanceN(reader, 2);  // XML declaration --> <xs:schema>

            LOOP_ASSERT(rc,
                        (rc = miniReader.advanceToEndNode()) == 0);     // SKIP

            ASSERT( reader.nodeType() ==
                                      balxml::Reader::e_NODE_TYPE_END_ELEMENT);
            ASSERT(!bsl::strcmp(reader.nodeName(), "xs:schema"));

            rc = advancePastWhiteSpace(reader); // Hit EOF here

            // Skip to end node fails, the parser is in the 'ST_EOF' state:

            LOOP_ASSERT(rc, (rc = miniReader.advanceToEndNode()) == -1);

            reader.close();

            // Skip to end node fails, the parser is in the 'ST_CLOSED' state:

            LOOP_ASSERT(rc, (rc = miniReader.advanceToEndNode()) == -1);
        }

        if (veryVerbose) bsl::cout << "\nTest in ST_ERROR state."
                                      "\n- - - - - - - - - - - -" << bsl::endl;
        {
            static const char xmlRaw[] =
              "<?xml version='1.0' encoding='UTF-8'?>\n"
              "<xs:schema xmlns:xs='http://www.w3.org/2001/XMLSchema'\n"
              "    elementFormDefault='qualified'\n"
              "    xmlns:bdem='http://bloomberg.com/schemas/bdem'\n"
              "    bdem:package='bascfg'>\n"
              "    <Node0></Node1>\n"
              "</xs:schema>";

            bsl::string xmlStr(xmlRaw);
            nonAsciiUtf8(&xmlStr);

            int rc = reader.open(xmlStr.c_str(), xmlStr.length());
            ASSERT(-1 < rc);
            ASSERT(reader.isOpen());
            ASSERT(reader.nodeType() == balxml::Reader::e_NODE_TYPE_NONE);

            advanceN(reader, 3);  // XML declaration --> <Node0>

            // Wrong end-element name, going into error state:

            LOOP_ASSERT(rc, (rc = advancePastWhiteSpace(reader)) < 0);

            // Skip to end node fails, the parser is in the 'ST_ERROR' state:

            LOOP_ASSERT(rc, (rc = miniReader.advanceToEndNode()) == -1);

            reader.close();
        }

        if (veryVerbose) bsl::cout << "\nTest empty element."
                                      "\n- - - - - - - - - -" << bsl::endl;
        {
            static const char xmlRaw[] =
              "<?xml version='1.0' encoding='UTF-8'?>\n"
              "<xs:schema xmlns:xs='http://www.w3.org/2001/XMLSchema'\n"
              "    elementFormDefault='qualified'\n"
              "    xmlns:bdem='http://bloomberg.com/schemas/bdem'\n"
              "    bdem:package='bascfg'>\n"
              "    <Node0/>\n"
              "</xs:schema>";

            bsl::string xmlStr(xmlRaw);
            nonAsciiUtf8(&xmlStr);

            int rc = reader.open(xmlStr.c_str(), xmlStr.length());
            ASSERT(-1 < rc);
            ASSERT(reader.isOpen());
            ASSERT(reader.nodeType() == balxml::Reader::e_NODE_TYPE_NONE);

            advanceN(reader, 3);  // XML declaration --> <Node0>

            LOOP_ASSERT(rc,
                        (rc = miniReader.advanceToEndNode()) == 0);     // SKIP

            ASSERT(reader.nodeType() == balxml::Reader::e_NODE_TYPE_ELEMENT);
            ASSERT(!bsl::strcmp(reader.nodeName(), "Node0"));

            reader.close();
        }

        if (veryVerbose) bsl::cout << "\nTest elements with the same name."
                                      "\n- - - - - - - - - - - - - - - - -"
                                   << bsl::endl;
        {
            static const char xmlRaw[] =
              "<?xml version='1.0' encoding='UTF-8'?>\n"
              "<xs:schema xmlns:xs='http://www.w3.org/2001/XMLSchema'\n"
              "    elementFormDefault='qualified'\n"
              "    xmlns:bdem='http://bloomberg.com/schemas/bdem'\n"
              "    bdem:package='bascfg'>\n"
              "    <Node0>"
              "        <Node0>"
              "            <Node0>"
              "            </Node0>"
              "        </Node0>"
              "    </Node0>\n"
              "</xs:schema>";

            bsl::string xmlStr(xmlRaw);
            nonAsciiUtf8(&xmlStr);

            int rc = reader.open(xmlStr.c_str(), xmlStr.length());
            ASSERT(-1 < rc);
            ASSERT(reader.isOpen());
            ASSERT(reader.nodeType() == balxml::Reader::e_NODE_TYPE_NONE);

            advanceN(reader, 3);  // XML declaration --> <Node0>

            LOOP_ASSERT(rc,
                        (rc = miniReader.advanceToEndNode()) == 0);     // SKIP

            ASSERT(reader.nodeType() ==
                                      balxml::Reader::e_NODE_TYPE_END_ELEMENT);
            ASSERT(!bsl::strcmp(reader.nodeName(), "Node0"));

            LOOP_ASSERT(rc, (rc = advancePastWhiteSpace(reader)) == 0);

            ASSERT( reader.nodeType() ==
                                      balxml::Reader::e_NODE_TYPE_END_ELEMENT);
            ASSERT(!bsl::strcmp(reader.nodeName(), "xs:schema"));

            reader.close();
        }

        if (veryVerbose) bsl::cout << "\nTest elements with longer name."
                                      "\n- - - - - - - - - - - - - - - -"
                                   << bsl::endl;
        {
            static const char xmlRaw[] =
              "<?xml version='1.0' encoding='UTF-8'?>\n"
              "<xs:schema xmlns:xs='http://www.w3.org/2001/XMLSchema'\n"
              "    elementFormDefault='qualified'\n"
              "    xmlns:bdem='http://bloomberg.com/schemas/bdem'\n"
              "    bdem:package='bascfg'>\n"
              "    <Node0>"
              "        <Node0>"
              "            <Node01>"
              "            </Node01>"
              "        </Node0>"
              "    </Node0>\n"
              "</xs:schema>";

            bsl::string xmlStr(xmlRaw);
            nonAsciiUtf8(&xmlStr);

            int rc = reader.open(xmlStr.c_str(), xmlStr.length());
            ASSERT(-1 < rc);
            ASSERT(reader.isOpen());
            ASSERT(reader.nodeType() == balxml::Reader::e_NODE_TYPE_NONE);

            advanceN(reader, 3);  // XML declaration --> <Node0>

            LOOP_ASSERT(rc,
                        (rc = miniReader.advanceToEndNode()) == 0);     // SKIP

            ASSERT(reader.nodeType() ==
                                      balxml::Reader::e_NODE_TYPE_END_ELEMENT);
            ASSERT(!bsl::strcmp(reader.nodeName(), "Node0"));

            LOOP_ASSERT(rc, (rc = advancePastWhiteSpace(reader)) == 0);

            ASSERT( reader.nodeType() ==
                                      balxml::Reader::e_NODE_TYPE_END_ELEMENT);
            ASSERT(!bsl::strcmp(reader.nodeName(), "xs:schema"));

            reader.close();
        }

        if (veryVerbose) bsl::cout << "\nTest elements with longer name."
                                      "\n- - - - - - - - - - - - - - - -"
                                   << bsl::endl;
        {
            static const char xmlRaw[] =
              "<?xml version='1.0' encoding='UTF-8'?>\n"
              "<xs:schema xmlns:xs='http://www.w3.org/2001/XMLSchema'\n"
              "    elementFormDefault='qualified'\n"
              "    xmlns:bdem='http://bloomberg.com/schemas/bdem'\n"
              "    bdem:package='bascfg'>\n"
              "    <Node0>"
              "        <Node0>"
              "            <LongNode0>"
              "            </LongNode0>"
              "        </Node0>"
              "    </Node0>\n"
              "</xs:schema>";

            bsl::string xmlStr(xmlRaw);
            nonAsciiUtf8(&xmlStr);

            int rc = reader.open(xmlStr.c_str(), xmlStr.length());
            ASSERT(-1 < rc);
            ASSERT(reader.isOpen());
            ASSERT(reader.nodeType() == balxml::Reader::e_NODE_TYPE_NONE);

            advanceN(reader, 3);  // XML declaration --> <Node0>

            LOOP_ASSERT(rc,
                        (rc = miniReader.advanceToEndNode()) == 0);     // SKIP

            ASSERT(reader.nodeType() ==
                                      balxml::Reader::e_NODE_TYPE_END_ELEMENT);
            ASSERT(!bsl::strcmp(reader.nodeName(), "Node0"));

            LOOP_ASSERT(rc, (rc = advancePastWhiteSpace(reader)) == 0);

            ASSERT( reader.nodeType() ==
                                      balxml::Reader::e_NODE_TYPE_END_ELEMENT);
            ASSERT(!bsl::strcmp(reader.nodeName(), "xs:schema"));

            reader.close();
        }
      } break;

      case 11: {
        // --------------------------------------------------------------------
        // TESTING 'xsi:nil' attribute
        //
        // Plan:
        //
        // Testing:
        //   isEmptyElement()
        //   numAttributes()
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << bsl::endl
                               << "Testing 'xsi:nil' attribute"   << bsl::endl
                               << "===========================\n" << bsl::endl;

        {
            static const bsl::string xmlRaw =
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

        bsl::string xmlStr(xmlRaw);
        nonAsciiUtf8(&xmlStr);

          if (veryVerbose) P(xmlStr);

          balxml::NamespaceRegistry namespaces;
          balxml::PrefixStack prefixStack(&namespaces);
          Mini miniReader; Obj reader(&miniReader);

          reader.setPrefixStack(&prefixStack);

          int rc = reader.open(xmlStr.c_str(), xmlStr.length());
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

            static const bsl::string xmlRaw =
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

          bsl::string xmlStr(xmlRaw);
          nonAsciiUtf8(&xmlStr);

          if (veryVerbose) P(xmlStr);

          balxml::NamespaceRegistry namespaces;
          balxml::PrefixStack prefixStack(&namespaces);
          Mini miniReader; Obj reader(&miniReader);

          reader.setPrefixStack(&prefixStack);

          int rc = reader.open(xmlStr.c_str(), xmlStr.length());
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
          ASSERT(reader.nodeType() == balxml::Reader::e_NODE_TYPE_END_ELEMENT);
          ASSERT(!bsl::strcmp(reader.nodeName(), "Element"));

          rc = advancePastWhiteSpace(reader);
          ASSERT(reader.nodeType() == balxml::Reader::e_NODE_TYPE_END_ELEMENT);
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

        int numEl = sizeof(table91) / sizeof(table91[0]);

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
        // Testing:
        //   nodeEndPosition()
        // --------------------------------------------------------------------
        if (verbose) bsl::cout << "\nNODE POSITIONS TEST"
                               << "\n==================="
                               << bsl::endl;

        int numEl = sizeof(table81) / sizeof(table81[0]);

        parseAndCompare("table81", table81, numEl);

      } break;
      case 7: {
        // --------------------------------------------------------------------
        // LOTS OF ATTRIBUTES (STRESS TEST)
        //
        // Concerns:
        //: 1 That balxml::MiniReader operate correctly with xml strings
        //:   (buffers) with many attributes while using the
        //:   'balxml::Utf8ReaderWrapper'.  The strings in the test vectors all
        //:   contain a *single element* with different number of attributes.
        //
        // Plan:
        //: 1 Construct a 'balxml::MiniReader' reader capable of buffering
        //:   'bufferSize' bytes of an XML document created with many
        //:   attributes, wrapped with a 'balxml::Utf8ReaderWrapper'.  For each
        //:   test vector generate the XML input starting with the
        //:   'strXmlStart' template, add attributes to a single node to get
        //:   the desired input size and then close the input with the
        //:   'strXmlEnd' template.  Use the reader to parse the input and
        //:   verify that the node and all its attributes are intact.  The
        //:   'bufferSize' is used to perturb the testing with different
        //:   initial buffer sizes, this will force the reader to operate
        //:   differently.  I.e., the reader will need to stop more frequently
        //:   to read in input with smaller buffer size while using less
        //:   memory, while the exact opposite will be true with larger
        //:   'bufferSize's'.
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
          Mini miniReader(BUFFER); Obj reader(&miniReader);

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

          if (veryVerbose) {
              bsl::cout << "\nlength: " << xmlStr.length() << bsl::endl;
          }

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
        //: 1 That 'balxml::MiniReader' operate correctly with xml strings
        //:   (buffers) with long node names when wrapped with a
        //:   'balxml::Utf8ReaderWrapper'.  The strings in the test vectors all
        //:   contain a *single element* with a name of different lengths.
        //
        // Plan:
        //: 1 Construct a 'balxml::MiniReader' reader capable of buffering
        //:   'bufferSize' bytes of an XML document created with long node
        //:   names and wrap it with a 'balxml::Utf8ReaderWrapper'.  For each
        //:   test vector generate the XML input starting with the
        //:   'strXmlStart' template, add a single node with a name of 'length'
        //:   to get the desired input size and then close the input with the
        //:   'strXmlEnd' template.  Use the reader to parse the input and
        //:   verify that the node and its name are intact.  The 'bufferSize'
        //:   is used to perturb the testing with different initial buffer
        //:   sizes, this will force the reader to operate differently.  I.e.,
        //:   the reader will need to stop more frequently to read in input
        //:   with smaller buffer size while using less memory, while the exact
        //:   opposite will be true with larger 'bufferSize's'.
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
          Mini miniReader(BUFFER); Obj reader(&miniReader);

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

          if (veryVerbose) {
              bsl::cout << "\nlength: " << xmlStr.length() << bsl::endl;
          }

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
        //: 1 That balxml::MiniReader operate correctly with xml strings
        //:   (buffers) of different lengths while wrapped with a
        //:   'balxml::Utf8ReaderWrapper'.  The strings in the test vectors
        //:   will contain elements of different breadth and depth.
        //
        // Plan:
        //: 1 Construct a 'balxml::MiniReader' reader wrapped with a
        //:   'balxml::Utf8ReaderWrapper' capable of buffering 'bufferSize'
        //:   bytes of an XML document created with the 'ggg' function.  The
        //:   XML documents created with the 'ggg' function will be of lengths
        //:   and depths that force 'pointer rebase' within the
        //:   'balxml::MiniReader'.  The 'bufferSize' is used to perturb the
        //:   testing with different initial buffer sizes, this will force the
        //:   reader to operate differently.  I.e., the reader will need to
        //:   stop more frequently to read in input with smaller buffer size
        //:   while using less memory, while the exact opposite will be true
        //:   with larger 'bufferSize's'.
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
          Mini miniReader(BUFFER); Obj reader(&miniReader);

          reader.setPrefixStack(&prefixStack);

          bsl::string xmlStr;
          ggg(xmlStr, NODES, DEPTH);

          nonAsciiUtf8(&xmlStr);

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

          if (veryVerbose) {
              bsl::cout << "\nnodes: " << NODES
                        << " depth: "  << DEPTH
                        << " length: " << xmlStr.length()
                        << " bytes: "  << bytes
                        << bsl::endl;
          }

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
        // TESTING ADVANCETONEXTNODE WITH BAD INPUT
        //
        // Plan:
        // Create and open a 'balxml::MiniReader' wrapped with a
        // 'balxml::Utf8ReaderWrapper' with strings containing invalid inputs.
        // Assert that calls to advanceToNextNode fail with the proper errors.
        //
        // Testing:
        //  int advanceToNextNode();
        //  balxml::ErrorInfo& errorInfo();
        //  close();
        // --------------------------------------------------------------------
        if (verbose) bsl::cout << "\nTESTING ADVANCETONEXTNODE WITH BAD INPUT"
                               << "\n========================================"
                               << bsl::endl;

        if (verbose) bsl::cout << "Testing 'advanceToNextNode'"   << bsl::endl
                               << "- - - - - - - - - - - - - -\n" << bsl::endl;

        if (verbose) bsl::cout << "Bad closing tag."   << bsl::endl
                               << "-  -  -  -  -  -\n" << bsl::endl;
        {
          static const char xmlRaw[] =
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

          bsl::string xmlStr(xmlRaw);
          nonAsciiUtf8(&xmlStr);

          if (veryVerbose) P(xmlStr);

          balxml::NamespaceRegistry namespaces;
          balxml::PrefixStack prefixStack(&namespaces);
          Mini miniReader; Obj reader(&miniReader);

          reader.setPrefixStack(&prefixStack);

          int rc = reader.open(xmlStr.c_str(), xmlStr.length());
          ASSERT(-1 < rc);
          ASSERT(reader.isOpen());

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

          ASSERT(reader.isOpen());
          reader.close();
          ASSERT(!reader.isOpen());
        }

        if (verbose) bsl::cout << "No closing tag."    << bsl::endl
                               << "-  -  -  -  -  -\n" << bsl::endl;
        {
          static const char xmlRaw[] =
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

          bsl::string xmlStr(xmlRaw);
          nonAsciiUtf8(&xmlStr);

          if (veryVerbose) P(xmlStr);

          balxml::NamespaceRegistry namespaces;
          balxml::PrefixStack prefixStack(&namespaces);
          Mini miniReader; Obj reader(&miniReader);

          reader.setPrefixStack(&prefixStack);

          int rc = reader.open(xmlStr.c_str(), xmlStr.length());
          ASSERT(-1 < rc);
          ASSERT(reader.isOpen());

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

          ASSERT(reader.isOpen());
          reader.close();
          ASSERT(!reader.isOpen());
        }

        if (verbose) bsl::cout << "Missing closing quote on attributes."
                               << bsl::endl
                               << "-  -  -  -  -  -  -  -  -  -  -  -  -\n"
                               << bsl::endl;
        {
          // closing quote is missing on the xmlns:bdem attribute within the
          // xs:schema element
          static const char xmlRaw[] =
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

          bsl::string xmlStr(xmlRaw);
          nonAsciiUtf8(&xmlStr);

          if (veryVerbose) P(xmlStr);

          balxml::NamespaceRegistry namespaces;
          balxml::PrefixStack prefixStack(&namespaces);
          Mini miniReader; Obj reader(&miniReader);

          reader.setPrefixStack(&prefixStack);

          int rc = reader.open(xmlStr.c_str(), xmlStr.length());
          ASSERT(-1 < rc);
          ASSERT(reader.isOpen());

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
          ASSERT( reader.nodeType() == balxml::Reader::e_NODE_TYPE_COMMENT);
          ASSERT(!bsl::strcmp(reader.nodeValue(),
                      " RCSId_bascfg_xsd = \"$Id: $\" "));

          rc = advancePastWhiteSpace(reader);
          ASSERT( reader.nodeType() == balxml::Reader::e_NODE_TYPE_ELEMENT);
          ASSERT(!bsl::strcmp(reader.nodeName(), "xs:schema"));
          ASSERT(0 > rc);
          const balxml::ErrorInfo& errorInfo = reader.errorInfo();
          LOOP_ASSERT(errorInfo.lineNumber(), 6 == errorInfo.lineNumber());
          LOOP_ASSERT(errorInfo.columnNumber(),
                      26 == errorInfo.columnNumber());
          LOOP_ASSERT(errorInfo.message(),
                      npos != errorInfo.message().find("No space"));

          ASSERT(reader.isOpen());
          reader.close();
          ASSERT(!reader.isOpen());
        }

        if (verbose) bsl::cout << "Missing '=' attributes."
                               << bsl::endl
                               << "-  -  -  -  -  -  -  -  -\n"
                               << bsl::endl;
        {
          // closing quote is missing on the xmlns:bdem attribute within the
          // xs:schema element
          static const char xmlRaw[] =
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

          bsl::string xmlStr(xmlRaw);
          nonAsciiUtf8(&xmlStr);

          if (veryVerbose) { P(xmlStr); }

          balxml::NamespaceRegistry namespaces;
          balxml::PrefixStack prefixStack(&namespaces);
          Mini miniReader; Obj reader(&miniReader);

          reader.setPrefixStack(&prefixStack);

          int rc = reader.open(xmlStr.c_str(), xmlStr.length());
          ASSERT(-1 < rc);
          ASSERT(reader.isOpen());

          ASSERT( reader.isOpen());
          ASSERT( reader.nodeType() == balxml::Reader::e_NODE_TYPE_NONE);

          rc = advancePastWhiteSpace(reader);
          ASSERT( reader.nodeType() == balxml::Reader::e_NODE_TYPE_ELEMENT);
          ASSERT(!bsl::strcmp(reader.nodeName(), "xs:schema"));
          LOOP_ASSERT(rc, 0 > rc);
          const balxml::ErrorInfo& errorInfo = reader.errorInfo();
          LOOP_ASSERT(errorInfo.lineNumber(), 3  == errorInfo.lineNumber());
          LOOP_ASSERT(errorInfo.columnNumber(),
                      16 == errorInfo.columnNumber());
          LOOP_ASSERT(errorInfo.message(),
                      npos != errorInfo.message().find("No '='"));

          ASSERT(reader.isOpen());
          reader.close();
          ASSERT(!reader.isOpen());
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
          static const char xmlRaw[] =
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

          bsl::string xmlStr(xmlRaw);
          nonAsciiUtf8(&xmlStr);

          if (veryVerbose) P(xmlStr);

          balxml::NamespaceRegistry namespaces;
          balxml::PrefixStack prefixStack(&namespaces);
          Mini miniReader; Obj reader(&miniReader);

          reader.setPrefixStack(&prefixStack);

          int rc = reader.open(xmlStr.c_str(), xmlStr.length());
          ASSERT(-1 < rc);
          ASSERT(reader.isOpen());

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
          ASSERT( reader.nodeType() == balxml::Reader::e_NODE_TYPE_COMMENT);
          ASSERT(!bsl::strcmp(reader.nodeValue(),
                              " RCSId_bascfg_xsd = \"$Id: $\" "));

          rc = advancePastWhiteSpace(reader);
          ASSERT( reader.nodeType() == balxml::Reader::e_NODE_TYPE_ELEMENT);
          ASSERT(!bsl::strcmp(reader.nodeName(), "xs:schema"));
          ASSERT(0 > rc);
          const balxml::ErrorInfo& errorInfo = reader.errorInfo();
          ASSERT(6  == errorInfo.lineNumber());
          ASSERT(26 == errorInfo.columnNumber());
          LOOP_ASSERT(errorInfo.message(),
                      npos != errorInfo.message().find("No space"));

          ASSERT(reader.isOpen());
          reader.close();
          ASSERT(!reader.isOpen());
        }

        if (verbose) bsl::cout << "Attribute starts with \" and ends with '"
                               << bsl::endl
                               << "-   -   -   -   -   -   -   -   -   -   -\n"
                               << bsl::endl;
        {
          // Closing quote is missing on the xmlns:bdem attribute within the
          // xs:schema element.  Start with ' end with ".
          static const char xmlRaw[] =
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

          bsl::string xmlStr(xmlRaw);
          nonAsciiUtf8(&xmlStr);

          if (veryVerbose) P(xmlStr);

          balxml::NamespaceRegistry namespaces;
          balxml::PrefixStack prefixStack(&namespaces);
          Mini miniReader; Obj reader(&miniReader);

          reader.setPrefixStack(&prefixStack);

          int rc = reader.open(xmlStr.c_str(), xmlStr.length());
          ASSERT(-1 < rc);
          ASSERT(reader.isOpen());

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
          ASSERT( reader.nodeType() == balxml::Reader::e_NODE_TYPE_COMMENT);
          ASSERT(!bsl::strcmp(reader.nodeValue(),
                      " RCSId_bascfg_xsd = \"$Id: $\" "));

          rc = advancePastWhiteSpace(reader);
          ASSERT( reader.nodeType() == balxml::Reader::e_NODE_TYPE_ELEMENT);
          ASSERT(!bsl::strcmp(reader.nodeName(), "xs:schema"));
          ASSERT(0 > rc);
          const balxml::ErrorInfo& errorInfo = reader.errorInfo();
          ASSERT(6  == errorInfo.lineNumber());
          ASSERT(26 == errorInfo.columnNumber());
          LOOP_ASSERT(errorInfo.message(),
                      npos != errorInfo.message().find("No space"));

          ASSERT(reader.isOpen());
          reader.close();
          ASSERT(!reader.isOpen());
        }

        if (verbose)
          bsl::cout << "Illegal Attribute Name"   << bsl::endl
                    << "-  -  -  -  -  -  -  -\n" << bsl::endl;
        {
          // The bdem:pac=kage [sic] attribute with in the xs:schema element
          // is invalid.
          static const char xmlRaw[] =
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

          bsl::string xmlStr(xmlRaw);
          nonAsciiUtf8(&xmlStr);

          if (veryVerbose) P(xmlStr);

          balxml::NamespaceRegistry namespaces;
          balxml::PrefixStack prefixStack(&namespaces);
          Mini miniReader; Obj reader(&miniReader);

          reader.setPrefixStack(&prefixStack);

          int rc = reader.open(xmlStr.c_str(), xmlStr.length());
          ASSERT(-1 < rc);
          ASSERT(reader.isOpen());

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
          ASSERT( reader.nodeType() == balxml::Reader::e_NODE_TYPE_COMMENT);
          ASSERT(!bsl::strcmp(reader.nodeValue(),
                      " RCSId_bascfg_xsd = \"$Id: $\" "));

          rc = advancePastWhiteSpace(reader);
          ASSERT( reader.nodeType() == balxml::Reader::e_NODE_TYPE_ELEMENT);
          ASSERT(!bsl::strcmp(reader.nodeName(), "xs:schema"));
          ASSERT(0 > rc);
          const balxml::ErrorInfo& errorInfo = reader.errorInfo();
          ASSERT(6  == errorInfo.lineNumber());
          ASSERT(15 == errorInfo.columnNumber());
          ASSERT(bsl::strstr(errorInfo.message().c_str(), "Attribute value"));

          ASSERT(reader.isOpen());
          reader.close();
          ASSERT(!reader.isOpen());
        }

        if (verbose)
          bsl::cout << "Illegal Attribute Value"     << bsl::endl
                    << "-  -  -  -  -  -  -  -  -\n" << bsl::endl;
        {
          // The bdem:package attribute with in the xs:schema element has an
          // invalid value.
          static const char xmlRaw[] =
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

          bsl::string xmlStr(xmlRaw);
          nonAsciiUtf8(&xmlStr);

          if (veryVerbose) P(xmlStr);

          balxml::NamespaceRegistry namespaces;
          balxml::PrefixStack prefixStack(&namespaces);
          Mini miniReader; Obj reader(&miniReader);

          reader.setPrefixStack(&prefixStack);

          int rc = reader.open(xmlStr.c_str(), xmlStr.length());
          ASSERT(-1 < rc);
          ASSERT(reader.isOpen());

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
          ASSERT( reader.nodeType() == balxml::Reader::e_NODE_TYPE_COMMENT);
          ASSERT(!bsl::strcmp(reader.nodeValue(),
                      " RCSId_bascfg_xsd = \"$Id: $\" "));

          rc = advancePastWhiteSpace(reader);
          ASSERT( reader.nodeType() == balxml::Reader::e_NODE_TYPE_ELEMENT);
          ASSERT(!bsl::strcmp(reader.nodeName(), "xs:schema"));
          ASSERT(0 > rc);
          const balxml::ErrorInfo& errorInfo = reader.errorInfo();
          LOOP_ASSERT(errorInfo.lineNumber(), 6  == errorInfo.lineNumber());
          LOOP_ASSERT(errorInfo.columnNumber(),
                      27 == errorInfo.columnNumber());
          LOOP_ASSERT(errorInfo.message(),
                      npos != errorInfo.message().find("No space"));

          ASSERT(reader.isOpen());
          reader.close();
          ASSERT(!reader.isOpen());
        }

        if (verbose)
          bsl::cout << "Illegal Element Name"     << bsl::endl
                    << "-  -  -  -  -  -  -  -\n" << bsl::endl;
        {
          // The <xs:sc>hema [sic] element is invalid.
          static const char xmlRaw[] =
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

          bsl::string xmlStr(xmlRaw);
          nonAsciiUtf8(&xmlStr);

          if (veryVerbose) P(xmlStr);

          balxml::NamespaceRegistry namespaces;
          balxml::PrefixStack prefixStack(&namespaces);
          Mini miniReader; Obj reader(&miniReader);

          reader.setPrefixStack(&prefixStack);

          int rc = reader.open(xmlStr.c_str(), xmlStr.length());
          ASSERT(-1 < rc);
          ASSERT(reader.isOpen());

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
          ASSERT( reader.nodeType() == balxml::Reader::e_NODE_TYPE_COMMENT);
          ASSERT(!bsl::strcmp(reader.nodeValue(),
                      " RCSId_bascfg_xsd = \"$Id: $\" "));

          rc = advancePastWhiteSpace(reader);
          ASSERT( reader.nodeType() == balxml::Reader::e_NODE_TYPE_ELEMENT);
          ASSERT(0 > rc);
          const balxml::ErrorInfo& errorInfo = reader.errorInfo();
          ASSERT(3 == errorInfo.lineNumber());
          ASSERT(8 == errorInfo.columnNumber());
          ASSERT(bsl::strstr(errorInfo.message().c_str(),
                             "Undefined namespace"));

          ASSERT(reader.isOpen());
          reader.close();
          ASSERT(!reader.isOpen());
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
        Mini miniReader; Obj reader(&miniReader);

        reader.setPrefixStack(&prefixStack);

        int rc = reader.open(xmlStr.c_str(), xmlStr.length());
        ASSERT(-1 < rc);

        ASSERT( reader.isOpen());
        ASSERT( reader.nodeType() == balxml::Reader::e_NODE_TYPE_NONE);

        // Test read header and readNodes functions.
        readHeader(reader);
        readNodes(reader, 0, 1, 0, 0);

        rc = advancePastWhiteSpace(reader);
        ASSERT( reader.nodeType() == balxml::Reader::e_NODE_TYPE_END_ELEMENT);
        ASSERT(!bsl::strcmp(reader.nodeName(), "xs:schema"));

        reader.close();
      } break;
      case 2: {
        //--------------------------------------------------------------------
        // TESTING PRIMARY MANIPULATORS
        //
        // Concerns:
        //: 1 The 'balxml::MiniReader's' wrapped in a
        //:   'balxml::Utf8ReaderWrapper' constructors work properly:
        //:   o The initial value is correct.
        //:   o The constructor is exception neutral w.r.t. memory allocation.
        //:   o The internal memory management system is hooked up properly so
        //:     that *all* internally allocated memory draws from a
        //:     user-supplied allocator whenever one is specified.
        //: 2 The destructor works properly as implicitly tested in the various
        //:   scopes of this test and in the presence of exceptions.
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
                      << "TESTING PRIMARY MANIPULATORS" << bsl::endl
                      << "============================" << bsl::endl;

        if (verbose) bsl::cout << "\nTesting 'balxml::MiniReader(*bA)' ctor"
                               << bsl::endl;

        if (verbose) bsl::cout << "\tPassing in an allocator." << bsl::endl;
        typedef bsls::Types::Int64 Int64;
        if (verbose) bsl::cout << "\t\tWith no exceptions."    << bsl::endl;
        {
            const Int64 NUM_BLOCKS = testAllocator.numBlocksInUse();
            const Int64 NUM_BYTES  = testAllocator.numBytesInUse();
            {
                Mini miniReader(&testAllocator);
                Obj mX(&miniReader, &testAllocator);

                ASSERT(NUM_BLOCKS < testAllocator.numBlocksInUse());
                ASSERT(NUM_BYTES  < testAllocator.numBytesInUse());
            }
            ASSERT(NUM_BLOCKS == testAllocator.numBlocksInUse());
            ASSERT(NUM_BYTES  == testAllocator.numBytesInUse());
        }

        if (verbose) bsl::cout << "\t\tWith exceptions." << bsl::endl;
        {
            const Int64 NUM_BLOCKS = testAllocator.numBlocksInUse();
            const Int64 NUM_BYTES  = testAllocator.numBytesInUse();
            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
                Mini miniReader(&testAllocator);
                Obj mX(&miniReader, &testAllocator);

                ASSERT(NUM_BLOCKS < testAllocator.numBlocksInUse());
                ASSERT(NUM_BYTES  < testAllocator.numBytesInUse());
            } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
            ASSERT(NUM_BLOCKS == testAllocator.numBlocksInUse());
            ASSERT(NUM_BYTES  == testAllocator.numBytesInUse());
        }

        if (verbose) {
            bsl::cout << "\nTesting 'balxml::MiniReader(bufSize, *bA)' ctor"
                << bsl::endl;
        }

        if (verbose) bsl::cout << "\tPassing in an allocator." << bsl::endl;
        if (verbose) bsl::cout << "\t\tWith no exceptions." << bsl::endl;
        {
            const Int64 NUM_BLOCKS = testAllocator.numBlocksInUse();
            const Int64 NUM_BYTES  = testAllocator.numBytesInUse();
            {
                Mini miniReader(1024, &testAllocator);
                Obj mX(&miniReader, &testAllocator);

                ASSERT(NUM_BLOCKS < testAllocator.numBlocksInUse());
                ASSERT(NUM_BYTES  < testAllocator.numBytesInUse());
            }
            ASSERT(NUM_BLOCKS == testAllocator.numBlocksInUse());
            ASSERT(NUM_BYTES  == testAllocator.numBytesInUse());
            {
                Mini miniReader(131072, &testAllocator);
                Obj mX(&miniReader, &testAllocator);

                ASSERT(NUM_BLOCKS < testAllocator.numBlocksInUse());
                ASSERT(NUM_BYTES  < testAllocator.numBytesInUse());
            }
            ASSERT(NUM_BLOCKS == testAllocator.numBlocksInUse());
            ASSERT(NUM_BYTES  == testAllocator.numBytesInUse());
        }

        if (verbose) bsl::cout << "\t\tWith exceptions." << bsl::endl;
        {
            const Int64 NUM_BLOCKS = testAllocator.numBlocksInUse();
            const Int64 NUM_BYTES  = testAllocator.numBytesInUse();
            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
                Mini miniReader(1024, &testAllocator);
                Obj mX(&miniReader, &testAllocator);

                ASSERT(NUM_BLOCKS < testAllocator.numBlocksInUse());
                ASSERT(NUM_BYTES  < testAllocator.numBytesInUse());
            } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

            ASSERT(NUM_BLOCKS == testAllocator.numBlocksInUse());
            ASSERT(NUM_BYTES  == testAllocator.numBytesInUse());
            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
                Mini miniReader(131072, &testAllocator);
                Obj mX(&miniReader, &testAllocator);

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
        //: 1 Exercise a broad cross-section of functionality on a
        //:   'balxml::MiniReader' wrapped in a 'balxml::Utf8ReaderWrapper'
        //:   before beginning testing in earnest.  Probe that functionality
        //:   systematically and incrementally to discover basic errors in
        //:   isolation.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nBREATHING TEST"
                               << "\n==============" << bsl::endl;

        static const char xmlRaw[] =
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

        bsl::string xmlStr(xmlRaw);
        nonAsciiUtf8(&xmlStr);

        if (veryVerbose) P(xmlStr);

        balxml::NamespaceRegistry namespaces;
        balxml::PrefixStack prefixStack(&namespaces);
        Mini miniReader; Obj reader(&miniReader);

        reader.setPrefixStack(&prefixStack);

        int rc = reader.open(xmlStr.c_str(), xmlStr.length());
        ASSERT(-1 < rc);

        ASSERT( reader.isOpen());
        ASSERT( reader.nodeType() == balxml::Reader::e_NODE_TYPE_NONE);

        rc = advancePastWhiteSpace(reader);
        //Note: Mini Reader needs to be fix to pass back the correct encoding.
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
        ASSERT(reader.nodeType() == balxml::Reader::e_NODE_TYPE_TEXT);
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
        ASSERT(reader.nodeType() == balxml::Reader::e_NODE_TYPE_END_ELEMENT);
        ASSERT(!bsl::strcmp(reader.nodeName(), "Element3"));

        rc = advancePastWhiteSpace(reader);
        ASSERT( reader.nodeType() == balxml::Reader::e_NODE_TYPE_CDATA);
        ASSERT(!bsl::strcmp(reader.nodeValue(), "&lt;123&#240;&gt;"));

        rc = advancePastWhiteSpace(reader);
        ASSERT( reader.nodeType() == balxml::Reader::e_NODE_TYPE_END_ELEMENT);
        ASSERT(!bsl::strcmp(reader.nodeName(), "Node0"));

        rc = advancePastWhiteSpace(reader);
        ASSERT( reader.nodeType() == balxml::Reader::e_NODE_TYPE_END_ELEMENT);
        ASSERT(!bsl::strcmp(reader.nodeName(), "xs:schema"));

        reader.close();
      } break;
      default: {
        bsl::cerr << "WARNING: CASE `" << test << "' NOT FOUND." << bsl::endl;
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
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
