// bdede_crc32.h                                                      -*-C++-*-
#ifndef INCLUDED_BDEDE_CRC32
#define INCLUDED_BDEDE_CRC32

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a mechanism for computing the CRC-32 checksum of a dataset.
//
//@CLASSES:
//  bdede_Crc32: stores and updates a CRC-32 checksum
//
//@AUTHOR: Jeffrey Mendelsohn (jmendelsohn)
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
// The following snippets of code illustrate a typical use of the 'bdede_Crc32'
// class.  Each function would typically execute in separate processes or
// potentially on separate machines.  The 'senderExample' function below
// demonstrates how a message sender can write a message and its CRC-32
// checksum to a 'bdex' output stream.  Note that 'Out' may be a 'typedef'
// of any class that implements the 'bdex_OutStream' protocol:
//..
//  void senderExample(Out& output)
//      // Write a message and its CRC-32 checksum to the specified 'output'
//      // stream.
//  {
//      // prepare a message
//      bsl::string message = "This is a test message.";
//
//      // generate a checksum for 'message'
//      bdede_Crc32 crc(message.data(), message.length());
//
//      // write the message to 'output'
//      bdex_OutStreamFunctions::streamOut(output, message, 0);
//
//      // write the checksum to 'output'
//      const int VERSION = 1;
//      bdex_OutStreamFunctions::streamOut(output, crc, VERSION);
//  }
//..
// The 'receiverExample' function below illustrates how a message receiver can
// read a message and its CRC-32 checksum from a 'bdex' input stream, then
// perform a local CRC-32 computation to verify that the message was received
// intact.  Note that 'In' may be a 'typedef' of any class that implements the
// 'bdex_InStream' protocol:
//..
//  void receiverExample(In& input)
//      // Read a message and its CRC-32 checksum from the specified 'input'
//      // stream, and verify the integrity of the message.
//  {
//      // read the message from 'input'
//      bsl::string message;
//      bdex_InStreamFunctions::streamIn(input, message, 0);
//
//      // read the checksum from 'input'
//      bdede_Crc32 crc;
//      const int VERSION = 1;
//      bdex_InStreamFunctions::streamIn(input, crc, VERSION);
//
//      // locally compute the checksum of the received 'message'
//      bdede_Crc32 crcLocal;
//      crcLocal.update(message.data(), message.length());
//
//      // verify that the received and locally-computed checksums match
//      assert(crcLocal == crc);
//  }
//..

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif


namespace BloombergLP {

                            // =================
                            // class bdede_Crc32
                            // =================

class bdede_Crc32 {
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
    friend bool operator==(const bdede_Crc32& lhs, const bdede_Crc32& rhs);

  public:
    // CLASS METHODS
    static int maxSupportedBdexVersion();
        // Return the most current 'bdex' streaming version number supported by
        // this class.  (See the package-group-level documentation for more
        // information on 'bdex' streaming of container types.)

    // CREATORS
    bdede_Crc32();
        // Construct a checksum having the value corresponding to no data
        // having been provided (i.e., having the value 0).

    bdede_Crc32(const void *data, int length);
        // Construct a checksum corresponding to the specified 'data' having
        // the specified 'length' (in bytes).  The behavior is undefined unless
        // '0 <= length'.  Note that if 'data' is 0, then 'length' also must
        // be 0.

    bdede_Crc32(const bdede_Crc32& original);
        // Construct a checksum having the value of the specified 'original'
        // checksum.

    // ~bdede_Crc32();
        // Destroy this checksum.  Note that this trivial destructor is
        // generated by the compiler.

    // MANIPULATORS
    bdede_Crc32& operator=(const bdede_Crc32& rhs);
        // Assign to this checksum the value of the specified 'rhs' checksum,
        // and return a reference to this modifiable checksum.

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
        // Write this value to the specified output 'stream' and return a
        // reference to the modifiable 'stream'.  Optionally specify an
        // explicit 'version' format; by default, the maximum supported version
        // is written to 'stream' and used as the format.  If 'version' is
        // specified, that format is used, but *not* written to 'stream'.  If
        // 'version' is not supported, 'stream' is left unmodified.  (See the
        // package-group-level documentation for more information on 'bdex'
        // streaming of container types).

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

#ifndef BDE_OMIT_INTERNAL_DEPRECATED
    unsigned int view() const;
        // Return the current value of this checksum.
        //
        // DEPRECATED: use method 'checksum' instead.
#endif // BDE_OMIT_INTERNAL_DEPRECATED

};

// FREE OPERATORS
bool operator==(const bdede_Crc32& lhs, const bdede_Crc32& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' checksums have the same
    // value, and 'false' otherwise.  Two checksums have the same value if the
    // values obtained from their 'checksum' methods are identical.

bool operator!=(const bdede_Crc32& lhs, const bdede_Crc32& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' checksums do not have the
    // same value, and 'false' otherwise.  Two checksums do not have the same
    // value if the values obtained from their 'checksum' methods differ.

bsl::ostream& operator<<(bsl::ostream& stream, const bdede_Crc32& checksum);
    // Write to the specified output 'stream' the specified 'checksum' value
    // and return a reference to the modifiable 'stream'.

// ===========================================================================
//                        INLINE FUNCTION DEFINITIONS
// ===========================================================================

                            // -----------------
                            // class bdede_Crc32
                            // -----------------

// CLASS METHODS
inline
int bdede_Crc32::maxSupportedBdexVersion()
{
    return 1;
}

// CREATORS
inline
bdede_Crc32::bdede_Crc32()
: d_crc(0xffffffff)
{
}

inline
bdede_Crc32::bdede_Crc32(const void *data, int length)
: d_crc(0xffffffff)
{
    update(data, length);
}

inline
bdede_Crc32::bdede_Crc32(const bdede_Crc32& original)
: d_crc(original.d_crc)
{
}

// MANIPULATORS
inline
bdede_Crc32& bdede_Crc32::operator=(const bdede_Crc32& rhs)
{
    d_crc = rhs.d_crc;
    return *this;
}

template <class STREAM>
STREAM& bdede_Crc32::bdexStreamIn(STREAM& stream, int version)
{
    if (stream) {
        switch (version) {
          case 1: {
            unsigned int crc;
            stream.getUint32(crc);
            if (!stream) {
                return stream;
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
unsigned int bdede_Crc32::checksumAndReset()
{
    const unsigned int crc = d_crc;
    d_crc = 0xffffffff;
    return crc ^ 0xffffffff;
}

inline
void bdede_Crc32::reset()
{
    d_crc = 0xffffffff;
}

// ACCESSORS
template <class STREAM>
STREAM& bdede_Crc32::bdexStreamOut(STREAM& stream, int version) const
{
    switch (version) {
      case 1: {
        stream.putUint32(d_crc);
      } break;
    }
    return stream;
}

inline
unsigned int bdede_Crc32::checksum() const
{
    return d_crc ^ 0xffffffff;
}

#ifndef BDE_OMIT_INTERNAL_DEPRECATED
inline
unsigned int bdede_Crc32::view() const
{
    return d_crc ^ 0xffffffff;
}
#endif // BDE_OMIT_INTERNAL_DEPRECATED

// FREE OPERATORS
inline
bool operator==(const bdede_Crc32& lhs, const bdede_Crc32& rhs)
{
    return lhs.d_crc == rhs.d_crc;
}

inline
bool operator!=(const bdede_Crc32& lhs, const bdede_Crc32& rhs)
{
    return !(lhs == rhs);
}

inline
bsl::ostream& operator<<(bsl::ostream& stream, const bdede_Crc32& checksum)
{
    return checksum.print(stream);
}

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2004
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
