// bdlde_crc32.h                                                      -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BDLDE_CRC32
#define INCLUDED_BDLDE_CRC32

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a mechanism for computing the CRC-32 checksum of a dataset.
//
//@CLASSES:
//  bdlde::Crc32: stores and updates a CRC-32 checksum
//
//@SEE_ALSO:
//
//@DESCRIPTION: This component implements a mechanism for computing, updating,
// and streaming a CRC-32 checksum (a cyclic redundancy check comprised of 32
// bits).  This checksum is a strong and fast technique for determining whether
// or not a message was received without errors.  Note that a CRC-32 checksum
// does not aid in error correction and is not naively useful in any sort of
// cryptographic application.  Compared to other methods such as MD5 and
// SHA-256, it is relatively easy to find alternate texts with identical
// checksum.
//
///Usage
///-----
// The following snippets of code illustrate a typical use of the
// 'bdlde::Crc32' class.  Each function would typically execute in separate
// processes or potentially on separate machines.  The 'senderExample' function
// below demonstrates how a message sender can write a message and its CRC-32
// checksum to a 'bdex' output stream.  Note that 'Out' may be a 'typedef' of
// any class that implements the 'bslx::OutStream' protocol:
//..
//  void senderExample(Out& output)
//      // Write a message and its CRC-32 checksum to the specified 'output'
//      // stream.
//  {
//      // prepare a message
//      bsl::string message = "This is a test message.";
//
//      // generate a checksum for 'message'
//      bdlde::Crc32 crc(message.data(), message.length());
//
//      // write the message to 'output'
//      output << message;
//
//      // write the checksum to 'output'
//      const int VERSION = 1;
//      crc.bdexStreamOut(output, VERSION);
//  }
//..
// The 'receiverExample' function below illustrates how a message receiver can
// read a message and its CRC-32 checksum from a 'bdex' input stream, then
// perform a local CRC-32 computation to verify that the message was received
// intact.  Note that 'In' may be a 'typedef' of any class that implements the
// 'bslx::InStream' protocol:
//..
//  void receiverExample(In& input)
//      // Read a message and its CRC-32 checksum from the specified 'input'
//      // stream, and verify the integrity of the message.
//  {
//      // read the message from 'input'
//      bsl::string message;
//      input >> message;
//
//      // read the checksum from 'input'
//      bdlde::Crc32 crc;
//      const int VERSION = 1;
//      crc.bdexStreamIn(input, VERSION);
//
//      // locally compute the checksum of the received 'message'
//      bdlde::Crc32 crcLocal;
//      crcLocal.update(message.data(), message.length());
//
//      // verify that the received and locally-computed checksums match
//      assert(crcLocal == crc);
//  }
//..

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif


namespace BloombergLP {
namespace bdlde {

                                // ===========
                                // class Crc32
                                // ===========

class Crc32 {
    // This class represents a CRC-32 checksum value that can be updated as
    // data is provided.
    //
    // More generally, this class supports a complete set of *value*
    // *semantic* operations, including copy construction, assignment,
    // equality comparison, 'ostream' printing, and 'bdex' serialization.
    // (A precise operational definition of when two objects have the same
    // value can be found in the description of 'operator==' for the class.)
    // This class is *exception* *neutral* with no guarantee of rollback:
    // if an exception is thrown during the invocation of a method on a
    // pre-existing object, the class is left in a valid state, but its value
    // is undefined.  In no event is memory leaked.  Finally, *aliasing* (e.g.,
    // using all or part of an object as both source and destination) is
    // supported in all cases.

    // DATA
    unsigned int d_crc;  // value of the checksum ^ 0xffffffff

    // FRIENDS
    friend bool operator==(const Crc32&, const Crc32&);

  public:
    // CLASS METHODS
    static int maxSupportedBdexVersion(int versionSelector);
        // Return the maximum valid BDEX format version, as indicated by the
        // specified 'versionSelector', to be passed to the 'bdexStreamOut'
        // method.  Note that the 'versionSelector' is expected to be formatted
        // as 'yyyymmdd', a date representation.  See the 'bslx' package-level
        // documentation for more information on BDEX streaming of
        // value-semantic types and containers.

    // CREATORS
    Crc32();
        // Construct a checksum having the value corresponding to no data
        // having been provided (i.e., having the value 0).

    Crc32(const void *data, int length);
        // Construct a checksum corresponding to the specified 'data' having
        // the specified 'length' (in bytes).  The behavior is undefined unless
        // '0 <= length'.  Note that if 'data' is 0, then 'length' also must
        // be 0.

    Crc32(const Crc32& original);
        // Construct a checksum having the value of the specified 'original'
        // checksum.

    // ~Crc32();
        // Destroy this checksum.  Note that this trivial destructor is
        // generated by the compiler.

    // MANIPULATORS
    Crc32& operator=(const Crc32& rhs);
        // Assign to this checksum the value of the specified 'rhs' checksum,
        // and return a reference to this modifiable checksum.

