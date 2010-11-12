// bcemt_threadattributes.t.cpp                                       -*-C++-*-
#include <bcemt_threadattributes.h>

#include <bsl_cstdlib.h>
#include <bsl_iostream.h>

using namespace BloombergLP;
using namespace bsl;

//-----------------------------------------------------------------------------
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
#define LOOP_ASSERT(I,X) {                                                    \
    if (!(X)) { cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP2_ASSERT(I,J,X) {                                                 \
    if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\n";   \
                aSsErT(1, #X, __LINE__); } }
#define LOOP3_ASSERT(I,J,K,X) {                                               \
    if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J <<         \
                        "\t" << #K << ": " << K << "\n";                      \
                aSsErT(1, #X, __LINE__); } }
//-----------------------------------------------------------------------------
#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", " << flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number
#define T_() cout << '\t' << flush;           // Print tab w/o linefeed.

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

int verbose;
int veryVerbose;

typedef bcemt_ThreadAttributes Obj;

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    verbose = argc > 2;
    veryVerbose = argc > 3;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:  // Zero is always the leading case.
      case 3: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE TEST
        //
        // Concerns:
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

        if (verbose) cout << endl << "USAGE EXAMPLE TEST" << endl
                                  << "==================" << endl;

///Usage
///-----
// The following snippets of code illustrate basic use of this component.
// First we create a default-constructed 'bcemt_ThreadAttributes' object and
// assert that its detached state does indeed have the default value (i.e.,
// 'bcemt_ThreadAttributes::BCEMT_CREATE_JOINABLE'):
//..
    bcemt_ThreadAttributes attributes;
    ASSERT(bcemt_ThreadAttributes::BCEMT_CREATE_JOINABLE ==
                                                   attributes.detachedState());
//..
// Next we modify the detached state of 'attributes' to have the non-default
// value 'bcemt_ThreadAttributes::BCEMT_CREATE_DETACHED':
//..
    attributes.setDetachedState(bcemt_ThreadAttributes::BCEMT_CREATE_DETACHED);
    ASSERT(bcemt_ThreadAttributes::BCEMT_CREATE_DETACHED ==
                                                   attributes.detachedState());
//..
// Finally, we make a copy of 'attributes':
//..
    bcemt_ThreadAttributes copy(attributes);
    ASSERT(bcemt_ThreadAttributes::BCEMT_CREATE_DETACHED ==
                                                         copy.detachedState());
    ASSERT(attributes == copy);
//..

      } break;
      case 2: {
        // ------------------------------------------------------------------
        // Testing Primary Manipulators / Accessors
        //
        // For each of the 6 attributes of Attribute, set the attribute
        // on a newly constructed object, copy the object, and use the accessor
        // for that attribute to verify the value.
        // ------------------------------------------------------------------
        if (verbose) {
            cout << "PRIMARY METHOD TEST" << endl;
            cout << "==============" << endl;
        }

        struct Parameters {
            int                   d_line;

            Obj::DetachedState    d_detachedState;
            Obj::SchedulingPolicy d_schedulingPolicy;
            int                   d_schedulingPriority;
            bool                  d_inheritSchedule;
            int                   d_stackSize;
            int                   d_guardSize;

            int                   d_whichVerify;
        } PARAM[] = {
           {L_, Obj::BCEMT_CREATE_DETACHED, Obj::BCEMT_SCHED_OTHER,
                0, 0, 0, 0, 1 },
           {L_, Obj::BCEMT_CREATE_DETACHED, Obj::BCEMT_SCHED_FIFO,
                0, 0, 0, 0, 2 },
           {L_, Obj::BCEMT_CREATE_DETACHED, Obj::BCEMT_SCHED_FIFO,
                5, 0, 0, 0, 3 },
           {L_, Obj::BCEMT_CREATE_DETACHED, Obj::BCEMT_SCHED_FIFO,
                4, true, 0, 0, 4 },
#ifdef BSLS_PLATFORM__CPU_64_BIT
           {L_, Obj::BCEMT_CREATE_DETACHED, Obj::BCEMT_SCHED_FIFO,
                3, 0, 300000, 0, 5 },
#else
           {L_, Obj::BCEMT_CREATE_DETACHED, Obj::BCEMT_SCHED_FIFO,
                3, 0, 80000, 0, 5 },
#endif
           {L_, Obj::BCEMT_CREATE_DETACHED, Obj::BCEMT_SCHED_FIFO,
                2, 0, 0, 2000, 6 }
        };

        int numParams = sizeof(PARAM) / sizeof(Parameters);
        for (int i = 0; i < numParams; ++i) {
            Obj mX;
            mX.setDetachedState(PARAM[i].d_detachedState);
            mX.setSchedulingPolicy(PARAM[i].d_schedulingPolicy);
            mX.setSchedulingPriority(PARAM[i].d_schedulingPriority);
            mX.setInheritSchedule(PARAM[i].d_inheritSchedule);
            mX.setStackSize(PARAM[i].d_stackSize);
            mX.setGuardSize(PARAM[i].d_guardSize);

            const Obj& X = mX;

            Obj mY;
            LOOP_ASSERT(i, X != mY);
            mY = X;
            LOOP_ASSERT(i, X == mY);

            const Obj& Y = mY;
            switch (PARAM[i].d_whichVerify) {
            case 1:
                LOOP_ASSERT(PARAM[i].d_line,
                            PARAM[i].d_detachedState == Y.detachedState());
                break;
            case 2:
                LOOP_ASSERT(PARAM[i].d_line,
                            PARAM[i].d_schedulingPolicy ==
                            Y.schedulingPolicy());
                break;
            case 3:
                LOOP_ASSERT(PARAM[i].d_line,
                            PARAM[i].d_schedulingPriority ==
                            Y.schedulingPriority());
                break;
            case 4:
                LOOP_ASSERT(PARAM[i].d_line,
                            PARAM[i].d_inheritSchedule ==
                            Y.inheritSchedule());
                break;
            case 5:
                LOOP2_ASSERT(PARAM[i].d_line,
                             Y.stackSize(), PARAM[i].d_stackSize ==
                             Y.stackSize());
                break;
            case 6:
                LOOP_ASSERT(PARAM[i].d_line, PARAM[i].d_guardSize ==
                            Y.guardSize());
                break;
            }
        }
      } break;
      case 1: {
        // ------------------------------------------------------------------
        // Breathing test
        //
        // Create and destroy and object.  Verify default values.
        // ------------------------------------------------------------------
        if (verbose) {
            cout << "BREATHING TEST" << endl;
            cout << "==============" << endl;
        }

        Obj mX;
        {
            Obj mY;
        }
        const Obj& X = mX;

        ASSERT(Obj::BCEMT_CREATE_JOINABLE == X.detachedState());
        ASSERT(Obj::BCEMT_SCHED_OTHER == X.schedulingPolicy());
        ASSERT(X.inheritSchedule());
        ASSERT(0 != X.stackSize());

        typedef bcemt_ThreadAttributes::Imp Imp;

        ASSERT(bcemt_ThreadAttributes::BCEMT_CREATE_JOINABLE ==
                                                   Imp::BCEMT_CREATE_JOINABLE);
        ASSERT(bcemt_ThreadAttributes::BCEMT_CREATE_DETACHED ==
                                                   Imp::BCEMT_CREATE_DETACHED);

        ASSERT(bcemt_ThreadAttributes::BCEMT_SCHED_OTHER ==
                                                   Imp::BCEMT_SCHED_OTHER);
        ASSERT(bcemt_ThreadAttributes::BCEMT_SCHED_FIFO  ==
                                                   Imp::BCEMT_SCHED_FIFO);
        ASSERT(bcemt_ThreadAttributes::BCEMT_SCHED_RR    ==
                                                   Imp::BCEMT_SCHED_RR);

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
//      Copyright (C) Bloomberg L.P., 2009
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
