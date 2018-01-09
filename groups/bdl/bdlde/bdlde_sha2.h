// bdlde_sha2.h                                                       -*-C++-*-

#ifndef INCLUDED_BDLDE_SHA2
#define INCLUDED_BDLDE_SHA2

#include <bsls_ident.h>

BSLS_IDENT("$Id$ $CSID$")

//@PURPOSE: Provide SHA-2 cryptographic hashes.
//
//@CLASSES:
// Sha224: Implements SHA-224
// Sha256: Implements SHA-256
// Sha384: Implements SHA-384
// Sha512: Implements SHA-512
//
//@AUTHOR: David Stone (dstone50)
//
//@SEE_ALSO:
//
//@DESCRIPTION: This component provides a set of classes ('Sha224', 'Sha256',
// 'Sha384', and 'Sha512') that implement the SHA-2 family of cryptographic
// functions.  See https://en.wikipedia.org/wiki/SHA-2 and
// http://csrc.nist.gov/publications/fips/fips180-4/fips-180-4.pdf
//
///Usage
///-----
// In this section we show intended usage of this component.
//
//  void senderExample(Out& output)
//      // Write a message and its SHA-2 hash to the specified 'output'
//      // stream.
//  {
//      // Prepare a message.
//      bsl::string message = "This is a test message.";
//
//      // Write the message to 'output'.
//      output << message;
//
//      // Generate a digest for 'message'.
//      bdlde::Sha512 hasher;
//      hasher.update(message.data(), message.length());
//      bsl::array<unsigned char, bdlde::Sha512::DIGEST_SIZE> digest;
//      hasher.finalize(digest.data());
//      output << digest;
//  }
//..
//  void receiverExample(In& input)
//      // Read a message and its SHA-2 hash from the specified 'input' stream,
//      // and verify the integrity of the message.
//  {
//      // Read the message from 'input'.
//      bsl::string message;
//      input >> message;
//
//      // Read the digest from 'input'.
//      bdlde::Sha512 hasher;
//      hasher.update(message.data(), message.length());
//      typedef bsl::array<unsigned char, bdlde::Sha512::DIGEST_SIZE> Digest;
//      Digest computed;
//      hasher.finalize(computed.data());
//
//      Digest received;
//      input >> received;
//      if (computed != received) handleError();
//  }

#include <bdlscm_version.h>

#include <bsl_cstddef.h>
#include <bsl_cstdint.h>

namespace BloombergLP {
namespace bdlde {

class Sha224 {
    // This 'class' represents a SHA-224 digest that can be updated as
    // additional data is provided.

    // DATA
    bsl::uint64_t d_totalSize;
    bsl::uint64_t d_blockBytesUsed;
    unsigned char d_block[512 / 8];
    bsl::uint32_t d_state[8];

  public:
    // TYPES
    static const bsl::size_t k_DIGEST_SIZE = 224 / 8;
        // The size (in bytes) of the output

    // CREATORS
    Sha224();
        // Construct a SHA-2 digest having the value corresponding to no data
        // having been provided.

    // MANIPULATORS
    void update(const void *message, bsl::size_t length);
        // Update the value of this SHA-2 digest to incorporate the specified
        // 'message' having the specified 'length' in bytes.  If the current
        // state is the default state, the resultant value of this SHA-2
        // digest is the application of the SHA-2 algorithm upon the currently
        // given 'message' of the given 'length'.  If this digest has been
        // previously provided data and has not been subsequently assigned to,
        // the current state is equivalent to applying the SHA-2 algorithm to
        // the concatenation of all the provided messages.  The behavior is
        // undefined unless finalize has not been called on this object and
        // the range '[message, message + length)' is a valid range.
    void finalize(unsigned char *digest);
        // Load the value of this SHA-2 digest into the specified 'digest'.
};

class Sha256 {
    // This 'class' represents a SHA-256 digest that can be updated as
    // additional data is provided.

