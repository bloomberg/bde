// bslalg_containerbase.t.cpp                                         -*-C++-*-
#include <bslalg_containerbase.h>

#include <bslma_allocator.h>
#include <bslma_allocatorutil.h>
#include <bslma_autodestructor.h>
#include <bslma_constructionutil.h>
#include <bslma_deallocateobjectproctor.h>
#include <bslma_destructionutil.h>
#include <bslma_isstdallocator.h>
#include <bslma_bslallocator.h>
#include <bslma_testallocator.h>
#include <bslma_testallocatormonitor.h>
#include <bslma_usesbslmaallocator.h>

#include <bslmf_isbitwiseequalitycomparable.h>
#include <bslmf_isbitwisemoveable.h>
#include <bslmf_isempty.h>
#include <bslmf_issame.h>
#include <bslmf_istriviallycopyable.h>
#include <bslmf_nestedtraitdeclaration.h>

#include <bsls_bsltestutil.h>
#include <bsls_bsltestutil.h>
#include <bsls_platform.h>
#include <bsls_util.h>

#include <cstdio>      // 'std::printf'
#include <cstdlib>     // 'std::atoi'

#include <new>

using namespace BloombergLP;
using std::printf;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                                   Overview
//                                   --------
// 'bslalg::ContainerBase' is a template for a mechanism class template
// intended for private inheritance from Allocator-Aware (AA) classes,
// especially containers.  It is neither copyable nor movable.  Testing this
// class involves testing its one constructor, it's two accessors and one
// manipulator, and its 'AllocatorType' member type.  Additionally, it's
// intended usage indicates that metafunctions should not deduce it to be an
// allocator type nor itself be an AA type (although classes that derive from
// it are typically AA types).  Additionally, the main service provided by this
// component is ensuring that the allocator does not take up space in the
// derived class if the allocator type is an empty class, so we test this
// feature, as well.
// ----------------------------------------------------------------------------
// [ 2] ALLOCATOR&       allocatorRef();
// [ 2] const ALLOCATOR& allocatorRef() const;
// [ 3] typedef ALLOCATOR AllocatorType;
// [ 2] ContainerBase(const ALLOCATOR&);
// [ 2] ~ContainerBase();
// [ 4] bool equalAllocator(const ContainerBase& rhs) const;
// [ 5] bslma::IsStdAllocator<ContainerBase<T> >
// [ 5] bslma::UsesBslmaAllocator<ContainerBase<T> >
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 6] EMPTY-BASE OPTIMIZATION
// [ 7] USAGE EXAMPLE
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

#define RUN_EACH_TYPE BSLTF_TEMPLATETESTFACILITY_RUN_EACH_TYPE

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

namespace {

class TestType{
    // DATA
    unsigned char d_data1[32];

    static int s_numCopyConstruct;
    static int s_numDestroy;

  public:
    // CLASS METHODS
    static int numCopyConstruct() { return s_numCopyConstruct; }
    static int numDestroy() { return s_numDestroy; }

    // CREATORS
    TestType() { (void) d_data1; }
    TestType(const TestType&) { ++s_numCopyConstruct; }
    ~TestType() { ++s_numDestroy; }
};

// FREE OPERATORS
int TestType::s_numCopyConstruct = 0;
int TestType::s_numDestroy = 0;

const void *g_lastStatelessSTLAllocatorConstructed = 0;
    // Address of the most recent 'StatelessSTLAllocator' object constructed.

const void *g_lastStatelessSTLAllocatorDestroyed = 0;
    // Address of the most recent 'StatelessSTLAllocator' object destroyed.

const void *g_lastStatelessSTLAllocatorCopied = 0;
    // Address of the most recent 'StatelessSTLAllocator' object from which a
    // copy was made.

bslma::TestAllocator g_StatelessSTLAllocatorRsrc("Stateless Allocator");
    // Global memory source for 'StatelessSTLAllocator'.

template <class TYPE>
class StatelessSTLAllocator {
    // Allocator that allocates from the default 'bslma::Allocator'
    // resource.  Constructors keep track of the address of the most-recently
    // constructed instance.

  public:
    // TYPES
    typedef TYPE value_type;

    // CLASS METHODS
    static bslma::Allocator* mechanism()
        { return &g_StatelessSTLAllocatorRsrc; }

    // CREATORS
    StatelessSTLAllocator() { g_lastStatelessSTLAllocatorConstructed = this; }

