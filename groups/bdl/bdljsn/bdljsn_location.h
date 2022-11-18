// bdljsn_location.h                                                  -*-C++-*-
#ifndef INCLUDED_BDLJSN_LOCATION
#define INCLUDED_BDLJSN_LOCATION

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a value-semantic type for location in a JSON document.
//
//@CLASSES:
//  bdljsn::Location: position in a JSON document
//
//@SEE_ALSO: bdljsn_jsonutil, bdljsn_json
//
//@DESCRIPTION: This component provides a single, unconstrained
// (value-semantic) attribute class, 'bdljsn::Location', that is used to
// describe a location in a (JSON) document.  Location is expressed by the
// 'offset' (attrbute) in bytes from the start of the document.  See
// {'jsonutil'} for utilities that may provide 'bdljsn::Location' values when
// reporting error states.
//
///Attributes
///----------
//..
//  Name                Type           Default
//  ------------------  -------------  -------
//  offset              bsl::uint64_t  0
//..
//: o 'offset': the offset into the JSON document
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Basic Syntax
///- - - - - - - - - - - -
// This example exercises each of the methods of the 'bdljsn::Location' class.
//
// First, create a 'bdljsn::Location' object (having the default value):
//..
//  bdljsn::Location locationA;
//  assert(0 == locationA.offset());
//..
// Then, set 'locationA' to some other offset:
//..
//  locationA.setOffset(1);
//  assert(1 == locationA.offset());
//..
// Next, use the value constructor to create a second location having the same
// offset as the first:
//..
//      bdljsn::Location locationB(1);
//      assert(1         == locationB.offset());
//      assert(locationA == locationB);
//..
// Then, set the second location to the maximum offset:
//..
//  const bsl::uint64_t maxOffset = bsl::numeric_limits<bsl::uint64_t>::max();
//
//  locationB.setOffset(maxOffset);
//  assert(maxOffset == locationB.offset());
//..
// Next, create another 'Location` that is a copy of the one at 'maxOffset':
//..
//  bdljsn::Location locationC(locationB);
//  assert(locationB == locationC);
//..
// Then, set the first location back to the default state:
//..
//  locationA.reset();
//  assert(0                  == locationA.offset());
//  assert(bdljsn::Location() == locationA);
//..
//  Next, print the value of each:
//..
//  bsl::cout << locationA << "\n"
//            << locationB << bsl::endl;
//
//  bsl::cout << "\n";
//
//  locationC.print(bsl::cout, 2, 3);
//..
// and observe:
//..
//   0
//   18446744073709551615
//
//        [
//           offset = 18446744073709551615
//        ]
//..
// Finally, set each location equal to the first:
//..
//  locationC = locationB = locationA;
//  assert(0 == locationA.offset());
//  assert(0 == locationB.offset());
//  assert(0 == locationC.offset());
//..

#include <bdlscm_version.h>

#include <bslalg_swaputil.h>

#include <bslmf_isbitwisemoveable.h>
#include <bslmf_nestedtraitdeclaration.h>

#include <bslh_hash.h>

#include <bsl_cstdint.h> // 'bsl::uint64_t'
#include <bsl_iosfwd.h>

namespace BloombergLP {
namespace bdljsn {

                               // ==============
                               // class Location
                               // ==============

class Location {
    // This unconstrained (value-semantic) attribute class specifies a location
    // in a (JSON) document.  See the {Attributes} section under {DESCRIPTION}
    // in the component-level documentation for information on the class
    // attributes.  Note that the class invariants are identically the
    // constraints on the individual attributes.

    // DATA
    bsl::uint64_t d_offset; // offset in bytes into a document

    // FRIENDS
    template <class HASHALG>
    friend void hashAppend(HASHALG& hashAlg , const Location& );
    friend void swap(Location& , Location& );

  public:
    BSLMF_NESTED_TRAIT_DECLARATION(Location, bslmf::IsBitwiseMoveable);

    // CREATORS
    Location();
        // Create a 'Location' object having the default value, 0.

