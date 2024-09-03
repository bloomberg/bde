// bdlde_md5.h                                                        -*-C++-*-
#ifndef INCLUDED_BDLDE_MD5
#define INCLUDED_BDLDE_MD5

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a value-semantic type encoding a message in an MD5 digest.
//
//@CLASSES:
//  bdlde::Md5: value-semantic type representing an MD5 digest
//
//@SEE_ALSO: bdlde_crc32
//
//@DESCRIPTION: This component implements a mechanism for computing, updating,
// and streaming an MD5 digest (a hash comprising 128 bits).  One possible
// application is determining whether or not a message was received without
// errors, although due to security vulnerabilities, it should no longer be
// used for this purpose (see the Security section below).  This implementation
// is based on the RFC 1321 specification which can be found at:
// ```
// http://www.ietf.org/rfc/rfc1321.txt
// ```
// Note that an MD5 digest does not aid in error correction.
//
///Security
///--------
// Practical collision and chosen-prefix collision attacks are known against
// MD5.  Do not use MD5 to generate digital signatures under any circumstances,
// and do not use MD5 at all except when it is required for interoperation with
// legacy systems that use MD5.  SHA-2 (available in the `bdlde_sha2`
// component) and SHA-3 are more secure alternatives to MD5.
//
// You might think that your application doesn't require collision resistance.
// However, (1) you might be mistaken, (2) once you start using MD5, you
// prevent future versions of your application from being able to rely on
// collision resistance unless they break backward compatibility, (3) a
// maintainer of your application might accidentally make a change that
// implicitly assumes collision resistance, and (4) if you expose MD5 hashes to
// your users, they might assume that they are secure digital signatures, which
// will make their applications insecure.  In light of the foregoing
// considerations, and the availability of SHA-2 and SHA-3 as alternatives,
// there is no justification for using MD5 unless you absolutely have to.
//
///Performance
///-----------
// The performance of this component is slightly slower than the native
// `openssl` implementation of MD5.  It is typically within 7% of the speed of
// the native `openssl` implementation with an error margin of +/-2%, depending
// on machine load.  A million iterations of the `update` method will typically
// take between 0.84s to 0.90s on Sun and 0.68s to 0.69s on IBM.  A million
// iterations of the equivalent function in `openssl`, `MD5_Update`, will
// typically take between 0.80s to 0.85s on Sun and 0.65s to 0.67s on IBM.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Basic Usage
/// - - - - - - - - - - -
// The following snippets of code illustrate a typical use of the `bdlde::Md5`
// class.  Each function would typically execute in separate processes or
// potentially on separate machines.  The `senderExample` function below
// demonstrates how a message sender can write a message and its MD5 digest
// to a `bdex` output stream.  Note that `Out` may be a `typedef` of any class
// that implements the `bslx::OutStream` protocol:
// ```
// /// Write a message and its MD5 digest to the specified `output`
// /// stream.
// void senderExample(Out& output)
// {
//     // Prepare a message.
//     bsl::string message = "This is a test message.";
//
//     // Generate a digest for `message`.
//     bdlde::Md5 digest(message.data(), static_cast<int>(message.length()));
//
//     // Write the message to `output`.
//     output << message;
//
//     // Write the digest to `output`.
//     const int VERSION = 1;
//     digest.bdexStreamOut(output, VERSION);
// }
// ```
// The `receiverExample` function below illustrates how a message receiver can
// read a message and its MD5 digest from a `bdex` input stream, then perform a
// local MD5 computation to verify that the message was received intact.  Note
// that `In` may be a `typedef` of any class that implements the
// `bslx::InStream` protocol:
// ```
// /// Read a message and its MD5 digest from the specified `input` stream,
// /// and verify the integrity of the message.
// void receiverExample(In& input)
// {
//     // Read the message from `input`.
//     bsl::string message;
//     input >> message;
//
//     // Read the digest from `input`.
//     bdlde::Md5 digest;
//     const int VERSION = 1;
//     digest.bdexStreamIn(input, VERSION);
//
//     // Locally compute the digest of the received `message`.
//     bdlde::Md5 digestLocal;
//     digestLocal.update(message.data(), static_cast<int>(message.length()));
//
//     // Verify that the received and locally-computed digests match.
//     assert(digestLocal == digest);
// }
// ```
// Due to security vulnerabilities in the MD5 algorithm (see the Security
// section above), the use of MD5 contemplated above is insecure unless the
// transmission channel is *completely trusted*, which is often impossible to
// guarantee in practice.  Therefore, MD5 should no longer be used in this way.
//
///Additional Copyright Notice
///---------------------------
// The implementation of this component is *substantially* derived from the RSA
// Data Security, Inc. MD5 Message-Digest Algorithm that was published in the
// aforementioned RFC 1321.