    // DATA
    bsl::uint64_t d_totalSize;
    bsl::uint64_t d_blockBytesUsed;
    unsigned char d_block[512 / 8];
    bsl::uint32_t d_state[8];

  public:
    // TYPES
    static const bsl::size_t k_DIGEST_SIZE = 256 / 8;
        // The size (in bytes) of the output

    // CREATORS
    Sha256();
        // Construct a SHA-2 digest having the value corresponding to no data
        // having been provided.

    // MANIPULATORS
    void update(const void *message, bsl::size_t length);
        // Update the value of this SHA-2 digest to incorporate the specified
        // 'message' having the specified 'length' in bytes.  If the current
        // state is the default state, the resultant value of this SHA-2
        // digest is the application of the SHA-2 algorithm upon the currently
        // given 'message' of the given 'length'.  If this digest has been
        // previously provided data and has not been subsequently assigned to,
        // the current state is equivalent to applying the SHA-2 algorithm to
        // the concatenation of all the provided messages.  The behavior is
        // undefined unless finalize has not been called on this object and
        // the range '[message, message + length)' is a valid range.
    void finalize(unsigned char *digest);
        // Load the value of this SHA-2 digest into the specified 'digest'.
};

class Sha384 {
    // This 'class' represents a SHA-384 digest that can be updated as
    // additional data is provided.

    // DATA
    bsl::uint64_t d_totalSize;
    bsl::uint64_t d_blockBytesUsed;
    unsigned char d_block[1024 / 8];
    bsl::uint64_t d_state[8];

  public:
    // TYPES
    static const bsl::size_t k_DIGEST_SIZE = 384 / 8;
        // The size (in bytes) of the output

    // CREATORS
    Sha384();
        // Construct a SHA-2 digest having the value corresponding to no data
        // having been provided.

    // MANIPULATORS
    void update(const void *message, bsl::size_t length);
        // Update the value of this SHA-2 digest to incorporate the specified
        // 'message' having the specified 'length' in bytes.  If the current
        // state is the default state, the resultant value of this SHA-2
        // digest is the application of the SHA-2 algorithm upon the currently
        // given 'message' of the given 'length'.  If this digest has been
        // previously provided data and has not been subsequently assigned to,
        // the current state is equivalent to applying the SHA-2 algorithm to
        // the concatenation of all the provided messages.  The behavior is
        // undefined unless finalize has not been called on this object and
        // the range '[message, message + length)' is a valid range.
    void finalize(unsigned char *digest);
        // Load the value of this SHA-2 digest into the specified 'digest'.
};

class Sha512 {
    // This 'class' represents a SHA-512 digest that can be updated as
    // additional data is provided.

    // DATA
    bsl::uint64_t d_totalSize;
    bsl::uint64_t d_blockBytesUsed;
    unsigned char d_block[1024 / 8];
    bsl::uint64_t d_state[8];

  public:
    // TYPES
    static const bsl::size_t k_DIGEST_SIZE = 512 / 8;
        // The size (in bytes) of the output

    // CREATORS
    Sha512();
        // Construct a SHA-2 digest having the value corresponding to no data
        // having been provided.

    // MANIPULATORS
    void update(const void *message, bsl::size_t length);
        // Update the value of this SHA-2 digest to incorporate the specified
        // 'message' having the specified 'length' in bytes.  If the current
        // state is the default state, the resultant value of this SHA-2
        // digest is the application of the SHA-2 algorithm upon the currently
        // given 'message' of the given 'length'.  If this digest has been
        // previously provided data and has not been subsequently assigned to,
        // the current state is equivalent to applying the SHA-2 algorithm to
        // the concatenation of all the provided messages.  The behavior is
        // undefined unless finalize has not been called on this object and
        // the range '[message, message + length)' is a valid range.
    void finalize(unsigned char *digest);
        // Load the value of this SHA-2 digest into the specified 'digest'.
};

} // close package namespace
} // close enterprise namespace

#endif // INCLUDED_BDLDE_SHA2
