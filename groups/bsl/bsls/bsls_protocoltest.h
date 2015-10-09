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
//  bsls::ProtocolTestImp: provides a framework for testing protocol classes
//  bsls::ProtocolTest: provides tests for protocol class concerns
//
//@MACROS:
//  BSLS_PROTOCOLTEST_ASSERT: macro for testing protocol methods
//
//@DESCRIPTION: This component provides classes and macros that simplify the
// creation of test drivers for protocol (i.e., pure abstract interface)
// classes.
//
// The purpose of a test driver for a protocol class is to verify concerns for
// that protocol's definition.  Although each protocol is different and
// requires its own test driver, there is a common set of concerns that apply
// to all protocol classes.  This component allows us to verify those concerns
// in a generic manner.
//
// Each protocol class has to satisfy the following set of requirements
// (concerns):
//: o The protocol is abstract: no objects of it can be created.
//: o The protocol has no data members.
//: o The protocol has a virtual destructor.
//: o All methods of the protocol are pure virtual.
//: o All methods of the protocol are publicly accessible.
//
// This protocol test component is intended to verify conformance to these
// requirements; however, it is not possible to verify all protocol
// requirements fully within the framework of the C++ language.  The following
// aspects of the above requirements are not verified by this component:
//: o Non-creator methods of the protocol are *pure* virtual.
//: o There are no methods in the protocol other than the ones being tested.
//
// Additionally some coding guidelines related to protocols are also not
// verified:
//: o The destructor is not pure virtual.
//: o The destructor is not implemented inline.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Testing a Protocol Class
///- - - - - - - - - - - - - - - - - -
// This example demonstrates how to test a protocol class, 'ProtocolClass',
// using this protocol test component.  Our 'ProtocolClass' provides two of
// pure virtual methods ('foo' and 'bar'), along with a virtual destructor:
//..
//  struct ProtocolClass {
//      virtual ~ProtocolClass();
//      virtual const char *bar(char const *, char const *) = 0;
//      virtual int foo(int) const = 0;
//  };
//
//  ProtocolClass::~ProtocolClass()
//  {
//  }
//..
// First, we define a test class derived from this protocol, and implement its
// virtual methods.  Rather than deriving the test class from 'ProtocolClass'
// directly, the test class is derived from
// 'bsls::ProtocolTestImp<ProtocolClass>' (which, in turn, is derived
// automatically from 'ProtocolClass').  This special base class implements
// boilerplate code and provides useful functionality for testing of protocols.
//..
//  // ========================================================================
//  //                  GLOBAL CLASSES/TYPEDEFS FOR TESTING
//  // ------------------------------------------------------------------------
//
//  struct ProtocolClassTestImp : bsls::ProtocolTestImp<ProtocolClass> {
//      const char *bar(char const *, char const *) { return markDone(); }
//      int foo(int) const                          { return markDone(); }
//  };
//..
// Notice that in 'ProtocolClassTestImp' we must provide an implementation for
// every protocol method except for the destructor.  The implementation of each
// method calls the (protected) 'markDone' which is provided by the base class
// for the purpose of verifying that the method from which it's called is
// declared as virtual in the protocol class.
//
// Then, in our protocol test case we describe the concerns we have for the
// protocol class and the plan to test those concerns:
//..
//  // ------------------------------------------------------------------------
//  // PROTOCOL TEST:
//  //   Ensure this class is a properly defined protocol.
//  //
//  // Concerns:
//  //: 1 The protocol is abstract: no objects of it can be created.
//  //:
//  //: 2 The protocol has no data members.
//  //:
//  //: 3 The protocol has a virtual destructor.
//  //:
//  //: 4 All methods of the protocol are pure virtual.
//  //:
//  //: 5 All methods of the protocol are publicly accessible.
//  //
//  // Plan:
//  //: 1 Define a concrete derived implementation, 'ProtocolClassTestImp',
//  //:   of the protocol.
//  //:
//  //: 2 Create an object of the 'bsls::ProtocolTest' class template
//  //:   parameterized by 'ProtocolClassTestImp', and use it to verify
//  //:   that:
//  //:
//  //:   1 The protocol is abstract. (C-1)
//  //:
//  //:   2 The protocol has no data members. (C-2)
//  //:
//  //:   3 The protocol has a virtual destructor. (C-3)
//  //:
//  //: 3 Use the 'BSLS_PROTOCOLTEST_ASSERT' macro to verify that
//  //:   non-creator methods of the protocol are:
//  //:
//  //:   1 virtual, (C-4)
//  //:
//  //:   2 publicly accessible. (C-5)
//  //
//  // Testing:
//  //   virtual ~ProtocolClass();
//  //   virtual const char *bar(char const *, char const *) = 0;
//  //   virtual int foo(int) const = 0;
//  // ------------------------------------------------------------------------
//..
// Next we print the banner for this test case:
//..
//  if (verbose) printf("\nPROTOCOL TEST"
//                      "\n=============\n");
//..
// Then, we create an object of type
// 'bsls::ProtocolTest<ProtocolClassTestImp>', 'testObj':
//..
//  if (verbose) printf("\nCreate a test object.\n");
//
//  bsls::ProtocolTest<ProtocolClassTestImp> testObj(veryVerbose);
//..
// Now we use the 'testObj' to test some general concerns about the protocol
// class.
//..
//  if (verbose) printf("\nVerify that the protocol is abstract.\n");
//
//  ASSERT(testObj.testAbstract());
//
//  if (verbose) printf("\nVerify that there are no data members.\n");
//
//  ASSERT(testObj.testNoDataMembers());
//
//  if (verbose) printf("\nVerify that the destructor is virtual.\n");
//
//  ASSERT(testObj.testVirtualDestructor());
//..
// Finally we use the 'testObj' to test concerns for each individual method of
// the protocol class.  To test a protocol method we need to call it from
// inside the 'BSLS_PROTOCOLTEST_ASSERT' macro, and also pass the 'testObj':
//..
//  if (verbose) printf("\nVerify that methods are public and virtual.\n");
//
//  BSLS_PROTOCOLTEST_ASSERT(testObj, foo(77));
//  BSLS_PROTOCOLTEST_ASSERT(testObj, bar("", ""));
//..
// These steps conclude the protocol testing.  If there are any failures, they
// will be reported via standard test driver assertions (i.e., the standard
// 'ASSERT' macro).

