// bslmf_integralconstant.t.cpp                                       -*-C++-*-

#include <bslmf_integralconstant.h>

#include <bsls_bsltestutil.h>

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

using namespace BloombergLP;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// The component under test implements a trivial mechanism class template
// containing public compile-time values and types. An instantiation of this
// class template is immutable and, therefore, there are no primary
// manipulators.  There is, however, one accessor that provides an alternate
// interface to the (public static) value of the object.
//
// Basic Accessors:
//: o 'operator TYPE() const'
//
// Although it is an empty class, an instantiation is nevertheless
// default-constructible, copy-constructible and assignable.  The class is not
// value-semantic, however, as the constructors and assignment operator are
// no-ops and there is no equality comparison operator.
//
// Global Concerns:
//: o The test driver is robust w.r.t. reuse in other, similar components.
//: o The template can be instantiated with different integer types.
//: o The template can be instantiated with different integer values,
//:   including min and max values for the instance type.
//: o ACCESSOR methods are declared 'const'.
//
// Global Assumption:
//: o The component under test does not allocate memory from the heap or from
//:   any allocator.
//: o All operations a fully thread safe (because there is no runtime code
//:   executed).
// ----------------------------------------------------------------------------
// VARIABLES:
// [ 3] const TYPE value;
//
// TYPES:
// [ 3] type
// [ 3] value_type
// [ 4] bsl::false_type
// [ 4] bsl::true_type
//
// CREATORS
// [ 3] integral_constant();
// [ 3] integral_constant(const integral_constant&);
// [ 3] ~integral_constant();
//
// MANIPULATORS
// [ 3] integral_constant& operator=(const integral_constant&);
//
// ACCESSORS
// [ 3] operator TYPE() const;
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 5] USAGE EXAMPLE
// [ 3] CONCERN: The template can be instantiated with different integer types.
// [ 3] CONCERN: The template can be instantiated with different integer
//      values, including min and max values for the instance type.
// [ 3] CONCERN: ACCESSOR methods are declared 'const'.
//
// TEST APPARATUS
// [ 2] IsSameType<class A, class B>
// [ 2] bool isConst(T&);
// [ 2] CONCERN: This test driver is reusable w/other, similar components.
//-----------------------------------------------------------------------------

//=============================================================================
//                  STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
// NOTE: THIS IS A LOW-LEVEL COMPONENT AND MAY NOT USE ANY C++ LIBRARY
// FUNCTIONS, INCLUDING IOSTREAMS.

namespace {

int verbose = 0;
int veryVerbose = 0;
int veryVeryVerbose = 0;
int testStatus = 0;

void aSsErT(int c, const char *s, int i) {
    if (c) {
        printf("Error " __FILE__ "(%d): %s    (failed)\n", i, s);
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}

}  // close unnamed namespace

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
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

enum { VERBOSE_ARG_NUM = 2, VERY_VERBOSE_ARG_NUM, VERY_VERY_VERBOSE_ARG_NUM };

struct DummyType
{
    int d_x;
};

//=============================================================================
//                      GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

template <class A, class B>
struct IsSameType
{
    // Provide a member enum 'VALUE' that has a true value if 'A' and 'B' are
    // the same type and a false value otherwise.

    enum { VALUE = false };
};

template <class A>
struct IsSameType<A, A>
{
    // Provide a member enum 'VALUE' that has a true value if 'A' and 'B' are
    // the same type and a false value otherwise.

