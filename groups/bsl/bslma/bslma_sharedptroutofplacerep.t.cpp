// bslma_sharedptroutofplacerep.t.cpp                                 -*-C++-*-
#include <bslma_sharedptroutofplacerep.h>

#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bslma_destructionutil.h>
#include <bslma_testallocator.h>

#include <bslmf_assert.h>
#include <bslmf_nestedtraitdeclaration.h>

#include <bsls_asserttest.h>
#include <bsls_bsltestutil.h>

#include <cstddef>      // std::size_t

#include <stdio.h>      // 'printf'
#include <stdlib.h>     // 'atoi'

#ifdef BSLS_PLATFORM_CMP_MSVC  // Microsoft Compiler
#ifdef _MSC_EXTENSIONS         // Microsoft Extensions Enabled
#include <new>                 // if so, need to include new as well
#endif
#endif

// These warning suppressions will be eliminated in due course.
// BDE_VERIFY pragma: -FD01  // Test-machinery lacks a contract
// BDE_VERIFY pragma: -TP06  // Test-case indexing thing
// BDE_VERIFY pragma: -TP09  // Test-case indexing thing
// BDE_VERIFY pragma: -TP18  // Test-case banners are ALL-CAPS

using namespace BloombergLP;

//=============================================================================
//                                  TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// This test driver tests the functionality of out-of-place implementation of
// the shared pointer representation object.
//-----------------------------------------------------------------------------
// bslma::SharedPtrRep
//--------------------
// [ 4] SharedPtrOutofplaceRep(TYPE *ptr, const...BCEMA_ALLOCATOR_PTR>);
// [ 4] SharedPtrOutofplaceRep(TYPE *ptr, const...BCEMA_FACTORY_PTR>);
// [ 4] SharedPtrOutofplaceRep(TYPE *ptr, ...BCEMA_FUNCTOR_WITH_ALLOC>);
// [ 4] SharedPtrOutofplaceRep(TYPE ...BCEMA_FUNCTOR_WITH_ALLOC_ARG_T>);
// [ 4] SharedPtrOutofplaceRep(TYPE *pt...BCEMA_FUNCTOR_WITHOUT_ALLOC>);
// [ 4] SharedPtrOutofplaceRep<TYPE, DELETER> *makeOutofplaceRep(...);
// [ 2] void disposeRep();
// [ 2] void disposeObject();
// [  ] void *getDeleter(const std::type_info& type);
// [ 2] void *originalPtr() const;
// [ 2] TYPE *ptr() const;
//
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 5] USAGE EXAMPLE
// [ 2] BOOTSTRAP SharedPtrOutofplaceRep(...);
// [ 2] BOOTSTRAP makeOutofplaceRep(...);
// [ 3] void releaseRef();
// [ 3] void releaseWeakRef();

//-----------------------------------------------------------------------------

// ============================================================================
//                     STANDARD BSL ASSERT TEST FUNCTION
// ----------------------------------------------------------------------------

