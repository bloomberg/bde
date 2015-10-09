// bslx_marshallingutil.h                                             -*-C++-*-
#ifndef INCLUDED_BSLX_MARSHALLINGUTIL
#define INCLUDED_BSLX_MARSHALLINGUTIL

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Support platform-independent marshalling of fundamental types.
//
//@CLASSES:
//   bslx::MarshallingUtil: namespace for put/get marshalling functions
//
//@SEE_ALSO: bslx_byteinstream, bslx_byteoutstream
//
//@DESCRIPTION: This component provides a byte-array-based implementation,
// 'bslx::MarshallingUtil', for a suite of marshalling functions used to
// convert values (and arrays of values) of the following fundamental integer
// and floating-point types:
//..
//      C++ TYPE          REQUIRED CONTENT OF ANY PLATFORM-NEUTRAL FORMAT
//      --------          -----------------------------------------------
//      Int64             least significant 64 bits (signed)
//      Uint64            least significant 64 bits (unsigned)
//      int               least significant 32 bits (signed)
//      unsigned int      least significant 32 bits (unsigned)
//      short             least significant 16 bits (signed)
//      unsigned short    least significant 16 bits (unsigned)
//      char              least significant  8 bits (platform-dependent)
//      signed char       least significant  8 bits (signed)
//      unsigned char     least significant  8 bits (unsigned)
//      double            IEEE standard 8-byte floating-point value
//      float             IEEE standard 4-byte floating-point value
//..
// In addition to basic marshalling functions, where each marshalled instance
// of a fundamental type occupies the same number of bytes in the stream
// (regardless of its value), this component provides an interface for
// efficient marshalling of integer types.  In particular, 64-bit values can be
// streamed as 40-, 48-, 56-, or 64-bit values, and 32-bit values can be
// streamed as 24- or 32-bit values.  Marshalled integers are written and
// assumed to be in two's complement, big-endian format (i.e., network byte
// order).  Floating-point formats are described below.
//
///Note on Function Naming and Interface
///-------------------------------------
// The names and interfaces of the functions of 'bslx::MarshallingUtil' follow
// a systematic fashion explained below.  This makes it easier to guess the
// name and signature of the intended function.  In what follows, 'buffer' is
// always of type 'char *' or 'const char *' depending on whether it is used as
// an input or an output, and 'variable' and 'value' are of a type that depends
// on the name of the function and intended width, with 'variable' used as an
// output, while 'value' is used as an input.
//
// Here are the 'get...' functions for integral and floating-point scalar
// types:
//..
//   Name                           Type of 'variable'           Notes
//   ----                           ------------------           -----
//   getIntNN(variable, buffer)     bsls::Types::Int64 *         NN=64,56,48,40
//                                  int *                        NN=32,24
//                                  short *                      NN=16
//                                  char *                       NN=8
//                                  signed char *                NN=8
//                                  unsigned char *              NN=8
//
//   getUintNN(variable, buffer)    bsls::Types::Uint64 *        NN=64,56,48,40
//                                  unsigned int *               NN=32,24
//                                  unsigned short *             NN=16
//
//   getFloatNN(variable, buffer)   double *                     NN=64
//                                  float *                      NN=32
//..
// Here are the 'put...' functions for scalar types.  Note that there is no
// 'putUintNN' since 'putIntNN' applies equally to unsigned 'NN'-bit values
// (through a conversion to a signed value):
//..
//   Name                           Type of 'value'              Notes
//   ----                           ---------------              -----
//   putIntNN(buffer, value)        bsls::Types::Int64           NN=64,56,48,40
//                                  int                          NN=32,24,16,8
//
//   putFloatNN(buffer, value)      double                       NN=64
//                                  float                        NN=32
//..
// Here are the 'getArray...' functions for integral and floating-point scalar
// array types:
//..
//   Name                           Type of 'variables'          Notes
//   ----                           ----------------             -----
//   getArrayIntNN(variables,       bsls::Types::Int64 *         NN=64,56,48,40
//                 buffer,          int *                        NN=32,24
//                 numVariables)    short *                      NN=16
//                                  char *                       NN=8
//                                  signed char *                NN=8
//                                  unsigned char *              NN=8
//
//   getArrayUintNN(variables,      bsls::Types::Uint64 *        NN=64,56,48,40
//                  buffer,         unsigned int *               NN=32,24
//                  numVariables)   unsigned short *             NN=16
//
//   getArrayFloatNN(variables,     double *                     NN=64
//                   buffer,        float *                      NN=32
//                   numVariables)
//..
// Finally, the 'putArray...' functions follow.  Note that this time there is
// an overload for unsigned types, but that the function name is still
// 'putArrayInt...' for arrays of both signed and unsigned integrals:
//..
//   Name                           Type of 'values'             Notes
//   ----                           ---------------              -----
//   putArrayIntNN(buffer,          const bsls::Types::Int64 *   NN=64,56,48,40
//                 values,          const bsls::Types::Uint64 *  NN=64,56,48,40
//                 numValues)       const int *                  NN=32,24
//                                  const unsigned int *         NN=32,24
//                                  const short *                NN=16
//                                  const unsigned short *       NN=16
//                                  const char *                 NN=8
//                                  const signed char *          NN=8
//                                  const unsigned char *        NN=8
//
//   putArrayFloatNN(buffer,        const double *               NN=64
//                   values,        const float *                NN=32
//                   numValues)
//..
//
///IEEE 754 Double-Precision Format
///--------------------------------
// A 'double' is assumed to be *at* *least* 64 bits in size.  The externalized
// byte representation of a 64-bit floating-point value is defined to conform
// to the IEEE double-precision format illustrated below.  If the native
// representation of a 64-bit floating-point value does not match this format,
// a conversion process to and from this format is performed.  This conversion
// may (of course) be lossy:
//..
//  sign bit    11-bit exponent             52-bit significand
//    /        /                           /
//  +-+-----------+----------------------------------------------------+
//  |s|e10......e0|m51...............................................m0|
//  +-+-----------+----------------------------------------------------+
//  LSB                                                              MSB
//..
//
///IEEE 754 Single-Precision Format
///--------------------------------
// A 'float' is assumed to be *at* *least* 32 bits in size.  The externalized
// byte representation of a 32-bit floating-point value is defined to conform
// to the IEEE single-precision format illustrated below.  If the native
// representation of a 32-bit floating-point value does not match this format,
// a conversion process to and from this format is performed.  This conversion
// may (of course) be lossy:
//..
//  sign bit    8-bit exponent        23-bit significand
//     /       /                     /
//    +-+--------+-----------------------+
//    |s|e7....e0|m22..................m0|
//    +-+--------+-----------------------+
//    LSB                              MSB
//..
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Round-Trip Marshalling
///- - - - - - - - - - - - - - - - -
// The 'bslx::MarshallingUtil' component can be used stand-alone to marshal a
// platform-neutral representation of fundamental data and arrays of
// fundamental data to and from a buffer.  In this example, the round-trip
// marshalling of an 'int' and an array of 'int' values will be demonstrated.
// First, declare the buffer and the data to be marshalled:
//..
//  char buffer[32];
//  int  value = 17;
//  int  values[] = { 1, 2, 3 };
//..
// Then, marshal all data into the 'buffer':
//..
//  bslx::MarshallingUtil::putInt32(buffer + 0, value);
//  bslx::MarshallingUtil::putArrayInt32(buffer + 4, values, 3);
//..
// Next, declare variables to hold the values to be extracted from the
// 'buffer':
//..
//  int newValue = 0;
//  int newValues[] = { 0, 0, 0 };
//..
// Finally, marshal the data from the 'buffer' to these variables and confirm
// the round-trip marshalling was successful:
//..
//  bslx::MarshallingUtil::getInt32(&newValue, buffer + 0);
//  bslx::MarshallingUtil::getArrayInt32(newValues, buffer + 4, 3);
//
//  assert(newValue     == value);
//  assert(newValues[0] == values[0]);
//  assert(newValues[1] == values[1]);
//  assert(newValues[2] == values[2]);
//..

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORM
#include <bsls_platform.h>
#endif

