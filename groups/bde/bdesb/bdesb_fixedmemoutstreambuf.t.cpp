// bdesb_fixedmemoutstreambuf.t.cpp             -*-C++-*-

#include <bdesb_fixedmemoutstreambuf.h>

#include <bsl_algorithm.h>
#include <bsl_iomanip.h>
#include <bsl_iostream.h>
#include <bsl_ostream.h>
#include <bsl_string.h>
#include <bsl_strstream.h>
#include <bsl_cctype.h>
#include <bsl_cstring.h>

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

//=============================================================================
//                              TEST PLAN
//-----------------------------------------------------------------------------
//                          *** Overview ***
//
// This test driver exercises all the protected virtual methods from the
// 'basic_streambuf' protocol that are overridden by the derived concrete
// class 'bdesb_FixedMemOutStreamBuf', as well as each new (non-protocol)
// public method added in the 'bdesb_FixedMemOutStreamBuf' class.
//
// Our goal here is to ensure that the implementations comply exactly with the
// IOStreams portion of the C++ standard where the standard explicitly defines
// behavior, and that they conform to a feasible interpretation of the
// standard as described in the function documentation, where the standard
// defined behavior only loosely.  For those methods that are not protocol
// defined, we check only compliance with the behavior as described in the
// function documentation.
//
// The protected methods (whose implementation or re-implementation define
// the behavior that differentiates one kind of stream buffer from another) are
// "driven", or invoked, by various (base-class) public methods provided for
// stream-buffer client use.  One recurrent concern in this test driver is to
// ensure correct interplay between these protected methods and the base-
// class-provided implementations that use them.
//
//       Primary Constructors, Primary Manipulators, and Basic Accessors
//       ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Primary Constructors:
//   A 'bdesb_FixedMemOutStreamBuf' is created with a client-supplied C char
//   array (which can be changed during the lifetime of an instance), and a
//   length specification.  This is the only constructor, so our set of
//   primary constructors will be:
//
//    o bdesb_FixedMemOutStreamBuf(char *buffer, bsl::streamsize length);
//
// Primary Manipulators:
//   We can bring a 'bdesb_FixedMemOutStreamBuf' to any achievable white-box
//   state by using a combination of 'pubseekpos', which allows us to
//   reposition the "cursor" (i.e., the position that the next write operation
//   will output to) anywhere in the stream buffer, and 'sputc', which writes
//   a single character into the stream buffer.
//
//    o int_type sputc(char_type);
//    o pos_type pubseekpos(pos_type, ios_base::openmode);
//
// Basic Accessors:
//   We would like to find the largest set of *direct* accessors that can be
//   used generally to report back on the state of the object.  The
//   'bdesb_FixedMemOutStreamBuf' component has only 'length' and 'data' as
//   accessors, and so they form our accessor set:
//
//    o const char_type *data();
//    o streamsize length();
//-----------------------------------------------------------------------------
// CREATORS
// [ 3] bdesb_FixedMemOutStreamBuf(char *buffer, bsl::streamsize length);
// [ 6] ~bdesb_FixedMemOutStreamBuf();
// MANIPULATORS
// [ 4] int_type overflow(int_type insertionChar = traits_type::eof());
// [ 4] int_type sputc(char_type);
// [ 4] pos_type seekpos(pos_type, openmode);
// [ 9] pos_type seekoff(off_type, seekdir, openmode);
// [ 8] streamsize xsputn(const char_type, streamsize);
//
// ACCESSORS
// [ 4] const char_type *data() const;
// [ 4] streamsize capacity() const;
// [ 4] streamsize length() const;
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 2] TEST APPARATUS
// [ 5] ostream& operator<<(ostream&, const bdesb_FixedMemOutStreamBuf&);
//
// USAGE EXAMPLE
// [10] Capitalizing Stream
//----------------------------------------------------------------------------

//=============================================================================
//                    STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
static int testStatus = 0;
static void aSsErT(int c, const char *s, int i)
{
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}
#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }
//-----------------------------------------------------------------------------
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
const int INITIAL_BUFSIZE = 20;

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

               // ====================================
               // operator<< for bdesb_MemOutStreamBuf
               // ====================================
// FREE OPERATORS
bsl::ostream& operator<<(bsl::ostream&                     stream,
                         const bdesb_FixedMemOutStreamBuf& streamBuffer);
    // Write the contents of the specified 'streamBuffer' (as well as a marker
    // indicating eight bytes groupings) to the specified output 'stream' in
    // binary format, and return a reference to the modifiable 'stream'.

