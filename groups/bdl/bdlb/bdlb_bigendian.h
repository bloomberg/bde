// bdlb_bigendian.h                                                   -*-C++-*-
#ifndef INCLUDED_BDLB_BIGENDIAN
#define INCLUDED_BDLB_BIGENDIAN

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide big-endian integer types.
//
//@CLASSES:
//  bdlb::BigEndianInt16:  signed 16-bit in-core big-endian integer
//  bdlb::BigEndianUint16: unsigned 16-bit in-core big-endian integer
//  bdlb::BigEndianInt32:  signed 32-bit in-core big-endian integer
//  bdlb::BigEndianUint32: unsigned 32-bit in-core big-endian integer
//  bdlb::BigEndianInt64:  signed 64-bit in-core big-endian integer
//  bdlb::BigEndianUint64: unsigned 64-bit in-core big-endian integer
//
//@SEE_ALSO: bsls_types
//
//@DESCRIPTION: This component provides generic in-core big-endian integer
// types.  The integral values that they represent are stored in the objects as
// big-endian values.
//
///Usage
///-----
// 'bdlb::BigEndian' types are very useful to represent structures meant to be
// exchanged over a network, or that are stored as big-endian integers:
//..
//  struct ProtocolHeader {
//      // This structure represents the header of the protocol.  All integer
//      // values are stored in network byte-order (i.e., big-endian).
//
//      bdlb::BigEndianUint16 d_protocolVersion;
//      bdlb::BigEndianUint64 d_messageLength;
//      bdlb::BigEndianInt32  d_source;
//      bdlb::BigEndianInt32  d_destination;
//  };
//..
// Using the above definition, it is trivial to read information from the
// header from any platform:
//..
//  struct ProtocolHeader header;
//  if (sizeof(header) == read(socket, &header, sizeof(header)) {
//      assert(1 == header.d_protocolVersion);
//          // This line will work on any architecture.  'bdlb::BigEndian'
//          // types handle byte swapping if necessary.
//
//      // ...
//  }
//  else {
//      bsl::cerr << "Transmission error!" << bsl::endl;
//  }
//..

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

#ifndef INCLUDED_BDLB_PRINTMETHODS
#include <bdlb_printmethods.h>
#endif

#ifndef INCLUDED_BSLMF_ISTRIVIALLYCOPYABLE
#include <bslmf_istriviallycopyable.h>
#endif

#ifndef INCLUDED_BSLS_BYTEORDER
#include <bsls_byteorder.h>
#endif

#ifndef INCLUDED_BSLS_TYPES
#include <bsls_types.h>
#endif

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif

namespace BloombergLP {
namespace bdlb {
                          // ==========================
                          // class BigEndianInt16
                          // ==========================

class BigEndianInt16 {
    // This class provides a container for an in-core representation of a
    // signed 16-bit big-endian integer.  It supports a complete set of *value*
    // *semantic* operations, including copy construction, assignment, equality
    // comparison, 'bsl::ostream' printing, and 'bdex' serialization.  Note
    // that the copy constructor and copy assignment operator are provided by
    // the compiler.  Any object of this class can be converted to a 'short'
    // allowing comparison with any other object of this class.  This class is
    // *exception* *neutral* with no guarantee of rollback: if an exception is
    // thrown during the invocation of a method on a pre-existing object, the
    // object is left in a valid state, but its value is undefined.  In no
    // event is memory leaked.  Finally, *aliasing* (e.g., using all or part of
    // an object as both source and destination) is supported in all cases.

    // DATA
    short d_value;  // in-core value (network byte-order)

    // FRIENDS
    friend bool operator==(const BigEndianInt16& lhs,
                           const BigEndianInt16& rhs);
    friend bool operator!=(const BigEndianInt16& lhs,
                           const BigEndianInt16& rhs);

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(BigEndianInt16, bsl::is_trivially_copyable);
    BSLMF_NESTED_TRAIT_DECLARATION(BigEndianInt16, bdlb::HasPrintMethod);

    // CLASS METHODS
    static BigEndianInt16 make(short value);
        // Create and initialize a 'BigEndianInt16' object that stores the
        // specified 'value' as a signed 16-bit big-endian integer.


    static int maxSupportedBdexVersion(int versionSelector);
        // Return the maximum valid BDEX format version, as indicated by the
        // specified 'versionSelector', to be passed to the 'bdexStreamOut'
        // method.  Note that the 'versionSelector' is expected to be formatted
        // as 'yyyymmdd', a date representation.  See the 'bslx' package-level
        // documentation for more information on BDEX streaming of
        // value-semantic types and containers.

    // Use the default constructor, copy constructor, destructor, and copy
    // assignment operator that are provided by the compiler.

    // MANIPULATORS
    BigEndianInt16& operator=(short value);
        // Store in this object the specified 'value' as a signed 16-bit
        // big-endian integer, and return a reference to this modifiable
        // object.

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

    // ACCESSORS
    operator short() const;
        // Return the value stored in this object as a 'short' in the native
        // byte-order.

