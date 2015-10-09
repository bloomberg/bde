// balxml_reader.h                                                    -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BALXML_READER
#define INCLUDED_BALXML_READER

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide common reader protocol for parsing XML documents.
//
//@CLASSES:
//   balxml::Reader: protocol for fast, forward-only access to XML data stream
//
//@SEE_ALSO: balxml_validatingreader,
//           balxml_elementattribute,
//           balxml::ErrorInfo,
//           balxml_prefixstack,
//           balxml_namespaceregistry
//
//@DESCRIPTION: This component supplies an abstract class, 'balxml::Reader'
// that defines an interface for accessing a forward-only, read-only stream of
// XML data.  The 'balxml::Reader' interface is somewhat similar to Microsoft
// XmlReader interface, which provides a simpler and more flexible programming
// model than the quasi-standard SAX/SAX2 model and a (potentially) more
// memory-efficient programming model than DOM.  Access to the data is done in
// a cursor-like fashion, going forward on the document stream and stopping at
// each node along the way.  A "node" is an XML syntactic construct such as
// the start of an element, the end of an element, element text, etc..  (See
// the 'balxml::Reader::NodeType' enumeration for a complete list.)  Note that,
// unlike the Microsoft interace, an element attribute is *not* considered a
// node in this interface, but is rather considered an attribute of a
// start-element node.  In the documentation below the "current node" refers
// to the node on which the reader is currently positioned.  The client code
// advances through all of the nodes in the XML document by calling the
// 'advanceToNextNode' function repeatedly and processing each node in the
// order it appears in the xml document.
//
// 'balxml::Reader' supplies accessors that query a node's attributes, such as
// the node's type, name, value, element attributes, etc..  Note that each call
// to 'advanceToNextNode' invalidates strings and data structures returned when
// the 'balxml::Reader' accessors were call for the prior node.  E.g., the
// pointer returned from 'nodeName' for one node will *not* be valid once the
// reader has advanced to the next node.  The fact that this interface provides
// so little prior context gives the derived-class implementations the
// potential to be very efficient in their use of memory.
//
// Any derived class must adhere to the class-level and function-level contract
// documented in this component.  Note that an object of a derived class
// implementation must be reusable such that, after parsing one document, the
// reader can be closed and re-opened to parse another document.
//
///Node Type
///---------
// An enumeration value that identifies a node as a specific XML construct,
// e.g., ELEMENT, END_ELEMENT, TEXT, CDATA, etc.  (See the
// 'balxml::Reader::NodeType' enumeration for a complete list.)
//
///Qualified and local names:
///--------------------------
// XML documents may contain some qualified names.  These are names with a
// prefix (optional) and a local name, separated by a colon.  (The colon is
// present only if the prefix is present.)  The prefix is a (typically short)
// word that is associated with a namespace URI via a namespace declaration.
// The local name specifies an entity within the specified namespace or, if no
// prefix is given, within the default namespace.  For each qualified name, the
// 'balxml::Reader' interface provides access to the entire qualified name and
// separate access to the prefix, the local name, the namespace URI, and the
// namespace ID.
//
///Base URI
///--------
// Networked XML documents may comprise chunks of data aggregated using various
// W3C standard inclusion mechanisms and can contain nodes that come from
// different places.  DTD entities are an example of this.  The base URI tells
// you where a node comes from (see http://www.w3.org/TR/xmlbase/).  The base
// URI of an element is:
//
//: 1 The base URI specified by an xml:base attribute on the element, if one
//:   exists, otherwise
//:
//: 2 The base URI of the element's parent element within the document or
//:   external entity, if one exists, otherwise
//:
//: 3 The base URI of the document entity or external entity containing the
//:   element.
//
// If there is no base URI for a node being returned (for example, it was
// parsed from an in-memory string), then 'nodeBaseUri' return an empty string.
//
///Encoding
///--------
// A XML document or any external reference (such as expanding an entity in a
// DTD file or reading a schema file) will be encoded, for example, in "ASCII,"
// "UTF-8," or "UTF-16".  The document can also contain self-describing
// information as to which encoding was used when the document was created.
// Note that the encoding returned from the 'documentEncoding' method can
// differ from the encoding of the strings returned from the 'balxml::Reader'
// accessors; all strings returned by these accessors are UTF-8 regardless of
// the encoding used in the original document.
//
// If encoding information is not provided in the document, the
// 'balxml::Reader::open' method allows clients to specify an encoding to use.
// The encoding passed to 'balxml::Reader::open' will take effect only when
// there is no encoding information in the original document, i.e., the
// encoding information obtained from the original document trumps all.  If
// there is no encoding provided within the document and the client has not
// provided one via the 'balxml::Reader::open' method, then a derived-class
// implementation should set the encoding to UTF-8.  (See the
// 'balxml::Reader::open' method for more details.)
//
///Thread Safety
///-------------
// This component does not provide any functions that present a thread safety
// issue, since the 'balxml::Reader' class is abstract and cannot be
// instantiated.  There is no guarantee that any specific derived class will
// provide a thread-safe implementation.
//
///Usage
///-----
// In this example, we will create a simple parser that prints out the contents
// of each node in an XML document.
//..
//  #include <balxml_reader.h>
//  #include <balxml_errorinfo.h>
//  #include <iostream>
//..
// The 'processNode' function prints the node's contents:
//..
//  void  processNode(balxml::Reader *reader)
//  {
//      balxml::Reader::NodeType nodeType = reader->nodeType();
//      const char  *name  = reader->nodeName();
//      const char  *value = reader->nodeValue();
//
//      if (!name)    name  = "";
//      if (!value)   value = "";
//
//      bsl::cout << "Node: type="
//                << nodeType
//                << "("
//                << balxml::Reader::nodeTypeAsString(nodeType)
//                << ") name=" << name
//                << " value=" << value
//                << bsl::endl;
//..
// If the node has attributes, then iterate through them:
//..
//
//      for (int i = 0; i < reader->numAttributes(); ++i) {
//          ElementAttribute attr;
//          reader->lookupAttribute(&attr, i);
//
//          name  = attr.name();
//          value = attr.value();
//
//          if (!name)    name  = "";
//          if (!value)   value = "";
//
//          bsl::cout << "Attribute: name=" << name
//                    << " value=" << value
//                    << bsl::endl;
//      }
//  }
//..
// The main parser function must construct a namespace registry and a prefix
// stack before opening the data stream:
//..
//  int parse(balxml::Reader *reader, const char* xmlData)
//  {
//      balxml::NamespaceRegistry namespaces;
//      balxml::PrefixStack prefixes(&namespaces);
//      reader->setPrefixStack(&prefixes);
//      reader->open(xmlData, bsl::strlen(xmlData));
//..
// Then it simply traverses each node and calls 'processNode':
//..
//      int rc;
//      while (! (rc = reader->advanceToNextNode()) {
//          processNode(reader);
//      }
//..
// If an error occurred, display the error:
//..
//      if (rc < 0) {
//          bsl::cout << reader->errorInfo();
//          return -rc;
//      }
//
//      return 0;
//  }
//..

