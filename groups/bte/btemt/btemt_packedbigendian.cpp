// btemt_packedbigendian.cpp               -*-C++-*-
#include <btemt_packedbigendian.h>

#include <bdeut_bigendian.h>
#include <bsl_cstring.h>

namespace BloombergLP {
namespace btemt {

                        // ---------------------------
                        // class PackedBigEndianUint64
                        // ---------------------------

// ACCESSORS
bsl::ostream& PackedBigEndianUint64::print(bsl::ostream& stream,
                                           int           level,
                                           int           spacesPerLevel) const
{
    bdeut_BigEndianUint64 tmp;
    bsl::memcpy(&tmp, this, sizeof(*this)); // TBD optimize
    return tmp.print(stream, level, spacesPerLevel);
}

                        // --------------------------
                        // class PackedBigEndianInt32
                        // --------------------------

// ACCESSORS
bsl::ostream& PackedBigEndianInt32::print(bsl::ostream& stream,
                                          int           level,
                                          int           spacesPerLevel) const
{
    bdeut_BigEndianInt32 tmp;
    bsl::memcpy(&tmp, this, sizeof(*this)); // TBD optimize
    return tmp.print(stream, level, spacesPerLevel);
}

                        // ---------------------------
                        // class PackedBigEndianUint32
                        // ---------------------------

// ACCESSORS
bsl::ostream& PackedBigEndianUint32::print(bsl::ostream& stream,
                                           int           level,
                                           int           spacesPerLevel) const
{
    bdeut_BigEndianUint32 tmp;
    bsl::memcpy(&tmp, this, sizeof(*this)); // TBD optimize
    return tmp.print(stream, level, spacesPerLevel);
}

                        // --------------------------
                        // class PackedBigEndianInt16
                        // --------------------------

// ACCESSORS
bsl::ostream& PackedBigEndianInt16::print(bsl::ostream& stream,
                                          int           level,
                                          int           spacesPerLevel) const
{
    bdeut_BigEndianInt16 tmp;
    bsl::memcpy(&tmp, this, sizeof(*this)); // TBD optimize
    return tmp.print(stream, level, spacesPerLevel);
}

                        // ---------------------------
                        // class PackedBigEndianUint16
                        // ---------------------------

// ACCESSORS
bsl::ostream& PackedBigEndianUint16::print(bsl::ostream& stream,
                                           int           level,
                                           int           spacesPerLevel) const
{
    bdeut_BigEndianUint16 tmp;
    bsl::memcpy(&tmp, this, sizeof(*this)); // TBD optimize
    return tmp.print(stream, level, spacesPerLevel);
}

}  // close package-level namespace

}  // close enterprise-wide namespace

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2010
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
