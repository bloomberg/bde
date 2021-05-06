// balxml_utf8readerwrapper.h                                         -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BALXML_UTF8READERWRAPPER
#define INCLUDED_BALXML_UTF8READERWRAPPER

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide wrapper for 'Reader' to check input UTF-8 validity.
//
//@CLASSES:
//   balxml::Utf8ReaderWrapper: Wrap a 'Reader', check UTF-8 input.
//
//@SEE_ALSO: balxml_reader
//           balxml_errorinfo
//           bdlde_utf8streambufinputwrapper
//
//@DESCRIPTION: This component supplies a mechanism,
// 'balxml::Utf8ReaderWrapper', which holds another object of type
// 'balxml::Reader' and forwards operations to the held object.  The held
// object is to operate on a 'bsl::streambuf', which is in fact a
// 'bdlde::Utf8CheckingInStreamBufWrapper' contained in the object, which holds
// another 'bsl::streambuf' and forward actions to that held 'bsl::streambuf'.
//
// The 'bdlde_Utf8StreamBufInputWrapper' detects invalid UTF-8.  If the input
// contains nothing but valid UTF-8, the 'bdlde_Utf8StreamBufInputWrapper'
// simply forwards all operations to the 'bsl::streambuf' it holds, and the
// wrapper has no influence on behavior.
//
// Similarly, if the input contains nothing but valid UTF-8, the reader wrapper
// simply forwards all operations to the held 'Reader' and has no influence on
// behavior.
//
// If invalid UTF-8 occurs in the input, 'errorInfo().message()' will reflect
// the nature of the UTF-8 error.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Routine Parsing:
/// - - - - - - - - - - - - -
// Utility function to skip past white space.
//..
//  int advancePastWhiteSpace(balxml::Reader& reader)
//  {
//      static const char whiteSpace[] = "\n\r\t ";
//      const char *value = 0;
//      int         type = 0;
//      int         rc = 0;
//
//      do {
//          rc    = reader.advanceToNextNode();
//          value = reader.nodeValue();
//          type  = reader.nodeType();
//      } while ((0 == rc && type == balxml::Reader::e_NODE_TYPE_WHITESPACE) ||
//               (type == balxml::Reader::e_NODE_TYPE_TEXT &&
//                bsl::strlen(value) == bsl::strspn(value, whiteSpace)));
//
//      assert( reader.nodeType() != balxml::Reader::e_NODE_TYPE_WHITESPACE);
//
//      return rc;
//  }
//..
// Then, in 'main', we parse an XML string using the UTF-8 reader wrapper:
//
// The following string describes xml for a very simple user directory.  The
// top level element contains one xml namespace attribute, with one embedded
// entry describing a user.  The person's name contains some non-ascii UTF-8.
//..
//  static const char TEST_XML_STRING[] =
//     "<?xml version='1.0' encoding='UTF-8'?>\n"
//     "<directory-entry xmlns:dir='http://bloomberg.com/schemas/directory'>\n"
//     "    <name>John Smith\xe7\x8f\x8f</name>\n"
//     "    <phone dir:phonetype='cell'>212-318-2000</phone>\n"
//     "    <address/>\n"
//     "</directory-entry>\n";
//..
// In order to read the XML, we first need to construct a
// 'balxml::NamespaceRegistry' object, a 'balxml::PrefixStack' object, and a
// 'Utf8ReaderWrapper' object.
//..
//  balxml::NamespaceRegistry namespaces;
//  balxml::PrefixStack prefixStack(&namespaces);
//  balxml::MiniReader miniReader;
//  balxml::Utf8ReaderWrapper reader(&miniReader);
//
//  assert(!reader.isOpen());
//..
// The reader uses a 'balxml::PrefixStack' to manage namespace prefixes so we
// need to set it before we call open.
//..
//  reader.setPrefixStack(&prefixStack);
//  assert(reader.prefixStack());
//  assert(reader.prefixStack() == &prefixStack);
//..
// Now we call the 'open' method to setup the reader for parsing using the data
// contained in the in the XML string.
//..
//  reader.open(TEST_XML_STRING, sizeof(TEST_XML_STRING) -1, 0, "UTF-8");
//..
// Confirm that the 'bdem::Reader' has opened properly
//..
//  assert( reader.isOpen());
//  assert(!bsl::strncmp(reader.documentEncoding(), "UTF-8", 5));
//  assert( reader.nodeType() == balxml::Reader::e_NODE_TYPE_NONE);
//  assert(!reader.nodeName());
//  assert(!reader.nodeHasValue());
//  assert(!reader.nodeValue());
//  assert(!reader.nodeDepth());
//  assert(!reader.numAttributes());
//  assert(!reader.isEmptyElement());
//..
// Advance through all the nodes and assert all information contained at each
// node is correct.
//
// Assert the next node's document type is xml.
//..
//  int rc = advancePastWhiteSpace(reader);
//  assert( 0 == rc);
//  assert( reader.nodeType() ==
//                            balxml::Reader::e_NODE_TYPE_XML_DECLARATION);
//  assert(!bsl::strcmp(reader.nodeName(), "xml"));
//  assert( reader.nodeHasValue());
//  assert(!bsl::strcmp(reader.nodeValue(), "version='1.0' encoding='UTF-8'"));
//  assert( reader.nodeDepth() == 1);
//  assert(!reader.numAttributes());
//  assert(!reader.isEmptyElement());
//  assert( 0 == rc);
//  assert( reader.nodeDepth() == 1);
//..
// Advance to the top level element, which has one attribute, the xml
// namespace.  Assert the namespace information has been added correctly to the
// prefix stack.
//..
//  rc = advancePastWhiteSpace(reader);
//  assert( 0 == rc);
//  assert( reader.nodeType() == balxml::Reader::e_NODE_TYPE_ELEMENT);
//  assert(!bsl::strcmp(reader.nodeName(), "directory-entry"));
//  assert(!reader.nodeHasValue());
//  assert( reader.nodeDepth() == 1);
//  assert( reader.numAttributes() == 1);
//  assert(!reader.isEmptyElement());
//
//  assert(!bsl::strcmp(prefixStack.lookupNamespacePrefix("dir"), "dir"));
//  assert(prefixStack.lookupNamespaceId("dir") == 0);
//  assert(!bsl::strcmp(prefixStack.lookupNamespaceUri("dir"),
//                      "http://bloomberg.com/schemas/directory"));
//..
// The XML being read contains one entry describing a user, advance the users
// name name and assert all information can be read correctly.
//..
//  rc = advancePastWhiteSpace(reader);
//  assert( 0 == rc);
//  assert( reader.nodeType() == balxml::Reader::e_NODE_TYPE_ELEMENT);
//  assert(!bsl::strcmp(reader.nodeName(), "name"));
//  assert(!reader.nodeHasValue());
//  assert( reader.nodeDepth() == 2);
//  assert( reader.numAttributes() == 0);
//  assert(!reader.isEmptyElement());
//
//  rc = reader.advanceToNextNode();
//  assert( 0 == rc);
//  assert( reader.nodeType() == balxml::Reader::e_NODE_TYPE_TEXT);
//  assert( reader.nodeHasValue());
//  assert(!bsl::strcmp(reader.nodeValue(), "John Smith\xe7\x8f\x8f"));
//  assert( reader.nodeDepth() == 3);
//  assert( reader.numAttributes() == 0);
//  assert(!reader.isEmptyElement());
//
//  rc = reader.advanceToNextNode();
//  assert( 0 == rc);
//  assert( reader.nodeType() == balxml::Reader::e_NODE_TYPE_END_ELEMENT);
//  assert(!bsl::strcmp(reader.nodeName(), "name"));
//  assert(!reader.nodeHasValue());
//  assert( reader.nodeDepth() == 2);
//  assert( reader.numAttributes() == 0);
//  assert(!reader.isEmptyElement());
//..
// Advance to the user's phone number and assert all information can be read
// correctly.
//..
//  rc = advancePastWhiteSpace(reader);
//  assert( 0 == rc);
//  assert( reader.nodeType() == balxml::Reader::e_NODE_TYPE_ELEMENT);
//  assert(!bsl::strcmp(reader.nodeName(), "phone"));
//  assert(!reader.nodeHasValue());
//  assert( reader.nodeDepth() == 2);
//  assert( reader.numAttributes() == 1);
//  assert(!reader.isEmptyElement());
//..
// The phone node has one attribute, look it up and assert the
// 'balxml::ElementAttribute' contains valid information and that the prefix
// returns the correct namespace URI from the prefix stack.
//..
//  balxml::ElementAttribute elemAttr;
//
//  rc = reader.lookupAttribute(&elemAttr, 0);
//  assert( 0 == rc);
//  assert(!elemAttr.isNull());
//  assert(!bsl::strcmp(elemAttr.qualifiedName(), "dir:phonetype"));
//  assert(!bsl::strcmp(elemAttr.value(), "cell"));
//  assert(!bsl::strcmp(elemAttr.prefix(), "dir"));
//  assert(!bsl::strcmp(elemAttr.localName(), "phonetype"));
//  assert(!bsl::strcmp(elemAttr.namespaceUri(),
//                      "http://bloomberg.com/schemas/directory"));
//  assert( elemAttr.namespaceId() == 0);
//
//  assert(!bsl::strcmp(prefixStack.lookupNamespaceUri(elemAttr.prefix()),
//                      elemAttr.namespaceUri()));
//
//  rc = advancePastWhiteSpace(reader);
//  assert( 0 == rc);
//  assert( reader.nodeType() == balxml::Reader::e_NODE_TYPE_TEXT);
//  assert( reader.nodeHasValue());
//  assert(!bsl::strcmp(reader.nodeValue(), "212-318-2000"));
//  assert( reader.nodeDepth() == 3);
//  assert( reader.numAttributes() == 0);
//  assert(!reader.isEmptyElement());
//
//  rc = advancePastWhiteSpace(reader);
//  assert( 0 == rc);
//  assert( reader.nodeType() == balxml::Reader::e_NODE_TYPE_END_ELEMENT);
//  assert(!bsl::strcmp(reader.nodeName(), "phone"));
//  assert(!reader.nodeHasValue());
//  assert( reader.nodeDepth() == 2);
//  assert( reader.numAttributes() == 0);
//  assert(!reader.isEmptyElement());
//..
// Advance to the user's address and assert all information can be read
// correctly.
//..
//  rc = advancePastWhiteSpace(reader);
//  assert( 0 == rc);
//  assert( reader.nodeType() == balxml::Reader::e_NODE_TYPE_ELEMENT);
//  assert(!bsl::strcmp(reader.nodeName(), "address"));
//  assert(!reader.nodeHasValue());
//  assert( reader.nodeDepth() == 2);
//  assert( reader.numAttributes() == 0);
//  assert( reader.isEmptyElement());
//..
// Advance to the end element.
//..
//  rc = advancePastWhiteSpace(reader);
//  assert( 0 == rc);
//  assert( reader.nodeType() == balxml::Reader::e_NODE_TYPE_END_ELEMENT);
//  assert(!bsl::strcmp(reader.nodeName(), "directory-entry"));
//  assert(!reader.nodeHasValue());
//  assert( reader.nodeDepth() == 1);
//  assert( reader.numAttributes() == 0);
//  assert(!reader.isEmptyElement());
//..
// Close the reader.
//..
//  reader.close();
//  assert(!reader.isOpen());
//
//  return 0;
//..

