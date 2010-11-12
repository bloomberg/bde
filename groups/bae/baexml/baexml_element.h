// baexml_element.h       -*-C++-*-
#ifndef INCLUDED_BAEXML_ELEMENT
#define INCLUDED_BAEXML_ELEMENT

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a 'bcema_Blob'-based XML element container.
//
//@CLASSES:
// baexml_ElementRef     : mutable reference to sub-element in 'baexml_Element'
// baexml_ElementConstRef: immutable reference to sub-element in
//                         'baexml_Element'
// baexml_Element        : 'bcema_Blob'-based XML element
//
//@SEE_ALSO: bcema_blob
//
//@AUTHOR: Shezan Baig (sbaig)
//
//@CONTACT: Rohan Bhindwale (rbhindwa)
//
//@DESCRIPTION:
// TBD: component-level doc
//
///Usage
///-----
// The following snippets of code illustrate the usage of this component.  The
// main usage of this component is to manipulate XML elements or rearrange
// them.  This example will demonstrate converting an XML document in 'bas' XML
// format to SOAP format.
//
// Suppose we receive an XML response that looks like the following:
//..
//  // Listing 1
//  // ---------
//  const char *portResponseMsg =
//      "<?xml version='1.0' encoding='utf-8'?>"
//      "<PortResponse>"
//          "<OpName>"
//              "<MyResponseElement>"
//                  "<SomeData>"
//                      "<FieldValue>qwerty</FieldValue>"
//                  "</SomeData>"
//                  "<OtherData>abcdef</OtherData>"
//              "</MyResponseElement>"
//          "</OpName>"
//      "</PortResponse>";
//..
// This is how a typical response looks like from a 'bas' service that was
// generated from a WSDL file using the 'bas_codegen.pl' service generator.
// The corresponding SOAP response looks something like:
//..
//  // Listing 2
//  // ---------
//  const char *soapEnvelopeMsg =
//      "<soap:Envelope"
//                 " xmlns:xsi='http://www.w3.org/2001/XMLSchema-instance'"
//                 " xmlns:xsd='http://www.w3.org/2001/XMLSchema'"
//                 " xmlns:soap='http://schemas.xmlsoap.org/soap/envelope/'>"
//          "<soap:Body>"
//              "<MyResponseElement xmlns='http://myservice/ns'>"
//                  "<SomeData>"
//                      "<FieldValue>qwerty</FieldValue>"
//                  "</SomeData>"
//                  "<OtherData>abcdef</OtherData>"
//              "</MyResponseElement>"
//          "</soap:Body>"
//      "</soap:Envelope>";
//..
// In order to convert from 'Listing 1' to 'Listing 2', we need to extract the
// 'MyResponseElement' element and insert it as a sub-element of the
// 'soap:Body' element.  As an additional requirement, we need to add the
// 'xmlns' attribute to the 'MyResponseElement' element.
//
// This component allows us to efficiently and intuitively implement this
// conversion.  The reason it is efficient is because the 'baexml_Element'
// class is blob-based, so there is no deep copy of the underlying data.
//
// In this example, we will use the following "scratch-data" to aid the
// conversion:
//..
//  // Listing 3
//  // ---------
//  const char *scratchDataText =
//      "<ScratchData>"
//          "<soap:Envelope"
//                 " xmlns:xsi='http://www.w3.org/2001/XMLSchema-instance'"
//                 " xmlns:xsd='http://www.w3.org/2001/XMLSchema'"
//                 " xmlns:soap='http://schemas.xmlsoap.org/soap/envelope/'>"
//              "<soap:Body>"
//              "</soap:Body>"
//          "</soap:Envelope>"
//          "<SomeResponse xmlns='http://myservice/ns'></SomeResponse>"
//          "<MyResponseElement xmlns='http://myservice/ns'>"
//      "</MyResponseElement>"
//          "<OtherResponse xmlns='http://myservice/ns'></OtherResponse>"
//          "<FourthResponse xmlns='http://myservice/ns'></FourthResponse>"
//      "</ScratchData>";
//..
// Note that our "scratch-data" contains a soap envelope with an empty soap
// body, and a few response elements with the 'xmlns' attribute defined.  Since
// we do not know which response element to use yet, we will walk through our
// data in 'Listing 1' and pick an appropriate element from the scratch data.
//
// For efficiency reasons, we will allocate this scratch data only once, into a
// 'bcema_Blob', and reuse the blob buffers for each request.  Our 'convert'
// function will take this global scratch data as a third argument:
//..
//  int convert(bcema_Blob            *soapEnvelopeData,
//              const bcema_Blob&      portResponseData,
//              const baexml_Element&  globalScratchData)
//      // Convert from the specified 'portResponseData' to the specified
//      // 'soapEnvelopeData', using the specified 'globalScratchData'.  Return
//      // 0 on success, and a non-negative value otherwise.
//  {
//      enum { SUCCESS = 0, FAILURE = -1 };
//..
// In this example, 'portResponseData' will contain the data from 'Listing 1'
// and 'soapEnvelopeData' will be filled with the data from 'Listing 2'.  The
// 'globalScratchData' is assumed to be an 'baexml_Element' object that has
// been loaded with the contents of 'Listing 3'.  The first thing we will do
// is make a local 'scratchData' so that we can manipulate it locally.  Note
// that this will only do a shallow copy of the scratch data blob, so it
// should be efficient:
//..
//      baexml_Element scratchData = globalScratchData;
//..
// Next, we create an 'baexml_ElementRef' to refer to the 'soap:Envelope'
// element in the scratch data.  For convenience, we will also create a
// reference to the 'soap:Body' element:
//..
//      baexml_ElementRef soapEnvelope = scratchData[0]; // TBD: lookup by name
//      baexml_ElementRef soapBody     = soapEnvelope[0];
//..
// Now, we will construct an 'baexml_Element' object, loading the data from
// 'portResponseData'.  Note that we are only interested in 3 levels of the
// tree (to get to the 'MyResponseElement' element).  Once we get to this
// element, we will loop from 'scratchData[1]' through to
// 'scratchData[scratchData.numSubElements()-1]' to find an appropriate element
// that has the 'xmlns' attribute filled in:
//..
//      baexml_Element portResponse;
//
//      if (0 != portResponse.load(portResponseData, 3)) {
//          return FAILURE;
//      }
//
//      if (1 != portResponse.numSubElements()
//       || 1 != portResponse[0].numSubElements()) {
//          return FAILURE;
//      }
//
//      baexml_ElementRef opName          = portResponse[0];
//      baexml_ElementRef responseElement = opName[0];
//      baexml_ElementRef responseElementWithNs;
//
//      bsl::string responseElementName;
//
//      responseElement.extractElementName(&responseElementName);
//
//      for (int i = 1; i < scratchData.numSubElements(); ++i) {
//          bsl::string testName;
//          scratchData[i].extractElementName(&testName);
//          if (testName == responseElementName) {
//              responseElementWithNs = scratchData[i];
//              break;
//          }
//      }
//
//      if (!responseElementWithNs) {
//          return FAILURE;
//      }
//..
// Now that we have found the corresponding response element with the 'xmlns'
// attribute, we can start moving data around.  Note that although new sub
// elements are inserted, there is no deep copying of data, so it will still be
// efficient:
//..
//      bcema_Blob content;
//
//      responseElement.extractContent(&content);
//      responseElementWithNs.setContent(content);
//      soapBody.insertSubElement(0, responseElementWithNs);
//
//      soapEnvelope.extractData(soapEnvelopeData);
//..
// At this point, the 'soapEnvelopeData' blob will contain the contents of
// 'Listing 2', so we can return successfully:
//..
//      return SUCCESS;
//  }
//..

