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
//  BDLDFP_DECIMALPLATFORM_C99_TR: using C99 (C-language) decimal fp support
//  BDLDFP_DECIMALPLATFORM_DECNUMBER: using the 'decNumber' library
//  BDLDFP_DECIMALPLATFORM_INTELDFP: using the 'IntelDFP' library
//  BDLDFP_DECIMALPLATFORM_HARDWARE: using hardware support for decimals
//  BDLDFP_DECIMALPLATFORM_SOFTWARE: using software only (no hardware support)
//  BDLDFP_DECIMALPLATFORM_BIG_ENDIAN: decimals are big endian
//  BDLDFP_DECIMALPLATFORM_LITTLE_ENDIAN: decimals are little endian
//  BDLDFP_DECIMALPLATFORM_DPD: using densely packed decimal representation
//  BDLDFP_DECIMALPLATFORM_BININT: binary integer significand representation
//  BDLDFP_DECIMALPLATFORM_SNPRINTF_BUFFER_SIZE: internal use only (remove)
//  BDLDFP_DECIMALPLATFORM_C99_QNAN*: internal use only, will be removed
//  BDLDFP_DECIMALPLATFORM_C99_SNAN*: internal use only, will be removed
//
//@SEE_ALSO: bdldfp_decimal, bdldfp_decimalutil, bdldfp_decimalconvertutil
//
//@DESCRIPTION: This component provides a suite of preprocessor macros that
// identify and define platform-specific compile-time attributes that are
// related to decimal floating-point support.  The macros provided by this
// component are defined to the be non-zero (true) when enabled on a given
// platform, and are undefined on platforms where they are not enabled.  Some
// macros are implentation details, providing platform-specific information
// that is not useful outside of the BDE implementation.
//
///Macros Defining the Underlying Implementation
///---------------------------------------------
// Only one of these macros will be defined any supported platform:
//
// 'BDLDFP_DECIMALPLATFORM_C99_TR' - Full ISO/IEC TR 24732 support with
//                                   library.
//
// 'BDLDFP_DECIMALPLATFORM_DECNUMBER' - Using bdl+decnumber as emulation
//
// 'BDLDFP_DECIMALPLATFORM_INTELDFP' - Using bdl+inteldfp as emulation
//
///Macros Defining Hardware Support vs. Software Emulation
///-------------------------------------------------------
// Only one of these macros will be defined on any supported platform:
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
//                                representation
//
// 'BDLDFP_DECIMALPLATFORM_BININT' - Binary Integer significand representation
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

#ifndef INCLUDED_BSLMF_ASSERT
#include <bslmf_assert.h>
#endif

// C99 decimal support on xlC/Aix issues should be resolved (see
// DRQS 39471014).


// We have decided to disable IBM hardware floating point, at this time.

#if 0 && defined(BSLS_PLATFORM_CMP_IBM) && defined(__IBM_DFP__) && __IBM_DFP__


#  define BDLDFP_DECIMALPLATFORM_C99_QNAN32  __d32_qNaN()
#  define BDLDFP_DECIMALPLATFORM_C99_QNAN64  __d64_qNaN()
#  define BDLDFP_DECIMALPLATFORM_C99_QNAN128 __d128_qNaN()
#  define BDLDFP_DECIMALPLATFORM_C99_SNAN32  __d32_sNaN()
#  define BDLDFP_DECIMALPLATFORM_C99_SNAN64  __d64_sNaN()
#  define BDLDFP_DECIMALPLATFORM_C99_SNAN128 __d128_sNaN()

#  define BDLDFP_DECIMALPLATFORM_C99_NO_FMAD64 1

#  if defined(__IBM_DFP_SW_EMULATION__) && __IBM_DFP_SW_EMULATION__
#    define BDLDFP_DECIMALPLATFORM_SOFTWARE 1
#  else
#    define BDLDFP_DECIMALPLATFORM_HARDWARE 1
#  endif
#  define BDLDFP_DECIMALPLATFORM_C99_TR   1
#else
#  define BDLDFP_DECIMALPLATFORM_SOFTWARE 1
#endif

// TODO: a rough hack -- fix this.

