// bsls_protocoltest.h                                                -*-C++-*-
#ifndef INCLUDED_BSLS_PROTOCOLTEST
#define INCLUDED_BSLS_PROTOCOLTEST

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide classes and macros for testing abstract protocols.
//
//@CLASSES:
//  bsls_ProtocolTestImp: provides a framework for testing protocol classes
//  bsls_ProtocolTest: provides tests for protocol class concerns
//
//@MACROS:
//  BSLS_PROTOCOLTEST_ASSERT: macro for testing protocol methods
//
//@AUTHOR: Alexei Zakharov (azakharov7)
//
//@DESCRIPTION: This component provides classes and macros that simplify the
// creation of test drivers for protocol (i.e., pure abstract interface)
// classes.
//
// The purpose of a test driver for a protocol class is to verify concerns for
// that protocol class definition.  Although each protocol class is different
// and requires its own test driver, there is a set of concerns that apply to
// all protocol classes.  This component allows us to verify those concerns in
// a generic manner.
//
// Each protocol class has to conform to the following set of concerns:
//: o the protocol class is an abstract: no objects of it can be created
//: o the protocol has no data members
//: o the protocol has a virtual destructor
//: o all methods of the protocol are pure virtual
//: o all methods of the protocol are publicly accessible
//
// This protocol test component is intended to verify the conformance of a
// protocol class to these concerns.  It's not possible, however, to verify all
// protocol concerns fully within the framework of the C++ language.  The
// following aspects of the concerns are not verified by the protocol test
// component:
//: o non-creator methods of the protocol are *pure* virtual
//: o there are no methods in the protocol other than the ones being tested
//: o the desctructor is not implemented inline
//
///Usage
///-----
// In this section we show intended usage of this component.
//
///Example 1: Testing a simple protocol class.
///- - - - - - - - - - - - - - - - - - - - - -
// This example demonstrates how to test a protocol class, 'MyInterface', using
// this protocol test component.  The protocol we want to test, 'MyInterface',
// provides a couple of pure virtual methods ('foo' and 'bar'), along with a
// pure virtual destructor.
//..
//  struct MyInterface {
//      virtual ~MyInterface() = 0;
//      virtual const char *bar(char const *, char const *) = 0;
//      virtual int foo(int) const = 0;
//  };
//
//  MyInterface::~MyInterface()
//  {
//  }
//..
// First, we define a test class derived from this protocol, and implement its
// virtual methods.  Rather than deriving the test class from 'MyInterface'
// directly, the test class is derived from 'bsls_ProtocolTestImp<MyInterface>'
// (which, in turn, is derived from 'MyInterface').  This base class implements
// boilerplate code and provides useful functionality for testing of protocols.
//..
//  struct MyInterfaceTest : bsls_ProtocolTestImp<MyInterface> {
//      const char *bar(char const *, char const *) { return markDone(); }
//      int foo(int) const                          { return markDone(); }
//  };
//..
// Notice that in 'MyInterfaceTest' we must provide an implementation for every
// protocol method except for the destructor.  The implementation of each
// method should simply call 'markDone' which is provided by the base class for
// the purpose of verifying that the method from which it's called is declared
// as virtual in the protocol class.
//
// Then, in our protocol test case we describe the concerns we have for the
// protocol class and the plan to test those concerns:
//..
//  // --------------------------------------------------------------------
//  // PROTOCOL TEST:
//  //   Ensure this class is a properly defined protocol.
//  //
//  // Concerns:
//  //: 1 The protocol class is an abstract: no objects of it can be created.
//  //:
//  //: 2 The protocol has no data members.
//  //:
//  //: 3 The protocol has a pure virtual destructor.
//  //:
//  //: 4 All methods of the protocol are pure virtual.
//  //:
//  //: 5 All methods of the protocol are publicly accessible.
//  //
//  // Plan:
//  //: 1 Define a concrete derived implementation, 'MyInterfaceTest' of the
//  //:   protocol.
//  //:
//  //: 2 Create an object of the 'bsls_ProtocolTest' class parameterized
//  //:   with 'MyInterfaceTest'.
//  //:
//  //: 3 Use the 'bsls_protocolTest' object to verify that the protocol is
//  //:   an abstract class. (C-1)
//  //:
//  //: 4 Use the 'bsls_ProtocolTest' object to verify that the protocol
//  //:   has no data members. (C-2)
//  //:
//  //: 5 Use the 'bsls_ProtocolTest' object to verify that the protocol
//  //:   has a virtual destructor. (C-3)
//  //:
//  //: 6 Use the 'BSLS_PROTOCOLTEST_ASSERT' macro to verify that:
//  //:
//  //:   1 All the methods of the protocol class are virtual. (C-4)
//  //:
//  //:   2 All the methods of the protocol class are publicly accessible.
//  //:     (C-5)
//  //
//  // Testing:
//  //   virtual ~MyInterface() = 0;
//  //   virtual const char *bar(char const *, char const *) = 0;
//  //   virtual int foo(int) const = 0;
//  // --------------------------------------------------------------------
//..
// Next, we use 'bsls_ProtocolTest' to perform the actual testing of our
// 'MyInterface' protocol class.
//..
//  if (verbose) printf("\nPROTOCOL TEST"
//                      "\n=============\n");
//..
// Then we create an object of type 'bsls_ProtocolTest' parameterized with
// 'MyInterfaceTest':
//..
//  if (verbose) printf("\nCreate a 'bsls_ProtocolTest' object.\n");
//
//  bsls_ProtocolTest<MyInterfaceTest> t(veryVerbose);
//..
// Now we use the 't' object to test some general concerns about the protocol
// class.
//..
//  if (verbose) printf("\nVerify that the protocol is abstract.\n");
//
//  ASSERT(t.testAbstract());
//
//  if (verbose) printf("\nVerify that there are no data members.\n");
//
//  ASSERT(t.testNoDataMembers());
//
//  if (verbose) printf("\nVerify that the destructor is virtual.\n");
//
//  ASSERT(t.testVirtualDestructor());
//..
// Finally we use the 't' object to test concerns for each individual method of
// the protocol class.  To test a protocol method we need to call it from
// inside the 'BSLS_PROTOCOLTEST_ASSERT' macro, and also pass the 't' object:
//..
//  if (verbose) printf("\nVerify that methods are public and virtual.\n");
//
//  BSLS_PROTOCOLTEST_ASSERT(t, foo(77));
//  BSLS_PROTOCOLTEST_ASSERT(t, bar("", ""));
//..
// These steps conclude the protocol testing.  If there are any failures, they
// will be reported via standard test driver assertions (i.e., the standard
// 'ASSERT' macro).

