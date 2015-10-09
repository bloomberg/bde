// ball_recordbuffer.t.cpp                                            -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <ball_recordbuffer.h>

#include <ball_severity.h>          // for testing only

#include <bslmt_lockguard.h>
#include <bslmt_recursivemutex.h>

#include <bslim_testutil.h>
#include <bdlma_deleter.h>
#include <bslma_testallocator.h>
#include <bsls_platform.h>
#include <bsls_protocoltest.h>

#include <bsl_iostream.h>
#include <bsl_new.h>                // placement 'new' syntax
#include <bsl_vector.h>

#include <bsl_cstdlib.h>                  // atoi()

// Note: on Windows -> WinGDI.h:#define ERROR 0
#if defined(BSLS_PLATFORM_CMP_MSVC) && defined(ERROR)
#undef ERROR
#endif

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

//=============================================================================
//                              TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// We are testing a pure protocol class.  We need to verify that (1) a
// concrete derived class compiles and links, and (2) usage examples must
// compile, link and execute on all the platforms.
//-----------------------------------------------------------------------------
// [ 1] virtual ~my_RecordBuffer();
// [ 1] virtual void beginSequence() = 0;
// [ 1] virtual void endSequence() = 0;
// [ 1] virtual void popBack() = 0;
// [ 1] virtual void popFront() = 0;
// [ 1] virtual int pushBack(const bsl::shared_ptr<ball::Record>&) = 0;
// [ 1] virtual int pushFront(const bsl::shared_ptr<ball::Record>&)= 0;
// [ 1] virtual void removeAll() = 0;
// [ 1] virtual const bsl::shared_ptr<const ball::Record>& back() const = 0;
// [ 1] virtual const bsl::shared_ptr<const ball::Record>& front() const = 0;
// [ 1] virtual int length() const = 0;
//-----------------------------------------------------------------------------
// [ 1] PROTOCOL TEST - Make sure derived class compiles and links.
// [ 2] TESTING USAGE EXAMPLE 1 and 2

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
//                  NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)
#define ASSERT_PASS(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
#define ASSERT_FAIL(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)
#define ASSERT_OPT_PASS(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS(EXPR)
#define ASSERT_OPT_FAIL(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL(EXPR)

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------
static int verbose;
static int veryVerbose;
static int veryVeryVerbose;

namespace BloombergLP {

namespace ball {

class RecordAttributes {
    int d_severity;

  public:
    RecordAttributes(int severity) : d_severity(severity) {}
    int severity() const { return d_severity; }
};

class Record {
    // This 'class' is a trivial implementation of the 'Record' type that is
    // used (*in* *name* *only*) in the protocol under test.

    // DATA
    RecordAttributes d_fields;

  public:
    // CREATORS
    Record(): d_fields(Severity::e_TRACE) {}
    Record(Severity::Level level) : d_fields(level) {}
    Record(const Record& original) : d_fields(original.d_fields) {}
    ~Record() {}

    // MANIPULATORS
    Record& operator=(const Record& rhs) {
        d_fields = rhs.d_fields;
        return *this;
    }

    // ACCESSORS
    const RecordAttributes& fixedFields() const { return d_fields; }
};
}  // close package namespace

// FREE OPERATORS
namespace ball {
bsl::ostream& operator<<(bsl::ostream& stream, const Record& rhs)
{
    stream << rhs.fixedFields().severity();
    return stream;
}
}  // close package namespace

}  // close enterprise namespace

typedef bsl::shared_ptr<ball::Record> Handle;

class my_DummyDeleter : public bdlma::Deleter<ball::Record> {
    // A dummy deleter that does nothing in 'deleteObject'.

  public:
    // CREATORS
    my_DummyDeleter();
    ~my_DummyDeleter();

    // MANIPULATORS
    void deleteObject(ball::Record *record);

};

// CREATORS
my_DummyDeleter::my_DummyDeleter()
{
}

