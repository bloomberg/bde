// bslalg_untrustedarrayreadutil.h                                    -*-C++-*-
#ifndef INCLUDED_BSLALG_UNTRUSTEDARRAYREADUTIL
#define INCLUDED_BSLALG_UNTRUSTEDARRAYREADUTIL

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide utilities for sizing reads of arrays of untrusted length.
//
//@CLASSES:
//  bslalg::UntrustedArrayReadUtil: namespace for array read-sizing functions
//
//@DESCRIPTION: This component provides a namespace,
// `bslalg::UntrustedArrayReadUtil`, for functions that help decoders read an
// array whose number of elements comes from untrusted input (e.g., a length
// prefix in a network message or a count in a file header) without reading
// past the end of the input or allocating memory out of proportion to the
// actual size of the input.
//
// Each function takes the number of bytes buffered in the input, i.e., present
// and readable without blocking (e.g., in memory, or in the get area of a
// stream buffer).  More input may follow unless the buffer holds the entire
// input, as when reading from memory.  Where this number may be unknown
// (e.g., the result of `std::streambuf::in_avail`), it is taken as a signed
// `std::ptrdiff_t`, and any non-positive value is treated as "unknown"; this
// lets callers pass `in_avail()` directly, without first having to handle its
// `-1` (end of input) and `0` (unknown) results.
//
// The following functions are provided:
// * `canReadAll`: whether all of the elements of an array are buffered.  When
//   the buffer holds the entire input (e.g., when reading from memory),
//   `false` means that the input is too short.
// * `computeReserveCount`: how many elements to reserve capacity for before
//   reading an array, so that a (possibly forged) element count does not
//   cause an allocation larger than the buffered input could fill.
// * `computeNextReadCount`: how many elements to read on each step of a loop
//   that grows a destination as it reads, so that the destination's size is
//   bounded by a constant factor of the input actually read and buffered.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Validating an Array Length Against an In-Memory Buffer
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we are reading 4-byte values from a buffer received from an
// untrusted source, and the number of values to read was itself read from that
// buffer.
//
// First, we define a simple type describing the input:
// ```
// /// This `struct` describes a buffer of input bytes and the position of the
// /// next byte to be read.
// struct Input {
//
//     // PUBLIC DATA
//     const char  *d_data_p;  // input bytes
//     std::size_t  d_length;  // number of bytes in `d_data_p`
//     std::size_t  d_cursor;  // index of the next byte to read
// };
// ```
// Then, we implement a function that reads the values, using `canReadAll` to
// check that the buffer holds all of them:
// ```
// /// Load into the specified `values` the specified `numValues` 4-byte
// /// values read from the specified `input`, and advance its cursor past
// /// them.  Return 0 on success, and a non-zero value, with no effect on
// /// `values` or `input`, if `input` does not hold `numValues` more values.
// int readValues(int *values, std::size_t numValues, Input *input)
// {
//     const std::size_t k_VALUE_SIZE = 4;
//
//     if (!bslalg::UntrustedArrayReadUtil::canReadAll(
//                                        numValues,
//                                        k_VALUE_SIZE,
//                                        input->d_length - input->d_cursor)) {
//         return -1;                                                // RETURN
//     }
//
//     const std::size_t numBytes = numValues * k_VALUE_SIZE;
//     std::memcpy(values, input->d_data_p + input->d_cursor, numBytes);
//     input->d_cursor += numBytes;
//     return 0;
// }
// ```
// Note that the multiplication is safe once `canReadAll` has returned `true`,
// as the product cannot then exceed the number of buffered bytes.
//
// Finally, we observe that a forged count is rejected even when multiplying it
// by the value size would wrap around to a small number:
// ```
// const char  buffer[16] = { 0 };
// Input       input      = { buffer, sizeof buffer, 0 };
// int         values[4];
//
// assert(0 == readValues(values, 4, &input));
// assert(16 == input.d_cursor);
//
// input.d_cursor = 0;
// const std::size_t k_FORGED = (std::numeric_limits<std::size_t>::max() / 4)
//                            + 1;  // `k_FORGED * 4` wraps to 0
// assert(0 != readValues(values, k_FORGED, &input));
// assert(0 == input.d_cursor);
// ```
//
///Example 2: Reading a String of Untrusted Length from a Stream Buffer
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we are reading a string whose length was read from a
// `std::streambuf` that delivers data from an untrusted source.  Resizing the
// string to the claimed length up front would let a forged length cause an
// arbitrarily large allocation, so we instead grow the string as we read.
//
// First, we implement a function that reads the string, using
// `computeNextReadCount` to choose how much to read on each step:
// ```
// /// Load into the specified `result` the specified `length` bytes read
// /// from the specified `streamBuf`.  Return 0 on success, and a non-zero
// /// value, leaving `result` in a valid but unspecified state, if
// /// `streamBuf` does not deliver `length` bytes.
// int readString(std::string    *result,
//                std::streambuf *streamBuf,
//                std::size_t     length)
// {
//     const std::size_t k_MIN_READ_COUNT = 4 * 1024;
//
//     result->clear();
//     while (result->size() < length) {
//         const std::size_t numRead   = result->size();
//         const std::size_t numToRead =
//             bslalg::UntrustedArrayReadUtil::computeNextReadCount(
//                                                       numRead,
//                                                       length - numRead,
//                                                       1,
//                                                       streamBuf->in_avail(),
//                                                       k_MIN_READ_COUNT);
//
//         result->resize(numRead + numToRead);
//         const std::streamsize numBytes =
//                                     static_cast<std::streamsize>(numToRead);
//         if (numBytes != streamBuf->sgetn(&(*result)[numRead], numBytes)) {
//             return -1;                                            // RETURN
//         }
//     }
//     return 0;
// }
// ```
// Then, we read a string whose length is accurate:
// ```
// std::stringbuf input("Hello, world!");
// std::string    result;
//
// assert(0 == readString(&result, &input, 13));
// assert("Hello, world!" == result);
// ```
// Finally, we observe that a forged length causes the read to fail after an
// allocation bounded by the minimum read count plus twice the buffered input,
// rather than by the length:
// ```
// std::stringbuf shortInput("Hello, world!");
//
// assert(0 != readString(&result, &shortInput, 1024 * 1024 * 1024));
// assert(4 * 1024 + 2 * 13 >= result.size());
// ```

