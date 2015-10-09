// balxml_validatingreader.t.cpp                                      -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <balxml_validatingreader.h>

#include <bslim_testutil.h>

#include <balxml_errorinfo.h>
#include <balxml_namespaceregistry.h>
#include <balxml_prefixstack.h>
#include <balxml_elementattribute.h>

#include <bsl_cstring.h>     // strlen()
#include <bsl_cstddef.h>
#include <bsl_cstdlib.h>     // atoi()
#include <bsl_iostream.h>
#include <bsl_sstream.h>
#include <bsl_string.h>
#include <bsl_strstream.h>

using namespace BloombergLP;
using namespace bsl;

// ============================================================================
//                                 TEST PLAN
// ----------------------------------------------------------------------------
//                                 Overview
//                                 --------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
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

#define NL "\n"

// ============================================================================
//                   GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------
#define CHK(X) (X != 0 ? (const char *) X : "")
#define NUM_ATTRIBUTES 5

typedef balxml::ValidatingReader Obj;

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
    int            d_retCode;     // return code when we advance to this node
    Obj::NodeType  d_type;        // Describes the type of the XML node
    const char    *d_qname;       // Name qualified name the XML node
    const char    *d_nodeValue;   // Value of the the XML node if null, then
                                  // hasValue() returns false
    int            d_depthChange; // Used to adjust the 'TestReader' depth
                                  // level, valid values are -1, 0 or 1
    bool           d_isEmptyElement;
    struct {
        const char *d_qname; // Qualified name of the attribute
        const char *d_value; // Value of the attribute
    } d_attributes[NUM_ATTRIBUTES]; // Array of attributes

};

static const char *XmlValue = "version='1.0' encoding='UTF-8'";

static const TestNode goodDocument[] = {
    // 'fakeDocument' is an array of 'TestNode's, this array will be use by the
    // 'TestReader' to traverse and describe the user directory XML above.
    { 0, Obj::e_NODE_TYPE_NONE },

    { 0, Obj::e_NODE_TYPE_XML_DECLARATION,
         "xml"          , XmlValue   , +1
                                                                             },

    { 0, Obj::e_NODE_TYPE_ELEMENT,
         "directory-entry" , 0             ,  0,
        false, { "xmlns:dir"    , "http://bloomberg.com/schemas/directory" } },

    { 0, Obj::e_NODE_TYPE_ELEMENT,
         "name"           , 0             , +1,
                                                                             },

    { 0, Obj::e_NODE_TYPE_TEXT,
         0                , "John Smith"  , +1
                                                                             },

    { 0, Obj::e_NODE_TYPE_END_ELEMENT,
         "name"           , 0             , -1
                                                                             },

    { 0, Obj::e_NODE_TYPE_ELEMENT,
         "phone"          , 0             ,  0,
        false, { "dir:phonetype", "cell"                                   } },

    { 0, Obj::e_NODE_TYPE_TEXT,
         0                , "212-318-2000", +1
                                                                             },

    { 0, Obj::e_NODE_TYPE_END_ELEMENT,
         "phone"          , 0             , -1
                                                                             },

    { 0, Obj::e_NODE_TYPE_ELEMENT,
         "address"        , 0             ,  0,
        true                                                                 },

    { 0, Obj::e_NODE_TYPE_END_ELEMENT,
         "directory-entry", 0             , -1
                                                                             },

    { 1, Obj::e_NODE_TYPE_NONE
                                                                             }
};

static const TestNode badDocument[] = {
    // 'fakeDocument' is an array of 'TestNode's, this array will be use by the
    // 'TestReader' to traverse and describe the user directory XML above.
    { 0, Obj::e_NODE_TYPE_NONE },

    { 0, Obj::e_NODE_TYPE_XML_DECLARATION,
         "xml"          , XmlValue      , +1
                                                                             },

    { 0, Obj::e_NODE_TYPE_ELEMENT,
         "directory-entry" , 0             ,  0,
        false, { "xmlns:dir"    , "http://bloomberg.com/schemas/directory" } },

    { 0, Obj::e_NODE_TYPE_ELEMENT,
         "name"           , 0             , +1,
                                                                             },

    { 0, Obj::e_NODE_TYPE_TEXT,
         0                , "John Smith"  , +1
                                                                             },

    { 0, Obj::e_NODE_TYPE_END_ELEMENT,
         "name"           , 0             , -1
                                                                             },

    { 0, Obj::e_NODE_TYPE_ELEMENT,
         "phone"          , 0             ,  0,
        false, { "dir:phonetype", "cell"                                   } },

    { 0, Obj::e_NODE_TYPE_TEXT,
         0                , "212-318-2000", +1
                                                                             },

    { 0, Obj::e_NODE_TYPE_END_ELEMENT,
         "phone"          , 0             , -1
                                                                             },

    { -1, Obj::e_NODE_TYPE_NONE
                                                                             }
};

