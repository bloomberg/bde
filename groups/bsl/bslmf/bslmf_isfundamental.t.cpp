// bslmf_isfundamental.t.cpp                                          -*-C++-*-

#include <bslmf_isfundamental.h>

#include <bsls_platform.h>
#include <bsls_types.h>

#include <cstdlib>     // atoi()
#include <iostream>

using namespace BloombergLP;
using namespace std;

//=============================================================================
//                                TEST PLAN
//-----------------------------------------------------------------------------
//                                Overview
//                                --------
//-----------------------------------------------------------------------------
// [ 1] bslmf_IsFundamental
//=============================================================================
//                  STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
static int testStatus = 0;

static void aSsErT(int c, const char *s, int i) {
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}
#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }
//-----------------------------------------------------------------------------
#define LOOP_ASSERT(I,X) { \
    if (!(X)) { cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__);}}

#define LOOP2_ASSERT(I,J,X) { \
    if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " \
        << J << "\n"; aSsErT(1, #X, __LINE__); } }

//=============================================================================
#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", " << flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number
#define T_() cout << '\t' << flush;           // Print tab w/o linefeed.

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

struct TestType {};

static char C0[1 + bslmf_IsFundamental<TestType>::VALUE];          // sz==1
static char C1[1 + bslmf_IsFundamental<TestType const>::VALUE];    // sz==1
static char C2[1 + bslmf_IsFundamental<TestType volatile>::VALUE]; // sz==1
static char C3[1 + bslmf_IsFundamental<int>::VALUE];               // sz==2
static char C4[1 + bslmf_IsFundamental<int const>::VALUE];         // sz==2
static char C5[1 + bslmf_IsFundamental<int volatile>::VALUE];      // sz==2

// from component doc

struct MyType {};

