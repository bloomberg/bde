// bcema_sharedptr.t.cpp       -*-C++-*-

#include <bcema_sharedptr.h>

#include <bcema_testallocator.h>                // for testing only
#include <bcemt_thread.h>                       // for testing only
#include <bcemt_lockguard.h>                    // for testing only
#include <bslalg_scalarprimitives.h>            // for testing only
#include <bdef_bind_test.h>                     // for testing only
#include <bdef_function.h>                      // for testing only
#include <bdema_managedptr.h>                   // for testing only
#include <bslma_testallocator.h>                // for testing only
#include <bslma_defaultallocatorguard.h>        // for testing only
#include <bslmf_isconvertible.h>                // for testing only
#include <bsls_stopwatch.h>                     // for testing only

#include <bsls_alignmentfromtype.h>
#include <bsls_alignmentutil.h>
#include <bsls_assert.h>

#include <bsl_iostream.h>
#include <bsl_map.h>
#include <bsl_new.h>          // placement syntax
#include <bsl_vector.h>

#include <bsl_cstdlib.h>      // atoi()
#include <bsl_cstring.h>      // strcmp, strcpy

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
// This test driver tests the functionality of a complex mechanism with pointer
// semantics.  It is somewhat thorough but still incomplete: among the untested
// concerns:
// - The test plans are still to be written (so marked TBD).
// - There is currently no testing of exception-safety behavior, even though
//   this is a documented concern (e.g., in the createInplace function-level
//   documentation) and there is code written specifically for avoiding memory
//   leaks in the constructors (untested).
// - The usage example is also untested.
//-----------------------------------------------------------------------------
// bcema_SharedPtrOutofplaceRep
//------------------------
// [17] void *originalPtr() const;
// [16] bcema_SharedPtrOutofplaceRep(...);
//
// bcema_SharedPtr
//----------------
// [ 2] bcema_SharedPtr();
// [ 2] bcema_SharedPtr(TYPE *ptr, bslma_Allocator *basicAllocator=0);
// [ 3] bcema_SharedPtr(PTRTYPE *ptr, bslma_Allocator *basicAllocator=0)
// [ 3] bcema_SharedPtr(PTRTYPE *ptr, const DELETER &deleter, ... *allocator
// [ 3] bcema_SharedPtr(bsl::auto_ptr<PTRTYPE> &autoPtr, ...
// [ 7] bcema_SharedPtr(bcema_SharedPtr<TYPE> const &alias, TYPE *object);
// [ 3] bcema_SharedPtr(bcema_SharedPtr<TYPE> const &original);
// [ 3] bcema_SharedPtr(bcema_SharedPtrRep *rep);
// [ 2] ~bcema_SharedPtr();
// [ 6] void load(PTRTYPE *ptr, bslma_Allocator *allocator=0)
// [ 6] void load(PTRTYPE *ptr, DELETER const&, bslma_Allocator *)
// [ 8] void loadAlias(bcema_SharedPtr<OTHER_TYPE> const& target, TYPE *object)
// [ 5] void createInplace(bslma_Allocator *allocator=0);
// [ 5] void createInplace(bslma_Allocator *allocator, A1 const& a1)
// [ 5] void createInplace(bslma_Allocator *allocator, A1 const& a1, ..&a2);
// [ 5] void createInplace(bslma_Allocator *allocator, A1 const& a1,...a3);
// [ 5] void createInplace(bslma_Allocator *allocator, A1 const& a1,...a4);
// [ 5] void createInplace(bslma_Allocator *allocator, A1 const& a1,...a5);
// [ 5] void createInplace(bslma_Allocator *allocator, A1 const& a1,...a6);
// [ 5] void createInplace(bslma_Allocator *allocator, A1 const& a1,...a7);
// [ 5] void createInplace(bslma_Allocator *allocator, A1 const& a1,...a8);
// [ 5] void createInplace(bslma_Allocator *allocator, A1 const& a1,...a9);
// [ 5] void createInplace(bslma_Allocator *allocator, A1 const& a1,...a10);
// [ 5] void createInplace(bslma_Allocator *allocator, A1 const& a1,...a11);
// [ 5] void createInplace(bslma_Allocator *allocator, A1 const& a1,...a12);
// [ 5] void createInplace(bslma_Allocator *allocator, A1 const& a1,...a13);
// [ 5] void createInplace(bslma_Allocator *allocator, A1 const& a1,...a14);
// [ 4] bcema_SharedPtr<TYPE>& operator=(const bcema_SharedPtr<TYPE> &);
// [ 4] bcema_SharedPtr<TYPE>& operator=(const bcema_SharedPtr<OTHER_TYPE> &)
// [ 4] bcema_SharedPtr<TYPE>& operator=(bsl::auto_ptr<PTRTYPE> &rhs)
// [12] void swap(bcema_SharedPtr<OTHER_TYPE> &src)
// [13] bdema_ManagedPtr<TYPE> managedPtr() const;
// [ 2] operator bool() const;
// [ 2] typename bcema_SharedPtr<TYPE>::Reference operator*() const;
// [ 2] TYPE *operator->() const;
// [ 2] TYPE *ptr() const;
// [ 2] bcema_SharedPtrRep *rep() const;
// [ 2] int numReferences() const;
//
// bcema_SharedPtrUtil
//--------------------
// [ 9] bcema_SharedPtr<TARGET> dynamicCast(bcema_SharedPtr<SOURCE> const& )
// [ 9] bcema_SharedPtr<TARGET> staticCast(bcema_SharedPtr<SOURCE> const& )
// [ 9] bcema_SharedPtr<TARGET> constCast(bcema_SharedPtr<SOURCE> const& )
// [10] bcema_SharedPtr<char> createInplaceUninitializedBuffer(...)
// [11] struct PtrLess<TYPE>;
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [14] CONCERN: SHARED POINTER IN-PLACE INSIDE 'bdef_Function' OBJECT
// [15] CONCERN: C++ 'bsl::shared_ptr' COMPLIANCE
// [18] USAGE EXAMPLE // TBD
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
class MyInplaceTestObject;
class MyTestBaseObject;
class MyTestDerivedObject;

// OTHER TEST OBJECTS (defined below)
class MyPDTestObject;
class MyInplaceTestObject;

// TEST DELETERS SECTION (defined below)
class MyTestObjectFactory;
class MyTestDeleter;
class MyAllocTestDeleter;
class MyTestFunctor;

void myTestFunctor(MyTestObject *);

// TYPEDEFS
typedef bcema_SharedPtr<MyTestObject> Obj;
typedef bcema_SharedPtr<const MyTestObject> ConstObj;
typedef MyTestObject TObj;

//=============================================================================
//               GLOBAL HELPER CLASSES AND FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

namespace NAMESPACE_TEST_CASE_16 {

int x = 1, y = 2;
double z = 3.0;

const bcema_SharedPtr<int>    ptrNil((int *)0);
const bcema_SharedPtr<int>    ptr1(&x, bcema_SharedPtrNilDeleter(), 0);
const bcema_SharedPtr<int>    ptr2(&y, bcema_SharedPtrNilDeleter(), 0);
const bcema_SharedPtr<double> ptr3(&z, bcema_SharedPtrNilDeleter(), 0);

bcema_SharedPtr<int> ptrNilFun() {
    return ptrNil;
}

bcema_SharedPtr<int> ptr1Fun() {
    return ptr1;
}

// For detecting absence of 'operator<':

struct NoOperatorLT {};

struct One {
    char d_member;
};     // sizeof(One) == 1

struct Two {
    char d_member[12345];
};  // sizeof(Two) != 1

One NoOperatorLTMatch(...);
Two NoOperatorLTMatch(NoOperatorLT);

}  // close NAMESPACE_TEST_CASE_16

template <class U>
NAMESPACE_TEST_CASE_16::NoOperatorLT
operator<(bcema_SharedPtr<U> const&, bcema_SharedPtr<U> const&);

template <class U, class V>
NAMESPACE_TEST_CASE_16::NoOperatorLT
operator<(bcema_SharedPtr<U> const&, bcema_SharedPtr<V> const&);

                   // *** 'MyTestObject' CLASS HIERARCHY ***

                           // ======================
                           // class MyTestBaseObject
                           // ======================

class MyTestBaseObject {
    // This class provides a test object used to verify that shared pointers
    // can statically and dynamically cast without slicing.

    // DATA
    char d_padding[32];

  public:
    // CREATORS
    MyTestBaseObject() {}
    virtual ~MyTestBaseObject() {}
};

                             // ==================
                             // class MyTestObject
                             // ==================

class MyTestObject: public MyTestBaseObject {
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

                         // =========================
                         // class MyTestDerivedObject
                         // =========================

class MyTestObject2: public MyTestBaseObject {
    // This supporting class for 'MyTestDerivedObject' is simply to make sure
    // that test objects with multiple inheritance work fine.

  public:
    // CREATORS
    MyTestObject2() {}
};

class MyTestDerivedObject: public MyTestObject2, public MyTestObject {
    // This class provides a test derived object, in order to make sure that

  public:
    // CREATORS
    MyTestDerivedObject(MyTestObject const& orig);
    MyTestDerivedObject(int *counter, int *copyCounter = 0);
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

                         // -------------------------
                         // class MyTestDerivedObject
                         // -------------------------

// CREATORS
MyTestDerivedObject::MyTestDerivedObject(MyTestObject const& orig)
: MyTestObject(orig)
{
}

MyTestDerivedObject::MyTestDerivedObject(int *counter, int *copyCounter)
: MyTestObject(counter, copyCounter)
{
}

                         // *** OTHER TEST OBJECTS ***

                            // ====================
                            // class MyPDTestObject
                            // ====================

class MyPDTestObject {
    // This class defines a private destructor, in order to test the conversion
    // to managed pointers.

  private:
    ~MyPDTestObject() {}
};

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
};

                       // *** TEST DELETERS SECTION ***

                         // =========================
                         // class MyTestObjectFactory
                         // =========================

class MyTestObjectFactory {
    // This class implements a prototypical factory deleter that simply wraps a
    // 'bslma_Allocator' without implementing this protocol.

    // DATA
    bslma_Allocator *d_allocator_p;

  public:
    // CREATORS
    MyTestObjectFactory(bslma_Allocator *basicAllocator = 0);

    // ACCESSORS
    void deleteObject(MyTestObject *obj) const;
};

                            // ===================
                            // class MyTestDeleter
                            // ===================

class MyTestDeleter {
    // This class provides a prototypical function-like deleter.

    // DATA
    bslma_Allocator *d_allocator_p;

  public:
    // CREATORS
    explicit MyTestDeleter(bslma_Allocator *basicAllocator = 0);
    MyTestDeleter(MyTestDeleter const& orig);

    // ACCESSORS
    template <class TYPE>
    void operator() (TYPE *ptr) const;

    bool operator==(const MyTestDeleter& rhs) const;
};

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
                     bslma_Allocator         *basicAllocator = 0);

    ~MyAllocTestDeleter();

    // MANIPULATORS
    MyAllocTestDeleter& operator=(MyAllocTestDeleter const& orig);

    // ACCESSORS
    template <class TYPE>
    void operator()(TYPE *ptr) const;
};

                             // ==================
                             // class MyTestFunctor
                             // ==================

class MyTestFunctor {
    // This class implements a 'bcef_Function' which does nothing.

  public:
    void operator()() {}
};

                           // ======================
                           // function myTestFunctor
                           // ======================

void myTestFunctor(MyTestObject *ptr)
    // This function can be used as a function-like deleter (by address) for a
    // 'bdema_SharedPtr' representation.
{
    delete ptr;
}

                         // -------------------------
                         // class MyTestObjectFactory
                         // -------------------------

// CREATORS
MyTestObjectFactory::MyTestObjectFactory(bslma_Allocator *basicAllocator)
: d_allocator_p(basicAllocator)
{
}

// ACCESSORS
void MyTestObjectFactory::deleteObject(MyTestObject *obj) const
{
    bslma_Allocator *ba = bslma_Default::allocator(d_allocator_p);
    ba->deleteObject(obj);
}

                            // -------------------
                            // class MyTestDeleter
                            // -------------------

// CREATORS
MyTestDeleter::MyTestDeleter(bslma_Allocator *basicAllocator)
: d_allocator_p(basicAllocator)
{
}

MyTestDeleter::MyTestDeleter(MyTestDeleter const& orig)
: d_allocator_p(orig.d_allocator_p)
{
}

template <class TYPE>
void MyTestDeleter::operator() (TYPE *ptr) const
{
    bslma_Allocator *ba = bslma_Default::allocator(d_allocator_p);
    ba->deleteObject(ptr);
}

bool MyTestDeleter::operator==(const MyTestDeleter& rhs) const
{
    return d_allocator_p == rhs.d_allocator_p;
}

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

template <class TYPE, class DELETER_FUNCTOR> class bcema_OldSharedPtr_RepImpl;
template <class TYPE> class bcema_OldSharedPtr_InplaceRepImpl;
template <class FACTORY> class bcema_OldSharedPtrFactoryAdapter;
template <class TYPE> struct bcema_OldSharedPtr_ReferenceType;
template <class TYPE, class DELETER> struct bcema_OldSharedPtr_InitGuard;

                        // ===========================
                        // class bcema_OldSharedPtrRep
                        // ===========================

class bcema_OldSharedPtrRep {
    // Partially implemented shared pointer representation ("letter") protocol.
    // This class provides a reference counter and a concrete implementation of
    // the 'bcema_Deleter' protocol that decrements the number references and
    // destroys itself if the number of references reaches zero.

    // DATA
    bces_AtomicInt    d_count;        // reference counter
    bslma_Allocator  *d_allocator_p;  // basic allocator

    // FRIENDS
    template <class TYPE> friend class bcema_OldSharedPtr_InplaceRepImpl;
        // TBD  (Friendship is for accessing 'd_allocator_p'; member should be
        // moved to concrete implementations and 'allocator()' below should be
        // made pure virtual.)

  public:
    // CLASS METHODS
    static void managedPtrDeleter(void *, bcema_OldSharedPtrRep *rep);
        // This function is used as a deleter when converting from a
        // 'bcema_OldSharedPtr' to a 'bdema_ManagedPtr'.  It decrements the
        // reference count of the specified 'rep' and destroys it if the number
        // of references reaches zero.  Note that the first argument (the
        // current pointer value of this shared pointer representation) is
        // ignored.

    // CREATORS
    bcema_OldSharedPtrRep(bslma_Allocator *allocator);
        // Construct a 'bcema_OldSharedPtrRep' object with an initial reference
        // count of one and store the specified 'allocator'.

    virtual ~bcema_OldSharedPtrRep() = 0;
        // Destroy this representation object.

    // MANIPULATORS
    void incrementRefs();
        // Increment the number of references to this representation by one.

    int decrementRefs();
        // Decrement the number of references to this representation by one and
        // return the result.

    // ACCESSORS
    virtual bool hasFunctionLikeDeleter() const = 0;
        // Return 'true' if the deleter whose address is returned by
        // 'getDeleter' is owned by this instance and will be invoked as a
        // function object (function-like deleter), and 'false' if this deleter
        // is held by address, either a factory deleters or pointer to
        // function.  Must be overridden by the derived implementation.

    virtual void *getDeleter(bsl::type_info const& typeInfo) const = 0;
        // Return the address of the deleter used to destroy the pointer value
        // supplied to the representation object at construction if its type
        // matches the specified 'typeInfo', or 0 otherwise.  Must be
        // overridden by the derived implementation.

    virtual void *originalPtr() const = 0;
        // Return the pointer value supplied to the representation object at
        // construction.  Must be overridden by the derived implementation.

    int numReferences() const;
        // Return a "snapshot" of the current number of references to this
        // representation object.

    bslma_Allocator *allocator() const;
        // Return a pointer to the allocator associated with this
        // representation object.
};

                        // ======================================
                        // class bcema_OldSharedPtr_ReferenceType
                        // ======================================

template <class TYPE>
struct bcema_OldSharedPtr_ReferenceType {
    // This class defines some basic traits used by bcema_OldSharedPtr.
    // It is primarily used to allow shared pointers of type 'void'
    // to work properly.

    typedef TYPE& Reference;
};

template <>
struct bcema_OldSharedPtr_ReferenceType<void> {
    // This specialization of 'bcema_OldSharedPtr_ReferenceType' for type
    // 'void' allows to avoid declaring a reference to 'void'.

    typedef void Reference;
};

                        // ========================
                        // class bcema_OldSharedPtr
                        // ========================

template <class TYPE>
class bcema_OldSharedPtr {
    // This class provides a thread-safe reference-counted "smart pointer" to
    // support "shared ownership" of objects: a shared pointer ensures that the
    // object that it manages is destroyed only when there are no longer any
    // references to it, and using the appropriate deletion method.  The object
    // (of the parameterized 'TYPE') pointed to by a shared pointer instance
    // may be accessed directly using the '->' operator, or the dereference
    // operator (operator '*') can be used to get a reference to that object.
    // See the component-level documentation for a thorough description and
    // examples of its many usages.
    //
    // Note that the object managed by a shared pointer instance is usually the
    // same as the object pointed to by that instance (of the same 'TYPE'), but
    // this need not always be true in the presence of "aliasing": the object
    // pointed-to, of the parameterized 'TYPE', may differ from the managed
    // object of type 'OTHER_TYPE' (see the section "Aliasing" in the
    // component-level documentation).  Nevertheless, both shall exist or else
    // the shared pointer is *empty* (i.e., manages no object, has no deleter,
    // and points to 0).
    //
    // More generally, this class supports a complete set of *in*-*core*
    // pointer semantic operations. . .  TBD (Exception guarantees, aliasing
    // guarantees - not to be confused with pointer aliasing, "In no event is
    // memory leaked", etc.)

    // DATA
    TYPE                  *d_ptr_p;  // pointer to the shared instance

    bcema_OldSharedPtrRep *d_rep_p;  // pointer to the representation object
                                     // that manages the shared instance

    // PRIVATE TYPES
    typedef bcema_OldSharedPtr<TYPE>                                 SelfType;
    typedef typename bcema_OldSharedPtr_ReferenceType<TYPE>::Reference
                                                                     Reference;

    // PRIVATE CREATORS
    bcema_OldSharedPtr(TYPE *ptr, bcema_OldSharedPtrRep *rep);
        // Construct a shared pointer having the specified 'ptr' and 'rep', but
        // *DO NOT* increment the number of references to rep, assume that
        // ownership is being transferred to this object.  This constructor is
        // used to transfer ownership to a temporary when a strong guarantee is
        // needed.  For performance reasons, this constructor is used instead
        // of 'swap' when an actual swap is not needed.

    // PRIVATE MANIPULATORS
    template <class OTHER_TYPE, class DELETER>
    void makeRep(OTHER_TYPE      *ptr,
                 const DELETER&   deleter,
                 bslma_Allocator *allocator);
        // Create a representation managing the object of the parameterized
        // 'OTHER_TYPE' at the specified 'ptr' address, using a copy of the
        // specified 'deleter' to destroy the specified 'ptr' when the
        // representation is destroyed, and reset this shared pointer to use
        // this representation and point to the object of the parameterized
        // 'TYPE' at the 'ptr' address (after converting 'ptr' to the type
        // 'TYPE *').  Use the specified 'allocator' to supply memory for the
        // representation object.

    void doReset(TYPE *ptr, bcema_OldSharedPtrRep *rep);
        // Reset this shared pointer to point to the specified 'ptr' address
        // (of the parameterized 'TYPE') and use the representation at the
        // specified 'rep' address.

    // FRIENDS
    friend struct bcema_OldSharedPtrUtil;
    template <class OTHER_TYPE>
        friend class bcema_OldSharedPtr;
    template <typename DELETER, typename OTHER_TYPE>
        friend DELETER *get_deleter(const bcema_OldSharedPtr<OTHER_TYPE>&);

  public:
    // TYPES
    typedef TYPE                 ElementType;
        // 'ElementType' is an alias to the parameterized 'TYPE' passed as
        // first and only template parameter to the 'bcema_OldSharedPtr' class
        // template.

    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(bcema_OldSharedPtr,
                                 bslalg_TypeTraitHasPointerSemantics);

    // CREATORS
    bcema_OldSharedPtr();
        // Create an empty shared pointer, i.e., a shared pointer with no
        // representation, that does not manage any object and has no deleter,
        // and that has a null pointer value.

    explicit bcema_OldSharedPtr(TYPE            *ptr,
                                bslma_Allocator *allocator = 0);
        // Create a shared pointer that points to and manages the object at the
        // specified 'ptr' address.  Optionally specify an 'allocator' to
        // allocate and deallocate the internal representation of the shared
        // pointer and to destroy the managed object when all references have
        // been released.  If 'allocator' is 0, the currently installed default
        // allocator is used (but see the "CAVEAT" in the section "C++ Standard
        // Compliance" of the component-level documentation).  Note that if
        // 'ptr' is 0, this shared pointer is initialized to an empty state and
        // 'allocator' is ignored.

    template <class OTHER_TYPE>
    explicit bcema_OldSharedPtr(OTHER_TYPE       *ptr,
                                bslma_Allocator *allocator = 0);
        // Create a shared pointer that manages the object of the parameterized
        // 'OTHER_TYPE' at the specified 'ptr' address and points to
        // '(TYPE *)ptr'.  Optionally specify an 'allocator' used to allocate
        // and deallocate the internal representation of this shared pointer
        // and to destroy the managed object when all references have been
        // released.  If 'allocator' is 0, the currently installed default
        // allocator is used (but see the "CAVEAT" in the section "C++ Standard
        // Compliance" of the component-level documentation).  If
        // 'OTHER_TYPE *' is not implicitly convertible to 'TYPE *' then a
        // compiler diagnostic will be emitted indicating the error.  Note that
        // if 'ptr' is 0, this shared pointer is initialized to an empty state
        // and 'allocator' is ignored.

    template <class OTHER_TYPE>
    bcema_OldSharedPtr(bdema_ManagedPtr<OTHER_TYPE>  managedPtr,
                       bslma_Allocator              *allocator = 0);
        // Create a shared pointer that takes over the management of the object
        // (if any) previously managed by the specified 'managedPtr' to the
        // parameterized 'OTHER_TYPE', and that points to the object at the
        // address '(TYPE *)managedPtr.ptr()'.  The deleter used in the
        // 'managedPtr' will be used to destroy the managed object when all
        // references have been released.  Optionally specify an 'allocator'
        // used to allocate and deallocate the internal representation of the
        // shared pointer.  If 'allocator' is 0, the currently installed
        // default allocator is used.  If 'OTHER_TYPE *' is not implicitly
        // convertible to 'TYPE *' then a compiler diagnostic will be emitted
        // indicating the error.  Note that if 'managedPtr' is empty, then this
        // shared pointer is initialized to the empty state.

    template <class OTHER_TYPE>
    explicit bcema_OldSharedPtr(bsl::auto_ptr<OTHER_TYPE>&  autoPtr,
                                bslma_Allocator            *allocator = 0);
        // Create a shared pointer that takes over the management of the
        // object previously managed by the specified 'autoPtr' to the
        // parameterized 'OTHER_TYPE', and that points to the object at the
        // address '(TYPE *)autoPtr.get()'.  The global '::operator delete'
        // will be used to destroy the managed object when all references have
        // been released.  Optionally specify an 'allocator' used to allocate
        // and deallocate the internal representation of the shared pointer.
        // If 'allocator' is 0, the currently installed default allocator is
        // used.  If 'OTHER_TYPE *' is not implicitly convertible to 'TYPE *'
        // then a compiler diagnostic will be emitted indicating the error.

    template <class OTHER_TYPE, class DELETER>
    bcema_OldSharedPtr(OTHER_TYPE      *ptr,
                       const DELETER&   deleter,
                       bslma_Allocator *allocator);
        // Create a shared pointer that manages the object of the
        // parameterized 'OTHER_TYPE' at the specified 'ptr' address and that
        // points to '(TYPE *)ptr', using the specified 'deleter' to delete the
        // managed object when all references have been released and the
        // specified 'allocator' to allocate and deallocate the internal
        // representation of the shared pointer.  If 'allocator' is 0, the
        // currently installed default allocator is used.  If 'DELETER' is a
        // reference type, then 'deleter' is assumed to be a function-like
        // deleter that may be invoked to destroy the object pointed-to by a
        // single argument of type 'OTHER_TYPE *' (i.e., 'deleter(ptr)' will be
        // called to destroy the managed object).  If 'DELETER' is a pointer
        // type, then 'deleter' is assumed to be a pointer to a factory
        // object which exposes a 'deleteObject(OTHER_TYPE *)' interface that
        // will be invoked to destroy the object at the 'ptr' address (i.e.,
        // 'deleter->deleteObject(ptr)' will be called to delete the managed
        // object).  (See the section "Deleters" in the component-level
        // documentation.)  If 'OTHER_TYPE *' is not implicitly convertible to
        // 'TYPE *' then a compiler diagnostic will be emitted indicating the
        // error.  Note that, for factory deleters, the specified 'deleter'
        // must remain valid until all references to 'ptr' have been released.
        // Also note that 'allocator' is not optional in this constructor form.
        // The reason is to avoid hiding the two-argument constructor
        // 'bcema_OldSharedPtr(OTHER_TYPE *ptr, bslma_Allocator *allocator=0)',
        // defined above.  Allowing the third argument to this constructor to
        // be optional causes it to be the best match for a constructor
        // invocation intended to match the preceding one.  Therefore, one must
        // always specify the allocator argument.  (Of course, one can
        // explicitly pass the 0 pointer to use the default allocator).  Note
        // that if 'ptr' is 0, this shared pointer is initialized to an empty
        // state and both 'deleter' and 'allocator' are ignored.

    template <class OTHER_TYPE>
    bcema_OldSharedPtr(bcema_OldSharedPtr<OTHER_TYPE> const&  source,
                    TYPE                                     *object);
        // Create a shared pointer that manages the same object (if any) as the
        // specified 'source' shared pointer to the parameterized 'OTHER_TYPE',
        // and that points to the object at the specified 'object' address.
        // The resulting shared pointer is known as an "alias" of 'source'.
        // Typically, the objects pointed to by 'source' and 'object' have
        // identical lifetimes (e.g., one might be a part of the other) so
        // that the deleter for 'source' will destroy them both, but do not
        // necessarily have the same type.  Note that if either 'source' is
        // empty or 'object' is null, then this shared pointer is
        // initialized to the empty state.

    bcema_OldSharedPtr(bcema_OldSharedPtr<TYPE> const& original);
        // Create a shared pointer that manages the same object (if any) as the
        // specified 'original' shared pointer, using the same deleter as the
        // 'original' to destroy the managed object, and that points to the
        // same object as 'original'.  Note that if 'original' is empty, then
        // this shared pointer will be initialized to the empty state.

    template <class OTHER_TYPE>
    bcema_OldSharedPtr(bcema_OldSharedPtr<OTHER_TYPE> const& original);
        // Create a shared pointer that manages the same object (if any) as the
        // specified 'original' shared pointer to the parameterized
        // 'OTHER_TYPE', using the same deleter as the 'original' to destroy
        // the managed object, and that points to the object at the address
        // '(TYPE *)original.ptr()'.  If 'OTHER_TYPE *' is not implicitly
        // convertible to 'TYPE *' then a compiler diagnostic will be emitted
        // indicating the error.  Note that if 'original' is empty, then this
        // shared pointer is initialized to the empty state.

    ~bcema_OldSharedPtr();
        // Destroy this shared pointer object.  If this shared pointer manages
        // a (possibly shared) object, then release the reference to the
        // managed object, calling the deleter to destroy the managed object if
        // this shared pointer is the last reference.

    // MANIPULATORS
    bcema_OldSharedPtr<TYPE>& operator=(const bcema_OldSharedPtr<TYPE>& rhs);
        // Make this shared pointer manage the same object as the specified
        // 'rhs' shared pointer, using the same deleter as 'rhs', pointing to
        // the same object as 'rhs', and return a reference to '*this'.  If
        // this shared pointer is already managing a (possibly shared) object,
        // then release the reference to the managed object, calling the
        // deleter to destroy the managed object if this pointer is the last
        // reference.  Note that if 'rhs' is empty, then this shared pointer
        // will also be empty after the assignment.  Also note that if '*this'
        // is the same object as 'rhs', then assignment has no effect.

    template <class OTHER_TYPE> bcema_OldSharedPtr<TYPE>&
                          operator=(const bcema_OldSharedPtr<OTHER_TYPE>& rhs);
        // Make this shared pointer manage the same object as the specified
        // 'rhs' shared pointer to the parameterized 'OTHER_TYPE', using the
        // same deleter as 'rhs', pointing to the address '(TYPE *)rhs.ptr()',
        // and return a reference to '*this'.  If this shared pointer is
        // already managing a (possibly shared) object, then release the
        // reference to the managed object, calling the deleter to destroy the
        // managed object if this pointer is the last reference.  Note that if
        // 'rhs' is empty, then this shared pointer will also be empty after
        // the assignment.

    template <class OTHER_TYPE>
    bcema_OldSharedPtr<TYPE>& operator=(bsl::auto_ptr<OTHER_TYPE>& rhs);
        // Transfer ownership to this shared pointer of the object managed by
        // the 'rhs' auto pointer to the parameterized 'OTHER_TYPE', using
        // '::operator delete' to destroy the managed object when the last
        // reference is released, and make this shared pointer point to the
        // address '(TYPE *)rhs.get()'.  If this shared pointer is already
        // managing a (possibly shared) object, then release the reference to
        // the managed object, calling the deleter to destroy the managed
        // object if this pointer is the last reference.  Note that if 'rhs' is
        // null, then this shared pointer will also be empty after the
        // assignment.

    void clear();
        // Reset this shared pointer to an empty state.  If this shared pointer
        // is already managing a (possibly shared) object, then release the
        // reference to the managed object, calling the deleter to destroy the
        // managed object if this pointer is the last reference.

    template <class OTHER_TYPE>
    void load(OTHER_TYPE *ptr, bslma_Allocator *allocator = 0);
        // Modify this shared pointer to manage the object of the parameterized
        // 'OTHER_TYPE' at the specified 'ptr' address and point to
        // '(TYPE *)ptr'.  If this shared pointer is already managing a
        // (possibly shared) object, then release the reference to the managed
        // object, calling the deleter to destroy the managed object if this
        // pointer is the last reference.  Optionally specify an 'allocator'
        // used to allocate and deallocate the internal representation of this
        // shared pointer and to destroy the managed object when all references
        // have been released.  If 'allocator' is 0, the currently installed
        // default allocator is used (but see the "CAVEAT" in the section "C++
        // Standard Compliance" of the component-level documentation).  If
        // 'OTHER_TYPE *' is not implicitly convertible to 'TYPE *' then a
        // compiler diagnostic will be emitted indicating the error.  Note that
        // if 'ptr' is 0, this shared pointer is initialized to the empty state
        // and 'allocator' is ignored.

    template <class OTHER_TYPE, class DELETER>
    void load(OTHER_TYPE      *ptr,
              const DELETER&   deleter,
              bslma_Allocator *allocator);
        // Modify this shared pointer to manage the object of the parameterized
        // 'OTHER_TYPE' at the specified 'ptr' address and to point to '(TYPE
        // *)ptr', using the specified 'deleter' to delete the managed object
        // when all references have been released and the specified 'allocator'
        // to allocate and deallocate the internal representation of the shared
        // pointer.  If 'allocator' is 0, the currently installed default
        // allocator is used.  If this shared pointer is already managing a
        // (possibly shared) object, then release the reference to the managed
        // object, calling the deleter to destroy the managed object if this
        // pointer is the last reference.  If 'DELETER' is a reference type,
        // then 'deleter' is assumed to be a function-like deleter that may
        // be invoked to destroy the object pointed-to by a single argument of
        // type 'OTHER_TYPE *' (i.e., 'deleter(ptr)' will be called to destroy
        // the managed object).  If 'DELETER' is a pointer type, then 'deleter'
        // is assumed to be a pointer to a factory object which exposes a
        // 'deleteObject(OTHER_TYPE *)' interface that will be invoked to
        // destroy the object at the 'ptr' address (i.e.,
        // 'deleter->deleteObject(ptr)' will be called to delete the managed
        // object).  (See the section "Deleters" in the component-level
        // documentation.)  If 'OTHER_TYPE *' is not implicitly convertible to
        // 'TYPE *' then a compiler diagnostic will be emitted indicating the
        // error.  Note that, for factory deleters, the specified 'deleter'
        // must remain valid until all references to 'ptr' have been released.
        // Note that if 'ptr' is 0, this shared pointer is initialized to the
        // empty state and both 'deleter' and 'allocator' are ignored.
        // Finally, note that this function is logically equivalent to:
        //..
        //  *this = bcema_OldSharedPtr<TYPE>(ptr, deleter, allocator);
        //..
        // and that, for the same reasons as explained in that constructor, the
        // third 'allocator' argument is not optional.

