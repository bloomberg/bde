// bslstl_sharedptrallocateoutofplacerep.t.cpp                        -*-C++-*-
#include <bslstl_sharedptrallocateoutofplacerep.h>

#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bslma_testallocator.h>

#include <bsls_asserttest.h>
#include <bsls_bsltestutil.h>

#include <stdio.h>
#include <stdlib.h>             // 'atoi'

#include <new>

#ifdef BSLS_PLATFORM_CMP_MSVC  // Microsoft Compiler
#ifdef _MSC_EXTENSIONS         // Microsoft Extensions Enabled
#include <new>                 // if so, need to include new as well
#endif
#endif

using namespace BloombergLP;

//=============================================================================
//                                  TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// This test driver tests the functionality of out-of-place implementation of
// the shared pointer representation object.
//-----------------------------------------------------------------------------
// bslstl::SharedPtrAllocateOutofplaceRep
//---------------------------------------
// [ 2] SharedPtrAllocateOutofplaceRep(TYPE *p, const DEL& d, constALLOC& a>);
// [ 2] SharedPtrAllocateOutofplaceRep<TYPE, DELETER> * makeOutofplaceRep(...);
// [ 2] void disposeRep();
// [ 3] void disposeObject();
// [ 2] void *originalPtr() const;
// [ 2] TYPE *ptr() const;
//
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 5] USAGE EXAMPLE
//-----------------------------------------------------------------------------

//=============================================================================
//                      STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
int testStatus = 0;

namespace {

void aSsErT(bool b, const char *s, int i)
{
    if (b) {
        printf("Error " __FILE__ "(%d): %s    (failed)\n", i, s);
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}

}  // close unnamed namespace

//# define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

//=============================================================================
//                      STANDARD BDE TEST DRIVER MACROS
//-----------------------------------------------------------------------------

#define ASSERT       BSLS_BSLTESTUTIL_ASSERT
#define LOOP_ASSERT  BSLS_BSLTESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLS_BSLTESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLS_BSLTESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLS_BSLTESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLS_BSLTESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLS_BSLTESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLS_BSLTESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLS_BSLTESTUTIL_LOOP6_ASSERT
#define ASSERTV      BSLS_BSLTESTUTIL_ASSERTV

#define Q   BSLS_BSLTESTUTIL_Q   // Quote identifier literally.
#define P   BSLS_BSLTESTUTIL_P   // Print identifier and value.
#define P_  BSLS_BSLTESTUTIL_P_  // P(X) without '\n'.
#define T_  BSLS_BSLTESTUTIL_T_  // Print a tab (w/o newline).
#define L_  BSLS_BSLTESTUTIL_L_  // current Line number

// ============================================================================
//                  NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)
#define ASSERT_PASS(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
#define ASSERT_FAIL(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)
#define ASSERT_OPT_PASS(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS(EXPR)
#define ASSERT_OPT_FAIL(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL(EXPR)

#define ASSERT_SAFE_PASS_RAW(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS_RAW(EXPR)
#define ASSERT_SAFE_FAIL_RAW(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL_RAW(EXPR)
#define ASSERT_PASS_RAW(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS_RAW(EXPR)
#define ASSERT_FAIL_RAW(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL_RAW(EXPR)
#define ASSERT_OPT_PASS_RAW(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(EXPR)
#define ASSERT_OPT_FAIL_RAW(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(EXPR)

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

// TEST IMPLEMENTATION (defined below)
class MyTestObject;
class MyAllocTestDeleter;

//=============================================================================
//              GLOBAL HELPER CLASSES AND FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

template <class TYPE>
void deleterFunction(TYPE *ptr)
    // Delete the specified 'ptr', using 'delete ptr'.
{
    delete ptr;
}

                         // ===================
                         // struct BasicDeleter
                         // ===================

template <class TYPE>
struct BasicDeleter {
    // Basic deleter to simply forward a call to 'delete' with a pointer passed
    // to the function call operator.

