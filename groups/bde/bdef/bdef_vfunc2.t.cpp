// bdef_vfunc2.t.cpp                                                  -*-C++-*-

#include <bdef_vfunc2.h>

#include <bslma_default.h>
#include <bslma_testallocator.h>                // for testing only

#include <bsl_iostream.h>
#include <bsl_c_stdlib.h>     // atoi()

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// The 'bdef_Vfunc2' class holds a pointer to an instance of a 'bdefr_Vfunc2'
// (an abstact class), and manipulates it appropriately.  Our main objective
// is to make sure that the 'bdefr_Vfunc2' object is initialized correctly and
// that its integer data (the reference count) is modified by the methods of
// 'bdef_Vfunc2' as expected.
//
// Note that places where test drivers in this family are likely to require
// adjustment are indicated by the tag: "ADJ".
//
// We use the following abbreviations:
//  'A1-N'    for 'A1, A2, ... , AN'
//  'c_A1'    for 'const A1'
//  'c_A1-N   for 'const A1, const A2, ... ,  const AN'
//  'c_A1-N&' for 'const A1&, const A2&, ... , const AN&'
//
//-----------------------------------------------------------------------------
// [ 1] bdef_Vfunc2<A1-2>();
// [ 1] bdef_Vfunc2<A1-2>(bdefr_Vfunc2<A1-2> *);
// [ 1] bdef_Vfunc2<A1-2>(const bdef_Vfunc2<A1-2>&);
// [ 1] ~bdef_Vfunc2<A1-2>();
// [ 2] bdef_Vfunc2<A1-2>& operator=(const bdef_Vfunc2<A1-2>&);
// [ 1] void operator()(c_A1-2&) const;
// [ 1] operator const void *() const;
//-----------------------------------------------------------------------------
// [ 3] USAGE EXAMPLE

//==========================================================================
//                  STANDARD BDE ASSERT TEST MACRO
//--------------------------------------------------------------------------
static int testStatus = 0;

static void aSsErT(int c, const char *s, int i)
{
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}

# define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", "<< flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number

//=============================================================================
//                  CONCRETE DERIVED CLASS FOR TESTING
//-----------------------------------------------------------------------------

template <class F, class A1, class A2, class D1, class D2>
class FunctorRep : public bdefr_Vfunc2<A1, A2> {
    // This class defines the representation for a function object (functor),
    // characterized by a function-call operator taking two arguments and
    // returning 'void', that holds a pure procedure (i.e., free function,
    // static member function, or functor) taking 2 additional trailing
    // arguments, and the arguments' corresponding values.
    //
    // It mimics the behavior of classes in 'bdefi' with one exception: it
    // allows to count the number of time the 'execute' function has been
    // called, which is used to verify that different functors, that are
    // supposed to share the implementation (as a result of copy constrution
    // or assignment) indeed share the implementation.

    F  d_f;  // function pointer or function object (functor)
    D1 d_d1; // first embedded argument
    D2 d_d2; // second embedded argument
    int d_invocationCounter;    // invocation counter
    int *d_invocationCounter_p; // pointer to the invocation counter.  Since
                                // the counter must be incremented in the
                                // 'const' method 'execute', we increment it
                                // via this pointer.

  private:
    // not implemented
    FunctorRep(const FunctorRep<F, A1, A2, D1, D2>&);
    FunctorRep<F, A1, A2, D1, D2>&
                           operator=(const FunctorRep<F, A1, A2, D1, D2>&);
  private:
    inline ~FunctorRep();
        // Destroy this functor.  This destructor can be called only through
        // the static 'deleteObject' method of the base class.

  public:
    // CREATORS
    inline FunctorRep(F                procedure,
                      const D1&        embeddedArg1,
                      const D2&        embeddedArg2,
                      bslma_Allocator *basicAllocator);
        // Create a representation for a function object (functor) taking two
        // arguments and returning 'void', using the specified 'procedure'
        // (i.e., free function, static member function, or functor) taking 2
        // additional trailing arguments, and the corresponding arguments'
        // specified 'embeddedArg1' and 'embeddedArg2' values.  Use the
        // specified 'basicAllocator' to supply memory.  Note that any value
        // returned from 'procedure' will be ignored.

