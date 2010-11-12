// baexml_minireader.h                                                -*-C++-*-
#ifndef INCLUDED_BAEXML_MINIREADER
#define INCLUDED_BAEXML_MINIREADER

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide light-weight implementation of 'baexml_Reader' protocol
//
//@CLASSES: baexml_MiniReader
//
//@SEE_ALSO: baexml_reader, baexml_errorinfo
//
//@AUTHOR: Alexander Libman (alibman1)
//
//@DESCRIPTION: The 'baexml_MiniReader' class is a light-weight
// implementation of 'baexml_Reader' interface.  The API acts as a currentNode
// going forward on the document stream and stopping at each node in the way.
// The current node refers to the node on which the reader is positioned.  The
// user's code keeps control of the progress and simply calls a 'read'
// function repeatedly to progress to each node in sequence in document order.
// This provides a far more standard, easy to use and powerful API than the
// existing SAX.
//
///Usage
///-----
// For this example, we will use 'baexml_MiniReader' to read each node in an
// XML document.  We do not care about whitespace, so we use the following
// utility function to skip over any whitespace nodes.  This makes our example
// more portable to other implementations of the 'baexml_Reader' protocol that
// handle whitespace differently from 'baexml_MiniReader'.
//..
//  int advancePastWhiteSpace(baexml_Reader& reader) {
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
//              type == baexml_Reader::BAEXML_NODE_TYPE_WHITESPACE ||
//              (type == baexml_Reader::BAEXML_NODE_TYPE_TEXT &&
//               bsl::strlen(value) == bsl::strspn(value, whiteSpace)));
//
//      assert( reader.nodeType() !=
//                                 baexml_Reader::BAEXML_NODE_TYPE_WHITESPACE);
//
//      return rc;
//  }
//..
// The main program parses an XML string using the TestReader
//..
//  int main()
//  {
//..
// The following string describes xml for a very simple user directory.
// The top level element contains one xml namespace attribute, with one
// embedded entry describing a user.
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
// 'baexml_NamespaceRegistry' object, a 'baexml_PrefixStack' object, and a
// 'TestReader' object, where 'TestReader' is a derived implementation of
// 'baexml_reader'.
//..
//      baexml_NamespaceRegistry namespaces;
//      baexml_PrefixStack prefixStack(&namespaces);
//      baexml_MiniReader miniReader; baexml_Reader& reader = miniReader;
//
//      assert(!reader.isOpen());
//..
// The reader uses a 'baexml_PrefixStack' to manage namespace prefixes so we
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
// Confirm that the 'bdem_Reader' has opened properly
//..
//      assert( reader.isOpen());
//      assert(!bsl::strncmp(reader.documentEncoding(), "UTF-8", 5));
//      assert( reader.nodeType() == baexml_Reader::BAEXML_NODE_TYPE_NONE);
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
//                            baexml_Reader::BAEXML_NODE_TYPE_XML_DECLARATION);
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
//      assert( reader.nodeType() == baexml_Reader::BAEXML_NODE_TYPE_ELEMENT);
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
//      assert( reader.nodeType() == baexml_Reader::BAEXML_NODE_TYPE_ELEMENT);
//      assert(!bsl::strcmp(reader.nodeName(), "name"));
//      assert(!reader.nodeHasValue());
//      assert( reader.nodeDepth() == 2);
//      assert( reader.numAttributes() == 0);
//      assert(!reader.isEmptyElement());
//
//      rc = reader.advanceToNextNode();
//      assert( 0 == rc);
//      assert( reader.nodeType() == baexml_Reader::BAEXML_NODE_TYPE_TEXT);
//      assert( reader.nodeHasValue());
//      assert(!bsl::strcmp(reader.nodeValue(), "John Smith"));
//      assert( reader.nodeDepth() == 3);
//      assert( reader.numAttributes() == 0);
//      assert(!reader.isEmptyElement());
//
//      rc = reader.advanceToNextNode();
//      assert( 0 == rc);
//      assert( reader.nodeType() ==
//                                baexml_Reader::BAEXML_NODE_TYPE_END_ELEMENT);
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
//      assert( reader.nodeType() == baexml_Reader::BAEXML_NODE_TYPE_ELEMENT);
//      assert(!bsl::strcmp(reader.nodeName(), "phone"));
//      assert(!reader.nodeHasValue());
//      assert( reader.nodeDepth() == 2);
//      assert( reader.numAttributes() == 1);
//      assert(!reader.isEmptyElement());
//..
// The phone node has one attribute, look it up and assert the
// 'baexml_ElementAttribute' contains valid information and that the prefix
// returns the correct namespace URI from the prefix stack.
//..
//      baexml_ElementAttribute elemAttr;
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
//      assert( reader.nodeType() == baexml_Reader::BAEXML_NODE_TYPE_TEXT);
//      assert( reader.nodeHasValue());
//      assert(!bsl::strcmp(reader.nodeValue(), "212-318-2000"));
//      assert( reader.nodeDepth() == 3);
//      assert( reader.numAttributes() == 0);
//      assert(!reader.isEmptyElement());
//
//      rc = advancePastWhiteSpace(reader);
//      assert( 0 == rc);
//      assert( reader.nodeType() ==
//                                baexml_Reader::BAEXML_NODE_TYPE_END_ELEMENT);
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
//      assert( reader.nodeType() == baexml_Reader::BAEXML_NODE_TYPE_ELEMENT);
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
//                                baexml_Reader::BAEXML_NODE_TYPE_END_ELEMENT);
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

