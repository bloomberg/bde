// balcl_optioninfo.h                                                 -*-C++-*-
#ifndef INCLUDED_BALCL_OPTIONINFO
#define INCLUDED_BALCL_OPTIONINFO

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a POD command-line-option descriptor `struct`.
//
//@CLASSES:
//   balcl::OptionInfo: POD `struct` that describes a command-line option
//
//@SEE_ALSO: balcl_option, balcl_commandline
//
//@DESCRIPTION: This component provides a `struct`, `balcl::OptionInfo`, that
// describes a command-line option.  The `balcl::OptionInfo` `struct` is used
// to specify the user-defined command-line options accepted by a
// `balcl::CommandLine` object.  This type is typically used when one wants to
// statically initialize an array of option specifications.  When an
// allocator-aware, full-featured value-semantic class is needed to describe
// command-line options, use `balcl::Option`.
//
// For further details see {`balcl_commandline`|Specifying Command-Line
// Arguments}.
//
///Usage
///-----
// The intended use of this component is illustrated in
// {`balcl_commandline`|Usage}.

#include <balscm_version.h>

#include <balcl_typeinfo.h>
#include <balcl_occurrenceinfo.h>

#include <bsls_compilerfeatures.h>

#include <bsl_iosfwd.h>
#include <bsl_string.h>

// Optioninfo is intended to be aggregate-initialized.  This means that in
// C++03, it must not have constructors declared.  Declaring the constructors
// is useful in C++11 and beyond because it avoids warnings when later fields
// are not initialized by the aggregate.

#undef   BALCL_OPTIONINFO_HAS_CONSTRUCTORS
#if 201103L <= BSLS_COMPILERFEATURES_CPLUSPLUS
# define BALCL_OPTIONINFO_HAS_CONSTRUCTORS
#endif

namespace BloombergLP {
namespace balcl {

                        // =================
                        // struct OptionInfo
                        // =================

/// This `struct` is a simple attribute class that describes the information
/// associated with an option, namely the associated tag (as a string, from
/// which the short and long tags are extracted), the option name, the
/// description used in printing usage, and optional associated `TypeInfo`
/// and `OccurrenceInfo` objects.
///
/// By design, this `struct` does not have any user-defined constructors, so
/// there is no provision for passing an allocator to its data members (all
/// of which take an allocator).  Consequently, all instances of this class
/// use the default allocator.  If proper allocator propagation is desired
/// (e.g., for storage within an allocator-aware container for which the use
/// of the default allocator is counter-indicated), one may use `Option`,
/// which is both allocator-aware and constructible from `OptionInfo`.
///
/// The main purpose of this `struct` is to provide a type whose values can
/// be statically-initialized.  For example:
/// ```
/// const balcl::OptionInfo OPTIONS[] = {
///    {
///      "s|longTag",                        // s(hortTag)
///      "optionName",
///      "option description",
///      balcl::TypeInfo(/* . . . */),       // optional
///      balcl::OccurrenceInfo(/* . . . */)  // optional
///    },
///    // ...
/// };
/// ```
/// Note that each of the first three fields can be default-constructed, and
/// thus omitted in such a declaration; however, such an object will be of
/// limited use because, to avoid undefined behavior, the constructor of
/// `balcl::CommandLine` requires that each of these fields be acceptable to
/// the `isDescriptionValid`, `isNameValid`, and `isTagValid` methods of
/// `balcl::Option`.  The default string value is not acceptable to any of
/// those methods.  See the {Usage} section for an example of such
/// initialization.
struct OptionInfo {

    // TYPES
    enum ArgType {
        // Enumerate the categories of command-line arguments.

        e_FLAG       = 0,  // boolean option (present on command line, or not)
        e_OPTION     = 1,  // option having a value
        e_NON_OPTION = 2   // other command-line argument
    };

    // PUBLIC DATA
    bsl::string    d_tag;          // tags (or "" for non-option argument)

    bsl::string    d_name;         // accessing name

    bsl::string    d_description;  // description used in printing usage

    TypeInfo       d_typeInfo;     // Optional field.  Within that,
                                   // - (optional) type/variable to be linked,
                                   // - (optional) constraint

    OccurrenceInfo d_defaultInfo;  // Optional -- two sub-parts:
                                   // - whether the option is required,
                                   //    optional, or hidden (default is
                                   //    optional)
                                   // - optionally, a default value.

    bsl::string    d_environmentVariableName;
                                   // Optional -- environment variable name

#ifdef BALCL_OPTIONINFO_HAS_CONSTRUCTORS
    // CREATORS

