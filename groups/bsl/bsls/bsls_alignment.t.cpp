// bsls_alignment.t.cpp                                               -*-C++-*-

#include <bsls_alignment.h>

#include <cstring>     // strcmp()
#include <cstdlib>     // atoi()
#include <iostream>

using namespace BloombergLP;
using namespace std;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                             Overview
//                             --------
// Most of what this component implements are compile-time computations that
// differ among platforms.  The tests do assume that alignment of 'char' is 1,
// 'short' is 2, 'int' is 4, and 'double' is at least 4.  In addition, certain
// invariants are tested, including:
//
// 1. That all alignment calculations result in a power of 2.
// 2. That all alignment-to-type calculations result are reversible, so that
//    the alignment of the resulting type equals the original input.
// 3. That the alignment of a 'struct' equals the alignment of its
//    most-strictly aligned member.
// 4. That 'bsls::Alignment::MAX_ALIGNMENT' really is the largest value that
//    will be produced by the alignment calculations and that
//    'bsls::Alignment::MaxAlignedType' is aligned at
//    'bsls::Alignment::MAX_ALIGNMENT'.
//
// For the few run-time functions provided in this component, we establish
// post-conditions and test that the postconditions hold over a reasonable
// range of inputs.
//-----------------------------------------------------------------------------
// [ 1] bsls_AlignmentOf<T>::VALUE (deprecated)
// [ 2] bsls::AlignmentToType<N>::Type (deprecated)
// [ 2] bsls_AlignmentOf<T>::Type (deprecated)
// [ 3] bsls::Alignment::MAX_ALIGNMENT (deprecated)
// [ 4] bsls::Alignment::MaxAlignedType (deprecated)
// [ 4] bsls::Alignment::Align (deprecated)
//
// [ 5] int bsls::Alignment::calculateAlignmentFromSize(int size); (deprecated)
// [ 6] int bsls::Alignment::calculateAlignmentOffset(void*, int); (deprecated)
// [ 7] bool bsls::Alignment::is2ByteAligned(const void *); (deprecated)
// [ 7] bool bsls::Alignment::is4ByteAligned(const void *); (deprecated)
// [ 7] bool bsls::Alignment::is8ByteAligned(const void *); (deprecated)
// [ 8] enum Strategy { ... };
// [ 8] char *toAscii(Type value);
//-----------------------------------------------------------------------------
// [ 3] PRINT ALIGNMENTS -- Display alignment values (very verbose mode only).
// [ 9] USAGE EXAMPLE -- Ensure the usage example compiles and works.
//=============================================================================

//-----------------------------------------------------------------------------
//                  STANDARD BDE ASSERT TEST MACRO
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
//----------------------------------------------------------------------------
#define LOOP_ASSERT(I,X) { \
    if (!(X)) { cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__);}}

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

//=============================================================================
//                      SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------

#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", " << flush; // P(X) without '\n'
#define A(X) cout << #X " = " << ((void *) X) << endl;  // Print address
#define A_(X) cout << #X " = " << ((void *) X) << ", " << flush;
#define L_ __LINE__                           // current Line number

//==========================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//--------------------------------------------------------------------------

typedef bsls::Alignment Obj;

//==========================================================================
//                               USAGE EXAMPLE
//--------------------------------------------------------------------------

///Usage
///-----
// Suppose that we want to create a static function, 'allocateFromBuffer', that
// takes a buffer, the size of the buffer, a cursor indicating a position
// within the buffer, an allocation request size, and a memory alignment
// strategy; 'allocateFromBuffer' returns a pointer to a block of memory,
// wholly contained within the buffer, having the specified size and alignment.
// As a side-effect, the cursor is updated to refer to the next available free
// byte in the buffer.  Such a function could be used by a memory manager to
// satisfy allocate requests from internally-managed buffers.  Clients of this
// function indicate which alignment strategy to use based on their specific
// requirements.
//
// Our 'allocateFromBuffer' function depends on an alignment utility,
// 'my_AlignmentUtil', whose minimal interface is limited to that required by
// this usage example.  (See the 'bsls_alignmentutil' component for a more
// realistic alignment utility.):
//..
    struct my_AlignmentUtil {
        // This 'struct' provides a namespace for basic types and utilities
        // related to memory alignment.

        // TYPES
        enum {
            MY_MAX_PLATFORM_ALIGNMENT = 8
        };
            // Provide the *minimal* value that satisfies the alignment
            // requirements for *all* types on the host platform.  Note that 8
            // is used for illustration purposes only; an actual implementation
            // would employ template meta-programming to deduce the value at
            // compile time.

        // CLASS METHODS
        static int calculateAlignmentFromSize(int size);
            // Calculate a usable alignment for a memory block of the specified
            // 'size' (in bytes) in the absence of compile-time knowledge of
            // the block's alignment requirements.  Return the largest power of
            // two that evenly divides 'size', up to a maximum of
            // 'MY_MAX_PLATFORM_ALIGNMENT'.  It is guaranteed that a block of
            // 'size' bytes can be safely aligned on the return value.   The
            // behavior is undefined unless '0 < size'.

        static int calculateAlignmentOffset(const void *address,
                                            int         alignment);
            // Return the smallest non-negative offset (in bytes) that, when
            // added to the specified 'address', yields the specified
            // 'alignment'.  The behavior is undefined unless '0 != alignment'
            // and 'alignment' is a non-negative, integral power of 2.
    };
