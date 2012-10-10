// bdeat_selectioninfo.h                  -*-C++-*-
#ifndef INCLUDED_BDEAT_SELECTIONINFO
#define INCLUDED_BDEAT_SELECTIONINFO

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a container for selection information.
//
//@CLASSES:
//  bdeat_SelectionInfo: container for selection information
//
//@SEE_ALSO: bdeat_choicefunctions
//
//@AUTHOR: Clay Wilson (cwilson9)
//
//@DESCRIPTION: This component provides the 'bdeat_SelectionInfo' 'struct',
// which is a container for holding information (properties) about a choice
// selection.  The properties of a selection include its name and the length of
// its name, its distinct id within its containing type, its formatting mode,
// and a brief annotation.  Although each selection property is publicly
// accessible, a manipulator and accessor is also supplied for each.
//
// When accessing or manipulating a selection of a "choice" type (using one of
// the functions from the 'bdeat_ChoiceFunctions' namespace), an instance of
// this 'struct' will be passed as the second argument to the accessor or
// manipulator.
//
// Note that this 'struct' needs to be a POD type.
//
///Usage
///-----
// The following snippets of code illustrate the usage of this component.
// Suppose you create an accessor for choice selections that prints the
// selection to an output stream:
//..
//  #include <bdeat_selectioninfo.h>
//  #include <ostream>
//
//  using namespace BloombergLP;
//
//  class PrintSelectionWithInfo {
//      // Print the selection along with its name and annotation.
//
//      // PRIVATE DATA MEMBERS
//      bsl::ostream *d_stream_p;
//
//    public:
//      // CREATORS
//      PrintSelectionWithInfo(bsl::ostream *stream)
//      : d_stream_p(stream)
//      {
//      }
//
//      // OPERATIONS
//      template <typename TYPE>
//      int operator()(const TYPE&                selection,
//                     const bdeat_SelectionInfo& info)
//      {
//          (*d_stream_p) << selection << " ("
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
                        // struct bdeat_SelectionInfo
                        // ==========================

struct bdeat_SelectionInfo {
    // This 'struct' holds information about an selection.  Its data members
    // are 'public' by design so that instances may be statically initialized.

    // PUBLIC DATA -- DO *NOT* REORDER
    int         d_id;              // distinct id of selection
    const char *d_name_p;          // name of selection
    int         d_nameLength;      // length of selection name (0-terminator
                                   // not included)
    const char *d_annotation_p;    // selection annotation
    int         d_formattingMode;  // formatting mode

    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(bdeat_SelectionInfo,
                                   bsl::is_trivially_copyable);
    BSLMF_NESTED_TRAIT_DECLARATION(bdeat_SelectionInfo,
                                   bsl::is_trivially_default_constructible);

    // CREATORS
    // The following methods are not defined by design:
    //..
    //   bdeat_SelectionInfo();
    //   bdeat_SelectionInfo(const bdeat_SelectionInfo& original);
    //   ~bdeat_SelectionInfo();
    //..
    // The corresponding methods supplied by the compiler are sufficient.

    // MANIPULATORS
    // The following method is not defined by design:
    //..
    //   bdeat_SelectionInfo& operator=(const bdeat_SelectionInfo& rhs);
    //..
    // The assignment operator supplied by the compiler is sufficient.

    const char *& annotation();
        // Return a reference to the modifiable annotation of this selection
        // info object.

    int& formattingMode();
        // Return a reference to the modifiable formatting mode of this
        // selection info object.

    int& id();
        // Return a reference to the modifiable id of this selection info
        // object.

    const char *& name();
        // Return a reference to the modifiable name of this selection info
        // object.

    int& nameLength();
        // Return a reference to the modifiable length of the name of this
        // selection info object.  Note that the 0-terminator is not included
        // in the length.

    // ACCESSORS
    const char *annotation() const;
        // Return the non-modifiable annotation of this selection info object.

    int formattingMode() const;
        // Return the formatting mode of this selection info object.

    int id() const;
        // Return the id of this selection info object.

    const char *name() const;
        // Return the non-modifiable name of this selection info object.

    int nameLength() const;
        // Return the length of the name of this selection info object.  Note
        // that the 0-terminator is not included in the length.
};

// FREE OPERATORS
inline
bool operator==(const bdeat_SelectionInfo& lhs,
                                               const bdeat_SelectionInfo& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' selection info objects
    // have the same value, and 'false' otherwise.  Two selection info objects
    // have the same value if each of their respective properties are
    // identical.

inline
bool operator!=(const bdeat_SelectionInfo& lhs,
                                               const bdeat_SelectionInfo& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' selection info objects do
    // not have the same value, and 'false' otherwise.  Two selection info
    // objects do not have the same value if at least one of their respective
    // properties is not identical.

bsl::ostream& operator<<(bsl::ostream&              stream,
                         const bdeat_SelectionInfo& selectionInfo);
    // Write the value of the specified 'selectionInfo' to the specified
    // 'stream'.

// ===========================================================================
//                      INLINE FUNCTION DEFINITIONS
// ===========================================================================

// MANIPULATORS

inline
const char *& bdeat_SelectionInfo::annotation()
{
    return d_annotation_p;
}

inline
int& bdeat_SelectionInfo::formattingMode()
{
    return d_formattingMode;
}

inline
int& bdeat_SelectionInfo::id()
{
    return d_id;
}

inline
const char *& bdeat_SelectionInfo::name()
{
    return d_name_p;
}

inline
int& bdeat_SelectionInfo::nameLength()
{
    return d_nameLength;
}

// ACCESSORS

inline
const char *bdeat_SelectionInfo::annotation() const
{
    return d_annotation_p;
}

inline
int bdeat_SelectionInfo::formattingMode() const
{
    return d_formattingMode;
}

inline
int bdeat_SelectionInfo::id() const
{
    return d_id;
}

inline
const char *bdeat_SelectionInfo::name() const
{
    return d_name_p;
}

inline
int bdeat_SelectionInfo::nameLength() const
{
    return d_nameLength;
}

// FREE OPERATORS

inline
bool operator==(const bdeat_SelectionInfo& lhs, const bdeat_SelectionInfo& rhs)
{
    return lhs.formattingMode() == rhs.formattingMode()
        && lhs.id()             == rhs.id()
        && lhs.nameLength()     == rhs.nameLength()
        && 0 == bsl::strncmp(lhs.name(), rhs.name(), lhs.nameLength())
        && 0 == bsl::strcmp(lhs.annotation(), rhs.annotation());
}

inline
bool operator!=(const bdeat_SelectionInfo& lhs, const bdeat_SelectionInfo& rhs)
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
