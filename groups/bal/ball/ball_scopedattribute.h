// ball_scopedattribute.h                                             -*-C++-*-
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
// This component is used to associate an attribute (a name-value pair) with
// the current thread context for use when writing log records for the current
// thread.  This context information can both be written to the log itself, and
// used as input when evaluating whether a particular log should be written.
// For more information on how to use this feature, please see the package
// level documentation and usage examples for "Log Attributes" and "Rule-Based
// Logging".
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Basic Usage of 'ball::ScopedAttribute'
///- - - - - - - - - - - - - - - - - - - - - - - - -
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
//     BALL_LOG_TRACE << "Handling request: " << request;
//
//     // handle request here
//  }
//..
// Attribute "request" will be set in the calling thread and will affect
// publication of any BALL messages for the lifetime of 'attribute'.

#include <balscm_version.h>

#include <ball_attribute.h>
#include <ball_attributecontainer.h>
#include <ball_attributecontainerlist.h>
#include <ball_attributecontext.h>

#include <bslma_allocator.h>
#include <bslma_stdallocator.h>
#include <bslma_usesbslmaallocator.h>

#include <bslmf_nestedtraitdeclaration.h>

#include <bsls_keyword.h>
#include <bsls_types.h>
#include <bsls_assert.h>

#include <bsl_iosfwd.h>
#include <bsl_string.h>

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
    // TYPES
    typedef bsl::allocator<char> allocator_type;

    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(ScopedAttribute_Container,
                                   bslma::UsesBslmaAllocator);

    // CREATORS
    ScopedAttribute_Container(
                        const char              *name,
                        const bsl::string_view&  value,
                        const allocator_type&    allocator = allocator_type());
    ScopedAttribute_Container(
                        const char              *name,
                        const char              *value,
                        const allocator_type&    allocator = allocator_type());
    ScopedAttribute_Container(
                        const char              *name,
                        int                      value,
                        const allocator_type&    allocator = allocator_type());
    ScopedAttribute_Container(
                        const char              *name,
                        long                     value,
                        const allocator_type&    allocator = allocator_type());
    ScopedAttribute_Container(
                        const char              *name,
                        long long                value,
                        const allocator_type&    allocator = allocator_type());
    ScopedAttribute_Container(
                        const char              *name,
                        unsigned int             value,
                        const allocator_type&    allocator = allocator_type());
    ScopedAttribute_Container(
                        const char              *name,
                        unsigned long            value,
                        const allocator_type&    allocator = allocator_type());
    ScopedAttribute_Container(
                        const char              *name,
                        unsigned long long       value,
                        const allocator_type&    allocator = allocator_type());
    ScopedAttribute_Container(
                        const char              *name,
                        const void              *value,
                        const allocator_type&    allocator = allocator_type());
        // Create a BALL attribute container holding a single rule, associating
        // the specified 'name' with the specified 'value'.  Optionally specify
        // an 'allocator' (e.g., the address of a 'bslma::Allocator' object) to
        // supply memory; otherwise, the default allocator is used.

    virtual ~ScopedAttribute_Container();
        // Destroy this object;

    // ACCESSORS
    bool hasValue(const Attribute& attribute) const BSLS_KEYWORD_OVERRIDE;
        // Return 'true' if the specified 'attribute' is the same as the value
        // held in this container, and 'false' otherwise.

    bsl::ostream& print(bsl::ostream& stream,
                        int           level = 0,
                        int           spacesPerLevel = 4)
                                                   const BSLS_KEYWORD_OVERRIDE;
        // Format this object to the specified output 'stream' at the (absolute
        // value of) the optionally specified indentation 'level' and return a
        // reference to 'stream'.  If 'level' is specified, optionally specify
        // 'spacesPerLevel', the number of spaces per indentation level for
        // this and all of its nested objects.  If 'level' is negative,
        // suppress indentation of the first line.  If 'spacesPerLevel' is
        // negative, format the entire output on one line, suppressing all but
        // the initial indentation (as governed by 'level').  If 'stream' is
        // not valid on entry, this operation has no effect.

    void visitAttributes(const bsl::function<void(const Attribute&)>& visitor)
                                                   const BSLS_KEYWORD_OVERRIDE;
        // Invoke the specified 'visitor' function for all attributes in this
        // container.

                                  // Aspects

    allocator_type get_allocator() const;
        // Return the allocator used by this object to supply memory.  Note
        // that if no allocator was supplied at construction the default
        // allocator in effect at construction is used.
};

                         // =====================
                         // class ScopedAttribute
                         // =====================