    bsl::ostream& print(bsl::ostream& stream,
                        int           level = 0,
                        int           spacesPerLevel = 4) const;
        // Format this object to the specified output 'stream' at the (absolute
        // value of) the optionally specified indentation 'level' and return a
        // reference to 'stream'.  If 'level' is specified, optionally specify
        // 'spacesPerLevel', the number of spaces per indentation level for
        // this and all of its nested objects.  If 'level' is negative,
        // suppress indentation of the first line.  If 'spacesPerLevel' is
        // negative, format the entire output on one line, suppressing all but
        // the initial indentation (as governed by 'level').  If 'stream' is
        // not valid on entry, this operation has no effect.

    template <class STREAM>
    STREAM& bdexStreamOut(STREAM& stream, int version) const;
        // Write this value to the specified output 'stream' using the
        // specified 'version' format, and return a reference to 'stream'.  If
        // 'stream' is initially invalid, this operation has no effect.  If
        // 'version' is not supported, 'stream' is invalidated but otherwise
        // unmodified.  Note that 'version' is not written to 'stream'.  See
        // the 'bslx' package-level documentation for more information on BDEX
        // streaming of value-semantic types and containers.
};

// FREE OPERATORS
inline
bool operator==(const BigEndianInt16& lhs, const BigEndianInt16& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' 'BigEndianInt16' objects
    // have the same value, and 'false' otherwise.  Two 'BigEndianInt16'
    // objects have the same value if and only if the respective integral
    // network byte-order values that they represent have the same value.

inline
bool operator!=(const BigEndianInt16& lhs, const BigEndianInt16& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' 'BigEndianInt16' objects
    // do not have the same value, and 'false' otherwise.  Two 'BigEndianInt16'
    // objects do not have the same value if and only if the respective
    // integral network byte-order values that they represent do not have the
    // same value.

inline
bsl::ostream& operator<<(bsl::ostream&         stream,
                         const BigEndianInt16& integer);
    // Write the specified 'integer' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.

                          // ===========================
                          // class BigEndianUint16
                          // ===========================

class BigEndianUint16 {
    // This class provides a container for an in-core representation of an
    // unsigned 16-bit big-endian integer.  It supports a complete set of
    // *value* *semantic* operations, including copy construction, assignment,
    // equality comparison, 'bsl::ostream' printing, and 'bdex' serialization.
    // Note that the copy constructor and copy assignment operator are provided
    // by the compiler.  Any object of this class can be converted to an
    // 'unsigned short' allowing comparison with any other object of this
    // class.  This class is *exception* *neutral* with no guarantee of
    // rollback: if an exception is thrown during the invocation of a method on
    // a pre-existing object, the object is left in a valid state, but its
    // value is undefined.  In no event is memory leaked.  Finally, *aliasing*
    // (e.g., using all or part of an object as both source and destination) is
    // supported in all cases.

    // DATA
    unsigned short d_value;  // in-core value (network byte-order)

    // FRIENDS
    friend bool operator==(const BigEndianUint16& lhs,
                           const BigEndianUint16& rhs);
    friend bool operator!=(const BigEndianUint16& lhs,
                           const BigEndianUint16& rhs);

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(BigEndianUint16,
                                   bsl::is_trivially_copyable);
    BSLMF_NESTED_TRAIT_DECLARATION(BigEndianUint16, bdlb::HasPrintMethod);

    // CLASS METHODS
    static BigEndianUint16 make(unsigned short value);
        // Create and initialize a 'BigEndianUint16' object that stores the
        // specified  'value' as a unsigned 16-bit big-endian integer.


    static int maxSupportedBdexVersion(int versionSelector);
        // Return the maximum valid BDEX format version, as indicated by the
        // specified 'versionSelector', to be passed to the 'bdexStreamOut'
        // method.  Note that the 'versionSelector' is expected to be formatted
        // as 'yyyymmdd', a date representation.  See the 'bslx' package-level
        // documentation for more information on BDEX streaming of
        // value-semantic types and containers.

    // Use the default constructor, copy constructor, destructor, and copy
    // assignment operator that are provided by the compiler.

    // MANIPULATORS
    BigEndianUint16& operator=(unsigned short value);
        // Store in this object the specified 'value' as an unsigned 16-bit
        // big-endian integer, and return a reference to this modifiable
        // object.

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

    // ACCESSORS
    operator unsigned short() const;
        // Return the value stored in this object as a 'unsigned short' in the
        // native byte-order.

    bsl::ostream& print(bsl::ostream& stream,
                        int           level = 0,
                        int           spacesPerLevel = 4) const;
        // Format this object to the specified output 'stream' at the (absolute
        // value of) the optionally specified indentation 'level' and return a
        // reference to 'stream'.  If 'level' is specified, optionally specify
        // 'spacesPerLevel', the number of spaces per indentation level for
        // this and all of its nested objects.  If 'level' is negative,
        // suppress indentation of the first line.  If 'spacesPerLevel' is
        // negative, format the entire output on one line, suppressing all but
        // the initial indentation (as governed by 'level').  If 'stream' is
        // not valid on entry, this operation has no effect.

    template <class STREAM>
    STREAM& bdexStreamOut(STREAM& stream, int version) const;
        // Write this value to the specified output 'stream' using the
        // specified 'version' format, and return a reference to 'stream'.  If
        // 'stream' is initially invalid, this operation has no effect.  If
        // 'version' is not supported, 'stream' is invalidated but otherwise
        // unmodified.  Note that 'version' is not written to 'stream'.  See
        // the 'bslx' package-level documentation for more information on BDEX
        // streaming of value-semantic types and containers.
};

// FREE OPERATORS
inline
bool operator==(const BigEndianUint16& lhs, const BigEndianUint16& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' 'BigEndianUint16' objects
    // have the same value, and 'false' otherwise.  Two 'BigEndianUint16'
    // objects have the same value if and only if the respective integral
    // network byte-order values that they represent have the same value.

inline
bool operator!=(const BigEndianUint16& lhs, const BigEndianUint16& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' 'BigEndianUint16' objects
    // do not have the same value, and 'false' otherwise.  Two
    // 'BigEndianUint16' objects do not have the same value if and only if the
    // respective integral network byte-order values that they represent do not
    // have the same value.

inline
bsl::ostream& operator<<(bsl::ostream&          stream,
                         const BigEndianUint16& integer);
    // Write the specified 'integer' to the specified output 'stream', and
    // return a reference to the modifiable 'stream'.

                          // ==========================
                          // class BigEndianInt32
                          // ==========================

class BigEndianInt32 {
    // This class provides a container for an in-core representation of a
    // signed 32-bit big-endian integer.  It supports a complete set of *value*
    // *semantic* operations, including copy construction, assignment, equality
    // comparison, 'bsl::ostream' printing, and 'bdex' serialization.  Note
    // that the copy constructor and copy assignment operator are provided by
    // the compiler.  Any object of this class can be converted to an 'int'
    // allowing comparison with any other object of this class.  This class is
    // *exception* *neutral* with no guarantee of rollback: if an exception is
    // thrown during the invocation of a method on a pre-existing object, the
    // object is left in a valid state, but its value is undefined.  In no
    // event is memory leaked.  Finally, *aliasing* (e.g., using all or part of
    // an object as both source and destination) is supported in all cases.

    // DATA
    int d_value;  // in-core value (network byte-order)

    // FRIENDS
    friend bool operator==(const BigEndianInt32& lhs,
                           const BigEndianInt32& rhs);
    friend bool operator!=(const BigEndianInt32& lhs,
                           const BigEndianInt32& rhs);

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(BigEndianInt32, bsl::is_trivially_copyable);
    BSLMF_NESTED_TRAIT_DECLARATION(BigEndianInt32, bdlb::HasPrintMethod);

    // CLASS METHODS
    static BigEndianInt32 make(int value);
        // Create and initialize a 'BigEndianInt32' object that stores the
        // specified 'value' as a signed 32-bit big-endian integer.


    static int maxSupportedBdexVersion(int versionSelector);
        // Return the maximum valid BDEX format version, as indicated by the
        // specified 'versionSelector', to be passed to the 'bdexStreamOut'
        // method.  Note that the 'versionSelector' is expected to be formatted
        // as 'yyyymmdd', a date representation.  See the 'bslx' package-level
        // documentation for more information on BDEX streaming of
        // value-semantic types and containers.

    // Use the default constructor, copy constructor, destructor, and copy
    // assignment operator that are provided by the compiler.

    // MANIPULATORS
    BigEndianInt32& operator=(int value);
        // Store in this object the specified 'value' as a signed 32-bit
        // big-endian integer, and return a reference to this modifiable
        // object.

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

    // ACCESSORS
    operator int() const;
        // Return the value stored in this object as a 'int' in the native
        // byte-order.

    bsl::ostream& print(bsl::ostream& stream,
                        int           level = 0,
                        int           spacesPerLevel = 4) const;
        // Format this object to the specified output 'stream' at the (absolute
        // value of) the optionally specified indentation 'level' and return a
        // reference to 'stream'.  If 'level' is specified, optionally specify
        // 'spacesPerLevel', the number of spaces per indentation level for
        // this and all of its nested objects.  If 'level' is negative,
        // suppress indentation of the first line.  If 'spacesPerLevel' is
        // negative, format the entire output on one line, suppressing all but
        // the initial indentation (as governed by 'level').  If 'stream' is
        // not valid on entry, this operation has no effect.

    template <class STREAM>
    STREAM& bdexStreamOut(STREAM& stream, int version) const;
        // Write this value to the specified output 'stream' using the
        // specified 'version' format, and return a reference to 'stream'.  If
        // 'stream' is initially invalid, this operation has no effect.  If
        // 'version' is not supported, 'stream' is invalidated but otherwise
        // unmodified.  Note that 'version' is not written to 'stream'.  See
        // the 'bslx' package-level documentation for more information on BDEX
        // streaming of value-semantic types and containers.
};

// FREE OPERATORS
inline
bool operator==(const BigEndianInt32& lhs, const BigEndianInt32& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' 'BigEndianInt32' objects
    // have the same value, and 'false' otherwise.  Two 'BigEndianInt32'
    // objects have the same value if and only if the respective integral
    // network byte-order values that they represent have the same value.

inline
bool operator!=(const BigEndianInt32& lhs, const BigEndianInt32& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' 'BigEndianInt32' objects
    // do not have the same value, and 'false' otherwise.  Two 'BigEndianInt32'
    // objects do not have the same value if and only if the respective
    // integral network byte-order values that they represent do not have the
    // same value.

inline
bsl::ostream& operator<<(bsl::ostream&         stream,
                         const BigEndianInt32& integer);
    // Write the specified 'integer' to the specified output 'stream', and
    // return a reference to the modifiable 'stream'.

                          // ===========================
                          // class BigEndianUint32
                          // ===========================

class BigEndianUint32 {
    // This class provides a container for an in-core representation of an
    // unsigned 32-bit big-endian integer.  It supports a complete set of
    // *value* *semantic* operations, including copy construction, assignment,
    // equality comparison, 'bsl::ostream' printing, and 'bdex' serialization.
    // Note that the copy constructor and copy assignment operator are provided
    // by the compiler.  Any object of this class can be converted to an
    // 'unsigned int' allowing comparison with any other object of this class.
    // This class is *exception* *neutral* with no guarantee of rollback: if an
    // exception is thrown during the invocation of a method on a pre-existing
    // object, the object is left in a valid state, but its value is undefined.
    // In no event is memory leaked.  Finally, *aliasing* (e.g., using all or
    // part of an object as both source and destination) is supported in all
    // cases.

    // DATA
    unsigned int d_value;  // in-core value (network byte-order)

    // FRIENDS
    friend bool operator==(const BigEndianUint32& lhs,
                           const BigEndianUint32& rhs);
    friend bool operator!=(const BigEndianUint32& lhs,
                           const BigEndianUint32& rhs);

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(BigEndianUint32,
                                   bsl::is_trivially_copyable);
    BSLMF_NESTED_TRAIT_DECLARATION(BigEndianUint32, bdlb::HasPrintMethod);

    // CLASS METHODS
    static BigEndianUint32 make(unsigned int value);
        // Create and initialize a 'BigEndianUint32' object that stores the
        // specified 'value' as a unsigned 32-bit big-endian integer.


    static int maxSupportedBdexVersion(int versionSelector);
        // Return the maximum valid BDEX format version, as indicated by the
        // specified 'versionSelector', to be passed to the 'bdexStreamOut'
        // method.  Note that the 'versionSelector' is expected to be formatted
        // as 'yyyymmdd', a date representation.  See the 'bslx' package-level
        // documentation for more information on BDEX streaming of
        // value-semantic types and containers.

    // Use the default constructor, copy constructor, destructor, and copy
    // assignment operator that are provided by the compiler.

    // MANIPULATORS
    BigEndianUint32& operator=(unsigned int value);
        // Store in this object the specified 'value' as an unsigned 32-bit
        // big-endian integer, and return a reference to this modifiable
        // object.

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

    // ACCESSORS
    operator unsigned int() const;
        // Return the value stored in this object as a 'unsigned int' in the
        // native byte-order.

    bsl::ostream& print(bsl::ostream& stream,
                        int           level = 0,
                        int           spacesPerLevel = 4) const;
        // Format this object to the specified output 'stream' at the (absolute
        // value of) the optionally specified indentation 'level' and return a
        // reference to 'stream'.  If 'level' is specified, optionally specify
        // 'spacesPerLevel', the number of spaces per indentation level for
        // this and all of its nested objects.  If 'level' is negative,
        // suppress indentation of the first line.  If 'spacesPerLevel' is
        // negative, format the entire output on one line, suppressing all but
        // the initial indentation (as governed by 'level').  If 'stream' is
        // not valid on entry, this operation has no effect.

    template <class STREAM>
    STREAM& bdexStreamOut(STREAM& stream, int version) const;
        // Write this value to the specified output 'stream' using the
        // specified 'version' format, and return a reference to 'stream'.  If
        // 'stream' is initially invalid, this operation has no effect.  If
        // 'version' is not supported, 'stream' is invalidated but otherwise
        // unmodified.  Note that 'version' is not written to 'stream'.  See
        // the 'bslx' package-level documentation for more information on BDEX
        // streaming of value-semantic types and containers.
};

// FREE OPERATORS
inline
bool operator==(const BigEndianUint32& lhs, const BigEndianUint32& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' 'BigEndianUint32' objects
    // have the same value, and 'false' otherwise.  Two 'BigEndianUint32'
    // objects have the same value if and only if the respective integral
    // network byte-order values that they represent have the same value.

inline
bool operator!=(const BigEndianUint32& lhs, const BigEndianUint32& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' 'BigEndianUint32' objects
    // do not have the same value, and 'false' otherwise.  Two
    // 'BigEndianUint32' objects do not have the same value if and only if the
    // respective integral network byte-order values that they represent do not
    // have the same value.

inline
bsl::ostream& operator<<(bsl::ostream&          stream,
                         const BigEndianUint32& integer);
    // Write the specified 'integer' to the specified output 'stream', and
    // return a reference to the modifiable 'stream'.

                          // ==========================
                          // class BigEndianInt64
                          // ==========================

class BigEndianInt64 {
    // This class provides a container for an in-core representation of a
    // signed 64-bit big-endian integer.  It supports a complete set of *value*
    // *semantic* operations, including copy construction, assignment, equality
    // comparison, 'bsl::ostream' printing, and 'bdex' serialization.  Note
    // that the copy constructor and copy assignment operator are provided by
    // the compiler.  Any object of this class can be converted to an 'Int64'
    // allowing comparison with any other object of this class.  This class is
    // *exception* *neutral* with no guarantee of rollback: if an exception is
    // thrown during the invocation of a method on a pre-existing object, the
    // object is left in a valid state, but its value is undefined.  In no
    // event is memory leaked.  Finally, *aliasing* (e.g., using all or part of
    // an object as both source and destination) is supported in all cases.

    // DATA
    bsls::Types::Int64 d_value;  // in-core value (network byte-order)

    // FRIENDS
    friend bool operator==(const BigEndianInt64& lhs,
                           const BigEndianInt64& rhs);
    friend bool operator!=(const BigEndianInt64& lhs,
                           const BigEndianInt64& rhs);

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(BigEndianInt64, bsl::is_trivially_copyable);
    BSLMF_NESTED_TRAIT_DECLARATION(BigEndianInt64, bdlb::HasPrintMethod);

    // CLASS METHODS
    static BigEndianInt64 make(bsls::Types::Int64 value);
        // Create and initialize a 'BigEndianInt64' object that stores the
        // specified 'value' as a signed 64-bit big-endian integer.


    static int maxSupportedBdexVersion(int versionSelector);
        // Return the maximum valid BDEX format version, as indicated by the
        // specified 'versionSelector', to be passed to the 'bdexStreamOut'
        // method.  Note that the 'versionSelector' is expected to be formatted
        // as 'yyyymmdd', a date representation.  See the 'bslx' package-level
        // documentation for more information on BDEX streaming of
        // value-semantic types and containers.

    // Use the default constructor, copy constructor, destructor, and copy
    // assignment operator that are provided by the compiler.

    // MANIPULATORS
    BigEndianInt64& operator=(bsls::Types::Int64 value);
        // Store in this object the specified 'value' as a signed 64-bit
        // big-endian integer, and return a reference to this modifiable
        // object.

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

    // ACCESSORS
    operator bsls::Types::Int64() const;
        // Return the value stored in this object as a 'bsls::Types::Int64' in
        // the native byte-order.

    bsl::ostream& print(bsl::ostream& stream,
                        int           level = 0,
                        int           spacesPerLevel = 4) const;
        // Format this object to the specified output 'stream' at the (absolute
        // value of) the optionally specified indentation 'level' and return a
        // reference to 'stream'.  If 'level' is specified, optionally specify
        // 'spacesPerLevel', the number of spaces per indentation level for
        // this and all of its nested objects.  If 'level' is negative,
        // suppress indentation of the first line.  If 'spacesPerLevel' is
        // negative, format the entire output on one line, suppressing all but
        // the initial indentation (as governed by 'level').  If 'stream' is
        // not valid on entry, this operation has no effect.

    template <class STREAM>
    STREAM& bdexStreamOut(STREAM& stream, int version) const;
        // Write this value to the specified output 'stream' using the
        // specified 'version' format, and return a reference to 'stream'.  If
        // 'stream' is initially invalid, this operation has no effect.  If
        // 'version' is not supported, 'stream' is invalidated but otherwise
        // unmodified.  Note that 'version' is not written to 'stream'.  See
        // the 'bslx' package-level documentation for more information on BDEX
        // streaming of value-semantic types and containers.
};

// FREE OPERATORS
inline
bool operator==(const BigEndianInt64& lhs, const BigEndianInt64& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' 'BigEndianInt64' objects
    // have the same value, and 'false' otherwise.  Two 'BigEndianInt64'
    // objects have the same value if and only if the respective integral
    // network byte-order values that they represent have the same value.

inline
bool operator!=(const BigEndianInt64& lhs, const BigEndianInt64& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' 'BigEndianInt64' objects
    // do not have the same value, and 'false' otherwise.  Two 'BigEndianInt64'
    // objects do not have the same value if and only if the respective
    // integral network byte-order values that they represent do not have the
    // same value.

inline
bsl::ostream& operator<<(bsl::ostream&         stream,
                         const BigEndianInt64& integer);
    // Write the specified 'integer' to the specified output 'stream', and
    // return a reference to the modifiable 'stream'.

                          // ===========================
                          // class BigEndianUint64
                          // ===========================

class BigEndianUint64 {
    // This class provides a container for an in-core representation of an
    // unsigned 64-bit big-endian integer.  It supports a complete set of
    // *value* *semantic* operations, including copy construction, assignment,
    // equality comparison, 'bsl::ostream' printing, and 'bdex' serialization.
    // Note that the copy constructor and copy assignment operator are provided
    // by the compiler.  Any object of this class can be converted to a
    // 'Uint64' allowing comparison with any other object of this class.  This
    // class is *exception* *neutral* with no guarantee of rollback: if an
    // exception is thrown during the invocation of a method on a pre-existing
    // object, the object is left in a valid state, but its value is undefined.
    // In no event is memory leaked.  Finally, *aliasing* (e.g., using all or
    // part of an object as both source and destination) is supported in all
    // cases.

    // DATA
    bsls::Types::Uint64 d_value;  // in-core value (network byte-order)

    // FRIENDS
    friend bool operator==(const BigEndianUint64& lhs,
                           const BigEndianUint64& rhs);
    friend bool operator!=(const BigEndianUint64& lhs,
                           const BigEndianUint64& rhs);

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(BigEndianUint64,
                                   bsl::is_trivially_copyable);
    BSLMF_NESTED_TRAIT_DECLARATION(BigEndianUint64, bdlb::HasPrintMethod);

    // CLASS METHODS
    static BigEndianUint64 make(bsls::Types::Uint64 value);
        // Create and initialize a 'BigEndianInt64' object that stores the
        // specified 'value' as an unsigned 64-bit big-endian integer.


    static int maxSupportedBdexVersion(int versionSelector);
        // Return the maximum valid BDEX format version, as indicated by the
        // specified 'versionSelector', to be passed to the 'bdexStreamOut'
        // method.  Note that the 'versionSelector' is expected to be formatted
        // as 'yyyymmdd', a date representation.  See the 'bslx' package-level
        // documentation for more information on BDEX streaming of
        // value-semantic types and containers.

    // Use the default constructor, copy constructor, destructor, and copy
    // assignment operator that are provided by the compiler.

    // MANIPULATORS
    BigEndianUint64& operator=(bsls::Types::Uint64 value);
        // Store in this object the specified 'value' as an unsigned 64-bit
        // big-endian integer, and return a reference to this modifiable
        // object.

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

    // ACCESSORS
    operator bsls::Types::Uint64() const;
        // Return the value stored in this object as a 'bsls::Types::Uint64' in
        // the native byte-order.

    bsl::ostream& print(bsl::ostream& stream,
                        int           level = 0,
                        int           spacesPerLevel = 4) const;
        // Format this object to the specified output 'stream' at the (absolute
        // value of) the optionally specified indentation 'level' and return a
        // reference to 'stream'.  If 'level' is specified, optionally specify
        // 'spacesPerLevel', the number of spaces per indentation level for
        // this and all of its nested objects.  If 'level' is negative,
        // suppress indentation of the first line.  If 'spacesPerLevel' is
        // negative, format the entire output on one line, suppressing all but
        // the initial indentation (as governed by 'level').  If 'stream' is
        // not valid on entry, this operation has no effect.

    template <class STREAM>
    STREAM& bdexStreamOut(STREAM& stream, int version) const;
        // Write this value to the specified output 'stream' using the
        // specified 'version' format, and return a reference to 'stream'.  If
        // 'stream' is initially invalid, this operation has no effect.  If
        // 'version' is not supported, 'stream' is invalidated but otherwise
        // unmodified.  Note that 'version' is not written to 'stream'.  See
        // the 'bslx' package-level documentation for more information on BDEX
        // streaming of value-semantic types and containers.
};

// FREE OPERATORS
inline
bool operator==(const BigEndianUint64& lhs, const BigEndianUint64& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' 'BigEndianUint64' objects
    // have the same value, and 'false' otherwise.  Two 'BigEndianUint64'
    // objects have the same value if and only if the respective integral
    // network byte-order values that they represent have the same value.

inline
bool operator!=(const BigEndianUint64& lhs, const BigEndianUint64& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' 'BigEndianUint64' objects
    // do not have the same value, and 'false' otherwise.  Two
    // 'BigEndianUint64' objects do not have the same value if and only if the
    // respective integral network byte-order values that they represent do not
    // have the same value.

inline
bsl::ostream& operator<<(bsl::ostream&          stream,
                         const BigEndianUint64& integer);
    // Write the specified 'integer' to the specified output 'stream', and
    // return a reference to the modifiable 'stream'.

// ============================================================================
//                 INLINE DEFINITIONS
// ============================================================================

inline
BigEndianInt16& BigEndianInt16::operator=(short value)
{
    d_value = BSLS_BYTEORDER_HTONS(value);
    return *this;
}

inline
BigEndianUint16& BigEndianUint16::operator=(unsigned short value)
{
    d_value = BSLS_BYTEORDER_HTONS(value);
    return *this;
}

inline
BigEndianInt32& BigEndianInt32::operator=(int value)
{
    d_value = BSLS_BYTEORDER_HTONL(value);
    return *this;
}

inline
BigEndianUint32& BigEndianUint32::operator=(unsigned int value)
{
    d_value = BSLS_BYTEORDER_HTONL(value);
    return *this;
}

inline
BigEndianInt64& BigEndianInt64::operator=(bsls::Types::Int64 value)
{
    d_value = BSLS_BYTEORDER_HOST_U64_TO_BE(value);
    return *this;
}

inline
BigEndianUint64& BigEndianUint64::operator=(
                                                     bsls::Types::Uint64 value)
{
    d_value = BSLS_BYTEORDER_HOST_U64_TO_BE(value);
    return *this;
}

                         // --------------------------
                         // class BigEndianInt16
                         // --------------------------

// CLASS METHODS
inline
BigEndianInt16 BigEndianInt16::make(short value)
{
    BigEndianInt16 ret;
    return ret = value;
}


inline
int BigEndianInt16::maxSupportedBdexVersion(int /* versionSelector */)
{
    return 1;
}

// MANIPULATORS
template <class STREAM>
STREAM& BigEndianInt16::bdexStreamIn(STREAM& stream, int version)
{
    if (stream) {
        switch (version) {
          case 1: {
            stream.getArrayUint8(reinterpret_cast<unsigned char *>(&d_value),
                                 sizeof d_value);
          } break;
          default: {
            stream.invalidate();
          }
        }
    }
    return stream;
}
}  // close package namespace

// ACCESSORS
inline
bdlb::BigEndianInt16::operator short() const
{
    return BSLS_BYTEORDER_NTOHS(d_value);
}

namespace bdlb {
template <class STREAM>
STREAM& BigEndianInt16::bdexStreamOut(STREAM& stream, int version) const
{
    switch (version) {
      case 1: {
        stream.putArrayUint8(reinterpret_cast<const unsigned char *>(&d_value),
                             sizeof d_value);
      } break;
      default: {
        stream.invalidate();
      }
    }
    return stream;
}
}  // close package namespace

// FREE OPERATORS
inline
bool bdlb::operator==(const BigEndianInt16& lhs, const BigEndianInt16& rhs)
{
    return lhs.d_value == rhs.d_value;
}

inline
bool bdlb::operator!=(const BigEndianInt16& lhs, const BigEndianInt16& rhs)
{
    return lhs.d_value != rhs.d_value;
}

inline
bsl::ostream& bdlb::operator<<(bsl::ostream&         stream,
                               const BigEndianInt16& integer)
{
    integer.print(stream, 0, -1);
    return stream;
}

namespace bdlb {
                         // ---------------------------
                         // class BigEndianUint16
                         // ---------------------------

// CLASS METHODS
inline
BigEndianUint16 BigEndianUint16::make(unsigned short value)
{
    BigEndianUint16 ret;
    return ret = value;
}


inline
int BigEndianUint16::maxSupportedBdexVersion(int /* versionSelector */)
{
    return 1;
}

// MANIPULATORS
template <class STREAM>
STREAM& BigEndianUint16::bdexStreamIn(STREAM& stream, int version)
{
    if (stream) {
        switch (version) {
          case 1: {
            stream.getArrayUint8(reinterpret_cast<unsigned char *>(&d_value),
                                 sizeof d_value);
          } break;
          default: {
            stream.invalidate();
          }
        }
    }
    return stream;
}
}  // close package namespace

// ACCESSORS
inline
bdlb::BigEndianUint16::operator unsigned short() const
{
    return BSLS_BYTEORDER_NTOHS(d_value);
}

namespace bdlb {
template <class STREAM>
STREAM& BigEndianUint16::bdexStreamOut(STREAM& stream, int version) const
{
    switch (version) {
      case 1: {
        stream.putArrayUint8(reinterpret_cast<const unsigned char *>(&d_value),
                             sizeof d_value);
      } break;
      default: {
        stream.invalidate();
      }
    }
    return stream;
}
}  // close package namespace

// FREE OPERATORS
inline
bool bdlb::operator==(const BigEndianUint16& lhs, const BigEndianUint16& rhs)
{
    return lhs.d_value == rhs.d_value;
}

inline
bool bdlb::operator!=(const BigEndianUint16& lhs, const BigEndianUint16& rhs)
{
    return lhs.d_value != rhs.d_value;
}

inline
bsl::ostream& bdlb::operator<<(bsl::ostream&          stream,
                               const BigEndianUint16& integer)
{
    integer.print(stream, 0, -1);
    return stream;
}

namespace bdlb {
                         // --------------------------
                         // class BigEndianInt32
                         // --------------------------

// CLASS METHODS
inline
BigEndianInt32 BigEndianInt32::make(int value)
{
    BigEndianInt32 ret;
    return ret = value;
}


inline
int BigEndianInt32::maxSupportedBdexVersion(int /* versionSelector */)
{
    return 1;
}

// MANIPULATORS
template <class STREAM>
STREAM& BigEndianInt32::bdexStreamIn(STREAM& stream, int version)
{
    switch (version) {
      case 1: {
        stream.getArrayUint8(reinterpret_cast<unsigned char *>(&d_value),
                             sizeof d_value);
      } break;
      default: {
        stream.invalidate();
      }
    }
    return stream;
}
}  // close package namespace

// ACCESSORS
inline
bdlb::BigEndianInt32::operator int() const
{
    return BSLS_BYTEORDER_NTOHL(d_value);
}

namespace bdlb {
template <class STREAM>
STREAM& BigEndianInt32::bdexStreamOut(STREAM& stream, int version) const
{
    switch (version) {
      case 1: {
        stream.putArrayUint8(reinterpret_cast<const unsigned char *>(&d_value),
                             sizeof d_value);
      } break;
      default: {
        stream.invalidate();
      }
    }
    return stream;
}
}  // close package namespace

// FREE OPERATORS
inline
bool bdlb::operator==(const BigEndianInt32& lhs, const BigEndianInt32& rhs)
{
    return lhs.d_value == rhs.d_value;
}

inline
bool bdlb::operator!=(const BigEndianInt32& lhs, const BigEndianInt32& rhs)
{
    return lhs.d_value != rhs.d_value;
}

inline
bsl::ostream& bdlb::operator<<(bsl::ostream&         stream,
                               const BigEndianInt32& integer)
{
    integer.print(stream, 0, -1);
    return stream;
}

namespace bdlb {
                         // ---------------------------
                         // class BigEndianUint32
                         // ---------------------------

// CLASS METHODS
inline
BigEndianUint32 BigEndianUint32::make(unsigned int value)
{
    BigEndianUint32 ret;
    return ret = value;
}


inline
int BigEndianUint32::maxSupportedBdexVersion(int /* versionSelector */)
{
    return 1;
}

// MANIPULATORS
template <class STREAM>
STREAM& BigEndianUint32::bdexStreamIn(STREAM& stream, int version)
{
    if (stream) {
        switch (version) {
          case 1: {
            stream.getArrayUint8(reinterpret_cast<unsigned char *>(&d_value),
                                 sizeof d_value);
          } break;
          default: {
            stream.invalidate();
          }
        }
    }
    return stream;
}
}  // close package namespace

// ACCESSORS
inline
bdlb::BigEndianUint32::operator unsigned int() const
{
    return BSLS_BYTEORDER_NTOHL(d_value);
}

namespace bdlb {
template <class STREAM>
STREAM& BigEndianUint32::bdexStreamOut(STREAM& stream, int version) const
{
    switch (version) {
      case 1: {
        stream.putArrayUint8(reinterpret_cast<const unsigned char *>(&d_value),
                             sizeof d_value);
      } break;
      default: {
        stream.invalidate();
      }
    }
    return stream;
}
}  // close package namespace

// FREE OPERATORS
inline
bool bdlb::operator==(const BigEndianUint32& lhs, const BigEndianUint32& rhs)
{
    return lhs.d_value == rhs.d_value;
}

inline
bool bdlb::operator!=(const BigEndianUint32& lhs, const BigEndianUint32& rhs)
{
    return lhs.d_value != rhs.d_value;
}

inline
bsl::ostream& bdlb::operator<<(bsl::ostream&          stream,
                               const BigEndianUint32& integer)
{
    integer.print(stream, 0, -1);
    return stream;
}

namespace bdlb {
                         // --------------------------
                         // class BigEndianInt64
                         // --------------------------

// CLASS METHODS
inline
BigEndianInt64 BigEndianInt64::make(bsls::Types::Int64 value)
{
    BigEndianInt64 ret;
    return ret = value;
}


inline
int BigEndianInt64::maxSupportedBdexVersion(int /* versionSelector */)
{
    return 1;
}

// MANIPULATORS
template <class STREAM>
STREAM& BigEndianInt64::bdexStreamIn(STREAM& stream, int version)
{
    switch (version) {
      case 1: {
        stream.getArrayUint8(reinterpret_cast<unsigned char *>(&d_value),
                             sizeof d_value);
      } break;
      default: {
        stream.invalidate();
      }
    }
    return stream;
}
}  // close package namespace

// ACCESSORS
inline
bdlb::BigEndianInt64::operator bsls::Types::Int64() const
{
    return BSLS_BYTEORDER_HOST_U64_TO_BE(d_value);
}

namespace bdlb {
template <class STREAM>
STREAM& BigEndianInt64::bdexStreamOut(STREAM& stream, int version) const
{
    switch (version) {
      case 1: {
        stream.putArrayUint8(reinterpret_cast<const unsigned char *>(&d_value),
                             sizeof d_value);
      } break;
      default: {
        stream.invalidate();
      }
    }
    return stream;
}
}  // close package namespace

// FREE OPERATORS
inline
bool bdlb::operator==(const BigEndianInt64& lhs, const BigEndianInt64& rhs)
{
    return lhs.d_value == rhs.d_value;
}

inline
bool bdlb::operator!=(const BigEndianInt64& lhs, const BigEndianInt64& rhs)
{
    return lhs.d_value != rhs.d_value;
}

inline
bsl::ostream& bdlb::operator<<(bsl::ostream&         stream,
                               const BigEndianInt64& integer)
{
    integer.print(stream, 0, -1);
    return stream;
}

namespace bdlb {
                         // ---------------------------
                         // class BigEndianUint64
                         // ---------------------------

// CLASS METHODS
inline
BigEndianUint64 BigEndianUint64::make(bsls::Types::Uint64 value)
{
    BigEndianUint64 ret;
    return ret = value;
}


inline
int BigEndianUint64::maxSupportedBdexVersion(int /* versionSelector */)
{
    return 1;
}

// MANIPULATORS
template <class STREAM>
STREAM& BigEndianUint64::bdexStreamIn(STREAM& stream, int version)
{
    switch (version) {
      case 1: {
        stream.getArrayUint8(reinterpret_cast<unsigned char *>(&d_value),
                             sizeof d_value);
      } break;
      default: {
        stream.invalidate();
      }
    }
    return stream;
}
}  // close package namespace

// ACCESSORS
inline
bdlb::BigEndianUint64::operator bsls::Types::Uint64() const
{
    return BSLS_BYTEORDER_HOST_U64_TO_BE(d_value);
}

namespace bdlb {
template <class STREAM>
STREAM& BigEndianUint64::bdexStreamOut(STREAM& stream, int version) const
{
    switch (version) {
      case 1: {
        stream.putArrayUint8(reinterpret_cast<const unsigned char *>(&d_value),
                             sizeof d_value);
      } break;
      default: {
        stream.invalidate();
      }
    }
    return stream;
}
}  // close package namespace

// FREE OPERATORS
inline
bool bdlb::operator==(const BigEndianUint64& lhs, const BigEndianUint64& rhs)
{
    return lhs.d_value == rhs.d_value;
}

inline
bool bdlb::operator!=(const BigEndianUint64& lhs, const BigEndianUint64& rhs)
{
    return lhs.d_value != rhs.d_value;
}

inline
bsl::ostream& bdlb::operator<<(bsl::ostream&          stream,
                               const BigEndianUint64& integer)
{
    integer.print(stream, 0, -1);
    return stream;
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
