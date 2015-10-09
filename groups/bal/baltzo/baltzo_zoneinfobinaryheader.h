// baltzo_zoneinfobinaryheader.h                                      -*-C++-*-
#ifndef INCLUDED_BALTZO_ZONEINFOBINARYHEADER
#define INCLUDED_BALTZO_ZONEINFOBINARYHEADER

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide an attribute class for Zoneinfo binary-file header data.
//
//@CLASSES:
//   baltzo::ZoneinfoBinaryHeader: attribute class for Zoneinfo header data
//
//@SEE_ALSO: baltzo_zoneinfobinaryreader
//
//@DESCRIPTION: This component provides a simply constrained attribute class,
// 'baltzo::ZoneinfoBinaryHeader', representing the header data of a Zoneinfo
// binary data file.
//
///Attributes
///----------
//..
//  Name               Type  Default  Simple Constraints
//  -----------------  ----  -------  ------------------
//  version            char  '\0'     == '\0' || == '2'
//  numIsGmt           int    0       >= 0
//  numIsStd           int    0       >= 0
//  numLeaps           int    0       == 0
//  numTransitions     int    0       >= 0
//  numLocalTimeTypes  int    1       >= 1
//  abbrevDataSize     int    1       >= 1
//..
//: o 'version': Zoneinfo file format version, as of 2011, either '\0' or '2'.
//:
//: o 'numIsGmt': number of encoded UTC/local indicators in the file,
//:   indicating whether a transition time was originally specified as UTC in
//:   the rule file.
//:
//: o 'numIsStd': number of encoded standard/wall indicators in the file,
//:   indicating whether a transition time was originally specified as standard
//:   time in the rule file.
//:
//: o 'numLeaps': number of leap corrections stored in the file.
//:
//: o 'numTransitions': number of local-time type transitions stored in the
//:   file.
//:
//: o 'numLocalTimeTypes': number of local-time types stored in the file.
//:
//: o 'abbrevDataSize': length of the sequence of characters containing the
//:   ('\0'-separated) abbreviation strings in the file.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Creating a 'baltzo::ZoneinfoBinaryHeader' from User Input
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// We define the 'getNextZoneinfoBinaryHeader' helper function, reads data from
// a stream, validates the data, and constructs a
// 'baltzo::ZoneinfoBinaryHeader' object.
//..
//  int getNextZoneinfoBinaryHeader(baltzo::ZoneinfoBinaryHeader *object,
//                                  bsl::istream&                stream)
//      // Set to the specified 'object' the value extracted from the
//      // specified 'stream'.  Return 0 on success, and a non-zero value
//      // otherwise, with no change to 'object'.  The 'stream' contains
//      // white-space separated decimal representations of the attributes
//      // of 'baltzo::ZoneinfoBinaryHeader' in the following order: 'version',
//      // 'numIsGmt', 'numIsStd', 'numLeaps', 'numTransitions',
//      // 'numLocalTimeTypes', and 'abbrevDataSize'.
//  {
//      int version;  // not 'char'
//      int numIsGmt;
//      int numIsStd;
//      int numLeaps;
//      int numTransitions;
//      int numLocalTimeTypes;
//      int abbrevDataSize;
//
//      if (!(stream >> version
//         && stream >> numIsGmt
//         && stream >> numIsStd
//         && stream >> numLeaps
//         && stream >> numTransitions
//         && stream >> numLocalTimeTypes
//         && stream >> abbrevDataSize)) {
//          return 1;                                                 // RETURN
//      }
//
//      if (!(baltzo::ZoneinfoBinaryHeader::isValidVersion(version)
//         && baltzo::ZoneinfoBinaryHeader::isValidNumIsGmt(numIsGmt)
//         && baltzo::ZoneinfoBinaryHeader::isValidNumIsStd(numIsStd)
//         && baltzo::ZoneinfoBinaryHeader::isValidNumLeaps(numLeaps)
//         && baltzo::ZoneinfoBinaryHeader::isValidNumTransitions(
//                                                              numTransitions)
//         && baltzo::ZoneinfoBinaryHeader::isValidNumLocalTimeTypes(
//                                                           numLocalTimeTypes)
//         && baltzo::ZoneinfoBinaryHeader::isValidAbbrevDataSize(
//                                                          abbrevDataSize))) {
//          return 2;                                                 // RETURN
//      }
//
//      object->setVersion(version);
//      object->setNumIsGmt(numIsGmt);
//      object->setNumIsStd(numIsStd);
//      object->setNumLeaps(numLeaps);
//      object->setNumTransitions(numTransitions);
//      object->setNumLocalTimeTypes(numLocalTimeTypes);
//      object->setAbbrevDataSize(abbrevDataSize);
//
//      return 0;
//  }
//..
// To use our helper function, we supply it with a stream of (decimal,
// whitespace-separated values).  The resulting object has the expected value.
//..
//  bsl::stringstream           input("50 1 2 0 3 4 5");
//  baltzo::ZoneinfoBinaryHeader header;
//  int rc;
//
//  rc = getNextZoneinfoBinaryHeader(&header, input);
//
//  assert( 0  == rc);
//  assert('2' == header.version());
//  assert( 1  == header.numIsGmt());
//  assert( 2  == header.numIsStd());
//  assert( 0  == header.numLeaps());
//  assert( 3  == header.numTransitions());
//  assert( 4  == header.numLocalTimeTypes());
//  assert( 5  == header.abbrevDataSize());
//..
// Since all of the data in the stream has now been consumed, another call to
// the function returns an error and leaves the object unchanged.
//..
//  header.setVersion(0);
//  header.setNumIsGmt(10);
//  header.setNumIsStd(20);
//  header.setNumLeaps(0);
//  header.setNumTransitions(30);
//  header.setNumLocalTimeTypes(40);
//  header.setAbbrevDataSize(50);
//
//  rc = getNextZoneinfoBinaryHeader(&header, input);
//
//  assert(  0  != rc);
//  assert('\0' == header.version());
//  assert( 10  == header.numIsGmt());
//  assert( 20  == header.numIsStd());
//  assert(  0  == header.numLeaps());
//  assert( 30  == header.numTransitions());
//  assert( 40  == header.numLocalTimeTypes());
//  assert( 50  == header.abbrevDataSize());
//..

