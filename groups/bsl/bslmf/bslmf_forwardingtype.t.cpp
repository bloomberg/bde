// bslmf_forwardingtype.t.cpp                                         -*-C++-*-
#include <bslmf_forwardingtype.h>

#include <bslmf_addconst.h>
#include <bslmf_addcv.h>
#include <bslmf_addlvaluereference.h>
#include <bslmf_addvolatile.h>
#include <bslmf_isarray.h>
#include <bslmf_issame.h>          // for testing only

#include <bsls_bsltestutil.h>
#include <bsls_nativestd.h>
#include <bsls_platform.h>
#include <bsls_bsltestutil.h>

#include <utility>     // native_std::move for C++11

#include <stdio.h>     // atoi()
#include <stdlib.h>    // atoi()

#include <typeinfo>

using namespace BloombergLP;

#if defined(BSLS_PLATFORM_CMP_MSVC)
#pragma warning(disable: 4521) // test classes offer multiple copy constructors
#endif

// Suppress bde_verify messages about all-uppercase type names.  Test drivers
// are rife with short names like 'F' or 'PF' or 'T' or 'T1'.

// BDE_VERIFY pragma: -UC01

// ============================================================================
//                                TEST PLAN
// ----------------------------------------------------------------------------
//                                Overview
//                                --------
// This component provides a meta-function.  We simply verify that it returns
// the proper type for a list of suitably chosen arguments.  It also provides a
// utility function for which we verify that it returns the correct type and
// value.  Finally, we perform an end-to-end test that exercises the intended
// use of the component in order to verify that it is actually useful as
// specified.
// ----------------------------------------------------------------------------
// [ 2] ForwardingType<TYPE>::Type
// [ 3] ForwardingTypeUtil<TYPE>::TargetType
// [ 3] ForwardingTypeUtil<TYPE>::forwardToTarget(v)
// ----------------------------------------------------------------------------
// [ 4] END-TO-END OVERLOADING
// [ 5] USAGE EXAMPLES
// [ 1] CvRefMatch<T>::operator()()
// [ 1] CvArrayMatch<T>::operator()()
// [ 1] FuncMatch<F>::operator()()

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

#define ASSERT_SAME(X, Y) ASSERT((bsl::is_same<X, Y>::value))

#if defined(BSLS_PLATFORM_CMP_SUN)                                            \
 || defined(BSLS_PLATFORM_CMP_IBM)                                            \
 ||(defined(BSLS_PLATFORM_CMP_GNU)  && BSLS_PLATFORM_CMP_VERSION < 40300)     \
 ||(defined(BSLS_PLATFORM_CMP_MSVC) && BSLS_PLATFORM_CMP_VERSION < 1700)

# define BSLMF_FORWARDINGTYPE_NO_ARRAY_OF_UNKNOWN_BOUND
    // This macro signifies that this compiler rejects 'Type[]' as incomplete,
    // even in contexts where it should be valid, such as where it will pass by
    // reference or pointer.
#endif

#if defined(BSLS_PLATFORM_CMP_MSVC) && BSLS_PLATFORM_CMP_VERSION <= 1800
    // This was last tested with MSVC 2013, but the bug may persist in later
    // versions, not yet released.  Update the version test accordingly.

# define BSLMF_FORWARDINGTYPE_NO_ARRAY_DECAY_TO_RVALUE_REF
    // This compiler cannot bind an rvalue array, such as 'char[]', to a
    // rvalue-reference to a decayed array pointer, which would be 'char *&&'
    // in this example.
#endif

#if defined(BSLS_PLATFORM_CMP_SUN)
# define BSLMF_FOWARDINGTYPE_WORK_AROUND_SUN_ARRAY_TESTS
    // The Sun compiler has problems with any test involving arrays, triggering
    // internal compiler errors with no hint of the line(s) triggering the
    // problem.  This appears to be an artifact of several function templates
    // that are called by explicitly specifying the first template parameter to
    // be an array type.  If the function argument is also an array type, the
    // internal compiler error is triggered, while transforming that function
    // parameter type to an array reference appears to solve the problem.  As
    // the fix is entirely about making the test driver well-formed, and does
    // not impact the component header, this is deemed an acceptable workaround
    // to pass the test.
    //
    // This code has not yet been tested against the Sun CC 12.4 compiler (or
    // later) which has significantly improved standard conformance, including
    // support for most of C++11.
#endif

//=============================================================================
//                  GLOBAL TYPES/OBJECTS FOR TESTING
//-----------------------------------------------------------------------------

enum   Enum   { e_VAL1 = 1, e_VAL2 };

struct Struct {
    int d_data;

    explicit Struct(int v) : d_data(v) { }
        // Construct from the specified 'v'.

    Struct(Struct& other) : d_data(other.d_data) { }
    Struct(const Struct& other) : d_data(other.d_data) { }
    Struct(volatile Struct& other) : d_data(other.d_data) { }
    Struct(const volatile Struct& other) : d_data(other.d_data) { }
        // Copy-construct from the specified 'other'.
};

inline
bool operator==(Struct a, Struct b)
    // Return true if the specified 'a' and 'b' objects have same data member
    // value.
{
    return a.d_data == b.d_data;
}

union  Union  {
    int d_data;

    explicit Union(int v) : d_data(v) { }
        // Construct from the specified 'v'.

    Union(Union& other) : d_data(other.d_data) { }
    Union(const Union& other) : d_data(other.d_data) { }
    Union(volatile Union& other) : d_data(other.d_data) { }
    Union(const volatile Union& other) : d_data(other.d_data) { }
        // Copy-construct from the specified 'other'.
};

inline
bool operator==(Union a, Union b)
    // Return true if the specified 'a' and 'b' objects have the same data
    // member value.
{
    return a.d_data == b.d_data;
}

class  Class  {
    int d_data;
  public:
    explicit Class(int v) : d_data(v) { }
        // Construct from the specified 'v'.

    Class(Class& other) : d_data(other.d_data) { }
    Class(const Class& other) : d_data(other.d_data) { }
    Class(volatile Class& other) : d_data(other.d_data) { }
    Class(const volatile Class& other) : d_data(other.d_data) { }
        // Copy-construct from the specified 'other'.

    int value() const { return d_data; }
        // Return the value of this object.
};

inline
bool operator==(Class a, Class b)
    // Return true if the specified 'a' and 'b' have the same 'value()'.
{
    return a.value() == b.value();
}

typedef void      F ();
typedef void (*  PF)();

typedef void    Fi  (int);
typedef void    FRi (int&);

typedef char    A [5];
typedef char   AU [];

typedef int Struct::*Pm;
typedef int (Class::*Pmf)() const;

void func() { }
    // Noop function that takes no arguments and returns nothing.

void funcI(int) { }
    // Noop function that takes one by-value argument and returns nothing.

void funcRi(int&) { }
    // Noop function that takes one by-reference argument and returns nothing.

enum {
    k_LVALUE,
    k_CONST_LVALUE,
    k_VOLATILE_LVALUE,
    k_CONST_VOLATILE_LVALUE,
    k_RVALUE,
    k_CONST_RVALUE,
    k_VOLATILE_RVALUE,
    k_CONST_VOLATILE_RVALUE,
    k_FUNC_POINTER,
    k_FUNC_REFERENCE
};

template <class TP>
TP toRvalue(TP v)
    // Return a copy of the specified 'v' object.  If 'TP' has a cv-qualifier,
    // then the compiler might discard it in the return type.  For testing
    // purposes, what is important is that we get exactly what we would get
    // when returning an object of type 'TP'.
{
    return v;
}

template <class TP>
struct CvRefMatch {
    // Function object type that can be invoked with a cv-qualified reference
    // to an object of the specified 'TP' parameter type.

    int operator()(TP&) const { return k_LVALUE; }
    int operator()(const TP&) const { return k_CONST_LVALUE; }
    int operator()(volatile TP&) const { return k_VOLATILE_LVALUE; }
    int operator()(const volatile TP&) const { return k_CONST_VOLATILE_LVALUE;}
        // Invoke with an l-value reference.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
    int operator()(TP&&) const { return k_RVALUE; }
    int operator()(const TP&&) const { return k_CONST_RVALUE; }
    int operator()(volatile TP&&) const { return k_VOLATILE_RVALUE; }
    int operator()(const volatile TP&&) const
        // Invoke with an r-value reference.
        { return k_CONST_VOLATILE_RVALUE; }
#endif
};