#ifndef INCLUDED_BSLS_TYPES
#include <bsls_types.h>
#endif

#ifndef INCLUDED_BSL_CSTRING
#include <bsl_cstring.h>           // for 'bsl::memcpy'
#endif

namespace BloombergLP {
namespace bslx {

                         // ======================
                         // struct MarshallingUtil
                         // ======================

struct MarshallingUtil {
    // This 'struct' provides a namespace for a suite of functions that
    // facilitate the marshalling of values, and C-style arrays of values, of
    // the fundamental integral and floating-point types in a data-independent,
    // platform-neutral representation.

    // TYPES

    enum {
        // Enumerate the platform-independent sizes (in bytes) of data types in
        // wire format.  Note that the wire format size may differ from the
        // size in memory.

        k_SIZEOF_INT64   = 8,
        k_SIZEOF_INT56   = 7,
        k_SIZEOF_INT48   = 6,
        k_SIZEOF_INT40   = 5,
        k_SIZEOF_INT32   = 4,
        k_SIZEOF_INT24   = 3,
        k_SIZEOF_INT16   = 2,
        k_SIZEOF_INT8    = 1,
        k_SIZEOF_FLOAT64 = 8,
        k_SIZEOF_FLOAT32 = 4
    };

    // CLASS METHODS

                        // *** put scalar integral values ***

    static void putInt64(char *buffer, bsls::Types::Int64 value);
        // Load into the specified 'buffer' the eight-byte, two's complement
        // integer (in network byte order) comprised of the least-significant
        // eight bytes of the specified 'value' (in host byte order).  The
        // behavior is undefined unless 'buffer' has sufficient capacity.  Note
        // that this function applies equally to unsigned 64-bit values.

    static void putInt56(char *buffer, bsls::Types::Int64 value);
        // Load into the specified 'buffer' the seven-byte, two's complement
        // integer (in network byte order) comprised of the least-significant
        // seven bytes of the specified 'value' (in host byte order).  The
        // behavior is undefined unless 'buffer' has sufficient capacity.  Note
        // that this function applies equally to unsigned 64-bit values.

    static void putInt48(char *buffer, bsls::Types::Int64 value);
        // Load into the specified 'buffer' the six-byte, two's complement
        // integer (in network byte order) comprised of the least-significant
        // six bytes of the specified 'value' (in host byte order).  The
        // behavior is undefined unless 'buffer' has sufficient capacity.  Note
        // that this function applies equally to unsigned 64-bit values.

    static void putInt40(char *buffer, bsls::Types::Int64 value);
        // Load into the specified 'buffer' the five-byte, two's complement
        // integer (in network byte order) comprised of the least-significant
        // five bytes of the specified 'value' (in host byte order).  The
        // behavior is undefined unless 'buffer' has sufficient capacity.  Note
        // that this function applies equally to unsigned 64-bit values.

    static void putInt32(char *buffer, int value);
        // Load into the specified 'buffer' the four-byte, two's complement
        // integer (in network byte order) comprised of the least-significant
        // four bytes of the specified 'value' (in host byte order).  The
        // behavior is undefined unless 'buffer' has sufficient capacity.  Note
        // that this function applies equally to unsigned 32-bit values, and
        // signed and unsigned 16- and 8-bit values.

    static void putInt24(char *buffer, int value);
        // Load into the specified 'buffer' the three-byte, two's complement
        // integer (in network byte order) comprised of the least-significant
        // three bytes of the specified 'value' (in host byte order).  The
        // behavior is undefined unless 'buffer' has sufficient capacity.  Note
        // that this function applies equally to unsigned 32-bit values, and
        // signed and unsigned 16- and 8-bit values.

    static void putInt16(char *buffer, int value);
        // Load into the specified 'buffer' the two-byte, two's complement
        // integer (in network byte order) comprised of the least-significant
        // two bytes of the specified 'value' (in host byte order).  The
        // behavior is undefined unless 'buffer' has sufficient capacity.  Note
        // that this function applies equally to unsigned 32-bit values, and
        // signed and unsigned 16- and 8-bit values.

    static void putInt8(char *buffer, int value);
        // Load into the specified 'buffer' the one-byte, two's complement
        // integer comprised of the least-significant one byte of the specified
        // 'value'.  The behavior is undefined unless 'buffer' has sufficient
        // capacity.  Note that this function applies equally to unsigned
        // 32-bit values, and signed and unsigned 16- and 8-bit values.

                        // *** put scalar floating-point values ***

    static void putFloat64(char *buffer, double value);
        // Load into the specified 'buffer' the eight-byte IEEE
        // double-precision floating-point number (in network byte order)
        // comprised of the most-significant eight bytes of the specified
        // 'value' (in host byte order).  The behavior is undefined unless
        // 'buffer' has sufficient capacity.  Note that for non-conforming
        // platforms, this operation may be lossy.

    static void putFloat32(char *buffer, float value);
        // Load into the specified 'buffer' the four-byte IEEE single-precision
        // floating-point number (in network byte order) comprised of the
        // most-significant four bytes of the specified 'value' (in host byte
        // order).  The behavior is undefined unless 'buffer' has sufficient
        // capacity.  Note that for non-conforming platforms, this operation
        // may be lossy.

                        // *** get scalar integral values ***

    static void getInt64(bsls::Types::Int64 *variable,
                         const char         *buffer);
        // Load into the specified 'variable' the eight-byte, two's complement
        // integer (in host byte order) comprised of the initial eight bytes of
        // the specified 'buffer' (in network byte order).  The behavior is
        // undefined unless 'buffer' has sufficient contents.  Note that the
        // value will be sign-extended.

    static void getUint64(bsls::Types::Uint64 *variable,
                          const char          *buffer);
        // Load into the specified 'variable' the eight-byte, two's complement
        // unsigned integer (in host byte order) comprised of the initial eight
        // bytes of the specified 'buffer' (in network byte order).  The
        // behavior is undefined unless 'buffer' has sufficient contents.  Note
        // that the value will be zero-extended.

