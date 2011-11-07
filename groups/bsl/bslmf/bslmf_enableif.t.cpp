// bslmf_enableif.t.cpp                                               -*-C++-*-

#include <bslmf_enableif.h>

#include <bslmf_isconvertible.h>
#include <bslmf_isfundamental.h>
#include <bslmf_ispolymorphic.h>
#include <bslmf_issame.h>
#include <bsls_bsltestutil.h>

// limit ourselves to the "C" library for packages below 'bslstl'
#include <iso646.h>  // required only by the Microsoft compiler
#include <limits.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

using namespace BloombergLP;
using namespace std;

//=============================================================================
//                                TEST PLAN
//-----------------------------------------------------------------------------
//                                Overview
//                                --------
// The component under test contains the simple meta-function 'bslmf_If'.  The
// meta-function is tested by enumeration of all combinations of (1) 'true' and
// 'false' conditions, and (2) defaulting of zero, one, or two of the type
// arguments to 'bslmf_Nil'.
//-----------------------------------------------------------------------------
// [ 1] bslmf_If<CONDITION, IF_TRUE_TYPE, IF_FALSE_TYPE>

//=============================================================================
//                  STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
// NOTE: THIS IS A LOW-LEVEL COMPONENT AND MAY NOT USE ANY C++ LIBRARY
// FUNCTIONS, INCLUDING IOSTREAMS.
static int testStatus = 0;

