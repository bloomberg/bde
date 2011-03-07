// bdex_bytestreamimputil.h                                           -*-C++-*-
#ifndef INCLUDED_BDEX_BYTESTREAMIMPUTIL
#define INCLUDED_BDEX_BYTESTREAMIMPUTIL

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Support platform-independent representation of fundamental types.
//
//@CLASSES:
//   bdex_ByteStreamImpUtil: namespace for put/get externalization functions
//
//@AUTHOR: John Lakos (jlakos)
//
//@SEE_ALSO: bdex_byteinstream, bdex_byteoutstream
//
//@DESCRIPTION: This component provides a byte-array-based implementation for a
// suite of externalization and unexternalization functions used to covert
// values (and arrays of values) of the following fundamental integer and
// floating-point types:
//..
//      C++ TYPE          REQUIRED CONTENT OF ANY PLATFORM NEUTRAL FORMAT
//      --------          -----------------------------------------------
//      Int64             least significant 64 bits (signed)
//      Uint64            least significant 64 bits (unsigned)
//      int               least significant 32 bits (signed)
//      unsigned int      least significant 32 bits (unsigned)
//      short             least significant 16 bits (signed)
//      unsigned short    least significant 16 bits (unsigned)
//      char              least significant  8 bits (platform dependent)
//      signed char       least significant  8 bits (signed)
//      unsigned char     least significant  8 bits (unsigned)
//      double            IEEE standard 8-byte floating-point value
//      float             IEEE standard 4-byte floating-point value
//..
// In addition to basic streaming methods, where each streamed instance of a
// fundamental type occupies the same number of bytes in the stream
// (regardless of its value), this component provides an interface
// for efficient streaming of integer types.  In particular, 64-bit values
// can be streamed as 40, 48, 56, or 64-bit values, and 32-bit values can be
// streamed as 24 and 32-bit values.  Externalized integers are written and
// assumed to be in two's complement, 'Big Endian' (i.e., network byte order)
// format.  Floating point formats are described below.
//
///Note on Function Naming and Interface
///-------------------------------------
// The names and interfaces of the methods of 'bdex_ByteStreamImpUtil' follow a
// systematic fashion explained below.  This makes it easier to guess the name
// and signature of the intended function.  In what follows, 'buffer' is always
// of type 'char *' or 'const char *' depending on whether it is used as an
// input or an output, and 'variable' and 'value' are of a type that depends on
// the name of the method and intended width, with 'variable' used as an
// output, while 'value' is used as an input.
//
// Here are the 'get...' methods for integral and floating-point scalar types.
//..
//   Name                         Type of 'variable'           Notes
//   ----                         ------------------           -----
//   getIntNN(variable, buffer)   bsls_PlatformUtil::Int64 *   NN=64,56,48,40
//                                int *                        NN=32,24
//                                short *                      NN=16
//                                char *                       NN=8
//                                signed char *                NN=8
//                                unsigned char *              NN=8
//
//   getUintNN(variable, buffer)  bsls_PlatformUtil::Uint64 *  NN=64,56,48,40
//                                int *                        NN=32,24
//                                unsigned short *             NN=16
//
//   getFloatNN(variable, buffer) double *                     NN=64
//                                float *                      NN=32
//..
// Here are the 'put...' methods for scalar types.  Note that there is no
// 'putUintNN' since 'putIntNN' applies equally to unsigned 'NN'-bit values
// (through a conversion to a signed value).
//..
//   Name                         Type of 'value'              Notes
//   ----                         ---------------              -----
//   putIntNN(buffer, value)      bsls_PlatformUtil::Int64     NN=64,56,48,40
//                                int                          NN=32,24,16,8
//
//   putFloatNN(buffer, value)    double                       NN=64
//                                float                        NN=32
//..
//
//
// Finally, the 'put...' array methods follow.  Note that this time, there is
// an overload for unsigned types, but that the function name is still
// 'putArrayInt...' for arrays of both signed and unsigned integrals.
//..
//   Name                   Type of 'array'                    Notes
//   ----                   ---------------                    -----
//   putArrayIntNN(buffer,  const bsls_PlatformUtil::Int64 *   NN=64,56,48,40
//                 array,   const bsls_PlatformUtil::Uint64 *  NN=64,56,48,40
//                 count)   const short *                      NN=32,34
//                          const unsigned short *             NN=16
//                          const char *                       NN=8
//                          const signed char *                NN=8
//                          const unsigned char *              NN=8
//..
///Performance
///-----------
// The performance is a function of both the platform and the optimization
// level (your mileage may vary).  A rough idea of the marshaling rate is
// given in the following tables.
//..
//      ====================================================================
//                      put/getFloat64 -- 10,000,000 times
//      --------------------------------------------------------------------
//      Optimization    Platform  Time                          bytes/second
//      --------        --------  ----                          ------------
//      NONE            sundev1   4.63u 0.04s 0:04.68 99.7%     17.3 * 10^6
//      NONE            sundev2   2.08u 0.01s 0:02.16 97.2%     38.5 * 10^6
//
//      -O              sundev1   2.82u 0.03s 0:02.88 98.9%     28.4 * 10^6
//      -O              sundev2   1.60u 0.02s 0:01.63 99.3%     50.0 * 10^6
//
//      -O2             sundev1   2.81u 0.03s 0:02.85 99.6%     28.4 * 10^6
//      -O2             sundev2   1.60u 0.01s 0:01.63 98.7%     50.0 * 10^6
//
//      -O3             sundev1   2.87u 0.02s 0:02.93 98.6%     27.9 * 10^6
//      -O3             sundev2   1.60u 0.02s 0:01.63 99.3%     50.0 * 10^6
//
//      -O4             sundev1   2.88u 0.04s 0:02.94 99.3%     27.8 * 10^6
//      -O4             sundev2   1.59u 0.01s 0:01.62 98.7%     50.3 * 10^6
//      ====================================================================
//              put/getArrayFloat64(10,000 items) -- 1000 times
//      --------------------------------------------------------------------
//      Optimization    Platform  Time                          bytes/second
//      --------        --------  ----                          ------------
//      NONE            sundev1   3.33u 0.12s 0:03.54 97.4%     24.0 * 10^6
//      NONE            sundev2   1.21u 0.01s 0:01.24 99.1%     66.1 * 10^6
//
//      -O              sundev1   2.57u 0.15s 0:02.85 95.4%     31.1 * 10^6
//      -O              sundev2   0.92u 0.01s 0:00.94 98.9%     87.0 * 10^6
//
//      -O2             sundev1   2.57u 0.03s 0:02.60 100.0%    31.1 * 10^6
//      -O2             sundev2   0.98u 0.01s 0:01.02 98.0%     81.6 * 10^6
//
//      -O3             sundev1   2.64u 0.17s 0:02.95 95.2%     30.3 * 10^6
//      -O3             sundev2   1.00u 0.01s 0:01.03 99.0%     80.0 * 10^6
//
//      -O4             sundev1   2.51u 0.07s 0:02.59 99.6%     31.8 * 10^6
//      -O4             sundev2   1.14u 0.01s 0:01.17 99.1%     70.1 * 10^6
//      ====================================================================
//..
///IEEE 754 Double-Precision Format
///--------------------------------
// A 'double' is assumed to be _at_ _least_ 64 bits in size.  The externalized
// byte representation of a 64-bit floating-point value is defined to conform
// to the IEEE double-precision format illustrated below.  If the native
// representation of a 64-bit floating point value does not match this
// format, a conversion process to and from this format is performed.
// This conversion may (of course) be lossy.
//..
//  sign bit    11-bit exponent             52-bit mantissa
//    /        /                           /
//  +-+-----------+----------------------------------------------------+
//  |s|e10......e0|m0...............................................m51|
//  +-+-----------+----------------------------------------------------+
//  LSB                                                              MSB
//..
///IEEE 754 Single-Precision Format
///--------------------------------
// A 'float' is assumed to be _at_ _least_ 32 bits in size.  The externalized
// byte representation of a 32-bit floating-point value is defined to conform
// to the IEEE single-precision format illustrated below.  If the native
// representation of a 32-bit floating point value does not match this
// format, a conversion process to and from this format is performed.
// This conversion may (of course) be lossy.
//..
//  sign bit    8-bit exponent        23-bit mantissa
//     /       /                     /
//    +-+--------+-----------------------+
//    |s|e7....e0|m0..................m22|
//    +-+--------+-----------------------+
//    LSB                              MSB
//..
///Usage
///-----
///Example 1
///- - - - -
// The 'bdex_ByteStreamImpUtil' component can be used stand-alone to save
// and restore a platform neutral representation of fundamental data to and
// from a byte buffer.
//..
// class MyPoint {
//     int d_x;
//     int d_y;
//
//     MyPoint(const MyPoint&);         // not used in example
//     MyPoint& operator=(const MyPoint&);      // not used in example
//
//   public:
//     MyPoint() : d_x(0), d_y(0) {}
//     MyPoint(int x, int y) : d_x(x), d_y(y) {}
//
//     // ...
//
//     void setX(int x) { d_x = x; }
//     void setY(int y) { d_y = y; }
//
//     int x() const { return d_x; }
//     int y() const { return d_y; }
// };
//
// inline bool operator==(const MyPoint& lhs, const MyPoint& rhs)  {
//     return lhs.x() == rhs.x() && lhs.y() == rhs.y();
// }
//
// inline bool operator!=(const MyPoint& lhs, const MyPoint& rhs)  {
//     return lhs.x() != rhs.x() || lhs.y() != rhs.y();
// }
//
// int savePoint(char *buffer, int bufferLength, const MyPoint& point)
// {
//     enum { INSUFFICIENT_SPACE = -1, SUCCESS = 0 };
//     if (bufferLength < 4 + 4) {
//         return INSUFFICIENT_SPACE;
//     }
//     bdex_ByteStreamImpUtil::putInt32(buffer + 0, point.x());
//     bdex_ByteStreamImpUtil::putInt32(buffer + 4, point.y());
//     return SUCCESS;
// }
//
// int restorePoint(MyPoint *point, const char *buffer, int bufferLength)
// {
//     enum { SUCCESS = 0, INSUFFICIENT_DATA = 1 };
//     if (bufferLength < 4 + 4) {
//         return INSUFFICIENT_DATA;
//     }
//
//     int result;
//     bdex_ByteStreamImpUtil::getInt32(&result, buffer + 0);
//     point->setX(result);
//     bdex_ByteStreamImpUtil::getInt32(&result, buffer + 4);
//     point->setY(result);
//     return SUCCESS;
// }
//
// void assignPointTheHardWay(MyPoint *dstPoint, const MyPoint& srcPoint)
// {
//     assert(dstPoint);
//
//     char buffer[sizeof srcPoint];
//
//     int s;
//     s = savePoint(buffer, sizeof buffer - 1, srcPoint);     assert(0 != s);
//     s = savePoint(buffer, sizeof buffer,     srcPoint);     assert(0 == s);
//
//     s = restorePoint(dstPoint, buffer, sizeof buffer - 1);  assert(0 != s);
//     s = restorePoint(dstPoint, buffer, sizeof buffer    );  assert(0 == s);
//
//     assert(*dstPoint == srcPoint);
// }
//
// void f()
// {
//     MyPoint x(3, 2);
//     MyPoint y;                     assert(x != y);  assert(!(x == y));
//     assignPointTheHardWay(&y, x);  assert(x == y);  assert(!(x != y));
// }
//..
///Example 2
///- - - - -
// Another intended usage is to facilitate the creation of concrete streaming
// objects derived from abstract streaming interfaces such as 'bdex_InStream'
// and 'bdex_OutStream' (see 'bdex_ByteInStream' and 'bdex_ByteOutStream).

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORMUTIL
#include <bsls_platformutil.h>
#endif

