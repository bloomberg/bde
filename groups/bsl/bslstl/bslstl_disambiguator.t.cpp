// bslstl_disambiguator.t.cpp                                         -*-C++-*-

#include <bslstl_disambiguator.h>

#include <bslmf_anytype.h>

#include <cstdio>
#include <cstdlib>

using namespace BloombergLP;
using namespace std;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//
//
//-----------------------------------------------------------------------------

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

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

//=============================================================================
//                              USAGE EXAMPLES
//-----------------------------------------------------------------------------

namespace usageExample1 {

///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: "Do the Right Thing" Dispatch
/// - - - - - - - - - - - - - - - - - - - -
// Suppose we have a container with two constructors:
//
//: o one constructor providing initialization with multiple copies of a single
//:   value (a "duplicate value constructor"), and
//:
//: o the other providing initialization via a pair of iterators (a "range
//:   constructor").
//
// There are common usage situations in which arguments meaningful to the
// former constructor are provided but where the compiler resolves the overload
// to the latter constructor.
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
            // 'stdout' the specified 'message'.

        template <class INPUT_ITER>
        MyProblematicContainer(INPUT_ITER  first,
                               INPUT_ITER  last,
                               const char *message);
            // Create a 'MyProblematicContainer' object containing the values
            // in the range starting at the specified 'first' iterator and
            // ending immediately before the specified 'last' iterator of the
            // parameterized 'INPUT_ITER' type, and write to 'stdout' the
            // specified 'message'.

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

        (void)numElements;
        (void)value;

        printf("CTOR: duplicate value: %s\n", message);
    }

    template <class VALUE_TYPE>
    template <class INPUT_ITER>
    MyProblematicContainer<VALUE_TYPE>::MyProblematicContainer(
                                                           INPUT_ITER  first,
                                                           INPUT_ITER  last,
                                                           const char *message)
    {
        ASSERT(message);

        (void)first;
        (void)last;

        printf("CTOR: range          : %s\n", message);
    }
//..

    int main1()
    {
//..
// The problem with the 'MyProblematicContainer' class become manifest when
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
// The trace messages from the constructors show that 'initFromIntAndInt',
// the third object, is created using the range constructor: at best, this
// will fail to compile!
//..
// CTOR: range : Called with pointer pair.  CTOR: duplicate value: Called with
// 'int' and 'char'.  CTOR: range : Called with 'int' and 'int'.
//..
// The arguments provided for the creation of 'initFromIntAndInt' resolve to
// the range constructor because the template match of two arguments of the
// same type ('int') is stronger than that found for the duplicate value
// constructor, which requires conversions.
//
// In many other situations, overloading resolution issues can be avoided by
// function renaming; however, as these are constructors, we do not have
// that option.
//
// Instead, we redesign our class ('MyContainer' is the redesigned class)
// so that the calls to the range constructor with two 'int' arguments
// (or pairs of matching integer types) are routed to the duplicate value
// constructor.
//..
        return 0;
    }
//..
// First, before we can perform a duplicate value construction from the context
// of the range constructor, we must isolate the essential actions of that
// these constructors into non-creator methods.  We define two 'privateInit'
// methods corresponding to the two constructors.
//..
                        // =================
                        // class MyContainer
                        // =================

    template <class VALUE_TYPE>
    class MyContainer {

        // ...

      private:
        // PRIVATE MANIPULATORS
        void privateInit(std::size_t        numElements,
                         const VALUE_TYPE&  value,
                         const char        *message);
            // Initialize a 'MyProblematic' object containing the specified
            // 'numElements' of the specified 'value', and write to 'stdout'
            // the specified 'message'.

        template <class INPUT_ITER>
        void privateInit(INPUT_ITER  first,
                         INPUT_ITER  last,
                         const char *message);
            // Initialize a 'MyProblematic' object containing the values in the
            // range starting at the specified 'first' and ending immediately
            // before the specified 'last' iterators of the parameterized
            // 'INPUT_ITER' type, and write to 'stdout' the specified
            // 'message'.
//..
// Next, we define two overloaded 'privateInitDispatch' methods, each taking
// two parameters (the last two) which serve no purpose other than guiding
// the overload resolution.
//
// One overload is quite general; the last two parameters are
// match 'bslmf::AnyType'.
//..
        template <class INPUT_ITER>
        void privateInitDispatch(INPUT_ITER                   first,
                                 INPUT_ITER                   last,
                                 const char                  *message,
                                 BloombergLP::bslmf::AnyType  ,
                                 BloombergLP::bslmf::AnyType  );
            // Initialize a 'MyProblematic' object containing the values in the
            // range starting at the specified 'first' and ending immediately
            // before the specified 'last' iterators of the parameterized
            // 'INPUT_ITER' type, and write to 'stdout' the specified
            // 'message'.  The last two arguments are used only for overload
            // resolution.

//..
// The other overload is specialized: The penultimate parameter
// is 'bslstl::Disambiguator' (basic integral types accepted here), and
// the last parameter is a simple 'int'.
//..
        template <class INTEGER_TYPE>
        void privateInitDispatch(INTEGER_TYPE           numElements,
                                 INTEGER_TYPE           value,
                                 const char            *message,
                                 bslstl::Disambiguator  ,
                                 int                    );
            // Initialize a 'MyProblematic' object containing the specified
            // 'numElements' of the specified 'value', and write to 'stdout'
            // the specified 'message'.  The last two arguments are used only
            // for overload resolution.
//..

      public:
        // CREATORS
        MyContainer(std::size_t        numElements,
                    const VALUE_TYPE&  value,
                    const char        *message);
            // Create a 'MyProblematicContainer' object containing the
            // specified 'numElements' of the specified 'value', and write to
            // 'stdout' the specified 'message'.

        template <class INPUT_ITER>
        MyContainer(INPUT_ITER  first, INPUT_ITER  last, const char *message);
            // Create a 'MyProblematicContainer' object containing
            // the values in the range starting at the specified
            // 'first' and ending immediately before the specified 'last'
            // iterators of the parameterized 'INPUT_ITER' type, and write to
            // 'stdout' the specified 'message'.

        // ...

    };

    // ========================================================================
    //                      INLINE FUNCTION DEFINITIONS
    // ========================================================================

                        // =================
                        // class MyContainer
                        // =================

