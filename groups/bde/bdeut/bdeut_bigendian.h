// bdeut_bigendian.h                                                  -*-C++-*-
#ifndef INCLUDED_BDEUT_BIGENDIAN
#define INCLUDED_BDEUT_BIGENDIAN

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide big-endian integer types.
//
//@CLASSES:
//  bdeut_BigEndianInt16:  signed 16-bit in-core big-endian integer
//  bdeut_BigEndianUint16: unsigned 16-bit in-core big-endian integer
//  bdeut_BigEndianInt32:  signed 32-bit in-core big-endian integer
//  bdeut_BigEndianUint32: unsigned 32-bit in-core big-endian integer
//  bdeut_BigEndianInt64:  signed 64-bit in-core big-endian integer
//  bdeut_BigEndianUint64: unsigned 64-bit in-core big-endian integer
//
//@SEE_ALSO: bsls_types
//
//@AUTHOR: Guillaume Morin (gmorin1)
//
//@DESCRIPTION: This component provides generic in-core big-endian integer
// types.  The integral values that they represent are stored in the objects as
// big-endian values.
//
///Usage
///-----
// 'bdeut_BigEndian' types are very useful to represent structures meant to be
// exchanged over a network, or that are stored as big-endian integers:
//..
//  struct ProtocolHeader {
//      // This structure represents the header of the protocol.  All integer
//      // values are stored in network byte-order (i.e., big-endian).
//
//      bdeut_BigEndianUint16 d_protocolVersion;
//      bdeut_BigEndianUint64 d_messageLength;
//      bdeut_BigEndianInt32  d_source;
//      bdeut_BigEndianInt32  d_destination;
//  };
//..
// Using the above definition, it is trivial to read information from the
// header from any platform:
//..
//  struct ProtocolHeader header;
//  if (sizeof(header) == read(socket, &header, sizeof(header)) {
//      assert(1 == header.d_protocolVersion);
//          // This line will work on any architecture.  'bdeut_BigEndian'
//          // types handle byte swapping if necessary.
//
//      // ...
//  }
//  else {
//      bsl::cerr << "Transmission error!" << bsl::endl;
//  }
//..

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BDEU_PRINTMETHODS
#include <bdeu_printmethods.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITS
#include <bslalg_typetraits.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITBITWISECOPYABLE
#include <bslalg_typetraitbitwisecopyable.h>
#endif

#ifndef INCLUDED_BSLS_BYTEORDER
#include <bsls_byteorder.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORM
#include <bsls_platform.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORMUTIL
#include <bsls_platformutil.h>    // @DEPRECATED
#endif

#ifndef INCLUDED_BSLS_TYPES
#include <bsls_types.h>
#endif

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif

namespace BloombergLP {

                          // ==========================
                          // class bdeut_BigEndianInt16
                          // ==========================

class bdeut_BigEndianInt16 {
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
    friend bool operator==(const bdeut_BigEndianInt16& lhs,
                           const bdeut_BigEndianInt16& rhs);
    friend bool operator!=(const bdeut_BigEndianInt16& lhs,
                           const bdeut_BigEndianInt16& rhs);

  public:
    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS2(bdeut_BigEndianInt16,
                                  bslalg_TypeTraitBitwiseCopyable,
                                  bdeu_TypeTraitHasPrintMethod);

    // CLASS METHODS
    static int maxSupportedBdexVersion();
        // Return the most current 'bdex' streaming version number supported by
        // this class.  See the 'bdex' package-level documentation for more
        // information on 'bdex' streaming of value-semantic types and
        // containers.

    static bdeut_BigEndianInt16 make(short value);
        // Create and initialize a 'bdeut_BigEndianInt16' object which stores
        // 'value' as a signed 16-bit big-endian integer.

    // Use the default constructor, copy constructor, destructor, and copy
    // assignment operator that are provided by the compiler.

    // MANIPULATORS
    bdeut_BigEndianInt16& operator=(short value);
        // Store in this object the specified 'value' as a signed 16-bit
        // big-endian integer, and return a reference to this modifiable
        // object.

