// bdlde_sha2.h                                                       -*-C++-*-

#ifndef INCLUDED_BDLDE_SHA2
#define INCLUDED_BDLDE_SHA2

#include <bsls_ident.h>

BSLS_IDENT("$Id$ $CSID$")

//@PURPOSE: Provide SHA-2 cryptographic hashes.
//
//@CLASSES:
//  bdlde::Sha224: value-semantic type representing a SHA-224 digest
//  bdlde::Sha256: value-semantic type representing a SHA-256 digest
//  bdlde::Sha384: value-semantic type representing a SHA-384 digest
//  bdlde::Sha512: value-semantic type representing a SHA-512 digest
//
//@AUTHOR: David Stone (dstone50)
//
//@SEE_ALSO: bdlde_md5
//
//@DESCRIPTION: This component provides a set of classes ('Sha224', 'Sha256',
// 'Sha384', and 'Sha512') that implement a mechanism for computing and updating a SHA-2 digest (a cryptographic hash).  The specification for this is based on FIPS-180, which can be found at
//..
//  http://csrc.nist.gov/publications/fips/fips180-4/fips-180-4.pdf
//..
//
// Note that a SHA-2 digest does not aid in error correction.
//
///Usage
///-----
// In this section we show intended usage of this component.
//..
//  void validatePassword()
//      // Ensure that a given password ("password") has the correct hash
//  {
//      // Prepare a message.
//      const bsl::string message = "password";
//
//      // Generate a digest for 'message'.
//      bdlde::Sha512 hasher;
//      hasher.update(message.data(), message.length());
//      unsigned char       digest[bdlde::Sha512::k_DIGEST_SIZE];
//      const unsigned char expected[bdlde::Sha512::k_DIGEST_SIZE] = {
//          0xB1, 0x09, 0xF3, 0xBB, 0xBC, 0x24, 0x4E, 0xB8, 0x24, 0x41, 0x91,
//          0x7E, 0xD0, 0x6D, 0x61, 0x8B, 0x90, 0x08, 0xDD, 0x09, 0xB3, 0xBE,
//          0xFD, 0x1B, 0x5E, 0x07, 0x39, 0x4C, 0x70, 0x6A, 0x8B, 0xB9, 0x80,
//          0xB1, 0xD7, 0x78, 0x5E, 0x59, 0x76, 0xEC, 0x04, 0x9B, 0x46, 0xDF,
//          0x5F, 0x13, 0x26, 0xAF, 0x5A, 0x2E, 0xA6, 0xD1, 0x03, 0xFD, 0x07,
//          0xC9, 0x53, 0x85, 0xFF, 0xAB, 0x0C, 0xAC, 0xBC, 0x86
//      };
//      hasher.loadDigest(digest);
//      ASSERT(bsl::equal(digest,
//                        digest + bdlde::Sha512::k_DIGEST_SIZE,
//                        expected));
//  }
//..

#include <bdlscm_version.h>

#include <bsl_cstddef.h>
#include <bsl_cstdint.h>
#include <bsl_iosfwd.h>

namespace BloombergLP {
namespace bdlde {

class Sha224 {
    // This 'class' represents a SHA-224 digest that can be updated as
    // additional data is provided.

    // DATA
    bsl::uint64_t d_totalSize;       // length of the entire message
    bsl::uint64_t d_bufferSize;      // bytes currently used for 'd_buffer'
    unsigned char d_buffer[512 / 8]; // buffer for storing remaining part of
                                     // message that is not yet incorporated
                                     // into 'd_state'
    bsl::uint32_t d_state[8];        // state array storing the digest

    // FRIENDS
    friend bool operator==(const Sha224&, const Sha224&);

  public:
    // TYPES
    static const bsl::size_t k_DIGEST_SIZE = 224 / 8;
        // The size (in bytes) of the output

    // CREATORS
    Sha224();
        // Construct a SHA-2 digest having the value corresponding to no data
        // having been provided.

