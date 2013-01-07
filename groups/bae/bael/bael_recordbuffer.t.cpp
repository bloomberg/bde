// bael_recordbuffer.t.cpp         -*-C++-*-
#include <bael_recordbuffer.h>

#include <bael_severity.h>          // for testing only

#include <bcema_deleter.h>          // for testing only
#include <bcema_testallocator.h>    // for testing only

#include <bcemt_lockguard.h>        // for testing only
#include <bcemt_thread.h>           // for testing only

#include <bsls_platform.h>          // for testing only
#include <bsls_protocoltest.h>      // for testing only

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
// [ 1] virtual int pushBack(const bcema_SharedPtr<bael_Record>&) = 0;
// [ 1] virtual int pushFront(const bcema_SharedPtr<bael_Record>&)= 0;
// [ 1] virtual void removeAll() = 0;
// [ 1] virtual const bcema_SharedPtr<const bael_Record>& back() const = 0;
// [ 1] virtual const bcema_SharedPtr<const bael_Record>& front() const = 0;
// [ 1] virtual int length() const = 0;
//-----------------------------------------------------------------------------
// [ 1] PROTOCOL TEST - Make sure derived class compiles and links.
// [ 2] TESTING USAGE EXAMPLE 1 and 2

//=============================================================================
//                      STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
static int testStatus = 0;

void aSsErT(int c, const char *s, int i)
{
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (0 <= testStatus && testStatus <= 100) ++testStatus;
    }
}

#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

//=============================================================================
//                  STANDARD BDE LOOP-ASSERT TEST MACROS
//-----------------------------------------------------------------------------
#define LOOP_ASSERT(I,X) { \
   if (!(X)) { cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__); }}

