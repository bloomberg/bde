// bdesb_memoutstreambuf.t.cpp               -*-C++-*-

#include <bdesb_memoutstreambuf.h>

#include <bslma_default.h>                // for testing only
#include <bslma_defaultallocatorguard.h>  // for testing only
#include <bslma_testallocator.h>          // for testing only

#include <bsl_algorithm.h>
#include <bsl_cctype.h>
#include <bsl_cstring.h>
#include <bsl_iomanip.h>
#include <bsl_iostream.h>
#include <bsl_ostream.h>
#include <bsl_string.h>
#include <bsl_strstream.h>

#include <bsl_cctype.h>

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

//=============================================================================
//                              TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// This test driver exercises all the protected virtual methods from the
// 'basic_streambuf' protocol that are overridden by the derived concrete
// class 'bdesb_MemOutStreamBuf', as well as each new (non-protocol) public
// method added in the 'bdesb_MemOutStreamBuf' class.
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
//   A 'bdesb_MemOutStreamBuf' is created with an allocator which cannot be
//   changed throughout the lifetime of an object, and an optional initial
//   capacity specification.  There is no particular advantage (for purposes
//   of putting the object into white-box state) to using the constructor
//   with capacity, so our set of primary constructors will be:
//
//    o bdesb_MemOutStreamBuf(bslma_Allocator *basicAllocator = 0);
//
// Primary Manipulators:
//   We can bring a 'bdesb_MemOutStreamBuf' to any achievable white-box
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
//   'bdesb_MemOutStreamBuf' component has only 'length' and 'data' as
//   accessors, and so they form our accessor set:
//
//    o const char_type *data();
//    o streamsize length();
//-----------------------------------------------------------------------------
// CREATORS
// [ 3] bdesb_MemOutStreamBuf(bslma_Allocator *basicAllocator = 0);
// [ 6] bdesb_MemOutStreamBuf(int numElements, *ba = 0);
// [ 6] ~bdesb_MemOutStreamBuf();
// MANIPULATORS
// [ 4] int_type overflow(int_type insertionChar = traits_type::eof());
// [ 4] int_type sputc(char_type);
// [ 4] pos_type seekpos(pos_type, openmode);
// [ 9] pos_type seekoff(off_type, seekdir, openmode);
// [ 8] streamsize xsputn(const char_type, streamsize);
// [ 7] void reserveCapacity(int numElements);
// [11] void reset();
// ACCESSORS
// [ 4] const char_type *data() const;
// [ 4] streamsize length() const;
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 2] TEST APPARATUS
// [ 5] ostream& operator<<(ostream&, const bdesb_MemOutStreamBuf&);
// [11] USAGE EXAMPLE: Capitalizing Stream
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

typedef bdesb_MemOutStreamBuf Obj;

const int INITIAL_BUFSIZE           = 256;
const int TWICE_INITIAL_BUFSIZE     = 512;
const int INITIAL_BUFSIZE_PLUS_44   = 300;  // arbitrary number in [1 .. 2k]
const int INITIAL_BUFSIZE_MINUS_ONE = 255;
const int INITIAL_BUFSIZE_PLUS_ONE  = 257;
const int TRIPLE_CAPACITY           = 768;
const int QUADRUPLE_CAPACITY        = 1024;

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

                   // ====================================
                   // operator<< for bdesb_MemOutStreamBuf
                   // ====================================

// FREE OPERATORS
bsl::ostream& operator<<(bsl::ostream&                stream,
                         const bdesb_MemOutStreamBuf& streamBuffer);
    // Write the contents of the specified 'streamBuffer' (as well as a marker
    // indicating eight bytes groupings) to the specified output 'stream' in
    // binary format, and return a reference to the modifiable 'stream'.

bsl::ostream& operator<<(bsl::ostream&                stream,
                         const bdesb_MemOutStreamBuf& streamBuffer)
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

///Usage
///-----
// This example demonstrates use of a stream buffer by a stream, in this case
// a stream with simple formatting requirements -- namely, capitalizing all
// lower-case ASCII character data that is output.  To simplify the example, we
// do not include the functions for streaming non-character data, e.g., numeric
// values:
//..
//  // my_capitalizingstream.h
//
    class my_CapitalizingStream {
        // This class capitalizes lower-case ASCII characters that are output.

        bdesb_MemOutStreamBuf d_streamBuf;  // buffer to write to

        friend
        my_CapitalizingStream& operator<<(my_CapitalizingStream&  stream,
                                          char                    data);
        friend
        my_CapitalizingStream& operator<<(my_CapitalizingStream&  stream,
                                          const char             *data);

      public:
        // CREATORS
        my_CapitalizingStream();
            // Create a capitalizing stream.

        ~my_CapitalizingStream();
            // Destroy this capitalizing stream.

        // ACCESSORS
        const bdesb_MemOutStreamBuf& streamBuf() { return d_streamBuf; }
            // Return the stream buffer used by this capitalizing stream.  Note
            // that this function is for debugging only.
    };

    // FREE OPERATORS
    my_CapitalizingStream& operator<<(my_CapitalizingStream&  stream,
                                      char                    data);
    my_CapitalizingStream& operator<<(my_CapitalizingStream&  stream,
                                      const char             *data);
        // Write the specified 'data' in capitalized form to the specified
        // capitalizing 'stream', and return a reference to the modifiable
        // 'stream'.

    my_CapitalizingStream::my_CapitalizingStream()
    {
    }

    my_CapitalizingStream::~my_CapitalizingStream()
    {
    }
//..
// As is typical, the streaming operators are made friends of the class.  We
// use the 'transform' algorithm to convert lower-case characters to uppercase:
//..
//  // my_capitalizingstream.cpp
//
//  #include <algorithm>
//
    // FREE OPERATORS
    my_CapitalizingStream& operator<<(my_CapitalizingStream& stream, char data)
    {
        stream.d_streamBuf.sputc(bsl::toupper(data));
        return stream;
    }

    my_CapitalizingStream& operator<<(my_CapitalizingStream&  stream,
                                      const char             *data)
    {
        bsl::string tmp(data);
        transform(tmp.begin(),
                  tmp.end(),
                  tmp.begin(),
                  (int(*)(int))bsl::toupper);
        stream.d_streamBuf.sputn(tmp.data(), tmp.length());
        return stream;
    }
