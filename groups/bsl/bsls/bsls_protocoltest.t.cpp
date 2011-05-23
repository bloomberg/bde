// bsls_protocoltest.t.cpp                                            -*-C++-*-

#include <bsls_protocoltest.h>

#include <new>
#include <stdio.h>
#include <stdlib.h>

using namespace BloombergLP;

//=============================================================================
//                                 TEST PLAN
//-----------------------------------------------------------------------------
//
//                                  Overview
//                                  --------
// This component provides mechanism classes and macros for testing protocol
// classes.  The test driver essentially tests the behavior of those classes
// and macros.  Where it made sense the creators, accessors and manipulators
// are tested first.  After that the test driver tests complex interactions
// between the provided mechanism classes and various possible protocol
// class definitions (both correct and incorrect protocol definitions).  The
// goal of most tests is to verify that correct protocol definitions are
// accepted and incorrect protocol definitions are rejected.
// ----------------------------------------------------------------------------
//
// [ 1] BREATHING TEST
//
// class bsls_ProtocolTest:
// [ 2] bsls_ProtocolTest();
// [ 2] ~bsls_ProtocolTest();
// [ 3] int failures();
// [ 3] bool lastStatus();
// [ 4] bool testAbstract();
// [ 4] bool testNoDataMembers();
// [ 4] bool testVirtualDestructor();
// [ 7] operator->();
//
// private classes:
// [ 5] class bsls_ProtocolTest_IsAbstract;
// [ 5] class bsls_ProtocolTest_MethodReturnType;
// [ 5] class bsls_ProtocolTest_MethodReturnRefType;
//
// class bsls_ProtocolTestImp:
// [ 6] void setTestStatus(bsls_ProtocolTest_Status *);
// [ 6] exit();
// [ 6] exitRef();
// [ 6] exitVal();
//
// class bsls_ProtocolTest_Status:
// [ 5] bsls_ProtocolTest_Status();
// [ 5] resetLast();
// [ 5] fail();
// [ 5] last();
// [ 5] failures();
//
// [ 8] BSLS_PROTOCOLTEST_ASSERT
//
// [ 9] USAGE EXAMPLE
// ----------------------------------------------------------------------------

// ============================================================================
//                    STANDARD BDE ASSERT TEST MACROS
// ----------------------------------------------------------------------------

namespace {

int testStatus = 0;
bool disableAssertOutput = false;

void aSsErT(int c, const char *s, int i) {
    if (c) {
        if (!disableAssertOutput) {
            printf("Error %s (%d): %s (failed)\n", __FILE__, i, s);
        }

        ++testStatus;
    }
}

}

#define ASSERT(X) do { aSsErT(!(X), #X, __LINE__); } while (0,0)

// ============================================================================
//                    TYPES AND FUNCTIONS REQUIRED FOR TESTING
// ----------------------------------------------------------------------------

namespace {

struct MyInterface {
    virtual ~MyInterface() {}
    virtual const char *bar(char const *, char const *) = 0;
    virtual int foo(int) const = 0;
};

struct MyInterfaceTest : bsls_ProtocolTestImp<MyInterface> {
    const char *bar(char const *, char const *) { return exit(); }
    int foo(int) const                          { return exit(); }
};

// For testing 'testAbstract'.
struct NotAbstractInterface {
    void foo();
};

struct NotAbstractInterfaceTest : bsls_ProtocolTestImp<NotAbstractInterface> {
    void foo() { exit(); }
};

// For testing 'testNoDataMembers'.
struct WithDataFields {
    virtual ~WithDataFields() {}

  private:
    int a;
};

struct WithDataFieldsTest : bsls_ProtocolTestImp<WithDataFields> {
};

// For testing 'testVirtualDestructor'.
struct NoVirtualDestructor {
    virtual void foo() = 0;
};

struct NoVirtualDestructorTest : bsls_ProtocolTestImp<NoVirtualDestructor> {
    void foo() { exit(); }
};

// For testing return value convertions to a reference to a non-copyable type
struct NonCopyable {
  private:
    NonCopyable(NonCopyable const &);
    NonCopyable & operator=(NonCopyable const &);
};

// For testing protocol method testing aparatus.
struct MyInterfaceNonVirtual {
    int foo(int) { return 0; }
    virtual void bar() = 0;
};

struct MyInterfaceNonVirtualTest : bsls_ProtocolTestImp<MyInterfaceNonVirtual> {
    int foo(int) { return exit(); }
    void bar()   { exit(); }
};

struct MyInterfaceNonPublic {
  private:
    virtual int foo(int) = 0;
};

struct MyInterfaceNonPublicTest : bsls_ProtocolTestImp<MyInterfaceNonPublic> {
    // This override must be public.
    int foo(int) { return exit(); }
};

struct MyInterfaceNonCopyableReturn {
    virtual const NonCopyable& foo(int) = 0;
};

struct MyInterfaceNonCopyableReturnTest
    : bsls_ProtocolTestImp<MyInterfaceNonCopyableReturn> {
    const NonCopyable& foo(int) { return exitRef(); }
};

}  // close unnamed namespace

