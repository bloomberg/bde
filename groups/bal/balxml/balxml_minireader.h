// balxml_minireader.h                                                -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BALXML_MINIREADER
#define INCLUDED_BALXML_MINIREADER

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide light-weight implementation of 'balxml::Reader' protocol.
//
//@CLASSES:
//   balxml::MiniReader: light-weight 'balxml::Reader' implementation
//
//@SEE_ALSO: balxml_reader, balxml_errorinfo
//
//@DESCRIPTION: The 'balxml::MiniReader' class is a light-weight
// implementation of 'balxml::Reader' interface.  The API acts as a currentNode
// going forward on the document stream and stopping at each node in the way.
// The current node refers to the node on which the reader is positioned.  The
// user's code keeps control of the progress and simply calls a 'read'
// function repeatedly to progress to each node in sequence in document order.
// This provides a far more standard, easy to use and powerful API than the
// existing SAX.
//
///Usage
///-----
// For this example, we will use 'balxml::MiniReader' to read each node in an
// XML document.  We do not care about whitespace, so we use the following
// utility function to skip over any whitespace nodes.  This makes our example
// more portable to other implementations of the 'balxml::Reader' protocol that
// handle whitespace differently from 'balxml::MiniReader'.
//..
//  int advancePastWhiteSpace(balxml::Reader& reader) {
//      const char *whiteSpace = "\n\r\t ";
//      const char *value = '\0';
//      int         type = 0;
//      int         rc = 0;
//
//      do {
//          rc    = reader.advanceToNextNode();
//          value = reader.nodeValue();
//          type  = reader.nodeType();
//      } while(0 == rc &&
//              type == balxml::Reader::BAEXML_NODE_TYPE_WHITESPACE ||
//              (type == balxml::Reader::BAEXML_NODE_TYPE_TEXT &&
//               bsl::strlen(value) == bsl::strspn(value, whiteSpace)));
//
//      assert( reader.nodeType() !=
//                                balxml::Reader::BAEXML_NODE_TYPE_WHITESPACE);
//
//      return rc;
//  }
//..
// The main program parses an XML string using the TestReader
//..
//  int main()
//  {
//..
// The following string describes xml for a very simple user directory.  The
// top level element contains one xml namespace attribute, with one embedded
// entry describing a user.
//..
//      const char TEST_XML_STRING[] =
//         "<?xml version='1.0' encoding='UTF-8'?>\n"
//         "<directory-entry xmlns:dir="
//                                "'http://bloomberg.com/schemas/directory'>\n"
//         "    <name>John Smith</name>\n"
//         "    <phone dir:phonetype='cell'>212-318-2000</phone>\n"
//         "    <address/>\n"
//         "</directory-entry>\n";
//..
// In order to read the XML, we first need to construct a
// 'balxml::NamespaceRegistry' object, a 'balxml::PrefixStack' object, and a
// 'TestReader' object, where 'TestReader' is a derived implementation of
// 'balxml_reader'.
//..
//      balxml::NamespaceRegistry namespaces;
//      balxml::PrefixStack prefixStack(&namespaces);
//      balxml::MiniReader miniReader; balxml::Reader& reader = miniReader;
//
//      assert(!reader.isOpen());
//..
// The reader uses a 'balxml::PrefixStack' to manage namespace prefixes so we
// need to set it before we call open.
//..
//      reader.setPrefixStack(&prefixStack);
//      assert(reader.prefixStack());
//      assert(reader.prefixStack() == &prefixStack);
//..
// Now we call the 'open' method to setup the reader for parsing using the data
// contained in the in the XML string.
//..
//      reader.open(TEST_XML_STRING, sizeof(TEST_XML_STRING) -1, 0, "UTF-8");
//..
// Confirm that the 'bdem::Reader' has opened properly
//..
//      assert( reader.isOpen());
//      assert(!bsl::strncmp(reader.documentEncoding(), "UTF-8", 5));
//      assert( reader.nodeType() == balxml::Reader::BAEXML_NODE_TYPE_NONE);
//      assert(!reader.nodeName());
//      assert(!reader.nodeHasValue());
//      assert(!reader.nodeValue());
//      assert(!reader.nodeDepth());
//      assert(!reader.numAttributes());
//      assert(!reader.isEmptyElement());
//..
// Advance through all the nodes and assert all information contained at each
// node is correct.
//
// Assert the next node's document type is xml.
//..
//      int rc = advancePastWhiteSpace(reader);
//      assert( 0 == rc);
//      assert( reader.nodeType() ==
//                           balxml::Reader::BAEXML_NODE_TYPE_XML_DECLARATION);
//      assert(!bsl::strcmp(reader.nodeName(), "xml"));
//      assert( reader.nodeHasValue());
//      assert(!bsl::strcmp(reader.nodeValue(),
//                          "version='1.0' encoding='UTF-8'"));
//      assert( reader.nodeDepth() == 1);
//      assert(!reader.numAttributes());
//      assert(!reader.isEmptyElement());
//      assert( 0 == rc);
//      assert( reader.nodeDepth() == 1);
//..
// Advance to the top level element, which has one attribute, the xml
// namespace.  Assert the namespace information has been added correctly to the
// prefix stack.
//..
//      rc = advancePastWhiteSpace(reader);
//      assert( 0 == rc);
//      assert( reader.nodeType() == balxml::Reader::BAEXML_NODE_TYPE_ELEMENT);
//      assert(!bsl::strcmp(reader.nodeName(), "directory-entry"));
//      assert(!reader.nodeHasValue());
//      assert( reader.nodeDepth() == 1);
//      assert( reader.numAttributes() == 1);
//      assert(!reader.isEmptyElement());
//
//      assert(!bsl::strcmp(prefixStack.lookupNamespacePrefix("dir"), "dir"));
//      assert(prefixStack.lookupNamespaceId("dir") == 0);
//      assert(!bsl::strcmp(prefixStack.lookupNamespaceUri("dir"),
//                          "http://bloomberg.com/schemas/directory"));
//..
// The XML being read contains one entry describing a user, advance the users
// name name and assert all information can be read correctly.
//..
//      rc = advancePastWhiteSpace(reader);
//      assert( 0 == rc);
//      assert( reader.nodeType() == balxml::Reader::BAEXML_NODE_TYPE_ELEMENT);
//      assert(!bsl::strcmp(reader.nodeName(), "name"));
//      assert(!reader.nodeHasValue());
//      assert( reader.nodeDepth() == 2);
//      assert( reader.numAttributes() == 0);
//      assert(!reader.isEmptyElement());
//
//      rc = reader.advanceToNextNode();
//      assert( 0 == rc);
//      assert( reader.nodeType() == balxml::Reader::BAEXML_NODE_TYPE_TEXT);
//      assert( reader.nodeHasValue());
//      assert(!bsl::strcmp(reader.nodeValue(), "John Smith"));
//      assert( reader.nodeDepth() == 3);
//      assert( reader.numAttributes() == 0);
//      assert(!reader.isEmptyElement());
//
//      rc = reader.advanceToNextNode();
//      assert( 0 == rc);
//      assert( reader.nodeType() ==
//                               balxml::Reader::BAEXML_NODE_TYPE_END_ELEMENT);
//      assert(!bsl::strcmp(reader.nodeName(), "name"));
//      assert(!reader.nodeHasValue());
//      assert( reader.nodeDepth() == 2);
//      assert( reader.numAttributes() == 0);
//      assert(!reader.isEmptyElement());
//..
// Advance to the user's phone number and assert all information can be read
// correctly.
//..
//      rc = advancePastWhiteSpace(reader);
//      assert( 0 == rc);
//      assert( reader.nodeType() == balxml::Reader::BAEXML_NODE_TYPE_ELEMENT);
//      assert(!bsl::strcmp(reader.nodeName(), "phone"));
//      assert(!reader.nodeHasValue());
//      assert( reader.nodeDepth() == 2);
//      assert( reader.numAttributes() == 1);
//      assert(!reader.isEmptyElement());
//..
// The phone node has one attribute, look it up and assert the
// 'balxml::ElementAttribute' contains valid information and that the prefix
// returns the correct namespace URI from the prefix stack.
//..
//      balxml::ElementAttribute elemAttr;
//
//      rc = reader.lookupAttribute(&elemAttr, 0);
//      assert( 0 == rc);
//      assert(!elemAttr.isNull());
//      assert(!bsl::strcmp(elemAttr.qualifiedName(), "dir:phonetype"));
//      assert(!bsl::strcmp(elemAttr.value(), "cell"));
//      assert(!bsl::strcmp(elemAttr.prefix(), "dir"));
//      assert(!bsl::strcmp(elemAttr.localName(), "phonetype"));
//      assert(!bsl::strcmp(elemAttr.namespaceUri(),
//                          "http://bloomberg.com/schemas/directory"));
//      assert( elemAttr.namespaceId() == 0);
//
//      assert(!bsl::strcmp(prefixStack.lookupNamespaceUri(elemAttr.prefix()),
//                          elemAttr.namespaceUri()));
//
//      rc = advancePastWhiteSpace(reader);
//      assert( 0 == rc);
//      assert( reader.nodeType() == balxml::Reader::BAEXML_NODE_TYPE_TEXT);
//      assert( reader.nodeHasValue());
//      assert(!bsl::strcmp(reader.nodeValue(), "212-318-2000"));
//      assert( reader.nodeDepth() == 3);
//      assert( reader.numAttributes() == 0);
//      assert(!reader.isEmptyElement());
//
//      rc = advancePastWhiteSpace(reader);
//      assert( 0 == rc);
//      assert( reader.nodeType() ==
//                               balxml::Reader::BAEXML_NODE_TYPE_END_ELEMENT);
//      assert(!bsl::strcmp(reader.nodeName(), "phone"));
//      assert(!reader.nodeHasValue());
//      assert( reader.nodeDepth() == 2);
//      assert( reader.numAttributes() == 0);
//      assert(!reader.isEmptyElement());
//..
// Advance to the user's address and assert all information can be read
// correctly.
//..
//      rc = advancePastWhiteSpace(reader);
//      assert( 0 == rc);
//      assert( reader.nodeType() == balxml::Reader::BAEXML_NODE_TYPE_ELEMENT);
//      assert(!bsl::strcmp(reader.nodeName(), "address"));
//      assert(!reader.nodeHasValue());
//      assert( reader.nodeDepth() == 2);
//      assert( reader.numAttributes() == 0);
//      assert( reader.isEmptyElement());
//..
// Advance to the end element.
//..
//      rc = advancePastWhiteSpace(reader);
//      assert( 0 == rc);
//      assert( reader.nodeType() ==
//                               balxml::Reader::BAEXML_NODE_TYPE_END_ELEMENT);
//      assert(!bsl::strcmp(reader.nodeName(), "directory-entry"));
//      assert(!reader.nodeHasValue());
//      assert( reader.nodeDepth() == 1);
//      assert( reader.numAttributes() == 0);
//      assert(!reader.isEmptyElement());
//..
// Close the reader.
//..
//      reader.close();
//      assert(!reader.isOpen());
//
//      return 0;
//  }
//..

