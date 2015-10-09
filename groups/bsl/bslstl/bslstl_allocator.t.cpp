// bslstl_allocator.t.cpp                                             -*-C++-*-

#include <bslstl_allocator.h>

#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>
#include <bsls_bsltestutil.h>

#include <limits>
#include <new>

#include <stdio.h>
#include <stdlib.h>

using namespace BloombergLP;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// An allocator is a value-semantic type whose value consists of a single
// pointer to a 'bslma::Allocator' object (its underlying "mechanism").  This
// pointer can be set at construction (and if 0 is passed, then it uses
// 'bslma_default' to substitute a pointer to the currently installed default
// allocator), and it can be accessed through the 'mechanism' accessor.  It
// cannot be reset, however, since normally an allocator does not change during
// the lifetime of an object.  A 'bsl::allocator' is parameterized by the type
// that it allocates, and that influences the behavior of several manipulators
// and accessors, mainly depending on the size of that type.  The same
// 'bsl::allocator" can be re-parameterized for another type ("rebound") using
// the 'rebind' nested template.
//
// Although 'bsl::allocator' is a value-semantic type, the fact that its value
// is fixed at construction and not permitted to change let us relax the usual
// concerns of a typical value-semantic type.  Our specific concerns are that
// an allocator constructed with a certain underlying mechanism actually uses
// that mechanism to allocate memory, and that its rebound versions also do.
// Another concern is that the 'max_size' is the maximum possible size for that
// type (i.e., it is impossible to meaningfully pass in a larger size), and
// that the 'size_type' is unsigned, the 'difference_type' is signed, and
// generally all the requirements of C++ standard allocators are met (20.1.2
// [allocator.requirements]).
//-----------------------------------------------------------------------------
// [ 3] bsl::allocator();
// [ 3] bsl::allocator(bslma::Allocator *);
// [ 3] bsl::allocator(const bsl::allocator&);
// [ 3] bsl::allocator(const bsl::allocator<U>&);
// [  ] ~bsl::allocator();
//
// Modifiers
// [  ] allocator& operator=(const allocator& rhs);
// [  ] pointer allocate(size_type n, const void *hint = 0);
// [  ] void deallocate(pointer p, size_type n = 1);
// [  ] void construct(pointer p, const TYPE& val);
// [  ] void destroy(pointer p);
//
// Accessors
// [  ] pointer address(reference x) const;
// [  ] const_pointer address(const_reference x) const;
// [ 4] bslma::Allocator *mechanism() const;
// [ 4] size_type max_size() const;
//
// Nested types
// [ 5] bsl::allocator::size_type
// [ 5] bsl::allocator::difference_type
// [ 5] bsl::allocator::pointer;
// [ 5] bsl::allocator::const_pointer;
// [ 5] bsl::allocator::reference;
// [ 5] bsl::allocator::const_reference;
// [ 5] bsl::allocator::value_type;
// [ 5] template rebind<U>::other
//
// Free functions (operators)
// [ 4] bool operator==(bsl::allocator<T>,  bsl::allocator<T>);
// [  ] bool operator==(bsl::allocator<T1>,  bsl::allocator<T2>);
// [ 4] bool operator==(bslma::Allocator *, bsl::allocator<T>);
// [ 4] bool operator==(bsl::allocator<T>,  bslma::Allocator*);
// [  ] bool operator!=(bsl::allocator<T1>,  bsl::allocator<T2>);
// [  ] bool operator!=(bslma::Allocator *, bsl::allocator<T>);
// [  ] bool operator!=(bsl::allocator<T>,  bslma::Allocator*);
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 6] USAGE EXAMPLE
// [ 2] bsl::is_trivially_copyable<bsl::allocator>
// [ 2] bslmf::IsBitwiseEqualityComparable<sl::allocator>
// [ 2] bslmf::IsBitwiseMoveable<bsl::allocator>

// ============================================================================
//                      STANDARD BDE ASSERT TEST MACROS
// ----------------------------------------------------------------------------
// NOTE: THIS IS A LOW-LEVEL COMPONENT AND MAY NOT USE ANY C++ LIBRARY
// FUNCTIONS, INCLUDING IOSTREAMS.