template <class TP>
class CvArrayMatch {
    // Function object type that can be invoked with an array of the specified
    // 'TP' parameter type.

    template <size_t k_SZ>
    int match(TP (&)[k_SZ], int) const { return int(k_SZ); }
        // Matches an array of known size.

    int match(TP [], ...) const { return 0; }
        // Matches an array of unknown size.

  public:

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
    template <class ANY>
    int operator()(ANY&& a) const { return match(a, 0); }
        // Matches both lvalue and rvalue arrays.
#else
    template <class ANY>
    int operator()(ANY& a) const { return match(a, 0); }
#endif
};

template <class FUNC>
struct FuncMatch {
    // Function object type that can be invoked with a function, function
    // pointer, or function reference.

    struct MatchFuncPtr {
        // Class that is convertible from 'FUNC *'
        MatchFuncPtr(FUNC *) { }                                    // IMPLICIT
            // Convert a 'FUNC *' pointer to a 'MatchFuncPtr' object.
    };

    int operator()(FUNC&) const { return k_FUNC_REFERENCE; }
        // Match a function or a function reference.

    int operator()(MatchFuncPtr) const { return k_FUNC_POINTER; }
        // Match function pointer.  This overload requires a user-defined
        // conversion and this is less preferred than the previous one for
        // arguments that match both.
};

template <class TP, class INVOCABLE>
int endToEndIntermediary(typename bslmf::ForwardingType<TP>::Type arg,
                         const INVOCABLE&                         target)
    // Forward the specified 'arg' to the specified 'target'.  This function is
    // called in the middle of a chain of function calls that forward to the
    // eventual 'target' invocable object.
{
    return target(bslmf::ForwardingTypeUtil<TP>::forwardToTarget(arg));
}

template <class TP, class INVOCABLE>
int testEndToEnd(TP arg, const INVOCABLE& target)
    // Forward the specified 'arg' to the specified 'target' via an
    // intermediate function.  This function is called at the start of a chain
    // of function calls that forward to the eventual 'target' invocable
    // object.
{
    return endToEndIntermediary<TP>(arg, target);
}

#if defined(BSLMF_FOWARDINGTYPE_WORK_AROUND_SUN_ARRAY_TESTS)
template <class TP, class INVOCABLE>
int testEndToEndArray(typename bsl::add_lvalue_reference<TP>::type arg,
                      const INVOCABLE&                             target)
    // Forward the specified 'arg' to the specified 'target' via an
    // intermediate function.  This function is called at the start of a chain
    // of function calls that forward to the eventual 'target' invocable
    // object.
{
    return endToEndIntermediary<TP>(arg, target);
}
#endif
//=============================================================================
//                           USAGE EXAMPLES
//-----------------------------------------------------------------------------

