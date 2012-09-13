// bslmf_matcharithmetictype.t.cpp                                    -*-C++-*-
#include <bslmf_matcharithmetictype.h>

#include <bslmf_anytype.h>               // testing only (Usage)
#include <bslmf_nil.h>                   // testing only (Usage)

#include <cstdio>
#include <cstdlib>
#include <typeinfo>

using namespace BloombergLP;
using namespace std;

// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
//                             Overview
//                             --------
// The implicity convertibility of integral and floating point types to
// 'bslmf::MatchArithmeticType' is confirmed by creating an object for each
// type of interest, and then using it a context where implicit conversion is
// required: i.e., as an argument to a function expecting an argument of type
// 'bslmf::MatchArithmeticType'.  Successful compilation is the indicator of
// success.  Correspondingly, a manually driven test case is supplied to allow
// confirmation that inappropriate types are *not* converted.
// Failure-to-compile is indication of success for that case.
//
// ----------------------------------------------------------------------------
// CREATORS
// [ 1] MatchArithmeticType(int);
// [ 1] MatchArithmeticType(const MatchArithmeticType&);
// [ 1] ~MatchArithmeticType();
// ----------------------------------------------------------------------------
// [ 2] INTEGRAL CONVERIBILITY
// [ 3] USAGE EXAMPLE
// ----------------------------------------------------------------------------
// [-1] NON-CONVERTIBLE

// ============================================================================
//                  STANDARD BDE ASSERT TEST MACRO
// ----------------------------------------------------------------------------
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
// ----------------------------------------------------------------------------

// ============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
// ----------------------------------------------------------------------------
// #define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) printf("<| " #X " |>\n");  // Quote identifier literally.
//#define P_(X) cout << #X " = " << (X) << ", " << flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number
#define T_ printf("\t");             // Print a tab (w/o newline)

// ============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

typedef bslmf::MatchArithmeticType Obj;

// ============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
// ----------------------------------------------------------------------------

static int globalVerbose = 0;

// ============================================================================
//                  GLOBAL HELPER CLASSES
// ----------------------------------------------------------------------------

void acceptObj(bslmf::MatchArithmeticType)
{
}

template <class TYPE>
struct IntegralConveribility
{
    // This 'struct' provides a namespace for a simple test method.

    // CLASS METHODS
    static void implicitlyConvert();
        // Implicity convert objects of type:
        //: o 'TYPE'
        //: o 'TYPE&'
        //: o 'const TYPE'
        //: o 'const TYPE&'
        //: o 'volatile TYPE'
        //: o 'volatile TYPE&'
        //: o 'const volatile TYPE'
        //: o 'const volatile TYPE&'
        // for the parameterized 'TYPE' to 'bslmf::MatchArithmeticType'
        // objects, and write to standard output a human-readable
        // representation of 'TYPE'.

};

template <class TYPE>
void IntegralConveribility<TYPE>::implicitlyConvert()
{
    if (globalVerbose) {
        printf("%s\n", typeid(TYPE).name());
    }

    TYPE obj = TYPE();
    acceptObj(obj);

    TYPE& objRef = obj;
    acceptObj(objRef);

    const TYPE cObj = TYPE();
    acceptObj(cObj);

    const TYPE cObjRef = obj;
    acceptObj(cObjRef);

    volatile TYPE vObj = TYPE();
    acceptObj(vObj);

    volatile TYPE vObjRef = obj;
    acceptObj(vObjRef);

    const volatile TYPE cvObj = TYPE();
    acceptObj(cvObj);

    const volatile TYPE cvObjRef = obj;
    acceptObj(cvObjRef);
}

class MyConvertibleToInt
{
  public:
    // ACCESSORS
    operator int() const;
};

// ACCESSORS
MyConvertibleToInt::operator int() const
{
    return 113355;
}

// ============================================================================
//                              USAGE EXAMPLES
// ----------------------------------------------------------------------------

