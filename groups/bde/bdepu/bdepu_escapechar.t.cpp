// bdepu_escapechar.t.cpp              -*-C++-*-

#include <bdepu_escapechar.h>

#include <bslma_testallocator.h>    // For testing only

#include <bsl_string.h>
#include <bsl_vector.h>
#include <bsl_iostream.h>
#include <bsl_c_stdlib.h>

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// TBD doc
//-----------------------------------------------------------------------------
// [ 1] escapeCodes();
//-----------------------------------------------------------------------------
// [ 2] USAGE EXAMPLE

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
    if (!(X)) { cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__);}}

#define LOOP2_ASSERT(I,J,X) { \
    if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " \
        << J << "\n"; aSsErT(1, #X, __LINE__); } }

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    // int veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;;

    bslma_TestAllocator testAllocator(veryVeryVerbose);

    switch (test) { case 0:  // Zero is always the leading case.
      case 2: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE
        //   The usage example provided in the component header file must
        //   compile, link, and run on all platforms as shown.
        //
        // Plan:
        //   Incorporate usage example from header into driver, remove leading
        //   comment characters, and replace 'assert' with 'ASSERT'.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing Usage Example" << endl
                          << "=====================" << endl;

        const bsl::string theBufferString = "abc\t\ndef";
        bsl::vector<char> theBuffer(&testAllocator);
        theBuffer.insert(theBuffer.end(),
                         theBufferString.begin(),
                         theBufferString.end());
        int bufLen = theBuffer.size();
        for (int i = 0; i < bufLen; ++i) {
            if ('\\' == theBuffer[i]) {
              switch(bdepu_EscapeChar::escapeCodes()[theBuffer[i + 1]]) {
                case bdepu_EscapeChar::BDEPU_BACKSPACE:
                case bdepu_EscapeChar::BDEPU_FORMFEED:
                case bdepu_EscapeChar::BDEPU_NEWLINE:
                case bdepu_EscapeChar::BDEPU_CARRIAGE_RETURN:
                    theBuffer[i] = theBuffer[i + 1] = ':'; // replace with "::"
                    break;
                default:      // for safety
                    break;
              } // end switch
            } // end if
        }  // end for

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // TESTING ESCAPECODES
        //
        // Concerns:
        // The method returns an array containing the correct values.
        //
        // Plan:
        // Exhaustive enumeration.
        //
        // Testing:
        //   escapeCodes();
        // --------------------------------------------------------------------

        for (int i = 0; i < 256; ++i) {
            unsigned char code = bdepu_EscapeChar::escapeCodes()[i];
            unsigned char EXP;

            switch (i) {
            case   0: EXP = bdepu_EscapeChar::BDEPU_ENDLINE; break;
            case  34: EXP = bdepu_EscapeChar::BDEPU_DOUBLE_QUOTE; break;
            case  39: EXP = bdepu_EscapeChar::BDEPU_SINGLE_QUOTE; break;
            case  48: EXP = bdepu_EscapeChar::BDEPU_OCTAL; break;
            case  49: EXP = bdepu_EscapeChar::BDEPU_OCTAL; break;
            case  50: EXP = bdepu_EscapeChar::BDEPU_OCTAL; break;
            case  51: EXP = bdepu_EscapeChar::BDEPU_OCTAL; break;
            case  52: EXP = bdepu_EscapeChar::BDEPU_OCTAL; break;
            case  53: EXP = bdepu_EscapeChar::BDEPU_OCTAL; break;
            case  54: EXP = bdepu_EscapeChar::BDEPU_OCTAL; break;
            case  55: EXP = bdepu_EscapeChar::BDEPU_OCTAL; break;
            case  63: EXP = bdepu_EscapeChar::BDEPU_QUESTION_MARK; break;
            case  92: EXP = bdepu_EscapeChar::BDEPU_BACKSLASH; break;
            case  97: EXP = bdepu_EscapeChar::BDEPU_ALERT; break;
            case  98: EXP = bdepu_EscapeChar::BDEPU_BACKSPACE; break;
            case 102: EXP = bdepu_EscapeChar::BDEPU_FORMFEED; break;
            case 110: EXP = bdepu_EscapeChar::BDEPU_NEWLINE; break;
            case 114: EXP = bdepu_EscapeChar::BDEPU_CARRIAGE_RETURN; break;
            case 116: EXP = bdepu_EscapeChar::BDEPU_TAB; break;
            case 118: EXP = bdepu_EscapeChar::BDEPU_VERTICAL_TAB; break;
            case 120: EXP = bdepu_EscapeChar::BDEPU_HEXADECIMAL; break;
            default:  EXP = bdepu_EscapeChar::BDEPU_ERROR; break;
            };

            LOOP_ASSERT(i, EXP == code);
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
//      Copyright (C) Bloomberg L.P., 2003
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
