// bdlde_sha2.t.cpp                                                   -*-C++-*-
#include <bdlde_sha2.h>

#include <bslim_testutil.h>

#include <bsls_assert.h>
#include <bsls_asserttest.h>
#include <bsls_types.h>

#include <bsl_algorithm.h>
#include <bsl_cstdlib.h>
#include <bsl_cstring.h>
#include <bsl_iostream.h>
#include <bsl_limits.h>
#include <bsl_vector.h>

using namespace BloombergLP;
using namespace bsl;

//=============================================================================
//                                  TEST PLAN
//-----------------------------------------------------------------------------
//                                  Overview
//                                  --------
// The various SHA hash types are mechanisms that are used to provide
// cryptographic hashes.  The primary concern is to ensure that the correct
// hashes are calculated.
//-----------------------------------------------------------------------------
// [ 2] bdlde::Sha224::Sha224();
// [ 2] bdlde::Sha224::update(message, size);
// [ 6] bdlde::Sha224::finalize(digest);
// [ 3] bdlde::Sha256::Sha256();
// [ 3] bdlde::Sha256::update(message, size);
// [ 7] bdlde::Sha256::finalize(digest);
// [ 4] bdlde::Sha384::Sha384();
// [ 4] bdlde::Sha384::update(message, size);
// [ 8] bdlde::Sha384::finalize(digest);
// [ 5] bdlde::Sha512::Sha512();
// [ 5] bdlde::Sha512::update(message, size);
// [ 9] bdlde::Sha512::finalize(digest);
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST

//=============================================================================
//                     STANDARD BDE ASSERT TEST FUNCTION
//-----------------------------------------------------------------------------

namespace {

int testStatus = 0;

void aSsErT(int c, const char *s, int i)
{
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (0 <= testStatus && testStatus <= 100) ++testStatus;
    }
}

}  // close unnamed namespace

//=============================================================================
//               STANDARD BDE TEST DRIVER MACRO ABBREVIATIONS
//-----------------------------------------------------------------------------

#define ASSERT       BSLIM_TESTUTIL_ASSERT
#define LOOP_ASSERT  BSLIM_TESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLIM_TESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLIM_TESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLIM_TESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLIM_TESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLIM_TESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLIM_TESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLIM_TESTUTIL_LOOP6_ASSERT
#define ASSERTV      BSLIM_TESTUTIL_ASSERTV

#define Q            BSLIM_TESTUTIL_Q   // Quote identifier literally.
#define P            BSLIM_TESTUTIL_P   // Print identifier and value.
#define P_           BSLIM_TESTUTIL_P_  // P(X) without '\n'.
#define T_           BSLIM_TESTUTIL_T_  // Print a tab (w/o newline).
#define L_           BSLIM_TESTUTIL_L_  // current Line number

// ============================================================================
//                  NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)
#define ASSERT_PASS(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
#define ASSERT_FAIL(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)
#define ASSERT_OPT_PASS(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS(EXPR)
#define ASSERT_OPT_FAIL(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL(EXPR)

//=============================================================================
//                               GLOBAL TYPEDEF
//-----------------------------------------------------------------------------

//=============================================================================
//                   HELPER CLASSES AND FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

template<bsl::size_t SIZE>
void toHex(bsl::string *output, const unsigned char (&input)[SIZE])
    // Store into the specified 'output' the hex representation of the bytes in
    // the specified 'input'.
{
    const char *hexTable = "0123456789abcdef";
    output->clear();
    for (bsl::size_t index = 0; index != SIZE; ++index) {
        const unsigned char byte = input[index];
        output->push_back(hexTable[byte / 16]);
        output->push_back(hexTable[byte % 16]);
    }
}

template<class VALUE, bsl::size_t SIZE>
bsl::size_t arraySize(const VALUE (&)[SIZE])
    // Return the size of the array parameter.
{
    return SIZE;
}

template<class HASHER>
void testKnownHashes(const char *const (&expected)[6])
    // Hash certain known messages using an instance of 'HASHER', and verify
    // that the results match the results in 'expected'.
{
    const bsl::string repeat_a(1000000, 'a');
    bsl::string       all_characters;
    for (int c = -128; c != 128; ++c)
    {
        all_characters.push_back(static_cast<char>(c));
    }

    const bsl::string messages[6] =
    {
        "",
        "abc",
        "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq",
        "abcdefghbcdefghicdefghijdefghijkefghijklfghijklmghijklmnhijklmnoijklm"
            "nopjklmnopqklmnopqrlmnopqrsmnopqrstnopqrstu",
        repeat_a,
        all_characters
    };

    unsigned char digest[HASHER::k_DIGEST_SIZE];
    bsl::string   hexDigest;
    for (bsl::size_t n = 0; n != arraySize(messages); ++n)
    {
        const char *message = messages[n].c_str();
        HASHER      hasher;
        hasher.update(message,
                      messages[n].size());
        hasher.finalize(digest);

        toHex(&hexDigest, digest);
        ASSERT(hexDigest == expected[n]);
    }
}

