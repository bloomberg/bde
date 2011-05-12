// bdema_managedptr.t.cpp          -*-C++-*-

#include <bdema_managedptr.h>
#include <bslma_allocator.h>                    // for testing only
#include <bslma_defaultallocatorguard.h>        // for testing only
#include <bslma_testallocator.h>                // for testing only

#include <bsl_cstdlib.h>     // atoi()
#include <bsl_cstring.h>     // memcpy()
#include <bsl_iostream.h>

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

//=============================================================================
//                             TEST PLAN
//
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
// [ 2] bdema_ManagedPtr();
// [ 2] bdema_ManagedPtr(TYPE *ptr);
// [ 2] bdema_ManagedPtr(bdema_ManagedPtr_Ref<TYPE> ref);
// [ 2] bdema_ManagedPtr(bdema_ManagedPtr<TYPE> &original);
// [ 2] bdema_ManagedPtr(bdema_ManagedPtr<OTHER> &original)
// [ 2] bdema_ManagedPtr(bdema_ManagedPtr<OTHER> &alias, TYPE *ptr)
// [ 2] bdema_ManagedPtr(TYPE *ptr, FACTORY *factory)
// [ 2] bdema_ManagedPtr(TYPE *ptr, void *factory,void(*deleter)(TYPE*, void*))
// [ 2] ~bdema_ManagedPtr();
// [ 3] operator bdema_ManagedPtr_Ref<TYPE>();
// [ 3] operator bdema_ManagedPtr_Ref<OTHER>();
// [ 3] void load(TYPE *ptr=0);
// [ 3] void load(TYPE *ptr);
// [ 3] void load(TYPE *ptr, FACTORY *factory)
// [ 3] void load(TYPE *ptr, void *factory, void (*deleter)(TYPE *, void*));
// [ 3] void load(TYPE *ptr, FACTORY *factory, void(*deleter)(TYPE *,FACTORY*))
// [ 3] void loadAlias(bdema_ManagedPtr<OTHER> &alias, TYPE *ptr)
// [ 3] void swap(bdema_ManagedPtr<TYPE>& rhs);
// [ 3] bdema_ManagedPtr<TYPE>& operator=(bdema_ManagedPtr<TYPE> &rhs);
// [ 3] bdema_ManagedPtr<TYPE>& operator=(bdema_ManagedPtr<OTHER> &rhs)
// [ 3] bdema_ManagedPtr<TYPE>& operator=(bdema_ManagedPtr_Ref<TYPE> ref);
// [ 4] void clear();
// [ 4] bsl::pair<TYPE*,bdema_ManagedPtrDeleter> release();
// [ 2] operator bdema_ManagedPtr_UnspecifiedBool() const;
// [ 4] TYPE& operator*() const;
// [ 4] TYPE *operator->() const;
// [ 2] TYPE *ptr() const;
// [ 4] bdema_ManagedPtrDeleter const& deleter() const;
//
// class bdema_ManagedPtrDeleter
// class bdema_ManagedPtrFactoryDeleter       (this one needs negative testing)
// class bdema_ManagedPtrNilDeleter
//
// imp. class bdema_ManagedPtr_Members
// imp. class bdema_ManagedPtr_Ref            (this one needs negative testing)
// imp. class bdema_ManagedPtr_ReferenceType
// imp. class bdema_ManagedPtr_UnspecifiedBoolHelper
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 5] CASTING EXAMPLE
// [ 6] USAGE EXAMPLE
// [ 7] VERIFYING FAILURES TO COMPILE

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