namespace usageExample1 {

// USAGE: BEG: SLICE 1 of 10
///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: "Do-the-Right-Thing" Clause Dispatch
///- - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we want to create a container with two constructors:
//
//: o one constructor providing initialization with multiple copies of a single
//:   value (a "repeated value constructor"), and
//:
//: o the other providing initialization from a sequence of values delimited by
//:   a pair of iterators (a "range constructor").
//
// A naive implementation can result in common usage situations in which
// arguments meaningful to the former constructor are provided but where the
// compiler resolves the overload to the latter constructor.
//
// For example, the 'MyProblematicContainer' class outlined below provides two
// such constructors.  Note that each is atypically embellished with a
// 'message' parameter, allowing us to trace the call flow.
//..
                        // ============================
                        // class MyProblematicContainer
                        // ============================

    template <class VALUE_TYPE>
    class MyProblematicContainer {

        // ...

      public:
        // CREATORS
        MyProblematicContainer(std::size_t        numElements,
                               const VALUE_TYPE&  value,
                               const char        *message);
            // Create a 'MyProblematicContainer' object containing the
            // specified 'numElements' of the specified 'value', and write to
            // standard output the specified 'message'.

        template <class INPUT_ITER>
        MyProblematicContainer(INPUT_ITER  first,
                               INPUT_ITER  last,
                               const char *message);
            // Create a 'MyProblematicContainer' object containing the values
            // in the range starting at the specified 'first' iterator and
            // ending immediately before the specified 'last' iterator of the
            // 'INPUT_ITER' type, and write to standard output the specified
            // 'message'.

        // ...

    };

    // ========================================================================
    //                      INLINE FUNCTION DEFINITIONS
    // ========================================================================

                        // ============================
                        // class MyProblematicContainer
                        // ============================

    // CREATORS
    template <class VALUE_TYPE>
    MyProblematicContainer<VALUE_TYPE>::MyProblematicContainer(
                                                std::size_t        numElements,
                                                const VALUE_TYPE&  value,
                                                const char        *message)
    {
        ASSERT(message);
#ifdef SHOW_IN_COMPONENT_LEVEL_DOC
        printf("CTOR: repeated value: %s\n", message);
        // ...
#else
        if(globalVerbose) {
            printf("CTOR: repeated value: %s\n", message);
        }
        (void)numElements;
        (void)value;
#endif
    }

    template <class VALUE_TYPE>
    template <class INPUT_ITER>
    MyProblematicContainer<VALUE_TYPE>::MyProblematicContainer(
                                                           INPUT_ITER  first,
                                                           INPUT_ITER  last,
                                                           const char *message)
    {
        ASSERT(message);
#ifdef SHOW_IN_COMPONENT_LEVEL_DOC
        printf("CTOR: range         : %s\n", message);
        // ...
#else
        if(globalVerbose) {
            printf("CTOR: range         : %s\n", message);
        }
        (void)first;
        (void)last;
#endif
    }
//..
// USAGE: END: SLICE 1 of 10
//..
    int main1()
    {
//..
// USAGE: BEG: SLICE 2 of 10
// The problem with the 'MyProblematicContainer' class becomes manifest when
// we create several objects:
//..
        const char                   input[] = "How now brown cow?";
        MyProblematicContainer<char> initFromPtrPair(
                                                  input,
                                                  input + sizeof(input),
                                                  "Called with pointer pair.");

        MyProblematicContainer<char> initFromIntAndChar(
                                              5,
                                              'A',
                                              "Called with 'int' and 'char'.");

        MyProblematicContainer<char> initFromIntAndInt(
                                              5,
                                              65, // 'A'
                                              "Called with 'int' and 'int'.");
//..
// Standard output shows:
//..
//  CTOR: range         : Called with pointer pair.
//  CTOR: repeated value: Called with 'int' and 'char'.
//  CTOR: range         : Called with 'int' and 'int'.
//..
// Notice that the range constructor, not the repeated value constructor, is
// invoked for the creation of 'initFromIntAndInt', the third object.
//
// The range constructor is chosen to resolve that overload because its match
// of two arguments of the same type ('int' in this usage) without conversion
// is better than that provided by the repeated value constructor, which
// requires conversions of two different arguments.
//
// If we are fortunate, range constructor code will fail to compile; otherwise,
// dereferencing integer values (i.e., using them as pointers) leads to
// undefined behavior.
//
// Note that, in many other situations, overloading resolution issues can be
// avoided by function renaming; however, as these are constructors, we do not
// have that option.
//
// Instead, we redesign our class ('MyContainer' is the redesigned class) so
// that the calls to the range constructor with two 'int' arguments (or pairs
// of the same integer types) are routed to the repeated value constructor.
// The 'bslmf::MatchArithmeticType' class is used to distinguish between
// integer types and other types.
//
// USAGE: END: SLICE 2 of 10
//..
        return 0;
    }
//..
// USAGE: BEG: SLICE 3 of 10
// First, we define the 'MyContainer' class to have constructors taking
// the same arguments as the constructors of 'MyProblematicContainer':
//..
                        // =================
                        // class MyContainer
                        // =================