#ifndef INCLUDED_BAESCM_VERSION
#include <baescm_version.h>
#endif

#ifndef INCLUDED_BCEMA_BLOB
#include <bcema_blob.h>
#endif

#ifndef INCLUDED_BCEMA_SHAREDPTR
#include <bcema_sharedptr.h>
#endif

#ifndef INCLUDED_BSLMA_DEFAULT
#include <bslma_default.h>
#endif

#ifndef INCLUDED_BSL_STRING
#include <bsl_string.h>
#endif

#ifndef INCLUDED_BSL_VECTOR
#include <bsl_vector.h>
#endif

namespace BloombergLP {

class bslma_Allocator;

// Forward references
class baexml_ElementRef;
class baexml_ElementConstRef;
class baexml_Element;

class baexml_Element_Node;
class baexml_Element_Imp;

// IMPLEMENTATION NOTES
//..
// <?xml version='1.0' encoding='utf-8'?>
// <Root>
//     <E1>
//         <SE1>abc</SE1>
//         <SE2>def</SE2>
//     </E1>
//     <E2>xyz</E2>
// </Root>
//
// baexml_Element_Imp
//   - bcema_Blob       d_data;
//   - bslma_Allocator *d_allocator_p;
//
// baexml_Element_Node
//   - baexml_Element_Imp    *d_imp_p;
//   - baexml_Element_Node   *d_parent_p;
//   - int                    d_openTagBegin;
//   - int                    d_openTagLength;
//   - int                    d_elementLength;
//   - vector<baexml_Element_Node*>  d_subNodes;
//
// baexml_ElementRef
//   - bcema_SharedPtr<baexml_Element_Node> d_node_p;
//
// baexml_Element
//   - baexml_Element_Imp d_imp;
//   - baexml_ElementRef  d_root;
//..
//
// EXTRA INFORMATION ABOUT NODE
//..
// - 'd_imp_p' points to the 'd_imp' member of 'baexml_Element'
// - 'd_parent_p' points to the parent node.  It will be 0 if this is the root
//   of the tree (i.e., if this 'baexml_Element_Node' is pointed to by
//   'd_root.d_node_p').
// - 'd_openTagBegin' is the offset index into 'd_imp_p->d_data' for the '<'
//   character of the opening tag.  Note that this is not the absolute
//   index into 'd_imp_p->d_data' - it is relative to 'indexOffsetAmount()'.
// - 'd_openTagLength' is the number of characters in the open tag.
// - 'd_elementLength' is the number of characters in the whole element
//   (from the '<' of the open tag to the '>' of the close tag).
//..
// Note that 'd_openTagLength == d_elementLength' if the XML element is
// opened and closed within the same tag, e.g:
//..
// <someElement attr1="abc" attr2="def" />
//..

// TBD: allow element lookup by name

