// bslmf_forwardingtype.t.cpp                                         -*-C++-*-
#include <bslmf_forwardingtype.h>

#include <bslmf_isarray.h>
#include <bslmf_issame.h>          // for testing only

#include <bsls_platform.h>

#include <stdlib.h>    // atoi()
#include <iostream>
#include <typeinfo>

using namespace BloombergLP;
using namespace std;

//=============================================================================
//                                TEST PLAN
//-----------------------------------------------------------------------------
//                                Overview
//                                --------
// This component provides two meta-functions.  We simply verify that they
// return the proper type for a list of suitably chosen arguments.
//-----------------------------------------------------------------------------
// [ 1] bslmf::ForwardingType
// [ 2] bslmf::ConstForwardingType
// [ 3] USAGE EXAMPLE
//=============================================================================
//                  STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
static int testStatus = 0;

static void aSsErT(int c, const char *s, int i) {
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}
#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

//-----------------------------------------------------------------------------
#define LOOP_ASSERT(I,X) { \
    if (!(X)) { cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__);}}

#define LOOP2_ASSERT(I,J,X) { \
    if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " \
        << J << "\n"; aSsErT(1, #X, __LINE__); } }
//=============================================================================
#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", " << flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number
#define T_() cout << '\t' << flush;           // Print tab w/o linefeed.

#define ASSERT_SAME(X, Y) ASSERT(1 == (bsl::is_same<X, Y>::value))

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

enum   Enum   {};
struct Struct {};
union  Union  {};
class  Class  {};

typedef int INT;

typedef void      F ();
typedef void ( & RF)();
typedef void (*  PF)();
typedef void (*&RPF)();

typedef void    Fi  (int);
typedef void (&RFi) (int);
typedef void (*PFi) (int);
typedef void    FRi (int&);
typedef void (&RFRi)(int&);

typedef char    A [5];
typedef char (&RA)[5];

//=============================================================================
//                           USAGE EXAMPLES
//-----------------------------------------------------------------------------

///Usage
///-----
// These examples demonstrate the expected use of this component.
// In this section we show intended use of this component.
//
///Example 1: Direct look at metafunction results
///- - - - - - - - - - - - - - - - - - - - - - -
// In this example, we invoke 'ForwardingType' on a variety of types and look
// at the resulting 'Type' member:
//..
    struct MyType {};
    typedef MyType& MyTypeRef;

    void usageExample1()
    {
        typedef int                    T1;
        typedef int&                   T2;
        typedef const volatile double& T3;
        typedef const double &         T4;
        typedef const float * &        T5;
        typedef const float * const &  T6;
        typedef MyType                 T7;
        typedef const MyType&          T8;
        typedef MyType&                T9;
        typedef MyType*                T10;

        typedef int                    EXP1;
        typedef int&                   EXP2;
        typedef const volatile double& EXP3;
        typedef const double &         EXP4;
        typedef const float * &        EXP5;
        typedef const float * const &  EXP6;
        typedef const MyType&          EXP7;
        typedef const MyType&          EXP8;
        typedef MyType&                EXP9;
        typedef MyType*                EXP10;

        ASSERT((bsl::is_same<bslmf::ForwardingType<T1>::Type, EXP1>::value));
        ASSERT((bsl::is_same<bslmf::ForwardingType<T2>::Type, EXP2>::value));
        ASSERT((bsl::is_same<bslmf::ForwardingType<T3>::Type, EXP3>::value));
        ASSERT((bsl::is_same<bslmf::ForwardingType<T4>::Type, EXP4>::value));
        ASSERT((bsl::is_same<bslmf::ForwardingType<T5>::Type, EXP5>::value));
        ASSERT((bsl::is_same<bslmf::ForwardingType<T6>::Type, EXP6>::value));
        ASSERT((bsl::is_same<bslmf::ForwardingType<T7>::Type, EXP7>::value));
        ASSERT((bsl::is_same<bslmf::ForwardingType<T8>::Type, EXP8>::value));
        ASSERT((bsl::is_same<bslmf::ForwardingType<T9>::Type, EXP9>::value));
        ASSERT((bsl::is_same<bslmf::ForwardingType<T10>::Type, EXP10>::value));
    }
