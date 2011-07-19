// bdema_managedptr.t.cpp          -*-C++-*-

#include <bdema_managedptr.h>
#include <bslma_allocator.h>                    // for testing only
#include <bslma_defaultallocatorguard.h>        // for testing only
#include <bslma_testallocator.h>                // for testing only
#include <bslmf_assert.h>
#include <bslmf_issame.h>

#include <bsl_cstdlib.h>     // atoi()
#include <bsl_cstring.h>     // memcpy()
#include <bsl_iostream.h>

//#define TEST_FOR_COMPILE_ERRORS

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

//=============================================================================
//                             TEST PLAN
//                             ---------
// The 'bdema_managedptr' component provides a small number of classes that
// combine to provide a common solution to the problem of managing and
// transferring ownership of a dynamically allocated object.  We choose to test
// each class in turn, according to their
//
// [ 2] imp. class bdema_ManagedPtr_Members
// [ 3] imp. class bdema_ManagedPtr_Ref       (this one needs negative testing)
// [ 4] imp. class bdema_ManagedPtr_FactoryDeleter  (this one needs negative testing)
// [ 5] class bdema_ManagedPtrNilDeleter
//      class bdema_ManagedPtr

//-----------------------------------------------------------------------------
//                             Overview
//                             --------
// We are testing a proctor class that makes sure that only one instance holds
// a copy of an allocated pointer, along with the necessary information to
// deallocate it properly (the deleter).  The primary goal of this test program
// is to ascertain that no resource ever gets leaked, i.e., that when the
// proctor is re-assigned or destroyed, the managed pointer gets deleted
// properly.  In addition, we must also make sure that all the conversion and
// aliasing machinery works as documented.  At last, we must also check that
// a 'bdema_ManagedPtr' acts exactly as a pointer wherever one is expected.
//-----------------------------------------------------------------------------
// [ 6] bdema_ManagedPtr();
// [ 6] bdema_ManagedPtr(nullptr_t);
// [ 6] template<class TARGET_TYPE> bdema_ManagedPtr(TARGET_TYPE *ptr);
// [ 8] bdema_ManagedPtr(bdema_ManagedPtr_Ref<BDEMA_TYPE> ref);
// [ 8] bdema_ManagedPtr(bdema_ManagedPtr& original);
// [ 8] bdema_ManagedPtr(bdema_ManagedPtr<OTHER> &original)
// [ 9] bdema_ManagedPtr(bdema_ManagedPtr<OTHER> &alias, TYPE *ptr)
// [ 8] bdema_ManagedPtr(TYPE *ptr, FACTORY *factory)
// [ 8] bdema_ManagedPtr(TYPE *ptr, void *factory,void(*deleter)(TYPE*, void*))
// [ 6] ~bdema_ManagedPtr();
// [11] operator bdema_ManagedPtr_Ref<BDEMA_TYPE>();
// [11] operator bdema_ManagedPtr_Ref<OTHER>();
// [ 7] void load(nullptr_t=0);
// [ 7] template<class TARGET_TYPE> void load(TARGET_TYPE *ptr);
// [ 7] void load(TYPE *ptr, FACTORY *factory)
// [ 7] void load(TYPE *ptr, void *factory, void (*deleter)(TYPE *, void*));
// [ 7] void load(TYPE *ptr, FACTORY *factory, void(*deleter)(TYPE *,FACTORY*))
// [ 9] void loadAlias(bdema_ManagedPtr<OTHER> &alias, TYPE *ptr)
// [11] void swap(bdema_ManagedPt& rhs);
// [11] bdema_ManagedPtr& operator=(bdema_ManagedPtr &rhs);
// [11] bdema_ManagedPtr& operator=(bdema_ManagedPtr<OTHER> &rhs)
// [11] bdema_ManagedPtr& operator=(bdema_ManagedPtr_Ref<BDEMA_TYPE> ref);
// [12] void clear();
// [12] bsl::pair<TYPE*,bdema_ManagedPtrDeleter> release();
// [10] operator BoolType() const;
// [10] TYPE& operator*() const;
// [10] TYPE *operator->() const;
// [10] TYPE *ptr() const;
// [10] const bdema_ManagedPtrDeleter& deleter() const;
//
// [ 5] class bdema_ManagedPtrNilDeleter
//
// [ 2] imp. class bdema_ManagedPtr_Members
// [ 3] imp. class bdema_ManagedPtr_Ref             (this one needs negative testing)
// [ 4] imp. class bdema_ManagedPtr_FactoryDeleter  (this one needs negative testing)
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [13] CASTING EXAMPLE
// [14] USAGE EXAMPLE
// [15] VERIFYING FAILURES TO COMPILE

namespace {

//=============================================================================
//                    STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
int testStatus = 0;

void aSsErT(int c, const char *s, int i) {
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}
# define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

//=============================================================================
//                  STANDARD BDE LOOP-ASSERT TEST MACROS
//-----------------------------------------------------------------------------

#define LOOP_ASSERT(I,X) { \
    if (!(X)) { cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__);}}

