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

#include <bslma_defaultallocatorguard.h>
#include <bslma_usesbslmaallocator.h>
#include <bslma_testallocator.h>

#include <bslmf_assert.h>

#include <bsls_types.h>

#include <bsl_cstdlib.h>
#include <bsl_ios.h>
#include <bsl_iostream.h>
#include <bsl_sstream.h>

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
int veryVeryVerbose;

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
    veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:  // Zero is always the leading case.
      case 5: {
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
      case 4: {
        // --------------------------------------------------------------------
        // PRINT AND OUTPUT OPERATOR
        //   Ensure that the value of the object can be formatted appropriately
        //   on an 'ostream' in some standard, human-readable form.
        //
        // Concerns:
        //: 1 The 'print' method writes the value to the specified 'ostream'.
        //:
        //: 2 The 'print' method writes the value in the intended format.
        //:
        //: 3 The output using 's << obj' is the same as 'obj.print(s, 0, -1)',
        //:   but with each "attributeName = " elided.
        //:
        //: 4 The 'print' method signature and return type are standard.
        //:
        //: 5 The 'print' method returns the supplied 'ostream'.
        //:
        //: 6 The optional 'level' and 'spacesPerLevel' parameters have the
        //:   correct default values.
        //:
        //: 7 The output 'operator<<' signature and return type are standard.
        //:
        //: 8 The output 'operator<<' returns the supplied 'ostream'.
        //
        // Plan:
        //: 1 Use the addresses of the 'print' member function and 'operator<<'
        //:   free function defined in this component to initialize,
        //:   respectively, member-function and free-function pointers having
        //:   the appropriate signatures and return types.  (C-4, 7)
        //:
        //: 2 Using the table-driven technique:  (C-1..3, 5..6, 8)
        //:
        //:   1 Define sixteen carefully selected combinations of (two) object
        //:     values ('A' and 'B'), having distinct values for each
        //:     corresponding salient attribute, and various values for the
        //:     two formatting parameters, along with the expected output.
        //:
        //:     ( 'value' x  'level'   x 'spacesPerLevel' ):
        //:     1 { A   } x {  0     } x {  0, 1, -1, -8 } --> 3 expected o/ps
        //:     2 { A   } x {  3, -3 } x {  0, 2, -2, -8 } --> 8 expected o/ps
        //:     3 { B   } x {  2     } x {  3            } --> 1 expected o/p
        //:     4 { A B } x { -8     } x { -8            } --> 2 expected o/ps
        //:     5 { A B } x { -9     } x { -9            } --> 2 expected o/ps
        //:
        //:   2 For each row in the table defined in P-2.1:  (C-1..3, 5..6, 8)
        //:
        //:     1 Using a 'const' 'Obj', supply each object value and pair of
        //:       formatting parameters to 'print', omitting the 'level' or
        //:       'spacesPerLevel' parameter if the value of that argument is
        //:       '-8'.  If the parameters are, arbitrarily, (-9, -9), then
        //:       invoke the 'operator<<' instead.
        //:
        //:     2 Use a standard 'ostringstream' to capture the actual output.
        //:
        //:     3 Verify the address of what is returned is that of the
        //:       supplied stream.  (C-5, 8)
        //:
        //:     4 Compare the contents captured in P-2.2.2 with what is
        //:       expected.  (C-1..3, 6)
        //
        // Testing:
        //   ostream& print(ostream& s, int level = 0, int sPL = 4) const;
        //   ostream& operator<<(ostream& s, const ThreadAttributes& obj);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nPRINT AND OUTPUT OPERATOR\n"
                          <<   "=========================\n";

        if (verbose) cout << "\nAssign the addresses of 'print' and "
                             "the output 'operator<<' to variables.\n";
        {
            using namespace bslmt;
            using bsl::ostream;

            typedef ostream& (Obj::*funcPtr)(ostream&, int, int) const;
            typedef ostream& (*operatorPtr)(ostream&, const Obj&);

            // Verify that the signatures and return types are standard.

            funcPtr     printMember = &Obj::print;
            operatorPtr operatorOp  = operator<<;

            (void)printMember;  // quash potential compiler warnings
            (void)operatorOp;
        }

        if (verbose) cout <<
             "\nCreate a table of distinct value/format combinations.\n";

        static const struct {
            int                    d_line;  // source line number
            int                    d_level;
            int                    d_spacesPerLevel;

            Obj::DetachedState     d_detachedState;
            int                    d_guardSize;
            bool                   d_inheritSchedule;
            Obj::SchedulingPolicy  d_schedulingPolicy;
            int                    d_schedulingPriority;
            int                    d_stackSize;
            const char            *d_threadName;

            const char            *d_expected_p;
        } DATA[] = {

        // ------------------------------------------------------------------
        // P-2.1.1: { A } x { 0 }     x { 0, 1, -1, -8 } -->  4 expected o/ps
        // ------------------------------------------------------------------

        { L_,  0,  0,
          Obj::e_CREATE_JOINABLE, 64, true, Obj::e_SCHED_RR, 1, 1024, "t",
         "[\n"
         "detachedState = 0\n"
         "guardSize = 64\n"
         "inheritSchedule = true\n"
         "schedulingPolicy = 2\n"
         "schedulingPriority = 1\n"
         "stackSize = 1024\n"
         "threadName = \"t\"\n"
         "]\n" },

        { L_,  0,  1,
          Obj::e_CREATE_JOINABLE, 64, true, Obj::e_SCHED_RR, 1, 1024, "t",
         "[\n"
         " detachedState = 0\n"
         " guardSize = 64\n"
         " inheritSchedule = true\n"
         " schedulingPolicy = 2\n"
         " schedulingPriority = 1\n"
         " stackSize = 1024\n"
         " threadName = \"t\"\n"
         "]\n" },

        { L_,  0, -1,
          Obj::e_CREATE_JOINABLE, 64, true, Obj::e_SCHED_RR, 1, 1024, "t",
         "[ "
         "detachedState = 0 "
         "guardSize = 64 "
         "inheritSchedule = true "
         "schedulingPolicy = 2 "
         "schedulingPriority = 1 "
         "stackSize = 1024 "
         "threadName = \"t\" "
         "]" },

        { L_,  0, -8,
          Obj::e_CREATE_JOINABLE, 64, true, Obj::e_SCHED_RR, 1, 1024, "t",
         "[\n"
         "    detachedState = 0\n"
         "    guardSize = 64\n"
         "    inheritSchedule = true\n"
         "    schedulingPolicy = 2\n"
         "    schedulingPriority = 1\n"
         "    stackSize = 1024\n"
         "    threadName = \"t\"\n"
         "]\n" },

        // ------------------------------------------------------------------
        // P-2.1.2: { A } x { 3, -3 } x { 0, 2, -2, -8 } -->  8 expected o/ps
        // ------------------------------------------------------------------

        { L_,  3,  0,
          Obj::e_CREATE_JOINABLE, 64, true, Obj::e_SCHED_RR, 1, 1024, "t",
         "[\n"
         "detachedState = 0\n"
         "guardSize = 64\n"
         "inheritSchedule = true\n"
         "schedulingPolicy = 2\n"
         "schedulingPriority = 1\n"
         "stackSize = 1024\n"
         "threadName = \"t\"\n"
         "]\n" },

        { L_,  3,  2,
          Obj::e_CREATE_JOINABLE, 64, true, Obj::e_SCHED_RR, 1, 1024, "t",
         "      [\n"
         "        detachedState = 0\n"
         "        guardSize = 64\n"
         "        inheritSchedule = true\n"
         "        schedulingPolicy = 2\n"
         "        schedulingPriority = 1\n"
         "        stackSize = 1024\n"
         "        threadName = \"t\"\n"
         "      ]\n" },

        { L_,  3, -2,
          Obj::e_CREATE_JOINABLE, 64, true, Obj::e_SCHED_RR, 1, 1024, "t",
         "      [ "
         "detachedState = 0 "
         "guardSize = 64 "
         "inheritSchedule = true "
         "schedulingPolicy = 2 "
         "schedulingPriority = 1 "
         "stackSize = 1024 "
         "threadName = \"t\" "
         "]" },

        { L_,  3, -8,
          Obj::e_CREATE_JOINABLE, 64, true, Obj::e_SCHED_RR, 1, 1024, "t",
         "            [\n"
         "                detachedState = 0\n"
         "                guardSize = 64\n"
         "                inheritSchedule = true\n"
         "                schedulingPolicy = 2\n"
         "                schedulingPriority = 1\n"
         "                stackSize = 1024\n"
         "                threadName = \"t\"\n"
         "            ]\n" },

        { L_, -3,  0,
          Obj::e_CREATE_JOINABLE, 64, true, Obj::e_SCHED_RR, 1, 1024, "t",
         "[\n"
         "detachedState = 0\n"
         "guardSize = 64\n"
         "inheritSchedule = true\n"
         "schedulingPolicy = 2\n"
         "schedulingPriority = 1\n"
         "stackSize = 1024\n"
         "threadName = \"t\"\n"
         "]\n" },

        { L_, -3,  2,
          Obj::e_CREATE_JOINABLE, 64, true, Obj::e_SCHED_RR, 1, 1024, "t",
         "[\n"
         "        detachedState = 0\n"
         "        guardSize = 64\n"
         "        inheritSchedule = true\n"
         "        schedulingPolicy = 2\n"
         "        schedulingPriority = 1\n"
         "        stackSize = 1024\n"
         "        threadName = \"t\"\n"
         "      ]\n" },

        { L_, -3, -2,
          Obj::e_CREATE_JOINABLE, 64, true, Obj::e_SCHED_RR, 1, 1024, "t",
         "[ "
         "detachedState = 0 "
         "guardSize = 64 "
         "inheritSchedule = true "
         "schedulingPolicy = 2 "
         "schedulingPriority = 1 "
         "stackSize = 1024 "
         "threadName = \"t\" "
         "]" },

        { L_, -3, -8,
          Obj::e_CREATE_JOINABLE, 64, true, Obj::e_SCHED_RR, 1, 1024, "t",
         "[\n"
         "                detachedState = 0\n"
         "                guardSize = 64\n"
         "                inheritSchedule = true\n"
         "                schedulingPolicy = 2\n"
         "                schedulingPriority = 1\n"
         "                stackSize = 1024\n"
         "                threadName = \"t\"\n"
         "            ]\n" },

        // -----------------------------------------------------------------
        // P-2.1.3: { B } x { 2 }     x { 3 }            -->  1 expected o/p
        // -----------------------------------------------------------------

        { L_,  2,  3,
          Obj::e_CREATE_DETACHED, 16, false, Obj::e_SCHED_FIFO, 3, 512, "nm",
         "      [\n"
         "         detachedState = 1\n"
         "         guardSize = 16\n"
         "         inheritSchedule = false\n"
         "         schedulingPolicy = 1\n"
         "         schedulingPriority = 3\n"
         "         stackSize = 512\n"
         "         threadName = \"nm\"\n"
         "      ]\n" },

        // -----------------------------------------------------------------
        // P-2.1.4: { A B } x { -8 }   x { -8 }         -->  2 expected o/ps
        // -----------------------------------------------------------------

        { L_, -8, -8,
          Obj::e_CREATE_JOINABLE, 64, true, Obj::e_SCHED_RR, 1, 1024, "t",
         "[\n"
         "    detachedState = 0\n"
         "    guardSize = 64\n"
         "    inheritSchedule = true\n"
         "    schedulingPolicy = 2\n"
         "    schedulingPriority = 1\n"
         "    stackSize = 1024\n"
         "    threadName = \"t\"\n"
         "]\n" },

        { L_, -8, -8,
          Obj::e_CREATE_DETACHED, 16, false, Obj::e_SCHED_FIFO, 3, 512, "nm",
         "[\n"
         "    detachedState = 1\n"
         "    guardSize = 16\n"
         "    inheritSchedule = false\n"
         "    schedulingPolicy = 1\n"
         "    schedulingPriority = 3\n"
         "    stackSize = 512\n"
         "    threadName = \"nm\"\n"
         "]\n" },

        // -----------------------------------------------------------------
        // P-2.1.5: { A B } x { -9 }   x { -9 }         -->  2 expected o/ps
        // -----------------------------------------------------------------

        { L_, -9, -9,
          Obj::e_CREATE_JOINABLE, 64, true, Obj::e_SCHED_RR, 1, 1024, "t",
         "[ "
         "detachedState = 0 "
         "guardSize = 64 "
         "inheritSchedule = true "
         "schedulingPolicy = 2 "
         "schedulingPriority = 1 "
         "stackSize = 1024 "
         "threadName = \"t\" "
         "]" },

        { L_, -9, -9,
          Obj::e_CREATE_DETACHED, 16, false, Obj::e_SCHED_FIFO, 3, 512, "nm",
         "[ "
         "detachedState = 1 "
         "guardSize = 16 "
         "inheritSchedule = false "
         "schedulingPolicy = 1 "
         "schedulingPriority = 3 "
         "stackSize = 512 "
         "threadName = \"nm\" "
         "]" }

        };
        enum { NUM_DATA = sizeof DATA / sizeof *DATA };

        if (verbose) cout << "\nTesting with various print specifications.\n";
        {
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int LINE = DATA[ti].d_line;
                const int L    = DATA[ti].d_level;
                const int SPL  = DATA[ti].d_spacesPerLevel;

                const Obj::DetachedState    DETACHED_STATE =
                                                      DATA[ti].d_detachedState;
                const int                   GUARD_SIZE =  DATA[ti].d_guardSize;
                const bool                  INHERIT_SCHEDULE =
                                                    DATA[ti].d_inheritSchedule;
                const Obj::SchedulingPolicy SCHEDULING_POLICY =
                                                   DATA[ti].d_schedulingPolicy;
                const int                   SCHEDULING_PRIORITY =
                                                 DATA[ti].d_schedulingPriority;
                const int                   STACK_SIZE =  DATA[ti].d_stackSize;
                const char *const           THREAD_NAME =
                                                         DATA[ti].d_threadName;

                const char *const EXP = DATA[ti].d_expected_p;

                if (veryVerbose) {
                    T_ P_(L) P_(SPL) P_(DETACHED_STATE) P_(GUARD_SIZE)
                    P_(INHERIT_SCHEDULE) P_(SCHEDULING_POLICY)
                    P_(SCHEDULING_PRIORITY) P_(STACK_SIZE) P(THREAD_NAME)
                }

                if (veryVeryVerbose) { T_ T_ Q(EXPECTED) cout << EXP; }

                const Obj X = Obj()
                    .setDetachedState(DETACHED_STATE)
                    .setGuardSize(GUARD_SIZE)
                    .setInheritSchedule(INHERIT_SCHEDULE)
                    .setSchedulingPolicy(SCHEDULING_POLICY)
                    .setSchedulingPriority(SCHEDULING_PRIORITY)
                    .setStackSize(STACK_SIZE)
                    .setThreadName(THREAD_NAME)
                ;

                bsl::ostringstream os;

                // Verify supplied stream is returned by reference.

                if (-9 == L && -9 == SPL) {
                    ASSERTV(LINE, &os == &(os << X));

                    if (veryVeryVerbose) { T_ T_ Q(operator<<) }
                }
                else {
                    ASSERTV(LINE, -8 == SPL || -8 != L);

                    if (-8 != SPL) {
                        ASSERTV(LINE, &os == &X.print(os, L, SPL));
                    }
                    else if (-8 != L) {
                        ASSERTV(LINE, &os == &X.print(os, L));
                    }
                    else {
                        ASSERTV(LINE, &os == &X.print(os));
                    }

                    if (veryVeryVerbose) { T_ T_ Q(print) }
                }

                // Verify output is formatted as expected.

                if (veryVeryVerbose) { P(os.str()) }

                ASSERTV(LINE, EXP, os.str(), EXP == os.str());
            }
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING TYPE TRAITS
        //
        // Concern:
        //: 1 That the class has the 'UsesBslmaAllocator' type trait.
        //
        // Plan:
        //: 1 Evaluate the type trait.  (C-1)
        //
        // Testing
        //   bslma::UsesBslmaAllocator
        // --------------------------------------------------------------------

        if (verbose) cout << "TESTING TYPE TRAITS\n"
                             "===================\n";

        BSLMF_ASSERT(bslma::UsesBslmaAllocator<Obj>::value);
        BSLMF_ASSERT(bslma::UsesBslmaAllocator<Obj>::VALUE);
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // Testing Primary Manipulators / Accessors
        //
        // For each of the 6 attributes of Attribute, set the attribute on a
        // newly constructed object, copy the object, and use the accessor for
        // that attribute to verify the value.  Also verify that each (fluent)
        //  manipulator returns a non-'const' reference to the targeted object.
        // --------------------------------------------------------------------

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
            {
                Obj &rv = mX.setDetachedState(PARAM[i].d_detachedState);
                ASSERTV(&rv, &mX, &rv == &mX);
            }
            {
                Obj &rv = mX.setSchedulingPolicy(PARAM[i].d_schedulingPolicy);
                ASSERTV(&rv, &mX, &rv == &mX);
            }
            {
                Obj &rv =
                       mX.setSchedulingPriority(PARAM[i].d_schedulingPriority);
                ASSERTV(&rv, &mX, &rv == &mX);
            }
            {
                Obj &rv = mX.setInheritSchedule(PARAM[i].d_inheritSchedule);
                ASSERTV(&rv, &mX, &rv == &mX);
            }
            {
                Obj &rv = mX.setStackSize(PARAM[i].d_stackSize);
                ASSERTV(&rv, &mX, &rv == &mX);
            }
            {
                Obj &rv = mX.setGuardSize(PARAM[i].d_guardSize);
                ASSERTV(&rv, &mX, &rv == &mX);
            }
            {
                Obj &rv = mX.setThreadName(PARAM[i].d_threadName);
                ASSERTV(&rv, &mX, &rv == &mX);
            }

            if (veryVerbose) {
                X.print(cout);
            }

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
        // --------------------------------------------------------------------
        // Breathing test
        //
        // Create and destroy and object.  Verify default values.
        // --------------------------------------------------------------------
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
// Copyright 2020 Bloomberg Finance L.P.
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