    // MANIPULATORS
    void reset();
        // Reset the value of this SHA-2 digest to the value provided by the
        // default constructor.

    void update(const void *data, bsl::size_t length);
        // Update the value of this SHA-2 digest to incorporate the specified
        // 'data' having the specified 'length' in bytes.  If the current
        // state is the default state, the resultant value of this SHA-2
        // digest is the application of the SHA-2 algorithm upon the currently
        // given 'data' of the given 'length'.  If this digest has been
        // previously provided data and has not been subsequently assigned to,
        // the current state is equivalent to applying the SHA-2 algorithm to
        // the concatenation of all the provided messages.  The behavior is
        // undefined unless the range '[data, data + length)' is a valid range.

    void loadDigestAndReset(unsigned char *digest);
        // Load the current value of this SHA-2 digest into the specified
        // 'digest' and set the value of this SHA-2 digest to the value
        // provided by the default constructor.

    // ACCESSORS
    void loadDigest(unsigned char *digest) const;
        // Load the value of this SHA-2 digest into the specified 'digest'.

    bsl::ostream& print(bsl::ostream& stream) const;
        // Format the current value of this SHA-2 digest to the specified
        // output 'stream' and return a reference to the modifiable 'stream'.
};

class Sha256 {
    // This 'class' represents a SHA-256 digest that can be updated as
    // additional data is provided.

    // DATA
    bsl::uint64_t d_totalSize;       // length of the entire message
    bsl::uint64_t d_bufferSize;      // bytes currently used for 'd_buffer'
    unsigned char d_buffer[512 / 8]; // buffer for storing remaining part of
                                     // message that is not yet incorporated
                                     // into 'd_state'
    bsl::uint32_t d_state[8];        // state array storing the digest

    // FRIENDS
    friend bool operator==(const Sha256&, const Sha256&);

  public:
    // TYPES
    static const bsl::size_t k_DIGEST_SIZE = 256 / 8;
        // The size (in bytes) of the output

    // CREATORS
    Sha256();
        // Construct a SHA-2 digest having the value corresponding to no data
        // having been provided.

    // MANIPULATORS
    void reset();
        // Reset the value of this SHA-2 digest to the value provided by the
        // default constructor.

    void update(const void *data, bsl::size_t length);
        // Update the value of this SHA-2 digest to incorporate the specified
        // 'data' having the specified 'length' in bytes.  If the current
        // state is the default state, the resultant value of this SHA-2
        // digest is the application of the SHA-2 algorithm upon the currently
        // given 'data' of the given 'length'.  If this digest has been
        // previously provided data and has not been subsequently assigned to,
        // the current state is equivalent to applying the SHA-2 algorithm to
        // the concatenation of all the provided messages.  The behavior is
        // undefined unless the range '[data, data + length)' is a valid range.

    void loadDigestAndReset(unsigned char *digest);
        // Load the current value of this SHA-2 digest into the specified
        // 'digest' and set the value of this SHA-2 digest to the value
        // provided by the default constructor.

    // ACCESSORS
    void loadDigest(unsigned char *digest) const;
        // Load the value of this SHA-2 digest into the specified 'digest'.

    bsl::ostream& print(bsl::ostream& stream) const;
        // Format the current value of this SHA-2 digest to the specified
        // output 'stream' and return a reference to the modifiable 'stream'.
};

class Sha384 {
    // This 'class' represents a SHA-384 digest that can be updated as
    // additional data is provided.

    // DATA
    bsl::uint64_t d_totalSize;        // length of the entire message
    bsl::uint64_t d_bufferSize;       // bytes currently used for 'd_buffer'
    unsigned char d_buffer[1024 / 8]; // buffer for storing remaining part of
                                      // message that is not yet incorporated
                                      // into 'd_state'
    bsl::uint64_t d_state[8];         // state array storing the digest

    // FRIENDS
    friend bool operator==(const Sha384&, const Sha384&);

