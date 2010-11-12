// bdema_allocator.t.cpp        -*-C++-*-

#include <bdema_allocator.h>

#include <bsl_cstdlib.h>     // atoi()
#include <bsl_cstring.h>     // memcpy()
#include <bsl_iostream.h>

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

//=============================================================================
//                              TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// We are testing a pure protocol class as well as a set of overloaded
// operators.  We need to verify that (1) a concrete derived class compiles
// and links, (2) that the overloaded new operator correctly forwards
// the call to the allocate method of the supplied allocator, and (3) that the
// one concrete function, deleteObject() destroys an object and calls the
// deallocate method of the supplied allocator.
//-----------------------------------------------------------------------------
// [ 1] virtual ~bdema_Allocator();
// [ 1] virtual void *allocate(size_type size) = 0;
// [ 1] virtual void deallocate(void *address) = 0;
// [ 2] template<typename TYPE> deleteObject(const TYPE *);
//
// [ 3] void *operator new(int size, bdema_Allocator& basicAllocator);
// [ 4] void operator delete(void *address, bdema_Allocator& basicAllocator);
//-----------------------------------------------------------------------------
// [ 1] PROTOCOL TEST - Make sure derived class compiles and links.
// [ 3] OPERATOR TEST - Make sure overloaded operators call correct functions.
// [ 4] EXCEPTION SAFETY OF OPERATOR NEW TEST - Make sure operator delete is
//                      invoked on exception.
// [ 5] USAGE TEST - Make sure main usage example compiles and works properly.
// [ 5] NEW OPERATOR FUNCTION USAGE TEST.  Verify that example as well.
//=============================================================================

//=============================================================================
//                    STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
static int testStatus = 0;
static void aSsErT(int c, const char *s, int i)
{
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}
#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", "<< flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number
#define PS(X) cout << #X " = \n" << (X) << endl; // Print identifier and value.
#define T_()  cout << "\t" << flush;          // Print a tab (w/o newline)

//=============================================================================
//                      CONCRETE DERIVED TYPES
//-----------------------------------------------------------------------------
class my_Allocator : public bdema_Allocator {
  // Test class used to verify protocol.

    int d_fun;  // holds code describing function:
                //   + 1 allocate
                //   + 2 deallocate

    int d_arg;  // holds argument from alloc

    char d_storage[32];  // space to allow for "large" allocations

    int d_allocateCount;    // number of times  allocate called
    int d_deallocateCount;  // number of times  deallocate called

  public:
    my_Allocator() : d_allocateCount(0), d_deallocateCount(0) { }
    ~my_Allocator() { }

    void *allocate(size_type s) {
        d_fun = 1;
        d_arg = s;
        ++d_allocateCount;
        return this;
    }

    void deallocate(void *) { d_fun = 2;  ++d_deallocateCount; }

    int fun() const { return d_fun; }
        // Return descriptive code for the function called.

    int arg() const { return d_arg; }
        // Return last argument value for allocate.

    int allocateCount() const { return d_allocateCount; }
        // Return number of times  allocate called.

    int deallocateCount() const { return d_deallocateCount; }
        // Return number of times  deallocate called.
};

class my_NewDeleteAllocator : public bdema_Allocator {
  // Test class used to verify examples.
    int d_count;

  public:
    my_NewDeleteAllocator(): d_count(0) { }
    ~my_NewDeleteAllocator() { }

    void *allocate(size_type size)  {
        ++d_count;
        void *p = operator new(size);
        return p;
    }

    void deallocate(void *address)  {
        ++d_count;
        operator delete(address);
    }

    int getCount() const            { return d_count; }
};

//=============================================================================
//                   CONCRETE OBJECTS FOR TESTING 'deleteObject'
//-----------------------------------------------------------------------------
static int globalObjectStatus = 0;    // global flag set by test-object d'tors
static int class3ObjectCount = 0;     // Count set by my_Class3 c'tor/d'tor

class my_Class1 {
  public:
    my_Class1() { globalObjectStatus = 1; }
    ~my_Class1() { globalObjectStatus = 0; }
};

