// bslmf_isconvertible.t.cpp                                          -*-C++-*-
#include <bslmf_isconvertible.h>

#include <bsls_bsltestutil.h>
#include <bsls_platform.h>

#include <stdio.h>      // 'printf'
#include <stdlib.h>     // 'atoi'
#include <string.h>     // 'strcmp'

using namespace BloombergLP;

//=============================================================================
//                                TEST PLAN
//-----------------------------------------------------------------------------
//                                Overview
//                                --------
// The component under test defines two meta-functions, 'bsl::is_convertible'
// and 'bslmf::IsConvertible', that determine whether a conversion exists from
// one template parameter type to the other template parameter type.  Thus, we
// need to ensure that the values returned by these meta-functions are correct
// for each possible pair of categorized types.  The two meta-functions are
// functionally equivalent except 'bsl::is_convertible' only allows complete
// template parameter types.  We will use the same set of complete types for
// 'bslmf::IsConvertible' as that for 'bsl::is_convertible', and an additional
// set of incomplete types for testing 'bslmf::IsConvertible' alone.
//
//-----------------------------------------------------------------------------
// PUBLIC CLASS DATA
// [ 1] bsl::is_convertible::value
// [ 2] bslmf::IsConvertible::VALUE
//-----------------------------------------------------------------------------
// [ 5] USAGE EXAMPLE
// [ 3] CONCERN: Warning-free on implicit conversions
// [ 4] CONCERN: Warning-free on user-defined conversions

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

// ============================================================================
//               ADDITIONAL MACROS FOR THIS TEST DRIVER
// ----------------------------------------------------------------------------

#if defined(BSLS_PLATFORM_CMP_SUN)                                            \
 || defined(BSLS_PLATFORM_CMP_IBM)                                            \
 ||(defined(BSLS_PLATFORM_CMP_GNU)  && BSLS_PLATFORM_CMP_VERSION < 40300)     \
 ||(defined(BSLS_PLATFORM_CMP_MSVC) && BSLS_PLATFORM_CMP_VERSION < 1700)

# define BSLMF_ISCONVERTIBLE_NO_ARRAY_REF_OF_UNKNOWN_BOUND
    // This macro signifies that this compiler rejects 'Type[]' as incomplete,
    // even in contexts where it should be valid, such as where it will pass by
    // reference or pointer.
#endif

#if defined(BSLS_PLATFORM_CMP_IBM)                                            \
 ||(defined(BSLS_PLATFORM_CMP_MSVC) && BSLS_PLATFORM_CMP_VERSION < 1600)
# define BSLMF_ISCONVERTIBLE_NO_ARRAY_OF_UNKNOWN_BOUND_AS_TEMPLATE_PARAMETER
    // The IBM compiler has a bigger problem, where it rejects arrays of
    // unknown bound as template type-parameter arguments.  Older Microsoft
    // compilers (prior to MSVC 2010) deduce references to such types when
    // trying to instantiate the 'is_convertible' trait for arrays of unknown
    // bound, so we drop tests on such platforms as well.
#endif

#ifdef BSLMF_ISCONVERTIBLE_USE_NATIVE_TRAITS
    // The 'NOT_NATIVE' macro is used to track tests that report different
    // results when computed using the portable C++98 algorithm vs. a built-in
    // type trait.  The C++11 version can give correct the result when trying
    // to convert a non-copyable type to itself, and when trying to convert to
    // an abstract base class.  The former would result in a non-SFINAEable
    // error in C++03 if we wrote the trait more carefully, so we accept an
    // always-compiling wrong answer as this is consistent with the result that
    // the BDE trait has always given.
# define NOT_NATIVE false
#else
# define NOT_NATIVE true
#endif

//# define BSLMF_ISCONVERTIBLE_TEST_COMPILE_ERROR_WITH_INCOMPLETE_TYPES
    // Uncomment this macro definition to test the correct diagnosis of errors
    // when 'is_convertible' is instantiated with incomplete class types.

#define ASSERT_IS_CONVERTIBLE(RESULT, FROM_TYPE, TO_TYPE) \
        ASSERT(RESULT  == (bsl::is_convertible<FROM_TYPE, TO_TYPE>::value))

#define  _(TYPE) TYPE
#define C_(TYPE) const TYPE
#define V_(TYPE) volatile TYPE
#define CV(TYPE) const volatile TYPE
        
//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

template <class TYPE>
class ConvertibleToObj {
    // This class is convertible to 'TYPE'.

    // DATA
    TYPE d_value;

  public:
    // CREATORS
    ConvertibleToObj();  // declared but not defined

    operator TYPE() const { return d_value; }
};

template <class TYPE>
class ConvertibleToRef {
    // This class is convertible to a reference to 'TYPE'.

    // DATA
    TYPE d_value;

  public:
    // CREATORS
    ConvertibleToRef();  // declared but not defined

    operator TYPE&() { return d_value; }
};

template <class TYPE>
class ConvertibleToConstRef {
    // This class is convertible to a const reference to 'TYPE'.

    // DATA
    TYPE d_value;

  public:
    // CREATORS
    ConvertibleToConstRef();  // declared but not defined

    operator TYPE const&() const { return d_value; }
};

template <class TYPE>
class ConvertibleFrom {
    // This class is convertible from 'TYPE'.

    // DATA
    TYPE d_value;

  public:
    // CREATORS
    ConvertibleFrom() : d_value() { }
    ConvertibleFrom(TYPE value) : d_value(value) { }                // IMPLICIT
};

class my_Class {
    // This class is CopyConstructible, not DefaultConstructible, and
    // convertible from 'int'.

    // DATA
    int d_i;

  public:
    // CREATORS
    my_Class(int i) : d_i(i) {}                                     // IMPLICIT
};

class my_OtherClass {
    // This trivial empty class is convertible to 'my_Class'.

  public:
    operator my_Class&();
};

class my_ThirdClass {
    // This trivial empty class is convertible from 'my_Class'.

  public:
    my_ThirdClass(const my_Class&);                                 // IMPLICIT
};

class my_AbstractClass {
    // This is an abstract class.

  public:
    // CREATORS
    my_AbstractClass() { }
    virtual ~my_AbstractClass() = 0;

    // MANIPULATORS
    int func1(int);
};

class my_DerivedClass : public my_AbstractClass {
    // This is a derived class.

  public:
    // CREATORS
    my_DerivedClass() { }
    ~my_DerivedClass();

    // MANIPULATORS
    int func2(int);
};

class my_NotCopyableClass {
    // Objects of this class type cannot be copied.

  private:
    // NOT DEFINED
    my_NotCopyableClass(const my_NotCopyableClass&); // = delete
    my_NotCopyableClass& operator=(const my_NotCopyableClass&); // = delete

  public:
    // CREATORS
    my_NotCopyableClass() { }
};

class my_IncompleteClass;   // incomplete class
class my_IncompleteClass2;  // incomplete class

enum my_Enum { MY_VAL0, MY_VAL1 };

class my_EnumClass {
    // This class defines a nested 'enum' type.
  public:
    enum Type { VAL0, VAL1 };
};

class my_BslmaAllocator;

template <class TYPE>
class my_StlAllocator {
    // This class is convertible from 'my_BslmaAllocator*'.

  public:
    // CREATORS
    my_StlAllocator(my_BslmaAllocator*);                            // IMPLICIT
};

struct my_PlacementNew {
    // This class is convertible from 'void*'.

    // DATA
    void *d_p;

    // CREATORS
    my_PlacementNew(void *p) : d_p(p) {}                            // IMPLICIT
};

void *operator new(size_t, my_PlacementNew p)
{
    return p.d_p;
}

// Verify that the 'bsl::is_convertible::value' is evaluated at compile-time.

static char C00[1 + bsl::is_convertible<int,   int  >::value];     // sz=2
static char C01[1 + bsl::is_convertible<int,   char >::value];     // sz=2
static char C02[1 + bsl::is_convertible<void*, int  >::value];     // sz=1
static char C03[1 + bsl::is_convertible<int,   int *>::value];     // sz=1

// Verify that the 'bslmf::IsConvertible::VALUE' is evaluated at compile-time.

static char C10[1 + bslmf::IsConvertible<int,   int  >::VALUE];    // sz=2
static char C11[1 + bslmf::IsConvertible<int,   char >::VALUE];    // sz=2
static char C12[1 + bslmf::IsConvertible<void*, int  >::VALUE];    // sz=1
static char C13[1 + bslmf::IsConvertible<int,   int *>::VALUE];    // sz=1

//=============================================================================
//                              USAGE EXAMPLES
//-----------------------------------------------------------------------------

// BDE_VERIFY pragma : push
// BDE_VERIFY pragma : -FD01   // Function contracts are descriptive text

///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: Select Function Based on Type Convertibility
///- - - - - - - - - - - - - - - - - - - - - - - - - - - -
// The 'bsl::is_convertible' meta-function can be used to select an appropriate
// function (at compile time) based on the convertibility of one type to
// another without causing a compiler error by actually trying the conversion.
//
// Suppose we are implementing a 'convertToInt' template method that converts a
// given object of the (template parameter) 'TYPE' to 'int' type, and returns
// the integer value.  If the given object can not convert to 'int', return 0.
// The method calls an overloaded function, 'getIntValue', to get the converted
// integer value.  The idea is to invoke one version of 'getIntValue' if the
// type provides a conversion operator that returns an integer value, and
// another version if the type does not provide such an operator.
//
// First, we define two classes, 'Foo' and 'Bar'.  The 'Foo' class has a
// conversion operator that returns an integer value while the 'Bar' class does
// not:
//..
class Foo {
    // DATA
    int d_value;

  public:
    // CREATORS
    explicit Foo(int value) : d_value(value) {}

    // ACCESSORS
    operator int() const { return d_value; }
};

class Bar {};
//..
// Then, we define the first 'getIntValue' function that takes a
// 'bsl::false_type' as its last argument, whereas the second 'getIntValue'
// function takes a 'bsl::true_type' object.  The result of the
// 'bsl::is_convertible' meta-function (i.e., its 'type' member) is used to
// create the last argument passed to 'getIntValue'.  Neither version of
// 'getIntValue' makes use of this argument -- it is used only to differentiate
// the argument list so we can overload the function.
//..
template <class TYPE>
inline
int getIntValue(TYPE * /* object */, bsl::false_type)
{
    // Return 0 because the specified 'object' of the (template parameter)
    // 'TYPE' is not convertible to the 'int' type.

    return 0;
}

template <class TYPE>
inline
int getIntValue(TYPE *object, bsl::true_type)
{
    // Return the integer value converted from the specified 'object' of the
    // (template parameter) 'TYPE'.

    return int(*object);
}
//..
// Now, we define our 'convertToInt' method:
//..
template <class TYPE>
inline
int convertToInt(TYPE *object)
{
    typedef typename bsl::is_convertible<TYPE, int>::type CanConvertToInt;
    return getIntValue(object, CanConvertToInt());
}
//..
// Notice that we use 'bsl::is_convertible' to get a 'bsl::false_type' or
// 'bsl::true_type', and then call the corresponding overloaded 'getIntValue'
// method.

// BDE_VERIFY pragma : pop

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

    (void) veryVerbose;          // eliminate unused variable warning
    (void) veryVeryVerbose;      // eliminate unused variable warning
    (void) veryVeryVeryVerbose;  // eliminate unused variable warning

    // Silence compiler warnings about unused static variables
    (void) C00[0];
    (void) C01[0];
    (void) C02[0];
    (void) C03[0];

    (void) C10[0];
    (void) C11[0];
    (void) C12[0];
    (void) C13[0];

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:  // Zero is always the leading case.
      case 5: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
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
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) printf("\nUSAGE EXAMPLE\n"
                              "=============\n");

//
// Finally, we call 'convertToInt' with both 'Foo' and 'Bar' classes:
//..
    Foo foo(99);
    Bar bar;

    printf("%d\n", convertToInt(&foo));
    printf("%d\n", convertToInt(&bar));
