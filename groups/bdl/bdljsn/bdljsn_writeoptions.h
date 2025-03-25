// bdljsn_writeoptions.h                                              -*-C++-*-
#ifndef INCLUDED_BDLJSN_WRITEOPTIONS
#define INCLUDED_BDLJSN_WRITEOPTIONS

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide options for writing a JSON document.
//
//@CLASSES:
//  bdljsn::WriteOptions: options for writing a JSON document
//
//@SEE_ALSO: bdljsn_jsonutil, bdljsn_json
//
//@DESCRIPTION: This component provides a single, simply constrained
// (value-semantic) attribute class, `bdljsn::WriteOptions`, that is used to
// specify options for writing a JSON document (see `bdljsn_jsonutil`).
//
///Attributes
///----------
// ```
// Name                Type          Default         Simple Constraints
// ------------------  -----------   -------         ------------------
// escapeForwardSlash  bool          true            none
// initialIndentLevel  int           0               >= 0
// sortMembers         bool          false           none
// spacesPerLevel      int           4               >= 0
// style               WriteStyle    e_COMPACT       none
// ```
// * `escapeForwardSlash`: determines whether any `/` characters are output
//   escaped (as `\/`) or not (as `/`) in names or strings.
// * `initialIndentLevel`: initial indent level for the top-most element.  If
//   `style` is `e_COMPACT`, or `spacesPerLevel` is 0, this option is ignored.
// * `sortMembers`: indicates whether the members of a object will be sorted
//    in lexicographical order based on the member name.
// * `spacesPerLevel`: spaces per indent level.  If this option is 0, no
//    indentation is used.  If `style` is `e_COMPACT` or `e_ONELINE`, this
//    option is ignored.
// * `style`: the style used to encode the JSON data.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Creating and Populating an Options Object
/// - - - - - - - - - - - - - - - - - - - - - - - - - -
// This component is designed to be used at a higher level to set the options
// for writing `bdljsn::Json` objects in JSON format.  This example shows how
// to create and populate an options object.
//
// First, we default-construct a `bdljsn::WriteOptions` object:
// ```
// const int  INITIAL_INDENT_LEVEL     = 1;
// const int  SPACES_PER_LEVEL         = 4;
//
// bdljsn::WriteOptions options;
// assert(true  == options.escapeForwardSlash());
// assert(0     == options.initialIndentLevel());
// assert(4     == options.spacesPerLevel());
// assert(false == options.sortMembers());
// assert(bdljsn::WriteStyle::e_COMPACT == options.style());
// ```
// Finally, we populate that object using a pre-defined initial indent level
// and spaces per level:
// ```
// options.setInitialIndentLevel(INITIAL_INDENT_LEVEL);
// assert(INITIAL_INDENT_LEVEL == options.initialIndentLevel());
//
// options.setSpacesPerLevel(SPACES_PER_LEVEL);
// assert(SPACES_PER_LEVEL == options.spacesPerLevel());
// ```

#include <bdlscm_version.h>

#include <bdljsn_writestyle.h>

#include <bsl_iosfwd.h>

#include <bsls_assert.h>

namespace BloombergLP {

namespace bdljsn {

                             // ==================
                             // class WriteOptions
                             // ==================

/// This simply constrained (value-semantic) attribute class specifies options
/// for writing a JSON document.  See the [](#Attributes) for information on
/// the class attributes.  Note that the class invariants are identically the
/// constraints on the individual attributes.
class WriteOptions {

    // INSTANCE DATA

    // initial indentation level for the topmost element
    int                      d_initialIndentLevel;

    // whether to sort members of an object by member name
    bool                     d_sortMembers;

    // whether `/` characters should be escaped in output
    bool                     d_escapeForwardSlash;

    // spaces per additional level of indentation
    int                      d_spacesPerLevel;

    // write style used for formatting JSON text
    bdljsn::WriteStyle::Enum d_style;

  public:
    // CONSTANTS
    static const int         s_DEFAULT_INITIALIZER_INITIAL_INDENT_LEVEL;

    static const bool        s_DEFAULT_INITIALIZER_SORT_MEMBERS;