  public:
    // TYPES
    static const bsl::size_t k_DIGEST_SIZE = 384 / 8;
        // The size (in bytes) of the output

    // CREATORS
    Sha384();
        // Construct a SHA-2 digest having the value corresponding to no data
        // having been provided.

    // MANIPULATORS
    void reset();
        // Reset the value of this SHA-2 digest to the value provided by the
        // default constructor.

    void update(const void *data, bsl::size_t length);
        // Update the value of this SHA-2 digest to incorporate the specified
        // 'data' having the specified 'length' in bytes.  If the current
        // state is the default state, the resultant value of this SHA-2
        // digest is the application of the SHA-2 algorithm upon the currently
        // given 'data' of the given 'length'.  If this digest has been
        // previously provided data and has not been subsequently assigned to,
        // the current state is equivalent to applying the SHA-2 algorithm to
        // the concatenation of all the provided messages.  The behavior is
        // undefined unless the range '[data, data + length)' is a valid range.

    void loadDigestAndReset(unsigned char *digest);
        // Load the current value of this SHA-2 digest into the specified
        // 'digest' and set the value of this SHA-2 digest to the value
        // provided by the default constructor.

    // ACCESSORS
    void loadDigest(unsigned char *digest) const;
        // Load the value of this SHA-2 digest into the specified 'digest'.

    bsl::ostream& print(bsl::ostream& stream) const;
        // Format the current value of this SHA-2 digest to the specified
        // output 'stream' and return a reference to the modifiable 'stream'.
};

class Sha512 {
    // This 'class' represents a SHA-512 digest that can be updated as
    // additional data is provided.

    // DATA
    bsl::uint64_t d_totalSize;        // length of the entire message
    bsl::uint64_t d_bufferSize;       // bytes currently used for 'd_buffer'
    unsigned char d_buffer[1024 / 8]; // buffer for storing remaining part of
                                      // message that is not yet incorporated
                                      // into 'd_state'
    bsl::uint64_t d_state[8];         // state array storing the digest

    // FRIENDS
    friend bool operator==(const Sha512&, const Sha512&);

  public:
    // TYPES
    static const bsl::size_t k_DIGEST_SIZE = 512 / 8;
        // The size (in bytes) of the output

    // CREATORS
    Sha512();
        // Construct a SHA-2 digest having the value corresponding to no data
        // having been provided.

    // MANIPULATORS
    void reset();
        // Reset the value of this SHA-2 digest to the value provided by the
        // default constructor.

    void update(const void *data, bsl::size_t length);
        // Update the value of this SHA-2 digest to incorporate the specified
        // 'data' having the specified 'length' in bytes.  If the current
        // state is the default state, the resultant value of this SHA-2
        // digest is the application of the SHA-2 algorithm upon the currently
        // given 'data' of the given 'length'.  If this digest has been
        // previously provided data and has not been subsequently assigned to,
        // the current state is equivalent to applying the SHA-2 algorithm to
        // the concatenation of all the provided messages.  The behavior is
        // undefined unless the range '[data, data + length)' is a valid range.

    void loadDigestAndReset(unsigned char *digest);
        // Load the current value of this SHA-2 digest into the specified
        // 'digest' and set the value of this SHA-2 digest to the value
        // provided by the default constructor.

    // ACCESSORS
    void loadDigest(unsigned char *digest) const;
        // Load the value of this SHA-2 digest into the specified 'digest'.

