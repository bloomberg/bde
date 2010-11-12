// bslstl_allocator.t.cpp                  -*-C++-*-

#include <bslstl_allocator.h>

#include <bslalg_typetraits.h>            // testing only
#include <bslalg_hastrait.h>              // testing only
#include <bslma_allocator.h>              // testing only
#include <bslma_default.h>                // testing only
#include <bslma_defaultallocatorguard.h>  // testing only
#include <bslma_testallocator.h>          // testing only
#include <bslmf_issame.h>                 // testing only
#include <bsls_platform.h>                // testing only

#include <cstdio>
#include <cstdlib>

#include <new>

using namespace BloombergLP;
using namespace std;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// An allocator is a value-semantic type whose value consists of a single
// pointer to a 'bslma_Allocator' instance (its underlying "mechanism").  This
// pointer can be set at construction (and if 0 is passed, then it uses
// 'bslma_default' to substitute a pointer to the currently installed default
// allocator), and it can be accessed through the 'mechanism' accessor.  It
// cannot be reset, however, since normally an allocator does not change
// during the lifetime of an object.  A 'bsl::allocator' is parameterized
// by the type that it allocates, and that influences the behavior of several
// manipulators and accessors, mainly depending on the size of that type.  The
// same 'bsl::allocator" can be re-parameterized for another type
// ("rebound") using the 'rebind' nested template.
//
// Although 'bsl::allocator' is a value-semantic type, the fact that its
// value is fixed at construction and not permitted to change let us relax the
// usual concerns of a typical value-semantic type.  Our specific concerns are
// that an allocator constructed with a certain underlying mechanism actually
// uses that mechanism to allocate memory, and that its rebound versions also
// do.  Another concern is that the 'max_size' is the maximum possible size for
// that type (i.e., it is impossible to meaningfully pass in a larger size),
// and that the 'size_type' is unsigned, the 'difference_type' is signed, and
// generally all the requirements of C++ standard allocators are met (20.1.2
// [allocator.requirements]).
//-----------------------------------------------------------------------------
// [ 3] bsl::allocator();
// [ 3] bsl::allocator(bslma_Allocator *);
// [ 3] bsl::allocator(bsl::allocator const&);
// [ 3] bsl::allocator(bsl::allocator<U> const&);
// [ 3] ~bsl::allocator();
// [ 4] mechanism() const;
// [ 4] max_size() const;
// [ 5] bsl::allocator::size_type
// [ 5] bsl::allocator::difference_type
// [ 5] bsl::allocator::pointer;
// [ 5] bsl::allocator::const_pointer;
// [ 5] bsl::allocator::reference;
// [ 5] bsl::allocator::const_reference;
// [ 5] bsl::allocator::value_type;
// [ 5] template rebind<U>::other
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 6] USAGE EXAMPLE

//==========================================================================
//                  STANDARD BDE ASSERT TEST MACRO
//--------------------------------------------------------------------------
// NOTE: THIS IS A LOW-LEVEL COMPONENT AND MAY NOT USE ANY C++ LIBRARY
// FUNCTIONS, INCLUDING IOSTREAMS.
static int testStatus = 0;