//..
// The definition of our 'allocateFromBuffer function is as follows:
//..
    static void *allocateFromBuffer(int                      *cursor,
                                    char                     *buffer,
                                    int                       bufferSize,
                                    int                       size,
                                    bsls::Alignment::Strategy strategy)
        // Allocate a memory block of the specified 'size' (in bytes) from the
        // specified 'buffer' having the specified 'bufferSize' at the
        // specified 'cursor' position, using the specified alignment
        // 'strategy'.  Return the address of the allocated memory block if
        // 'buffer' contains sufficient available memory, and 0 otherwise.  The
        // 'cursor' is set to the first byte position immediately after the
        // allocated memory (which might be 1 byte past the end of 'buffer') if
        // there is sufficient memory, and is not modified otherwise.  The
        // behavior is undefined unless '0 <= bufferSize', '0 < size', and
        // 'cursor' refers to a valid position in 'buffer'.
    {
//..
// First we assert the function pre-conditions:
//..
        ASSERT(cursor);
        ASSERT(buffer);
        ASSERT(0 <= bufferSize);
        ASSERT(0 < size);
//..
// Then, based on the alignment 'strategy', we calculate the alignment value
// that can satisfy the allocation request.  In the case of
// 'bsls::Alignment::BSLS_NATURAL', we calculate the alignment from 'size'; for
// 'bsls::Alignment::BSLS_MAXIMUM', we use the platform-dependent
// 'my_AlignmentUtil::MY_MAX_PLATFORM_ALIGNMENT' value:
//..
        const int alignment =
                           strategy == bsls::Alignment::BSLS_NATURAL
                           ? my_AlignmentUtil::calculateAlignmentFromSize(size)
                           : my_AlignmentUtil::MY_MAX_PLATFORM_ALIGNMENT;
//..
// Now we calculate the offset from the current 'cursor' value that can satisfy
// the 'alignment' requirements:
//..
        const int offset = my_AlignmentUtil::calculateAlignmentOffset(
                                                              buffer + *cursor,
                                                              alignment);
//
//..
// Next we check if the available free memory in 'buffer' can satisfy the
// allocation request; 0 is returned if the request cannot be satisfied:
//..
        if (*cursor + offset + size > bufferSize) {
            return 0;                                                 // RETURN
        }

        void *result = &buffer[*cursor + offset];
        *cursor += offset + size;

//..
// Finally, return the address of the correctly aligned memory block:
//..
        return result;
    }
//..
// The 'allocateFromBuffer' function may be used by a memory manager that needs
// to appropriately align memory blocks that are allocated from
// internally-managed buffers.  For an example, see the 'bslma_bufferimputil'
// component.

// Provide dummy implementations of 'my_AlignmentUtil' functions, so that the
// test driver links successfully on all platforms.

int my_AlignmentUtil::calculateAlignmentFromSize(int)
{
    return 8;
}

int my_AlignmentUtil::calculateAlignmentOffset(const void *, int)
{
    return 0;
}

//==========================================================================
//                              MAIN PROGRAM
//--------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? std::atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
      case 2: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE
        // Concerns:
        //   The usage example provided in the component header file must
        //   compile, link, and run on all platforms as shown.
        //
        // Plan:
        //   Incorporate usage example from header into driver, remove leading
        //   comment characters, and replace 'assert' with 'ASSERT'.  Suppress
        //   all 'cout' statements in non-verbose mode, and add streaming to
        //   a buffer to test programmatically the printing examples.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Usage Examples"
                          << "\n======================" << endl;

      } break;
      case 1: {
        // -------------------------------------------------------------------
        // BASIC TEST:
        //   Iterate over a set of table-generated test vectors of varying
        //   enumerator value and verify that 'toAscii' generates the expected
        //   format.
        //
        // Testing:
        //   const char *toAscii(code);
        // -------------------------------------------------------------------

        if (verbose) cout << endl << "BASIC TEST" << endl
                                  << "==========" << endl;

        struct {
            int            d_line;
            Obj::Strategy  d_code;
            const char    *d_exp;
        } DATA[] = {
           // line    code                           expected output
           // ----    ---------                      ---------------
            { L_,     Obj::BSLS_MAXIMUM,            "MAXIMUM"      },
            { L_,     Obj::BSLS_NATURAL,            "NATURAL"      },
            { L_,     (Obj::Strategy)  2,          "(* UNKNOWN *)" },
            { L_,     (Obj::Strategy) -2,          "(* UNKNOWN *)" },
            { L_,     (Obj::Strategy) -5,          "(* UNKNOWN *)" },
            { L_,     (Obj::Strategy) 10,          "(* UNKNOWN *)" }
        };

        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        if (verbose) cout <<
                          "\nVerify enumerator values are sequential." << endl;

        for (int i = 0; i < 2; ++i) {
            LOOP_ASSERT(i, i == DATA[i].d_code);
        }

        if (verbose) cout << "Testing 'toAscii'." << endl;
        {
            for (int i = 0 ; i < NUM_DATA; ++i) {
                const int            LINE = DATA[i].d_line;
                const Obj::Strategy  CODE = DATA[i].d_code;
                const char          *EXP  = DATA[i].d_exp;

                const char *res = Obj::toAscii(CODE);
                if (veryVerbose) { cout << '\t'; P_(i); P_(CODE); P(res); }
                LOOP2_ASSERT(LINE, i, strlen(EXP) == strlen(res));
                LOOP2_ASSERT(LINE, i, 0 == strcmp(EXP, res));
            }
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
//      Copyright (C) Bloomberg L.P., 2010
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