#ifndef INCLUDED_CSTRING
#include <cstring>           // for 'memcpy'
#define INCLUDED_CSTRING
#endif

namespace BloombergLP {

                        // =============================
                        // struct bdex_ByteStreamImpUtil
                        // =============================

struct bdex_ByteStreamImpUtil {
    // This 'struct' provides a namespace for a suite of pure procedures that
    // facilitate the (un)externalization of values (and C-style arrays of
    // values) of the fundamental integral and floating-point types in a
    // data-independent, platform-neutral representation.

    // CLASS METHODS

                        // *** putting scalar integral values ***

    static void putInt64(char *buffer, bsls_PlatformUtil::Int64 value);
        // Format, as an eight-byte, two's complement integer (in network byte
        // order), the least significant 64 bits of the specified 'value' (in
        // host byte order) to the specified byte 'buffer'.  Note that this
        // function applies equally to unsigned 64-bit values.

    static void putInt56(char *buffer, bsls_PlatformUtil::Int64 value);
        // Format, as a seven-byte, two's complement integer (in network byte
        // order), the least significant 56 bits of the specified 'value' (in
        // host byte order) to the specified byte 'buffer'.  Note that this
        // function applies equally to unsigned 64-bit values.

    static void putInt48(char *buffer, bsls_PlatformUtil::Int64 value);
        // Format, as a six-byte, two's complement integer (in network byte
        // order), the least significant 48 bits of the specified 'value' (in
        // host byte order) to the specified byte 'buffer'.  Note that this
        // function applies equally to unsigned 64-bit values.

    static void putInt40(char *buffer, bsls_PlatformUtil::Int64 value);
        // Format, as a five-byte, two's complement integer (in network byte
        // order), the least significant 40 bits of the specified 'value' (in
        // host byte order) to the specified byte 'buffer'.  Note that this
        // function applies equally to unsigned 64-bit values.

    static void putInt32(char *buffer, int value);
        // Format, as a four-byte, two's complement integer (in network byte
        // order), the least significant 32 bits of the specified 'value' (in
        // host byte order) to the specified byte 'buffer'.  Note that this
        // function applies equally to unsigned 32-bit values.

    static void putInt24(char *buffer, int value);
        // Format, as a three-byte, two's complement integer (in network byte
        // order), the least significant 24 bits of the specified 'value' (in
        // host byte order) to the specified byte 'buffer'.  Note that this
        // function applies equally to unsigned 32-bit values.

    static void putInt16(char *buffer, int value);
        // Format, as a two-byte, two's complement integer (in network byte
        // order), the least significant 16 bits of the specified 'value' (in
        // host byte order) to the specified byte 'buffer'.  Note that this
        // function applies equally to unsigned 32-bit values.

    static void putInt8(char *buffer, int value);
        // Format, as a one-byte, two's complement integer (in network byte
        // order), the least significant 8 bits of the specified 'value' (in
        // host byte order) to the specified byte 'buffer'.  Note that this
        // function applies equally to unsigned 32-bit values.

                        // *** putting scalar floating-point values ***

    static void putFloat64(char *buffer, double value);
        // Format, as an 8-byte IEEE double-precision floating point number
        // (in network byte order), the most significant 8-bytes worth of
        // information in the specified 'value' (in host byte order) to the
        // specified byte 'buffer'.  Note that for non-conforming platforms,
        // this operation may be lossy.

    static void putFloat32(char *buffer, float value);
        // Format, as a 4-byte IEEE double-precision floating point number (in
        // network byte order), the most significant 4-bytes worth of
        // information in the specified 'value' (in host byte order) to the
        // specified byte 'buffer'.  Note that for non-conforming platforms,
        // this operation may be lossy.

                        // *** getting scalar integral values ***

    static void getInt64(bsls_PlatformUtil::Int64 *variable,
                         const char               *buffer);
        // Read eight bytes from the specified 'buffer' as a 64-bit, two's
        // complement integer (in network byte order) and store that value
        // in the specified 'variable' (in host byte order).  Note that the
        // value will be sign-extended if 'variable' holds more than 64 bits
        // on this platform.

    static void getUint64(bsls_PlatformUtil::Uint64 *variable,
                          const char                *buffer);
        // Read eight bytes from the specified 'buffer' as a 64-bit unsigned
        // integer (in network byte order) and store that value in the
        // specified 'variable' (in host byte order).  Note that the value will
        // be zero-extended if 'variable' holds more than 64 bits on this
        // platform.

    static void getInt56(bsls_PlatformUtil::Int64 *variable,
                         const char               *buffer);
        // Read seven bytes from the specified 'buffer' as a 56-bit, two's
        // complement integer (in network byte order) and store that value
        // in the specified 'variable' (in host byte order).  Note that the
        // value will be sign-extended to the size of the 'variable'
        // on this platform.