#define LOOP2_ASSERT(I,J,X) { \
    if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " \
              << J << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" \
              << #K << ": " << K << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP4_ASSERT(I,J,K,L,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" << \
       #K << ": " << K << "\t" << #L << ": " << L << "\n"; \
       aSsErT(1, #X, __LINE__); } }

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------

#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", " << flush; // P(X) without '\n'
#define PA(X, L) cout << #X " = "; printArray(X, L); cout << endl;
                                              // Print array 'X' of length 'L'
#define PA_(X, L) cout << #X " = "; printArray(X, L); cout << ", " << flush;
                                              // PA(X, L) without '\n'
#define L_ __LINE__                           // current Line number

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

int verbose = 0;
int veryVerbose = 0;

//bdema_ManagedPtr<int> i;

class MyTestObject;
class MyDerivedObject;
class MySecondDerivedObject;
typedef MyTestObject TObj;
typedef bdema_ManagedPtr<MyTestObject> Obj;
typedef bdema_ManagedPtr<const MyTestObject> CObj;
typedef MyDerivedObject TDObj;
typedef bdema_ManagedPtr<MyDerivedObject> DObj;
typedef bdema_ManagedPtr<const MyDerivedObject> CDObj;
typedef MySecondDerivedObject TSObj;
typedef bdema_ManagedPtr<MySecondDerivedObject> SObj;
typedef bdema_ManagedPtr<const MySecondDerivedObject> CSObj;
typedef bdema_ManagedPtr<void> VObj;

//=============================================================================
//                         HELPER CLASSES FOR TESTING
//-----------------------------------------------------------------------------

class MyTestObject {
    volatile int *d_deleteCounter_p;
    int           d_value[2];

  public:
    MyTestObject(int *counter);

    // Use compiler-generated copy constructor and assignment operator
    // MyTestObject(MyTestObject const& orig);
    // MyTestObject operator=(MyTestObject const& orig);

    virtual ~MyTestObject();

    int *valuePtr(int index = 0);

    volatile int *deleteCounter() const;
};

MyTestObject::MyTestObject(int *counter)
: d_deleteCounter_p(counter)
, d_value()
{
//    for (unsigned i = 0; i < sizeof(d_value) / sizeof(d_value[0]); ++i) {
//        d_value[i] = 0;
//    }
}

MyTestObject::~MyTestObject()
{
    ++(*d_deleteCounter_p);
}

inline
int *MyTestObject::valuePtr(int index)
{
    return d_value + index;
}

volatile int* MyTestObject::deleteCounter() const
{
    return d_deleteCounter_p;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

class MyDerivedObject : public MyTestObject
{
  public:
    MyDerivedObject(int *counter);
    // Use compiler-generated copy and destruction
};

inline
MyDerivedObject::MyDerivedObject(int *counter)
: MyTestObject(counter)
{
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

class MySecondDerivedObject : public MyTestObject
{
  public:
    MySecondDerivedObject(int *counter);
    // Use compiler-generated copy and destruction
};

inline
MySecondDerivedObject::MySecondDerivedObject(int *counter)
: MyTestObject(counter)
{
}

//=============================================================================
//                    FILE-STATIC FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

void myTestDeleter(TObj *object, bslma_TestAllocator *allocator)
{
    allocator->deleteObject(object);
    if (verbose) {
        bsl::cout << "myTestDeleter called" << endl;
    }
}

bdema_ManagedPtr<MyTestObject>
returnManagedPtr(int *numDels, bslma_TestAllocator *allocator)
{
    MyTestObject *p = new (*allocator) MyTestObject(numDels);
    bdema_ManagedPtr<MyTestObject> ret(p, allocator);
    return ret;
}

bdema_ManagedPtr<MyDerivedObject>
returnDerivedPtr(int *numDels, bslma_TestAllocator *allocator)
{
    MyDerivedObject *p = new (*allocator) MyDerivedObject(numDels);
    bdema_ManagedPtr<MyDerivedObject> ret(p, allocator);
    return ret;
}

bdema_ManagedPtr<MySecondDerivedObject>
returnSecondDerivedPtr(int *numDels, bslma_TestAllocator *allocator)
{
    MySecondDerivedObject *p = new (*allocator) MySecondDerivedObject(numDels);
    bdema_ManagedPtr<MySecondDerivedObject> ret(p, allocator);
    return ret;
}

}  // close unnamed namespace

//=============================================================================
//                                CASTING EXAMPLE
//-----------------------------------------------------------------------------
namespace TYPE_CASTING_TEST_NAMESPACE {

    typedef MyTestObject A;
    typedef MyDerivedObject B;

///Type Casting
///------------
// 'bdema_ManagedPtr' objects can be implicitly and explicitly cast to
// different types in the same way as native pointers can.
//
///Implicit Casting
/// - - - - - - - -
// As with native pointers, a pointer of the type 'B' that is derived from the
// type 'A', can be directly assigned a 'bcema_SharedPtr' of 'A'.
// In other words, consider the following code snippets:
//..
    void implicitCastingExample() {
//..
// If the statements:
//..
        bslma_TestAllocator localDefaultTa;
        bslma_TestAllocator localTa;

        bslma_DefaultAllocatorGuard guard(&localDefaultTa);

        int numdels = 0;

        {
//            B *b_p = 0;
//            A *a_p = b_p;
    //..
    // are legal expressions, then the statements
    //..
            bdema_ManagedPtr<A> a_mp1;
            bdema_ManagedPtr<B> b_mp1;

            ASSERT(!a_mp1 && !b_mp1);

            a_mp1 = b_mp1;      // conversion assignment of nil ptr to nil
            ASSERT(!a_mp1 && !b_mp1);

            B *b_p2 = new (localDefaultTa) B(&numdels);
            bdema_ManagedPtr<B> b_mp2(b_p2);    // default allocator
            ASSERT(!a_mp1 && b_mp2);

            a_mp1 = b_mp2;      // conversion assignment of nonnil ptr to nil
            ASSERT(a_mp1 && !b_mp2);

            B *b_p3 = new (localTa) B(&numdels);
            bdema_ManagedPtr<B> b_mp3(b_p3, &localTa);
            ASSERT(a_mp1 && b_mp3);

            a_mp1 = b_mp3;      // conversion assignment of nonnil to nonnil
            ASSERT(a_mp1 && !b_mp3);

            a_mp1 = b_mp3;  // conversion assignment of nil to nonnil
            ASSERT(!a_mp1 && !b_mp3);

            // c'tor conversion init with nil
            bdema_ManagedPtr<A> a_mp4(b_mp3, b_mp3.ptr());
            ASSERT(!a_mp4 && !b_mp3);

            // c'tor conversion init with nonnil
            B *p_b5 = new (localTa) B(&numdels);
            bdema_ManagedPtr<B> b_mp5(p_b5, &localTa);
            bdema_ManagedPtr<A> a_mp5(b_mp5, b_mp5.ptr());
            ASSERT(a_mp5 && !b_mp5);
            ASSERT(a_mp5.ptr() == p_b5);

            // c'tor conversion init with nonnil
            B *p_b6 = new (localTa) B(&numdels);
            bdema_ManagedPtr<B> b_mp6(p_b6, &localTa);
            bdema_ManagedPtr<A> a_mp6(b_mp6);
            ASSERT(a_mp6 && !b_mp6);
            ASSERT(a_mp6.ptr() == p_b6);

            struct S {
                int d_i[10];
            };

#if 0
            S *pS = new (localTa) S;
            for (int i = 0; 10 > i; ++i) {
                pS->d_i[i] = i;
            }

            bdema_ManagedPtr<S> s_mp1(pS);
            bdema_ManagedPtr<int> i_mp1(pS, static_cast<int*>(pS.ptr()) + 4);
            ASSERT(4 == *i_mp1);
#endif

            ASSERT(2 == numdels);
        }

        ASSERT(4 == numdels);
    } // implicitCastingExample()
//..
//
///Explicit Casting
/// - - - - - - - -
// Through "aliasing", a managed pointer of any type can be explicitly cast
// to a managed pointer of any other type using any legal cast expression.
// For example, to static-cast a managed pointer of type A to a shared pointer
// of type B, one can simply do the following:
//..
    void explicitCastingExample() {

        bdema_ManagedPtr<A> a_mp;
        bdema_ManagedPtr<B> b_mp1(a_mp, static_cast<B*>(a_mp.ptr()));
        //..
        // or even use the less safe "C"-style casts:
        //..
        // bdema_ManagedPtr<A> a_mp;
        bdema_ManagedPtr<B> b_mp2(a_mp, (B*)(a_mp.ptr()));

    } // explicitCastingExample()
//..
// Note that when using dynamic cast, if the cast fails, the target managed
// pointer will be reset to an unset state, and the source will not be
// modified.  Consider for example the following snippet of code:
//..
    void processPolymorphicObject(bdema_ManagedPtr<A> aPtr,
                                  bool *castSucceeded)
    {
        bdema_ManagedPtr<B> bPtr(aPtr, dynamic_cast<B*>(aPtr.ptr()));
        if (bPtr) {
            ASSERT(!aPtr);
            *castSucceeded = true;
        }
        else {
            ASSERT(aPtr);
            *castSucceeded = false;
        }
    }
//..
// If the value of 'aPtr' can be dynamically cast to 'B*' then ownership is
// transferred to 'bPtr', otherwise 'aPtr' is to be modified.  As previously
// stated, the managed instance will be destroyed correctly regardless of how
// it is cast.

} // namespace TYPE_CASTING_TEST_NAMESPACE

//=============================================================================
//                              CREATORS TEST
//=============================================================================

namespace CREATORS_TEST_NAMESPACE {

struct SS {
    char  d_buf[100];
    int  *d_numDeletes_p;

    SS(int *numDeletes) {
        d_numDeletes_p = numDeletes;
    }
    ~SS() {
        ++*d_numDeletes_p;
    }
};

typedef bdema_ManagedPtr<SS> SSObj;
typedef bdema_ManagedPtr<char> ChObj;

}  // close namespace CREATORS_TEST_NAMESPACE

//=============================================================================
//                                USAGE EXAMPLE
//-----------------------------------------------------------------------------
namespace USAGE_EXAMPLE {

// What follows is a concrete example illustrating the alias concept.
// Let's say our array stores data acquired from a ticker
// plant accessible by a global 'getQuote()' function:
//..
    double getQuote() // From ticker plant. Simulated here
    {
        static const double QUOTES[] = {
            7.25, 12.25, 11.40, 12.00, 15.50, 16.25, 18.75, 20.25, 19.25, 21.00
        };
        static const int NUM_QUOTES = sizeof(QUOTES) / sizeof(QUOTES[0]);
        static int index = 0;

        double ret = QUOTES[index];
        index = (index + 1) % NUM_QUOTES;
        return ret;
    }
//..
// We now want to find the first quote larger than a specified threshold, but
// would also like to keep the earlier and later quotes for possible
// examination.  Our 'getFirstQuoteLargerThan' function must allocate memory
// for an array of quotes (the threshold and its neighbors).  It thus returns
// a managed pointer to the desired value:
//..
    const double END_QUOTE = -1;

    bdema_ManagedPtr<double>
    getFirstQuoteLargerThan(double threshold, bslma_Allocator *allocator)
    {
        ASSERT( END_QUOTE < 0 && 0 <= threshold );
//..
// We allocate our array with extra room to mark the beginning and end with a
// special 'END_QUOTE' value:
//..
        const int MAX_QUOTES = 100;
        int numBytes = (MAX_QUOTES + 2) * sizeof(double);
        double *quotes = (double*) allocator->allocate(numBytes);
        quotes[0] = quotes[MAX_QUOTES + 1] = END_QUOTE;
//..
// Then we read quotes until the array is full, keeping track of the first
// quote that exceeds the threshold.
//..
        double *finger = 0;

        for (int i = 1; i <= MAX_QUOTES; ++i) {
            double quote = getQuote();
            quotes[i] = quote;
            if (! finger && quote > threshold) {
                finger = &quotes[i];
            }
        }
//..
// Before we return, we create a managed pointer to the entire array:
//..
        bdema_ManagedPtr<double> managedQuotes(quotes, allocator);
//..
// Then we use the alias constructor to create a managed pointer that points
// to the desired value (the finger) but manages the entire array:
//..
        return bdema_ManagedPtr<double>(managedQuotes, finger);
    }
//..
// Our main program calls 'getFirstQuoteLargerThan' like this:
//..
    int usageExample1()
    {
        bslma_TestAllocator ta;
        bdema_ManagedPtr<double> result = getFirstQuoteLargerThan(16.00, &ta);
        ASSERT(*result > 16.00);
        ASSERT(1 == ta.numBlocksInUse());
        if (verbose) bsl::cout << "Found quote: " << *result << bsl::endl;
//..
// We also print the preceding 5 quotes in last-to-first order:
//..
        int i;
        if (verbose) bsl::cout << "Preceded by:";
        for (i = -1; i >= -5; --i) {
            double quote = result.ptr()[i];
            if (END_QUOTE == quote) {
                break;
            }
            ASSERT(quote < *result);
            if (verbose) bsl::cout << ' ' << quote;
        }
        if (verbose) bsl::cout << bsl::endl;
// To move the finger, e.g. to the last position printed, one must be careful
// to retain the ownership of the entire array.  Using the statement
// 'result.load(result.ptr()-i)' would be an error, because it would first
// compute the pointer value 'result.ptr()-i' of the argument, then release the
// entire array before starting to manage what has now become an invalid
// pointer.  Instead, 'result' must retain its ownership to the entire array,
// which can be attained by:
//..
        result.loadAlias(result, result.ptr()-i);
//..
// If we reset the result pointer, the entire array is deallocated:
//..
        result.clear();
        ASSERT(0 == ta.numBlocksInUse());
        ASSERT(0 == ta.numBytesInUse());

        return 0;
    }
//..

} // namespace USAGE_EXAMPLE

//=============================================================================
//                  TEST PROGRAM
//-----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    verbose = argc > 2;
    veryVerbose = argc > 3;

    (void) verbose;
    (void) veryVerbose;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    bslma_TestAllocator ta;

    switch (test) { case 0:
      case 15: {
        // --------------------------------------------------------------------
        // TESTING FAILURE TO COMPILE
        //
        // verifying that things that shouldn't compile, don't
        // --------------------------------------------------------------------

        int numdels = 0;

        Obj mX;
        Obj mY;

#       if 0
            // all comparisons should be illegal, these lines would not
            // compile

            bool a;
            a = mX == mY;
            a = mX != mY;
            a = mX <  mY;
            a = mX <= mY;
            a = mX >  mY;
            a = mX >= mY;
#       endif
      } break;
      case 14: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE
        //
        // Concerns
        //   The usage example provided in the component header file must
        //   compile, link, and run on all platforms as shown.
        //
        // Plan:
        //   Incorporate usage example from header into driver, remove leading
        //   comment characters, and replace 'assert' with 'ASSERT'.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        using namespace USAGE_EXAMPLE;

        if (verbose) cout << "\nUSAGE EXAMPLE"
                          << "\n-------------" << endl;

        usageExample1();

      } break;
      case 13: {
        // --------------------------------------------------------------------
        // TESTING CONVERSION EXAMPLES
        //
        // Concerns
        //   Test casting of managed pointers, both when the pointer is null
        //   and when it is not.
        //
        // Plan:
        //   Incorporate usage example from header into driver, remove leading
        //   comment characters, and replace 'assert' with 'ASSERT'.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        using namespace TYPE_CASTING_TEST_NAMESPACE;

        if (verbose) cout << "\nTYPE CASTING EXAMPLE"
                          << "\n--------------------" << endl;

        int numdels = 0;

        {
            implicitCastingExample();
            explicitCastingExample();

            bool castSucceeded;

            processPolymorphicObject(returnManagedPtr(&numdels, &ta),
                                                               &castSucceeded);
            ASSERT(!castSucceeded);
            processPolymorphicObject(
                    bdema_ManagedPtr<A>(returnDerivedPtr(&numdels, &ta)),
                                                               &castSucceeded);
            ASSERT(castSucceeded);
            processPolymorphicObject(
                    bdema_ManagedPtr<A>(returnSecondDerivedPtr(&numdels, &ta)),
                                                               &castSucceeded);
            ASSERT(!castSucceeded);

            returnManagedPtr(&numdels, &ta);
            returnDerivedPtr(&numdels, &ta);
            returnSecondDerivedPtr(&numdels, &ta);
        }

        LOOP_ASSERT(numdels, 6 == numdels);
      } break;
      case 12: {
        // --------------------------------------------------------------------
        // CLEAR and RELEASE
        //
        // Concerns:
        //   That clear and release work properly.
        //   Clear reclaims resources/runs deleter
        //   Release gives up ownership of resources without running deleters
        //
        //   Test each function behaves correctly given one of the following
        //     kinds of managed pointer objects:
        //     empty
        //     simple
        //     simple with factory
        //     simple with factory and deleter
        //     aliased
        //     aliased (original created with factory)
        //     aliased (original created with factory and deleter)
        //
        // Plan:
        //   TBD...
        //
        // Tested:
        //   void clear();
        //   bsl::pair<TYPE*,bdema_ManagedPtrDeleter> release();
        //
        // ADD NEGATIVE TESTING FOR operator*()
        // --------------------------------------------------------------------

        using namespace CREATORS_TEST_NAMESPACE;

        bslma_TestAllocator taDefault;
        bslma_DefaultAllocatorGuard allocatorGuard(&taDefault);

        int numDeletes = 0;
        {
            TObj *p = new (taDefault) MyTestObject(&numDeletes);
            Obj o(p);

            ASSERT(0 == numDeletes);
            o.clear();
            ASSERT(1 == numDeletes);

            ASSERT(!o && !o.ptr());
        }
        ASSERT(1 == numDeletes);

        numDeletes = 0;
        {
            TObj *p;
            {
                p = new (taDefault) MyTestObject(&numDeletes);
                Obj o(p);

                ASSERT(p == o.release().first);
                ASSERT(0 == numDeletes);

                ASSERT(!o && !o.ptr());
            }

            ASSERT(0 == numDeletes);
            taDefault.deleteObject(p);
        }
        ASSERT(1 == numDeletes);

        // testing 'release().second'
        numDeletes = 0;
        {
            TObj *p;
            {
                p =  new (taDefault) MyTestObject(&numDeletes);
                Obj o(p);

                bdema_ManagedPtrDeleter d(o.deleter());
                bdema_ManagedPtrDeleter d2(o.release().second);
                ASSERT(0 == numDeletes);

                ASSERT(d.object()  == d2.object());
                ASSERT(d.factory() == d2.factory());
                ASSERT(d.deleter() == d2.deleter());
            }

            ASSERT(0 == numDeletes);
            taDefault.deleteObject(p);
        }
        ASSERT(1 == numDeletes);
      } break;
      case 11: {
        // --------------------------------------------------------------------
        // SWAP AND ASSIGN TEST
        //
        // Concerns:
        //   Test all varieties of load, swap function and all assignments.
        //
        //   (AJM concerns, not yet confirmed to be tested)
        //
        //   assign clears the pointer being assigned from
        //   self-assignment safe
        //   assign destroys held pointer, does not merely swap
        //   assign-with-null
        //   assign with aliased pointer
        //   assign from pointer with factory/deleter
        //   assign to pointer with factory/deleter/aliased-pointer
        //   assign from a compatible managed pointer type
        //      (e.g. ptr-to-derived, to ptr-to-base, ptr to ptr-to-const)
        //   any managed pointer can be assigned to 'bdema_ManagedPtr<void>'
        //   assign to/from an empty managed pointer, each of the cases above
        //   assigning incompatible pointers should fail to compile (hand test)
        //
        //   swap with self changes nothing
        //   swap two simple pointer exchanged pointer values
        //   swap two aliased pointer exchanges aliases as well as pointers
        //   swap a simple managed pointer with an empty managed pointer
        //   swap a simple managed pointer with an aliased managed pointer
        //   swap an aliased managed pointer with an empty managed pointer
        //
        //   REFORMULATION
        //   want to be sure assignment works correctly for all combinations of
        //   assigning from and to a managed pointer with each of the following
        //   states.  Similarly, want to swap with each possible combination of
        //   each of the following states:
        //     empty
        //     simple
        //     simple with factory
        //     simple with factory and deleter
        //     aliased
        //     aliased (original created with factory)
        //     aliased (original created with factory and deleter)
        //
        //  In addition, assignment supports the following that 'swap' does not
        //  assignment from temporary/rvalue must be supported
        //  assignment from 'compatible' managed pointer must be supported
        //    i.e. where raw pointers would be convertible under assignment
        //
        //: X No 'bdema_ManagedPtr' method should allocate any memory.
        // Plan:
        //   TBD...
        //
        //   Test the functions in the order in which they are declared in
        //   the ManagedPtr class.
        //
        // Tested:
        //   [Just because a function is tested, we do not (yet) confirm that
        //    the testing is adequate.]
        //   operator bdema_ManagedPtr_Ref<BDEMA_TYPE>();
        //   operator bdema_ManagedPtr_Ref<OTHER>();
        //   void swap(bdema_ManagedPtr<BDEMA_TYPE>& rhs);
        //   bdema_ManagedPtr& operator=(bdema_ManagedPtr<BDEMA_TYPE> &rhs);
        //   bdema_ManagedPtr& operator=(bdema_ManagedPtr<OTHER> &rhs)
        //   bdema_ManagedPtr& operator=(bdema_ManagedPtr_Ref<BDEMA_TYPE> ref);
        // --------------------------------------------------------------------

        using namespace CREATORS_TEST_NAMESPACE;

        bslma_TestAllocator taDefault;
        bslma_DefaultAllocatorGuard allocatorGuard(&taDefault);

        int numDeletes = 0;
        {
            TObj *p =  new (taDefault) MyTestObject(&numDeletes);
            TObj *p2 = new (taDefault) MyTestObject(&numDeletes);

            Obj o(p);
            Obj o2(p2);

            o.swap(o2);

            ASSERT(o.ptr() == p2);
            ASSERT(o2.ptr() == p);
        }
        ASSERT(2 == numDeletes);

        numDeletes = 0;
        {
            TObj *p =  new (taDefault) MyTestObject(&numDeletes);
            TObj *p2 = new (taDefault) MyTestObject(&numDeletes);

            Obj o(p);
            Obj o2(p2);

            o = o2;

            ASSERT(!o2);
            ASSERT(1 == numDeletes);

            ASSERT(o.ptr() == p2);
        }
        ASSERT(2 == numDeletes);

        numDeletes = 0;
        {
            TObj *p =   new (taDefault) MyTestObject(&numDeletes);
            TDObj *p2 = new (taDefault) MyDerivedObject(&numDeletes);

            Obj o(p);
            DObj o2(p2);

            o = o2;

            ASSERT(!o2);
            ASSERT(1 == numDeletes);

            ASSERT(o.ptr() == p2);
        }
        ASSERT(2 == numDeletes);

        numDeletes = 0;
        {
            TObj *p = new (taDefault) MyTestObject(&numDeletes);
            Obj o(p);
            Obj o2;

            bdema_ManagedPtr_Ref<TObj> r = o;
            o2 = r;
            ASSERT(o2);
            ASSERT(!o);
            ASSERT(0 == numDeletes);

            bdema_ManagedPtr_Ref<TObj> r2 = o;
            o2 = r2;
            ASSERT(!o2);
            ASSERT(!o);

            ASSERT(1 == numDeletes);
        }
        ASSERT(1 == numDeletes);

        numDeletes = 0;
        {
            TDObj *p = new (taDefault) MyDerivedObject(&numDeletes);
            DObj o(p);
            Obj o2;

            bdema_ManagedPtr_Ref<TObj> r = o;
            o2 = r;
            ASSERT(o2);
            ASSERT(!o);
            ASSERT(0 == numDeletes);
        }
        ASSERT(1 == numDeletes);
      } break;
      case 10: {
        // --------------------------------------------------------------------
        // TESTING ACCESSORS
        //
        // Concerns:
        //   That all accessors work properly.  The 'ptr()' accessor has
        //   already been substantially tested in previous tests.
        //   The unspecified bool conversion evaluates as expected in all
        //     circumstances: if/while/for, (implied) operator!
        //   All accessors work on 'const'- qualified objects
        //   All accessors can be called for 'bdema_ManagedPtr<void>'
        //
        //: X No 'bdema_ManagedPtr' method should allocate any memory.
        //
        // Plan:
        //   TBD...
        //
        // Tested:
        //   operator BoolType() const;
        //   TYPE& operator*() const;
        //   TYPE *operator->() const;
        //   TYPE *ptr() const;
        //   const bdema_ManagedPtrDeleter& deleter() const;
        //
        // ADD NEGATIVE TESTING FOR operator*()
        // --------------------------------------------------------------------

        using namespace CREATORS_TEST_NAMESPACE;

        bslma_TestAllocator taDefault;
        bslma_DefaultAllocatorGuard allocatorGuard(&taDefault);

        // testing 'deleter()' accessor and 'release().second'
        int numDeletes = 0;
        {
            TObj *p;
            {
                p =  new (taDefault) MyTestObject(&numDeletes);
                Obj o(p);

                bdema_ManagedPtrDeleter d(o.deleter());
                bdema_ManagedPtrDeleter d2(o.release().second);
                ASSERT(0 == numDeletes);

                ASSERT(d.object()  == d2.object());
                ASSERT(d.factory() == d2.factory());
                ASSERT(d.deleter() == d2.deleter());
            }

            ASSERT(0 == numDeletes);
            taDefault.deleteObject(p);
        }
        ASSERT(1 == numDeletes);

        {
            int numDeallocation = taDefault.numDeallocation();
            numDeletes = 0;
            {
                SS *p = new (taDefault) SS(&numDeletes);
                std::strcpy(p->d_buf, "Woof meow");

                SSObj s(p);

                // testing * and -> references
                ASSERT(!strcmp(&(*s).d_buf[5], "meow"));
                ASSERT(!strcmp(&s->d_buf[5],   "meow"));
            }
            ASSERT(taDefault.numDeallocation() == numDeallocation + 1);
        }
      } break;
      case 9: {
        // --------------------------------------------------------------------
        // ALIAS SUPPORT TEST
        //
        // Concerns:
        //   class can hold an alias
        //   'ptr()' returns the alias pointer, and not the managed pointer
        //   correct deleter is run when an aliased pointer is destroyed
        //   appropriate object as cleared/deleters run when assigning to/from an aliased managed pointer
        //   class can alias itself
        //   alias type need not be the same as the managed type (often isn't)
        //   aliasing a null pointer clears the managed pointer, releasing any previously held object
        //
        //: X No 'bdema_ManagedPtr' method should allocate any memory.
        //
        // Plan:
        //   TBD...
        //
        // Tested:
        //   bdema_ManagedPtr(bdema_ManagedPtr<OTHER> &alias, TYPE *ptr)
        //   void loadAlias(bdema_ManagedPtr<OTHER> &alias, TYPE *ptr)
        // --------------------------------------------------------------------

        using namespace CREATORS_TEST_NAMESPACE;

        bslma_TestAllocator taDefault;
        bslma_DefaultAllocatorGuard allocatorGuard(&taDefault);

        int numDeletes = 0;
        {
            SS *p = new (taDefault) SS(&numDeletes);
            std::strcpy(p->d_buf, "Woof meow");

            SSObj s(p);
            ChObj c(s, &p->d_buf[5]);

            ASSERT(!s); // should not be testing operator! until test 13

            ASSERT(!std::strcmp(c.ptr(), "meow"));

            ASSERT(0 == numDeletes);
        }
        ASSERT(1 == numDeletes);


        int numDeallocation = taDefault.numDeallocation();
        numDeletes = 0;
        {
            SS *p = new (taDefault) SS(&numDeletes);
            std::strcpy(p->d_buf, "Woof meow");
            char *pc = (char *) taDefault.allocate(5);
            std::strcpy(pc, "Werf");

            SSObj s(p);
            ChObj c(pc);

            ASSERT(taDefault.numDeallocation() == numDeallocation);
            c.loadAlias(s, &p->d_buf[5]);
            ASSERT(taDefault.numDeallocation() == numDeallocation + 1);

            ASSERT(!s); // should not be testing operator! until test 13

            ASSERT(!std::strcmp(c.ptr(), "meow"));
        }
        ASSERT(taDefault.numDeallocation() == numDeallocation + 2);
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // CREATORS TEST
        //
        // Concerns:
        //   Exercise all declared constructors of ManagedPtr and conversion
        //   operator ManagedPtrRef().  Note that ManagedPtrRef normally just
        //   exists to facilitate copies and assignments of one managedptr to
        //   another, however, it is a good idea to test it explicitly
        //   because on some platforms, such as Sun, it is unnecessary and
        //   does not come into play when copying and assigning ManagedPtr's.
        //   Note that the primary accessor, 'ptr', cannot be considered to be
        //   validated until after testing the alias support, test case 11.
        //
        //: X No 'bdema_ManagedPtr' method should allocate any memory.
        //
        // Plan:
        //   TBD...
        //
        //   Go through the constructors in the order in which they are
        //   declared in the ManagedPtr class and exercise all of them,
        //   exercising the ManagedPtrRef class when need to exercise the
        //   ManagedPtr class.
        //   Remember to pass '0' as a null-pointer literal to all arguments
        //   that accept pointers (with negative testing if that is out of
        //   contract).
        //
        // Tested:
        //   bdema_ManagedPtr(bdema_ManagedPtr_Ref<BDEMA_TYPE> ref);
        //   bdema_ManagedPtr(bdema_ManagedPtr<BDEMA_TYPE> &original);
        //   bdema_ManagedPtr(bdema_ManagedPtr<OTHER> &original)
        //   bdema_ManagedPtr(bdema_ManagedPtr<OTHER> &alias, TYPE *ptr)
        //   bdema_ManagedPtr(TYPE *ptr, FACTORY *factory)
        //   bdema_ManagedPtr(TYPE *ptr,
        //                    void *factory,
        //                    void(*deleter)(TYPE*, void*))
        // --------------------------------------------------------------------

        using namespace CREATORS_TEST_NAMESPACE;

        bslma_TestAllocator taDefault;
        bslma_DefaultAllocatorGuard allocatorGuard(&taDefault);

        int numDeletes = 0;
        {
            // this test tests creation of a ref from the same type of
            // managedPtr, then assignment to a managedptr.

            Obj o2;
            {
                TObj *p = new (taDefault) MyTestObject(&numDeletes);
                Obj o(p);

                bdema_ManagedPtr_Ref<TObj> r = o;
                o2 = r;

                ASSERT(o2.ptr() == p);
            }
            ASSERT(0 == numDeletes);
        }
        ASSERT(1 == numDeletes);

        numDeletes = 0;
        {
            // this cast tests both a cast while creating the ref,
            // and the constructor from a ref.

            TDObj *p = new (taDefault) MyDerivedObject(&numDeletes);
            DObj o(p);

            ASSERT(o);
            ASSERT(o.ptr() == p);

            bdema_ManagedPtr_Ref<TObj> r = o;
            ASSERT(o);
            Obj o2(r);

            ASSERT(!o && !o.ptr()); // should not be testing operator! until test 13
            ASSERT(0 == numDeletes);

            ASSERT(o2.ptr() == p);
        }
        ASSERT(1 == numDeletes);

        numDeletes = 0;
        {
            TObj *p = new (taDefault) MyTestObject(&numDeletes);
            Obj o(p);
            ASSERT(o.ptr() == p);

            Obj o2(o);
            ASSERT(o2.ptr() == p);
            ASSERT(0 == o.ptr());
        }
        ASSERT(1 == numDeletes);

        numDeletes = 0;
        {
            TDObj *p = new (taDefault) MyDerivedObject(&numDeletes);
            DObj d(p);
            ASSERT(d.ptr() == p);

            Obj o(d);
            ASSERT(o.ptr() == p);
            ASSERT(0 == d.ptr());
        }
        ASSERT(1 == numDeletes);

        numDeletes = 0;
        {
            TObj *p = new (ta) MyTestObject(&numDeletes);
            Obj o(p, &ta);
        }
        ASSERT(1 == numDeletes);

        numDeletes = 0;
        {
            typedef void (*DeleterFunc)(MyTestObject *, void *);
            DeleterFunc deleterFunc = (DeleterFunc) &myTestDeleter;

            TObj *p = new (ta) MyTestObject(&numDeletes);
            Obj o(p, (void *) &ta, deleterFunc);
        }
        ASSERT(1 == numDeletes);

        numDeletes = 0;
        {
            TObj *p = new (ta) MyTestObject(&numDeletes);
            Obj o(p, &ta, &myTestDeleter);
        }
        ASSERT(1 == numDeletes);

      } break;
      case 7: {
        // --------------------------------------------------------------------
        // Testing 'load' overloads
        //
        // Concerns:
        //: 1 Calling 'load' on an empty pointer assigns ownership of the 
        //:   pointer passed as the argument.
        //:
        //: 2 Calling 'load' on a 'bdema_ManagedPtr' that owns a non-null
        //:   pointer destroys that pointer, and takes ownership of the new one
        //:
        //: 3 Calling load with a null pointer, or no argument, causes a
        //:   'bdema_ManagedPtr' object to destroy any managed object, without
        //:   owning a new one.
        //:
        //: 4 'bdema_ManagedPtr<void>' can load a pointer to any other type,
        //:   owning the pointer and deducing a deleter that will correctly
        //:   destroy the pointed-to object.
        //:
        //: 5 'bdema_ManagedPtr<void>' can load a true 'void *' pointer only if
        //:   an appropriate factory or deleter function is also passed.  The
        //:   single argument 'load(void *) should fail to compile.
        //:
        //: 6 'bdema_ManagedPtr<const T>' can be loaded with a 'T *' pointer
        //:   (cv-qualification conversion).
        //:
        //: 7 'bdema_ManagedPtr<base>' can be loaded with a 'derived *' pointer
        //:   and the deleter will destroy the 'derived' type, even if the
        //:   'base' destructor is not virtual.
        //:
        //: 8 When 'bdema_ManagedPtr' is passed a single 'FACTORY *' argument,
        //:   the implicit deleter-function will destroy the pointed-to object
        //:   using the FACTORY::deleteObject (non-static) method.
        //:
        //: 9 'bslma_Allocator' serves as a valid FACTORY type.
        //:
        //:10 A custom type offering just the 'deleteObject' (non-virtual)
        //:   member function serves as a valid FACTORY type.
        //:
        //:11 A 'bdema_ManagedPtr' points to the same object as the pointer
        //:   passed to 'load'.  Note that this includes null pointers.
        //:
        //:12 Destroying a 'bdema_ManagedPtr' destroys any owned object using
        //:   the deleter mechanism supplied by 'load'.
        //:
        //:13 Destroying a bdema_ManagedPtr that does not own a pointer has
        //:   no observable effect.
        //:
        //:14 No 'bdema_ManagedPtr' method should allocate any memory.
        //:
        //:15 Defensive checks assert in safe build modes when passing null
        //:   pointers as arguments for factory or deleters, unless target
        //:   pointer is also null.
        //
        // Plan:
        //   take an empty pointer, and call each overload of load.
        //      confirm pointer is initially null
        //      confirm new pointer value is stored by 'ptr()'
        //      confirm destructor destroys target object
        //      be sure to pass both '0' and valid pointer values to each potential overload
        //   Write a pair of nested loops
        //     For each iteration, first create a default-constructed bdema_ManagedPtr
        //     Then call a load function (testing each overload by the first loop)
        //     Then, in inner loop, load a second pointer and verify first target is destroyed
        //     Then verify the new target is destroyed when test object goes out of scope.
        //
        // Tested:
        //   void load(nullptr_t=0);
        //   void load(TYPE *ptr);
        //   void load(TYPE *ptr, FACTORY *factory)
        //   void load(TYPE *ptr,
        //             void *factory,
        //             void (*deleter)(TYPE *, void*));
        //   void load(TYPE *ptr,
        //             FACTORY *factory,
        //             void(*deleter)(TYPE *,FACTORY*))
        //   ~bdema_ManagedPtr();
        // --------------------------------------------------------------------

        using namespace CREATORS_TEST_NAMESPACE;

        bslma_TestAllocator taDefault;
        bslma_DefaultAllocatorGuard allocatorGuard(&taDefault);

        int numDeletes = 0;
        {
            TObj *p = new (taDefault) MyTestObject(&numDeletes);
            Obj o(p);

            ASSERT(o);
            ASSERT(0 == numDeletes);

            o.load();
            ASSERT(!o); // should not be testing operator! until test 13

            ASSERT(1 == numDeletes);
        }
        ASSERT(1 == numDeletes);

        numDeletes = 0;
        {
            TObj *p =  new (taDefault) MyTestObject(&numDeletes);
            TObj *p2 = new (taDefault) MyTestObject(&numDeletes);
            Obj o(p);

            ASSERT(o);
            ASSERT(0 == numDeletes);

            o.load(p2);
            ASSERT(o);

            ASSERT(1 == numDeletes);
        }
        ASSERT(2 == numDeletes);

        numDeletes = 0;
        {
            TObj *p =  new (taDefault) MyTestObject(&numDeletes);
            TObj *p2 = new (taDefault) MyTestObject(&numDeletes);
            Obj o(p);

            ASSERT(o);
            ASSERT(0 == numDeletes);

            o.load(p2, &taDefault);
            ASSERT(o);

            ASSERT(1 == numDeletes);
        }
        ASSERT(2 == numDeletes);

        numDeletes = 0;
        {
            typedef void (*DeleterFunc)(MyTestObject *, void *);
            DeleterFunc deleterFunc = (DeleterFunc) &myTestDeleter;

            TObj *p =  new (taDefault) MyTestObject(&numDeletes);
            TObj *p2 = new (taDefault) MyTestObject(&numDeletes);
            Obj o(p);
            o.load(p2, (void *) &taDefault, deleterFunc);

            ASSERT(1 == numDeletes);
        }
        ASSERT(2 == numDeletes);

        numDeletes = 0;
        {
            TObj *p =  new (taDefault) MyTestObject(&numDeletes);
            TObj *p2 = new (taDefault) MyTestObject(&numDeletes);
            Obj o(p);

            o.load(p2, &taDefault, &myTestDeleter);

            ASSERT(1 == numDeletes);
        }
        ASSERT(2 == numDeletes);
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // PRIMARY CREATORS TEST
        //   Note that we will not deem the destructor to be completely tested
        //   until the next test case, which tests the range of management
        //   strategies a bdema_ManagedPtr may hold.
        //
        // Concerns:
        //: 1 A default constructed 'bdema_ManagedPtr' does not own a pointer.
        //: 2 A default constructed 'bdema_ManagedPtr' does not allocate any
        //:   memory.
        //: 3 A 'bdema_ManagedPtr' takes ownership of a pointer passed as a
        //:   single argument to its constructor, and destroys the pointed-to
        //:   object in its destructor.  It does not allocate any memory.
        //: 4 A 'bdema_ManagedPtr<base>' object created by passing a 'derived *'
        //:   pointer calls the 'derived' destructor when destroying the
        //:   managed object, regardless of whether the 'base' destructor is
        //:   declared as 'virtual'.  No memory is allocated by
        //:   'bdema_ManagedPtr'.
        //: 5 A 'bdema_ManagedPtr<void>' object created by passing a 'derived *'
        //:   pointer calls the 'derived' destructor when destroying the
        //:   managed object.  No memory is allocated by 'bdema_ManagedPtr'.
        //: 6 A 'bdema_ManagedPtr' taking ownership of a null pointer passed as
        //:   a single argument is equivalent to default construction; it does
        //:   not allocate any memory.
        //
        // Plan:
        //    TBD
        //
        // Tested:
        //   bdema_ManagedPtr();
        //   bdema_ManagedPtr(nullptr_t);
        //   template<TARGET_TYPE> bdema_ManagedPtr(TARGET_TYPE *ptr);
        //   template<TARGET_TYPE> bdema_ManagedPtr<void>(TARGET_TYPE *ptr);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING PRIMARY CREATORS"
                          << "\n------------------------" << endl;

        using namespace CREATORS_TEST_NAMESPACE;

        if (verbose) cout << "\tTest default constructor\n";

        bslma_TestAllocator taDefault;
        bslma_DefaultAllocatorGuard allocatorGuard(&taDefault);

        int numDeletes = 0;
        {
            Obj o;

            ASSERT(0 == o.ptr());
        }
        ASSERT(0 == numDeletes);


        if (verbose) cout << "\tTest constructing with a null pointer\n";

        numDeletes = 0;
        {
            Obj o(0);

            ASSERT(0 == o.ptr());
        }
        ASSERT(0 == numDeletes);


        if (verbose) cout << "\tTest constructing void* with a null pointer\n";

        numDeletes = 0;
        {
            VObj o(0);

            ASSERT(0 == o.ptr());
        }
        ASSERT(0 == numDeletes);


        if (verbose) cout << "\tTest the single owned-pointer constructor\n";

        numDeletes = 0;
        {
            TObj *p = new (taDefault) MyTestObject(&numDeletes);
            Obj o(p);

            ASSERT(o.ptr() == p);
        }
        ASSERT(1 == numDeletes);


        if (verbose) cout << "\tTest derived-to-base pointer in constructor\n";

        numDeletes = 0;
        {
            TObj *p = new (taDefault) MyDerivedObject(&numDeletes);
            Obj o(p);

            ASSERT(o.ptr() == p);
            ASSERT(dynamic_cast<MyDerivedObject *>(o.ptr()) == p);
        }
        ASSERT(1 == numDeletes);


        if (verbose) cout << "\tTest valid pointer passed to void*\n";

        numDeletes = 0;
        {
            TObj *p = new (taDefault) MyDerivedObject(&numDeletes);
            VObj o(p);

            ASSERT(o.ptr() == p);
        }
        ASSERT(1 == numDeletes);

#if defined TEST_FOR_COMPILE_ERRORS
        // This segment of the test case examines the quality of compiler
        // diagnostics when trying to create a 'bdema_ManagedPtr' object with a
        // pointer that it not convertible to a pointer of the type that the
        // smart pointer is managing.
        if (verbose) cout << "\tTesting compiler diagnostics*\n";

        numDeletes = 0;
        {
            double *p = new (taDefault) double;
            Obj o(p);

//            ASSERT(o.ptr() == p);
        }
        ASSERT(1 == numDeletes);
#endif
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING bdema_ManagedPtrNilDeleter
        //
        // Concerns:
        //: 1 TBD Enumerate concerns
        //
        // Plan:
        //: 1 blah ...
        //
        // Testing:
        //   ... list class operations that are tested ...
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING bdema_ManagedPtrNilDeleter"
                          << "\n----------------------------------" << endl;

        if (verbose) cout << "\tTest blah...\n";

      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING bdema_ManagedPtr_FactoryDeleter (this one needs negative testing)
        //
        // Concerns:
        //: 1 TBD Enumerate concerns
        //
        // Plan:
        //: 1 blah ...
        //
        // Testing:
        //   ... list class operations that are tested ...
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING bdema_ManagedPtr_FactoryDeleter"
                          << "\n--------------------------------------" << endl;

        if (verbose) cout << "\tTest blah...\n";

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING bdema_ManagedPtr_Ref (this one needs negative testing)
        //
        // Concerns:
        //: 1 TBD Enumerate concerns
        //
        // Plan:
        //: 1 blah ...
        //
        // Testing:
        //   ... list class operations that are tested ...
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING bdema_ManagedPtr_Ref"
                          << "\n----------------------------" << endl;

        if (verbose) cout << "\tTest blah...\n";

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING bdema_ManagedPtr_Members
        //
        // Concerns:
        //: 1 TBD Enumerate concerns
        //
        // Plan:
        //: 1 blah ...
        //
        // Testing:
        //   ... list class operations that are tested ...
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING bdema_ManagedPtr_Members"
                          << "\n--------------------------------" << endl;

        if (verbose) cout << "\tTest blah...\n";

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //
        // Concerns:
        //   1. That the functions exist with the documented signatures.
        //   2. That the basic functionality works as documented.
        //
        // Plan:
        //   Exercise each function in turn and devise an elementary test
        //   sequence to ensure that the basic functionality is as documented.
        //
        // Testing:
        //   This test exercises basic functionality but *tests* *nothing*.
        // --------------------------------------------------------------------

        if (verbose) cout << "\nBREATHING TEST"
                          << "\n--------------" << endl;

        if (verbose) cout << "\tTest copy construction.\n";

        int numDeletes = 0;
        {
            TObj *p = new MyTestObject(&numDeletes);
            LOOP_ASSERT(numDeletes, 0 == numDeletes);

            Obj o(p);
            Obj o2(o);

            ASSERT(p == o2.ptr());
            ASSERT(0 == o.ptr());
            LOOP_ASSERT(numDeletes, 0 == numDeletes);
        }
        LOOP_ASSERT(numDeletes, 1 == numDeletes);

        if (verbose) cout << "\tTest assignment.\n";

        numDeletes = 0;
        {
            TObj *p = new MyTestObject(&numDeletes);
            LOOP_ASSERT(numDeletes, 0 == numDeletes);

            Obj o(p);
            Obj o2;

            ASSERT(p == o.ptr());
            LOOP_ASSERT(numDeletes, 0 == numDeletes);

            o2  = o;

            ASSERT(p == o2.ptr());
            ASSERT(0 == o.ptr());
            LOOP_ASSERT(numDeletes, 0 == numDeletes);
        }
        LOOP_ASSERT(numDeletes, 1 == numDeletes);

        if (verbose) cout << "\tTest construction from an rvalue.\n";

        numDeletes = 0;
        {
            Obj x(returnManagedPtr(&numDeletes, &ta)); Obj const &X = x;

            ASSERT(X.ptr());
            LOOP_ASSERT(numDeletes, 0 == numDeletes);
        }
        LOOP_ASSERT(numDeletes, 1 == numDeletes);

        if (verbose) cout << "\tTest assignment from an rvalue.\n";

        numDeletes = 0;
        {
            Obj x; Obj const &X = x;
            x = returnManagedPtr(&numDeletes, &ta);

            ASSERT(X.ptr());
            LOOP_ASSERT(numDeletes, 0 == numDeletes);
        }
        LOOP_ASSERT(numDeletes, 1 == numDeletes);

        if (verbose) cout << "\tTest conversion construction.\n";

        numDeletes = 0;
        {
            TDObj *p = new MyDerivedObject(&numDeletes);
            LOOP_ASSERT(numDeletes, 0 == numDeletes);

            DObj o(p);

            ASSERT(p == o.ptr());
            LOOP_ASSERT(numDeletes, 0 == numDeletes);

            Obj o2(o); // conversion construction

            ASSERT(p == o2.ptr());
            ASSERT(0 == o.ptr());
            LOOP_ASSERT(numDeletes, 0 == numDeletes);

            CObj o3(o2); // const-conversion construction

            ASSERT(p == o3.ptr());
            ASSERT(0 == o2.ptr());
            LOOP_ASSERT(numDeletes, 0 == numDeletes);
        }
        LOOP_ASSERT(numDeletes, 1 == numDeletes);

        if (verbose) cout << "\tTest conversion assignment.\n";

        numDeletes = 0;
        {
            TDObj *p = new MyDerivedObject(&numDeletes);
            LOOP_ASSERT(numDeletes, 0 == numDeletes);

            DObj o(p);

            ASSERT(p == o.ptr());
            LOOP_ASSERT(numDeletes, 0 == numDeletes);

            Obj o2;
            o2  = o; // conversion assignment

            ASSERT(p == o2.ptr());
            ASSERT(0 == o.ptr());
            LOOP_ASSERT(numDeletes, 0 == numDeletes);

            CObj o3;
            o3 = o2; // const-conversion assignment

            ASSERT(p == o3.ptr());
            ASSERT(0 == o2.ptr());
            LOOP_ASSERT(numDeletes, 0 == numDeletes);
        }
        LOOP_ASSERT(numDeletes, 1 == numDeletes);

        if (verbose)
            cout << "\tTest conversion construction from an rvalue.\n";

        numDeletes = 0;
        {
            Obj x(returnDerivedPtr(&numDeletes, &ta)); Obj const &X = x;

            ASSERT(X.ptr());
            LOOP_ASSERT(numDeletes, 0 == numDeletes);
        }
        LOOP_ASSERT(numDeletes, 1 == numDeletes);

        if (verbose)
            cout << "\tTest conversion assignment from an rvalue.\n";

        numDeletes = 0;
        {
            Obj x; Obj const &X = x;
            x = returnDerivedPtr(&numDeletes, &ta); // conversion-assignment
                                                    // from an rvalue

            ASSERT(X.ptr());
            LOOP_ASSERT(numDeletes, 0 == numDeletes);
        }
        LOOP_ASSERT(numDeletes, 1 == numDeletes);

        if (verbose) cout << "\tTest alias construction.\n";

        numDeletes = 0;
        {
            TObj *p = new MyTestObject(&numDeletes);
            ASSERT(0 != p);
            LOOP_ASSERT(numDeletes, 0 == numDeletes);

            Obj o(p);
            bdema_ManagedPtr<int> o2(o, o->valuePtr()); // alias construction

            ASSERT(p->valuePtr() == o2.ptr());
            ASSERT(0 == o.ptr());
            LOOP_ASSERT(numDeletes, 0 == numDeletes);
        }
        LOOP_ASSERT(numDeletes, 1 == numDeletes);

        if (verbose) cout << "\tTest alias construction with conversion.\n";

        numDeletes = 0;
        {
            TDObj *p = new MyDerivedObject(&numDeletes);
            ASSERT(0 != p);
            LOOP_ASSERT(numDeletes, 0 == numDeletes);

            Obj o(p);
            bdema_ManagedPtr<int> o2(o, o->valuePtr()); // alias construction

            ASSERT(p->valuePtr() == o2.ptr());
            ASSERT(0 == o.ptr());
            LOOP_ASSERT(numDeletes, 0 == numDeletes);
        }
        LOOP_ASSERT(numDeletes, 1 == numDeletes);

        if (verbose) cout << "\tTest 'load' method.\n";

        numDeletes = 0;
        {
            int numDeletes2 = 0;
            TObj *p = new MyTestObject(&numDeletes2);
            ASSERT(0 != p);
            ASSERT(0 == numDeletes2);

            Obj o(p);

            TObj *p2 = new MyTestObject(&numDeletes);
            LOOP_ASSERT(numDeletes, 0 == numDeletes);

            o.load(p2);
            ASSERT(p2 == o.ptr());
            ASSERT(1 == numDeletes2);
            LOOP_ASSERT(numDeletes, 0 == numDeletes);
        }
        LOOP_ASSERT(numDeletes, 1 == numDeletes);

        if (verbose) cout << "\tTest 'load' method with allocator.\n";

        numDeletes = 0;
        {
            int numDeletes2 = 0;
            TObj *p = new MyTestObject(&numDeletes2);
            ASSERT(0 == numDeletes2);

            Obj o(p);

            TObj *p2 = new(ta) MyTestObject(&numDeletes);
            LOOP_ASSERT(numDeletes, 0 == numDeletes);

            o.load(p2,&ta);
            ASSERT(p2 == o.ptr());
            LOOP_ASSERT(numDeletes2, 1 == numDeletes2);
            LOOP_ASSERT(numDeletes, 0 == numDeletes);
        }
        LOOP_ASSERT(numDeletes, 1 == numDeletes);

        if (verbose) cout << "\tTest 'loadAlias'.\n";

        numDeletes = 0;
        {
            TObj *p = new MyTestObject(&numDeletes);
            LOOP_ASSERT(numDeletes, 0 == numDeletes);

            Obj o(p);
            bdema_ManagedPtr<int> o2;

            ASSERT(p == o.ptr());
            LOOP_ASSERT(numDeletes, 0 == numDeletes);

            o2.loadAlias(o, o->valuePtr());

            ASSERT(p->valuePtr() == o2.ptr());
            ASSERT(0 == o.ptr());
            LOOP_ASSERT(numDeletes, 0 == numDeletes);

            // Check load alias to self
            o2.loadAlias(o2, p->valuePtr(1));
            ASSERT(p->valuePtr(1) == o2.ptr());
            LOOP_ASSERT(numDeletes, 0 == numDeletes);
        }
        LOOP_ASSERT(numDeletes, 1 == numDeletes);

        if (verbose) cout << "\tTest 'swap'.\n";

        numDeletes = 0;
        {
            TObj *p = new MyTestObject(&numDeletes);
            LOOP_ASSERT(numDeletes, 0 == numDeletes);

            Obj o(p);
            Obj o2;

            ASSERT(p == o.ptr());
            LOOP_ASSERT(numDeletes, 0 == numDeletes);

            o2.swap(o);
            ASSERT(p == o2.ptr());
            ASSERT(0 == o.ptr());
            LOOP_ASSERT(numDeletes, 0 == numDeletes);
        }
        LOOP_ASSERT(numDeletes, 1 == numDeletes);

        if (verbose) cout << "\tTest 'swap' with custom deleter.\n";

        numDeletes = 0;
        {
            TObj *p = new(ta) MyTestObject(&numDeletes);
            LOOP_ASSERT(numDeletes, 0 == numDeletes);

            Obj o(p, &ta, &myTestDeleter);
            Obj o2;

            ASSERT(p == o.ptr());
            LOOP_ASSERT(numDeletes, 0 == numDeletes);

            o2.swap(o);
            ASSERT(p == o2.ptr());
            ASSERT(0 == o.ptr());
            LOOP_ASSERT(numDeletes, 0 == numDeletes);
        }
        LOOP_ASSERT(numDeletes, 1 == numDeletes);

        if (verbose) cout << "\tTest boolean.\n";

        numDeletes = 0;
        {
            TObj *p = new(ta) MyTestObject(&numDeletes);
            LOOP_ASSERT(numDeletes, 0 == numDeletes);

            Obj o(p, &ta, &myTestDeleter);
            Obj o2;

            ASSERT(o);
            ASSERT(!o2);

            LOOP_ASSERT(numDeletes, 0 == numDeletes);
        }
        LOOP_ASSERT(numDeletes, 1 == numDeletes);
     } break;
      case -1: {
        // --------------------------------------------------------------------
        // TESTING ADDITIONAL CONCERNS
        //
        // Concerns:
        //: 1 Two 'bdema_ManagedPtr<T>' objects should not be comparable with
		//:   the equality operator.
		//
        //: 2 Two objects of different instantiations of the 'bdema_ManagedPtr'
		//:   class template should not be comparable with the equality
		//:   operator
        //
        // Plan:
        //   The absence of a specific operator will be tested by failing to
		//   compile test code using that operator.  These tests will be
		//   configured to compile only when specific macros are defined as
		//   part of the build configuration, and not routinely tested.
        //
        // Testing:
        //   This test is checking for the *absence* of 'operator=='.
        // --------------------------------------------------------------------
//#define BDEMA_MANAGEDPTR_TEST_FLAG_1
//#define BDEMA_MANAGEDPTR_TEST_FLAG_2

#if defined BDEMA_MANAGEDPTR_TEST_FLAG_1
		bdema_ManagedPtr<int> x;
		bdema_ManagedPtr<int> y;
		// The following two lines should fail to compile
		ASSERT(x == y);
		ASSERT(x != y);
#endif

#if defined BDEMA_MANAGEDPTR_TEST_FLAG_2
		bdema_ManagedPtr<int> a;
		bdema_ManagedPtr<double> b;
		// The following two lines should fail to compile
		ASSERT(a == b);
		ASSERT(a != b);

		ASSERT(b == a);
		ASSERT(b != a);
#endif
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
//      Copyright (C) Bloomberg L.P., 2005
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