    template <class STREAM>
    STREAM& bdexStreamIn(STREAM& stream, int version);
        // Assign to this object the value read from the specified input
        // 'stream' using the specified 'version' format, and return a
        // reference to 'stream'.  If 'stream' is initially invalid, this
        // operation has no effect.  If 'version' is not supported, this object
        // is unaltered and 'stream' is invalidated but otherwise unmodified.
        // If 'version' is supported but 'stream' becomes invalid during this
        // operation, this object has an undefined, but valid, state.  Note
        // that no version is read from 'stream'.  See the 'bslx' package-level
        // documentation for more information on BDEX streaming of
        // value-semantic types and containers.

    unsigned int checksumAndReset();
        // Return the current value of this checksum and set the value of this
        // checksum to the value the default constructor provides.

    void reset();
        // Reset the value of this checksum to the value the default
        // constructor provides.

    void update(const void *data, int length);
        // Update the value of this checksum to incorporate the specified
        // 'data' having the specified 'length'.  If the current state is the
        // default state, the resultant value of this checksum is the
        // application of the CRC-32 algorithm upon the currently given 'data'
        // of the given 'length'.  If this checksum has been previously
        // provided data and has not been subsequently reset, the current state
        // is not the default state and the resultant value is equivalent to
        // applying the CRC-32 algorithm upon the concatenation of all the
        // provided data.  The behavior is undefined unless '0 <= length'.
        // Note that if 'data' is 0, then 'length' also must be 0.

    // ACCESSORS
    template <class STREAM>
    STREAM& bdexStreamOut(STREAM& stream, int version) const;
        // Write this value to the specified output 'stream' using the
        // specified 'version' format, and return a reference to 'stream'.
        // If 'stream' is initially invalid, this operation has no effect.
        // If 'version' is not supported, 'stream' is invalidated but
        // otherwise unmodified.  Note that 'version' is not written to
        // 'stream'.  See the 'bslx' package-level documentation for more
        // information on BDEX streaming of value-semantic types and
        // containers.

    unsigned int checksum() const;
        // Return the current value of this checksum.

    bsl::ostream& print(bsl::ostream& stream) const;
        // Format this object to the specified output 'stream' at the (absolute
        // value of) the optionally specified indentation 'level' and return a
        // reference to 'stream'.  If 'level' is specified, optionally specify
        // 'spacesPerLevel', the number of spaces per indentation level for
        // this and all of its nested objects.  If 'level' is negative,
        // suppress indentation of the first line.  If 'spacesPerLevel' is
        // negative, format the entire output on one line, suppressing all but
        // the initial indentation (as governed by 'level').  If 'stream' is
        // not valid on entry, this operation has no effect.


};

// FREE OPERATORS
bool operator==(const Crc32& lhs, const Crc32& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' checksums have the same
    // value, and 'false' otherwise.  Two checksums have the same value if the
    // values obtained from their 'checksum' methods are identical.

bool operator!=(const Crc32& lhs, const Crc32& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' checksums do not have the
    // same value, and 'false' otherwise.  Two checksums do not have the same
    // value if the values obtained from their 'checksum' methods differ.

bsl::ostream& operator<<(bsl::ostream& stream, const Crc32& checksum);
    // Write to the specified output 'stream' the specified 'checksum' value
    // and return a reference to the modifiable 'stream'.

// ============================================================================
//                        INLINE FUNCTION DEFINITIONS
// ============================================================================

                                // -----------
                                // class Crc32
                                // -----------

// CLASS METHODS
inline
int Crc32::maxSupportedBdexVersion(int)
{
    return 1;
}

// CREATORS
inline
Crc32::Crc32()
: d_crc(0xffffffff)
{
}

inline
Crc32::Crc32(const void *data, int length)
: d_crc(0xffffffff)
{
    update(data, length);
}

inline
Crc32::Crc32(const Crc32& original)
: d_crc(original.d_crc)
{
}

// MANIPULATORS
inline
Crc32& Crc32::operator=(const Crc32& rhs)
{
    d_crc = rhs.d_crc;
    return *this;
}

template <class STREAM>
STREAM& Crc32::bdexStreamIn(STREAM& stream, int version)
{
    if (stream) {
        switch (version) {
          case 1: {
            unsigned int crc;
            stream.getUint32(crc);
            if (!stream) {
                return stream;                                        // RETURN
            }
            d_crc = crc;
          } break;
          default: {
            stream.invalidate();
          } break;
        }
    }
    return stream;
}

inline
unsigned int Crc32::checksumAndReset()
{
    const unsigned int crc = d_crc;
    d_crc = 0xffffffff;
    return crc ^ 0xffffffff;
}

inline
void Crc32::reset()
{
    d_crc = 0xffffffff;
}

// ACCESSORS
template <class STREAM>
STREAM& Crc32::bdexStreamOut(STREAM& stream, int version) const
{
    switch (version) {
      case 1: {
        stream.putUint32(d_crc);
      } break;
      default: {
        stream.invalidate();
      } break;
    }
    return stream;
}

inline
unsigned int Crc32::checksum() const
{
    return d_crc ^ 0xffffffff;
}


// FREE OPERATORS
inline
bool operator==(const Crc32& lhs, const Crc32& rhs)
{
    return lhs.d_crc == rhs.d_crc;
}

inline
bool operator!=(const Crc32& lhs, const Crc32& rhs)
{
    return !(lhs == rhs);
}

inline
bsl::ostream& operator<<(bsl::ostream& stream, const Crc32& checksum)
{
    return checksum.print(stream);
}

}  // close package namespace
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