    template <class VALUE_TYPE>
    class MyContainer {

        // ...

      public:
        // CREATORS
        MyContainer(std::size_t        numElements,
                    const VALUE_TYPE&  value,
                    const char        *message);
            // Create a 'MyProblematicContainer' object containing the
            // specified 'numElements' of the specified 'value', and write to
            // standard output the specified 'message'.

        template <class INPUT_ITER>
        MyContainer(INPUT_ITER  first, INPUT_ITER  last, const char *message);
            // Create a 'MyProblematicContainer' object containing the values
            // in the range starting at the specified 'first' and ending
            // immediately before the specified 'last' iterators of the type
            // 'INPUT_ITER', and write to standard output the specified
            // 'message'.

        // ...

#ifdef SHOW_IN_COMPONENT_LEVEL_DOC
   };
#endif
//..
// USAGE: END: SLICE 3 of 10
// USAGE: BEG: SLICE 4 of 10
// Then, we isolate the essential actions of our two constructors into two
// private, non-creator methods.  This allows us to achieve the results of
// either constructor, as appropriate, from the context of the range
// constructor.  The two 'privateInit*' methods are:
//..
      private:
        // PRIVATE MANIPULATORS
        void privateInit(std::size_t        numElements,
                         const VALUE_TYPE&  value,
                         const char        *message);
            // Initialize a 'MyContainer' object containing the specified
            // 'numElements' of the specified 'value', and write to standard
            // output the specified 'message'.

        template <class INPUT_ITER>
        void privateInit(INPUT_ITER  first,
                         INPUT_ITER  last,
                         const char *message);
            // Initialize a 'MyContainer' object containing the values in the
            // range starting at the specified 'first' and ending immediately
            // before the specified 'last' iterators of the type 'INPUT_ITER',
            // and write to standard output the specified 'message'.
//..
// USAGE: END: SLICE 4 of 10
// USAGE: BEG: SLICE 6 of 10
// Now, we define two overloaded 'privateInitDispatch' methods, each taking
// two parameters (the last two) which serve no run-time purpose.  As we shall
// see, they exist only to guide overload resolution at compile-time.
//..
        template <class INTEGER_TYPE>
        void privateInitDispatch(INTEGER_TYPE                numElements,
                                 INTEGER_TYPE                value,
                                 const char                 *message,
                                 bslmf::MatchArithmeticType  ,
                                 bslmf::Nil                  );
            // Initialize a 'MyContainer' object containing the specified
            // 'numElements' of the specified 'value', and write to standard
            // output the specified 'message'.  The last two arguments are used
            // only for overload resolution.

        template <class INPUT_ITER>
        void privateInitDispatch(INPUT_ITER                   first,
                                 INPUT_ITER                   last,
                                 const char                  *message,
                                 BloombergLP::bslmf::AnyType  ,
                                 BloombergLP::bslmf::AnyType  );
            // Initialize a 'MyContainer' object containing the values in the
            // range starting at the specified 'first' and ending immediately
            // before the specified 'last' iterators of the type 'INPUT_ITER',
            // and write to standard output the specified 'message'.  The last
            // two arguments are used only for overload resolution.
//..
// Notice that the first overload has strict requirements on the last two
// arguments, but the second overload (accepting 'bslmf::AnyType' in those
// positions) will match all contexts in which the first fails to match.
//
// USAGE: END: SLICE 6 of 10
//..

    };

    // ========================================================================
    //                      INLINE FUNCTION DEFINITIONS
    // ========================================================================

                        // =================
                        // class MyContainer
                        // =================
