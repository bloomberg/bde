// bslmf_isconvertible.t.cpp                                          -*-C++-*-
#include <bslmf_isconvertible.h>

#include <bsls_bsltestutil.h>
#include <bsls_compilerfeatures.h>
#include <bsls_platform.h>

#include <stdio.h>      // 'printf'
#include <stdlib.h>     // 'atoi'
#include <string.h>     // 'strcmp'

#if defined(BSLS_PLATFORM_CMP_SUN)
# pragma error_messages(off, functypequal, refarray0)
#endif

using namespace BloombergLP;

//=============================================================================
//                                TEST PLAN
//-----------------------------------------------------------------------------
//                                Overview
//                                --------
// The component under test defines two meta-functions, 'bsl::is_convertible'
// and 'bslmf::IsConvertible' and a template variable 'bsl::is_convertible_v',
// that determine whether a conversion exists from one template parameter type
// to the other template parameter type.  Thus, we need to ensure that the
// values returned by these meta-functions are correct for each possible pair
// of categorized types.  The two meta-functions are functionally equivalent
// except 'bsl::is_convertible' only allows complete template parameter types.
// We will use the same set of complete types for 'bslmf::IsConvertible' as
// that for 'bsl::is_convertible', and an additional set of incomplete types
// for testing 'bslmf::IsConvertible' alone.
//
//-----------------------------------------------------------------------------
// PUBLIC CLASS DATA
// [ 1] bsl::is_convertible::value
// [ 1] bsl::is_convertible_v
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

#if defined(BSLS_PLATFORM_CMP_IBM)
# define BSLMF_ISCONVERTIBLE_NO_ARRAY_REF_OF_UNKNOWN_BOUND
    // This macro signifies that this compiler rejects 'Type[]' as incomplete,
    // even in contexts where it should be valid, such as where it will pass by
    // reference or pointer.
#endif

#if defined(BSLS_PLATFORM_CMP_IBM)
# define BSLMF_ISCONVERTIBLE_NO_ARRAY_OF_UNKNOWN_BOUND_AS_TEMPLATE_PARAMETER
    // The IBM compiler has a bigger problem, where it rejects arrays of
    // unknown bound as template type-parameter arguments.
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

#define  _(TYPE) TYPE
#define C_(TYPE) const TYPE
#define V_(TYPE) volatile TYPE
#define CV(TYPE) const volatile TYPE
    // Macros to provide simple cv-qualifier decorations for types, enabling a
    // more table-like presentation of a sequence of test cases.

#if BSLS_PLATFORM_CMP_VERSION >= 1910 && BSLS_PLATFORM_CMP_VERSION <= 1914
# define BSLMF_ISCONVERTIBLE_MSVC_VOLATILE_BUG
    // Microsoft Visual Studio 2017 has a bug in its 'is_convertible'
    // implementation when it is used with 'volatile' qualified non-fundamental
    // types.  This macro is defined when that bug is present.
#endif

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

// Support types to demonstrate a convertible bug with Oracle CC 12.4.  The
// member function are declared, but never defined, as we are testing only
// compile-time properties associated with conversions.

#if !defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
template <class TYPE>
struct Mover {
    // Forwarding emulation tool, that acts like a movable (rvalue) reference
    // in C++03.

    operator TYPE&() const;
};
#else
template <class TYPE>
using Mover = TYPE&&;
#endif

struct Movable {
    // This 'struct' is both copyable and movable, in both C++11, and through
    // move-semantic emulation in C++03.

    Movable();
    Movable(const Movable&);
    Movable(Mover<Movable>); // implicit
};

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

    setbuf(stdout, NULL);       // Use unbuffered output

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
        //: 1 Instantiate 'bsl::is_convertible' with combinations of a
        //:   user-defined class that is convertible to a fundamental type,
        //:   'T1', and another fundamental type, 'T2', to which 'T1' is
        //:   implicitly convertible.  Verify that the 'value' member is
        //:   initialized properly, and (manually) verify that no warning is
        //:   generated for conversions between floating-point types and
        //:   integral types.  For each combination of 'T1', use three
        //:   different user-defined classes: one that provides conversion to
        //:   an object of type 'T1', one that provides conversion to a
        //:   reference to 'T1' and one that provides conversion to a const
        //:   reference to 'T1'.  (C-1)
        //
        // Testing:
        //   CONCERN: Warning-free on user-defined conversions
        // --------------------------------------------------------------------

        if (verbose)
            printf(
                "TESTING CONCERN: WARNING-FREE ON USER-DEFINED CONVERSIONS\n"
                "=========================================================\n");

#define ASSERT_IS_CONVERTIBLE(RESULT, FROM_TYPE, TO_TYPE)                     \
        ASSERT(RESULT == (bsl::is_convertible  <FROM_TYPE, TO_TYPE>::value))

        // Providing conversions from 'T' to 'T':

        // Conversion of basic types via two user-defined classes returns
        // 'false'.

        ASSERT_IS_CONVERTIBLE(false, ConvertibleFrom<int>,
                                     ConvertibleToObj<int> );

        // Test conversion of basic types via a user-defined class.

        ASSERT_IS_CONVERTIBLE(true,  ConvertibleToObj<int  >, float);
        ASSERT_IS_CONVERTIBLE(true,  ConvertibleToObj<float>, int  );
        ASSERT_IS_CONVERTIBLE(true,  int,   ConvertibleFrom<float> );
        ASSERT_IS_CONVERTIBLE(true,  float, ConvertibleFrom<int  > );

        // Test 'const' type conversions via a user-defined class.

        ASSERT_IS_CONVERTIBLE(true,  ConvertibleToObj<   int   >, C_(float));
        ASSERT_IS_CONVERTIBLE(true,  ConvertibleToObj<   float >, C_(int)  );
        ASSERT_IS_CONVERTIBLE(true,  ConvertibleToObj<C_(int)  >,    float );
        ASSERT_IS_CONVERTIBLE(true,  ConvertibleToObj<C_(float)>,    int   );
        ASSERT_IS_CONVERTIBLE(true,     int,    ConvertibleFrom<C_(float)> );
        ASSERT_IS_CONVERTIBLE(true,     float,  ConvertibleFrom<C_(int)  > );
        ASSERT_IS_CONVERTIBLE(true,  C_(int),   ConvertibleFrom<   float > );
        ASSERT_IS_CONVERTIBLE(true,  C_(float), ConvertibleFrom<   int   > );

        // Test 'volatile' type conversions via a user-defined class.

        ASSERT_IS_CONVERTIBLE(true,  ConvertibleToObj<   int   >, V_(float));
        ASSERT_IS_CONVERTIBLE(true,  ConvertibleToObj<   float >, V_(int)  );
        ASSERT_IS_CONVERTIBLE(true,  ConvertibleToObj<V_(int)  >,    float );
        ASSERT_IS_CONVERTIBLE(true,  ConvertibleToObj<V_(float)>,    int   );
        ASSERT_IS_CONVERTIBLE(true,     int,    ConvertibleFrom<V_(float)> );
        ASSERT_IS_CONVERTIBLE(true,     float,  ConvertibleFrom<V_(int)  > );
        ASSERT_IS_CONVERTIBLE(true,  V_(int),   ConvertibleFrom<   float > );
        ASSERT_IS_CONVERTIBLE(true,  V_(float), ConvertibleFrom<   int   > );

        // Providing conversions from 'T' to 'T&':

        // // Conversion of basic types via two user-defined classes returns
        // 'false'.

        ASSERT_IS_CONVERTIBLE(false, ConvertibleFrom<int>,
                                     ConvertibleToRef<int> );

        // Test conversion of basic types via a user-defined class.

        ASSERT_IS_CONVERTIBLE(true,  ConvertibleToRef<int  >, float);
        ASSERT_IS_CONVERTIBLE(true,  ConvertibleToRef<float>, int  );
        ASSERT_IS_CONVERTIBLE(true,  int,   ConvertibleFrom<float> );
        ASSERT_IS_CONVERTIBLE(true,  float, ConvertibleFrom<int  > );

        // Test 'const' type conversions via a user-defined class.

        ASSERT_IS_CONVERTIBLE(true,  ConvertibleToRef<   int   >, C_(float));
        ASSERT_IS_CONVERTIBLE(true,  ConvertibleToRef<   float >, C_(int  ));
        ASSERT_IS_CONVERTIBLE(true,  ConvertibleToRef<C_(int  )>,    float );
        ASSERT_IS_CONVERTIBLE(true,  ConvertibleToRef<C_(float)>,    int   );
        ASSERT_IS_CONVERTIBLE(true,     int,    ConvertibleFrom<C_(float)> );
        ASSERT_IS_CONVERTIBLE(true,     float,  ConvertibleFrom<C_(int  )> );
        ASSERT_IS_CONVERTIBLE(true,  C_(int),   ConvertibleFrom<   float > );
        ASSERT_IS_CONVERTIBLE(true,  C_(float), ConvertibleFrom<   int   > );

        // Test 'volatile' type conversions via a user-defined class.

        ASSERT_IS_CONVERTIBLE(true,  ConvertibleToRef<   int   >, V_(float));
        ASSERT_IS_CONVERTIBLE(true,  ConvertibleToRef<   float >, V_(int  ));
        ASSERT_IS_CONVERTIBLE(true,  ConvertibleToRef<V_(int  )>,    float );
        ASSERT_IS_CONVERTIBLE(true,  ConvertibleToRef<V_(float)>,    int   );
        ASSERT_IS_CONVERTIBLE(true,     int,    ConvertibleFrom<V_(float)> );
        ASSERT_IS_CONVERTIBLE(true,     float,  ConvertibleFrom<V_(int  )> );
        ASSERT_IS_CONVERTIBLE(true,  V_(int),   ConvertibleFrom<   float > );
        ASSERT_IS_CONVERTIBLE(true,  V_(float), ConvertibleFrom<   int   > );

        // Providing conversions from 'T' to 'const T&':

        // Conversion of basic types via two user-defined classes returns
        // 'false'.

        ASSERT_IS_CONVERTIBLE(false, ConvertibleFrom<int>,
                                     ConvertibleToConstRef<int> );

        // Test conversion of basic types via a user-defined class.

        ASSERT_IS_CONVERTIBLE(true, ConvertibleToConstRef<int  >, float);
        ASSERT_IS_CONVERTIBLE(true, ConvertibleToConstRef<float>, int  );
        ASSERT_IS_CONVERTIBLE(true, int,   ConvertibleFrom<float> );
        ASSERT_IS_CONVERTIBLE(true, float, ConvertibleFrom<int  > );

        // Test 'const' type conversions via a user-defined class.

        ASSERT_IS_CONVERTIBLE(true, ConvertibleToConstRef<int  >, C_(float));
        ASSERT_IS_CONVERTIBLE(true, ConvertibleToConstRef<float>, C_(int  ));
        ASSERT_IS_CONVERTIBLE(true, ConvertibleToConstRef<C_(int  )>, float);
        ASSERT_IS_CONVERTIBLE(true, ConvertibleToConstRef<C_(float)>, int  );
        ASSERT_IS_CONVERTIBLE(true,    int,    ConvertibleFrom<C_(float)>  );
        ASSERT_IS_CONVERTIBLE(true,    float,  ConvertibleFrom<C_(int  )>  );
        ASSERT_IS_CONVERTIBLE(true, C_(int  ), ConvertibleFrom<   float >  );
        ASSERT_IS_CONVERTIBLE(true, C_(float), ConvertibleFrom<   int   >  );

        // Test 'volatile' type conversions via a user-defined class.

        ASSERT_IS_CONVERTIBLE(true, ConvertibleToConstRef<int  >, V_(float));
        ASSERT_IS_CONVERTIBLE(true, ConvertibleToConstRef<float>, V_(int  ));
        ASSERT_IS_CONVERTIBLE(true, ConvertibleToConstRef<V_(int  )>, float);
        ASSERT_IS_CONVERTIBLE(true, ConvertibleToConstRef<V_(float)>, int  );
        ASSERT_IS_CONVERTIBLE(true,    int,    ConvertibleFrom<V_(float)>  );
        ASSERT_IS_CONVERTIBLE(true,    float,  ConvertibleFrom<V_(int  )>  );
        ASSERT_IS_CONVERTIBLE(true, V_(int  ), ConvertibleFrom<   float >  );
        ASSERT_IS_CONVERTIBLE(true, V_(float), ConvertibleFrom<   int   >  );

        // Test implicit conversion of user-defined movable types.

        ASSERT_IS_CONVERTIBLE(true, Movable, Movable);
        ASSERT_IS_CONVERTIBLE(true, const Movable, Movable);
        ASSERT_IS_CONVERTIBLE(true, Mover<Movable>, Movable);
        ASSERT_IS_CONVERTIBLE(true, const Mover<Movable>, Movable);
        ASSERT_IS_CONVERTIBLE(true, Movable, const Movable);
        ASSERT_IS_CONVERTIBLE(true, const Movable, const Movable);
        ASSERT_IS_CONVERTIBLE(true, Mover<Movable>, const Movable);
        ASSERT_IS_CONVERTIBLE(true, const Mover<Movable>, const Movable);

