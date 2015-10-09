// balxml_reader.t.cpp                                                -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <balxml_reader.h>

#include <bslim_testutil.h>

#include <balxml_errorinfo.h>
#include <balxml_namespaceregistry.h>
#include <balxml_prefixstack.h>
#include <balxml_elementattribute.h>

#include <bsl_cstring.h>     // strlen()
#include <bsl_cstdlib.h>     // atoi()
#include <bsl_iostream.h>
#include <bsl_sstream.h>

using namespace BloombergLP;
using namespace bsl;

// ============================================================================
//                                 TEST PLAN
// ----------------------------------------------------------------------------
//                                 Overview
//                                 --------
// ----------------------------------------------------------------------------
// FREE OPERATORS
// [ 2] bsl::ostream& operator(bsl::ostream&, balxml::Reader::NodeType);
// ----------------------------------------------------------------------------
// [ 3] USAGE EXMAPLE
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

// ============================================================================
//                   GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------
#define CHK(X) (X != 0 ? (const char *) X : "")
#define NUM_ATTRIBUTES 5

typedef balxml::Reader Obj;

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
    Obj::NodeType  d_type;           // Describes the type of the XML node
    const char    *d_qname;          // Name qualified name the XML node
    const char    *d_nodeValue;      // Value of the the XML node if null, then
                                     // hasValue() returns false
    int            d_depthChange;    // Used to adjust the 'TestReader' depth
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
    { Obj::e_NODE_TYPE_NONE },

    { Obj::e_NODE_TYPE_XML_DECLARATION,
      "xml"          , XmlValue      , +1   },

    { Obj::e_NODE_TYPE_ELEMENT,
      "directory-entry" , 0             ,  0,
      false, { "xmlns:dir"    , "http://bloomberg.com/schemas/directory" } },

    { Obj::e_NODE_TYPE_ELEMENT,
      "name"           , 0             , +1,  },

    { Obj::e_NODE_TYPE_TEXT,
      0                , "John Smith"  , +1   },

    { Obj::e_NODE_TYPE_END_ELEMENT,
      "name"           , 0             , -1   },

    { Obj::e_NODE_TYPE_ELEMENT,
      "phone"          , 0             ,  0,
      false, { "dir:phonetype", "cell"                                   } },

    { Obj::e_NODE_TYPE_TEXT,
      0                , "212-318-2000", +1   },

    { Obj::e_NODE_TYPE_END_ELEMENT,
      "phone"          , 0             , -1   },

    { Obj::e_NODE_TYPE_ELEMENT,
       "address"        , 0             ,  0,
       true                                                                 },

    { Obj::e_NODE_TYPE_END_ELEMENT,
      "directory-entry", 0             , -1   },

    { Obj::e_NODE_TYPE_NONE }
};

// Start of usage example, extract to the 'balxml::Reader' header file.

// Create a class that implements the 'balxml::Reader' interface.
//..
class TestReader : public balxml::Reader
{

private:
    balxml::ErrorInfo    d_errorInfo; // Contains the current error information
    balxml::PrefixStack *d_prefixes; // The prefix stack used by the TestReader
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

    virtual void setPrefixStack(balxml::PrefixStack *prefixes);

    // MANIPULATORS - OPEN/CLOSE AND NAVIGATION METHODS
    virtual int open(const char *filename, const char *encoding = 0);
    virtual int open(const char *buffer,
                     size_t      size,
                     const char *url = 0,
                     const char *encoding = 0);
    virtual int open(bsl::streambuf *stream,
                     const char     *url = 0,
                     const char     *encoding = 0);

    virtual void close();

    virtual int advanceToNextNode();

    virtual int lookupAttribute(balxml::ElementAttribute *attribute,
                                int                       index) const;
    virtual int lookupAttribute(balxml::ElementAttribute *attribute,
                                const char               *qname) const;
    virtual int lookupAttribute(balxml::ElementAttribute *attribute,
                                const char               *localName,
                                const char               *namespaceUri) const;
    virtual int lookupAttribute(balxml::ElementAttribute *attribute,
                                const char               *localName,
                                int                       namespaceId) const;