    template <class OTHER_TYPE>
    void loadAlias(bcema_OldSharedPtr<OTHER_TYPE> const&  source,
                   TYPE                              *object);
        // Modify this shared pointer to manage the same object (if any) as the
        // specified 'source' shared pointer to the parameterized 'OTHER_TYPE',
        // and point to the object at the specified 'object' address (i.e.,
        // make this shared pointer an "alias" of 'source').  If this shared
        // pointer is already managing a (possibly shared) object, then release
        // the reference to the managed object, calling the deleter to destroy
        // the managed object if this pointer is the last reference.
        // Typically, the objects pointed to by 'source' and 'object' have
        // identical lifetimes (e.g., one might be a part of the other) so
        // that the deleter for 'source' will destroy them both, but do not
        // necessarily have the same type.  Note that if either 'source' is
        // unset or 'object' is null, then this shared pointer will be
        // initialized to the empty state.  Also note that this function is
        // logically equivalent to:
        //..
        // '*this = bcema_OldSharedPtr<TYPE>(source, object)'
        //..

    void createInplace(bslma_Allocator *allocator = 0);
        // Create "in-place" in a large enough contiguous memory region both an
        // internal representation for this shared pointer and a
        // default-constructed instance of 'TYPE', and make this shared pointer
        // point to the newly created 'TYPE' instance.  Optionally specify an
        // 'allocator' used to supply memory.  If 'allocator' is 0, the
        // currently installed default allocator is used.  If an exception is
        // thrown during allocation or construction of 'TYPE', this shared
        // pointer will be unchanged.  Otherwise, if this shared pointer is
        // already managing a (possibly shared) object, then release the
        // reference to the managed object, calling the deleter to destroy the
        // managed object if this pointer is the last reference.  Note that the
        // allocator argument is *not* passed to the constructor for 'TYPE'.
        // To construct an in-place 'TYPE' with an allocator, use one of the
        // other variants of 'createInplace', below.

    template <class A1>
    void createInplace(bslma_Allocator *allocator, const A1& a1);
    template <class A1, class A2>
    void createInplace(bslma_Allocator *allocator, const A1& a1,
                       const A2& a2);
    template <class A1, class A2, class A3>
    void createInplace(bslma_Allocator *allocator, const A1& a1,
                       const A2& a2, const A3& a3);
    template <class A1, class A2, class A3, class A4>
    void createInplace(bslma_Allocator *allocator, const A1& a1,
                       const A2& a2, const A3& a3, const A4& a4);
    template <class A1, class A2, class A3, class A4, class A5>
    void createInplace(bslma_Allocator *allocator, const A1& a1,
                       const A2& a2, const A3& a3, const A4& a4,
                       const A5& a5);
    template <class A1, class A2, class A3, class A4, class A5, class A6>
    void createInplace(bslma_Allocator *allocator, const A1& a1,
                       const A2& a2, const A3& a3, const A4& a4,
                       const A5& a5, const A6& a6);
    template <class A1, class A2, class A3, class A4, class A5, class A6,
              class A7>
    void createInplace(bslma_Allocator *allocator, const A1& a1,
                       const A2& a2, const A3& a3, const A4& a4,
                       const A5& a5, const A6& a6, const A7& a7);
    template <class A1, class A2, class A3, class A4, class A5, class A6,
              class A7, class A8>
    void createInplace(bslma_Allocator *allocator, const A1& a1,
                       const A2& a2, const A3& a3, const A4& a4,
                       const A5& a5, const A6& a6, const A7& a7,
                       const A8& a8);
    template <class A1, class A2, class A3, class A4, class A5, class A6,
              class A7, class A8, class A9>
    void createInplace(bslma_Allocator *allocator, const A1& a1,
                       const A2& a2, const A3& a3, const A4& a4,
                       const A5& a5, const A6& a6, const A7& a7,
                       const A8& a8, const A9& a9);
    template <class A1, class A2, class A3, class A4, class A5, class A6,
              class A7, class A8, class A9, class A10>
    void createInplace(bslma_Allocator *allocator, const A1& a1,
                       const A2& a2, const A3& a3, const A4& a4,
                       const A5& a5, const A6& a6, const A7& a7,
                       const A8& a8, const A9& a9, const A10& a10);
    template <class A1, class A2, class A3, class A4, class A5, class A6,
              class A7, class A8, class A9, class A10, class A11>
    void createInplace(bslma_Allocator *allocator, const A1& a1,
                       const A2& a2, const A3& a3, const A4& a4,
                       const A5& a5, const A6& a6, const A7& a7,
                       const A8& a8, const A9& a9, const A10& a10,
                       const A11& a11);
    template <class A1, class A2, class A3, class A4, class A5, class A6,
              class A7, class A8, class A9, class A10, class A11, class A12>
    void createInplace(bslma_Allocator *allocator, const A1& a1,
                       const A2& a2, const A3& a3, const A4& a4,
                       const A5& a5, const A6& a6, const A7& a7,
                       const A8& a8, const A9& a9, const A10& a10,
                       const A11& a11, const A12& a12);
    template <class A1, class A2, class A3, class A4, class A5, class A6,
              class A7, class A8, class A9, class A10, class A11, class A12,
              class A13>
    void createInplace(bslma_Allocator *allocator, const A1& a1,
                       const A2& a2, const A3& a3, const A4& a4,
                       const A5& a5, const A6& a6, const A7& a7,
                       const A8& a8, const A9& a9, const A10& a10,
                       const A11& a11, const A12& a12, const A13& a13);
    template <class A1, class A2, class A3, class A4, class A5, class A6,
              class A7, class A8, class A9, class A10, class A11, class A12,
              class A13, class A14>
    void createInplace(bslma_Allocator *allocator, const A1& a1,
                       const A2& a2, const A3& a3, const A4& a4,
                       const A5& a5, const A6& a6, const A7& a7,
                       const A8& a8, const A9& a9, const A10& a10,
                       const A11& a11, const A12& a12, const A13& a13,
                       const A14& a14);
        // Create "in-place" in a large enough contiguous memory region both an
        // internal representation for this shared pointer and an instance of
        // 'TYPE' using the 'TYPE' constructor that takes the specified
        // arguments 'a1' up to 'aN' where 'N' (currently at most 14) is the
        // number of arguments passed to this method, and make this shared
        // pointer point to the newly created 'TYPE' instance.  Optionally
        // specify an 'allocator' used to supply memory.  If 'allocator' is 0,
        // the currently installed default allocator is used.  If an exception
        // is thrown during the construction of 'TYPE', this shared pointer
        // will be unchanged.  Otherwise, if this shared pointer is already
        // managing a (possibly shared) object, then release the reference to
        // the managed object, calling the deleter to destroy the managed
        // object if this pointer is the last reference.  Note that the
        // allocator argument is *not* implicitly passed to the constructor for
        // 'TYPE'.  To construct an instance of 'TYPE' with an allocator, pass
        // the allocator as one of the arguments (typically the last argument).

    void swap(bcema_OldSharedPtr<TYPE>& src);
        // Efficiently exchange the states of this shared pointer and the
        // specified 'src' shared pointer such that each will point to the
        // object formerly pointed to by the other and each will manage the
        // object formerly managed by the other.

    // ACCESSORS
    operator bool() const;
        // Return 'false' if this shared pointer is empty, and 'true'
        // otherwise.

    typename bcema_OldSharedPtr<TYPE>::Reference
    operator[](bsl::ptrdiff_t index) const;
        // Return a reference to the instance at the specified 'index' offset
        // of the object of the parameterized 'TYPE' pointed to by this shared
        // pointer.  The behavior is undefined if this shared pointer is empty.
        // Note that this is logically equivalent to '*(ptr() + index)'.

    typename bcema_OldSharedPtr<TYPE>::Reference operator*() const;
        // Return a reference to the object pointed to by this shared pointer.
        // The behavior is undefined if this shared pointer is empty.

    TYPE *operator->() const;
        // Return the address of the object pointed to by this shared pointer,
        // or 0 if this shared pointer is empty.  Note that applying this
        // operator conventionally (e.g., to invoke a method) to an empty
        // shared pointer will result in undefined behavior.

    TYPE *ptr() const;
        // Return the address of the 'TYPE' instance pointed to by this shared
        // pointer, or 0 if this shared pointer is empty.

    int numReferences() const;
        // Return a "snapshot" of the number of shared pointers (including this
        // one) that share ownership of the object managed by this shared
        // pointer.

    bdema_ManagedPtr<TYPE> managedPtr() const;
        // Return a managed pointer that points to the same object as this
        // shared pointer and which has a deleter that decrements the
        // reference count for the managed object.

                        // *** bsl::tr1 COMPATIBILITY ***
    // TYPES
    typedef TYPE  element_type;
        // Equivalent to 'ElementType'.  See 'ElementType'.

    // MANIPULATORS
    void reset();
        // Equivalent to 'clear()'.  See 'clear'.

    template <class OTHER_TYPE>
    void reset(OTHER_TYPE *ptr);
        // Equivalent to 'load(ptr)'.  See 'load'.

    template <class OTHER_TYPE, class DELETER>
    void reset(OTHER_TYPE *ptr, const DELETER& deleter);
        // Equivalent to 'load(ptr,deleter, (bslma_Allocator*)0)'.  See 'load'.

    template <class OTHER_TYPE>
    void reset(bcema_OldSharedPtr<OTHER_TYPE> const&  source,
               TYPE                                  *ptr);
        // Equivalent to 'loadAlias(source, ptr)'.  See 'loadAlias'.

    // ACCESSORS
    TYPE *get() const;
        // Return the address of the 'TYPE' instance pointed to by this shared
        // pointer, or 0 if this shared pointer is empty.  See 'ptr'.

    bool unique() const;
        // Return 'true' if this shared pointer does not share ownership of its
        // managed object with any other shared pointer, and 'false' otherwise.
        // See 'numReferences'.

    int use_count() const;
        // Return a "snapshot" of the number of shared pointers (including this
        // one) that share ownership of the object managed by this shared
        // pointer.  See 'numReferences'.
};

// FREE OPERATORS
template <typename LHS_TYPE, typename RHS_TYPE>
bool operator==(const bcema_OldSharedPtr<LHS_TYPE>& lhs,
                const bcema_OldSharedPtr<RHS_TYPE>& rhs);
    // Return 'true' if the specified 'lhs' has the same value as the specified
    // 'rhs', and 'false' otherwise.  Two shared pointers have the same value
    // if they point to the same address.  Note that if a (raw) pointer of the
    // parameterized type 'LHS_TYPE' cannot be compared to a (raw) pointer of
    // the parameterized type 'RHS_TYPE', then a compiler diagnostic will be
    // emitted indicating the error.  Note that two equal shared pointers do
    // not necessarily manage the same object.

template <typename LHS_TYPE, typename RHS_TYPE>
bool operator!=(const bcema_OldSharedPtr<LHS_TYPE>& lhs,
                const bcema_OldSharedPtr<RHS_TYPE>& rhs);
    // Return 'true' if the specified 'lhs' does not have the same value as the
    // specified 'rhs', and 'false' otherwise.  Two shared pointers do not have
    // the same value if they point to different addresses.  Note that if a
    // (raw) pointer of the parameterized type 'LHS_TYPE' cannot be compared
    // to a (raw) pointer of the parameterized type 'RHS_TYPE', then a
    // compiler diagnostic will be emitted indicating the error.  Note that two
    // equal shared pointers do not necessarily manage the same object.

template <typename LHS_TYPE, typename RHS_TYPE>
bool operator<(const bcema_OldSharedPtr<LHS_TYPE>& lhs,
               const bcema_OldSharedPtr<RHS_TYPE>& rhs);
    // Return 'true' if the specified 'lhs.ptr()' has a smaller value than the
    // specified 'rhs.ptr()', and 'false' otherwise.  Note that if a (raw)
    // pointer of the parameterized type 'LHS_TYPE' cannot be compared to a
    // (raw) pointer of the parameterized type 'RHS_TYPE', then a compiler
    // diagnostic will be emitted indicating the error.

template <typename LHS_TYPE, typename RHS_TYPE>
bool operator<=(const bcema_OldSharedPtr<LHS_TYPE>& lhs,
                const bcema_OldSharedPtr<RHS_TYPE>& rhs);
    // Return 'true' if the specified 'lhs.ptr()' has a value equal to or
    // smaller than the specified 'rhs.ptr()', and 'false' otherwise.  Note
    // that if a (raw) pointer of the parameterized type 'LHS_TYPE' cannot be
    // compared to a (raw) pointer of the parameterized type 'RHS_TYPE', then a
    // compiler diagnostic will be emitted indicating the error.

template <typename LHS_TYPE, typename RHS_TYPE>
bool operator>(const bcema_OldSharedPtr<LHS_TYPE>& lhs,
               const bcema_OldSharedPtr<RHS_TYPE>& rhs);
    // Return 'true' if the specified 'lhs.ptr()' has a larger value than the
    // specified 'rhs.ptr()', and 'false' otherwise.  Note that if a (raw)
    // pointer of the parameterized type 'LHS_TYPE' cannot be compared to a
    // (raw) pointer of the parameterized type 'RHS_TYPE', then a compiler
    // diagnostic will be emitted indicating the error.

template <typename LHS_TYPE, typename RHS_TYPE>
bool operator>=(const bcema_OldSharedPtr<LHS_TYPE>& lhs,
                const bcema_OldSharedPtr<RHS_TYPE>& rhs);
    // Return 'true' if the specified 'lhs.ptr()' has a value equal to or
    // larger than the specified 'rhs.ptr()', and 'false' otherwise.  Note that
    // if a (raw) pointer of the parameterized type 'LHS_TYPE' cannot be
    // compared to a (raw) pointer of the parameterized type 'RHS_TYPE', then a
    // compiler diagnostic will be emitted indicating the error.

template <typename TYPE>
bsl::ostream& operator<<(bsl::ostream&                stream,
                         const bcema_OldSharedPtr<TYPE>& rhs);
    // Print the pointer value of the specified 'rhs' shared pointer to the
    // specified 'stream' and return a reference to the modifiable 'stream'.

                        // *** bsl::tr1 COMPATIBILITY ***

template <typename TYPE, typename OTHER_TYPE>
bcema_OldSharedPtr<OTHER_TYPE>
const_pointer_cast(const bcema_OldSharedPtr<TYPE>& rhs);
    // Return a 'bcema_OldSharedPtr<TARGET>' object sharing ownership of the
    // same object as the specified 'source' shared pointer to the
    // parameterized 'SOURCE', and pointing to
    // 'const_cast<TARGET *>(source.get())'.  Note that if 'source' cannot be
    // 'const'-cast to 'TARGET', then a compiler diagnostic will be emitted
    // indicating the error.  Also note that the seemingly equivalent
    // expression:
    //..
    //  bcema_OldSharedPtr<TARGET>(const_cast<TARGET *>(source.get()))
    //..
    // will eventually result in undefined behavior, attempting to delete
    // the same object twice.

template <typename TYPE, typename OTHER_TYPE>
bcema_OldSharedPtr<OTHER_TYPE>
dynamic_pointer_cast(const bcema_OldSharedPtr<TYPE>& rhs);
    // Return a 'bcema_OldSharedPtr<TARGET>' object sharing ownership of the
    // same object as the specified 'source' shared pointer to the
    // parameterized 'SOURCE', and pointing to
    // 'dynamic_cast<TARGET *>(source.get())'.  Note that if 'source' cannot be
    // dynamically cast to 'TARGET', then an empty shared pointer will be
    // returned.  Also note that the seemingly equivalent expression:
    //..
    //  bcema_OldSharedPtr<TARGET>(dynamic_cast<TARGET *>(source.get()))
    //..
    // will eventually result in undefined behavior, attempting to delete
    // the same object twice.

template<class DELETER, class TYPE>
DELETER *get_deleter(const bcema_OldSharedPtr<TYPE>& rhs);
    // Return the address of the deleter of type cv-unqualified 'DELETER' used
    // by the specified 'rhs' shared pointer, if any, and 0 if either 'rhs'
    // does not use a deleter object or if this deleter object is not of type
    // cv-unqualified 'DELETER'.  Note that the returned pointer remains valid
    // as long as there exists a 'bcema_OldSharedPtr' instance that owns it.
    // Also note that 'rhs' must own either an instance of 'DELETER' if 'rhs'
    // was created with a function-like deleter, or it must own a 'DELETER *'
    // pointer (in particular, if 'DELETER' is a base class of the actual
    // factory passed at creation of 'rhs', this function call will *not*
    // return a polymorphic factory object, but will return 0 -- in other
    // words, 'DELETER' must be *identical* the actual factory type).

template <typename TYPE, typename OTHER_TYPE>
bcema_OldSharedPtr<OTHER_TYPE>
static_pointer_cast(const bcema_OldSharedPtr<TYPE>& rhs);
    // Return a 'bcema_OldSharedPtr<TARGET>' object sharing ownership of the
    // same object as the specified 'source' shared pointer to the
    // parameterized 'SOURCE', and pointing to
    // 'static_cast<TARGET *>(source.get())'.  Note that if 'source' cannot be
    // statically cast to 'TARGET', then a compiler diagnostic will be emitted
    // indicating the error.  Also note that the seemingly equivalent
    // expression:
    //..
    //  bcema_OldSharedPtr<TARGET>(static_cast<TARGET *>(source.get()))
    //..
    // will eventually result in undefined behavior, attempting to delete
    // the same object twice.

template <typename TYPE>
void swap(bcema_OldSharedPtr<TYPE>& lhs, bcema_OldSharedPtr<TYPE>& rhs);
    // Efficiently exchange the states of the specified 'lhs' shared pointer
    // and the specified 'rhs' shared pointer such that each will point to the
    // object formerly pointed to by the other and each will manage the object
    // formerly managed by the other.

template <class TYPE>
bcema_OldSharedPtr<TYPE> make_shared();
    // Return a shared pointer to a default-constructed instance of 'TYPE'.
    // Use the currently installed default allocator to supply memory for both
    // the representation and the instance of 'TYPE'.

template <class TYPE, class A1>
bcema_OldSharedPtr<TYPE>
make_shared(const A1& a1);
template <class TYPE, class A1, class A2>
bcema_OldSharedPtr<TYPE>
make_shared(const A1& a1, const A2& a2);
template <class TYPE, class A1, class A2, class A3>
bcema_OldSharedPtr<TYPE>
make_shared(const A1& a1, const A2& a2, const A3& a3);
template <class TYPE, class A1, class A2, class A3, class A4>
bcema_OldSharedPtr<TYPE>
make_shared(const A1& a1, const A2& a2, const A3& a3, const A4& a4);
template <class TYPE, class A1, class A2, class A3, class A4, class A5>
bcema_OldSharedPtr<TYPE>
make_shared(const A1& a1, const A2& a2, const A3& a3, const A4& a4,
            const A5& a5);
template <class TYPE, class A1, class A2, class A3, class A4, class A5,
          class A6>
bcema_OldSharedPtr<TYPE>
make_shared(const A1& a1, const A2& a2, const A3& a3, const A4& a4,
            const A5& a5, const A6& a6);
template <class TYPE, class A1, class A2, class A3, class A4, class A5,
          class A6,   class A7>
bcema_OldSharedPtr<TYPE>
make_shared(const A1& a1, const A2& a2, const A3& a3, const A4& a4,
            const A5& a5, const A6& a6, const A7& a7);
template <class TYPE, class A1, class A2, class A3, class A4, class A5,
          class A6,   class A7, class A8>
bcema_OldSharedPtr<TYPE>
make_shared(const A1& a1, const A2& a2, const A3& a3, const A4& a4,
            const A5& a5, const A6& a6, const A7& a7, const A8& a8);
template <class TYPE, class A1, class A2, class A3, class A4, class A5,
          class A6,   class A7, class A8, class A9>
bcema_OldSharedPtr<TYPE>
make_shared(const A1& a1, const A2& a2, const A3& a3, const A4& a4,
            const A5& a5, const A6& a6, const A7& a7, const A8& a8,
            const A9& a9);
template <class TYPE, class A1, class A2, class A3, class A4, class A5,
          class A6,   class A7, class A8, class A9, class A10>
bcema_OldSharedPtr<TYPE>
make_shared(const A1& a1, const A2&  a2, const A3& a3, const A4& a4,
            const A5& a5, const A6&  a6, const A7& a7, const A8& a8,
            const A9& a9, const A10& a10);
template <class TYPE, class A1, class A2, class A3, class A4,  class A5,
          class A6,   class A7, class A8, class A9, class A10, class A11>
bcema_OldSharedPtr<TYPE>
make_shared(const A1& a1, const A2&  a2,  const A3&  a3, const A4& a4,
            const A5& a5, const A6&  a6,  const A7&  a7, const A8& a8,
            const A9& a9, const A10& a10, const A11& a11);
template <class TYPE, class A1, class A2, class A3, class A4,  class A5,
          class A6,   class A7, class A8, class A9, class A10, class A11,
          class A12>
bcema_OldSharedPtr<TYPE>
make_shared(const A1& a1, const A2&  a2,  const A3&  a3,  const A4&  a4,
            const A5& a5, const A6&  a6,  const A7&  a7,  const A8&  a8,
            const A9& a9, const A10& a10, const A11& a11, const A12& a12);
template <class TYPE, class A1, class A2, class A3, class A4,  class A5,
          class A6,   class A7, class A8, class A9, class A10, class A11,
          class A12,  class A13>
bcema_OldSharedPtr<TYPE>
make_shared(const A1&  a1, const A2&  a2,  const A3&  a3,  const A4&  a4,
            const A5&  a5, const A6&  a6,  const A7&  a7,  const A8&  a8,
            const A9&  a9, const A10& a10, const A11& a11, const A12& a12,
            const A13& a13);
template <class TYPE, class A1,  class A2, class A3, class A4,  class A5,
          class A6,   class A7,  class A8, class A9, class A10, class A11,
          class A12,  class A13, class A14>
bcema_OldSharedPtr<TYPE>
make_shared(const A1&  a1,  const A2&  a2,  const A3&  a3,  const A4&  a4,
            const A5&  a5,  const A6&  a6,  const A7&  a7,  const A8&  a8,
            const A9&  a9,  const A10& a10, const A11& a11, const A12& a12,
            const A13& a13, const A14& a14);
    // Return a shared pointer to an instance of 'TYPE' created using the
    // 'TYPE' constructor that takes the specified arguments 'a1' up to 'aN'
    // where 'N' (currently at most 14) is the number of arguments passed to
    // this function.  Use the currently installed default allocator to supply
    // memory for the footprints of the representation and the instance of
    // 'TYPE' in a single continuous memory region.  Note that if the last
    // argument 'aN' is of a type convertible to 'bslma_Allocator *' and if
    // 'TYPE' takes a 'bslma_Allocator' and has a constructor of a signature
    // compatible the first 'N - 1' arguments plus a 'bslma_Allocator *', then
    // this allocator is passed through to that instance; use
    // 'allocate_shared(aN, a1, . . . aN)' instead to ensure that the footprint
    // and any memory used by the 'TYPE' instance are supplied by the same
    // allocator 'aN'.

template <class TYPE>
bcema_OldSharedPtr<TYPE> allocate_shared(bslma_Allocator *allocator);
    // Return a shared pointer to a default-constructed instance of 'TYPE'.
    // Use the specified 'allocator' to supply memory for both the
    // representation and the instance of 'TYPE' in a single memory region.  If
    // 'allocator' is 0, use the currently installed default allocator.

template <class TYPE, class A1>
bcema_OldSharedPtr<TYPE>
allocate_shared(bslma_Allocator *allocator, const A1& a1);
template <class TYPE, class A1, class A2>
bcema_OldSharedPtr<TYPE>
allocate_shared(bslma_Allocator *allocator, const A1& a1, const A2& a2);
template <class TYPE, class A1, class A2, class A3>
bcema_OldSharedPtr<TYPE>
allocate_shared(bslma_Allocator *allocator, const A1& a1, const A2& a2,
                const A3& a3);
template <class TYPE, class A1, class A2, class A3, class A4>
bcema_OldSharedPtr<TYPE>
allocate_shared(bslma_Allocator *allocator, const A1& a1, const A2& a2,
                const A3& a3, const A4& a4);
template <class TYPE, class A1, class A2, class A3, class A4,
          class A5>
bcema_OldSharedPtr<TYPE>
allocate_shared(bslma_Allocator *allocator, const A1& a1, const A2& a2,
                const A3& a3, const A4& a4, const A5& a5);
template <class TYPE, class A1, class A2, class A3, class A4,
          class A5,   class A6>
bcema_OldSharedPtr<TYPE>
allocate_shared(bslma_Allocator *allocator, const A1& a1, const A2& a2,
                const A3& a3, const A4& a4, const A5& a5, const A6& a6);
template <class TYPE, class A1, class A2, class A3, class A4,
          class A5,   class A6, class A7>
bcema_OldSharedPtr<TYPE>
allocate_shared(bslma_Allocator *allocator, const A1& a1, const A2& a2,
                const A3& a3, const A4& a4, const A5& a5, const A6& a6,
                const A7& a7);
template <class TYPE, class A1, class A2, class A3, class A4,
          class A5,   class A6, class A7, class A8>
bcema_OldSharedPtr<TYPE>
allocate_shared(bslma_Allocator *allocator, const A1& a1, const A2& a2,
                const A3& a3, const A4& a4, const A5& a5, const A6& a6,
                const A7& a7, const A8& a8);
template <class TYPE, class A1, class A2, class A3, class A4,
          class A5,   class A6, class A7, class A8, class A9>
bcema_OldSharedPtr<TYPE>
allocate_shared(bslma_Allocator *allocator, const A1& a1, const A2& a2,
                const A3& a3, const A4& a4, const A5& a5, const A6& a6,
                const A7& a7, const A8& a8, const A9& a9);
template <class TYPE, class A1, class A2, class A3, class A4,
          class A5, class A6, class A7, class A8, class A9, class A10>
bcema_OldSharedPtr<TYPE>
allocate_shared(bslma_Allocator *allocator, const A1& a1, const A2&  a2,
                const A3& a3, const A4& a4, const A5& a5, const A6&  a6,
                const A7& a7, const A8& a8, const A9& a9, const A10& a10);
template <class TYPE, class A1, class A2, class A3, class A4,
          class A5,   class A6, class A7, class A8, class A9, class A10,
          class A11>
bcema_OldSharedPtr<TYPE>
allocate_shared(bslma_Allocator *allocator, const A1& a1, const A2&  a2,
                const A3& a3, const A4& a4, const A5& a5, const A6&  a6,
                const A7& a7, const A8& a8, const A9& a9, const A10& a10,
                const A11& a11);
template <class TYPE, class A1, class A2, class A3, class A4,
          class A5,   class A6, class A7, class A8, class A9, class A10,
          class A11,  class A12>
bcema_OldSharedPtr<TYPE>
allocate_shared(bslma_Allocator *allocator, const A1& a1, const A2&  a2,
                const A3& a3, const A4& a4, const A5& a5, const A6&  a6,
                const A7& a7, const A8& a8, const A9& a9, const A10& a10,
                const A11& a11, const A12& a12);
template <class TYPE, class A1,  class A2, class A3, class A4,
          class A5,   class A6,  class A7, class A8, class A9, class A10,
          class A11,  class A12, class A13>
bcema_OldSharedPtr<TYPE>
allocate_shared(bslma_Allocator *allocator,     const A1&  a1,  const A2&  a2,
                const A3&  a3,  const A4&  a4,  const A5&  a5,  const A6&  a6,
                const A7&  a7,  const A8&  a8,  const A9&  a9,  const A10& a10,
                const A11& a11, const A12& a12, const A13& a13);
template <class TYPE, class A1, class  A2,  class A3, class A4,
          class A5,   class A6, class  A7,  class A8, class A9, class A10,
          class A11,  class A12, class A13, class A14>
bcema_OldSharedPtr<TYPE>
allocate_shared(bslma_Allocator *allocator,     const A1&  a1,  const A2&  a2,
                const A3&  a3,  const A4&  a4,  const A5&  a5,  const A6&  a6,
                const A7&  a7,  const A8&  a8,  const A9&  a9,  const A10& a10,
                const A11& a11, const A12& a12, const A13& a13,
                const A14& a14);
    // Return a shared pointer to an instance of 'TYPE' created using the
    // 'TYPE' constructor that takes the specified arguments 'a1' up to 'aN'
    // where 'N' (currently at most 14) is the number of arguments passed to
    // this function.  Use the specified 'allocator' to supply memory for the
    // footprints of both the representation and the instance of 'TYPE' in a
    // single continuous memory region.  If 'allocator' is 0, use the currently
    // installed default allocator.  Note that if the last argument 'aN' is of
    // a type convertible to 'bslma_Allocator *' and if 'TYPE' takes a
    // 'bslma_Allocator' and has a constructor of a signature compatible the
    // first 'N - 1' arguments plus a 'bslma_Allocator *', then this allocator
    // is passed through to that instance; use the same value for 'allocator'
    // and 'aN' to ensure that the footprint and any memory used by the 'TYPE'
    // instance are supplied by the same allocator.

#ifdef BSL_STDHDRS_DEFINED_STD_MACRO
#undef std
#define BCEMA_DEFINE_STD
#endif
namespace std {

template <typename TYPE>
struct less< bcema_OldSharedPtr<TYPE> >
                                    : binary_function<
                                          bcema_OldSharedPtr<TYPE>,
                                          bcema_OldSharedPtr<TYPE>,
                                          bool>
{
    // This specialization of 'bsl::less' for 'bcema_OldSharedPtr<TYPE>' is
    // necessary because 'bsl::less<TYPE*>' might be a specialization of
    // 'bsl::less' which does not use 'operator<' on 'TYPE*', due to (section
    // 20.5.7, paragraph 8) of the C++ standard: "For templates greater, less,
    // greater_equal, and less_equal, the specializations for any pointer type
    // yield a total order, even if the built-in operators <, >, <=, >= do
    // not."

    bool operator()(bcema_OldSharedPtr<TYPE> const& lhs,
                    bcema_OldSharedPtr<TYPE> const& rhs) const {
        return less<TYPE *>()(lhs.ptr(), rhs.ptr());
    }
};

template <typename TYPE>
struct greater< bcema_OldSharedPtr<TYPE> >
                                    : binary_function<
                                          bcema_OldSharedPtr<TYPE>,
                                          bcema_OldSharedPtr<TYPE>,
                                          bool>
{
    // This specialization of 'bsl::greater' for 'bcema_OldSharedPtr<TYPE>'
    // uses 'bsl::greater<TYPE*>' on the pointer values of the shared pointers.

    bool operator()(bcema_OldSharedPtr<TYPE> const& lhs,
                    bcema_OldSharedPtr<TYPE> const& rhs) const {
        return greater<TYPE *>()(lhs.ptr(), rhs.ptr());
    }
};

template <typename TYPE>
struct less_equal< bcema_OldSharedPtr<TYPE> >
                                    : binary_function<
                                          bcema_OldSharedPtr<TYPE>,
                                          bcema_OldSharedPtr<TYPE>,
                                          bool>
{
    // This specialization of 'bsl::less_equal' for 'bcema_OldSharedPtr<TYPE>'
    // uses 'bsl::less_equal<TYPE*>' on the pointer values of the shared
    // pointers.

    bool operator()(bcema_OldSharedPtr<TYPE> const& lhs,
                    bcema_OldSharedPtr<TYPE> const& rhs) const {
        return less_equal<TYPE *>()(lhs.ptr(), rhs.ptr());
    }
};

template <typename TYPE>
struct greater_equal< bcema_OldSharedPtr<TYPE> >
                                    : binary_function<
                                          bcema_OldSharedPtr<TYPE>,
                                          bcema_OldSharedPtr<TYPE>,
                                          bool>
{
    // This specialization of 'bsl::greater_equal' for
    // 'bcema_OldSharedPtr<TYPE>' uses 'bsl::greater_equal<TYPE*>' on the
    // pointer values of the shared pointers.

    bool operator()(bcema_OldSharedPtr<TYPE> const& lhs,
                    bcema_OldSharedPtr<TYPE> const& rhs) const {
        return greater_equal<TYPE *>()(lhs.ptr(), rhs.ptr());
    }
};

}  // close namespace std