#undef ASSERT_IS_CONVERTIBLE
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

#define ASSERT_IS_CONVERTIBLE(RESULT, FROM_TYPE, TO_TYPE) \
        ASSERT(RESULT  == (bsl::is_convertible<FROM_TYPE, TO_TYPE>()))

        // Test conversion of basic types.

        ASSERT_IS_CONVERTIBLE(true, int,   float);
        ASSERT_IS_CONVERTIBLE(true, float, int  );

        // Test 'const' type conversions.

        ASSERT_IS_CONVERTIBLE(true,    int   , C_(float));
        ASSERT_IS_CONVERTIBLE(true,    float , C_(int  ));
        ASSERT_IS_CONVERTIBLE(true, C_(float),    int   );
        ASSERT_IS_CONVERTIBLE(true, C_(int  ),    float );

        // Test 'volatile' type conversions.

        ASSERT_IS_CONVERTIBLE(true,    int,    V_(float));
        ASSERT_IS_CONVERTIBLE(true,    float,  V_(int)  );
        ASSERT_IS_CONVERTIBLE(true, V_(int  ),    float );
        ASSERT_IS_CONVERTIBLE(true, V_(float),    int   );

        // Test 'volatile' pointer and reference conversions from integral to
        // floating-point.

        ASSERT_IS_CONVERTIBLE(false,    int *, V_(float)*);
        ASSERT_IS_CONVERTIBLE(false, V_(int)*,    float *);
        ASSERT_IS_CONVERTIBLE(false,    int &, V_(float)&);
        ASSERT_IS_CONVERTIBLE(false, V_(int)&,    float &);
        ASSERT_IS_CONVERTIBLE(false,    int  , V_(float)&);
        ASSERT_IS_CONVERTIBLE(false, V_(int) ,    float &);
        ASSERT_IS_CONVERTIBLE(true,     int &, V_(float) );
        ASSERT_IS_CONVERTIBLE(true,  V_(int)&,    float  );

        ASSERT_IS_CONVERTIBLE(false,    int *, CV(float)*);
        ASSERT_IS_CONVERTIBLE(false, CV(int)*,    float *);
        ASSERT_IS_CONVERTIBLE(false,    int &, CV(float)&);
        ASSERT_IS_CONVERTIBLE(false, CV(int)&,    float &);
        ASSERT_IS_CONVERTIBLE(false,    int  , CV(float)&);
        ASSERT_IS_CONVERTIBLE(false, CV(int) ,    float &);
        ASSERT_IS_CONVERTIBLE(true,     int &, CV(float) );
        ASSERT_IS_CONVERTIBLE(true,  CV(int)&,    float  );

        ASSERT_IS_CONVERTIBLE(false, C_(int)*, V_(float)*);
        ASSERT_IS_CONVERTIBLE(false, V_(int)*, C_(float)*);
        ASSERT_IS_CONVERTIBLE(true,  C_(int)&, V_(float) );
        ASSERT_IS_CONVERTIBLE(true,  V_(int)&, C_(float) );
        ASSERT_IS_CONVERTIBLE(false, C_(int)&, CV(float)&);
        ASSERT_IS_CONVERTIBLE(false, V_(int)&, CV(float)&);
        ASSERT_IS_CONVERTIBLE(false, C_(int) , CV(float)&);
        ASSERT_IS_CONVERTIBLE(false, V_(int) , CV(float)&);
        ASSERT_IS_CONVERTIBLE(false, C_(int) , V_(float)&);
        ASSERT_IS_CONVERTIBLE(false, CV(int) , V_(float)&);

        // Test volatile pointer and reference conversions from floating-point
        // to integral.

        ASSERT_IS_CONVERTIBLE(true,     float  ,    int  );
        ASSERT_IS_CONVERTIBLE(true,     float  , C_(int) );
        ASSERT_IS_CONVERTIBLE(true,     float  , V_(int) );
        ASSERT_IS_CONVERTIBLE(true,     float  , CV(int) );

        ASSERT_IS_CONVERTIBLE(true,  C_(float) ,    int  );
        ASSERT_IS_CONVERTIBLE(true,  C_(float) , C_(int) );
        ASSERT_IS_CONVERTIBLE(true,  C_(float) , V_(int) );
        ASSERT_IS_CONVERTIBLE(true,  C_(float) , CV(int) );

        ASSERT_IS_CONVERTIBLE(true,  V_(float) ,    int  );
        ASSERT_IS_CONVERTIBLE(true,  V_(float) , C_(int) );
        ASSERT_IS_CONVERTIBLE(true,  V_(float) , V_(int) );
        ASSERT_IS_CONVERTIBLE(true,  V_(float) , CV(int) );

        ASSERT_IS_CONVERTIBLE(true,  CV(float) ,    int  );
        ASSERT_IS_CONVERTIBLE(true,  CV(float) , C_(int) );
        ASSERT_IS_CONVERTIBLE(true,  CV(float) , V_(int) );
        ASSERT_IS_CONVERTIBLE(true,  CV(float) , CV(int) );

        ASSERT_IS_CONVERTIBLE(false,    float  ,    int &);
        ASSERT_IS_CONVERTIBLE(true,     float  , C_(int)&);
        ASSERT_IS_CONVERTIBLE(false,    float  , V_(int)&);
        ASSERT_IS_CONVERTIBLE(false,    float  , CV(int)&);

        ASSERT_IS_CONVERTIBLE(false, C_(float) ,    int &);
        ASSERT_IS_CONVERTIBLE(true,  C_(float) , C_(int)&);
        ASSERT_IS_CONVERTIBLE(false, C_(float) , V_(int)&);
        ASSERT_IS_CONVERTIBLE(false, C_(float) , CV(int)&);

        ASSERT_IS_CONVERTIBLE(false, V_(float) ,    int &);
        ASSERT_IS_CONVERTIBLE(true,  V_(float) , C_(int)&);
        ASSERT_IS_CONVERTIBLE(false, V_(float) , V_(int)&);
        ASSERT_IS_CONVERTIBLE(false, V_(float) , CV(int)&);

        ASSERT_IS_CONVERTIBLE(false, CV(float) ,    int &);
        ASSERT_IS_CONVERTIBLE(true,  CV(float) , C_(int)&);
        ASSERT_IS_CONVERTIBLE(false, CV(float) , V_(int)&);
        ASSERT_IS_CONVERTIBLE(false, CV(float) , CV(int)&);


        ASSERT_IS_CONVERTIBLE(true,     float &,    int  );
        ASSERT_IS_CONVERTIBLE(true,     float &, C_(int) );
        ASSERT_IS_CONVERTIBLE(true,     float &, V_(int) );
        ASSERT_IS_CONVERTIBLE(true,     float &, CV(int) );

        ASSERT_IS_CONVERTIBLE(true,  C_(float)&,    int  );
        ASSERT_IS_CONVERTIBLE(true,  C_(float)&, C_(int) );
        ASSERT_IS_CONVERTIBLE(true,  C_(float)&, V_(int) );
        ASSERT_IS_CONVERTIBLE(true,  C_(float)&, CV(int) );

        ASSERT_IS_CONVERTIBLE(true,  V_(float)&,    int  );
        ASSERT_IS_CONVERTIBLE(true,  V_(float)&, C_(int) );
        ASSERT_IS_CONVERTIBLE(true,  V_(float)&, V_(int) );
        ASSERT_IS_CONVERTIBLE(true,  V_(float)&, CV(int) );

        ASSERT_IS_CONVERTIBLE(true,  CV(float)&,    int  );
        ASSERT_IS_CONVERTIBLE(true,  CV(float)&, C_(int) );
        ASSERT_IS_CONVERTIBLE(true,  CV(float)&, V_(int) );
        ASSERT_IS_CONVERTIBLE(true,  CV(float)&, CV(int) );

        ASSERT_IS_CONVERTIBLE(false,    float &,    int &);
        ASSERT_IS_CONVERTIBLE(true,     float &, C_(int)&);
        ASSERT_IS_CONVERTIBLE(false,    float &, V_(int)&);
        ASSERT_IS_CONVERTIBLE(false,    float &, CV(int)&);

        ASSERT_IS_CONVERTIBLE(false, C_(float)&,    int &);
        ASSERT_IS_CONVERTIBLE(true,  C_(float)&, C_(int)&);
        ASSERT_IS_CONVERTIBLE(false, C_(float)&, V_(int)&);
        ASSERT_IS_CONVERTIBLE(false, C_(float)&, CV(int)&);

        ASSERT_IS_CONVERTIBLE(false, V_(float)&,    int &);
        ASSERT_IS_CONVERTIBLE(true,  V_(float)&, C_(int)&);
        ASSERT_IS_CONVERTIBLE(false, V_(float)&, V_(int)&);
        ASSERT_IS_CONVERTIBLE(false, V_(float)&, CV(int)&);

        ASSERT_IS_CONVERTIBLE(false, CV(float)&,    int &);
        ASSERT_IS_CONVERTIBLE(true,  CV(float)&, C_(int)&);
        ASSERT_IS_CONVERTIBLE(false, CV(float)&, V_(int)&);
        ASSERT_IS_CONVERTIBLE(false, CV(float)&, CV(int)&);


        // Test volatile reference conversions from pointer-to-fundamental to
        // pointer-to-different-fundamental type.

        ASSERT_IS_CONVERTIBLE(false,    float*  ,    int*  );
        ASSERT_IS_CONVERTIBLE(false,    float*  , C_(int*) );
        ASSERT_IS_CONVERTIBLE(false,    float*  , V_(int*) );
        ASSERT_IS_CONVERTIBLE(false,    float*  , CV(int*) );

        ASSERT_IS_CONVERTIBLE(false, C_(float*) ,    int*  );
        ASSERT_IS_CONVERTIBLE(false, C_(float*) , C_(int*) );
        ASSERT_IS_CONVERTIBLE(false, C_(float*) , V_(int*) );
        ASSERT_IS_CONVERTIBLE(false, C_(float*) , CV(int*) );

        ASSERT_IS_CONVERTIBLE(false, V_(float*) ,    int*  );
        ASSERT_IS_CONVERTIBLE(false, V_(float*) , C_(int*) );
        ASSERT_IS_CONVERTIBLE(false, V_(float*) , V_(int*) );
        ASSERT_IS_CONVERTIBLE(false, V_(float*) , CV(int*) );

        ASSERT_IS_CONVERTIBLE(false, CV(float*) ,    int*  );
        ASSERT_IS_CONVERTIBLE(false, CV(float*) , C_(int*) );
        ASSERT_IS_CONVERTIBLE(false, CV(float*) , V_(int*) );
        ASSERT_IS_CONVERTIBLE(false, CV(float*) , CV(int*) );

        ASSERT_IS_CONVERTIBLE(false,    float*  ,    int* &);
        ASSERT_IS_CONVERTIBLE(false,    float*  , C_(int*)&);
        ASSERT_IS_CONVERTIBLE(false,    float*  , V_(int*)&);
        ASSERT_IS_CONVERTIBLE(false,    float*  , CV(int*)&);

        ASSERT_IS_CONVERTIBLE(false, C_(float*) ,    int* &);
        ASSERT_IS_CONVERTIBLE(false, C_(float*) , C_(int*)&);
        ASSERT_IS_CONVERTIBLE(false, C_(float*) , V_(int*)&);
        ASSERT_IS_CONVERTIBLE(false, C_(float*) , CV(int*)&);

        ASSERT_IS_CONVERTIBLE(false, V_(float*) ,    int* &);
        ASSERT_IS_CONVERTIBLE(false, V_(float*) , C_(int*)&);
        ASSERT_IS_CONVERTIBLE(false, V_(float*) , V_(int*)&);
        ASSERT_IS_CONVERTIBLE(false, V_(float*) , CV(int*)&);

        ASSERT_IS_CONVERTIBLE(false, CV(float*) ,    int* &);
        ASSERT_IS_CONVERTIBLE(false, CV(float*) , C_(int*)&);
        ASSERT_IS_CONVERTIBLE(false, CV(float*) , V_(int*)&);
        ASSERT_IS_CONVERTIBLE(false, CV(float*) , CV(int*)&);


        ASSERT_IS_CONVERTIBLE(false,    float* &,    int*  );
        ASSERT_IS_CONVERTIBLE(false,    float* &, C_(int*) );
        ASSERT_IS_CONVERTIBLE(false,    float* &, V_(int*) );
        ASSERT_IS_CONVERTIBLE(false,    float* &, CV(int*) );

        ASSERT_IS_CONVERTIBLE(false, C_(float*)&,    int*  );
        ASSERT_IS_CONVERTIBLE(false, C_(float*)&, C_(int*) );
        ASSERT_IS_CONVERTIBLE(false, C_(float*)&, V_(int*) );
        ASSERT_IS_CONVERTIBLE(false, C_(float*)&, CV(int*) );

        ASSERT_IS_CONVERTIBLE(false, V_(float*)&,    int*  );
        ASSERT_IS_CONVERTIBLE(false, V_(float*)&, C_(int*) );
        ASSERT_IS_CONVERTIBLE(false, V_(float*)&, V_(int*) );
        ASSERT_IS_CONVERTIBLE(false, V_(float*)&, CV(int*) );

        ASSERT_IS_CONVERTIBLE(false, CV(float*)&,    int*  );
        ASSERT_IS_CONVERTIBLE(false, CV(float*)&, C_(int*) );
        ASSERT_IS_CONVERTIBLE(false, CV(float*)&, V_(int*) );
        ASSERT_IS_CONVERTIBLE(false, CV(float*)&, CV(int*) );

        ASSERT_IS_CONVERTIBLE(false,    float* &,    int* &);
        ASSERT_IS_CONVERTIBLE(false,    float* &, C_(int*)&);
        ASSERT_IS_CONVERTIBLE(false,    float* &, V_(int*)&);
        ASSERT_IS_CONVERTIBLE(false,    float* &, CV(int*)&);

        ASSERT_IS_CONVERTIBLE(false, C_(float*)&,    int* &);
        ASSERT_IS_CONVERTIBLE(false, C_(float*)&, C_(int*)&);
        ASSERT_IS_CONVERTIBLE(false, C_(float*)&, V_(int*)&);
        ASSERT_IS_CONVERTIBLE(false, C_(float*)&, CV(int*)&);

        ASSERT_IS_CONVERTIBLE(false, V_(float*)&,    int* &);
        ASSERT_IS_CONVERTIBLE(false, V_(float*)&, C_(int*)&);
        ASSERT_IS_CONVERTIBLE(false, V_(float*)&, V_(int*)&);
        ASSERT_IS_CONVERTIBLE(false, V_(float*)&, CV(int*)&);

        ASSERT_IS_CONVERTIBLE(false, CV(float*)&,    int* &);
        ASSERT_IS_CONVERTIBLE(false, CV(float*)&, C_(int*)&);
        ASSERT_IS_CONVERTIBLE(false, CV(float*)&, V_(int*)&);
        ASSERT_IS_CONVERTIBLE(false, CV(float*)&, CV(int*)&);


        // test conversion from 'void *'

        ASSERT_IS_CONVERTIBLE(false,    void*  ,    int*  );
        ASSERT_IS_CONVERTIBLE(false,    void*  , C_(int*) );
        ASSERT_IS_CONVERTIBLE(false,    void*  , V_(int*) );
        ASSERT_IS_CONVERTIBLE(false,    void*  , CV(int*) );

        ASSERT_IS_CONVERTIBLE(false, C_(void*) ,    int*  );
        ASSERT_IS_CONVERTIBLE(false, C_(void*) , C_(int*) );
        ASSERT_IS_CONVERTIBLE(false, C_(void*) , V_(int*) );
        ASSERT_IS_CONVERTIBLE(false, C_(void*) , CV(int*) );

        ASSERT_IS_CONVERTIBLE(false, V_(void*) ,    int*  );
        ASSERT_IS_CONVERTIBLE(false, V_(void*) , C_(int*) );
        ASSERT_IS_CONVERTIBLE(false, V_(void*) , V_(int*) );
        ASSERT_IS_CONVERTIBLE(false, V_(void*) , CV(int*) );

        ASSERT_IS_CONVERTIBLE(false, CV(void*) ,    int*  );
        ASSERT_IS_CONVERTIBLE(false, CV(void*) , C_(int*) );
        ASSERT_IS_CONVERTIBLE(false, CV(void*) , V_(int*) );
        ASSERT_IS_CONVERTIBLE(false, CV(void*) , CV(int*) );

        ASSERT_IS_CONVERTIBLE(false,    void*  ,    int* &);
        ASSERT_IS_CONVERTIBLE(false,    void*  , C_(int*)&);
        ASSERT_IS_CONVERTIBLE(false,    void*  , V_(int*)&);
        ASSERT_IS_CONVERTIBLE(false,    void*  , CV(int*)&);

        ASSERT_IS_CONVERTIBLE(false, C_(void*) ,    int* &);
        ASSERT_IS_CONVERTIBLE(false, C_(void*) , C_(int*)&);
        ASSERT_IS_CONVERTIBLE(false, C_(void*) , V_(int*)&);
        ASSERT_IS_CONVERTIBLE(false, C_(void*) , CV(int*)&);

        ASSERT_IS_CONVERTIBLE(false, V_(void*) ,    int* &);
        ASSERT_IS_CONVERTIBLE(false, V_(void*) , C_(int*)&);
        ASSERT_IS_CONVERTIBLE(false, V_(void*) , V_(int*)&);
        ASSERT_IS_CONVERTIBLE(false, V_(void*) , CV(int*)&);

        ASSERT_IS_CONVERTIBLE(false, CV(void*) ,    int* &);
        ASSERT_IS_CONVERTIBLE(false, CV(void*) , C_(int*)&);
        ASSERT_IS_CONVERTIBLE(false, CV(void*) , V_(int*)&);
        ASSERT_IS_CONVERTIBLE(false, CV(void*) , CV(int*)&);


        ASSERT_IS_CONVERTIBLE(false,    void* &,    int*  );
        ASSERT_IS_CONVERTIBLE(false,    void* &, C_(int*) );
        ASSERT_IS_CONVERTIBLE(false,    void* &, V_(int*) );
        ASSERT_IS_CONVERTIBLE(false,    void* &, CV(int*) );

        ASSERT_IS_CONVERTIBLE(false, C_(void*)&,    int*  );
        ASSERT_IS_CONVERTIBLE(false, C_(void*)&, C_(int*) );
        ASSERT_IS_CONVERTIBLE(false, C_(void*)&, V_(int*) );
        ASSERT_IS_CONVERTIBLE(false, C_(void*)&, CV(int*) );

        ASSERT_IS_CONVERTIBLE(false, V_(void*)&,    int*  );
        ASSERT_IS_CONVERTIBLE(false, V_(void*)&, C_(int*) );
        ASSERT_IS_CONVERTIBLE(false, V_(void*)&, V_(int*) );
        ASSERT_IS_CONVERTIBLE(false, V_(void*)&, CV(int*) );

        ASSERT_IS_CONVERTIBLE(false, CV(void*)&,    int*  );
        ASSERT_IS_CONVERTIBLE(false, CV(void*)&, C_(int*) );
        ASSERT_IS_CONVERTIBLE(false, CV(void*)&, V_(int*) );
        ASSERT_IS_CONVERTIBLE(false, CV(void*)&, CV(int*) );

        ASSERT_IS_CONVERTIBLE(false,    void* &,    int* &);
        ASSERT_IS_CONVERTIBLE(false,    void* &, C_(int*)&);
        ASSERT_IS_CONVERTIBLE(false,    void* &, V_(int*)&);
        ASSERT_IS_CONVERTIBLE(false,    void* &, CV(int*)&);

        ASSERT_IS_CONVERTIBLE(false, C_(void*)&,    int* &);
        ASSERT_IS_CONVERTIBLE(false, C_(void*)&, C_(int*)&);
        ASSERT_IS_CONVERTIBLE(false, C_(void*)&, V_(int*)&);
        ASSERT_IS_CONVERTIBLE(false, C_(void*)&, CV(int*)&);

        ASSERT_IS_CONVERTIBLE(false, V_(void*)&,    int* &);
        ASSERT_IS_CONVERTIBLE(false, V_(void*)&, C_(int*)&);
        ASSERT_IS_CONVERTIBLE(false, V_(void*)&, V_(int*)&);
        ASSERT_IS_CONVERTIBLE(false, V_(void*)&, CV(int*)&);

        ASSERT_IS_CONVERTIBLE(false, CV(void*)&,    int* &);
        ASSERT_IS_CONVERTIBLE(false, CV(void*)&, C_(int*)&);
        ASSERT_IS_CONVERTIBLE(false, CV(void*)&, V_(int*)&);
        ASSERT_IS_CONVERTIBLE(false, CV(void*)&, CV(int*)&);


        // test conversion to 'void *'

        ASSERT_IS_CONVERTIBLE(true,     float*  ,    void*  );
        ASSERT_IS_CONVERTIBLE(true,     float*  , C_(void*) );
        ASSERT_IS_CONVERTIBLE(true,     float*  , V_(void*) );
        ASSERT_IS_CONVERTIBLE(true,     float*  , CV(void*) );

        ASSERT_IS_CONVERTIBLE(false, C_(float*) ,    void*  );
        ASSERT_IS_CONVERTIBLE(true,  C_(float*) , C_(void*) );
        ASSERT_IS_CONVERTIBLE(false, C_(float*) , V_(void*) );
        ASSERT_IS_CONVERTIBLE(true,  C_(float*) , CV(void*) );

        ASSERT_IS_CONVERTIBLE(false, V_(float*) ,    void*  );
        ASSERT_IS_CONVERTIBLE(false, V_(float*) , C_(void*) );
        ASSERT_IS_CONVERTIBLE(true,  V_(float*) , V_(void*) );
        ASSERT_IS_CONVERTIBLE(true,  V_(float*) , CV(void*) );

        ASSERT_IS_CONVERTIBLE(false, CV(float*) ,    void*  );
        ASSERT_IS_CONVERTIBLE(false, CV(float*) , C_(void*) );
        ASSERT_IS_CONVERTIBLE(false, CV(float*) , V_(void*) );
        ASSERT_IS_CONVERTIBLE(true,  CV(float*) , CV(void*) );

        ASSERT_IS_CONVERTIBLE(false,    float*  ,    void* &);
        ASSERT_IS_CONVERTIBLE(false,    float*  , C_(void*)&);
        ASSERT_IS_CONVERTIBLE(false,    float*  , V_(void*)&);
        ASSERT_IS_CONVERTIBLE(false,    float*  , CV(void*)&);

        ASSERT_IS_CONVERTIBLE(false, C_(float*) ,    void* &);
        ASSERT_IS_CONVERTIBLE(false, C_(float*) , C_(void*)&);
        ASSERT_IS_CONVERTIBLE(false, C_(float*) , V_(void*)&);
        ASSERT_IS_CONVERTIBLE(false, C_(float*) , CV(void*)&);

        ASSERT_IS_CONVERTIBLE(false, V_(float*) ,    void* &);
        ASSERT_IS_CONVERTIBLE(false, V_(float*) , C_(void*)&);
        ASSERT_IS_CONVERTIBLE(false, V_(float*) , V_(void*)&);
        ASSERT_IS_CONVERTIBLE(false, V_(float*) , CV(void*)&);

        ASSERT_IS_CONVERTIBLE(false, CV(float*) ,    void* &);
        ASSERT_IS_CONVERTIBLE(false, CV(float*) , C_(void*)&);
        ASSERT_IS_CONVERTIBLE(false, CV(float*) , V_(void*)&);
        ASSERT_IS_CONVERTIBLE(false, CV(float*) , CV(void*)&);


        ASSERT_IS_CONVERTIBLE(true,     float* &,    void*  );
        ASSERT_IS_CONVERTIBLE(true,     float* &, C_(void*) );
        ASSERT_IS_CONVERTIBLE(true,     float* &, V_(void*) );
        ASSERT_IS_CONVERTIBLE(true,     float* &, CV(void*) );

        ASSERT_IS_CONVERTIBLE(false, C_(float*)&,    void*  );
        ASSERT_IS_CONVERTIBLE(true,  C_(float*)&, C_(void*) );
        ASSERT_IS_CONVERTIBLE(false, C_(float*)&, V_(void*) );
        ASSERT_IS_CONVERTIBLE(true,  C_(float*)&, CV(void*) );

        ASSERT_IS_CONVERTIBLE(false, V_(float*)&,    void*  );
        ASSERT_IS_CONVERTIBLE(false, V_(float*)&, C_(void*) );
        ASSERT_IS_CONVERTIBLE(true,  V_(float*)&, V_(void*) );
        ASSERT_IS_CONVERTIBLE(true,  V_(float*)&, CV(void*) );

        ASSERT_IS_CONVERTIBLE(false, CV(float*)&,    void*  );
        ASSERT_IS_CONVERTIBLE(false, CV(float*)&, C_(void*) );
        ASSERT_IS_CONVERTIBLE(false, CV(float*)&, V_(void*) );
        ASSERT_IS_CONVERTIBLE(true,  CV(float*)&, CV(void*) );

        ASSERT_IS_CONVERTIBLE(false,    float* &,    void* &);
        ASSERT_IS_CONVERTIBLE(false,    float* &, C_(void*)&);
        ASSERT_IS_CONVERTIBLE(false,    float* &, V_(void*)&);
        ASSERT_IS_CONVERTIBLE(false,    float* &, CV(void*)&);

        ASSERT_IS_CONVERTIBLE(false, C_(float*)&,    void* &);
        ASSERT_IS_CONVERTIBLE(false, C_(float*)&, C_(void*)&);
        ASSERT_IS_CONVERTIBLE(false, C_(float*)&, V_(void*)&);
        ASSERT_IS_CONVERTIBLE(false, C_(float*)&, CV(void*)&);

        ASSERT_IS_CONVERTIBLE(false, V_(float*)&,    void* &);
        ASSERT_IS_CONVERTIBLE(false, V_(float*)&, C_(void*)&);
        ASSERT_IS_CONVERTIBLE(false, V_(float*)&, V_(void*)&);
        ASSERT_IS_CONVERTIBLE(false, V_(float*)&, CV(void*)&);

        ASSERT_IS_CONVERTIBLE(false, CV(float*)&,    void* &);
        ASSERT_IS_CONVERTIBLE(false, CV(float*)&, C_(void*)&);
        ASSERT_IS_CONVERTIBLE(false, CV(float*)&, V_(void*)&);
        ASSERT_IS_CONVERTIBLE(false, CV(float*)&, CV(void*)&);


        // Obviously no conversion between pointers and fundamental types.

        ASSERT_IS_CONVERTIBLE(false, float*, int );
        ASSERT_IS_CONVERTIBLE(false, float , int*);

        // Sun 5.2 and 5.5 get this right if the cv-unqualified types differ.

        ASSERT_IS_CONVERTIBLE(true, V_(int), C_(float)&);
        ASSERT_IS_CONVERTIBLE(true, CV(int), C_(float)&);

        ASSERT_IS_CONVERTIBLE(false, int*, float );
        ASSERT_IS_CONVERTIBLE(false, int , float*);

