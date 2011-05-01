// bcema_sharedptrrep.t.cpp                                           -*-C++-*-

#include <bcema_sharedptrrep.h>
#include <bsl_algorithm.h>       // for 'bsl::swap'
#include <bslma_default.h>
#include <bslma_allocator.h>
#include <bdet_datetime.h>
#include <bcema_testallocator.h>                // for testing only
#include <iostream>

using bsl::cout;
using bsl::endl;
using bsl::cerr;

#ifdef BSLS_PLATFORM__CMP_MSVC  // Microsoft Compiler
#ifdef _MSC_EXTENSIONS          // Microsoft Extensions Enabled
#include <bsl_new.h>            // if so, need to include new as well
#endif
#endif

using namespace BloombergLP;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// This test driver tests the functionality of the representation object for
// shared pointer.
// - The test plans are still to be written (so marked TBD).
//-----------------------------------------------------------------------------
// bcema_SharedPtrRep
//------------------------
// [ 9] void managedPtrDeleter(void *, bcema_SharedPtrRep *rep); // TBD
// [ 2] bcema_SharedPtrRep();
// [ 3] void acquireRef();
// [ 8] void incrementRefs(int incrementAmount = 1);
// [ 3] void acquireWeakRef();
// [ 4] void releaseRef();
// [ 4] void releaseWeakRef();
// [ 7] bool tryAcquireRef();
// [ 6] void resetCountsRaw(int numSharedReferences, int numWeakReferences);
// [ 5] void disposeRep();
// [ 5] void disposeObject();
// [ 2] int numReferences() const;
// [ 2] int numWeakReferences() const;
// [ 2] bool hasUniqueOwner() const;
//
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [10] USAGE EXAMPLE // TBD
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

// TEST IMPLEMENTATION (defined below)
class MyTestImplementation;

// TYPEDEFS
typedef bcema_SharedPtrRep Obj;
typedef MyTestImplementation TObj;

//=============================================================================
//               GLOBAL HELPER CLASSES AND FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

                         // ========================
                         // class MyTestImplementation
                         // ========================

class MyTestImplementation : public bcema_SharedPtrRep{
    // This class provides an implementation for 'bcema_SharedPtrRep' so that
    // it can be initialized and tested.

    // DATA
    int d_numRepDisposed;
    int d_numObjectDisposed;
  public:
    // CREATORS
    MyTestImplementation();

    // ACCESSORS
    bool getNumRepDisposed();
    bool getNumObjectDisposed();
    virtual void *originalPtr() const;

    // MUTATORS
    virtual void disposeRep();
    virtual void disposeObject();
};

                         // ------------------------
                         // class MyTestImplementation
                         // ------------------------

// CREATORS
MyTestImplementation::MyTestImplementation()
: d_numRepDisposed(0)
, d_numObjectDisposed(0) {
}

bool MyTestImplementation::getNumRepDisposed() {
    return d_numRepDisposed;
}

// ACCESSORS
bool MyTestImplementation::getNumObjectDisposed() {
    return d_numObjectDisposed;
}
void *MyTestImplementation::originalPtr() const {
   return NULL;
}

// MUTATORS
void MyTestImplementation::disposeRep() {
    // The implementation of this method allows verification on whether
    // 'disposeRep' is called or not.

    ++d_numRepDisposed;
}

void MyTestImplementation::disposeObject() {
    // The implementation of this method allows verification on whether
    // 'disposeObject' is called or not.

    ++d_numObjectDisposed;
}

                      // =============================
                      // class MySharedDatetimeRepImpl
                      // =============================

class MySharedDatetimeRepImpl : public bcema_SharedPtrRep {
  private:
    // Implementation of 'bcema_SharedPtrRep' for in-place object.

    // DATA
    bslma_Allocator *d_allocator_p; // memory allocator (held, not owned)
    bdet_Datetime    d_instance;    // in-place object

  public:
    // CREATORS
    MySharedDatetimeRepImpl(bslma_Allocator *basicAllocator,
                            int              year,
                            int              month,
                            int              day);
        // Create a shared representation of a 'bdet_Datetime' object
        // having the specified 'year', 'month' and 'day' using the
        // specified 'basicAllocator' to allocate memory.

    // MANIPULATORS
    virtual void disposeRep();
        // Dispose of this 'MySharedDatetimeRepImpl' object.

    virtual void disposeObject();
        // Dispose of the managed 'bdet_Datetime' object.

    // ACCESSORS
    bdet_Datetime *ptr();
        // Returns a modifiable pointer to the managed 'bdet_Datetime'
        // object.