#ifndef INCLUDED_BALSCM_VERSION
#include <balscm_version.h>
#endif

#ifndef INCLUDED_BALXML_READER
#include <balxml_reader.h>
#endif

#ifndef INCLUDED_BALXML_ELEMENTATTRIBUTE
#include <balxml_elementattribute.h>
#endif

#ifndef INCLUDED_BALXML_NAMESPACEREGISTRY
#include <balxml_namespaceregistry.h>
#endif

#ifndef INCLUDED_BALXML_PREFIXSTACK
#include <balxml_prefixstack.h>
#endif

#ifndef INCLUDED_BSLMA_ALLOCATOR
#include <bslma_allocator.h>
#endif

#ifndef INCLUDED_BSL_FSTREAM
#include <bsl_fstream.h>
#endif

#ifndef INCLUDED_BSL_VECTOR
#include <bsl_vector.h>
#endif

#ifndef INCLUDED_BSL_STRING
#include <bsl_string.h>
#endif

#ifndef INCLUDED_BSL_CSTRING
#include <bsl_cstring.h>
#endif

#ifndef INCLUDED_BSL_CSTDDEF
#include <bsl_cstddef.h>
#endif

#ifndef INCLUDED_BSL_CSTDLIB
#include <bsl_cstdlib.h>
#endif

