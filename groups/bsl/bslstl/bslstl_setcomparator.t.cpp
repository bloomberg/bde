// bslstl_setcomparator.t.cpp                                         -*-C++-*-
#include <bslstl_setcomparator.h>

#include <bslstl_treenode.h>

#include <bslalg_rbtreeanchor.h>
#include <bslalg_rbtreenode.h>
#include <bslalg_rbtreeutil.h>

#include <bsltf_templatetestfacility.h>

#include <bslma_allocator.h>
#include <bslma_allocatortraits.h>
#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_stdallocator.h>
#include <bslma_testallocator.h>
#include <bslma_testallocatormonitor.h>

#include <bsls_assert.h>
#include <bsls_bsltestutil.h>
#include <bsls_util.h>

#include <algorithm>
#include <functional>
#include <limits.h>
#include <stdio.h>
#include <typeinfo>

//=============================================================================
//                              TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// The component under test implements a mechanism.  The component doesn't
// define any state explicitly, but may inherit state from its parent class,
// which depends on the parameterized 'COMPARATOR' type.  The component doesn't
// define any primary manipulators, but it's parent state may be set on
// construction.
//
// Basic Accessors:
//: o 'keyComparator'
//
// Global Concerns:
//: o All accessor methods are declared 'const'.
//: o Pointer/reference parameters are declared 'const'.
//: o No memory is ever allocated.
//-----------------------------------------------------------------------------
// CREATORS
// [ 3] SetComparator();
// [ 3] explicit SetComparator(const COMPARATOR& keyComparator);
//
// MANIPULATORS
// [ 4] void swap(SetComparator& other);
//
// ACCESSORS
// [ 3] bool operator()(const KEY& lhs, const bslalg::RbTreeNode& rhs) const;
// [ 3] bool operator()(const bslalg::RbTreeNode& rhs, const KEY& lhs) const;
// [ 2] COMPARATOR keyComparator() const;
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 5] USAGE EXAMPLE
// [ *] CONCERN: All accessor methods are declared 'const'.
// [ *] CONCERN: Pointer/reference parameters are declared 'const'.
// [ *] CONCERN: In no case does memory come from the global allocator.
// [ 3] CONCERN: No memory is ever allocated from this class.
//-----------------------------------------------------------------------------
//=============================================================================

// ============================================================================
//                          ADL SWAP TEST HELPER
// ----------------------------------------------------------------------------

template <class TYPE>
void invokeAdlSwap(TYPE& a, TYPE& b)
    // Exchange the values of the specified 'a' and 'b' objects using the
    // 'swap' method found by ADL (Argument Dependent Lookup).  The behavior
    // is undefined unless 'a' and 'b' were created with the same allocator.
{
    using namespace bsl;
    swap(a, b);
}

// The following 'using' directives must come *after* the definition of
// 'invokeAdlSwap' (above).

using namespace BloombergLP;
using namespace std;
using namespace bslstl;

//=============================================================================
//                  STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
// NOTE: THIS IS A LOW-LEVEL COMPONENT AND MAY NOT USE ANY C++ LIBRARY
// FUNCTIONS, INCLUDING IOSTREAMS.

namespace {

int testStatus = 0;

void aSsErT(bool b, const char *s, int i)
{
    if (b) {
        printf("Error " __FILE__ "(%d): %s    (failed)\n", i, s);
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}

}  // close unnamed namespace

//=============================================================================
//                       STANDARD BDE TEST DRIVER MACROS
//-----------------------------------------------------------------------------

#define ASSERT       BSLS_BSLTESTUTIL_ASSERT
#define LOOP_ASSERT  BSLS_BSLTESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLS_BSLTESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLS_BSLTESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLS_BSLTESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLS_BSLTESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLS_BSLTESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLS_BSLTESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLS_BSLTESTUTIL_LOOP6_ASSERT
#define ASSERTV      BSLS_BSLTESTUTIL_ASSERTV

#define Q   BSLS_BSLTESTUTIL_Q   // Quote identifier literally.
#define P   BSLS_BSLTESTUTIL_P   // Print identifier and value.
#define P_  BSLS_BSLTESTUTIL_P_  // P(X) without '\n'.
#define T_  BSLS_BSLTESTUTIL_T_  // Print a tab (w/o newline).
#define L_  BSLS_BSLTESTUTIL_L_  // current Line number

#define RUN_EACH_TYPE BSLTF_TEMPLATETESTFACILITY_RUN_EACH_TYPE

// ============================================================================
//                  NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)
#define ASSERT_PASS(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
#define ASSERT_FAIL(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)
#define ASSERT_OPT_PASS(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS(EXPR)
#define ASSERT_OPT_FAIL(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL(EXPR)

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

static int verbose, veryVerbose, veryVeryVerbose, veryVeryVeryVerbose;
static int numFunctionComp;

// ============================================================================
//                               TEST APPARATUS
// ----------------------------------------------------------------------------
namespace {