    virtual void *originalPtr() const;
        // Returns a void pointer to the to the managed 'bdet_Datetime'
        // object;
};


                      // -----------------------------
                      // class MySharedDatetimeRepImpl
                      // -----------------------------

MySharedDatetimeRepImpl::MySharedDatetimeRepImpl(
                                               bslma_Allocator *basicAllocator,
                                               int              year,
                                               int              month,
                                               int              day)
: d_allocator_p(basicAllocator)
, d_instance(year, month, day)
{
}

void MySharedDatetimeRepImpl::disposeRep() {
    d_allocator_p->deallocate(this);
}

void MySharedDatetimeRepImpl::disposeObject() {
    d_instance.~bdet_Datetime();
}

bdet_Datetime *MySharedDatetimeRepImpl::ptr() {
    return &d_instance;
}

void *MySharedDatetimeRepImpl::originalPtr() const {
    return const_cast<void*>(static_cast<const void *>(&d_instance));
}

                      // ======================
                      // class MySharedDatetime
                      // ======================

class MySharedDatetime {
    // This class provide a reference counted managed pointer to supporting
    // shared ownership of a 'bdet_Datetime' object.

  private:
    bdet_Datetime      *d_ptr_p;  // pointer to the managed object
    bcema_SharedPtrRep *d_rep_p;  // pointer to the representation object

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
        // object it might be referring to.

    // MANIPULATORS
    void createInplace(bslma_Allocator *basicAllocator,
                       int              year,
                       int              month,
                       int              day);
        // Create a new 'MySharedDatetimeRepImpl', using the specified
        // 'basicAllocator' to supply memory, using the specified 'year',
        // 'month' and 'day' to initialize the 'bdet_Datetime' within the
        // newly created 'MySharedDatetimeRepImpl', and make this
        // 'MySharedDatetime' refer to the newly created 'bdet_Datetime'.

    bdet_Datetime& operator*() const;
        // Return a modifiable reference to the shared datetime.

    bdet_Datetime *operator->() const;
        // Return the address of the modifiable 'bdet_Datetime' to which
        // this object refers.

    bdet_Datetime *ptr() const;
        // Return the address of the modifiable 'bdet_Datetime' to which
        // this object refers.
};

                         // ----------------------
                         // class MySharedDatetime
                         // ----------------------

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