#include <balscm_version.h>

#include <balxml_errorinfo.h>
#include <balxml_reader.h>

#include <bdlde_utf8checkinginstreambufwrapper.h>
#include <bdlsb_fixedmeminstreambuf.h>

#include <bslmf_nestedtraitdeclaration.h>
#include <bsls_keyword.h>

#include <bsl_cstddef.h> // for size_t
#include <bsl_functional.h>
#include <bsl_fstream.h>
#include <bsl_streambuf.h>

namespace BloombergLP  {
namespace balxml {

class ElementAttribute;
class ErrorInfo;
class PrefixStack;

                            // =======================
                            // class Utf8ReaderWrapper
                            // =======================

class Utf8ReaderWrapper : public Reader {
    // This class "has a" pointer to a held and wrapped 'Reader' object, and
    // operations on this object are passed to the held reader.  The held
    // reader is passed a 'Utf8CheckingInStreamBufWrapper', which holds and
    // wraps a normal 'streambuf'.  The 'Utf8CheckingInStreamBufWrapper' checks
    // input for invalid UTF-8, and if it detects any, makes the diagnosis of
    // the problem available through the 'errorInfo' accessor.

    // DATA
    bdlde::Utf8CheckingInStreamBufWrapper d_utf8StreamBuf;
    bdlsb::FixedMemInStreamBuf            d_fixedStreamBuf;
    bsl::ifstream                         d_stream;
    Reader *                              d_reader_p;
    ErrorInfo                             d_errorInfo;
    bool                                  d_useHeldErrorInfo;

