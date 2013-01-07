// bcema_sharedptrinplacerep.t.cpp                                   -*-C++-*-

#include <bcema_sharedptrinplacerep.h>

#include <bcema_testallocator.h>                            // for testing only
#include <bdet_datetime.h>                                  // for testing only
#include <bsl_iostream.h>

using bsl::cout;
using bsl::endl;
using bsl::cerr;

#ifdef BSLS_PLATFORM_CMP_MSVC  // Microsoft Compiler
#ifdef _MSC_EXTENSIONS          // Microsoft Extensions Enabled
#include <bsl_new.h>            // if so, need to include new as well
#endif
#endif

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// This test driver tests the functionality of the in-place shared pointer
// representation.
//-----------------------------------------------------------------------------
// bcema_SharedPtrInplaceRep
//------------------------
// [ 2] bcema_SharedPtrInplaceRep(bslma_Allocator *basicAllocator);
// [ 3] bcema_SharedPtrInplaceRep(bslma_Allocator *allocator, const A1& a1);
// [ 3] bcema_SharedPtrInplaceRep(bslma_Allocator *allocator, const A1&...a2);
// [ 3] bcema_SharedPtrInplaceRep(bslma_Allocator *allocator, const A1&...a3);
// [ 3] bcema_SharedPtrInplaceRep(bslma_Allocator *allocator, const A1&...a4);
// [ 3] bcema_SharedPtrInplaceRep(bslma_Allocator *allocator, const A1&...a5);
// [ 3] bcema_SharedPtrInplaceRep(bslma_Allocator *allocator, const A1&...a6);
// [ 3] bcema_SharedPtrInplaceRep(bslma_Allocator *allocator, const A1&...a7);
// [ 3] bcema_SharedPtrInplaceRep(bslma_Allocator *allocator, const A1&...a8);
// [ 3] bcema_SharedPtrInplaceRep(bslma_Allocator *allocator, const A1&...a9);
// [ 3] bcema_SharedPtrInplaceRep(bslma_Allocator *allocator, const A1...a10);
// [ 3] bcema_SharedPtrInplaceRep(bslma_Allocator *allocator, const A1...a11);
// [ 3] bcema_SharedPtrInplaceRep(bslma_Allocator *allocator, const A1...a12);
// [ 3] bcema_SharedPtrInplaceRep(bslma_Allocator *allocator, const A1...a13);
// [ 3] bcema_SharedPtrInplaceRep(bslma_Allocator *allocator, const A1...a14);
// [ 2] TYPE *ptr();
// [ 2] void disposeRep();
// [ 4] void disposeObject();
// [ 2] void *originalPtr() const;
// [ 5] void releaseRef();
// [ 5] void releaseWeakRef();
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 6] USAGE EXAMPLE
//-----------------------------------------------------------------------------

//=============================================================================
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

//=============================================================================
//                    STANDARD BDE LOOP-ASSERT TEST MACROS
//-----------------------------------------------------------------------------

#define LOOP_ASSERT(I,X) { \
    if (!(X)) { cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__);}}

