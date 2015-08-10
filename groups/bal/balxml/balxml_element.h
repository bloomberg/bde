// balxml_element.h       -*-C++-*-
#ifndef INCLUDED_BALXML_ELEMENT
#define INCLUDED_BALXML_ELEMENT

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a 'btlb::Blob'-based XML element container.
//
//@CLASSES:
// balxml::ElementRef     : mutable reference to sub-element in 'balxml::Element'
// balxml::ElementConstRef: immutable reference to sub-element in
//                         'balxml::Element'
// balxml::Element        : 'btlb::Blob'-based XML element
//
//@SEE_ALSO: btlb_blob
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
// conversion.  The reason it is efficient is because the 'balxml::Element'
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
// 'btlb::Blob', and reuse the blob buffers for each request.  Our 'convert'
// function will take this global scratch data as a third argument:
//..
//  int convert(btlb::Blob            *soapEnvelopeData,
//              const btlb::Blob&      portResponseData,
//              const balxml::Element&  globalScratchData)
//      // Convert from the specified 'portResponseData' to the specified
//      // 'soapEnvelopeData', using the specified 'globalScratchData'.  Return
//      // 0 on success, and a non-negative value otherwise.
//  {
//      enum { SUCCESS = 0, FAILURE = -1 };
//..
// In this example, 'portResponseData' will contain the data from 'Listing 1'
// and 'soapEnvelopeData' will be filled with the data from 'Listing 2'.  The
// 'globalScratchData' is assumed to be an 'balxml::Element' object that has
// been loaded with the contents of 'Listing 3'.  The first thing we will do
// is make a local 'scratchData' so that we can manipulate it locally.  Note
// that this will only do a shallow copy of the scratch data blob, so it
// should be efficient:
//..
//      balxml::Element scratchData = globalScratchData;
//..
// Next, we create an 'balxml::ElementRef' to refer to the 'soap:Envelope'
// element in the scratch data.  For convenience, we will also create a
// reference to the 'soap:Body' element:
//..
//      balxml::ElementRef soapEnvelope = scratchData[0]; // TBD: lookup by name
//      balxml::ElementRef soapBody     = soapEnvelope[0];
//..
// Now, we will construct an 'balxml::Element' object, loading the data from
// 'portResponseData'.  Note that we are only interested in 3 levels of the
// tree (to get to the 'MyResponseElement' element).  Once we get to this
// element, we will loop from 'scratchData[1]' through to
// 'scratchData[scratchData.numSubElements()-1]' to find an appropriate element
// that has the 'xmlns' attribute filled in:
//..
//      balxml::Element portResponse;
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
//      balxml::ElementRef opName          = portResponse[0];
//      balxml::ElementRef responseElement = opName[0];
//      balxml::ElementRef responseElementWithNs;
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
//      btlb::Blob content;
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

#ifndef INCLUDED_BALSCM_VERSION
#include <balscm_version.h>
#endif

#ifndef INCLUDED_BTLB_BLOB
#include <btlb_blob.h>
#endif

#ifndef INCLUDED_BSLMA_ALLOCATOR
#include <bslma_allocator.h>
#endif

#ifndef INCLUDED_BSLMA_DEFAULT
#include <bslma_default.h>
#endif

#ifndef INCLUDED_BSL_MEMORY
#include <bsl_memory.h>
#endif

#ifndef INCLUDED_BSL_STRING
#include <bsl_string.h>
#endif

#ifndef INCLUDED_BSL_VECTOR
#include <bsl_vector.h>
#endif

namespace BloombergLP {

// Forward references

namespace balxml {class ElementRef;
class ElementConstRef;
class Element;

class Element_Node;
class Element_Imp;

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
// Element_Imp
//   - btlb::Blob        d_data;
//   - bslma::Allocator *d_allocator_p;
//
// Element_Node
//   - Element_Imp    *d_imp_p;
//   - Element_Node   *d_parent_p;
//   - int                    d_openTagBegin;
//   - int                    d_openTagLength;
//   - int                    d_elementLength;
//   - vector<Element_Node*>  d_subNodes;
//
// ElementRef
//   - bsl::shared_ptr<Element_Node> d_node_p;
//
// Element
//   - Element_Imp d_imp;
//   - ElementRef  d_root;
//..
//
// EXTRA INFORMATION ABOUT NODE
//..
// - 'd_imp_p' points to the 'd_imp' member of 'Element'
// - 'd_parent_p' points to the parent node.  It will be 0 if this is the root
//   of the tree (i.e., if this 'Element_Node' is pointed to by
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
                         // private class Element_Imp
                         // ================================

class Element_Imp {
    // Component-private class.  Do not use.
    // This class provides access to an 'Element's data members so that they
    // can be used by multiple instances of 'Element_Node'.

    // NOT IMPLEMENTED
    Element_Imp(const Element_Imp&);
    Element_Imp& operator=(const Element_Imp&);