    void operator()(TYPE *ptr) const
        // Delete the specified 'ptr', using 'delete ptr'.
    {
        delete ptr;
    }
};

                         // =====================
                         // class StatefulDeleter
                         // =====================


class StatefulDeleter {
    // Stateful deleter to simply forward a call to 'delete' with a pointer
    // passed to the overloaded function call operator, while counting the
    // number of calls to that operator.  The operator must not be 'const'
    // qualified in order to support the mutating state - which is a specific
    // testing concern.  This will also test the case that the overloaded
    // operator is a function member template.

private:
    int d_count;

public:
    // CREATORS
    StatefulDeleter();
        // Create a 'StatefulDeleter' object having a 'count' of 0.

    // MANIPULATORS
    template <class TYPE>
    void operator()(TYPE *ptr);
        // Delete the specified 'ptr' (using 'delete ptr') and increment the
        // stored 'count' of calls to this function.

    // ACCESSORS
    int count() const;
        // Return the number of time that 'operator()' has been called on this
        // object.
};


// CREATORS
StatefulDeleter::StatefulDeleter()
: d_count()
{
}

// MANIPULATORS
template <class TYPE>
void StatefulDeleter::operator()(TYPE *ptr)
{
    ++d_count;
    delete ptr;
}

// ACCESSORS
int StatefulDeleter::count() const
{
    return d_count;
}
                         // ==================
                         // class MyTestObject
                         // ==================
class MyTestObject{
    // This class provides an implementation for 'bslma::SharedPtrRep' so that
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
: d_data(0)
{
}

MyTestObject::~MyTestObject()
{
    ++d_deleteCounter;
}

// ACCESSORS
int MyTestObject::getNumDeletes()
{
    return d_deleteCounter;
}

int MyTestObject::d_deleteCounter = 0;

                         // ======================
                         // struct MyDeleteFunctor
                         // ======================

typedef BasicDeleter<MyTestObject> MyDeleteFunctor;

#if 0
                         // ========================
                         // class MyAllocTestDeleter
                         // ========================

class MyAllocTestDeleter {
    // This class provides a prototypical function-like deleter that takes a
    // 'bslma::Allocator' at construction.  It is used to check that the
    // allocator used to construct the representation is passed correctly to
    // the deleter.

    // DATA
    bslma::Allocator *d_allocator_p;
    bslma::Allocator *d_deleter_p;
    void             *d_someMemory;

  public:
//    BSLALG_DECLARE_NESTED_TRAITS(MyAllocTestDeleter,
//                                 bslalg::TypeTraitUsesBslmaAllocator);

  public:
    // CREATORS
    explicit MyAllocTestDeleter(bslma::Allocator *deleter,
                       bslma::Allocator *basicAllocator = 0);

    MyAllocTestDeleter(const MyAllocTestDeleter&  original,
                       bslma::Allocator          *basicAllocator = 0);

    ~MyAllocTestDeleter();

    // MANIPULATORS
    MyAllocTestDeleter& operator=(const MyAllocTestDeleter& rhs);

    // ACCESSORS
    template <class TYPE>
    void operator()(TYPE *ptr) const;
};

namespace BloombergLP {
namespace bslma {
template <>
struct UsesBslmaAllocator<MyAllocTestDeleter> : bsl::true_type {};
}  // close namespace bslma
}  // close namespace BloombergLP

