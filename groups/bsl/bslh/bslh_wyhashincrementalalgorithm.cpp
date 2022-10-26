// bslh_wyhashincrementalalgorithm.cpp                                -*-C++-*-
#include <bslh_wyhashincrementalalgorithm.h>

// The original was downloaded from
// https://github.com/wangyi-fudan/wyhash/blob/master/wyhash.h which had been
// updated on September 14, 2021, last commit 166f352

// License copied from github:

// This is free and unencumbered software released into the public domain under
// The Unlicense (http://unlicense.org/)
// main repo: https://github.com/wangyi-fudan/wyhash
// author: Wang Yi <godspeed_china@yeah.net>
// contributors: Reini Urban, Dietrich Epp, Joshua Haberman, Tommy Ettinger,
// Daniel Lemire, Otmar Ertl, cocowalla, leo-yuriev, Diego Barrios Romero,
// paulie-g, dumblob, Yann Collet, ivte-ms, hyb, James Z.M. Gao,
// easyaspi314 (Devin), TheOneric

// quick example:
// string s="fjsakfdsjkf";
// uint64_t hash=wyhash(s.c_str(), s.size(), 0, _wyp);

//=============================================================================
//                      BDE IMPLEMENTATION NOTES
//=============================================================================

                        // * Incremental Property *

// 'bslh::WyHashAlgorithm' has been radically altered from the 'wyhash' source
// downloaded from github, to achieve the 'incremental' quality, namely that
// hashing several segments with 'operator()' following by a single
// 'computeHash' at the end will yield the same result as if all the segments
// had been concatenated into a single segment, and hashed with a single
// 'operator()-then-computeHash' call.  When hashed on a single segment,
// 'bslh::WyHashAlgorithm' will give a value that is exactly equal to that
// yielded by the github code.
//
// The modified algorithm turned out, surprisingly, to be faster than the much
// simpler original algorithm when hashing an object with several parts adding
// up to 48 bytes or less, because with the modified algorithm, all that the
// 'operator()' calls do is append the data to be hashed to a buffer, and
// 'computeHash' does all the calculation in one action.  With the old
// algorithm, a lot of calculation was done by each 'operator()' call and
// 'computeHash' just returns an already computed 8-byte value with no work.

                            // * Pseudo Multiply *

// This algorithm does a lot of 64x64 -> 128-bit multiples.  At the time of
// this writing, only 64-bit builds on x86 architectures had an intrinsic
// operation for this, on all other platforms, including 32-bit x86
// architectures, the multiples are done by combining several 64x64 -> 64-bit
// multiplies with shifts and adds.  There is a #define'd constant
// 'BSLH_WYHASHINCREMENTALALGORITHM_WYMUM_PSEUDO_MULTIPLY' that substitutes a
// simpler operation for 64x64 -> 128-bit multiplies that does not give exactly
// the same result, but provides adequate hash quality.  However, when we
// benchmarked this, it did not result in a significant speed improvement, and
// the intrinsic multiply was faster on 64-bit x86 architectures, so pseudo
// mutliply is disabled.

                             // Byte Order Asymmetry

// In the original 'wyhash' algorithm on github.com, the two methods that load
// words of data '_wyr4' and '_wyr8', were conditionally compiled to byte-swap
// the loaded word on big-endian machines, which made the algorithm produce the
// same hash value regardless of the host's byte order.
//
// We felt this symmetry was not a valuable property, and it came at the cost
// of speed, so we eliminated the byte-swapping.  This means big-endian
// machines and little-endian machines will get different hash values.
//
// It would be trivial to re-introduce this symmetry, just by conditionally
// compiling a byte order swap into '_wry4' and '_wr8', only on big-endian
// machines.

                // Description of 'wyhash' Algorithm on github

