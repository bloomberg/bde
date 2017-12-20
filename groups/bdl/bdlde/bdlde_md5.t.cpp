// bdlde_md5.t.cpp                                                    -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------


#include <bdlde_md5.h>

#include <bslx_testoutstream.h>                 // for testing only
#include <bslx_testinstream.h>                  // for testing only
#include <bslx_testinstreamexception.h>         // for testing only
#include <bsls_stopwatch.h>                     // for testing only

#include <bsl_cstdlib.h>     // atoi()
#include <bsl_iostream.h>
#include <bsl_string.h>      // test case 11, string
#include <bsl_sstream.h>
#include <bsl_vector.h>      // test case 11, vector

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

//=============================================================================
//                                 TEST PLAN
//-----------------------------------------------------------------------------
//                                  Overview
//                                  --------
// The component under test is a value-semantic scalar class with internal
// state representing the value.  There is no allocator involved.  We are
// mostly concerned with the mechanical functioning of the various methods and
// free operators, and exception neutrality during 'bdex' streaming.
//
// The component interface represents a 128-bit MD5 digest as 4 unsigned
// integers.  The MD5 digest is calculated in the 'update' and 'loadDigest'
// functions based on a sequence of data bytes and the data length.  Note that
// the reason that the 'loadDigest' function is responsible for part of the
// calculation is specified in the implementation file (bdlde_md5.cpp).  We
// need to verify that these functions calculates the checksum correctly.
// Furthermore, we also need to verify that the 'loadDigest' function also
// returns a correct MD5 digest.
//
//       Primary Constructors, Primary Manipulators, and Basic Accessors
//       ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Primary Constructors:
//   A 'bdlde::Md5' object is created with a default message length of 0 and a
//   digest of d41d8cd98f00b204e9800998ecf8427e.  An object's white-box
//   state is represented by a buffer of unprocessed bytes, the length of the
//   message and also the current states.  The black-box state is represented
//   by a 128-bit digest, which can be modified using the primary manipulator
//   'update'.  The default constructor, in conjunction with the primary
//   manipulator, is sufficient to attain any achievable white-box state.
//
//    o bdlde::Md5();
//
// Primary Manipulators:
//   The 'update' method comprises the minimal set of manipulators that can
//   attain any achievable white-box state.
//
//    o void update(const void *data, int length);
//
// Basic Accessors:
//   This is the maximal set of accessors that have direct contact with the
//   black-box representation of the object.  The 'loadDigest' method is an
//   obvious member of this set.  It appends the length of the current message
//   to a copy of the current MD5 object and calculates the digest.  The
//   'loadDigestAndReset' method is really a manipulator as it resets the
//   states within the object.  Therefore, it is not included in the set.
//
//    o void loadDigest(Md5Digest *result) const;
//
//-----------------------------------------------------------------------------
// CLASS METHODS
// [10] static int maxSupportedBdexVersion(int);
//
// CREATORS
// [ 2] bdlde::Md5();
// [12] bdlde::Md5(const void *data, int length);
// [ 7] bdlde::Md5(const bdlde::Crc32& original);
// [ 2] ~bdlde::Md5();

// MANIPULATORS
// [ 9] bdlde::Md5& operator=(const bdlde::Crc32& rhs);
// [10] STREAM& bdexStreamIn(STREAM& stream, int version);
// [13] void reset();
// [11] void update(const void *data, int length);
// [14] void loadDigestAndReset(Md5Digest *result);
//
// ACCESSORS
// [10] STREAM& bdexStreamOut(STREAM& stream, int version) const;
// [ 5] bsl::ostream& print(bsl::ostream& stream) const;
// [ 4] void loadDigest(Md5Digest *result) const;
//
// FREE OPERATORS
// [ 6] bool operator==(const bdlde::Md5& lhs, const bdlde::Md5& rhs);
// [ 6] bool operator!=(const bdlde::Md5& lhs, const bdlde::Md5& rhs);
// [ 5] bsl::ostream& operator<<(bsl::ostream& stream, const bdlde::Md5&);
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [15] USAGE EXAMPLE
// [ 2] BOOTSTRAP: void update(const void *data, int length);
// [-1] PERFORMANCE TEST
//
// [ 3] int ggg(bdlde::Crc32 *object, const char *spec, int vF = 1);
// [ 3] bdlde::Md5& gg(bdlde::Md5 *object, const char *spec);
// [ 8] bdlde::Md5   g(const char *spec);


// ============================================================================
//                      STANDARD BDE ASSERT TEST MACRO
// ----------------------------------------------------------------------------
static int testStatus = 0;

static void aSsErT(int c, const char *s, int i)
{
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (0 <= testStatus && testStatus <= 100) ++testStatus;
    }
}

#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

// ============================================================================
//                   STANDARD BDE LOOP-ASSERT TEST MACROS
// ----------------------------------------------------------------------------
#define LOOP_ASSERT(I,X) { \
   if (!(X)) { cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__); }}