//..
// USAGE: BEG: SLICE 5 of 10
// Note that, as in the constructors of the 'MyProblematic' class, the
// 'privateInit*' methods provide display a message so we can trace the call
// path.
//..
    // PRIVATE MANIPULATORS
    template <class VALUE_TYPE>
    void MyContainer<VALUE_TYPE>::privateInit(std::size_t        numElements,
                                              const VALUE_TYPE&  value,
                                              const char        *message)
    {
        ASSERT(message);
#ifdef SHOW_IN_COMPONENT_LEVEL_DOC
        printf("INIT: repeated value: %s\n", message);
        // ...
#else
        if(globalVerbose) {
            printf("INIT: repeated value: %s\n", message);
        }
        (void) numElements;
        (void) value;
#endif
    }

    template <class VALUE_TYPE>
    template <class INPUT_ITER>
    void MyContainer<VALUE_TYPE>::privateInit(INPUT_ITER  first,
                                              INPUT_ITER  last,
                                              const char *message)
    {
        ASSERT(message);
#ifdef SHOW_IN_COMPONENT_LEVEL_DOC
        printf("INIT: range         : %s\n", message);
        // ...
#else
        if(globalVerbose) {
            printf("INIT: range         : %s\n", message);
        }
        (void)first;
        (void)last;
#endif
    }
//..
// USAGE: END: SLICE 5 of 10
// USAGE: BEG: SLICE 7 of 10
// Then, we implement the two 'privateInitDispatch' overloads so that
// each invokes a different overload of the 'privateInit' methods:
//
//: o The 'privateInit' corresponding to repeated value constructor is invoked
//:   from the "strict" overload of 'privateInitDispatch'.
//:
//: o The 'privateInit' for range construction is invoked from the other
//:   'privateInitDispatch' overload.
//..
    template <class VALUE_TYPE>
    template <class INTEGER_TYPE>
    void MyContainer<VALUE_TYPE>::privateInitDispatch(
                                       INTEGER_TYPE                numElements,
                                       INTEGER_TYPE                value,
                                       const char                 *message,
                                       bslmf::MatchArithmeticType  ,
                                       bslmf::Nil                  )
    {
        (void)message;

        privateInit(static_cast<std::size_t>(numElements),
                    static_cast<VALUE_TYPE>(value),
                    "Called via 'privateInitDispatch'.");
    }

    template <class VALUE_TYPE>
    template <class INPUT_ITER>
    void MyContainer<VALUE_TYPE>::privateInitDispatch(INPUT_ITER      first,
                                                      INPUT_ITER      last,
                                                      const char     *message,
                                                      bslmf::AnyType  ,
                                                      bslmf::AnyType  )
    {
        privateInit(first, last, message);
    }
//..
// USAGE: END: SLICE 7 of 10
//
// USAGE: BEG: SLICE 9 of 10
// Then, we implement the repeated value constructor using a direct call
// to the repeated value private initializer:
//..
    // CREATORS
    template <class VALUE_TYPE>
    MyContainer<VALUE_TYPE>::MyContainer(std::size_t        numElements,
                                         const VALUE_TYPE&  value,
                                         const char        *message)
    {
        privateInit(numElements, value, message);
    }
//..
// USAGE: END: SLICE 9 of 10
// USAGE: BEG: SLICE 8 of 10
// Next, we use overloaded 'privateInitDispatch' method in the range
// constructor of 'MyContainer'.  Note that we always supply a 'bslmf::Nil'
// object (an exact type match) as the final argument, the choice of overload
// will be govered according to the type of 'first'.  Consequently, if 'first'
// is implicitly convertible to 'bslmf::MatchArithmeticType', then the overload
// leading to repeated value construction is used; otherwise, the overload
// leading to range construction is used.
//..
    template <class VALUE_TYPE>
    template <class INPUT_ITER>
    MyContainer<VALUE_TYPE>::MyContainer(INPUT_ITER  first,
                                         INPUT_ITER  last,
                                         const char *message)
    {
        privateInitDispatch(first, last, message, first, bslmf::Nil());
    }
