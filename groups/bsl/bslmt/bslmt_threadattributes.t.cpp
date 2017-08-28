// bslmt_threadattributes.t.cpp                                       -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <bslmt_threadattributes.h>

#include <bslmt_platform.h>

#include <bslim_testutil.h>

#include <bslma_testallocator.h>
#include <bslma_defaultallocatorguard.h>

#include <bsls_types.h>

#include <bsl_cstdlib.h>
#include <bsl_ios.h>
#include <bsl_iostream.h>

#ifdef BSLMT_PLATFORM_POSIX_THREADS
#include <pthread.h>
#endif

using namespace BloombergLP;
using namespace bsl;

// ============================================================================
//                     STANDARD BDE ASSERT TEST FUNCTION
// ----------------------------------------------------------------------------

namespace {

int testStatus = 0;

void aSsErT(bool condition, const char *message, int line)
{
    if (condition) {
        cout << "Error " __FILE__ "(" << line << "): " << message
             << "    (failed)" << endl;

        if (0 <= testStatus && testStatus <= 100) {
            ++testStatus;
        }
    }
}

}  // close unnamed namespace

// ============================================================================
//               STANDARD BDE TEST DRIVER MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT       BSLIM_TESTUTIL_ASSERT
#define ASSERTV      BSLIM_TESTUTIL_ASSERTV

#define LOOP_ASSERT  BSLIM_TESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLIM_TESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLIM_TESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLIM_TESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLIM_TESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLIM_TESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLIM_TESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLIM_TESTUTIL_LOOP6_ASSERT

#define Q            BSLIM_TESTUTIL_Q   // Quote identifier literally.
#define P            BSLIM_TESTUTIL_P   // Print identifier and value.
#define P_           BSLIM_TESTUTIL_P_  // P(X) without '\n'.
#define T_           BSLIM_TESTUTIL_T_  // Print a tab (w/o newline).
#define L_           BSLIM_TESTUTIL_L_  // current Line number

// ============================================================================
//                   GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

int verbose;
int veryVerbose;

typedef bsls::Types::Int64      Int64;
typedef bslmt::ThreadAttributes Obj;

// ============================================================================
//                               USAGE EXAMPLE
// ----------------------------------------------------------------------------

///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Creating and modifying thread attributes objects
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// In this example we will demonstrate creating and configuring a
// 'bslmt::ThreadAttributes' object, then using it with a hypothetical
// thread-creation function.  Finally we show how a thread creation function
// might interpret those attributes for the underlying operating system.
//
// First we forward declare a routine that we will use to create a thread:
//..
    void myThreadCreate(int                             *threadHandle,
                        const bslmt::ThreadAttributes&   attributes,
                        void                           (*function)());
        // Spawn a thread having properties described by the specified
        // 'attributes' and that runs the specified 'function', and assign a
        // handle referring to the spawned thread to the specified
        // '*threadHandle'.
//..
// Then, we declare two routines that will return the minimum and maximum
// thread priority given a scheduling policy.  Note that similar methods exist
// in 'bslmt_threadutil'.
//..
    int myMinPriority(bslmt::ThreadAttributes::SchedulingPolicy policy);
    int myMaxPriority(bslmt::ThreadAttributes::SchedulingPolicy policy);
//..
// Next we define a function that we will use as our thread entry point.  This
// function declares a single variable on the stack of predetermined size.
//..
    enum { k_BUFFER_SIZE = 128 * 1024 };

    void myThreadFunction()
    {
        int bufferLocal[k_BUFFER_SIZE];

        // Perform some calculation that involves no subroutine calls or
        // additional automatic variables.

        (void) bufferLocal;    // silence unused warnings
    }