#ifndef INCLUDED_CSTDIO
#include <cstdio>
#define INCLUDED_CSTDIO
#endif

namespace BloombergLP {

namespace bsls {

                    // =============================
                    // class ProtocolTest_IsAbstract
                    // =============================

template <class T>
struct ProtocolTest_IsAbstract {
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

                 // ===================================
                 // class ProtocolTest_MethodReturnType
                 // ===================================

struct ProtocolTest_MethodReturnType {
    // This class is a proxy for a return type designed to simplify testing
    // implementations of protocol methods.
    // 'ProtocolTest_MethodReturnType' can be converted to any
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

                // ======================================
                // class ProtocolTest_MethodReturnRefType
                // ======================================

struct ProtocolTest_MethodReturnRefType {
    // This class is a proxy for a return type designed to simplify testing
    // implementations of protocol methods.
    // 'ProtocolTest_MethodReturnRefType' can be converted to any
    // reference type.  When an object of this class is returned from a test
    // implementation of a protocol method, it is implicitly converted to
    // the return type of the protocol method.

    // ACCESSORS
    template <class T>
    operator T&() const;
        // Return a 'T&' reference to an invalid object.  The returned value
        // cannot be used and should be immediately discarded.
};

                       // =======================
                       // class ProtocolTest_Dtor
                       // =======================

template <class BSLS_TESTIMP>
struct ProtocolTest_Dtor : BSLS_TESTIMP {
    // This class template is a helper protocol-test implementation class that
    // tests that a protocol destructor is declared 'virtual', which it does by
    // calling the 'markDone' function from its destructor.  The destructor
    // will be executed if the protocol's destructor is declared 'virtual' and
    // not executed otherwise.  Note that the 'BSLS_TESTIMP' template parameter
    // is required to be a type derived from 'ProtocolTestImp' class.

    // CREATORS
    ~ProtocolTest_Dtor();
        // Destroy this object and call the 'markDone' method, indicating that
        // the base class's destructor was declared 'virtual'.
};

                      // =========================
                      // class ProtocolTest_Status
                      // =========================

class ProtocolTest_Status {
    // This class keeps track of the test status, which includes the status of
    // the last test and the number of failures across all tests.