///Usage
///-----
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
        // Usage example.
    {
        typedef int                     T1;
        typedef int&                    T2;
        typedef const volatile double&  T3;
        typedef const double &          T4;
        typedef const float * &         T5;
        typedef const float * const &   T6;
        typedef MyType                  T7;
        typedef const MyType&           T8;
        typedef MyType&                 T9;
        typedef MyType                 *T10;

        typedef int                     EXP1;
        typedef int&                    EXP2;
        typedef const volatile double&  EXP3;
        typedef const double &          EXP4;
        typedef const float * &         EXP5;
        typedef const float * const &   EXP6;
        typedef const MyType&           EXP7;
        typedef const MyType&           EXP8;
        typedef MyType&                 EXP9;
        typedef MyType                 *EXP10;

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
// Suppose the pointed-to-function takes three arguments whose types are
// specified via template arguments, where the first argument is required to
// be convertible to 'int'.
//
// First we create a wrapper class that holds a function pointer of the
// desired type:
//..
    // Primary template is never defined.
    template <class PROTOTYPE>
    class LoggingWrapper;

    template <class RET, class ARG1, class ARG2, class ARG3>
    class LoggingWrapper<RET(ARG1, ARG2, ARG3)> {
        // Specialization of wrapper for specified function prototype.

        RET (*d_function_p)(ARG1, ARG2, ARG3);

      public:
        explicit LoggingWrapper(RET (*function_p)(ARG1, ARG2, ARG3))
            // Create a 'LoggingWrapper' object for the specified 'function_p'
            // function.
          : d_function_p(function_p) { }
//..
// Then, we declare an overload of the function-call operator that actually
// invokes the wrapped function.  In order to avoid excessive copies of
// pass-by-value arguments, we use 'ForwardingType' to declare a more efficient
// intermediate argument type for our private member function:
//..
        RET operator()(typename bslmf::ForwardingType<ARG1>::Type a1,
                       typename bslmf::ForwardingType<ARG2>::Type a2,
                       typename bslmf::ForwardingType<ARG3>::Type a3) const;
            // Invoke the stored function pointer with the specified 'a1',
            // 'a2', and 'a3' arguments, logging the invocation and returning
            // the result of the function pointer invocation.
    };
//..
// Next, we define logging functions that simply count the number of
// invocations and number of returns from invocations (e.g., to count how may
// invocations completed without exiting via exceptions):
//..
    int invocations = 0, returns = 0;
    void logInvocation(int /* ignored */) { ++invocations; }
        // Log an invocation of the wrapped function.
    void logReturn(int /* ignored */) { ++returns; }
        // Log a return from the wrapped function.
//..
// Now, we implement 'operator()' call the logging functions, either side of
// calling the logged function through the wrapped pointer.  To reconstitute
// the arguments to the function as close as possible to the types they were
// passed in as, we call the 'forwardToTarget' member of 'ForwardingTypeUtil':
//..
    template <class RET, class ARG1, class ARG2, class ARG3>
    RET LoggingWrapper<RET(ARG1, ARG2, ARG3)>::operator()(
                         typename bslmf::ForwardingType<ARG1>::Type a1,
                         typename bslmf::ForwardingType<ARG2>::Type a2,
                         typename bslmf::ForwardingType<ARG3>::Type a3) const {
        logInvocation(a1);
        RET r = d_function_p(
            bslmf::ForwardingTypeUtil<ARG1>::forwardToTarget(a1),
            bslmf::ForwardingTypeUtil<ARG2>::forwardToTarget(a2),
            bslmf::ForwardingTypeUtil<ARG3>::forwardToTarget(a3));
        logReturn(a1);
        return r;
    }
//..
// Then, in order to see this wrapper in action, we must define a function we
// wish to wrap.  This function will take an argument of type 'ArgType' that
// holds an integer 'value' and keeps track of whether it has been directly
// constructed or copied from anther 'ArgType' object.  If it has been copied,
// it keeps track of how many "generations" of copy were made:
//..
    class ArgType {
        int d_value;
        int d_copies;
      public:
        explicit ArgType(int v = 0) : d_value(v), d_copies(0) { }
            // Create an 'ArgType' object.  Optionally specify 'v' as the
            // initial value of this 'ArgType' object, otherwise this object
            // will hold the value 0.

        ArgType(const ArgType& original)
            // Create an 'ArgType' object that is a copy of the specified
            // 'original'.
        : d_value(original.d_value)
        , d_copies(original.d_copies + 1)
        { }

        int copies() const { return d_copies; }
            // Return the number of copies that this object is from the
            // original.

        int value() const { return d_value; }
            // Return the value of this object.
    };

    int myFunc(const short& i, ArgType& x, ArgType y)
        // Assign the specified 'x' the value of the specified 'y' and return
        // the 'value()' of 'x'.  Verify that the specified 'i' matches
        // 'y.copies()'.  'x' is passed by reference in order to demonstrate
        // forwarding of reference arguments.
    {
        ASSERT(i == y.copies());
        x = y;
        return x.value();
    }
//..
// Finally, we create a instance of 'LoggingWrapper' to wrap 'myFunc', and we
// invoke it.  Note that 'y' is copied into the second argument of 'operator()'
// and is copied again when 'myFunc' is invoked.  However, it is *not* copied
// when 'operator()' calls 'invoke()' because the 'ForwardType' of 'ArgType' is
// 'const ArgType&', which does not create another copy.  In C++11, if
// 'ArgType' had a move constructor, then the number of copies would be only 1,
// since the final forwarding would be a move instead of a copy.
//..
    void usageExample2()
        // Usage Example
    {
        ArgType x(0);
        ArgType y(99);

        LoggingWrapper<int(const short&, ArgType&, ArgType)> lw(myFunc);
        ASSERT(0 == invocations && 0 == returns);
        lw(1, x, y);  // Expect exactly one copy of 'y'
        ASSERT(1 == invocations && 1 == returns);
        ASSERT(99 == x.value());
    }
//..

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

template <class TYPE>
bool sameAddress(const TYPE& a, const TYPE& b)
    // Return true if address of the specified 'a' object matches the address
    // of the specified 'b' object.  Unlike a direct test, this function can be
    // called even if 'a' or 'b' is an rvalue or rvalue reference.
{
    return &a == &b;
}

template <class TYPE>
bool sameAddress(TYPE& a, TYPE& b)
    // Return true if address of the specified 'a' entity (of parameterized
    // 'TYPE')) matches the address of the specified 'b' entity.  Note that
    // this second overload is necessary only for compilers like IBM xlC that
    // will not deduce a match for 'const &' for function types and references.
{
    return &a == &b;
}

template <class TYPE>
void testForwardToTargetVal(TYPE obj)
    // Test 'forwardToTarget' when the specified 'obj' is an rvalue.
{
    typedef typename bslmf::ForwardingType<TYPE>::Type FwdType;
    typedef typename bslmf::ForwardingTypeUtil<TYPE>::TargetType TargetType;

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
    ASSERT_SAME(TYPE, typename bsl::remove_reference<TargetType>::type);
#else
    ASSERTV(typeid(TYPE).name(),
           (bsl::is_same<TYPE,
                    typename bsl::remove_reference<TargetType>::type>::value ||
            bsl::is_same<const TYPE,
                    typename bsl::remove_reference<TargetType>::type>::value));
#endif

    FwdType fwdObj = obj;

    // For pass-by-value, compare original and final value.
    ASSERTV(typeid(TYPE).name(),
            obj == bslmf::ForwardingTypeUtil<TYPE>::forwardToTarget(fwdObj));
}

template <class TYPE>
#if defined(BSLMF_FOWARDINGTYPE_WORK_AROUND_SUN_ARRAY_TESTS)
void testForwardToTargetArray(
                            typename bsl::add_lvalue_reference<TYPE>::type obj)
#else
void testForwardToTargetArray(TYPE obj)
#endif
    // Test 'forwardToTarget' when the specified 'obj' is an array.
{
    typedef typename bslmf::ForwardingType<TYPE>::Type FwdType;
    typedef typename bslmf::ForwardingTypeUtil<TYPE>::TargetType TargetType;

    typedef typename bsl::add_lvalue_reference<TYPE>::type RefType;
                            // gcc 4.1.2 does not support reference collapsing.
    ASSERT_SAME(RefType, TargetType);

    FwdType fwdObj = obj;

    // For arrays, compare address of first element of original and final
    // arrays.
    ASSERTV(typeid(TYPE).name(),
            &obj[0] ==
            &bslmf::ForwardingTypeUtil<TYPE>::forwardToTarget(fwdObj)[0]);
}

template <class TYPE>
void testForwardToTargetRef(TYPE ref)
    // Test 'forwardToTarget' when the specified 'ref' is a reference type.
{
    typedef typename bslmf::ForwardingType<TYPE>::Type FwdType;
    typedef typename bslmf::ForwardingTypeUtil<TYPE>::TargetType TargetType;

    ASSERT_SAME(TYPE, TargetType);

    FwdType fwdRef = ref;

    // For pass-by-reference, compare addresses of original and final
    // references.
    ASSERTV(typeid(TYPE).name(),
            sameAddress(ref,
                    bslmf::ForwardingTypeUtil<TYPE>::forwardToTarget(fwdRef)));
}

int main(int argc, char *argv[])
{
    int  test        = argc > 1 ? atoi(argv[1]) : 0;
    bool verbose     = argc > 2;
    bool veryVerbose = argc > 3;

    (void) verbose;      // eliminate unused variable warning
    (void) veryVerbose;  // eliminate unused variable warning

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:  // Zero is always the leading case.

      case 5: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLES
        //
        // Concerns: The usage examples in the component doc compile and run
        //   as promised.
        //
        // Plan: Copy the usage example from the component header, replace
        //   'assert' with 'ASSERT' and 'main' with 'usageExampleN' and verify
        //   that it compiles and runs successfully.
        //
        // Testing:
        //   USAGE EXAMPLES
        // --------------------------------------------------------------------

        if (verbose) printf("\nUSAGE EXAMPLES"
                            "\n==============\n");

        usageExample1();
        usageExample2();

      } break;

      case 4: {
        // --------------------------------------------------------------------
        // TESTING END-TO-END OVERLOADING
        //
        // Concerns:
        //: 1 An argument of type 'TP' that is passed to one function as 'TP',
        //:   forwarded through a second function as
        //:   'ForwardingType<TP>::Type' and passed to a third (target)
        //:   function by calling 'ForwardingTypeUtil<TP>::forwardToTarget()'
        //:   will be seen by the target function as if the original argument
        //:   had been passed directly to it, including selecting the correct
        //:   overload and instantiation of the target function.
        //: 2 Rvalue types are forwarded to the target function without
        //:   cv-qualification, regardless of whether an rvalue type is
        //:   primitive or of class type.
        //: 3 Lvalue reference types are forwarded to the target with the
        //:   original cv-qualification.
        //: 4 Rvalue reference types (C++11 and newer) are forwarded to the
        //:   target with the original cv-qualification.
        //: 5 Sized array types are forwarded to the target with the original
        //:   array size.
        //: 6 array-of-unknown-bound types are forwarded to the target as
        //:   pointer types.
        //
        // Plan:
        //: 1 For concern 1, create a small set of functor classes with
        //:   several overloads of 'operator()'.  Each overload verifies
        //:   correct invocation by returning a different enumerated
        //:   value.  Call each functor via an intermediary function that
        //:   takes a 'TP' argument and passes it to a second intermediary
        //:   function taking a 'ForwardingType<TP>::Type' argument which, in
        //:   turn, calls the functor.  Verify that the functor returns the
        //:   same value as would be returned if it were called directly.
        //: 2 For concern 2, perform step 1 with a functor class whose
        //:   'operator()' takes 'const', 'volatile', and 'const
        //:   volatile' lvalue reference types (C++03 and before) or
        //:   unqualified, 'const', 'volatile', and 'const volatile' rvalue
        //:   reference types (C++11 and newer).  Instantiate with a variety
        //:   of primitive and non-primitive rvalue types of different
        //:   cv-qualifications.
        //: 3 For concern 3, repeat step 2, except instantiate with lvalue
        //:   reference types.
        //: 4 For concern 4, repeat step 2, except instantiate with rvalue
        //:   reference types.
        //: 5 For concerns 5 and 6, perform step 1 with a functor class whose
        //:   'operator()' returns the size of the array on which it is
        //:   called, or zero for pointer types.  Instantiate with a variety
        //:   of array types and reference-to-array types.
        //
        // Testing:
        //   END-TO-END OVERLOADING
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING END-TO-END OVERLOADING"
                            "\n==============================\n");

        int     i = 5;
        Enum    e = e_VAL2;
        Struct  s(99);
        Union   u(98);
        Class   c(97);
        double  d = 1.23;
        double *p = &d;
        F      *f_p = func;
        Pm      m_p  = &Struct::d_data;
        Pmf     mf_p = &Class::value;

        char a[5]    = { '5', '4', '3', '2', '1' };
#if !defined(BSLMF_FORWARDINGTYPE_NO_ARRAY_OF_UNKNOWN_BOUND)
        char (&au)[] = reinterpret_cast<AU&>(a);
#endif

// Volatile rvalue types are not useful and have strange rules.  Do not test
// them.  Attempting to test volatile rvalues will not only complicate the test
// driver unnecessarily, it may actually hide real errors.
#define TEST_ENDTOEND_RVALUE(TP, v) {                                         \
            typedef TP T;                                                     \
            typedef bsl::add_const<T>::type CT;                               \
            CvRefMatch<T> target;                                             \
            ASSERT(testEndToEnd<T>(v, target)   == target(toRvalue<T>(v)));   \
            ASSERT(testEndToEnd<CT>(v, target)  == target(toRvalue<CT>(v)));  \
        }

        if (veryVerbose) printf("rvalue types\n");
        TEST_ENDTOEND_RVALUE(int      , i);
        TEST_ENDTOEND_RVALUE(Enum     , e);
