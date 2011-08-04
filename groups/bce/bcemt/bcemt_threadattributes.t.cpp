// bcemt_threadattributes.t.cpp                                       -*-C++-*-
#include <bcemt_threadattributes.h>

#include <bces_platform.h>

#include <bsl_cstdlib.h>
#include <bsl_iostream.h>

#ifdef BCES_PLATFORM__POSIX_THREADS
#include <pthread.h>
#endif

using namespace BloombergLP;
using namespace bsl;

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
        ASSERT(Obj::BCEMT_SCHED_DEFAULT == X.schedulingPolicy());
        ASSERT(X.inheritSchedule());
        ASSERT(0 != X.stackSize());

#if 0
        // 'Imp has been eliminated

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
#endif
      } break;
      case -1: {
        // --------------------------------------------------------------------
        // DEFAULT VALUES FROM PTHREADS
        //   Sg: Solaris gcc
        //   SC: Solaris CC
        //   AI: AIX
        //   HP: HPUX
        //   Li: Linux
        //
        //   Sg 32: Other (-20, 0, 20), Fifo (0, 0, 59), RR (0, 0, 59)
        //   Sg 64: Other (-20, 0, 20), Fifo (0, 0, 59), RR (0, 0, 59)
        //
        //   Sc 32: Other (-20, 0, 20), Fifo (0, 0, 59), RR (0, 0, 59)
        //   Sc 64: Other (-20, 0, 20), Fifo (0, 0, 59), RR (0, 0, 59)
        //
        //   AI 32: Other (1, 1, 127), Fifo (1, 1, 127), RR (1, 1, 127)
        //   AI 64: Other (1, 1, 127), Fifo (1, 1, 127), RR (1, 1, 127)
        //
        //   HP 32: Other (-256, -192, -129), Fifo (0, 0, 31), RR (0, 0, 31)
        //   HP 64: Other (-256, -192, -129), Fifo (0, 0, 31), RR (0, 0, 31)
        //
        //   Li 32: Other (0, 0, 0), Fifo (1, 0, 99), RR (1, 0, 99)
        //   Li 64: Other (0, 0, 0), Fifo (1, 0, 99), RR (1, 0, 99)
        // --------------------------------------------------------------------

#ifdef BCES_PLATFORM__POSIX_THREADS
        int rc = 0;

        pthread_attr_t attr;
        rc |= pthread_attr_init(&attr);

        int policy;
        rc |= pthread_attr_getschedpolicy(&attr, &policy);
        ASSERT(SCHED_OTHER == policy);

        sched_param param;
        rc |= pthread_attr_getschedparam(&attr, &param);
        int priority = param.sched_priority;

#define MAXPRI(policy)  sched_get_priority_max(policy)
#define MINPRI(policy)  sched_get_priority_min(policy)

        cout << "Other (" << MINPRI(policy) << ", " <<
                             priority       << ", " <<
                             MAXPRI(policy) << "), ";

        policy = SCHED_FIFO;
        rc |= pthread_attr_setschedpolicy(&attr, policy);
        rc |= pthread_attr_getschedparam(&attr, &param);
        priority = param.sched_priority;

        cout << "Fifo (" << MINPRI(policy) << ", " <<
                            priority       << ", " <<
                            MAXPRI(policy) << "), ";

        policy = SCHED_RR;
        rc |= pthread_attr_setschedpolicy(&attr, policy);
        rc |= pthread_attr_getschedparam(&attr, &param);
        priority = param.sched_priority;

        cout << "RR (" << MINPRI(policy) << ", " <<
                          priority       << ", " <<
                          MAXPRI(policy) << ")\n";

        ASSERT(0 == rc);
#undef MAXPRI
#undef MINPRI
#endif
      } break;
      case -2: {
        // --------------------------------------------------------------------
        // DEFAULT VALUES FROM THREADATTRIBUTES
        //
        // Results:
        //   Sg: Solaris gcc
        //   SC: Solaris CC
        //   AI: AIX
        //   HP: HPUX
        //   Li: Linux
        //
        //   Sg 32: Other (-20, 0, 20), Fifo (0, 0, 59), Rr (0, 0, 59)
        //   Sg 64: Other (-20, 0, 20), Fifo (0, 0, 59), Rr (0, 0, 59)
        //
        //   Sc 32: Other (-20, 0, 20), Fifo (0, 0, 59), Rr (0, 0, 59)
        //   Sc 64: Other (-20, 0, 20), Fifo (0, 0, 59), Rr (0, 0, 59)
        //
        //   AI 32: Other (1, 1, 60), Fifo (1, 1, 60), Rr (1, 1, 60)
        //   AI 64: Other (1, 1, 60), Fifo (1, 1, 60), Rr (1, 1, 60)
        //
        //   HP 32: Other (-256, -256, -129), Fifo (0, 0, 31), Rr (0, 0, 31)
        //   HP 64: Other (-256, -256, -129), Fifo (0, 0, 31), Rr (0, 0, 31)
        //
        //   Li 32: Other (0, 0, 0), Fifo (1, 1, 99), Rr (1, 1, 99)
        //   Li 64: Other (0, 0, 0), Fifo (1, 1, 99), Rr (1, 1, 99)
        // --------------------------------------------------------------------
        
        bcemt_ThreadAttributes attr;

#define MAXPRI(policy)  (bcemt_ThreadAttributes::getMaxSchedPriority(policy))
#define MINPRI(policy)  (bcemt_ThreadAttributes::getMinSchedPriority(policy))

        typedef bcemt_ThreadAttributes::SchedulingPolicy Policy;

        const Policy OTHER = bcemt_ThreadAttributes::BCEMT_SCHED_OTHER;
        const Policy FIFO  = bcemt_ThreadAttributes::BCEMT_SCHED_FIFO;
        const Policy RR    = bcemt_ThreadAttributes::BCEMT_SCHED_RR;

        attr.setSchedulingPolicy(OTHER);

        cout << "Other ("   << MINPRI(OTHER) <<
                ", "        << attr.schedulingPriority() <<
                ", "        << MAXPRI(OTHER);

        attr.setSchedulingPolicy(FIFO);

        cout << "), Fifo (" << MINPRI(FIFO) <<
                ", "        << attr.schedulingPriority() <<
                ", "        << MAXPRI(FIFO);

        attr.setSchedulingPolicy(RR);

        cout << "), Rr ("   << MINPRI(RR) <<
                ", "        << attr.schedulingPriority() <<
                ", "        << MAXPRI(RR) <<

                ")\n";
#undef MAXPRI
#undef MINPRI
      }  break;
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