#ifdef BCEMA_DEFINE_STD
#define std bsl
#undef BCEMA_DEFINE_STD
#endif

                        // ============================
                        // class bcema_OldSharedPtrUtil
                        // ============================

struct bcema_OldSharedPtrUtil {
    // Namespace for operations on shared pointers.

    // CLASS METHODS
    template <class TARGET, class SOURCE>
    static
    bcema_OldSharedPtr<TARGET> dynamicCast(
                                     bcema_OldSharedPtr<SOURCE> const& source);
        // Return a 'bcema_OldSharedPtr<TARGET>' object sharing ownership of
        // the same object as the specified 'source' shared pointer to the
        // parameterized 'SOURCE', and pointing to
        // 'dynamic_cast<TARGET *>(source.ptr())'.  If 'source' cannot be
        // dynamically cast to 'TARGET', then return an empty
        // 'bcema_OldSharedPtr<TARGET>' object.

    template <class TARGET, class SOURCE>
    static
    bcema_OldSharedPtr<TARGET> staticCast(
                                     bcema_OldSharedPtr<SOURCE> const& source);
        // Return a 'bcema_OldSharedPtr<TARGET>' object sharing ownership of
        // the same object as the specified 'source' shared pointer to the
        // parameterized 'SOURCE', and pointing to
        // 'static_cast<TARGET *>(source.ptr())'.  Note that if 'source' cannot
        // be statically cast to 'TARGET', then a compiler diagnostic will be
        // emitted indicating the error.

    template <class TARGET, class SOURCE>
    static
    bcema_OldSharedPtr<TARGET> constCast(
                                     bcema_OldSharedPtr<SOURCE> const& source);
        // Return a 'bcema_OldSharedPtr<TARGET>' object sharing ownership of
        // the same object as the specified 'source' shared pointer to the
        // parameterized 'SOURCE', and pointing to
        // 'const_cast<TARGET *>(source.ptr())'.  Note that if 'source' cannot
        // be 'const'-cast to 'TARGET', then a compiler diagnostic will be
        // emitted indicating the error.

    template <class TARGET, class SOURCE>
    static
    void dynamicCast(bcema_OldSharedPtr<TARGET>        *target,
                     bcema_OldSharedPtr<SOURCE> const&  source);
        // Load into the specified 'target' shared pointer an alias of the
        // specified 'source' shared pointer pointing to
        // 'dynamic_cast<TARGET *>(source.ptr())'.  The previous 'target'
        // shared pointer is destroyed (destroying the managed object if
        // 'target' holds the last reference to this object).  If 'source'
        // cannot be dynamically cast to 'TARGET', 'target' will be initialized
        // to an empty value.

    template <class TARGET, class SOURCE>
    static
    void staticCast(bcema_OldSharedPtr<TARGET>        *target,
                    bcema_OldSharedPtr<SOURCE> const&  source);
        // Load into the specified 'target' shared pointer an alias of the
        // specified 'source' shared pointer pointing to
        // 'static_cast<TARGET *>(source.ptr())'.  The previous 'target' shared
        // pointer is destroyed (destroying the managed object if 'target'
        // holds the last reference to this object).  Note that if 'source'
        // cannot be statically cast to 'TARGET', then a compiler diagnostic
        // will be emitted indicating the error.

    template <class TARGET, class SOURCE>
    static
    void constCast(bcema_OldSharedPtr<TARGET>        *target,
                   bcema_OldSharedPtr<SOURCE> const&  source);
        // Load into the specified 'target' shared pointer an alias of the
        // specified 'source' shared pointer pointing to
        // 'static_cast<TARGET *>(source.ptr())'.  The previous 'target' shared
        // pointer is destroyed (destroying the managed object if 'target'
        // holds the last reference to this object).  Note that if 'source'
        // cannot be statically cast to 'TARGET', then a compiler diagnostic
        // will be emitted indicating the error.

    static
    bcema_OldSharedPtr<char>
    createInplaceUninitializedBuffer(bsl::size_t      bufferSize,
                                     bslma_Allocator *allocator = 0);
        // Return a shared pointer with an in-place representation to a newly
        // created uninitialized buffer of the specified 'bufferSize'.
        // Optionally specify the 'allocator' used to supply memory.  If
        // 'allocator' is 0, the currently installed default allocator is used.
        // Note that the return buffer is only *naturally* aligned to
        // 'bufferSize', even if 'allocator' always return maximally aligned
        // memory addresses.
};

                        // ===============================
                        // class bcema_OldSharedPtrNilDeleter
                        // ===============================

struct bcema_OldSharedPtrNilDeleter {
    // This class provides a shared pointer deleter that does nothing when
    // invoked.

    template <class TYPE>
    void operator() (TYPE *)
        // No-Op.
    {
    }
};

                        // ===================================
                        // class bcema_OldSharedPtrFactoryAdapter
                        // ===================================

template <class FACTORY>
class bcema_OldSharedPtrFactoryAdapter {
    // This class provides an adapter to allow objects that expose a
    // 'deleteObject' method to be used as "deleters".

    // INSTANCE DATA
    FACTORY *d_factory_p; // held but not owned

  public:
    // CREATORS
    bcema_OldSharedPtrFactoryAdapter(
              const bcema_OldSharedPtrFactoryAdapter<FACTORY>& original);
        // Construct a 'bcema_OldSharedPtrFactoryAdapter' object using the
        // value of the specified 'original' object.

    bcema_OldSharedPtrFactoryAdapter(FACTORY *factory);
        // Construct 'bcema_OldSharedPtrFactoryAdapter' that uses the specified
        // 'factory' to destroy objects.

    // ACCESSORS
    template <class TYPE>
    void operator() (TYPE *instance) const;
        // Delete the specified 'instance' by invoking the
        // 'deleteObject(instance)' on the associated factory.
};

                        // ==============================
                        // class bcema_OldSharedPtrRepImpl
                        // ==============================

template <class TYPE, class DELETER_FUNCTOR>
class bcema_OldSharedPtr_RepImpl: public bcema_OldSharedPtrRep {
    // This class provides a concrete implementation of the
    // 'bcema_OldSharedPtr_Rep' protocol for out-of-place instances of the
    // parameterized 'TYPE'.  Upon destruction of this object, a copy of the
    // deleter of the parameterized type 'DELETER_FUNCTOR' that was passed at
    // construction is invoked with a pointer to the managed object.

    // PRIVATE TYPES
    struct PointerTag {};
    struct NonPointerTag {};

    typedef typename
        bslmf_If<bslmf_IsPointer<DELETER_FUNCTOR>::VALUE, PointerTag,
                     NonPointerTag>::Type TagType;

    // INSTANCE DATA
    DELETER_FUNCTOR  d_deleter; // deleter for this out-of-place instance
    TYPE            *d_ptr_p;   // pointer to out-of-place instance (owned)

    // PRIVATE MANIPULATORS
    template <class FACTORY>
    void doDeleteObject(TYPE *ptr, FACTORY *factory,  PointerTag);

    template < class RET, class A1>
    void doDeleteObject(TYPE *ptr, RET(*deleterFunc)(A1),  PointerTag);

    template <class DELETER>
    void doDeleteObject(TYPE *ptr, DELETER& deleter, NonPointerTag);

    void initDefaultAllocator(bslma_Allocator *, PointerTag);

    template <class DELETER>
    void initDefaultAllocator(DELETER *, PointerTag);

    template <class DELETER>
    void initDefaultAllocator(DELETER& , NonPointerTag);

  public:
    // CREATORS
    bcema_OldSharedPtr_RepImpl(TYPE                   *ptr,
                            const DELETER_FUNCTOR&  deleter,
                            bslma_Allocator        *allocator = 0);
        // Construct a 'bcema_OldSharedPtr_RepImpl' to represent the object of
        // parameterized 'TYPE' specified by 'ptr' and instantiate a copy the
        // specified 'deleter' that will be used to destroy the specified 'ptr'
        // at destruction of this object.  Note that the specified 'allocator'
        // is the allocator that should be used to destroy this representation
        // object, but not necessarily to destroy 'ptr'.

    ~bcema_OldSharedPtr_RepImpl();
        // Destroy this representation object and delete the managed pointer
        // using the associated deleter.

    // MANIPULATORS
    void deleteObject(TYPE *object);
        // Decrement the number of references to this representation object.
        // If the number of references reaches zero, then destroy this object.

    DELETER_FUNCTOR& deleter();
        // Return a reference to the modifiable deleter used by this
        // representation.

    // ACCESSORS
    bool hasFunctionLikeDeleter() const;
        // Return 'true' if 'DELETER_FUNCTOR' is not a pointer type, and
        // 'false' otherwise.

    void *getDeleter(bsl::type_info const& typeInfo) const;
        // Return the address of the deleter owned by this out-of-place
        // instance if its type matches the specified 'typeInfo', or 0
        // otherwise.

    void *originalPtr() const;
        // Return the pointer value supplied to the representation object at
        // construction.

    TYPE *ptr() const;
        // Return the pointer managed by this representation object.
};

                      // =======================================
                      // class bcema_OldSharedPtr_InplaceRepImpl
                      // =======================================

template <class TYPE>
class bcema_OldSharedPtr_InplaceRepImpl: public bcema_OldSharedPtrRep {
    // This class provides a concrete implementation of the
    // 'bcema_OldSharedPtrRep' protocol for in-place instances of 'TYPE'.

    // DATA
    TYPE d_instance;
                  // instance or beginning of in-place buffer
                  // (* must be last in this layout, as the in-place
                  // uninitialized buffers created by
                  // 'bcema_OldSharedPtrUtil::createInplaceUninitializedBuffer'
                  // may access memory beyond the 'd_instance' footprint *)

  public:
    // CREATORS
    explicit bcema_OldSharedPtr_InplaceRepImpl(bslma_Allocator *allocator = 0);
        // Create "in-place" in this representation object a
        // default-constructed instance of the parameterized type 'TYPE'.

    template <class A1>
    bcema_OldSharedPtr_InplaceRepImpl(bslma_Allocator *allocator,
                                      const A1& a1);
    template <class A1, class A2>
    bcema_OldSharedPtr_InplaceRepImpl(bslma_Allocator *allocator, const A1& a1,
                                      const A2& a2);
    template <class A1, class A2, class A3>
    bcema_OldSharedPtr_InplaceRepImpl(bslma_Allocator *allocator, const A1& a1,
                                      const A2& a2, const A3& a3);
    template <class A1, class A2, class A3, class A4>
    bcema_OldSharedPtr_InplaceRepImpl(bslma_Allocator *allocator, const A1& a1,
                                      const A2& a2, const A3& a3,
                                      const A4& a4);
    template <class A1, class A2, class A3, class A4, class A5>
    bcema_OldSharedPtr_InplaceRepImpl(bslma_Allocator *allocator, const A1& a1,
                                      const A2& a2, const A3& a3, const A4& a4,
                                      const A5& a5);
    template <class A1, class A2, class A3, class A4, class A5, class A6>
    bcema_OldSharedPtr_InplaceRepImpl(bslma_Allocator *allocator, const A1& a1,
                                      const A2& a2, const A3& a3, const A4& a4,
                                      const A5& a5, const A6& a6);
    template <class A1, class A2, class A3, class A4, class A5, class A6,
              class A7>
    bcema_OldSharedPtr_InplaceRepImpl(bslma_Allocator *allocator, const A1& a1,
                                      const A2& a2, const A3& a3, const A4& a4,
                                      const A5& a5, const A6& a6,
                                      const A7& a7);
    template <class A1, class A2, class A3, class A4, class A5, class A6,
              class A7, class A8>
    bcema_OldSharedPtr_InplaceRepImpl(bslma_Allocator *allocator, const A1& a1,
                                      const A2& a2, const A3& a3, const A4& a4,
                                      const A5& a5, const A6& a6, const A7& a7,
                                      const A8& a8);
    template <class A1, class A2, class A3, class A4, class A5, class A6,
              class A7, class A8, class A9>
    bcema_OldSharedPtr_InplaceRepImpl(bslma_Allocator *allocator, const A1& a1,
                                      const A2& a2, const A3& a3, const A4& a4,
                                      const A5& a5, const A6& a6, const A7& a7,
                                      const A8& a8, const A9& a9);
    template <class A1, class A2, class A3, class A4, class A5, class A6,
              class A7, class A8, class A9, class A10>
    bcema_OldSharedPtr_InplaceRepImpl(bslma_Allocator *allocator, const A1& a1,
                                      const A2& a2, const A3& a3, const A4& a4,
                                      const A5& a5, const A6& a6, const A7& a7,
                                      const A8& a8, const A9& a9,
                                      const A10& a10);
    template <class A1, class A2, class A3, class A4, class A5, class A6,
              class A7, class A8, class A9, class A10, class A11>
    bcema_OldSharedPtr_InplaceRepImpl(bslma_Allocator *allocator, const A1& a1,
                                      const A2& a2, const A3& a3, const A4& a4,
                                      const A5& a5, const A6& a6, const A7& a7,
                                      const A8& a8, const A9& a9,
                                      const A10& a10, const A11& a11);
    template <class A1, class A2, class A3, class A4, class A5, class A6,
              class A7, class A8, class A9, class A10, class A11, class A12>
    bcema_OldSharedPtr_InplaceRepImpl(bslma_Allocator *allocator, const A1& a1,
                                      const A2& a2, const A3& a3, const A4& a4,
                                      const A5& a5, const A6& a6, const A7& a7,
                                      const A8& a8, const A9& a9,
                                      const A10& a10, const A11& a11,
                                      const A12& a12);
    template <class A1, class A2, class A3, class A4, class A5, class A6,
              class A7, class A8, class A9, class A10, class A11, class A12,
              class A13>
    bcema_OldSharedPtr_InplaceRepImpl(bslma_Allocator *allocator, const A1& a1,
                                      const A2& a2, const A3& a3, const A4& a4,
                                      const A5& a5, const A6& a6, const A7& a7,
                                      const A8& a8, const A9& a9,
                                      const A10& a10, const A11& a11,
                                      const A12& a12, const A13& a13);
    template <class A1, class A2, class A3, class A4, class A5, class A6,
              class A7, class A8, class A9, class A10, class A11, class A12,
              class A13, class A14>
    bcema_OldSharedPtr_InplaceRepImpl(bslma_Allocator *allocator, const A1& a1,
                                      const A2& a2, const A3& a3, const A4& a4,
                                      const A5& a5, const A6& a6, const A7& a7,
                                      const A8& a8, const A9& a9,
                                      const A10& a10, const A11& a11,
                                      const A12& a12, const A13& a13,
                                      const A14& a14);
        // Create "in-place" in this representation object an instance of the
        // parameterized type 'TYPE' using the 'TYPE' constructor that takes
        // the specified arguments 'a1' up to 'aN' where 'N' (currently at most
        // 14) is the number of arguments passed to this method.

    ~bcema_OldSharedPtr_InplaceRepImpl();
        // Destroy this representation object and the embedded instance of
        // 'TYPE'.

    // MANIPULATORS
    void deleteObject(TYPE *object);
        // Decrement the number of references to this representation object.
        // If the number of references reaches zero, then destroy this object.

    // ACCESSORS
    bool hasFunctionLikeDeleter() const;
        // Return 'true' for this in-place instance.

    void *getDeleter(bsl::type_info const& typeInfo) const;
        // Return the address of the allocator *pointer* owned by this in-place
        // instance if the specified 'typeInfo' matches the type ID of
        // 'bslma_Allocator *', or 0 otherwise.  Note that the allocator
        // pointer must be obtained through one level of dereferencing, as
        // '*static_cast<bslma_Allocator **>(getDeleter())'.

    TYPE *ptr();
        // Return a pointer to the in-place object.

    void *originalPtr() const;
        // Return the pointer value supplied to the representation object at
        // construction.
};

                        // ===============================
                        // class bcema_OldSharedPtr_InitGuard
                        // ===============================

template <class TYPE, class DELETER>
struct bcema_OldSharedPtr_InitGuard {
    // This guard is used for out-of-place shared pointer instantiations.
    // Generally it is instantiated prior to attempting to construct the rep
    // and release after the rep has been successfully constructed.  In the
    // event that an exception is thrown during construction of the rep, the
    // guard will delete the provided pointer using the provided deleter.  Note
    // that the provided deleter is held by reference and must remain valid for
    // the lifetime of this guard.  If this guard is not release before it's
    // destruction, a copy of the deleter is instantiated to delete the
    // pointer(in case operator() is non-const).  Also note that if the deleter
    // throws during copy-construction, the provided pointer will not be
    // destroyed.

    // PRIVATE TYPES
    struct PointerTag {};
    struct NonPointerTag {};

    // INSTANCE DATA
    TYPE          *d_ptr_p;
    const DELETER& d_deleter;

    // PRIVATE MANIPULATORS
    template <class FACTORY>
    void doDeleteObject(TYPE *ptr, FACTORY *factory, PointerTag);

    template < class RET, class A1>
    void doDeleteObject(TYPE *ptr, RET(*deleterFunc)(A1), PointerTag);

    template <class DELETER_TYPE>
    void doDeleteObject(TYPE *ptr, const DELETER_TYPE& deleter, NonPointerTag);

  public:
    // CREATORS
    bcema_OldSharedPtr_InitGuard(TYPE *ptr, const DELETER& deleter);

    ~bcema_OldSharedPtr_InitGuard();
        // Destroy this guard.

    // MANIPULATORS
    void release();
};

// ============================================================================
//                      INLINE AND TEMPLATE FUNCTION IMPLEMENTATIONS
// ============================================================================
// IMPLEMENTATION NOTES
//
// The following expression is a class invariant of 'bcema_OldSharedPtr' and
// shall always return 'true':
//..
//  d_rep_p == 0 || d_ptr_p != 0
//..

                        // --------------------------
                        // class bcema_OldSharedPtrRep
                        // --------------------------

// CREATORS
inline
bcema_OldSharedPtrRep::bcema_OldSharedPtrRep(bslma_Allocator *allocator)
: d_count(1)
, d_allocator_p(allocator)
{
    BSLS_ASSERT_SAFE(allocator);
}

// MANIPULATORS
inline
void bcema_OldSharedPtrRep::incrementRefs()
{
    ++this->d_count;
}

inline
int bcema_OldSharedPtrRep::decrementRefs()
{
    return --this->d_count;
}

// ACCESSORS
inline
int bcema_OldSharedPtrRep::numReferences() const
{
    return this->d_count;
}

inline
bslma_Allocator *bcema_OldSharedPtrRep::allocator() const
{
    // TBD  This function should be made pure virtual in this abstract base
    // class, and both member and implementation moved to derived concrete
    // implementations.

    return this->d_allocator_p;
}

                            // ---------------------
                            // class bcema_OldSharedPtr
                            // ---------------------

// PRIVATE CREATORS
template <class TYPE>
inline
bcema_OldSharedPtr<TYPE>::bcema_OldSharedPtr(TYPE                  *ptr,
                                             bcema_OldSharedPtrRep *rep)
: d_ptr_p(ptr)
, d_rep_p(rep)
{
}

// PRIVATE MANIPULATORS
template <class TYPE>
template <class OTHER_TYPE, class DELETER>
void bcema_OldSharedPtr<TYPE>::makeRep(OTHER_TYPE      *ptr,
                                       const DELETER&   deleter,
                                       bslma_Allocator *allocator)
{
    this->d_ptr_p = ptr;
    if (this->d_ptr_p) {
        allocator = bslma_Default::allocator(allocator);
        this->d_rep_p = new(*allocator)
            bcema_OldSharedPtr_RepImpl<OTHER_TYPE, DELETER>(ptr,
                                                            deleter,
                                                            allocator);
    } else {
        this->d_rep_p = 0;
    }
}

template <class TYPE>
void bcema_OldSharedPtr<TYPE>::doReset(TYPE               *ptr,
                                    bcema_OldSharedPtrRep *rep)
{
    bcema_OldSharedPtr<TYPE> guard(this->d_ptr_p, this->d_rep_p);

    this->d_ptr_p = ptr;
    if (ptr) {
        this->d_rep_p = rep;
        rep->incrementRefs();
    }
    else this->d_rep_p = 0;
}

// CREATORS
template <class TYPE>
inline
bcema_OldSharedPtr<TYPE>::bcema_OldSharedPtr()
: d_ptr_p(0)
, d_rep_p(0)
{
}

template <class TYPE>
inline
bcema_OldSharedPtr<TYPE>::bcema_OldSharedPtr(TYPE            *ptr,
                                             bslma_Allocator *allocator)
{
    bcema_OldSharedPtr_InitGuard<TYPE,bslma_Allocator*> guard(ptr,allocator);
    this->makeRep(ptr, allocator, allocator);
    guard.release();
}

template <class TYPE>
template <class OTHER_TYPE>
bcema_OldSharedPtr<TYPE>::bcema_OldSharedPtr(OTHER_TYPE      *ptr,
                                             bslma_Allocator *allocator)
{
    bcema_OldSharedPtr_InitGuard<OTHER_TYPE, bslma_Allocator*>
                                                         guard(ptr, allocator);
    this->makeRep(ptr, allocator, allocator);
    guard.release();
}

template <class TYPE>
template <class OTHER_TYPE>
bcema_OldSharedPtr<TYPE>::bcema_OldSharedPtr(
                                      bdema_ManagedPtr<OTHER_TYPE>  managedPtr,
                                      bslma_Allocator              *allocator)
: d_ptr_p(managedPtr.ptr())
, d_rep_p(0)
{
    typedef bcema_OldSharedPtr_InplaceRepImpl<bdema_ManagedPtr<TYPE> > Rep;

    if (d_ptr_p) {
        typedef void(*ManagedPtrDeleter)(void *, bcema_OldSharedPtrRep *);

        if (&bcema_OldSharedPtrRep::managedPtrDeleter ==
            (ManagedPtrDeleter)managedPtr.deleter().deleter()) {
            this->d_rep_p = (bcema_OldSharedPtrRep*)
                managedPtr.release().second.factory();
        }
        else {
            allocator = bslma_Default::allocator(allocator);
            Rep *rep = new(*allocator) Rep(allocator);
            (*rep->ptr())  = managedPtr;
            this->d_rep_p = rep;
        }
    }
}

template <class TYPE>
template <class OTHER_TYPE>
bcema_OldSharedPtr<TYPE>::bcema_OldSharedPtr(
                                          bsl::auto_ptr<OTHER_TYPE>&  autoPtr,
                                          bslma_Allocator           *allocator)
: d_ptr_p(autoPtr.get())
, d_rep_p(0)
{
    typedef bcema_OldSharedPtr_InplaceRepImpl<bsl::auto_ptr<OTHER_TYPE> > Rep;

    if (d_ptr_p) {
        allocator = bslma_Default::allocator(allocator);
        Rep *rep = new(*allocator) Rep(allocator);
        (*rep->ptr())  = autoPtr;
        this->d_rep_p = rep;
    }
}

template <class TYPE>
template <class OTHER_TYPE, class DELETER>
bcema_OldSharedPtr<TYPE>::bcema_OldSharedPtr(OTHER_TYPE      *ptr,
                                             const DELETER&   deleter,
                                             bslma_Allocator *allocator)
: d_ptr_p(ptr)
, d_rep_p(0)
{
    bcema_OldSharedPtr_InitGuard<OTHER_TYPE, DELETER> guard(ptr, deleter);
    this->makeRep(ptr, deleter, allocator);
    guard.release();
}

template <class TYPE>
template <class OTHER_TYPE>
bcema_OldSharedPtr<TYPE>::bcema_OldSharedPtr(
                                 bcema_OldSharedPtr<OTHER_TYPE> const&  source,
                                 TYPE                                  *object)
: d_ptr_p(object)
, d_rep_p(source.d_rep_p)
{
    if (this->d_ptr_p && this->d_rep_p) {
        this->d_rep_p->incrementRefs();
    } else {
        this->d_ptr_p = 0;
        this->d_rep_p = 0;
    }
}

template <class TYPE>
template <class OTHER_TYPE>
bcema_OldSharedPtr<TYPE>::bcema_OldSharedPtr(
                                bcema_OldSharedPtr<OTHER_TYPE> const& original)
: d_ptr_p(original.d_ptr_p)
, d_rep_p(original.d_rep_p)
{
    if (this->d_ptr_p) {
        this->d_rep_p->incrementRefs();
    } else {
        this->d_rep_p = 0;
    }
}

template <class TYPE>
inline
bcema_OldSharedPtr<TYPE>::bcema_OldSharedPtr(
                                      bcema_OldSharedPtr<TYPE> const& original)
: d_ptr_p(original.d_ptr_p)
, d_rep_p(original.d_rep_p)
{
    if (this->d_ptr_p) {
        this->d_rep_p->incrementRefs();
    } else {
        this->d_rep_p = 0;
    }
}

template <class TYPE>
bcema_OldSharedPtr<TYPE>::~bcema_OldSharedPtr()
{
    BSLS_ASSERT_SAFE(d_rep_p == 0 || d_ptr_p != 0);
    if (this->d_rep_p && !this->d_rep_p->decrementRefs()) {
        this->d_rep_p->allocator()->deleteObject(d_rep_p);
    }
}

// MANIPULATORS
template <class TYPE>
bcema_OldSharedPtr<TYPE>& bcema_OldSharedPtr<TYPE>::
                                 operator=(bcema_OldSharedPtr<TYPE> const& rhs)
{
    if (&rhs == this) {
        return *this;
    }
    this->doReset(rhs.d_ptr_p, rhs.d_rep_p);

    return *this;
}

template <class TYPE>
template <class OTHER_TYPE>
bcema_OldSharedPtr<TYPE>& bcema_OldSharedPtr<TYPE>::operator=(
                                     const bcema_OldSharedPtr<OTHER_TYPE>& rhs)
{
    if ((bcema_OldSharedPtr<TYPE>*)&rhs == this) {
        return *this;
    }
    this->doReset(rhs.d_ptr_p, rhs.d_rep_p);
    return *this;
}

template <class TYPE>
template <class OTHER_TYPE>
bcema_OldSharedPtr<TYPE>& bcema_OldSharedPtr<TYPE>::operator=(
                                                bsl::auto_ptr<OTHER_TYPE>& rhs)
{
    bcema_OldSharedPtr<TYPE> newPtr(rhs);
    newPtr.swap(*this);
    return *this;
}

template <class TYPE>
void bcema_OldSharedPtr<TYPE>::clear()
{
    if (this->d_rep_p && !this->d_rep_p->decrementRefs()) {
        this->d_rep_p->allocator()->deleteObject(this->d_rep_p);
    }
    this->d_ptr_p = 0;
    this->d_rep_p = 0;
}

template <class TYPE>
template <class OTHER_TYPE>
void bcema_OldSharedPtr<TYPE>::load(OTHER_TYPE      *ptr,
                                    bslma_Allocator *allocator)
{
    SelfType(ptr, allocator).swap(*this);
}

template <class TYPE>
template <class OTHER_TYPE, class DELETER>
void bcema_OldSharedPtr<TYPE>::load(OTHER_TYPE   *ptr,
                                 const DELETER&   deleter,
                                 bslma_Allocator *allocator)
{
    SelfType(ptr, deleter, allocator).swap(*this);
}

template <class TYPE>
template <class OTHER_TYPE>
void bcema_OldSharedPtr<TYPE>::loadAlias(
        bcema_OldSharedPtr<OTHER_TYPE> const&  source,
        TYPE                              *object)
{
    doReset(object ? (source.ptr() ? object : 0) : 0, source.d_rep_p);
}

template <class TYPE>
void bcema_OldSharedPtr<TYPE>::createInplace(bslma_Allocator *allocator)
{
    typedef bcema_OldSharedPtr_InplaceRepImpl<TYPE> Rep;
    allocator = bslma_Default::allocator(allocator);
    Rep *rep = new(*allocator) Rep(allocator);
    SelfType(rep->ptr(), rep).swap(*this);
}

template <class TYPE>
template <class A1>
void bcema_OldSharedPtr<TYPE>::createInplace(bslma_Allocator *allocator,
       const A1& a1)
{
    typedef bcema_OldSharedPtr_InplaceRepImpl<TYPE> Rep;
    allocator = bslma_Default::allocator(allocator);
    Rep *rep = new(*allocator) Rep(allocator, a1);
    SelfType(rep->ptr(), rep).swap(*this);
}

template <class TYPE>
template <class A1, class A2>
void bcema_OldSharedPtr<TYPE>::createInplace(bslma_Allocator *allocator,
        const A1& a1, const A2& a2)
{
    typedef bcema_OldSharedPtr_InplaceRepImpl<TYPE> Rep;
    allocator = bslma_Default::allocator(allocator);
    Rep *rep = new(*allocator) Rep(allocator, a1, a2);
    SelfType(rep->ptr(), rep).swap(*this);
}

template <class TYPE>
template <class A1, class A2, class A3>
void bcema_OldSharedPtr<TYPE>::createInplace(bslma_Allocator *allocator,
        const A1& a1, const A2& a2, const A3& a3)
{
    typedef bcema_OldSharedPtr_InplaceRepImpl<TYPE> Rep;
    allocator = bslma_Default::allocator(allocator);
    Rep *rep = new(*allocator) Rep(allocator, a1, a2, a3);
    SelfType(rep->ptr(), rep).swap(*this);
}

template <class TYPE>
template <class A1, class A2, class A3, class A4>
void bcema_OldSharedPtr<TYPE>::createInplace(bslma_Allocator *allocator,
        const A1& a1, const A2& a2, const A3& a3, const A4& a4)
{
    typedef bcema_OldSharedPtr_InplaceRepImpl<TYPE> Rep;
    allocator = bslma_Default::allocator(allocator);
    Rep *rep = new(*allocator) Rep(allocator, a1, a2, a3, a4);
    SelfType(rep->ptr(), rep).swap(*this);
}

template <class TYPE>
template <class A1, class A2, class A3, class A4, class A5>
void bcema_OldSharedPtr<TYPE>::createInplace(bslma_Allocator *allocator,
        const A1& a1, const A2& a2, const A3& a3, const A4& a4,
        const A5& a5)
{
    typedef bcema_OldSharedPtr_InplaceRepImpl<TYPE> Rep;
    allocator = bslma_Default::allocator(allocator);
    Rep *rep = new(*allocator) Rep(allocator, a1, a2, a3, a4, a5);
    SelfType(rep->ptr(), rep).swap(*this);
}

template <class TYPE>
template <class A1, class A2, class A3, class A4, class A5, class A6>
void bcema_OldSharedPtr<TYPE>::createInplace(bslma_Allocator *allocator,
        const A1& a1, const A2& a2, const A3& a3, const A4& a4,
        const A5& a5, const A6& a6)
{
    typedef bcema_OldSharedPtr_InplaceRepImpl<TYPE> Rep;
    allocator = bslma_Default::allocator(allocator);
    Rep *rep = new(*allocator) Rep(allocator, a1, a2, a3, a4, a5, a6);
    SelfType(rep->ptr(), rep).swap(*this);
}

template <class TYPE>
template <class A1, class A2, class A3, class A4, class A5, class A6,
          class A7>
void bcema_OldSharedPtr<TYPE>::createInplace(bslma_Allocator *allocator,
        const A1& a1, const A2& a2, const A3& a3, const A4& a4,
        const A5& a5, const A6& a6, const A7& a7)
{
    typedef bcema_OldSharedPtr_InplaceRepImpl<TYPE> Rep;
    allocator = bslma_Default::allocator(allocator);
    Rep *rep = new(*allocator) Rep(allocator, a1, a2, a3, a4, a5, a6, a7);
    SelfType(rep->ptr(), rep).swap(*this);
}

template <class TYPE>
template <class A1, class A2, class A3, class A4, class A5, class A6,
          class A7, class A8>
void bcema_OldSharedPtr<TYPE>::createInplace(bslma_Allocator *allocator,
        const A1& a1, const A2& a2, const A3& a3, const A4& a4,
        const A5& a5, const A6& a6, const A7& a7, const A8& a8)
{
    typedef bcema_OldSharedPtr_InplaceRepImpl<TYPE> Rep;
    allocator = bslma_Default::allocator(allocator);
    Rep *rep = new(*allocator) Rep(allocator, a1, a2, a3, a4, a5, a6, a7,
                                   a8);
    SelfType(rep->ptr(), rep).swap(*this);
}

template <class TYPE>
template <class A1, class A2, class A3, class A4, class A5, class A6,
          class A7, class A8, class A9>