bsl::ostream& operator<<(bsl::ostream&                     stream,
                         const bdesb_FixedMemOutStreamBuf& streamBuffer)
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
//                  CLASSES FOR TESTING USAGE EXAMPLES
//-----------------------------------------------------------------------------
// Usage
// -----
// This example demonstrates use of a stream buffer by a stream, in this case
// a stream with simple formatting requirements -- namely, capitalizing all
// character data that passes through its management.  (To simplify the
// example, we do not include the functions for streaming non-character data.)
//..
 class my_CapitalizingStream {
     // This class capitalizes character data....

     enum { STREAMBUF_CAPACITY = 30 };

     char *d_buffer;
     bdesb_FixedMemOutStreamBuf *d_streamBuf;

     friend my_CapitalizingStream& operator<<(my_CapitalizingStream& stream,
                                              const string&          data);
     friend my_CapitalizingStream& operator<<(my_CapitalizingStream& stream,
                                              const char *           data);
     friend my_CapitalizingStream& operator<<(my_CapitalizingStream& stream,
                                              char                   data);
   public:
     // CREATORS
     my_CapitalizingStream();
         // Create a stream that....

     ~my_CapitalizingStream();
         // Destroy this object.

     // ACCESSORS
     const bdesb_FixedMemOutStreamBuf* streamBuf() { return d_streamBuf; }
         // Return the stream buffer used by this stream.  Note that this
         // function is for debugging only.
 };

     // FREE OPERATORS
     my_CapitalizingStream& operator<<(my_CapitalizingStream& stream,
                                       const bsl::string&     data);
     my_CapitalizingStream& operator<<(my_CapitalizingStream& stream,
                                       const char *           data);
     my_CapitalizingStream& operator<<(my_CapitalizingStream& stream,
                                       char                   data);
         // Write the specified 'data' in capitalized form to the
         // specified 'stream'....
//..
// As is typical, the streaming operators are made friends of the class.  We
// use the 'transform' algorithm to convert all string characters to upper-
// case.
//..
 #include <bsl_algorithm.h>

 my_CapitalizingStream::my_CapitalizingStream()
 {
     d_buffer = new char[STREAMBUF_CAPACITY];
     d_streamBuf = new bdesb_FixedMemOutStreamBuf(d_buffer,
                                                           STREAMBUF_CAPACITY);
 }

 my_CapitalizingStream::~my_CapitalizingStream()
 {
     delete d_streamBuf;
     delete [] d_buffer;
 }

 // FREE OPERATORS
 my_CapitalizingStream& operator<<(my_CapitalizingStream& stream,
                                   const bsl::string&     data)
 {
     bsl::string tmp(data);
     transform(tmp.begin(), tmp.end(), tmp.begin(), (int(*)(int))bsl::toupper);
     stream.d_streamBuf->sputn(tmp.data(), tmp.length());
     return stream;
 }

 my_CapitalizingStream& operator<<(my_CapitalizingStream& stream,
                                   const char *           data)
 {
     bsl::string tmp(data);
     transform(tmp.begin(), tmp.end(), tmp.begin(), (int(*)(int))bsl::toupper);
     stream.d_streamBuf->sputn(tmp.data(), tmp.length());
     return stream;
 }

 my_CapitalizingStream& operator<<(my_CapitalizingStream& stream,
                                   char                   data)
 {
     stream.d_streamBuf->sputc(bsl::toupper(data));
     return stream;
 }

