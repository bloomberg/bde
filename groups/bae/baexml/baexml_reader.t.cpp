// baexml_reader.t.cpp                         -*-C++-*-

#include <baexml_reader.h>
#include <baexml_errorinfo.h>
#include <baexml_namespaceregistry.h>
#include <baexml_prefixstack.h>
#include <baexml_elementattribute.h>

#include <bsl_cstring.h>     // strlen()
#include <bsl_cstdlib.h>     // atoi()
#include <bsl_iostream.h>
#include <bsl_sstream.h>

using namespace BloombergLP;

//=============================================================================
//                                 TEST PLAN
//-----------------------------------------------------------------------------
//                                 Overview
//                                 --------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

//=============================================================================
//                      STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
static int testStatus = 0;

static void aSsErT(int c, const char *s, int i)
{
    if (c) {
        bsl::cout << "Error " << __FILE__ << "(" << i << "): " << s
                  << "    (failed)" << bsl::endl;
        if (0 <= testStatus && testStatus <= 100) ++testStatus;
    }
}

#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

//=============================================================================
//                  STANDARD BDE LOOP-ASSERT TEST MACROS
//-----------------------------------------------------------------------------
#define LOOP_ASSERT(I,X) { \
    if (!(X)) { bsl::cout << #I << ": " << I << "\n"; \
                aSsErT(1, #X, __LINE__); }}

#define LOOP2_ASSERT(I,J,X) { \
    if (!(X)) { bsl::cout << #I << ": " << I << "\t"  \
                          << #J << ": " << J << "\n"; \
                aSsErT(1, #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) { \
   if (!(X)) { bsl::cout << #I << ": " << I << "\t" \
                         << #J << ": " << J << "\t" \
                         << #K << ": " << K << "\n";\
               aSsErT(1, #X, __LINE__); } }

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define P(X) bsl::cout << #X " = " << (X) << bsl::endl;
                                              // Print identifier and value.
#define Q(X) bsl::cout << "<| " #X " |>" << bsl::endl;
                                              // Quote identifier literally.
#define P_(X) bsl::cout << #X " = " << (X) << ", " << bsl::flush;
                                              // P(X) without '\n'
#define L_ __LINE__                           // current Line number
#define T_ bsl::cout << "\t" << bsl::flush;   // Print a tab (w/o newline)
#define NL "\n"

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------
#define CHK(X) (X != 0 ? (const char *) X : "")
#define NUM_ATTRIBUTES 5

typedef baexml_Reader Obj;

// Assume we have a very simple user directory.  The XML below describes a
// typical entry, it has a top level element, with one embedded entry
// describing a user.
//
//  <?xml version='1.0' encoding='UTF-8'?>
//  <directory-entry xmlns:dir='http://bloomberg.com/schemas/directory'>
//      <name>John Smith</name>
//      <phone dir:phonetype='cell'>212-318-2000</phone>
//      <address/>
//  </directory-entry>
//
// To make our example easier, i.e., in order to keep us from having to parse
// the above XML, lets introduce an array of 'helper' structs.  This array will
// be filling in with data capable of describing the information contained in
// the user directory XML above.

struct TestNode {
    // TestNode is a struct that contains information capable of describing an
    // XML node.
    Obj::NodeType  d_type;        // Describes the type of the XML node
    const char    *d_qname;       // Name qualified name the XML node
    const char    *d_nodeValue;   // Value of the the XML node
                                  // if null, then hasValue() returns false
    int            d_depthChange; // Used to adjust the 'TestReader' depth
                                  // level, valid values are -1, 0 or 1
    bool           d_isEmptyElement;
    struct {
        const char *d_qname; // Qualified name of the attribute
        const char *d_value; // Value of the attribute
    } d_attributes[NUM_ATTRIBUTES]; // Array of attributes
};

static const char *XmlValue = "version='1.0' encoding='UTF-8'";

static const TestNode fakeDocument[] = {
    // 'fakeDocument' is an array of 'TestNode's, this array will be use by the
    // 'TestReader' to traverse and describe the user directory XML above.
    { Obj::BAEXML_NODE_TYPE_NONE },

    { Obj::BAEXML_NODE_TYPE_XML_DECLARATION,
      "xml"          , XmlValue      , +1   },

    { Obj::BAEXML_NODE_TYPE_ELEMENT,
      "directory-entry" , 0             ,  0,
      false, { "xmlns:dir"    , "http://bloomberg.com/schemas/directory" } },

    { Obj::BAEXML_NODE_TYPE_ELEMENT,
      "name"           , 0             , +1,  },

    { Obj::BAEXML_NODE_TYPE_TEXT,
      0                , "John Smith"  , +1   },

    { Obj::BAEXML_NODE_TYPE_END_ELEMENT,
      "name"           , 0             , -1   },

    { Obj::BAEXML_NODE_TYPE_ELEMENT,
      "phone"          , 0             ,  0,
      false, { "dir:phonetype", "cell"                                   } },

    { Obj::BAEXML_NODE_TYPE_TEXT,
      0                , "212-318-2000", +1   },

    { Obj::BAEXML_NODE_TYPE_END_ELEMENT,
      "phone"          , 0             , -1   },

    { Obj::BAEXML_NODE_TYPE_ELEMENT,
       "address"        , 0             ,  0,
       true                                                                 },

    { Obj::BAEXML_NODE_TYPE_END_ELEMENT,
      "directory-entry", 0             , -1   },

    { Obj::BAEXML_NODE_TYPE_NONE }
};

// Start of usage example, extract to the 'baexml_Reader' header file.

// Create a class that implements the 'baexml_Reader' interface.
//..
class TestReader : public baexml_Reader
{

private:
    baexml_ErrorInfo    d_errorInfo; // Contains the current error information
    baexml_PrefixStack *d_prefixes;  // The prefix stack used by the TestReader
    XmlResolverFunctor  d_resolver;  // Just a place holder, not actually used
                                     // while looking up XML information
    bool                d_isOpen;    // Keeps track if the reader is open
    bsl::string         d_encoding;  // The documents encoding
    int                 d_nodeDepth; // The nesting level of the current node
    const TestNode     *d_currentNode; // The current xml node being

    // PRIVATE CLASS METHODS
    void setEncoding(const char *encoding);
    void adjustPrefixStack();

public:

    // PUBLIC CREATORS
    TestReader(void);
    virtual ~TestReader(void);

    // MANIPULATORS - SETUP METHODS
    virtual void setResolver(XmlResolverFunctor resolver);

    virtual void setPrefixStack(baexml_PrefixStack *prefixes);

    // MANIPULATORS - OPEN/CLOSE AND NAVIGATION METHODS
    virtual int open(const char *filename,
                     const char *encoding = 0);
    virtual int open(const char *buffer,
                     size_t      size,
                     const char *url = 0,
                     const char *encoding = 0);
    virtual int open(bsl::streambuf *stream,
                     const char     *url = 0,
                     const char     *encoding = 0);

    virtual void close();

    virtual int advanceToNextNode();

    virtual int lookupAttribute(baexml_ElementAttribute *attribute,
                                int                      index) const;
    virtual int lookupAttribute(baexml_ElementAttribute  *attribute,
                                const char               *qname) const;
    virtual int lookupAttribute(baexml_ElementAttribute  *attribute,
                                const char               *localName,
                                const char               *namespaceUri) const;
    virtual int lookupAttribute(baexml_ElementAttribute  *attribute,
                                const char               *localName,
                                int                       namespaceId) const;

    virtual void setOptions(unsigned int flags);

    // ACCESSORS
    virtual const char *documentEncoding() const;
    virtual XmlResolverFunctor resolver() const;
    virtual bool isOpen() const;
    virtual const baexml_ErrorInfo& errorInfo() const;
    virtual int getLineNumber() const;
    virtual int getColumnNumber() const;
    virtual baexml_PrefixStack *prefixStack() const;
    virtual NodeType nodeType() const;
    virtual const char *nodeName() const;
    virtual const char *nodeLocalName() const;
    virtual const char *nodePrefix() const;
    virtual int nodeNamespaceId() const;
    virtual const char *nodeNamespaceUri() const;
    virtual const char *nodeBaseUri() const;
    virtual bool nodeHasValue() const;
    virtual const char *nodeValue() const;
    virtual int nodeDepth() const;
    virtual int numAttributes() const;
    virtual bool isEmptyElement() const;
    virtual unsigned int options() const;
};
//..
// Utility function to skip past white space.
//..
int advancePastWhiteSpace(baexml_Reader& reader) {
    const char *whiteSpace = "\n\r\t ";
    const char *value = '\0';
    int         type = 0;
    int         rc = 0;

    do {
        rc    = reader.advanceToNextNode();
        value = reader.nodeValue();
        type  = reader.nodeType();
    } while(0 == rc &&
            type == baexml_Reader::BAEXML_NODE_TYPE_WHITESPACE ||
            (type == baexml_Reader::BAEXML_NODE_TYPE_TEXT &&
             bsl::strlen(value) == bsl::strspn(value, whiteSpace)));

    ASSERT( reader.nodeType() != baexml_Reader::BAEXML_NODE_TYPE_WHITESPACE);

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
    const char TEST_XML_STRING[] =
       "<?xml version='1.0' encoding='UTF-8'?>\n"
       "<directory-entry xmlns:dir='http://bloomberg.com/schemas/directory'>\n"
       "    <name>John Smith</name>\n"
       "    <phone dir:phonetype='cell'>212-318-2000</phone>\n"
       "    <address/>\n"
       "</directory-entry>\n";
//..
// In order to read the XML, we first need to construct a
// 'baexml_NamespaceRegistry' object, a 'baexml_PrefixStack' object, and a
// 'TestReader' object, where 'TestReader' is a derived implementation of
// 'baexml_reader'.
//..
    baexml_NamespaceRegistry namespaces;
    baexml_PrefixStack prefixStack(&namespaces);
    TestReader testReader; baexml_Reader& reader = testReader;

    ASSERT(!reader.isOpen());
//..
// The reader uses a 'baexml_PrefixStack' to manage namespace prefixes so we
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
// Confirm that the 'bdem_Reader' has opened properly
//..
    ASSERT( reader.isOpen());
    ASSERT(!bsl::strncmp(reader.documentEncoding(), "UTF-8", 5));
    ASSERT( reader.nodeType() == baexml_Reader::BAEXML_NODE_TYPE_NONE);
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
    ASSERT( rc == 0);
    ASSERT( reader.nodeType() ==
                              baexml_Reader::BAEXML_NODE_TYPE_XML_DECLARATION);
    ASSERT(!bsl::strcmp(reader.nodeName(), "xml"));
    ASSERT( reader.nodeHasValue());
    ASSERT(!bsl::strcmp(reader.nodeValue(), "version='1.0' encoding='UTF-8'"));
    ASSERT( reader.nodeDepth() == 1);
    ASSERT( reader.numAttributes() == 0);
    ASSERT(!reader.isEmptyElement());
//..
// Advance to the top level element, which has one attribute, the xml
// namespace.  Assert the namespace information has been added correctly to the
// prefix stack.
//..
    rc = advancePastWhiteSpace(reader);
    ASSERT( rc == 0);
    ASSERT( reader.nodeType() == baexml_Reader::BAEXML_NODE_TYPE_ELEMENT);
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
    ASSERT( rc == 0);
    ASSERT( reader.nodeType() == baexml_Reader::BAEXML_NODE_TYPE_ELEMENT);
    ASSERT(!bsl::strcmp(reader.nodeName(), "name"));
    ASSERT(!reader.nodeHasValue());
    ASSERT( reader.nodeDepth() == 2);
    ASSERT( reader.numAttributes() == 0);
    ASSERT(!reader.isEmptyElement());

    rc = advancePastWhiteSpace(reader);
    ASSERT( rc == 0);
    ASSERT( reader.nodeType() == baexml_Reader::BAEXML_NODE_TYPE_TEXT);
    ASSERT( reader.nodeHasValue());
    ASSERT(!bsl::strcmp(reader.nodeValue(), "John Smith"));
    ASSERT( reader.nodeDepth() == 3);
    ASSERT( reader.numAttributes() == 0);
    ASSERT(!reader.isEmptyElement());

    rc = advancePastWhiteSpace(reader);
    ASSERT( rc == 0);
    ASSERT( reader.nodeType() == baexml_Reader::BAEXML_NODE_TYPE_END_ELEMENT);
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
    ASSERT( rc == 0);
    ASSERT( reader.nodeType() == baexml_Reader::BAEXML_NODE_TYPE_ELEMENT);
    ASSERT(!bsl::strcmp(reader.nodeName(), "phone"));
    ASSERT(!reader.nodeHasValue());
    ASSERT( reader.nodeDepth() == 2);
    ASSERT( reader.numAttributes() == 1);
    ASSERT(!reader.isEmptyElement());
//..
// The phone node has one attribute, look it up and assert the
// 'baexml_ElementAttribute' contains valid information and that the prefix
// returns the correct namespace URI from the prefix stack.
//..
    baexml_ElementAttribute elemAttr;

    rc = reader.lookupAttribute(&elemAttr, 0);
    ASSERT( rc == 0);
    ASSERT(!elemAttr.isNull());
    ASSERT(!bsl::strcmp(elemAttr.qualifiedName(), "dir:phonetype"));
    ASSERT(!bsl::strcmp(elemAttr.value(), "cell"));
    ASSERT(!bsl::strcmp(elemAttr.prefix(), "dir"));
    ASSERT(!bsl::strcmp(elemAttr.localName(), "phonetype"));
    ASSERT(!bsl::strcmp(elemAttr.namespaceUri(),
                        "http://bloomberg.com/schemas/directory"));
    ASSERT( elemAttr.namespaceId() == 0);

    ASSERT(prefixStack.lookupNamespaceUri(elemAttr.prefix()) ==
           elemAttr.namespaceUri());

    rc = advancePastWhiteSpace(reader);
    ASSERT( rc == 0);
    ASSERT( reader.nodeType() == baexml_Reader::BAEXML_NODE_TYPE_TEXT);
    ASSERT( reader.nodeHasValue());
    ASSERT(!bsl::strcmp(reader.nodeValue(), "212-318-2000"));
    ASSERT( reader.nodeDepth() == 3);
    ASSERT( reader.numAttributes() == 0);
    ASSERT(!reader.isEmptyElement());

    rc = advancePastWhiteSpace(reader);
    ASSERT( rc == 0);
    ASSERT( reader.nodeType() == baexml_Reader::BAEXML_NODE_TYPE_END_ELEMENT);
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
    ASSERT( rc == 0);
    ASSERT( reader.nodeType() == baexml_Reader::BAEXML_NODE_TYPE_ELEMENT);
    ASSERT(!bsl::strcmp(reader.nodeName(), "address"));
    ASSERT(!reader.nodeHasValue());
    ASSERT( reader.nodeDepth() == 2);
    ASSERT( reader.numAttributes() == 0);
    ASSERT( reader.isEmptyElement());
//..
// Advance to the end element.
//..
    rc = advancePastWhiteSpace(reader);
    ASSERT( rc == 0);
    ASSERT( reader.nodeType() == baexml_Reader::BAEXML_NODE_TYPE_END_ELEMENT);
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
// End of usage example, extract to the 'baexml_Reader' header file.

                        // ----------------
                        // class TestReader
                        // ----------------

// Do to the size of the TestReader's implementation it does not get promoted
// to the usage example in the 'baexml_Reader' header.

// PRIVATE CLASS METHODS
inline void TestReader::setEncoding(const char *encoding) {
    d_encoding = (0 == encoding || '\0' != encoding[0])
               ? "UTF-8"
               : encoding;
}

inline void TestReader::adjustPrefixStack() {
    // Each time a node is read that is a BAEXML_NODE_TYPE_ELEMENT, we must
    // push an namespace prefixed on the prefix stack.
    if (Obj::BAEXML_NODE_TYPE_ELEMENT == d_currentNode->d_type) {

        for (int ii = 0; ii < NUM_ATTRIBUTES; ++ii) {
            const char *prefix = d_currentNode->d_attributes[ii].d_qname;

            if (!prefix || bsl::strncmp("xmlns", prefix, 5)) {
                continue;
            }

            if (':' == prefix[5]) {
                d_prefixes->pushPrefix(
                    prefix + 6, d_currentNode->d_attributes[ii].d_value);
            }
            else {
                // default namespace
                d_prefixes->pushPrefix(
                    "", d_currentNode->d_attributes[ii].d_value);
            }
        }
    }
    else if (Obj::BAEXML_NODE_TYPE_NONE == d_currentNode->d_type) {
        d_prefixes->reset();
    }
}

// PUBLIC CREATORS
TestReader::TestReader()
    : d_errorInfo ()
    , d_prefixes(0)
    , d_resolver()
    , d_isOpen(false)
    , d_encoding()
    , d_nodeDepth(0)
    , d_currentNode(0) { }

TestReader::~TestReader(void) {}

// MANIPULATORS - SETUP METHODS
void TestReader::setResolver(XmlResolverFunctor resolver) {
    d_resolver = resolver;
}

void TestReader::setPrefixStack(baexml_PrefixStack *prefixes) {
    if (d_isOpen)
    {
        return;
    }

    d_prefixes = prefixes;
}

// MANIPULATORS - OPEN/CLOSE AND NAVIGATION METHODS
int TestReader::open(const char *filename,
                     const char *encoding) {
    if (d_isOpen) {
        return false;
    }
    d_isOpen = true;
    d_nodeDepth = 0;

    d_currentNode = fakeDocument;

    setEncoding(encoding);
    return 0;
}

int TestReader::open(const char *buffer,
                     size_t      size,
                     const char *url,
                     const char *encoding) {
    if (d_isOpen) {
        return false;
    }
    d_isOpen = true;
    d_nodeDepth = 0;

    d_currentNode = fakeDocument;

    setEncoding(encoding);
    return 0;
}

int TestReader::open(bsl::streambuf *stream,
                     const char     *url,
                     const char     *encoding) {
    if (d_isOpen) {
        return false;
    }
    d_isOpen = true;
    d_nodeDepth = 0;

    d_currentNode = fakeDocument;

    setEncoding(encoding);
    return 0;
}

void TestReader::close() {
    if (d_prefixes) {
        d_prefixes->reset();
    }

    d_isOpen = false;
    d_encoding.clear();
    d_nodeDepth = 0;
    d_currentNode = 0;
}

int TestReader::advanceToNextNode() {
    if (!d_currentNode) {
        return -1;
    }

    d_currentNode++;

    if (Obj::BAEXML_NODE_TYPE_NONE == d_currentNode->d_type) {
        // If the node type is BAEXML_NODE_TYPE_NONE after we have just
        // incremented to the next node, that mean we are at the end of the
        // document.  An easy way to deal with not incrementing d_currentNode
        // to a forbidden value is to simply decrement it.
        d_currentNode--;
        d_prefixes->reset();
        return 1;
    }

    if (d_prefixes && 1 == d_nodeDepth) {
        // The 'TestReader' only recognizes namespace URIs with the prefix
        // (xmlns:) on the top level element, these URIs will be added to
        // the prefix stack.  Namespace URI declarations on any other
        // elements will be treated like normal attributes.  The prefix
        // stack will be reset once the top level element is closed.
        adjustPrefixStack();
    }

    d_nodeDepth += d_currentNode->d_depthChange;

    return 0;
}

int TestReader::lookupAttribute(baexml_ElementAttribute *attribute,
                                int                      index) const {
    if (!d_currentNode || index < 0 || index >= NUM_ATTRIBUTES) {
        return 1;
    }

    const char *qname = d_currentNode->d_attributes[index].d_qname;
    if ('\0' == qname[0]) {
        return 1;
    }

    attribute->reset(d_prefixes, qname,
                     d_currentNode->d_attributes[index].d_value);
    return 0;
}

int TestReader::lookupAttribute(baexml_ElementAttribute  *attribute,
                                const char               *qname) const {
    if (!d_currentNode) {
        return 1;
    }

    for (int index = 0; index < NUM_ATTRIBUTES; ++index) {
        const char *currentQName = d_currentNode->d_attributes[index].d_qname;

        if (!bsl::strcmp(currentQName, qname)) {
            attribute->reset(d_prefixes, qname,
                             d_currentNode->d_attributes[index].d_value);
            return 0;
        }
    }

    return 1;
}

int TestReader::lookupAttribute(baexml_ElementAttribute  *attribute,
                                const char               *localName,
                                const char               *namespaceUri) const {
    if (!d_currentNode) {
        return 1;
    }

    baexml_ElementAttribute a;

    for (int index = 0; index < NUM_ATTRIBUTES; ++index) {
        const char *qname = d_currentNode->d_attributes[index].d_qname;

        if ('\0' == qname[0]) {
            break;
        }

        a.reset(d_prefixes, qname, d_currentNode->d_attributes[index].d_value);

        if (!bsl::strcmp(a.localName(), localName) &&
            !bsl::strcmp(a.namespaceUri(), namespaceUri)) {
            *attribute = a;
            return 0;
        }
    }

    return 1;
}

int TestReader::lookupAttribute(baexml_ElementAttribute  *attribute,
                                const char               *localName,
                                int                       namespaceId) const {
    if (!d_currentNode) {
        return 1;
    }

    baexml_ElementAttribute a;

    for (int index = 0; index < NUM_ATTRIBUTES; ++index) {
        const char *qname = d_currentNode->d_attributes[index].d_qname;

        if (!bsl::strcmp("", qname)) {
            break;
        }

        a.reset(d_prefixes, qname, d_currentNode->d_attributes[index].d_value);

        if (!bsl::strcmp(a.localName(), localName) &&
            a.namespaceId(), namespaceId) {
            *attribute = a;
            return 0;
        }
    }

    return 1;
}

void TestReader::setOptions(unsigned int flags) { }

// ACCESSORS
const char *TestReader::documentEncoding() const {
    return d_encoding.c_str();
}

TestReader::XmlResolverFunctor TestReader::resolver() const {
    return d_resolver;
}

bool TestReader::isOpen() const {
    return d_isOpen;
}

const baexml_ErrorInfo& TestReader::errorInfo() const {
    return d_errorInfo;
}

int TestReader::getLineNumber() const {
    return 0;
}

int TestReader::getColumnNumber() const {
    return 0;
}

baexml_PrefixStack *TestReader::prefixStack() const {
    return d_prefixes;
}

TestReader::NodeType TestReader::nodeType() const {
    if (!d_currentNode || !d_isOpen) {
        return BAEXML_NODE_TYPE_NONE;
    }

    return d_currentNode->d_type;
}

const char *TestReader::nodeName() const {
    if (!d_currentNode || !d_isOpen) {
        return 0;
    }

    return d_currentNode->d_qname;
}

const char *TestReader::nodeLocalName() const {
    if (!d_currentNode || !d_isOpen) {
        return 0;
    }

    // Our simple 'TestReader' does not understand XML with qualified node
    // names, as such local name always equals qualified name.  Simply return
    // d_qname.
    return d_currentNode->d_qname;
}

const char *TestReader::nodePrefix() const {
    // Our simple 'TestReader' does not understand XML with qualified node
    // names, as such their are no prefixes.  Simply return "".
    return "";
}

int TestReader::nodeNamespaceId() const {
    // Our simple 'TestReader' does not understand XML with namespaces on the
    // node level, as such their are no namespace ids.  Simply return -1.
    return -1;
}

const char *TestReader::nodeNamespaceUri() const {
    // Our simple 'TestReader' does not understand XML with namespaces on the
    // node level, as such their are no namespace URIs.  Simply return "".
    return "";
}

const char *TestReader::nodeBaseUri() const {
    // Our simple 'TestReader' does not understand XML with base URIs.  Simply
    // return "".
    return "";
}

bool TestReader::nodeHasValue() const {
    if (!d_currentNode || !d_isOpen) {
        return false;
    }

    if (0 == d_currentNode->d_nodeValue) {
        return 0;
    }

    return ('\0' != d_currentNode->d_nodeValue[0]);
}

const char *TestReader::nodeValue() const {
    if (!d_currentNode || !d_isOpen) {
        return 0;
    }

    return d_currentNode->d_nodeValue;
}

int TestReader::nodeDepth() const {
    return d_nodeDepth;
}

int TestReader::numAttributes() const {
    for (int index = 0; index < NUM_ATTRIBUTES; ++index) {
        if (0 == d_currentNode->d_attributes[index].d_qname) {
            return index;
        }
    }

    return NUM_ATTRIBUTES;
}

bool TestReader::isEmptyElement() const {
    return d_currentNode->d_isEmptyElement;
}

unsigned int TestReader::options() const {
    return 0;
}

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? bsl::atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;

    bsl::cout << "TEST " << __FILE__ << " CASE " << test << bsl::endl;;

    switch (test) { case 0:  // Zero is always the leading case.
      case 2: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //
        // This is a simple implementation of the 'baexml_Reader' interface.
        //
        // Concerns:
        //   That the usage example compiles and runs correctly.
        //   That a class can be derived from 'baexml_Reader' and that it can
        //       override all the methods.
        //
        // Plan:
        //   Copy the usage example from the component-level documentation.
        //   Replace 'assert' with 'ASSERT'.
        //
        // Testing:
        //   baexml_Reader();  // Constructor
        //   ~baexml_Reader(); // Destructor
        //   setPrefixStack(baexml_PrefixStack *prefixes);
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
        //
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nUSAGE EXAMPLE"
                               << "\n=============" << bsl::endl;

        usageExample();

      } break;
      case 1: {
            // ------------------------------------------------------------
            // Basic Test: Derived class can be instantiated
            // -------------------------------------------------------------

            if (verbose) bsl::cout << bsl::endl
                << "Basic Test" << bsl::endl
                << "=====================" << bsl::endl;

            TestReader reader;
            reader.open ("somefilename", "UTF-8");
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
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2006
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ------------------------------ END-OF-FILE ---------------------------------
