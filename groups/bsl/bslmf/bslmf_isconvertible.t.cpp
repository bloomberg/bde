// bslmf_isconvertible.t.cpp                                          -*-C++-*-

#include <bslmf_isconvertible.h>

#include <bsls_platform.h>
#include <bsls_bsltestutil.h>

#include <cstdlib>     // atoi()
#include <cstdio>
#include <cstring>     // strcmp()

// These 4 compilers cannot handle conversions to/from cv-qualified types
// in all cases.
#if defined(BSLS_PLATFORM_CMP_SUN)  \
 || defined(BSLS_PLATFORM_CMP_MSVC) \
 || defined(BSLS_PLATFORM_CMP_HP)   \
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
// The objects under test are two meta-functions, 'bsl::is_pointer' and
// 'bslmf::IsPointer', that determine whether a template parameter type is a
// pointer type.  Thus, we need to ensure that the values returned by these
// meta-functions are correct for each possible category of types.  Since the
// two meta-functions are functionally equivalent, we will use the same set of
// types for both.
//
//-----------------------------------------------------------------------------
// [ 1] bsl::Is_convertible
// [ 2] bslmf::IsConvertible
// [ 3] bslmf::IsConvertible warnings during conversion
// [ 4] bslmf::IsConvertible warnings during conversion
//-----------------------------------------------------------------------------
// [ 5] Usage Example
//

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

template <typename T>
class ConvertibleTo {
    // Object that's convertible to 'T'.
    T d_value;

  public:
    ConvertibleTo();  // Declared but not defined
    operator T const&() const { return d_value; }
    operator T&() { return d_value; }
    operator T() const { return d_value; }
};

template <typename T>
class ConvertibleFrom {
    // Object that's convertible From 'T', for use in name only.
    T d_value;

  public:
    ConvertibleFrom() : d_value() { }
    ConvertibleFrom(T value) : d_value(value) { }
};

class my_Class {
    int d_i;

  public:
    // Convertible from int
    my_Class(int i) : d_i(i) {}
};

class my_OtherClass
{
    // Convertible to my_Class
  public:
    operator my_Class&();
};

class my_ThirdClass
{
    // Convertible from my_Class
  public:
    my_ThirdClass(const my_Class&);
};

class my_AbstractClass
{
    // Abstract class
  public:
    my_AbstractClass() { }
    virtual ~my_AbstractClass() = 0;

    int func1(int);
};

class my_DerivedClass : public my_AbstractClass
{
  public:
    my_DerivedClass() { }
    ~my_DerivedClass();

    int func2(int);
};

class my_IncompleteClass;
class my_IncompleteClass2;

enum my_Enum { MY_VAL0, MY_VAL1 };

class my_EnumClass
{
  public:
    enum Type { VAL0, VAL1 };
};

class my_BslmaAllocator;

template <typename TYPE>
class my_StlAllocator
{
  public:
    my_StlAllocator(my_BslmaAllocator*);
};

// Because this is a very low-level component, we are not allowed to use
// '#include <new>' nor on '#include <bslma::PlacementNew>'.  Thus, we create
// our own placement new that won't conflict with the standard one but does
// the same thing.
struct my_PlacementNew { void *d_p; my_PlacementNew(void *p) : d_p(p) { } };
void *operator new(size_t, my_PlacementNew p) { return p.d_p; }

// from component doc

static const int A = bslmf::IsConvertible<int, char >::value; // A is 1
static const int B = bslmf::IsConvertible<int, char*>::value; // B is 0

// verify that the 'value' member is evaluated at compile-time

static char C0[1 + bslmf::IsConvertible<int, int>::value];     // sz=2
static char C1[1 + bslmf::IsConvertible<int, char>::value];    // sz=2
static char C2[1 + bslmf::IsConvertible<void*, int>::value];   // sz=1
static char C3[1 + bslmf::IsConvertible<int, int *>::value];   // sz=1

//=============================================================================
//                              USAGE EXAMPLES
//-----------------------------------------------------------------------------