    enum { VALUE = true };
};

template <class T> inline bool isConst(T&) { return false; }
template <class T> inline bool isConst(const T&) { return true; }
    // Return 'true' if the specified argument is 'const' and false otherwise.

//=============================================================================
//                  CODE FOR TESTING USAGE EXAMPLES
//-----------------------------------------------------------------------------

///Usage
///-----
// This section illustrates intended usage of this component
//
///Example 1: Compile-Time Function Dispatching
/// - - - - - - - - - - - - - - - - - - - - - -
// The most common use of this structure is to perform compile-time function
// dispatching based on a compile-time calculation.  Often the calculation is
// nothing more than a simple predicate, allowing us to select one of two
// functions based on whether the predicate holds.  The following function,
// 'doSomething', uses a fast implementation (e.g., using 'memcpy') if the
// parameterized type allows for such operations, otherwise it will use a more
// generic and slower implementation (e.g., using the copy constructor).  This
// example uses the types 'true_type' and 'false_type', which are simple
// typedefs for 'integral_constant<bool, true>' and
// 'integral_constant<bool, false>', respectively.
//..
      #include <bslmf_integralconstant.h>

      template <class T>
      int doSomethingImp(T *t, bsl::true_type)
      {
          // slow, generic implementation
          // ...
          (void) t;
          return 11;
      }

      template <class T>
      int doSomethingImp(T *t, bsl::false_type)
      {
          // fast implementation that works only for some types of T
          // ...
          (void) t;
          return 55;
      }

      template <bool IsSlow, class T>
      int doSomething(T *t)
      {
          // Dispatch to an implementation depending on the (compile-time)
          // value of 'IsSlow'.
          return doSomethingImp(t, bsl::integral_constant<bool, IsSlow>());
      }
//..
// For some parameter types, the fast version of 'doSomethingImp' is not
// legal.  The power of this approach is that the compiler will not attempt
// semantic analysis on the implementation that does not match the appropriate
// 'integral_constant' argument.
//..
      int usageExample1()
      {
          int r;

          int i;
          r = doSomething<false>(&i);   // select fast version for int
          ASSERT(55 == r);

          double m;
          r = doSomething<true>(&m); // select slow version for double
          ASSERT(11 == r);

          return 0;
      }
//..
///Example 2: Base class for metafunctions
/// - - - - - - - - - - - - - - - - - - -
// Hard-coding the value of an 'integral_constant' is not especially useful.
// Rather, 'integral_constant' is typically used as the base class for
// "metafunction" classes, classes that yield the value of compile-time
// properties, including properties that are associated with types, rather
// than with values.  For example, the following metafunction can be used at
// compile time to determine whether a type is a floating point type:
//..
      template <class TYPE> struct IsFloatingPoint    : bsl::false_type { };
      template <> struct IsFloatingPoint<float>       : bsl::true_type { };
      template <> struct IsFloatingPoint<double>      : bsl::true_type { };
      template <> struct IsFloatingPoint<long double> : bsl::true_type { };
//..
// The value 'IsFloatingPoint<int>::value' is false and
// 'IsFloatingPoint<double>::value' is true.  The 'integral_constant' base
// class has a member type, 'type', that refers to itself and is inherited by
// 'IsFloatingPoint'. Thus 'IsFloatingPoint<float>::type' is 'true_type' and
// 'IsFloatingPoint<char>::type' is 'false_type'.  'IsFloatingPoint' is an a
// member of a common category of metafunctions known as "type traits" because
// they express certain properties (traits) of a type.  Using this
// metafunction, we can rewrite the 'doSomething' function from first example
// so that it does not require the user to specify the 'IsSlow' template
// argument:
//..
      template <class T>
      int doSomething2(T *t)
      {
          // Automatically detect whether to use slow or fast imp.
          const bool isSlow = IsFloatingPoint<T>::value;
          return doSomethingImp(t, bsl::integral_constant<bool, isSlow>());
      }