#undef ASSERT_IS_CONVERTIBLE
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

#define ASSERT_IS_CONVERTIBLE(RESULT, FROM_TYPE, TO_TYPE) \
        ASSERT( RESULT == (bslmf::IsConvertible<FROM_TYPE, TO_TYPE>()) )

        // C-1: Test conversion of basic types.

        ASSERT_IS_CONVERTIBLE(1, int,   int  );
        ASSERT_IS_CONVERTIBLE(1, int,   char );
        ASSERT_IS_CONVERTIBLE(0, int,   char*);
        ASSERT_IS_CONVERTIBLE(0, void*, char*);
        ASSERT_IS_CONVERTIBLE(1, char*, void*);

        // C-2: Test cv-qualified conversions for values of the same type.

        ASSERT_IS_CONVERTIBLE(1,    int,     int );
        ASSERT_IS_CONVERTIBLE(1,    int , C_(int));
        ASSERT_IS_CONVERTIBLE(1,    int , V_(int));
        ASSERT_IS_CONVERTIBLE(1,    int , CV(int));

        ASSERT_IS_CONVERTIBLE(1, C_(int),    int );
        ASSERT_IS_CONVERTIBLE(1, C_(int), C_(int));
        ASSERT_IS_CONVERTIBLE(1, C_(int), V_(int));
        ASSERT_IS_CONVERTIBLE(1, C_(int), CV(int));

        ASSERT_IS_CONVERTIBLE(1, V_(int),    int );
        ASSERT_IS_CONVERTIBLE(1, V_(int), C_(int));
        ASSERT_IS_CONVERTIBLE(1, V_(int), V_(int));
        ASSERT_IS_CONVERTIBLE(1, V_(int), CV(int));

        ASSERT_IS_CONVERTIBLE(1, CV(int),    int );
        ASSERT_IS_CONVERTIBLE(1, CV(int), C_(int));
        ASSERT_IS_CONVERTIBLE(1, CV(int), V_(int));
        ASSERT_IS_CONVERTIBLE(1, CV(int), CV(int));

        // C3 Pointer conversions with cv-qualified variants of the same type

        ASSERT_IS_CONVERTIBLE(1,    int *,    int *);
        ASSERT_IS_CONVERTIBLE(1,    int *, C_(int)*);
        ASSERT_IS_CONVERTIBLE(1,    int *, V_(int)*);
        ASSERT_IS_CONVERTIBLE(1,    int *, CV(int)*);

        ASSERT_IS_CONVERTIBLE(0, C_(int)*,    int *);
        ASSERT_IS_CONVERTIBLE(1, C_(int)*, C_(int)*);
        ASSERT_IS_CONVERTIBLE(0, C_(int)*, V_(int)*);
        ASSERT_IS_CONVERTIBLE(1, C_(int)*, CV(int)*);

        ASSERT_IS_CONVERTIBLE(0, V_(int)*,    int *);
        ASSERT_IS_CONVERTIBLE(0, V_(int)*, C_(int)*);
        ASSERT_IS_CONVERTIBLE(1, V_(int)*, V_(int)*);
        ASSERT_IS_CONVERTIBLE(1, V_(int)*, CV(int)*);

        ASSERT_IS_CONVERTIBLE(0, CV(int)*,    int *);
        ASSERT_IS_CONVERTIBLE(0, CV(int)*, C_(int)*);
        ASSERT_IS_CONVERTIBLE(0, CV(int)*, V_(int)*);
        ASSERT_IS_CONVERTIBLE(1, CV(int)*, CV(int)*);


        ASSERT_IS_CONVERTIBLE(1,    int *const,    int *);
        ASSERT_IS_CONVERTIBLE(1,    int *const, C_(int)*);
        ASSERT_IS_CONVERTIBLE(1,    int *const, V_(int)*);
        ASSERT_IS_CONVERTIBLE(1,    int *const, CV(int)*);

        ASSERT_IS_CONVERTIBLE(0, C_(int)*const,    int *);
        ASSERT_IS_CONVERTIBLE(1, C_(int)*const, C_(int)*);
        ASSERT_IS_CONVERTIBLE(0, C_(int)*const, V_(int)*);
        ASSERT_IS_CONVERTIBLE(1, C_(int)*const, CV(int)*);

        ASSERT_IS_CONVERTIBLE(0, V_(int)*const,    int *);
        ASSERT_IS_CONVERTIBLE(0, V_(int)*const, C_(int)*);
        ASSERT_IS_CONVERTIBLE(1, V_(int)*const, V_(int)*);
        ASSERT_IS_CONVERTIBLE(1, V_(int)*const, CV(int)*);

        ASSERT_IS_CONVERTIBLE(0, CV(int)*const,    int *);
        ASSERT_IS_CONVERTIBLE(0, CV(int)*const, C_(int)*);
        ASSERT_IS_CONVERTIBLE(0, CV(int)*const, V_(int)*);
        ASSERT_IS_CONVERTIBLE(1, CV(int)*const, CV(int)*);


        ASSERT_IS_CONVERTIBLE(1,    int *volatile,    int *);
        ASSERT_IS_CONVERTIBLE(1,    int *volatile, C_(int)*);
        ASSERT_IS_CONVERTIBLE(1,    int *volatile, V_(int)*);
        ASSERT_IS_CONVERTIBLE(1,    int *volatile, CV(int)*);

        ASSERT_IS_CONVERTIBLE(0, C_(int)*volatile,    int *);
        ASSERT_IS_CONVERTIBLE(1, C_(int)*volatile, C_(int)*);
        ASSERT_IS_CONVERTIBLE(0, C_(int)*volatile, V_(int)*);
        ASSERT_IS_CONVERTIBLE(1, C_(int)*volatile, CV(int)*);

        ASSERT_IS_CONVERTIBLE(0, V_(int)*volatile,    int *);
        ASSERT_IS_CONVERTIBLE(0, V_(int)*volatile, C_(int)*);
        ASSERT_IS_CONVERTIBLE(1, V_(int)*volatile, V_(int)*);
        ASSERT_IS_CONVERTIBLE(1, V_(int)*volatile, CV(int)*);

        ASSERT_IS_CONVERTIBLE(0, CV(int)*volatile,    int *);
        ASSERT_IS_CONVERTIBLE(0, CV(int)*volatile, C_(int)*);
        ASSERT_IS_CONVERTIBLE(0, CV(int)*volatile, V_(int)*);
        ASSERT_IS_CONVERTIBLE(1, CV(int)*volatile, CV(int)*);


        ASSERT_IS_CONVERTIBLE(1,    int *const volatile,    int *);
        ASSERT_IS_CONVERTIBLE(1,    int *const volatile, C_(int)*);
        ASSERT_IS_CONVERTIBLE(1,    int *const volatile, V_(int)*);
        ASSERT_IS_CONVERTIBLE(1,    int *const volatile, CV(int)*);

        ASSERT_IS_CONVERTIBLE(0, C_(int)*const volatile,    int *);
        ASSERT_IS_CONVERTIBLE(1, C_(int)*const volatile, C_(int)*);
        ASSERT_IS_CONVERTIBLE(0, C_(int)*const volatile, V_(int)*);
        ASSERT_IS_CONVERTIBLE(1, C_(int)*const volatile, CV(int)*);

        ASSERT_IS_CONVERTIBLE(0, V_(int)*const volatile,    int *);
        ASSERT_IS_CONVERTIBLE(0, V_(int)*const volatile, C_(int)*);
        ASSERT_IS_CONVERTIBLE(1, V_(int)*const volatile, V_(int)*);
        ASSERT_IS_CONVERTIBLE(1, V_(int)*const volatile, CV(int)*);

        ASSERT_IS_CONVERTIBLE(0, CV(int)*const volatile,    int *);
        ASSERT_IS_CONVERTIBLE(0, CV(int)*const volatile, C_(int)*);
        ASSERT_IS_CONVERTIBLE(0, CV(int)*const volatile, V_(int)*);
        ASSERT_IS_CONVERTIBLE(1, CV(int)*const volatile, CV(int)*);

        // C-3: Test reference conversions for cv-qualified variants of a type

        ASSERT_IS_CONVERTIBLE(0,             int ,     int &);
        ASSERT_IS_CONVERTIBLE(1,             int ,  C_(int)&);
        ASSERT_IS_CONVERTIBLE(0,             int ,  V_(int)&);
        ASSERT_IS_CONVERTIBLE(0,             int ,  CV(int)&);

        ASSERT_IS_CONVERTIBLE(0,          C_(int),     int &);
        ASSERT_IS_CONVERTIBLE(1,          C_(int),  C_(int)&);
        ASSERT_IS_CONVERTIBLE(0,          C_(int),  V_(int)&);
        ASSERT_IS_CONVERTIBLE(0,          C_(int),  CV(int)&);

        ASSERT_IS_CONVERTIBLE(0,          V_(int),     int &);
        ASSERT_IS_CONVERTIBLE(NOT_NATIVE, V_(int),  C_(int)&);
        ASSERT_IS_CONVERTIBLE(0,          V_(int),  V_(int)&);
        ASSERT_IS_CONVERTIBLE(0,          V_(int),  CV(int)&);

        ASSERT_IS_CONVERTIBLE(0,          CV(int),     int &);
        ASSERT_IS_CONVERTIBLE(NOT_NATIVE, CV(int),  C_(int)&);
        ASSERT_IS_CONVERTIBLE(0,          CV(int),  V_(int)&);
        ASSERT_IS_CONVERTIBLE(0,          CV(int),  CV(int)&);

        ASSERT_IS_CONVERTIBLE(1,             int &,    int  );
        ASSERT_IS_CONVERTIBLE(1,             int &, C_(int) );
        ASSERT_IS_CONVERTIBLE(1,             int &, V_(int) );
        ASSERT_IS_CONVERTIBLE(1,             int &, CV(int) );

        ASSERT_IS_CONVERTIBLE(1,          C_(int)&,    int  );
        ASSERT_IS_CONVERTIBLE(1,          C_(int)&, C_(int) );
        ASSERT_IS_CONVERTIBLE(1,          C_(int)&, V_(int) );
        ASSERT_IS_CONVERTIBLE(1,          C_(int)&, CV(int) );

        ASSERT_IS_CONVERTIBLE(1,          V_(int)&,    int  );
        ASSERT_IS_CONVERTIBLE(1,          V_(int)&, C_(int) );
        ASSERT_IS_CONVERTIBLE(1,          V_(int)&, V_(int) );
        ASSERT_IS_CONVERTIBLE(1,          V_(int)&, CV(int) );

        ASSERT_IS_CONVERTIBLE(1,          CV(int)&,    int  );
        ASSERT_IS_CONVERTIBLE(1,          CV(int)&, C_(int) );
        ASSERT_IS_CONVERTIBLE(1,          CV(int)&, V_(int) );
        ASSERT_IS_CONVERTIBLE(1,          CV(int)&, CV(int) );

        ASSERT_IS_CONVERTIBLE(1,             int &,    int &);
        ASSERT_IS_CONVERTIBLE(1,             int &, C_(int)&);
        ASSERT_IS_CONVERTIBLE(1,             int &, V_(int)&);
        ASSERT_IS_CONVERTIBLE(1,             int &, CV(int)&);

        ASSERT_IS_CONVERTIBLE(0,          C_(int)&,    int &);
        ASSERT_IS_CONVERTIBLE(1,          C_(int)&, C_(int)&);
        ASSERT_IS_CONVERTIBLE(0,          C_(int)&, V_(int)&);
        ASSERT_IS_CONVERTIBLE(1,          C_(int)&, CV(int)&);

        ASSERT_IS_CONVERTIBLE(0,          V_(int)&,    int &);
        ASSERT_IS_CONVERTIBLE(0,          V_(int)&, C_(int)&);
        ASSERT_IS_CONVERTIBLE(1,          V_(int)&, V_(int)&);
        ASSERT_IS_CONVERTIBLE(1,          V_(int)&, CV(int)&);

        ASSERT_IS_CONVERTIBLE(0,          CV(int)&,    int &);
        ASSERT_IS_CONVERTIBLE(0,          CV(int)&, C_(int)&);
        ASSERT_IS_CONVERTIBLE(0,          CV(int)&, V_(int)&);
        ASSERT_IS_CONVERTIBLE(1,          CV(int)&, CV(int)&);


        // C-2: Test cv-qualified conversions for values of the same type.

        ASSERT_IS_CONVERTIBLE(1,             my_Class,     my_Class );
        ASSERT_IS_CONVERTIBLE(1,             my_Class , C_(my_Class));
        ASSERT_IS_CONVERTIBLE(1,             my_Class , V_(my_Class));
        ASSERT_IS_CONVERTIBLE(1,             my_Class , CV(my_Class));

        ASSERT_IS_CONVERTIBLE(1,          C_(my_Class),    my_Class );
        ASSERT_IS_CONVERTIBLE(1,          C_(my_Class), C_(my_Class));
        ASSERT_IS_CONVERTIBLE(1,          C_(my_Class), V_(my_Class));
        ASSERT_IS_CONVERTIBLE(1,          C_(my_Class), CV(my_Class));

        ASSERT_IS_CONVERTIBLE(NOT_NATIVE, V_(my_Class),    my_Class );
        ASSERT_IS_CONVERTIBLE(0,          V_(my_Class), C_(my_Class));
        ASSERT_IS_CONVERTIBLE(0,          V_(my_Class), V_(my_Class));
        ASSERT_IS_CONVERTIBLE(0,          V_(my_Class), CV(my_Class));

        ASSERT_IS_CONVERTIBLE(0,          CV(my_Class),    my_Class );
        ASSERT_IS_CONVERTIBLE(NOT_NATIVE, CV(my_Class), C_(my_Class));
        ASSERT_IS_CONVERTIBLE(0,          CV(my_Class), V_(my_Class));
        ASSERT_IS_CONVERTIBLE(0,          CV(my_Class), CV(my_Class));


        // C-3: Test reference conversions for cv-qualified variants of a type

        ASSERT_IS_CONVERTIBLE(0,    my_Class ,     my_Class &);
        ASSERT_IS_CONVERTIBLE(1,    my_Class ,  C_(my_Class)&);
        ASSERT_IS_CONVERTIBLE(0,    my_Class ,  V_(my_Class)&);
        ASSERT_IS_CONVERTIBLE(0,    my_Class ,  CV(my_Class)&);

        ASSERT_IS_CONVERTIBLE(0, C_(my_Class),     my_Class &);
        ASSERT_IS_CONVERTIBLE(1, C_(my_Class),  C_(my_Class)&);
        ASSERT_IS_CONVERTIBLE(0, C_(my_Class),  V_(my_Class)&);
        ASSERT_IS_CONVERTIBLE(0, C_(my_Class),  CV(my_Class)&);

        ASSERT_IS_CONVERTIBLE(0, V_(my_Class),     my_Class &);
        ASSERT_IS_CONVERTIBLE(0, V_(my_Class),  C_(my_Class)&);