    static void getUint56(bsls_PlatformUtil::Uint64 *variable,
                          const char                *buffer);
        // Read seven bytes from the specified 'buffer' as a 56-bit unsigned
        // integer (in network byte order) and store that value in the
        // specified 'variable' (in host byte order).  Note that the value will
        // be zero-extended to the size of the 'variable' on this platform.

    static void getInt48(bsls_PlatformUtil::Int64 *variable,
                         const char               *buffer);
        // Read six bytes from the specified 'buffer' as a 48-bit, two's
        // complement integer (in network byte order) and store that value
        // in the specified 'variable' (in host byte order).  Note that the
        // value will be sign-extended to the size of the 'variable'
        // on this platform.

    static void getUint48(bsls_PlatformUtil::Uint64 *variable,
                          const char                *buffer);
        // Read six bytes from the specified 'buffer' as a 48-bit unsigned
        // integer (in network byte order) and store that value in the
        // specified 'variable' (in host byte order).  Note that the value will
        // be zero-extended to the size of the 'variable' on this platform.

    static void getInt40(bsls_PlatformUtil::Int64 *variable,
                         const char               *buffer);
        // Read five bytes from the specified 'buffer' as a 40-bit, two's
        // complement integer (in network byte order) and store that value
        // in the specified 'variable' (in host byte order).  Note that the
        // value will be sign-extended to the size of the 'variable'
        // on this platform.

    static void getUint40(bsls_PlatformUtil::Uint64 *variable,
                          const char                *buffer);
        // Read five bytes from the specified 'buffer' as a 40-bit unsigned
        // integer (in network byte order) and store that value in the
        // specified 'variable' (in host byte order).  Note that the value will
        // be zero-extended to the size of the 'variable' on this platform.

    static void getInt32(int *variable, const char *buffer);
        // Read four bytes from the specified 'buffer' as a 32-bit, two's
        // complement integer (in network byte order) and store that value
        // in the specified 'variable' (in host byte order).  Note that the
        // value will be sign-extended if 'variable' holds more than 32 bits
        // on this platform.

    static void getUint32(unsigned int *variable, const char *buffer);
        // Read four bytes from the specified 'buffer' as a 32-bit unsigned
        // integer (in network byte order) and store that value in the
        // specified 'variable' (in host byte order).  Note that the value will
        // be zero-extended if 'variable' holds more than 32 bits on this
        // platform.

    static void getInt24(int *variable, const char *buffer);
        // Read three bytes from the specified 'buffer' as a 24-bit, two's
        // complement integer (in network byte order) and store that value
        // in the specified 'variable' (in host byte order).  Note that the
        // value will be sign-extended to the size of the 'variable'
        // on this platform.

    static void getUint24(unsigned int *variable, const char *buffer);
        // Read three bytes from the specified 'buffer' as a 24-bit unsigned
        // integer (in network byte order) and store that value in the
        // specified 'variable' (in host byte order).  Note that the value will
        // be zero-extended to the size of the 'variable' on this platform.

    static void getInt16(short *variable, const char *buffer);
        // Read two bytes from the specified 'buffer' as a 16-bit, two's
        // complement integer (in network byte order) and store that value
        // in the specified 'variable' (in host byte order).  Note that the
        // value will be sign-extended if 'variable' holds more than 16 bits
        // on this platform

    static void getUint16(unsigned short *variable, const char *buffer);
        // Read two bytes from the specified 'buffer' as a 16-bit unsigned
        // integer (in network byte order) and store that value in the
        // specified 'variable' (in host byte order).  Note that the value will
        // be zero-extended if 'variable' holds more than 16 bits on this
        // platform.

    static void getInt8(char *variable, const char *buffer);
    static void getInt8(signed char *variable, const char *buffer);
    static void getInt8(unsigned char *variable, const char *buffer);
        // Read one byte from the specified 'buffer' as an 8-bit, two's
        // complement integer and store that value in the specified 'variable'.

                        // *** getting scalar floating-point values ***

    static void getFloat64(double *variable, const char *buffer);
        // Read eight bytes from the specified 'buffer' as a standard IEEE
        // double-precision floating-point number (in network byte order) and
        // store that value in the specified 'variable' (in host byte order).
        // Note that for non-conforming platforms, this operation may be lossy.

    static void getFloat32(float *variable, const char *buffer);
        // Read four bytes from the specified 'buffer' as a standard IEEE
        // single-precision floating-point number (in network byte order) and
        // store that value in the specified 'variable' (in host byte order).
        // Note that for non-conforming platforms, this operation may be lossy.

                        // *** putting arrays of integral values ***

    static void putArrayInt64(char                           *buffer,
                              const bsls_PlatformUtil::Int64 *array,
                              int                             count);
    static void putArrayInt64(char                            *buffer,
                              const bsls_PlatformUtil::Uint64 *array,
                              int                              count);
        // Format, as consecutive eight-byte, two's complement integers (in
        // network byte order), the least significant 64 bits of each of the
        // specified 'count' leading entires in the specified 'array' (in host
        // byte order) to the specified 'buffer'.  The behavior is undefined
        // unless '0 <= count'.

    static void putArrayInt56(char                           *buffer,
                              const bsls_PlatformUtil::Int64 *array,
                              int                             count);
    static void putArrayInt56(char                            *buffer,
                              const bsls_PlatformUtil::Uint64 *array,
                              int                              count);
        // Format, as consecutive seven-byte, two's complement integers (in
        // network byte order), the least significant 56 bits of each of the
        // specified 'count' leading entires in the specified 'array' (in host
        // byte order) to the specified 'buffer'.  The behavior is undefined
        // unless '0 <= count'.

    static void putArrayInt48(char                           *buffer,
                              const bsls_PlatformUtil::Int64 *array,
                              int                             count);
    static void putArrayInt48(char                            *buffer,
                              const bsls_PlatformUtil::Uint64 *array,
                              int                              count);
        // Format, as consecutive six-byte, two's complement integers (in
        // network byte order), the least significant 48 bits of each of the
        // specified 'count' leading entires in the specified 'array' (in host
        // byte order) to the specified 'buffer'.  The behavior is undefined
        // unless '0 <= count'.

    static void putArrayInt40(char                           *buffer,
                              const bsls_PlatformUtil::Int64 *array,
                              int                             count);
    static void putArrayInt40(char                            *buffer,
                              const bsls_PlatformUtil::Uint64 *array,
                              int                              count);
        // Format, as consecutive five-byte, two's complement integers (in
        // network byte order), the least significant 40 bits of each of the
        // specified 'count' leading entires in the specified 'array' (in host
        // byte order) to the specified 'buffer'.  The behavior is undefined
        // unless '0 <= count'.

    static void putArrayInt32(char *buffer, const int *array, int count);
    static void putArrayInt32(char               *buffer,
                              const unsigned int *array,
                              int                 count);
        // Format, as consecutive four-byte, two's complement integers (in
        // network byte order), the least significant 32 bits of each of the
        // specified 'count' leading entires in the specified 'array' (in host
        // byte order) to the specified 'buffer'.  The behavior is undefined
        // unless '0 <= count'.

    static void putArrayInt24(char *buffer, const int *array, int count);
    static void putArrayInt24(char               *buffer,
                              const unsigned int *array,
                              int                 count);
        // Format, as consecutive three-byte, two's complement integers (in
        // network byte order), the least significant 24 bits of each of the
        // specified 'count' leading entires in the specified 'array' (in host
        // byte order) to the specified 'buffer'.  The behavior is undefined
        // unless '0 <= count'.

    static void putArrayInt16(char *buffer, const short *array, int count);
    static void putArrayInt16(char                 *buffer,
                              const unsigned short *array,
                              int                   count);
        // Format, as consecutive two-byte, two's complement integers (in
        // network byte order), the least significant 16 bits of each of the
        // specified 'count' leading entires in the specified 'array' (in host
        // byte order) to the specified 'buffer'.  The behavior is undefined
        // unless '0 <= count'.

    static void putArrayInt8(char *buffer, const char *array, int count);
    static void putArrayInt8(char              *buffer,
                             const signed char *array,
                             int                count);
    static void putArrayInt8(char                *buffer,
                             const unsigned char *array,
                             int                  count);
        // Format, as consecutive one-byte, two's complement integers, each of
        // the specified 'count' leading entires in the specified 'array' to
        // the specified 'buffer'.  The behavior is undefined unless
        // '0 <= count'.

                        // *** putting arrays of floating-point values ***