void MySharedDatetime::createInplace(bslma_Allocator *basicAllocator,
                                     int              year,
                                     int              month,
                                     int              day)
{
    basicAllocator = bslma_Default::allocator(basicAllocator);
    MySharedDatetimeRepImpl *rep = new (*basicAllocator)
                                        MySharedDatetimeRepImpl(basicAllocator,
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

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    bcema_TestAllocator ta;

    switch (test) { case 0:  // Zero is always the leading case.
    case 10: {
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
      case 9: {
        // --------------------------------------------------------------------
        // TESTING 'managedPtrDeleter'
        //
        // Concerns:
        //   'managedPtrDeleter' decrement the number of references as
        //   expected.
        //
        // Plan:
        //   Call 'managedPtrDeleter' and verify the number of references
        //   change as specified.
        //
        // Testing:
        //   void managedPtrDeleter(void*, bcema_SharedPtrRep *rep);
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "Testing 'managedPtrDeleter'" << endl
                          << "==========================" << endl;

        {
            TObj t;
            Obj& x = t;
            Obj const& X = x;

            x.managedPtrDeleter(0, &x);
            ASSERT(0 == X.numReferences());
            ASSERT(0 == X.numWeakReferences());
            ASSERT(false == X.hasUniqueOwner());
            ASSERT(1 == t.getNumObjectDisposed());
            ASSERT(1 == t.getNumRepDisposed());
        }
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // TESTING 'incrementRefs'
        //
        // Concerns:
        //   'incrementRefs' increase the number of references by the specified
        //   amount.
        //
        // Plan:
        //   Call 'incrementRefs' with different parameters and verify the
        //   number of references change as specified.
        //
        // Testing:
        //   void incrementRefs(int incrementAmount = 1);
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "Testing 'incrementRefs'" << endl
                          << "========================" << endl;

        {
            TObj t;
            Obj& x = t;
            Obj const& X = x;

            ASSERT(1 == X.numReferences());

            x.incrementRefs();
            ASSERT(2 == X.numReferences());

            x.incrementRefs();
            ASSERT(3 == X.numReferences());

            x.incrementRefs(1);
            ASSERT(4 == X.numReferences());

            x.releaseRef();
            x.releaseRef();
            x.releaseRef();

            ASSERT(1 == X.numReferences());

            x.incrementRefs(3);
            ASSERT(4 == X.numReferences());

            x.incrementRefs(3);
            ASSERT(7 == X.numReferences());

            x.incrementRefs(3);
            ASSERT(10 == X.numReferences());
        }
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING 'tryAcquireRef'
        //
        // Concerns:
        //   'tryAcquireRef' increments the number of shared references if
        //   'numReference' > 0 and do nothing otherwise.
        //
        // Plan:
        //   Call 'tryAcquireRef'.  Verify that 'tryAcquireRef' returns true
        //   and the 'numReference' is correct.  Call 'acquireWeakRef' and two
        //   'releaseRef', then call 'tryAcquireRef'.   Verify that
        //   'tryAcquireRef' return false and it does not change
        //   'numReference'.
        //
        // Testing:
        //   bool tryAcquireRef();
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "Testing 'tryAcquireRef'" << endl
                          << "========================" << endl;

        {
            TObj t;
            Obj& x = t;
            Obj const& X = x;

            ASSERT(x.tryAcquireRef());
            ASSERT(2 == X.numReferences());

            x.acquireWeakRef();
            x.releaseRef();
            x.releaseRef();

            ASSERT(!x.tryAcquireRef());
            ASSERT(0 == X.numReferences());
        }
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING 'resetCountsRaw'
        //
        // Concerns:
        //   'resetCountsRaw' is setting number of shared references and weak
        //   references to the correct value.
        //
        // Plan:
        //   Call 'resetCountsRaw' with enumeration in two dimension up to two
        //   levels deep and verify the number of shared/weak references are
        //   correct.
        //
        // Testing:
        //   void resetCountsRaw(int numSharedReferences,
        //                       int numWeakReferences);
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "Testing 'resetCountsRaw'" << endl
                          << "========================" << endl;

        {
            TObj t;
            Obj& x = t;
            Obj const& X = x;

            for (int i = 0; i < 3; ++i) {
                for (int j = 0; j < i + 1; ++j) {
                    int numRef = j;
                    int numWeakRef = i - j;
                    x.resetCountsRaw(numRef, numWeakRef);
                    LOOP2_ASSERT(numRef, numWeakRef,
                                                  numRef == X.numReferences());
                    LOOP2_ASSERT(numRef, numWeakRef,
                                          numWeakRef == X.numWeakReferences());
                    LOOP2_ASSERT(numRef, numWeakRef,
                                                0 == t.getNumObjectDisposed());
                    LOOP2_ASSERT(numRef, numWeakRef,
                                                   0 == t.getNumRepDisposed());
                }
            }
        }
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING 'disposeObject' and 'disposeRep'
        //
        // Concerns:
        //   1) disposeObject() is called when there is no shared reference.
        //   2) disposeRep() is called only when there is no shared reference
        //   and no weak reference.
        //
        // Plan:
        //   Create an object and call 'releaseRef'.  Then verify that
        //   both 'disposeObject' and 'disposeRep' is called.
        //   Create another object and call 'acquireWeakRef' before calling
        //   'releaseRef'.  Verify that only 'disposeObject' is called.  Then
        //   call 'releaseWeakRef' and verify that 'disposeRep' is called.
        //
        // Testing:
        //   void disposeObject();
        //   void disposeRep();
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "Testing 'disposeObject' and 'disposeRep'" << endl
                          << "====================================" << endl;

        {
            TObj t;
            Obj& x = t;
            Obj const& X = x;

            ASSERT(0 == t.getNumObjectDisposed());
            ASSERT(0 == t.getNumRepDisposed());

            x.releaseRef();

            ASSERT(0 == X.numReferences());
            ASSERT(0 == X.numWeakReferences());
            ASSERT(false == X.hasUniqueOwner());
            ASSERT(1 == t.getNumObjectDisposed());
            ASSERT(1 == t.getNumRepDisposed());
        }
        {
            TObj t;
            Obj& x = t;
            Obj const& X = x;

            ASSERT(0 == t.getNumObjectDisposed());
            ASSERT(0 == t.getNumRepDisposed());

            x.acquireWeakRef();
            x.releaseRef();

            ASSERT(0 == X.numReferences());
            ASSERT(1 == X.numWeakReferences());
            ASSERT(false == X.hasUniqueOwner());
            ASSERT(1 == t.getNumObjectDisposed());
            ASSERT(0 == t.getNumRepDisposed());

            x.releaseWeakRef();
            ASSERT(0 == X.numReferences());
            ASSERT(0 == X.numWeakReferences());
            ASSERT(1 == t.getNumRepDisposed());
        }
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING 'releaseRef' and 'releaseWeakRef'
        //
        // Concerns:
        //   'releaseRef' and 'releaseWeakRef' is decrementing the reference
        //   count correctly.
        //
        // Plan:
        //   Call 'acquireRef' then 'releaseRef' and verify 'numReference'
        //   did not change.  Call 'acquireWeakRef' then 'releaseWeakRef' and
        //   verify 'numWeakReference' did not change.
        //
        // Testing:
        //   void releaseRef();
        //   void releaseWeakRef();
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "Testing 'acquireRef' and 'releaseRef'" << endl
                          << "====================================" << endl;

        {
            TObj t;
            Obj& x = t;
            Obj const& X = x;

            x.acquireRef();
            x.acquireRef();
            x.releaseRef();

            ASSERT(2 == X.numReferences());
            ASSERT(0 == X.numWeakReferences());
            ASSERT(false == X.hasUniqueOwner());

            x.releaseRef();

            ASSERT(1 == X.numReferences());
            ASSERT(0 == X.numWeakReferences());
            ASSERT(true == X.hasUniqueOwner());

            x.acquireWeakRef();
            x.acquireWeakRef();

            x.releaseWeakRef();

            ASSERT(1 == X.numReferences());
            ASSERT(1 == X.numWeakReferences());
            ASSERT(false == X.hasUniqueOwner());

            x.releaseWeakRef();

            ASSERT(1 == X.numReferences());
            ASSERT(0 == X.numWeakReferences());
            ASSERT(true == X.hasUniqueOwner());
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING 'acquireRef' and 'acquireWeakRef'
        //
        // Concerns:
        //   'acquireRef' and 'acquireWeakRef' is incrementing the reference
        //   count correctly.
        //
        // Plan:
        //   Call 'acquireRef' and 'acquireWeakRef' and verify 'numReference'
        //   and 'numWeakReference' is incremented correctly.
        //
        // Testing:
        //   void acquireRef();
        //   void acquireWeakRef();
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "Testing 'acquireRef' and 'releaseRef'" << endl
                          << "====================================" << endl;

        {
            TObj t;
            Obj& x = t;
            Obj const& X = x;

            x.acquireRef();

            ASSERT(2 == X.numReferences());
            ASSERT(0 == X.numWeakReferences());
            ASSERT(false == X.hasUniqueOwner());

            x.acquireRef();

            ASSERT(3 == X.numReferences());
            ASSERT(0 == X.numWeakReferences());
            ASSERT(false == X.hasUniqueOwner());

            x.acquireWeakRef();

            ASSERT(3 == X.numReferences());
            ASSERT(1 == X.numWeakReferences());
            ASSERT(false == X.hasUniqueOwner());

            x.acquireWeakRef();

            ASSERT(3 == X.numReferences());
            ASSERT(2 == X.numWeakReferences());
            ASSERT(false == X.hasUniqueOwner());

        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING BASIC CONSTRUCTORS AND ACCESSOR
        //
        // Concerns:
        //   Object is properly initialized.
        //
        // Plan:
        //   Create an object and verify accessors are returning the expected
        //   values.
        //
        // Testing:
        //   bcema_SharedPtrRep();
        //   int numReferences() const;
        //   int numWeakReferences() const;
        //   bool hasUniqueOwner() const;
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "Testing Constructors and Destructor" << endl
                          << "===================================" << endl;

        if (verbose) cout << endl
                          << "Testing default constructor" << endl
                          << "---------------------------" << endl;
        {
            TObj t;
            Obj& x = t;
            Obj const& X = x;

            ASSERT(1 == X.numReferences());
            ASSERT(0 == X.numWeakReferences());
            ASSERT(true == X.hasUniqueOwner());
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
        {
            TObj t;
            Obj& x = t;
            Obj const& X = x;

            ASSERT(1 == X.numReferences());

            x.acquireRef();

            ASSERT(2 == X.numReferences());

            x.releaseRef();

            ASSERT(1 == X.numReferences());

            x.releaseRef();

            ASSERT(0 == X.numReferences());
            ASSERT(1 == t.getNumObjectDisposed());
            ASSERT(1 == t.getNumRepDisposed());
        }
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
//      Copyright (C) Bloomberg L.P., 2011
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