///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: Select Function Based on Convertibility
/// - - - - - - - - - - - - - - - - - - - - - - - - -
// This 'bsl::is_convertible' meta-function can be used to select an
// appropriate function (at compile time) based on the convertibility of one
// type to another without causing a compiler error by actually trying the
// conversion.  This implementation technique is especially useful when
// building generic containers that use an allocator protocol to acquire
// resources.  As a design goal, we want to pass the container's allocator to
// contained types if they provide an appropriate constructor.
//
// Suppose we are implementing some container's 'addObj' method that adds a new
// object (in its default state) of the container's template parameter 'TYPE'.
// The method calls an overloaded function 'createObj' to create a new object
// of the parameterized type in its internal array.  The idea is to invoke one
// version of 'createObj' if the type provides a constructor that takes a
// pointer to an allocator as its sole argument, and another version if the
// type provides only a default constructor.
//
// First, we define the allocator to be used:
//..
    struct MyAllocator {
        // This is a user-defined allocator.

        void *allocate(size_t sz)
        {
            return operator new(sz);
        }

        void  deallocate(void *address)
        {
            operator delete(address);
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
// Next, we define the first 'createObj' function that takes a
// 'bsl::false_type' as its last argument, whereas the second 'createObj'
// function takes a 'bsl::true_type' object.  The result of the
// 'bsl::is_convertible' meta-function (i.e., its 'Type' member) is used to
// create the last argument passed to 'createObj'.  Neither version of
// 'createObj' makes use of this argument -- it is used only to differentiate
// the argument list so we can overload the function.
//..
    template<class TYPE>
    void createObj(TYPE *space, MyAllocator *, bsl::false_type)
    {
       // Use the type's default constructor if
       // 'bsl::is_convertible<MyAllocator*, TYPE>::value == false', i.e.,
       // there is no conversion from a 'MyAllocator' pointer to a 'TYPE'.

       new (space) TYPE();
    }

    template<class TYPE>
    void createObj(TYPE *space, MyAllocator *alloc, bsl::true_type)
    {
       // Use the type's constructor that takes a pointer to an allocator if
       // 'bsl::is_convertible<MyAllocator*, TYPE>::value == true', i.e., there
       // is a conversion from a 'MyAllocator' pointer to a 'TYPE'.

       new (space) TYPE(alloc);
    }
//..
// Now, we define our 'MyContainer' type and implement the 'addObj' method:
//..
    template <class TYPE>
    class MyContainer {
        // DATA
        TYPE *d_array_p;         // underlying array

        MyAllocator *d_alloc_p;  // allocator protocol

        int d_length;            // logical length of array

        // ...

        void resizeInternalArrayIfNeeded() { /* ... */ };

      public:
        // CREATORS
        MyContainer(MyAllocator *alloc)
        : d_alloc_p(alloc)
        , d_length(0)
        {
            d_array_p = (TYPE*) d_alloc_p->allocate(sizeof(TYPE));
        }

        ~MyContainer()
        {
            d_alloc_p->deallocate(d_array_p);
        }

        // MANIPULATORS
        void addObj()
        {
            resizeInternalArrayIfNeeded();
            // Work around some Sun's compiler weirdness the code won't compile
            // with just the typename
            typedef typename bsl::is_convertible<MyAllocator*, TYPE>::type
                                                                       isAlloc;
            createObj(d_array_p + d_length++, d_alloc_p, isAlloc());
        }
    };
//..
// Notice that in 'addObj' method we use 'bsl::is_convertible' to get a
// 'bsl::false_type' or 'bsl::true_type', and then call the corresponding
// overloaded 'createObj' method.

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? std::atoi(argv[1]) : 0;
    int verbose = argc > 2;
    // int veryVerbose = argc > 3;

    // Silence compiler warnings about unused static variables
    (void) C0[0];
    (void) C1[0];
    (void) C2[0];
    (void) C3[0];

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
// call 'addObj' on both containers:
//..
    MyAllocator a;

    MyContainer<Foo> fc(&a);
    fc.addObj();

    MyContainer<Bar> bc(&a);
    bc.addObj();
//..

      } break;
      case 4: {
        // --------------------------------------------------------------------
        // 'bslmf::IsConvertible::VALUE'
        //
        // Concern:
        //: 1 Gcc, for example, generates a warning for integral to floating
        //:   point conversions for implicit conversion.  This is vexing,
        //:   because even fixing case 2 below does not take care of the
        //:   user-defined classes convertible to or from fundamental types.
        //
        // Plan:
        //   Instantiate 'bslmf::IsConvertible' with various fundamental type
        //   combinations and verify that the 'VALUE' member is initialized
        //   properly, and that no warning is generated for conversions between
        //   floating point types and integral types.
        //
        // Testing:
        //   bslmf::IsConvertible::VALUE
        // --------------------------------------------------------------------

        if (verbose)
            printf("Testing gcc warnings for float to int conversions"
                   "via a user-defined class\n"
                   "================================================="
                   "========================\n");

        // Two user conversions is one too many.
        ASSERT(0 == (bslmf::IsConvertible<ConvertibleFrom<int>,
                                          ConvertibleTo<int> >::VALUE));

        // Test conversion of basic types via a user-defined class.
        ASSERT(1 == (bslmf::IsConvertible<ConvertibleTo<int>, float>::VALUE));
        ASSERT(1 == (bslmf::IsConvertible<ConvertibleTo<float>, int>::VALUE));
        ASSERT(1 == (bslmf::IsConvertible<int,
                                          ConvertibleFrom<float> >::VALUE));
        ASSERT(1 == (bslmf::IsConvertible<float,
                                          ConvertibleFrom<int> >::VALUE));

        // Test const VALUE conversions via a user-defined class.
        ASSERT(1 == (bslmf::IsConvertible<ConvertibleTo<int>,
                                          const float>::VALUE));
        ASSERT(1 == (bslmf::IsConvertible<ConvertibleTo<float>,
                                          const int>::VALUE));
        ASSERT(1 == (bslmf::IsConvertible<ConvertibleTo<const float>,
                                          int>::VALUE));
        ASSERT(1 == (bslmf::IsConvertible<ConvertibleTo<const int>,
                                          float>::VALUE));
        ASSERT(1 == (bslmf::IsConvertible<int,
                                      ConvertibleFrom<const float> >::VALUE));
        ASSERT(1 == (bslmf::IsConvertible<float,
                                      ConvertibleFrom<const int> >::VALUE));
        ASSERT(1 == (bslmf::IsConvertible<const float,
                                      ConvertibleFrom<int> >::VALUE));
        ASSERT(1 == (bslmf::IsConvertible<const int,
                                      ConvertibleFrom<float> >::VALUE));

        // Test volatile value conversions via a user-defined class.
        ASSERT(1 == (bslmf::IsConvertible<ConvertibleTo<int>,
                                          volatile float>::VALUE));
        ASSERT(1 == (bslmf::IsConvertible<ConvertibleTo<float>,
                                          volatile int>::VALUE));
        ASSERT(1 == (bslmf::IsConvertible<ConvertibleTo<volatile int>,
                                          float>::VALUE));
        ASSERT(1 == (bslmf::IsConvertible<ConvertibleTo<volatile float>,
                                          int>::VALUE));
        ASSERT(1 == (bslmf::IsConvertible<int,
                                  ConvertibleFrom<volatile float> >::VALUE));
        ASSERT(1 == (bslmf::IsConvertible<float,
                                  ConvertibleFrom<volatile int> >::VALUE));
        ASSERT(1 == (bslmf::IsConvertible<volatile int,
                                  ConvertibleFrom<float> >::VALUE));
        ASSERT(1 == (bslmf::IsConvertible<volatile float,
                                  ConvertibleFrom<int> >::VALUE));
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // 'bslmf::IsConvertible::VALUE'
        //
        // Concern:
        //: 1  Gcc, for example, generates a warning for integral to floating
        //:    point conversions for implicit conversion.
        //
        // Plan:
        //   Instantiate 'bslmf::IsConvertible' with various fundamental type
        //   combinations and verify that the 'VALUE' member is initialized
        //   properly, and that no warning is generated for conversions between
        //   floating point types and integral types.
        //
        // Testing:
        //   bslmf::IsConvertible::VALUE
        // --------------------------------------------------------------------

        if (verbose)
            printf("Testing gcc warnings for float to int conversions\n"
                   "=================================================\n");

        // Test conversion of basic types.
        ASSERT(1 == (bslmf::IsConvertible<int, float>::VALUE));
        ASSERT(1 == (bslmf::IsConvertible<float, int>::VALUE));

        // Test const value conversions.
        ASSERT(1 == (bslmf::IsConvertible<int, const float>::VALUE));
        ASSERT(1 == (bslmf::IsConvertible<float, const int>::VALUE));
        ASSERT(1 == (bslmf::IsConvertible<const float, int>::VALUE));
        ASSERT(1 == (bslmf::IsConvertible<const int, float>::VALUE));

        // Test volatile value conversions.
        ASSERT(1 == (bslmf::IsConvertible<int, volatile float>::VALUE));
        ASSERT(1 == (bslmf::IsConvertible<float, volatile int>::VALUE));
        ASSERT(1 == (bslmf::IsConvertible<volatile int, float>::VALUE));
        ASSERT(1 == (bslmf::IsConvertible<volatile float, int>::VALUE));

        // Test volatile pointer and reference conversions
        // from integral to floating point.
        ASSERT(0 == (bslmf::IsConvertible<int*, volatile float*>::VALUE));
        ASSERT(0 == (bslmf::IsConvertible<volatile int*, float*>::VALUE));
        ASSERT(0 == (bslmf::IsConvertible<int&, volatile float&>::VALUE));
        ASSERT(0 == (bslmf::IsConvertible<volatile int&, float&>::VALUE));
        ASSERT(0 == (bslmf::IsConvertible<int,  volatile float&>::VALUE));
        ASSERT(0 == (bslmf::IsConvertible<volatile int,  float&>::VALUE));
        ASSERT(1 == (bslmf::IsConvertible<int&,  volatile float>::VALUE));
        ASSERT(1 == (bslmf::IsConvertible<volatile int&,  float>::VALUE));

        ASSERT(0 == (bslmf::IsConvertible<int*,
                                          const volatile float*>::VALUE));
        ASSERT(0 == (bslmf::IsConvertible<const volatile int*,
                                          float*>::VALUE));
#if !defined(BSLMF_ODD_COMPILER_CONST_OR_VOLATILE_CONVERSION_RULES)
       ASSERT(0 == (bslmf::IsConvertible<int&, const volatile float&>::VALUE));
#endif
       ASSERT(0 == (bslmf::IsConvertible<const volatile int&, float&>::VALUE));
#if !defined(BSLMF_ODD_COMPILER_CONST_OR_VOLATILE_CONVERSION_RULES)
       ASSERT(0 == (bslmf::IsConvertible<int,  const volatile float&>::VALUE));
#endif
       ASSERT(0 == (bslmf::IsConvertible<const volatile int, float&>::VALUE));
        ASSERT(1 == (bslmf::IsConvertible<int&, const volatile float>::VALUE));
        ASSERT(1 == (bslmf::IsConvertible<const volatile int&, float>::VALUE));

        ASSERT(0 == (bslmf::IsConvertible<const int*,
                                          volatile float*>::VALUE));
        ASSERT(0 == (bslmf::IsConvertible<volatile int*,
                                          const float*>::VALUE));
        ASSERT(1 == (bslmf::IsConvertible<const int&, volatile float>::VALUE));
        ASSERT(1 == (bslmf::IsConvertible<volatile int&, const float>::VALUE));
#if !defined(BSLMF_ODD_COMPILER_CONST_OR_VOLATILE_CONVERSION_RULES)
        ASSERT(0 == (bslmf::IsConvertible<const int&,
                                          const volatile float&>::VALUE));
        ASSERT(0 == (bslmf::IsConvertible<volatile int&,
                                          const volatile float&>::VALUE));
        ASSERT(0 == (bslmf::IsConvertible<const int,
                                          const volatile float&>::VALUE));
        ASSERT(0 == (bslmf::IsConvertible<volatile int,
                                          const volatile float&>::VALUE));
#endif
        ASSERT(0 == (bslmf::IsConvertible<const int,
                                          volatile float&>::VALUE));
        ASSERT(0 == (bslmf::IsConvertible<const volatile int,
                                          volatile float&>::VALUE));

        // Sun 5.2 and 5.5 get this right if the cv-unqualified types differ.
        ASSERT(1 == (bslmf::IsConvertible<volatile int,
                                          const float&>::VALUE));
        ASSERT(1 == (bslmf::IsConvertible<const volatile int,
                                          const float&>::VALUE));

        ASSERT(0 == (bslmf::IsConvertible<int*, float>::VALUE));
        ASSERT(0 == (bslmf::IsConvertible<int, float*>::VALUE));

        // Test volatile pointer and reference conversions
        // from floating point to integral.
        ASSERT(0 == (bslmf::IsConvertible<float*, volatile int*>::VALUE));
        ASSERT(0 == (bslmf::IsConvertible<volatile float*, int*>::VALUE));
        ASSERT(0 == (bslmf::IsConvertible<float&, volatile int&>::VALUE));
        ASSERT(0 == (bslmf::IsConvertible<volatile float&, int&>::VALUE));
        ASSERT(0 == (bslmf::IsConvertible<float,  volatile int&>::VALUE));
        ASSERT(0 == (bslmf::IsConvertible<volatile float,  int&>::VALUE));
        ASSERT(1 == (bslmf::IsConvertible<float&,  volatile int>::VALUE));
        ASSERT(1 == (bslmf::IsConvertible<volatile float&,  int>::VALUE));

        ASSERT(0 == (bslmf::IsConvertible<float*,
                                          const volatile int*>::VALUE));
        ASSERT(0 == (bslmf::IsConvertible<const volatile float*,
                                          int*>::VALUE));
#if !defined(BSLMF_ODD_COMPILER_CONST_OR_VOLATILE_CONVERSION_RULES)
       ASSERT(0 == (bslmf::IsConvertible<float&, const volatile int&>::VALUE));
#endif
       ASSERT(0 == (bslmf::IsConvertible<const volatile float&, int&>::VALUE));
#if !defined(BSLMF_ODD_COMPILER_CONST_OR_VOLATILE_CONVERSION_RULES)
       ASSERT(0 == (bslmf::IsConvertible<float,  const volatile int&>::VALUE));
#endif
       ASSERT(0 == (bslmf::IsConvertible<const volatile float,  int&>::VALUE));
        ASSERT(1 == (bslmf::IsConvertible<float&, const volatile int>::VALUE));
        ASSERT(1 == (bslmf::IsConvertible<const volatile float&, int>::VALUE));

        ASSERT(0 == (bslmf::IsConvertible<const float*,
                                          volatile int*>::VALUE));
        ASSERT(0 == (bslmf::IsConvertible<volatile float*,
                                          const int*>::VALUE));
        ASSERT(1 == (bslmf::IsConvertible<const float&, volatile int>::VALUE));
        ASSERT(1 == (bslmf::IsConvertible<volatile float&, const int>::VALUE));
#if !defined(BSLMF_ODD_COMPILER_CONST_OR_VOLATILE_CONVERSION_RULES)
        ASSERT(0 == (bslmf::IsConvertible<const float&,
                                          const volatile int&>::VALUE));
        ASSERT(0 == (bslmf::IsConvertible<volatile float&,
                                          const volatile int&>::VALUE));
        ASSERT(0 == (bslmf::IsConvertible<const float,
                                          const volatile int&>::VALUE));
        ASSERT(0 == (bslmf::IsConvertible<volatile float,
                                          const volatile int&>::VALUE));
#endif
        ASSERT(0 == (bslmf::IsConvertible<const float,
                                          volatile int&>::VALUE));
        ASSERT(0 == (bslmf::IsConvertible<const volatile float,
                                          volatile int&>::VALUE));

// #ifndef BSLS_PLATFORM_CMP_SUN
        // Sun 5.2 and 5.5 both get this wrong
        ASSERT(1 == (bslmf::IsConvertible<volatile float,
                                          const int&>::VALUE));
        ASSERT(1 == (bslmf::IsConvertible<const volatile float,
                                          const int&>::VALUE));
// #endif

        ASSERT(0 == (bslmf::IsConvertible<float*, int>::VALUE));
        ASSERT(0 == (bslmf::IsConvertible<float, int*>::VALUE));

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // 'bslmf::IsConvertible::VALUE'
        //
        // Concerns
        //: 1 'IsConvertible::VALUE' is 'false' when the 'FROM_TYPE' is not
        //:   convertible to 'TO_TYPE'.
        //:
        //: 2 'IsConvertible::VALUE' is 'true' when the 'FROM_TYPE' is
        //:   convertible to 'TO_TYPE'.
        //
        // Plan:
        //   Instantiate 'bslmf::IsConvertible' with various type combinations
        //   and verify that the 'VALUE' member is initialized properly.
        //
        // Testing:
        //   bslmf::IsConvertible::VALUE
        // --------------------------------------------------------------------

        if (verbose) printf("bslmf_IsConvertible\n"
                            "===================\n");

        ASSERT(2 == sizeof(C0));
        ASSERT(2 == sizeof(C1));
        ASSERT(1 == sizeof(C2));
        ASSERT(1 == sizeof(C3));

        // Test conversion of basic types.
        ASSERT(1 == (bslmf::IsConvertible<int, int  >::VALUE));
        ASSERT(1 == (bslmf::IsConvertible<int, char >::VALUE));
        ASSERT(0 == (bslmf::IsConvertible<int, char*>::VALUE));
        ASSERT(0 == (bslmf::IsConvertible<void*, char*>::VALUE));
        ASSERT(1 == (bslmf::IsConvertible<char*, void*>::VALUE));

        // Test const value conversions.
        ASSERT(1 == (bslmf::IsConvertible<int, const int>::VALUE));
        ASSERT(1 == (bslmf::IsConvertible<const int, int>::VALUE));

        // Test const pointer and reference conversions.
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

        // Test volatile value conversions.
        ASSERT(1 == (bslmf::IsConvertible<int, volatile int>::VALUE));
        ASSERT(1 == (bslmf::IsConvertible<volatile int, int>::VALUE));

        // Test volatile pointer and reference conversions
        ASSERT(1 == (bslmf::IsConvertible<int*, volatile int*>::VALUE));
        ASSERT(0 == (bslmf::IsConvertible<volatile int*, int*>::VALUE));
        ASSERT(1 == (bslmf::IsConvertible<int&, volatile int&>::VALUE));
        ASSERT(0 == (bslmf::IsConvertible<volatile int&, int&>::VALUE));
        ASSERT(1 == (bslmf::IsConvertible<int,  volatile int&>::VALUE));
        ASSERT(0 == (bslmf::IsConvertible<volatile int,  int&>::VALUE));
        ASSERT(1 == (bslmf::IsConvertible<int&,  volatile int>::VALUE));
        ASSERT(1 == (bslmf::IsConvertible<volatile int&,  int>::VALUE));

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

        ASSERT(1 == (bslmf::IsConvertible<void, void>::VALUE));
        ASSERT(0 == (bslmf::IsConvertible<void, int>::VALUE));
        ASSERT(0 == (bslmf::IsConvertible<int, void>::VALUE));

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

        // Test abstract class.  Mainly just testing that it compiles at all.
        ASSERT(1 == (bslmf::IsConvertible<my_AbstractClass,
                                          my_AbstractClass>::VALUE));
        ASSERT(1 == (bslmf::IsConvertible<my_AbstractClass,
                                          my_AbstractClass&>::VALUE));
        ASSERT(1 == (bslmf::IsConvertible<my_AbstractClass&,
                                          my_AbstractClass>::VALUE));
        ASSERT(1 == (bslmf::IsConvertible<const my_AbstractClass,
                                          my_AbstractClass>::VALUE));
        ASSERT(0 == (bslmf::IsConvertible<int, my_AbstractClass >::VALUE));
        ASSERT(0 == (bslmf::IsConvertible<int, my_AbstractClass&>::VALUE));
        ASSERT(0 == (bslmf::IsConvertible<my_AbstractClass,  int>::VALUE));
        ASSERT(0 == (bslmf::IsConvertible<my_AbstractClass&, int>::VALUE));

        // Test derived-to-base convertibility
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

        // Test pointer-to-member variable convertibility
        ASSERT(1 == (bslmf::IsConvertible<int my_AbstractClass::*,
                                          int my_DerivedClass::*>::VALUE));
        ASSERT(0 == (bslmf::IsConvertible<int my_DerivedClass::*,
                                          int my_AbstractClass::*>::VALUE));
        ASSERT(0 == (bslmf::IsConvertible<int my_AbstractClass::*,
                                          void*>::VALUE));
        ASSERT(0 == (bslmf::IsConvertible<void*,
                                          int my_AbstractClass::*>::VALUE));

        // Test pointer-to-member function convertibility
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
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // 'bsl::is_convertible::value'
        //
        // Concerns
        //: 1 'is_convertible::value' is 'false' when the 'FROM_TYPE' is not
        //:   convertible to 'TO_TYPE'.
        //:
        //: 2 'is_convertible::value' is 'true' when the 'FROM_TYPE' is
        //:   convertible to 'TO_TYPE'.
        //
        // Plan:
        //   Instantiate 'bslmf::IsConvertible' with various type combinations
        //   and verify that the 'VALUE' member is initialized properly.
        //
        // Testing:
        //   bsl::is_convertible::value
        // --------------------------------------------------------------------

        if (verbose) printf("bslmf_IsConvertible\n"
                            "===================\n");

        ASSERT(2 == sizeof(C0));
        ASSERT(2 == sizeof(C1));
        ASSERT(1 == sizeof(C2));
        ASSERT(1 == sizeof(C3));

        // Test conversion of basic types.
        ASSERT(true  == (bsl::is_convertible<int, int  >::value));
        ASSERT(true  == (bsl::is_convertible<int, char >::value));
        ASSERT(false == (bsl::is_convertible<int, char*>::value));
        ASSERT(false == (bsl::is_convertible<void*, char*>::value));
        ASSERT(true  == (bsl::is_convertible<char*, void*>::value));

        // Test const value conversions.
        ASSERT(true == (bsl::is_convertible<int, const int>::value));
        ASSERT(true == (bsl::is_convertible<const int, int>::value));

        // Test const pointer and reference conversions.
        ASSERT(true  == (bsl::is_convertible<int*, const int*>::value));
        ASSERT(false == (bsl::is_convertible<const int*, int*>::value));
        ASSERT(true  == (bsl::is_convertible<int&, const int&>::value));
        ASSERT(false == (bsl::is_convertible<const int&, int&>::value));
        ASSERT(true  == (bsl::is_convertible<int,  int&>::value));
        ASSERT(true  == (bsl::is_convertible<int,  const int&>::value));
        ASSERT(false == (bsl::is_convertible<const int,  int&>::value));
        ASSERT(true  == (bsl::is_convertible<int&,  const int>::value));
        ASSERT(true  == (bsl::is_convertible<const int&, int>::value));
        ASSERT(false == (bsl::is_convertible<const int,  int&>::value));
        ASSERT(true  == (bsl::is_convertible<const int,  const int&>::value));
        ASSERT(true  == (bsl::is_convertible<const int&, const int>::value));

        // Test volatile value conversions.
        ASSERT(true == (bsl::is_convertible<int, volatile int>::value));
        ASSERT(true == (bsl::is_convertible<volatile int, int>::value));

        // Test volatile pointer and reference conversions
        ASSERT(true  == (bsl::is_convertible<int*, volatile int*>::value));
        ASSERT(false == (bsl::is_convertible<volatile int*, int*>::value));
        ASSERT(true  == (bsl::is_convertible<int&, volatile int&>::value));
        ASSERT(false == (bsl::is_convertible<volatile int&, int&>::value));
        ASSERT(true  == (bsl::is_convertible<int,  volatile int&>::value));
        ASSERT(false == (bsl::is_convertible<volatile int,  int&>::value));
        ASSERT(true  == (bsl::is_convertible<int&,  volatile int>::value));
        ASSERT(true  == (bsl::is_convertible<volatile int&,  int>::value));

        ASSERT(
              true == (bsl::is_convertible<int*, const volatile int*>::value));
        ASSERT(
             false == (bsl::is_convertible<const volatile int*, int*>::value));
        ASSERT(
              true == (bsl::is_convertible<int&, const volatile int&>::value));
        ASSERT(
             false == (bsl::is_convertible<const volatile int&, int&>::value));
        ASSERT(
              true == (bsl::is_convertible<int,  const volatile int&>::value));
        ASSERT(
             false == (bsl::is_convertible<const volatile int,  int&>::value));
        ASSERT(
              true == (bsl::is_convertible<int&,  const volatile int>::value));
        ASSERT(
              true == (bsl::is_convertible<const volatile int&,  int>::value));

        ASSERT(
             false == (bsl::is_convertible<const int*, volatile int*>::value));
        ASSERT(
             false == (bsl::is_convertible<volatile int*, const int*>::value));
        ASSERT(true == (bsl::is_convertible<const int&, volatile int>::value));
        ASSERT(true == (bsl::is_convertible<volatile int&, const int>::value));
        ASSERT(true == (bsl::is_convertible<const int&,
                                            const volatile int&>::value));
        ASSERT(true  == (bsl::is_convertible<volatile int&,
                                             const volatile int&>::value));
        ASSERT(true  == (bsl::is_convertible<const int,
                                             const volatile int&>::value));
        ASSERT(true  == (bsl::is_convertible<volatile int,
                                             const volatile int&>::value));
        ASSERT(false == (bsl::is_convertible<const int,
                                             volatile int&>::value));
        ASSERT(false == (bsl::is_convertible<const volatile int,
                                             volatile int&>::value));

#if !defined(BSLS_PLATFORM__CMP_SUN)
        // Sun 5.2 and 5.5 both get this wrong when the cv-unqualified types
        // are the same.
        ASSERT(false == (bsl::is_convertible<volatile int,
                                             const int&>::value));
        ASSERT(false == (bsl::is_convertible<const volatile int,
                                             const int&>::value));
#endif

        ASSERT(false == (bsl::is_convertible<int*, int>::value));
        ASSERT(false == (bsl::is_convertible<int, int*>::value));

        ASSERT(
             true == (bsl::is_convertible<MyAllocator*, MyAllocator*>::value));
        ASSERT(false == (bsl::is_convertible<MyAllocator*, int*>::value));

        ASSERT(true == (bsl::is_convertible<my_Class, my_Class>::value));
        ASSERT(true == (bsl::is_convertible<my_Class, const my_Class>::value));
        ASSERT(
              true == (bsl::is_convertible<my_Class, const my_Class&>::value));
        ASSERT(
             false == (bsl::is_convertible<const my_Class, my_Class&>::value));
        ASSERT(true == (bsl::is_convertible<const my_Class,
                                            const my_Class&>::value));
        ASSERT(true == (bsl::is_convertible<const my_Class&,
                                            const my_Class>::value));

        ASSERT(
             false == (bsl::is_convertible<const my_Class, my_Class&>::value));
        ASSERT(true == (bsl::is_convertible<int, my_Class>::value));
        ASSERT(false == (bsl::is_convertible<my_Class, int>::value));
        ASSERT(false == (bsl::is_convertible<int*, my_Class*>::value));

        ASSERT(true == (bsl::is_convertible<my_OtherClass, my_Class>::value));
        ASSERT(true == (bsl::is_convertible<my_OtherClass,
                                            const my_Class>::value));

        ASSERT(false == (bsl::is_convertible<const my_OtherClass,
                                             const my_Class>::value));

        ASSERT(true  == (bsl::is_convertible<my_Class, my_ThirdClass>::value));
        ASSERT(true  == (bsl::is_convertible<const my_Class,
                                             my_ThirdClass>::value));
        ASSERT(true  == (bsl::is_convertible<my_Class,
                                             const my_ThirdClass>::value));
        ASSERT(true  == (bsl::is_convertible<const my_Class,
                                            const my_ThirdClass>::value));
        ASSERT(false == (bsl::is_convertible<volatile my_Class,
                                             my_ThirdClass>::value));
        ASSERT(true  == (bsl::is_convertible<my_Class&,
                                             const my_ThirdClass>::value));
        ASSERT(true  == (bsl::is_convertible<const my_Class&,
                                            const my_ThirdClass>::value));
        ASSERT(false == (bsl::is_convertible<volatile my_Class&,
                                             my_ThirdClass>::value));
        ASSERT(true  == (bsl::is_convertible<my_Class&,
                                             const my_ThirdClass&>::value));
        ASSERT(true  == (bsl::is_convertible<const my_Class&,
                                             const my_ThirdClass&>::value));
        ASSERT(false == (bsl::is_convertible<volatile my_Class&,
                                             my_ThirdClass&>::value));

        ASSERT(false == (bsl::is_convertible<my_OtherClass, int>::value));

        ASSERT(true  == (bsl::is_convertible<my_Enum, my_Enum >::value));
        ASSERT(true  == (bsl::is_convertible<my_Enum, int     >::value));
        ASSERT(false == (bsl::is_convertible<int, my_Enum     >::value));

        ASSERT(true  == (bsl::is_convertible<my_Enum,  my_Class>::value));
        ASSERT(false == (bsl::is_convertible<my_Class,  my_Enum>::value));
        ASSERT(false == (bsl::is_convertible<my_Enum, my_Class*>::value));
        ASSERT(false == (bsl::is_convertible<my_Class, my_Enum*>::value));
        ASSERT(false == (bsl::is_convertible<my_Enum*, my_Class>::value));
        ASSERT(false == (bsl::is_convertible<my_Class*, my_Enum>::value));

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

        ASSERT(true  == (bsl::is_convertible<void, void>::value));
        ASSERT(false == (bsl::is_convertible<void, int>::value));
        ASSERT(false == (bsl::is_convertible<int, void>::value));

        // An incomplete class can only be tested as the FROM parameter.
        ASSERT(true  == (bsl::is_convertible<my_IncompleteClass,
                                             my_IncompleteClass>::value));
        ASSERT(false == (bsl::is_convertible<my_IncompleteClass, int>::value));

        // Test references with incomplete types as both TO and FROM parameters
        ASSERT(true  == (bsl::is_convertible<my_IncompleteClass,
                                             my_IncompleteClass&>::value));
        ASSERT(
              false == (bsl::is_convertible<my_IncompleteClass&, int>::value));
        ASSERT(
              false == (bsl::is_convertible<int, my_IncompleteClass&>::value));
        ASSERT(false == (bsl::is_convertible<my_IncompleteClass,
                                             my_IncompleteClass2&>::value));
        ASSERT(false == (bsl::is_convertible<my_IncompleteClass&,
                                             my_IncompleteClass2&>::value));

        // Test abstract class.  Mainly just testing that it compiles at all.
        ASSERT(true  == (bsl::is_convertible<my_AbstractClass,
                                             my_AbstractClass>::value));
        ASSERT(true  == (bsl::is_convertible<my_AbstractClass,
                                             my_AbstractClass&>::value));
        ASSERT(true  == (bsl::is_convertible<my_AbstractClass&,
                                             my_AbstractClass>::value));
        ASSERT(true  == (bsl::is_convertible<const my_AbstractClass,
                                             my_AbstractClass>::value));
        ASSERT(false == (bsl::is_convertible<int, my_AbstractClass >::value));
        ASSERT(false == (bsl::is_convertible<int, my_AbstractClass&>::value));
        ASSERT(false == (bsl::is_convertible<my_AbstractClass,  int>::value));
        ASSERT(false == (bsl::is_convertible<my_AbstractClass&, int>::value));

        // Test derived-to-base convertibility
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

        // Test pointer-to-member variable convertibility
        ASSERT(true  == (bsl::is_convertible<int my_AbstractClass::*,
                                             int my_DerivedClass::*>::value));
        ASSERT(false == (bsl::is_convertible<int my_DerivedClass::*,
                                             int my_AbstractClass::*>::value));
        ASSERT(false == (bsl::is_convertible<int my_AbstractClass::*,
                                             void*>::value));
        ASSERT(false == (bsl::is_convertible<void*,
                                             int my_AbstractClass::*>::value));

        // Test pointer-to-member function convertibility
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
//      Copyright (C) Bloomberg L.P., 2002
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
