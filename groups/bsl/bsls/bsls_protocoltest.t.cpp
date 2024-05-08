// bsls_protocoltest.t.cpp                                            -*-C++-*-

// Test case 4 of this test driver calls the 'testVirtualDestructor' method on
// a 'bsls::ProtocolTest' object instantiation that has no virtual destructor.
// This call generates expected compiler warnings that can be silenced only by
// surrounding the '#include' directive for 'bsls_protocoltest.h' with
// diagnostic pragmas.  The diagnostic pragmas are platform-dependent, so
// 'bsls_platform.h' must be included before the component header, contrary to
// the usual requirements of the BDE coding standards.

#include <bsls_platform.h>
#ifdef BSLS_PLATFORM_HAS_PRAGMA_GCC_DIAGNOSTIC
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wdelete-non-virtual-dtor"
#endif
#include <bsls_protocoltest.h>
#ifdef BSLS_PLATFORM_HAS_PRAGMA_GCC_DIAGNOSTIC
    #pragma GCC diagnostic pop
#endif

#include <bsls_bsltestutil.h>

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
// class bsls::ProtocolTest:
// [ 2] bsls::ProtocolTest();
// [ 2] ~bsls::ProtocolTest();
// [ 3] int failures();
// [ 3] bool lastStatus();
// [ 4] bool testAbstract();
// [ 4] bool testNoDataMembers();
// [ 4] bool testVirtualDestructor();
// [ 7] operator->();
//
// private classes:
// [ 5] class bsls::ProtocolTest_IsAbstract;
// [ 5] class bsls::ProtocolTest_MethodReturnType;
// [ 5] class bsls::ProtocolTest_MethodReturnRefType;
//
// class bsls::ProtocolTestImp:
// [ 6] void setTestStatus(bsls::ProtocolTest_Status *);
// [ 6] markDone();
// [ 6] markDoneRef();
// [ 6] markDoneVal();
//
// class bsls::ProtocolTest_Status:
// [ 5] bsls::ProtocolTest_Status();
// [ 5] resetLast();
// [ 5] fail();
// [ 5] last();
// [ 5] failures();
//
// [ 8] BSLS_PROTOCOLTEST_ASSERT
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 9] USAGE EXAMPLE

// ============================================================================
//                EXTENDED STANDARD BSL ASSERT TEST FUNCTION
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

}  // close unnamed namespace

// ============================================================================
//               STANDARD BSL TEST DRIVER MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT       BSLS_BSLTESTUTIL_ASSERT
#define ASSERTV      BSLS_BSLTESTUTIL_ASSERTV

#define LOOP_ASSERT  BSLS_BSLTESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLS_BSLTESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLS_BSLTESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLS_BSLTESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLS_BSLTESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLS_BSLTESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLS_BSLTESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLS_BSLTESTUTIL_LOOP6_ASSERT

#define Q            BSLS_BSLTESTUTIL_Q   // Quote identifier literally.
#define P            BSLS_BSLTESTUTIL_P   // Print identifier and value.
#define P_           BSLS_BSLTESTUTIL_P_  // P(X) without '\n'.
#define T_           BSLS_BSLTESTUTIL_T_  // Print a tab (w/o newline).
#define L_           BSLS_BSLTESTUTIL_L_  // current Line number

// ============================================================================
//                              USAGE EXAMPLE
// ----------------------------------------------------------------------------

