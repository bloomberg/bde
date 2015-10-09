// bdlde_crc32.t.cpp                                                  -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------


#include <bdlde_crc32.h>

#include <bslx_testoutstream.h>                 // for testing only
#include <bslx_testinstream.h>                  // for testing only
#include <bslx_testinstreamexception.h>         // for testing only
#include <bsls_stopwatch.h>                     // for testing only

#include <bsl_algorithm.h>   // sort()
#include <bsl_cstdlib.h>     // atoi()
#include <bsl_cstring.h>     // atoi()
#include <bsl_iostream.h>
#include <bsl_sstream.h>
#include <bsl_vector.h>

using namespace BloombergLP;
using namespace bsl;  // automatically added by script


//=============================================================================
//                                 TEST PLAN
//-----------------------------------------------------------------------------
//                                  Overview
//                                  --------
// The component under test is a value-semantic scalar whose state is
// identically its value.  There is no allocator involved.  We are mostly
// concerned with the mechanical functioning of the various methods and free
// operators, and exception neutrality during 'bdex' streaming.
//
// The component interface represents a 32-bit CRC checksum as an unsigned
// integer value.  The checksum is calculated in the 'update' function based
// on a sequence of data bytes and the data length.  We need to verify that the
// 'update' member function calculates the checksum correctly and the
// 'checksum' member function returns the correct checksum.
//
//       Primary Constructors, Primary Manipulators, and Basic Accessors
//       ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Primary Constructors:
//   A 'bdlde::Crc32' object is created with a default checksum value of
//   0x00000000.  An object's white-box state is exactly the same as its
//   black-box state (just a single unsigned integer value), which can be
//   modified using the primary manipulator 'update'.  The default constructor,
//   in conjunction with the primary manipulator, is sufficient to attain any
//   achievable white-box state.
//
//    o bdlde::Crc32();
//
// Primary Manipulators:
//   The 'update' method comprises the minimal set of manipulators that can
//   attain any achievable white-box state.
//
//    o void update(const void *data, int length);
//
// Basic Accessors:
//   This is the maximal set of accessors that have direct contact with the
//   black-box representation of the object.  The 'checksum' method is an
//   obvious member of this set.  Although the 'checksumAndReset' method is
//   really a destructive accessor, we also include it in the set because it
//   touches the internal state of the object.
//
//    o unsigned int checksum() const;
//    o unsigned int checksumAndReset();
//
//-----------------------------------------------------------------------------
// CLASS METHODS
// [10] static int maxSupportedBdexVersion(int);
//
// CREATORS
// [ 2] bdlde::Crc32();
// [12] bdlde::Crc32(const void *data, int length);
// [ 7] bdlde::Crc32(const bdlde::Crc32& original);
// [ 2] ~bdlde::Crc32();

// MANIPULATORS
// [ 9] bdlde::Crc32& operator=(const bdlde::Crc32& rhs);
// [10] STREAM& bdexStreamIn(STREAM& stream, int version);
// [ 4] unsigned int checksumAndReset();
// [13] void reset();
// [11] void update(const void *data, int length);
//
// ACCESSORS
// [10] STREAM& bdexStreamOut(STREAM& stream, int version) const;
// [ 4] unsigned int checksum() const;
// [ 5] bsl::ostream& print(bsl::ostream& stream) const;
//
// FREE OPERATORS
// [ 6] bool operator==(const bdlde::Crc32& lhs, const bdlde::Crc32& rhs);
// [ 6] bool operator!=(const bdlde::Crc32& lhs, const bdlde::Crc32& rhs);
// [ 5] bsl::ostream& operator<<(bsl::ostream& stream, const bdlde::Crc32&);
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [15] USAGE EXAMPLE
// [ 2] BOOTSTRAP: void update(const void *data, int length);
// [14] CRC_TABLE TEST
// [-1] PERFORMANCE TEST
//
// [ 3] int ggg(bdlde::Crc32 *object, const char *spec, int vF = 1);
// [ 3] bdlde::Crc32& gg(bdlde::Crc32 *object, const char *spec);
// [ 8] bdlde::Crc32   g(const char *spec);

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

typedef bdlde::Crc32        Obj;
typedef bslx::TestInStream  In;
typedef bslx::TestOutStream Out;

// ============================================================================
//                               USAGE EXAMPLE
// ----------------------------------------------------------------------------