namespace BloombergLP  {

namespace balxml {
                              // ================
                              // class MiniReader
                              // ================

class MiniReader :  public Reader {
    // This 'class' provides a concrete and efficient implementation of the
    // 'Reader' protocol.

  private:
    // PRIVATE TYPES
    enum {
        k_MIN_BUFSIZE     = 1024,        // MIN - 1 KB
        k_MAX_BUFSIZE     = 1024 * 128,  // MAX - 128 KB
        k_DEFAULT_BUFSIZE = 1024 * 8,    // DEFAULT - 8 KB
        k_DEFAULT_DEPTH   = 20           // Average expected deep
    };                                        // to minimize allocations

    typedef ElementAttribute Attribute;
    typedef bsl::vector<Attribute> AttributeVector;

    struct Node;
    friend struct Node;
    struct Node
    {
        enum {
            k_NODE_NO_FLAGS = 0x0000,
            k_NODE_EMPTY    = 0x0001
        };

        NodeType         d_type;
        const char      *d_qualifiedName;
        const char      *d_prefix;
        const char      *d_localName;
        const char      *d_value;
        int              d_namespaceId;
        const char      *d_namespaceUri;
        int              d_flags;
        AttributeVector  d_attributes;
        size_t           d_attrCount;
        size_t           d_namespaceCount;
        int              d_startPos;
        int              d_endPos;