#ifndef INCLUDED_BALSCM_VERSION
#include <balscm_version.h>
#endif

#ifndef INCLUDED_BALXML_ERRORINFO
#include <balxml_errorinfo.h>
#endif

#ifndef INCLUDED_BSLMA_MANAGEDPTR
#include <bslma_managedptr.h>
#endif

#ifndef INCLUDED_BSL_FUNCTIONAL
#include <bsl_functional.h>
#endif

#ifndef INCLUDED_BSL_OSTREAM
#include <bsl_ostream.h>
#endif

#ifndef INCLUDED_BSL_STREAMBUF
#include <bsl_streambuf.h>
#endif

#ifndef INCLUDED_BSL_CSTDDEF
#include <bsl_cstddef.h> // for size_t
#endif

namespace BloombergLP  {


namespace balxml {class ElementAttribute;
class PrefixStack;

                                // ============
                                // class Reader
                                // ============

class Reader {
    // This abstract class defines an interface for fast, forward-only access
    // to XML data.  An object belonging to a derived-class implementation of
    // this protocol is required to be re-usable, such that a new XML document
    // can be parsed using the same reader object by calling 'close' followed
    // by another 'open'.

  public:
    // PUBLIC TYPES
    enum NodeType {
        // Node types, returned by 'nodeType' method, which represent a XML
        // syntactic construct within a document.  Note: Not every
        // implementation of 'Reader' will distinguish among all of the node
        // types.
        e_NODE_TYPE_NONE                   = 0,
        e_NODE_TYPE_ELEMENT                = 1,
        e_NODE_TYPE_TEXT                   = 2,
        e_NODE_TYPE_CDATA                  = 3,
        e_NODE_TYPE_ENTITY_REFERENCE       = 4,
        e_NODE_TYPE_ENTITY                 = 5,
        e_NODE_TYPE_PROCESSING_INSTRUCTION = 6,
        e_NODE_TYPE_COMMENT                = 7,
        e_NODE_TYPE_DOCUMENT               = 8,
        e_NODE_TYPE_DOCUMENT_TYPE          = 9,
        e_NODE_TYPE_DOCUMENT_FRAGMENT      = 10,
        e_NODE_TYPE_NOTATION               = 11,
        e_NODE_TYPE_WHITESPACE             = 12,
        e_NODE_TYPE_SIGNIFICANT_WHITESPACE = 13,
        e_NODE_TYPE_END_ELEMENT            = 14,
        e_NODE_TYPE_END_ENTITY             = 15,
        e_NODE_TYPE_XML_DECLARATION        = 16
    };