#if !defined(BSLS_PLATFORM_CMP_MSVC) || BSLS_PLATFORM_CMP_VERSION > 0x1500
        // Our test harness exposes an MSVC 2008 bug that treats the test code
        // as ambiguous.
        TEST_ENDTOEND_RVALUE(Struct   , s);
        TEST_ENDTOEND_RVALUE(Union    , u);
        TEST_ENDTOEND_RVALUE(Class    , c);
#endif
        TEST_ENDTOEND_RVALUE(double   , d);
        TEST_ENDTOEND_RVALUE(double * , p);
        TEST_ENDTOEND_RVALUE(F      * , f_p);
        TEST_ENDTOEND_RVALUE(Pm       , m_p);
        TEST_ENDTOEND_RVALUE(Pmf      , mf_p);

#undef TEST_ENDTOEND_RVALUE

#define TEST_ENDTOEND_LVALUE_REF(TP, v) {                                     \
            typedef TP T;                                                     \
            typedef bsl::add_const<T>::type CT;                               \
            typedef bsl::add_volatile<T>::type VT;                            \
            typedef bsl::add_cv<T>::type CVT;                                 \
            CT& cv = v;                                                       \
            VT& vv = v;                                                       \
            CVT& cvv = v;                                                     \
            CvRefMatch<T> target;                                             \
            ASSERT(testEndToEnd<T&>(v, target)     == target(v));             \
            ASSERT(testEndToEnd<CT&>(cv, target)   == target(cv));            \
            ASSERT(testEndToEnd<VT&>(vv, target)   == target(vv));            \
            ASSERT(testEndToEnd<CVT&>(cvv, target) == target(cvv));           \
        }

        if (veryVerbose) printf("lvalue reference types\n");
        TEST_ENDTOEND_LVALUE_REF(int      , i);
        TEST_ENDTOEND_LVALUE_REF(Enum     , e);
        TEST_ENDTOEND_LVALUE_REF(Struct   , s);
        TEST_ENDTOEND_LVALUE_REF(Union    , u);
        TEST_ENDTOEND_LVALUE_REF(Class    , c);
        TEST_ENDTOEND_LVALUE_REF(double   , d);
        TEST_ENDTOEND_LVALUE_REF(double * , p);
        TEST_ENDTOEND_LVALUE_REF(F      * , f_p);
        TEST_ENDTOEND_LVALUE_REF(Pm       , m_p);
        TEST_ENDTOEND_LVALUE_REF(Pmf      , mf_p);

#undef TEST_ENDTOEND_LVALUE_REF

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)

#define TEST_ENDTOEND_RVALUE_REF(TP, v) {                                     \
            typedef TP T;                                                     \
            typedef bsl::add_const<T>::type CT;                               \
            typedef bsl::add_volatile<T>::type VT;                            \
            typedef bsl::add_cv<T>::type CVT;                                 \
            CvRefMatch<T> target;                                             \
            ASSERT(testEndToEnd<T&&>(native_std::move(v), target) ==          \
                   target(native_std::move(v)));                              \
            ASSERT(testEndToEnd<CT&&>(static_cast<CT&&>(v), target) ==        \
                   target(static_cast<CT&&>(v)));                             \
            ASSERT(testEndToEnd<VT&&>(static_cast<VT&&>(v), target) ==        \
                   target(static_cast<VT&&>(v)));                             \
            ASSERT(testEndToEnd<CVT&&>(static_cast<CVT&&>(v), target) ==      \
                   target(static_cast<CVT&&>(v)));                            \
        }

        if (veryVerbose) printf("rvalue reference types\n");
        TEST_ENDTOEND_RVALUE_REF(int      , i);
        TEST_ENDTOEND_RVALUE_REF(Enum     , e);
        TEST_ENDTOEND_RVALUE_REF(Struct   , s);
        TEST_ENDTOEND_RVALUE_REF(Union    , u);
        TEST_ENDTOEND_RVALUE_REF(Class    , c);
        TEST_ENDTOEND_RVALUE_REF(double   , d);
        TEST_ENDTOEND_RVALUE_REF(double * , p);
        TEST_ENDTOEND_RVALUE_REF(F      * , f_p);
        TEST_ENDTOEND_RVALUE_REF(Pm       , m_p);
        TEST_ENDTOEND_RVALUE_REF(Pmf      , mf_p);

#undef TEST_ENDTOEND_RVALUE_REF

#endif // defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)

        if (veryVerbose) printf("array types\n");
#if defined(BSLMF_FOWARDINGTYPE_WORK_AROUND_SUN_ARRAY_TESTS)
#define TEST_ENDTOEND_ARRAY(TP, a, exp) {                                     \
            CvArrayMatch<char> am;                                            \
            ASSERT(testEndToEndArray<TP>(a, am)   == exp);                    \
            CvArrayMatch<const char> acm;                                     \
            ASSERT(testEndToEndArray<bsl::add_const<TP>::type>(a, acm)== exp);\
            CvArrayMatch<volatile char> avm;                                  \
            ASSERT(testEndToEndArray<bsl::add_volatile<TP>::type>(a, avm)     \
                                                                      == exp);\
            CvArrayMatch<const volatile char> acvm;                           \
            ASSERT(testEndToEndArray<bsl::add_cv<TP>::type>(a, acvm)  == exp);\
        }
#else
#define TEST_ENDTOEND_ARRAY(TP, a, exp) {                                     \
            CvArrayMatch<char> am;                                            \
            ASSERT(testEndToEnd<TP>(a, am)   == exp);                         \
            CvArrayMatch<const char> acm;                                     \
            ASSERT(testEndToEnd<bsl::add_const<TP>::type>(a, acm)  == exp);   \
            CvArrayMatch<volatile char> avm;                                  \
            ASSERT(testEndToEnd<bsl::add_volatile<TP>::type>(a, avm)  == exp);\
            CvArrayMatch<const volatile char> acvm;                           \
            ASSERT(testEndToEnd<bsl::add_cv<TP>::type>(a, acvm) == exp);      \
        }
#endif

        TEST_ENDTOEND_ARRAY(char[5], a,    5);
        TEST_ENDTOEND_ARRAY(char(&)[5], a, 5);
#if !defined(BSLMF_FORWARDINGTYPE_NO_ARRAY_OF_UNKNOWN_BOUND)
        TEST_ENDTOEND_ARRAY(char[], au,    0);
        TEST_ENDTOEND_ARRAY(char(&)[], au, 0);
#endif
#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)                  \
 &&!defined(BSLMF_FORWARDINGTYPE_NO_ARRAY_DECAY_TO_RVALUE_REF)
        TEST_ENDTOEND_ARRAY(char *&&, au,  0);