//..

      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING CONCERN: WARNING-FREE ON USER-DEFINED CONVERSIONS
        //
        // Concerns:
        //: 1 GCC should not generate a warning for integral to floating-point
        //:   type conversions via a user-defined class.
        //
        // Plan:
        //   Instantiate 'bsl::is_convertible' with combinations of a
        //   user-defined class that is convertible to a fundamental type,
        //   'T1', and another fundamental type, 'T2', to which 'T1' is
        //   implicitly convertible.  Verify that the 'value' member is
        //   initialized properly, and (manually) verify that no warning is
        //   generated for conversions between floating-point types and
        //   integral types.  For each combination of 'T1', use three different
        //   user-defined classes: one that provides conversion to an object of
        //   type 'T1', one that provides conversion to a reference to 'T1' and
        //   one that provides conversion to a const reference to 'T1'.  (C-1)
        //
        // Testing:
        //   CONCERN: Warning-free on user-defined conversions
        // --------------------------------------------------------------------

        if (verbose)
            printf(
                "TESTING CONCERN: WARNING-FREE ON USER-DEFINED CONVERSIONS\n"
                "=========================================================\n");

        // Providing conversions from 'T' to 'T':

        // Conversion of basic types via two user-defined classes returns
        // 'false'.

        ASSERT(false == (bsl::is_convertible<ConvertibleFrom<int>,
                                              ConvertibleToObj<int> >::value));

        // Test conversion of basic types via a user-defined class.

        ASSERT(true ==
                   (bsl::is_convertible<ConvertibleToObj<int>, float>::value));
        ASSERT(true ==
                   (bsl::is_convertible<ConvertibleToObj<float>, int>::value));
        ASSERT(true ==
                   (bsl::is_convertible<int, ConvertibleFrom<float> >::value));
        ASSERT(true ==
                   (bsl::is_convertible<float, ConvertibleFrom<int> >::value));

        // Test 'const' type conversions via a user-defined class.

        ASSERT(true ==
             (bsl::is_convertible<ConvertibleToObj<int>, const float>::value));
        ASSERT(true ==
             (bsl::is_convertible<ConvertibleToObj<float>, const int>::value));
        ASSERT(true ==
             (bsl::is_convertible<ConvertibleToObj<const float>, int>::value));
        ASSERT(true ==
             (bsl::is_convertible<ConvertibleToObj<const int>, float>::value));
        ASSERT(true ==
             (bsl::is_convertible<int, ConvertibleFrom<const float> >::value));
        ASSERT(true ==
             (bsl::is_convertible<float, ConvertibleFrom<const int> >::value));
        ASSERT(true ==
             (bsl::is_convertible<const float, ConvertibleFrom<int> >::value));
        ASSERT(true ==
             (bsl::is_convertible<const int, ConvertibleFrom<float> >::value));

        // Test 'volatile' type conversions via a user-defined class.

        ASSERT(true ==
          (bsl::is_convertible<ConvertibleToObj<int>, volatile float>::value));
        ASSERT(true ==
          (bsl::is_convertible<ConvertibleToObj<float>, V_(int)>::value));
        ASSERT(true ==
          (bsl::is_convertible<ConvertibleToObj<V_(int)>, float>::value));
        ASSERT(true ==
          (bsl::is_convertible<ConvertibleToObj<volatile float>, int>::value));
        ASSERT(true ==
          (bsl::is_convertible<int, ConvertibleFrom<volatile float> >::value));
        ASSERT(true ==
          (bsl::is_convertible<float, ConvertibleFrom<V_(int)> >::value));
        ASSERT(true ==
          (bsl::is_convertible<V_(int), ConvertibleFrom<float> >::value));
        ASSERT(true ==
          (bsl::is_convertible<volatile float, ConvertibleFrom<int> >::value));

        // Providing conversions from 'T' to 'T&':

        // // Conversion of basic types via two user-defined classes returns
        // 'false'.

        ASSERT(false == (bsl::is_convertible<ConvertibleFrom<int>,
                                              ConvertibleToRef<int> >::value));

        // Test conversion of basic types via a user-defined class.

        ASSERT(true ==
                   (bsl::is_convertible<ConvertibleToRef<int>, float>::value));
        ASSERT(true ==
                   (bsl::is_convertible<ConvertibleToRef<float>, int>::value));
        ASSERT(true ==
                   (bsl::is_convertible<int, ConvertibleFrom<float> >::value));
        ASSERT(true ==
                   (bsl::is_convertible<float, ConvertibleFrom<int> >::value));

        // Test 'const' type conversions via a user-defined class.

        ASSERT(true ==
             (bsl::is_convertible<ConvertibleToRef<int>, const float>::value));
        ASSERT(true ==
             (bsl::is_convertible<ConvertibleToRef<float>, const int>::value));
        ASSERT(true ==
             (bsl::is_convertible<ConvertibleToRef<const float>, int>::value));
        ASSERT(true ==
             (bsl::is_convertible<ConvertibleToRef<const int>, float>::value));
        ASSERT(true ==
             (bsl::is_convertible<int, ConvertibleFrom<const float> >::value));
        ASSERT(true ==
             (bsl::is_convertible<float, ConvertibleFrom<const int> >::value));
        ASSERT(true ==
             (bsl::is_convertible<const float, ConvertibleFrom<int> >::value));
        ASSERT(true ==
             (bsl::is_convertible<const int, ConvertibleFrom<float> >::value));

        // Test 'volatile' type conversions via a user-defined class.

        ASSERT(true ==
          (bsl::is_convertible<ConvertibleToRef<int>, volatile float>::value));
        ASSERT(true ==
          (bsl::is_convertible<ConvertibleToRef<float>, V_(int)>::value));
        ASSERT(true ==
          (bsl::is_convertible<ConvertibleToRef<V_(int)>, float>::value));
        ASSERT(true ==
          (bsl::is_convertible<ConvertibleToRef<volatile float>, int>::value));
        ASSERT(true ==
          (bsl::is_convertible<int, ConvertibleFrom<volatile float> >::value));
        ASSERT(true ==
          (bsl::is_convertible<float, ConvertibleFrom<V_(int)> >::value));
        ASSERT(true ==
          (bsl::is_convertible<V_(int), ConvertibleFrom<float> >::value));
        ASSERT(true ==
          (bsl::is_convertible<volatile float, ConvertibleFrom<int> >::value));

        // Providing conversions from 'T' to 'const T&':

        // Conversion of basic types via two user-defined classes returns
        // 'false'.

        ASSERT(false == (bsl::is_convertible<ConvertibleFrom<int>,
                                         ConvertibleToConstRef<int> >::value));

        // Test conversion of basic types via a user-defined class.

        ASSERT(true ==
              (bsl::is_convertible<ConvertibleToConstRef<int>, float>::value));
        ASSERT(true ==
              (bsl::is_convertible<ConvertibleToConstRef<float>, int>::value));
        ASSERT(true ==
                   (bsl::is_convertible<int, ConvertibleFrom<float> >::value));
        ASSERT(true ==
                   (bsl::is_convertible<float, ConvertibleFrom<int> >::value));

        // Test 'const' type conversions via a user-defined class.

        ASSERT(true == (bsl::is_convertible<ConvertibleToConstRef<int>,
                                                         const float>::value));
        ASSERT(true == (bsl::is_convertible<ConvertibleToConstRef<float>,
                                                           const int>::value));
        ASSERT(true == (bsl::is_convertible<ConvertibleToConstRef<const float>,
                                                                 int>::value));
        ASSERT(true == (bsl::is_convertible<ConvertibleToConstRef<const int>,
                                                               float>::value));
        ASSERT(true ==
             (bsl::is_convertible<int, ConvertibleFrom<const float> >::value));
        ASSERT(true ==
             (bsl::is_convertible<float, ConvertibleFrom<const int> >::value));
        ASSERT(true ==
             (bsl::is_convertible<const float, ConvertibleFrom<int> >::value));
        ASSERT(true ==
             (bsl::is_convertible<const int, ConvertibleFrom<float> >::value));

        // Test 'volatile' type conversions via a user-defined class.

        ASSERT(true == (bsl::is_convertible<ConvertibleToConstRef<int>,
                                                      volatile float>::value));
        ASSERT(true == (bsl::is_convertible<ConvertibleToConstRef<float>,
                                                        V_(int)>::value));
        ASSERT(true ==
                      (bsl::is_convertible<ConvertibleToConstRef<V_(int)>,
                                                               float>::value));
        ASSERT(true ==
                    (bsl::is_convertible<ConvertibleToConstRef<volatile float>,
                                                                 int>::value));
        ASSERT(true ==
          (bsl::is_convertible<int, ConvertibleFrom<volatile float> >::value));
        ASSERT(true ==
          (bsl::is_convertible<float, ConvertibleFrom<V_(int)> >::value));
        ASSERT(true ==
          (bsl::is_convertible<V_(int), ConvertibleFrom<float> >::value));
        ASSERT(true ==
          (bsl::is_convertible<volatile float, ConvertibleFrom<int> >::value));
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING CONCERN: WARNING-FREE ON IMPLICIT CONVERSIONS
        //
        // Concerns:
        //: 1  GCC should not generate a warning for implicit integral to
        //:    floating-point type conversions.
        //
        // Plan:
        //: 1 Instantiate 'bsl::is_convertible' with various fundamental type
        //:   combinations and verify that the 'value' member is initialized
        //:   properly, and (manually) verify no warning is generated for
        //:   conversions between floating-point types and integral types.
        //:   (C-1)
        //
        // Testing:
        //   CONCERN: Warning-free on implicit conversions
        // --------------------------------------------------------------------

        if (verbose)
            printf("TESTING CONCERN: WARNING-FREE ON IMPLICIT CONVERSIONS\n"
                   "=====================================================\n");

        // Test conversion of basic types.

        ASSERT(true == (bsl::is_convertible<int, float>::value));
        ASSERT(true == (bsl::is_convertible<float, int>::value));

        // Test 'const' type conversions.

        ASSERT(true == (bsl::is_convertible<int, const float>::value));
        ASSERT(true == (bsl::is_convertible<float, const int>::value));
        ASSERT(true == (bsl::is_convertible<const float, int>::value));
        ASSERT(true == (bsl::is_convertible<const int, float>::value));

        // Test 'volatile' type conversions.

        ASSERT(true == (bsl::is_convertible<int, volatile float>::value));
        ASSERT(true == (bsl::is_convertible<float, V_(int)>::value));
        ASSERT(true == (bsl::is_convertible<V_(int), float>::value));
        ASSERT(true == (bsl::is_convertible<volatile float, int>::value));

        // Test 'volatile' pointer and reference conversions from integral to
        // floating-point.

        ASSERT(false == (bsl::is_convertible<int*, volatile float*>::value));
        ASSERT(false == (bsl::is_convertible<V_(int)*, float*>::value));
        ASSERT(false == (bsl::is_convertible<int&, volatile float&>::value));
        ASSERT(false == (bsl::is_convertible<V_(int)&, float&>::value));
        ASSERT(false == (bsl::is_convertible<int,  volatile float&>::value));
        ASSERT(false == (bsl::is_convertible<V_(int),  float&>::value));
        ASSERT(true  == (bsl::is_convertible<int&,  volatile float>::value));
        ASSERT(true  == (bsl::is_convertible<V_(int)&,  float>::value));

        ASSERT(false ==
                    (bsl::is_convertible<int*, const volatile float*>::value));
        ASSERT(false ==
                    (bsl::is_convertible<CV(int)*, float*>::value));
#if !defined(BSLMF_ODD_COMPILER_CONST_OR_VOLATILE_CONVERSION_RULES)
        ASSERT(false ==
                    (bsl::is_convertible<int&, const volatile float&>::value));
#endif
        ASSERT(false ==
                    (bsl::is_convertible<CV(int)&, float&>::value));
#if !defined(BSLMF_ODD_COMPILER_CONST_OR_VOLATILE_CONVERSION_RULES)
        ASSERT(false ==
                    (bsl::is_convertible<int,  const volatile float&>::value));
#endif
        ASSERT(false ==
                     (bsl::is_convertible<CV(int), float&>::value));
        ASSERT(true  ==
                     (bsl::is_convertible<int&, const volatile float>::value));
        ASSERT(true  ==
                     (bsl::is_convertible<CV(int)&, float>::value));

        ASSERT(false ==
                    (bsl::is_convertible<const int*, volatile float*>::value));
        ASSERT(false ==
                    (bsl::is_convertible<V_(int)*, const float*>::value));
        ASSERT(true  ==
                    (bsl::is_convertible<const int&, volatile float>::value));
        ASSERT(true  ==
                    (bsl::is_convertible<V_(int)&, const float>::value));