                          // ------------------------
                          // class MyAllocTestDeleter
                          // ------------------------

// CREATORS
MyAllocTestDeleter::MyAllocTestDeleter(bslma::Allocator *deleter,
                                       bslma::Allocator *basicAllocator)
: d_allocator_p(bslma::Default::allocator(basicAllocator))
, d_deleter_p(deleter)
{
    d_someMemory = d_allocator_p->allocate(13);
}

MyAllocTestDeleter::MyAllocTestDeleter(
                                     const MyAllocTestDeleter&  original,
                                     bslma::Allocator          *basicAllocator)
: d_allocator_p(bslma::Default::allocator(basicAllocator))
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
                                                 const MyAllocTestDeleter& rhs)
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
#endif

#if 0
                         // ===================
                         // class MyTestFactory
                         // ===================
class MyTestFactory {
  public:
    // MANIPULATORS
    MyTestObject *createObject(bslma::Allocator *basicAllocator = 0)
    {
        // Create a 'MyTestObject' object.  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.
        return new MyTestObject();
    }

    void deleteObject(MyTestObject *object)
    {
        // Delete the specified 'object'.
        delete object;
    }
};
#endif


#if 0  // TBD Need an appropriately levelized usage example
                              // ================
                              // MySharedDatetime
                              // ================

class MySharedDatetime {
    // This class provide a reference counted smart pointer to support shared
    // ownership of a 'bdet_Datetime' object.

  private:
    bdet_Datetime      *d_ptr_p;  // pointer to the managed object
    bslma::SharedPtrRep *d_rep_p;  // pointer to the representation object

  public:
    // CREATORS
    explicit MySharedDatetime(bdet_Datetime    *ptr,
                              bslma::Allocator *basicAllocator = 0);
    MySharedDatetime(const MySharedDatetime& original);
    ~MySharedDatetime();

    // MANIPULATORS
    bdet_Datetime& operator*() const;
        // Dereference the shared Datetime

    bdet_Datetime *operator->() const;
        // Return address of the modifiable 'bdet_Datetime' referred to by this
        // class.

    bdet_Datetime *ptr() const;
        // Return address of the modifiable 'bdet_Datetime' referred to by this
        // class.
};

