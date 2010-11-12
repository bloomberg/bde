// bdema_managedptr.t.cpp          -*-C++-*-

#include <bdema_managedptr.h>
#include <bslma_allocator.h>        // for testing only
#include <bslma_testallocator.h>    // for testing only

#include <cstdlib>     // atoi()
#include <cstring>     // memcpy()
#include <bsl_iostream.h>

using namespace BloombergLP;
using namespace std;
using namespace bsl;  //automatically added by script


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
// [  ] bdema_ManagedPtr(TYPE *ptr=0);
// [  ] bdema_ManagedPtr(bdema_ManagedPtrRef<TYPE> ref);
// [  ] bdema_ManagedPtr(bdema_ManagedPtrRef<OTHER> ref)
// [  ] bdema_ManagedPtr(bdema_ManagedPtr<TYPE> &original);
// [  ] bdema_ManagedPtr(bdema_ManagedPtr<OTHER> &original)
// [  ] bdema_ManagedPtr(bdema_ManagedPtr<OTHER> &alias, TYPE *ptr)
// [  ] bdema_ManagedPtr(TYPE *ptr, FACTORY *factory)
// [  ] bdema_ManagedPtr(TYPE *ptr, void *factory,void(*deleter)(TYPE*, void*))
// [  ] bdema_ManagedPtr(TYPE *ptr, FACTORY *factory, ...)
// [  ] ~bdema_ManagedPtr();
// [  ] void load(TYPE *ptr=0);
// [  ] void load(TYPE *ptr, FACTORY *factory) 
// [  ] void load(TYPE *ptr, void *factory, void (*deleter)(TYPE *, void*));
// [  ] void load(TYPE *ptr, FACTORY *factory, void(*deleter)(TYPE *,FACTORY*))
// [  ] void loadAlias(bdema_ManagedPtr<OTHER> &alias, TYPE *ptr)
// [  ] void swap(bdema_ManagedPtr<TYPE>& rhs);
// [  ] bdema_ManagedPtr<TYPE>& operator=(bdema_ManagedPtr<TYPE> &rhs);
// [  ] bdema_ManagedPtr<TYPE>& operator=(bdema_ManagedPtr<OTHER> &rhs)
// [  ] void clear();
// [  ] bsl::pair<TYPE*,bdema_ManagedPtrDeleter> release();
// [  ] operator bdema_ManagedPtrRef<TYPE>();
// [  ] operator bool() const;
// [  ] TYPE& operator*() const;
// [  ] TYPE *operator->() const;
// [  ] TYPE *ptr() const;
// [  ] bdema_ManagedPtrDeleter const& deleter() const;
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 2] CASTING EXAMPLE
// [ 3] USAGE EXAMPLE
//=============================================================================
//                    STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
static int testStatus = 0;