// Hashing occurs one segment at a time, and a single 'uint64_t' field 'd_seed'
// is updated with the result of hashing each segment, accumulating a result.
//
//: 1 if the segment length is 16 bytes or less, one approach that is not
//:   complex enough to warrant documentation in prose here is used.
//:
//: 2 if the segment length is 16 to 48 bytes:
//:   o Each 16 byte sub-segment except the last are hashed, accumulating
//:     results in a 'uint64_t'.
//:
//:   o The last 16 bytes of the whole segment are always hashed, which may
//:     overlap with the most recent 16-byte hash.
//:
//: 3 if the segment is over 48 bytes
//:   o Each 48 byte sub-segment except the last is hashed, accumulating
//:     results in 3 'uint64_t's.
//:
//:   o The 3 'uint64_t's are xor-ed into one.
//:
//:   o Each remaining 16 byte sub-segment except the last are hashed,
//:     accumulating results into the one 'uint64_t'.
//:
//:   o The last 16 bytes of the whole segment are always hashed, which may
//:     overlap with the most recent 16-byte hash.
//
                    // Description of Incremental 'wyhash'

// Our desire was to develop an algorithm that would, when hashing a single
// memory segment of any size, yield exactly the same result as the 'wyhash'
// algorithm on 'github', but would also support 'incremental' hashing, that
// is, if the segment was processed with several calls to 'operator()'
// processing the segment in sub-segments in their sequential order, followed
// by 'computeHash', will still yield exactly the same result.
//
// It was clear that the original algorithm was not designed with this in mind.
//
// The approach taken was to have a 48-byte ring-buffer, the 'repeat buffer',
// and when 'operator()' is called, sub-segments are just appended to the
// buffer, and then processing is all done on the buffer in 'computeHash' at
// the end.  Which is how we handle total lengths up to 48 bytes.
//
// The problem, then, is what if more than 48 bytes are processed?
//
// When the buffer is full and more data is available within an 'operator()'
// call, the 48 bytes are processed as in the first step within '3' above.
// These results have to be accumulated into 3 'uint64_t's, 'd_seed', 'd_see1',
// and 'd_see2', which have to be member variables of the hashing object rather
// than automatic local variables.  Hashing 48 bytes at a time is always done
// by the private function 'process48ByteSection'.
//
// If a very large segment, many times 48 bytes long, is hashed with a single
// 'operator()' call it is desirable to hash these 48-byte sections of it
// without copying them to the buffer, which is why the private function
// 'process48ByteSection' takes a pointer to the 48-byte segment to be
// processed, rather than assuming that the data is in the buffer.
//
// The biggest difficulty in this was, if the total length was over 48, we do
// the final unconditional hash of the last 16 bytes of everything at the end
// of 'computeHash' (see the 4th step within '3' above), which, if
// 'process48ByteSection' was called on data outside 'd_repeatBuffer', may no
// longer be accessible when 'computeHash' is called.  So we extend another 16
// bytes before 'd_repeatBuffer' in another sub-buffer, the 'prePad', into
// which we copy the last data from a 'process48ByteSection' if it had been
// called on data outside the buffer.  We keep a boolean, 'd_last16AtEnd'
// indicating whether the last 16 bytes of 'd_repeatBuffer' contain the data
// preceding the data in the beginning of 'd_repeatBuffer', in which case it
// has not been necessary to propagate that data to the 'prePad'.
//
// It works out that the first byte of the 'prePad' is never used, so we put
// the boolean 'd_last16AtEnd' in that location and shorten 'd_buffer' by 1
// byte.  This keeps the 'prePad' and the beginning and end of 'd_repeatBuffer'
// all 8-byte aligned, which will result in faster hashing if the data that
// comes in through 'operator()' is in 4 and 8 byte data fields (often the
// case), since memory accesses won't be split across memory word boundaries.

namespace BloombergLP {
namespace bslh {

}  // close package namespace
}  // close enterprise namespace

// ----------------------------------------------------------------------------
// Copyright 2022 Bloomberg Finance L.P.
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
