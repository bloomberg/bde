// bdlb_guid.h                                                        -*-C++-*-
#ifndef INCLUDED_BDLB_GUID
#define INCLUDED_BDLB_GUID

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

//@PURPOSE: Provide a value-semantic type for Globally Unique Identifiers.
//
//@CLASSES:
//  bdlb::Guid: value-semantic type to represent Globally Unique Identifiers
//
//@SEE_ALSO: bdlb_guidutil
//
//@DESCRIPTION: This component provides a value-semantic type for Globally
// Unique Identifiers (GUIDs), 'bdlb::Guid', with format as described by RFC
// 4122 (<http://http://www.ietf.org/rfc/rfc4122.txt>).  All equality and
// comparison methods are defined for these GUIDs.  Note that this component
// does not provide the facilities to generate GUIDs, and thus makes no
// guarantees of uniqueness or randomness.
//
///Usage
///-----
// Suppose we are building a utility to create globally unique names which may
// be based on a common base name, such as a code-generator.
//
// First, let us define the core types needed, the first of which is a utility
// to allocate GUIDs.
//..
//  struct MyGuidGeneratorUtil {
//      // This struct provides a namespace for methods to generate GUIDs.
//
//      // CLASS METHODS
//      static int generate(bdlb::Guid *guid);
//          // Generate a version 1 GUID, placing the value into the
//          // specified 'guid' pointer.  Return 0 on success, and non-zero
//          // otherwise.
//  };
//
//  // CLASS METHODS
//  inline
//  int my_GuidGeneratorUtil::generate(bdlb::Guid *guid)
//  {
//      // For brevity, we use a static sequence of pre-generated GUIDs.
//
//      static unsigned char GUIDS[][bdlb::Guid::k_GUID_NUM_BYTES] = {
//          { 0xde, 0xad, 0xbe, 0xef, 0xca, 0xfe, 0xba, 0xbe,
//            0x91, 0x91, 0x08, 0x00, 0x20, 0x0c, 0x9a, 0x66 },
//
//          { 0x5c, 0x9d, 0x4e, 0x51, 0x0d, 0xf1, 0x11, 0xe4,
//            0x91, 0x91, 0x08, 0x00, 0x20, 0x0c, 0x9a, 0x66 },
//
//          { 0x5c, 0x9d, 0x4e, 0x52, 0x0d, 0xf1, 0x11, 0xe4,
//            0x91, 0x91, 0x08, 0x00, 0x20, 0x0c, 0x9a, 0x66 },
//
//          { 0x5c, 0x9d, 0x4e, 0x53, 0x0d, 0xf1, 0x11, 0xe4,
//            0x91, 0x91, 0x08, 0x00, 0x20, 0x0c, 0x9a, 0x66 },
//
//          { 0x5c, 0x9d, 0x4e, 0x54, 0x0d, 0xf1, 0x11, 0xe4,
//            0x91, 0x91, 0x08, 0x00, 0x20, 0x0c, 0x9a, 0x66 },
//
//          { 0x5c, 0x9d, 0x4e, 0x55, 0x0d, 0xf1, 0x11, 0xe4,
//            0x91, 0x91, 0x08, 0x00, 0x20, 0x0c, 0x9a, 0x66 },
//
//          { 0x5c, 0x9d, 0x4e, 0x56, 0x0d, 0xf1, 0x11, 0xe4,
//            0x91, 0x91, 0x08, 0x00, 0x20, 0x0c, 0x9a, 0x66 },
//      };
//
//      const bsl::size_t NUM_GUIDS = sizeof GUIDS / sizeof *GUIDS;
//
//      static bsl::size_t nextGuidIdx = 0;
//
//      int rval = -1;
//      if (nextGuidIdx++ < NUM_GUIDS) {
//          *guid = bdlb::Guid(GUIDS[nextGuidIdx]);
//          rval = 0;
//      }
//      return rval;
// }
//..
//  Next, we create a utility to create unique strings.
//..
//  struct UniqueStringGenerator {
//      // This struct provides methods to create globally unique strings.
//
//      static int uniqueStringFromBase(bsl::string        *unique,
//                                      const bsl::string&  base);
//            // Create a globally unique string from the specified non-unique
//            // 'base' string, placing the result into the specified 'unique'
//            // string pointer.
//
//  };
//
//  int
//  UniqueStringGenerator::uniqueStringFromBase(bsl::string        *unique,
//                                              const bsl::string&  base,)
//  {
//      bdlb::Guid guid;
//
//      int rval = my_GuidGeneratorUtil::generate(&guid);
//      if (rval == 0) {
//      {
//          ostringstream convert;
//          convert << base << "-" << guid;
//          *unique = convert.str();
//      }
//      return rval;
//  }
//..
//  Finally, we implement a program to generate unique names for a code
//  auto-generator.
//..
//  bsl::string baseFileName = "foo.cpp";
//  bsl::string uniqueFileName;
//  bsl::string previousFileName;
//
//  const bsl::size_t NUM_FILES = 5;
//  for (bsl::size_t i = 0; i < NUM_FILES; ++i) {
//      UniqueStringGenerator::uniqueStringFromBase(&uniqueFileName,
//                                                  baseFileName);
//      assert(previousFileName != uniqueFileName);
//       previousFileName = uniqueFileName;
//  }
//..