#ifndef BSLMF_ISCONVERTIBLE_MSVC_VOLATILE_BUG
        ASSERT_IS_CONVERTIBLE(0, V_(my_Class),  V_(my_Class)&);
        ASSERT_IS_CONVERTIBLE(0, V_(my_Class),  CV(my_Class)&);
#endif

        ASSERT_IS_CONVERTIBLE(0, CV(my_Class),     my_Class &);
        ASSERT_IS_CONVERTIBLE(0, CV(my_Class),  C_(my_Class)&);
        ASSERT_IS_CONVERTIBLE(0, CV(my_Class),  V_(my_Class)&);
#ifndef BSLMF_ISCONVERTIBLE_MSVC_VOLATILE_BUG
        ASSERT_IS_CONVERTIBLE(0, CV(my_Class),  CV(my_Class)&);
#endif

        ASSERT_IS_CONVERTIBLE(1,    my_Class &,    my_Class  );
        ASSERT_IS_CONVERTIBLE(1,    my_Class &, C_(my_Class) );
        ASSERT_IS_CONVERTIBLE(1,    my_Class &, V_(my_Class) );
        ASSERT_IS_CONVERTIBLE(1,    my_Class &, CV(my_Class) );

        ASSERT_IS_CONVERTIBLE(1, C_(my_Class)&,    my_Class  );
        ASSERT_IS_CONVERTIBLE(1, C_(my_Class)&, C_(my_Class) );
        ASSERT_IS_CONVERTIBLE(1, C_(my_Class)&, V_(my_Class) );
        ASSERT_IS_CONVERTIBLE(1, C_(my_Class)&, CV(my_Class) );

        ASSERT_IS_CONVERTIBLE(0, V_(my_Class)&,    my_Class  );
        ASSERT_IS_CONVERTIBLE(0, V_(my_Class)&, C_(my_Class) );
        ASSERT_IS_CONVERTIBLE(0, V_(my_Class)&, V_(my_Class) );
        ASSERT_IS_CONVERTIBLE(0, V_(my_Class)&, CV(my_Class) );

        ASSERT_IS_CONVERTIBLE(0, CV(my_Class)&,    my_Class  );
        ASSERT_IS_CONVERTIBLE(0, CV(my_Class)&, C_(my_Class) );
        ASSERT_IS_CONVERTIBLE(0, CV(my_Class)&, V_(my_Class) );
        ASSERT_IS_CONVERTIBLE(0, CV(my_Class)&, CV(my_Class) );

        ASSERT_IS_CONVERTIBLE(1,    my_Class &,    my_Class &);
        ASSERT_IS_CONVERTIBLE(1,    my_Class &, C_(my_Class)&);
        ASSERT_IS_CONVERTIBLE(1,    my_Class &, V_(my_Class)&);
        ASSERT_IS_CONVERTIBLE(1,    my_Class &, CV(my_Class)&);

        ASSERT_IS_CONVERTIBLE(0, C_(my_Class)&,    my_Class &);
        ASSERT_IS_CONVERTIBLE(1, C_(my_Class)&, C_(my_Class)&);
        ASSERT_IS_CONVERTIBLE(0, C_(my_Class)&, V_(my_Class)&);
        ASSERT_IS_CONVERTIBLE(1, C_(my_Class)&, CV(my_Class)&);

        ASSERT_IS_CONVERTIBLE(0, V_(my_Class)&,    my_Class &);
        ASSERT_IS_CONVERTIBLE(0, V_(my_Class)&, C_(my_Class)&);
        ASSERT_IS_CONVERTIBLE(1, V_(my_Class)&, V_(my_Class)&);
        ASSERT_IS_CONVERTIBLE(1, V_(my_Class)&, CV(my_Class)&);

        ASSERT_IS_CONVERTIBLE(0, CV(my_Class)&,    my_Class &);
        ASSERT_IS_CONVERTIBLE(0, CV(my_Class)&, C_(my_Class)&);
        ASSERT_IS_CONVERTIBLE(0, CV(my_Class)&, V_(my_Class)&);
        ASSERT_IS_CONVERTIBLE(1, CV(my_Class)&, CV(my_Class)&);

        // C-4: Test volatile value conversions.

        // C-5: Test volatile pointer and reference conversions

        // C-6: Test conversions on different combinations of cv-qualified
        //      types.

        // C-7: Test conversions on different combinations of cv-qualified
        //      user-defined types.

        ASSERT_IS_CONVERTIBLE(1,       my_Class,        my_Class );
        ASSERT_IS_CONVERTIBLE(1,       my_Class,  const my_Class );
        ASSERT_IS_CONVERTIBLE(1,       my_Class,  const my_Class&);
        ASSERT_IS_CONVERTIBLE(0, const my_Class,        my_Class&);
        ASSERT_IS_CONVERTIBLE(1, const my_Class,  const my_Class&);
        ASSERT_IS_CONVERTIBLE(1, const my_Class&, const my_Class );

        ASSERT_IS_CONVERTIBLE(0, const my_Class, my_Class&);
        ASSERT_IS_CONVERTIBLE(1,       int,      my_Class );
        ASSERT_IS_CONVERTIBLE(0,       my_Class, int      );
        ASSERT_IS_CONVERTIBLE(0,       int*,     my_Class*);

        ASSERT_IS_CONVERTIBLE(1,       my_OtherClass,       my_Class);
        ASSERT_IS_CONVERTIBLE(1,       my_OtherClass, const my_Class);
        ASSERT_IS_CONVERTIBLE(0, const my_OtherClass, const my_Class);

        ASSERT_IS_CONVERTIBLE(1,          my_Class,        my_ThirdClass );
        ASSERT_IS_CONVERTIBLE(1, const    my_Class,        my_ThirdClass );
        ASSERT_IS_CONVERTIBLE(1,          my_Class,  const my_ThirdClass );
        ASSERT_IS_CONVERTIBLE(1, const    my_Class,  const my_ThirdClass );
        ASSERT_IS_CONVERTIBLE(0, volatile my_Class,        my_ThirdClass );
        ASSERT_IS_CONVERTIBLE(1,          my_Class&, const my_ThirdClass );
        ASSERT_IS_CONVERTIBLE(1, const    my_Class&, const my_ThirdClass );
        ASSERT_IS_CONVERTIBLE(0, volatile my_Class&,       my_ThirdClass );
        ASSERT_IS_CONVERTIBLE(1,          my_Class&, const my_ThirdClass&);
        ASSERT_IS_CONVERTIBLE(1, const    my_Class&, const my_ThirdClass&);
        ASSERT_IS_CONVERTIBLE(0, volatile my_Class&,       my_ThirdClass&);

        ASSERT_IS_CONVERTIBLE(0, my_OtherClass, int);

        ASSERT_IS_CONVERTIBLE(1, my_Enum, my_Enum );
        ASSERT_IS_CONVERTIBLE(1, my_Enum, int     );
        ASSERT_IS_CONVERTIBLE(0, int,     my_Enum );

        ASSERT_IS_CONVERTIBLE(1, my_Enum,   my_Class );
        ASSERT_IS_CONVERTIBLE(0, my_Class,  my_Enum  );
        ASSERT_IS_CONVERTIBLE(0, my_Enum,   my_Class*);
        ASSERT_IS_CONVERTIBLE(0, my_Class,  my_Enum *);
        ASSERT_IS_CONVERTIBLE(0, my_Enum *, my_Class );
        ASSERT_IS_CONVERTIBLE(0, my_Class*, my_Enum  );

        ASSERT_IS_CONVERTIBLE(1, my_EnumClass::Type, my_Class);
        ASSERT_IS_CONVERTIBLE(0, my_Class, my_EnumClass::Type);
        ASSERT_IS_CONVERTIBLE(0, my_EnumClass::Type, my_Class*);
        ASSERT_IS_CONVERTIBLE(0, my_Class, my_EnumClass::Type*);
        ASSERT_IS_CONVERTIBLE(0, my_EnumClass::Type*, my_Class);
        ASSERT_IS_CONVERTIBLE(0, my_Class*, my_EnumClass::Type);

        ASSERT_IS_CONVERTIBLE(1, my_BslmaAllocator*, my_StlAllocator<int>    );
        ASSERT_IS_CONVERTIBLE(1, my_BslmaAllocator*, my_StlAllocator<void>   );
        ASSERT_IS_CONVERTIBLE(1, my_BslmaAllocator*, my_StlAllocator<void*>  );
        ASSERT_IS_CONVERTIBLE(1, my_BslmaAllocator*, my_StlAllocator<my_Enum>);
        ASSERT_IS_CONVERTIBLE(0, void*, my_StlAllocator<my_Enum> );
        ASSERT_IS_CONVERTIBLE(1, my_BslmaAllocator*,
                                 my_StlAllocator<my_EnumClass::Type> );
        ASSERT_IS_CONVERTIBLE(0, void*, my_StlAllocator<my_EnumClass::Type>);

        // C-8: Test conversion between 'void' type and other types.

        ASSERT_IS_CONVERTIBLE(1, void, void);
        ASSERT_IS_CONVERTIBLE(0, void, int );
        ASSERT_IS_CONVERTIBLE(0, int,  void);

        // const-qualified void should also be supported
        ASSERT_IS_CONVERTIBLE(1, const void, const void);
        ASSERT_IS_CONVERTIBLE(1,       void, const void);
        ASSERT_IS_CONVERTIBLE(0, const void,       int );
        ASSERT_IS_CONVERTIBLE(0,       int,  const void);

        // C-9: Test derived-to-base convertibility

        ASSERT_IS_CONVERTIBLE(NOT_NATIVE, my_DerivedClass, my_AbstractClass);
        ASSERT_IS_CONVERTIBLE(0, my_AbstractClass,  my_DerivedClass);
        ASSERT_IS_CONVERTIBLE(1, my_DerivedClass &, my_AbstractClass&);
        ASSERT_IS_CONVERTIBLE(0, my_AbstractClass&, my_DerivedClass &);
        ASSERT_IS_CONVERTIBLE(1, my_DerivedClass *, my_AbstractClass*);
        ASSERT_IS_CONVERTIBLE(0, my_AbstractClass*, my_DerivedClass *);

        // C-10: Test pointer-to-member variable convertibility

        ASSERT_IS_CONVERTIBLE(1, int my_AbstractClass::*,
                                 int my_DerivedClass ::*);
        ASSERT_IS_CONVERTIBLE(0, int my_DerivedClass ::*,
                                 int my_AbstractClass::*);
        ASSERT_IS_CONVERTIBLE(0, int my_AbstractClass::*, void*);
        ASSERT_IS_CONVERTIBLE(0, void*, int my_AbstractClass::*);

        // C-11: Test pointer-to-member function convertibility

        ASSERT_IS_CONVERTIBLE(1, int (my_AbstractClass::*)(int),
                                 int (my_DerivedClass::*)(int) );
        ASSERT_IS_CONVERTIBLE(0, int (my_DerivedClass::*)(int),
                                 int (my_AbstractClass::*)(int) );
        ASSERT_IS_CONVERTIBLE(0, int (my_AbstractClass::*)(int),
                                 void*);
        ASSERT_IS_CONVERTIBLE(0, void*,
                                 int (my_AbstractClass::*)(int) );

        // C-12: Test arrays of unknown bound convertibility

        ASSERT_IS_CONVERTIBLE(1,    int  (*)[],    int  (*)[]   );
        ASSERT_IS_CONVERTIBLE(1,    int  (*)[][5], int  (*)[][5]);
        ASSERT_IS_CONVERTIBLE(0,    int  (*)[],    int   *      );
        ASSERT_IS_CONVERTIBLE(0,    int   *,       int  (*)[]   );
        ASSERT_IS_CONVERTIBLE(0,    int  (*)[][5], int   *      );
        ASSERT_IS_CONVERTIBLE(1,    int  (*)[], V_(int) (*)[]   );
        ASSERT_IS_CONVERTIBLE(0, V_(int) (*)[],    int  (*)[]   );
        ASSERT_IS_CONVERTIBLE(1,    int  (*)[], C_(int) (*)[]   );
        ASSERT_IS_CONVERTIBLE(0, C_(int) (*)[],    int  (*)[]   );
        ASSERT_IS_CONVERTIBLE(0, C_(int) (*)[], V_(int) (*)[]   );
        ASSERT_IS_CONVERTIBLE(0, V_(int) (*)[], C_(int) (*)[]   );

        // C-13