        Node(bslma::Allocator *basicAllocator = 0);
        Node(const Node& other, bslma::Allocator *basicAllocator = 0);

        void reset();
        void swap(Node& other);
        void addAttribute(const Attribute& attr);
    };

    typedef bsl::pair<bsl::string, int> Element;

    typedef bsl::vector<Element> ElementVector;

    enum State
    {
        ST_INITIAL,   // Initial state after successful open
        ST_TAG_BEGIN, // Current position - next symbol after '<'
        ST_TAG_END,   // Current position - next symbol after '>'
        ST_EOF,       // End of Data is reached successfully
        ST_ERROR,     // Parser error : prevents from further scanning
        ST_CLOSED     // close method has been called
    };

    enum Flags
    {
        FLG_READ_EOF    = 0x0001,  // End of input data
        FLG_ROOT_CLOSED = 0x0002   // Root closed
    };

  private:
    // PRIVATE DATA
    bslma::Allocator         *d_allocator;
    State                     d_state;
    int                       d_flags;
    int                       d_readSize;
    bsl::vector<char>         d_parseBuf;
    int                       d_streamOffset;

    bsl::ifstream             d_stream;
    bsl::streambuf           *d_streamBuf;
    const char *              d_memStream;      // memory buffer to decode from
    size_t                    d_memSize;        // memory buffer size

    char                     *d_startPtr;
    char                     *d_endPtr;
    char                     *d_scanPtr;        // pointer used to traverse the
                                                // input

    char                     *d_markPtr;        // pointer to the previous node
                                                // value

    char                     *d_attrNamePtr;
    char                     *d_attrValPtr;

    int                       d_lineNum;      // current line number
    char                     *d_linePtr;      // position of the beginning of
                                              // current line

    ErrorInfo                 d_errorInfo;
    XmlResolverFunctor        d_resolver;

    NamespaceRegistry         d_ownNamespaces;
    PrefixStack               d_ownPrefixes;
    PrefixStack              *d_prefixes;

    Node                      d_currentNode;
    size_t                    d_activeNodesCount;  // active nodes count
    ElementVector             d_activeNodes;       // active nodes stack

    bsl::string               d_baseURL;
    bsl::string               d_encoding;
    bsl::string               d_dummyStr;

    unsigned int              d_options;      // option flags for the reader

  private:
    // NOT IMPLEMENTED
    MiniReader(const MiniReader&);             // = delete;
    MiniReader& operator=(const MiniReader&);  // = delete;

    // PRIVATE MANIPULATORS
    Node&       currentNode();
    const Node& currentNode() const;

    int setError(ErrorInfo::Severity error, const bsl::string &msg);

    int setParseError(const char *errText,
                      const char *startFragment,
                      const char *endFragment);

    // HIGH LEVEL PARSING PRIMITIVES

    void  preAdvance();
    const bsl::string& findNamespace(const char *prefix) const;
    const bsl::string& findNamespace(const bsl::string &prefix) const;
    int   checkPrefixes();

    int   scanNode();
        // Scan the node at the current position.
    int   scanOpenTag();
    int   scanProcessingInstruction();
    int   scanExclaimConstruct();
    int   scanText();
    int   scanStartElement();
    int   scanEndElement();
    int   scanAttributes();
    int   addAttribute();
    int   updateElementInfo();
    int   updateAttributes();