#if !defined(BSLMF_ODD_COMPILER_CONST_OR_VOLATILE_CONVERSION_RULES)
        ASSERT(false ==
           (bsl::is_convertible<   const int&, const volatile float&>::value));
        ASSERT(false ==
           (bsl::is_convertible<V_(int)&, const volatile float&>::value));
        ASSERT(false ==
           (bsl::is_convertible<   const int,  const volatile float&>::value));
        ASSERT(false ==
           (bsl::is_convertible<V_(int),  const volatile float&>::value));
#endif
        ASSERT(false ==
            (bsl::is_convertible<C_(int), volatile float&>::value));
        ASSERT(false ==
            (bsl::is_convertible<CV(int), volatile float&>::value));

        // Sun 5.2 and 5.5 get this right if the cv-unqualified types differ.

        ASSERT(true ==
               (bsl::is_convertible<      V_(int), const float&>::value));
        ASSERT(true ==
               (bsl::is_convertible<CV(int), const float&>::value));

        ASSERT(false == (bsl::is_convertible<int*, float>::value));
        ASSERT(false == (bsl::is_convertible<int, float*>::value));

        // Test volatile pointer and reference conversions from floating-point
        // to integral.
        ASSERT(false == (bsl::is_convertible<float*, V_(int)*>::value));
        ASSERT(false == (bsl::is_convertible<volatile float*, int*>::value));
        ASSERT(false == (bsl::is_convertible<float&, V_(int)&>::value));
        ASSERT(false == (bsl::is_convertible<volatile float&, int&>::value));
        ASSERT(false == (bsl::is_convertible<float,  V_(int)&>::value));
        ASSERT(false == (bsl::is_convertible<volatile float,  int&>::value));
        ASSERT(true  == (bsl::is_convertible<float&,  V_(int)>::value));
        ASSERT(true  == (bsl::is_convertible<volatile float&,  int>::value));

        ASSERT(false ==
                    (bsl::is_convertible<float*, CV(int)*>::value));
        ASSERT(false ==
                    (bsl::is_convertible<const volatile float*, int*>::value));
#if !defined(BSLMF_ODD_COMPILER_CONST_OR_VOLATILE_CONVERSION_RULES)
        ASSERT(false ==
                    (bsl::is_convertible<float&, CV(int)&>::value));
#endif
        ASSERT(false ==
                    (bsl::is_convertible<const volatile float&, int&>::value));
#if !defined(BSLMF_ODD_COMPILER_CONST_OR_VOLATILE_CONVERSION_RULES)
        ASSERT(false ==
                    (bsl::is_convertible<float,  CV(int)&>::value));
#endif
        ASSERT(false ==
                    (bsl::is_convertible<const volatile float,  int&>::value));
        ASSERT(true  ==
                    (bsl::is_convertible<float&, CV(int) >::value));
        ASSERT(true  ==
                    (bsl::is_convertible<const volatile float&, int >::value));

        ASSERT(false ==
                    (bsl::is_convertible<const float*, V_(int)*>::value));
        ASSERT(false ==
                    (bsl::is_convertible<volatile float*, const int*>::value));
        ASSERT(true  ==
                    (bsl::is_convertible<const float&, V_(int) >::value));
        ASSERT(true  ==
                    (bsl::is_convertible<volatile float&, const int >::value));
#if !defined(BSLMF_ODD_COMPILER_CONST_OR_VOLATILE_CONVERSION_RULES)
        ASSERT(false ==
           (bsl::is_convertible<const    float&, CV(int)&>::value));
        ASSERT(false ==
           (bsl::is_convertible<volatile float&, CV(int)&>::value));
        ASSERT(false ==
           (bsl::is_convertible<const    float , CV(int)&>::value));
        ASSERT(false ==
           (bsl::is_convertible<volatile float , CV(int)&>::value));
#endif
        ASSERT(false ==
            (bsl::is_convertible<const          float, V_(int)&>::value));
        ASSERT(false ==
            (bsl::is_convertible<const volatile float, V_(int)&>::value));

        ASSERT(true  ==
               (bsl::is_convertible<      volatile float, const int&>::value));
        ASSERT(true  ==
               (bsl::is_convertible<const volatile float, const int&>::value));

        ASSERT(false == (bsl::is_convertible<float*, int>::value));
        ASSERT(false == (bsl::is_convertible<float, int*>::value));

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // 'bslmf::IsConvertible::VALUE'
        //
        // Concerns:
        //: 1 'IsConvertible::VALUE' returns the correct value when both
        //:   'FROM_TYPE' and 'TO_TYPE' are basic types.
        //:
        //: 2 'IsConvertible::VALUE' returns the correct value when one of
        //:   'FROM_TYPE' and 'TO_TYPE' is a 'const' type.
        //:
        //: 3 'IsConvertible::VALUE' returns the correct value when one of
        //:   'FROM_TYPE' and 'TO_TYPE' is a 'const' pointer or 'const'
        //:   reference type.
        //:
        //: 4 'IsConvertible::VALUE' returns the correct value when one of
        //:   'FROM_TYPE' and 'TO_TYPE' is a 'volatile' type.
        //:
        //: 5 'IsConvertible::VALUE' returns the correct value when 'FROM_TYPE'
        //:   and 'TO_TYPE' are various combinations of (possibly cv-qualified)
        //:   types.
        //:
        //: 6 'IsConvertible::VALUE' returns the correct value when one of
        //:   'FROM_TYPE' and 'TO_TYPE' is a 'volatile' pointer or 'volatile'
        //:   reference type.
        //:
        //: 7 'IsConvertible::VALUE' returns the correct value when 'FROM_TYPE'
        //:   and 'TO_TYPE' are various combinations of different (possibly
        //:   cv-qualified) user-defined types, pointer to user-defined types,
        //:   and reference to user-defined types.
        //:
        //: 8 'IsConvertible::VALUE' returns the correct value when one or both
        //:   of 'FROM_TYPE' and 'TO_TYPE' are 'void' types.
        //:
        //: 9 'IsConvertible::VALUE' returns the correct value when conversion
        //:   happens between a base class type and a derived class type.
        //:
        //: 10 'IsConvertible::VALUE' returns the correct value when conversion
        //:    happens between pointer to base class member object type and
        //:    pointer to derived class member object type.
        //:
        //: 11 'IsConvertible::VALUE' returns the correct value when conversion
        //:    happens between pointer to base class member function type and
        //:    pointer to derived class member function type.
        //:
        //: 12 'IsConvertible::VALUE' returns the correct value when conversion
        //:    happens between arrays of unknown bound, and other types.
        //:
        //: 13 'IsConvertible::VALUE' returns the correct value when conversion
        //:    happens between incomplete types and other types.
        //:
        //: 14 Test function references decay to function pointers
        //
        // Plan:
        //: 1 Instantiate 'bslmf::IsConvertible' with various type combinations
        //:   and verify that the 'VALUE' member is initialized properly.
        //:   (C-1..14)
        //
        // Testing:
        //   bslmf::IsConvertible::VALUE
        // --------------------------------------------------------------------

        if (verbose) printf("'bslmf::IsConvertible::VALUE'\n"
                            "=============================\n");

        ASSERT(2 == sizeof(C10));
        ASSERT(2 == sizeof(C11));
        ASSERT(1 == sizeof(C12));
        ASSERT(1 == sizeof(C13));

        // C-1: Test conversion of basic types.

        ASSERT(1 == (bslmf::IsConvertible<int, int  >::VALUE));
        ASSERT(1 == (bslmf::IsConvertible<int, char >::VALUE));
        ASSERT(0 == (bslmf::IsConvertible<int, char*>::VALUE));
        ASSERT(0 == (bslmf::IsConvertible<void*, char*>::VALUE));
        ASSERT(1 == (bslmf::IsConvertible<char*, void*>::VALUE));

        // C-2: Test const value conversions.

        ASSERT(1 == (bslmf::IsConvertible<int, const int>::VALUE));
        ASSERT(1 == (bslmf::IsConvertible<const int, int>::VALUE));

        // C-3: Test const pointer and reference conversions.

        ASSERT(1 == (bslmf::IsConvertible<int*, const int*>::VALUE));
        ASSERT(0 == (bslmf::IsConvertible<const int*, int*>::VALUE));
        ASSERT(1 == (bslmf::IsConvertible<int&, const int&>::VALUE));
        ASSERT(0 == (bslmf::IsConvertible<const int&, int&>::VALUE));
        ASSERT(0 == (bslmf::IsConvertible<int,  int&>::VALUE));
        ASSERT(1 == (bslmf::IsConvertible<int,  const int&>::VALUE));
        ASSERT(1 == (bslmf::IsConvertible<int&,  int&>::VALUE));
        ASSERT(1 == (bslmf::IsConvertible<const int&,  const int&>::VALUE));
        ASSERT(0 == (bslmf::IsConvertible<const int,  int&>::VALUE));
        ASSERT(1 == (bslmf::IsConvertible<int&,  const int>::VALUE));
        ASSERT(1 == (bslmf::IsConvertible<const int&, int>::VALUE));
        ASSERT(0 == (bslmf::IsConvertible<const int,  int&>::VALUE));
        ASSERT(1 == (bslmf::IsConvertible<const int,  const int&>::VALUE));
        ASSERT(1 == (bslmf::IsConvertible<const int&, const int>::VALUE));

        // C-4: Test volatile value conversions.

        ASSERT(1 == (bslmf::IsConvertible<int, V_(int)>::VALUE));
        ASSERT(1 == (bslmf::IsConvertible<V_(int), int>::VALUE));

        // C-5: Test volatile pointer and reference conversions

        ASSERT(1 == (bslmf::IsConvertible<int*, V_(int)*>::VALUE));
        ASSERT(0 == (bslmf::IsConvertible<V_(int)*, int*>::VALUE));
        ASSERT(1 == (bslmf::IsConvertible<int&, V_(int)&>::VALUE));
        ASSERT(0 == (bslmf::IsConvertible<V_(int)&, int&>::VALUE));
        ASSERT(0 == (bslmf::IsConvertible<int,  V_(int)&>::VALUE));
        ASSERT(0 == (bslmf::IsConvertible<V_(int),  int&>::VALUE));
        ASSERT(1 == (bslmf::IsConvertible<int&,  V_(int)>::VALUE));
        ASSERT(1 == (bslmf::IsConvertible<V_(int)&,  int>::VALUE));

        // C-6: Test conversions on different combinations of cv-qualified
        //      types.

        ASSERT(1 == (bslmf::IsConvertible<int*, CV(int)*>::VALUE));
        ASSERT(0 == (bslmf::IsConvertible<CV(int)*, int*>::VALUE));
        ASSERT(1 == (bslmf::IsConvertible<int&, CV(int)&>::VALUE));
        ASSERT(0 == (bslmf::IsConvertible<CV(int)&, int&>::VALUE));
        ASSERT(0 == (bslmf::IsConvertible<int,  CV(int)&>::VALUE));
        ASSERT(0 == (bslmf::IsConvertible<CV(int),  int&>::VALUE));
        ASSERT(1 == (bslmf::IsConvertible<int&,  CV(int)>::VALUE));
        ASSERT(1 == (bslmf::IsConvertible<CV(int)&,  int>::VALUE));

        ASSERT(0 == (bslmf::IsConvertible<const int*, V_(int)*>::VALUE));
        ASSERT(0 == (bslmf::IsConvertible<V_(int)*, const int*>::VALUE));
        ASSERT(1 == (bslmf::IsConvertible<const int&, V_(int)>::VALUE));
        ASSERT(1 == (bslmf::IsConvertible<V_(int)&, const int>::VALUE));
        ASSERT(1 == (bslmf::IsConvertible<const int&,
                                          CV(int)&>::VALUE));
        ASSERT(1 == (bslmf::IsConvertible<V_(int)&,
                                          CV(int)&>::VALUE));
        ASSERT(0 == (bslmf::IsConvertible<const int,
                                          CV(int)&>::VALUE));
        ASSERT(0 == (bslmf::IsConvertible<V_(int),
                                          CV(int)&>::VALUE));
        ASSERT(0 == (bslmf::IsConvertible<const int,
                                          V_(int)&>::VALUE));
        ASSERT(0 == (bslmf::IsConvertible<CV(int),
                                          V_(int)&>::VALUE));