#ifndef INCLUDED_BALSCM_VERSION
#include <balscm_version.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSL_ALGORITHM
#include <bsl_algorithm.h>
#endif

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif

namespace BloombergLP {
namespace baltzo {
                         // ==========================
                         // class ZoneinfoBinaryHeader
                         // ==========================

class ZoneinfoBinaryHeader {
    // This simply constrained (value-semantic) attribute class represents the
    // header information found at the start of a Zoneinfo (binary) database
    // file, which describes the contents of the file.
    //
    // See the Attributes section under @DESCRIPTION in the component-level
    // documentation.  Note that the class invariants are identically the
    // constraints on the individual attributes.
    //
    // This class:
    //: o supports a complete set of *value* *semantic* operations
    //:   o except for 'bdex' serialization
    //: o is *exception-neutral*
    //: o is *alias-safe*
    //: o is 'const' *thread-safe*
    // For terminology see 'bsldoc_glossary'.

    // DATA
    char d_version;            // file format version of the Zoneinfo, as of
                               // 2011, it can be either '\0' or '2'.

    int  d_numIsGmt;           // number of encoded UTC/local indicators in the
                               // file, indicating whether a transition time
                               // was originally specified as UTC in the rule
                               // file.

    int  d_numIsStd;           // number of encoded standard/wall indicators in
                               // the file, indicating whether a transition
                               // time was originally specified as standard the
                               // in the rule file.

    int  d_numLeaps;           // number of leap corrections stored in the file

    int  d_numTransitions;     // number of local-time type transitions stored
                               // in the file

    int  d_numLocalTimeTypes;  // number of local-time types stored in the file