                         // ================================
                         // private class baexml_Element_Imp
                         // ================================

class baexml_Element_Imp {
    // Component-private class.  Do not use.
    // This class provides access to an 'Element's data members so that they
    // can be used by multiple instances of 'Element_Node'.

    // NOT IMPLEMENTED
    baexml_Element_Imp(const baexml_Element_Imp&);
    baexml_Element_Imp& operator=(const baexml_Element_Imp&);

  public:
    // DATA MEMBERS
    bcema_Blob       d_data;
    bslma_Allocator *d_allocator_p;

    // CREATORS
    explicit baexml_Element_Imp(bslma_Allocator *basicAllocator = 0)
    : d_data(basicAllocator)
    , d_allocator_p(bslma_Default::allocator(basicAllocator))
    {
    }

    ~baexml_Element_Imp()
    {
    }
};

                         // =================================
                         // private class baexml_Element_Node
                         // =================================

class baexml_Element_Node {
    // Component-private class.  Do not use.
    // This class contains information about an element node.

    // NOT IMPLEMENTED
    baexml_Element_Node(const baexml_Element_Node&);
    baexml_Element_Node& operator=(const baexml_Element_Node&);

  public:
    // TYPES
    typedef
    bsl::vector<bcema_SharedPtr<baexml_Element_Node> > SubNodes;

    // DATA MEMBERS
    baexml_Element_Imp  *d_imp_p;
    baexml_Element_Node *d_parent_p;
    int           d_openTagBegin;
    int           d_openTagLength;
    int           d_elementLength;
    SubNodes      d_subNodes;

