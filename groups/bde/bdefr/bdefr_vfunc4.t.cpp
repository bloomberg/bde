// bdefr_vfunc4.t.cpp              -*-C++-*-

#include <bdefr_vfunc4.h>
#include <bslma_testallocator.h>                // for testing only
#include <bslma_default.h>           // for testing only

#include <bsl_iostream.h>
#include <bsl_c_stdlib.h>     // atoi()
using namespace BloombergLP;
using namespace bsl;  // automatically added by script



//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// 'bdefr_Vfunc4' is an abstract class that provides a pure virtual function
// 'execute', a reference counter 'd_count', and methods to modify it.  Each
// instance of this class holds a pointer to the memory allocator used to
// create it.  The class provides a static method to be used to destroy the
// object and subsequently deallocate it.
//
// This test verifies that member functions modify 'd_count' according to their
// specification and that the static 'deleteObject' method destroys the
// derived instance and successfully uses the memory allocator supplied at
// construction to deallocate the memory used by the destroyed object.
//
// We can observe the value of 'd_count' only through the return value of the
// 'decrement' method.  We will use a composition of the 'increment' and
// 'decrement' functions to retrieve the value of 'd_count'.
//
// We will use the API provided by bslma_TestAllocator to verify that
// 'deleteObject' deallocates memory as expected.
//
// In order to test 'bdefr_Vfunc4' we have to implement a concrete class
// derived from it.  This class automatically tests the correctness of the
// signature of the pure virtual function 'execute'.  We will use global
// variables to ensure that both 'execute' method and the virtual destructor
// are called.
//
// We use the following abbreviations:
//  'bA'      for 'bslma_Allocator'
//  'c_A1'    for 'const A1'
//  'c_A1-N   for 'const A1, const A2, ... ,  const AN'
//  'c_A1-N&' for 'const A1&, const A2&, ... , const AN&'
//  'c_D1'    for 'const D1'
//  'c_D1-N'  for 'const D1, const D2, ... , const DN'
//  'c_D1-N&' for 'const D1&, const D2&, ... , const DN&'
//
//-----------------------------------------------------------------------------
// [ 2] bdefr_Vfunc4<A1-4>(bslma_Allocator *ba);
// [ 2] void increment();
// [ 2] int decrement();
// [ 2] void execute(c_A1-4&) const;
// [ 2] ~bdefr_Vfunc4<A1-4>();
// [ 2] static void deleteObject(bdefr_Vfunc4 *object);
//-----------------------------------------------------------------------------
// [ 1] int getCount(Obj *object);
// [ 3] USAGE EXAMPLE

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

static int executeUsageCounter = 0;
static int dtorUsageCounter = 0;
template <class A1, class A2, class A3, class A4>
class ConcreteDerivedClass : public bdefr_Vfunc4 <A1, A2, A3, A4> {
  public:
    ConcreteDerivedClass(bslma_Allocator *basicAllocator)
    : bdefr_Vfunc4<A1, A2, A3, A4>(basicAllocator) { }

    virtual void execute(const A1& argument1, const A2& argument2,
                         const A3& argument3, const A4& argument4) const
        // Increment global counter 'executeUsageCounter'.
        //
        // This function tests that the 'execute' method of 'bdefr_Vfunc4' is
        // indeed virtual.  If it is not virtual, then the
        // 'executeUsageCounter' will not be incremented.
    {
        ++executeUsageCounter;
    }

  private:
    virtual ~ConcreteDerivedClass()
        // Destroy the class instance.  Increment a global 'dtorUsageCounter'.
        //
        // Test that a) the destructor in the base class is virtual, b) the
        // destructor is called.
    {
        ++dtorUsageCounter;
    }
};

//=============================================================================
//                  ENVELOPE CLASS FOR USAGE EXAMPLE
//-----------------------------------------------------------------------------

template <class A1, class A2, class A3, class A4>
class EnvelopeClass {
    // Provide an object that encapsulates a 'bdefr_Vfunc4' object.

