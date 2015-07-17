// bdlsb_memoutstreambuf.t.cpp                                        -*-C++-*-
#include <bdlsb_memoutstreambuf.h>

#include <bsls_asserttest.h>
#include <bdls_testutil.h>

#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>

#include <bsl_algorithm.h>
#include <bsl_cctype.h>
#include <bsl_cstddef.h>
#include <bsl_cstring.h>
#include <bsl_iomanip.h>
#include <bsl_iostream.h>
#include <bsl_limits.h>
#include <bsl_map.h>
#include <bsl_ostream.h>
#include <bsl_string.h>
#include <bsl_strstream.h>

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

//=============================================================================
//                              TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// This test driver exercises all the protected virtual methods from the
// 'basic_streambuf' protocol that are overridden by the derived concrete class
// 'bdlsb::MemOutStreamBuf', as well as each new (non-protocol) public method
// added in the 'bdlsb::MemOutStreamBuf' class.
//
// Our goal here is to ensure that the implementations comply exactly with the
// IOStreams portion of the C++ standard where the standard explicitly defines
// behavior, and that they conform to a feasible interpretation of the standard
// as described in the function documentation, where the standard defined
// behavior only loosely.  For those methods that are not protocol defined, we
// check only compliance with the behavior as described in the function
// documentation.
//
// The protected methods (whose implementation or re-implementation define the
// behavior that differentiates one kind of stream buffer from another) are
// "driven", or invoked, by various (base-class) public methods provided for
// stream-buffer client use.  One recurrent concern in this test driver is to
// ensure correct interplay between these protected methods and the base-
// class-provided implementations that use them.
//
// Note that output operator used for test tracing purposes is tested in test
// case 3.
//
// Primary Constructors:
//: o MemOutStreamBuf(bslma::Allocator *basicAllocator = 0);
//
// Primary Manipulators:
//: o void reserveCapacity(int numElements);
//
// Basic Accessors:
//: o const char_type *data();
//: o streamsize length();
//-----------------------------------------------------------------------------
// CREATORS
// [ 2] MemOutStreamBuf(bslma::Allocator *basicAllocator = 0);
// [ 5] MemOutStreamBuf(int numElements, *ba = 0);
// [ 5] ~MemOutStreamBuf();
// MANIPULATORS
// [ 7] int_type overflow(int_type insertionChar = traits_type::eof());
// [ 8] pos_type seekpos(pos_type, openmode);
// [ 8] pos_type seekoff(off_type, seekdir, openmode);
// [ 6] streamsize xsputn(const char_type, streamsize);
// [ 2] void reserveCapacity(int numElements);
// [ 9] void reset();
// ACCESSORS
// [ 4] const char_type *data() const;
// [ 4] streamsize length() const;
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 3] TEST APPARATUS
// [10] USAGE EXAMPLE
//-----------------------------------------------------------------------------

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

#define ASSERT       BDLS_TESTUTIL_ASSERT
#define ASSERTV      BDLS_TESTUTIL_ASSERTV

#define LOOP_ASSERT  BDLS_TESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BDLS_TESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BDLS_TESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BDLS_TESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BDLS_TESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BDLS_TESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BDLS_TESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BDLS_TESTUTIL_LOOP6_ASSERT

#define Q            BDLS_TESTUTIL_Q   // Quote identifier literally.
#define P            BDLS_TESTUTIL_P   // Print identifier and value.
#define P_           BDLS_TESTUTIL_P_  // P(X) without '\n'.
#define T_           BDLS_TESTUTIL_T_  // Print a tab (w/o newline).
#define L_           BDLS_TESTUTIL_L_  // current Line number

// ============================================================================
//                  NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)
#define ASSERT_PASS(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
#define ASSERT_FAIL(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)
#define ASSERT_OPT_PASS(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS(EXPR)
#define ASSERT_OPT_FAIL(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL(EXPR)

#define ASSERT_SAFE_PASS_RAW(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS_RAW(EXPR)
#define ASSERT_SAFE_FAIL_RAW(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL_RAW(EXPR)
#define ASSERT_PASS_RAW(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS_RAW(EXPR)
#define ASSERT_FAIL_RAW(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL_RAW(EXPR)
#define ASSERT_OPT_PASS_RAW(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(EXPR)
#define ASSERT_OPT_FAIL_RAW(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(EXPR)

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

typedef bdlsb::MemOutStreamBuf Obj;

const bsl::size_t INITIAL_BUFSIZE           = 256;
const bsl::size_t TWICE_INITIAL_BUFSIZE     = INITIAL_BUFSIZE * 2;
const bsl::size_t INITIAL_BUFSIZE_PLUS_44   = INITIAL_BUFSIZE + 44;
                                               // arbitrary number in [1 .. 2k]

const bsl::size_t INITIAL_BUFSIZE_MINUS_ONE = INITIAL_BUFSIZE - 1;
const bsl::size_t INITIAL_BUFSIZE_PLUS_ONE  = INITIAL_BUFSIZE + 1;
const bsl::size_t TRIPLE_CAPACITY           = INITIAL_BUFSIZE * 3;
const bsl::size_t QUADRUPLE_CAPACITY        = INITIAL_BUFSIZE * 4;

BSLMF_ASSERT(sizeof(int) <= sizeof(bsl::size_t));

const bsl::size_t LARGE_CAPACITY            =
                 static_cast<bsl::size_t>(bsl::numeric_limits<int>::max()) + 1;

const bsl::size_t MAX_CAPACITY              =
                                       bsl::numeric_limits<bsl::size_t>::max();
//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

                   // =====================================
                   // operator<< for bdlsb::MemOutStreamBuf
                   // =====================================

// FREE OPERATORS
bsl::ostream& operator<<(bsl::ostream&                 stream,
                         const bdlsb::MemOutStreamBuf& streamBuffer);
    // Write the contents of the specified 'streamBuffer' (as well as a marker
    // indicating eight bytes groupings) to the specified output 'stream' in
    // binary format, and return a reference to the modifiable 'stream'.