#ifndef INCLUDED_CSTDIO
#define INCLUDED_CSTDIO
#include <cstdio>
#endif

namespace BloombergLP {

                    // ==================================
                    // class bsls_ProtocolTest_IsAbstract
                    // ==================================

template <class T>
struct bsls_ProtocolTest_IsAbstract {
    // This class template is a compile-time meta-function, parameterized with
    // type 'T', the output of which is 'VALUE', which will be 'true' if 'T' is
    // abstract and 'false' otherwise.  The 'IsAbstract' test makes use of the
    // fact that a type 'an array of objects of an abstract type' (e.g.,
    // 'U (*)[1]') cannot exist.  Note that it is only an approximation,
    // because this is also true for an incomplete type.  But this
    // approximation is good enough for the purpose of testing protocol
    // classes.

    typedef char                    YesType;
    typedef struct { char a[2]; }   NoType;

    template <class U>
    static NoType test(U (*)[1]);

    template <class U>
    static YesType test(...);

    enum { VALUE = sizeof(test<T>(0)) == sizeof(YesType) };
};

                 // ========================================
                 // class bsls_ProtocolTest_MethodReturnType
                 // ========================================

struct bsls_ProtocolTest_MethodReturnType {
    // This class is a proxy for a return type designed to simplify testing
    // implementations of protocol methods.
    // 'bsls_ProtocolTest_MethodReturnType' can be converted to any
    // non-reference type (i.e., the type can be either a value or pointer
    // type, but not a reference type).  When an object of this class is
    // returned from a test implementation of a protocol method, it is
    // implicitly converted to the return type of the protocol method.

