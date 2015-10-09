// bdlat_attributeinfo.h                                              -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BDLAT_ATTRIBUTEINFO
#define INCLUDED_BDLAT_ATTRIBUTEINFO

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a container for attribute information.
//
//@CLASSES:
//  bdlat_AttributeInfo: container for attribute information
//
//@SEE_ALSO: bdlat_sequencefunctions
//
//@DESCRIPTION: This component provides the 'bdlat_AttributeInfo' 'struct',
// which is a container for holding information (properties) about a sequence
// attribute.  The properties of an attribute include its name and the length
// of its name, its distinct id within its containing type, its formatting
// mode, and a brief annotation.  Although each attribute property is publicly
// accessible, a manipulator and accessor is also supplied for each.
//
// When accessing or manipulating an attribute of a "sequence" type (using one
// of the functions from the 'bdlat_SequenceFunctions' namespace), an instance
// of this 'struct' will be passed as the second argument to the accessor or
// manipulator.
//
// Note that this 'struct' needs to be a POD type.
//
///Usage
///-----
// The following snippets of code illustrate the usage of this component.
// Suppose you create an accessor for sequence attributes that prints each
// attribute to an output stream:
//..
//  #include <bdlat_attributeinfo.h>
//  #include <ostream>
//
//  using namespace BloombergLP;
//
//  class PrintAttributeWithInfo {
//      // Print the attribute along with its name and annotation.
//
//      // PRIVATE DATA MEMBERS
//      bsl::ostream *d_stream_p;
//
//    public:
//      // CREATORS
//      PrintAttributeWithInfo(bsl::ostream *stream)
//      : d_stream_p(stream)
//      {
//      }
//
//      // OPERATIONS
//      template <typename TYPE>
//      int operator()(const TYPE&                attribute,
//                     const bdlat_AttributeInfo& info)
//      {
//          (*d_stream_p) << attribute << " ("
//                        << bsl::string(info.name(),
//                                       info.nameLength())
//                        << ", "
//                        << info.annotation() << ")\n";
//      }
//  };
//..

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

#ifndef INCLUDED_BDLAT_BDEATOVERRIDES
#include <bdlat_bdeatoverrides.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITS
#include <bslalg_typetraits.h>
#endif

#ifndef INCLUDED_BSLMF_NESTEDTRAITDECLARATION
#include <bslmf_nestedtraitdeclaration.h>
#endif

#ifndef INCLUDED_BSL_CLIMITS
#include <bsl_climits.h>
#endif

#ifndef INCLUDED_BSL_CSTRING
#include <bsl_cstring.h>
#endif

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif


namespace BloombergLP {

                         // ==========================
                         // struct bdlat_AttributeInfo
                         // ==========================

struct bdlat_AttributeInfo {
    // This 'struct' holds information about an attribute.  Its data members
    // are 'public' by design so that instances may be statically initialized.

    // PUBLIC DATA -- DO *NOT* REORDER
    int         d_id;              // distinct id of attribute
    const char *d_name_p;          // name of attribute
    int         d_nameLength;      // length of attribute name (0-terminator
                                   // not included)
    const char *d_annotation_p;    // attribute annotation
    int         d_formattingMode;  // formatting mode

    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(bdlat_AttributeInfo,
                                   bsl::is_trivially_copyable);
    BSLMF_NESTED_TRAIT_DECLARATION(bdlat_AttributeInfo,
                                   bsl::is_trivially_default_constructible);

    // CREATORS

    // The following methods are not defined by design:
    //..
    //   bdlat_AttributeInfo();
    //   bdlat_AttributeInfo(const bdlat_AttributeInfo& original);
    //   ~bdlat_AttributeInfo();
    //..
    // The corresponding methods supplied by the compiler are sufficient.

    // MANIPULATORS

    // The following method is not defined by design:
    //..
    //   bdlat_AttributeInfo& operator=(const bdlat_AttributeInfo& rhs);
    //..
    // The assignment operator supplied by the compiler is sufficient.

    const char *& annotation();
        // Return a reference to the modifiable annotation of this attribute
        // info object.

    int& formattingMode();
        // Return a reference to the modifiable formatting mode of this
        // attribute info object.

    int& id();
        // Return a reference to the modifiable id of this attribute info
        // object.

    const char *& name();
        // Return a reference to the modifiable name of this attribute info
        // object.

    int& nameLength();
        // Return a reference to the modifiable length of the name of this
        // attribute info object.  Note that the 0-terminator is not included
        // in the length.

    // ACCESSORS
    const char *annotation() const;
        // Return the non-modifiable annotation of this attribute info object.

    int formattingMode() const;
        // Return the formatting mode of this attribute info object.

    int id() const;
        // Return the id of this attribute info object.

    const char *name() const;
        // Return the non-modifiable name of this attribute info object.

    int nameLength() const;
        // Return the length of the name of this attribute info object.  Note
        // that the 0-terminator is not included in the length.
};

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

// FREE OPERATORS
inline
bool operator==(const bdlat_AttributeInfo& lhs,
                const bdlat_AttributeInfo& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute info objects
    // have the same value, and 'false' otherwise.  Two attribute info objects
    // have the same value if each of their respective properties are
    // identical.

inline
bool operator!=(const bdlat_AttributeInfo& lhs,
                const bdlat_AttributeInfo& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute info objects do
    // not have the same value, and 'false' otherwise.  Two attribute info
    // objects do not have the same value if at least one of their respective
    // properties is not identical.

bsl::ostream& operator<<(bsl::ostream&              stream,
                         const bdlat_AttributeInfo& attributeInfo);
    // Write the value of the specified 'attributeInfo' to the specified
    // 'stream'.

// MANIPULATORS

inline
const char *& bdlat_AttributeInfo::annotation()
{
    return d_annotation_p;
}

inline
int& bdlat_AttributeInfo::formattingMode()
{
    return d_formattingMode;
}

inline
int& bdlat_AttributeInfo::id()
{
    return d_id;
}

inline
const char *& bdlat_AttributeInfo::name()
{
    return d_name_p;
}

inline
int& bdlat_AttributeInfo::nameLength()
{
    return d_nameLength;
}

// ACCESSORS

inline
const char *bdlat_AttributeInfo::annotation() const
{
    return d_annotation_p;
}

inline
int bdlat_AttributeInfo::formattingMode() const
{
    return d_formattingMode;
}

inline
int bdlat_AttributeInfo::id() const
{
    return d_id;
}

inline
const char *bdlat_AttributeInfo::name() const
{
    return d_name_p;
}

inline
int bdlat_AttributeInfo::nameLength() const
{
    return d_nameLength;
}

// FREE OPERATORS

inline
bool operator==(const bdlat_AttributeInfo& lhs, const bdlat_AttributeInfo& rhs)
{
    return lhs.formattingMode() == rhs.formattingMode()
        && lhs.id()             == rhs.id()
        && lhs.nameLength()     == rhs.nameLength()
        && 0 == bsl::memcmp(lhs.name(), rhs.name(), lhs.nameLength())
        && 0 == bsl::strcmp(lhs.annotation(), rhs.annotation());
}

inline
bool operator!=(const bdlat_AttributeInfo& lhs, const bdlat_AttributeInfo& rhs)
{
    return !(lhs == rhs);
}

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
