// bslmf_forwardingtype.t.cpp                                         -*-C++-*-
#include <bslmf_forwardingtype.h>

#include <bslmf_isarray.h>
#include <bslmf_issame.h>          // for testing only

#include <bsls_platform.h>
#include <bsls_bsltestutil.h>

#include <stdlib.h>    // atoi()
#include <iostream>
#include <typeinfo>

using namespace BloombergLP;
using namespace std;

// ============================================================================
//                                TEST PLAN
// ----------------------------------------------------------------------------
//                                Overview
//                                --------
// This component provides a meta-function.  We simply verify that it returns
// the proper type for a list of suitably chosen arguments.  It also provides a
// utility function for which we verify that it returns the correct type and
// value.  Finally, we perform and end-to-end test that exercises the intended
// use of the component in order to verify that it is actually useful as
// specified.
// ----------------------------------------------------------------------------
// [ 1] bslmf::ForwardingType<TYPE>::Type
// [ 2] bslmf::ForwardingTypeUtil<TYPE>::TargetType
// [ 2] bslmf::ForwardingTypeUtil<TYPE>::forwardToTarget(v)
// [ 3] END-TO-END OVERLOADING
// [ 4] USAGE EXAMPLES
// ----------------------------------------------------------------------------

// ============================================================================
//                      STANDARD BDE ASSERT TEST MACROS
// ----------------------------------------------------------------------------

static int testStatus = 0;

static void aSsErT(int c, const char *s, int i)
{
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}
#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

#define ASSERT_SAME(X, Y) ASSERT((bsl::is_same<X, Y>::value))

// ============================================================================
//                    STANDARD BDE LOOP-ASSERT TEST MACROS
// ----------------------------------------------------------------------------

#define C_(X)   << #X << ": " << X << '\t'
#define A_(X,S) { if (!(X)) { cout S << endl; aSsErT(1, #X, __LINE__); } }
#define LOOP_ASSERT(I,X)            A_(X,C_(I))
#define LOOP2_ASSERT(I,J,X)         A_(X,C_(I)C_(J))
#define LOOP3_ASSERT(I,J,K,X)       A_(X,C_(I)C_(J)C_(K))
#define LOOP4_ASSERT(I,J,K,L,X)     A_(X,C_(I)C_(J)C_(K)C_(L))
#define LOOP5_ASSERT(I,J,K,L,M,X)   A_(X,C_(I)C_(J)C_(K)C_(L)C_(M))
#define LOOP6_ASSERT(I,J,K,L,M,N,X) A_(X,C_(I)C_(J)C_(K)C_(L)C_(M)C_(N))

//=============================================================================

#define Q   BSLS_BSLTESTUTIL_Q   // Quote identifier literally.
#define P   BSLS_BSLTESTUTIL_P   // Print identifier and value.
#define P_  BSLS_BSLTESTUTIL_P_  // P(X) without '\n'.
#define T_  BSLS_BSLTESTUTIL_T_  // Print a tab (w/o newline).
#define L_  BSLS_BSLTESTUTIL_L_  // current Line number

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

inline bool operator==(Struct a, Struct b)
    // Return true if the specified 'a' and 'b' objects have same data member.
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

inline bool operator==(Union a, Union b)
    // Return true if the specified 'a' and 'b' objects have the same data.
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

inline bool operator==(Class a, Class b)
    // Return true if the specified 'a' and 'b' have the same 'value()'.
{
    return a.value() == b.value();
}

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
    k_CONST_VOLATILE_RVALUE
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
    int operator()(const volatile TP&) const { return k_CONST_VOLATILE_LVALUE; }
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
struct CvArrayMatch {
    // Function object type that can be invoked with an array of the specified
    // 'TP' parameter type.

    template <std::size_t SZ>
    int operator()(TP (&)[SZ]) const { return int(SZ); }
        // Invoke with an array of known size.

    int operator()(TP *const&) const { return 0; }
        // Invoke with an array of unknown size.
};