    virtual void setOptions(unsigned int flags);

    // ACCESSORS
    virtual const char *documentEncoding() const;
    virtual XmlResolverFunctor resolver() const;
    virtual bool isOpen() const;
    virtual const balxml::ErrorInfo& errorInfo() const;
    virtual int getLineNumber() const;
    virtual int getColumnNumber() const;
    virtual balxml::PrefixStack *prefixStack() const;
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
int advancePastWhiteSpace(balxml::Reader& reader) {
    const char *whiteSpace = "\n\r\t ";
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
    const char TEST_XML_STRING[] =
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
    TestReader testReader; balxml::Reader& reader = testReader;

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
    ASSERT( rc == 0);
    ASSERT( reader.nodeType() ==
                              balxml::Reader::e_NODE_TYPE_XML_DECLARATION);
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
    ASSERT( rc == 0);
    ASSERT( reader.nodeType() == balxml::Reader::e_NODE_TYPE_ELEMENT);
    ASSERT(!bsl::strcmp(reader.nodeName(), "name"));
    ASSERT(!reader.nodeHasValue());
    ASSERT( reader.nodeDepth() == 2);
    ASSERT( reader.numAttributes() == 0);
    ASSERT(!reader.isEmptyElement());

    rc = advancePastWhiteSpace(reader);
    ASSERT( rc == 0);
    ASSERT( reader.nodeType() == balxml::Reader::e_NODE_TYPE_TEXT);
    ASSERT( reader.nodeHasValue());
    ASSERT(!bsl::strcmp(reader.nodeValue(), "John Smith"));
    ASSERT( reader.nodeDepth() == 3);
    ASSERT( reader.numAttributes() == 0);
    ASSERT(!reader.isEmptyElement());

    rc = advancePastWhiteSpace(reader);
    ASSERT( rc == 0);
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
    ASSERT( rc == 0);
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
    ASSERT( reader.nodeType() == balxml::Reader::e_NODE_TYPE_TEXT);
    ASSERT( reader.nodeHasValue());
    ASSERT(!bsl::strcmp(reader.nodeValue(), "212-318-2000"));
    ASSERT( reader.nodeDepth() == 3);
    ASSERT( reader.numAttributes() == 0);
    ASSERT(!reader.isEmptyElement());

    rc = advancePastWhiteSpace(reader);
    ASSERT( rc == 0);
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
    ASSERT( rc == 0);
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
    ASSERT( rc == 0);
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

                              // ----------------
                              // class TestReader
                              // ----------------

// Do to the size of the TestReader's implementation it does not get promoted
// to the usage example in the 'balxml::Reader' header.

// PRIVATE CLASS METHODS
inline void TestReader::setEncoding(const char *encoding) {
    d_encoding = (0 == encoding || '\0' != encoding[0])
               ? "UTF-8"
               : encoding;
}

inline void TestReader::adjustPrefixStack() {
    // Each time a node is read that is a BAEXML_NODE_TYPE_ELEMENT, we must
    // push an namespace prefixed on the prefix stack.
    if (Obj::e_NODE_TYPE_ELEMENT == d_currentNode->d_type) {

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
    else if (Obj::e_NODE_TYPE_NONE == d_currentNode->d_type) {
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

void TestReader::setPrefixStack(balxml::PrefixStack *prefixes) {
    if (d_isOpen)
    {
        return;                                                       // RETURN
    }

    d_prefixes = prefixes;
}

// MANIPULATORS - OPEN/CLOSE AND NAVIGATION METHODS
int TestReader::open(const char *filename, const char *encoding)
{
    if (d_isOpen) {
        return false;                                                 // RETURN
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
        return false;                                                 // RETURN
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
        return false;                                                 // RETURN
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
        return -1;                                                    // RETURN
    }

    d_currentNode++;

    if (Obj::e_NODE_TYPE_NONE == d_currentNode->d_type) {
        // If the node type is BAEXML_NODE_TYPE_NONE after we have just
        // incremented to the next node, that mean we are at the end of the
        // document.  An easy way to deal with not incrementing d_currentNode
        // to a forbidden value is to simply decrement it.
        d_currentNode--;
        d_prefixes->reset();
        return 1;                                                     // RETURN
    }

    if (d_prefixes && 1 == d_nodeDepth) {
        // The 'TestReader' only recognizes namespace URIs with the prefix
        // (xmlns:) on the top level element, these URIs will be added to the
        // prefix stack.  Namespace URI declarations on any other elements will
        // be treated like normal attributes.  The prefix stack will be reset
        // once the top level element is closed.
        adjustPrefixStack();
    }

    d_nodeDepth += d_currentNode->d_depthChange;

    return 0;
}

int TestReader::lookupAttribute(balxml::ElementAttribute *attribute,
                                int                       index) const {
    if (!d_currentNode || index < 0 || index >= NUM_ATTRIBUTES) {
        return 1;                                                     // RETURN
    }

    const char *qname = d_currentNode->d_attributes[index].d_qname;
    if ('\0' == qname[0]) {
        return 1;                                                     // RETURN
    }

    attribute->reset(d_prefixes,
                     qname,
                     d_currentNode->d_attributes[index].d_value);
    return 0;
}

int TestReader::lookupAttribute(balxml::ElementAttribute *attribute,
                                const char               *qname) const {
    if (!d_currentNode) {
        return 1;                                                     // RETURN
    }

    for (int index = 0; index < NUM_ATTRIBUTES; ++index) {
        const char *currentQName = d_currentNode->d_attributes[index].d_qname;

        if (!bsl::strcmp(currentQName, qname)) {
            attribute->reset(d_prefixes,
                             qname,
                             d_currentNode->d_attributes[index].d_value);
            return 0;                                                 // RETURN
        }
    }

    return 1;
}

int TestReader::lookupAttribute(balxml::ElementAttribute *attribute,
                                const char               *localName,
                                const char               *namespaceUri) const {
    if (!d_currentNode) {
        return 1;                                                     // RETURN
    }

    balxml::ElementAttribute a;

    for (int index = 0; index < NUM_ATTRIBUTES; ++index) {
        const char *qname = d_currentNode->d_attributes[index].d_qname;

        if ('\0' == qname[0]) {
            break;
        }

        a.reset(d_prefixes, qname, d_currentNode->d_attributes[index].d_value);

        if (!bsl::strcmp(a.localName(), localName) &&
            !bsl::strcmp(a.namespaceUri(), namespaceUri)) {
            *attribute = a;
            return 0;                                                 // RETURN
        }
    }

    return 1;
}

int TestReader::lookupAttribute(balxml::ElementAttribute *attribute,
                                const char               *localName,
                                int                       namespaceId) const {
    if (!d_currentNode) {
        return 1;                                                     // RETURN
    }

    balxml::ElementAttribute a;

    for (int index = 0; index < NUM_ATTRIBUTES; ++index) {
        const char *qname = d_currentNode->d_attributes[index].d_qname;

        if (!bsl::strcmp("", qname)) {
            break;
        }

        a.reset(d_prefixes, qname, d_currentNode->d_attributes[index].d_value);

        if (!bsl::strcmp(a.localName(), localName) &&
            a.namespaceId(), namespaceId) {
            *attribute = a;
            return 0;                                                 // RETURN
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

const balxml::ErrorInfo& TestReader::errorInfo() const {
    return d_errorInfo;
}

int TestReader::getLineNumber() const {
    return 0;
}

int TestReader::getColumnNumber() const {
    return 0;
}

balxml::PrefixStack *TestReader::prefixStack() const {
    return d_prefixes;
}

TestReader::NodeType TestReader::nodeType() const {
    if (!d_currentNode || !d_isOpen) {
        return e_NODE_TYPE_NONE;                                      // RETURN
    }

    return d_currentNode->d_type;
}

const char *TestReader::nodeName() const {
    if (!d_currentNode || !d_isOpen) {
        return 0;                                                     // RETURN
    }

    return d_currentNode->d_qname;
}

const char *TestReader::nodeLocalName() const {
    if (!d_currentNode || !d_isOpen) {
        return 0;                                                     // RETURN
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
        return false;                                                 // RETURN
    }

    if (0 == d_currentNode->d_nodeValue) {
        return 0;                                                     // RETURN
    }

    return ('\0' != d_currentNode->d_nodeValue[0]);
}

const char *TestReader::nodeValue() const {
    if (!d_currentNode || !d_isOpen) {
        return 0;                                                     // RETURN
    }

    return d_currentNode->d_nodeValue;
}

int TestReader::nodeDepth() const {
    return d_nodeDepth;
}

int TestReader::numAttributes() const {
    for (int index = 0; index < NUM_ATTRIBUTES; ++index) {
        if (0 == d_currentNode->d_attributes[index].d_qname) {
            return index;                                             // RETURN
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

// ============================================================================
//                               MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? bsl::atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;

    bsl::cout << "TEST " << __FILE__ << " CASE " << test << bsl::endl;;

    switch (test) { case 0:  // Zero is always the leading case.
      case 3: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //
        // This is a simple implementation of the 'balxml::Reader' interface.
        //
        // Concerns:
        //   That the usage example compiles and runs correctly.
        //   That a class can be derived from 'balxml::Reader' and that it can
        //       override all the methods.
        //
        // Plan:
        //   Copy the usage example from the component-level documentation.
        //   Replace 'assert' with 'ASSERT'.
        //
        // Testing:
        //   balxml::Reader();  // Constructor
        //   ~balxml::Reader(); // Destructor
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
        //
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nUSAGE EXAMPLE"
                               << "\n=============" << bsl::endl;

        usageExample();

      } break;
      case 2: {
        // ------------------------------------------------------------------
        // NODETYPE STREAMING
        //
        // Concerns:
        //: 1 Streaming a NodeType enumeration value results in the same
        //:   string as calling 'nodeTypeAsString()'.
        //
        // Plan:
        //: 1 Stream various NodeType values to a string stream and compare
        //:   the results to 'nodeTypeAsString()'. (C-1)
        //
        // Testing:
        //   bsl::ostream& operator(bsl::ostream&, balxml::Reader::NodeType);
        // ------------------------------------------------------------------

        if (verbose) bsl::cout      << bsl::endl
            << "NODETYPE STREAMING" << bsl::endl
            << "==================" << bsl::endl;

        static const balxml::Reader::NodeType DATA[] = {
            balxml::Reader::e_NODE_TYPE_NONE,
            balxml::Reader::e_NODE_TYPE_ELEMENT,
            balxml::Reader::e_NODE_TYPE_TEXT,
            balxml::Reader::e_NODE_TYPE_CDATA,
            balxml::Reader::e_NODE_TYPE_ENTITY_REFERENCE,
            balxml::Reader::e_NODE_TYPE_ENTITY,
            balxml::Reader::e_NODE_TYPE_PROCESSING_INSTRUCTION,
            balxml::Reader::e_NODE_TYPE_COMMENT,
            balxml::Reader::e_NODE_TYPE_DOCUMENT,
            balxml::Reader::e_NODE_TYPE_DOCUMENT_TYPE,
            balxml::Reader::e_NODE_TYPE_DOCUMENT_FRAGMENT,
            balxml::Reader::e_NODE_TYPE_NOTATION,
            balxml::Reader::e_NODE_TYPE_WHITESPACE,
            balxml::Reader::e_NODE_TYPE_SIGNIFICANT_WHITESPACE,
            balxml::Reader::e_NODE_TYPE_END_ELEMENT,
            balxml::Reader::e_NODE_TYPE_END_ENTITY,
            balxml::Reader::e_NODE_TYPE_XML_DECLARATION,
            balxml::Reader::NodeType(31)   // out of range value
        };
        static const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int i = 0; i < NUM_DATA; ++i) {
            balxml::Reader::NodeType NT = DATA[i];
            if (veryVerbose) {
                T_ bsl::cout << "Streaming " << NT << bsl::endl;
            }
            bsl::ostringstream os;
            os << NT;
            LOOP_ASSERT(NT, balxml::Reader::nodeTypeAsString(NT) == os.str());
        }
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