    // ACCESSORS
    template <class T>
    operator T() const;
        // Return a temporary value of type 'T'.  The returned object is valid
        // but it does not have any meaningful value so it should not be used.
        // Type 'T' is required to be default-constructible.
};

                // ===========================================
                // class bsls_ProtocolTest_MethodReturnRefType
                // ===========================================

struct bsls_ProtocolTest_MethodReturnRefType {
    // This class is a proxy for a return type designed to simplify testing
    // implementations of protocol methods.
    // 'bsls_ProtocolTest_MethodReturnRefType' can be converted to any
    // reference type.  When an object of this class is returned from a test
    // implementation of a protocol method, it is implicitly converted to
    // the return type of the protocol method.

    // ACCESSORS
    template <class T>
    operator T&() const;
        // Return a 'T&' reference to an invalid object.  The returned value
        // cannot be used and should be immediately discarded.
};

                       // ============================
                       // class bsls_ProtocolTest_Dtor
                       // ============================

template <class BSLS_TESTIMP>
struct bsls_ProtocolTest_Dtor : BSLS_TESTIMP {
    // This class template is a helper protocol-test implementation class that
    // tests that a protocol destructor is declared 'virtual', which it does by
    // calling the 'markDone' function from its destructor.  The destructor
    // will be executed if the protocol's destructor is declared 'virtual' and
    // not executed otherwise.  Note that the 'BSLS_TESTIMP' template parameter
    // is required to be a type derived from 'bsls_ProtocolTestImp' class.

    // CREATORS
    ~bsls_ProtocolTest_Dtor();
        // Destroy this object and call the 'markDone' method, indicating that
        // the base class's destructor was declared 'virtual'.
};

                      // ==============================
                      // class bsls_ProtocolTest_Status
                      // ==============================

class bsls_ProtocolTest_Status {
    // This class keeps track of the test status, which includes the status of
    // the last test and the number of failures across all tests.

  private:
    // DATA
    int  d_failures;   // number of test failures encountered so far
    bool d_last;       // result of the last test ('true' indicates success)

  public:
    // CREATORS
    bsls_ProtocolTest_Status();
        // Create an object of the 'bsls_ProtocolTest_Status' class with the
        // default state in which 'failures() == 0' and 'last() == true'.

    // MANIPULATORS
    void resetLast();
        // Reset the status of the last test to 'true'.

    void fail();
        // Record a test failure by increasing the number of 'failures' and
        // setting the status of the last test to 'false'.

    // ACCESSORS
    int failures() const;
        // Return the number of failures encountered during testing of a
        // protocol class, which is 0 if all tests succeeded or if no tests
        // ran.

    bool last() const;
        // Return 'true' if the last test completed successfully (or no test
        // has yet completed), and 'false' if it failed.
};

                          // ==========================
                          // class bsls_ProtocolTestImp
                          // ==========================

template <class BSLS_PROTOCOL>
class bsls_ProtocolTestImp : public BSLS_PROTOCOL {
    // This mechanism class template is a base class for a test implementation
    // of a protocol class defined by the 'BSLS_PROTOCOL' template parameter.
    // Its purpose is to reduce the boilerplate test code required to verify
    // that derived virtual methods are called.  It provides 'markDone' member
    // functions one of which should be called from each method of the protocol
    // class test implementation to indicate that the virtual method is
    // correctly overridden.  It also overloads 'operator->' to serve as a
    // proxy to 'BSLS_PROTOCOL' and detect when 'BSLS_PROTOCOL' methods are
    // called.

  private:
    // DATA
    bsls_ProtocolTest_Status *d_status;   // test status object for test
                                          // failure reporting

    bool                      d_entered;  // 'true' if this object entered a
                                          // protocol method call

    bool mutable              d_exited;   // 'true' if this object exited a
                                          // protocol method in the derived
                                          // class; mutable, so it can be set
                                          // from 'const' methods
  public:
    // TYPES
    typedef BSLS_PROTOCOL ProtocolType;

