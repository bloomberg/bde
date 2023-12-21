// bslma_deleteobjectproctor.t.cpp                                    -*-C++-*-

#include <bslma_deleteobjectproctor.h>

#include <bslma_allocator.h>
#include <bslma_allocatorutil.h>
#include <bslma_isstdallocator.h>
#include <bslma_bslallocator.h>
#include <bslma_testallocator.h>

#include <bslmf_nestedtraitdeclaration.h>

#include <bsls_alignmentfromtype.h>
#include <bsls_bsltestutil.h>
#include <bsls_nullptr.h>
#include <bsls_objectbuffer.h>

#include <cstdio>      // 'printf'
#include <cstdlib>     // 'atoi'

using namespace BloombergLP;

//=============================================================================
//                                 TEST PLAN
//-----------------------------------------------------------------------------
//                                  Overview
//                                  --------
// We are testing a proctor object to ensure that it destroys the managed
// objects and deallocates memory used if 'release' is not called before the
// proctor object goes out of scope.  We achieve this goal by creating objects
// of a user-defined type that are each initialized with the address of a
// unique counter.  As each object is destroyed, its destructor increments the
// counter held by the object, indicating the number of times the object's
// destructor is called.  After the proctor is destroyed, we verify that the
// corresponding counters of the object managed by the proctor are modified,
// and all allocated memory are deallocated.  The proctor is instantiated with
// 'bsl::allocator', 'bslma::TestAllocator *', and a custom pool class pointer
// to test that it correctly calls the correct interface for the memory
// supplier.
//-----------------------------------------------------------------------------
// [3] DeleteObjectProctor(const ALLOCATOR&, TYPE *);
// [6] DeleteObjectProctor(DeleteObjectProctor&& original);
// [3] ~DeleteObjectProctor();
// [3] TYPE *ptr() const;
// [4] TYPE *release();
// [5] void reset(ptr);
//-----------------------------------------------------------------------------
// [1] BREATHING TEST
// [2] Helper Class: 'my_Class'
// [2] Helper Class: 'my_Pool'
// [7] USAGE EXAMPLES
//=============================================================================

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

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

typedef bslma::AllocatorUtil  AllocUtil;
typedef bslmf::MovableRefUtil MoveUtil;

//=============================================================================
//                          HELPER CLASS FOR TESTING
//-----------------------------------------------------------------------------

template <class TYPE>
class FancyPointer
{
    // A pointer-like class for testing allocators with non-raw pointers.

    // DATA
    TYPE *m_pointer;

  public:
    // CREATORS
    explicit FancyPointer(TYPE *p = 0) : m_pointer(p) { }
    FancyPointer(bsl::nullptr_t) : m_pointer(0) { }

    // ACCESSORS
    TYPE *get()        const { return m_pointer; }
    TYPE *operator->() const { return m_pointer; }
    TYPE& operator*()  const { return *m_pointer; }
    operator bool()  const { return 0 != m_pointer; }
    bool operator!() const { return 0 == m_pointer; }

    // HIDDEN FRIENDS
    friend bool operator==(FancyPointer a, FancyPointer b)
        { return a.m_pointer == b.m_pointer; }
    friend bool operator!=(FancyPointer a, FancyPointer b)
        { return a.m_pointer != b.m_pointer; }
};

template <class TYPE>
class FancyAllocator
{
    // Allocator with non-raw pointer type

    // DATA
    bsl::memory_resource *m_resource;

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(FancyAllocator, bslma::IsStdAllocator);

    // TYPES
    typedef TYPE                     value_type;
    typedef FancyPointer<TYPE>       pointer;
    typedef FancyPointer<const TYPE> const_pointer;
    typedef FancyPointer<void>       void_pointer;
    typedef FancyPointer<const void> const_void_pointer;

    // CREATORS
    FancyAllocator(bsl::memory_resource *r) : m_resource(r) { } // IMPLICIT

    template <class OTHER>
    FancyAllocator(const FancyAllocator<OTHER>& other)
        : m_resource(other.resource()) { }

    // MANIPULATORS
    pointer allocate(std::size_t n) {
        void *p = m_resource->allocate(sizeof(TYPE) * n,
                                       bsls::AlignmentFromType<TYPE>::VALUE);
        return pointer(static_cast<TYPE *>(p));
    }

    void deallocate(pointer p, std::size_t n) {
        m_resource->deallocate(p.get(), sizeof(TYPE) * n,
                               bsls::AlignmentFromType<TYPE>::VALUE);
    }

    template <class OBJ>
    void construct(OBJ *p) { ::new(p) OBJ; }
    template <class OBJ, class ARG>
    void construct(OBJ *p, const ARG& arg) { ::new(p) OBJ(arg); }

    // ACCESSORS
    bsl::memory_resource *resource() const { return m_resource; }

    // HIDDEN FRIENDS
    friend bool operator==(const FancyAllocator& a, const FancyAllocator& b)
        { return *a.resource() == *b.resource(); }
    friend bool operator!=(const FancyAllocator& a, const FancyAllocator& b)
        { return *a.resource() != *b.resource(); }
};