    StatelessSTLAllocator(const StatelessSTLAllocator& original) {
        g_lastStatelessSTLAllocatorConstructed = this;
        g_lastStatelessSTLAllocatorCopied      = &original;
    }

    template <class T2>
    StatelessSTLAllocator(const StatelessSTLAllocator<T2>& original) {
        g_lastStatelessSTLAllocatorConstructed = this;
        g_lastStatelessSTLAllocatorCopied      = &original;
    }

    ~StatelessSTLAllocator() { g_lastStatelessSTLAllocatorDestroyed = this; }

    // MANIPULATORS
    value_type *allocate(std::size_t n, void * = 0) {
        return bslma::AllocatorUtil::allocateObject<value_type>(
                                              &g_StatelessSTLAllocatorRsrc, n);
    }

    void deallocate(value_type *p, std::size_t n) {
        bslma::Allocator *rsrc = bslma::Default::defaultAllocator();
        bslma::AllocatorUtil::deallocateObject(&g_StatelessSTLAllocatorRsrc,
                                               p, n);
    }
};

template <class T1, class T2>
inline
bool operator==(const StatelessSTLAllocator<T1>&,
                const StatelessSTLAllocator<T2>&)
{
    return true;
}

template <class T1, class T2>
inline
bool operator!=(const StatelessSTLAllocator<T1>&,
                const StatelessSTLAllocator<T2>&)
{
    return false;
}

struct EmptyBase { };

template <class BASE>
struct DerivedClass : private BASE
{
    // Derived class will have size of 1 byte if 'BASE' is empty and compiler
    // supports EBO, otherwise will have a size at least as big as 'BASE' + 1.

    char d_data;
};

}  // close unnamed namespace

//=============================================================================
//                              USAGE EXAMPLES
//-----------------------------------------------------------------------------

///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Creating a Fixed-Size Array with 'bslalg::ContainerBase'
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we would like to implement a fixed-size array that allocates memory
// from a user-supplied allocator at construction.
//
// First, we define the interface of the container, 'MyFixedSizeArray'.  We
// privately derive from 'ContainerBase' to take advantage of the
// empty-base-class optimization (in case 'ALLOCATOR' is an empty class) and to
// take advantage of implementation conveniences 'ContainerBase' provides:
//..
//  #include <bslalg_containerbase.h>
//  #include <bslma_allocatorutil.h>
//  #include <bslma_autodestructor.h>
//  #include <bslma_constructionutil.h>
//  #include <bslma_deallocateobjectproctor.h>
//  #include <bslma_destructionutil.h>
//  #include <bslma_bslallocator.h>
//  #include <bslma_testallocator.h>
//  #include <bslmf_isempty.h>

    template <class VALUE, class ALLOCATOR>
    class MyFixedSizeArray : private bslalg::ContainerBase<ALLOCATOR>
        // This class implements a container that contains a fixed number of
        // elements of the parameterized type 'VALUE' using the parameterized
        // 'ALLOCATOR' to allocate memory.  The number of elements is specified
        // on construction.
    {
        // PRIVATE TYPES
        typedef bslalg::ContainerBase<ALLOCATOR> Base;

        // DATA
        VALUE     *d_array;  // head pointer to the array of elements
        const int  d_size;   // (fixed) number of elements in 'd_array'

      public:
        // TYPES
        typedef ALLOCATOR allocator_type;

        // CREATORS
        explicit MyFixedSizeArray(int              size,
                                  const ALLOCATOR& allocator = ALLOCATOR());
            // Create a 'MyFixedSizeArray' object having the specified 'size'
            // elements, and using the optionally specified 'allocator' to
            // supply memory.  Each element of the array is value initialized.

        MyFixedSizeArray(const MyFixedSizeArray& original,
                         const ALLOCATOR&        allocator = ALLOCATOR());
            // Create a 'MyFixedSizeArray' object having same number of
            // elements as that of the specified 'original', the same value of
            // each element as that of corresponding element in 'original', and
            // using the optionally specified 'allocator' to supply memory.

        ~MyFixedSizeArray();
            // Destroy this object.

        // MANIPULATORS
        VALUE& operator[](int i) { return d_array[i]; }
            // Return a modifiable reference to the specified 'i'th element of
            // this object.  The behavior is undefined unless 'i < size()'.

        // ACCESSORS
        const VALUE& operator[](int i) const { return d_array[i]; }
            // Return a const reference to the specified 'i'th element of this
            // object.  The behavior is undefined unless 'i < size()'.

        ALLOCATOR get_allocator() const;
            // Return the allocator used by this object to allocate memory.

        int size() const { return d_size; }
            // Return the number of elements contained in this object.
    };