    // CREATORS
    bsls_ProtocolTestImp();
        // Create an object of the 'bsls_ProtocolTestImp' class.

    ~bsls_ProtocolTestImp();
        // Destroy this object and check the status of the test execution
        // (success or failure).  On test failure, report it to
        // 'bsls_ProtocolTest_Status'.

    // MANIPULATORS
    BSLS_PROTOCOL *operator->();
        // Dereference this object as if it were a pointer to 'BSLS_PROTOCOL'
        // in order to call a method on 'BSLS_PROTOCOL'.  Also mark this
        // object as 'entered' for the purpose of calling a protocol method.

    void markEnter();
        // Mark this object as entered for the purpose of calling a protocol
        // method.  The 'entered' property is tested in the destructor to
        // check for test failures (i.e., if 'entered == false' then the test
        // cannot fail since it never ran).  Note that 'markEnter' and
        // 'markDone' calls have to be paired for a protocol-method-call test
        // to succeed.

    void setTestStatus(bsls_ProtocolTest_Status *testStatus);
        // Connect this protocol test object with the specified 'testStatus'
        // object, which will be used for test failure reporting.

    // ACCESSORS
    bsls_ProtocolTest_MethodReturnType markDone() const;
        // Return a proxy object convertible to any value or pointer type.
        // Derived classed should call this method from their implementations
        // of protocol virtual methods to indicate that virtual methods were
        // overridden correctly.

    bsls_ProtocolTest_MethodReturnRefType markDoneRef() const;
        // Return a proxy object convertible to any reference type.  Derived
        // classed should call this method from their implementations of
        // protocol virtual methods to indicate that virtual methods were
        // overridden correctly.

    template <class T>
    T markDoneVal(const T& value) const;
        // Return the specified 'value'.  Derived classes should call this
        // method from their implementations of protocol virtual methods to
        // indicate that virtual methods were overridden correctly.
};

                       // =======================
                       // class bsls_ProtocolTest
                       // =======================

template <class BSLS_TESTIMP>
class bsls_ProtocolTest {
    // This mechanism class template provides the implementation of protocol
    // testing concerns via 'test*' methods (for non-method concerns), and via
    // 'operator->' (for method concerns).  The 'BSLS_TESTIMP' template
    // parameter is required to be a class derived from 'bsls_ProtocolTestImp'
    // that provides test implementations of all protocol methods.

  private:
    // TYPES
    typedef typename BSLS_TESTIMP::ProtocolType ProtocolType;

    // DATA
    bsls_ProtocolTest_Status d_status;
    bool                     d_verbose;  // print trace messages if 'true'

  private:
    // PRIVATE MANIPULATORS
    void startTest();
        // Start a new test by resetting this object to the state before the
        // test.

    void trace(char const *message) const;
        // Print a trace 'message' if 'd_verbose' is 'true'.

  public:
    // CREATORS
    explicit
    bsls_ProtocolTest(bool verbose = false);
        // Construct a 'bsls_ProtocolTest' object.

    // MANIPULATORS
    BSLS_TESTIMP method(const char *methodDesc = "");
        // Return a 'BSLS_TESTIMP' object to perform testing of a specific
        // method which gets called via 'operator->()'.  Note that
        // 'BSLS_TESTIMP' is a proxy to the actual protocol class.

    bool testAbstract();
        // Return 'true' (i.e., the test passed) if the protocol class being
        // tested is abstract and return 'false' (i.e., the test failed)
        // otherwise.  Increase the count of 'failures' and set 'lastStatus' to
        // 'false' on failure.

    bool testNoDataMembers();
        // Return 'true' (i.e., the test passed) if the protocol class being
        // tested contains no data fields and return 'false' (i.e., the test
        // failed) otherwise.  Increase the count of 'failures' and set
        // 'lastStatus' to 'false' on failure.

    bool testVirtualDestructor();
        // Return 'true' (i.e., the test passed) if the protocol class being
        // tested has a virtual destructor and return 'false' (i.e., the test
        // failed) otherwise.  Increase the 'failures' count and set
        // 'lastStatus' to 'false' on failure.