// Start of usage example, extract to the 'balxml::ValidatingReader' header
// file.

// Create a class that implements the 'balxml::ValidatingReader' interface.
//..
class TestReader : public balxml::ValidatingReader
{

private:
    struct SchemaInfo;
    friend struct SchemaInfo;
    struct SchemaInfo
    {
        bsl::string               d_location;
        bsl::streambuf           *d_streambuf;
        bsl::streambuf::pos_type  d_startPos;
    };
    typedef bsl::vector<SchemaInfo>  SchemasCache;

    const TestNode     *d_document;  // fake document

    balxml::ErrorInfo    d_errorInfo; // Contains the current error information
    balxml::PrefixStack *d_prefixes; // The prefix stack used by the TestReader
    XmlResolverFunctor  d_resolver;  // Just a place holder, not actually used
                                     // while looking up XML information
    bool                d_isOpen;    // Keeps track if the reader is open
    bsl::string         d_encoding;  // The documents encoding
    int                 d_nodeDepth; // The nesting level of the current node
    const TestNode     *d_currentNode; // The current xml node being

    bool                d_flgValidation;  // Validation mode
    SchemasCache        d_schemasCache;   // The cache of schemas

    // PRIVATE CLASS METHODS
    void setEncoding(const char *encoding);
    void adjustPrefixStack();

public:

    // PUBLIC CREATORS
    TestReader(const TestNode *document);
    virtual ~TestReader(void);

    //------------------------------------------------
    // INTERFACE balxml::ValidatingReader
    //------------------------------------------------

    // MANIPULATORS
    virtual void enableValidation(bool validationFlag);

    virtual int addSchema(const char *location, bsl::streambuf *schema);

    virtual void  removeSchemas();

    // ACCESSORS
    virtual bool validationFlag() const;

    //------------------------------------------------
    // INTERFACE baexml::gReader
    //------------------------------------------------

    // MANIPULATORS - SETUP METHODS
    virtual void setResolver(XmlResolverFunctor resolver);

    virtual void setPrefixStack(balxml::PrefixStack *prefixes);

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
int advancePastWhiteSpace(balxml::ValidatingReader& reader) {
    const char *whiteSpace = "\n\r\t ";
    const char *value = 0;
    int         type = 0;
    int         rc = 0;

    do {
        rc    = reader.advanceToNextNode();
        value = reader.nodeValue();
        type  = reader.nodeType();
    } while(0 == rc &&
            (type == balxml::ValidatingReader::e_NODE_TYPE_WHITESPACE ||
             (type == balxml::ValidatingReader::e_NODE_TYPE_TEXT &&
              bsl::strlen(value) == bsl::strspn(value, whiteSpace))));

    ASSERT( reader.nodeType() !=
                         balxml::ValidatingReader::e_NODE_TYPE_WHITESPACE);

    return rc;
}