// ============================================================================
//                            MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int             test = argc > 1 ? atoi(argv[1]) : 0;
    bool         verbose = argc > 2;
    bool     veryVerbose = argc > 3;
    bool veryVeryVerbose = argc > 4;

    printf("TEST %s CASE %d\n", __FILE__, test);

    switch (test) { case 0:
      case 9: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //
        // Concerns:
        //:  1 The usage example provided in the component header file
        //:    compiles, links, and runs as shown.
        //
        // Plan:
        //:  1 Incorporate usage example from header into test driver, remove
        //:    leading comment characters, and replace 'assert' with 'ASSERT'.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) printf("USAGE EXAMPLE\n"
                            "=============\n");

        bsls_ProtocolTest<MyInterfaceTest> t;

        ASSERT(t.testAbstract());
        ASSERT(t.testNoDataMembers());
        ASSERT(t.testVirtualDestructor());
        BSLS_PROTOCOLTEST_ASSERT(t, foo(77));
        BSLS_PROTOCOLTEST_ASSERT(t, bar("", ""));
        testStatus = t.failures();
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // TESTING BSLS_PROTOCOLTEST_ASSERT MACRO
        //
        // Concerns:
        //:  1 'BSLS_PROTOCOLTEST_ASSERT' reports no failures for protocol
        //:    methods satisfying method concerns.
        //:  2 'BSLS_PROTOCOLTEST_ASSERT' report failures for protocol methods
        //:    that don't satisfy concerns.
        //
        // Plan:
        //:  1 Instantiate 'bsls_ProtocolTest' with a protocol class
        //:    with methods that satisfy protocol method concerns and verify
        //:    that 'BSLS_PROTOCOLTEST_ASSERT' reports no failures for those
        //:    methods.
        //:  2 Instantiate 'bsls_ProtocolTest' with a protocol class
        //:    with methods that don't satisfy protocol method concerns and
        //:    verify 'BSLS_PROTOCOLTEST_ASSERT' reports failures for those
        //:    methods.  Intercept 'ASSERT' failures and prevent them from
        //:    being printed to the console for the duration of this test.
        //
        // Testing:
        //   BSLS_PROTOCOLTEST_ASSERT
        // --------------------------------------------------------------------

        if (verbose) printf("TESTING BSLS_PROTOCOLTEST_ASSERT MACRO\n"
                            "======================================\n");

        if (veryVerbose) printf("\ttesting that the macro succeeds with a "
                                "'good' protocol\n");

        {
            bsls_ProtocolTest<MyInterfaceTest> t;
            BSLS_PROTOCOLTEST_ASSERT(t, foo(int()));
            BSLS_PROTOCOLTEST_ASSERT(t, bar("", ""));
        }

        if (veryVerbose) printf("\ttesting that the macro fails with a "
                                "'bad' protocol\n");

        {
            // suspend reporting of assert failures
            int oldTestStatus = testStatus;
            disableAssertOutput = true;

            bsls_ProtocolTest<MyInterfaceNonVirtualTest> t;

            // non-virtual method, should get a failure
            BSLS_PROTOCOLTEST_ASSERT(t, foo(int()));
            int failureTestStatus = testStatus - oldTestStatus;

            // virtual method, should get a success
            BSLS_PROTOCOLTEST_ASSERT(t, bar());
            int successTestStatus = testStatus - oldTestStatus;

            // resume reporting of assert failures
            disableAssertOutput = false;
            testStatus = oldTestStatus;

            ASSERT(failureTestStatus == 1);
            ASSERT(successTestStatus == 1);
        }
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING PROTOCOL METHOD TESTING APARATUS
        //
        // Concerns:
        //:  1 Protocol method calls from the test driver object indirectly
        //:    (using overloaded 'operator->') get correctly dispatched to the
        //:    most derived implementation of those methods.
        //:  2 When the protocol method is correctly declared virtual and
        //:    public, the count of 'failures' doesn't increase and the
        //:    'lastStatus' value is 'true'.
        //:  3 When the protocol method is not declared virtual, the count of
        //:    'failures' value doesn't increase and 'lastStatus' value is
        //:    'false'.
        //:  4 The indirect protocol method calls work for a special case when
        //:    a protocol method returns a reference to a non-copyable type.
        //
        // Plan:
        //:  1 Instantiate 'bsls_PrototolTestDriver' with a protocol type that
        //:    declares its methods correctly and verify the 'failures' and
        //:    'lastStatus' assume the proper values after calling the methods
        //:    of the protocol type.
        //:  2 Instantiate 'bsls_ProtocolTest' with a protocol type that
        //:    doesn't declare its methods virtual and verify that the count of
        //:    'failures' increases and 'lastStatus' is 'false'.
        //:  3 Verify that an attempt to indirectly call a non-public method of
        //:    a protocol type produces a compile-time error.
        //:  4 Create a protocol with a method returning a reference to a
        //:    non-copyable type and verify that it can be called correctly
        //:    from the test driver object.
        //
        // Testing:
        //   Protocol method testing aparatus.
        // --------------------------------------------------------------------

        if (verbose) printf("TESTING PROTOCOL METHOD TESTING APARATUS\n"
                            "========================================\n");

        if (veryVerbose) printf("\ttesting 'good' protocol methods\n");

        {
            bsls_ProtocolTest<MyInterfaceTest> t;

            t->foo(int());
            ASSERT(t.failures() == 0);
            ASSERT(t.lastStatus());

            t->bar("", "");
            ASSERT(t.failures() == 0);
            ASSERT(t.lastStatus());
        }

        if (veryVerbose) printf("\ttesting that non-virtual protocol methods "
                                "fail\n");

        {
            bsls_ProtocolTest<MyInterfaceNonVirtualTest> t;

            // verify failure
            t->foo(int());
            ASSERT(t.failures() == 1);
            ASSERT(!t.lastStatus());

            // try one more time
            t->foo(int());
            ASSERT(t.failures() == 2);
            ASSERT(!t.lastStatus());

            // now try to passing test and make sure it doesn't change the
            // count of 'failures'
            t->bar();
            ASSERT(t.failures() == 2);
            ASSERT(t.lastStatus());
        }

        if (veryVerbose) printf("\ttesting that non-public protocol methods "
                                "fail\n");

        {
            bsls_ProtocolTest<MyInterfaceNonPublicTest> t;

            // uncomment to get a compile-time error
            // t->foo(int());
        }

        if (veryVerbose) printf("\ttesting a special case of a method "
                                "returning a reference to non-copyable "
                                "object\n");

        {
            bsls_ProtocolTest<MyInterfaceNonCopyableReturnTest> t;

            t->foo(int());
            ASSERT(t.failures() == 0);
            ASSERT(t.lastStatus());
        }
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING PROTOCOL TEST BASE CLASS
        //
        // Concerns:
        //:  1 An object derived from type 'bsls_ProtocolTestImp' can be
        //:    constructed.
        //:  2 'setTestDriver' establishes the connection between the
        //:    'bsls_ProtocolTestImp' and 'bsls_ProtocolTest'.
        //:  3 'exit', 'exitRef' and 'exitVal' methods when called mark the
        //:    test as "pass" and "fail" when not called.
        //:  4 The type of the return value of 'exit', 'exitRef' and 'exitVal'
        //:    is what we expect.
        //
        // Plan:
        //:  1 Create an object of a class derived from 'bsls_ProtocolTestImp'.
        //:    This is a protocol test object.
        //:  2 Connect the protocol test object with the test driver status
        //:    object using 'setTestStatus'.
        //:  3 Mark the protocol test object as 'entered' by calling 'enter()'.
        //:  4 Do not call any of the 'exit', 'exitRef' or 'exitVal' methods,
        //:    and verify that on destruction the protocol test objects calls
        //:    method 'fail()' of the test driver object (setting the 'failed'
        //:    flag).
        //:  5 Now do call 'exit', 'exitRef' and 'exitVal' individually, and
        //:    verify that on destruction the protocol test objects does not
        //:    call method 'fail()' of the test driver object.
        //:  6 Verify that the return value type of 'exit', 'exitRef' and
        //:    'exitVal' is what's expected.
        //
        // Testing:
        //   class bsls_ProtocolTestImp;
        //   void setTestDriver(bsls_ProtocolTest_Status *);
        //   exit();
        //   exitRef();
        //   exitVal();
        // --------------------------------------------------------------------

        if (verbose) printf("TESTING PROTOCOL TEST BASE CLASS\n"
                            "================================\n");

        if (veryVerbose) printf("\tnot calling 'exit' fails the test\n");

        {
            bsls_ProtocolTest_Status testStatus;
            ASSERT(testStatus.failures() == 0);

            {
                MyInterfaceTest testObj;
                testObj.setTestStatus(&testStatus);
                testObj.enter();
            }

            ASSERT(testStatus.failures() != 0);
        }

        if (veryVerbose) printf("\tcalling 'exit' passes the test\n");

        {
            bsls_ProtocolTest_Status testStatus;
            ASSERT(testStatus.failures() == 0);

            {
                MyInterfaceTest testObj;
                testObj.setTestStatus(&testStatus);
                testObj.enter();
                testObj.exit();
            }

            ASSERT(testStatus.failures() == 0);
        }

        if (veryVerbose) printf("\tcalling 'exitRef' passes the test\n");

        {
            bsls_ProtocolTest_Status testStatus;
            ASSERT(testStatus.failures() == 0);

            {
                MyInterfaceTest testObj;
                testObj.setTestStatus(&testStatus);
                testObj.enter();
                testObj.exitRef();
            }

            ASSERT(testStatus.failures() == 0);
        }

        if (veryVerbose) printf("\tcalling 'exitVal' passes the test\n");

        {
            bsls_ProtocolTest_Status testStatus;
            ASSERT(testStatus.failures() == 0);

            {
                MyInterfaceTest testObj;
                testObj.setTestStatus(&testStatus);
                testObj.enter();
                testObj.exitVal(0);
            }

            ASSERT(testStatus.failures() == 0);
        }

        if (veryVerbose) printf("\t'exit' has a proper return type "
                                "(compile-time)\n");

        {
            MyInterfaceTest testObj;
            bsls_ProtocolTest_MethodReturnType proxy = testObj.exit();
        }

        if (veryVerbose) printf("\t'exitRef' has a proper return type "
                                "(compile-time)\n");

        {
            MyInterfaceTest testObj;
            bsls_ProtocolTest_MethodReturnRefType proxy = testObj.exitRef();
        }

        if (veryVerbose) printf("\t'exitVal' has a proper return type\n");

        {
            MyInterfaceTest testObj;
            int r = testObj.exitVal(10);

            ASSERT(r == 10);
        }
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING PRIVATE COMPONENT CLASSES
        //
        // Concerns:
        //:  1 Private component classes behave according to their contract.
        //
        // Plan:
        //:  1 Verify that bsls_ProtocolTest_IsAbstract meta-function returns
        //:    the correct value for abstract and concrete classes.
        //:  2 Verify that an object of bsls_ProtocolTestImp class can be a proxy
        //:    (by overloading 'operator->()') to the protocol.
        //:  3 Verify that bsls_ProtocolTest_MethodReturnType provides proper
        //:    convertions to value and pointer types.
        //:  4 Verify that bsls_ProtocolTest_MethodReturnRefType provides
        //:    proper convertions to referece types.
        //:  5 Verify that bsls_ProtocolTest_Status correctly keeps track of
        //:    the last test status and the count of failures.
        //
        // Testing:
        //   class bsls_ProtocolTest_IsAbstract;
        //   class bsls_ProtocolTest_MethodReturnType;
        //   class bsls_ProtocolTest_MethodReturnRefType;
        //   class bsls_ProtocolTest_Status;
        //   class bsls_ProtocolTest_Dtor;
        // --------------------------------------------------------------------

        if (verbose) printf("TESTING PRIVATE COMPONENT CLASSES\n"
                            "=================================\n");

        {
            if (veryVerbose) printf("\tbsls_ProtocolTest_IsAbstract\n");

            ASSERT(bsls_ProtocolTest_IsAbstract<MyInterface>::VALUE);
            ASSERT(!bsls_ProtocolTest_IsAbstract<NotAbstractInterface>::VALUE);

            if (veryVerbose) printf("\tbsls_ProtocolTest::operator->()\n");

            {
                bsls_ProtocolTest_Status testStatus;
                MyInterfaceTest testObj;
                testObj.setTestStatus(&testStatus);

                // verify that op->() returns a pointer to the protocol
                ASSERT(static_cast<MyInterface *>(&testObj) ==
                                                         testObj.operator->());
            }

            if (veryVerbose) printf("\tbsls_ProtocolTest_MethodReturnType "
                                    "(compile-time)\n");

            {
                // test the possibility of convertions to value/pointer types
                int val                 = bsls_ProtocolTest_MethodReturnType();
                int const & const_ref   = bsls_ProtocolTest_MethodReturnType();
                int * ptr               = bsls_ProtocolTest_MethodReturnType();
                int const * const_ptr   = bsls_ProtocolTest_MethodReturnType();

                void * vptr             = bsls_ProtocolTest_MethodReturnType();
                void const * const_vptr = bsls_ProtocolTest_MethodReturnType();
            }

            if (veryVerbose) printf("\tbsls_ProtocolTest_MethodReturnRefType "
                                    "(compile-time)\n");

            {
                // test the possibility of convertions to reference types
                NonCopyable & ref = bsls_ProtocolTest_MethodReturnRefType();
                NonCopyable const & const_ref =
                                       bsls_ProtocolTest_MethodReturnRefType();
            }

            if (veryVerbose) printf("\tbsls_ProtocolTest_Status\n");

            {
                bsls_ProtocolTest_Status status;

                // initially "it's all good" (tm)
                ASSERT(status.failures() == 0);
                ASSERT(status.last());

                // simulate test failure
                status.fail();
                ASSERT(status.failures() == 1);
                ASSERT(!status.last());

                // prepare for the next test
                status.resetLast();
                ASSERT(status.failures() == 1);
                ASSERT(status.last());

                // fail the next test
                status.fail();
                ASSERT(status.failures() == 2);
                ASSERT(!status.last());
            }

            if (veryVerbose) printf("\tbsls_ProtocolTest_Dtor\n");

            {
                if (veryVerbose) printf("\t\tpassing case\n");

                bsls_ProtocolTest_Status status;

                {
                    bsls_ProtocolTest_Dtor<MyInterfaceTest> dtorTest;
                    dtorTest.setTestStatus(&status);
                    dtorTest.enter();
                }

                // if dtorTest destructor called 'exit' than the test succeeded
                ASSERT(status.failures() == 0);
                ASSERT(status.last());

                if (veryVerbose) printf("\t\tfailure case\n");

                {
                    union InPlaceObject
                    {
                        char  buffer[
                              sizeof(bsls_ProtocolTest_Dtor<MyInterfaceTest>)];
                        void *alignment;
                    };

                    InPlaceObject o;

                    bsls_ProtocolTest_Dtor<MyInterfaceTest> * dtorTest =
                        new (&o) bsls_ProtocolTest_Dtor<MyInterfaceTest>();

                    dtorTest->enter();
                    dtorTest->setTestStatus(&status);
                    dtorTest->MyInterfaceTest::~MyInterfaceTest();
                }

                // dtorTest destructor is not called, test 'failed'
                ASSERT(status.failures() == 1);
                ASSERT(!status.last());
            }
        }
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING MANIPULATORS
        //
        // Concerns:
        //:  1 'testAbstract' returns 'true' for an abstract class and 'false'
        //:    for a non-abstract class.
        //:  2 'testNoDataMembers' returns 'true' for a class with no data
        //:    fields (but with virtual methods), and 'false' otherwise.
        //:  3 'testVirtualDestructor' returns 'true' for a class with a
        //:    virtual destructor and 'false' for a class with a non-virtual
        //:    destructor.
        //:  4 All above methods set 'failures' and 'lastStatus' properties
        //:    properly after their execution.
        //
        // Plan:
        //:  1 Instantiate 'bsls_ProtocolTest' with an abstract class and
        //:    verify that 'testAbstract' returns 'true'.  Instantiate
        //:    'bsls_PrototolTestDriver' with a non-abstract class and verify
        //:    that 'testAbstract' returns 'false'.
        //:  2 Instantiate 'bsls_ProtocolTest' with a protocol having no
        //:    data fields and verify that 'testNoDataMembers' returns 'true'.
        //:    Instantiate 'bsls_ProtocolTest' with a protocol having
        //:    data fields and verify that 'testNoDataMembers' returns 'false'.
        //:  3 Instantiate 'bsls_ProtocolTest' with a protocol having a
        //:    virtual destructor and verify that 'testVirtualDestructor'
        //:    returns 'true'.  Instantiate 'bsls_ProtocolTest' with a
        //:    protocol withtout a virtual destructor and verify that
        //:    'testVirtualDestructor' returns 'false'.
        //
        // Testing:
        //   bool testAbstract();
        //   bool testNoDataMembers();
        //   bool testVirtualDestructor();
        // --------------------------------------------------------------------

        if (verbose) printf("TESTING MANIPULATORS\n"
                            "====================\n");

        if (veryVerbose) printf("\ttestAbstract()\n");

        {
            if (veryVerbose) printf("\t\twith abstract class\n");

            {
                bsls_ProtocolTest<MyInterfaceTest> t;
                ASSERT(t.testAbstract());
                ASSERT(t.failures() == 0);
                ASSERT(t.lastStatus());
            }

            if (veryVerbose) printf("\t\twith non-abstract class\n");

            {
                bsls_ProtocolTest<NotAbstractInterfaceTest> t;
                ASSERT(!t.testAbstract());
                ASSERT(t.failures() == 1);
                ASSERT(!t.lastStatus());
            }
        }

        if (veryVerbose) printf("\ttestNoDataMembers()\n");

        {
            if (veryVerbose) printf("\t\twith protocol with no data fields\n");

            {
                bsls_ProtocolTest<MyInterfaceTest> t;
                ASSERT(t.testNoDataMembers());
                ASSERT(t.failures() == 0);
                ASSERT(t.lastStatus());
            }

            if (veryVerbose) printf("\t\twith protocol with data fields\n");

            {
                bsls_ProtocolTest<WithDataFieldsTest> t;
                ASSERT(!t.testNoDataMembers());
                ASSERT(t.failures() == 1);
                ASSERT(!t.lastStatus());
            }
        }

        if (veryVerbose) printf("\ttestVirtualDestructor()\n");

        {
            if (veryVerbose) printf("\t\twith protocol having a virtual "
                                    "destructor\n");

            {
                bsls_ProtocolTest<MyInterfaceTest> t;
                ASSERT(t.testVirtualDestructor());
                ASSERT(t.failures() == 0);
                ASSERT(t.lastStatus());
            }

            if (veryVerbose) printf("\t\twith protocol without a virtual "
                                    "destructor\n");

            {
                bsls_ProtocolTest<NoVirtualDestructorTest> t;
                ASSERT(!t.testVirtualDestructor());
                ASSERT(t.failures() == 1);
                ASSERT(!t.lastStatus());
            }
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING BASIC ACCESSORS
        //
        // Concerns:
        //: 1 Each accessor return a value correctly describing the state of
        //:   that object.
        //
        // Plan:
        //: 1 Create a default-constructed 'bsls_ProtocolTest' object and
        //:   assign it to a 'const' reference.
        //: 2 Verify that accessors on the 'const' reference return proper
        //:   values.
        //
        // Testing:
        //   int failures();
        //   bool lastStatus();
        // --------------------------------------------------------------------

        if (verbose) printf("TESTING BASIC ACCESSORS\n"
                            "=======================\n");

        bsls_ProtocolTest<MyInterfaceTest> t;

        ASSERT(t.failures() == 0);
        ASSERT(t.lastStatus());
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING CONSTRUCTORS AND DESTRUCTORS
        //   Ensure that the classes in this component can be constructed and
        //   destructed.
        //
        // Concerns:
        //: 1 The value constructor for 'bsls_ProtocolTest' can create an
        //:   object of a that class.
        //
        // Plan:
        //: 1 Instantiate 'bsls_ProtocolTest' with 'MyTestInterfaceTest'
        //:   and create an object of that type.
        //
        // Testing:
        //   bsls_ProtocolTest();
        //   ~bsls_ProtocolTest();
        // --------------------------------------------------------------------

        if (verbose) printf("TESTING CONSTRUCTORS AND DESTRUCTORS\n"
                            "====================================\n");

        bsls_ProtocolTest<MyInterfaceTest> t;
        testStatus = t.failures();
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //   This test excercies (but doesn't fully test basic functionality.
        //
        // Concerns:
        //: 1 Classes in this component are sufficiently functional to enable
        //:   comprehensive testing in subsequent test cases.
        //
        // Plan:
        //: 1 Create an object of 'bsls_PrototolTestDriver' and try some of its
        //:   accessors.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) printf("BREATHING TEST\n"
                            "==============\n");

        bsls_ProtocolTest<MyInterfaceTest> t;

        ASSERT(t.failures() == 0);
        ASSERT(t.lastStatus());
      } break;
      default: {
        fprintf(stderr, "WARNING: CASE `%d' NOT FOUND.\n", test);
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        fprintf(stderr, "Error, non-zero test status = %d.\n", testStatus);
    }
    return testStatus;
}

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2011
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