#ifndef INCLUDED_BSL_ALGORITHM
#include <bsl_algorithm.h>
#endif

#ifndef INCLUDED_BSL_CSTDDEF
#include <bsl_cstddef.h>
#endif

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif

#ifndef INCLUDED_BSLMF_ISBITWISEEQUALITYCOMPARABLE
#include <bslmf_isbitwiseequalitycomparable.h>
#endif

#ifndef INCLUDED_BSLMF_ISTRIVIALLYCOPYABLE
#include <bslmf_istriviallycopyable.h>
#endif

#ifndef INCLUDED_BSLMF_NESTEDTRAITDECLARATION
#include <bslmf_nestedtraitdeclaration.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSLS_TYPES
#include <bsls_types.h>
#endif

namespace BloombergLP {
namespace bdlb {
                                // ==========
                                // bdlb::Guid
                                // ==========
class Guid {
    // This class implements a value-semantic 'Guid' type.  Each object
    // represents an unconstrained 'Guid' object, but its uniqueness is *not*
    // guaranteed, and this component provides no ability to generate a GUID.
    //
    // This class provides a constructor and several accessors with names and
    // parameters phrased using RFC 4122 field names.  These names are used (by
    // RFC 4122 and this component) as designators for parts of the GUID even
    // when those names do not accurately describe the parts (for example,
    // 'time low' names bytes 0-3 of the GUID regardless of whether the values
    // of those bytes come from a clock or are generated randomly).

  public:
    // CLASS DATA
    enum { k_GUID_NUM_BYTES = 16 };            // number of bytes in a guid

    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(Guid, bslmf::IsBitwiseEqualityComparable)
    BSLMF_NESTED_TRAIT_DECLARATION(Guid, bsl::is_trivially_copyable)

  private:
    // DATA
    unsigned char d_buffer[k_GUID_NUM_BYTES];  // byte array to hold the guid

    // FRIENDS
    friend bool operator==(const Guid& lhs, const Guid& rhs);
    friend bool operator!=(const Guid& lhs, const Guid& rhs);

  public:
    // CREATORS
    Guid();
        // Construct a zero-initialized guid object.  Note that a zero-
        // initialized guid object is not a GUID according to RFC 4122.

    //! ~Guid() = default;
        // Destroy this object

    explicit Guid(const unsigned char (&buffer)[k_GUID_NUM_BYTES]);
        // Construct a guid object with the internal buffer set equal to the
        // specified 'buffer' with the first byte representing the most
        // significant byte.  Note that this method does guarantee that the
        // created guid object is a GUID.

    Guid(unsigned long       timeLow,
         unsigned short      timeMid,
         unsigned short      timeHiAndVersion,
         unsigned char       clockSeqHiRes,
         unsigned char       clockSeqLow,
         bsls::Types::Uint64 node);
        // Construct a guid object with an internal buffer composed from the
        // specified 'timeLow', 'timeMid', 'timeHiAndVersion', 'clockSeqHiRes',
        // 'clockSeqLow', and 'node' as specified by RFC 4122.  Note that only
        // the least significant 48 bits of 'node' are used in constructing
        // the guid.

    //! Guid(const Guid& original) = default;
        // Construct a guid object having the same value as the specified
        // 'original' object.

    // MANIPULATORS
    //! Guid& operator=(const Guid& rhs) = default;
        // Assign to this guid object the value of the specified 'rhs' and
        // return a reference to this modifiable object.

    Guid& operator=(const unsigned char (&buffer)[k_GUID_NUM_BYTES]);
        // Assign to the buffer of this guid the byte sequence in the specified
        // 'buffer'.

    // ACCESSORS
    const unsigned char& operator[](bsl::size_t offset) const;
        // Return a reference offering unmodifiable access to the byte at the
        // specified 'offset' from the most significant byte of this guid
        // object.  The behavior is undefined unless
        // '0 <= offset < k_GUID_NUM_BYTES'.

    const unsigned char *begin() const;
    const unsigned char *data() const;
        // Return a pointer offering unmodifiable access to the most
        // significant byte of this guid object.

    const unsigned char *end() const;
        // Return a pointer one past the end of the least significant byte of
        // this guid object.