void bcema_OldSharedPtr<TYPE>::createInplace(bslma_Allocator *allocator,
        const A1& a1, const A2& a2, const A3& a3, const A4& a4,
        const A5& a5, const A6& a6, const A7& a7, const A8& a8,
        const A9& a9)
{
    typedef bcema_OldSharedPtr_InplaceRepImpl<TYPE> Rep;
    allocator = bslma_Default::allocator(allocator);
    Rep *rep = new(*allocator) Rep(allocator, a1, a2, a3, a4, a5, a6, a7,
                                   a8, a9);
    SelfType(rep->ptr(), rep).swap(*this);
}

template <class TYPE>
template <class A1, class A2, class A3, class A4, class A5, class A6,
          class A7, class A8, class A9, class A10>
void bcema_OldSharedPtr<TYPE>::createInplace(bslma_Allocator *allocator,
        const A1& a1, const A2& a2, const A3& a3, const A4& a4,
        const A5& a5, const A6& a6, const A7& a7, const A8& a8,
        const A9& a9, const A10& a10)
{
    typedef bcema_OldSharedPtr_InplaceRepImpl<TYPE> Rep;
    allocator = bslma_Default::allocator(allocator);
    Rep *rep = new(*allocator) Rep(allocator, a1, a2, a3, a4, a5, a6, a7,
                                   a8, a9, a10);
    SelfType(rep->ptr(), rep).swap(*this);
}

template <class TYPE>
template <class A1, class A2, class A3, class A4, class A5, class A6,
          class A7, class A8, class A9, class A10, class A11>
void bcema_OldSharedPtr<TYPE>::createInplace(bslma_Allocator *allocator,
        const A1& a1, const A2& a2, const A3& a3, const A4& a4,
        const A5& a5, const A6& a6, const A7& a7, const A8& a8,
        const A9& a9, const A10& a10, const A11& a11)
{
    typedef bcema_OldSharedPtr_InplaceRepImpl<TYPE> Rep;
    allocator = bslma_Default::allocator(allocator);
    Rep *rep = new(*allocator) Rep(allocator, a1, a2, a3, a4, a5, a6, a7,
                                   a8, a9, a10, a11);
    SelfType(rep->ptr(), rep).swap(*this);
}

template <class TYPE>
template <class A1, class A2, class A3, class A4, class A5, class A6,
          class A7, class A8, class A9, class A10, class A11, class A12>
void bcema_OldSharedPtr<TYPE>::createInplace(bslma_Allocator *allocator,
        const A1& a1, const A2& a2, const A3& a3, const A4& a4,
        const A5& a5, const A6& a6, const A7& a7, const A8& a8,
        const A9& a9, const A10& a10, const A11& a11, const A12& a12)
{
    typedef bcema_OldSharedPtr_InplaceRepImpl<TYPE> Rep;
    allocator = bslma_Default::allocator(allocator);
    Rep *rep = new(*allocator) Rep(allocator, a1, a2, a3, a4, a5, a6, a7,
                                   a8, a9, a10, a11, a12);
    SelfType(rep->ptr(), rep).swap(*this);
}

template <class TYPE>
template <class A1, class A2, class A3, class A4, class A5, class A6,
          class A7, class A8, class A9, class A10, class A11, class A12,
          class A13>
void bcema_OldSharedPtr<TYPE>::createInplace(bslma_Allocator *allocator,
        const A1& a1, const A2& a2, const A3& a3, const A4& a4,
        const A5& a5, const A6& a6, const A7& a7, const A8& a8,
        const A9& a9, const A10& a10, const A11& a11, const A12& a12,
        const A13& a13)
{
    typedef bcema_OldSharedPtr_InplaceRepImpl<TYPE> Rep;
    allocator = bslma_Default::allocator(allocator);
    Rep *rep = new(*allocator) Rep(allocator, a1, a2, a3, a4, a5, a6, a7,
                                   a8, a9, a10, a11, a12, a13);
    SelfType(rep->ptr(), rep).swap(*this);
}

template <class TYPE>
template <class A1, class A2, class A3, class A4, class A5, class A6,
          class A7, class A8, class A9, class A10, class A11, class A12,
          class A13, class A14>
void bcema_OldSharedPtr<TYPE>::createInplace(bslma_Allocator *allocator,
        const A1& a1, const A2& a2, const A3& a3, const A4& a4,
        const A5& a5, const A6& a6, const A7& a7, const A8& a8,
        const A9& a9, const A10& a10, const A11& a11, const A12& a12,
        const A13& a13, const A14& a14)
{
    typedef bcema_OldSharedPtr_InplaceRepImpl<TYPE> Rep;
    allocator = bslma_Default::allocator(allocator);
    Rep *rep = new(*allocator) Rep(allocator, a1, a2, a3, a4, a5, a6, a7,
                                   a8, a9, a10, a11, a12, a13, a14);
    SelfType(rep->ptr(), rep).swap(*this);
}

template <class TYPE>
void bcema_OldSharedPtr<TYPE>::swap(bcema_OldSharedPtr<TYPE>& src)
{
    bsl::swap(this->d_ptr_p, src.d_ptr_p);
    bsl::swap(this->d_rep_p, src.d_rep_p);
}

// ACCESSORS
template <class TYPE>
inline
bcema_OldSharedPtr<TYPE>::operator bool() const
{
    return this->d_ptr_p;
}

template <class TYPE>
inline
typename bcema_OldSharedPtr<TYPE>::Reference
    bcema_OldSharedPtr<TYPE>::operator[](bsl::ptrdiff_t index) const
{
    return *(this->d_ptr_p + index);
}

template <class TYPE>
inline
typename bcema_OldSharedPtr<TYPE>::Reference
    bcema_OldSharedPtr<TYPE>::operator*() const
{
    return *this->d_ptr_p;
}

template <class TYPE>
inline
TYPE *bcema_OldSharedPtr<TYPE>::operator->() const
{
    return this->d_ptr_p;
}

template <class TYPE>
inline
TYPE *bcema_OldSharedPtr<TYPE>::ptr() const
{
    return this->d_ptr_p;
}

template <class TYPE>
inline
int bcema_OldSharedPtr<TYPE>::numReferences() const
{
    return this->d_rep_p ? d_rep_p->numReferences() : 0;
}

template <class TYPE>
bdema_ManagedPtr<TYPE> bcema_OldSharedPtr<TYPE>::managedPtr() const
{
    typedef void(*Deleter)(TYPE*, bcema_OldSharedPtrRep*);
    if (this->d_rep_p) {
        this->d_rep_p->incrementRefs();
    }
    bdema_ManagedPtr<TYPE> ptr(this->d_ptr_p, this->d_rep_p,
                           (Deleter)&bcema_OldSharedPtrRep::managedPtrDeleter);
    return ptr;
}

                        // *** bsl::tr1 COMPATIBILITY ***
// MANIPULATORS
template <class TYPE>
inline
void bcema_OldSharedPtr<TYPE>::reset()
{
    clear();
}

template <class TYPE>
template <class OTHER_TYPE>
void bcema_OldSharedPtr<TYPE>::reset(OTHER_TYPE *ptr)
{
    // Wrap 'ptr' in 'auto_ptr' to ensure standard delete behavior.
    bsl::auto_ptr<OTHER_TYPE> ap(ptr);
    SelfType(ap).swap(*this);
}

template <class TYPE>
template <class OTHER_TYPE, class DELETER>
void bcema_OldSharedPtr<TYPE>::reset(OTHER_TYPE *ptr, const DELETER& deleter)
{
    SelfType(ptr, deleter, 0).swap(*this);
}

template <class TYPE>
template <class OTHER_TYPE>
void bcema_OldSharedPtr<TYPE>::reset(
                                 bcema_OldSharedPtr<OTHER_TYPE> const&  source,
                                 TYPE                                  *ptr)
{
    SelfType(source, ptr).swap(*this);
}

// ACCESSORS
template <class TYPE>
inline
TYPE *bcema_OldSharedPtr<TYPE>::get() const
{
    return this->ptr();
}

template <class TYPE>
inline
bool bcema_OldSharedPtr<TYPE>::unique() const
{
    return 1 == this->numReferences();
}

template <class TYPE>
inline
int bcema_OldSharedPtr<TYPE>::use_count() const
{
    return this->numReferences();
}

// FREE OPERATORS

template <typename LHS_TYPE, typename RHS_TYPE>
inline
bool operator==(const bcema_OldSharedPtr<LHS_TYPE>& lhs,
                const bcema_OldSharedPtr<RHS_TYPE>& rhs)
{
    return rhs.ptr() == lhs.ptr();
}

template <typename LHS_TYPE, typename RHS_TYPE>
inline
bool operator!=(const bcema_OldSharedPtr<LHS_TYPE>& lhs,
                const bcema_OldSharedPtr<RHS_TYPE>& rhs)
{
    return ! (lhs == rhs);
}

template <typename LHS_TYPE, typename RHS_TYPE>
inline
bool operator<(const bcema_OldSharedPtr<LHS_TYPE>& lhs,
               const bcema_OldSharedPtr<RHS_TYPE>& rhs)
{
    return lhs.ptr() < rhs.ptr();
}

template <typename LHS_TYPE, typename RHS_TYPE>
inline
bool operator<=(const bcema_OldSharedPtr<LHS_TYPE>& lhs,
                const bcema_OldSharedPtr<RHS_TYPE>& rhs)
{
    return lhs.ptr() <= rhs.ptr();
}

template <typename LHS_TYPE, typename RHS_TYPE>
inline
bool operator>(const bcema_OldSharedPtr<LHS_TYPE>& lhs,
               const bcema_OldSharedPtr<RHS_TYPE>& rhs)
{
    return lhs.ptr() > rhs.ptr();
}

template <typename LHS_TYPE, typename RHS_TYPE>
inline
bool operator>=(const bcema_OldSharedPtr<LHS_TYPE>& lhs,
                const bcema_OldSharedPtr<RHS_TYPE>& rhs)
{
    return lhs.ptr() >= rhs.ptr();
}

template <typename TYPE>
bsl::ostream& operator<<(bsl::ostream&                stream,
                         const bcema_OldSharedPtr<TYPE>& rhs)
{
    stream << rhs.ptr();
    return stream;
}

                        // *** bsl::tr1 COMPATIBILITY ***

template <class TARGET, class SOURCE>
bcema_OldSharedPtr<TARGET>
const_pointer_cast(bcema_OldSharedPtr<SOURCE> const& rhs)
{
    return bcema_OldSharedPtr<TARGET>(rhs,
                                   const_cast<TARGET*>(rhs.ptr()));
}

template <class TARGET, class SOURCE>
bcema_OldSharedPtr<TARGET>
dynamic_pointer_cast(bcema_OldSharedPtr<SOURCE> const& rhs)
{
    return bcema_OldSharedPtr<TARGET>(rhs,
                                   dynamic_cast<TARGET*>(rhs.ptr()));
}

template <typename DELETER, typename TYPE>
DELETER *get_deleter(const bcema_OldSharedPtr<TYPE>& rhs)
{
    if (rhs.d_rep_p) {
        if (rhs.d_rep_p->hasFunctionLikeDeleter()) {
            return static_cast<DELETER *>(
                                     rhs.d_rep_p->getDeleter(typeid(DELETER)));
        }
        void *deleterPtr = rhs.d_rep_p->getDeleter(typeid(DELETER *));
        return deleterPtr ? *static_cast<DELETER **>(deleterPtr)
                          : 0;
    }
    return 0;
}

template <class TARGET, class SOURCE>
bcema_OldSharedPtr<TARGET>
static_pointer_cast(bcema_OldSharedPtr<SOURCE> const& rhs)
{
    return bcema_OldSharedPtr<TARGET>(rhs,
                                   static_cast<TARGET*>(rhs.ptr()));
}

template <class TYPE>
void swap(bcema_OldSharedPtr<TYPE>& lhs, bcema_OldSharedPtr<TYPE>& rhs)
{
    lhs.swap(rhs);
}

template <class TYPE>
bcema_OldSharedPtr<TYPE>
make_shared()
{
    bcema_OldSharedPtr<TYPE> x;
    x.createInplace(0);
    return x;
}

template <class TYPE, class A1>
bcema_OldSharedPtr<TYPE>
make_shared(const A1& a1)
{
    bcema_OldSharedPtr<TYPE> x;
    x.createInplace(0, a1);
    return x;
}

template <class TYPE, class A1, class A2>
bcema_OldSharedPtr<TYPE>
make_shared(const A1& a1, const A2& a2)
{
    bcema_OldSharedPtr<TYPE> x;
    x.createInplace(0, a1, a2);
    return x;
}

template <class TYPE, class A1, class A2, class A3>
bcema_OldSharedPtr<TYPE>
make_shared(const A1& a1, const A2& a2, const A3& a3)
{
    bcema_OldSharedPtr<TYPE> x;
    x.createInplace(0, a1, a2, a3);
    return x;
}

template <class TYPE, class A1, class A2, class A3, class A4>
bcema_OldSharedPtr<TYPE>
make_shared(const A1& a1, const A2& a2, const A3& a3, const A4& a4)
{
    bcema_OldSharedPtr<TYPE> x;
    x.createInplace(0, a1, a2, a3, a4);
    return x;
}

template <class TYPE, class A1, class A2, class A3, class A4, class A5>
bcema_OldSharedPtr<TYPE>
make_shared(const A1& a1, const A2& a2, const A3& a3, const A4& a4,
            const A5& a5)
{
    bcema_OldSharedPtr<TYPE> x;
    x.createInplace(0, a1, a2, a3, a4, a5);
    return x;
}

template <class TYPE, class A1, class A2, class A3, class A4, class A5,
          class A6>
bcema_OldSharedPtr<TYPE>
make_shared(const A1& a1, const A2& a2, const A3& a3, const A4& a4,
            const A5& a5, const A6& a6)
{
    bcema_OldSharedPtr<TYPE> x;
    x.createInplace(0, a1, a2, a3, a4, a5, a6);
    return x;
}

template <class TYPE, class A1, class A2, class A3, class A4, class A5,
          class A6,   class A7>
bcema_OldSharedPtr<TYPE>
make_shared(const A1& a1, const A2& a2, const A3& a3, const A4& a4,
            const A5& a5, const A6& a6, const A7& a7)
{
    bcema_OldSharedPtr<TYPE> x;
    x.createInplace(0, a1, a2, a3, a4, a5, a6, a7);
    return x;
}

template <class TYPE, class A1, class A2, class A3, class A4, class A5,
          class A6,   class A7, class A8>
bcema_OldSharedPtr<TYPE>
make_shared(const A1& a1, const A2& a2, const A3& a3, const A4& a4,
            const A5& a5, const A6& a6, const A7& a7, const A8& a8)
{
    bcema_OldSharedPtr<TYPE> x;
    x.createInplace(0, a1, a2, a3, a4, a5, a6, a7, a8);
    return x;
}

template <class TYPE, class A1, class A2, class A3, class A4, class A5,
          class A6,   class A7, class A8, class A9>
bcema_OldSharedPtr<TYPE>
make_shared(const A1& a1, const A2& a2, const A3& a3, const A4& a4,
            const A5& a5, const A6& a6, const A7& a7, const A8& a8,
            const A9& a9)
{
    bcema_OldSharedPtr<TYPE> x;
    x.createInplace(0, a1, a2, a3, a4, a5, a6, a7, a8, a9);
    return x;
}

template <class TYPE, class A1, class A2, class A3, class A4, class A5,
          class A6,   class A7, class A8, class A9, class A10>
bcema_OldSharedPtr<TYPE>
make_shared(const A1& a1, const A2&  a2, const A3& a3, const A4& a4,
            const A5& a5, const A6&  a6, const A7& a7, const A8& a8,
            const A9& a9, const A10& a10)
{
    bcema_OldSharedPtr<TYPE> x;
    x.createInplace(0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10);
    return x;
}

template <class TYPE, class A1, class A2, class A3, class A4,  class A5,
          class A6,   class A7, class A8, class A9, class A10, class A11>
bcema_OldSharedPtr<TYPE>
make_shared(const A1& a1, const A2&  a2,  const A3&  a3, const A4& a4,
            const A5& a5, const A6&  a6,  const A7&  a7, const A8& a8,
            const A9& a9, const A10& a10, const A11& a11)
{
    bcema_OldSharedPtr<TYPE> x;
    x.createInplace(0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11);
    return x;
}

template <class TYPE, class A1, class A2, class A3, class A4,  class A5,
          class A6,   class A7, class A8, class A9, class A10, class A11,
          class A12>
bcema_OldSharedPtr<TYPE>
make_shared(const A1& a1, const A2&  a2,  const A3&  a3,  const A4&  a4,
            const A5& a5, const A6&  a6,  const A7&  a7,  const A8&  a8,
            const A9& a9, const A10& a10, const A11& a11, const A12& a12)
{
    bcema_OldSharedPtr<TYPE> x;
    x.createInplace(0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12);
    return x;
}

template <class TYPE, class A1, class A2, class A3, class A4,  class A5,
          class A6,   class A7, class A8, class A9, class A10, class A11,
          class A12,  class A13>
bcema_OldSharedPtr<TYPE>
make_shared(const A1&  a1, const A2&  a2,  const A3&  a3,  const A4&  a4,
            const A5&  a5, const A6&  a6,  const A7&  a7,  const A8&  a8,
            const A9&  a9, const A10& a10, const A11& a11, const A12& a12,
            const A13& a13)
{
    bcema_OldSharedPtr<TYPE> x;
    x.createInplace(0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13);
    return x;
}

template <class TYPE, class A1,  class A2, class A3, class A4,  class A5,
          class A6,   class A7,  class A8, class A9, class A10, class A11,
          class A12,  class A13, class A14>
bcema_OldSharedPtr<TYPE>
make_shared(const A1&  a1,  const A2&  a2,  const A3&  a3,  const A4&  a4,
            const A5&  a5,  const A6&  a6,  const A7&  a7,  const A8&  a8,
            const A9&  a9,  const A10& a10, const A11& a11, const A12& a12,
            const A13& a13, const A14& a14)
{
    bcema_OldSharedPtr<TYPE> x;
    x.createInplace(0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13,
                       a14);
    return x;
}

template <class TYPE>
bcema_OldSharedPtr<TYPE>
allocate_shared(bslma_Allocator *allocator)
{
    bcema_OldSharedPtr<TYPE> x;
    x.createInplace(allocator);
    return x;
}

template <class TYPE, class A1>
bcema_OldSharedPtr<TYPE>
allocate_shared(bslma_Allocator *allocator, const A1& a1)
{
    bcema_OldSharedPtr<TYPE> x;
    x.createInplace(allocator, a1);
    return x;
}

template <class TYPE, class A1, class A2>
bcema_OldSharedPtr<TYPE>
allocate_shared(bslma_Allocator *allocator, const A1& a1, const A2& a2)
{
    bcema_OldSharedPtr<TYPE> x;
    x.createInplace(allocator, a1, a2);
    return x;
}

template <class TYPE, class A1, class A2, class A3>
bcema_OldSharedPtr<TYPE>
allocate_shared(bslma_Allocator *allocator, const A1& a1, const A2& a2,
                const A3& a3)
{
    bcema_OldSharedPtr<TYPE> x;
    x.createInplace(allocator, a1, a2, a3);
    return x;
}

template <class TYPE, class A1, class A2, class A3, class A4>
bcema_OldSharedPtr<TYPE>
allocate_shared(bslma_Allocator *allocator, const A1& a1, const A2& a2,
                const A3& a3, const A4& a4)
{
    bcema_OldSharedPtr<TYPE> x;
    x.createInplace(allocator, a1, a2, a3, a4);
    return x;
}

template <class TYPE, class A1, class A2, class A3, class A4,
          class A5>
bcema_OldSharedPtr<TYPE>
allocate_shared(bslma_Allocator *allocator, const A1& a1, const A2& a2,
                const A3& a3, const A4& a4, const A5& a5)
{
    bcema_OldSharedPtr<TYPE> x;
    x.createInplace(allocator, a1, a2, a3, a4, a5);
    return x;
}

template <class TYPE, class A1, class A2, class A3, class A4,
          class A5, class A6>
bcema_OldSharedPtr<TYPE>
allocate_shared(bslma_Allocator *allocator, const A1& a1, const A2& a2,
                const A3& a3, const A4& a4, const A5& a5, const A6& a6)
{
    bcema_OldSharedPtr<TYPE> x;
    x.createInplace(allocator, a1, a2, a3, a4, a5, a6);
    return x;
}

template <class TYPE, class A1, class A2, class A3, class A4,
          class A5, class A6, class A7>
bcema_OldSharedPtr<TYPE>
allocate_shared(bslma_Allocator *allocator, const A1& a1, const A2& a2,
                const A3& a3, const A4& a4, const A5& a5, const A6& a6,
                const A7& a7)
{
    bcema_OldSharedPtr<TYPE> x;
    x.createInplace(allocator, a1, a2, a3, a4, a5, a6, a7);
    return x;
}

template <class TYPE, class A1, class A2, class A3, class A4,
          class A5, class A6, class A7, class A8>
bcema_OldSharedPtr<TYPE>
allocate_shared(bslma_Allocator *allocator, const A1& a1, const A2& a2,
                const A3& a3, const A4& a4, const A5& a5, const A6& a6,
                const A7& a7, const A8& a8)
{
    bcema_OldSharedPtr<TYPE> x;
    x.createInplace(allocator, a1, a2, a3, a4, a5, a6, a7, a8);
    return x;
}

template <class TYPE, class A1, class A2, class A3, class A4,
          class A5, class A6, class A7, class A8, class A9>
bcema_OldSharedPtr<TYPE>
allocate_shared(bslma_Allocator *allocator, const A1& a1, const A2& a2,
                const A3& a3, const A4& a4, const A5& a5, const A6& a6,
                const A7& a7, const A8& a8, const A9& a9)
{
    bcema_OldSharedPtr<TYPE> x;
    x.createInplace(allocator, a1, a2, a3, a4, a5, a6, a7, a8, a9);
    return x;
}

template <class TYPE, class A1, class A2, class A3, class A4,
          class A5, class A6, class A7, class A8, class A9, class A10>
bcema_OldSharedPtr<TYPE>
allocate_shared(bslma_Allocator *allocator, const A1& a1, const A2&  a2,
                const A3& a3, const A4& a4, const A5& a5, const A6&  a6,
                const A7& a7, const A8& a8, const A9& a9, const A10& a10)
{
    bcema_OldSharedPtr<TYPE> x;
    x.createInplace(allocator, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10);
    return x;
}

template <class TYPE, class A1, class A2, class A3, class A4,
          class A5, class A6, class A7, class A8, class A9, class A10,
          class A11>
bcema_OldSharedPtr<TYPE>
allocate_shared(bslma_Allocator *allocator, const A1& a1, const A2&  a2,
                const A3& a3, const A4& a4, const A5& a5, const A6&  a6,
                const A7& a7, const A8& a8, const A9& a9, const A10& a10,
                const A11& a11)
{
    bcema_OldSharedPtr<TYPE> x;
    x.createInplace(allocator, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11);
    return x;
}

template <class TYPE, class A1, class A2, class A3, class A4,
          class A5, class A6, class A7, class A8, class A9, class A10,
          class A11, class A12>
bcema_OldSharedPtr<TYPE>
allocate_shared(bslma_Allocator *allocator, const A1& a1, const A2&  a2,
                const A3& a3, const A4& a4, const A5& a5, const A6&  a6,
                const A7& a7, const A8& a8, const A9& a9, const A10& a10,
                const A11& a11, const A12& a12)
{
    bcema_OldSharedPtr<TYPE> x;
    x.createInplace(allocator, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11,
                               a12);
    return x;
}

template <class TYPE, class A1, class A2, class A3, class A4,
          class A5, class A6, class A7, class A8, class A9, class A10,
          class A11, class A12, class A13>
bcema_OldSharedPtr<TYPE>
allocate_shared(bslma_Allocator *allocator,     const A1&  a1,  const A2&  a2,
                const A3&  a3,  const A4&  a4,  const A5&  a5,  const A6&  a6,
                const A7&  a7,  const A8&  a8,  const A9&  a9,  const A10& a10,
                const A11& a11, const A12& a12, const A13& a13)
{
    bcema_OldSharedPtr<TYPE> x;
    x.createInplace(allocator, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11,
                               a12, a13);
    return x;
}

template <class TYPE, class A1, class A2, class A3, class A4,
          class A5, class A6, class A7, class A8, class A9, class A10,
          class A11, class A12, class A13, class A14>
bcema_OldSharedPtr<TYPE>
allocate_shared(bslma_Allocator *allocator,     const A1&  a1,  const A2&  a2,
                const A3&  a3,  const A4&  a4,  const A5&  a5,  const A6&  a6,
                const A7&  a7,  const A8&  a8,  const A9&  a9,  const A10& a10,
                const A11& a11, const A12& a12, const A13& a13,
                const A14& a14)
{
    bcema_OldSharedPtr<TYPE> x;
    x.createInplace(allocator, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11,
                               a12, a13, a14);
    return x;
}

                        // -----------------------------
                        // struct bcema_OldSharedPtrUtil
                        // -----------------------------

// CLASS METHODS
template <class TARGET, class SOURCE>
bcema_OldSharedPtr<TARGET>
bcema_OldSharedPtrUtil::dynamicCast(bcema_OldSharedPtr<SOURCE> const& source)
{
    return bcema_OldSharedPtr<TARGET>(source,
                                      dynamic_cast<TARGET*>(source.ptr()));
}

template <class TARGET, class SOURCE>
bcema_OldSharedPtr<TARGET>
bcema_OldSharedPtrUtil::staticCast(bcema_OldSharedPtr<SOURCE> const& source)
{
    return bcema_OldSharedPtr<TARGET>(source,
                                      static_cast<TARGET*>(source.ptr()));
}

template <class TARGET, class SOURCE>
bcema_OldSharedPtr<TARGET>
bcema_OldSharedPtrUtil::constCast(bcema_OldSharedPtr<SOURCE> const& source)
{
    return bcema_OldSharedPtr<TARGET>(source,
                                      const_cast<TARGET*>(source.ptr()));
}

template <class TARGET, class SOURCE>
void bcema_OldSharedPtrUtil::dynamicCast(
                                     bcema_OldSharedPtr<TARGET>        *target,
                                     bcema_OldSharedPtr<SOURCE> const&  source)
{
    target->loadAlias(source, dynamic_cast<TARGET*>(source.ptr()));
}

template <class TARGET, class SOURCE>
void bcema_OldSharedPtrUtil::staticCast(
                                     bcema_OldSharedPtr<TARGET>        *target,
                                     bcema_OldSharedPtr<SOURCE> const&  source)
{
    target->loadAlias(source, static_cast<TARGET*>(source.ptr()));
}

template <class TARGET, class SOURCE>
void bcema_OldSharedPtrUtil::constCast(
                                    bcema_OldSharedPtr<TARGET>        *target,
                                    bcema_OldSharedPtr<SOURCE> const&  source)
{
    target->loadAlias(source, const_cast<TARGET*>(source.ptr()));
}

                        // --------------------------------------
                        // class bcema_OldSharedPtrFactoryAdapter
                        // --------------------------------------

// CREATORS
template <class FACTORY>
inline
bcema_OldSharedPtrFactoryAdapter<FACTORY>::bcema_OldSharedPtrFactoryAdapter(
                     const bcema_OldSharedPtrFactoryAdapter<FACTORY>& original)
: d_factory_p(original.d_factory_p)
{
}

template <class FACTORY>
inline
bcema_OldSharedPtrFactoryAdapter<FACTORY>::bcema_OldSharedPtrFactoryAdapter(
                                                              FACTORY *factory)
: d_factory_p(factory)
{
}

// ACCESSORS
template <class FACTORY>
template <class TYPE>
void
bcema_OldSharedPtrFactoryAdapter<FACTORY>::operator()(TYPE *instance) const
{
    d_factory_p->deleteObject(instance);
}

                        // ------------------------------
                        // class bcema_OldSharedPtr_RepImpl
                        // ------------------------------

// PRIVATE MANIPULATORS
template <class TYPE,class DELETER_FUNCTOR>
template <class FACTORY>
void bcema_OldSharedPtr_RepImpl<TYPE,DELETER_FUNCTOR>::doDeleteObject(
                                                           TYPE       *ptr,
                                                           FACTORY    *factory,
                                                           PointerTag)
{
    factory->deleteObject(ptr);
}

template <class TYPE,class DELETER_FUNCTOR>
template < class RET, class A1>
void bcema_OldSharedPtr_RepImpl<TYPE,DELETER_FUNCTOR>::doDeleteObject(
                                                TYPE         *ptr,
                                                RET         (*deleterFunc)(A1),
                                                PointerTag)
{
    deleterFunc(ptr);
}

template <class TYPE,class DELETER_FUNCTOR>
template <class DELETER>
void bcema_OldSharedPtr_RepImpl<TYPE,DELETER_FUNCTOR>::doDeleteObject(
                                                        TYPE          *ptr,
                                                        DELETER&       deleter,
                                                        NonPointerTag)
{
    deleter(ptr);
}

template <class TYPE,class DELETER_FUNCTOR>
void bcema_OldSharedPtr_RepImpl<TYPE,DELETER_FUNCTOR>::initDefaultAllocator(
                                                             bslma_Allocator *,
                                                             PointerTag)
{
    if (!d_deleter) {
        d_deleter = bslma_Default::allocator(d_deleter);
    }
}

template <class TYPE,class DELETER_FUNCTOR>
template <class DELETER>
void bcema_OldSharedPtr_RepImpl<TYPE,DELETER_FUNCTOR>::initDefaultAllocator(
                                                                    DELETER *,
                                                                    PointerTag)
{
}

template <class TYPE,class DELETER_FUNCTOR>
template <class DELETER>
void bcema_OldSharedPtr_RepImpl<TYPE,DELETER_FUNCTOR>::initDefaultAllocator(
                                                               DELETER& ,
                                                               NonPointerTag)
{
}

// CREATORS
template <class TYPE,class DELETER_FUNCTOR>
bcema_OldSharedPtr_RepImpl<TYPE,DELETER_FUNCTOR>::bcema_OldSharedPtr_RepImpl(
                                                TYPE *ptr,
                                                const DELETER_FUNCTOR& deleter,
                                                bslma_Allocator *allocator)
: bcema_OldSharedPtrRep(allocator)
, d_deleter(deleter)
, d_ptr_p(ptr)
{
    initDefaultAllocator(deleter, TagType());
}

template <class TYPE, class DELETER_FUNCTOR>
bcema_OldSharedPtr_RepImpl<TYPE,DELETER_FUNCTOR>::~bcema_OldSharedPtr_RepImpl()
{
    this->doDeleteObject(d_ptr_p, d_deleter, TagType());
}

// MANIPULATORS
template <class TYPE, class DELETER_FUNCTOR>
void bcema_OldSharedPtr_RepImpl<TYPE,DELETER_FUNCTOR>::deleteObject(TYPE *)
{
    if (!this->decrementRefs()) {
        this->allocator()->deleteObject(this);
    }
}

template <class TYPE, class DELETER_FUNCTOR>
DELETER_FUNCTOR& bcema_OldSharedPtr_RepImpl<TYPE,DELETER_FUNCTOR>::deleter()
{
    return d_deleter;
}

// ACCESSORS
template <class TYPE, class DELETER_FUNCTOR>
inline
bool
bcema_OldSharedPtr_RepImpl<TYPE,
                           DELETER_FUNCTOR>::hasFunctionLikeDeleter() const
{
    return !bslmf_IsPointer<DELETER_FUNCTOR>::VALUE;
}

template <class TYPE, class DELETER_FUNCTOR>
inline
void *bcema_OldSharedPtr_RepImpl<TYPE, DELETER_FUNCTOR>::getDeleter(
                                         bsl::type_info const & typeInfo) const
{
    return (void *)((typeid(DELETER_FUNCTOR) == typeInfo) ? &d_deleter : 0);
}

template <class TYPE, class DELETER_FUNCTOR>
inline
void *bcema_OldSharedPtr_RepImpl<TYPE, DELETER_FUNCTOR>::originalPtr() const
{
    return const_cast<void*>(static_cast<const void*>(d_ptr_p));
}

template <class TYPE, class DELETER_FUNCTOR>
inline
TYPE *bcema_OldSharedPtr_RepImpl<TYPE,DELETER_FUNCTOR>::ptr() const
{
    return this->d_ptr_p;
}

                        // -------------------------------------
                        // class bcema_OldSharedPtr_InplaceRepImpl
                        // -------------------------------------