static const int a1 = bslmf_IsFundamental<int>::VALUE;          // a1 == 1
static const int a2 = bslmf_IsFundamental<const int>::VALUE;    // a2 == 1
static const int a3 = bslmf_IsFundamental<volatile int>::VALUE; // a3 == 1
static const int a4 = bslmf_IsFundamental<int *>::VALUE;        // a4 == 0
static const int a5 = bslmf_IsFundamental<MyType>::VALUE;       // a5 == 0

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    // int veryVerbose = argc > 3;

    // Suppress compiler warning about static variables that were never used.
    (void) C0[0];
    (void) C1[0];
    (void) C2[0];
    (void) C3[0];
    (void) C4[0];
    (void) C5[0];

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:  // Zero is always the leading case.
      case 1: {
        // --------------------------------------------------------------------
        // Test Plan:
        //   Instantiate 'bslmf_IsFundamental' with various types and verify
        //   that their 'VALUE' member is initialized properly.
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "bslmf_IsFundamental" << endl
                          << "===================" << endl;

        ASSERT(1 == sizeof(C0));
        ASSERT(1 == sizeof(C1));
        ASSERT(1 == sizeof(C2));
        ASSERT(2 == sizeof(C3));
        ASSERT(2 == sizeof(C4));
        ASSERT(2 == sizeof(C5));

        ASSERT(1 == a1);
        ASSERT(1 == a2);
        ASSERT(1 == a3);
        ASSERT(0 == a4);
        ASSERT(0 == a5);

        ASSERT(1 == bslmf_IsFundamental<bool>::VALUE);
        ASSERT(1 == bslmf_IsFundamental<char>::VALUE);
        ASSERT(1 == bslmf_IsFundamental<signed char>::VALUE);
        ASSERT(1 == bslmf_IsFundamental<unsigned char>::VALUE);
        ASSERT(1 == bslmf_IsFundamental<wchar_t>::VALUE);
        ASSERT(1 == bslmf_IsFundamental<short>::VALUE);
        ASSERT(1 == bslmf_IsFundamental<unsigned short>::VALUE);
        ASSERT(1 == bslmf_IsFundamental<int>::VALUE);
        ASSERT(1 == bslmf_IsFundamental<unsigned int>::VALUE);
        ASSERT(1 == bslmf_IsFundamental<long>::VALUE);
        ASSERT(1 == bslmf_IsFundamental<unsigned long>::VALUE);
        ASSERT(1 == bslmf_IsFundamental<long long>::VALUE);
        ASSERT(1 == bslmf_IsFundamental<unsigned long long>::VALUE);
        ASSERT(1 == bslmf_IsFundamental<bsls_Types::Int64>::VALUE);
        ASSERT(1 == bslmf_IsFundamental<bsls_Types::Uint64>::VALUE);
        ASSERT(1 == bslmf_IsFundamental<float>::VALUE);
        ASSERT(1 == bslmf_IsFundamental<double>::VALUE);
        ASSERT(1 == bslmf_IsFundamental<long double>::VALUE);
        ASSERT(1 == bslmf_IsFundamental<void>::VALUE);
        ASSERT(1 == bslmf_IsFundamental<bsls_Types::Int64>::VALUE);

        ASSERT(1 == bslmf_IsFundamental<bool const>::VALUE);
        ASSERT(1 == bslmf_IsFundamental<char const>::VALUE);
        ASSERT(1 == bslmf_IsFundamental<signed char const>::VALUE);
        ASSERT(1 == bslmf_IsFundamental<unsigned char const>::VALUE);
        ASSERT(1 == bslmf_IsFundamental<wchar_t const>::VALUE);
        ASSERT(1 == bslmf_IsFundamental<short const>::VALUE);
        ASSERT(1 == bslmf_IsFundamental<unsigned short const>::VALUE);
        ASSERT(1 == bslmf_IsFundamental<int const>::VALUE);
        ASSERT(1 == bslmf_IsFundamental<unsigned int const>::VALUE);
        ASSERT(1 == bslmf_IsFundamental<long const>::VALUE);
        ASSERT(1 == bslmf_IsFundamental<unsigned long const>::VALUE);
        ASSERT(1 == bslmf_IsFundamental<float const>::VALUE);
        ASSERT(1 == bslmf_IsFundamental<double const>::VALUE);
        ASSERT(1 == bslmf_IsFundamental<long double const>::VALUE);
        ASSERT(1 == bslmf_IsFundamental<void const>::VALUE);
        ASSERT(1 == bslmf_IsFundamental<bsls_Types::Int64 const>::VALUE);

        ASSERT(1 == bslmf_IsFundamental<bool volatile>::VALUE);
        ASSERT(1 == bslmf_IsFundamental<char volatile>::VALUE);
        ASSERT(1 == bslmf_IsFundamental<signed char volatile>::VALUE);
        ASSERT(1 == bslmf_IsFundamental<unsigned char volatile>::VALUE);
        ASSERT(1 == bslmf_IsFundamental<wchar_t volatile>::VALUE);
        ASSERT(1 == bslmf_IsFundamental<short volatile>::VALUE);
        ASSERT(1 == bslmf_IsFundamental<unsigned short volatile>::VALUE);
        ASSERT(1 == bslmf_IsFundamental<int volatile>::VALUE);
        ASSERT(1 == bslmf_IsFundamental<unsigned int volatile>::VALUE);
        ASSERT(1 == bslmf_IsFundamental<long volatile>::VALUE);
        ASSERT(1 == bslmf_IsFundamental<unsigned long volatile>::VALUE);
        ASSERT(1 == bslmf_IsFundamental<float volatile>::VALUE);
        ASSERT(1 == bslmf_IsFundamental<double volatile>::VALUE);
        ASSERT(1 == bslmf_IsFundamental<long double volatile>::VALUE);
        ASSERT(1 == bslmf_IsFundamental<void volatile>::VALUE);
        ASSERT(1 == bslmf_IsFundamental<bsls_Types::Int64 volatile>::VALUE);

        ASSERT(1 == bslmf_IsFundamental<const bool volatile>::VALUE);
        ASSERT(1 == bslmf_IsFundamental<const char volatile>::VALUE);
        ASSERT(1 == bslmf_IsFundamental<const signed char volatile>::VALUE);
        ASSERT(1 == bslmf_IsFundamental<const unsigned char volatile>::VALUE);
        ASSERT(1 == bslmf_IsFundamental<const wchar_t volatile>::VALUE);
        ASSERT(1 == bslmf_IsFundamental<const short volatile>::VALUE);
        ASSERT(1 == bslmf_IsFundamental<const unsigned short volatile>::VALUE);
        ASSERT(1 == bslmf_IsFundamental<const int volatile>::VALUE);
        ASSERT(1 == bslmf_IsFundamental<const unsigned int volatile>::VALUE);
        ASSERT(1 == bslmf_IsFundamental<const long volatile>::VALUE);
        ASSERT(1 == bslmf_IsFundamental<const unsigned long volatile>::VALUE);
        ASSERT(1 == bslmf_IsFundamental<const float volatile>::VALUE);
        ASSERT(1 == bslmf_IsFundamental<const double volatile>::VALUE);
        ASSERT(1 == bslmf_IsFundamental<const long double volatile>::VALUE);
        ASSERT(1 == bslmf_IsFundamental<const void volatile>::VALUE);
        ASSERT(1 ==
                 bslmf_IsFundamental<const bsls_Types::Int64 volatile>::VALUE);

        ASSERT(1 == bslmf_IsFundamental<bool&>::VALUE);
        ASSERT(1 == bslmf_IsFundamental<char&>::VALUE);
        ASSERT(1 == bslmf_IsFundamental<signed char&>::VALUE);
        ASSERT(1 == bslmf_IsFundamental<unsigned char&>::VALUE);
        ASSERT(1 == bslmf_IsFundamental<wchar_t&>::VALUE);
        ASSERT(1 == bslmf_IsFundamental<short&>::VALUE);
        ASSERT(1 == bslmf_IsFundamental<unsigned short&>::VALUE);
        ASSERT(1 == bslmf_IsFundamental<int&>::VALUE);
        ASSERT(1 == bslmf_IsFundamental<unsigned int&>::VALUE);
        ASSERT(1 == bslmf_IsFundamental<long&>::VALUE);
        ASSERT(1 == bslmf_IsFundamental<unsigned long&>::VALUE);
        ASSERT(1 == bslmf_IsFundamental<float&>::VALUE);
        ASSERT(1 == bslmf_IsFundamental<double&>::VALUE);
        ASSERT(1 == bslmf_IsFundamental<long double&>::VALUE);
        ASSERT(1 == bslmf_IsFundamental<bsls_Types::Int64&>::VALUE);

        ASSERT(1 == bslmf_IsFundamental<const bool volatile&>::VALUE);
        ASSERT(1 == bslmf_IsFundamental<const char volatile&>::VALUE);
        ASSERT(1 == bslmf_IsFundamental<const signed char volatile&>::VALUE);
        ASSERT(1 == bslmf_IsFundamental<const unsigned char volatile&>::VALUE);
        ASSERT(1 == bslmf_IsFundamental<const wchar_t volatile&>::VALUE);
        ASSERT(1 == bslmf_IsFundamental<const short volatile&>::VALUE);
        ASSERT(1 ==
               bslmf_IsFundamental<const unsigned short volatile&>::VALUE);
        ASSERT(1 == bslmf_IsFundamental<const int volatile&>::VALUE);
        ASSERT(1 == bslmf_IsFundamental<const unsigned int volatile&>::VALUE);
        ASSERT(1 == bslmf_IsFundamental<const long volatile&>::VALUE);
        ASSERT(1 == bslmf_IsFundamental<const unsigned long volatile&>::VALUE);
        ASSERT(1 == bslmf_IsFundamental<const float volatile&>::VALUE);
        ASSERT(1 == bslmf_IsFundamental<const double volatile&>::VALUE);
        ASSERT(1 == bslmf_IsFundamental<const long double volatile&>::VALUE);
        ASSERT(1 ==
                bslmf_IsFundamental<const bsls_Types::Int64 volatile&>::VALUE);

        ASSERT(0 == bslmf_IsFundamental<bool *>::VALUE);
        ASSERT(0 == bslmf_IsFundamental<char *>::VALUE);
        ASSERT(0 == bslmf_IsFundamental<signed char *>::VALUE);
        ASSERT(0 == bslmf_IsFundamental<unsigned char *>::VALUE);
        ASSERT(0 == bslmf_IsFundamental<wchar_t *>::VALUE);
        ASSERT(0 == bslmf_IsFundamental<short *>::VALUE);
        ASSERT(0 == bslmf_IsFundamental<unsigned short *>::VALUE);
        ASSERT(0 == bslmf_IsFundamental<int *>::VALUE);
        ASSERT(0 == bslmf_IsFundamental<unsigned int *>::VALUE);
        ASSERT(0 == bslmf_IsFundamental<long *>::VALUE);
        ASSERT(0 == bslmf_IsFundamental<unsigned long *>::VALUE);
        ASSERT(0 == bslmf_IsFundamental<float *>::VALUE);
        ASSERT(0 == bslmf_IsFundamental<double *>::VALUE);
        ASSERT(0 == bslmf_IsFundamental<long double *>::VALUE);
        ASSERT(0 == bslmf_IsFundamental<void *>::VALUE);
        ASSERT(0 == bslmf_IsFundamental<bsls_Types::Int64 *>::VALUE);

        // The following typedef's are necessitated by cc-5.2 (Sun).  But they
        // don't help cc-5.5 compilations.
        typedef void    VF  ();
        typedef void (*PVFI)(int);
        ASSERT(0 == bslmf_IsFundamental< VF >::VALUE);
        ASSERT(0 == bslmf_IsFundamental<PVFI>::VALUE);

      } break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        cerr << "Error, non-zero test status = "
             << testStatus << "." << endl;
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