namespace {

///Example 1: Testing a Protocol Class
///- - - - - - - - - - - - - - - - - -
// This example demonstrates how to test a protocol class, 'ProtocolClass',
// using this protocol test component.  Our 'ProtocolClass' provides two of
// pure virtual methods ('foo' and 'bar'), along with a virtual destructor:
//..
struct ProtocolClass {
    virtual ~ProtocolClass();
    virtual const char *bar(char const *, char const *) = 0;
    virtual int foo(int) const = 0;
};

ProtocolClass::~ProtocolClass()
{
}
//..
// First, we define a test class derived from this protocol, and implement its
// virtual methods.  Rather than deriving the test class from 'ProtocolClass'
// directly, the test class is derived from
// 'bsls::ProtocolTestImp<ProtocolClass>' (which, in turn, is derived
// automatically from 'ProtocolClass').  This special base class implements
// boilerplate code and provides useful functionality for testing of protocols.
//..
// ========================================================================
//                  GLOBAL CLASSES/TYPEDEFS FOR TESTING
// ------------------------------------------------------------------------

struct ProtocolClassTestImp : bsls::ProtocolTestImp<ProtocolClass> {
    const char *bar(char const *, char const *) { return markDone(); }
    int foo(int) const                          { return markDone(); }
};
//..
// Notice that in 'ProtocolClassTestImp' we must provide an implementation for
// every protocol method except for the destructor.  The implementation of each
// method calls the (protected) 'markDone' which is provided by the base class
// for the purpose of verifying that the method from which it's called is
// declared as virtual in the protocol class.
//
// See continuation in 'main()'.

///Example 2: Testing a Method Overloaded on 'const'ness
///- - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we have a protocol that represent a sequence of integers.  Such a
// protocol will have an overloaded 'at()' method of both the 'const' and the
// "mutable" variation.  In verification of such methods we need to ensure that
// we verify *both* overloads of the 'virtual' function.
//
// First let's define the interesting parts of our imaginary sequence, the
// overloaded 'at()' methods, and a virtual destructor to avoid warnings:
//..
struct IntSeqExample {
    // CREATORS
    virtual ~IntSeqExample();

    // MANIPULATORS
    virtual int& at(size_t index) = 0;

    // ACCESSORS
    virtual int at(size_t index) const = 0;
};

IntSeqExample::~IntSeqExample()
{
}
//..
// Next, we define the test implementation as usual:
//..
struct IntSeqExampleTestImp : bsls::ProtocolTestImp<IntSeqExample> {
    static int s_int;

    int& at(size_t)       { s_int = 4; markDone(); return s_int; }
    int  at(size_t) const { s_int = 2; return markDone();        }
};

int IntSeqExampleTestImp::s_int = 0;
//..
// Note the use of a dummy variable to return a reference.  We also use that
// variable, by giving it different values in the two overloads, to demonstrate
// that we have called the overload we have intended.
//
// The example is continued in 'main()'.

// ============================================================================
//                    TYPES AND FUNCTIONS REQUIRED FOR TESTING
// ----------------------------------------------------------------------------

typedef ProtocolClass        MyInterface;
typedef ProtocolClassTestImp MyInterfaceTest;

// For testing 'testAbstract'.
struct NotAbstractInterface {
    void foo();
};

struct NotAbstractInterfaceTest : bsls::ProtocolTestImp<NotAbstractInterface> {
    void foo() { markDone(); }
};

// For testing 'testNoDataMembers'.
struct WithDataFields {
    virtual ~WithDataFields() {}

  private:
    int a;

  public:
    int *aPtr() {
        return &a;
    }
};

struct WithDataFieldsTest : bsls::ProtocolTestImp<WithDataFields> {
};

// For testing 'testVirtualDestructor'.
struct NoVirtualDestructor {
    virtual void foo() = 0;
};

struct NoVirtualDestructorTest : bsls::ProtocolTestImp<NoVirtualDestructor> {
    void foo() { markDone(); }
};

// For testing return value conversions to a reference to a non-copyable type
struct NonCopyable {
  private:
    NonCopyable(NonCopyable const &);
    NonCopyable & operator=(NonCopyable const &);
};

// For testing protocol method testing apparatus.
struct MyInterfaceNonVirtual {
    int foo(int) { return 0; }
    virtual void bar() = 0;
};

struct MyInterfaceNonVirtualTest
    : bsls::ProtocolTestImp<MyInterfaceNonVirtual>
{
    int foo(int) { return markDone(); }
    void bar()   { markDone(); }
};

struct MyInterfaceNonPublic {
  private:
    virtual int foo(int) = 0;
};

struct MyInterfaceNonPublicTest : bsls::ProtocolTestImp<MyInterfaceNonPublic> {
    // This override must be public.
    int foo(int) { return markDone(); }
};

struct MyInterfaceNonCopyableReturn {
    virtual const NonCopyable& foo(int) = 0;
};

struct MyInterfaceNonCopyableReturnTest
    : bsls::ProtocolTestImp<MyInterfaceNonCopyableReturn> {
    const NonCopyable& foo(int) { return markDoneRef(); }
};

struct DummyAllocator {
    // Make sure the protocol test works with types which override operator
    // new.

    virtual ~DummyAllocator() {}

    void *operator new(std::size_t size)
    {
        return ::operator new(size);
    }

    void operator delete(void *p)
    {
        ::operator delete(p);
    }

