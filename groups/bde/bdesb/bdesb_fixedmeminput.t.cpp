// bdesb_fixedmeminput.t.cpp             -*-C++-*-

#include <bdesb_fixedmeminput.h>

#include <bsl_iostream.h>
#include <bsl_iomanip.h>
#include <bsl_strstream.h>
#include <bsl_string.h>

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

//=============================================================================
//                              TEST PLAN
//-----------------------------------------------------------------------------
//                          *** Overview ***
//
// This test driver exercises all the public methods from the
// 'basic_streambuf' protocol that are implemented by the class
// 'bdesb_FixedMemInput', as well as each public method in the
// 'bdesb_FixedMemInput' class that is not part of the 'basic_streambuf'
// protocol.
//
// Our goal here is to ensure that the implementations comply exactly with the
// IOStreams portion of the C++ standard where the standard explicitly defines
// behavior, and that they conform to a feasible interpretation of the standard
// as described in the function documentation, where the standard defined
// behavior only loosely.  For those methods that are not protocol-defined, we
// check only compliance with the behavior as described in the function
// documentation.
//
//       Primary Constructors, Primary Manipulators, and Basic Accessors
//       ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Primary Constructors:
//   A 'bdesb_FixedMemInput' is created with a client-supplied C char
//   array (which can be changed during the lifetime of an object), and a
//   length specification.  This is the only constructor, so our set of
//   primary constructors will be:
//
//    o bdesb_FixedMemInput(char *buffer, bsl::streamsize length);
//
// Primary Manipulators:
//   We can bring a 'bdesb_FixedMemInput' to any achievable white-box
//   state by using a combination of 'pubseekpos', which allows us to
//   reposition the "cursor" (i.e., the position that the next read operation
//   will input from) anywhere in the stream buffer, and 'sbumpc', which
//   removes a single character from the stream buffer.
//
//    o int_type sgetc(char_type);
//    o pos_type pubseekpos(pos_type, ios_base::openmode);
//
//
// Basic Accessors:
//   We would like to find the largest set of *direct* accessors that can be
//   used generally to report back on the state of the object.  The
//   'bdesb_MemInStreamBuf' component has only 'length' and 'data' as
//   accessors, and so they form our accessor set:
//
//    o const char_type *data();
//    o streamsize length();
//-----------------------------------------------------------------------------
// CREATORS
// [ 3] bdesb_FixedMemInput(char *, streamsize);
//
// MANIPULATORS
// [ 4] seekpos(pos_type, openmode);
// [ 6] setbuf(char *, streamsize);
// [ 6] setbuf(const char *, streamsize);
// [ 6] pubsetbuf(char *, streamsize);
// [ 6] pubsetbuf(const char *, streamsize);
// [ 7] streamsize xsgetn(char_type, streamsize);
// [ 8] pbackfail(int_type);
// [ 9] seekoff(off_type, seekdir, openmode);
//
// ACCESSORS
// [10] showmanyc();
// [ 4] data();
// [ 4] length();
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 2] TEST APPARATUS
// [ 5] ostream& operator<<(ostream&, const bdesb_FixedMemInput&);
//-----------------------------------------------------------------------------

//==========================================================================
//                  STANDARD BDE ASSERT TEST MACRO
//--------------------------------------------------------------------------
static int testStatus = 0;

static void aSsErT(int c, const char *s, int i) {
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}

# define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }
//--------------------------------------------------------------------------
#define LOOP_ASSERT(I,X) { \
    if (!(X)) { cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__); } }

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

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", " << flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number
#define T_ cout << "\t" << flush;             // Print a tab (w/o newline)

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------
const int ARBITRARY_INITIAL_BUFSIZE = 20;

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

               // ==================================
               // operator<< for bdesb_FixedMemInput
               // ==================================

// FREE OPERATORS
bsl::ostream& operator<<(bsl::ostream&              stream,
                         const bdesb_FixedMemInput& streamBuffer);
    // Write the contents of the specified 'streamBuffer' (as well as a marker
    // indicating eight bytes groupings) to the specified output 'stream' in
    // binary format, and return a reference to the modifiable 'stream'.