    // LOW LEVEL PARSING PRIMITIVES
    const char *rebasePointer(const char *ptr, const char *newBase);
    void  rebasePointers(const char *newBase, size_t newLength);

    int   readInput();
    int   doOpen(const char *url, const char *encoding);

    int   peekChar();
        // Return the character at the current position, and zero if the end of
        // stream was reached.

    int   getChar();
        // Return the character at the current position and then advance the
        // current position.  If the end of stream is reached the return value
        // is zero.  The behavior is undefined if this method is called once
        // the end is reached.

    int   getCharAndSet(char ch);
        // Set the specified symbol 'ch' at the current position.  Return the
        // original character at the current position, and advance the current
        // position.  If the end of stream is reached the return value is zero.
        // The behavior is undefined if this method is called once the end is
        // reached.

    bool  checkForNewLine();
        // Check if the current symbol is NL and adjust line number
        // information.  Return 'true' if it was NL, otherwise 'false'

    int   skipSpaces();
        // Skip spaces and set the current position to first non space
        // character or to end if there is no non space found symbol.  Return
        // the character at the new current position.

    int   scanForSymbol(char symbol);
        // Scan for the specified 'symbol' and set the current position to the
        // found symbol.  Return the character at the new current position.  If
        // the symbol is not found, the current position is set to end and
        // returned value is zero.

    int   scanForSymbolOrSpace(char symbol1, char symbol2);
    int   scanForSymbolOrSpace(char symbol);
        // Scan one of the specified 'symbol', 'symbol1', or 'symbol2'
        // characters or any space character and set the current position to
        // the found symbol.  Return the character at the new current position.
        // If there were no symbols found, the current position is set to end
        // and returned value is zero.

    int   scanForString(const char * str);
        // Scan for the required string and set the current position to the
        // first character of the found string.  Return the character at the
        // new current position.  If there were no symbols found, the current
        // position is set to end and returned value is zero.

    bool skipIfMatch(const char *str);
        // Compare the content of the buffer, starting from the current
        // position, with the specified string 'str'.  If matches, advance the
        // current position by the length of 'str' and return 'true'; otherwise
        // return 'false' and the current position is unmodified.

  public:
    // PUBLIC CREATORS
    virtual ~MiniReader();

    explicit MiniReader(bslma::Allocator *basicAllocator = 0);
    explicit MiniReader(int bufSize, bslma::Allocator *basicAllocator = 0);
        // Construct a reader with the optionally specified 'bufSize' and use
        // the optionally specified 'basicAllocator' to allocate memory.  The
        // instantiated MiniReader will utilize a memory buffer of 'bufSize'
        // while reading the input document.  If 'basicAllocator' null, the
        // currently installed default allocator will be used.  Note that
        // 'bufSize' is a hint, which may be modified or ignored if it is not
        // within a "sane" range.

    // CLASS METHODS
    //------------------------------------------------
    // INTERFACE Reader
    //------------------------------------------------

    // MANIPULATORS - SETUP METHODS
    virtual void setPrefixStack(PrefixStack *prefixes);
        // Set the prefix stack to the stack at the specified 'prefixes'
        // address or disable prefix stack support if 'prefixes' == 0.  This
        // stack is used to push and pop namespace prefixes as the parse
        // progresses, so that, at any point, the stack will reflect the set of
        // active prefixes for the current node.  It is legitimate to pass a
        // stack that already contains prefixes, these prefixes shall be
        // preserved when 'close' is called, i.e., the prefix stack shall be
        // returned to the stack depth it had when 'setPrefixStack' was called.
        // The behavior is undefined if this method is called after calling
        // 'open' and before calling 'close'.

    virtual void setResolver(XmlResolverFunctor resolver);
        // Set the external XML resource resolver to the specified 'resolver'.
        // The XML resource resolver is used by the 'balxml_reader' to find and
        // open an external resources (See the 'XmlResolverFunctor' typedef for
        // more details).  The XML resource resolver remains valid; it is not
        // effected by a call to 'close' and should be available until the
        // reader is destroyed.  The behavior is undefined if this method is
        // called after calling 'open' and before calling 'close'.