    // placement new and delete
    void *operator new(std::size_t, DummyAllocator *);
    void operator delete(void *, DummyAllocator *);
};

struct DummyAllocatorTest : bsls::ProtocolTestImp<DummyAllocator> {};

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

    (void)veryVeryVerbose;

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

        if (verbose) puts("\nUSAGE EXAMPLE"
                          "\n=============");
        {
///Example 1: Testing a Protocol Class
///- - - - - - - - - - - - - - - - - -
// See type definitions in the global scope.
//
// Then, in our protocol test case we describe the concerns we have for the
// protocol class and the plan to test those concerns:
//..
      // ----------------------------------------------------------------------
      // PROTOCOL TEST:
      //   Ensure this class is a properly defined protocol.
      //
      // Concerns:
      //: 1 The protocol is abstract: no objects of it can be created.
      //:
      //: 2 The protocol has no data members.
      //:
      //: 3 The protocol has a virtual destructor.
      //:
      //: 4 All methods of the protocol are pure virtual.
      //:
      //: 5 All methods of the protocol are publicly accessible.
      //
      // Plan:
      //: 1 Define a concrete derived implementation, 'ProtocolClassTestImp',
      //:   of the protocol.
      //:
      //: 2 Create an object of the 'bsls::ProtocolTest' class template
      //:   parameterized by 'ProtocolClassTestImp', and use it to verify
      //:   that:
      //:
      //:   1 The protocol is abstract. (C-1)
      //:
      //:   2 The protocol has no data members. (C-2)
      //:
      //:   3 The protocol has a virtual destructor. (C-3)
      //:
      //: 3 Use the 'BSLS_PROTOCOLTEST_ASSERT' macro to verify that
      //:   non-creator methods of the protocol are:
      //:
      //:   1 virtual, (C-4)
      //:
      //:   2 publicly accessible. (C-5)
      //
      // Testing:
      //   virtual ~ProtocolClass();
      //   virtual const char *bar(char const *, char const *) = 0;
      //   virtual int foo(int) const = 0;
      // ----------------------------------------------------------------------
//..
// Next we print the banner for this test case:
//..
      if (verbose) puts("\nPROTOCOL TEST"
                        "\n=============");
//..
// Then, we create an object of type
// 'bsls::ProtocolTest<ProtocolClassTestImp>', 'testObj':
//..
      if (verbose) puts("\n\tCreate a test object.");

      bsls::ProtocolTest<ProtocolClassTestImp> testObj(veryVerbose);
//..
// Now we use the 'testObj' to test some general concerns about the protocol
// class.
//..
      if (verbose) puts("\tVerify that the protocol is abstract.");

      ASSERT(testObj.testAbstract());

      if (verbose) puts("\tVerify that there are no data members.");

      ASSERT(testObj.testNoDataMembers());

      if (verbose) puts("\tVerify that the destructor is virtual.");

      ASSERT(testObj.testVirtualDestructor());
//..
// Finally we use the 'testObj' to test concerns for each individual method of
// the protocol class.  To test a protocol method we need to call it from
// inside the 'BSLS_PROTOCOLTEST_ASSERT' macro, and also pass the 'testObj':
//..
      if (verbose) puts("\tVerify that methods are public and virtual.");

      BSLS_PROTOCOLTEST_ASSERT(testObj, foo(77));
      BSLS_PROTOCOLTEST_ASSERT(testObj, bar("", ""));
//..
// These steps conclude the protocol testing.  If there are any failures, they
// will be reported via standard test driver assertions (i.e., the standard
// 'ASSERT' macro).
      }