bsl::ostream& operator<<(bsl::ostream&              stream,
                         const bdesb_FixedMemInput& streamBuffer)
{
    const int   len  = streamBuffer.length();
    const char *data = streamBuffer.data();

    bsl::ios::fmtflags flags = stream.flags();
    stream << bsl::hex;
    for (int i = 0; i < len; ++i) {
        if (0 < i && 0 != i % 8) stream << ' ';

        if (0 == i % 8) { // print new line and address after 8 bytes
            stream << '\n' << bsl::setw(4) << bsl::setfill('0') << i << '\t';
        }
        for (int j = 7; j >= 0; --j) stream << !!((data[i] >> j) & 0x01);
    }
    stream.flags(flags); // reset stream format flags
    return stream;
}

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char **argv)
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    // int veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
      case 10: {
        // --------------------------------------------------------------------
        // SHOWMANYC
        //
        // Concerns:
        //   - That 'showmanyc' return the correct number of items left
        //     to consume in the streambuf.
        //
        // Plan:
        //   Use a depth-ordered enumeration to simulate categories 0, 1,
        //   and >-1 for category partitioning of stream buffer capacity, in
        //   cross-product with remaining-characters categories 0, [1..
        //   stream buffer capacity -1], and [entire capacity].
        //
        // Tactics:
        //   - Category-Partitioning Data Selection Method
        //   - Table-Based Implementation Technique
        //
        // Testing:
        //   showmanyc();
        // --------------------------------------------------------------------

        const char *msg1 = "Testing when entire capacity remains.";
        const char *msg2 = "Testing when entire capacity minus one remains.";
        const char *msg3 = "Testing when at most 1 character remains.";
        const char *msg4 = "Testing when 0 characters remain.";
        const char *msgs[4] = {msg1, msg2, msg3, msg4};

        if (verbose) cout << endl
                          << "SHOWMANYC TEST" << endl
                          << "==============" << endl;

        {
            const int ENUM_DEPTH_LIMIT = 5;
            char buffer[ENUM_DEPTH_LIMIT];
            for (int i = 0; i < ENUM_DEPTH_LIMIT; ++i) {

                if (veryVerbose) {
                  cout <<
          "\nTesting 'inavail' ('showmanyc') for streambufs of initial length "
                       << i << '.' << endl;
                }

                // Test for entire capacity and entire-minus-one left
                for (int k = 0; k <= 1; ++k) {
                    for(int j = 0; j < i; ++j) {
                        buffer[j] = 'a' + (j %26);
                    }
                    bdesb_FixedMemInput mSB(buffer, i);

                    if (veryVerbose) { cout << "\t" << msgs[k] << endl; }
                    const int WAY_BIG_ENOUGH = 100;
                    char temp[WAY_BIG_ENOUGH];
                    mSB.sgetn(temp, k);
                    bsl::streamsize ret = mSB.in_avail();
                    if (i - k == 0) {
                        LOOP2_ASSERT(i, k, -1 == ret);
                    }
                    else {
                        LOOP2_ASSERT(i, k, i - k == ret);
                    }
                }

                // Test for 1 char left and 0 chars left
                if (veryVerbose) cout << endl;

                for (int k = 1; k >= 0; --k) {
                    for(int j = 0; j < i; ++j) {
                        buffer[j] = 'a' + (j %26);
                    }
                    bdesb_FixedMemInput mSB(buffer, i);

                    if (veryVerbose) { cout << "\t" << msgs[3 - k] << endl; }
                    const int WAY_BIG_ENOUGH = 100;
                    char temp[WAY_BIG_ENOUGH];
                    if (i - k >= 0) {
                        mSB.sgetn(temp, i - k);
                        bsl::streamsize ret = mSB.in_avail();
                        if ( k == 0) {
                             LOOP2_ASSERT(i, k, -1 == ret);
                        }
                        else {
                            LOOP2_ASSERT(i, k, k == ret);
                        }
                    }
                }

            }
        }
      } break;
      case 9: {
        // --------------------------------------------------------------------
        // SEEK TEST
        //
        // Concerns:
        //   - that seeking relative to a specification ('seekoff') uses
        //     the correct location from which to offset
        //   - that both negative and positive offsets compute correctly
        //   - that seeking positions the "cursor" (i.e., pptr()) at the
        //     correct location
        //   - that seeking out of bounds does not throw or abort or crash
        //     the program
        //   - that trying to seek in the "get" area has no effect
        //
        // Plan:
        //   Perform a variety of seeks, using representative test vectors from
        //   the cross-product of offset categories beginning-pointer, current-
        //   pointer and end-pointer, with direction categories negative-
        //   forcing-past-beginning, negative-falling-within-bounds, 0,
        //   positive-falling-within bounds, and positive-forcing-past-end.
        //
        // Tactics:
        //   - Category-Partitioning Data Selection Method
        //   - Table-Based Implementation Technique
        //
        // Testing:
        //   pos_type seekoff(off_type, seekdir, openmode);
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "SEEKOFF TESTS" << endl
                          << "=============" << endl;

        if (verbose) cout << "\nTesting seekoff from beginning and end."
                          << endl;

        const int OUT = bsl::ios_base::out;
        const int IN  = bsl::ios_base::in;
        const int CUR = bsl::ios_base::cur;
        const int BEG = bsl::ios_base::beg;
        const int END = bsl::ios_base::end;
        char mFILL[ARBITRARY_INITIAL_BUFSIZE];
        memset(mFILL, 'a', sizeof mFILL);

        {
            static const struct {
                int         d_line;          // line number
                int         d_areaFlags;     // "put" area or "get" area
                bdesb_FixedMemInput::pos_type
                            d_amount;        // amount to seek
                int         d_base;          // seekoff from where?
                int         d_retVal;
            } DATA[] = {
               //L#  area                      final
               //    flag   amount     base    position/ retVal
               //--  ----   -------    ----    ----------------
               // seekoff from the start of the streambuf
               { L_,  IN,    -2,      BEG,      -1,     },
               { L_,  IN,     0,      BEG,       0      },
               { L_,  IN,   ARBITRARY_INITIAL_BUFSIZE - 1,
                                      BEG,
                                    ARBITRARY_INITIAL_BUFSIZE - 1 },
               { L_,  IN,   500,      BEG,      -1  },

               // seekoff in the "put" area
               { L_,  OUT,   22,      BEG,      -1  },

               // seekoff from the end of the streambuf.
               { L_,  IN,  -300,      END,      -1  },
               { L_,  IN,  -ARBITRARY_INITIAL_BUFSIZE,
                                      END,       0  },  // excessive but good
               { L_,  IN,  -30,       END,      -1 },
               { L_,  IN,    0,       END,     ARBITRARY_INITIAL_BUFSIZE },
               { L_,  IN,    1,       END,      -1 },

               // seekoff in the "put" area
               { L_,  OUT,    22,     END,      -1  },

               // seekoff from the current cursor, where cur == begin
               { L_,  IN,  -300,      CUR,      -1 },
               { L_,  IN,  -30,       CUR,      -1  },
               { L_,  IN,    0,       CUR,       0   },
               { L_,  IN,    1,       CUR,       1   },

               // seekoff in the "put" area
               { L_,  OUT,    22,       CUR,      -1  }
            };
            const int DATA_LEN = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < DATA_LEN; ++i ) {
                const int LINE      = DATA[i].d_line;
                const int FINAL_POS = (0 < DATA[i].d_retVal ?
                                       DATA[i].d_retVal : 0);

                char buffer[ARBITRARY_INITIAL_BUFSIZE];
                for(int j = 0; j < ARBITRARY_INITIAL_BUFSIZE; ++j) {
                    buffer[j] = 'a' + (j %26);
                }
                bdesb_FixedMemInput mSB(buffer,
                                              ARBITRARY_INITIAL_BUFSIZE);

                bdesb_FixedMemInput::int_type ret;
                ret = mSB.pubseekoff(DATA[i].d_amount,
                                (bsl::ios_base::seekdir) DATA[i].d_base,
                                (bsl::ios_base::openmode) DATA[i].d_areaFlags);
                LOOP_ASSERT(LINE, DATA[i].d_retVal == ret);
                if (veryVerbose) { T_ P(ret) };

                // Verify positioning by reading one char, and check the
                // char (except for out-of-bounds positions).
                int expectedChar = (FINAL_POS < ARBITRARY_INITIAL_BUFSIZE ?
                                    'a' + FINAL_POS % 26 : -1);
                if (veryVerbose) {
                    cout << "\tAt position " << FINAL_POS << " char is: "
                    << (char)mSB.sgetc() << endl;
                }
                LOOP_ASSERT(LINE, mSB.sgetc() == expectedChar);
            }
        }

        if (verbose) cout <<
              "\nTesting seekoff from a variety of current-pointer positions."
                          << endl;
        {
            const int MIDPOINT = ARBITRARY_INITIAL_BUFSIZE / 2;
            static const struct {
                int         d_line;             // line number
                int         d_offset;           // seek offset
                int         d_initialPosition;  // where to set pptr() first
                int         d_retVal;           // return from pubseekoff()
            } DATA[] = {

               //L#  offset   startPoint                  retVal
               //--  ------   ----------                  ------
               { L_,  -1,     ARBITRARY_INITIAL_BUFSIZE,
                                             ARBITRARY_INITIAL_BUFSIZE - 1 },
               { L_,   0,     ARBITRARY_INITIAL_BUFSIZE,
                                             ARBITRARY_INITIAL_BUFSIZE  },
               { L_,  500,    ARBITRARY_INITIAL_BUFSIZE,  -1           },
               { L_,  -110,   ARBITRARY_INITIAL_BUFSIZE,  -1           },
               { L_,  -20,    MIDPOINT,                   -1           },
               { L_,  -5,     MIDPOINT,                   MIDPOINT - 5 },
               { L_,   0,     MIDPOINT,                   MIDPOINT     },
               { L_,   5,     MIDPOINT,                   MIDPOINT + 5 },
               { L_,   31,    MIDPOINT,                   -1           },
               { L_,   ARBITRARY_INITIAL_BUFSIZE, MIDPOINT, -1 }
            };
            const int DATA_LEN = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < DATA_LEN; ++i ) {
                const int LINE      = DATA[i].d_line;
                const int FINAL_POS = (0 < DATA[i].d_retVal ?
                                       DATA[i].d_retVal :
                                       DATA[i].d_initialPosition);

                char buffer[ARBITRARY_INITIAL_BUFSIZE];
                for(int j = 0; j < ARBITRARY_INITIAL_BUFSIZE; ++j) {
                    buffer[j] = 'a' + (j %26);
                }
                bdesb_FixedMemInput mSB(buffer,
                                              ARBITRARY_INITIAL_BUFSIZE);

                bdesb_FixedMemInput::int_type ret;

                mSB.pubseekpos(DATA[i].d_initialPosition);
                ret = mSB.pubseekoff(DATA[i].d_offset,
                                     (bsl::ios_base::seekdir) CUR,
                                     (bsl::ios_base::openmode)IN);
                if (veryVerbose) { T_ P(ret) };

                LOOP_ASSERT(LINE, DATA[i].d_retVal == ret);

                // Verify positioning by reading one char, and check the
                // char (except for out-of-bounds positions).
                int expectedChar = (FINAL_POS < ARBITRARY_INITIAL_BUFSIZE ?
                                    'a' + FINAL_POS % 26 : -1);
                if (veryVerbose) {
                    cout << "\tAt position " << FINAL_POS << " char is: "
                    << (char)mSB.sgetc() << endl;
                }
                LOOP_ASSERT(LINE, mSB.sgetc() == expectedChar);
            }
          }
      } break;

      case 8: {
        // --------------------------------------------------------------------
        // PBACKFAIL
        //
        // Concerns:
        //   - Ensure that no character can be written into the buffer.
        //
        // Plan:
        //   Invoke the single public function that can call 'pbackfail'
        //   without asserting, in the circumstances under which it would
        //   do so, and ensure that the content of the streambuf is the same
        //   after the call as before the call.
        //
        // Tactics:
        //   - Ad-hoc Data Selection Method
        //   - Brute-Force Implementation Technique
        //
        // Testing:
        //   pbackfail(int_type);
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "PBACKFAIL" << endl
                          << "=========" << endl;

        {
            if (verbose) cout << endl
                 << "Testing 'pbackfail' triggered by 'sungetc'." << endl;

            static const int sampleSizes[] = { 0, 1 };

            for (int j = 0; j < 2; ++j) {

                if (verbose) cout << endl;

                const int BUF_LEN = sampleSizes[j];
                char *buffer = new char[BUF_LEN];
                for (int i = 0; i < BUF_LEN; ++i) {
                    buffer[i] = 'a' + (i % 26);
                }

                bdesb_FixedMemInput mSB(buffer, BUF_LEN);
                const bdesb_FixedMemInput& SB = mSB;

                LOOP_ASSERT(j, BUF_LEN == SB.length());

                bdesb_FixedMemInput::int_type ret;
                ret = mSB.sungetc();
                LOOP_ASSERT(j,
                      bdesb_FixedMemInput::traits_type::eof() == ret);
                if (veryVerbose) {
                    cout << "pbackfail (via sungetc) returned " <<
                            ret << endl;
                }

                delete [] buffer;
            }
        }

        {
            if (verbose) cout << endl <<
  "Cannot test 'pbackfail' triggered by 'sputbackc' because such use asserts."
                              << endl;
        }
        } break;

      case 7: {
        // --------------------------------------------------------------------
        // XSGETN TEST
        //
        // Concerns:
        //   - that 'xsgetn' retrieve the correct bytes from the streambuf.
        //   - that 'xsgetn' properly handle requests for 0, 1, and >_1
        //     characters, from streambufs of length 0, 1, and >_1.
        //   - that 'xsgetn' properly handle requests for more characters
        //     than contained in the streambuf.
        //
        // Plan:
        //   Create a table whose vectors test the cross-product of the
        //   categories indicated above.
        //
        // Tactics:
        //   - Category-Partitioning and Ad-hoc Data Selection Method
        //   - Table-Based and Brute-Force Implementation Technique
        //
        // Testing:
        //   streamsize xsgetn(char_type, streamsize);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "GET MULTIPLE CHARACTERS TEST" << endl
                          << "============================" << endl;

            static const struct {
                int         d_line;             // line number
                int         d_request;          // num bytes to retrieve
                int         d_capacity;         // length of streambuf
                int         d_return;           // expected return val
            } DATA[] = {
              //       chars to   stream   expected
              //L#     retrieve   length   return
               //--    --------   -------  --------
              { L_,       0,         0,       0    },
              { L_,       0,         1,       0    },
              { L_,       0,         30,      0    },

              { L_,       1,         0,       0    },
              { L_,       1,         1,       1    },
              { L_,       1,         30,      1    },

              { L_,       30,        0,       0    },
              { L_,       30,        1,       1    },
              { L_,       30,        30,      30   }
            };

        const int DATA_LEN = sizeof DATA / sizeof *DATA;

        {
            if (verbose) cout <<
                          "\nTesting 'sgetn' right after streambuf creation."
                              << endl;

            for (int i = 0; i < DATA_LEN; ++i) {
                const int LINE      = DATA[i].d_line;

                char *buffer = new char[DATA[i].d_capacity];
                for (int j = 0; j < DATA[i].d_capacity; ++j) {
                    buffer[j] = 'a' + (j %26);
                }
                bdesb_FixedMemInput mSB(buffer, DATA[i].d_capacity);
                const bdesb_FixedMemInput& SB = mSB;

                char *temp = new char[DATA[i].d_request];
                bsl::streamsize ret = mSB.sgetn(temp, DATA[i].d_request);
                LOOP_ASSERT(LINE, DATA[i].d_return == ret);
                LOOP_ASSERT(LINE, 0 == strncmp(temp, SB.data(), ret));
                if (veryVerbose) { cout << "\tRequested " << DATA[i].d_request
                        << " chars from a streambuf of length " <<
                        DATA[i].d_capacity << endl;
                }

                delete [] temp;
                delete [] buffer;

            }
        }
        {
            if (verbose) cout <<
                          "\nTesting successive 'sgetn' from same streambuf"
                              << endl;

            const int BUF_LEN = 100;
            char buffer[BUF_LEN];
            for(int k = 0; k < 100; ++k) {
                buffer[k] = 'a' + (k %26);
            }
            bdesb_FixedMemInput mSB(buffer, BUF_LEN);

            char a[2], b[3], c[4], d[5], e[6], f[7], g[8], h[9], i[10], j[11];
            a[1] = b[2] = c[3] = d[4] = e[5] = f[6] = g[7] = h[8] = i[9] =
                  j[10] = 'X';
            char *resultBuf[10] = { a, b, c, d, e, f, g, h, i, j };
            int result;
            const char *nextCompare = buffer;
            for(int j = 1; j < 10; ++j) {
                result = mSB.sgetn(resultBuf[j - 1], j);
                LOOP_ASSERT(j, j == result);
                LOOP_ASSERT(j, 0 == memcmp(nextCompare, resultBuf[j - 1], j));
                LOOP_ASSERT(j, 'X' == resultBuf[j - 1][j]);
                if (veryVerbose) { cout << "\tPicked up " << j
                    << " chars from the same streambuf." << endl;
                }
                nextCompare += j;
            }
        }
      } break;

      case 6: {
        // --------------------------------------------------------------------
        // SETBUF TEST
        //
        //   Ensure that 'pubsetbuf' sets up the new character buffer so that
        //   the stream buffer uses it correctly.
        //
        // Concerns:
        //   - That setbuf sets up all streambuf machinery properly so that
        //     streambuf operations use the client-supplied buffer.
        //   - That the streambuf is capable of using the entirety of the
        //     available storage, i.e., the full amount specified by the
        //     client.
        //   - That 'const' C arrays work as do the non-'const'.
        //
        // Plan:
        //   Use the base-class-implemented method 'sbumpc' to successively
        //   consume all characters from a 0-, 1-, and arbitrary_size_>_1-
        //   length bdesb_FixedMemInput, and ensure that all characters
        //   are retrieved.
        //
        //   Re-run the same suite for 'const' C arrays.  Three tests are
        //   overkill given white-box knowledge of the implementation, but it
        //   is cheap enough that the simplicity of duplication drives this
        //   choice.
        //
        // Tactics:
        //   - Category-Partitioning Data Selection Method
        //   - Loop-Based Implementation Techniques
        //
        // Testing:
        //   setbuf(char *, streamsize);
        //   setbuf(const char *, streamsize);
        //   pubsetbuf(char *, streamsize);
        //   pubsetbuf(const char *, streamsize);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "SETBUF" << endl
                          << "======" << endl;
        {
            static const int sampleSizes[] = { 0, 1, 5 };

            for (int j = 0; j < 3; ++j) {

                if (verbose) cout <<
                    "Testing pub/setbuf using non-const C arrays of length "
                                  << sampleSizes[j] << '.' << endl;

                const int BUF_LEN = sampleSizes[j];
                char *buffer = new char[BUF_LEN];
                for (int i = 0; i < BUF_LEN; ++i) {
                    buffer[i] = 'a' + (i % 26);
                }

                char fakeBuffer[1];
                bdesb_FixedMemInput mSB(fakeBuffer, 1);
                const bdesb_FixedMemInput& SB = mSB;

                mSB.pubsetbuf(buffer, BUF_LEN);
                ASSERT(BUF_LEN == SB.length());

                bdesb_FixedMemInput::int_type ret;
                for (int i = 0; i < BUF_LEN; ++i) {
                    ret = mSB.sbumpc();
                    if (veryVerbose) {T_ P((char)ret)}
                }
                delete [] buffer;
            }
        }

        {
            static const int sampleSizes[] = { 0, 1, 5 };

            for (int j = 0; j < 3; ++j) {
                if (verbose) cout <<
                    "Testing pub/setbuf using const C arrays of length "
                                  << sampleSizes[j] << '.' << endl;

                const int BUF_LEN = sampleSizes[j];
                const char *buffer = new char[BUF_LEN];
                char *hack = const_cast<char *>(buffer);
                for (int i = 0; i < BUF_LEN; ++i) {
                    hack[i] = 'a' + (i % 26);
                }

                char fakeBuffer[1];
                bdesb_FixedMemInput mSB(fakeBuffer, 1);
                const bdesb_FixedMemInput& SB = mSB;

                mSB.pubsetbuf(buffer, BUF_LEN);
                ASSERT(BUF_LEN == SB.length());

                bdesb_FixedMemInput::int_type ret;
                for (int i = 0; i < BUF_LEN; ++i) {
                    ret = mSB.sbumpc();
                    if (veryVerbose) {T_ P((char)ret)}
                }
                delete [] buffer;
            }
        }
      } break;


      case 5: {
        // --------------------------------------------------------------------
        // OUTPUT: bsl::ostream& 'operator<<'
        //
        // NOTE: 'op<<' is NOT part of the component.  It is, however, an
        // auxiliary function used in the test driver, and so we include the
        // 'traditional' test case 5 here in order to test the operator.
        //
        // Concerns:
        //   - that value is formatted correctly (i.e., as binary)
        //   - that no additional characters are written after terminating.
        //   - that these functions work on references to 'const' objects.
        //   - that each return a reference to the modifiable stream argument.
        //
        // Plan:
        //   For each of a small representative set of object values use
        //   'ostrstream' to write that object's value to two separate
        //   character buffers each with different initial values.  Compare
        //   the contents of these buffers with the literal expected output
        //   format and verify that the characters beyond the length of the
        //   streambuf contents are unaffected in both buffers.
        //   - Test operator<< on the empty streambuf.
        //   - Test operator<< on a streambuf containing some characters.
        //
        // Tactics:
        //   - Ad-Hoc Data Selection Method
        //
        // Testing:
        //   ostream& operator<<(ostream&, const bdesb_MemOutStreamRaw&);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                  << "OUTPUT bsl::ostream& 'operator<<'" << endl
                  << "=================================" << endl;

        if (verbose) cout << "\nCreate stream buffers to be printed." << endl;

        {
            if (verbose) cout << "\tChecking operator<< return value."
                              << endl;

            const int BUF_LEN = 10;
            char buffer[BUF_LEN];
            for(int j = 0; j < BUF_LEN; ++j) {
                buffer[j] = 'a' + (j %26);
            }
            const bdesb_FixedMemInput SB(buffer, BUF_LEN);

            const int SIZE = 100;
            char buf1[SIZE], buf2[SIZE];
            ostrstream out1(buf1, SIZE), out2(buf2, SIZE);
            out1 << SB << "arbitrary";  // Ensure modifiable
            out2 << SB << "value";      // stream is returned.
        }

        {
            if (verbose) cout << "\tChecking operator<<-generated content."
                              << endl;

            const int BUF_LEN = 10;
            char buffer[BUF_LEN];
            for(int j = 0; j < BUF_LEN; ++j) {
                buffer[j] = 'a' + (j %26);
            }
            bdesb_FixedMemInput mSB(buffer, BUF_LEN);
            const bdesb_FixedMemInput& SB = mSB;

            const int SIZE = 200;
            char buf1[SIZE], buf2[SIZE];
            memset(buf1, 'X', SIZE);
            memset(buf2, 'Y', SIZE);
            ostrstream out1(buf1, SIZE), out2(buf2, SIZE);
            out1 << mSB;
            out2 << SB;
            char *endOfString = strchr(buf1, 'X');
            int stringLen = endOfString - buf1;
            ASSERT(0 == memcmp(buf1, buf2, stringLen));
            ASSERT('X' == buf1[stringLen]);
            ASSERT('Y' == buf2[stringLen]);

            const char *EXPECTED =
                     "\n0000\t01100001 01100010 01100011 01100100 01100101 "
                     "01100110 01100111 01101000"
                     "\n0008\t01101001 01101010";
            ASSERT(0 == memcmp(buf1, EXPECTED, stringLen));
            buf1[stringLen] = 0;
            if (verbose) {
                T_ P(buf1)
            }
        }
      } break;

      case 4: {
        // --------------------------------------------------------------------
        // PRIMARY MANIPULATORS/BASIC ACCESSORS
        //   Verify the most basic functionality of a
        //   'bdesb_FixedMemInput'.
        //
        // Concerns:
        //   We note that 'sbumpc' is one of our primary manipulators, but we
        //   do not ourselves test it because the implementation is completely
        //   defined by the base class, which we can presume has been
        //   thoroughly vetted.
        //
        //   (seekpos)
        //   - that seeking positions the "cursor" (i.e., pptr()), at the
        //     correct location
        //   - that seeking out of bounds does not throw or abort or crash
        //     the program
        //   - that trying to seek in the "put" area has no effect
        //
        //    (capacity, data, and length)
        //   - To ensure that accessors work off of references to 'const'
        //     instances.
        //   - That 'capacity' returns the size of the buffer held by this
        //     stream buffer.
        //   - That 'data' returns the address of the underlying character
        //     array.
        //   - That 'length' returns the number of characters available in the
        //     stream buffer.
        //
        // Plan:
        //   (seekpos)
        //   - Perform a variety of seeks, each testing different settings so
        //     as to address various combinations of the above concerns.
        //
        //   (capacity, data, and length)
        //   - Create a 'bdesb_MemInput' of known size and verify its
        //     length.
        //   - Read some characters, and verify the length and content.
        //   - Consume enough characters to use all the capacity, and then
        //     verify length and content.
        //   - Read one more character so as to cause underflow, and then
        //     verify length and content.
        //   - At all times verify that 'capacity' returns the same known size
        //     with wich the 'bdesb_MemInput was created'.
        // Tactics:
        //   - Category-Partitioning Data Selection Method for 'seekpos'
        //   - Table-Based Implementation Technique for 'seekpos'
        //   - Brute-Force and Loop-Based Implementation Techniques for
        //     'data' and 'length'
        //
        // Testing:
        //   seekpos(pos_type, openmode);
        //   data();
        //   length();
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "PRIMARY MANIPULATORS/BASIC ACCESSORS" << endl
                          << "====================================" << endl;

        {
            if (verbose) cout << "\nChecking 'capacity', 'data' and, 'length'"
                              << " of varying-size streambufs right after"
                              << " point of construction." << endl;

            const int TEST_LENGTH = 5;
            char buffer[TEST_LENGTH];
            for (int i = 1; i <= TEST_LENGTH; ++i) {
                for(int j = 0; j < i; ++j) {
                    buffer[j] = 'a' + (j %26);
                }
                bdesb_FixedMemInput mSB(buffer, i);
                const bdesb_FixedMemInput& SB = mSB;
                LOOP_ASSERT(i, i == SB.length());
                LOOP_ASSERT(i, SB.data() == buffer);
                LOOP_ASSERT(i, SB.capacity() == i);
                if(veryVerbose) { T_ P(SB) }
            }
        }

        {
            if (verbose) cout << "\nChecking 'capacity', 'data' and, 'length'"
                              << " after character consumption." << endl;

            const int ARBITRARY_SMALL_SIZE = 3;
            char a[ARBITRARY_SMALL_SIZE], b[ARBITRARY_SMALL_SIZE],
                 c[ARBITRARY_SMALL_SIZE];
            char *const DATA[] = {
                a, b, c
            };

            const int DATA_LEN = sizeof DATA / sizeof *DATA;
            const int BUF_LEN = 100;
            char buffer[BUF_LEN];
            for(int i = 0; i < BUF_LEN; ++i) {
                buffer[i] = 'a' + (i % 26);
            }
            bdesb_FixedMemInput mSB(buffer, BUF_LEN);
            const bdesb_FixedMemInput& SB = mSB;
            ASSERT(SB.capacity() == BUF_LEN);

            for (int iLen = 0; iLen < DATA_LEN; iLen++) {
                for (int j = 0; j < ARBITRARY_SMALL_SIZE; ++j) {
                    DATA[iLen][j] = mSB.sbumpc();
                }

                LOOP_ASSERT(iLen, SB.length() == BUF_LEN -
                                   (ARBITRARY_SMALL_SIZE * (iLen + 1)));
                LOOP_ASSERT(iLen, 0 == strncmp(DATA[iLen],
                              &(SB.data()[ARBITRARY_SMALL_SIZE * iLen]),
                              ARBITRARY_SMALL_SIZE));

                if (veryVerbose) {
                  cout << "\t(Diminishing) stream length is "
                       <<  SB.length() << endl;
                }
            }

            if (veryVerbose) cout << endl;

            for (int j = BUF_LEN - (ARBITRARY_SMALL_SIZE * DATA_LEN);
                                                               j >= 0; --j) {
                LOOP_ASSERT(j, SB.length() == j);
                if (veryVerbose) {
                    cout << "\tWinding down to 0, length is: " << SB.length()
                         << endl;
                }
                mSB.sbumpc();

            }

            if (veryVerbose) {
                cout << "\n\tCausing underflow, and then verifying length."
                     << endl;
            }
            mSB.sbumpc();
            ASSERT(0 == SB.length());
        }

        {
            if (verbose) { T_ cout << "\nTesting seekpos." << endl;}

            const int OUT = bsl::ios_base::out;
            const int IN  = bsl::ios_base::in;

            static const struct {
                int         d_line;           // line number
                int         d_areaFlags;      // "put" or "get" area
                bdesb_FixedMemInput::pos_type  d_amount;
                                              // amount to seek
                int         d_retVal;         // Return value from pubseekpos
            } DATA[] = {
               //L#  area   amount  return
               //    flag           value
               //--  ----   ------- ----------------
               { L_,  IN,   -20,       -1   },
               { L_,  IN,    0,         0   },
               { L_,  IN,   ARBITRARY_INITIAL_BUFSIZE - 2,
                                        ARBITRARY_INITIAL_BUFSIZE - 2},
               { L_,  IN,   ARBITRARY_INITIAL_BUFSIZE - 1,
                                        ARBITRARY_INITIAL_BUFSIZE - 1  },
               { L_,  IN,   ARBITRARY_INITIAL_BUFSIZE,
                                        ARBITRARY_INITIAL_BUFSIZE},
               { L_,  IN,   ARBITRARY_INITIAL_BUFSIZE + 1,
                                       -1    },

               { L_,  OUT,    1,       -1    }
            };

            const int DATA_LEN = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < DATA_LEN; ++i ) {
                const int LINE      = DATA[i].d_line;
                const int FINAL_POS = (0 < DATA[i].d_retVal ?
                                       DATA[i].d_retVal : 0);

                char buffer[ARBITRARY_INITIAL_BUFSIZE];
                for(int j = 0; j < ARBITRARY_INITIAL_BUFSIZE; ++j) {
                    buffer[j] = 'a' + (j %26);
                }
                bdesb_FixedMemInput mSB(buffer,
                                              ARBITRARY_INITIAL_BUFSIZE);

                bdesb_FixedMemInput::pos_type ret;
                ret = mSB.pubseekpos(DATA[i].d_amount,
                                 (bsl::ios_base::openmode)DATA[i].d_areaFlags);

                if (veryVerbose) { T_ P(ret) }
                LOOP_ASSERT(LINE, DATA[i].d_retVal == ret);

                // Verify positioning by reading one char, and check the
                // char (except for out-of-bounds positions).
                int expectedChar = (FINAL_POS < ARBITRARY_INITIAL_BUFSIZE ?
                                    'a' + FINAL_POS % 26 : -1);
                if (veryVerbose) {
                    cout << "\tAt position " << FINAL_POS << " char is: "
                         << char(mSB.sgetc()) << endl;
                }
                LOOP_ASSERT(LINE, mSB.sgetc() == expectedChar);
            }
        }
      } break;

      case 3: {
        // --------------------------------------------------------------------
        // PRIMARY CONSTRUCTOR
        //   Ensure that this constructor is "wired-up" and defaults properly.
        //
        // Concerns:
        //   - That the constructor sets up all streambuf machinery properly
        //     so that streambuf operations use the client-supplied buffer.
        //   - That the streambuf is capable of using the entirety of the
        //     available storage, i.e., the full amount specified by the
        //     client.
        //   - That 'const' C arrays work as do the non-'const'.
        //
        // Plan:
        //   Use the base-class-implemented method 'sbumpc' to successively
        //   consume all characters from a 0-, 1-, and arbitrary_size_>_1-
        //   length bdesb_FixedMemInput, and ensure that all characters
        //   are retrieved.
        //
        //   Re-run the same suite for 'const' C arrays.  Three tests are
        //   overkill given white-box knowledge of the implementation, but it
        //   is cheap enough that the simplicity of duplication drives this
        //   choice.
        //
        // Tactics:
        //   - Category-Partitioning Data Selection Method
        //   - Loop-Based Implementation Techniques
        //
        // Testing:
        //   bdesb_FixedMemInput(char *, streamsize);
        //
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "PRIMARY CONSTRUCTOR" << endl
                          << "===================" << endl;
        {
            static const int sampleSizes[] = { 0, 1, 5 };

            for (int j = 0; j < 3; ++j) {

                if (verbose) cout <<
                    "Testing constructor using non-const C arrays of length "
                                  << sampleSizes[j] << '.' << endl;

                const int BUF_LEN = sampleSizes[j];
                char *buffer = new char[BUF_LEN];
                for (int i = 0; i < BUF_LEN; ++i) {
                    buffer[i] = 'a' + (i % 26);
                }
                bdesb_FixedMemInput mSB(buffer, BUF_LEN);
                const bdesb_FixedMemInput& SB = mSB;
                ASSERT(BUF_LEN == SB.length());

                bdesb_FixedMemInput::int_type ret;
                for (int i = 0; i < BUF_LEN; ++i) {
                    ret = mSB.sbumpc();
                    if (veryVerbose) {T_ P((char)ret)}
                }
                delete [] buffer;
            }
        }

        {
            static const int sampleSizes[] = { 0, 1, 5 };

            for (int j = 0; j < 3; ++j) {
                if (verbose) cout <<
                    "Testing constructor using const C arrays of length "
                                  << sampleSizes[j] << '.' << endl;

                const int BUF_LEN = sampleSizes[j];
                const char *buffer = new char[BUF_LEN];
                char *hack = const_cast<char *>(buffer);
                for (int i = 0; i < BUF_LEN; ++i) {
                    hack[i] = 'a' + (i % 26);
                }
                bdesb_FixedMemInput mSB(buffer, BUF_LEN);
                const bdesb_FixedMemInput& SB = mSB;
                ASSERT(BUF_LEN == SB.length());

                bdesb_FixedMemInput::int_type ret;
                for (int i = 0; i < BUF_LEN; ++i) {
                    ret = mSB.sbumpc();
                    if (veryVerbose) {T_ P((char)ret)}
                }
                delete [] buffer;
            }
        }

      } break;

      case 2: {

        // --------------------------------------------------------------------
        // TEST APPARATUS
        //   Verify the equipment we have set up to test itself works properly.
        //
        // Concerns:
        //     This driver uses no test apparatus.
        //
        // Plan:
        //      N/A
        //
        // Tactics:
        //   - N/A
        //
        // Testing:
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TEST APPARATUS" << endl
                          << "==============" << endl;

        if (verbose) cout << endl
                          << "This driver uses no apparatus." << endl;

      } break;

      case 1: {
        // --------------------------------------------------------------------
        // BREATHING/USAGE TEST
        //   Note: This test merely exercises basic functionality.
        //
        // Concerns:
        //   - That basic essential functionality is superficially operational.
        //   - Provide "Developers' Sandbox".
        //
        // Plan:
        //  Do whatever is needed:
        //   - Try reading from a stream buffer.
        //
        // Tactics:
        //   - Ad-Hoc Data Selection Method
        //   - Brute-Force Implementation Technique
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BREATHING TEST" << endl
                          << "==============" << endl;

        if (verbose)
           cout << "\tMake sure we can create and use a 'bdesb_FixedMemInput'."
                << endl;

        const int BUF_LEN = 100;
        char buffer[BUF_LEN];
        for(int i = 0; i < BUF_LEN; ++i) {
            buffer[i] = 'a' + (i % 26);
        }
        bdesb_FixedMemInput mSB(buffer, BUF_LEN);
        const bdesb_FixedMemInput& SB = mSB;
        ASSERT(BUF_LEN == SB.length());

        if (verbose) cout << "\tTry 'sgetc' getting character 'a'." << endl;
        int result;
        result = mSB.sgetc();
        ASSERT('a' == result);
        ASSERT(BUF_LEN == SB.length());

        if (verbose) cout << "\tTry 'sbumpc' getting character 'a'." << endl;
        result = mSB.sbumpc();
        ASSERT('a' == result);
        ASSERT(BUF_LEN - 1 == mSB.length());

        if (verbose) cout << "\tTry getting 10 characters at once." << endl;
        char temp[11];
        memset(temp, 'X', 11);
        mSB.sgetn(temp, 10);
        ASSERT(BUF_LEN - 11 == SB.length());
        ASSERT(0 == strncmp(temp, "bcdefghijk", 10));
        ASSERT('X' == temp[10]);
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
//      Copyright (C) Bloomberg L.P., 2007
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
