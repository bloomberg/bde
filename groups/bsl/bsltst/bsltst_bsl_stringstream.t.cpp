// bsltst_stringstream.t.cpp                  -*-C++-*-
#ifndef BSL_OVERRIDES_STD
#define BSL_OVERRIDES_STD
#endif
#include <bsltst_bsl_stringstream.h>

#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>

#include <bslalg_hastrait.h>
#include <bslalg_typetraits.h>
#include <bsls_platform.h>

#include <algorithm>

#include <limits>
#include <sstream>
#include <iostream>

#include <cstring>
#include <cstdlib>

using namespace BloombergLP;
using std::cout;
using std::wcout;
using std::cerr;
using std::endl;
using std::flush;
using std::atoi;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
// [ 5] Stringstream Test
// [ 4] Ostringstream Test
// [ 3] Istringstream Test
// [ 2] Memory Consumption Test
// [ 1] Breathing Test
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
//                  GLOBALS, TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

int verbose         = 0;
int veryVerbose     = 0;
int veryVeryVerbose = 0;

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

// sameType -- returns 'true' if objects 'lhs' and 'rhs' are of exactly the
// same type and 'false' otherwise.  Note that C++ performs no implicit
// conversions on parameters to template functions.

template<typename LHSTYPE, typename RHSTYPE>
bool sameType(const LHSTYPE& lhs, const RHSTYPE& rhs)
{
    (void) lhs;    (void) rhs;

    return false;
}

template<typename TYPE>
bool sameType(const TYPE& lhs, const TYPE& rhs)
{
    (void) lhs;    (void) rhs;

    return true;
}

// ----------------------------------------------------------------------------

template<typename TYPE>
bool usesBslmaAllocator(const TYPE& arg)
    // returns 'true' if 'TYPE' uses bslma_Allocator and 'false' otherwise.
{
    (void) arg;

    return bslalg_HasTrait<TYPE, bslalg_TypeTraitUsesBslmaAllocator>::VALUE;
}

template <typename TYPE>
bool myAbs(const TYPE& x)
{
    return x >= 0 ? x : -x;
}

// ae: Approximately Equal
template <typename FLOAT_TYPE>
bool ae(FLOAT_TYPE lhs, FLOAT_TYPE rhs)
{
    FLOAT_TYPE diff = myAbs(lhs - rhs);

    lhs = myAbs(lhs);
    rhs = myAbs(rhs);
    return diff <= bsl::max(lhs, rhs) * 0.0001;
}

//=============================================================================
//                  GLOBAL HELPER CLASSES FOR TESTING
//-----------------------------------------------------------------------------