                       // =====================
                       // class LessThanFunctor
                       // =====================

template <class TYPE>
class LessThanFunctor {
    // This test class provides a mechanism that defines a const function-call
    // operator that compares two objects of the parameterized 'TYPE'.  The
    // function-call operator is implemented with integer comparison using
    // integers converted from objects of 'TYPE' by the class method
    // 'bsltf::TemplateTestFacility::getIdentifier'.  As a side effect, the
    // function-call operator also increments a counter 'd_numCalls' used to
    // keep track the method call count.  Object of this class can be
    // identified by an id passed on construction.

    // DATA
    int d_id;
    mutable int d_numCalls;

  public:

    // CREATORS
    LessThanFunctor()
    : d_id(0), d_numCalls(0)
    {
    }

    explicit LessThanFunctor(int id)
    : d_id(id), d_numCalls(0)
    {
    }

    // ACCESSORS
    bool operator() (const TYPE& lhs, const TYPE& rhs) const
    {
        ++d_numCalls;
        return bsltf::TemplateTestFacility::getIdentifier(lhs)
             < bsltf::TemplateTestFacility::getIdentifier(rhs);
    }

    int numCalls() const
    {
        return d_numCalls;
    }

    int id() const
    {
        return d_id;
    }
};

// FREE OPERATORS
template <class TYPE>
bool operator== (const LessThanFunctor<TYPE>& lhs,
                 const LessThanFunctor<TYPE>& rhs)
{
    return lhs.id() == rhs.id();
}

template <class TYPE>
bool operator!= (const LessThanFunctor<TYPE>& lhs,
                 const LessThanFunctor<TYPE>& rhs)
{
    return lhs.id() != rhs.id();
}

                       // ============================
                       // class LessThanFunctorMovable
                       // ============================

template <class TYPE>
class LessThanFunctorMovable {
    // This test class provides a mechanism that defines a const function-call
    // operator that compares two objects of the parameterized 'TYPE' and has
    // a move constructor.  The function-call operator is implemented with
    // integer comparison using integers converted from objects of 'TYPE' by
    // the class method 'bsltf::TemplateTestFacility::getIdentifier'.  As a
    // side effect, the function-call operator also increments a counter
    // 'd_numCalls' used to keep track the method call count.  Object of this
    // class can be identified by an id passed on construction.

    // DATA
    int d_id;
    mutable int d_numCalls;

  public:

    // CREATORS
    LessThanFunctorMovable()
    : d_id(0), d_numCalls(0)
    {
    }

    explicit LessThanFunctorMovable(int id)
    : d_id(id), d_numCalls(0)
    {
    }

    explicit LessThanFunctorMovable(
                            bslmf::MovableRef<LessThanFunctorMovable> original)
    : d_id(bslmf::MovableRefUtil::access(original).d_id)
    , d_numCalls(bslmf::MovableRefUtil::access(original).d_numCalls)
    {
        LessThanFunctorMovable& lvalue = original;
        lvalue.d_id = 0;
        lvalue.d_numCalls = 0;
    }

    // ACCESSORS
    bool operator() (const TYPE& lhs, const TYPE& rhs) const
    {
        ++d_numCalls;
        return bsltf::TemplateTestFacility::getIdentifier(lhs)
             < bsltf::TemplateTestFacility::getIdentifier(rhs);
    }

    int numCalls() const
    {
        return d_numCalls;
    }

    int id() const
    {
        return d_id;
    }
};

                       // =============================
                       // class LessThanFunctorNonConst
                       // =============================

template <class TYPE>
class LessThanFunctorNonConst {
    // This test class provides a mechanism that defines a non-'const'
    // function-call operator that compares two objects of the parameterized
    // 'TYPE'.  The function-call operator is implemented with integer
    // comparison using integers converted from objects of 'TYPE' by the class
    // method 'bsltf::TemplateTestFacility::getIdentifier'.  As a side effect,
    // the function-call operator also increments a counter 'd_numCalls' used
    // to keep track the method call count.  Object of this class can be
    // identified by an id passed on construction.