#include <bslscm_version.h>

#include <bsls_assert.h>

#include <cstddef>  // `std::size_t`, `std::ptrdiff_t`
#include <limits>

namespace BloombergLP {
namespace bslalg {

                       // =============================
                       // struct UntrustedArrayReadUtil
                       // =============================

/// This `struct` provides a namespace for functions that size the reads of an
/// array whose number of elements comes from untrusted input.
struct UntrustedArrayReadUtil {
    // CLASS METHODS

    /// Return `true` if the specified `numBufferedBytes` bytes hold at least
    /// the specified `numElements` elements of the specified `elementSize`
    /// bytes each, and `false` otherwise.  The behavior is undefined unless
    /// `0 < elementSize`.
    static bool canReadAll(std::size_t numElements,
                           std::size_t elementSize,
                           std::size_t numBufferedBytes);

    /// Return the number of elements for which to reserve capacity before
    /// reading an array of the specified (untrusted) `numElements` elements
    /// of the specified `elementSize` bytes each, from input of which the
    /// specified `numBufferedBytes` bytes are buffered: the lesser of
    /// `numElements` and the number of elements that `numBufferedBytes` bytes
    /// can hold, or 0 if `numBufferedBytes` is not positive (i.e., if the
    /// number of buffered bytes is unknown).  The behavior is undefined unless
    /// `0 < elementSize`.
    static std::size_t computeReserveCount(std::size_t    numElements,
                                           std::size_t    elementSize,
                                           std::ptrdiff_t numBufferedBytes);

    /// Return the number of elements to read next when reading an array of
    /// elements of the specified `elementSize` bytes each, of which the
    /// specified `numRead` elements have been read and the specified (possibly
    /// untrusted) `numRemaining` elements remain, from input of which the
    /// specified `numBufferedBytes` bytes are buffered, a non-positive
    /// `numBufferedBytes` indicating that this is unknown.  The returned count
    /// never exceeds `numRemaining`, and is chosen so that a loop that reads
    /// it on each step:
    /// * reads at least the specified `minReadCount` elements on each step,
    ///   or all remaining elements if fewer remain;
    /// * at least doubles the number of elements read on each step, and so
    ///   completes in a number of steps logarithmic in `numRemaining`;
    /// * reads all buffered elements in a single step; and
    /// * never grows a destination beyond `minReadCount` plus twice the
    ///   number of elements read plus buffered.
    ///
    /// The behavior is undefined unless `0 < elementSize` and
    /// `0 < minReadCount`.
    static std::size_t computeNextReadCount(std::size_t    numRead,
                                            std::size_t    numRemaining,
                                            std::size_t    elementSize,
                                            std::ptrdiff_t numBufferedBytes,
                                            std::size_t    minReadCount);
};

// ============================================================================
//                          INLINE DEFINITIONS
// ============================================================================

                       // -----------------------------
                       // struct UntrustedArrayReadUtil
                       // -----------------------------

// CLASS METHODS
inline
bool UntrustedArrayReadUtil::canReadAll(std::size_t numElements,
                                        std::size_t elementSize,
                                        std::size_t numBufferedBytes)
{
    BSLS_ASSERT(0 < elementSize);

    return numElements <= numBufferedBytes / elementSize;
}

inline
std::size_t UntrustedArrayReadUtil::computeReserveCount(
                                               std::size_t    numElements,
                                               std::size_t    elementSize,
                                               std::ptrdiff_t numBufferedBytes)
{
    BSLS_ASSERT(0 < elementSize);

    if (numBufferedBytes <= 0) {
        return 0;                                                     // RETURN
    }

    const std::size_t numBuffered =
                      static_cast<std::size_t>(numBufferedBytes) / elementSize;

    return numElements < numBuffered ? numElements : numBuffered;
}

inline
std::size_t UntrustedArrayReadUtil::computeNextReadCount(
                                               std::size_t    numRead,
                                               std::size_t    numRemaining,
                                               std::size_t    elementSize,
                                               std::ptrdiff_t numBufferedBytes,
                                               std::size_t    minReadCount)
{
    BSLS_ASSERT(0 < elementSize);
    BSLS_ASSERT(0 < minReadCount);

    const std::size_t numBuffered =
                 0 < numBufferedBytes
                     ? static_cast<std::size_t>(numBufferedBytes) / elementSize
                     : 0;

    const std::size_t base   = numRead < minReadCount ? minReadCount : numRead;
    const std::size_t growth = 2 * numBuffered;  // numBuffered <= PTRDIFF_MAX

    // Saturated add to prevent potential wrap when adding `base` and `growth`.
    const std::size_t k_MAX = std::numeric_limits<std::size_t>::max();
    const std::size_t count = growth < k_MAX - base ? base + growth : k_MAX;

    return count < numRemaining ? count : numRemaining;
}

}  // close package namespace
}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2026 Bloomberg Finance L.P.
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