// As in the 'MyProblematic' class, the essential behavior is merely to
// provide a trace of the call path.
//..
    // PRIVATE MANIPULATORS
    template <class VALUE_TYPE>
    void MyContainer<VALUE_TYPE>::privateInit(std::size_t        numElements,
                                              const VALUE_TYPE&  value,
                                              const char        *message)
    {
        ASSERT(message);
        (void) numElements;
        (void) value;

        printf("INIT: duplicate value: %s\n", message);
    }
    template <class VALUE_TYPE>
    template <class INPUT_ITER>
    void MyContainer<VALUE_TYPE>::privateInit(INPUT_ITER  first,
                                              INPUT_ITER  last,
                                              const char *message)
    {
        ASSERT(message);
        (void)first;
        (void)last;

        printf("INIT: range          : %s\n", message);
    }
//..
// The implementation of the more general overload invokes the private
// manipulator that handles the range construction.
//..
    template <class VALUE_TYPE>
    template <class INPUT_ITER>
    void MyContainer<VALUE_TYPE>::privateInitDispatch(
                                          INPUT_ITER                   first,
                                          INPUT_ITER                   last,
                                          const char                  *message,
                                          BloombergLP::bslmf::AnyType  ,
                                          BloombergLP::bslmf::AnyType  )
    {
        privateInit(first, last, message);
    }

//..
// The implementation of the more specialized overload invokes the private
// manipulator that handles the duplicate value construction.
//..
    template <class VALUE_TYPE>
    template <class INTEGER_TYPE>
    void MyContainer<VALUE_TYPE>::privateInitDispatch(
                                            INTEGER_TYPE           numElements,
                                            INTEGER_TYPE           value,
                                            const char            *message,
                                            bslstl::Disambiguator  ,
                                            int                    )
    {
        (void)message;

        privateInit(static_cast<std::size_t>(numElements),
                    static_cast<VALUE_TYPE>(value),
                    "via 'privateInitDispatch'");
    }
//..
// Notice that a distinctive message is supplied to mark our passage through
// this dispatch method.
//
// The implementation of the duplicate value constructor simply calls the
// the corresponding private manipulator.
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
// The range constructor (problematic in our earlier
// class) calls the overloaded 'privateInitDispatch' method.
// The use of hardcoded '0' in the last argument is an exact match
// for method expecting an 'int' as its last argument.  Thus, the
// overload resolves to that method provided the type of the
// penultimate argument, 'first', can be converted to 'bslstl::Disambiguate'
// (i.e., 'first' is an integral type).
//..
    template <class VALUE_TYPE>
    template <class INPUT_ITER>
    MyContainer<VALUE_TYPE>::MyContainer(INPUT_ITER  first,
                                         INPUT_ITER  last,
                                         const char *message)
    {
        privateInitDispatch(first, last, message, first, 0);
    }
//..
// Notice that this solution excludes iterators that themselves happen
// to have a conversion to 'int'.  Such types would require two
// user-defined conversions (iterator-to-integral type, then
// integral-type to 'bslstl::Disambiguate') which are disallowed
// by the C++ compiler.

    int main2()
    {
//..
// Finally, we create three objects of 'MyContainer', using the same arguments
// has we used for the three 'MyProblematicContainer' objects.
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
// INIT: range : Called with pointer pair.  INIT: duplicate value: Called with
// 'int' and 'char'.  INIT: duplicate value: via 'privateInitDispatch'
//..
// Notice that the duplicate value 'privateInit' method is called directly
// for the second object, but called via 'privateInitDispatch' for the
// third object.
//..
        return 0;
    }

}  // end example namespace

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int    test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;

    setbuf(stdout, NULL);    // Use unbuffered output

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:
      case 2: {
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
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING/USAGE TEST
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        //
        // --------------------------------------------------------------------

        if (verbose) printf("\nBREATHING TEST"
                            "\n==============\n");

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
//      Copyright (C) Bloomberg L.P., 2004
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
