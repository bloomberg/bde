// bdem_functiontemplates.t.cpp                  -*-C++-*-

#include <bdem_functiontemplates.h>

#include <bdex_testinstream.h>
#include <bdex_testoutstream.h>

#include <bsl_iostream.h>
#include <bsl_sstream.h>
#include <bsl_vector.h>

#include <bsl_climits.h>
#include <bsl_cstdlib.h>

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

//=============================================================================
//                      STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
static int testStatus = 0;

static void aSsErT(int c, const char *s, int i)
{
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (0 <= testStatus && testStatus <= 100) ++testStatus;
    }
}

#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

#define LOOP_ASSERT(I,X) { \
   if (!(X)) { cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__); }}

#define LOOP2_ASSERT(I,J,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " \
              << J << "\n"; aSsErT(1, #X, __LINE__); } }


//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    // int veryVerbose = argc > 3;
    // int veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
      case 4: {
        bsl::vector<char> v1;

        v1.push_back('H');
        v1.push_back('E');
        v1.push_back('l');
        v1.push_back('l');
        v1.push_back('o');
        v1.push_back(' ');
        v1.push_back(0x85);
        v1.push_back(0xA4);
        v1.push_back(0xB1);
        v1.push_back(0xC7);
        v1.push_back(' ');
        v1.push_back('W');
        v1.push_back('o');
        v1.push_back('r');
        v1.push_back('l');
        v1.push_back('d');

        ostringstream stream;
        bdem_FunctionTemplates::printArray<bsl::vector<char> >(&v1, stream,
                                                               0, -1);

        const char FMT[] = { "\"HEllo \\x85\\xA4\\xB1\\xC7 World\"" };

        if (verbose) {
            cout << "Expected format(" << strlen(FMT)           << "): " <<
                                                                   FMT << endl;
            cout << "  Actual format(" << stream.str().length() << "): " <<
                                                          stream.str() << endl;
        }
        LOOP2_ASSERT(FMT, stream.str(), strlen(FMT) == stream.str().length() &&
                                                          stream.str() == FMT);
      } break;
      case 3: {
        bsl::vector<char> a1, a2;
        a1.push_back('a');
        a1.push_back('r');
        a1.push_back('b');
        a1.push_back('i');
        a1.push_back('t');
        a1.push_back('r');
        a1.push_back('a');
        a1.push_back('r');
        a1.push_back('y');
        a1.push_back(' ');
        a1.push_back('d');
        a1.push_back('a');
        a1.push_back('t');
        a1.push_back('a');
        bdex_TestOutStream strm1;

        bdem_FunctionTemplates::streamOutArray<char, bdex_TestOutStream>(
            &a1, strm1, 0,
            (const bdem_DescriptorStreamOut<bdex_TestOutStream> *) 0);

        if (verbose) {
            cout << "a = [" << strm1.length() << ']';
            for (int i = 0; i < strm1.length(); ++i) {
                cout << hex
                     << ' '
                     << ((unsigned) strm1.data()[i] & 0xffU);
            }
            cout << bsl::endl;
        }

        bdex_TestInStream strm2(strm1.data(), strm1.length());

        bdem_FunctionTemplates::streamInArray<char, bdex_TestInStream>(
                &a2, strm2, 0,
                (const bdem_DescriptorStreamIn<bdex_TestInStream> *) 0,
                (const bdem_Descriptor**) 0);

        ASSERT(a1 == a2);
      } break;

      case 2: {
        int a = 0x12345678;
        bdex_TestOutStream strm;

        bdem_FunctionTemplates::streamOutFundamental<int, bdex_TestOutStream>(
            &a, strm, 1,
            (const bdem_DescriptorStreamOut<bdex_TestOutStream> *) 0);

        if (verbose) {
            cout << "a = [" << strm.length() << ']';
            for (int i = 0; i < strm.length(); ++i) {
                cout << hex
                     << ' '
                     << ((unsigned) strm.data()[i] & 0xffU);
            }
            cout << bsl::endl;
        }

        static const char expected[] = { 0xe6, 0x12, 0x34, 0x56, 0x78 };
        ASSERT(strm.length() == sizeof(expected));
        ASSERT(0 == memcmp(strm.data(),
                           expected,
                           sizeof(expected)));

      } break;

      case 1: {
        int val = 0;
        bdem_FunctionTemplates::makeUnset<int>(&val);
        ASSERT(val == INT_MIN);
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