template <class TP, class INVOCABLE>
int endToEndIntermediary(typename bslmf::ForwardingType<TP>::Type arg,
                         const INVOCABLE& target)
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
    // of function callss that forward to the eventual 'target' invocable
    // object.
{
    return endToEndIntermediary<TP>(arg, target);
}

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
// implement a wrapper class that holds a function pointer and logs information
// about each call to the pointed-to-function through the wrapper.  The
// pointed-to-function takes three arguments whose types a4re specified via
// template arguments.  The first argument is required to be convertible to
// 'int'.  The class definition looks as follows:
//..
    // Primary template is never defined
    template <class PROTOTYPE> class LoggingWrapper;

    template <class RET, class ARG1, class ARG2, class ARG3>
    class LoggingWrapper<RET(ARG1, ARG2, ARG3)> {
        // Specialization of wrapper for specified function prototype.

        RET (*d_function_p)(ARG1, ARG2, ARG3);

    public:
        explicit LoggingWrapper(RET (*function_p)(ARG1, ARG2, ARG3))
            // Construct wrapper around specified 'function_p' pointer.
          : d_function_p(function_p) { }

        RET operator()(ARG1, ARG2, ARG3) const;
            // Invoke the stored function pointer, logging the invocation and
            // return.
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
            // Invoke the wrapped function.
    };
//..
// Next, we define logging functions that simply count the number of
// invocations and return from invocations (e.g., to count how may invocations
// completed without exceptions):
//..
    int invocations = 0, returns = 0;
    void logInvocation(int /* ignored */) { ++invocations; }
        // Log the invocation of the wrapped function.
    void logReturn(int /* ignored */) { ++returns; }
        // Log a return from the wrapped function.
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
// to the types they were passed in as, we call the 'forwardToTarget' member of
// 'ForwardingTypeUtil':
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
// Next, in order to see this wrapper in action, we must define the function we
// wish to wrap.  This function will take an argument of type 'ArgType', which,
// among other things, keeps track of whether it has been directly constructed
// or copied from anther 'ArgType' object.  If it has been copied, it keeps
// track of how many "generations" of copy were done:
//..
    class ArgType {
        int d_value;
        int d_copies;
    public:
        explicit ArgType(int v = 0) : d_value(v), d_copies(0) { }
            // Construct. Optionally specify 'v'.

        ArgType(const ArgType& other)
            // Copy-construct from the specified 'other'.
          : d_value(other.d_value)
          , d_copies(other.d_copies + 1) { }

        int copies() const { return d_copies; }
            // Return the number of copies that this object is from the
            // original.

        int value() const { return d_value; }
            // Return the value of this object.
    };

    int myFunc(const short& i, ArgType& x, ArgType y)
        // Assign the specified 'x' the value of the specified 'y' and return
        // the 'value()' of 'x'.  Verify that the specified 'i' matches
        // 'y.copies()'.
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
        lw(2, x, y);  // Expect two copies of 'y'
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
    // called even if 'a' or 'b' is an rvalue or rvalue refearence.
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
    ASSERT((bsl::is_same<TYPE,
                    typename bsl::remove_reference<TargetType>::type>::value ||
            bsl::is_same<const TYPE,
                    typename bsl::remove_reference<TargetType>::type>::value));
#endif

    FwdType fwdObj = obj;

    // For pass-by-value, compare original and final value.
    ASSERT(obj == bslmf::ForwardingTypeUtil<TYPE>::forwardToTarget(fwdObj));
}

