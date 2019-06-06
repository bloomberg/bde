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
#include <bsl_sstream.h>
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

// The component under test is a value-semantic scalar class with internal
// state representing the value.  There is no allocator involved.  We are
// mostly concerned with the mechanical functioning of the various methods and
// free operators.
//
// The component interface represents a SHA-2 digest as 8 unsigned integers.
// The SHA-2 digest is calculated in the 'update' and 'loadDigest' functions
// based on a sequence of data bytes and the data length.  Note that the reason
// that the 'loadDigest' function is responsible for part of the calculation is
// specified in the implementation file (bdlde_sha2.cpp).  We need to verify
// that these functions calculates the checksum correctly.  Furthermore, we
// also need to verify that the 'loadDigest' function also returns a correct
// SHA-2 digest.
//
//       Primary Constructors, Primary Manipulators, and Basic Accessors
//       ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Primary Constructors:
//   A 'bdlde::Sha224' object is created with a default message length of 0 and
//   a digest of d14a028c2a3a2bc9476102bb288234c415a2b01f828ea62ac5b3e42f.  A
//   'bdlde::Sha256' object is created with a default message length of 0 and a
//   digest of
//   e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855.  A
//   'bdlde::Sha384' object is created with a default message length of 0 and a
//   digest of 38b060a751ac96384cd9327eb1b1e36a21fdb71114be0743
//   4c0cc7bf63f6e1da274edebfe76f65fbd51ad2f14898b95b.  A 'bdlde::Sha512'
//   object is created with a default message length of 0 and a digest of
//   cf83e1357eefb8bdf1542850d66d8007d620e4050b5715dc83f4a921d36ce9ce
//   47d0d13c5d85f2b0ff8318d2877eec2f63b931bd47417a81a538327af927da3e.  An
//   object's white-box state is represented by a buffer of unprocessed bytes,
//   the length of the message and also the current states.  The black-box
//   state is represented by a 224-bit digest for 'bdlde::Sha224', a 256-bit
//   digest for 'bdlde::Sha256', a 384-bit digest for 'bdlde::Sha384', and a
//   512-bit digest for 'bdlde::Sha512', which can all be modified using the
//   primary manipulator 'update'.  The default constructor, in conjunction
//   with the primary manipulator, is sufficient to attain any achievable
//   white-box state.
//
//    o bdlde::Sha224();
//    o bdlde::Sha256();
//    o bdlde::Sha384();
//    o bdlde::Sha512();
//
// Primary Manipulators:
//   The 'update' method comprises the minimal set of manipulators that can
//   attain any achievable white-box state.
//
//    o void update(const void *data, bsl::size_t length);
//
// Basic Accessors:
//   This is the maximal set of accessors that have direct contact with the
//   black-box representation of the object.  The 'loadDigest' method is an
//   obvious member of this set.  It appends the length of the current message
//   to a copy of the current SHA-2 object and calculates the digest.  The
//   'loadDigestAndReset' method is really a manipulator as it resets the
//   states within the object.  Therefore, it is not included in the set.
//
//    o void loadDigest(unsigned char *result) const;
//
//-----------------------------------------------------------------------------
// CREATORS
// [ 2] Sha224::Sha224();
// [ 3] Sha256::Sha256();
// [ 4] Sha384::Sha384();
// [ 5] Sha512::Sha512();
// [18] Sha224::Sha224(const void *data, bsl::size_t length);
// [19] Sha256::Sha256(const void *data, bsl::size_t length);
// [20] Sha384::Sha384(const void *data, bsl::size_t length);
// [21] Sha512::Sha512(const void *data, bsl::size_t length);
//
// MANIPULATORS
// [10] void Sha224::reset();
// [11] void Sha256::reset();
// [12] void Sha384::reset();
// [13] void Sha512::reset();
// [ 6] void Sha224::update(const void *message, bsl::size_t length);
// [ 7] void Sha256::update(const void *message, bsl::size_t length);
// [ 8] void Sha384::update(const void *message, bsl::size_t length);
// [ 9] void Sha512::update(const void *message, bsl::size_t length);
// [14] void Sha224::loadDigestAndReset(unsigned char *result);
// [15] void Sha256::loadDigestAndReset(unsigned char *result);
// [16] void Sha384::loadDigestAndReset(unsigned char *result);
// [17] void Sha512::loadDigestAndReset(unsigned char *result);
//
// ACCESSORS
// [ 2] void Sha224::loadDigest(unsigned char* digest);
// [ 3] void Sha256::loadDigest(unsigned char* digest);
// [ 4] void Sha384::loadDigest(unsigned char* digest);
// [ 5] void Sha512::loadDigest(unsigned char* digest);
// [22] bsl::ostream& Sha224::print(bsl::ostream& stream) const;
// [23] bsl::ostream& Sha256::print(bsl::ostream& stream) const;
// [24] bsl::ostream& Sha384::print(bsl::ostream& stream) const;
// [25] bsl::ostream& Sha512::print(bsl::ostream& stream) const;
//
// FREE OPERATORS
// [ 6] bool operator==(const Sha224& lhs, const Sha224& rhs);
// [ 7] bool operator==(const Sha256& lhs, const Sha256& rhs);
// [ 8] bool operator==(const Sha384& lhs, const Sha384& rhs);
// [ 9] bool operator==(const Sha512& lhs, const Sha512& rhs);
// [ 6] bool operator!=(const Sha224& lhs, const Sha224& rhs);
// [ 7] bool operator!=(const Sha256& lhs, const Sha256& rhs);
// [ 8] bool operator!=(const Sha384& lhs, const Sha384& rhs);
// [ 9] bool operator!=(const Sha512& lhs, const Sha512& rhs);
// [22] bsl::ostream& operator<<(bsl::ostream& stream, const Sha224& digest);
// [23] bsl::ostream& operator<<(bsl::ostream& stream, const Sha256& digest);
// [24] bsl::ostream& operator<<(bsl::ostream& stream, const Sha384& digest);
// [25] bsl::ostream& operator<<(bsl::ostream& stream, const Sha512& digest);
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [26] USAGE EXAMPLE
// [ *] CONCERN: This test driver is reusable w/other, similar components.
// [ *] CONCERN: In no case does memory come from the global allocator.
// [  ] CONCERN: All memory allocation is from the object's allocator.
// [  ] CONCERN: All memory allocation is exception neutral.
// [  ] CONCERN: Precondition violations are detected when enabled.