      {
///Example 2: Testing a Method Overloaded on 'const'ness
///- - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we have a protocol that represent a sequence of integers.  Such a
// protocol will have an overloaded 'operators'at()' method of both the 'const'
// and the "mutable" variation.  In verification of such methods we need to
// ensure that we verify *both* overloads of the 'virtual' function.
//
// First let's define the interesting parts of our imaginary sequence, the
// overloaded 'at()' methods, and a virtual destructor to avoid warnings:
//..
//  struct IntSeqExample {
//      // CREATORS
//      virtual ~IntSeqExample() = 0;
//
//      // MANIPULATORS
//      virtual int& at(bsl::size_t index) = 0;
//
//      // ACCESSORS
//      virtual int at(bsl::size_t index) const = 0;
//  };
//
//  IntSeqExample::~IntSeqExample()
//  {
//  }
//..
// Next, we define the test implementation as usual:
//..
//  struct IntSeqExampleTestImp : bsls::ProtocolTestImp<IntSeqExample> {
//      static int s_int;
//
//      int& at(size_t)       { s_int = 4; markDone(); return s_int; }
//      int  at(size_t) const { s_int = 2; return markDone();        }
//  };
//
//  int IntSeqExampleTestImp::s_int = 0;
//..
// Note the use of a dummy variable to return a reference.  We also use that
// variable, by giving it different values in the two overloads, to demonstrate
// we that indeed will call both, and the expected one.
//
// Then, we test the non-'const' overload as usual:
//..
      bsls::ProtocolTest<IntSeqExampleTestImp> testObj(veryVerbose);
      BSLS_PROTOCOLTEST_ASSERT(testObj, at(0));
//..
// Now, we can verify that we have indeed tested the non-'const' overload:
//..
      ASSERT(4 == IntSeqExampleTestImp::s_int);
//..
// Finally, we test 'at(size_t) const' and also verify that we indeed called
// the intended overload.  Notice that we "force" the 'const' variant of the
// method to be picked by specifying a 'const Implementation' type argument to
// 'bsls::ProtocolTest':
//..
      bsls::ProtocolTest<const IntSeqExampleTestImp> test_OBJ(veryVerbose);
      BSLS_PROTOCOLTEST_ASSERT(test_OBJ, at(0));

      ASSERT(2 == IntSeqExampleTestImp::s_int);
//..
// Note that the assertion that verifies that the intended overload was called
// is not strictly necessary, it is included for demonstration purposes.
      }
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // TESTING BSLS_PROTOCOLTEST_ASSERT MACRO
        //
        // Concerns:
        //:  1 'BSLS_PROTOCOLTEST_ASSERT' reports no failures for protocol
        //:    methods satisfying method concerns.
        //:
        //:  2 'BSLS_PROTOCOLTEST_ASSERT' report failures for protocol methods
        //:    that don't satisfy concerns.
        //
        // Plan:
        //:  1 Instantiate 'bsls::ProtocolTest' with a protocol class
        //:    with methods that satisfy protocol method concerns and verify
        //:    that 'BSLS_PROTOCOLTEST_ASSERT' reports no failures for those
        //:    methods.
        //:
        //:  2 Instantiate 'bsls::ProtocolTest' with a protocol class
        //:    with methods that don't satisfy protocol method concerns and
        //:    verify 'BSLS_PROTOCOLTEST_ASSERT' reports failures for those
        //:    methods.  Intercept 'ASSERT' failures and prevent them from
        //:    being printed to the console for the duration of this test.
        //
        // Testing:
        //   BSLS_PROTOCOLTEST_ASSERT
        // --------------------------------------------------------------------

        if (verbose) puts("BSLS_PROTOCOLTEST_ASSERT MACRO"
                          "==============================");

        if (veryVerbose) puts("\tsucceeds with a 'good' protocol");
        {
            bsls::ProtocolTest<MyInterfaceTest> t;
            BSLS_PROTOCOLTEST_ASSERT(t, foo(int()));
            BSLS_PROTOCOLTEST_ASSERT(t, bar("", ""));
        }

        if (veryVerbose) puts("\tverbose mode\n");
        {
            bsls::ProtocolTest<MyInterfaceTest> t(veryVerbose);
            BSLS_PROTOCOLTEST_ASSERT(t, foo(int()));
            BSLS_PROTOCOLTEST_ASSERT(t, bar("", ""));
        }