    typedef bslma::ManagedPtr<bsl::streambuf> StreamBufPtr;

    typedef bsl::function<StreamBufPtr(const char *location,
                                       const char *namespaceUri)>
                                                            XmlResolverFunctor;
        // Type for a user supplied functor that finds and opens an external
        // resource for the specified 'location' and/or 'namespaceUri' and
        // returns that resource as a managed pointer to a stream.  The
        // 'location' argument specifies the location of the external resource
        // and is typically a filename or a URI, depending on the context.  The
        // 'namespaceUri' argument always refers to the XML namespace of the
        // entity to be resolved.  A conforming functor returns an empty
        // managed pointer if it cannot resolve the resource.  For example, the
        // reader may use a resolver to open an external entity, even if the
        // reader does not do validation (see definition of '<!ENTITY>' in the
        // XML standard).  Note that either argument can be NULL in situations
        // where its value is not needed or can be computed from the other
        // argument.

    // CLASS METHODS
    static const char *nodeTypeAsString(NodeType nodeType);
        // Return a string representation for the specified 'nodeType' code or
        // "(* UNKNOWN NODE TYPE *)" if 'nodeType' is not one of the values
        // enumerated in 'NodeType'.

    // PUBLIC CREATORS
    virtual ~Reader(void);
        // Destroy this object.  The implementation for this pure abstract base
        // class does nothing.

    // NON-VIRTUAL ACCESSORS (implemented in this base class)
    void dumpNode(bsl::ostream& os) const;
        // Print the information about the current node to the specified output
        // 'os' stream.

    bool isFatalError() const;
        // Return 'true' if the derived object encountered a fatal error.  This
        // method is equivalent to a call to 'errorInfo().isFatalError();'

    bool isError() const;
        // Return 'true' if the derived object encountered a error.  This
        // method is equivalent to a call to 'errorInfo().isError();'

    bool isWarning() const;
        // Return 'true' if the derived object encountered a warning.  This
        // method is equivalent to a call to 'errorInfo().isWarning();'

    // MANIPULATORS - SETUP METHODS
    virtual void setPrefixStack(PrefixStack *prefixes) = 0;
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

    virtual void setResolver(XmlResolverFunctor resolver) = 0;
        // Set the external XML resource resolver to the specified 'resolver'.
        // The XML resource resolver is used by the 'balxml_reader' to find and
        // open an external resources (See the 'XmlResolverFunctor' typedef for
        // more details).  The XML resource resolver remains valid; it is not
        // effected by a call to 'close' and should be available until the
        // reader is destroyed.  The behavior is undefined if this method is
        // called after calling 'open' and before calling 'close'.

    // MANIPULATORS - OPEN/CLOSE AND NAVIGATION METHODS
    virtual int open(const char *filename, const char *encoding = 0) = 0;
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
                     const char  *encoding = 0) = 0;
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

    virtual int open(bsl::streambuf *stream,
                     const char     *url = 0,
                     const char     *encoding = 0) = 0;
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

    virtual void close() = 0;
        // Close the reader.  Most, but not all state is reset.  Specifically,
        // the XML resource resolver and the prefix stack remain.  The prefix
        // stack shall be returned to the stack depth it had when
        // 'setPrefixStack' was called.  Call the method 'open' to reuse the
        // reader.  Note that 'close' invalidates all strings and data
        // structures obtained via 'Reader' accessors.  E.g., the pointer
        // returned from 'nodeName' for this node will not be valid once
        // 'close' is called.