#if defined(BSLMF_ISCONVERTIBLE_TEST_COMPILE_ERROR_WITH_INCOMPLETE_TYPES)
        // An incomplete class can only be tested as the FROM parameter.

        ASSERT_IS_CONVERTIBLE(1, my_IncompleteClass, my_IncompleteClass);
        ASSERT_IS_CONVERTIBLE(0, my_IncompleteClass, int);

        // Test references with incomplete types as both TO and FROM parameters

        ASSERT_IS_CONVERTIBLE(1, my_IncompleteClass,  my_IncompleteClass &);
        ASSERT_IS_CONVERTIBLE(0, my_IncompleteClass&, int                 );
        ASSERT_IS_CONVERTIBLE(0, int,                 my_IncompleteClass &);
        ASSERT_IS_CONVERTIBLE(0, my_IncompleteClass,  my_IncompleteClass2&);
        ASSERT_IS_CONVERTIBLE(0, my_IncompleteClass&, my_IncompleteClass2&);
#endif

        // C-14: Test function references decay to function pointers

        ASSERT_IS_CONVERTIBLE(0,    void(),    void   );
        ASSERT_IS_CONVERTIBLE(0,    void(),    int    );
        ASSERT_IS_CONVERTIBLE(0,    void(), C_(void)  );
        ASSERT_IS_CONVERTIBLE(0,    void(), V_(int)   );
        ASSERT_IS_CONVERTIBLE(0,    void,      void() );
        ASSERT_IS_CONVERTIBLE(0,    int,       void() );
        ASSERT_IS_CONVERTIBLE(0, C_(void),     void() );
        ASSERT_IS_CONVERTIBLE(0, V_(int),      void() );

        ASSERT_IS_CONVERTIBLE(0, void    (), void    ());
        ASSERT_IS_CONVERTIBLE(1, void    (), void (&)());
        ASSERT_IS_CONVERTIBLE(1, void    (), void (*)());
        ASSERT_IS_CONVERTIBLE(1, void (&)(), void (&)());
        ASSERT_IS_CONVERTIBLE(1, void (&)(), void (*)());
        ASSERT_IS_CONVERTIBLE(0, void (*)(), void    ());
        ASSERT_IS_CONVERTIBLE(0, void (*)(), void (&)());
        ASSERT_IS_CONVERTIBLE(0, void (&)(), void    ());
        ASSERT_IS_CONVERTIBLE(1, void (*)(), void (*)());

        ASSERT_IS_CONVERTIBLE(0, void(...),    void   );
        ASSERT_IS_CONVERTIBLE(0, void(...),    int    );
        ASSERT_IS_CONVERTIBLE(0, void(...), C_(void)  );
        ASSERT_IS_CONVERTIBLE(0, void(...), V_(int)   );
        ASSERT_IS_CONVERTIBLE(0,    void,   void(...) );
        ASSERT_IS_CONVERTIBLE(0,    int,    void(...) );
        ASSERT_IS_CONVERTIBLE(0, C_(void),  void(...) );
        ASSERT_IS_CONVERTIBLE(0, V_(int),   void(...) );

        ASSERT_IS_CONVERTIBLE(0, void    (...), void    (...));
        ASSERT_IS_CONVERTIBLE(1, void    (...), void (&)(...));
        ASSERT_IS_CONVERTIBLE(1, void    (...), void (*)(...));
        ASSERT_IS_CONVERTIBLE(1, void (&)(...), void (&)(...));
        ASSERT_IS_CONVERTIBLE(1, void (&)(...), void (*)(...));
        ASSERT_IS_CONVERTIBLE(0, void (*)(...), void    (...));
        ASSERT_IS_CONVERTIBLE(0, void (*)(...), void (&)(...));
        ASSERT_IS_CONVERTIBLE(0, void (&)(...), void    (...));

        ASSERT_IS_CONVERTIBLE(0, void    (...), void    ());
        ASSERT_IS_CONVERTIBLE(0, void    (...), void (&)());
        ASSERT_IS_CONVERTIBLE(0, void    (...), void (*)());
        ASSERT_IS_CONVERTIBLE(0, void (&)(...), void (&)());
        ASSERT_IS_CONVERTIBLE(0, void (&)(...), void (*)());
        ASSERT_IS_CONVERTIBLE(0, void (*)(...), void    ());
        ASSERT_IS_CONVERTIBLE(0, void (*)(...), void (&)());
        ASSERT_IS_CONVERTIBLE(0, void (&)(...), void    ());

        ASSERT_IS_CONVERTIBLE(0, void    (), void    (...));
        ASSERT_IS_CONVERTIBLE(0, void    (), void (&)(...));
        ASSERT_IS_CONVERTIBLE(0, void    (), void (*)(...));
        ASSERT_IS_CONVERTIBLE(0, void (&)(), void (&)(...));
        ASSERT_IS_CONVERTIBLE(0, void (&)(), void (*)(...));
        ASSERT_IS_CONVERTIBLE(0, void (*)(), void    (...));
        ASSERT_IS_CONVERTIBLE(0, void (*)(), void (&)(...));
        ASSERT_IS_CONVERTIBLE(0, void (&)(), void    (...));

        ASSERT_IS_CONVERTIBLE(0, void(), void);
        ASSERT_IS_CONVERTIBLE(0, void(),  int);
        ASSERT_IS_CONVERTIBLE(0, void, void());
        ASSERT_IS_CONVERTIBLE(0, int,  void());

        ASSERT_IS_CONVERTIBLE(0, int(char, float...), void);
        ASSERT_IS_CONVERTIBLE(0, int(char, float...),  int);
        ASSERT_IS_CONVERTIBLE(0, void, int(char, float...));
        ASSERT_IS_CONVERTIBLE(0, int,  int(char, float...));

        ASSERT_IS_CONVERTIBLE(0, void(), void());
        ASSERT_IS_CONVERTIBLE(1, void(), void(&)());
        ASSERT_IS_CONVERTIBLE(1, void(), void(*)());