#define LOOP2_ASSERT(I,J,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " \
              << J << "\n"; aSsErT(1, #X, __LINE__); } }

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", "<< flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number
#define T_()  cout << "\t" << flush;          // Print tab w/o newline

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------
static int verbose;
static int veryVerbose;
static int veryVeryVerbose;

namespace BloombergLP {

class bael_Record {
    // This 'class' is a trivial implementation of the 'bael_Record' type
    // that is used (*in* *name* *only*) in the protocol under test.

    // DATA
    int d_level;

  public:
    // CREATORS
    bael_Record();
    bael_Record(bael_Severity::Level d_level);
    bael_Record(const bael_Record& original);
    ~bael_Record();

    // MANIPULATORS
    bael_Record& operator=(const bael_Record& rhs);

    // ACCESSORS
    int severity() const;
};

// CREATORS
bael_Record::bael_Record()
: d_level((int)bael_Severity::BAEL_TRACE)
{
}

bael_Record::bael_Record(bael_Severity::Level level)
: d_level((int)level)
{
}

bael_Record::bael_Record(const bael_Record& original)
: d_level(original.d_level)
{
}

bael_Record::~bael_Record()
{
}

// MANIPULATORS
bael_Record& bael_Record::operator=(const bael_Record& rhs)
{
    d_level = rhs.d_level;
    return *this;
}

// ACCESSORS
int bael_Record::severity() const
{
    return d_level;
}

// FREE OPERATORS
bsl::ostream& operator<<(bsl::ostream& stream, const bael_Record& rhs)
{
    stream << rhs.severity();
    return stream;
}

}  // close namespace BloombergLP

typedef bcema_SharedPtr<bael_Record> Handle;

class my_DummyDeleter : public bcema_Deleter<bael_Record> {
    // A dummy deleter that does nothing in 'deleteObject'.

  public:
    // CREATORS
    my_DummyDeleter();
    ~my_DummyDeleter();

    // MANIPULATORS
    void deleteObject(bael_Record *record);

};

// CREATORS
my_DummyDeleter::my_DummyDeleter()
{
}

my_DummyDeleter::~my_DummyDeleter()
{
}

// MANIPULATORS
void my_DummyDeleter::deleteObject(bael_Record *)
{
}

//=============================================================================
//                             USAGE EXAMPLE 1 AND 2
//-----------------------------------------------------------------------------

void processRecord(const bcema_SharedPtr<bael_Record>& handle,
                         bael_RecordBuffer&            buffer)
{
    int severity = handle->severity();

    if (bael_Severity::BAEL_WARN >= severity) {
        buffer.pushBack(handle);
    }
    if (bael_Severity::BAEL_ERROR >= severity) {
        if (veryVerbose) bsl::cout << *handle << endl;
        buffer.beginSequence();
        int length = buffer.length();
        while (length--) {
            if (veryVerbose) bsl::cout << *buffer.back() << endl;
            buffer.popBack();
        }
        buffer.endSequence();
    }
}

// my_recordbuffer.h
class my_RecordBuffer : public bael_RecordBuffer {

    // DATA
    mutable bcemt_RecursiveMutex               d_mutex;
    bsl::vector<bcema_SharedPtr<bael_Record> > d_buffer;

    // NOT IMPLEMENTED
    my_RecordBuffer(const my_RecordBuffer&);
    my_RecordBuffer& operator=(const my_RecordBuffer&);

  public:
    // CREATORS
    my_RecordBuffer();
    virtual ~my_RecordBuffer();

    // MANIPULATORS
    virtual void beginSequence();
    virtual void endSequence();
    virtual void popBack();
    virtual void popFront();
    virtual int pushBack(const bcema_SharedPtr<bael_Record>& handle);
    virtual int pushFront(const bcema_SharedPtr<bael_Record>& handle);
    virtual void removeAll();

    // ACCESSORS
    virtual const bcema_SharedPtr<bael_Record>& back()  const;
    virtual const bcema_SharedPtr<bael_Record>& front() const;
    virtual int length() const;
};

// CREATORS
inline
my_RecordBuffer::my_RecordBuffer()
{
}

// MANIPULATORS
inline
void my_RecordBuffer::beginSequence()
{
    d_mutex.lock();
}

inline
void my_RecordBuffer::endSequence()
{
    d_mutex.unlock();
}

inline
void my_RecordBuffer::popBack()
{
    bcemt_LockGuard<bcemt_RecursiveMutex> guard(&d_mutex);
    d_buffer.pop_back();
}

inline
void my_RecordBuffer::popFront()
{
    bcemt_LockGuard<bcemt_RecursiveMutex> guard(&d_mutex);
    d_buffer.erase(d_buffer.begin());
}

inline
int my_RecordBuffer::pushBack(const bcema_SharedPtr<bael_Record>& handle)
{
    bcemt_LockGuard<bcemt_RecursiveMutex> guard(&d_mutex);
    d_buffer.push_back(handle);
    return 0;
}

inline
int my_RecordBuffer::pushFront(const bcema_SharedPtr<bael_Record>& handle)
{
    bcemt_LockGuard<bcemt_RecursiveMutex> guard(&d_mutex);
    d_buffer.insert(d_buffer.begin(), handle);
    return 0;
}

inline
void my_RecordBuffer::removeAll()
{
    bcemt_LockGuard<bcemt_RecursiveMutex> guard(&d_mutex);
    d_buffer.clear();
}

// ACCESSORS
inline
const bcema_SharedPtr<bael_Record>& my_RecordBuffer::back() const
{
    bcemt_LockGuard<bcemt_RecursiveMutex> guard(&d_mutex);
    return d_buffer.back();
}

inline
const bcema_SharedPtr<bael_Record>& my_RecordBuffer::front() const
{
    bcemt_LockGuard<bcemt_RecursiveMutex> guard(&d_mutex);
    return d_buffer.front();
}

inline
int my_RecordBuffer::length() const
{
    bcemt_LockGuard<bcemt_RecursiveMutex> guard(&d_mutex);
    return d_buffer.size();
}

// my_recordbuffer.cpp
// CREATORS
my_RecordBuffer::~my_RecordBuffer()
{
}

//=============================================================================
//                       CONCRETE DERIVED TYPES
//-----------------------------------------------------------------------------

struct RecordBufferTest : bsls_ProtocolTestImp<bael_RecordBuffer> {
    void popBack()                                     { markDone(); }
    void popFront()                                    { markDone(); }
    int pushBack(const bcema_SharedPtr<bael_Record>&)  { return markDone(); }
    int pushFront(const bcema_SharedPtr<bael_Record>&) { return markDone(); }
    void removeAll()                                   { markDone(); }
    void beginSequence()                               { markDone(); }
    void endSequence()                                 { markDone(); }
    const bcema_SharedPtr<bael_Record>& back() const
                                                      { return markDoneRef(); }
    const bcema_SharedPtr<bael_Record>& front() const
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

        bcema_TestAllocator ta;

        my_DummyDeleter deleter;
        my_RecordBuffer buffer;

        bael_Record record1(bael_Severity::BAEL_FATAL);
        bcema_SharedPtr<bael_Record> handle1(&record1, &deleter, &ta);
        processRecord(handle1, buffer);

        bael_Record record2(bael_Severity::BAEL_TRACE);
        bcema_SharedPtr<bael_Record> handle2(&record2, &deleter, &ta);
        processRecord(handle2, buffer);

        bael_Record record3(bael_Severity::BAEL_WARN);
        bcema_SharedPtr<bael_Record> handle3(&record3, &deleter, &ta);
        processRecord(handle3, buffer);

        bael_Record record4(bael_Severity::BAEL_ERROR);
        bcema_SharedPtr<bael_Record> handle4(&record4, &deleter, &ta);
        processRecord(handle4, buffer);

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // PROTOCOL TEST:
        // Concerns:
        //   We must ensure that (1) a subclass of the 'bael_RecordBuffer'
        //   class compiles and links when all virtual functions are defined,
        //   and (2) the functions are in fact virtual.
        //
        // Plan:
        //   Construct an object of a class derived from 'bael_RecordBuffer'
        //   and bind a 'bael_RecordBuffer' reference to the object.  Using the
        //   base class reference, invoke the 'beginSequence',
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
        //   virtual int pushBack(const bcema_SharedPtr<bael_Record>&) = 0;
        //   virtual int pushFront(const bcema_SharedPtr<bael_Record>&)= 0;
        //   virtual void removeAll() = 0;
        //   virtual const bael_Record& back() const = 0;
        //   virtual const bael_Record& front() const = 0;
        //   virtual int length() const = 0;
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "PROTOCOL TEST" << endl
                                  << "=============" << endl;

        bsls_ProtocolTest<RecordBufferTest> t(veryVerbose);

        ASSERT(t.testAbstract());
        ASSERT(t.testNoDataMembers());
        ASSERT(t.testVirtualDestructor());

        BSLS_PROTOCOLTEST_ASSERT(t, popBack());
        BSLS_PROTOCOLTEST_ASSERT(t, popFront());
        BSLS_PROTOCOLTEST_ASSERT(t, pushBack(bcema_SharedPtr<bael_Record>()));
        BSLS_PROTOCOLTEST_ASSERT(t, pushFront(bcema_SharedPtr<bael_Record>()));
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

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2004
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