                        // RFC 4122 FIELD ACCESSORS

    unsigned char clockSeqHi() const;
        // Return the 5-bit value of the 'clk_seq_hi_res' field of this guid as
        // specified in RFC 4122, excluding the variant bits.

    unsigned char clockSeqHiRes() const;
        // Return the 8-bit 'clk_seq_hi_res' field of this guid as specified in
        // RFC 4122.

    unsigned char clockSeqLow() const;
        // Return the 8-bit 'clk_seq_low' field of this guid as specified in
        // RFC 4122.

    bsls::Types::Uint64 node() const;
        // Return the 48-bit 'node' field of this guid as specified in RFC
        // 4122.

    unsigned short timeHi() const;
        // Return the 12-bit value of the 'time_hi_and_version' field of this
        // guid as specified in RFC 4122, excluding the 'version' bits.

    unsigned short timeHiAndVersion() const;
        // Return the 16-bit 'time_hi_and_version' field of this guid as
        // specified in RFC 4122.

    unsigned long timeLow() const;
        // Return the 32-bit 'time_low' field of this guid as specified in RFC
        // 4122.

    unsigned short timeMid() const;
        // Return the 16-bit 'time_mid' field of this guid as specified in RFC
        // 4122.

    unsigned char variant() const;
        // Return the 3-bit 'variant' portion of the 'clk_seq_hi_res' field of
        // this guid as specified in RFC 4122.

    unsigned char version() const;
        // Return the four-bit 'version' portion of the 'time_hi_and_version'
        // field of this guid as specified in RFC 4122.

    // ASPECTS
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
        // valid on entry, this operation has no effect.  Note that this
        // human-readable format is not fully specified, and can change without
        // notice.
};

// FREE OPERATORS
bool operator==(const Guid& lhs, const Guid& rhs);
    // Return 'true' if the specified 'lhs' and specified 'rhs' guid objects
    // have the same value, and 'false' otherwise.  Two guid objects have the
    // same value if each corresponding byte in their internal buffers are
    // equal.

bool operator!=(const Guid& lhs, const Guid& rhs);
    // Return 'true' if the specified 'lhs' and specified 'rhs' guid objects
    // have different values, and 'false' otherwise.  Two guid objects have
    // different value if any of corresponding byte in their internal buffers
    // differ.

bool operator< (const Guid& lhs, const Guid& rhs);
    // Return 'true' if the value of the specified 'lhs' guid object is less
    // than the value of the specified 'rhs' guid object, and 'false'
    // otherwise.  Note that the comparison is accomplished using a
    // lexicographic comparison of the internal representations.

bool operator<=(const Guid& lhs, const Guid& rhs);
    // Return 'true' if the value of the specified 'lhs' guid object is less
    // than or equal to the value of the specified 'rhs' guid object, and
    // 'false' otherwise.  Note that the comparison is accomplished using  a
    // lexicographic comparison of the internal representations.

bool operator> (const Guid& lhs, const Guid& rhs);
    // Return 'true' if the value of the specified 'lhs' guid object is greater
    // than the value of the specified 'rhs' guid object, and 'false'
    // otherwise.  Note that the comparison is accomplished using a
    // lexicographic comparison of the internal representations.

bool operator>=(const Guid& lhs, const Guid& rhs);
    // Return 'true' if the value of the specified 'lhs' guid object is greater
    // than or equal to the value of the specified 'rhs' guid object, and
    // 'false' otherwise.  Note that the comparison is accomplished using a
    // lexicographic comparison of the internal representations.

bsl::ostream& operator<<(bsl::ostream& stream, const Guid& guid);
    // Write the value of the specified 'guid' object to the specified output
    // 'stream' in a single-line format, and return a reference to 'stream'.
    // If 'stream' is not valid on entry, this operation has no effect.  Note
    // that this human-readable format is not fully specified, can change
    // without notice, and is logically equivalent to:
    //..
    //  print(stream, 0, -1);
    //..

template <class HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& hashAlgorithm, const Guid& guid);
    // Invoke the specified 'hashAlgorithm' on the underlying buffer held by
    // the specified 'guid' object.

// ============================================================================
//                      INLINE DEFINITIONS
// ============================================================================

