// balcl_option.h                                                     -*-C++-*-
#ifndef INCLUDED_BALCL_OPTION
#define INCLUDED_BALCL_OPTION

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide an allocator-aware command-line-option descriptor class.
//
//@CLASSES:
//   balcl::Option: allocator-aware, in-core VST option descriptor
//
//@SEE_ALSO:  balcl_optioninfo, balcl_commandline
//
//@DESCRIPTION: This component provides an unconstrained, allocator-aware,
// in-core (value-semantic) attribute class, `balcl::Option`, that describes a
// command-line option.  Class `balcl::Option` is used to specify the
// user-defined command-line options accepted by a `balcl::CommandLine` object.
//
// The value of a `balcl::Option` object is the same as that of the
// `balcl::OptionInfo` object accessible from the option object via a
// conversion operator.  That value consists of several (subordinate)
// attributes:
//
// * the strings associated with the option:
//   - `tag`
//   - `name`, and
//   - `description`
// * the `typeInfo` attribute (see {`balcl_typeinfo`}) that, in turn,
//   consists of:
//   - `type` (type of the option's value)
//   - `linkedVariable` [optional], and
//   - `constraint` [optional]
//
// * the `occurrenceInfo` attribute (see {`balcl_occurrenceinfo`}) that, in
//   turn, consists of:
//   - `occurrenceType` (required, optional, or hidden)
//   - `defaulValue` [optional]
//
// Since `balcl::TypeInfo` is an in-core VST, so is `balcl::Option`.
//
// When constructing a `balcl::Option` from a `balcl::OptionInfo` object, the
// class places no constraints on the latter's value except, of course, for the
// constraints required by the types that compose the `balcl::OptionInfo`
// class.
//
// Additionally, the `balcl::Option` class provides:
//
// * A set of (`is*Valid`) methods that report whether or not the option's
//   string attributes (tag, name, description) are valid for use by
//   `balcl::CommandLine`.
// * Allocator awareness that allows `balcl::Option` values to be stored in
//   allocator-aware containers.
//
///Usage
///-----
// The intended use of this component is illustrated in
// [](balcl_commandline#Usage).

#include <balscm_version.h>

#include <balcl_optioninfo.h>

#include <bdlb_printmethods.h> // 'bdlb::HasPrintMethod'

#include <bslma_allocator.h>
#include <bslma_usesbslmaallocator.h>

#include <bslmf_nestedtraitdeclaration.h>

#include <bsls_objectbuffer.h>

#include <bsl_iosfwd.h>  // 'bsl::ostream'
#include <bsl_string.h>

namespace BloombergLP {
namespace balcl {

class OccurrenceInfo;
class TypeInfo;

                        // ============
                        // class Option
                        // ============

/// This `class`, constructible from and implicitly convertible to
/// `OptionInfo`, provides the same attributes, but also uses a
/// `bslma::Allocator` and thus can be stored in a container.  A minor subtlety
/// arises in the `name` attribute, whereby any suffix starting with an `=`
/// sign in the `name` attribute of a `OptionInfo` is removed in order to
/// derive the `name` of the `Option`.
class Option {

    // DATA
    bsls::ObjectBuffer<OptionInfo>
                      d_optionInfo;   // underlying option info

    bslma::Allocator *d_allocator_p;  // memory allocator (held, not owned)

  private:
    // PRIVATE MANIPULATORS

    /// Default initialize the underlying option info.
    void init();

    /// Initialize the underlying option info from the value of the
    /// specified `optionInfo` object.
    void init(const OptionInfo& optionInfo);

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(Option, bslma::UsesBslmaAllocator);
    BSLMF_NESTED_TRAIT_DECLARATION(Option, bdlb::HasPrintMethod);

    // CREATORS

    /// Create a default (empty) command-line option.  Optionally specify a
    /// `basicAllocator` used to supply memory.  If `basicAllocator` is 0,
    /// the currently installed default allocator is used.
    Option();
    explicit
    Option(bslma::Allocator *basicAllocator);

    /// Create a command-line option containing the value of the specified
    /// `optionInfo`.  Optionally specify a `basicAllocator` used to supply
    /// memory.  If `basicAllocator` is 0, the currently installed default
    /// allocator is used.
    explicit
    Option(const OptionInfo&  optionInfo,
           bslma::Allocator  *basicAllocator = 0);

    /// Create a `Option` object having the same value as the specified
    /// `original` object.  Optionally specify a `basicAllocator` used to
    /// supply memory.  If `basicAllocator` is 0, the currently installed
    /// default allocator is used.
    Option(const Option&     original,
           bslma::Allocator *basicAllocator = 0);

    /// Destroy this command-line option object.
    ~Option();

