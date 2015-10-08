// bdlb_print.t.cpp                                                   -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <bdlb_print.h>

#include <bslim_testutil.h>

#include <bsls_platform.h>
#include <bsls_types.h>

#include <bsl_iomanip.h>
#include <bsl_iostream.h>
#include <bsl_sstream.h>
#include <bsl_string.h>

#include <bsl_cctype.h>      // 'bsl::isspace'
#include <bsl_cstdlib.h>     // 'atoi'
#include <bsl_cstring.h>     // 'bsl::strcmp', 'bsl::memset'

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
//                             Overview
//                             --------
// This component namespace for a set of functions, each tested independently.
// ----------------------------------------------------------------------------
// [ 3] indent(ostream& s, int l, int spl = 4);
// [ 4] newlineAndIndent(ostream& s, int l, int spl = 4)
// [ 2] printPtr(ostream& stream, const void *value);
// [ 8] printString(ostream& s, const char *s, int l, bool ebs = 0);
// [ 5] hexDump(ostream& s, const char *b, int l);
// [ 7] hexDump(ostream& s, bsl::pair<const char *, int> *b, int nb)
// [ 9] singleLineHexDump(ostream& s, INPUT_ITER b, INPUT_ITER e);
// [ 9] singleLineHexDump(ostream& s, const char *b, const char *e);
// [ 9] singleLineHexDump(ostream& s, const char *b, int l);
// [ 6] struct PrintStringHexDumper;
// [10] struct PrintStringSingleLineHexDumper;
// ----------------------------------------------------------------------------
// [ 1] USAGE EXAMPLE

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
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
// ----------------------------------------------------------------------------

// ============================================================================
//                     GLOBAL TYPEDEFS FOR TESTING
// ----------------------------------------------------------------------------

typedef bdlb::Print Util;


// ============================================================================
//              SUPPORTING FUNCTIONS USED FOR TESTING
// ----------------------------------------------------------------------------
// expectedOut[X] denotes the expected output for the input buffer of length
// 'X' (used in cases 5 and 6).

bsl::string expectedOut[] = {
                                                                           "",
     "     0:   00                                      |.               |\n",
     "     0:   0001                                    |..              |\n",
     "     0:   000102                                  |...             |\n",
     "     0:   00010203                                |....            |\n",
     "     0:   00010203 04                             |.....           |\n",
     "     0:   00010203 0405                           |......          |\n",
     "     0:   00010203 040506                         |.......         |\n",
     "     0:   00010203 04050607                       |........        |\n",
     "     0:   00010203 04050607 08                    |.........       |\n",
     "     0:   00010203 04050607 0809                  |..........      |\n",
     "     0:   00010203 04050607 08090A                |...........     |\n",
     "     0:   00010203 04050607 08090A0B              |............    |\n",
     "     0:   00010203 04050607 08090A0B 0C           |.............   |\n",
     "     0:   00010203 04050607 08090A0B 0C0D         |..............  |\n",
     "     0:   00010203 04050607 08090A0B 0C0D0E       |............... |\n",
     "     0:   00010203 04050607 08090A0B 0C0D0E0F     |................|\n",
     "     0:   00010203 04050607 08090A0B 0C0D0E0F     |................|\n"
     "    16:   10                                      |.               |\n",
};

// expectedOutCase7[X] denotes the expected output for case 7.

bsl::string expectedOutCase7[] = {
    "     0:   61626364 65666768 696A6B6C 6D6E6F70     |abcdefghijklmnop|\n"
    "    16:   71727374 75767778 797A6162 6364         |qrstuvwxyzabcd  |",

    "     0:   61616261 62636162 63646162 63646561     |aababcabcdabcdea|\n"
    "    16:   62636465 66616263 64656667 61626364     |bcdefabcdefgabcd|\n"
    "    32:   65666768 61626364 65666768 69616263     |efghabcdefghiabc|\n"
    "    48:   64656667 68696A61 62636465 66676869     |defghijabcdefghi|\n"
    "    64:   6A6B6162 63646566 6768696A 6B6C6162     |jkabcdefghijklab|\n"
    "    80:   63646566 6768696A 6B6C6D61 62636465     |cdefghijklmabcde|\n"
    "    96:   66676869 6A6B6C6D 6E616263 64656667     |fghijklmnabcdefg|\n"
    "   112:   68696A6B 6C6D6E6F 61626364 65666768     |hijklmnoabcdefgh|\n"
    "   128:   696A6B6C 6D6E6F70 61626364 65666768     |ijklmnopabcdefgh|\n"
    "   144:   696A6B6C 6D6E6F70 71616263 64656667     |ijklmnopqabcdefg|\n"
    "   160:   68696A6B 6C6D6E6F 707172                |hijklmnopqr     |",

    "     0:   61616161 61616161 61616161 61616161     |aaaaaaaaaaaaaaaa|\n"
    "    16:   42424242 42424242 61616161 61616161     |BBBBBBBBaaaaaaaa|\n"
    "    32:   61616161 61616161 42424242 42424242     |aaaaaaaaBBBBBBBB|\n"
    "    48:   61616161 61616161 61616161 61616161     |aaaaaaaaaaaaaaaa|\n"
    "    64:   42424242 42424242 61616161 61616161     |BBBBBBBBaaaaaaaa|\n"
    "    80:   61616161 61616161 42424242 42424242     |aaaaaaaaBBBBBBBB|\n"
};

// The following table of size 256 holds the hex representation of each byte:

static const bsl::string ASCII_HEX_PRESENTATION[256] = {
"00","01","02","03","04","05","06","07","08","09","0A","0B","0C","0D","0E","0F"
,
"10","11","12","13","14","15","16","17","18","19","1A","1B","1C","1D","1E","1F"
,
"20","21","22","23","24","25","26","27","28","29","2A","2B","2C","2D","2E","2F"
,
"30","31","32","33","34","35","36","37","38","39","3A","3B","3C","3D","3E","3F"
,
"40","41","42","43","44","45","46","47","48","49","4A","4B","4C","4D","4E","4F"
,
"50","51","52","53","54","55","56","57","58","59","5A","5B","5C","5D","5E","5F"
,
"60","61","62","63","64","65","66","67","68","69","6A","6B","6C","6D","6E","6F"
,
"70","71","72","73","74","75","76","77","78","79","7A","7B","7C","7D","7E","7F"
,
"80","81","82","83","84","85","86","87","88","89","8A","8B","8C","8D","8E","8F"
,
"90","91","92","93","94","95","96","97","98","99","9A","9B","9C","9D","9E","9F"
,
"A0","A1","A2","A3","A4","A5","A6","A7","A8","A9","AA","AB","AC","AD","AE","AF"
,
"B0","B1","B2","B3","B4","B5","B6","B7","B8","B9","BA","BB","BC","BD","BE","BF"
,
"C0","C1","C2","C3","C4","C5","C6","C7","C8","C9","CA","CB","CC","CD","CE","CF"
,
"D0","D1","D2","D3","D4","D5","D6","D7","D8","D9","DA","DB","DC","DD","DE","DF"
,
"E0","E1","E2","E3","E4","E5","E6","E7","E8","E9","EA","EB","EC","ED","EE","EF"
,
"F0","F1","F2","F3","F4","F5","F6","F7","F8","F9","FA","FB","FC","FD","FE","FF"
};

bsl::size_t generateHexRepresentation(char        *result,
                                      const char  *input,
                                      bsl::size_t  length)
    // Load into the specified 'result' a string that is the uppercase hex
    // representation of the specified 'input' string of the specified
    // 'length'.  Return the number of characters written.  Note that we will
    // use this function as an oracle.
{
    bsl::size_t ret = 0;
    for (bsl::size_t i = 0; i < length; ++i) {
        const bsl::string s = ASCII_HEX_PRESENTATION[
                                         static_cast<unsigned char>(input[i])];
        result[2 * i + 0] = s[0];
        result[2 * i + 1] = s[1];
        ret += 2;
    }
    result[ret] = 0;
    ASSERT(ret == 2 * length);
    return ret;
}

void generatePseudoRandomInputBufferData(char         *result,
                                         bsl::size_t   length,
                                         unsigned int  seed)
    // Load into the specified 'result' the specified 'length' number of bytes
    // pseudo-randomly generated using the specified 'seed'.
{
    static unsigned int value = seed;
    for (bsl::size_t i = 0; i < length; ++i) {
        value *= 11;
        value += 7;
        result[i] = char(value);
    }
}

void printPrintable(const char *input, int length)
{
    for (int i = 0; i < length; ++i) {
        char c = input[i];
        if (bsl::isspace(c)) {
            cout << '_';
        }
        else if (bsl::isalnum(c) || bsl::ispunct(c)) {
            cout << c;
        }
        else {
            cout << '.';
        }
    }
}

// The following is another implementation that can be used as a second oracle:

bsl::ostream& originalSingleLineHexDump(bsl::ostream&  stream,
                                        const char    *buffer,
                                        int            length)
{
    static const char HEX[] = "0123456789ABCDEF";

    const char *cursor = buffer;
    const char *end    = cursor + length;

    while (cursor != end) {
        const char value = *cursor;

        stream << HEX[(value >> 4) & 0xF]
               << HEX[value        & 0xF];

        ++cursor;
    }

    return stream;
}