    int  d_abbrevDataSize;     // length of the sequence of characters
                               // containing the ('\0' separated) abbreviation
                               // strings in the file

  public:
    // CLASS METHODS
    static bool isValidVersion(char value);
        // Return 'true' if the specified value equals '\0' or '2', and 'false'
        // otherwise.

    static bool isValidNumIsGmt(int value);
        // Return 'true' if the specified 'value' is greater than or equal to
        // 0, and 'false' otherwise'.

    static bool isValidNumIsStd(int value);
        // Return 'true' if the specified 'value' is greater than or equal to
        // 0, and 'false' otherwise'.

    static bool isValidNumLeaps(int value);
        // Return 'true' if the specified value equals 0, and 'false'
        // otherwise'.

    static bool isValidNumTransitions(int value);
        // Return 'true' if the specified 'value' is greater than or equal to
        // 0, and 'false' otherwise'.

    static bool isValidNumLocalTimeTypes(int value);
        // Return 'true' if the specified 'value' is greater than or equal to
        // 1, and 'false' otherwise'.

    static bool isValidAbbrevDataSize(int value);
        // Return 'true' if the specified 'value' is greater than or equal to
        // 1, and 'false' otherwise'.

    // CREATORS
    ZoneinfoBinaryHeader();
        // Create a 'ZoneinfoBinaryHeader' object having the (default)
        // attribute values:
        //..
        //  version()           == 0
        //  numIsGmt()          == 0
        //  numIsStd()          == 0
        //  numLeaps()          == 0
        //  numTransitions()    == 0
        //  numLocalTimeTypes() == 1
        //  abbrevDataSize()    == 1
        //..

    ZoneinfoBinaryHeader(char version,
                         int  numIsGmt,
                         int  numIsStd,
                         int  numLeaps,
                         int  numTransitions,
                         int  numLocalTimeTypes,
                         int  abbrevDataSize);
        // Create a 'ZoneinfoBinaryHeader' having the specified 'version',
        // 'numIsGmt', 'numIsStd', 'numLeaps', 'numTransitions',
        // 'numLocalTimeTypes', and 'abbrevDataSize' values.  The behavior is
        // undefined unless '0 == version || 50 == version', '0 <= numIsGmt',
        // '0 <= numIsStd', '0 == numLeaps', '0 <= numTransitions',
        // '1 <= numLocalTimeTypes', and '1 <= abbrevDataSize'.  Note that 50
        // is the value of ascii character '2'.

    ~ZoneinfoBinaryHeader();
        // Destroy this object.

    // MANIPULATORS
    ZoneinfoBinaryHeader& operator=(const ZoneinfoBinaryHeader& rhs);
        // Assign to this object the value of the specified 'rhs' object, and
        // return a reference providing modifiable access to this object.

    void setVersion(char value);
        // Set the 'version' attribute of this object to the specified 'value'.
        // The behavior is undefined unless '0 == value || 50 == value'.  Note
        // that 50 is the value of ascii character '2'.

    void setNumIsGmt(int value);
        // Set the 'numIsGmt' attribute of this object to the specified
        // 'value'.  The behavior is undefined unless '0 <= value'.

    void setNumIsStd(int value);
        // Set the 'numIsStd' attribute of this object to the specified
        // 'value'.  The behavior is undefined unless '0 <= value'.

    void setNumLeaps(int value);
        // Set the 'numLeaps' attribute of this object to the specified
        // 'value'.  The behavior is undefined unless '0 == value'.

    void setNumTransitions(int value);
        // Set the 'numTransitions' attribute of this object to the specified
        // 'value'.  The behavior is undefined unless '0 <= value'.

    void setNumLocalTimeTypes(int value);
        // Set the 'numLocalTimeTypes' attribute of this object to the
        // specified 'value'.  The behavior is undefined unless '1 <= value'.

    void setAbbrevDataSize(int value);
        // Set the 'abbrevDataSize' attribute of this object to the specified
        // 'value'.  The behavior is undefined unless '1 <= value'.

