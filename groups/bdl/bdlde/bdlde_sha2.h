// bdlde_sha2.h                                                       -*-C++-*-
#ifndef INCLUDED_BDLDE_SHA2
#define INCLUDED_BDLDE_SHA2

#include <bsls_ident.h>
BSLS_IDENT("$Id$ $CSID$")

//@PURPOSE: Provide a value-semantic type encoding a message in a SHA-2 digest.
//
//@CLASSES:
//  bdlde::Sha224: value-semantic type representing a SHA-224 digest
//  bdlde::Sha256: value-semantic type representing a SHA-256 digest
//  bdlde::Sha384: value-semantic type representing a SHA-384 digest
//  bdlde::Sha512: value-semantic type representing a SHA-512 digest
//
//@SEE_ALSO: bdlde_md5
//
//@DESCRIPTION: This component provides a set of classes ('Sha224', 'Sha256',
// 'Sha384', and 'Sha512') that implement a mechanism for computing and
// updating a SHA-2 digest (a cryptographic hash).  The specification for this
// is based on FIPS-180, which can be found at
//..
//  http://csrc.nist.gov/publications/fips/fips180-4/fips-180-4.pdf
//..
//
// Note that a SHA-2 digest does not aid in error correction.
//
///Usage
///-----
// In this section we show intended usage of this component.  The
// 'validatePassword' function below returns whether a specified password has a
// specified hash value.  The 'assertPasswordIsExpected' function below has a
// sample password to hash and a hash value that matches it.  Note that the
// output of 'loadDigest' is a binary representation.  When hashes are
// displayed for human consumption, they are typically converted to hex, but
// that would create unnecessary overhead here.
//..
//  bool validatePassword(const bsl::string&   password,
//                        const bsl::string&   salt,
//                        const unsigned char *expected)
//      // Return 'true' if the specified 'password' concatenated with the
//      // specified 'salt' has a SHA-512 hash equal to the specified
//      // 'expected', and 'false' otherwise.
//  {
//      bdlde::Sha512 hasher;
//      hasher.update(password.c_str(), password.length());
//      hasher.update(salt.c_str(), salt.length());
//
//      unsigned char digest[bdlde::Sha512::k_DIGEST_SIZE];
//      hasher.loadDigest(digest);
//
//      return bsl::equal(digest,
//                        digest + bdlde::Sha512::k_DIGEST_SIZE,
//                        expected);
//  }
//
//  void assertPasswordIsExpected()
//      // Asserts that the constant string 'pass' salted with 'word' has the
//      // expected hash value.  In a real application, the expected hash would
//      // likely come from some sort of database.
//  {
//      const bsl::string   password = "pass";
//      const bsl::string   salt     = "word";
//      const unsigned char expected[bdlde::Sha512::k_DIGEST_SIZE] = {
//          0xB1, 0x09, 0xF3, 0xBB, 0xBC, 0x24, 0x4E, 0xB8, 0x24, 0x41, 0x91,
//          0x7E, 0xD0, 0x6D, 0x61, 0x8B, 0x90, 0x08, 0xDD, 0x09, 0xB3, 0xBE,
//          0xFD, 0x1B, 0x5E, 0x07, 0x39, 0x4C, 0x70, 0x6A, 0x8B, 0xB9, 0x80,
//          0xB1, 0xD7, 0x78, 0x5E, 0x59, 0x76, 0xEC, 0x04, 0x9B, 0x46, 0xDF,
//          0x5F, 0x13, 0x26, 0xAF, 0x5A, 0x2E, 0xA6, 0xD1, 0x03, 0xFD, 0x07,
//          0xC9, 0x53, 0x85, 0xFF, 0xAB, 0x0C, 0xAC, 0xBC, 0x86
//      };
//
//      ASSERT(validatePassword(password, salt, expected));
//  }
//..

#include <bdlscm_version.h>

