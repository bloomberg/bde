// bsls_unspecifiedbool.t.cpp                                         -*-C++-*-

#include <bsls_unspecifiedbool.h>

#include <stdio.h>
#include <stdlib.h>     // atoi()
#include <typeinfo>     // for typeid, but bad levelization.

using namespace BloombergLP;

//=============================================================================
//                             TEST PLAN
//                             ---------
// The 'bsls_unspecifiedbool' component provides a class supplying the typedef
// for a type suitable for use in a conversion operator for types that must be
// convertible to bool, but without suffering the accidental integeral
// promotions that would result if a true 'operator bool' was defined.  It
// further provides two static member functions that return values of this type
// equivalant to 'true' and 'false'.  In order to completely test this
// component we must prove that the aliases type serves the advertized pupose
// of supporting only the desired boolean conversions, and not the broader set
// of integral conversions.  Then we must demonstrate that the two functions
// return values with the correct properties.  Finally, we will compile and
// run the usage example.
//
//-----------------------------------------------------------------------------
// [ 3] BoolType falseValue();
// [ 3] BoolType trueValue();
// [ 2] typedef BoolType 
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 4] USAGE EXAMPLE

namespace {

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
//                  STANDARD BDE LOOP-ASSERT TEST MACROS
//-----------------------------------------------------------------------------
# define LOOP_ASSERT(I,X) { \
    if (!(X)) { P_(I); aSsErT(!(X), #X, __LINE__); } }

# define LOOP2_ASSERT(I,J,X) { \
    if (!(X)) { P(I) P_(J);   \
                aSsErT(!(X), #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) { \
    if (!(X)) { P(I) P(J) P_(K) \
                aSsErT(!(X), #X, __LINE__); } }

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define Q(X) printf("<| " #X " |>\n");      // Quote identifier literally.
#define P(X) dbg_print(#X " = ", X, "\n");  // Print identifier and value.
#define P_(X) dbg_print(#X " = ", X, ", "); // P(X) without '\n'
#define L_ __LINE__                         // current Line number
#define T_ putchar('\t');                   // Print a tab (w/o newline)

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

// Fundamental-type-specific print functions.
inline void dbg_print(bool b) { printf(b ? "true" : "false"); fflush(stdout); }
inline void dbg_print(char c) { printf("%c", c); fflush(stdout); }
inline void dbg_print(unsigned char c) { printf("%c", c); fflush(stdout); }
inline void dbg_print(signed char c) { printf("%c", c); fflush(stdout); }
inline void dbg_print(short val) { printf("%d", (int)val); fflush(stdout); }
inline void dbg_print(unsigned short val) {
    printf("%d", (int)val); fflush(stdout);
}
inline void dbg_print(int val) { printf("%d", val); fflush(stdout); }
inline void dbg_print(unsigned int val) { printf("%u", val); fflush(stdout); }
inline void dbg_print(long val) { printf("%ld", val); fflush(stdout); }
inline void dbg_print(unsigned long val) {
    printf("%lu", val); fflush(stdout);
}
inline void dbg_print(long long val) { printf("%lld", val); fflush(stdout); }
inline void dbg_print(unsigned long long val) {
    printf("%llu", val); fflush(stdout);
}
inline void dbg_print(float val) {
    printf("'%f'", (double)val); fflush(stdout);
}
inline void dbg_print(double val) { printf("'%f'", val); fflush(stdout); }
inline void dbg_print(long double val) {
    printf("'%Lf'", val); fflush(stdout);
}
inline void dbg_print(const char* s) { printf("\"%s\"", s); fflush(stdout); }
inline void dbg_print(char* s) { printf("\"%s\"", s); fflush(stdout); }
inline void dbg_print(void* p) { printf("%p", p); fflush(stdout); }

// Generic debug print function (3-arguments).
template <typename T>
void dbg_print(const char* s, const T& val, const char* nl) {
    printf("%s", s); dbg_print(val);
    printf("%s", nl);
    fflush(stdout);
}

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

//=============================================================================
//                         HELPER CLASSES FOR TESTING
//-----------------------------------------------------------------------------

}  // close unnamed namespace

//=============================================================================
//                                USAGE EXAMPLE
//-----------------------------------------------------------------------------
namespace USAGE_EXAMPLE {

// A common requirement for "smart pointer" types is to emulate the native
// pointer types, and in particular support testing for "null" or "empty" 
// pointer values as a simple boolean conversion in 'if' and 'while' clauses.
// We will demonstrate with a simple smart pointer type, 'SimplePtr', that
// holds a pointer value but does not claim ownership or any responsibility for
// the lifetime of the pointed-to object.  It will act as a "simple" native
// pointer.
//..
    template <class T>
    class SimplePtr
    {
        // This class holds a pointer to a single object, and provides a subset
        // of the regular pointer operatiors.  For example, it can be
        // dereferenced with 'operator*' and tested as a boolean value to see
        // if it is null.  Conversely, it does not support pointer arithmetic.
      private:
        // DATA
        T *d_ptr;  // address of the referenced object

      public:
        // CREATORS
        explicit SimplePtr(T *ptr = 0) : d_ptr(ptr) {}
            // Create a 'SimplePtr' what the value of the specified 'ptr'.

        //! ~SimplePtr() = default;
            // Destroy this object.  Note that this trivial destructor is
            // generated by the compiler.

        // ACCESSORS
        T & operator*() const  { return *d_ptr; }
            // Return a reference to the object pointed to by this
            // 'SimplePtr'.

        T * operator->() const { return d_ptr; }
            // Return the held 'd_ptr'.
//..
// Next we define, for conventience, an alias for a unique type that is
// implcititly convertible to bool.  Note that we pass the current template
// instantiation to the 'bsls_UnspecefiedBool' template in order to guarantee
// a unique name, even for different instantiation of this same 'SimplePtr'
// template.
//..
        typedef typename bsls_UnspecifiedBool<SimplePtr>::BoolType BoolType;
//..
// Now we can define a boolean conversion operator that tests whether or not
// this 'SimplePtr' object is holding a null pointer, or a valid address.  Note
// that we do not need to define 'operator!' as this single boolean conversion
// operator will be invoked with the correct semantics when the user tries that
// operator.
//..
        operator BoolType() const {
            return d_ptr
                 ? bsls_UnspecifiedBool<SimplePtr>::trueValue()
                 : bsls_UnspecifiedBool<SimplePtr>::falseValue();
        }
    }; // class simple_pointer
//..
// Next we will write a simple test function, creating a couple of 'SimplePtr'
// objects, one "null" and the other with a well-defined address.
//..
    void runTests() {
        int i = 3;
        SimplePtr<int> p1;
        SimplePtr<int> p2(&i);
//..
// Finally we test each 'SimplePtr' value, as if they were raw pointers.
//..
        ASSERT(!p1);
        ASSERT(p2);
        if (p2) {
            ASSERT(3 == *p2);
        }
    }
//..
} // namespace USAGE_EXAMPLE

//=============================================================================
//                  TEST PROGRAM
//-----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;

    (void) verbose;
    (void) veryVerbose;

    setbuf(stdout, 0);    // Use unbuffered output

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:
      case 4: {
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
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING usage example"
                            "\n---------------------\n");

          USAGE_EXAMPLE::runTests();
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING true and false values
        // Concerns:
        //: 1 The static member functions 'trueValue' and 'falseValue' each
        //:   return a value of type BoolType.
        //: 2 'falseValue' returns a value that converts to the 'bool' value
        //:   'false'
        //: 3 'trueValue' returns a value that converts to the 'bool' value
        //:   'true'
        //
        // Plan:
        //: 1 Use the addresses of the 'falseFunc' and 'trueFunc' static
        //:   methods in this component to initialize free-function pointers
        //:   having the appropriate signatures return types.  (C-1)
        //:
        //: 2 Confirm that the value returned by 'falseValue' implicitly
        //:   converts to a 'bool' having the value 'false'.  (C-2)
        //:
        //: 3 Confirm that the value returned by 'trueValue' implicitly
        //:   converts to a 'bool' having the value 'true'.  (C-3)
        //
        // Testing:
        //  BoolType falseValue()
        //  BoolType trueValue()
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING falseValue() and trueValue()"
                            "\n------------------------------------\n");

        if (verbose) printf("\t1. Functions must return BoolType values\n");

        typedef bsls_UnspecifiedBool<int(int)> HostType;
        typedef HostType::BoolType        BoolType;

        typedef BoolType (*FuncType)();

        FuncType falseFunc = &HostType::falseValue;
        FuncType trueFunc  = &HostType::trueValue;

        // Silence unused variable warnings.
        (void)falseFunc;
        (void)trueFunc;

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) printf("\t2. falseValue() converts to false\n");

        const bool bFalse = HostType::falseValue();
        LOOP2_ASSERT(L_, bFalse, false == bFalse);

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) printf("\t3. trueValue() converts to true\n");

        const bool bTrue = HostType::trueValue();
        LOOP2_ASSERT(L_, bTrue, true == bTrue);
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING bsls_UnspecifiedBool<T>::BoolType
        //
        // Concerns:
        //: 1 Objects of type 'bsls_UnspecifiedBool<T>::BoolType' must be
        //:   implicitly convertible to bool.
        //: 2 Objects of type 'bsls_UnspecifiedBool<T>::BoolType' must not
        //:   promote to type 'int'.
        //: 3 Objects of type 'bsls_UnspecifiedBool<T>::BoolType' must not
        //:   convert to a simple pointer type.
        //: 4 A default constructed 'bsls_UnspecifiedBool<T>::BoolType'
        //:   should convert to 'false' when converted to a boolean value, the
        //:   same value as a default constructed 'bool'.
        //: 5 A 'bsls_UnspecifiedBool<T>::BoolType' object initialized
        //:   with the literal '0' should produce the value 'false' when
        //:   converted to a boolean value.
        //: 6 A class with a conversion operator to type
        //:   'bsls_UnspecifiedBool<T>::BoolType' should be implicitly
        //:   convertible to bool.
        //: 7 Two classes that are implicitly convertible to different
        //:   instantiations of type 'bsls_UnspecifiedBool<T>::BoolType' should
        //:   not accidentally be comparable to each other using 'operator=='.
        //
        // Plan:
        //: 1 Evaluate a value of type 'bsls_UnspecifiedBool<T>::BoolType' in
        //:   each context where the language supports an implicit conversion
        //:   to a boolean type.  The complete list of language contexts is:
        //:
        //:    1 Initialization
        //:    2 Passing as an argument to a function
        //:    3 In a return expression
        //:    4 Assignment
        //:    5 In a cast expression
        //:    6 In conjuntion with operator!
        //:    7 In conjunction with operator '&&' or '||', preserving boolean
        //:      short-circuit semantics
        //:    8 Testing in an 'if' clause
        //:    9 Testing in a 'while' loop
        //:   10 Testing in a 'for' loop
        //:   11 Testing as the condition of a ternary ?: expression
        //:   12 Passes safely through a ',' operator
        //:
        //:   Note that while there may be additional contexts to test in C++11
        //:   we have no interest in listing or testing them, as in that case
        //:   this whole idiom and component should be replaced with 'explicit
        //:   operator bool'.  (C1)
        //:
        //: 2 Define a pair of overloaded functions returning a 'bool' value,
        //:   one taking a single 'int' and returning 'true', the other
        //:   accepting anything through an elipsis parameter and returning
        //:   'false'.  Call this function function with a value of type
        //:   'bsls_UnspecifiedBool<T>::BoolType' to prove the 'int' overload
        //:   is *not* selected.
        //:
        //: 3 Define a set of overloaded functions returning a 'bool' value.
        //:   Four overloaded functions will take a single pointer argument,
        //:   covering all four cv-qualification combinations on a 'void *'
        //:   pointer, and return 'true'.  The other overload will accept
        //:   anything through an elipsis parameter and return 'false'.  Call
        //:   this function function with a value of type
        //:   'bsls_UnspecifiedBool<T>::BoolType' to prove that none of the
        //:   pointer-parameter overloads are selected.
        //:
        //: 4 Create a value-initialized object of type
        //:   'bsls_UnspecifiedBool<T>::BoolType' and ASSERT that its value
        //:   when converted to 'bool' is 'false'.
        //:
        //: 5 Create an object of type 'bsls_UnspecifiedBool<T>::BoolType' and
        //:   initialize it with the literal '0'.  ASSERT that its value
        //:   when converted to 'bool' is 'false'.
        //:
        //: 6 Define a new type, 'Booleable', with a conversion operator
        //:   converting to type 'bsls_UnspecifiedBool<Booleable>::BoolType'.
        //:   Evaluate a value of type 'Booleable' in each context where the
        //:   language supports an implicit conversion to a boolean type.  The
        //:   complete list of language contexts is:
        //:
        //:    1 Initialization
        //:    2 Passing as an argument to a function
        //:    3 In a return expression
        //:    4 Assignment
        //:    5 In a cast expression
        //:    6 In conjuntion with operator!
        //:    7 In conjunction with operator '&&' or '||', preserving boolean
        //:      short-circuit semantics
        //:    8 Testing in an 'if' clause
        //:    9 Testing in a 'while' loop
        //:   10 Testing in a 'for' loop
        //:   11 Testing as the condition of a ternary ?: expression
        //:   12 Passes safely through a ',' operator
        //:
        //:   Note that while there may be additional contexts to test in C++11
        //:   we have no interest in listing or testing them, as in that case
        //:   this whole idiom and component should be replaced with 'explicit
        //:   operator bool'.  (C6)
        //
        // Testing:
        //   typedef bsls_UnspecifiedBool::BoolType
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING bsls_UnspecifiedBool"
                            "\n----------------------------\n");

        typedef bsls_UnspecifiedBool<int>::BoolType BoolType;

        if (verbose) printf("\t1. BoolType implicitly conversts to bool\n");

        {
        const BoolType bt = BoolType();

        if (veryVerbose) printf("\t\t1.1 Initialization\n");

         // copy-initialization
        const bool bc = bt;
        ASSERT(!bc);

        // value-initialization
        const bool bv(bt);
        ASSERT(!bv);

        // reference initialization
        const bool& br = bt;
        ASSERT(!br);

        // aggregate initialization (array)
        const bool bar[] = { bt };
        ASSERT(!bar[0]);

        // aggregate initialization
        struct Aggregate {
            bool data;
        } bag = { bt };
        ASSERT(!bag.data);

        // member initialization
        struct MemberTest {
            bool data;

            MemberTest(BoolType b) : data(b) {}
        } bmt = bt;
        ASSERT(!bmt.data);

        if (veryVerbose) printf("\t\t1.2 Function argument\n");
        struct TestFunctionArgument {
            // Test as default argument, although it is hard to see a use-case
            static bool call(bool result = BoolType()) { return result; }
        };
        ASSERT(!TestFunctionArgument::call(bt));  // Explicitly passed as arg.

        if (veryVerbose) printf("\t\t1.3 Return statement\n");
        struct TestReturn {
            static bool call() { return BoolType(); }
        };
        ASSERT(!TestReturn::call());

        if (veryVerbose) printf("\t\t1.4 Assignment\n");
        bool bass = true;
        bass = bt;
        ASSERT(!bass);

        if (veryVerbose) printf("\t\t1.5 Cast expressions\n");
        // static_cast
        bool bsc = static_cast<bool>(bt);
        ASSERT(!bsc);

        // C-style cast
        bool boldc = (bool)bt;
        ASSERT(!boldc);

        if (veryVerbose) printf("\t\t1.6 operator!\n");
        ASSERT(typeid(bool)!=typeid(bt));
        ASSERT(typeid(bool)==typeid(!bt));
        if (!bt) {
            ASSERT(true);
        }
        else {
            ASSERT(false);
        }

        if (veryVerbose) printf("\t\t1.7 operator|| and &&\n");
        struct DidNotShortCircuit {
            static bool call() { ASSERT(false); return false; }
                // Return a 'bool' value.  Note that this function asserts if
                // it is ever called, in order to demonstrate that boolean
                // short-circuit evaluation was not honoured.
        };
        if(!bt || DidNotShortCircuit::call()) {
            ASSERT(true);
        }
        else {
            ASSERT(false);
        }

        if(bt && DidNotShortCircuit::call()) {
            ASSERT(false);
        }

        if (veryVerbose) printf("\t\t1.8 if clause\n");
        if (bt) {
            ASSERT(false);
        }

        if (veryVerbose) printf("\t\t1.9 while clause\n");
        while (bt) {
            ASSERT(false);
        }

        do {} while (bt);

        if (veryVerbose) printf("\t\t1.10 for loop\n");
        for ( ; bt; ) {
            ASSERT(false);
        }

        if (veryVerbose) printf("\t\t1.11 Ternary operator\n");
        int ix = bt ? 0 : 9;
        ASSERT(9 == ix);

        if (veryVerbose) printf("\t\t1.12 Comma operator\n");
        bool bx = (true, bt);
        ASSERT(!bx);

        }

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) printf("\t2. BoolType does not promote to int\n");

        {

        struct TestPromoteToInt {
            static bool call(...) { return false; }
            static bool call(int) { return true;  }
        };

        const BoolType bt = BoolType();
        ASSERT(TestPromoteToInt::call(0));      // verify 'int' is detected
        ASSERT(!TestPromoteToInt::call(bt));    // verify 'bt' does not promote

        }

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) printf("\t3. BoolType does not convert to pointer\n");