// CREATORS
template <class TYPE>
inline
bcema_OldSharedPtr_InplaceRepImpl<TYPE>::bcema_OldSharedPtr_InplaceRepImpl(
        bslma_Allocator *allocator)
: bcema_OldSharedPtrRep(allocator)
{
}

template <class TYPE>
template <class A1>
bcema_OldSharedPtr_InplaceRepImpl<TYPE>::bcema_OldSharedPtr_InplaceRepImpl(
        bslma_Allocator *allocator, const A1& a1)
: bcema_OldSharedPtrRep(allocator)
, d_instance(a1)
{
}

template <class TYPE>
template <class A1, class A2>
bcema_OldSharedPtr_InplaceRepImpl<TYPE>::bcema_OldSharedPtr_InplaceRepImpl(
        bslma_Allocator *allocator, const A1& a1, const A2& a2)
: bcema_OldSharedPtrRep(allocator)
, d_instance(a1, a2)
{
}

template <class TYPE>
template <class A1, class A2, class A3>
bcema_OldSharedPtr_InplaceRepImpl<TYPE>::bcema_OldSharedPtr_InplaceRepImpl(
        bslma_Allocator *allocator, const A1& a1, const A2& a2, const A3& a3)
: bcema_OldSharedPtrRep(allocator)
, d_instance(a1, a2, a3)
{
}

template <class TYPE>
template <class A1, class A2, class A3, class A4>
bcema_OldSharedPtr_InplaceRepImpl<TYPE>::bcema_OldSharedPtr_InplaceRepImpl(
        bslma_Allocator *allocator,
        const A1& a1, const A2& a2, const A3& a3, const A4& a4)
: bcema_OldSharedPtrRep(allocator)
, d_instance(a1, a2, a3, a4)
{
}

template <class TYPE>
template <class A1, class A2, class A3, class A4, class A5>
bcema_OldSharedPtr_InplaceRepImpl<TYPE>::bcema_OldSharedPtr_InplaceRepImpl(
        bslma_Allocator *allocator,
        const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5)
: bcema_OldSharedPtrRep(allocator)
, d_instance(a1, a2, a3, a4, a5)
{
}

template <class TYPE>
template <class A1, class A2, class A3, class A4, class A5, class A6>
bcema_OldSharedPtr_InplaceRepImpl<TYPE>::bcema_OldSharedPtr_InplaceRepImpl(
        bslma_Allocator *allocator,
        const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5,
        const A6& a6)
: bcema_OldSharedPtrRep(allocator)
, d_instance(a1, a2, a3, a4, a5, a6)
{
}

template <class TYPE>
template <class A1, class A2, class A3, class A4, class A5, class A6,
          class A7>
bcema_OldSharedPtr_InplaceRepImpl<TYPE>::bcema_OldSharedPtr_InplaceRepImpl(
        bslma_Allocator *allocator,
        const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5,
        const A6& a6, const A7& a7)
: bcema_OldSharedPtrRep(allocator)
, d_instance(a1, a2, a3, a4, a5, a6, a7)
{
}

template <class TYPE>
template <class A1, class A2, class A3, class A4, class A5, class A6,
          class A7, class A8>
bcema_OldSharedPtr_InplaceRepImpl<TYPE>::bcema_OldSharedPtr_InplaceRepImpl(
        bslma_Allocator *allocator,
        const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5,
        const A6& a6, const A7& a7, const A8& a8)
: bcema_OldSharedPtrRep(allocator)
, d_instance(a1, a2, a3, a4, a5, a6, a7, a8)
{
}

template <class TYPE>
template <class A1, class A2, class A3, class A4, class A5, class A6,
          class A7, class A8, class A9>
bcema_OldSharedPtr_InplaceRepImpl<TYPE>::bcema_OldSharedPtr_InplaceRepImpl(
        bslma_Allocator *allocator,
        const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5,
        const A6& a6, const A7& a7, const A8& a8, const A9& a9)
: bcema_OldSharedPtrRep(allocator)
, d_instance(a1, a2, a3, a4, a5, a6, a7, a8, a9)
{
}

template <class TYPE>
template <class A1, class A2, class A3, class A4, class A5, class A6,
          class A7, class A8, class A9, class A10>
bcema_OldSharedPtr_InplaceRepImpl<TYPE>::bcema_OldSharedPtr_InplaceRepImpl(
        bslma_Allocator *allocator,
        const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5,
        const A6& a6, const A7& a7, const A8& a8, const A9& a9, const A10& a10)
: bcema_OldSharedPtrRep(allocator)
, d_instance(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10)
{
}

template <class TYPE>
template <class A1, class A2, class A3, class A4, class A5, class A6,
          class A7, class A8, class A9, class A10, class A11>
bcema_OldSharedPtr_InplaceRepImpl<TYPE>::bcema_OldSharedPtr_InplaceRepImpl(
        bslma_Allocator *allocator,
        const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5,
        const A6& a6, const A7& a7, const A8& a8, const A9& a9, const A10& a10,
        const A11& a11)
: bcema_OldSharedPtrRep(allocator)
, d_instance(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11)
{
}

template <class TYPE>
template <class A1, class A2, class A3, class A4, class A5, class A6,
          class A7, class A8, class A9, class A10, class A11, class A12>
bcema_OldSharedPtr_InplaceRepImpl<TYPE>::bcema_OldSharedPtr_InplaceRepImpl(
        bslma_Allocator *allocator,
        const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5,
        const A6& a6, const A7& a7, const A8& a8, const A9& a9, const A10& a10,
        const A11& a11, const A12& a12)
: bcema_OldSharedPtrRep(allocator)
, d_instance(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12)
{
}

template <class TYPE>
template <class A1, class A2, class A3, class A4, class A5, class A6,
          class A7, class A8, class A9, class A10, class A11, class A12,
          class A13>
bcema_OldSharedPtr_InplaceRepImpl<TYPE>::bcema_OldSharedPtr_InplaceRepImpl(
        bslma_Allocator *allocator,
        const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5,
        const A6& a6, const A7& a7, const A8& a8, const A9& a9, const A10& a10,
        const A11& a11, const A12& a12, const A13& a13)
: bcema_OldSharedPtrRep(allocator)
, d_instance(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13)
{
}

template <class TYPE>
template <class A1, class A2, class A3, class A4, class A5, class A6,
          class A7, class A8, class A9, class A10, class A11, class A12,
          class A13, class A14>
bcema_OldSharedPtr_InplaceRepImpl<TYPE>::bcema_OldSharedPtr_InplaceRepImpl(
        bslma_Allocator *allocator,
        const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5,
        const A6& a6, const A7& a7, const A8& a8, const A9& a9, const A10& a10,
        const A11& a11, const A12& a12, const A13& a13, const A14& a14)
: bcema_OldSharedPtrRep(allocator)
, d_instance(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14)
{
}

template <class TYPE>
inline
bcema_OldSharedPtr_InplaceRepImpl<TYPE>::~bcema_OldSharedPtr_InplaceRepImpl()
{
}

// MANIPULATORS
template <class TYPE>
void bcema_OldSharedPtr_InplaceRepImpl<TYPE>::deleteObject(TYPE *)
{
    if (!this->decrementRefs()) {
        this->allocator()->deleteObject(this);
    }
}

// ACCESSORS
template <class TYPE>
inline
bool bcema_OldSharedPtr_InplaceRepImpl<TYPE>::hasFunctionLikeDeleter() const
{
    return false;
}

template <class TYPE>
inline
void *bcema_OldSharedPtr_InplaceRepImpl<TYPE>::getDeleter(
                                         bsl::type_info const & typeInfo) const
{
    // TBD  d_allocator_p should be moved into the derived classes anyway.
    return (typeid(bslma_Allocator*) == typeInfo) ? (void *)&d_allocator_p : 0;
}

template <class TYPE>
inline
void *bcema_OldSharedPtr_InplaceRepImpl<TYPE>::originalPtr() const
{
    return const_cast<void*>(static_cast<const void*>(&d_instance));
}

template <class TYPE>
inline
TYPE *bcema_OldSharedPtr_InplaceRepImpl<TYPE>::ptr()
{
    return &(this->d_instance);
}

                        // ----------------------------------
                        // class bcema_OldSharedPtr_InitGuard
                        // ----------------------------------

// PRIVATE MANIPULATORS
template <class TYPE, class DELETER>
template <class FACTORY>
void bcema_OldSharedPtr_InitGuard<TYPE,DELETER>::doDeleteObject(
                                                          TYPE        *ptr,
                                                          FACTORY     *factory,
                                                          PointerTag)
{
    factory->deleteObject(ptr);
}

template <class TYPE, class DELETER>
template < class RET, class A1>
void bcema_OldSharedPtr_InitGuard<TYPE,DELETER>::doDeleteObject(
                                                TYPE         *ptr,
                                                RET         (*deleterFunc)(A1),
                                                PointerTag)
{
    deleterFunc(ptr);
}

template <class TYPE, class DELETER>
template <class DELETER_TYPE>
void bcema_OldSharedPtr_InitGuard<TYPE,DELETER>::doDeleteObject(
                                                  TYPE                *ptr,
                                                  const DELETER_TYPE&  deleter,
                                                  NonPointerTag)
{
    DELETER_TYPE tempDeleter(deleter);
    tempDeleter(ptr);
}

//CREATORS
template <class TYPE, class DELETER>
inline
bcema_OldSharedPtr_InitGuard<TYPE,DELETER>::bcema_OldSharedPtr_InitGuard(
                                                       TYPE           *ptr,
                                                       const DELETER&  deleter)
: d_ptr_p(ptr)
, d_deleter(deleter)
{
}

template <class TYPE, class DELETER>
inline
bcema_OldSharedPtr_InitGuard<TYPE,DELETER>::~bcema_OldSharedPtr_InitGuard()
{
    if(this->d_ptr_p) {
        typedef typename bslmf_If<bslmf_IsPointer<DELETER>::VALUE, PointerTag,
                                  NonPointerTag>::Type TagType;

        this->doDeleteObject(this->d_ptr_p, this->d_deleter, TagType());
    }
}

// MANIPULATORS
template <class TYPE, class DELETER>
inline
void bcema_OldSharedPtr_InitGuard<TYPE,DELETER>::release()
{
    this->d_ptr_p = 0;
}

template <int BUFFER_ALIGNMENT>
inline
bsl::pair<char *, bcema_OldSharedPtrRep *>
createInplaceBuffer(bsl::size_t bufferSize, bslma_Allocator *allocator)
    // Allocate a buffer of the specified 'bufferSize' inside a
    // 'bcema_OldSharedPtr_InplaceRepImpl' instance, such that both the 'Rep'
    // and the buffer have the required alignment (actual alignment for 'Rep'
    // and natural alignment required by 'bufferSize' for the buffer) and
    // return a 'bcema_OldSharedPtr' using this rep and pointing to the
    // allocated buffer.
{
    typedef typename bsls_AlignmentToType<BUFFER_ALIGNMENT>::Type
                                                             AlignedBufferType;
    typedef bcema_OldSharedPtr_InplaceRepImpl<AlignedBufferType> Rep;

    // Compute actual alignment required for our inplace representation, all at
    // compile time.
    enum {
        ALIGNMENT = bsls_AlignmentFromType<Rep>::VALUE,
            // alignment of 'Rep' object

        BUFFER_OFFSET_MINUS_ONE = ((sizeof(Rep) - 1) & ~(ALIGNMENT-1)) - 1
            // offset of buffer within 'Rep' object
    };

    // Compute required size, rounded up to the next multiple of 'ALIGNMENT'.
    // Note that 'ALIGNMENT' is a power of two, hence rounding up can be made
    // more efficient than the usual:
    //..
    //  '((x + ALIGNMENT - 1) / ALIGNMENT) * ALIGNMENT
    //..
    bsl::size_t repSize = ((BUFFER_OFFSET_MINUS_ONE + bufferSize) &
                           ~(ALIGNMENT - 1)) + ALIGNMENT;

    // Create inplace instance, using (at least) natural alignment.  Note that
    // many allocators use maximal alignment no matter what, but we have no way
    // to tell (TBD).
    Rep *rep = new(allocator->allocate(repSize)) Rep(allocator);

    return bsl::make_pair((char *)rep->ptr(), rep);
}

void bcema_OldSharedPtrRep::managedPtrDeleter(void *,
                                              bcema_OldSharedPtrRep *rep)
{
    if (!rep->decrementRefs()) {
        rep->allocator()->deleteObject(rep);
    }
}

bcema_OldSharedPtrRep::~bcema_OldSharedPtrRep()
{
}

                         // ----------------------------
                         // class bcema_OldSharedPtrUtil
                         // ----------------------------

bcema_OldSharedPtr<char>
bcema_OldSharedPtrUtil::
    createInplaceUninitializedBuffer(bsl::size_t      bufferSize,
                                     bslma_Allocator *allocator) {
    // We have alignment problems here:  the buffer address (i.e., the address
    // of 'd_instance' in the 'bcema_OldSharedPtr_InplaceRepImpl' object) must
    // be *naturally* *aligned* to 'bufferSize'.  (See 'bdema' package
    // documentation for a definition of natural alignment.)  This is achieved
    // by choosing the 'TYPE' template parameter to have the same alignment as
    // the natural alignment of 'bufferSize'.

    typedef bcema_OldSharedPtr_InplaceRepImpl<char> Rep;

    // Unfortunately, to call the 'bcema_OldSharedPtr' ctor directly, we need
    // to be in this function; the next value, optimized away, is used to pass
    // in both pointers (to buffer, and to 'bcema_OldSharedPtrRep') back from
    // the local function 'createInplaceBuffer'.

    bsl::pair<char *, bcema_OldSharedPtrRep *> ptrRep;

    // Make sure we don't need a default case in the 'switch' statement below.
    // Also make sure our template parameter below does not exceed
    // MAX_ALIGNMENT; typically, 1, 2 and 4 are not the problem, but 8
    // sometimes poses a problem if 'bsls_AlignmentUtil::BSLS_MAX_ALIGNMENT'
    // is 4: passing the raw value 8 as template parameter can trigger
    // compilation mistakes as in that case 'bsls_AlignmentToType<8>' does not
    // have a 'Type' member.
    BSLMF_ASSERT(bsls_AlignmentUtil::BSLS_MAX_ALIGNMENT <= 16);
    enum {
        ONE_ALIGN = 1,
        TWO_ALIGN = (bsls_AlignmentUtil::BSLS_MAX_ALIGNMENT > 2)
                                      ? 2
                                      : bsls_AlignmentUtil::BSLS_MAX_ALIGNMENT,
        FOUR_ALIGN = (bsls_AlignmentUtil::BSLS_MAX_ALIGNMENT > 4)
                                      ? 4
                                      : bsls_AlignmentUtil::BSLS_MAX_ALIGNMENT,
        EIGHT_ALIGN = (bsls_AlignmentUtil::BSLS_MAX_ALIGNMENT > 8)
                                       ? 8
                                       : bsls_AlignmentUtil::BSLS_MAX_ALIGNMENT
    };

    allocator = bslma_Default::allocator(allocator);
    switch (bufferSize % bsls_AlignmentUtil::BSLS_MAX_ALIGNMENT) {
      case 0:  {
        ptrRep =
            createInplaceBuffer<bsls_AlignmentUtil::BSLS_MAX_ALIGNMENT>(
                                                                    bufferSize,
                                                                    allocator);
        return bcema_OldSharedPtr<char>(ptrRep.first, ptrRep.second);
      }

      case 1:
      case 3:
      case 5:
      case 7:
      case 9:
      case 11:
      case 13:
      case 15: {
        ptrRep = createInplaceBuffer<ONE_ALIGN>(bufferSize, allocator);
        return bcema_OldSharedPtr<char>(ptrRep.first, ptrRep.second);
      }

      case 2:
      case 6:
      case 10:
      case 14: {
        ptrRep = createInplaceBuffer<TWO_ALIGN>(bufferSize, allocator);
        return bcema_OldSharedPtr<char>(ptrRep.first, ptrRep.second);
      }

      case 4:
      case 12: {
        ptrRep = createInplaceBuffer<FOUR_ALIGN>(bufferSize, allocator);
        return bcema_OldSharedPtr<char>(ptrRep.first, ptrRep.second);
      }

      case 8: {
        ptrRep = createInplaceBuffer<EIGHT_ALIGN>(bufferSize, allocator);
        return bcema_OldSharedPtr<char>(ptrRep.first, ptrRep.second);
      }
    }

    BSLS_ASSERT(0 && "Unreachable by design");
    return bcema_OldSharedPtr<char>();  // quell g++ warning
}

typedef bcema_OldSharedPtr<MyTestObject> OldObj;

template <class POINTER>
struct PerformanceTester
{
   static void test(bool verbose, bool allocVerbose) {
        bcema_TestAllocator ta(allocVerbose);

        enum {
            NUM_ITER        = 1000,
            VECTOR_SIZE     = 1000,
            BIG_VECTOR_SIZE = NUM_ITER * VECTOR_SIZE
        };

        int deleteCounter, copyCounter, numAlloc, numBytes;

        bsl::vector<TObj *> mZ(&ta);
        const bsl::vector<TObj *>& Z = mZ;

        bsls_Stopwatch timer;

        mZ.resize(BIG_VECTOR_SIZE);
        deleteCounter = copyCounter = 0;
        numAlloc = ta.numAllocations();
        numBytes = ta.numBytesInUse();
        timer.start();
        for (int i = 0; i < BIG_VECTOR_SIZE; ++i) {
            mZ[i] = new(ta) TObj(&deleteCounter, &copyCounter);
        }
        timer.stop();
        cout << "Creating " << BIG_VECTOR_SIZE << " owned instances in "
             << timer.elapsedTime() << "s ("
             << timer.elapsedTime() / BIG_VECTOR_SIZE << "s each)" << endl;
        if (verbose) {
            cout << "\t" << ta.numAllocations() - numAlloc << " allocations, "
                 << ta.numBytesInUse() - numBytes << " bytes\n"
                 << "\t" << copyCounter << " copies of test objects\n"
                 << "\t" << deleteCounter << " deletions of test objects"
                 << endl;
        }

        for (int i = 1; i < BIG_VECTOR_SIZE; ++i) {
            mZ[i]->~TObj();
        }

        deleteCounter = copyCounter = 0;
        numAlloc = ta.numAllocations();
        numBytes = ta.numBytesInUse();
        timer.start();
        for (int i = 1; i < BIG_VECTOR_SIZE; ++i) {
            new(mZ[i]) TObj(*Z[0]);
        }
        timer.stop();
        cout << "Copy-constructing " << BIG_VECTOR_SIZE - 1
             << " owned instances in " << timer.elapsedTime() << "s ("
             << timer.elapsedTime() / (BIG_VECTOR_SIZE - 1) << "s each)"
             << endl;
        if (verbose) {
            cout << "\t" << ta.numAllocations() - numAlloc << " allocations, "
                 << ta.numBytesInUse() - numBytes << " bytes\n"
                 << "\t" << copyCounter << " copies of test objects\n"
                 << "\t" << deleteCounter << " deletions of test objects"
                 << endl;
        }

        deleteCounter = copyCounter = 0;
        numAlloc = ta.numAllocations();
        numBytes = ta.numBytesInUse();
        timer.start();
        for (int i = 0; i < BIG_VECTOR_SIZE; ++i) {
            ((bslma_Allocator*)&ta)->deleteObject(mZ[i]);
        }
        timer.stop();
        cout << "Destroying " << BIG_VECTOR_SIZE << " owned instances in "
             << timer.elapsedTime() << "s ("
             << timer.elapsedTime() / BIG_VECTOR_SIZE << "s each)" << endl;
        if (verbose) {
            cout << "\t" << ta.numAllocations() - numAlloc << " allocations, "
                 << ta.numBytesInUse() - numBytes << " bytes\n"
                 << "\t" << copyCounter << " copies of test objects\n"
                 << "\t" << deleteCounter << " deletions of test objects"
                 << endl;
        }

        deleteCounter = copyCounter = 0;
        numAlloc = ta.numAllocations();
        numBytes = ta.numBytesInUse();
        for (int i = 0; i < BIG_VECTOR_SIZE; ++i) {
            mZ[i] = new(ta) TObj(&deleteCounter, &copyCounter);
        }
        cout << "Re-hydrated " << BIG_VECTOR_SIZE
             << " owned instances" << endl;
        if (verbose) {
            cout << "\t" << ta.numAllocations() - numAlloc << " allocations, "
                 << ta.numBytesInUse() - numBytes << " bytes\n"
                 << "\t" << copyCounter << " copies of test objects\n"
                 << "\t" << deleteCounter << " deletions of test objects"
                 << endl;
        }

        bsl::vector<POINTER> mX(&ta);
        const bsl::vector<POINTER>& X = mX;

        // -------------------------------------------------------------------
        cout << "\nCreating out-of-place representations."
             << "\n--------------------------------------" << endl;

        mX.resize(BIG_VECTOR_SIZE);
        for (int i = 0; i < BIG_VECTOR_SIZE; ++i) {
            // We first destroy the contents of mX in order to be able to
            // recreate them in place.  Using push_back instead would involve
            // an additional creation (for a temporary) and copy construction
            // into the vector, which is not what we intend to measure.

            (&mX[i])->~POINTER();
        }
        timer.reset();
        deleteCounter = copyCounter = 0;
        numAlloc = ta.numAllocations();
        numBytes = ta.numBytesInUse();
        timer.start();
        for (int i = 0; i < BIG_VECTOR_SIZE; ++i) {
            // If this code throws an exception, then the remaining elements
            // will be destroyed twice, once above and another time with the
            // destruction of mX.  But that is OK since they are empty.

            new(&mX[i]) POINTER(Z[i], &ta);
        }
        timer.stop();
        cout << "Creating " << BIG_VECTOR_SIZE
             << " distinct shared pointers in "
             << timer.elapsedTime() << "s ("
             << timer.elapsedTime() / BIG_VECTOR_SIZE << "s each)" << endl;
        if (verbose) {
            cout << "\t" << ta.numAllocations() - numAlloc << " allocations, "
                 << ta.numBytesInUse() - numBytes << " bytes\n"
                 << "\t" << copyCounter << " copies of test objects\n"
                 << "\t" << deleteCounter << " deletions of test objects"
                 << endl;
        }

        timer.reset();
        deleteCounter = copyCounter = 0;
        numAlloc = ta.numAllocations();
        numBytes = ta.numBytesInUse();
        timer.start();
        for (int i = 0; i < BIG_VECTOR_SIZE; ++i) {
            (&mX[i])->~POINTER();
        }
        timer.stop();
        cout << "Destroying " << BIG_VECTOR_SIZE
             << " distinct shared pointers in "
             << timer.elapsedTime() << "s ("
             << timer.elapsedTime() / BIG_VECTOR_SIZE << "s each)" << endl;
        if (verbose) {
            cout << "\t" << ta.numAllocations() - numAlloc << " allocations, "
                 << ta.numBytesInUse() - numBytes << " bytes\n"
                 << "\t" << copyCounter << " copies of test objects\n"
                 << "\t" << deleteCounter << " deletions of test objects"
                 << endl;
        }

        // Note:  Z now contains dangling pointers.  Rehydrate!
        deleteCounter = copyCounter = 0;
        numAlloc = ta.numAllocations();
        numBytes = ta.numBytesInUse();
        for (int i = 0; i < BIG_VECTOR_SIZE; ++i) {
            mZ[i] = new(ta) TObj(&deleteCounter, &copyCounter);
        }
        cout << "Re-hydrated " << BIG_VECTOR_SIZE
             << " owned instances" << endl;
        if (verbose) {
            cout << "\t" << ta.numAllocations() - numAlloc << " allocations, "
                 << ta.numBytesInUse() - numBytes << " bytes\n"
                 << "\t" << copyCounter << " copies of test objects\n"
                 << "\t" << deleteCounter << " deletions of test objects"
                 << endl;
        }

        {
            POINTER Y(Z[0], &ta);
            timer.reset();
            deleteCounter = copyCounter = 0;
            numAlloc = ta.numAllocations();
            numBytes = ta.numBytesInUse();
            timer.start();
            for (int i = 0; i < BIG_VECTOR_SIZE; ++i) {
                new(&mX[i]) POINTER(Y);
            }
            timer.stop();
            cout << "Creating " << BIG_VECTOR_SIZE
                 << " copies of the same shared pointer in "
                 << timer.elapsedTime() << "s ("
                 << timer.elapsedTime() / BIG_VECTOR_SIZE << "s each)" << endl;
            if (verbose) {
                cout << "\t" << ta.numAllocations() - numAlloc
                     << " allocations, "
                     << ta.numBytesInUse() - numBytes << " bytes\n"
                     << "\t" << copyCounter << " copies of test objects\n"
                     << "\t" << deleteCounter << " deletions of test objects"
                     << endl;
            }
        }

        timer.reset();
        deleteCounter = copyCounter = 0;
        numAlloc = ta.numAllocations();
        numBytes = ta.numBytesInUse();
        timer.start();
        for (int i = 0; i < BIG_VECTOR_SIZE; ++i) {
            (&mX[i])->~POINTER();
        }
        timer.stop();
        cout << "Destroying " << BIG_VECTOR_SIZE
             << " times the same shared pointer in "
             << timer.elapsedTime() << "s ("
             << timer.elapsedTime() / BIG_VECTOR_SIZE << "s each)" << endl;
        if (verbose) {
            cout << "\t" << ta.numAllocations() - numAlloc << " allocations, "
                 << ta.numBytesInUse() - numBytes << " bytes\n"
                 << "\t" << copyCounter << " copies of test objects\n"
                 << "\t" << deleteCounter << " deletions of test objects"
                 << endl;
        }

        // Note:  Z[0] is now dangling, and X contains only empty shared
        // pointers.  Rehydrate, but with empty shared pointers!
        deleteCounter = copyCounter = 0;
        numAlloc = ta.numAllocations();
        numBytes = ta.numBytesInUse();
        mZ[0] = new(ta) TObj(&deleteCounter, &copyCounter);
        for (int i = 0; i < BIG_VECTOR_SIZE; ++i) {
            new(&mX[i]) POINTER();
        }
        cout << "Re-hydrated 1 owned instance and " << BIG_VECTOR_SIZE
             << " empty shared pointers" << endl;
        if (verbose) {
            cout << "\t" << ta.numAllocations() - numAlloc << " allocations, "
                 << ta.numBytesInUse() - numBytes << " bytes\n"
                 << "\t" << copyCounter << " copies of test objects\n"
                 << "\t" << deleteCounter << " deletions of test objects"
                 << endl;
        }

        cout << "\nCreating in-place representations."
             << "\n----------------------------------" << endl;

        timer.reset();
        deleteCounter = copyCounter = 0;
        numAlloc = ta.numAllocations();
        numBytes = ta.numBytesInUse();
        timer.start();
        for (int i = 0; i < BIG_VECTOR_SIZE; ++i) {
            mX[i].createInplace(&ta, *Z[i]);
        }
        timer.stop();
        cout << "Creating " << BIG_VECTOR_SIZE
             << " distinct in-place shared pointers in "
             << timer.elapsedTime() << "s ("
             << timer.elapsedTime() / BIG_VECTOR_SIZE << "s each)" << endl;
        if (verbose) {
            cout << "\t" << ta.numAllocations() - numAlloc << " allocations, "
                 << ta.numBytesInUse() - numBytes << " bytes\n"
                 << "\t" << copyCounter << " copies of test objects\n"
                 << "\t" << deleteCounter << " deletions of test objects"
                 << endl;
        }

        timer.reset();
        deleteCounter = copyCounter = 0;
        numAlloc = ta.numAllocations();
        numBytes = ta.numBytesInUse();
        timer.start();
        for (int i = 0; i < BIG_VECTOR_SIZE; ++i) {
            (&mX[i])->~POINTER();
        }
        timer.stop();
        cout << "Destroying " << BIG_VECTOR_SIZE
             << " distinct in-place shared pointers in "
             << timer.elapsedTime() << "s ("
             << timer.elapsedTime() / BIG_VECTOR_SIZE << "s each)" << endl;
        if (verbose) {
            cout << "\t" << ta.numAllocations() - numAlloc << " allocations, "
                 << ta.numBytesInUse() - numBytes << " bytes\n"
                 << "\t" << copyCounter << " copies of test objects\n"
                 << "\t" << deleteCounter << " deletions of test objects"
                 << endl;
        }

        cout << "\nCreating aliased shared pointers."
             << "\n---------------------------------" << endl;

        {
            POINTER Y(Z[0], &ta);
            timer.reset();
            deleteCounter = copyCounter = 0;
            numAlloc = ta.numAllocations();
            numBytes = ta.numBytesInUse();
            timer.start();
            for (int i = 0; i < BIG_VECTOR_SIZE; ++i) {
                new(&mX[i]) POINTER(Y, Z[i]);
            }
            timer.stop();
            cout << "Creating " << BIG_VECTOR_SIZE
                 << " aliases of the same shared pointer in "
                 << timer.elapsedTime() << "s ("
                 << timer.elapsedTime() / BIG_VECTOR_SIZE << "s each)" << endl;
            if (verbose) {
                cout << "\t" << ta.numAllocations() - numAlloc
                     << " allocations, "
                     << ta.numBytesInUse() - numBytes << " bytes\n"
                     << "\t" << copyCounter << " copies of test objects\n"
                     << "\t" << deleteCounter << " deletions of test objects"
                     << endl;
            }
        }

        timer.reset();
        deleteCounter = copyCounter = 0;
        numAlloc = ta.numAllocations();
        numBytes = ta.numBytesInUse();
        timer.start();
        for (int i = 0; i < BIG_VECTOR_SIZE; ++i) {
            (&mX[i])->~POINTER();
        }
        timer.stop();
        cout << "Destroying " << BIG_VECTOR_SIZE
             << " aliases of the same shared pointer in "
             << timer.elapsedTime() << "s ("
             << timer.elapsedTime() / BIG_VECTOR_SIZE << "s each)" << endl;
        if (verbose) {
            cout << "\t" << ta.numAllocations() - numAlloc << " allocations, "
                 << ta.numBytesInUse() - numBytes << " bytes\n"
                 << "\t" << copyCounter << " copies of test objects\n"
                 << "\t" << deleteCounter << " deletions of test objects"
                 << endl;
        }

        // Note:  Z[0] is now dangling, and X contains only empty shared
        // pointers.  Rehydrate!
        deleteCounter = copyCounter = 0;
        numAlloc = ta.numAllocations();
        numBytes = ta.numBytesInUse();
        mZ[0] = new(ta) TObj(&deleteCounter, &copyCounter);
        for (int i = 0; i < BIG_VECTOR_SIZE; ++i) {
            new(&mX[i]) POINTER(Z[i], &ta);
        }
        cout << "Re-hydrated 1 owned instance and " << BIG_VECTOR_SIZE
             << " shared pointers" << endl;
        if (verbose) {
            cout << "\t" << ta.numAllocations() - numAlloc << " allocations, "
                 << ta.numBytesInUse() - numBytes << " bytes\n"
                 << "\t" << copyCounter << " copies of test objects\n"
                 << "\t" << deleteCounter << " deletions of test objects"
                 << endl;
        }

        // -------------------------------------------------------------------
        cout << "\nAssignment."
             << "\n-----------" << endl;

        timer.reset();
        deleteCounter = copyCounter = 0;
        numAlloc = ta.numAllocations();
        numBytes = ta.numBytesInUse();
        timer.start();
        {
            POINTER Y = X[0];
            for (int j = 1; j < BIG_VECTOR_SIZE; ++j) {
                mX[j - 1] = X[j];
            }
            mX.back() = Y;
        }
        timer.stop();
        cout << "Assigning " << BIG_VECTOR_SIZE + 1
             << " distinct shared pointers in "
             << timer.elapsedTime() << "s ("
             << timer.elapsedTime() / (BIG_VECTOR_SIZE + 1) << "s each)"
             << endl;
        if (verbose) {
            cout << "\t" << ta.numAllocations() - numAlloc << " allocations, "
                 << ta.numBytesInUse() - numBytes << " bytes\n"
                 << "\t" << copyCounter << " copies of test objects\n"
                 << "\t" << deleteCounter << " deletions of test objects"
                 << endl;
        }

        timer.reset();
        deleteCounter = copyCounter = 0;
        numAlloc = ta.numAllocations();
        numBytes = ta.numBytesInUse();
        timer.start();
        {
            POINTER Y = X[0];
            for (int j = 1; j < BIG_VECTOR_SIZE; ++j) {
                mX[j] = Y;
            }
        }
        timer.stop();
        cout << "Assigning " << BIG_VECTOR_SIZE
             << " times the same shared pointer in "
             << timer.elapsedTime() << "s ("
             << timer.elapsedTime() / BIG_VECTOR_SIZE << "s each)" << endl;
        if (verbose) {
            cout << "\t" << ta.numAllocations() - numAlloc << " allocations, "
                 << ta.numBytesInUse() - numBytes << " bytes\n"
                 << "\t" << copyCounter << " copies of test objects\n"
                 << "\t" << deleteCounter << " deletions of test objects"
                 << endl;
        }

        // Note:  Z now contains dangling pointers, except Z[0].  Rehydrate!
        // Note:  Z now contains dangling pointers.  Rehydrate!
        deleteCounter = copyCounter = 0;
        numAlloc = ta.numAllocations();
        numBytes = ta.numBytesInUse();
        for (int i = 0; i < BIG_VECTOR_SIZE; ++i) {
            mZ[i] = new(ta) TObj(&deleteCounter, &copyCounter);
        }
        cout << "Re-hydrated " << BIG_VECTOR_SIZE
             << " owned instances" << endl;
        if (verbose) {
            cout << "\t" << ta.numAllocations() - numAlloc << " allocations, "
                 << ta.numBytesInUse() - numBytes << " bytes\n"
                 << "\t" << copyCounter << " copies of test objects\n"
                 << "\t" << deleteCounter << " deletions of test objects"
                 << endl;
        }

        // -------------------------------------------------------------------
        cout << "\nPooling out-of-place representations."
             << "\n-------------------------------------" << endl;

        // TBD

        // -------------------------------------------------------------------
        for (int i = 0; i < BIG_VECTOR_SIZE; ++i) {
            ta.deleteObject(mZ[i]);
        }
   }
};