  private:
    // NOT IMPLEMENTED
    Utf8ReaderWrapper(const Utf8ReaderWrapper&);
    Utf8ReaderWrapper& operator=(const Utf8ReaderWrapper&);

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(Utf8ReaderWrapper,
                                   bslma::UsesBslmaAllocator);

  private:
    // PRIVATE MANIPULATORS
    int doOpen(const char *url, const char *encoding);
        // Open the held reader with 'd_utf8StreamBuf', as well as the
        // specified 'url' and 'encoding'.  Note that all public 'open'
        // functions of this class prepare 'd_utf8StreamBuf' and then delegate
        // to this function as part of their implementation.

    Reader *heldReader();
        // Return a pointer providing modifiable access to the held 'Reader'.

    void reportUtf8Error(int utf8Rc);
        // Called when a UTF-8 error is encountered, to make 'd_errorInfo' into
        // a combination of 'heldReader()->errorInfo()' and the nature of the
        // UTF-8 error as reported by the specified 'utf8Rc'.  The behavior is
        // undefined unless 'utf8Rc < 0' and 'utf8Rc' is one of the values
        // enumerated by 'Utf8Util::ErrorStatus'.

    // PRIVATE ACCESSORS
    const Reader *heldReader() const;
        // Return a pointer providing non-modifiable access to the held
        // 'Reader'.