template <class T1, class T2>
inline
bool operator==(const FancyAllocator<T1>& a, const FancyAllocator<T2>& b)
{
    return *a.resource() == *b.resource();
}

template <class T1, class T2>
inline
bool operator!=(const FancyAllocator<T1>& a, const FancyAllocator<T2>& b)
{
    return *a.resource() != *b.resource();
}

class my_Class {
    // This object indicates that its destructor is called by incrementing the
    // global counter (supplied at construction) that it holds.

    // DATA
    int *d_counter_p;  // (non-owned) counter to be incremented at destruction

  public:
    // CREATORS
    explicit my_Class(int *counter) : d_counter_p(counter) {}
        // Create this object using the address of the specified 'counter' to
        // be held.

    ~my_Class() { ++*d_counter_p; }
        // Destroy this object and increment this object's (global) counter.
};

class my_Pool {
    // This class provides a 'deallocate' method, used to exercise the
    // contract promised by the destructor of the 'bslma::DeleteObjectProctor'.
    // This object indicates that its 'deallocate' method is called by
    // incrementing the global counter (supplied at construction) that it
    // *holds*.

    // DATA
    int *d_counter_p;  // (non-owned) counter incremented on 'deallocate'

  public:
    // CREATORS
    explicit my_Pool(int *counter) : d_counter_p(counter) {}
        // Create this object holding the specified (global) counter.

    // MANIPULATORS
    void deallocate(void *) { ++*d_counter_p; }
        // Increment this object's counter.
};

//=============================================================================
//                                USAGE EXAMPLE
//-----------------------------------------------------------------------------

#if defined(BDE_BUILD_TARGET_EXC)

//Example 1: Class having an owning pointer
//- - - - - - - - - - - - - - - - - - - - -
// In this example, we create a class, 'my_Manager', having an owning pointer
// to an object of another class, 'my_Data'.  Because it owns the 'my_Data'
// object, 'my_Manager' is responsible for allocating, constructing,
// deallocating, and destroying it.
//
// First, we define the 'my_Data' class, which holds an integer value and
// counts how many times its constructor and destructor have been called. It
// also has a manipulator, 'mightThrow', that throws an exception if the
// integer value equals the number of constructor calls:
//..
//  #include <bslma_allocatorutil.h>
//  #include <bslma_bslallocator.h>
//  #include <bslma_testallocator.h>

    class my_Data {

        // DATA
        int d_value;

        // CLASS DATA
        static int s_numConstructed;
        static int s_numDestroyed;

      public:
        // CLASS METHODS
        static int numConstructed() { return s_numConstructed; }
        static int numDestroyed()   { return s_numDestroyed;   }

        // CREATORS
        explicit my_Data(int v) : d_value(v) { ++s_numConstructed; }
        my_Data(const my_Data& original);
        ~my_Data() { ++s_numDestroyed; }

        // MANIPULATORS
        void mightThrow()
            { if (s_numConstructed == d_value) { throw --d_value; } }
    };

    int my_Data::s_numConstructed = 0;
    int my_Data::s_numDestroyed   = 0;
//..
// Next, we define 'my_Manager' as an allocator-aware class holding a pointer
// to 'my_Data' and maintaining its own count of constructor invocations:
//..
    class my_Manager {

        // DATA
        bsl::allocator<my_Data>  d_allocator;
        my_Data                 *d_data_p;

        // CLASS DATA
        static int s_numConstructed;

      public:
        // TYPES
        typedef bsl::allocator<> allocator_type;

        // CLASS METHODS
        static int numConstructed() { return s_numConstructed; }

        // CREATORS
        explicit my_Manager(int                   v,
                          const allocator_type& allocator = allocator_type());
        my_Manager(const my_Manager& original);
        ~my_Manager();

        // ...
    };

    int my_Manager::s_numConstructed = 0;
//..
// Next, we define the constructor for 'my_Manager', which begins by allocating
// and constructing a 'my_Data' object:
//..
    my_Manager::my_Manager(int v, const allocator_type& allocator)
        : d_allocator(allocator), d_data_p(0)
    {
        d_data_p = bslma::AllocatorUtil::newObject<my_Data>(allocator, v);
//..
// Then, the 'my_Manager' constructor invokes the 'mightThrow' manipulator on
// the new data object, but first, it protects the object with a
// 'bslma::DeleteObjectProctor':
//..
        bslma::DeleteObjectProctor<allocator_type, my_Data>
                                                proctor(d_allocator, d_data_p);
        d_data_p->mightThrow();
//..
// Then, once the 'mightThrow' operation completes successfully, we can release
// the data object from the proctor.  Only then do we increment the
// construction count:
//..
        proctor.release();
        ++s_numConstructed;
    }
//..
// Next, we define the 'my_Manager' destructor, which destroys and deallocates
// its data object:
//..
    my_Manager::~my_Manager()
    {
        bslma::AllocatorUtil::deleteObject(d_allocator, d_data_p);
    }