      int usageExample2()
      {
          int r;

          int i;
          r = doSomething2(&i); // select fast version for int
          ASSERT(55 == r);

          double m;
          r = doSomething2(&m); // select slow version for double
          ASSERT(11 == r);

          return 0;
      }
//..

//=============================================================================
//                   FUNCTIONS FOR INDIVIDUAL TEST CASES
//-----------------------------------------------------------------------------

template <class TYPE, TYPE VAL>
void fullTest(const char TYPENAME[])
    // Perform the full suite of tests on 'integral_constant' instantiated with
    // the specified 'TYPE' and 'VAL'.  The specified 'TYPENAME' string
    // contains a printable representation of the name of 'TYPE'.  This
    // function reads the global 'verbose' variable and (if an error is
    // detected) changes the value of the global 'status' variable.  See the
    // "FULL TEST" case in 'main' for a full description of the concerns and
    // plan for this test.
{
    if (verbose) {
        printf("\nRun fullTest<%s, ", TYPENAME);
        bsls::debugprint(VAL);
        printf(">()\n");
    }

    typedef bsl::integral_constant<TYPE, VAL> Obj;

    if (verbose) printf("\tTest 'value' static member constant\n");
    LOOP2_ASSERT(TYPENAME, VAL, isConst(Obj::value));
    const TYPE* p = &Obj::value; (void) p;  // Test that address can be taken
    LOOP2_ASSERT(TYPENAME, VAL, VAL == Obj::value);

    if (verbose) printf("\tTest 'type' and 'value_type' member types\n");
    LOOP2_ASSERT(TYPENAME, VAL, VAL == Obj::type::value);
    LOOP2_ASSERT(TYPENAME, VAL, (IsSameType<Obj, typename Obj::type>::VALUE));
    LOOP2_ASSERT(TYPENAME, VAL,
                 (IsSameType<TYPE, typename Obj::value_type>::VALUE));

    if (verbose) printf("\tTest default constructor\n");
    Obj x; const Obj& X = x;

    if (verbose) printf("\tTest conversion to 'TYPE'\n");
    TYPE v = X;
    LOOP2_ASSERT(TYPENAME, VAL, VAL == v);

    if (verbose) printf("\tTest copy constructor\n");
    // Result of copy constructor is same as result of default constructor.
    Obj x2(x); const Obj& X2 = x2;
    v = X2;
    LOOP2_ASSERT(TYPENAME, VAL, VAL == v);

    if (verbose) printf("\tTest assignment operator\n");
    // Assignment has no effect
    Obj x3; const Obj& X3 = x3;
    x3 = x;
    v = X3;
    LOOP2_ASSERT(TYPENAME, VAL, VAL == v);
}

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    verbose = argc > 2;
    veryVerbose = argc > 3;
    veryVeryVerbose = argc > 4;

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:  // Zero is always the leading case.
      case 5: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //
        // Concerns:
        //: 1 The usage example provided in the component header file
        //:   compiles, links, and runs as shown.
        //
        // Plan:
        //: 1 Copy the usage examples from the header into this test
        //:   driver.  (C-1)
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) printf("\nTesting USAGE EXAMPLE"
                            "\n=====================\n");

        usageExample1();
        usageExample2();

      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING false_type AND true_type
        //
        // Concerns:
        //: 1 'false_type' is identical to 'integral_constant<bool, false>'.
        //:
        //: 2 'true_type' is identical to 'integral_constant<bool, true>'.
        //
        // Plan:
        //: 1 Use 'IsSameType' to verify the type of 'false_type' and
        //:   'true_type'.  (C-1..2)
        //
        // Testing:
        //   bsl::false_type
        //   bsl::true_type
        // --------------------------------------------------------------------

        if (verbose) printf("\nTesting false_type and true_type"
                            "\n================================\n");

        ASSERT((IsSameType<bsl::false_type,
                           bsl::integral_constant<bool, false> >::VALUE));
        ASSERT((IsSameType<bsl::true_type,
                           bsl::integral_constant<bool, true> >::VALUE));

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // FULL TEST
        //
        // Concerns:
        //: 1 'value' type is a const lvalue of type 'TYPE' with value 'VAL'
        //:
        //: 2 'type' member is the same as 'integral_constant<TYPE, VAL>'
        //:
        //: 3 'value_type' member is the same as 'TYPE'
        //:
        //: 4 An 'integral_constant<TYPE, VAL>' object is convertible to an
        //:   object of type 'TYPE' with a 'value' of 'VAL.
        //:
        //: 5 'integral_constant<TYPE, VAL>' is default constructible.
        //:
        //: 6 'integral_constant<TYPE, VAL>' is copy constructible such that
        //:   the copy constructor has exactly the same behavior as the default
        //:   constructor (i.e., the argument to the copy constructor is
        //:   ignored).
        //:
        //: 7 'integral_constant<TYPE, VAL>' is assignable, though the
        //:   assignment operator has no effect.
        //:
        //: 8 'integral_constant<TYPE, VAL>' is destructible.
        //:
        //: 9 All of the above concerns apply to each built-in integer type.
        //:
        //: 10 All of the above concerns apply to multiple values, including
        //     the minimum and maximum values of of 'TYPE'.
        //
        // Plan:
        //: 1 Assert that 'value' is const and verify that its address can be
        //:   taken and stored in a pointer to 'const TYPE'.  (C-1)
        //:
        //: 2 Use 'IsSameType' verify that the 'type' and 'value_type' are as
        //:   expected. (C-2 and C-3)
        //:
        //: 3 Construct an object of type 'integral_constant<TYPE, VAL>' and
        //:   use it to initialize a variable of type 'TYPE'.  Verify that
        //:   the variable compares equal to 'VAL'. (C-4)
        //:
        //: 4 Default construct, copy construct, and assign objects of type
        //:   'integral_constant<TYPE, VAL>'.  In each case, verify that
        //:   converting the result to 'TYPE' yields 'VAL'.  (C5..7)
        //:
        //: 5 The destructor is tested automatically when the objects created
        //:   in step 4 go out of scope. (C-8)
        //:
        //: 6 Instantiate the entire test in a template function, 'fullTest',
        //:   instantiate 'fullTest' with each C++ integer type and with
        //:   several different values, including the minimum and maximum
        //:   values for the type.  (C-9..10)
        //
        // Testing:
        //   value
        //   type
        //   value_type
        //   operator TYPE() const;
        //   compiler-generated default constructor
        //   compiler-generated copy constructor
        //   compiler-generated assignment operator
        //   compiler-generated destructor
        // --------------------------------------------------------------------

        if (verbose) printf("\nFULL TEST"
                            "\n=========\n");

        fullTest<bool, false>("bool");
        fullTest<bool, true>("bool");

        fullTest<char, 0>("char");
        fullTest<char, 'x'>("char");
        fullTest<char, CHAR_MIN>("char");
        fullTest<char, CHAR_MAX>("char");

        fullTest<unsigned char, 'y'>("unsigned char");
        fullTest<unsigned char, 0>("unsigned char");
        fullTest<unsigned char, 255>("unsigned char");

        fullTest<signed char, 0>("signed char");
        fullTest<signed char, -128>("signed char");
        fullTest<signed char, 127>("signed char");

        fullTest<short, -5>("short");
        fullTest<short, SHRT_MIN>("short");
        fullTest<short, SHRT_MAX>("short");

        fullTest<unsigned short, 6>("unsigned short");
        fullTest<unsigned short, 0>("unsigned short");
        fullTest<unsigned short, USHRT_MAX>("unsigned short");

        fullTest<int, 0>("int");
        fullTest<int, -1>("int");
        fullTest<int, 99>("int");
        fullTest<int, INT_MIN>("int");
        fullTest<int, INT_MAX>("int");

        fullTest<unsigned, 1>("unsigned");
        fullTest<unsigned, 99>("unsigned");
        fullTest<unsigned, 0>("unsigned");
        fullTest<unsigned, UINT_MAX>("unsigned");

        fullTest<long, -7>("long");
        fullTest<long, LONG_MIN>("long");
        fullTest<long, LONG_MAX>("long");

        fullTest<unsigned long, 8>("unsigned long");
        fullTest<unsigned long, 0>("unsigned long");
        fullTest<unsigned long, ULONG_MAX>("unsigned long");

        // Not all implementations define LLONG_MIN/MAX, so use hard-coded
        // 64-bit hex values instead.
        fullTest<long long, 0>("long long");
        fullTest<long long, 0x1000000000000000LL>("long long");
        fullTest<long long, 0x7fffffffffffffffLL>("long long");

        // Not all implementations define ULLONG_MAX, so use hard-coded
        // 64-bit hex value instead.
        fullTest<unsigned long long, 10>("unsigned long long");
        fullTest<unsigned long long, 0>("unsigned long long");
        fullTest<unsigned long long, 0xffffffffffffffffULL>(
            "unsigned long long");

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING TEST APPARATUS
        //
        // Concerns:
        //: 1 When called with values of the same type, 'IsSameType' returns
        //:   true
        //:
        //: 2 When called with values of different type, 'IsSameType' returns
        //:   false
        //:
        //: 3 When called with values of the same type but where
        //:   one is a reference, 'IsSameType' returns false.
        //:
        //: 4 When called with values of the same type but with
        //:   different cv-qualifiers, 'IsSameType' returns false.
        //:
        //: 5 Arguments to 'IsSameType' can be class type or integral type.
        //:
        //: 6 For a variable 'x', 'isConst(x)' returns true if 'x' is 'const'
        //:   and false otherwise.
        //
        // Plan:
        //: 1 Instantiate 'IsSameType' with every combination of types 'int',
        //:   'char', 'DummyType'; with every combination of 'int', 'const
        //:   int', 'volatile int', and with every combination of 'int'
        //:   'int&' and 'const int&'.  Verify that its 'VALUE' member is
        //:   true only when the types are identically the same.  (C-1..5)
        //:
        //: 2 Call 'isConst' for both const and non-const variables and
        //:   verify the result.  (C-6)
        //
        // Testing:
        //   IsSameType<class A, class B>
        //   bool isConst(T&);
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING TEST APPARATUS"
                            "\n======================\n");

        if (verbose) printf("Testing IsSameType<A, B>\n");
        ASSERT((  IsSameType<int,       int>::VALUE));
        ASSERT((! IsSameType<int,       char>::VALUE));
        ASSERT((! IsSameType<int,       DummyType>::VALUE));
        ASSERT((! IsSameType<char,      int>::VALUE));
        ASSERT((  IsSameType<char,      char>::VALUE));
        ASSERT((! IsSameType<char,      DummyType>::VALUE));
        ASSERT((! IsSameType<DummyType, int>::VALUE));
        ASSERT((! IsSameType<DummyType, char>::VALUE));
        ASSERT((  IsSameType<DummyType, DummyType>::VALUE));

        ASSERT((  IsSameType<               int,                int>::VALUE));
        ASSERT((! IsSameType<               int, const          int>::VALUE));
        ASSERT((! IsSameType<               int,       volatile int>::VALUE));
        ASSERT((! IsSameType<               int, const volatile int>::VALUE));
        ASSERT((! IsSameType<const          int,                int>::VALUE));
        ASSERT((  IsSameType<const          int, const          int>::VALUE));
        ASSERT((! IsSameType<const          int,       volatile int>::VALUE));
        ASSERT((! IsSameType<const          int, const volatile int>::VALUE));
        ASSERT((! IsSameType<      volatile int,                int>::VALUE));
        ASSERT((! IsSameType<      volatile int, const          int>::VALUE));
        ASSERT((  IsSameType<      volatile int,       volatile int>::VALUE));
        ASSERT((! IsSameType<      volatile int, const volatile int>::VALUE));
        ASSERT((! IsSameType<const volatile int,                int>::VALUE));
        ASSERT((! IsSameType<const volatile int, const          int>::VALUE));
        ASSERT((! IsSameType<const volatile int,       volatile int>::VALUE));
        ASSERT((  IsSameType<const volatile int, const volatile int>::VALUE));

        ASSERT((  IsSameType<      int ,       int >::VALUE));
        ASSERT((! IsSameType<      int ,       int&>::VALUE));
        ASSERT((! IsSameType<      int , const int&>::VALUE));
        ASSERT((! IsSameType<      int&,       int >::VALUE));
        ASSERT((  IsSameType<      int&,       int&>::VALUE));
        ASSERT((! IsSameType<      int&, const int&>::VALUE));
        ASSERT((! IsSameType<const int&,       int >::VALUE));
        ASSERT((! IsSameType<const int&,       int&>::VALUE));
        ASSERT((  IsSameType<const int&, const int&>::VALUE));

        if (verbose) printf("Testing isConst(T&)\n");
        int                i;
        const int          ci = 0;
        volatile int       vi;
        const volatile int cvi = 0;

        DummyType d;
        const DummyType& D = d;

        ASSERT(! isConst(i));
        ASSERT(  isConst(ci));
        ASSERT(! isConst(vi));
        ASSERT(  isConst(cvi));

        ASSERT(! isConst(d));
        ASSERT(  isConst(D));

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //
        // Concerns:
        //: 1 The basic functionality of 'integral_constant' works as
        //:   expected.
        //
        // Plan:
        //: 1 Instantiate 'integral_constant' with several combinations of
        //:   types and values.  For each combination test that the (static)
        //:   'value' member has the expected value, that the 'type' member
        //:   exists and has the same 'value', that the 'value_type' member
        //:   exists and can hold 'value', and that an object of
        //:   'integral_constant' can be constructed and is convertible to the
        //:   specified 'TYPE'.  (C-1)
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) printf("\nBREATHING TEST"
                            "\n==============\n");

        using namespace bsl;

        if (verbose) printf("... With 'bool' type\n");
        {
            typedef integral_constant<bool, false> TypeFalse;
            typedef integral_constant<bool, true>  TypeTrue;

            ASSERT(false == TypeFalse::value);
            ASSERT(true  == TypeTrue::value);

            ASSERT(false == TypeFalse::type::value);
            ASSERT(true  == TypeTrue::type::value);

            TypeFalse icFalse; const TypeFalse& ICFALSE = icFalse;
            TypeFalse::value_type vFalse = ICFALSE;
            ASSERT(false == vFalse);

            TypeTrue icTrue; const TypeTrue& ICTRUE = icTrue;
            bool vTrue = ICTRUE;
            ASSERT(true == vTrue);
        }

        if (verbose) printf("... With 'int' type\n");
        {
            typedef integral_constant<int, 0>  Type0;
            typedef integral_constant<int, 99> Type99;

            ASSERT(0  == Type0::value);
            ASSERT(99 == Type99::value);

            ASSERT(0  == Type0::type::value);
            ASSERT(99 == Type99::type::value);

            Type0 ic0; const Type0& IC0 = ic0;
            Type0::value_type v0 = IC0;
            ASSERT(0 == v0);

            Type99 ic99; const Type99& IC99 = ic99;
            int v99 = IC99;
            ASSERT(99 == v99);
        }

        if (verbose) printf("... With 'unsigned char' type\n");
        {
            typedef integral_constant<unsigned char, 0>  Type0;
            typedef integral_constant<unsigned char, 99> Type99;

            ASSERT(0  == Type0::value);
            ASSERT(99 == Type99::value);

            ASSERT(0  == Type0::type::value);
            ASSERT(99 == Type99::type::value);

            Type0 ic0; const Type0& IC0 = ic0;
            Type0::value_type v0 = IC0;
            ASSERT(0 == v0);

            Type99 ic99; const Type99& IC99 = ic99;
            unsigned char v99 = IC99;
            ASSERT(99 == v99);
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