class ScopedAttribute {
    // This class provides a scoped guard that sets a single BALL attribute in
    // the current thread.

    // DATA
    ScopedAttribute_Container        d_container;  // contains the attribute

    const AttributeContext::iterator d_it;         // reference to attribute
                                                   // container

    // NOT IMPLEMENTED
    ScopedAttribute(const ScopedAttribute&);
    ScopedAttribute& operator=(const ScopedAttribute&);

  public:
    // TYPES
    typedef bsl::allocator<char> allocator_type;

    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(ScopedAttribute,
                                   bslma::UsesBslmaAllocator);

    // CREATORS
    ScopedAttribute(const char              *name,
                    const bsl::string_view&  value,
                    const allocator_type&    allocator = allocator_type());
    ScopedAttribute(const char              *name,
                    const char              *value,
                    const allocator_type&    allocator = allocator_type());
    ScopedAttribute(const char              *name,
                    int                      value,
                    const allocator_type&    allocator = allocator_type());
    ScopedAttribute(const char              *name,
                    long                     value,
                    const allocator_type&    allocator = allocator_type());
    ScopedAttribute(const char              *name,
                    long long                value,
                    const allocator_type&    allocator = allocator_type());
    ScopedAttribute(const char              *name,
                    unsigned int             value,
                    const allocator_type&    allocator = allocator_type());
    ScopedAttribute(const char              *name,
                    unsigned long            value,
                    const allocator_type&    allocator = allocator_type());
    ScopedAttribute(const char              *name,
                    unsigned long long       value,
                    const allocator_type&    allocator = allocator_type());
    ScopedAttribute(const char              *name,
                    const void              *value,
                    const allocator_type&    allocator = allocator_type());
        // Set BALL logging attributes for the current thread for the scope of
        // this object, associating the specified 'name' with the specified
        // 'value'.  Optionally specify an 'allocator' (e.g., the address of a
        // 'bslma::Allocator' object) to supply memory; otherwise, the default
        // allocator is used.

    ~ScopedAttribute();
        // Remove the attributes managed by this object from the BALL system,
        // and destroy this object.

                                  // Aspects

    allocator_type get_allocator() const;
        // Return the allocator used by this object to supply memory.  Note
        // that if no allocator was supplied at construction the default
        // allocator in effect at construction is used.
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
                                            const char              *name,
                                            const bsl::string_view&  value,
                                            const allocator_type&    allocator)
: d_attribute(name, value, allocator)
{
}

inline
ScopedAttribute_Container::ScopedAttribute_Container(
                                            const char              *name,
                                            const char              *value,
                                            const allocator_type&    allocator)
: d_attribute(name, static_cast<bsl::string>(value), allocator)
{
}

inline
ScopedAttribute_Container::ScopedAttribute_Container(
                                             const char             *name,
                                             int                     value,
                                             const allocator_type&   allocator)
: d_attribute(name, value, allocator)
{
}

inline
ScopedAttribute_Container::ScopedAttribute_Container(
                                              const char            *name,
                                              long                   value,
                                              const allocator_type&  allocator)
: d_attribute(name, value, allocator)
{
}

inline
ScopedAttribute_Container::ScopedAttribute_Container(
                                              const char            *name,
                                              long long              value,
                                              const allocator_type&  allocator)
: d_attribute(name, value, allocator)
{
}

inline
ScopedAttribute_Container::ScopedAttribute_Container(
                                             const char             *name,
                                             unsigned int            value,
                                             const allocator_type&   allocator)