        if (veryVerbose) puts("\tfails with a 'bad' protocol\n");
        {
            // suspend reporting of assert failures
            int oldTestStatus = testStatus;
            disableAssertOutput = true;

            bsls::ProtocolTest<MyInterfaceNonVirtualTest> t;

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
        // PROTOCOL METHOD TESTING APPARATUS
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
        //:  1 Instantiate 'bsls::ProtocolTestDriver' with a protocol type that
        //:    declares its methods correctly and verify the 'failures' and
        //:    'lastStatus' assume the proper values after calling the methods
        //:    of the protocol type.
        //:  2 Instantiate 'bsls::ProtocolTest' with a protocol type that
        //:    doesn't declare its methods virtual and verify that the count of
        //:    'failures' increases and 'lastStatus' is 'false'.
        //:  3 Verify that an attempt to indirectly call a non-public method of
        //:    a protocol type produces a compile-time error.
        //:  4 Create a protocol with a method returning a reference to a
        //:    non-copyable type and verify that it can be called correctly
        //:    from the test driver object.
        //
        // Testing:
        //   Protocol method testing apparatus.
        // --------------------------------------------------------------------

        if (verbose) puts("\nPROTOCOL METHOD TESTING APPARATUS"
                          "\n=================================");

        if (veryVerbose) puts("\t'good' protocol methods");
        {
            bsls::ProtocolTest<MyInterfaceTest> t;

            t.method()->foo(int());
            ASSERT(t.failures() == 0);
            ASSERT(t.lastStatus());

            t.method()->bar("", "");
            ASSERT(t.failures() == 0);
            ASSERT(t.lastStatus());
        }

        if (veryVerbose) puts("\tnon-virtual protocol methods fail");
        {
            bsls::ProtocolTest<MyInterfaceNonVirtualTest> t;

            // verify failure
            t.method()->foo(int());
            ASSERT(t.failures() == 1);
            ASSERT(!t.lastStatus());

            // try one more time
            t.method()->foo(int());
            ASSERT(t.failures() == 2);
            ASSERT(!t.lastStatus());

            // now try to passing test and make sure it doesn't change the
            // count of 'failures'
            t.method()->bar();
            ASSERT(t.failures() == 2);
            ASSERT(t.lastStatus());
        }

        if (veryVerbose) puts("\tnon-public protocol methods fail");
        {
            bsls::ProtocolTest<MyInterfaceNonPublicTest> t;

            // uncomment to get a compile-time error
            // t->foo(int());
        }

        if (veryVerbose) puts("\tspecial case of a method returning a "
                                           "reference to non-copyable object");
        {
            bsls::ProtocolTest<MyInterfaceNonCopyableReturnTest> t;

            t.method()->foo(int());
            ASSERT(t.failures() == 0);
            ASSERT(t.lastStatus());
        }
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // PROTOCOL TEST BASE CLASS
        //
        // Concerns:
        //:  1 An object derived from type 'bsls::ProtocolTestImp' can be
        //:    constructed.
        //:  2 'setTestDriver' establishes the connection between the
        //:    'bsls::ProtocolTestImp' and 'bsls::ProtocolTest'.
        //:  3 'markDone', 'markDoneRef' and 'markDoneVal' methods when called
        //:    mark the test as "pass" and "fail" when not called.
        //:  4 The type of the return value of 'markDone', 'markDoneRef' and
        //:    'markDoneVal' is what we expect.
        //
        // Plan:
        //:  1 Create an object of a class derived from
        //:    'bsls::ProtocolTestImp'.  This is a protocol test object.
        //:  2 Connect the protocol test object with the test driver status
        //:    object using 'setTestStatus'.
        //:  3 Mark the protocol test object as 'entered' by calling
        //:    'markEnter()'.
        //:  4 Do not call any of the 'markDone', 'markDoneRef' or
        //:    'markDoneVal' methods, and verify that on destruction the
        //:     protocol test objects calls method 'fail()' of the test
        //:     driver object (setting the 'failed' flag).
        //:  5 Now do call 'markDone', 'markDoneRef' and 'markDoneVal'
        //:    individually, and verify that on destruction the protocol test
        //:    objects does not call method 'fail()' of the test driver object.
        //:  6 Verify that the return value type of 'markDone', 'markDoneRef'
        //:    and 'markDoneVal' is what's expected.
        //
        // Testing:
        //   class bsls::ProtocolTestImp;
        //   void setTestDriver(bsls::ProtocolTest_Status *);
        //   markDone();
        //   markDoneRef();
        //   markDoneVal();
        // --------------------------------------------------------------------

        if (verbose) puts("\nPROTOCOL TEST BASE CLASS"
                          "\n========================");

        if (veryVerbose) puts("\tnot calling 'markDone' fails the test");
        {
            bsls::ProtocolTest_Status testStatus;
            ASSERT(testStatus.failures() == 0);

            {
                MyInterfaceTest testObj;
                testObj.setTestStatus(&testStatus);
                testObj.markEnter();
            }

            ASSERT(testStatus.failures() != 0);
        }

        if (veryVerbose) puts("\tcalling 'markDone' passes the test");
        {
            bsls::ProtocolTest_Status testStatus;
            ASSERT(testStatus.failures() == 0);

            {
                MyInterfaceTest testObj;
                testObj.setTestStatus(&testStatus);
                testObj.markEnter();
                testObj.markDone();
            }

            ASSERT(testStatus.failures() == 0);
        }

        if (veryVerbose) puts("\tcalling 'markDoneRef' passes the test");
        {
            bsls::ProtocolTest_Status testStatus;
            ASSERT(testStatus.failures() == 0);

            {
                MyInterfaceTest testObj;
                testObj.setTestStatus(&testStatus);
                testObj.markEnter();
                testObj.markDoneRef();
            }

            ASSERT(testStatus.failures() == 0);
        }

        if (veryVerbose) puts("\tcalling 'markDoneVal' passes the test");
        {
            bsls::ProtocolTest_Status testStatus;
            ASSERT(testStatus.failures() == 0);

            {
                MyInterfaceTest testObj;
                testObj.setTestStatus(&testStatus);
                testObj.markEnter();
                testObj.markDoneVal(0);
            }

            ASSERT(testStatus.failures() == 0);
        }

        if (veryVerbose) puts("\t'markDone' has a proper return type "
                                                             "(compile-time)");
        {
            MyInterfaceTest testObj;
            bsls::ProtocolTest_MethodReturnType proxy = testObj.markDone();
            (void)proxy;
        }

        if (veryVerbose) puts("\t'markDoneRef' has a proper return type "
                                                             "(compile-time)");
        {
            MyInterfaceTest testObj;
            bsls::ProtocolTest_MethodReturnRefType proxy =
                                                         testObj.markDoneRef();
            (void)proxy;
        }

        if (veryVerbose) puts("\t'markDoneVal' has a proper return type");
        {
            MyInterfaceTest testObj;
            int r = testObj.markDoneVal(10);

            ASSERT(r == 10);
        }
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // PRIVATE COMPONENT CLASSES
        //
        // Concerns:
        //:  1 Private component classes behave according to their contract.
        //
        // Plan:
        //:  1 Verify that bsls::ProtocolTest_IsAbstract meta-function returns
        //:    the correct value for abstract and concrete classes.
        //:  2 Verify that an object of bsls::ProtocolTestImp class can be a
        //:    proxy (by overloading 'operator->()') to the protocol.
        //:  3 Verify that bsls::ProtocolTest_MethodReturnType provides proper
        //:    conversions to value and pointer types.
        //:  4 Verify that bsls::ProtocolTest_MethodReturnRefType provides
        //:    proper conversions to reference types.
        //:  5 Verify that bsls::ProtocolTest_Status correctly keeps track of
        //:    the last test status and the count of failures.
        //
        // Testing:
        //   class bsls::ProtocolTest_IsAbstract;
        //   class bsls::ProtocolTest_MethodReturnType;
        //   class bsls::ProtocolTest_MethodReturnRefType;
        //   class bsls::ProtocolTest_Status;
        //   class bsls::ProtocolTest_Dtor;
        // --------------------------------------------------------------------

        if (verbose) puts("\nPRIVATE COMPONENT CLASSES"
                          "\n=========================");

        if (veryVerbose) puts("\tbsls_ProtocolTest_IsAbstract");

        ASSERT(bsls::ProtocolTest_IsAbstract<MyInterface>::value);
        ASSERT(!bsls::ProtocolTest_IsAbstract<NotAbstractInterface>::value);

        if (veryVerbose) puts("\tbsls_ProtocolTest::operator->()");
        {
            bsls::ProtocolTest_Status testStatus;
            MyInterfaceTest testObj;
            testObj.setTestStatus(&testStatus);

            // verify that op->() returns a pointer to the protocol
            ASSERT(static_cast<MyInterface *>(&testObj) ==
                                                        testObj.operator->());
        }

        if (veryVerbose) puts("\tbsls_ProtocolTest_MethodReturnType "
                                                             "(compile-time)");
        {
            // test the possibility of conversions to value/pointer types
            int val                 = bsls::ProtocolTest_MethodReturnType();
            int const & const_ref   = bsls::ProtocolTest_MethodReturnType();
            int * ptr               = bsls::ProtocolTest_MethodReturnType();
            int const * const_ptr   = bsls::ProtocolTest_MethodReturnType();

            void * vptr             = bsls::ProtocolTest_MethodReturnType();
            void const * const_vptr = bsls::ProtocolTest_MethodReturnType();

            (void)val;
            (void)const_ref;
            (void)const_ptr;
            (void)ptr;
            (void)vptr;
            (void)const_vptr;
        }

        if (veryVerbose) puts("\tbsls_ProtocolTest_MethodReturnRefType "
                                                             "(compile-time)");
        {
            // test the possibility of conversions to reference types
            NonCopyable& ref = bsls::ProtocolTest_MethodReturnRefType();
            const NonCopyable& const_ref =
                                      bsls::ProtocolTest_MethodReturnRefType();
            (void)ref;
            (void)const_ref;

            // test the possibility of conversions to abstract classes

            ProtocolClass& protocolRef =
                                      bsls::ProtocolTest_MethodReturnRefType();
            (void)protocolRef;
        }

        if (veryVerbose) puts("\tbsls_ProtocolTest_Status");
        {
            bsls::ProtocolTest_Status status;

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

        if (veryVerbose) puts("\tbsls_ProtocolTest_Dtor");
        {
            if (veryVerbose) puts("\t\tpassing case");

            bsls::ProtocolTest_Status status;

            {
                bsls::ProtocolTest_Dtor<MyInterfaceTest> dtorTest;
                dtorTest.setTestStatus(&status);
                dtorTest.markEnter();
            }

            // if dtorTest destructor called 'markDone' then the test
            // succeeded

            ASSERT(status.failures() == 0);
            ASSERT(status.last());

#ifndef BSLS_PLATFORM_CMP_CLANG
            // this test is broken on Clang TBD (afeher) how?  Why?

            if (veryVerbose) puts("\t\tfailure case");
            {
                union InPlaceObject
                {
                    char  buffer[sizeof(
                                    bsls::ProtocolTest_Dtor<MyInterfaceTest>)];
                    void *alignment;
                };

                InPlaceObject o;

                bsls::ProtocolTest_Dtor<MyInterfaceTest> * dtorTest =
                           new (&o) bsls::ProtocolTest_Dtor<MyInterfaceTest>();

                dtorTest->markEnter();
                dtorTest->setTestStatus(&status);
                dtorTest->MyInterfaceTest::~MyInterfaceTest();
            }

            // dtorTest destructor is not called, the test 'failed'

            ASSERT(status.failures() == 1);
            ASSERT(!status.last());
#endif
        }
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // MANIPULATORS
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
        //:  1 Instantiate 'bsls::ProtocolTest' with an abstract class and
        //:    verify that 'testAbstract' returns 'true'.  Instantiate
        //:    'bsls::ProtocolTestDriver' with a non-abstract class and verify
        //:    that 'testAbstract' returns 'false'.
        //:  2 Instantiate 'bsls::ProtocolTest' with a protocol having no
        //:    data fields and verify that 'testNoDataMembers' returns 'true'.
        //:    Instantiate 'bsls::ProtocolTest' with a protocol having
        //:    data fields and verify that 'testNoDataMembers' returns 'false'.
        //:  3 Instantiate 'bsls::ProtocolTest' with a protocol having a
        //:    virtual destructor and verify that 'testVirtualDestructor'
        //:    returns 'true'.  Instantiate 'bsls::ProtocolTest' with a
        //:    protocol without a virtual destructor and verify that
        //:    'testVirtualDestructor' returns 'false'.
        //
        // Testing:
        //   bool testAbstract();
        //   bool testNoDataMembers();
        //   bool testVirtualDestructor();
        // --------------------------------------------------------------------

        if (verbose) puts("\nMANIPULATORS"
                          "\n============");

        if (veryVerbose) puts("\ttestAbstract() with");
        {
            if (veryVerbose) puts("\t\tan abstract class");
            {
                bsls::ProtocolTest<MyInterfaceTest> t;
                ASSERT(t.testAbstract());
                ASSERT(t.failures() == 0);
                ASSERT(t.lastStatus());
            }

            if (veryVerbose) puts("\t\ta non-abstract class");
            {
                bsls::ProtocolTest<NotAbstractInterfaceTest> t;
                ASSERT(!t.testAbstract());
                ASSERT(t.failures() == 1);
                ASSERT(!t.lastStatus());
            }
        }

        if (veryVerbose) puts("\ttestNoDataMembers() with");
        {
            if (veryVerbose) puts("\t\ta protocol with no data fields");
            {
                bsls::ProtocolTest<MyInterfaceTest> t;
                ASSERT(t.testNoDataMembers());
                ASSERT(t.failures() == 0);
                ASSERT(t.lastStatus());
            }

            if (veryVerbose) puts("\t\ta protocol with data fields");
            {
                bsls::ProtocolTest<WithDataFieldsTest> t;
                ASSERT(!t.testNoDataMembers());
                ASSERT(t.failures() == 1);
                ASSERT(!t.lastStatus());
            }
        }

        if (veryVerbose) puts("\ttestVirtualDestructor() with");
        {
            if (veryVerbose) puts("\t\ta protocol having virtual destructor");
            {
                bsls::ProtocolTest<MyInterfaceTest> t;
                ASSERT(t.testVirtualDestructor());
                ASSERT(t.failures() == 0);
                ASSERT(t.lastStatus());
            }

            if (veryVerbose) puts("\t\ta protocol without virtual destructor");
            {
                bsls::ProtocolTest<NoVirtualDestructorTest> t;

                // Note that the following call to 'testVirtualDestructor'
                // generates an expected "-Wdelete-non-virtual-dtor" warning on
                // gcc.  The warning is suppressed using a diagnostic pragma
                // around the '#include' directive for 'bsls_protocoltest.h'

                ASSERT(!t.testVirtualDestructor());
                ASSERT(t.failures() == 1);
                ASSERT(!t.lastStatus());
            }

            if (veryVerbose) puts("\t\ta protocol having operator new");
            {
                bsls::ProtocolTest<DummyAllocatorTest> t;
                ASSERT(t.testVirtualDestructor());
                ASSERT(t.failures() == 0);
                ASSERT(t.lastStatus());
            }
        }

        if (veryVerbose) puts("\tmanipulators in verbose mode");
        {
            bsls::ProtocolTest<MyInterfaceTest> t(veryVerbose);
            ASSERT(t.testAbstract());
            ASSERT(t.testNoDataMembers());
            ASSERT(t.testVirtualDestructor());
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // BASIC ACCESSORS
        //
        // Concerns:
        //: 1 Each accessor return a value correctly describing the state of
        //:   that object.
        //
        // Plan:
        //: 1 Create a default-constructed 'bsls::ProtocolTest' object and
        //:   assign it to a 'const' reference.
        //: 2 Verify that accessors on the 'const' reference return proper
        //:   values.
        //
        // Testing:
        //   int failures();
        //   bool lastStatus();
        // --------------------------------------------------------------------

        if (verbose) puts("\nBASIC ACCESSORS"
                          "\n===============");

        bsls::ProtocolTest<MyInterfaceTest> t;

        ASSERT(t.failures() == 0);
        ASSERT(t.lastStatus());
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // CONSTRUCTORS AND DESTRUCTORS
        //   Ensure that the classes in this component can be constructed and
        //   destructed.
        //
        // Concerns:
        //: 1 The value constructor for 'bsls::ProtocolTest' can create an
        //:   object of a that class.
        //
        // Plan:
        //: 1 Instantiate 'bsls::ProtocolTest' with 'MyTestInterfaceTest'
        //:   and create an object of that type.
        //
        // Testing:
        //   bsls::ProtocolTest();
        //   ~bsls::ProtocolTest();
        // --------------------------------------------------------------------

        if (verbose) puts("\nCONSTRUCTORS AND DESTRUCTORS"
                          "\n============================");

        bsls::ProtocolTest<MyInterfaceTest> t;
        testStatus = t.failures();
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //   This test exercise (but doesn't fully test basic functionality.
        //
        // Concerns:
        //: 1 Classes in this component are sufficiently functional to enable
        //:   comprehensive testing in subsequent test cases.
        //
        // Plan:
        //: 1 Create an object of 'bsls::ProtocolTestDriver' and try some of
        //:   its accessors.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) puts("\nBREATHING TEST"
                          "\n==============");

        bsls::ProtocolTest<MyInterfaceTest> t;

        ASSERT(t.failures() == 0);
        ASSERT(t.lastStatus());
      } break;
      default: {
        fprintf(stderr, "WARNING: CASE `%d' NOT FOUND.\n", test);
        testStatus = -1;
      }
    }

    (void)&WithDataFields::aPtr;

    if (testStatus > 0) {
        fprintf(stderr, "Error, non-zero test status = %d.\n", testStatus);
    }
    return testStatus;
}

// ----------------------------------------------------------------------------
// Copyright 2013 Bloomberg Finance L.P.
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