//..
// Then, we define our main function, in which we demonstrate configuring a
// 'bslmt::ThreadAttributes' object describing the properties a thread we will
// create.
//..
    void testMain()
    {
//..
// Next, we create a thread attributes object, 'attributes', and set its
// 'stackSize' attribute to a value large enough to accommodate the
// 'BUFFER_SIZE' buffer used by 'myThreadFunction'.  Note that we use
// 'BUFFER_SIZE' as an illustration; in practice, it is difficult or impossible
// to gauge the exact amount of stack size required for a typical thread, and
// the value supplied should be a reasonable *upper* bound on the anticipated
// requirement.
//..
        bslmt::ThreadAttributes attributes;
        attributes.setStackSize(k_BUFFER_SIZE);
//..
// Then, we set the 'detachedState' property to 'e_CREATE_DETACHED', indicating
// that the thread will not be joinable, and its resources will be reclaimed
// upon termination.
//..
        attributes.setDetachedState(
                               bslmt::ThreadAttributes::e_CREATE_DETACHED);
//..
// Now, we create a thread, using the attributes configured above:
//..
        int handle;
        myThreadCreate(&handle, attributes, &myThreadFunction);
    }
// Finally, we define the thread creation function, and show how a thread
// attributes object might be interpreted by it. This creation function
// supplies its own default values for stack and thread guard sizes; a real
// routine using 'ThreadAttributes' should base its defaults on the
// 'bslmt_configuration' component.
//..
    enum {
       MY_DEFAULT_STACK_SIZE = 512 * 1024,
       MY_DEFAULT_GUARD_SIZE = 16384
    };

    void myThreadCreate(int                             *threadHandle,
                        const bslmt::ThreadAttributes&   attributes,
                        void                           (*function)())
        // Spawn a thread with properties described by the specified
        // 'attributes', running the specified 'function', and assign a handle
        // referring to the spawned thread to the specified '*threadHandle'.
    {
        int stackSize = attributes.stackSize();
        if (bslmt::ThreadAttributes::e_UNSET_STACK_SIZE == stackSize) {
            stackSize = MY_DEFAULT_STACK_SIZE;
        }

        // Add a "fudge factor" to 'stackSize' to ensure that the client can
        // declare an object of 'stackSize' bytes on the stack safely.

        stackSize += 8192;

    #ifdef BSLS_PLATFORM_OS_HPUX
        // The Itanium divides the stack into two sections: a variable stack
        // and a control stack.  To make 'stackSize' have the same meaning
        // across platforms, we must double it on this platform.

        stackSize *= 2;
    #endif

        int guardSize = attributes.guardSize();
        if (bslmt::ThreadAttributes::e_UNSET_GUARD_SIZE == guardSize) {
            guardSize = MY_DEFAULT_GUARD_SIZE;
        }

        int policy = attributes.schedulingPolicy();
        int priority = attributes.schedulingPriority();

        (void) policy;          // silence unused warnings
        (void) priority;        // silence unused warnings
        (void) threadHandle;    // silence unused warnings
        (void) function;        // silence unused warnings

        // the following is pseudo-code for actually creating the thread
#if 0
        if (bslmt::ThreadAttributes::e_UNSET_PRIORITY == priority) {
            priority = operatingSystemDefaultPriority(policy);
        }

        operatingSystemThreadCreate(threadHandle,
                                    stackSize,
                                    guardSize,
                                    attributes.inheritSchedule(),
                                    policy,
                                    priority,
                                    attributes.detachedState()
                                    function);
#endif
    }
//..
// Notice that a new value derived from the 'stackSize' attribute is used so
// that the meaning of the attribute is platform neutral.

// ============================================================================
//                               MAIN PROGRAM
// ----------------------------------------------------------------------------

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

        testMain();

///Usage
///-----
// The following snippets of code illustrate basic use of this component.
// First we create a default-constructed 'bslmt::ThreadAttributes' object and
// assert that its detached state does indeed have the default value (i.e.,
// 'bslmt::ThreadAttributes::e_CREATE_JOINABLE'):
//..
    bslmt::ThreadAttributes attributes;
    ASSERT(bslmt::ThreadAttributes::e_CREATE_JOINABLE ==
                                                   attributes.detachedState());
//..
// Next we modify the detached state of 'attributes' to have the non-default
// value 'bslmt::ThreadAttributes::e_CREATE_DETACHED':
//..
    attributes.setDetachedState(
                               bslmt::ThreadAttributes::e_CREATE_DETACHED);
    ASSERT(bslmt::ThreadAttributes::e_CREATE_DETACHED ==
                                                   attributes.detachedState());