: d_attribute(name, value, allocator)
{
}

inline
ScopedAttribute_Container::ScopedAttribute_Container(
                                             const char             *name,
                                             unsigned long           value,
                                             const allocator_type&   allocator)
: d_attribute(name, value, allocator)
{
}

inline
ScopedAttribute_Container::ScopedAttribute_Container(
                                             const char             *name,
                                             unsigned long long      value,
                                             const allocator_type&   allocator)
: d_attribute(name, value, allocator)
{
}

inline
ScopedAttribute_Container::ScopedAttribute_Container(
                                            const char              *name,
                                            const void              *value,
                                            const allocator_type&    allocator)
: d_attribute(name, value, allocator)
{
}

// ACCESSORS
inline
bool ScopedAttribute_Container::hasValue(const Attribute& attribute) const
{
    return d_attribute == attribute;
}

inline
void ScopedAttribute_Container::visitAttributes(
              const bsl::function<void(const ball::Attribute&)>& visitor) const
{
    visitor(d_attribute);
}

                                  // Aspects

inline
ScopedAttribute_Container::allocator_type
ScopedAttribute_Container::get_allocator() const
{
    return d_attribute.get_allocator();
}

                         // ---------------------
                         // class ScopedAttribute
                         // ---------------------

// CREATORS
inline
ScopedAttribute::ScopedAttribute(const char              *name,
                                 const bsl::string_view&  value,
                                 const allocator_type&    allocator)
: d_container(name, value, allocator)
, d_it(AttributeContext::getContext()->addAttributes(&d_container))
{
}

inline
ScopedAttribute::ScopedAttribute(const char              *name,
                                 const char              *value,
                                 const allocator_type&    allocator)
: d_container(name, value, allocator)
, d_it(AttributeContext::getContext()->addAttributes(&d_container))
{
}

inline
ScopedAttribute::ScopedAttribute(const char            *name,
                                 int                    value,
                                 const allocator_type&  allocator)
: d_container(name, value, allocator)
, d_it(AttributeContext::getContext()->addAttributes(&d_container))
{
}

inline
ScopedAttribute::ScopedAttribute(const char            *name,
                                 long                   value,
                                 const allocator_type&  allocator)
: d_container(name, value, allocator)
, d_it(AttributeContext::getContext()->addAttributes(&d_container))
{
}

inline
ScopedAttribute::ScopedAttribute(const char            *name,
                                 long long              value,
                                 const allocator_type&  allocator)
: d_container(name, value, allocator)
, d_it(AttributeContext::getContext()->addAttributes(&d_container))
{
}

inline
ScopedAttribute::ScopedAttribute(const char            *name,
                                 unsigned int           value,
                                 const allocator_type&  allocator)
: d_container(name, value, allocator)
, d_it(AttributeContext::getContext()->addAttributes(&d_container))
{
}

inline
ScopedAttribute::ScopedAttribute(const char            *name,
                                 unsigned long          value,
                                 const allocator_type&  allocator)
: d_container(name, value, allocator)
, d_it(AttributeContext::getContext()->addAttributes(&d_container))
{
}

inline
ScopedAttribute::ScopedAttribute(const char            *name,
                                 unsigned long long     value,
                                 const allocator_type&  allocator)
: d_container(name, value, allocator)
, d_it(AttributeContext::getContext()->addAttributes(&d_container))
{
}

inline
ScopedAttribute::ScopedAttribute(const char              *name,
                                 const void              *value,
                                 const allocator_type&    allocator)
: d_container(name, value, allocator)
, d_it(AttributeContext::getContext()->addAttributes(&d_container))
{
}

inline
ScopedAttribute::~ScopedAttribute()
{
    AttributeContext::getContext()->removeAttributes(d_it);
}

// ACCESSORS

                                  // Aspects

inline
ScopedAttribute::allocator_type
ScopedAttribute::get_allocator() const
{
    return d_container.get_allocator();
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