#include <bdlscm_version.h>

#include <bsls_alignedbuffer.h>
#include <bsls_compilerfeatures.h>
#include <bsls_types.h>

#include <bsl_cstring.h>
#include <bsl_iosfwd.h>

namespace BloombergLP {
namespace bdlde {

                                 // =========
                                 // class Md5
                                 // =========

/// This `class` represents an MD5 digest that can be updated as additional
/// data is provided.
///
/// More generally, this class supports a complete set of *value*
/// *semantic* operations, including copy construction, assignment,
/// equality comparison, `ostream` printing, and `bdex` serialization.
/// (A precise operational definition of when two instances have the same
/// value can be found in the description of `operator==` for the class.)
/// This container is *exception* *neutral* with no guarantee of rollback:
/// if an exception is thrown during the invocation of a method on a
/// pre-existing instance, the class is left in a valid state, but its
/// value is undefined.  In no event is memory leaked.  Finally, *aliasing*
/// (e.g., using all or part of an object as both source and destination)
/// is supported in all cases.
class Md5 {

    // DATA
    unsigned int       d_state[4];    // state array storing the digest

    bsls::Types::Int64 d_length;      // length of the message

    unsigned char      d_buffer[64];  // buffer for storing remaining part of
                                      // message that is not yet incorporated
                                      // into `d_state`

    // FRIENDS
    friend bool operator==(const Md5&, const Md5&);

  public:
    // TYPES

    /// A maximally-aligned, 16-byte object type used to represent an MD5
    /// digest.
    typedef bsls::AlignedBuffer<16> Md5Digest;

    // CLASS METHODS

    /// Return the maximum valid BDEX format version, as indicated by the
    /// specified `versionSelector`, to be passed to the `bdexStreamOut`
    /// method.  Note that the `versionSelector` is expected to be formatted
    /// as `yyyymmdd`, a date representation.  See the `bslx` package-level
    /// documentation for more information on BDEX streaming of
    /// value-semantic types and containers.
    static int maxSupportedBdexVersion(int);

    // CREATORS

    /// Construct an MD5 digest having the value corresponding to no data
    /// having been provided.
    Md5();

    /// Construct an MD5 digest corresponding to the specified `data` having
    /// the specified `length` (in bytes).  The behavior is undefined unless
    /// `0 <= length`.  Note that if `data` is 0, then `length` also must
    /// be 0.
    Md5(const void *data, int length);

    /// Construct an MD5 digest having the value of the specified `original`
    /// digest.
    Md5(const Md5& original);

    /// Destroy this MD5 digest.
    ~Md5();

    // MANIPULATORS

#if defined(BSLS_COMPILERFEATURES_SUPPORT_DEFAULTED_FUNCTIONS)
    /// Assign to this MD5 digest the value of the specified `rhs` MD5
    /// digest and return a reference to this modifiable MD5 digest.
    Md5& operator=(const Md5& rhs) = default;
#endif

    /// Assign to this object the value read from the specified input
    /// `stream` using the specified `version` format and return a reference
    /// to the modifiable `stream`.  If `stream` is initially invalid, this
    /// operation has no effect.  If `stream` becomes invalid during this
    /// operation, this object is valid, but its value is undefined.  If the
    /// specified `version` is not supported, `stream` is marked invalid,
    /// but this object is unaltered.  Note that no version is read from
    /// `stream`.  (See the package-group-level documentation for more
    /// information on `bdex` streaming of container types.)
    template <class STREAM>
    STREAM& bdexStreamIn(STREAM& stream, int version);

    /// Reset the value of this MD5 digest to the value provided by the
    /// default constructor.
    void reset();

    /// Update the value of this MD5 digest to incorporate the specified
    /// `data` having the specified `length` in bytes.  If the current state
    /// is the default state, the resultant value of this MD5 digest is the
    /// application of the MD5 algorithm upon the currently given `data` of
    /// the given `length`.  If this digest has been previously provided
    /// data and has not been subsequently reset, the current state is not
    /// the default state and the resultant value is equivalent to applying
    /// the MD5 algorithm upon the concatenation of all the provided data.
    /// The behavior is undefined unless `0 <= length`.  Note that if `data`
    /// is 0, then `length` also must be 0.
    void update(const void *data, int length);