    static void putArrayFloat64(char *buffer, const double *array, int count);
        // Format, as consecutive 8-byte IEEE double-precision floating point
        // numbers (in network byte order), the most significant 8-bytes worth
        // of information in each of the specified 'count' leading entries in
        // the specified 'array' (in host byte order) to the specified byte
        // 'buffer'.  The behavior is undefined unless '0 <= count'.  Note
        // that for non-conforming platforms, this operation may be lossy.

    static void putArrayFloat32(char *buffer, const float *array, int count);
        // Format, as consecutive 4-byte IEEE double-precision floating point
        // numbers (in network byte order), the most significant 4-bytes worth
        // of information in each of the specified 'count' leading entries in
        // the specified 'array' (in host byte order) to the specified byte
        // 'buffer'.  The behavior is undefined unless '0 <= count'.  Note
        // that for non-conforming platforms, this operation may be lossy.

                        // *** getting arrays of integral values ***

    static void getArrayInt64(bsls_PlatformUtil::Int64 *array,
                              const char               *buffer,
                              int                       count);
        // Read the specified 'count' eight-byte, two's complement integers (in
        // network byte order) from the specified 'buffer' and store these
        // values sequentially in the the specified 'array' (in host byte
        // order).  The behavior is undefined unless '0 <= count'.  Note that
        // each element value will be sign-extended if '*array' holds more than
        // 64 bits on this platform.

    static void getArrayUint64(bsls_PlatformUtil::Uint64 *array,
                               const char                *buffer,
                               int                        count);
        // Read the specified 'count' eight-byte unsigned integers (in
        // network byte order) from the specified 'buffer' and store these
        // values sequentially in the the specified 'array' (in host byte
        // order).  The behavior is undefined unless '0 <= count'.  Note that
        // each element value will be zero-extended if '*array' holds more than
        // 64 bits on this platform.

    static void getArrayInt56(bsls_PlatformUtil::Int64 *array,
                              const char               *buffer,
                              int                       count);
        // Read the specified 'count' seven-byte unsigned integers (in
        // network byte order) from the specified 'buffer' and store these
        // values sequentially in the the specified 'array' (in host byte
        // order).  The behavior is undefined unless '0 <= count'.  Note that
        // each element value will be sign-extended to the size of the array
        // element on this platform.

    static void getArrayUint56(bsls_PlatformUtil::Uint64 *array,
                               const char                *buffer,
                               int                        count);
        // Read the specified 'count' seven-byte, two's complement integers (in
        // network byte order) from the specified 'buffer' and store these
        // values sequentially in the the specified 'array' (in host byte
        // order).  The behavior is undefined unless '0 <= count'.  Note that
        // each element value will be zero-extended to the size of the array
        // element on this platform.

    static void getArrayInt48(bsls_PlatformUtil::Int64 *array,
                              const char               *buffer,
                              int                       count);
        // Read the specified 'count' six-byte, two's complement integers (in
        // network byte order) from the specified 'buffer' and store these
        // values sequentially in the the specified 'array' (in host byte
        // order).  The behavior is undefined unless '0 <= count'.  Note that
        // each element value will be sign-extended to the size of the array
        // element on this platform.

    static void getArrayUint48(bsls_PlatformUtil::Uint64 *array,
                               const char                *buffer,
                               int                        count);
        // Read the specified 'count' six-byte, two's complement integers (in
        // network byte order) from the specified 'buffer' and store these
        // values sequentially in the the specified 'array' (in host byte
        // order).  The behavior is undefined unless '0 <= count'.  Note that
        // each element value will be zero-extended to the size of the array
        // element on this platform.

    static void getArrayInt40(bsls_PlatformUtil::Int64 *array,
                              const char               *buffer,
                              int                       count);
        // Read the specified 'count' five-byte, two's complement integers (in
        // network byte order) from the specified 'buffer' and store these
        // values sequentially in the the specified 'array' (in host byte
        // order).  The behavior is undefined unless '0 <= count'.  Note that
        // each element value will be sign-extended to the size of the array
        // element on this platform.

    static void getArrayUint40(bsls_PlatformUtil::Uint64 *array,
                               const char                *buffer,
                               int                        count);
        // Read the specified 'count' five-byte, two's complement integers (in
        // network byte order) from the specified 'buffer' and store these
        // values sequentially in the the specified 'array' (in host byte
        // order).  The behavior is undefined unless '0 <= count'.  Note that
        // each element value will be zero-extended to the size of the array
        // element on this platform.

    static void getArrayInt32(int *array, const char *buffer, int count);
        // Read the specified 'count' four-byte, two's complement integers (in
        // network byte order) from the specified 'buffer' and store these
        // values sequentially in the the specified 'array' (in host byte
        // order).  The behavior is undefined unless '0 <= count'.  Note that
        // each element value will be zero-extended to the size of the array
        // element on this platform.

    static void getArrayUint32(unsigned int *array,
                               const char   *buffer,
                               int           count);
        // Read the specified 'count' four-byte, two's complement integers (in
        // network byte order) from the specified 'buffer' and store these
        // values sequentially in the the specified 'array' (in host byte
        // order).  The behavior is undefined unless '0 <= count'.  Note that
        // each element value will be zero-extended to the size of the array
        // element on this platform.

    static void getArrayInt24(int        *array,
                              const char *buffer,
                              int         count);
        // Read the specified 'count' three-byte, two's complement integers (in
        // network byte order) from the specified 'buffer' and store these
        // values sequentially in the the specified 'array' (in host byte
        // order).  The behavior is undefined unless '0 <= count'.  Note that
        // each element value will be zero-extended to the size of the array
        // element on this platform.

    static void getArrayUint24(unsigned int *array,
                               const char   *buffer,
                               int           count);
        // Read the specified 'count' three-byte, two's complement integers (in
        // network byte order) from the specified 'buffer' and store these
        // values sequentially in the the specified 'array' (in host byte
        // order).  The behavior is undefined unless '0 <= count'.  Note that
        // each element value will be zero-extended to the size of the array
        // element on this platform.

    static void getArrayInt16(short *array, const char *buffer, int count);
        // Read the specified 'count' two-byte, two's complement integers (in
        // network byte order) from the specified 'buffer' and store these
        // values sequentially in the the specified 'array' (in host byte
        // order).  The behavior is undefined unless '0 <= count'.  Note that
        // each element value will be zero-extended to the size of the array
        // element on this platform.

    static void getArrayUint16(unsigned short *array,
                               const char     *buffer,
                               int             count);
        // Read the specified 'count' two-byte, two's complement integers (in
        // network byte order) from the specified 'buffer' and store these
        // values sequentially in the the specified 'array' (in host byte
        // order).  The behavior is undefined unless '0 <= count'.  Note that
        // each element value will be zero-extended to the size of the array
        // element on this platform.

    static void getArrayInt8(char       *array,
                             const char *buffer,
                             int         count);
    static void getArrayInt8(signed char *array,
                             const char  *buffer,
                             int          count);
    static void getArrayInt8(unsigned char *array,
                             const char    *buffer,
                             int            count);
        // Read the specified 'count' one-byte, two's complement integers from
        // the specified 'buffer' and store these values sequentially in the
        // the specified 'array'.  The behavior is undefined unless
        // '0 <= count'.

                        // *** getting arrays of floating-point values ***

    static void getArrayFloat64(double     *array,
                                const char *buffer,
                                int         count);
        // Read the specified 'count' eight-byte, IEEE double-precision
        // floating-point numbers (in network byte order) from the specified
        // 'buffer' and store these values sequentially in the specified
        // 'array' (in host byte order).  The behavior is undefined unless
        // '0 <= count'.  Note that for non-conforming platforms, this
        // operation may be lossy.

    static void getArrayFloat32(float      *array,
                                const char *buffer,
                                int         count);
        // Read the specified 'count' four-byte, IEEE double-precision
        // floating-point numbers (in network byte order) from the specified
        // 'buffer' and store these values sequentially in the specified
        // 'array' (in host byte order).  The behavior is undefined unless
        // '0 <= count'.  Note that for non-conforming platforms, this
        // operation may be lossy.
};

// ===========================================================================
//                      INLINE FUNCTION DEFINITIONS
// ===========================================================================

                        // -----------------------------
                        // struct bdex_ByteStreamImpUtil
                        // -----------------------------

// CLASS METHODS

