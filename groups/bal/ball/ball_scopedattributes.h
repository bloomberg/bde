// ball_scopedattributes.h                                            -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BALL_SCOPEDATTRIBUTES
#define INCLUDED_BALL_SCOPEDATTRIBUTES

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a class to add and remove attributes automatically.
//
//@CLASSES:
//  ball::ScopedAttributes: helper for safely managing attributes
//
//@SEE_ALSO: ball_attributecontext, ball_attribute
//
//@DESCRIPTION: This component provides a type, 'ball::ScopedAttributes',
// that serves as a "scoped helper" to safely manage 'ball::AttributeContainer'
// objects for the current attribute context.  A 'ball::ScopedAttributes'
// object, upon construction, will install a 'ball::AttributeContainer' object
// in the current attribute context and, more importantly, automatically
// remove that 'ball::AttributeContainer' object from the current attribute
// context upon destruction.
//
// Note that the 'ball::AttributeContainer' supplied at construction must
// remain valid and *unmodified* for the lifetime of this object.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Installing an 'ball::AttributeContanier'
///- - - - - - - - - - - - - - - - - - - - - - - - - -
// In the following code fragment, we will use a 'ball::ScopedAttributes' to
// install a 'ball::AttributeContainer' in the current context.
//
// We first create the current attribute context and two attributes:
//..
//  ball::AttributeContext *context = ball::AttributeContext::getContext();
//
//  ball::Attribute a1("uuid", 4044457);
//  ball::Attribute a2("name", "Gang Chen");
//  assert(false == context->hasAttribute(a1));
//  assert(false == context->hasAttribute(a2));
//..
// Now we create an 'AttributeSet' and add the two attributes to this set,
// then we use a 'ball::ScopedAttributes to install these attributes in the
// current thread's attribute context.
//
// Note that we use the 'AttributeSet' implementation of the
// 'ball::AttributeContainer' protocol defined in the component documentation
// for 'ball_attributecontainer' (the 'ball' package provides a similar class
// in the 'ball_defaultattributecontainer' component).
//..
//  {
//      AttributeSet attributes;
//      attributes.insert(a1);
//      attributes.insert(a2);
//      ball::ScopedAttributes attributeGuard(&attributes);
//      assert(true == context->hasAttribute(a1));
//      assert(true == context->hasAttribute(a2));
//..
// When 'attributeGuard' goes out of scope and is destroyed, 'attributes' are
// removed from the current thread's attribute context, which prevents the
// attribute context from referring to an invalid memory address (on the
// stack).
//..
//  }
//
//  assert(!context->hasAttribute(a1));
//  assert(!context->hasAttribute(a2));
//..

#ifndef INCLUDED_BALSCM_VERSION
#include <balscm_version.h>
#endif

#ifndef INCLUDED_BALL_ATTRIBUTECONTAINER
#include <ball_attributecontainer.h>
#endif

#ifndef INCLUDED_BALL_ATTRIBUTECONTAINERLIST
#include <ball_attributecontainerlist.h>
#endif

#ifndef INCLUDED_BALL_ATTRIBUTECONTEXT
#include <ball_attributecontext.h>
#endif

namespace BloombergLP {

namespace ball {

class AttributeContainer;

                        // ======================
                        // class ScopedAttributes
                        // ======================

class ScopedAttributes {
    // This class installs a 'AttributeContainer' object in the current
    // attribute context on construction, and removes it on destruction.  Note
    // that the 'AttributeContainer' supplied at construction must remain valid
    // and *unmodified* for the lifetime of this object.

    const AttributeContext::iterator  d_it ;   // refers to attributes

    // NOT IMPLEMENTED
    ScopedAttributes(const ScopedAttributes&);
    ScopedAttributes& operator=(const ScopedAttributes&);

  public:
    // CREATORS
    ScopedAttributes(const AttributeContainer* attributes);
        // Create a 'ScopedAttributes' object having the specified
        // 'attributes'.  Note that 'attributes' must remain valid and
        // *unmodified* for the lifetime of this object.

    ~ScopedAttributes();
        // Remove the associated attributes from the current attribute context.
};

// ============================================================================
//                              INLINE DEFINITIONS
// ============================================================================

                        // ----------------------
                        // class ScopedAttributes
                        // ----------------------

// CREATORS
inline
ScopedAttributes::ScopedAttributes(const AttributeContainer* attributes)
: d_it(AttributeContext::getContext()->addAttributes(attributes))
{
}

inline
ScopedAttributes::~ScopedAttributes()
{
    AttributeContext::getContext()->removeAttributes(d_it);
}

}  // close package namespace

}  // close enterprise namespace

#endif

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