#undef ASSERT_IS_CONVERTIBLE
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
        //:
        //: 13 That 'is_convertible_v<T>' has the same value as
        //:    'is_convertible<T>::value' for a variety of template parameter
        //:    types.
        //
        // Plan:
        //: 1 Instantiate 'bsl::is_convertible' with various type combinations
        //:   and verify that the 'VALUE' member is initialized properly.
        //:   (C-1..11)
        //
        // Testing:
        //   bsl::is_convertible::value
        //   bsl::is_convertible_v
        // --------------------------------------------------------------------

        if (verbose) printf("'bsl::is_convertible::value'\n"
                            "============================\n");

        ASSERT(2 == sizeof(C00));
        ASSERT(2 == sizeof(C01));
        ASSERT(1 == sizeof(C02));
        ASSERT(1 == sizeof(C03));

#ifdef BSLS_COMPILERFEATURES_SUPPORT_VARIABLE_TEMPLATES
        // C-13
#define ASSERT_IS_CONVERTIBLE(RESULT, FROM_TYPE, TO_TYPE)                     \
        ASSERT(RESULT == (bsl::is_convertible  <FROM_TYPE, TO_TYPE>::value)); \
                                                                              \
        ASSERT(          (bsl::is_convertible  <FROM_TYPE, TO_TYPE>::value)   \
                      == (bsl::is_convertible_v<FROM_TYPE, TO_TYPE>))
#else
#define ASSERT_IS_CONVERTIBLE(RESULT, FROM_TYPE, TO_TYPE)                     \
        ASSERT(RESULT == (bsl::is_convertible  <FROM_TYPE, TO_TYPE>::value))