//..
// Notice that this design is safe for iterators that themselves happen to have
// a conversion to 'int'.  Such types would require two user-defined
// conversions, which are disallowed by the C++ compiler, to match the
// 'bslmf::MatchArithmeticType' parameter of the "strict" 'privateInitDispatch'
// overload.
//
// USAGE: END: SLICE 8 of 10
//..
    int main2()
    {
//..
// USAGE: BEG: SLICE 10 of 10
// Finally, we create three objects of 'MyContainer', using the same arguments
// as we used for the three 'MyProblematicContainer' objects.
//..
        const char        input[] = "How now brown cow?";
        MyContainer<char> initFromPtrPair(input,
                                          input + sizeof(input),
                                          "Called with pointer pair.");

        MyContainer<char> initFromIntAndChar(5,
                                             'A',
                                             "Called with 'int' and 'char'.");

        MyContainer<char> initFromIntAndInt(5,
                                            65, // 'A'
                                            "Called with 'int' and 'int'.");
//..
// Standard output shows:
//..
//  INIT: range         : Called with pointer pair.
//  INIT: repeated value: Called with 'int' and 'char'.
//  INIT: repeated value: Called via 'privateInitDispatch'.
//..
// Notice that the repeated value 'privateInit' method is called directly for
// the second object, but called via 'privateInitDispatch' for the third
// object.
// USAGE: END: SLICE 10 of 10
//..
        return 0;
    }

}  // end example namespace

