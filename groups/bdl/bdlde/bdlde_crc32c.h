// bdlde_crc32c.h                                                     -*-C++-*-
#ifndef INCLUDED_BDLDE_CRC32C
#define INCLUDED_BDLDE_CRC32C

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id$")

//@PURPOSE: Provide utilities to calculate the CRC32-C checksum of a dataset.
//
//@CLASSES:
//  bdlde::Crc32c     : calculates CRC32-C checksum
//  bdlde::Crc32c_Impl: calculates CRC32-C checksum with alternative impl.
//
//@SEE_ALSO: bdlde_crc32
//
//@DESCRIPTION: This component defines a struct, 'bdlde::Crc32c' that
// dramatically accelerates (as opposite to the 'bdlde::Crc32' component)
// calculation of a CRC32-C checksum (a cyclic redundancy check, comprised of
// 32 bits, that uses the Castagnoli polynomial), using a hardware-accelerated
// implementation if supported or a software implementation otherwise.  It
// additionally defines the struct 'bdlde::Crc32c_Impl' to expose alternative
// implementations that should not be used other than to test and benchmark.
// Note that a CRC32-C checksum is a strong and fast technique for determining
// whether or not a message was received without errors.  Also note that a
// CRC-32 checksum does not aid in error correction and is not naively useful
// in any sort of cryptography application.
//
///Thread Safety
///-------------
// Thread safe.
//
///Support for Hardware Acceleration
///---------------------------------
// Hardware-accelerated implementation is enabled at compile time when building
// on a supported architecture with a compatible compiler.  In addition,
// runtime checks are performed to detect whether the running platform has the
// required hardware support:
//: o x86:   SSE4.2 instructions are required
//: o sparc: runtime check is detected by the 'is_sparc_crc32c_avail' system
//:   call
//
///Performance
///-----------
// See the test driver for this component in the '.t.cpp' to compare
// performance of the hardware-accelerated and software implementations against
// various alternative implementations that compute a 32-bit CRC checksum.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Computing and updating a checksum
/// - - - - - - - - - - - - - - - - - - - - - -
// The following code illustrates how to calculate and update a CRC32-C
// checksum for a message over the course of building the full message.
//
// First, prepare a message.
//..
//  bsl::string message = "This is a test message.";
//..
// Now, generate a checksum for 'message'.
//..
//  unsigned int checksum = bdlde::Crc32c::calculate(message.c_str(),
//                                                   message.size());
//..
// Finally, if we learn that our message has grown by another chunk and we want
// to compute the checksum of the original message plus the new chunk, let's
// update the checksum by using it as a starting point.
//..
//  // New chunk
//  bsl::string newChunk = "This is a chunk appended to original message";
//  message += newChunk;
//
//  // Update checksum using previous value as starting point
//  checksum = bdlde::Crc32c::calculate(newChunk.c_str(),
//                                      newChunk.size(),
//                                      checksum);
//..

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

#ifndef INCLUDED_BSL_CSTDDEF
#include <bsl_cstddef.h>
#endif

namespace BloombergLP {
namespace bdlde {

                               // =============
                               // struct Crc32c
                               // =============

struct Crc32c {
    // This class provides runtime-efficient utilities to calculate a CRC32-C
    // checksum.

  public:
    // CLASS DATA
    static const unsigned int k_NULL_CRC32C = 0U;
        // CRC32-C value for a 0 length input.  Note that a buffer with this
        // CRC32-C value need not be a 0 length input.

    // CLASS METHODS
    static unsigned int calculate(const void   *data,
                                  bsl::size_t   length,
                                  unsigned int  crc = k_NULL_CRC32C);
        // Return the CRC32-C value calculated for the specified 'data' over
        // the specified 'length' number of bytes, using the optionally
        // specified 'crc' value as the starting point for the calculation.
        // Note that if 'data' is 0, then 'length' also must be 0.
};

                             // ==================
                             // struct Crc32c_Impl
                             // ==================

struct Crc32c_Impl {
    // This class provides alternative implementations of utilities to
    // calculate a CRC32-C checksum.

  public:
    // CLASS METHODS
    static
    unsigned int calculateSoftware(const void   *data,
                                   bsl::size_t   length,
                                   unsigned int  crc = Crc32c::k_NULL_CRC32C);
        // Return the CRC32-C value calculated for the specified 'data' over
        // the specified 'length' number of bytes, using the optionally
        // specified 'crc' value as the starting point for the calculation.
        // This utilizes a portable software-based implementation to perform
        // the calculation.  Note that if 'data' is 0, then 'length' must also
        // be 0.

    static
    unsigned int calculateHardwareSerial(
                                    const void   *data,
                                    bsl::size_t   length,
                                    unsigned int  crc = Crc32c::k_NULL_CRC32C);
        // Return the CRC32-C value calculated for the specified 'data' over
        // the specified 'length' number of bytes, using the optionally
        // specified 'crc' value as the starting point for the calculation.
        // This utilizes a hardware-based implementation that does not leverage
        // instruction level parallelism to perform the calculation (hence it
        // calculates the crc32c in "serial").  Note that this function will
        // fall back to the software version when running on unsupported
        // platforms.  Also note that if 'data' is 0, then 'length' must also
        // be 0.
};

}  // close package namespace
}  // close enterprise namespace


#endif

// ----------------------------------------------------------------------------
// Copyright 2018 Bloomberg Finance L.P.
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
