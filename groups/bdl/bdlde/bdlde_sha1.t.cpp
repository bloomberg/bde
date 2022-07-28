// bdlde_sha1.t.cpp                                                   -*-C++-*-
#include <bdlde_sha1.h>

#include <bslim_testutil.h>

#include <bsls_assert.h>
#include <bsls_asserttest.h>
#include <bsls_types.h>

#include <bsl_algorithm.h>
#include <bsl_cstdlib.h>
#include <bsl_cstring.h>
#include <bsl_iostream.h>
#include <bsl_iterator.h>
#include <bsl_string.h>
#include <bsl_sstream.h>

using namespace BloombergLP;
using namespace bsl;

//=============================================================================
//                                  TEST PLAN
//-----------------------------------------------------------------------------
//                                  Overview
//                                  --------
// The 'bdlde::Sha1' type is a mechanism that is used to provide the SHA-1
// hash.  The primary concern is to ensure that the correct hashes are
// calculated.
//
// The component under test is a value-semantic scalar class with internal
// state representing the value.  There is no allocator involved.  We are
// mostly concerned with the mechanical functioning of the various methods and
// free operators.
//
// The component interface represents a SHA-1 digest as 5 unsigned 32-bit
// integers.  The SHA-1 digest is calculated in the 'update' and 'loadDigest'
// functions based on a sequence of data bytes and the data length.  Note that
// the reason that the 'loadDigest' function is responsible for part of the
// calculation is specified in the implementation file (bdlde_sha1.cpp).  We
// need to verify that these functions calculates the checksum correctly.
// Furthermore, we also need to verify that the 'loadDigest' function also
// returns a correct SHA-1 digest.
//
//        Primary Constructor, Primary Manipulator, and Basic Accessor
//        ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Primary Constructor:
//   A 'bdlde::Sha1' object is created with a default message length of 0 and
//   a digest of da39a3ee5e6b4b0d3255bfef95601890afd80709.  An object's
//   white-box state is represented by a buffer of unprocessed bytes and the
//   length of the message.  The black-box state is represented by a 160-bit
//   digest, which can be modified using the primary manipulator 'update'.  The
//   default constructor, in conjunction with the primary manipulator, is
//   sufficient to attain any achievable white-box state.
//
//    o 'Sha1()'
//
// Primary Manipulator:
//   The 'update' method comprises the minimal set of manipulators that can
//   attain any achievable white-box state.
//
//    o 'void update(const void *data, bsl::size_t length);'
//
// Basic Accessor:
//   This is the maximal set of accessors that have direct contact with the
//   black-box representation of the object.  The 'loadDigest' method is an
//   obvious member of this set.  It appends the length of the current message
//   to a copy of the current SHA-1 object and calculates the digest.  The
//   'loadDigestAndReset' method is really a manipulator as it resets the
//   states within the object.  Therefore, it is not included in the set.
//
//    o 'void loadDigest(unsigned char *result) const;'
//
//-----------------------------------------------------------------------------
// CREATORS
// [ 2] Sha1::Sha1();
// [ 5] Sha1::Sha1(const void *data, bsl::size_t length);
//
// MANIPULATORS
// [ 3] void Sha1::reset();
// [ 2] void Sha1::update(const void *data, bsl::size_t length);
// [ 4] void Sha1::loadDigestAndReset(unsigned char *result);
//
// ACCESSORS
// [ 2] void Sha1::loadDigest(unsigned char *result) const;
// [ 6] bsl::ostream& Sha1::print(bsl::ostream& stream) const;
//
// FREE OPERATORS
// [ 2] bool operator==(const Sha1& lhs, const Sha1& rhs);
// [ 2] bool operator!=(const Sha1& lhs, const Sha1& rhs);
// [ 6] bsl::ostream& operator<<(bsl::ostream&, const Sha1&);
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 7] USAGE EXAMPLE
// [ *] CONCERN: This test driver is reusable w/other, similar components.
// [ *] CONCERN: In no case does memory come from the global allocator.

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