    // MANIPULATORS - OPEN/CLOSE AND NAVIGATION METHODS
    virtual int open(const char *filename, const char *encoding = 0);
        // Set up the reader for parsing using the data contained in the XML
        // file described by the specified 'filename', and set the encoding
        // value to the optionally specified 'encoding' ("ASCII", "UTF-8",
        // etc).  Returns 0 on success and non-zero otherwise.  The encoding
        // passed to 'Reader::open' will take effect only when there is no
        // encoding information in the original document, i.e., the encoding
        // information obtained from the XML file described by the 'filename'
        // trumps all.  If there is no encoding provided within the document
        // and 'encoding' is null or a blank string is passed, then set the
        // encoding to the default "UTF-8".  It is an error to 'open' a reader
        // that is already open.  Note that the reader will not be on a valid
        // node until 'advanceToNextNode' is called.

    virtual int open(const char  *buffer,
                     bsl::size_t  size,
                     const char  *url = 0,
                     const char  *encoding = 0);
        // Set up the reader for parsing using the data contained in the
        // specified (XML) 'buffer' of the specified 'size', set the base URL
        // to the optionally specified 'url' and set the encoding value to the
        // optionally specified 'encoding' ("ASCII", "UTF-8", etc).  Return 0
        // on success and non-zero otherwise.  If 'url' is null 0 or a blank
        // string is passed, then base URL will be empty.  The encoding passed
        // to 'Reader::open' will take effect only when there is no encoding
        // information in the original document, i.e., the encoding information
        // obtained from the (XML) 'buffer' trumps all.  If there is no
        // encoding provided within the document and 'encoding' is null or a
        // blank string is passed, then set the encoding to the default
        // "UTF-8".  It is an error to 'open' a reader that is already open.
        // Note that the reader will not be on a valid node until
        // 'advanceToNextNode' is called.

    virtual int open(bsl::streambuf *stream,
                     const char     *url = 0,
                     const char     *encoding = 0);
        // Set up the reader for parsing using the data contained in the
        // specified (XML) 'stream', set the base URL to the optionally
        // specified 'url' and set the encoding value to the optionally
        // specified 'encoding' ("ASCII", "UTF-8", etc).  Return 0 on success
        // and non-zero otherwise.  If 'url' is null or a blank string is
        // passed, then base URL will be empty.  The encoding passed to
        // 'Reader::open' will take effect only when there is no encoding
        // information in the original document, i.e., the encoding information
        // obtained from the (XML) 'stream' trumps all.  If there is no
        // encoding provided within the document and 'encoding' is null or a
        // blank string is passed, then set the encoding to the default
        // "UTF-8".  It is an error to 'open' a reader that is already open.
        // Note that the reader will not be on a valid node until
        // 'advanceToNextNode' is called.

    virtual void close();
        // Close the reader.  Most, but not all state is reset.  Specifically,
        // the XML resource resolver and the prefix stack remain.  The prefix
        // stack shall be returned to the stack depth it had when
        // 'setPrefixStack' was called.  Call the method 'open' to reuse the
        // reader.  Note that 'close' invalidates all strings and data
        // structures obtained via 'Reader' accessors.  E.g., the pointer
        // returned from 'nodeName' for this node will not be valid once
        // 'close' is called.

    virtual int advanceToNextNode();
        // Move to the next node in the data steam created by 'open' thus
        // allowing the node's properties to be queried via the 'Reader'
        // accessors.  Return 0 on successful read, 1 if there are no more
        // nodes to read, and a negative number otherwise.  Note that each call
        // to 'advanceToNextNode' invalidates strings and data structures
        // returned when 'Reader' accessors where call for the "prior node".
        // E.g., the pointer returned from 'nodeName' for this node will not be
        // valid once 'advanceToNextNode' is called.  Note that the reader will
        // not be on a valid node until the first call to 'advanceToNextNode'
        // after the reader is opened.

    virtual int lookupAttribute(ElementAttribute *attribute, int index) const;
        // Find the attribute at the specified 'index' in the current node, and
        // fill in the specified 'attribute' structure.  Return 0 on success, 1
        // if no attribute is found at the 'index', and an a negative value
        // otherwise.  The strings that were filled into the 'attribute'
        // structure are invalid upon the next 'advanceToNextNode' or 'close'
        // is called.

    virtual int lookupAttribute(ElementAttribute *attribute,
                                const char       *qname) const;
        // Find the attribute with the specified 'qname' (qualified name) in
        // the current node, and fill in the specified 'attribute' structure.
        // Return 0 on success, 1 if there is no attribute found with 'qname',
        // and a negative value otherwise.  The strings that were filled into
        // the 'attribute' structure are invalid upon the next
        // 'advanceToNextNode' or 'close' is called.