  private:
    // DATA
    int  d_failures;   // number of test failures encountered so far
    bool d_last;       // result of the last test ('true' indicates success)

  public:
    // CREATORS
    ProtocolTest_Status();
        // Create an object of the 'ProtocolTest_Status' class with the
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

                          // =====================
                          // class ProtocolTestImp
                          // =====================

template <class BSLS_PROTOCOL>
class ProtocolTestImp : public BSLS_PROTOCOL {
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
    ProtocolTest_Status      *d_status;   // test status object for test
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
    ProtocolTestImp();
        // Create an object of the 'ProtocolTestImp' class.

    ~ProtocolTestImp();
        // Destroy this object and check the status of the test execution
        // (success or failure).  On test failure, report it to
        // 'ProtocolTest_Status'.

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

    void setTestStatus(ProtocolTest_Status *testStatus);
        // Connect this protocol test object with the specified 'testStatus'
        // object, which will be used for test failure reporting.

    // ACCESSORS
    ProtocolTest_MethodReturnType markDone() const;
        // Return a proxy object convertible to any value or pointer type.
        // Derived classed should call this method from their implementations
        // of protocol virtual methods to indicate that virtual methods were
        // overridden correctly.

    ProtocolTest_MethodReturnRefType markDoneRef() const;
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

                       // ==================
                       // class ProtocolTest
                       // ==================

template <class BSLS_TESTIMP>
class ProtocolTest {
    // This mechanism class template provides the implementation of protocol
    // testing concerns via 'test*' methods (for non-method concerns), and via
    // 'operator->' (for method concerns).  The 'BSLS_TESTIMP' template
    // parameter is required to be a class derived from 'ProtocolTestImp'
    // that provides test implementations of all protocol methods.

  private:
    // TYPES
    typedef typename BSLS_TESTIMP::ProtocolType ProtocolType;

    // DATA
    ProtocolTest_Status d_status;
    bool                d_verbose;  // print trace messages if 'true'

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
    ProtocolTest(bool verbose = false);
        // Construct a 'ProtocolTest' object.

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

}  // close package namespace

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

namespace bsls {

                     // -----------------------------------
                     // class ProtocolTest_MethodReturnType
                     // -----------------------------------

// ACCESSORS
template <class T>
inline
ProtocolTest_MethodReturnType::operator T() const
{
    return T();
}

                   // --------------------------------------
                   // class ProtocolTest_MethodReturnRefType
                   // --------------------------------------

// ACCESSORS
template <class T>
inline
ProtocolTest_MethodReturnRefType::operator T&() const
{
    return *reinterpret_cast<T *>(0);
}

                       // -----------------------
                       // class ProtocolTest_Dtor
                       // -----------------------

// CREATORS
template <class BSLS_TESTIMP>
inline
ProtocolTest_Dtor<BSLS_TESTIMP>::~ProtocolTest_Dtor()
{
    this->markDone();
}

                      // -------------------------
                      // class ProtocolTest_Status
                      // -------------------------

// CREATORS
inline
ProtocolTest_Status::ProtocolTest_Status()
: d_failures(0)
, d_last(true)
{
}

// MANIPULATORS
inline
void ProtocolTest_Status::resetLast()
{
    d_last = true;
}

inline
void ProtocolTest_Status::fail()
{
    ++d_failures;
    d_last = false;
}

// ACCESSORS
inline
int ProtocolTest_Status::failures() const
{
    return d_failures;
}

inline
bool ProtocolTest_Status::last() const
{
    return d_last;
}