                                // ----------
                                // bdlb::Guid
                                // ----------
// CREATORS
inline
Guid::Guid()
{
    bsl::fill(d_buffer, d_buffer + k_GUID_NUM_BYTES, 0);
}

inline
Guid::Guid(const unsigned char (&buffer)[k_GUID_NUM_BYTES])
{
    bsl::copy(buffer, buffer + k_GUID_NUM_BYTES, d_buffer);
}

inline Guid::Guid(unsigned long       timeLow,
                  unsigned short      timeMid,
                  unsigned short      timeHiAndVersion,
                  unsigned char       clockSeqHiRes,
                  unsigned char       clockSeqLow,
                  bsls::Types::Uint64 node)
{
    typedef unsigned char uc;

    d_buffer[ 0] = uc(timeLow >> 24);
    d_buffer[ 1] = uc(timeLow >> 16);
    d_buffer[ 2] = uc(timeLow >>  8);
    d_buffer[ 3] = uc(timeLow);

    d_buffer[ 4] = uc(timeMid >> 8);
    d_buffer[ 5] = uc(timeMid);

    d_buffer[ 6] = uc(timeHiAndVersion >> 8);
    d_buffer[ 7] = uc(timeHiAndVersion);

    d_buffer[ 8] = uc(clockSeqHiRes);

    d_buffer[ 9] = uc(clockSeqLow);

    d_buffer[10] = uc(node >> 40);
    d_buffer[11] = uc(node >> 32);
    d_buffer[12] = uc(node >> 24);
    d_buffer[13] = uc(node >> 16);
    d_buffer[14] = uc(node >>  8);
    d_buffer[15] = uc(node);
}

// MANIPULATORS
inline
Guid& Guid::operator=(const unsigned char (&buffer)[k_GUID_NUM_BYTES])
{
    bsl::copy(buffer, buffer + k_GUID_NUM_BYTES, d_buffer);
    return *this;
}

// ACCESSORS
inline
const unsigned char& Guid::operator[](bsl::size_t offset) const
{
    BSLS_ASSERT_SAFE(offset < k_GUID_NUM_BYTES);
    return d_buffer[offset];
}

inline
const unsigned char *Guid::begin() const
{
    return d_buffer;
}

inline
const unsigned char *Guid::data() const
{
    return d_buffer;
}

inline
const unsigned char *Guid::end() const
{
    return d_buffer + k_GUID_NUM_BYTES;
}

                        // RFC 4122 FIELD ACCESSORS

inline
unsigned char Guid::clockSeqHi() const
{
    return clockSeqHiRes() & 0x1F;
}

inline
unsigned char Guid::clockSeqHiRes() const
{
    return d_buffer[8];
}

inline
unsigned char Guid::clockSeqLow() const
{
    return d_buffer[9];
}

inline
bsls::Types::Uint64 Guid::node() const
{
    return bsls::Types::Uint64(d_buffer[10]) << 40 |
           bsls::Types::Uint64(d_buffer[11]) << 32 |
           bsls::Types::Uint64(d_buffer[12]) << 24 |
           bsls::Types::Uint64(d_buffer[13]) << 16 |
           bsls::Types::Uint64(d_buffer[14]) <<  8 |
                               d_buffer[15];
}

inline
unsigned short Guid::timeHi() const
{
    return timeHiAndVersion() & 0x0FFF;
}

inline
unsigned short Guid::timeHiAndVersion() const
{
    typedef unsigned short us;
    return us(d_buffer[6] << 8 |
              d_buffer[7]);
}

inline
unsigned long Guid::timeLow() const {
    typedef unsigned long ul;
    return ul(d_buffer[0]) << 24 |
              d_buffer[1]  << 16 |
              d_buffer[2]  <<  8 |
              d_buffer[3];
}

inline
unsigned short Guid::timeMid() const {
    typedef unsigned short us;
    return us(d_buffer[4] << 8 |
              d_buffer[5]);
}

inline
unsigned char Guid::variant() const {
    typedef unsigned char uc;
    return uc(clockSeqHiRes() >> 5);
}

inline
unsigned char Guid::version() const {
    typedef unsigned char uc;
    return uc(timeHiAndVersion() >> 12);
}

}  // close package namespace

// FREE OPERATORS
inline
bsl::ostream& bdlb::operator<<(bsl::ostream& stream, const bdlb::Guid& guid)
{
    return guid.print(stream, 0, -1);
}

inline
bool bdlb::operator==(const bdlb::Guid& lhs, const bdlb::Guid& rhs)
{
    return bsl::equal(
        lhs.d_buffer, lhs.d_buffer + lhs.k_GUID_NUM_BYTES, rhs.d_buffer);
}

inline
bool bdlb::operator!=(const bdlb::Guid& lhs, const bdlb::Guid& rhs)
{
    return !bsl::equal(
        lhs.d_buffer, lhs.d_buffer + lhs.k_GUID_NUM_BYTES, rhs.d_buffer);
}

template <class HASH_ALGORITHM>
void bdlb::hashAppend(HASH_ALGORITHM& hashAlgorithm, const Guid& guid)
{
    hashAlgorithm(guid.data(), Guid::k_GUID_NUM_BYTES);
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
