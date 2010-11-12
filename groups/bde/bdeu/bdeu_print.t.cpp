// bdeu_print.t.cpp           -*-C++-*-

#include <bdeu_print.h>

#include <bsls_platform.h>
#include <bsls_platformutil.h>

#include <bsl_iomanip.h>
#include <bsl_iostream.h>
#include <bsl_sstream.h>
#include <bsl_string.h>
#include <bsl_strstream.h>

#include <bsl_cctype.h>      // isspace(), isprint()
#include <bsl_cstdio.h>      // sprintf()
#include <bsl_cstdlib.h>     // atoi()
#include <bsl_cstring.h>     // strcmp(), memset()

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                          *** Overview ***
//
// This component namespace for a set of pure procedures.  Each
// function is tested independently.
//
// [ 3] indent(ostream& s, int l, int spl = 4);
// [ 4] newlineAndIndent(ostream& s, int l, int spl = 4)
// [ 2] printPtr(ostream& stream, const void *value);
// [ 8] printString(ostream& s, const char *s, int l, bool ebs = 0);
// [ 5] hexDump(ostream& s, const char *b, int l);
// [ 7] hexDump(ostream& s, bsl::pair<const char *, int> *b, int nb)
// [ 9] singleLineHexDump(ostream& s, INPUT_ITER b, INPUT_ITER e);
// [ 9] singleLineHexDump(ostream& s, const char *b, const char *e);
// [ 9] singleLineHexDump(ostream& s, const char *b, int l);
// [ 6] struct bdeu_PrintStringHexDumper;
// [10] struct bdeu_PrintStringSingleLineHexDumper;
//--------------------------------------------------------------------------
// [ 1] USAGE EXAMPLE
//=============================================================================

//==========================================================================
//                  STANDARD BDE ASSERT TEST MACRO
//--------------------------------------------------------------------------
static int testStatus = 0;

static void aSsErT(int c, const char *s, int i)
{
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}

#define L_ __LINE__
#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

//=============================================================================
//                  STANDARD BDE LOOP-ASSERT TEST MACROS
//-----------------------------------------------------------------------------
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

//==========================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//--------------------------------------------------------------------------

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", " << flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number
#define T_ cout << "\t" << flush;             // Print a tab (w/o newline)

//==========================================================================
//              SUPPORTING FUNCTIONS USED FOR TESTING
//--------------------------------------------------------------------------
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

// The following table of size 256 holds the hex representation of each byte

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

int generateHexRepresentation(char *result, const char *input, int length)
    // Load into result a string that is the uppercase hex representation
    // of the input string of specified length.  Return the number of
    // characters written.  Note that we will use this function as an oracle.
{
    ASSERT(0 <= length);
    int ret = 0;
    for (int i = 0; i < length; ++i) {
        const bsl::string s = ASCII_HEX_PRESENTATION[(unsigned char)input[i]];
        result[2 * i + 0] = s[0];
        result[2 * i + 1] = s[1];
        ret += 2;
    }
    result[ret] = 0;
    ASSERT(ret == 2 * length);
    return ret;
}