#  define BDLDFP_DECIMALPLATFORM_SNPRINTF_BUFFER_SIZE 256

#ifndef BDLDFP_DECIMALPLATFORM_C99_TR
#  if 1
#    define BDLDFP_DECIMALPLATFORM_INTELDFP  1
#  else
#    define BDLDFP_DECIMALPLATFORM_DECNUMBER 1
#  endif
#endif

#ifdef BDLDFP_DECIMALPLATFORM_C99_TR
#  define BDLDFP_DECIMALPLATFORM_DPD    1
#elif defined(BDLDFP_DECIMALPLATFORM_DECNUMBER)
#  define BDLDFP_DECIMALPLATFORM_DPD    1
#elif defined(BDLDFP_DECIMALPLATFORM_INTELDFP)
#  define BDLDFP_DECIMALPLATFORM_BININT 1
#else
#  error Unsupported decimal floating point platform.
BSLMF_ASSERT(false);
#endif

// The Decimal Platform macros for endian settings allow for the possibility of
// future implementations which have divergent integer and decimal floating
// point byte orders.

#ifdef BSLS_PLATFORM_IS_BIG_ENDIAN
#  define BDLDFP_DECIMALPLATFORM_BIG_ENDIAN    1
#elif defined(BSLS_PLATFORM_IS_LITTLE_ENDIAN)
#  define BDLDFP_DECIMALPLATFORM_LITTLE_ENDIAN 1
#else
#  error "Unsupported endianness"
BSLMF_ASSERT(false);
#endif

#define BDLDFP_DECIMALPLATFORM_COMPILER_ERROR BSLMF_ASSERT(false)

                        // Validation

// The following checks are performed to guarantee the reasonable
// initialization of the compile time features defined in this component.


// Verify exactly one Implementation class (Hardware or software)

#if BDLDFP_DECIMALPLATFORM_HARDWARE                                           \
  + BDLDFP_DECIMALPLATFORM_SOFTWARE != 1
    #error "Exactly one decimal platform implementation class must be set."
    BDLDFP_DECIMALPLATFORM_COMPILER_ERROR;
#endif

// Verify exactly one endian selection must be made

#if BDLDFP_DECIMALPLATFORM_BIG_ENDIAN                                         \
  + BDLDFP_DECIMALPLATFORM_LITTLE_ENDIAN != 1
    #error "Exactly one decimal platform endian selection must be set."
    BDLDFP_DECIMALPLATFORM_COMPILER_ERROR;
#endif

// Verify exactly one format selection must be made

#if BDLDFP_DECIMALPLATFORM_DPD                                                \
  + BDLDFP_DECIMALPLATFORM_BININT != 1
    #error "Exactly one decimal platform format selection must be set."
    BDLDFP_DECIMALPLATFORM_COMPILER_ERROR;
#endif

// Verify exactly one implementation library must be selected

#if BDLDFP_DECIMALPLATFORM_INTELDFP                                           \
  + BDLDFP_DECIMALPLATFORM_C99_TR                                             \
  + BDLDFP_DECIMALPLATFORM_DECNUMBER != 1
    #error "Exactly one decimal implementation library must be selected."
    BDLDFP_DECIMALPLATFORM_COMPILER_ERROR;
#endif

// Under Intel implementation, SOFTWARE and BININT must be set
#if defined(BDLDFP_DECIMALPLATFORM_INTELDFP) && \
   !defined(BDLDFP_DECIMALPLATFORM_SOFTWARE) && \
   !defined(BDLDFP_DECIMALPLATFORM_BININT)
    #error "INTELDFP mode requires SOFTWARE and BININT modes to be set."
    BDLDFP_DECIMALPLATFORM_COMPILER_ERROR;
#elif defined(BDLDFP_DECIMALPLATFORM_DECNUMBER) && \
     !defined(BDLDFP_DECIMALPLATFORM_SOFTWARE) &&\
     !defined(BDLDFP_DECIMALPLATFORM_DPD)
    #error "DECNUMBER mode requires SOFTWARE and DPD modes to be set."
    BDLDFP_DECIMALPLATFORM_COMPILER_ERROR;
#endif

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
