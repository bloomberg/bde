// bslmf_ispointer.t.cpp                                              -*-C++-*-

#include <bslmf_ispointer.h>

#include <cstdlib>    // atoi()
#include <cstring>    // strcmp()
#include <iostream>

using namespace BloombergLP;
using namespace std;

//=============================================================================
//                                TEST PLAN
//-----------------------------------------------------------------------------
//                                Overview
//                                --------
//-----------------------------------------------------------------------------
// [ 1] bslmf::IsPointer
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

enum {
    C00 = 1 + bslmf::IsPointer<int*>::VALUE,                               // 2
    C01 = 1 + bslmf::IsPointer<int *const>::VALUE,                         // 2
    C02 = 1 + bslmf::IsPointer<int *volatile>::VALUE,                      // 2
    C03 = 1 + bslmf::IsPointer<int *const volatile>::VALUE,                // 2
    C04 = 1 + bslmf::IsPointer<const int*>::VALUE,                         // 2
    C05 = 1 + bslmf::IsPointer<const int *const>::VALUE,                   // 2
    C06 = 1 + bslmf::IsPointer<const int *volatile>::VALUE,                // 2
    C07 = 1 + bslmf::IsPointer<const int *const volatile>::VALUE,          // 2
    C08 = 1 + bslmf::IsPointer<volatile int*>::VALUE,                      // 2
    C09 = 1 + bslmf::IsPointer<volatile int *const>::VALUE,                // 2
    C10 = 1 + bslmf::IsPointer<volatile int *volatile>::VALUE,             // 2
    C11 = 1 + bslmf::IsPointer<volatile int *const volatile>::VALUE,       // 2
    C12 = 1 + bslmf::IsPointer<const volatile int*>::VALUE,                // 2
    C13 = 1 + bslmf::IsPointer<const volatile int *const>::VALUE,          // 2
    C14 = 1 + bslmf::IsPointer<const volatile int *volatile>::VALUE,       // 2
    C15 = 1 + bslmf::IsPointer<const volatile int *const volatile>::VALUE, // 2
    C16 = 1 + bslmf::IsPointer<TestType *>::VALUE,                         // 2

    D00 = 1 + bslmf::IsPointer<TestType *const>::VALUE,                    // 2
    D01 = 1 + bslmf::IsPointer<TestType *volatile>::VALUE,                 // 2
    D02 = 1 + bslmf::IsPointer<TestType *const volatile>::VALUE,           // 2
    D03 = 1 + bslmf::IsPointer<int>::VALUE,                                // 1
    D04 = 1 + bslmf::IsPointer<int const>::VALUE,                          // 1
    D05 = 1 + bslmf::IsPointer<int volatile>::VALUE,                       // 1
    D06 = 1 + bslmf::IsPointer<int const volatile>::VALUE,                 // 1
    D07 = 1 + bslmf::IsPointer<TestType>::VALUE,                           // 1
    D08 = 1 + bslmf::IsPointer<TestType const>::VALUE,                     // 1
    D09 = 1 + bslmf::IsPointer<TestType volatile>::VALUE,                  // 1
    D10 = 1 + bslmf::IsPointer<TestType const volatile>::VALUE             // 1
};

// from component doc

struct MyType {};
typedef MyType* PMT;

static const int a00 = bslmf::IsPointer<int *                >::VALUE;// a00==1
static const int a01 = bslmf::IsPointer<int *const           >::VALUE;// a01==1
static const int a02 = bslmf::IsPointer<int *volatile        >::VALUE;// a02==1
static const int a03 = bslmf::IsPointer<int *const volatile  >::VALUE;// a03==1
static const int a04 = bslmf::IsPointer<int                  >::VALUE;// a04==0
static const int a05 = bslmf::IsPointer<MyType               >::VALUE;// a05==0
static const int a06 = bslmf::IsPointer<MyType*              >::VALUE;// a06==1
static const int a07 = bslmf::IsPointer<MyType*const         >::VALUE;// a07==1
static const int a08 = bslmf::IsPointer<MyType*volatile      >::VALUE;// a08==1
static const int a09 = bslmf::IsPointer<MyType*const volatile>::VALUE;// a09==1
static const int a10 = bslmf::IsPointer<PMT                  >::VALUE;// a10==1

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    // int veryVerbose = argc > 3;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:  // Zero is always the leading case.
      case 1: {
        // --------------------------------------------------------------------
        // Test Plan:
        //   Instantiate 'bslmf::IsPointer' with various types and verify
        //   that their 'VALUE' member is initialized properly.
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "bslmf::IsPointer" << endl
                          << "================" << endl;

        ASSERT(2 == C00);
        ASSERT(2 == C01);
        ASSERT(2 == C02);
        ASSERT(2 == C03);
        ASSERT(2 == C04);
        ASSERT(2 == C05);
        ASSERT(2 == C06);
        ASSERT(2 == C07);
        ASSERT(2 == C08);
        ASSERT(2 == C09);
        ASSERT(2 == C10);
        ASSERT(2 == C11);
        ASSERT(2 == C12);
        ASSERT(2 == C13);
        ASSERT(2 == C14);
        ASSERT(2 == C15);
        ASSERT(2 == C16);

        ASSERT(2 == D00);
        ASSERT(2 == D01);
        ASSERT(2 == D02);
        ASSERT(1 == D03);
        ASSERT(1 == D04);
        ASSERT(1 == D05);
        ASSERT(1 == D06);
        ASSERT(1 == D07);
        ASSERT(1 == D08);
        ASSERT(1 == D09);
        ASSERT(1 == D10);

        ASSERT(1 == a00);
        ASSERT(1 == a01);
        ASSERT(1 == a02);
        ASSERT(1 == a03);
        ASSERT(0 == a04);
        ASSERT(0 == a05);
        ASSERT(1 == a06);
        ASSERT(1 == a07);
        ASSERT(1 == a08);
        ASSERT(1 == a09);
        ASSERT(1 == a10);
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
//      Copyright (C) Bloomberg L.P., 2002
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