my_DummyDeleter::~my_DummyDeleter()
{
}

// MANIPULATORS
void my_DummyDeleter::deleteObject(ball::Record *)
{
}

//=============================================================================
//                             USAGE EXAMPLE 1 AND 2
//-----------------------------------------------------------------------------

///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Defining a Concrete 'RecordBuffer' Type
///- - - - - - - - - - - - - - - - - -- - - - - - - -
// This example shows the definition of a simple concrete record buffer.  The
// requisite steps are:
//
//: 1 Define a concrete class derived from 'ball::RecordBuffer'.
//: 2 Implement all pure virtual functions.
//
// The concrete thread-safe 'my_RecordBuffer' class in this example implements
// the 'ball::RecordBuffer' protocol by delegating to an instance of
// 'bsl::vector<bsl::shared_ptr<ball::Record> > ':
//..
// my_recordbuffer.h
//
    class my_RecordBuffer : public ball::RecordBuffer {
        // This class provides a thread-safe implementation of the
        // 'ball::RecordBuffer' protocol.  This implementation employs a
        // vector to hold an unlimited number of record handles.
//
        mutable bslmt::RecursiveMutex d_mutex; // thread safety provider (see
                                // the implementation notes for the
                                // justification for using recursive mutex
                                // rather a plain mutex)
//
        bsl::vector<bsl::shared_ptr<ball::Record> >
                           d_buffer; // buffer of record handles
//
      private:
        // NOT IMPLEMENTED
        my_RecordBuffer(const my_RecordBuffer&);
        my_RecordBuffer& operator=(const my_RecordBuffer&);
//
      public:
        // CREATORS
        my_RecordBuffer();
        virtual ~my_RecordBuffer();

        // MANIPULATORS
        virtual void beginSequence();
        virtual void endSequence();
        virtual void popBack();
        virtual void popFront();
        virtual int pushBack(const bsl::shared_ptr<ball::Record>& handle);
        virtual int pushFront(
                     const bsl::shared_ptr<ball::Record>& handle);
        virtual void removeAll();

        // ACCESSORS
        virtual const bsl::shared_ptr<ball::Record>& back() const;
        virtual const bsl::shared_ptr<ball::Record>& front() const;
        virtual int length() const;
    };
//..
//
///Implementation Notes
///- - - - - - - - - - -
// Recursive mutex (rather than plain mutex) is chosen to provide thread
// safety.  This allows the manipulation of the record buffer between the call
// to 'beginSequence' and 'endSequence'.  If we had used plain mutex, calling
// any method on the record buffer between the calls to 'beginSequence' and
// 'endSequence' would result in a deadlock.
//..
    // CREATORS
    inline
    my_RecordBuffer::my_RecordBuffer() { }
//
    // MANIPULATORS
    inline
    void my_RecordBuffer::beginSequence()
    {
        d_mutex.lock();
    }
//
    inline
    void my_RecordBuffer::endSequence()
    {
        d_mutex.unlock();
    }
//
    inline
    void my_RecordBuffer::popBack()
    {
        bslmt::LockGuard<bslmt::RecursiveMutex> guard(&d_mutex);
        d_buffer.pop_back();
    }
//
    inline
    void my_RecordBuffer::popFront()
    {
        bslmt::LockGuard<bslmt::RecursiveMutex> guard(&d_mutex);
        d_buffer.erase(d_buffer.begin());
    }
//
    inline
    int my_RecordBuffer::pushBack(
                         const bsl::shared_ptr<ball::Record>& handle)
    {
        bslmt::LockGuard<bslmt::RecursiveMutex> guard(&d_mutex);
        d_buffer.push_back(handle);
        return 0;
    }
//
    inline
    int my_RecordBuffer::pushFront(
                         const bsl::shared_ptr<ball::Record>& handle)
    {
        bslmt::LockGuard<bslmt::RecursiveMutex> guard(&d_mutex);
        d_buffer.insert(d_buffer.begin(), handle);
        return 0;
    }