#endif

        if (veryVerbose) printf("function types\n");
        FuncMatch<F> fm;
        ASSERT(testEndToEnd<F*>(f_p,  fm) == k_FUNC_POINTER);
        ASSERT(testEndToEnd<F&>(func, fm) == k_FUNC_REFERENCE);
      } break;

      case 3: {
        // --------------------------------------------------------------------
        // TESTING 'bslmf::ForwardingTypeUtil'
        //
        // Concerns:
        //: 1 For types that are not references, arrays, or functions,
        //:   'ForwardingTypeUtil<TYPE>::TargetType' is similar to
        //:   'TYPE' except that 'TargetType' might be a const reference
        //:   (C++03) or rvalue reference (C++11+).  An object of 'TYPE'
        //:   converted to 'ForwardingType<TYPE>::Type' and then forwarded
        //:   using 'ForwardingTypeUtil<TYPE>::forwardToTarget(), will yield
        //:   a value equal to the original object.
        //: 2 For array types of (known or unknown) size,
        //:   'ForwardingTypeUtil<TYPE>::TargetType' yields a reference to
        //:   'TYPE'.  An array object of 'TYPE' converted to
        //:   'ForwardingType<TYPE>::Type' then forwarded using
        //:   'ForwardingTypeUtil<TYPE>::forwardToTarget() will yield a
        //:   reference to the original array.
        //: 3 For reference types, 'ForwardingTypeUtil<TYPE>::TargetType'
        //:   yields 'TYPE'.  A reference of 'TYPE' converted to
        //:   'ForwardingType<TYPE>::Type' then forwarded using
        //:   'ForwardingTypeUtil<TYPE>::forwardToTarget() will yield a
        //:   a reference identical to the original.
        //: 4 All of the above concerns apply when 'TYPE' is
        //:   cv-qualified.  Note that passing volatile-qualified objects by
        //:   value or by rvalue-reference does not really happen in real code
        //:   and is not supported by this component.
        //: 5 For function types, 'ForwardingTypeUtil<TYPE>::TargetType'
        //:   yields 'TYPE&'.  A function converted to
        //:   'ForwardingType<TYPE>::Type' then forwarded using
        //:   'ForwardingTypeUtil<TYPE>::forwardToTarget() will yield a a
        //:   reference to the original function.
        //
        // Plan:
        //: 1 For concern 1, implement a function template,
        //:   'testForwardToTargetVal' that can be instantiated with a 'TYPE'
        //:   and that takes an argument 'obj' of 'TYPE'.  Instantiated on a
        //:   variety of basic and non-basic types, 'testForwardToTargetVal'
        //:   performs the following operations:
        //:   a Verify that 'TargetType' is the expected transformation of
        //:     'TYPE'.
        //:   b Initialize a temporary variable of type
        //:     'ForwardingType<TYPE>::Type' using 'obj.
        //:   c Call 'forwardToTarget' on the temporary variable and verify
        //:     that the resulting object compares equal to 'obj'.
        //: 2 For concern 2, implement a function template,
        //    'testForwardToTargetArray', that can be instantiated with an
        //:   an array 'TYPE' (or reference-to-array 'TYPE') and that takes
        //:   an argument 'obj' of 'TYPE'.  Instantiated on a variety of array
        //:   types of known and unknown size as well a lvalue and rvalues to
        //:   such types, 'testForwardToTargetArray' performs the following
        //:   operations:
        //:   a Verify that 'TargetType' is the expected transformation of
        //:     'TYPE'.
        //:   b Initialize a temporary variable of type
        //:     'ForwardingType<TYPE>::Type' using 'obj.
        //:   c Call 'forwardToTarget' on the temporary variable and verify
        //:     that the resulting object has the same address as 'obj'.
        //: 3 For concern 3, implement a function template,
        //:   'testForwardToTargetRef' that can be instantiated with a
        //:   reference 'TYPE' and that takes an argument 'ref' of 'TYPE'.
        //:   Instantiated on a variety of lvalue and rvalue reference types,
        //:   'testForwardToTargetRef', performs the following operations:
        //:   a Verify that 'TargetType' is the expected transformation of
        //:     'TYPE'.
        //:   b Initialize a temporary variable of type
        //:     'ForwardingType<TYPE>::Type' using 'obj'.
        //:   c Call 'forwardToTarget' on the temporary variable and verify
        //:     that the returned reference has the same address as 'obj'.
        //: 4 For concern 4, instantiate the templates defined in the previous
        //:   steps using cv-qualified template parameters.
        //: 5 For concern 5, instantiate the template with a function
        //:   type, 'F' and then:
        //:   a Verify that 'TargetType' is 'F&'.
        //:   b Initialize a temporary variable of type
        //:     'ForwardingType<F>::Type' using 'func'.
        //:   c Call 'forwardToTarget' on the temporary variable and verify
        //:     that the returned reference has the same address as 'func'.
        //
        // Testing:
        //   ForwardingTypeUtil<TYPE>::TargetType
        //   ForwardingTypeUtil<TYPE>::forwardToTarget(v)
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING 'bslmf::ForwardingTypeUtil'"
                            "\n===================================\n");

        Enum    e = e_VAL2;
        Struct  s(99);
        Union   u(98);
        Class   c(97);
        double  d = 1.23;
        double *p = &d;
        char    a[5] = { '5', '4', '3', '2', '1' };
#if !defined(BSLMF_FORWARDINGTYPE_NO_ARRAY_OF_UNKNOWN_BOUND)
        char  (&au)[] = reinterpret_cast<AU&>(a);
#endif
        F      *f_p = func;
        Pm      m_p  = &Struct::d_data;
        Pmf     mf_p = &Class::value;

        testForwardToTargetVal<Enum    >(e);
        testForwardToTargetVal<Struct  >(s);
        testForwardToTargetVal<Union   >(u);
        testForwardToTargetVal<Class   >(c);
        testForwardToTargetVal<double  >(d);
        testForwardToTargetVal<double *>(p);
        testForwardToTargetVal<PF      >(f_p);
        testForwardToTargetVal<Pm      >(m_p);
        testForwardToTargetVal<Pmf     >(mf_p);

        testForwardToTargetVal<Enum    const>(e);
        testForwardToTargetVal<Struct  const>(s);
        testForwardToTargetVal<Union   const>(u);
        testForwardToTargetVal<Class   const>(c);
        testForwardToTargetVal<double  const>(d);
        testForwardToTargetVal<double *const>(p);
        testForwardToTargetVal<PF      const>(f_p);
        testForwardToTargetVal<Pm      const>(m_p);
        testForwardToTargetVal<Pmf     const>(mf_p);

        // Do note test volatile rvalues of class types.  They have no real use
        // and require strange copy constructors and comparison operators to
        // test correctly.
        testForwardToTargetVal<Enum    volatile>(e);
        testForwardToTargetVal<double  volatile>(d);
        testForwardToTargetVal<double *volatile>(p);
        testForwardToTargetVal<PF      volatile>(f_p);
        testForwardToTargetVal<Pm      volatile>(m_p);
        testForwardToTargetVal<Pmf     volatile>(mf_p);
#if !defined(BSLMF_FOWARDINGTYPE_WORK_AROUND_SUN_ARRAY_TESTS)
        testForwardToTargetVal<A       volatile>(a);
#if !defined(BSLMF_FORWARDINGTYPE_NO_ARRAY_OF_UNKNOWN_BOUND)
        testForwardToTargetVal<AU      volatile>(au);
#endif
#endif  // Disable testing arrays on broken compilers


        testForwardToTargetArray<A           >(a);
        testForwardToTargetArray<A  const    >(a);
        testForwardToTargetArray<A          &>(a);
        testForwardToTargetArray<A  const   &>(a);
        testForwardToTargetArray<A  volatile&>(a);
#if !defined(BSLMF_FORWARDINGTYPE_NO_ARRAY_OF_UNKNOWN_BOUND)
        testForwardToTargetArray<AU          >(au);
        testForwardToTargetArray<AU const    >(au);
        testForwardToTargetArray<AU         &>(au);
        testForwardToTargetArray<AU const   &>(au);
        testForwardToTargetArray<AU volatile&>(au);
#endif

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)   \
 &&!defined(BSLMF_FORWARDINGTYPE_NO_ARRAY_DECAY_TO_RVALUE_REF)
        testForwardToTargetArray<A          &&>(native_std::move(a));
        testForwardToTargetArray<A  const   &&>(native_std::move(a));
        testForwardToTargetArray<AU         &&>(native_std::move(au));
        testForwardToTargetArray<AU const   &&>(native_std::move(au));
#endif

        testForwardToTargetRef<Enum    &>(e);
        testForwardToTargetRef<Struct  &>(s);
        testForwardToTargetRef<Union   &>(u);
        testForwardToTargetRef<Class   &>(c);
        testForwardToTargetRef<double  &>(d);
        testForwardToTargetRef<double *&>(p);
        testForwardToTargetRef<F       &>(func);
        testForwardToTargetRef<Fi      &>(funcI);
        testForwardToTargetRef<FRi     &>(funcRi);
        testForwardToTargetRef<PF      &>(f_p);
        testForwardToTargetRef<Pm      &>(m_p);
        testForwardToTargetRef<Pmf     &>(mf_p);

        testForwardToTargetRef<Enum    const&>(e);
        testForwardToTargetRef<Struct  const&>(s);
        testForwardToTargetRef<Union   const&>(u);
        testForwardToTargetRef<Class   const&>(c);
        testForwardToTargetRef<double  const&>(d);
        testForwardToTargetRef<double *const&>(p);
        testForwardToTargetRef<PF      const&>(f_p);
        testForwardToTargetRef<Pm      const&>(m_p);
        testForwardToTargetRef<Pmf     const&>(mf_p);

        testForwardToTargetRef<Enum    volatile&>(e);
        testForwardToTargetRef<Struct  volatile&>(s);
        testForwardToTargetRef<Union   volatile&>(u);
        testForwardToTargetRef<Class   volatile&>(c);
        testForwardToTargetRef<double  volatile&>(d);
        testForwardToTargetRef<double *volatile&>(p);
        testForwardToTargetRef<PF      volatile&>(f_p);
        testForwardToTargetRef<Pm      volatile&>(m_p);
        testForwardToTargetRef<Pmf     volatile&>(mf_p);

        testForwardToTargetRef<Enum    const volatile&>(e);
        testForwardToTargetRef<Struct  const volatile&>(s);
        testForwardToTargetRef<Union   const volatile&>(u);
        testForwardToTargetRef<Class   const volatile&>(c);
        testForwardToTargetRef<double  const volatile&>(d);
        testForwardToTargetRef<double *const volatile&>(p);
        testForwardToTargetRef<PF      const volatile&>(f_p);
        testForwardToTargetRef<Pm      const volatile&>(m_p);
        testForwardToTargetRef<Pmf     const volatile&>(mf_p);

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
        testForwardToTargetRef<Struct  &&>(native_std::move(s));
        testForwardToTargetRef<Union   &&>(native_std::move(u));
        testForwardToTargetRef<Class   &&>(native_std::move(c));
