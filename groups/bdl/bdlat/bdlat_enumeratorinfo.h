// bdlat_enumeratorinfo.h                                             -*-C++-*-
#ifndef INCLUDED_BDLAT_ENUMERATORINFO
#define INCLUDED_BDLAT_ENUMERATORINFO

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a container for enumerator information.
//
//@CLASSES:
//   bdlat_EnumeratorInfo: container for enumerator information
//
//@SEE_ALSO:
//
//@DESCRIPTION:
// This component provides the `bdlat_EnumeratorInfo` `class` which is a
// container for holding information (properties) about an enumerator.  The
// properties of an enumerator include its name and the length of its name, its
// value, and a brief annotation.  Although each enumerator property is
// publicly accessible, a manipulator and accessor is also supplied for each.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Basic Usage
/// - - - - - - - - - - -
// TBD doc

#include <bdlscm_version.h>

#include <bdlat_bdeatoverrides.h>

#include <bslmf_istriviallycopyable.h>
#include <bslmf_istriviallydefaultconstructible.h>
#include <bslmf_nestedtraitdeclaration.h>

#include <bsls_keyword.h>

#include <bsl_climits.h>
#include <bsl_cstring.h>
#include <bsl_iosfwd.h>

#ifndef BDE_DONT_ALLOW_TRANSITIVE_INCLUDES
#include <bslalg_typetraits.h>
#endif // BDE_DONT_ALLOW_TRANSITIVE_INCLUDES


namespace BloombergLP {

                        // ===========================
                        // struct bdlat_EnumeratorInfo
                        // ===========================

/// This `struct` holds information about an enumerator.  Its data members
/// are `public` by design so that instances may be statically initialized.
struct bdlat_EnumeratorInfo {

    // PUBLIC DATA -- DO *NOT* REORDER
    int         d_value;         // value of enumerator
    const char *d_name_p;        // name of enumerator
    int         d_nameLength;    // length of enumerator name (0-terminator not
                                 // included)
    const char *d_annotation_p;  // enumerator annotation

    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(bdlat_EnumeratorInfo,
                                   bsl::is_trivially_copyable);
    BSLMF_NESTED_TRAIT_DECLARATION(bdlat_EnumeratorInfo,
                                   bsl::is_trivially_default_constructible);

    // CREATORS

    /// Compiler-generated methods.
    //! bdlat_EnumeratorInfo() = default;
    //! bdlat_EnumeratorInfo(const bdlat_EnumeratorInfo& original) = default;
    //! ~bdlat_EnumeratorInfo() = default;

    // MANIPULATORS

    /// Compiler-generated methods.
    //! bdlat_EnumeratorInfo& operator=(const bdlat_EnumeratorInfo& rhs);

    /// Return a reference to the modifiable annotation of this enumerator
    /// info object.
    const char *& annotation();

    /// Return a reference to the modifiable name of this enumerator info
    /// object.
    const char *& name();

    /// Return a reference to the modifiable length of the name of this
    /// enumerator info object.  Note that the 0-terminator is not included
    /// in the length.
    int& nameLength();

    /// Return a reference to the modifiable id of this enumerator info
    /// object.
    int& value();

    // ACCESSORS

    /// Return the non-modifiable annotation of this enumerator info object.
    BSLS_KEYWORD_CONSTEXPR
    const char *annotation() const;

    /// Return the non-modifiable name of this enumerator info object.
    BSLS_KEYWORD_CONSTEXPR
    const char *name() const;

    /// Return the length of the name of this enumerator info object.  Note
    /// that the 0-terminator is not included in the length.
    BSLS_KEYWORD_CONSTEXPR
    int nameLength() const;

    /// Return the index of this enumerator info object.
    BSLS_KEYWORD_CONSTEXPR
    int value() const;
};

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

// FREE OPERATORS

/// Return `true` if the specified `lhs` and `rhs` enumerator info objects
/// have the same value, and `false` otherwise.  Two enumerator info objects
/// have the same value if each of their respective properties are
/// identical.
inline
bool operator==(const bdlat_EnumeratorInfo& lhs,
                const bdlat_EnumeratorInfo& rhs);

/// Return `true` if the specified `lhs` and `rhs` enumerator info objects
/// do not have the same value, and `false` otherwise.  Two enumerator info
/// objects do not have the same value if at least one of their respective
/// properties is not identical.
inline
bool operator!=(const bdlat_EnumeratorInfo& lhs,
                const bdlat_EnumeratorInfo& rhs);

/// Write the value of the specified `enumeratorInfo` to the specified
/// `stream`.
bsl::ostream& operator<<(bsl::ostream&               stream,
                         const bdlat_EnumeratorInfo& enumeratorInfo);

// MANIPULATORS

inline
const char *& bdlat_EnumeratorInfo::annotation()
{
    return d_annotation_p;
}

inline
const char *& bdlat_EnumeratorInfo::name()
{
    return d_name_p;
}

inline
int& bdlat_EnumeratorInfo::nameLength()
{
    return d_nameLength;
}

inline
int& bdlat_EnumeratorInfo::value()
{
    return d_value;
}

// ACCESSORS

inline
BSLS_KEYWORD_CONSTEXPR
const char *bdlat_EnumeratorInfo::annotation() const
{
    return d_annotation_p;
}

inline
BSLS_KEYWORD_CONSTEXPR
const char *bdlat_EnumeratorInfo::name() const
{
    return d_name_p;
}

inline
BSLS_KEYWORD_CONSTEXPR
int bdlat_EnumeratorInfo::nameLength() const
{
    return d_nameLength;
}

inline
BSLS_KEYWORD_CONSTEXPR
int bdlat_EnumeratorInfo::value() const
{
    return d_value;
}

// FREE OPERATORS

inline
bool operator==(const bdlat_EnumeratorInfo& lhs,
                const bdlat_EnumeratorInfo& rhs)
{
    return lhs.value()      == rhs.value()
        && lhs.nameLength() == rhs.nameLength()
        && 0 == bsl::memcmp(lhs.name(), rhs.name(), lhs.nameLength())
        && 0 == bsl::strcmp(lhs.annotation(), rhs.annotation());
}

inline
bool operator!=(const bdlat_EnumeratorInfo& lhs,
                const bdlat_EnumeratorInfo& rhs)
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