//
    inline
    void my_RecordBuffer::removeAll()
    {
        bslmt::LockGuard<bslmt::RecursiveMutex> guard(&d_mutex);
        d_buffer.clear();
    }
//
    // ACCESSORS
    inline
    const bsl::shared_ptr<ball::Record>& my_RecordBuffer::back() const
    {
        bslmt::LockGuard<bslmt::RecursiveMutex> guard(&d_mutex);
        return d_buffer.back();
    }
//
    inline
    const bsl::shared_ptr<ball::Record>& my_RecordBuffer::front() const
    {
        bslmt::LockGuard<bslmt::RecursiveMutex> guard(&d_mutex);
        return d_buffer.front();
    }
//
    inline
    int my_RecordBuffer::length() const
    {
        bslmt::LockGuard<bslmt::RecursiveMutex> guard(&d_mutex);
        return d_buffer.size();
    }
//..
// Note that we always implement a virtual destructor (non-inline) in the .cpp
// file (to indicate the *unique* location of the class's virtual table):
//..
    // my_recordbuffer.cpp
//
    my_RecordBuffer::~my_RecordBuffer() { }
//..
//
///Example 2: Using a 'ball::RecordBuffer'
///- - - - - - - - - - - - - - - - - - - -
// This example demonstrates working with the 'ball::RecordBuffer' protocol.
// We implement a function 'processRecord' that processes a specified record
// handle based on its severity.
//..
    void processRecord(const bsl::shared_ptr<ball::Record>& handle,
                             ball::RecordBuffer&            buffer)
        // Process the specified 'handle', based on it's severity.  Records
        // (encapsulated in 'handle') with severity equal to or *greater*
        // severe than (i.e., with *numerical* value *less* than or equal to)
        // 'ball::Severity::e_WARN' are pushed back into the specified
        // 'buffer'.  Records with a severity equal to or more severe than
        // 'ball::Severity::e_ERROR' are (in addition to get pushed back
        // into the 'buffer') printed to 'stdout', and then each record
        // contained in 'buffer' is in turn printed to 'stdout' and then
        // removed from 'buffer'.  That is, any severity level equal to or
        // more severe than 'ball::Severity::e_ERROR' triggers a trace-back
        // of all records accumulated in the buffer and flushes the buffer.
        // The function is thread safe and thus allows multiple concurrent
        // invocations of this function with the same record buffer.
    {
        int severity = handle->fixedFields().severity();
//
        if (ball::Severity::e_WARN >= severity) {
            buffer.pushBack(handle);
        }
        if (ball::Severity::e_ERROR >= severity) {
            bsl::cout << *handle;
            buffer.beginSequence(); // lock the buffer before traversing
            int length = buffer.length();
            while (length--) {
                bsl::cout << buffer.back();
                buffer.popBack();
            }
            buffer.endSequence();   // unlock the buffer after traversing
        }
//
    }
//..

//=============================================================================
//                       CONCRETE DERIVED TYPES
//-----------------------------------------------------------------------------