typedef bsl::ios_base::openmode io_openmode;
typedef bsl::ios_base::seekdir  io_seekdir;

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
        // USAGE EXAMPLE
        //
        // Testing:
        //   A capitalizing stream.
        // --------------------------------------------------------------------
        {
            my_CapitalizingStream cs;
            cs << "Hello" << ' ' << "world." << '\0';

            if (verbose) {
                // Visually verify that the streamed data has been capitalized.
                cout << cs.streamBuf()->data() << endl;
            }

            ASSERT(0 == strcmp("HELLO WORLD.", cs.streamBuf()->data()));
        }
      } break;

      case 9: {
        // --------------------------------------------------------------------
        // OSTREAM BASED ON FIXEDMEMOUTSTREAMBUF TEST
        //
        // Concerns:
        //   That it is possible to initialize an ostream with a
        //   bdesb_FixedMemOutStreamBuf and do some output using stream output.
        //
        // Plan:
        //   Initialize an ostream with a bdesb_FixedMemOutStreamBuf, do some
        //   stream output, and examine the result.
        // --------------------------------------------------------------------
        if (verbose) cout << "\nSTREAM TESTS"
                             "\n============\n";

        char buffer[INITIAL_BUFSIZE];
        bdesb_FixedMemOutStreamBuf mSB(buffer, INITIAL_BUFSIZE);

        ostream os(&mSB);

        os << "Woof " << 38 << "\n" << ends;

        ASSERT(!strcmp(mSB.data(), "Woof 38\n"));
      } break;

      case 8: {
        // --------------------------------------------------------------------
        // SEEK TEST
        //
        // Concerns:
        //   - that seeking relative to a specification ('seekoff') uses
        //     the correct location from which to offset
        //   - that both negative and positive offsets compute correctly
        //   - that seeking positions the "cursor" (i.e., pptr()) at the
        //     correct location
        //   - that seeking out of bounds returns -1 and does not throw or
        //     abort or crash the program
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

        const io_openmode OUT = bsl::ios_base::out;
        const io_openmode IN  = bsl::ios_base::in;
        const io_seekdir  CUR = bsl::ios_base::cur;
        const io_seekdir  BEG = bsl::ios_base::beg;
        const io_seekdir  END = bsl::ios_base::end;
        char mFILL[INITIAL_BUFSIZE];
        memset(mFILL, 'a', sizeof mFILL);

        {
            static const struct {
                int         d_line;          // line number
                io_openmode d_areaFlags;     // "put" area or "get" area
                bdesb_FixedMemOutStreamBuf::off_type
                            d_amount;        // amount to seek
                io_seekdir  d_base;          // seekoff from where?
                int         d_retVal;        // expected return from seekoff
            } DATA[] = {
               //L#  area
               //    flag   amount                base retVal
               //--  ----   -------               ---- ------
               // seekoff from the start of the streambuf
               { L_,  OUT,    -2,                 BEG, -1  },
               { L_,  OUT,    0,                  BEG,  0  },
               { L_,  OUT,    1,                  BEG,  1  },
               { L_,  OUT,   INITIAL_BUFSIZE - 1, BEG, INITIAL_BUFSIZE - 1  },
               { L_,  OUT,   INITIAL_BUFSIZE,     BEG, INITIAL_BUFSIZE  },
               { L_,  OUT,   INITIAL_BUFSIZE + 1, BEG, -1  },
               { L_,  OUT,   500,                 BEG, -1  },

               // seekoff in the "get" area
               { L_,  IN,    22,                  BEG, -1  },

               // seekoff from the end of the streambuf.
               { L_,  OUT,  -300,                 END, -1  },
               { L_,  OUT,  -INITIAL_BUFSIZE+1,   END,  1  },
               { L_,  OUT,  -INITIAL_BUFSIZE,     END,  0  },
               { L_,  OUT,  -INITIAL_BUFSIZE-1,   END, -1  },
               { L_,  OUT,  -10,                  END, INITIAL_BUFSIZE - 10 },
               { L_,  OUT,    0,                  END, INITIAL_BUFSIZE },
               { L_,  OUT,    1,                  END, -1 },

               // seekoff in the "get" area
               { L_,  IN,    22,                  END, -1  },

               // seekoff from the current cursor, where cur == end
               { L_,  OUT,  -300,                 CUR, -1 },
               { L_,  OUT,  -INITIAL_BUFSIZE+1,   CUR, 1  },
               { L_,  OUT,  -INITIAL_BUFSIZE,     CUR, 0  },
               { L_,  OUT,  -INITIAL_BUFSIZE-1,   CUR, -1  },
               { L_,  OUT,  -10,                  CUR, INITIAL_BUFSIZE - 10 },
               { L_,  OUT,    0,                  CUR, INITIAL_BUFSIZE },
               { L_,  OUT,    1,                  CUR, -1 },

               // seekoff in the "get" area
               { L_,  IN,    22,                  CUR, -1  }
            };
            const int DATA_LEN = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < DATA_LEN; ++i ) {
                const int LINE      = DATA[i].d_line;
                const int FINAL_POS = (0 <= DATA[i].d_retVal ?
                                       DATA[i].d_retVal : INITIAL_BUFSIZE);

                char buffer[INITIAL_BUFSIZE];
                memset(buffer, 'Z', INITIAL_BUFSIZE);
                bdesb_FixedMemOutStreamBuf mSB(buffer, INITIAL_BUFSIZE);

                bdesb_FixedMemOutStreamBuf::int_type ret;
                mSB.sputn(mFILL, INITIAL_BUFSIZE);

                ret = mSB.pubseekoff(DATA[i].d_amount,
                                     DATA[i].d_base,
                                     DATA[i].d_areaFlags);
                LOOP_ASSERT(LINE, DATA[i].d_retVal == ret);
                if (veryVerbose) P(ret);

                // Verify positioning by writing one char, and check the
                // char, its predecessor, and its successor.  (Except in
                // out of bounds conditions.)

                if (FINAL_POS >= INITIAL_BUFSIZE) {
                    continue;
                }

                mSB.sputc('!');
                LOOP_ASSERT(LINE, '!' == buffer[FINAL_POS]);
                LOOP_ASSERT(LINE, FINAL_POS < 1 ||
                            mFILL[FINAL_POS - 1] == buffer[FINAL_POS - 1]);
                LOOP_ASSERT(LINE, FINAL_POS >= INITIAL_BUFSIZE - 1 ||
                            mFILL[FINAL_POS + 1] == buffer[FINAL_POS + 1]);
            }
        }

        if (verbose) cout <<
              "\nTesting seekoff from a variety of current-pointer positions."
                          << endl;
        {
            static const struct {
                int         d_line;             // line number
                int         d_offset;           // seek offset
                int         d_initialPosition;  // where to set pptr() first
                int         d_retVal;           // expected return value
            } DATA[] = {
               //L#  offset   startPoint   retVal
               //--  ------   ----------   --------
               { L_,  -1,          0,         -1  },
               { L_,   0,          0,          0  },
               { L_,   0,         10,         10  },
               { L_,  INITIAL_BUFSIZE - 1,
                                   0,         -1  },
               { L_,  INITIAL_BUFSIZE,
                                   0,         -1  },
               { L_,  INITIAL_BUFSIZE + 1,
                                   0,         -1  },
               { L_,  500,         0,         -1  },

               { L_,  -110,       10,         -1  },
               { L_,  -11,        10,         -1  },
               { L_,  -10,        10,         0   },
               { L_,  -9,         10,          1  },
               { L_,   0,         10,         10  },
               { L_,   10,        10,         -1  },
               { L_,   11,        10,         -1  },
               { L_,   200,       10,         -1  },

               { L_,  -110, INITIAL_BUFSIZE,  -1  },
               { L_,  -(INITIAL_BUFSIZE+1),
                            INITIAL_BUFSIZE,  -1 },
               { L_,  -INITIAL_BUFSIZE,
                            INITIAL_BUFSIZE,  0 },
               { L_,  -(INITIAL_BUFSIZE-1),
                            INITIAL_BUFSIZE,  1 },
               { L_,  -10,  INITIAL_BUFSIZE,  INITIAL_BUFSIZE - 10 },
               { L_,  -1,   INITIAL_BUFSIZE,  INITIAL_BUFSIZE - 1  },
               { L_,   0,   INITIAL_BUFSIZE,  INITIAL_BUFSIZE },
               { L_,   1,   INITIAL_BUFSIZE,  -1  },
            };
            const int DATA_LEN = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < DATA_LEN; ++i ) {
                const int LINE      = DATA[i].d_line;
                const int FINAL_POS = (0 <= DATA[i].d_retVal ?
                                       DATA[i].d_retVal :
                                       DATA[i].d_initialPosition);

                char buffer[INITIAL_BUFSIZE];
                memset(buffer, 'Z', INITIAL_BUFSIZE);
                bdesb_FixedMemOutStreamBuf mSB(buffer, INITIAL_BUFSIZE);
                const bdesb_FixedMemOutStreamBuf& SB = mSB;

                bdesb_FixedMemOutStreamBuf::int_type ret;
                mSB.sputn(mFILL, DATA[i].d_initialPosition);
                ret = mSB.pubseekoff(DATA[i].d_offset, CUR, OUT);
                if (veryVerbose) P(ret);

                LOOP_ASSERT(LINE, DATA[i].d_retVal == ret);
                LOOP_ASSERT(LINE, FINAL_POS == SB.length());
                LOOP_ASSERT(LINE, 0 == bsl::memcmp(mFILL,buffer,SB.length()));

                // Verify positioning by writing one char, and check the
                // char, its predecessor, and its successor.  (Except in
                // out of bounds conditions.)

                if (FINAL_POS >= INITIAL_BUFSIZE) {
                    continue;
                }

                mSB.sputc('!');
                LOOP_ASSERT(LINE, '!' == buffer[FINAL_POS]);
                LOOP_ASSERT(LINE, FINAL_POS < 1 ||
                            mFILL[FINAL_POS - 1] == buffer[FINAL_POS - 1]);
                LOOP_ASSERT(LINE, FINAL_POS >= DATA[i].d_initialPosition - 1 ||
                            mFILL[FINAL_POS + 1] == buffer[FINAL_POS + 1]);
            }
        }
      } break;

      case 7: {
        // --------------------------------------------------------------------
        // PUT string TEST
        //
        // Concerns:
        //   - that string of varying lengths are written correctly
        //   - that writing strings does not overwrite existing buffer
        //     contents
        //   - that no more than the specified number of characters are written
        //   - that no writing happens beyond existing capacity
        //
        // Plan:
        //   Write out representative strings from the categories 0 characters,
        //   1 character, and > 1 character, into streambufs with
        //   representative contents "empty", substantially less than
        //   capacity, and almost-full-so-that-next-write-exceeds-capacity
        //
        // Tactics:
        //   - Category-Partitioning Data Selection Method
        //   - Table-Based Implementation Technique
        //
        // Testing:
        //   streamsize xsputn(const char_type, streamsize);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "PUT STRING TEST" << endl
                          << "===============" << endl;

        {
            if (verbose) cout << "\nTesting sputn." << endl;

            const struct TestData {
                int         d_line;          // line number
                const char *d_outStr;        // string to output
                int         d_strCap;        // stream capacity
                const char *d_initialCont;   // initial contents of stream
                const char *d_result;        // expected contents after 'sputc'
                int         d_numChars;      // number of chars
                int         d_returnVal;     // 'sputn' return val
            } DATA[] = {
               //L#   out  stream  init   result   stream   ret
               //    strng cpcity contnt  cntnt   length()  val
               //--  ----- ------ ------  ------  --------  ---
              { L_,  "",    0,      "",     "",     0,       0  }, // N = 0
              { L_,  "s",   0,      "",     "",     0,       0  },
              { L_,  "abc", 0,      "",     "",     0,       0  },

              { L_,  "",    1,      "",     "",     0,       0  }, // N = 1
              { L_,  "s",   1,      "a",   "a",     1,       0  },
              { L_,  "abc", 1,      "a",   "a",     1,       0  },

              { L_,  "",    2,      "",     "",     0,       0  }, // N = 2
              { L_,  "s",   2,      "a",   "as",    2,       1  },
              { L_,  "abc", 2,      "a",   "aa",    2,       1  },

              { L_,  "s",   3,      "ab",   "abs",  3,       1  }, // N = 3
              { L_,  "abc", 3,      "ab",   "aba",  3,       1  },

              { L_,  "s",   4,      "ab",   "abs",  3,       1  }, // N = 4
              { L_,  "abc", 4,      "ab",  "abab",  4,       2  },

              { L_,  "abc", 5,      "ab",  "ababc", 5,       3  }  // N = 5
            };

            const int DATA_LEN = sizeof DATA / sizeof *DATA;

            // This segment verifies correct behavior across different
            // initial buffer states (buffer length x buffer contents.)
            for(int i = 0; i < DATA_LEN; ++i ) {
                const int LINE      = DATA[i].d_line;

                char *bytes = new char[DATA[i].d_strCap];
                bdesb_FixedMemOutStreamBuf sb(bytes, DATA[i].d_strCap);
                for(unsigned j = 0; j < strlen(DATA[i].d_initialCont); ++j ) {
                    sb.sputc(DATA[i].d_initialCont[j]);
                }
                int retResult = sb.sputn(DATA[i].d_outStr,
                                         strlen(DATA[i].d_outStr));
                LOOP_ASSERT(LINE, 0 == strncmp(bytes, DATA[i].d_result,
                                            strlen(DATA[i].d_result )) );
                LOOP_ASSERT(LINE, DATA[i].d_returnVal == retResult );
                LOOP_ASSERT(LINE, DATA[i].d_numChars == sb.length());
                delete [] bytes;
            }
        }
      } break;

      case 6: {
        // --------------------------------------------------------------------
        // NON-PRIMARY CONSTRUCTORS AND DESTRUCTOR.
        //
        // This component has no non-primary constructors, and the destructor
        // does nothing.
        //
        // Testing:
        //   ~bdesb_FixedMemOutStreamBuf();
        // --------------------------------------------------------------------
        {
            if (verbose) cout <<
    "There are no non-primary constructors, and the destructor does nothing."
                              << endl;

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
        //   - that these functions work on references to 'const' instances.
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
        //   ostream& operator<<(ostream&, const bdesb_MemOutStreamBuf&);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                  << "OUTPUT bsl::ostream& 'operator<<'" << endl
                  << "=================================" << endl;

        if (verbose) cout << "\nCreate stream buffers to be printed." << endl;

        {
            if (verbose) cout << "\tChecking operator<< return value."
                              << endl;

            char buffer[INITIAL_BUFSIZE];
            memset(buffer, 'Z', INITIAL_BUFSIZE);
            bdesb_FixedMemOutStreamBuf mSB(buffer, INITIAL_BUFSIZE);
            const bdesb_FixedMemOutStreamBuf& SB = mSB;

            const int SIZE = 100;
            char buf1[SIZE], buf2[SIZE];
            ostrstream out1(buf1, SIZE), out2(buf2, SIZE);
            out1 << SB << "arbitrary";  // Ensure modifiable
            out2 << SB << "value";      // stream is returned.
        }

        {
            if (verbose) cout << "\n\tChecking operator<< output."
                              << endl;

            char buffer[INITIAL_BUFSIZE];
            memset(buffer, 'Z', INITIAL_BUFSIZE);
            bdesb_FixedMemOutStreamBuf mSB(buffer,
                                            INITIAL_BUFSIZE);
            const bdesb_FixedMemOutStreamBuf& SB = mSB;

            mSB.sputn("hello", 5);

            const int SIZE = 100;
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
                     "\n0000\t01101000 01100101 01101100 01101100 01101111";
            ASSERT(0 == memcmp(buf1, EXPECTED, stringLen));
            buf1[stringLen] = 0;
            if (verbose) {
                T_ P(buf1)
            }
        }

        {
            if (verbose) cout << "\n\tChecking operator<< output again."
                              << endl;

            // One more test, just to see something different
            char buffer[INITIAL_BUFSIZE];
            memset(buffer, 'Z', INITIAL_BUFSIZE);
            bdesb_FixedMemOutStreamBuf mSB(buffer,
                                            INITIAL_BUFSIZE);
            const bdesb_FixedMemOutStreamBuf& SB = mSB;

            mSB.sputc('0');  mSB.sputc('1');  mSB.sputc('2');  mSB.sputc('3');
            mSB.sputc('4');  mSB.sputc('5');  mSB.sputc('6');  mSB.sputc('7');
            mSB.sputc('8');  mSB.sputc('9');  mSB.sputc('A');  mSB.sputc('B');

            const char *EXPECTED =
                                 "\n0000\t00110000 00110001 00110010 00110011 "
                                         "00110100 00110101 00110110 00110111"
                                 "\n0008\t00111000 00111001 01000001 01000010";
            char ACTUAL[500];
            memset(ACTUAL, 'Z', 500);
            ostrstream out(ACTUAL, 500);
            out << SB << ends;
            const int LEN = strlen(EXPECTED) + 1;
            if (verbose) cout << endl;
            if (verbose) {
                T_ P(EXPECTED)
                T_ P(ACTUAL)
            }
            ASSERT('Z' == ACTUAL[LEN]); // check for overrun
            ASSERT(0 == memcmp(ACTUAL, EXPECTED, LEN));
        }
      } break;

      case 4: {
        // --------------------------------------------------------------------
        // PRIMARY MANIPULATORS/BASIC ACCESSORS
        //   Verify the most basic functionality of a
        //   'bdesb_FixedMemOutStreamBuf'.
        //
        // Concerns:
        //   (sputc)
        //   - that printing and non-printing characters are written correctly
        //   - that bytes with leading bit set are written correctly
        //   - that no more than one character is written
        //
        //   We also test overflow as triggered by 'sputc' here.
        //
        //   (seekpos)
        //   - that seeking positions the "cursor" (i.e., pptr()), at the
        //     correct location
        //   - that seeking out of bounds does not throw or abort or crash
        //     the program
        //   - that trying to seek in the "get" area has no effect
        //
        //    (data & length)
        //   - To ensure that accessors work off of references to 'const'
        //    instances.
        //   - That data return the address of the underlying character array.
        //   - That length return the number of characters written to the
        //     stream buffer.
        //
        // Plan:
        //   (sputc)
        //   - Write at least one sample value from each category of characters
        //     using the 'sputc' method, and verify that the bit pattern for
        //     that character is present and correct in the stream buffer.
        //
        //   (seekpos)
        //   - Perform a variety of seeks, each testing different settings so
        //     as to address various combinations of the above concerns.
        //
        //   (data and length)
        //   - Create an empty 'bdesb_MemOutStreamBuf' and verify its length.
        //   - Add a character, and verify the length and content.
        //   - Add enough characters to use all the initial capacity, and then
        //     verify length and content.
        //   - Then add one more character so as to exceed initial capacity,
        //     and verify length and content.
        //
        // Tactics:
        //   - Category-Partitioning Data Selection Method for 'sputc'
        //   - Table-Based and Brute-Force Implementation Technique for
        //     'sputc'
        //   - Ad-Hoc Data Selection Method for 'seekpos'
        //   - Brute-Force Techniques for 'seekpos'
        //
        // Testing:
        //   int_type overflow(int_type insertionChar = traits_type::eof());
        //   int_type sputc(char_type);
        //   pos_type seekpos(pos_type, openmode);
        //   const char_type *data() const;
        //   streamsize length() const;
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "PRIMARY MANIPULATORS/BASIC ACCESSORS" << endl
                          << "====================================" << endl;

        if (verbose) cout <<
            "\nEmpty streambuf -- checking data() and length()." << endl;
        {
            char buffer[INITIAL_BUFSIZE];
            memset(buffer, 'Z', INITIAL_BUFSIZE);
            bdesb_FixedMemOutStreamBuf mSB(buffer,
                                            INITIAL_BUFSIZE - 1);
            const bdesb_FixedMemOutStreamBuf& SB = mSB;
            ASSERT(0 == SB.length());
            ASSERT(SB.data() == buffer);

            if(veryVerbose) P(SB);
        }

        if (verbose) cout << "\nTesting sputc." << endl;
        {
            typedef bdesb_FixedMemOutStreamBuf::char_type T;

            static const struct {
                int            d_line;       // line number
                T              d_outChar;    // character to output
                const char    *d_result;     // expected contents after 'sputc'
            } DATA[] = {
                  //L#  output char    resulting
                  //                 stream contents
                  //--  -----------  ---------------
                  // Add ordinary ASCII character
                  { L_,    's',            "s"     },
                  // Add non-printing character
                  { L_,    10,             "\xA"   },
                  // Add <= 127
                  { L_,    127,            "\x7F"  },
                  // Add >= 128
                  { L_,    (T)128,         "\x80"  }
            };   // end table DATA

            const int DATA_LEN = sizeof DATA / sizeof *DATA;

            // This loop verifies that 'sputc' both:
            //    1. Adds the character, and
            //    2. Does not overwrite beyond the character.

            if (verbose) { T_ cout << "Testing different character types."
                                   << endl; }

            for (int i = 0; i < DATA_LEN; ++i ) {
                const int LINE      = DATA[i].d_line;
                char buffer[INITIAL_BUFSIZE];
                memset(buffer, 'Z', INITIAL_BUFSIZE);
                bdesb_FixedMemOutStreamBuf mSB(buffer,
                                               INITIAL_BUFSIZE - 1);
                const bdesb_FixedMemOutStreamBuf& SB = mSB;

                mSB.sputc(DATA[i].d_outChar);
                LOOP_ASSERT(LINE, DATA[i].d_outChar   == SB.data()[0]);
                LOOP_ASSERT(LINE, 1                   == SB.length());
                LOOP_ASSERT(LINE, INITIAL_BUFSIZE - 1 == SB.capacity());
                LOOP_ASSERT(LINE, 'Z'                 == SB.data()[1]);
                if (veryVerbose) { P(SB); }
            }
        }
        {
            if (verbose) { T_ cout <<
                                   "Testing different initial buffer states."
                                   << endl; }

            const struct TestData {
                int         d_line;          // line number
                char        d_outChar;       // character to output
                int         d_strCap;        // stream capacity
                const char *d_initialCont;   // initial contents of stream
                const char *d_result;        // expected contents after 'sputc'
                int         d_numChars;      // number of chars
                int         d_returnVal;     // 'sputc' return val
            } DATA[] = {
               //L#  out  stream  init   result   stream    ret
               //    char cpcity contnt  cntnt   length()   val
               //--  ---- ------ ------  ------  --------  ----
               { L_,  's',  0,      "",     "",     0,      -1  }, // N = 0

               { L_,  's',  1,      "",    "s",     1,      's' }, // N = 1
               { L_,  's',  1,      "a",   "a",     1,      -1  },

               { L_,  's',  2,     "",     "s",     1,     's'  }, // N = 2
               { L_,  's',  2,     "a",   "as",     2,     's'  },
               { L_,  's',  2,    "ab",   "ab",     2,      -1  },

               { L_,  's',  3,     "",     "s",     1,     's'  }, // N = 3
               { L_,  's',  3,    "ab",   "abs",    3,     's'  },
               { L_,  's',  3,    "abc",  "abc",    3,      -1  },

               { L_,  's',  4,     "",     "s",     1,     's'  }, // N = 4
               { L_,  's',  4,     "ab",  "abs",    3,     's'  },
               { L_,  's',  4,    "abcd", "abcd",   4,      -1  },
            };

            const int DATA_LEN = sizeof DATA / sizeof *DATA;

            // This segment verifies correct behavior across different
            // initial buffer states (buffer length x buffer contents.)
            for(int i = 0; i < DATA_LEN; ++i ) {
                const int LINE      = DATA[i].d_line;
                char *bytes = new char[DATA[i].d_strCap];
                bdesb_FixedMemOutStreamBuf mSB(bytes, DATA[i].d_strCap);
                const bdesb_FixedMemOutStreamBuf& SB = mSB;

                for (unsigned j = 0; j < strlen(DATA[i].d_initialCont); ++j) {
                    mSB.sputc(DATA[i].d_initialCont[j]);
                }
                if (veryVerbose) { T_ cout << "Initial contents: "; P(SB)};
                int retResult = mSB.sputc(DATA[i].d_outChar);
                LOOP_ASSERT(LINE, 0 == strncmp(bytes, DATA[i].d_result,
                                            strlen(DATA[i].d_result )));
                LOOP_ASSERT(LINE, DATA[i].d_returnVal == retResult);
                LOOP_ASSERT(LINE, DATA[i].d_numChars  == SB.length());
                LOOP_ASSERT(LINE, DATA[i].d_strCap    == SB.capacity());
                delete [] bytes;
            }
        }
        {
            if (verbose) {
                    cout <<
                  "\n\tVerifying that overflow does not corrupt the streambuf."
                                   << endl; }

            // Do an extra test to ensure that overflow does not corrupt
            // the stream
            char buffer[5];
            memset(buffer, 'X', 5);
            bdesb_FixedMemOutStreamBuf x(buffer, 5);
            x.sputc('a'); x.sputc('b'); x.sputc('c'); x.sputc('d');
            x.sputc('e');
            ASSERT( 0 == strncmp(buffer, "abcde", 5));
            x.sputc('f');
            ASSERT( 0 == strncmp(buffer, "abcde", 5));
            ASSERT( 5 == x.length());
        }
        {
            if (verbose) { T_ cout << "\nTesting seekpos." << endl;}

            const io_openmode OUT = bsl::ios_base::out;

            static const struct {
                int         d_line;           // line number
                io_openmode d_areaFlags;      // "put" or "get" area
                bdesb_FixedMemOutStreamBuf::pos_type  d_amount;
                                              // amount to seek
                int         d_retVal;         // expected return value
            } DATA[] = {
               //L#  area   amount                return
               //    flag                         value
               //--  ----   -------               ----------------
               { L_,  OUT,   -20,                 -1  },
               { L_,  OUT,   -1,                  -1  },
               { L_,  OUT,    0,                  0   },
               { L_,  OUT,   INITIAL_BUFSIZE - 2, INITIAL_BUFSIZE - 2},
               { L_,  OUT,   INITIAL_BUFSIZE - 1, INITIAL_BUFSIZE - 1  },
               { L_,  OUT,   INITIAL_BUFSIZE,     INITIAL_BUFSIZE},
               { L_,  OUT,   INITIAL_BUFSIZE + 1, -1 },
            };

            const int DATA_LEN = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < DATA_LEN; ++i ) {
                const int LINE      = DATA[i].d_line;
                const int FINAL_POS = (0 <= DATA[i].d_retVal ?
                                       DATA[i].d_retVal : INITIAL_BUFSIZE);

                char buffer[INITIAL_BUFSIZE];
                bdesb_FixedMemOutStreamBuf mSB(buffer,
                                            INITIAL_BUFSIZE);

                bdesb_FixedMemOutStreamBuf::pos_type ret;
                for(int j = 0; j < INITIAL_BUFSIZE; ++j) {
                    mSB.sputc('a');
                }

                ret = mSB.pubseekpos(DATA[i].d_amount, DATA[i].d_areaFlags);

                if (veryVerbose) P(ret)
                LOOP_ASSERT(LINE, DATA[i].d_retVal == ret);

                // Verify positioning by writing one char, and check the
                // char, its predecessor, and its successor.  (Except in
                // out of bounds conditions.)

                if (FINAL_POS < 0 || INITIAL_BUFSIZE - 1)
                    continue;

                mSB.sputc('b');
                LOOP_ASSERT(LINE, 'b' == buffer[FINAL_POS]);
                LOOP_ASSERT(LINE, FINAL_POS < 1 ||
                            'a' == buffer[FINAL_POS - 1]);
                LOOP_ASSERT(LINE, FINAL_POS >= INITIAL_BUFSIZE - 1 ||
                            'a' == buffer[FINAL_POS + 1]);
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
        //
        // Plan:
        //   There are no public functions with which to cleanly observe the
        //   state of the object without manipulating it.  This test will rely
        //   on using the base-class-implemented 'sputc' to check that writing
        //   into the stream buffer in fact writes into the client-supplied
        //   C-array.  We will write enough characters to verify that the
        //   length as specified is completely usable, and then write one more
        //   to ensure that the stream buffer is using the client-supplied
        //   specification as an upper bound as well as a lower bound.
        //
        // Tactics:
        //   - Ad-Hoc Data Selection Method
        //   - Brute-Force Implementation Technique
        //
        // Testing:
        //   bdesb_FixedMemOutStreamBuf(char *buffer, bsl::streamsize length);
        //
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "PRIMARY CONSTRUCTOR" << endl
                          << "===================" << endl;

        if (verbose) cout <<
            "\nEnsure that the stream buffer uses client-specified buffer."
                          << endl;
        {
            if (verbose) cout <<
             "\nEnsure that the stream buffer has at least specified capacity."
                              << endl;

             char buffer[INITIAL_BUFSIZE];
             memset(buffer, 'Z', INITIAL_BUFSIZE);
             bdesb_FixedMemOutStreamBuf mSB(buffer,
                                            INITIAL_BUFSIZE - 1);
             const bdesb_FixedMemOutStreamBuf& SB = mSB;

             for (int i = 0; i < INITIAL_BUFSIZE - 1; ++i) {
                 mSB.sputc('a');
                 LOOP_ASSERT(i, 'a' == buffer[i]);
                 LOOP_ASSERT(i, 'Z' == buffer[i + 1]);
                 if(veryVerbose) P(SB);
             }

             if (verbose) cout <<
         "\nEnsure that the stream buffer has no more than specified capacity."
                              << endl;

             mSB.sputc('a');
             ASSERT('Z' == buffer[INITIAL_BUFSIZE - 1]);
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
        //   - Try writing out to a stream buffer.
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

        if (verbose) cout <<
          "\nMake sure we can create and use a 'bdesb_FixedMemOutStreamBuf'."
                          << endl;
        {
            char buffer[INITIAL_BUFSIZE];
            bdesb_FixedMemOutStreamBuf mSB(buffer, INITIAL_BUFSIZE);
            const bdesb_FixedMemOutStreamBuf& SB = mSB;
            if (verbose) { cout <<
                 "\tCreate a fixed-length output stream buffer: "; P(SB) }
            ASSERT(0 == SB.length());

            mSB.sputn("hello", 5);
            if (verbose) { cout <<
               "\n\tWrite a string (five chars) to the stream buffer: ";
                                                                 P(SB) }
            ASSERT(5 == SB.length());
            ASSERT(0 == strncmp(SB.data(), "hello", 5));

            mSB.sputc('s');
            if (verbose) { cout <<
                     "\n\tWrite a single char to the stream buffer: "; P(SB) }
            ASSERT(6 == SB.length());
            ASSERT(0 == strncmp(SB.data(), "hellos", 6));
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

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2004
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
