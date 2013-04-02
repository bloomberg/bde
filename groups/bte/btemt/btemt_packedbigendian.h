// btemt_packedbigendian.h               -*-C++-*-
#ifndef INCLUDED_BTEMT_PACKEDBIGENDIAN
#define INCLUDED_BTEMT_PACKEDBIGENDIAN

//@PURPOSE: Provide big endian types that work in the face of byte alignment
//
//@CLASSES:
//   PackedBigEndianInt32:  value semantic, byte aligned, signed 32-bit big
//                          endian value
//   PackedBigEndianUint32: value semantic, byte aligned, unsigned 32-bit big
//                          endian value
//   PackedBigEndianInt16:  value semantic, byte aligned, signed 16-bit big
//                          endian value
//   PackedBigEndianUint16: value semantic, byte aligned, unsigned 16-bit big
//                          endian value
//
//@AUTHOR: Ujjwal Bhoota (ubhoota)
//         Christopher Palmer (cpalmer)
//         Eric Vander Weele (evander)
//
//@SEE ALSO: bdeut_bigendian
//
//@DESCRIPTION: This component defines ...
//
///Usage Example
///-------------
// The following code snippet shows how you can use this class to define a
// struct that is packed and yet still can be cast into a buffer in bigendian
// format from the network without further work:
//..
//  #include <btemt_pack.h> // for BTEMT_PACK1 and BTEMT_UNPACK
//  #pragma BTEMT_PACK1
//  struct Foo {
//      unsigned char     d_data1;
//      PackedBigEndianInt32  d_data2;
//      PackedBigEndianInt16  d_data3;
//      PackedBigEndianUint32 d_data4;
//      PackedBigEndianUint16 d_data5;
//  };
//  #pragma BTEMT_UNPACK
//..

#ifndef INCLUDED_BSLALG_TYPETRAITS
#include <bslalg_typetraits.h>
#endif

#ifndef INCLUDED_BDEU_PRINTMETHODS
#include <bdeu_printmethods.h>
#endif

namespace BloombergLP {
namespace btemt {


                        // ===========================
                        // class PackedBigEndianUint64
                        // ===========================

class PackedBigEndianUint64 {

    // DATA
    unsigned char d_data[8]; // in-core value

  public:
    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(PackedBigEndianUint64,
                                 bdeu_TypeTraitHasPrintMethod);

  public:
    // CREATORS
    PackedBigEndianUint64(unsigned long long value = 0);

    // MANIPULATORS
    PackedBigEndianUint64& operator=(unsigned long long value);

    // ACCESSORS
    operator unsigned long long() const;

    bsl::ostream& print(bsl::ostream& stream,
                        int           level = 0,
                        int           spacesPerLevel = 4) const;
};


                        // ==========================
                        // class PackedBigEndianInt32
                        // ==========================

class PackedBigEndianInt32 {

    // DATA
    unsigned char d_data[4]; // in-core value

  public:
    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(PackedBigEndianInt32,
                                 bdeu_TypeTraitHasPrintMethod);

  public:
    // CREATORS
    PackedBigEndianInt32(int value = 0);

    // MANIPULATORS
    PackedBigEndianInt32& operator=(int value);

    // ACCESSORS
    operator int() const;

    bsl::ostream& print(bsl::ostream& stream,
                        int           level = 0,
                        int           spacesPerLevel = 4) const;
};


                        // ===========================
                        // class PackedBigEndianUint32
                        // ===========================

class PackedBigEndianUint32 {

    // DATA
    unsigned char d_data[4]; // in-core value

  public:
    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(PackedBigEndianUint32,
                                 bdeu_TypeTraitHasPrintMethod);

  public:
    // CREATORS
    PackedBigEndianUint32(unsigned int value = 0);

    // MANIPULATORS
    PackedBigEndianUint32& operator=(unsigned int value);

    // ACCESSORS
    operator unsigned int() const;

    bsl::ostream& print(bsl::ostream& stream,
                        int           level = 0,
                        int           spacesPerLevel = 4) const;
};

                    // ==========================
                    // class PackedBigEndianInt16
                    // ==========================

class PackedBigEndianInt16 {

    // DATA
    unsigned char d_data[2]; // in-core value

  public:
    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(PackedBigEndianInt16,
                                 bdeu_TypeTraitHasPrintMethod);

  public:
    // CREATORS
    PackedBigEndianInt16(short value = 0);

    // MANIPULATORS
    PackedBigEndianInt16& operator=(short value);

    // ACCESSORS
    operator short() const;

    bsl::ostream& print(bsl::ostream& stream,
                        int           level = 0,
                        int           spacesPerLevel = 4) const;
};


                    // ===========================
                    // class PackedBigEndianUint16
                    // ===========================

class PackedBigEndianUint16 {

    // DATA
    unsigned char d_data[2]; // in-core value

  public:
    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(PackedBigEndianUint16,
                                 bdeu_TypeTraitHasPrintMethod);

  public:
    // CREATORS
    PackedBigEndianUint16(unsigned short value = 0);

    // MANIPULATORS
    PackedBigEndianUint16& operator=(unsigned short value);

    // ACCESSORS
    operator unsigned short() const;