struct RecordBufferTest : bsls::ProtocolTestImp<ball::RecordBuffer> {
    void popBack()                                     { markDone(); }
    void popFront()                                    { markDone(); }
    int pushBack(const bsl::shared_ptr<ball::Record>&)  { return markDone(); }
    int pushFront(const bsl::shared_ptr<ball::Record>&) { return markDone(); }
    void removeAll()                                   { markDone(); }
    void beginSequence()                               { markDone(); }
    void endSequence()                                 { markDone(); }
    const bsl::shared_ptr<ball::Record>& back() const
                                                      { return markDoneRef(); }
    const bsl::shared_ptr<ball::Record>& front() const
                                                      { return markDoneRef(); }
    int length() const                                { return markDone(); }
};

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    verbose = argc > 2;
    veryVerbose = argc > 3;
    veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;;

    switch (test) { case 0:  // Zero is always the leading case.
      case 2: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE 1 and 2
        //
        // Concerns:
        //   The usage examples provided in the component header file must
        //   compile, link, and run on all platforms as shown.
        //
        // Plan:
        //   Incorporate usage examples from header into driver, remove
        //   leading comment characters, and replace 'assert' with 'ASSERT'.
        //
        // Testing:
        //   USAGE EXAMPLES
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "Testing Usage Example 1 and 2" << endl
                                  << "=============================" << endl;

        bslma::TestAllocator ta;

        my_DummyDeleter deleter;
        my_RecordBuffer buffer;

        ball::Record record1(ball::Severity::e_FATAL);
        bsl::shared_ptr<ball::Record> handle1(&record1, &deleter, &ta);
        processRecord(handle1, buffer);

        ball::Record record2(ball::Severity::e_TRACE);
        bsl::shared_ptr<ball::Record> handle2(&record2, &deleter, &ta);
        processRecord(handle2, buffer);

        ball::Record record3(ball::Severity::e_WARN);
        bsl::shared_ptr<ball::Record> handle3(&record3, &deleter, &ta);
        processRecord(handle3, buffer);

        ball::Record record4(ball::Severity::e_ERROR);
        bsl::shared_ptr<ball::Record> handle4(&record4, &deleter, &ta);
        processRecord(handle4, buffer);

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // PROTOCOL TEST:
        //
        // Concerns:
        //   We must ensure that (1) a subclass of the 'ball::RecordBuffer'
        //   class compiles and links when all virtual functions are defined,
        //   and (2) the functions are in fact virtual.
        //
        // Plan:
        //   Construct an object of a class derived from 'ball::RecordBuffer'
        //   and bind a 'ball::RecordBuffer' reference to the object.  Using
        //   the base class reference, invoke the 'beginSequence',
        //   'endSequence', 'popBack', 'popFront', 'pushBack', 'pushFront',
        //   'removeAll', 'back', 'front' and 'length' methods.  Verify that
        //   the correct implementations of the methods are called.
        //
        // Testing:
        //   virtual ~my_RecordBuffer();
        //   virtual void beginSequence() = 0;
        //   virtual void endSequence() = 0;
        //   virtual void popBack() = 0;
        //   virtual void popFront() = 0;
        //   virtual int pushBack(const bsl::shared_ptr<ball::Record>&) = 0;
        //   virtual int pushFront(const bsl::shared_ptr<ball::Record>&)= 0;
        //   virtual void removeAll() = 0;
        //   virtual const ball::Record& back() const = 0;
        //   virtual const ball::Record& front() const = 0;
        //   virtual int length() const = 0;
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "PROTOCOL TEST" << endl
                                  << "=============" << endl;

        bsls::ProtocolTest<RecordBufferTest> t(veryVerbose);

        ASSERT(t.testAbstract());
        ASSERT(t.testNoDataMembers());
        ASSERT(t.testVirtualDestructor());

        BSLS_PROTOCOLTEST_ASSERT(t, popBack());
        BSLS_PROTOCOLTEST_ASSERT(t, popFront());
        BSLS_PROTOCOLTEST_ASSERT(t, pushBack(bsl::shared_ptr<ball::Record>()));
        BSLS_PROTOCOLTEST_ASSERT(t,
                                 pushFront(bsl::shared_ptr<ball::Record>()));
        BSLS_PROTOCOLTEST_ASSERT(t, removeAll());
        BSLS_PROTOCOLTEST_ASSERT(t, beginSequence());
        BSLS_PROTOCOLTEST_ASSERT(t, endSequence());
        BSLS_PROTOCOLTEST_ASSERT(t, back());
        BSLS_PROTOCOLTEST_ASSERT(t, front());
        BSLS_PROTOCOLTEST_ASSERT(t, length());
      } break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        cerr << "Error, non-zero test status = " << testStatus << "." << endl;
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