    bsl::ostream& print(bsl::ostream& stream) const;
        // Format the current value of this SHA-2 digest to the specified
        // output 'stream' and return a reference to the modifiable 'stream'.
};

// FREE OPERATORS
bool operator==(const Sha224& lhs, const Sha224& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' SHA digests have the same
    // value, and 'false' otherwise.  Two digests have the same value if the
    // values obtained from their respective 'loadDigest' methods are identical.

inline
bool operator!=(const Sha224& lhs, const Sha224& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' SHA digests do not have
    // the same value, and 'false' otherwise.  Two digests do not have the same
    // value if the values obtained from their respective 'loadDigest' methods
    // differ.

bool operator==(const Sha256& lhs, const Sha256& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' SHA digests have the same
    // value, and 'false' otherwise.  Two digests have the same value if the
    // values obtained from their respective 'loadDigest' methods are identical.

inline
bool operator!=(const Sha256& lhs, const Sha256& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' SHA digests do not have
    // the same value, and 'false' otherwise.  Two digests do not have the same
    // value if the values obtained from their respective 'loadDigest' methods
    // differ.

bool operator==(const Sha384& lhs, const Sha384& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' SHA digests have the same
    // value, and 'false' otherwise.  Two digests have the same value if the
    // values obtained from their respective 'loadDigest' methods are identical.

inline
bool operator!=(const Sha384& lhs, const Sha384& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' SHA digests do not have
    // the same value, and 'false' otherwise.  Two digests do not have the same
    // value if the values obtained from their respective 'loadDigest' methods
    // differ.

bool operator==(const Sha512& lhs, const Sha512& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' SHA digests have the same
    // value, and 'false' otherwise.  Two digests have the same value if the
    // values obtained from their respective 'loadDigest' methods are identical.

inline
bool operator!=(const Sha512& lhs, const Sha512& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' SHA digests do not have
    // the same value, and 'false' otherwise.  Two digests do not have the same
    // value if the values obtained from their respective 'loadDigest' methods
    // differ.

inline
bsl::ostream& operator<<(bsl::ostream& stream, const Sha224& digest);
    // Write to the specified output 'stream' the specified SHA-2 'digest' and
    // return a reference to the modifiable 'stream'.

inline
bsl::ostream& operator<<(bsl::ostream& stream, const Sha256& digest);
    // Write to the specified output 'stream' the specified SHA-2 'digest' and
    // return a reference to the modifiable 'stream'.

inline
bsl::ostream& operator<<(bsl::ostream& stream, const Sha384& digest);
    // Write to the specified output 'stream' the specified SHA-2 'digest' and
    // return a reference to the modifiable 'stream'.

inline
bsl::ostream& operator<<(bsl::ostream& stream, const Sha512& digest);
    // Write to the specified output 'stream' the specified SHA-2 'digest' and
    // return a reference to the modifiable 'stream'.

// ============================================================================
//                        INLINE FUNCTION DEFINITIONS
// ============================================================================

                                 // ---------
                                 // class Sha224
                                 // ---------

// FREE OPERATORS
inline
bool operator!=(const Sha224& lhs, const Sha224& rhs)
{
    return !(lhs == rhs);
}

                                 // ---------
                                 // class Sha256
                                 // ---------

// FREE OPERATORS
inline
bool operator!=(const Sha256& lhs, const Sha256& rhs)
{
    return !(lhs == rhs);
}

                                 // ---------
                                 // class Sha384
                                 // ---------

// FREE OPERATORS
inline
bool operator!=(const Sha384& lhs, const Sha384& rhs)
{
    return !(lhs == rhs);
}

                                 // ---------
                                 // class Sha512
                                 // ---------

// FREE OPERATORS
inline
bool operator!=(const Sha512& lhs, const Sha512& rhs)
{
    return !(lhs == rhs);
}

inline
bsl::ostream& operator<<(bsl::ostream& stream, const Sha224& digest)
{
    return digest.print(stream);
}

inline
bsl::ostream& operator<<(bsl::ostream& stream, const Sha256& digest)
{
    return digest.print(stream);
}

inline
bsl::ostream& operator<<(bsl::ostream& stream, const Sha384& digest)
{
    return digest.print(stream);
}

inline
bsl::ostream& operator<<(bsl::ostream& stream, const Sha512& digest)
{
    return digest.print(stream);
}

} // close package namespace
} // close enterprise namespace

#endif // INCLUDED_BDLDE_SHA2