  public:
    // CREATORS
    explicit
    Utf8ReaderWrapper(Reader           *reader,
                      bslma::Allocator *basicAllocator = 0);
        // Create a 'Utf8ReaderWrapper' that holds the specified 'reader'.
        // Optionally specify a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.  The behavior is undefined unless 'reader' has never been
        // opened or closed.

    ~Utf8ReaderWrapper();
        // Close the held reader and destroy this object.

    // MANIPULATORS

                              // ** setup methods **

    void setOptions(unsigned int flags) BSLS_KEYWORD_OVERRIDE;
        // Set the options of the held reader to the flags in the specified
        // 'flags'.  The options for the reader are persistent, i.e., the
        // options are not reset by 'close'.  The behavior is undefined if this
        // method is called after calling 'open' and before calling 'close';
        // except that derived classes are permitted to specify valid behavior
        // for calling this function for specific arguments while the reader is
        // open.

    void setPrefixStack(PrefixStack *prefixes) BSLS_KEYWORD_OVERRIDE;
        // Set the prefix stack to the stack at the optionally specified
        // 'prefixes' address or disable prefix stack support if 'prefixes' is
        // null.  This stack is used to push and pop namespace prefixes as the
        // parse progresses, so that, at any point, the stack will reflect the
        // set of active prefixes for the current node.  It is legitimate to
        // pass a stack that already contains prefixes, these prefixes shall be
        // preserved when 'close' is called, i.e., the prefix stack shall be
        // returned to the stack depth it had when 'setPrefixStack' was called.
        // The behavior is undefined if this method is called after calling
        // 'open' and before calling 'close'.