    static void getInt56(bsls::Types::Int64 *variable,
                         const char         *buffer);
        // Load into the specified 'variable' the seven-byte, two's complement
        // integer (in host byte order) comprised of the initial seven bytes of
        // the specified 'buffer' (in network byte order).  The behavior is
        // undefined unless 'buffer' has sufficient contents.  Note that the
        // value will be sign-extended.

    static void getUint56(bsls::Types::Uint64 *variable,
                          const char          *buffer);
        // Load into the specified 'variable' the seven-byte, two's complement
        // unsigned integer (in host byte order) comprised of the initial seven
        // bytes of the specified 'buffer' (in network byte order).  The
        // behavior is undefined unless 'buffer' has sufficient contents.  Note
        // that the value will be zero-extended.

    static void getInt48(bsls::Types::Int64 *variable,
                         const char         *buffer);
        // Load into the specified 'variable' the six-byte, two's complement
        // integer (in host byte order) comprised of the initial six bytes of
        // the specified 'buffer' (in network byte order).  The behavior is
        // undefined unless 'buffer' has sufficient contents.  Note that the
        // value will be sign-extended.

    static void getUint48(bsls::Types::Uint64 *variable,
                          const char          *buffer);
        // Load into the specified 'variable' the six-byte, two's complement
        // unsigned integer (in host byte order) comprised of the initial six
        // bytes of the specified 'buffer' (in network byte order).  The
        // behavior is undefined unless 'buffer' has sufficient contents.  Note
        // that the value will be zero-extended.

    static void getInt40(bsls::Types::Int64 *variable,
                         const char         *buffer);
        // Load into the specified 'variable' the five-byte, two's complement
        // integer (in host byte order) comprised of the initial five bytes of
        // the specified 'buffer' (in network byte order).  The behavior is
        // undefined unless 'buffer' has sufficient contents.  Note that the
        // value will be sign-extended.

    static void getUint40(bsls::Types::Uint64 *variable,
                          const char          *buffer);
        // Load into the specified 'variable' the five-byte, two's complement
        // unsigned integer (in host byte order) comprised of the initial five
        // bytes of the specified 'buffer' (in network byte order).  The
        // behavior is undefined unless 'buffer' has sufficient contents.  Note
        // that the value will be zero-extended.

    static void getInt32(int *variable, const char *buffer);
        // Load into the specified 'variable' the four-byte, two's complement
        // integer (in host byte order) comprised of the initial four bytes of
        // the specified 'buffer' (in network byte order).  The behavior is
        // undefined unless 'buffer' has sufficient contents.  Note that the
        // value will be sign-extended.

    static void getUint32(unsigned int *variable, const char *buffer);
        // Load into the specified 'variable' the four-byte, two's complement
        // unsigned integer (in host byte order) comprised of the initial four
        // bytes of the specified 'buffer' (in network byte order).  The
        // behavior is undefined unless 'buffer' has sufficient contents.  Note
        // that the value will be zero-extended.

    static void getInt24(int *variable, const char *buffer);
        // Load into the specified 'variable' the three-byte, two's complement
        // integer (in host byte order) comprised of the initial three bytes of
        // the specified 'buffer' (in network byte order).  The behavior is
        // undefined unless 'buffer' has sufficient contents.  Note that the
        // value will be sign-extended.

    static void getUint24(unsigned int *variable, const char *buffer);
        // Load into the specified 'variable' the three-byte, two's complement
        // unsigned integer (in host byte order) comprised of the initial three
        // bytes of the specified 'buffer' (in network byte order).  The
        // behavior is undefined unless 'buffer' has sufficient contents.  Note
        // that the value will be zero-extended.

    static void getInt16(short *variable, const char *buffer);
        // Load into the specified 'variable' the two-byte, two's complement
        // integer (in host byte order) comprised of the initial two bytes of
        // the specified 'buffer' (in network byte order).  The behavior is
        // undefined unless 'buffer' has sufficient contents.  Note that the
        // value will be sign-extended.

    static void getUint16(unsigned short *variable, const char *buffer);
        // Load into the specified 'variable' the two-byte, two's complement
        // unsigned integer (in host byte order) comprised of the initial two
        // bytes of the specified 'buffer' (in network byte order).  The
        // behavior is undefined unless 'buffer' has sufficient contents.  Note
        // that the value will be zero-extended.

    static void getInt8(char          *variable, const char *buffer);
    static void getInt8(signed char   *variable, const char *buffer);
    static void getInt8(unsigned char *variable, const char *buffer);
        // Load into the specified 'variable' the one-byte, two's complement
        // integer comprised of the initial one byte of the specified 'buffer'.
        // The behavior is undefined unless 'buffer' has sufficient contents.

                        // *** get scalar floating-point values ***

    static void getFloat64(double *variable, const char *buffer);
        // Load into the specified 'variable' the eight-byte IEEE
        // double-precision floating-point number (in host byte order)
        // comprised of the initial eight bytes of the specified 'buffer' (in
        // network byte order).  The behavior is undefined unless 'buffer' has
        // sufficient contents.

    static void getFloat32(float *variable, const char *buffer);
        // Load into the specified 'variable' the four-byte IEEE
        // single-precision floating-point number (in host byte order)
        // comprised of the initial four bytes of the specified 'buffer' (in
        // network byte order).  The behavior is undefined unless 'buffer' has
        // sufficient contents.

                        // *** put arrays of integral values ***

    static void putArrayInt64(char                      *buffer,
                              const bsls::Types::Int64  *values,
                              int                        numValues);
    static void putArrayInt64(char                      *buffer,
                              const bsls::Types::Uint64 *values,
                              int                        numValues);
        // Load into the specified 'buffer' the consecutive eight-byte, two's
        // complement integers (in network byte order) comprised of the
        // least-significant eight bytes of each of the specified 'numValues'
        // leading entries in the specified 'values' (in host byte order).  The
        // behavior is undefined unless 'buffer' has sufficient capacity,
        // 'values' has sufficient contents, and '0 <= numValues'.

    static void putArrayInt56(char                      *buffer,
                              const bsls::Types::Int64  *values,
                              int                        numValues);
    static void putArrayInt56(char                      *buffer,
                              const bsls::Types::Uint64 *values,
                              int                        numValues);
        // Load into the specified 'buffer' the consecutive seven-byte, two's
        // complement integers (in network byte order) comprised of the
        // least-significant seven bytes of each of the specified 'numValues'
        // leading entries in the specified 'values' (in host byte order).  The
        // behavior is undefined unless 'buffer' has sufficient capacity,
        // 'values' has sufficient contents, and '0 <= numValues'.

    static void putArrayInt48(char                      *buffer,
                              const bsls::Types::Int64  *values,
                              int                        numValues);
    static void putArrayInt48(char                      *buffer,
                              const bsls::Types::Uint64 *values,
                              int                        numValues);
        // Load into the specified 'buffer' the consecutive six-byte, two's
        // complement integers (in network byte order) comprised of the
        // least-significant six bytes of each of the specified 'numValues'
        // leading entries in the specified 'values' (in host byte order).  The
        // behavior is undefined unless 'buffer' has sufficient capacity,
        // 'values' has sufficient contents, and '0 <= numValues'.

