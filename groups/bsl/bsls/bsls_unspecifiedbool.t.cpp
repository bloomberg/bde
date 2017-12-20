// bsls_unspecifiedbool.t.cpp                                         -*-C++-*-

#include <bsls_unspecifiedbool.h>

#include <bsls_bsltestutil.h>    // for testing purposes only

#include <stdio.h>
#include <stdlib.h>     // atoi()
#include <typeinfo>     // for typeid, but bad levelization.

using namespace BloombergLP;

//=============================================================================
//                             TEST PLAN
//                             ---------
// The 'bsls_unspecifiedbool' component provides a class supplying the
// 'typedef' for a type suitable for use in a conversion operator for types
// that must be convertible to bool, but without suffering the accidental
// integral promotions that would result if a true 'operator bool' was defined.
// It further provides two static member functions that return values of this
// type equivalent to 'true' and 'false'.  In order to completely test this
// component we must prove that the aliases type serves the advertized purpose
// of supporting only the desired boolean conversions, and not the broader set
// of integral conversions.  Then we must demonstrate that the two functions
// return values with the correct properties.  Finally, we will compile and run
// the usage example.
//
//-----------------------------------------------------------------------------
// [ 3] BoolType falseValue();
// [ 3] BoolType trueValue();
// [ 3] BoolType makeValue(bool);
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
//                         HELPER CLASSES FOR TESTING
//-----------------------------------------------------------------------------

struct Booleable {
    typedef bsls::UnspecifiedBool<Booleable>::BoolType BoolType;

    operator BoolType() const
    {
        return bsls::UnspecifiedBool<Booleable>::falseValue();
    }
};

}  // close unnamed namespace

//=============================================================================
//                                USAGE EXAMPLE
//-----------------------------------------------------------------------------

namespace USAGE_EXAMPLE {

///Usage
///-----
// In this section we show intended usage of this component.
//
///Example 1: A Simple Smart Pointer
///- - - - - - - - - - - - - - - - -
// A common requirement for "smart pointer" types is to emulate the native
// pointer types and, in particular, support testing for "null" or "empty"
// pointer values as a simple boolean conversion in 'if' and 'while' clauses.
// We here demonstrate how to create a simple smart pointer type, 'SimplePtr',
// using this component to implement a safe the boolean conversion.
//
// An object of type 'SimplePtr' holds a pointer value, but does not claim
// ownership or any responsibility for the lifetime of the referenced object.
// A 'SimplePtr' object acts as a "simple" native pointer.
//
// First, we create the 'SimplePtr' class, define its data members, creators
// and manipulators:
//..
    template <class TYPE>
    class SimplePtr
    {
        // This class holds a pointer to a single object, and provides a subset
        // of the regular pointer operators.  For example, objects of this
        // class can be dereferenced with 'operator*' and tested as a boolean
        // value to determine if null.  Conversely, this class does not support
        // pointer arithmetic.

      private:
        // DATA
        TYPE *d_ptr_p;  // address of the referenced object

        // PRIVATE ACCESSORS
        bool operator==(const SimplePtr &);  // = delete;
        bool operator!=(const SimplePtr &);  // = delete;
            // Suppress equality comparison operations on objects of this
            // class.

      public:
        // CREATORS
        explicit SimplePtr(TYPE *ptr = 0) : d_ptr_p(ptr) {}
            // Create a 'SimplePtr' having the value of the specified 'ptr'.

        //! ~SimplePtr() = default;
            // Destroy this object.

        // ACCESSORS
        TYPE& operator*() const  { return *d_ptr_p; }
            // Return a reference to the object pointed to by this
            // 'SimplePtr'.

        TYPE *operator->() const { return d_ptr_p; }
            // Return the held 'd_ptr_p'.
//..
// Next, we define, for convenience, an alias for a unique type that is
// implicitly convertible to 'bool' (note that we pass the current template
// instantiation to the 'bsls::UnspecifiedBool' template to guarantee
// a unique name, even for different instantiations of this same 'SimplePtr'
// template):
//..
        // TYPES
        typedef typename bsls::UnspecifiedBool<SimplePtr>::BoolType BoolType;
//..
// Now, we can define a boolean conversion operator that tests whether or not
// this 'SimplePtr' object is holding a null pointer, or a valid address:
//..
        operator BoolType() const {
            return bsls::UnspecifiedBool<SimplePtr>::makeValue(d_ptr_p);
        }
    }; // class SimplePtr
//..
// Note that we do not need to define 'operator!' as this single boolean
// conversion operator is invoked with the correct semantics when the user
// tries that operator.
//
// Finally, we write a simple test function, creating a couple of 'SimplePtr'
// objects, one "null", and the other with a well-defined address.
//..
    void runTests() {
        SimplePtr<int> p1;  // default ctor sets to null
        ASSERT(!p1);

        int            i = 3;
        SimplePtr<int> p2(&i);

        if (p2) {
            ASSERT(3 == *p2);
        }
    }
//..
// Notice that 'SimplePtr' objects behave as native pointers.  They should
// be tested before dereferencing (as they could be null).

}  // close namespace USAGE_EXAMPLE