static void aSsErT(int c, const char *s, int i) {
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

static int verbose = 0;
static int veryVerbose = 0;

bdema_ManagedPtr<int> i;

class MyTestObject;
class MyDerivedObject;
typedef MyTestObject TObj;
typedef bdema_ManagedPtr<MyTestObject> Obj;
typedef bdema_ManagedPtr<const MyTestObject> CObj;
typedef MyDerivedObject TDObj;
typedef bdema_ManagedPtr<MyDerivedObject> DObj;
typedef bdema_ManagedPtr<const MyDerivedObject> CDObj;

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

//=============================================================================
//                    FILE-STATIC FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

static void MyTestDeleter(TObj *object, bslma_TestAllocator *allocator)
{
    allocator->deleteObject(object);
    bsl::cout << "MyTestDeleter called" << endl;
}

static bdema_ManagedPtr<MyTestObject>
returnManagedPtr(int *numDels, bslma_TestAllocator *allocator)
{
    MyTestObject *p = new (*allocator) MyTestObject(numDels);
    bdema_ManagedPtr<MyTestObject> ret(p, allocator);
    return ret;
}

static bdema_ManagedPtr<MyDerivedObject>
returnDerivedPtr(int *numDels, bslma_TestAllocator *allocator)
{
    MyDerivedObject *p = new (*allocator) MyDerivedObject(numDels);
    bdema_ManagedPtr<MyDerivedObject> ret(p, allocator);
    return ret;
}

//=============================================================================
//                                CASTING EXAMPLE
//-----------------------------------------------------------------------------
namespace TYPE_CASTING_EXAMPLE {

    typedef MyTestObject A;
    typedef MyDerivedObject B;
    void processAObject(bdema_ManagedPtr<A> Aptr) {}
    void processBObject(bdema_ManagedPtr<B> Bptr) {}

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
        B *b_p = 0; // = 0 not in the header usage example - added to bypass
                    // /RTC failure on Win64
        A *a_p = b_p;
//..
// are legal expressions, then the statements
//..
#ifdef BSLS_PLATFORM__CMP_SUN
        bdema_ManagedPtr<B> b_mp1;
        bdema_ManagedPtr<A> a_mp1;
        a_mp1 = b_mp1; // conversion assignment
                       // WARNING: READ RELEASE NOTES BELOW
#endif
//..
// and
//..
        bdema_ManagedPtr<B> b_mp2;
        bdema_ManagedPtr<A> a_mp2(b_mp2); // conversion construction
//..
// should also be valid.  NOTE that in all of the above cases, the proper
// destructor of 'B' will be invoked when the instance is destroyed even if
// 'A' does not provide a polymorphic destructor.
//
#if 0
// RELEASE NOTES: the assignment with conversion will *not* compile on all
// platforms with BDE 1.3.0.  That is, the following statement will not compile
// (for instance, with the AIX or Gnu compilers):
//..
        a_mp2 = b_mp2; // WILL NOT COMPILE WITH BDE 1.3.0
//..
#else
// The fixed version is given in the Clearcase dev branch, but because of
// binary incompatibilities, it is not possible to release the fix at this
// time.  This limitation can be overcome, however, by using an explicit cast
// as detailed below:
//..
        a_mp2  = bdema_ManagedPtr<A>(b_mp2); // WORKAROUND
#endif
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
    void processPolymorphicObject(bdema_ManagedPtr<A> aPtr)
    {
        bdema_ManagedPtr<B> bPtr(aPtr, dynamic_cast<B*>(aPtr.ptr()));
        if (bPtr) {
            processBObject(bPtr);
        }
        else {
            processAObject(aPtr);
        }
    }
//..
// If the value of 'aPtr' can be dynamically cast to 'B*' then ownership is
// transferred to 'bPtr', otherwise 'aPtr' is to be modified.  As previously
// stated, the managed instance will be destroyed correctly regardless of how
// it is cast.

} // namespace TYPE_CASTING_EXAMPLE
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
      case 3: {
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
      case 2: {
        // --------------------------------------------------------------------
        // TESTING CONVERSION EXAMPLES
        //
        // Concerns
        //   The examples provided in the component header file in the "Type
        //   Casting" section must compile, link, and run on all platforms as
        //   shown.
        //
        // Plan:
        //   Incorporate usage example from header into driver, remove leading
        //   comment characters, and replace 'assert' with 'ASSERT'.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        using namespace TYPE_CASTING_EXAMPLE;

        if (verbose) cout << "\nTYPE CASTING EXAMPLE"
                          << "\n--------------------" << endl;

        implicitCastingExample();
        explicitCastingExample();
        processPolymorphicObject(Obj());

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
#if !defined(BSLS_PLATFORM__CMP_SUN)
        if (verbose) cout << "\t\tdisabled for this compiler.\n";
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
#if !defined(BSLS_PLATFORM__CMP_SUN)
        if (verbose) cout << "\t\tdisabled for this compiler.\n";
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
#if !defined(BSLS_PLATFORM__CMP_SUN)
        if (verbose) cout << "\t\tdisabled for this compiler.\n";
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

            Obj o(p, &ta, &MyTestDeleter);
            Obj o2;

            ASSERT(p == o.ptr());
            LOOP_ASSERT(numDeletes, 0 == numDeletes);

            o2.swap(o);
            ASSERT(p == o2.ptr());
            ASSERT(0 == o.ptr());
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
