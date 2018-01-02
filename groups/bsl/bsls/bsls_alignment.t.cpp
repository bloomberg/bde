// bsls_alignment.t.cpp                                               -*-C++-*-

#include <bsls_alignment.h>

#include <bsls_bsltestutil.h>      // for testing only

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

using namespace BloombergLP;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                             Overview
//                             --------
// The component under test implements a single enumeration having sequential
// enumerator values that start from 0.  We will therefore follow our standard
// approach to testing enumeration types.
//-----------------------------------------------------------------------------
// TYPES
// [ 1] enum Strategy { ... };
//
// CLASS METHODS
// [ 1] const char *toAscii(Alignment::Strategy value);
//-----------------------------------------------------------------------------
// [ 2] USAGE EXAMPLE

//=============================================================================
//                  STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
// NOTE: THIS IS A LOW-LEVEL COMPONENT AND MAY NOT USE ANY C++ LIBRARY
// FUNCTIONS, INCLUDING IOSTREAMS.
static int testStatus = 0;

static void aSsErT(bool b, const char *s, int i) {
    if (b) {
        printf("Error " __FILE__ "(%d): %s    (failed)\n", i, s);
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}

# define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

//=============================================================================
//                       STANDARD BDE TEST DRIVER MACROS
//-----------------------------------------------------------------------------
#define LOOP_ASSERT  BSLS_BSLTESTUTIL_LOOP_ASSERT
#define LOOP2_ASSERT BSLS_BSLTESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLS_BSLTESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLS_BSLTESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLS_BSLTESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLS_BSLTESTUTIL_LOOP6_ASSERT

#define Q   BSLS_BSLTESTUTIL_Q   // Quote identifier literally.
#define P   BSLS_BSLTESTUTIL_P   // Print identifier and value.
#define P_  BSLS_BSLTESTUTIL_P_  // P(X) without '\n'.
#define T_  BSLS_BSLTESTUTIL_T_  // Print a tab (w/o newline).
#define L_  BSLS_BSLTESTUTIL_L_  // current Line number

// ============================================================================
//                        GLOBAL TYPEDEFS FOR TESTING
// ----------------------------------------------------------------------------

typedef bsls::Alignment::Strategy Enum;
typedef bsls::Alignment           Obj;

// ============================================================================
//                       GLOBAL CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

const int NUM_ENUMERATORS = 3;

#define UNKNOWN_FORMAT "(* UNKNOWN *)"

//=============================================================================
//                               USAGE EXAMPLE
//-----------------------------------------------------------------------------

///Usage
///-----
// Suppose that we want to create a static function, 'allocateFromBuffer', that
// takes a buffer, the size of the buffer, a cursor indicating a position
// within the buffer, an allocation request size, and a memory alignment
// strategy; 'allocateFromBuffer' returns a pointer to a block of memory,
// wholly contained within the buffer, having the specified size and alignment.
// As a side-effect, the cursor is updated to refer to the next available free
// byte in the buffer.  Such a function could be used by a memory manager to
// satisfy allocation requests from internally-managed buffers.  Clients of
// this function indicate which alignment strategy to use based on their
// specific requirements.
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
// The definition of our 'allocateFromBuffer' function is as follows:
//..
    static void *allocateFromBuffer(int                       *cursor,
                                    char                      *buffer,
                                    int                        bufferSize,
                                    int                        size,
                                    bsls::Alignment::Strategy  strategy)
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
// 'my_AlignmentUtil::MY_MAX_PLATFORM_ALIGNMENT' value; and for
// 'bsls::Alignment::BSLS_BYTEALIGNED', we simply use 1:
//..
        const int alignment =
                           strategy == bsls::Alignment::BSLS_NATURAL
                           ? my_AlignmentUtil::calculateAlignmentFromSize(size)
                           : strategy == bsls::Alignment::BSLS_MAXIMUM
                             ? my_AlignmentUtil::MY_MAX_PLATFORM_ALIGNMENT
                             : 1;
//..
// Now we calculate the offset from the current 'cursor' value that can satisfy
// the 'alignment' requirements:
//..
        const int offset = my_AlignmentUtil::calculateAlignmentOffset(
                                                              buffer + *cursor,
                                                              alignment);

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

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int        test = argc > 1 ? atoi(argv[1]) : 0;
    int     verbose = argc > 2;
    int veryVerbose = argc > 3;

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:  // Zero is always the leading case.
      case 2: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //
        // Concerns:
        //: 1 The usage example provided in the component header file must
        //:   compile, link, and run as shown.  (P-1)
        //
        // Plan:
        //: 1 Incorporate usage example from header into test driver, replace
        //:   leading comment characters with spaces, replace 'assert' with
        //:   'ASSERT', and insert 'if (veryVerbose)' before all output
        //:   operations.  (C-1)
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) printf("\nUSAGE EXAMPLE"
                            "\n=============\n");

        // Silence 'defined but not used' warning:

        (void) allocateFromBuffer;

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // TESTING 'enum' AND 'toAscii'
        //
        // Concerns:
        //: 1 The enumerator values are sequential, starting from 0.  (P-1)
        //: 2 The 'toAscii' method returns the expected string representation
        //:   for each enumerator.  (P-2)
        //: 3 The 'toAscii' method returns a distinguished string when passed
        //:   an out-of-band value.  (P-3)
        //: 4 The string returned by 'toAscii' is non-modifiable.  (P-4)
        //: 5 The 'toAscii' method has the expected signature.  (P-4)
        //
        // Plan:
        //: 1 Verify that the enumerator values are sequential, starting from
        //:   0.  (C-1)
        //: 2 Verify that the 'toAscii' method returns the expected string
        //:   representation for each enumerator.  (C-2)
        //: 3 Verify that the 'toAscii' method returns a distinguished string
        //:   when passed an out-of-band value.  (C-3)
        //: 4 Take the address of the 'toAscii' (class) method and use the
        //:   result to initialize a variable of the appropriate type.
        //:   (C-4, C-5)
        //
        // Testing:
        //   enum Strategy { ... };
        //   const char *toAscii(Alignment::Strategy value);
        // --------------------------------------------------------------------

        if (verbose) printf("\nTesting 'enum' and 'toAscii'"
                            "\n============================\n");

        int invalidEnumValues[] = { -1, -5, 99 };
        static const struct {
            int         d_lineNum;  // source line number
            Enum        d_value;    // enumerator value
            const char *d_exp;      // expected result
        } DATA[] = {
            // line         enumerator value            expected result
            // ----    ---------------------------     -----------------
            {  L_,     Obj::BSLS_MAXIMUM,              "MAXIMUM"         },
            {  L_,     Obj::BSLS_NATURAL,              "NATURAL"         },
            {  L_,     Obj::BSLS_BYTEALIGNED,          "BYTEALIGNED"     },

            {  L_,     (Enum)NUM_ENUMERATORS,          UNKNOWN_FORMAT    },
            {  L_,     (Enum)invalidEnumValues[0],     UNKNOWN_FORMAT    },
            {  L_,     (Enum)invalidEnumValues[1],     UNKNOWN_FORMAT    },
            {  L_,     (Enum)invalidEnumValues[2],     UNKNOWN_FORMAT    }
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        if (verbose) printf("\nVerify enumerator values are sequential.\n");

        for (int ti = 0; ti < NUM_ENUMERATORS; ++ti) {
            const Enum VALUE = DATA[ti].d_value;

            if (veryVerbose) { T_; P_(ti); P(VALUE); }

            LOOP_ASSERT(ti, ti == VALUE);
        }

        if (verbose) printf("\nTesting 'toAscii'.\n");

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int   LINE  = DATA[ti].d_lineNum;
            const Enum  VALUE = DATA[ti].d_value;
            const char *EXP   = DATA[ti].d_exp;

            const char *result = Obj::toAscii(VALUE);

            if (veryVerbose) { T_; P_(ti); P_(VALUE); P_(EXP); P(result); }

            LOOP2_ASSERT(LINE, ti, strlen(EXP) == strlen(result));
            LOOP2_ASSERT(LINE, ti,           0 == strcmp(EXP, result));
        }

        if (verbose) printf("\nVerify 'toAscii' signature.\n");

        {
            typedef const char *(*FuncPtr)(Enum);

            const FuncPtr FP = &Obj::toAscii;
            (void) FP;  // Suppress the unused variable warning
        }

      } break;
      default: {
        fprintf(stderr, "WARNING: CASE `%d' NOT FOUND.\n", test);
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        fprintf(stderr, "Error, non-zero test status = %d.\n", testStatus);
    }

    return testStatus;
}

// ----------------------------------------------------------------------------
// Copyright 2013 Bloomberg Finance L.P.
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
