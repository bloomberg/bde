// bdldfp_decimalplatform.h                                           -*-C++-*-
#ifndef INCLUDED_BDLDFP_DECIMALPLATFORM
#define INCLUDED_BDLDFP_DECIMALPLATFORM

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id$")

//@PURPOSE: Provide decimal floating-point platform information macros.
//
//@MACROS:
//  BDLDFP_DECIMALPLATFORM_C99_TR: We have an use C99 decimal fp support
//  BDLDFP_DECIMALPLATFORM_DECNUMBER: using the 'decNumber' library
//  BDLDFP_DECIMALPLATFORM_INTELDFP: using the 'IntelDFP' library
//  BDLDFP_DECIMALPLATFORM_HARDWARE: hardware support for decimals
//  BDLDFP_DECIMALPLATFORM_SOFTWARE: no hardware support for decimals is used
//  BDLDFP_DECIMALPLATFORM_BIG_ENDIAN: decimals are big endian
//  BDLDFP_DECIMALPLATFORM_LITTLE_ENDIAN: decimals are little endian
//  BDLDFP_DECIMALPLATFORM_DPD: densely packed decimal significand
//  BDLDFP_DECIMALPLATFORM_BININT: binary integer significand
//  BDLDFP_DECIMALPLATFORM_SNPRINTF_BUFFER_SIZE: internal use only (remove)
//  BDLDFP_DECIMALPLATFORM_C99_QNAN*: internal use only, will be removed
//  BDLDFP_DECIMALPLATFORM_C99_SNAN*: internal use only, will be removed
//
//@SEE ALSO: bdldfp_decimal, bdldfp_decimalutil, bdldfp_decimalconvertutil
//
//@DESCRIPTION: This component provides a suite of preprocessor macros that
// identify and define platform-specific compile-time attributes that are
// related to decimal floating-point support.  Most of these attributes are
// represented by preprocessor macros that are always defined by this component
// (header) and their replacement value (0 or 1) determines the attribute.
// Some macros are implentation details, providing platform-specific
// information that is not useful outside of the BDE implementation.
//
///Macros Defining the Underlying Implementation
///---------------------------------------------
// Only one of these macros will evaluate to '1' on any supported platform:
//
// 'BDLDFP_DECIMALPLATFORM_C99_TR' - Full ISO/IEC TR 24732 support with
//                                   library.
//
// 'BDLDFP_DECIMALPLATFORM_DECNUMBER' - Using bdl+decnumber as emulation
//
///Macros Defining Hardware Support vs. Software Emulation
///-------------------------------------------------------
// Only one of these macros will evaluate to '1' on any supported platform:
//
// 'BDLDFP_DECIMALPLATFORM_HARDWARE' - Hardware support for at least one type.
//
// 'BDLDFP_DECIMALPLATFORM_SOFTWARE' - Software emulation for all types
//
///Endianness
///----------
// There is a chance that the endianness of integers and decimal floating
// point are not the same as those of integers; therefore we provide two
// macros here to support implementation of endianness conversions.
//
// 'BDLDFP_DECIMALPLATFORM_BIG_ENDIAN' - big endian storage
//
// 'BDLDFP_DECIMALPLATFORM_LITTLE_ENDIAN' - little endian storage
//
///Encoding
///--------
// There are two IEEE-754 sanctioned encodings for the significand: densely
// packed decimal and binary integer significand.  We provide two macros here
// to support implementation of encoding conversions.
//
// 'BDLDFP_DECIMALPLATFORM_DPD' - Densely Packed Decimal significand
//
// 'BDLDFP_DECIMALPLATFORM_BININT' - Binary Integer significand
//
///Implementation Detail Macros
///----------------------------
// These macros may or may not be present, depending on the platform.  They are
// not meant for use outside of the Decimal Floating Point implementation;
// Therefore using them in your code will result in undefined behavior.
//
// 'BDLDFP_DECIMALPLATFORM_SNPRINTF_BUFFER_SIZE' - max chars to print Dec128
//
// 'BDLDFP_DECIMALPLATFORM_C99_QNAN32' - The expression to get a quiet NaN of
// 'BDLDFP_DECIMALPLATFORM_C99_QNAN64'   the type the number postfix refers to.
// 'BDLDFP_DECIMALPLATFORM_C99_QNAN128'
//
// 'BDLDFP_DECIMALPLATFORM_C99_SNAN32' - The expression to get a signaling NaN
// 'BDLDFP_DECIMALPLATFORM_C99_SNAN64'   of the type the number postfix refers
// 'BDLDFP_DECIMALPLATFORM_C99_SNAN128'  to.
//
///Usage
///-----
// In this section, we show the intended usage of this component.
//..
//..

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORM
#include <bsls_platform.h>
#endif

