// bslmf_isconvertible.t.cpp                                          -*-C++-*-
#include <bslmf_isconvertible.h>

#include <bsls_bsltestutil.h>
#include <bsls_platform.h>

#include <cstdio>
#include <cstdlib>     // 'atoi'
#include <cstring>     // 'strcmp'

// These 4 compilers cannot handle conversions to/from cv-qualified types
// in all cases.
#if defined(BSLS_PLATFORM_CMP_SUN)  \
 || defined(BSLS_PLATFORM_CMP_MSVC) \
 || defined(BSLS_PLATFORM_CMP_HP)   \
 || defined(BSLS_PLATFORM_CMP_CLANG)
    #define BSLMF_ODD_COMPILER_CONST_OR_VOLATILE_CONVERSION_RULES
#endif

using namespace BloombergLP;
using namespace std;

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
// 'bslmf::IsConvertible as that for 'bsl::is_convertible', and an additional
// set of incomplete types for testing 'bslmf::IsConvertible' alone.
//
//-----------------------------------------------------------------------------
// PUBLIC CLASS DATA
// [ 1] bsl::is_convertible::value
// [ 2] bslmf::IsConvertible::VALUE
// [ 3] Testing GCC Warnings Suppression
// [ 4] Testing GCC Warnings Suppression Via a User-Defined Class
//
//-----------------------------------------------------------------------------
// [ 5] USAGE EXAMPLE

//=============================================================================
//                       STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
// NOTE: THIS IS A LOW-LEVEL COMPONENT AND MAY NOT USE ANY C++ LIBRARY
// FUNCTIONS, INCLUDING IOSTREAMS.
static int testStatus = 0;