#include <bsl_cstddef.h>
#include <bsl_cstdint.h>
#include <bsl_iosfwd.h>

namespace BloombergLP {
namespace bdlde {

                                 // ============
                                 // class Sha224
                                 // ============

class Sha224 {
    // This 'class' represents a SHA-224 digest that can be updated as
    // additional data is provided.
    //
    // More generally, this class supports a complete set of *value*
    // *semantic* operations, including copy construction, assignment, equality
    // comparison, and 'ostream' printing.  (A precise operational definition
    // of when two instances have the same value can be found in the
    // description of 'operator==' for the class.)  This container is
    // *exception* *neutral* with no guarantee of rollback: if an exception is
    // thrown during the invocation of a method on a pre-existing instance, the
    // class is left in a valid state, but its value is undefined.  In no event
    // is memory leaked.  Finally, *aliasing* (e.g., using all or part of an
    // object as both source and destination) is supported in all cases.

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

    Sha224(const void *data, bsl::size_t length);
        // Construct a SHA-2 digest corresponding to the specified 'data'
        // having the specified 'length' (in bytes).  Note that if 'data' is 0,
        // then 'length' also must be 0.

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
        // previously provided data and has not been subsequently reset, the
        // current state is not the default state and the resultant value is
        // equivalent to applying the SHA-2 algorithm upon the concatenation of
        // all the provided data.  The behavior is undefined unless the range
        // '[data, data + length)' is a valid range.  Note that if 'data' is 0,
        // then 'length' must also be 0.

    void loadDigestAndReset(unsigned char *result);
        // Load the current value of this SHA-2 digest into the specified
        // 'result' and set the value of this SHA-2 digest to the value
        // provided by the default constructor.

    // ACCESSORS
    void loadDigest(unsigned char *result) const;
        // Load the value of this SHA-2 digest into the specified 'result'.

    bsl::ostream& print(bsl::ostream& stream) const;
        // Format the current value of this SHA-2 digest to the specified
        // output 'stream' and return a reference to the modifiable 'stream'.
};

                                 // ============
                                 // class Sha256
                                 // ============

class Sha256 {
    // This 'class' represents a SHA-256 digest that can be updated as
    // additional data is provided.
    //
    // More generally, this class supports a complete set of *value*
    // *semantic* operations, including copy construction, assignment, equality
    // comparison, and 'ostream' printing.  (A precise operational definition
    // of when two instances have the same value can be found in the
    // description of 'operator==' for the class.)  This container is
    // *exception* *neutral* with no guarantee of rollback: if an exception is
    // thrown during the invocation of a method on a pre-existing instance, the
    // class is left in a valid state, but its value is undefined.  In no event
    // is memory leaked.  Finally, *aliasing* (e.g., using all or part of an
    // object as both source and destination) is supported in all cases.

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

    Sha256(const void *data, bsl::size_t length);
        // Construct a SHA-2 digest corresponding to the specified 'data'
        // having the specified 'length' (in bytes).  Note that if 'data' is 0,
        // then 'length' also must be 0.

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
        // previously provided data and has not been subsequently reset, the
        // current state is not the default state and the resultant value is
        // equivalent to applying the SHA-2 algorithm upon the concatenation of
        // all the provided data.  The behavior is undefined unless the range
        // '[data, data + length)' is a valid range.  Note that if 'data' is 0,
        // then 'length' must also be 0.

    void loadDigestAndReset(unsigned char *result);
        // Load the current value of this SHA-2 digest into the specified
        // 'result' and set the value of this SHA-2 digest to the value
        // provided by the default constructor.

    // ACCESSORS
    void loadDigest(unsigned char *result) const;
        // Load the value of this SHA-2 digest into the specified 'result'.

    bsl::ostream& print(bsl::ostream& stream) const;
        // Format the current value of this SHA-2 digest to the specified
        // output 'stream' and return a reference to the modifiable 'stream'.
};