    template <class STREAM>
    STREAM& bdexStreamIn(STREAM& stream, int version);
        // Assign to this object the value read from the specified input
        // 'stream' using the specified 'version' format and return a reference
        // to the modifiable 'stream'.  If 'stream' is initially invalid, this
        // operation has no effect.  If 'stream' becomes invalid during this
        // operation this object is unaltered.  If 'version' is not supported,
        // 'stream' is invalidated and this object remains unaltered.  Note
        // that no version is read from 'stream'.  See the 'bdex' package-level
        // documentation for more information on 'bdex' streaming of
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
        // specified 'version' format and return a reference to the
        // modifiable 'stream'.  If 'version' is not supported, 'stream' is
        // invalidated.  Note that 'version' is not written to 'stream'.  See
        // the 'bdex' package-level documentation for more information on
        // 'bdex' streaming of value-semantic types and containers.
};

// FREE OPERATORS
inline
bool operator==(const bdeut_BigEndianInt16& lhs,
                const bdeut_BigEndianInt16& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' 'bdeut_BigEndianInt16'
    // objects have the same value, and 'false' otherwise.  Two
    // 'bdeut_BigEndianInt16' objects have the same value if and only if the
    // respective integral network byte-order values that they represent have
    // the same value.

inline
bool operator!=(const bdeut_BigEndianInt16& lhs,
                const bdeut_BigEndianInt16& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' 'bdeut_BigEndianInt16'
    // objects do not have the same value, and 'false' otherwise.  Two
    // 'bdeut_BigEndianInt16' objects do not have the same value if and only if
    // the respective integral network byte-order values that they represent do
    // not have the same value.

inline
bsl::ostream& operator<<(bsl::ostream&               stream,
                         const bdeut_BigEndianInt16& integer);
    // Write the specified 'integer' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.

                          // ===========================
                          // class bdeut_BigEndianUint16
                          // ===========================

class bdeut_BigEndianUint16 {
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
    friend bool operator==(const bdeut_BigEndianUint16& lhs,
                           const bdeut_BigEndianUint16& rhs);
    friend bool operator!=(const bdeut_BigEndianUint16& lhs,
                           const bdeut_BigEndianUint16& rhs);

  public:
    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS2(bdeut_BigEndianUint16,
                                  bslalg_TypeTraitBitwiseCopyable,
                                  bdeu_TypeTraitHasPrintMethod);

    // CLASS METHODS
    static int maxSupportedBdexVersion();
        // Return the most current 'bdex' streaming version number supported by
        // this class.  See the 'bdex' package-level documentation for more
        // information on 'bdex' streaming of value-semantic types and
        // containers.

    static bdeut_BigEndianUint16 make(unsigned short value);
        // Create and initialize a 'bdeut_BigEndianUint16' object which stores
        // 'value' as a unsigned 16-bit big-endian integer.

    // Use the default constructor, copy constructor, destructor, and copy
    // assignment operator that are provided by the compiler.

    // MANIPULATORS
    bdeut_BigEndianUint16& operator=(unsigned short value);
        // Store in this object the specified 'value' as an unsigned 16-bit
        // big-endian integer, and return a reference to this modifiable
        // object.

    template <class STREAM>
    STREAM& bdexStreamIn(STREAM& stream, int version);
        // Assign to this object the value read from the specified input
        // 'stream' using the specified 'version' format and return a reference
        // to the modifiable 'stream'.  If 'stream' is initially invalid, this
        // operation has no effect.  If 'stream' becomes invalid during this
        // operation this object is unaltered.  If 'version' is not supported,
        // 'stream' is invalidated and this object remains unaltered.  Note
        // that no version is read from 'stream'.  See the 'bdex' package-level
        // documentation for more information on 'bdex' streaming of
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
        // specified 'version' format and return a reference to the
        // modifiable 'stream'.  If 'version' is not supported, 'stream' is
        // invalidated.  Note that 'version' is not written to 'stream'.  See
        // the 'bdex' package-level documentation for more information on
        // 'bdex' streaming of value-semantic types and containers.
};

// FREE OPERATORS
inline
bool operator==(const bdeut_BigEndianUint16& lhs,
                const bdeut_BigEndianUint16& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' 'bdeut_BigEndianUint16'
    // objects have the same value, and 'false' otherwise.  Two
    // 'bdeut_BigEndianUint16' objects have the same value if and only if the
    // respective integral network byte-order values that they represent have
    // the same value.

inline
bool operator!=(const bdeut_BigEndianUint16& lhs,
                const bdeut_BigEndianUint16& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' 'bdeut_BigEndianUint16'
    // objects do not have the same value, and 'false' otherwise.  Two
    // 'bdeut_BigEndianUint16' objects do not have the same value if and only
    // if the respective integral network byte-order values that they represent
    // do not have the same value.

inline
bsl::ostream& operator<<(bsl::ostream&                stream,
                         const bdeut_BigEndianUint16& integer);
    // Write the specified 'integer' to the specified output 'stream', and
    // return a reference to the modifiable 'stream'.

                          // ==========================
                          // class bdeut_BigEndianInt32
                          // ==========================

class bdeut_BigEndianInt32 {
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
    friend bool operator==(const bdeut_BigEndianInt32& lhs,
                           const bdeut_BigEndianInt32& rhs);
    friend bool operator!=(const bdeut_BigEndianInt32& lhs,
                           const bdeut_BigEndianInt32& rhs);

  public:
    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS2(bdeut_BigEndianInt32,
                                  bslalg_TypeTraitBitwiseCopyable,
                                  bdeu_TypeTraitHasPrintMethod);

    // CLASS METHODS
    static int maxSupportedBdexVersion();
        // Return the most current 'bdex' streaming version number supported by
        // this class.  See the 'bdex' package-level documentation for more
        // information on 'bdex' streaming of value-semantic types and
        // containers.

    static bdeut_BigEndianInt32 make(int value);
        // Create and initialize a 'bdeut_BigEndianInt32' object which stores
        // 'value' as a signed 32-bit big-endian integer.

    // Use the default constructor, copy constructor, destructor, and copy
    // assignment operator that are provided by the compiler.

    // MANIPULATORS
    bdeut_BigEndianInt32& operator=(int value);
        // Store in this object the specified 'value' as a signed 32-bit
        // big-endian integer, and return a reference to this modifiable
        // object.

    template <class STREAM>
    STREAM& bdexStreamIn(STREAM& stream, int version);
        // Assign to this object the value read from the specified input
        // 'stream' using the specified 'version' format and return a reference
        // to the modifiable 'stream'.  If 'stream' is initially invalid, this
        // operation has no effect.  If 'stream' becomes invalid during this
        // operation this object is unaltered.  If 'version' is not supported,
        // 'stream' is invalidated and this object remains unaltered.  Note
        // that no version is read from 'stream'.  See the 'bdex' package-level
        // documentation for more information on 'bdex' streaming of
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
        // specified 'version' format and return a reference to the
        // modifiable 'stream'.  If 'version' is not supported, 'stream' is
        // invalidated.  Note that 'version' is not written to 'stream'.  See
        // the 'bdex' package-level documentation for more information on
        // 'bdex' streaming of value-semantic types and containers.
};

// FREE OPERATORS
inline
bool operator==(const bdeut_BigEndianInt32& lhs,
                const bdeut_BigEndianInt32& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' 'bdeut_BigEndianInt32'
    // objects have the same value, and 'false' otherwise.  Two
    // 'bdeut_BigEndianInt32' objects have the same value if and only if the
    // respective integral network byte-order values that they represent have
    // the same value.

inline
bool operator!=(const bdeut_BigEndianInt32& lhs,
                const bdeut_BigEndianInt32& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' 'bdeut_BigEndianInt32'
    // objects do not have the same value, and 'false' otherwise.  Two
    // 'bdeut_BigEndianInt32' objects do not have the same value if and only if
    // the respective integral network byte-order values that they represent do
    // not have the same value.

inline
bsl::ostream& operator<<(bsl::ostream&               stream,
                         const bdeut_BigEndianInt32& integer);
    // Write the specified 'integer' to the specified output 'stream', and
    // return a reference to the modifiable 'stream'.

                          // ===========================
                          // class bdeut_BigEndianUint32
                          // ===========================

class bdeut_BigEndianUint32 {
    // This class provides a container for an in-core representation of an
    // unsigned 32-bit big-endian integer.  It supports a complete set of
    // *value* *semantic* operations, including copy construction, assignment,
    // equality comparison, 'bsl::ostream' printing, and 'bdex' serialization.
    // Note that the copy constructor and copy assignment operator are provided
    // by the compiler.  Any object of this class can be converted to an
    // 'unsigned int' allowing comparison with any other object of this class.
    // This class is *exception* *neutral* with no guarantee of rollback: if an
    // exception is thrown during the invocation of a method on a pre-existing
    // object, the object is left in a valid state, but its value is
    // undefined.  In no event is memory leaked.  Finally, *aliasing* (e.g.,
    // using all or part of an object as both source and destination) is
    // supported in all cases.

    // DATA
    unsigned int d_value;  // in-core value (network byte-order)

    // FRIENDS
    friend bool operator==(const bdeut_BigEndianUint32& lhs,
                           const bdeut_BigEndianUint32& rhs);
    friend bool operator!=(const bdeut_BigEndianUint32& lhs,
                           const bdeut_BigEndianUint32& rhs);

  public:
    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS2(bdeut_BigEndianUint32,
                                  bslalg_TypeTraitBitwiseCopyable,
                                  bdeu_TypeTraitHasPrintMethod);

    // CLASS METHODS
    static int maxSupportedBdexVersion();
        // Return the most current 'bdex' streaming version number supported by
        // this class.  See the 'bdex' package-level documentation for more
        // information on 'bdex' streaming of value-semantic types and
        // containers.

    static bdeut_BigEndianUint32 make(unsigned int value);
        // Create and initialize a 'bdeut_BigEndianUint32' object which stores
        // 'value' as a unsigned 32-bit big-endian integer.

    // Use the default constructor, copy constructor, destructor, and copy
    // assignment operator that are provided by the compiler.

    // MANIPULATORS
    bdeut_BigEndianUint32& operator=(unsigned int value);
        // Store in this object the specified 'value' as an unsigned 32-bit
        // big-endian integer, and return a reference to this modifiable
        // object.

    template <class STREAM>
    STREAM& bdexStreamIn(STREAM& stream, int version);
        // Assign to this object the value read from the specified input
        // 'stream' using the specified 'version' format and return a reference
        // to the modifiable 'stream'.  If 'stream' is initially invalid, this
        // operation has no effect.  If 'stream' becomes invalid during this
        // operation this object is unaltered.  If 'version' is not supported,
        // 'stream' is invalidated and this object remains unaltered.  Note
        // that no version is read from 'stream'.  See the 'bdex' package-level
        // documentation for more information on 'bdex' streaming of
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
        // specified 'version' format and return a reference to the
        // modifiable 'stream'.  If 'version' is not supported, 'stream' is
        // invalidated.  Note that 'version' is not written to 'stream'.  See
        // the 'bdex' package-level documentation for more information on
        // 'bdex' streaming of value-semantic types and containers.
};

// FREE OPERATORS
inline
bool operator==(const bdeut_BigEndianUint32& lhs,
                const bdeut_BigEndianUint32& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' 'bdeut_BigEndianUint32'
    // objects have the same value, and 'false' otherwise.  Two
    // 'bdeut_BigEndianUint32' objects have the same value if and only if the
    // respective integral network byte-order values that they represent have
    // the same value.

inline
bool operator!=(const bdeut_BigEndianUint32& lhs,
                const bdeut_BigEndianUint32& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' 'bdeut_BigEndianUint32'
    // objects do not have the same value, and 'false' otherwise.  Two
    // 'bdeut_BigEndianUint32' objects do not have the same value if and only
    // if the respective integral network byte-order values that they represent
    // do not have the same value.

inline
bsl::ostream& operator<<(bsl::ostream&                stream,
                         const bdeut_BigEndianUint32& integer);
    // Write the specified 'integer' to the specified output 'stream', and
    // return a reference to the modifiable 'stream'.

                          // ==========================
                          // class bdeut_BigEndianInt64
                          // ==========================

class bdeut_BigEndianInt64 {
    // This class provides a container for an in-core representation of a
    // signed 64-bit big-endian integer.  It supports a complete set of
    // *value* *semantic* operations, including copy construction,
    // assignment, equality comparison, 'bsl::ostream' printing, and 'bdex'
    // serialization.  Note that the copy constructor and copy assignment
    // operator are provided by the compiler.  Any object of this class
    // can be converted to an 'Int64' allowing comparison with any other
    // object of this class.  This class is *exception* *neutral* with no
    // guarantee of rollback: if an exception is thrown during the
    // invocation of a method on a pre-existing object, the object is left
    // in a valid state, but its value is undefined.  In no event is memory
    // leaked.  Finally, *aliasing* (e.g., using all or part of an object as
    // both source and destination) is supported in all cases.

    // DATA
    bsls_Types::Int64 d_value;  // in-core value (network byte-order)

    // FRIENDS
    friend bool operator==(const bdeut_BigEndianInt64& lhs,
                           const bdeut_BigEndianInt64& rhs);
    friend bool operator!=(const bdeut_BigEndianInt64& lhs,
                           const bdeut_BigEndianInt64& rhs);

  public:
    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS2(bdeut_BigEndianInt64,
                                  bslalg_TypeTraitBitwiseCopyable,
                                  bdeu_TypeTraitHasPrintMethod);

    // CLASS METHODS
    static int maxSupportedBdexVersion();
        // Return the most current 'bdex' streaming version number supported
        // by this class.  See the 'bdex' package-level documentation for
        // more information on 'bdex' streaming of value-semantic types and
        // containers.

    static bdeut_BigEndianInt64 make(bsls_Types::Int64 value);
        // Create and initialize a 'bdeut_BigEndianInt64' object which stores
        // 'value' as a signed 64-bit big-endian integer.

    // Use the default constructor, copy constructor, destructor, and copy
    // assignment operator that are provided by the compiler.

    // MANIPULATORS
    bdeut_BigEndianInt64& operator=(bsls_Types::Int64 value);
       // Store in this object the specified 'value' as a signed 64-bit
       // big-endian integer, and return a reference to this modifiable
       // object.

    template <class STREAM>
    STREAM& bdexStreamIn(STREAM& stream, int version);
        // Assign to this object the value read from the specified input
        // 'stream' using the specified 'version' format and return a reference
        // to the modifiable 'stream'.  If 'stream' is initially invalid, this
        // operation has no effect.  If 'stream' becomes invalid during this
        // operation this object is unaltered.  If 'version' is not supported,
        // 'stream' is invalidated and this object remains unaltered.  Note
        // that no version is read from 'stream'.  See the 'bdex' package-level
        // documentation for more information on 'bdex' streaming of
        // value-semantic types and containers.

    // ACCESSORS
    operator bsls_Types::Int64() const;
        // Return the value stored in this object as a 'bsls_Types::Int64' in
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
        // specified 'version' format and return a reference to the
        // modifiable 'stream'.  If 'version' is not supported, 'stream' is
        // invalidated.  Note that 'version' is not written to 'stream'.  See
        // the 'bdex' package-level documentation for more information on
        // 'bdex' streaming of value-semantic types and containers.
};

// FREE OPERATORS
inline
bool operator==(const bdeut_BigEndianInt64& lhs,
                const bdeut_BigEndianInt64& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' 'bdeut_BigEndianInt64'
    // objects have the same value, and 'false' otherwise.  Two
    // 'bdeut_BigEndianInt64' objects have the same value if and only if the
    // respective integral network byte-order values that they represent have
    // the same value.

inline
bool operator!=(const bdeut_BigEndianInt64& lhs,
                const bdeut_BigEndianInt64& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' 'bdeut_BigEndianInt64'
    // objects do not have the same value, and 'false' otherwise.  Two
    // 'bdeut_BigEndianInt64' objects do not have the same value if and only if
    // the respective integral network byte-order values that they represent do
    // not have the same value.

inline
bsl::ostream& operator<<(bsl::ostream&               stream,
                         const bdeut_BigEndianInt64& integer);
    // Write the specified 'integer' to the specified output 'stream', and
    // return a reference to the modifiable 'stream'.

                          // ===========================
                          // class bdeut_BigEndianUint64
                          // ===========================

class bdeut_BigEndianUint64 {
    // This class provides a container for an in-core representation of an
    // unsigned 64-bit big-endian integer.  It supports a complete set of
    // *value* *semantic* operations, including copy construction, assignment,
    // equality comparison, 'bsl::ostream' printing, and 'bdex' serialization.
    // Note that the copy constructor and copy assignment operator are provided
    // by the compiler.  Any object of this class can be converted to a
    // 'Uint64' allowing comparison with any other object of this class.  This
    // class is *exception* *neutral* with no guarantee of rollback: if an
    // exception is thrown during the invocation of a method on a pre-existing
    // object, the object is left in a valid state, but its value is
    // undefined.  In no event is memory leaked.  Finally, *aliasing* (e.g.,
    // using all or part of an object as both source and destination) is
    // supported in all cases.

    // DATA
    bsls_Types::Uint64 d_value;  // in-core value (network byte-order)

    // FRIENDS
    friend bool operator==(const bdeut_BigEndianUint64& lhs,
                           const bdeut_BigEndianUint64& rhs);
    friend bool operator!=(const bdeut_BigEndianUint64& lhs,
                           const bdeut_BigEndianUint64& rhs);

  public:
    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS2(bdeut_BigEndianUint64,
                                  bslalg_TypeTraitBitwiseCopyable,
                                  bdeu_TypeTraitHasPrintMethod);

    // CLASS METHODS
    static int maxSupportedBdexVersion();
        // Return the most current 'bdex' streaming version number supported by
        // this class.  See the 'bdex' package-level documentation for more
        // information on 'bdex' streaming of value-semantic types and
        // containers.

    static bdeut_BigEndianUint64 make(bsls_Types::Uint64 value);
        // Create and initialize a 'bdeut_BigEndianInt64' object which stores
        // 'value' as an unsigned 64-bit big-endian integer.

    // Use the default constructor, copy constructor, destructor, and copy
    // assignment operator that are provided by the compiler.

    // MANIPULATORS
    bdeut_BigEndianUint64& operator=(bsls_Types::Uint64 value);
        // Store in this object the specified 'value' as an unsigned 64-bit
        // big-endian integer, and return a reference to this modifiable
        // object.

    template <class STREAM>
    STREAM& bdexStreamIn(STREAM& stream, int version);
        // Assign to this object the value read from the specified input
        // 'stream' using the specified 'version' format and return a reference
        // to the modifiable 'stream'.  If 'stream' is initially invalid, this
        // operation has no effect.  If 'stream' becomes invalid during this
        // operation this object is unaltered.  If 'version' is not supported,
        // 'stream' is invalidated and this object remains unaltered.  Note
        // that no version is read from 'stream'.  See the 'bdex' package-level
        // documentation for more information on 'bdex' streaming of
        // value-semantic types and containers.

    // ACCESSORS
    operator bsls_Types::Uint64() const;
        // Return the value stored in this object as a 'bsls_Types::Uint64' in
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
        // specified 'version' format and return a reference to the
        // modifiable 'stream'.  If 'version' is not supported, 'stream' is
        // invalidated.  Note that 'version' is not written to 'stream'.  See
        // the 'bdex' package-level documentation for more information on
        // 'bdex' streaming of value-semantic types and containers.
};

// FREE OPERATORS
inline
bool operator==(const bdeut_BigEndianUint64& lhs,
                const bdeut_BigEndianUint64& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' 'bdeut_BigEndianUint64'
    // objects have the same value, and 'false' otherwise.  Two
    // 'bdeut_BigEndianUint64' objects have the same value if and only if the
    // respective integral network byte-order values that they represent have
    // the same value.

inline
bool operator!=(const bdeut_BigEndianUint64& lhs,
                const bdeut_BigEndianUint64& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' 'bdeut_BigEndianUint64'
    // objects do not have the same value, and 'false' otherwise.  Two
    // 'bdeut_BigEndianUint64' objects do not have the same value if and only
    // if the respective integral network byte-order values that they represent
    // do not have the same value.

inline
bsl::ostream& operator<<(bsl::ostream&                stream,
                         const bdeut_BigEndianUint64& integer);
    // Write the specified 'integer' to the specified output 'stream', and
    // return a reference to the modifiable 'stream'.

// ============================================================================
//                          INLINE FUNCTION DEFINITIONS
// ============================================================================

              // ---------------------------------------------
              // inlined methods used by other inlined methods
              // ---------------------------------------------

inline
bdeut_BigEndianInt16& bdeut_BigEndianInt16::operator=(short value)
{
    d_value = BSLS_BYTEORDER_HTONS(value);
    return *this;
}

inline
bdeut_BigEndianUint16& bdeut_BigEndianUint16::operator=(unsigned short value)
{
    d_value = BSLS_BYTEORDER_HTONS(value);
    return *this;
}

inline
bdeut_BigEndianInt32& bdeut_BigEndianInt32::operator=(int value)
{
    d_value = BSLS_BYTEORDER_HTONL(value);
    return *this;
}

inline
bdeut_BigEndianUint32& bdeut_BigEndianUint32::operator=(unsigned int value)
{
    d_value = BSLS_BYTEORDER_HTONL(value);
    return *this;
}

inline
bdeut_BigEndianInt64& bdeut_BigEndianInt64::operator=(bsls_Types::Int64 value)
{
    d_value = BSLS_BYTEORDER_HOST_U64_TO_BE(value);
    return *this;
}

inline
bdeut_BigEndianUint64& bdeut_BigEndianUint64::operator=(
                                                      bsls_Types::Uint64 value)
{
    d_value = BSLS_BYTEORDER_HOST_U64_TO_BE(value);
    return *this;
}

                         // --------------------------
                         // class bdeut_BigEndianInt16
                         // --------------------------

// CLASS METHODS
inline
int bdeut_BigEndianInt16::maxSupportedBdexVersion()
{
    return 1;
}

inline
bdeut_BigEndianInt16 bdeut_BigEndianInt16::make(short value)
{
    bdeut_BigEndianInt16 ret;
    return ret = value;
}

// MANIPULATORS
template <class STREAM>
STREAM& bdeut_BigEndianInt16::bdexStreamIn(STREAM& stream, int version)
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

// ACCESSORS
inline
bdeut_BigEndianInt16::operator short() const
{
    return BSLS_BYTEORDER_NTOHS(d_value);
}

template <class STREAM>
STREAM& bdeut_BigEndianInt16::bdexStreamOut(STREAM& stream, int version) const
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

// FREE OPERATORS
inline
bool operator==(const bdeut_BigEndianInt16& lhs,
                const bdeut_BigEndianInt16& rhs)
{
    return lhs.d_value == rhs.d_value;
}

inline
bool operator!=(const bdeut_BigEndianInt16& lhs,
                const bdeut_BigEndianInt16& rhs)
{
    return lhs.d_value != rhs.d_value;
}

inline
bsl::ostream& operator<<(bsl::ostream&               stream,
                         const bdeut_BigEndianInt16& integer)
{
    integer.print(stream, 0, -1);
    return stream;
}

                         // ---------------------------
                         // class bdeut_BigEndianUint16
                         // ---------------------------

// CLASS METHODS
inline
int bdeut_BigEndianUint16::maxSupportedBdexVersion()
{
    return 1;
}

inline
bdeut_BigEndianUint16 bdeut_BigEndianUint16::make(unsigned short value)
{
    bdeut_BigEndianUint16 ret;
    return ret = value;
}

// MANIPULATORS
template <class STREAM>
STREAM& bdeut_BigEndianUint16::bdexStreamIn(STREAM& stream, int version)
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

// ACCESSORS
inline
bdeut_BigEndianUint16::operator unsigned short() const
{
    return BSLS_BYTEORDER_NTOHS(d_value);
}

template <class STREAM>
STREAM& bdeut_BigEndianUint16::bdexStreamOut(STREAM& stream, int version) const
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

// FREE OPERATORS
inline
bool operator==(const bdeut_BigEndianUint16& lhs,
                const bdeut_BigEndianUint16& rhs)
{
    return lhs.d_value == rhs.d_value;
}

inline
bool operator!=(const bdeut_BigEndianUint16& lhs,
                const bdeut_BigEndianUint16& rhs)
{
    return lhs.d_value != rhs.d_value;
}

inline
bsl::ostream& operator<<(bsl::ostream&                stream,
                         const bdeut_BigEndianUint16& integer)
{
    integer.print(stream, 0, -1);
    return stream;
}

                         // --------------------------
                         // class bdeut_BigEndianInt32
                         // --------------------------

// CLASS METHODS
inline
int bdeut_BigEndianInt32::maxSupportedBdexVersion()
{
    return 1;
}

inline
bdeut_BigEndianInt32 bdeut_BigEndianInt32::make(int value)
{
    bdeut_BigEndianInt32 ret;
    return ret = value;
}

// MANIPULATORS
template <class STREAM>
STREAM& bdeut_BigEndianInt32::bdexStreamIn(STREAM& stream, int version)
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

// ACCESSORS
inline
bdeut_BigEndianInt32::operator int() const
{
    return BSLS_BYTEORDER_NTOHL(d_value);
}

template <class STREAM>
STREAM& bdeut_BigEndianInt32::bdexStreamOut(STREAM& stream, int version) const
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

// FREE OPERATORS
inline
bool operator==(const bdeut_BigEndianInt32& lhs,
                const bdeut_BigEndianInt32& rhs)
{
    return lhs.d_value == rhs.d_value;
}

inline
bool operator!=(const bdeut_BigEndianInt32& lhs,
                const bdeut_BigEndianInt32& rhs)
{
    return lhs.d_value != rhs.d_value;
}

inline
bsl::ostream& operator<<(bsl::ostream&               stream,
                         const bdeut_BigEndianInt32& integer)
{
    integer.print(stream, 0, -1);
    return stream;
}

                         // ---------------------------
                         // class bdeut_BigEndianUint32
                         // ---------------------------

// CLASS METHODS
inline
int bdeut_BigEndianUint32::maxSupportedBdexVersion()
{
    return 1;
}

inline
bdeut_BigEndianUint32 bdeut_BigEndianUint32::make(unsigned int value)
{
    bdeut_BigEndianUint32 ret;
    return ret = value;
}

// MANIPULATORS
template <class STREAM>
STREAM& bdeut_BigEndianUint32::bdexStreamIn(STREAM& stream, int version)
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

// ACCESSORS
inline
bdeut_BigEndianUint32::operator unsigned int() const
{
    return BSLS_BYTEORDER_NTOHL(d_value);
}

template <class STREAM>
STREAM& bdeut_BigEndianUint32::bdexStreamOut(STREAM& stream, int version) const
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

// FREE OPERATORS
inline
bool operator==(const bdeut_BigEndianUint32& lhs,
                const bdeut_BigEndianUint32& rhs)
{
    return lhs.d_value == rhs.d_value;
}

inline
bool operator!=(const bdeut_BigEndianUint32& lhs,
                const bdeut_BigEndianUint32& rhs)
{
    return lhs.d_value != rhs.d_value;
}

inline
bsl::ostream& operator<<(bsl::ostream&                stream,
                         const bdeut_BigEndianUint32& integer)
{
    integer.print(stream, 0, -1);
    return stream;
}

                         // --------------------------
                         // class bdeut_BigEndianInt64
                         // --------------------------

// CLASS METHODS
inline
int bdeut_BigEndianInt64::maxSupportedBdexVersion()
{
    return 1;
}

inline
bdeut_BigEndianInt64 bdeut_BigEndianInt64::make(bsls_Types::Int64 value)
{
    bdeut_BigEndianInt64 ret;
    return ret = value;
}

// MANIPULATORS
template <class STREAM>
STREAM& bdeut_BigEndianInt64::bdexStreamIn(STREAM& stream, int version)
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

// ACCESSORS
inline
bdeut_BigEndianInt64::operator bsls_Types::Int64() const
{
    return BSLS_BYTEORDER_HOST_U64_TO_BE(d_value);
}

template <class STREAM>
STREAM& bdeut_BigEndianInt64::bdexStreamOut(STREAM& stream, int version) const
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

// FREE OPERATORS
inline
bool operator==(const bdeut_BigEndianInt64& lhs,
                const bdeut_BigEndianInt64& rhs)
{
    return lhs.d_value == rhs.d_value;
}

inline
bool operator!=(const bdeut_BigEndianInt64& lhs,
                const bdeut_BigEndianInt64& rhs)
{
    return lhs.d_value != rhs.d_value;
}

inline
bsl::ostream& operator<<(bsl::ostream&               stream,
                         const bdeut_BigEndianInt64& integer)
{
    integer.print(stream, 0, -1);
    return stream;
}

                         // ---------------------------
                         // class bdeut_BigEndianUint64
                         // ---------------------------

// CLASS METHODS
inline
int bdeut_BigEndianUint64::maxSupportedBdexVersion()
{
    return 1;
}

inline
bdeut_BigEndianUint64 bdeut_BigEndianUint64::make(bsls_Types::Uint64 value)
{
    bdeut_BigEndianUint64 ret;
    return ret = value;
}

// MANIPULATORS
template <class STREAM>
STREAM& bdeut_BigEndianUint64::bdexStreamIn(STREAM& stream, int version)
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

// ACCESSORS
inline
bdeut_BigEndianUint64::operator bsls_Types::Uint64() const
{
    return BSLS_BYTEORDER_HOST_U64_TO_BE(d_value);
}

template <class STREAM>
STREAM& bdeut_BigEndianUint64::bdexStreamOut(STREAM& stream, int version) const
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

// FREE OPERATORS
inline
bool operator==(const bdeut_BigEndianUint64& lhs,
                const bdeut_BigEndianUint64& rhs)
{
    return lhs.d_value == rhs.d_value;
}

inline
bool operator!=(const bdeut_BigEndianUint64& lhs,
                const bdeut_BigEndianUint64& rhs)
{
    return lhs.d_value != rhs.d_value;
}

inline
bsl::ostream& operator<<(bsl::ostream&                stream,
                         const bdeut_BigEndianUint64& integer)
{
    integer.print(stream, 0, -1);
    return stream;
}

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2006
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
