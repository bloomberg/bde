// bcema_sharedptroutofplacerep.t.cpp                                -*-C++-*-

#include <bcema_sharedptroutofplacerep.h>
#include <bsl_algorithm.h>       // for 'bsl::swap'
#include <bslma_default.h>
#include <bslma_allocator.h>
#include <bcema_testallocator.h>                // for testing only
#include <bsl_iostream.h>
#include <bdet_datetime.h>

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
// This test driver tests the functionality of out-of-place implementation of
// the shared pointer representation object.
//-----------------------------------------------------------------------------
// bcema_SharedPtrRep
//------------------------
// [ 2] bcema_SharedPtrOutofplaceRep(TYPE *ptr, const...BCEMA_ALLOCATOR_PTR>);
// [ 3] bcema_SharedPtrOutofplaceRep(TYPE *ptr, const...BCEMA_FACTORY_PTR>);
// [ 3] bcema_SharedPtrOutofplaceRep(TYPE *ptr, ...BCEMA_FUNCTOR_WITH_ALLOC>);
// [ 3] bcema_SharedPtrOutofplaceRep(TYPE *pt...BCEMA_FUNCTOR_WITHOUT_ALLOC>);
// [ 2] bcema_SharedPtrOutofplaceRep<TYPE, DELETER> * makeOutofplaceRep(...);
// [ 2] void disposeRep();
// [ 3] void disposeObject();
// [ 2] void *originalPtr() const;
// [ 2] TYPE *ptr() const;
//
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [10] USAGE EXAMPLE
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

enum { TEST_ALLOCATOR_DELETER =
  bcema_SharedPtrOutofplaceRep_DeleterDiscriminator<bslma_Allocator*>::VALUE };

// TEST IMPLEMENTATION (defined below)
class MyTestObject;
class MyAllocTestDeleter;

// TYPEDEFS
typedef bcema_SharedPtrOutofplaceRep<MyTestObject, bslma_Allocator *> Obj;
typedef MyTestObject TObj;
typedef void (*DeleteFunction)(MyTestObject *);

//=============================================================================
//               GLOBAL HELPER CLASSES AND FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

                         // ==================
                         // class MyTestObject
                         // ==================
class MyTestObject{
    // This class provides an implementation for 'bcema_SharedPtrRep' so that
    // it can be initialized and tested.

    // DATA
    int        d_data;
    static int d_deleteCounter;
  public:
    // CREATORS
    MyTestObject();
    ~MyTestObject();

    // ACCESSORS
    static int getNumDeletes();
};

                         // ------------------
                         // class MyTestObject
                         // ------------------
// CREATORS
MyTestObject::MyTestObject()
: d_data(0) {
}

MyTestObject::~MyTestObject() {
    ++d_deleteCounter;
}

// ACCESSORS
int MyTestObject::getNumDeletes() {
    return d_deleteCounter;
}

int MyTestObject::d_deleteCounter = 0;

                         // ======================
                         // struct MyDeleteFunctor
                         // ======================

struct MyDeleteFunctor {
    // This 'struct' provides an 'operator()' that can be used to delete a
    // 'MyTestObject' object.

  public:
    void operator()(MyTestObject *object) {
        // Destroy the specified 'object'.
        delete object;
    }
};

void myDeleteFunction(MyTestObject *object) {
    // Delete the specified 'object'.
    delete object;
}

                         // ========================
                         // class MyAllocTestDeleter
                         // ========================

class MyAllocTestDeleter {
    // This class provides a prototypical function-like deleter that takes a
    // 'bslma_Allocator' at construction.  It is used to check that the
    // allocator used to construct the representation is passed correctly to
    // the deleter.

    // DATA
    bslma_Allocator *d_allocator_p;
    bslma_Allocator *d_deleter_p;
    void            *d_someMemory;

  public:
    BSLALG_DECLARE_NESTED_TRAITS(MyAllocTestDeleter,
                                 bslalg_TypeTraitUsesBslmaAllocator);