// ============================================================================
//                              MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int    test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;

    globalVerbose = verbose;

    setbuf(stdout, NULL);    // Use unbuffered output

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:
      case 3: {
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
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) printf("\nUSAGE EXAMPLE"
                            "\n=============\n");

        usageExample1::main1();
        usageExample1::main2();

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // INTEGRAL CONVERIBILITY
        //
        // Concerns:
        //: 1 Every C++ arithmetic type, each with and without
        //:   cv-qualifications, can be implicitly converted to a
        //:   'bslmf::MatchArithmeticType' object.
        //
        // Plan:
        //: 1 Define 'acceptObj', a function specifying a parameter of type
        //:   'bslmf::MatchArithmeticType', and 'IntegralConvertability' a
        //:   structure templated on 'TYPE' whose 'implicityConvert' method has
        //:   multipled invocations of 'acceptObj' providing and object 'TYPE'
        //:   with different qualifications: e.g., by value, by reference, by
        //:   'const'-reference, by 'volatile'-'const'-reference.  Successful
        //:   compilation indicates success.  (C-1)
        //
        // Testing:
        //   INTEGRAL CONVERIBILITY
        // --------------------------------------------------------------------

        if (verbose) printf("\nINTEGRAL CONVERIBILITY"
                            "\n======================\n");

        IntegralConveribility<                    bool  >::implicitlyConvert();

        IntegralConveribility<                    char  >::implicitlyConvert();
        IntegralConveribility<                   wchar_t>::implicitlyConvert();

        IntegralConveribility<  signed            char  >::implicitlyConvert();
        IntegralConveribility<  signed      short int   >::implicitlyConvert();
        IntegralConveribility<  signed            int   >::implicitlyConvert();
        IntegralConveribility<  signed      long  int   >::implicitlyConvert();
        IntegralConveribility<  signed long long  int   >::implicitlyConvert();

        IntegralConveribility<unsigned            char  >::implicitlyConvert();
        IntegralConveribility<unsigned      short int   >::implicitlyConvert();
        IntegralConveribility<unsigned            int   >::implicitlyConvert();
        IntegralConveribility<unsigned      long  int   >::implicitlyConvert();
        IntegralConveribility<unsigned long long  int   >::implicitlyConvert();

        IntegralConveribility<                    float >::implicitlyConvert();
        IntegralConveribility<                    double>::implicitlyConvert();
        IntegralConveribility<              long  double>::implicitlyConvert();

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // VALUE CTOR, COPY CTOR, and DTOR
        //   Ensure that we can construct an object, use it as a function
        //   argument, and destroy it safely.
        //
        // Concerns:
        //: 1 The non-'explicit' constructor will safely construct an object
        //:   for an arbitrary 'int' value.
        //:
        //: 2 The object can be copy-constructed and used a function argument.
        //:
        //: 3 The destructor, safely destroys the object.
        //
        // Plan:
        //: 1 Construct an object 'obj' of type 'bslmf::MatchArithmeticType'
        //:   using an arbitrary integer value.  (C-1)
        //:
        //: 2 Pass the constructed 'obj' to 'accepObj' to a locally defined
        //:   function with a formal parameter of 'MatchArithmeticType', and
        //:   confirm that the correct function was called.  (C-2)
        //:
        //: 3 Allow the constructed object to go out of scope.  (C-3)
        //
        // Testing:
        //   MatchArithmeticType(int);
        //   MatchArithmeticType(const MatchArithmeticType&);
        //   ~MatchArithmeticType();
        // --------------------------------------------------------------------

        if (verbose) printf("\nVALUE CTOR, COPY CTOR, and DTOR"
                            "\n===============================\n");

        {
            Obj obj(-1234);

            struct Local {
                static int sink(Obj obj)
                {
                    return 4321;
                }
                static int sink(...) 
                {
                    return 5678;
                }
            };

            ASSERT(4321 == Local::sink(obj));
        }

      } break;
      case -1: {
        // --------------------------------------------------------------------
        // NON-CONVERTIBLE
        //   This manually driven test case demonstrates how the
        //   'bslmf::MatchArithmeticType' class disallows implicit conversion
        //   from inappropriate types.  Each
        //   'BSLSTL_DISAMBIGUATOR_NON_CONVERT_*' preprocessor symbol should in
        //   turn be defined and compiler failure (success for the case)
        //   confirmed.
        //
        // Concerns:
        //: 1 Classes convertible to 'int' are not implicitly convertible to
        //:   'bslmf::MatchArithmeticType'.
        //:
        //: 2 Pointers are are not implicitly convertible to
        //:   'bslmf::MatchArithmeticType'.
        //
        // Plan:
        //: 1 Create an object of 'MyConvertibleToInt' class and attempt
        //:   to use it in a context that requires implicit conversion to a
        //:   'bslmf::MatchArithmeticType' object.  (C-1)
        //:
        //: 2 Create a 'int *' object and attempt to use it n a context
        //:   to use it in a context requiring implicit conversion to a
        //:   'bslmf::MatchArithmeticType' object.
        //
        // Testing:
        //   NON-CONVERTIBLE
        // --------------------------------------------------------------------

        if (verbose) printf("\nNON-CONVERTIBLE"
                            "\n===============\n");

//#define BSLSTL_DISAMBIGUATOR_NON_CONVERT_MYCONVERTITBLETOINTCLASS
#ifdef  BSLSTL_DISAMBIGUATOR_NON_CONVERT_MYCONVERTITBLETOINTCLASS
        MyConvertibleToInt obj;
        if (verbose) {
            printf ("MyConvertibleToInt: %d\n", (int)obj);
        }
        acceptObj(obj);
#endif

//#define BSLSTL_DISAMBIGUATOR_NON_CONVERT_POINTER
#ifdef  BSLSTL_DISAMBIGUATOR_NON_CONVERT_POINTER
        int i;
        int *p = &i;
        if (verbose) {
            printf ("int *: %p\n", p);
        }
        acceptObj(p);
#endif
      } break;
      default: {
        fprintf(stderr, "WARNING: CASE `%d' NOT FOUND.\n", test);
        testStatus = -1;
      } break;
    }

    if (testStatus > 0) {
        fprintf(stderr, "Error, non-zero test status = %d.\n", testStatus);
    }

    return testStatus;
}

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2012
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