namespace {

int testStatus = 0;

void aSsErT(bool b, const char *s, int i)
{
    if (b) {
        printf("Error " __FILE__ "(%d): %s    (failed)\n", i, s);
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}

}  // close unnamed namespace

//=============================================================================
//                       STANDARD BDE TEST DRIVER MACROS
//-----------------------------------------------------------------------------

#define ASSERT       BSLS_BSLTESTUTIL_ASSERT
#define LOOP_ASSERT  BSLS_BSLTESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLS_BSLTESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLS_BSLTESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLS_BSLTESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLS_BSLTESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLS_BSLTESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLS_BSLTESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLS_BSLTESTUTIL_LOOP6_ASSERT
#define ASSERTV      BSLS_BSLTESTUTIL_ASSERTV

#define Q   BSLS_BSLTESTUTIL_Q   // Quote identifier literally.
#define P   BSLS_BSLTESTUTIL_P   // Print identifier and value.
#define P_  BSLS_BSLTESTUTIL_P_  // P(X) without '\n'.
#define T_  BSLS_BSLTESTUTIL_T_  // Print a tab (w/o newline).
#define L_  BSLS_BSLTESTUTIL_L_  // current Line number

// ============================================================================
//                  NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)
#define ASSERT_PASS(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
#define ASSERT_FAIL(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)
#define ASSERT_OPT_PASS(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS(EXPR)
#define ASSERT_OPT_FAIL(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL(EXPR)

#define ASSERT_SAFE_PASS_RAW(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS_RAW(EXPR)
#define ASSERT_SAFE_FAIL_RAW(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL_RAW(EXPR)
#define ASSERT_PASS_RAW(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS_RAW(EXPR)
#define ASSERT_FAIL_RAW(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL_RAW(EXPR)
#define ASSERT_OPT_PASS_RAW(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(EXPR)
#define ASSERT_OPT_FAIL_RAW(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(EXPR)

// ============================================================================
//                           PRINTF FORMAT MACROS
// ----------------------------------------------------------------------------
#define ZU BSLS_BSLTESTUTIL_FORMAT_ZU

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

                               // =======================
                               // class my_FixedSizeArray
                               // =======================

    template <class TYPE, class ALLOC>
    class my_FixedSizeArray {
        // This class provides an array of (the template parameter) 'TYPE'
        // passed of fixed length at construction, using an instance of the
        // parameterized 'ALLOC' type to supply memory.

        // DATA
        ALLOC  d_allocator;
        int    d_length;
        TYPE  *d_array;

        // INVARIANTS

      public:
        // TYPES
        typedef ALLOC  allocator_type;
        typedef TYPE   value_type;

        // CREATORS
        explicit my_FixedSizeArray(int length,
                                   const ALLOC& allocator = ALLOC());
            // Create a fixed-size array of the specified 'length', using the
            // optionally specified 'allocator' to supply memory.  If
            // 'allocator' is not specified, a default-constructed instance of
            // the parameterized 'ALLOC' type is used.  Note that all the
            // elements in that array are default-constructed.

        my_FixedSizeArray(const my_FixedSizeArray& original,
                          const ALLOC&             allocator = ALLOC());
            // Create a copy of the specified 'original' fixed-size array,
            // using the optionally specified 'allocator' to supply memory.  If
            // 'allocator' is not specified, a default-constructed instance of
            // the parameterized 'ALLOC' type is used.

        ~my_FixedSizeArray();
            // Destroy this fixed size array.

        // MANIPULATORS
        TYPE& operator[](int index);
            // Return a reference to the modifiable element at the specified
            // 'index' position in this fixed size array.

        // ACCESSORS
        const TYPE& operator[](int index) const;
            // Return a reference to the modifiable element at the specified
            // 'index' position in this fixed size array.

        const ALLOC& allocator() const;
            // Return a reference to the non-modifiable allocator used by this
            // fixed size array to supply memory.  This is here for
            // illustrative purposes.  We should not generally have an accessor
            // to return the allocator.

        int length() const;
            // Return the length specified at construction of this fixed size
            // array.
    };

    // FREE OPERATORS
    template<class TYPE, class ALLOC>
    bool operator==(const my_FixedSizeArray<TYPE, ALLOC>& lhs,
                    const my_FixedSizeArray<TYPE, ALLOC>& rhs);
        // Return 'true' if the specified 'lhs' fixed-size array has the same
        // value as the specified 'rhs' fixed-size array, and 'false'
        // otherwise.  Two fixed-size arrays have the same value if they have
        // the same length and if the element at any index in 'lhs' has the
        // same value as the corresponding element at the same index in 'rhs'.