#if !defined(BSLS_PLATFORM_CMP_MSVC) || BSLS_PLATFORM_CMP_VERSION > 0x1800
        // The following 6 tests fail for MS Visual C++ (tested up to VC 2013).
        // Suspect the optimizer is creating a temporary, rather than truly
        // passing by reference, when given a fundamental/primitive type.
        testForwardToTargetRef<Enum    &&>(native_std::move(e));
        testForwardToTargetRef<double  &&>(native_std::move(d));
        testForwardToTargetRef<double *&&>(native_std::move(p));
        testForwardToTargetRef<PF      &&>(native_std::move(f_p));
        testForwardToTargetRef<Pm      &&>(native_std::move(m_p));
        testForwardToTargetRef<Pmf     &&>(native_std::move(mf_p));
#endif

        testForwardToTargetRef<Enum     const&&>(native_std::move(e));
        testForwardToTargetRef<Struct   const&&>(native_std::move(s));
        testForwardToTargetRef<Union    const&&>(native_std::move(u));
        testForwardToTargetRef<Class    const&&>(native_std::move(c));
        testForwardToTargetRef<double   const&&>(native_std::move(d));
        testForwardToTargetRef<double * const&&>(native_std::move(p));
        testForwardToTargetRef<PF       const&&>(native_std::move(f_p));
        testForwardToTargetRef<Pm       const&&>(native_std::move(m_p));
        testForwardToTargetRef<Pmf      const&&>(native_std::move(mf_p));

        // Do not test volatile rvalue references.  They have no real uses and
        // would require distortions in the test that could result in missing
        // actual errors.
#endif

        // Test function type
        {
            typedef bslmf::ForwardingType<F>::Type FwdType;
            typedef bslmf::ForwardingTypeUtil<F>::TargetType TargetType;

            ASSERT_SAME(F&, TargetType);

            FwdType fwdRef = func;
            ASSERT(&func ==
                   bslmf::ForwardingTypeUtil<F>::forwardToTarget(fwdRef));
        }
      } break;

      case 2: {
        // --------------------------------------------------------------------
        // TESTING 'bslmf::ForwardingType<TYPE>::Type'
        //
        // Concerns:
        //: 1 The forwarding type for "*basic* type cvq 'TP'" is the same as
        //:   'TP' with the cv-qualification removed.
        //: 2 The forwarding type for "*class* *or* *union* type cvq 'TP'" is
        //:   'const vq TP&', where 'vq' is 'volatile' if 'TP' is
        //:   volatile-qualified and is empty otherwise.
        //: 3 The forwarding type for "function of type 'F'" or "reference to
        //:   function of type 'F'" is "reference to function of type
        //:   'F'".  The forwarding type for "pointer to function of type 'F'"
        //:   is the same pointer type, 'F*'.
        //: 4 The forwarding type for "array of cvq 'TP'" or "(lvalue or
        //:   rvalue) reference to array of cvq 'TP'" is "cvq 'TP*'",
        //:   regardless of whether the array size is known.
        //: 5 The forwarding type for "lvalue reference to type cvq 'TP'" is
        //:   the same "lvalue reference to cvq 'TP', for non-function and
        //:   non-array types.
        //: 6 (C++11 and newer only) The forwarding type for "rvalue reference
        //:   to cvq type 'TP'" for non-function and non-array 'TP' is 'const
        //:   vq TP&', where 'vq' is 'volatile' if 'TP' is volatile-qualified
        //:   and 'vq' is empty otherwise.
        //
        // Test Plan:
        //: 1 For concern 1, instantiate 'ForwardingType' for fundamental,
        //:   pointer, and enumeration types, both cv-qualified and
        //:   unqualified, and verify  that the resulting 'Type' member is
        //:   the parameter type without cv qualification.
        //: 2 For concern 2, instantiate 'ForwardingType' for class and union
        //:   type, both unqualified and cv-qualified, and verify that the
        //:   resulting 'Type' member is the expected const lvalue reference
        //:   type.
        //: 3 For concern 3, instantiate 'ForwardingType' for a small number
        //:   of function, reference-to-function, and pointer-to-function
        //:   parameters and verify that the resulting 'Type' member is the
        //:   expected type.
        //: 3 For concern 4, instantiate 'ForwardingType' for a small number of
        //:   array types, lvalue-reference-to-array types, and
        //:   rvalue-reference-to-array types, both sized and unsized, and
        //:   both cv-qualified and unqualified, and verify that the resulting
        //:   'Type' member is the expected pointer type.
        //: 5 For concern 5, instantiate 'ForwardingType' for *lvalue*
        //:   reference to fundamental, pointer, enumeration, class, and union
        //:   types, both cv-qualified and unqualified, and verify that the
        //:   resulting 'Type' member is the same as the parameter type.
        //: 6 For concern 6, instantiate 'ForwardingType' for *rvalue*
        //:   reference to fundamental, pointer, enumeration, class, and union
        //:   types, both cv-qualified and unqualified, and verify that the
        //:   resulting 'Type' member is the expected const lvalue reference
        //:   type.
        //
        // Testing:
        //   ForwardingType<TYPE>::Type
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING 'bslmf::ForwardingType<TYPE>::Type'"
                            "\n===========================================\n");

#define TEST_FWD_TYPE(T, EXP) ASSERT_SAME(bslmf::ForwardingType<T>::Type, EXP)
#define TEST_FWD_TYPE_UNCHANGED(T) TEST_FWD_TYPE(T, T)

        if (veryVerbose) printf("Basic types\n");

        TEST_FWD_TYPE(int                        , int);
        TEST_FWD_TYPE(int *                      , int *);
        TEST_FWD_TYPE(int Class::*               , int Class::*);
        TEST_FWD_TYPE(int (*)(float)             , int (*)(float));
        TEST_FWD_TYPE(int (Class::*)(char)       , int (Class::*)(char));
        TEST_FWD_TYPE(Enum                       , Enum);

        TEST_FWD_TYPE(const int                  , int);
        TEST_FWD_TYPE(int *const                 , int *);
        TEST_FWD_TYPE(const int *                , const int *);
        TEST_FWD_TYPE(int Class::* const         , int Class::*);
        TEST_FWD_TYPE(int (* const)(float)       , int (*)(float));
        TEST_FWD_TYPE(int (Class::* const)(char) , int (Class::*)(char));
        TEST_FWD_TYPE(const Enum                 , Enum);

        TEST_FWD_TYPE(volatile int               , int);
        TEST_FWD_TYPE(int *volatile              , int *);
        TEST_FWD_TYPE(volatile int *             , volatile int *);
        TEST_FWD_TYPE(int Class::* volatile      , int Class::*);
        TEST_FWD_TYPE(int (* volatile)(float)    , int (*)(float));
        TEST_FWD_TYPE(volatile Enum              , Enum);

        TEST_FWD_TYPE(const volatile int         , int);
        TEST_FWD_TYPE(int *const volatile        , int *);
        TEST_FWD_TYPE(const volatile int *       , const volatile int *);
        TEST_FWD_TYPE(int Class::* const volatile, int Class::*);
        TEST_FWD_TYPE(const volatile Enum        , Enum);

        if (veryVerbose) printf("Class and union types\n");

        TEST_FWD_TYPE(Class                 , const Class&);
        TEST_FWD_TYPE(Struct                , const Struct&);
        TEST_FWD_TYPE(Union                 , const Union&);

        TEST_FWD_TYPE(const Class           , const Class&);
        TEST_FWD_TYPE(const Struct          , const Struct&);
        TEST_FWD_TYPE(const Union           , const Union&);

        TEST_FWD_TYPE(volatile Class        , const volatile Class&);
        TEST_FWD_TYPE(volatile Struct       , const volatile Struct&);
        TEST_FWD_TYPE(volatile Union        , const volatile Union&);

        TEST_FWD_TYPE(const volatile Class  , const volatile Class&);
        TEST_FWD_TYPE(const volatile Struct , const volatile Struct&);
        TEST_FWD_TYPE(const volatile Union  , const volatile Union&);

        if (veryVerbose) printf("Function types\n");