  public:
    // DATA MEMBERS
    btlb::Blob        d_data;
    bslma::Allocator *d_allocator_p;

    // CREATORS
    explicit Element_Imp(bslma::Allocator *basicAllocator = 0)
    : d_data(basicAllocator)
    , d_allocator_p(bslma::Default::allocator(basicAllocator))
    {
    }

    ~Element_Imp()
    {
    }
};

                         // =================================
                         // private class Element_Node
                         // =================================

class Element_Node {
    // Component-private class.  Do not use.
    // This class contains information about an element node.

    // NOT IMPLEMENTED
    Element_Node(const Element_Node&);
    Element_Node& operator=(const Element_Node&);

  public:
    // TYPES
    typedef
    bsl::vector<bsl::shared_ptr<Element_Node> > SubNodes;

    // DATA MEMBERS
    Element_Imp  *d_imp_p;
    Element_Node *d_parent_p;
    int           d_openTagBegin;
    int           d_openTagLength;
    int           d_elementLength;
    SubNodes      d_subNodes;

    // CREATORS
    explicit Element_Node(bslma::Allocator *basicAllocator = 0)
    : d_imp_p(0)
    , d_parent_p(0)
    , d_openTagBegin(0)
    , d_openTagLength(0)
    , d_elementLength(0)
    , d_subNodes(basicAllocator)
    {
    }

    ~Element_Node()
    {
    }

    // ACCESSORS
    int indexOffsetAmount() const
        // Return the relative index offset for the 'd_openTagBegin' member.
    {
        int           ret  = 0;
        Element_Node *node = d_parent_p;

        while (node) {
            ret  += node->d_openTagBegin;
            node  = node->d_parent_p;
        }

        return ret;
    }
};

                              // =======================
                              // class ElementRef
                              // =======================

class ElementRef {
    // This class has *reference* *semantics*.

    // FRIENDS
    friend class Element;
    friend bool operator==(const ElementRef& lhs,
                           const ElementRef& rhs);

    // PRIVATE DATA MEMBERS
    bsl::shared_ptr<Element_Node> d_node_p;
        // multiple 'ElementRef's can refer to the same element.

  public:
    // CREATORS
    explicit ElementRef();
        // Construct a null element reference.

    ElementRef(const ElementRef& original);
        // Construct an element reference that refers to the same element as
        // the specified 'original'.

    ~ElementRef();
        // Destroy this object.

    // MANIPULATORS
    ElementRef& operator=(const ElementRef& rhs);
        // Assign the reference from the specified 'rhs' 'ElementRef' to this
        // 'ElementRef'.  Return a reference to this modifiable 'ElementRef'.

    // ACCESSORS
    operator bool() const;
        // Return 'true' if this 'ElementRef' is valid, and false otherwise.

    ElementRef operator[](int index) const;
        // Return an 'ElementRef' to the sub-element at the specified 'index'.
        // The behavior is undefined unless '0 <= index' and
        // 'index < numSubElements()'.

    void extractContent(btlb::Blob *content) const;
        // Extract the XML content for the element referred to by this
        // 'ElementRef' into the specified 'content'.  This does not include
        // the open tag or close tag.

    void extractData(btlb::Blob *data) const;
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
                          const ElementConstRef& elementRef) const;
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

    void setContent(const btlb::Blob& content) const;
        // Set the content of the element referred to by this 'ElementRef' to
        // the specified 'content'.
};

                           // =====================
                           // class ElementConstRef
                           // =====================

class ElementConstRef {
    // This class has *reference* *semantics*.

    // PRIVATE DATA MEMBERS
    ElementRef d_imp;

    // FRIENDS
    friend class ElementRef;
    friend class Element;
    friend bool operator==(const ElementConstRef& lhs,
                           const ElementConstRef& rhs);

  public:
    // CREATORS
    explicit ElementConstRef();
        // Construct a null immutable element reference.

    ElementConstRef(const ElementConstRef& original);
        // Construct an immutable element reference that refers to the same
        // element as the specified 'original'.

    ElementConstRef(const ElementRef& original);
        // Construct an immutable element reference that refers to the same
        // element as the specified 'original' mutable element reference.  This
        // provides an implicit conversion from 'ElementRef' to
        // 'ElementConstRef'.

    ~ElementConstRef();
        // Destroy this object.

    // MANIPULATORS
    ElementConstRef& operator=(const ElementConstRef& rhs);
        // Assign the reference from the specified 'rhs' 'ElementConstRef' to
        // this 'ElementConstRef'.  Return a reference to this modifiable
        // 'ElementConstRef'.

    // ACCESSORS
    operator bool() const;
        // Return 'true' if this 'ElementConstRef' is valid, and false
        // otherwise.

    ElementConstRef operator[](int index) const;
        // Return an 'ElementConstRef' to the sub-element at the specified
        // 'index'.  The behavior is undefined unless '0 <= index' and
        // 'index < numSubElements()'.

