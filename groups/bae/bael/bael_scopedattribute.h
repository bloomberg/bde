// bael_scopedattribute.h   -*-C++-*-
#ifndef INCLUDED_BAEL_SCOPEDATTRIBUTE
#define INCLUDED_BAEL_SCOPEDATTRIBUTE

//@PURPOSE: Provide a scoped guard for a single BAEL attribute attributes
//
//@CLASSES:
//   bael_ScopedAttribute: single attribute scoped guard
//
//@SEE_ALSO: bael_attribute
//
//@AUTHOR: Henry Verschell (hverschell)
//
//@DESCRIPTION: This component defines a scoped BAEL attribute guard.  It
// defines a single attribute for the current thread while it is in scope.
//
///Usage
///-----
// Suppose that service requests for a fictional service with id '999' are
// handled asynchronously by the function below.  Creating an instance of this
// class will set BAEL attributes for any logging performed while the
// request is being processed:
//..
//  void handleServiceRequest(const Request& request)
//  {
//     BAEL_LOG_SET_CATEGORY("MY.SERVICE");
//
//     bael_ScopedAttribute attribute("request", request.selectionName());
//
//     BAEL_LOG_TRACE << "Handling request: " << request << BAEL_LOG_END;
//
//     // handle request here
//  }
//..
// Attribute "request" will be set in the calling thread and will affect
// publication of any BAEL messages for the lifetime of 'attribute'.

#ifndef INCLUDED_BAESCM_VERSION
#include <baescm_version.h>
#endif

#ifndef INCLUDED_BAEL_ATTRIBUTECONTAINER
#include <bael_attributecontainer.h>
#endif

#ifndef INCLUDED_BAEL_ATTRIBUTECONTEXT
#include <bael_attributecontext.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORMUTIL
#include <bsls_platformutil.h>
#endif

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif

#ifndef INCLUDED_BSL_STRING
#include <bsl_string.h>
#endif

namespace BloombergLP {

class bslma_Allocator;

                    // ====================================
                    // class bael_ScopedAttribute_Container
                    // ====================================

class bael_ScopedAttribute_Container : public bael_AttributeContainer {
    // This component-private class is a concrete implementation of the
    // 'bael_AttributeContainer' protocol for a single attribute.

    // DATA
    bael_Attribute d_attribute;

    // NOT IMPLEMENTED
    bael_ScopedAttribute_Container(const bael_ScopedAttribute_Container&);
    bael_ScopedAttribute_Container& operator=(
                                   const bael_ScopedAttribute_Container&);

  public:
    // CREATORS
    bael_ScopedAttribute_Container(const char               *name,
                                   const bsl::string&        value,
                                   bslma_Allocator          *basicAllocator);
    bael_ScopedAttribute_Container(const char               *name,
                                   int                       value);
    bael_ScopedAttribute_Container(const char               *name,
                                   bsls_PlatformUtil::Int64  value);
       // Create a BAEL attribute container holding a single rule, associating
       // the specified 'name' with the specified 'value'.  Optionally specify
       // a 'basicAllocator used to supply memory.  If 'basicAllocator' is 0 or
       // unspecified, the currently installed default allocator is used.

    virtual ~bael_ScopedAttribute_Container();
       // Destroy this object;

    // ACCESSORS
    virtual bool hasValue(const bael_Attribute& value) const;
       // Return 'true' if the specified 'value' is the same as the value held
       // in this container, and 'false' otherwise.

    virtual bsl::ostream& print(bsl::ostream& stream,
                                int           level = 0,
                                int           spacesPerLevel = 4) const;
        // Format this object to the specified output 'stream' at the (absolute
        // value of) the optionally specified indentation 'level' and return a
        // reference to 'stream'.  If 'level' is specified, optionally specify
        // 'spacesPerLevel', the number of spaces per indentation level for
        // this and all of its nested objects.  If 'level' is negative,
        // suppress indentation of the first line.  If 'spacesPerLevel' is
        // negative, format the entire output on one line, suppressing all but
        // the initial indentation (as governed by 'level').  If 'stream' is
        // not valid on entry, this operation has no effect.
};

                         // ==========================
                         // class bael_ScopedAttribute
                         // ==========================

class bael_ScopedAttribute {
    // Provides a scoped guard that sets BAEL attributes for "serviceId" and
    // "uuid" in the current thread.

    // DATA
    bael_ScopedAttribute_Container d_container;  // contains the attribute

    const bael_AttributeContext::iterator        // reference to attribute
                                   d_it;         // container

    // NOT IMPLEMENTED
    bael_ScopedAttribute(const bael_ScopedAttribute&);
    bael_ScopedAttribute& operator=(const bael_ScopedAttribute&);

  public:
    // CREATORS
    bael_ScopedAttribute(const char               *name,
                         const bsl::string&        value,
                         bslma_Allocator          *basicAllocator = 0);
    bael_ScopedAttribute(const char               *name,
                         int                       value);
    bael_ScopedAttribute(const char               *name,
                         bsls_PlatformUtil::Int64  value);
      // Set BAEL logging attributes for the current thread for the scope of
      // this object, associating the specified 'name' with the specified
      // 'value'.  Optionally specify a 'basicAllocator' used to supply memory.
      // If 'basicAllocator' is 0 or unspecified, the currently installed
      // default allocator is used.

    ~bael_ScopedAttribute();
      // Remove the attributes managed by this object from the BAEL system, and
      // destroy this object.
};

// ============================================================================
//                          INLINE FUNCTION DEFINITIONS
// ============================================================================

                    // ------------------------------------
                    // class bael_ScopedAttribute_Container
                    // ------------------------------------

// CREATORS
inline
bael_ScopedAttribute_Container::bael_ScopedAttribute_Container(
        const char         *name,
        const bsl::string&  value,
        bslma_Allocator    *basicAllocator)
: d_attribute(name, value.c_str(), basicAllocator)
{
}

inline
bael_ScopedAttribute_Container::bael_ScopedAttribute_Container(
        const char *name,
        int         value)
: d_attribute(name, value)
{
}

inline
bael_ScopedAttribute_Container::bael_ScopedAttribute_Container(
        const char               *name,
        bsls_PlatformUtil::Int64  value)
: d_attribute(name, value)
{
}

// ACCESSORS
inline
bool bael_ScopedAttribute_Container::hasValue(const bael_Attribute& value)
                                                                          const
{
    return d_attribute == value;
}

                         // --------------------------
                         // class bael_ScopedAttribute
                         // --------------------------

// CREATORS
inline
bael_ScopedAttribute::bael_ScopedAttribute(
        const char         *name,
        const bsl::string&  value,
        bslma_Allocator    *basicAllocator)
: d_container(name, value, basicAllocator)
, d_it(bael_AttributeContext::getContext()->addAttributes(&d_container))
{
}

inline
bael_ScopedAttribute::bael_ScopedAttribute(
        const char *name,
        int         value)
: d_container(name, value)
, d_it(bael_AttributeContext::getContext()->addAttributes(&d_container))
{
}

inline
bael_ScopedAttribute::bael_ScopedAttribute(
        const char               *name,
        bsls_PlatformUtil::Int64  value)
: d_container(name, value)
, d_it(bael_AttributeContext::getContext()->addAttributes(&d_container))
{
}

inline
bael_ScopedAttribute::~bael_ScopedAttribute()
{
    bael_AttributeContext::getContext()->removeAttributes(d_it);
}

}  // close namespace BloombergLP

#endif

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2009
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ------------------------------ END-OF-FILE ---------------------------------