template<class HASHER>
void testIncremental()
    // Verify that an instance of the specified 'HASHER' produces the same hash
    // when hashing an entire message as it does when hashing the message in
    // pieces.
{
    const char       *in         = "abcdef";
    const bsl::size_t size       = bsl::strlen(in);
    const bsl::size_t digestSize = HASHER::k_DIGEST_SIZE;
    unsigned char     digest1[digestSize];
    unsigned char     digest2[digestSize];

    HASHER completeHasher;
    completeHasher.update(in, size);
    completeHasher.finalize(digest1);

    for (bsl::size_t stride = 1; stride <= 3; ++stride) {
        ASSERT(size % stride == 0);
        HASHER incrementalHasher;
        for (bsl::size_t index = 0; index != size; index += stride) {
            incrementalHasher.update(in + index, stride);
        }
        incrementalHasher.finalize(digest2);

        ASSERT(bsl::equal(digest1, digest1 + digestSize, digest2));
    }
}

//=============================================================================
//                                USAGE EXAMPLE
//-----------------------------------------------------------------------------

///Usage
///-----
// This section illustrates intended use of this component.

//=============================================================================
//                                MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int        test = argc > 1 ? bsl::atoi(argv[1]) : 0;
    int     verbose = argc > 2;
    int veryVerbose = argc > 3;

    cout << "TEST " << __FILE__ << " CASE " << test << '\n';

    switch (test) { case 0:
      case 9: {
        // --------------------------------------------------------------------
        // KNOWN HASHES FOR SHA-512
        //   Ensure that values hash to their expected hashes
        //
        // Concerns:
        //: 1 Hashing a particular value provides the expected hash output when
        //:   using either the class interface or the function interface.
        //
        // Plan:
        //: 1 Verify a range of values with varying sizes and bytes against
        //:   known values.
        // --------------------------------------------------------------------
        if (verbose) cout << "KNOWN HASHES FOR SHA-512" "\n"
                             "========================" "\n";
        const char *const results[6] =
        {
            "cf83e1357eefb8bdf1542850d66d8007d620e4050b5715dc83f4a921d36ce9ce"
            "47d0d13c5d85f2b0ff8318d2877eec2f63b931bd47417a81a538327af927da3e",
            "ddaf35a193617abacc417349ae20413112e6fa4e89a97ea20a9eeee64b55d39a"
            "2192992a274fc1a836ba3c23a3feebbd454d4423643ce80e2a9ac94fa54ca49f",
            "204a8fc6dda82f0a0ced7beb8e08a41657c16ef468b228a8279be331a703c335"
            "96fd15c13b1b07f9aa1d3bea57789ca031ad85c7a71dd70354ec631238ca3445",
            "8e959b75dae313da8cf4f72814fc143f8f7779c6eb9f7fa17299aeadb6889018"
            "501d289e4900f7e4331b99dec4b5433ac7d329eeb6dd26545e96e55b874be909",
            "e718483d0ce769644e2e42c7bc15b4638e1f98b13b2044285632a803afa973eb"
            "de0ff244877ea60a4cb0432ce577c31beb009c5c2c49aa2e4eadb217ad8cc09b",
            "f91a8584486a5f167ca103e390444e52fd294e10d43af7bd94402876954ae9b1"
            "d0ec65ab9aaf47a7ab7f8733a8d111c038ff78d1238e3aa32b58e9b63767f7d3"
        };
        testKnownHashes<bdlde::Sha512>(results);
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // KNOWN HASHES FOR SHA-384
        //   Ensure that values hash to their expected hashes
        //
        // Concerns:
        //: 1 Hashing a particular value provides the expected hash output when
        //:   using either the class interface or the function interface.
        //
        // Plan:
        //: 1 Verify a range of values with varying sizes and bytes against
        //:   known values.
        // --------------------------------------------------------------------
        if (verbose) cout << "KNOWN HASHES FOR SHA-384" "\n"
                             "========================" "\n";
        const char *const results[6] =
        {
            "38b060a751ac96384cd9327eb1b1e36a21fdb71114be0743"
            "4c0cc7bf63f6e1da274edebfe76f65fbd51ad2f14898b95b",
            "cb00753f45a35e8bb5a03d699ac65007272c32ab0eded163"
            "1a8b605a43ff5bed8086072ba1e7cc2358baeca134c825a7",
            "3391fdddfc8dc7393707a65b1b4709397cf8b1d162af05ab"
            "fe8f450de5f36bc6b0455a8520bc4e6f5fe95b1fe3c8452b",
            "09330c33f71147e83d192fc782cd1b4753111b173b3b05d2"
            "2fa08086e3b0f712fcc7c71a557e2db966c3e9fa91746039",
            "9d0e1809716474cb086e834e310a4a1ced149e9c00f24852"
            "7972cec5704c2a5b07b8b3dc38ecc4ebae97ddd87f3d8985",
            "a7902aa7f28885d54c4dadbff0f721cd5532b1e56e6f7a4b"
            "b2baad0229e576da5902c1bf0cc809fa3efa6e6476e62696"
        };
        testKnownHashes<bdlde::Sha384>(results);
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // KNOWN HASHES FOR SHA-256
        //   Ensure that values hash to their expected hashes
        //
        // Concerns:
        //: 1 Hashing a particular value provides the expected hash output when
        //:   using either the class interface or the function interface.
        //
        // Plan:
        //: 1 Verify a range of values with varying sizes and bytes against
        //:   known values.
        // --------------------------------------------------------------------
        if (verbose) cout << "KNOWN HASHES FOR SHA-256" "\n"
                             "========================" "\n";
        const char *const results[6] =
        {
            "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855",
            "ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad",
            "248d6a61d20638b8e5c026930c3e6039a33ce45964ff2167f6ecedd419db06c1",
            "cf5b16a778af8380036ce59e7b0492370b249b11e8f07a51afac45037afee9d1",
            "cdc76e5c9914fb9281a1c7e284d73e67f1809a48a497200e046d39ccc7112cd0",
            "2bae3a9530e35152c19d73f13f6c0e22cb92f22ce8aa895796711f52b8f7f516"
        };
        testKnownHashes<bdlde::Sha256>(results);
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // KNOWN HASHES FOR SHA-224
        //   Ensure that values hash to their expected hashes
        //
        // Concerns:
        //: 1 Hashing a particular value provides the expected hash output when
        //:   using either the class interface or the function interface.
        //
        // Plan:
        //: 1 Verify a range of values with varying sizes and bytes against
        //:   known values.
        // --------------------------------------------------------------------
        if (verbose) cout << "KNOWN HASHES FOR SHA-224" "\n"
                             "========================" "\n";
        const char *const results[6] =
        {
            "d14a028c2a3a2bc9476102bb288234c415a2b01f828ea62ac5b3e42f",
            "23097d223405d8228642a477bda255b32aadbce4bda0b3f7e36c9da7",
            "75388b16512776cc5dba5da1fd890150b0c6455cb4f58b1952522525",
            "c97ca9a559850ce97a04a96def6d99a9e0e0e2ab14e6b8df265fc0b3",
            "20794655980c91d8bbb4c1ea97618a4bf03f42581948b2ee4ee7ad67",
            "5dfb7c35156ef5385de415bd1e04379c2fe5b7a002484d73730d4661"
        };

        testKnownHashes<bdlde::Sha224>(results);
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // INCREMENTAL UPDATES FOR SHA-512
        //   Test the update function
        //
        // Concerns:
        //: 1 Ensure that providing data one byte at a time is the same as
        //:   providing it all at once.
        //
        // Plan:
        //: 1 Pass each byte of a string to update and compare the result of
        //    passing the entire string to update.
        // --------------------------------------------------------------------
        if (verbose) cout << "INCREMENTAL UPDATES FOR SHA-512" "\n"
                             "===============================" "\n";
        testIncremental<bdlde::Sha512>();
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // INCREMENTAL UPDATES FOR SHA-384
        //   Test the update function
        //
        // Concerns:
        //: 1 Ensure that providing data one byte at a time is the same as
        //:   providing it all at once.
        //
        // Plan:
        //: 1 Pass each byte of a string to update and compare the result of
        //    passing the entire string to update.
        // --------------------------------------------------------------------
        if (verbose) cout << "INCREMENTAL UPDATES FOR SHA-384" "\n"
                             "===============================" "\n";
        testIncremental<bdlde::Sha384>();
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // INCREMENTAL UPDATES FOR SHA-256
        //   Test the update function
        //
        // Concerns:
        //: 1 Ensure that providing data one byte at a time is the same as
        //:   providing it all at once.
        //
        // Plan:
        //: 1 Pass each byte of a string to update and compare the result of
        //    passing the entire string to update.
        // --------------------------------------------------------------------
        if (verbose) cout << "INCREMENTAL UPDATES FOR SHA-256" "\n"
                             "===============================" "\n";
        testIncremental<bdlde::Sha256>();
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // INCREMENTAL UPDATES FOR SHA-224
        //   Test the update function
        //
        // Concerns:
        //: 1 Ensure that providing data one byte at a time is the same as
        //:   providing it all at once.
        //
        // Plan:
        //: 1 Pass each byte of a string to update and compare the result of
        //    passing the entire string to update.
        // --------------------------------------------------------------------
        if (verbose) cout << "INCREMENTAL UPDATES FOR SHA-224" "\n"
                             "===============================" "\n";
        testIncremental<bdlde::Sha224>();
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //   This case exercises (but does not fully test) basic functionality.
        //
        // Concerns:
        //: 1 The class is sufficiently functional to enable comprehensive
        //:   testing in subsequent test cases.
        //
        // Plan:
        //: 1 Verify different hashes are produced for different bytes.
        //: 2 Verify the same hashes are produced for the same bytes.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) cout << "\nBREATHING TEST"
                             "\n==============\n";
        {
            bdlde::Sha224 hasher;
        }
        {
            bdlde::Sha256 hasher;
        }
        {
            bdlde::Sha384 hasher;
        }
        {
            bdlde::Sha512 hasher;
        }
      } break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        cerr << "Error, non-zero test status = " << testStatus << "." << endl;
    }
    return testStatus;
}

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