//..
// Next, we define the 'get_allocator' accessor, which extracts the allocator
// from the 'ContainerBase' base class using its 'allocatorRef' method:
//..
    template<class VALUE, class ALLOCATOR>
    inline
    ALLOCATOR
    MyFixedSizeArray<VALUE,ALLOCATOR>::get_allocator() const {
        return Base::allocatorRef();
    }
//..
// Next, we define the first constructor, beginning with the initialization the
// 'ContainerBase' base class with the supplied 'allocator':
//..
    template<class VALUE, class ALLOCATOR>
    MyFixedSizeArray<VALUE,ALLOCATOR>::MyFixedSizeArray(
                                                    int              size,
                                                    const ALLOCATOR& allocator)
    : Base(allocator)
    , d_size(size)
    {
//..
// Then, we allocate the specified number of array elements using the allocator
// returned by the 'get_allocator()' method.  Once allocated, we protect the
// array memory with a 'bslma::DeallocateObjectProctor' object:
//..
        d_array =
            bslma::AllocatorUtil::allocateObject<VALUE>(get_allocator(),
                                                        d_size);
        bslma::DeallocateObjectProctor<ALLOCATOR, VALUE>
            deallocateProctor(get_allocator(), d_array, d_size);
//..
// Then, we invoke the constructor for each array element using the
// 'bslma::ConstructionUtil::construct' method.  We use a
// 'bslma::AutoDestuctor' proctor to unwind these constructions if an exception
// is thrown:
//..
        bslma::AutoDestructor<VALUE> autoDtor(d_array, 0);
        // Default construct each element of the array:
        for (int i = 0; i < d_size; ++i) {
            bslma::ConstructionUtil::construct(&d_array[i], get_allocator());
            ++autoDtor;
        }
//..
// Then, when every element has been constructed, we free the proctors:
//..
        autoDtor.release();
        deallocateProctor.release();
    }
//..
// Next we implement the destructor as the reverse of the constructor, invoking
// 'bslma::DestructionUtil::destroy' on each element then deallocating them
// with 'bslma::AllocatorUtil::deallocateObject':
//..
    template<class VALUE, class ALLOCATOR>
    MyFixedSizeArray<VALUE,ALLOCATOR>::~MyFixedSizeArray()
    {
        // Call destructor for each element
        for (int i = 0; i < d_size; ++i) {
            bslma::DestructionUtil::destroy(&d_array[i]);
        }

        // Return memory to allocator.
        bslma::AllocatorUtil::deallocateObject(get_allocator(),
                                               d_array, d_size);
    }
//..
// Next, for testing purposes, we create a 'StatelessAllocator' template that
// simply allocates a global test allocator:
//..
    bslma::TestAllocator g_testAllocator;

    template <class TYPE>
    class StatelessAllocator {
        // Allocator that allocates from the default 'bslma::Allocator'
        // resource.

      public:
        typedef TYPE value_type;

        value_type *allocate(std::size_t n, void * = 0) {
            return bslma::AllocatorUtil::allocateObject<value_type>(
                                                          &g_testAllocator, n);
        }

        void deallocate(value_type *p, std::size_t n) {
            bslma::AllocatorUtil::deallocateObject(&g_testAllocator, p, n);
        }
    };
//..
// Finally, we create two 'MyFixedSizeArray' objects, one using
// 'StatelessAllocator', and the other using 'bsl::allocator', and we verify
// that memory is allocated from the correct allocator for each.  Because
// 'StatelessAllocator' is an empty class, the first object is smaller than the
// second object by at least the size of a 'bsl::allocator'.
//..
    void usageExample()
    {
        ASSERT(bsl::is_empty<StatelessAllocator<int> >::value);

        MyFixedSizeArray<int, StatelessAllocator<int> > fixedArray1(3);
        ASSERT(3               == fixedArray1.size());
        ASSERT(1               == g_testAllocator.numBlocksInUse());
        ASSERT(3 * sizeof(int) == g_testAllocator.numBytesInUse());

        bslma::TestAllocator                            ta;
        MyFixedSizeArray<int, bsl::allocator<int> >     fixedArray2(3, &ta);
        ASSERT(3               == fixedArray2.size());
        ASSERT(&ta             == fixedArray2.get_allocator());
        ASSERT(1               == ta.numBlocksInUse());
        ASSERT(3 * sizeof(int) == ta.numBytesInUse());

        ASSERT(sizeof(fixedArray2) - sizeof(fixedArray1) >=
               sizeof(bsl::allocator<int>));
    }