    // DATA
    int d_id;
    mutable int d_numCalls;

  public:
    // CREATORS
    LessThanFunctorNonConst()
    : d_id(0), d_numCalls(0)
    {
    }

    explicit LessThanFunctorNonConst(int id)
    : d_id(id), d_numCalls(0)
    {
    }

    // ACCESSORS
    bool operator() (const TYPE& lhs, const TYPE& rhs)
    {
        ++d_numCalls;
        return bsltf::TemplateTestFacility::getIdentifier(lhs)
             < bsltf::TemplateTestFacility::getIdentifier(rhs);
    }

    int numCalls() const
    {
        return d_numCalls;
    }

    int id() const
    {
        return d_id;
    }
};

template <class TYPE>
bool lessThanFunction(const TYPE& lhs, const TYPE& rhs)
{
    ++numFunctionComp;
    return bsltf::TemplateTestFacility::getIdentifier(lhs)
         < bsltf::TemplateTestFacility::getIdentifier(rhs);
}

template <class TYPE>
bool lessThanFunction2(const TYPE& /* lhs */, const TYPE& /* rhs */)
{
    return true;
}

template <class TYPE>
struct TestDriver {
    // TYPES
    typedef TYPE           Key;
    typedef TreeNode<Key>  Node;

    typedef bsl::allocator<Node>          Alloc;
    typedef bsl::allocator_traits<Alloc>  AllocTraits;

    typedef LessThanFunctor<Key>         FunctorType;
    typedef LessThanFunctorMovable<Key>  MovableFunctorType;
    typedef LessThanFunctorNonConst<Key> NonConstFunctorType;
    typedef bool (*FunctionType) (const TYPE&, const TYPE&);