    /// Load the current value of this MD5 digest into the specified
    /// `result` and set the value of this MD5 digest to the value provided
    /// by the default constructor.
    void loadDigestAndReset(Md5Digest *result);

    // ACCESSORS

    /// Write this value to the specified output `stream` and return a
    /// reference to the modifiable `stream`.  Optionally specify an
    /// explicit `version` format; by default, the maximum supported version
    /// is written to `stream` and used as the format.  If `version` is
    /// specified, that format is used, but *not* written to `stream`.  If
    /// `version` is not supported, `stream` is left unmodified.  (See the
    /// package-group-level documentation for more information on `bdex`
    /// streaming of container types).
    template <class STREAM>
    STREAM& bdexStreamOut(STREAM& stream, int version) const;

    /// Format the current value of this MD5 digest to the specified output
    /// `stream` and return a reference to the modifiable `stream`.
    bsl::ostream& print(bsl::ostream& stream) const;

    /// Load the current value of this MD5 digest into the specified
    /// `result`.
    void loadDigest(Md5Digest *result) const;

#ifndef BDE_OMIT_DEPRECATED
    // CLASS METHODS

    /// Return the most current `bdex` streaming version number supported by
    /// this class.  (See the package-group-level documentation for more
    /// information on `bdex` streaming of container types.)
    static int maxSupportedBdexVersion();
#endif
};

// FREE OPERATORS

/// Return `true` if the specified `lhs` and `rhs` MD5 digests have the same
/// value, and `false` otherwise.  Two digests have the same value if the
/// values obtained from their respective `loadDigest` methods are identical.
bool operator==(const Md5& lhs, const Md5& rhs);

/// Return `true` if the specified `lhs` and `rhs` MD5 digests do not have
/// the same value, and `false` otherwise.  Two digests do not have the same
/// value if the values obtained from their respective `loadDigest` methods
/// differ.
inline
bool operator!=(const Md5& lhs, const Md5& rhs);

/// Write to the specified output `stream` the specified MD5 `digest` and
/// return a reference to the modifiable `stream`.
inline
bsl::ostream& operator<<(bsl::ostream& stream, const Md5& digest);

// ============================================================================
//                        INLINE FUNCTION DEFINITIONS
// ============================================================================

                                 // ---------
                                 // class Md5
                                 // ---------

// CLASS METHODS
inline
int Md5::maxSupportedBdexVersion(int)
{
    return 1;
}

// MANIPULATORS
template <class STREAM>
STREAM& Md5::bdexStreamIn(STREAM& stream, int version)
{
    switch (version) {
      case 1: {
        unsigned int       state[4];
        unsigned char      buf[64];
        bsls::Types::Int64 length;

        // first the state

        for (int i = 0; i < 4; i++) {
            stream.getUint32(state[i]);
        }

        // then the length

        stream.getInt64(length);

        // finally the buffer

        for (int i = 0; i < 64; i++) {
            stream.getUint8(buf[i]);
        }

        if (!stream) {
            return stream;                                            // RETURN
        }

        d_length = length;
        bsl::memcpy(d_state, state, sizeof state);
        bsl::memcpy(d_buffer, buf, sizeof buf);

      } break;
      default: {
        stream.invalidate();
      } break;
    }
    return stream;
}

// ACCESSORS
template <class STREAM>
STREAM& Md5::bdexStreamOut(STREAM& stream, int version) const
{
    if (stream) {
        switch (version) {
          case 1: {
            // first the state

            for (int i = 0; i < 4; ++i) {
                stream.putUint32(d_state[i]);
            }

            // then the length

            stream.putInt64(d_length);

            // finally the buffer

            for (int i = 0; i < 64; ++i) {
                stream.putUint8(d_buffer[i]);
            }

          } break;
          default: {
            stream.invalidate();
          } break;
        }
    }
    return stream;
}

#ifndef BDE_OMIT_DEPRECATED
inline
int Md5::maxSupportedBdexVersion()
{
    return maxSupportedBdexVersion(0);
}
#endif

}  // close package namespace

// FREE OPERATORS
inline
bool bdlde::operator!=(const Md5& lhs, const Md5& rhs)
{
    return !(lhs == rhs);
}

inline
bsl::ostream& bdlde::operator<<(bsl::ostream& stream, const Md5& digest)
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