    namespace BloombergLP {
    namespace bslma {

    template <class TYPE, class ALLOC>
    struct UsesBslmaAllocator< my_FixedSizeArray<TYPE, ALLOC> >
    : bsl::is_convertible<Allocator*, ALLOC>::type
    {
    };

    }  // close namespace bslma
    }  // close enterprise namespace

//..
// The implementation is straightforward
//..
                           // ----------------------
                           // class my_FixedSizeArray
                           // ----------------------

    // CREATORS
    template<class TYPE, class ALLOC>
    my_FixedSizeArray<TYPE, ALLOC>::my_FixedSizeArray(int          length,
                                                      const ALLOC& allocator)
    : d_allocator(allocator), d_length(length)
    {
        d_array = d_allocator.allocate(d_length);  // sizeof(T)*d_length bytes

        // Default construct each element of the array:
        for (int i = 0; i < d_length; ++i) {
            d_allocator.construct(&d_array[i], TYPE());
        }
    }

    template<class TYPE, class ALLOC>
    my_FixedSizeArray<TYPE, ALLOC>::my_FixedSizeArray(
                                            const my_FixedSizeArray& original,
                                            const ALLOC&             allocator)
    : d_allocator(allocator), d_length(original.d_length)
    {
        d_array = d_allocator.allocate(d_length);  // sizeof(T)*d_length bytes

        // copy construct each element of the array:
        for (int i = 0; i < d_length; ++i) {
            d_allocator.construct(&d_array[i], original.d_array[i]);
        }
    }

    template<class TYPE, class ALLOC>
    my_FixedSizeArray<TYPE, ALLOC>::~my_FixedSizeArray()
    {
        // Call destructor for each element
        for (int i = 0; i < d_length; ++i) {
            d_allocator.destroy(&d_array[i]);
        }

        // Return memory to allocator.
        d_allocator.deallocate(d_array, d_length);
    }

    // MANIPULATORS
    template<class TYPE, class ALLOC>
    inline TYPE& my_FixedSizeArray<TYPE, ALLOC>::operator[](int index)
    {
        return d_array[index];
    }

    // ACCESSORS
    template<class TYPE, class ALLOC>
    inline
    const TYPE& my_FixedSizeArray<TYPE, ALLOC>::operator[](int index) const
    {
        return d_array[index];
    }

    template<class TYPE, class ALLOC>
    inline
    const ALLOC& my_FixedSizeArray<TYPE, ALLOC>::allocator() const
    {
        return d_allocator;
    }

    // FREE OPERATORS
    template<class TYPE, class ALLOC>
    bool operator==(const my_FixedSizeArray<TYPE, ALLOC>& lhs,
                    const my_FixedSizeArray<TYPE, ALLOC>& rhs)
    {
        if (lhs.length() != rhs.length()) {
            return false;                                             // RETURN
        }
        for (int i = 0; i < lhs.length(); ++i) {
            if (lhs[i] != rhs[i]) {
                return false;                                         // RETURN
            }
        }
        return true;
    }