void aSsErT(bool b, const char *s, int i)
{
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
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

template <class TYPE>
class ConvertibleTo {
    // This class is convertible to 'TYPE'.

    // DATA
    TYPE d_value;

  public:
    // CREATORS
    ConvertibleTo();  // declared but not defined

    operator TYPE const&() const { return d_value; }
    operator TYPE&() { return d_value; }
    operator TYPE() const { return d_value; }
};

template <class TYPE>
class ConvertibleFrom {
    // This class is convertible from 'TYPE'.

    // DATA
    TYPE d_value;

  public:
    // CREATORS
    ConvertibleFrom() : d_value() { }
    ConvertibleFrom(TYPE value) : d_value(value) { }
};

class my_Class {
    // This class is convertible from 'int'.

    // DATA
    int d_i;

  public:
    // CREATORS
    my_Class(int i) : d_i(i) {}
};

class my_OtherClass {
    // This class is convertible to 'my_Class'.

  public:
    operator my_Class&();
};

class my_ThirdClass {
    // This class is convertible from 'my_Class'.

  public:
    my_ThirdClass(const my_Class&);
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

class my_IncompleteClass;   // incomplete class
class my_IncompleteClass2;  // incomplete class

enum my_Enum { MY_VAL0, MY_VAL1 };

class my_EnumClass {
    // This class defines nested 'enum' type.
  public:
    enum Type { VAL0, VAL1 };
};

class my_BslmaAllocator;

template <class TYPE>
class my_StlAllocator {
    // This class is convertible from 'my_BslmaAllocator*'.

  public:
    // CREATORS
    my_StlAllocator(my_BslmaAllocator*);
};

struct my_PlacementNew {
    void *d_p; my_PlacementNew(void *p) : d_p(p) {}
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

///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: Select Function Based on Type Convertibility
///- - - - - - - - - - - - - - - - - - - - - - - - - - - -
// The 'bsl::is_convertible' meta-function can be used to select an
// appropriate function (at compile time) based on the convertibility of one
// type to another without causing a compiler error by actually trying the
// conversion.  This implementation technique is especially useful when
// building generic containers that use an allocator protocol to acquire
// resources.  As a design goal, we want to pass the container's allocator to
// contained types if they provide an appropriate constructor.
//
// Suppose we are implementing some container's 'addObject' method that adds a
// new object (in its default state) of the container's template parameter
// 'TYPE'.  The method calls an overloaded function, 'createObject', to create
// a new object of the template parameter type in its internal array.  The idea
// is to invoke one version of 'createObject' if the type provides a
// constructor that takes a pointer to an allocator as its sole argument, and
// another version if the type provides only a default constructor.
//
// First, we define the allocator to be used:
//..
struct MyAllocator {
    // This is a user-defined allocator.

    void *allocate(std::size_t sz)
    {
        return ::operator new(sz);
    }

    void  deallocate(void *address)
    {
        ::operator delete(address);
    }
};
//..
// Then, we define two 'struct's, 'Foo' and 'Bar'.  The constructor of 'Foo'
// takes a 'MyAllocator' object pointer while that of 'Bar' does not:
//..
struct Foo {
    Foo(MyAllocator *) {}
};

struct Bar {
    Bar() {}
};
//..
// Next, we define the first 'createObject' function that takes a
// 'bsl::false_type' as its last argument, whereas the second 'createObject'
// function takes a 'bsl::true_type' object.  The result of the
// 'bsl::is_convertible' meta-function (i.e., its 'type' member) is used to
// create the last argument passed to 'createObject'.  Neither version of
// 'createObject' makes use of this argument -- it is used only to
// differentiate the argument list so we can overload the function.
//..
template <class TYPE>
void createObject(TYPE *space, MyAllocator *, bsl::false_type)
{
    // Create an object of the (template parameter) 'TYPE' using its
    // default constructor at the specified memory address 'space'.

    new (space) TYPE();
}

template <class TYPE>
void createObject(TYPE *space, MyAllocator *allocator, bsl::true_type)
{
    // Create an object of the (template parameter) 'TYPE' using the
    // specified 'allocator' at the specified memory address 'space'.

    new (space) TYPE(allocator);
}
//..
// Now, we define our 'MyContainer' type and implement its 'addObject' method:
//..
template <class TYPE>
class MyContainer {
    // DATA
    TYPE        *d_array_p;  // underlying array
    int          d_length;   // logical length of array
    MyAllocator *d_alloc_p;  // allocator protocol

    // ...

    void resizeInternalArrayIfNeeded() { /* ... */ };

  public:
    // CREATORS
    MyContainer(MyAllocator *allocator)
    : d_alloc_p(allocator)
    , d_length(0)
    {
        d_array_p
             = reinterpret_cast<TYPE *>(d_alloc_p->allocate(sizeof(TYPE)));
    }

    ~MyContainer()
    {
        d_alloc_p->deallocate(d_array_p);
    }

    // MANIPULATORS
    void addObject()
    {
        resizeInternalArrayIfNeeded();

        typedef typename bsl::is_convertible<MyAllocator *, TYPE>::type
                                                            TakesAllocator;
        createObject(d_array_p + d_length, d_alloc_p, TakesAllocator());
        ++d_length;
    }
};
//..
// Notice that in the 'addObject' method we use 'bsl::is_convertible' to get a
// 'bsl::false_type' or 'bsl::true_type', and then call the corresponding
// overloaded 'createObject' method.

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? std::atoi(argv[1]) : 0;
    int verbose = argc > 2;
//  int veryVerbose = argc > 3;

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
                            "\n=============\n");

//
// Finally, we instantiate 'MyContainer' with both 'Foo' and 'Bar' types, and
// call 'addObject' on both containers:
//..
    MyAllocator a;

    MyContainer<Foo> fc(&a);
    fc.addObject();

    MyContainer<Bar> bc(&a);
    bc.addObject();
//..

      } break;
      case 4: {
        // --------------------------------------------------------------------
        // Testing GCC Warnings Suppression Via a User-Defined Class
        //
        // Concern:
        //: 1 GCC should not generate a warning for integral to floating-point
        //:   type conversions via a user-defined class.
        //
        // Plan:
        //   Instantiate 'bsl::is_convertible' with various fundamental type
        //   combinations and verify that the 'value' member is initialized
        //   properly, and (manually) verify that no warning is generated for
        //   conversions between floating-point types and integral types.
        //   (C-1)
        //
        // Testing:
        //   bsl::is_convertible::value
        // --------------------------------------------------------------------

        if (verbose)
            printf(
                "Testing GCC Warnings Suppression Via a User-Defined Class\n"
                "=========================================================\n");

        // Conversion of basic types via two user-defined classes returns
        // 'false'.

        ASSERT(false == (bsl::is_convertible<ConvertibleFrom<int>,
                                             ConvertibleTo<int> >::value));

        // Test conversion of basic types via a user-defined class.

        ASSERT(true ==
                   (bsl::is_convertible<ConvertibleTo<int>, float>::value));
        ASSERT(true ==
                   (bsl::is_convertible<ConvertibleTo<float>, int>::value));
        ASSERT(true ==
                   (bsl::is_convertible<int, ConvertibleFrom<float> >::value));
        ASSERT(true ==
                   (bsl::is_convertible<float, ConvertibleFrom<int> >::value));

        // Test 'const' type conversions via a user-defined class.

        ASSERT(true ==
                (bsl::is_convertible<ConvertibleTo<int>, const float>::value));
        ASSERT(true ==
                (bsl::is_convertible<ConvertibleTo<float>, const int>::value));
        ASSERT(true ==
                (bsl::is_convertible<ConvertibleTo<const float>, int>::value));
        ASSERT(true ==
                (bsl::is_convertible<ConvertibleTo<const int>, float>::value));
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
             (bsl::is_convertible<ConvertibleTo<int>, volatile float>::value));
        ASSERT(true ==
             (bsl::is_convertible<ConvertibleTo<float>, volatile int>::value));
        ASSERT(true ==
             (bsl::is_convertible<ConvertibleTo<volatile int>, float>::value));
        ASSERT(true ==
             (bsl::is_convertible<ConvertibleTo<volatile float>, int>::value));
        ASSERT(true ==
          (bsl::is_convertible<int, ConvertibleFrom<volatile float> >::value));
        ASSERT(true ==
          (bsl::is_convertible<float, ConvertibleFrom<volatile int> >::value));
        ASSERT(true ==
          (bsl::is_convertible<volatile int, ConvertibleFrom<float> >::value));
        ASSERT(true ==
          (bsl::is_convertible<volatile float, ConvertibleFrom<int> >::value));
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // Testing GCC Warnings Suppression
        //
        // Concern:
        //: 1  GCC should not generate a warning for implicit integral to
        //:    floating-point type conversions.
        //
        // Plan:
        //   Instantiate 'bsl::is_convertible' with various fundamental type
        //   combinations and verify that the 'value' member is initialized
        //   properly, and (manually) verify no warning is generated for
        //   conversions between floating-point types and integral types.
        //   (C-1)
        //
        // Testing:
        //   bsl::is_convertible::value
        // --------------------------------------------------------------------

        if (verbose)
            printf("Testing GCC Warnings Suppression\n"
                   "================================\n");

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
        ASSERT(true == (bsl::is_convertible<float, volatile int>::value));
        ASSERT(true == (bsl::is_convertible<volatile int, float>::value));
        ASSERT(true == (bsl::is_convertible<volatile float, int>::value));

        // Test 'volatile' pointer and reference conversions from integral to
        // floating-point.

        ASSERT(false == (bsl::is_convertible<int*, volatile float*>::value));
        ASSERT(false == (bsl::is_convertible<volatile int*, float*>::value));
        ASSERT(false == (bsl::is_convertible<int&, volatile float&>::value));
        ASSERT(false == (bsl::is_convertible<volatile int&, float&>::value));
        ASSERT(false == (bsl::is_convertible<int,  volatile float&>::value));
        ASSERT(false == (bsl::is_convertible<volatile int,  float&>::value));
        ASSERT(true  == (bsl::is_convertible<int&,  volatile float>::value));
        ASSERT(true  == (bsl::is_convertible<volatile int&,  float>::value));

        ASSERT(false ==
                    (bsl::is_convertible<int*, const volatile float*>::value));
        ASSERT(false ==
                    (bsl::is_convertible<const volatile int*, float*>::value));
#if !defined(BSLMF_ODD_COMPILER_CONST_OR_VOLATILE_CONVERSION_RULES)
        ASSERT(false ==
                    (bsl::is_convertible<int&, const volatile float&>::value));
#endif
        ASSERT(false ==
                    (bsl::is_convertible<const volatile int&, float&>::value));
#if !defined(BSLMF_ODD_COMPILER_CONST_OR_VOLATILE_CONVERSION_RULES)
        ASSERT(false ==
                    (bsl::is_convertible<int,  const volatile float&>::value));
#endif
        ASSERT(false ==
                     (bsl::is_convertible<const volatile int, float&>::value));
        ASSERT(true  ==
                     (bsl::is_convertible<int&, const volatile float>::value));
        ASSERT(true  ==
                     (bsl::is_convertible<const volatile int&, float>::value));

        ASSERT(false ==
                    (bsl::is_convertible<const int*, volatile float*>::value));
        ASSERT(false ==
                    (bsl::is_convertible<volatile int*, const float*>::value));
        ASSERT(true  ==
                    (bsl::is_convertible<const int&, volatile float>::value));
        ASSERT(true  ==
                    (bsl::is_convertible<volatile int&, const float>::value));
#if !defined(BSLMF_ODD_COMPILER_CONST_OR_VOLATILE_CONVERSION_RULES)
        ASSERT(false ==
           (bsl::is_convertible<   const int&, const volatile float&>::value));
        ASSERT(false ==
           (bsl::is_convertible<volatile int&, const volatile float&>::value));
        ASSERT(false ==
           (bsl::is_convertible<   const int,  const volatile float&>::value));
        ASSERT(false ==
           (bsl::is_convertible<volatile int,  const volatile float&>::value));
#endif
        ASSERT(false ==
            (bsl::is_convertible<const          int, volatile float&>::value));
        ASSERT(false ==
            (bsl::is_convertible<const volatile int, volatile float&>::value));

        // Sun 5.2 and 5.5 get this right if the cv-unqualified types differ.

        ASSERT(true ==
               (bsl::is_convertible<      volatile int, const float&>::value));
        ASSERT(true ==
               (bsl::is_convertible<const volatile int, const float&>::value));

        ASSERT(false == (bsl::is_convertible<int*, float>::value));
        ASSERT(false == (bsl::is_convertible<int, float*>::value));

        // Test volatile pointer and reference conversions
        // from floating-point to integral.
        ASSERT(false == (bsl::is_convertible<float*, volatile int*>::value));
        ASSERT(false == (bsl::is_convertible<volatile float*, int*>::value));
        ASSERT(false == (bsl::is_convertible<float&, volatile int&>::value));
        ASSERT(false == (bsl::is_convertible<volatile float&, int&>::value));
        ASSERT(false == (bsl::is_convertible<float,  volatile int&>::value));
        ASSERT(false == (bsl::is_convertible<volatile float,  int&>::value));
        ASSERT(true  == (bsl::is_convertible<float&,  volatile int>::value));
        ASSERT(true  == (bsl::is_convertible<volatile float&,  int>::value));

        ASSERT(false ==
                    (bsl::is_convertible<float*, const volatile int*>::value));
        ASSERT(false ==
                    (bsl::is_convertible<const volatile float*, int*>::value));
#if !defined(BSLMF_ODD_COMPILER_CONST_OR_VOLATILE_CONVERSION_RULES)
        ASSERT(false ==
                    (bsl::is_convertible<float&, const volatile int&>::value));
#endif
        ASSERT(false ==
                    (bsl::is_convertible<const volatile float&, int&>::value));
#if !defined(BSLMF_ODD_COMPILER_CONST_OR_VOLATILE_CONVERSION_RULES)
        ASSERT(false ==
                    (bsl::is_convertible<float,  const volatile int&>::value));
#endif
        ASSERT(false ==
                    (bsl::is_convertible<const volatile float,  int&>::value));
        ASSERT(true  ==
                    (bsl::is_convertible<float&, const volatile int >::value));
        ASSERT(true  ==
                    (bsl::is_convertible<const volatile float&, int >::value));

        ASSERT(false ==
                    (bsl::is_convertible<const float*, volatile int*>::value));
        ASSERT(false ==
                    (bsl::is_convertible<volatile float*, const int*>::value));
        ASSERT(true  ==
                    (bsl::is_convertible<const float&, volatile int >::value));
        ASSERT(true  ==
                    (bsl::is_convertible<volatile float&, const int >::value));
#if !defined(BSLMF_ODD_COMPILER_CONST_OR_VOLATILE_CONVERSION_RULES)
        ASSERT(false ==
           (bsl::is_convertible<const    float&, const volatile int&>::value));
        ASSERT(false ==
           (bsl::is_convertible<volatile float&, const volatile int&>::value));
        ASSERT(false ==
           (bsl::is_convertible<const    float , const volatile int&>::value));
        ASSERT(false ==
           (bsl::is_convertible<volatile float , const volatile int&>::value));
#endif
        ASSERT(false ==
            (bsl::is_convertible<const          float, volatile int&>::value));
        ASSERT(false ==
            (bsl::is_convertible<const volatile float, volatile int&>::value));

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
        // Concerns
        //: 1 'IsConvertible::VALUE' returns the correct value when both
        //:   'FROM_TYPE' and 'TO_TYPE' are basic types.
        //:
        //: 2 'IsConvertible::VALUE' returns the correct value when one of
        //:   'FROM_TYPE' and 'TO_TYPE' is 'const' type.
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
        //
        // Plan:
        //   Instantiate 'bslmf::IsConvertible' with various type combinations
        //   and verify that the 'VALUE' member is initialized properly.
        //   (C-1..13)
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
        ASSERT(1 == (bslmf::IsConvertible<int,  int&>::VALUE));
        ASSERT(1 == (bslmf::IsConvertible<int,  const int&>::VALUE));
        ASSERT(0 == (bslmf::IsConvertible<const int,  int&>::VALUE));
        ASSERT(1 == (bslmf::IsConvertible<int&,  const int>::VALUE));
        ASSERT(1 == (bslmf::IsConvertible<const int&, int>::VALUE));
        ASSERT(0 == (bslmf::IsConvertible<const int,  int&>::VALUE));
        ASSERT(1 == (bslmf::IsConvertible<const int,  const int&>::VALUE));
        ASSERT(1 == (bslmf::IsConvertible<const int&, const int>::VALUE));

        // C-4: Test volatile value conversions.

        ASSERT(1 == (bslmf::IsConvertible<int, volatile int>::VALUE));
        ASSERT(1 == (bslmf::IsConvertible<volatile int, int>::VALUE));

        // C-5: Test volatile pointer and reference conversions

        ASSERT(1 == (bslmf::IsConvertible<int*, volatile int*>::VALUE));
        ASSERT(0 == (bslmf::IsConvertible<volatile int*, int*>::VALUE));
        ASSERT(1 == (bslmf::IsConvertible<int&, volatile int&>::VALUE));
        ASSERT(0 == (bslmf::IsConvertible<volatile int&, int&>::VALUE));
        ASSERT(1 == (bslmf::IsConvertible<int,  volatile int&>::VALUE));
        ASSERT(0 == (bslmf::IsConvertible<volatile int,  int&>::VALUE));
        ASSERT(1 == (bslmf::IsConvertible<int&,  volatile int>::VALUE));
        ASSERT(1 == (bslmf::IsConvertible<volatile int&,  int>::VALUE));

        // C-6: Test conversions on different combinations of cv-qualified
        //      types.

        ASSERT(1 == (bslmf::IsConvertible<int*, const volatile int*>::VALUE));
        ASSERT(0 == (bslmf::IsConvertible<const volatile int*, int*>::VALUE));
        ASSERT(1 == (bslmf::IsConvertible<int&, const volatile int&>::VALUE));
        ASSERT(0 == (bslmf::IsConvertible<const volatile int&, int&>::VALUE));
        ASSERT(1 == (bslmf::IsConvertible<int,  const volatile int&>::VALUE));
        ASSERT(0 == (bslmf::IsConvertible<const volatile int,  int&>::VALUE));
        ASSERT(1 == (bslmf::IsConvertible<int&,  const volatile int>::VALUE));
        ASSERT(1 == (bslmf::IsConvertible<const volatile int&,  int>::VALUE));

        ASSERT(0 == (bslmf::IsConvertible<const int*, volatile int*>::VALUE));
        ASSERT(0 == (bslmf::IsConvertible<volatile int*, const int*>::VALUE));
        ASSERT(1 == (bslmf::IsConvertible<const int&, volatile int>::VALUE));
        ASSERT(1 == (bslmf::IsConvertible<volatile int&, const int>::VALUE));
        ASSERT(1 == (bslmf::IsConvertible<const int&,
                                          const volatile int&>::VALUE));
        ASSERT(1 == (bslmf::IsConvertible<volatile int&,
                                          const volatile int&>::VALUE));
        ASSERT(1 == (bslmf::IsConvertible<const int,
                                          const volatile int&>::VALUE));
        ASSERT(1 == (bslmf::IsConvertible<volatile int,
                                          const volatile int&>::VALUE));
        ASSERT(0 == (bslmf::IsConvertible<const int,
                                          volatile int&>::VALUE));
        ASSERT(0 == (bslmf::IsConvertible<const volatile int,
                                          volatile int&>::VALUE));

#if !defined(BSLS_PLATFORM_CMP_SUN)
        // Sun 5.2 and 5.5 both get this wrong when the cv-unqualified types
        // are the same.
        ASSERT(0 == (bslmf::IsConvertible<volatile int,
                                          const int&>::VALUE));
        ASSERT(0 == (bslmf::IsConvertible<const volatile int,
                                          const int&>::VALUE));
#endif
        // C-7: Test conversions on different combinations of cv-qualified
        //      user-defined types.

        ASSERT(0 == (bslmf::IsConvertible<int*, int>::VALUE));
        ASSERT(0 == (bslmf::IsConvertible<int, int*>::VALUE));

        ASSERT(1 == (bslmf::IsConvertible<MyAllocator*, MyAllocator*>::VALUE));
        ASSERT(0 == (bslmf::IsConvertible<MyAllocator*, int*>::VALUE));

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

        // C-9: Test derived-to-base convertibility

        ASSERT(1 == (bslmf::IsConvertible<my_DerivedClass,
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
                                       int (*)[], volatile int (*)[]>::VALUE));
        ASSERT(0 == (bslmf::IsConvertible<
                              volatile int (*)[],          int (*)[]>::VALUE));
        ASSERT(1 == (bslmf::IsConvertible<
                                       int (*)[], const    int (*)[]>::VALUE));
        ASSERT(0 == (bslmf::IsConvertible<
                              const    int (*)[],          int (*)[]>::VALUE));
        ASSERT(0 == (bslmf::IsConvertible<
                              const    int (*)[], volatile int (*)[]>::VALUE));
        ASSERT(0 == (bslmf::IsConvertible<
                              volatile int (*)[], const    int (*)[]>::VALUE));

        // C-13

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
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // 'bsl::is_convertible::value'
        //
        // Concerns
        //: 1 'is_convertible::value' returns the correct value when both
        //:   'FROM_TYPE' and 'TO_TYPE' are basic types.
        //:
        //: 2 'is_convertible::value' returns the correct value when one of
        //:   'FROM_TYPE' and 'TO_TYPE' is a 'const' type.
        //:
        //: 3 'is_convertible::value' returns the correct value when one of
        //:   'FROM_TYPE' and 'TO_TYPE' is  a'const' pointer or 'const'
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
        //   Instantiate 'bsl::is_convertible' with various type combinations
        //   and verify that the 'VALUE' member is initialized properly.
        //   (C-1..11)
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

        ASSERT(true  == (bsl::is_convertible<int,   int  >::value));
        ASSERT(true  == (bsl::is_convertible<int,   char >::value));
        ASSERT(false == (bsl::is_convertible<int,   char*>::value));
        ASSERT(false == (bsl::is_convertible<void*, char*>::value));
        ASSERT(true  == (bsl::is_convertible<char*, void*>::value));

        // C-2: Test const value conversions.

        ASSERT(true == (bsl::is_convertible<      int, const int>::value));
        ASSERT(true == (bsl::is_convertible<const int,       int>::value));

        // C-3: Test const pointer and reference conversions.

        ASSERT(true  == (bsl::is_convertible<      int*, const int*>::value));
        ASSERT(false == (bsl::is_convertible<const int*,       int*>::value));

        ASSERT(true  == (bsl::is_convertible<      int&, const int&>::value));
        ASSERT(false == (bsl::is_convertible<const int&,       int&>::value));

        ASSERT(true  == (bsl::is_convertible<      int,        int&>::value));
        ASSERT(true  == (bsl::is_convertible<      int,  const int&>::value));

        ASSERT(false == (bsl::is_convertible<const int,         int&>::value));
        ASSERT(true  == (bsl::is_convertible<      int&,  const int >::value));

        ASSERT(true  == (bsl::is_convertible<const int&, int >::value));
        ASSERT(false == (bsl::is_convertible<const int,  int&>::value));

        ASSERT(true  == (bsl::is_convertible<const int,  const int&>::value));
        ASSERT(true  == (bsl::is_convertible<const int&, const int >::value));

        // C-4: Test volatile value conversions.

        ASSERT(true == (bsl::is_convertible<int, volatile int>::value));
        ASSERT(true == (bsl::is_convertible<volatile int, int>::value));

        // C-5: Test volatile pointer and reference conversions

        ASSERT(true  == (bsl::is_convertible<int*, volatile int*>::value));
        ASSERT(false == (bsl::is_convertible<volatile int*, int*>::value));

        ASSERT(true  == (bsl::is_convertible<int&, volatile int&>::value));
        ASSERT(false == (bsl::is_convertible<volatile int&, int&>::value));

        ASSERT(true  == (bsl::is_convertible<int,  volatile int&>::value));
        ASSERT(false == (bsl::is_convertible<volatile int,  int&>::value));

        ASSERT(true  == (bsl::is_convertible<int&,  volatile int>::value));
        ASSERT(true  == (bsl::is_convertible<volatile int&,  int>::value));

        // C-6: Test conversions on different combinations of cv-qualified
        //      types.

        ASSERT(true  ==
                      (bsl::is_convertible<int*, const volatile int*>::value));
        ASSERT(false ==
                      (bsl::is_convertible<const volatile int*, int*>::value));
        ASSERT(true  ==
                      (bsl::is_convertible<int&, const volatile int&>::value));
        ASSERT(false ==
                      (bsl::is_convertible<const volatile int&, int&>::value));
        ASSERT(true  ==
                      (bsl::is_convertible<int,  const volatile int&>::value));
        ASSERT(false ==
                      (bsl::is_convertible<const volatile int,  int&>::value));
        ASSERT(true  ==
                      (bsl::is_convertible<int&,  const volatile int>::value));
        ASSERT(true  ==
                      (bsl::is_convertible<const volatile int&,  int>::value));
        ASSERT(false ==
                      (bsl::is_convertible<const int*, volatile int*>::value));
        ASSERT(false ==
                      (bsl::is_convertible<volatile int*, const int*>::value));
        ASSERT(true  ==
                       (bsl::is_convertible<const int&, volatile int>::value));
        ASSERT(true  ==
                       (bsl::is_convertible<volatile int&, const int>::value));
        ASSERT(true  ==
             (bsl::is_convertible<   const int&, const volatile int&>::value));
        ASSERT(true  ==
             (bsl::is_convertible<volatile int&, const volatile int&>::value));
        ASSERT(true  ==
             (bsl::is_convertible<   const int,  const volatile int&>::value));
        ASSERT(true  ==
             (bsl::is_convertible<volatile int,  const volatile int&>::value));
        ASSERT(false ==
             (bsl::is_convertible<   const int,        volatile int&>::value));
        ASSERT(false ==
             (bsl::is_convertible<const volatile int, volatile  int&>::value));

#if !defined(BSLS_PLATFORM__CMP_SUN)
        // Sun 5.2 and 5.5 both get this wrong when the cv-unqualified types
        // are the same.
        ASSERT(false == (bsl::is_convertible<volatile int,
                                             const int&>::value));
        ASSERT(false == (bsl::is_convertible<const volatile int,
                                             const int&>::value));
#endif

        // C-7: Test conversions on different combinations of cv-qualified
        //      user-defined types.

        ASSERT(false == (bsl::is_convertible<int*, int>::value));
        ASSERT(false == (bsl::is_convertible<int, int*>::value));

        ASSERT(true  ==
                     (bsl::is_convertible<MyAllocator*, MyAllocator*>::value));
        ASSERT(false ==
                     (bsl::is_convertible<MyAllocator*, int*        >::value));

        ASSERT(true  ==
               (bsl::is_convertible<      my_Class,        my_Class >::value));
        ASSERT(true  ==
               (bsl::is_convertible<      my_Class,  const my_Class >::value));
        ASSERT(true  ==
               (bsl::is_convertible<      my_Class,  const my_Class&>::value));
        ASSERT(false ==
               (bsl::is_convertible<const my_Class,        my_Class&>::value));
        ASSERT(true  ==
               (bsl::is_convertible<const my_Class,  const my_Class&>::value));
        ASSERT(true  ==
               (bsl::is_convertible<const my_Class&, const my_Class >::value));
        ASSERT(false ==
               (bsl::is_convertible<const my_Class,        my_Class&>::value));
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

        // C-8: Test conversion between 'void' type and other types.

        ASSERT(true  == (bsl::is_convertible<void, void>::value));
        ASSERT(false == (bsl::is_convertible<void, int >::value));
        ASSERT(false == (bsl::is_convertible<int,  void>::value));

        // C-9: Test derived-to-base convertibility

        ASSERT(true  == (bsl::is_convertible<my_DerivedClass,
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
                                             int my_DerivedClass::*>::value));
        ASSERT(false == (bsl::is_convertible<int my_DerivedClass::*,
                                             int my_AbstractClass::*>::value));
        ASSERT(false == (bsl::is_convertible<int my_AbstractClass::*,
                                             void*>::value));
        ASSERT(false == (bsl::is_convertible<void*,
                                             int my_AbstractClass::*>::value));

        // C-11: Test pointer-to-member function convertibility

        ASSERT(true  == (bsl::is_convertible<int (my_AbstractClass::*)(int),
                                             int (my_DerivedClass::*)(int)
                                             >::value));
        ASSERT(false == (bsl::is_convertible<int (my_DerivedClass::*)(int),
                                             int (my_AbstractClass::*)(int)
                                             >::value));
        ASSERT(false == (bsl::is_convertible<int (my_AbstractClass::*)(int),
                                             void*>::value));
        ASSERT(false == (bsl::is_convertible<void*,
                                             int (my_AbstractClass::*)(int)
                                             >::value));

        // C-12: Test arrays of unknown bound convertibility

        ASSERT(true  ==
                     (bsl::is_convertible<int (*)[],    int (*)[]   >::value));
        ASSERT(true  ==
                     (bsl::is_convertible<int (*)[][5], int (*)[][5]>::value));
        ASSERT(false ==
                     (bsl::is_convertible<int (*)[],    int*        >::value));
        ASSERT(false ==
                     (bsl::is_convertible<int*,         int (*)[]   >::value));
        ASSERT(false ==
                     (bsl::is_convertible<int (*)[][5], int*        >::value));
        ASSERT(true  ==
         (bsl::is_convertible<         int (*)[], volatile int (*)[]>::value));
        ASSERT(false ==
         (bsl::is_convertible<volatile int (*)[],          int (*)[]>::value));
        ASSERT(true  ==
         (bsl::is_convertible<         int (*)[], const    int (*)[]>::value));
        ASSERT(false ==
         (bsl::is_convertible<const    int (*)[],          int (*)[]>::value));
        ASSERT(false ==
         (bsl::is_convertible<const    int (*)[], volatile int (*)[]>::value));
        ASSERT(false ==
         (bsl::is_convertible<volatile int (*)[], const    int (*)[]>::value));
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
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2002
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ----------------------------------