    // CREATORS
    explicit baexml_Element_Node(bslma_Allocator *basicAllocator = 0)
    : d_imp_p(0)
    , d_parent_p(0)
    , d_openTagBegin(0)
    , d_openTagLength(0)
    , d_elementLength(0)
    , d_subNodes(basicAllocator)
    {
    }

    ~baexml_Element_Node()
    {
    }

    // ACCESSORS
    int indexOffsetAmount() const
        // Return the relative index offset for the 'd_openTagBegin' member.
    {
        int           ret  = 0;
        baexml_Element_Node *node = d_parent_p;

        while (node) {
            ret  += node->d_openTagBegin;
            node  = node->d_parent_p;
        }

        return ret;
    }
};

                              // =======================
                              // class baexml_ElementRef
                              // =======================

class baexml_ElementRef {
    // This class has *reference* *semantics*.

    // FRIENDS
    friend class baexml_Element;
    friend bool operator==(const baexml_ElementRef& lhs,
                           const baexml_ElementRef& rhs);

    // PRIVATE DATA MEMBERS
    bcema_SharedPtr<baexml_Element_Node> d_node_p;
        // multiple 'baexml_ElementRef's can refer to the same element.

  public:
    // CREATORS
    explicit baexml_ElementRef();
        // Construct a null element reference.

    baexml_ElementRef(const baexml_ElementRef& original);
        // Construct an element reference that refers to the same element as
        // the specified 'original'.

    ~baexml_ElementRef();
        // Destroy this object.

    // MANIPULATORS
    baexml_ElementRef& operator=(const baexml_ElementRef& rhs);
        // Assign the reference from the specified 'rhs' 'ElementRef' to this
        // 'ElementRef'.  Return a reference to this modifiable 'ElementRef'.

    // ACCESSORS
    operator bool() const;
        // Return 'true' if this 'ElementRef' is valid, and false otherwise.

    baexml_ElementRef operator[](int index) const;
        // Return an 'ElementRef' to the sub-element at the specified 'index'.
        // The behavior is undefined unless '0 <= index' and
        // 'index < numSubElements()'.

    void extractContent(bcema_Blob *content) const;
        // Extract the XML content for the element referred to by this
        // 'ElementRef' into the specified 'content'.  This does not include
        // the open tag or close tag.

    void extractData(bcema_Blob *data) const;
        // Extract the XML data for the element referred to by this
        // 'ElementRef' into the specified 'data'.  This includes the open tag,
        // content, and close tag.

    void extractElementName(bsl::string *elementName) const;
        // Extract the element name for the element referred to by this
        // 'ElementRef' into the specified 'elementName'.

    void extractOpenTag(bsl::string *tag) const;
        // Extract the open tag for the element referred to by this
        // 'ElementRef' into the specified 'tag'.

    int numSubElements() const;
        // Return the number of sub-elements in the element referred to by this
        // 'ElementRef'.

    // REFERENCED-VALUE MANIPULATORS

    void insertSubElement(int                           index,
                          const baexml_ElementConstRef& elementRef) const;
        // Insert the element referred to by the specified 'elementRef' as a
        // sub-element of the element referred to by this 'ElementRef' at the
        // specified 'index'.  The behavior is undefined unless '0 <= index'
        // and 'index <= numSubElements()'.

    void removeAllSubElements() const;
        // Remove all sub-elements from the element referred to by this
        // 'ElementRef'.

    void removeSubElement(int index) const;
        // Remove the sub-element at the specified 'index' from the element
        // referred to by this 'ElementRef'.  The behavior is undefined unless
        // '0 <= index' and 'index < numSubElements()'.

    void setContent(const bcema_Blob& content) const;
        // Set the content of the element referred to by this 'ElementRef' to
        // the specified 'content'.
};

                           // =====================
                           // class baexml_ElementConstRef
                           // =====================

class baexml_ElementConstRef {
    // This class has *reference* *semantics*.