                              // ----------------
                              // MySharedDatetime
                              // ----------------

MySharedDatetime::MySharedDatetime(bdet_Datetime    *ptr,
                                   bslma::Allocator *basicAllocator)
{
    d_ptr_p = ptr;
    d_rep_p = bslstl::SharedPtrAllocateOutofplaceRep<bdet_Datetime,
                                                     bslma::Allocator *>::
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
#endif

                              // ==================
                              // class StdAllocator
                              // ==================

template <class TYPE>
class StdAllocator {
    // This class template conforms to the requirements of an allocator, as
    // specified in section 17.6.3.5 ([allocator.requirements]) of the ISO
    // C++11 standard.

  public:
    template <class OTHER>
    struct rebind {
        typedef StdAllocator<OTHER> other;
    };

    // CREATORS
    StdAllocator();
        // Create a 'StdAllocator' object.

    // StdAllocator(const StdAllocator<OTHER>& other) = default;
        // Create a 'StdAllocator' object from the specified 'other', using the
        // implicitly generated trivial copy constructor.  Note that as
        // 'StdAllocator' is an empty class, the 'other' object is not used.

    template <class OTHER>
    StdAllocator(const StdAllocator<OTHER>& other);
        // Create a 'StdAllocator' object from the specified 'other'.  Note
        // that as 'StdAllocator' is an empty class, the 'other' object is not
        // used.

    // ~StdAllocator() = default;
        // Destroy this object.

    // MANIPULATORS
    TYPE *allocate(size_t numObjects);
        // Allocate a contiguous block of memory large enough to hold the
        // specified 'numObjects' of (template parameter) 'TYPE' using the C
        // function 'malloc', and return the address of that block.

    void deallocate(TYPE *ptr, size_t numObjects);
};

                              // ------------------
                              // class StdAllocator
                              // ------------------

// CREATORS
template <class TYPE>
inline
StdAllocator<TYPE>::StdAllocator()
{
}

template <class TYPE>
template <class OTHER>
inline
StdAllocator<TYPE>::StdAllocator(const StdAllocator<OTHER>& /*other*/)
{
}

// MANIPULATORS
template <class TYPE>
inline
TYPE *StdAllocator<TYPE>::allocate(size_t numObjects)
{
    return static_cast<TYPE *>(malloc(numObjects * sizeof(TYPE)));
}

template <class TYPE>
inline
void StdAllocator<TYPE>::deallocate(TYPE *ptr, size_t numObjects)
{
    free(ptr);
}

// TYPEDEFS
typedef MyTestObject TObj;
typedef bslstl::SharedPtrAllocateOutofplaceRep<MyTestObject,
                                              MyDeleteFunctor,
                                              StdAllocator<MyTestObject> > Obj;
typedef void (*DeleteFunction)(MyTestObject *);

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int                test = argc > 1 ? atoi(argv[1]) : 0;
    int             verbose = argc > 2;
    int         veryVerbose = argc > 3;
    int     veryVeryVerbose = argc > 4;
    int veryVeryVeryVerbose = argc > 5;

    bslma::TestAllocator globalAllocator("global", veryVeryVeryVerbose);
    bslma::Default::setGlobalAllocator(&globalAllocator);

    // Confirm no static intialization locekd the global allocator
    ASSERT(&globalAllocator == bslma::Default::globalAllocator());

    bslma::TestAllocator defaultAllocator("default", veryVeryVeryVerbose);
    bslma::Default::setDefaultAllocator(&defaultAllocator);

    // Confirm no static intialization locked the default allocator
    ASSERT(&defaultAllocator == bslma::Default::defaultAllocator());

    bslma::TestAllocator ta(veryVeryVeryVerbose);
    bsls::Types::Int64 numDeallocations;
    bsls::Types::Int64 numAllocations;

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:  // Zero is always the leading case.
#if 0  // TBD Need an appropriately levelized usage example
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
#endif  // 0
#if 0   // we no longer have these constructors, primary ctor tested in case 2.
    case 4: {
        // --------------------------------------------------------------------
        // TESTING CONSTRUCTORS ACCESSOR
        //
        // Concerns:
        //   Object is properly initialized, and can be properly destructed
        //   when the last reference is released.
        //
        // Plan:
        //   Construct bslstl::SharedPtrAllocateOutofplaceRep using each of the
        //   constructor and call releaseRef() to remove the last reference and
        //   check the that destructor for the object is called.
        //
        // Testing:
        //   bslstl::SharedPtrAllocateOutofplaceRep(
        //                   TYPE             *ptr,
        //                   const DELETER&    deleter,
        //                   bslma::Allocator *basicAllocator,
        //                   bslmf::MetaInt<DeleterType::BCEMA_ALLOCATOR_PTR>);
        //   bslstl::SharedPtrAllocateOutofplaceRep(
        //                     TYPE             *ptr,
        //                     const DELETER&    deleter,
        //                     bslma::Allocator *basicAllocator,
        //                     bslmf::MetaInt<DeleterType::BCEMA_FACTORY_PTR>);
        //   bslstl::SharedPtrAllocateOutofplaceRep(
        //              TYPE             *ptr,
        //              const DELETER&    deleter,
        //              bslma::Allocator *basicAllocator,
        //              bslmf::MetaInt<DeleterType::BCEMA_FUNCTOR_WITH_ALLOC>);
        //   bslstl::SharedPtrAllocateOutofplaceRep(
        //           TYPE             *ptr,
        //           const DELETER&    deleter,
        //           bslma::Allocator *basicAllocator,
        //           bslmf::MetaInt<DeleterType::BCEMA_FUNCTOR_WITHOUT_ALLOC>);
        //   bslstl::SharedPtrAllocateOutofplaceRep<TYPE, DELETER> * makeOutofplaceRep(
        //                                TYPE             *ptr,
        //                                const DELETER&    deleter,
        //                                bslma::Allocator *basicAllocator=0);
        // --------------------------------------------------------------------

        if (verbose) printf("\nTesting Constructors and Destructor"
                            "\n===================================\n");

        if (verbose) printf("\nTesting bslma::AllocatorDeleter"
                            "\n-------------------------------\n");

        {
            TObj *t = new(ta) TObj();
            Obj *xPtr = Obj::makeOutofplaceRep(t, &ta, &ta);
            Obj& x = *xPtr;
            const Obj& X = x;

            ASSERT(1 == X.numReferences());
            ASSERT(0 == X.numWeakReferences());
            ASSERT(true == X.hasUniqueOwner());

            x.releaseRef();
            ASSERT(1 == TObj::getNumDeletes());
        }

        if (verbose) printf("\nTesting Factory Deleter"
                            "\n-----------------------\n");
        {
            enum { DELETER_TYPE = bslstl::
                           SharedPtrAllocateOutofplaceRep_DeleterDiscriminator<
                                                        MyTestFactory *>::VALUE
            };

            MyTestFactory *factory = new(ta) MyTestFactory();


            typedef bslstl::SharedPtrAllocateOutofplaceRep<MyTestObject,
                                                  MyTestFactory *> TestRep;
            TObj     *t    = factory->createObject();
            TestRep  *xPtr = TestRep::makeOutofplaceRep(t, factory, &ta);
            TestRep&  x    = *xPtr;   const TestRep& X = x;

            ASSERT(1 == X.numReferences());
            ASSERT(0 == X.numWeakReferences());
            ASSERT(true == X.hasUniqueOwner());

            x.releaseRef();
            ASSERT(2 == TObj::getNumDeletes());
            ta.deleteObject(factory);
        }

        if (verbose) printf("\nTesting Function Deleter"
                            "\n------------------------\n");
        {
            enum { DELETER_TYPE =
                    bslstl::SharedPtrAllocateOutofplaceRep_DeleterDiscriminator<
                                                        DeleteFunction> ::VALUE
            };

            typedef
                 bslstl::SharedPtrAllocateOutofplaceRep<MyTestObject,
                                                       DeleteFunction> TestRep;

            TObj *t = new TObj();
            TestRep *xPtr = TestRep::makeOutofplaceRep(t,
                                                       myDeleteFunction,
                                                       &ta);
            TestRep&  x    = *xPtr;   const TestRep& X = x;

            ASSERT(1 == X.numReferences());
            ASSERT(0 == X.numWeakReferences());
            ASSERT(true == X.hasUniqueOwner());

            x.acquireWeakRef();
            x.releaseRef();
            ASSERT(3 == TObj::getNumDeletes());
            x.releaseWeakRef();
        }

        if (verbose) printf("\nTesting Functor Deleter Without Alloc"
                            "\n-------------------------------------\n");
        {
            enum { DELETER_TYPE =
                    bslstl::SharedPtrAllocateOutofplaceRep_DeleterDiscriminator<
                                                        MyDeleteFunctor>::VALUE
            };

            typedef bslstl::SharedPtrAllocateOutofplaceRep<MyTestObject,
                                                  MyDeleteFunctor> TestRep;
            MyDeleteFunctor deleteFunctor;
            TObj     *t = new TObj();
            TestRep  *xPtr = TestRep::makeOutofplaceRep(t, deleteFunctor, &ta);
            TestRep&  x    = *xPtr;   const TestRep& X = x;

            ASSERT(1 == X.numReferences());
            ASSERT(0 == X.numWeakReferences());
            ASSERT(true == X.hasUniqueOwner());

            x.releaseRef();
            ASSERT(4 == TObj::getNumDeletes());
        }

        if (verbose) printf("\nTesting Functor Deleter With Alloc"
                            "\n----------------------------------\n");
        {
            enum { DELETER_TYPE =
                   bslstl::SharedPtrAllocateOutofplaceRep_DeleterDiscriminator<
                                                     MyAllocTestDeleter>::VALUE
            };

            TObj* t = new(ta) TObj();
            MyAllocTestDeleter deleteFunctor(&ta, &ta);

            typedef bslstl::SharedPtrAllocateOutofplaceRep<MyTestObject,
                                                  MyAllocTestDeleter>  TestRep;

            TestRep  *xPtr = TestRep::makeOutofplaceRep(t, deleteFunctor, &ta);
            TestRep&  x    = *xPtr;   const TestRep& X = x;

            ASSERT(1 == X.numReferences());
            ASSERT(0 == X.numWeakReferences());
            ASSERT(true == X.hasUniqueOwner());

            x.releaseRef();
            ASSERT(5 == TObj::getNumDeletes());
        }
      } break;
#endif  // 0
      case 3: {
        // --------------------------------------------------------------------
        // TESTING 'releaseRef' and 'releaseWeakRef'
        //
        // Concerns:
        //   1) 'releaseRef' and 'releaseWeakRef' is decrementing the reference
        //      count correctly.
        //   2) disposeObject() is called when there is no shared reference.
        //   3) disposeRep() is called only when there is no shared reference
        //      and no weak reference.
        //
        // Plan:
        //   1) Call 'acquireRef' then 'releaseRef' and verify 'numReference'
        //      did not change.  Call 'acquireWeakRef' then 'releaseWeakRef'
        //      and verify 'numWeakReference' did not change.
        //   2) Call 'releaseRef' when there is only one reference remaining.
        //      Then verify that both 'disposeObject' and 'disposeRep' is
        //      called.
        //   3) Create another object and call 'acquireWeakRef' before calling
        //      'releaseRef'.  Verify that only 'disposeObject' is called.
        //      Then call 'releaseWeakRef' and verify that 'disposeRep' is
        //      called.
        //
        // Testing:
        //   void releaseRef();
        //   void releaseWeakRef();
        // --------------------------------------------------------------------

        if (verbose) printf("\nTesting 'releaseRef' and 'releaseWeakRef'"
                            "\n=========================================\n");

        numAllocations = ta.numAllocations();
        numDeallocations = ta.numDeallocations();
        {
#if 0
            TObj *t = new (ta) TObj();

            ASSERT(++numAllocations == ta.numAllocations());

            Obj *xPtr = Obj::makeOutofplaceRep(t, &ta, &ta);
#else
            MyDeleteFunctor            deleter = {};
            StdAllocator<MyTestObject> allocator;

            TObj *t = allocator.allocate(1u);
            new(t) TObj();

            Obj *xPtr = Obj::makeOutofplaceRep(t, deleter, allocator);
#endif
            Obj& x = *xPtr;
            const Obj& X = *xPtr;

#if 0
            ASSERT(++numAllocations == ta.numAllocations());
#endif

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

            if (verbose) printf(
                        "\nTesting 'releaseRef' with no weak reference'"
                        "\n--------------------------------------------\n");

            x.releaseRef();

            ASSERT(1 == TObj::getNumDeletes());
#if 0
            numDeallocations += 2;
#endif
            ASSERT(numDeallocations == ta.numDeallocations());
        }

        if (verbose) printf("\nTesting 'releaseRef' with weak reference'"
                            "\n-----------------------------------------\n");

        {
#if 0
            TObj *t = new (ta) TObj();

            ASSERT(++numAllocations == ta.numAllocations());

            Obj *xPtr = Obj::makeOutofplaceRep(t, &ta, &ta);
#else
            MyDeleteFunctor            deleter = {};
            StdAllocator<MyTestObject> allocator;

            TObj *t = allocator.allocate(1u);
            new(t) TObj();

            Obj *xPtr = Obj::makeOutofplaceRep(t, deleter, allocator);
#endif
            Obj& x = *xPtr;
            const Obj& X = *xPtr;

#if 0
            ASSERT(++numAllocations == ta.numAllocations());
#endif

            x.acquireWeakRef();
            x.releaseRef();

            ASSERT(0 == X.numReferences());
            ASSERT(1 == X.numWeakReferences());
            ASSERT(false == X.hasUniqueOwner());
            ASSERT(2 == TObj::getNumDeletes());
#if 0
            ASSERT(++numDeallocations == ta.numDeallocations());
#endif

            x.releaseWeakRef();
#if 0
            ASSERT(++numDeallocations == ta.numDeallocations());
#endif
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
        //   Construct bslstl::SharedPtrAllocateOutofplaceRep using each of the
        //   constructor and call releaseRef() to remove the last reference and
        //   check the that destructor for the object is called.
        //
        // Testing:
        //   SharedPtrAllocateOutofplaceRep(TYPE *, const DEL&, const ALLOC&);
        //   SharedPtrAllocateOutofplaceRep *makeOutofplaceRep(T *, DEL, AL);
        //   void disposeObject();
        //   void disposeRep();
        //   void *originalPtr() const;
        //   TYPE *ptr() const;
        // --------------------------------------------------------------------

        if (verbose) printf("\nTesting 'disposeObject' and 'disposeRep'"
                            "\n========================================\n");

        numAllocations = ta.numAllocations();
        numDeallocations = ta.numDeallocations();
        {
#if 0
            TObj *t = new (ta) TObj();
            Obj *xPtr = Obj::makeOutofplaceRep(t, &ta, &ta);
#else
            MyDeleteFunctor            deleter = {};
            StdAllocator<MyTestObject> allocator;

            TObj *t = allocator.allocate(1u);
            new(t) TObj();

            Obj *xPtr = Obj::makeOutofplaceRep(t, deleter, allocator);
#endif

            Obj& x = *xPtr;
            const Obj& X = *xPtr;

            ASSERT(1 == X.numReferences());
            ASSERT(0 == X.numWeakReferences());
            ASSERT(x.ptr() ==  t);
            ASSERT(x.originalPtr() == static_cast<void *>(t));

            x.disposeObject();
#if 0
            ASSERT(++numDeallocations == ta.numDeallocations());
#endif
            x.disposeRep();
#if 0
            ASSERT(++numDeallocations == ta.numDeallocations());
#endif
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //
        // Testing:
        //   This test exercises basic functionality but tests nothing.
        // --------------------------------------------------------------------

        if (verbose) printf("\nBREATHING TEST"
                            "\n==============\n");

        numAllocations = ta.numAllocations();
        numDeallocations = ta.numDeallocations();
        {
#if 0
            TObj *t = new (ta) TObj();
            Obj *xPtr = Obj::makeOutofplaceRep(t, &ta, &ta);
#else
            MyDeleteFunctor            deleter = {};
            StdAllocator<MyTestObject> allocator;

            TObj *t = allocator.allocate(1u);
            new(t) TObj();

            Obj *xPtr = Obj::makeOutofplaceRep(t, deleter, allocator);
#endif
            Obj& x = *xPtr;
            const Obj& X = *xPtr;

            ASSERT(1 == X.numReferences());
            ASSERT(0 == X.numWeakReferences());
            ASSERT(x.ptr() ==  t);
            ASSERT(x.originalPtr() == static_cast<void *>(t));

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

#if 0
            numDeallocations += 2;
#endif
            ASSERT(numDeallocations == ta.numDeallocations());
        }
      } break;
      default: {
        fprintf(stderr, "WARNING: CASE `%d' NOT FOUND.\n", test);
        testStatus = -1;
      }
    }

    // CONCERN: In no case does memory come from the global allocator.

    LOOP_ASSERT(globalAllocator.numBlocksTotal(),
                0 == globalAllocator.numBlocksTotal());

    if (testStatus > 0) {
        fprintf(stderr, "Error, non-zero test status = %d.\n", testStatus);
    }
    return testStatus;
}

// ----------------------------------------------------------------------------
// Copyright (C) 2013 Bloomberg L.P.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
// ----------------------------- END-OF-FILE ----------------------------------