static void aSsErT(bool b, const char *s, int i) {
    if (b) {
        printf("Error " __FILE__ "(%d): %s    (failed)\n", i, s);
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}

# define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

//=============================================================================
//                       STANDARD BDE TEST DRIVER MACROS
//-----------------------------------------------------------------------------
#define LOOP_ASSERT  BSLS_BSLTESTUTIL_LOOP_ASSERT
#define LOOP2_ASSERT BSLS_BSLTESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLS_BSLTESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLS_BSLTESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLS_BSLTESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLS_BSLTESTUTIL_LOOP6_ASSERT

#define Q   BSLS_BSLTESTUTIL_Q   // Quote identifier literally.
#define P   BSLS_BSLTESTUTIL_P   // Print identifier and value.
#define P_  BSLS_BSLTESTUTIL_P_  // P(X) without '\n'.
#define T_  BSLS_BSLTESTUTIL_T_  // Print a tab (w/o newline).
#define L_  BSLS_BSLTESTUTIL_L_  // current Line number

//=============================================================================
//                    GLOBAL TEST TYPES AND FUNCTIONS
//-----------------------------------------------------------------------------

class DummyClass {
};

template <bool COND>
void testFunction() {}

template <bool COND>
typename bslmf_EnableIf<COND, void>::type testFunction(){}


template <bool COND>
typename bslmf_EnableIf<COND, int>::type testMutuallyExclusiveFunction()
{
    return 1;
}

template <bool COND>
typename bslmf_EnableIf<!COND, int>::type testMutuallyExclusiveFunction()
{
    return 2;
}


//=============================================================================
//                GLOBAL TYPES AND FUNCTIONS FOR USAGE EXAMPLE
//-----------------------------------------------------------------------------

///Usage
//------
// The following snippets of code illustrate basic use of the 'bslmf_EnableIf'
// meta-function.  We will demonstrate how to use this utility to control
// overload sets with three increasingly complex examples.
//
// For the first example we will implement a simple 'Swap' function template
// to exchange two arbitrary values, as if declared as below:
//..
    template<class T>
    void DummySwap(T& a, T&b)
        // Exchange the values of the specified objects, 'a' and 'b'.
    {
        T temp(a);
        a = b;
        b = temp;
    }
//..
// However, we want to take advantage of member-swap methods supplied by user-
// defined types, so we define a trait that can be customized by a class
// implementer to indicate that their class supports an optimized member-swap
// method:
//..
    template<class T>
    struct HasMemberSwap {
        // This traits class indicates whether the specified template type
        // parameter 'T' has a public 'swap' method to exchange values.
        static const bool VALUE = false;
    };
//..
// Now we can implement a generic 'Swap' function template that will invoke the
// member swap operation for any type that specialized our trait.  The use of
// 'bslmf_EnableIf' to declare the result type causes an attempt to deduce the
// type 'T' to fail unless the specified condition is 'true', and this falls
// under the "Substitution Failure Is Not An Error" (SFINAE) clause of the C++
// standard, so the compiler will look for a more suitable overload rather than
// fail with an error.  Note that we provide two overloaded declarations that
// appear to differ only in their return type, which would normally raise an
// ambiguity error.  This works, and is in fact required, in this case as the
// "enable-if" conditions are mutually exclusive, so that only one overload
// will ever be present in an overload set.
//..
    template<class T>
    typename bslmf_EnableIf<HasMemberSwap<T>::VALUE>::type
    Swap(T& a, T& b)
    {
        a.swap(b);
    }

    template<class T>
    typename bslmf_EnableIf< ! HasMemberSwap<T>::VALUE>::type
    Swap(T& a, T& b)
    {
        T temp(a);
        a = b;
        b = temp;
    }
//..
// Next we define a simple container template, that supports an optimized
// 'swap' operation by merely swapping the internal pointers, rather than
// exchanging each element:
//..
    template<class T>
    class MyContainer {
        // This is a simple container implementation for demonstration purposes
        // that is modeled after 'std::vector'.
        T *d_storage;
        size_t d_length;

        // Copy operations are declared private and not defined.
        MyContainer(const MyContainer&);
        MyContainer& operator=(const MyContainer&);

    public:
        MyContainer(const T& value, int n);
            // Create a 'MyContainer' object having the specified 'n' copies of
            // the specified 'value'.

        ~MyContainer();
            // Destroy this container and all of its elements, reclaiming any
            // allocated memory.

        void swap(MyContainer &other);
            // Exchange the contents of 'this' container with those of the
            // specified 'other'.  No memory will be allocated, and no
            // exceptions are thrown.

        const T& front() const;
            // Return a reference with 'const' access to the first element in
            // this container.

        size_t size() const;
            // Return the number of elements held by this container.
    };
//..
// Then we specialize our 'HasMemberSwap' trait for this new container type.
//..
    template<class T>
    struct HasMemberSwap<MyContainer<T> > {
        static const bool VALUE = true;
    };
//..
// Next we implement the methods of this class:
//..
    template<class T>
    MyContainer<T>::MyContainer(const T& value, int n)
    : d_storage(new T[n])
    , d_length(n)
    {
        for (int i = 0; i !=n; ++i) {
            d_storage[i] = value;
        }
    }

    template<class T>
    MyContainer<T>::~MyContainer()
    {
        delete[] d_storage;
    }

    template<class T>
    void MyContainer<T>::swap(MyContainer& other)
    {
        Swap(d_storage, other.d_storage);
        Swap(d_length,  other.d_length);
    }

    template<class T>
    const T& MyContainer<T>::front() const
    {
        return d_storage[0];
    }

    template<class T>
    size_t MyContainer<T>::size() const
    {
        return d_length;
    }
//..
// Finally we can test that the member-'swap' method is called by the generic
// 'Swap' function.  Note that the following code will not compile unless the
// member-function 'swap' is used, as the copy constructor and assignment
// operator for the 'MyContainer' class template are declared as private.
//..
    void TestSwap()
    {
        MyContainer<int> x(3, 14);
        MyContainer<int> y(2, 78);
        ASSERT(14 == x.size());
        ASSERT( 3 == x.front());
        ASSERT(78 == y.size());
        ASSERT( 2 == y.front());

        Swap(x, y);

        ASSERT(78 == x.size());
        ASSERT( 2 == x.front());
        ASSERT(14 == y.size());
        ASSERT( 3 == y.front());
   }
//..
// For the next example, we will demonstrate the use of the second template
// parameter in the 'bslmf_EnableIf' template, which serves as the "result"
// type if the test condition passes.  Assume we want to write a generic
// function to allow us to cast between pointers of different types.  If the
// types are polymorphic, we can use 'dynamic_cast' to potentially cast between
// two seemingly unrelated types.  However, if either type is not polymorphic
// then the attempt to use 'dynamic_cast' would be a compile-time failure, and
// we must use 'static_cast' instead.
//
//..
    template<class TO, class FROM>
    typename bslmf_EnableIf<bslmf_IsPolymorphic<FROM>::VALUE &&
                                                bslmf_IsPolymorphic<TO>::VALUE,
                            TO>::type *
    smart_cast(FROM *from)
        // Returns a pointer to the specified 'TO' type if the specified 'from'
        // pointer refers to an object whose complete class publicly derives,
        // directly or indirectly, from 'TO', and a null pointer otherwise.
    {
        return dynamic_cast<TO *>(from);
    }

    template<class TO, class FROM>
    typename bslmf_EnableIf<not(bslmf_IsPolymorphic<FROM>::VALUE &&
                                               bslmf_IsPolymorphic<TO>::VALUE),
                            TO>::type *
    smart_cast(FROM *from)
        // Return the specified 'from' pointer value cast as a pointer to type
        // 'TO'.  Behavior is undefined unless such a conversion is valid.
    {
        return static_cast<TO *>(from);
    }
//..
// Next we define a small number of classes to demonstrate that this casting
// utility works correctly:
//..
    class A {
        // Sample non-polymorphic class

      public:
        ~A() {}
    };

    class B {
        // Sample polymorphic base-class

      public:
        virtual ~B() {}
    };

    class C {
        // Sample polymorphic base-class

      public:
        virtual ~C() {}
    };

    class ABC : public A, public B, public C {
        // Most-derived example class using multiple bases in order to
        // demonstrate cross-casting.
    };
//..
// Finally we demonstrate the correct behavior of the 'smart_cast' utility:
//..
    void TestSmartCast()
    {
        ABC object;
        ABC *pABC = &object;
        A   *pA   = &object;
        B   *pB   = &object;
        C   *pC   = &object;

        A *pA2 = smart_cast<A>(pABC);
        B *pB2 = smart_cast<B>(pC);
        C *pC2 = smart_cast<C>(pB);

        ASSERT(&object == pA2);
        ASSERT(&object == pB2);
        ASSERT(&object == pC2);

        // These lines would fail to compile
        // A *pA3 = smart_cast<A>(pB);
        // C *pC3 = smart_cast<C>(pA);
    }
//..
// The final example demonstrates controlling the selection of a constructor
// template in a class with (potentially) many constructors.  We define a
// simple container template based on 'std::vector', that illustrates a problem
// that may occur when trying to call the constructor the user expects.  For
// this example, assume we are trying to create a 'vector<int>' with '42'
// copies of the value '13'.  When we pass the literal values '42' and '13' the
// compiler, the "best" candidate constructor should be the template
// constructor that takes two arguments of the same kind, deducing that type to
// be 'int'.  Unfortunately, that constructor expects those values to be of an
// iterator type, forming a valid range.  We need to avoid calling this
// constructor unless the deduced type really is an iterator, otherwise a
// compile-error will occur trying to instantiate that constructor with an
// incompatible argument type.  We use 'bslmf_EnableIf' to create a deduction
// context where SFINAE can kick in.  Note that we cannot deduce the '::type'
// result of a metafunction, and there is no result type (as with a regular
// function) to decorate, so we add an extra dummy argument using a pointer
// type (produced from 'bslma_EnableIf::type') with a default null argument:
//..
    template<class T>
    class MyVector {
        // This is a simple container implementation for demonstration purposes
        // that is modeled after 'std::vector'.
        T *d_storage;
        size_t d_length;

        // Copy operations are declared private and not defined.
        MyVector(const MyVector&);
        MyVector& operator=(const MyVector&);

    public:
        MyVector(const T& value, int n);
            // Create a 'MyVector' object having the specified 'n' copies of
            // the specified 'value'.

        template<typename FORWARD_ITERATOR>
        MyVector(FORWARD_ITERATOR first, FORWARD_ITERATOR last,
                    typename bslmf_EnableIf<
                                  !bslmf_IsFundamental<FORWARD_ITERATOR>::VALUE
                                                                 >::type * = 0)
            // Create a 'MyVector' object having the same sequence of values as
            // found in range described by the iterators '[first, last)'.
       {
           d_length = 0;
           for (FORWARD_ITERATOR cursor = first; cursor != last; ++cursor) {
               ++d_length;
           }

           d_storage = new T[d_length];
           for (int i = 0; i != d_length; ++i) {
              d_storage[i] = *first++;
           }
        }

        ~MyVector();
            // Destroy this container and all of its elements, reclaiming any
            // allocated memory.

        const T& operator[](int index) const;
            // Return a reference with 'const' access to the element held by
            // this container at the specified 'index'.

        int size() const;
            // Return the number of elements held by this container.
    };
//..
// Note that there is no easy test for whether a type is an iterator, so we
// assume any attempt to call a constructor with two arguments that are not
// fundamental (such as int) must be passing iterators.  Now that we have
// defined the class template, we implement its methods:
//..
    template<class T>
    MyVector<T>::MyVector(const T& value, int n)
    : d_storage(new T[n])
    , d_length(n)
    {
        for (int i = 0; i !=n; ++i) {
            d_storage[i] = value;
        }
    }

    template<class T>
    MyVector<T>::~MyVector()
    {
        delete[] d_storage;
    }

    template<class T>
    const T& MyVector<T>::operator[](int index) const
    {
        return d_storage[index];
    }

    template<class T>
    int MyVector<T>::size() const
    {
        return d_length;
    }
//..
// Finally we demonstrate that the correct constructors are called when invoked
// with appropriate arguments.
//..
    void TestContainerConstructor()
    {
        const unsigned int TEST_DATA[] = { 1, 2, 3, 4, 5 };

        const MyVector<unsigned int> x(&TEST_DATA[0], &TEST_DATA[5]);
        const MyVector<unsigned int> y(13, 42);

        ASSERT(5 == x.size());
        for(int i = 0; i != 5; ++i) {
            ASSERT(TEST_DATA[i] == x[i]);
        }

        ASSERT(42 == y.size());
        for(int i = 0; i != 42; ++i) {
            ASSERT(13 == y[i]);
        }
    }
//..

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;

    setbuf(stdout, 0);    // Use unbuffered output

    printf("TEST " __FILE__ " CASE %d\n", test);


    switch (test) { case 0:  // Zero is always the leading case.
     case 4: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE 3
        //   This case verifies that the usage example works as advertised.
        //
        // Concerns:
        //   That the usage example compiles, links, and runs as expected.
        //
        // Plan:
        //   Replicate the usage example from the component above, and invoke
        //   the test function here.
        //
        // Testing:
        //   USAGE EXAMPLE 3
        // --------------------------------------------------------------------

        if (verbose) printf("\n"
                            "USAGE EXAMPLE 3\n"
                            "===============\n");

        TestContainerConstructor();
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE 2
        //   This case verifies that the usage example works as advertised.
        //
        // Concerns:
        //   That the usage example compiles, links, and runs as expected.
        //
        // Plan:
        //   Replicate the usage example from the component above, and invoke
        //   the test function here.
        //
        // Testing:
        //   USAGE EXAMPLE 2
        // --------------------------------------------------------------------

        if (verbose) printf("\n"
                            "USAGE EXAMPLE 2\n"
                            "===============\n");

        TestSmartCast();
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE 1
        //   This case verifies that the usage example works as advertised.
        //
        // Concerns:
        //   That the usage example compiles, links, and runs as expected.
        //
        // Plan:
        //   Replicate the usage example from the component above, and invoke
        //   the test function here.
        //
        // Testing:
        //   USAGE EXAMPLE 1
        // --------------------------------------------------------------------

        if (verbose) printf("\n"
                            "USAGE EXAMPLE 1\n"
                            "===============\n");

        TestSwap();
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // TESTING BSLMF_ENABLEIF
        //   Test the 'bslmf_EnableIf' meta-function.
        //
        // Concerns:
        //:  1 If the supplied boolean template parameter is true, then
        //:    'bslmf_EnableIf' provides a type named 'type' matching the
        //:     second template parameter.
        //:
        //:  2 If the supplied boolean template parameter is 'false', then
        //:    'bslmf_EnableIf' does not provide a type named 'type'.
        // Plan:
        //:  1 For a series of possible types, instantiate 'bslmf_EnableIf'
        //:    with a first parameter 'true' and the second parameter of the
        //:    test type.  Verify that 'bslmf_EnableIf' defines a type 'type'
        //:    matching the second template parameter type. (C-1)
        //:
        //:  2 Create a two instances of a template function parameterized on
        //:    a boolean, one returning 1, the other returning 2.  If 'true'
        //:    is supplied as the template parameter, only the first function
        //:    is part of the overload set, if 'false' is then only the second
        //:    function is part of the overload set.  Call the template
        //:    function with both 'true' and 'false', verify that there is no
        //:    compilation error, and that the return value is 1 and 2
        //:    respectively (for 'true' and 'false). (C-2)
        //:
        //:  3 Create a two instances of a template function parameterized on
        //:    a boolean, the first simply returns void, and the second returns
        //:    a 'bslmf_EnableIf' supplied the boolean parameter value. (C-2)
        //:
        //:    1 Instantiate this template function for 'false' to verify
        //:      enable-if removes the second implementation from the overload
        //:      set.
        //:
        //:    2 Manually verify that if the template function is instantiated
        //:      for 'true' that enable-if leaves the second implementation in
        //:      the overload set, resulting in a compiler diagnostic (for an
        //:      ambiguous function declaration).
        //
        // Testing:
        //   bslmf_EnableIf<CONDITION, RESULT_TYPE>
        // --------------------------------------------------------------------

        if (verbose) printf("\n"
                            "bslmf_EnableIf\n"
                            "==============\n");

        if (veryVerbose) printf("\nTest the return type\n");

        {
            {
                const bool R =
                    bslmf_IsSame<bool,
                                 bslmf_EnableIf<true, bool>::type >::VALUE;
                ASSERT(R);
            }
            {
                const bool R =
                    bslmf_IsSame<int,
                                 bslmf_EnableIf<true, int>::type >::VALUE;
                ASSERT(R);
            }
            {
                const bool R =
                    bslmf_IsSame<void *,
                                 bslmf_EnableIf<true, void *>::type >::VALUE;
                ASSERT(R);
            }
            {
                const bool R =
                    bslmf_IsSame<const void *,
                           bslmf_EnableIf<true, const void *>::type >::VALUE;
                ASSERT(R);
            }
            {
                const bool R =
                  bslmf_IsSame<const volatile void *,
                    bslmf_EnableIf<true, const volatile void *>::type >::VALUE;
                ASSERT(R);
            }
            {
                const bool R =
                  bslmf_IsSame<DummyClass,
                    bslmf_EnableIf<true, DummyClass>::type >::VALUE;
                ASSERT(R);
            }
            {
                const bool R =
                  bslmf_IsSame<DummyClass&,
                    bslmf_EnableIf<true, DummyClass&>::type >::VALUE;
                ASSERT(R);
            }
        }

        if (veryVerbose) {
            printf("\nTest whether enabelif modifies the overload set\n");
        }
        {

            ASSERT(1 == testMutuallyExclusiveFunction<true>());
            ASSERT(2 == testMutuallyExclusiveFunction<false>());
        }
        if (veryVerbose) {
            printf("\nManually test if enableif modifies the overload set\n");
        }
        {
            testFunction<false>();
            // This should fail to compile if un-commented.
            // testFunction<true>();

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

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2011
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