template <typename T>
class ManagedPtrTestDeleter {

    T* d_providedObj;

public:
    ManagedPtrTestDeleter() : d_providedObj(0) {}

    void deleteObject(T* obj) {
        ASSERT((int)(0 == d_providedObj));
        d_providedObj = obj;
    }

    T* providedObj() {
        return d_providedObj;
    }

    void reset() {
        d_providedObj = 0;
    }
};

class SelfReference
{
    // DATA
    bcema_SharedPtr<SelfReference> d_dataPtr;

  public:
    // MANIPULATORS
    void setData(bcema_SharedPtr<SelfReference>& value) { d_dataPtr = value; }
    void release() { d_dataPtr.reset(); }
};

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
    int numDeletes = 0;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:  // Zero is always the leading case.
    case 21: {
        // --------------------------------------------------------------------
        // TESTING 'reset' using a self-referenced shared ptr (DRQS 26465543)
        //
        // Concerns: 
        //: 1 Resetting the last reference to a self-referenced shared pointer
        //:   calls 'releaseRef' only once.
        //
        // Plan:
        //: 1 Create a self-referring shared pointer.
        //:
        //: 2 Call 'reset' on the referenced object. 
        //:
        //: 3 Verify that an assertion failure does not happen (in any mode).
        //
        // Testing:
        //   void reset();
        // --------------------------------------------------------------------

        SelfReference *ptr;
        {
            bcema_SharedPtr<SelfReference> mX;
            mX.createInplace();
            mX->setData(mX);
            ptr = mX.ptr();
        }

        ptr->release();
      } break;        
      case 20: {
        // --------------------------------------------------------------------
        // TESTING constructing from ManagedPtr
        // 
        // Concerns: 
        //   1) When constructing from a managed-ptr, the original deleter 
        //      specified in the managed-ptr is used to destroy the object
        //
        //   2) When constructing from an aliased managed-ptr, the original
        //      deleter is supplied the correct address
        //
        // Testing:
        //   bcema_SharedPtr(bdema_ManagedPtr<OT> & original);
        // --------------------------------------------------------------------

        ManagedPtrTestDeleter<int> deleter;

        int obj1, obj2;

        bcema_SharedPtr<int> outerSp;
        {
            bdema_ManagedPtr<int> mp1 (&obj1, &deleter);
            
            bcema_SharedPtr<int> sp1 (mp1);
            sp1.clear();
            
            // check non-aliased managed-ptr assignment
            ASSERT(&obj1 == deleter.providedObj());
            
            deleter.reset();
            mp1.load(&obj2, &deleter);
            
            bdema_ManagedPtr<int> mp2 (mp1, &obj1);
            bcema_SharedPtr<int> sp2 (mp2);
            outerSp = sp2;
        }
        outerSp.clear();
        // check aliased managed-ptr assignment
        ASSERT(&obj2 == deleter.providedObj());
      } break;        

      case 19: {
        // --------------------------------------------------------------------
        // TESTING bcema_SharedPtrOutofplaceRep
        //
        // Concerns:
        //   1) 'incrementRefs' and 'decrementRefs' correctly adjust the number
        //      of references by the specified amount.
        //   2) 'incrementRefs' and 'decrementRefs' defaults the number of
        //      increments and decrements to 1.
        //   3) 'decrementRefs' correctly return the updated number of
        //      references.
        //   4) 'numReferences' return the current count of the number of
        //      of references.
        //
        // Plan:
        //   First increment and decrement the number of references by the
        //   default amount, then verify using 'numReferences' that the number
        //   of references are adjusted accordingly.  Then increment and
        //   decrement the reference count by 1 first (non-default), then 2,
        //   then 3, and verify the number of references are adjusted
        //   accordingly between each increment / decrement.
        //
        // Testing:
        //   void incrementRefs(int incrementAmount = 1);
        //   int decrementRefs(int decrementAmount = 1);
        //   int numReferences() const;
        // --------------------------------------------------------------------

        if (verbose) cout << "\n" << endl;

        {
            bslma_Allocator     *da = bslma_Default::allocator();
            bslma_TestAllocator *t  = 0;
            int *x                  = new int(0);
            int count               = 0;

            bcema_SharedPtrOutofplaceRep<int, bslma_TestAllocator*> *implPtr =
                     bcema_SharedPtrOutofplaceRep<int, bslma_TestAllocator*>::
                                                         makeOutofplaceRep(x,
                                                                           t,
                                                                           t);
            bcema_SharedPtrOutofplaceRep<int, bslma_TestAllocator*>&impl
                                                              = *implPtr;
            LOOP_ASSERT(impl.numReferences(), 1 == impl.numReferences());

            impl.acquireRef();
            LOOP_ASSERT(impl.numReferences(), 2 == impl.numReferences());

            impl.acquireRef();
            LOOP_ASSERT(impl.numReferences(), 3 == impl.numReferences());

            impl.releaseRef();
            LOOP_ASSERT(impl.numReferences(), 2 == impl.numReferences());

            impl.releaseRef();
            LOOP_ASSERT(impl.numReferences(), 1 == impl.numReferences());

            impl.incrementRefs(1);
            LOOP_ASSERT(impl.numReferences(), 2 == impl.numReferences());

            impl.incrementRefs(1);
            LOOP_ASSERT(impl.numReferences(), 3 == impl.numReferences());

            impl.releaseRef();
            LOOP_ASSERT(impl.numReferences(), 2 == impl.numReferences());

            impl.releaseRef();
            LOOP_ASSERT(impl.numReferences(), 1 == impl.numReferences());

            impl.incrementRefs(2);
            LOOP_ASSERT(impl.numReferences(), 3 == impl.numReferences());

            impl.incrementRefs(2);
            LOOP_ASSERT(impl.numReferences(), 5 == impl.numReferences());

            impl.releaseRef();
            impl.releaseRef();
            LOOP_ASSERT(impl.numReferences(), 3 == impl.numReferences());

            impl.releaseRef();
            impl.releaseRef();
            LOOP_ASSERT(impl.numReferences(), 1 == impl.numReferences());

            impl.incrementRefs(3);
            LOOP_ASSERT(impl.numReferences(), 4 == impl.numReferences());

            impl.incrementRefs(3);
            LOOP_ASSERT(impl.numReferences(), 7 == impl.numReferences());

            impl.releaseRef();
            impl.releaseRef();
            impl.releaseRef();
            LOOP_ASSERT(impl.numReferences(), 4 == impl.numReferences());

            impl.releaseRef();
            impl.releaseRef();
            impl.releaseRef();
            LOOP_ASSERT(impl.numReferences(), 1 == impl.numReferences());

            impl.releaseRef();
            //da->deallocate(implPtr);
        }
      } break;
      case 18: {
        // --------------------------------------------------------------------
        // TESTING bcema_SharedPtrOutofplaceRep CTORS
        //
        // Plan: TBD
        //
        // Testing:
        //   CONCERN: bcema_SharedPtrOutofplaceRep passes allocator
        //            to the deleter's constructor
        // --------------------------------------------------------------------

        if (verbose) cout << "\nConcern: bcema_SharedPtrOutofplaceRep passes"
                          << "\nallocator to the deleter's constructor."
                          << "\n======================================="
                          << endl;

        bcema_TestAllocator ta1(veryVeryVerbose);
        bcema_TestAllocator ta2(veryVeryVerbose);
        int numDeletes1 = 0;

        {
            bslma_DefaultAllocatorGuard allocGuard(&ta1);

            MyTestObject *p1 = new (ta2) MyTestObject(&numDeletes1);
            MyAllocTestDeleter d(&ta2, &ta2);
            Obj x1(p1,d,&ta2);
            ASSERT(0 == ta1.numBytesInUse());
        }

        ASSERT(1 == numDeletes1);
        ASSERT(0 == ta1.numBytesInUse());
        ASSERT(0 == ta2.numBytesInUse());

      } break;
      case 17: {
        // --------------------------------------------------------------------
        // TESTING bcema_SharedPtrOutofplaceRep::originalPtr
        //
        // Plan:  Create shared pointers with various representations, release
        // them (getting back a pointer to the representation object) and
        // assert that the 'originalPtr' of that representation is identical to
        // the address of the managed object.
        //
        // Testing:
        //   CONCERN: bcema_SharedPtrRep::originalPtr returns correct value
        // --------------------------------------------------------------------

        if (verbose)
            cout << "\nConcern: 'bcema_SharedPtrRep::originalPtr' returns"
                 << "\ncorrect value for 'bcema_SharedPtrOutofplaceRep'"
                 << "\n================================================="
                 << endl;

        {
            bcema_TestAllocator ta(veryVeryVerbose);
            int numDeletes1 = 0;
            MyTestObject *p1 = new (ta) MyTestObject(&numDeletes1);
            Obj x1(p1, &ta);

            bsl::pair<MyTestObject*,bcema_SharedPtrRep*> r = x1.release();

            ASSERT(0 == x1.get());

            ASSERT(r.first == r.second->originalPtr());
            ASSERT(p1 == r.first);

            Obj x2(r.first, r.second);
            x2.clear();

            ASSERT(1 == numDeletes1);
            ASSERT(2 == ta.numAllocations());
            ASSERT(0 == ta.numBytesInUse());
        }

        if (verbose)
            cout << "\nConcern: 'bcema_SharedPtrRep::originalPtr' returns"
                 << "\ncorrect value for 'bcema_SharedPtrInplaceRep'"
                 << "\n===================================================="
                 << endl;

        {
            bcema_TestAllocator ta(veryVeryVerbose);
            int numDeletes1 = 0;
            Obj x1;
            x1.createInplace(&ta, &numDeletes1);
            bsl::pair<MyTestObject*,bcema_SharedPtrRep*> r = x1.release();

            ASSERT(0 == x1.get());

            ASSERT(r.first == r.second->originalPtr());

            Obj x2(r.first, r.second);
            x2.clear();

            ASSERT(1 == numDeletes1);
            ASSERT(1 == ta.numAllocations());
            ASSERT(0 == ta.numBytesInUse());
        }

        if (verbose)
            cout << "\nConcern: 'bcema_SharedPtrRep::originalPtr' returns"
                 << "\ncorrect value when aliased"
                 << "\n================================================="
                 << endl;

        {
            typedef bsl::vector<MyTestObject2> V;
            bcema_TestAllocator ta(veryVeryVerbose);
            int numDeletes1 = 0;
            V *v1 = new (ta) V(&ta);
            bcema_SharedPtr<V> x1(v1,&ta);
            v1->resize(2);

            bcema_SharedPtr<MyTestObject2> a1(x1,&v1->at(1));
            bsl::pair<MyTestObject2*,bcema_SharedPtrRep*> r = a1.release();
            ASSERT(0==a1.get());

            ASSERT(r.first != r.second->originalPtr());
            ASSERT(v1 == r.second->originalPtr());

            r.second->releaseRef();
            x1.clear();

            ASSERT(0 == ta.numBytesInUse());
        }

      } break;
      case 16: {
        // --------------------------------------------------------------------
        // TESTING CONCERN: CONVERSION TO BOOL
        //
        // Concerns: Implicit conversion to 'bool' are troublesome, because a
        //   bool is further convertible to integral types, and thus such
        //   conversions should be banned.  Specifically, we are concerned that
        //     o SharedPtr can be used in "boolean" contexts such as 'if (p)',
        //       'if (!p)', 'if
        //     o SharedPtr cannot be converted to an 'int'.
        //     o SharedPtr cannot be compared via 'operator<'.
        //     o SharedPtr returned by a function (as a temporary) does not
        //       lead to erroneous bool value (DRQS 12252806).
        //
        // Plan: We test the conversion in a variety of "boolean" contexts and
        //   assert that the result is as expected.  In order to test for the
        //   *absence* of conversion, we can use 'bslmf_isconvertible'.  In
        //   order to test for the absence of 'operator<', we use our own
        //   definition of 'operator<', which will be picked up and which will
        //   create an ambiguity if one is already defined.  We verify that our
        //   'operator<' has been picked up by using a helper function, which
        //   has two matchings, one to the return type of our definition of
        //   'operator<', and the other to '...'.
        //
        // Testing:
        //   operator bcema_SharedPtr_UnspecifiedBool()
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Concern: 'bsl::shared_ptr' compliance" << endl
                          << "=====================================" << endl;

        using namespace NAMESPACE_TEST_CASE_16;

        if (verbose) cout << "Not convertible to ints.\n" << endl;

        ASSERT((0 == bslmf_IsConvertible<bcema_SharedPtr<int>, int>::VALUE));

        if (verbose) cout << "Not less than comparable.\n" << endl;

        ASSERT(sizeof(Two) == sizeof(NoOperatorLTMatch(ptr1 < ptr2)));
        ASSERT(sizeof(Two) == sizeof(NoOperatorLTMatch(ptr1 < ptr3)));

        if (verbose) cout << "Simple boolean expressions.\n" << endl;

        ASSERT(!ptrNil);
        ASSERT(ptr1);

        if (verbose) cout << "Comparisons.\n" << endl;

            // COMPARISON SHR PTR TO SHR PTR
        ASSERT(!(ptrNil == ptr1));
        ASSERT(ptrNil != ptr1);

            // COMPARISON SHR PTR TO BOOL
        ASSERT(ptrNil == false);
        ASSERT(ptr1 != false);

            // COMPARISON SHR PTR TO INT
        ASSERT(ptrNil == 0);
        ASSERT(ptr1 != 0);

            // COMPARISON BOOL TO SHR PTR
        ASSERT(false == ptrNil);
        ASSERT(false != ptr1);
        ASSERT(true  && ptr1);

        if (verbose) cout << "Boolean operators.\n" << endl;

        ASSERT(!ptrNil && true);
        ASSERT(!ptrNil || false);
        ASSERT(ptr1 && true);
        ASSERT(ptr1 || false);

        ASSERT(true && !ptrNil);
        ASSERT(false || !ptrNil);
        ASSERT(true && ptr1);
        ASSERT(false || ptr1);

        ASSERT(!ptrNil && 1);
        ASSERT(!ptrNil || 0);
        ASSERT(ptr1 && 1);
        ASSERT(ptr1 || 0);

        ASSERT(1 && !ptrNil);
        ASSERT(0 || !ptrNil);
        ASSERT(1 && ptr1);
        ASSERT(0 || ptr1);

        if (verbose) cout << "With function return values.\n" << endl;
        // DRQS 12252806

        ASSERT(ptr1Fun() && true);
        ASSERT(!ptrNilFun() && true);

        ASSERT(!ptrNilFun() && ptrNilFun() == ptrNil);
        ASSERT(!ptrNilFun() || ptrNilFun() != ptr1);
        ASSERT(ptr1Fun() && ptr1Fun() == ptr1);
        ASSERT(ptr1Fun() || ptr1Fun() != ptrNil);

        ASSERT(!(ptrNilFun() && *ptrNilFun() != *ptr1));

#if 0
        if (verbose) cout << "Should not compile.\n" << endl;

        // COMPARISON SHR PTR TO INT
        ASSERT(ptrNil != 1);
        ASSERT(ptr1 == 1);
        ASSERT(ptrNil != 2);
        ASSERT(ptr1 == 2);

        // COMPARISON BOOL TO SHR PTR
        ASSERT(ptrNil != true);   // compiles on CC :(
        ASSERT(ptr1 == true);     // compiles on CC :(
        ASSERT(true  != ptrNil);  // compiles on CC :(
        ASSERT(true  == ptr1);    // compiles on CC :(

        // COMPARISON INT TO SHR PTR
        ASSERT(0 == ptrNil);
        ASSERT(0 != ptr1);
        ASSERT(1 != ptrNil);
        ASSERT(1 == ptr1);
        ASSERT(2 != ptrNil);
        ASSERT(2 == ptr1);  cout << "r on CC, error on x.\n" << endl;

        // LESS THAN COMPARISONS
        ASSERT(ptrNil < ptr1);
        ASSERT(ptrNil < ptr3);
        ASSERT(ptr1 < ptr2);
        ASSERT(ptr1 < ptr3);

        bsl::map<SharedPtr<int>, int> sharedPtrMap;
        // no instantiation of comparison and conversion to int, no error!
        sharedPtrMap[p] = 1;  // needs comparisons: error on CC, error on xlC_r
        sharedPtrMap[q] = 2;  // needs comparisons: error on CC, error on xlC_r

        if using bool instead of unspecifiedBool, everything OK until here:
            cout << sharedPtrMap[p] << "\n";  // prints 1
        cout << sharedPtrMap[q] << "\n";  // prints 2
        cout << sharedPtrMap[r] << "\n";  // oops: prints 1, should print 0
#endif

      } break;
      case 15: {
        // --------------------------------------------------------------------
        // TESTING CONCERN: C++ 'bsl::shared_ptr' COMPLIANCE
        //
        // Plan:  Check that the 'reset' function works, by simply exercising
        //   them on a non-empty shared pointer, and checking that the result
        //   and reference counts are as expected.  Also check that the 'swap'
        //   global function works, using the same test plan as in case 11.
        //
        //   No longer tested because no longer part of this component, but
        //   part of the C++ compliance test, we would check that the
        //   '..._pointer_cast' global functions work, using the same test plan
        //   as in case 9.  For 'get_deleter' we would need to test that
        //   'get_deleter' of instances created empty, or with the default or
        //   test allocator, a factory, or function-like deleters does return a
        //   pointer to the deleter if the correct type is passed as template
        //   argument of 'get_deleter' and 0 otherwise.  Next, we would test
        //   that the comparison operators and functors do return the correct
        //   comparison results.  Finally, we would test the 'make_shared' and
        //   'allocate_shared' functions using the same test plan as in case 5.
        //
        // Testing:
        //   CONCERN: C++ 'bsl::shared_ptr' COMPLIANCE
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Concern: 'bsl::shared_ptr' compliance" << endl
                          << "=====================================" << endl;

        if (verbose) cout << "\nTesting 'reset'."
                          << "\n----------------" << endl;

        numDeallocations = ta.numDeallocations();
        {
            numDeletes = 0;
            TObj *p = new(ta) TObj(&numDeletes);

            Obj x(p, &ta); Obj const& X=x;

            numAllocations = ta.numAllocations();

            if (veryVerbose) {
                T_ P_(numDeletes) P(X.numReferences())
            }

            ASSERT(0 == numDeletes);
            ASSERT(p == X.ptr());
            ASSERT(1 == X.numReferences());

            x.reset();

            numDeallocations += 2;
            ASSERT(numDeallocations == ta.numDeallocations());
            ASSERT(1 == numDeletes);
            ASSERT(0 == X.ptr());
            ASSERT(0 == X.numReferences());
        }
        ASSERT(1 == numDeletes);
        ASSERT(numDeallocations == ta.numDeallocations());
        if (veryVerbose) {
            T_ P_(numDeletes)
            P_(numDeallocations)
            P(ta.numDeallocations())
        }

        if (verbose) cout << "\nTesting reset(ptr)"
                          << "\n------------------" << endl;

        {
            numDeletes = 0;
            TObj *p = new TObj(&numDeletes);

            Obj x; Obj const& X=x;

            x.reset(p);

            if (veryVerbose) {
                T_ P_(numDeletes) P(X.numReferences())
            }
            ASSERT(0 == numDeletes);
            ASSERT(p == X.ptr());
            ASSERT(1 == X.numReferences());
        }
        ASSERT(1 == numDeletes);
        if (veryVerbose) {
            T_ P(numDeletes);
        }

        if (verbose) cout << "\nTesting reset(ptr,deleter)."
                          << "\n---------------------------" << endl;

        numDeallocations = ta.numDeallocations();
        {
            numDeletes = 0;
            TObj *p = new(ta) TObj(&numDeletes);
            numAllocations = ta.numAllocations();

            MyTestDeleter deleter(&ta);
            Obj x; Obj const& X=x;

            x.reset(p, deleter);

            ASSERT(numAllocations == ta.numAllocations());

            if (veryVerbose) {
                T_ P_(numDeletes) P(X.numReferences())
            }
            ASSERT(0 == numDeletes);
            ASSERT(p == X.ptr());
            ASSERT(1 == X.numReferences());
        }
        ASSERT(1 == numDeletes);
        ASSERT(++numDeallocations == ta.numDeallocations());
        if (veryVerbose) {
            T_ P_(numDeletes) P_(numDeallocations) P(ta.numDeallocations())
        }

        if (verbose) cout << "\nTesting 'reset(source, ptr)'."
                          << "\n-----------------------------" << endl;

        numDeallocations = ta.numDeallocations();
        {
            numDeletes = 0;
            TObj *p = new(ta) TObj(&numDeletes);
            Obj x(p,&ta, 0); Obj const& X = x;

            numAllocations = ta.numAllocations();
            ASSERT(p == X.ptr());
            ASSERT(1 == X.numReferences());

            bcema_SharedPtr<double> y; bcema_SharedPtr<double> const& Y=y;
            double dummy;

            y.reset(X, &dummy);
            ASSERT(&dummy == Y.ptr());
            ASSERT(2 == X.numReferences());
            ASSERT(2 == Y.numReferences());
            ASSERT(numAllocations == ta.numAllocations());
            ASSERT(numDeallocations == ta.numDeallocations());
            ASSERT(0 == numDeletes);

            int dummy2;
            bcema_SharedPtr<int> z;
            bcema_SharedPtr<int> const& Z=z;
            z.loadAlias(Y, &dummy2);
            ASSERT(&dummy2 == Z.ptr());
            ASSERT(3 == Z.numReferences());
            ASSERT(3 == Y.numReferences());
            ASSERT(&dummy == Y.ptr());
            ASSERT(3 == X.numReferences());
            ASSERT(p == X.ptr());
            ASSERT(numAllocations == ta.numAllocations());
            ASSERT(numDeallocations == ta.numDeallocations());
            ASSERT(0 == numDeletes);

        }
        ASSERT(++numDeallocations == ta.numDeallocations());
        ASSERT(1 == numDeletes);
        if (veryVerbose) {
            T_ P_(numDeletes); P_(numDeallocations); P(ta.numDeallocations());
        }

        if (verbose) cout << "\nTesting global 'swap' function."
                          << "\n-------------------------------" << endl;

        int numDeletes1 = 0;
        numDeletes = 0;
        {
            MyTestObject *p1 = new MyTestObject(&numDeletes);
            MyTestObject *p2 = new MyTestObject(&numDeletes1);
            Obj x(p1); const Obj &X = x;

            ASSERT(p1 == X.ptr());
            ASSERT(1 == X.numReferences());

            ASSERT(0 == numDeletes);
            ASSERT(0 == numDeletes1);
            {
                Obj y(p2); const Obj &Y = y;
                ASSERT(p2 == Y.ptr());
                ASSERT(1 == Y.numReferences());
                ASSERT(0 == numDeletes);
                ASSERT(0 == numDeletes1);

                swap(x, y);

                ASSERT(p2 == X.ptr());
                ASSERT(p1 == Y.ptr());
                ASSERT(0 == numDeletes);
                ASSERT(0 == numDeletes1);
                ASSERT(1 == X.numReferences());
                ASSERT(1 == Y.numReferences());
            }
            ASSERT(p2 == X.ptr());
            ASSERT(1 == X.numReferences());
            ASSERT(1 == numDeletes);
            ASSERT(0 == numDeletes1);
        }
        ASSERT(1 == numDeletes);
        ASSERT(1 == numDeletes1);

      } break;
      case 14: {
        // --------------------------------------------------------------------
        // TESTING CONCERN: SHARED PTR IN-PLACE INSIDE 'bdef_Function' OBJECT
        //
        // Plan:  Simply instantiate a 'bcema_SharedPtr' to a function object
        //   inside a 'bdef_Function' object and make sure that the
        //   function object is created in-place.  This can even be achieved
        //   without creating a shared pointer, from the type only, using the
        //   'bdef_FunctionUtil::IsInplace' utility.
        //
        // Testing:
        //   CONCERN: Shared ptr in-place inside 'bdef_Function' object
        // --------------------------------------------------------------------

        if (verbose)
            cout << endl
                 << "Concern: Shared ptr in-place in 'bdef_Function'" << endl
                 << "===============================================" << endl;

        typedef bcema_SharedPtr<MyTestFunctor>  SharedFunctor;

        ASSERT(1 == bdef_FunctionUtil::IsInplace<SharedFunctor>::VALUE);

      } break;
      case 13: {
        // --------------------------------------------------------------------
        // TESTING conversion to bdema_ManagedPtr
        //
        // Plan: TBD
        //
        // Testing:
        //   bdema_ManagedPtr<TYPE> managedPtr() const;
        // --------------------------------------------------------------------

        if (verbose)
            cout << endl
                 << "Testing conversion to bdema_ManagedPtr" << endl
                 << "======================================" << endl;

        bdema_ManagedPtr<MyPDTestObject> mp;
        bcema_SharedPtr<MyPDTestObject>  sp(mp);

        numDeallocations = ta.numDeallocations();
        {
            numDeletes = 0;
            TObj *p = new(ta) TObj(&numDeletes);

            Obj x(p, &ta, 0); Obj const& X=x;
            numAllocations = ta.numAllocations();

            if (veryVerbose) {
                T_ T_ P_(numDeletes); P(X.numReferences());
            }
            ASSERT(0 == numDeletes);
            ASSERT(p == X.ptr());
            ASSERT(1 == X.numReferences());

            {
                bdema_ManagedPtr<TObj> y(X.managedPtr());
                bdema_ManagedPtr<TObj> const& Y=y;

                ASSERT(0 == numDeletes);
                ASSERT(p == X.ptr());
                ASSERT(2 == X.numReferences());
                ASSERT(p == Y.ptr());
                ASSERT(numAllocations == ta.numAllocations());
                ASSERT(numDeallocations == ta.numDeallocations());
            }
            ASSERT(0 == numDeletes);
            ASSERT(p == X.ptr());
            ASSERT(1 == X.numReferences());
            ASSERT(numAllocations == ta.numAllocations());
            ASSERT(numDeallocations == ta.numDeallocations());
        }
        ASSERT(1 == numDeletes);
        ASSERT(++numDeallocations == ta.numDeallocations());

        numDeallocations = ta.numDeallocations();
        {
            numDeletes = 0;
            TObj *p = new(ta) TObj(&numDeletes);

            Obj x(p, &ta, 0); Obj const& X=x;
            numAllocations = ta.numAllocations();

            if (veryVerbose) {
                T_ T_ P_(numDeletes); P(X.numReferences());
            }
            ASSERT(0 == numDeletes);
            ASSERT(p == X.ptr());
            ASSERT(1 == X.numReferences());

            bdema_ManagedPtr<TObj> y(X.managedPtr());
            bdema_ManagedPtr<TObj> const& Y=y;

            ASSERT(0 == numDeletes);
            ASSERT(p == X.ptr());
            ASSERT(2 == X.numReferences());
            ASSERT(p == Y.ptr());
            ASSERT(numAllocations == ta.numAllocations());
            ASSERT(numDeallocations == ta.numDeallocations());

            x.load((TObj*)0);

            ASSERT(0 == numDeletes);
            ASSERT(numAllocations == ta.numAllocations());
            ASSERT(numDeallocations == ta.numDeallocations());
        }
        ASSERT(1 == numDeletes);
        ASSERT(numAllocations == ta.numAllocations());
        ASSERT(++numDeallocations == ta.numDeallocations());

      } break;
      case 12: {
        // --------------------------------------------------------------------
        // TESTING 'swap'
        //
        // Plan: TBD
        //
        // Testing:
        //   void swap(bcema_SharedPtr<OTHER_TYPE> &src)
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "Testing 'swap'" << endl
                          << "==============" << endl;

        if (verbose) cout << "\tWith default allocator." << endl;

        int numDeletes1 = 0;
        numDeletes = 0;
        {
            MyTestObject *p1 = new MyTestObject(&numDeletes);
            MyTestObject *p2 = new MyTestObject(&numDeletes1);
            Obj x(p1); const Obj &X = x;

            ASSERT(p1 == X.ptr());
            ASSERT(1 == X.numReferences());

            ASSERT(0 == numDeletes);
            ASSERT(0 == numDeletes1);
            {
                Obj y(p2); const Obj &Y = y;
                ASSERT(p2 == Y.ptr());
                ASSERT(1 == Y.numReferences());
                ASSERT(0 == numDeletes);
                ASSERT(0 == numDeletes1);

                x.swap(y);

                ASSERT(p2 == X.ptr());
                ASSERT(p1 == Y.ptr());
                ASSERT(0 == numDeletes);
                ASSERT(0 == numDeletes1);
                ASSERT(1 == X.numReferences());
                ASSERT(1 == Y.numReferences());
            }
            ASSERT(p2 == X.ptr());
            ASSERT(1 == X.numReferences());
            ASSERT(1 == numDeletes);
            ASSERT(0 == numDeletes1);
        }
        ASSERT(1 == numDeletes);
        ASSERT(1 == numDeletes1);

        if (verbose) cout << "\tWith mix of deleters." << endl;

        bslma_TestAllocator da(veryVeryVerbose);
        bslma_DefaultAllocatorGuard defaultGuard(&da);

        // WARNING:  Installing a test allocator as the default means that
        //    bcema_SharedPtr<T> x(new T());
        // will break, as it tries to delete the object using the installed
        // default allocator (i.e., the test allocator) and not the new/delete
        // allocator.  Therefore, from then on in this test case, whenever
        // using the expression 'new T()', should be replaced by 'new(da) T()'.

        numDeletes1 = 0;
        numDeletes = 0;
        {
            MyTestObject *p1 = new(da) MyTestObject(&numDeletes);
            MyTestObject *p2 = new(ta) MyTestObject(&numDeletes1);
            Obj x(p1); const Obj &X = x;

            ASSERT(p1 == X.ptr());
            ASSERT(1 == X.numReferences());

            ASSERT(0 == numDeletes);
            ASSERT(0 == numDeletes1);
            {
                Obj y(p2, &ta); const Obj &Y = y;
                ASSERT(p2 == Y.ptr());
                ASSERT(1 == Y.numReferences());
                ASSERT(0 == numDeletes);
                ASSERT(0 == numDeletes1);

                x.swap(y);

                ASSERT(p2 == X.ptr());
                ASSERT(p1 == Y.ptr());
                ASSERT(0 == numDeletes);
                ASSERT(0 == numDeletes1);
                ASSERT(1 == X.numReferences());
                ASSERT(1 == Y.numReferences());
            }
            ASSERT(p2 == X.ptr());
            ASSERT(1 == X.numReferences());
            ASSERT(1 == numDeletes);
            ASSERT(0 == numDeletes1);
        }
        ASSERT(1 == numDeletes);
        ASSERT(1 == numDeletes1);

        numDeletes1 = 0;
        numDeletes = 0;
        {
            MyTestObject *p1 = new(da) MyTestObject(&numDeletes);
            MyTestObject *p2 = new(ta) MyTestObject(&numDeletes1);
            MyTestDeleter deleter(&ta);
            Obj x(p1); const Obj &X = x;

            ASSERT(p1 == X.ptr());
            ASSERT(1 == X.numReferences());

            ASSERT(0 == numDeletes);
            ASSERT(0 == numDeletes1);
            {
                Obj y(p2, deleter, &da); const Obj &Y = y;
                ASSERT(p2 == Y.ptr());
                ASSERT(1 == Y.numReferences());
                ASSERT(0 == numDeletes);
                ASSERT(0 == numDeletes1);

                x.swap(y);

                ASSERT(p2 == X.ptr());
                ASSERT(p1 == Y.ptr());
                ASSERT(0 == numDeletes);
                ASSERT(0 == numDeletes1);
                ASSERT(1 == X.numReferences());
                ASSERT(1 == Y.numReferences());
            }
            ASSERT(p2 == X.ptr());
            ASSERT(1 == X.numReferences());
            ASSERT(1 == numDeletes);
            ASSERT(0 == numDeletes1);
        }
        ASSERT(1 == numDeletes);
        ASSERT(1 == numDeletes1);

      } break;
      case 11: {
        // --------------------------------------------------------------------
        // TESTING 'PtrLess<TYPE>'
        //   Test that 'bcema_SharedPtrUtil::PtrLess' is a functor that
        //   compares shared pointers according to their 'ptr()' values.
        //
        // Plan: Create a functor 'LT' and check that it does compare suitably
        //   created and ordered shared pointers to either 'MyTestObject' or a
        //   derived type.
        //
        // Testing:
        //   struct bcema_SharedPtrUtil::PtrLess<TYPE>;
        // --------------------------------------------------------------------

        if (verbose) cout << "\tTesting 'bcema_SharedPtrUtil::PtrLess'\n"
                          << "\t--------------------------------------\n";

        bcema_SharedPtrUtil::PtrLess<MyTestObject> LT;
        {
            MyTestDerivedObject *pd = new(ta) MyTestDerivedObject(&numDeletes);
            MyTestDerivedObject *qd = new(ta) MyTestDerivedObject(&numDeletes);
            if (qd < pd) { bsl::swap(pd, qd); }
            ASSERT(pd < qd);

            TObj *p = pd;
            TObj *q = qd;
            if (q < p) { bsl::swap(p, q); }
            ASSERT(p  < q);
            ASSERT(pd < q);
            ASSERT(p  < qd);

            bcema_SharedPtr<MyTestDerivedObject> xd(pd, &ta);
            const Obj& XD = xd;

            bcema_SharedPtr<MyTestDerivedObject> yd(qd, &ta);
            const Obj& YD = yd;

            Obj x(XD);  const Obj& X = x;
            Obj y(YD);  const Obj& Y = y;

            if (veryVerbose)
                bsl::cout << "\tFunctor comparisons with self." << bsl::endl;

#if 0  // Uncomment to generate compilation errors.
            ASSERT(!(X  <  X));
            ASSERT(!(X  < XD));
            ASSERT(!(XD < X));
            ASSERT(!(XD < XD));
#endif

#if 0
            bsl::map<Obj, int> sharedPtrMap;
            sharedPtrMap[X] = 1;
#endif

            ASSERT(!LT(X,  X));
            ASSERT(!LT(X,  XD));
            ASSERT(!LT(XD, X));
            ASSERT(!LT(XD, XD));

#if 0
            {
                typedef bsl::map<Obj, int, bcema_SharedPtrUtil::PtrLess<Obj> >
                   Map;
                Map map1;
                map1.insert(bsl::make_pair(X, 1));
                map1.insert(bsl::make_pair(Y, 2));
                Map::iterator it1 = map1.begin();
                ASSERT(it1->first == X && it1->second == 1);
                ++it1;
                ASSERT(it1->first == Y && it1->second == 2);
            }
#endif

            {
                typedef bsl::map<Obj, int, bcema_SharedPtrLess> Map;
                Map map1;
                map1.insert(bsl::make_pair(X, 1));
                map1.insert(bsl::make_pair(Y, 2));
                Map::iterator it1 = map1.begin();
                ASSERT(it1->first == X && it1->second == 1);
                ++it1;
                ASSERT(it1->first == Y && it1->second == 2);
            }

            if (veryVerbose)
                bsl::cout << "\tFunctor comparisons with other." << bsl::endl;

#if 0  // Uncomment to generate compilation errors.
            ASSERT(X  < Y));
            ASSERT(XD < Y));
            ASSERT(X  < YD));
            ASSERT(XD < YD));