#if (!defined(BSLS_PLATFORM_CMP_MSVC) || (BSLS_PLATFORM_CMP_VERSION < 0x1700))\
 || (!defined(BSLS_PLATFORM_CMP_IBM)  || (BSLS_PLATFORM_CMP_VERSION < 0x0800))
        // xlc-8 seems to have problems with function types.  Similarly, older
        // MS Visual C++ compilers have problem binding functions types to
        // function references, rather than decaying to function pointers.
        TEST_FWD_TYPE(void()        , void(&)());
        TEST_FWD_TYPE(int(int)      , int(&)(int));
        TEST_FWD_TYPE(void(int&)    , void(&)(int&));
#endif

        TEST_FWD_TYPE(void(&)()     , void(&)());
        TEST_FWD_TYPE(int(&)(int)   , int(&)(int));
        TEST_FWD_TYPE(void(&)(int&) , void(&)(int&));

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
        TEST_FWD_TYPE(void(&&)()    , void(&)());
        TEST_FWD_TYPE(int(&&)(int)  , int(&)(int));
        TEST_FWD_TYPE(void(&&)(int&), void(&)(int&));
#endif

        TEST_FWD_TYPE(void(*)()     , void(*)());
        TEST_FWD_TYPE(int(*)(int)   , int(*)(int));
        TEST_FWD_TYPE(void(*)(int&) , void(*)(int&));

        if (veryVerbose) printf("Array types\n");

        TEST_FWD_TYPE(int[5]                   , int*                    );
        TEST_FWD_TYPE(int*[5]                  , int**                   );
        TEST_FWD_TYPE(int[5][6]                , int(*)[6]               );
        TEST_FWD_TYPE(int(&)[5]                , int*                    );
        TEST_FWD_TYPE(int *const(&)[5]         , int *const *            );
        TEST_FWD_TYPE(int(&)[5][6]             , int(*)[6]               );
        TEST_FWD_TYPE(int *const[5]            , int *const *            );
        TEST_FWD_TYPE(const int[5][6]          , const int(*)[6]         );
        TEST_FWD_TYPE(const int(&)[5]          , const int*              );
        TEST_FWD_TYPE(volatile int[5]          , volatile int*           );
        TEST_FWD_TYPE(volatile int[5][6]       , volatile int(*)[6]      );
        TEST_FWD_TYPE(volatile int(&)[5]       , volatile int*           );
        TEST_FWD_TYPE(const volatile int[5]    , const volatile int*     );
        TEST_FWD_TYPE(const volatile int[5][6] , const volatile int(*)[6]);
        TEST_FWD_TYPE(const volatile int(&)[5] , const volatile int*     );
#if !defined(BSLMF_FORWARDINGTYPE_NO_ARRAY_OF_UNKNOWN_BOUND)
        TEST_FWD_TYPE(Class[]                  , Class*                  );
        TEST_FWD_TYPE(Struct[][6]              , Struct(*)[6]            );
        TEST_FWD_TYPE(Class(&)[]               , Class*                  );
        TEST_FWD_TYPE(Struct(&)[][6]           , Struct(*)[6]            );
        TEST_FWD_TYPE(const int[]              , const int*              );
        TEST_FWD_TYPE(const int[][6]           , const int(*)[6]         );
        TEST_FWD_TYPE(volatile int[]           , volatile int*           );
        TEST_FWD_TYPE(volatile int[][6]        , volatile int(*)[6]      );
        TEST_FWD_TYPE(const volatile int[]     , const volatile int*     );
        TEST_FWD_TYPE(const volatile int[][6]  , const volatile int(*)[6]);
#endif
#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
        TEST_FWD_TYPE(int *const(&&)[5]        , int *const *            );
        TEST_FWD_TYPE(int(&&)[5]               , int*                    );
        TEST_FWD_TYPE(int(&&)[5][6]            , int(*)[6]               );
# if !defined(BSLMF_FORWARDINGTYPE_NO_ARRAY_OF_UNKNOWN_BOUND)
        TEST_FWD_TYPE(Class(&&)[]              , Class*                  );
        TEST_FWD_TYPE(Struct(&&)[][6]          , Struct(*)[6]            );
# endif
        TEST_FWD_TYPE(const int(&&)[5]         , const int*              );
        TEST_FWD_TYPE(volatile int(&&)[5]      , volatile int*           );
        TEST_FWD_TYPE(const volatile int(&&)[5], const volatile int*     );
#endif

        if (veryVerbose) printf("Lvalue references\n");

        TEST_FWD_TYPE_UNCHANGED(int&                        );
        TEST_FWD_TYPE_UNCHANGED(int *&                      );
        TEST_FWD_TYPE_UNCHANGED(int Class::*&               );
        TEST_FWD_TYPE_UNCHANGED(int (*&)(float)             );
        TEST_FWD_TYPE_UNCHANGED(int (Class::*&)(char)       );
        TEST_FWD_TYPE_UNCHANGED(Enum&                       );
        TEST_FWD_TYPE_UNCHANGED(Class&                      );
        TEST_FWD_TYPE_UNCHANGED(Struct&                     );
        TEST_FWD_TYPE_UNCHANGED(Union&                      );

        TEST_FWD_TYPE_UNCHANGED(const int&                  );
        TEST_FWD_TYPE_UNCHANGED(int *const&                 );
        TEST_FWD_TYPE_UNCHANGED(const int *&                );
        TEST_FWD_TYPE_UNCHANGED(int Class::* const&         );
        TEST_FWD_TYPE_UNCHANGED(int (* const&)(float)       );
        TEST_FWD_TYPE_UNCHANGED(int (Class::* const&)(char) );
        TEST_FWD_TYPE_UNCHANGED(const Enum&                 );
        TEST_FWD_TYPE_UNCHANGED(const Class&                );
        TEST_FWD_TYPE_UNCHANGED(const Struct&               );
        TEST_FWD_TYPE_UNCHANGED(const Union&                );

        TEST_FWD_TYPE_UNCHANGED(volatile int&               );
        TEST_FWD_TYPE_UNCHANGED(int *volatile&              );
        TEST_FWD_TYPE_UNCHANGED(volatile int *&             );
        TEST_FWD_TYPE_UNCHANGED(int Class::* volatile&      );
        TEST_FWD_TYPE_UNCHANGED(int (* volatile&)(float)    );
        TEST_FWD_TYPE_UNCHANGED(volatile Enum&              );
        TEST_FWD_TYPE_UNCHANGED(volatile Class&             );
        TEST_FWD_TYPE_UNCHANGED(volatile Struct&            );
        TEST_FWD_TYPE_UNCHANGED(volatile Union&             );

        TEST_FWD_TYPE_UNCHANGED(const volatile int&         );
        TEST_FWD_TYPE_UNCHANGED(int *const volatile&        );
        TEST_FWD_TYPE_UNCHANGED(const volatile int *&       );
        TEST_FWD_TYPE_UNCHANGED(int Class::* const volatile&);
        TEST_FWD_TYPE_UNCHANGED(const volatile Enum&        );
        TEST_FWD_TYPE_UNCHANGED(const volatile Class&       );
        TEST_FWD_TYPE_UNCHANGED(const volatile Struct&      );
        TEST_FWD_TYPE_UNCHANGED(const volatile Union&       );

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
        if (veryVerbose) printf("Rvalue references\n");

        TEST_FWD_TYPE(int&&                  , const int&);
        TEST_FWD_TYPE(int *&&                , int *const &);
        TEST_FWD_TYPE(const int *&&          , const int *const &);
        TEST_FWD_TYPE(volatile int *&&       , volatile int *const &);
        TEST_FWD_TYPE(const volatile int *&& , const volatile int *const &);
        TEST_FWD_TYPE(int Class::*&&         , int Class::* const&);
        TEST_FWD_TYPE(int (*&&)(float)       , int (*const &)(float));
        TEST_FWD_TYPE(int (Class::*&&)(char) , int (Class::*const &)(char));
        TEST_FWD_TYPE(Enum&&                 , const Enum&);
        TEST_FWD_TYPE(Class&&                , const Class&);
        TEST_FWD_TYPE(Struct&&               , const Struct&);
        TEST_FWD_TYPE(Union&&                , const Union&);

        TEST_FWD_TYPE(const int&&            , const int&);
        TEST_FWD_TYPE(int *const&&           , int *const &);
        TEST_FWD_TYPE(const int *const &&    , const int *const &);
        TEST_FWD_TYPE(int Class::* const&&   , int Class::* const &);
        TEST_FWD_TYPE(int (* const&&)(float) , int (* const&)(float));
        TEST_FWD_TYPE(int (Class::* const&&)(char)
                                             , int (Class::* const&)(char));
        TEST_FWD_TYPE(const Enum&&           , const Enum&);
        TEST_FWD_TYPE(const Class&&          , const Class&);
        TEST_FWD_TYPE(const Struct&&         , const Struct&);
        TEST_FWD_TYPE(const Union&&          , const Union&);

        TEST_FWD_TYPE(volatile int&&         , const volatile int&);
        TEST_FWD_TYPE(int *volatile&&        , int *const volatile&);
        TEST_FWD_TYPE(int Class::* volatile&&, int Class::* const volatile&);
        TEST_FWD_TYPE(int (* volatile&&)(float)
                                             , int (* const volatile&)(float));
        TEST_FWD_TYPE(volatile Enum&&        , const volatile Enum&);
        TEST_FWD_TYPE(volatile Class&&       , const volatile Class&);
        TEST_FWD_TYPE(volatile Struct&&      , const volatile Struct&);
        TEST_FWD_TYPE(volatile Union&&       , const volatile Union&);

        TEST_FWD_TYPE(const volatile int&&   , const volatile int&);
        TEST_FWD_TYPE(int *const volatile&&  , int *const volatile&);
        TEST_FWD_TYPE(int Class::* const volatile&&
                                             , int Class::* const volatile&);
        TEST_FWD_TYPE(const volatile Enum&&  , const volatile Enum&);
        TEST_FWD_TYPE(const volatile Class&& , const volatile Class&);
        TEST_FWD_TYPE(const volatile Struct&&, const volatile Struct&);
        TEST_FWD_TYPE(const volatile Union&& , const volatile Union&);