#if !defined(BSLS_PLATFORM_CMP_SUN)
        // Sun 5.2 and 5.5 both get this wrong when the cv-unqualified types
        // are the same.  Confirmed as recently as the 12.3 compiler.
        ASSERT(0 == (bslmf::IsConvertible<V_(int),
                                          const int&>::VALUE));
        ASSERT(0 == (bslmf::IsConvertible<CV(int),
                                          const int&>::VALUE));
#endif
        // C-7: Test conversions on different combinations of cv-qualified
        //      user-defined types.

        ASSERT(0 == (bslmf::IsConvertible<int*, int>::VALUE));
        ASSERT(0 == (bslmf::IsConvertible<int, int*>::VALUE));

        ASSERT(1 == (bslmf::IsConvertible<my_Class, my_Class>::VALUE));
        ASSERT(1 == (bslmf::IsConvertible<my_Class, const my_Class>::VALUE));
        ASSERT(1 == (bslmf::IsConvertible<my_Class, const my_Class&>::VALUE));
        ASSERT(0 == (bslmf::IsConvertible<const my_Class, my_Class&>::VALUE));
        ASSERT(1 == (bslmf::IsConvertible<const my_Class,
                                          const my_Class&>::VALUE));
        ASSERT(1 == (bslmf::IsConvertible<const my_Class&,
                                          const my_Class>::VALUE));

        ASSERT(0 == (bslmf::IsConvertible<const my_Class, my_Class&>::VALUE));
        ASSERT(1 == (bslmf::IsConvertible<int, my_Class>::VALUE));
        ASSERT(0 == (bslmf::IsConvertible<my_Class, int>::VALUE));
        ASSERT(0 == (bslmf::IsConvertible<int*, my_Class*>::VALUE));

        ASSERT(1 == (bslmf::IsConvertible<my_OtherClass, my_Class>::VALUE));
        ASSERT(1 == (bslmf::IsConvertible<my_OtherClass,
                                          const my_Class>::VALUE));

        ASSERT(0 == (bslmf::IsConvertible<const my_OtherClass,
                                          const my_Class>::VALUE));

        ASSERT(1 == (bslmf::IsConvertible<my_Class, my_ThirdClass>::VALUE));
        ASSERT(1 == (bslmf::IsConvertible<const my_Class,
                                          my_ThirdClass>::VALUE));
        ASSERT(1 == (bslmf::IsConvertible<my_Class,
                                          const my_ThirdClass>::VALUE));
        ASSERT(1 == (bslmf::IsConvertible<const my_Class,
                                          const my_ThirdClass>::VALUE));
        ASSERT(0 == (bslmf::IsConvertible<volatile my_Class,
                                          my_ThirdClass>::VALUE));
        ASSERT(1 == (bslmf::IsConvertible<my_Class&,
                                          const my_ThirdClass>::VALUE));
        ASSERT(1 == (bslmf::IsConvertible<const my_Class&,
                                          const my_ThirdClass>::VALUE));
        ASSERT(0 == (bslmf::IsConvertible<volatile my_Class&,
                                          my_ThirdClass>::VALUE));
        ASSERT(1 == (bslmf::IsConvertible<my_Class&,
                                          const my_ThirdClass&>::VALUE));
        ASSERT(1 == (bslmf::IsConvertible<const my_Class&,
                                          const my_ThirdClass&>::VALUE));
        ASSERT(0 == (bslmf::IsConvertible<volatile my_Class&,
                                          my_ThirdClass&>::VALUE));

        ASSERT(0 == (bslmf::IsConvertible<my_OtherClass, int>::VALUE));

        ASSERT(1 == (bslmf::IsConvertible<my_Enum, my_Enum >::VALUE));
        ASSERT(1 == (bslmf::IsConvertible<my_Enum, int     >::VALUE));
        ASSERT(0 == (bslmf::IsConvertible<int, my_Enum     >::VALUE));

        ASSERT(1 == (bslmf::IsConvertible<my_Enum,  my_Class>::VALUE));
        ASSERT(0 == (bslmf::IsConvertible<my_Class,  my_Enum>::VALUE));
        ASSERT(0 == (bslmf::IsConvertible<my_Enum, my_Class*>::VALUE));
        ASSERT(0 == (bslmf::IsConvertible<my_Class, my_Enum*>::VALUE));
        ASSERT(0 == (bslmf::IsConvertible<my_Enum*, my_Class>::VALUE));
        ASSERT(0 == (bslmf::IsConvertible<my_Class*, my_Enum>::VALUE));

        ASSERT(1 == (bslmf::IsConvertible<my_EnumClass::Type,
                                          my_Class>::VALUE));
        ASSERT(0 == (bslmf::IsConvertible<my_Class,
                                          my_EnumClass::Type>::VALUE));
        ASSERT(0 == (bslmf::IsConvertible<my_EnumClass::Type,
                                          my_Class*>::VALUE));
        ASSERT(0 == (bslmf::IsConvertible<my_Class,
                                          my_EnumClass::Type*>::VALUE));
        ASSERT(0 == (bslmf::IsConvertible<my_EnumClass::Type*,
                                          my_Class>::VALUE));
        ASSERT(0 == (bslmf::IsConvertible<my_Class*,
                                          my_EnumClass::Type>::VALUE));

        ASSERT(1 == (bslmf::IsConvertible<my_BslmaAllocator*,
                                          my_StlAllocator<int> >::VALUE));
        ASSERT(1 == (bslmf::IsConvertible<my_BslmaAllocator*,
                                          my_StlAllocator<void> >::VALUE));
        ASSERT(1 == (bslmf::IsConvertible<my_BslmaAllocator*,
                                          my_StlAllocator<void*> >::VALUE));
        ASSERT(1 == (bslmf::IsConvertible<my_BslmaAllocator*,
                                          my_StlAllocator<my_Enum> >::VALUE));
        ASSERT(0 == (bslmf::IsConvertible<void*,
                                          my_StlAllocator<my_Enum> >::VALUE));
        ASSERT(1 == (bslmf::IsConvertible<my_BslmaAllocator*,
                                          my_StlAllocator<my_EnumClass::Type>
                                         >::VALUE));
        ASSERT(0 == (bslmf::IsConvertible<void*,
                                          my_StlAllocator<my_EnumClass::Type>
                                         >::VALUE));

        // C-8: Test conversion between 'void' type and other types.

        ASSERT(1 == (bslmf::IsConvertible<void, void>::VALUE));
        ASSERT(0 == (bslmf::IsConvertible<void, int>::VALUE));
        ASSERT(0 == (bslmf::IsConvertible<int, void>::VALUE));

        // const-qualified void should also be supported
        ASSERT(1 == (bslmf::IsConvertible<const void, const void>::VALUE));
        ASSERT(1 == (bslmf::IsConvertible<void, const void>::VALUE));
        ASSERT(0 == (bslmf::IsConvertible<const void, int>::VALUE));
        ASSERT(0 == (bslmf::IsConvertible<int, const void>::VALUE));

        // C-9: Test derived-to-base convertibility

        ASSERT(NOT_NATIVE == (bslmf::IsConvertible<my_DerivedClass,
                                          my_AbstractClass>::VALUE));
        ASSERT(0 == (bslmf::IsConvertible<my_AbstractClass,
                                          my_DerivedClass>::VALUE));
        ASSERT(1 == (bslmf::IsConvertible<my_DerivedClass&,
                                          my_AbstractClass&>::VALUE));
        ASSERT(0 == (bslmf::IsConvertible<my_AbstractClass&,
                                          my_DerivedClass&>::VALUE));
        ASSERT(1 == (bslmf::IsConvertible<my_DerivedClass*,
                                          my_AbstractClass*>::VALUE));
        ASSERT(0 == (bslmf::IsConvertible<my_AbstractClass*,
                                          my_DerivedClass*>::VALUE));

        // C-10: Test pointer-to-member variable convertibility

        ASSERT(1 == (bslmf::IsConvertible<int my_AbstractClass::*,
                                          int my_DerivedClass::*>::VALUE));
        ASSERT(0 == (bslmf::IsConvertible<int my_DerivedClass::*,
                                          int my_AbstractClass::*>::VALUE));
        ASSERT(0 == (bslmf::IsConvertible<int my_AbstractClass::*,
                                          void*>::VALUE));
        ASSERT(0 == (bslmf::IsConvertible<void*,
                                          int my_AbstractClass::*>::VALUE));

        // C-11: Test pointer-to-member function convertibility

        ASSERT(1 == (bslmf::IsConvertible<int (my_AbstractClass::*)(int),
                                          int (my_DerivedClass::*)(int)
                                         >::VALUE));
        ASSERT(0 == (bslmf::IsConvertible<int (my_DerivedClass::*)(int),
                                          int (my_AbstractClass::*)(int)
                                         >::VALUE));
        ASSERT(0 == (bslmf::IsConvertible<int (my_AbstractClass::*)(int),
                                          void*>::VALUE));
        ASSERT(0 == (bslmf::IsConvertible<void*,
                                          int (my_AbstractClass::*)(int)
                                         >::VALUE));

        // C-12: Test arrays of unknown bound convertibility

        ASSERT(1 == (bslmf::IsConvertible<int (*)[],    int (*)[]   >::VALUE));
        ASSERT(1 == (bslmf::IsConvertible<int (*)[][5], int (*)[][5]>::VALUE));
        ASSERT(0 == (bslmf::IsConvertible<int (*)[],    int*        >::VALUE));
        ASSERT(0 == (bslmf::IsConvertible<int*,         int (*)[]   >::VALUE));
        ASSERT(0 == (bslmf::IsConvertible<int (*)[][5], int*        >::VALUE));
        ASSERT(1 == (bslmf::IsConvertible<
                                       int (*)[], V_(int) (*)[]>::VALUE));
        ASSERT(0 == (bslmf::IsConvertible<
                              V_(int) (*)[],          int (*)[]>::VALUE));
        ASSERT(1 == (bslmf::IsConvertible<
                                       int (*)[], const    int (*)[]>::VALUE));
        ASSERT(0 == (bslmf::IsConvertible<
                              const    int (*)[],          int (*)[]>::VALUE));
        ASSERT(0 == (bslmf::IsConvertible<
                              const    int (*)[], V_(int) (*)[]>::VALUE));
        ASSERT(0 == (bslmf::IsConvertible<
                              V_(int) (*)[], const    int (*)[]>::VALUE));

        // C-13
#if defined(BSLMF_ISCONVERTIBLE_TEST_COMPILE_ERROR_WITH_INCOMPLETE_TYPES)
        // An incomplete class can only be tested as the FROM parameter.

        ASSERT(1 == (bslmf::IsConvertible<my_IncompleteClass,
                                          my_IncompleteClass>::VALUE));
        ASSERT(0 == (bslmf::IsConvertible<my_IncompleteClass, int>::VALUE));

        // Test references with incomplete types as both TO and FROM parameters

        ASSERT(1 == (bslmf::IsConvertible<my_IncompleteClass,
                                          my_IncompleteClass&>::VALUE));
        ASSERT(0 == (bslmf::IsConvertible<my_IncompleteClass&, int>::VALUE));
        ASSERT(0 == (bslmf::IsConvertible<int, my_IncompleteClass&>::VALUE));
        ASSERT(0 == (bslmf::IsConvertible<my_IncompleteClass,
                                          my_IncompleteClass2&>::VALUE));
        ASSERT(0 == (bslmf::IsConvertible<my_IncompleteClass&,
                                          my_IncompleteClass2&>::VALUE));