                          // ---------------------
                          // class ProtocolTestImp
                          // ---------------------

// CREATORS
template <class BSLS_PROTOCOL>
inline
ProtocolTestImp<BSLS_PROTOCOL>::ProtocolTestImp()
: d_status(0)
, d_entered(false)
, d_exited(false)
{
}

template <class BSLS_PROTOCOL>
inline
ProtocolTestImp<BSLS_PROTOCOL>::~ProtocolTestImp()
{
    if (d_entered && !d_exited) {
        d_status->fail();
    }
}

// MANIPULATORS
template <class BSLS_PROTOCOL>
inline
typename ProtocolTestImp<BSLS_PROTOCOL>::ProtocolType *
ProtocolTestImp<BSLS_PROTOCOL>::operator->()
{
    markEnter();
    return static_cast<BSLS_PROTOCOL *>(this);
}

template <class BSLS_PROTOCOL>
inline
void
ProtocolTestImp<BSLS_PROTOCOL>::setTestStatus(ProtocolTest_Status *testStatus)
{
    d_status = testStatus;
}

template <class BSLS_PROTOCOL>
inline
void ProtocolTestImp<BSLS_PROTOCOL>::markEnter()
{
    d_entered = true;
}

// ACCESSORS
template <class BSLS_PROTOCOL>
inline
ProtocolTest_MethodReturnType
ProtocolTestImp<BSLS_PROTOCOL>::markDone() const
{
    d_exited = true;
    return ProtocolTest_MethodReturnType();
}

template <class BSLS_PROTOCOL>
inline
ProtocolTest_MethodReturnRefType
ProtocolTestImp<BSLS_PROTOCOL>::markDoneRef() const
{
    d_exited = true;
    return ProtocolTest_MethodReturnRefType();
}

template <class BSLS_PROTOCOL>
template <class T>
inline
T ProtocolTestImp<BSLS_PROTOCOL>::markDoneVal(const T& value) const
{
    d_exited = true;
    return value;
}

                       // ------------------
                       // class ProtocolTest
                       // ------------------

// PRIVATE MANIPULATORS
template <class BSLS_TESTIMP>
inline
void ProtocolTest<BSLS_TESTIMP>::startTest()
{
    d_status.resetLast();
}

template <class BSLS_TESTIMP>
inline
void ProtocolTest<BSLS_TESTIMP>::trace(char const *message) const
{
    if (d_verbose) {
        std::printf("\t%s\n", message);
    }
}

// CREATORS
template <class BSLS_TESTIMP>
inline
ProtocolTest<BSLS_TESTIMP>::ProtocolTest(bool verbose)
: d_verbose(verbose)
{
}

// MANIPULATORS
template <class BSLS_TESTIMP>
inline
BSLS_TESTIMP ProtocolTest<BSLS_TESTIMP>::method(const char *methodDesc)
{
    trace(methodDesc);
    startTest();

    BSLS_TESTIMP impl;
    impl.setTestStatus(&d_status);
    return impl;
}

template <class BSLS_TESTIMP>
inline
bool ProtocolTest<BSLS_TESTIMP>::testAbstract()
{
    trace("inside ProtocolTest::testAbstract()");
    startTest();

    if (!ProtocolTest_IsAbstract<ProtocolType>::VALUE) {
        d_status.fail();
    }

    return lastStatus();
}

template <class BSLS_TESTIMP>
inline
bool ProtocolTest<BSLS_TESTIMP>::testNoDataMembers()
{
    trace("inside ProtocolTest::testNoDataMembers()");
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
bool ProtocolTest<BSLS_TESTIMP>::testVirtualDestructor()
{
    trace("inside ProtocolTest::testVirtualDestructor");
    startTest();

    // Can't use an automatic buffer and the placement new for an object of
    // type ProtocolTest_Dtor<BSLS_TESTIMP> here, because bslma::Allocator
    // defines its own placement new, making it impossible to test
    // bslma::Allocator protocol this way.

    // Prepare a test
    ProtocolTest_Dtor<BSLS_TESTIMP> * obj =
                                    new ProtocolTest_Dtor<BSLS_TESTIMP>();
    BSLS_TESTIMP * base = obj;
    obj->setTestStatus(&d_status);

    // Run the test.
    obj->markEnter();
    delete base;

    // 'ProtocolTest_Dtor::~ProtocolTest_Dtor' will be called only if
    // the destructor was declared 'virtual' in the interface, but
    // 'BSLS_TESTIMP::~BSLS_TESTIMP' is always executed to check if the
    // derived destructor was called.

    return lastStatus();
}

// ACCESSORS
template <class BSLS_TESTIMP>
inline
int ProtocolTest<BSLS_TESTIMP>::failures() const
{
    return d_status.failures();
}

template <class BSLS_TESTIMP>
inline
bool ProtocolTest<BSLS_TESTIMP>::lastStatus() const
{
    return d_status.last();
}

}  // close package namespace


}  // close enterprise namespace

#endif

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