//..
// Given the above two functions, we can now write 'main' as follows:
//..
//  // my_app.m.cpp
//
//  int main(int argc, char **argv)
//  {
//      my_CapitalizingStream cs;
//      cs << "Hello," << ' ' << "World." << '\0';
//
//      // Verify the results by writing to 'stdout'.
//      bsl::cout << cs.streamBuf().data() << bsl::endl;
//  }
//..
// Running the program above produces the following output on 'stdout':
//..
//  HELLO, WORLD.
//..

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;;

    switch (test) { case 0:
      case 11: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //   Simple example illustrating how one might use an output streambuf.
        //
        // Concerns:
        //
        // Plan:
        //
        // Tactics:
        //   - Add-Hoc Data Selection Method
        //   - Brute-Force implementation technique
        //
        // Testing:
        //   USAGE EXAMPLE: Capitalizing Stream
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "USAGE EXAMPLE" << endl
                                  << "=============" << endl;
        {
            my_CapitalizingStream cs;
            cs << "Hello," << ' ' << "World." << '\0';

            if (verbose) {
                // Visually verify that the streamed data has been capitalized.
                bsl::cout << cs.streamBuf().data() << bsl::endl;
            }

            ASSERT(0 == bsl::strcmp("HELLO, WORLD.", cs.streamBuf().data()));
        }
      } break;
      case 10: {
        // --------------------------------------------------------------------
        // RESET TEST
        //
        // Concerns:
        //   - Calling reset() on a default-constructed streambuf does nothing.
        //   - Calling reset() after reserveCapacity() returns deallocates
        //     reserved memory.
        //   - Calling reset() after adding output returns allocated memory
        //     and sets length() to zero.
        //   - The streambuf works normally after reset()
        //
        // Plan:
        //   - Create a test allocator.
        //   - Default-construct and reset a streambuf and verify that no
        //     memory is allocated and that length() remains zero.
        //   - Construct a streambuf with an initial capacity then reset().
        //     Verify that allocated memory is returned and that length()
        //     remains zero.
        //   - Construct a streambuf and add text to it, then reset().
        //     Verify that allocated memory is returned and that length()
        //     returns to zero.
        //   - In each case do some output after reset to verify function.
        //
        // Testing:
        //   void reset();
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "RESET TESTS" << endl
                                  << "===========" << endl;

        static const char DATA1[] = "hello";
        static const int DATA1_LEN = sizeof(DATA1) - 1; // No null terminator

        static const char DATA2[] = "goodbye";
        static const int DATA2_LEN = sizeof(DATA2) - 1; // No null terminator

        {
            // Reset after default construct.
            bslma_TestAllocator alloc;
            Obj mSB(&alloc);  const Obj& SB = mSB;
            ASSERT(0 == SB.length());
            ASSERT(0 == alloc.numBlocksTotal());

            mSB.reset();
            ASSERT(0 == SB.length());
            ASSERT(0 == alloc.numBlocksTotal());
            ASSERT(0 == SB.data());

            mSB.sputn(DATA2, DATA2_LEN);
            ASSERT(DATA2_LEN == SB.length());
            ASSERT(1 == alloc.numBlocksInUse());
            ASSERT(0 != SB.data() && 0 == memcmp(SB.data(), DATA2, DATA2_LEN));
        }

        {
            // Reset after initial capacity
            bslma_TestAllocator alloc;
            Obj mSB(50, &alloc);  const Obj& SB = mSB;
            ASSERT(0 == SB.length());
            ASSERT(1 == alloc.numBlocksInUse());
            ASSERT(0 != SB.data());

            mSB.reset();
            ASSERT(0 == SB.length());
            ASSERT(0 == alloc.numBlocksInUse());
            ASSERT(0 == SB.data());

            mSB.sputn(DATA2, DATA2_LEN);
            ASSERT(DATA2_LEN == SB.length());
            ASSERT(1 == alloc.numBlocksInUse());
            ASSERT(0 != SB.data() && 0 == memcmp(SB.data(), DATA2, DATA2_LEN));
        }

        {
            // Reset after output
            bslma_TestAllocator alloc;
            Obj mSB(&alloc);  const Obj& SB = mSB;

            mSB.sputn(DATA1, DATA1_LEN);
            ASSERT(DATA1_LEN == SB.length());
            ASSERT(1 == alloc.numBlocksInUse());
            ASSERT(0 != SB.data() && 0 == memcmp(SB.data(), DATA1, DATA1_LEN));

            mSB.reset();
            ASSERT(0 == SB.length());
            ASSERT(0 == alloc.numBlocksInUse());
            ASSERT(0 == SB.data());

            mSB.sputn(DATA2, DATA2_LEN);
            ASSERT(DATA2_LEN == SB.length());
            ASSERT(1 == alloc.numBlocksInUse());
            ASSERT(0 != SB.data() && 0 == memcmp(SB.data(), DATA2, DATA2_LEN));
        }

      } break;
      case 9: {
        // --------------------------------------------------------------------
        // SEEK TEST
        //
        // Concerns:
        //   - that seeking relative to a specification ('seekoff') uses
        //      the correct location from which to offset
        //   - that both negative and positive offsets compute correctly
        //   - that seeking positions the "cursor" (i.e., the base-class'
        //      pptr()) at the correct location
        //   - that seeking out of bounds does not throw or abort or crash
        //      the program
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
        char mFILL[INITIAL_BUFSIZE];

        // Fill mFILL with every printable ASCII character except space and '!'
        const int CHAR_RANGE = '~' - '!';
        for (int i = 0; i > INITIAL_BUFSIZE; ++i) {
            mFILL[i] = '"' + (i % CHAR_RANGE);
        }

        const char *const FILL = mFILL;
        const int IB = INITIAL_BUFSIZE;
        const int IBMO = INITIAL_BUFSIZE_MINUS_ONE;
        const int IBPO = INITIAL_BUFSIZE_PLUS_ONE;

        {
            static const struct {
                int           d_line;       // line number
                int           d_areaFlags;  // "put" area or "get" area
                Obj::pos_type d_amount;     // amount to seek
                int           d_base;       // seekoff from where?
                int           d_retVal;     // expected return value
            } DATA[] = {
               //L#  area                      final
               //    flag   amount     base    position/ retVal
               //--  ----   -------    ----    ----------------
               // seekoff from the start of the streambuf
               { L_,  OUT,   -2,       BEG,      -1   },
               { L_,  OUT,    0,       BEG,      0    },
               { L_,  OUT,   10,       BEG,      10   },
               { L_,  OUT,   IBMO,     BEG,      IBMO },
               { L_,  OUT,   IB,       BEG,      IB   },
               { L_,  OUT,   IBPO,     BEG,      -1   },
               { L_,  OUT,   500,      BEG,      -1   },

               // seekoff in the "get" area
               { L_,  IN,    22,       BEG,      -1  },

               // seekoff from the end of the streambuf
               { L_,  OUT,  -300,      END,      -1  },
               { L_,  OUT,  -IBMO,     END,      1   },
               { L_,  OUT,  -IB,       END,      0   },  // excessive but good
               { L_,  OUT,  -IBPO,     END,      -1 },
               { L_,  OUT,  -10,       END,      IB - 10 },
               { L_,  OUT,    0,       END,      IB  },
               { L_,  OUT,    1,       END,      -1 },


               // seekoff in the "get" area
               { L_,  IN,    22,       END,      -1  },

               // seekoff from the current cursor, where cur == end
               { L_,  OUT,  -300,      CUR,      -1  },
               { L_,  OUT,  -IBMO,     CUR,      1   },
               { L_,  OUT,  -IB,       CUR,      0   },  // excessive but good
               { L_,  OUT,  -IBPO,     CUR,      -1 },
               { L_,  OUT,  -10,       CUR,      IB - 10 },
               { L_,  OUT,    0,       CUR,      IB  },
               { L_,  OUT,    1,       CUR,      -1 },

               // seekoff in the "get" area
               { L_,  IN,    22,       CUR,      -1  }
            };
            const int DATA_LEN = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < DATA_LEN; ++i ) {
                const int LINE      = DATA[i].d_line;
                const int RET_VAL   = DATA[i].d_retVal;
                const int FINAL_POS = (0 <= RET_VAL ? RET_VAL : IB);

                Obj mSB;  const Obj& SB = mSB;
                Obj::int_type ret;
                mSB.sputn(FILL, INITIAL_BUFSIZE);

                ret = mSB.pubseekoff(DATA[i].d_amount,
                                 (bsl::ios_base::seekdir)DATA[i].d_base,
                                 (bsl::ios_base::openmode)DATA[i].d_areaFlags);

                // Assert return value, new position, and unchanged buffer.
                LOOP_ASSERT(LINE, RET_VAL == ret);
                LOOP_ASSERT(LINE, FINAL_POS == SB.length());
                LOOP_ASSERT(LINE, 0 == bsl::memcmp(FILL, SB.data(), IB));

                // Verify positioning by writing one char, and check the
                // char, its predecessor, and its successor.  (Except in
                // out of bounds conditions.)

                if (veryVerbose) P(ret);
                mSB.sputc('!');
                LOOP_ASSERT(LINE, '!' == SB.data()[FINAL_POS]);
                if (FINAL_POS > 0) {
                    LOOP_ASSERT(LINE,
                                FILL[FINAL_POS-1] == SB.data()[FINAL_POS-1]);
                }
                if (FINAL_POS < INITIAL_BUFSIZE - 1) {
                    LOOP_ASSERT(LINE,
                                FILL[FINAL_POS+1] == SB.data()[FINAL_POS+1]);
                }
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
                int         d_retVal;           // pptr() location after seek
            } DATA[] = {
               //L#  offset   startPoint   endPoint
               //--  ------   ----------   --------
               { L_,  -1,          0,         -1  },
               { L_,   0,          0,          0  },
               { L_,  INITIAL_BUFSIZE_MINUS_ONE,
                                   0,         -1  },
               { L_,  500,         0,         -1  },

               { L_,  -110,       95,        -1   },
               { L_,  -96,        95,        -1   },
               { L_,  -95,        95,         0   },
               { L_,  -94,        95,         1   },
               { L_,  -20,        95,         75  },
               { L_,   0,         95,         95  },
               { L_,   1,         95,         -1  },
               { L_,   31,        95,         -1  },
               { L_,   200,       95,         -1  }
            };
            const int DATA_LEN = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < DATA_LEN; ++i ) {
                const int LINE      = DATA[i].d_line;
                const int RET_VAL   = DATA[i].d_retVal;
                const int INIT_POS  = DATA[i].d_initialPosition;
                const int FINAL_POS = (0 <= RET_VAL ? RET_VAL : INIT_POS);

                Obj mSB;  const Obj& SB = mSB;
                Obj::int_type ret;
                mSB.sputn(FILL, INITIAL_BUFSIZE);

                mSB.pubseekpos(INIT_POS);
                ret = mSB.pubseekoff(DATA[i].d_offset,
                                     (bsl::ios_base::seekdir)CUR,
                                     (bsl::ios_base::openmode)OUT );
                if (veryVerbose) {
                    P(ret);
                }

                // Assert return value, new position, and unchanged buffer.
                LOOP_ASSERT(LINE, RET_VAL == ret);
                LOOP_ASSERT(LINE, FINAL_POS == SB.length());
                LOOP_ASSERT(LINE, 0 == bsl::memcmp(FILL, SB.data(), IB));

                // Verify positioning by writing one char, and check the
                // char, its predecessor, and its successor.  (Except in
                // out of bounds conditions.)

                if (veryVerbose) P(ret);
                mSB.sputc('!');
                LOOP_ASSERT(LINE, '!' == SB.data()[FINAL_POS]);
                if (FINAL_POS > 0) {
                    LOOP_ASSERT(LINE,
                                FILL[FINAL_POS-1] == SB.data()[FINAL_POS-1]);
                }
                if (FINAL_POS < INITIAL_BUFSIZE - 1) {
                    LOOP_ASSERT(LINE,
                                FILL[FINAL_POS+1] == SB.data()[FINAL_POS+1]);
                }
            }
        }
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // PUT string TEST
        //
        // Concerns:
        //   - that string of varying lengths are written correctly
        //   - that writing strings does not overwrite existing buffer
        //      contents
        //   - that no more than the specified number of characters are written
        //   - that writing beyond existing capacity is handled correctly
        //
        // Plan:
        //   Write out representative strings from the equivalence classes 0
        //   characters, 1 character, and > 1 character, into streambufs with
        //   representative contents "empty", substantially less than
        //   capacity, and almost-full-so-that-next-write-exceeds-capacity.
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

        if (verbose) cout << "\nTesting sputn." << endl;
        {
            const int IBMO = INITIAL_BUFSIZE_MINUS_ONE;

            char FILL[2001];     bsl::memset(FILL, 'a', 2001);
            char abcFILL[2004];  bsl::memset(abcFILL + 3, 'a', 2001);
            abcFILL[0] = 'a';
            abcFILL[1] = 'b';
            abcFILL[2] = 'c';
            char CAPACITYFILL[INITIAL_BUFSIZE];
            bsl::memset(CAPACITYFILL, 'a', IBMO);  CAPACITYFILL[IBMO] = 0;
            const char *CF = CAPACITYFILL;

            char RES1[INITIAL_BUFSIZE];
            bsl::memset(RES1, 'a', INITIAL_BUFSIZE_MINUS_ONE);
            RES1[INITIAL_BUFSIZE_MINUS_ONE] = 's';

            char RES2[2255];      bsl::memset(RES2, 'a', 2255);
            char RES3[IBMO + 3];  bsl::memset(RES3, 'a', IBMO);
            RES3[IBMO]     = 'a';
            RES3[IBMO + 1] = 'b';
            RES3[IBMO + 2] = 'c';

            static const struct {
                int         d_line;          // line number
                const char *d_outStr;        // string to output from
                int         d_outLen;        // num chars to write
                const char *d_initialCont;   // initial contents of stream
                const char *d_result;        // expected contents after 'sputn'
                int         d_resultLen;     // how much to compare
            } DATA[] = {
               //L#   out    out  initial  result   result
               //    strng length content content   length
               //--  ----- ------ ------- -------   -------
               { L_,  "",     0,     "",   "",     0           },
               { L_,  "s",    1,     "",   "s",    1           },
               { L_,  "abc",  3,     "",   "abc",  3           },
               { L_,  FILL,  2000,   "",   FILL,   2000        },

               { L_,  "",     0,     "a",  "a",    1           }, //xtra
               { L_,  "s",    1,     "a",  "as",   2           },
               { L_,  "abc",  3,     "a",  "aabc", 4           },
               { L_,  FILL,  2000,   "a",  FILL,   2001        },

               // this entire quad is extra
               { L_,  "",     0,    "abc", "abc",    3         },
               { L_,  "s",    1,    "abc", "abcs",   4         },
               { L_,  "abc",  3,    "abc", "abcabc", 6         },
               { L_,  FILL,  2000,  "abc", abcFILL,  2003      },

               { L_,  "",     0,     CF,   RES1,   IBMO        },
               { L_,  "s",    1,     CF,   RES1,   IBMO + 1    },
               { L_,  "abc",  3,     CF,   RES3,   IBMO + 3    },
               { L_,  FILL, 2000,    CF,   RES2,   IBMO + 2000 }
            };

            const int DATA_LEN = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < DATA_LEN; ++i ) {
                const int LINE = DATA[i].d_line;

                Obj mSB;  const Obj& SB = mSB;

                for (unsigned j = 0; j < strlen(DATA[i].d_initialCont); ++j) {
                    mSB.sputc(DATA[i].d_initialCont[j]);
                }
                int retResult = mSB.sputn(DATA[i].d_outStr, DATA[i].d_outLen);
                LOOP_ASSERT(LINE, 0 == strncmp(SB.data(), DATA[i].d_result,
                                                       DATA[i].d_resultLen));
                LOOP_ASSERT(LINE, DATA[i].d_outLen == retResult);
                LOOP_ASSERT(LINE, DATA[i].d_resultLen == SB.length())

                if (veryVerbose) { cout << '\t'; P(SB.length()); }
            }
        }
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // Reserve Capacity
        //
        // Concerns:
        //   - that the method obtains at least as much (total) capacity as
        //      specified
        //   - that growth is geometric (i.e., always increases by a fixed
        //      multiplicative factor).
        //   - that if the requested capacity is less than the current
        //      capacity, no internal state changes as a result of method
        //      execution (i.e., it is effectively a no-op).
        //
        // Plan:
        //   Test reserveCapacity with each of the following properties:
        //    - a request asking for less than the present capacity
        //    - a request for exactly the current capacity
        //    - a request for one more than current capacity
        //    - a request for triple the current capacity (to ensure that the
        //       resulting capacity is actually quadruple the initial, since we
        //       expect geometric growth with multiplicative factor 2.)
        //
        // Tactics:
        //   - Category-Partitioning Data Selection Method
        //   - Table-Based Implementation Technique
        //
        // Testing:
        //   void reserveCapacity(int numElements);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "RESERVE CAPACITY TEST" << endl
                          << "=====================" << endl;

        if (verbose) cout <<
        "\nTesting reserveCapacity in streambuf with default initial capacity."
                          << endl;
        {
            const int IBMO = INITIAL_BUFSIZE_MINUS_ONE;
            const int IBPO = INITIAL_BUFSIZE_PLUS_ONE;

            // Data structure that contains testing data for testing
            // 'reserveCapacity'.
            static const struct {
                int d_line;          // line number
                int d_requestAmount; // how many bytes to ask for
                int d_capacity;      // expected streambuf capacity
            } DATA[] = {
                  //L#  Request Amount     Resulting Capacity
                  //--  --------------     ------------------
                  // Ask for less than current capacity
                  { L_,       0,           INITIAL_BUFSIZE       },
                  { L_,      IBMO,         INITIAL_BUFSIZE       },

                  // Ask for exactly current capacity
                  { L_,  INITIAL_BUFSIZE,  INITIAL_BUFSIZE       },

                  // Ask for one more than current capacity
                  { L_,      IBPO,         IBPO                  },

                  // Ask for triple current capacity
                  { L_,  TRIPLE_CAPACITY,  TRIPLE_CAPACITY    }
            };   // end table DATA


            const int DATA_LEN = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < DATA_LEN; ++i ) {
                const int LINE = DATA[i].d_line;

                bslma_TestAllocator ta(veryVeryVerbose);
                Obj mSB(&ta);
                mSB.sputc('Z');
                LOOP_ASSERT(LINE, INITIAL_BUFSIZE == ta.numBytesInUse());
                if (veryVerbose) cout << "\tRequesting capacity of "
                                      << DATA[i].d_requestAmount << '.'
                                      << endl;
                mSB.reserveCapacity(DATA[i].d_requestAmount);
                LOOP_ASSERT(LINE, DATA[i].d_capacity == ta.numBytesInUse());

                // Now prove that the entirety of the returned memory is
                // used for capacity (rather than, say, other object
                // infrastructure) by writing out that many characters and
                // showing that no realloc happens.

                for (int j = 1; j < DATA[i].d_capacity; ++j) {
                    mSB.sputc('Z');
                }
                LOOP_ASSERT(LINE, DATA[i].d_capacity == ta.numBytesInUse());
            }
        }

        if (verbose) cout <<
         "\nTesting reserveCapacity in streambuf with initial capacity of one."
                          << endl;

        {
            // Data structure that contains testing data for testing
            // 'reserveCapacity'.
            static const struct {
                int d_line;          // line number
                int d_requestAmount; // how many bytes to ask for
                int d_capacity;      // expected streambuf capacity
            } DATA[] = {
                  //L#  Request Amount     Resulting Capacity
                  //--  --------------     ------------------
                  // Ask for less than current capacity
                  { L_,       0,                 1 },

                  // Ask for exactly current capacity
                  { L_,       1,                 1 },

                  // Ask for one more than current capacity
                  { L_,       2,                 2 },

                  // Ask for triple current capacity
                  { L_,       3,                 3 }
            };   // end table DATA

            const int DATA_LEN = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < DATA_LEN; ++i ) {
                const int LINE = DATA[i].d_line;

                bslma_TestAllocator ta(veryVeryVerbose);
                Obj mSB(1, &ta);
                LOOP_ASSERT(LINE, 1 == ta.numBytesInUse());
                if (veryVerbose) cout << "\tRequesting capacity of "
                                      << DATA[i].d_requestAmount << '.'
                                      << endl;
                mSB.reserveCapacity(DATA[i].d_requestAmount);
                LOOP_ASSERT(LINE, DATA[i].d_capacity == ta.numBytesInUse());

                // Now prove that the entirety of the returned memory is
                // used for capacity (rather than, say, other object
                // infrastructure) by writing out that many characters and
                // showing that no realloc happens.

                for (int j = 0; j < DATA[i].d_capacity; ++j) {
                    mSB.sputc('Z');
                }
                LOOP_ASSERT(LINE, DATA[i].d_capacity == ta.numBytesInUse());
            }
        }
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // NON-PRIMARY CONSTRUCTORS AND DESTRUCTOR.
        //
        // Concerns:
        //   - That the initial capacity for the constructed streambuf is
        //     equal to the requested initial capacity.
        //   - That the specified or bslma_Default::defaultAllocator is used.
        //   - That the destructor cleans up properly, in particular, returns
        //     the allocated memory.
        //
        // Plan:
        //   - Use 'bslma_TestAllocator' to verify that specified allocator
        //      is used.
        //   - Use 'bslma_TestAllocator' to verify initial memory request size.
        //   - Write out the requested-initial-capacity number of bytes,
        //      and verify that no reallocation is done.
        //   - Install a 'bslma_TestAllocator' in 'bdema_DefaultAllocator' to
        //      verify that 'bslma_Default::defaultAllocator()' is used by
        //      default.
        //   - Allow the destructor to execute and verify (from TestAllocator)
        //      that all memory has been released.
        //
        // Tactics:
        //   - Ad-Hoc Data Selection Method
        //   - Brute-Force Techniques
        //
        // Testing:
        //   bdesb_MemOutStreamBuf(int numElements, *ba = 0);
        //   ~bdesb_MemOutStreamBuf();
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTest capacity-reserving constructors." << endl;

        static int DATA[] = {
            0, 2, 4, 8, 16, 32, 64, 128, 256, 512
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        const int BUFSIZE = 1024;
        char buffer[BUFSIZE];  bsl::memset(buffer, 'a', BUFSIZE);

        if (verbose) cout << "\nExplicitly specify allocator." << endl;
        {
            bslma_TestAllocator da(veryVeryVerbose);
            const int NUM_BLOCKS_DEFAULT = da.numBlocksInUse();
            ASSERT(0 == NUM_BLOCKS_DEFAULT);
            bslma_DefaultAllocatorGuard dag(&da);

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int CAPACITY = DATA[ti];

                bslma_TestAllocator ta(veryVeryVerbose);
                const int NUM_BLOCKS = ta.numBlocksInUse();
                ASSERT(0 == NUM_BLOCKS);
                {
                    Obj mSB(CAPACITY, &ta);
                    ASSERT(NUM_BLOCKS + 1     == ta.numBlocksInUse());
                    ASSERT(NUM_BLOCKS_DEFAULT == da.numBlocksInUse());

                    mSB.sputn(buffer, CAPACITY);
                    ASSERT(NUM_BLOCKS + 1     == ta.numBlocksInUse());
                    ASSERT(NUM_BLOCKS_DEFAULT == da.numBlocksInUse());
                }
                ASSERT(0 == NUM_BLOCKS);
            }
        }
        {
            bslma_TestAllocator da(veryVeryVerbose);
            const int NUM_BLOCKS_DEFAULT = da.numBlocksInUse();
            ASSERT(0 == NUM_BLOCKS_DEFAULT);
            bslma_DefaultAllocatorGuard dag(&da);

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int CAPACITY = DATA[ti];

                bslma_TestAllocator ta(veryVeryVerbose);
                const int NUM_BLOCKS = ta.numBlocksInUse();
                ASSERT(0 == NUM_BLOCKS);
                {
                    Obj mSB(CAPACITY, &ta);
                    ASSERT(NUM_BLOCKS + 1     == ta.numBlocksInUse());
                    ASSERT(NUM_BLOCKS_DEFAULT == da.numBlocksInUse());

                    mSB.sputn(buffer, CAPACITY);
                    ASSERT(NUM_BLOCKS + 1     == ta.numBlocksInUse());
                    ASSERT(NUM_BLOCKS_DEFAULT == da.numBlocksInUse());
                }
                ASSERT(0 == NUM_BLOCKS);
            }
        }

        if (verbose) cout << "\nUse default allocator." << endl;
        {
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int CAPACITY = DATA[ti];

                bslma_TestAllocator da(veryVeryVerbose);
                const int NUM_BLOCKS = da.numBlocksInUse();
                ASSERT(0 == NUM_BLOCKS);
                {
                    bslma_DefaultAllocatorGuard dag(&da);

                    Obj mSB(CAPACITY);
                    ASSERT(NUM_BLOCKS + 1 == da.numBlocksInUse());

                    mSB.sputn(buffer, CAPACITY);
                    ASSERT(NUM_BLOCKS + 1 == da.numBlocksInUse());
                }
                ASSERT(0 == NUM_BLOCKS);
            }
        }
        {
            bslma_TestAllocator da(veryVeryVerbose);
            const int NUM_BLOCKS_DEFAULT = da.numBlocksInUse();
            ASSERT(0 == NUM_BLOCKS_DEFAULT);
            bslma_DefaultAllocatorGuard dag(&da);

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int CAPACITY = DATA[ti];

                bslma_TestAllocator da(veryVeryVerbose);
                const int NUM_BLOCKS = da.numBlocksInUse();
                ASSERT(0 == NUM_BLOCKS);
                {
                    bslma_DefaultAllocatorGuard dag(&da);

                    Obj mSB(CAPACITY);
                    ASSERT(NUM_BLOCKS + 1 == da.numBlocksInUse());

                    mSB.sputn(buffer, CAPACITY);
                    ASSERT(NUM_BLOCKS + 1 == da.numBlocksInUse());
                }
                ASSERT(0 == NUM_BLOCKS);
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
        //   - that no additional characters are written after terminating
        //   - that these functions work on references to 'const' objects
        //   - that each return a reference to the modifiable stream argument
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
            if (verbose) cout << "\tChecking operator<< return value." << endl;
            const Obj SB;

            const int SIZE = 100;
            char buf1[SIZE], buf2[SIZE];
            ostrstream out1(buf1, SIZE), out2(buf2, SIZE);
            out1 << SB << "arbitrary";  // Ensure modifiable
            out2 << SB << "value";      // stream is returned.
        }

        {
            Obj mSB;  const Obj& SB = mSB;
            mSB.sputn("hello", 5);

            const int SIZE = 100;
            char buf1[SIZE], buf2[SIZE];
            bsl::memset(buf1, 'X', SIZE);
            bsl::memset(buf2, 'Y', SIZE);
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
            // One more test, just to see something different
            Obj SB;
            SB.sputc('0');  SB.sputc('1');  SB.sputc('2');  SB.sputc('3');
            SB.sputc('4');  SB.sputc('5');  SB.sputc('6');  SB.sputc('7');
            SB.sputc('8');  SB.sputc('9');  SB.sputc('A');  SB.sputc('B');

            const char *EXPECTED =
                                 "\n0000\t00110000 00110001 00110010 00110011 "
                                         "00110100 00110101 00110110 00110111"
                                 "\n0008\t00111000 00111001 01000001 01000010";
            char ACTUAL[TWICE_INITIAL_BUFSIZE];
            bsl::memset(ACTUAL, 255, TWICE_INITIAL_BUFSIZE);
            ostrstream out(ACTUAL, TWICE_INITIAL_BUFSIZE);
            out << SB << ends;

            const int LEN = strlen(EXPECTED) + 1;
            if (verbose) {
                cout << endl;
                T_ P(EXPECTED)
                T_ P(ACTUAL)
            }
            ASSERT((char)255 == ACTUAL[LEN]);
                                                        // check for overrun
            ASSERT(0 == memcmp(ACTUAL, EXPECTED, LEN));
        }
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // PRIMARY MANIPULATORS/BASIC ACCESSORS
        //   Verify the most basic functionality of a 'bdesb_MemOutStreamBuf'.
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
        //      correct location
        //   - that seeking out of bounds does not throw or abort or crash
        //      the program
        //   - that trying to seek in the "get" area has no effect
        //
        //    (data & length)
        //   - To ensure that accessors work off of references to 'const'
        //      objects.
        //   - That data return the address of the underlying character array.
        //   - That length return the number of characters written to the
        //      stream buffer.
        //
        // Plan:
        //   (sputc)
        //   - Write each range-endpoint from each category of characters
        //      using the 'sputc' method, and verify that the bit pattern for
        //      that character is present and correct in the stream buffer.
        //
        //   (seekpos)
        //   - Seek to the following equivalence classes: before the start of
        //      the buffer, inside the buffer, and after the end of the buffer.
        //      Also try seeking in the "get" area.
        //
        //   (data and length)
        //   - Create an empty 'bdesb_MemOutStreamBuf' and verify its length.
        //   - Add a character, and verify the length and content.
        //   - Add enough characters to use all the initial capacity, and then
        //      verify length and content.
        //   - Then add one more character so as to exceed initial capacity,
        //      and verify length and content.
        //
        // Tactics:
        //   'sputc':
        //     - Category-Partitioning Data Selection Method
        //     - Table-Based and Brute-Force Implementation Technique
        //   'seekpos':
        //     - Category-Partitioning Data Selection Method
        //     - Table-Based Techniques
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

        if (verbose) cout << "\nEmpty streambuf." << endl;
        {
            bslma_TestAllocator da(veryVeryVerbose);
            const Obj SB(&da);
            ASSERT(0 == SB.length());
            ASSERT(SB.data() == da.lastAllocatedAddress());

            if (veryVerbose) P(SB);
        }

        if (verbose) cout << "\nTesting sputc." << endl;
        {
            typedef Obj::char_type T;

            static const struct {
                int            d_line;     // line number
                T              d_outChar;  // character to output
                const char    *d_result;   // expected contents after 'sputc'
            } DATA[] = {
                  //L#  output char    resulting
                  //                 stream contents
                  //--  -----------  ---------------
                  // Printing character equivalence classes are
                  // ranges [0..31], [32..126], and {127}.
                  { L_,    0,            "\x0"    },
                  { L_,    31,           "\x1F"   },
                  { L_,    32,           " "      },
                  { L_,   126,           "~"      },
                  { L_,   127,           "\x7F"   },

                  // Leading-bit equivalence classes are ranges
                  // [0..127] and [128..255]
                  { L_,      0,            "0"     },
                  { L_,    127,            "\x7F"  },
                  { L_,    (T)128,         "\x80"  },
                  { L_,    (T)255,         "\xFF"  }
            };   // end table DATA

            const int DATA_LEN = sizeof DATA / sizeof *DATA;

            // This loop verifies that 'sputc' both:
            //    1. adds the character, and
            //    2. does not overwrite beyond the character.

            if (verbose) {
                T_ cout << "Testing different character types." << endl;
            }

            for (int i = 0; i < DATA_LEN; ++i ) {
                const int LINE = DATA[i].d_line;

                Obj mSB;  const Obj& SB = mSB;
                mSB.sputc(DATA[i].d_outChar);
                LOOP_ASSERT(LINE, DATA[i].d_outChar == SB.data()[0]);
                LOOP_ASSERT(LINE, 1 == SB.length());
                if (veryVerbose) { P(SB); }
            }
        }
        {
            Obj mSB;  const Obj& SB = mSB;

            // Stream out INITIAL_BUFSIZE characters.

            if (verbose) {
                T_ cout << "Using 'sputc' to fill capacity." << endl;
            }

            ASSERT(0 == SB.length());
            for (int i = 0; i < INITIAL_BUFSIZE; ++i ) {
                mSB.sputc('a');
            }

            // Check results.

            ASSERT(INITIAL_BUFSIZE == SB.length());
            for (int i = 0; i < INITIAL_BUFSIZE; ++i ) {
                LOOP_ASSERT(i, 'a' == SB.data()[i]);
                if (verbose) P(SB.data()[i]);
            }
            if (veryVerbose) { P(SB); }

            // Add one, forcing an overflow.

            if (verbose) {
                T_ cout << "Using 'sputc' to exceed capacity." << endl;
            }

            mSB.sputc('a');
            ASSERT(INITIAL_BUFSIZE_PLUS_ONE == SB.length());
            ASSERT('a' == SB.data()[INITIAL_BUFSIZE]);
        }

        if (verbose) cout << "\nTesting seekpos." << endl;
        {
            const int OUT  = bsl::ios_base::out;
            const int IN   = bsl::ios_base::in;
            const int IB   = INITIAL_BUFSIZE;
            const int IBMO = INITIAL_BUFSIZE_MINUS_ONE;
            const int IBPO = INITIAL_BUFSIZE_PLUS_ONE;

            static const struct {
                int           d_line;       // line number
                int           d_areaFlags;  // "put" or "get" area
                Obj::pos_type d_amount;     // amount to seek
                int           d_retVal;     // 'length()' location after seek
            } DATA[] = {
               //L#  area   amount  final
               //    flag           position/ retVal
               //--  ----   ------- ----------------
               // Seek to before the start of the buffer.
               { L_,  OUT,   -1,       -1   },

               // Seek to the start of the buffer.
               { L_,  OUT,    0,        0   },

               // Seek to the last char of the buffer.
               { L_,  OUT,   IBMO,      IBMO  },

               // Seek to the end of the buffer.
               { L_,  OUT,   IB,        IB    },

               // Seek to past the end of the buffer.
               { L_,  OUT,   IBPO,       -1    },

               // Seek in the "get" area, not legal for output buffers.
               { L_,  IN,    138,        -1   }
            };

            const int DATA_LEN = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < DATA_LEN; ++i ) {
                const int LINE      = DATA[i].d_line;
                const int RET_VAL   = DATA[i].d_retVal;
                const int FINAL_POS = RET_VAL >= 0 ? RET_VAL : INITIAL_BUFSIZE;

                Obj mSB;  const Obj& SB = mSB;
                Obj::pos_type ret;
                for (int j = 0; j < INITIAL_BUFSIZE; ++j) {
                    mSB.sputc('a');
                }

                ret = mSB.pubseekpos(DATA[i].d_amount,
                                 (bsl::ios_base::openmode)DATA[i].d_areaFlags);

                if (veryVerbose) P(ret)

                LOOP_ASSERT(LINE, RET_VAL   == ret);
                LOOP_ASSERT(LINE, FINAL_POS == SB.length());

                // Verify positioning by writing one char, and check the
                // char, its predecessor, and its successor. (Except for
                // out-of-bounds positions).

                // Verify positioning by writing one char, and check the
                // char, its predecessor, and its successor.  (Except in
                // out of bounds conditions.)

                if (veryVerbose) P(ret);
                mSB.sputc('!');
                LOOP_ASSERT(LINE, '!' == SB.data()[FINAL_POS]);
                if (FINAL_POS > 0) {
                    LOOP_ASSERT(LINE, 'a' == SB.data()[FINAL_POS-1]);
                }
                if (FINAL_POS < INITIAL_BUFSIZE - 1) {
                    LOOP_ASSERT(LINE, 'a' == SB.data()[FINAL_POS+1]);
                }
            }
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // PRIMARY CONSTRUCTOR
        //   Ensure that this constructor is "wired-up" and defaults properly.
        //
        // Concerns:
        //   - That the default allocator comes from 'bslma_Default::allocator'
        //   - That changing the default allocator after construction has
        //      no effect on an existing 'bdesb_MemOutStreamBuf' object.
        //   - That the primary constructor initializes the streambuf to
        //      have capacity INITIAL_BUF_SIZE.
        //
        // Plan:
        //   - To ensure that the basic constructor takes its allocator by
        //      default from 'bslma_Default::allocator', install a separate
        //      object of 'bdem_TestAllocator' as the default allocator and
        //      use its object-specific statistics to verify that it is in
        //      fact the source of default allocations and deallocations.
        //   - To ensure that changing the default allocator has no effect
        //      on existing table objects, swap it out and then continue to
        //      allocate additional objects.
        //   - To ensure that initial capacity is INITIAL_BUFSIZE, use the
        //      'bslma_TestAllocator' and show that right after stream buffer
        //      construction, INITIAL_BUFSIZE bytes have been explicitly
        //      requested.  As additional evidence, write out INITIAL_BUFSIZE
        //      chars; show that no new blocks have been requested; then write
        //      an additional character, and show that another block has been
        //      requested as a consequence.
        //      Note that this test does not constitute proof, because the
        //      writing function has not been tested, and so cannot be relied
        //      upon completely.  We nonetheless need this further evidence
        //      because, although the test allocator can tell us that
        //      INITIAL_BUFSIZE bytes were obtained, it cannot tell us that
        //      the streambuf is allocating them all for 'capacity' (as
        //      opposed to, say, object infrastructure).
        //
        // Tactics:
        //   - Ad-Hoc Data Selection Method
        //   - Brute-Force Implementation Technique
        //
        // Testing:
        //   bdesb_MemOutStreamBuf(bslma_Allocator *basicAllocator = 0);
        //
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "PRIMARY CONSTRUCTOR" << endl
                          << "===================" << endl;

        if (verbose) cout <<
              "\nEnsure bdem_Default::allocator() is used by default." << endl;
        {

            bslma_TestAllocator da(veryVeryVerbose);

            if (verbose) cout <<
                       "\tInstall test allocator 'da' as the default." << endl;

            {
                const bslma_DefaultAllocatorGuard dag(&da);

                ASSERT(0 == da.numBlocksInUse());
                Obj x;
                const int NBT = da.numBlocksInUse();
                ASSERT(0 == NBT);

                bslma_TestAllocator ta(veryVeryVerbose);
                ASSERT(0 == ta.numBlocksInUse());

                Obj y(&ta);
                ASSERT(NBT == da.numBlocksInUse());
                ASSERT(NBT == ta.numBlocksInUse());

                bslma_TestAllocator oa(veryVeryVerbose);
                           // Installing this other allocator should have no
                           // effect on subsequent use of pre-existing objects.
                if (verbose) cout <<
                       "\tInstall test allocator 'oa' as the default." << endl;
                {
                    const bslma_DefaultAllocatorGuard oag(&oa);

                    ASSERT(0 == oa.numBlocksInUse());
                    Obj z;
                    ASSERT(NBT == oa.numBlocksTotal());

                    ASSERT(NBT == da.numBlocksTotal());
                    ASSERT(NBT == ta.numBlocksTotal());

                    const int WRITE_SIZE = 300;
                               // Note: An arbitrary number > INITIAL_BUFSIZE!
                    char bigBuf[WRITE_SIZE];
                    bsl::memset(bigBuf, 'a', WRITE_SIZE);
                    streamsize result;
                    result = x.sputn(bigBuf, WRITE_SIZE);
                                               // Note: method not yet tested.

                    ASSERT(NBT <  da.numBlocksTotal());
                    ASSERT(NBT == ta.numBlocksTotal());
                    ASSERT(NBT == oa.numBlocksTotal());
                    ASSERT(WRITE_SIZE == result);  // ensure 'sputn' succeeded

                    result = y.sputn(bigBuf, WRITE_SIZE);
                                                // Note: method not yet tested.

                    ASSERT(NBT <  da.numBlocksTotal());
                    ASSERT(NBT <  ta.numBlocksTotal());
                    ASSERT(NBT == oa.numBlocksTotal());
                    ASSERT(WRITE_SIZE == result);  // ensure 'sputn' succeeded

                    result = z.sputn(bigBuf, WRITE_SIZE);
                                                // Note: method not yet tested.

                    ASSERT(NBT <  da.numBlocksTotal());
                    ASSERT(NBT <  ta.numBlocksTotal());
                    ASSERT(NBT <  oa.numBlocksTotal());
                    ASSERT(WRITE_SIZE == result);  // ensure 'sputn' succeeded

                    ASSERT(ta.numBlocksTotal() == da.numBlocksTotal());
                    ASSERT(ta.numBlocksTotal() == oa.numBlocksTotal());
                }

                if (verbose) cout <<
                    "\tUn-install test allocator 'oa' as the default." << endl;

            }
            if (verbose) cout <<
                    "\tUn-install test allocator 'da' as the default." << endl;
        }

        if (verbose) cout <<
            "\nEnsure that stream buffers initially have INITIAL_BUFSIZE "
            "bytes by default."
                          << endl;

        {
            bslma_TestAllocator da(veryVeryVerbose);
            {
                const bslma_DefaultAllocatorGuard dag(&da);

                ASSERT(0 == da.numBlocksTotal());
                Obj x;
                const int NBT = da.numBlocksTotal();
                ASSERT(0 == NBT);
                ASSERT(0 == da.numBytesInUse());

                const int WRITE_SIZE = INITIAL_BUFSIZE;
                char bigBuf[WRITE_SIZE];
                bsl::memset(bigBuf, 'a', WRITE_SIZE);
                streamsize result = x.sputn(bigBuf, INITIAL_BUFSIZE);

                ASSERT(INITIAL_BUFSIZE == da.numBytesInUse());
                ASSERT(1 == da.numBlocksTotal());
                ASSERT(WRITE_SIZE == result);  // ensure 'sputn' succeeded

                Obj::int_type result2 = x.sputc('f');
                ASSERT(TWICE_INITIAL_BUFSIZE == da.numBytesInUse());
                ASSERT(2 == da.numBlocksTotal());
                ASSERT('f' == result2);
            }
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TEST APPARATUS
        //
        // Concerns:
        //   This test driver has no test apparatus.
        //
        // Plan:
        //   N/A
        //
        // Tactics:
        //   N/A
        //
        // Testing:
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TEST APPARATUS" << endl
                          << "==============" << endl;

        if (verbose) cout << endl
                          << "This test driver has no test apparatus." << endl;

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
            "\nMake sure we can create and use a 'bdesb_MemOutStreamBuf'."
                          << endl;
        {
            Obj mSB;  const Obj& SB = mSB;
            if (verbose) {
                 cout << "\tCreate a variable-capacity output stream buffer: ";
                 P(SB)
            }
            ASSERT(0 == SB.length());

            mSB.sputn("hello", 5);
            if (verbose) {
                cout << "\tWrite a string (five chars) to the stream buffer: ";
                P(SB)
            }
            ASSERT(5 == SB.length());
            ASSERT(0 == strncmp(SB.data(), "hello", 5));

            mSB.sputc('s');
            if (verbose) {
                cout << "\tWrite a single char to the stream buffer: ";
                P(SB)
            }
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