#endif

        // C-14: Test function references decay to function pointers

        ASSERT(false == (bslmf::IsConvertible<void(), void>::VALUE));
        ASSERT(false == (bslmf::IsConvertible<void(), int>::VALUE));
        ASSERT(false == (bslmf::IsConvertible<void(), const void>::VALUE));
        ASSERT(false == (bslmf::IsConvertible<void(), V_(int)>::VALUE));
        ASSERT(false == (bslmf::IsConvertible<void,         void()>::VALUE));
        ASSERT(false == (bslmf::IsConvertible<int,          void()>::VALUE));
        ASSERT(false == (bslmf::IsConvertible<const void,   void()>::VALUE));
        ASSERT(false == (bslmf::IsConvertible<V_(int), void()>::VALUE));

        ASSERT(false == (bslmf::IsConvertible<void    (), void    ()>::value));
        ASSERT(true  == (bslmf::IsConvertible<void    (), void (&)()>::value));
        ASSERT(true  == (bslmf::IsConvertible<void    (), void (*)()>::value));
        ASSERT(true  == (bslmf::IsConvertible<void (&)(), void (&)()>::value));
        ASSERT(true  == (bslmf::IsConvertible<void (&)(), void (*)()>::value));
        ASSERT(false == (bslmf::IsConvertible<void (*)(), void    ()>::value));
        ASSERT(false == (bslmf::IsConvertible<void (*)(), void (&)()>::value));
        ASSERT(false == (bslmf::IsConvertible<void (&)(), void    ()>::value));
        ASSERT(true  == (bslmf::IsConvertible<void (*)(), void (*)()>::value));

        ASSERT(false == (bslmf::IsConvertible<void(...), void>::VALUE));
        ASSERT(false == (bslmf::IsConvertible<void(...), int>::VALUE));
        ASSERT(false == (bslmf::IsConvertible<void(...), const void>::VALUE));
        ASSERT(false ==
                       (bslmf::IsConvertible<void(...), V_(int)>::VALUE));
        ASSERT(false == (bslmf::IsConvertible<void,        void(...)>::VALUE));
        ASSERT(false == (bslmf::IsConvertible<int,         void(...)>::VALUE));
        ASSERT(false == (bslmf::IsConvertible<const void,  void(...)>::VALUE));
        ASSERT(false ==
                       (bslmf::IsConvertible<V_(int), void(...)>::VALUE));

        ASSERT(false  ==
                  (bslmf::IsConvertible<void    (...), void    (...)>::value));
        ASSERT(true  ==
                  (bslmf::IsConvertible<void    (...), void (&)(...)>::value));
        ASSERT(true  ==
                  (bslmf::IsConvertible<void    (...), void (*)(...)>::value));
        ASSERT(true  ==
                  (bslmf::IsConvertible<void (&)(...), void (&)(...)>::value));
        ASSERT(true  ==
                  (bslmf::IsConvertible<void (&)(...), void (*)(...)>::value));
        ASSERT(false ==
                  (bslmf::IsConvertible<void (*)(...), void    (...)>::value));
        ASSERT(false ==
                  (bslmf::IsConvertible<void (*)(...), void (&)(...)>::value));
        ASSERT(false  ==
                  (bslmf::IsConvertible<void (&)(...), void    (...)>::value));

        ASSERT(false ==
                     (bslmf::IsConvertible<void    (...), void    ()>::value));
        ASSERT(false ==
                     (bslmf::IsConvertible<void    (...), void (&)()>::value));
        ASSERT(false ==
                     (bslmf::IsConvertible<void    (...), void (*)()>::value));
        ASSERT(false ==
                     (bslmf::IsConvertible<void (&)(...), void (&)()>::value));
        ASSERT(false ==
                     (bslmf::IsConvertible<void (&)(...), void (*)()>::value));
        ASSERT(false ==
                     (bslmf::IsConvertible<void (*)(...), void    ()>::value));
        ASSERT(false ==
                     (bslmf::IsConvertible<void (*)(...), void (&)()>::value));
        ASSERT(false ==
                     (bslmf::IsConvertible<void (&)(...), void    ()>::value));

        ASSERT(false ==
                     (bslmf::IsConvertible<void    (), void    (...)>::value));
        ASSERT(false ==
                     (bslmf::IsConvertible<void    (), void (&)(...)>::value));
        ASSERT(false ==
                     (bslmf::IsConvertible<void    (), void (*)(...)>::value));
        ASSERT(false ==
                     (bslmf::IsConvertible<void (&)(), void (&)(...)>::value));
        ASSERT(false ==
                     (bslmf::IsConvertible<void (&)(), void (*)(...)>::value));
        ASSERT(false ==
                     (bslmf::IsConvertible<void (*)(), void    (...)>::value));
        ASSERT(false ==
                     (bslmf::IsConvertible<void (*)(), void (&)(...)>::value));
        ASSERT(false ==
                     (bslmf::IsConvertible<void (&)(), void    (...)>::value));

        ASSERT(0 == (bslmf::IsConvertible<void(), void>::VALUE));
        ASSERT(0 == (bslmf::IsConvertible<void(),  int>::VALUE));
        ASSERT(0 == (bslmf::IsConvertible<void, void()>::VALUE));
        ASSERT(0 == (bslmf::IsConvertible<int,  void()>::VALUE));

        ASSERT(0 == (bslmf::IsConvertible<int(char, float...), void>::VALUE));
        ASSERT(0 == (bslmf::IsConvertible<int(char, float...),  int>::VALUE));
        ASSERT(0 == (bslmf::IsConvertible<void, int(char, float...)>::VALUE));
        ASSERT(0 == (bslmf::IsConvertible<int,  int(char, float...)>::VALUE));

        ASSERT(0 == (bslmf::IsConvertible<void(), void()>::VALUE));
        ASSERT(1 == (bslmf::IsConvertible<void(), void(&)()>::VALUE));
        ASSERT(1 == (bslmf::IsConvertible<void(), void(*)()>::VALUE));
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // 'bsl::is_convertible::value'
        //
        // Concerns:
        //: 1 'is_convertible::value' returns the correct value when both
        //:   'FROM_TYPE' and 'TO_TYPE' are basic types.
        //:
        //: 2 'is_convertible::value' returns the correct value when one of
        //:   'FROM_TYPE' and 'TO_TYPE' is a 'const' type.
        //:
        //: 3 'is_convertible::value' returns the correct value when one of
        //:   'FROM_TYPE' and 'TO_TYPE' is  a 'const' pointer or 'const'
        //:   reference type.
        //:
        //: 4 'is_convertible::value' returns the correct value when one of
        //:   'FROM_TYPE' and 'TO_TYPE' is a 'volatile' type.
        //:
        //: 5 'is_convertible::value' returns the correct value when
        //:   'FROM_TYPE' and 'TO_TYPE' are various combinations of (possibly
        //:   cv-qualified) types.
        //:
        //: 6 'is_convertible::value' returns the correct value when one of
        //:   'FROM_TYPE' and 'TO_TYPE' is a 'volatile' pointer or 'volatile'
        //:   reference type.
        //:
        //: 7 'is_convertible::value' returns the correct value when
        //:   'FROM_TYPE' and 'TO_TYPE' are various combinations of different
        //:   (possibly cv-qualified) user-defined types, pointer to
        //:   user-defined types, and reference to user-defined types.
        //:
        //: 8 'is_convertible::value' returns the correct value when one or
        //:    both of 'FROM_TYPE' and 'TO_TYPE' are 'void' types.
        //:
        //: 9 'is_convertible::value' returns the correct value when conversion
        //:   happens between a base class type and a derived class type.
        //:
        //: 10 'is_convertible::value' returns the correct value when
        //:    conversion happens between pointer to base class member object
        //:    type and pointer to derived class member object type.
        //:
        //: 11 'is_convertible::value' returns the correct value when
        //:    conversion happens between pointer to base class member function
        //:    type and pointer to derived class member function type.
        //:
        //: 12 'is_convertible::value' returns the correct value when
        //:    conversion happens between arrays of unknown bound, and other
        //:    types.
        //
        // Plan:
        //: 1 Instantiate 'bsl::is_convertible' with various type combinations
        //:   and verify that the 'VALUE' member is initialized properly.
        //:   (C-1..11)
        //
        // Testing:
        //   bsl::is_convertible::value
        // --------------------------------------------------------------------

        if (verbose) printf("'bsl::is_convertible::value'\n"
                            "============================\n");

        ASSERT(2 == sizeof(C00));
        ASSERT(2 == sizeof(C01));
        ASSERT(1 == sizeof(C02));
        ASSERT(1 == sizeof(C03));

        // C-1: Test conversion of basic types.

        ASSERT_IS_CONVERTIBLE(true,  int,   int  );
        ASSERT_IS_CONVERTIBLE(true,  int,   char );
        ASSERT_IS_CONVERTIBLE(false, int,   char*);
        ASSERT_IS_CONVERTIBLE(false, void*, char*);
        ASSERT_IS_CONVERTIBLE(true,  char*, void*);

        // C-2: Test cv-qualified conversions for values of the same type.

        ASSERT_IS_CONVERTIBLE(true,     int,     int );
        ASSERT_IS_CONVERTIBLE(true,     int , C_(int));
        ASSERT_IS_CONVERTIBLE(true,     int , V_(int));
        ASSERT_IS_CONVERTIBLE(true,     int , CV(int));

        ASSERT_IS_CONVERTIBLE(true,  C_(int),    int );
        ASSERT_IS_CONVERTIBLE(true,  C_(int), C_(int));
        ASSERT_IS_CONVERTIBLE(true,  C_(int), V_(int));
        ASSERT_IS_CONVERTIBLE(true,  C_(int), CV(int));

        ASSERT_IS_CONVERTIBLE(true,  V_(int),    int );
        ASSERT_IS_CONVERTIBLE(true,  V_(int), C_(int));
        ASSERT_IS_CONVERTIBLE(true,  V_(int), V_(int));
        ASSERT_IS_CONVERTIBLE(true,  V_(int), CV(int));

        ASSERT_IS_CONVERTIBLE(true,  CV(int),    int );
        ASSERT_IS_CONVERTIBLE(true,  CV(int), C_(int));
        ASSERT_IS_CONVERTIBLE(true,  CV(int), V_(int));
        ASSERT_IS_CONVERTIBLE(true,  CV(int), CV(int));

        // C3 Pointer conversions with cv-qualified variants of the same type

        ASSERT_IS_CONVERTIBLE(true,     int *,    int *);
        ASSERT_IS_CONVERTIBLE(true,     int *, C_(int)*);
        ASSERT_IS_CONVERTIBLE(true,     int *, V_(int)*);
        ASSERT_IS_CONVERTIBLE(true,     int *, CV(int)*);

        ASSERT_IS_CONVERTIBLE(false, C_(int)*,    int *);
        ASSERT_IS_CONVERTIBLE(true,  C_(int)*, C_(int)*);
        ASSERT_IS_CONVERTIBLE(false, C_(int)*, V_(int)*);
        ASSERT_IS_CONVERTIBLE(true,  C_(int)*, CV(int)*);

        ASSERT_IS_CONVERTIBLE(false, V_(int)*,    int *);
        ASSERT_IS_CONVERTIBLE(false, V_(int)*, C_(int)*);
        ASSERT_IS_CONVERTIBLE(true,  V_(int)*, V_(int)*);
        ASSERT_IS_CONVERTIBLE(true,  V_(int)*, CV(int)*);

        ASSERT_IS_CONVERTIBLE(false, CV(int)*,    int *);
        ASSERT_IS_CONVERTIBLE(false, CV(int)*, C_(int)*);
        ASSERT_IS_CONVERTIBLE(false, CV(int)*, V_(int)*);
        ASSERT_IS_CONVERTIBLE(true,  CV(int)*, CV(int)*);


        ASSERT_IS_CONVERTIBLE(true,     int *const,    int *);
        ASSERT_IS_CONVERTIBLE(true,     int *const, C_(int)*);
        ASSERT_IS_CONVERTIBLE(true,     int *const, V_(int)*);
        ASSERT_IS_CONVERTIBLE(true,     int *const, CV(int)*);

        ASSERT_IS_CONVERTIBLE(false, C_(int)*const,    int *);
        ASSERT_IS_CONVERTIBLE(true,  C_(int)*const, C_(int)*);
        ASSERT_IS_CONVERTIBLE(false, C_(int)*const, V_(int)*);
        ASSERT_IS_CONVERTIBLE(true,  C_(int)*const, CV(int)*);

        ASSERT_IS_CONVERTIBLE(false, V_(int)*const,    int *);
        ASSERT_IS_CONVERTIBLE(false, V_(int)*const, C_(int)*);
        ASSERT_IS_CONVERTIBLE(true,  V_(int)*const, V_(int)*);
        ASSERT_IS_CONVERTIBLE(true,  V_(int)*const, CV(int)*);

        ASSERT_IS_CONVERTIBLE(false, CV(int)*const,    int *);
        ASSERT_IS_CONVERTIBLE(false, CV(int)*const, C_(int)*);
        ASSERT_IS_CONVERTIBLE(false, CV(int)*const, V_(int)*);
        ASSERT_IS_CONVERTIBLE(true,  CV(int)*const, CV(int)*);


        ASSERT_IS_CONVERTIBLE(true,     int *volatile,    int *);
        ASSERT_IS_CONVERTIBLE(true,     int *volatile, C_(int)*);
        ASSERT_IS_CONVERTIBLE(true,     int *volatile, V_(int)*);
        ASSERT_IS_CONVERTIBLE(true,     int *volatile, CV(int)*);

        ASSERT_IS_CONVERTIBLE(false, C_(int)*volatile,    int *);
        ASSERT_IS_CONVERTIBLE(true,  C_(int)*volatile, C_(int)*);
        ASSERT_IS_CONVERTIBLE(false, C_(int)*volatile, V_(int)*);
        ASSERT_IS_CONVERTIBLE(true,  C_(int)*volatile, CV(int)*);

        ASSERT_IS_CONVERTIBLE(false, V_(int)*volatile,    int *);
        ASSERT_IS_CONVERTIBLE(false, V_(int)*volatile, C_(int)*);
        ASSERT_IS_CONVERTIBLE(true,  V_(int)*volatile, V_(int)*);
        ASSERT_IS_CONVERTIBLE(true,  V_(int)*volatile, CV(int)*);

        ASSERT_IS_CONVERTIBLE(false, CV(int)*volatile,    int *);
        ASSERT_IS_CONVERTIBLE(false, CV(int)*volatile, C_(int)*);
        ASSERT_IS_CONVERTIBLE(false, CV(int)*volatile, V_(int)*);
        ASSERT_IS_CONVERTIBLE(true,  CV(int)*volatile, CV(int)*);


        ASSERT_IS_CONVERTIBLE(true,     int *const volatile,    int *);
        ASSERT_IS_CONVERTIBLE(true,     int *const volatile, C_(int)*);
        ASSERT_IS_CONVERTIBLE(true,     int *const volatile, V_(int)*);
        ASSERT_IS_CONVERTIBLE(true,     int *const volatile, CV(int)*);

        ASSERT_IS_CONVERTIBLE(false, C_(int)*const volatile,    int *);
        ASSERT_IS_CONVERTIBLE(true,  C_(int)*const volatile, C_(int)*);
        ASSERT_IS_CONVERTIBLE(false, C_(int)*const volatile, V_(int)*);
        ASSERT_IS_CONVERTIBLE(true,  C_(int)*const volatile, CV(int)*);

        ASSERT_IS_CONVERTIBLE(false, V_(int)*const volatile,    int *);
        ASSERT_IS_CONVERTIBLE(false, V_(int)*const volatile, C_(int)*);
        ASSERT_IS_CONVERTIBLE(true,  V_(int)*const volatile, V_(int)*);
        ASSERT_IS_CONVERTIBLE(true,  V_(int)*const volatile, CV(int)*);

        ASSERT_IS_CONVERTIBLE(false, CV(int)*const volatile,    int *);
        ASSERT_IS_CONVERTIBLE(false, CV(int)*const volatile, C_(int)*);
        ASSERT_IS_CONVERTIBLE(false, CV(int)*const volatile, V_(int)*);
        ASSERT_IS_CONVERTIBLE(true,  CV(int)*const volatile, CV(int)*);

        // C-3: Test reference conversions for cv-qualified variants of a type

        ASSERT_IS_CONVERTIBLE(false,    int ,     int &);
        ASSERT_IS_CONVERTIBLE(true,     int ,  C_(int)&);
        ASSERT_IS_CONVERTIBLE(false,    int ,  V_(int)&);
        ASSERT_IS_CONVERTIBLE(false,    int ,  CV(int)&);

        ASSERT_IS_CONVERTIBLE(false, C_(int),     int &);
        ASSERT_IS_CONVERTIBLE(true,  C_(int),  C_(int)&);
        ASSERT_IS_CONVERTIBLE(false, C_(int),  V_(int)&);
        ASSERT_IS_CONVERTIBLE(false, C_(int),  CV(int)&);

        ASSERT_IS_CONVERTIBLE(false, V_(int),     int &);
        ASSERT_IS_CONVERTIBLE(false, V_(int),  C_(int)&);
        ASSERT_IS_CONVERTIBLE(false, V_(int),  V_(int)&);
        ASSERT_IS_CONVERTIBLE(false, V_(int),  CV(int)&);

        ASSERT_IS_CONVERTIBLE(false, CV(int),     int &);
        ASSERT_IS_CONVERTIBLE(false, CV(int),  C_(int)&);
        ASSERT_IS_CONVERTIBLE(false, CV(int),  V_(int)&);
        ASSERT_IS_CONVERTIBLE(false, CV(int),  CV(int)&);

        ASSERT_IS_CONVERTIBLE(true,     int &,    int  );
        ASSERT_IS_CONVERTIBLE(true,     int &, C_(int) );
        ASSERT_IS_CONVERTIBLE(true,     int &, V_(int) );
        ASSERT_IS_CONVERTIBLE(true,     int &, CV(int) );

        ASSERT_IS_CONVERTIBLE(true,  C_(int)&,    int  );
        ASSERT_IS_CONVERTIBLE(true,  C_(int)&, C_(int) );
        ASSERT_IS_CONVERTIBLE(true,  C_(int)&, V_(int) );
        ASSERT_IS_CONVERTIBLE(true,  C_(int)&, CV(int) );

        ASSERT_IS_CONVERTIBLE(true,  V_(int)&,    int  );
        ASSERT_IS_CONVERTIBLE(true,  V_(int)&, C_(int) );
        ASSERT_IS_CONVERTIBLE(true,  V_(int)&, V_(int) );
        ASSERT_IS_CONVERTIBLE(true,  V_(int)&, CV(int) );

        ASSERT_IS_CONVERTIBLE(true,  CV(int)&,    int  );
        ASSERT_IS_CONVERTIBLE(true,  CV(int)&, C_(int) );
        ASSERT_IS_CONVERTIBLE(true,  CV(int)&, V_(int) );
        ASSERT_IS_CONVERTIBLE(true,  CV(int)&, CV(int) );

        ASSERT_IS_CONVERTIBLE(true,     int &,    int &);
        ASSERT_IS_CONVERTIBLE(true,     int &, C_(int)&);
        ASSERT_IS_CONVERTIBLE(true,     int &, V_(int)&);
        ASSERT_IS_CONVERTIBLE(true,     int &, CV(int)&);

        ASSERT_IS_CONVERTIBLE(false, C_(int)&,    int &);
        ASSERT_IS_CONVERTIBLE(true,  C_(int)&, C_(int)&);
        ASSERT_IS_CONVERTIBLE(false, C_(int)&, V_(int)&);
        ASSERT_IS_CONVERTIBLE(true,  C_(int)&, CV(int)&);

        ASSERT_IS_CONVERTIBLE(false, V_(int)&,    int &);
        ASSERT_IS_CONVERTIBLE(false, V_(int)&, C_(int)&);
        ASSERT_IS_CONVERTIBLE(true,  V_(int)&, V_(int)&);
        ASSERT_IS_CONVERTIBLE(true,  V_(int)&, CV(int)&);

        ASSERT_IS_CONVERTIBLE(false, CV(int)&,    int &);
        ASSERT_IS_CONVERTIBLE(false, CV(int)&, C_(int)&);
        ASSERT_IS_CONVERTIBLE(false, CV(int)&, V_(int)&);
        ASSERT_IS_CONVERTIBLE(true,  CV(int)&, CV(int)&);


        // C-2: Test cv-qualified conversions for values of the same type.

        ASSERT_IS_CONVERTIBLE(true,          my_Class,     my_Class );
        ASSERT_IS_CONVERTIBLE(true,          my_Class , C_(my_Class));
        ASSERT_IS_CONVERTIBLE(true,          my_Class , V_(my_Class));
        ASSERT_IS_CONVERTIBLE(true,          my_Class , CV(my_Class));

        ASSERT_IS_CONVERTIBLE(true,       C_(my_Class),    my_Class );
        ASSERT_IS_CONVERTIBLE(true,       C_(my_Class), C_(my_Class));
        ASSERT_IS_CONVERTIBLE(true,       C_(my_Class), V_(my_Class));
        ASSERT_IS_CONVERTIBLE(true,       C_(my_Class), CV(my_Class));

        ASSERT_IS_CONVERTIBLE(NOT_NATIVE, V_(my_Class),    my_Class );
        ASSERT_IS_CONVERTIBLE(false,      V_(my_Class), C_(my_Class));
        ASSERT_IS_CONVERTIBLE(false,      V_(my_Class), V_(my_Class));
        ASSERT_IS_CONVERTIBLE(false,      V_(my_Class), CV(my_Class));

        ASSERT_IS_CONVERTIBLE(false,      CV(my_Class),    my_Class );
        ASSERT_IS_CONVERTIBLE(NOT_NATIVE, CV(my_Class), C_(my_Class));
        ASSERT_IS_CONVERTIBLE(false,      CV(my_Class), V_(my_Class));
        ASSERT_IS_CONVERTIBLE(false,      CV(my_Class), CV(my_Class));


        // C-3: Test reference conversions for cv-qualified variants of a type

        ASSERT_IS_CONVERTIBLE(false,    my_Class ,     my_Class &);
        ASSERT_IS_CONVERTIBLE(true,     my_Class ,  C_(my_Class)&);
        ASSERT_IS_CONVERTIBLE(false,    my_Class ,  V_(my_Class)&);
        ASSERT_IS_CONVERTIBLE(false,    my_Class ,  CV(my_Class)&);

        ASSERT_IS_CONVERTIBLE(false, C_(my_Class),     my_Class &);
        ASSERT_IS_CONVERTIBLE(true,  C_(my_Class),  C_(my_Class)&);
        ASSERT_IS_CONVERTIBLE(false, C_(my_Class),  V_(my_Class)&);
        ASSERT_IS_CONVERTIBLE(false, C_(my_Class),  CV(my_Class)&);

        ASSERT_IS_CONVERTIBLE(false, V_(my_Class),     my_Class &);
        ASSERT_IS_CONVERTIBLE(false, V_(my_Class),  C_(my_Class)&);
        ASSERT_IS_CONVERTIBLE(false, V_(my_Class),  V_(my_Class)&);
        ASSERT_IS_CONVERTIBLE(false, V_(my_Class),  CV(my_Class)&);

        ASSERT_IS_CONVERTIBLE(false, CV(my_Class),     my_Class &);
        ASSERT_IS_CONVERTIBLE(false, CV(my_Class),  C_(my_Class)&);
        ASSERT_IS_CONVERTIBLE(false, CV(my_Class),  V_(my_Class)&);
        ASSERT_IS_CONVERTIBLE(false, CV(my_Class),  CV(my_Class)&);

        ASSERT_IS_CONVERTIBLE(true,     my_Class &,    my_Class  );
        ASSERT_IS_CONVERTIBLE(true,     my_Class &, C_(my_Class) );
        ASSERT_IS_CONVERTIBLE(true,     my_Class &, V_(my_Class) );
        ASSERT_IS_CONVERTIBLE(true,     my_Class &, CV(my_Class) );

        ASSERT_IS_CONVERTIBLE(true,  C_(my_Class)&,    my_Class  );
        ASSERT_IS_CONVERTIBLE(true,  C_(my_Class)&, C_(my_Class) );
        ASSERT_IS_CONVERTIBLE(true,  C_(my_Class)&, V_(my_Class) );
        ASSERT_IS_CONVERTIBLE(true,  C_(my_Class)&, CV(my_Class) );

        ASSERT_IS_CONVERTIBLE(false, V_(my_Class)&,    my_Class  );
        ASSERT_IS_CONVERTIBLE(false, V_(my_Class)&, C_(my_Class) );
        ASSERT_IS_CONVERTIBLE(false, V_(my_Class)&, V_(my_Class) );
        ASSERT_IS_CONVERTIBLE(false, V_(my_Class)&, CV(my_Class) );

        ASSERT_IS_CONVERTIBLE(false, CV(my_Class)&,    my_Class  );
        ASSERT_IS_CONVERTIBLE(false, CV(my_Class)&, C_(my_Class) );
        ASSERT_IS_CONVERTIBLE(false, CV(my_Class)&, V_(my_Class) );
        ASSERT_IS_CONVERTIBLE(false, CV(my_Class)&, CV(my_Class) );

        ASSERT_IS_CONVERTIBLE(true,     my_Class &,    my_Class &);
        ASSERT_IS_CONVERTIBLE(true,     my_Class &, C_(my_Class)&);
        ASSERT_IS_CONVERTIBLE(true,     my_Class &, V_(my_Class)&);
        ASSERT_IS_CONVERTIBLE(true,     my_Class &, CV(my_Class)&);

        ASSERT_IS_CONVERTIBLE(false, C_(my_Class)&,    my_Class &);
        ASSERT_IS_CONVERTIBLE(true,  C_(my_Class)&, C_(my_Class)&);
        ASSERT_IS_CONVERTIBLE(false, C_(my_Class)&, V_(my_Class)&);
        ASSERT_IS_CONVERTIBLE(true,  C_(my_Class)&, CV(my_Class)&);

        ASSERT_IS_CONVERTIBLE(false, V_(my_Class)&,    my_Class &);
        ASSERT_IS_CONVERTIBLE(false, V_(my_Class)&, C_(my_Class)&);
        ASSERT_IS_CONVERTIBLE(true,  V_(my_Class)&, V_(my_Class)&);
        ASSERT_IS_CONVERTIBLE(true,  V_(my_Class)&, CV(my_Class)&);

        ASSERT_IS_CONVERTIBLE(false, CV(my_Class)&,    my_Class &);
        ASSERT_IS_CONVERTIBLE(false, CV(my_Class)&, C_(my_Class)&);
        ASSERT_IS_CONVERTIBLE(false, CV(my_Class)&, V_(my_Class)&);
        ASSERT_IS_CONVERTIBLE(true,  CV(my_Class)&, CV(my_Class)&);


        // C-3: Test const pointer and reference conversions.

        // Can bind a temporary to a 'const &'
        ASSERT(true  == (bsl::is_convertible<char,   const int&>::value));
        ASSERT(true  == (bsl::is_convertible<double, const int&>::value));