                              // ----------------
                              // class TestReader
                              // ----------------

// Do to the size of the TestReader's implementation it does not get promoted
// to the usage example in the 'balxml::ValidatingReader' header.

// PRIVATE CLASS METHODS
inline void TestReader::setEncoding(const char *encoding) {
    d_encoding = (0 == encoding || '\0' != encoding[0])
               ? "UTF-8"
               : encoding;
}

inline void TestReader::adjustPrefixStack() {
    // Each time a node is read that is a BAEXML_NODE_TYPE_ELEMENT, we must
    // push an prefixed on the prefix stack.
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
TestReader::TestReader(const TestNode *document)
    : d_document(document)
    , d_errorInfo ()
    , d_prefixes(0)
    , d_resolver()
    , d_isOpen(false)
    , d_encoding()
    , d_nodeDepth(0)
    , d_currentNode(0)
    , d_flgValidation(false)
    , d_schemasCache()
{
}

TestReader::~TestReader(void)
{
}

// ----------------------------------------------------------------------------
//                    balxml::ValidatingReader interface
// ----------------------------------------------------------------------------
bool
TestReader::validationFlag() const
{
    return d_flgValidation;
}

void
TestReader::enableValidation(bool validationFlag)
{
    d_flgValidation = validationFlag;
}

void
TestReader::removeSchemas()
{
    d_schemasCache.clear();
}

int
TestReader::addSchema(const char *location, bsl::streambuf *schema)
{
    SchemaInfo info;

    info.d_location  = location;
    info.d_streambuf = schema;
    info.d_startPos  = schema->pubseekoff(0,
                                          bsl::ios_base::cur,
                                          bsl::ios::in);

    bsl::vector<SchemaInfo>::iterator it1 = d_schemasCache.begin();
    bsl::vector<SchemaInfo>::iterator it2 = d_schemasCache.end();

    for (; it1 != it2; ++it1) {
        if (info.d_location == (*it1).d_location) {
            *it1 = info;
            return 0;                                                 // RETURN
        }
    }

    d_schemasCache.push_back (info);
    return 0;
}

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
int TestReader::open(const char *filename,
                     const char *encoding) {
    if (d_isOpen) {
        return false;                                                 // RETURN
    }
    d_isOpen = true;
    d_nodeDepth = 0;

    d_currentNode = d_document;

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

    d_currentNode = d_document;

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

    d_currentNode = d_document;

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

    if (d_currentNode->d_retCode != 0) {
        d_prefixes->reset();
    }
    else {
        d_currentNode++;

        if (d_prefixes && 1 == d_nodeDepth) {
        // The 'TestReader' only recognizes namespace URIs with the prefix
        // (xmlns:) on the top level element, these URIs will be added to the
        // prefix stack.  Namespace URI declarations on any other elements will
        // be treated like normal attributes.  The prefix stack will be reset
        // once the top level element is closed.
            adjustPrefixStack();
        }

        d_nodeDepth += d_currentNode->d_depthChange;
    }

     return d_currentNode->d_retCode;
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

    attribute->reset(d_prefixes, qname,
                     d_currentNode->d_attributes[index].d_value);
    return 0;
}

int TestReader::lookupAttribute(balxml::ElementAttribute  *attribute,
                                const char                *qname) const {
    if (!d_currentNode) {
        return 1;                                                     // RETURN
    }

    for (int index = 0; index < NUM_ATTRIBUTES; ++index) {
        const char *currentQName = d_currentNode->d_attributes[index].d_qname;

        if (!bsl::strcmp(currentQName, qname)) {
            attribute->reset(d_prefixes, qname,
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

int TestReader::lookupAttribute(balxml::ElementAttribute  *attribute,
                                const char                *localName,
                                int                        namespaceId) const {
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
// ----------------------------------------------------------------------------
//                              USAGE EXAMPLE
// ----------------------------------------------------------------------------
//..
// The following string describes xsd schema for
// the documents we are going to parse
//..
    const char TEST_XSD_STRING[] =
        "<?xml version='1.0' encoding='UTF-8'?>"
        "<xsd:schema xmlns:xsd='http://www.w3.org/2001/XMLSchema'"
        "            xmlns='http://bloomberg.com/schemas/directory'"
        "            targetNamespace='http://bloomberg.com/schemas/directory'"
        "            elementFormDefault='qualified'"
        "            attributeFormDefault='qualified' >"
        " "
        "<xsd:complexType name='entryType'>"
        "    <xsd:sequence>"
        "    <xsd:element name='name' type='xsd:string'/>"
        "    <xsd:element name='phone'>"
        "        <xsd:complexType>"
        "        <xsd:simpleContent>"
        "            <xsd:extension base='xsd:string'>"
        "                <xsd:attribute name='phonetype' type='xsd:string'/>"
        "            </xsd:extension>"
        "        </xsd:simpleContent>"
        "        </xsd:complexType>"
        "    </xsd:element>"
        "    <xsd:element name='address' type='xsd:string'/>"
        "    </xsd:sequence>"
        "</xsd:complexType>"
        " "
        "<xsd:element name='directory-entry' type='entryType'/>"
        "</xsd:schema>";
//..
// The following string describes correct xml for conforming schema.
// The top level element contains one xml namespace attribute, with one
// embedded entry describing a user.
//..
    const char TEST_GOOD_XML_STRING[] =
       "<?xml version='1.0' encoding='UTF-8'?>\n"
       "<directory-entry xmlns:dir='http://bloomberg.com/schemas/directory'\n"
       "     xmlns:xsi='http://www.w3.org/2001/XMLSchema-instance'\n"
       "     xsi:schemaLocation='http://bloomberg.com/schemas/directory  \n"
       "                         aaa.xsd' >\n"
       "    <name>John Smith</name>\n"
       "    <phone dir:phonetype='cell'>212-318-2000</phone>\n"
       "    <address/>\n"
       "</directory-entry>\n";
//..
// The following string describes not valid xml.
// Xml document is well formed, but does not conform schema.
//..
    const char TEST_BAD_XML_STRING[] =
       "<?xml version='1.0' encoding='UTF-8'?>\n"
       "<directory-entry xmlns:dir='http://bloomberg.com/schemas/directory'\n"
       "     xmlns:xsi='http://www.w3.org/2001/XMLSchema-instance'\n"
       "     xsi:schemaLocation='http://bloomberg.com/schemas/directory  \n"
       "                         aaa.xsd' >\n"
       "    <name>John Smith</name>\n"
       "    <phone dir:phonetype='cell'>212-318-2000</phone>\n"
       "</directory-entry>\n";
//..

int parse(balxml::ValidatingReader *reader,
          const char               *xmlData,
          const char               *xsdSchema)
{
//..
// In order to read the XML, we first need to construct a
// 'balxml::NamespaceRegistry' object, a 'balxml::PrefixStack' object, and a
// 'TestReader' object, where 'TestReader' is a derived implementation of
// 'balxml_validatingreader'.
//..
    balxml::NamespaceRegistry namespaces;
    balxml::PrefixStack prefixStack(&namespaces);

    ASSERT(!reader->isOpen());
//..
// The reader uses a 'balxml::PrefixStack' to manage namespace prefixes so we
// need to set it before we call open.
//..
    reader->setPrefixStack(&prefixStack);
    ASSERT(reader->prefixStack() == &prefixStack);
//..
// Setup validation
//..
    reader->removeSchemas();

    reader->enableValidation(true);
    ASSERT(reader->validationFlag());

    bsl::istrstream schemaStream(xsdSchema);
    reader->addSchema("aaa.xsd", schemaStream.rdbuf());
//..
// Now we call the 'open' method to setup the reader for parsing using the data
// contained in the in the XML string.
//..
    int rc = reader->open(xmlData, bsl::strlen(xmlData), 0, "UTF-8");
    ASSERT(rc == 0);
//..
// Confirm that the 'bdem::Reader' has opened properly
//..
    ASSERT(reader->isOpen());

//..
// Do actual document reading
//..
    while(1) {
        rc = reader->advanceToNextNode ();
        if (rc != 0) {
            break;
        }
//..
//      process current node here
//..
    }
//..
// Cleanup and close the reader.
//..
    reader->close();
    ASSERT(!reader->isOpen());

    reader->setPrefixStack(0);
    ASSERT(reader->prefixStack() == 0);

    return rc;
}
//..
// The main program parses an XML string using the TestReader
//..
int usageExample()
{
    TestReader testReader1(goodDocument);

    int rc = parse(&testReader1, TEST_GOOD_XML_STRING, TEST_XSD_STRING);
//..
//  Normal end of data
//..
    ASSERT(rc==1);

    TestReader testReader2(badDocument);

    rc = parse(&testReader2, TEST_BAD_XML_STRING, TEST_XSD_STRING);
//..
//  Parser error - document validation failed
//..
    ASSERT(rc==-1);

    return 0;
}
// End of usage example, extract to the 'balxml::ValidatingReader' header file.

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
      case 2: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //
        // This is a simple implementation of the 'balxml::ValidatingReader'
        // interface.
        //
        // Concerns:
        //   That the usage example compiles and runs correctly.
        //   That a class can be derived from 'balxml::ValidatingReader' and
        //       that it can override all the methods.
        //
        // Plan:
        //   Copy the usage example from the component-level documentation.
        //   Replace 'assert' with 'ASSERT'.
        //
        // Testing:
        //   balxml::ValidatingReader();  // Constructor
        //   ~balxml::ValidatingReader(); // Destructor
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
      case 1: {
            // ------------------------------------------------------------
            // Basic Test: Derived class can be instantiated
            // -------------------------------------------------------------

            if (verbose) bsl::cout << bsl::endl
                << "Basic Test" << bsl::endl
                << "=====================" << bsl::endl;

            TestReader reader(goodDocument);
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