    void setResolver(XmlResolverFunctor resolver) BSLS_KEYWORD_OVERRIDE;
        // Set the external XML resource resolver to the specified 'resolver'.
        // The XML resource resolver is used by the 'balxml_reader' to find and
        // open an external resources (See the 'XmlResolverFunctor' typedef for
        // more details).  The XML resource resolver remains valid; it is not
        // affected by a call to 'close' and should be available until the
        // reader is destroyed.  The behavior is undefined if this method is
        // called after calling 'open' and before calling 'close'.

                            // ** open/close methods **

    int open(const char *filename, const char *encoding = 0)
                                                         BSLS_KEYWORD_OVERRIDE;
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

    int open(const char  *buffer,
             bsl::size_t  size,
             const char  *url = 0,
             const char  *encoding = 0) BSLS_KEYWORD_OVERRIDE;
        // Set up the reader for parsing using the data contained in the
        // specified (XML) 'buffer' of the specified 'size', set the base URL
        // to the optionally specified 'url' and set the encoding value to the
        // optionally specified 'encoding' ("ASCII", "UTF-8", etc).  Return 0
        // on success and non-zero otherwise.  If 'url' is null or a blank
        // string is passed, then base URL will be empty.  The encoding passed
        // to 'Reader::open' will take effect only when there is no encoding
        // information in the original document, i.e., the encoding information
        // obtained from the (XML) 'buffer' trumps all.  If there is no
        // encoding provided within the document and 'encoding' is null or a
        // blank string is passed, then set the encoding to the default
        // "UTF-8".  It is an error to 'open' a reader that is already open.
        // Note that the reader will not be on a valid node until
        // 'advanceToNextNode' is called.

    int open(bsl::streambuf *stream,
             const char     *url = 0,
             const char     *encoding = 0) BSLS_KEYWORD_OVERRIDE;
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

    void close() BSLS_KEYWORD_OVERRIDE;
        // Close the reader.  Most, but not all state is reset.  Specifically,
        // the XML resource resolver and the prefix stack remain.  The prefix
        // stack shall be returned to the stack depth it had when
        // 'setPrefixStack' was called.  Call the method 'open' to reuse the
        // reader.  Note that 'close' invalidates all strings and data
        // structures obtained via 'Reader' accessors.  E.g., the pointer
        // returned from 'nodeName' for this node will not be valid once
        // 'close' is called.

                                // ** navigation method **

    int advanceToNextNode() BSLS_KEYWORD_OVERRIDE;
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

    // ACCESSORS
    bslma::Allocator *allocator() const;
        // Return the allocator used by this object to allocate memory.

    const char *documentEncoding() const BSLS_KEYWORD_OVERRIDE;
        // Return the document encoding or NULL on error.  The returned poiner
        // is owned by this object and must not be modified or deallocated by
        // the caller.  The returned pointer becomes invalid when 'close' is
        // called or the reader is destroyed.

    const ErrorInfo& errorInfo() const BSLS_KEYWORD_OVERRIDE;
        // Return a reference to the non-modifiable error information for this
        // reader.  The returned value becomes invalid when 'close' is called
        // or the reader is destroyed.