    template<class TYPE, class ALLOC>
    inline int my_FixedSizeArray<TYPE, ALLOC>::length() const
    {
        return d_length;
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

    class my_CountingAllocator : public bslma::Allocator {
        // This concrete implementation of the 'bslma::Allocator' protocol
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
// we create an array, 'a1', using the default allocator and fill it with the
// values '1 .. 5':
//..
    void usageExample() {

        my_FixedSizeArray<int, bsl::allocator<int> > a1(5);
        ASSERT(5 == a1.length());
        ASSERT(bslma::Default::defaultAllocator() == a1.allocator());

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
    int                 test = argc > 1 ? atoi(argv[1]) : 0;
    bool             verbose = argc > 2;
    bool         veryVerbose = argc > 3;
    bool     veryVeryVerbose = argc > 4;
    bool veryVeryVeryVerbose = argc > 5;

    (void) veryVerbose;
    (void) veryVeryVerbose;

    bslma::TestAllocator globalAllocator("global", veryVeryVeryVerbose);
    bslma::Default::setGlobalAllocator(&globalAllocator);

    bslma::TestAllocator defaultAllocator("default", veryVeryVeryVerbose);
    bslma::Default::setDefaultAllocator(&defaultAllocator);

    setbuf(stdout, NULL);    // Use unbuffered output

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:  // Zero is always the leading case.
      case 6: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
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

        bslma::TestAllocator ta("default for usage", veryVeryVeryVerbose);
        bslma::DefaultAllocatorGuard allocGuard(&ta);

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
            ASSERT((bsl::is_same<AI::pointer, int*>::value));
            ASSERT((bsl::is_same<AF::pointer, float*>::value));
            ASSERT((bsl::is_same<AV::pointer, void*>::value));
        }

        if (verbose) printf("\tTesting 'const_pointer'.\n");
        {
            ASSERT((bsl::is_same<AI::const_pointer, const int*>::value));
            ASSERT((bsl::is_same<AF::const_pointer, const float*>::value));
            ASSERT((bsl::is_same<AV::const_pointer, const void*>::value));
        }

        if (verbose) printf("\tTesting 'reference'.\n");
        {
            ASSERT((bsl::is_same<AI::reference, int&>::value));
            ASSERT((bsl::is_same<AF::reference, float&>::value));
        }

        if (verbose) printf("\tTesting 'const_reference'.\n");
        {
            ASSERT((bsl::is_same<AI::const_reference, const int&>::value));
            ASSERT((bsl::is_same<AF::const_reference, const float&>::value));
        }

        if (verbose) printf("\tTesting 'value_type'.\n");
        {
            ASSERT((bsl::is_same<AI::value_type, int>::value));
            ASSERT((bsl::is_same<AF::value_type, float>::value));
            ASSERT((bsl::is_same<AV::value_type, void>::value));
        }

        if (verbose) printf("\tTesting 'rebind'.\n");
        {
            ASSERT((bsl::is_same<AI::rebind<int  >::other, AI>::value));
            ASSERT((bsl::is_same<AI::rebind<float>::other, AF>::value));
            ASSERT((bsl::is_same<AI::rebind<void >::other, AV>::value));
            ASSERT((bsl::is_same<AF::rebind<int  >::other, AI>::value));
            ASSERT((bsl::is_same<AF::rebind<float>::other, AF>::value));
            ASSERT((bsl::is_same<AF::rebind<void >::other, AV>::value));
            ASSERT((bsl::is_same<AV::rebind<int  >::other, AI>::value));
            ASSERT((bsl::is_same<AV::rebind<float>::other, AF>::value));
            ASSERT((bsl::is_same<AV::rebind<void >::other, AV>::value));
        }

      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING ACCESSORS
        //
        // Concerns:
        //   o that the correct 'bslma::Allocator*' is returned by 'mechanism'.
        //   o that the result of 'max_size' fits and represents the maximum
        //     possible number of bytes in a 'bslma::Allocator::size_type'.
        //   o that all comparisons exist and resolve to comparing the
        //     mechanisms.
        //
        // Plan: The concerns are straightforward to test.
        //
        // Testing:
        //   bslma::Allocator *mechanism() const;
        //   size_type max_size() const;
        //   bool operator==(bsl::allocator<T>,  bsl::allocator<T>);
        //   bool operator==(bslma::Allocator *, bsl::allocator<T>);
        //   bool operator==(bsl::allocator<T>,  bslma::Allocator*);
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING ACCESSORS"
                            "\n=================\n");

        bslma::Allocator *dflt = bslma::Default::allocator();

        if (verbose) printf("\tTesting 'mechanism()'.\n");
        {
            bslma::TestAllocator ta(veryVeryVeryVerbose);

            bsl::allocator<int>  ai1;       ASSERT(dflt == ai1.mechanism());
            bsl::allocator<int>  ai2(&ta);  ASSERT(&ta  == ai2.mechanism());
            bsl::allocator<int>  ai4(0);    ASSERT(dflt == ai4.mechanism());

            bsl::allocator<void> av1;       ASSERT(dflt == av1.mechanism());
            bsl::allocator<void> av2(&ta);  ASSERT(&ta  == av2.mechanism());
            bsl::allocator<void> av4(0);    ASSERT(dflt == av4.mechanism());

            bsl::allocator<int>  ai5(av2);  ASSERT(&ta  == ai5.mechanism());
            bsl::allocator<void> av5(ai2);  ASSERT(&ta  == av5.mechanism());
        }

        if (verbose) printf("\tTesting 'max_size()'.\n");
        {
            typedef bslma::Allocator::size_type bsize;

            bsl::allocator<char> charAlloc;
            bsize cas = charAlloc.max_size();

            // verify that max_size() is the largest positive integer of type
            // size_type
            LOOP_ASSERT(cas, cas > 0);
            LOOP_ASSERT(cas, cas == std::numeric_limits<bsize>::max());

            if (verbose) {
                printf("cas = " ZU "\n", cas);
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
                printf("\tAs unsigned long: oas = " ZU ", oass = " ZU ", "
                       "oassplus = " ZU ".\n", oas, oass, oassplus);
            }
        }

        if (verbose) printf("\tTesting 'operator=='.\n");
        {
            bslma::TestAllocator ta1(veryVeryVeryVerbose);
            bslma::TestAllocator ta2(veryVeryVeryVerbose);

            bsl::allocator<int>  ai1(&ta1);
            bsl::allocator<int>  ai2(&ta2);

            bsl::allocator<void> av1(&ta1);
            bsl::allocator<void> av2(&ta2);

            // One of lhs or rhs is 'bslma::Allocator *'.

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
        //   bsl::allocator(bslma::Allocator *);
        //   bsl::allocator(const bsl::allocator&);
        //   bsl::allocator(const bsl::allocator<U>&);
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING CONSTRUCTORS"
                            "\n====================\n");

        bslma::Allocator *dflt = bslma::Default::allocator();

        bslma::TestAllocator ta("test case 3", veryVeryVeryVerbose);

        bsl::allocator<int>  ai1;        ASSERT(dflt == ai1);
        bsl::allocator<int>  ai2(&ta);   ASSERT(&ta  == ai2);
        bsl::allocator<int>  ai3(ai2);   ASSERT(&ta  == ai3);
        bsl::allocator<int>  ai4(0);     ASSERT(dflt == ai4);

        bsl::allocator<void> av1;        ASSERT(dflt == av1);
        bsl::allocator<void> av2(&ta);   ASSERT(&ta  == av2);
        bsl::allocator<void> av3(av2);   ASSERT(&ta  == av3);
        bsl::allocator<void> av4(0);     ASSERT(dflt == av4);

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
        // Plan: Since it does not matter what type 'bsl::allocator' is
        // instantiated with, use 'int' and test for each expected trait.
        // Note that 'void' also needs to be tested since it is a
        // specialization.
        //
        // Testing:
        //   bsl::is_trivially_copyable<bsl::allocator>
        //   bslmf::IsBitwiseEqualityComparable<sl::allocator>
        //   bslmf::IsBitwiseMoveable<bsl::allocator>
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING TRAITS"
                            "\n==============\n");