bool     verbose = false;
bool veryVerbose = false;

bsl::string allCharacters()
    // Return a string that contains every value of 'char' exactly once, sorted
    // in numerical order.
{
    bsl::string result;
    for (int c = -128; c != 128; ++c)
    {
        result.push_back(static_cast<char>(c));
    }
    return result;
}

int fromHexDigit(char c)
    // Return the value of the specified 'c' interpreted as a base 16 digit.
{
    if (c >= 'a' && c <= 'f') {
        return c - 'a' + 10;                                          // RETURN
    } else if (c >= 'A' && c <= 'F') {
        return c - 'A' + 10;                                          // RETURN
    } else {
        ASSERT(c >= '0' && c <= '9');
        return c - '0';                                               // RETURN
    }
}

bsl::string fromHex(const bsl::string_view& hexString)
    // Return a string whose characters have the values obtained from
    // interpreting each block of 2 characters from the specified 'hexString'
    // as a base 16 integer.
{
    ASSERT(hexString.length() % 2 == 0);
    bsl::string result;
    for (bsl::size_t i = 0; i < hexString.length(); i += 2) {
        const int hi = fromHexDigit(hexString[i]);
        const int lo = fromHexDigit(hexString[i + 1]);
        const int val = 16 * hi + lo;
        result.push_back(static_cast<char>(val <= CHAR_MAX ? val : val - 256));
    }
    return result;
}

const bsl::string inputMessages[6] =
    // The second, third, and fourth strings below (and their expected results)
    // are taken from NIST examples for SHA-1.  We add to this list some
    // additional cases whose hashes are obtained from the Linux command-line
    // tool 'sha1sum': the empty string and a string containing all possible
    // (as these cases represent possible interesting values), as well as a
    // very long string (consisting of 1 million repetitions of the character
    // 'a'.
{
    "",
    "abc",
    "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq",
    fromHex("7c9c67323a1df1adbfe5ceb415eaef0155ece2820f4d50c1ec22cba4928ac656"
            "c83fe585db6a78ce40bc42757aba7e5a3f582428d6ca68d0c3978336a6efb729"
            "613e8d9979016204bfd921322fdd5222183554447de5e6e9bbe6edf76d7b71e1"
            "8dc2e8d6dc89b7398364f652fafc734329aafa3dcd45d4f31e388e4fafd7fc64"
            "95f37ca5cbab7f54d586463da4bfeaa3bae09f7b8e9239d832b4f0a733aa609c"
            "c1f8d4"),
    bsl::string(1000000, 'a'),
    allCharacters()
};