#ifndef INCLUDED_BAESCM_VERSION
#include <baescm_version.h>
#endif

#ifndef INCLUDED_BAEXML_READER
#include <baexml_reader.h>
#endif

#ifndef INCLUDED_BAEXML_ELEMENTATTRIBUTE
#include <baexml_elementattribute.h>
#endif

#ifndef INCLUDED_BAEXML_NAMESPACEREGISTRY
#include <baexml_namespaceregistry.h>
#endif

#ifndef INCLUDED_BAEXML_PREFIXSTACK
#include <baexml_prefixstack.h>
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

#ifndef INCLUDED_BSL_CSTDLIB
#include <bsl_cstdlib.h>
#endif

#ifndef BDE_DONT_ALLOW_TRANSITIVE_INCLUDES

#ifndef INCLUDED_BSL_CASSERT
#include <bsl_cassert.h>
#endif

#endif

namespace BloombergLP  {

                        // =======================
                        // class baexml_MiniReader
                        // =======================

class baexml_MiniReader :  public baexml_Reader {
    // TBD doc

  private:
    // PRIVATE TYPES
    enum {
        BAEXML_MIN_BUFSIZE     = 1024,        // MIN - 1 KB
        BAEXML_MAX_BUFSIZE     = 1024 * 128,  // MAX - 128 KB
        BAEXML_DEFAULT_BUFSIZE = 1024 * 8,    // DEFAULT - 8 KB
        BAEXML_DEFAULT_DEPTH   = 20           // Average expected deep
    };                                        // to minimize allocations

    typedef baexml_ElementAttribute Attribute;
    typedef bsl::vector<Attribute>  AttributeVector;

    struct Node;
    friend struct Node;
    struct Node
    {
        enum {
            BAEXML_NODE_NO_FLAGS = 0x0000,
            BAEXML_NODE_EMPTY    = 0x0001
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

        Node (bslma_Allocator *basicAllocator = 0);
        Node (const Node & other,
              bslma_Allocator *basicAllocator = 0);

        void reset();
        void swap(Node & other);
        void addAttribute(const Attribute & attr);
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
    bslma_Allocator         *d_allocator;
    State                    d_state;
    int                      d_flags;
    int                      d_readSize;
    bsl::vector<char>        d_parseBuf;
    int                      d_streamOffset;

    bsl::ifstream            d_stream;
    bsl::streambuf          *d_streamBuf;
    const char *             d_memStream;
    size_t                   d_memSize;

    char                    *d_startPtr;
    char                    *d_endPtr;
    char                    *d_scanPtr;
    char                    *d_markPtr;

    char                    *d_attrNamePtr;
    char                    *d_attrValPtr;

    int                      d_lineNum;  // current line number
    char                    *d_linePtr;  // position of the beginning
                                         // of current line

    baexml_ErrorInfo         d_errorInfo;
    XmlResolverFunctor       d_resolver;