#if 0
#if !defined(BSLS_PLATFORM_CMP_SUN)
        // Sun 5.2 and 5.5 both get this wrong when the cv-unqualified types
        // are the same.  Re-confirmed with Sun CC 12.3.
        ASSERT(false == (bsl::is_convertible<V_(int),
                                             const int&>::value));
        ASSERT(false == (bsl::is_convertible<CV(int),
                                             const int&>::value));
#endif
        ASSERT(false == (bsl::is_convertible<V_(int) &,
                                             const int&>::value));
#endif
        ASSERT(true  == (bsl::is_convertible<volatile double,
                                             const int&>::value));
        ASSERT(true  == (bsl::is_convertible<char &,   const int&>::value));

        // C-4: Test volatile value conversions.

        ASSERT(true == (bsl::is_convertible<int, V_(int)>::value));
        ASSERT(true == (bsl::is_convertible<V_(int), int>::value));

        // C-5: Test volatile pointer and reference conversions


        // C-6: Test conversions on different combinations of cv-qualified
        //      types.


        // C-7: Test conversions on different combinations of cv-qualified
        //      user-defined types.

        ASSERT(true  ==
               (bsl::is_convertible<      int,             my_Class >::value));
        ASSERT(false ==
               (bsl::is_convertible<      my_Class,        int      >::value));
        ASSERT(false ==
               (bsl::is_convertible<      int*,            my_Class*>::value));

        ASSERT(true  == (bsl::is_convertible<
                                 my_OtherClass,       my_Class      >::value));
        ASSERT(true  == (bsl::is_convertible<
                                 my_OtherClass, const my_Class      >::value));
        ASSERT(false == (bsl::is_convertible<
                        const    my_OtherClass, const my_Class      >::value));
        ASSERT(true  == (bsl::is_convertible<
                                 my_Class,            my_ThirdClass >::value));
        ASSERT(true  == (bsl::is_convertible<
                        const    my_Class,            my_ThirdClass >::value));
        ASSERT(true  == (bsl::is_convertible<
                                 my_Class,      const my_ThirdClass >::value));
        ASSERT(true  == (bsl::is_convertible<
                        const    my_Class,      const my_ThirdClass >::value));
        ASSERT(false == (bsl::is_convertible<
                        volatile my_Class,            my_ThirdClass >::value));
        ASSERT(true  == (bsl::is_convertible<
                                 my_Class&,     const my_ThirdClass >::value));
        ASSERT(true  == (bsl::is_convertible<
                        const    my_Class&,     const my_ThirdClass >::value));
        ASSERT(false == (bsl::is_convertible<
                        volatile my_Class&,           my_ThirdClass >::value));
        ASSERT(true  == (bsl::is_convertible<
                                 my_Class&,     const my_ThirdClass&>::value));
        ASSERT(true  == (bsl::is_convertible<
                        const    my_Class&,     const my_ThirdClass&>::value));
        ASSERT(false == (bsl::is_convertible<
                        volatile my_Class&,           my_ThirdClass&>::value));

        ASSERT(NOT_NATIVE == (bsl::is_convertible<
                            my_NotCopyableClass, my_NotCopyableClass>::value));
        ASSERT(false == (bsl::is_convertible<
                          my_NotCopyableClass, my_NotCopyableClass &>::value));
        ASSERT(true == (bsl::is_convertible<
                    my_NotCopyableClass, const my_NotCopyableClass &>::value));

        ASSERT(false == (bsl::is_convertible<my_OtherClass, int>::value));

        ASSERT(true  == (bsl::is_convertible<my_Enum, my_Enum>::value));
        ASSERT(true  == (bsl::is_convertible<my_Enum, int    >::value));
        ASSERT(false == (bsl::is_convertible<int,     my_Enum>::value));

        ASSERT(true  == (bsl::is_convertible<my_Enum,   my_Class >::value));
        ASSERT(false == (bsl::is_convertible<my_Class,  my_Enum  >::value));
        ASSERT(false == (bsl::is_convertible<my_Enum,   my_Class*>::value));
        ASSERT(false == (bsl::is_convertible<my_Class,  my_Enum* >::value));
        ASSERT(false == (bsl::is_convertible<my_Enum*,  my_Class >::value));
        ASSERT(false == (bsl::is_convertible<my_Class*, my_Enum  >::value));

        ASSERT(true  == (bsl::is_convertible<my_EnumClass::Type,
                                             my_Class>::value));
        ASSERT(false == (bsl::is_convertible<my_Class,
                                             my_EnumClass::Type>::value));
        ASSERT(false == (bsl::is_convertible<my_EnumClass::Type,
                                             my_Class*>::value));
        ASSERT(false == (bsl::is_convertible<my_Class,
                                             my_EnumClass::Type*>::value));
        ASSERT(false == (bsl::is_convertible<my_EnumClass::Type*,
                                             my_Class>::value));
        ASSERT(false == (bsl::is_convertible<my_Class*,
                                             my_EnumClass::Type>::value));

        ASSERT(true  == (bsl::is_convertible<my_BslmaAllocator*,
                                             my_StlAllocator<int> >::value));
        ASSERT(true  == (bsl::is_convertible<my_BslmaAllocator*,
                                             my_StlAllocator<void> >::value));
        ASSERT(true  == (bsl::is_convertible<my_BslmaAllocator*,
                                             my_StlAllocator<void*> >::value));
        ASSERT(true  == (bsl::is_convertible<my_BslmaAllocator*,
                                           my_StlAllocator<my_Enum> >::value));
        ASSERT(false == (bsl::is_convertible<void*,
                                           my_StlAllocator<my_Enum> >::value));
        ASSERT(true  == (bsl::is_convertible<my_BslmaAllocator*,
                                            my_StlAllocator<my_EnumClass::Type>
                                            >::value));
        ASSERT(false == (bsl::is_convertible<void*,
                                            my_StlAllocator<my_EnumClass::Type>
                                            >::value));
        ASSERT(true  == (bsl::is_convertible<void*, my_PlacementNew>::value));


        // C-8: Test conversion between 'void' type and other types.

        ASSERT(true  == (bsl::is_convertible<void, void>::value));
        ASSERT(false == (bsl::is_convertible<void, int >::value));
        ASSERT(false == (bsl::is_convertible<void, int&>::value));
        ASSERT(false == (bsl::is_convertible<void, int*>::value));

        ASSERT(true  == (bsl::is_convertible<const void, void>::value));
        ASSERT(false == (bsl::is_convertible<const void, int >::value));
        ASSERT(false == (bsl::is_convertible<const void, int&>::value));
        ASSERT(false == (bsl::is_convertible<const void, int*>::value));

        ASSERT(true  == (bsl::is_convertible<volatile void, void>::value));
        ASSERT(false == (bsl::is_convertible<volatile void, int >::value));
        ASSERT(false == (bsl::is_convertible<volatile void, int&>::value));
        ASSERT(false == (bsl::is_convertible<volatile void, int*>::value));

        ASSERT(true  == (bsl::is_convertible<const volatile void, void>::value));
        ASSERT(false == (bsl::is_convertible<const volatile void, int >::value));
        ASSERT(false == (bsl::is_convertible<const volatile void, int&>::value));
        ASSERT(false == (bsl::is_convertible<const volatile void, int*>::value));

        ASSERT(false == (bsl::is_convertible<int,  void>::value));
        ASSERT(false == (bsl::is_convertible<int,  const void>::value));
        ASSERT(false == (bsl::is_convertible<int,  volatile void>::value));
        ASSERT(false == (bsl::is_convertible<int,  const volatile void>::value));

        ASSERT(false == (bsl::is_convertible<int&, void>::value));
        ASSERT(false == (bsl::is_convertible<int&, const void>::value));
        ASSERT(false == (bsl::is_convertible<int&, volatile void>::value));
        ASSERT(false == (bsl::is_convertible<int&, const volatile void>::value));

        ASSERT(false == (bsl::is_convertible<int*, void>::value));
        ASSERT(false == (bsl::is_convertible<int*, const void>::value));
        ASSERT(false == (bsl::is_convertible<int*, volatile void>::value));
        ASSERT(false == (bsl::is_convertible<int*, const volatile void>::value));

        ASSERT(true == (bsl::is_convertible<const void, const void>::value));
        ASSERT(true == (bsl::is_convertible<const void, volatile void>::value));
        ASSERT(true == (bsl::is_convertible<const void, const volatile void>::value));

        ASSERT(true == (bsl::is_convertible<volatile void, const void>::value));
        ASSERT(true == (bsl::is_convertible<volatile void, volatile void>::value));
        ASSERT(true == (bsl::is_convertible<volatile void, const volatile void>::value));

        ASSERT(true == (bsl::is_convertible<const volatile void, const void>::value));
        ASSERT(true == (bsl::is_convertible<const volatile void, volatile void>::value));
        ASSERT(true == (bsl::is_convertible<const volatile void, const volatile void>::value));

        // C-9: Test derived-to-base convertibility

        ASSERT(NOT_NATIVE == (bsl::is_convertible<my_DerivedClass,
                                             my_AbstractClass>::value));
        ASSERT(false == (bsl::is_convertible<my_AbstractClass,
                                             my_DerivedClass>::value));
        ASSERT(true  == (bsl::is_convertible<my_DerivedClass&,
                                             my_AbstractClass&>::value));
        ASSERT(false == (bsl::is_convertible<my_AbstractClass&,
                                             my_DerivedClass&>::value));
        ASSERT(true  == (bsl::is_convertible<my_DerivedClass*,
                                             my_AbstractClass*>::value));
        ASSERT(false == (bsl::is_convertible<my_AbstractClass*,
                                             my_DerivedClass*>::value));

        // C-10: Test pointer-to-member variable convertibility

        ASSERT(true  == (bsl::is_convertible<int my_AbstractClass::*,
                                 int  my_DerivedClass::*>::value));
        ASSERT(false == (bsl::is_convertible<int my_DerivedClass::*,
                                 int  my_AbstractClass::*>::value));
        ASSERT(false == (bsl::is_convertible<int my_AbstractClass::*,
                                             void*>::value));
        ASSERT(false == (bsl::is_convertible<void*,
                                 int  my_AbstractClass::*>::value));

        // C-11: Test pointer-to-member function convertibility

        ASSERT(true  == (bsl::is_convertible<int (my_AbstractClass::*)(int),
                                 int  (my_DerivedClass::*)(int)
                                             >::value));
        ASSERT(false == (bsl::is_convertible<int (my_DerivedClass::*)(int),
                                 int  (my_AbstractClass::*)(int)
                                             >::value));
        ASSERT(false == (bsl::is_convertible<int (my_AbstractClass::*)(int),
                                             void*>::value));
        ASSERT(false == (bsl::is_convertible<void*,
                                 int  (my_AbstractClass::*)(int)
                                             >::value));

        // C-12: Test arrays of unknown bound convertibility