#endif

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

        ASSERT_IS_CONVERTIBLE(false,         int ,     int &);
        ASSERT_IS_CONVERTIBLE(true,          int ,  C_(int)&);
        ASSERT_IS_CONVERTIBLE(false,         int ,  V_(int)&);
        ASSERT_IS_CONVERTIBLE(false,         int ,  CV(int)&);

        ASSERT_IS_CONVERTIBLE(false,      C_(int),     int &);
        ASSERT_IS_CONVERTIBLE(true,       C_(int),  C_(int)&);
        ASSERT_IS_CONVERTIBLE(false,      C_(int),  V_(int)&);
        ASSERT_IS_CONVERTIBLE(false,      C_(int),  CV(int)&);

        ASSERT_IS_CONVERTIBLE(false,      V_(int),     int &);
        ASSERT_IS_CONVERTIBLE(NOT_NATIVE, V_(int),  C_(int)&);
        ASSERT_IS_CONVERTIBLE(false,      V_(int),  V_(int)&);
        ASSERT_IS_CONVERTIBLE(false,      V_(int),  CV(int)&);

        ASSERT_IS_CONVERTIBLE(false,      CV(int),     int &);
        ASSERT_IS_CONVERTIBLE(NOT_NATIVE, CV(int),  C_(int)&);
        ASSERT_IS_CONVERTIBLE(false,      CV(int),  V_(int)&);
        ASSERT_IS_CONVERTIBLE(false,      CV(int),  CV(int)&);

        ASSERT_IS_CONVERTIBLE(true,          int &,    int  );
        ASSERT_IS_CONVERTIBLE(true,          int &, C_(int) );
        ASSERT_IS_CONVERTIBLE(true,          int &, V_(int) );
        ASSERT_IS_CONVERTIBLE(true,          int &, CV(int) );

        ASSERT_IS_CONVERTIBLE(true,       C_(int)&,    int  );
        ASSERT_IS_CONVERTIBLE(true,       C_(int)&, C_(int) );
        ASSERT_IS_CONVERTIBLE(true,       C_(int)&, V_(int) );
        ASSERT_IS_CONVERTIBLE(true,       C_(int)&, CV(int) );

        ASSERT_IS_CONVERTIBLE(true,       V_(int)&,    int  );
        ASSERT_IS_CONVERTIBLE(true,       V_(int)&, C_(int) );
        ASSERT_IS_CONVERTIBLE(true,       V_(int)&, V_(int) );
        ASSERT_IS_CONVERTIBLE(true,       V_(int)&, CV(int) );

        ASSERT_IS_CONVERTIBLE(true,       CV(int)&,    int  );
        ASSERT_IS_CONVERTIBLE(true,       CV(int)&, C_(int) );
        ASSERT_IS_CONVERTIBLE(true,       CV(int)&, V_(int) );
        ASSERT_IS_CONVERTIBLE(true,       CV(int)&, CV(int) );

        ASSERT_IS_CONVERTIBLE(true,          int &,    int &);
        ASSERT_IS_CONVERTIBLE(true,          int &, C_(int)&);
        ASSERT_IS_CONVERTIBLE(true,          int &, V_(int)&);
        ASSERT_IS_CONVERTIBLE(true,          int &, CV(int)&);

        ASSERT_IS_CONVERTIBLE(false,      C_(int)&,    int &);
        ASSERT_IS_CONVERTIBLE(true,       C_(int)&, C_(int)&);
        ASSERT_IS_CONVERTIBLE(false,      C_(int)&, V_(int)&);
        ASSERT_IS_CONVERTIBLE(true,       C_(int)&, CV(int)&);

        ASSERT_IS_CONVERTIBLE(false,      V_(int)&,    int &);
        ASSERT_IS_CONVERTIBLE(false,      V_(int)&, C_(int)&);
        ASSERT_IS_CONVERTIBLE(true,       V_(int)&, V_(int)&);
        ASSERT_IS_CONVERTIBLE(true,       V_(int)&, CV(int)&);

        ASSERT_IS_CONVERTIBLE(false,      CV(int)&,    int &);
        ASSERT_IS_CONVERTIBLE(false,      CV(int)&, C_(int)&);
        ASSERT_IS_CONVERTIBLE(false,      CV(int)&, V_(int)&);
        ASSERT_IS_CONVERTIBLE(true,       CV(int)&, CV(int)&);


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
#ifndef BSLMF_ISCONVERTIBLE_MSVC_VOLATILE_BUG
        ASSERT_IS_CONVERTIBLE(false, V_(my_Class),  V_(my_Class)&);
        ASSERT_IS_CONVERTIBLE(false, V_(my_Class),  CV(my_Class)&);
#endif

        ASSERT_IS_CONVERTIBLE(false, CV(my_Class),     my_Class &);
        ASSERT_IS_CONVERTIBLE(false, CV(my_Class),  C_(my_Class)&);
        ASSERT_IS_CONVERTIBLE(false, CV(my_Class),  V_(my_Class)&);
#ifndef BSLMF_ISCONVERTIBLE_MSVC_VOLATILE_BUG
        ASSERT_IS_CONVERTIBLE(false, CV(my_Class),  CV(my_Class)&);
#endif

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
        ASSERT_IS_CONVERTIBLE(true,  char,   const int&);
        ASSERT_IS_CONVERTIBLE(true,  double, const int&);

        ASSERT_IS_CONVERTIBLE(true,  volatile double, const int&);
        ASSERT_IS_CONVERTIBLE(true,  char &,          const int&);

        // C-4: Test volatile value conversions.

        // C-5: Test volatile pointer and reference conversions


        // C-6: Test conversions on different combinations of cv-qualified
        //      types.


        // C-7: Test conversions on different combinations of cv-qualified
        //      user-defined types.

        ASSERT_IS_CONVERTIBLE(true , int,      my_Class );
        ASSERT_IS_CONVERTIBLE(false, my_Class, int      );
        ASSERT_IS_CONVERTIBLE(false, int*,     my_Class*);

        ASSERT_IS_CONVERTIBLE(true,     my_OtherClass,     my_Class       );
        ASSERT_IS_CONVERTIBLE(true,     my_OtherClass,  C_(my_Class)      );
        ASSERT_IS_CONVERTIBLE(false, C_(my_OtherClass), C_(my_Class)      );
        ASSERT_IS_CONVERTIBLE(true,     my_Class,          my_ThirdClass  );
        ASSERT_IS_CONVERTIBLE(true,  C_(my_Class),         my_ThirdClass  );
        ASSERT_IS_CONVERTIBLE(true,     my_Class,       C_(my_ThirdClass) );
        ASSERT_IS_CONVERTIBLE(true,  C_(my_Class),      C_(my_ThirdClass) );
        ASSERT_IS_CONVERTIBLE(false, V_(my_Class),         my_ThirdClass  );
        ASSERT_IS_CONVERTIBLE(true,     my_Class&,      C_(my_ThirdClass) );
        ASSERT_IS_CONVERTIBLE(true,  C_(my_Class)&,     C_(my_ThirdClass) );
        ASSERT_IS_CONVERTIBLE(false, V_(my_Class)&,        my_ThirdClass  );
        ASSERT_IS_CONVERTIBLE(true,     my_Class&,      C_(my_ThirdClass)&);
        ASSERT_IS_CONVERTIBLE(true,  C_(my_Class)&,     C_(my_ThirdClass)&);
        ASSERT_IS_CONVERTIBLE(false, V_(my_Class)&,        my_ThirdClass &);

        ASSERT_IS_CONVERTIBLE(NOT_NATIVE, my_NotCopyableClass,
                                          my_NotCopyableClass);
        ASSERT_IS_CONVERTIBLE(false,      my_NotCopyableClass,
                                          my_NotCopyableClass  &);
        ASSERT_IS_CONVERTIBLE(true,       my_NotCopyableClass,
                                       C_(my_NotCopyableClass) &);

        ASSERT_IS_CONVERTIBLE(false, my_OtherClass, int);

        ASSERT_IS_CONVERTIBLE(true,  my_Enum, my_Enum);
        ASSERT_IS_CONVERTIBLE(true,  my_Enum, int    );
        ASSERT_IS_CONVERTIBLE(false, int,     my_Enum);

        ASSERT_IS_CONVERTIBLE(true,  my_Enum,   my_Class );
        ASSERT_IS_CONVERTIBLE(false, my_Class,  my_Enum  );
        ASSERT_IS_CONVERTIBLE(false, my_Enum,   my_Class*);
        ASSERT_IS_CONVERTIBLE(false, my_Class,  my_Enum* );
        ASSERT_IS_CONVERTIBLE(false, my_Enum*,  my_Class );
        ASSERT_IS_CONVERTIBLE(false, my_Class*, my_Enum  );

        ASSERT_IS_CONVERTIBLE(true,  my_EnumClass::Type,  my_Class           );
        ASSERT_IS_CONVERTIBLE(false, my_Class,            my_EnumClass::Type );
        ASSERT_IS_CONVERTIBLE(false, my_EnumClass::Type,  my_Class*          );
        ASSERT_IS_CONVERTIBLE(false, my_Class,            my_EnumClass::Type*);
        ASSERT_IS_CONVERTIBLE(false, my_EnumClass::Type*, my_Class           );
        ASSERT_IS_CONVERTIBLE(false, my_Class*,           my_EnumClass::Type );

        ASSERT_IS_CONVERTIBLE(true,  my_BslmaAllocator*,
                                     my_StlAllocator<int> );
        ASSERT_IS_CONVERTIBLE(true,  my_BslmaAllocator*,
                                     my_StlAllocator<void> );
        ASSERT_IS_CONVERTIBLE(true,  my_BslmaAllocator*,
                                     my_StlAllocator<void*> );
        ASSERT_IS_CONVERTIBLE(true,  my_BslmaAllocator*,
                                     my_StlAllocator<my_Enum> );
        ASSERT_IS_CONVERTIBLE(false, void*,
                                     my_StlAllocator<my_Enum> );
        ASSERT_IS_CONVERTIBLE(true,  my_BslmaAllocator*,
                                     my_StlAllocator<my_EnumClass::Type>);
        ASSERT_IS_CONVERTIBLE(false, void*,
                                      my_StlAllocator<my_EnumClass::Type>);
        ASSERT_IS_CONVERTIBLE(true,  void*,
                                     my_PlacementNew);


        // C-8: Test conversion between 'void' type and other types.


        ASSERT_IS_CONVERTIBLE(true,                void,                void);
        ASSERT_IS_CONVERTIBLE(true,                void, const          void);
        ASSERT_IS_CONVERTIBLE(true,                void,       volatile void);
        ASSERT_IS_CONVERTIBLE(true,                void, const volatile void);

        ASSERT_IS_CONVERTIBLE(true, const          void,                void);
        ASSERT_IS_CONVERTIBLE(true, const          void, const          void);
        ASSERT_IS_CONVERTIBLE(true, const          void,       volatile void);
        ASSERT_IS_CONVERTIBLE(true, const          void, const volatile void);

        ASSERT_IS_CONVERTIBLE(true,       volatile void,                void);
        ASSERT_IS_CONVERTIBLE(true,       volatile void, const          void);
        ASSERT_IS_CONVERTIBLE(true,       volatile void,       volatile void);
        ASSERT_IS_CONVERTIBLE(true,       volatile void, const volatile void);

        ASSERT_IS_CONVERTIBLE(true, const volatile void,                void);
        ASSERT_IS_CONVERTIBLE(true, const volatile void, const          void);
        ASSERT_IS_CONVERTIBLE(true, const volatile void,       volatile void);
        ASSERT_IS_CONVERTIBLE(true, const volatile void, const volatile void);

        ASSERT_IS_CONVERTIBLE(false,                void, int );
        ASSERT_IS_CONVERTIBLE(false,                void, int&);
        ASSERT_IS_CONVERTIBLE(false,                void, int*);

        ASSERT_IS_CONVERTIBLE(false, const          void, int );
        ASSERT_IS_CONVERTIBLE(false, const          void, int&);
        ASSERT_IS_CONVERTIBLE(false, const          void, int*);

        ASSERT_IS_CONVERTIBLE(false,       volatile void, int );
        ASSERT_IS_CONVERTIBLE(false,       volatile void, int&);
        ASSERT_IS_CONVERTIBLE(false,       volatile void, int*);

        ASSERT_IS_CONVERTIBLE(false, const volatile void, int );
        ASSERT_IS_CONVERTIBLE(false, const volatile void, int&);
        ASSERT_IS_CONVERTIBLE(false, const volatile void, int*);

        ASSERT_IS_CONVERTIBLE(false, int,                 void);
        ASSERT_IS_CONVERTIBLE(false, int,  const          void);
        ASSERT_IS_CONVERTIBLE(false, int,        volatile void);
        ASSERT_IS_CONVERTIBLE(false, int,  const volatile void);

        ASSERT_IS_CONVERTIBLE(false, int&,                void);
        ASSERT_IS_CONVERTIBLE(false, int&, const          void);
        ASSERT_IS_CONVERTIBLE(false, int&,       volatile void);
        ASSERT_IS_CONVERTIBLE(false, int&, const volatile void);

        ASSERT_IS_CONVERTIBLE(false, int*,                void);
        ASSERT_IS_CONVERTIBLE(false, int*, const          void);
        ASSERT_IS_CONVERTIBLE(false, int*,       volatile void);
        ASSERT_IS_CONVERTIBLE(false, int*, const volatile void);

        // C-9: Test derived-to-base convertibility

        ASSERT_IS_CONVERTIBLE(NOT_NATIVE,
                                     my_DerivedClass,   my_AbstractClass);
        ASSERT_IS_CONVERTIBLE(false, my_AbstractClass,  my_DerivedClass);
        ASSERT_IS_CONVERTIBLE(true,  my_DerivedClass &, my_AbstractClass&);
        ASSERT_IS_CONVERTIBLE(false, my_AbstractClass&, my_DerivedClass &);
        ASSERT_IS_CONVERTIBLE(true,  my_DerivedClass *, my_AbstractClass*);
        ASSERT_IS_CONVERTIBLE(false, my_AbstractClass*, my_DerivedClass *);

        // C-10: Test pointer-to-member variable convertibility

        ASSERT_IS_CONVERTIBLE(true,  int my_AbstractClass::*,
                                     int my_DerivedClass ::*);
        ASSERT_IS_CONVERTIBLE(false, int my_DerivedClass ::*,
                                     int my_AbstractClass::*);
        ASSERT_IS_CONVERTIBLE(false, int my_AbstractClass::*, void*);
        ASSERT_IS_CONVERTIBLE(false, void*, int my_AbstractClass::*);

        // C-11: Test pointer-to-member function convertibility

        ASSERT_IS_CONVERTIBLE(true,  int (my_AbstractClass::*)(int),
                                     int (my_DerivedClass:: *)(int));
        ASSERT_IS_CONVERTIBLE(false, int (my_DerivedClass ::*)(int),
                                     int (my_AbstractClass::*)(int));
        ASSERT_IS_CONVERTIBLE(false, int (my_AbstractClass::*)(int), void*);
        ASSERT_IS_CONVERTIBLE(false, void*,  int  (my_AbstractClass::*)(int) );

        // C-12: Test arrays of unknown bound convertibility