// ============================================================================
//                     STANDARD BDE ASSERT TEST FUNCTION
// ----------------------------------------------------------------------------

namespace {

int testStatus = 0;

void aSsErT(bool condition, const char *message, int line)
{
    if (condition) {
        cout << "Error " __FILE__ "(" << line << "): " << message
             << "    (failed)" << endl;

        if (0 <= testStatus && testStatus <= 100) {
            ++testStatus;
        }
    }
}

}  // close unnamed namespace

// ============================================================================
//               STANDARD BDE TEST DRIVER MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT       BSLIM_TESTUTIL_ASSERT
#define ASSERTV      BSLIM_TESTUTIL_ASSERTV

#define LOOP_ASSERT  BSLIM_TESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLIM_TESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLIM_TESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLIM_TESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLIM_TESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLIM_TESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLIM_TESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLIM_TESTUTIL_LOOP6_ASSERT

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
//                   GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

namespace {

bsl::string allCharacters()
{
    bsl::string result;
    for (int c = -128; c != 128; ++c)
    {
        result.push_back(static_cast<char>(c));
    }
    return result;
}

const bsl::string inputMessages[6] =
    // The FIPS-180 validation tests are made up of the middle four strings
    // (i.e., "abc", "abcbcd...opq", "abcdef...stu", and a string containing
    // 1,000,000 repetitions of the character 'a').  We add to this list the
    // empty string and a string containing all possible ASCII values, as these
    // cases represent possible interesting values.
{
    "",
    "abc",
    "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq",
    "abcdefghbcdefghicdefghijdefghijkefghijklfghijklmghijklmnhijklmnoijklm"
        "nopjklmnopqklmnopqrlmnopqrsmnopqrstnopqrstu",
    bsl::string(1000000, 'a'),
    allCharacters()
};

const char *const sha224Results[6] =
{
    "d14a028c2a3a2bc9476102bb288234c415a2b01f828ea62ac5b3e42f",
    "23097d223405d8228642a477bda255b32aadbce4bda0b3f7e36c9da7",
    "75388b16512776cc5dba5da1fd890150b0c6455cb4f58b1952522525",
    "c97ca9a559850ce97a04a96def6d99a9e0e0e2ab14e6b8df265fc0b3",
    "20794655980c91d8bbb4c1ea97618a4bf03f42581948b2ee4ee7ad67",
    "5dfb7c35156ef5385de415bd1e04379c2fe5b7a002484d73730d4661"
};

const char *const sha256Results[6] =
{
    "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855",
    "ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad",
    "248d6a61d20638b8e5c026930c3e6039a33ce45964ff2167f6ecedd419db06c1",
    "cf5b16a778af8380036ce59e7b0492370b249b11e8f07a51afac45037afee9d1",
    "cdc76e5c9914fb9281a1c7e284d73e67f1809a48a497200e046d39ccc7112cd0",
    "2bae3a9530e35152c19d73f13f6c0e22cb92f22ce8aa895796711f52b8f7f516"
};

const char *const sha384Results[6] =
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

const char *const sha512Results[6] =
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

//=============================================================================
//                   HELPER CLASSES AND FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

// ============================================================================
//                               USAGE EXAMPLE
// ----------------------------------------------------------------------------

///Usage
///-----
// In this section we show intended usage of this component.  The
// 'validatePassword' function below returns whether a specified password has a
// specified hash value.  The 'assertPasswordIsExpected' function below has a
// sample password to hash and a hash value that matches it.  Note that the
// output of 'loadDigest' is a binary representation.  When hashes are
// displayed for human consumption, they are typically converted to hex, but
// that would create unnecessary overhead here.
//..
bool validatePassword(const bsl::string&   password,
                      const bsl::string&   salt,
                      const unsigned char *expected)
    // Return 'true' if the specified 'password' concatenated with the
    // specified 'salt' has a SHA-512 hash equal to the specified
    // 'expected' and return 'false' otherwise.
{
    bdlde::Sha512 hasher;
    hasher.update(password.c_str(), password.length());
    hasher.update(salt.c_str(), salt.length());

    unsigned char digest[bdlde::Sha512::k_DIGEST_SIZE];
    hasher.loadDigest(digest);

    return bsl::equal(digest,
                      digest + bdlde::Sha512::k_DIGEST_SIZE,
                      expected);
}

void assertPasswordIsExpected()
    // Asserts that the constant string 'pass' salted with 'word' has the
    // expected hash value.  In a real application, the expected hash would
    // likely come from some sort of database.
{
    const bsl::string   password = "pass";
    const bsl::string   salt     = "word";
    const unsigned char expected[bdlde::Sha512::k_DIGEST_SIZE] = {
        0xB1, 0x09, 0xF3, 0xBB, 0xBC, 0x24, 0x4E, 0xB8, 0x24, 0x41, 0x91,
        0x7E, 0xD0, 0x6D, 0x61, 0x8B, 0x90, 0x08, 0xDD, 0x09, 0xB3, 0xBE,
        0xFD, 0x1B, 0x5E, 0x07, 0x39, 0x4C, 0x70, 0x6A, 0x8B, 0xB9, 0x80,
        0xB1, 0xD7, 0x78, 0x5E, 0x59, 0x76, 0xEC, 0x04, 0x9B, 0x46, 0xDF,
        0x5F, 0x13, 0x26, 0xAF, 0x5A, 0x2E, 0xA6, 0xD1, 0x03, 0xFD, 0x07,
        0xC9, 0x53, 0x85, 0xFF, 0xAB, 0x0C, 0xAC, 0xBC, 0x86
    };

    ASSERT(validatePassword(password, salt, expected));
}

// ============================================================================
//                    GLOBAL HELPER FUNCTIONS FOR TESTING
// ----------------------------------------------------------------------------

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
void testIncremental()
    // Verify that an instance of the specified 'HASHER' produces the same hash
    // when hashing an entire message as it does when hashing the message in
    // pieces.
{
    const char *inputs[] =
    {
        "abcdef",
        "abcdefghijkl"
    };
    for (bsl::size_t index = 0; index != arraySize(inputs); ++index)
    {
        const char       *input      = inputs[index];
        const bsl::size_t size       = bsl::strlen(input);
        const bsl::size_t digestSize = HASHER::k_DIGEST_SIZE;
        unsigned char     digest1[digestSize];
        unsigned char     digest2[digestSize];

        HASHER completeHasher;
        completeHasher.update(input, size);
        completeHasher.loadDigest(digest1);

        for (bsl::size_t stride = 1; stride <= 3; ++stride) {
            ASSERT(size % stride == 0);
            HASHER incrementalHasher;
            for (bsl::size_t index = 0; index != size; index += stride) {
                ASSERT(completeHasher != incrementalHasher);
                incrementalHasher.update(input + index, stride);
            }
            ASSERT(completeHasher == incrementalHasher);
            incrementalHasher.loadDigest(digest2);
            ASSERT(completeHasher == incrementalHasher);

            ASSERT(bsl::equal(digest1, digest1 + digestSize, digest2));
        }
    }
}

template<class HASHER>
void testKnownHashes(const char *const (&expected)[6])
    // Hash certain known messages using an instance of 'HASHER', and verify
    // that the results match the results in the specified 'expected'.
{
    unsigned char digest[HASHER::k_DIGEST_SIZE];
    bsl::string   hexDigest;
    for (bsl::size_t index = 0; index != arraySize(inputMessages); ++index)
    {
        const char *message = inputMessages[index].c_str();
        HASHER      hasher;
        hasher.update(message, inputMessages[index].size());
        hasher.loadDigest(digest);

        toHex(&hexDigest, digest);
        ASSERT(hexDigest == expected[index]);
    }
}

template<class HASHER, bsl::size_t LENGTH>
void testLoadDigestAndReset(const char (&message)[LENGTH])
    // Test the member function 'loadDigestAndReset' after updating the digest
    // with the specified 'message'.  The result can be verified to be correct
    // by using the result from 'loadDigest' as an oracle, since this function
    // has already been tested.
{
    HASHER digest1;
    HASHER digest2;
    ASSERT(digest1 == digest2);

    digest1.update(message, LENGTH);
    ASSERT(digest1 != digest2);

    unsigned char regularResult[HASHER::k_DIGEST_SIZE];
    unsigned char resetResult  [HASHER::k_DIGEST_SIZE];

    digest1.loadDigest(regularResult);
    digest1.loadDigestAndReset(resetResult);
    ASSERT(bsl::equal(regularResult,
                      regularResult + HASHER::k_DIGEST_SIZE,
                      resetResult));

    ASSERT(digest1 == digest2);
}

template<class HASHER>
void testPrinting(const char *const (&expected)[6])
    // Test that the member function 'print' and the stream operator ('<<')
    // both output the correct value when for each element of the specified
    // 'expected'.
{
    for (bsl::size_t index = 0; index != arraySize(inputMessages); ++index)
    {
        const bsl::string & message = inputMessages[index];
        const HASHER digest(message.c_str(), message.size());

        bsl::stringstream printStream;
        digest.print(printStream);
        ASSERT(printStream.str() == expected[index]);

        bsl::stringstream streamStream;
        streamStream << digest;
        ASSERT(streamStream.str() == expected[index]);
    }
}

template<class HASHER, bsl::size_t LENGTH>
void testReset(const char (&message)[LENGTH])
    // Test the member function 'reset' after updating the digest with the
    // specified 'message' and the specified 'length'.
{
    HASHER digest1;
    HASHER digest2;
    ASSERT(digest1 == digest2);

    digest1.update(message, LENGTH);
    ASSERT(digest1 != digest2);

    digest1.reset();
    ASSERT(digest1 == digest2);
}

template<class HASHER, bsl::size_t LENGTH>
void testTwoArgumentConstructor(const char (&message)[LENGTH])
    // Test the two-argument constructor accepting the specified 'message' and
    // the specified 'length'.
{
    HASHER digest1(message, LENGTH);
    HASHER digest2;
    digest2.update(message, LENGTH);

    ASSERT(digest1 == digest2);
}

}  // close unnamed namespace