    static void putArrayInt40(char                      *buffer,
                              const bsls::Types::Int64  *values,
                              int                        numValues);
    static void putArrayInt40(char                      *buffer,
                              const bsls::Types::Uint64 *values,
                              int                        numValues);
        // Load into the specified 'buffer' the consecutive five-byte, two's
        // complement integers (in network byte order) comprised of the
        // least-significant five bytes of each of the specified 'numValues'
        // leading entries in the specified 'values' (in host byte order).  The
        // behavior is undefined unless 'buffer' has sufficient capacity,
        // 'values' has sufficient contents, and '0 <= numValues'.

    static void putArrayInt32(char               *buffer,
                              const int          *values,
                              int                 numValues);
    static void putArrayInt32(char               *buffer,
                              const unsigned int *values,
                              int                 numValues);
        // Load into the specified 'buffer' the consecutive four-byte, two's
        // complement integers (in network byte order) comprised of the
        // least-significant four bytes of each of the specified 'numValues'
        // leading entries in the specified 'values' (in host byte order).  The
        // behavior is undefined unless 'buffer' has sufficient capacity,
        // 'values' has sufficient contents, and '0 <= numValues'.

    static void putArrayInt24(char               *buffer,
                              const int          *values,
                              int                 numValues);
    static void putArrayInt24(char               *buffer,
                              const unsigned int *values,
                              int                 numValues);
        // Load into the specified 'buffer' the consecutive three-byte, two's
        // complement integers (in network byte order) comprised of the
        // least-significant three bytes of each of the specified 'numValues'
        // leading entries in the specified 'values' (in host byte order).  The
        // behavior is undefined unless 'buffer' has sufficient capacity,
        // 'values' has sufficient contents, and '0 <= numValues'.

    static void putArrayInt16(char                 *buffer,
                              const short          *values,
                              int                   numValues);
    static void putArrayInt16(char                 *buffer,
                              const unsigned short *values,
                              int                   numValues);
        // Load into the specified 'buffer' the consecutive two-byte, two's
        // complement integers (in network byte order) comprised of the
        // least-significant two bytes of each of the specified 'numValues'
        // leading entries in the specified 'values' (in host byte order).  The
        // behavior is undefined unless 'buffer' has sufficient capacity,
        // 'values' has sufficient contents, and '0 <= numValues'.

    static void putArrayInt8(char                *buffer,
                             const char          *values,
                             int                  numValues);
    static void putArrayInt8(char                *buffer,
                             const signed char   *values,
                             int                  numValues);
    static void putArrayInt8(char                *buffer,
                             const unsigned char *values,
                             int                  numValues);
        // Load into the specified 'buffer' the consecutive one-byte, two's
        // complement integers comprised of the one byte of each of the
        // specified 'numValues' leading entries in the specified 'values'.
        // The behavior is undefined unless 'buffer' has sufficient capacity,
        // 'values' has sufficient contents, and '0 <= numValues'.

                        // *** put arrays of floating-point values ***

    static void putArrayFloat64(char         *buffer,
                                const double *values,
                                int           numValues);
        // Load into the specified 'buffer' the consecutive eight-byte IEEE
        // double-precision floating-point numbers (in network byte order)
        // comprised of the most-significant eight bytes of each of the
        // specified 'numValues' leading entries in the specified 'values' (in
        // host byte order).  The behavior is undefined unless 'buffer' has
        // sufficient capacity, 'values' has sufficient contents, and
        // '0 <= numValues'.  Note that for non-conforming platforms, this
        // operation may be lossy.

    static void putArrayFloat32(char        *buffer,
                                const float *values,
                                int          numValues);
        // Load into the specified 'buffer' the consecutive four-byte IEEE
        // single-precision floating-point numbers (in network byte order)
        // comprised of the most-significant four bytes of each of the
        // specified 'numValues' leading entries in the specified 'values' (in
        // host byte order).  The behavior is undefined unless 'buffer' has
        // sufficient capacity, 'values' has sufficient contents, and
        // '0 <= numValues'.  Note that for non-conforming platforms, this
        // operation may be lossy.

                        // *** get arrays of integral values ***

    static void getArrayInt64(bsls::Types::Int64 *variables,
                              const char         *buffer,
                              int                 numVariables);
        // Load into the specified 'variables' the consecutive eight-byte,
        // two's complement integers (in host byte order) comprised of each of
        // the specified 'numVariables' leading eight-byte sequences in the
        // specified 'buffer' (in network byte order).  The behavior is
        // undefined unless 'variables' has sufficient capacity, 'buffer' has
        // sufficient contents, and '0 <= numVariables'.  Note that each of the
        // values will be sign-extended.

    static void getArrayUint64(bsls::Types::Uint64 *variables,
                               const char          *buffer,
                               int                  numVariables);
        // Load into the specified 'variables' the consecutive eight-byte,
        // two's complement unsigned integers (in host byte order) comprised of
        // each of the specified 'numVariables' leading eight-byte sequences in
        // the specified 'buffer' (in network byte order).  The behavior is
        // undefined unless 'variables' has sufficient capacity, 'buffer' has
        // sufficient contents, and '0 <= numVariables'.  Note that each of the
        // values will be zero-extended.

    static void getArrayInt56(bsls::Types::Int64 *variables,
                              const char         *buffer,
                              int                 numVariables);
        // Load into the specified 'variables' the consecutive seven-byte,
        // two's complement integers (in host byte order) comprised of each of
        // the specified 'numVariables' leading seven-byte sequences in the
        // specified 'buffer' (in network byte order).  The behavior is
        // undefined unless 'variables' has sufficient capacity, 'buffer' has
        // sufficient contents, and '0 <= numVariables'.  Note that each of the
        // values will be sign-extended.

    static void getArrayUint56(bsls::Types::Uint64 *variables,
                               const char          *buffer,
                               int                  numVariables);
        // Load into the specified 'variables' the consecutive seven-byte,
        // two's complement unsigned integers (in host byte order) comprised of
        // each of the specified 'numVariables' leading seven-byte sequences in
        // the specified 'buffer' (in network byte order).  The behavior is
        // undefined unless 'variables' has sufficient capacity, 'buffer' has
        // sufficient contents, and '0 <= numVariables'.  Note that each of the
        // values will be zero-extended.

    static void getArrayInt48(bsls::Types::Int64 *variables,
                              const char         *buffer,
                              int                 numVariables);
        // Load into the specified 'variables' the consecutive six-byte, two's
        // complement integers (in host byte order) comprised of each of the
        // specified 'numVariables' leading six-byte sequences in the specified
        // 'buffer' (in network byte order).  The behavior is undefined unless
        // 'variables' has sufficient capacity, 'buffer' has sufficient
        // contents, and '0 <= numVariables'.  Note that each of the values
        // will be sign-extended.

    static void getArrayUint48(bsls::Types::Uint64 *variables,
                               const char          *buffer,
                               int                  numVariables);
        // Load into the specified 'variables' the consecutive six-byte, two's
        // complement unsigned integers (in host byte order) comprised of each
        // of the specified 'numVariables' leading six-byte sequences in the
        // specified 'buffer' (in network byte order).  The behavior is
        // undefined unless 'variables' has sufficient capacity, 'buffer' has
        // sufficient contents, and '0 <= numVariables'.  Note that each of the
        // values will be zero-extended.