    baexml_NamespaceRegistry d_ownNamespaces;
    baexml_PrefixStack       d_ownPrefixes;
    baexml_PrefixStack *     d_prefixes;

    Node                     d_currentNode;
    size_t                   d_activeNodesCount;
    ElementVector            d_activeNodes;

    bsl::string              d_baseURL;
    bsl::string              d_encoding;
    bsl::string              d_dummyStr;

    unsigned int             d_options; // option flags for the reader

  private:
    // PRIVATE MANIPULATORS
    Node&       currentNode();
    const Node& currentNode() const;

    int   setError (baexml_ErrorInfo::Severity error,
                    const bsl::string&         msg);

    int   setParseError(const char *errText,
                        const char *startFragment,
                        const char *endFragment);

    // HIGH LEVEL PARSING PRIMITIVES
    void  preAdvance();
    const bsl::string&  findNamespace(const char *prefix) const ;
    const bsl::string&  findNamespace(const bsl::string& prefix) const;
    int   checkPrefixes();

    int   scanNode();
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
    const char *rebasePointer(const char* ptr, const char * newBase);
    void  rebasePointers(const char *newBase, size_t newLength);

    int   readInput();
    int   doOpen(const char *url, const char *encoding);

    int   peekChar();
        // Return the character at the current position.  Zero means
        // the end of stream is reached/

    int   getChar();
        // Return the character at the current position
        // and then advance the current position.  If the end of
        // stream is reached the return value is zero.  The behavior
        // is undefined if this method is called once the end is reached.

    int   getCharAndSet(char ch);
        // Set the specified symbol 'ch' at the current position.
        // Return the original character at the current position,
        // and advance the current position.  If the end of
        // stream is reached the return value is zero.  The behavior
        // is undefined if this method is called once the end is reached.

    bool  checkForNewLine();
        // Check if the current symbol is NL and adjust line number
        // information.  Return 'true' if it was NL, otherwise 'false'

    int   skipSpaces();
        // Skip spaces and set the current position to first non space
        // character or to end if there is no non space found symbol.
        // Return the character at the new current position.

    int   scanForSymbol(char symbol);
        // Scan for the required symbol and set the current position
        // to the found symbol.  Return the character at the new current
        // position.  If the symbol is not found, the current position is
        // set to end and returned value is zero.

    int   scanForSymbolOrSpace(char symbol1, char symbol2);
    int   scanForSymbolOrSpace(char symbol);
        // Scan for the one of required symbol(s) or any space character
        // and set the current position to the found symbol.  Return the
        // character at the new current position.  If there were no
        // symbols found, the current position is set to end and
        // returned value is zero.

    int   scanForString(const char * str);
        // Scan for the required string and set the current position to
        // the first character of the found string.  Return the
        // character at the new current position.  If there were no
        // symbols found, the current position is set to end and
        // returned value is zero.

    bool skipIfMatch(const char *str);
        // Compare the content of the buffer, starting from the current
        // position, with the specified string 'str'.  If matches, advance
        // the current position by the length of 'str' and return 'true';
        // otherwise return 'false' and the current position is unmodified.

  public:
    // PUBLIC CREATORS
    virtual ~baexml_MiniReader(void);

    explicit
    baexml_MiniReader(bslma_Allocator *basicAllocator = 0);
    explicit
    baexml_MiniReader(int bufSize,
                      bslma_Allocator *basicAllocator = 0);
       // Construct a reader with the (optionally) specified 'bufSize' and use
       // the specified 'basicAllocator' to allocate memory.  The instantiated
       // baexml_MiniReader will utilize a memory buffer of 'bufSize' while
       // reading the input document.  If 'basicAllocator' == 0, the
       // currently installed default allocator will be used.  Note that
       // 'bufSize' is a hint, which may be modified or ignored if it is not
       // within a "sane" range.

    // CLASS METHODS
    //------------------------------------------------
    // INTERFACE baexml_Reader
    //------------------------------------------------

    // MANIPULATORS - SETUP METHODS
    virtual void setPrefixStack(baexml_PrefixStack *prefixes);
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
        // The XML resource resolver is used by the 'baexml_reader' to find and
        // open an external resources (See the 'XmlResolverFunctor' typedef for
        // more details).  The XML resource resolver remains valid; it is not
        // effected by a call to 'close' and should be available until the
        // reader is destroyed.  The behavior is undefined if this method is
        // called after calling 'open' and before calling 'close'.