//=============================================================================
//                                MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int        test = argc > 1 ? bsl::atoi(argv[1]) : 0;
    int     verbose = argc > 2;

    cout << "TEST " << __FILE__ << " CASE " << test << '\n';

    switch (test) { case 0:
      case 26: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE
        //   This will test the usage example provided in the component header
        //   file.
        //
        // Concerns:
        //   The usage example provided in the component header file must
        //   compile, link, and run on all platforms as shown.
        //
        // Plan:
        //   Run the usage example function 'assertPasswordIsExpected'.
        //
        // Testing:
        //   Usage example.
        // --------------------------------------------------------------------

        if (verbose) cout << "TESTING USAGE EXAMPLE" "\n"
                          << "=====================" "\n";

        assertPasswordIsExpected();
      } break;
      case 25: {
        // --------------------------------------------------------------------
        // TESTING PRINTING AND OUTPUT (<<) OPERATOR FOR SHA-512
        //
        // Concerns:
        //: 1 The member function 'print' outputs the digest that would be
        //:   obtained by a call to 'loadDigest' in hex format.
        //: 2 The stream operator ('<<') outputs the digest that would be
        //:   obtained by a call to 'loadDigest' in hex format.
        //
        // Plan:
        //: 1 Construct a digest from a given message with a known hash.
        //:   (C-1..2)
        //: 2 Construct a 'stringstream' object and pass it to print.  Compare
        //:   the result of the 'str()' member function with the expected
        //:   output.  (C-1)
        //: 3 Construct a 'stringstream' object and pass it to stream
        //:   operator.  Compare the result of the 'str()' member function with
        //:   the expected output.  (C-2)
        //
        // Testing:
        //   void print(bsl::ostream& stream);
        //   bsl::ostream& operator<<(bsl::ostream&, const Sha512&);
        // --------------------------------------------------------------------

        if (verbose) cout
               << "TESTING PRINTING AND OUTPUT (<<) OPERATOR FOR SHA-512" "\n"
               << "=====================================================" "\n";

        testPrinting<bdlde::Sha512>(sha512Results);
      } break;
      case 24: {
        // --------------------------------------------------------------------
        // TESTING PRINTING AND OUTPUT (<<) OPERATOR FOR SHA-384
        //
        // Concerns:
        //: 1 The member function 'print' outputs the digest that would be
        //:   obtained by a call to 'loadDigest' in hex format.
        //: 2 The stream operator ('<<') outputs the digest that would be
        //:   obtained by a call to 'loadDigest' in hex format.
        //
        // Plan:
        //: 1 Construct a digest from a given message with a known hash.
        //:   (C-1..2)
        //: 2 Construct a 'stringstream' object and pass it to print.  Compare
        //:   the result of the 'str()' member function with the expected
        //:   output.  (C-1)
        //: 3 Construct a 'stringstream' object and pass it to stream
        //:   operator.  Compare the result of the 'str()' member function with
        //:   the expected output.  (C-2)
        //
        // Testing:
        //   void print(bsl::ostream& stream);
        //   bsl::ostream& operator<<(bsl::ostream&, const Sha384&);
        // --------------------------------------------------------------------

        if (verbose) cout
               << "TESTING PRINTING AND OUTPUT (<<) OPERATOR FOR SHA-384" "\n"
               << "=====================================================" "\n";

        testPrinting<bdlde::Sha384>(sha384Results);
      } break;
      case 23: {
        // --------------------------------------------------------------------
        // TESTING PRINTING AND OUTPUT (<<) OPERATOR FOR SHA-256
        //
        // Concerns:
        //: 1 The member function 'print' outputs the digest that would be
        //:   obtained by a call to 'loadDigest' in hex format.
        //: 2 The stream operator ('<<') outputs the digest that would be
        //:   obtained by a call to 'loadDigest' in hex format.
        //
        // Plan:
        //: 1 Construct a digest from a given message with a known hash.
        //:   (C-1..2)
        //: 2 Construct a 'stringstream' object and pass it to print.  Compare
        //:   the result of the 'str()' member function with the expected
        //:   output.  (C-1)
        //: 3 Construct a 'stringstream' object and pass it to stream
        //:   operator.  Compare the result of the 'str()' member function with
        //:   the expected output.  (C-2)
        //
        // Testing:
        //   void print(bsl::ostream& stream);
        //   bsl::ostream& operator<<(bsl::ostream&, const Sha256&);
        // --------------------------------------------------------------------

        if (verbose) cout
               << "TESTING PRINTING AND OUTPUT (<<) OPERATOR FOR SHA-256" "\n"
               << "=====================================================" "\n";

        testPrinting<bdlde::Sha256>(sha256Results);
      } break;
      case 22: {
        // --------------------------------------------------------------------
        // TESTING PRINTING AND OUTPUT (<<) OPERATOR FOR SHA-224
        //
        // Concerns:
        //: 1 The member function 'print' outputs the digest that would be
        //:   obtained by a call to 'loadDigest' in hex format.
        //: 2 The stream operator ('<<') outputs the digest that would be
        //:   obtained by a call to 'loadDigest' in hex format.
        //
        // Plan:
        //: 1 Construct a digest from a given message with a known hash.
        //:   (C-1..2)
        //: 2 Construct a 'stringstream' object and pass it to print.  Compare
        //:   the result of the 'str()' member function with the expected
        //:   output.  (C-1)
        //: 3 Construct a 'stringstream' object and pass it to stream
        //:   operator.  Compare the result of the 'str()' member function with
        //:   the expected output.  (C-2)
        //
        // Testing:
        //   void print(bsl::ostream& stream);
        //   bsl::ostream& operator<<(bsl::ostream&, const Sha224&);
        // --------------------------------------------------------------------

        if (verbose) cout
               << "TESTING PRINTING AND OUTPUT (<<) OPERATOR FOR SHA-224" "\n"
               << "=====================================================" "\n";

        testPrinting<bdlde::Sha224>(sha224Results);
      } break;
      case 21: {
        // --------------------------------------------------------------------
        // TESTING 'Sha512(const void *data, bsl::size_t length)'
        //   This will test the two-argument constructor.
        //
        // Concerns:
        //: 1 We want to ensure that the two-argument constructor from data and
        //:   a length is equivalent to the default constructor followed by a
        //:   call to update.
        //
        // Plan:
        //: 1 Default construct a SHA-512 digest and call update with a given
        //:   message.  (C-1)
        //: 2 Construct a second SHA-512 digest directly from the data and size
        //:   of the message.  (C-1)
        //: 3 Assert that the two digests compare equal according to
        //:   'operator=='.  (C-1)
        //
        // Testing:
        //   Sha512(const void *data, bsl::size_t length);
        // --------------------------------------------------------------------

        if (verbose) cout
               << "TESTING 'Sha512(const void *data, bsl::size_t length)'" "\n"
               << "======================================================" "\n";

        testTwoArgumentConstructor<bdlde::Sha512>("a");
        testTwoArgumentConstructor<bdlde::Sha512>("a1");
        testTwoArgumentConstructor<bdlde::Sha512>("abc");
        testTwoArgumentConstructor<bdlde::Sha512>("a1c4");
        testTwoArgumentConstructor<bdlde::Sha512>("12345");
      } break;
      case 20: {
        // --------------------------------------------------------------------
        // TESTING 'Sha384(const void *data, bsl::size_t length)'
        //   This will test the two-argument constructor.
        //
        // Concerns:
        //: 1 We want to ensure that the two-argument constructor from data and
        //:   a length is equivalent to the default constructor followed by a
        //:   call to update.
        //
        // Plan:
        //: 1 Default construct a SHA-384 digest and call update with a given
        //:   message.  (C-1)
        //: 2 Construct a second SHA-384 digest directly from the data and size
        //:   of the message.  (C-1)
        //: 3 Assert that the two digests compare equal according to
        //:   'operator=='.  (C-1)
        //
        // Testing:
        //   Sha384(const void *data, bsl::size_t length);
        // --------------------------------------------------------------------

        if (verbose) cout
               << "TESTING 'Sha384(const void *data, bsl::size_t length)'" "\n"
               << "======================================================" "\n";

        testTwoArgumentConstructor<bdlde::Sha384>("a");
        testTwoArgumentConstructor<bdlde::Sha384>("a1");
        testTwoArgumentConstructor<bdlde::Sha384>("abc");
        testTwoArgumentConstructor<bdlde::Sha384>("a1c4");
        testTwoArgumentConstructor<bdlde::Sha384>("12345");
      } break;
      case 19: {
        // --------------------------------------------------------------------
        // TESTING 'Sha256(const void *data, bsl::size_t length)'
        //   This will test the two-argument constructor.
        //
        // Concerns:
        //: 1 We want to ensure that the two-argument constructor from data and
        //:   a length is equivalent to the default constructor followed by a
        //:   call to update.
        //
        // Plan:
        //: 1 Default construct a SHA-256 digest and call update with a given
        //:   message.  (C-1)
        //: 2 Construct a second SHA-256 digest directly from the data and size
        //:   of the message.  (C-1)
        //: 3 Assert that the two digests compare equal according to
        //:   'operator=='.  (C-1)
        //
        // Testing:
        //   Sha256(const void *data, bsl::size_t length);
        // --------------------------------------------------------------------

        if (verbose) cout
               << "TESTING 'Sha256(const void *data, bsl::size_t length)'" "\n"
               << "======================================================" "\n";

        testTwoArgumentConstructor<bdlde::Sha256>("a");
        testTwoArgumentConstructor<bdlde::Sha256>("a1");
        testTwoArgumentConstructor<bdlde::Sha256>("abc");
        testTwoArgumentConstructor<bdlde::Sha256>("a1c4");
        testTwoArgumentConstructor<bdlde::Sha256>("12345");
      } break;
      case 18: {
        // --------------------------------------------------------------------
        // TESTING 'Sha224(const void *data, bsl::size_t length)'
        //   This will test the two-argument constructor.
        //
        // Concerns:
        //: 1 We want to ensure that the two-argument constructor from data and
        //:   a length is equivalent to the default constructor followed by a
        //:   call to update.
        //
        // Plan:
        //: 1 Default construct a SHA-224 digest and call update with a given
        //:   message.  (C-1)
        //: 2 Construct a second SHA-224 digest directly from the data and size
        //:   of the message.  (C-1)
        //: 3 Assert that the two digests compare equal according to
        //:   'operator=='.  (C-1)
        //
        // Testing:
        //   Sha224(const void *data, bsl::size_t length);
        // --------------------------------------------------------------------

        if (verbose) cout
               << "TESTING 'Sha224(const void *data, bsl::size_t length)'" "\n"
               << "======================================================" "\n";

        testTwoArgumentConstructor<bdlde::Sha224>("a");
        testTwoArgumentConstructor<bdlde::Sha224>("a1");
        testTwoArgumentConstructor<bdlde::Sha224>("abc");
        testTwoArgumentConstructor<bdlde::Sha224>("a1c4");
        testTwoArgumentConstructor<bdlde::Sha224>("12345");
      } break;
      case 17: {
        // --------------------------------------------------------------------
        // TESTING 'loadDigestAndReset' FOR SHA-512
        //   This will test the 'loadDigestAndReset' method.
        //
        // Concerns:
        //: 1 We need to make sure that the resulting object after the call to
        //:   'loadDigestAndReset' contains the same value as a default object,
        //:   and the resulting output matches that which is given by
        //:   'loadDigest'.
        //
        // Plan:
        //: 1 Create a set of test data with varying lengths from 1 to 5.  For
        //:   each datum, create a 'bdlde::Sha512' object using the
        //:   fully-tested default constructor and 'update' member function.
        //:   Then call the 'loadDigestAndReset' member function and ensure
        //:   that the resulting object contains the same value as an object
        //:   created using the default constructor.  (C-1)
        //
        // Testing:
        //   void loadDigestAndReset();
        // --------------------------------------------------------------------

        if (verbose) cout << "TESTING 'loadDigestAndReset' FOR SHA-512" "\n"
                          << "========================================" "\n";

        testLoadDigestAndReset<bdlde::Sha512>("a");
        testLoadDigestAndReset<bdlde::Sha512>("a1");
        testLoadDigestAndReset<bdlde::Sha512>("abc");
        testLoadDigestAndReset<bdlde::Sha512>("a1c4");
        testLoadDigestAndReset<bdlde::Sha512>("12345");
      } break;
      case 16: {
        // --------------------------------------------------------------------
        // TESTING 'loadDigestAndReset' FOR SHA-384
        //   This will test the 'loadDigestAndReset' method.
        //
        // Concerns:
        //: 1 We need to make sure that the resulting object after the call to
        //:   'loadDigestAndReset' contains the same value as a default object,
        //:   and the resulting output matches that which is given by
        //:   'loadDigest'.
        //
        // Plan:
        //: 1 Create a set of test data with varying lengths from 1 to 5.  For
        //:   each datum, create a 'bdlde::Sha384' object using the
        //:   fully-tested default constructor and 'update' member function.
        //:   Then call the 'loadDigestAndReset' member function and ensure
        //:   that the resulting object contains the same value as an object
        //:   created using the default constructor.  (C-1)
        //
        // Testing:
        //   void loadDigestAndReset();
        // --------------------------------------------------------------------

        if (verbose) cout << "TESTING 'loadDigestAndReset' FOR SHA-384" "\n"
                          << "========================================" "\n";

        testLoadDigestAndReset<bdlde::Sha384>("a");
        testLoadDigestAndReset<bdlde::Sha384>("a1");
        testLoadDigestAndReset<bdlde::Sha384>("abc");
        testLoadDigestAndReset<bdlde::Sha384>("a1c4");
        testLoadDigestAndReset<bdlde::Sha384>("12345");
      } break;
      case 15: {
        // --------------------------------------------------------------------
        // TESTING 'loadDigestAndReset' FOR SHA-256
        //   This will test the 'loadDigestAndReset' method.
        //
        // Concerns:
        //: 1 We need to make sure that the resulting object after the call to
        //:   'loadDigestAndReset' contains the same value as a default object,
        //:   and the resulting output matches that which is given by
        //:   'loadDigest'.
        //
        // Plan:
        //: 1 Create a set of test data with varying lengths from 1 to 5.  For
        //:   each datum, create a 'bdlde::Sha256' object using the
        //:   fully-tested default constructor and 'update' member function.
        //:   Then call the 'loadDigestAndReset' member function and ensure
        //:   that the resulting object contains the same value as an object
        //:   created using the default constructor.  (C-1)
        //
        // Testing:
        //   void loadDigestAndReset();
        // --------------------------------------------------------------------

        if (verbose) cout << "TESTING 'loadDigestAndReset' FOR SHA-256" "\n"
                          << "========================================" "\n";

        testLoadDigestAndReset<bdlde::Sha256>("a");
        testLoadDigestAndReset<bdlde::Sha256>("a1");
        testLoadDigestAndReset<bdlde::Sha256>("abc");
        testLoadDigestAndReset<bdlde::Sha256>("a1c4");
        testLoadDigestAndReset<bdlde::Sha256>("12345");
      } break;
      case 14: {
        // --------------------------------------------------------------------
        // TESTING 'loadDigestAndReset' FOR SHA-224
        //   This will test the 'loadDigestAndReset' method.
        //
        // Concerns:
        //: 1 We need to make sure that the resulting object after the call to
        //:   'loadDigestAndReset' contains the same value as a default object,
        //:   and the resulting output matches that which is given by
        //:   'loadDigest'.
        //
        // Plan:
        //: 1 Create a set of test data with varying lengths from 1 to 5.  For
        //:   each datum, create a 'bdlde::Sha224' object using the
        //:   fully-tested default constructor and 'update' member function.
        //:   Then call the 'loadDigestAndReset' member function and ensure
        //:   that the resulting object contains the same value as an object
        //:   created using the default constructor.  (C-1)
        //
        // Testing:
        //   void loadDigestAndReset();
        // --------------------------------------------------------------------

        if (verbose) cout << "TESTING 'loadDigestAndReset' FOR SHA-224" "\n"
                          << "========================================" "\n";

        testLoadDigestAndReset<bdlde::Sha224>("a");
        testLoadDigestAndReset<bdlde::Sha224>("a1");
        testLoadDigestAndReset<bdlde::Sha224>("abc");
        testLoadDigestAndReset<bdlde::Sha224>("a1c4");
        testLoadDigestAndReset<bdlde::Sha224>("12345");
      } break;
      case 13: {
        // --------------------------------------------------------------------
        // TESTING 'reset' FOR SHA-512
        //   This will test the 'reset' method.
        //
        // Concerns:
        //: 1 We need to make sure that the resulting object after the call to
        //:   'reset' contains the same value as a default object.
        //
        // Plan:
        //: 1 Create a set of test data with varying lengths from 1 to 5.  For
        //:   each datum, create a 'bdlde::Sha512' object using the
        //:   fully-tested default constructor and 'update' member function.
        //:   Then call the 'reset' member function and ensure that the
        //:   resulting object contains the same value as an object created
        //:   using the default constructor.  (C-1)
        //
        // Testing:
        //   void reset();
        // --------------------------------------------------------------------

        if (verbose) cout << "TESTING 'reset' FOR SHA-512" "\n"
                          << "===========================" "\n";

        testReset<bdlde::Sha512>("a");
        testReset<bdlde::Sha512>("a1");
        testReset<bdlde::Sha512>("abc");
        testReset<bdlde::Sha512>("a1c4");
        testReset<bdlde::Sha512>("12345");
      } break;
      case 12: {
        // --------------------------------------------------------------------
        // TESTING 'reset' FOR SHA-384
        //   This will test the 'reset' method.
        //
        // Concerns:
        //: 1 We need to make sure that the resulting object after the call to
        //:   'reset' contains the same value as a default object.
        //
        // Plan:
        //: 1 Create a set of test data with varying lengths from 1 to 5.  For
        //:   each datum, create a 'bdlde::Sha384' object using the
        //:   fully-tested default constructor and 'update' member function.
        //:   Then call the 'reset' member function and ensure that the
        //:   resulting object contains the same value as an object created
        //:   using the default constructor.  (C-1)
        //
        // Testing:
        //   void reset();
        // --------------------------------------------------------------------

        if (verbose) cout << "TESTING 'reset' FOR SHA-384" "\n"
                          << "===========================" "\n";

        testReset<bdlde::Sha384>("a");
        testReset<bdlde::Sha384>("a1");
        testReset<bdlde::Sha384>("abc");
        testReset<bdlde::Sha384>("a1c4");
        testReset<bdlde::Sha384>("12345");
      } break;
      case 11: {
        // --------------------------------------------------------------------
        // TESTING 'reset' FOR SHA-256
        //   This will test the 'reset' method.
        //
        // Concerns:
        //: 1 We need to make sure that the resulting object after the call to
        //:   'reset' contains the same value as a default object.
        //
        // Plan:
        //: 1 Create a set of test data with varying lengths from 1 to 5.  For
        //:   each datum, create a 'bdlde::Sha256' object using the
        //:   fully-tested default constructor and 'update' member function.
        //:   Then call the 'reset' member function and ensure that the
        //:   resulting object contains the same value as an object created
        //:   using the default constructor.  (C-1)
        //
        // Testing:
        //   void reset();
        // --------------------------------------------------------------------

        if (verbose) cout << "TESTING 'reset' FOR SHA-256" "\n"
                          << "===========================" "\n";

        testReset<bdlde::Sha256>("a");
        testReset<bdlde::Sha256>("a1");
        testReset<bdlde::Sha256>("abc");
        testReset<bdlde::Sha256>("a1c4");
        testReset<bdlde::Sha256>("12345");
      } break;
      case 10: {
        // --------------------------------------------------------------------
        // TESTING 'reset' FOR SHA-224
        //   This will test the 'reset' method.
        //
        // Concerns:
        //: 1 We need to make sure that the resulting object after the call to
        //:   'reset' contains the same value as a default object.
        //
        // Plan:
        //: 1 Create a set of test data with varying lengths from 1 to 5.  For
        //:   each datum, create a 'bdlde::Sha224' object using the
        //:   fully-tested default constructor and 'update' member function.
        //:   Then call the 'reset' member function and ensure that the
        //:   resulting object contains the same value as an object created
        //:   using the default constructor.  (C-1)
        //
        // Testing:
        //   void reset();
        // --------------------------------------------------------------------

        if (verbose) cout << "TESTING 'reset' FOR SHA-224" "\n"
                          << "===========================" "\n";

        testReset<bdlde::Sha224>("a");
        testReset<bdlde::Sha224>("a1");
        testReset<bdlde::Sha224>("abc");
        testReset<bdlde::Sha224>("a1c4");
        testReset<bdlde::Sha224>("12345");
      } break;
      case 9: {
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
        //    passing the entire string to update.  (C-1)
        //
        // Testing:
        //   void Sha512::update(const void *message, bsl::size_t length);
        //   bool operator==(const Sha512& lhs, const Sha512& rhs);
        //   bool operator!=(const Sha512& lhs, const Sha512& rhs);
        // --------------------------------------------------------------------
        if (verbose) cout << "INCREMENTAL UPDATES FOR SHA-512" "\n"
                             "===============================" "\n";
        testIncremental<bdlde::Sha512>();
      } break;
      case 8: {
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
        //    passing the entire string to update.  (C-1)
        //
        // Testing:
        //   void Sha384::update(const void *message, bsl::size_t length);
        //   bool operator==(const Sha384& lhs, const Sha384& rhs);
        //   bool operator!=(const Sha384& lhs, const Sha384& rhs);
        // --------------------------------------------------------------------
        if (verbose) cout << "INCREMENTAL UPDATES FOR SHA-384" "\n"
                             "===============================" "\n";
        testIncremental<bdlde::Sha384>();
      } break;
      case 7: {
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
        //    passing the entire string to update.  (C-1)
        //
        // Testing:
        //   void Sha256::update(const void *message, bsl::size_t length);
        //   bool operator==(const Sha256& lhs, const Sha256& rhs);
        //   bool operator!=(const Sha256& lhs, const Sha256& rhs);
        // --------------------------------------------------------------------
        if (verbose) cout << "INCREMENTAL UPDATES FOR SHA-256" "\n"
                             "===============================" "\n";
        testIncremental<bdlde::Sha256>();
      } break;
      case 6: {
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
        //    passing the entire string to update.  (C-1)
        //
        // Testing:
        //   void Sha224::update(const void *message, bsl::size_t length);
        //   bool operator==(const Sha224& lhs, const Sha224& rhs);
        //   bool operator!=(const Sha224& lhs, const Sha224& rhs);
        // --------------------------------------------------------------------
        if (verbose) cout << "INCREMENTAL UPDATES FOR SHA-224" "\n"
                             "===============================" "\n";
        testIncremental<bdlde::Sha224>();
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // KNOWN HASHES FOR SHA-512
        //   Ensure that values hash to their expected hashes
        //
        // Concerns:
        //: 1 Hashing a particular value provides the expected hash output when
        //:   known values.  These known values came from running the Python
        //:   implementation of the SHA-2 algorithms.
        //
        // Plan:
        //: 1 Verify a range of values with varying sizes and bytes against
        //:   known values.  (C-1)
        //
        // Testing:
        //   Sha512::Sha512();
        //   void Sha512::update(const void *message, bsl::size_t length);
        //   void Sha512::loadDigest(unsigned char *digest);
        // --------------------------------------------------------------------
        if (verbose) cout << "KNOWN HASHES FOR SHA-512" "\n"
                             "========================" "\n";

        testKnownHashes<bdlde::Sha512>(sha512Results);
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // KNOWN HASHES FOR SHA-384
        //   Ensure that values hash to their expected hashes
        //
        // Concerns:
        //: 1 Hashing a particular value provides the expected hash output when
        //:   known values.  These known values came from running the Python
        //:   implementation of the SHA-2 algorithms.
        //
        // Plan:
        //: 1 Verify a range of values with varying sizes and bytes against
        //:   known values.  (C-1)
        //
        // Testing:
        //   Sha384::Sha384();
        //   void Sha384::update(const void *message, bsl::size_t length);
        //   void Sha384::loadDigest(unsigned char *digest);
        // --------------------------------------------------------------------
        if (verbose) cout << "KNOWN HASHES FOR SHA-384" "\n"
                             "========================" "\n";

        testKnownHashes<bdlde::Sha384>(sha384Results);
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // KNOWN HASHES FOR SHA-256
        //   Ensure that values hash to their expected hashes
        //
        // Concerns:
        //: 1 Hashing a particular value provides the expected hash output when
        //:   known values.  These known values came from running the Python
        //:   implementation of the SHA-2 algorithms.
        //
        // Plan:
        //: 1 Verify a range of values with varying sizes and bytes against
        //:   known values.  (C-1)
        //
        // Testing:
        //   Sha256::Sha256();
        //   void Sha256::update(const void *message, bsl::size_t length);
        //   void Sha256::loadDigest(unsigned char *digest);
        // --------------------------------------------------------------------
        if (verbose) cout << "KNOWN HASHES FOR SHA-256" "\n"
                             "========================" "\n";

        testKnownHashes<bdlde::Sha256>(sha256Results);
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // KNOWN HASHES FOR SHA-224
        //   Ensure that values hash to their expected hashes
        //
        // Concerns:
        //: 1 Hashing a particular value provides the expected hash output when
        //:   known values.  These known values came from running the Python
        //:   implementation of the SHA-2 algorithms.
        //
        // Plan:
        //: 1 Verify a range of values with varying sizes and bytes against
        //:   known values.  (C-1)
        //
        // Testing:
        //   Sha224::Sha224();
        //   void Sha224::update(const void *message, bsl::size_t length);
        //   void Sha224::loadDigest(unsigned char *digest);
        // --------------------------------------------------------------------
        if (verbose) cout << "KNOWN HASHES FOR SHA-224" "\n"
                             "========================" "\n";

        testKnownHashes<bdlde::Sha224>(sha224Results);
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
        //: 1 Verify default constructed hashes equal themselves.  (C-1)
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) cout << "\nBREATHING TEST"
                             "\n==============\n";
        {
            bdlde::Sha224 hasher;
            ASSERT(hasher == hasher);
        }
        {
            bdlde::Sha256 hasher;
            ASSERT(hasher == hasher);
        }
        {
            bdlde::Sha384 hasher;
            ASSERT(hasher == hasher);
        }
        {
            bdlde::Sha512 hasher;
            ASSERT(hasher == hasher);
        }
      } break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." "\n";
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        cerr << "Error, non-zero test status = " << testStatus << "." "\n";
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