    static void test4();
    static void test3();
    static void test2();
    static void test1();
};

template <class TYPE>
void TestDriver<TYPE>::test4()
{
    // --------------------------------------------------------------------
    // SWAP METHOD
    // Concerns:
    //: 1 Invoking the 'swap' method or free function on an object with functor
    //:   'COMPARATOR' passing another object exchanges their 'COMPARATOR'
    //:   objects.
    //:
    //: 2 Invoking the 'swap' method or free function on an object with
    //:   function pointer 'COMPARATOR' passing another object exchanges their
    //:   'COMPARATOR' function pointers.
    //:
    //: 3 The free 'swap' function is discoverable through ADL (Argument
    //:   Dependent Lookup).
    //
    // Plan:
    //: 1 Construct two objects using two different functors.  Verify the
    //:   functors returned by 'keyComparator' compare equal to the functors
    //:   used on construction'
    //:
    //: 2 Invoke the 'swap' method on one of the objects passing in the other.
    //:   Verify the functor returned by calling 'keyComparator' on each
    //:   object compare equal to the functor used on construction for the
    //:   other object.  (C-1)
    //:
    //: 3 Construct two objects using two different function pointer.  Verify
    //:   the function pointers returned by 'keyComparator' point to the same
    //:   address as the function pointer used on construction.
    //:
    //: 4 Invoke the 'swap' method on one of the objects passing in the other.
    //:   Verify the function pointer returned by calling 'keyComparator' on
    //:   each object compare equal to the function pointer used on
    //:   construction for the other object.  (C-2)
    //:
    //: 5 Repeat P-1..4, except this time use the 'invokeAdlSwap' helper
    //:   function template the swap the objects.  (C-1..3)
    //
    // Testing:
    //   void swap(SetComparator& other);
    // --------------------------------------------------------------------

    if (verbose)
        printf("\n==TYPE: %s==\n", typeid(TYPE).name());

    if (verbose)
        printf("\n\tTesting 'swap' method"
               "for objects constructed with a functor\n");
    {
        FunctorType fA(1);
        SetComparator<Key, FunctorType> mA(fA);
        const SetComparator<Key, FunctorType>& A = mA;

        FunctorType fB(2);
        SetComparator<Key, FunctorType> mB(fB);
        const SetComparator<Key, FunctorType>& B = mB;

        ASSERT(fA != fB);
        ASSERT(A.keyComparator() == fA);
        ASSERT(B.keyComparator() == fB);

        mA.swap(mB);
        ASSERT(A.keyComparator() == fB);
        ASSERT(B.keyComparator() == fA);
    }

    if (verbose)
        printf("\n\tTesting 'swap' method"
               "for objects constructed with a function pointer\n");
    {
        FunctionType fA = &lessThanFunction<TYPE>;
        SetComparator<Key, FunctionType> mA(fA);
        const SetComparator<Key, FunctionType>& A = mA;

        FunctionType fB = &lessThanFunction2<TYPE>;
        SetComparator<Key, FunctionType> mB(fB);
        const SetComparator<Key, FunctionType>& B = mB;

        ASSERT(fA != fB);
        ASSERT(A.keyComparator() == fA);
        ASSERT(B.keyComparator() == fB);

        mA.swap(mB);
        ASSERT(A.keyComparator() == fB);
        ASSERT(B.keyComparator() == fA);
    }

    if (verbose)
        printf("\n\tTesting 'swap' free function"
               "for objects constructed with a functor\n");
    {
        FunctorType fA(1);
        SetComparator<Key, FunctorType> mA(fA);
        const SetComparator<Key, FunctorType>& A = mA;

        FunctorType fB(2);
        SetComparator<Key, FunctorType> mB(fB);
        const SetComparator<Key, FunctorType>& B = mB;

        ASSERT(fA != fB);
        ASSERT(A.keyComparator() == fA);
        ASSERT(B.keyComparator() == fB);

        invokeAdlSwap(mA, mB);
        ASSERT(A.keyComparator() == fB);
        ASSERT(B.keyComparator() == fA);
    }

    if (verbose)
        printf("\n\tTesting 'swap' free function"
               "for objects constructed with a function pointer\n");
    {
        FunctionType fA = &lessThanFunction<TYPE>;
        SetComparator<Key, FunctionType> mA(fA);
        const SetComparator<Key, FunctionType>& A = mA;

        FunctionType fB = &lessThanFunction2<TYPE>;
        SetComparator<Key, FunctionType> mB(fB);
        const SetComparator<Key, FunctionType>& B = mB;

        ASSERT(fA != fB);
        ASSERT(A.keyComparator() == fA);
        ASSERT(B.keyComparator() == fB);

        invokeAdlSwap(mA, mB);
        ASSERT(A.keyComparator() == fB);
        ASSERT(B.keyComparator() == fA);
    }
}

template <class TYPE>
void TestDriver<TYPE>::test3()
{
    // --------------------------------------------------------------------
    // FUNCTION OPERATORS
    // Concerns:
    //: 1 An object supports comparing a const 'Key' object with a const
    //:   'TreeNode' object and a const 'TreeNode' object with a const 'Key'
    //:   object for any 'COMPARATOR' type.
    //:
    //: 2 An object delegates its comparison operations to a default
    //:   constructed object of the parameterized 'COMPARATOR' type if
    //:   'COMPARATOR' is a functor type.
    //:
    //: 3 An object delegates its comparison operations to a copy of a functor
    //:   passed on construction.
    //:
    //: 4 An objects delegates its comparison operations to a function referred
    //:   to by a function pointer passed on construction.
    //:
    //: 5 A modifiable reference to an object support delegating to a functor
    //:   'COMPARATOR' providing a non-'const' function call operator.
    //
    // Plan:
    //: 1 Construct two objects using the default constructor with (1) a
    //:   functor type providing a const function call operator and (2) a
    //:   functor type providing a non-'const' function call operator.  Verify
    //:   the operations of calling the 'operator()' on non-modifiable
    //:   references to the first object and a modifiable reference to the
    //:   second object.  (C-1..2, 5)
    //:
    //: 2 Construct two object using the value constructor passing in (1) a
    //:   functor providing a const function call operator and (2) a functor
    //:   providing a non-'const' function call operator.  Verify that invoking
    //:   the 'operator()' on a non-modifiable reference to the first object
    //:   and a modifiable reference to the second object delegate their
    //:   operations to a copy of the functor passed on construction.
    //:   (C-1, 3, 5)
    //:
    //: 3 Construct an object using a function pointer.  Verify 'operator()'
    //:   delegates its operations to the function pointed to by that function
    //:   pointer.  (C-1, 4)
    //
    // Testing:
    //   SetComparator();
    //   explicit SetComparator(const COMPARATOR& keyComparator);
    //   bool operator()(const KEY& lhs, const bslalg::RbTreeNode& rhs) const;
    //   bool operator()(const bslalg::RbTreeNode& rhs, const KEY& lhs) const;
    // --------------------------------------------------------------------

    if (verbose)
        printf("\n==TYPE: %s==\n", typeid(TYPE).name());

    bslma::TestAllocator da("default");
    bslma::TestAllocator oa("object");

    bslma::DefaultAllocatorGuard dag(&da);

    if (verbose)
        printf("\n\tTesting 'operator()' "
               "for default constructed object\n");
    {
        SetComparator<Key, FunctorType> mComp;
        const SetComparator<Key, FunctorType>& comp = mComp;

        SetComparator<Key, NonConstFunctorType> ncComp;

        Key mK0 = bsltf::TemplateTestFacility::create<Key>(0);
        const Key& K0 = mK0;

        Alloc allocator(&oa);
        Node *n1Ptr = AllocTraits::allocate(allocator, 1);

        Node& mN1 = *n1Ptr; const Node& N1 = mN1;
        AllocTraits::construct(allocator, bsls::Util::addressOf(mN1.value()));

        mN1.value() = bsltf::TemplateTestFacility::create<Key>(1);

        ASSERTV(N1.value(), K0, comp(K0, N1));
        ASSERTV(comp.keyComparator().numCalls(),
                1 == comp.keyComparator().numCalls());
        ASSERTV(K0, N1.value(), !comp(N1, K0));
        ASSERTV(comp.keyComparator().numCalls(),
                2 == comp.keyComparator().numCalls());

        ASSERTV(N1.value(), K0, ncComp(K0, N1));
        ASSERTV(ncComp.keyComparator().numCalls(),
                1 == ncComp.keyComparator().numCalls());
        ASSERTV(K0, N1.value(), !ncComp(N1, K0));
        ASSERTV(ncComp.keyComparator().numCalls(),
                2 == ncComp.keyComparator().numCalls());

        AllocTraits::destroy(allocator, bsls::Util::addressOf(mN1.value()));
        AllocTraits::deallocate(allocator, n1Ptr, 1);
    }

    if (verbose)
        printf("\n\tTesting 'operator()' "
               "for objects constructed with a functor\n");
    {

        Key mK0 = bsltf::TemplateTestFacility::create<Key>(0);
        const Key& K0 = mK0;

        Alloc allocator(&oa);
        Node *n1Ptr = AllocTraits::allocate(allocator, 1);

        Node& mN1 = *n1Ptr; const Node& N1 = mN1;
        AllocTraits::construct(allocator, bsls::Util::addressOf(mN1.value()));

        mN1.value() = bsltf::TemplateTestFacility::create<Key>(1);

        FunctorType functor(1);
        SetComparator<Key, FunctorType> mComp(functor);
        const SetComparator<Key, FunctorType>& comp = mComp;

        ASSERTV(comp.keyComparator().id() == functor.id());

        ASSERTV(N1.value(), K0, comp(K0, N1));
        ASSERTV(comp.keyComparator().numCalls(),
                1 == comp.keyComparator().numCalls());
        ASSERTV(0 == functor.numCalls());

        ASSERTV(K0, N1.value(), !comp(N1, K0));
        ASSERTV(comp.keyComparator().numCalls(),
                2 == comp.keyComparator().numCalls());
        ASSERTV(0 == functor.numCalls());

        NonConstFunctorType ncFunctor(1);
        SetComparator<Key, NonConstFunctorType> ncComp(ncFunctor);

        ASSERTV(ncComp.keyComparator().id() == ncFunctor.id());

        ASSERTV(N1.value(), K0, ncComp(K0, N1));
        ASSERTV(ncComp.keyComparator().numCalls(),
                1 == ncComp.keyComparator().numCalls());
        ASSERTV(0 == ncFunctor.numCalls());

        ASSERTV(K0, N1.value(), !ncComp(N1, K0));
        ASSERTV(ncComp.keyComparator().numCalls(),
                2 == ncComp.keyComparator().numCalls());
        ASSERTV(0 == ncFunctor.numCalls());

        AllocTraits::destroy(allocator, bsls::Util::addressOf(mN1.value()));
        AllocTraits::deallocate(allocator, n1Ptr, 1);
    }

    if (verbose)
        printf("\n\tTesting 'operator()' "
               "for objects constructed with a functor\n");
    {
        int id = 1;
        FunctorType functor(id);
        SetComparator<Key, FunctorType> mComp(functor);
        const SetComparator<Key, FunctorType>& comp = mComp;

        ASSERTV(0 == da.numBytesInUse());

        Key mK0 = bsltf::TemplateTestFacility::create<Key>(0);
        const Key& K0 = mK0;

        Alloc allocator(&oa);
        Node *n1Ptr = AllocTraits::allocate(allocator, 1);

        Node& mN1 = *n1Ptr; const Node& N1 = mN1;
        AllocTraits::construct(allocator, bsls::Util::addressOf(mN1.value()));

        mN1.value() = bsltf::TemplateTestFacility::create<Key>(1);

        ASSERTV(N1.value(), K0, comp(K0, N1));
        ASSERTV(comp.keyComparator().numCalls(),
                1 == comp.keyComparator().numCalls());
        ASSERTV(K0, N1.value(), !comp(N1, K0));
        ASSERTV(comp.keyComparator().numCalls(),
                2 == comp.keyComparator().numCalls());

        AllocTraits::destroy(allocator, bsls::Util::addressOf(mN1.value()));
        AllocTraits::deallocate(allocator, n1Ptr, 1);
    }

    if (verbose)
        printf("\n\tTesting 'operator()' "
               "for objects constructed with a function pointer\n");
    {
        SetComparator<Key, FunctionType> mComp(&lessThanFunction<TYPE>);
        const SetComparator<Key, FunctionType>& comp = mComp;
        FunctionType functionPtr = &lessThanFunction<TYPE>;
        ASSERT(functionPtr == comp.keyComparator());

        ASSERTV(0 == da.numBytesInUse());

        Key mK0 = bsltf::TemplateTestFacility::create<Key>(0);
        const Key& K0 = mK0;

        Alloc allocator(&oa);
        Node *n1Ptr = AllocTraits::allocate(allocator, 1);

        Node& mN1 = *n1Ptr; const Node& N1 = mN1;
        AllocTraits::construct(allocator, bsls::Util::addressOf(mN1.value()));

        mN1.value() = bsltf::TemplateTestFacility::create<Key>(1);

        numFunctionComp = 0;
        ASSERTV(N1.value(), K0, comp(K0, N1));
        ASSERTV(numFunctionComp, 1 == numFunctionComp);
        ASSERTV(K0, N1.value(), !comp(N1, K0));
        ASSERTV(numFunctionComp, 2 == numFunctionComp);

        AllocTraits::destroy(allocator, bsls::Util::addressOf(mN1.value()));
        AllocTraits::deallocate(allocator, n1Ptr, 1);
    }
}

template <class TYPE>
void TestDriver<TYPE>::test2()
{
    // --------------------------------------------------------------------
    // CONSTRUCTORS AND BASIC ACCESSORS
    // Concerns:
    //: 1 If a object is created using the default constructor,
    //:   'keyComparator' returns a default constructed 'COMPARATOR' object.
    //:
    //: 2 If a object is created using a functor 'COMPARATOR', 'keyComparator'
    //:   returns a copy constructed object of that functor.
    //:
    //: 3 If a object is created using a function pointer 'COMPARATOR',
    //:   'keyComparator' returns a pointer pointing to the function.
    //:
    //: 4 Default constructing an object with parameterized type as a function
    //:   pointer causes a compile time failure.
    //
    // Plan:
    //: 1 Construct a object using the default constructor with a functor type
    //:   as the parameterized 'COMPARATOR' type.  Verify the functor returned
    //:   by 'keyComparator' is default constructed.  (C-1)
    //:
    //: 2 Construct a object using a functor.  Verify the functor returned by
    //:  'keyComparator' compare equal to the functor used on construction'.
    //:  (C-2)
    //:
    //: 3 Construct a object using a function pointer.  Verify the function
    //:   pointer returned by 'keyComparator' points to the same address as
    //:   the function pointer used on construction.  (C-3)
    //:
    //: 4 Manually verify that default-constructing an object with a function
    //:   pointer as the parameterized 'COMPARATOR' type causes the compilation
    //:   to fail.  (C-4)
    //
    // Testing:
    //   SetComparator();
    //   explicit SetComparator(const COMPARATOR& keyComparator);
    //   COMPARATOR keyComparator() const;
    // --------------------------------------------------------------------
    if (verbose)
        printf("\n==TYPE: %s==\n", typeid(TYPE).name());

    bslma::TestAllocator da("default");
    bslma::TestAllocator oa("object");

    bslma::DefaultAllocatorGuard dag(&da);

    if (verbose)
        printf("\n\tTesting 'keyComparator()' "
               "for default constructed objects\n");
    {
        FunctorType functor;
        SetComparator<Key, FunctorType> mComp;
        const SetComparator<Key, FunctorType>& comp = mComp;
        ASSERT(comp.keyComparator() == functor);
    }

    if (verbose)
        printf("\n\tTesting 'keyComparator()' "
               "for objects constructed with a functor\n");
    {
        FunctorType functor(1);
        SetComparator<Key, FunctorType> mComp(functor);
        const SetComparator<Key, FunctorType>& comp = mComp;
        ASSERT(comp.keyComparator() == functor);
    }

    if (verbose)
        printf("\n\tTesting 'keyComparator()' "
               "for objects constructed with a function pointer\n");
    {
        SetComparator<Key, FunctionType> mComp(&lessThanFunction<TYPE>);
        const SetComparator<Key, FunctionType>& comp = mComp;
        FunctionType functionPtr = &lessThanFunction<TYPE>;
        ASSERT(functionPtr == comp.keyComparator());
    }

    // Manually test compilation failure for default constructing an object
    // with a function pointer as the parameterized type.
    {
        // SetComparator<Key, FunctionType> mComp;  // This will not compile
    }
}


template <class TYPE>
void TestDriver<TYPE>::test1()
{
    // --------------------------------------------------------------------
    // BREATHING TEST
    //   Developers' Sandbox.
    //
    // Plan:
    //   Perform and ad-hoc test of the primary modifiers and accessors.
    //
    // Testing:
    //   This "test" *exercises* basic functionality, but *tests* nothing.
    // --------------------------------------------------------------------

    if (verbose)
        printf("\n==TYPE: %s==\n", typeid(TYPE).name());

    bslma::TestAllocator da("default");
    bslma::TestAllocator oa("object");

    bslma::DefaultAllocatorGuard dag(&da);


    if (verbose)
        printf("Verify Using Functor Comparator in SetComparator\n");
    {
        // Verify Using Functor Comparator in SetComparator
        Alloc allocator(&oa);
        SetComparator<Key, FunctorType> mComp;
        const SetComparator<Key, FunctorType>& comp = mComp;
        Key mK0 = bsltf::TemplateTestFacility::create<Key>(0);
        const Key& K0 = mK0;
        Key mK1 = bsltf::TemplateTestFacility::create<Key>(1);
        const Key& K1 = mK1;

        Node *nPtr = AllocTraits::allocate(allocator, 1);
        Node& mN = *nPtr; const Node& N = mN;
        AllocTraits::construct(allocator, bsls::Util::addressOf(mN.value()));

        mN.value() = K0;
        ASSERTV(N.value(), K0, comp(N, K1));

        AllocTraits::destroy(allocator, bsls::Util::addressOf(mN.value()));

        AllocTraits::deallocate(allocator, nPtr, 1);
    }

    if (verbose)
        printf("Verify Using Function Pointer Comparator in SetComparator.\n");
    {
        Alloc allocator(&oa);

        SetComparator<Key, FunctionType> mComp(&lessThanFunction<TYPE>);
        const SetComparator<Key, FunctionType>& comp = mComp;
        Key mK0 = bsltf::TemplateTestFacility::create<Key>(0);
        const Key& K0 = mK0;
        Key mK1 = bsltf::TemplateTestFacility::create<Key>(1);
        const Key& K1 = mK1;

        Node *nPtr = AllocTraits::allocate(allocator, 1);
        Node& mN = *nPtr; const Node& N = mN;
        AllocTraits::construct(allocator, bsls::Util::addressOf(mN.value()));

        mN.value() = K0;
        ASSERTV(N.value(), K0, comp(N, K1));

        AllocTraits::destroy(allocator, bsls::Util::addressOf(mN.value()));

        AllocTraits::deallocate(allocator, nPtr, 1);
    }
}
}  // close unnamed namespace

//=============================================================================
//                                 MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test            = argc > 1 ? atoi(argv[1]) : 0;
    verbose             = argc > 2;
    veryVerbose         = argc > 3;
    veryVeryVerbose     = argc > 4;
    veryVeryVeryVerbose = argc > 5;