    explicit Location(bsl::uint64_t offset);
        // Create a 'Location' object having the specified 'offset'.

    Location(const Location& original);
        // Create a 'Location' object having the same value as the specified
        // 'original' object.

    //! ~Location() = default;
        // Destroy this object.

    // MANIPULATORS
    Location& operator=(const Location& rhs);
        // Assign to this object the value of the specified 'rhs' object.

    void reset();
        // Reset this object to the default value (i.e., its value upon default
        // construction).

    void setOffset(bsl::uint64_t value);
        // Set the "offset" attribute of this object to the specified 'value'.

                                  // Aspects

    void swap(Location& other);
        // Efficiently exchange the value of this object with the value of the
        // specified 'other' object.  This method provides the no-throw
        // exception-safety guarantee.

    // ACCESSORS
    bsl::uint64_t offset() const;
        // Return the 'offset' attribute of this object.

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
bool operator==(const Location& lhs, const Location& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
    // the same value, and 'false' otherwise.  Two attribute objects have the
    // same value if each respective attribute has the same value.

inline
bool operator!=(const Location& lhs, const Location& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
    // have the same value, and 'false' otherwise.  Two attribute objects do
    // not have the same value if one or more respective attributes differ in
    // values.

bsl::ostream& operator<<(bsl::ostream& stream, const Location& object);
    // Write the value of the specified 'object' to the specified output
    // 'stream' in a single-line format, and return a non-'const' reference to
    // 'stream'.  If 'stream' is not valid on entry, this operation has no
    // effect.  Note that this human-readable format is not fully specified and
    // can change without notice.  Also note that this method has the same
    // behavior as 'object.print(stream, 0, -1)', but with the attribute names
    // elided.

// FREE FUNCTIONS
template <class HASHALG>
void hashAppend(HASHALG& hashAlg, const Location& object);
    // Pass the specified 'object' to the specified 'hashAlg'.  This function
    // integrates with the 'bslh' modular hashing system and effectively
    // provides a 'bsl::hash' specialization for 'Location'.

void swap(Location& a, Location& b);
    // Exchange the values of the specified 'a' and 'b' objects.  This function
    // provides the no-throw exception-safety guarantee.

// ============================================================================
//                         INLINE DEFINITIONS
// ============================================================================

                               // --------------
                               // class Location
                               // --------------

// CREATORS
inline
Location::Location()
: d_offset(0)
{
}

inline
Location::Location(bsl::uint64_t offset)
: d_offset(offset)
{
}

inline
Location::Location(const Location& original)
: d_offset(original.d_offset)
{
}

// MANIPULATORS
inline
Location& Location::operator=(const Location& rhs)
{
    d_offset = rhs.d_offset;

    return *this;
}

inline
void Location::reset()
{
    d_offset = 0;
}

inline
void Location::setOffset(bsl::uint64_t value)
{
    d_offset = value;
}

                                  // Aspects

inline
void Location::swap(Location& other)
{
    bslalg::SwapUtil::swap(&d_offset,&other.d_offset);
}

// ACCESSORS
inline
bsl::uint64_t Location::offset() const
{
    return d_offset;
}

}  // close package namespace

// FREE OPERATORS
inline
bool bdljsn::operator==(const bdljsn::Location& lhs,
                        const bdljsn::Location& rhs)
{
    return lhs.offset() == rhs.offset();
}

inline
bool bdljsn::operator!=(const bdljsn::Location& lhs,
                        const bdljsn::Location& rhs)
{
    return lhs.offset() != rhs.offset();
}

// FREE FUNCTIONS
template <class HASHALG>
inline
void bdljsn::hashAppend(HASHALG& hashAlg, const Location& object)
{
    using ::BloombergLP::bslh::hashAppend;
    hashAppend(hashAlg, object.d_offset);
}

inline
void bdljsn::swap(Location& a, Location& b)
{
    bslalg::SwapUtil::swap(&a.d_offset, &b.d_offset);
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
