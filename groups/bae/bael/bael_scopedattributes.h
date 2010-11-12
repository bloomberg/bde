// bael_scopedattributes.h                                            -*-C++-*-
#ifndef INCLUDED_BAEL_SCOPEDATTRIBUTES
#define INCLUDED_BAEL_SCOPEDATTRIBUTES

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a class to add and remove attributes automatically.
//
//@CLASSES:
//  bael_ScopedAttributes: helper for safely managing attributes
//
//@SEE_ALSO: bael_attributecontext, bael_attribute
//
//@AUTHOR: Gang Chen (gchen20)
//
//@DESCRIPTION: This component provides an object, 'bael_ScopedAttributes',
// that serves as a "scoped helper" to safely manage 'bael_AttributeContainer'
// objects for the current attribute context.  A 'bael_ScopedAttributes'
// object, upon construction, will install a 'bael_AttributeContainer' object
// in the current attribute context and, more importantly, automatically
// remove that 'bael_AttributeContainer' object from the current attribute
// context upon destruction.
//
// Note that the 'bael_AttributeContainer' supplied at construction
// must remain valid and *unmodified* for the lifetime of this object.
//
///USAGE
///-----
// In the following code fragment, we will use a 'bael_ScopedAttributes' to
// install a 'bael_AttributeContainer' in the current context.
//
// We first create the current attribute context and two attributes:
//..
//  bael_AttributeContext *context = bael_AttributeContext::getContext();
//
//  bael_Attribute a1("uuid", 4044457);
//  bael_Attribute a2("name", "Gang Chen");
//  assert(false == context->hasAttribute(a1));
//  assert(false == context->hasAttribute(a2));
//..
// Now we create an 'AttributeSet' and add the two attributes to this set,
// then we use a 'bael_ScopedAttributes to install these attributes in the
// current thread's attribute context.
//
// Note that we use the 'AttributeSet' implementation of the
// 'bael_AttributeContainer' protocol defined in the component documentation
// for 'bael_attributecontainer' (the 'bael' package provides a similar class
// in the 'bael_defaultattributecontainer' component).
//..
//  {
//      AttributeSet attributes;
//      attributes.insert(a1);
//      attributes.insert(a2);
//      bael_ScopedAttributes attributeGuard(&attributes);
//      assert(true == context->hasAttribute(a1));
//      assert(true == context->hasAttribute(a2));
//..
// When 'attributeGuard' goes out of scope and is destroyed, 'attributes'
// are removed from the current thread's attribute context, which prevents the
// attribute context from referring to an invalid memory address (on the
// stack).
//..
//  }
//
//  assert(!context->hasAttribute(a1));
//  assert(!context->hasAttribute(a2));
//..

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BAEL_ATTRIBUTECONTEXT
#include <bael_attributecontext.h>
#endif

namespace BloombergLP {

class bael_AttributeContainer;

                        // ===========================
                        // class bael_ScopedAttributes
                        // ===========================

class bael_ScopedAttributes {
    // This class installs a 'bael_AttributeContainer' object in the current
    // attribute context on construction, and removes it on destruction.  Note
    // that the 'bael_AttributeContainer' supplied at construction must remain
    // valid and *unmodified* for the lifetime of this object.

    const bael_AttributeContext::iterator  d_it ;   // refers to attributes

    // NOT IMPLEMENTED
    bael_ScopedAttributes(const bael_ScopedAttributes&);
    bael_ScopedAttributes& operator=(const bael_ScopedAttributes&);

  public:
    // CREATORS
    bael_ScopedAttributes(const bael_AttributeContainer* attributes);
        // Create a 'bael_ScopedAttributes' object having the specified
        // 'attributes'.  Note that 'attributes' must remain valid and
        // *unmodified* for the lifetime of this object.

    ~bael_ScopedAttributes();
        // Remove the associated attributes from the current attribute context.
};

// ===========================================================================
//                      INLINE FUNCTION DEFINITIONS
// ===========================================================================

                        // ---------------------------
                        // class bael_ScopedAttributes
                        // ---------------------------

// CREATORS
inline
bael_ScopedAttributes::bael_ScopedAttributes(
                                    const bael_AttributeContainer* attributes)
: d_it(bael_AttributeContext::getContext()->addAttributes(attributes))
{
}

inline
bael_ScopedAttributes::~bael_ScopedAttributes()
{
    bael_AttributeContext::getContext()->removeAttributes(d_it);
}

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2007
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
