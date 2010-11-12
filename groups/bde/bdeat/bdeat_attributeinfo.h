// bdeat_attributeinfo.h                  -*-C++-*-
#ifndef INCLUDED_BDEAT_ATTRIBUTEINFO
#define INCLUDED_BDEAT_ATTRIBUTEINFO

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a container for attribute information.
//
//@CLASSES:
//  bdeat_AttributeInfo: container for attribute information
//
//@SEE_ALSO: bdeat_sequencefunctions
//
//@AUTHOR: Clay Wilson (cwilson9)
//
//@DESCRIPTION: This component provides the 'bdeat_AttributeInfo' 'struct',
// which is a container for holding information (properties) about a sequence
// attribute.  The properties of an attribute include its name and the length
// of its name, its distinct id within its containing type, its formatting
// mode, and a brief annotation.  Although each attribute property is publicly
// accessible, a manipulator and accessor is also supplied for each.
//
// When accessing or manipulating an attribute of a "sequence" type (using one
// of the functions from the 'bdeat_SequenceFunctions' namespace), an instance
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
//  #include <bdeat_attributeinfo.h>
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
//                     const bdeat_AttributeInfo& info)
//      {
//          (*d_stream_p) << attribute << " ("
//                        << bsl::string(info.name(),
//                                       info.nameLength())
//                        << ", "
//                        << info.annotation() << ")\n";
//      }
//  };
//..

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITS
#include <bslalg_typetraits.h>
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
                        // struct bdeat_AttributeInfo
                        // ==========================

struct bdeat_AttributeInfo {
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
    BSLALG_DECLARE_NESTED_TRAITS(bdeat_AttributeInfo,
                                 bslalg_TypeTraitsGroupPod);

    // CREATORS
    // The following methods are not defined by design:
    //..
    //   bdeat_AttributeInfo();
    //   bdeat_AttributeInfo(const bdeat_AttributeInfo& original);
    //   ~bdeat_AttributeInfo();
    //..
    // The corresponding methods supplied by the compiler are sufficient.

    // MANIPULATORS
    // The following method is not defined by design:
    //..
    //   bdeat_AttributeInfo& operator=(const bdeat_AttributeInfo& rhs);
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

// FREE OPERATORS
inline
bool operator==(const bdeat_AttributeInfo& lhs,
                                               const bdeat_AttributeInfo& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute info objects
    // have the same value, and 'false' otherwise.  Two attribute info objects
    // have the same value if each of their respective properties are
    // identical.

inline
bool operator!=(const bdeat_AttributeInfo& lhs,
                                               const bdeat_AttributeInfo& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute info objects do
    // not have the same value, and 'false' otherwise.  Two attribute info
    // objects do not have the same value if at least one of their respective
    // properties is not identical.

bsl::ostream& operator<<(bsl::ostream&              stream,
                         const bdeat_AttributeInfo& attributeInfo);
    // Write the value of the specified 'attributeInfo' to the specified
    // 'stream'.

// ===========================================================================
//                      INLINE FUNCTION DEFINITIONS
// ===========================================================================

// MANIPULATORS

inline
const char *& bdeat_AttributeInfo::annotation()
{
    return d_annotation_p;
}

inline
int& bdeat_AttributeInfo::formattingMode()
{
    return d_formattingMode;
}

inline
int& bdeat_AttributeInfo::id()
{
    return d_id;
}

inline
const char *& bdeat_AttributeInfo::name()
{
    return d_name_p;
}

inline
int& bdeat_AttributeInfo::nameLength()
{
    return d_nameLength;
}

// ACCESSORS

inline
const char *bdeat_AttributeInfo::annotation() const
{
    return d_annotation_p;
}

inline
int bdeat_AttributeInfo::formattingMode() const
{
    return d_formattingMode;
}

inline
int bdeat_AttributeInfo::id() const
{
    return d_id;
}

inline
const char *bdeat_AttributeInfo::name() const
{
    return d_name_p;
}

inline
int bdeat_AttributeInfo::nameLength() const
{
    return d_nameLength;
}

// FREE OPERATORS

inline
bool operator==(const bdeat_AttributeInfo& lhs, const bdeat_AttributeInfo& rhs)
{
    return lhs.formattingMode() == rhs.formattingMode()
        && lhs.id()             == rhs.id()
        && lhs.nameLength()     == rhs.nameLength()
        && 0 == bsl::memcmp(lhs.name(), rhs.name(), lhs.nameLength())
        && 0 == bsl::strcmp(lhs.annotation(), rhs.annotation());
}

inline
bool operator!=(const bdeat_AttributeInfo& lhs, const bdeat_AttributeInfo& rhs)
{
    return !(lhs == rhs);
}

}  // close namespace BloombergLP



#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2005
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