template <class TYPE>
void testForwardToTargetArray(TYPE obj)
    // Test 'forwardToTarget' when the specified 'obj' is an array.
{
    typedef typename bslmf::ForwardingType<TYPE>::Type FwdType;
    typedef typename bslmf::ForwardingTypeUtil<TYPE>::TargetType TargetType;

    ASSERT_SAME(TYPE&, TargetType);

    FwdType fwdObj = obj;

    // For arrays,j compare address of first element of original and final
    // arrays.
    ASSERT(&obj[0] ==
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
    ASSERT(sameAddress(ref,
                    bslmf::ForwardingTypeUtil<TYPE>::forwardToTarget(fwdRef)));
}

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;

    (void) verbose;      // eliminate unused variable warning
    (void) veryVerbose;  // eliminate unused variable warning

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:  // Zero is always the leading case.

      case 4: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLES
        //
        // Concerns: The usage examples in the component doc compile and run
        //   as promised.
        //
        // Plan: Copy the usage example from the component header, replace
        //   'assert' with 'ASSERT' and 'main' with 'usageExampleN' and verify
        //   that it compiles and runs succesfully.
        //
        // Testing:
        //   USAGE EXAMPLES
        // --------------------------------------------------------------------

        if (verbose) cout << "\nUSAGE EXAMPLES"
                          << "\n==============" << endl;

        usageExample1();
        usageExample2();

      } break;

      case 3: {
        // --------------------------------------------------------------------
        // TESTING END-TO-END OVERLOADING
        //
        // Concerns:
        //: 1 An argument of type 'TP' that is passed to one function as 'TP',
        //:   forwarded through a second function as 'ForwardingType<TP>::Type'
        //:   and passed to a third (target) function by calling
        //:   'ForwardingTypeUtil<TP>::forwardToTarget()' will be seen by the
        //:   target function almost exactly as if the original argument had
        //:   been passed directly to it, including selecting the correct
        //:   overload and instantiation of the target function.
        //: 2 Rvalue types are forwarded to the target function without
        //:   cv-qualification, regardless of whether an rvalue type is
        //:   primitive or of class type
        //: 3 Lvalue reference types are forwarded to the target with the
        //:   original cv-qualification.
        //: 4 Rvalue reference types (C++11 and newer) are forwarded to the
        //:   target with the original cv-qualification.
        //: 5 Sized array types are forwarded to the target with the original
        //:   array size.
        //: 6 Unsized array types are forwarded to the target as pointer types.
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
        //      END-TO-END OVERLOADING
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING END-TO-END OVERLOADING"
                          << "\n==============================" << endl;

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
        char (&au)[] = reinterpret_cast<AU&>(a);

// Volatile rvalue types are not useful and have strange rules.  Do not test
// them.
#define TEST_ENDTOEND_RVALUE(TP, v) {                                         \
            typedef TP T;                                                     \
            typedef const T CT;                                               \
            CvRefMatch<T> target;                                             \
            ASSERT(testEndToEnd<T>(v, target)   == target(toRvalue<T>(v)));   \
            ASSERT(testEndToEnd<CT>(v, target)  == target(toRvalue<CT>(v)));  \
        }

        if (veryVerbose) cout << "rvalue types" << endl;
        TEST_ENDTOEND_RVALUE(int      , i);
        TEST_ENDTOEND_RVALUE(Enum     , e);
        TEST_ENDTOEND_RVALUE(Struct   , s);
        TEST_ENDTOEND_RVALUE(Union    , u);
        TEST_ENDTOEND_RVALUE(Class    , c);
        TEST_ENDTOEND_RVALUE(double   , d);
        TEST_ENDTOEND_RVALUE(double * , p);
        TEST_ENDTOEND_RVALUE(F      * , f_p);
        TEST_ENDTOEND_RVALUE(Pm       , m_p);
        TEST_ENDTOEND_RVALUE(Pmf      , mf_p);

#undef TEST_ENDTOEND_RVALUE

#define TEST_ENDTOEND_LVALUE_REF(TP, v) {                                         \
            typedef TP T;                                                     \
            typedef const T CT;                                               \
            typedef volatile T VT;                                            \
            typedef const volatile T CVT;                                     \
            CT& cv = v;                                                       \
            VT& vv = v;                                                       \
            CVT& cvv = v;                                                     \
            CvRefMatch<T> target;                                             \
            ASSERT(testEndToEnd<T&>(v, target)     == target(v));             \
            ASSERT(testEndToEnd<CT&>(cv, target)   == target(cv));            \
            ASSERT(testEndToEnd<VT&>(vv, target)   == target(vv));            \
            ASSERT(testEndToEnd<CVT&>(cvv, target) == target(cvv));           \
        }

        if (veryVerbose) cout << "lvalue reference types" << endl;
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
            typedef const T CT;                                               \
            typedef volatile T VT;                                            \
            typedef const volatile T CVT;                                     \
            CvRefMatch<T> target;                                             \
            ASSERT(testEndToEnd<T&&>(std::move(v), target) ==                 \
                   target(std::move(v)));                                     \
            ASSERT(testEndToEnd<CT&&>(static_cast<CT&&>(v), target) ==        \
                   target(static_cast<CT&&>(v)));                             \
            ASSERT(testEndToEnd<VT&&>(static_cast<VT&&>(v), target) ==        \
                   target(static_cast<VT&&>(v)));                             \
            ASSERT(testEndToEnd<CVT&&>(static_cast<CVT&&>(v), target) ==      \
                   target(static_cast<CVT&&>(v)));                            \
        }

        if (veryVerbose) cout << "rvalue reference types" << endl;
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

        if (veryVerbose) cout << "array types" << endl;
        CvArrayMatch<char> am;
        ASSERT(testEndToEnd<char[5]>(a, am)    == am(a));
        ASSERT(testEndToEnd<char[]>(au, am)    == am(au));
        ASSERT(testEndToEnd<char(&)[5]>(a, am) == am(a));
        ASSERT(testEndToEnd<char(&)[]>(au, am) == am(au));