#endif

            ASSERT(LT(X,  Y));
            ASSERT(LT(XD, Y));
            ASSERT(LT(X,  YD));
            ASSERT(LT(XD, YD));
        }

      } break;
      case 10: {
        // --------------------------------------------------------------------
        // TESTING
        //   Test that 'createInplaceUninitializedBuffer' creates a buffer of
        //   the specified size, properly aligned, and that the buffer is
        //   deallocated properly.
        //
        // Plan:  For every size between 1 and 5 times the maximal alignment,
        //   create a shared pointer to a buffer of this size using
        //   'createInplaceUninitializedBuffer', and verify that the returned
        //   pointer is at least naturally aligned, that we can write into that
        //   buffer, and that the allocated buffer has at least the requested
        //   size.
        //
        // Testing:
        //   bcema_SharedPtr<char> createInplaceUninitializedBuffer(...)
        // --------------------------------------------------------------------

        if (verbose)
            cout << endl
                 << "Testing 'createInplaceUninitializedBuffer'" << endl
                 << "==========================================" << endl;

        static const char EXP[] = "createInplaceUninitializedBuffer";

        numAllocations = ta.numAllocations();
        numDeallocations = ta.numDeallocations();

        if (verbose) {
            cout << endl
                 << "Sizes of InplaceRepImpl<TYPE>:" << endl
                 << "------------------------------"<< endl;

            enum {
                ONE_ALIGN = 1,
                TWO_ALIGN = (bsls_AlignmentUtil::BSLS_MAX_ALIGNMENT > 2)
                                      ? 2
                                      : bsls_AlignmentUtil::BSLS_MAX_ALIGNMENT,
                FOUR_ALIGN = (bsls_AlignmentUtil::BSLS_MAX_ALIGNMENT > 4)
                                      ? 4
                                      : bsls_AlignmentUtil::BSLS_MAX_ALIGNMENT,
                EIGHT_ALIGN = (bsls_AlignmentUtil::BSLS_MAX_ALIGNMENT > 8)
                                       ? 8
                                       : bsls_AlignmentUtil::BSLS_MAX_ALIGNMENT
            };

            P(sizeof(bcema_SharedPtrInplaceRep<
                                bsls_AlignmentToType<ONE_ALIGN>::Type[1] >));
            P(sizeof(bcema_SharedPtrInplaceRep<
                                bsls_AlignmentToType<TWO_ALIGN>::Type[1] >));
            P(sizeof(bcema_SharedPtrInplaceRep<
                                bsls_AlignmentToType<ONE_ALIGN>::Type[3] >));
            P(sizeof(bcema_SharedPtrInplaceRep<
                                bsls_AlignmentToType<FOUR_ALIGN>::Type[1] >));
            P(sizeof(bcema_SharedPtrInplaceRep<
                                bsls_AlignmentToType<ONE_ALIGN>::Type[5] >));
            P(sizeof(bcema_SharedPtrInplaceRep<
                                bsls_AlignmentToType<TWO_ALIGN>::Type[3] >));
            P(sizeof(bcema_SharedPtrInplaceRep<
                                bsls_AlignmentToType<ONE_ALIGN>::Type[7] >));
            P(sizeof(bcema_SharedPtrInplaceRep<
                                bsls_AlignmentToType<EIGHT_ALIGN>::Type[1] >));
            P(sizeof(bcema_SharedPtrInplaceRep<
                                bsls_AlignmentToType<ONE_ALIGN>::Type[9] >));
            P(sizeof(bcema_SharedPtrInplaceRep<
                                bsls_AlignmentToType<TWO_ALIGN>::Type[5] >));
            P(sizeof(bcema_SharedPtrInplaceRep<
                                bsls_AlignmentToType<ONE_ALIGN>::Type[11] >));
            P(sizeof(bcema_SharedPtrInplaceRep<
                                bsls_AlignmentToType<FOUR_ALIGN>::Type[3] >));
            P(sizeof(bcema_SharedPtrInplaceRep<
                                bsls_AlignmentToType<ONE_ALIGN>::Type[13] >));
            P(sizeof(bcema_SharedPtrInplaceRep<
                                bsls_AlignmentToType<TWO_ALIGN>::Type[7] >));
            P(sizeof(bcema_SharedPtrInplaceRep<
                                bsls_AlignmentToType<ONE_ALIGN>::Type[15] >));
        }

        if (verbose)
            cout << endl
                 << "Testing 'createInplaceUninitializedBuffer'." << endl
                 << "-------------------------------------------" << endl;

        for (int size = 1;
             size < 5 * bsls_AlignmentUtil::BSLS_MAX_ALIGNMENT;
             ++size) {
            {
                bcema_SharedPtr<char> x;
                bcema_SharedPtr<char> const& X = x;

                x = bcema_SharedPtrUtil::createInplaceUninitializedBuffer(
                                                                    size, &ta);
                bsl::strncpy(X.ptr(), EXP, size);

                static const int ALLOC_SIZE =
                    sizeof(bcema_SharedPtrInplaceRep<char>) + size - 1;
                LOOP_ASSERT(size, ++numAllocations == ta.numAllocations());
                LOOP_ASSERT(size, ALLOC_SIZE <= ta.lastAllocatedNumBytes());

                LOOP_ASSERT(size, X);
                LOOP_ASSERT(size, 0 == bsl::strncmp(EXP, X.ptr(), size));

                int alignment =
                          bsls_AlignmentUtil::calculateAlignmentFromSize(size);
                int alignmentOffset =
                                  bsls_AlignmentUtil::calculateAlignmentOffset(
                                                           X.ptr(), alignment);
                LOOP3_ASSERT(size, alignment, alignmentOffset,
                              0 == alignmentOffset);

                void* repAddr = ta.lastAllocatedAddress();
                int repAllocSize = ta.lastAllocatedNumBytes();
                LOOP4_ASSERT(repAddr, repAllocSize, (void*) X.ptr(), size,
                             (char*) repAddr + repAllocSize >= X.ptr() + size);

                if (veryVerbose) {
                    P_(size);
                    P_(ta.numAllocations());
                    P(ta.lastAllocatedNumBytes());
                    P_(alignment);
                    P((void *)X.ptr());
                    P(bsls_AlignmentUtil::calculateAlignmentOffset(X.ptr(),
                                                               alignment));
                }
            }
            ASSERT(++numDeallocations == ta.numDeallocations());
        }

      } break;
      case 9: {
        // --------------------------------------------------------------------
        // TESTING explicit cast 'load' operations
        //   Test that explicit cast operations properly loads the object
        //
        // Plan: TBD
        //
        // Testing:
        //  bcema_SharedPtr<TARGET> dynamicCast(bcema_SharedPtr<SOURCE> ..
        //  bcema_SharedPtr<TARGET> staticCast(bcema_SharedPtr<SOURCE> const& )
        //  bcema_SharedPtr<TARGET> constCast(bcema_SharedPtr<SOURCE> const& )
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing explicit cast operations" << endl
                          << "================================" << endl;

        if (verbose) cout << "\nTesting 'dynamicCast'"
                          << "\n---------------------\n";

        numDeallocations = ta.numDeallocations();
        {
            numDeletes = 0;
            MyTestDerivedObject *p = new(ta) MyTestDerivedObject(&numDeletes);
            Obj x(p,&ta, 0); Obj const& X = x;

            numAllocations = ta.numAllocations();
            ASSERT(static_cast<MyTestObject*>(p) == X.ptr());
            ASSERT(1 == X.numReferences());

            bcema_SharedPtr<MyTestDerivedObject> y;
            bcema_SharedPtr<MyTestDerivedObject> const& Y=y;

            {
                // This inner block necessary against Sun CC bug, the lifetime
                // of the temporary copied into y would otherwise pollute the
                // Y.numReferences below.
                y = bcema_SharedPtrUtil::dynamicCast<MyTestDerivedObject>(X);
            }
            if (veryVerbose) {
                P_(Y.ptr());
                P_(X.numReferences());
                P(Y.numReferences());
            }
            ASSERT(p == Y.ptr());
            ASSERT(2 == X.numReferences());
            ASSERT(2 == Y.numReferences());
            ASSERT(numAllocations == ta.numAllocations());
            ASSERT(numDeallocations == ta.numDeallocations());
            ASSERT(0 == numDeletes);
        }
        ASSERT(++numDeallocations == ta.numDeallocations());
        ASSERT(1 == numDeletes);

        if (verbose) cout << "\nTesting 'staticCast'"
                          << "\n--------------------\n";

        numDeallocations = ta.numDeallocations();
        {
            numDeletes = 0;
            MyTestDerivedObject *p = new(ta) MyTestDerivedObject(&numDeletes);
            bcema_SharedPtr<MyTestDerivedObject> x(p,&ta, 0);
            bcema_SharedPtr<MyTestDerivedObject> const& X = x;

            numAllocations = ta.numAllocations();
            ASSERT(p == X.ptr());
            ASSERT(1 == X.numReferences());

            Obj y(bcema_SharedPtrUtil::staticCast<TObj>(X)); Obj const& Y=y;

            ASSERT(static_cast<MyTestObject*>(p) == Y.ptr());
            ASSERT(2 == X.numReferences());
            ASSERT(2 == Y.numReferences());
            ASSERT(numAllocations == ta.numAllocations());
            ASSERT(numDeallocations == ta.numDeallocations());
            ASSERT(0 == numDeletes);
        }
        ASSERT(++numDeallocations == ta.numDeallocations());
        ASSERT(1 == numDeletes);

        if (verbose) cout << "\nTesting 'constCast'"
                          << "\n-------------------\n";

        numDeallocations = ta.numDeallocations();
        {
            // Construct ConstObj with a nil deleter.
            // This exposes a former const-safety bug.
            int counter = 0;
            const MyTestObject V(&counter);
            ConstObj x(&V, bcema_SharedPtrNilDeleter(), 0);
        }
        {
            numDeletes = 0;
            TObj *p = new(ta) TObj(&numDeletes);
            ConstObj x(p,&ta, 0); ConstObj const& X = x;

            numAllocations = ta.numAllocations();
            ASSERT(p == X.ptr());
            ASSERT(1 == X.numReferences());

            Obj y(bcema_SharedPtrUtil::constCast<TObj>(X)); Obj const& Y=y;

            ASSERT(const_cast<TObj*>(p) == Y.ptr());
            ASSERT(2 == X.numReferences());
            ASSERT(2 == Y.numReferences());
            ASSERT(numAllocations == ta.numAllocations());
            ASSERT(numDeallocations == ta.numDeallocations());
            ASSERT(0 == numDeletes);
        }
        ASSERT(++numDeallocations == ta.numDeallocations());
        ASSERT(1 == numDeletes);

      } break;
      case 8: {
        // --------------------------------------------------------------------
        // TESTING explicit cast 'load' operations
        //
        //
        // Plan: TBD
        //
        // Testing:
        //   void loadAlias(bcema_SharedPtr<OTHER_TYPE> const& target, TYPE *)
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing alias operations" << endl
                          << "========================" << endl;

        if (verbose) cout << "\nTesting 'loadAlias' (unset target)"
                          << "\n----------------------------------\n";

        numDeallocations = ta.numDeallocations();
        {
            numDeletes = 0;
            TObj *p = new(ta) TObj(&numDeletes);
            Obj x(p, &ta, 0); Obj const& X=x;

            numAllocations = ta.numAllocations();
            ASSERT(0 == numDeletes);
            ASSERT(p == X.ptr());
            ASSERT(1 == X.numReferences());

            bcema_SharedPtr<double> y; bcema_SharedPtr<double> const& Y=y;

            double dummy;
            y.loadAlias(X, &dummy);
            ASSERT(&dummy == Y.ptr());
            ASSERT(2 == Y.numReferences());
            ASSERT(0 == numDeletes);
            ASSERT(p == X.ptr());
            ASSERT(2 == X.numReferences());
            ASSERT(numAllocations == ta.numAllocations());
            ASSERT(numDeallocations == ta.numDeallocations());
        }
        ASSERT(1 == numDeletes);
        ASSERT(++numDeallocations == ta.numDeallocations());

        if (verbose) cout << "\nTesting 'loadAlias' (unset target and src)"
                          << "\n------------------------------------------\n";
        {
            bcema_SharedPtr<MyTestObject2> x;
            bcema_SharedPtr<MyTestObject2> const& X=x;

            ASSERT(0 == X.ptr());
            ASSERT(0 == X.numReferences());

            bcema_SharedPtr<double> y; bcema_SharedPtr<double> const& Y=y;

            double dummy;
            y.loadAlias(X, &dummy);

            ASSERT(0 == Y.ptr());
            ASSERT(0 == Y.numReferences());
        }

        if (verbose) cout << "\nTesting 'loadAlias' (partially unset)"
                          << "\n-------------------------------------\n";

        numDeallocations = ta.numDeallocations();
        {
            numDeletes = 0;
            TObj *p = new(ta) TObj(&numDeletes);
            Obj x(p, &ta, 0); Obj const& X=x;

            numAllocations = ta.numAllocations();
            ASSERT(0 == numDeletes);
            ASSERT(p == X.ptr());
            ASSERT(1 == X.numReferences());

            double dummy;
            bcema_SharedPtr<double> y(X, &dummy);
                                            bcema_SharedPtr<double> const& Y=y;
            ASSERT(2 == X.numReferences());
            ASSERT(2 == Y.numReferences());

            y.loadAlias(X, (double*)0);
            ASSERT(0 == Y.ptr());
            ASSERT(0 == Y.numReferences());
            ASSERT(0 == numDeletes);
            ASSERT(p == X.ptr());
            ASSERT(1 == X.numReferences());
            ASSERT(numAllocations == ta.numAllocations());
            ASSERT(numDeallocations == ta.numDeallocations());
        }
        ASSERT(1 == numDeletes);
        ASSERT(++numDeallocations == ta.numDeallocations());

        if (verbose) cout << "\nTesting 'loadAlias'(set)"
                          << "\n------------------------\n";

        numDeallocations = ta.numDeallocations();
        {
            numDeletes = 0;
            TObj *p = new(ta) TObj(&numDeletes);
            Obj x(p,&ta, 0); Obj const& X = x;

            numAllocations = ta.numAllocations();
            ASSERT(p == X.ptr());
            ASSERT(1 == X.numReferences());

            bcema_SharedPtr<double> y; bcema_SharedPtr<double> const& Y=y;
            double dummy;

            y.loadAlias(X, &dummy);
            ASSERT(&dummy == Y.ptr());
            ASSERT(2 == X.numReferences());
            ASSERT(2 == Y.numReferences());
            ASSERT(numAllocations == ta.numAllocations());
            ASSERT(numDeallocations == ta.numDeallocations());
            ASSERT(0 == numDeletes);

            int dummy2;
            bcema_SharedPtr<int> z;
            bcema_SharedPtr<int> const& Z=z;
            z.loadAlias(Y, &dummy2);
            ASSERT(&dummy2 == Z.ptr());
            ASSERT(3 == Z.numReferences());
            ASSERT(3 == Y.numReferences());
            ASSERT(&dummy == Y.ptr());
            ASSERT(3 == X.numReferences());
            ASSERT(p == X.ptr());
            ASSERT(numAllocations == ta.numAllocations());
            ASSERT(numDeallocations == ta.numDeallocations());
            ASSERT(0 == numDeletes);

        }
        ASSERT(++numDeallocations == ta.numDeallocations());
        ASSERT(1 == numDeletes);

      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING explicit cast constructors
        //   Test that the aliasing constructor work as expected
        //
        // Plan: TBD
        //
        // Testing:
        //   bcema_SharedPtr(bcema_SharedPtr<TYPE> const& alias, TYPE *object);
        // --------------------------------------------------------------------

        if (verbose)
            cout << endl
                 << "Testing explicit cast constructors" << endl
                 << "==================================" << endl;

        if (verbose) cout << "\nTesting \"alias\" constructor"
                          << "\n---------------------------\n";

        numDeallocations = ta.numDeallocations();
        {
            numDeletes = 0;
            MyTestDerivedObject *p = new(ta) MyTestDerivedObject(&numDeletes);
            Obj x(p,&ta, 0); Obj const& X = x;

            numAllocations = ta.numAllocations();
            ASSERT(p == X.ptr());
            ASSERT(1 == X.numReferences());

            double dummy;
            bcema_SharedPtr<double> y(X, &dummy);
            bcema_SharedPtr<double> const& Y=y;

            ASSERT(&dummy == Y.ptr());
            ASSERT(2 == X.numReferences());
            ASSERT(2 == Y.numReferences());
            ASSERT(numAllocations   == ta.numAllocations());
            ASSERT(numDeallocations == ta.numDeallocations());
            ASSERT(0 == numDeletes);
        }
        ASSERT(++numDeallocations == ta.numDeallocations());
        ASSERT(1 == numDeletes);

        if (verbose) cout << "\nTesting \"alias\" constructor (nil object)"
                          << "\n----------------------------------------\n";

        numDeallocations = ta.numDeallocations();
        {
            numDeletes = 0;
            MyTestDerivedObject *p = new(ta) MyTestDerivedObject(&numDeletes);
            Obj x(p,&ta, 0); Obj const& X = x;

            numAllocations = ta.numAllocations();
            ASSERT(p == X.ptr());
            ASSERT(1 == X.numReferences());

            bcema_SharedPtr<double> y(X, 0);
            bcema_SharedPtr<double> const& Y=y;

            ASSERT(0 == Y.ptr());
            ASSERT(1 == X.numReferences());
            ASSERT(0 == Y.numReferences());
            ASSERT(numAllocations   == ta.numAllocations());
            ASSERT(numDeallocations == ta.numDeallocations());
            ASSERT(0 == numDeletes);
        }
        ASSERT(++numDeallocations == ta.numDeallocations());
        ASSERT(1 == numDeletes);

        if (verbose) cout << "\nTesting \"alias\" constructor (unset target)"
                          << "\n------------------------------------------\n";

        numDeallocations = ta.numDeallocations();
        {
            numDeletes = 0;
            Obj x; Obj const& X = x;

            numAllocations = ta.numAllocations();
            ASSERT(0 == X.ptr());
            ASSERT(0 == X.numReferences());

            double dummy;
            bcema_SharedPtr<double> y(X, &dummy);
            bcema_SharedPtr<double> const& Y=y;

            ASSERT(0 == Y.ptr());
            ASSERT(0 == X.numReferences());
            ASSERT(0 == Y.numReferences());
            ASSERT(numAllocations   == ta.numAllocations());
            ASSERT(numDeallocations == ta.numDeallocations());
            ASSERT(0 == numDeletes);
        }
        ASSERT(numDeallocations == ta.numDeallocations());

        numDeallocations = ta.numDeallocations();
        {
            numDeletes = 0;
            Obj x(new (ta) TObj(&numDeletes), &ta, 0); Obj const& X = x;

            numAllocations = ta.numAllocations();
            ASSERT(0 != X.ptr());
            ASSERT(1 == X.numReferences());

            bcema_SharedPtr<double> y(X, (double *)0);
            bcema_SharedPtr<double> const& Y=y;

            ASSERT(0 == Y.ptr());
            ASSERT(1 == X.numReferences());
            ASSERT(0 == Y.numReferences());
            ASSERT(numAllocations   == ta.numAllocations());
            ASSERT(numDeallocations == ta.numDeallocations());
            ASSERT(0 == numDeletes);
        }
        ASSERT(++numDeallocations == ta.numDeallocations());

      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING 'load'
        //
        // Plan: TBD
        //
        // Testing:
        //   void load(PTRTYPE *ptr, bslma_Allocator *allocator=0)
        //   void load(PTRTYPE *ptr, DELETER const&, bslma_Allocator *)
        // --------------------------------------------------------------------
          if (verbose)
              cout << endl
                   << "Testing load of null ptr(on empty object)" << endl
                   << "-----------------------------------------" << endl;

          numDeallocations = ta.numDeallocations();
          {
              Obj x; Obj const& X=x;
              x.load((TObj*)0);
              ASSERT(0 == X.ptr());
              ASSERT(0 == X.numReferences());

              numAllocations = ta.numAllocations();

              Obj y; Obj const&Y=y;
              y.load((TObj*)0, &ta);
              ASSERT(0 == Y.ptr());
              ASSERT(0 == Y.numReferences());
              ASSERT(numAllocations == ta.numAllocations());

              Obj z; Obj const&Z=z;
              z.load((TObj*)0, &ta, &ta);
              ASSERT(0 == Z.ptr());
              ASSERT(0 == Z.numReferences());
              ASSERT(numAllocations == ta.numAllocations());
          }
          ASSERT(numDeallocations == ta.numDeallocations());

          if (verbose)
              cout << endl
                   << "Testing load of null ptr(on non-empty object)" << endl
                   << "---------------------------------------------" << endl;
          {
              numDeletes = 0;
              Obj x(new (ta) TObj(&numDeletes), &ta, 0); Obj const& X=x;
              numAllocations = ta.numAllocations();
              numDeallocations = ta.numDeallocations();
              ASSERT(0 == numDeletes);
              x.load((TObj*)0);
              ASSERT(1 == numDeletes);
              ASSERT(numAllocations == ta.numAllocations());
              ASSERT(++numDeallocations == ta.numDeallocations());
              ASSERT(0 == X.ptr());
              ASSERT(0 == X.numReferences());
          }
          ASSERT(numAllocations == ta.numAllocations());
          ASSERT(numDeallocations == ta.numDeallocations());

          {
              numDeletes = 0;
              Obj y(new (ta) TObj(&numDeletes), &ta, 0); Obj const& Y=y;
              numAllocations = ta.numAllocations();
              numDeallocations = ta.numDeallocations();
              ASSERT(0 == numDeletes);
              y.load((TObj*)0, &ta);
              ASSERT(1 == numDeletes);
              ASSERT(numAllocations == ta.numAllocations());
              ASSERT(++numDeallocations == ta.numDeallocations());
              ASSERT(0 == Y.ptr());
              ASSERT(0 == Y.numReferences());
              ASSERT(numAllocations == ta.numAllocations());
          }
          ASSERT(numAllocations == ta.numAllocations());
          ASSERT(numDeallocations == ta.numDeallocations());

          {
              numDeletes = 0;
              Obj z(new (ta) TObj(&numDeletes), &ta, 0); Obj const& Z=z;
              numAllocations = ta.numAllocations();
              numDeallocations = ta.numDeallocations();
              ASSERT(0 == numDeletes);
              z.load((TObj*)0, &ta, &ta);
              ASSERT(1 == numDeletes);
              ASSERT(numAllocations == ta.numAllocations());
              ASSERT(++numDeallocations == ta.numDeallocations());
              ASSERT(0 == Z.ptr());
              ASSERT(0 == Z.numReferences());
              ASSERT(numAllocations == ta.numAllocations());
          }
          ASSERT(numAllocations == ta.numAllocations());
          ASSERT(numDeallocations == ta.numDeallocations());

          if (verbose)
              cout << "\nTesting load of non-null ptr (on non-empty object)"
                   << "\n--------------------------------------------------\n";

          {
              numDeletes = 0;
              Obj x(new (ta) TObj(&numDeletes), &ta, 0); Obj const& X=x;
              numAllocations = ta.numAllocations();
              numDeallocations = ta.numDeallocations();
              ASSERT(0 == numDeletes);
              TObj *p = new TObj(&numDeletes);
              x.load(p);
              ASSERT(1 == numDeletes);
              ASSERT(numAllocations == ta.numAllocations());
              ASSERT(++numDeallocations == ta.numDeallocations());
              ASSERT(p == X.ptr());
              ASSERT(1 == X.numReferences());
          }
          ASSERT(2 == numDeletes);
          ASSERT(numAllocations == ta.numAllocations());
          ASSERT(numDeallocations == ta.numDeallocations());

          {
              numDeletes = 0;
              Obj y(new (ta) TObj(&numDeletes), &ta, 0); Obj const& Y=y;
              numAllocations = ta.numAllocations();
              numDeallocations = ta.numDeallocations();
              ASSERT(0 == numDeletes);
              TObj *p = new(ta) TObj(&numDeletes);
              numAllocations = ta.numAllocations();
              y.load(p, &ta);
              ASSERT(1 == numDeletes);
              ASSERT(++numAllocations == ta.numAllocations());
              ASSERT(++numDeallocations == ta.numDeallocations());
              ASSERT(p == Y.ptr());
              ASSERT(1 == Y.numReferences());
          }
          ASSERT(2 == numDeletes);
          ASSERT(numAllocations == ta.numAllocations());
          ASSERT((numDeallocations+2) == ta.numDeallocations());

          {
              numDeletes = 0;
              Obj z(new (ta) TObj(&numDeletes), &ta, 0); Obj const& Z=z;
              numAllocations = ta.numAllocations();
              numDeallocations = ta.numDeallocations();
              ASSERT(0 == numDeletes);
              TObj *p = new(ta) TObj(&numDeletes);
              numAllocations = ta.numAllocations();
              z.load(p, &ta, &ta);
              ASSERT(1 == numDeletes);
              ASSERT(++numAllocations == ta.numAllocations());
              ASSERT(++numDeallocations == ta.numDeallocations());
              ASSERT(p == Z.ptr());
              ASSERT(1 == Z.numReferences());
              ASSERT(numAllocations == ta.numAllocations());
          }
          ASSERT(2 == numDeletes);
          ASSERT(numAllocations == ta.numAllocations());
          ASSERT((numDeallocations+2) == ta.numDeallocations());

      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING 'createInplace'
        //
        // Concerns:
        //
        // Plan: TBD
        //
        // Testing:
        //   void createInplace(bslma_Allocator *allocator=0);
        //   void createInplace(bslma_Allocator *, A1 const& a1)
        //   void createInplace(bslma_Allocator *, A1 const& a1, ..&a2);
        //   void createInplace(bslma_Allocator *, A1 const& a1,...a3);
        //   void createInplace(bslma_Allocator *, A1 const& a1,...a4);
        //   void createInplace(bslma_Allocator *, A1 const& a1,...a5);
        //   void createInplace(bslma_Allocator *, A1 const& a1,...a6);
        //   void createInplace(bslma_Allocator *, A1 const& a1,...a7);
        //   void createInplace(bslma_Allocator *, A1 const& a1,...a8);
        //   void createInplace(bslma_Allocator *, A1 const& a1,...a9);
        //   void createInplace(bslma_Allocator *, A1 const& a1,...a10);
        //   void createInplace(bslma_Allocator *, A1 const& a1,...a11);
        //   void createInplace(bslma_Allocator *, A1 const& a1,...a12);
        //   void createInplace(bslma_Allocator *, A1 const& a1,...a13);
        //   void createInplace(bslma_Allocator *, A1 const& a1,...a14);
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "Testing 'createInplace'" << endl
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

        if (verbose) cout << "\nTesting 'createInplace' with 1 argument"
                          << "\n---------------------------------------\n";

        numAllocations = ta.numAllocations();
        numDeallocations = ta.numDeallocations();
        {
            bcema_SharedPtr<MyInplaceTestObject> x;
            bcema_SharedPtr<MyInplaceTestObject> const& X=x;
            static const MyInplaceTestObject EXP(V1);

            x.createInplace(&ta, V1);

            ASSERT(++numAllocations == ta.numAllocations());
            ASSERT(X);
            ASSERT(EXP == *(X.ptr()));
        }
        ASSERT(++numDeallocations == ta.numDeallocations());

        if (verbose) cout << "\nTesting 'createInplace' with 2 arguments"
                          << "\n----------------------------------------\n";

        numAllocations = ta.numAllocations();
        numDeallocations = ta.numDeallocations();
        {
            bcema_SharedPtr<MyInplaceTestObject> x;
            bcema_SharedPtr<MyInplaceTestObject> const& X=x;
            static const MyInplaceTestObject EXP(V1, V2);

            x.createInplace(&ta, V1, V2);

            ASSERT(++numAllocations == ta.numAllocations());
            ASSERT(X);
            ASSERT(EXP == *(X.ptr()));
        }
        ASSERT(++numDeallocations == ta.numDeallocations());

        if (verbose) cout << "\nTesting 'createInplace' with 3 arguments"
                          << "\n----------------------------------------\n";

        numAllocations = ta.numAllocations();
        numDeallocations = ta.numDeallocations();
        {
            bcema_SharedPtr<MyInplaceTestObject> x;
            bcema_SharedPtr<MyInplaceTestObject> const& X=x;
            static const MyInplaceTestObject EXP(V1, V2, V3);

            x.createInplace(&ta, V1, V2, V3);

            ASSERT(++numAllocations == ta.numAllocations());
            ASSERT(X);
            ASSERT(EXP == *(X.ptr()));
        }
        ASSERT(++numDeallocations == ta.numDeallocations());

        if (verbose) cout << "\nTesting 'createInplace' with 4 arguments"
                          << "\n----------------------------------------\n";

        numAllocations = ta.numAllocations();
        numDeallocations = ta.numDeallocations();
        {
            bcema_SharedPtr<MyInplaceTestObject> x;
            bcema_SharedPtr<MyInplaceTestObject> const& X=x;
            static const MyInplaceTestObject EXP(V1, V2, V3, V4);

            x.createInplace(&ta, V1, V2, V3, V4);

            ASSERT(++numAllocations == ta.numAllocations());
            ASSERT(X);
            ASSERT(EXP == *(X.ptr()));
        }
        ASSERT(++numDeallocations == ta.numDeallocations());

        if (verbose) cout << "\nTesting 'createInplace' with 5 arguments"
                          << "\n----------------------------------------\n";

        numAllocations = ta.numAllocations();
        numDeallocations = ta.numDeallocations();
        {
            bcema_SharedPtr<MyInplaceTestObject> x;
            bcema_SharedPtr<MyInplaceTestObject> const& X=x;
            static const MyInplaceTestObject EXP(V1, V2, V3, V4, V5);

            x.createInplace(&ta, V1, V2, V3, V4, V5);

            ASSERT(++numAllocations == ta.numAllocations());
            ASSERT(X);
            ASSERT(EXP == *(X.ptr()));
        }
        ASSERT(++numDeallocations == ta.numDeallocations());

        if (verbose) cout << "\nTesting 'createInplace' with 6 arguments"
                          << "\n----------------------------------------\n";

        numAllocations = ta.numAllocations();
        numDeallocations = ta.numDeallocations();
        {
            bcema_SharedPtr<MyInplaceTestObject> x;
            bcema_SharedPtr<MyInplaceTestObject> const& X=x;
            static const MyInplaceTestObject EXP(V1, V2, V3, V4, V5, V6);

            x.createInplace(&ta, V1, V2, V3, V4, V5, V6);

            ASSERT(++numAllocations == ta.numAllocations());
            ASSERT(X);
            ASSERT(EXP == *(X.ptr()));
        }
        ASSERT(++numDeallocations == ta.numDeallocations());

        if (verbose) cout << "\nTesting 'createInplace' with 7 arguments"
                          << "\n----------------------------------------\n";

        numAllocations = ta.numAllocations();
        numDeallocations = ta.numDeallocations();
        {
            bcema_SharedPtr<MyInplaceTestObject> x;
            bcema_SharedPtr<MyInplaceTestObject> const& X=x;
            static const MyInplaceTestObject EXP(V1, V2, V3, V4, V5, V6, V7);

            x.createInplace(&ta, V1, V2, V3, V4, V5, V6,V7);

            ASSERT(++numAllocations == ta.numAllocations());
            ASSERT(X);
            ASSERT(EXP == *(X.ptr()));
        }
        ASSERT(++numDeallocations == ta.numDeallocations());

        if (verbose) cout << "\nTesting 'createInplace' with 9 arguments"
                          << "\n----------------------------------------\n";

        numAllocations = ta.numAllocations();
        numDeallocations = ta.numDeallocations();
        {
            bcema_SharedPtr<MyInplaceTestObject> x;
            bcema_SharedPtr<MyInplaceTestObject> const& X=x;
            static const MyInplaceTestObject EXP(V1, V2, V3, V4, V5, V6, V7,
                    V8);

            x.createInplace(&ta, V1, V2, V3, V4, V5, V6,V7, V8);

            ASSERT(++numAllocations == ta.numAllocations());
            ASSERT(X);
            ASSERT(EXP == *(X.ptr()));
        }
        ASSERT(++numDeallocations == ta.numDeallocations());

        if (verbose) cout << "\nTesting 'createInplace' with 10 arguments"
                          << "\n-----------------------------------------\n";

        numAllocations = ta.numAllocations();
        numDeallocations = ta.numDeallocations();
        {
            bcema_SharedPtr<MyInplaceTestObject> x;
            bcema_SharedPtr<MyInplaceTestObject> const& X=x;
            static const MyInplaceTestObject EXP(V1, V2, V3, V4, V5, V6, V7,
                    V8, V9);

            x.createInplace(&ta, V1, V2, V3, V4, V5, V6,V7, V8, V9);

            ASSERT(++numAllocations == ta.numAllocations());
            ASSERT(X);
            ASSERT(EXP == *(X.ptr()));
        }
        ASSERT(++numDeallocations == ta.numDeallocations());

        if (verbose) cout << "\nTesting 'createInplace' with 2 arguments"
                          << "\n----------------------------------------\n";

        numAllocations = ta.numAllocations();
        numDeallocations = ta.numDeallocations();
        {
            bcema_SharedPtr<MyInplaceTestObject> x;
            bcema_SharedPtr<MyInplaceTestObject> const& X=x;
            static const MyInplaceTestObject EXP(V1, V2, V3, V4, V5, V6, V7,
                    V8, V9, V10);

            x.createInplace(&ta, V1, V2, V3, V4, V5, V6,V7, V8, V9, V10);

            ASSERT(++numAllocations == ta.numAllocations());
            ASSERT(X);
            ASSERT(EXP == *(X.ptr()));
        }
        ASSERT(++numDeallocations == ta.numDeallocations());

        if (verbose) cout << "\nTesting 'createInplace' with 11 arguments"
                          << "\n----------------------------------------\n";

        numAllocations = ta.numAllocations();
        numDeallocations = ta.numDeallocations();
        {
            bcema_SharedPtr<MyInplaceTestObject> x;
            bcema_SharedPtr<MyInplaceTestObject> const& X=x;
            static const MyInplaceTestObject EXP(V1, V2, V3, V4, V5, V6, V7,
                    V8, V9, V10, V11);

            x.createInplace(&ta, V1, V2, V3, V4, V5, V6,V7, V8, V9, V10, V11);

            ASSERT(++numAllocations == ta.numAllocations());
            ASSERT(X);
            ASSERT(EXP == *(X.ptr()));
        }
        ASSERT(++numDeallocations == ta.numDeallocations());

        if (verbose) cout << "\nTesting 'createInplace' with 12 arguments"
                          << "\n-----------------------------------------\n";

        numAllocations = ta.numAllocations();
        numDeallocations = ta.numDeallocations();
        {
            bcema_SharedPtr<MyInplaceTestObject> x;
            bcema_SharedPtr<MyInplaceTestObject> const& X=x;
            static const MyInplaceTestObject EXP(V1, V2, V3, V4, V5, V6, V7,
                    V8, V9, V10, V11, V12);

            x.createInplace(&ta, V1, V2, V3, V4, V5, V6,V7, V8, V9, V10, V11,
                    V12);

            ASSERT(++numAllocations == ta.numAllocations());
            ASSERT(X);
            ASSERT(EXP == *(X.ptr()));
        }
        ASSERT(++numDeallocations == ta.numDeallocations());

        if (verbose) cout << "\nTesting 'createInplace' with 13 arguments"
                          << "\n-----------------------------------------\n";

        numAllocations = ta.numAllocations();
        numDeallocations = ta.numDeallocations();
        {
            bcema_SharedPtr<MyInplaceTestObject> x;
            bcema_SharedPtr<MyInplaceTestObject> const& X=x;
            static const MyInplaceTestObject EXP(V1, V2, V3, V4, V5, V6, V7,
                    V8, V9, V10, V11, V12, V13);

            x.createInplace(&ta, V1, V2, V3, V4, V5, V6,V7, V8, V9, V10,
                    V11, V12, V13);

            ASSERT(++numAllocations == ta.numAllocations());
            ASSERT(X);
            ASSERT(EXP == *(X.ptr()));
        }
        ASSERT(++numDeallocations == ta.numDeallocations());

        if (verbose) cout << "\nTesting 'createInplace' with 14 arguments"
                          << "\n-----------------------------------------\n";

        numAllocations = ta.numAllocations();
        numDeallocations = ta.numDeallocations();
        {
            bcema_SharedPtr<MyInplaceTestObject> x;
            bcema_SharedPtr<MyInplaceTestObject> const& X=x;
            static const MyInplaceTestObject EXP(V1, V2, V3, V4, V5, V6, V7,
                    V8, V9, V10, V11, V12, V13, V14);

            x.createInplace(&ta, V1, V2, V3, V4, V5, V6,V7, V8, V9, V10, V11,
                    V12, V13, V14);

            ASSERT(++numAllocations == ta.numAllocations());
            ASSERT(X);
            ASSERT(EXP == *(X.ptr()));
        }
        ASSERT(++numDeallocations == ta.numDeallocations());

        if (verbose)
            cout << "\nTesting 'createInplace' passing allocator to args"
                 << "\n-------------------------------------------------\n";

        bslma_TestAllocator ta0;
        bslma_TestAllocator ta1;
        bslma_TestAllocator ta2;

        bslma_TestAllocator *Z0 = &ta0;
        bslma_TestAllocator *Z1 = &ta1;
        bslma_TestAllocator *Z2 = &ta2;

        bdef_Bind_TestSlotsAlloc::setZ0(Z0);
        bdef_Bind_TestSlotsAlloc::setZ1(Z1);
        bdef_Bind_TestSlotsAlloc::setZ2(Z2);

        bslma_DefaultAllocatorGuard allocGuard(Z0);

        bdef_Bind_TestArgAlloc<1>  VA1(1, Z2);
        bdef_Bind_TestArgAlloc<2>  VA2(2, Z2);
        bdef_Bind_TestArgAlloc<3>  VA3(3, Z2);
        bdef_Bind_TestArgAlloc<4>  VA4(4, Z2);
        bdef_Bind_TestArgAlloc<5>  VA5(5, Z2);
        bdef_Bind_TestArgAlloc<6>  VA6(6, Z2);
        bdef_Bind_TestArgAlloc<7>  VA7(7, Z2);
        bdef_Bind_TestArgAlloc<8>  VA8(8, Z2);
        bdef_Bind_TestArgAlloc<9>  VA9(9, Z2);
        bdef_Bind_TestArgAlloc<10> VA10(10, Z2);
        bdef_Bind_TestArgAlloc<11> VA11(11, Z2);
        bdef_Bind_TestArgAlloc<12> VA12(12, Z2);
        bdef_Bind_TestArgAlloc<13> VA13(13, Z2);
        bdef_Bind_TestArgAlloc<14> VA14(14, Z2);

        bslma_Allocator *ALLOC_SLOTS[] = {
            //   1,  2   3   4   5   6   7   8   9  10  11  12  13  14
            Z0, Z1, Z1, Z1, Z1, Z1, Z1, Z1, Z1, Z1, Z1, Z1, Z1, Z1, Z1
        };

        {
            bcema_SharedPtr<bdef_Bind_TestTypeAlloc> x;
            bcema_SharedPtr<bdef_Bind_TestTypeAlloc> const& X=x;

            const bdef_Bind_TestTypeAlloc EXP(Z0, VA1, VA2, VA3, VA4,
                    VA5, VA6, VA7, VA8, VA9, VA10, VA11, VA12, VA13);

            x.createInplace(Z1, Z1, VA1, VA2, VA3, VA4, VA5, VA6,VA7, VA8, VA9,
                    VA10, VA11, VA12, VA13);

            bdef_Bind_TestSlotsAlloc::resetSlots(Z0);
            x->setSlots(); // should set slots to Z1, as this should be the
                           // allocator used by data members of *(X.ptr())

            ASSERT(X);
            ASSERT(EXP == *(X.ptr()));
            ASSERT(bdef_Bind_TestSlotsAlloc::verifySlots(ALLOC_SLOTS,
                                                         verbose));
        }

      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING ASSIGNMENT OPERATORS
        //
        // Plan: TBD
        //
        // Testing:
        //   bcema_SharedPtr<TYPE>& operator=(const bcema_SharedPtr<TYPE> &);
        //   bcema_SharedPtr<TYPE>& operator=(const bcema_SharedPtr<OTHER_TYPE>
        //   bcema_SharedPtr<TYPE>& operator=(bsl::auto_ptr<PTRTYPE> &rhs)
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "Testing ASSIGNMENT OPERATORS" << endl
                          << "============================" << endl;

        if (verbose) cout << "\nTesting ASSIGNMENT to empty object"
                          << "\n----------------------------------\n";
        {
            Obj x1;
            Obj const& X1 = x1;
            ASSERT(0 == x1.ptr());
            ASSERT(0 == x1.numReferences());

            numDeletes = 0;
            TObj *p = new TObj(&numDeletes);

            Obj x2(p); Obj const& X2=x2;

            if (veryVerbose) {
                P_(numDeletes); P_(X1.numReferences());
                P(X2.numReferences());
            }
            ASSERT(0 == numDeletes);
            ASSERT(p == X2.ptr());
            ASSERT(1 == X2.numReferences());

            x1 = X2;

            if (veryVerbose) {
                P_(numDeletes); P_(X1.numReferences());
                P(X2.numReferences());
            }
            ASSERT(0 == numDeletes);
            ASSERT(p == X2.ptr());
            ASSERT(p == X1.ptr());

            ASSERT(2 == X2.numReferences());
            ASSERT(2 == X1.numReferences());
        }
        ASSERT(1 == numDeletes);

        if (verbose) cout << "\nTesting ASSIGNMENT of auto_ptr"
                          << "\n------------------------------\n";
        {
            Obj x;
            Obj const& X = x;
            ASSERT(0 == x.ptr());
            ASSERT(0 == x.numReferences());

            numDeletes = 0;
            TObj *p = new TObj(&numDeletes);
            bsl::auto_ptr<TObj> ap(p);

            x = ap;

            if (veryVerbose) {
                P_(numDeletes);        P_(X.numReferences());
                P(ap.get());
            }

            ASSERT(0 == ap.get());
            ASSERT(0 == numDeletes);
            ASSERT(p == X.ptr());
            ASSERT(1 == X.numReferences());
        }
        ASSERT(1 == numDeletes);

        if (verbose) cout << "\nTesting ASSIGNMENT of empty object"
                          << "\n----------------------------------\n";
        {
            numDeletes = 0;
            TObj *p = new TObj(&numDeletes);

            Obj x1(p); Obj const& X1=x1;

            if (veryVerbose) {
                P_(numDeletes); P_(X1.numReferences());
            }
            ASSERT(0 == numDeletes);
            ASSERT(p == X1.ptr());
            ASSERT(1 == X1.numReferences());

            Obj x2;
            Obj const& X2 = x2;
            ASSERT(0 == x2.ptr());
            ASSERT(0 == x2.numReferences());

            x1 = X2;

            if (veryVerbose) {
                P_(numDeletes); P_(X1.numReferences());
                P(X2.numReferences());
            }
            ASSERT(1 == numDeletes);
            ASSERT(0 == X2.ptr());
            ASSERT(0 == X1.ptr());

            ASSERT(0 == X2.numReferences());
            ASSERT(0 == X1.numReferences());
        }
        ASSERT(1 == numDeletes);

        if (verbose) cout << "\nTesting ASSIGNMENT of loaded object"
                          << "\n----------------------------------\n";
        {
            int numDeletes1 = 0;
            numDeletes = 0;
            TObj *p1 = new TObj(&numDeletes1);

            Obj x1(p1); Obj const& X1=x1;

            if (veryVerbose) {
                P_(numDeletes1); P_(X1.numReferences());
            }
            ASSERT(0 == numDeletes1);
            ASSERT(p1 == X1.ptr());
            ASSERT(1 == X1.numReferences());

            numDeletes = 0;
            TObj *p2 = new TObj(&numDeletes);
            Obj x2(p2);
            Obj const& X2 = x2;

            ASSERT(0 == numDeletes);
            ASSERT(p2 == x2.ptr());
            ASSERT(1 == x2.numReferences());

            x1 = X2;

            if (veryVerbose) {
                P_(numDeletes1); P_(numDeletes); P_(X1.numReferences());
                P(X2.numReferences());
            }

            ASSERT(1 == numDeletes1);
            ASSERT(0 == numDeletes);
            ASSERT(p2 == X2.ptr());
            ASSERT(p2 == X1.ptr());

            ASSERT(2 == X2.numReferences());
            ASSERT(2 == X1.numReferences());
        }
        ASSERT(1 == numDeletes);

        if (verbose) cout << "\nTesting ASSIGNMENT to self"
                          << "\n--------------------------\n";
        {
            numDeletes = 0;
            TObj *p = new TObj(&numDeletes);

            Obj x1(p); Obj const& X1=x1;

            x1 = X1;

            if (veryVerbose) {
                P_(numDeletes); P_(X1.numReferences());
            }
            ASSERT(0 == numDeletes);
            ASSERT(p == X1.ptr());
            ASSERT(1 == X1.numReferences());
        }

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING CONSTRUCTORS
        //   Verify that upon construction the object is properly initialized,
        //   and the reference count is updated.  Check that memory is
        //   deallocated upon object destruction.  Check that no memory is
        //   leaked in the presence of exceptions.
        //
        // Plan: TBD
        //
        // Testing:
        //   bcema_SharedPtr(PTRTYPE *ptr, bslma_Allocator *allocator=0)
        //   bcema_SharedPtr(PTRTYPE *ptr, const DELETER &deleter, ...
        //   bcema_SharedPtr(bsl::auto_ptr<PTRTYPE> &autoPtr, ...
        //   bcema_SharedPtr(bcema_SharedPtr<TYPE> const& original);
        //   bcema_SharedPtr(bcema_SharedPtrRep *);
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "Testing Constructors and Destructor" << endl
                          << "===================================" << endl;

        if (verbose)
            cout << "\nTesting null ptr constructor"
                 << "\n----------------------------\n";

        numDeallocations = ta.numDeallocations();
        {

            Obj w((TObj*)0); Obj const& W=w;
            ASSERT(0 == W.ptr());
            ASSERT(0 == W.numReferences());

            numAllocations = ta.numAllocations();

            Obj x((TObj*)0, &ta); Obj const&X=x;
            ASSERT(0 == X.ptr());
            ASSERT(0 == X.numReferences());
            ASSERT(numAllocations == ta.numAllocations());

            bsl::auto_ptr<TObj> apY((TObj*)0);
            Obj y(apY, &ta); Obj const&Y=y;
            ASSERT(0 == Y.ptr());
            ASSERT(0 == Y.numReferences());
            ASSERT(numAllocations == ta.numAllocations());

            Obj z((TObj*)0, &ta, &ta); Obj const&Z=z;
            ASSERT(0 == Z.ptr());
            ASSERT(0 == Z.numReferences());
            ASSERT(numAllocations == ta.numAllocations());
        }
        ASSERT(numDeallocations == ta.numDeallocations());

        if (verbose)
            cout << "\nTesting constructor (with factory)"
                 << "\n---------------------------------\n";

        numDeallocations = ta.numDeallocations();
        {
            numDeletes = 0;
            TObj *p = new (ta) TObj(&numDeletes);
            numAllocations = ta.numAllocations();

            Obj x(p, &ta); Obj const& X=x;
            ASSERT(++numAllocations == ta.numAllocations());

            if (veryVerbose) {
                P_(numDeletes); P(X.numReferences());
            }
            ASSERT(0 == numDeletes);
            ASSERT(p == X.ptr());
            ASSERT(1 == X.numReferences());
        }
        if (veryVerbose) {
            P_(numDeletes); P_(numDeallocations); P(ta.numDeallocations());
        }
        ASSERT(1 == numDeletes);
        ASSERT((numDeallocations+2) == ta.numDeallocations());

        if (verbose)
            cout << "\nTesting auto_ptr constructor (with allocator)"
                 << "\n--------------------------------------------\n";

        numDeallocations = ta.numDeallocations();
        {
            numDeletes = 0;
            TObj  *p = new TObj(&numDeletes);
            bsl::auto_ptr<TObj> ap(p);

            numAllocations = ta.numAllocations();

            Obj x(ap, &ta); Obj const& X=x;
            ASSERT(0 == ap.get());
            ASSERT(++numAllocations == ta.numAllocations());

            if (veryVerbose) {
                P_(numDeletes); P(X.numReferences());
            }
            ASSERT(0 == numDeletes);
            ASSERT(p == X.ptr());
            ASSERT(1 == X.numReferences());
        }
        if (veryVerbose) {
            P_(numDeletes); P_(numDeallocations); P(ta.numDeallocations());
        }
        ASSERT(1 == numDeletes);
        ASSERT((++numDeallocations) == ta.numDeallocations());

        if (verbose)
            cout << "\nTesting constructor (with factory and allocator)"
                 << "\n-----------------------------------------------\n";

        numDeallocations = ta.numDeallocations();
        {
            numDeletes = 0;
            TObj *p = new (ta) TObj(&numDeletes);
            numAllocations = ta.numAllocations();

            Obj x(p, &ta, &ta); Obj const& X=x;
            ASSERT(++numAllocations == ta.numAllocations());

            if (veryVerbose) {
                P_(numDeletes); P(X.numReferences());
            }
            ASSERT(0 == numDeletes);
            ASSERT(p == X.ptr());
            ASSERT(1 == X.numReferences());
        }
        if (veryVerbose) {
            P_(numDeletes); P_(numDeallocations); P(ta.numDeallocations());
        }
        ASSERT(1 == numDeletes);
        ASSERT(numDeallocations + 2 == ta.numDeallocations());

        if (verbose)
            cout << "\nTesting constructor(with deleter)"
                 << "\n---------------------------------\n";

        numDeallocations = ta.numDeallocations();
        {
            numDeletes = 0;
            TObj *p = new (ta) TObj(&numDeletes);
            numAllocations = ta.numAllocations();

            MyTestDeleter deleter(&ta);
            Obj x(p, deleter, 0);  Obj const& X=x;
            ASSERT(numAllocations == ta.numAllocations());

            if (veryVerbose) {
                P_(numDeletes); P(X.numReferences());
            }
            ASSERT(0 == numDeletes);
            ASSERT(p == X.ptr());
            ASSERT(1 == X.numReferences());
        }
        if (veryVerbose) {
            P_(numDeletes); P_(numDeallocations); P(ta.numDeallocations());
        }
        ASSERT(1 == numDeletes);
        ASSERT(numDeallocations + 1 == ta.numDeallocations());

        if (verbose)
            cout << "\nTesting constructor(with deleter and allocator)"
                 << "\n-----------------------------------------------\n";

        numDeallocations = ta.numDeallocations();
        {
            numDeletes = 0;
            TObj *p = new (ta) TObj(&numDeletes);
            numAllocations = ta.numAllocations();

            MyTestDeleter deleter(&ta);
            Obj x(p, deleter, &ta); Obj const& X=x;
            ASSERT(++numAllocations == ta.numAllocations());

            if (veryVerbose) {
                P_(numDeletes); P(X.numReferences());
            }
            ASSERT(0 == numDeletes);
            ASSERT(p == X.ptr());
            ASSERT(1 == X.numReferences());
        }
        if (veryVerbose) {
            P_(numDeletes); P_(numDeallocations); P(ta.numDeallocations());
        }
        ASSERT(1 == numDeletes);
        ASSERT((numDeallocations+2) == ta.numDeallocations());

        if (verbose)
            cout << "\nTesting constructor(with rep)"
                 << "\n-----------------------------\n";

        numDeallocations = ta.numDeallocations();
        {
            numDeletes = 0;
            TObj *p = new (ta) TObj(&numDeletes);
            numAllocations = ta.numAllocations();

            Obj x(p, &ta); Obj const& X=x;
            ASSERT(++numAllocations == ta.numAllocations());

            if (veryVerbose) {
                P(X.numReferences());
            }
            ASSERT(0 == numDeletes);
            ASSERT(p == X.ptr());
            ASSERT(1 == X.numReferences());

            bcema_SharedPtrRep *rep = x.rep();
            x.release();

            Obj xx(rep); const Obj& XX = xx;
            ASSERT(p == XX.ptr());
            ASSERT(rep ==  XX.rep());
            ASSERT(1 == XX.numReferences());
        }
        if (veryVerbose) {
            P_(numDeletes); P_(numDeallocations); P(ta.numDeallocations());
        }
        ASSERT((numDeallocations+2) == ta.numDeallocations());

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING BASIC CONSTRUCTORS AND ACCESSORS
        //   Verify that upon construction the object is properly initialized,
        //
        // Testing:
        //   bcema_SharedPtr();
        //   bcema_SharedPtr(TYPE *ptr, bslma_Allocator *allocator=0);
        //   operator bool() const;
        //   typename bcema_SharedPtr<TYPE>::Reference operator[]() const;
        //   typename bcema_SharedPtr<TYPE>::Reference operator*() const;
        //   TYPE *operator->() const;
        //   TYPE *ptr() const;
        //   bcema_SharedPtrRep *rep() const;
        //   int numReferences() const;
        //   TYPE *get() const;
        //   bool unique() const;
        //   int use_count() const;
        //   void clear();
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing Constructors and Destructor" << endl
                          << "===================================" << endl;

        if (verbose) cout << endl
                          << "Testing default constructor" << endl
                          << "---------------------------" << endl;
        {
            Obj x; Obj const& X=x;

            ASSERT(0 == X.ptr());
            ASSERT(0 == X.get());
            ASSERT(0 == X.rep());
            ASSERT(0 == X.numReferences());
            ASSERT(0 == X.use_count());
            ASSERT(false == X.unique());
            ASSERT(false == X);
        }

        if (verbose) cout << endl
                          << "Testing basic constructor" << endl
                          << "-------------------------" << endl;
        {
            numDeletes = 0;
            TObj *p = new TObj(&numDeletes);

            Obj x(p); Obj const& X=x;

            if (veryVerbose) {
                P_(numDeletes); P(X.numReferences());
            }
            ASSERT(0 == numDeletes);
            ASSERT(p == X.ptr());
            ASSERT(0 != X.rep());
            ASSERT(1 == X.rep()->numReferences());
            ASSERT((void *) p == X.rep()->originalPtr());
            ASSERT(p == X.get());
            ASSERT(1 == X.numReferences());
            ASSERT(1 == X.use_count());
            ASSERT(true == X.unique());
            ASSERT(false != X);
            ASSERT(p == X.operator->());
            ASSERT(p == &X.operator*());
            ASSERT(p == &X.operator[](0));
        }

        if (verbose) cout << endl
                          << "Testing 'clear' " << endl
                          << "----------------" << endl;

        numDeallocations = ta.numDeallocations();
        {
            numDeletes = 0;
            TObj *p = new(ta) TObj(&numDeletes);

            Obj x(p,&ta); Obj const& X=x;
            numAllocations = ta.numAllocations();

            if (veryVerbose) {
                P_(numDeletes); P(X.numReferences());
            }

            ASSERT(0 == numDeletes);
            ASSERT(p == X.ptr());
            ASSERT(0 != X.rep());
            ASSERT(1 == X.rep()->numReferences());
            ASSERT((void *) p == X.rep()->originalPtr());
            ASSERT(1 == X.numReferences());

            x.clear();

            numDeallocations += 2;
            ASSERT(numDeallocations == ta.numDeallocations());
            ASSERT(1 == numDeletes);
            ASSERT(0 == X.ptr());
            ASSERT(0 == X.rep());
            ASSERT(0 == X.numReferences());
        }

        ASSERT(1 == numDeletes);
        ASSERT(numDeallocations == ta.numDeallocations());

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

        int numDeletes = 0;
        {
            MyTestObject *obj = new MyTestObject(&numDeletes);
            ASSERT(0 == numDeletes);

            Obj x(obj); Obj const& X=x;
            ASSERT(0 == numDeletes);
            ASSERT(obj == X.ptr());
            ASSERT(obj == X.get());
            ASSERT(X);
        }
        ASSERT(1 == numDeletes);
        if (veryVerbose) { P(numDeletes); }

        numDeletes = 0;
        {
            MyTestObject *obj = new MyTestObject(&numDeletes);
            MyTestObjectFactory deleter;
            ASSERT(0 == numDeletes);

            Obj x(obj,&deleter,0);
            Obj const& X=x;

            ASSERT(0 == numDeletes);
            ASSERT(obj == X.ptr());
            ASSERT(obj == X.get());
            ASSERT(X);
        }
        ASSERT(1 == numDeletes);

        numDeletes = 0;
        {
            MyTestObject *obj = new MyTestObject(&numDeletes);
            MyTestDeleter deleter;
            ASSERT(0 == numDeletes);

            Obj x(obj, deleter, 0); Obj const& X=x;
            ASSERT(0 == numDeletes);
            ASSERT(obj == X.ptr());
            ASSERT(obj == X.get());
            ASSERT(X);

        }
        ASSERT(1 == numDeletes);

        numDeletes = 0;
        {
            MyTestObject *obj = new MyTestObject(&numDeletes);

            Obj x1(obj); Obj const& X1=x1;
            Obj x2; Obj const& X2=x2;
            ASSERT(0 == numDeletes);
            ASSERT(obj == X1.ptr());
            ASSERT(obj == X1.get());
            ASSERT(X1);
        }
        ASSERT(1 == numDeletes);

        numDeletes = 0;
        {
            ConstObj x1(new MyTestObject(&numDeletes));
            ConstObj const& X1=x1;

            Obj x2(bcema_SharedPtrUtil::constCast<TObj>(x1)); Obj const& X2=x2;
            if (veryVeryVerbose) {
                P(numDeletes);
            }
            ASSERT(0 == numDeletes);
            ASSERT(X1.ptr() == X2.ptr());
            if (veryVeryVerbose) {
                P(numDeletes);
            }

        }
        if (veryVeryVerbose) {
            P(numDeletes);
        }
        ASSERT(1 == numDeletes);

        numDeletes = 0;
        {
            TObj obj(&numDeletes);

            Obj x1; const Obj &X1 = x1;

            ASSERT(0 == X1.ptr());
            x1.createInplace(0, obj);

            ASSERT(0 != X1.ptr());
            ASSERT(0 == numDeletes);
            ASSERT(0 == X1->copyCounter());
            ASSERT(&numDeletes == X1->deleteCounter());
        }
        ASSERT(2 == numDeletes);

        numDeletes = 0;
        {
            TObj *p = new TObj(&numDeletes);

            Obj x1(p, &myTestFunctor, (bslma_Allocator*)0); const Obj &X1 = x1;

        }
        ASSERT(1 == numDeletes);
      } break;
      case -2: {

        cout << endl
             << "PERFORMANCE TEST: Old Implementation" << endl
             << "====================================" << endl;

        PerformanceTester<OldObj>::test(verbose, veryVeryVerbose);

      } break;
      case -1: {
        // --------------------------------------------------------------------
        // PERFORMANCE TEST
        //
        // Concerns:  We should have a benchmark to time performance and keep
        //   track of performance improvements or losses as the code evolves.
        //   Performance here means both runtime and memory usage.
        //
        // Test plan: First measure the basics:  1. time to construct and
        //   destroy a shared pointer (executed in a loop), for various kinds
        //   of representations, and for various kinds of creation (allocating
        //   a new representation or simply incrementing a reference count); 2.
        //   time to assign copy shared pointers (measured by shuffling a
        //   vector of shared pointers).  Note that the type that is pointed to
        //   matters in (1), for in-place construction, but what matters is the
        //   base line, not the absolute timings; the type that is pointed to
        //   does *not* matter in (2), since this only measures the time to
        //   increment/decrement the shared pointers.  We measure the baselines
        //   (creation, copying and destruction of the test objects) so that
        //   they can be deducted from the shared pointers runtimes (e.g., when
        //   constructing inplace).
        //       Then we measure the times in various more specialized
        //   scenarios such as releasing in-place representations into a pool.
        //
        // Testing:
        //   PERFORMANCE
        // --------------------------------------------------------------------

        cout << endl
             << "PERFORMANCE TEST" << endl
             << "================" << endl;

        PerformanceTester<Obj>::test(verbose, veryVeryVerbose);

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