                                 // ============
                                 // class Sha384
                                 // ============

class Sha384 {
    // This 'class' represents a SHA-384 digest that can be updated as
    // additional data is provided.
    //
    // More generally, this class supports a complete set of *value*
    // *semantic* operations, including copy construction, assignment, equality
    // comparison, and 'ostream' printing.  (A precise operational definition
    // of when two instances have the same value can be found in the
    // description of 'operator==' for the class.)  This container is
    // *exception* *neutral* with no guarantee of rollback: if an exception is
    // thrown during the invocation of a method on a pre-existing instance, the
    // class is left in a valid state, but its value is undefined.  In no event
    // is memory leaked.  Finally, *aliasing* (e.g., using all or part of an
    // object as both source and destination) is supported in all cases.

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

    Sha384(const void *data, bsl::size_t length);
        // Construct a SHA-2 digest corresponding to the specified 'data'
        // having the specified 'length' (in bytes).  Note that if 'data' is 0,
        // then 'length' also must be 0.

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
        // previously provided data and has not been subsequently reset, the
        // current state is not the default state and the resultant value is
        // equivalent to applying the SHA-2 algorithm upon the concatenation of
        // all the provided data.  The behavior is undefined unless the range
        // '[data, data + length)' is a valid range.  Note that if 'data' is 0,
        // then 'length' must also be 0.

    void loadDigestAndReset(unsigned char *result);
        // Load the current value of this SHA-2 digest into the specified
        // 'result' and set the value of this SHA-2 digest to the value
        // provided by the default constructor.

    // ACCESSORS
    void loadDigest(unsigned char *result) const;
        // Load the value of this SHA-2 digest into the specified 'result'.

    bsl::ostream& print(bsl::ostream& stream) const;
        // Format the current value of this SHA-2 digest to the specified
        // output 'stream' and return a reference to the modifiable 'stream'.
};

                                 // ============
                                 // class Sha512
                                 // ============

class Sha512 {
    // This 'class' represents a SHA-512 digest that can be updated as
    // additional data is provided.
    //
    // More generally, this class supports a complete set of *value*
    // *semantic* operations, including copy construction, assignment, equality
    // comparison, and 'ostream' printing.  (A precise operational definition
    // of when two instances have the same value can be found in the
    // description of 'operator==' for the class.)  This container is
    // *exception* *neutral* with no guarantee of rollback: if an exception is
    // thrown during the invocation of a method on a pre-existing instance, the
    // class is left in a valid state, but its value is undefined.  In no event
    // is memory leaked.  Finally, *aliasing* (e.g., using all or part of an
    // object as both source and destination) is supported in all cases.

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

    Sha512(const void *data, bsl::size_t length);
        // Construct a SHA-2 digest corresponding to the specified 'data'
        // having the specified 'length' (in bytes).  Note that if 'data' is 0,
        // then 'length' also must be 0.

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
        // previously provided data and has not been subsequently reset, the
        // current state is not the default state and the resultant value is
        // equivalent to applying the SHA-2 algorithm upon the concatenation of
        // all the provided data.  The behavior is undefined unless the range
        // '[data, data + length)' is a valid range.  Note that if 'data' is 0,
        // then 'length' must also be 0.

    void loadDigestAndReset(unsigned char *result);
        // Load the current value of this SHA-2 digest into the specified
        // 'result' and set the value of this SHA-2 digest to the value
        // provided by the default constructor.

    // ACCESSORS
    void loadDigest(unsigned char *result) const;
        // Load the value of this SHA-2 digest into the specified 'result'.