bdema_ManagedPtr<int> i;

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
{
    for (unsigned i = 0; i < sizeof(d_value) / sizeof(d_value[0]); ++i) {
        d_value[i] = 0;
    }
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
            B *b_p = 0;
            A *a_p = b_p;
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
    int numDeletes = 0;

    bslma_TestAllocator ta;

    switch (test) { case 0:
      case 7: {
        // --------------------------------------------------------------------
        // TESTING FAILURE TO COMPILE
        //
        // verifying that things that shouldn't compile, don't
        // --------------------------------------------------------------------

        int numdels = 0;

        Obj mX;
        Obj mY;

#       if 0
            // < and > comparisons should be illegal, these lines would not
            // compile

            bool a;
            a = mX <  mY;
            a = mX <= mY;
            a = mX >  mY;
            a = mX >= mY;
#       endif
      } break;
      case 6: {
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
      case 5: {
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
      case 4: {
        // --------------------------------------------------------------------
        // CLEAR, RELEASE, AND ACCESSORS
        //
        // Concerns:
        //   That clear, release, and all accessors work properly.  The 'ptr()'
        //   accessor and the 'ManagedPtr_unspecifiedBool()' conversion have
        //   already been substantially tested in previous tests.
        //
        // Tested:
        // '*' is my short-hand for "not yet verified this test as claimed"
        // 'X' is my short-hand for "confirmed to *not* be tested"
        // '-' is my short-hand for "confirmed to not be explicitly tested"
        //     Just because a function is tested, we do not confirm that the
        //     testing is adequate, that audit happens later.
        // * void clear();
        // * bsl::pair<TYPE*,bdema_ManagedPtrDeleter> release();
        // * operator bdema_ManagedPtr_UnspecifiedBool() const;
        // * TYPE& operator*() const;
        // * TYPE *operator->() const;
        // * bdema_ManagedPtrDeleter const& deleter() const;
        //
        // ADD NEGATIVE TESTING FOR operator*()
        // --------------------------------------------------------------------

        using namespace CREATORS_TEST_NAMESPACE;

        bslma_TestAllocator taDefault;
        bslma_DefaultAllocatorGuard allocatorGuard(&taDefault);

        numDeletes = 0;
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

        // testing 'deleter()' accessor and 'release().second'
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
      case 3: {
        // --------------------------------------------------------------------
        // LOAD, SWAP, AND ASSIGN TEST
        //
        // Concerns:
        //   Test all varieties of load, swap function and all assignments.
        //
        // Plan:
        //   Test the functions in the order in which they are declared in
        //   the ManagedPtr class.
        //
        // Tested:
        // '*' is my short-hand for "not yet verified this test as claimed"
        // 'X' is my short-hand for "confirmed to *not* be tested"
        // '-' is my short-hand for "confirmed to not be explicitly tested"
        //     Just because a function is tested, we do not confirm that the
        //     testing is adequate, that audit happens later.
        // * operator bdema_ManagedPtr_Ref<TYPE>();
        // * operator bdema_ManagedPtr_Ref<OTHER>();
        // * void load(TYPE *ptr=0);
        // * void load(TYPE *ptr);
        // * void load(TYPE *ptr, FACTORY *factory)
        // * void load(TYPE *ptr,
        //             void *factory,
        //             void (*deleter)(TYPE *, void*));
        // * void load(TYPE *ptr,
        //             FACTORY *factory,
        //             void(*deleter)(TYPE *,FACTORY*))
        // * void loadAlias(bdema_ManagedPtr<OTHER> &alias, TYPE *ptr)
        // * void swap(bdema_ManagedPtr<TYPE>& rhs);
        // * bdema_ManagedPtr<TYPE>& operator=(bdema_ManagedPtr<TYPE> &rhs);
        // * bdema_ManagedPtr<TYPE>& operator=(bdema_ManagedPtr<OTHER> &rhs)
        // * bdema_ManagedPtr<TYPE>& operator=(bdema_ManagedPtr_Ref<TYPE> ref);
        // --------------------------------------------------------------------

        using namespace CREATORS_TEST_NAMESPACE;

        bslma_TestAllocator taDefault;
        bslma_DefaultAllocatorGuard allocatorGuard(&taDefault);

        numDeletes = 0;
        {
            TObj *p = new (taDefault) MyTestObject(&numDeletes);
            Obj o(p);

            ASSERT(o);
            ASSERT(0 == numDeletes);

            o.load();
            ASSERT(!o);

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

        {
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

                ASSERT(!s);

                ASSERT(!std::strcmp(c.ptr(), "meow"));
            }
            ASSERT(taDefault.numDeallocation() == numDeallocation + 2);
        }

        numDeletes = 0;
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
      case 2: {
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
        //
        // Plan:
        //   Go through the constructors in the order in which they are
        //   declared in the ManagedPtr class and exercise all of them,
        //   exercising the ManagedPtrRef class when need to exercise the
        //   ManagedPtr class.
        //
        // Tested:
        // '*' is my short-hand for "not yet verified this test as claimed"
        // 'X' is my short-hand for "confirmed to *not* be tested"
        // '-' is my short-hand for "confirmed to not be explicitly tested"
        //     Just because a function is tested, we do not confirm that the
        //     testing is adequate, that audit happens later.
        // X bdema_ManagedPtr();
        // * bdema_ManagedPtr(TYPE *ptr);
        // * bdema_ManagedPtr(bdema_ManagedPtr_Ref<TYPE> ref);
        // * bdema_ManagedPtr(bdema_ManagedPtr<TYPE> &original);
        // * bdema_ManagedPtr(bdema_ManagedPtr<OTHER> &original)
        // * bdema_ManagedPtr(bdema_ManagedPtr<OTHER> &alias, TYPE *ptr)
        // * bdema_ManagedPtr(TYPE *ptr, FACTORY *factory)
        // * bdema_ManagedPtr(TYPE *ptr,
        //                    void *factory,
        //                    void(*deleter)(TYPE*, void*))
        // - ~bdema_ManagedPtr();
        // * TYPE *ptr() const;
        // --------------------------------------------------------------------

        using namespace CREATORS_TEST_NAMESPACE;

        bslma_TestAllocator taDefault;
        bslma_DefaultAllocatorGuard allocatorGuard(&taDefault);

        numDeletes = 0;
        {
            Obj o;
        }
        ASSERT(0 == numDeletes);

        numDeletes = 0;
        {
            TObj *p = new (taDefault) MyTestObject(&numDeletes);
            Obj o(p);

            ASSERT(o.ptr() == p);
        }
        ASSERT(1 == numDeletes);

        numDeletes = 0;
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

            ASSERT(!o && !o.ptr());
            ASSERT(0 == numDeletes);

            ASSERT(o2.ptr() == p);
        }
        ASSERT(1 == numDeletes);

        numDeletes = 0;
        {
            TObj *p = new (taDefault) MyTestObject(&numDeletes);
            Obj o(p);

            Obj o2(o);
            ASSERT(o2.ptr() == p);
        }
        ASSERT(1 == numDeletes);

        numDeletes = 0;
        {
            TDObj *p = new (taDefault) MyDerivedObject(&numDeletes);
            DObj d(p);

            Obj o(d);
        }
        ASSERT(1 == numDeletes);

        numDeletes = 0;
        {
            SS *p = new (taDefault) SS(&numDeletes);
            std::strcpy(p->d_buf, "Woof meow");

            SSObj s(p);
            ChObj c(s, &p->d_buf[5]);

            ASSERT(!s);

            ASSERT(!std::strcmp(c.ptr(), "meow"));

            ASSERT(0 == numDeletes);
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

        numDeletes = 0;
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
#ifndef OK_CONVERSION_ASSIGNMENT
        if (verbose) cout << "\t\tdisabled for this version.\n";
#else
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
#endif

        if (verbose)
            cout << "\tTest conversion construction from an rvalue.\n";
#ifndef OK_CONVERSION_ASSIGNMENT
        if (verbose) cout << "\t\tdisabled for this version.\n";
#else
        numDeletes = 0;
        {
            Obj x(returnDerivedPtr(&numDeletes, &ta)); Obj const &X = x;

            ASSERT(X.ptr());
            LOOP_ASSERT(numDeletes, 0 == numDeletes);
        }
        LOOP_ASSERT(numDeletes, 1 == numDeletes);
#endif

        if (verbose)
            cout << "\tTest conversion assignment from an rvalue.\n";
#ifndef OK_CONVERSION_ASSIGNMENT
        if (verbose) cout << "\t\tdisabled for this version.\n";
#else
        numDeletes = 0;
        {
            Obj x; Obj const &X = x;
            x = returnDerivedPtr(&numDeletes, &ta); // conversion-assignment
                                                    // from an rvalue

            ASSERT(X.ptr());
            LOOP_ASSERT(numDeletes, 0 == numDeletes);
        }
        LOOP_ASSERT(numDeletes, 1 == numDeletes);
#endif

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