        {

        struct TestConvertToPointer {
            static bool call(...) { return false; }
            static bool call(void *) { return true;  }
            static bool call(const void *) { return true;  }
            static bool call(volatile void *) { return true;  }
            static bool call(const volatile void *) { return true;  }
        };

        const BoolType bt = BoolType();
        ASSERT(TestConvertToPointer::call((void*)0));  // show 'void*' detected
        ASSERT(!TestConvertToPointer::call(bt));       // show 'bt' is not

        }

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) printf("\t4. BoolType default value converts to false\n");

        {

        const BoolType bt = BoolType();
        ASSERT(bt == false);
        ASSERT(false == bt);
        ASSERT(!bt);

        }

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) printf("\t5. BoolType(0) converts to false\n");

        {

        const BoolType b0 = 0;
        ASSERT(b0 == false);
        ASSERT(false == b0);
        ASSERT(!b0);

        }

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) printf("\t6. Testing BoolType conversion operator\n");

        {

        struct Booleable {
            typedef bsls_UnspecifiedBool<Booleable>::BoolType BoolType;

            operator BoolType() const { return false; }
        };

        const Booleable babel;
        ASSERT(babel == false);
        ASSERT(false == babel);
        ASSERT(!babel);

        if (veryVerbose) printf("\t\t6.1 Initialization\n");

         // copy-initialization
        const bool bc = babel;
        ASSERT(!bc);

        // value-initialization
        const bool bv(babel);
        ASSERT(!bv);

        // reference initialization
        const bool& br = babel;
        ASSERT(!br);

        // aggregate initialization (array)
        const bool bar[] = { babel };
        ASSERT(!bar[0]);

        // aggregate initialization
        struct Aggregate {
            bool data;
        } bag = { babel };
        ASSERT(!bag.data);

        // member initialization
        struct MemberTest {
            bool data;

            MemberTest(Booleable b) : data(b) {}
        } bmt = babel;
        ASSERT(!bmt.data);

        if (veryVerbose) printf("\t\t6.2 Function argument\n");
        struct TestFunctionArgument {
            // Test as default argument, although it is hard to see a use-case
            static bool call(bool result = Booleable()) { return result; }
        };
        ASSERT(!TestFunctionArgument::call(babel));// Explicitly passed as arg.

        if (veryVerbose) printf("\t\t6.3 Return statement\n");
        struct TestReturn {
            static bool call() { return Booleable(); }
        };
        ASSERT(!TestReturn::call());

        if (veryVerbose) printf("\t\t6.4 Assignment\n");
        bool bass = true;
        bass = babel;
        ASSERT(!bass);

        if (veryVerbose) printf("\t\t6.5 Cast expressions\n");
        // static_cast
        bool bsc = static_cast<bool>(babel);
        ASSERT(!bsc);

        // C-style cast
        bool boldc = (bool)babel;
        ASSERT(!boldc);

        if (veryVerbose) printf("\t\t6.6 operator!\n");
        ASSERT(typeid(bool)!=typeid(babel));
        ASSERT(typeid(bool)==typeid(!babel));
        if (!babel) {
            ASSERT(true);
        }
        else {
            ASSERT(false);
        }

        if (veryVerbose) printf("\t\t6.7 operator|| and &&\n");
        struct DidNotShortCircuit {
            static bool call() { ASSERT(false); return false; }
                // Return a 'bool' value.  Note that this function asserts if
                // it is ever called, in order to demonstrate that boolean
                // short-circuit evaluation was not honoured.
        };
        if(!babel || DidNotShortCircuit::call()) {
            ASSERT(true);
        }
        else {
            ASSERT(false);
        }

        if(babel && DidNotShortCircuit::call()) {
            ASSERT(false);
        }

        if (veryVerbose) printf("\t\t6.8 if clause\n");
        if (babel) {
            ASSERT(false);
        }

        if (veryVerbose) printf("\t\t6.9 while clause\n");
        while (babel) {
            ASSERT(false);
        }

        do {} while (babel);

        if (veryVerbose) printf("\t\t6.10 for loop\n");
        for ( ; babel; ) {
            ASSERT(false);
        }

        if (veryVerbose) printf("\t\t6.11 Ternary operator\n");
        int ix = babel ? 0 : 9;
        ASSERT(9 == ix);

        if (veryVerbose) printf("\t\t6.12 Comma operator\n");
        bool bx = (true, babel);
        ASSERT(!bx);

        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //
        // Concerns:
        //: 1 That the functions exist with the documented signatures.
        //: 2 That the basic functionality works as documented.
        //
        // Plan:
        //   Exercise each function in turn and devise an elementary test
        //   sequence to ensure that the basic functionality is as documented.
        //
        // Testing:
        //   This test exercises basic functionality but *tests* *nothing*.
        // --------------------------------------------------------------------

        if (verbose) printf("\nBREATHING TEST"
                            "\n==============\n");

        typedef bsls_UnspecifiedBool<void> HostType;
        ASSERT(!HostType::falseValue());
        if (HostType::trueValue()) {
            ASSERT(true);
        }
        else {
            ASSERT(false);
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