    static void getArrayInt40(bsls::Types::Int64 *variables,
                              const char         *buffer,
                              int                 numVariables);
        // Load into the specified 'variables' the consecutive five-byte, two's
        // complement integers (in host byte order) comprised of each of the
        // specified 'numVariables' leading five-byte sequences in the
        // specified 'buffer' (in network byte order).  The behavior is
        // undefined unless 'variables' has sufficient capacity, 'buffer' has
        // sufficient contents, and '0 <= numVariables'.  Note that each of the
        // values will be sign-extended.

    static void getArrayUint40(bsls::Types::Uint64 *variables,
                               const char          *buffer,
                               int                  numVariables);
        // Load into the specified 'variables' the consecutive five-byte, two's
        // complement unsigned integers (in host byte order) comprised of each
        // of the specified 'numVariables' leading five-byte sequences in the
        // specified 'buffer' (in network byte order).  The behavior is
        // undefined unless 'variables' has sufficient capacity, 'buffer' has
        // sufficient contents, and '0 <= numVariables'.  Note that each of the
        // values will be zero-extended.

    static void getArrayInt32(int        *variables,
                              const char *buffer,
                              int         numVariables);
        // Load into the specified 'variables' the consecutive four-byte, two's
        // complement integers (in host byte order) comprised of each of the
        // specified 'numVariables' leading four-byte sequences in the
        // specified 'buffer' (in network byte order).  The behavior is
        // undefined unless 'variables' has sufficient capacity, 'buffer' has
        // sufficient contents, and '0 <= numVariables'.  Note that each of the
        // values will be sign-extended.

    static void getArrayUint32(unsigned int *variables,
                               const char   *buffer,
                               int           numVariables);
        // Load into the specified 'variables' the consecutive four-byte, two's
        // complement unsigned integers (in host byte order) comprised of each
        // of the specified 'numVariables' leading four-byte sequences in the
        // specified 'buffer' (in network byte order).  The behavior is
        // undefined unless 'variables' has sufficient capacity, 'buffer' has
        // sufficient contents, and '0 <= numVariables'.  Note that each of the
        // values will be zero-extended.

    static void getArrayInt24(int        *variables,
                              const char *buffer,
                              int         numVariables);
        // Load into the specified 'variables' the consecutive three-byte,
        // two's complement integers (in host byte order) comprised of each of
        // the specified 'numVariables' leading three-byte sequences in the
        // specified 'buffer' (in network byte order).  The behavior is
        // undefined unless 'variables' has sufficient capacity, 'buffer' has
        // sufficient contents, and '0 <= numVariables'.  Note that each of the
        // values will be sign-extended.

    static void getArrayUint24(unsigned int *variables,
                               const char   *buffer,
                               int           numVariables);
        // Load into the specified 'variables' the consecutive three-byte,
        // two's complement unsigned integers (in host byte order) comprised of
        // each of the specified 'numVariables' leading three-byte sequences in
        // the specified 'buffer' (in network byte order).  The behavior is
        // undefined unless 'variables' has sufficient capacity, 'buffer' has
        // sufficient contents, and '0 <= numVariables'.  Note that each of the
        // values will be zero-extended.

    static void getArrayInt16(short      *variables,
                              const char *buffer,
                              int         numVariables);
        // Load into the specified 'variables' the consecutive two-byte, two's
        // complement integers (in host byte order) comprised of each of the
        // specified 'numVariables' leading two-byte sequences in the specified
        // 'buffer' (in network byte order).  The behavior is undefined unless
        // 'variables' has sufficient capacity, 'buffer' has sufficient
        // contents, and '0 <= numVariables'.  Note that each of the values
        // will be sign-extended.

    static void getArrayUint16(unsigned short *variables,
                               const char     *buffer,
                               int             numVariables);
        // Load into the specified 'variables' the consecutive two-byte, two's
        // complement unsigned integers (in host byte order) comprised of each
        // of the specified 'numVariables' leading two-byte sequences in the
        // specified 'buffer' (in network byte order).  The behavior is
        // undefined unless 'variables' has sufficient capacity, 'buffer' has
        // sufficient contents, and '0 <= numVariables'.  Note that each of the
        // values will be zero-extended.

    static void getArrayInt8(char          *variables,
                             const char    *buffer,
                             int            numVariables);
    static void getArrayInt8(signed char   *variables,
                             const char    *buffer,
                             int            numVariables);
    static void getArrayInt8(unsigned char *variables,
                             const char    *buffer,
                             int            numVariables);
        // Load into the specified 'variables' the consecutive one-byte, two's
        // complement integers comprised of each of the specified
        // 'numVariables' leading one-byte sequences in the specified 'buffer'.
        // The behavior is undefined unless 'variables' has sufficient
        // capacity, 'buffer' has sufficient contents, and '0 <= numVariables'.

                        // *** get arrays of floating-point values ***

    static void getArrayFloat64(double     *variables,
                                const char *buffer,
                                int         numVariables);
        // Load into the specified 'variables' the consecutive eight-byte IEEE
        // double-precision floating-point numbers (in host byte order)
        // comprised of each of the specified 'numVariables' leading eight-byte
        // sequences in the specified 'buffer' (in network byte order).  The
        // behavior is undefined unless 'variables' has sufficient capacity,
        // 'buffer' has sufficient contents, and '0 <= numVariables'.

    static void getArrayFloat32(float      *variables,
                                const char *buffer,
                                int         numVariables);
        // Load into the specified 'variables' the consecutive four-byte IEEE
        // single-precision floating-point numbers (in host byte order)
        // comprised of each of the specified 'numVariables' leading four-byte
        // sequences in the specified 'buffer' (in network byte order).  The
        // behavior is undefined unless 'variables' has sufficient capacity,
        // 'buffer' has sufficient contents, and '0 <= numVariables'.

};

// ============================================================================
//                           INLINE DEFINITIONS
// ============================================================================

                         // ----------------------
                         // struct MarshallingUtil
                         // ----------------------

// CLASS METHODS

