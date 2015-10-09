// bslalg_hashutil.h                                                  -*-C++-*-
#ifndef INCLUDED_BSLALG_HASHUTIL
#define INCLUDED_BSLALG_HASHUTIL

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a utility of hash functions.
//
//@CLASSES:
//  bslalg::HashUtil: utility for hash functions
//
//@DESCRIPTION: This component provides a namespace class, 'HashUtil', for
// hash functions.  At the current time it has one hash function,
// 'HashUtil::computeHash', which will hash most fundamental types, and
// pointers, rapidly.  Note that when a pointer is passed, only the bits in the
// pointer itself are hashed, the memory the pointer refers to is not examined.
//
///Usage
///-----
// This section illustrates intended usage of this component.
//
///Example 1: Dumping Out Hash Values
/// - - - - - - - - - - - - - - - - -
// Suppose we want to analyze our hash function by seeing how it distributes
// integers across buckets.   We will declare 64 buckets, and distribute hits
// among the bucket by indexing them with the low order 6 bits of the hash.
// Then we will display the distribution of hits in each bucket, to see if
// the hash function is distributing them evenly.
//..
//  int buckets[64];
//..
// First, we hash on the values of i in the range '[ 0, 1 << 15 )':
//..
//  {
//      memset(buckets, 0, sizeof(buckets));
//      for (int i = 0; i < (1 << 15); ++i) {
//          native_std::size_t hash = bslalg::HashUtil::computeHash(i);
//
//          ++buckets[hash & 63];
//      }
//      if (verbose) printf("Straight hash:\n");
//      int col = 0;
//      for (int i = 0; i < 64; ++i) {
//          if (verbose) printf("%s%5d", (0 == col ? "    " : ", "),
//                                                            buckets[i]);
//          ++col;
//          if (8 == col) {
//              col = 0;
//              if (verbose) printf("\n");
//          }
//      }
//  }
//..
// Then, we will hash on the values of '4 * i' for i in the range
// '[ 0, 1 << 15 )'.  This is interesting because pointers will often be 4-byte
// aligned and have the 2 low-order bits always zero, so this will be a
// simulation of that:
//..
//  {
//      memset(buckets, 0, sizeof(buckets));
//      for (int i = 0; i < (1 << 15); ++i) {
//          native_std::size_t hash = bslalg::HashUtil::computeHash(4 * i);
//
//          ++buckets[hash & 63];
//      }
//      if (verbose) printf("\nStraight * 4 hash:\n");
//      int col = 0;
//      for (int i = 0; i < 64; ++i) {
//          if (verbose) printf("%s%5d", (0 == col ? "    " : ", "),
//                                                            buckets[i]);
//          ++col;
//          if (8 == col) {
//              col = 0;
//              if (verbose) printf("\n");
//          }
//      }
//  }
//..
// Next, we will xor the bottom 30 bits of the hash into the bottom 6 bits, so
// we'll be observing more of the whole word:
//..
//  {
//      memset(buckets, 0, sizeof(buckets));
//      for (int i = 0; i < (1 << 15); ++i) {
//          native_std::size_t hash = bslalg::HashUtil::computeHash(i);
//          hash = hash ^ (hash >> 6) ^ (hash >> 12) ^ (hash >> 18) ^
//                        (hash >> 24);
//
//          ++buckets[hash & 63];
//      }
//      if (verbose) printf("\nFolded hash:\n");
//      int col = 0;
//      for (int i = 0; i < 64; ++i) {
//          if (verbose) printf("%s%5d", (0 == col ? "    " : ", "),
//                                                            buckets[i]);
//          ++col;
//          if (8 == col) {
//              col = 0;
//              if (verbose) printf("\n");
//          }
//      }
//  }
//..
// Now, bear in mind that an identity hash will perform very optimally on the
// first and third tests we did.  This time we will take the difference between
// the current hash and the previous one, a test for which the identity
// function would perform abominably:
//..
//  {
//      memset(buckets, 0, sizeof(buckets));
//      native_std::size_t prev = 0;
//      for (int i = 0; i < (1 << 15); ++i) {
//          native_std::size_t hash = bslalg::HashUtil::computeHash(i);
//
//          ++buckets[(hash - prev) & 63];
//          prev = hash;
//      }
//      if (verbose) printf("\nDiff hash:\n");
//      int col = 0;
//      for (int i = 0; i < 64; ++i) {
//          if (verbose) printf("%s%5d", (0 == col ? "    " : ", "),
//                                                            buckets[i]);
//          ++col;
//          if (8 == col) {
//              col = 0;
//              if (verbose) printf("\n");
//          }
//      }
//  }
//..
// Finally, take the difference between the previous hash and the current one,
// only this time, instead of subtracting, take a bitwise xor:
//..
//  {
//      memset(buckets, 0, sizeof(buckets));
//      native_std::size_t prev = 0;
//      for (int i = 0; i < (1 << 15); ++i) {
//          native_std::size_t hash = bslalg::HashUtil::computeHash(i);
//
//          ++buckets[(hash ^ prev) & 63];
//          prev = hash;
//      }
//      if (verbose) printf("\nXor diff hash:\n");
//      int col = 0;
//      for (int i = 0; i < 64; ++i) {
//          if (verbose) printf("%s%5d", (0 == col ? "    " : ", "),
//                                                            buckets[i]);
//          ++col;
//          if (8 == col) {
//              col = 0;
//              if (verbose) printf("\n");
//          }
//      }
//  }
//..
// The output produced by this usage example follows:
//..
//  Straight hash:
//        508,   501,   511,   502,   522,   524,   515,   500
//        501,   519,   523,   520,   495,   514,   540,   497
//        500,   523,   525,   518,   491,   515,   527,   509
//        513,   500,   511,   520,   487,   515,   505,   520
//        519,   516,   505,   534,   507,   514,   522,   517
//        538,   514,   510,   510,   531,   491,   513,   506
//        515,   497,   504,   496,   541,   508,   501,   523
//        501,   523,   485,   492,   517,   510,   503,   534
//
//  Straight * 4 hash:
//        513,   512,   493,   517,   514,   472,   501,   527
//        528,   504,   527,   507,   516,   494,   534,   514
//        517,   500,   513,   533,   507,   499,   511,   540
//        492,   518,   530,   522,   503,   522,   505,   494
//        520,   492,   490,   508,   538,   560,   522,   487
//        521,   516,   493,   491,   532,   504,   497,   530
//        495,   534,   537,   504,   487,   525,   533,   497
//        510,   499,   511,   506,   523,   512,   498,   517
//
//  Folded hash:
//        537,   493,   517,   544,   501,   508,   535,   528
//        502,   530,   536,   541,   500,   475,   540,   510
//        521,   513,   501,   525,   497,   511,   521,   513
//        522,   523,   479,   479,   508,   490,   507,   523
//        577,   490,   520,   514,   493,   465,   468,   511
//        518,   544,   503,   484,   550,   514,   517,   500
//        510,   501,   542,   528,   517,   456,   513,   530
//        518,   484,   510,   506,   522,   477,   563,   493
//
//  Diff hash:
//        329,   329,   526,   660,   755,   726,   466,   398
//        235,   410,   424,   713,   695,   714,   535,   314
//        342,   274,   598,   662,   711,   772,   498,   463
//        268,   360,   399,   655,   672,   694,   584,   311
//        404,   282,   601,   678,   660,   661,   418,   377
//        245,   425,   462,   762,   682,   627,   588,   295
//        369,   331,   529,   712,   659,   688,   418,   357
//        238,   387,   467,   687,   730,   695,   540,   302
//
//  Xor diff hash:
//        329,   142,   535,   475,   781,   800,   567,   510
//        258,   258,   378,   549,   664,   906,   477,   496
//        298,   118,   554,   467,   765,   883,   576,   418
//        233,   226,   375,   608,   633,   903,   422,   388
//        404,   133,   648,   412,   658,   896,   521,   459
//        258,   256,   383,   601,   699,   836,   511,   599
//        413,   169,   660,   450,   658,   826,   524,   560
//        237,   255,   383,   573,   706,   834,   539,   715
//..

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLS_NATIVESTD
#include <bsls_nativestd.h>
#endif