    virtual int lookupAttribute(ElementAttribute *attribute,
                                const char       *localName,
                                const char       *namespaceUri) const;
        // Find the attribute with the specified 'localName' and specified
        // 'namespaceUri' in the current node, and fill in the specified
        // 'attribute' structure.  Return 0 on success, 1 if there is no
        // attribute found with 'localName' and 'namespaceUri', and a negative
        // value otherwise.  If 'namespaceUri' == 0 or a blank string is
        // passed, then the document's default namespace will be used.  The
        // strings that were filled into the 'attribute' structure are invalid
        // upon the next 'advanceToNextNode' or 'close' is called.

    virtual int lookupAttribute(ElementAttribute *attribute,
                                const char       *localName,
                                int               namespaceId) const;
        // Find the attribute with the specified 'localName' and specified
        // 'namespaceId' in the current node, and fill in the specified
        // 'attribute' structure.  Return 0 on success, 1 if there is no
        // attribute found with 'localName' and 'namespaceId', and a negative
        // value otherwise.  If 'namespaceId' == -1, then the document's
        // default namespace will be used.  The strings that were filled into
        // the 'attribute' structure are invalid upon the next
        // 'advanceToNextNode' or 'close' is called.

    virtual void setOptions(unsigned int flags);
        // Set the options to the flags in the specified 'flags'.  The options
        // for the reader are persistent, i.e., the options are not reset by
        // 'close'.  The behavior is undefined if this method is called after
        // calling 'open' and before calling 'close'.

    // ACCESSORS
    virtual const char *documentEncoding() const;
        // Return the document encoding or NULL on error.  The returned pointer
        // is owned by this object and must not be modified or deallocated by
        // the caller.  The returned pointer becomes invalid when 'close' is
        // called or the reader is destroyed.

    virtual XmlResolverFunctor resolver() const;
        // Return the external XML resource resolver.

    virtual bool isOpen() const;
        // Return true if 'open' was called successfully and 'close' has not
        // yet been called and false otherwise.

    virtual const ErrorInfo& errorInfo() const;
        // Return a reference to the non-modifiable error information for this
        // reader.  The returned value becomes invalid when 'close' is called
        // or the reader is destroyed.

    virtual int getLineNumber() const;
        // Return the current line number within the input stream.  The current
        // line is the last line for which the reader has not yet seen a
        // newline.  Lines are counted starting at one from the time a stream
        // is provide to 'open'.  Return 0 if not available.  Note that a
        // derived-class implementation is not required to count lines and may
        // just return 0.

    virtual int getColumnNumber() const;
        // Return the current column number within the input stream.  The
        // current column number is the number of characters since the last
        // newline was read by the reader plus one, i.e., the first column of
        // each line is column number one.  Return 0 if not available.  Note
        // that a derived-class implementation is not required to count
        // columns and may just return 0.

    virtual PrefixStack *prefixStack() const;
        // Return a pointer to the modifiable prefix stack that is used by this
        // reader to manage namespace prefixes or 0 if namespace support is
        // disabled.  The behavior is undefined if the returned prefix stack is
        // augmented in any way after calling 'open' and before calling
        // 'close'.

    virtual NodeType nodeType() const;
        // Return the node type of the current node if the reader 'isOpen' and
        // has not encounter an error and 'Reader::NONE' otherwise.

    virtual const char *nodeName() const;
        // Return the qualified name of the current node if the current node
        // has a name and NULL otherwise.  The returned pointer is owned by
        // this object and must not be modified or deallocated by the caller.
        // The returned pointer becomes invalid upon the next
        // 'advanceToNextNode', when 'close' is called or the reader is
        // destroyed.

    virtual const char *nodeLocalName() const;
        // Return the local name of the current node if the current node has a
        // local name and NULL otherwise.  The returned pointer is owned by
        // this object and must not be modified or deallocated by the caller.
        // The returned pointer becomes invalid upon the next
        // 'advanceToNextNode', when 'close' is called or the reader is
        // destroyed.