    void extractContent(btlb::Blob *content) const;
        // Extract the XML content for the element referred to by this
        // 'ElementConstRef' into the specified 'content'.  This does not
        // include the open tag or close tag.

    void extractData(btlb::Blob *data) const;
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
                               // class Element
                               // =============

class Element {
    // TBD: doc

    // PRIVATE DATA MEMBERS
    Element_Imp d_imp;
    ElementRef  d_root;

  public:
    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(Element,
                                 bslalg::TypeTraitUsesBslmaAllocator);

    // CREATORS
    explicit Element(bslma::Allocator *basicAllocator = 0);
        // Construct an empty element and use the specified 'basicAllocator' to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator will be used.  Note that this 'Element' object
        // cannot be used until it is assigned state using 'operator=' or
        // 'load(const btlb::Blob&)'.

    Element(const Element&  original,
                   bslma::Allocator      *basicAllocator = 0);
        // Construct an copy of the specified 'original' element and use the
        // specified 'basicAllocator' to supply memory.  If 'basicAllocator' is
        // 0, the currently installed default allocator will be used.

    Element(const ElementRef&       original,
                   bslma::Allocator              *basicAllocator = 0);
    Element(const ElementConstRef&  original,
                   bslma::Allocator              *basicAllocator = 0);
        // Construct a copy of the element referred to by the specified
        // 'original' and use the specified 'basicAllocator' to supply memory.
        // If 'basicAllocator' is 0, the currently installed default allocator
        // will be used.

    ~Element();
        // Destroy this object.

    // MANIPULATORS
    Element& operator=(const Element& rhs);
        // Assign the specified 'rhs' element to this object and return a
        // reference to this modifiable object.

    Element& operator=(const ElementRef&      rhs);
    Element& operator=(const ElementConstRef& rhs);
        // Assign the element referred to by the specified 'rhs' to this object
        // and return a reference to this modifiable object.

    ElementRef operator[](int index);
        // Return an 'ElementRef' to the sub-element at the specified 'index'.
        // The behavior is undefined unless '0 <= index' and
        // 'index < numSubElements()'.

    void insertSubElement(int                           index,
                          const ElementConstRef& elementRef) const;
        // Insert the element referred to by the specified 'elementRef' as a
        // sub-element of this element at the specified 'index'.  The behavior
        // is undefined unless '0 <= index' and 'index <= numSubElements()'.

    int load(const btlb::Blob& data);
    int load(const btlb::Blob& data, int numLevels);
        // Load the element with the specified 'data'.  Return 0 on success,
        // and a non-zero value otherwise, with no effect on this object.

    void setContent(const btlb::Blob& content);
        // Set the content of this element to the specified 'content'.

    void removeAllSubElements();
        // Remove all sub-elements from this element.

    void removeSubElement(int index);
        // Remove the sub-element at the specified 'index' from this element.
        // The behavior is undefined unless '0 <= index' and
        // 'index < numSubElements()'.

    // ACCESSORS
    ElementConstRef operator[](int index) const;
        // Return an 'ElementConstRef' to the sub-element at the specified
        // 'index'.  The behavior is undefined unless '0 <= index' and
        // 'index < numSubElements()'.

    operator ElementConstRef() const;
        // Return an 'ElementConstRef' to this element.

    void extractContent(btlb::Blob *content) const;
        // Extract the XML content for this element into the specified
        // 'content'.  This does not include the open tag or close tag.

    void extractData(btlb::Blob *data) const;
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
bool operator==(const ElementRef& lhs, const ElementRef& rhs);
    // Return 'true' if the specified 'lhs' and the specified 'rhs' refer to
    // the same element, and 'false' otherwise.

inline
bool operator!=(const ElementRef& lhs, const ElementRef& rhs);
    // Return 'true' if the specified 'lhs' and the specified 'rhs' do not
    // refer to the same element, and false otherwise.

inline
bool operator==(const ElementConstRef& lhs,
                const ElementConstRef& rhs);
    // Return 'true' if the specified 'lhs' and the specified 'rhs' refer to
    // the same element, and false otherwise.

inline
bool operator!=(const ElementConstRef& lhs,
                const ElementConstRef& rhs);
}  // close package namespace
    // Return 'true' if the specified 'lhs' and the specified 'rhs' do not
    // refer to the same element, and false otherwise.

// ============================================================================
//                        INLINE FUNCTION DEFINITIONS
// ============================================================================

// FREE OPERATORS

inline
bool balxml::operator==(const ElementRef& lhs, const ElementRef& rhs)
{
    return lhs.d_node_p == rhs.d_node_p;
}

inline
bool balxml::operator!=(const ElementRef& lhs, const ElementRef& rhs)
{
    return !(lhs == rhs);
}

inline
bool balxml::operator==(const ElementConstRef& lhs,
                const ElementConstRef& rhs)
{
    return lhs.d_imp == rhs.d_imp;
}

inline
bool balxml::operator!=(const ElementConstRef& lhs,
                const ElementConstRef& rhs)
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