                        // *** put scalar integral values ***

inline
void MarshallingUtil::putInt64(char *buffer, bsls::Types::Int64 value)
{
    BSLS_ASSERT_SAFE(buffer);

    const char *bytes = reinterpret_cast<char *>(&value);

#if BSLS_PLATFORM_IS_LITTLE_ENDIAN
    buffer[0] = bytes[7];
    buffer[1] = bytes[6];
    buffer[2] = bytes[5];
    buffer[3] = bytes[4];
    buffer[4] = bytes[3];
    buffer[5] = bytes[2];
    buffer[6] = bytes[1];
    buffer[7] = bytes[0];
#else
    bsl::memcpy(buffer, bytes + sizeof value - k_SIZEOF_INT64, k_SIZEOF_INT64);
#endif
}

inline
void MarshallingUtil::putInt56(char *buffer, bsls::Types::Int64 value)
{
    BSLS_ASSERT_SAFE(buffer);

    const char *bytes = reinterpret_cast<char *>(&value);

#if BSLS_PLATFORM_IS_LITTLE_ENDIAN
    buffer[0] = bytes[6];
    buffer[1] = bytes[5];
    buffer[2] = bytes[4];
    buffer[3] = bytes[3];
    buffer[4] = bytes[2];
    buffer[5] = bytes[1];
    buffer[6] = bytes[0];
#else
    bsl::memcpy(buffer, bytes + sizeof value - k_SIZEOF_INT56, k_SIZEOF_INT56);
#endif
}

inline
void MarshallingUtil::putInt48(char *buffer, bsls::Types::Int64 value)
{
    BSLS_ASSERT_SAFE(buffer);

    const char *bytes = reinterpret_cast<char *>(&value);

#if BSLS_PLATFORM_IS_LITTLE_ENDIAN
    buffer[0] = bytes[5];
    buffer[1] = bytes[4];
    buffer[2] = bytes[3];
    buffer[3] = bytes[2];
    buffer[4] = bytes[1];
    buffer[5] = bytes[0];
#else
    bsl::memcpy(buffer, bytes + sizeof value - k_SIZEOF_INT48, k_SIZEOF_INT48);
#endif
}

inline
void MarshallingUtil::putInt40(char *buffer, bsls::Types::Int64 value)
{
    BSLS_ASSERT_SAFE(buffer);

    const char *bytes = reinterpret_cast<char *>(&value);

#if BSLS_PLATFORM_IS_LITTLE_ENDIAN
    buffer[0] = bytes[4];
    buffer[1] = bytes[3];
    buffer[2] = bytes[2];
    buffer[3] = bytes[1];
    buffer[4] = bytes[0];
#else
    bsl::memcpy(buffer, bytes + sizeof value - k_SIZEOF_INT40, k_SIZEOF_INT40);
#endif
}

inline
void MarshallingUtil::putInt32(char *buffer, int value)
{
    BSLS_ASSERT_SAFE(buffer);

    const char *bytes = reinterpret_cast<char *>(&value);

#if BSLS_PLATFORM_IS_LITTLE_ENDIAN
    buffer[0] = bytes[3];
    buffer[1] = bytes[2];
    buffer[2] = bytes[1];
    buffer[3] = bytes[0];
#else
    bsl::memcpy(buffer, bytes + sizeof value - k_SIZEOF_INT32, k_SIZEOF_INT32);
#endif
}

inline
void MarshallingUtil::putInt24(char *buffer, int value)
{
    BSLS_ASSERT_SAFE(buffer);

    const char *bytes = reinterpret_cast<char *>(&value);

#if BSLS_PLATFORM_IS_LITTLE_ENDIAN
    buffer[0] = bytes[2];
    buffer[1] = bytes[1];
    buffer[2] = bytes[0];
#else
    bsl::memcpy(buffer, bytes + sizeof value - k_SIZEOF_INT24, k_SIZEOF_INT24);
#endif
}

inline
void MarshallingUtil::putInt16(char *buffer, int value)
{
    BSLS_ASSERT_SAFE(buffer);

    const char *bytes = reinterpret_cast<char *>(&value);

#if BSLS_PLATFORM_IS_LITTLE_ENDIAN
    buffer[0] = bytes[1];
    buffer[1] = bytes[0];
#else
    bsl::memcpy(buffer, bytes + sizeof value - k_SIZEOF_INT16, k_SIZEOF_INT16);
#endif
}

inline
void MarshallingUtil::putInt8(char *buffer, int value)
{
    BSLS_ASSERT_SAFE(buffer);

    *buffer = static_cast<char>(value);
}

                        // *** put scalar floating-point values ***

inline
void MarshallingUtil::putFloat64(char *buffer, double value)
{
    BSLS_ASSERT_SAFE(buffer);

    const char *bytes = reinterpret_cast<char *>(&value);

#if BSLS_PLATFORM_IS_LITTLE_ENDIAN
    buffer[0] = bytes[sizeof value - 1];
    buffer[1] = bytes[sizeof value - 2];
    buffer[2] = bytes[sizeof value - 3];
    buffer[3] = bytes[sizeof value - 4];
    buffer[4] = bytes[sizeof value - 5];
    buffer[5] = bytes[sizeof value - 6];
    buffer[6] = bytes[sizeof value - 7];
    buffer[7] = bytes[sizeof value - 8];
#else
    bsl::memcpy(buffer, bytes, k_SIZEOF_FLOAT64);
#endif
}

inline
void MarshallingUtil::putFloat32(char *buffer, float value)
{
    BSLS_ASSERT_SAFE(buffer);

    const char *bytes = reinterpret_cast<char *>(&value);

#if BSLS_PLATFORM_IS_LITTLE_ENDIAN
    buffer[0] = bytes[sizeof value - 1];
    buffer[1] = bytes[sizeof value - 2];
    buffer[2] = bytes[sizeof value - 3];
    buffer[3] = bytes[sizeof value - 4];
#else
    bsl::memcpy(buffer, bytes, k_SIZEOF_FLOAT32);
#endif
}