#define LOOP2_ASSERT(I,J,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " \
              << J << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" \
              << #K << ": " << K << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP4_ASSERT(I,J,K,L,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" << \
       #K << ": " << K << "\t" << #L << ": " << L << "\n"; \
       aSsErT(1, #X, __LINE__); } }

#define LOOP5_ASSERT(I,J,K,L,M,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" << \
       #K << ": " << K << "\t" << #L << ": " << L << "\t" << \
       #M << ": " << M << "\n"; \
       aSsErT(1, #X, __LINE__); } }

#define LOOP6_ASSERT(I,J,K,L,M,N,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" << \
       #K << ": " << K << "\t" << #L << ": " << L << "\t" << \
       #M << ": " << M << "\t" << #N << ": " << N << "\n"; \
       aSsErT(1, #X, __LINE__); } }

// ============================================================================
//                     SEMI-STANDARD TEST OUTPUT MACROS
// ----------------------------------------------------------------------------
#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", "<< flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number
#define T_ cout << "\t" << flush;             // Print tab w/o newline

// ============================================================================
//                   GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

typedef bdlde::Md5          Obj;
typedef bslx::TestInStream  In;
typedef bslx::TestOutStream Out;

// ============================================================================
//                               USAGE EXAMPLE
// ----------------------------------------------------------------------------

///Usage
///-----
// The following snippets of code illustrate a typical use of the 'bdlde::Md5'
// class.  Each function would typically execute in separate processes or
// potentially on separate machines.  The 'senderExample' function below
// demonstrates how a message sender can write a message and its MD5 digest to
// a 'bdex' output stream.  Note that 'Out' may be a 'typedef' of any class
// that implements the 'bslx::OutStream' protocol:
//..
void senderExample(Out& output)
    // Write a message and its MD5 digest to the specified 'output' stream.
{
    // Prepare a message.
    bsl::string message = "This is a test message.";

    // Generate a digest for 'message'.
    bdlde::Md5 digest(message.data(), message.length());

    // Write the message to 'output'.
    output << message;

    // Write the digest to 'output'.
    const int VERSION = 1;
    digest.bdexStreamOut(output, VERSION);
}
//..
// The 'receiverExample' function below illustrates how a message receiver can
// read a message and its MD5 digest from a 'bdex' input stream, then perform a
// local MD5 computation to verify that the message was received intact.  Note
// that 'In' may be a 'typedef' of any class that implements the
// 'bslx::InStream' protocol:
//..
void receiverExample(In& input)
    // Read a message and its MD5 digest from the specified 'input' stream,
    // and verify the integrity of the message.
{
    // Read the message from 'input'.
    bsl::string message;
    input >> message;

    // Read the digest from 'input'.
    bdlde::Md5 digest;
    const int VERSION = 1;
    digest.bdexStreamIn(input, VERSION);

    // Locally compute the digest of the received 'message'.
    bdlde::Md5 digestLocal;
    digestLocal.update(message.data(), message.length());

    // Verify that the received and locally-computed digests match.
    ASSERT(digestLocal == digest);
}

// ============================================================================
//                    GLOBAL HELPER FUNCTIONS FOR TESTING
// ----------------------------------------------------------------------------

void printHex(const char *str)
    // Print the specified 'str' string to 'bsl::cout', taking care to expand
    // non-printable characters into their hexadecimal representation.
{
    cout << "\"";
    for (int i = 0; str[i]; ++i) {
        if (0x20 <= str[i] && str[i] <= 0x7e) {
            // printable characters
            cout << str[i];
        } else {
            // non-printable characters
            static const char *hex = "0123456789abcdef";

            cout << "\\x"
                 << hex[(str[i] >> 4) & 0xf]
                 << hex[ str[i]       & 0xf];
        }
    }
    cout << "\"";
}

// Print identifier and hex string.
#define PH(X) cout << #X " = "; printHex(X); cout << endl;
// PH(X) without '\n'.
#define PH_(X) cout << #X " = "; printHex(X); cout << ", " << flush;

//=============================================================================
//            GENERATOR FUNCTIONS 'g', 'gg' and 'ggg' FOR TESTING
//-----------------------------------------------------------------------------
// The following functions interpret the given 'spec' in order from left to
// right to configure the object according to a custom language.  The given
// 'spec' contains the data that will be passed to the 'update' method.  The
// following escape sequences are recognized by the language:
//
//    o '//'    : will be replaced with a '/'.
//    o '/xx'   : will be replaced with the specified 'xx' hexadecimal value
//                (each 'x' must be one of '0'..'9' or 'a'..'f').
//
// Any other character following a '/' will be considered an error.  The
// maximum length supported for the data in an 'update' call is 1024 bytes
// (this is more than sufficient for all test data).
//
// LANGUAGE SPECIFICATION
// ----------------------
//..
// <SPEC>       ::= <EMPTY> | <DATA>
//
// <EMPTY>      ::=
//
// <DATA>       ::= ( '\x01'..'\x2e'       # 0x2f is the '/' escape character
//                  | '\x30'..'\xff'
//                  | <ESC_SEQ>
//                  )*
//
// <ESC_SEQ>    ::= '/' (<HEX> | '/')
//
// <HEX>        ::= <HEX_CHAR> <HEX_CHAR>
//
// <HEX_CHAR>   ::= ('0'..'9' || 'a'..'f')
//
// Spec String       Description
// ----------------- ----------------------------------------------------------
// ""                Has no effect.
// "/00"             Calls 'update("\x00", 1)'.
// "abc"             Calls 'update("abc", 3)'.
// "abc//de~/20"     Calls 'update("abc/de~ ", 8)'.   # ('/20' will become ' ')
// "abc~de"          Calls 'update("abc~de", 6)'.
// "/01/02\xff"      Calls 'update("\x01\x02\xff", 3)'.
// ----------------------------------------------------------------------------
//..

int ggg(Obj *object, const char *spec, int vF = 1)
    // Configure the specified 'object' according to the specified 'spec'
    // using the primary manipulator function 'update' and 'reset'.  Optionally
    // specify a zero 'vF' to suppress 'spec' syntax error messages.  Return
    // the index of the first invalid character, and a negative value
    // otherwise.  Note that this function is used to implement 'gg' as well as
    // allow for verification of syntax error detection.
{
    enum { SUCCESS = -1 };

    static char update_buffer[1024];
    int buf_pos = 0;

    for (int i = 0; spec[i]; ++i) {
        if ('/' == spec[i]) {
            // This is the escape character.  Here we need to look ahead and
            // verify that the characters that follow are valid escape codes.

            ++i;
            if ('/' == spec[i]) {
                // add '/' to 'update_buffer'
                update_buffer[buf_pos++] = '/';
            } else if (('0' <= spec[i] && spec[i] <= '9')
                    || ('a' <= spec[i] && spec[i] <= 'f')) {
                // build the hexadecimal character, add it to 'update_buffer'

                unsigned char hex;

                if ('0' <= spec[i] && spec[i] <= '9') {
                    hex = (spec[i] - '0') << 4;
                } else if ('a' <= spec[i] && spec[i] <= 'f') {
                    hex = (spec[i] - 'a' + 10) << 4;
                }

                // look at the next character

                ++i;
                if ('0' <= spec[i] && spec[i] <= '9') {
                    hex |= (spec[i] - '0');
                } else if ('a' <= spec[i] && spec[i] <= 'f') {
                    hex |= (spec[i] - 'a' + 10);
                } else {
                    // syntax error, print an error message if vF != 0

                    if (vF) {
                        cout << "Error 2, bad character ('" << spec[i] << "') "
                             << "at position " << i << "." << endl;
                        PH(spec);
                    }

                    // Discontinue processing this spec.

                    return i;                                         // RETURN
                }

                // add 'hex' to 'update_buffer'

                update_buffer[buf_pos++] = hex;
            } else {
                // syntax error, print an error message if vF != 0

                if (vF) {
                    cout << "Error, bad character ('" << spec[i] << "') at "
                         << "position " << i << "." << endl;
                    PH(spec);
                }

                // Discontinue processing this spec.

                return i;                                             // RETURN
            }
        } else {
            // add the current character to 'update_buffer'

            update_buffer[buf_pos++] = spec[i];
        }
    }

    // call 'update' with the remaining data in 'update_buffer'

    if (buf_pos) {
        object->update(update_buffer, buf_pos);
    }

    return SUCCESS;
}

Obj& gg(Obj *object, const char *spec)
    // Return, by reference, the specified 'object' with its value adjusted
    // according to the specified 'spec'.
{
    ASSERT(ggg(object, spec) < 0);
    return *object;
}

Obj g(const char *spec)
    // Return, by value, a new object corresponding to the specified 'spec'.
{
    Obj object;
    return gg(&object, spec);
}

int verify(const bdlde::Md5::Md5Digest& result, const char *hexString)
{
    const char hex[32] = "0123456789abcdef";

    const char *resultBytes = result.buffer();

    int j = 0;
    int val = 1;
    for (int i = 0; i < 16; ++i) {
        val &= hex[(resultBytes[i] >> 4) & 0xf] == hexString[j++];
        val &= hex[resultBytes[i] & 0xf] == hexString[j++];
    }
    return val;
}

// ============================================================================
//                               MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;;

    switch (test) { case 0:  // Zero is always the leading case.
      case 15: {
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
        //   Run the usage example functions 'senderExample' and
        //   'receiverExample'.
        //
        // Testing:
        //   Usage example.
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Usage Example"
                          << "\n=====================" << endl;

        Out out(20150813);
        senderExample(out);

        const char *const OD  = out.data();
        const int         LOD = out.length();
        In in(OD, LOD);

        receiverExample(in);

      } break;
      case 14: {
        // --------------------------------------------------------------------
        // TESTING 'loadDigestAndReset(Md5Digest *result)'
        //   Test the manipulator 'loadDigestAndReset'.
        //
        // Concerns:
        //   We want to verify that, for each unique object value, the
        //   manipulator returns the correct values and the object is reset
        //   afterwards.
        //
        // Plan:
        //   For each test vector in DATA, construct an object 'mX' using
        //   the default constructor.  Then call 'mX.update' using the current
        //   STR and LENGTH.  Ensure that the MD5 value returned by
        //   'loadDigestAndReset' is the same as the expected MD5 value.  Then
        //   verify that a subsequent value returned by 'loadDigest' will be
        //   the same as a default object.
        //
        // Testing:
        //   void loadDigestAndReset(bdlde::Md5::Md5Digest result);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Basic Accessors"
                          << "\n=======================" << endl;
        static const struct {
            int         d_lineNum;  // source line number
            const char *d_str;      // source string
            int         d_length;   // length of source input
            const char *d_result;   // expected result string
        } DATA[] = {
            //line  source        length  digest
            //----  ------        ------  ----------------------------------
            { L_,   "",                0, "d41d8cd98f00b204e9800998ecf8427e" },
            { L_,   "a",               1, "0cc175b9c0f1b6a831c399e269772661" },
            { L_,   "abc",             3, "900150983cd24fb0d6963f7d28e17f72" },
            { L_,   "message digest", 14, "f96b697d7cb7938d525a2f31aaf161d0" },
            { L_,   "abcdefghijklmnopqrstuvwxyz",
                                      26, "c3fcd3d76192e4007dfb496cca67e13b" },
            { L_,   "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                    "abcdefghijklmnopqrstuvwxyz0123456789",
                                      62, "d174ab98d277d9f5a5611c2c9f419d9f" },
            { L_,   "1234567890123456789012345678901234567890"
                    "1234567890123456789012345678901234567890",
                                      80, "57edf4a22be3c955ac49da2e2107b67a" },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        if (verbose) cout << "\nTesting 'loadDigestAndReset'." << endl;
        {
            const char *URESULT = DATA[0].d_result;
            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE   = DATA[i].d_lineNum;
                const char *STR    = DATA[i].d_str;
                const int   LENGTH = DATA[i].d_length;
                const char *RESULT = DATA[i].d_result;

                Obj mX;
                const Obj& X = mX;
                mX.update(STR, LENGTH);

                if (veryVerbose) {
                    T_ P_(LENGTH);
                    T_ P(X);
                }

                // check the digest
                bdlde::Md5::Md5Digest r;
                mX.loadDigestAndReset(&r);
                LOOP_ASSERT(LINE, verify(r, RESULT));

                // make sure reset is done properly
                X.loadDigest(&r);
                LOOP_ASSERT(LINE, verify(r, URESULT));
            }
        }
      } break;

      case 13: {
        // --------------------------------------------------------------------
        // TESTING 'reset'
        //   This will test the 'reset' method.
        //
        // Concerns:
        //   We need to make sure that the resulting object after the call to
        //   'reset' contains the same value as a default object.
        //
        // Plan:
        //   Create a set of test data with varying lengths from 0 to 5.  For
        //   each datum, create a 'bdlde::Md5' object using the fully-tested
        //   two-argument init constructor.  Then call the 'reset' member
        //   function and ensure that the resulting object contains the same
        //   value as an object created using the default constructor.
        //
        // Testing:
        //   void reset();
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting 'reset'"
                          << "\n===============" << endl;

        static const struct {
            int         d_lineNum;  // source line number
            const char *d_str;      // source string
            int         d_length;   // length of source input
            const char *d_result;   // expected md5 digest
        } DATA[] = {
            //line  source        length  digest
            //----  ------        ------  ----------------------------------
            { L_,   "",                0, "d41d8cd98f00b204e9800998ecf8427e"},
            { L_,   "a",               1, "0cc175b9c0f1b6a831c399e269772661"},
            { L_,   "abc",             3, "900150983cd24fb0d6963f7d28e17f72"},
            { L_,   "message digest", 14, "f96b697d7cb7938d525a2f31aaf161d0"},
            { L_,   "abcdefghijklmnopqrstuvwxyz",
                                      26, "c3fcd3d76192e4007dfb496cca67e13b"},
            { L_,   "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                    "abcdefghijklmnopqrstuvwxyz0123456789",
                                      62, "d174ab98d277d9f5a5611c2c9f419d9f"},
            { L_,   "1234567890123456789012345678901234567890"
                    "1234567890123456789012345678901234567890",
                                      80, "57edf4a22be3c955ac49da2e2107b67a"},
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int i = 0; i < NUM_DATA; ++i) {
            const int   LINE   = DATA[i].d_lineNum;
            const char *STR    = DATA[i].d_str;
            const int   LENGTH = DATA[i].d_length;
            const char *RESULT = DATA[i].d_result;

            const Obj          DEFAULT;

            Obj x(STR, LENGTH);  const Obj& X = x;
            bdlde::Md5::Md5Digest r;
            X.loadDigest(&r);
            LOOP_ASSERT(LINE, verify(r, RESULT));

            x.reset();
            if (veryVerbose) { T_ P(X); }
            LOOP_ASSERT(LINE, DEFAULT == X);
        }

      } break;
      case 12: {
        // --------------------------------------------------------------------
        // TESTING INIT CONSTRUCTOR
        //   We need to test the init constructor that takes two arguments.
        //
        // Concerns:
        //   Since 'update' has already been tested thoroughly in [11], we just
        //   need to make sure that the values passed to 'update' in the
        //   constructor are correct.
        //
        // Plan:
        //   Create a set of test data with various lengths.  For each
        //   datum, create a 'bdlde::Md5' object using the two-argument
        //   init constructor.  Then verify the Md5 digest stored in the object
        //   is the same as the expected value.
        //
        // Testing:
        //   bdlde::Md5(const void *data, int length);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Init Constructor"
                          << "\n========================" << endl;

        static const struct {
            int           d_lineNum;  // source line number
            const char   *d_str;      // source string
            int           d_length;   // length of source input
            const char   *d_result;   // expected md5 digest
        } DATA[] = {
            //line  source        length  digest
            //----  ------        ------  ----------------------------------
            { L_,   "",                0, "d41d8cd98f00b204e9800998ecf8427e"},
            { L_,   "a",               1, "0cc175b9c0f1b6a831c399e269772661"},
            { L_,   "abc",             3, "900150983cd24fb0d6963f7d28e17f72"},
            { L_,   "message digest", 14, "f96b697d7cb7938d525a2f31aaf161d0"},
            { L_,   "abcdefghijklmnopqrstuvwxyz",
                                      26, "c3fcd3d76192e4007dfb496cca67e13b"},
            { L_,   "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                    "abcdefghijklmnopqrstuvwxyz0123456789",
                                      62, "d174ab98d277d9f5a5611c2c9f419d9f"},
            { L_,   "1234567890123456789012345678901234567890"
                    "1234567890123456789012345678901234567890",
                                      80, "57edf4a22be3c955ac49da2e2107b67a"},
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int i = 0; i < NUM_DATA; ++i) {
            const int   LINE   = DATA[i].d_lineNum;
            const char *STR    = DATA[i].d_str;
            const int   LENGTH = DATA[i].d_length;
            const char *RESULT = DATA[i].d_result;

            Obj mX(STR, LENGTH);  const Obj& X = mX;
            if (veryVerbose) {
                T_ P_(X);
            }

            bdlde::Md5::Md5Digest r;
            X.loadDigest(&r);
            LOOP_ASSERT(LINE, verify(r, RESULT));
        }

      } break;
      case 11: {
        // --------------------------------------------------------------------
        // TESTING 'update'
        //   We need to thoroughly test the 'update' member function.
        //
        // Concerns:
        //   There are three things we are concerned about in this test.
        //
        //   Firstly, we need to test the 'update' function using data that
        //   exercises all code paths.  The algorithm selects control paths
        //   based on the length of the data, and not the data itself.  The
        //   control path is selected using 'length % 64'.  So, in order to
        //   exercise all control paths, the test vectors must contain data
        //   with lengths:
        //
        //   0   - empty string
        //   1   - unit length string
        //   10  - normal length string
        //   63  - approaching 64
        //   64  - at 64
        //   65  - one above 64
        //   80  - make sure loops properly
        //   128 - two times 64
        //   160 - make sure loops properly
        //
        //   Secondly, we need to ensure that there are no MD5 collisions for
        //   any possible combination of 1 or 2 byte data.
        //
        //   Finally, we need to ensure that multiple calls to 'update' produce
        //   the same result as calling 'update' once with the original two
        //   strings concatenated together.
        //
        // Plan:
        //   For the first part of the test, create a test vector containing
        //   data with lengths as specified above.  Loop through the vector
        //   to update the MD5 object and make sure the object's digest is
        //   equal to the expected digest.
        //
        //   For the second part of the test, create an integer array using
        //   'bsl::vector<char*>'.  Fill the array using the following steps:
        //      a. Loop through all the possible values for an 'unsigned char'
        //         (0 .. 255) and add its digest to the array.
        //      b. Then loop through all the possible values for a 16-bit
        //         'unsigned short' (0 .. 65535) and add its digest to the
        //         array.
        //      c. Again, loop through all the possible values for a 16-bit
        //         'unsigned short', but insert a 0-valued byte in between the
        //         2 bytes.  Calculate the digest and add it to the array.
        //      d. Repeat step (c), but insert a 1-valued byte instead of a
        //         0-valued byte.
        //   Sort the array, then assert that each MD5 value in the array
        //   (except for the 0th array element) are unique.
        //
        //   For the third part of the test, create test vectors containing
        //   substring 1 ('MSG1') and substring 2 ('MSG2').  For each input
        //   case, create two 'bdlde::Crc32' objects ('obj' and 'objAccum').
        //   Call 'obj.update' with 'MSG1' followed by 'MSG2'.  Concatenate
        //   'MSG1' and 'MSG2' into a single string and call
        //   'objAccum.update' with the concatenated string.  Finally,
        //   assert that 'obj' has the same value as 'objAccum'.
        //
        // Testing:
        //   void update(const void *data, int length);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting update"
                          << "\n==============" << endl;

        if (verbose) cout << "\n1. Testing With Data Lengths 0-10." << endl;
        {
            static const struct {
                int         d_lineNum;  // source line number
                const char *d_str;      // source string
                int         d_length;   // length of source input
                const char *d_result;   // expected digest
            } DATA[] = {
                //line  source    length   digest
                //----  ------    ------   ------
                { L_,   "",       0,       "d41d8cd98f00b204e9800998ecf8427e"},
                { L_,   "a",      1,       "0cc175b9c0f1b6a831c399e269772661"},
                { L_,   "ab",     2,       "187ef4436122d1cc2f40dc2b92f0eba0"},
                { L_,   "abc",    3,       "900150983cd24fb0d6963f7d28e17f72"},
                { L_,   "abcdefghij",
                                  10,      "a925576942e94b2ef57a066101b48876"},
                { L_,   "abcdefghijklmnopqrstuvwxyz0123456789"
                        "abcdefghijklmnopqrstuvwxyz0",
                                  63,      "87d2cdc81ca700a7259acd6bc75abf0b"},
                { L_,   "abcdefghijklmnopqrstuvwxyz0123456789"
                        "abcdefghijklmnopqrstuvwxyz01",
                                  64,      "bbd17cbd1784152cd93cca62dee11b5b"},
                { L_,   "abcdefghijklmnopqrstuvwxyz0123456789"
                        "abcdefghijklmnopqrstuvwxyz012",
                                  65,      "39da148ae70842bc58abffba06b4dafe"},
                { L_,   "abcdefghijklmnopqrstuvwxyz0123456789"
                        "abcdefghijklmnopqrstuvwxyz0123456789"
                        "abcdefgh",
                                  80,      "b2b87123a40dc7e80e54cd7a05dacc57"},
                { L_,   "abcdefghijklmnopqrstuvwxyz0123456789"
                        "abcdefghijklmnopqrstuvwxyz0123456789"
                        "abcdefghijklmnopqrstuvwxyz0123456789"
                        "abcdefghijklmnopqrst",
                                  128,     "bde3bb14a0590a2eba7a42d63c961e88"},
                { L_,   "abcdefghijklmnopqrstuvwxyz0123456789"
                        "abcdefghijklmnopqrstuvwxyz0123456789"
                        "abcdefghijklmnopqrstuvwxyz0123456789"
                        "abcdefghijklmnopqrstuvwxyz0123456789"
                        "abcdefghijklmnop",
                                  160,     "6595fca00213c57d509917a807f8041d"},
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE   = DATA[i].d_lineNum;
                const char *STR    = DATA[i].d_str;
                const int   LEN    = DATA[i].d_length;
                const char *RESULT = DATA[i].d_result;

                Obj mX;  const Obj& X = mX;
                mX.update(STR, LEN);
                if (veryVerbose) {
                    T_ P_(LEN);
                    P(X);
                }
                bdlde::Md5::Md5Digest r;
                X.loadDigest(&r);
                LOOP_ASSERT(LINE, verify(r, RESULT));
            }
        }

        if (verbose) cout << "\n2. Collision test." << endl;
        {
            bsl::vector<bsl::string> a;
            Obj obj;
            bdlde::Md5::Md5Digest r;

            // populate the list of results

            // one byte
            for (int i = 0; i < 256; ++i) {
                unsigned char data = (unsigned char)i;
                obj.update(&data, 1);
                obj.loadDigestAndReset(&r);
                a.push_back(bsl::string(r.buffer(), 16));
            }

            // two bytes
            for (int i = 0; i < 65536; ++i) {
                unsigned char data[2];
                data[0] = (unsigned char)(i / 256);
                data[1] = (unsigned char)(i % 256);
                obj.update(data, 2);
                obj.loadDigestAndReset(&r);
                a.push_back(bsl::string(r.buffer(), 16));
            }

            // two bytes with a zero separator
            for (int i = 0; i < 65536; ++i) {
                unsigned char data[3];
                data[0] = (unsigned char)(i / 256);
                data[1] = 0;
                data[2] = (unsigned char)(i % 256);
                obj.update(data, 3);
                obj.loadDigestAndReset(&r);
                a.push_back(bsl::string(r.buffer(), 16));
            }

            // two bytes with a one separator
            for (int i = 0; i < 65536; ++i) {
                unsigned char data[3];
                data[0] = (unsigned char)(i / 256);
                data[1] = 1;
                data[2] = (unsigned char)(i % 256);
                obj.update(data, 3);
                obj.loadDigestAndReset(&r);
                a.push_back(bsl::string(r.buffer(), 16));
            }

            for (unsigned int i = 1; i < a.size(); ++i) {
                ASSERT(0 != a[i].compare(a[i-1]));
            }
        }

        if (verbose) cout << "\n3. Multiple 'update' test." << endl;
        {
            static const struct {
                int         d_lineNum;   // source line number
                const char *d_message1;  // first string
                const char *d_message2;  // second string
                int         d_len1;      // length of d_message1
                int         d_len2;      // length of d_message2
            } DATA[] = {
                //line  msg1     msg2     len1 len2
                //----  ----     ----     ---- ----
                { L_,   "",      "",      0,   0    },  // length = 0

                { L_,   "a",     "",      1,   0    },  // length = 1
                { L_,   "",      "z",     0,   1    },

                { L_,   "ab",    "",      2,   0    },  // length = 2
                { L_,   "a",     "z",     1,   1    },
                { L_,   "",      "yz",    0,   2    },

                { L_,   "abc",   "",      3,   0    },  // length = 3
                { L_,   "ab",    "z",     2,   1    },
                { L_,   "a",     "yz",    1,   2    },
                { L_,   "",      "xyz",   0,   3    },

                { L_,   "abcd",  "",      4,   0    },  // length = 4
                { L_,   "abc",   "z",     3,   1    },
                { L_,   "ab",    "yz",    2,   2    },
                { L_,   "a",     "xyz",   1,   3    },
                { L_,   "",      "wxyz",  0,   4    },

                { L_,   "abcde", "",      5,   0    },  // length = 5
                { L_,   "abcd",  "z",     4,   1    },
                { L_,   "abc",   "yz",    3,   2    },
                { L_,   "ab",    "xyz",   2,   3    },
                { L_,   "a",     "wxyz",  1,   4    },
                { L_,   "",      "vwxyz", 0,   5    },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                // test each element in the test vector
                const int   LINE = DATA[i].d_lineNum;
                const char *MSG1 = DATA[i].d_message1;
                const char *MSG2 = DATA[i].d_message2;
                const int   LEN1 = DATA[i].d_len1;
                const int   LEN2 = DATA[i].d_len2;

                Obj obj, objAccum;

                // initialize the first object
                obj.update(MSG1, LEN1);
                obj.update(MSG2, LEN2);

                // create a concatenated version of MSG1 and MSG2
                char msgAccum[20];
                memcpy(msgAccum,      MSG1, LEN1);
                memcpy(msgAccum+LEN1, MSG2, LEN2);

                // initialize the second object
                objAccum.update(msgAccum, LEN1+LEN2);

                if (veryVerbose) {
                    T_ PH_(MSG1); P(LEN1);
                    T_ PH_(MSG2); P(LEN2);
                    T_ P_(obj);   P(objAccum);
                }

                LOOP_ASSERT(LINE, obj == objAccum);
            }
        }
      } break;
      case 10: {
        // --------------------------------------------------------------------
        // TESTING 'bdex' STREAMING FUNCTIONALITY
        //   The 'bdex' streaming concerns for this component are absolutely
        //   standard.
        //
        // Concerns:
        //   We need to probe the member functions 'bdexStreamIn' and
        //   'bdexStreamOut' in the manner of a "breathing test" to verify
        //   basic functionality, then we need to thoroughly test that
       //   functionality using the available bdex stream functions,
        //   which forward appropriate calls to the member functions.  We also
        //   want to step through the sequence of possible stream states
        //   (valid, empty, invalid, incomplete, and corrupted), appropriately
        //   selecting data sets as described below.  In all cases, we need to
        //   confirm exception neutrality using the specially instrumented
        //   'bslx::TestInStream' and a pair of standard macros,
        //   'BSLX_TESTINSTREAM_EXCEPTION_TEST_BEGIN' and
        //   'BSLX_TESTINSTREAM_EXCEPTION_TEST_END', which
        //   configure the 'bslx::TestInStream' object appropriately in a loop.
        //
        // Plan:
        //   PRELIMINARY MEMBER FUNCTION TEST
        //     First perform a trivial direct test of the 'bdexStreamOut' and
        //     'bdexStreamIn' methods.  (The remaining tests will use the
        //     bdex stream functions.)
        //
        //   VALID STREAMS
        //     For the set S of globally-defined test values, use all
        //     combinations (u, v) in the cross product S X S, stream the
        //     value of v into (a temporary copy of) u and assert u == v.
        //
        //   EMPTY AND INVALID STREAMS
        //     For each u in S, create a copy and attempt to stream into it
        //     from an empty stream, and then an invalid stream.  Verify after
        //     each attempt that the object is unchanged and that the stream is
        //     invalid.
        //
        //   INCOMPLETE (BUT OTHERWISE VALID) DATA
        //     Write 3 distinct objects to an output stream buffer of total
        //     length N.  For each partial stream length from 0 to N - 1,
        //     construct an input stream and attempt to read into objects
        //     initialized with distinct values.  Verify values of objects that
        //     are either successfully modified or left entirely unmodified,
        //     and that the stream became invalid immediately after the first
        //     incomplete read.  Finally ensure that each object streamed into
        //     is in some valid state by assigning it a distinct new value and
        //     testing for equality.
        //
        //   CORRUPTED DATA
        //     Use the underlying stream package to simulate an instance of
        //     a typical valid (control) stream and verify that it can be
        //     streamed in successfully.  The component does not define
        //     'corrupted' data (all digests are valid), so just check for bad
        //     version numbers.  After each test, verify that the object is in
        //     some valid state after streaming, and that the input stream has
        //     gone invalid.
        //
        // Testing:
        //   static int maxSupportedBdexVersion(int);
        //   STREAM& bdexStreamIn(STREAM& stream, int version);
        //   STREAM& bdexStreamOut(STREAM& stream, int version) const;
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting 'bdex' Streaming Functionality"
                          << "\n======================================"
                          << endl;

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        // scalar and array object values for various stream tests
        const Obj VA(g(""));
        const Obj VB(g("ABC"));
        const Obj VC(g("DEF"));
        const Obj VD(g("GHI"));
        const Obj VE(g("JKL"));
        const Obj VF(g("MNO"));

        const int NUM_VALUES = 6;
        const Obj VALUES[NUM_VALUES] = { VA, VB, VC, VD, VE, VF };
        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        const int VERSION = Obj::maxSupportedBdexVersion(150813);
        if (verbose) cout << "\nTesting 'bdexStreamOut' and (valid) "
                          << "'bdexStreamIn' functionality." << endl;
        {
            // testing 'bdexStreamOut' and 'bdexStreamIn' directly
            const Obj X(VC);
            Out out(20150813);
            out.putVersion(VERSION);
            X.bdexStreamOut(out, VERSION);

            const char *const OD  = out.data();
            const int         LOD = out.length();
            In in(OD, LOD);
            ASSERT(in);                         ASSERT(!in.isEmpty());

            Obj t(VA);                          ASSERT(X != t);
            int version;
            in.getVersion(version);
            ASSERT(VERSION == version);
            t.bdexStreamIn(in, version);        ASSERT(X == t);
            ASSERT(in);                         ASSERT(in.isEmpty());
        }

        if (verbose) cout << "\nThorough test using the bdex stream functions "
                          << endl;
        {
            for (int i = 0; i < NUM_VALUES; ++i) {
                const Obj X(VALUES[i]);
                Out out(20150813);
                X.bdexStreamOut(out, VERSION);

                const char *const OD  = out.data();
                const int         LOD = out.length();

                // Verify that each new value overwrites every old value and
                // that the input stream is emptied, but remains valid.

                for (int j = 0; j < NUM_VALUES; ++j) {
                    In in(OD, LOD);  In &testInStream = in;

//                  in.setSuppressVersionCheck(1);

                    LOOP2_ASSERT(i, j, in);
                    LOOP2_ASSERT(i, j, !in.isEmpty());

                    Obj t(VALUES[j]);
                    BSLX_TESTINSTREAM_EXCEPTION_TEST_BEGIN(in) {
                      in.reset();
                      LOOP2_ASSERT(i, j, (X == t) == (i == j));
                      t.bdexStreamIn(in, VERSION);
                    } BSLX_TESTINSTREAM_EXCEPTION_TEST_END
                    LOOP2_ASSERT(i, j, X == t);
                    LOOP2_ASSERT(i, j, in);
                    LOOP2_ASSERT(i, j, in.isEmpty());
                }
            }
        }

        if (verbose) cout << "\tOn empty and invalid streams." << endl;
        {
            // testing empty and invalid streams
            Out out(20150813);
            const char *const OD  = out.data();
            const int         LOD = out.length();
            ASSERT(0 == LOD);

            for (int i = 0; i < NUM_VALUES; ++i) {
                In in(OD, LOD);  In& testInStream = in;
                LOOP_ASSERT(i, in);
                LOOP_ASSERT(i, in.isEmpty());

                // Ensure that reading from an empty or invalid input stream
                // leaves the stream invalid and the target object unchanged.

                const Obj X(VALUES[i]);  Obj t(X);  LOOP_ASSERT(i, X == t);
                BSLX_TESTINSTREAM_EXCEPTION_TEST_BEGIN(in) {
                  in.reset();
                  t.bdexStreamIn(in, VERSION);
                  LOOP_ASSERT(i, !in);    LOOP_ASSERT(i, X == t);
                  t.bdexStreamIn(in, VERSION);
                  LOOP_ASSERT(i, !in);    LOOP_ASSERT(i, X == t);
                } BSLX_TESTINSTREAM_EXCEPTION_TEST_END
            }
        }

        if (verbose) cout << "\tOn incomplete (but otherwise valid) data."
                          << endl;
        {
            // testing incomplete (but otherwise valid) data
            const Obj W1 = VA, X1 = VB, Y1 = VC;
            const Obj W2 = VB, X2 = VC, Y2 = VD;
            const Obj W3 = VC, X3 = VD, Y3 = VE;

            Out out(20150813);
            X1.bdexStreamOut(out, VERSION);
            const int LOD1 = out.length();
            X2.bdexStreamOut(out, VERSION);
            const int LOD2 = out.length();
            X3.bdexStreamOut(out, VERSION);
            const int LOD  = out.length();
            const char *const    OD   = out.data();

            for (int i = 0; i < LOD; ++i) {
                In in(OD, i);  In& testInStream = in;

//              in.setSuppressVersionCheck(1);

                BSLX_TESTINSTREAM_EXCEPTION_TEST_BEGIN(in) {
                  in.reset();
                  LOOP_ASSERT(i, in);
                  LOOP_ASSERT(i, !i == in.isEmpty());
                  Obj t1(W1), t2(W2), t3(W3);

                  if (i < LOD1) {
                      t1.bdexStreamIn(in, VERSION);
                      LOOP_ASSERT(i, !in);    LOOP_ASSERT(i, W1 == t1);
                      t2.bdexStreamIn(in, VERSION);
                      LOOP_ASSERT(i, !in);    LOOP_ASSERT(i, W2 == t2);
                      t3.bdexStreamIn(in, VERSION);
                      LOOP_ASSERT(i, !in);    LOOP_ASSERT(i, W3 == t3);
                  } else if (i < LOD2) {
                      t1.bdexStreamIn(in, VERSION);
                      LOOP_ASSERT(i, in);     LOOP_ASSERT(i, X1 == t1);
                      t2.bdexStreamIn(in, VERSION);
                      LOOP_ASSERT(i, !in);    LOOP_ASSERT(i, W2 == t2);
                      t3.bdexStreamIn(in, VERSION);
                      LOOP_ASSERT(i, !in);    LOOP_ASSERT(i, W3 == t3);
                  } else {
                      t1.bdexStreamIn(in, VERSION);
                      LOOP_ASSERT(i, in);     LOOP_ASSERT(i, X1 == t1);
                      t2.bdexStreamIn(in, VERSION);
                      LOOP_ASSERT(i, in);     LOOP_ASSERT(i, X2 == t2);
                      t3.bdexStreamIn(in, VERSION);
                      LOOP_ASSERT(i, !in);    LOOP_ASSERT(i, W3 == t3);
                  }

                              LOOP_ASSERT(i, Y1 != t1);
                  t1 = Y1;    LOOP_ASSERT(i, Y1 == t1);

                              LOOP_ASSERT(i, Y2 != t2);
                  t2 = Y2;    LOOP_ASSERT(i, Y2 == t2);

                              LOOP_ASSERT(i, Y3 != t3);
                  t3 = Y3;    LOOP_ASSERT(i, Y3 == t3);
                } BSLX_TESTINSTREAM_EXCEPTION_TEST_END
            }
        }

        // On corrupted data, the component does not define 'corrupted' data
        // (all digests are valid), so just check for bad version numbers.

        if (verbose) cout << "\tOn corrupted data." << endl;

        const Obj W;            // default value
        const Obj X(g("/01"));  // original (control) value
        const Obj Y(g("/02"));  // new (streamed-out) value

        if (verbose) cout << "\t\tGood stream (for control)." << endl;
        {
            // good version (for control)
            Out out(20150813);
            out.putVersion(VERSION);
            Y.bdexStreamOut(out, VERSION);

            const char *const OD  = out.data();
            const int         LOD = out.length();

            Obj t(X);       ASSERT(W != t); ASSERT(X == t); ASSERT(Y != t);
            In in(OD, LOD); ASSERT(in);
            int version;
            in.getVersion(version);
            ASSERT(VERSION == version);
            t.bdexStreamIn(in, version);
            ASSERT(in);
            ASSERT(W != t); ASSERT(X != t); ASSERT(Y == t);
        }

        if (verbose) cout << "\t\tBad version." << endl;
        {
            const char version = 0;  // too small ('version' must be >= 1)

            Out out(20150813);
            out.putVersion(version);
            Y.bdexStreamOut(out, version);

            const char *const OD  = out.data();
            const int         LOD = out.length();

            Obj t(X);       ASSERT(W != t); ASSERT(X == t); ASSERT(Y != t);
            In in(OD, LOD); ASSERT(in);
            in.setQuiet(!veryVerbose);
            t.bdexStreamIn(in, VERSION);
            ASSERT(!in);
            ASSERT(W != t); ASSERT(X == t); ASSERT(Y != t);
        }
        {
            const char version = 5;  // too large (current version is 1)

            Out out(20150813);
            out.putVersion(version);
            Y.bdexStreamOut(out, version);

            const char *const OD  = out.data();
            const int         LOD = out.length();

            Obj t(X);       ASSERT(W != t); ASSERT(X == t); ASSERT(Y != t);
            In in(OD, LOD); ASSERT(in);
            in.setQuiet(!veryVerbose);
            t.bdexStreamIn(in, VERSION);
            ASSERT(!in);
            ASSERT(W != t); ASSERT(X == t); ASSERT(Y != t);
        }

        if (verbose) cout << "\nTesting 'maxSupportedBdexVersion()'." << endl;
        {
            // test 'maxSupportedBdexVersion()'
            if (verbose) cout << "\tusing object syntax:" << endl;
            const Obj X;
            ASSERT(1 == X.maxSupportedBdexVersion(150813));
            if (verbose) cout << "\tusing class method syntax:" << endl;
            ASSERT(1 == Obj::maxSupportedBdexVersion(150813));
        }
      } break;
      case 9: {
        // --------------------------------------------------------------------
        // TESTING ASSIGNMENT OPERATOR
        //   We need to test the assignment operator ('operator=').
        //
        // Concerns:
        //   Any value must be assignable to an object having any initial value
        //   without affecting the rhs operand value.  Also, any object must be
        //   assignable to itself.
        //
        // Plan:
        //   Specify a set S of (unique) objects with substantial and varied
        //   differences in value.  Construct and initialize all combinations
        //   (u, v) in the cross product S X S, copy construct a control w from
        //   v, assign v to u, and assert that w == u and w == v.  Then test
        //   aliasing by copy constructing a control w from each u in S,
        //   assigning u to itself, and verifying that w == u.
        //
        // Testing:
        //   bdlde::Md5& operator=(const bdlde::Md5& rhs);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Assignment Operator"
                          << "\n===========================" << endl;

        {
            if (verbose) cout << "\nTesting Assignment u = V." << endl;
            static const struct {
                int         d_lineNum;  // source line number
                const char *d_spec;     // spec string
            } DATA[] = {
                //line  source
                //----  ------
                { L_,   ""                                     },
                { L_,   "/00"                                  },
                { L_,   "/ff"                                  },
                { L_,   "This is a test message."              },
                { L_,   "Another totally different MeSsAgE."   },
                { L_,   "Shorter msg"                          },
                { L_,   "@(*#*&@%!&&^@)!)(@"                   },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            int i;
            for (i = 0; i < NUM_DATA; ++i) {
                const int   V_LINE = DATA[i].d_lineNum;
                const char *V_SPEC = DATA[i].d_spec;
                if (veryVerbose) {
                    T_ PH(V_SPEC);
                }

                Obj mV;
                const Obj& V = mV;
                gg(&mV, V_SPEC);

                for (int j = 0; j < NUM_DATA; ++j) {
                    const int   U_LINE = DATA[j].d_lineNum;
                    const char *U_SPEC = DATA[j].d_spec;
                    if (veryVeryVerbose) {
                        T_ T_ PH(U_SPEC);
                    }

                    Obj mU;
                    const Obj& U = mU;
                    gg(&mU, U_SPEC);

                    if (veryVeryVerbose) { T_ T_ P_(V); P_(U); }

                    Obj mW(V);
                    const Obj& W = mW;  // control
                    mU = V;
                    if (veryVeryVerbose) P(U);
                    LOOP2_ASSERT(V_LINE, U_LINE, W == U);
                    LOOP2_ASSERT(V_LINE, U_LINE, W == V);
                }
            }

            if (verbose) cout << "\nTesting assignment u = u (Aliasing)."
                              << endl;

            for (i = 0; i < NUM_DATA; ++i) {
                const int   LINE = DATA[i].d_lineNum;
                const char *SPEC = DATA[i].d_spec;

                Obj mU;
                const Obj& U = mU;
                gg(&mU, SPEC);

                Obj mW(U);
                const Obj& W = mW;  // control
                mU = U;
                if (veryVerbose) { T_ P_(U); P(W); }
                LOOP_ASSERT(LINE, W == U);
            }
        }

      } break;
      case 8: {
        // --------------------------------------------------------------------
        // TESTING GENERATOR FUNCTION 'g'
        //   This will test the 'g' generator function.
        //
        // Concerns:
        //   Since 'g' is implemented almost entirely using 'gg', we need only
        //   to verify that the arguments are properly forwarded and that 'g'
        //   returns an object by value.
        //
        // Plan:
        //   For each spec in a short list of specifications, compare the
        //   object returned (by value) from the generator function, 'g(SPEC)'
        //   with the value of a newly constructed object 'mX' configured using
        //   'gg(&mX, SPEC)'.  The test also ensures that 'g' returns a
        //   distinct object by comparing the memory addresses.
        //
        // Testing:
        //   bdlde::Md5 g(const char *spec);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Generator Function 'g'" << endl
                          << "\n==============================" << endl;

        static const struct {
            int         d_lineNum;  // source line number
            const char *d_spec;     // spec string
        } DATA[] = {
            //line  spec
            //----  ----
            { L_,   ""           },
            { L_,   "~"          },
            { L_,   "A"          },
            { L_,   "B"          },
            { L_,   "C"          },
            { L_,   "D"          },
            { L_,   "E"          },
            { L_,   "A~B~C~D~E"  },
            { L_,   "ABCDE"      },
            { L_,   "ABC~DE"     },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        if (verbose) cout << "\nCompare values produced by 'g' and 'gg' "
                          << "for various inputs." << endl;

        for (int i = 0; i < NUM_DATA; ++i) {
            const int   LINE = DATA[i].d_lineNum;
            const char *SPEC = DATA[i].d_spec;

            if (veryVerbose) {
                T_ P_(i); PH(SPEC);
            }
            Obj mX;
            const Obj& X = gg(&mX, SPEC);
            if (veryVeryVerbose) {
                cout << "\t\t g = " << g(SPEC) << endl;
                cout << "\t\tgg = " << X       << endl;
            }

            LOOP_ASSERT(LINE, X == g(SPEC));
        }

        if (verbose) cout << "\nConfirm return-by-value." << endl;
        {
            const char *SPEC = "ABCDE";

            ASSERT(sizeof(Obj) == sizeof g(SPEC));  // compile-time fact

            Obj mX;                                 // runtime tests
            Obj& r1 = gg(&mX, SPEC);
            Obj& r2 = gg(&mX, SPEC);
            const Obj& r3 = g(SPEC);
            const Obj& r4 = g(SPEC);
            ASSERT(&r2 == &r1);
            ASSERT(&mX == &r1);
            ASSERT(&r4 != &r3);
            ASSERT(&mX != &r3);
        }

      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING COPY CONSTRUCTOR
        //   This will test the copy constructor.
        //
        // Concerns:
        //   Any value must be able to be copy constructed without affecting
        //   its argument.
        //
        // Plan:
        //   Specify a set S whose elements have substantial and varied
        //   differences in value.  For each element in S, construct and
        //   initialize identically-values objects W and X using tested methods
        //   (in this case, the 'gg' function).  Then copy construct an object
        //   Y from X, and use the equality operator to assert that both X and
        //   Y have the same value as W.
        //
        // Testing:
        //   bdlde::Md5(const bdlde::Md5& original);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Copy Constructor"
                          << "\n========================" << endl;

        {
            if (verbose) cout << "\nTesting copy constructor." << endl;

            static const struct {
                int         d_lineNum;  // source line number
                const char *d_spec;     // spec string
            } DATA[] = {
                //line  spec
                //----  ----
                { L_,   ""                                     },
                { L_,   "/00"                                  },
                { L_,   "/ff"                                  },
                { L_,   "This is a test message."              },
                { L_,   "Another totally different MeSsAgE."   },
                { L_,   "Shorter msg"                          },
                { L_,   "@(*#*&@%!&&^@)!)(@"                   },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE = DATA[i].d_lineNum;
                const char *SPEC = DATA[i].d_spec;

                Obj mW;
                const Obj& W = gg(&mW, SPEC);

                Obj mX;
                const Obj& X = gg(&mX, SPEC);

                Obj mY(X);
                const Obj& Y = mY;

                if (veryVerbose) {
                    T_ P_(W); P_(X); P_(Y); PH(SPEC);
                }
                LOOP_ASSERT(LINE, X == W);
                LOOP_ASSERT(LINE, Y == W);
            }
        }

      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING EQUALITY OPERATORS
        //   This will test the equality operations ('operator==' and
        //   'operator!=').
        //
        // Concerns:
        //   We want to make sure that 'operator==' returns false for objects
        //   that are very similar but still different, but returns true for
        //   objects that are exactly the same.  Likewise, we want to make sure
        //   that 'operator!=' returns true for objects that are very similar
        //   but still different, but returns false for objects that are
        //   exactly the same.
        //
        // Plan:
        //   Construct a set of specs containing similar but different data
        //   values.  Then loop through the cross product of the test data.
        //   For each tuple, generate two objects 'U' and 'V' using the
        //   previously tested 'gg' function.  Use the '==' and '!=' operators
        //   and check their return value for correctness.
        //
        // Testing:
        //   bool operator==(const bdlde::Md5& lhs, const bdlde::Md5& rhs);
        //   bool operator!=(const bdlde::Md5& lhs, const bdlde::Md5& rhs);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Equality Operators" << endl
                          << "\n==========================" << endl;

        {
            if (verbose) cout << "\nCompare each pair of similar values "
                              << "(u, v) in S X S." << endl;

            static const struct {
                int         d_lineNum;  // source line number
                const char *d_spec;     // spec string
            } DATA[] = {
                //line  spec
                //----  ----
                { L_,   ""                            },

                { L_,   "/01"                         },
                { L_,   "/02"                         },
                { L_,   "/03"                         },
                { L_,   "/04"                         },
                { L_,   "/05"                         },
                { L_,   "/06"                         },
                { L_,   "/07"                         },
                { L_,   "/08"                         },

                { L_,   "/01/02"                      },
                { L_,   "/02/01"                      },
                { L_,   "/03/04"                      },
                { L_,   "/04/03"                      },
                { L_,   "/05/06"                      },
                { L_,   "/06/05"                      },
                { L_,   "/07/08"                      },
                { L_,   "/08/07"                      },

                { L_,   "/01/01"                      },
                { L_,   "/02/02"                      },
                { L_,   "/03/03"                      },
                { L_,   "/04/04"                      },
                { L_,   "/05/05"                      },
                { L_,   "/06/06"                      },
                { L_,   "/07/07"                      },
                { L_,   "/08/08"                      },

                { L_,   "/01/02/03/04/05/06/07/08"    },
                { L_,   "/01/02/03/04~/05/06/07/08"   },
            };
            int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   U_LINE = DATA[i].d_lineNum;
                const char *U_SPEC = DATA[i].d_spec;

                Obj mU;
                const Obj& U = gg(&mU, U_SPEC);
                if (veryVerbose) {
                    T_ P_(i); P_(U); PH(U_SPEC);
                }

                for (int j = 0; j < NUM_DATA; ++j) {
                    const int   V_LINE = DATA[j].d_lineNum;
                    const char *V_SPEC = DATA[j].d_spec;

                    Obj mV;
                    const Obj& V = gg(&mV, V_SPEC);
                    if (veryVeryVerbose) {
                        T_ T_ P_(j); P_(V); PH(V_SPEC);
                    }

                    const bool isSame = (i == j);
                    LOOP2_ASSERT(U_LINE, V_LINE,  isSame == (U == V));
                    LOOP2_ASSERT(U_LINE, V_LINE, !isSame == (U != V));
                }
            }
        }

      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING OUTPUT (<<) OPERATOR
        //   We need to test the '<<' operator.
        //
        // Concerns:
        //   We want to make sure that the object is written to the stream
        //   correctly in the expected format (with '0x' prepended to the hex
        //   value).
        //
        // Plan:
        //   The '<<' operator depends on the 'print' member function.
        //   So, we need to test 'print' before testing 'operator<<'.
        //   This test is broken up into two parts:
        //      1. Testing of 'print'
        //      2. Testing of 'operator<<'
        //
        //   Each test vector in DATA contains STR, its LEN, the expected MD5
        //   value and also an expected output FMT.  For each datum, construct
        //   an independent object 'mX' and call 'update' with STR and LEN.
        //   Assert that the object contains the expected MD5 value.  Create an
        //   'ostringstream' object and use the 'print' function to stream
        //   'mX'.  Compare the contents of the stream object with the expected
        //   FMT value.
        //
        //   To test the '<<' operator, construct an independent object 'obj'
        //   for each test vector in DATA and then call 'update' with STR and
        //   LEN.  Assert that the object contains the expected MD5 value.
        //   Create an 'ostringstream' object and use the '<<' operator to
        //   stream 'obj'.  Finally, compare the contents of the stream object
        //   with the expected FMT value.
        //
        // Testing:
        //   bsl::ostream& print(bsl::ostream& stream) const;
        //   bsl::ostream& operator<<(bsl::ostream&, const bdlde::Md5&);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Output (<<) Operator"
                          << "\n===========================" << endl;

        static const struct {
            int         d_lineNum;  // source line number
            const char *d_str;      // source string
            int         d_length;   // length of source input
            const char *d_result;   // expected result string
            const char *d_fmt;
        } DATA[] = {
            //line  source    length  digest               output format
            //----  ------    ------  -------------------  ------------------
            { L_,   "",           0,  "d41d8cd98f00b204"
                                      "e9800998ecf8427e", "d41d8cd98f00b204"
                                                          "e9800998ecf8427e"},
            { L_,   "a",          1,  "0cc175b9c0f1b6a8"
                                      "31c399e269772661", "0cc175b9c0f1b6a8"
                                                          "31c399e269772661" },
            { L_,   "abc",        3,  "900150983cd24fb0"
                                      "d6963f7d28e17f72", "900150983cd24fb0"
                                                          "d6963f7d28e17f72" },
            { L_,   "message digest",
                                  14, "f96b697d7cb7938d"
                                      "525a2f31aaf161d0", "f96b697d7cb7938d"
                                                          "525a2f31aaf161d0" },
            { L_,   "abcdefghijklmnopqrstuvwxyz",
                                  26, "c3fcd3d76192e400"
                                      "7dfb496cca67e13b", "c3fcd3d76192e400"
                                                          "7dfb496cca67e13b" },
            { L_,   "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                    "abcdefghijklmnopqrstuvwxyz0123456789",
                                  62, "d174ab98d277d9f5"
                                      "a5611c2c9f419d9f", "d174ab98d277d9f5"
                                                          "a5611c2c9f419d9f" },
            { L_,   "1234567890123456789012345678901234567890"
                    "1234567890123456789012345678901234567890",
                                  80, "57edf4a22be3c955"
                                      "ac49da2e2107b67a", "57edf4a22be3c955"
                                                          "ac49da2e2107b67a" },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;
        const int SIZE     = 128;
        int i;

        if (verbose) cout << "\n1. Testing 'print'." << endl;
        for (i = 0; i < NUM_DATA; ++i) {
            const int       LINE    = DATA[i].d_lineNum;
            const char     *STR     = DATA[i].d_str;
            const int       LEN     = DATA[i].d_length;
            const char     *RESULT  = DATA[i].d_result;
            const char     *FMT     = DATA[i].d_fmt;

            Obj mX;
            const Obj& X = mX;
            mX.update(STR, LEN);

            bdlde::Md5::Md5Digest r;
            X.loadDigest(&r);
            LOOP_ASSERT(LINE, verify(r, RESULT));

            char buf[SIZE];
            memset(buf, 0, sizeof(buf));
            ostringstream outbuf(bsl::string(buf, SIZE));
            X.print(outbuf);
            if (veryVerbose) { T_ P_(X); P_(FMT); P(outbuf.str()); }

            LOOP_ASSERT(LINE,
                        0 == bsl::strncmp(outbuf.str().c_str(), FMT, 32));
        }

        if (verbose) cout << "\n2. Testing 'operator<<'." << endl;
        for (i = 0; i < NUM_DATA; ++i) {
            const int       LINE   = DATA[i].d_lineNum;
            const char     *STR    = DATA[i].d_str;
            const int       LEN    = DATA[i].d_length;
            const char     *RESULT = DATA[i].d_result;
            const char     *FMT    = DATA[i].d_fmt;

            Obj mX;
            const Obj& X = mX;
            mX.update(STR, LEN);

            bdlde::Md5::Md5Digest r;
            X.loadDigest(&r);
            LOOP_ASSERT(LINE, verify(r, RESULT));

            char buf[SIZE];
            memset(buf, 0, sizeof(buf));
            ostringstream outbuf(bsl::string(buf, SIZE));
            outbuf << X;
            if (veryVerbose) { T_ P_(X); P_(FMT); P(outbuf.str()); }
            LOOP_ASSERT(LINE,
                        0 == bsl::strncmp(outbuf.str().c_str(), FMT, 32));
        }

      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING BASIC ACCESSORS
        //   In this test case, we will test the basic accessors.
        //
        // Concerns:
        //   We want to verify that, for each unique object value, the basic
        //   accessors return the correct values.
        //
        // Plan:
        //   For each test vector in DATA, construct an object 'mX' using the
        //   default constructor.  Then call 'mX.update' using the current STR
        //   and LENGTH.  Ensure that the MD5 digest returned by 'loadDigest'
        //   is the same as the expected digest.
        //
        // Testing:
        //   void loadDigest(bdlde::Md5::Md5Digest result) const;
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Basic Accessors"
                          << "\n=======================" << endl;
        static const struct {
            int         d_lineNum;  // source line number
            const char *d_str;      // source string
            int         d_length;   // length of source input
            const char *d_result;   // expected digest
        } DATA[] = {
            //line  source        length  digest
            //----  ------        ------  ----------------------------------
            { L_,   "",                0, "d41d8cd98f00b204e9800998ecf8427e" },
            { L_,   "a",               1, "0cc175b9c0f1b6a831c399e269772661" },
            { L_,   "abc",             3, "900150983cd24fb0d6963f7d28e17f72" },
            { L_,   "message digest", 14, "f96b697d7cb7938d525a2f31aaf161d0" },
            { L_,   "abcdefghijklmnopqrstuvwxyz",
                                      26, "c3fcd3d76192e4007dfb496cca67e13b" },
            { L_,   "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                    "abcdefghijklmnopqrstuvwxyz0123456789",
                                      62, "d174ab98d277d9f5a5611c2c9f419d9f" },
            { L_,   "1234567890123456789012345678901234567890"
                    "1234567890123456789012345678901234567890",
                                      80, "57edf4a22be3c955ac49da2e2107b67a" },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        if (verbose) cout << "\nTesting 'loadDigest'." << endl;
        {
            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE   = DATA[i].d_lineNum;
                const char *STR    = DATA[i].d_str;
                const int   LENGTH = DATA[i].d_length;
                const char *RESULT = DATA[i].d_result;

                Obj mX;
                const Obj& X = mX;
                mX.update(STR, LENGTH);

                if (veryVerbose) {
                    T_ P_(LENGTH);
                    T_ P(X);
                }

                bdlde::Md5::Md5Digest r;
                X.loadDigest(&r);
                LOOP_ASSERT(LINE, verify(r, RESULT));
            }
        }

        if (verbose) cout << "\nTesting 'loadDigestAndReset'." << endl;
        {
            const char *URESULT = DATA[0].d_result;
            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE   = DATA[i].d_lineNum;
                const char *STR    = DATA[i].d_str;
                const int   LENGTH = DATA[i].d_length;
                const char *RESULT = DATA[i].d_result;

                Obj mX;
                const Obj& X = mX;
                mX.update(STR, LENGTH);

                if (veryVerbose) {
                    T_ P_(LENGTH);
                    T_ P(X);
                }

                // check the digest
                bdlde::Md5::Md5Digest r;
                mX.loadDigestAndReset(&r);
                LOOP_ASSERT(LINE, verify(r, RESULT));

                // make sure reset is done properly
                X.loadDigest(&r);
                LOOP_ASSERT(LINE, verify(r, URESULT));
            }
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING PRIMITIVE GENERATOR FUNCTION 'gg'
        //   This will test the 'gg' generator function.
        //
        // Concerns:
        //   We want to verify (1) that valid generator syntax produces
        //   expected results, and (2) that invalid syntax is detected and
        //   reported.
        //
        // Plan:
        //   For each enumerated sequence of 'spec' values, ordered by
        //   increasing 'spec' length, use the primitive generator function
        //   'gg' to set the state of a newly created object.  Verify that 'gg'
        //   returns a valid reference to the modified object and, using basic
        //   accessors, that the value of the object is as expected.  Note that
        //   we are testing the parser only; the primary manipulators are
        //   already assumed to work.
        //
        //   This test case also tests the 'ggg' function using invalid 'spec'
        //   values, to ensure that error messages are caught and reported
        //   correctly.
        //
        // Testing:
        //   int ggg(bdlde::Md5 *object, const char *spec, int vF = 1);
        //   bdlde::Md5& gg(bdlde::Md5 *object, const char *spec);
        // --------------------------------------------------------------------
        if (verbose)
            cout << endl << "Testing Primitive Generator Function 'gg'"
                 << endl << "========================================="
                 << endl;

        {
            if (verbose) cout << "\nTesting generator on valid specs." << endl;

            static const struct {
                int         d_lineNum;      // source line number
                const char *d_spec;         // specification string
                const char *d_interpreted;  // interpreted string
                int         d_length;       // length of 'd_interpreted'
                const char *d_result;
            } DATA[] = {
                //line  spec     interpreted  length  digest
                //----  ----     -----------  ------  ----------------
                { L_,   "",      "",          0,    "d41d8cd98f00b204"
                                                    "e9800998ecf8427e"},//len 0
                { L_,   "a",     "a",         1,    "0cc175b9c0f1b6a8"
                                                    "31c399e269772661"},//len 1
                { L_,   "//",    "/",         1,    "6666cd76f9695646"
                                                    "9e7be39d750cc7d9"},//len 2
                { L_,   "ab",    "ab",        2,    "187ef4436122d1cc"
                                                    "2f40dc2b92f0eba0"},//len 2
                { L_,   "/00",   "\x00",      1,    "93b885adfe0da089"
                                                    "cdf634904fd59f71"},//len 3
                { L_,   "//c",   "/c",        2,    "c840eb2586c15e45"
                                                    "28e3eef23eabb92d"},//len 3
                { L_,   "abc",   "abc",       3,    "900150983cd24fb0"
                                                    "d6963f7d28e17f72"},//len 3
                { L_,   "////",  "//",        2,    "7bc0ee636b3b8348"
                                                    "4fc3b9348863bd22"},//len 4
                { L_,   "//ab",  "/ab",       3,    "ff594c70ffa0a50d"
                                                    "7926a419007a6dd7"},//len 4
                { L_,   "abcd",  "abcd",      4,    "e2fc714c4727ee93"
                                                    "95f324cd2e7f331f"},//len 4
                { L_,   "/20//", " /",        2,    "cce9940712af9392"
                                                    "818df7ae2ec368a8"},//len 5
                { L_,   "/20ab", " ab",       3,    "ac32dfca0cf310a8"
                                                    "2cf1c6a7d677b531"},//len 5
                { L_,   "//abc", "/abc",      4,    "482a7143ac747eff"
                                                    "5e5a5992a6016d65"},//len 5
                { L_,   "abcde", "abcde",     5,    "ab56b4d92b40713a"
                                                    "cc5af89985d4b786"},//len 5
                { L_,   "/41/42","AB",        2,    "b86fc6b051f63d73"
                                                    "de262d4c34e3a0a9"},//len 6
                { L_,   "/41//B","A/B",       3,    "e649fc2cbb4c0992"
                                                    "3a41281ff3dafebd"},//len 6
                { L_,   "//A//B","/A/B",      4,    "dad5a0a365c61f8a"
                                                    "86cad9dfc25f8b3a"},//len 6
                { L_,   "A//BCD","A/BCD",     5,    "d292cc29fb1e8339"
                                                    "98571d94857d8936"},//len 6
                { L_,   "ABCDEF","ABCDEF",    6,    "8827a41122a5028b"
                                                    "9808c7bf84b9fcf6"},//len 6
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE        = DATA[i].d_lineNum;
                const char *SPEC        = DATA[i].d_spec;
                const char *INTERPRETED = DATA[i].d_interpreted;
                const int   LENGTH      = DATA[i].d_length;
                const char *RESULT      = DATA[i].d_result;

                Obj mX;
                const Obj& X = gg(&mX, SPEC);  // original spec

                Obj mY(INTERPRETED, LENGTH);
                const Obj& Y = mY;

                if (veryVerbose) {
                    T_ PH(SPEC);
                    T_ PH(INTERPRETED);
                }
                if (veryVeryVerbose) {
                    T_ T_ P(X);
                    T_ T_ P(Y);
                }

                bdlde::Md5::Md5Digest r1;
                X.loadDigest(&r1);
                LOOP_ASSERT(LINE, verify(r1, RESULT));

                bdlde::Md5::Md5Digest r2;
                Y.loadDigest(&r2);
                LOOP_ASSERT(LINE, verify(r2, RESULT));

            }
        }

        {
            if (verbose) cout << "\nTesting generator on invalid specs."
                              << endl;

            static const struct {
                int         d_lineNum;  // source line number
                const char *d_spec;     // specification string
                int         d_index;    // offending character index
            } DATA[] = {
                // Most of the boundary checks are selected based on the
                // ASCII table values.

                //line  spec             index
                //----  ----             -----
                { L_,   "",              -1     },  // control

                { L_,   "r",             -1     },  // control
                { L_,   "/",              1     },
                { L_,   "/\\",            1     },

                { L_,   "//",            -1     },  // control
                { L_,   "/.",             1     },
                { L_,   "/0",             2     },
                { L_,   "/0:",            2     },
                { L_,   "/0`",            2     },
                { L_,   "/0g",            2     },

                { L_,   "/00",           -1     },  // control
                { L_,   "/0f",           -1     },  // control
                { L_,   "/ff",           -1     },  // control
                { L_,   "/:",             1     },
                { L_,   "/`",             1     },
                { L_,   "/g",             1     },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE  = DATA[i].d_lineNum;
                const char *SPEC  = DATA[i].d_spec;
                const int   INDEX = DATA[i].d_index;

                Obj mX;

                if (veryVerbose) {
                    T_ P_(INDEX); PH(SPEC);
                }

                int result = ggg(&mX, SPEC, veryVerbose);
                LOOP_ASSERT(LINE, INDEX == result);
            }
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING PRIMARY MANIPULATORS (BOOTSTRAP)
        //   This test will verify that the primary manipulators are working as
        //   expected.
        //
        // Concerns:
        //   When an arbitrary string is passed to the object, the correct MD5
        //   digest should be returned.  Also, the default constructor should
        //   create an object with a digest of
        //   d41d8cd98f00b204e9800998ecf8427e.  We also need to make sure that
        //   the destructor works.
        //
        // Plan:
        //   First, verify the default constructor by testing the value of the
        //   resulting object.
        //
        //   Next, verify that the 'update' member function works by
        //   constructing a series of independent objects using the default
        //   constructor and running 'update' using increasing string lengths.
        //   Verify the MD5 digest in the object using the basic accessor.
        //
        //   Note that the destructor is exercised on each configuration as the
        //   object being tested leaves scope.
        //
        // Testing:
        //   bdlde::Md5();
        //   ~bdlde::Md5();
        //   BOOTSTRAP: void update(const void *data, int length);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Primary Manipulators"
                          << "\n============================" << endl;

        bdlde::Md5::Md5Digest r;
        if (verbose) cout << "\nTesting default constructor." << endl;
        {
            const char* result = "d41d8cd98f00b204e9800998ecf8427e";
            Obj mX;
            const Obj& X = mX;
            if (veryVerbose) { T_ P(X); }
            X.loadDigest(&r);
            ASSERT(verify(r, result));
        }

        if (verbose) cout << "\nTesting 'update'." << endl;
        {
            if (veryVerbose) cout << "\tUsing string with length 0." << endl;

            const char* DATA = "";
            const char* result = "d41d8cd98f00b204e9800998ecf8427e";
            Obj mX;
            const Obj& X = mX;
            mX.update(DATA, 0);
            if (veryVeryVerbose) { T_ T_ P(X); }
            X.loadDigest(&r);
            ASSERT(verify(r, result));
        }

        {
            if (veryVerbose) cout << "\tUsing string with length 1." << endl;

            const char* DATA = "a";
            const char* result = "0cc175b9c0f1b6a831c399e269772661";
            Obj mX;
            const Obj& X = mX;
            mX.update(DATA, 1);
            if (veryVeryVerbose) { T_ T_ P(X); }
            X.loadDigest(&r);
            ASSERT(verify(r, result));
        }

        {
            if (veryVerbose) cout << "\tUsing string with length 2." << endl;

            const char* DATA = "ab";
            const char* result = "187ef4436122d1cc2f40dc2b92f0eba0";
            Obj mX;
            const Obj& X = mX;
            mX.update(DATA, 2);
            if (veryVeryVerbose) { T_ T_ P(X); }
            X.loadDigest(&r);
            ASSERT(verify(r, result));
        }

        {
            if (veryVerbose) cout << "\tUsing string with length 3." << endl;

            const char* DATA = "abc";
            const char* result = "900150983cd24fb0d6963f7d28e17f72";
            Obj mX;
            const Obj& X = mX;
            mX.update(DATA, 3);
            if (veryVeryVerbose) { T_ T_ P(X); }
            X.loadDigest(&r);
            ASSERT(verify(r, result));
        }

        {
            if (veryVerbose) cout << "\tUsing string with length 4." << endl;

            const char* DATA = "abcd";
            const char* result = "e2fc714c4727ee9395f324cd2e7f331f";
            Obj mX;
            const Obj& X = mX;
            mX.update(DATA, 4);
            if (veryVeryVerbose) { T_ T_ P(X); }
            X.loadDigest(&r);
            ASSERT(verify(r, result));
        }

        {
            if (veryVerbose) cout << "\tUsing string with length 5." << endl;

            const char* DATA = "abcde";
            const char* result = "ab56b4d92b40713acc5af89985d4b786";
            Obj mX;
            const Obj& X = mX;
            mX.update(DATA, 5);
            if (veryVeryVerbose) { T_ T_ P(X); }
            X.loadDigest(&r);
            ASSERT(verify(r, result));
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //   This test exercises basic functionality, but tests nothing.
        //
        // Concerns:
        //   We want to demonstrate a base-line level of correct operation of
        //   the following methods and operators:
        //     - default and copy constructors.
        //     - the assignment operator (including aliasing).
        //     - equality operators: 'operator==' and 'operator!='.
        //     - primary manipulators: 'update' and 'reset'.
        //     - basic accessors: 'loadDigest'.
        //
        // Plan:
        //   Create four test objects using the default, initializing, and copy
        //   constructors.  Exercise the basic value-semantic methods and the
        //   equality operators using the test objects.  Invoke the primary
        //   manipulator [5, 6, 7], copy constructor [2, 4], assignment
        //   operator without [9, 9] and with [10] aliasing.  Use the basic
        //   accessors to verify the expected results.  Display object values
        //   frequently in verbose mode.  Note that 'VA', 'VB' and 'VC' denote
        //   unique, but otherwise arbitrary, object values, while 'U' denotes
        //   the valid, but "unknown", default object value.
        //
        //    1. Create an object x1 (init to VA)   { x1:VA                  }
        //    2. Create an object x2 (copy of x1)   { x1:VA x2:VA            }
        //    3. Create an object x3 (default ctor) { x1:VA x2:VA x3:U       }
        //    4. Create an object x4 (copy of x3)   { x1:VA x2:VA x3:U  x4:U }
        //    5. Set x3 using 'update' (set to VB)  { x1:VA x2:VA x3:VB x4:U }
        //    6. Change x1 using 'reset'            { x1:U  x2:VA x3:VB x4:U }
        //    7. Change x1 ('update', set to VC)    { x1:VC x2:VA x3:VB x4:U }
        //    8. Assign x2 = x1                     { x1:VC x2:VC x3:VB x4:U }
        //    9. Assign x2 = x3                     { x1:VC x2:VB x3:VB x4:U }
        //   10. Assign x1 = x1 (aliasing)          { x1:VC x2:VB x3:VB x4:U }
        //
        // Testing:
        //   This test case exercises basic value-semantic functionality.
        // --------------------------------------------------------------------

        if (verbose) cout << "\nBREATHING TEST"
                          << "\n==============" << endl;

        static const struct {
            int         d_lineNum;  // source line number
            const char *d_str;      // source string
            int         d_length;   // length of source input
            const char *d_result;   // expected digest
        } DATA[] = {
            //line  source        length  digest
            //----  ------        ------  ----------------------------------
            { L_,   "",                0, "d41d8cd98f00b204e9800998ecf8427e" },
            { L_,   "a",               1, "0cc175b9c0f1b6a831c399e269772661" },
            { L_,   "ab",              2, "187ef4436122d1cc2f40dc2b92f0eba0" },
            { L_,   "abc",             3, "900150983cd24fb0d6963f7d28e17f72" },
        };

        const char  *SA = DATA[1].d_str,
                    *SB = DATA[2].d_str,
                    *SC = DATA[3].d_str;
        const int    LA = DATA[1].d_length,
                     LB = DATA[2].d_length,
                     LC = DATA[3].d_length;
        const char  *VA = DATA[1].d_result,
                    *VB = DATA[2].d_result,
                    *VC = DATA[3].d_result,
                    *VU = DATA[0].d_result;

        bdlde::Md5::Md5Digest r;
        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 1. Create an object x1 (init to SA)."
                             "\t\t{ x1:SA }" << endl;
        Obj mX1(SA, LA);
        const Obj& X1 = mX1;
        if (verbose) { cout << "\t"; P(X1); }

        if (verbose) cout << "\ta. Check initial state of x1." << endl;
        mX1.loadDigest(&r);
        ASSERT(verify(r, VA));

        if (verbose) cout << "\tb. Try equality operators: x1 <op> x1."
                          << endl;
        ASSERT(1 == (X1 == X1));
        ASSERT(0 == (X1 != X1));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 2. Create an object x2 (copy of x1)."
                             "\t\t{ x1:SA x2:SA }" << endl;
        Obj mX2(X1);
        const Obj& X2 = mX2;
        if (verbose) { cout << '\t'; P(X2); }

        if (verbose) cout << "\ta. Check initial state of x2." << endl;
        mX2.loadDigest(&r);
        ASSERT(verify(r, VA));

        if (verbose) cout << "\tb. Try equality operators: "
                             "x2 <op> x1, x2." << endl;
        ASSERT(1 == (X2 == X1));        ASSERT(0 == (X2 != X1));
        ASSERT(1 == (X2 == X2));        ASSERT(0 == (X2 != X2));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 3. Create an object x3 (default ctor)."
                             "\t\t{ x1:SA x2:SA x3:U }" << endl;
        Obj mX3;
        const Obj& X3 = mX3;
        if (verbose) { cout << '\t'; P(X3); }

        if (verbose) cout << "\ta. Check initial state of x3." << endl;
        mX3.loadDigest(&r);
        ASSERT(verify(r, VU));

        if (verbose) cout << "\tb. Try equality operators: "
                             "x3 <op> x1, x2, x3." << endl;
        ASSERT(0 == (X3 == X1));        ASSERT(1 == (X3 != X1));
        ASSERT(0 == (X3 == X2));        ASSERT(1 == (X3 != X2));
        ASSERT(1 == (X3 == X3));        ASSERT(0 == (X3 != X3));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 4. Create an object x4 (copy of x3)."
                             "\t\t{ x1:SA x2:SA x3:U x4:U }" << endl;
        Obj mX4(X3);
        const Obj& X4 = mX4;
        if (verbose) { cout << '\t'; P(X4); }
        mX4.loadDigest(&r);
        ASSERT(verify(r, VU));

        if (verbose) cout << "\ta. Check initial state of x4." << endl;

        if (verbose) cout << "\tb. Try equality operators: "
                             "x4 <op> x1, x2, x3, x4." << endl;
        ASSERT(0 == (X4 == X1));        ASSERT(1 == (X4 != X1));
        ASSERT(0 == (X4 == X2));        ASSERT(1 == (X4 != X2));
        ASSERT(1 == (X4 == X3));        ASSERT(0 == (X4 != X3));
        ASSERT(1 == (X4 == X4));        ASSERT(0 == (X4 != X4));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 5. Set x3 using 'update' (set to VB)."
                             "\t\t{ x1:SA x2:SA x3:SB x4:U }" << endl;
        mX3.update(SB, LB);
        if (verbose) { cout << '\t'; P(X3); }

        if (verbose) cout << "\ta. Check new state of x3." << endl;
        mX3.loadDigest(&r);
        ASSERT(verify(r, VB));

        if (verbose) cout << "\tb. Try equality operators: "
                             "x3 <op> x1, x2, x3, x4." << endl;
        ASSERT(0 == (X3 == X1));        ASSERT(1 == (X3 != X1));
        ASSERT(0 == (X3 == X2));        ASSERT(1 == (X3 != X2));
        ASSERT(1 == (X3 == X3));        ASSERT(0 == (X3 != X3));
        ASSERT(0 == (X3 == X4));        ASSERT(1 == (X3 != X4));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 6. Change x1 using 'reset'."
                             "\t\t{ x1:U x2:SA x3:SB x4:U }" << endl;
        mX1.reset();
        if (verbose) { cout << '\t'; P(X1); }

        if (verbose) cout << "\ta. Check new state of x1." << endl;
        mX1.loadDigest(&r);
        ASSERT(verify(r, VU));

        if (verbose) cout << "\tb. Try equality operators: "
                             "x1 <op> x1, x2, x3, x4." << endl;
        ASSERT(1 == (X1 == X1));        ASSERT(0 == (X1 != X1));
        ASSERT(0 == (X1 == X2));        ASSERT(1 == (X1 != X2));
        ASSERT(0 == (X1 == X3));        ASSERT(1 == (X1 != X3));
        ASSERT(1 == (X1 == X4));        ASSERT(0 == (X1 != X4));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 7. Change x1 ('update', set to VC)."
                             "\t\t{ x1:SC x2:SA x3:SB x4:U }" << endl;
        mX1.update(SC, LC);
        if (verbose) { cout << '\t'; P(X1); }

        if (verbose) cout << "\ta. Check new state of x1." << endl;
        mX1.loadDigest(&r);
        ASSERT(verify(r, VC));

        if (verbose) cout << "\tb. Try equality operators: "
                             "x1 <op> x1, x2, x3, x4." << endl;
        ASSERT(1 == (X1 == X1));        ASSERT(0 == (X1 != X1));
        ASSERT(0 == (X1 == X2));        ASSERT(1 == (X1 != X2));
        ASSERT(0 == (X1 == X3));        ASSERT(1 == (X1 != X3));
        ASSERT(0 == (X1 == X4));        ASSERT(1 == (X1 != X4));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 8. Assign x2 = x1."
                             "\t\t{ x1:SC x2:SC x3:SB x4:U }" << endl;
        mX2 = X1;
        if (verbose) { cout << '\t'; P(X2); }

        if (verbose) cout << "\ta. Check new state of x2." << endl;
        mX2.loadDigest(&r);
        ASSERT(verify(r, VC));

        if (verbose) cout << "\tb. Try equality operators: "
                             "x2 <op> x1, x2, x3, x4." << endl;
        ASSERT(1 == (X2 == X1));        ASSERT(0 == (X2 != X1));
        ASSERT(1 == (X2 == X2));        ASSERT(0 == (X2 != X2));
        ASSERT(0 == (X2 == X3));        ASSERT(1 == (X2 != X3));
        ASSERT(0 == (X2 == X4));        ASSERT(1 == (X2 != X4));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 9. Assign x2 = x3."
                             "\t\t{ x1:SC x2:SB x3:SB x4:U }" << endl;
        mX2 = X3;
        if (verbose) { cout << '\t'; P(X2); }

        if (verbose) cout << "\ta. Check new state of x2." << endl;
        mX2.loadDigest(&r);
        ASSERT(verify(r, VB));

        if (verbose) cout << "\tb. Try equality operators: "
                             "x2 <op> x1, x2, x3, x4." << endl;
        ASSERT(0 == (X2 == X1));        ASSERT(1 == (X2 != X1));
        ASSERT(1 == (X2 == X2));        ASSERT(0 == (X2 != X2));
        ASSERT(1 == (X2 == X3));        ASSERT(0 == (X2 != X3));
        ASSERT(0 == (X2 == X4));        ASSERT(1 == (X2 != X4));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 10. Assign x1 = x1."
                             "\t\t{ x1:SC x2:SB x3:SB x4:U }" << endl;
        mX1 = X1;
        if (verbose) { cout << '\t'; P(X1); }

        if (verbose) cout << "\ta. Check new state of x1." << endl;
        mX1.loadDigest(&r);
        ASSERT(verify(r, VC));

        if (verbose) cout << "\tb. Try equality operators: "
                             "x1 <op> x1, x2, x3, x4." << endl;
        ASSERT(1 == (X1 == X1));        ASSERT(0 == (X1 != X1));
        ASSERT(0 == (X1 == X2));        ASSERT(1 == (X1 != X2));
        ASSERT(0 == (X1 == X3));        ASSERT(1 == (X1 != X3));
        ASSERT(0 == (X1 == X4));        ASSERT(1 == (X1 != X4));

      } break;
      case -1: {
        // --------------------------------------------------------------------
        // PERFORMANCE TEST
        //
        // Concerns:
        //   We want to ensure that the performance of the 'update' method is
        //   close to the performance of the 'openssl' implementation.
        //   However, due to the fact that 3rd party libraries should not be
        //   linked into the test driver, only a sanity check against the speed
        //   of 'openssl' will be performed here.  The 'openssl' implementation
        //   speed in this sanity check comes from performance test conducted
        //   separately.
        //
        // Plan:
        //   Using 'bsls_stopwatch', the run time of the 'update' method will
        //   be tallied over 1 million iterations.  This value will be computed
        //   10 times and averaged.  After, it will be printed out for sanity
        //   check against the run time of the 'openssl' implementation.
        //
        // Testing:
        //   void update(const void *data, int length);  // performance
        // --------------------------------------------------------------------

        if (verbose) cout << "\nPERFORMANCE TEST"
                          << "\n================" << endl;

        const char TEST[81] = "1234567890123456789012345678901234567890"
                              "1234567890123456789012345678901234567890";
        const int SIZE = 80;
        const int NUMITERATIONS =  1000000;
        const int NUMAVERAGE = 10;

        double total_bde = 0;
        {
            bsls::Stopwatch timer;

            for (int i = 0; i < NUMAVERAGE; i++) {
                timer.reset();
                Obj digest;

                timer.start();
                for (int i = 0; i < NUMITERATIONS; i++) {
                    //digest.reset();
                    digest.update(TEST, SIZE);
                }
                timer.stop();

                bsl::cout << "BDE MD5: " << timer.elapsedTime() << bsl::endl;
                total_bde += timer.elapsedTime();
            }
            bsl::cout << "average: " << total_bde / NUMAVERAGE << bsl::endl;
        }

        bsl::cout << "Typical openssl performance on Sun: 0.84s" << bsl::endl;
        bsl::cout << "Typical openssl performance on IBM: 0.66s" << bsl::endl;
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
// Copyright 2015 Bloomberg Finance L.P.
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
