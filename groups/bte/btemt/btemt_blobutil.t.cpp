// btemt_blobutil.t.cpp     -*-C++-*-
#include <btemt_blobutil.h>

#include <bcema_blob.h>
#include <bcema_blobutil.h>
#include <bcema_pooledblobbufferfactory.h>

#include <bslma_testallocator.h>
#include <bslma_defaultallocatorguard.h> // for testing only
#include <bslma_default.h>               // for testing only
#include <bsls_assert.h>

#include <bsl_iostream.h>
#include <bsl_vector.h>

using namespace BloombergLP;
using namespace bsl;
using namespace BloombergLP::btemt;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// This component defines a set of utility functions for operating on Blobs.
// Since this is a utility class, it holds no internal state and each method
// can be tested independently.
//-----------------------------------------------------------------------------
// [ 3] void extract(char *buffer, int numBytes, bcema_Blob *blob);
// [ 4] void copyOut(char *buffer, int numBytes, const bcema_Blob& blob);
// [ 5] const char *getData(*buffer, numBytes, blob, blobOffset);
//-----------------------------------------------------------------------------
// [ 2] TESTING APPARATUS
// [ 1] BREATHING TEST
//=============================================================================
//                  STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
static int testStatus = 0;

static void aSsErT(int c, const char *s, int i) {
    if (c) {
        bsl::cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << bsl::endl;
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}

#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }


//=============================================================================
//                    STANDARD BDE LOOP-ASSERT TEST MACROS
//-----------------------------------------------------------------------------

#define LOOP_ASSERT(I,X) { \
    if (!(X)) { bsl::cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__);}}