#ifndef BSLMF_ISCONVERTIBLE_NO_ARRAY_OF_UNKNOWN_BOUND_AS_TEMPLATE_PARAMETER
        ASSERT(true  == (bsl::is_convertible<int    [],    int  *   >::value));
        ASSERT(false == (bsl::is_convertible<int    [][5], int  *   >::value));
        ASSERT(false == (bsl::is_convertible<int (*)[],    int  *   >::value));
        ASSERT(false == (bsl::is_convertible<int (*)[][5], int  *   >::value));
# if !defined(BSLMF_ISCONVERTIBLE_NO_ARRAY_REF_OF_UNKNOWN_BOUND)
        // If the test call cannot be parsed, there is no concern for whether
        // the component diagnoses the trait correctly - it cannot be called.
        ASSERT(true  == (bsl::is_convertible<int (&)[],    int  *   >::value));
        ASSERT(false == (bsl::is_convertible<int (&)[][5], int  *   >::value));
        ASSERT(false == (bsl::is_convertible<int  *,       int (&)[]>::value));
# endif // BSLMF_ISCONVERTIBLE_NO_ARRAY_REF_OF_UNKNOWN_BOUND
        ASSERT(false == (bsl::is_convertible<int  *,       int (*)[]>::value));

        ASSERT(false  ==
                     (bsl::is_convertible<int [],    int (*)[]   >::value));
        ASSERT(false  ==
                     (bsl::is_convertible<int [][5], int (*)[][5]>::value));
        ASSERT(false  ==
         (bsl::is_convertible<         int [], V_(int) (*)[]>::value));
        ASSERT(false ==
         (bsl::is_convertible<V_(int) [],          int (*)[]>::value));
        ASSERT(false  ==
         (bsl::is_convertible<         int [], const    int (*)[]>::value));
        ASSERT(false ==
         (bsl::is_convertible<const    int [],          int (*)[]>::value));
        ASSERT(false ==
         (bsl::is_convertible<const    int [], V_(int) (*)[]>::value));
        ASSERT(false ==
         (bsl::is_convertible<V_(int) [], const    int (*)[]>::value));

        ASSERT(true  ==
                     (bsl::is_convertible<int (*)[],    int (*)[]   >::value));
        ASSERT(true  ==
                     (bsl::is_convertible<int (*)[][5], int (*)[][5]>::value));
        ASSERT(true  ==
         (bsl::is_convertible<         int (*)[], V_(int) (*)[]>::value));
        ASSERT(false ==
         (bsl::is_convertible<V_(int) (*)[],          int (*)[]>::value));
        ASSERT(true  ==
         (bsl::is_convertible<         int (*)[], const    int (*)[]>::value));
        ASSERT(false ==
         (bsl::is_convertible<const    int (*)[],          int (*)[]>::value));
        ASSERT(false ==
         (bsl::is_convertible<const    int (*)[], V_(int) (*)[]>::value));
        ASSERT(false ==
         (bsl::is_convertible<V_(int) (*)[], const    int (*)[]>::value));