///Usage
///-----
// The following snippets of code illustrate a typical use of the
// 'bdlde::Crc32' class.  Each function would typically execute in separate
// processes or potentially on separate machines.  The 'senderExample' function
// below demonstrates how a message sender can write a message and its CRC-32
// checksum to a 'bdex' output stream.  Note that 'Out' may be a 'typedef' of
// any class that implements the 'bslx::OutStream' protocol:
//..
void senderExample(Out& output)
    // Write a message and its CRC-32 checksum to the specified 'output'
    // stream.
{
    // prepare a message
    bsl::string message = "This is a test message.";

    // generate a checksum for 'message'
    bdlde::Crc32 crc(message.data(), message.length());

    // write the message to 'output'
    output << message;

    // write the checksum to 'output'
    const int VERSION = 1;
    crc.bdexStreamOut(output, VERSION);
}
//..
// The 'receiverExample' function below illustrates how a message receiver can
// read a message and its CRC-32 checksum from a 'bdex' input stream, then
// perform a local CRC-32 computation to verify that the message was received
// intact.  Note that 'In' may be a 'typedef' of any class that implements the
// 'bslx::InStream' protocol:
//..
void receiverExample(In& input)
    // Read a message and its CRC-32 checksum from the specified 'input'
    // stream, and verify the integrity of the message.
{
    // read the message from 'input'
    bsl::string message;
    input >> message;

    // read the checksum from 'input'
    bdlde::Crc32 crc;
    const int VERSION = 1;
    crc.bdexStreamIn(input, VERSION);

    // locally compute the checksum of the received 'message'
    bdlde::Crc32 crcLocal;
    crcLocal.update(message.data(), message.length());

    // verify that the received and locally-computed checksums match
    ASSERT(crcLocal == crc);
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

                        // -----------------------
                        // RFC 1952 IMPLEMENTATION
                        // -----------------------

// The following functions were taken directly from RFC 1952 (with some minor
// cosmetic changes) and are used as an oracle to verify the CRC value computed
// by 'bdlde::Crc32'.

// Table of CRCs of all 8-bit messages.
static unsigned int crc_table[256];

// Flag: has the table been computed?  Initially false.
static int crc_table_computed = 0;

void make_crc_table(void)
    // Make the table for a fast CRC.
{
    unsigned int c;
    int n, k;

    for (n = 0; n < 256; n++) {
        c = (unsigned int) n;
        for (k = 0; k < 8; k++) {
            if (c & 1) {
                c = 0xedb88320 ^ (c >> 1);
            } else {
                c = c >> 1;
            }
        }
        crc_table[n] = c;
    }
    crc_table_computed = 1;
}

unsigned int update_crc(unsigned int  crc,
                        const char   *buf,
                        int           len)
    // Update the specified running 'crc' with the bytes in the specified 'buf'
    // having the specified 'len' and return the updated crc.  The crc should
    // be initialized to 0.  Pre- and post-conditioning (one's complement) is
    // performed within this function, so it should not be done by the caller.
    // Usage example:
    //
    //      unsigned long crc = 0L;
    //
    //      while (read_buffer(buffer, length) != EOF) {
    //          crc = update_crc(crc, buffer, length);
    //      }
    //      if (crc != original_crc) error();
{
    unsigned int c = crc ^ 0xffffffff;
    int n;

    if (!crc_table_computed) {
        make_crc_table();
    }
    for (n = 0; n < len; n++) {
        c = crc_table[(c ^ buf[n]) & 0xff] ^ (c >> 8);
    }
    return c ^ 0xffffffff;
}

unsigned int crc(const char *buf, int len)
    // Return the CRC of the bytes in the specified 'buf' having the specified
    // 'len'.
{
    return update_crc(0, buf, len);
}
                        // --------------------------------
                        // crc32 Implementation from dbutil
                        // --------------------------------

// 'crc32trm' (below) was taken from crc32.c in dbutil and the following
// modifications were made to it:
//..
//  1) 'crctab' was replaced with 'crc_table' (defined above).
//  2) 'u_int32_t' was replaced with 'unsigned int'.
//  3) '~0' was changed to '(unsigned int)~0' to quell a compiler warning.
//..
// 'crc32trm' is used in the PERFORMANCE TEST (case -1).
//
// Note that the formatting style (indentation, etc.) was deliberately
// preserved.

extern "C"
unsigned int crc32trm(const char *buf, int bufsize, char trm)
{
    unsigned int crc=(unsigned int)~0;
        for (; (*buf)!=trm && bufsize--; ++buf)
        {
                crc=(crc>>8) ^ crc_table[(crc^(*buf)) & 0xff];
        }
        return ~crc;
}

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
//
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
        // TESTING CRC_TABLE
        //   This will test the 'CRC_TABLE' array defined in bdlde_crc32.cpp.
        //
        // Concerns:
        //   The CRC algorithm depends on a static constant array of unsigned
        //   integers defined in bdlde_crc32.cpp as 'CRC_TABLE[256]'.  This
        //   array was generated from the initialization step found at the end
        //   of RFC 1952.  We want to ensure that the values in the table are
        //   correct.
        //
        // Plan:
        //   We cannot access the array directly from this test driver because
        //   it is internal to the bdlde_crc32.cpp translation unit.  However,
        //   we do have an array 'crc_table' that is internal to this
        //   translation unit and we can use this array as an oracle.  We can
        //   loop through this array, and for each 'crc_table[i]', perform the
        //   following steps:
        //      a. Create an 'unsigned char' 'j' with the value 255 - 'i'.
        //      b. Create a 'bdlde::Crc32' object 'obj' using the default
        //         constructor.
        //      c. Call 'obj.update(&j, 1)'.
        //      d. Call 'obj.checksum' and store the result in an 'unsigned'
        //         integer 'result'.
        //      e. Flip the eight most significant bits of 'result'.
        //      f. Verify that 'result' is exactly the same as 'crc_table[i]'.
        //
        //   These steps were created based on a study of the implementation of
        //   the 'update' and 'checksum' methods with the assumption that the
        //   'update' and 'checksum' methods are correct.
        //
        // Testing:
        //   static const unsigned int CRC_TABLE[256]; (bdlde_crc32.cpp)
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting CRC_TABLE"
                          << "\n=================" << endl;

        // make sure crc_table has been initialized
        if (!crc_table_computed) {
            make_crc_table();
        }

        for (int i = 0; i < 256; ++i) {
            const unsigned char j = 255 - i;

            Obj obj;
            obj.update(&j, 1);

            unsigned int result = obj.checksum();
            result ^= 0xff000000;
            if (veryVerbose) {
                T_ P_(i);
                cout << hex;
                P_(result);
                P_(crc_table[i]);
                cout << dec << endl;
            }

            LOOP_ASSERT(i, crc_table[i] == result);
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
        //   each datum, create a 'bdlde::Crc32' object using the fully-tested
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
        } DATA[] = {
            //line  source                  length
            //----  ------                  ------
            { L_,   "",                     0       },
            { L_,   "\x00",                 1       },
            { L_,   "\x01\x02",             2       },
            { L_,   "\x01\x02\x03",         3       },
            { L_,   "\x01\x02\x03\x04",     4       },
            { L_,   "\x00\x01\x02\x03\x04", 5       },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int i = 0; i < NUM_DATA; ++i) {
            const int   LINE   = DATA[i].d_lineNum;
            const char *STR    = DATA[i].d_str;
            const int   LENGTH = DATA[i].d_length;

            const unsigned int CRC = crc(STR, LENGTH);
            const Obj          DEFAULT;

            Obj x(STR, LENGTH);  const Obj& X = x;
            LOOP_ASSERT(LINE, CRC == X.checksum());

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
        //   Create a set of test data with varying lengths from 0 to 5.  For
        //   each datum, create a 'bdlde::Crc32' object using the two-argument
        //   init constructor.  Then verify the CRC value stored in the object
        //   is the same as the expected value.
        //
        // Testing:
        //   bdlde::Crc32(const void *data, int length);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Init Constructor"
                          << "\n========================" << endl;

        static const struct {
            int           d_lineNum;  // source line number
            const char   *d_str;      // source string
            int           d_length;   // length of source input
        } DATA[] = {
            //line  source                  length
            //----  ------                  ------
            { L_,   "",                     0      },
            { L_,   "\x00",                 1      },
            { L_,   "\x01\x02",             2      },
            { L_,   "\x01\x02\x03",         3      },
            { L_,   "\x01\x02\x03\x04",     4      },
            { L_,   "\x00\x01\x02\x03\x04", 5      },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int i = 0; i < NUM_DATA; ++i) {
            const int   LINE   = DATA[i].d_lineNum;
            const char *STR    = DATA[i].d_str;
            const int   LENGTH = DATA[i].d_length;

            const unsigned int CRC = crc(STR, LENGTH);

            Obj mX(STR, LENGTH);  const Obj& X = mX;
            if (veryVerbose) {
                T_ P_(X);
                cout << hex; P_(CRC); cout << dec << endl;
            }
            LOOP_ASSERT(LINE, CRC == X.checksum());
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
        //   Firstly, we need to test the 'update' function using data with
        //   lengths varying from 0-10.  The algorithm selects control paths
        //   based on the length of the data, and not the data itself.  The
        //   control path is selected using 'length % 4'.  So, in order to
        //   exercise all control paths, the test vectors must contain data
        //   with lengths varying from 0 to 3.  The test vectors must also
        //   contain data with lengths 4 to 10 to ensure that the loop is
        //   repeated correctly.  The test vectors should contain all of the
        //   possible values for single-byte data (0x00 .. 0xff) to make sure
        //   that the expected result is produced for all single-byte values.
        //   Data with length 0 is used as a boundary check.
        //
        //   Secondly, we need to ensure that there are no CRC collisions for
        //   any possible combination of 1 or 2 byte data.
        //
        //   Finally, we need to ensure that multiple calls to 'update' produce
        //   the same result as calling 'update' once with the original two
        //   strings concatenated together.
        //
        // Plan:
        //   For the first part of the test, create a test vector containing
        //   data with lengths varying from 0 to 10.  Exercise the 'update'
        //   member function for each datum and make sure the object's CRC
        //   value is equal to the expected CRC value.
        //
        //   For the second part of the test, create an integer array using
        //   'bsl::vector<int>'.  Fill the array using the following steps:
        //      a. Loop through all the possible values for an 'unsigned char'
        //         (0 .. 255) and add its CRC value to the array.
        //      b. Then loop through all the possible values for a 16-bit
        //         'unsigned short' (0 .. 65535) and add its CRC value to the
        //         array.
        //      c. Again, loop through all the possible values for a 16-bit
        //         'unsigned short', but insert a 0-valued byte in between the
        //         2 bytes.  Calculate the CRC and add it to the array.
        //      d. Repeat step (c), but insert a 1-valued byte instead of a
        //         0-valued byte.
        //   Sort the array, then assert that each CRC value in the array
        //   (except for the 0th array element) differs from the CRC value
        //   immediately preceding it in the array.
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
            } DATA[] = {
                //line  source                                       length
                //----  ------                                       ------
                { L_,   "",                                          0      },
                { L_,   "\x00",                                      1      },
                { L_,   "\x01",                                      1      },
                { L_,   "\x02",                                      1      },
                { L_,   "\x03",                                      1      },
                { L_,   "\x04",                                      1      },
                { L_,   "\x05",                                      1      },
                { L_,   "\x06",                                      1      },
                { L_,   "\x07",                                      1      },
                { L_,   "\x08",                                      1      },
                { L_,   "\x09",                                      1      },
                { L_,   "\x0a",                                      1      },
                { L_,   "\x0b",                                      1      },
                { L_,   "\x0c",                                      1      },
                { L_,   "\x0d",                                      1      },
                { L_,   "\x0e",                                      1      },
                { L_,   "\x0f",                                      1      },
                { L_,   "\x10",                                      1      },
                { L_,   "\x11",                                      1      },
                { L_,   "\x12",                                      1      },
                { L_,   "\x13",                                      1      },
                { L_,   "\x14",                                      1      },
                { L_,   "\x15",                                      1      },
                { L_,   "\x16",                                      1      },
                { L_,   "\x17",                                      1      },
                { L_,   "\x18",                                      1      },
                { L_,   "\x19",                                      1      },
                { L_,   "\x1a",                                      1      },
                { L_,   "\x1b",                                      1      },
                { L_,   "\x1c",                                      1      },
                { L_,   "\x1d",                                      1      },
                { L_,   "\x1e",                                      1      },
                { L_,   "\x1f",                                      1      },
                { L_,   "\x20",                                      1      },
                { L_,   "\x21",                                      1      },
                { L_,   "\x22",                                      1      },
                { L_,   "\x23",                                      1      },
                { L_,   "\x24",                                      1      },
                { L_,   "\x25",                                      1      },
                { L_,   "\x26",                                      1      },
                { L_,   "\x27",                                      1      },
                { L_,   "\x28",                                      1      },
                { L_,   "\x29",                                      1      },
                { L_,   "\x2a",                                      1      },
                { L_,   "\x2b",                                      1      },
                { L_,   "\x2c",                                      1      },
                { L_,   "\x2d",                                      1      },
                { L_,   "\x2e",                                      1      },
                { L_,   "\x2f",                                      1      },
                { L_,   "\x30",                                      1      },
                { L_,   "\x31",                                      1      },
                { L_,   "\x32",                                      1      },
                { L_,   "\x33",                                      1      },
                { L_,   "\x34",                                      1      },
                { L_,   "\x35",                                      1      },
                { L_,   "\x36",                                      1      },
                { L_,   "\x37",                                      1      },
                { L_,   "\x38",                                      1      },
                { L_,   "\x39",                                      1      },
                { L_,   "\x3a",                                      1      },
                { L_,   "\x3b",                                      1      },
                { L_,   "\x3c",                                      1      },
                { L_,   "\x3d",                                      1      },
                { L_,   "\x3e",                                      1      },
                { L_,   "\x3f",                                      1      },
                { L_,   "\x40",                                      1      },
                { L_,   "\x41",                                      1      },
                { L_,   "\x42",                                      1      },
                { L_,   "\x43",                                      1      },
                { L_,   "\x44",                                      1      },
                { L_,   "\x45",                                      1      },
                { L_,   "\x46",                                      1      },
                { L_,   "\x47",                                      1      },
                { L_,   "\x48",                                      1      },
                { L_,   "\x49",                                      1      },
                { L_,   "\x4a",                                      1      },
                { L_,   "\x4b",                                      1      },
                { L_,   "\x4c",                                      1      },
                { L_,   "\x4d",                                      1      },
                { L_,   "\x4e",                                      1      },
                { L_,   "\x4f",                                      1      },
                { L_,   "\x50",                                      1      },
                { L_,   "\x51",                                      1      },
                { L_,   "\x52",                                      1      },
                { L_,   "\x53",                                      1      },
                { L_,   "\x54",                                      1      },
                { L_,   "\x55",                                      1      },
                { L_,   "\x56",                                      1      },
                { L_,   "\x57",                                      1      },
                { L_,   "\x58",                                      1      },
                { L_,   "\x59",                                      1      },
                { L_,   "\x5a",                                      1      },
                { L_,   "\x5b",                                      1      },
                { L_,   "\x5c",                                      1      },
                { L_,   "\x5d",                                      1      },
                { L_,   "\x5e",                                      1      },
                { L_,   "\x5f",                                      1      },
                { L_,   "\x60",                                      1      },
                { L_,   "\x61",                                      1      },
                { L_,   "\x62",                                      1      },
                { L_,   "\x63",                                      1      },
                { L_,   "\x64",                                      1      },
                { L_,   "\x65",                                      1      },
                { L_,   "\x66",                                      1      },
                { L_,   "\x67",                                      1      },
                { L_,   "\x68",                                      1      },
                { L_,   "\x69",                                      1      },
                { L_,   "\x6a",                                      1      },
                { L_,   "\x6b",                                      1      },
                { L_,   "\x6c",                                      1      },
                { L_,   "\x6d",                                      1      },
                { L_,   "\x6e",                                      1      },
                { L_,   "\x6f",                                      1      },
                { L_,   "\x70",                                      1      },
                { L_,   "\x71",                                      1      },
                { L_,   "\x72",                                      1      },
                { L_,   "\x73",                                      1      },
                { L_,   "\x74",                                      1      },
                { L_,   "\x75",                                      1      },
                { L_,   "\x76",                                      1      },
                { L_,   "\x77",                                      1      },
                { L_,   "\x78",                                      1      },
                { L_,   "\x79",                                      1      },
                { L_,   "\x7a",                                      1      },
                { L_,   "\x7b",                                      1      },
                { L_,   "\x7c",                                      1      },
                { L_,   "\x7d",                                      1      },
                { L_,   "\x7e",                                      1      },
                { L_,   "\x7f",                                      1      },
                { L_,   "\x80",                                      1      },
                { L_,   "\x81",                                      1      },
                { L_,   "\x82",                                      1      },
                { L_,   "\x83",                                      1      },
                { L_,   "\x84",                                      1      },
                { L_,   "\x85",                                      1      },
                { L_,   "\x86",                                      1      },
                { L_,   "\x87",                                      1      },
                { L_,   "\x88",                                      1      },
                { L_,   "\x89",                                      1      },
                { L_,   "\x8a",                                      1      },
                { L_,   "\x8b",                                      1      },
                { L_,   "\x8c",                                      1      },
                { L_,   "\x8d",                                      1      },
                { L_,   "\x8e",                                      1      },
                { L_,   "\x8f",                                      1      },
                { L_,   "\x90",                                      1      },
                { L_,   "\x91",                                      1      },
                { L_,   "\x92",                                      1      },
                { L_,   "\x93",                                      1      },
                { L_,   "\x94",                                      1      },
                { L_,   "\x95",                                      1      },
                { L_,   "\x96",                                      1      },
                { L_,   "\x97",                                      1      },
                { L_,   "\x98",                                      1      },
                { L_,   "\x99",                                      1      },
                { L_,   "\x9a",                                      1      },
                { L_,   "\x9b",                                      1      },
                { L_,   "\x9c",                                      1      },
                { L_,   "\x9d",                                      1      },
                { L_,   "\x9e",                                      1      },
                { L_,   "\x9f",                                      1      },
                { L_,   "\xa0",                                      1      },
                { L_,   "\xa1",                                      1      },
                { L_,   "\xa2",                                      1      },
                { L_,   "\xa3",                                      1      },
                { L_,   "\xa4",                                      1      },
                { L_,   "\xa5",                                      1      },
                { L_,   "\xa6",                                      1      },
                { L_,   "\xa7",                                      1      },
                { L_,   "\xa8",                                      1      },
                { L_,   "\xa9",                                      1      },
                { L_,   "\xaa",                                      1      },
                { L_,   "\xab",                                      1      },
                { L_,   "\xac",                                      1      },
                { L_,   "\xad",                                      1      },
                { L_,   "\xae",                                      1      },
                { L_,   "\xaf",                                      1      },
                { L_,   "\xb0",                                      1      },
                { L_,   "\xb1",                                      1      },
                { L_,   "\xb2",                                      1      },
                { L_,   "\xb3",                                      1      },
                { L_,   "\xb4",                                      1      },
                { L_,   "\xb5",                                      1      },
                { L_,   "\xb6",                                      1      },
                { L_,   "\xb7",                                      1      },
                { L_,   "\xb8",                                      1      },
                { L_,   "\xb9",                                      1      },
                { L_,   "\xba",                                      1      },
                { L_,   "\xbb",                                      1      },
                { L_,   "\xbc",                                      1      },
                { L_,   "\xbd",                                      1      },
                { L_,   "\xbe",                                      1      },
                { L_,   "\xbf",                                      1      },
                { L_,   "\xc0",                                      1      },
                { L_,   "\xc1",                                      1      },
                { L_,   "\xc2",                                      1      },
                { L_,   "\xc3",                                      1      },
                { L_,   "\xc4",                                      1      },
                { L_,   "\xc5",                                      1      },
                { L_,   "\xc6",                                      1      },
                { L_,   "\xc7",                                      1      },
                { L_,   "\xc8",                                      1      },
                { L_,   "\xc9",                                      1      },
                { L_,   "\xca",                                      1      },
                { L_,   "\xcb",                                      1      },
                { L_,   "\xcc",                                      1      },
                { L_,   "\xcd",                                      1      },
                { L_,   "\xce",                                      1      },
                { L_,   "\xcf",                                      1      },
                { L_,   "\xd0",                                      1      },
                { L_,   "\xd1",                                      1      },
                { L_,   "\xd2",                                      1      },
                { L_,   "\xd3",                                      1      },
                { L_,   "\xd4",                                      1      },
                { L_,   "\xd5",                                      1      },
                { L_,   "\xd6",                                      1      },
                { L_,   "\xd7",                                      1      },
                { L_,   "\xd8",                                      1      },
                { L_,   "\xd9",                                      1      },
                { L_,   "\xda",                                      1      },
                { L_,   "\xdb",                                      1      },
                { L_,   "\xdc",                                      1      },
                { L_,   "\xdd",                                      1      },
                { L_,   "\xde",                                      1      },
                { L_,   "\xdf",                                      1      },
                { L_,   "\xe0",                                      1      },
                { L_,   "\xe1",                                      1      },
                { L_,   "\xe2",                                      1      },
                { L_,   "\xe3",                                      1      },
                { L_,   "\xe4",                                      1      },
                { L_,   "\xe5",                                      1      },
                { L_,   "\xe6",                                      1      },
                { L_,   "\xe7",                                      1      },
                { L_,   "\xe8",                                      1      },
                { L_,   "\xe9",                                      1      },
                { L_,   "\xea",                                      1      },
                { L_,   "\xeb",                                      1      },
                { L_,   "\xec",                                      1      },
                { L_,   "\xed",                                      1      },
                { L_,   "\xee",                                      1      },
                { L_,   "\xef",                                      1      },
                { L_,   "\xf0",                                      1      },
                { L_,   "\xf1",                                      1      },
                { L_,   "\xf2",                                      1      },
                { L_,   "\xf3",                                      1      },
                { L_,   "\xf4",                                      1      },
                { L_,   "\xf5",                                      1      },
                { L_,   "\xf6",                                      1      },
                { L_,   "\xf7",                                      1      },
                { L_,   "\xf8",                                      1      },
                { L_,   "\xf9",                                      1      },
                { L_,   "\xfa",                                      1      },
                { L_,   "\xfb",                                      1      },
                { L_,   "\xfc",                                      1      },
                { L_,   "\xfd",                                      1      },
                { L_,   "\xfe",                                      1      },
                { L_,   "\xff",                                      1      },
                { L_,   "\x00\x01",                                  2      },
                { L_,   "\x01\x02",                                  2      },
                { L_,   "\x02\x03",                                  2      },
                { L_,   "\x00\x01\x02",                              3      },
                { L_,   "\x01\x02\x03",                              3      },
                { L_,   "\x02\x03\x04",                              3      },
                { L_,   "\x00\x01\x02\x03",                          4      },
                { L_,   "\x01\x02\x03\x04",                          4      },
                { L_,   "\x02\x03\x04\x05",                          4      },
                { L_,   "\x00\x01\x02\x03\x04",                      5      },
                { L_,   "\x01\x02\x03\x04\x05",                      5      },
                { L_,   "\x02\x03\x04\x05\x06",                      5      },
                { L_,   "\x00\x01\x02\x03\x04\x05",                  6      },
                { L_,   "\x01\x02\x03\x04\x05\x06",                  6      },
                { L_,   "\x02\x03\x04\x05\x06\x07",                  6      },
                { L_,   "\x00\x01\x02\x03\x04\x05\x06",              7      },
                { L_,   "\x01\x02\x03\x04\x05\x06\x07",              7      },
                { L_,   "\x02\x03\x04\x05\x06\x07\x08",              7      },
                { L_,   "\x00\x01\x02\x03\x04\x05\x06\x07",          8      },
                { L_,   "\x01\x02\x03\x04\x05\x06\x07\x08",          8      },
                { L_,   "\x02\x03\x04\x05\x06\x07\x08\x09",          8      },
                { L_,   "\x00\x01\x02\x03\x04\x05\x06\x07\x08",      9      },
                { L_,   "\x01\x02\x03\x04\x05\x06\x07\x08\x09",      9      },
                { L_,   "\x02\x03\x04\x05\x06\x07\x08\x09\x0a",      9      },
                { L_,   "\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09", 10      },
                { L_,   "\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0a", 10      },
                { L_,   "\x02\x03\x04\x05\x06\x07\x08\x09\x0a\x0b", 10      },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE = DATA[i].d_lineNum;
                const char *STR  = DATA[i].d_str;
                const int   LEN  = DATA[i].d_length;

                const unsigned int CRC = crc(STR, LEN);

                Obj mX;  const Obj& X = mX;
                mX.update(STR, LEN);
                if (veryVerbose) {
                    T_ P_(LEN);
                    cout << hex; P_(CRC); cout << dec;
                    P(X);
                }
                LOOP_ASSERT(LINE, CRC == X.checksum());
            }
        }

        if (verbose) cout << "\n2. Collision test." << endl;
        {
            bsl::vector<int> a;
            Obj obj;

            // populate the list of results

            // one byte
            for (int i = 0; i < 256; ++i) {
                unsigned char data = (unsigned char)i;
                obj.update(&data, 1);
                a.push_back(obj.checksum());
                obj.reset();
            }

            // two bytes
            for (int i = 0; i < 65536; ++i) {
                unsigned char data[2];
                data[0] = (unsigned char)(i / 256);
                data[1] = (unsigned char)(i % 256);
                obj.update(data, 2);
                a.push_back(obj.checksum());
                obj.reset();
            }

            // two bytes with a zero separator
            for (int i = 0; i < 65536; ++i) {
                unsigned char data[3];
                data[0] = (unsigned char)(i / 256);
                data[1] = 0;
                data[2] = (unsigned char)(i % 256);
                obj.update(data, 3);
                a.push_back(obj.checksum());
                obj.reset();
            }

            // two bytes with a one separator
            for (int i = 0; i < 65536; ++i) {
                unsigned char data[3];
                data[0] = (unsigned char)(i / 256);
                data[1] = 1;
                data[2] = (unsigned char)(i % 256);
                obj.update(data, 3);
                a.push_back(obj.checksum());
                obj.reset();
            }

            bsl::sort(a.begin(), a.end());
            for (unsigned int i = 1; i < a.size(); ++i) {
                ASSERT(a[i] != a[i - 1]);
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
                bsl::memcpy(msgAccum,      MSG1, LEN1);
                bsl::memcpy(msgAccum+LEN1, MSG2, LEN2);

                // initialize the second object
                objAccum.update(msgAccum, LEN1+LEN2);

                if (veryVerbose) {
                    T_ PH_(MSG1); P(LEN1);
                    T_ PH_(MSG2); P(LEN2);
                    T_ P_(obj);   P(objAccum);
                }

                LOOP_ASSERT(LINE,
                            objAccum.checksum() == crc(msgAccum, LEN1+LEN2));
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
        //   functionality using the bdex stream functions, which forward
        //   appropriate calls to the member functions.  We also want to step
        //   through the sequence of possible stream states (valid, empty,
        //   invalid, incomplete, and corrupted), appropriately selecting data
        //   sets as described below.  In all cases, we need to confirm
        //   exception neutrality using the specially instrumented
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
        //     'corrupted' data (all CRCs are valid), so just check for bad
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

        const int VERSION = Obj::maxSupportedBdexVersion(20150813);
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
            // testing '<<' and '>>' operators thoroughly
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

                    // in.setSuppressVersionCheck(1); -- no longer supported

                    LOOP2_ASSERT(i, j, in);
                    LOOP2_ASSERT(i, j, !in.isEmpty());

                    Obj t(VALUES[j]);
                    BSLX_TESTINSTREAM_EXCEPTION_TEST_BEGIN(in) {
                      in.reset();
                      LOOP2_ASSERT(i, j, X == t == (i == j));
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
            const char *const OD = out.data();

            for (int i = 0; i < LOD; ++i) {
                In in(OD, i);  In& testInStream = in;

                // in.setSuppressVersionCheck(1); -- no longer supperted

                BSLX_TESTINSTREAM_EXCEPTION_TEST_BEGIN(in) {
                  in.reset();
                  LOOP_ASSERT(i, in);
                  LOOP_ASSERT(i, !i == in.isEmpty());
                  Obj t1(W1), t2(W2), t3(W3);

                  if (i < LOD1) {
                      t1.bdexStreamIn(in, VERSION);
                      LOOP_ASSERT(i, !in);
                      if (0 == i) LOOP_ASSERT(i, W1 == t1);
                      t2.bdexStreamIn(in, VERSION);
                      LOOP_ASSERT(i, !in);  LOOP_ASSERT(i, W2 == t2);
                      t3.bdexStreamIn(in, VERSION);
                      LOOP_ASSERT(i, !in);  LOOP_ASSERT(i, W3 == t3);
                  }
                  else if (i < LOD2) {
                      t1.bdexStreamIn(in, VERSION);
                      LOOP_ASSERT(i,  in);  LOOP_ASSERT(i, X1 == t1);
                      t2.bdexStreamIn(in, VERSION);
                      LOOP_ASSERT(i, !in);
                      if (LOD1 == i) LOOP_ASSERT(i, W2 == t2);
                      t3.bdexStreamIn(in, VERSION);
                      LOOP_ASSERT(i, !in);  LOOP_ASSERT(i, W3 == t3);
                  }
                  else {
                      t1.bdexStreamIn(in, VERSION);
                      LOOP_ASSERT(i,  in);  LOOP_ASSERT(i, X1 == t1);
                      t2.bdexStreamIn(in, VERSION);
                      LOOP_ASSERT(i,  in);
                      t3.bdexStreamIn(in, VERSION);
                      LOOP_ASSERT(i, !in);  LOOP_ASSERT(i, W3 == t3);
                      if (LOD2 == i) LOOP_ASSERT(i, W3 == t3);
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
        // (all CRCs are valid), so just check for bad version numbers.

        if (verbose) cout << "\tOn corrupted data." << endl;

        const Obj W;            // default value
        const Obj X(g("/01"));  // original (control) value
        const Obj Y(g("/02"));  // new (streamed-out) value

        // internal representation of Obj Y
        const unsigned int SERIAL_Y = crc("\x02", 1) ^ 0xffffffff;

        if (verbose) cout << "\t\tGood stream (for control)." << endl;
        {
            Out out(20150813);
            out.putVersion(VERSION);
            out.putUint32(SERIAL_Y);  // stream out "new" value

            const char *const OD  = out.data();
            const int         LOD = out.length();

            Obj t(X);       ASSERT(W != t); ASSERT(X == t); ASSERT(Y != t);
            In in(OD, LOD); ASSERT(in);
            int version;
            in.getVersion(version);
            ASSERT(version == VERSION);
            t.bdexStreamIn(in, version);
            ASSERT(in);
            ASSERT(W != t); ASSERT(X != t); ASSERT(Y == t);
        }

        if (verbose) cout << "\t\tBad version." << endl;
        {
            const int version = 0;
            // too small ('version' must be >= 1)

            Out out(20150813);
            out.putUint32(SERIAL_Y);  // stream out "new" value

            const char *const OD  = out.data();
            const int         LOD = out.length();

            Obj t(X);       ASSERT(W != t); ASSERT(X == t); ASSERT(Y != t);
            In in(OD, LOD); ASSERT(in);
            in.setQuiet(!veryVerbose);
            t.bdexStreamIn(in, version);
            ASSERT(!in);
            ASSERT(W != t); ASSERT(X == t); ASSERT(Y != t);
        }
        {
            const char version = 5;  // too large (current version is 1)

            Out out(20150813);
            out.putUint32(SERIAL_Y);    // stream out "new" value

            const char *const OD  = out.data();
            const int         LOD = out.length();

            Obj t(X);       ASSERT(W != t); ASSERT(X == t); ASSERT(Y != t);
            In in(OD, LOD); ASSERT(in);
            in.setQuiet(!veryVerbose);
            t.bdexStreamIn(in, version);
            ASSERT(!in);
            ASSERT(W != t); ASSERT(X == t); ASSERT(Y != t);
        }

        if (verbose) cout << "\nTesting 'maxSupportedBdexVersion()'." << endl;
        {
            // test 'maxSupportedBdexVersion'
            if (verbose) cout << "\tusing object syntax:" << endl;
            const Obj X;
            ASSERT(1 == X.maxSupportedBdexVersion(20150813));
            if (verbose) cout << "\tusing class method syntax:" << endl;
            ASSERT(1 == Obj::maxSupportedBdexVersion(20150813));
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
        //   bdlde::Crc32& operator=(const bdlde::Crc32& rhs);
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

                Obj mV;  const Obj& V = mV;
                gg(&mV, V_SPEC);

                for (int j = 0; j < NUM_DATA; ++j) {
                    const int   U_LINE = DATA[j].d_lineNum;
                    const char *U_SPEC = DATA[j].d_spec;
                    if (veryVeryVerbose) {
                        T_ T_ PH(U_SPEC);
                    }

                    Obj mU;  const Obj& U = mU;
                    gg(&mU, U_SPEC);

                    if (veryVeryVerbose) { T_ T_ P_(V); P_(U); }

                    Obj mW(V);  const Obj& W = mW;  // control
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

                Obj mU;  const Obj& U = mU;
                gg(&mU, SPEC);

                Obj mW(U);  const Obj& W = mW;  // control
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
        //   bdlde::Crc32 g(const char *spec);
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
        //   bdlde::Crc32(const bdlde::Crc32& original);
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

                Obj mY(X);  const Obj& Y = mY;

                if (veryVerbose) {
                    T_ P_(W); P_(X); P_(Y); PH(SPEC);
                }
                LOOP_ASSERT(LINE, X == W);     LOOP_ASSERT(LINE, Y == W);
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
        //   We want to make sure that 'operator==' returns true for objects
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
        //   bool operator==(const bdlde::Crc32& lhs, const bdlde::Crc32& rhs);
        //   bool operator!=(const bdlde::Crc32& lhs, const bdlde::Crc32& rhs);
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
        //   This test is broken into two parts:
        //:    1 Testing of 'print'
        //:    2 Testing of 'operator<<'
        //
        //   Each test vector in DATA contains STR, its LEN, the expected CRC
        //   value and also an expected output FMT.  For each datum, construct
        //   an independent object 'mX' and call 'update' with STR and LEN.
        //   Assert that the object contains the expected CRC value.  Create an
        //   'ostringstream' object and use the 'print' function to stream
        //   'mX'.  Compare the contents of the stream object with the expected
        //   FMT value.
        //
        //   To test the 'print' operator, for each datum, construct an
        //   independent object 'mX' and call 'update' with STR and LEN.
        //   Assert that the object contains the expected CRC value.  Create an
        //   'ostringstream' object and use the 'print' function to stream
        //   'mX'.  Compare the contents of the stream object with the expected
        //   FMT value.
        //
        //   To test the '<<' operator, construct an independent object 'obj'
        //   for each test vector in DATA and then call 'update' with STR and
        //   LEN.  Assert that the object contains the expected CRC value.
        //   Create an 'ostringstream' object and use the '<<' operator to
        //   stream 'obj'.  Finally, compare the contents of the stream object
        //   with the expected FMT value.
        //
        // Testing:
        //   bsl::ostream& print(bsl::ostream& stream) const;
        //   bsl::ostream& operator<<(bsl::ostream&, const bdlde::Crc32&);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Ouput (<<) Operator"
                          << "\n===========================" << endl;

        static const struct {
            int           d_lineNum;  // source line number
            const char   *d_str;      // source string
            int           d_length;   // length of source input
            unsigned int  d_crc;      // expected CRC-32 value
            const char   *d_fmt;      // expected output format
        } DATA[] = {
            //line  source                  length crc value   output format
            //----  ------                  ------ ---------   -------------
            { L_,   "",                     0,     0x00000000, "0x00000000"  },
            { L_,   "\x00",                 1,     0xd202ef8d, "0xd202ef8d"  },
            { L_,   "\x01\x02",             2,     0xb6cc4292, "0xb6cc4292"  },
            { L_,   "\x01\x02\x03",         3,     0x55bc801d, "0x55bc801d"  },
            { L_,   "\x01\x02\x03\x04",     4,     0xb63cfbcd, "0xb63cfbcd"  },
            { L_,   "\x00\x01\x02\x03\x04", 5,     0x515ad3cc, "0x515ad3cc"  },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;
        const int SIZE     = 128;
        int i;

        if (verbose) cout << "\n2. Testing 'print'." << endl;
        for (i = 0; i < NUM_DATA; ++i) {
            const int           LINE = DATA[i].d_lineNum;
            const char         *STR  = DATA[i].d_str;
            const int           LEN  = DATA[i].d_length;
            const unsigned int  CRC  = DATA[i].d_crc;
            const char         *FMT  = DATA[i].d_fmt;

            Obj mX;  const Obj& X = mX;
            mX.update(STR, LEN);
            LOOP_ASSERT(LINE, CRC == X.checksum());
            LOOP_ASSERT(LINE, CRC == crc(STR, LEN));

            char buf[SIZE];
            memset(buf, 0, sizeof(buf));
            ostringstream outbuf(bsl::string(buf, SIZE));
            X.print(outbuf);
            if (veryVerbose) { T_ P_(X); P_(FMT); P(outbuf.str()); }
            LOOP_ASSERT(LINE, 0 == strcmp(outbuf.str().c_str(), FMT));
        }

        if (verbose) cout << "\n3. Testing 'operator<<'." << endl;
        for (i = 0; i < NUM_DATA; ++i) {
            const int           LINE = DATA[i].d_lineNum;
            const char         *STR  = DATA[i].d_str;
            const int           LEN  = DATA[i].d_length;
            const unsigned int  CRC  = DATA[i].d_crc;
            const char         *FMT  = DATA[i].d_fmt;

            Obj mX;  const Obj& X = mX;
            mX.update(STR, LEN);
            LOOP_ASSERT(LINE, CRC == X.checksum());
            LOOP_ASSERT(LINE, CRC == crc(STR, LEN));

            char buf[SIZE];
            memset(buf, 0, sizeof(buf));
            ostringstream outbuf(bsl::string(buf, SIZE));
            outbuf << X;
            if (veryVerbose) { T_ P_(X); P_(FMT); P(outbuf.str()); }
            LOOP_ASSERT(LINE, 0 == strcmp(outbuf.str().c_str(), FMT));
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
        //   and LENGTH.  Ensure that the CRC value returned by 'checksum' is
        //   the same as the expected CRC value.
        //
        //   Then, for each test vector in DATA, construct an object 'mX' using
        //   the default constructor.  Then call 'mX.update' using the current
        //   STR and LENGTH.  Ensure that the CRC value returned by
        //   'checksumAndReset' is the same as the expected CRC value.  Then
        //   verify that a subsequent value returned by 'checksum' will be the
        //   same as a default object.
        //
        // Testing:
        //   unsigned int checksumAndReset();
        //   unsigned int checksum() const;
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Basic Accessors"
                          << "\n=======================" << endl;

        static const struct {
            int         d_lineNum;  // source line number
            const char *d_str;      // source string
            int         d_length;   // length of source input
        } DATA[] = {
            //line  source                  length
            //----  ------                  ------
            { L_,   "",                     0       },
            { L_,   "\x00",                 1       },
            { L_,   "\x01",                 1       },
            { L_,   "\x02",                 1       },
            { L_,   "\x03",                 1       },
            { L_,   "\x04",                 1       },
            { L_,   "\x05",                 1       },
            { L_,   "\x06",                 1       },
            { L_,   "\x07",                 1       },
            { L_,   "\x08",                 1       },
            { L_,   "\x01\x02",             2       },
            { L_,   "\x01\x02\x03",         3       },
            { L_,   "\x01\x02\x03\x04",     4       },
            { L_,   "\x00\x01\x02\x03\x04", 5       },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        if (verbose) cout << "\nTesting 'checksum'." << endl;
        {
            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE   = DATA[i].d_lineNum;
                const char *STR    = DATA[i].d_str;
                const int   LENGTH = DATA[i].d_length;

                const unsigned int CRC = crc(STR, LENGTH);

                Obj mX;  const Obj& X = mX;
                mX.update(STR, LENGTH);

                if (veryVerbose) {
                    T_ P_(LENGTH);
                    cout << hex; P_(CRC); cout << dec;
                    P(X);
                }
                LOOP_ASSERT(LINE, CRC == X.checksum());
            }
        }

        if (verbose) cout << "\nTesting 'checksumAndReset'." << endl;
        {
            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE   = DATA[i].d_lineNum;
                const char *STR    = DATA[i].d_str;
                const int   LENGTH = DATA[i].d_length;

                const unsigned int CRC = crc(STR, LENGTH);
                const Obj          DEFAULT;

                Obj mX;  const Obj& X = mX;
                mX.update(STR, LENGTH);

                unsigned int result = mX.checksumAndReset();
                if (veryVerbose) {
                    T_ P_(X);
                    cout << hex;
                    P_(result); P_(CRC);
                    cout << dec << endl;
                }

                LOOP_ASSERT(LINE, DEFAULT.checksum() == X.checksum());
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
        //   int ggg(bdlde::Crc32 *object, const char *spec, int vF = 1);
        //   bdlde::Crc32& gg(bdlde::Crc32 *object, const char *spec);
        // --------------------------------------------------------------------

        if (verbose)
            cout << endl << "Testing Primitive Generator Function 'gg'"
                 << endl << "========================================="
                 << endl;

        {
            if (verbose) cout << "\nTesting generator on valid specs." << endl;

            static const struct {
                int          d_lineNum;      // source line number
                const char  *d_spec;         // specification string
                const char  *d_interpreted;  // interpreted string
                int          d_length;       // length of 'd_interpreted'
            } DATA[] = {
                //line  spec             interpreted      length
                //----  ----             -----------      ------
                { L_,   "",              "",               0     },  // len 0
                { L_,   "a",             "a",              1     },  // len 1
                { L_,   "//",            "/",              1     },  // len 2
                { L_,   "ab",            "ab",             2     },  // len 2
                { L_,   "/00",           "\x00",           1     },  // len 3
                { L_,   "//c",           "/c",             2     },  // len 3
                { L_,   "abc",           "abc",            3     },  // len 3
                { L_,   "////",          "//",             2     },  // len 4
                { L_,   "//ab",          "/ab",            3     },  // len 4
                { L_,   "abcd",          "abcd",           4     },  // len 4
                { L_,   "/20//",         " /",             2     },  // len 5
                { L_,   "/20ab",         " ab",            3     },  // len 5
                { L_,   "//abc",         "/abc",           4     },  // len 5
                { L_,   "abcde",         "abcde",          5     },  // len 5
                { L_,   "/41/42",        "AB",             2     },  // len 6
                { L_,   "/41//B",        "A/B",            3     },  // len 6
                { L_,   "//A//B",        "/A/B",           4     },  // len 6
                { L_,   "A//BCD",        "A/BCD",          5     },  // len 6
                { L_,   "ABCDEF",        "ABCDEF",         6     },  // len 6
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE        = DATA[i].d_lineNum;
                const char *SPEC        = DATA[i].d_spec;
                const char *INTERPRETED = DATA[i].d_interpreted;
                const int   LENGTH      = DATA[i].d_length;

                const unsigned int CRC = crc(INTERPRETED, LENGTH);

                Obj mX;
                const Obj& X = gg(&mX, SPEC);  // original spec

                if (veryVerbose) {
                    T_ PH(SPEC);
                }
                if (veryVeryVerbose) {
                    T_ T_ P(X);
                }

                LOOP_ASSERT(LINE, CRC == X.checksum());
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
        //   When an arbitrary string is passed to the object, the correct CRC
        //   value should be returned.  Also, the default constructor should
        //   create an object with a value of 0x00000000.  We also need to make
        //   sure that the destructor works.
        //
        // Plan:
        //   First, verify the default constructor by testing the value of the
        //   resulting object.
        //
        //   Next, verify that the 'update' member function works by
        //   constructing a series of independent objects using the default
        //   constructor and running 'update' using increasing string lengths.
        //   Verify the CRC value in the object using the basic accessor.
        //
        //   Note that the destructor is exercised on each configuration as the
        //   object being tested leaves scope.
        //
        // Testing:
        //   bdlde::Crc32();
        //   ~bdlde::Crc32();
        //   BOOTSTRAP: void update(const void *data, int length);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Primary Manipulators"
                          << "\n============================" << endl;

        if (verbose) cout << "\nTesting default constructor." << endl;
        {
            const unsigned int defaultCrc = 0x00000000;
            Obj mX;  const Obj& X = mX;
            if (veryVerbose) { T_ P(X); }
            ASSERT(defaultCrc == X.checksum());
        }

        if (verbose) cout << "\nTesting 'update'." << endl;
        {
            if (veryVerbose) cout << "\tUsing string with length 0." << endl;

            const char         *DATA = "";
            const unsigned int  CRC  = crc(DATA, 0);
            Obj mX;  const Obj& X = mX;
            mX.update(DATA, 0);
            if (veryVeryVerbose) { T_ T_ P(X); }
            ASSERT(CRC == X.checksum());
        }

        {
            if (veryVerbose) cout << "\tUsing string with length 1." << endl;

            const char         *DATA = "\x0d";
            const unsigned int  CRC  = crc(DATA, 1);
            Obj mX;  const Obj& X = mX;
            mX.update(DATA, 1);
            if (veryVeryVerbose) { T_ T_ P(X); }
            ASSERT(CRC == X.checksum());
        }

        {
            if (veryVerbose) cout << "\tUsing string with length 2." << endl;

            const char         *DATA = "\x01\x02";
            const unsigned int  CRC  = crc(DATA, 2);
            Obj mX;  const Obj& X = mX;
            mX.update(DATA, 2);
            if (veryVeryVerbose) { T_ T_ P(X); }
            ASSERT(CRC == X.checksum());
        }

        {
            if (veryVerbose) cout << "\tUsing string with length 3." << endl;

            const char         *DATA = "\x01\x02\x03";
            const unsigned int  CRC  = crc(DATA, 3);
            Obj mX;  const Obj& X = mX;
            mX.update(DATA, 3);
            if (veryVeryVerbose) { T_ T_ P(X); }
            ASSERT(CRC == X.checksum());
        }

        {
            if (veryVerbose) cout << "\tUsing string with length 4." << endl;

            const char         *DATA = "\x01\x02\x03\x04";
            const unsigned int  CRC  = crc(DATA, 4);
            Obj mX;  const Obj& X = mX;
            mX.update(DATA, 4);
            if (veryVeryVerbose) { T_ T_ P(X); }
            ASSERT(CRC == X.checksum());
        }

        {
            if (veryVerbose) cout << "\tUsing string with length 5." << endl;

            const char         *DATA = "\x00\x01\x02\x03\x04";
            const unsigned int  CRC  = crc(DATA, 5);
            Obj mX;  const Obj& X = mX;
            mX.update(DATA, 5);
            if (veryVeryVerbose) { T_ T_ P(X); }
            ASSERT(CRC == X.checksum());
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
        //     - basic accessors: 'checksum'.
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
        //    1. Create an object x1 (init. to VA)  { x1:VA                  }
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
        } DATA[] = {
            //line  source                   length
            //----  ------                   ------
            { L_,   "",                      0        },
            { L_,   "\x00\x01\x02\x03\x04",  5        },
            { L_,   "\x01\x02\x03\x04\x05",  5        },
            { L_,   "\x02\x03\x04\x05\x06",  5        },
        };

        const char         *SA = DATA[1].d_str,
                           *SB = DATA[2].d_str,
                           *SC = DATA[3].d_str,
                           *SU = DATA[0].d_str;
        const int           LA = DATA[1].d_length,
                            LB = DATA[2].d_length,
                            LC = DATA[3].d_length,
                            LU = DATA[0].d_length;
        const unsigned int  CA = crc(SA, LA),
                            CB = crc(SB, LB),
                            CC = crc(SC, LC),
                            CU = crc(SU, LU);

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 1. Create an object x1 (init. to VA)."
                             "\t\t{ x1:VA }" << endl;
        Obj mX1(SA, LA);  const Obj& X1 = mX1;
        if (verbose) { cout << '\t'; P(X1); }

        if (verbose) cout << "\ta. Check initial state of x1." << endl;
        ASSERT(CA == X1.checksum());

        if (verbose) cout << "\tb. Try equality operators: x1 <op> x1."
                          << endl;
        ASSERT(1 == (X1 == X1));        ASSERT(0 == (X1 != X1));


        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 2. Create an object x2 (copy of x1)."
                             "\t\t{ x1:VA x2:VA }" << endl;
        Obj mX2(X1);  const Obj& X2 = mX2;
        if (verbose) { cout << '\t'; P(X2); }

        if (verbose) cout << "\ta. Check initial state of x2." << endl;
        ASSERT(CA == X2.checksum());

        if (verbose) cout << "\tb. Try equality operators: "
                             "x2 <op> x1, x2." << endl;
        ASSERT(1 == (X2 == X1));        ASSERT(0 == (X2 != X1));
        ASSERT(1 == (X2 == X2));        ASSERT(0 == (X2 != X2));


        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 3. Create an object x3 (default ctor)."
                             "\t\t{ x1:VA x2:VA x3:U }" << endl;
        Obj mX3;  const Obj& X3 = mX3;
        if (verbose) { cout << '\t'; P(X3); }

        if (verbose) cout << "\ta. Check initial state of x3." << endl;
        ASSERT(CU == X3.checksum());

        if (verbose) cout << "\tb. Try equality operators: "
                             "x3 <op> x1, x2, x3." << endl;
        ASSERT(0 == (X3 == X1));        ASSERT(1 == (X3 != X1));
        ASSERT(0 == (X3 == X2));        ASSERT(1 == (X3 != X2));
        ASSERT(1 == (X3 == X3));        ASSERT(0 == (X3 != X3));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 4. Create an object x4 (copy of x3)."
                             "\t\t{ x1:VA x2:VA x3:U x4:U }" << endl;
        Obj mX4;  const Obj& X4 = mX4;
        if (verbose) { cout << '\t'; P(X4); }

        if (verbose) cout << "\ta. Check initial state of x4." << endl;
        ASSERT(CU == X4.checksum());

        if (verbose) cout << "\tb. Try equality operators: "
                             "x4 <op> x1, x2, x3, x4." << endl;
        ASSERT(0 == (X4 == X1));        ASSERT(1 == (X4 != X1));
        ASSERT(0 == (X4 == X2));        ASSERT(1 == (X4 != X2));
        ASSERT(1 == (X4 == X3));        ASSERT(0 == (X4 != X3));
        ASSERT(1 == (X4 == X4));        ASSERT(0 == (X4 != X4));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 5. Set x3 using 'update' (set to VB)."
                             "\t\t{ x1:VA x2:VA x3:VB x4:U }" << endl;
        mX3.update(SB, LB);
        if (verbose) { cout << '\t'; P(X3); }

        if (verbose) cout << "\ta. Check new state of x3." << endl;
        ASSERT(CB == X3.checksum());

        if (verbose) cout << "\tb. Try equality operators: "
                             "x3 <op> x1, x2, x3, x4." << endl;
        ASSERT(0 == (X3 == X1));        ASSERT(1 == (X3 != X1));
        ASSERT(0 == (X3 == X2));        ASSERT(1 == (X3 != X2));
        ASSERT(1 == (X3 == X3));        ASSERT(0 == (X3 != X3));
        ASSERT(0 == (X3 == X4));        ASSERT(1 == (X3 != X4));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 6. Change x1 using 'reset'."
                             "\t\t{ x1:U x2:VA x3:VB x4:U }" << endl;
        mX1.reset();
        if (verbose) { cout << '\t'; P(X1); }

        if (verbose) cout << "\ta. Check new state of x1." << endl;
        ASSERT(CU == X1.checksum());

        if (verbose) cout << "\tb. Try equality operators: "
                             "x1 <op> x1, x2, x3, x4." << endl;
        ASSERT(1 == (X1 == X1));        ASSERT(0 == (X1 != X1));
        ASSERT(0 == (X1 == X2));        ASSERT(1 == (X1 != X2));
        ASSERT(0 == (X1 == X3));        ASSERT(1 == (X1 != X3));
        ASSERT(1 == (X1 == X4));        ASSERT(0 == (X1 != X4));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 7. Change x1 ('update', set to VC)."
                             "\t\t{ x1:VC x2:VA x3:VB x4:U }" << endl;
        mX1.update(SC, LC);
        if (verbose) { cout << '\t'; P(X1); }

        if (verbose) cout << "\ta. Check new state of x1." << endl;
        ASSERT(CC == X1.checksum());

        if (verbose) cout << "\tb. Try equality operators: "
                             "x1 <op> x1, x2, x3, x4." << endl;
        ASSERT(1 == (X1 == X1));        ASSERT(0 == (X1 != X1));
        ASSERT(0 == (X1 == X2));        ASSERT(1 == (X1 != X2));
        ASSERT(0 == (X1 == X3));        ASSERT(1 == (X1 != X3));
        ASSERT(0 == (X1 == X4));        ASSERT(1 == (X1 != X4));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 8. Assign x2 = x1."
                             "\t\t{ x1:VC x2:VC x3:VB x4:U }" << endl;
        mX2 = X1;
        if (verbose) { cout << '\t'; P(X2); }

        if (verbose) cout << "\ta. Check new state of x2." << endl;
        ASSERT(CC == X2.checksum());

        if (verbose) cout << "\tb. Try equality operators: "
                             "x2 <op> x1, x2, x3, x4." << endl;
        ASSERT(1 == (X2 == X1));        ASSERT(0 == (X2 != X1));
        ASSERT(1 == (X2 == X2));        ASSERT(0 == (X2 != X2));
        ASSERT(0 == (X2 == X3));        ASSERT(1 == (X2 != X3));
        ASSERT(0 == (X2 == X4));        ASSERT(1 == (X2 != X4));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 9. Assign x2 = x3."
                             "\t\t{ x1:VC x2:VB x3:VB x4:U }" << endl;
        mX2 = X3;
        if (verbose) { cout << '\t'; P(X2); }

        if (verbose) cout << "\ta. Check new state of x2." << endl;
        ASSERT(CB == X2.checksum());

        if (verbose) cout << "\tb. Try equality operators: "
                             "x2 <op> x1, x2, x3, x4." << endl;
        ASSERT(0 == (X2 == X1));        ASSERT(1 == (X2 != X1));
        ASSERT(1 == (X2 == X2));        ASSERT(0 == (X2 != X2));
        ASSERT(1 == (X2 == X3));        ASSERT(0 == (X2 != X3));
        ASSERT(0 == (X2 == X4));        ASSERT(1 == (X2 != X4));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 10. Assign x1 = x1."
                             "\t\t{ x1:VC x2:VB x3:VB x4:U }" << endl;
        mX1 = X1;
        if (verbose) { cout << '\t'; P(X1); }

        if (verbose) cout << "\ta. Check new state of x1." << endl;
        ASSERT(CC == X1.checksum());

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
        //   at least as efficient as the 'crc32' legacy implementation from
        //   dbutil.
        //
        // Plan:
        //   Incorporate, verbatim, the source code of the 'crc32trm' function
        //   from dbutil and make the minimal modifications required to compile
        //   and run it in the context of this test driver.  Compare the
        //   performance of 'crc32trm' against 'update' when applied to a small
        //   set of test strings within a loop of at least 1M iterations.
        //
        // Testing:
        //   void update(const void *data, int length);  // performance
        // --------------------------------------------------------------------

        if (verbose) cout << "\nPERFORMANCE TEST"
                          << "\n================" << endl;

        // make sure crc_table has been initialized
        if (!crc_table_computed) {
            make_crc_table();
        }

        const char *DATA[] = {
            "Propagating error message from interface BASS.TCP.N278, "
            "session 8389360, peer 172.17.133.154:28588, transmissions "
            "2/2147483647 for ServiceId = 36087 and version 3.21.  Attempt "
            "1/1 for priority 0, tried 2 route(s) :  [ ERROR_MSG_REROUTE "
            "BASS.TCP.P279 sid = 8389361 peer = 279 ] [ ERROR_MSG_REROUTE "
            "BASS.TCP.N278 sid = 8389360 peer = 278 ] .  Original request "
            "was received from peer -4856@0",

            "Failed to send message through the available interfaces (2 "
            "sessions).  ServiceId = 38413 and version 3.21.  Attempt 1/1 for "
            "priority 0, tried 2 route(s) :  [ "
            "ERROR_MSG_REROUTE BASS.TCP.P279 sid = 8389361 peer = 279 ]  [ "
            "ERROR_MSG_REROUTE BASS.TCP.N278 sid = 8389360 peer = 278 ]",

            "Notification failed for entry 29824 originally received from "
            "peer -7434@0, prolog {  [ MessageLength = 5104 HeaderNumWords = "
            "25 HeaderVer = 1 MessageType = RESPONSE ServiceId = 38391 "
            "ServiceVerMajor = 1 ServiceVerMinor = 4 ServiceVerPatch = 0 "
            "RequestId = 0 ServiceSessionId = 0 ClientSessionId = 0 "
            "PayloadEncoding = BER UserIdentificationNumWords = 9 "
            "ServiceEndpointNumWords = 0 ClientEndpointNumWords = 2 "
            "PayloadLength = 4981 NumControlBlocks = 1 ControlBlocksNumWords "
            "= 5 Flags = 0 ] userIdentificationWords = [ 1, 4805801, 1202805, "
            "551678, 318705500, 182, 3932, 25983, 0 ] serviceEndpointWords = "
            "[  ] clientEndpointWords  = [ 0, 29824 ] controlBlocks  = { "
            "[Role = 1 Encoding = BDEX NumWords = 3 ]} }"
        };

        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        bsl::cout << "Comparing BDE crc32 vs legacy" << bsl::endl;

        for (int i = 0; i < NUM_DATA; ++i) {
            Obj mX(DATA[i], bsl::strlen(DATA[i]));
            const Obj& X = mX;

            unsigned int crc32t =
                          crc32trm(DATA[i], bsl::strlen(DATA[i]), '\0');

            if (crc32t != X.checksum()) {
                bsl::cout << "CRC32 different: " << crc32t << "(legacy) != "
                          << X.checksum() << "(bde)." << bsl::endl;
            }
        }

        enum { NUM_ITERATIONS = 1000000 };

        {
            bsl::cout << "BDE crc32 run" << bsl::endl;

            Obj mX;  const Obj& X = mX;

            bsls::Stopwatch timer;
            timer.start();
            for (int i = 0; i < NUM_ITERATIONS; ++i) {
                for (int j = 0; j < NUM_DATA; ++j) {
                    mX.reset();
                    mX.update(DATA[j], bsl::strlen(DATA[j]));
                }
            }
            timer.stop();

            bsl::cout << "BDE CRC32: " << timer.elapsedTime() << " sec."
                      << bsl::endl;
        }

        {
            bsl::cout << "Legacy CRC32 run" << bsl::endl;

            bsls::Stopwatch timer;
            timer.start();
            for (int i = 0; i < NUM_ITERATIONS; ++i) {
                for (int j = 0; j < NUM_DATA; ++j) {
                    unsigned int crc32t =
                          crc32trm(DATA[j], bsl::strlen(DATA[j]), '\0');
                }
            }
            timer.stop();

            bsl::cout << "Legacy crc32: " << timer.elapsedTime() << " sec."
                      << bsl::endl;
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