    // MANIPULATORS - OPEN/CLOSE AND NAVIGATION METHODS
    virtual int open(const char *filename, const char *encoding = 0);
        // Setup the reader for parsing using the data contained in the XML
        // file described by the specified 'filename', and set the encoding
        // value to the specified 'encoding' ("ASCII", "UTF-8", etc).  Returns
        // 0 on success and non-zero otherwise.  The encoding passed to
        // 'baexml_Reader::open' will take effect only when there is no
        // encoding information in the original document, i.e., the encoding
        // information obtained from the XML file described by the 'filename'
        // trumps all.  If there is no encoding provided within the document
        // and 'encoding' == 0 or a blank string is passed, then set the
        // encoding to the default "UTF-8".  It is an error to 'open' a reader
        // that is already open.  Note that the reader will not be on a valid
        // node until 'advanceToNextNode' is called.

    virtual int open(const char  *buffer,
                     bsl::size_t  size,
                     const char  *url = 0,
                     const char  *encoding = 0);
        // Setup the reader for parsing using the data contained in the
        // specified (XML) 'buffer' of the specified 'size', set the base URL
        // to the specified 'url' and set the encoding value to the specified
        // 'encoding' ("ASCII", "UTF-8", etc).  Return 0 on success and
        // non-zero otherwise.  If the 'url' == 0 or a blank string is passed,
        // then base URL will be empty.  The encoding passed to
        // 'baexml_Reader::open' will take effect only when there is no
        // encoding information in the original document, i.e., the encoding
        // information obtained from the (XML) 'buffer' trumps all.  If
        // there is no encoding provided within the document and 'encoding' ==
        // 0 or a blank string is passed, then set the encoding to the default
        // "UTF-8".  It is an error to 'open' a reader that is already open.
        // Note that the reader will not be on a valid node until
        // 'advanceToNextNode' is called.

    virtual int open(bsl::streambuf *stream,
                     const char     *url = 0,
                     const char     *encoding = 0);
        // Setup the reader for parsing using the data contained in the
        // specified (XML) 'stream', set the base URL to the specified 'url'
        // and set the encoding value to the specified 'encoding' ("ASCII",
        // "UTF-8", etc).  Return 0 on success and non-zero otherwise.  If the
        // 'url' == 0 or a blank string is passed, then base URL will be empty.
        // The encoding passed to 'baexml_Reader::open' will take effect only
        // when there is no encoding information in the original document,
        // i.e., the encoding information obtained from the (XML) 'stream'
        // trumps all.  If there is no encoding provided within the document
        // and 'encoding' == 0 or a blank string is passed, then set the
        // encoding to the default "UTF-8".  It is an error to 'open' a reader
        // that is already open.  Note that the reader will not be on a valid
        // node until 'advanceToNextNode' is called.

    virtual void close();
        // Close the reader.  Most, but not all state is reset.  Specifically,
        // the XML resource resolver and the prefix stack remain.  The prefix
        // stack shall be returned to the stack depth it had when
        // 'setPrefixStack' was called.  Call the method 'open' to reuse the
        // reader.  Note that 'close' invalidates all strings and data
        // structures obtained via 'baexml_Reader' accessors.  E.g., the
        // pointer returned from 'nodeName' for this node will not be valid
        // once 'close' is called.

    virtual int advanceToNextNode();
        // Move to the next node in the data steam created by 'open' thus
        // allowing the node's properties to be queried via the
        // 'baexml_Reader' accessors.  Return 0 on successful read, 1 if there
        // are no more nodes to read, and a negative number otherwise.  Note
        // that each call to 'advanceToNextNode' invalidates strings and data
        // structures returned when 'baexml_Reader' accessors where call for
        // the "prior node".  E.g., the pointer returned from 'nodeName' for
        // this node will not be valid once 'advanceToNextNode' is called.
        // Note that the reader will not be on a valid node until the first
        // call to 'advanceToNextNode' after the reader is opened.