#endif // defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)

      } break;

      case 1: {
        // --------------------------------------------------------------------
        // TESTING TEST INFRASTRUCTURE
        //
        // Concerns:
        //: 1 For 'crm' of type 'CvRefMatch<T>', 'crm(v)' returns the correct
        //:   enumerator indicating whether 'v' is an lvalue, const lvalue,
        //:   volatile lvalue, const volatile lvalue, rvalue, const rvalue,
        //:   volatile rvalue, or const volatile rvalue.
        //: 2 For 'cam' of type 'CvArrayMatch<T>' and 'a' of type array-of-T,
        //:   'cam(a)' returns the number of elements of 'a' or '0' if 'a' has
        //:   unknown bounds.
        //: 3 For 'cam' of type 'CvArrayMatch<T>' and 'p' of type
        //:   pointer-to-T, 'cam(p)' returns 0.
        //: 4 Concerns 2 and 3 apply regardless of the cv qualification of 'a'
        //:   or 'p'.
        //: 5 For 'cam' of type 'CvArrayMatch<T>' and 'a' of type
        //:   rvalue-reference-to-array-of-T, 'CvArrayMatch<T>' returns the
        //:   same value as if 'a' were not an rvalue reference.
        //: 6 For 'fm' of type 'FuncMatch<F>', 'fm(f)' returns
        //:   'k_FUNC_REFERENCE' if 'f' is of type 'F' or 'F&' and
        //:   'k_FUNC_POINTER' if 'f' is of type 'F*'.
        //
        // Plan:
        //: 1 For concern 1, create a variable, 'crm' of type
        //:   'CvRefMatch<int>'.  Call 'crm(v)' for variables 'v' of type cvq
        //:   'int' correct lvalue enumeration value is returned.  In C++11 or
        //:   later mode, test with 'std::move(v)' and verify that the correct
        //:   lvalue enumeration is returned.  Also test with a literal integer
        //:   and verify that the return value is 'k_RVALUE_REF' in C++11 mode
        //:   or 'k_CONST_LVALUE_REF' in C++03 mode.
        //: 2 For concerns 2 and 3, create a variable, 'cam' of type
        //:   'CvArrayMatch<int>'.  Define variables of type array-of-int,
        //:   with both known and unknown sizes.  Verify that 'cam(v)' returns
        //:   the correct size (or 0) for each array variable, 'v'.
        //: 3 For concern 4, repeat step 2 for variables of each cvq
        //:   combination.
        //: 4 For concern 5 (C++11 or later), repeat steps 2 and 3 using
        //:   'cam(std::move(v))'.
        //: 6 For concern 6, create a variable, 'fm', of type
        //:   'FuncMatch<F>'.  Call 'fm(f)' for arguments 'f' of type 'void()',
        //:   'void(&)()', and 'void(*)()', and verify that the correct
        //:   enumeration value is returned.
        //
        // Testing:
        //   CvRefMatch<T>::operator()()
        //   CvArrayMatch<T>::operator()()
        //   FuncMatch<F>::operator()()
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING TEST INFRASTRUCTURE"
                            "\n===========================\n");

        if (veryVerbose) printf("CvRefMatch\n");

        int i                  = 0;
        const int ci           = 1;
        volatile int vi        = 2;
        const volatile int cvi = 3;
        volatile int &rvi      = vi;

        CvRefMatch<int> crm;
        ASSERT(k_LVALUE                  == crm(  i));
        ASSERT(k_CONST_LVALUE            == crm( ci));
        ASSERT(k_VOLATILE_LVALUE         == crm( vi));
        ASSERT(k_CONST_VOLATILE_LVALUE   == crm(cvi));
#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
        ASSERT(k_RVALUE                  == crm(native_std::move(  i)));
        ASSERT(k_CONST_RVALUE            == crm(native_std::move( ci)));
        ASSERT(k_VOLATILE_RVALUE         == crm(native_std::move( vi)));
        ASSERT(k_CONST_VOLATILE_RVALUE   == crm(native_std::move(cvi)));
#endif

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
        ASSERT(k_RVALUE                  == crm(5));
        ASSERT(k_RVALUE                  == crm(toRvalue(rvi)));
#else
        ASSERT(k_CONST_LVALUE            == crm(5));
        ASSERT(k_CONST_LVALUE            == crm(toRvalue(rvi)));
#endif

        if (veryVerbose) printf("CvArrayMatch\n");

        int                  a[1]   = { 1 };
        const int            ca[2]  = { 1, 2 };
        volatile int         va[3]  = { 1, 2, 3 };
        const volatile int   cva[4] = { 1, 2, 3, 4};
        int                (*pa)[]   = reinterpret_cast<int (*)[]>(a);
        const int          (*cpa)[]  = pa;
        volatile int       (*vpa)[]  = pa;
        const volatile int (*cvpa)[] = pa;
        int                 *p;
        const int           *cp;
        volatile int        *vp;
        const volatile int  *cvp;

        CvArrayMatch<int>                cam;
        CvArrayMatch<const int>          camc;
        CvArrayMatch<volatile int>       camv;
        CvArrayMatch<const volatile int> camcv;
        ASSERT(1 == cam  (  a));
        ASSERT(2 == camc ( ca));
        ASSERT(3 == camv ( va));
        ASSERT(4 == camcv(cva));
#if !defined(BSLMF_FORWARDINGTYPE_NO_ARRAY_OF_UNKNOWN_BOUND)
        ASSERT(0 == cam  (*  pa));
        ASSERT(0 == camc (* cpa));
        ASSERT(0 == camv (* vpa));
        ASSERT(0 == camcv(*cvpa));
#endif

        ASSERT(0 == cam  (  p));
        ASSERT(0 == camc ( cp));
        ASSERT(0 == camv ( vp));
        ASSERT(0 == camcv(cvp));

        if (veryVerbose) printf("FuncMatch\n");
        FuncMatch<F> fm;
        F& func_r = func;
        F* func_p = func;
        ASSERT(k_FUNC_REFERENCE == fm(func));
        ASSERT(k_FUNC_REFERENCE == fm(func_r));
        ASSERT(k_FUNC_POINTER   == fm(&func));
        ASSERT(k_FUNC_POINTER   == fm(func_p));
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
// Copyright 2015 Bloomberg Finance L.P.
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