                        // *** get scalar integral values ***

inline
void MarshallingUtil::getInt64(bsls::Types::Int64 *variable,
                               const char         *buffer)
{
    BSLS_ASSERT_SAFE(variable);
    BSLS_ASSERT_SAFE(buffer);

    if (sizeof *variable > k_SIZEOF_INT64) {
        *variable = 0x80 & buffer[0] ? -1 : 0;  // sign extend
    }

    char *bytes = reinterpret_cast<char *>(variable);

#if BSLS_PLATFORM_IS_LITTLE_ENDIAN
    bytes[7] = buffer[0];
    bytes[6] = buffer[1];
    bytes[5] = buffer[2];
    bytes[4] = buffer[3];
    bytes[3] = buffer[4];
    bytes[2] = buffer[5];
    bytes[1] = buffer[6];
    bytes[0] = buffer[7];
#else
    bsl::memcpy(bytes + sizeof *variable - k_SIZEOF_INT64,
                buffer,
                k_SIZEOF_INT64);
#endif
}

inline
void MarshallingUtil::getUint64(bsls::Types::Uint64 *variable,
                                const char          *buffer)
{
    BSLS_ASSERT_SAFE(variable);
    BSLS_ASSERT_SAFE(buffer);

    if (sizeof *variable > k_SIZEOF_INT64) {
        *variable = 0;  // zero-extend
    }

    char *bytes = reinterpret_cast<char *>(variable);

#if BSLS_PLATFORM_IS_LITTLE_ENDIAN
    bytes[7] = buffer[0];
    bytes[6] = buffer[1];
    bytes[5] = buffer[2];
    bytes[4] = buffer[3];
    bytes[3] = buffer[4];
    bytes[2] = buffer[5];
    bytes[1] = buffer[6];
    bytes[0] = buffer[7];
#else
    bsl::memcpy(bytes + sizeof *variable - k_SIZEOF_INT64,
                buffer,
                k_SIZEOF_INT64);
#endif
}

inline
void MarshallingUtil::getInt56(bsls::Types::Int64 *variable,
                               const char         *buffer)
{
    BSLS_ASSERT_SAFE(variable);
    BSLS_ASSERT_SAFE(buffer);

    *variable = 0x80 & buffer[0] ? -1 : 0;  // sign extend

    char *bytes = reinterpret_cast<char *>(variable);

#if BSLS_PLATFORM_IS_LITTLE_ENDIAN
    bytes[6] = buffer[0];
    bytes[5] = buffer[1];
    bytes[4] = buffer[2];
    bytes[3] = buffer[3];
    bytes[2] = buffer[4];
    bytes[1] = buffer[5];
    bytes[0] = buffer[6];
#else
    bsl::memcpy(bytes + sizeof *variable - k_SIZEOF_INT56,
                buffer,
                k_SIZEOF_INT56);
#endif
}

inline
void MarshallingUtil::getUint56(bsls::Types::Uint64 *variable,
                                const char          *buffer)
{
    BSLS_ASSERT_SAFE(variable);
    BSLS_ASSERT_SAFE(buffer);

    *variable = 0;  // zero-extend

    char *bytes = reinterpret_cast<char *>(variable);

#if BSLS_PLATFORM_IS_LITTLE_ENDIAN
    bytes[6] = buffer[0];
    bytes[5] = buffer[1];
    bytes[4] = buffer[2];
    bytes[3] = buffer[3];
    bytes[2] = buffer[4];
    bytes[1] = buffer[5];
    bytes[0] = buffer[6];
#else
    bsl::memcpy(bytes + sizeof *variable - k_SIZEOF_INT56,
                buffer,
                k_SIZEOF_INT56);
#endif
}

inline
void MarshallingUtil::getInt48(bsls::Types::Int64 *variable,
                               const char         *buffer)
{
    BSLS_ASSERT_SAFE(variable);
    BSLS_ASSERT_SAFE(buffer);

    *variable = 0x80 & buffer[0] ? -1 : 0;  // sign extend

    char *bytes = reinterpret_cast<char *>(variable);

#if BSLS_PLATFORM_IS_LITTLE_ENDIAN
    bytes[5] = buffer[0];
    bytes[4] = buffer[1];
    bytes[3] = buffer[2];
    bytes[2] = buffer[3];
    bytes[1] = buffer[4];
    bytes[0] = buffer[5];
#else
    bsl::memcpy(bytes + sizeof *variable - k_SIZEOF_INT48,
                buffer,
                k_SIZEOF_INT48);
#endif
}

inline
void MarshallingUtil::getUint48(bsls::Types::Uint64 *variable,
                                const char          *buffer)
{
    BSLS_ASSERT_SAFE(variable);
    BSLS_ASSERT_SAFE(buffer);

    *variable = 0;  // zero-extend

    char *bytes = reinterpret_cast<char *>(variable);

#if BSLS_PLATFORM_IS_LITTLE_ENDIAN
    bytes[5] = buffer[0];
    bytes[4] = buffer[1];
    bytes[3] = buffer[2];
    bytes[2] = buffer[3];
    bytes[1] = buffer[4];
    bytes[0] = buffer[5];
#else
    bsl::memcpy(bytes + sizeof *variable - k_SIZEOF_INT48,
                buffer,
                k_SIZEOF_INT48);
#endif
}

inline
void MarshallingUtil::getInt40(bsls::Types::Int64 *variable,
                               const char         *buffer)
{
    BSLS_ASSERT_SAFE(variable);
    BSLS_ASSERT_SAFE(buffer);

    *variable = 0x80 & buffer[0] ? -1 : 0;  // sign extend

    char *bytes = reinterpret_cast<char *>(variable);

#if BSLS_PLATFORM_IS_LITTLE_ENDIAN
    bytes[4] = buffer[0];
    bytes[3] = buffer[1];
    bytes[2] = buffer[2];
    bytes[1] = buffer[3];
    bytes[0] = buffer[4];
#else
    bsl::memcpy(bytes + sizeof *variable - k_SIZEOF_INT40,
                buffer,
                k_SIZEOF_INT40);
#endif
}

inline
void MarshallingUtil::getUint40(bsls::Types::Uint64 *variable,
                                const char          *buffer)
{
    BSLS_ASSERT_SAFE(variable);
    BSLS_ASSERT_SAFE(buffer);

    *variable = 0;  // zero-extend

    char *bytes = reinterpret_cast<char *>(variable);

#if BSLS_PLATFORM_IS_LITTLE_ENDIAN
    bytes[4] = buffer[0];
    bytes[3] = buffer[1];
    bytes[2] = buffer[2];
    bytes[1] = buffer[3];
    bytes[0] = buffer[4];
#else
    bsl::memcpy(bytes + sizeof *variable - k_SIZEOF_INT40,
                buffer,
                k_SIZEOF_INT40);
#endif
}

inline
void MarshallingUtil::getInt32(int *variable, const char *buffer)
{
    BSLS_ASSERT_SAFE(variable);
    BSLS_ASSERT_SAFE(buffer);

    if (sizeof *variable > k_SIZEOF_INT32) {
        *variable = 0x80 & buffer[0] ? -1 : 0;  // sign extend
    }

    char *bytes = reinterpret_cast<char *>(variable);

#if BSLS_PLATFORM_IS_LITTLE_ENDIAN
    bytes[3] = buffer[0];
    bytes[2] = buffer[1];
    bytes[1] = buffer[2];
    bytes[0] = buffer[3];
#else
    bsl::memcpy(bytes + sizeof *variable - k_SIZEOF_INT32,
                buffer,
                k_SIZEOF_INT32);
#endif
}

inline
void MarshallingUtil::getUint32(unsigned int *variable, const char *buffer)
{
    BSLS_ASSERT_SAFE(variable);
    BSLS_ASSERT_SAFE(buffer);

    if (sizeof *variable > k_SIZEOF_INT32) {
        *variable = 0;  // zero-extend
    }

    char *bytes = reinterpret_cast<char *>(variable);

#if BSLS_PLATFORM_IS_LITTLE_ENDIAN
    bytes[3] = buffer[0];
    bytes[2] = buffer[1];
    bytes[1] = buffer[2];
    bytes[0] = buffer[3];
#else
    bsl::memcpy(bytes + sizeof *variable - k_SIZEOF_INT32,
                buffer,
                k_SIZEOF_INT32);
#endif
}

inline
void MarshallingUtil::getInt24(int *variable, const char *buffer)
{
    BSLS_ASSERT_SAFE(variable);
    BSLS_ASSERT_SAFE(buffer);

    *variable = 0x80 & buffer[0] ? -1 : 0;  // sign extend

    char *bytes = reinterpret_cast<char *>(variable);

#if BSLS_PLATFORM_IS_LITTLE_ENDIAN
    bytes[2] = buffer[0];
    bytes[1] = buffer[1];
    bytes[0] = buffer[2];
#else
    bsl::memcpy(bytes + sizeof *variable - k_SIZEOF_INT24,
                buffer,
                k_SIZEOF_INT24);
#endif
}

inline
void MarshallingUtil::getUint24(unsigned int *variable, const char *buffer)
{
    BSLS_ASSERT_SAFE(variable);
    BSLS_ASSERT_SAFE(buffer);

    *variable = 0x80 & buffer[0] ? -1 : 0;  // sign extend

    char *bytes = reinterpret_cast<char *>(variable);

#if BSLS_PLATFORM_IS_LITTLE_ENDIAN
    bytes[2] = buffer[0];
    bytes[1] = buffer[1];
    bytes[0] = buffer[2];
#else
    bsl::memcpy(bytes + sizeof *variable - k_SIZEOF_INT24,
                buffer,
                k_SIZEOF_INT24);
#endif
}

inline
void MarshallingUtil::getInt16(short *variable, const char *buffer)
{
    BSLS_ASSERT_SAFE(variable);
    BSLS_ASSERT_SAFE(buffer);

    if (sizeof *variable > k_SIZEOF_INT16) {
        *variable = static_cast<short>(0x80 & buffer[0] ? -1 : 0);
                                                                 // sign extend
    }

    char *bytes = reinterpret_cast<char *>(variable);

#if BSLS_PLATFORM_IS_LITTLE_ENDIAN
    bytes[1] = buffer[0];
    bytes[0] = buffer[1];
#else
    bsl::memcpy(bytes + sizeof *variable - k_SIZEOF_INT16,
                buffer,
                k_SIZEOF_INT16);
#endif
}

inline
void MarshallingUtil::getUint16(unsigned short *variable, const char *buffer)
{
    BSLS_ASSERT_SAFE(variable);
    BSLS_ASSERT_SAFE(buffer);

    if (sizeof *variable > k_SIZEOF_INT16) {
        *variable = 0;  // zero-extend
    }

    char *bytes = reinterpret_cast<char *>(variable);

#if BSLS_PLATFORM_IS_LITTLE_ENDIAN
    bytes[1] = buffer[0];
    bytes[0] = buffer[1];
#else
    bsl::memcpy(bytes + sizeof *variable - k_SIZEOF_INT16,
                buffer,
                k_SIZEOF_INT16);
#endif
}

inline
void MarshallingUtil::getInt8(char *variable, const char *buffer)
{
    BSLS_ASSERT_SAFE(variable);
    BSLS_ASSERT_SAFE(buffer);

    *variable = *buffer;
}

inline
void MarshallingUtil::getInt8(signed char *variable, const char *buffer)
{
    BSLS_ASSERT_SAFE(variable);
    BSLS_ASSERT_SAFE(buffer);

    getInt8(reinterpret_cast<char *>(variable), buffer);
}

inline
void MarshallingUtil::getInt8(unsigned char *variable, const char *buffer)
{
    BSLS_ASSERT_SAFE(variable);
    BSLS_ASSERT_SAFE(buffer);

    getInt8(reinterpret_cast<char *>(variable), buffer);
}