    // MANIPULATORS

    /// Assign to this object the value of the specified `rhs` object and
    /// return a reference providing modifiable access to this object.
    Option& operator=(const Option&     rhs);
    Option& operator=(const OptionInfo& rhs);

    // ACCESSORS

    /// Return a `const` reference to the `OptionInfo` object having the value
    /// of this option.
    operator const OptionInfo&() const;

    /// Return the `ArgType` enumerator identifying whether this option is a
    /// flag (i.e., a tag not followed by a value), or whether it is an option
    /// (i.e., a tag followed by a value), or a non-option argument (i.e., a
    /// value not preceded by a short or long tag).
    OptionInfo::ArgType argType() const;

    /// Return the description of this option.
    const bsl::string& description() const;

    /// Return the environment variable name of this option.
    const bsl::string& environmentVariableName() const;

    /// Return `true` if the value of this option is of array type, and `false`
    /// otherwise.
    bool isArray() const;

    /// Return the string used to identify this option on a command line
    /// preceded with `--`, or 0 if there is no long tag associated with this
    /// option.  The behavior is undefined if this option is a non-option
    /// argument (i.e., has no tag).
    const char *longTag() const;

    /// Return the name of this option.
    const bsl::string& name() const;

    /// Return a `const` reference to the occurrence info for this option
    /// (i.e., whether the option is required, optional, or hidden, and its
    /// default value, if any).
    const OccurrenceInfo& occurrenceInfo() const;

    /// Return the single character used to identify this option on a command
    /// line preceded with a single `-`, or 0 if this option only has a long
    /// tag.  The behavior is undefined if this option is a non-option argument
    /// (i.e., has no tag).
    char shortTag() const;

    /// Return the tag string provided to this object at construction.  Note
    /// that this string is empty if this is a non-option argument.
    const bsl::string& tagString() const;

    /// Return a `const` reference to the type info for this option (i.e., the
    /// type of the option, whether the option is linked to a variable, and
    /// whether it has a constraint).
    const TypeInfo& typeInfo() const;

                                  // Validators

    /// Return `true` if the description is valid for this option, leaving
    /// the specified `stream` unaffected; otherwise, write a diagnostic
    /// message to `stream` and return `false`.
    bool isDescriptionValid(bsl::ostream& stream) const;

    /// Return `true` if the specified `longTag` is valid for this option,
    /// leaving the specified `stream` unaffected; otherwise, write a
    /// diagnostic message to `stream` and return `false`.
    bool isLongTagValid(const char *longTag, bsl::ostream& stream) const;

    /// Return `true` if the name is valid for this option, leaving the
    /// specified `stream` unaffected; otherwise, write a diagnostic message to
    /// `stream` and return `false`.
    bool isNameValid(bsl::ostream& stream) const;

    /// Return `true` if the tag is valid for this option, leaving the
    /// specified `stream` unaffected; otherwise, write a diagnostic message to
    /// `stream` and return `false`.
    bool isTagValid(bsl::ostream& stream) const;

                                  // Aspects

    /// Return the allocator used by this object to supply memory.  Note that
    /// if no allocator was supplied at construction the currently installed
    /// default allocator at construction is used.
    bslma::Allocator *allocator() const;

    /// Format this object to the specified output `stream` at the (absolute
    /// value of) the optionally specified indentation `level` and return a
    /// reference to `stream`.  If `level` is specified, optionally specify
    /// `spacesPerLevel`, the number of spaces per indentation level for this
    /// object.  If `level` is negative, suppress indentation of the first
    /// line.  If `stream` is not valid on entry, this operation has no effect.
    /// The behavior is undefined if `spacesPerLevel` is negative.
    bsl::ostream& print(bsl::ostream& stream,
                        int           level = 0,
                        int           spacesPerLevel = 4) const;
};

// FREE OPERATORS

/// Return `true` if the specified `lhs` and `rhs` have the same value, and
/// `false` otherwise.  Two `Option` objects have the same value if their
/// underlying `OptionInfo` objects have the same value.
bool operator==(const Option& lhs, const Option& rhs);

/// Return `true` if the specified `lhs` command-line option has a different
/// value from the specified `rhs` command-line option, and `false` otherwise.
/// Two `Option` objects do not have the same value if their underlying
/// `OptionInfo` objects do not have the same value.
bool operator!=(const Option& lhs, const Option& rhs);

/// Write the value of the specified `rhs` object to the specified `stream` in
/// a (multi-line) human readable format and return a reference to `stream`.
/// Note that the last line is *not* terminated by a newline character.
bsl::ostream& operator<<(bsl::ostream& stream, const Option& rhs);

}  // close package namespace
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