    bsl::ostream& print(bsl::ostream& stream,
                        int           level = 0,
                        int           spacesPerLevel = 4) const;
};

// FREE OPERATORS DECLARATIONS
bsl::ostream& operator<<(bsl::ostream&               stream,
                         const PackedBigEndianInt32& value);

bsl::ostream& operator<<(bsl::ostream&               stream,
                         const PackedBigEndianUint32& value);

bsl::ostream& operator<<(bsl::ostream&               stream,
                         const PackedBigEndianInt16& value);

bsl::ostream& operator<<(bsl::ostream&                stream,
                         const PackedBigEndianUint16& value);

//-----------------------------------------------------------------------------
//                      INLINE FUNCTION DEFINITIONS
//-----------------------------------------------------------------------------


                          // ---------------------------
                          // class PackedBigEndianUint64
                          // ---------------------------

// CREATORS
inline
PackedBigEndianUint64::PackedBigEndianUint64(unsigned long long value)
{
    *this = value;
}

// MANIPULATORS
inline
PackedBigEndianUint64& PackedBigEndianUint64::operator=(unsigned long long value)
{
    d_data[0] = (value >> 56) & 0xFF;
    d_data[1] = (value >> 48) & 0xFF;
    d_data[2] = (value >> 40) & 0xFF;
    d_data[3] = (value >> 32) & 0xFF;
    d_data[4] = (value >> 24) & 0xFF;
    d_data[5] = (value >> 16) & 0xFF;
    d_data[6] = (value >>  8) & 0xFF;
    d_data[7] = (value      ) & 0xFF;


    return *this;
}

// ACCESSORS
inline
PackedBigEndianUint64::operator unsigned long long() const
{
    return ((unsigned long long) d_data[0] << 56 | 
            (unsigned long long) d_data[1] << 48 | 
            (unsigned long long) d_data[2] << 40 | 
            (unsigned long long) d_data[3] << 32 |
            (unsigned long long) d_data[4] << 24 | 
            (unsigned long long) d_data[5] << 16 | 
            (unsigned long long) d_data[6] <<  8 | 
            (unsigned long long) d_data[7]);
}



                          // --------------------------
                          // class PackedBigEndianInt32
                          // --------------------------

// CREATORS
inline
PackedBigEndianInt32::PackedBigEndianInt32(int value)
{
    *this = value;
}

// MANIPULATORS
inline
PackedBigEndianInt32& PackedBigEndianInt32::operator=(int value)
{
    d_data[0] = (value >> 24) & 0xFF;
    d_data[1] = (value >> 16) & 0xFF;
    d_data[2] = (value >> 8) & 0xFF;
    d_data[3] = value & 0xFF;
    return *this;
}

// ACCESSORS
inline
PackedBigEndianInt32::operator int() const
{
    return (d_data[0] << 24 | d_data[1] << 16 | d_data[2] <<  8 | d_data[3]);
}

                          // ---------------------------
                          // class PackedBigEndianUint32
                          // ---------------------------

// CREATORS
inline
PackedBigEndianUint32::PackedBigEndianUint32(unsigned int value)
{
    *this = value;
}

// MANIPULATORS
inline
PackedBigEndianUint32& PackedBigEndianUint32::operator=(unsigned int value)
{
    d_data[0] = (value >> 24) & 0xFF;
    d_data[1] = (value >> 16) & 0xFF;
    d_data[2] = (value >> 8) & 0xFF;
    d_data[3] = value & 0xFF;
    return *this;
}

// ACCESSORS
inline
PackedBigEndianUint32::operator unsigned int() const
{
    return (d_data[0] << 24 | d_data[1] << 16 | d_data[2] <<  8 | d_data[3]);
}

                          // --------------------------
                          // class PackedBigEndianInt16
                          // --------------------------

// CREATORS
inline
PackedBigEndianInt16::PackedBigEndianInt16(short value)
{
    *this = value;
}

// MANIPULATORS
inline
PackedBigEndianInt16& PackedBigEndianInt16::operator=(short value)
{
    d_data[0] = (value >> 8) & 0xFF;
    d_data[1] = value & 0xFF;
    return *this;
}

// ACCESSORS
inline
PackedBigEndianInt16::operator short() const
{
    return (d_data[0] <<  8 | d_data[1]);
}

                          // ---------------------------
                          // class PackedBigEndianUint16
                          // ---------------------------

// CREATORS
inline
PackedBigEndianUint16::PackedBigEndianUint16(unsigned short value)
{
    *this = value;
}

// MANIPULATORS
inline
PackedBigEndianUint16& PackedBigEndianUint16::operator=(unsigned short value)
{
    d_data[0] = (value >> 8) & 0xFF;
    d_data[1] = value & 0xFF;
    return *this;
}

// ACCESSORS
inline
PackedBigEndianUint16::operator unsigned short() const
{
    return (d_data[0] <<  8 | d_data[1]);
}


} // close package-level namespace

inline
bsl::ostream& btemt::operator<<(bsl::ostream&                     stream,
                               const btemt::PackedBigEndianInt32& rhs)
{
    return rhs.print(stream, 0, -1);
}

inline
bsl::ostream& btemt::operator<<(bsl::ostream&                     stream,
                               const btemt::PackedBigEndianUint32& rhs)
{
    return rhs.print(stream, 0, -1);
}

inline
bsl::ostream& btemt::operator<<(bsl::ostream&                     stream,
                               const btemt::PackedBigEndianInt16& rhs)
{
    return rhs.print(stream, 0, -1);
}

inline
bsl::ostream& btemt::operator<<(bsl::ostream&                      stream,
                               const btemt::PackedBigEndianUint16& rhs)
{
    return rhs.print(stream, 0, -1);
}

}  // close enterprise-wide namespace

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2010
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
