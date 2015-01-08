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

//@PURPOSE: Provide a value semantic type for Globally Unique Identifiers.
//
//@CLASSES:
//  bdlb::Guid: value-semantic type to represent Globally Unique Identifiers
//
//@SEE_ALSO: bdlb_guidutil
//
//@AUTHOR: Mickey Sweatt (msweatt1)
//
//@DESCRIPTION: This component provides a value semantic type for Globally
// Unique Identifiers (GUIDs), 'bdlb::Guid'.  All equality and comparison
// methods are defined for these GUIDs.  In addition this type is zero-
// initialized to support compatibility with constructs which depend on this.
// Note that this component does not provide the facilities to generate GUIDs,
// and thus makes no guarantees of uniqueness.
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

namespace BloombergLP {
namespace bdlb {
                                // ==========
                                // bdlb::Guid
                                // ==========
class Guid {
    // This class implements a value-semantic 'Guid' type.  Each object
    // represents an unconstrained 'Guid' object, but its uniqueness is *not*
    // guaranteed, and this component provides no ability to generate a GUID.

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
        // Construct a zero initialized guid object.  Note that a zero-
        // initialized guid object is not a GUID.

    //! ~Guid() = default;
        // Destroy this object

    explicit Guid(const unsigned char (&buffer)[k_GUID_NUM_BYTES]);
        // Construct a guid object with the internal buffer set equal to the
        // specified 'buffer' with the first byte representing the most
        // significant byte.  Note that this method does guarantee that the
        // created guid object is a GUID.

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

bsl::ostream& operator<<(bsl::ostream& stream, const Guid& guid);
    // Write the value of the specified 'guid' object to the specified output
    // 'stream' in a single-line format, and return a reference to 'stream'.
    // If 'stream' is not valid on entry, this operation has no effect.  Note
    // that this human-readable format is not fully specified, can change
    // without notice, and is logically equivalent to:
    //..
    //  print(stream, 0, -1);
    //..

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
    BSLS_ASSERT_SAFE(&buffer);
    bsl::copy(buffer, buffer + k_GUID_NUM_BYTES, d_buffer);
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

}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2015
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ----------------------------------