//..
// Now, we use a 'bslma::TestAllocator' to verify that, under normal (non
// exceptional) circumstances, constructing a 'my_Manager' object will result
// in one block of memory being allocated and one invocation of the 'my_Data'
// constructor:
//..
    void usageExample1()
    {
        bslma::TestAllocator ta;

        {
            my_Manager obj1(7, &ta);
            ASSERT(1 == ta.numBlocksInUse());
            ASSERT(1 == ta.numBlocksTotal());
            ASSERT(1 == my_Data::numConstructed());
            ASSERT(0 == my_Data::numDestroyed());
            ASSERT(1 == my_Manager::numConstructed());
        }
        ASSERT(0 == ta.numBlocksInUse());
        ASSERT(1 == ta.numBlocksTotal());
        ASSERT(1 == my_Data::numConstructed());
        ASSERT(1 == my_Data::numDestroyed());
        ASSERT(1 == my_Manager::numConstructed());
//..
// Finally, when 'mightThrow' does throw, a block is allocated and a 'my_Data'
// constructor is invoked, but we verify that the 'my_Manager' constructor did
// not complete, the 'my_Data' destructor was called and the block was
// deallocated, resulting in no leaks:
//..
        try {
            my_Manager obj2(2, &ta);
            ASSERT(false && "Can't get here");
        }
        catch (int e) {
            ASSERT(1 == e);
            ASSERT(0 == ta.numBlocksInUse());
            ASSERT(2 == ta.numBlocksTotal());
            ASSERT(2 == my_Data::numConstructed());
            ASSERT(2 == my_Data::numDestroyed());   //
            ASSERT(1 == my_Manager::numConstructed());
        }
        ASSERT(1 == my_Manager::numConstructed());
    }
//..
#endif // defined(BDE_BUILD_TARGET_EXC)

//=============================================================================
//                                MAIN PROGRAM
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

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:  // Zero is always the leading case.
      case 7: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLES
        //
        // Concerns:
        //: 1 That the usage examples shown in the component-level
        //:   documentation compile and run as described.
        //
        // Plan:
        //: 1 Copy the usage examples from the component header, changing
        //    'assert' to 'ASSERT' and execute them.
        //
        // Testing:
        //     USAGE EXAMPLES
        // --------------------------------------------------------------------

        if (verbose) printf("\nUSAGE EXAMPLES"
                            "\n==============\n");

#if defined(BDE_BUILD_TARGET_EXC)
        usageExample1();
#else
        if (veryVerbose) printf("Test not run without exception support.\n");