    // PRIVATE DATA MEMBERS
    baexml_ElementRef d_imp;

    // FRIENDS
    friend class baexml_ElementRef;
    friend class baexml_Element;
    friend bool operator==(const baexml_ElementConstRef& lhs,
                           const baexml_ElementConstRef& rhs);

  public:
    // CREATORS
    explicit baexml_ElementConstRef();
        // Construct a null immutable element reference.

    baexml_ElementConstRef(const baexml_ElementConstRef& original);
        // Construct an immutable element reference that refers to the same
        // element as the specified 'original'.

    baexml_ElementConstRef(const baexml_ElementRef& original);
        // Construct an immutable element reference that refers to the same
        // element as the specified 'original' mutable element reference.  This
        // provides an implicit conversion from 'ElementRef' to
        // 'ElementConstRef'.

    ~baexml_ElementConstRef();
        // Destroy this object.

    // MANIPULATORS
    baexml_ElementConstRef& operator=(const baexml_ElementConstRef& rhs);
        // Assign the reference from the specified 'rhs' 'ElementConstRef' to
        // this 'ElementConstRef'.  Return a reference to this modifiable
        // 'ElementConstRef'.

    // ACCESSORS
    operator bool() const;
        // Return 'true' if this 'ElementConstRef' is valid, and false
        // otherwise.

    baexml_ElementConstRef operator[](int index) const;
        // Return an 'ElementConstRef' to the sub-element at the specified
        // 'index'.  The behavior is undefined unless '0 <= index' and
        // 'index < numSubElements()'.

    void extractContent(bcema_Blob *content) const;
        // Extract the XML content for the element referred to by this
        // 'ElementConstRef' into the specified 'content'.  This does not
        // include the open tag or close tag.

    void extractData(bcema_Blob *data) const;
        // Extract the XML data for the element referred to by this
        // 'ElementConstRef' into the specified 'data'.  This includes the open
        // tag, content, and close tag.

    void extractElementName(bsl::string *elementName) const;
        // Extract the element name for this element into the specified
        // 'elementName'.

    void extractOpenTag(bsl::string *tag) const;
        // Extract the open tag for the element referred to by this
        // 'ElementConstRef' into the specified 'tag'.

    int numSubElements() const;
        // Return the number of sub-elements in the element referred to by this
        // 'ElementConstRef'.
};

                               // =============
                               // class baexml_Element
                               // =============

class baexml_Element {
    // TBD: doc

    // PRIVATE DATA MEMBERS
    baexml_Element_Imp d_imp;
    baexml_ElementRef  d_root;

  public:
    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(baexml_Element,
                                 bslalg_TypeTraitUsesBslmaAllocator);

    // CREATORS
    explicit baexml_Element(bslma_Allocator *basicAllocator = 0);
        // Construct an empty element and use the specified 'basicAllocator' to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator will be used.  Note that this 'Element' object
        // cannot be used until it is assigned state using 'operator=' or
        // 'load(const bcema_Blob&)'.

    baexml_Element(const baexml_Element&  original,
                   bslma_Allocator       *basicAllocator = 0);
        // Construct an copy of the specified 'original' element and use the
        // specified 'basicAllocator' to supply memory.  If 'basicAllocator' is
        // 0, the currently installed default allocator will be used.

    baexml_Element(const baexml_ElementRef&       original,
                   bslma_Allocator               *basicAllocator = 0);
    baexml_Element(const baexml_ElementConstRef&  original,
                   bslma_Allocator               *basicAllocator = 0);
        // Construct a copy of the element referred to by the specified
        // 'original' and use the specified 'basicAllocator' to supply memory.
        // If 'basicAllocator' is 0, the currently installed default allocator
        // will be used.

    ~baexml_Element();
        // Destroy this object.

    // MANIPULATORS
    baexml_Element& operator=(const baexml_Element& rhs);
        // Assign the specified 'rhs' element to this object and return a
        // reference to this modifiable object.