    virtual int advanceToNextNode() = 0;
        // Move to the next node in the data steam created by 'open' thus
        // allowing the node's properties to be queried via the 'Reader'
        // accessors.  Return 0 on successful read, 1 if there are no more
        // nodes to read, and a negative number otherwise.  Note that each call
        // to 'advanceToNextNode' invalidates strings and data structures
        // returned when 'Reader' accessors where call for the "prior node".
        // E.g., the pointer returned from 'nodeName' for this node will not be
        // valid once 'advanceToNextNode' is called.  Note that the reader will
        // not be on a valid node until the first call to 'advanceToNextNode'
        // after the reader is opened.  TBD: add comment about insignificant
        // white space.

    virtual int lookupAttribute(ElementAttribute *attribute,
                                int               index) const = 0;
        // Find the attribute at the specified 'index' in the current node, and
        // fill in the specified 'attribute' structure.  Return 0 on success, 1
        // if no attribute is found at the 'index', and an a negative value
        // otherwise.  The strings that were filled into the 'attribute'
        // structure are invalid upon the next 'advanceToNextNode' or 'close'
        // is called.

    virtual int lookupAttribute(ElementAttribute *attribute,
                                const char       *qname) const = 0;
        // Find the attribute with the specified 'qname' (qualified name) in
        // the current node, and fill in the specified 'attribute' structure.
        // Return 0 on success, 1 if there is no attribute found with 'qname',
        // and a negative value otherwise.  The strings that were filled into
        // the 'attribute' structure are invalid upon the next
        // 'advanceToNextNode' or 'close' is called.

    virtual int
    lookupAttribute(ElementAttribute *attribute,
                    const char       *localName,
                    const char       *namespaceUri) const = 0;
        // Find the attribute with the specified 'localName' and specified
        // 'namespaceUri' in the current node, and fill in the specified
        // 'attribute' structure.  Return 0 on success, 1 if there is no
        // attribute found with 'localName' and 'namespaceUri', and a negative
        // value otherwise.  If 'namespaceUri' == 0 or a blank string is
        // passed, then the document's default namespace will be used.  The
        // strings that were filled into the 'attribute' structure are invalid
        // upon the next 'advanceToNextNode' or 'close' is called.

    virtual int
    lookupAttribute(ElementAttribute *attribute,
                    const char       *localName,
                    int               namespaceId) const  = 0;
        // Find the attribute with the specified 'localName' and specified
        // 'namespaceId' in the current node, and fill in the specified
        // 'attribute' structure.  Return 0 on success, 1 if there is no
        // attribute found with 'localName' and 'namespaceId', and a negative
        // value otherwise.  If 'namespaceId' == -1, then the document's
        // default namespace will be used.  The strings that were filled into
        // the 'attribute' structure are invalid upon the next
        // 'advanceToNextNode' or 'close' is called.

    virtual void setOptions(unsigned int flags) = 0;
        // Set the options to the flags in the specified 'flags'.  The options
        // for the reader are persistent, i.e., the options are not reset by
        // 'close'.  The behavior is undefined if this method is called after
        // calling 'open' and before calling 'close'; except that derived
        // classes are permitted to specify valid behavior for calling this
        // function for specific arguments while the reader is open.

    // ACCESSORS
    virtual const char *documentEncoding() const = 0;
        // Return the document encoding or NULL on error.  The returned pointer
        // is owned by this object and must not be modified or deallocated by
        // the caller.  The returned pointer becomes invalid when 'close' is
        // called or the reader is destroyed.

    virtual XmlResolverFunctor resolver() const = 0;
        // Return the external XML resource resolver.

    virtual bool isOpen() const = 0;
        // Return true if 'open' was called successfully and 'close' has not
        // yet been called and false otherwise.

    virtual const ErrorInfo& errorInfo() const = 0;
        // Return a reference to the non-modifiable error information for this
        // reader.  The returned value becomes invalid when 'close' is called
        // or the reader is destroyed.

    virtual int getLineNumber() const = 0;
        // Return the current line number within the input stream.  The current
        // line is the last line for which the reader has not yet seen a
        // newline.  Lines are counted starting at one from the time a stream
        // is provided to 'open'.  Return 0 if not available.  Note that a
        // derived-class implementation is not required to count lines and may
        // just return 0.