#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
        ASSERT(testEndToEnd<char *&&>(au, am)  == am(std::move(au)));
#endif

      } break;

      case 2: {
        // --------------------------------------------------------------------
        // TESTING 'bslmf::ForwardingTypeUtil'
        //
        // Concerns:
        //: 1 For types that are neither references not arrays,
        //:   'ForwardingTypeUtil<TYPE>::TargetType' is similar to
        //:   'TYPE' except that 'TargetType' might be a const reference
        //:   (C++03) or rvalue reference (C++11+). An object of 'TYPE'
        //:   converted to 'ForwardingType<TYPE>::Type', then forwarded using
        //:   'ForwardingTypeUtil<TYPE>::forwardToTarget() will yield a value
        //:   equal to the original object.
        //: 2 For array types of (known or unknown) size,
        //:   'ForwardingTypeUtil<TYPE>::TargetType' yields a reference to
        //:   'TYPE'. An array object of 'TYPE' converted to
        //:   'ForwardingType<TYPE>::Type' then forwarded using
        //:   'ForwardingTypeUtil<TYPE>::forwardToTarget() will yield a
        //:   reference to the original array.
        //: 3 For reference types, 'ForwardingTypeUtil<TYPE>::TargetType'
        //:   yields 'TYPE'.  A reference of 'TYPE' converted to
        //:   'ForwardingType<TYPE>::Type' then forwarded using
        //:   'ForwardingTypeUtil<TYPE>::forwardToTarget() will yield a
        //:   a reference identical to the original.
        //: 4 All of the above concerns apply when 'TYPE' is
        //:   cv-qualified. Note that passing volatile-qualified objects by
        //:   value or by rvalue-reference does not really happen in real code
        //:   and need not be tested.
        //
        // Plan:
        //: 1 For concern 1, implement a function template,
        //:   'testForwardToTargetVal' that can be instantiated with a 'TYPE'
        //:   and which takes an argument 'obj' of 'TYPE'.  Instantiated
        //:   on a variety of basic and non-basic types,
        //:   'testForwardToTargetVal' performs the following operations:
        //:   a Verify that 'TargetType' is the expected transformation of
        //:     'TYPE'
        //:   b Initialize a temporary variable of type
        //:     'ForwardingType<TYPE>::Type' using 'obj.
        //:   c Call 'forwardToTarget' on the temporary variable and verify
        //:     that the resulting object compares equal to 'obj'.
        //: 2 For concern 2, implement a function template,
        //    'testForwardToTargetArray' that can be instantiated with an
        //:   an array 'TYPE' (or reference-to-array 'TYPE') and which takes
        //:   an argument 'obj' of 'TYPE'.  Instantiated on a variety of array
        //:   types of known and unknown size as well a lvalue and rvalues to
        //:   such types, 'testForwardToTargetArray' performs the following
        //:   operations:
        //:   a Verify that 'TargetType' is the expected transformation of
        //:     'TYPE'
        //:   b Initialize a temporary variable of type
        //:     'ForwardingType<TYPE>::Type' using 'obj.
        //:   c Call 'forwardToTarget' on the temporary variable and verify
        //:     that the resulting object has the same address as 'obj'.
        //: 3 For concern 3, implement a function template,
        //:   'testForwardToTargetRef' that can be instantiated with a
        //:   reference 'TYPE' and which takes an argument 'ref' of 'TYPE'.
        //:   Instantiated on a variety of lvalue and rvalue reference types,
        //:   'testForwardToTargetRef' performs the following operations:
        //:   a Verify that 'TargetType' is the expected transformation of
        //:     'TYPE'
        //:   b Initialize a temporary variable of type
        //:     'ForwardingType<TYPE>::Type' using 'obj.
        //:   c Call 'forwardToTarget' on the temporary variable and verify
        //:     that the resulting object has the same address as 'obj'.
        //: 4 For concern 4, instantiate the templates defined in the previous
        //:   steps using cv-qualified template parameters.
        //
        // Testing:
        //      bslmf::ForwardingTypeUtil<TYPE>::TargetType
        //      bslmf::ForwardingTypeUtil<TYPE>::forwardToTarget(v)
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING 'bslmf::ForwardingTypeUtil'"
                          << "\n===================================" << endl;

        Enum    e = e_VAL2;
        Struct  s(99);
        Union   u(98);
        Class   c(97);
        double  d = 1.23;
        double *p = &d;
        char    a[5] = { '5', '4', '3', '2', '1' };
        char  (&au)[] = reinterpret_cast<AU&>(a);
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

        // Do note test volatile rvalues of class types. They have no real use
        // and require strange copy constructors and comparison operators to
        // test correctly.
        testForwardToTargetVal<Enum    volatile>(e);
        testForwardToTargetVal<double  volatile>(d);
        testForwardToTargetVal<double *volatile>(p);
        testForwardToTargetVal<A       volatile>(a);
        testForwardToTargetVal<AU      volatile>(au);
        testForwardToTargetVal<PF      volatile>(f_p);
        testForwardToTargetVal<Pm      volatile>(m_p);
        testForwardToTargetVal<Pmf     volatile>(mf_p);

        testForwardToTargetArray<A           >(a);
        testForwardToTargetArray<A  const    >(a);
        testForwardToTargetArray<A          &>(a);
        testForwardToTargetArray<A  const   &>(a);
        testForwardToTargetArray<A  volatile&>(a);
        testForwardToTargetArray<AU          >(au);
        testForwardToTargetArray<AU const    >(au);
        testForwardToTargetArray<AU         &>(au);
        testForwardToTargetArray<AU const   &>(au);
        testForwardToTargetArray<AU volatile&>(au);
#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
        testForwardToTargetArray<A          &&>(std::move(a));
        testForwardToTargetArray<A  const   &&>(std::move(a));
        testForwardToTargetArray<AU         &&>(std::move(au));
        testForwardToTargetArray<AU const   &&>(std::move(au));
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
        testForwardToTargetRef<Enum    &&>(std::move(e));
        testForwardToTargetRef<Struct  &&>(std::move(s));
        testForwardToTargetRef<Union   &&>(std::move(u));
        testForwardToTargetRef<Class   &&>(std::move(c));
        testForwardToTargetRef<double  &&>(std::move(d));
        testForwardToTargetRef<double *&&>(std::move(p));
        testForwardToTargetRef<PF      &&>(std::move(f_p));
        testForwardToTargetRef<Pm      &&>(std::move(m_p));
        testForwardToTargetRef<Pmf     &&>(std::move(mf_p));

        testForwardToTargetRef<Enum     const&&>(std::move(e));
        testForwardToTargetRef<Struct   const&&>(std::move(s));
        testForwardToTargetRef<Union    const&&>(std::move(u));
        testForwardToTargetRef<Class    const&&>(std::move(c));
        testForwardToTargetRef<double   const&&>(std::move(d));
        testForwardToTargetRef<double * const&&>(std::move(p));
        testForwardToTargetRef<PF       const&&>(std::move(f_p));
        testForwardToTargetRef<Pm       const&&>(std::move(m_p));
        testForwardToTargetRef<Pmf      const&&>(std::move(mf_p));

        // Do not test volatile rvalue references.  They have no real uses and
        // would require distortions in the test that could result in missing
        // actual errors.
#endif

      } break;

      case 1: {
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
        //:   'F'". The forwarding type for "pointer to function of type 'F'"
        //:   is the same pointer type, 'F*'.
        //: 4 The forwarding type for "array of cvq 'TP'" or "(lvalue or
        //:   rvalue) reference to
        //:   array of cvq 'TP'" is "cvq 'TP*'", regardless of whether
        //:   the array size is known.
        //: 5 The forwarding type for "lvalue reference to type cvq 'TP'" is
        //:   the same "lvalue reference to cvq 'TP', for non-function and
        //:   non-array types.
        //: 6 (C++11 and newer only) The forwarding type for "rvalue reference
        //:   to cvq type 'TP'" for non-function and non-array 'TP' is 'const vq
        //:   TP&', where 'vq' is 'volatile' if 'TP' is volatile-qualified and
        //:   'vq' is empty otherwise.
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
        //: 3 For concern 4, instantiate 'ForwadingType' for a small number of
        //:   array types, lvalue-reference-to-array types, and
        //:   rvalue-reference-to-array types, both sized and unsized, and
        //:   both cv-qualified and unqualified, and verify that the resulting
        //:   'Type' member is the expected pointer type.
        //: 5 For concern 5, instantiate 'ForwardingType' for *lvalue*
        //:   reference to fundamental, pointer, enumeration, class, and union
        //:   types, both cv-qualified and unqualified, and verify that the
        //:   resulting 'Type' member is the same as the parameter type.
        //: 6 For concern 6, instantiate 'ForwardingType' for *rvalue* reference
        //:   to fundamental, pointer, enumeration, class, and union
        //:   types, both cv-qualified and unqualified, and
        //:   verify that the resulting 'Type' member is the expected const
        //:   lvalue reference type.
        //
        // Testing:
        //     bslmf::ForwardingType<TYPE>::Type
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING 'bslmf::ForwardingType<TYPE>::Type'"
                          << "\n==========================================="
                          << endl;

#define TEST_FWD_TYPE(T, EXP) ASSERT_SAME(bslmf::ForwardingType<T>::Type, EXP)
#define TEST_FWD_TYPE_UNCHANGED(T) TEST_FWD_TYPE(T, T)

        if (veryVerbose) cout << "Basic types" << endl;

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

        if (veryVerbose) cout << "Class and union types" << endl;

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

        if (veryVerbose) cout << "Function types" << endl;

#if !defined(BSLS_PLATFORM_CMP_MSVC) && \
    (!defined(BSLS_PLATFORM_CMP_IBM) || (BSLS_PLATFORM_CMP_VER_MAJOR < 0x0800))
        // xlc-8 and MSVC 2005 seem to have problems with function types.  Skip
        // these tests for those compilers.
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

        if (veryVerbose) cout << "Array types" << endl;

        TEST_FWD_TYPE(int[5]                   , int*                    );
        TEST_FWD_TYPE(int*[5]                  , int**                   );
        TEST_FWD_TYPE(int[5][6]                , int(*)[6]               );
        TEST_FWD_TYPE(Class[]                  , Class*                  );
        TEST_FWD_TYPE(Struct[][6]              , Struct(*)[6]            );
        TEST_FWD_TYPE(int(&)[5]                , int*                    );
        TEST_FWD_TYPE(int *const(&)[5]         , int *const *            );
        TEST_FWD_TYPE(int(&)[5][6]             , int(*)[6]               );
        TEST_FWD_TYPE(Class(&)[]               , Class*                  );
        TEST_FWD_TYPE(Struct(&)[][6]           , Struct(*)[6]            );
        TEST_FWD_TYPE(int *const[5]            , int *const *            );
        TEST_FWD_TYPE(const int[5][6]          , const int(*)[6]         );
        TEST_FWD_TYPE(const int[]              , const int*              );
        TEST_FWD_TYPE(const int[][6]           , const int(*)[6]         );
        TEST_FWD_TYPE(const int(&)[5]          , const int*              );
        TEST_FWD_TYPE(volatile int[5]          , volatile int*           );
        TEST_FWD_TYPE(volatile int[5][6]       , volatile int(*)[6]      );
        TEST_FWD_TYPE(volatile int[]           , volatile int*           );
        TEST_FWD_TYPE(volatile int[][6]        , volatile int(*)[6]      );
        TEST_FWD_TYPE(volatile int(&)[5]       , volatile int*           );
        TEST_FWD_TYPE(const volatile int[5]    , const volatile int*     );
        TEST_FWD_TYPE(const volatile int[5][6] , const volatile int(*)[6]);
        TEST_FWD_TYPE(const volatile int[]     , const volatile int*     );
        TEST_FWD_TYPE(const volatile int[][6]  , const volatile int(*)[6]);
        TEST_FWD_TYPE(const volatile int(&)[5] , const volatile int*     );
#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
        TEST_FWD_TYPE(int *const(&&)[5]        , int *const *            );
        TEST_FWD_TYPE(int(&&)[5]               , int*                    );
        TEST_FWD_TYPE(int(&&)[5][6]            , int(*)[6]               );
        TEST_FWD_TYPE(Class(&&)[]              , Class*                  );
        TEST_FWD_TYPE(Struct(&&)[][6]          , Struct(*)[6]            );
        TEST_FWD_TYPE(const int(&&)[5]         , const int*              );
        TEST_FWD_TYPE(volatile int(&&)[5]      , volatile int*           );
        TEST_FWD_TYPE(const volatile int(&&)[5], const volatile int*     );
#endif

        if (veryVerbose) cout << "Lvalue references" << endl;

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
        if (veryVerbose) cout << "Rvalue references" << endl;

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
// Copyright (C) 2013 Bloomberg L.P.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
// ----------------------------- END-OF-FILE ----------------------------------