//=============================================================================
//                              TEST PROGRAM
//-----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
    int        test = argc > 1 ? atoi(argv[1]) : 0;
    int     verbose = argc > 2;
    int veryVerbose = argc > 3;

    (void)     verbose;
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

        if (verbose) printf("\nUSAGE EXAMPLE"
                            "\n=============\n");

        USAGE_EXAMPLE::runTests();

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // UTILITY FUNCTIONS: 'trueValue', 'falseValue' and 'makeValue'
        //
        // Concerns:
        //: 1 The static member functions 'trueValue', 'falseValue' and
        //:   'makeValue' each return a value of type BoolType.
        //: 2 'falseValue' returns a value that converts to the 'bool' value
        //:   'false'
        //: 3 'trueValue' returns a value that converts to the 'bool' value
        //:   'true'
        //: 4 'makeValue' returns a value that converts to 'true' or 'false'
        //:    depending on the input parameter.
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
        //  BoolType makeValue()
        // --------------------------------------------------------------------

        if (verbose) printf(
                        "\nUTILITY FUNCTIONS: '{true,false,make}Value'"
                        "\n-----------------------------------------------\n");

        if (verbose) printf("\t1. Functions must return BoolType values\n");

        typedef bsls::UnspecifiedBool<int(int)> HostType;
        typedef HostType::BoolType        BoolType;

        typedef BoolType (*FuncType)();

        FuncType falseFunc = &HostType::falseValue;
        FuncType trueFunc  = &HostType::trueValue;

        // Silence unused variable warnings.
        (void) falseFunc;
        (void) trueFunc;

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) printf("\t2. 'falseValue()' converts to 'false'\n");

        const bool bFalse = HostType::falseValue();
        LOOP2_ASSERT(L_, bFalse, false == bFalse);

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) printf("\t3. 'trueValue()' converts to 'true'\n");

        const bool bTrue = HostType::trueValue();
        LOOP2_ASSERT(L_, bTrue, true == bTrue);

        if (verbose) printf("\t4. 'makeValue()' makes the correct value\n");

        const bool madeTrue = HostType::makeValue(true);
        LOOP2_ASSERT(L_, madeTrue, true == madeTrue);

        const bool madeFalse = HostType::makeValue(false);
        LOOP2_ASSERT(L_, madeFalse, false == madeFalse);

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // PROPERTIES: bsls::UnspecifiedBool<TYPE>::BoolType
        //
        // The type 'bsls::UnspecifiedBool<TYPE>::BoolType' is intended to
        // substitute as a boolean type; therefore, in C-1 and C-6 below, we
        // will test it in all contexts where the language demands a boolean
        // type.  The exhaustive list of occasions in which a contextual
        // conversion to 'bool' may occur is:
        //:    1 Initialization
        //:    2 Passing as an argument to a function
        //:    3 In a return expression
        //:    4 Assignment
        //:    5 In a cast expression
        //:    6 In conjunction with 'operator!'
        //:    7 In conjunction with 'operator &&' or 'operator||',
        //:      preserving boolean short-circuit semantics
        //:    8 Testing in an 'if' clause
        //:    9 Testing in a 'while' loop
        //:   10 Testing in a 'for' loop
        //:   11 Testing as the condition of a ternary '?:' expression
        //:   12 Passes safely through a ',' operator
        //
        // Note that while there may be additional contexts to test in C++11
        // we have no interest in listing or testing them, as in that case
        // this whole idiom and component should be replaced with
        // 'explicit operator bool()'.
        //
        // Concerns:
        //: 1 Objects of type 'bsls::UnspecifiedBool<TYPE>::BoolType' must be
        //:   contextually convertible to 'bool'.
        //:
        //: 2 Objects of type 'bsls::UnspecifiedBool<TYPE>::BoolType' must not
        //:   promote to type 'int'.
        //:
        //: 3 Objects of type 'bsls::UnspecifiedBool<TYPE>::BoolType' must not
        //:   convert to any native pointer type.
        //:
        //: 4 A default constructed 'bsls::UnspecifiedBool<TYPE>::BoolType'
        //:   object, when converted to 'bool', has the value 'false',
        //:   the same value as a default constructed 'bool'.
        //:
        //: 5 A 'bsls::UnspecifiedBool<TYPE>::BoolType' object initialized with
        //:   the literal '0', when converted to a 'bool', has the value
        //:   'false'.
        //:
        //: 6 A class with a conversion operator to type
        //:   'bsls::UnspecifiedBool<TYPE>::BoolType' should be implicitly
        //:   convertible to 'bool'.
        //:
        //: 7 Two classes that are implicitly convertible to different
        //:   instantiations of type 'bsls::UnspecifiedBool<TYPE>::BoolType'
        //:   should not accidentally be comparable to each other using
        //:   'operator=='.
        //
        // Plan:
        //: 1 Evaluate a value of type 'bsls::UnspecifiedBool<TYPE>::BoolType'
        //:   in each situation where the language supports a contextual
        //:   conversion to a boolean type.  This must compile.  (C1)
        //:
        //: 2 Define a pair of overloaded functions returning a 'bool' value,
        //:   one taking a single 'int' and returning 'true', the other
        //:   accepting anything through an ellipsis parameter
        //:   'false'.  Call this function function with a value of type
        //:   'bsls::UnspecifiedBool<TYPE>::BoolType' to prove the 'int'
        //:   overload is *not* selected. (C2)
        //:
        //: 3 Define a set of overloaded functions returning a 'bool' value.
        //:   Four overloaded functions take a single pointer argument,
        //:   covering all four cv-qualification combinations on a 'void *'
        //:   pointer, and return 'true'.  The other overload will accept
        //:   anything through an ellipsis parameter and return 'false'.  Call
        //:   this function function with a value of type
        //:   'bsls::UnspecifiedBool<TYPE>::BoolType' to prove that none of the
        //:   pointer-parameter overloads are selected. (C3)
        //:
        //: 4 Create a value-initialized object of type
        //:   'bsls::UnspecifiedBool<TYPE>::BoolType' and assert that its value
        //:   when converted to 'bool' is 'false'. (C4)
        //:
        //: 5 Create an object of type 'bsls::UnspecifiedBool<TYPE>::BoolType'
        //:   and initialize it with the literal '0'.  Assert that its value
        //:   when converted to 'bool' is 'false'. (C5)
        //:
        //: 6 Define a new type, 'Booleable', with a conversion operator
        //:   converting to type 'bsls::UnspecifiedBool<Booleable>::BoolType'.
        //:   Evaluate a value of type 'Booleable' in each context where the
        //:   language supports an implicit conversion to a boolean type.  (C6)
        //:
        //: 7 TBD (C7)
        //
        // Testing:
        //   typedef bsls::UnspecifiedBool::BoolType
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING bsls::UnspecifiedBool"
                            "\n-----------------------------\n");

        typedef bsls::UnspecifiedBool<int>::BoolType BoolType;

        if (verbose) printf("\t1. 'BoolType' implicitly converts to 'bool'\n");

        {
            const BoolType bt = BoolType();

            if (veryVerbose) printf("\t\t1.1 Initialization\n");

             // copy-initialization
            const bool bc = bt;
            ASSERT(!bc);

            // direct-initialization
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

                MemberTest(BoolType b) : data(b) {}                 // IMPLICIT
            } bmt = bt;
            ASSERT(!bmt.data);

            if (veryVerbose) printf("\t\t1.2 Function argument\n");

            struct TestFunctionArgument {
                // Test as default argument, although it is hard to see a
                // use-case
                static bool call(bool result = BoolType()) { return result; }
            };
            ASSERT(!TestFunctionArgument::call(bt));  // Explicitly passed arg.

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

            if (veryVerbose) printf("\t\t1.6 'operator!'\n");

            ASSERT(typeid(bool) != typeid(bt));
            ASSERT(typeid(bool) == typeid(!bt));
            if (!bt) {
                ASSERT(true);
            }
            else {
                ASSERT(false);
            }

            if (veryVerbose) printf("\t\t1.7 'operator||' and 'operator&&'\n");

            struct DidNotShortCircuit {
                static bool call() { ASSERT(false); return false; }
                    // Return a 'bool' value.  Note that this function asserts
                    // if it is ever called, in order to demonstrate that
                    // boolean short-circuit evaluation was not honored.
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

            if (veryVerbose) printf("\t\t1.8 'if' clause\n");

            if (bt) {
                ASSERT(false);
            }

            if (veryVerbose) printf("\t\t1.9 'while' clauses\n");

            while (bt) {
                ASSERT(false);
            }

            do {} while (bt);

            if (veryVerbose) printf("\t\t1.10 'for' loop\n");

            for ( ; bt; ) {
                ASSERT(false);
            }

            if (veryVerbose) printf("\t\t1.11 Ternary operator\n");

            int ix = bt ? 0 : 9;
            ASSERT(9 == ix);

            if (veryVerbose) printf("\t\t1.12 Comma operator\n");

            // Note that compilers (i.e., gcc and clang) will give a warning if
            // the left-hand side of a comma operator computes a result that is
            // not explicitly used or contains no side effects.  gcc recommends
            // casting the unused expression to 'void'.

            bool bx = (static_cast<void>(true), bt);
            ASSERT(!bx);

        }

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) printf("\t2. 'BoolType' does not promote to 'int'\n");

        {
            struct TestPromoteToInt {
                static bool call(...) { return false; }
                static bool call(int) { return true;  }
            };

            const BoolType bt = BoolType();
            ASSERT( TestPromoteToInt::call(0));  // 'int' is detected
            ASSERT(!TestPromoteToInt::call(bt)); // 'bt' does not promote
        }

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) printf("\t3. 'BoolType' does not convert to pointer\n");

        {
            struct TestConvertToPointer {
                static bool call(...) { return false; }
                static bool call(void *) { return true;  }
                static bool call(const void *) { return true;  }
                static bool call(volatile void *) { return true;  }
                static bool call(const volatile void *) { return true;  }
            };

            const BoolType bt = BoolType();
            ASSERT( TestConvertToPointer::call((void*)0)); // 'void*' detected
            ASSERT(!TestConvertToPointer::call(bt));       // 'bt' is not
        }

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) printf(
                        "\t4. 'BoolType' default value converts to 'false'\n");

        {
            // Perform non-contextual explicit conversion in order to compare
            // to 'false'.
            const BoolType bt = BoolType();
            const bool vt = bt;
            ASSERT(vt == false);
            ASSERT(!bt);
        }

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) printf("\t5. 'BoolType(0)' explicitly converts to "
                            "'false'\n");

        {
            const BoolType b0 = 0;
            const bool vt = b0;
            ASSERT(vt  == false);
            ASSERT(!b0);
        }

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) printf("\t6. Testing 'BoolType' conversion operator\n");

        {
            // note this variable must be static to work around a stack
            // corruption bug on VC2008 when initializing a variable of
            // empty class type.
            static const Booleable babel = {};
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

                MemberTest(Booleable b) : data(b) {}                // IMPLICIT
            } bmt = babel;
            ASSERT(!bmt.data);

            if (veryVerbose) printf("\t\t6.2 Function argument\n");

            struct TestFunctionArgument {
                // Test as default argument, although it is hard to see a
                // use-case
                static bool call(bool result = Booleable()) { return result; }
            };
            ASSERT(!TestFunctionArgument::call(babel));  // Explicitly passed
                                                         //  as arg.

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

            if (veryVerbose) printf("\t\t6.6 'operator!'\n");

            ASSERT(typeid(bool) != typeid(babel));
            ASSERT(typeid(bool) == typeid(!babel));
            if (!babel) {
                ASSERT(true);
            }
            else {
                ASSERT(false);
            }

            if (veryVerbose) printf("\t\t6.7 'operator||' and 'operator&&'\n");

            struct DidNotShortCircuit {
                static bool call() { ASSERT(false); return false; }
                    // Return a 'bool' value.  Note that this function asserts
                    // if it is ever called, in order to demonstrate that
                    // boolean short-circuit evaluation was not honored.
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

            if (veryVerbose) printf("\t\t6.8 'if' clause\n");

            if (babel) {
                ASSERT(false);
            }

            if (veryVerbose) printf("\t\t6.9 'while' clause\n");

            while (babel) {
                ASSERT(false);
            }

            do {} while (babel);

            if (veryVerbose) printf("\t\t6.10 'for' loop\n");

            for ( ; babel; ) {
                ASSERT(false);
            }

            if (veryVerbose) printf("\t\t6.11 Ternary operator\n");

            int ix = babel ? 0 : 9;
            ASSERT(9 == ix);

            if (veryVerbose) printf("\t\t6.12 Comma operator\n");

            // Note that compilers (i.e., gcc and clang) will give a warning if
            // the left-hand side of a comma operator computes a result that is
            // not explicitly used or contains no side effects.  gcc recommends
            // casting the unused expression to 'void'.

            bool bx = (static_cast<void>(true), babel);
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

        typedef bsls::UnspecifiedBool<void> HostType;
        ASSERT(!HostType::falseValue());
        if (HostType::trueValue()) {
            ASSERT(true);
        }
        else {
            ASSERT(false);
        }

        ASSERT(HostType::makeValue(true));
        ASSERT(!HostType::makeValue(false));
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