    baexml_Element& operator=(const baexml_ElementRef&      rhs);
    baexml_Element& operator=(const baexml_ElementConstRef& rhs);
        // Assign the element referred to by the specified 'rhs' to this object
        // and return a reference to this modifiable object.

    baexml_ElementRef operator[](int index);
        // Return an 'ElementRef' to the sub-element at the specified 'index'.
        // The behavior is undefined unless '0 <= index' and
        // 'index < numSubElements()'.

    void insertSubElement(int                           index,
                          const baexml_ElementConstRef& elementRef) const;
        // Insert the element referred to by the specified 'elementRef' as a
        // sub-element of this element at the specified 'index'.  The behavior
        // is undefined unless '0 <= index' and 'index <= numSubElements()'.

    int load(const bcema_Blob& data);
    int load(const bcema_Blob& data, int numLevels);
        // Load the element with the specified 'data'.  Return 0 on success,
        // and a non-zero value otherwise, with no effect on this object.

    void setContent(const bcema_Blob& content);
        // Set the content of this element to the specified 'content'.

    void removeAllSubElements();
        // Remove all sub-elements from this element.

    void removeSubElement(int index);
        // Remove the sub-element at the specified 'index' from this element.
        // The behavior is undefined unless '0 <= index' and
        // 'index < numSubElements()'.

    // ACCESSORS
    baexml_ElementConstRef operator[](int index) const;
        // Return an 'ElementConstRef' to the sub-element at the specified
        // 'index'.  The behavior is undefined unless '0 <= index' and
        // 'index < numSubElements()'.

    operator baexml_ElementConstRef() const;
        // Return an 'ElementConstRef' to this element.

    void extractContent(bcema_Blob *content) const;
        // Extract the XML content for this element into the specified
        // 'content'.  This does not include the open tag or close tag.

    void extractData(bcema_Blob *data) const;
        // Extract the XML data for this element into the specified 'data'.
        // This includes the open tag, content, and close tag.

    void extractElementName(bsl::string *elementName) const;
        // Extract the element name for this element into the specified
        // 'elementName'.

    void extractOpenTag(bsl::string *tag) const;
        // Extract the open tag for this element into the specified 'tag'.

    int numSubElements() const;
        // Return the number of sub-elements in this element.
};

// FREE OPERATORS

inline
bool operator==(const baexml_ElementRef& lhs, const baexml_ElementRef& rhs);
    // Return 'true' if the specified 'lhs' and the specified 'rhs' refer to
    // the same element, and 'false' otherwise.

inline
bool operator!=(const baexml_ElementRef& lhs, const baexml_ElementRef& rhs);
    // Return 'true' if the specified 'lhs' and the specified 'rhs' do not
    // refer to the same element, and false otherwise.

inline
bool operator==(const baexml_ElementConstRef& lhs,
                const baexml_ElementConstRef& rhs);
    // Return 'true' if the specified 'lhs' and the specified 'rhs' refer to
    // the same element, and false otherwise.

inline
bool operator!=(const baexml_ElementConstRef& lhs,
                const baexml_ElementConstRef& rhs);
    // Return 'true' if the specified 'lhs' and the specified 'rhs' do not
    // refer to the same element, and false otherwise.

// ============================================================================
//                        INLINE FUNCTION DEFINITIONS
// ============================================================================

// FREE OPERATORS

inline
bool operator==(const baexml_ElementRef& lhs, const baexml_ElementRef& rhs)
{
    return lhs.d_node_p == rhs.d_node_p;
}

inline
bool operator!=(const baexml_ElementRef& lhs, const baexml_ElementRef& rhs)
{
    return !(lhs == rhs);
}

inline
bool operator==(const baexml_ElementConstRef& lhs,
                const baexml_ElementConstRef& rhs)
{
    return lhs.d_imp == rhs.d_imp;
}

inline
bool operator!=(const baexml_ElementConstRef& lhs,
                const baexml_ElementConstRef& rhs)
{
    return !(lhs == rhs);
}

}  // close namespace BloombergLP

#endif // INCLUDED_BAEXML_ELEMENT

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2005
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