    int getColumnNumber() const BSLS_KEYWORD_OVERRIDE;
        // Return the current column number within the input stream.  The
        // current column number is the number of characters since the last
        // newline was read by the reader plus one, i.e., the first column of
        // each line is column number one.  Return 0 if not available.  Note
        // that a derived-class implementation is not required to count
        // columns and may just return 0.

    int getLineNumber() const BSLS_KEYWORD_OVERRIDE;
        // Return the current line number within the input stream.  The current
        // line is the last line for which the reader has not yet seen a
        // newline.  Lines are counted starting at one from the time a stream
        // is provided to 'open'.  Return 0 if not available.  Note that a
        // derived-class implementation is not required to count lines and may
        // just return 0.

    bool isEmptyElement() const BSLS_KEYWORD_OVERRIDE;
        // Return true if the current node is an element (i.e., node type is
        // 'BAEXML_NODE_TYPE_ELEMENT') that ends with '/>'; and false
        // otherwise.  Note that '<a/>' will be considered empty but '<a></a>'
        // will not.

    bool isOpen() const BSLS_KEYWORD_OVERRIDE;
        // Return true if 'open' was called successfully and 'close' has not
        // yet been called and false otherwise.

    int lookupAttribute(ElementAttribute *attribute,
                        int               index) const BSLS_KEYWORD_OVERRIDE;
        // Find the attribute at the specified 'index' in the current node, and
        // fill in the specified 'attribute' structure.  Return 0 on success, 1
        // if no attribute is found at the 'index', and an a negative value
        // otherwise.  The strings that were filled into the 'attribute'
        // structure are invalid upon the next 'advanceToNextNode' or 'close'
        // is called.

    int lookupAttribute(ElementAttribute *attribute,
                        const char       *qname) const BSLS_KEYWORD_OVERRIDE;
        // Find the attribute with the specified 'qname' (qualified name) in
        // the current node, and fill in the specified 'attribute' structure.
        // Return 0 on success, 1 if there is no attribute found with 'qname',
        // and a negative value otherwise.  The strings that were filled into
        // the 'attribute' structure are invalid upon the next
        // 'advanceToNextNode' or 'close' is called.

    int
    lookupAttribute(
                   ElementAttribute *attribute,
                   const char       *localName,
                   const char       *namespaceUri) const BSLS_KEYWORD_OVERRIDE;
        // Find the attribute with the specified 'localName' and specified
        // 'namespaceUri' in the current node, and fill in the specified
        // 'attribute' structure.  Return 0 on success, 1 if there is no
        // attribute found with 'localName' and 'namespaceUri', and a negative
        // value otherwise.  If 'namespaceUri' == 0 or a blank string is
        // passed, then the document's default namespace will be used.  The
        // strings that were filled into the 'attribute' structure are invalid
        // upon the next 'advanceToNextNode' or 'close' is called.

    int lookupAttribute(
                    ElementAttribute *attribute,
                    const char       *localName,
                    int               namespaceId) const BSLS_KEYWORD_OVERRIDE;
        // Find the attribute with the specified 'localName' and specified
        // 'namespaceId' in the current node, and fill in the specified
        // 'attribute' structure.  Return 0 on success, 1 if there is no
        // attribute found with 'localName' and 'namespaceId', and a negative
        // value otherwise.  If 'namespaceId' == -1, then the document's
        // default namespace will be used.  The strings that were filled into
        // the 'attribute' structure are invalid upon the next
        // 'advanceToNextNode' or 'close' is called.

    const char *nodeBaseUri() const BSLS_KEYWORD_OVERRIDE;
        // Return the base URI name of the current node if the current node has
        // a base URI and NULL otherwise.  The returned pointer is owned by
        // this object and must not be modified or deallocated by the caller.
        // The returned pointer becomes invalid upon the next
        // 'advanceToNextNode', when 'close' is called or the reader is
        // destroyed.