    static const bool        s_DEFAULT_INITIALIZER_ESCAPE_FORWARD_SLASH;

    static const int         s_DEFAULT_INITIALIZER_SPACES_PER_LEVEL;

    static const bdljsn::WriteStyle::Enum
                             s_DEFAULT_INITIALIZER_STYLE;

  public:
    // CREATORS

    /// Create an object of type `WriteOptions` having the (default)
    /// attribute values:
    /// ```
    /// escapeForwardSlash()  == false
    /// initialIndentLevel()  == 0
    /// sortMembers()         == false
    /// spacesPerLevel()      == 4
    /// style()               == e_COMPACT
    /// ```
    WriteOptions();

    /// Create an object of type `WriteOptions` having the value of the
    /// specified `original` object.
    WriteOptions(const WriteOptions& original);

    /// Destroy this object.
    ~WriteOptions();

    // MANIPULATORS

    /// Assign to this object the value of the specified `rhs` object and
    /// return a non-`const` reference to this object.
    WriteOptions& operator=(const WriteOptions& rhs);

    /// Reset this object to the default value (i.e., its value upon default
    /// construction) and return a non-`const` reference to this object.
    WriteOptions& reset();

    /// Set the `escapeForwardSlash` attribute of this object to the specified
    /// `value` and return a non-`const` reference to this object.
    WriteOptions& setEscapeForwardSlash(bool value);

    /// Set the `initialIndentLevel` attribute of this object to the specified
    /// `value` and return a non-`const` reference to this object.  The
    /// behavior is undefined unless `0 <= value`.
    WriteOptions& setInitialIndentLevel(int value);

    /// Set the `sortMembers` attribute of this object to the specified `value`
    /// and return a non-`const` reference to this object.
    WriteOptions& setSortMembers(bool value);

    /// Set the `spacesPerLevel` attribute of this object to the specified
    /// `value` and return a non-`const` reference to this object.  The
    /// behavior is undefined unless `0 <= value`.
    WriteOptions& setSpacesPerLevel(int value);

    /// Set the `style` attribute of this object to the specified `value` and
    /// return a non-`const` reference to this object.
    WriteOptions& setStyle(bdljsn::WriteStyle::Enum value);

    // ACCESSORS

    /// Return the `escapeForwardSlash` attribute of this object.
    bool escapeForwardSlash() const;

    /// Return the `initialIndentLevel` attribute of this object.
    int initialIndentLevel() const;

    /// Return the `sortMembers` attribute of this object.
    bool sortMembers() const;

    /// Return the `spacesPerLevel` attribute of this object.
    int spacesPerLevel() const;

    /// Return the `style` attribute of this object.
    bdljsn::WriteStyle::Enum style() const;

                                  // Aspects

    /// Format this object to the specified output `stream` at the
    /// optionally specified indentation `level` and return a reference to
    /// the modifiable `stream`.  If `level` is specified, optionally
    /// specify `spacesPerLevel`, the number of spaces per indentation level
    /// for this and all of its nested objects.  Each line is indented by
    /// the absolute value of `level * spacesPerLevel`.  If `level` is
    /// negative, suppress indentation of the first line.  If
    /// `spacesPerLevel` is negative, suppress line breaks and format the
    /// entire output on one line.  If `stream` is initially invalid, this
    /// operation has no effect.  Note that a trailing newline is provided
    /// in multiline mode only.  Also note that the format is not fully
    /// specified, and can change without notice.
    bsl::ostream& print(bsl::ostream& stream,
                        int           level = 0,
                        int           spacesPerLevel = 4) const;
};

// FREE OPERATORS

/// Return `true` if the specified `lhs` and `rhs` attribute objects have the
/// same value, and `false` otherwise.  Two attribute objects have the same
/// value if each respective attribute has the same value.
inline
bool operator==(const WriteOptions& lhs, const WriteOptions& rhs);

/// Return `true` if the specified `lhs` and `rhs` attribute objects do not
/// have the same value, and `false` otherwise.  Two attribute objects do not
/// have the same value if one or more respective attributes differ in values.
inline
bool operator!=(const WriteOptions& lhs, const WriteOptions& rhs);

/// Format the specified `rhs` to the specified output `stream` and return a
/// non-`const` reference `stream`.
inline
bsl::ostream& operator<<(bsl::ostream& stream, const WriteOptions& rhs);

// ============================================================================
//                         INLINE DEFINITIONS
// ============================================================================