        ASSERT((bslmf::IsBitwiseMoveable<bsl::allocator<int> >::value));
        ASSERT((bsl::is_trivially_copyable<bsl::allocator<int> >::value));
        ASSERT((bslmf::IsBitwiseEqualityComparable<
                                                bsl::allocator<int> >::value));

        ASSERT((bslmf::IsBitwiseMoveable<bsl::allocator<void> >::value));
        ASSERT((bsl::is_trivially_copyable<bsl::allocator<void> >::value));
        ASSERT((bslmf::IsBitwiseEqualityComparable<
                                               bsl::allocator<void> >::value));

      } break;

      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //    This test case exercises the component but *tests* nothing.
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) printf("\nBREATHING TEST"
                            "\n==============\n");

        bslma::TestAllocator ta("breathing test", veryVeryVeryVerbose);

        my_FixedSizeArray<int, bsl::allocator<int> > a1(5, &ta);

        ASSERT(5 == a1.length());
//        ASSERT(bslma::Default::defaultAllocator() == a1.allocator());
        ASSERT(&ta == a1.allocator());

        for (int i = 0; i < a1.length(); ++i) {
            a1[i] = i + 1;
        }

        my_CountingAllocator countingAlloc;
        my_FixedSizeArray<int, bsl::allocator<int> > a2(a1, &countingAlloc);

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

    // CONCERN: In no case does memory come from the default allocator.
    ASSERTV(defaultAllocator.numBlocksTotal(),
            0 == defaultAllocator.numBlocksTotal());

    // CONCERN: In no case does memory come from the global allocator.
    ASSERTV(globalAllocator.numBlocksTotal(),
            0 == globalAllocator.numBlocksTotal());

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