# if !defined(BSLMF_ISCONVERTIBLE_NO_ARRAY_REF_OF_UNKNOWN_BOUND)
        // If the test call cannot be parsed, there is no concern for whether
        // the component diagnoses the trait correctly - it cannot be called.
        ASSERT(false ==
                     (bsl::is_convertible<int [],    int (&)[]   >::value));
        ASSERT(false ==
                     (bsl::is_convertible<int [][5], int (&)[][5]>::value));
        ASSERT(false ==
         (bsl::is_convertible<         int [], V_(int) (&)[]>::value));
        ASSERT(false ==
         (bsl::is_convertible<V_(int) [],          int (&)[]>::value));
        ASSERT(true  ==
         (bsl::is_convertible<         int [], const    int (&)[]>::value));
        ASSERT(false ==
         (bsl::is_convertible<const    int [],          int (&)[]>::value));
        ASSERT(false ==
         (bsl::is_convertible<const    int [], V_(int) (&)[]>::value));
        ASSERT(false ==
         (bsl::is_convertible<V_(int) [], const    int (&)[]>::value));

        ASSERT(false  ==
                     (bsl::is_convertible<int (*)[],    int (&)[]   >::value));
        ASSERT(false  ==
                     (bsl::is_convertible<int (*)[][5], int (&)[][5]>::value));
        ASSERT(false  ==
         (bsl::is_convertible<         int (*)[], V_(int) (&)[]>::value));
        ASSERT(false ==
         (bsl::is_convertible<V_(int) (*)[],          int (&)[]>::value));
        ASSERT(false  ==
         (bsl::is_convertible<         int (*)[], const    int (&)[]>::value));
        ASSERT(false ==
         (bsl::is_convertible<const    int (*)[],          int (&)[]>::value));
        ASSERT(false ==
         (bsl::is_convertible<const    int (*)[], V_(int) (&)[]>::value));
        ASSERT(false ==
         (bsl::is_convertible<V_(int) (*)[], const    int (&)[]>::value));

        ASSERT(false  ==
                     (bsl::is_convertible<int (&)[],    int (*)[]   >::value));
        ASSERT(false  ==
                     (bsl::is_convertible<int (&)[][5], int (*)[][5]>::value));
        ASSERT(false  ==
         (bsl::is_convertible<         int (&)[], V_(int) (*)[]>::value));
        ASSERT(false ==
         (bsl::is_convertible<V_(int) (&)[],          int (*)[]>::value));
        ASSERT(false ==
         (bsl::is_convertible<         int (&)[], const    int (*)[]>::value));
        ASSERT(false ==
         (bsl::is_convertible<const    int (&)[],          int (*)[]>::value));
        ASSERT(false ==
         (bsl::is_convertible<const    int (&)[], V_(int) (*)[]>::value));
        ASSERT(false ==
         (bsl::is_convertible<V_(int) (&)[], const    int (*)[]>::value));
# endif // BSLMF_ISCONVERTIBLE_NO_ARRAY_REF_OF_UNKNOWN_BOUND
#endif  // BSLMF_ISCONVERTIBLE_NO_ARRAY_OF_UNKNOWN_BOUND_AS_TEMPLATE_PARAMETER

        // C-13: Test function references decay to function pointers

        ASSERT(false == (bsl::is_convertible<void(), void>::VALUE));
        ASSERT(false == (bsl::is_convertible<void(), int>::VALUE));
        ASSERT(false == (bsl::is_convertible<void(), const void>::VALUE));
        ASSERT(false == (bsl::is_convertible<void(), V_(int)>::VALUE));
        ASSERT(false == (bsl::is_convertible<void,         void()>::VALUE));
        ASSERT(false == (bsl::is_convertible<int,          void()>::VALUE));
        ASSERT(false == (bsl::is_convertible<const void,   void()>::VALUE));
        ASSERT(false == (bsl::is_convertible<V_(int), void()>::VALUE));

        ASSERT(false == (bsl::is_convertible<void    (), void    ()>::value));
        ASSERT(true  == (bsl::is_convertible<void    (), void (&)()>::value));
        ASSERT(true  == (bsl::is_convertible<void    (), void (*)()>::value));
        ASSERT(true  == (bsl::is_convertible<void (&)(), void (&)()>::value));
        ASSERT(true  == (bsl::is_convertible<void (&)(), void (*)()>::value));
        ASSERT(false == (bsl::is_convertible<void (*)(), void    ()>::value));
        ASSERT(false == (bsl::is_convertible<void (*)(), void (&)()>::value));
        ASSERT(false == (bsl::is_convertible<void (&)(), void    ()>::value));
        ASSERT(true  == (bsl::is_convertible<void (*)(), void (*)()>::value));

        ASSERT(false == (bsl::is_convertible<void(...), void>::VALUE));
        ASSERT(false == (bsl::is_convertible<void(...), int>::VALUE));
        ASSERT(false == (bsl::is_convertible<void(...), const void>::VALUE));
        ASSERT(false == (bsl::is_convertible<void(...), V_(int)>::VALUE));
        ASSERT(false == (bsl::is_convertible<void,         void(...)>::VALUE));
        ASSERT(false == (bsl::is_convertible<int,          void(...)>::VALUE));
        ASSERT(false == (bsl::is_convertible<const void,   void(...)>::VALUE));
        ASSERT(false == (bsl::is_convertible<V_(int), void(...)>::VALUE));

        ASSERT(false  ==
                   (bsl::is_convertible<void    (...), void    (...)>::value));
        ASSERT(true  ==
                   (bsl::is_convertible<void    (...), void (&)(...)>::value));
        ASSERT(true  ==
                   (bsl::is_convertible<void    (...), void (*)(...)>::value));
        ASSERT(true  ==
                   (bsl::is_convertible<void (&)(...), void (&)(...)>::value));
        ASSERT(true  ==
                   (bsl::is_convertible<void (&)(...), void (*)(...)>::value));
        ASSERT(false ==
                   (bsl::is_convertible<void (*)(...), void    (...)>::value));
        ASSERT(false ==
                   (bsl::is_convertible<void (*)(...), void (&)(...)>::value));
        ASSERT(false  ==
                   (bsl::is_convertible<void (&)(...), void    (...)>::value));

        ASSERT(false ==
                      (bsl::is_convertible<void    (...), void    ()>::value));
        ASSERT(false ==
                      (bsl::is_convertible<void    (...), void (&)()>::value));
        ASSERT(false ==
                      (bsl::is_convertible<void    (...), void (*)()>::value));
        ASSERT(false ==
                      (bsl::is_convertible<void (&)(...), void (&)()>::value));
        ASSERT(false ==
                      (bsl::is_convertible<void (&)(...), void (*)()>::value));
        ASSERT(false ==
                      (bsl::is_convertible<void (*)(...), void    ()>::value));
        ASSERT(false ==
                      (bsl::is_convertible<void (*)(...), void (&)()>::value));
        ASSERT(false ==
                      (bsl::is_convertible<void (&)(...), void    ()>::value));

        ASSERT(false ==
                      (bsl::is_convertible<void    (), void    (...)>::value));
        ASSERT(false ==
                      (bsl::is_convertible<void    (), void (&)(...)>::value));
        ASSERT(false ==
                      (bsl::is_convertible<void    (), void (*)(...)>::value));
        ASSERT(false ==
                      (bsl::is_convertible<void (&)(), void (&)(...)>::value));
        ASSERT(false ==
                      (bsl::is_convertible<void (&)(), void (*)(...)>::value));
        ASSERT(false ==
                      (bsl::is_convertible<void (*)(), void    (...)>::value));
        ASSERT(false ==
                      (bsl::is_convertible<void (*)(), void (&)(...)>::value));
        ASSERT(false ==
                      (bsl::is_convertible<void (&)(), void    (...)>::value));

        ASSERT(false ==
                      (bsl::is_convertible<int(char, float...), void>::VALUE));
        ASSERT(false ==
                      (bsl::is_convertible<int(char, float...),  int>::VALUE));
        ASSERT(false ==
                      (bsl::is_convertible<void, int(char, float...)>::VALUE));
        ASSERT(false ==
                      (bsl::is_convertible<int,  int(char, float...)>::VALUE));

        ASSERT(false == (bsl::is_convertible<int   (char, float...),
                                             int   (char, float...)>::VALUE));
        ASSERT(true  == (bsl::is_convertible<int   (char, float...),
                                             int(&)(char, float...)>::VALUE));
        ASSERT(true  == (bsl::is_convertible<int   (char, float...),
                                             int(*)(char, float...)>::VALUE));
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