static void aSsErT(int c, const char *s, int i) {
    if (c) {
        printf("Error " __FILE__ "(%d): %s    (failed)\n", i, s);
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}

# define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }
# define LOOP_ASSERT(I,X) { \
    if (!(X)) { printf("%s: %ld\n", #I, (long) I); aSsErT(1, #X, __LINE__); } }
//--------------------------------------------------------------------------

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
// #define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) printf("<| " #X " |>\n");  // Quote identifier literally.
//#define P_(X) cout << #X " = " << (X) << ", " << flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number
#define T_ printf("\t");             // Print a tab (w/o newline)

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

enum { VERBOSE_ARG_NUM = 2, VERY_VERBOSE_ARG_NUM, VERY_VERY_VERBOSE_ARG_NUM };

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

//=============================================================================
//                            USAGE EXAMPLE
//-----------------------------------------------------------------------------

///Usage
///-----
// We first show how to define a container type parameterized with an STL-style
// allocator template parameter.  For simplicity, we choose a fixed-size array
// to avoid issues concerning reallocation, dynamic growth, etc.  Furthermore,
// we do not assume the 'bslma' allocation protocol, which would dictate that
// we pass-through the allocator to the parameterized 'T' contained type (see
// the 'bslma_allocator' component and 'bslalg' package).  The interface would
// be as follows:
//..
    // my_fixedsizearray.h

                               // ======================
                               // class my_FixedSizeArray
                               // ======================

template <class T, class ALLOC>
    class my_FixedSizeArray {
        // This class provides an array of the parameterized 'T' type passed of
        // fixed length at construction, using an instance of the parameterized
        // 'ALLOC' type to supply memory.

        // DATA
    ALLOC  d_allocator;
    int    d_length;
    T     *d_array;

        // INVARIANTS

      public:
        // TYPES
    typedef ALLOC  allocator_type;
    typedef T      value_type;

    // CREATORS
        my_FixedSizeArray(int length, const ALLOC& allocator = ALLOC());
            // Create a fixed-size array of the specified 'length', using the
            // optionally specified 'allocator' to supply memory.  If
            // 'allocator' is not specified, a default-constructed instance of
            // the parameterized 'ALLOC' type is used.  Note that all the
            // elements in that array are default-constructed.

        my_FixedSizeArray(const my_FixedSizeArray& original,
                         const ALLOC&            allocator = ALLOC());
            // Create a copy of the specified 'original' fixed-size array,
            // using the optionally specified 'allocator' to supply memory.  If
            // 'allocator' is not specified, a default-constructed instance of
            // the parameterized 'ALLOC' type is used.

        ~my_FixedSizeArray();
            // Destroy this fixed size array.

        // MANIPULATORS
        T& operator[](int index);
            // Return a reference to the modifiable element at the specified
            // 'index' position in this fixed size array.

        // ACCESSORS
        const T& operator[](int index) const;
            // Return a reference to the modifiable element at the specified
            // 'index' position in this fixed size array.

    int length() const;
            // Return the length specified at construction of this fixed size
            // array.

    const ALLOC& allocator() const;
            // Return a reference to the non-modifiable allocator used by this
            // fixed size array to supply memory.  This is here for
            // illustrative purposes.  We should not generally have an accessor
            // to return the allocator.
    };

    // FREE OPERATORS
    template<class T, class ALLOC>
    bool operator==(const my_FixedSizeArray<T,ALLOC>& lhs,
                    const my_FixedSizeArray<T,ALLOC>& rhs);
        // Return 'true' if the specified 'lhs' fixed-size array has the same
        // value as the specified 'rhs' fixed-size array, and 'false'
        // otherwise.  Two fixed-size arrays have the same value if they have
        // the same length and if the element at any index in 'lhs' has the
        // same value as the corresponding element at the same index in 'rhs'.
//..
// The implementation is straightforward
//..
                           // ----------------------
                           // class my_FixedSizeArray
                           // ----------------------

    // CREATORS
    template<class T, class ALLOC>
    my_FixedSizeArray<T,ALLOC>::my_FixedSizeArray(int          length,
                                                  const ALLOC& alloc)
    : d_allocator(alloc), d_length(length)
{
    d_array = d_allocator.allocate(d_length);  // sizeof(T)*d_length bytes

    // Default construct each element of the array:
    for (int i = 0; i < d_length; ++i) {
        d_allocator.construct(&d_array[i], T());
    }
}

template<class T, class ALLOC>
    my_FixedSizeArray<T,ALLOC>::my_FixedSizeArray(
        const my_FixedSizeArray& rhs,
    const ALLOC&               alloc)
    : d_allocator(alloc), d_length(rhs.d_length)
{
    d_array = d_allocator.allocate(d_length);  // sizeof(T)*d_length bytes

    // copy construct each element of the array:
    for (int i = 0; i < d_length; ++i) {
        d_allocator.construct(&d_array[i], rhs.d_array[i]);
    }
}

template<class T, class ALLOC>
    my_FixedSizeArray<T,ALLOC>::~my_FixedSizeArray()
{
    // Call destructor for each element
    for (int i = 0; i < d_length; ++i) {
        d_allocator.destroy(&d_array[i]);
    }

    // Return memory to allocator.
    d_allocator.deallocate(d_array, d_length);
}

    // MANIPULATORS
    template<class T, class ALLOC>
    inline T& my_FixedSizeArray<T,ALLOC>::operator[](int i)
{
    return d_array[i];
}

    // ACCESSORS
template<class T, class ALLOC>
inline
    const T& my_FixedSizeArray<T,ALLOC>::operator[](int i) const
{
    return d_array[i];
}

template<class T, class ALLOC>
    inline int my_FixedSizeArray<T,ALLOC>::length() const
{
    return d_length;
}

template<class T, class ALLOC>
inline
    const ALLOC& my_FixedSizeArray<T,ALLOC>::allocator() const
{
    return d_allocator;
}

    // FREE OPERATORS
    template<class T, class ALLOC>
    bool operator==(const my_FixedSizeArray<T,ALLOC>& lhs,
                    const my_FixedSizeArray<T,ALLOC>& rhs)
{
    if (lhs.length() != rhs.length()) {
        return false;
    }
    for (int i = 0; i < lhs.length(); ++i) {
        if (lhs[i] != rhs[i]) {
            return false;
        }
    }
    return true;
}
//..
// Now we declare an allocator mechanism.  Our mechanism will be to simply call
// the global 'operator new' and 'operator delete' functions, and count the
// number of blocks outstanding (allocated but not deallocated).  Note that a
// more reusable implementation would take an underlying mechanism at
// construction.  We keep things simple only for the sake of this example.
//..
    // my_countingallocator.h

                             // ==========================
                             // class my_CountingAllocator
                             // ==========================

    class my_CountingAllocator : public bslma_Allocator {
        // This concrete implementation of the 'bslma_Allocator' protocol
        // maintains some statistics of the number of blocks outstanding (i.e.,
        // allocated but not yet deallocated).

        // DATA
    int d_blocksOutstanding;

      public:
        // CREATORS
        my_CountingAllocator();
        // Create a counting allocator that uses the operators 'new' and
        // 'delete' to supply and free memory.

        // MANIPULATORS
    virtual void *allocate(size_type size);
            // Return a pointer to an uninitialized memory of the specified
            // 'size (in bytes).

    virtual void deallocate(void *address);
            // Return the memory at the specified 'address' to this allocator.

        // ACCESSORS
        int blocksOutstanding() const;
            // Return the number of blocks outstanding (i.e., allocated but not
            // yet deallocated by this counting allocator).
    };
//..
// The implementation is really straightforward:
//..
    // my_countingallocator.cpp

                             // -------------------------
                             // class my_CountingAllocator
                             // -------------------------

    // CREATORS
    my_CountingAllocator::my_CountingAllocator()
    : d_blocksOutstanding(0)
{
}

    // MANIPULATORS
    void *my_CountingAllocator::allocate(size_type size)
{
    ++d_blocksOutstanding;
    return operator new(size);
}

    void my_CountingAllocator::deallocate(void *address)
{
    --d_blocksOutstanding;
    operator delete(address);
}

    // ACCESSORS
    int my_CountingAllocator::blocksOutstanding() const
    {
        return d_blocksOutstanding;
    }
//..
// Now we can create array objects with different allocator mechanisms.  First
// we create an array, 'a1', using the default allocator and fill it
// with the values '1 .. 5':
//..
    void usageExample() {

        my_FixedSizeArray<int, bsl::allocator<int> > a1(5);
                   ASSERT(5 == a1.length());
                   ASSERT(bslma_Default::defaultAllocator() == a1.allocator());

        for (int i = 0; i < a1.length(); ++i) {
            a1[i] = i + 1;
        }
//..
// Then we create a copy of 'a1' using the counting allocator.  The values of
// 'a1' and 'a2' are equal, even though they have different allocation
// mechanisms.
//..
        my_CountingAllocator countingAlloc;
        my_FixedSizeArray<int, bsl::allocator<int> > a2(a1,&countingAlloc);
                   ASSERT(a1 == a2);
                   ASSERT(a1.allocator() != a2.allocator());
                   ASSERT(&countingAlloc == a2.allocator());
                   ASSERT(1 == countingAlloc.blocksOutstanding())
    }
//..

                              // ===============
                              // struct MyObject
                              // ===============

struct MyObject
{
    // A non-trivial-sized object.

    // DATA
    int  d_i;
    char d_s[10];
};

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    // int veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;

    setbuf(stdout, NULL);    // Use unbuffered output

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:  // Zero is always the leading case.
      case 6: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE
        //
        // Concerns:  The usage example must compile end execute without
        //   errors.
        //
        // Plan:  Copy-paste the usage example and replace 'assert' by
        //   'ASSERT'.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) printf("\nUSAGE EXAMPLE"
                            "\n=============\n");

        usageExample();

      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING NESTED TYPES
        //
        // Concerns:
        //   o that 'size_type' is unsigned while 'difference_type' is signed.
        //   o that size_type and difference_type are the right size (i.e.,
        //     they can represent any difference of pointers in the memory
        //     model)
        //   o that all other types exist and are as specified by the C++
        //     standard
        //   o that if Y is X::rebind<U>::other, then Y::rebind<T>::other is
        //     the same type as X
        //
        // Plan: The testing is straightforward and follows the concerns.
        //
        // Testing:
        //   bsl::allocator::size_type
        //   bsl::allocator::difference_type
        //   bsl::allocator::pointer;
        //   bsl::allocator::const_pointer;
        //   bsl::allocator::reference;
        //   bsl::allocator::const_reference;
        //   bsl::allocator::value_type;
        //   template rebind<U>::other
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING NESTED TYPES"
                            "\n====================\n");

        typedef bsl::allocator<int>   AI;
        typedef bsl::allocator<float> AF;
        typedef bsl::allocator<void>  AV;

        if (verbose) printf("\tTesting 'size_type'.\n");
        {
            ASSERT(sizeof(AI::size_type) == sizeof(int*));
            ASSERT(sizeof(AV::size_type) == sizeof(void*));

            ASSERT(0 < ~(AI::size_type)0);
            ASSERT(0 < ~(AV::size_type)0);
        }

        if (verbose) printf("\tTesting 'difference_type'.\n");
        {
            ASSERT(sizeof(AI::difference_type) == sizeof(int*));
            ASSERT(sizeof(AV::difference_type) == sizeof(void*));

            ASSERT(0 > ~(AI::difference_type)0);
            ASSERT(0 > ~(AV::difference_type)0);
        }

        if (verbose) printf("\tTesting 'pointer'.\n");
        {
            ASSERT((bslmf_IsSame<AI::pointer, int*>::VALUE));
            ASSERT((bslmf_IsSame<AF::pointer, float*>::VALUE));
            ASSERT((bslmf_IsSame<AV::pointer, void*>::VALUE));
        }

        if (verbose) printf("\tTesting 'const_pointer'.\n");
        {
            ASSERT((bslmf_IsSame<AI::const_pointer, const int*>::VALUE));
            ASSERT((bslmf_IsSame<AF::const_pointer, const float*>::VALUE));
            ASSERT((bslmf_IsSame<AV::const_pointer, const void*>::VALUE));
        }

        if (verbose) printf("\tTesting 'reference'.\n");
        {
            ASSERT((bslmf_IsSame<AI::reference, int&>::VALUE));
            ASSERT((bslmf_IsSame<AF::reference, float&>::VALUE));
        }

        if (verbose) printf("\tTesting 'const_reference'.\n");
        {
            ASSERT((bslmf_IsSame<AI::const_reference, const int&>::VALUE));
            ASSERT((bslmf_IsSame<AF::const_reference, const float&>::VALUE));
        }

        if (verbose) printf("\tTesting 'value_type'.\n");
        {
            ASSERT((bslmf_IsSame<AI::value_type, int>::VALUE));
            ASSERT((bslmf_IsSame<AF::value_type, float>::VALUE));
            ASSERT((bslmf_IsSame<AV::value_type, void>::VALUE));
        }

        if (verbose) printf("\tTesting 'rebind'.\n");
        {
            ASSERT((bslmf_IsSame<AI::rebind<int  >::other, AI>::VALUE));
            ASSERT((bslmf_IsSame<AI::rebind<float>::other, AF>::VALUE));
            ASSERT((bslmf_IsSame<AI::rebind<void >::other, AV>::VALUE));
            ASSERT((bslmf_IsSame<AF::rebind<int  >::other, AI>::VALUE));
            ASSERT((bslmf_IsSame<AF::rebind<float>::other, AF>::VALUE));
            ASSERT((bslmf_IsSame<AF::rebind<void >::other, AV>::VALUE));
            ASSERT((bslmf_IsSame<AV::rebind<int  >::other, AI>::VALUE));
            ASSERT((bslmf_IsSame<AV::rebind<float>::other, AF>::VALUE));
            ASSERT((bslmf_IsSame<AV::rebind<void >::other, AV>::VALUE));
        }

      } break;
      case 4: {
        // --------------------------------------------------------------------
        // ACCESSORS TEST
        //
        // Concerns:
        //   o that the correct 'bslma_Allocator *' is returned by 'mechanism'.
        //   o that the result of 'max_size' fits and represents the maximum
        //     possible number of bytes in a 'bslma_Allocator::size_type'.
        //   o that all comparisons exist and resolve to comparing the
        //     mechanisms.
        //
        // Plan: The concerns are straightforward to test.
        //
        // Testing:
        //   mechanism();
        //   max_size();
        //   operator==(bsl::allocator<T>, bsl::allocator<T>);
        //   operator==(bslma_Allocator *,    bsl::allocator<T>);
        //   operator==(bsl::allocator<T>, bslma_Allocator*);
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING ACCESSORS"
                            "\n=================\n");

        if (verbose) printf("\tTesting 'mechanism()'.\n");
        {
        bslma_TestAllocator ta1(veryVeryVerbose), ta2(veryVeryVerbose);
        bslma_DefaultAllocatorGuard allocGuard(&ta1);

        bsl::allocator<int>  ai1;       ASSERT(&ta1 == ai1.mechanism());
        bsl::allocator<int>  ai2(&ta2); ASSERT(&ta2 == ai2.mechanism());
        bsl::allocator<int>  ai4(0);    ASSERT(&ta1 == ai4.mechanism());

        bsl::allocator<void> av1;       ASSERT(&ta1 == av1.mechanism());
        bsl::allocator<void> av2(&ta2); ASSERT(&ta2 == av2.mechanism());
        bsl::allocator<void> av4(0);    ASSERT(&ta1 == av4.mechanism());

        bsl::allocator<int>  ai5(av2);  ASSERT(&ta2 == ai5.mechanism());
        bsl::allocator<void> av5(ai2);  ASSERT(&ta2 == av5.mechanism());
        }

        if (verbose) printf("\tTesting 'max_size()'.\n");
        {
            // Note that these tests will work, no matter whether
            // 'bslma_Allocator::size_type' is signed or unsigned.

            typedef bslma_Allocator::size_type bsize;

            enum {
                BSLMA_SIZE_IS_SIGNED = ~bslma_Allocator::size_type(0) < 0,
                MAX_NUM_BYTES = ~std::size_t(0) /
                                                (BSLMA_SIZE_IS_SIGNED ? 2 : 1),
                MAX_ELEMENTS1 = MAX_NUM_BYTES / sizeof(char),
                MAX_ELEMENTS2 = (std::size_t)MAX_NUM_BYTES / sizeof(char)
            };

            if (verbose) {
                printf("Illustrating the reason for the cast in the"
                       " enumeration (on AIX 64-bit mode):\n");
                printf("\tBSLMA_SIZE_IS_SIGNED = %d\n", BSLMA_SIZE_IS_SIGNED);
                printf("\tMAX_NUM_BYTES = %ld\n", (bsize)MAX_NUM_BYTES);
                printf("\tMAX_ELEMENTS1 = %ld\n", (bsize)MAX_ELEMENTS1);
                printf("\tMAX_ELEMENTS2 = %ld\n", (bsize)MAX_ELEMENTS2);

                printf("Printing the same values as unsigned:\n");
                printf("\tBSLMA_SIZE_IS_SIGNED = %d\n", BSLMA_SIZE_IS_SIGNED);
                printf("\tMAX_NUM_BYTES = %lu\n", (bsize)MAX_NUM_BYTES);
                printf("\tMAX_ELEMENTS1 = %lu\n", (bsize)MAX_ELEMENTS1);
                printf("\tMAX_ELEMENTS2 = %lu\n", (bsize)MAX_ELEMENTS2);
            }

            bsl::allocator<char> charAlloc;
            bsize cas = charAlloc.max_size();
            bsize casplus = cas + 1;
            LOOP_ASSERT(cas, cas > 0);
            LOOP_ASSERT(casplus, casplus < cas); // overflow

            if (verbose) {
                printf("As long: cas = %ld, casplus = %ld.\n", cas, casplus);
                printf("As unsigned long: cas = %lu, casplus = %lu.\n",
                        cas, casplus);
            }

            bsl::allocator<MyObject> objAlloc;

            // Detect problem with MSVC in 64-bit mode, which can't do 64-bit
            // int arithmetic correctly for enums.
            ASSERT(objAlloc.max_size() < charAlloc.max_size());

            bsize oas = objAlloc.max_size();
            bsize oass = oas * sizeof(MyObject);
            bsize oassplus = oass + sizeof(MyObject);
            LOOP_ASSERT(oas, oas > 0);
            LOOP_ASSERT(oass, oass < cas);
            LOOP_ASSERT(oass, oass > oas);         // no overflow
            LOOP_ASSERT(oassplus, oassplus < oas); // overflow

            if (verbose) {
                printf("\tAs long: oas = %ld, oass = %ld, oassplus = %ld.\n",
                       oas, oass, oassplus);
                printf("\tAs unsigned long: oas = %lu, oass = %lu, "
                       "oassplus = %lu.\n", oas, oass, oassplus);
            }
        }

        if (verbose) printf("\tTesting 'operator=='.\n");
        {
            bslma_TestAllocator ta1(veryVeryVerbose), ta2(veryVeryVerbose);
            bslma_DefaultAllocatorGuard allocGuard(&ta1);

            bsl::allocator<int>  ai1(&ta1);
            bsl::allocator<int>  ai2(&ta2);

            bsl::allocator<void> av1(&ta1);
            bsl::allocator<void> av2(&ta2);

            // One of lhs or rhs is 'bslma_Allocator *'.

            ASSERT(&ta1 == ai1);  ASSERT(ai1 == &ta1);
            ASSERT(&ta2 != ai1);  ASSERT(ai1 != &ta2);

            ASSERT(&ta1 == ai1);  ASSERT(ai1 == &ta1);
            ASSERT(&ta2 != ai1);  ASSERT(ai1 != &ta2);

            ASSERT(&ta1 == av1);  ASSERT(av1 == &ta1);
            ASSERT(&ta2 != av1);  ASSERT(av1 != &ta2);

            ASSERT(&ta1 == av1);  ASSERT(av1 == &ta1);
            ASSERT(&ta2 != av1);  ASSERT(av1 != &ta2);

            // Both lhs and rhs are 'bsl::allocator'.

            ASSERT(ai1 == ai1);  ASSERT(ai1 != ai2);
            ASSERT(ai1 == av1);  ASSERT(ai1 != av2);

            ASSERT(av1 == ai1);  ASSERT(av1 != ai2);
            ASSERT(av1 == av1);  ASSERT(av1 != av2);

            ASSERT(ai2 != ai1);  ASSERT(ai2 == ai2);
            ASSERT(ai2 != av1);  ASSERT(ai2 == av2);

            ASSERT(av2 != ai1);  ASSERT(av2 == ai2);
            ASSERT(av2 != av1);  ASSERT(av2 == av2);
        }

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING CONSTRUCTORS
        //
        // Concerns:
        //   o that an allocator can be constructed from the various
        //     constructors and that it uses the correct mechanism object.
        //   o that an allocator can be constructed from an allocator to a
        //     different type
        //
        // Plan:  We construct a number of allocators from various mechanisms,
        //   and test that they do compare equal to the selected mechanism.
        //   Copy constructed allocators have to compare equal to their
        //   original values.

        //
        // Testing:
        //   bsl::allocator();
        //   bsl::allocator(bslma_Allocator *);
        //   bsl::allocator(const bsl::allocator&);
        //   bsl::allocator(const bsl::allocator<U>&);
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING CONSTRUCTORS"
                            "\n====================");

        bslma_TestAllocator ta1(veryVeryVerbose), ta2(veryVeryVerbose);
        bslma_DefaultAllocatorGuard allocGuard(&ta1);

        bsl::allocator<int>  ai1;        ASSERT(&ta1 == ai1);
        bsl::allocator<int>  ai2(&ta2);  ASSERT(&ta2 == ai2);
        bsl::allocator<int>  ai3(ai2);   ASSERT(&ta2 == ai3);
        bsl::allocator<int>  ai4(0);     ASSERT(&ta1 == ai4);

        bsl::allocator<void> av1;        ASSERT(&ta1 == av1);
        bsl::allocator<void> av2(&ta2);  ASSERT(&ta2 == av2);
        bsl::allocator<void> av3(av2);   ASSERT(&ta2 == av3);
        bsl::allocator<void> av4(0);     ASSERT(&ta1 == av4);

        bsl::allocator<int>  ai5(av2);   ASSERT(av2  == ai5);
        bsl::allocator<void> av5(ai2);   ASSERT(ai2  == av5);

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING TRAITS
        //
        // Concerns:
        //   That an allocator has the proper traits defined.
        //
        // Plan:  Since it does not matter what type 'bsl::allocator' is
        //   instantiated with, use 'int' and test for the traits using
        //   'bslalg_HasTrait'.  Note that 'void' also needs to be tested since
        //   it is a specialization.
        //
        // Testing:
        //   bslalg_TypeTrait<bsl::allocator>
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING TRAITS"
                            "\n==============");

        ASSERT((bslalg_HasTrait<bsl::allocator<int>,
                                bslalg_TypeTraitBitwiseMoveable>::VALUE));
        ASSERT((bslalg_HasTrait<bsl::allocator<int>,
                                bslalg_TypeTraitBitwiseCopyable>::VALUE));
        ASSERT((bslalg_HasTrait<
                           bsl::allocator<int>,
                           bslalg_TypeTraitBitwiseEqualityComparable>::VALUE));

        ASSERT((bslalg_HasTrait<bsl::allocator<void>,
                                bslalg_TypeTraitBitwiseMoveable>::VALUE));
        ASSERT((bslalg_HasTrait<bsl::allocator<void>,
                                bslalg_TypeTraitBitwiseCopyable>::VALUE));
        ASSERT((bslalg_HasTrait<
                           bsl::allocator<void>,
                           bslalg_TypeTraitBitwiseEqualityComparable>::VALUE));

      } break;

      case 1: {
        // --------------------------------------------------------------------
        // BREATHING/USAGE TEST
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        //    This test case exercises the component but *tests* nothing.
        // --------------------------------------------------------------------

        if (verbose) printf("\nBREATHING TEST"
                            "\n==============");

        my_FixedSizeArray<int, bsl::allocator<int> > a1(5);
                ASSERT(5 == a1.length());
                ASSERT(bslma_Default::defaultAllocator() == a1.allocator());

        for (int i = 0; i < a1.length(); ++i) {
            a1[i] = i + 1;
        }

        my_CountingAllocator countingAlloc;
        my_FixedSizeArray<int, bsl::allocator<int> >
            a2(a1, &countingAlloc);
                ASSERT(a1 == a2);
                ASSERT(a1.allocator() != a2.allocator());
                ASSERT(&countingAlloc == a2.allocator());
                ASSERT(1 == countingAlloc.blocksOutstanding());

        // Test that this will compile:

        bsl::allocator<void> voidAlloc(&countingAlloc);

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

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2008
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