  public:
    // CREATORS
    MyAllocTestDeleter(bslma_Allocator *deleter,
                       bslma_Allocator *basicAllocator = 0);

    MyAllocTestDeleter(MyAllocTestDeleter const&  orig,
                       bslma_Allocator           *basicAllocator = 0);

    ~MyAllocTestDeleter();

    // MANIPULATORS
    MyAllocTestDeleter& operator=(MyAllocTestDeleter const& orig);

    // ACCESSORS
    template <class TYPE>
    void operator()(TYPE *ptr) const;
};
                          // ------------------------
                          // class MyAllocTestDeleter
                          // ------------------------

// CREATORS
MyAllocTestDeleter::MyAllocTestDeleter(bslma_Allocator *deleter,
                                   bslma_Allocator *basicAllocator)
: d_allocator_p(bslma_Default::allocator(basicAllocator))
, d_deleter_p(deleter)
{
    d_someMemory = d_allocator_p->allocate(13);
}

MyAllocTestDeleter::MyAllocTestDeleter(
                                     MyAllocTestDeleter const&  original,
                                     bslma_Allocator           *basicAllocator)
: d_allocator_p(bslma_Default::allocator(basicAllocator))
, d_deleter_p(original.d_deleter_p)
{
    d_someMemory = d_allocator_p->allocate(13);
}

MyAllocTestDeleter::~MyAllocTestDeleter()
{
    d_allocator_p->deallocate(d_someMemory);
}

// MANIPULATORS
MyAllocTestDeleter& MyAllocTestDeleter::operator=(
                                                 MyAllocTestDeleter const& rhs)
{
    ASSERT(!"I think we do not use operator =");
    d_deleter_p = rhs.d_deleter_p;
    return *this;
}

// ACCESSORS
template <class TYPE>
void MyAllocTestDeleter::operator()(TYPE *ptr) const
{
    d_deleter_p->deleteObject(ptr);
}

                         // ========================
                         // class MyTestFactory
                         // ========================
class MyTestFactory {
  public:
    // MANIPULATORS
    MyTestObject *createObject(bslma_Allocator *basicAllocator = 0) {
        // Create a 'MyTestObject' object.  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is
        // 0, the currently installed default allocator is used.
        return new MyTestObject();
    }

    void deleteObject(MyTestObject *object) {
        // Delete the specified 'object'.
        delete object;
    }
};

                              // ================
                              // MySharedDatetime
                              // ================

class MySharedDatetime {
    // This class provide a reference counted smart pointer to support
    // shared ownership of a 'bdet_Datetime' object.

  private:
    bdet_Datetime      *d_ptr_p;  // pointer to the managed object
    bcema_SharedPtrRep *d_rep_p;  // pointer to the representation object

  public:
    // CREATORS
    MySharedDatetime(bdet_Datetime* ptr, bslma_Allocator* basicAllocator = 0);
    MySharedDatetime(const MySharedDatetime& original);
    ~MySharedDatetime();

    // MANIPULATORS
    bdet_Datetime& operator*() const;
        // Dereference the shared Datetime

    bdet_Datetime *operator->() const;
        // Return address of the modifiable 'bdet_Datetime' referred to by
        // this class.

    bdet_Datetime *ptr() const;
        // Return address of the modifiable 'bdet_Datetime' referred to by
        // this class.
};

                              // ----------------
                              // MySharedDatetime
                              // ----------------