    virtual const char *nodePrefix() const;
        // Return the prefix name of the current node if the correct node has a
        // prefix name and NULL otherwise.  The returned pointer is owned by
        // this object and must not be modified or deallocated by the caller.
        // The returned pointer becomes invalid upon the next
        // 'advanceToNextNode', when 'close' is called or the reader is
        // destroyed.

    virtual int nodeNamespaceId() const;
        // Return the namespace ID of the current node if the current node has
        // a namespace id and a negative number otherwise.

    virtual const char *nodeNamespaceUri() const;
        // Return the namespace URI name of the current node if the current
        // node has a namespace URI and NULL otherwise.  The returned pointer
        // is owned by this object and must not be modified or deallocated by
        // the caller.  The returned pointer becomes invalid upon the next
        // 'advanceToNextNode', when 'close' is called or the reader is
        // destroyed.

    virtual const char *nodeBaseUri() const;
        // Return the base URI name of the current node if the current node has
        // a base URI and NULL otherwise.  The returned pointer is owned by
        // this object and must not be modified or deallocated by the caller.
        // The returned pointer becomes invalid upon the next
        // 'advanceToNextNode', when 'close' is called or the reader is
        // destroyed.

    virtual bool nodeHasValue() const;
        // Return true if the current node has a value and false otherwise.

    virtual const char *nodeValue() const;
        // Return the value of the current node if the current node has a value
        // and NULL otherwise.  The returned pointer is owned by this object
        // and must not be modified or deallocated by the caller.  The returned
        // pointer becomes invalid upon the next 'advanceToNextNode', when
        // 'close' is called or the reader is destroyed.

    virtual int nodeDepth() const;
        // Return the nesting depth of the current node in the XML document.
        // The root node has depth 0.

    virtual int numAttributes() const;
        // Return the number of attributes for the current node if that node
        // has attributes and 0 otherwise.

    virtual bool isEmptyElement() const;
        // Return true if the current node is an element (i.e., node type is
        // 'NODE_TYPE_ELEMENT') that ends with '/>'; and false otherwise.
        // Note that '<a/>' will be considered empty but '<a></a>' will not.

    virtual unsigned int options() const;
        // Return the option flags.

    // ACCESSORS
    // SPECIFIC FOR MiniReader
    int getCurrentPosition() const;
        // Return the current scanner position as offset from the beginning of
        // document.

    int nodeStartPosition() const;
        // Return the byte position within the document corresponding to the
        // first byte of the current node.

    int nodeEndPosition() const;
        // Return the byte position within the document corresponding to the
        // byte following after the last byte of the current node.

};

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

inline
MiniReader::Node& MiniReader::currentNode()
{
    return d_currentNode;
}

inline
const MiniReader::Node& MiniReader::currentNode() const
{
    return d_currentNode;
}

inline
int MiniReader::peekChar()
{
    if (d_scanPtr >= d_endPtr) {
        if (readInput() == 0){
            return 0;                                                 // RETURN
        }
    }

    return *d_scanPtr;
}

inline
int MiniReader::getChar()
{
    if (d_scanPtr >= d_endPtr) {
        if (readInput() == 0){
            return 0;                                                 // RETURN
        }
    }
    return *d_scanPtr++;
}

inline
bool MiniReader::checkForNewLine()
{
    if (*d_scanPtr == '\n') {
        ++d_lineNum;
        d_linePtr = d_scanPtr + 1;
        return true;                                                  // RETURN
    }
    return false;
}

inline
int MiniReader::getCharAndSet(char ch)
{
    //checkForNewLine();   // modify line, column

    int rc = peekChar();   // get current char

    if (rc != 0) {
        checkForNewLine();
        *d_scanPtr++ = ch;  // replace, advance position
    }
    return rc;
}

inline
const char *MiniReader::rebasePointer(const char *ptr, const char *newBase)
{
    if (ptr && ptr >= d_markPtr && ptr <= d_endPtr) {
        return newBase + (ptr-d_markPtr);                             // RETURN
    }
    return ptr;
}

inline
int MiniReader::getCurrentPosition() const
{
    return static_cast<int>(d_streamOffset + (d_scanPtr - d_startPtr));
}

inline
int MiniReader::nodeStartPosition() const
{
    return  currentNode().d_startPos;
}

inline
int MiniReader::nodeEndPosition() const
{
    return  currentNode().d_endPos;
}

}  // close package namespace
}  // close enterprise namespace

#endif  // INCLUDED_BALXML_MINIREADER

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