                        // *** putting scalar integral values ***

inline
void bdex_ByteStreamImpUtil::putInt64(char                     *buffer,
                                      bsls_PlatformUtil::Int64  value)
{
    BSLS_ASSERT_SAFE(buffer);

    typedef const union Dummy {
        bsls_PlatformUtil::Int64 d_variable;
        char                     d_bytes[1];
    }& T;

#if BSLS_PLATFORMUTIL__IS_LITTLE_ENDIAN
    buffer[0] = T(value).d_bytes[7];
    buffer[1] = T(value).d_bytes[6];
    buffer[2] = T(value).d_bytes[5];
    buffer[3] = T(value).d_bytes[4];
    buffer[4] = T(value).d_bytes[3];
    buffer[5] = T(value).d_bytes[2];
    buffer[6] = T(value).d_bytes[1];
    buffer[7] = T(value).d_bytes[0];
#else
    buffer[0] = T(value).d_bytes[sizeof value - 8];
    buffer[1] = T(value).d_bytes[sizeof value - 7];
    buffer[2] = T(value).d_bytes[sizeof value - 6];
    buffer[3] = T(value).d_bytes[sizeof value - 5];
    buffer[4] = T(value).d_bytes[sizeof value - 4];
    buffer[5] = T(value).d_bytes[sizeof value - 3];
    buffer[6] = T(value).d_bytes[sizeof value - 2];
    buffer[7] = T(value).d_bytes[sizeof value - 1];
#endif
}

inline
void bdex_ByteStreamImpUtil::putInt56(char                     *buffer,
                                      bsls_PlatformUtil::Int64  value)
{
    BSLS_ASSERT_SAFE(buffer);

    typedef const union Dummy {
        bsls_PlatformUtil::Int64 d_variable;
        char                     d_bytes[1];
    }& T;

#if BSLS_PLATFORMUTIL__IS_LITTLE_ENDIAN
    buffer[0] = T(value).d_bytes[6];
    buffer[1] = T(value).d_bytes[5];
    buffer[2] = T(value).d_bytes[4];
    buffer[3] = T(value).d_bytes[3];
    buffer[4] = T(value).d_bytes[2];
    buffer[5] = T(value).d_bytes[1];
    buffer[6] = T(value).d_bytes[0];
#else
    buffer[0] = T(value).d_bytes[sizeof value - 7];
    buffer[1] = T(value).d_bytes[sizeof value - 6];
    buffer[2] = T(value).d_bytes[sizeof value - 5];
    buffer[3] = T(value).d_bytes[sizeof value - 4];
    buffer[4] = T(value).d_bytes[sizeof value - 3];
    buffer[5] = T(value).d_bytes[sizeof value - 2];
    buffer[6] = T(value).d_bytes[sizeof value - 1];
#endif
}

inline
void bdex_ByteStreamImpUtil::putInt48(char                     *buffer,
                                      bsls_PlatformUtil::Int64  value)
{
    BSLS_ASSERT_SAFE(buffer);

    typedef const union Dummy {
        bsls_PlatformUtil::Int64 d_variable;
        char                     d_bytes[1];
    }& T;

#if BSLS_PLATFORMUTIL__IS_LITTLE_ENDIAN
    buffer[0] = T(value).d_bytes[5];
    buffer[1] = T(value).d_bytes[4];
    buffer[2] = T(value).d_bytes[3];
    buffer[3] = T(value).d_bytes[2];
    buffer[4] = T(value).d_bytes[1];
    buffer[5] = T(value).d_bytes[0];
#else
    buffer[0] = T(value).d_bytes[sizeof value - 6];
    buffer[1] = T(value).d_bytes[sizeof value - 5];
    buffer[2] = T(value).d_bytes[sizeof value - 4];
    buffer[3] = T(value).d_bytes[sizeof value - 3];
    buffer[4] = T(value).d_bytes[sizeof value - 2];
    buffer[5] = T(value).d_bytes[sizeof value - 1];
#endif
}

inline
void bdex_ByteStreamImpUtil::putInt40(char                     *buffer,
                                      bsls_PlatformUtil::Int64  value)
{
    BSLS_ASSERT_SAFE(buffer);

    typedef const union Dummy {
        bsls_PlatformUtil::Int64 d_variable;
        char                     d_bytes[1];
    }& T;

#if BSLS_PLATFORMUTIL__IS_LITTLE_ENDIAN
    buffer[0] = T(value).d_bytes[4];
    buffer[1] = T(value).d_bytes[3];
    buffer[2] = T(value).d_bytes[2];
    buffer[3] = T(value).d_bytes[1];
    buffer[4] = T(value).d_bytes[0];
#else
    buffer[0] = T(value).d_bytes[sizeof value - 5];
    buffer[1] = T(value).d_bytes[sizeof value - 4];
    buffer[2] = T(value).d_bytes[sizeof value - 3];
    buffer[3] = T(value).d_bytes[sizeof value - 2];
    buffer[4] = T(value).d_bytes[sizeof value - 1];
#endif
}

inline
void bdex_ByteStreamImpUtil::putInt32(char *buffer, int value)
{
    BSLS_ASSERT_SAFE(buffer);

    typedef const union Dummy {
        int  d_variable;
        char d_bytes[1];
    }& T;

#if BSLS_PLATFORMUTIL__IS_LITTLE_ENDIAN
    buffer[0] = T(value).d_bytes[3];
    buffer[1] = T(value).d_bytes[2];
    buffer[2] = T(value).d_bytes[1];
    buffer[3] = T(value).d_bytes[0];
#else
    buffer[0] = T(value).d_bytes[sizeof value - 4];
    buffer[1] = T(value).d_bytes[sizeof value - 3];
    buffer[2] = T(value).d_bytes[sizeof value - 2];
    buffer[3] = T(value).d_bytes[sizeof value - 1];
#endif
}

inline
void bdex_ByteStreamImpUtil::putInt24(char *buffer, int value)
{
    BSLS_ASSERT_SAFE(buffer);

    typedef const union Dummy {
        int  d_variable;
        char d_bytes[1];
    }& T;

#if BSLS_PLATFORMUTIL__IS_LITTLE_ENDIAN
    buffer[0] = T(value).d_bytes[2];
    buffer[1] = T(value).d_bytes[1];
    buffer[2] = T(value).d_bytes[0];
#else
    buffer[0] = T(value).d_bytes[sizeof value - 3];
    buffer[1] = T(value).d_bytes[sizeof value - 2];
    buffer[2] = T(value).d_bytes[sizeof value - 1];
#endif
}

inline
void bdex_ByteStreamImpUtil::putInt16(char *buffer, int value)
{
    BSLS_ASSERT_SAFE(buffer);

    typedef const union Dummy {
        int  d_variable;
        char d_bytes[1];
    }& T;

#if BSLS_PLATFORMUTIL__IS_LITTLE_ENDIAN
    buffer[0] = T(value).d_bytes[1];
    buffer[1] = T(value).d_bytes[0];
#else
    buffer[0] = T(value).d_bytes[sizeof value - 2];
    buffer[1] = T(value).d_bytes[sizeof value - 1];
#endif
}

inline
void bdex_ByteStreamImpUtil::putInt8(char *buffer, int value)
{
    BSLS_ASSERT_SAFE(buffer);

// The standard code causes compile issues for optimized AIX.  The following
// code seems to fix the problem.
//
//    typedef const union Dummy {
//        int  d_variable;
//        char d_bytes[1];
//    }& T;
//
//#if BSLS_PLATFORMUTIL__IS_LITTLE_ENDIAN
//    buffer[0] = T(value).d_bytes[0];
//#else
//    buffer[0] = T(value).d_bytes[sizeof value - 1];
//#endif

    typedef const union Dummy {
        int  d_variable;
        char d_bytes[1];
    }* T;

#if BSLS_PLATFORMUTIL__IS_LITTLE_ENDIAN
    buffer[0] = ((T)&value)->d_bytes[0];
#else
    buffer[0] = ((T)&value)->d_bytes[sizeof value - 1];
#endif
}
                        // *** putting scalar floating-point values ***

inline
void bdex_ByteStreamImpUtil::putFloat64(char *buffer, double value)
{
    BSLS_ASSERT_SAFE(buffer);

    typedef const union Dummy {
        double d_variable;
        char   d_bytes[1];
    }& T;

#if BSLS_PLATFORMUTIL__IS_LITTLE_ENDIAN
    buffer[0] = T(value).d_bytes[sizeof value - 1];
    buffer[1] = T(value).d_bytes[sizeof value - 2];
    buffer[2] = T(value).d_bytes[sizeof value - 3];
    buffer[3] = T(value).d_bytes[sizeof value - 4];
    buffer[4] = T(value).d_bytes[sizeof value - 5];
    buffer[5] = T(value).d_bytes[sizeof value - 6];
    buffer[6] = T(value).d_bytes[sizeof value - 7];
    buffer[7] = T(value).d_bytes[sizeof value - 8];
#else
    buffer[0] = T(value).d_bytes[0];
    buffer[1] = T(value).d_bytes[1];
    buffer[2] = T(value).d_bytes[2];
    buffer[3] = T(value).d_bytes[3];
    buffer[4] = T(value).d_bytes[4];
    buffer[5] = T(value).d_bytes[5];
    buffer[6] = T(value).d_bytes[6];
    buffer[7] = T(value).d_bytes[7];
#endif
}

inline
void bdex_ByteStreamImpUtil::putFloat32(char *buffer, float value)
{
    BSLS_ASSERT_SAFE(buffer);

    typedef const union Dummy {
        float d_variable;
        char  d_bytes[1];
    }& T;

#if BSLS_PLATFORMUTIL__IS_LITTLE_ENDIAN
    buffer[0] = T(value).d_bytes[sizeof value - 1];
    buffer[1] = T(value).d_bytes[sizeof value - 2];
    buffer[2] = T(value).d_bytes[sizeof value - 3];
    buffer[3] = T(value).d_bytes[sizeof value - 4];
#else
    buffer[0] = T(value).d_bytes[0];
    buffer[1] = T(value).d_bytes[1];
    buffer[2] = T(value).d_bytes[2];
    buffer[3] = T(value).d_bytes[3];
#endif
}