//..


//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int                 test = argc > 1 ? std::atoi(argv[1]) : 0;
    bool             verbose = argc > 2;
    bool         veryVerbose = argc > 3;
    bool     veryVeryVerbose = argc > 4;
    bool veryVeryVeryVerbose = argc > 5;

    (void)veryVerbose;          // suppress warning
    (void)veryVeryVerbose;      // suppress warning
    (void)veryVeryVeryVerbose;  // suppress warning

    setbuf(stdout, NULL);    // Use unbuffered output

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:  // Zero is always the leading case.
      case 7: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //   Extracted from component header file.
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
        // Testing:
        //      USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) printf("\nUSAGE EXAMPLE"
                            "\n=============\n");

        usageExample();

      } break;
      case 6: {
        // --------------------------------------------------------------------
        // EMPTY-BASE OPTIMIZATION
        //   'ContainerBase' is designed for derived classes to take advantage
        //   of the empty-base optimization (EBO) when the allocator type is an
        //   empty class.
        //
        // Concerns:
        //: 1 If 'ALLOCATOR' is an empty class, a class derived from
        //:   'bslalg::ContainerBase<ALLOCATOR>' can take advantage of the EBO
        //:   such that the allocator takes up no space in the derived class.
        //
        // Plan:
        //: 1 Define an empty class, 'EmptyBase'.  Define a class template,
        //:   'DerivedClass<T>', that inherits privately from 'T'.
        //: 2 Instantiate 'DerivedClass<ContainerBase<A>>' with both empty
        //:   (stateless) and non-empty (statefull) allocator types, 'A'.
        //:   Verify that, when 'A' is empty, 'DerivedClass<ContainerBase<A>>'
        //:   is the same size as 'DerivedClass<EmptyBase>'.  (C-1)
        //
        // Testing:
        //      EMPTY-BASE OPTIMIZATION
        // --------------------------------------------------------------------

        if (verbose) printf("\nEMPTY-BASE OPTIMIZATION"
                            "\n=======================\n");

        const std::size_t EBOSize = sizeof(DerivedClass<EmptyBase>);

        typedef bslalg::ContainerBase<bsl::allocator<> >           Obj1;
        typedef bslalg::ContainerBase<StatelessSTLAllocator<int> > Obj2;

        ASSERT(sizeof(Obj1) >  EBOSize);
        ASSERT(sizeof(Obj2) == EBOSize);

      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TRAITS
        //   Check that 'ContainerBase' cannot be confused for an allocator or
        //   an allocator-aware type.
        //
        // Concerns:
        //: 1 'bslma::IsStdAllocator' does not read 'bslalg::ContainerBase' as
        //:   an allocator; i.e., it evaluates to 'false' for all
        //:   instantiations.
        //: 2 'bslma::UsesBslmaAllocator' does not read 'bslalg::ContainerBase'
        //:   as an allocator-aware type; i.e., it evaluates to 'false' for all
        //:   instantiations.
        //
        // Plan:
        //: 1 Instantiate 'bslalg::ContainerBase' with both stateful and
        //:   stateless allocator types.
        //: 2 Verify that 'bslma::IsStdAllocator' evaluates to 'false' for all
        //:   instantiations.  (C-1)
        //: 3 Verify that 'bslma::UsesBslmaAllocator' evaluates to 'false' for
        //:   all instantiations.  (C-2)
        //
        // Testing:
        //      bslma::IsStdAllocator<ContainerBase<T> >
        //      bslma::UsesBslmaAllocator<ContainerBase<T> >
        // --------------------------------------------------------------------

        if (verbose) printf("\nTRAITS"
                            "\n======\n");

        typedef bslalg::ContainerBase<bsl::allocator<> >           Obj1;
        typedef bslalg::ContainerBase<StatelessSTLAllocator<int> > Obj2;

        ASSERT(false == bslma::IsStdAllocator<Obj1>::value);
        ASSERT(false == bslma::IsStdAllocator<Obj2>::value);

        ASSERT(false == bslma::UsesBslmaAllocator<Obj1>::value);
        ASSERT(false == bslma::UsesBslmaAllocator<Obj2>::value);

      } break;
      case 4: {
        // --------------------------------------------------------------------
        // 'equalAllocator' ACCESSOR
        //
        // Concerns:
        //: 1 For two 'ContainerBase' objects of the same type, 'a' and 'b',
        //:   'a.equalAllocator(b)' returns 'true' if the allocator for 'a' and
        //:   the allocator for 'b' compare equal; otherwise it returns false.
        //: 2 'a.equalAllocator(a)' always returns 'true'.
        //
        // Plan:
        //: 1 Instantiate 'ContainerBase' with both statefull and stateless
        //:   allocator types.
        //: 2 For each instantiation, create three objects, two with the same
        //:   allocator and one with a different allocator.
        //: 3 Verify that 'a.equalAllocator(b)' returns 'true' if the allocator
        //:   for 'a' and the allocator for 'b' compare equal and 'false'
        //:   otherwise.  Note that, for allocators that always compare 'true'
        //:   (e.g., stateless allocators), 'equalAllocator' will always return
        //:   'true'.  (C-1)
        //: 4 Verify that 'a.equalAllocator(a)' always returns 'true'.  (C-2)
        //
        // Testing:
        //      bool equalAllocator(const ContainerBase& rhs) const;
        // --------------------------------------------------------------------

        if (verbose) printf("\n'equalAllocator' ACCESSOR"
                            "\n=========================\n");

        {
            bslma::TestAllocator ta1, ta2;
            typedef bslalg::ContainerBase<bsl::allocator<> > Obj;

            const Obj A(&ta1);
            const Obj B(&ta1);
            const Obj C(&ta2);
            ASSERT( A.equalAllocator(B));
            ASSERT(!A.equalAllocator(C));
            ASSERT( A.equalAllocator(A));
        }

        {
            StatelessSTLAllocator<int> ta1, ta2;
            typedef bslalg::ContainerBase<StatelessSTLAllocator<int> > Obj;

            const Obj A(ta1);
            const Obj B(ta1);
            const Obj C(ta2);
            ASSERT( A.equalAllocator(B));
            ASSERT( A.equalAllocator(C));  // always compares equal
            ASSERT( A.equalAllocator(A));
        }

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // 'AllocatorType' TYPEDEF
        //   There is a nested 'AllocatorType' typedef in 'ContainerBase'.
        //
        // Concerns:
        //: 1 Each instantiation of 'bslalg::ContainerBase<ALLOCATOR>' has a
        //:   nested 'typedef' that is an alias for 'ALLOCATOR'.
        //
        // Plan
        //: 1 Instantiate 'ContainerBase' with a couple of different
        //:   allocators, both stateful and stateless.  Verify that the
        //:   'AllocatorType' typedef is the same as the allocator type.  (C-1)
        //
        // TESTING
        //      typedef ALLOCATOR AllocatorType;
        // --------------------------------------------------------------------

        if (verbose) printf("\n'AllocatorType' TYPEDEF"
                            "\n=======================\n");

        typedef bslalg::ContainerBase<bsl::allocator<> > Cb1;
        ASSERT((bsl::is_same<Cb1::AllocatorType, bsl::allocator<> >::value));

        typedef bslalg::ContainerBase<StatelessSTLAllocator<short> > Cb2;
        ASSERT((bsl::is_same<Cb2::AllocatorType,
                             StatelessSTLAllocator<short> >::value));

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // CONSTRUCTOR AND BASIC ACCESSORS
        //   Test constructor and main accessors.
        //
        // Concerns:
        //: 1 A 'bslalg::ContainerBase<ALLOCATOR>' object can be constructed
        //:   from a single argument that is convertable to 'ALLOCATOR'.  The
        //:   new 'ContainerBase' stores a copy of the constructor argument.
        //: 2 The 'allocatorRef' accessor and manipulator return the stored
        //:   allocator by const reference and non-const reference,
        //:   respectively.
        //: 3 The destructor for 'ContainerBase' invokes the destructor for the
        //: 4 The above concerns apply for both stateful and stateless
        //:   'ALLOCATOR' types.
        //:   stored allocator.
        //
        // Plan:
        //: 1 Using 'bsl::allocator' for 'ALLOCATOR', create 'ContainerBase'
        //:   object, passing the constructor the address of a 'TestAllocator'.
        //:   Verify that the 'allocatorRef' methods return a reference to
        //:   'bsl::allocator' whose mechanism is the test allocator address.
        //:   (C-1, C-2)
        //: 2 Create stateless allocator type, 'StatelessSTLAllocator', that
        //:   tracks constructors, destructors, and copies.  Create a
        //:   'ContainerBase' object, passing the constructor an object of type
        //:   'StatelessSTLAllocator'.  Verify that the object returned by
        //:   'allocatorRef' is the most-recently constructed allocator and
        //:   that the object that was passed to the constructor is the
        //:   most-recently copied allocator.  (C-4)
        //: 3 When the 'ContainerBase' goes out of scope at the end of step 2,
        //:   verify that the most-recently destroyed allocator is the same as
        //:   the most-recently constructed one.  (C-3)
        //
        // Testing:
        //      ContainerBase(const ALLOCATOR&);
        //      ~ContainerBase();
        //      ALLOCATOR&       allocatorRef();
        //      const ALLOCATOR& allocatorRef() const;
        // --------------------------------------------------------------------

        if (verbose) printf("\nCONSTRUCTOR AND BASIC ACCESSORS"
                            "\n===============================\n");

        if (veryVerbose) printf("statefull allocator\n");
        {
            typedef bsl::allocator<int>          Alloc;
            typedef bslalg::ContainerBase<Alloc> Obj;

            bslma::TestAllocator ta;
            Alloc                allocArg(&ta);

            // Construct with exact allocator type.
            Obj          cb1(allocArg); const Obj& CB1 = cb1;
            Alloc&       a1 = cb1.allocatorRef();
            const Alloc& A1 = CB1.allocatorRef();
            ASSERT(&a1 == &A1);
            ASSERT(a1 == allocArg);
            ASSERT(A1 == allocArg);

            // Construct using conversion to allocator type
            Obj cb2(&ta); const Obj& CB2 = cb2;
            ASSERT(&ta == cb2.allocatorRef().mechanism());
            ASSERT(&ta == CB2.allocatorRef().mechanism());
        }

        if (veryVerbose) printf("stateless allocator\n");
        {
            typedef StatelessSTLAllocator<int>   Alloc;
            typedef bslalg::ContainerBase<Alloc> Obj;

            Alloc        allocArg1;
            {
                Obj          cb1(allocArg1); const Obj& CB1 = cb1;
                Alloc&       a1 = cb1.allocatorRef();
                const Alloc& A1 = CB1.allocatorRef();
                ASSERT(&a1 == &A1);
                ASSERT(a1  == allocArg1);
                ASSERT(A1  == allocArg1);
                ASSERT(&A1        == g_lastStatelessSTLAllocatorConstructed);
                ASSERT(&allocArg1 == g_lastStatelessSTLAllocatorCopied);
            }
            ASSERT(g_lastStatelessSTLAllocatorDestroyed ==
                   g_lastStatelessSTLAllocatorConstructed);
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
        //: 1 Invoke all methods and verify their behavior.
        //
        // Testing:
        //      BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) printf("\nBREATHING TEST"
                            "\n==============\n");

        // Test with stateful allocator
        {
            bslma::TestAllocator ta;

            typedef bslalg::ContainerBase<bsl::allocator<TestType> > Obj;

            bsl::allocator<int>  a(&ta);
            bsl::allocator<int>  da;

            // A 'ContainerBase' is NOT an allocator.
            ASSERT(! bslma::IsStdAllocator<Obj>::value);

            Obj mX(a);  const Obj& X = mX;
            Obj mY(a);  const Obj& Y = mY;
            Obj mZ(da); const Obj& Z = mZ;
            ASSERT(a == mX.allocatorRef());
            ASSERT(a == X.allocatorRef());
            ASSERT(  X.equalAllocator(Y));
            ASSERT(! X.equalAllocator(Z));
        }

        // Test with stateless allocator
        {
            typedef bslalg::ContainerBase<StatelessSTLAllocator<int> > Obj;

            StatelessSTLAllocator<char> a;

            // A 'ContainerBase' is NOT an allocator.
            ASSERT(! bslma::IsStdAllocator<Obj>::value);

            Obj mX(a); const Obj& X = mX;
            Obj mY(a); const Obj& Y = mY;
            ASSERT(a == mX.allocatorRef());
            ASSERT(a == X.allocatorRef());
            ASSERT(X.equalAllocator(Y));
        }

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