    /// Create an `OptionInfo` with the specified `tag`, `name`, and
    /// `description`, with `d_typeInfo`, `d_defaultInfo`, and
    /// `d_environmentVariableName` default-constructed.
    OptionInfo(bsl::string_view      tag = "",
               bsl::string_view      name = "",
               bsl::string_view      description = "");             // IMPLICIT

    /// Create an `OptionInfo` with the specified `tag`, `name`,
    /// `description`, and `typeInfo`, with `d_defaultInfo`, and
    /// `d_environmentVariableName` default-constructed.
    OptionInfo(bsl::string_view      tag,
               bsl::string_view      name,
               bsl::string_view      description,
               const TypeInfo&       typeInfo);

    /// Create an `OptionInfo` with the specified `tag`, `name`,
    /// `description`, `typeInfo`, `defaultInfo`, and `envVarName`.
    OptionInfo(bsl::string_view      tag,
               bsl::string_view      name,
               bsl::string_view      description,
               const TypeInfo&       typeInfo,
               const OccurrenceInfo& defaultInfo,
               bsl::string_view      envVarName = "");
#endif
};

#ifdef BALCL_OPTIONINFO_HAS_CONSTRUCTORS
// CREATORS
inline
OptionInfo::OptionInfo(bsl::string_view      tag,
                       bsl::string_view      name,
                       bsl::string_view      description)
: d_tag(tag)
, d_name(name)
, d_description(description)
, d_typeInfo()
, d_defaultInfo()
, d_environmentVariableName()
{
}

inline
OptionInfo::OptionInfo(bsl::string_view      tag,
                       bsl::string_view      name,
                       bsl::string_view      description,
                       const TypeInfo&       typeInfo)
: d_tag(tag)
, d_name(name)
, d_description(description)
, d_typeInfo(typeInfo)
, d_defaultInfo()
, d_environmentVariableName()
{
}

inline
OptionInfo::OptionInfo(bsl::string_view      tag,
                       bsl::string_view      name,
                       bsl::string_view      description,
                       const TypeInfo&       typeInfo,
                       const OccurrenceInfo& defaultInfo,
                       bsl::string_view      envVarName)
: d_tag(tag)
, d_name(name)
, d_description(description)
, d_typeInfo(typeInfo)
, d_defaultInfo(defaultInfo)
, d_environmentVariableName(envVarName)
{
}
#endif

// FREE OPERATORS

/// Return `true` if the specified `lhs` and `rhs` have the same value, and
/// `false` otherwise.  Two `OptionInfo` objects have the same value if they
/// have the same tag string, the same name, the same description, the same
/// type info, and the same occurrence info values.
bool operator==(const OptionInfo& lhs, const OptionInfo& rhs);

/// Return `true` if the specified `lhs` and `rhs` do not have the same
/// value, and `false` otherwise.  Two `OptionInfo` object do not have the
/// same value if they do not have the same tag strings, or the same names,
/// or the same descriptions, or the same type information, or the same
/// occurrence information.
bool operator!=(const OptionInfo& lhs, const OptionInfo& rhs);

/// Write the value of the specified `rhs` object to the specified `stream`
/// in a (multi-line) human readable format and return a reference to
/// `stream`.  Note that the last line is *not* terminated by a newline
/// character.
bsl::ostream& operator<<(bsl::ostream& stream, const OptionInfo& rhs);

}  // close package namespace

// ============================================================================
//                        INLINE DEFINITIONS
// ============================================================================

                     // -----------------
                     // struct OptionInfo
                     // -----------------

// FREE OPERATORS
inline
bool balcl::operator==(const OptionInfo& lhs, const OptionInfo& rhs)
{
    return lhs.d_tag                      == rhs.d_tag
        && lhs.d_name                     == rhs.d_name
        && lhs.d_description              == rhs.d_description
        && lhs.d_typeInfo                 == rhs.d_typeInfo
        && lhs.d_defaultInfo              == rhs.d_defaultInfo
        && lhs.d_environmentVariableName  == rhs.d_environmentVariableName;
}

inline
bool balcl::operator!=(const OptionInfo& lhs, const OptionInfo& rhs)
{
    return lhs.d_tag                      != rhs.d_tag
        || lhs.d_name                     != rhs.d_name
        || lhs.d_description              != rhs.d_description
        || lhs.d_typeInfo                 != rhs.d_typeInfo
        || lhs.d_defaultInfo              != rhs.d_defaultInfo
        || lhs.d_environmentVariableName  != rhs.d_environmentVariableName;
}

}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2020 Bloomberg Finance L.P.
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