bsl::ostream& operator<<(bsl::ostream&                 stream,
                         const bdlsb::MemOutStreamBuf& streamBuffer)
{
    const bsl::streamsize  len  = streamBuffer.length();
    const char            *data = streamBuffer.data();

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
//                               USAGE EXAMPLE
//-----------------------------------------------------------------------------
///Usage
///-----
// This section illustrates intended use of this component.
//
// This example demonstrates use of a stream buffer by a stream, in this case
// a stream with simple formatting requirements - namely, capitalizing all
// lower-case ASCII character data that is output.  To simplify the example, we
// do not include the functions for streaming non-character data, e.g., numeric
// values.
//
// First, we define a stream class, that will use our stream buffer:
//..
    class my_CapitalizingStream {
        // This class capitalizes lower-case ASCII characters that are output.

        bdlsb::MemOutStreamBuf d_streamBuf;  // buffer to write to

        friend
        my_CapitalizingStream& operator<<(my_CapitalizingStream&, char);
        friend
        my_CapitalizingStream& operator<<(my_CapitalizingStream&,
                                          const char *);

      public:
        // CREATORS
        my_CapitalizingStream();
            // Create a capitalizing stream.

        ~my_CapitalizingStream();
            // Destroy this capitalizing stream.

        // ACCESSORS
        const bdlsb::MemOutStreamBuf& streamBuf() { return d_streamBuf; }
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
//
//  #include <algorithm>
//
    // FREE OPERATORS
    my_CapitalizingStream& operator<<(my_CapitalizingStream& stream, char data)
    {
        stream.d_streamBuf.sputc(static_cast<char>(bsl::toupper(data)));
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

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    const int                 test = argc > 1 ? atoi(argv[1]) : 0;
    const bool             verbose = argc > 2;
    const bool         veryVerbose = argc > 3;
    const bool     veryVeryVerbose = argc > 4;
    const bool veryVeryVeryVerbose = argc > 5;

    (void) veryVeryVerbose;
    (void) veryVeryVeryVerbose;
    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
      case 10: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //   Extracted from component header file.
        //
        // Concerns:
        //: 1 The usage example provided in the component header file compiles,
        //:   links, and runs as shown.
        //
        // Plan:
        //: 1 Incorporate usage example from header into test driver, replace
        //:   leading comment characters with spaces, and replace 'assert' with
        //:   'ASSERT'.  (C-1)
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "USAGE EXAMPLE" << endl
                                  << "=============" << endl;
        {
/// Example 1: Basic Use of 'bdlsb::MemOutStreamBuf'
///- - - - - - - - - - - - - - - - - - - - - - - - -
//
// Now, we create an object of our stream and write some words to it:
//..
    my_CapitalizingStream cs;
    cs << "Hello," << ' ' << "World." << '\0';
//..
// Finally, we verify that the streamed data has been capitalized:
//..
    if (verbose) {
        // Visually verify that the streamed data has been capitalized.
        bsl::cout << cs.streamBuf().data() << bsl::endl;
    }

    ASSERT(0 == bsl::strcmp("HELLO, WORLD.", cs.streamBuf().data()));
//..
        }
      } break;
      case 9: {
        // --------------------------------------------------------------------
        // RESET TEST
        //
        // Concerns:
        //: 1 Calling 'reset' on a default-constructed streambuf has no effect.
        //:
        //: 2 Calling 'reset' for object with non-zero capacity deallocates
        //:   reserved memory.
        //:
        //: 3 Calling 'reset' after adding output returns allocated memory
        //:   and sets internal pointers to null.
        //:
        //: 4 The streambuf works normally after 'reset' invocation.
        //
        // Plan:
        //: 1 Create a test allocator.
        //:
        //: 2 Default-construct and reset a streambuf and verify that no
        //:   memory is allocated and that internal pointers are set to null.
        //:   (C-1,3)
        //:
        //: 3 Do some output after reset to verify object validity.  (C-4)
        //:
        //: 4 Construct a streambuf, write some text to it, and then reset.
        //:   Verify that allocated memory is returned and that internal
        //:   pointers are set to null.  (C-2..3)
        //:
        //: 5 Do some output after reset to verify object validity.  (C-4)
        //
        // Testing:
        //   void reset();
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "RESET TESTS" << endl
                                  << "===========" << endl;

        static const char        DATA[]   = "hello";
        static const bsl::size_t DATA_LEN = sizeof(DATA) - 1;
                                                          // No null terminator

        if (verbose) cout << "\nTesting reset for default-constructed object."
                          << endl;
        {
            bslma::TestAllocator ta;
            Obj                  mSB(&ta);
            const bsls::Types::Int64 ND = ta.numDeallocations();

            mSB.reset();
            ASSERT(0 == ta.numBlocksTotal());
            ASSERT(ND + 1 == ta.numDeallocations());
            ASSERT(0 == mSB.data());
            ASSERT(0 == mSB.length());

            mSB.sputn(DATA, DATA_LEN);
            ASSERT(DATA_LEN == mSB.length());
            ASSERT(1 == ta.numBlocksInUse());
            ASSERT(0 != mSB.data());
            ASSERT(0 == memcmp(mSB.data(), DATA, DATA_LEN));
        }

        if (verbose) cout << "\nTesting reset after some output."
                          << endl;
        {
            bslma::TestAllocator ta;
            Obj                  mSB(INITIAL_BUFSIZE, &ta);
            mSB.sputn(DATA, DATA_LEN);
            const bsls::Types::Int64 ND = ta.numDeallocations();

            mSB.reset();
            ASSERT(0 == ta.numBlocksInUse());
            ASSERT(ND + 1 == ta.numDeallocations());
            ASSERT(0 == mSB.data());
            ASSERT(0 == mSB.length());

            mSB.sputn(DATA, DATA_LEN);
            ASSERT(DATA_LEN == mSB.length());
            ASSERT(1 == ta.numBlocksInUse());
            ASSERT(0 != mSB.data());
            ASSERT(0 == memcmp(mSB.data(), DATA, DATA_LEN));
        }
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // TESTING 'seek' METHODS
        //   As the only action performed in 'seekpos' is the call for
        //   'seekoff' with predetermined second parameter, then we can test
        //   'seekpos' superficially.
        //   Note that 'seekoff' and 'seekpos' methods are called by base class
        //   methods 'pubseekoff' and 'pubseekpos'.
        //
        // Concerns:
        //: 1 Seeking uses the correct location from which to offset.
        //:
        //: 2 Both negative and positive offsets compute correctly.
        //:
        //: 3 Seeking sets the "cursor" (i.e., the base-class' pptr()) position
        //:   to the correct location.
        //:
        //: 4 Seeking out of bounds is handled correctly and returns invalid
        //:   value.
        //:
        //: 5 Trying to seek in the "get" area has no effect and returns
        //:   invalid value.
        //
        // Plan:
        //: 1 Perform a variety of seeks, using representative test vectors
        //:   from the cross-product of offset categories beginning-pointer,
        //:   current-pointer and end-pointer, with direction categories
        //:   negative-forcing-past-beginning, negative-falling-within-bounds,
        //:   0, positive-falling-within bounds, and positive-forcing-past-end.
        //:   (C-1..5)
        //:
        //: 2 Perform several 'seekpos' invocations with different initial
        //:    states of the tested object. (C-1)
        //
        // Testing:
        //   pos_type seekoff(off_type, seekdir, openmode);
        //   pos_type seekpos(pos_type, openmode);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "SEEK TEST" << endl
                          << "=============" << endl;
#ifdef IN // 'IN' and 'OUT'  are #define'd in a windows header
#undef IN
#undef OUT
#endif
        const int OUT = bsl::ios_base::out;
        const int IN  = bsl::ios_base::in;
        const int CUR = bsl::ios_base::cur;
        const int BEG = bsl::ios_base::beg;
        const int END = bsl::ios_base::end;
        char mFILL[INITIAL_BUFSIZE];

        // Fill mFILL with every printable ASCII character except space and '!'
        const int CHAR_RANGE = '~' - '!';
        for (bsl::size_t i = 0; i > INITIAL_BUFSIZE; ++i) {
            mFILL[i] = static_cast<char>('"' + (i % CHAR_RANGE));
        }

        const char *const FILL = mFILL;
        const int IB = INITIAL_BUFSIZE;
        const int IBMO = INITIAL_BUFSIZE_MINUS_ONE;
        const int IBPO = INITIAL_BUFSIZE_PLUS_ONE;

        if (verbose) cout << "\nTesting seekoff from beginning and end."
                          << endl;
        {
            static const struct {
                int           d_line;       // line number
                int           d_areaFlags;  // "put" area or "get" area
                Obj::pos_type d_amount;     // amount to seek
                int           d_base;       // seekoff from where?
                int           d_retVal;     // expected return value (final
                                            // position)
            } DATA[] = {
               //LINE  AREA                  RETURN
               //      FLAG   AMOUNT  BASE   VALUE
               //----  ----   ------  ----   --------
               // seekoff from the start of the streambuf
               { L_,   OUT,   -2,     BEG,   -1       },
               { L_,   OUT,    0,     BEG,    0       },
               { L_,   OUT,    10,    BEG,    10      },
               { L_,   OUT,    IBMO,  BEG,    IBMO    },
               { L_,   OUT,    IB,    BEG,    IB      },
               { L_,   OUT,    IBPO,  BEG,   -1       },
               { L_,   OUT,    500,   BEG,   -1       },

               // seekoff in the "get" area
               { L_,   IN,     22,    BEG,   -1       },

               // seekoff from the end of the streambuf
               { L_,   OUT,   -300,   END,   -1       },
               { L_,   OUT,   -IBMO,  END,    1       },
               { L_,   OUT,   -IB,    END,    0       },  // excessive but good
               { L_,   OUT,   -IBPO,  END,   -1       },
               { L_,   OUT,   -10,    END,    IB - 10 },
               { L_,   OUT,    0,     END,    IB      },
               { L_,   OUT,    1,     END,   -1       },

               // seekoff in the "get" area
               { L_,   IN,     22,     END,  -1       },

               // seekoff from the current cursor, where cur == end
               { L_,   OUT,   -300,   CUR,   -1       },
               { L_,   OUT,   -IBMO,  CUR,    1       },
               { L_,   OUT,   -IB,    CUR,    0       },  // excessive but good
               { L_,   OUT,   -IBPO,  CUR,   -1       },
               { L_,   OUT,   -10,    CUR,    IB - 10 },
               { L_,   OUT,    0,     CUR,    IB      },
               { L_,   OUT,    1,     CUR,   -1       },

               // seekoff in the "get" area
               { L_,   IN,     22,    CUR,   -1       }
            };
            const size_t DATA_LEN = sizeof DATA / sizeof *DATA;

            for (size_t i = 0; i < DATA_LEN; ++i ) {
                const int    LINE      = DATA[i].d_line;
                const int    RET_VAL   = DATA[i].d_retVal;
                const size_t FINAL_POS = (0 <= RET_VAL ? RET_VAL : IB);

                Obj mSB;
                bsl::streamoff ret;
                mSB.sputn(FILL, INITIAL_BUFSIZE);

                ret = mSB.pubseekoff(
                                 DATA[i].d_amount,
                                 (bsl::ios_base::seekdir)DATA[i].d_base,
                                 (bsl::ios_base::openmode)DATA[i].d_areaFlags);

                // Assert return value, new position, and unchanged buffer.
                ASSERTV(LINE, RET_VAL == ret);
                ASSERTV(LINE, FINAL_POS == mSB.length());
                ASSERTV(LINE, 0 == bsl::memcmp(FILL, mSB.data(), IB));

                // Verify positioning by writing one char, and check the char,
                // its predecessor, and its successor.  (Except in out of
                // bounds conditions.)

                if (veryVerbose) P(ret);
                mSB.sputc('!');
                ASSERTV(LINE, '!' == mSB.data()[FINAL_POS]);
                if (FINAL_POS > 0) {
                    ASSERTV(LINE,
                            FILL[FINAL_POS-1] == mSB.data()[FINAL_POS-1]);
                }
                if (FINAL_POS < INITIAL_BUFSIZE - 1) {
                    ASSERTV(LINE,
                            FILL[FINAL_POS+1] == mSB.data()[FINAL_POS+1]);
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
               //LINE  OFFSET  INITIAL   RETURN
               //              POSITION  VALUE
               //----  ------  --------  --------
               { L_,   -1,     0,        -1   },
               { L_,    0,     0,         0   },
               { L_,  INITIAL_BUFSIZE_MINUS_ONE,
                               0,        -1   },
               { L_,    500,   0,        -1   },

               { L_,   -110,   95,       -1   },
               { L_,   -96,    95,       -1   },
               { L_,   -95,    95,        0   },
               { L_,   -94,    95,        1   },
               { L_,   -20,    95,        75  },
               { L_,    0,     95,        95  },
               { L_,    1,     95,       -1   },
               { L_,    31,    95,       -1   },
               { L_,    200,   95,       -1   }
            };
            const size_t DATA_LEN = sizeof DATA / sizeof *DATA;

            for (size_t i = 0; i < DATA_LEN; ++i ) {
                const int    LINE      = DATA[i].d_line;
                const int    RET_VAL   = DATA[i].d_retVal;
                const int    INIT_POS  = DATA[i].d_initialPosition;
                const size_t FINAL_POS = (0 <= RET_VAL ? RET_VAL : INIT_POS);

                Obj mSB;
                bsl::streamoff ret;
                mSB.sputn(FILL, INITIAL_BUFSIZE);
                ret = mSB.pubseekoff(INIT_POS,
                                     (bsl::ios_base::seekdir)BEG,
                                     (bsl::ios_base::openmode)OUT );

                ret = mSB.pubseekoff(DATA[i].d_offset,
                                     (bsl::ios_base::seekdir)CUR,
                                     (bsl::ios_base::openmode)OUT );
                if (veryVerbose) {
                    P(ret);
                }

                // Assert return value, new position, and unchanged buffer.
                ASSERTV(LINE, RET_VAL == ret);
                ASSERTV(LINE, FINAL_POS == mSB.length());
                ASSERTV(LINE, 0 == bsl::memcmp(FILL, mSB.data(), IB));

                // Verify positioning by writing one char, and check the
                // char, its predecessor, and its successor.  (Except in
                // out of bounds conditions.)

                if (veryVerbose) P(ret);
                mSB.sputc('!');
                ASSERTV(LINE, '!' == mSB.data()[FINAL_POS]);
                if (FINAL_POS > 0) {
                    ASSERTV(LINE,
                            FILL[FINAL_POS-1] == mSB.data()[FINAL_POS-1]);
                }
                if (FINAL_POS < INITIAL_BUFSIZE - 1) {
                    ASSERTV(LINE,
                            FILL[FINAL_POS+1] == mSB.data()[FINAL_POS+1]);
                }
            }
        }

        if (verbose) cout << "\nTesting seekpos." << endl;
        {
            const int BUFFER_SIZE = 10;
            char      mFILL[BUFFER_SIZE];
            bsl::memset(mFILL, 'a', BUFFER_SIZE);
            Obj           mSB;
            Obj::pos_type ret;
            mSB.sputn(mFILL, BUFFER_SIZE);

            ASSERT(-1 == mSB.pubseekpos(-1));

            for (int i = BUFFER_SIZE; i >= 0; --i ) {
                ret = mSB.pubseekpos(i);
                ASSERTV(i, i == ret);
            }
        }
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // OVERFLOW TEST
        //   'overflow' method is called to write a character when there are no
        //   writing positions available at the put pointer.  It allocates
        //   enough memory and then writes a character.  Memory is allocated by
        //   'grow' method and necessary amount is calculated by 'capacity'
        //   method.  Both of them have been tested already.  So we need just
        //   to test 'overflow' invocation and successful character placement.
        //   Note that protected 'overflow' method is called by base class
        //   method 'sputc'.
        //
        // Concerns:
        //: 1 'overflow' method is called when there are no writing positions
        //:   available.
        //:
        //: 2 Character is successfully written in result of method invocation.
        //
        // Plan:
        //: 1 Create an object with limited capacity, write enough characters
        //:   to fulfil the buffer, add one more.  Verify, that character has
        //:   been successfully written to the stream.  (C-1..2)
        //
        // Testing:
        //   int_type overflow(int_type insertionChar = traits_type::eof());
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "OVERFLOW TEST" << endl
                          << "=============" << endl;

        bslma::TestAllocator ta(veryVeryVerbose);
        Obj                  mSB(1, &ta);
        mSB.sputc('a');
        int result = mSB.sputc('b');
        ASSERT('b' == result);
        ASSERT('b' == mSB.data()[1]);
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // SPUTN TEST
        //   'xsputn' increases buffer size by calling 'grow' method in case of
        //   lack of space to write requested string.  We will test separately
        //   string writing and memory allocation.  Method 'grow', in its turn,
        //   calculates necessary amount of memory and calls method
        //   'reserveCapacity' that has been tested already.  So we need to
        //   test only memory amount calculation.
        //   Note that protected 'xsputn' method is called by base class method
        //   'sputn'.
        //
        // Concerns:
        //: 1 String of varying lengths are written correctly.
        //:
        //: 2 Writing strings does not overwrite existing buffer contents.
        //:
        //: 3 No more than the specified number of characters are written.
        //:
        //: 4 Writing beyond existing capacity is handled correctly.
        //:
        //: 5 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Create an object and write out several strings with sequentially
        //:   increasing lengths.  Verify, that all strings have been written
        //:   correctly.  (C-1..3)
        //:
        //: 2 Create an object and write out several strings with length
        //:   exceeding current capacity.  Verify that enough memory for string
        //:   storing has been allocated.  (C-4)
        //:
        //: 3 Verify that, in appropriate build modes, defensive checks are
        //:   triggered when an attempt is made to perform operations that
        //:   would overflow the valid range of 'Datetime' values.
        //:   (using the 'BSLS_ASSERTTEST_*' macros).  (C-5)
        //
        // Testing:
        //   streamsize xsputn(const char_type, streamsize);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "SPUTN TEST" << endl
                          << "==========" << endl;

        if (verbose) cout << "\nBasic sputn test." << endl;
        {
            Obj mSB(INITIAL_BUFSIZE);
            // 256 bytes are more than enough for 210 characters, so 'grow'
            // method will not be called in this test.

            char FILL[INITIAL_BUFSIZE];
            bsl::memset(FILL, 'a', INITIAL_BUFSIZE);
            size_t    summ = 0;
            const int NUM_ITERATIONS = 20;
            // The sum of first 20 terms of an arithmetic progression is 210

            for (int i = 0; i <= NUM_ITERATIONS; i++)
            {
                mSB.sputn(FILL, i);
                summ += i;
                ASSERTV(i, summ == mSB.length());
                ASSERTV(i, 0 == strncmp(mSB.data(), FILL, summ));
            }
        }

        if (verbose) cout << "\n\'grow\' method test." << endl;
        {
            const int IBMO  = INITIAL_BUFSIZE_MINUS_ONE;
            const int IBPO  = INITIAL_BUFSIZE_PLUS_ONE;
            const int HIBPO = (INITIAL_BUFSIZE / 2) + 1;

            char FILL[TWICE_INITIAL_BUFSIZE];
            bsl::memset(FILL, 'a', TWICE_INITIAL_BUFSIZE);

            static const struct {
                int    d_line;               // line number
                int    d_initialCapacity;    // initial object capacity
                int    d_numCharsToWrite;    // num chars to write
                size_t d_expectedCapacity;   // expected object capacity
            } DATA[] = {
               //LINE  INITIAL           CHARACTERS  EXPECTED
               //      CAPACITY          NUMBER      CAPACITY
               //----  ---------------   ----------  ---------------------
               { L_,   0,                1,          INITIAL_BUFSIZE       },
               { L_,   0,                IBMO,       INITIAL_BUFSIZE       },
               { L_,   0,                IBPO,       TWICE_INITIAL_BUFSIZE },
               { L_,   1,                HIBPO,      INITIAL_BUFSIZE       },
               { L_,   INITIAL_BUFSIZE,  IBPO,       TWICE_INITIAL_BUFSIZE },
            };

            const size_t DATA_LEN = sizeof DATA / sizeof *DATA;
            for (size_t i = 0; i < DATA_LEN; ++i ) {
                const int LINE = DATA[i].d_line;
                bslma::TestAllocator ta(veryVeryVerbose);
                Obj mSB(&ta);
                mSB.reserveCapacity(DATA[i].d_initialCapacity);

                mSB.sputn(FILL, DATA[i].d_numCharsToWrite);

                ASSERTV(
                     LINE,
                     DATA[i].d_expectedCapacity == ta.lastAllocatedNumBytes());
            }
        }

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertFailureHandlerGuard hG(
                                             bsls::AssertTest::failTestDriver);

            Obj mSB(INITIAL_BUFSIZE);
            ASSERT_SAFE_PASS(mSB.sputn("hello", 1));
            ASSERT_SAFE_FAIL(mSB.sputn("hello", -1));
        }
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // VALUE CONSTRUCTOR / DESTRUCTOR.
        //
        // Concerns:
        //: 1 Value constructor can create an object.
        //:
        //: 2 The specified or bslma::Default::defaultAllocator is used for
        //:   memory allocation.
        //:
        //: 3 The initial capacity for the constructed streambuf is equal to
        //:   the requested non-zero positive initial capacity.
        //:
        //: 4 An implementation-defined initial capacity is used if null or
        //:   negative capacity has been requested at object construction.
        //:
        //: 5 Changing the default allocator after construction has no effect
        //:   on an existing 'bdlsb::MemOutStreamBuf' object.
        //:
        //: 6 That the destructor cleans up properly, in particular, returns
        //:   the allocated memory.
        //
        // Plan:
        //: 1 Using the table-driven technique, specify a set of requested
        //:   capacity values for buffers and expected result values.
        //:
        //: 2 For each row 'R' in the table of P-1:
        //:
        //:   1 Create an 'bdlsb::MemOutStreamBuf' object with required
        //:     capacity.
        //:
        //:   2 Verify that memory has been allocated by default allocator.
        //:     (C-2)
        //:
        //:   3 Verify, that allocated memory size is equal to expected
        //:     streambuf capacity.  (C-3..4)
        //:
        //:   4 Using 'sputc' method write a symbol to streambuf.
        //:
        //:   5 Check the first byte of allocated memory to verify that that
        //:     all streambuf machinery has been set up properly.  (C-1)
        //:
        //: 3 Setup the default allocator.  Create an 'bdlsb::MemOutStreamBuf'
        //:   object with another allocator.  Verify that memory has been
        //:   allocated by allocator, specified at construction.  (C-2)
        //:
        //: 4 Setup the default allocator.  Create an 'bdlsb::MemOutStreamBuf'
        //:   object with it.  Swap the default allocator out and then continue
        //:   to allocate additional objects to ensure that its changing has no
        //:   effect on existing table objects.  (C-5)
        //:
        //: 5 Create an 'bdlsb::MemOutStreamBuf' object and let it go out of
        //:   scope.  verify that all memory has been released.  (C-6)
        //
        // Testing:
        //   MemOutStreamBuf(int numElements, *ba = 0);
        //   ~MemOutStreamBuf();
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "VALUE CONSTRUCTOR / DESTRUCTOR" << endl
                          << "==============================" << endl;

        if (verbose) cout << "\nTesting capacity-reserving constructor."
                          << endl;
        {
            static const struct {
                int                d_line;            // line number
                int                d_requestedCap;    // requested capacity
                bsls::Types::Int64 d_expectedCap;     // expected capacity
            } DATA[] = {
                //LINE  REQUESTED               EXPECTED
                //----  ---------------------   ---------------
                { L_,   0,                      INITIAL_BUFSIZE       },
                { L_,   1,                      1                     },
                { L_,   INITIAL_BUFSIZE,        INITIAL_BUFSIZE       },
                { L_,   TWICE_INITIAL_BUFSIZE,  TWICE_INITIAL_BUFSIZE },
            };   // end table DATA

            const size_t DATA_LEN = sizeof DATA / sizeof *DATA;

            for (size_t i = 0; i < DATA_LEN; ++i ) {
                const int LINE = DATA[i].d_line;

                bslma::TestAllocator da(veryVeryVerbose);
                ASSERT(0 == da.numBlocksInUse());
                bslma::DefaultAllocatorGuard dag(&da);

                Obj mSB(DATA[i].d_requestedCap);

                ASSERTV(LINE, DATA[i].d_expectedCap == da.numBytesTotal());
                ASSERTV(LINE, 1                     == da.numBlocksInUse());

                char *buffer = static_cast<char *>(da.lastAllocatedAddress());
                buffer[0] = 'a';
                mSB.sputc('Z');
                ASSERTV(LINE, 'Z' == buffer[0]);
            }
        }

        if (verbose) cout << "\nTesting explicitly specified allocator."
                          << endl;
        {
            bslma::TestAllocator da(veryVeryVerbose);
            ASSERT(0 == da.numBlocksInUse());
            bslma::DefaultAllocatorGuard dag(&da);

            {
                bslma::TestAllocator ta(veryVeryVerbose);
                ASSERT(0 == ta.numBlocksInUse());

                Obj mSB(INITIAL_BUFSIZE, &ta);
                ASSERT(1 == ta.numBlocksInUse());
                ASSERT(0 == da.numBlocksInUse());

                ASSERT(INITIAL_BUFSIZE == ta.numBytesTotal());
            }

            ASSERT(0 == da.numAllocations());
        }

        if (verbose) cout << "\nTesting default allocator substitution"
                              << endl;
        {
            bslma::TestAllocator da(veryVeryVerbose);
            const bslma::DefaultAllocatorGuard dag(&da);

            ASSERT(0 == da.numBlocksInUse());
            Obj x(INITIAL_BUFSIZE);
            const bsls::Types::Int64 xNBT  = da.numBytesTotal();
            const bsls::Types::Int64 xNBIU = da.numBlocksInUse();
            ASSERT(INITIAL_BUFSIZE == xNBT);
            ASSERT(1               == xNBIU);
            {
                bslma::TestAllocator ta(veryVeryVerbose);
                ASSERT(0 == ta.numBlocksInUse());
                const bslma::DefaultAllocatorGuard tag(&ta);

                Obj y(TWICE_INITIAL_BUFSIZE);
                ASSERT(xNBT  == da.numBytesTotal());
                ASSERT(xNBIU == da.numBlocksInUse());
                ASSERT(TWICE_INITIAL_BUFSIZE == ta.numBytesTotal());
                ASSERT(1                     == ta.numBlocksInUse());

                if (verbose) cout <<
                    "\tUn-install test allocator 'ta' as the default." << endl;
            }
            if (verbose) cout <<
                    "\tUn-install test allocator 'da' as the default." << endl;
        }

        if (verbose) cout << "\nTesting destructor." << endl;
        {
            bslma::TestAllocator da(veryVeryVerbose);
            const bslma::DefaultAllocatorGuard dag(&da);

            ASSERT(0 == da.numBlocksInUse());
            {
                Obj x(INITIAL_BUFSIZE);

                ASSERT(INITIAL_BUFSIZE == da.numBytesInUse());
                ASSERT(1               == da.numBlocksInUse());
            }

            ASSERT(0 == da.numBytesInUse());
            ASSERT(0 == da.numBlocksInUse());
        }
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // BASIC ACCESSORS
        //   Verify the basic accessors functionality.
        //
        // Concerns:
        //: 1 Accessors work off of references to 'const' objects.
        //:
        //: 2 'data' returns the address of the underlying character array.
        //:
        //: 3 'length' returns the number of characters written to the stream
        //:   buffer.
        //
        // Plan:
        //: 1 Create an empty 'bdlsb::MemOutStreamBuf' and verify 'data' and
        //:   'length' methods return values.  (C-2..3)
        //:
        //: 2 Create a constant reference to this object and verify 'data' and
        //:   'length' methods return values.  (C-1)
        //:
        //: 3 Add some characters to the initial streambuf.  Verify 'length'
        //:   return value and character buffer content, 'data' pointing to.
        //:   (C-2..3)
        //
        // Testing:
        //   const char_type *data() const;
        //   streamsize length() const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BASIC ACCESSORS" << endl
                          << "===============" << endl;

        if (verbose) cout << "\nEmpty streambuf." << endl;

        bslma::TestAllocator da(veryVeryVerbose);

        Obj mSB(&da);
        mSB.reserveCapacity(INITIAL_BUFSIZE);
        ASSERT(0 == mSB.length());
        ASSERT(mSB.data() == da.lastAllocatedAddress());

        const Obj& SB = mSB;
        ASSERT(0 == SB.length());
        ASSERT(SB.data() == da.lastAllocatedAddress());

        if (verbose) cout << "\nAdding some characters." << endl;

        mSB.sputc('h');
        mSB.sputc('e');
        mSB.sputc('l');
        mSB.sputc('l');
        mSB.sputc('o');

        ASSERT(5 == mSB.length());
        ASSERT(0 == memcmp(mSB.data(), "hello", 5));

        if (veryVerbose) P(mSB);
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TEST APPARATUS
        //   Verify the auxiliary function used in the test driver. Note that
        //   the tested function is not part of the component and use only to
        //   provide human readable test traces.
        //
        // Concerns:
        //: 1 Output operator formats the stream buffer correctly.
        //:
        //: 2 Output operator does not produce any trailing characters.
        //:
        //: 3 Output operator works on references to 'const' object.
        //:
        //: 4 Output operator returns a reference to the modifiable stream
        //:   argument.
        //
        // Plan:
        //: 1 Create a 'bdlbs::MemOutStreamBuf' object and write some
        //:   characters to it.  Use 'ostrstream' to write that object's value
        //:   to two separate character buffers each with different initial
        //:   values.  Compare the contents of these buffers with the literal
        //:   expected output format and verify that the characters beyond the
        //:   length of the streambuf contents are unaffected in both buffers.
        //:   (C-1..3)
        //:
        //: 2 Create a 'bdlbs::MemOutStreamBuf' object.  Use 'ostrstream' to
        //:   write that object's value and some characters in consecutive
        //:   order.  (C-4)
        //
        // Testing:
        //   ostream& operator<<(ostream&, const MemOutStreamBuf&);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TEST APPARATUS" << endl
                          << "==============" << endl;

        if (verbose) cout << "\nChecking correctness of value formatting."
                          << endl;
        {
            Obj        mSB;
            const Obj& SB = mSB;
            mSB.reserveCapacity(INITIAL_BUFSIZE);
            mSB.sputc('h');
            mSB.sputc('e');
            mSB.sputc('l');
            mSB.sputc('l');
            mSB.sputc('o');

            const int SIZE = 100;
            char      buf1[SIZE];
            char      buf2[SIZE];
            bsl::memset(buf1, 'X', SIZE);
            bsl::memset(buf2, 'Y', SIZE);
            ostrstream out1(buf1, SIZE);
            ostrstream out2(buf2, SIZE);
            out1 << mSB;
            out2 << SB;
            char *endOfString = strchr(buf1, 'X');
            size_t stringLen = endOfString - buf1;
            ASSERT(0   == memcmp(buf1, buf2, stringLen));
            ASSERT('X' == buf1[stringLen]);
            ASSERT('Y' == buf2[stringLen]);

            const char *EXPECTED =
                        "\n0000\t01101000 01100101 01101100 01101100 01101111";
            ASSERT(0 == strncmp(buf1, EXPECTED, stringLen));
            buf1[stringLen] = 0;
            if (verbose) {
                T_ P(buf1)
            }
        }

        if (verbose) cout << "\tChecking operator<< return value." << endl;
        {
            const Obj  SB;
            const int  SIZE = 100;
            char       buf[SIZE];
            ostrstream out(buf, SIZE);

            out << SB << "arbitrary value";  // Ensure modifiable stream is
                                             // returned.
        }

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // PRIMARY MANIPULATORS
        //   Ensure that this constructor is "wired-up" and defaults properly.
        //
        //   Note that this test does not constitute proof, because the writing
        //   function has not been tested, and so cannot be relied upon
        //   completely.
        //
        // Concerns:
        //: 1 Value constructor can create an object.
        //:
        //: 2 The default allocator comes from 'bslma::Default::allocator'.
        //:
        //: 3 Changing the default allocator after construction has no effect
        //:   on an existing 'bdlsb::MemOutStreamBuf' object.
        //:
        //: 4 Method 'reserveCapacity' obtains as much (total) capacity as
        //:   specified.
        //:
        //: 5 Method 'capacity' returns correct amount of reserved memory.  We
        //:   can't test private method directly, so we will check next
        //:   statement: if the requested capacity is less than the current
        //:   capacity (calculated by 'capacity' method, no internal state
        //:   changes as a result of 'reserveCapacity' method execution (i.e.,
        //:   it is effectively a no-op).
        //:
        //: 6 Method 'reserveCapacity' cautiously copies all data stored in
        //:   buffer to the new allocated memory and sets up current location
        //:   pointer correctly.
        //:
        //: 7 Method 'reserveCapacity' deallocates memory previously allocated
        //:   for buffer.
        //
        // Plan:
        //: 1 Create an object with default constructor.  (C-1)
        //:
        //: 2 Using the table-driven technique, specify a set of requested
        //:   capacity values for buffers and expected result values.
        //:
        //: 3 For each row 'R' in the table of P-2:
        //:
        //:   1 Create an 'bdlsb::MemOutStreamBuf' object with default
        //:     capacity.
        //:
        //:   2 Reserve requested amount of bytes.
        //:
        //:   3 Verify that correct memory amount has been allocated by
        //:     allocator.  (C-4..5)
        //:
        //: 4 Using the table-driven technique, specify a set of requested
        //:   capacity values for buffers and expected result values.
        //:
        //: 5 For each row 'R' in the table of P-4:
        //:
        //:   1 Create an 'bdlsb::MemOutStreamBuf' object and reserve capacity
        //:     of one byte.
        //:
        //:   2 Reserve requested amount of bytes.
        //:
        //:   3 Verify that correct memory amount has been allocated by
        //:     allocator.  (C-4..5)
        //:
        //: 6 Create an 'bdlsb::MemOutStreamBuf' object, reserve some memory
        //:   and write some characters to the buffer.  Reserve bigger amount
        //:   of memory and write one more character.  Verify that buffer
        //:   contains all written characters in the right order.  (C-6)
        //:
        //: 7 Create an 'bdlsb::MemOutStreamBuf' object and reserve some
        //:   memory.  Reserve another amount of memory. Verify that previously
        //:    allocated memory has been deallocated.  (C-7)
        //:
        //: 8 Install a separate object of 'bdem_TestAllocator' as the default
        //:   allocator and use its object-specific statistics to verify that
        //:   it is in fact the source of default allocations and
        //:   deallocations.  (C-2)
        //:
        //: 9 Swap the default allocator out and then continue to allocate
        //:   additional objects to ensure that its changing has no effect on
        //:   existing table objects.  (C-3)
        //
        // Testing:
        //   MemOutStreamBuf(bslma::Allocator *basicAllocator = 0);
        //   void reserveCapacity(int numElements);
        //
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "PRIMARY MANIPULATORS" << endl
                          << "====================" << endl;

        if (verbose) cout << "\nSimple object creation." << endl;
        {
            Obj mSB1;

            bslma::TestAllocator ta(veryVeryVerbose);
            const Obj mSB2(&ta);
        }

        if (verbose) cout <<
        "\nTesting reserveCapacity in streambuf with default initial capacity."
                          << endl;
        {
            // Data structure that contains testing data for testing
            // 'reserveCapacity'.
            static const struct {
                int         d_line;          // line number
                bsl::size_t d_requestAmount; // how many bytes to ask for
                bsl::size_t d_capacity;      // expected streambuf capacity
            } DATA[] = {
                  //LINE  REQUEST AMOUNT    RESULTING CAPACITY
                  //----  --------------    ------------------
                  { L_,   0,                0                  },
                  { L_,   1,                1                  },
                  { L_,   INITIAL_BUFSIZE,  INITIAL_BUFSIZE    },
                  { L_,   LARGE_CAPACITY,   LARGE_CAPACITY     },
                  { L_,   MAX_CAPACITY,     MAX_CAPACITY       }
            };   // end table DATA


            const size_t DATA_LEN = sizeof DATA / sizeof *DATA;

            for (size_t i = 0; i < DATA_LEN; ++i ) {
                const int LINE = DATA[i].d_line;

                bslma::TestAllocator ta(veryVeryVerbose);
                Obj mSB(&ta);
                ASSERTV(LINE, 0 == ta.numBytesInUse());
                if (veryVerbose) cout << "\tRequesting capacity of "
                                      << DATA[i].d_requestAmount << '.'
                                      << endl;
                mSB.reserveCapacity(DATA[i].d_requestAmount);
                bsl::size_t NBIU =
                                  static_cast<bsl::size_t>(ta.numBytesInUse());
                ASSERTV(LINE,
                        DATA[i].d_capacity,
                        ta.numBytesInUse(),
                        DATA[i].d_capacity == NBIU);
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

            const size_t DATA_LEN = sizeof DATA / sizeof *DATA;

            for (size_t i = 0; i < DATA_LEN; ++i ) {
                const int LINE = DATA[i].d_line;

                bslma::TestAllocator ta(veryVeryVerbose);
                Obj mSB(&ta);
                mSB.reserveCapacity(1);
                ASSERTV(LINE, 1 == ta.numBytesInUse());
                if (veryVerbose) cout << "\tRequesting capacity of "
                                      << DATA[i].d_requestAmount << '.'
                                      << endl;
                mSB.reserveCapacity(DATA[i].d_requestAmount);
                ASSERTV(LINE, DATA[i].d_capacity == ta.numBytesInUse());

                // Now prove that the entirety of the returned memory is
                // used for capacity (rather than, say, other object
                // infrastructure) by writing out that many characters and
                // showing that no realloc happens.

                for (int j = 0; j < DATA[i].d_capacity; ++j) {
                    mSB.sputc('Z');
                }
                ASSERTV(LINE, DATA[i].d_capacity == ta.numBytesInUse());
            }
        }

        if (verbose) cout << "\nTesting data copying correctness." << endl;
        {
            bslma::TestAllocator ta(veryVeryVerbose);
            const size_t BUFFER_SIZE = 5;
            Obj mSB(&ta);
            mSB.reserveCapacity(BUFFER_SIZE);
            for (size_t i = 0; i < BUFFER_SIZE; ++i ) {
                mSB.sputc('Z');
            }
            mSB.reserveCapacity(2 * BUFFER_SIZE);
            mSB.sputc('a');

            ASSERT(2 * BUFFER_SIZE == ta.numBytesInUse());
            ASSERT(0 == strncmp(mSB.data(), "ZZZZZa", BUFFER_SIZE + 1));
        }

        if (verbose) cout << "\nTesting memory deallocation." << endl;
        {
            bslma::TestAllocator ta(veryVeryVerbose);
            const size_t BUFFER_SIZE = 5;
            Obj mSB(&ta);
            mSB.reserveCapacity(BUFFER_SIZE);
            const bsls::Types::Int64 NUM_DEALLOCATIONS = ta.numDeallocations();
            void *laa = ta.lastAllocatedAddress();
            mSB.reserveCapacity(2 * BUFFER_SIZE);

            ASSERT(NUM_DEALLOCATIONS + 1 == ta.numDeallocations());
            ASSERT(laa == ta.lastDeallocatedAddress());
        }

        if (verbose) cout <<
              "\nEnsure bdem_Default::allocator() is used by default." << endl;
        {

            bslma::TestAllocator da(veryVeryVerbose);

            if (verbose) cout <<
                       "\tInstall test allocator 'da' as the default." << endl;
            {
                const bslma::DefaultAllocatorGuard dag(&da);

                ASSERT(0 == da.numBlocksInUse());
                Obj x;

                bslma::TestAllocator ta(veryVeryVerbose);
                ASSERT(0 == ta.numBlocksInUse());

                Obj y(&ta);
                ASSERT(0 == da.numBlocksInUse());
                ASSERT(0 == ta.numBlocksInUse());

                bslma::TestAllocator oa(veryVeryVerbose);
                           // Installing this other allocator should have no
                           // effect on subsequent use of pre-existing objects.
                if (verbose) cout <<
                       "\tInstall test allocator 'oa' as the default." << endl;
                {
                    const bslma::DefaultAllocatorGuard oag(&oa);

                    ASSERT(0 == oa.numBlocksInUse());
                    Obj z;
                    ASSERT(0 == oa.numBlocksTotal());

                    ASSERT(0 == da.numBlocksTotal());
                    ASSERT(0 == ta.numBlocksTotal());

                    const int CAPACITY = INITIAL_BUFSIZE_MINUS_ONE;
                    x.reserveCapacity(CAPACITY);

                    bsls::Types::Int64 xNBT = da.numBytesTotal();
                    ASSERT(CAPACITY == xNBT);
                    ASSERT(0 == ta.numBytesTotal());
                    ASSERT(0 == oa.numBytesTotal());

                    y.reserveCapacity(CAPACITY);

                    bsls::Types::Int64 yNBT = ta.numBytesTotal();
                    ASSERT(xNBT == da.numBytesTotal());
                    ASSERT(CAPACITY == yNBT);
                    ASSERT(0    == oa.numBytesTotal());

                    z.reserveCapacity(CAPACITY);
                    bsls::Types::Int64 zNBT = oa.numBytesTotal();
                    ASSERT(xNBT == da.numBytesTotal());
                    ASSERT(yNBT == ta.numBytesTotal());
                    ASSERT(CAPACITY == zNBT);

                    ASSERT(ta.numBlocksTotal() == da.numBlocksTotal());
                    ASSERT(ta.numBlocksTotal() == oa.numBlocksTotal());
                }
                if (verbose) cout <<
                    "\tUn-install test allocator 'oa' as the default." << endl;
            }
            if (verbose) cout <<
                    "\tUn-install test allocator 'da' as the default." << endl;
        }
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
        //: 1 Developer test sandbox.  (C-1)
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BREATHING TEST" << endl
                          << "==============" << endl;

        if (verbose) cout <<
            "\nMake sure we can create and use a 'bdlsb::MemOutStreamBuf'."
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