#ifndef BSLMF_ISCONVERTIBLE_NO_ARRAY_OF_UNKNOWN_BOUND_AS_TEMPLATE_PARAMETER
        ASSERT_IS_CONVERTIBLE(true,  int    [],    int  *   );
        ASSERT_IS_CONVERTIBLE(false, int    [][5], int  *   );
        ASSERT_IS_CONVERTIBLE(false, int (*)[],    int  *   );
        ASSERT_IS_CONVERTIBLE(false, int (*)[][5], int  *   );
# if !defined(BSLMF_ISCONVERTIBLE_NO_ARRAY_REF_OF_UNKNOWN_BOUND)
        // If the test call cannot be parsed, there is no concern for whether
        // the component diagnoses the trait correctly - it cannot be called.
        ASSERT_IS_CONVERTIBLE(true,  int (&)[],    int  *   );
        ASSERT_IS_CONVERTIBLE(false, int (&)[][5], int  *   );
        ASSERT_IS_CONVERTIBLE(false, int  *,       int (&)[]);
# endif // BSLMF_ISCONVERTIBLE_NO_ARRAY_REF_OF_UNKNOWN_BOUND
        ASSERT_IS_CONVERTIBLE(false, int  *,       int (*)[]);

        ASSERT_IS_CONVERTIBLE(false,    int  [],       int  (*)[]   );
        ASSERT_IS_CONVERTIBLE(false,    int  [][5],    int  (*)[][5]);
        ASSERT_IS_CONVERTIBLE(false,    int  [],    V_(int) (*)[]   );
        ASSERT_IS_CONVERTIBLE(false, V_(int) [],       int  (*)[]   );
        ASSERT_IS_CONVERTIBLE(false,    int  [],    C_(int) (*)[]   );
        ASSERT_IS_CONVERTIBLE(false, C_(int) [],       int  (*)[]   );
        ASSERT_IS_CONVERTIBLE(false, C_(int) [],    V_(int) (*)[]   );
        ASSERT_IS_CONVERTIBLE(false, V_(int) [],    C_(int) (*)[]   );

        ASSERT_IS_CONVERTIBLE(true,     int  (*)[],    int  (*)[]   );
        ASSERT_IS_CONVERTIBLE(true,     int  (*)[][5], int  (*)[][5]);
        ASSERT_IS_CONVERTIBLE(true,     int  (*)[], V_(int) (*)[]   );
        ASSERT_IS_CONVERTIBLE(false, V_(int) (*)[],    int  (*)[]   );
        ASSERT_IS_CONVERTIBLE(true,     int  (*)[], C_(int) (*)[]   );
        ASSERT_IS_CONVERTIBLE(false, C_(int) (*)[],    int  (*)[]   );
        ASSERT_IS_CONVERTIBLE(false, C_(int) (*)[], V_(int) (*)[]   );
        ASSERT_IS_CONVERTIBLE(false, V_(int) (*)[], C_(int) (*)[]   );

# if !defined(BSLMF_ISCONVERTIBLE_NO_ARRAY_REF_OF_UNKNOWN_BOUND)
        // If the test call cannot be parsed, there is no concern for whether
        // the component diagnoses the trait correctly - it cannot be called.
        ASSERT_IS_CONVERTIBLE(false,    int  [],       int  (&)[]   );
        ASSERT_IS_CONVERTIBLE(false,    int  [][5],    int  (&)[][5]);
        ASSERT_IS_CONVERTIBLE(false,    int  [],    V_(int) (&)[]   );
        ASSERT_IS_CONVERTIBLE(false, V_(int) [],       int  (&)[]   );
        ASSERT_IS_CONVERTIBLE(true ,    int  [],    C_(int) (&)[]   );
        ASSERT_IS_CONVERTIBLE(false, C_(int) [],       int  (&)[]   );
        ASSERT_IS_CONVERTIBLE(false, C_(int) [],    V_(int) (&)[]   );
        ASSERT_IS_CONVERTIBLE(false, V_(int) [],    C_(int) (&)[]   );

        ASSERT_IS_CONVERTIBLE(false,    int  (*)[],    int  (&)[]   );
        ASSERT_IS_CONVERTIBLE(false,    int  (*)[][5], int  (&)[][5]);
        ASSERT_IS_CONVERTIBLE(false,    int  (*)[], V_(int) (&)[]   );
        ASSERT_IS_CONVERTIBLE(false, V_(int) (*)[],    int  (&)[]   );
        ASSERT_IS_CONVERTIBLE(false,    int  (*)[], C_(int) (&)[]   );
        ASSERT_IS_CONVERTIBLE(false, C_(int) (*)[],    int  (&)[]   );
        ASSERT_IS_CONVERTIBLE(false, C_(int) (*)[], V_(int) (&)[]   );
        ASSERT_IS_CONVERTIBLE(false, V_(int) (*)[], C_(int) (&)[]   );

        ASSERT_IS_CONVERTIBLE(false,    int  (&)[],    int  (*)[]   );
        ASSERT_IS_CONVERTIBLE(false,    int  (&)[][5], int  (*)[][5]);
        ASSERT_IS_CONVERTIBLE(false,    int  (&)[], V_(int) (*)[]   );
        ASSERT_IS_CONVERTIBLE(false, V_(int) (&)[],    int  (*)[]   );
        ASSERT_IS_CONVERTIBLE(false,    int  (&)[], C_(int) (*)[]   );
        ASSERT_IS_CONVERTIBLE(false, C_(int) (&)[],    int  (*)[]   );
        ASSERT_IS_CONVERTIBLE(false, C_(int) (&)[], V_(int) (*)[]   );
        ASSERT_IS_CONVERTIBLE(false, V_(int) (&)[], C_(int) (*)[]   );
# endif // BSLMF_ISCONVERTIBLE_NO_ARRAY_REF_OF_UNKNOWN_BOUND
#endif  // BSLMF_ISCONVERTIBLE_NO_ARRAY_OF_UNKNOWN_BOUND_AS_TEMPLATE_PARAMETER

        // C-13: Test function references decay to function pointers

        ASSERT_IS_CONVERTIBLE(false, void(),      void  );
        ASSERT_IS_CONVERTIBLE(false, void(),      int   );
        ASSERT_IS_CONVERTIBLE(false, void(),   C_(void) );
        ASSERT_IS_CONVERTIBLE(false, void(),   V_(int)  );
        ASSERT_IS_CONVERTIBLE(false, void,     void());
        ASSERT_IS_CONVERTIBLE(false, int,      void());
        ASSERT_IS_CONVERTIBLE(false, C_(void), void());
        ASSERT_IS_CONVERTIBLE(false, V_(int),  void());

        ASSERT_IS_CONVERTIBLE(false, void    (), void    ());
        ASSERT_IS_CONVERTIBLE(true,  void    (), void (&)());
        ASSERT_IS_CONVERTIBLE(true,  void    (), void (*)());
        ASSERT_IS_CONVERTIBLE(true,  void (&)(), void (&)());
        ASSERT_IS_CONVERTIBLE(true,  void (&)(), void (*)());
        ASSERT_IS_CONVERTIBLE(false, void (*)(), void    ());
        ASSERT_IS_CONVERTIBLE(false, void (*)(), void (&)());
        ASSERT_IS_CONVERTIBLE(false, void (&)(), void    ());
        ASSERT_IS_CONVERTIBLE(true,  void (*)(), void (*)());

        ASSERT_IS_CONVERTIBLE(false, void(...), void);
        ASSERT_IS_CONVERTIBLE(false, void(...), int );
        ASSERT_IS_CONVERTIBLE(false, void(...), C_(void));
        ASSERT_IS_CONVERTIBLE(false, void(...), V_(int) );
        ASSERT_IS_CONVERTIBLE(false, void,      void(...));
        ASSERT_IS_CONVERTIBLE(false, int,       void(...));
        ASSERT_IS_CONVERTIBLE(false, C_(void),  void(...));
        ASSERT_IS_CONVERTIBLE(false, V_(int),   void(...));

        ASSERT_IS_CONVERTIBLE(false, void    (...), void    (...));
        ASSERT_IS_CONVERTIBLE(true,  void    (...), void (&)(...));
        ASSERT_IS_CONVERTIBLE(true,  void    (...), void (*)(...));
        ASSERT_IS_CONVERTIBLE(true,  void (&)(...), void (&)(...));
        ASSERT_IS_CONVERTIBLE(true,  void (&)(...), void (*)(...));
        ASSERT_IS_CONVERTIBLE(false, void (*)(...), void    (...));
        ASSERT_IS_CONVERTIBLE(false, void (*)(...), void (&)(...));
        ASSERT_IS_CONVERTIBLE(false, void (&)(...), void    (...));

        ASSERT_IS_CONVERTIBLE(false, void    (...), void    ());
        ASSERT_IS_CONVERTIBLE(false, void    (...), void (&)());
        ASSERT_IS_CONVERTIBLE(false, void    (...), void (*)());
        ASSERT_IS_CONVERTIBLE(false, void (&)(...), void (&)());
        ASSERT_IS_CONVERTIBLE(false, void (&)(...), void (*)());
        ASSERT_IS_CONVERTIBLE(false, void (*)(...), void    ());
        ASSERT_IS_CONVERTIBLE(false, void (*)(...), void (&)());
        ASSERT_IS_CONVERTIBLE(false, void (&)(...), void    ());

        ASSERT_IS_CONVERTIBLE(false, void    (), void    (...));
        ASSERT_IS_CONVERTIBLE(false, void    (), void (&)(...));
        ASSERT_IS_CONVERTIBLE(false, void    (), void (*)(...));
        ASSERT_IS_CONVERTIBLE(false, void (&)(), void (&)(...));
        ASSERT_IS_CONVERTIBLE(false, void (&)(), void (*)(...));
        ASSERT_IS_CONVERTIBLE(false, void (*)(), void    (...));
        ASSERT_IS_CONVERTIBLE(false, void (*)(), void (&)(...));
        ASSERT_IS_CONVERTIBLE(false, void (&)(), void    (...));

        ASSERT_IS_CONVERTIBLE(false, int(char, float...), void);
        ASSERT_IS_CONVERTIBLE(false, int(char, float...),  int);
        ASSERT_IS_CONVERTIBLE(false, void, int(char, float...));
        ASSERT_IS_CONVERTIBLE(false, int,  int(char, float...));

        ASSERT_IS_CONVERTIBLE(false, int(char, float...),
                                     int(char, float...));
        ASSERT_IS_CONVERTIBLE(true,  int   (char, float...),
                                     int(&)(char, float...));
        ASSERT_IS_CONVERTIBLE(true,  int   (char, float...),
                                     int(*)(char, float...));

#undef ASSERT_IS_CONVERTIBLE
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