//=============================================================================
//                             USAGE EXAMPLES
//-----------------------------------------------------------------------------

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    verbose = argc > 2;
    veryVerbose = argc > 3;
    veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;;

    bslma_TestAllocator ta;
    bslma_TestAllocator tda;

    bslma_DefaultAllocatorGuard defaultGuard(&tda);

    switch (test) { case 0:  // Zero is always the leading case.
      case 5: {
        // --------------------------------------------------------------------
        // INOUT STRINGSTREAM TEST
        //
        // Concerns:
        //   That 'stringstream' works as specced.
        //
        // Plan:
        //   Create some 'stringstream's, read and write various types to them
        //   and observe that the behavior is as expected.  Repeat the
        //   experimetns passing non-default allocators to the stringstreams'
        //   c'tors.
        // --------------------------------------------------------------------

        if (verbose) cout << "\nINOUT STRINGSTREAM TEST\n"
                               "=======================\n";

        {
            const int vals[] = { -2, -1, 0, 1, 2, 3, 4, 5, 6, 1023, 1024,
                                 -1023, -1024 };
                                 
            enum { NUM_VALS = sizeof vals / sizeof *vals };

            bsl::stringstream ss;
            int j, k;
            for (j = 0; j < NUM_VALS; ++j) {
                ss << vals[j] << " ";
            }
            LOOP_ASSERT(ss.str(),
                        ss.str() == "-2 -1 0 1 2 3 4 5 6 1023 1024 -1023 -1024 ");

            for (j = 0; ss >> k; ++j) {
                ASSERT(vals[j] == k);
            }
            ASSERT(NUM_VALS == j);
        }

        {
            double vals[] = {
                           0, 0.0, 1.0, 2, 3.21, 4.5, -7.3, 4.3e28, -2.4e-92 };

            enum { NUM_VALS = sizeof vals / sizeof *vals };

            bsl::stringstream ss;
            int j;
            for (j = 0; j < NUM_VALS; ++j) {
                ss << vals[j] << " ";
            }

#ifdef BSLS_PLATFORM__CMP_MSVC
            const char *EXPECTED = "0 0 1 2 3.21 4.5 -7.3 4.3e+028 -2.4e-092 ";
#else 
            const char *EXPECTED = "0 0 1 2 3.21 4.5 -7.3 4.3e+28 -2.4e-92 ";
#endif
            LOOP2_ASSERT(ss.str(), EXPECTED,
                        ss.str() == EXPECTED);
            double d;
            for (j = 0; ss >> d; ++j) {
                ASSERT(vals[j] == d);
            }
            ASSERT(NUM_VALS == j);
        }

        {
            float vals[] = {
                           0, 0.0, 1.0, 2, 3.21, 4.5, -7.3, 4.3e28, -2.4e-28 };
            enum { NUM_VALS = sizeof vals / sizeof *vals };

            bsl::stringstream ss;
            int j;
            for (j = 0; j < NUM_VALS; ++j) {
                ss << vals[j] << " ";
            }
#ifdef BSLS_PLATFORM__CMP_MSVC
            const char *EXPECTED ="0 0 1 2 3.21 4.5 -7.3 4.3e+028 -2.4e-028 ";
#else 
            const char *EXPECTED ="0 0 1 2 3.21 4.5 -7.3 4.3e+28 -2.4e-28 ";
#endif
            LOOP2_ASSERT(ss.str(), EXPECTED,
                        ss.str() == EXPECTED);

            float f;
            for (j = 0; ss >> f; ++j) {
                ASSERT(vals[j] == f);
            }
            ASSERT(NUM_VALS == j);
        }

        {
            const char *vals[] = { "woof", "arf", "meow", "1.1", "1.2.3.4",
                                   "1_2", "1-2", "~~~", "```", "!!!", "@@@",
                                   "###", "$$$", "%%%", "^^^", "&&&", "***",
                                   "(((", ")))", "---", "___", "+++", "===",
                                   "[[[", "]]]", ":::", ";;;", "'''",
                                   "\"\"\"", "<<<", ">>>", ",,,", "...",
                                   "???", "///", "|||", "\\\\\\" };
            const char *expected = "woof arf meow 1.1 1.2.3.4 1_2 1-2"
                                   " ~~~ ``` !!! @@@ ### $$$ %%% ^^^ &&& ***"
                                   " ((( ))) --- ___ +++ === [[[ ]]] ::: ;;;"
                                   " ''' \"\"\" <<< >>> ,,, ... ??? /// |||"
                                   " \\\\\\ ";
            enum { NUM_VALS = sizeof vals / sizeof *vals };

            native_std::stringstream osA;
            native_std::string ns;
            int j;
            for (j = 0; j < NUM_VALS; ++j) {
                osA << vals[j] << " ";
            }
            ASSERT(osA.str() == expected);
            for (j = 0; osA >> ns; ++j) {
                ASSERT(vals[j] == ns);
            }
            ASSERT(NUM_VALS == j);

            bsl::stringstream osB;
            bsl::string bs;
            for (j = 0; j < NUM_VALS; ++j) {
                osB << vals[j] << " ";
            }
            ASSERT(osB.str() == expected);
            for (j = 0; osB >> bs; ++j) {
                ASSERT(vals[j] == bs);
            }
            ASSERT(NUM_VALS == j);

            bsl::stringstream osC;
            for (j = 0; j < NUM_VALS; ++j) {
                osC << native_std::string(vals[j]) << " ";
            }
            ASSERT(osC.str() == expected);

            bsl::ostringstream osD;
            for (j = 0; j < NUM_VALS; ++j) {
                osD << bsl::string(vals[j]) << " ";
            }
            ASSERT(osD.str() == expected);
        }

        {
            const char vals[] = "abcd1234~`!@#$%^&*()_-+={}[]:;\"'<>,.?/|\\";
            const int NUM_VALS = std::strlen(vals) / sizeof *vals;

            int j;

            native_std::stringstream nss;
            for (j = 0; j < NUM_VALS; ++j) {
                nss << vals[j];
            }
            LOOP_ASSERT(nss.str(), nss.str() == vals);
            char c;
            for (j = 0; nss >> c; ++j) {
                ASSERT(vals[j] == c);
            }
            LOOP2_ASSERT(NUM_VALS, j, NUM_VALS == j);

            bsl::stringstream bss;
            for (j = 0; j < NUM_VALS; ++j) {
                bss << vals[j];
            }
            LOOP_ASSERT(bss.str(), bss.str() == vals);
            for (j = 0; bss >> c; ++j) {
                ASSERT(vals[j] == c);
            }
            ASSERT(NUM_VALS == j);
            LOOP2_ASSERT(NUM_VALS, j, NUM_VALS == j);
        }

        {
            // verify '>> char' ignores whitespace
            const char vals[] = " \t\n\r";
            const int NUM_VALS = std::strlen(vals) / sizeof *vals;

            native_std::stringstream nss;
            char c;
            int j;
            for (j = 0; j < NUM_VALS; ++j) {
                nss << vals[j];
            }
            ASSERT(nss.str() == vals);
            for (j = 0; nss >> c; ++j) {        // reading skips white space
                ASSERT(0);
            }
            ASSERT(0 == j);

            bsl::stringstream bss;
            for (j = 0; j < NUM_VALS; ++j) {
                bss << vals[j];
            }
            ASSERT(bss.str() == vals);
            for (j = 0; bss >> c; ++j) {
                ASSERT(0);
            }
            ASSERT(0 == j);
        }

        typedef native_std::basic_string<wchar_t,
                                         std::char_traits<wchar_t>,
                                         bsl::allocator<wchar_t> > Nwstring;
        typedef bsl::basic_string<wchar_t,
                                  std::char_traits<wchar_t>,
                                  bsl::allocator<wchar_t> > Bwstring;
        typedef native_std::basic_stringstream<wchar_t,
                                               std::char_traits<wchar_t>,
                                               bsl::allocator<wchar_t> >
                                                                     Nwsstream;
        typedef bsl::basic_stringstream<wchar_t,
                                        std::char_traits<wchar_t>,
                                        bsl::allocator<wchar_t> > Bwsstream;

        {
            bsl::istringstream is();
            const int vals[] = { -2, -1, 0, 1, 2, 3, 4, 5, 6, 1023, 1024,
                                 -1023, -1024 };
                                 
            enum { NUM_VALS = sizeof vals / sizeof *vals };

            Bwsstream bss;
            int j, k;
            for (j = 0; j < NUM_VALS; ++j) {
                bss << vals[j] << " ";
            }
            ASSERT(bss.str() == L"-2 -1 0 1 2 3 4 5 6 1023 1024 -1023 -1024 ");
            for (j = 0; bss >> k; ++j) {
                ASSERT(vals[j] == k);
            }
            ASSERT(NUM_VALS == j);
        }

        {
            double vals[] = {
                           0, 0.0, 1.0, 2, 3.21, 4.5, -7.3, 4.3e28, -2.4e-92 };
            enum { NUM_VALS = sizeof vals / sizeof *vals };

            Bwsstream bss;
            int j;
            for (j = 0; j < NUM_VALS; ++j) {
                bss << vals[j] << " ";
            }

#ifdef BSLS_PLATFORM__CMP_MSVC
            const wchar_t *EXPECTED =
                                  L"0 0 1 2 3.21 4.5 -7.3 4.3e+028 -2.4e-092 ";
#else 
            const wchar_t *EXPECTED =
                                    L"0 0 1 2 3.21 4.5 -7.3 4.3e+28 -2.4e-92 ";
#endif
            ASSERT(bss.str() == EXPECTED);

            double d;
            for (j = 0; bss >> d; ++j) {
                ASSERT(vals[j] == d);
            }
            ASSERT(NUM_VALS == j);
        }

        {
            float vals[] = {
                           0, 0.0, 1.0, 2, 3.21, 4.5, -7.3, 4.3e28, -2.4e-28 };
            enum { NUM_VALS = sizeof vals / sizeof *vals };

            bsl::basic_stringstream<wchar_t,
                                    std::char_traits<wchar_t>,
                                    bsl::allocator<wchar_t> > bss;
            int j;
            for (j = 0; j < NUM_VALS; ++j) {
                bss << vals[j] << " ";
            }

#ifdef BSLS_PLATFORM__CMP_MSVC
            const wchar_t *EXPECTED =
                                  L"0 0 1 2 3.21 4.5 -7.3 4.3e+028 -2.4e-028 ";
#else 
            const wchar_t *EXPECTED =
                                    L"0 0 1 2 3.21 4.5 -7.3 4.3e+28 -2.4e-28 ";
#endif
            ASSERT(bss.str() == EXPECTED);

            float f;
            for (j = 0; bss >> f; ++j) {
                ASSERT(vals[j] == f);
            }
            ASSERT(NUM_VALS == j);
        }

#if (!defined(BSLS_PLATFORM__OS_SOLARIS) && !defined(BSLS_PLATFORM__OS_SUNOS))\
   || defined(BSLS_PLATFORM__CMP_GNU)
        {
            typedef native_std::basic_stringstream<wchar_t,
                                     native_std::char_traits<wchar_t>,
                                     native_std::allocator<wchar_t> > Wsstream;
            typedef native_std::basic_string<wchar_t,
                                      native_std::char_traits<wchar_t>,
                                      native_std::allocator<wchar_t> > Wstring;

            Wsstream wss;
            Wstring ws;

            wss << L"arf" << L" meow" << L" woof" << L" growl";
            (void) wss.str();
            ASSERT(wss >> ws);
            ASSERT(L"arf" == ws);
        }

        {

            const wchar_t *vals[] = {L"woof", L"arf", L"meow", L"1.1",
                                     L"1.2.3.4", L"1_2", L"1-2", L"~~~",
                                     L"```", L"!!!", L"@@@", L"###", L"$$$",
                                     L"%%%", L"^^^", L"&&&", L"***", L"(((",
                                     L")))", L"---", L"___", L"+++", L"===",
                                     L"[[[", L"]]]", L":::", L";;;", L"'''",
                                     L"\"\"\"", L"<<<", L">>>", L",,,", L"...",
                                     L"???", L"///", L"|||", L"\\\\\\" };
            const wchar_t *expected = L"woof arf meow 1.1 1.2.3.4 1_2 1-2"
                                      L" ~~~ ``` !!! @@@ ### $$$ %%% ^^^ &&&"
                                      L" *** ((( ))) --- ___ +++ === [[[ ]]]"
                                      L" ::: ;;; ''' \"\"\" <<< >>> ,,, ..."
                                      L" ??? /// ||| \\\\\\ ";
            enum { NUM_VALS = sizeof vals / sizeof *vals };

            Nwsstream ssA;
            Nwstring nws;
            Bwsstream ssB;
            Bwstring bws;

            int j;

            for (j = 0; j < NUM_VALS; ++j) {
                ssA << vals[j] << L" ";
            }
            ASSERT(ssA.str() == expected);
            for (j = 0; ssA >> nws; ++j) {
                ASSERT(vals[j] == nws);
            }
            LOOP2_ASSERT(NUM_VALS, j, NUM_VALS == j);

            for (j = 0; j < NUM_VALS; ++j) {
                ssB << vals[j] << L" ";
            }
            ASSERT(ssB.str() == expected);
            for (j = 0; ssB >> nws; ++j) {
                ASSERT(vals[j] == nws);
            }
            LOOP2_ASSERT(NUM_VALS, j, NUM_VALS == j);

            Nwsstream ssC;
            for (j = 0; j < NUM_VALS; ++j) {
                ssC << Nwstring(vals[j]) << L" ";
            }
            ASSERT(ssC.str() == expected);
            for (j = 0; ssC >> nws; ++j) {
                ASSERT(vals[j] == nws);
            }
            LOOP2_ASSERT(NUM_VALS, j, NUM_VALS == j);

            Bwsstream ssD;
            for (j = 0; j < NUM_VALS; ++j) {
                ssD << Bwstring(vals[j]) << L" ";
            }
            ASSERT(ssD.str() == expected);
            for (j = 0; ssD >> bws; ++j) {
                ASSERT(vals[j] == bws);
            }
            LOOP2_ASSERT(NUM_VALS, j, NUM_VALS == j);

            Nwsstream ssE(expected);
            ASSERT(ssE.str() == expected);
            for (j = 0; ssE >> nws; ++j) {
                ASSERT(vals[j] == nws);
            }
            LOOP2_ASSERT(NUM_VALS, j, NUM_VALS == j);
        }
#endif

        {
            const wchar_t vals[] =
                          L"abcd1234~`!@#$%^&*()_-+={}[]:;\"'<>,.?/|\\";
            enum { NUM_VALS = sizeof(vals) / sizeof *vals - 1};

            int j;
            wchar_t wc;

            Nwsstream nws;
            for (j = 0; j < NUM_VALS; ++j) {
                nws << vals[j];
            }
            ASSERT(nws.str() == vals);
            for (j = 0; nws >> wc; ++j) {
                ASSERT(vals[j] == wc);
            }
            ASSERT(NUM_VALS == j);

            bsl::basic_ostringstream<wchar_t,
                                     std::char_traits<wchar_t>,
                                     bsl::allocator<wchar_t> > bos;
            for (j = 0; j < NUM_VALS; ++j) {
                bos << vals[j];
            }
            ASSERT(bos.str() == vals);
        }

        {
            const wchar_t vals[] = L" \t\n\r";
            enum { NUM_VALS = sizeof(vals) / sizeof *vals - 1};

            int j;
            wchar_t wc;

            Nwsstream nws;
            for (j = 0; j < NUM_VALS; ++j) {
                nws << vals[j];
            }
            ASSERT(nws.str() == vals);
            for (j = 0; nws >> wc; ++j) {
                ASSERT(0);      // should skip whitespace
            }
            ASSERT(0 == j);

            bsl::basic_ostringstream<wchar_t,
                                     std::char_traits<wchar_t>,
                                     bsl::allocator<wchar_t> > bos;
            for (j = 0; j < NUM_VALS; ++j) {
                bos << vals[j];
            }
            ASSERT(bos.str() == vals);
        }

        {
            const int vals[] = { -2, -1, 0, 1, 2, 3, 4, 5, 6, 1023, 1024,
                                 -1023, -1024 };
                                 
            enum { NUM_VALS = sizeof vals / sizeof *vals };

            bsl::stringstream ss(&ta);
            int j, k;
            for (j = 0; j < NUM_VALS; ++j) {
                ss << vals[j] << " ";
            }
            LOOP_ASSERT(ss.str(),
                        ss.str() == "-2 -1 0 1 2 3 4 5 6 1023 1024 -1023 -1024 ");

            for (j = 0; ss >> k; ++j) {
                ASSERT(vals[j] == k);
            }
            ASSERT(NUM_VALS == j);
        }

        {
            double vals[] = {
                           0, 0.0, 1.0, 2, 3.21, 4.5, -7.3, 4.3e28, -2.4e-92 };

            enum { NUM_VALS = sizeof vals / sizeof *vals };

            bsl::stringstream ss(&ta);
            int j;
            for (j = 0; j < NUM_VALS; ++j) {
                ss << vals[j] << " ";
            }

#ifdef BSLS_PLATFORM__CMP_MSVC
            const char *EXPECTED = "0 0 1 2 3.21 4.5 -7.3 4.3e+028 -2.4e-092 ";
#else 
            const char *EXPECTED = "0 0 1 2 3.21 4.5 -7.3 4.3e+28 -2.4e-92 ";
#endif
            LOOP2_ASSERT(ss.str(), EXPECTED,
                        ss.str() == EXPECTED);
            double d;
            for (j = 0; ss >> d; ++j) {
                ASSERT(vals[j] == d);
            }
            ASSERT(NUM_VALS == j);
        }

        {
            float vals[] = {
                           0, 0.0, 1.0, 2, 3.21, 4.5, -7.3, 4.3e28, -2.4e-28 };
            enum { NUM_VALS = sizeof vals / sizeof *vals };

            bsl::stringstream ss(&ta);
            int j;
            for (j = 0; j < NUM_VALS; ++j) {
                ss << vals[j] << " ";
            }
#ifdef BSLS_PLATFORM__CMP_MSVC
            const char *EXPECTED ="0 0 1 2 3.21 4.5 -7.3 4.3e+028 -2.4e-028 ";
#else 
            const char *EXPECTED ="0 0 1 2 3.21 4.5 -7.3 4.3e+28 -2.4e-28 ";
#endif
            LOOP2_ASSERT(ss.str(), EXPECTED,
                        ss.str() == EXPECTED);

            float f;
            for (j = 0; ss >> f; ++j) {
                ASSERT(vals[j] == f);
            }
            ASSERT(NUM_VALS == j);
        }

        {
            const char *vals[] = { "woof", "arf", "meow", "1.1", "1.2.3.4",
                                   "1_2", "1-2", "~~~", "```", "!!!", "@@@",
                                   "###", "$$$", "%%%", "^^^", "&&&", "***",
                                   "(((", ")))", "---", "___", "+++", "===",
                                   "[[[", "]]]", ":::", ";;;", "'''",
                                   "\"\"\"", "<<<", ">>>", ",,,", "...",
                                   "???", "///", "|||", "\\\\\\" };
            const char *expected = "woof arf meow 1.1 1.2.3.4 1_2 1-2"
                                   " ~~~ ``` !!! @@@ ### $$$ %%% ^^^ &&& ***"
                                   " ((( ))) --- ___ +++ === [[[ ]]] ::: ;;;"
                                   " ''' \"\"\" <<< >>> ,,, ... ??? /// |||"
                                   " \\\\\\ ";
            enum { NUM_VALS = sizeof vals / sizeof *vals };

            native_std::stringstream osA;
            native_std::string ns;
            int j;
            for (j = 0; j < NUM_VALS; ++j) {
                osA << vals[j] << " ";
            }
            ASSERT(osA.str() == expected);
            for (j = 0; osA >> ns; ++j) {
                ASSERT(vals[j] == ns);
            }
            ASSERT(NUM_VALS == j);

            bsl::stringstream osB(&ta);
            bsl::string bs;
            for (j = 0; j < NUM_VALS; ++j) {
                osB << vals[j] << " ";
            }
            ASSERT(osB.str() == expected);
            for (j = 0; osB >> bs; ++j) {
                ASSERT(vals[j] == bs);
            }
            ASSERT(NUM_VALS == j);

            bsl::stringstream osC(&ta);
            for (j = 0; j < NUM_VALS; ++j) {
                osC << native_std::string(vals[j]) << " ";
            }
            ASSERT(osC.str() == expected);

            bsl::ostringstream osD(&ta);
            for (j = 0; j < NUM_VALS; ++j) {
                osD << bsl::string(vals[j]) << " ";
            }
            ASSERT(osD.str() == expected);
        }

        {
            const char vals[] = "abcd1234~`!@#$%^&*()_-+={}[]:;\"'<>,.?/|\\";
            const int NUM_VALS = std::strlen(vals) / sizeof *vals;

            int j;

            native_std::stringstream nss;
            for (j = 0; j < NUM_VALS; ++j) {
                nss << vals[j];
            }
            LOOP_ASSERT(nss.str(), nss.str() == vals);
            char c;
            for (j = 0; nss >> c; ++j) {
                ASSERT(vals[j] == c);
            }
            LOOP2_ASSERT(NUM_VALS, j, NUM_VALS == j);

            bsl::stringstream bss(&ta);
            for (j = 0; j < NUM_VALS; ++j) {
                bss << vals[j];
            }
            LOOP_ASSERT(bss.str(), bss.str() == vals);
            for (j = 0; bss >> c; ++j) {
                ASSERT(vals[j] == c);
            }
            ASSERT(NUM_VALS == j);
            LOOP2_ASSERT(NUM_VALS, j, NUM_VALS == j);
        }

        {
            // verify '>> char' ignores whitespace
            const char vals[] = " \t\n\r";
            const int NUM_VALS = std::strlen(vals) / sizeof *vals;

            native_std::stringstream nss;
            char c;
            int j;
            for (j = 0; j < NUM_VALS; ++j) {
                nss << vals[j];
            }
            ASSERT(nss.str() == vals);
            for (j = 0; nss >> c; ++j) {        // reading skips white space
                ASSERT(0);
            }
            ASSERT(0 == j);

            bsl::stringstream bss;
            for (j = 0; j < NUM_VALS; ++j) {
                bss << vals[j];
            }
            ASSERT(bss.str() == vals);
            for (j = 0; bss >> c; ++j) {
                ASSERT(0);
            }
            ASSERT(0 == j);
        }

        {
            bsl::istringstream is();
            const int vals[] = { -2, -1, 0, 1, 2, 3, 4, 5, 6, 1023, 1024,
                                 -1023, -1024 };
                                 
            enum { NUM_VALS = sizeof vals / sizeof *vals };

            Bwsstream bss(&ta);
            int j, k;
            for (j = 0; j < NUM_VALS; ++j) {
                bss << vals[j] << " ";
            }
            ASSERT(bss.str() == L"-2 -1 0 1 2 3 4 5 6 1023 1024 -1023 -1024 ");
            for (j = 0; bss >> k; ++j) {
                ASSERT(vals[j] == k);
            }
            ASSERT(NUM_VALS == j);
        }

        {
            double vals[] = {
                           0, 0.0, 1.0, 2, 3.21, 4.5, -7.3, 4.3e28, -2.4e-92 };
            enum { NUM_VALS = sizeof vals / sizeof *vals };

            Bwsstream bss(&ta);
            int j;
            for (j = 0; j < NUM_VALS; ++j) {
                bss << vals[j] << " ";
            }

#ifdef BSLS_PLATFORM__CMP_MSVC
            const wchar_t *EXPECTED =
                                  L"0 0 1 2 3.21 4.5 -7.3 4.3e+028 -2.4e-092 ";
#else 
            const wchar_t *EXPECTED =
                                    L"0 0 1 2 3.21 4.5 -7.3 4.3e+28 -2.4e-92 ";
#endif
            ASSERT(bss.str() == EXPECTED);

            double d;
            for (j = 0; bss >> d; ++j) {
                ASSERT(vals[j] == d);
            }
            ASSERT(NUM_VALS == j);
        }

        {
            float vals[] = {
                           0, 0.0, 1.0, 2, 3.21, 4.5, -7.3, 4.3e28, -2.4e-28 };
            enum { NUM_VALS = sizeof vals / sizeof *vals };

            bsl::basic_stringstream<wchar_t,
                                    std::char_traits<wchar_t>,
                                    bsl::allocator<wchar_t> > bss(&ta);
            int j;
            for (j = 0; j < NUM_VALS; ++j) {
                bss << vals[j] << " ";
            }

#ifdef BSLS_PLATFORM__CMP_MSVC
            const wchar_t *EXPECTED =
                                  L"0 0 1 2 3.21 4.5 -7.3 4.3e+028 -2.4e-028 ";
#else 
            const wchar_t *EXPECTED =
                                    L"0 0 1 2 3.21 4.5 -7.3 4.3e+28 -2.4e-28 ";
#endif
            ASSERT(bss.str() == EXPECTED);

            float f;
            for (j = 0; bss >> f; ++j) {
                ASSERT(vals[j] == f);
            }
            ASSERT(NUM_VALS == j);
        }

#if (!defined(BSLS_PLATFORM__OS_SOLARIS) && !defined(BSLS_PLATFORM__OS_SUNOS))\
   || defined(BSLS_PLATFORM__CMP_GNU)
        {
            typedef native_std::basic_stringstream<wchar_t,
                                     native_std::char_traits<wchar_t>,
                                     native_std::allocator<wchar_t> > Wsstream;
            typedef native_std::basic_string<wchar_t,
                                      native_std::char_traits<wchar_t>,
                                      native_std::allocator<wchar_t> > Wstring;

            Wsstream wss;
            Wstring ws;

            wss << L"arf" << L" meow" << L" woof" << L" growl";
            (void) wss.str();
            ASSERT(wss >> ws);
            ASSERT(L"arf" == ws);
        }

        {

            const wchar_t *vals[] = {L"woof", L"arf", L"meow", L"1.1",
                                     L"1.2.3.4", L"1_2", L"1-2", L"~~~",
                                     L"```", L"!!!", L"@@@", L"###", L"$$$",
                                     L"%%%", L"^^^", L"&&&", L"***", L"(((",
                                     L")))", L"---", L"___", L"+++", L"===",
                                     L"[[[", L"]]]", L":::", L";;;", L"'''",
                                     L"\"\"\"", L"<<<", L">>>", L",,,", L"...",
                                     L"???", L"///", L"|||", L"\\\\\\" };
            const wchar_t *expected = L"woof arf meow 1.1 1.2.3.4 1_2 1-2"
                                      L" ~~~ ``` !!! @@@ ### $$$ %%% ^^^ &&&"
                                      L" *** ((( ))) --- ___ +++ === [[[ ]]]"
                                      L" ::: ;;; ''' \"\"\" <<< >>> ,,, ..."
                                      L" ??? /// ||| \\\\\\ ";
            enum { NUM_VALS = sizeof vals / sizeof *vals };

            Nwsstream ssA;
            Nwstring nws;
            Bwsstream ssB(&ta);
            Bwstring bws(&ta);

            int j;

            for (j = 0; j < NUM_VALS; ++j) {
                ssA << vals[j] << L" ";
            }
            ASSERT(ssA.str() == expected);
            for (j = 0; ssA >> nws; ++j) {
                ASSERT(vals[j] == nws);
            }
            LOOP2_ASSERT(NUM_VALS, j, NUM_VALS == j);

            for (j = 0; j < NUM_VALS; ++j) {
                ssB << vals[j] << L" ";
            }
            ASSERT(ssB.str() == expected);
            for (j = 0; ssB >> nws; ++j) {
                ASSERT(vals[j] == nws);
            }
            LOOP2_ASSERT(NUM_VALS, j, NUM_VALS == j);

            Nwsstream ssC;
            for (j = 0; j < NUM_VALS; ++j) {
                ssC << Nwstring(vals[j]) << L" ";
            }
            ASSERT(ssC.str() == expected);
            for (j = 0; ssC >> nws; ++j) {
                ASSERT(vals[j] == nws);
            }
            LOOP2_ASSERT(NUM_VALS, j, NUM_VALS == j);

            Bwsstream ssD(&ta);
            for (j = 0; j < NUM_VALS; ++j) {
                ssD << Bwstring(vals[j]) << L" ";
            }
            ASSERT(ssD.str() == expected);
            for (j = 0; ssD >> bws; ++j) {
                ASSERT(vals[j] == bws);
            }
            LOOP2_ASSERT(NUM_VALS, j, NUM_VALS == j);

            Nwsstream ssE(expected);
            ASSERT(ssE.str() == expected);
            for (j = 0; ssE >> nws; ++j) {
                ASSERT(vals[j] == nws);
            }
            LOOP2_ASSERT(NUM_VALS, j, NUM_VALS == j);
        }
#endif

        {
            const wchar_t vals[] =
                          L"abcd1234~`!@#$%^&*()_-+={}[]:;\"'<>,.?/|\\";
            enum { NUM_VALS = sizeof(vals) / sizeof *vals - 1};

            int j;
            wchar_t wc;

            Nwsstream nws;
            for (j = 0; j < NUM_VALS; ++j) {
                nws << vals[j];
            }
            ASSERT(nws.str() == vals);
            for (j = 0; nws >> wc; ++j) {
                ASSERT(vals[j] == wc);
            }
            ASSERT(NUM_VALS == j);

            bsl::basic_ostringstream<wchar_t,
                                     std::char_traits<wchar_t>,
                                     bsl::allocator<wchar_t> > bos(&ta);
            for (j = 0; j < NUM_VALS; ++j) {
                bos << vals[j];
            }
            ASSERT(bos.str() == vals);
        }

        {
            const wchar_t vals[] = L" \t\n\r";
            enum { NUM_VALS = sizeof(vals) / sizeof *vals - 1};

            int j;
            wchar_t wc;

            Nwsstream nws;
            for (j = 0; j < NUM_VALS; ++j) {
                nws << vals[j];
            }
            ASSERT(nws.str() == vals);
            for (j = 0; nws >> wc; ++j) {
                ASSERT(0);      // should skip whitespace
            }
            ASSERT(0 == j);

            bsl::basic_ostringstream<wchar_t,
                                     std::char_traits<wchar_t>,
                                     bsl::allocator<wchar_t> > bos(&ta);
            for (j = 0; j < NUM_VALS; ++j) {
                bos << vals[j];
            }
            ASSERT(bos.str() == vals);
        }
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // OUT STRINGSTREAM TEST
        //
        // Concerns:
        //   That ostringstream works as specced.
        //
        // Plan:
        //   Create some ostringstreams and write various types to them,
        //   observing that the resultant strings are as expected.  Repeat the
        //   experiments passing a non-default allocator to the ostringstreams.
        // --------------------------------------------------------------------

        if (verbose) cout << "\nOUT STRINGSTREAM TEST\n"
                               "=====================\n";

        {
            const int vals[] = { -2, -1, 0, 1, 2, 3, 4, 5, 6, 1023, 1024,
                                 -1023, -1024 };
                                 
            enum { NUM_VALS = sizeof vals / sizeof *vals };

            bsl::ostringstream os;
            int j;
            for (j = 0; j < NUM_VALS; ++j) {
                os << vals[j] << " ";
            }
            ASSERT(os.str() == "-2 -1 0 1 2 3 4 5 6 1023 1024 -1023 -1024 ");
        }

        {
            double vals[] = {
                           0, 0.0, 1.0, 2, 3.21, 4.5, -7.3, 4.3e28, -2.4e-92 };
            enum { NUM_VALS = sizeof vals / sizeof *vals };

            bsl::ostringstream os;
            int j;
            for (j = 0; j < NUM_VALS; ++j) {
                os << vals[j] << " ";
            }
#ifdef BSLS_PLATFORM__OS_WINDOWS
            LOOP_ASSERT(os.str(),
                      os.str() == "0 0 1 2 3.21 4.5 -7.3 4.3e+028 -2.4e-092 ");
#else
            LOOP_ASSERT(os.str(),
                        os.str() == "0 0 1 2 3.21 4.5 -7.3 4.3e+28 -2.4e-92 ");
#endif
        }

        {
            float vals[] = {
                           0, 0.0, 1.0, 2, 3.21, 4.5, -7.3, 4.3e28, -2.4e-28 };
            enum { NUM_VALS = sizeof vals / sizeof *vals };

            bsl::ostringstream os;
            int j;
            for (j = 0; j < NUM_VALS; ++j) {
                os << vals[j] << " ";
            }
#ifdef BSLS_PLATFORM__OS_WINDOWS
            LOOP_ASSERT(os.str(),
                      os.str() == "0 0 1 2 3.21 4.5 -7.3 4.3e+028 -2.4e-028 ");
#else
            LOOP_ASSERT(os.str(),
                        os.str() == "0 0 1 2 3.21 4.5 -7.3 4.3e+28 -2.4e-28 ");
#endif
        }

        {
            const char *vals[] = { "woof", "arf", "meow", "1.1", "1.2.3.4",
                                   "1_2", "1-2", "~~~", "```", "!!!", "@@@",
                                   "###", "$$$", "%%%", "^^^", "&&&", "***",
                                   "(((", ")))", "---", "___", "+++", "===",
                                   "[[[", "]]]", ":::", ";;;", "'''",
                                   "\"\"\"", "<<<", ">>>", ",,,", "...",
                                   "???", "///", "|||", "\\\\\\" };
            const char *expected = "woof arf meow 1.1 1.2.3.4 1_2 1-2"
                                   " ~~~ ``` !!! @@@ ### $$$ %%% ^^^ &&& ***"
                                   " ((( ))) --- ___ +++ === [[[ ]]] ::: ;;;"
                                   " ''' \"\"\" <<< >>> ,,, ... ??? /// |||"
                                   " \\\\\\ ";
            enum { NUM_VALS = sizeof vals / sizeof *vals };

            native_std::ostringstream osA;
            int j;
            for (j = 0; j < NUM_VALS; ++j) {
                osA << vals[j] << " ";
            }
            ASSERT(osA.str() == expected);

            bsl::ostringstream osB;
            for (j = 0; j < NUM_VALS; ++j) {
                osB << vals[j] << " ";
            }
            ASSERT(osB.str() == expected);

            bsl::ostringstream osC;
            for (j = 0; j < NUM_VALS; ++j) {
                osC << native_std::string(vals[j]) << " ";
            }
            ASSERT(osC.str() == expected);

            bsl::ostringstream osD;
            for (j = 0; j < NUM_VALS; ++j) {
                osD << bsl::string(vals[j]) << " ";
            }
            ASSERT(osD.str() == expected);
        }

        {
            const char vals[] =
                           "abcd1234~`!@#$%^&*()_-+={}[]:;\"'<>,.?/|\\ \t\n\r";
            const int NUM_VALS = std::strlen(vals) / sizeof *vals;

            int j;

            native_std::ostringstream nos;
            for (j = 0; j < NUM_VALS; ++j) {
                nos << vals[j];
            }
            LOOP_ASSERT(nos.str(), nos.str() == vals);

            bsl::ostringstream bos;
            for (j = 0; j < NUM_VALS; ++j) {
                bos << vals[j];
            }
            LOOP_ASSERT(bos.str(), bos.str() == vals);
        }

        {
            const int vals[] = { -2, -1, 0, 1, 2, 3, 4, 5, 6, 1023, 1024,
                                 -1023, -1024 };
                                 
            enum { NUM_VALS = sizeof vals / sizeof *vals };

            bsl::basic_ostringstream<wchar_t,
                                     std::char_traits<wchar_t>,
                                     bsl::allocator<wchar_t> > os;
            int j;
            for (j = 0; j < NUM_VALS; ++j) {
                os << vals[j] << " ";
            }
            ASSERT(os.str() == L"-2 -1 0 1 2 3 4 5 6 1023 1024 -1023 -1024 ");
        }

        {
            double vals[] = {
                           0, 0.0, 1.0, 2, 3.21, 4.5, -7.3, 4.3e28, -2.4e-92 };
            enum { NUM_VALS = sizeof vals / sizeof *vals };

            bsl::basic_ostringstream<wchar_t,
                                     std::char_traits<wchar_t>,
                                     bsl::allocator<wchar_t> > os;
            int j;
            for (j = 0; j < NUM_VALS; ++j) {
                os << vals[j] << " ";
            }
#ifdef BSLS_PLATFORM__OS_WINDOWS
            ASSERT(os.str() == L"0 0 1 2 3.21 4.5 -7.3 4.3e+028 -2.4e-092 ");
#else
            ASSERT(os.str() == L"0 0 1 2 3.21 4.5 -7.3 4.3e+28 -2.4e-92 ");
#endif
        }

        {
            float vals[] = {
                           0, 0.0, 1.0, 2, 3.21, 4.5, -7.3, 4.3e28, -2.4e-28 };
            enum { NUM_VALS = sizeof vals / sizeof *vals };

            bsl::basic_ostringstream<wchar_t,
                                     std::char_traits<wchar_t>,
                                     bsl::allocator<wchar_t> > os;
            int j;
            for (j = 0; j < NUM_VALS; ++j) {
                os << vals[j] << " ";
            }
#ifdef BSLS_PLATFORM__OS_WINDOWS
            ASSERT(os.str() == L"0 0 1 2 3.21 4.5 -7.3 4.3e+028 -2.4e-028 ");
#else
            if (os.str() !=    L"0 0 1 2 3.21 4.5 -7.3 4.3e+28 -2.4e-28 ") {
                wcout << L"os.str(): " << os.str() << L"\n";
            }
            ASSERT(os.str() == L"0 0 1 2 3.21 4.5 -7.3 4.3e+28 -2.4e-28 ");
#endif
        }

        {
            typedef native_std::basic_string<wchar_t,
                                             std::char_traits<wchar_t>,
                                             bsl::allocator<wchar_t> >Nwstring;
            typedef bsl::basic_string<wchar_t,
                                      std::char_traits<wchar_t>,
                                      bsl::allocator<wchar_t> > Bwstring;
            typedef native_std::basic_ostringstream<wchar_t,
                                                    std::char_traits<wchar_t>,
                                                    bsl::allocator<wchar_t> >
                                                                     Nwsstream;
            typedef bsl::basic_ostringstream<wchar_t,
                                             std::char_traits<wchar_t>,
                                             bsl::allocator<wchar_t> >
                                                                     Bwsstream;

            const wchar_t *vals[] = {L"woof", L"arf", L"meow", L"1.1",
                                     L"1.2.3.4", L"1_2", L"1-2", L"~~~",
                                     L"```", L"!!!", L"@@@", L"###", L"$$$",
                                     L"%%%", L"^^^", L"&&&", L"***", L"(((",
                                     L")))", L"---", L"___", L"+++", L"===",
                                     L"[[[", L"]]]", L":::", L";;;", L"'''",
                                     L"\"\"\"", L"<<<", L">>>", L",,,", L"...",
                                     L"???", L"///", L"|||", L"\\\\\\" };
            const wchar_t *expected = L"woof arf meow 1.1 1.2.3.4 1_2 1-2"
                                      L" ~~~ ``` !!! @@@ ### $$$ %%% ^^^ &&&"
                                      L" *** ((( ))) --- ___ +++ === [[[ ]]]"
                                      L" ::: ;;; ''' \"\"\" <<< >>> ,,, ..."
                                      L" ??? /// ||| \\\\\\ ";
            enum { NUM_VALS = sizeof vals / sizeof *vals };

            Nwsstream osA;
            int j;
            for (j = 0; j < NUM_VALS; ++j) {
                osA << vals[j] << L" ";
            }
            ASSERT(osA.str() == expected);

            Bwsstream osB;
            for (j = 0; j < NUM_VALS; ++j) {
                osB << vals[j] << L" ";
            }
            ASSERT(osB.str() == expected);

            Nwsstream osC;
            for (j = 0; j < NUM_VALS; ++j) {
                osC << Nwstring(vals[j]) << L" ";
            }
            ASSERT(osC.str() == expected);

            Bwsstream osD;
            for (j = 0; j < NUM_VALS; ++j) {
                osD << Bwstring(vals[j]) << L" ";
            }
            ASSERT(osD.str() == expected);
        }

        {
            const wchar_t vals[] =
                          L"abcd1234~`!@#$%^&*()_-+={}[]:;\"'<>,.?/|\\ \t\n\r";
            enum { NUM_VALS = sizeof(vals) / sizeof *vals - 1};

            int j;

            native_std::basic_ostringstream<wchar_t,
                                            std::char_traits<wchar_t>,
                                            std::allocator<wchar_t> > nos;
            for (j = 0; j < NUM_VALS; ++j) {
                nos << vals[j];
            }
            ASSERT(nos.str() == vals);

            bsl::basic_ostringstream<wchar_t,
                                     std::char_traits<wchar_t>,
                                     bsl::allocator<wchar_t> > bos;
            for (j = 0; j < NUM_VALS; ++j) {
                bos << vals[j];
            }
            ASSERT(bos.str() == vals);
        }

        {
            const int vals[] = { -2, -1, 0, 1, 2, 3, 4, 5, 6, 1023, 1024,
                                 -1023, -1024 };
                                 
            enum { NUM_VALS = sizeof vals / sizeof *vals };

            bsl::ostringstream os(&ta);
            int j;
            for (j = 0; j < NUM_VALS; ++j) {
                os << vals[j] << " ";
            }
            ASSERT(os.str() == "-2 -1 0 1 2 3 4 5 6 1023 1024 -1023 -1024 ");
        }

        {
            double vals[] = {
                           0, 0.0, 1.0, 2, 3.21, 4.5, -7.3, 4.3e28, -2.4e-92 };
            enum { NUM_VALS = sizeof vals / sizeof *vals };

            bsl::ostringstream os(&ta);
            int j;
            for (j = 0; j < NUM_VALS; ++j) {
                os << vals[j] << " ";
            }
#ifdef BSLS_PLATFORM__OS_WINDOWS
            LOOP_ASSERT(os.str(),
                      os.str() == "0 0 1 2 3.21 4.5 -7.3 4.3e+028 -2.4e-092 ");
#else
            LOOP_ASSERT(os.str(),
                        os.str() == "0 0 1 2 3.21 4.5 -7.3 4.3e+28 -2.4e-92 ");
#endif
        }

        {
            float vals[] = {
                           0, 0.0, 1.0, 2, 3.21, 4.5, -7.3, 4.3e28, -2.4e-28 };
            enum { NUM_VALS = sizeof vals / sizeof *vals };

            bsl::ostringstream os(&ta);
            int j;
            for (j = 0; j < NUM_VALS; ++j) {
                os << vals[j] << " ";
            }
#ifdef BSLS_PLATFORM__OS_WINDOWS
            LOOP_ASSERT(os.str(),
                      os.str() == "0 0 1 2 3.21 4.5 -7.3 4.3e+028 -2.4e-028 ");
#else
            LOOP_ASSERT(os.str(),
                        os.str() == "0 0 1 2 3.21 4.5 -7.3 4.3e+28 -2.4e-28 ");
#endif
        }

        {
            const char *vals[] = { "woof", "arf", "meow", "1.1", "1.2.3.4",
                                   "1_2", "1-2", "~~~", "```", "!!!", "@@@",
                                   "###", "$$$", "%%%", "^^^", "&&&", "***",
                                   "(((", ")))", "---", "___", "+++", "===",
                                   "[[[", "]]]", ":::", ";;;", "'''",
                                   "\"\"\"", "<<<", ">>>", ",,,", "...",
                                   "???", "///", "|||", "\\\\\\" };
            const char *expected = "woof arf meow 1.1 1.2.3.4 1_2 1-2"
                                   " ~~~ ``` !!! @@@ ### $$$ %%% ^^^ &&& ***"
                                   " ((( ))) --- ___ +++ === [[[ ]]] ::: ;;;"
                                   " ''' \"\"\" <<< >>> ,,, ... ??? /// |||"
                                   " \\\\\\ ";
            enum { NUM_VALS = sizeof vals / sizeof *vals };

            native_std::ostringstream osA;
            int j;
            for (j = 0; j < NUM_VALS; ++j) {
                osA << vals[j] << " ";
            }
            ASSERT(osA.str() == expected);

            bsl::ostringstream osB(&ta);
            for (j = 0; j < NUM_VALS; ++j) {
                osB << vals[j] << " ";
            }
            ASSERT(osB.str() == expected);

            bsl::ostringstream osC(&ta);
            for (j = 0; j < NUM_VALS; ++j) {
                osC << native_std::string(vals[j]) << " ";
            }
            ASSERT(osC.str() == expected);

            bsl::ostringstream osD(&ta);
            for (j = 0; j < NUM_VALS; ++j) {
                osD << bsl::string(vals[j]) << " ";
            }
            ASSERT(osD.str() == expected);
        }

        {
            const char vals[] =
                           "abcd1234~`!@#$%^&*()_-+={}[]:;\"'<>,.?/|\\ \t\n\r";
            const int NUM_VALS = std::strlen(vals) / sizeof *vals;

            int j;

            native_std::ostringstream nos;
            for (j = 0; j < NUM_VALS; ++j) {
                nos << vals[j];
            }
            LOOP_ASSERT(nos.str(), nos.str() == vals);

            bsl::ostringstream bos(&ta);
            for (j = 0; j < NUM_VALS; ++j) {
                bos << vals[j];
            }
            LOOP_ASSERT(bos.str(), bos.str() == vals);
        }

        {
            const int vals[] = { -2, -1, 0, 1, 2, 3, 4, 5, 6, 1023, 1024,
                                 -1023, -1024 };
                                 
            enum { NUM_VALS = sizeof vals / sizeof *vals };

            bsl::basic_ostringstream<wchar_t,
                                     std::char_traits<wchar_t>,
                                     bsl::allocator<wchar_t> > os(&ta);
            int j;
            for (j = 0; j < NUM_VALS; ++j) {
                os << vals[j] << " ";
            }
            ASSERT(os.str() == L"-2 -1 0 1 2 3 4 5 6 1023 1024 -1023 -1024 ");
        }

        {
            double vals[] = {
                           0, 0.0, 1.0, 2, 3.21, 4.5, -7.3, 4.3e28, -2.4e-92 };
            enum { NUM_VALS = sizeof vals / sizeof *vals };

            bsl::basic_ostringstream<wchar_t,
                                     std::char_traits<wchar_t>,
                                     bsl::allocator<wchar_t> > os(&ta);
            int j;
            for (j = 0; j < NUM_VALS; ++j) {
                os << vals[j] << " ";
            }
#ifdef BSLS_PLATFORM__OS_WINDOWS
            ASSERT(os.str() == L"0 0 1 2 3.21 4.5 -7.3 4.3e+028 -2.4e-092 ");
#else
            ASSERT(os.str() == L"0 0 1 2 3.21 4.5 -7.3 4.3e+28 -2.4e-92 ");
#endif
        }

        {
            float vals[] = {
                           0, 0.0, 1.0, 2, 3.21, 4.5, -7.3, 4.3e28, -2.4e-28 };
            enum { NUM_VALS = sizeof vals / sizeof *vals };

            bsl::basic_ostringstream<wchar_t,
                                     std::char_traits<wchar_t>,
                                     bsl::allocator<wchar_t> > os(&ta);
            int j;
            for (j = 0; j < NUM_VALS; ++j) {
                os << vals[j] << " ";
            }
#ifdef BSLS_PLATFORM__OS_WINDOWS
            ASSERT(os.str() == L"0 0 1 2 3.21 4.5 -7.3 4.3e+028 -2.4e-028 ");
#else
            if (os.str() !=    L"0 0 1 2 3.21 4.5 -7.3 4.3e+28 -2.4e-28 ") {
                wcout << L"os.str(): " << os.str() << L"\n";
            }
            ASSERT(os.str() == L"0 0 1 2 3.21 4.5 -7.3 4.3e+28 -2.4e-28 ");
#endif
        }

        {
            typedef native_std::basic_string<wchar_t,
                                             std::char_traits<wchar_t>,
                                             bsl::allocator<wchar_t> >Nwstring;
            typedef bsl::basic_string<wchar_t,
                                      std::char_traits<wchar_t>,
                                      bsl::allocator<wchar_t> > Bwstring;
            typedef native_std::basic_ostringstream<wchar_t,
                                                    std::char_traits<wchar_t>,
                                                    bsl::allocator<wchar_t> >
                                                                     Nwsstream;
            typedef bsl::basic_ostringstream<wchar_t,
                                             std::char_traits<wchar_t>,
                                             bsl::allocator<wchar_t> >
                                                                     Bwsstream;

            const wchar_t *vals[] = {L"woof", L"arf", L"meow", L"1.1",
                                     L"1.2.3.4", L"1_2", L"1-2", L"~~~",
                                     L"```", L"!!!", L"@@@", L"###", L"$$$",
                                     L"%%%", L"^^^", L"&&&", L"***", L"(((",
                                     L")))", L"---", L"___", L"+++", L"===",
                                     L"[[[", L"]]]", L":::", L";;;", L"'''",
                                     L"\"\"\"", L"<<<", L">>>", L",,,", L"...",
                                     L"???", L"///", L"|||", L"\\\\\\" };
            const wchar_t *expected = L"woof arf meow 1.1 1.2.3.4 1_2 1-2"
                                      L" ~~~ ``` !!! @@@ ### $$$ %%% ^^^ &&&"
                                      L" *** ((( ))) --- ___ +++ === [[[ ]]]"
                                      L" ::: ;;; ''' \"\"\" <<< >>> ,,, ..."
                                      L" ??? /// ||| \\\\\\ ";
            enum { NUM_VALS = sizeof vals / sizeof *vals };

            Nwsstream osA;
            int j;
            for (j = 0; j < NUM_VALS; ++j) {
                osA << vals[j] << L" ";
            }
            ASSERT(osA.str() == expected);

            Bwsstream osB;
            for (j = 0; j < NUM_VALS; ++j) {
                osB << vals[j] << L" ";
            }
            ASSERT(osB.str() == expected);

            Nwsstream osC;
            for (j = 0; j < NUM_VALS; ++j) {
                osC << Nwstring(vals[j]) << L" ";
            }
            ASSERT(osC.str() == expected);

            Bwsstream osD;
            for (j = 0; j < NUM_VALS; ++j) {
                osD << Bwstring(vals[j]) << L" ";
            }
            ASSERT(osD.str() == expected);
        }

        {
            const wchar_t vals[] =
                          L"abcd1234~`!@#$%^&*()_-+={}[]:;\"'<>,.?/|\\ \t\n\r";
            enum { NUM_VALS = sizeof(vals) / sizeof *vals - 1};

            int j;

            native_std::basic_ostringstream<wchar_t,
                                            std::char_traits<wchar_t>,
                                            std::allocator<wchar_t> > nos;
            for (j = 0; j < NUM_VALS; ++j) {
                nos << vals[j];
            }
            ASSERT(nos.str() == vals);

            bsl::basic_ostringstream<wchar_t,
                                     std::char_traits<wchar_t>,
                                     bsl::allocator<wchar_t> > bos(&ta);
            for (j = 0; j < NUM_VALS; ++j) {
                bos << vals[j];
            }
            ASSERT(bos.str() == vals);
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // IN STRINGSTREAM TEST
        //
        // Concerns:
        //   That istringstream works as documented.
        //
        // Plan:
        //   Create istringstreams and read from them, various types.  Repeat
        //   the tests passing a non-default allocator to the istringstream.
        // --------------------------------------------------------------------

        if (verbose) cout << "\nIN STRINGSTREAM TEST\n"
                               "====================\n";

        {
            bsl::istringstream is("-2 -1 0 1 2 3 4 5 6 1023 1024 -1023 -1024");
            const int vals[] = { -2, -1, 0, 1, 2, 3, 4, 5, 6, 1023, 1024,
                                 -1023, -1024 };
            enum { NUM_VALS = sizeof vals / sizeof *vals };

            int j, k;
            for (j = 0; is >> k; ++j) {
                LOOP2_ASSERT(k, vals[j], k == vals[j]);
            }
            ASSERT(NUM_VALS == j);
        }

        {
            bsl::istringstream is("0 0.0 1.0 2 3.21 4.5 -7.3 4.3e28 -2.4e-92");
            double vals[] = {
                           0, 0.0, 1.0, 2, 3.21, 4.5, -7.3, 4.3e28, -2.4e-92 };
            enum { NUM_VALS = sizeof vals / sizeof *vals };

            int j;
            double d;
            for (j = 0; is >> d; ++j) {
                LOOP2_ASSERT(d, vals[j], ae(d, vals[j]));
            }
            ASSERT(NUM_VALS == j);
        }

        {
            bsl::istringstream is("0 0.0 1.0 2 3.21 4.5 -7.3 4.3e28 -2.4e-28");
            float vals[] = {
                           0, 0.0, 1.0, 2, 3.21, 4.5, -7.3, 4.3e28, -2.4e-28 };
            enum { NUM_VALS = sizeof vals / sizeof *vals };

            int j;
            float f;
            for (j = 0; is >> f; ++j) {
                LOOP2_ASSERT(f, vals[j], ae(f, vals[j]));
            }
            LOOP2_ASSERT(NUM_VALS, j, NUM_VALS == j);
        }

        {
            const char *data = "woof arf meow 1.1 1.2.3.4 1_2 1-2"
                               " ~~~ ``` !!! @@@ ### $$$ %%% ^^^ &&& ***"
                               " ((( ))) --- ___ +++ === [[[ ]]] ::: ;;;"
                               " ''' \"\"\" <<< >>> ,,, ... ??? /// |||"
                               " \\\\\\";
            const char *vals[] = { "woof", "arf", "meow", "1.1", "1.2.3.4",
                                   "1_2", "1-2", "~~~", "```", "!!!", "@@@",
                                   "###", "$$$", "%%%", "^^^", "&&&", "***",
                                   "(((", ")))", "---", "___", "+++", "===",
                                   "[[[", "]]]", ":::", ";;;", "'''",
                                   "\"\"\"", "<<<", ">>>", ",,,", "...",
                                   "???", "///", "|||", "\\\\\\" };
            enum { NUM_VALS = sizeof vals / sizeof *vals };

            bsl::istringstream isA(data);
            bsl::string s;
            int j;
            for (j = 0; isA >> s; ++j) {
                LOOP2_ASSERT(s, vals[j], s == vals[j]);
            }
            ASSERT(NUM_VALS == j);

            bsl::istringstream isB(data);
            native_std::string ns;
            for (j = 0; isB >> ns; ++j) {
                LOOP2_ASSERT(ns, vals[j], ns == vals[j]);
            }
            LOOP2_ASSERT(NUM_VALS, j, NUM_VALS == j);
        }

        {
            const char vals[] = "abcd1234~`!@#$%^&*()_-+={}[]:;\"'<>,.?/|\\";
            const int NUM_VALS = std::strlen(vals) / sizeof *vals;

            native_std::istringstream nis(vals);
            char c;
            int j;
            for (j = 0; nis >> c; ++j) {
                LOOP2_ASSERT(c, vals[j], c == vals[j]);
            }
            LOOP3_ASSERT(NUM_VALS, j, vals[j], NUM_VALS == j);

            bsl::istringstream bis(vals);
            for (j = 0; bis >> c; ++j) {
                LOOP2_ASSERT(c, vals[j], c == vals[j]);
            }
            LOOP3_ASSERT(NUM_VALS, j, vals[j], NUM_VALS == j);
        }

        {
            // verify '>> char' ignores whitespace
            const char vals[] = " \t\n\r";

            native_std::istringstream nis(vals);
            char c;
            int j;
            for (j = 0; nis >> c; ++j) {
                ASSERT(0);
            }
            ASSERT(0 == j);

            bsl::istringstream bis(vals);
            for (j = 0; bis >> c; ++j) {
                ASSERT(0);
            }
            ASSERT(0 == j);
        }

        {
            const wchar_t vals[] =
                                 L"abcd1234~`!@#$%^&*()_-+={}[]:;\"'<>,.?/|\\";
            enum { NUM_VALS = sizeof(vals) / sizeof *vals - 1};

            wchar_t c;
            int j;

            native_std::basic_istringstream<wchar_t,
                                            std::char_traits<wchar_t>,
                                            std::allocator<wchar_t> >
                                                                     nis(vals);
            for (j = 0; nis >> c; ++j) {
                LOOP2_ASSERT(c, vals[j], c == vals[j]);
            }
            LOOP3_ASSERT(NUM_VALS, j, vals[j], NUM_VALS == j);

            bsl::basic_istringstream<wchar_t,
                                     std::char_traits<wchar_t>,
                                     bsl::allocator<wchar_t> > bis(vals);
            for (j = 0; bis >> c; ++j) {
                LOOP2_ASSERT(c, vals[j], c == vals[j]);
            }
            LOOP3_ASSERT(NUM_VALS, j, vals[j], NUM_VALS == j);
        }

        {
            // verify '>> char' ignores whitespace
            const wchar_t vals[] = L" \t\n\r";

            native_std::basic_istringstream<wchar_t,
                                            std::char_traits<wchar_t>,
                                            std::allocator<wchar_t> >
                                                                     nis(vals);
            wchar_t c;
            int j;
            for (j = 0; nis >> c; ++j) {
                ASSERT(0);
            }
            ASSERT(0 == j);

            bsl::basic_istringstream<wchar_t,
                                     std::char_traits<wchar_t>,
                                     bsl::allocator<wchar_t> > bis(vals);
            for (j = 0; bis >> c; ++j) {
                ASSERT(0);
            }
            ASSERT(0 == j);
        }

        {
            bsl::istringstream is("-2 -1 0 1 2 3 4 5 6 1023 1024 -1023 -1024",
                                  &ta);
            const int vals[] = { -2, -1, 0, 1, 2, 3, 4, 5, 6, 1023, 1024,
                                 -1023, -1024 };
            enum { NUM_VALS = sizeof vals / sizeof *vals };

            int j, k;
            for (j = 0; is >> k; ++j) {
                LOOP2_ASSERT(k, vals[j], k == vals[j]);
            }
            ASSERT(NUM_VALS == j);
        }

        {
            bsl::istringstream is("0 0.0 1.0 2 3.21 4.5 -7.3 4.3e28 -2.4e-92",
                                  &ta);
            double vals[] = {
                           0, 0.0, 1.0, 2, 3.21, 4.5, -7.3, 4.3e28, -2.4e-92 };
            enum { NUM_VALS = sizeof vals / sizeof *vals };

            int j;
            double d;
            for (j = 0; is >> d; ++j) {
                LOOP2_ASSERT(d, vals[j], ae(d, vals[j]));
            }
            ASSERT(NUM_VALS == j);
        }

        {
            bsl::istringstream is("0 0.0 1.0 2 3.21 4.5 -7.3 4.3e28 -2.4e-28",
                                  &ta);
            float vals[] = {
                           0, 0.0, 1.0, 2, 3.21, 4.5, -7.3, 4.3e28, -2.4e-28 };
            enum { NUM_VALS = sizeof vals / sizeof *vals };

            int j;
            float f;
            for (j = 0; is >> f; ++j) {
                LOOP2_ASSERT(f, vals[j], ae(f, vals[j]));
            }
            LOOP2_ASSERT(NUM_VALS, j, NUM_VALS == j);
        }

        {
            const char *data = "woof arf meow 1.1 1.2.3.4 1_2 1-2"
                               " ~~~ ``` !!! @@@ ### $$$ %%% ^^^ &&& ***"
                               " ((( ))) --- ___ +++ === [[[ ]]] ::: ;;;"
                               " ''' \"\"\" <<< >>> ,,, ... ??? /// |||"
                               " \\\\\\";
            const char *vals[] = { "woof", "arf", "meow", "1.1", "1.2.3.4",
                                   "1_2", "1-2", "~~~", "```", "!!!", "@@@",
                                   "###", "$$$", "%%%", "^^^", "&&&", "***",
                                   "(((", ")))", "---", "___", "+++", "===",
                                   "[[[", "]]]", ":::", ";;;", "'''",
                                   "\"\"\"", "<<<", ">>>", ",,,", "...",
                                   "???", "///", "|||", "\\\\\\" };
            enum { NUM_VALS = sizeof vals / sizeof *vals };

            bsl::istringstream isA(data, &ta);
            bsl::string s;
            int j;
            for (j = 0; isA >> s; ++j) {
                LOOP2_ASSERT(s, vals[j], s == vals[j]);
            }
            ASSERT(NUM_VALS == j);

            bsl::istringstream isB(data);
            native_std::string ns;
            for (j = 0; isB >> ns; ++j) {
                LOOP2_ASSERT(ns, vals[j], ns == vals[j]);
            }
            LOOP2_ASSERT(NUM_VALS, j, NUM_VALS == j);
        }

        {
            const char vals[] = "abcd1234~`!@#$%^&*()_-+={}[]:;\"'<>,.?/|\\";
            const int NUM_VALS = std::strlen(vals) / sizeof *vals;

            native_std::istringstream nis(vals);
            char c;
            int j;
            for (j = 0; nis >> c; ++j) {
                LOOP2_ASSERT(c, vals[j], c == vals[j]);
            }
            LOOP3_ASSERT(NUM_VALS, j, vals[j], NUM_VALS == j);

            bsl::istringstream bis(vals, &ta);
            for (j = 0; bis >> c; ++j) {
                LOOP2_ASSERT(c, vals[j], c == vals[j]);
            }
            LOOP3_ASSERT(NUM_VALS, j, vals[j], NUM_VALS == j);
        }

        {
            // verify '>> char' ignores whitespace
            const char vals[] = " \t\n\r";

            native_std::istringstream nis(vals);
            char c;
            int j;
            for (j = 0; nis >> c; ++j) {
                ASSERT(0);
            }
            ASSERT(0 == j);

            bsl::istringstream bis(vals, &ta);
            for (j = 0; bis >> c; ++j) {
                ASSERT(0);
            }
            ASSERT(0 == j);
        }

        {
            const wchar_t vals[] =
                                 L"abcd1234~`!@#$%^&*()_-+={}[]:;\"'<>,.?/|\\";
            enum { NUM_VALS = sizeof(vals) / sizeof *vals - 1};

            wchar_t c;
            int j;

            native_std::basic_istringstream<wchar_t,
                                            std::char_traits<wchar_t>,
                                            std::allocator<wchar_t> >
                                                                     nis(vals);
            for (j = 0; nis >> c; ++j) {
                LOOP2_ASSERT(c, vals[j], c == vals[j]);
            }
            LOOP3_ASSERT(NUM_VALS, j, vals[j], NUM_VALS == j);

            bsl::basic_istringstream<wchar_t,
                                     std::char_traits<wchar_t>,
                                     bsl::allocator<wchar_t> > bis(vals, &ta);
            for (j = 0; bis >> c; ++j) {
                LOOP2_ASSERT(c, vals[j], c == vals[j]);
            }
            LOOP3_ASSERT(NUM_VALS, j, vals[j], NUM_VALS == j);
        }

        {
            // verify '>> char' ignores whitespace
            const wchar_t vals[] = L" \t\n\r";

            native_std::basic_istringstream<wchar_t,
                                            std::char_traits<wchar_t>,
                                            std::allocator<wchar_t> >
                                                                     nis(vals);
            wchar_t c;
            int j;
            for (j = 0; nis >> c; ++j) {
                ASSERT(0);
            }
            ASSERT(0 == j);

            bsl::basic_istringstream<wchar_t,
                                     std::char_traits<wchar_t>,
                                     bsl::allocator<wchar_t> > bis(vals, &ta);
            for (j = 0; bis >> c; ++j) {
                ASSERT(0);
            }
            ASSERT(0 == j);
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // MEMORY CONSUMPTION TEST
        //
        // Concerns:
        //   That the memory allocator is getting properly used in
        //   stringstreams.
        //
        // Plan:
        //   Create and use some stringstreams, and verify that their memory
        //   is coming from the correct allocator and not from the default
        //   allocator.
        // --------------------------------------------------------------------

        if (verbose) cout << "\nMEMORY CONSUMPTION TEST\n"
                               "=======================\n";

        {
            const int vals[] = { -2, -1, 0, 1, 2, 3, 4, 5, 6, 1023, 1024,
                                 -1023, -1024 };
            enum { NUM_VALS = sizeof vals / sizeof *vals };

            int memUsed = ta.numBytesInUse();
            int defaultMemUsed = tda.numBytesInUse();

            const char initStr[] = "-2 -1 0 1 2 3 4 5 6 1023 1024 -1023 -1024";

            bsl::istringstream is(initStr, &ta);

            ASSERT(ta.numBytesInUse() >= memUsed + (int) sizeof(initStr) - 1);
            ASSERT(tda.numBytesInUse() == defaultMemUsed);

            int j, k;
            for (j = 0; is >> k; ++j) {
                LOOP2_ASSERT(k, vals[j], k == vals[j]);
            }
            ASSERT(NUM_VALS == j);

            ASSERT(tda.numBytesInUse() == defaultMemUsed);
        }

        {
            const int vals[] = { -2, -1, 0, 1, 2, 3, 4, 5, 6, 1023, 1024,
                                 -1023, -1024 };
            enum { NUM_VALS = sizeof vals / sizeof *vals };
            const char compareStr[] =
                                  "-2 -1 0 1 2 3 4 5 6 1023 1024 -1023 -1024 ";

            const int memUsed = ta.numBytesInUse();
            const int defaultMemUsed = tda.numBytesInUse();

            bsl::ostringstream os(&ta);
            int j;
            for (j = 0; j < NUM_VALS; ++j) {
                os << vals[j] << " ";
            }
            ASSERT(os.str() == compareStr);

            ASSERT(ta.numBytesInUse() >= memUsed + (int)sizeof(compareStr)- 1);
            ASSERT(tda.numBytesInUse() == defaultMemUsed);
        }

        {
            const int vals[] = { -2, -1, 0, 1, 2, 3, 4, 5, 6, 1023, 1024,
                                 -1023, -1024 };
            enum { NUM_VALS = sizeof vals / sizeof *vals };
            const char compareStr[] =
                                  "-2 -1 0 1 2 3 4 5 6 1023 1024 -1023 -1024 ";

            const int memUsed = ta.numBytesInUse();
            const int defaultMemUsed = tda.numBytesInUse();

            bsl::stringstream ss(&ta);
            int j, k;
            for (j = 0; j < NUM_VALS; ++j) {
                ss << vals[j] << " ";
            }
            LOOP_ASSERT(ss.str(), ss.str() == compareStr);

            ASSERT(ta.numBytesInUse() >= memUsed + (int)sizeof(compareStr)- 1);
            ASSERT(tda.numBytesInUse() == defaultMemUsed);

            for (j = 0; ss >> k; ++j) {
                ASSERT(vals[j] == k);
            }
            ASSERT(NUM_VALS == j);

            ASSERT(tda.numBytesInUse() == defaultMemUsed);
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //
        // Concerns:
        //   That bsl::stringstream is a different type than std::stringstream,
        //   and that bsl::stringstream allocates while std::stringstream
        //   does not.
        //
        // Plan:
        //   Use the 'sameType' and 'usesBslmaAllocator' template functions
        //   to perform the tests.
        // --------------------------------------------------------------------

        if (verbose) cout << "\nBREATHING TEST\n"
                               "==============\n";

        // technically, if one binds a (ref to a const) to a temporary, the
        // C++ std says the temporary is to be copied to another temporary
        // that is bound to the reference.  But the compiler is allowed to
        // optimize this copy away and just bind to the original temporary.
        // Pretty much all compilers do this optimization, but the g++
        // compiler, being paranoid, checks that you are able to do the copy.
        // stringstream contains things with private copy c'tors, so it cannot
        // be copied, causing errors.  So we pass non-temporaries to the
        // routines.

        bsl::stringstream b;
        bsl::istringstream bi;
        bsl::ostringstream bo;
        std::stringstream s;
        std::istringstream si;
        std::ostringstream so;
        native_std::stringstream n;
        native_std::istringstream ni;
        native_std::ostringstream no;

        ASSERT(false == sameType(b, n));
        ASSERT(false == sameType(s, n));
        ASSERT(true  == sameType(b, s));
        ASSERT(true  == usesBslmaAllocator(b));
        ASSERT(true  == usesBslmaAllocator(s));
        ASSERT(false == usesBslmaAllocator(n));

        ASSERT(false == sameType(bi, ni));
        ASSERT(false == sameType(si, ni));
        ASSERT(true  == sameType(bi, si));
        ASSERT(true  == usesBslmaAllocator(bi));
        ASSERT(true  == usesBslmaAllocator(si));
        ASSERT(false == usesBslmaAllocator(ni));

        ASSERT(false == sameType(bo, no));
        ASSERT(false == sameType(so, no));
        ASSERT(true  == sameType(bo, so));
        ASSERT(true  == usesBslmaAllocator(bo));
        ASSERT(true  == usesBslmaAllocator(so));
        ASSERT(false == usesBslmaAllocator(no));
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