    int nodeDepth() const BSLS_KEYWORD_OVERRIDE;
        // Return the nesting depth of the current node in the XML document.
        // The root node has depth 0.

    const char *nodeLocalName() const BSLS_KEYWORD_OVERRIDE;
        // Return the local name of the current node if the current node has a
        // local name and NULL otherwise.  The returned pointer is owned by
        // this object and must not be modified or deallocated by the caller.
        // The returned pointer becomes invalid upon the next
        // 'advanceToNextNode', when 'close' is called or the reader is
        // destroyed.

    bool nodeHasValue() const BSLS_KEYWORD_OVERRIDE;
        // Return true if the current node has a value and false otherwise.

    const char *nodeName() const BSLS_KEYWORD_OVERRIDE;
        // Return the qualified name of the current node if the current node
        // has a name and NULL otherwise.  The returned pointer is owned by
        // this object and must not be modified or deallocated by the caller.
        // The returned pointer becomes invalid upon the next
        // 'advanceToNextNode', when 'close' is called or the reader is
        // destroyed.

    int nodeNamespaceId() const BSLS_KEYWORD_OVERRIDE;
        // Return the namespace ID of the current node if the current node has
        // a namespace id and a negative number otherwise.

    const char *nodeNamespaceUri() const BSLS_KEYWORD_OVERRIDE;
        // Return the namespace URI name of the current node if the current
        // node has a namespace URI and NULL otherwise.  The returned pointer
        // is owned by this object and must not be modified or deallocated by
        // the caller.  The returned pointer becomes invalid upon the next
        // 'advanceToNextNode', when 'close' is called or the reader is
        // destroyed.

    const char *nodePrefix() const BSLS_KEYWORD_OVERRIDE;
        // Return the prefix name of the current node if the correct node has a
        // prefix name and NULL otherwise.  The returned pointer is owned by
        // this object and must not be modified or deallocated by the caller.
        // The returned pointer becomes invalid upon the next
        // 'advanceToNextNode', when 'close' is called or the reader is
        // destroyed.

    NodeType nodeType() const BSLS_KEYWORD_OVERRIDE;
        // Return the node type of the current node if the reader 'isOpen' and
        // has not encounter an error and 'Reader::NONE' otherwise.

    const char *nodeValue() const BSLS_KEYWORD_OVERRIDE;
        // Return the value of the current node if the current node has a value
        // and NULL otherwise.  The returned pointer is owned by this object
        // and must not be modified or deallocated by the caller.  The returned
        // pointer becomes invalid upon the next 'advanceToNextNode', when
        // 'close' is called or the reader is destroyed.

    int numAttributes() const BSLS_KEYWORD_OVERRIDE;
        // Return the number of attributes for the current node if that node
        // has attributes and 0 otherwise.

    unsigned int options() const BSLS_KEYWORD_OVERRIDE;
        // Return the option flags.

    PrefixStack *prefixStack() const BSLS_KEYWORD_OVERRIDE;
        // Return a pointer to the modifiable prefix stack that is used by this
        // reader to manage namespace prefixes or 0 if namespace support is
        // disabled.  The behavior is undefined if the returned prefix stack is
        // augmented in any way after calling 'open' and before calling
        // 'close'.

    XmlResolverFunctor resolver() const BSLS_KEYWORD_OVERRIDE;
        // Return the external XML resource resolver.
};

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

                                // ------------
                                // class Reader
                                // ------------

// PRIVATE MANIPULATORS
inline
Reader *Utf8ReaderWrapper::heldReader()
{
    return d_reader_p;
}

// PRIVATE ACCESSORS
inline
const Reader *Utf8ReaderWrapper::heldReader() const
{
    return d_reader_p;
}

}  // close package namespace
}  // close enterprise namespace

#endif // INCLUDED_BALXML_UTF8READERWRAPPER

// ----------------------------------------------------------------------------
// Copyright 2020 Bloomberg Finance L.P.
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