                        // *** getting scalar integral values ***

inline
void bdex_ByteStreamImpUtil::getInt64(bsls_PlatformUtil::Int64 *variable,
                                      const char               *buffer)
{
    BSLS_ASSERT_SAFE(variable);
    BSLS_ASSERT_SAFE(buffer);

    typedef union Dummy {
        bsls_PlatformUtil::Int64 d_variable;
        char                     d_bytes[1];
    }& T;

    if (sizeof *variable > 8) {
        *variable = 0x80 & buffer[0] ? -1 : 0;  // sign extend
    }

#if BSLS_PLATFORMUTIL__IS_LITTLE_ENDIAN
    T(*variable).d_bytes[7] = buffer[0];
    T(*variable).d_bytes[6] = buffer[1];
    T(*variable).d_bytes[5] = buffer[2];
    T(*variable).d_bytes[4] = buffer[3];
    T(*variable).d_bytes[3] = buffer[4];
    T(*variable).d_bytes[2] = buffer[5];
    T(*variable).d_bytes[1] = buffer[6];
    T(*variable).d_bytes[0] = buffer[7];
#else
    T(*variable).d_bytes[sizeof *variable - 8] = buffer[0];
    T(*variable).d_bytes[sizeof *variable - 7] = buffer[1];
    T(*variable).d_bytes[sizeof *variable - 6] = buffer[2];
    T(*variable).d_bytes[sizeof *variable - 5] = buffer[3];
    T(*variable).d_bytes[sizeof *variable - 4] = buffer[4];
    T(*variable).d_bytes[sizeof *variable - 3] = buffer[5];
    T(*variable).d_bytes[sizeof *variable - 2] = buffer[6];
    T(*variable).d_bytes[sizeof *variable - 1] = buffer[7];
#endif
}

inline
void bdex_ByteStreamImpUtil::getUint64(bsls_PlatformUtil::Uint64 *variable,
                                       const char                *buffer)
{
    BSLS_ASSERT_SAFE(variable);
    BSLS_ASSERT_SAFE(buffer);

    typedef union Dummy {
        bsls_PlatformUtil::Uint64 d_variable;
        char                      d_bytes[1];
    }& T;

    if (sizeof *variable > 8) {
        *variable = 0;  // zero extend
    }

#if BSLS_PLATFORMUTIL__IS_LITTLE_ENDIAN
    T(*variable).d_bytes[7] = buffer[0];
    T(*variable).d_bytes[6] = buffer[1];
    T(*variable).d_bytes[5] = buffer[2];
    T(*variable).d_bytes[4] = buffer[3];
    T(*variable).d_bytes[3] = buffer[4];
    T(*variable).d_bytes[2] = buffer[5];
    T(*variable).d_bytes[1] = buffer[6];
    T(*variable).d_bytes[0] = buffer[7];
#else
    T(*variable).d_bytes[sizeof *variable - 8] = buffer[0];
    T(*variable).d_bytes[sizeof *variable - 7] = buffer[1];
    T(*variable).d_bytes[sizeof *variable - 6] = buffer[2];
    T(*variable).d_bytes[sizeof *variable - 5] = buffer[3];
    T(*variable).d_bytes[sizeof *variable - 4] = buffer[4];
    T(*variable).d_bytes[sizeof *variable - 3] = buffer[5];
    T(*variable).d_bytes[sizeof *variable - 2] = buffer[6];
    T(*variable).d_bytes[sizeof *variable - 1] = buffer[7];
#endif
}

inline
void bdex_ByteStreamImpUtil::getInt56(bsls_PlatformUtil::Int64 *variable,
                                      const char               *buffer)
{
    BSLS_ASSERT_SAFE(variable);
    BSLS_ASSERT_SAFE(buffer);

    typedef union Dummy {
        bsls_PlatformUtil::Int64 d_variable;
        char                     d_bytes[1];
    }& T;

    *variable = 0x80 & buffer[0] ? -1 : 0;  // sign extend

#if BSLS_PLATFORMUTIL__IS_LITTLE_ENDIAN
    T(*variable).d_bytes[6] = buffer[0];
    T(*variable).d_bytes[5] = buffer[1];
    T(*variable).d_bytes[4] = buffer[2];
    T(*variable).d_bytes[3] = buffer[3];
    T(*variable).d_bytes[2] = buffer[4];
    T(*variable).d_bytes[1] = buffer[5];
    T(*variable).d_bytes[0] = buffer[6];
#else
    T(*variable).d_bytes[sizeof *variable - 7] = buffer[0];
    T(*variable).d_bytes[sizeof *variable - 6] = buffer[1];
    T(*variable).d_bytes[sizeof *variable - 5] = buffer[2];
    T(*variable).d_bytes[sizeof *variable - 4] = buffer[3];
    T(*variable).d_bytes[sizeof *variable - 3] = buffer[4];
    T(*variable).d_bytes[sizeof *variable - 2] = buffer[5];
    T(*variable).d_bytes[sizeof *variable - 1] = buffer[6];
#endif
}

inline
void bdex_ByteStreamImpUtil::getUint56(bsls_PlatformUtil::Uint64 *variable,
                                       const char                *buffer)
{
    BSLS_ASSERT_SAFE(variable);
    BSLS_ASSERT_SAFE(buffer);

    typedef union Dummy {
        bsls_PlatformUtil::Uint64 d_variable;
        char                      d_bytes[1];
    }& T;

    *variable = 0;  // zero extend

#if BSLS_PLATFORMUTIL__IS_LITTLE_ENDIAN
    T(*variable).d_bytes[6] = buffer[0];
    T(*variable).d_bytes[5] = buffer[1];
    T(*variable).d_bytes[4] = buffer[2];
    T(*variable).d_bytes[3] = buffer[3];
    T(*variable).d_bytes[2] = buffer[4];
    T(*variable).d_bytes[1] = buffer[5];
    T(*variable).d_bytes[0] = buffer[6];
#else
    T(*variable).d_bytes[sizeof *variable - 7] = buffer[0];
    T(*variable).d_bytes[sizeof *variable - 6] = buffer[1];
    T(*variable).d_bytes[sizeof *variable - 5] = buffer[2];
    T(*variable).d_bytes[sizeof *variable - 4] = buffer[3];
    T(*variable).d_bytes[sizeof *variable - 3] = buffer[4];
    T(*variable).d_bytes[sizeof *variable - 2] = buffer[5];
    T(*variable).d_bytes[sizeof *variable - 1] = buffer[6];
#endif
}

inline
void bdex_ByteStreamImpUtil::getInt48(bsls_PlatformUtil::Int64 *variable,
                                      const char               *buffer)
{
    BSLS_ASSERT_SAFE(variable);
    BSLS_ASSERT_SAFE(buffer);

    typedef union Dummy {
        bsls_PlatformUtil::Int64 d_variable;
        char                     d_bytes[1];
    }& T;

    *variable = 0x80 & buffer[0] ? -1 : 0;  // sign extend

#if BSLS_PLATFORMUTIL__IS_LITTLE_ENDIAN
    T(*variable).d_bytes[5] = buffer[0];
    T(*variable).d_bytes[4] = buffer[1];
    T(*variable).d_bytes[3] = buffer[2];
    T(*variable).d_bytes[2] = buffer[3];
    T(*variable).d_bytes[1] = buffer[4];
    T(*variable).d_bytes[0] = buffer[5];
#else
    T(*variable).d_bytes[sizeof *variable - 6] = buffer[0];
    T(*variable).d_bytes[sizeof *variable - 5] = buffer[1];
    T(*variable).d_bytes[sizeof *variable - 4] = buffer[2];
    T(*variable).d_bytes[sizeof *variable - 3] = buffer[3];
    T(*variable).d_bytes[sizeof *variable - 2] = buffer[4];
    T(*variable).d_bytes[sizeof *variable - 1] = buffer[5];
#endif
}

inline
void bdex_ByteStreamImpUtil::getUint48(bsls_PlatformUtil::Uint64 *variable,
                                       const char                *buffer)
{
    BSLS_ASSERT_SAFE(variable);
    BSLS_ASSERT_SAFE(buffer);

    typedef union Dummy {
        bsls_PlatformUtil::Uint64 d_variable;
        char                      d_bytes[1];
    }& T;

    *variable = 0;  // zero extend

#if BSLS_PLATFORMUTIL__IS_LITTLE_ENDIAN
    T(*variable).d_bytes[5] = buffer[0];
    T(*variable).d_bytes[4] = buffer[1];
    T(*variable).d_bytes[3] = buffer[2];
    T(*variable).d_bytes[2] = buffer[3];
    T(*variable).d_bytes[1] = buffer[4];
    T(*variable).d_bytes[0] = buffer[5];
#else
    T(*variable).d_bytes[sizeof *variable - 6] = buffer[0];
    T(*variable).d_bytes[sizeof *variable - 5] = buffer[1];
    T(*variable).d_bytes[sizeof *variable - 4] = buffer[2];
    T(*variable).d_bytes[sizeof *variable - 3] = buffer[3];
    T(*variable).d_bytes[sizeof *variable - 2] = buffer[4];
    T(*variable).d_bytes[sizeof *variable - 1] = buffer[5];
#endif
}

inline
void bdex_ByteStreamImpUtil::getInt40(bsls_PlatformUtil::Int64 *variable,
                                      const char               *buffer)
{
    BSLS_ASSERT_SAFE(variable);
    BSLS_ASSERT_SAFE(buffer);

    typedef union Dummy {
        bsls_PlatformUtil::Int64 d_variable;
        char                     d_bytes[1];
    }& T;

    *variable = 0x80 & buffer[0] ? -1 : 0;  // sign extend

#if BSLS_PLATFORMUTIL__IS_LITTLE_ENDIAN
    T(*variable).d_bytes[4] = buffer[0];
    T(*variable).d_bytes[3] = buffer[1];
    T(*variable).d_bytes[2] = buffer[2];
    T(*variable).d_bytes[1] = buffer[3];
    T(*variable).d_bytes[0] = buffer[4];
#else
    T(*variable).d_bytes[sizeof *variable - 5] = buffer[0];
    T(*variable).d_bytes[sizeof *variable - 4] = buffer[1];
    T(*variable).d_bytes[sizeof *variable - 3] = buffer[2];
    T(*variable).d_bytes[sizeof *variable - 2] = buffer[3];
    T(*variable).d_bytes[sizeof *variable - 1] = buffer[4];
#endif
}

inline
void bdex_ByteStreamImpUtil::getUint40(bsls_PlatformUtil::Uint64 *variable,
                                       const char                *buffer)
{
    BSLS_ASSERT_SAFE(variable);
    BSLS_ASSERT_SAFE(buffer);

    typedef union Dummy {
        bsls_PlatformUtil::Uint64 d_variable;
        char                      d_bytes[1];
    }& T;

    *variable = 0;  // zero extend

#if BSLS_PLATFORMUTIL__IS_LITTLE_ENDIAN
    T(*variable).d_bytes[4] = buffer[0];
    T(*variable).d_bytes[3] = buffer[1];
    T(*variable).d_bytes[2] = buffer[2];
    T(*variable).d_bytes[1] = buffer[3];
    T(*variable).d_bytes[0] = buffer[4];
#else
    T(*variable).d_bytes[sizeof *variable - 5] = buffer[0];
    T(*variable).d_bytes[sizeof *variable - 4] = buffer[1];
    T(*variable).d_bytes[sizeof *variable - 3] = buffer[2];
    T(*variable).d_bytes[sizeof *variable - 2] = buffer[3];
    T(*variable).d_bytes[sizeof *variable - 1] = buffer[4];
#endif
}

inline
void bdex_ByteStreamImpUtil::getInt32(int *variable, const char *buffer)
{
    BSLS_ASSERT_SAFE(variable);
    BSLS_ASSERT_SAFE(buffer);

    typedef union Dummy {
        int  d_variable;
        char d_bytes[1];
    }& T;

    if (sizeof *variable > 4) {
        *variable = 0x80 & buffer[0] ? -1 : 0;  // sign extend
    }

#if BSLS_PLATFORMUTIL__IS_LITTLE_ENDIAN
    T(*variable).d_bytes[3] = buffer[0];
    T(*variable).d_bytes[2] = buffer[1];
    T(*variable).d_bytes[1] = buffer[2];
    T(*variable).d_bytes[0] = buffer[3];
#else
    T(*variable).d_bytes[sizeof *variable - 4] = buffer[0];
    T(*variable).d_bytes[sizeof *variable - 3] = buffer[1];
    T(*variable).d_bytes[sizeof *variable - 2] = buffer[2];
    T(*variable).d_bytes[sizeof *variable - 1] = buffer[3];
#endif
}

inline
void bdex_ByteStreamImpUtil::getUint32(unsigned int *variable,
                                       const char   *buffer)
{
    BSLS_ASSERT_SAFE(variable);
    BSLS_ASSERT_SAFE(buffer);

    typedef union Dummy {
        int  d_variable;
        char d_bytes[1];
    }& T;

    if (sizeof *variable > 4) {
        *variable = 0;  // zero extend
    }

#if BSLS_PLATFORMUTIL__IS_LITTLE_ENDIAN
    T(*variable).d_bytes[3] = buffer[0];
    T(*variable).d_bytes[2] = buffer[1];
    T(*variable).d_bytes[1] = buffer[2];
    T(*variable).d_bytes[0] = buffer[3];
#else
    T(*variable).d_bytes[sizeof *variable - 4] = buffer[0];
    T(*variable).d_bytes[sizeof *variable - 3] = buffer[1];
    T(*variable).d_bytes[sizeof *variable - 2] = buffer[2];
    T(*variable).d_bytes[sizeof *variable - 1] = buffer[3];
#endif
}

inline
void bdex_ByteStreamImpUtil::getInt24(int *variable, const char *buffer)
{
    BSLS_ASSERT_SAFE(variable);
    BSLS_ASSERT_SAFE(buffer);

    typedef union Dummy {
        int  d_variable;
        char d_bytes[1];
    }& T;

    *variable = 0x80 & buffer[0] ? -1 : 0;  // sign extend

#if BSLS_PLATFORMUTIL__IS_LITTLE_ENDIAN
    T(*variable).d_bytes[2] = buffer[0];
    T(*variable).d_bytes[1] = buffer[1];
    T(*variable).d_bytes[0] = buffer[2];
#else
    T(*variable).d_bytes[sizeof *variable - 3] = buffer[0];
    T(*variable).d_bytes[sizeof *variable - 2] = buffer[1];
    T(*variable).d_bytes[sizeof *variable - 1] = buffer[2];
#endif
}

inline
void bdex_ByteStreamImpUtil::getUint24(unsigned int *variable,
                                       const char   *buffer)
{
    BSLS_ASSERT_SAFE(variable);
    BSLS_ASSERT_SAFE(buffer);

    typedef union Dummy {
        int  d_variable;
        char d_bytes[1];
    }& T;

    *variable = 0;  // zero extend

#if BSLS_PLATFORMUTIL__IS_LITTLE_ENDIAN
    T(*variable).d_bytes[2] = buffer[0];
    T(*variable).d_bytes[1] = buffer[1];
    T(*variable).d_bytes[0] = buffer[2];
#else
    T(*variable).d_bytes[sizeof *variable - 3] = buffer[0];
    T(*variable).d_bytes[sizeof *variable - 2] = buffer[1];
    T(*variable).d_bytes[sizeof *variable - 1] = buffer[2];
#endif
}

inline
void bdex_ByteStreamImpUtil::getInt16(short *variable, const char *buffer)
{
    BSLS_ASSERT_SAFE(variable);
    BSLS_ASSERT_SAFE(buffer);

    typedef union Dummy {
        short d_variable;
        char  d_bytes[1];
    }& T;

    if (sizeof *variable > 2) {
        *variable = (short)(0x80 & buffer[0] ? -1 : 0);  // sign extend
    }

#if BSLS_PLATFORMUTIL__IS_LITTLE_ENDIAN
    T(*variable).d_bytes[1] = buffer[0];
    T(*variable).d_bytes[0] = buffer[1];
#else
    T(*variable).d_bytes[sizeof *variable - 2] = buffer[0];
    T(*variable).d_bytes[sizeof *variable - 1] = buffer[1];
#endif
}

inline
void bdex_ByteStreamImpUtil::getUint16(unsigned short *variable,
                                       const char     *buffer)
{
    BSLS_ASSERT_SAFE(variable);
    BSLS_ASSERT_SAFE(buffer);

    typedef union Dummy {
        short d_variable;
        char  d_bytes[1];
    }& T;

    if (sizeof *variable > 2) {
        *variable = 0;  // zero extend
    }

#if BSLS_PLATFORMUTIL__IS_LITTLE_ENDIAN
    T(*variable).d_bytes[1] = buffer[0];
    T(*variable).d_bytes[0] = buffer[1];
#else
    T(*variable).d_bytes[sizeof *variable - 2] = buffer[0];
    T(*variable).d_bytes[sizeof *variable - 1] = buffer[1];
#endif
}

inline
void bdex_ByteStreamImpUtil::getInt8(char *variable, const char *buffer)
{
    BSLS_ASSERT_SAFE(variable);
    BSLS_ASSERT_SAFE(buffer);

    *variable = buffer[0];
}

inline
void bdex_ByteStreamImpUtil::getInt8(signed char *variable, const char *buffer)
{
    BSLS_ASSERT_SAFE(variable);
    BSLS_ASSERT_SAFE(buffer);

    getInt8((char *)variable, buffer);
}

inline
void bdex_ByteStreamImpUtil::getInt8(unsigned char *variable,
                                     const char    *buffer)
{
    BSLS_ASSERT_SAFE(variable);
    BSLS_ASSERT_SAFE(buffer);

    getInt8((char *)variable, buffer);
}