                        // *** get scalar floating-point values ***

inline
void MarshallingUtil::getFloat64(double *variable, const char *buffer)
{
    BSLS_ASSERT_SAFE(variable);
    BSLS_ASSERT_SAFE(buffer);

    if (sizeof *variable > k_SIZEOF_FLOAT64) {
        *variable = 0;  // zero-fill significand
    }

    char *bytes = reinterpret_cast<char *>(variable);

#if BSLS_PLATFORM_IS_LITTLE_ENDIAN
    bytes[sizeof *variable - 1] = buffer[0];
    bytes[sizeof *variable - 2] = buffer[1];
    bytes[sizeof *variable - 3] = buffer[2];
    bytes[sizeof *variable - 4] = buffer[3];
    bytes[sizeof *variable - 5] = buffer[4];
    bytes[sizeof *variable - 6] = buffer[5];
    bytes[sizeof *variable - 7] = buffer[6];
    bytes[sizeof *variable - 8] = buffer[7];
#else
    bsl::memcpy(bytes, buffer, k_SIZEOF_FLOAT64);
#endif
}

inline
void MarshallingUtil::getFloat32(float *variable, const char *buffer)
{
    BSLS_ASSERT_SAFE(variable);
    BSLS_ASSERT_SAFE(buffer);

    if (sizeof *variable > k_SIZEOF_FLOAT32) {
        *variable = 0;  // zero-fill significand
    }

    char *bytes = reinterpret_cast<char *>(variable);

#if BSLS_PLATFORM_IS_LITTLE_ENDIAN
    bytes[sizeof *variable - 1] = buffer[0];
    bytes[sizeof *variable - 2] = buffer[1];
    bytes[sizeof *variable - 3] = buffer[2];
    bytes[sizeof *variable - 4] = buffer[3];
#else
    bsl::memcpy(bytes, buffer, k_SIZEOF_FLOAT32);
#endif
}

                        // *** put arrays of integral values ***

inline
void MarshallingUtil::putArrayInt8(char       *buffer,
                                   const char *values,
                                   int         numValues)
{
    BSLS_ASSERT_SAFE(buffer);
    BSLS_ASSERT_SAFE(values);
    BSLS_ASSERT_SAFE(0 <= numValues);

    bsl::memcpy(buffer, values, numValues);
}

inline
void MarshallingUtil::putArrayInt8(char              *buffer,
                                   const signed char *values,
                                   int                numValues)
{
    BSLS_ASSERT_SAFE(buffer);
    BSLS_ASSERT_SAFE(values);
    BSLS_ASSERT_SAFE(0 <= numValues);

    putArrayInt8(buffer, reinterpret_cast<const char *>(values), numValues);
}

inline
void MarshallingUtil::putArrayInt8(char                *buffer,
                                   const unsigned char *values,
                                   int                  numValues)
{
    BSLS_ASSERT_SAFE(buffer);
    BSLS_ASSERT_SAFE(values);
    BSLS_ASSERT_SAFE(0 <= numValues);

    putArrayInt8(buffer, reinterpret_cast<const char *>(values), numValues);
}

                        // *** get arrays of integral values ***

inline
void MarshallingUtil::getArrayInt8(char       *variables,
                                   const char *buffer,
                                   int         numVariables)
{
    BSLS_ASSERT_SAFE(variables);
    BSLS_ASSERT_SAFE(buffer);
    BSLS_ASSERT_SAFE(0 <= numVariables);

    bsl::memcpy(variables, buffer, numVariables);
}

inline
void MarshallingUtil::getArrayInt8(signed char *variables,
                                   const char  *buffer,
                                   int          numVariables)
{
    BSLS_ASSERT_SAFE(variables);
    BSLS_ASSERT_SAFE(buffer);
    BSLS_ASSERT_SAFE(0 <= numVariables);

    getArrayInt8(reinterpret_cast<char *>(variables), buffer, numVariables);
}

inline
void MarshallingUtil::getArrayInt8(unsigned char *variables,
                                   const char    *buffer,
                                   int            numVariables)
{
    BSLS_ASSERT_SAFE(variables);
    BSLS_ASSERT_SAFE(buffer);
    BSLS_ASSERT_SAFE(0 <= numVariables);

    getArrayInt8(reinterpret_cast<char *>(variables), buffer, numVariables);
}

}  // close package namespace
}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2014 Bloomberg Finance L.P.
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
