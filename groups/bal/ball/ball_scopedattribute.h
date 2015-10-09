// ball_scopedattribute.h                                             -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BALL_SCOPEDATTRIBUTE
#define INCLUDED_BALL_SCOPEDATTRIBUTE

//@PURPOSE: Provide a scoped guard for a single BALL attribute.
//
//@CLASSES:
//  ball::ScopedAttribute: single attribute scoped guard
//
//@SEE_ALSO: ball_attribute
//
//@DESCRIPTION: This component defines a type, 'ball::ScopedAttribute', that
// serves as a scoped guard for 'ball::Attribute' objects.  It defines a single
// attribute for the current thread while it is in scope.
//
///Usage
///-----
// Suppose that service requests for a fictional service with id '999' are
// handled asynchronously by the function below.  Creating an instance of this
// class will set BALL attributes for any logging performed while the request
// is being processed:
//..
//  void handleServiceRequest(const Request& request)
//  {
//     BALL_LOG_SET_CATEGORY("MY.SERVICE");
//
//     ball::ScopedAttribute attribute("request", request.selectionName());
//
//     BALL_LOG_TRACE << "Handling request: " << request << BALL_LOG_END;
//
//     // handle request here
//  }
//..
// Attribute "request" will be set in the calling thread and will affect
// publication of any BALL messages for the lifetime of 'attribute'.

#ifndef INCLUDED_BALSCM_VERSION
#include <balscm_version.h>
#endif

#ifndef INCLUDED_BALL_ATTRIBUTE
#include <ball_attribute.h>
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

#ifndef INCLUDED_BSLMA_ALLOCATOR
#include <bslma_allocator.h>
#endif

#ifndef INCLUDED_BSLS_TYPES
#include <bsls_types.h>
#endif

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif

#ifndef INCLUDED_BSL_STRING
#include <bsl_string.h>
#endif

namespace BloombergLP {

namespace ball {
                    // ===============================
                    // class ScopedAttribute_Container
                    // ===============================

class ScopedAttribute_Container : public AttributeContainer {
    // This component-private class is a concrete implementation of the
    // 'AttributeContainer' protocol for a single attribute.

    // DATA
    Attribute d_attribute;

    // NOT IMPLEMENTED
    ScopedAttribute_Container(const ScopedAttribute_Container&);
    ScopedAttribute_Container& operator=(const ScopedAttribute_Container&);

  public:
    // CREATORS
    ScopedAttribute_Container(const char         *name,
                              const bsl::string&  value,
                              bslma::Allocator   *basicAllocator = 0);
    ScopedAttribute_Container(const char         *name,
                              int                 value,
                              bslma::Allocator   *basicAllocator = 0);
    ScopedAttribute_Container(const char         *name,
                              bsls::Types::Int64  value,
                              bslma::Allocator   *basicAllocator = 0);
        // Create a BALL attribute container holding a single rule, associating
        // the specified 'name' with the specified 'value'.  Optionally specify
        // a 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0
        // or unspecified, the currently installed default allocator is used.


    virtual ~ScopedAttribute_Container();
        // Destroy this object;

    // ACCESSORS
    virtual bool hasValue(const Attribute& value) const;
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

                         // =====================
                         // class ScopedAttribute
                         // =====================

class ScopedAttribute {
    // Provides a scoped guard that sets BALL attributes for "serviceId" and
    // "uuid" in the current thread.

    // DATA
    ScopedAttribute_Container        d_container;  // contains the attribute

    const AttributeContext::iterator d_it;         // reference to attribute
                                                   // container

    // NOT IMPLEMENTED
    ScopedAttribute(const ScopedAttribute&);
    ScopedAttribute& operator=(const ScopedAttribute&);

  public:
    // CREATORS
    ScopedAttribute(const char         *name,
                    const bsl::string&  value,
                    bslma::Allocator   *basicAllocator = 0);
    ScopedAttribute(const char         *name,
                    int                 value,
                    bslma::Allocator   *basicAllocator = 0);
    ScopedAttribute(const char         *name,
                    bsls::Types::Int64  value,
                    bslma::Allocator   *basicAllocator = 0);
        // Set BALL logging attributes for the current thread for the scope of
        // this object, associating the specified 'name' with the specified
        // 'value'.  Optionally specify a 'basicAllocator' used to supply
        // memory.  If 'basicAllocator' is 0 or unspecified, the currently
        // installed default allocator is used.

    ~ScopedAttribute();
        // Remove the attributes managed by this object from the BALL system,
        // and destroy this object.

};

// ============================================================================
//                              INLINE DEFINITIONS
// ============================================================================

                    // -------------------------------
                    // class ScopedAttribute_Container
                    // -------------------------------

// CREATORS
inline
ScopedAttribute_Container::ScopedAttribute_Container(
                                            const char         *name,
                                            const bsl::string&  value,
                                            bslma::Allocator   *basicAllocator)
: d_attribute(name, value.c_str(), basicAllocator)
{
}

inline
ScopedAttribute_Container::ScopedAttribute_Container(
                                            const char         *name,
                                            int                 value,
                                            bslma::Allocator   *basicAllocator)
    : d_attribute(name, value, basicAllocator)
{
}

inline
ScopedAttribute_Container::ScopedAttribute_Container(
                                            const char         *name,
                                            bsls::Types::Int64  value,
                                            bslma::Allocator   *basicAllocator)
: d_attribute(name, value, basicAllocator)
{
}

// ACCESSORS
inline
bool ScopedAttribute_Container::hasValue(const Attribute& value) const
{
    return d_attribute == value;
}

                         // ---------------------
                         // class ScopedAttribute
                         // ---------------------

// CREATORS
inline
ScopedAttribute::ScopedAttribute(const char         *name,
                                 const bsl::string&  value,
                                 bslma::Allocator   *basicAllocator)
: d_container(name, value, basicAllocator)
, d_it(AttributeContext::getContext()->addAttributes(&d_container))
{
}

inline
ScopedAttribute::ScopedAttribute(const char         *name,
                                 int                 value,
                                 bslma::Allocator   *basicAllocator)
: d_container(name, value, basicAllocator)
, d_it(AttributeContext::getContext()->addAttributes(&d_container))
{
}

inline
ScopedAttribute::ScopedAttribute(const char         *name,
                                 bsls::Types::Int64  value,
                                 bslma::Allocator   *basicAllocator)
: d_container(name, value, basicAllocator)
, d_it(AttributeContext::getContext()->addAttributes(&d_container))
{
}

inline
ScopedAttribute::~ScopedAttribute()
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
