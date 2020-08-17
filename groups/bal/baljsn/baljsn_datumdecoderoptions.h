// baljsn_datumdecoderoptions.h                                       -*-C++-*-
#ifndef INCLUDED_BALJSN_DATUMDECODEROPTIONS
#define INCLUDED_BALJSN_DATUMDECODEROPTIONS

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide options for decoding JSON into a 'Datum' object.
//
//@CLASSES:
//  baljsn::DatumDecoderOptions: options for decoding JSON into a 'Datum'
//
//@SEE_ALSO: baljsn_datumutil
//
//@DESCRIPTION: This component provides a single, simply constrained
// (value-semantic) attribute class, 'baljsn::DatumDecoderOptions', that is
// used to specify options for decoding 'Datum' objects in the JSON format (see
// 'baljsn::DatumUtil').
//
///Attributes
///----------
//..
//  Name                Type           Default         Simple Constraints
//  ------------------  -----------    -------         ------------------
//  maxNestedDepth      int            64              > 0
//..
//: o 'maxNestedDepth': the maximum depth to which JSON objects and arrays are
//:   allowed to be nested before the JSON decoder reports an error.  For
//:   example, if 'maxNestedDepth' is 8, and a JSON text has 9 consecutive open
//:   brackets ('[') the decoding will return an error.  This option can be
//:   used to prevent poorly formed, or malicious JSON text from causing a
//:   stack overflow.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Creating and Populating an Options Object
/// - - - - - - - - - - - - - - - - - - - - - - - - - -
// This component is designed to be used at a higher level to set the options
// for decoding 'Datum' objects in the JSON format.  This example shows how to
// create and populate an options object.
//
// First, we default-construct a 'baljsn::DatumDecoderOptions' object:
//..
//  const int MAX_NESTED_DEPTH = 16;
//
//  baljsn::DatumDecoderOptions options;
//  assert(64 == options.maxNestedDepth());
//..
// Finally, we populate that object to limit the maximum nested depth using a
// pre-defined limit:
//..
//  options.setMaxNestedDepth(MAX_NESTED_DEPTH);
//  assert(MAX_NESTED_DEPTH == options.maxNestedDepth());
//..

#include <balscm_version.h>

#include <bslalg_typetraits.h>

#include <bsl_limits.h>
#include <bsl_iosfwd.h>

#include <bsls_assert.h>
#include <bsls_objectbuffer.h>
#include <bsls_review.h>

namespace BloombergLP {

namespace baljsn { class DatumDecoderOptions; }
namespace baljsn {

                         // =========================
                         // class DatumDecoderOptions
                         // =========================

class DatumDecoderOptions {
    // This simply constrained (value-semantic) attribute class specifies
    // options for decoding 'Datum' objects in the JSON format.  See the
    // Attributes section under @DESCRIPTION in the component-level
    // documentation for information on the class attributes.  Note that the
    // class invariants are identically the constraints on the individual
    // attributes.

    // INSTANCE DATA
    int d_maxNestedDepth;
        // maximum nesting level for JSON objects and arrays

  public:
    // CONSTANTS
    static const int s_DEFAULT_INITIALIZER_MAX_NESTED_DEPTH;

  public:
    // CREATORS
    DatumDecoderOptions();
        // Create an object of type 'DatumDecoderOptions' having the default
        // value.

    DatumDecoderOptions(const DatumDecoderOptions& original);
        // Create an object of type 'DatumDecoderOptions' having the value of
        // the specified 'original' object.

    ~DatumDecoderOptions();
        // Destroy this object.

    // MANIPULATORS
    DatumDecoderOptions& operator=(const DatumDecoderOptions& rhs);
        // Assign to this object the value of the specified 'rhs' object.

    void reset();
        // Reset this object to the default value (i.e., its value upon
        // default construction).

    void setMaxNestedDepth(int value);
        // Set the "maxNestedDepth" attribute of this object to the specified
        // 'value'.

    // ACCESSORS
    int maxNestedDepth() const;
        // Return the "maxNestedDepth" attribute of this object.

                                  // Aspects

    bsl::ostream& print(bsl::ostream& stream,
                        int           level = 0,
                        int           spacesPerLevel = 4) const;
        // Format this object to the specified output 'stream' at the
        // optionally specified indentation 'level' and return a reference to
        // the modifiable 'stream'.  If 'level' is specified, optionally
        // specify 'spacesPerLevel', the number of spaces per indentation level
        // for this and all of its nested objects.  Each line is indented by
        // the absolute value of 'level * spacesPerLevel'.  If 'level' is
        // negative, suppress indentation of the first line.  If
        // 'spacesPerLevel' is negative, suppress line breaks and format the
        // entire output on one line.  If 'stream' is initially invalid, this
        // operation has no effect.  Note that a trailing newline is provided
        // in multiline mode only.
};

// FREE OPERATORS
inline
bool operator==(const DatumDecoderOptions& lhs,
                const DatumDecoderOptions& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
    // the same value, and 'false' otherwise.  Two attribute objects have the
    // same value if each respective attribute has the same value.

inline
bool operator!=(const DatumDecoderOptions& lhs,
                const DatumDecoderOptions& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
    // have the same value, and 'false' otherwise.  Two attribute objects do
    // not have the same value if one or more respective attributes differ in
    // values.

inline
bsl::ostream& operator<<(bsl::ostream& stream, const DatumDecoderOptions& rhs);
    // Format the specified 'rhs' to the specified output 'stream' and return a
    // reference to the modifiable 'stream'.

// ============================================================================
//                         INLINE FUNCTION DEFINITIONS
// ============================================================================

                         // -------------------------
                         // class DatumDecoderOptions
                         // -------------------------

inline
void DatumDecoderOptions::setMaxNestedDepth(int value)
{
    BSLS_ASSERT(0 < value);

    d_maxNestedDepth = value;
}

// ACCESSORS
inline
int DatumDecoderOptions::maxNestedDepth() const
{
    return d_maxNestedDepth;
}

}  // close package namespace

// FREE OPERATORS

inline
bool baljsn::operator==(const baljsn::DatumDecoderOptions& lhs,
                        const baljsn::DatumDecoderOptions& rhs)
{
    return lhs.maxNestedDepth() == rhs.maxNestedDepth();
}

inline
bool baljsn::operator!=(const baljsn::DatumDecoderOptions& lhs,
                        const baljsn::DatumDecoderOptions& rhs)
{
    return lhs.maxNestedDepth() != rhs.maxNestedDepth();
}

inline
bsl::ostream& baljsn::operator<<(bsl::ostream&                      stream,
                                 const baljsn::DatumDecoderOptions& rhs)
{
    return rhs.print(stream, 0, -1);
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