    // ACCESSORS
    inline void execute(const A1& argument1, const A2& argument2) const;
        // Invoke the underlying procedure (free function, static member
        // function, or functor) with the specified 'argument1' and
        // 'argument2' along with the sequence of 2 argument values specified
        // at construction.  Increase the invocation counter.

    int invocationCounter() const;
        // Return the number of times the 'execute' method was invoked.
};

template <class F, class A1, class A2, class D1, class D2>
inline FunctorRep<F, A1, A2, D1, D2>::FunctorRep(
                                             F                procedure,
                                             const D1&        embeddedArg1,
                                             const D2&        embeddedArg2,
                                             bslma_Allocator *basicAllocator)
: bdefr_Vfunc2<A1, A2>(basicAllocator)
, d_f(procedure)
, d_d1(embeddedArg1)
, d_d2(embeddedArg2)
, d_invocationCounter(0)
{
  d_invocationCounter_p = &d_invocationCounter;
}

template <class F, class A1, class A2, class D1, class D2>
inline FunctorRep<F, A1, A2, D1, D2>::~FunctorRep()
{
}

template <class F, class A1, class A2, class D1, class D2> inline
void FunctorRep<F, A1, A2, D1, D2>::execute(const A1& argument1,
                                            const A2& argument2) const
{
    d_f(argument1, argument2, d_d1, d_d2);
    ++*d_invocationCounter_p;
}

template <class F, class A1, class A2, class D1, class D2> inline
int FunctorRep<F, A1, A2, D1, D2>::invocationCounter() const
{
    return d_invocationCounter;
}

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

    typedef void (*parmMatchingFunc)(int*, int*, int* ,int*);
    typedef FunctorRep<parmMatchingFunc, int*, int*, int*, int*>
                                                        ParmMatchingFunctorImp;
    typedef bdefr_Vfunc2<int *, int *> ObjRep;
    typedef bdef_Vfunc2<int *, int *> Obj;

    const int ARG1       = 1;
    const int ARG2       = 2;
    const int ARG3       = 3;
    const int ARG4       = 4;
    const int ARG1_PLUS1 = ARG1 + 1;

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

static void checkParameterMatching(int *a, int *b, int *c, int *d)
    // Invoke the global 'ASSERT' macro to verify that the specified arguments
    // 'a', 'b', 'c', and 'd' are equal the global values 'PARAM1', 'PARAM2',
    // 'PARAM3', and 'PARAM4, respectively.  Then increment the 'a' by one.
    //
    // This function is used to test the correctness of parameter passing.  We
    // always call 'checkParameterMatching' as 'checkParameterMatching(ARG1,
    // ARG2, ARG3, ARG4);' If the parameters are passed in the wrong order,
    // the ASSERT statement will catch the bug.  The last statement
    // incrementing '*a' allows the caller to verify that the function was
    // actually called and that the 'ASSERT' statements were in fact executed.
{
        ASSERT(*a == ARG1);
        ASSERT(*b == ARG2);
        ASSERT(*c == ARG3);
        ASSERT(*d == ARG4);
        ++(*a);
}

static int getCount(ObjRep *rep)
    // Return the current count of the specified 'object'.
{
    // Since we can retrieve the value of the count only by calling the
    // 'decrement' method, we will use a composition of the 'increment' and
    // 'decrement' functions to retrieve the value of 'd_count' yet still
    // preserve its current value.

    rep->increment();
    return rep->decrement();
}

//=============================================================================
//                 USAGE EXAMPLE CLASSES AND FUNCTIONS
//-----------------------------------------------------------------------------

class MyGuiContext {
    int d_changedFlag;

   public:
     MyGuiContext():d_changedFlag(0) { }
     int isChanged() { return d_changedFlag; }
     void changeState() { ++d_changedFlag; }

     // 'MyGuiContext' implementation
};

class MyGuiLocation {
  public:
    MyGuiLocation() { };
    // 'MyGuiLocation' implementation
};

class MyGuiButton  {
    bdef_Vfunc2<MyGuiContext *, MyGuiLocation> d_callback;
        // Functor to execute when button is pressed.

  public:
    MyGuiButton(const bdef_Vfunc2<MyGuiContext *,
                MyGuiLocation>& buttonPressCallback);
        // Create a graphical button object that executes the specified
        // callback when 'pressButton' is invoked.