#ifndef INCLUDED_BSLS_TYPES
#include <bsls_types.h>
#endif

namespace BloombergLP {

namespace bslalg {
                        // ===============
                        // struct HashUtil
                        // ===============

struct HashUtil {
    // This 'struct' provides a namespace for hash functions.

    // CLASS METHODS
    static native_std::size_t computeHash(char key);
    static native_std::size_t computeHash(signed char key);
    static native_std::size_t computeHash(unsigned char key);
    static native_std::size_t computeHash(short key);
    static native_std::size_t computeHash(unsigned short key);
    static native_std::size_t computeHash(int key);
    static native_std::size_t computeHash(unsigned int key);
    static native_std::size_t computeHash(long key);
    static native_std::size_t computeHash(unsigned long key);
    static native_std::size_t computeHash(long long key);
    static native_std::size_t computeHash(unsigned long long key);
    static native_std::size_t computeHash(float key);
    static native_std::size_t computeHash(double key);
    static native_std::size_t computeHash(const void *key);
        // Return a 'size_t' hash value corresponding to the specified 'key'.
        // Note that the return value is seemingly random (i.e., the hash is
        // good) but identical on all platforms (irrespective of endianness).
        //
        // NOTE: We reserve the right to change these hash functions to return
        // different values.  The current implementation only returns a 32 bit
        // value -- when 'native_std::size_t' is 64 bits, the high-order 32
        // bits of the return value are all zero.  This is not a feature, it is
        // a bug that we will fix in a later release.
};

// ============================================================================
//                        INLINE FUNCTION DEFINITIONS
// ============================================================================

}  // close package namespace
}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2013 Bloomberg Finance L.P.
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
