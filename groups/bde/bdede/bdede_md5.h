// bdede_md5.h                                                        -*-C++-*-
#ifndef INCLUDED_BDEDE_MD5
#define INCLUDED_BDEDE_MD5

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a value-semantic type encoding a message in an MD5 digest.
//
//@CLASSES:
//  bdede_Md5: value-semantic type representing an MD5 digest
//
//@AUTHOR: Jeffrey Mendelsohn (jmendelsohn), Arthur Chiu (achiu21)
//
//@SEE_ALSO:
//
//@DESCRIPTION: This component implements a mechanism for computing, updating,
// and streaming an MD5 digest (a cryptographic hash comprised of 128 bits).
// This digest is a strong technique for determining whether or not a message
// was received without errors.  This implementation is based on the RFC 1321
// specification which can be found at:
//..
//  http://www.ietf.org/rfc/rfc1321.txt
//..
// Note that an MD5 digest does not aid in error correction.
//
///Performance
///-----------
// The performance of this component is slightly slower than the native
// 'openssl' implementation of MD5.  It is typically within 7% of the speed of
// the native 'openssl' implementation with an error margin of +/-2%, depending
// on machine load.  A million iterations of the 'update' method will typically
// take between 0.84s to 0.90s on Sun and 0.68s to 0.69s on IBM.  A million
// iterations of the equivalent function in 'openssl', 'MD5_Update', will
// typically take between 0.80s to 0.85s on Sun and 0.65s to 0.67s on IBM.
//
///Usage
///-----
// The following snippets of code illustrate a typical use of the 'bdede_Md5'
// class.  Each function would typically execute in separate processes or
// potentially on separate machines.  The 'senderExample' function below
// demonstrates how a message sender can write a message and its MD5 digest to
// a 'bdex' output stream.  Note that 'Out' may be a 'typedef' of any class
// that implements the 'bdex_OutStream' protocol:
//..
//  void senderExample(Out& output)
//      // Write a message and its MD5 digest to the specified 'output' stream.
//  {
//      // Prepare a message.
//      bsl::string message = "This is a test message.";
//
//      // Generate a digest for 'message'.
//      bdede_Md5 digest(message.data(), message.length());
//
//      // Write the message to 'output'.
//      bdex_OutStreamFunctions::streamOut(output, message, 0);
//
//      // Write the digest to 'output'.
//      const int VERSION = 1;
//      bdex_OutStreamFunctions::streamOut(output, digest, VERSION);
//  }
//..
// The 'receiverExample' function below illustrates how a message receiver can
// read a message and its MD5 digest from a 'bdex' input stream, then perform a
// local MD5 computation to verify that the message was received intact.  Note
// that 'In' may be a 'typedef' of any class that implements the
// 'bdex_InStream' protocol:
//..
//  void receiverExample(In& input)
//      // Read a message and its MD5 digest from the specified 'input' stream,
//      // and verify the integrity of the message.
//  {
//      // Read the message from 'input'.
//      bsl::string message;
//      bdex_InStreamFunctions::streamIn(input, message, 0);
//
//      // Read the digest from 'input'.
//      bdede_Md5 digest;
//      const int VERSION = 1;
//      bdex_InStreamFunctions::streamIn(input, digest, VERSION);
//
//      // Locally compute the digest of the received 'message'.
//      bdede_Md5 digestLocal;
//      digestLocal.update(message.data(), message.length());
//
//      // Verify that the received and locally-computed digests match.
//      assert(digestLocal == digest);
//  }
//..
//
///Additional Copyright Notice
///---------------------------
// The implementation of this component is *substantially* derived from the RSA
// Data Security, Inc. MD5 Message-Digest Algorithm that was published in the
// aforementioned RFC 1321.

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BSLS_ALIGNEDBUFFER
#include <bsls_alignedbuffer.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORMUTIL
#include <bsls_platformutil.h>  // @DEPRECATED
#endif

#ifndef INCLUDED_BSLS_TYPES
#include <bsls_types.h>
#endif

#ifndef INCLUDED_BSL_CSTRING
#include <bsl_cstring.h>
#endif

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif

namespace BloombergLP {

                            // ===============
                            // class bdede_Md5
                            // ===============

class bdede_Md5 {
    // This 'class' represents an MD5 digest that can be updated as additional
    // data is provided.
    //
    // More generally, this class supports a complete set of *value*
    // *semantic* operations, including copy construction, assignment,
    // equality comparison, 'ostream' printing, and 'bdex' serialization.
    // (A precise operational definition of when two instances have the same
    // value can be found in the description of 'operator==' for the class.)
    // This container is *exception* *neutral* with no guarantee of rollback:
    // if an exception is thrown during the invocation of a method on a
    // pre-existing instance, the class is left in a valid state, but its
    // value is undefined.  In no event is memory leaked.  Finally, *aliasing*
    // (e.g., using all or part of an object as both source and destination)
    // is supported in all cases.

    // DATA
    unsigned int      d_state[4];    // state array storing the digest

    bsls_Types::Int64 d_length;      // length of the message

    unsigned char     d_buffer[64];  // buffer for storing remaining part of
                                     // message that is not yet incorporated
                                     // into 'd_state'

    // FRIENDS
    friend bool operator==(const bdede_Md5&, const bdede_Md5&);