#define LOOP2_ASSERT(I,J,X) { \
    if (!(X)) { bsl::cout << #I << ": " << I << "\t" << #J << ": " \
    << J << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) { \
    if (!(X)) { bsl::cout << #I << ": " << I << "\t" << #J << ": " \
    << J << "\t" << #K << ": " << K << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP4_ASSERT(I,J,K,L,X) { \
    if (!(X)) { bsl::cout << #I << ": " << I << "\t" << #J << ": " << J \
    << "\t" << #K << ": " << K << "\t" << #L << ": " << L \
    << "\n"; aSsErT(1, #X, __LINE__); } }
        
//=============================================================================
//                    SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------

#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value
#define Q(X) cout << "<! " #X " |>" << endl;  // Quote identifier literally
#define P_(X) cout << #X " = " << (X) << ", " << flush; // P(X) without '\n'
#define L_ __LINE__
#define T_ cout << "\t" << flush;           // Print tab w/o newline

//=============================================================================
//               GLOBAL HELPER CLASSES AND FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

void assertHandler(const char *text, const char *file, int line)
{
	LOOP3_ASSERT(text, file, line, false);
}

//=============================================================================
//                                 MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[]) {
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;

    bslma_TestAllocator ta(veryVeryVerbose);
    bslma_Default::setDefaultAllocatorRaw(&ta);
	bsls_Assert::setFailureHandler(assertHandler);

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:  // Zero is always the leading case.
      case 5: {
        // --------------------------------------------------------------------
        // TESTING 'getData'
        //
        // Concerns:
        //   This function attempts to efficiently give back a pointer to a
        //   requested buffer.  If the method can return the buffer from the
        //   blob directly it should, else it will use the provided storage
        //   buffer.  Testing needs to make sure the right thing is done when
        //   the requested buffer fits into one BlobBuffer and when it spans
        //   multiple buffers.  Also the blobOffset parameters needs to be
        //   regardless of which BlobBuffer in the Blob the start position
        //   is located.
        //
        // Plan:
        //   Construct a test vector of possible inputs and specify for each
        //   set of inputs whether the storage should have been used and to
        //   check the returned buffer for correctness.
        //
        // Testing:
        //   const char *getData(*buffer, numBytes, blob, blobOffset);
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "TESTING 'getData'" << endl
                                  << "=================" << endl;

        const struct {
            int         d_line;           // line number of test case
            int         d_blobBufferSize; // length of each blob buffer
            const char *d_blobData;       // data in the blob
            int         d_length;         // length of data to get
            int         d_offset;         // starting offset
            bool        d_usedBuffer;     // should the storage have been used
            const char *d_result;         // result data
        } DATA[] = {
            //LINE BBSIZE          BLOB        LENGTH  OFFSET    USEDBUFFER        RESULT      
            //---- ------  ------------------  ------  ------    ----------    ------------------ 
            // Single buffer
            { L_,      5,  "",                     0,      0,        false,    ""               },
            { L_,      5,  "",                     1,      0,        false,    ""               },
            { L_,      5,  "abc",                  0,      0,        false,    ""               },
            { L_,      5,  "abc",                  2,      0,        false,    "ab"             },
            { L_,      5,  "abc",                  4,      0,        false,    "abc"            },
            { L_,      5,  "abcd",                 5,      0,        false,    "abcd"           },
            { L_,      5,  "abcde",                5,      0,        false,    "abcde"          },
            { L_,      5,  "abcdefghijklm",        5,      0,        false,    "abcde"          },
            { L_,      6,  "abcdefghijklm",        6,      0,        false,    "abcdef"         },
            { L_,     14,  "abcdefghijklm",       14,      0,        false,    "abcdefghijklm"  },
            { L_,   1024,  "abcdefghijklm",       14,      0,        false,    "abcdefghijklm"  },

            // Across buffers
            { L_,      2,  "abc",                  4,      0,        true,    "abc"             },
            { L_,      2,  "abcde",                4,      0,        true,    "abcd"            },
            { L_,      2,  "abcde",                5,      0,        true,    "abcde"           },
            { L_,      1,  "abcdefghijklm",       14,      0,        true,    "abcdefghijklm"   },
            { L_,      2,  "abcdefghijklm",       14,      0,        true,    "abcdefghijklm"   },
            { L_,      3,  "abcdefghijklm",       14,      0,        true,    "abcdefghijklm"   },
            { L_,      8,  "abcdefghijklm",       14,      0,        true,    "abcdefghijklm"   },
            { L_,     12,  "abcdefghijklm",       14,      0,        true,    "abcdefghijklm"   },
            { L_,     13,  "abcdefghijklm",       14,      0,        true,    "abcdefghijklm"   },

            // With offset
            { L_,      5,  "abc",                  0,      1,        false,    ""               },
            { L_,      5,  "abc",                  2,      1,        false,    "bc"             },
            { L_,      5,  "abc",                  3,      1,        false,    "bc"             },
            { L_,      5,  "abcdefgh",             1,      4,        false,    "e"              },
            { L_,      5,  "abcdefgh",             1,      5,        false,    "f"              },
            { L_,      5,  "abcdefgh",             2,      5,        false,    "fg"             },
            { L_,      5,  "abcdefgh",             1,      6,        false,    "g"              },
            { L_,      5,  "abcdefghijklm",        3,     10,        false,    "klm"            },

            // Across buffers with offset
            { L_,      2,  "abc",                  2,      1,        true,    "bc"              },
            { L_,      2,  "abcde",                3,      1,        true,    "bcd"             },
            { L_,      2,  "abcde",                2,      3,        true,    "de"              },
            { L_,      1,  "abcdefghijklm",        3,      1,        true,    "bcd"             },
            { L_,      1,  "abcdefghijklm",        3,      3,        true,    "def"             },
            { L_,      1,  "abcdefghijklm",        7,      3,        true,    "defghij"         },
            { L_,      1,  "abcdefghijklm",        3,      4,        true,    "efg"             },
            { L_,      1,  "abcdefghijklm",        1,     12,        false,   "m"               },

        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int i = 0; i < NUM_DATA; ++i) {
            const int   LINE           = DATA[i].d_line;
            const int   BLOBBUFFERSIZE = DATA[i].d_blobBufferSize;
            const char *BLOBDATA       = DATA[i].d_blobData;
            const int   LENGTH         = DATA[i].d_length;
            const int   OFFSET         = DATA[i].d_offset;
            const bool  USEDBUFFER     = DATA[i].d_usedBuffer;
            const char *RESULT         = DATA[i].d_result;

            if (veryVerbose) { 
                T_ P_(LINE) P_(BLOBBUFFERSIZE) P_(BLOBDATA) P_(LENGTH)
                    P_(OFFSET) P_(RESULT) P(USEDBUFFER)
            }

            bcema_PooledBlobBufferFactory blobFactory(BLOBBUFFERSIZE);
            bcema_Blob blob(&blobFactory);
            bcema_BlobUtil::append(&blob, BLOBDATA, strlen(BLOBDATA)+1);

            bsl::vector<char> buffer;
            buffer.resize(LENGTH + 10, -1); // extra bytes for overrun detection

            const char *result =
                BlobUtil::getData(&buffer[0], LENGTH, blob, OFFSET);

            LOOP_ASSERT(LINE, 0 == memcmp(BLOBDATA + OFFSET, RESULT, LENGTH));
            if (USEDBUFFER && LENGTH > 0) {
                LOOP_ASSERT(LINE, result == &buffer[0]);
                for (unsigned i = LENGTH; i < buffer.size(); ++i) {
                    LOOP_ASSERT(LINE, buffer[i] == -1);
                }
            }
            else {
                for (unsigned i = 0; i < buffer.size(); ++i) {
                    LOOP_ASSERT(LINE, buffer[i] == -1);
                }
            }
        }
        
      } break;
      case 4: {
          // --------------------------------------------------------------------
          // TESTING 'extract'
          //
          // Concerns:
          //   
          //
          // Plan:
          //   
          //
          // Testing:
          //   
          // --------------------------------------------------------------------

          if (verbose) cout << endl << "TESTING 'extract'" << endl
                                    << "=================" << endl;

          // . . . 
        
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING 'copyOut'
        //
        // Concerns:
        //   
        //
        // Plan:
        //   
        //
        // Testing:
        //   
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "TESTING 'copyOut'" << endl
                                  << "=================" << endl;
        
        // . . . 

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING APPARATUS
        //
        // Concerns:
        //   Ensure that the generator 'gg' method used to help create 
        //
        // Plan:
        //   
        //
        // Testing:
        //   
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "TESTING 'copyOut'" << endl
                                  << "=================" << endl;
        
        // . . . 
        
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //
        // Concerns:
        //   This test exercises basic functionality but tests nothing.
        //
        // Plan:
        //   Nothing to do.  See specific method test cases.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "BREATHING TEST" << endl
                                  << "==============" << endl;
        
      } break;
      default: {
        cerr << "WARNING: CASE '" << test << "' NOT FOUND." << endl;
        return -1;
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