#define LOOP2_ASSERT(I,J,X) { \
    if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " \
              << J << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) { \
    if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " \
          << J << "\t" << #K << ": " << K << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP4_ASSERT(I,J,K,L,X) { \
    if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J \
          << "\t" << #K << ": " << K << "\t" << #L << ": " << L \
          << "\n"; aSsErT(1, #X, __LINE__); } }

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", "<< flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number
#define T_ cout << "\t" << flush;

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

// 'MyTestObject' CLASS HIERARCHY (defined below)
class MyTestObject;

// OTHER TEST OBJECTS (defined below)
class MyInplaceTestObject;

// TYPEDEFS
typedef bcema_SharedPtrInplaceRep<MyTestObject> Obj;

typedef bcema_SharedPtrInplaceRep<MyInplaceTestObject> TCObj;
                                                    // For testing constructors
typedef MyTestObject TObj;

//=============================================================================
//               GLOBAL HELPER CLASSES AND FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

                             // ==================
                             // class MyTestObject
                             // ==================

class MyTestObject {
    // This class provides a test object that keeps track of how many instances
    // have been deleted.  Optionally, also keeps track of how many instances
    // have been copied.

    // DATA
    volatile int *d_deleteCounter_p;
    volatile int *d_copyCounter_p;

  public:
    // CREATORS
    MyTestObject(MyTestObject const& orig);
    MyTestObject(int *deleteCounter, int *copyCounter = 0);
    virtual ~MyTestObject();

    // ACCESSORS
    volatile int *deleteCounter() const;
    volatile int *copyCounter() const;
};

                             // ------------------
                             // class MyTestObject
                             // ------------------

// CREATORS
MyTestObject::MyTestObject(MyTestObject const& orig)
: d_deleteCounter_p(orig.d_deleteCounter_p)
, d_copyCounter_p(orig.d_copyCounter_p)
{
    if (d_copyCounter_p) ++(*d_copyCounter_p);
}

MyTestObject::MyTestObject(int *deleteCounter, int *copyCounter)
: d_deleteCounter_p(deleteCounter)
, d_copyCounter_p(copyCounter)
{
}

MyTestObject::~MyTestObject()
{
    ++(*d_deleteCounter_p);
}

// ACCESSORS
volatile int* MyTestObject::deleteCounter() const
{
    return d_deleteCounter_p;
}

volatile int* MyTestObject::copyCounter() const
{
    return d_copyCounter_p;
}

                         // =========================
                         // class MyInplaceTestObject
                         // =========================

#define DECLARE_TEST_ARG(NAME)                                            \
class NAME {                                                              \
    int d_value;                                                          \
  public:                                                                 \
    NAME(int value = -1) : d_value(value) {}                              \
    operator int&()      { return d_value; }                              \
    operator int() const { return d_value; }                              \
};
    // This macro declares a separate type with the specified 'NAME' that wraps
    // an integer value and provides implicit conversion to and from 'int'.
    // Its main purpose is that having separate types allows to distinguish
    // them in function interface, thereby avoiding ambiguities or accidental
    // switching of arguments in the implementation of in-place constructors.

DECLARE_TEST_ARG(MyTestArg1)
DECLARE_TEST_ARG(MyTestArg2)
DECLARE_TEST_ARG(MyTestArg3)
DECLARE_TEST_ARG(MyTestArg4)
DECLARE_TEST_ARG(MyTestArg5)
DECLARE_TEST_ARG(MyTestArg6)
DECLARE_TEST_ARG(MyTestArg7)
DECLARE_TEST_ARG(MyTestArg8)
DECLARE_TEST_ARG(MyTestArg9)
DECLARE_TEST_ARG(MyTestArg10)
DECLARE_TEST_ARG(MyTestArg11)
DECLARE_TEST_ARG(MyTestArg12)
DECLARE_TEST_ARG(MyTestArg13)
DECLARE_TEST_ARG(MyTestArg14)
    // Define fourteen test argument types 'MyTestArg1..14' to be used with the
    // in-place constructors of 'MyInplaceTestObject'.

class MyInplaceTestObject {
    // This class provides a test object used to check that the arguments
    // passed for creating a shared pointer with an in-place representation are
    // of the correct types and values.

    // DATA
    MyTestArg1 d_a1;
    MyTestArg2 d_a2;
    MyTestArg3 d_a3;
    MyTestArg4 d_a4;
    MyTestArg5 d_a5;
    MyTestArg6 d_a6;
    MyTestArg7 d_a7;
    MyTestArg8 d_a8;
    MyTestArg9 d_a9;
    MyTestArg10 d_a10;
    MyTestArg11 d_a11;
    MyTestArg12 d_a12;
    MyTestArg13 d_a13;
    MyTestArg14 d_a14;
    static int d_numDeletes;

  public:
    // CREATORS
    MyInplaceTestObject() {}

    MyInplaceTestObject(MyTestArg1 a1) : d_a1(a1) {}

    MyInplaceTestObject(MyTestArg1 a1, MyTestArg2 a2)
        : d_a1(a1), d_a2(a2) {}

    MyInplaceTestObject(MyTestArg1 a1, MyTestArg2 a2,
                        MyTestArg3 a3)
        : d_a1(a1), d_a2(a2), d_a3(a3) {}

    MyInplaceTestObject(MyTestArg1 a1, MyTestArg2 a2,
                        MyTestArg3 a3, MyTestArg4 a4)
        : d_a1(a1), d_a2(a2), d_a3(a3), d_a4(a4) {}

    MyInplaceTestObject(MyTestArg1 a1, MyTestArg2 a2,
                        MyTestArg3 a3, MyTestArg4 a4,
                        MyTestArg5 a5)
        : d_a1(a1), d_a2(a2), d_a3(a3), d_a4(a4), d_a5(a5) {}

    MyInplaceTestObject(MyTestArg1 a1, MyTestArg2 a2,
                        MyTestArg3 a3, MyTestArg4 a4,
                        MyTestArg5 a5, MyTestArg6 a6)
        : d_a1(a1), d_a2(a2), d_a3(a3), d_a4(a4), d_a5(a5)
        , d_a6(a6) {}

    MyInplaceTestObject(MyTestArg1 a1, MyTestArg2 a2,
                        MyTestArg3 a3, MyTestArg4 a4,
                        MyTestArg5 a5, MyTestArg6 a6,
                        MyTestArg7 a7)
        : d_a1(a1), d_a2(a2), d_a3(a3), d_a4(a4), d_a5(a5)
        , d_a6(a6), d_a7(a7) {}

    MyInplaceTestObject(MyTestArg1 a1, MyTestArg2 a2,
                        MyTestArg3 a3, MyTestArg4 a4,
                        MyTestArg5 a5, MyTestArg6 a6,
                        MyTestArg7 a7, MyTestArg8 a8)
        : d_a1(a1), d_a2(a2), d_a3(a3), d_a4(a4), d_a5(a5)
        , d_a6(a6), d_a7(a7), d_a8(a8) {}

    MyInplaceTestObject(MyTestArg1 a1, MyTestArg2 a2,
                        MyTestArg3 a3, MyTestArg4 a4,
                        MyTestArg5 a5, MyTestArg6 a6,
                        MyTestArg7 a7, MyTestArg8 a8,
                        MyTestArg9 a9)
        : d_a1(a1), d_a2(a2), d_a3(a3), d_a4(a4), d_a5(a5)
        , d_a6(a6), d_a7(a7), d_a8(a8), d_a9(a9) {}

    MyInplaceTestObject(MyTestArg1 a1, MyTestArg2 a2,
                        MyTestArg3 a3, MyTestArg4 a4,
                        MyTestArg5 a5, MyTestArg6 a6,
                        MyTestArg7 a7, MyTestArg8 a8,
                        MyTestArg9 a9, MyTestArg10 a10)
        : d_a1(a1), d_a2(a2), d_a3(a3), d_a4(a4), d_a5(a5)
        , d_a6(a6), d_a7(a7), d_a8(a8), d_a9(a9), d_a10(a10) {}

    MyInplaceTestObject(MyTestArg1  a1, MyTestArg2  a2,
                        MyTestArg3  a3, MyTestArg4  a4,
                        MyTestArg5  a5, MyTestArg6  a6,
                        MyTestArg7  a7, MyTestArg8  a8,
                        MyTestArg9  a9, MyTestArg10 a10,
                        MyTestArg11 a11)
        : d_a1(a1), d_a2(a2), d_a3(a3), d_a4(a4), d_a5(a5)
        , d_a6(a6), d_a7(a7), d_a8(a8), d_a9(a9), d_a10(a10)
        , d_a11(a11) {}

    MyInplaceTestObject(MyTestArg1  a1,  MyTestArg2  a2,
                        MyTestArg3  a3,  MyTestArg4  a4,
                        MyTestArg5  a5,  MyTestArg6  a6,
                        MyTestArg7  a7,  MyTestArg8  a8,
                        MyTestArg9  a9,  MyTestArg10 a10,
                        MyTestArg11 a11, MyTestArg12 a12)
        : d_a1(a1), d_a2(a2), d_a3(a3), d_a4(a4), d_a5(a5)
        , d_a6(a6), d_a7(a7), d_a8(a8), d_a9(a9), d_a10(a10)
        , d_a11(a11), d_a12(a12) {}

    MyInplaceTestObject(MyTestArg1  a1,  MyTestArg2  a2,
                        MyTestArg3  a3,  MyTestArg4  a4,
                        MyTestArg5  a5,  MyTestArg6  a6,
                        MyTestArg7  a7,  MyTestArg8  a8,
                        MyTestArg9  a9,  MyTestArg10 a10,
                        MyTestArg11 a11, MyTestArg12 a12,
                        MyTestArg13 a13)
        : d_a1(a1), d_a2(a2), d_a3(a3), d_a4(a4), d_a5(a5)
        , d_a6(a6), d_a7(a7), d_a8(a8), d_a9(a9), d_a10(a10)
        , d_a11(a11), d_a12(a12), d_a13(a13) {}

    MyInplaceTestObject(MyTestArg1  a1,  MyTestArg2  a2,
                        MyTestArg3  a3,  MyTestArg4  a4,
                        MyTestArg5  a5,  MyTestArg6  a6,
                        MyTestArg7  a7,  MyTestArg8  a8,
                        MyTestArg9  a9,  MyTestArg10 a10,
                        MyTestArg11 a11, MyTestArg12 a12,
                        MyTestArg13 a13, MyTestArg14 a14)
        : d_a1(a1), d_a2(a2), d_a3(a3), d_a4(a4), d_a5(a5)
        , d_a6(a6), d_a7(a7), d_a8(a8), d_a9(a9), d_a10(a10)
        , d_a11(a11), d_a12(a12), d_a13(a13), d_a14(a14) {}
    ~MyInplaceTestObject() { ++d_numDeletes; };

    // ACCESSORS
    bool operator == (MyInplaceTestObject const& rhs) const
    {
        return d_a1  == rhs.d_a1  &&
               d_a1  == rhs.d_a1  &&
               d_a2  == rhs.d_a2  &&
               d_a3  == rhs.d_a3  &&
               d_a4  == rhs.d_a4  &&
               d_a5  == rhs.d_a5  &&
               d_a6  == rhs.d_a6  &&
               d_a7  == rhs.d_a7  &&
               d_a8  == rhs.d_a8  &&
               d_a9  == rhs.d_a9  &&
               d_a10 == rhs.d_a10 &&
               d_a11 == rhs.d_a11 &&
               d_a12 == rhs.d_a12 &&
               d_a13 == rhs.d_a13 &&
               d_a14 == rhs.d_a14;
    }

    int getNumDeletes() { return d_numDeletes; }
};

int MyInplaceTestObject::d_numDeletes = 0;

                              // ================
                              // MySharedDatetime
                              // ================

class MySharedDatetime {
    // This class provide a reference counted smart pointer to support
    // shared ownership of a 'bdet_Datetime' object.

  private:
    bdet_Datetime      *d_ptr_p;  // pointer to the managed object
    bcema_SharedPtrRep *d_rep_p;  // pointer to the representation object

  private:
    // NOT IMPLEMENTED
    MySharedDatetime& operator=(const MySharedDatetime&);

  public:
    // CREATORS
    MySharedDatetime();
        // Create an empty shared datetime.

    MySharedDatetime(bdet_Datetime* ptr, bcema_SharedPtrRep* rep);
        // Create a shared datetime that adopts ownership of the specified
        // 'ptr' and the specified 'rep.

    MySharedDatetime(const MySharedDatetime& original);
        // Create a shared datetime that refers to the same object managed
        // by the specified 'original'

    ~MySharedDatetime();
        // Destroy this shared datetime and release the reference any
        // object it might be referring to.  If this is the last shared
        // reference, deleted the managed object.

    // MANIPULATORS
    void createInplace(bslma_Allocator *allocator,
                       int              year,
                       int              month,
                       int              day);
        // Create a new 'MySharedDatetimeRepImpl', using the specified
        // 'allocator' to supply memory, using the specified 'year',
        // 'month' and 'day' to initialize the 'bdet_Datetime' within the
        // newly created 'MySharedDatetimeRepImpl', and make this
        // 'MySharedDatetime' refer to the 'bdet_Datetime'.

    bdet_Datetime& operator*() const;
        // Return a modifiable reference to the shared 'bdet_Datetime'
        // object.

    bdet_Datetime *operator->() const;
        // Return the address of the modifiable 'bdet_Datetime' to which
        // this object refers.

    bdet_Datetime *ptr() const;
        // Return the address of the modifiable 'bdet_Datetime' to which
        // this object refers.
};

                              // ----------------
                              // MySharedDatetime
                              // ----------------

MySharedDatetime::MySharedDatetime()
: d_ptr_p(0)
, d_rep_p(0)
{
}

MySharedDatetime::MySharedDatetime(bdet_Datetime      *ptr,
                                   bcema_SharedPtrRep *rep)
: d_ptr_p(ptr)
, d_rep_p(rep)
{
}

MySharedDatetime::MySharedDatetime(const MySharedDatetime& original)
: d_ptr_p(original.d_ptr_p)
, d_rep_p(original.d_rep_p)
{
    if (d_ptr_p) {
        d_rep_p->acquireRef();
    } else {
        d_rep_p = 0;
    }
}

MySharedDatetime::~MySharedDatetime()
{
    if (d_rep_p) {
        d_rep_p->releaseRef();
    }
}

void MySharedDatetime::createInplace(bslma_Allocator *allocator,
                                     int              year,
                                     int              month,
                                     int              day)
{
    allocator = bslma_Default::allocator(allocator);
    bcema_SharedPtrInplaceRep<bdet_Datetime> *rep = new (*allocator)
                        bcema_SharedPtrInplaceRep<bdet_Datetime>(allocator,
                                                                 year,
                                                                 month,
                                                                 day);
    MySharedDatetime temp(rep->ptr(), rep);
    bsl::swap(d_ptr_p, temp.d_ptr_p);
    bsl::swap(d_rep_p, temp.d_rep_p);
}

bdet_Datetime& MySharedDatetime::operator*() const {
    return *d_ptr_p;
}

bdet_Datetime *MySharedDatetime::operator->() const {
    return d_ptr_p;
}

bdet_Datetime *MySharedDatetime::ptr() const {
    return d_ptr_p;
}

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;

    bcema_TestAllocator ta(veryVeryVerbose);
    int numDeallocations;
    int numAllocations;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:  // Zero is always the leading case.
    case 6: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE
        //
        // Concern:
        //   Usage example described in header doc compiles and run.
        //
        // Plan:
        //   Copy the usage example and strip the comments.  Then create simple
        //   test case to use the implementation described in doc.
        //
        // Testing:
        //   Usage example
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                        << "Testing Usage Example" << endl
                        << "=====================" << endl;
        {
            MySharedDatetime dt1;
            ASSERT(0 == ta.numAllocations());

            dt1.createInplace(&ta, 2011, 1, 1);

            ASSERT(1 == ta.numAllocations());

            MySharedDatetime dt2(dt1);

            ASSERT(dt2.ptr() == dt1.ptr());
            ASSERT(*dt2 == *dt1);

            dt2->addDays(31);
            ASSERT(2 == dt2->month());
            ASSERT(*dt1 == *dt2);
            ASSERT(0 == ta.numDeallocations());
        }
        ASSERT(1 == ta.numDeallocations());
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING 'releaseRef' and 'releaseWeakRef'
        //
        // Concerns:
        //   1) 'releaseRef' and 'releaseWeakRef' is decrementing the reference
        //   count correctly.
        //   2) disposeObject() is called when there is no shared reference.
        //   3) disposeRep() is called only when there is no shared reference
        //   and no weak reference.
        //
        // Plan:
        //   1) Call 'acquireRef' then 'releaseRef' and verify 'numReference'
        //   did not change.  Call 'acquireWeakRef' then 'releaseWeakRef' and
        //   verify 'numWeakReference' did not change.
        //   2) Call 'releaseRef' when there is only one reference remaining.
        //   Then verify that both 'disposeObject' and 'disposeRep' is called.
        //   3) Create another object and call 'acquireWeakRef' before calling
        //   'releaseRef'.  Verify that only 'disposeObject' is called.  Then
        //   call 'releaseWeakRef' and verify that 'disposeRep' is called.
        //
        // Testing:
        //   void releaseRef();
        //   void releaseWeakRef();
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                        << "Testing 'releaseRef' and 'releaseWeakRef'" << endl
                        << "=========================================" << endl;

        numAllocations = ta.numAllocations();
        numDeallocations = ta.numDeallocations();
        {
            int numDeletes = 0;
            Obj* xPtr = new(ta) Obj(&ta, &numDeletes);
            Obj& x = *xPtr;
            Obj const& X = *xPtr;

            ASSERT(++numAllocations == ta.numAllocations());

            x.acquireRef();
            x.releaseRef();

            ASSERT(1 == X.numReferences());
            ASSERT(0 == X.numWeakReferences());
            ASSERT(true == X.hasUniqueOwner());

            x.acquireWeakRef();
            x.releaseWeakRef();

            ASSERT(1 == X.numReferences());
            ASSERT(0 == X.numWeakReferences());
            ASSERT(true == X.hasUniqueOwner());

            if (verbose) cout << endl
                     << "Testing 'releaseRef' with no weak reference'" << endl
                     << "--------------------------------------------" << endl;

            x.releaseRef();

            ASSERT(1 == numDeletes);
            ASSERT(++numDeallocations == ta.numDeallocations());
        }
        if (verbose) cout << endl
                        << "Testing 'releaseRef' with weak reference'" << endl
                        << "-----------------------------------------" << endl;

        {
            int numDeletes = 0;
            Obj* xPtr = new(ta) Obj(&ta, &numDeletes);
            Obj& x = *xPtr;
            Obj const& X = *xPtr;

            ASSERT(++numAllocations == ta.numAllocations());

            x.acquireWeakRef();
            x.releaseRef();

            ASSERT(0 == X.numReferences());
            ASSERT(1 == X.numWeakReferences());
            ASSERT(false == X.hasUniqueOwner());
            ASSERT(1 == numDeletes);
            ASSERT(numDeallocations == ta.numDeallocations());

            x.releaseWeakRef();
            ASSERT(++numDeallocations == ta.numDeallocations());
        }
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING 'disposeObject'
        //
        // Concerns:
        //   The destructor of the object is called when 'disposeObject' is
        //   called.
        //
        // Plan:
        //   Call 'disposeObject' and verify that the destructor is called.
        //
        // Testing:
        //   void disposeObject();
        //
        // --------------------------------------------------------------------
        if (verbose) cout << "\nTesting disposeObject"
                          << "\n---------------------------------------\n";

        numAllocations = ta.numAllocations();
        numDeallocations = ta.numDeallocations();
        {
            int numDeletes = 0;
            Obj* xPtr = new(ta) Obj(&ta, &numDeletes);
            Obj& x = *xPtr;
            Obj const& X = *xPtr;

            ASSERT(++numAllocations == ta.numAllocations());
            ASSERT(0 == numDeletes);

            x.disposeObject();
            ASSERT(1 == numDeletes);

            x.disposeRep();
            ASSERT(++numDeallocations == ta.numDeallocations());
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING CONSTRUCTOR
        //
        // Concerns:
        //   All constructor is able to initialize the object correctly.
        //
        // Plan:
        //   Call all 14 different constructors and supply it with the
        //   appropriate arguments.  Then verify that the object created inside
        //   the representation is initialized using the arguments supplied.
        //
        // Testing:
        //   bcema_SharedPtrInplaceRep(bslma_Allocator *allocator, ...a1);
        //   bcema_SharedPtrInplaceRep(bslma_Allocator *allocator, ...a2);
        //   bcema_SharedPtrInplaceRep(bslma_Allocator *allocator, ...a3);
        //   bcema_SharedPtrInplaceRep(bslma_Allocator *allocator, ...a4);
        //   bcema_SharedPtrInplaceRep(bslma_Allocator *allocator, ...a5);
        //   bcema_SharedPtrInplaceRep(bslma_Allocator *allocator, ...a6);
        //   bcema_SharedPtrInplaceRep(bslma_Allocator *allocator, ...a7);
        //   bcema_SharedPtrInplaceRep(bslma_Allocator *allocator, ...a8);
        //   bcema_SharedPtrInplaceRep(bslma_Allocator *allocator, ...a9);
        //   bcema_SharedPtrInplaceRep(bslma_Allocator *allocator, ...a10);
        //   bcema_SharedPtrInplaceRep(bslma_Allocator *allocator, ...a11);
        //   bcema_SharedPtrInplaceRep(bslma_Allocator *allocator, ...a12);
        //   bcema_SharedPtrInplaceRep(bslma_Allocator *allocator, ...a13);
        //   bcema_SharedPtrInplaceRep(bslma_Allocator *allocator, ...a14);
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "Testing constructor" << endl
                          << "=======================" << endl;

        static const MyTestArg1 V1(1);
        static const MyTestArg2 V2(20);
        static const MyTestArg3 V3(23);
        static const MyTestArg4 V4(44);
        static const MyTestArg5 V5(66);
        static const MyTestArg6 V6(176);
        static const MyTestArg7 V7(878);
        static const MyTestArg8 V8(8);
        static const MyTestArg9 V9(912);
        static const MyTestArg10 V10(102);
        static const MyTestArg11 V11(111);
        static const MyTestArg12 V12(333);
        static const MyTestArg13 V13(712);
        static const MyTestArg14 V14(1414);

        if (verbose) cout << "\nTesting constructor with 1 argument"
                          << "\n---------------------------------------\n";

        numAllocations = ta.numAllocations();
        numDeallocations = ta.numDeallocations();
        {
            static const MyInplaceTestObject EXP(V1);
            TCObj* xPtr = new(ta) TCObj(&ta, V1);
            TCObj& x = *xPtr;
            TCObj const& X = *xPtr;

            ASSERT(++numAllocations == ta.numAllocations());
            ASSERT(EXP == *(x.ptr()));
            x.disposeRep();
            ASSERT(++numDeallocations == ta.numDeallocations());
        }

        if (verbose) cout << "\nTesting contructor with 2 arguments"
                          << "\n---------------------------------------\n";

        numAllocations = ta.numAllocations();
        numDeallocations = ta.numDeallocations();
        {
            static const MyInplaceTestObject EXP(V1, V2);
            TCObj* xPtr = new(ta) TCObj(&ta, V1, V2);
            TCObj& x = *xPtr;
            TCObj const& X = *xPtr;

            ASSERT(++numAllocations == ta.numAllocations());
            ASSERT(EXP == *(x.ptr()));
            x.disposeRep();
            ASSERT(++numDeallocations == ta.numDeallocations());
        }

        if (verbose) cout << "\nTesting constructor with 3 arguments"
                          << "\n---------------------------------------\n";

        numAllocations = ta.numAllocations();
        numDeallocations = ta.numDeallocations();
        {
            static const MyInplaceTestObject EXP(V1, V2, V3);
            TCObj* xPtr = new(ta) TCObj(&ta, V1, V2, V3);
            TCObj& x = *xPtr;
            TCObj const& X = *xPtr;

            ASSERT(++numAllocations == ta.numAllocations());
            ASSERT(EXP == *(x.ptr()));
            x.disposeRep();
            ASSERT(++numDeallocations == ta.numDeallocations());
        }

        if (verbose) cout << "\nTesting constructor with 4 arguments"
                          << "\n---------------------------------------\n";

        numAllocations = ta.numAllocations();
        numDeallocations = ta.numDeallocations();
        {
            static const MyInplaceTestObject EXP(V1, V2, V3, V4);
            TCObj* xPtr = new(ta) TCObj(&ta, V1, V2, V3, V4);
            TCObj& x = *xPtr;
            TCObj const& X = *xPtr;

            ASSERT(++numAllocations == ta.numAllocations());
            ASSERT(EXP == *(x.ptr()));
            x.disposeRep();
            ASSERT(++numDeallocations == ta.numDeallocations());
        }

        if (verbose) cout << "\nTesting constructor with 5 arguments"
                          << "\n---------------------------------------\n";

        numAllocations = ta.numAllocations();
        numDeallocations = ta.numDeallocations();
        {
            static const MyInplaceTestObject EXP(V1, V2, V3, V4, V5);
            TCObj* xPtr = new(ta) TCObj(&ta, V1, V2, V3, V4, V5);
            TCObj& x = *xPtr;
            TCObj const& X = *xPtr;

            ASSERT(++numAllocations == ta.numAllocations());
            ASSERT(EXP == *(x.ptr()));
            x.disposeRep();
            ASSERT(++numDeallocations == ta.numDeallocations());
        }

        if (verbose) cout << "\nTesting constructor with 6 arguments"
                          << "\n---------------------------------------\n";

        numAllocations = ta.numAllocations();
        numDeallocations = ta.numDeallocations();
        {
            static const MyInplaceTestObject EXP(V1, V2, V3, V4, V5, V6);
            TCObj* xPtr = new(ta) TCObj(&ta, V1, V2, V3, V4, V5, V6);
            TCObj& x = *xPtr;
            TCObj const& X = *xPtr;

            ASSERT(++numAllocations == ta.numAllocations());
            ASSERT(EXP == *(x.ptr()));
            x.disposeRep();
            ASSERT(++numDeallocations == ta.numDeallocations());
        }

        if (verbose) cout << "\nTesting constructor with 7 arguments"
                          << "\n---------------------------------------\n";

        numAllocations = ta.numAllocations();
        numDeallocations = ta.numDeallocations();
        {
            static const MyInplaceTestObject EXP(V1, V2, V3, V4, V5, V6, V7);
            TCObj* xPtr = new(ta) TCObj(&ta, V1, V2, V3, V4, V5, V6, V7);
            TCObj& x = *xPtr;
            TCObj const& X = *xPtr;

            ASSERT(++numAllocations == ta.numAllocations());
            ASSERT(EXP == *(x.ptr()));
            x.disposeRep();
            ASSERT(++numDeallocations == ta.numDeallocations());
        }

        if (verbose) cout << "\nTesting constructor with 8 arguments"
                          << "\n---------------------------------------\n";

        numAllocations = ta.numAllocations();
        numDeallocations = ta.numDeallocations();
        {
            static const MyInplaceTestObject EXP(V1, V2, V3, V4, V5, V6, V7,
                                                                           V8);
            TCObj* xPtr = new(ta) TCObj(&ta, V1, V2, V3, V4, V5, V6, V7, V8);
            TCObj& x = *xPtr;
            TCObj const& X = *xPtr;

            ASSERT(++numAllocations == ta.numAllocations());
            ASSERT(EXP == *(x.ptr()));
            x.disposeRep();
            ASSERT(++numDeallocations == ta.numDeallocations());
        }

        if (verbose) cout << "\nTesting constructor with 9 arguments"
                          << "\n---------------------------------------\n";

        numAllocations = ta.numAllocations();
        numDeallocations = ta.numDeallocations();
        {
            static const MyInplaceTestObject EXP(V1, V2, V3, V4, V5, V6, V7,
                                                                       V8, V9);
            TCObj* xPtr = new(ta) TCObj(&ta, V1, V2, V3, V4, V5, V6, V7, V8,
                                                                           V9);
            TCObj& x = *xPtr;
            TCObj const& X = *xPtr;

            ASSERT(++numAllocations == ta.numAllocations());
            ASSERT(EXP == *(x.ptr()));
            x.disposeRep();
            ASSERT(++numDeallocations == ta.numDeallocations());
        }

        if (verbose) cout << "\nTesting constructor with 10 arguments"
                          << "\n---------------------------------------\n";

        numAllocations = ta.numAllocations();
        numDeallocations = ta.numDeallocations();
        {
            static const MyInplaceTestObject EXP(V1, V2, V3, V4, V5, V6, V7,
                                                                  V8, V9, V10);
            TCObj* xPtr = new(ta) TCObj(&ta, V1, V2, V3, V4, V5, V6, V7, V8,
                                                                      V9, V10);
            TCObj& x = *xPtr;
            TCObj const& X = *xPtr;

            ASSERT(++numAllocations == ta.numAllocations());
            ASSERT(EXP == *(x.ptr()));
            x.disposeRep();
            ASSERT(++numDeallocations == ta.numDeallocations());
        }

        if (verbose) cout << "\nTesting constructor with 11 arguments"
                          << "\n---------------------------------------\n";

        numAllocations = ta.numAllocations();
        numDeallocations = ta.numDeallocations();
        {
            static const MyInplaceTestObject EXP(V1, V2, V3, V4, V5, V6, V7,
                                                             V8, V9, V10, V11);
            TCObj* xPtr = new(ta) TCObj(&ta, V1, V2, V3, V4, V5, V6, V7, V8,
                                                                 V9, V10, V11);
            TCObj& x = *xPtr;
            TCObj const& X = *xPtr;

            ASSERT(++numAllocations == ta.numAllocations());
            ASSERT(EXP == *(x.ptr()));
            x.disposeRep();
            ASSERT(++numDeallocations == ta.numDeallocations());
        }

        if (verbose) cout << "\nTesting constructor with 12 arguments"
                          << "\n---------------------------------------\n";

        numAllocations = ta.numAllocations();
        numDeallocations = ta.numDeallocations();
        {
            static const MyInplaceTestObject EXP(V1, V2, V3, V4, V5, V6, V7,
                                                        V8, V9, V10, V11, V12);
            TCObj* xPtr = new(ta) TCObj(&ta, V1, V2, V3, V4, V5, V6, V7, V8,
                                                            V9, V10, V11, V12);
            TCObj& x = *xPtr;
            TCObj const& X = *xPtr;

            ASSERT(++numAllocations == ta.numAllocations());
            ASSERT(EXP == *(x.ptr()));
            x.disposeRep();
            ASSERT(++numDeallocations == ta.numDeallocations());
        }

        if (verbose) cout << "\nTesting constructor with 13 arguments"
                          << "\n---------------------------------------\n";

        numAllocations = ta.numAllocations();
        numDeallocations = ta.numDeallocations();
        {
            static const MyInplaceTestObject EXP(V1, V2, V3, V4, V5, V6, V7,
                                                   V8, V9, V10, V11, V12, V13);
            TCObj* xPtr = new(ta) TCObj(&ta, V1, V2, V3, V4, V5, V6, V7, V8,
                                                       V9, V10, V11, V12, V13);
            TCObj& x = *xPtr;
            TCObj const& X = *xPtr;

            ASSERT(++numAllocations == ta.numAllocations());
            ASSERT(EXP == *(x.ptr()));
            x.disposeRep();
            ASSERT(++numDeallocations == ta.numDeallocations());
        }

        if (verbose) cout << "\nTesting constructor with 14 arguments"
                          << "\n---------------------------------------\n";

        numAllocations = ta.numAllocations();
        numDeallocations = ta.numDeallocations();
        {
            static const MyInplaceTestObject EXP(V1, V2, V3, V4, V5, V6, V7,
                                              V8, V9, V10, V11, V12, V13, V14);
            TCObj* xPtr = new(ta) TCObj(&ta, V1, V2, V3, V4, V5, V6, V7, V8,
                                                  V9, V10, V11, V12, V13, V14);
            TCObj& x = *xPtr;
            TCObj const& X = *xPtr;

            ASSERT(++numAllocations == ta.numAllocations());
            ASSERT(EXP == *(x.ptr()));
            x.disposeRep();
            ASSERT(++numDeallocations == ta.numDeallocations());
        }


      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING BASIC CONSTRUCTOR
        //
        // Concerns:
        //   Verify that upon construction the object is properly initialized,
        //
        // Plan:
        //   Construct object using basic constructor and verify that that
        //   accessors return the expected values.
        //
        // Testing:
        //   bcema_SharedPtrInplaceRep(bslma_Allocator *basicAllocator);
        //   TYPE *ptr();
        //   void disposeRep();
        //   void *originalPtr() const;
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Constructor"
                          << "\n---------------------------------------\n";

        numAllocations = ta.numAllocations();
        numDeallocations = ta.numDeallocations();
        {
            static const MyInplaceTestObject EXP;

            // Dynamically allocate object as the destructor is declared as
            // private.

            TCObj* xPtr = new(ta) TCObj(&ta);
            TCObj& x = *xPtr;
            TCObj const& X = *xPtr;

            ASSERT(++numAllocations == ta.numAllocations());
            ASSERT(EXP == *(x.ptr()));
            ASSERT(x.originalPtr() == static_cast<void*>(x.ptr()));

            // Manually deallocate the representation using 'disposeRep'.

            x.disposeRep();

            ASSERT(++numDeallocations == ta.numDeallocations());
        }

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //
        // Testing:
        //   This test exercises basic functionality but tests nothing.
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BREATHING TEST" << endl
                          << "==============" << endl;

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