class my_Class2 {
  public:
    my_Class2() { globalObjectStatus = 1; }
    virtual ~my_Class2() { globalObjectStatus = 0; }
};

class my_Class3Base {
  public:
    my_Class3Base() { }
    virtual ~my_Class3Base() = 0;
};

class my_Class3 : public my_Class3Base {
  public:
    my_Class3() { ++class3ObjectCount; }
    virtual ~my_Class3();
};

my_Class3Base::~my_Class3Base() { }
my_Class3::~my_Class3() { --class3ObjectCount; }

// The "dreaded diamond".

static int virtualBaseObjectCount = 0;
static int leftBaseObjectCount    = 0;
static int rightBaseObjectCount   = 0;
static int mostDerivedObjectCount = 0;

class my_VirtualBase {
    int x;
public:
    my_VirtualBase()          { virtualBaseObjectCount = 1; }
    virtual ~my_VirtualBase() { virtualBaseObjectCount = 0; }
};

class my_LeftBase : virtual public my_VirtualBase {
    int x;
public:
    my_LeftBase()             { leftBaseObjectCount = 1; }
    virtual ~my_LeftBase()    { leftBaseObjectCount = 0; }
};

class my_RightBase : virtual public my_VirtualBase {
    int x;
public:
    my_RightBase()            { rightBaseObjectCount = 1; }
    virtual ~my_RightBase()   { rightBaseObjectCount = 0; }
};

class my_MostDerived : public my_LeftBase, public my_RightBase {
    int x;
public:
    my_MostDerived()          { mostDerivedObjectCount = 1; }
    ~my_MostDerived()         { mostDerivedObjectCount = 0; }
};

//=============================================================================
//                              USAGE EXAMPLE
//-----------------------------------------------------------------------------
class my_DoubleStack {
    double *d_stack_p; // dynamically allocated array (d_size elements)
    int d_size;        // physical capacity of this stack (in elements)
    int d_length;      // logical index of next available stack element
    bdema_Allocator *d_allocator_p; // holds (but doesn't own) object

    friend class my_DoubleStackIter;

  private:
    void increaseSize(); // Increase the capacity by at least one element.

  public:
    // CREATORS
    my_DoubleStack(bdema_Allocator *basicAllocator = 0);
    my_DoubleStack(const my_DoubleStack& other,
                   bdema_Allocator *basicAllocator = 0);
    ~my_DoubleStack();

    // MANIPULATORS
    my_DoubleStack& operator=(const my_DoubleStack& rhs);
    void push(double value);
    void pop();

    // ACCESSORS
    const double& top() const;
    int isEmpty() const;
};

enum { INITIAL_SIZE = 1, GROW_FACTOR = 2 };

// ...

my_NewDeleteAllocator myA;

my_DoubleStack::my_DoubleStack(bdema_Allocator *basicAllocator)
: d_size(INITIAL_SIZE)
, d_length(0)
, d_allocator_p(basicAllocator ? basicAllocator : &myA)
    // The above initialization expression is equivalent to 'basicAllocator
    // ? basicAllocator : &bslma_NewDeleteAllocator::singleton()'.
{
    ASSERT(d_allocator_p);
    d_stack_p = (double *) d_allocator_p->allocate(d_size * sizeof *d_stack_p);
}

my_DoubleStack::~my_DoubleStack()
{
    // CLASS INVARIANTS
    ASSERT(d_allocator_p);
    ASSERT(d_stack_p);
    ASSERT(0 <= d_length);
    ASSERT(0 <= d_size);
    ASSERT(d_length <= d_size);

    d_allocator_p->deallocate(d_stack_p);
}

inline void my_DoubleStack::push(double value)
{
    if (d_length >= d_size) {
        increaseSize();
    }
    d_stack_p[d_length++] = value;
}