    bdefr_Vfunc4<A1, A2, A3, A4> *d_rep_p;
        // polymorphic functor representation

  public:
    // CREATORS
    EnvelopeClass(bdefr_Vfunc4<A1, A2, A3, A4> *rep) :
        d_rep_p(rep)
        // Create a functor that assumes shared ownership of the specified,
        // dynamically allocated, reference-counted representation.
    {
        if (d_rep_p) {
            d_rep_p->increment();
        }
    }

    ~EnvelopeClass()
        // Decrement the reference count of that internal representation
        // object, and, if the count is now 0, destroy and deallocate the
        // representation using 'deleteObject' method of 'bdefr_Vfunc4' class.
    {
        if (0 == d_rep_p->decrement()) {
            bdefr_Vfunc4<A1, A2, A3, A4>::deleteObject(
                                                                     d_rep_p);
        }
    }


    // ACCESSORS
    void operator()(const A1& argument1, const A2& argument2,
                    const A3& argument3, const A4& argument4) const
        // Execute this functor.
    {
        d_rep_p->execute(argument1, argument2, argument3, argument4);
    }
};

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------
typedef ConcreteDerivedClass<int, int, int, int> DerivedObj;
typedef bdefr_Vfunc4<int, int, int, int> Obj;

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

static int getCount(Obj *object)
    // Return the current count of the specified 'object'.
{
    // Since we can retrieve the value of the count only by calling the
    // 'decrement' method, we will use a composition of the 'increment' and
    // 'decrement' functions to retrieve the value of 'd_count' yet still
    // preserve its current value.

    object->increment();
    return object->decrement();
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
        //   comment characters, replace 'assert' with 'ASSERT', inserted
        //   'getCount' statements to confirm the value of the reference
        //   counter.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing Usage Example" << endl
                          << "=====================" << endl;
        {
            executeUsageCounter = 0;
            dtorUsageCounter = 0;

            typedef ConcreteDerivedClass<int, int, int, int> DerivedObj;
            typedef bdefr_Vfunc4<int, int, int, int> Obj;
            bslma_Allocator *myAllocator = bslma_Default::defaultAllocator();

            Obj *x = new(*myAllocator) DerivedObj(myAllocator);
            {
                // The reference counter is 0
                ASSERT(0 == getCount(x));
                EnvelopeClass<int, int, int, int> env1(x);
                env1(1, 2, 3, 4);
                ASSERT(1 == executeUsageCounter);
                // The reference counter is 1
                ASSERT(1 == getCount(x));
                {
                    EnvelopeClass<int, int, int, int> env1(x);
                    // The reference counter is 2
                    ASSERT(2 == getCount(x));
                    env1(1, 2, 3, 4);
                    ASSERT(2 == executeUsageCounter);
                }
                // The reference counter is 1
                ASSERT(1 == getCount(x));
                ASSERT(0 == dtorUsageCounter);
            }
            ASSERT(1 == dtorUsageCounter);
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING CREATORS AND MANIPULATORS
        //   The basic concerns are that the constructor and manipulators
        //   'increment' and 'decrement' initialize/manipulate the 'd_count'
        //   correctly, and that 'deleteObject' class method destroys the
        //   object and deallocates it to the held valid memory allocator
        //   supplied at construction.
        //
        //   We have the following specific concerns:
        //    1. The constructor initializes 'd_count' to '0' and initializes
        //       memory allocator to the value that was passed in.
        //    2. 'increment' increases the value of 'd_count' by '1'.
        //       'decrement' decreases the value of 'd_count' by '1'.
        //    3. 'deleteObject' (a) destroys the object (b) frees the memory
        //       using a basic alloator supplied at construction.
        //    4. 'execute' is defined as a virtual method.
        //    5. 'dtor' is defined as a virtual method.
        //
        // Plan
        //   1.   Create a class instance that uses a 'bslma_TestAllocator' to
        //        initialize 'd_allocator_p'.  Check the value of 'd_count'
        //        using global 'getCount' function.
        //   2.   Verify that 'd_count' equals '0' using 'getCount' function.
        //        Check that 'increment' and 'decrement' work correctly when
        //        'd_count' is >= '0'.
        //   3,5. Invoke 'deleteObject'.  (a) Verify that it destroys the
        //        object by calling its destructor.  (b) Verify that the
        //        memory was freed by 'bslma_TestAllocator'.
        //   4.   Invoke 'execute' method.  Verify that the global counter was
        //        incremented.
        //
        // Testing:
        //   bdefr_Vfunc4<A1-4>(bslma_Allocator *ba);
        //   void increment();
        //   int decrement();
        //   void execute(c_A1-4&) const;
        //   static void deleteObject(bdefr_Vfunc4 *object);
        //   ~bdefr_Vfunc4<A1-4>();
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing Basic Functionality" << endl
                          << "===========================" << endl;

        executeUsageCounter = 0;
        dtorUsageCounter = 0;

        if (veryVerbose) cout <<
            "\nTesting Constructor." << endl;
        {
            ASSERT(0 == testAllocator.numBytesInUse());

            Obj *x = new(testAllocator) DerivedObj(&testAllocator);
            ASSERT(sizeof(Obj) == testAllocator.numBytesInUse());

            if (veryVerbose) { cout << "\t"; P(x);}
            if (veryVerbose)
                cout << "\tTesting that constructor initializes 'd_count' "
                        "correctly." << endl;
            ASSERT(0 == getCount(x));

            if (verbose)
                cout << "\nTesting 'increment' and 'decrement'." << endl;

            if (veryVerbose)
                cout << "\tTesting that 'increment' from '0' works as "
                        "expected." << endl;
            x->increment();
            ASSERT(1 == getCount(x));

            if (veryVerbose)
                cout << "\tTesting that 'increment' from positive works as "
                        "expected." << endl;
            x->increment();
            ASSERT(2 == getCount(x));

            x->decrement();
            ASSERT(1 == getCount(x));

            if (veryVerbose)
                cout << "\tTesting that 'decrement' from '1' works as "
                        "expected." << endl;
            x->decrement();
            ASSERT(0 == getCount(x));

            if (verbose) cout << "\nTesting 'execute'." << endl;

            x->execute(1, 2, 3, 4);
            ASSERT(1 == executeUsageCounter);
            x->execute(1, 2, 3, 4);
            ASSERT(2 == executeUsageCounter);

            if (verbose) cout << "\nTesting 'deleteObject'." << endl;

            ASSERT(sizeof(Obj) == testAllocator.numBytesInUse());
            Obj::deleteObject(x);
            ASSERT(0 == testAllocator.numBytesInUse());
            ASSERT(1 == dtorUsageCounter);
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // TESTING 'getCount' HELPER FUNCTION:
        //   We need to confirm that the helper returns current count without
        //   chnging it.
        //
        // Plan:
        //   Construct an object x and verify that 'getCount(x)' always returns
        //   the same value as 'x.decrement()' in the following sequence:
        //       x.increment();  x.decrement();
        //
        // Testing:
        //   int getCount(Obj *object);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing 'getCount' Helper Function" << endl
                          << "==================================" << endl;
        {
            Obj *x = new(testAllocator) DerivedObj(&testAllocator);
            x->increment();
            int expected = x->decrement();
            ASSERT(expected == getCount(x));

            // Do it again with the same value.
            x->increment();
            expected = x->decrement();
            ASSERT(expected == getCount(x));

            // Do it twice with different values.
            x->increment();
            x->increment();
            expected = x->decrement();
            ASSERT(expected == getCount(x));

            x->increment();
            x->increment();
            expected = x->decrement();
            ASSERT(expected == getCount(x));

            Obj::deleteObject(x);
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