    virtual int getColumnNumber() const = 0;
        // Return the current column number within the input stream.  The
        // current column number is the number of characters since the last
        // newline was read by the reader plus one, i.e., the first column of
        // each line is column number one.  Return 0 if not available.  Note
        // that a derived-class implementation is not required to count
        // columns and may just return 0.

    virtual PrefixStack *prefixStack() const = 0;
        // Return a pointer to the modifiable prefix stack that is used by this
        // reader to manage namespace prefixes or 0 if namespace support is
        // disabled.  The behavior is undefined if the returned prefix stack is
        // augmented in any way after calling 'open' and before calling
        // 'close'.

    virtual NodeType nodeType() const = 0;
        // Return the node type of the current node if the reader 'isOpen' and
        // has not encounter an error and 'Reader::NONE' otherwise.

    virtual const char *nodeName() const = 0;
        // Return the qualified name of the current node if the current node
        // has a name and NULL otherwise.  The returned pointer is owned by
        // this object and must not be modified or deallocated by the caller.
        // The returned pointer becomes invalid upon the next
        // 'advanceToNextNode', when 'close' is called or the reader is
        // destroyed.

    virtual const char *nodeLocalName() const = 0;
        // Return the local name of the current node if the current node has a
        // local name and NULL otherwise.  The returned pointer is owned by
        // this object and must not be modified or deallocated by the caller.
        // The returned pointer becomes invalid upon the next
        // 'advanceToNextNode', when 'close' is called or the reader is
        // destroyed.

    virtual const char *nodePrefix() const = 0;
        // Return the prefix name of the current node if the correct node has a
        // prefix name and NULL otherwise.  The returned pointer is owned by
        // this object and must not be modified or deallocated by the caller.
        // The returned pointer becomes invalid upon the next
        // 'advanceToNextNode', when 'close' is called or the reader is
        // destroyed.

    virtual int nodeNamespaceId() const = 0;
        // Return the namespace ID of the current node if the current node has
        // a namespace id and a negative number otherwise.

    virtual const char *nodeNamespaceUri() const = 0;
        // Return the namespace URI name of the current node if the current
        // node has a namespace URI and NULL otherwise.  The returned pointer
        // is owned by this object and must not be modified or deallocated by
        // the caller.  The returned pointer becomes invalid upon the next
        // 'advanceToNextNode', when 'close' is called or the reader is
        // destroyed.

    virtual const char *nodeBaseUri() const = 0;
        // Return the base URI name of the current node if the current node has
        // a base URI and NULL otherwise.  The returned pointer is owned by
        // this object and must not be modified or deallocated by the caller.
        // The returned pointer becomes invalid upon the next
        // 'advanceToNextNode', when 'close' is called or the reader is
        // destroyed.

    virtual bool nodeHasValue() const = 0;
        // Return true if the current node has a value and false otherwise.

    virtual const char *nodeValue() const = 0;
        // Return the value of the current node if the current node has a value
        // and NULL otherwise.  The returned pointer is owned by this object
        // and must not be modified or deallocated by the caller.  The returned
        // pointer becomes invalid upon the next 'advanceToNextNode', when
        // 'close' is called or the reader is destroyed.

    virtual int nodeDepth() const = 0;
        // Return the nesting depth of the current node in the XML document.
        // The root node has depth 0.

    virtual int numAttributes() const = 0;
        // Return the number of attributes for the current node if that node
        // has attributes and 0 otherwise.

    virtual bool isEmptyElement() const = 0;
        // Return true if the current node is an element (i.e., node type is
        // 'BAEXML_NODE_TYPE_ELEMENT') that ends with '/>'; and false
        // otherwise.  Note that '<a/>' will be considered empty but '<a></a>'
        // will not.

    virtual unsigned int options() const = 0;
        // Return the option flags.
};

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

inline
bsl::ostream& operator<<(bsl::ostream& stream, Reader::NodeType value)
    // Print the specified node type 'value' to the specified 'stream' in
    // human-readable form and return a modifiable reference to 'stream'.
{
    return stream << Reader::nodeTypeAsString(value);
}

}  // close package namespace

// FREE OPERATORS

namespace balxml {
inline bool
Reader::isWarning() const
{
    return errorInfo().isWarning();
}

inline bool
Reader::isError() const
{
    return errorInfo().isError();
}

inline bool
Reader::isFatalError() const
{
    return errorInfo().isFatalError();
}
}  // close package namespace

}  // close enterprise namespace

#endif // INCLUDED_BAEXML_READER

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