const char *const sha1Results[6] =
{
    "da39a3ee5e6b4b0d3255bfef95601890afd80709",
    "a9993e364706816aba3e25717850c26c9cd0d89d",
    "84983e441c3bd26ebaae4aa1f95129e5e54670f1",
    "d8fd6a91ef3b6ced05b98358a99107c1fac8c807",
    "34aa973cd4c4daa4f61eeb2bdbad27316534016f",
    "eb38c997fb698b7e65330d45cbc28435f5e96bb8"
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
// that would create unnecessary overhead here.  Also note that because SHA-1
// digests are inexpensive to compute, they are vulnerable to brute force
// attacks and should not be used for password hashing in real-world
// applications.  A function like 'validatePassword' must only be used to
// validate passwords against previously computed SHA-1 hashes, and only during
// a transition period to a more secure password hashing function.
bool validatePassword(const bsl::string_view&  password,
                      const bsl::string_view&  salt,
                      const unsigned char     *expected)
    // Return 'true' if the specified 'password' concatenated with the
    // specified 'salt' has a SHA-1 hash equal to the specified 'expected', and
    // 'false' otherwise.
{
    bdlde::Sha1 hasher;
    hasher.update(password.data(), password.length());
    hasher.update(salt.data(), salt.length());

    unsigned char digest[bdlde::Sha1::k_DIGEST_SIZE];
    hasher.loadDigest(digest);

    return bsl::equal(bsl::begin(digest), bsl::end(digest), expected);
}

void assertPasswordIsExpected()
    // Asserts that the constant string 'pass' salted with 'word' has the
    // expected hash value.  In a real application, the expected hash would
    // likely come from some sort of database.
{
    const bsl::string   password = "pass";
    const bsl::string   salt     = "word";
    const unsigned char expected[bdlde::Sha1::k_DIGEST_SIZE] = {
        0x5B, 0xAA, 0x61, 0xE4, 0xC9, 0xB9, 0x3F, 0x3F, 0x06, 0x82,
        0x25, 0x0B, 0x6C, 0xF8, 0x33, 0x1B, 0x7E, 0xE6, 0x8F, 0xD8
    };

    ASSERT(validatePassword(password, salt, expected));
}

// ============================================================================
//                    GLOBAL HELPER FUNCTIONS FOR TESTING
// ----------------------------------------------------------------------------

template <bsl::size_t SIZE>
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

void testIncrementalSmall()
    // Verify that our 'Sha1' hasher produces the same hash when hashing a
    // small message as it does when hashing the message in pieces.
{
    const char *inputs[] =
    {
        "abcdef",
        "abcdefghijkl"
    };
    for (bsl::size_t index = 0; index != bsl::size(inputs); ++index)
    {
        const char       *input      = inputs[index];
        const bsl::size_t size       = bsl::strlen(input);
        const bsl::size_t digestSize = bdlde::Sha1::k_DIGEST_SIZE;
        unsigned char     digest1[digestSize];
        unsigned char     digest2[digestSize];

        if (veryVerbose) { T_ P(input) }

        bdlde::Sha1 completeHasher;
        completeHasher.update(input, size);
        completeHasher.loadDigest(digest1);

        for (bsl::size_t stride = 1; stride <= 3; ++stride) {
            ASSERT(size % stride == 0);
            if (veryVerbose) { T_ P_(input) P(stride) }
            bdlde::Sha1 incrementalHasher;
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

template <bsl::size_t NUMBER_OF_CHUNKS>
void testWithChunkSizes(
                   const bsl::string_view& input,
                   const unsigned char   (&digest)[bdlde::Sha1::k_DIGEST_SIZE],
                   const int             (&chunkSizes)[NUMBER_OF_CHUNKS])
    // Hash the specified 'input' incrementally using the specified
    // 'chunkSizes', and assert that the final result equals the specified
    // 'digest'.
{
    bdlde::Sha1 hasher;
    bsl::size_t charsProcessed = 0;
    for (bsl::size_t i = 0; i < NUMBER_OF_CHUNKS; i++) {
        const bsl::size_t chunkSize = chunkSizes[i];
        if (veryVerbose) { T_ P_(i) P_(charsProcessed) P(chunkSize) }
        ASSERT(charsProcessed + chunkSize <= input.size());
        if (chunkSize == 0) {
            // As a special case, test that 'update' behaves properly when
            // passed a null pointer.
            hasher.update(static_cast<void*>(0), 0);
        } else {
            hasher.update(input.data() + charsProcessed, chunkSize);
        }
        charsProcessed += chunkSize;
    }
    ASSERT(charsProcessed == input.size());
    unsigned char incrementalDigest[bdlde::Sha1::k_DIGEST_SIZE];
    hasher.loadDigest(incrementalDigest);
    ASSERT(bsl::equal(bsl::begin(digest),
                      bsl::end(digest),
                      bsl::begin(incrementalDigest)));
}

void testIncrementalLargeMultiple()
    // Verify that our 'Sha1' hasher produces the same hash when hashing a
    // long message (whose length is a multiple of the 64 byte block size) as
    // it does when hashing the message in pieces.
{
    // Generate a low-quality pseudorandom input string.
    bsl::string input;
    for (bsl::uint64_t x = 1;
         input.size() < 384;
         x = 1000000009 * x + 1) {
        const int hi = static_cast<int>(x >> 56);
        input.push_back(static_cast<char>(hi <= CHAR_MAX ? hi : hi - 256));
    }
    ASSERT(input.size() % 64 == 0);

    unsigned char digest[bdlde::Sha1::k_DIGEST_SIZE];
    bdlde::Sha1   completeHasher;
    completeHasher.update(input.data(), input.size());
    completeHasher.loadDigest(digest);

    const int chunkSizes[] = {0, 1, 0, 63, 64, 65, 64, 127};
    testWithChunkSizes(input, digest, chunkSizes);
}

void testIncrementalLargeNonMultiple()
    // Verify that our 'Sha1' hasher produces the same hash when hashing a
    // long message (whose length is *not* a multiple of the 64 byte block
    // size) as it does when hashing the message in pieces.
{
    // Generate a low-quality pseudorandom input string
    bsl::string input;
    for (bsl::uint64_t x = 1;
         input.size() < 480;
         x = 1000000009 * x + 1) {
        const int hi = static_cast<int>(x >> 56);
        input.push_back(static_cast<char>(hi <= CHAR_MAX ? hi : hi - 256));
    }
    ASSERT(input.size() % 64 != 0);

    unsigned char digest[bdlde::Sha1::k_DIGEST_SIZE];
    bdlde::Sha1   completeHasher;
    completeHasher.update(input.c_str(), input.size());
    completeHasher.loadDigest(digest);

    const int chunkSizes[] = {32, 128, 96, 96, 128};
    testWithChunkSizes(input, digest, chunkSizes);
}

void testKnownHashes()
    // Hash certain known messages using an instance of 'Sha1', and verify that
    // the results match the known expected results in 'sha1Results'.
{
    unsigned char digest[bdlde::Sha1::k_DIGEST_SIZE];
    bsl::string   hexDigest;
    ASSERT(bsl::size(inputMessages) == bsl::size(sha1Results));
    for (bsl::size_t index = 0; index != bsl::size(inputMessages); ++index)
    {
        if (veryVerbose) { T_ P(index) }
        const char *message = inputMessages[index].c_str();
        bdlde::Sha1 hasher;
        hasher.update(message, inputMessages[index].size());
        hasher.loadDigest(digest);

        toHex(&hexDigest, digest);
        ASSERT(hexDigest == sha1Results[index]);
    }
}

void testLoadDigestAndReset(const char *message)
    // Test the member function 'loadDigestAndReset' after updating the digest
    // with the specified 'message'.  The result can be verified to be correct
    // by using the result from 'loadDigest' as an oracle, since this function
    // has already been tested.
{
    const bsl::size_t length = bsl::strlen(message);
    bdlde::Sha1       digest1;
    bdlde::Sha1       digest2;
    ASSERT(digest1 == digest2);

    digest1.update(message, length);
    ASSERT(digest1 != digest2);

    unsigned char regularResult[bdlde::Sha1::k_DIGEST_SIZE];
    unsigned char resetResult  [bdlde::Sha1::k_DIGEST_SIZE];

    digest1.loadDigest(regularResult);
    digest1.loadDigestAndReset(resetResult);
    ASSERT(bsl::equal(bsl::begin(regularResult),
                      bsl::end(regularResult),
                      resetResult));

    ASSERT(digest1 == digest2);
}

void testPrinting()
    // Test that the member function 'print' and the stream operator ('<<')
    // both output the correct value for each of the inputs with known hashes.
{
    ASSERT(bsl::size(inputMessages) == bsl::size(sha1Results));
    for (bsl::size_t index = 0; index != bsl::size(inputMessages); ++index)
    {
        const bsl::string& message = inputMessages[index];
        const bdlde::Sha1  digest(message.c_str(), message.size());

        if (veryVerbose) { T_ P(index) }

        bsl::stringstream printStream;
        digest.print(printStream);
        ASSERT(printStream.str() == sha1Results[index]);

        bsl::stringstream streamStream;
        streamStream << digest;
        ASSERT(streamStream.str() == sha1Results[index]);
    }
}

void testReset(const char *message)
    // Test the member function 'reset' after updating the digest with the
    // specified 'message' and the specified 'length'.
{
    bdlde::Sha1 digest1;
    bdlde::Sha1 digest2;
    ASSERT(digest1 == digest2);

    digest1.update(message, bsl::strlen(message));
    ASSERT(digest1 != digest2);

    digest1.reset();
    ASSERT(digest1 == digest2);
}

void testTwoArgumentConstructor(const char *message)
    // Test the two-argument constructor accepting the specified 'message' and
    // the specified 'length'.
{
    const bsl::size_t length = bsl::strlen(message);
    bdlde::Sha1       digest1(message, length);
    bdlde::Sha1       digest2;
    digest2.update(message, length);

    ASSERT(digest1 == digest2);
}

}  // close unnamed namespace

//=============================================================================
//                                MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int    test = argc > 1 ? bsl::atoi(argv[1]) : 0;
        verbose = argc > 2;
    veryVerbose = argc > 3;

    cout << "TEST " << __FILE__ << " CASE " << test << '\n';

    switch (test) { case 0:
      case 7: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE
        //   This will test the usage example provided in the component header
        //   file.
        //
        // Concerns:
        //: 1 The usage example provided in the component header file must
        //:   compile, link, and run on all platforms as shown.
        //
        // Plan:
        //: 1 Run the usage example function 'assertPasswordIsExpected'.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << "TESTING USAGE EXAMPLE" "\n"
                          << "=====================" "\n";

        assertPasswordIsExpected();
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING PRINTING AND OUTPUT (<<) OPERATOR
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
        //   bsl::ostream& Sha1::print(bsl::ostream& stream) const;
        //   bsl::ostream& operator<<(bsl::ostream&, const Sha1&);
        // --------------------------------------------------------------------

        if (verbose) cout
               << "TESTING PRINTING AND OUTPUT (<<) OPERATOR\n"
               << "=========================================\n";

        testPrinting();
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING 'Sha1(const void *data, bsl::size_t length)'
        //   This will test the two-argument constructor.
        //
        // Concerns:
        //: 1 We want to ensure that the two-argument constructor from data and
        //:   a length is equivalent to the default constructor followed by a
        //:   call to 'update'.
        //
        // Plan:
        //: 1 Create a set of test data with varying lengths from 1 to 5. For
        //:   each datum, carry out the following steps.
        //: 2 Default construct a 'bdlde::Sha1' object and call 'update' with a
        //:   given message.  (C-1)
        //: 3 Construct a second 'bdlde::Sha1' object directly from the data
        //:   and size of the message.  (C-1)
        //: 4 Assert that the two hashers compare equal according to
        //:   'operator=='.  (C-1)
        //
        // Testing:
        //   Sha1::Sha1(const void *data, bsl::size_t length);
        // --------------------------------------------------------------------

        if (verbose) cout
               << "TESTING 'Sha1(const void *data, bsl::size_t length)'\n"
               << "====================================================\n";

        const char *const inputs[] = {"a", "a1", "abc", "a1c4", "12345"};

        for (bsl::size_t i = 0; i < bsl::size(inputs); i++) {
            if (veryVerbose) { T_ P_(i) P(inputs[i]); }
            testTwoArgumentConstructor(inputs[i]);
        }
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING 'loadDigestAndReset'
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
        //:   each datum, create a 'bdlde::Sha1' object using the fully-tested
        //:   default constructor and 'update' member function.  Then call the
        //:   'loadDigestAndReset' member function and ensure that the
        //:   resulting object contains the same value as an object created
        //:   using the default constructor.  (C-1)
        //
        // Testing:
        //   void Sha1::loadDigestAndReset(unsigned char *result);
        // --------------------------------------------------------------------

        if (verbose) cout << "TESTING 'loadDigestAndReset'\n"
                          << "============================\n";

        const char *const inputs[] = {"a", "a1", "abc", "a1c4", "12345"};

        for (bsl::size_t i = 0; i < bsl::size(inputs); i++) {
            if (veryVerbose) { T_ P_(i) P(inputs[i]); }
            testLoadDigestAndReset(inputs[i]);
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING 'reset'
        //   This will test the 'reset' method.
        //
        // Concerns:
        //: 1 We need to make sure that the resulting object after the call to
        //:   'reset' contains the same value as a default object.
        //
        // Plan:
        //: 1 Create a set of test data with varying lengths from 1 to 5.  For
        //:   each datum, create a 'bdlde::Sha1' object using the fully-tested
        //:   default constructor and 'update' member function.  Then call the
        //:   'reset' member function and ensure that the resulting object
        //:   contains the same value as an object created using the default
        //:   constructor.  (C-1)
        //
        // Testing:
        //   void Sha1::reset();
        // --------------------------------------------------------------------

        if (verbose) cout << "TESTING 'reset'\n"
                          << "===============\n";

        const char *const inputs[] = {"a", "a1", "abc", "a1c4", "12345"};

        for (bsl::size_t i = 0; i < bsl::size(inputs); i++) {
            if (veryVerbose) { T_ P_(i) P(inputs[i]); }
            testReset(inputs[i]);
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING 'update' AND 'loadDigest'
        //   Ensure that the 'Sha1' class computes SHA-1 digests correctly
        //   when given input through one or more calls to 'update' followed by
        //   a call to 'loadDigest'.
        //
        // Concerns:
        //: 1 Hashing a particular value provides the expected hash output when
        //:   known values.  These known values came from running the Linux
        //:   command-line utility 'sha1sum'.
        //: 2 Providing a message all at once through a single call to 'update'
        //:   gives the same result as providing it incrementally through
        //:   multiple calls to 'update'.
        //: 3 Our hasher handles various cases of complete and incomplete
        //:   blocks correctly.
        //
        // Plan:
        //: 1 Verify a range of values with varying sizes and bytes against
        //:   known values.  (C-1)
        //: 2 For some small strings, pass the entire string to 'update' and
        //:   compare the result with what is obtained from breaking it up
        //:   into equally-sized chunks and passing them to 'update' one by
        //:   one.  (C-2)
        //: 3 For a large string whose length is a multiple of the SHA-1 block
        //:   size (64 bytes), pass the entire string to 'update' and compare
        //:   the result with what is obtained from breaking it up into
        //:   substrings with certain sizes chosen in order to produce various
        //:   states of the internal buffer (64 bytes).  (C-2..3)
        //: 4 For a large string whose length is NOT a multiple of the SHA-1
        //:   block size (64 bytes), pass the entire string to 'update' and
        //:   compare the result with what is obtained from breaking it up into
        //:   substrings with certain sizes chosen in order to produce various
        //:   states of the internal buffer (64 bytes).  (C-2..3)
        //
        // Testing:
        //   Sha1::Sha1();
        //   void Sha1::update(const void *data, bsl::size_t length);
        //   void Sha1::loadDigest(unsigned char *result) const;
        //   bool operator==(const Sha1& lhs, const Sha1& rhs);
        //   bool operator!=(const Sha1& lhs, const Sha1& rhs);
        // --------------------------------------------------------------------
        if (verbose) cout << "TESTING 'update' AND 'loadDigest'\n"
                             "=================================\n";

        testKnownHashes();
        testIncrementalSmall();
        testIncrementalLargeMultiple();
        testIncrementalLargeNonMultiple();
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
        //: 1 Verify that a default constructed hasher equals itself.  (C-1)
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) cout << "\nBREATHING TEST"
                             "\n==============\n";
        bdlde::Sha1 hasher;
        ASSERT(hasher == hasher);
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