    void pressButton(MyGuiContext         *context,
                     const MyGuiLocation&  location);
        // Execute the callback owned by this button object.
};

MyGuiButton::MyGuiButton(const bdef_Vfunc2<MyGuiContext *,
                         MyGuiLocation>& buttonPressCallback)
: d_callback(buttonPressCallback) // Retain copy of specified functor.
{
}

void MyGuiButton::pressButton(MyGuiContext         *context,
                              const MyGuiLocation&  location)
{
    d_callback(context, location);
        // Execute the contained callback object.
}

static void buttonpressFunction(MyGuiContext         *a,
                                const MyGuiLocation&  b,
                                int                  *invokationCounter)
    // This function will be invoked by a functor to increment the specified
    // 'invokationCounter'.
{
        a->changeState();
        ++(*invokationCounter);
}

template <class F, class A1, class A2, class D1>
class FuncRep : public bdefr_Vfunc2<A1, A2> {
    // This class defines the representation for a function object (functor),
    // characterized by a function-call operator taking two arguments and
    // returning 'void', that holds a pure procedure (i.e., free function,
    // static member function, or functor) taking one additional trailing
    // argument, and this argument's corresponding value.

    F  d_f;  // function pointer or function object (functor)
    D1 d_d1; // first embedded argument

  private:
    // not implemented
    FuncRep(const FuncRep<F, A1, A2, D1>&);
    FuncRep<F, A1, A2, D1>& operator=(const FuncRep<F, A1, A2, D1>&);

  private:
    ~FuncRep()
        // Destroy this functor.  Note that this destructor can be invoked only
        // through the static 'deleteObject' method of the base class.
    {
    };

  public:
    // CREATORS
    FuncRep(F                procedure,      const D1& embeddedArg1,
            bslma_Allocator *basicAllocator)
    : bdefr_Vfunc2<A1, A2>(basicAllocator)
    , d_f(procedure)
    , d_d1(embeddedArg1)
        // Create a representation for a function object (functor) taking two
        // arguments and returning 'void', using the specified 'procedure'
        // (i.e., free function, static member function, or functor) taking 1
        // additional trailing argument, and this argument's specified
        // 'embeddedArg1' value.  Use the specified 'basicAllocator' to supply
        // memory.
    {
    };

    // ACCESSORS
    void execute(const A1& argument1, const A2& argument2) const
        // Invoke the underlying procedure (free function, static member
        // function, or functor) with the specified 'argument1' and
        // 'argument2' along with the argument value specified at construction.
    {
        d_f(argument1, argument2, d_d1);
    };
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

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    bslma_TestAllocator testAllocator(veryVeryVerbose);