MySharedDatetime::MySharedDatetime(bdet_Datetime *ptr,
                                   bslma_Allocator* basicAllocator)
{
    d_ptr_p = ptr;
    d_rep_p = bcema_SharedPtrOutofplaceRep<bdet_Datetime, bslma_Allocator*>::
                        makeOutofplaceRep(ptr, basicAllocator, basicAllocator);
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
    case 5: {
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
            ASSERT(0 == ta.numAllocations());
            MySharedDatetime dt1(new(ta) bdet_Datetime(2011, 1, 1), &ta);

            ASSERT(2 == ta.numAllocations());

            MySharedDatetime dt2(dt1);

            ASSERT(dt2.ptr() == dt1.ptr());
            ASSERT(*dt2 == *dt1);

            dt2->addDays(31);
            ASSERT(2 == dt2->month());
            ASSERT(*dt1 == *dt2);
            ASSERT(0 == ta.numDeallocations());
        }
        ASSERT(2 == ta.numDeallocations());
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING CONSTRUCTORS ACCESSOR
        //
        // Concerns:
        //   Object is properly initialized, and can be properly destructed
        //   when the last reference is released.
        //
        // Plan:
        //   Construct bcema_SharedPtrOutofplaceRep using each of the
        //   constructor and call releaseRef() to remove the last reference and
        //   check the that destructor fo the object is called.
        //
        // Testing:
        //   bcema_SharedPtrOutofplaceRep(
        //                    TYPE            *ptr,
        //                    const DELETER&   deleter,
        //                    bslma_Allocator *basicAllocator,
        //                    bslmf_MetaInt<DeleterType::BCEMA_ALLOCATOR_PTR>);
        //   bcema_SharedPtrOutofplaceRep(
        //                      TYPE            *ptr,
        //                      const DELETER&   deleter,
        //                      bslma_Allocator *basicAllocator,
        //                      bslmf_MetaInt<DeleterType::BCEMA_FACTORY_PTR>);
        //   bcema_SharedPtrOutofplaceRep(
        //               TYPE            *ptr,
        //               const DELETER&   deleter,
        //               bslma_Allocator *basicAllocator,
        //               bslmf_MetaInt<DeleterType::BCEMA_FUNCTOR_WITH_ALLOC>);
        //   bcema_SharedPtrOutofplaceRep(
        //            TYPE            *ptr,
        //            const DELETER&   deleter,
        //            bslma_Allocator *basicAllocator,
        //            bslmf_MetaInt<DeleterType::BCEMA_FUNCTOR_WITHOUT_ALLOC>);
        //   bcema_SharedPtrOutofplaceRep<TYPE, DELETER> * makeOutofplaceRep(
        //                                TYPE             *ptr,
        //                                const DELETER&   deleter,
        //                                bslma_Allocator *basicAllocator = 0);
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "Testing Constructors and Destructor" << endl
                          << "===================================" << endl;

        if (verbose) cout << endl
                          << "Testing bslma_Allocator Deleter" << endl
                          << "-------------------------------" << endl;

        {
            TObj* t = new(ta) TObj();
            Obj* xPtr = Obj::makeOutofplaceRep(t, &ta, &ta);
            Obj & x = *xPtr;
            Obj const& X = x;

            ASSERT(1 == X.numReferences());
            ASSERT(0 == X.numWeakReferences());
            ASSERT(true == X.hasUniqueOwner());

            x.releaseRef();
            ASSERT(1 == TObj::getNumDeletes());
        }

        if (verbose) cout << endl
                          << "Testing Factory Deleter" << endl
                          << "-----------------------" << endl;
        {
            enum { DELETER_TYPE =
              bcema_SharedPtrOutofplaceRep_DeleterDiscriminator<MyTestFactory*>
              ::VALUE };

            MyTestFactory *factory = new(ta) MyTestFactory();


            TObj* t = factory->createObject();
            bcema_SharedPtrOutofplaceRep<MyTestObject, MyTestFactory*>* xPtr =
                   bcema_SharedPtrOutofplaceRep<MyTestObject, MyTestFactory*>::
                   makeOutofplaceRep(t, factory, &ta);
            bcema_SharedPtrOutofplaceRep<MyTestObject, MyTestFactory* >& x =
                                                                         *xPtr;
            bcema_SharedPtrOutofplaceRep<MyTestObject, MyTestFactory* >
                                                                  const& X = x;

            ASSERT(1 == X.numReferences());
            ASSERT(0 == X.numWeakReferences());
            ASSERT(true == X.hasUniqueOwner());

            x.releaseRef();
            ASSERT(2 == TObj::getNumDeletes());
            ta.deleteObject(factory);
        }

        if (verbose) cout << endl
                          << "Testing Function Deleter" << endl
                          << "------------------------" << endl;
        {
            enum { DELETER_TYPE =
              bcema_SharedPtrOutofplaceRep_DeleterDiscriminator<DeleteFunction>
              ::VALUE };

            TObj* t = new TObj();
            bcema_SharedPtrOutofplaceRep<MyTestObject, DeleteFunction>* xPtr =
                   bcema_SharedPtrOutofplaceRep<MyTestObject, DeleteFunction>::
                   makeOutofplaceRep(t, myDeleteFunction, &ta);
            bcema_SharedPtrOutofplaceRep<MyTestObject, DeleteFunction >& x =
                                                                         *xPtr;
            bcema_SharedPtrOutofplaceRep<MyTestObject, DeleteFunction > const&
                                                                         X = x;

            ASSERT(1 == X.numReferences());
            ASSERT(0 == X.numWeakReferences());
            ASSERT(true == X.hasUniqueOwner());

            x.acquireWeakRef();
            x.releaseRef();
            ASSERT(3 == TObj::getNumDeletes());
            x.releaseWeakRef();
        }

        if (verbose) cout << endl
                          << "Testing Functor Deleter Without Alloc" << endl
                          << "-------------------------------------" << endl;
        {
            enum { DELETER_TYPE =
             bcema_SharedPtrOutofplaceRep_DeleterDiscriminator<MyDeleteFunctor>
             ::VALUE };

            TObj* t = new TObj();
            MyDeleteFunctor deleteFunctor;
            bcema_SharedPtrOutofplaceRep<MyTestObject, MyDeleteFunctor>* xPtr=
                  bcema_SharedPtrOutofplaceRep<MyTestObject, MyDeleteFunctor>::
                  makeOutofplaceRep(t, deleteFunctor, &ta);
            bcema_SharedPtrOutofplaceRep<MyTestObject, MyDeleteFunctor >& x =
                                                                         *xPtr;
            bcema_SharedPtrOutofplaceRep<MyTestObject, MyDeleteFunctor >
                                                                  const& X = x;

            ASSERT(1 == X.numReferences());
            ASSERT(0 == X.numWeakReferences());
            ASSERT(true == X.hasUniqueOwner());

            x.releaseRef();
            ASSERT(4 == TObj::getNumDeletes());
        }

        if (verbose) cout << endl
                          << "Testing Functor Deleter With Alloc" << endl
                          << "----------------------------------" << endl;
        {
            enum { DELETER_TYPE =
          bcema_SharedPtrOutofplaceRep_DeleterDiscriminator<MyAllocTestDeleter>
          ::VALUE };

            TObj* t = new(ta) TObj();
            MyAllocTestDeleter deleteFunctor(&ta, &ta);

            bcema_SharedPtrOutofplaceRep<MyTestObject, MyAllocTestDeleter>*
               xPtr =
               bcema_SharedPtrOutofplaceRep<MyTestObject, MyAllocTestDeleter>::
               makeOutofplaceRep(t, deleteFunctor, &ta);

            bcema_SharedPtrOutofplaceRep<MyTestObject, MyAllocTestDeleter>& x =
                                                                         *xPtr;

            bcema_SharedPtrOutofplaceRep<MyTestObject, MyAllocTestDeleter>
                                                                  const& X = x;

            ASSERT(1 == X.numReferences());
            ASSERT(0 == X.numWeakReferences());
            ASSERT(true == X.hasUniqueOwner());

            x.releaseRef();
            ASSERT(5 == TObj::getNumDeletes());
        }
      } break;
      case 3: {
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
            TObj* t = new (ta) TObj();

            ASSERT(++numAllocations == ta.numAllocations());

            Obj* xPtr = Obj::makeOutofplaceRep(t, &ta, &ta);
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

            ASSERT(1 == TObj::getNumDeletes());
            numDeallocations += 2;
            ASSERT(numDeallocations == ta.numDeallocations());
        }

        if (verbose) cout << endl
                        << "Testing 'releaseRef' with weak reference'" << endl
                        << "-----------------------------------------" << endl;

        {
            TObj* t = new (ta) TObj();

            ASSERT(++numAllocations == ta.numAllocations());

            Obj* xPtr = Obj::makeOutofplaceRep(t, &ta, &ta);
            Obj& x = *xPtr;
            Obj const& X = *xPtr;

            ASSERT(++numAllocations == ta.numAllocations());

            x.acquireWeakRef();
            x.releaseRef();

            ASSERT(0 == X.numReferences());
            ASSERT(1 == X.numWeakReferences());
            ASSERT(false == X.hasUniqueOwner());
            ASSERT(2 == TObj::getNumDeletes());
            ASSERT(++numDeallocations == ta.numDeallocations());

            x.releaseWeakRef();
            ASSERT(++numDeallocations == ta.numDeallocations());
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING BASIC CONSTRUCTOR
        //
        // Concerns:
        //   Object is properly initialized, and can be properly destructed
        //   when the last reference is released.
        //
        // Plan:
        //   Construct bcema_SharedPtrOutofplaceRep using each of the
        //   constructor and call releaseRef() to remove the last reference and
        //   check the that destructor fo the object is called.
        //
        // Testing:
        //   bcema_SharedPtrOutofplaceRep(
        //                    TYPE            *ptr,
        //                    const DELETER&   deleter,
        //                    bslma_Allocator *basicAllocator,
        //                    bslmf_MetaInt<DeleterType::BCEMA_ALLOCATOR_PTR>);
        //   bcema_SharedPtrOutofplaceRep<TYPE, DELETER> * makeOutofplaceRep(
        //                                TYPE             *ptr,
        //                                const DELETER&   deleter,
        //                                bslma_Allocator *basicAllocator = 0);
        //   void disposeObject();
        //   void disposeRep();
        //   void *originalPtr() const;
        //   TYPE *ptr() const;
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                         << "Testing 'disposeObject' and 'disposeRep'" << endl
                         << "========================================" << endl;

        numAllocations = ta.numAllocations();
        numDeallocations = ta.numDeallocations();
        {
            TObj* t = new (ta) TObj();
            Obj* xPtr = Obj::makeOutofplaceRep(t, &ta, &ta);
            Obj& x = *xPtr;
            Obj const& X = *xPtr;

            ASSERT(1 == X.numReferences());
            ASSERT(0 == X.numWeakReferences());
            ASSERT(x.ptr() ==  t);
            ASSERT(x.originalPtr() == (void*) t);

            x.disposeObject();
            ASSERT(++numDeallocations == ta.numDeallocations());
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
        numAllocations = ta.numAllocations();
        numDeallocations = ta.numDeallocations();
        {
            TObj* t = new (ta) TObj();
            Obj* xPtr = Obj::makeOutofplaceRep(t, &ta, &ta);
            Obj& x = *xPtr;
            Obj const& X = *xPtr;

            ASSERT(1 == X.numReferences());
            ASSERT(0 == X.numWeakReferences());
            ASSERT(x.ptr() ==  t);
            ASSERT(x.originalPtr() == (void*) t);

            x.acquireRef();
            ASSERT(2 == X.numReferences());
            ASSERT(0 == X.numWeakReferences());

            x.acquireWeakRef();
            ASSERT(2 == X.numReferences());
            ASSERT(1 == X.numWeakReferences());

            x.releaseRef();
            ASSERT(1 == X.numReferences());
            ASSERT(1 == X.numWeakReferences());

            x.releaseWeakRef();
            ASSERT(1 == X.numReferences());
            ASSERT(0 == X.numWeakReferences());

            ASSERT(0 == TObj::getNumDeletes());
            x.releaseRef();
            ASSERT(1 == TObj::getNumDeletes());

            numDeallocations += 2;
            ASSERT(numDeallocations == ta.numDeallocations());
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