    void swap(ZoneinfoBinaryHeader& other);
        // Efficiently exchange the value of this object with the value of the
        // specified 'other' object.  This method provides the no-throw
        // exception-safety guarantee.

    // ACCESSORS
    char version() const;
        // Return the value of the 'version' attribute of this object.

    int numIsGmt() const;
        // Return the value of the 'numIsGmt' attribute of this object.

    int numIsStd() const;
        // Return the value of the 'numIsStd' attribute of this object.

    int numLeaps() const;
        // Return the value of the 'numLeaps' attribute of this object.

    int numTransitions() const;
        // Return the value of the 'numTransitions' attribute of this object.

    int numLocalTimeTypes() const;
        // Return the value of the 'numLocalTimeTypes' attribute of this
        // object.

    int abbrevDataSize() const;
        // Return the value of the 'abbrevDataSize' attribute of this object.

                        // Aspects

    bsl::ostream& print(bsl::ostream& stream,
                        int           level = 0,
                        int           spacesPerLevel = 4) const;
        // Write the value of this object to the specified output 'stream' in a
        // human-readable format, and return a reference to 'stream'.
        // Optionally specify an initial indentation 'level', whose absolute
        // value is incremented recursively for nested objects.  If 'level' is
        // specified, optionally specify 'spacesPerLevel', whose absolute value
        // indicates the number of spaces per indentation level for this and
        // all of its nested objects.  If 'level' is negative, suppress
        // indentation of the first line.  If 'spacesPerLevel' is negative,
        // format the entire output on one line, suppressing all but the
        // initial indentation (as governed by 'level').  If 'stream' is not
        // valid on entry, this operation has no effect.  Note that the format
        // is not fully specified, and can change without notice.
};

// FREE OPERATORS
bool operator==(const ZoneinfoBinaryHeader& lhs,
                const ZoneinfoBinaryHeader& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects have the same
    // value, and 'false' otherwise.  Two 'ZoneinfoBinaryHeader' objects have
    // the same value if the corresponding values of their 'version',
    // 'numIsGmt', 'numIsStd', 'numLeaps', 'numTransitions',
    // 'numLocalTimeTypes', and 'abbrevDataSize' attributes are the same.

bool operator!=(const ZoneinfoBinaryHeader& lhs,
                const ZoneinfoBinaryHeader& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects have the same
    // value, and 'false' otherwise.  Two 'ZoneinfoBinaryHeader' objects do not
    // have the same value if the corresponding values of their 'version',
    // 'numIsGmt', 'numIsStd', 'numLeaps', 'numTransitions',
    // 'numLocalTimeTypes', or 'abbrevDataSize' attributes are not the same.

bsl::ostream& operator<<(bsl::ostream&               stream,
                         const ZoneinfoBinaryHeader& object);
    // Write the value of the specified 'object' to the specified output
    // 'stream' in a single-line format, and return a reference to 'stream'.
    // If 'stream' is not valid on entry, this operation has no effect.  Note
    // that this human-readable format is not fully specified and can change
    // without notice.  Also note that this method has the same behavior as
    // 'object.print(stream, 0, -1)'.

// FREE FUNCTIONS
void swap(baltzo::ZoneinfoBinaryHeader& a, baltzo::ZoneinfoBinaryHeader& b);
    // Efficiently exchange the values of the specified 'a' and 'b' objects.
    // This function provides the no-throw exception-safety guarantee.

}  // close package namespace

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

                         // --------------------------
                         // class ZoneinfoBinaryHeader
                         // --------------------------

// CLASS METHODS
inline
bool baltzo::ZoneinfoBinaryHeader::isValidVersion(char value)
{
    return '\0' == value || '2' == value;
}

inline
bool baltzo::ZoneinfoBinaryHeader::isValidNumIsGmt(int value)
{
    return value >= 0;
}

inline
bool baltzo::ZoneinfoBinaryHeader::isValidNumIsStd(int value)
{
    return value >= 0;
}

inline
bool baltzo::ZoneinfoBinaryHeader::isValidNumLeaps(int value)
{
    return value == 0;
}

inline
bool baltzo::ZoneinfoBinaryHeader::isValidNumTransitions(int value)
{
    return value >= 0;
}

inline
bool baltzo::ZoneinfoBinaryHeader::isValidNumLocalTimeTypes(int value)
{
    return value >= 1;
}

inline
bool baltzo::ZoneinfoBinaryHeader::isValidAbbrevDataSize(int value)
{
    return value >= 1;
}

// CREATORS
inline
baltzo::ZoneinfoBinaryHeader::ZoneinfoBinaryHeader()
: d_version(0)
, d_numIsGmt(0)
, d_numIsStd(0)
, d_numLeaps(0)
, d_numTransitions(0)
, d_numLocalTimeTypes(1)
, d_abbrevDataSize(1)
{
}

inline
baltzo::ZoneinfoBinaryHeader::ZoneinfoBinaryHeader(char version,
                                                   int  numIsGmt,
                                                   int  numIsStd,
                                                   int  numLeaps,
                                                   int  numTransitions,
                                                   int  numLocalTimeTypes,
                                                   int  abbrevDataSize)
: d_version(version)
, d_numIsGmt(numIsGmt)
, d_numIsStd(numIsStd)
, d_numLeaps(numLeaps)
, d_numTransitions(numTransitions)
, d_numLocalTimeTypes(numLocalTimeTypes)
, d_abbrevDataSize(abbrevDataSize)
{
    BSLS_ASSERT_SAFE(isValidVersion(version));
    BSLS_ASSERT_SAFE(isValidNumIsGmt(numIsGmt));
    BSLS_ASSERT_SAFE(isValidNumIsStd(numIsStd));
    BSLS_ASSERT_SAFE(isValidNumLeaps(numLeaps));
    BSLS_ASSERT_SAFE(isValidNumTransitions(numTransitions));
    BSLS_ASSERT_SAFE(isValidNumLocalTimeTypes(numLocalTimeTypes));
    BSLS_ASSERT_SAFE(isValidAbbrevDataSize(abbrevDataSize));
}

inline
baltzo::ZoneinfoBinaryHeader::~ZoneinfoBinaryHeader()
{
    BSLS_ASSERT_SAFE(isValidVersion(d_version));
    BSLS_ASSERT_SAFE(isValidNumIsGmt(d_numIsGmt));
    BSLS_ASSERT_SAFE(isValidNumIsStd(d_numIsStd));
    BSLS_ASSERT_SAFE(isValidNumLeaps(d_numLeaps));
    BSLS_ASSERT_SAFE(isValidNumTransitions(d_numTransitions));
    BSLS_ASSERT_SAFE(isValidNumLocalTimeTypes(d_numLocalTimeTypes));
    BSLS_ASSERT_SAFE(isValidAbbrevDataSize(d_abbrevDataSize));
}

// MANIPULATORS
inline
baltzo::ZoneinfoBinaryHeader& baltzo::ZoneinfoBinaryHeader::operator=(
                                               const ZoneinfoBinaryHeader& rhs)
{
    d_version           = rhs.d_version;
    d_numIsGmt          = rhs.d_numIsGmt;
    d_numIsStd          = rhs.d_numIsStd;
    d_numLeaps          = rhs.d_numLeaps;
    d_numTransitions    = rhs.d_numTransitions;
    d_numLocalTimeTypes = rhs.d_numLocalTimeTypes;
    d_abbrevDataSize    = rhs.d_abbrevDataSize;
    return *this;
}
inline
void baltzo::ZoneinfoBinaryHeader::setVersion(char value)
{
    BSLS_ASSERT_SAFE(isValidVersion(value));

    d_version = value;
}

inline
void baltzo::ZoneinfoBinaryHeader::setNumIsGmt(int value)
{
    BSLS_ASSERT_SAFE(isValidNumIsGmt(value));

    d_numIsGmt = value;
}

inline
void baltzo::ZoneinfoBinaryHeader::setNumIsStd(int value)
{
    BSLS_ASSERT_SAFE(isValidNumIsStd(value));

    d_numIsStd = value;
}

inline
void baltzo::ZoneinfoBinaryHeader::setNumLeaps(int value)
{
    BSLS_ASSERT_SAFE(isValidNumLeaps(value));

    d_numLeaps = value;
}

inline
void baltzo::ZoneinfoBinaryHeader::setNumTransitions(int value)
{
    BSLS_ASSERT_SAFE(isValidNumTransitions(value));

    d_numTransitions = value;
}

inline
void baltzo::ZoneinfoBinaryHeader::setNumLocalTimeTypes(int value)
{
    BSLS_ASSERT_SAFE(isValidNumLocalTimeTypes(value));

    d_numLocalTimeTypes = value;
}

inline
void baltzo::ZoneinfoBinaryHeader::setAbbrevDataSize(int value)
{
    BSLS_ASSERT_SAFE(isValidAbbrevDataSize(value));

    d_abbrevDataSize = value;
}

inline
void baltzo::ZoneinfoBinaryHeader::swap(ZoneinfoBinaryHeader& other)
{
    bsl::swap(d_version,           other.d_version);
    bsl::swap(d_numIsGmt,          other.d_numIsGmt);
    bsl::swap(d_numIsStd,          other.d_numIsStd);
    bsl::swap(d_numLeaps,          other.d_numLeaps);
    bsl::swap(d_numTransitions,    other.d_numTransitions);
    bsl::swap(d_numLocalTimeTypes, other.d_numLocalTimeTypes);
    bsl::swap(d_abbrevDataSize,    other.d_abbrevDataSize);
}

// ACCESSORS
inline
char baltzo::ZoneinfoBinaryHeader::version() const
{
    return d_version;
}

inline
int baltzo::ZoneinfoBinaryHeader::numIsGmt() const
{
    return d_numIsGmt;
}

inline
int baltzo::ZoneinfoBinaryHeader::numIsStd() const
{
    return d_numIsStd;
}

inline
int baltzo::ZoneinfoBinaryHeader::numLeaps() const
{
    return d_numLeaps;
}

inline
int baltzo::ZoneinfoBinaryHeader::numTransitions() const
{
    return d_numTransitions;
}

inline
int baltzo::ZoneinfoBinaryHeader::numLocalTimeTypes() const
{
    return d_numLocalTimeTypes;
}

inline
int baltzo::ZoneinfoBinaryHeader::abbrevDataSize() const
{
    return d_abbrevDataSize;
}

// FREE OPERATORS
inline
bool baltzo::operator==(const ZoneinfoBinaryHeader& lhs,
                        const ZoneinfoBinaryHeader& rhs)
{
    return lhs.version()           == rhs.version()
        && lhs.numIsGmt()          == rhs.numIsGmt()
        && lhs.numIsStd()          == rhs.numIsStd()
        && lhs.numLeaps()          == rhs.numLeaps()
        && lhs.numTransitions()    == rhs.numTransitions()
        && lhs.numLocalTimeTypes() == rhs.numLocalTimeTypes()
        && lhs.abbrevDataSize()    == rhs.abbrevDataSize();
}

inline
bool baltzo::operator!=(const ZoneinfoBinaryHeader& lhs,
                        const ZoneinfoBinaryHeader& rhs)
{
    return !(lhs == rhs);
}

inline
bsl::ostream& baltzo::operator<<(bsl::ostream&               stream,
                                 const ZoneinfoBinaryHeader& object)
    // Write a single line description of the specified 'description' to the
    // specified 'stream' and a reference to the modifiable 'stream'.
{
    return object.print(stream, 0, -1);
}

// FREE FUNCTIONS
inline
void baltzo::swap(ZoneinfoBinaryHeader& a, ZoneinfoBinaryHeader& b)
{
    a.swap(b);
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