                        // *** getting scalar floating-point values ***

inline
void bdex_ByteStreamImpUtil::getFloat64(double *variable, const char *buffer)
{
    BSLS_ASSERT_SAFE(variable);
    BSLS_ASSERT_SAFE(buffer);

    typedef union Dummy {
        double d_variable;
        char   d_bytes[1];
    }& T;

    if (sizeof *variable > 8) {
        *variable = 0;  // zero fill mantissa
    }

#if BSLS_PLATFORMUTIL__IS_LITTLE_ENDIAN
    T(*variable).d_bytes[sizeof *variable - 1] = buffer[0];
    T(*variable).d_bytes[sizeof *variable - 2] = buffer[1];
    T(*variable).d_bytes[sizeof *variable - 3] = buffer[2];
    T(*variable).d_bytes[sizeof *variable - 4] = buffer[3];
    T(*variable).d_bytes[sizeof *variable - 5] = buffer[4];
    T(*variable).d_bytes[sizeof *variable - 6] = buffer[5];
    T(*variable).d_bytes[sizeof *variable - 7] = buffer[6];
    T(*variable).d_bytes[sizeof *variable - 8] = buffer[7];
#else
    T(*variable).d_bytes[0] = buffer[0];
    T(*variable).d_bytes[1] = buffer[1];
    T(*variable).d_bytes[2] = buffer[2];
    T(*variable).d_bytes[3] = buffer[3];
    T(*variable).d_bytes[4] = buffer[4];
    T(*variable).d_bytes[5] = buffer[5];
    T(*variable).d_bytes[6] = buffer[6];
    T(*variable).d_bytes[7] = buffer[7];
#endif
}

inline
void bdex_ByteStreamImpUtil::getFloat32(float *variable, const char *buffer)
{
    BSLS_ASSERT_SAFE(variable);
    BSLS_ASSERT_SAFE(buffer);

    typedef union Dummy {
        float d_variable;
        char  d_bytes[1];
    }& T;

    if (sizeof *variable > 4) {
        *variable = 0;  // zero fill mantissa
    }

#if BSLS_PLATFORMUTIL__IS_LITTLE_ENDIAN
    T(*variable).d_bytes[sizeof *variable - 1] = buffer[0];
    T(*variable).d_bytes[sizeof *variable - 2] = buffer[1];
    T(*variable).d_bytes[sizeof *variable - 3] = buffer[2];
    T(*variable).d_bytes[sizeof *variable - 4] = buffer[3];
#else
    T(*variable).d_bytes[0] = buffer[0];
    T(*variable).d_bytes[1] = buffer[1];
    T(*variable).d_bytes[2] = buffer[2];
    T(*variable).d_bytes[3] = buffer[3];
#endif
}