//..
//
///Example 2: A logging invocation wrapper
///- - - - - - - - - - - - - - - - - - - - - - -
// This example illustrates the use of 'ForwardingType' to efficiently
// implement a wrapper class that holds a function pointer and logs
// information about each call to the pointed-to-function through the wrapper.
// The pointed-to-function takes three arguments whose types a4re specified
// via template arguments.  The first argument is required to be convertible
// to 'int'.  The class definition looks as follows:
//..
    // Primary template is never defined
    template <class PROTOTYPE> class LoggingWrapper;

    template <class RET, class ARG1, class ARG2, class ARG3>
    class LoggingWrapper<RET(ARG1, ARG2, ARG3)> {
        // Specialization of wrapper for specified function prototype.

        RET (*d_function_p)(ARG1, ARG2, ARG3);

    public:
        LoggingWrapper(RET (*function_p)(ARG1, ARG2, ARG3))
          : d_function_p(function_p) { }

        RET operator()(ARG1, ARG2, ARG3) const;
//..
// Next, we declare a private member function that actually invokes the
// function. This member function will be called by 'operator()' and must
// therefore receive arguments indirectly through 'operator()'. In order to
// avoid excessive copies of pass-by-value arguments, we use 'ForwardingType'
// to declare a more efficient intermediate argument type for our private
// member function:
//..
    private:
        RET invoke(typename bslmf::ForwardingType<ARG1>::Type a1,
                   typename bslmf::ForwardingType<ARG2>::Type a2,
                   typename bslmf::ForwardingType<ARG3>::Type a3) const;
    };
//..
// Next, we define logging functions that simply count the number of
// invocations and return from invocations (e.g., to count how may invocations
// completed without exceptions):
//..
    int invocations = 0, returns = 0;
    void logInvocation(int /* ignored */) { ++invocations; }
    void logReturn(int /* ignored */) { ++returns; }
//..
// Next, we implement 'operator()' to call the logging functions and call
// 'invoke()':
//..
    template <class RET, class ARG1, class ARG2, class ARG3>
    RET LoggingWrapper<RET(ARG1, ARG2, ARG3)>::operator()(ARG1 a1,
                                                          ARG2 a2,
                                                          ARG3 a3) const {
        logInvocation(a1);
        RET r = invoke(a1, a2, a3);
        logReturn(a1);
        return r;
    }
//..
// Next, we implement 'invoke()' to actually call the function through the
// pointer. To reconstitute the arguments to the function as close as possible
// to the types they were passed in as, we call the 'forwardToTarget' member
// of 'ForwardingTypeUtil':
//..
    template <class RET, class ARG1, class ARG2, class ARG3>
    RET LoggingWrapper<RET(ARG1,ARG2,ARG3)>::invoke(
        typename bslmf::ForwardingType<ARG1>::Type a1,
        typename bslmf::ForwardingType<ARG2>::Type a2,
        typename bslmf::ForwardingType<ARG3>::Type a3) const
    {
        return d_function_p(
            bslmf::ForwardingTypeUtil<ARG1>::forwardToTarget(a1),
            bslmf::ForwardingTypeUtil<ARG2>::forwardToTarget(a2),
            bslmf::ForwardingTypeUtil<ARG3>::forwardToTarget(a3));
    }
//..
// Next, in order to see this wrapper in action, we must define the function
// we wish to wrap.  This function will take an argument of type 'ArgType',
// which, among other things, keeps track of whether it has been directly
// constructed or copied from anther 'ArgType' object.  If it has been copied,
// it keeps track of how many "generations" of copy were done:
//..
    class ArgType {
        int d_value;
        int d_copies;
    public:
        ArgType(int v = 0) : d_value(v), d_copies(0) { }
        ArgType(const ArgType& other)
          : d_value(other.d_value)
          , d_copies(other.d_copies + 1) { }

        int value() const { return d_value; }
        int copies() const { return d_copies; }
    };

    int myFunc(const short& i, ArgType& x, ArgType y)
        // Assign 'x' the value of 'y' and return the 'value()' of 'x'.
    {
        ASSERT(i == y.copies());
        x = y;
        return x.value();
    }