void generatePseudoRandomInputBufferData(char *result, int length, int seed)
{
    static unsigned int value = seed;
    for (int i = 0; i < length; ++i) {
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

//=============================================================================
//                             MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
      case 10: {
        // --------------------------------------------------------------------
        // TESTING 'bdeu_PrintStringSingleLineHexDumper' CLASS
        //
        // Concerns:
        //   dumping via the class results in the same output as if done to the
        //   stream directly.
        //
        // Testing:
        //   ostream& operator<<(ostream&,
        //                       const bdeu_PrintStringSingleLineHexDumper&)
        // --------------------------------------------------------------------

        static const char SOME_STRING[] = "ABCDefghIJKLmnopQRSTuvwXYZ";
        static const size_t SOME_STRING_LEN = sizeof SOME_STRING - 1;

        bsl::ostringstream oss1;
        bsl::ostringstream oss2;

        oss1 << bdeu_PrintStringSingleLineHexDumper(SOME_STRING,
                                                    SOME_STRING_LEN);
        bdeu_Print::singleLineHexDump(oss2, SOME_STRING, SOME_STRING_LEN);

        ASSERT(0 == strncmp(oss1.str().c_str(),
                            oss2.str().c_str(),
                            oss1.str().size()));

      } break;
      case 9: {
        // --------------------------------------------------------------------
        // TESTING 'singleLineHexDump' METHODS
        //
        // Concerns:
        //   1) all ASCII character values (printable and non-printable) get
        //   encoded correctly;
        //   2) exactly 'length' characters get dumped.
        //
        // Plan:
        //   Build up a table of the hex representations of all 256 different
        //   byte patterns.  After some preliminary tests, mechanically
        //   create the expected output from the input and verify that that
        //   each of the functions performs the same way.
        //
        // Testing:
        //   singleLineHexDump(ostream& s, INPUT_ITER b, INPUT_ITER e);
        //   singleLineHexDump(ostream& s, const char *b, const char *e);
        //   singleLineHexDump(ostream& s, const char *b, int l);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting 'singleLineHexDuop Methods"
                          << "\n==================================" << endl;

        static const char LONG_STR[] =
            "jjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjj";
        static const size_t LONG_STR_LEN = sizeof LONG_STR - 1;

        if (verbose) bsl::cout
                << "\nConcern 1: Test dumping characters" << bsl::endl;

        for (size_t i = 0; i <= 255; ++i) {
            bsl::ostringstream oss;
            char c = static_cast<char>(i);

            bdeu_Print::singleLineHexDump(oss, &c, 1) << bsl::flush;

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

            bdeu_Print::singleLineHexDump(oss, LONG_STR, i) << bsl::flush;

            if (veryVerbose) {
                bsl::cout
                    << "i: " << i << ' '
                    << "length: " << oss.str().size() << bsl::endl;
            }

            LOOP_ASSERT(i, oss.str().size() == i*2);
        }

        if (verbose) bsl::cout
            << "\nNow test all 3 functions against two oracles." << bsl::endl;

        const size_t SIZE = 2050;
        char inputBuffer[SIZE];  // big enough to hold any of the following:
        int lengths[] = { 0, 1, 1, 1, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4, 5, 5, 5, 7,
                          7, 7, 8, 8, 8, 9, 9, 9, 15, 15, 16, 16, 17, 17, 31,
                         32, 33, 127, 128, 129, 255, 256, 257, 511, 512, 513 };

        char outBuf[2 * SIZE + 1]; // big enough to hold rep + null.

        const int N = sizeof lengths / sizeof *lengths;
        for (int i = 0; i < N; ++i) {
            int len = lengths[i];
            int seed = 19;

            generatePseudoRandomInputBufferData(inputBuffer, len, seed);
            int outLen = generateHexRepresentation(outBuf,  inputBuffer, len);

            if (veryVeryVerbose || veryVerbose && len < 50) {
                 cout << "     Input: "; printPrintable(inputBuffer, len);
                 cout << endl;
                 cout << "    Output: " << outBuf << endl;
            }

            const char *const INPUT_BUFFER = inputBuffer; // for non-template

            {
                bsl::ostringstream out1;
                bdeu_Print::singleLineHexDump<const char *> (out1, inputBuffer,
                                      inputBuffer + len) << '\0' << bsl::flush;
                LOOP2_ASSERT(i, len, outLen == strlen(out1.str().c_str()));
                LOOP2_ASSERT(i, len, 0 == strcmp(out1.str().c_str(), outBuf));
            }

            {
                bsl::ostringstream out2;
                bdeu_Print::singleLineHexDump(out2, INPUT_BUFFER,
                                     INPUT_BUFFER + len) << '\0' << bsl::flush;
                LOOP2_ASSERT(i, len, outLen == strlen(out2.str().c_str()));
                LOOP2_ASSERT(i, len, 0 == strcmp(out2.str().c_str(), outBuf));
            }

            {
                bsl::ostringstream out3;
                bdeu_Print::singleLineHexDump(out3, inputBuffer,
                                                    len) << '\0' << bsl::flush;
                LOOP2_ASSERT(i, len, outLen == strlen(out3.str().c_str()));
                LOOP2_ASSERT(i, len, 0 == strcmp(out3.str().c_str(), outBuf));
            }

            {
                bsl::ostringstream out4;
                ::originalSingleLineHexDump(out4, inputBuffer,
                                                    len) << '\0' << bsl::flush;
                LOOP2_ASSERT(i, len, outLen == strlen(out4.str().c_str()));
                LOOP2_ASSERT(i, len, 0 == strcmp(out4.str().c_str(), outBuf));
            }
        }

      } break;
      case 8: {
        // --------------------------------------------------------------------
        // TESTING 'printString' METHOD
        //   This will test the 'printString' method
        //
        // Concerns:
        //   1. Non-printable characters must be printed using their
        //      hexadecimal representation.
        //   2. Interleaved printable and non-printable characters must work
        //      as expected.
        //   3. Printing with bad streams should be a no-op.
        //   4. If the expandBackSlash is 'false' (default case) then '\' is
        //      not output as '\\', else it is.
        //
        // Plan:
        //   For a series of values print the test string to an ostream and
        //   verify that the result is as expected.
        //
        // Testing:
        //   printString(ostream& s, const char *s, int l, bool ebs = 0);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting vector<char> 'print' Method"
                          << "\n===================================" << endl;

        static const struct {
            int         d_lineNum;
            const char *d_spec;
            bool        d_expandSlash;
            const char *d_expectedResult;
        } DATA[] = {
            //line   spec             expand           expectedResult
            //----   ----             ------           --------------

            // Printable characters of increasing length interleaved with '\'
            { L_,    "",                  false,  ""               },
            { L_,    "",                  true,   ""               },
            { L_,    "\\",                false,  "\\"             },
            { L_,    "\\",                true,   "\\\\"           },
            { L_,    "a",                 false,  "a"              },
            { L_,    "a",                 true,   "a"              },
            { L_,    "A",                 false,  "A"              },
            { L_,    "A",                 true,   "A"              },
            { L_,    "a\\",               false,  "a\\"            },
            { L_,    "a\\",               true,   "a\\\\"          },
            { L_,    "aA",                false,  "aA"             },
            { L_,    "aA",                true,   "aA"             },
            { L_,    "a\\A",              false,  "a\\A"           },
            { L_,    "a\\A",              true,   "a\\\\A"         },
            { L_,    "abc\\D",            false,  "abc\\D"         },
            { L_,    "abc\\D",            true,   "abc\\\\D"       },
            { L_,    "aBcDe\\",           false,  "aBcDe\\"        },
            { L_,    "aBcDe\\",           true,   "aBcDe\\\\"      },

            // Hex conversion with interleaved '\'.  Note: 0x00 is tested
            // separately below.
            { L_,    "\x01",              false,  "\\x01"          },
            { L_,    "\x01",              true,   "\\x01"          },
            { L_,    "\x01\\",            false,  "\\x01\\"        },
            { L_,    "\x01\\",            true,   "\\x01\\\\"      },
            { L_,    "\x02",              false,  "\\x02"          },
            { L_,    "\x02",              true,   "\\x02"          },
            { L_,    "\x02\\",            false,  "\\x02\\"        },
            { L_,    "\x02\\",            true,   "\\x02\\\\"      },
            { L_,    "\n",                false,  "\\x0A"          },
            { L_,    "\n",                true,   "\\x0A"          },
            { L_,    "\n\\",              false,  "\\x0A\\"        },
            { L_,    "\n\\",              true,   "\\x0A\\\\"      },
            { L_,    "\\\r",              false,  "\\\\x0D"        },
            { L_,    "\\\r",              true,   "\\\\\\x0D"      },
            { L_,    "\x7F",              false,  "\\x7F"          },
            { L_,    "\x7F",              true,   "\\x7F"          },
            { L_,    "\x80",              false,  "\\x80"          },
            { L_,    "\x80",              true,   "\\x80"          },
            { L_,    "\x81",              false,  "\\x81"          },
            { L_,    "\x81",              true,   "\\x81"          },
            { L_,    "\xFE\\",            false,  "\\xFE\\"        },
            { L_,    "\xFE\\",            true,   "\\xFE\\\\"      },
            { L_,    "\\\xFF",            false,  "\\\\xFF"        },
            { L_,    "\\\xFF",            true,   "\\\\\\xFF"      },

            // Interleaved printable, non-printable and slash.
            { L_,    "a\rb\nc\\",         false,  "a\\x0Db\\x0Ac\\"       },
            { L_,    "a\rb\nc\\",         true,   "a\\x0Db\\x0Ac\\\\"     },
            { L_,    "\\aA\rbB\ncC",      false,  "\\aA\\x0DbB\\x0AcC"    },
            { L_,    "\\aA\rbB\ncC",      true,   "\\\\aA\\x0DbB\\x0AcC"  },
            { L_,    " \r\n",             false,  " \\x0D\\x0A"           },
            { L_,    " \r\n",             true,   " \\x0D\\x0A"           },
            { L_,    "\r\nx",             false,  "\\x0D\\x0Ax"           },
            { L_,    "\r\nx",             true,   "\\x0D\\x0Ax"           },
            { L_,    "\r\nz\\y",          false,  "\\x0D\\x0Az\\y"        },
            { L_,    "\r\nz\\y",          true,   "\\x0D\\x0Az\\\\y"      },
            { L_,    "a\r\nz\\y",         false,  "a\\x0D\\x0Az\\y"       },
            { L_,    "a\r\nz\\y",         true,   "a\\x0D\\x0Az\\\\y"     },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        if (verbose) cout << "\nTesting with valid stream." << endl;
        for (int i = 0; i < NUM_DATA; ++i) {
            const int   LINE             = DATA[i].d_lineNum;
            const char *SPEC             = DATA[i].d_spec;
            const bool  EXPAND           = DATA[i].d_expandSlash;
            const char *EXPECTED_RESULT  = DATA[i].d_expectedResult;
            const int   LEN              = bsl::strlen(SPEC);
            stringstream ss;

            ostream& ret = bdeu_Print::printString(ss,
                                                   SPEC,
                                                   LEN,
                                                   EXPAND);

            LOOP_ASSERT(LINE, &ss == &ret);
            LOOP3_ASSERT(LINE, EXPECTED_RESULT,   ss.str(),
                               EXPECTED_RESULT == ss.str());
        }

        if (verbose) cout << "\nTesting with invalid stream." << endl;
        for (int i = 0; i < NUM_DATA; ++i) {
            const int   LINE             = DATA[i].d_lineNum;
            const char *SPEC             = DATA[i].d_spec;
            const int   LEN              = bsl::strlen(SPEC);
            const bool  EXPAND           = DATA[i].d_expandSlash;

            stringstream ss;

            ss.setstate(ios_base::badbit);

            ostream& ret = bdeu_Print::printString(ss,
                                                   SPEC,
                                                   LEN,
                                                   EXPAND);

            LOOP_ASSERT(LINE, &ss == &ret);
            LOOP2_ASSERT(LINE, ss.str(),
                         "" == ss.str());
        }

        if (verbose) cout << "\nTesting null hex conversion." << endl;
        {
            const char SPEC[] = "\x00";
            const int  LEN    = bsl::strlen(SPEC);

            const char EXPECTED_RETURN[] = "";

            stringstream ss;

            ostream& ret = bdeu_Print::printString(ss, SPEC, LEN, false);

            ASSERT(&ss == &ret);
            LOOP2_ASSERT(ss.str(), EXPECTED_RETURN,
                         EXPECTED_RETURN == ss.str());
        }
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING HEXDUMP (Multiple buffers)
        //
        // Concerns:
        //   When multiple buffers are printed out, on the border where one
        //   buffer ends and a second buffer begins must have a seemless
        //   transition, one should not be able to tell where one buffer
        //   ends and the next begins.
        //
        //   a. Small buffers test.
        //      Test when a supplied buffer is smaller than CHAR_PER_LINE,
        //      resulting in multiple buffers printed out per line.
        //
        //   b. Variable sizes buffer test.
        //      Test when supplied buffers are not all the same size.
        //
        //   c. Buffer endpoint test.
        //      Test when buffer size end in the middle of a line, and when
        //      it ends at the end of a line.
        //
        //
        // Plan:
        //   a. Small buffers test.
        //      Create a number of small static buffers, such that for each
        //      line, multiple buffers will be used.  Pass these into the
        //      function and verify output is correct.
        //
        //   b. Variable sizes buffer test.
        //      Create a number of different sized static buffers.  Pass these
        //      into the function and verify output is correct.
        //
        //   c. Buffer endpoint test.
        //      Create a number of buffers of size (CHAR_PER_LINE * 1.5).  The
        //      end of the first buffer must end at the middle of the line,
        //      where the second buffer will be used to finish the line.
        //      The next buffer will end precisely at the end of the third
        //      line, so this will test buffers that end exactly at the
        //      end of a line as well.  Verify that output is correct.
        //
        // Testing:
        //    hexDump(ostream& s, bsl::pair<const char *, int> *b, int nb)
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting 'hexdump' (multiple buffers) Function."
                          << "\n=============================================="
                          << endl;

        {
            if (verbose) cout << "a.  Small buffers test."  << endl;

            enum { NUM_STATIC_BUFFERS = 30,
                   SIZE               = 1024 };

            bsl::pair<const char *, int> buffers[NUM_STATIC_BUFFERS];
            char                         staticBuffers[NUM_STATIC_BUFFERS][1];

            for (int i=0; i < NUM_STATIC_BUFFERS; i++) {
                staticBuffers[i][0] = 'a' + (i % 26);
                buffers[i]          = bsl::make_pair(&staticBuffers[i][0], 1);
            }

            char buf[SIZE];  bsl::strstream out(buf, SIZE);
            bdeu_Print::hexDump(out, buffers, NUM_STATIC_BUFFERS);
            ASSERT(0 == strncmp(buf, expectedOutCase7[0].c_str(),
                                expectedOutCase7[0].size()));

            if (veryVerbose) {
                bsl::string output(buf, out.pcount());
                bsl::cout << "Hexdumped String :\n"
                          << output
                          << "Expected String  :\n"
                          << expectedOutCase7[0] << bsl::endl;

            }
        }
        {
            if (verbose) cout << "b.  Variable sizes buffer test."  << endl;
            enum { NUM_STATIC_BUFFERS = 18,
                   SIZE               = 1024 };

            bsl::pair<const char *, int> buffers[NUM_STATIC_BUFFERS];
            char staticBuffers[NUM_STATIC_BUFFERS][NUM_STATIC_BUFFERS];

            for (int i=0; i < NUM_STATIC_BUFFERS; i++) {
                for (int j=0; j <=i; j ++ ) {
                  staticBuffers[i][j] = 'a' + (j % 26);
                }
                buffers[i] = bsl::make_pair(&staticBuffers[i][0], i + 1);
            }

            char buf[SIZE];  bsl::strstream out(buf, SIZE);
            bdeu_Print::hexDump(out, buffers, NUM_STATIC_BUFFERS);
            ASSERT(0 == strncmp(buf, expectedOutCase7[1].c_str(),
                                expectedOutCase7[1].size()));

            if (veryVerbose) {
                bsl::string output(buf, out.pcount());
                bsl::cout << "Hexdumped String :\n"
                          << output
                          << "Expected String  :\n"
                          << expectedOutCase7[1] << bsl::endl;

            }
        }
        {
            if (verbose) cout << "c.  Buffer endpoint test."  << endl;
            enum { NUM_STATIC_BUFFERS = 4 ,
                   CHAR_PER_LINE      = 16,  // As defined in bdeu_print.
                   SIZE               = 1024
            };

            bsl::pair<const char *, int> buffers[NUM_STATIC_BUFFERS];
            char staticBuffers[NUM_STATIC_BUFFERS][2 * CHAR_PER_LINE];

            for (int i=0; i < NUM_STATIC_BUFFERS; i++) {
                for (int j=0; j < CHAR_PER_LINE; j++ ) {
                  staticBuffers[i][j]             = 'a';
                  staticBuffers[i][CHAR_PER_LINE + j] = 'B';
                }
                buffers[i] = bsl::make_pair(&staticBuffers[i][0],
                                            CHAR_PER_LINE + CHAR_PER_LINE/2);
            }

            char buf[SIZE];  bsl::strstream out(buf, SIZE);
            bdeu_Print::hexDump(out, buffers, NUM_STATIC_BUFFERS);
            ASSERT(0 == strncmp(buf, expectedOutCase7[2].c_str(),
                                     expectedOutCase7[2].size()));

            if (veryVerbose) {
                bsl::string output(buf, out.pcount());
                bsl::cout << "Hexdumped String :\n"
                          << output
                          << "Expected String  :\n"
                          << expectedOutCase7[2] << bsl::endl;

            }
        }

      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING 'bdeu_PrintStringHexDumper':
        //
        // Concerns:
        //   That the output resulting from the use of this struct and its
        //   output operator tis the same as the corresponding dump method.
        //
        // Plan:
        //   Use the same test input and expected output as in case 5.
        //
        // Testing:
        //   struct bdeu_PrintStringHexDumper;
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting 'bdeu_PrintStringHexDumper'." << endl;
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

                char buf[SIZE];  bsl::strstream out(buf, SIZE);
                out << bdeu_PrintStringHexDumper(INPUT.c_str(),
                                                 INPUT.length());

                bsl::string OUTPUT(buf, out.pcount());

                if (verbose) { P(LINE)  P(INPUT)  P(EXPECTED)  P(OUTPUT) }

                LOOP_ASSERT(LINE, OUTPUT == EXPECTED);
            }
        }

      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING HEX DUMP
        //
        // Concerns:
        //   That the input buffer is formatted correctly.
        //
        // Plan:
        //   Use the format array at the top of this file to test inputs of
        //  varying length from 0 to 16.
        //
        // Testing:
        //   hexDump(ostream& s, const char *b, int l);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting 'hexDump'." << endl;
        {
            const int SIZE = 256;

            // Prepare input buffer.

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

                char buf[SIZE];  bsl::strstream out(buf, SIZE);
                bdeu_Print::hexDump(out, INPUT.c_str(), INPUT.length());

                bsl::string OUTPUT(buf, out.pcount());

                if (verbose) { P(LINE)  P(INPUT)  P(EXPECTED)  P(OUTPUT) }

                LOOP_ASSERT(LINE, OUTPUT == EXPECTED);
            }
        }

      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING NEWLINEANDINDENT METHOD
        //
        // Concerns:
        //   Ensure that the newlineAndIndent method prints properly for:
        //     - negative, 0, and positive levels.
        //     - negative, 0, and positive spaces per level.
        //
        // Plan:
        //   Specifying a set of test vectors and verify the return value.
        //
        // Testing:
        //   newlineAndIndent(ostream& s, int l, int spl = 4)
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "Testing 'newLineAndindent'" << endl
                                  << "==========================" << endl;

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

            const int SIZE = 128; // Must be big enough to hold output string.
            const char Z1 = (char) 0xff;  // Used to represent an unset char.
            const char Z2 = 0x00;  // Value 2 used to represent an unset char.

            char mCtrlBuf1[SIZE];  memset(mCtrlBuf1, Z1, SIZE);
            char mCtrlBuf2[SIZE];  memset(mCtrlBuf2, Z2, SIZE);
            const char *CTRL_BUF1 = mCtrlBuf1;
            const char *CTRL_BUF2 = mCtrlBuf2;

            for (int ti = 0; ti < NUM_DATA;  ++ti) {
                const int         LINE = DATA[ti].d_lineNum;
                const int         IND  = DATA[ti].d_indent;
                const int         SPL  = DATA[ti].d_spaces;
                const char *const FMT  = DATA[ti].d_fmt_p;

                char buf1[SIZE], buf2[SIZE];
                memcpy(buf1, CTRL_BUF1, SIZE); // Preset buf1 to Z1 values.
                memcpy(buf2, CTRL_BUF2, SIZE); // Preset buf2 to Z2 values.

                if (verbose) { P_(IND); P(SPL); }
                if (veryVerbose) {
                    cout << "EXPECTED FORMAT: '" << FMT << '\'' << endl;
                }
                ostrstream out1(buf1, SIZE);
                bdeu_Print::newlineAndIndent(out1, IND, SPL);  out1 << ends;
                ostrstream out2(buf2, SIZE);
                bdeu_Print::newlineAndIndent(out2, IND, SPL);  out2 << ends;
                if (veryVerbose) {
                    cout << "ACTUAL FORMAT:   '" << buf1 << '\'' << endl;
                }

                const int SZ = (int) strlen(FMT) + 1;
                const int REST = SIZE - SZ;
                LOOP_ASSERT(LINE, SZ < SIZE);  // Check buffer is large enough.
                LOOP_ASSERT(LINE, Z1 == buf1[SIZE - 1]);  // Check for overrun.
                LOOP_ASSERT(LINE, Z2 == buf2[SIZE - 1]);  // Check for overrun.
                LOOP_ASSERT(LINE, 0 == memcmp(buf1, FMT, SZ));
                LOOP_ASSERT(LINE, 0 == memcmp(buf2, FMT, SZ));
                LOOP_ASSERT(LINE,
                            0 == memcmp(buf1 + SZ, CTRL_BUF1 + SZ, REST));
                LOOP_ASSERT(LINE,
                            0 == memcmp(buf2 + SZ, CTRL_BUF2 + SZ, REST));
            }
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING INDENT METHOD
        //   Ensure that the indent method prints properly for:
        //     - negative, 0, and positive levels.
        //     - negative, 0, and positive spaces per level.
        //
        // Plan:
        //   Specifying a set of test vectors and verify the return value.
        //
        // Testing:
        //   indent(ostream& s, int l, int spl = 4);
        //   static ostream&
        //              indent(ostream& stream, int level, int spacesPerLevel);
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "Testing 'indent'" << endl
                                  << "================" << endl;

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

            const int SIZE = 128; // Must be big enough to hold output string.
            const char Z1 = (char) 0xff;  // Used to represent an unset char.
            const char Z2 = 0x00;  // Value 2 used to represent an unset char.

            char mCtrlBuf1[SIZE];  memset(mCtrlBuf1, Z1, SIZE);
            char mCtrlBuf2[SIZE];  memset(mCtrlBuf2, Z2, SIZE);
            const char *CTRL_BUF1 = mCtrlBuf1;
            const char *CTRL_BUF2 = mCtrlBuf2;

            for (int ti = 0; ti < NUM_DATA;  ++ti) {
                const int         LINE   = DATA[ti].d_lineNum;
                const int         IND    = DATA[ti].d_indent;
                const int         SPL    = DATA[ti].d_spaces;
                const char *const FMT    = DATA[ti].d_fmt_p;

                char buf1[SIZE], buf2[SIZE];
                memcpy(buf1, CTRL_BUF1, SIZE); // Preset buf1 to Z1 values.
                memcpy(buf2, CTRL_BUF2, SIZE); // Preset buf2 to Z2 values.

                if (verbose) { P_(IND); P(SPL); }
                if (veryVerbose) {
                    cout << "EXPECTED FORMAT: '" << FMT << '\'' << endl;
                }
                ostrstream out1(buf1, SIZE);
                bdeu_Print::indent(out1, IND, SPL);  out1 << ends;
                ostrstream out2(buf2, SIZE);
                bdeu_Print::indent(out2, IND, SPL);  out2 << ends;
                if (veryVerbose) {
                    cout << "ACTUAL FORMAT:   '" << buf1 << '\'' << endl;
                }

                const int SZ = (int) strlen(FMT) + 1;
                const int REST = SIZE - SZ;
                LOOP_ASSERT(LINE, SZ < SIZE);  // Check buffer is large enough.
                LOOP_ASSERT(LINE, Z1 == buf1[SIZE - 1]);  // Check for overrun.
                LOOP_ASSERT(LINE, Z2 == buf2[SIZE - 1]);  // Check for overrun.
                LOOP_ASSERT(LINE, 0 == memcmp(buf1, FMT, SZ));
                LOOP_ASSERT(LINE, 0 == memcmp(buf2, FMT, SZ));
                LOOP_ASSERT(LINE,
                            0 == memcmp(buf1 + SZ, CTRL_BUF1 + SZ, REST));
                LOOP_ASSERT(LINE,
                            0 == memcmp(buf2 + SZ, CTRL_BUF2 + SZ, REST));
            }
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING PRINT POINTER FUNCTION
        //
        // Concerns:
        //   1.   Leading zeros are suppressed.
        //   2    Embedded zeros are displayed.
        //   3.   Values don't get confused for negative numbers.
        //   4.   Case is always lower case for a, b, c, d, e, and f"
        //   5.   Upper and lower 32-bits treated the same.
        //
        // Plan:
        //   Create a table consisting of high an low 32-bit address values
        //   and the expected print value.  Depending on the architecture
        //   test all the addresses or just those with the upper 32-bit
        //   values 0.
        //
        // Testing:
        //   printPtr(ostream& stream, const void *value);
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "Test 'printPtr' function." << endl
                                  << "=========================" << endl;

        if (verbose) cout << "\nTesting printPtr function" << endl;
        static const struct {
                unsigned int addr1;             // MS 32 bits
                unsigned int addr2;             // LS 32 bits
                const char * str;
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
            char buf[100];  memset(buf, 0xff, sizeof buf);  // Scribble on buf.
            ostrstream out(buf, sizeof buf);

            // test if 64-bit pointers or if value has only 32 bits
            #if defined(BSLS_PLATFORM__CPU_64_BIT)
            {
                void *p = (void *) ((((bsls_PlatformUtil::Int64)
                                        DATA[t].addr1) << 32) + DATA[t].addr2);
                bdeu_Print::printPtr(out, p);
                out << ends;

                if (veryVerbose) {
                    cout << "EXPECTED: " << DATA[t].str;
                    cout << "  ACTUAL: " << buf;
                    cout << endl;
                }

                ASSERT(strcmp(buf, DATA[t].str) == 0);
            }
            #else  // BSLS_PLATFORM__CPU_32_BIT
            if (0 == DATA[t].addr1) {
                bdeu_Print::printPtr(out, (void *) DATA[t].addr2);
                out << ends;

                if (veryVerbose) {
                    cout << "EXPECTED: " << DATA[t].str;
                    cout << "  ACTUAL: " << buf;
                    cout << endl;
                }

                ASSERT(strcmp(buf, DATA[t].str) == 0);
            }
            #endif
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //
        // Concerns:
        //   1. That the usage example works as expected.
        //
        // Plan:
        //   1. Create a usage example here and copy it into the header.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "USAGE EXAMPLE" << endl
                                  << "=============" << endl;

        if (verbose) cout << "\nTesting printPtr function" << endl;
        // USAGE
        const void *a = (void *) 0x0;
        const void *b = (void *) 0xf2ff;
        const void *c = (void *) 0x0123;
        const void *d = (void *) 0xf1f2abc9;

        char buf[100];  memset(buf, 0xff, sizeof buf);  // Scribble on buf.
        ostrstream out(buf, sizeof buf);

        const char * EXPECTED = "0\nf2ff\n123\nf1f2abc9\n";

        bdeu_Print::printPtr(out, a);
        out << endl;
        bdeu_Print::printPtr(out, b);
        out << endl;
        bdeu_Print::printPtr(out, c);
        out << endl;
        bdeu_Print::printPtr(out, d);
        out << endl;
        out << ends;

        if (veryVerbose) {
            cout << "EXPECTED: " << EXPECTED;
            cout << "  ACTUAL: " << buf;
        }

        ASSERT(strcmp (buf, EXPECTED) == 0);

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

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2009
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