                        // *** putting arrays of integral values ***

inline
void bdex_ByteStreamImpUtil::putArrayInt16(char                 *buffer,
                                           const unsigned short *array,
                                           int                   count)
{
    BSLS_ASSERT_SAFE(buffer);
    BSLS_ASSERT_SAFE(array);
    BSLS_ASSERT_SAFE(0 <= count);

    putArrayInt16(buffer, (const short *)array, count);
}

inline
void bdex_ByteStreamImpUtil::putArrayInt8(char       *buffer,
                                          const char *array,
                                          int         count)
{
    BSLS_ASSERT_SAFE(buffer);
    BSLS_ASSERT_SAFE(array);
    BSLS_ASSERT_SAFE(0 <= count);

    std::memcpy(buffer, array, count);
}

inline
void bdex_ByteStreamImpUtil::putArrayInt8(char              *buffer,
                                          const signed char *array,
                                          int                count)
{
    BSLS_ASSERT_SAFE(buffer);
    BSLS_ASSERT_SAFE(array);
    BSLS_ASSERT_SAFE(0 <= count);

    putArrayInt8(buffer, (const char *)array, count);
}

inline
void bdex_ByteStreamImpUtil::putArrayInt8(char                *buffer,
                                          const unsigned char *array,
                                          int                  count)
{
    BSLS_ASSERT_SAFE(buffer);
    BSLS_ASSERT_SAFE(array);
    BSLS_ASSERT_SAFE(0 <= count);

    putArrayInt8(buffer, (const char *)array, count);
}

                        // *** getting arrays of integral values ***

inline
void bdex_ByteStreamImpUtil::getArrayInt8(char       *array,
                                          const char *buffer,
                                          int         count)
{
    BSLS_ASSERT_SAFE(buffer);
    BSLS_ASSERT_SAFE(array);
    BSLS_ASSERT_SAFE(0 <= count);

    std::memcpy(array, buffer, count);
}

inline
void bdex_ByteStreamImpUtil::getArrayInt8(signed char *array,
                                          const char  *buffer,
                                          int          count)
{
    BSLS_ASSERT_SAFE(buffer);
    BSLS_ASSERT_SAFE(array);
    BSLS_ASSERT_SAFE(0 <= count);

    getArrayInt8((char *)array, buffer, count);
}

inline
void bdex_ByteStreamImpUtil::getArrayInt8(unsigned char *array,
                                          const char    *buffer,
                                          int            count)
{
    BSLS_ASSERT_SAFE(buffer);
    BSLS_ASSERT_SAFE(array);
    BSLS_ASSERT_SAFE(0 <= count);

    getArrayInt8((char *)array, buffer, count);
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