                             // ------------------
                             // class WriteOptions
                             // ------------------

// CREATORS
inline
WriteOptions::WriteOptions(const WriteOptions& original)
: d_initialIndentLevel(original.d_initialIndentLevel)
, d_sortMembers       (original.d_sortMembers)
, d_escapeForwardSlash(original.d_escapeForwardSlash)
, d_spacesPerLevel    (original.d_spacesPerLevel)
, d_style             (original.d_style)
{
}

inline
WriteOptions::~WriteOptions()
{
    BSLS_ASSERT(0 <= d_initialIndentLevel);
    BSLS_ASSERT(0 <= d_spacesPerLevel);
}

// MANIPULATORS
inline
WriteOptions& WriteOptions::operator=(const WriteOptions& rhs)
{
    d_initialIndentLevel = rhs.d_initialIndentLevel;
    d_sortMembers        = rhs.d_sortMembers;
    d_escapeForwardSlash = rhs.d_escapeForwardSlash;
    d_spacesPerLevel     = rhs.d_spacesPerLevel;
    d_style              = rhs.d_style;

    return *this;
}

inline
WriteOptions& WriteOptions::setEscapeForwardSlash(bool value)
{
    d_escapeForwardSlash = value;
    return *this;
}

inline
WriteOptions& WriteOptions::setInitialIndentLevel(int value)
{
    BSLS_ASSERT(0 <= value);

    d_initialIndentLevel = value;
    return *this;
}

inline
WriteOptions& WriteOptions::setSortMembers(bool value)
{
    d_sortMembers = value;
    return *this;
}

inline
WriteOptions& WriteOptions::setSpacesPerLevel(int value)
{
    BSLS_ASSERT(0 <= value);

    d_spacesPerLevel = value;
    return *this;
}

inline
WriteOptions& WriteOptions::setStyle(bdljsn::WriteStyle::Enum value)
{
    d_style = value;
    return *this;
}

// ACCESSORS
inline
bool WriteOptions::escapeForwardSlash() const
{
    return d_escapeForwardSlash;
}

inline
int WriteOptions::initialIndentLevel() const
{
    return d_initialIndentLevel;
}

inline
bool WriteOptions::sortMembers() const
{
    return d_sortMembers;
}

inline
int WriteOptions::spacesPerLevel() const
{
    return d_spacesPerLevel;
}

inline
bdljsn::WriteStyle::Enum WriteOptions::style() const
{
    return d_style;
}

}  // close package namespace

// FREE OPERATORS
inline
bool bdljsn::operator==(const bdljsn::WriteOptions& lhs,
                        const bdljsn::WriteOptions& rhs)
{
    return  lhs.initialIndentLevel() == rhs.initialIndentLevel()
         && lhs.sortMembers()        == rhs.sortMembers()
         && lhs.escapeForwardSlash() == rhs.escapeForwardSlash()
         && lhs.spacesPerLevel()     == rhs.spacesPerLevel()
         && lhs.style()              == rhs.style();
}

inline
bool bdljsn::operator!=(const bdljsn::WriteOptions& lhs,
                        const bdljsn::WriteOptions& rhs)
{
    return  lhs.initialIndentLevel() != rhs.initialIndentLevel()
         || lhs.sortMembers()        != rhs.sortMembers()
         || lhs.escapeForwardSlash() != rhs.escapeForwardSlash()
         || lhs.spacesPerLevel()     != rhs.spacesPerLevel()
         || lhs.style()              != rhs.style();
}

inline
bsl::ostream& bdljsn::operator<<(bsl::ostream&               stream,
                                 const bdljsn::WriteOptions& rhs)
{
    return rhs.print(stream, 0, -1);
}

}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2022 Bloomberg Finance L.P.
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