    bsl::ostream& print(bsl::ostream& stream) const;
        // Format the current value of this SHA-2 digest to the specified
        // output 'stream' and return a reference to the modifiable 'stream'.
};

// FREE OPERATORS
bool operator==(const Sha224& lhs, const Sha224& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' SHA digests have the same
    // value, and 'false' otherwise.  Two digests have the same value if, after
    // applying any number of equivalent updates to both (possibly including no
    // updates), the values obtained from their respective 'loadDigest' methods
    // are identical.

inline
bool operator!=(const Sha224& lhs, const Sha224& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' SHA digests do not have
    // the same value, and 'false' otherwise.  Two digests do not have the same
    // value if there exists a set of updates (possibly including the empty
    // set) that, if applied to both, lead to different values being obtained
    // from their respective 'loadDigest' methods.

bool operator==(const Sha256& lhs, const Sha256& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' SHA digests have the same
    // value, and 'false' otherwise.  Two digests have the same value if, after
    // applying any number of equivalent updates to both (possibly including no
    // updates), the values obtained from their respective 'loadDigest' methods
    // are identical.

inline
bool operator!=(const Sha256& lhs, const Sha256& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' SHA digests do not have
    // the same value, and 'false' otherwise.  Two digests do not have the same
    // value if there exists a set of updates (possibly including the empty
    // set) that, if applied to both, lead to different values being obtained
    // from their respective 'loadDigest' methods.

bool operator==(const Sha384& lhs, const Sha384& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' SHA digests have the same
    // value, and 'false' otherwise.  Two digests have the same value if, after
    // applying any number of equivalent updates to both (possibly including no
    // updates), the values obtained from their respective 'loadDigest' methods
    // are identical.

inline
bool operator!=(const Sha384& lhs, const Sha384& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' SHA digests do not have
    // the same value, and 'false' otherwise.  Two digests do not have the same
    // value if there exists a set of updates (possibly including the empty
    // set) that, if applied to both, lead to different values being obtained
    // from their respective 'loadDigest' methods.

bool operator==(const Sha512& lhs, const Sha512& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' SHA digests have the same
    // value, and 'false' otherwise.  Two digests have the same value if, after
    // applying any number of equivalent updates to both (possibly including no
    // updates), the values obtained from their respective 'loadDigest' methods
    // are identical.

inline
bool operator!=(const Sha512& lhs, const Sha512& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' SHA digests do not have
    // the same value, and 'false' otherwise.  Two digests do not have the same
    // value if there exists a set of updates (possibly including the empty
    // set) that, if applied to both, lead to different values being obtained
    // from their respective 'loadDigest' methods.

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

}  // close package namespace

                                 // ------------
                                 // class Sha224
                                 // ------------

// FREE OPERATORS
inline
bool bdlde::operator!=(const Sha224& lhs, const Sha224& rhs)
{
    return !(lhs == rhs);
}

inline
bsl::ostream& bdlde::operator<<(bsl::ostream& stream, const Sha224& digest)
{
    return digest.print(stream);
}

                                 // ------------
                                 // class Sha256
                                 // ------------

// FREE OPERATORS
inline
bool bdlde::operator!=(const Sha256& lhs, const Sha256& rhs)
{
    return !(lhs == rhs);
}

inline
bsl::ostream& bdlde::operator<<(bsl::ostream& stream, const Sha256& digest)
{
    return digest.print(stream);
}

                                 // ------------
                                 // class Sha384
                                 // ------------

inline
bsl::ostream& bdlde::operator<<(bsl::ostream& stream, const Sha384& digest)
{
    return digest.print(stream);
}

// FREE OPERATORS
inline
bool bdlde::operator!=(const Sha384& lhs, const Sha384& rhs)
{
    return !(lhs == rhs);
}

                                 // ------------
                                 // class Sha512
                                 // ------------

// FREE OPERATORS
inline
bool bdlde::operator!=(const Sha512& lhs, const Sha512& rhs)
{
    return !(lhs == rhs);
}

inline
bsl::ostream& bdlde::operator<<(bsl::ostream& stream, const Sha512& digest)
{
    return digest.print(stream);
}

}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2018 Bloomberg Finance L.P.
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