  public:
    // TYPES
    typedef bsls_AlignedBuffer<16> Md5Digest;
        // A maximally-aligned, 16-byte object type used to represent an MD5
        // digest.

    // CLASS METHODS
    static int maxSupportedBdexVersion();
        // Return the most current 'bdex' streaming version number supported by
        // this class.  (See the package-group-level documentation for more
        // information on 'bdex' streaming of container types.)

    // CREATORS
    bdede_Md5();
        // Construct an MD5 digest having the value corresponding to no data
        // having been provided.

    bdede_Md5(const void *data, int length);
        // Construct an MD5 digest corresponding to the specified 'data' having
        // the specified 'length' (in bytes).  The behavior is undefined unless
        // '0 <= length'.  Note that if 'data' is 0, then 'length' also must
        // be 0.

    bdede_Md5(const bdede_Md5& original);
        // Construct an MD5 digest having the value of the specified 'original'
        // digest.

    ~bdede_Md5();
        // Destroy this MD5 digest.

    // MANIPULATORS
    // !bdede_Md5& operator=(const bdede_Md5& rhs);
        // Assign to this MD5 digest the value of the specified 'rhs' MD5
        // digest and return a reference to this modifiable MD5 digest.  Note
        // that this method's definition is compiler generated.

    template <class STREAM>
    STREAM& bdexStreamIn(STREAM& stream, int version);
        // Assign to this object the value read from the specified input
        // 'stream' using the specified 'version' format and return a reference
        // to the modifiable 'stream'.  If 'stream' is initially invalid, this
        // operation has no effect.  If 'stream' becomes invalid during this
        // operation, this object is valid, but its value is undefined.  If the
        // specified 'version' is not supported, 'stream' is marked invalid,
        // but this object is unaltered.  Note that no version is read from
        // 'stream'.  (See the package-group-level documentation for more
        // information on 'bdex' streaming of container types.)

    void reset();
        // Reset the value of this MD5 digest to the value provided by the
        // default constructor.

    void update(const void *data, int length);
        // Update the value of this MD5 digest to incorporate the specified
        // 'data' having the specified 'length' in bytes.  If the current state
        // is the default state, the resultant value of this MD5 digest is the
        // application of the MD5 algorithm upon the currently given 'data' of
        // the given 'length'.  If this digest has been previously provided
        // data and has not been subsequently reset, the current state is not
        // the default state and the resultant value is equivalent to applying
        // the MD5 algorithm upon the concatenation of all the provided data.
        // The behavior is undefined unless '0 <= length'.  Note that if 'data'
        // is 0, then 'length' also must be 0.

    void loadDigestAndReset(Md5Digest *result);
        // Load the current value of this MD5 digest into the specified
        // 'result' and set the value of this MD5 digest to the value provided
        // by the default constructor.

    // ACCESSORS
    template <class STREAM>
    STREAM& bdexStreamOut(STREAM& stream, int version) const;
        // Write this value to the specified output 'stream' and return a
        // reference to the modifiable 'stream'.  Optionally specify an
        // explicit 'version' format; by default, the maximum supported version
        // is written to 'stream' and used as the format.  If 'version' is
        // specified, that format is used, but *not* written to 'stream'.  If
        // 'version' is not supported, 'stream' is left unmodified.  (See the
        // package-group-level documentation for more information on 'bdex'
        // streaming of container types).

    bsl::ostream& print(bsl::ostream& stream) const;
        // Format the current value of this MD5 digest to the specified output
        // 'stream' and return a reference to the modifiable 'stream'.

    void loadDigest(Md5Digest *result) const;
        // Load the current value of this MD5 digest into the specified
        // 'result'.
};

// FREE OPERATORS
bool operator==(const bdede_Md5& lhs, const bdede_Md5& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' MD5 digests have the same
    // value, and 'false' otherwise.  Two digests have the same value if the
    // values obtained from their respective 'loadDigest' methods are
    // identical.

inline
bool operator!=(const bdede_Md5& lhs, const bdede_Md5& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' MD5 digests do not have
    // the same value, and 'false' otherwise.  Two digests do not have the same
    // value if the values obtained from their respective 'loadDigest' methods
    // differ.

inline
bsl::ostream& operator<<(bsl::ostream& stream, const bdede_Md5& digest);
    // Write to the specified output 'stream' the specified MD5 'digest' and
    // return a reference to the modifiable 'stream'.

// ===========================================================================
//                        INLINE FUNCTION DEFINITIONS
// ===========================================================================

                            // ---------------
                            // class bdede_Md5
                            // ---------------

// CLASS METHODS
inline
int bdede_Md5::maxSupportedBdexVersion()
{
    return 1;
}

// MANIPULATORS
template <class STREAM>
STREAM& bdede_Md5::bdexStreamIn(STREAM& stream, int version)
{
    switch (version) {
      case 1: {
        unsigned int      state[4];
        unsigned char     buf[64];
        bsls_Types::Int64 length;

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
            return stream;
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
STREAM& bdede_Md5::bdexStreamOut(STREAM& stream, int version) const
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

// FREE OPERATORS
inline
bool operator!=(const bdede_Md5& lhs, const bdede_Md5& rhs)
{
    return !(lhs == rhs);
}

inline
bsl::ostream& operator<<(bsl::ostream& stream, const bdede_Md5& digest)
{
    return digest.print(stream);
}

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2008
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