    printf("TEST " __FILE__ " CASE %d\n", test);

    // CONCERN: No memory is ever allocated from this class.

    bslma::TestAllocator ga("global", veryVeryVeryVerbose);
    bslma::Default::setGlobalAllocator(&ga);

    bslma::TestAllocator da("default", veryVeryVeryVerbose);
    ASSERT(0 == bslma::Default::setDefaultAllocator(&da));

    bslma::TestAllocatorMonitor gam(&ga), dam(&da);

    switch (test) { case 0:
      case 5: {
          if (verbose) printf("\nUSAGE EXAMPLE"
                              "\n=============\n");
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Create a Simple Tree of 'TreeNode' Objects
///- - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose that we want to create a tree of 'TreeNode' objects arranged
// according to a functor that we supply.
//
// First, we create an array of 'bslstl::TreeNode' objects, each holding a pair
// of integers:
//..
          typedef bsl::allocator<TreeNode<int> > Alloc;

          bslma::TestAllocator oa;
          Alloc allocator(&oa);

          enum { NUM_NODES = 5 };

          TreeNode<int>*       nodes[NUM_NODES];

          for (int i = 0; i < NUM_NODES; ++i) {
              nodes[i] = allocator.allocate(1);
              nodes[i]->value() = i;
          }
//..
// Then, we define a 'SetComparator' object, 'comparator', for comparing
// 'bslstl::TreeNode<int>' objects with integers.
//..
          SetComparator<int, std::less<int> > comparator;
//..
// Now, we can use the functions in 'bslalg::RbTreeUtil' to arrange our tree:
//..
          bslalg::RbTreeAnchor tree;

          for (int i = 0; i < NUM_NODES; ++i) {
              int comparisonResult;
              bslalg::RbTreeNode *insertLocation =
                  bslalg::RbTreeUtil::findUniqueInsertLocation(
                      &comparisonResult,
                      &tree,
                      comparator,
                      nodes[i]->value());

              ASSERT(0 != comparisonResult);

              bslalg::RbTreeUtil::insertAt(&tree,
                                           insertLocation,
                                           comparisonResult < 0,
                                           nodes[i]);
          }

          ASSERT(5 == tree.numNodes());
//..
// Then, we use 'bslalg::RbTreeUtil::next()' to navigate the elements of the
// tree, printing their values:
//..
          const bslalg::RbTreeNode *nodeIterator = tree.firstNode();

          while (nodeIterator != tree.sentinel()) {
              printf("Node value: %d\n",
                    static_cast<const TreeNode<int> *>(nodeIterator)->value());
              nodeIterator = bslalg::RbTreeUtil::next(nodeIterator);
          }
//..
// Next, we destroy and deallocate each of the 'bslstl::TreeNode' objects:
//..
          for (int i = 0; i < NUM_NODES; ++i) {
              allocator.deallocate(nodes[i], 1);
          }
//..
// Finally, we observe the console output:
//..
//  Node value: 0
//  Node value: 1
//  Node value: 2
//  Node value: 3
//  Node value: 4
//..
      } break;
      case 4: {
        if (verbose) printf("\nSWAP METHOD"
                            "\n===========\n");
        RUN_EACH_TYPE(TestDriver,
                      test4,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);
      } break;
      case 3: {
        if (verbose) printf("\nFUNCTION OPERATORS"
                            "\n==================\n");
        RUN_EACH_TYPE(TestDriver,
                      test3,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);
      } break;
      case 2: {
        if (verbose) printf("\nCONSTRUCTORS AND BASIC ACCESSORS"
                            "\n================================\n");
        RUN_EACH_TYPE(TestDriver,
                      test2,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);
      } break;
      case 1: {
        if (verbose) printf("\nBREATHING TEST"
                            "\n==============\n");
        RUN_EACH_TYPE(TestDriver,
                      test1,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);
      } break;
      default: {
          fprintf(stderr, "WARNING: CASE `%d' NOT FOUND.\n", test);
          testStatus = -1;
      }
    }

    // CONCERN: No memory is ever allocated from this class.

    ASSERTV(gam.isTotalSame());
    ASSERTV(dam.isTotalSame());

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