// ============================================================================
//                             MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int             test = argc > 1 ? atoi(argv[1]) : 0;
    bool         verbose = argc > 2;
    bool     veryVerbose = argc > 3;
    bool veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
      case 10: {
        // --------------------------------------------------------------------
        // TESTING 'PrintStringSingleLineHexDumper' CLASS
        //
        // Concerns:
        //: 1 Dumping via the class results in the same output as if done to
        //:   the stream directly.
        //
        // Plan:
        //: 1 Ad hoc test.
        //
        // Testing:
        //  struct PrintStringSingleLineHexDumper;
        // --------------------------------------------------------------------

        if (verbose) cout <<
              "\n" "TESTING 'PrintStringSingleLineHexDumper' CLASS" "\n"
                   "==============================================" "\n";

        static const char   SOME_STRING[]   = "ABCDefghIJKLmnopQRSTuvwXYZ";
        static const size_t SOME_STRING_LEN = sizeof SOME_STRING - 1;

        bsl::ostringstream oss1;
        bsl::ostringstream oss2;

        oss1 << bdlb::PrintStringSingleLineHexDumper(SOME_STRING,
                                                    SOME_STRING_LEN);
        Util::singleLineHexDump(oss2, SOME_STRING, SOME_STRING_LEN);

        ASSERT(0 == strncmp(oss1.str().c_str(),
                            oss2.str().c_str(),
                            oss1.str().size()));

      } break;
      case 9: {
        // --------------------------------------------------------------------
        // TESTING 'singleLineHexDump' METHODS
        //
        // Concerns:
        //: 1 All ASCII character values (printable and non-printable) get
        //:   encoded correctly;
        //: 2 Exactly 'length' characters get dumped.
        //
        // Plan:
        //: 1 Build up a table of the hex representations of all 256 different
        //:   byte patterns.  After some preliminary tests, mechanically create
        //:   the expected output from the input and verify that that each of
        //:   the functions performs the same way.
        //
        // Testing:
        //   singleLineHexDump(ostream& s, INPUT_ITER b, INPUT_ITER e);
        //   singleLineHexDump(ostream& s, const char *b, const char *e);
        //   singleLineHexDump(ostream& s, const char *b, int l);
        // --------------------------------------------------------------------

        if (verbose) cout << "\n" "TESTING 'singleLineHexDump' METHODS" "\n"
                                  "===================================" "\n";

        static const char   LONG_STR[]   =
            "jjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjj";
        static const size_t LONG_STR_LEN = sizeof LONG_STR - 1;

        if (verbose) bsl::cout
                << "\nConcern 1: Test dumping characters" << bsl::endl;

        for (size_t i = 0; i <= 255; ++i) {
            bsl::ostringstream oss;
            char               c = static_cast<char>(i);

            Util::singleLineHexDump(oss, &c, 1) << bsl::flush;

            if (veryVerbose) {
                bsl::cout
                    << "Char: "      << bsl::setw(3) << i << ' '
                    << "  Stream: '" << oss.str() << '\'' << ' '
                    << "  Expected: '"
                             << ASCII_HEX_PRESENTATION[i] << '\'' << bsl::endl;
            }

            LOOP_ASSERT(i,
                        0 == strncmp(oss.str().c_str(),
                        ASCII_HEX_PRESENTATION[i].c_str(),
                        ASCII_HEX_PRESENTATION[i].size()));
        }

        if (verbose) bsl::cout << "\nConcern 2: Test dump length" << bsl::endl;

        for (size_t i = 0; i < LONG_STR_LEN; ++i) {
            bsl::ostringstream oss;

            Util::singleLineHexDump(oss, LONG_STR, i) << bsl::flush;

            if (veryVerbose) {
                bsl::cout
                    << "i: " << i << ' '
                    << "length: " << oss.str().size() << bsl::endl;
            }

            LOOP_ASSERT(i, oss.str().size() == i*2);
        }

        if (verbose) bsl::cout
            << "\nNow test all 3 functions against two oracles." << bsl::endl;

        const size_t k_SIZE = 2050;
        char         inputBuffer[k_SIZE];
                                    // big enough to hold any of the following:
        bsl::size_t  lengths[] =
                        { 0, 1, 1, 1, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4, 5, 5, 5, 7,
                          7, 7, 8, 8, 8, 9, 9, 9, 15, 15, 16, 16, 17, 17, 31,
                         32, 33, 127, 128, 129, 255, 256, 257, 511, 512, 513 };
        const int    N = sizeof lengths / sizeof *lengths;
        char         outBuf[2 * k_SIZE + 1]; // big enough to hold rep + null.

        for (int i = 0; i < N; ++i) {
            bsl::size_t  len  = lengths[i];
            unsigned int seed = 19;

            if (veryVerbose) { P(N) }

            generatePseudoRandomInputBufferData(inputBuffer, len, seed);
            bsl::size_t outLen = generateHexRepresentation(outBuf,
                                                           inputBuffer,
                                                           len);

            if (veryVeryVerbose || (veryVerbose && len < 50)) {
                 cout << "     Input: "; printPrintable(inputBuffer, len);
                 cout << endl;
                 cout << "    Output: " << outBuf << endl;
            }

            const char *const INPUT_BUFFER = inputBuffer; // for non-template

            {
                bsl::ostringstream out1;
                Util::singleLineHexDump<const char *>(out1,
                                                      inputBuffer,
                                                      inputBuffer + len)
                                                         << '\0' << bsl::flush;
                LOOP2_ASSERT(i, len,
                             outLen == bsl::strlen(out1.str().c_str()));
                LOOP2_ASSERT(i, len,
                             0      == bsl::strcmp(out1.str().c_str(),
                                                   outBuf));
            }

            {
                bsl::ostringstream out2;
                Util::singleLineHexDump(out2,
                                        INPUT_BUFFER,
                                        INPUT_BUFFER + len)
                                                         << '\0' << bsl::flush;
                LOOP2_ASSERT(i, len,
                             outLen == bsl::strlen(out2.str().c_str()));
                LOOP2_ASSERT(i, len,
                             0      == bsl::strcmp(out2.str().c_str(),
                                                   outBuf));
            }

            {
                bsl::ostringstream out3;
                Util::singleLineHexDump(out3,
                                        inputBuffer,
                                        len) << '\0' << bsl::flush;
                LOOP2_ASSERT(i, len,
                             outLen == bsl::strlen(out3.str().c_str()));
                LOOP2_ASSERT(i, len,
                             0      == bsl::strcmp(out3.str().c_str(),
                                                   outBuf));
            }

            {
                bsl::ostringstream out4;
                ::originalSingleLineHexDump(out4,
                                            inputBuffer,
                                            len) << '\0' << bsl::flush;
                LOOP2_ASSERT(i, len,
                             outLen == bsl::strlen(out4.str().c_str()));
                LOOP2_ASSERT(i, len,
                             0      == bsl::strcmp(out4.str().c_str(),
                                                   outBuf));
            }
        }

      } break;
      case 8: {
        // --------------------------------------------------------------------
        // TESTING 'printString' METHOD
        //   This will test the 'printString' method
        //
        // Concerns:
        //: 1 Non-printable characters must be printed using their hexadecimal
        //:   representation.
        //:
        //: 2 Interleaved printable and non-printable characters must work as
        //:   expected.
        //:
        //: 3 Printing with bad streams should be a no-op.
        //:
        //: 4 If the expandBackSlash is 'false' (default case) then '\' is not
        //:   output as '\\', else it is.
        //
        // Plan:
        //: 1 For a series of values print the test string to an 'ostream' and
        //:   verify that the result is as expected.
        //
        // Testing:
        //   printString(ostream& s, const char *s, int l, bool ebs = 0);
        // --------------------------------------------------------------------

        if (verbose) cout << "\n" "TESTING 'printString' METHOD" "\n"
                                  "============================" "\n";
        static const struct {
            int         d_lineNum;
            const char *d_spec;
            bool        d_expandSlash;
            const char *d_expectedResult;
        } DATA[] = {
            //LINE   SPEC             EXPAND  EXPECTED_RESULT
            //----   ----             ------  ---------------

            // Printable characters of increasing length interleaved with '\'
            { L_,    "",              false,  ""               },
            { L_,    "",              true,   ""               },
            { L_,    "\\",            false,  "\\"             },
            { L_,    "\\",            true,   "\\\\"           },
            { L_,    "a",             false,  "a"              },
            { L_,    "a",             true,   "a"              },
            { L_,    "A",             false,  "A"              },
            { L_,    "A",             true,   "A"              },
            { L_,    "a\\",           false,  "a\\"            },
            { L_,    "a\\",           true,   "a\\\\"          },
            { L_,    "aA",            false,  "aA"             },
            { L_,    "aA",            true,   "aA"             },
            { L_,    "a\\A",          false,  "a\\A"           },
            { L_,    "a\\A",          true,   "a\\\\A"         },
            { L_,    "abc\\D",        false,  "abc\\D"         },
            { L_,    "abc\\D",        true,   "abc\\\\D"       },
            { L_,    "aBcDe\\",       false,  "aBcDe\\"        },
            { L_,    "aBcDe\\",       true,   "aBcDe\\\\"      },

            // Hex conversion with interleaved '\'.  Note: 0x00 is tested
            // separately below.
            { L_,    "\x01",          false,  "\\x01"          },
            { L_,    "\x01",          true,   "\\x01"          },
            { L_,    "\x01\\",        false,  "\\x01\\"        },
            { L_,    "\x01\\",        true,   "\\x01\\\\"      },
            { L_,    "\x02",          false,  "\\x02"          },
            { L_,    "\x02",          true,   "\\x02"          },
            { L_,    "\x02\\",        false,  "\\x02\\"        },
            { L_,    "\x02\\",        true,   "\\x02\\\\"      },
            { L_,    "\n",            false,  "\\x0A"          },
            { L_,    "\n",            true,   "\\x0A"          },
            { L_,    "\n\\",          false,  "\\x0A\\"        },
            { L_,    "\n\\",          true,   "\\x0A\\\\"      },
            { L_,    "\\\r",          false,  "\\\\x0D"        },
            { L_,    "\\\r",          true,   "\\\\\\x0D"      },
            { L_,    "\x7F",          false,  "\\x7F"          },
            { L_,    "\x7F",          true,   "\\x7F"          },
            { L_,    "\x80",          false,  "\\x80"          },
            { L_,    "\x80",          true,   "\\x80"          },
            { L_,    "\x81",          false,  "\\x81"          },
            { L_,    "\x81",          true,   "\\x81"          },
            { L_,    "\xFE\\",        false,  "\\xFE\\"        },
            { L_,    "\xFE\\",        true,   "\\xFE\\\\"      },
            { L_,    "\\\xFF",        false,  "\\\\xFF"        },
            { L_,    "\\\xFF",        true,   "\\\\\\xFF"      },

            // Interleaved printable, non-printable and slash.
            { L_,    "a\rb\nc\\",     false,  "a\\x0Db\\x0Ac\\"       },
            { L_,    "a\rb\nc\\",     true,   "a\\x0Db\\x0Ac\\\\"     },
            { L_,    "\\aA\rbB\ncC",  false,  "\\aA\\x0DbB\\x0AcC"    },
            { L_,    "\\aA\rbB\ncC",  true,   "\\\\aA\\x0DbB\\x0AcC"  },
            { L_,    " \r\n",         false,  " \\x0D\\x0A"           },
            { L_,    " \r\n",         true,   " \\x0D\\x0A"           },
            { L_,    "\r\nx",         false,  "\\x0D\\x0Ax"           },
            { L_,    "\r\nx",         true,   "\\x0D\\x0Ax"           },
            { L_,    "\r\nz\\y",      false,  "\\x0D\\x0Az\\y"        },
            { L_,    "\r\nz\\y",      true,   "\\x0D\\x0Az\\\\y"      },
            { L_,    "a\r\nz\\y",     false,  "a\\x0D\\x0Az\\y"       },
            { L_,    "a\r\nz\\y",     true,   "a\\x0D\\x0Az\\\\y"     },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        if (verbose) cout << "\nTesting with valid stream." << endl;
        for (int i = 0; i < NUM_DATA; ++i) {
            const int   LINE            = DATA[i].d_lineNum;
            const char *SPEC            = DATA[i].d_spec;
            const bool  EXPAND          = DATA[i].d_expandSlash;
            const char *EXPECTED_RESULT = DATA[i].d_expectedResult;
            const int   LEN             = bsl::strlen(SPEC);

            if (veryVerbose) {
                    P_(LINE)
                    P_(SPEC)
                    P_(EXPAND)
                    P_(EXPECTED_RESULT)
                    P(LEN)
            }

            stringstream ss;

            ostream& ret = Util::printString(ss, SPEC, LEN, EXPAND);

            LOOP_ASSERT(LINE, &ss == &ret);
            LOOP3_ASSERT(LINE, EXPECTED_RESULT,   ss.str(),
                               EXPECTED_RESULT == ss.str());
        }

        if (verbose) cout << "\nTesting with invalid stream." << endl;
        for (int i = 0; i < NUM_DATA; ++i) {
            const int   LINE   = DATA[i].d_lineNum;
            const char *SPEC   = DATA[i].d_spec;
            const int   LEN    = bsl::strlen(SPEC);
            const bool  EXPAND = DATA[i].d_expandSlash;

            if (veryVerbose) {
                P_(LINE)
                P_(SPEC)
                P_(LEN)
                P(EXPAND)
            }

            stringstream ss;

            ss.setstate(ios_base::badbit);

            ostream& ret = Util::printString(ss, SPEC, LEN, EXPAND);

            LOOP_ASSERT(LINE, &ss == &ret);
            LOOP2_ASSERT(LINE, ss.str(), "" == ss.str());
        }

        if (verbose) cout << "\nTesting null hex conversion." << endl;
        {
            const char SPEC[] = "\x00";
            const int  LEN    = bsl::strlen(SPEC);

            const char EXPECTED_RETURN[] = "";

            stringstream ss;

            ostream& ret = Util::printString(ss, SPEC, LEN, false);

            ASSERT(&ss == &ret);
            LOOP2_ASSERT(ss.str(), EXPECTED_RETURN,
                         EXPECTED_RETURN == ss.str());
        }
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING 'hexDump' (MULTIPLE BUFFERS)
        //
        // Concerns:
        //: 1 When multiple buffers are printed out, on the border where one
        //:   buffer ends and a second buffer begins must have a seemless
        //:   transition, one should not be able to tell where one buffer ends
        //:   and the next begins.
        //:
        //:   1 Small buffers test: Test when a supplied buffer is smaller than
        //:     'CHAR_PER_LINE', resulting in multiple buffers printed out per
        //:     line.
        //:
        //:   2 Variable sizes buffer test: Test when supplied buffers are not
        //:     all the same size.
        //:
        //:   3 Buffer endpoint test: Test when buffer size end in the middle
        //:     of a line, and when it ends at the end of a line.
        //
        // Plan:
        //: 1 Small buffers test: Create a number of small static buffers, such
        //:   that for each line, multiple buffers will be used.  Pass these
        //:   into the function and verify output is correct.
        //:
        //: 2 Variable sizes buffer test: Create a number of different sized
        //:   static buffers.  Pass these into the function and verify output
        //:   is correct.
        //:
        //: 3 Buffer endpoint test: Create a number of buffers of size
        //:   'CHAR_PER_LINE * 1.5'.  The end of the first buffer must end at
        //:   the middle of the line, where the second buffer will be used to
        //:   finish the line.  The next buffer will end precisely at the end
        //:   of the third line, so this will test buffers that end exactly at
        //:   the end of a line as well.  Verify that output is correct.
        //
        // Testing:
        //    hexDump(ostream& s, bsl::pair<const char *, int> *b, int nb)
        // --------------------------------------------------------------------

        if (verbose) cout << "\n" "TESTING 'hexDump' (MULTIPLE BUFFERS)" "\n"
                                  "====================================" "\n";

        {
            if (verbose) cout << "a.  Small buffers test."  << endl;

            enum { k_NUM_STATIC_BUFFERS = 30,
                   k_SIZE               = 1024 };

            bsl::pair<const char *, int> buffers[k_NUM_STATIC_BUFFERS];
            char                         staticBuffers[k_NUM_STATIC_BUFFERS]
                                                                           [1];

            for (int i=0; i < k_NUM_STATIC_BUFFERS; i++) {
                staticBuffers[i][0] = 'a' + (i % 26);
                buffers[i]          = bsl::make_pair(&staticBuffers[i][0], 1);
            }

            bsl::stringstream out;
            Util::hexDump(out, buffers, k_NUM_STATIC_BUFFERS);
            ASSERT(0 == strncmp(out.str().c_str(),
                                expectedOutCase7[0].c_str(),
                                expectedOutCase7[0].size()));

            if (veryVerbose) {
                bsl::cout << "Hexdumped String :\n"
                          << out.str()
                          << "Expected String  :\n"
                          << expectedOutCase7[0] << bsl::endl;
            }
        }
        {
            if (verbose) cout << "b.  Variable sizes buffer test."  << endl;
            enum { k_NUM_STATIC_BUFFERS = 18,
                   k_SIZE               = 1024 };

            bsl::pair<const char *, int> buffers[k_NUM_STATIC_BUFFERS];
            char                         staticBuffers[k_NUM_STATIC_BUFFERS]
                                                        [k_NUM_STATIC_BUFFERS];

            for (int i = 0; i < k_NUM_STATIC_BUFFERS; i++) {
                for (int j = 0; j <= i; j ++ ) {
                    staticBuffers[i][j] = 'a' + (j % 26);
                }
                buffers[i] = bsl::make_pair(&staticBuffers[i][0], i + 1);
            }

            bsl::stringstream out;
            Util::hexDump(out, buffers, k_NUM_STATIC_BUFFERS);
            ASSERT(0 == strncmp(out.str().c_str(),
                                expectedOutCase7[1].c_str(),
                                expectedOutCase7[1].size()));

            if (veryVerbose) {
                bsl::cout << "Hexdumped String :\n"
                          << out.str()
                          << "Expected String  :\n"
                          << expectedOutCase7[1] << bsl::endl;
            }
        }
        {
            if (verbose) cout << "c.  Buffer endpoint test."  << endl;
            enum { k_NUM_STATIC_BUFFERS =    4 ,
                   k_CHAR_PER_LINE      =   16,  // defined in 'bdlb_print'
                   k_SIZE               = 1024
            };

            bsl::pair<const char *, int> buffers[k_NUM_STATIC_BUFFERS];
            char                         staticBuffers[k_NUM_STATIC_BUFFERS]
                                                         [2 * k_CHAR_PER_LINE];

            for (int i=0; i < k_NUM_STATIC_BUFFERS; i++) {
                for (int j=0; j < k_CHAR_PER_LINE; j++ ) {
                    staticBuffers[i][j]                   = 'a';
                    staticBuffers[i][k_CHAR_PER_LINE + j] = 'B';
                }
                buffers[i] = bsl::make_pair(&staticBuffers[i][0],
                                            k_CHAR_PER_LINE
                                          + k_CHAR_PER_LINE/2);
            }

            bsl::stringstream out;
            Util::hexDump(out, buffers, k_NUM_STATIC_BUFFERS);
            ASSERT(0 == strncmp(out.str().c_str(),
                                expectedOutCase7[2].c_str(),
                                expectedOutCase7[2].size()));

            if (veryVerbose) {
                bsl::cout << "Hexdumped String :\n"
                          << out.str()
                          << "Expected String  :\n"
                          << expectedOutCase7[2] << bsl::endl;
            }
        }

      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING 'PrintStringHexDumper'
        //
        // Concerns:
        //: 1 That the output resulting from the use of this struct and its
        //:   output operator is the same as the corresponding dump method.
        //
        // Plan:
        //: 1 Use the same test input and expected output as in case 5.
        //
        // Testing:
        //   struct PrintStringHexDumper;
        // --------------------------------------------------------------------

        if (verbose) cout << "\n" "TESTING 'PrintStringHexDumper'" "\n"
                                  "==============================" "\n";
        {
            const int SIZE = 256;

            // prepare input buffer
            char input[SIZE];
            for (int i = 0; i < SIZE; ++i) {
                input[i] = static_cast<char>(i);
            }

            static const struct {
                int         d_line;
                bsl::string d_expected;
                bsl::string d_input;
            } DATA[] = {
                //line no.  expected output                 input
                //-------   ---------------        ----------------------
                { L_,       expectedOut[ 0],       bsl::string(input,  0)   },
                { L_,       expectedOut[ 1],       bsl::string(input,  1)   },
                { L_,       expectedOut[ 2],       bsl::string(input,  2)   },
                { L_,       expectedOut[ 3],       bsl::string(input,  3)   },
                { L_,       expectedOut[ 4],       bsl::string(input,  4)   },
                { L_,       expectedOut[ 5],       bsl::string(input,  5)   },
                { L_,       expectedOut[ 6],       bsl::string(input,  6)   },
                { L_,       expectedOut[ 7],       bsl::string(input,  7)   },
                { L_,       expectedOut[ 8],       bsl::string(input,  8)   },
                { L_,       expectedOut[ 9],       bsl::string(input,  9)   },
                { L_,       expectedOut[10],       bsl::string(input, 10)   },
                { L_,       expectedOut[11],       bsl::string(input, 11)   },
                { L_,       expectedOut[12],       bsl::string(input, 12)   },
                { L_,       expectedOut[13],       bsl::string(input, 13)   },
                { L_,       expectedOut[14],       bsl::string(input, 14)   },
                { L_,       expectedOut[15],       bsl::string(input, 15)   },
                { L_,       expectedOut[16],       bsl::string(input, 16)   }
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int          LINE     = DATA[ti].d_line;
                const bsl::string& EXPECTED = DATA[ti].d_expected;
                const bsl::string& INPUT    = DATA[ti].d_input;

                bsl::stringstream out;
                out << bdlb::PrintStringHexDumper(INPUT.c_str(),
                                                  INPUT.length());

                if (veryVerbose) {
                    P_(LINE)  P_(INPUT)  P_(EXPECTED)  P(out.str());
                }

                LOOP_ASSERT(LINE, out.str() == EXPECTED);
            }
        }

      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING 'hexDump'
        //
        // Concerns:
        //: 1 The input buffer is formatted correctly.
        //
        // Plan:
        //: 2 Use the format array at the top of this file to test inputs of
        //:   varying length from 0 to 16.
        //
        // Testing:
        //   hexDump(ostream& s, const char *b, int l);
        // --------------------------------------------------------------------

        if (verbose) cout << "\n" "TESTING 'hexDump'" "\n"
                                  "=================" "\n";
        {
            const int k_SIZE = 256;

            // Prepare input buffer.

            char input[k_SIZE];
            for (int i = 0; i < k_SIZE; ++i) {
                input[i] = static_cast<char>(i);
            }

            static const struct {
                int         d_line;
                bsl::string d_expected;
                bsl::string d_input;
            } DATA[] = {
                //LINE      EXPECTED output        INPUT
                //----      ---------------        ----------------------
                { L_,       expectedOut[ 0],       bsl::string(input,  0)   },
                { L_,       expectedOut[ 1],       bsl::string(input,  1)   },
                { L_,       expectedOut[ 2],       bsl::string(input,  2)   },
                { L_,       expectedOut[ 3],       bsl::string(input,  3)   },
                { L_,       expectedOut[ 4],       bsl::string(input,  4)   },
                { L_,       expectedOut[ 5],       bsl::string(input,  5)   },
                { L_,       expectedOut[ 6],       bsl::string(input,  6)   },
                { L_,       expectedOut[ 7],       bsl::string(input,  7)   },
                { L_,       expectedOut[ 8],       bsl::string(input,  8)   },
                { L_,       expectedOut[ 9],       bsl::string(input,  9)   },
                { L_,       expectedOut[10],       bsl::string(input, 10)   },
                { L_,       expectedOut[11],       bsl::string(input, 11)   },
                { L_,       expectedOut[12],       bsl::string(input, 12)   },
                { L_,       expectedOut[13],       bsl::string(input, 13)   },
                { L_,       expectedOut[14],       bsl::string(input, 14)   },
                { L_,       expectedOut[15],       bsl::string(input, 15)   },
                { L_,       expectedOut[16],       bsl::string(input, 16)   }
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int          LINE     = DATA[ti].d_line;
                const bsl::string& EXPECTED = DATA[ti].d_expected;
                const bsl::string& INPUT    = DATA[ti].d_input;

                bsl::stringstream out;
                Util::hexDump(out, INPUT.c_str(), INPUT.length());

                if (veryVerbose) {
                    P_(LINE)  P_(INPUT)  P_(EXPECTED)  P(out.str());
                }

                LOOP_ASSERT(LINE, out.str() == EXPECTED);
            }
        }

      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING 'newLineAndIndent'
        //
        // Concerns:
        //: 1 Ensure that the newlineAndIndent method prints properly for:
        //:   1 Negative, 0, and positive levels.
        //:   2 Negative, 0, and positive spaces per level.
        //
        // Plan:
        //: 1 Specifying a set of test vectors and verify the return value.
        //
        // Testing:
        //   newlineAndIndent(ostream& s, int l, int spl = 4)
        // --------------------------------------------------------------------

        if (verbose) cout << "\n" "TESTING 'newLineAndIndent'" "\n"
                                  "==========================" "\n";

        {
            static const struct {
                int         d_lineNum;  // source line number
                int         d_indent;   // indentation level
                int         d_spaces;   // spaces per indentation level
                const char *d_fmt_p;    // expected output format
            } DATA[] = {
                //line  indent +/-  spaces/Tab  format
                //----  ----------  ----------  ------
                { L_,     -1,          0,       "\n"     },
                { L_,     -1,          1,       "\n "    },
                { L_,      0,          0,       "\n"     },
                { L_,      0,          1,       "\n"     },
                { L_,      1,          0,       "\n"     },
                { L_,      1,          1,       "\n "    },
                { L_,      1,          2,       "\n  "   },
                { L_,      2,          0,       "\n"     },
                { L_,      2,          1,       "\n  "   },
                { L_,      2,          2,       "\n    " },
                { L_,     -1,         -1,       " "      },
                { L_,      0,         -1,       " "      },
                { L_,      1,         -1,       " "      }
            };

            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            const int  SIZE = 128;        // Must be big enough to hold output
                                          // string.
            const char Z1 = static_cast<char>(0xff);
                                          // Used to represent an unset 'char'.
            const char Z2 = 0x00;         // Value 2 used to represent an unset
                                          // 'char'.

            char        mCtrlBuf1[SIZE];  bsl::memset(mCtrlBuf1, Z1, SIZE);
            char        mCtrlBuf2[SIZE];  bsl::memset(mCtrlBuf2, Z2, SIZE);
            const char *CTRL_BUF1 = mCtrlBuf1;
            const char *CTRL_BUF2 = mCtrlBuf2;

            for (int ti = 0; ti < NUM_DATA;  ++ti) {
                const int         LINE = DATA[ti].d_lineNum;
                const int         IND  = DATA[ti].d_indent;
                const int         SPL  = DATA[ti].d_spaces;
                const char *const FMT  = DATA[ti].d_fmt_p;

                if (veryVerbose) { P_(IND); P(SPL); }

                if (veryVeryVerbose) {
                    cout << "EXPECTED FORMAT: '" << FMT << '\'' << endl;
                }

                ostringstream out1(bsl::string(CTRL_BUF1, SIZE));
                Util::newlineAndIndent(out1, IND, SPL);  out1 << ends;

                ostringstream out2(bsl::string(CTRL_BUF2, SIZE));
                Util::newlineAndIndent(out2, IND, SPL);  out2 << ends;

                if (veryVeryVerbose) {
                    cout << "ACTUAL FORMAT:   '" << out1.str() << '\'' << endl;
                }

                const int SZ   = bsl::strlen(FMT) + 1;
                const int REST = SIZE - SZ;
                LOOP_ASSERT(LINE, SZ < SIZE);  // Check buffer is large enough.
                LOOP_ASSERT(LINE,
                            Z1 == out1.str()[SIZE - 1]);  // Check for overrun.
                LOOP_ASSERT(LINE,
                            Z2 == out2.str()[SIZE - 1]);  // Check for overrun.
                LOOP_ASSERT(LINE,
                            0 == bsl::memcmp(out1.str().c_str(), FMT, SZ));
                LOOP_ASSERT(LINE,
                            0 == bsl::memcmp(out2.str().c_str(), FMT, SZ));
                LOOP_ASSERT(LINE,0 == bsl::memcmp(out1.str().c_str() + SZ,
                                                  CTRL_BUF1 + SZ,
                                                  REST));
                LOOP_ASSERT(LINE, 0 == bsl::memcmp(out2.str().c_str() + SZ,
                                                   CTRL_BUF2 + SZ,
                                                   REST));
            }
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING 'indent'
        //
        // Concerns:
        //: 1 The indent method prints properly for:
        //:   1 Negative, 0, and positive levels.
        //:   2 Negative, 0, and positive spaces per level.
        //
        // Plan:
        //: 1 Specifying a set of test vectors and verify the return value.
        //
        // Testing:
        //   indent(ostream& s, int l, int spl = 4);
        // --------------------------------------------------------------------

        if (verbose) cout << "\n" "TESTING 'indent'" "\n"
                                  "================" "\n";

        {
            static const struct {
                int         d_lineNum;  // source line number
                int         d_indent;   // indentation level
                int         d_spaces;   // spaces per indentation level
                const char *d_fmt_p;    // expected output format
            } DATA[] = {
                //line  indent +/-  spaces/Tab  format
                //----  ----------  ----------  ------
                { L_,     -1,          0,       ""     },
                { L_,     -1,          1,       ""     },
                { L_,      0,          0,       ""     },
                { L_,      0,          1,       ""     },
                { L_,      1,          0,       ""     },
                { L_,      1,          1,       " "    },
                { L_,      1,          2,       "  "   },
                { L_,      2,          0,       ""     },
                { L_,      2,          1,       "  "   },
                { L_,      2,          2,       "    " },

                { L_,     -1,         -1,       ""     },
                { L_,      0,         -1,       ""     },
                { L_,      1,         -1,       " "    },
                { L_,      1,         -2,       "  "   },
                { L_,      2,         -1,       "  "   },
                { L_,      2,         -2,       "    " }
            };

            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            const int  SIZE = 128;   // Must be big enough to hold the output
                                     // string.
            const char Z1   = static_cast<char>(0xff);
                                     // Used to represent an unset 'char'.
            const char Z2   = 0x00;  // Value 2 used to represent an unset
                                     // 'char'.

            char mCtrlBuf1[SIZE];  bsl::memset(mCtrlBuf1, Z1, SIZE);
            char mCtrlBuf2[SIZE];  bsl::memset(mCtrlBuf2, Z2, SIZE);

            const char *CTRL_BUF1 = mCtrlBuf1;
            const char *CTRL_BUF2 = mCtrlBuf2;

            for (int ti = 0; ti < NUM_DATA;  ++ti) {
                const int         LINE   = DATA[ti].d_lineNum;
                const int         IND    = DATA[ti].d_indent;
                const int         SPL    = DATA[ti].d_spaces;
                const char *const FMT    = DATA[ti].d_fmt_p;

                if (veryVerbose) { P_(IND); P(SPL); }
                if (veryVeryVerbose) {
                    cout << "EXPECTED FORMAT: '" << FMT << '\'' << endl;
                }
                ostringstream out1(bsl::string(CTRL_BUF1, SIZE));
                Util::indent(out1, IND, SPL);  out1 << ends;
                ostringstream out2(bsl::string(CTRL_BUF2, SIZE));
                Util::indent(out2, IND, SPL);  out2 << ends;
                if (veryVeryVerbose) {
                    cout << "ACTUAL FORMAT:   '" << out1.str() << '\'' << endl;
                }

                const int SZ   = bsl::strlen(FMT) + 1;
                const int REST = SIZE - SZ;
                LOOP_ASSERT(LINE, SZ < SIZE);  // Check buffer is large enough.
                LOOP_ASSERT(LINE,
                            Z1 == out1.str()[SIZE - 1]);  // Check for overrun.
                LOOP_ASSERT(LINE,
                            Z2 == out2.str()[SIZE - 1]);  // Check for overrun.
                LOOP_ASSERT(LINE,
                            0 == bsl::memcmp(out1.str().c_str(), FMT, SZ));
                LOOP_ASSERT(LINE,
                            0 == bsl::memcmp(out2.str().c_str(), FMT, SZ));
                LOOP_ASSERT(LINE, 0 == bsl::memcmp(out1.str().c_str() + SZ,
                                                   CTRL_BUF1 + SZ,
                                                   REST));
                LOOP_ASSERT(LINE, 0 == bsl::memcmp(out2.str().c_str() + SZ,
                                                   CTRL_BUF2 + SZ,
                                                   REST));
            }
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING 'printPtr'
        //
        // Concerns:
        //: 1 Leading zeros are suppressed.
        //: 2 Embedded zeros are displayed.
        //: 3 Values do not get confused for negative numbers.
        //: 4 Case is always lower case for a, b, c, d, e, and f.
        //: 5 Upper and lower 32-bits treated the same.
        //
        // Plan:
        //: 1 Create a table consisting of high an low 32-bit address values
        //:   and the expected print value.  Depending on the architecture test
        //:   all the addresses or just those with the upper 32-bit values 0.
        //
        // Testing:
        //   printPtr(ostream& stream, const void *value);
        // --------------------------------------------------------------------

        if (verbose) cout << "\n" "TESTING 'printPtr'" "\n"
                                  "==================" "\n";

        static const struct {
            unsigned int  d_addr1;             // MS 32 bits
            unsigned int  d_addr2;             // LS 32 bits
            const char   *d_str;
        } DATA[] = {
                {  0x0,           0x0,              "0"                },
                {  0x0,           0x1,              "1"                },
                {  0x0,           0x5,              "5"                },
                {  0x0,           0xa,              "a"                },
                {  0x0,           0xb,              "b"                },
                {  0x0,           0x6,              "6"                },
                {  0x0,           0x65,             "65"               },
                {  0x0,           0x654,            "654"              },
                {  0x0,           0x6543,           "6543"             },
                {  0x0,           0x65432,          "65432"            },
                {  0x0,           0x654321,         "654321"           },
                {  0x0,           0x6543210,        "6543210"          },
                {  0x0,           0x6543210a,       "6543210a"         },
                {  0x6,           0x543210ab,       "6543210ab"        },
                {  0x65,          0x43210abc,       "6543210abc"       },
                {  0x654,         0x3210abcd,       "6543210abcd"      },
                {  0x6543,        0x210abcde,       "6543210abcde"     },
                {  0x65432,       0x10abcdef,       "6543210abcdef"    },
                {  0x654321,      0x0abcdef9,       "6543210abcdef9"   },
                {  0x6543210,     0xabcdef98,       "6543210abcdef98"  },
                {  0x6543210A,    0xbcdef987,       "6543210abcdef987" },
                {  0x0,           0xf,              "f"                },
                {  0x0,           0xf0,             "f0"               },
                {  0x0,           0xf00,            "f00"              },
                {  0x0,           0xf000,           "f000"             },
                {  0x0,           0xf0000,          "f0000"            },
                {  0x0,           0xf00000,         "f00000"           },
                {  0x0,           0xf000000,        "f000000"          },
                {  0x0,           0xf0000000,       "f0000000"         },
                {  0xf,           0x0,              "f00000000"        },
                {  0xf0,          0x0,              "f000000000"       },
                {  0xf00,         0x0,              "f0000000000"      },
                {  0xf000,        0x0,              "f00000000000"     },
                {  0xf0000,       0x0,              "f000000000000"    },
                {  0xf00000,      0x0,              "f0000000000000"   },
                {  0xf000000,     0x0,              "f00000000000000"  },
                {  0xf0000000,    0x0,              "f000000000000000" },
                {  0x0,           0xf0000009,       "f0000009"         },
                {  0x0,           0x12345678,       "12345678"         },
                {  0x0,           0xABCDEF12,       "abcdef12"         },
                {  0x0,           0x01020304,       "1020304"          },
                {  0x01020304,    0x05060708,       "102030405060708"  },
                {  0x0,           0xA0B0C0D0,       "a0b0c0d0"         },
                {  0x0,           0xFEeEeEeE,       "feeeeeee"         },
                {  0xFEeeeeee,    0xeeeeeeee,       "feeeeeeeeeeeeeee" },
                {  0xffffffff,    0xffffffff,       "ffffffffffffffff" },
                {  0xDCBAffff,    0xffffffff,       "dcbaffffffffffff" },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int t = 0; t < NUM_DATA; ++t) {
            char CTRL_BUF[100];
            bsl::memset(CTRL_BUF, 0xff, sizeof CTRL_BUF);

            ostringstream out(bsl::string(CTRL_BUF, sizeof CTRL_BUF));

            // test if 64-bit pointers or if value has only 32 bits
            #if defined(BSLS_PLATFORM_CPU_64_BIT)
            {
                void *p = reinterpret_cast<void *>(
                       (static_cast<bsls::Types::Int64>(DATA[t].d_addr1) << 32)
                     + DATA[t].d_addr2);

                Util::printPtr(out, p);
                out << ends;

                if (veryVerbose) {
                    cout << "EXPECTED: " << DATA[t].d_str;
                    cout << "  ACTUAL: " << out.str();
                    cout << endl;
                }

                ASSERT(0 == bsl::strcmp(out.str().c_str(), DATA[t].d_str));
            }
            #else  // BSLS_PLATFORM_CPU_32_BIT
            if (0 == DATA[t].d_addr1) {
                    Util::printPtr(out, (void *) DATA[t].d_addr2);
                out << ends;

                if (veryVerbose) {
                    cout << "EXPECTED: " << DATA[t].d_str;
                    cout << "  ACTUAL: " << out.str();
                    cout << endl;
                }

                ASSERT(0 == bsl::strcmp(out.str().c_str(), DATA[t].d_str));
            }
            #endif
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //   Extracted from component header file.
        //
        // Concerns:
        //: 1 The usage example provided in the component header file compiles,
        //:   links, and runs as shown.
        //
        // Plan:
        //: 1 Incorporate usage example from header into test driver, remove
        //:   leading comment characters, and replace 'assert' with 'ASSERT'.
        //:   (C-1)
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "USAGE EXAMPLE" << endl
                                  << "=============" << endl;

        if (verbose) cout << "\nTesting 'printPtr' function" << endl;
///Usage
///-----
// In this section we show intended usage of this component.
//
///Example 1: Using 'printPtr'
///- - - - - - - - - - - - - -
// The default output produced from pointer values is non-standard across
// vendor platforms.  The 'printPtr' method addresses this inconsistency by
// always producing a consistent format for a given pointer size:
//..
    const void *a = reinterpret_cast<void *>(0x0);
    const void *b = reinterpret_cast<void *>(0xf2ff);
    const void *c = reinterpret_cast<void *>(0x0123);
    const void *d = reinterpret_cast<void *>(0xf1f2abc9);

    bsl::ostringstream out1;

    bdlb::Print::printPtr(out1, a); out1 << endl;
    bdlb::Print::printPtr(out1, b); out1 << endl;
    bdlb::Print::printPtr(out1, c); out1 << endl;
    bdlb::Print::printPtr(out1, d); out1 << endl;

    ASSERT("0"        "\n"
           "f2ff"     "\n"
           "123"      "\n"
           "f1f2abc9" "\n" == out1.str());
//..
//
///Example 2: Using the Helper Classes
///- - - - - - - - - - - - - - - - - -
// The two helper classes allow users to stream a hexadecimal representation
// of a sequence of bytes into an output stream.
//
// The 'bdlb::PrintStringHexDumper' provides a formatted, possibly multi-line
// representation:
//..
    char buf[] = "abcdefghijklmnopqrstuvwxyz";

    bsl::ostringstream out2a;
    out2a << bdlb::PrintStringHexDumper(buf, sizeof buf);

    ASSERT(
       "     0:   61626364 65666768 696A6B6C 6D6E6F70     |abcdefghijklmnop|\n"
       "    16:   71727374 75767778 797A00                |qrstuvwxyz.     |\n"
        == out2a.str());

    bsl::ostringstream out2b;
    out2b << bdlb::PrintStringSingleLineHexDumper(buf, sizeof buf);
//..
// The 'bdlb::PrintStringSingleLineHexDumper' provides a simple, single-line
// representation.
//..
    ASSERT("6162636465666768696A6B6C6D6E6F707172737475767778797A00"
        == out2b.str());
//..
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