inline static
void reallocate(double **array, int newSize, int length,
                bdema_Allocator *basicAllocator)
    // Reallocate memory in the specified 'array' to the specified
    // 'newSize' using the specified 'basicAllocator'.  The specified
    // 'length' number of leading elements are preserved.  Since the
    //  class invariant requires that the physical capacity of the
    // container may grow but never shrink; the behavior is undefined
    // unless length <= newSize.
{
    ASSERT(array);
    ASSERT(1 <= newSize);
    ASSERT(0 <= length);
    ASSERT(basicAllocator);
    ASSERT(length <= newSize);        // enforce class invariant

    double *tmp = *array;             // support exception neutrality
    *array = (double *) basicAllocator->allocate(newSize * sizeof **array);

    // COMMIT POINT

    memcpy(*array, tmp, length * sizeof **array);
    basicAllocator->deallocate(tmp);
}

void my_DoubleStack::increaseSize()
{
     int proposedNewSize = d_size * GROW_FACTOR;      // reallocate can throw
     ASSERT(proposedNewSize > d_length);
     reallocate(&d_stack_p, proposedNewSize, d_length, d_allocator_p);
     d_size = proposedNewSize;                        // we're committed
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//           Additional Functionality Need to Complete Usage Test Case

class my_DoubleStackIter {
    const double *d_stack_p;
    int d_index;
  private:
    my_DoubleStackIter(const my_DoubleStackIter&);
    my_DoubleStackIter& operator=(const my_DoubleStackIter&);
  public:
    my_DoubleStackIter(const my_DoubleStack& stack)
    : d_stack_p(stack.d_stack_p), d_index(stack.d_length - 1) { }
    void operator++() { --d_index; }
    operator const void *() const { return d_index >= 0 ? this : 0; }
    const double& operator()() const { return d_stack_p[d_index]; }
};

ostream& operator<<(ostream& stream, const my_DoubleStack& stack)
{
    stream << "(top) [";
    for (my_DoubleStackIter it(stack); it; ++it) {
        stream << ' ' << it();
    }
    return stream << " ] (bottom)" << flush;
}

//-----------------------------------------------------------------------------
//                      OVERLOADED OPERATOR NEW USAGE EXAMPLE
//-----------------------------------------------------------------------------

class my_Type {
    char *d_stuff_p;
    bdema_Allocator *d_allocator_p;

  public:
    my_Type(int size, bdema_Allocator *basicAllocator)
    : d_allocator_p(basicAllocator)
    {
        d_stuff_p = (char *) d_allocator_p->allocate(size);
    }

    ~my_Type()
    {
        d_allocator_p->deallocate(d_stuff_p);
    }
};

my_Type *newMyType(bdema_Allocator *basicAllocator) {
    return new (*basicAllocator) my_Type(5, basicAllocator);
}
void deleteMyType(bdema_Allocator *basicAllocator, my_Type *t) {
    t->~my_Type();
    basicAllocator->deallocate(t);
}

//-----------------------------------------------------------------------------
// HELPER CLASS FOR TESTING EXCEPTION SAFETY OF OVERLOADED OPERATOR NEW
//-----------------------------------------------------------------------------

class my_ClassThatMayThrowFromConstructor {
    char d_c;
  public:
    my_ClassThatMayThrowFromConstructor()
    {
#ifdef BDE_BUILD_TARGET_EXC
        throw int(13);
#endif
    }

    ~my_ClassThatMayThrowFromConstructor()
    {
    }
};

//=============================================================================
//                                 MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    // int veryVerbose = argc > 3;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
      case 5: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE
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

        if (verbose) cout << "\nTesting Usage Example"
                          << "\n=====================" << endl;

        if (verbose) cout << "\nMain example usage test." << endl;

        if (verbose) cout <<
                "\tCreating a stack with a specified allocator." << endl;
        {
            my_NewDeleteAllocator myA;
            bdema_Allocator& a = myA;
            my_DoubleStack s(&a);
            s.push(1.25);
            s.push(1.5);
            s.push(1.75);

            if (verbose) {
                cout << "\t\t" << s << endl;
            }
        }

        if (verbose) cout <<
                "\tCreating a stack without a specified allocator." << endl;
        {
            my_DoubleStack s;
            s.push(2.25);
            s.push(2.5);
            s.push(2.75);

            if (verbose) {
                cout << "\t\t" << s << endl;
            }
        }

        if (verbose) cout << "\nUsage test for 'new' operator." << endl;
        {
            my_NewDeleteAllocator myA;
            bdema_Allocator& a = myA;
            my_Type *t = newMyType(&a);
            deleteMyType(&a, t);
        }

      } break;
      case 4: {
        // --------------------------------------------------------------------
        // EXCEPTION SAFETY OF OPERATOR NEW TEST:
        //   We want to make sure that when the overloaded operator new
        //   is invoked and the constructor of the new object throws an
        //   exception, the overloaded delete operator is invoked
        //   automatically to deallocate the object.
        //
        // Plan:
        //   Invoke the 'operator new' for a class which throws exception from
        //   the constructor.  Catch the exception and verify that deallocation
        //   was performed automatically.
        //
        // Testing:
        //   void *operator new(int size, bdema_Allocator& basicAllocator);
        //   void operator delete(void *address, bdema_Allocator& bA);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "EXCEPTION SAFETY OF OPERATOR NEW TEST" << endl
                          << "=====================================" << endl;
        {
            my_Allocator myA;
            bdema_Allocator& a = myA;

            my_ClassThatMayThrowFromConstructor *p=0;
#ifdef BDE_BUILD_TARGET_EXC
            try
#endif
            {
                p = new (a) my_ClassThatMayThrowFromConstructor;
                p->~my_ClassThatMayThrowFromConstructor();
                a.deallocate(p);
                p = 0;
            }
#ifdef BDE_BUILD_TARGET_EXC
            catch(int n)
            {
                if (verbose) cout << "\nCaught exception." << endl;
                ASSERT(13 == n);
            }
#endif
            ASSERT(!p);
            ASSERT(2 == myA.fun());
            ASSERT(1 == myA.allocateCount());
            ASSERT(1 == myA.deallocateCount());
        }

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // OPERATOR TEST:
        //   We want to make sure that the correct underlying method is
        //   called based on the type of the overloaded 'new' operator.
        //
        // Plan:
        //   Invoke 'operator new' for types of various size.  Verify that
        //   correct parameters are passed to allocate method.
        //
        // Testing:
        //   void *operator new(int size, bdema_Allocator& basicAllocator);
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "OPERATOR TEST" << endl
                                  << "=============" << endl;
        my_Allocator myA;
        bdema_Allocator& a = myA;


        if (verbose) cout << "\nTesting scalar input operators." << endl;
        {
            ASSERT((char *) &myA == new(a) char);
            ASSERT(1 == myA.fun());     ASSERT(1 == myA.arg());

            ASSERT((short *) &myA == new(a) short);
            ASSERT(1 == myA.fun());     ASSERT(sizeof(short) == myA.arg());

            ASSERT((int *) &myA == new(a) int);
            ASSERT(1 == myA.fun());     ASSERT(sizeof(int) == myA.arg());

            ASSERT((double *) &myA == new(a) double);
            ASSERT(1 == myA.fun());     ASSERT(sizeof(double) == myA.arg());

            struct X15 { char d_z[15]; };       ASSERT(15 == sizeof(X15));
            ASSERT((X15 *) &myA == new(a) X15);
            ASSERT(1 == myA.fun());     ASSERT(15 == myA.arg());
        }

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // MEMBER TEMPLATE METHOD 'deleteObject' TEST:
        //   We want to make sure that when 'deleteObj' is used both
        //   destructor and 'deallocate' are invoked.
        //
        // Plan:
        //   Using an allocator and placement new operator construct objects of
        //   two different classes.  Invoke 'deleteObject' to delete constructed
        //   objects and check that both destructor and 'deallocate' have been
        //   called.  Repeat tests with a derived-class object with a virtual
        //   destructor.  Test with null pointer.
        //
        // Testing:
        //   template<typename TYPE> deleteObject(const TYPE *)
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "'deleteObject' TEST" << endl
                                  << "===================" << endl;

        if (verbose) cout << "\nTesting 'deleteObject':" << endl;
        {
            my_NewDeleteAllocator myA;  bdema_Allocator& a = myA;

            if (verbose) cout << "\twith a my_Class1 object" << endl;

            if (verbose) { T_();  T_();  P(globalObjectStatus); }
            ASSERT(0 == globalObjectStatus);

            ASSERT(0 == myA.getCount());
            my_Class1 *pC1 = (my_Class1 *) a.allocate(sizeof(my_Class1));
            const my_Class1 *pC1CONST = pC1;
            ASSERT(1 == myA.getCount());

            new(pC1) my_Class1;
            if (verbose) { T_();  T_();  P(globalObjectStatus); }
            ASSERT(1 == globalObjectStatus);

            ASSERT(1 == myA.getCount());
            a.deleteObject(pC1CONST);
            if (verbose) { T_();  T_();  P(globalObjectStatus); }
            ASSERT(0 == globalObjectStatus);   ASSERT(2 == myA.getCount());

            if (verbose) cout << "\twith a my_Class2 object" << endl;

            if (verbose) { T_();  T_();  P(globalObjectStatus); }
            ASSERT(0 == globalObjectStatus);

            ASSERT(2 == myA.getCount());
            my_Class2 *pC2 = (my_Class2 *) a.allocate(sizeof(my_Class2));
            const my_Class2 *pC2CONST = pC2;
            ASSERT(3 == myA.getCount());

            new(pC2) my_Class2;
            if (verbose) { T_();  T_();  P(globalObjectStatus); }
            ASSERT(1 == globalObjectStatus);

            ASSERT(3 == myA.getCount());
            a.deleteObject(pC2CONST);
            if (verbose) { T_();  T_();  P(globalObjectStatus); }
            ASSERT(0 == globalObjectStatus);   ASSERT(4 == myA.getCount());

            if (verbose) cout << "\tWith a my_Class3Base object" << endl;

            ASSERT(0 == class3ObjectCount);
            my_Class3 *pC3 = (my_Class3 *) a.allocate(sizeof(my_Class3));
            const my_Class3Base *pC3bCONST = pC3;
            ASSERT(0 == class3ObjectCount);
            ASSERT(0 == globalObjectStatus);
            ASSERT(5 == myA.getCount());

            new(pC3) my_Class3;
            if (verbose) { T_();  T_();  P(class3ObjectCount); }
            ASSERT(1 == class3ObjectCount);
            ASSERT(0 == globalObjectStatus);

            ASSERT(5 == myA.getCount());
            a.deleteObject(pC3bCONST);
            if (verbose) { T_();  T_();  P(class3ObjectCount); }
            ASSERT(0 == class3ObjectCount);
            ASSERT(0 == globalObjectStatus);
            ASSERT(6 == myA.getCount());

            if (verbose) cout << "\tWith a null my_Class3 pointer" << endl;

            pC3 = 0;
            a.deleteObject(pC3);
            if (verbose) { T_();  T_();  P(class3ObjectCount); }
            ASSERT(0 == class3ObjectCount);
            ASSERT(0 == globalObjectStatus);
            ASSERT(6 == myA.getCount());
        }
        {
            my_NewDeleteAllocator myA;  bdema_Allocator& a = myA;

            if (verbose) cout << "\tdeleteObject(my_MostDerived*)" << endl;

            ASSERT(0 == myA.getCount());
            my_MostDerived *pMost =
                         (my_MostDerived *) a.allocate(sizeof(my_MostDerived));
            const my_MostDerived *pMostCONST = pMost;
            ASSERT(1 == myA.getCount());

            ASSERT(0 == mostDerivedObjectCount);
            ASSERT(0 == rightBaseObjectCount);
            ASSERT(0 == leftBaseObjectCount);
            ASSERT(0 == virtualBaseObjectCount);
            new(pMost) my_MostDerived;
            ASSERT(1 == mostDerivedObjectCount);
            ASSERT(1 == rightBaseObjectCount);
            ASSERT(1 == leftBaseObjectCount);
            ASSERT(1 == virtualBaseObjectCount);

            a.deleteObject(pMostCONST);
            ASSERT(2 == myA.getCount());
            ASSERT(0 == mostDerivedObjectCount);
            ASSERT(0 == rightBaseObjectCount);
            ASSERT(0 == leftBaseObjectCount);
            ASSERT(0 == virtualBaseObjectCount);

            if (verbose) cout << "\tdeleteObject(my_LeftBase*)" << endl;

            pMost = (my_MostDerived *) a.allocate(sizeof(my_MostDerived));
            ASSERT(3 == myA.getCount());

            new(pMost) my_MostDerived;
            const my_LeftBase *pLeftCONST = pMost;
            ASSERT(1 == mostDerivedObjectCount);
            ASSERT(1 == rightBaseObjectCount);
            ASSERT(1 == leftBaseObjectCount);
            ASSERT(1 == virtualBaseObjectCount);

            a.deleteObject(pLeftCONST);
            ASSERT(4 == myA.getCount());
            ASSERT(0 == mostDerivedObjectCount);
            ASSERT(0 == rightBaseObjectCount);
            ASSERT(0 == leftBaseObjectCount);
            ASSERT(0 == virtualBaseObjectCount);

            if (verbose) cout << "\tdeleteObject(my_RightBase*)" << endl;

            pMost = (my_MostDerived *) a.allocate(sizeof(my_MostDerived));
            ASSERT(5 == myA.getCount());

            new(pMost) my_MostDerived;
            const my_RightBase *pRightCONST = pMost;
            ASSERT(1 == mostDerivedObjectCount);
            ASSERT(1 == rightBaseObjectCount);
            ASSERT(1 == leftBaseObjectCount);
            ASSERT(1 == virtualBaseObjectCount);

            a.deleteObject(pRightCONST);
            ASSERT(6 == myA.getCount());
            ASSERT(0 == mostDerivedObjectCount);
            ASSERT(0 == rightBaseObjectCount);
            ASSERT(0 == leftBaseObjectCount);
            ASSERT(0 == virtualBaseObjectCount);

            if (verbose) cout << "\tdeleteObject(my_VirtualBase*)" << endl;

            pMost = (my_MostDerived *) a.allocate(sizeof(my_MostDerived));
            ASSERT(7 == myA.getCount());

            new(pMost) my_MostDerived;
            const my_VirtualBase *pVirtualCONST = pMost;
            ASSERT(1 == mostDerivedObjectCount);
            ASSERT(1 == rightBaseObjectCount);
            ASSERT(1 == leftBaseObjectCount);
            ASSERT(1 == virtualBaseObjectCount);

            a.deleteObject(pVirtualCONST);
            ASSERT(8 == myA.getCount());
            ASSERT(0 == mostDerivedObjectCount);
            ASSERT(0 == rightBaseObjectCount);
            ASSERT(0 == leftBaseObjectCount);
            ASSERT(0 == virtualBaseObjectCount);
        }

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // PROTOCOL TEST:
        //   All we need to do is make sure that a subclass of the
        //   'bdema_Allocator' class compiles and links when all virtual
        //   functions are defined.
        //
        // Plan:
        //   Construct an object of a class derived from 'bdema_Allocator'.
        //   Up-cast a reference to the object to the base class
        //   'bdema_Allocator'.  Using the base class reference invoke both
        //   'allocate' and 'deallocate' methods.  Verify that the correct
        //   implementations of the methods are called.
        //
        // Testing:
        //   virtual ~bdema_Allocator();
        //   virtual void *allocate(size_type size) = 0;
        //   virtual void deallocate(void *address) = 0;
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "PROTOCOL TEST" << endl
                                  << "=============" << endl;
        my_Allocator myA;
        bdema_Allocator& a = myA;

        if (verbose) cout << "\nTesting allocate/deallocate" << endl;
        {
            ASSERT(&myA == a.allocate(100));    ASSERT(1 == myA.fun());
            a.deallocate(&myA);                 ASSERT(2 == myA.fun());
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
//      Copyright (C) Bloomberg L.P., 2004
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