    // ACCESSORS
    int failures() const;
        // Return the number of failures encountered during testing of a
        // protocol class.  The returned value is 0 if all tests succeeded, or
        // no tests ran.

    bool lastStatus() const;
        // Return 'true' if the last test completed successfully (or no test
        // has yes completed) and 'false' otherwise.
};

                         // ========================
                         // BSLS_PROTOCOLTEST_ASSERT
                         // ========================

// This macro provides a test for method-related concerns of a protocol class.
// It ensures that a method is publicly accessible and declared 'virtual'.  It
// requires that a standard test driver 'ASSERT' macro is defined, which is
// used to assert the test completion status.

#define BSLS_PROTOCOLTEST_ASSERT(test, methodCall)                            \
    do {                                                                      \
        test.method(                                                          \
                "inside BSLS_PROTOCOLTEST_ASSERT("#methodCall")")->methodCall;\
        if (!test.lastStatus()) {                                             \
            ASSERT(0 && "Not a virtual method: "#methodCall);                 \
        }                                                                     \
    } while (0)

// ============================================================================
//                        INLINE FUNCTION DEFINITIONS
// ============================================================================

                 // ----------------------------------------
                 // class bsls_ProtocolTest_MethodReturnType
                 // ----------------------------------------

// ACCESSORS
template <class T>
inline
bsls_ProtocolTest_MethodReturnType::operator T() const
{
    return T();
}

                // -------------------------------------------
                // class bsls_ProtocolTest_MethodReturnRefType
                // -------------------------------------------

// ACCESSORS
template <class T>
inline
bsls_ProtocolTest_MethodReturnRefType::operator T&() const
{
    return *reinterpret_cast<T *>(0);
}

                       // ----------------------------
                       // class bsls_ProtocolTest_Dtor
                       // ----------------------------

// CREATORS
template <class BSLS_TESTIMP>
inline
bsls_ProtocolTest_Dtor<BSLS_TESTIMP>::~bsls_ProtocolTest_Dtor()
{
    this->markDone();
}

                      // ------------------------------
                      // class bsls_ProtocolTest_Status
                      // ------------------------------

// CREATORS
inline
bsls_ProtocolTest_Status::bsls_ProtocolTest_Status()
: d_failures(0)
, d_last(true)
{
}

// MANIPULATORS
inline
void bsls_ProtocolTest_Status::resetLast()
{
    d_last = true;
}

inline
void bsls_ProtocolTest_Status::fail()
{
    ++d_failures;
    d_last = false;
}

// ACCESSORS
inline
int bsls_ProtocolTest_Status::failures() const
{
    return d_failures;
}

inline
bool bsls_ProtocolTest_Status::last() const
{
    return d_last;
}

                          // --------------------------
                          // class bsls_ProtocolTestImp
                          // --------------------------

// CREATORS
template <class BSLS_PROTOCOL>
inline
bsls_ProtocolTestImp<BSLS_PROTOCOL>::bsls_ProtocolTestImp()
: d_status(0)
, d_entered(false)
, d_exited(false)
{
}

template <class BSLS_PROTOCOL>
inline
bsls_ProtocolTestImp<BSLS_PROTOCOL>::~bsls_ProtocolTestImp()
{
    if (d_entered && !d_exited) {
        d_status->fail();
    }
}

// MANIPULATORS
template <class BSLS_PROTOCOL>
inline
typename bsls_ProtocolTestImp<BSLS_PROTOCOL>::ProtocolType *
bsls_ProtocolTestImp<BSLS_PROTOCOL>::operator->()
{
    markEnter();
    return static_cast<BSLS_PROTOCOL *>(this);
}

template <class BSLS_PROTOCOL>
inline
void bsls_ProtocolTestImp<BSLS_PROTOCOL>::setTestStatus(
                                              bsls_ProtocolTest_Status *status)
{
    d_status = status;
}

template <class BSLS_PROTOCOL>
inline
void bsls_ProtocolTestImp<BSLS_PROTOCOL>::markEnter()
{
    d_entered = true;
}

// ACCESSORS
template <class BSLS_PROTOCOL>
inline
bsls_ProtocolTest_MethodReturnType
bsls_ProtocolTestImp<BSLS_PROTOCOL>::markDone() const
{
    d_exited = true;
    return bsls_ProtocolTest_MethodReturnType();
}

template <class BSLS_PROTOCOL>
inline
bsls_ProtocolTest_MethodReturnRefType
bsls_ProtocolTestImp<BSLS_PROTOCOL>::markDoneRef() const
{
    d_exited = true;
    return bsls_ProtocolTest_MethodReturnRefType();
}

template <class BSLS_PROTOCOL>
template <class T>
inline
T bsls_ProtocolTestImp<BSLS_PROTOCOL>::markDoneVal(const T& value) const
{
    d_exited = true;
    return value;
}

                       // -----------------------
                       // class bsls_ProtocolTest
                       // -----------------------

// PRIVATE MANIPULATORS
template <class BSLS_TESTIMP>
inline
void bsls_ProtocolTest<BSLS_TESTIMP>::startTest()
{
    d_status.resetLast();
}

template <class BSLS_TESTIMP>
inline
void bsls_ProtocolTest<BSLS_TESTIMP>::trace(char const *message) const
{
    if (d_verbose) {
        std::printf("\t%s\n", message);
    }
}

// CREATORS
template <class BSLS_TESTIMP>
inline
bsls_ProtocolTest<BSLS_TESTIMP>::bsls_ProtocolTest(bool verbose)
: d_verbose(verbose)
{
}

// MANIPULATORS
template <class BSLS_TESTIMP>
inline
BSLS_TESTIMP bsls_ProtocolTest<BSLS_TESTIMP>::method(const char *methodDesc)
{
    trace(methodDesc);
    startTest();

    BSLS_TESTIMP impl;
    impl.setTestStatus(&d_status);
    return impl;
}

template <class BSLS_TESTIMP>
inline
bool bsls_ProtocolTest<BSLS_TESTIMP>::testAbstract()
{
    trace("inside bsls_ProtocolTest::testAbstract()");
    startTest();

    if (!bsls_ProtocolTest_IsAbstract<ProtocolType>::VALUE) {
        d_status.fail();
    }

    return lastStatus();
}

template <class BSLS_TESTIMP>
inline
bool bsls_ProtocolTest<BSLS_TESTIMP>::testNoDataMembers()
{
    trace("inside bsls_ProtocolTest::testNoDataMembers()");
    struct EmptyProtocol
    {
        virtual ~EmptyProtocol() {}
    };

    startTest();

    if (sizeof(EmptyProtocol) != sizeof(ProtocolType)) {
        d_status.fail();
    }

    return lastStatus();
}

template <class BSLS_TESTIMP>
bool bsls_ProtocolTest<BSLS_TESTIMP>::testVirtualDestructor()
{
    trace("inside bsls_ProtocolTest::testVirtualDestructor");
    startTest();

    // Can't use an automatic buffer and the placement new for an object of
    // type bsls_ProtocolTest_Dtor<BSLS_TESTIMP> here, because bslma_Allocator
    // defines its own placement new, making it impossible to test
    // bslma_Allocator protocol this way.

    // Prepare a test
    bsls_ProtocolTest_Dtor<BSLS_TESTIMP> * obj =
                                    new bsls_ProtocolTest_Dtor<BSLS_TESTIMP>();
    BSLS_TESTIMP * base = obj;
    obj->setTestStatus(&d_status);

    // Run the test.
    obj->markEnter();
    delete base;

    // 'bsls_ProtocolTest_Dtor::~bsls_ProtocolTest_Dtor' will be called only if
    // the destructor was declared 'virtual' in the interface, but
    // 'BSLS_TESTIMP::~BSLS_TESTIMP' is always executed to check if the
    // derived destructor was called.

    return lastStatus();
}

// ACCESSORS
template <class BSLS_TESTIMP>
inline
int bsls_ProtocolTest<BSLS_TESTIMP>::failures() const
{
    return d_status.failures();
}

template <class BSLS_TESTIMP>
inline
bool bsls_ProtocolTest<BSLS_TESTIMP>::lastStatus() const
{
    return d_status.last();
}

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2011
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