    virtual int lookupAttribute(baexml_ElementAttribute *attribute,
                                int                      index) const;
        // Find the attribute at the specified 'index' in the current node, and
        // fill in the specified 'attribute' structure.  Return 0 on success, 1
        // if no attribute is found at the 'index', and an a negative value
        // otherwise.  The strings that were filled into the 'attribute'
        // structure are invalid upon the next 'advanceToNextNode' or 'close'
        // is called.

    virtual int lookupAttribute(baexml_ElementAttribute *attribute,
                                const char              *qname) const;
        // Find the attribute with the specified 'qname' (qualified name) in
        // the current node, and fill in the specified 'attribute' structure.
        // Return 0 on success, 1 if there is no attribute found with 'qname',
        // and a negative value otherwise.  The strings that were filled into
        // the 'attribute' structure are invalid upon the next
        // 'advanceToNextNode' or 'close' is called.

    virtual int
    lookupAttribute(baexml_ElementAttribute  *attribute,
                    const char               *localName,
                    const char               *namespaceUri) const;
        // Find the attribute with the specified 'localName' and specified
        // 'namespaceUri' in the current node, and fill in the specified
        // 'attribute' structure.  Return 0 on success, 1 if there is no
        // attribute found with 'localName' and 'namespaceUri', and a negative
        // value otherwise.  If 'namespaceUri' == 0 or a blank string is
        // passed, then the document's default namespace will be used.  The
        // strings that were filled into the 'attribute' structure are invalid
        // upon the next 'advanceToNextNode' or 'close' is called.

    virtual int
    lookupAttribute(baexml_ElementAttribute *attribute,
                    const char              *localName,
                    int                      namespaceId) const ;
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

    virtual const baexml_ErrorInfo& errorInfo() const;
        // Return a reference to the non-modifiable error information for this
        // reader.  The returned value becomes invalid when 'close' is
        // called or the reader is destroyed.

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

    virtual baexml_PrefixStack *prefixStack() const;
        // Return a pointer to the modifiable prefix stack that is used by this
        // reader to manage namespace prefixes or 0 if namespace support is
        // disabled.  The behavior is undefined if the returned prefix stack is
        // augmented in any way after calling 'open' and before calling
        // 'close'.

    virtual NodeType nodeType() const;
        // Return the node type of the current node if the reader 'isOpen' and
        // has not encounter an error and 'baexml_Reader::NONE' otherwise.

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
    // SPECIFIC FOR baexml_MiniReader
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
//====================================================
//   INLINE FUNCTIONS
//====================================================
inline baexml_MiniReader::Node &
baexml_MiniReader::currentNode()
{
    return  d_currentNode;
}

inline const baexml_MiniReader::Node &
baexml_MiniReader::currentNode() const
{
    return  d_currentNode;
}

inline int
baexml_MiniReader::peekChar()
{
    if (d_scanPtr >= d_endPtr) {
        if (readInput() == 0){
            return 0;
        }
    }

    return *d_scanPtr;
}

inline int
baexml_MiniReader::getChar()
{
    if (d_scanPtr >= d_endPtr) {
        if (readInput() == 0){
            return 0;
        }
    }
    return *d_scanPtr++;
}

inline bool
baexml_MiniReader::checkForNewLine()
{
    if (*d_scanPtr == '\n') {
        ++d_lineNum;
        d_linePtr = d_scanPtr + 1;
        return true;
    }
    return false;
}

inline int
baexml_MiniReader::getCharAndSet(char ch)
{
    //checkForNewLine();   // modify line, column

    int rc = peekChar();   // get current char

    if (rc != 0) {
        checkForNewLine();
        *d_scanPtr++ = ch;  // replace, advance position
    }
    return rc;
}

inline const char*
baexml_MiniReader::rebasePointer(const char* ptr,
                                 const char* newBase)
{
    if (ptr && ptr >= d_markPtr && ptr <= d_endPtr) {
        return newBase + (ptr-d_markPtr);
    }
    return ptr;
}

inline int
baexml_MiniReader::getCurrentPosition() const
{
    return d_streamOffset + (d_scanPtr - d_startPtr);
}

inline int
baexml_MiniReader::nodeStartPosition() const
{
    return  currentNode().d_startPos;
}

inline int
baexml_MiniReader::nodeEndPosition() const
{
    return  currentNode().d_endPos;
}

} // namespace BloombergLP

#endif // INCLUDED_BAEXML_MINIREADER
// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2006
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