// C99 decimal support on xlC/Aix issues should be resolved (see 
// DRQS 39471014).

#if defined(BSLS_PLATFORM_CMP_IBM) && defined(__IBM_DFP__) && __IBM_DFP__


#  define BDLDFP_DECIMALPLATFORM_C99_QNAN32  __d32_qNaN()
#  define BDLDFP_DECIMALPLATFORM_C99_QNAN64  __d64_qNaN()
#  define BDLDFP_DECIMALPLATFORM_C99_QNAN128 __d128_qNaN()
#  define BDLDFP_DECIMALPLATFORM_C99_SNAN32  __d32_sNaN()
#  define BDLDFP_DECIMALPLATFORM_C99_SNAN64  __d64_sNaN()
#  define BDLDFP_DECIMALPLATFORM_C99_SNAN128 __d128_sNaN()

#  define BDLDFP_DECIMALPLATFORM_C99_NO_FMAD64 1

#  if defined(__IBM_DFP_SW_EMULATION__) && __IBM_DFP_SW_EMULATION__
#    define BDLDFP_DECIMALPLATFORM_C99_TR   0
#    define BDLDFP_DECIMALPLATFORM_SOFTWARE 1
#    define BDLDFP_DECIMALPLATFORM_HARDWARE 0
#  else
#    define BDLDFP_DECIMALPLATFORM_C99_TR   1
#    define BDLDFP_DECIMALPLATFORM_SOFTWARE 0
#    define BDLDFP_DECIMALPLATFORM_HARDWARE 1
#  endif
#  define BDLDFP_DECIMALPLATFORM_SNPRINTF_BUFFER_SIZE 129
#else
#  define BDLDFP_DECIMALPLATFORM_C99_TR 0
#  define BDLDFP_DECIMALPLATFORM_SOFTWARE 1
#  define BDLDFP_DECIMALPLATFORM_HARDWARE 0
#endif


#if BDLDFP_DECIMALPLATFORM_SOFTWARE
#  if defined(BSLS_PLATFORM_CMP_GNU) && (defined(BSLS_PLATFORM_CPU_X86) || defined(BSLS_PLATFORM_CPU_X86_64))
#    define BDLDFP_DECIMALPLATFORM_DECNUMBER 0
#    define BDLDFP_DECIMALPLATFORM_INTELDFP  1
#  else
#    define BDLDFP_DECIMALPLATFORM_DECNUMBER 1
#    define BDLDFP_DECIMALPLATFORM_INTELDFP  0
#  endif
#else
#  define BDLDFP_DECIMALPLATFORM_DECNUMBER 0
#  define BDLDFP_DECIMALPLATFORM_INTELDFP  0
#endif

#if BDLDFP_DECIMALPLATFORM_C99_TR
#  define BDLDFP_DECIMALPLATFORM_DPD    1
#  define BDLDFP_DECIMALPLATFORM_BININT 0
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
#  define BDLDFP_DECIMALPLATFORM_DPD    1
#  define BDLDFP_DECIMALPLATFORM_BININT 0
#  define BDLDFP_DECIMALPLATFORM_SNPRINTF_BUFFER_SIZE DECQUAD_String
#elif BDLDFP_DECIMALPLATFORM_INTELDFP
#  define BDLDFP_DECIMALPLATFORM_DPD    0
#  define BDLDFP_DECIMALPLATFORM_BININT 1
// TODO: a rough hack -- fix this.
#  define BDLDFP_DECIMALPLATFORM_SNPRINTF_BUFFER_SIZE 256
#else
#  error Unsupported decimal floating point platform.
#endif

#ifdef BSLS_PLATFORM_IS_BIG_ENDIAN
#  define BDLDFP_DECIMALPLATFORM_BIG_ENDIAN    1
#  define BDLDFP_DECIMALPLATFORM_LITTLE_ENDIAN 0
#elif BSLS_PLATFORM_IS_LITTLE_ENDIAN
#  define BDLDFP_DECIMALPLATFORM_BIG_ENDIAN    0
#  define BDLDFP_DECIMALPLATFORM_LITTLE_ENDIAN 1
#else
#  error "Unsupported endianness"
class UnsupportedEndinanness;
UnsupportedEndinanness forceError[sizeof(UnsupportedEndinanness)];
#endif

// All supported platforms are DPD for now


#endif

// ----------------------------------------------------------------------------
// Copyright (C) 2014 Bloomberg L.P.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
// ----------------------------- END-OF-FILE ----------------------------------