#endif
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // MOVE CONSTRUCTOR TEST
        //
        // Concerns:
        //: 1 Move-constructing one 'bslma::DeleteObjectProctor' from
        //:   another results in the new proctor engaging the same object with
        //:   the same allocator as the original.
        //: 2 After the move, the original object is disengaged, but retains
        //:   its allocator.
        //: 3 Move-constructing a disengaged proctor yields a second disengaged
        //:   proctor using the same allocator.
        //: 4 The above concerns apply for all allocator/pool categories
        //:   supported by this component.
        //
        // Plan:
        //: 1 Construct a 'bslma::DeleteObjectProctor' managing an instrumented
        //:   class object.  Copy construct a second
        //:   'bslma::DeleteObjectProctor'.  Verify that 'ptr()' returns the
        //:   original class object and that no deallocations or destructor
        //:   calls occured.  Verify that when the second proctor goes out of
        //:   scope, the original object is deleted.  (C-1)
        //: 2 Verify that the original (first) proctor from step 1 is
        //:   disengaged ('ptr()' returns null).  Reset it to manage a new
        //:   object.  When the first proctor goes out of scope, verify that it
        //:   deletes the new object.  (C-2)
        //: 3 Create a third, disengaged 'bslma::DeleteObjectProctor', then
        //:   move-construct it, creating a fourth proctor.  Verify that the
        //:   third and fourth proctors are both disengaged.  Reset the fourth
        //:   proctor to manage a new object.  When the fourth proctor goes out
        //:   of scope, verify that it deleted the new object.  (C-3)
        //: 4 Repeat the preceding steps using 'bsl::allocator',
        //:   'bslma::Allocator *', and 'my_Pool *' for the allocator type.
        //
        // Testing:
        //     DeleteObjectProctor(DeleteObjectProctor&& original);
        // --------------------------------------------------------------------

        if (verbose) printf("\nMOVE CONSTRUCTOR TEST"
                            "\n=====================\n");

        bslma::TestAllocator ta;

        if (veryVerbose) printf("Test with bsl::allocator\n");
        int counter1 = 0, counter2 = 0, counter3 = 0;
        {
            typedef bsl::allocator<>                            Alloc;
            typedef bslma::DeleteObjectProctor<Alloc, my_Class> Obj;

            Alloc alloc(&ta);
            my_Class *p1 = AllocUtil::newObject<my_Class>(alloc, &counter1);
            my_Class *p2 = 0, *p3 = 0;
            Obj proctorA(alloc, p1);  // Engaged
            Obj proctorC(alloc, 0);   // Disenaged
            {
                // Move engaged proctor
                Obj proctorB(MoveUtil::move(proctorA));
                ASSERT(p1 == proctorB.ptr());
                ASSERT(0  == proctorA.ptr());

                p2 = AllocUtil::newObject<my_Class>(alloc, &counter2);
                proctorA.reset(p2);  // Re-engage with same allocator
                ASSERT(sizeof(my_Class) * 2 == ta.numBytesInUse());
                ASSERT(0                    == counter1);
                ASSERT(0                    == counter2);

                // Move disengaged proctor
                Obj proctorD(MoveUtil::move(proctorC));
                ASSERT(0 == proctorC.ptr());
                ASSERT(0 == proctorD.ptr());

                p3 = AllocUtil::newObject<my_Class>(alloc, &counter3);
                proctorD.reset(p3);  // Engage with same allocator
                ASSERT(sizeof(my_Class) * 3 == ta.numBytesInUse());
                ASSERT(0                    == counter1);
                ASSERT(0                    == counter2);
                ASSERT(0                    == counter3);
            }
            ASSERT(p2               == proctorA.ptr());
            ASSERT(sizeof(my_Class) == ta.numBytesInUse());
            ASSERT(1                == counter1);  // proctorB destroyed
            ASSERT(0                == counter2);  // proctorA managing
            ASSERT(1                == counter3);  // proctorD destroyed
        }
        ASSERT(0 == ta.numBytesInUse());
        ASSERT(1 == counter1);  // unchanged
        ASSERT(1 == counter2);  // proctorA destroyed
        ASSERT(1 == counter3);  // unchanged

        if (veryVerbose) printf("Test with bslma::TestAllocator*\n");
        counter1 = counter2 = counter3 = 0;
        {
            typedef bslma::TestAllocator                        *Alloc;
            typedef bslma::DeleteObjectProctor<Alloc, my_Class>  Obj;

            Alloc alloc(&ta);
            my_Class *p1 = AllocUtil::newObject<my_Class>(alloc, &counter1);
            my_Class *p2 = 0, *p3 = 0;
            Obj proctorA(alloc, p1);  // Engaged
            Obj proctorC(alloc, 0);   // Disenaged
            {
                // Move engaged proctor
                Obj proctorB(MoveUtil::move(proctorA));
                ASSERT(p1 == proctorB.ptr());
                ASSERT(0  == proctorA.ptr());

                p2 = AllocUtil::newObject<my_Class>(alloc, &counter2);
                proctorA.reset(p2);  // Re-engage with same allocator
                ASSERT(sizeof(my_Class) * 2 == ta.numBytesInUse());
                ASSERT(0                    == counter1);
                ASSERT(0                    == counter2);

                // Move disengaged proctor
                Obj proctorD(MoveUtil::move(proctorC));
                ASSERT(0 == proctorC.ptr());
                ASSERT(0 == proctorD.ptr());

                p3 = AllocUtil::newObject<my_Class>(alloc, &counter3);
                proctorD.reset(p3);  // Engage with same allocator
                ASSERT(sizeof(my_Class) * 3 == ta.numBytesInUse());
                ASSERT(0                    == counter1);
                ASSERT(0                    == counter2);
                ASSERT(0                    == counter3);
            }
            ASSERT(p2               == proctorA.ptr());
            ASSERT(sizeof(my_Class) == ta.numBytesInUse());
            ASSERT(1                == counter1);  // proctorB destroyed
            ASSERT(0                == counter2);  // proctorA managing
            ASSERT(1                == counter3);  // proctorD destroyed
        }
        ASSERT(0 == ta.numBytesInUse());
        ASSERT(1 == counter1);  // unchanged
        ASSERT(1 == counter2);  // proctorA destroyed
        ASSERT(1 == counter3);  // unchanged

        if (veryVerbose) printf("Test with my_Pool\n");
        counter1 = counter2 = counter3 = 0;
        int deallocCounter = 0;
        bsls::ObjectBuffer<my_Class> ob1, ob2, ob3;
        {
            typedef my_Pool                                     *Alloc;
            typedef bslma::DeleteObjectProctor<Alloc, my_Class>  Obj;

            my_Pool pool(&deallocCounter);
            Alloc   alloc(&pool);
            my_Class *p1 = new (&ob1.object()) my_Class(&counter1);
            my_Class *p2 = 0, *p3 = 0;
            Obj proctorA(alloc, p1);  // Engaged
            Obj proctorC(alloc, 0);   // Disenaged
            {
                // Move engaged proctor
                Obj proctorB(MoveUtil::move(proctorA));
                ASSERT(p1 == proctorB.ptr());
                ASSERT(0  == proctorA.ptr());

                p2 = new (&ob2.object()) my_Class(&counter2);
                proctorA.reset(p2);  // Re-engage with same allocator
                ASSERT(0 == deallocCounter);
                ASSERT(0 == counter1);
                ASSERT(0 == counter2);

                // Move disengaged proctor
                Obj proctorD(MoveUtil::move(proctorC));
                ASSERT(0 == proctorC.ptr());
                ASSERT(0 == proctorD.ptr());

                p3 = new (&ob3.object()) my_Class(&counter3);
                proctorD.reset(p3);  // Engage with same allocator
                ASSERT(0 == deallocCounter);
                ASSERT(0 == counter1);
                ASSERT(0 == counter2);
                ASSERT(0 == counter3);
            }
            ASSERT(p2 == proctorA.ptr());
            ASSERT(2  == deallocCounter);
            ASSERT(1  == counter1);  // proctorB destroyed
            ASSERT(0  == counter2);  // proctorA managing
            ASSERT(1  == counter3);  // proctorD destroyed
        }
        ASSERT(3 == deallocCounter);
        ASSERT(1 == counter1);  // unchanged
        ASSERT(1 == counter2);  // proctorA destroyed
        ASSERT(1 == counter3);  // unchanged

      } break;
      case 5: {
        // --------------------------------------------------------------------
        // 'reset' TEST
        //
        // Concerns:
        //: 1 When the 'reset' method is called, the proctor object properly
        //:   manages a different object.
        //
        // Plan:
        //: 1 Create a 'my_Class' object using a 'bslma::TestAllocator' and
        //:   initialize it with a counter.  Create another 'my_Class'
        //:   object and initialize it with a different counter.  Finally
        //:   initialize a 'bslma::DeleteObjectProctor' object with the first
        //:   object and 'bslma::TestAllocator'.
        //: 2 Call 'reset' on the proctor with the second object before it goes
        //:   out of scope.  Verify that the 'ptr' method returns the second
        //:   pointer.  (C-1)
        //: 3 Once the proctor goes out of scope, verify that only the second
        //:   counter is incremented, and only the memory allocated for the
        //:   one 'my_Class' object was deallocated.  (C-1)
        //
        // Testing:
        //   void reset(ptr);
        // --------------------------------------------------------------------

        if (verbose) printf("\n'reset' TEST"
                            "\n============\n");

        bslma::TestAllocator        z(veryVeryVeryVerbose);
        const bslma::TestAllocator& Z = z;

        int counter1 = 0;
        int counter2 = 0;

        my_Class *pC1, *pC2;
        {
            pC1 = AllocUtil::newObject<my_Class>(&z, &counter1);
            pC2 = AllocUtil::newObject<my_Class>(&z, &counter2);
            ASSERT(0                    == counter1);
            ASSERT(0                    == counter2);
            ASSERT(2 * sizeof(my_Class) == Z.numBytesInUse());

            bslma::DeleteObjectProctor<bslma::Allocator*, my_Class>
                proctor(&z, pC1);
            ASSERT(proctor.ptr()        == pC1);
            ASSERT(0                    == counter1);
            ASSERT(0                    == counter2);
            ASSERT(2 * sizeof(my_Class) == Z.numBytesInUse());

            proctor.reset(pC2);
            ASSERT(proctor.ptr()        == pC2);
            ASSERT(0                    == counter1);
            ASSERT(0                    == counter2);
            ASSERT(2 * sizeof(my_Class) == Z.numBytesInUse());
        }
        ASSERT(0                == counter1);
        ASSERT(1                == counter2);
        ASSERT(sizeof(my_Class) == Z.numBytesInUse());

        AllocUtil::deleteObject(&z, pC1);
        ASSERT(1 == counter1);
        ASSERT(1 == counter2);
        ASSERT(0 == Z.numBytesInUse());

      } break;
      case 4: {
        // --------------------------------------------------------------------
        // 'release' TEST
        //
        // Concerns:
        //: 1 Calling 'release' on a proctor returns a pointer to the proctored
        //:   object.
        //: 2 After calling 'release', 'ptr()' returns a null pointer.
        //:   'release' is idempotent; calling 'release' on a proctor that has
        //:   already been released returns a null pointer.
        //: 3 Calling 'release' on a proctor does not destroy or deallocate the
        //:   proctored object.
        //: 4 After calling 'release' on a proctor, its destructor becomes a
        //:   no-op - no objects are destroyed or deallocated.
        //
        // Plan:
        //: 1 Create 'my_Class' objects using 'bslma::TestAllocator' and
        //:   initialize it with a counter.  Next initialize a
        //:   'bslma::DeleteObjectProctor' object with the corresponding
        //:   'my_Class' object and 'bslma::TestAllocator'.
        //: 2 Call 'release' on the proctor before it goes out of scope.
        //:   Verify that the return value is the pointer that was used on
        //:   construction.  Verify that 'ptr()' returns null.  Verify that the
        //:   counter is not incremented, and the memory allocated by the test
        //:   allocator is not deallocated.  (C-1, C-2)
        //: 3 Call 'release' on the proctor again.  Verify that the return
        //:   value is null.  (C-3)
        //: 4 Allow the proctor to go out of scope.  Verify that the counter is
        //:   not incremented, and the memory allocated by the test allocator
        //:   is not deallocated.  (C-4)
        //
        // Testing:
        //   TYPE *release();
        // --------------------------------------------------------------------

        if (verbose) printf("\n'release' TEST"
                            "\n==============\n");

        bslma::TestAllocator z(veryVeryVeryVerbose);
        const bslma::TestAllocator& Z = z;

        if (veryVerbose) std::printf("Test with 'bslma::Allocator *'\n");
        {
            int counter = 0;
            my_Class *pC;
            {
                pC = AllocUtil::newObject<my_Class>(&z, &counter);

                ASSERT(0                == counter);
                ASSERT(sizeof(my_Class) == Z.numBytesInUse());

                bslma::DeleteObjectProctor<bslma::Allocator*, my_Class>
                    proctor(&z, pC);
                ASSERT(pC               == proctor.ptr());
                ASSERT(0                == counter);
                ASSERT(sizeof(my_Class) == Z.numBytesInUse());

                my_Class *ret = proctor.release();
                ASSERT(pC               == ret);
                ASSERT(0                == proctor.ptr());
                ASSERT(0                == counter);
                ASSERT(sizeof(my_Class) == Z.numBytesInUse());

                ret = proctor.release();  // idempotent
                ASSERT(0                == ret);
                ASSERT(0                == proctor.ptr());
                ASSERT(0                == counter);
                ASSERT(sizeof(my_Class) == Z.numBytesInUse());
            }
            ASSERT(0                == counter);
            ASSERT(sizeof(my_Class) == Z.numBytesInUse());

            AllocUtil::deleteObject(&z, pC);
            ASSERT(1 == counter);
            ASSERT(0 == Z.numBytesInUse());
        }

        if (veryVerbose) std::printf("Test with 'FancyAllocator'\n");
        {
            int                    counter = 0;
            FancyAllocator<int>    alloc(&z);
            FancyPointer<my_Class> pC;
            {
                pC = AllocUtil::newObject<my_Class>(alloc, &counter);

                ASSERT(0                == counter);
                ASSERT(sizeof(my_Class) == Z.numBytesInUse());

                bslma::DeleteObjectProctor<FancyAllocator<my_Class> >
                    proctor(alloc, pC);
                ASSERT(pC               == proctor.ptr());
                ASSERT(0                == counter);
                ASSERT(sizeof(my_Class) == Z.numBytesInUse());

                FancyPointer<my_Class> ret = proctor.release();
                ASSERT(pC               == ret);
                ASSERT(bsl::nullptr_t() == proctor.ptr());
                ASSERT(0                == counter);
                ASSERT(sizeof(my_Class) == Z.numBytesInUse());

                ret = proctor.release();  // idempotent
                ASSERT(bsl::nullptr_t() == ret);
                ASSERT(bsl::nullptr_t() == proctor.ptr());
                ASSERT(0                == counter);
                ASSERT(sizeof(my_Class) == Z.numBytesInUse());
            }
            ASSERT(0                == counter);
            ASSERT(sizeof(my_Class) == Z.numBytesInUse());

            AllocUtil::deleteObject(alloc, pC);
            ASSERT(1 == counter);
            ASSERT(0 == Z.numBytesInUse());
        }


      } break;
      case 3: {
        // --------------------------------------------------------------------
        // CONSTRUCTOR / DESTRUCTOR TEST
        //
        // Concerns:
        //: 1 The proctor constructor takes, as arguments, an allocator or pool
        //:   along with a pointer to an object.  The 'ptr()' accessor returns
        //:   the object pointer passed to the constructor.
        //: 2 On destruction, the protected object is destroyed and
        //:   deallocated.
        //: 3 The above concerns apply to a variety of allocator-like types.
        //:   If the allocator, 'a', is an STL-style allocator, the destructor
        //:   will call 'a.deallocate(p, 1)'.  If it is a pointer to a class
        //:   derived from 'bslma::Allocator' or a pointer to any class having
        //:   a 'deallocate(p)' member (e.g., a pool), then the destructor will
        //:   call 'a->deallocate(p)'.
        //: 4 If the object pointer is null, however, the destructor has no
        //:   effect.
        //
        // Plan:
        //: 1 Create a 'bsl::allocator' object that gets memory from a
        //:   'bslma::TestAllocator'. Allocate and construct a 'my_Class'
        //:   object using that allocator.  Create a proctor using that
        //:   allocator and the allocated 'my_Class' object.  (C-1)
        //: 2 When the proctor is destroyed, verify that it destroys and
        //:   deallocates the proctored 'my_Class' object, using the 'my_Class'
        //:   destructor instrumentation and the allocator's deallocation
        //:   instrumentation.  (C-2)
        //: 3 Repeat steps 1 and 2 with a 'bslma::TestAllocator' pointer for
        //:   the allocator.  Repeat steps 1 and 2 with a 'my_Pool' object
        //:   except, since 'my_Pool' doesn't have an 'allocate' method, create
        //:   the object in a 'bsls::ObjectBuffer<my_Class>' instead.  (C-3)
        //: 4 Repeat step 2 for each allocator type, passing a null pointer as
        //:   the second argument to the proctor constructor.  Verify that
        //:   nothing is deallocated by that proctor.  (C-4)
        //
        // Testing:
        //   DeleteObjectProctor(const ALLOCATOR&, TYPE *);
        //   ~DeleteObjectProctor();
        //   TYPE *ptr() const;
        // --------------------------------------------------------------------

        if (verbose) printf("\nCONSTRUCTOR / DESTRUCTOR TEST"
                            "\n=============================\n");

        if (veryVerbose) printf("\nTesting with bsl::allocator\n");
        {
            bslma::TestAllocator        z(veryVeryVeryVerbose);
            const bslma::TestAllocator& Z = z;
            bsl::allocator<>            alloc(&z);

            int counter = 0;

            {
                my_Class *pC = AllocUtil::newObject<my_Class>(alloc, &counter);
                ASSERT(0                == counter);
                ASSERT(sizeof(my_Class) == Z.numBytesInUse());

                // Form 1: explicitly specify 'TYPE', which might be different
                // from 'ALLOCATOR::value_type'.
                bslma::DeleteObjectProctor<bsl::allocator<>, my_Class>
                                                            proctor(alloc, pC);
                const bslma::DeleteObjectProctor<bsl::allocator<>, my_Class>&
                                                             PROCTOR = proctor;
                ASSERT(PROCTOR.ptr()    == pC);
                ASSERT(0                == counter);
                ASSERT(sizeof(my_Class) == Z.numBytesInUse());
            }
            ASSERT(1 == counter);
            ASSERT(0 == Z.numBytesInUse());

            {
                // Form 2: deduce 'TYPE' from 'ALLOCATOR::value_type'.
                bslma::DeleteObjectProctor<bsl::allocator<my_Class> >
                                                                proctor(&z, 0);
                const bslma::DeleteObjectProctor<bsl::allocator<my_Class> >&
                                                             PROCTOR = proctor;
                ASSERT(0 == PROCTOR.ptr());
                ASSERT(1 == counter);
                ASSERT(0 == Z.numBytesInUse());
            }
            ASSERT(1 == counter);
            ASSERT(0 == Z.numBytesInUse());
        }

        if (veryVerbose) printf("\nTesting with FancyAllocator\n");
        {
            bslma::TestAllocator        z(veryVeryVeryVerbose);
            const bslma::TestAllocator& Z = z;
            FancyAllocator<char>        alloc(&z);

            int counter = 0;

            {
                FancyPointer<my_Class> pC =
                    AllocUtil::newObject<my_Class>(alloc, &counter);
                ASSERT(0                == counter);
                ASSERT(sizeof(my_Class) == Z.numBytesInUse());

                // Form 1: explicitly specify 'TYPE', which might be different
                // from 'ALLOCATOR::value_type'.
                bslma::DeleteObjectProctor<FancyAllocator<char>, my_Class>
                                                            proctor(alloc, pC);
                const bslma::DeleteObjectProctor<FancyAllocator<char>,
                                                 my_Class>& PROCTOR = proctor;
                ASSERT(PROCTOR.ptr()    == pC);
                ASSERT(0                == counter);
                ASSERT(sizeof(my_Class) == Z.numBytesInUse());
            }
            ASSERT(1 == counter);
            ASSERT(0 == Z.numBytesInUse());

            {
                // Form 2: deduce 'TYPE' from 'ALLOCATOR::value_type'.
                bslma::DeleteObjectProctor<FancyAllocator<my_Class> >
                                                    proctor(&z, bsl::nullptr_t());
                const bslma::DeleteObjectProctor<FancyAllocator<my_Class> >&
                                                             PROCTOR = proctor;
                ASSERT(bsl::nullptr_t() == PROCTOR.ptr());
                ASSERT(1 == counter);
                ASSERT(0 == Z.numBytesInUse());
            }
            ASSERT(1 == counter);
            ASSERT(0 == Z.numBytesInUse());
        }

        if (veryVerbose) printf("\nTesting with bslma::TestAllocator *\n");
        {
            bslma::TestAllocator        z(veryVeryVeryVerbose);
            const bslma::TestAllocator& Z       = z;
            int                         counter = 0;

            {
                my_Class *pC = AllocUtil::newObject<my_Class>(&z, &counter);
                ASSERT(0                == counter);
                ASSERT(sizeof(my_Class) == Z.numBytesInUse());

                bslma::DeleteObjectProctor<bslma::Allocator*, my_Class>
                                                               proctor(&z, pC);
                const bslma::DeleteObjectProctor<bslma::Allocator*, my_Class>&
                                                             PROCTOR = proctor;
                ASSERT(PROCTOR.ptr()    == pC);
                ASSERT(0                == counter);
                ASSERT(sizeof(my_Class) == Z.numBytesInUse());
            }
            ASSERT(1 == counter);
            ASSERT(0 == Z.numBytesInUse());

            {
                bslma::DeleteObjectProctor<bslma::Allocator*, my_Class>
                                                                proctor(&z, 0);
                const bslma::DeleteObjectProctor<bslma::Allocator*, my_Class>&
                                                             PROCTOR = proctor;
                ASSERT(0 == PROCTOR.ptr());
                ASSERT(1 == counter);
                ASSERT(0 == Z.numBytesInUse());
            }
            ASSERT(1 == counter);
            ASSERT(0 == Z.numBytesInUse());
       }

        if (veryVerbose) printf("\nTesting with my_Pool\n");
        {
            int     counter        = 0;
            int     deallocCounter = 0;
            my_Pool pool(&deallocCounter);

            {
                bsls::ObjectBuffer<my_Class> buf;
                my_Class *pC = new (buf.address()) my_Class(&counter);
                ASSERT(0 == counter);
                ASSERT(0 == deallocCounter);

                bslma::DeleteObjectProctor<my_Pool*, my_Class>
                                                            proctor(&pool, pC);
                const bslma::DeleteObjectProctor<my_Pool*, my_Class>&
                                                             PROCTOR = proctor;
                ASSERT(PROCTOR.ptr() == pC);
                ASSERT(0             == counter);
                ASSERT(0             == deallocCounter);
            }
            ASSERT(1 == counter);
            ASSERT(1 == deallocCounter);

            {
                bslma::DeleteObjectProctor<my_Pool*, my_Class>
                                                             proctor(&pool, 0);
                const bslma::DeleteObjectProctor<my_Pool*, my_Class>&
                                                             PROCTOR = proctor;
                ASSERT(0 == PROCTOR.ptr());
                ASSERT(1 == counter);
                ASSERT(1 == deallocCounter);
            }
            ASSERT(1 == counter);
            ASSERT(1 == deallocCounter);
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // HELPER CLASS TEST
        //
        // Concerns:
        //: 1 The helper class 'my_Class' properly increments its counter upon
        //:   destruction.
        //: 2 The helper class 'my_Pool' properly increments its counter when
        //:   its 'deallocate' method is called.
        //
        // Plan:
        //: 1 Create a 'my_Class' object, passing to the constructor the
        //:   address of a counter.  Verify that the counter is incremented on
        //:   destruction.  Repeat several times.  (C-1)
        //: 2 Create a 'my_Pool' object, passing to the constructor the address
        //:   of a counter.  Invoke the 'deallocate' method several times and
        //:   verify that the counter is incremented each time.  (C-2)
        //
        // Testing:
        //   Helper Class: 'my_Class'
        //   Helper Class: 'my_Pool'
        // --------------------------------------------------------------------

        if (verbose) printf("\nHELPER CLASS TEST"
                            "\n=================\n");

        if (veryVerbose) printf("\nTesting 'my_Class'.\n");

        if (veryVerbose) printf("\tTesting default ctor and dtor.\n");
        {
            int counter = 0;
            const int NUM_TEST = 5;
            for (int i = 0; i < NUM_TEST; ++i) {
                ASSERTV(i, counter == i);
                my_Class mx(&counter);
            }
            ASSERT(NUM_TEST == counter);
        }

        if (veryVerbose) printf("\nTesting 'my_Pool'.\n");

        if (veryVerbose) printf("\tTesting default ctor and 'deallocate'.\n");

        {
            int counter = 0;
            const int NUM_TEST = 5;
            my_Pool mx(&counter);

            for (int i = 0; i < NUM_TEST; ++i) {
                ASSERTV(i, i == counter);
                mx.deallocate(0);
                ASSERTV(i, i + 1 == counter);
            }
            ASSERT(NUM_TEST == counter);
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //   This case exercises (but does not fully test) basic functionality.
        //
        // Concerns:
        //: 1 The class is sufficiently functional to enable comprehensive
        //:   testing in subsequent test cases.
        //
        // Plan:
        //: 1 Execute each method to verify functionality for simple cases.
        //
        // Testing:
        //      BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) printf("\nBREATHING TEST"
                            "\n==============\n");

        bslma::TestAllocator        z(veryVeryVeryVerbose);
        const bslma::TestAllocator& Z       = z;
        int                         counter = 0;

        if (veryVerbose) printf("\tTesting with 'my_Class' object\n");
        {

            my_Class *p = AllocUtil::newObject<my_Class>(&z, &counter);
            ASSERT(sizeof(my_Class) == Z.numBytesInUse());

            bslma::DeleteObjectProctor<bslma::Allocator*, my_Class>
                                                                proctor(&z, p);
            ASSERT(proctor.ptr() == p);
        }
        ASSERT(1 == counter);
        ASSERT(0 == Z.numBytesInUse());

      } break;
      default: {
        fprintf(stderr, "WARNING: CASE `%d' NOT FOUND.\n", test);
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        fprintf(stderr, "Error, non-zero test status = %d.\n", testStatus);
    }

    return testStatus;
}

// ----------------------------------------------------------------------------
// Copyright 2023 Bloomberg Finance L.P.
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