//..
// Finally, we create a instance of 'LoggingWrapper' to wrap 'myFunc', and we
// invoke it.  Note that 'y' is copied into the second argument of
// 'operator()' and is copied again when 'myFunc' is invoked.  However, it is
// *not* copied when 'operator()' calls 'invoke()' because the 'ForwardType'
// of 'ArgType' is 'const ArgType&', which does not create another copy.  In
// C++11, if 'ArgType' had a move constructor, then the number of copies would
// be only 1, since the final forwarding would be a move instead of a copy.
//..
    void usageExample2() {
        ArgType x(0);
        ArgType y(99);

        LoggingWrapper<int(const short&, ArgType&, ArgType)> lw(myFunc);
        ASSERT(0 == invocations && 0 == returns);
        lw(2, x, y);  // Expect two copies of 'y'
        ASSERT(1 == invocations && 1 == returns);
        ASSERT(99 == x.value());
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

    (void) verbose;      // eliminate unused variable warning
    (void) veryVerbose;  // eliminate unused variable warning

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:  // Zero is always the leading case.
      case 3: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //   Simple example illustrating use of 'bsl::remove_reference'.
        //
        // Concerns:
        //
        // Plan:
        //
        // Tactics:
        //   - Add-Hoc Data Selection Method
        //   - Brute-Force implementation technique
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "USAGE EXAMPLE" << endl
                                  << "=============" << endl;

        usageExample1();
        usageExample2();

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // Test Plan:
        //   Instantiate 'bslmf::ConstForwardingType' with various types and
        //   verify that its 'Type' typedef is set to the expected type.
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "bslmf::ConstForwardingType" << endl
                          << "==========================" << endl;

        ASSERT_SAME(bslmf::ConstForwardingType<int       >::Type, int);
        ASSERT_SAME(bslmf::ConstForwardingType<int&      >::Type, int&);
        if (verbose)
            P(bslmf::ConstForwardingType<int&>::BSLMF_FORWARDING_TYPE_ID);
        ASSERT_SAME(bslmf::ConstForwardingType<int const&>::Type, const int&);

        ASSERT_SAME(bslmf::ConstForwardingType<void *>::Type, void *);
        if (verbose)
            P(bslmf::ConstForwardingType<void *>::BSLMF_FORWARDING_TYPE_ID);
        ASSERT_SAME(bslmf::ConstForwardingType<void *&>::Type,
                    void *&);
        ASSERT_SAME(bslmf::ConstForwardingType<void volatile *&>::Type,
                    volatile void                            *&);

        ASSERT_SAME(bslmf::ConstForwardingType<char const *const&>::Type,
                    char const *const &);

        ASSERT_SAME(bslmf::ConstForwardingType<Enum    >::Type, Enum);
        ASSERT_SAME(bslmf::ConstForwardingType<Enum&   >::Type, Enum&);
        ASSERT_SAME(bslmf::ConstForwardingType<Struct  >::Type, const Struct&);
        ASSERT_SAME(bslmf::ConstForwardingType<Struct& >::Type, Struct&);
        ASSERT_SAME(bslmf::ConstForwardingType<Union   >::Type, const Union&);
        ASSERT_SAME(bslmf::ConstForwardingType<Union&  >::Type, Union&);
        ASSERT(0 == bsl::is_array<Class>::value);
        ASSERT_SAME(bslmf::ConstForwardingType<Class   >::Type,const Class&);
        if (verbose)
            P(bslmf::ConstForwardingType<Class>::BSLMF_FORWARDING_TYPE_ID);
        ASSERT_SAME(bslmf::ConstForwardingType<const Class&>::Type,
                    const Class&);

        ASSERT_SAME(bslmf::ConstForwardingType<INT >::Type, int);
        ASSERT_SAME(bslmf::ConstForwardingType<INT&>::Type, int&);

        ASSERT_SAME(bslmf::ConstForwardingType<int Class:: *>::Type,
                    int Class::*);
        ASSERT_SAME(bslmf::ConstForwardingType<int Class::* const& >::Type,
                    int                            Class::* const&);

        ASSERT_SAME(bslmf::ConstForwardingType<int Class::*&>::Type,
                    int                            Class::*&);

        ASSERT_SAME(bslmf::ConstForwardingType< PF>::Type, PF);
        ASSERT_SAME(bslmf::ConstForwardingType<RPF>::Type, PF&);

        ASSERT_SAME(bslmf::ConstForwardingType< A>::Type, char*);
        ASSERT_SAME(bslmf::ConstForwardingType<RA>::Type, char*);
        if (verbose)
            P(bslmf::ConstForwardingType<RA>::BSLMF_FORWARDING_TYPE_ID);
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // Test Plan:
        //   Instantiate 'bslmf::ForwardingType' with various types and verify
        //   that its 'Type' typedef is set to the expected type.
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "bslmf::ForwardingType" << endl
                          << "======================" << endl;

        ASSERT_SAME(bslmf::ForwardingType<int       >::Type, int);
        if (verbose) P(typeid(bslmf::ForwardingType<int>::Type).name());
        ASSERT_SAME(bslmf::ForwardingType<int&      >::Type, int&);
        ASSERT_SAME(bslmf::ForwardingType<int const&>::Type, int const&);

        ASSERT_SAME(bslmf::ForwardingType<void *          >::Type, void *);
        ASSERT_SAME(bslmf::ForwardingType<void *&         >::Type, void *&);
        ASSERT_SAME(bslmf::ForwardingType<void volatile *&>::Type,
                    volatile void *&);

        ASSERT_SAME(bslmf::ForwardingType<char const *const&>::Type,
                    char const *const &);

        ASSERT_SAME(bslmf::ForwardingType<Enum        >::Type, Enum);
        ASSERT_SAME(bslmf::ForwardingType<Enum&       >::Type, Enum&);
        ASSERT_SAME(bslmf::ForwardingType<Struct      >::Type, const Struct&);
        ASSERT_SAME(bslmf::ForwardingType<Struct&     >::Type, Struct&);
        ASSERT_SAME(bslmf::ForwardingType<Union       >::Type, const Union&);
        ASSERT_SAME(bslmf::ForwardingType<Union&      >::Type, Union&);
        ASSERT(0 == bsl::is_array<Class>::value);
        ASSERT_SAME(bslmf::ForwardingType<Class       >::Type, const Class&);
        if (verbose)
            P(bslmf::ForwardingType<Class>::BSLMF_FORWARDING_TYPE_ID);
        ASSERT_SAME(bslmf::ForwardingType<const Class&>::Type, const Class&);

        ASSERT_SAME(bslmf::ForwardingType<INT >::Type, int);
        ASSERT_SAME(bslmf::ForwardingType<INT&>::Type, int&);

        ASSERT_SAME(bslmf::ForwardingType<int Class::* >::Type, int Class::*);
        ASSERT_SAME(bslmf::ForwardingType<int Class::* const& >::Type,
                    int                       Class::* const&);

        ASSERT_SAME(bslmf::ForwardingType<int Class::*&>::Type, int Class::*&);

#if !defined(BSLS_PLATFORM_CMP_MSVC)    \
    && (!defined(BSLS_PLATFORM_CMP_IBM) \
        || (BSLS_PLATFORM_CMP_VER_MAJOR < 0x0800))
        // xlc-8 and MSVC 2005 seem to have problems with function types.

        ASSERT_SAME(bslmf::ForwardingType<  F>::Type, F&);
#endif
        ASSERT_SAME(bslmf::ForwardingType< RF>::Type, F&);

        ASSERT_SAME(bslmf::ForwardingType< PF>::Type, PF);
        ASSERT_SAME(bslmf::ForwardingType<RPF>::Type, PF&);

#if !defined(BSLS_PLATFORM_CMP_MSVC)    \
    && (!defined(BSLS_PLATFORM_CMP_IBM) \
        || (BSLS_PLATFORM_CMP_VER_MAJOR < 0x0800))
        // xlc-8 and MSVC 2005 seem to have problems with function types.

        ASSERT_SAME(bslmf::ForwardingType< Fi >::Type, Fi&);
        ASSERT_SAME(bslmf::ForwardingType< FRi>::Type, FRi&);
#endif
        ASSERT_SAME(bslmf::ForwardingType<RFi >::Type, Fi&);
        ASSERT_SAME(bslmf::ForwardingType<RFRi>::Type, FRi&);

        ASSERT_SAME(bslmf::ForwardingType< A>::Type, char*);
        ASSERT_SAME(bslmf::ForwardingType<RA>::Type, char *);

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
        ASSERT_SAME(bslmf::ForwardingType<int&&      >::Type, const int&);
        ASSERT_SAME(bslmf::ForwardingType<int const&&>::Type, int const&);
        ASSERT_SAME(bslmf::ForwardingType<void *&&         >::Type,
                    void *const &);
        ASSERT_SAME(bslmf::ForwardingType<void volatile *&&>::Type,
                    volatile void *const &);
        ASSERT_SAME(bslmf::ForwardingType<char const *const&&>::Type,
                    char const *const &);
        ASSERT_SAME(bslmf::ForwardingType<Enum&&       >::Type, const Enum&);
        ASSERT_SAME(bslmf::ForwardingType<Struct&&     >::Type, const Struct&);
        ASSERT_SAME(bslmf::ForwardingType<Union&&      >::Type, const Union&);
        ASSERT_SAME(bslmf::ForwardingType<const Class&&>::Type, const Class&);
        ASSERT_SAME(bslmf::ForwardingType<INT&&>::Type, const int&);
        ASSERT_SAME(bslmf::ForwardingType<int Class::* const&& >::Type,
                    int                       Class::* const&);
        ASSERT_SAME(bslmf::ForwardingType<int Class::*&&>::Type,
                    int Class::* const&);
        ASSERT_SAME(bslmf::ForwardingType< A&&>::Type, char*);
#endif // defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)

      } break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        cerr << "Error, non-zero test status = "
             << testStatus << "." << endl;
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