    switch (test) { case 0:  // Zero is always the leading case.
      case 3: {
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

        if (verbose) cout << endl
                          << "Testing Usage Example" << endl
                          << "=====================" << endl;
        {
          // (1) Create the representation.
          typedef void (*BpFun)(MyGuiContext *,
                                const MyGuiLocation&, int *);
          bslma_Allocator *myAllocator =
              bslma_Default::defaultAllocator();

          int globalCounter = 0;

          bdefr_Vfunc2<MyGuiContext *, MyGuiLocation>  *rep = new(*myAllocator)
              FuncRep<BpFun, MyGuiContext *, MyGuiLocation, int*>
                  (buttonpressFunction, &globalCounter, myAllocator);

          // (2) Create the functor using the representation.
          bdef_Vfunc2<MyGuiContext *, MyGuiLocation> callbackFunctor(rep);

          // (3) Register the functor as a callback.
          MyGuiButton button(callbackFunctor);

          // (4) Use the object.
          MyGuiContext gc;
          const MyGuiLocation gl;             ASSERT(0 == globalCounter);
                                              ASSERT(0 == gc.isChanged());
          button.pressButton(&gc, gl);        ASSERT(1 == globalCounter);
                                              ASSERT(1 == gc.isChanged());
          button.pressButton(&gc, gl);        ASSERT(2 == globalCounter);
                                              ASSERT(2 == gc.isChanged());
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING ASSIGNMENT OPERATOR
        //
        //   We need to test the following six cases:
        //    a. Assignment to uninitialized object from unitialized object.
        //    b. Assignment to initialized object from unitialized object.
        //    c. Assignment of unitialized object to self.
        //    d. Assignment to uninitialized object from itialized object.
        //    e. Assignment to initialized object from itialized object.
        //    f. Assignment of itialized object to self.
        //
        //   We have the following specific concerns for each case:
        //    1. The assignment operator fully initializes the object
        //       whose function can be detected with 'operator const void *()
        //       const', and executed with 'void operator()<A1, A2>(const A1&,
        //       const A2&) const'
        //    2. A new object shares a 'bdefr_Vfunc2' object with its "right
        //       hand side (or both are null)."
        //    3. The reference counting works as expected.
        //
        // Plan:
        //   Create an object 'Obj1' using the default constructor.
        //    1a. Create an object 'Obj2' using the default constructor.
        //        Execute the assignment operator using 'Obj1' as the
        //        right-hand-side.  Execute 'operator const void *() const'.
        //    2a. N/A.
        //    3a. N/A.
        //
        //   Create an object 'Obj1' using the default constructor.  Create a
        //   'bdefr_Vfunc2<A1, A2> *' object 'R'.  Call the constructor to
        //   create 'bdef_Vfunc2' objects 'Obj2' and 'Obj3' using 'R'.
        //    1b. Assign 'Obj1' to 'Obj2'.  Execute
        //        'operator const void *() const'.
        //    2b. N/A.
        //    3b. Check the reference counter using the 'getCount' function.
        //        Assign 'Obj1' to 'Obj3'.  Using 'bslma_TestAllocator' verify
        //        that the memory allocated for 'R' was deallocated.
        //
        //   Create an object 'Obj1' using the default constructor.
        //    1c. Assign 'Obj1' to 'Obj1'.
        //        Execute 'operator const void *() const'.
        //    2c. N/A.
        //    3c. N/A.
        //
        //   Create the 'bdefr_Vfunc2<A1, A2> *' object.  Call the constructor
        //   to create the 'bdef_Vfunc2' object 'Obj1'.
        //    1d. Create an object 'Obj2' using the default constructor.
        //        Execute the assignment operator using 'Obj1' as the
        //        right-hand-side.  Execute 'operator const void *() const',
        //        and 'void operator()<A1, A2>(const A1&, const A2&) const' of
        //        the newly assigned object.
        //    2d. Use 'invocationCounter' method of the 'FunctorRep' class
        //        to verify that 'Obj1' and 'Obj2' use the same 'bdefr_Vfunc2'
        //        object.
        //    3d. Check the reference counter using the 'getCount' function.
        //        Create another object and check it again.
        //
        //   Create two 'bdefr_Vfunc2<A1, A2> *' objects 'R1' and 'R2'.  Create
        //   two 'bdef_Vfunc2' objects 'Obj1' and 'Obj2' using 'R1' and 'R2',
        //   respectively.
        //    1e. Create an object 'Obj3' using a copy constructor with 'Obj1'
        //        as an argument.  Assign 'Obj2' to 'Obj3'.  Execute 'operator
        //        const void *() const', and 'void operator()<A1, A2>(const
        //        A1&, const A2&) const' of 'Obj3'.
        //    2e. Use the 'invocationCounter' method of the 'FunctorRep' class
        //        to verify that 'Obj2' and 'Obj3' use the same 'bdefr_Vfunc2'
        //        object.
        //    3e. Check the reference counter using the 'getCount' function
        //        before and after the assignment for both 'Obj1' and 'Obj2'
        //        representations.
        //
        //   Create a 'bdefr_Vfunc2<A1, A2> *' object.  Call the constructor to
        //   create a 'bdef_Vfunc2' object 'Obj1'.
        //    1f. Assign 'Obj1' to 'Obj1'.
        //        Execute 'operator const void *() const', and 'void
        //        operator()<A1, A2>(const A1&, const A2&) const'
        //    2f. N/A.
        //    3f. Check the reference counter using the 'getCount' function.
        //        Using 'bslma_TestAllocator', test that memory was not freed
        //        and then allocated again.
        //
        // Testing:
        //   bdef_Vfunc2<A1-2>& operator=(const bdef_Vfunc2<A1-2>&);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing Assignment Operator" << endl
                          << "===========================" << endl;

        if (verbose)
            cout << "\nTesting an assignment from an empty object." << endl;

        if (verbose)
            cout << "\tTesting an assignment to an empty object." << endl;
        {
            Obj x1;
            Obj x2;
            x2 = x1;

            if (veryVerbose)
                cout << "\t\tTesting that functor initialization is "
                        "detectable." << endl;
            ASSERT(!x2);
        }

        if (verbose)
            cout << "\tTesting an assignment to an initialized object."
                 << endl;
        {
            Obj x1;

            // int a1 = ARG1;
            // int b1 = ARG2;
            int c1 = ARG3;
            int d1 = ARG4;
            ObjRep *rep1 = new(testAllocator)
                ParmMatchingFunctorImp(checkParameterMatching, &c1, &d1,
                                    &testAllocator);
            int numDeallocations = testAllocator.numDeallocations();

            Obj x2(rep1);
            Obj x3(rep1);

            ASSERT(2 == getCount(rep1));

            x2 = x1;

            if (veryVerbose)
                cout << "\t\tTesting that functor initialization is "
                        "detectable." << endl;
            ASSERT(!x2);

            if (veryVerbose)
                cout << "\t\tTesting reference counter." << endl;
            if (veryVerbose)
                cout << "\t\t\tDecreased to positive number." << endl;
            ASSERT(1 == getCount(rep1));

            if (veryVerbose)
                cout << "\t\t\tDecreased to '0'." << endl;
            ASSERT(numDeallocations == testAllocator.numDeallocations());
            x3 = x1;
            ASSERT(numDeallocations + 1 == testAllocator.numDeallocations());
            ASSERT(sizeof(ParmMatchingFunctorImp) ==
                   testAllocator.lastAllocatedNumBytes());
        }

        if (verbose) cout << "\tTesting an assignment to self." << endl;
        {
            Obj x1;
            x1 = x1;
            if (veryVerbose)
                cout << "\t\tTesting that functor initialization is "
                        "detectable." << endl;
            ASSERT(!x1);
        }

        if (verbose)
            cout << "\nTesting an assignment from non-empty object." << endl;

        if (verbose)
            cout << "\tTesting an assignment to an empty object." << endl;
        {
            int a = ARG1;
            int b = ARG2;
            int c = ARG3;
            int d = ARG4;
            ObjRep *rep1 = new(testAllocator)
                ParmMatchingFunctorImp(checkParameterMatching, &c, &d,
                                    &testAllocator);
            Obj x1(rep1);
            Obj x2;
            x2 = x1;

            if (veryVerbose)
                cout << "\t\tTesting that functor initialization is "
                        "detectable." << endl;
            ASSERT(x2);

            if (veryVerbose)
                cout << "\t\tTesting that the functor executes correctly."
                     << endl;
            x2(&a, &b);
            ASSERT(ARG1_PLUS1 == a);
            a = ARG1;

            if (veryVerbose)
                cout << "\t\tTesting that objects share representation."
                     << endl;
            ParmMatchingFunctorImp* rep1_2 = (ParmMatchingFunctorImp *)rep1;


            ASSERT(1 == rep1_2->invocationCounter());
            x1(&a, &b);
            a = ARG1;
            ASSERT(2 == rep1_2->invocationCounter());
            x2(&a, &b);
            a = ARG1;
            ASSERT(3 == rep1_2->invocationCounter());

            if (veryVerbose)
                cout << "\t\tTesting the reference counter." << endl;
            ASSERT(2 == getCount(rep1));
        }

        if (verbose)
            cout << "\tTesting assignment to an initialized object." << endl;
        {
            // int a1 = ARG1;
            // int b1 = ARG2;
            int c1 = ARG3;
            int d1 = ARG4;
            ObjRep *rep1 = new(testAllocator)
                ParmMatchingFunctorImp(checkParameterMatching, &c1, &d1,
                                    &testAllocator);
            Obj x1(rep1);

            int a2 = ARG1;
            int b2 = ARG2;
            int c2 = ARG3;
            int d2 = ARG4;
            ObjRep *rep2 = new(testAllocator)
                ParmMatchingFunctorImp(checkParameterMatching, &c2, &d2,
                                    &testAllocator);
            Obj x2(rep2);

            ASSERT(1 == getCount(rep1));
            ASSERT(1 == getCount(rep2));

            Obj x3(x1);
            x3 = x2;

            if (veryVerbose)
                cout << "\t\tTesting that functors share representation."
                     << endl;
            ParmMatchingFunctorImp* rep2_2 = (ParmMatchingFunctorImp *)rep2;

            ASSERT(0 == rep2_2->invocationCounter());
            x2(&a2, &b2);
            a2 = ARG1;
            ASSERT(1 == rep2_2->invocationCounter());
            x3(&a2, &b2);
            a2 = ARG1;
            ASSERT(2 == rep2_2->invocationCounter());

            if (veryVerbose)
                cout << "\t\tTesting that the functor executes correctly."
                     << endl;
            x3(&a2, &b2);
            ASSERT(ARG1_PLUS1 == a2);
            a2 = ARG1;

            if (veryVerbose)
                cout << "\t\tTesting reference counting." << endl;
            ASSERT(1 == getCount(rep1));
            ASSERT(2 == getCount(rep2));

            if (veryVerbose)
                cout << "\t\t\tWhen counter decreases to '0'." << endl;

            int numDeallocations = testAllocator.numDeallocations();
            x1 = x3;
            ASSERT(3 == getCount(rep2));
            ASSERT(numDeallocations + 1 == testAllocator.numDeallocations());
            ASSERT(sizeof(ParmMatchingFunctorImp) ==
                   testAllocator.lastAllocatedNumBytes());
        }

        if (verbose) cout << "\tTesting assignment to self." << endl;
        {
            int a = ARG1;
            int b = ARG2;
            int c = ARG3;
            int d = ARG4;
            ObjRep *rep1 = new(testAllocator)
                ParmMatchingFunctorImp(checkParameterMatching, &c, &d,
                                    &testAllocator);
            Obj x1(rep1);
            int numAlloc = testAllocator.numAllocations();
            int numDealloc = testAllocator.numDeallocations();
            x1 = x1;
            if (veryVerbose)
                cout << "\t\tTesting that ref. counter did not change."
                     << endl;
            ASSERT(1 == getCount(rep1));
            if (veryVerbose)
                cout << "\t\tTesting that memory was not newed/deleted."
                     << endl;
            ASSERT(numAlloc == testAllocator.numAllocations());
            ASSERT(numDealloc == testAllocator.numDeallocations());

            if (veryVerbose)
                cout << "\t\tTesting that functor initialization is visible."
                     << endl;
            ASSERT(x1);

            if (veryVerbose)
                 cout << "\t\tTesting that functor is executable." << endl;
            x1(&a, &b);
            ASSERT(ARG1_PLUS1 == a);
            a = ARG1;
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // TESTING CREATORS and ACCESSORS
        //   For each of the following creators:
        //    a. default constructor
        //    b. constructor taking a pointer to a 'bdefr_Vfunc2' object
        //    c. copy constructor
        //
        //   we want to verify that:
        //    1. Method creates a fully initialized instance.
        //    2. For each of the created objects, the function is detectable
        //       with 'operator const void *() const' and can be executed with
        //       'void operator()<A1, A2>(const A1&, const A2&) const'.
        //    3. The reference count of the 'bdefr_Vfunc2' object used is
        //       incremented.
        //
        //   Specific concerns for the copy constructor:
        //    4. A new object created by the copy constructor shares a
        //       'bdefr_Vfunc2' object with its copy.
        //
        //   Specific concerns for the destructor:
        //    5. The value of the reference counter is decreased when the
        //       envelope object is destroyed.
        //    6. The 'bdefr_Vfunc2<A1, A2>' object is destroyed when the
        //       reference counter reaches '0'.
        //
        // Plan:
        //   1a. Create an object using the default constructor.
        //   2a. Verify that 'd_rep_p' is initialized to '0' using
        //       'operator const void *() const'.
        //
        //   1b. Create the 'bdefr_Vfunc2<A1, A2> *' object.  Call the
        //       constructor taking it as an argument.
        //   2b. Check if the function is detectable with
        //       'operator const void *() const'.  Execute the function.
        //       Verify that the function was executed.
        //   3b. Verify that the reference counter was incremented using
        //       'getCount' method.  Create another object using the same
        //       'bdefr_Vfunc2<A1, A2> *' object.  Verify that the reference
        //       counter is incremented.
        //
        //   1c. Create an object using copy constructor.
        //   2c. Execute 'operator const void *() const', and
        //       'void operator()<A1, A2>(const A1&, const A2&) const' of the
        //       newly created object.
        //   3c. Verify that the reference counter was incremented using
        //       'getCount' function.
        //   4.  Use 'invocationCounter' method of the 'FunctorRep' class
        //       to verify that both 'bdef_Vfunc2' objects use the same
        //       'bdefr_Vfunc2' object.
        //   5.  Destroy all objects.  Verify that the reference counter is
        //   6.  decreased.  When the last object is destroyed check the
        //       memory allocator to verify that the memory was freed.
        //
        // Testing:
        //   bdef_Vfunc2(const bdef_Vfunc2 &rhs);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing Creators and Accessors" << endl
                          << "==============================" << endl;
        {
            if (verbose) cout << "\nTesting Default Constructor." << endl;
            Obj x1;

            if (veryVerbose)
                cout << "\tTesting that the functor rep. is initialized to '0'"
                     << endl;
            ASSERT(!x1);

            if (verbose)
                cout << "\nTesting Constructor Taking '*bdefr_Vfunc2'."
                     << endl;

            int a = ARG1;
            int b = ARG2;
            int c = ARG3;
            int d = ARG4;

            ObjRep *rep = new(testAllocator)
                ParmMatchingFunctorImp(checkParameterMatching, &c, &d,
                                    &testAllocator);
            ASSERT(sizeof(ParmMatchingFunctorImp) ==
                       testAllocator.numBytesInUse());
            {
                Obj x1(rep);

                if (veryVerbose)
                    cout << "\tTesting that the functor rep. is detectable."
                         << endl;
                ASSERT(x1);

                if (veryVerbose)
                    cout << "\tTesting the 'execute' method." << endl;
                x1(&a, &b);
                ASSERT(ARG1_PLUS1 == a);
                a = ARG1;
                if (veryVerbose)
                    cout << "\tTesting reference count." << endl;
                ASSERT(1 == getCount(rep));

                if (veryVerbose)
                    cout << "\tTesting that two objects share representation."
                         << endl;
                {
                    Obj x2(rep);
                    ParmMatchingFunctorImp *rep_2 =
                        (ParmMatchingFunctorImp *)rep;

                    ASSERT(1 == rep_2->invocationCounter());
                    x1(&a, &b);
                    a = ARG1;
                    ASSERT(2 == rep_2->invocationCounter());
                    x2(&a, &b);
                    a = ARG1;
                    ASSERT(3 == rep_2->invocationCounter());

                    if (veryVerbose)
                        cout << "\tTesting reference count." << endl;
                    ASSERT(2 == getCount(rep));

                    if (verbose) cout << "\nTest Copy Constructor." << endl;
                    {
                        Obj x3(x1);

                        if (veryVerbose)
                            cout << "\tTesting that the function is "
                                    "detectable." << endl;
                        ASSERT(x3);

                        if (veryVerbose)
                            cout << "\tTesting 'execute' method." << endl;
                        x3(&a, &b);
                        ASSERT(ARG1_PLUS1 == a);
                        a = ARG1;

                        if (veryVerbose)
                            cout << "\tTesting that two objects share "
                                    "representation." << endl;
                        ASSERT(4 == rep_2->invocationCounter());
                        if (veryVerbose)
                            cout << "\tTesting reference counting." << endl;
                        ASSERT(3 == getCount(rep));
                    }
                    ASSERT(2 == getCount(rep));
                }
                ASSERT(1 == getCount(rep));
                ASSERT(sizeof(ParmMatchingFunctorImp) ==
                       testAllocator.numBytesInUse());
            }
            if (veryVerbose)
                cout << "\tTesting reference counting when the counter is '0'."
                     << endl;
            ASSERT(0 == testAllocator.numBytesInUse());
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
//      Copyright (C) Bloomberg L.P., 2002
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