//..
// Finally, we make a copy of 'attributes':
//..
    bslmt::ThreadAttributes copy(attributes);
    ASSERT(bslmt::ThreadAttributes::e_CREATE_DETACHED ==
                                                         copy.detachedState());
    ASSERT(attributes == copy);
//..

      } break;
      case 2: {
        // ------------------------------------------------------------------
        // Testing Primary Manipulators / Accessors
        //
        // For each of the 6 attributes of Attribute, set the attribute on a
        // newly constructed object, copy the object, and use the accessor for
        // that attribute to verify the value.
        // ------------------------------------------------------------------

        if (verbose) {
            cout << "PRIMARY METHOD TEST\n"
                    "===================\n";
        }

        bslma::TestAllocator ta;
        bslma::TestAllocator da;
        bslma::DefaultAllocatorGuard dag(&da);

        struct Parameters {
            int                    d_line;

            Obj::DetachedState     d_detachedState;
            Obj::SchedulingPolicy  d_schedulingPolicy;
            int                    d_schedulingPriority;
            bool                   d_inheritSchedule;
            int                    d_stackSize;
            int                    d_guardSize;
            const char            *d_threadName;
        } PARAM[] = {
           {L_, Obj::e_CREATE_DETACHED, Obj::e_SCHED_OTHER, 0, 0, 0, 0, "" },
           {L_, Obj::e_CREATE_DETACHED, Obj::e_SCHED_OTHER, 0, 0, 0, 0, "x" },
           {L_, Obj::e_CREATE_DETACHED, Obj::e_SCHED_OTHER, 0, 0, 0, 0,
                                                                "short name" },
           {L_, Obj::e_CREATE_DETACHED, Obj::e_SCHED_OTHER, 0, 0, 0, 0,
                               "How long is your thread name? I wanna know." },
           {L_, Obj::e_CREATE_DETACHED, Obj::e_SCHED_FIFO, 0, 0, 0, 0,
                                      "incredibly terribly long thread name" },
           {L_, Obj::e_CREATE_DETACHED, Obj::e_SCHED_FIFO, 5, 0, 0, 0,
                            "My thread name is sooooooooooooooooooooo long." },
           {L_, Obj::e_CREATE_DETACHED, Obj::e_SCHED_FIFO, 4, true, 0, 0,
                   "My thread name got lost and couldn't find its way home." },
           {L_, Obj::e_CREATE_DETACHED, Obj::e_SCHED_FIFO, 3, 0, 300000, 0,
                                "My thread name goes to the next time zone." },
           {L_, Obj::e_CREATE_DETACHED, Obj::e_SCHED_FIFO, 3, 0, 80000, 0,
                                                                "short name" },
           {L_, Obj::e_CREATE_DETACHED, Obj::e_SCHED_FIFO, 2, 0, 0, 2000,
                              "My thread name goes to Nova Scotia and back." },
           {L_, Obj::e_CREATE_DETACHED, Obj::e_SCHED_FIFO, 2, 0, 0, 2000,
                 "That's nothing!"
                         "  The other end of my thread name is in Timbuktu." }
        };

        size_t numParams = sizeof(PARAM) / sizeof(Parameters);
        for (unsigned i = 0; i < numParams; ++i) {
            const Int64 numTaPreAlloc = ta.numAllocations();
            const Int64 numDaPreAlloc = da.numAllocations();

            Obj mX(&ta);    const Obj& X = mX;
            mX.setDetachedState(PARAM[i].d_detachedState);
            mX.setSchedulingPolicy(PARAM[i].d_schedulingPolicy);
            mX.setSchedulingPriority(PARAM[i].d_schedulingPriority);
            mX.setInheritSchedule(PARAM[i].d_inheritSchedule);
            mX.setStackSize(PARAM[i].d_stackSize);
            mX.setGuardSize(PARAM[i].d_guardSize);
            mX.setThreadName(PARAM[i].d_threadName);

            ASSERT(da.numAllocations() == numDaPreAlloc);
            ASSERTV(X.threadName(), (X.threadName().length() > 15) ==
                                        (ta.numAllocations() > numTaPreAlloc));

            Obj mY(&ta);
            LOOP_ASSERT(i, X != mY);
            mY = X;
            LOOP_ASSERT(i, X == mY);

            const Obj Z(X, &ta);

            const Obj& Y = mY;

            ASSERT(da.numAllocations() == numDaPreAlloc);

            const Obj ZD(X);
            Obj mA;    const Obj& A = mA;

            ASSERT(A != X);

            mA = X;

            ASSERT(&ta == X.allocator());
            ASSERT(&ta == Y.allocator());
            ASSERT(&ta == Z.allocator());
            ASSERT(&da == ZD.allocator());
            ASSERT(&da == A.allocator());

            ASSERT(X  == Y);
            ASSERT(Z  == X);
            ASSERT(ZD == X)
            ASSERT(A  == X);

            LOOP_ASSERT(PARAM[i].d_line,
                        PARAM[i].d_detachedState == X.detachedState());
            LOOP_ASSERT(PARAM[i].d_line,
                        PARAM[i].d_detachedState == Y.detachedState());
            LOOP_ASSERT(PARAM[i].d_line,
                        PARAM[i].d_detachedState == Z.detachedState());
            LOOP_ASSERT(PARAM[i].d_line,
                        PARAM[i].d_schedulingPolicy ==
                        X.schedulingPolicy());
            LOOP_ASSERT(PARAM[i].d_line,
                        PARAM[i].d_schedulingPolicy ==
                        Y.schedulingPolicy());
            LOOP_ASSERT(PARAM[i].d_line,
                        PARAM[i].d_schedulingPolicy ==
                        Z.schedulingPolicy());
            LOOP_ASSERT(PARAM[i].d_line,
                        PARAM[i].d_schedulingPriority ==
                        X.schedulingPriority());
            LOOP_ASSERT(PARAM[i].d_line,
                        PARAM[i].d_schedulingPriority ==
                        Y.schedulingPriority());
            LOOP_ASSERT(PARAM[i].d_line,
                        PARAM[i].d_inheritSchedule ==
                        X.inheritSchedule());
            LOOP_ASSERT(PARAM[i].d_line,
                        PARAM[i].d_inheritSchedule ==
                        Y.inheritSchedule());
            LOOP_ASSERT(PARAM[i].d_line,
                        PARAM[i].d_inheritSchedule ==
                        Z.inheritSchedule());
            LOOP2_ASSERT(PARAM[i].d_line,
                         X.stackSize(), PARAM[i].d_stackSize ==
                         X.stackSize());
            LOOP2_ASSERT(PARAM[i].d_line,
                         Y.stackSize(), PARAM[i].d_stackSize ==
                         Y.stackSize());
            LOOP2_ASSERT(PARAM[i].d_line,
                         Z.stackSize(), PARAM[i].d_stackSize ==
                         Z.stackSize());
            LOOP_ASSERT(PARAM[i].d_line, PARAM[i].d_guardSize ==
                        X.guardSize());
            LOOP_ASSERT(PARAM[i].d_line, PARAM[i].d_guardSize ==
                        Y.guardSize());
            LOOP_ASSERT(PARAM[i].d_line, PARAM[i].d_guardSize ==
                        Z.guardSize());
            LOOP_ASSERT(PARAM[i].d_line,
                        PARAM[i].d_threadName == X.threadName());
            LOOP_ASSERT(PARAM[i].d_line,
                        PARAM[i].d_threadName == Y.threadName());
            LOOP_ASSERT(PARAM[i].d_line,
                        PARAM[i].d_threadName == Z.threadName());
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
#if __cplusplus >= 201103L
        {
            Obj b[2]{{}, {}};
            (void)b;
        }
#endif
        const Obj& X = mX;

        ASSERT(Obj::e_CREATE_JOINABLE == X.detachedState());
        ASSERT(Obj::e_SCHED_DEFAULT == X.schedulingPolicy());
        ASSERT(X.inheritSchedule());
        ASSERT(0 != X.stackSize());
        ASSERT("" == X.threadName());
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

#ifdef BSLMT_PLATFORM_POSIX_THREADS
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

// ----------------------------------------------------------------------------
// Copyright 2015 Bloomberg Finance L.P.
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