namespace {

int testStatus = 0;

void aSsErT(bool condition, const char *message, int line)
{
    if (condition) {
        printf("Error " __FILE__ "(%d): %s    (failed)\n", line, message);

        if (0 <= testStatus && testStatus <= 100) {
            ++testStatus;
        }
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

// TYPEDEFS
typedef bslma::SharedPtrOutofplaceRep<MyTestObject, bslma::Allocator *> Obj;
typedef MyTestObject TObj;
typedef void (*DeleteFunction)(MyTestObject *);

//=============================================================================
//              GLOBAL HELPER CLASSES AND FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

                          // ========================
                          // class MyBslTestAllocator
                          // ========================

template <class TYPE>
class MyBslTestAllocator {
    // This class meets enough of the standard 'Allocator' requirements in
    // order to test the out-of-place creation apparatus with such allocators
    // that are convertible from 'bslma::Allocator *' (e.g. 'bsl::allocator').

    BSLMF_ASSERT((!bsl::is_same<TYPE, void>::value));
        // 'Allocator' implementations with a 'void' 'value_type' have to
        // provide a different interface.  Support for this is not necessary
        // for this test driver.

  public:
    // TYPES
    typedef TYPE value_type;

  private:
    // DATA
    bslma::Allocator *d_allocator_p;

  public:
    // CREATORS
    MyBslTestAllocator();

    MyBslTestAllocator(bslma::Allocator *allocator);                // IMPLICIT

    MyBslTestAllocator(const MyBslTestAllocator& original);

    template <class BDE_OTHER_TYPE>
    MyBslTestAllocator(const MyBslTestAllocator<BDE_OTHER_TYPE>& original);

    // MANIPULATORS
    TYPE *allocate(std::size_t n);

    void deallocate(TYPE *pointer, std::size_t n);

    template <class ELEMENT_TYPE>
    void destroy(ELEMENT_TYPE *address);
};

                          // ------------------------
                          // class MyBslTestAllocator
                          // ------------------------

// CREATORS
template <class TYPE>
MyBslTestAllocator<TYPE>::MyBslTestAllocator()
: d_allocator_p(bslma::Default::allocator())
{
}

template <class TYPE>
MyBslTestAllocator<TYPE>::MyBslTestAllocator(bslma::Allocator *allocator)
: d_allocator_p(allocator)
{
}

template <class TYPE>
MyBslTestAllocator<TYPE>::MyBslTestAllocator(
                                            const MyBslTestAllocator& original)
: d_allocator_p(original.d_allocator_p)
{
}

template <class TYPE>
template <class BDE_OTHER_TYPE>
MyBslTestAllocator<TYPE>::MyBslTestAllocator(
                            const MyBslTestAllocator<BDE_OTHER_TYPE>& original)
: d_allocator_p(original.d_allocator_p)
{
}

// MANIPULATORS
template <class TYPE>
TYPE *MyBslTestAllocator<TYPE>::allocate(std::size_t n)
{
    return static_cast<TYPE *>(d_allocator_p->allocate(n * sizeof(TYPE)));
}

template <class TYPE>
void MyBslTestAllocator<TYPE>::deallocate(TYPE *pointer, std::size_t)
{
    d_allocator_p->deallocate(pointer);
}


template <class TYPE>
template <class ELEMENT_TYPE>
void MyBslTestAllocator<TYPE>::destroy(ELEMENT_TYPE *address)
{
    bslma::DestructionUtil::destroy(address);
}

                         // ==================
                         // class MyTestObject
                         // ==================
class MyTestObject {
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

struct MyDeleteFunctor {
    // This 'struct' provides an 'operator()' that can be used to delete a
    // 'MyTestObject' object.

  public:
    void operator()(MyTestObject *object)
    {
        // Destroy the specified 'object'.
        delete object;
    }
};

void myDeleteFunction(MyTestObject *object)
{
    // Delete the specified 'object'.
    delete object;
}

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
    void             *d_memory_p;

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(MyAllocTestDeleter,
                                   bslma::UsesBslmaAllocator);

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

                          // ------------------------
                          // class MyAllocTestDeleter
                          // ------------------------

// CREATORS
MyAllocTestDeleter::MyAllocTestDeleter(bslma::Allocator *deleter,
                                       bslma::Allocator *basicAllocator)
: d_allocator_p(bslma::Default::allocator(basicAllocator))
, d_deleter_p(deleter)
{
    d_memory_p = d_allocator_p->allocate(13);
}

MyAllocTestDeleter::MyAllocTestDeleter(
                                     const MyAllocTestDeleter&  original,
                                     bslma::Allocator          *basicAllocator)
: d_allocator_p(bslma::Default::allocator(basicAllocator))
, d_deleter_p(original.d_deleter_p)
{
    d_memory_p = d_allocator_p->allocate(13);
}

MyAllocTestDeleter::~MyAllocTestDeleter()
{
    d_allocator_p->deallocate(d_memory_p);
}

// MANIPULATORS
MyAllocTestDeleter& MyAllocTestDeleter::operator=(
                                                 const MyAllocTestDeleter& rhs)
{
    ASSERT(!"'MyAllocTestDeleter::operator=' should not be used.");
    d_deleter_p = rhs.d_deleter_p;
    return *this;
}

// ACCESSORS
template <class TYPE>
void MyAllocTestDeleter::operator()(TYPE *ptr) const
{
    d_deleter_p->deleteObject(ptr);
}

                        // ===========================
                        // class MyAllocArgTestDeleter
                        // ===========================

class MyAllocArgTestDeleter {

    // DATA
    bslma::Allocator *d_allocator_p;
    bslma::Allocator *d_deleter_p;
    void             *d_memory_p;

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(MyAllocArgTestDeleter,
                                   bslma::UsesBslmaAllocator);

    BSLMF_NESTED_TRAIT_DECLARATION(MyAllocArgTestDeleter,
                                   bslmf::UsesAllocatorArgT);

    // CREATORS
    MyAllocArgTestDeleter(bsl::allocator_arg_t,
                          bslma::Allocator *basicAllocator,
                          bslma::Allocator *deleter);

    MyAllocArgTestDeleter(const MyAllocArgTestDeleter& original);

    MyAllocArgTestDeleter(bsl::allocator_arg_t,
                          bslma::Allocator             *basicAllocator,
                          const MyAllocArgTestDeleter&  original);

    ~MyAllocArgTestDeleter();

    // MANIPULATORS
    MyAllocArgTestDeleter& operator=(const MyAllocArgTestDeleter& rhs);

    // ACCESSORS
    template <class TYPE>
    void operator()(TYPE *ptr) const;
};

                        // ---------------------------
                        // class MyAllocArgTestDeleter
                        // ---------------------------

// CREATORS
MyAllocArgTestDeleter::MyAllocArgTestDeleter(
                                          bsl::allocator_arg_t,
                                          bslma::Allocator     *basicAllocator,
                                          bslma::Allocator     *deleter)
: d_allocator_p(bslma::Default::allocator(basicAllocator))
, d_deleter_p(deleter)
, d_memory_p(d_allocator_p->allocate(13))
{
}

MyAllocArgTestDeleter::MyAllocArgTestDeleter(
                                         const MyAllocArgTestDeleter& original)
: d_allocator_p(original.d_allocator_p)
, d_deleter_p(original.d_deleter_p)
, d_memory_p(d_allocator_p->allocate(13))
{
    ASSERT(!"'MyAllocArgTestDeleter::MyAllocArgTestDeleter(original)' should"
            " not be used.");
}

MyAllocArgTestDeleter::MyAllocArgTestDeleter(
                                  bsl::allocator_arg_t,
                                  bslma::Allocator             *basicAllocator,
                                  const MyAllocArgTestDeleter&  original)
: d_allocator_p(bslma::Default::allocator(basicAllocator))
, d_deleter_p(original.d_deleter_p)
, d_memory_p(d_allocator_p->allocate(13))
{
}

MyAllocArgTestDeleter::~MyAllocArgTestDeleter()
{
    d_allocator_p->deallocate(d_memory_p);
}

// MANIPULATORS
MyAllocArgTestDeleter& MyAllocArgTestDeleter::operator=(
                                              const MyAllocArgTestDeleter& rhs)
{
    ASSERT(!"'MyAllocArgTestDeleter::operator=' should not be used.");
    d_deleter_p = rhs.d_deleter_p;
    return *this;
}

// ACCESSORS
template <class TYPE>
void MyAllocArgTestDeleter::operator()(TYPE *ptr) const
{
    d_deleter_p->deleteObject(ptr);
}

                       // ==============================
                       // class MyBslAllocArgTestDeleter
                       // ==============================

class MyBslAllocArgTestDeleter {

  public:
    // TYPES
    typedef MyBslTestAllocator<char> allocator_type;

  private:
    // DATA
    allocator_type    d_allocator;
    bslma::Allocator *d_deleter_p;
    void             *d_memory_p;

  public:
    // CREATORS
    MyBslAllocArgTestDeleter(bsl::allocator_arg_t,
                             const allocator_type&  allocator,
                             bslma::Allocator      *deleter);

    MyBslAllocArgTestDeleter(const MyBslAllocArgTestDeleter& original);

    MyBslAllocArgTestDeleter(bsl::allocator_arg_t,
                             const allocator_type&           allocator,
                             const MyBslAllocArgTestDeleter& original);

    ~MyBslAllocArgTestDeleter();

    // MANIPULATORS
    MyBslAllocArgTestDeleter& operator=(const MyBslAllocArgTestDeleter& rhs);

    // ACCESSORS
    template <class TYPE>
    void operator()(TYPE *ptr) const;
};

namespace BloombergLP {
namespace bslma {
template <>
struct UsesBslmaAllocator<MyBslAllocArgTestDeleter> : bsl::true_type {
};
}  // close package namespace

namespace bslmf {
template <>
struct UsesAllocatorArgT<MyBslAllocArgTestDeleter> : bsl::true_type {
};
}  // close namespace bslmf
}  // close enterprise namespace

                       // ------------------------------
                       // class MyBslAllocArgTestDeleter
                       // ------------------------------

// CREATORS
MyBslAllocArgTestDeleter::MyBslAllocArgTestDeleter(
                                              bsl::allocator_arg_t,
                                              const allocator_type&  allocator,
                                              bslma::Allocator      *deleter)
: d_allocator(allocator)
, d_deleter_p(deleter)
, d_memory_p(d_allocator.allocate(13))
{
}

MyBslAllocArgTestDeleter::MyBslAllocArgTestDeleter(
                                      const MyBslAllocArgTestDeleter& original)
: d_allocator()
, d_deleter_p(original.d_deleter_p)
, d_memory_p(d_allocator.allocate(13))
{
    ASSERT(!"'MyBslAllocArgTestDeleter::MyBslAllocArgTestDeleter(original)'"
            " should not be used.");
}

MyBslAllocArgTestDeleter::MyBslAllocArgTestDeleter(
                                     bsl::allocator_arg_t,
                                     const allocator_type&           allocator,
                                     const MyBslAllocArgTestDeleter& original)
: d_allocator(allocator)
, d_deleter_p(original.d_deleter_p)
, d_memory_p(d_allocator.allocate(13))
{
}

MyBslAllocArgTestDeleter::~MyBslAllocArgTestDeleter()
{
    d_allocator.deallocate(static_cast<char *>(d_memory_p), 13);
}

// MANIPULATORS
MyBslAllocArgTestDeleter& MyBslAllocArgTestDeleter::operator=(
                                           const MyBslAllocArgTestDeleter& rhs)
{
    ASSERT(!"'MyBslAllocArgTestDeleter::operator=' should not be used.");
    return *this;
}

// ACCESSORS
template <class TYPE>
void MyBslAllocArgTestDeleter::operator()(TYPE *ptr) const
{
    d_deleter_p->deleteObject(ptr);
}

                         // ===================
                         // class MyTestFactory
                         // ===================
class MyTestFactory {
  public:
    // MANIPULATORS
    MyTestObject *createObject()
    {
        // Dynamically allocate a new 'MyTestObject'.
        return new MyTestObject();
    }

    void deleteObject(MyTestObject *object)
    {
        // Delete the specified 'object'.
        delete object;
    }
};

#if 0  // TBD Need an appropriately levelized usage example
                              // ================
                              // MySharedDatetime
                              // ================

class MySharedDatetime {
    // This class provide a reference counted smart pointer to support shared
    // ownership of a 'bdlt::Datetime' object.

  private:
    bdlt::Datetime      *d_ptr_p;  // pointer to the managed object
    bslma::SharedPtrRep *d_rep_p;  // pointer to the representation object

  public:
    // CREATORS
    explicit MySharedDatetime(bdlt::Datetime   *ptr,
                              bslma::Allocator *basicAllocator = 0);
    MySharedDatetime(const MySharedDatetime& original);
    ~MySharedDatetime();

    // MANIPULATORS
    bdlt::Datetime& operator*() const;
        // Dereference the shared Datetime

    bdlt::Datetime *operator->() const;
        // Return address of the modifiable 'bdlt::Datetime' referred to by
        // this class.

    bdlt::Datetime *ptr() const;
        // Return address of the modifiable 'bdlt::Datetime' referred to by
        // this class.
};

                              // ----------------
                              // MySharedDatetime
                              // ----------------

MySharedDatetime::MySharedDatetime(bdlt::Datetime   *ptr,
                                   bslma::Allocator *basicAllocator)
{
    d_ptr_p = ptr;
    d_rep_p = bslma::SharedPtrOutofplaceRep<bdlt::Datetime,
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

bdlt::Datetime& MySharedDatetime::operator*() const {
    return *d_ptr_p;
}

bdlt::Datetime *MySharedDatetime::operator->() const {
    return d_ptr_p;
}

bdlt::Datetime *MySharedDatetime::ptr() const {
    return d_ptr_p;
}
#endif

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int                 test = argc > 1 ? atoi(argv[1]) : 0;
    bool             verbose = argc > 2;
    bool         veryVerbose = argc > 3;
    bool     veryVeryVerbose = argc > 4;
    bool veryVeryVeryVerbose = argc > 5;

    (void)veryVerbose;           // suppress unused variable warning
    (void)veryVeryVerbose;       // suppress unused variable warning

    bslma::TestAllocator globalAllocator("global", veryVeryVeryVerbose);
    bslma::Default::setGlobalAllocator(&globalAllocator);

    // Confirm no static initialization locked the global allocator
    ASSERT(&globalAllocator == bslma::Default::globalAllocator());

    bslma::TestAllocator defaultAllocator("default", veryVeryVeryVerbose);
    ASSERT(0 == bslma::Default::setDefaultAllocator(&defaultAllocator));

    // Confirm no static initialization locked the default allocator
    ASSERT(&defaultAllocator == bslma::Default::defaultAllocator());

    bslma::TestAllocator ta(veryVeryVeryVerbose);
    bsls::Types::Int64 numDeallocations;
    bsls::Types::Int64 numAllocations;

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:  // Zero is always the leading case.
#if 0  // TBD Need an appropriately levelized usage example
      case 5: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //
        // Concerns:
        //: 1 The usage example provided in the component header file compiles,
        //:   links, and runs as shown.
        //
        // Plan:
        //: 1 Incorporate usage example from header into test driver, remove
        //:   leading comment characters, and replace 'assert' with 'ASSERT'.
        //:   (C-1)
        //
        // Testing: USAGE EXAMPLE
        // --------------------------------------------------------------------
        {
            ASSERT(0 == ta.numAllocations());
            MySharedDatetime dt1(new(ta) bdlt::Datetime(2011, 1, 1), &ta);

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
#endif
      case 4: {
        // --------------------------------------------------------------------
        // TESTING CREATORS
        //
        // Concerns:
        //: 1 Object is properly initialized, and can be properly destroyed
        //:   when the last reference is released.
        //
        // Plan:
        //: 1 Construct a 'bslma::SharedPtrOutofplaceRep' using each
        //:   constructor overload, calling 'releaseRef' to remove the last
        //:   reference.  Then check the deleter is called to destroy the
        //:   object.
        //
        // Testing:
        //   SharedPtrOutofplaceRep<TYPE, DELETER> *makeOutofplaceRep(...);
        //   SharedPtrOutofplaceRep(TYPE *ptr, const...BCEMA_ALLOCATOR_PTR>);
        //   SharedPtrOutofplaceRep(TYPE *ptr, const...BCEMA_FACTORY_PTR>);
        //   SharedPtrOutofplaceRep(TYPE *ptr, ...BCEMA_FUNCTOR_WITH_ALLOC>);
        //   SharedPtrOutofplaceRep(TYPE ...BCEMA_FUNCTOR_WITH_ALLOC_ARG_T>);
        //   SharedPtrOutofplaceRep(TYPE *pt...BCEMA_FUNCTOR_WITHOUT_ALLOC>);
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING CREATORS"
                            "\n================\n");

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
            MyTestFactory factory = MyTestFactory();


            typedef bslma::SharedPtrOutofplaceRep<MyTestObject,
                                                  MyTestFactory *> TestRep;
            TObj     *t    = factory.createObject();
            TestRep  *xPtr = TestRep::makeOutofplaceRep(t, &factory, &ta);
            TestRep&  x    = *xPtr;   const TestRep& X = x;

            ASSERT(1 == X.numReferences());
            ASSERT(0 == X.numWeakReferences());
            ASSERT(true == X.hasUniqueOwner());

            x.releaseRef();
            ASSERT(2 == TObj::getNumDeletes());
        }

        if (verbose) printf("\nTesting Function Deleter"
                            "\n------------------------\n");
        {
            typedef bslma::SharedPtrOutofplaceRep<MyTestObject, DeleteFunction>
                                                                       TestRep;

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
            typedef bslma::SharedPtrOutofplaceRep<MyTestObject,
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
            TObj* t = new(ta) TObj();
            MyAllocTestDeleter deleteFunctor(&ta, &ta);

            typedef bslma::SharedPtrOutofplaceRep<MyTestObject,
                                                  MyAllocTestDeleter>  TestRep;

            TestRep  *xPtr = TestRep::makeOutofplaceRep(t, deleteFunctor, &ta);
            TestRep&  x    = *xPtr;   const TestRep& X = x;

            ASSERT(1 == X.numReferences());
            ASSERT(0 == X.numWeakReferences());
            ASSERT(true == X.hasUniqueOwner());

            x.releaseRef();
            ASSERT(5 == TObj::getNumDeletes());
        }

        if (verbose) printf("\nTesting Functor Deleter With Alloc Arg"
                            "\n--------------------------------------\n");
        {
            TObj* t = new(ta) TObj();
            MyAllocArgTestDeleter deleteFunctor(bsl::allocator_arg, &ta, &ta);

            typedef bslma::SharedPtrOutofplaceRep<MyTestObject,
                                                  MyAllocArgTestDeleter>
                TestRep;

            TestRep  *xPtr = TestRep::makeOutofplaceRep(t, deleteFunctor, &ta);
            TestRep&  x    = *xPtr;   const TestRep& X = x;

            ASSERT(1 == X.numReferences());
            ASSERT(0 == X.numWeakReferences());
            ASSERT(true == X.hasUniqueOwner());

            x.releaseRef();
            ASSERT(6 == TObj::getNumDeletes());
        }

        if (verbose) printf("\nTesting Functor Deleter With BSL Alloc Arg"
                            "\n------------------------------------------\n");
        {
            TObj* t = new(ta) TObj();
            MyBslAllocArgTestDeleter  deleteFunctor(bsl::allocator_arg,
                                                    &ta,
                                                    &ta);

            typedef bslma::SharedPtrOutofplaceRep<MyTestObject,
                                                  MyBslAllocArgTestDeleter>
                TestRep;

            TestRep  *xPtr = TestRep::makeOutofplaceRep(t, deleteFunctor, &ta);
            TestRep&  x    = *xPtr;   const TestRep& X = x;

            ASSERT(1 == X.numReferences());
            ASSERT(0 == X.numWeakReferences());
            ASSERT(true == X.hasUniqueOwner());

            x.releaseRef();
            ASSERT(7 == TObj::getNumDeletes());
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING 'releaseRef' AND 'releaseWeakRef'
        //   Note that this test is essentially reconfirming the base class
        //   implementation has not been broken by this derived class; these
        //   non-virtual methods should be fully tested already by the base
        //   class.
        //
        // Concerns:
        //: 1 'releaseRef' and 'releaseWeakRef' decrement the reference count
        //:   correctly.
        //: 2 'disposeObject' is called when there is no shared reference.
        //: 3 'disposeRep' is called only when there is no shared reference
        //:   and no weak reference.
        //
        // Plan:
        //: 1 Call 'acquireRef' then 'releaseRef' and verify 'numReference' did
        //:   not change.  Call 'acquireWeakRef' then 'releaseWeakRef' and
        //:   verify 'numWeakReference' did not change.
        //: 2 Call 'releaseRef' when there is only one reference remaining.
        //:   Then verify that both 'disposeObject' and 'disposeRep' is called.
        //: 3 Create another object and call 'acquireWeakRef' before calling
        //:   'releaseRef'.  Verify that only 'disposeObject' is called.  Then
        //:   call 'releaseWeakRef' and verify that 'disposeRep' is called.
        //
        // Testing:
        //   void releaseRef();
        //   void releaseWeakRef();
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING 'releaseRef' AND 'releaseWeakRef'"
                            "\n=========================================\n");

        numAllocations = ta.numAllocations();
        numDeallocations = ta.numDeallocations();
        {
            TObj *t = new (ta) TObj();

            ASSERT(++numAllocations == ta.numAllocations());

            Obj *xPtr = Obj::makeOutofplaceRep(t, &ta, &ta);
            Obj& x = *xPtr;
            const Obj& X = *xPtr;

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

            if (verbose) printf(
                        "\nTesting 'releaseRef' with no weak reference'"
                        "\n--------------------------------------------\n");

            x.releaseRef();

            ASSERT(1 == TObj::getNumDeletes());
            numDeallocations += 2;
            ASSERT(numDeallocations == ta.numDeallocations());
        }

        if (verbose) printf("\nTesting 'releaseRef' with weak reference'"
                            "\n-----------------------------------------\n");

        {
            TObj *t = new (ta) TObj();

            ASSERT(++numAllocations == ta.numAllocations());

            Obj *xPtr = Obj::makeOutofplaceRep(t, &ta, &ta);
            Obj& x = *xPtr;
            const Obj& X = *xPtr;

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
        //: 1 Object is properly initialized, and can be properly destroyed
        //:   when the last reference is released.
        //
        // Plan:
        //: 1 Construct a 'bslma::SharedPtrOutofplaceRep' using the preferred
        //:   constructor.  Then call 'releaseRef' to remove the last reference
        //:   and check the that the destructor for the held object is called.
        //
        // Testing:
        //   BOOTSTRAP SharedPtrOutofplaceRep(...);
        //   BOOTSTRAP makeOutofplaceRep(...);
        //   void disposeObject();
        //   void disposeRep();
        //   void *originalPtr() const;
        //   TYPE *ptr() const;
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING BASIC CONSTRUCTOR"
                            "\n=========================\n");

        if (verbose) printf("\nTesting 'disposeObject' and 'disposeRep'"
                            "\n========================================\n");

        numAllocations = ta.numAllocations();
        numDeallocations = ta.numDeallocations();
        {
            TObj *t = new (ta) TObj();
            Obj *xPtr = Obj::makeOutofplaceRep(t, &ta, &ta);
            Obj& x = *xPtr;
            const Obj& X = *xPtr;

            ASSERT(1 == X.numReferences());
            ASSERT(0 == X.numWeakReferences());
            ASSERT(x.ptr() ==  t);
            ASSERT(x.originalPtr() == static_cast<void *>(t));

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
        // Concerns:
        //: 1 This test exercises basic functionality but tests nothing.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) printf("\nBREATHING TEST"
                            "\n==============\n");

        numAllocations = ta.numAllocations();
        numDeallocations = ta.numDeallocations();
        {
            TObj *t = new (ta) TObj();
            Obj *xPtr = Obj::makeOutofplaceRep(t, &ta, &ta);
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

            numDeallocations += 2;
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
