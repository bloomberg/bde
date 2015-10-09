// bslstl_unorderedmultimap.t.cpp                                     -*-C++-*-

#include <bslstl_unorderedmultimap.h>

#include <bslstl_pair.h>
#include <bslstl_string.h>

#include <bslalg_swaputil.h>

#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_mallocfreeallocator.h>
#include <bslma_testallocator.h>
#include <bslma_testallocatormonitor.h>
#include <bslma_usesbslmaallocator.h>

#include <bslmf_haspointersemantics.h>

#include <bsls_assert.h>
#include <bsls_bsltestutil.h>
#include <bsls_objectbuffer.h>

#include <bsltf_stdtestallocator.h>
#include <bsltf_templatetestfacility.h>
#include <bsltf_testvaluesarray.h>

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

// To resolve gcc warnings, while printing 'size_t' arguments portably on
// Windows, we use a macro and string literal concatenation to produce the
// correct 'printf' format flag.
#ifdef ZU
#undef ZU
#endif

#if defined BSLS_PLATFORM_CMP_MSVC
#  define ZU "%Iu"
#else
#  define ZU "%zu"
#endif

#if defined(BDE_BUILD_TARGET_EXC)
enum IBM_MUST_NAME_ENUM_TYPES_IF_THEY_ARE_USED_IN_ASSERTV { PLAT_EXC = 1 };
#else
enum IBM_MUST_NAME_ENUM_TYPES_IF_THEY_ARE_USED_IN_ASSERTV { PLAT_EXC = 0 };
#endif

using namespace BloombergLP;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
// Initial breathing test iterates all operations with a single template
// instantiation and test obvious boundary conditions and iterator stability
// guarantees.
//
// MULTIMAP TEST SHOULD MAP *DIFFERENT* VALUES AGAINST DUPLICATE KEYS AND TEST
// ACCORDINGLY.
//-----------------------------------------------------------------------------
// [ ]
//-----------------------------------------------------------------------------
// [1] BREATHING TEST
// [2] USAGE EXAMPLE
//-----------------------------------------------------------------------------

// ============================================================================
//                      STANDARD BDE ASSERT TEST MACROS
// ----------------------------------------------------------------------------
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

#define EXCEPTION_TEST_BEGIN(CONTAINER)                                       \
        BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(                             \
          (* (bslma::TestAllocator *) (CONTAINER).get_allocator().mechanism()))

#define EXCEPTION_TEST_END  BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

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
//                             BSL TEST SUPPORT
//-----------------------------------------------------------------------------

namespace bsl {

template <class FIRST, class SECOND>
inline
void debugprint(const bsl::pair<FIRST, SECOND>& p)
{
    bsls::BslTestUtil::callDebugprint(p.first);
    bsls::BslTestUtil::callDebugprint(p.second);
}

// unordered_multimap-specific print function.

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOC>
void debugprint(
              const bsl::unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOC>& s)
{
    if (s.empty()) {
        printf("<empty>");
    }
    else {
        typedef bsl::unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOC> TObj;
        typedef typename TObj::const_iterator CIter;
        typedef typename TObj::const_local_iterator LCIter;

        for (size_t n = 0; n < s.bucket_count(); ++n) {
            if (s.cbegin(n) == s.cend(n)) {
                continue;
            }
            printf("\nBucket [" ZU "]: ", n);
            for (LCIter lci = s.cbegin(n); lci != s.cend(n); ++lci) {
                bsls::BslTestUtil::callDebugprint(static_cast<char>(
                      bsltf::TemplateTestFacility::getIdentifier(lci->first)));
            }
            printf("\n");
        }
    }
    fflush(stdout);
}

}  // close namespace bsl

//=============================================================================
//                               GLOBAL DATA
//-----------------------------------------------------------------------------

namespace {

struct DefaultDataRow {
    int         d_line;     // source line number
    const char *d_spec;     // specification string, for input to 'gg' function
};

const DefaultDataRow DEFAULT_DATA[] = {
    //line spec
    //---- --------
    { L_,  "" },
    { L_,  "A" },
    { L_,  "AA" },
    { L_,  "B" },
    { L_,  "AB" },
    { L_,  "AC" },
    { L_,  "CD" },
    { L_,  "ABC" },
    { L_,  "BAD" },
    { L_,  "ABCA" },
    { L_,  "ABCB" },
    { L_,  "ABCC" },
    { L_,  "LLLLQQQQ" },
    { L_,  "AABBCC" },
    { L_,  "ABCD" },
    { L_,  "BEADDDDD" },
    { L_,  "BCDE" },
    { L_,  "ABCDE" },
    { L_,  "FEDCB" },
    { L_,  "FEDCBFEDCB" },
    { L_,  "FEDCBA" },
    { L_,  "ABCDEFG" },
    { L_,  "ABCDEFGHAB" },
    { L_,  "ABCDEFGHI" },
    { L_,  "ABCDEFGHIJKLMNOP" },
    { L_,  "ABCDEFGHIJKLMNOPP" },
    { L_,  "DHBIMACOPELGFKNJQ" },
    { L_,  "KEEPCOOLWITHCOOLIDGE" },
    { L_,  "ILIKEIKE" },
    { L_,  "NOONECANFIGHTLIKEKENNEDYCAN" },
    { L_,  "ALLTHEWAYWITHLBJ" }
};
static const size_t DEFAULT_NUM_DATA = sizeof DEFAULT_DATA / sizeof *DEFAULT_DATA;

}  // close unnamed namespace

//=============================================================================
//                     GLOBAL VARIABLES / FUNCTIOONS / CLASSES
//-----------------------------------------------------------------------------

namespace {

bool             verbose;
bool         veryVerbose;
bool     veryVeryVerbose;
bool veryVeryVeryVerbose;

#if 0   // bslalg::SwapUtil is our componentized ADL swap-invoker.
template <class TYPE>
void invokeAdlSwap(TYPE& a, TYPE& b)
    // Exchange the values of the specified 'a' and 'b' objects using the
    // 'swap' method found by ADL (Argument Dependent Lookup).  The behavior
    // is undefined unless 'a' and 'b' were created with the same allocator.
{
    BSLS_ASSERT_OPT(a.get_allocator() == b.get_allocator());

    using namespace bsl;
    swap(a, b);
}
#endif

size_t numCharInstances(const char *SPEC, const char c)
{
    size_t ret = 0;
    for (const char *pc = SPEC; *pc; ++pc) {
        ret += (c == *pc);
    }
    return ret;
}

template <class TYPE>
const TYPE& my_max(const TYPE& x, const TYPE& y)
{
    return x > y ? x : y;
}

template <class TYPE>
TYPE my_abs(const TYPE& x)
{
    return x < 0 ? -x : x;
}

template <class TYPE>
bool nearlyEqual(const TYPE& x, const TYPE& y)
{
    TYPE tolerance = my_max(my_abs(x), my_abs(y)) * 0.0001;
    return my_abs(x - y) <= tolerance;
}

template <class TYPE>
const char *testTypeName()
{
    return "unrecognized type";
};

template <>
const char *testTypeName<signed char>()
{
    return "signed char";
};

template <>
const char *testTypeName<size_t>()
{
    return "size_t";
};

template <>
const char *testTypeName<bsltf::TemplateTestFacility::ObjectPtr>()
{
    return "ObjectPtr";
};

template <>
const char *testTypeName<bsltf::TemplateTestFacility::FunctionPtr>()
{
    return "FunctionPtr";
};

template <>
const char *testTypeName<bsltf::TemplateTestFacility::MethodPtr>()
{
    return "MethodPtr";
};

template <>
const char *testTypeName<bsltf::EnumeratedTestType::Enum>()
{
    return "Enum";
};

template <>
const char *testTypeName<bsltf::UnionTestType>()
{
    return "UnionTestType";
};

template <>
const char *testTypeName<bsltf::SimpleTestType>()
{
    return "SimpleTestType";
};

template <>
const char *testTypeName<bsltf::AllocTestType>()
{
    return "AllocTestType";
};

template <>
const char *testTypeName<bsltf::BitwiseMoveableTestType>()
{
    return "BitwiseMoveableTestType";
};

template <>
const char *testTypeName<bsltf::AllocBitwiseMoveableTestType>()
{
    return "AllocBitwiseMoveableTestType";
};

template <>
const char *testTypeName<bsltf::NonTypicalOverloadsTestType>()
{
    return "NonTypicalOverloadTestType";
};

template <class KEY, class VALUE>
struct CharToPairConverter {
    // Convert a 'char' value to a 'bsl::pair' of the parameterized 'KEY' and
    // 'VALUE' type.

    // CLASS METHODS
    static void createInplace(bsl::pair<const KEY, VALUE> *address,
                              char                         value,
                              bslma::Allocator            *allocator)
    {
        BSLS_ASSERT(address);
        BSLS_ASSERT(allocator);
        BSLS_ASSERT(0 < value);
#if CHAR_MAX >= 128
        BSLS_ASSERT(value < 128);
#endif

        // If creating the 'key' and 'value' temporary objects requires an
        // allocator, it should not be the default allocator as that will
        // confuse the arithmetic of our test machinery.  Therefore, we will
        // use the global MallocFree allocator, as being the simplest, least
        // obtrusive allocator that is also unlikely to be employed by an end
        // user.

        bslma::Allocator *privateAllocator =
                                      &bslma::MallocFreeAllocator::singleton();

        bsls::ObjectBuffer<KEY> tempKey;
        bsltf::TemplateTestFacility::emplace(
                                       bsls::Util::addressOf(tempKey.object()),
                                       value,
                                       privateAllocator);

        bsls::ObjectBuffer<VALUE> tempValue;
        bsltf::TemplateTestFacility::emplace(
                                     bsls::Util::addressOf(tempValue.object()),
                                     value - 'A' + '0',
                                     privateAllocator);

        bslalg::ScalarPrimitives::construct(address,
                                            tempKey.object(),
                                            tempValue.object(),
                                            allocator);
    }
};

                                //-----------
                                // verifySpec
                                //-----------

template <class CONTAINER>
bool verifySpec(const CONTAINER& obj, const char *SPEC, bool keysOnly = false)
    // Verify that the specified 'SPEC' accurately and completely describes the
    // contents of container 'obj'.  It is assumed that 'cbegin', 'cend', and
    // 'find' work, and that identical nodes are stored adjacent to each other
    // in the container.
{
    typedef typename CONTAINER::key_type        Key;
    typedef typename CONTAINER::mapped_type     Value;
    typedef typename CONTAINER::const_iterator  CIter;
    typedef typename CONTAINER::value_type      Pair;

    bslma::Allocator *pDa = &bslma::NewDeleteAllocator::singleton();
    bslma::DefaultAllocatorGuard guard(pDa);

    bsltf::TestValuesArray<Pair, CharToPairConverter<Key, Value> > VALUES;

    const size_t LENGTH = strlen(SPEC);

    size_t num = 0;
    CIter end = obj.cend();
    for (CIter it = obj.cbegin(); end != it; ++it) {
        ++num;
    }
    if (LENGTH != num) {
        return false;                                                 // RETURN
    }

    for (const char *pc = SPEC; *pc; ++pc) {
        if (*pc < 'A' || *pc > 'Z') {
            // invalid spec

            return false;                                             // RETURN
        }
    }

    // sort SPEC so that all identical chars are adjacent to each other

    char buf[1000];
    BSLS_ASSERT(LENGTH < sizeof(buf));
    strcpy(buf, SPEC);
    for (const char *pcEnd = buf + LENGTH - 1; pcEnd > buf; --pcEnd) {
        for (char *pc = buf; pc < pcEnd; ++pc) {
            if (pc[0] > pc[1]) {
                char tmp = pc[0];
                pc[0] = pc[1];
                pc[1] = tmp;
            }
        }
    }

    // verify sort

    for (char *pc = buf, c = 0; *pc; c = *pc++) {
        ASSERTV(*pc >= c);
    }
    ASSERTV(LENGTH == strlen(buf));

    SPEC = buf;

    size_t total = 0;
    for (const char *pc = SPEC; *pc; ) {
        const char C = *pc;
        size_t numInSpec = 0;
        do {
            ++numInSpec;
            ++pc;
        } while (C == *pc);

        size_t numInObj = 0;
        const Pair& p = VALUES[C - 'A'];

        for (CIter it = obj.find(p.first);
             end != it && (keysOnly ? p.first == it->first
                                    : p       == *it);
             ++it) {
            ++numInObj;
        }

        if (numInObj != numInSpec) {
            return false;                                             // RETURN
        }

        total += numInObj;
    }
    ASSERTV(LENGTH == total);

    return true;
}

                            // ====================
                            // class ExceptionGuard
                            // ====================

template <class OBJECT>
struct ExceptionGuard {
    // This class provide a mechanism to verify the strong exception guarantee
    // in exception-throwing code.  On construction, this class stores the
    // a copy of an object of the parameterized type 'OBJECT' and the address
    // of that object.  On destruction, if 'release' was not invoked, it will
    // verify the value of the object is the same as the value of the copy
    // create on construction.  This class requires the copy constructor and
    // 'operator ==' to be tested before use.

    // DATA
    int           d_line;      // the line number at construction
    OBJECT        d_copy;      // copy of the object being tested
    const OBJECT *d_object_p;  // address of the original object

  public:
    // CREATORS
    ExceptionGuard(const OBJECT    *object,
                   int              line,
                   bslma::Allocator *basicAllocator = 0)
    : d_line(line)
    , d_copy(*object, basicAllocator)
    , d_object_p(object)
        // Create the exception guard for the specified 'object' at the
        // specified 'line' number.  Optionally, specify 'basicAllocator' used
        // to supply memory.
    {}

    ~ExceptionGuard()
        // Destroy the exception guard.  If the guard was not released, verify
        // that the state of the object supplied at construction has not
        // change.
    {
        if (d_object_p) {
            const int LINE = d_line;
            ASSERTV(LINE, d_copy == *d_object_p);
        }
    }

    // MANIPULATORS
    void release()
        // Release the guard from verifying the state of the object.
    {
        d_object_p = 0;
    }
};

                       // ====================
                       // class TestComparator
                       // ====================

bool g_enableAllFunctorsFlag = true;

template <class TYPE>
class TestEqualityComparator {
    // This test class provides a mechanism that defines a function-call
    // operator that compares two objects of the parameterized 'TYPE'.  The
    // function-call operator is implemented with integer comparison using
    // integers converted from objects of 'TYPE' by the class method
    // 'TemplateTestFacility::getIdentifier'.  The function-call operator also
    // increments a counter used to keep track the method call count.  Object
    // of this class can be identified by an id passed on construction.

    // DATA
    int         d_id;           // identifier for the functor
    mutable int d_count;        // number of times 'operator()' is called

  public:
    // CLASS METHOD
    static void disableFunctor()
        // Disable all objects of 'TestComparator' such that an 'ASSERT' will
        // be triggered if 'operator()' is invoked
    {
        g_enableAllFunctorsFlag = false;
    }

    static void enableFunctor()
        // Enable all objects of 'TestComparator' such that 'operator()' may
        // be invoked
    {
        g_enableAllFunctorsFlag = true;
    }

    // CREATORS
    //! TestEqualityComparator(const TestEqualityComparator& original) =
    //                                                                 default;
        // Create a copy of the specified 'original'.

    explicit TestEqualityComparator(int id = 0)
        // Create a 'TestComparator'.  Optionally, specify 'id' that can be
        // used to identify the object.
    : d_id(id)
    , d_count(0)
    {
    }

    // ACCESSORS
    bool operator() (const TYPE& lhs, const TYPE& rhs) const
        // Increment a counter that records the number of times this method is
        // called.   Return 'true' if the integer representation of the
        // specified 'lhs' is less than integer representation of the specified
        // 'rhs'.
    {
        if (!g_enableAllFunctorsFlag) {
            ASSERTV(!"'TestComparator' was invoked when it was disabled");
        }

        ++d_count;

        return bsltf::TemplateTestFacility::getIdentifier<TYPE>(lhs)
            == bsltf::TemplateTestFacility::getIdentifier<TYPE>(rhs);
    }

    bool operator== (const TestEqualityComparator& rhs) const
    {
        return (id() == rhs.id());// && d_compareLess == rhs.d_compareLess);
    }

    int id() const
        // Return the 'id' of this object.
    {
        return d_id;
    }

    size_t count() const
        // Return the number of times 'operator()' is called.
    {
        return d_count;
    }
};


template <class TYPE>
class TestNonConstEqualityComparator {
    // This test class provides a functor for equality comparison of objects
    // where the 'operator()' is not declared const.

  public:
    // CREATORS
    TestNonConstEqualityComparator() {}
        // Create a 'TestComparator'.  Optionally, specify 'id' that can be
        // used to identify the object.

    //! TestNonConstEqualityComparator(const TestEqualityComparator& original)
    //                                                               = default;
        // Create a copy of the specified 'original'.

    // ACCESSORS
    bool operator() (const TYPE& lhs, const TYPE& rhs)
        // Increment a counter that records the number of times this method is
        // called.   Return 'true' if the integer representation of the
        // specified 'lhs' is less than integer representation of the specified
        // 'rhs'.
    {
        return bsltf::TemplateTestFacility::getIdentifier<TYPE>(lhs)
            == bsltf::TemplateTestFacility::getIdentifier<TYPE>(rhs);
    }

    bool operator==(const TestNonConstEqualityComparator&) const
    {
        return true;
    }
};

template <class TYPE>
class TestHashFunctor {
    // This test class provides a mechanism that defines a function-call
    // operator that compares two objects of the parameterized 'TYPE'.  The
    // function-call operator is implemented with integer comparison using
    // integers converted from objects of 'TYPE' by the class method
    // 'TemplateTestFacility::getIdentifier'.  The function-call operator also
    // increments a counter used to keep track the method call count.  Object
    // of this class can be identified by an id passed on construction.

    // DATA
    int         d_id;           // identifier for the functor
    mutable int d_count;        // number of times 'operator()' is called
    bool        d_shortCircuit; // always returns 0 if true

  public:
    // CLASS METHOD
    static void disableFunctor()
        // Disable all objects of 'TestComparator' such that an 'ASSERT' will
        // be triggered if 'operator()' is invoked
    {
        g_enableAllFunctorsFlag = false;
    }

    static void enableFunctor()
        // Enable all objects of 'TestComparator' such that 'operator()' may
        // be invoked
    {
        g_enableAllFunctorsFlag = true;
    }

    // CREATORS
    //! TestHashFunctor(const TestHashFunctor& original) = default;
        // Create a copy of the specified 'original'.

    explicit TestHashFunctor(int id = 0, bool shortCircuit = false)
        // Create a 'TestComparator'.  Optionally, specify 'id' that can be
        // used to identify the object.  Also optionally specify
        // 'shortCircuit', which, if 'true' indicates that this functor will
        // always return 0.
    : d_id(id)
    , d_count(0)
    , d_shortCircuit(shortCircuit)
    {
    }

    // ACCESSORS
    size_t operator() (const TYPE& obj) const
        // Increment a counter that records the number of times this method is
        // called.   Return 'true' if the integer representation of the
        // specified 'lhs' is less than integer representation of the specified
        // 'rhs'.
    {
        if (!g_enableAllFunctorsFlag) {
            ASSERTV(!"'TestComparator' was invoked when it was disabled");
        }

        ++d_count;

        return d_shortCircuit
               ? 0
               : bsltf::TemplateTestFacility::getIdentifier<TYPE>(obj);
    }

    bool operator== (const TestHashFunctor& rhs) const
    {
        return (id() == rhs.id());// && d_compareLess == rhs.d_compareLess);
    }

    int id() const
        // Return the 'id' of this object.
    {
        return d_id;
    }

    size_t count() const
        // Return the number of times 'operator()' is called.
    {
        return d_count;
    }
};

template <class TYPE>
class TestNonConstHashFunctor {
    // This class provides a hash functor whose 'operator()()' hasn't been
    // declared 'const'.

  public:
    // CREATORS
    TestNonConstHashFunctor() {}
        // Create a copy of the specified 'original'.
    //! TestNonConstHashFunctor(const TestHashFunctor& original) = default;
        // Create a copy of the specified 'original'.

    // ACCESSORS
    size_t operator() (const TYPE& obj)
        // Increment a counter that records the number of times this method is
        // called.   Return 'true' if the integer representation of the
        // specified 'lhs' is less than integer representation of the specified
        // 'rhs'.
    {
        return bsltf::TemplateTestFacility::getIdentifier<TYPE>(obj);
    }

    bool operator==(const TestNonConstHashFunctor&)
    {
        return true;
    }
};

}  // close unnamed namespace

//=============================================================================
//                              TestDriver
//-----------------------------------------------------------------------------

                            // ================
                            // class TestDriver
                            // ================

template <class KEY,
          class VALUE = KEY,
          class HASH  = TestHashFunctor<KEY>,
          class EQUAL = TestEqualityComparator<KEY>,
          class ALLOC = bsl::allocator<bsl::pair<const KEY, VALUE> > >
class TestDriver {
    // This templatized struct provide a namespace for testing the
    // 'unordered_multimap' container.  The parameterized 'KEY', 'VALUE',
    // 'HASH', 'COMP' and 'ALLOC' specifies the key type, the mapped type, the
    // hash functor, the equality comparator type and allocator type
    // respectively.  Each "testCase*" method test a specific aspect of
    // 'unordered_map<KEY, VALUE, HASH, COMP, ALLOC>'.  Every test cases should
    // be invoked with various parameterized type to fully test the container.

  private:
    // TYPES
    typedef bsl::unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOC>  Obj;
        // Type under testing.

    typedef TestNonConstEqualityComparator<KEY> NonConstEqual;
        // Comparator functor with a non-const function call operator.

    typedef typename Obj::iterator                Iter;
    typedef typename Obj::const_iterator          CIter;
    typedef typename Obj::local_iterator          LIter;
    typedef typename Obj::const_local_iterator    CLIter;
    typedef typename Obj::size_type               SizeType;
    typedef typename Obj::value_type              Pair;
        // Shorthands

    BSLMF_ASSERT((!bslmf::IsSame< Iter,  CIter>::value));
    BSLMF_ASSERT((!bslmf::IsSame<LIter, CLIter>::value));

    static const bool TYPE_ALLOC = bslma::UsesBslmaAllocator<KEY>::value;

    typedef bsltf::TestValuesArray<
                                  Pair,
                                  CharToPairConverter<KEY, VALUE> > TestValues;

  public:
    typedef bsltf::StdTestAllocator<KEY> StlAlloc;

  private:
    // TEST APPARATUS
    //-------------------------------------------------------------------------
    // The generating functions interpret the given 'spec' in order from left
    // to right to configure the object according to a custom language.
    // Uppercase letters [A..Z] correspond to arbitrary (but unique) char
    // values to be appended to the
    // 'unordered_multimap<KEY, VALUE, COMP, ALLOC>' object.
    //
    // LANGUAGE SPECIFICATION:
    // -----------------------
    //
    // <SPEC>       ::= <EMPTY>   | <LIST>
    //
    // <EMPTY>      ::=
    //
    // <LIST>       ::= <ITEM>    | <ITEM><LIST>
    //
    // <ITEM>       ::= <ELEMENT> | <CLEAR>
    //
    // <ELEMENT>    ::= 'A' | 'B' | 'C' | 'D' | 'E' | ... | 'Z'
    //                                      // unique but otherwise arbitrary
    // Spec String  Description
    // -----------  -----------------------------------------------------------
    // ""           Has no effect; leaves the object empty.
    // "A"          Insert the value corresponding to A.
    // "AA"         Insert two values both corresponding to A.
    // "ABC"        Insert three values corresponding to A, B and C.
    //-------------------------------------------------------------------------

    static int ggg(Obj *object, const char *spec, int nosiy = 1);
        // Configure the specified 'object' according to the specified 'spec',
        // using only the primary manipulator function 'insert' and white-box
        // manipulator 'clear'.  Optionally specify a zero 'noisy' to suppress
        // 'spec' syntax error messages.  Return the index of the first invalid
        // character, and a negative value otherwise.  Note that this function
        // is used to implement 'gg' as well as allow for verification of
        // syntax error detection.

    static Obj& gg(Obj *object, const char *spec);
        // Return, by reference, the specified object with its value adjusted
        // according to the specified 'spec'.

    static Obj g(const char *spec);
        // Return, by value, a new object corresponding to the specified
        // 'spec'.

    static bool matchFirstValues(const Obj&        object,
                                 const TestValues& values,
                                 const size_t      count,
                                 bool              keysOnly = false);
        // Return true iffd exactly the first specified 'count' values of the
        // specified array 'values' match all of the objects in the specified
        // 'object'.

  public:
    // TEST CASES

    static void testCase16();
        // Testing Typedefs

    static void testCase15();
        // Growing functions

    static void testCase14();
        // Type Traits

    static void testCase13();
        // Object with STL allocator.

    static void testCase12();
        // Range C'tor

    static void testCase11();
        // Test generator functions 'g'.

//   static void testCase10();
//      // bslx streaming -- N/A

    static void testCase9();
        // operator=

    static void testCase8();
        // Swap

    static void testCase7();
        // Copy c'tor

    static void testCase6();
        // operator==

//   static void testCase5();
//      // operator<< -- N/A

    static void testCase4();
        // Basic accessor tests.

    static void testCase3();
        // Range c'tor, 'ggg' and 'gg'.

    static void testCase2();
        // Basic manipulator test.
};

template <class KEY>
class StdAllocTestDriver :
    public TestDriver<KEY,
                      KEY,
                      TestHashFunctor<KEY>,
                      TestEqualityComparator<KEY>,
                      bsltf::StdTestAllocator<bsl::pair<const KEY, KEY> > > {
};

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOC>
int TestDriver<KEY, VALUE, HASH, EQUAL, ALLOC>::ggg(Obj        *object,
                                                    const char *spec,
                                                    int         noisy)
{
    const TestValues VALUES;

    enum { SUCCESS = -1 };

    for (int i = 0; spec[i]; ++i) {
        if ('A' <= spec[i] && spec[i] <= 'Z') {
            object->insert(VALUES[spec[i] - 'A']);
        }
        else {
            if (noisy) {
                printf("Error, bad character ('%c') "
                       "in spec \"%s\" at position %d.\n", spec[i], spec, i);
            }

            // Discontinue processing this spec.

            return i;                                                 // RETURN
        }
   }
   return SUCCESS;
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOC>
bsl::unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOC>&
TestDriver<KEY, VALUE, HASH, EQUAL, ALLOC>::gg(Obj        *object,
                                               const char *spec)
{
    ASSERTV(ggg(object, spec) < 0);
    return *object;
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOC>
bsl::unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOC>
TestDriver<KEY, VALUE, HASH, EQUAL, ALLOC>::g(const char *spec)
{
    Obj object((bslma::Allocator *)0);
    return gg(&object, spec);
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOC>
bool TestDriver<KEY, VALUE, HASH, EQUAL, ALLOC>::matchFirstValues(
                                                    const Obj&        object,
                                                    const TestValues& values,
                                                    const size_t      count,
                                                    bool              keysOnly)
{
    CIter end = object.cend();
    size_t sizeObj = 0;
    for (CIter it = object.cbegin(), itB = it; end != it; it = itB) {
        size_t numInObj = 0;
        do {
            ++numInObj;
            ++itB;
        } while (end != itB && (keysOnly ? it->first == itB->first
                                         : *it       == *itB));

        size_t numInValues = 0;
        for (size_t j = 0; j < count; ++j) {
            if (keysOnly ? values[j].first == it->first
                         : values[j]       == *it) {
                ++numInValues;
            }
        }

        if (numInObj != numInValues) {
            return false;                                             // RETURN
        }

        sizeObj += numInObj;
    }

    return true;
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOC>
void TestDriver<KEY, VALUE, HASH, EQUAL, ALLOC>::testCase16()
{
    // ------------------------------------------------------------------------
    // TESTING TYPEDEFs
    // ------------------------------------------------------------------------

    typedef bsl::pair<const KEY, VALUE> VT;
    typedef typename Obj::size_type SzT;
    typedef typename Obj::difference_type Diff;
    typedef typename Obj::iterator It;
    typedef typename Obj::const_iterator CIt;
    typedef typename Obj::local_iterator LIt;
    typedef typename Obj::const_local_iterator CLIt;

    BSLMF_ASSERT((bslmf::IsSame<KEY,        typename Obj::key_type>::value));
    BSLMF_ASSERT((bslmf::IsSame<VT,         typename Obj::value_type>::value));
    BSLMF_ASSERT((bslmf::IsSame<VALUE,      typename Obj::mapped_type>::
                                                                       value));
    BSLMF_ASSERT((bslmf::IsSame<HASH,       typename Obj::hasher>::value));
    BSLMF_ASSERT((bslmf::IsSame<EQUAL,      typename Obj::key_equal>::value));
    BSLMF_ASSERT((bslmf::IsSame<ALLOC,      typename Obj::allocator_type>::
                                                                       value));
    BSLMF_ASSERT((bslmf::IsSame<VT *,       typename Obj::pointer>::value));
    BSLMF_ASSERT((bslmf::IsSame<const VT *, typename Obj::const_pointer>::
                                                                       value));
    BSLMF_ASSERT((bslmf::IsSame<VT &,       typename Obj::reference>::value));
    BSLMF_ASSERT((bslmf::IsSame<const VT &, typename Obj::const_reference>::
                                                                       value));

    BSLMF_ASSERT((bslmf::IsSame<typename Obj::pointer,
                                typename ALLOC::pointer>::value));
    BSLMF_ASSERT((bslmf::IsSame<typename Obj::const_pointer,
                                typename ALLOC::const_pointer>::value));
    BSLMF_ASSERT((bslmf::IsSame<typename Obj::reference,
                                typename ALLOC::reference>::value));
    BSLMF_ASSERT((bslmf::IsSame<typename Obj::const_reference,
                                typename ALLOC::const_reference>::value));
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOC>
void TestDriver<KEY, VALUE, HASH, EQUAL, ALLOC>::testCase15()
{
    // ------------------------------------------------------------------------
    // BUCKET GROWTH
    // ------------------------------------------------------------------------

    {
        typedef float (Obj::*MP)() const;
        MP mp = &Obj::load_factor;
        (void) mp;
        mp    = &Obj::max_load_factor;
        (void) mp;
    }
    {
        typedef void (Obj::*MP)(float);
        MP mp = &Obj::max_load_factor;
        (void) mp;
    }
    {
        typedef void (Obj::*MP)(size_t);
        MP mp = &Obj::reserve;
        (void) mp;
        mp    = &Obj::rehash;
        (void) mp;
    }

    const size_t NUM_DATA                  = DEFAULT_NUM_DATA;
    const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

    bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

    for (size_t ti = 0; ti < NUM_DATA; ++ti) {
        const int         LINE   = DATA[ti].d_line;
        const char       *SPEC   = DATA[ti].d_spec;
        const size_t      LENGTH = strlen(SPEC);

        bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);

        if (veryVeryVerbose) Q(Test 'rehash');
        {
            Obj mX(&sa);    const Obj& X = mX;
            gg(&mX, SPEC);

            const size_t BC = X.bucket_count();

            ASSERTV((LENGTH > 0) == (BC > 1));

            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(sa) {
                ASSERTV(BC == X.bucket_count());

                mX.rehash(2 * BC);
            } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

            ASSERTV((LENGTH > 0) == (X.bucket_count() > 2 * BC));
        }

        if (veryVeryVerbose) Q(Test 'load_factor' and 'max_load_factor');
        {
            const float loadFactors[] = { 1.0f, 2.0f, 1.0f / 2, 1.0f / 4 };
            enum { NUM_LOAD_FACTORS =
                                    sizeof loadFactors / sizeof *loadFactors };

            for (size_t tj = 0; tj < NUM_LOAD_FACTORS; ++tj) {
                const float MAX_LOAD_FACTOR = loadFactors[tj];

                Obj mX(&sa);    const Obj& X = mX;
                if (0 != tj) {
                    mX.max_load_factor(MAX_LOAD_FACTOR);
                }
                ASSERTV(X.max_load_factor() == MAX_LOAD_FACTOR);
                gg(&mX, SPEC);

                const size_t BC = X.bucket_count();

                ASSERTV(X.load_factor() <= X.max_load_factor());
                ASSERTV(nearlyEqual<double>(X.load_factor(),
                                            static_cast<double>(X.size())
                                            / static_cast<double>(BC)));

                if (LENGTH > 0) {
                    const float newLoadFactor = X.load_factor() / 2;
                    int numPasses = 0;
                    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(sa) {
                        ++numPasses;

                        ASSERTV(BC == X.bucket_count());

                        mX.max_load_factor(newLoadFactor);
                    } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
                    ASSERTV(!PLAT_EXC || numPasses > 1);

                    ASSERTV(newLoadFactor == X.max_load_factor());

                    ASSERTV(X.bucket_count() > 2 * BC);
                    ASSERTV(X.load_factor() <= X.max_load_factor());
                }
            }
        }

        if (veryVeryVerbose) Q(Test 'reserve');
        {
            for (size_t len2 = 0; len2 <= LENGTH; ++len2) {
                Obj mX(&sa);    const Obj& X = mX;

                TestValues values(SPEC, &sa);

                mX.insert(values.begin(), values.index(len2));

                int numPasses = 0;
                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(sa) {
                    ++numPasses;

                    mX.reserve(LENGTH);
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
                ASSERTV(!PLAT_EXC || len2 || 0 == LENGTH || numPasses > 1);

                const size_t BC = X.bucket_count();
                ASSERTV(X.load_factor() <= X.max_load_factor());
                ASSERTV(0.9999
                        * static_cast<double>(LENGTH)
                          / static_cast<double>(X.bucket_count()) <
                                                          X.max_load_factor());

                mX.insert(values.index(len2), values.end());

                ASSERTV(LINE, SPEC, LENGTH, X.size(), LENGTH == X.size());
                ASSERTV(verifySpec(X, SPEC));

                ASSERTV(BC == X.bucket_count());
            }
        }
    }
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOC>
void TestDriver<KEY, VALUE, HASH, EQUAL, ALLOC>::testCase14()
{
    // ------------------------------------------------------------------------
    // TESTING TYPE TRAITS
    //
    // Concern:
    //: 1 The object has the necessary type traits.
    //
    // Plan:
    //: 1 Use 'BSLMF_ASSERT' to verify all the type traits exists.  (C-1)
    //
    // Testing:
    //   CONCERN: The object has the necessary type traits
    // ------------------------------------------------------------------------

    // Verify set defines the expected traits.

    typedef bsl::unordered_multimap<KEY, VALUE> UMMKV;

    BSLMF_ASSERT((1 == bslalg::HasStlIterators<UMMKV>::value));

    BSLMF_ASSERT((1 == bslma::UsesBslmaAllocator<UMMKV>::value));

    BSLMF_ASSERT((1 == bslmf::IsBitwiseMoveable<UMMKV>::value));

    // Verify the bslma-allocator trait is not defined for non
    // bslma-allocators.

    typedef bsl::unordered_multimap<KEY, VALUE, HASH, EQUAL, StlAlloc>
                                                                   ObjStlAlloc;
    BSLMF_ASSERT((0 == bslma::UsesBslmaAllocator<ObjStlAlloc>::value));

    // Verify unordered_map does not define other common traits.

    BSLMF_ASSERT((0 == bsl::is_trivially_copyable<UMMKV>::value));

    BSLMF_ASSERT((0 == bslmf::IsBitwiseEqualityComparable<UMMKV>::value));

    BSLMF_ASSERT((0 == bslmf::HasPointerSemantics<UMMKV>::value));

    BSLMF_ASSERT((0 == bsl::is_trivially_default_constructible<UMMKV>::value));
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOC>
void TestDriver<KEY, VALUE, HASH, EQUAL, ALLOC>::testCase13()
{
    // ------------------------------------------------------------------------
    // TESTING STL ALLOCATOR
    //
    // Concern:
    //: 1 A standard compliant allocator can be used instead of
    //:   'bsl::allocator'.
    //:
    //: 2 Methods that uses the allocator (e.g., variations of constructor,
    //:   'insert' and 'swap') can successfully populate the object.
    //:
    //: 3 'KEY' types that allocate memory uses the default allocator instead
    //:   of the object allocator.
    //:
    //: 4 Every object releases any allocated memory at destruction.
    //
    // Plan:
    //: 1 Using a loop base approach, create a list of specs and their
    //:   expected value.  For each spec:
    //:
    //:   1 Create an object using a standard allocator through multiple ways,
    //:     including: range-based constructor, copy constructor, range-based
    //:     insert, multiple inserts, and swap.
    //:
    //:   2 Verify the value of each objects is as expected.
    //:
    //:   3 For types that allocate memory, verify memory for the elements
    //:     comes from the default allocator.
    //
    // Testing:
    //  CONCERN: 'set' is compatible with a standard allocator.
    // ------------------------------------------------------------------------

    const size_t NUM_DATA                  = DEFAULT_NUM_DATA;
    const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

    typedef bsl::unordered_multimap<KEY, VALUE, HASH, EQUAL, StlAlloc>
                                                                   ObjStlAlloc;

    bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

    for (size_t ti = 0; ti < NUM_DATA; ++ti) {
        const int         LINE   = DATA[ti].d_line;
        const char       *SPEC   = DATA[ti].d_spec;
        const size_t      LENGTH = strlen(SPEC);

        TestValues CONT(SPEC, &scratch);

        typename TestValues::iterator BEGIN = CONT.begin();
        typename TestValues::iterator END   = CONT.end();

        bslma::TestAllocator da("default",   veryVeryVeryVerbose);

        bslma::DefaultAllocatorGuard dag(&da);

        {
            ObjStlAlloc mX(BEGIN, END);  const ObjStlAlloc& X = mX;

            ASSERTV(LINE, verifySpec(X, SPEC));

            ObjStlAlloc mY(X);  const ObjStlAlloc& Y = mY;

            ASSERTV(LINE, verifySpec(Y, SPEC));

            ObjStlAlloc mZ;  const ObjStlAlloc& Z = mZ;

            mZ.swap(mX);

            ASSERTV(LINE, verifySpec(Z, SPEC));
        }

        CONT.resetIterators();

        {
            ObjStlAlloc mX;  const ObjStlAlloc& X = mX;
            mX.insert(BEGIN, END);
            ASSERTV(LINE, verifySpec(X, SPEC));
        }

        CONT.resetIterators();

        {
            ObjStlAlloc mX;  const ObjStlAlloc& X = mX;
            for (size_t tj = 0; tj < CONT.size(); ++tj) {
                Iter RESULT = mX.insert(CONT[tj]);

                ASSERTV(LINE, tj, LENGTH, CONT[tj] == *RESULT);
            }
            ASSERTV(LINE, verifySpec(X, SPEC));
        }

        ASSERTV(LINE, da.numBlocksInUse(), 0 == da.numBlocksInUse());
    }

    // IBM empty class swap bug test

    {
        typedef bsl::unordered_multimap<int,
                                        int,
                                        TestHashFunctor<int>,
                                        TestEqualityComparator<int>,
                                        StlAlloc> TestObjIntStlAlloc;

        TestObjIntStlAlloc mX;
        mX.insert(bsl::pair<int, int>(1, 1));
        TestObjIntStlAlloc mY;
        mY = mX;
    }
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOC>
void TestDriver<KEY, VALUE, HASH, EQUAL, ALLOC>::testCase12()
{
    // ------------------------------------------------------------------------
    // TESTING RANGE C'TORS
    //
    // Concern:
    //   That all c'tors taking a range of objects of type 'KEY' function
    //   correctly.
    // ------------------------------------------------------------------------

    TestValues VALUES;

    HASH  tstHash(7);
    EQUAL tstEqual(9);

    ASSERTV(!(HASH()  == tstHash));
    ASSERTV(!(EQUAL() == tstEqual));

    for (size_t i = 0; i < DEFAULT_NUM_DATA; ++i) {
        const int LINE      = DEFAULT_DATA[i].d_line;
        const char *SPEC    = DEFAULT_DATA[i].d_spec;
        const size_t LENGTH = strlen(SPEC);

        TestValues values(SPEC);

        int done = 0;
        for (char config = 'a'; config <= 'i'; ++config) {
            const char CONFIG = config;

            bslma::TestAllocator sa("scratch",   veryVeryVeryVerbose);
            bslma::TestAllocator fa("footprint", veryVeryVeryVerbose);
            bslma::TestAllocator da("default",   veryVeryVeryVerbose);
            bslma::DefaultAllocatorGuard dag(&da);

            bslma::TestAllocator&  oa = strchr("ei", CONFIG) ? sa : da;
            bslma::TestAllocator& noa = &oa == &da           ? sa : da;

            Obj *objPtr;
            int numPasses = 0;
            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                ++numPasses;
                values.resetIterators();

                switch (CONFIG) {
                  case 'a': {
                    objPtr = new (fa) Obj(values.begin(), values.end());
                  } break;
                  case 'b': {
                    objPtr = new (fa) Obj(values.begin(), values.end(), 0);
                  } break;
                  case 'c': {
                    objPtr = new (fa) Obj(values.begin(), values.end(), 0,
                                          tstHash);
                  } break;
                  case 'd': {
                    objPtr = new (fa) Obj(values.begin(), values.end(), 0,
                                          tstHash, tstEqual);
                  } break;
                  case 'e': {
                    objPtr = new (fa) Obj(values.begin(), values.end(), 0,
                                          tstHash, tstEqual, &sa);
                  } break;
                  case 'f': {
                    objPtr = new (fa) Obj(values.begin(), values.end(), 100);
                  } break;
                  case 'g': {
                    objPtr = new (fa) Obj(values.begin(), values.end(), 100,
                                          tstHash);
                  } break;
                  case 'h': {
                    objPtr = new (fa) Obj(values.begin(), values.end(), 100,
                                          tstHash, tstEqual);
                  } break;
                  case 'i': {
                    objPtr = new (fa) Obj(values.begin(), values.end(), 100,
                                          tstHash, tstEqual, &sa);
                    ++done;
                  } break;
                  default: {
                    ASSERTV(0 && "unrecognized CONFIG");
                    return;                                           // RETURN
                  }
                }
            } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

            const Obj& X = *objPtr;

            ASSERTV(0 == noa.numBlocksTotal());

            const bool EXP0 = CONFIG <= 'e' && 0 == LENGTH;
            ASSERTV(LINE, CONFIG, EXP0 == (0 == oa.numBlocksInUse()));
            ASSERTV(LINE, CONFIG, EXP0 == (0 == oa.numBlocksTotal()));

            ASSERTV(numPasses, CONFIG, EXP0,
                                  (PLAT_EXC && !EXP0)      == (numPasses > 1));
            ASSERTV(numPasses, CONFIG,
                                  (PLAT_EXC && LENGTH > 0) == (numPasses > 2));

            ASSERTV(LENGTH == X.size());
            ASSERTV(verifySpec(X, SPEC));

            Obj mY;            const Obj& Y = gg(&mY, SPEC);
            ASSERTV(X == Y);

            ASSERTV((CONFIG >= 'f') == (X.bucket_count() > 100));
            ASSERTV(CONFIG, (strchr("cdeghi", CONFIG) ? tstHash  : HASH())  ==
                                                            X.hash_function());
            ASSERTV(CONFIG, (strchr("dehi",   CONFIG) ? tstEqual : EQUAL()) ==
                                                            X.key_eq());

            fa.deleteObject(objPtr);
        }
        ASSERTV(1 == done);
    }
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOC>
void TestDriver<KEY, VALUE, HASH, EQUAL, ALLOC>::testCase11()
{
    // ------------------------------------------------------------------------
    // TESTING GENERATOR FUNCTION, g:
    //
    // Concern:
    //: 1 Since 'g' is implemented almost entirely using 'gg', we need to
    //:   verify only that the arguments are properly forwarded.
    //:
    //: 2 'g' does not affect the test allocator, and that 'g' returns an
    //:   object by value.
    //
    // Plan:
    //: 1 For each SPEC in a short list of specifications:
    //:
    //:   1 Compare the object returned (by value) from the generator function,
    //:     'g(SPEC)' with the value of a newly constructed OBJECT configured
    //:     using 'gg(&OBJECT,  SPEC)'.
    //:
    //:   2 Compare the results of calling the allocator's 'numBlocksTotal' and
    //:     'numBytesInUse' methods before and after calling 'g' in order to
    //:     demonstrate that 'g' has no effect on the test allocator.
    //:
    //:   3 Use 'sizeof' to confirm that the (temporary) returned by 'g'
    //:     differs in size from that returned by 'gg'.
    //
    // Testing:
    //   set g(const char *spec);
    // ------------------------------------------------------------------------

    bslma::TestAllocator oa(veryVeryVerbose);

    static const char *SPECS[] = {
        "", "A", "B", "C", "D", "E", "ABCDE",
        0  // null string required as last element
    };

    if (verbose)
        printf("\nCompare values produced by 'g' and 'gg' "
               "for various inputs.\n");

    for (int ti = 0; SPECS[ti]; ++ti) {
        const char *SPEC = SPECS[ti];
        if (veryVerbose) { P_(ti);  P(SPEC); }

        Obj mX(&oa);
        gg(&mX, SPEC);  const Obj& X = mX;

        if (veryVerbose) {
            printf("\t g = ");
            bsls::BslTestUtil::callDebugprint(g(SPEC));
            printf("\n");

            printf("\tgg = ");
            bsls::BslTestUtil::callDebugprint(X);
            printf("\n");
        }
        const bsls::Types::Int64 TOTAL_BLOCKS_BEFORE = oa.numBlocksTotal();
        const bsls::Types::Int64 IN_USE_BYTES_BEFORE = oa.numBytesInUse();
        ASSERTV(ti, X == g(SPEC));
        const bsls::Types::Int64 TOTAL_BLOCKS_AFTER = oa.numBlocksTotal();
        const bsls::Types::Int64 IN_USE_BYTES_AFTER = oa.numBytesInUse();
        ASSERTV(ti, TOTAL_BLOCKS_BEFORE == TOTAL_BLOCKS_AFTER);
        ASSERTV(ti, IN_USE_BYTES_BEFORE == IN_USE_BYTES_AFTER);
    }

    if (verbose) printf("\nConfirm return-by-value.\n");
    {
        const char *SPEC = "ABCDE";

        // compile-time fact
        ASSERT(sizeof(Obj) == sizeof g(SPEC));

        Obj x(&oa);                      // runtime tests
        Obj& r1 = gg(&x, SPEC);
        Obj& r2 = gg(&x, SPEC);
        const Obj& r3 = g(SPEC);
        const Obj& r4 = g(SPEC);
        ASSERT(&r2 == &r1);
        ASSERT(&x  == &r1);
        ASSERT(&r4 != &r3);
        ASSERT(&x  != &r3);
    }
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOC>
void TestDriver<KEY, VALUE, HASH, EQUAL, ALLOC>::testCase9()
{
    // ------------------------------------------------------------------------
    // COPY-ASSIGNMENT OPERATOR:
    //   Ensure that we can assign the value of any object of the class to any
    //   object of the class, such that the two objects subsequently have the
    //   same value.
    //
    // Concerns:
    //: 1 The assignment operator can change the value of any modifiable target
    //:   object to that of any source object.
    //:
    //: 2 If allocator propagation is not enabled for copy assignment, the
    //:   allocator address held by the target object is unchanged.
    //:
    //: 3 If allocator propagation is not enabled for copy assignment, any
    //:   memory allocation is from the target object's allocator.
    //:
    //: 4 The signature and return type are standard.
    //:
    //: 5 The reference returned is to the target object (i.e., '*this').
    //:
    //: 6 The value of the source object is not modified.
    //:
    //: 7 The allocator address held by the source object is unchanged.
    //:
    //: 8 QoI: Assigning a source object having the default-constructed value
    //:   allocates no memory.
    //:
    //: 9 Any memory allocation is exception neutral.
    //:
    //:10 Assigning an object to itself behaves as expected (alias-safety).
    //:
    //:11 Every object releases any allocated memory at destruction.
    //:
    //:12 If allocator propagation is enabled for copy assignment,
    //:   any memory allocation is from the source object's
    //:   allocator.
    //:
    //:13 If allocator propagation is enabled for copy assignment, the
    //:   allocator address held by the target object is changed to that of the
    //:   source.
    //:
    //:14 If allocator propagation is enabled for copy assignment, any memory
    //:   allocation is from the original target allocator will be released
    //:   after copy assignment.
    //
    // Plan:
    //: 1 Use the address of 'operator=' to initialize a member-function
    //:   pointer having the appropriate signature and return type for the
    //:   copy-assignment operator defined in this component.  (C-4)
    //:
    //: 2 Create a 'bslma::TestAllocator' object, and install it as the default
    //:   allocator (note that a ubiquitous test allocator is already installed
    //:   as the global allocator).
    //:
    //: 3 Using the table-driven technique:
    //:
    //:   1 Specify a set of (unique) valid object values.
    //:
    //: 4 For each row 'R1' (representing a distinct object value, 'V') in the
    //:   table described in P-3: (C-1..2, 5..8, 11)
    //:
    //:   1 Use the value constructor and a "scratch" allocator to create two
    //:     'const' 'Obj', 'Z' and 'ZZ', each having the value 'V'.
    //:
    //:   2 Execute an inner loop that iterates over each row 'R2'
    //:     (representing a distinct object value, 'W') in the table described
    //:     in P-3:
    //:
    //:   3 For each of the iterations (P-4.2): (C-1..2, 5..8, 11)
    //:
    //:     1 Create a 'bslma::TestAllocator' object, 'oa'.
    //:
    //:     2 Use the value constructor and 'oa' to create a modifiable 'Obj',
    //:       'mX', having the value 'W'.
    //:
    //:     3 Assign 'mX' from 'Z' in the presence of injected exceptions
    //:       (using the 'BSLMA_TESTALLOCATOR_EXCEPTION_TEST_*' macros).
    //:
    //:     4 Verify that the address of the return value is the same as that
    //:       of 'mX'.  (C-5)
    //:
    //:     5 Use the equality-comparison operator to verify that: (C-1, 6)
    //:
    //:       1 The target object, 'mX', now has the same value as that of 'Z'.
    //:         (C-1)
    //:
    //:       2 'Z' still has the same value as that of 'ZZ'.  (C-6)
    //:
    //:     6 Use the 'allocator' accessor of both 'mX' and 'Z' to verify that
    //:       the respective allocator addresses held by the target and source
    //:       objects are unchanged.  (C-2, 7)
    //:
    //:     7 Use the appropriate test allocators to verify that: (C-8, 11)
    //:
    //:       1 For an object that (a) is initialized with a value that did NOT
    //:         require memory allocation, and (b) is then assigned a value
    //:         that DID require memory allocation, the target object DOES
    //:         allocate memory from its object allocator only (irrespective of
    //:         the specific number of allocations or the total amount of
    //:         memory allocated); also cross check with what is expected for
    //:         'mX' and 'Z'.
    //:
    //:       2 An object that is assigned a value that did NOT require memory
    //:         allocation, does NOT allocate memory from its object allocator;
    //:         also cross check with what is expected for 'Z'.
    //:
    //:       3 No additional memory is allocated by the source object.  (C-8)
    //:
    //:       4 All object memory is released when the object is destroyed.
    //:         (C-11)
    //:
    //: 5 Repeat steps similar to those described in P-4 except that, this
    //:   time, there is no inner loop (as in P-4.2); instead, the source
    //:   object, 'Z', is a reference to the target object, 'mX', and both 'mX'
    //:   and 'ZZ' are initialized to have the value 'V'.  For each row
    //:   (representing a distinct object value, 'V') in the table described in
    //:   P-3: (C-9)
    //:
    //:   1 Create a 'bslma::TestAllocator' object, 'oa'.
    //:
    //:   2 Use the value constructor and 'oa' to create a modifiable 'Obj'
    //:     'mX'; also use the value constructor and a distinct "scratch"
    //:     allocator to create a 'const' 'Obj' 'ZZ'.
    //:
    //:   3 Let 'Z' be a reference providing only 'const' access to 'mX'.
    //:
    //:   4 Assign 'mX' from 'Z' in the presence of injected exceptions (using
    //:     the 'BSLMA_TESTALLOCATOR_EXCEPTION_TEST_*' macros).  (C-9)
    //:
    //:   5 Verify that the address of the return value is the same as that of
    //:     'mX'.
    //:
    //:   6 Use the equality-comparison operator to verify that the target
    //:     object, 'mX', still has the same value as that of 'ZZ'.
    //:
    //:   7 Use the 'allocator' accessor of 'mX' to verify that it is still the
    //:     object allocator.
    //:
    //:   8 Use the appropriate test allocators to verify that:
    //:
    //:     1 Any memory that is allocated is from the object allocator.
    //:
    //:     2 No additional (e.g., temporary) object memory is allocated when
    //:       assigning an object value that did NOT initially require
    //:       allocated memory.
    //:
    //:     3 All object memory is released when the object is destroyed.
    //:
    //: 6 Use the test allocator from P-2 to verify that no memory is ever
    //:   allocated from the default allocator.  (C-3)
    //
    // Testing:
    //   unordered_map& operator=(const unordered_map& rhs);
    // ------------------------------------------------------------------------

    const int NUM_DATA                     = DEFAULT_NUM_DATA;
    const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

    bslma::TestAllocator         da("default", veryVeryVeryVerbose);
    bslma::DefaultAllocatorGuard dag(&da);

    if (verbose) printf("\nCompare each pair of similar and different"
                        " values (u, ua, v, va) in S X A X S X A"
                        " without perturbation.\n");
    {
        // Create first object
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE1   = DATA[ti].d_line;
            const char *const SPEC1   = DATA[ti].d_spec;

            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

            Obj mZ(&scratch);  const Obj& Z  = gg(&mZ,  SPEC1);
            Obj mZZ(&scratch); const Obj& ZZ = gg(&mZZ, SPEC1);

            mZ.max_load_factor(2.0);

            if (veryVerbose) { T_ P_(LINE1) P_(Z) P(ZZ) }

            // Ensure the first row of the table contains the
            // default-constructed value.

            static bool firstFlag = true;
            if (firstFlag) {
                ASSERTV(LINE1, Obj(), Z, Obj() == Z);
                firstFlag = false;
            }

            // Create second object
            for (int tj = 0; tj < NUM_DATA; ++tj) {
                const int         LINE2   = DATA[tj].d_line;
                const char *const SPEC2   = DATA[tj].d_spec;

                bslma::TestAllocator oa("object", veryVeryVeryVerbose);

                {
                    Obj mX( &oa);  const Obj& X  = gg(&mX,   SPEC2);
                    Obj mXX(&oa);  const Obj& XX = gg(&mXX,  SPEC2);

                    mX.max_load_factor(3.0);

                    if (veryVerbose) { T_ P_(LINE2) P(X) }

                    ASSERTV(LINE1, LINE2, Z, X,
                            (Z == X) == (ti == tj));

                    bslma::TestAllocatorMonitor oam(&oa), sam(&scratch);

                    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                        if (veryVeryVerbose) { T_ T_ Q(ExceptionTestBody) }

                        ASSERT(XX == X);

                        Obj *mR = &(mX = Z);
                        ASSERTV(LINE1, LINE2,  Z,   X,  Z == X);
                        ASSERTV(LINE1, LINE2, mR, &mX, mR == &mX);
                    } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                    ASSERTV(2.0 == X.max_load_factor());

                    ASSERTV(LINE1, LINE2, ZZ, Z, ZZ == Z);

                    ASSERTV(LINE1, LINE2, &oa == X.get_allocator());
                    ASSERTV(LINE1, LINE2, &scratch == Z.get_allocator());

                    ASSERTV(LINE1, LINE2, sam.isInUseSame());

#if !defined(BDE_BUILD_TARGET_SAFE_2)
                    // When mX and Z use different allocators, the assignment
                    // is done through the copy-swap idiom.  This means the
                    // copy is destroyed within the assignment, and in SAFE2
                    // mode the destructor calls 'isWellFormed', which
                    // allocates from the default allocator.

                    ASSERTV(LINE1, LINE2, 0 == da.numBlocksTotal());
#endif
                }

                // Verify all memory is released on object destruction.

                ASSERTV(LINE1, LINE2, oa.numBlocksInUse(),
                             0 == oa.numBlocksInUse());
            }
        }
    }
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOC>
void TestDriver<KEY, VALUE, HASH, EQUAL, ALLOC>::testCase8()
{
    // ------------------------------------------------------------------------
    // SWAP MEMBER AND FREE FUNCTIONS
    //   Ensure that, when member and free 'swap' are implemented, we can
    //   exchange the values of any two objects that use the same
    //   allocator.
    //
    // Concerns:
    //: 1 Both functions exchange the values of the (two) supplied objects.
    //:
    //: 2 Both functions have standard signatures and return types.
    //:
    //: 3 Using either function to swap an object with itself does not
    //:   affect the value of the object (alias-safety).
    //:
    //: 4 If the two objects being swapped uses the same allocator, neither
    //:   function allocates memory from any allocator and the allocator
    //:   address held by both objects is unchanged.
    //:
    //: 5 If the two objects being swapped uses different allocators and
    //:   'AllocatorTraits::propagate_on_container_swap' is an alias to
    //:   'false_type', then both function may allocate memory and the
    //:   allocator address held by both object is unchanged.
    //:
    //: 6 If the two objects being swapped uses different allocators and
    //:   'AllocatorTraits::propagate_on_container_swap' is an alias to
    //:   'true_type', then no memory will be allocated and the allocators will
    //:   also be swapped.
    //:
    //: 7 Both functions provides the strong exception guarantee w.t.r. to
    //:   memory allocation .
    //:
    //: 8 The free 'swap' function is discoverable through ADL (Argument
    //:   Dependent Lookup).
    //
    // Plan:
    //: 1 Use the addresses of the 'swap' member and free functions defined
    //:   in this component to initialize, respectively, member-function
    //:   and free-function pointers having the appropriate signatures and
    //:   return types.  (C-2)
    //:
    //: 2 Create a 'bslma::TestAllocator' object, and install it as the
    //:   default allocator (note that a ubiquitous test allocator is
    //:   already installed as the global allocator).
    //:
    //: 3 Using the table-driven technique:
    //:
    //:   1 Specify a set of (unique) valid object values (one per row) in
    //:     terms of their individual attributes, including (a) first, the
    //:     default value, (b) boundary values corresponding to every range
    //:     of values that each individual attribute can independently
    //:     attain, and (c) values that should require allocation from each
    //:     individual attribute that can independently allocate memory.
    //:
    //:   2 Additionally, provide a (tri-valued) column, 'MEM', indicating
    //:     the expectation of memory allocation for all typical
    //:     implementations of individual attribute types: ('Y') "Yes",
    //:     ('N') "No", or ('?') "implementation-dependent".
    //:
    //: 4 For each row 'R1' in the table of P-3:  (C-1, 3..7)
    //:
    //:   1 Create a 'bslma::TestAllocator' object, 'oa'.
    //:
    //:   2 Use the value constructor and 'oa' to create a modifiable
    //:     'Obj', 'mW', having the value described by 'R1'; also use the
    //:     copy constructor and a "scratch" allocator to create a 'const'
    //:     'Obj' 'XX' from 'mW'.
    //:
    //:   3 Use the member and free 'swap' functions to swap the value of
    //:     'mW' with itself; verify, after each swap, that:  (C-3..4)
    //:
    //:     1 The value is unchanged.  (C-3)
    //:
    //:     2 The allocator address held by the object is unchanged.  (C-4)
    //:
    //:     3 There was no additional object memory allocation.  (C-4)
    //:
    //:   4 For each row 'R2' in the table of P-3:  (C-1, 4)
    //:
    //:     1 Use the copy constructor and 'oa' to create a modifiable
    //:       'Obj', 'mX', from 'XX' (P-4.2).
    //:
    //:     2 Use the value constructor and 'oa' to create a modifiable
    //:       'Obj', 'mY', and having the value described by 'R2'; also use
    //:       the copy constructor to create, using a "scratch" allocator,
    //:       a 'const' 'Obj', 'YY', from 'Y'.
    //:
    //:     3 Use, in turn, the member and free 'swap' functions to swap
    //:       the values of 'mX' and 'mY'; verify, after each swap, that:
    //:       (C-1..2)
    //:
    //:       1 The values have been exchanged.  (C-1)
    //:
    //:       2 The common object allocator address held by 'mX' and 'mY'
    //:         is unchanged in both objects.  (C-4)
    //:
    //:       3 There was no additional object memory allocation.  (C-4)
    //:
    //:     5 Use the value constructor and 'oaz' to a create a modifiable
    //:       'Obj' 'mZ', having the value described by 'R2'; also use the copy
    //:       constructor to create, using a "scratch" allocator, a const
    //:       'Obj', 'ZZ', from 'Z'.
    //:
    //:     6 Use the member and free 'swap' functions to swap the values of
    //:       'mX' and 'mZ' respectively (when
    //:       AllocatorTraits::propagate_on_container_swap is an alias to
    //:       false_type) under the presence of exception; verify, after each
    //:       swap, that:  (C-1, 5, 7)
    //:
    //:       1 If exception occurred during the swap, both values are
    //:         unchanged.  (C-7)
    //
    //:       2 If no exception occurred, the values have been exchanged.
    //:         (C-1)
    //:
    //:       3 The common object allocator address held by 'mX' and 'mZ' is
    //:         unchanged in both objects.  (C-5)
    //:
    //:       4 Temporary memory were allocated from 'oa' if 'mZ' is is not
    //:         empty, and temporary memory were allocated from 'oaz' if 'mX'
    //:         is not empty.  (C-5)
    //:
    //:     7 Create a new object allocator, 'oap'.
    //:
    //:     8 Use the value constructor and 'oap' to create a modifiable 'Obj'
    //:       'mP', having the value described by 'R2'; also use the copy
    //:       constructor to create, using a "scratch" allocator, a const
    //:       'Obj', 'PP', from 'P.
    //:
    //:     9 Manually change 'AllocatorTraits::propagate_on_container_swap' to
    //:       be an alias to 'true_type' (Instead of this manual step, use an
    //:       allocator that enables propagate_on_container_swap when
    //:       AllocatorTraits supports it) and use the the member and free
    //:       'swap functions to swap the values 'mX' and 'mZ' respectively;
    //:       verify, after each swap, that: (C-1, 6)
    //:
    //:       1 The values have been exchanged.  (C-1)
    //:
    //:       2 The allocators addresses have been exchanged.  (C-6)
    //:
    //:       3 There was no additional object memory allocation.  (C-6)
    //:
    //:    7 Create a new object allocator, 'oap'.
    //:
    //:    8 Use the value constructor and 'oap' to create a modifiable 'Obj'
    //:      'mP', having the value described by 'R2'; also use the copy
    //:      constructor to create, using a "scratch" allocator, a const 'Obj',
    //:      'PP', from 'P.
    //:
    //:    9 Manually change 'AllocatorTraits::propagate_on_container_swap' to
    //:      be an alias to 'true_type' (Instead of this manual step, use an
    //:      allocator that enables propagate_on_container_swap when
    //:      AllocatorTraits supports it) and use the the member and free 'swap
    //:      functions to swap the values 'mX' and 'mZ' respectively; verify,
    //:      after each swap, that:  (C-1, 6)
    //:
    //:      1 The values have been exchanged.  (C-1)
    //:
    //:      2 The allocators addresses have been exchanged.  (C-6)
    //:
    //:      3 There was no additional object memory allocation.  (C-6)
    //:
    //: 5 Verify that the free 'swap' function is discoverable through ADL:
    //:   (C-8)
    //:
    //:   1 Create a set of attribute values, 'A', distinct from the values
    //:     corresponding to the default-constructed object, choosing
    //:     values that allocate memory if possible.
    //:
    //:   2 Create a 'bslma::TestAllocator' object, 'oa'.
    //:
    //:   3 Use the default constructor and 'oa' to create a modifiable
    //:     'Obj' 'mX' (having default attribute values); also use the copy
    //:     constructor and a "scratch" allocator to create a 'const' 'Obj'
    //:     'XX' from 'mX'.
    //:
    //:   4 Use the value constructor and 'oa' to create a modifiable 'Obj'
    //:     'mY' having the value described by the 'Ai' attributes; also
    //:     use the copy constructor and a "scratch" allocator to create a
    //:     'const' 'Obj' 'YY' from 'mY'.
    //:
    //:   5 Use the 'invokeAdlSwap' helper function template to swap the
    //:     values of 'mX' and 'mY', using the free 'swap' function defined
    //:     in this component, then verify that:  (C-8)
    //:
    //:     1 The values have been exchanged.  (C-1)
    //:
    //:     2 There was no additional object memory allocation.  (C-4)
    //
    // Testing:
    //   void swap(unordered_map& other);
    //   void swap(unordered_map<K, V, H, E, A>& a,
    //             unordered_map<K, V, H, E, A>& b);
    // ------------------------------------------------------------------------

    if (verbose) printf("\nSWAP MEMBER AND FREE FUNCTIONS"
                        "\n==============================\n");

    if (verbose) printf(
                     "\nAssign the address of each function to a variable.\n");
    {
        typedef void (Obj::*funcPtr)(Obj&);
        typedef void (*freeFuncPtr)(Obj&, Obj&);

        // Verify that the signatures and return types are standard.

        funcPtr     memberSwap = &Obj::swap;
        freeFuncPtr freeSwap   = bsl::swap;

        (void) memberSwap;  // quash potential compiler warnings
        (void) freeSwap;
    }

    if (verbose) printf(
                 "\nCreate a test allocator and install it as the default.\n");

    bslma::TestAllocator         da("default", veryVeryVeryVerbose);
    bslma::DefaultAllocatorGuard dag(&da);

    if (verbose) printf(
       "\nUse a table of distinct object values and expected memory usage.\n");

    const int NUM_DATA                     = DEFAULT_NUM_DATA;
    const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

    for (int ti = 0; ti < NUM_DATA; ++ti) {
        const int         LINE1   = DATA[ti].d_line;
        const char *const SPEC1   = DATA[ti].d_spec;

        bslma::TestAllocator      oa("object",  veryVeryVeryVerbose);
        bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

        Obj mW(&oa);  const Obj& W = gg(&mW,  SPEC1);
        const Obj XX(W, &scratch);

        if (veryVerbose) { T_ P_(LINE1) P_(W) P(XX) }

        // Ensure the first row of the table contains the
        // default-constructed value.

        ASSERTV(LINE1, Obj(), W, 0 != ti || Obj() == W);

        // member 'swap'
        {
            bslma::TestAllocatorMonitor oam(&oa);

            mW.swap(mW);

            ASSERTV(LINE1, XX, W, XX == W);
            ASSERTV(LINE1, &oa == W.get_allocator());
            ASSERTV(LINE1, oam.isTotalSame());
        }

        // free function 'swap'
        {
            bslma::TestAllocatorMonitor oam(&oa);

            swap(mW, mW);

            ASSERTV(LINE1, XX, W, XX == W);
            ASSERTV(LINE1, &oa == W.get_allocator());
            ASSERTV(LINE1, oam.isTotalSame());
        }

        for (int tj = 0; tj < NUM_DATA; ++tj) {
            const int         LINE2   = DATA[tj].d_line;
            const char *const SPEC2   = DATA[tj].d_spec;

            Obj mX(XX, &oa);  const Obj& X = mX;

            Obj mY(&oa);  const Obj& Y = gg(&mY, SPEC2);
            const Obj YY(Y, &scratch);

            mX.max_load_factor(2.0f);
            mY.max_load_factor(3.0f);

            ASSERT(2.0f == X.max_load_factor());
            ASSERT(3.0f == Y.max_load_factor());

            if (veryVerbose) { T_ P_(LINE2) P_(X) P_(Y) P(YY) }

            // member 'swap'
            {
                bslma::TestAllocatorMonitor oam(&oa);

                mX.swap(mY);

                ASSERT(3.0f == X.max_load_factor());
                ASSERT(2.0f == Y.max_load_factor());

                ASSERTV(LINE1, LINE2, YY, X, YY == X);
                ASSERTV(LINE1, LINE2, XX, Y, XX == Y);
                ASSERTV(LINE1, LINE2, &oa == X.get_allocator());
                ASSERTV(LINE1, LINE2, &oa == Y.get_allocator());
                ASSERTV(LINE1, LINE2, oam.isTotalSame());
            }

            // free function 'swap'
            {
                bslma::TestAllocatorMonitor oam(&oa);

                swap(mX, mY);

                ASSERT(2.0f == X.max_load_factor());
                ASSERT(3.0f == Y.max_load_factor());

                ASSERTV(LINE1, LINE2, XX, X, XX == X);
                ASSERTV(LINE1, LINE2, YY, Y, YY == Y);
                ASSERTV(LINE1, LINE2, &oa == X.get_allocator());
                ASSERTV(LINE1, LINE2, &oa == Y.get_allocator());
                ASSERTV(LINE1, LINE2, oam.isTotalSame());
            }

#if 0       // Unlike 'set', 'uordered multimap' does not support swapping with
            // unequal bslma allocators.


            bslma::TestAllocator oaz("z_object", veryVeryVeryVerbose);

            Obj mZ(&oaz);  const Obj& Z = gg(&mZ, SPEC2);
            const Obj ZZ(Z, &scratch);

            if (veryVerbose) { T_ P_(LINE2) P_(X) P_(Y) P(YY) }

            // member 'swap'
            {
                bslma::TestAllocatorMonitor oam(&oa);
                bslma::TestAllocatorMonitor oazm(&oaz);

                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                    ExceptionGuard<Obj> guardX(&X, L_, &scratch);
                    ExceptionGuard<Obj> guardZ(&Z, L_, &scratch);

                    mX.swap(mZ);

                    guardX.release();
                    guardZ.release();
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END


                ASSERTV(LINE1, LINE2, ZZ, X, ZZ == X);
                ASSERTV(LINE1, LINE2, XX, Z, XX == Z);
                ASSERTV(LINE1, LINE2, &oa == X.get_allocator());
                ASSERTV(LINE1, LINE2, &oaz == Z.get_allocator());

                if (0 == X.size()) {
                    ASSERTV(LINE1, LINE2, oam.isTotalSame());
                }
                else {
                    ASSERTV(LINE1, LINE2, oam.isTotalUp());
                }

                if (0 == Z.size()) {
                    ASSERTV(LINE1, LINE2, oazm.isTotalSame());
                }
                else {
                    ASSERTV(LINE1, LINE2, oazm.isTotalUp());
                }
            }

            // free function 'swap'
            {
                bslma::TestAllocatorMonitor oam(&oa);
                bslma::TestAllocatorMonitor oazm(&oaz);

                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                    ExceptionGuard<Obj> guardX(&X, L_, &scratch);
                    ExceptionGuard<Obj> guardZ(&Z, L_, &scratch);

                    swap(mX, mZ);

                    guardX.release();
                    guardZ.release();
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                ASSERTV(LINE1, LINE2, XX, X, XX == X);
                ASSERTV(LINE1, LINE2, ZZ, Z, ZZ == Z);
                ASSERTV(LINE1, LINE2, &oa == X.get_allocator());
                ASSERTV(LINE1, LINE2, &oaz == Z.get_allocator());

                if (0 == X.size()) {
                    ASSERTV(LINE1, LINE2, oam.isTotalSame());
                }
                else {
                    ASSERTV(LINE1, LINE2, oam.isTotalUp());
                }

                if (0 == Z.size()) {
                    ASSERTV(LINE1, LINE2, oazm.isTotalSame());
                }
                else {
                    ASSERTV(LINE1, LINE2, oazm.isTotalUp());
                }
            }
#endif

        }
    }

    if (verbose) printf(
            "\nInvoke free 'swap' function in a context where ADL is used.\n");
    {
        // 'A' values: Should cause memory allocation if possible.

        bslma::TestAllocator      oa("object",  veryVeryVeryVerbose);
        bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

        Obj mX(&oa);  const Obj& X = mX;
        const Obj XX(X, &scratch);

        Obj mY(&oa);  const Obj& Y = gg(&mY, "ABC");
        const Obj YY(Y, &scratch);

        mX.max_load_factor(2.0);
        mY.max_load_factor(3.0);

        if (veryVerbose) { T_ P_(X) P(Y) }

        bslma::TestAllocatorMonitor oam(&oa);

#if 0
    invokeAdlSwap(mX, mY);
#else
        // We know that the types of 'mX' and 'mY' do not overload the unary
        // address-of 'operator&'.

        bslalg::SwapUtil::swap(&mX, &mY);
#endif

        ASSERTV(YY, X, YY == X);
        ASSERTV(XX, Y, XX == Y);
        ASSERT(oam.isTotalSame());

        ASSERT(3.0 == X.max_load_factor());
        ASSERT(2.0 == Y.max_load_factor());

        if (veryVerbose) { T_ P_(X) P(Y) }
    }
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOC>
void TestDriver<KEY, VALUE, HASH, EQUAL, ALLOC>::testCase7()
{
    // ------------------------------------------------------------------------
    // TESTING COPY CONSTRUCTOR:
    //: 1 The new object's value is the same as that of the original object
    //:   (relying on the equality operator) and created with the correct
    //:   capacity.
    //:
    //: 2 All internal representations of a given value can be used to create a
    //:   new object of equivalent value.
    //:
    //: 3 The value of the original object is left unaffected.
    //:
    //: 4 Subsequent changes in or destruction of the source object have no
    //:   effect on the copy-constructed object.
    //:
    //: 5 Subsequent changes ('insert's) on the created object have no
    //:   effect on the original and change the capacity of the new object
    //:   correctly.
    //:
    //: 6 The object has its internal memory management system hooked up
    //:   properly so that *all* internally allocated memory draws from a
    //:   user-supplied allocator whenever one is specified.
    //:
    //: 7 The function is exception neutral w.r.t. memory allocation.
    //
    // Plan:
    //: 1 Specify a set S of object values with substantial and varied
    //:   differences, ordered by increasing length, to be used in the
    //:   following tests.
    //:
    //: 2 For each value in S, initialize objects w and x, copy construct y
    //:   from x and use 'operator==' to verify that both x and y subsequently
    //:   have the same value as w.  Let x go out of scope and again verify
    //:   that w == y.  (C-1..4)
    //:
    //: 3 For each value in S initialize objects w and x, and copy construct y
    //:   from x.  Change the state of y, by using the *primary* *manipulator*
    //:   'push_back'.  Using the 'operator!=' verify that y differs from x and
    //:   w, and verify that the capacity of y changes correctly.  (C-5)
    //:
    //: 4 Perform tests performed as P-2:  (C-6)
    //:   1 While passing a testAllocator as a parameter to the new object and
    //:     ascertaining that the new object gets its memory from the provided
    //:     testAllocator.
    //:   2 Verify neither of global and default allocator is used to supply
    //:     memory.  (C-6)
    //:
    //: 5 Perform tests as P-2 in the presence of exceptions during memory
    //:   allocations using a 'bslma::TestAllocator' and varying its
    //:   *allocation* *limit*.  (C-7)
    //
    // Testing:
    //   unordered_map(const unordered_map& original);
    //   unordered_map(const unordered_map& original, const A& allocator);
    // ------------------------------------------------------------------------

    bslma::TestAllocator oa(veryVeryVerbose);

    const TestValues VALUES;

    if (verbose)
        printf("\nTesting parameters: TYPE = %s.\n", testTypeName<KEY>());
    {
        for (size_t ti = 0; ti < DEFAULT_NUM_DATA; ++ti) {
            const char *const SPEC        = DEFAULT_DATA[ti].d_spec;
            const size_t      LENGTH      = strlen(SPEC);

            if (verbose) {
                printf("\nFor an object of length " ZU ":\n", LENGTH);
                P(SPEC);
            }

            // Create control object w.
            Obj mW; const Obj& W = gg(&mW, SPEC);

            ASSERTV(ti, LENGTH == W.size()); // same lengths
            if (veryVerbose) { printf("\tControl Obj: "); P(W); }

            Obj mX(&oa);  const Obj& X = gg(&mX, SPEC);

            if (veryVerbose) { printf("\t\tDynamic Obj: "); P(X); }

            {   // Testing concern 1..4.

                if (veryVerbose) { printf("\t\t\tRegular Case :"); }

                const Obj Y0(X);

                ASSERTV(SPEC, X == Y0);
                ASSERTV(SPEC, W == Y0);
                ASSERTV(SPEC, Y0.get_allocator() ==
                                           bslma::Default::defaultAllocator());

                ASSERTV(SPEC, W == Y0);
            }
            {   // Testing concern 5.

                if (veryVerbose) printf("\t\t\tInsert into created obj, "
                                        "without test allocator:\n");

                Obj Y1(X);

                if (veryVerbose) {
                    printf("\t\t\t\tBefore Insert: "); P(Y1);
                }

                Y1.insert(VALUES['Z' - 'A']);

                if (veryVerbose) {
                    printf("\t\t\t\tAfter Insert : ");
                    P(Y1);
                }

                ASSERTV(SPEC, Y1.size() == LENGTH + 1);
                ASSERTV(SPEC, W != Y1);
                ASSERTV(SPEC, X != Y1);
            }
            {   // Testing concern 5 with test allocator.
                if (veryVerbose) {
                    printf("\t\t\tInsert into created obj, "
                           "with test allocator:\n");
                }

                const bsls::Types::Int64 A = oa.numBlocksTotal();

                Obj Y11(X, &oa);

                ASSERT(0 == LENGTH || oa.numBlocksTotal() > A);

                // Due of pooling of memory alloctioon, we can't predict
                // whether this insert will allocate or not.

                Y11.insert(VALUES['Z' - 'A']);

                if (veryVerbose) {
                    printf("\t\t\t\tAfter Append : ");
                    P(Y11);
                }

                ASSERTV(SPEC, Y11.size() == LENGTH + 1);
                ASSERTV(SPEC, W != Y11);
                ASSERTV(SPEC, X != Y11);
                ASSERTV(SPEC, Y11.get_allocator() == X.get_allocator());
            }
            {   // Exception checking.

                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                    const Obj Y2(X, &oa);
                    if (veryVerbose) {
                        printf("\t\t\tException Case  :\n");
                        printf("\t\t\t\tObj : "); P(Y2);
                    }
                    ASSERTV(SPEC, W == Y2);
                    ASSERTV(SPEC, W == X);
                    ASSERTV(SPEC, Y2.get_allocator() == X.get_allocator());
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
            }
        }
    }
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOC>
void TestDriver<KEY, VALUE, HASH, EQUAL, ALLOC>::testCase6()
{
    // ------------------------------------------------------------------------
    // TESTING EQUALITY OPERATORS:
    // Concerns:
    //: 1 Two objects, 'X' and 'Y', compare equal if and only if they contain
    //:   the same values.
    //:
    //: 2 No non-salient attributes (i.e., 'allocator') participate.
    //:
    //: 3 'true  == (X == X)' (i.e., identity)
    //:
    //: 4 'false == (X != X)' (i.e., identity)
    //:
    //: 5 'X == Y' if and only if 'Y == X' (i.e., commutativity)
    //:
    //: 6 'X != Y' if and only if 'Y != X' (i.e., commutativity)
    //:
    //: 7 'X != Y' if and only if '!(X == Y)'
    //:
    //: 8 Comparison is symmetric with respect to user-defined conversion
    //:   (i.e., both comparison operators are free functions).
    //:
    //: 9 Non-modifiable objects can be compared (i.e., objects or references
    //:   providing only non-modifiable access).
    //:
    //:10 'operator==' is defined in terms of 'operator==(KEY)' instead of the
    //:   supplied comparator function.
    //:
    //:11 No memory allocation occurs as a result of comparison (e.g., the
    //:   arguments are not passed by value).
    //:
    //:12 The equality operator's signature and return type are standard.
    //:
    //:13 The inequality operator's signature and return type are standard.
    //
    // Plan:
    //: 1 Use the respective addresses of 'operator==' and 'operator!=' to
    //:   initialize function pointers having the appropriate signatures and
    //:   return types for the two homogeneous, free equality- comparison
    //:   operators defined in this component.  (C-8..9, 12..13)
    //:
    //: 2 Create a 'bslma::TestAllocator' object, and install it as the default
    //:   allocator (note that a ubiquitous test allocator is already installed
    //:   as the global allocator).
    //:
    //: 3 Using the table-driven technique, specify a set of distinct
    //:   specifications for the 'gg' function.
    //:
    //: 4 For each row 'R1' in the table of P-3: (C-1..7)
    //:
    //:   1 Create a single object, using a comparator that can be disabled and
    //:     a"scratch" allocator, and use it to verify the reflexive
    //:     (anti-reflexive) property of equality (inequality) in the presence
    //:     of aliasing.  (C-3..4)
    //:
    //:   2 For each row 'R2' in the table of P-3: (C-1..2, 5..7)
    //:
    //:     1 Record, in 'EXP', whether or not distinct objects created from
    //:       'R1' and 'R2', respectively, are expected to have the same value.
    //:
    //:     2 For each of two configurations, 'a' and 'b': (C-1..2, 5..7)
    //:
    //:       1 Create two (object) allocators, 'oax' and 'oay'.
    //:
    //:       2 Create an object 'X', using 'oax', having the value 'R1'.
    //:
    //:       3 Create an object 'Y', using 'oax' in configuration 'a' and
    //:         'oay' in configuration 'b', having the value 'R2'.
    //:
    //:       4 Disable the comparator so that it will cause an error if it's
    //:         used.
    //:
    //:       5 Verify the commutativity property and expected return value for
    //:         both '==' and '!=', while monitoring both 'oax' and 'oay' to
    //:         ensure that no object memory is ever allocated by either
    //:         operator.  (C-1..2, 5..7, 10)
    //:
    //:       6 Verify that modifying the 'second' fields of the pair affects
    //:         the result -- when X == Y, go through the elements of X,
    //:         perterbing the 'second' field of every element, and verify
    //:         that even this small change causing an inequality result.
    //:
    //: 5 Use the test allocator from P-2 to verify that no memory is ever
    //:   allocated from the default allocator.  (C-11)
    //
    // Testing:
    //   bool operator==(const unordered_map<K, V, H, E, A>& lhs,
    //                   const unordered_map<K, V, H, E, A>& rhs);
    //   bool operator!=(const unordered_map<K, V, H, E, A>& lhs,
    //                   const unordered_map<K, V, H, E, A>& rhs);
    // ------------------------------------------------------------------------

    if (veryVerbose) printf("EQUALITY-COMPARISON OPERATORS\n"
                            "=============================\n");

    if (veryVerbose)
              printf("\nAssign the address of each operator to a variable.\n");
    {
        typedef bool (*OP)(const Obj&, const Obj&);

        // Verify that the signatures and return types are standard.

        OP op = bsl::operator==;
        (void) op;
        op    = bsl::operator!=;
        (void) op;
    }

    const int NUM_DATA                     = DEFAULT_NUM_DATA;
    const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

    const TestValues VALUES;

    if (verbose) printf("\nCompare every value with every value.\n");
    {
        // Create first object
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE1   = DATA[ti].d_line;
            const char *const SPEC1   = DATA[ti].d_spec;
            const size_t      LENGTH1 = strlen(SPEC1);

            if (veryVerbose) { T_ P_(LINE1) P_(LENGTH1) P(SPEC1) }

            // Ensure an object compares correctly with itself (alias test).
            {
                bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

                Obj mX(&scratch); const Obj& X = gg(&mX, SPEC1);

                ASSERTV(LINE1, X,   X == X);
                ASSERTV(LINE1, X, !(X != X));
            }

            for (int tj = 0; tj < NUM_DATA; ++tj) {
                const int         LINE2   = DATA[tj].d_line;
                const char *const SPEC2   = DATA[tj].d_spec;
                const size_t      LENGTH2 = strlen(SPEC2);

                if (veryVerbose) { T_ T_ P_(LINE2) P_(LENGTH2) P(SPEC2) }

                const bool EXP = (ti == tj);  // expected result

                for (char cfg = 'a'; cfg <= 'b'; ++cfg) {

                    const char CONFIG = cfg;  // Determines 'Y's allocator.

                    // Create two distinct test allocators, 'oax' and 'oay'.

                    bslma::TestAllocator oax("objectx", veryVeryVeryVerbose);
                    bslma::TestAllocator oay("objecty", veryVeryVeryVerbose);

                    // Map allocators above to objects 'X' and 'Y' below.

                    bslma::TestAllocator& xa = oax;
                    bslma::TestAllocator& ya = 'a' == CONFIG ? oax : oay;

                    Obj mX(&xa); const Obj& X = gg(&mX, SPEC1);
                    Obj mY(&ya); const Obj& Y = gg(&mY, SPEC2);

                    ASSERTV(LINE1, LINE2, CONFIG, LENGTH1 == X.size());
                    ASSERTV(LINE1, LINE2, CONFIG, LENGTH2 == Y.size());

                    if (veryVerbose) { T_ T_ P_(X) P(Y); }

                    // Verify value, commutativity, and no memory allocation.

                    bsls::Types::Int64 numX = xa.numBlocksTotal();
                    bsls::Types::Int64 numY = ya.numBlocksTotal();

                    // EQUAL::disableFunctor();
                            // TBD -- fails this test EQUAL is used to
                            // determine the equality groups, then
                            // bsl::permutation is used to determine if they're
                            // isomorphic, and bsl::permutation uses
                            // 'operator=='.  It will take a lot of work to
                            // verify that this is the case, putting it off for
                            // later.

                    ASSERTV(LINE1, LINE2, CONFIG,  EXP == (X == Y));
                    ASSERTV(LINE1, LINE2, CONFIG,  EXP == (Y == X));

                    ASSERTV(LINE1, LINE2, CONFIG, !EXP == (X != Y));
                    ASSERTV(LINE1, LINE2, CONFIG, !EXP == (Y != X));

                    ASSERTV(xa.numBlocksTotal() == numX);
                    ASSERTV(ya.numBlocksTotal() == numY);

                    if (EXP) {
                        const Iter end = mX.end();
                        for (Iter it = mX.begin(); end != it; ++it) {
                            VALUE v = it->second;
                            typedef bsltf::TemplateTestFacility TTF;
                            int id = TTF::getIdentifier(it->second);
                            ++id;
                            it->second = TTF::create<VALUE>(id);

                            numX = xa.numBlocksTotal();
                            numY = ya.numBlocksTotal();

                            ASSERTV(LINE1, LINE2, !(X == Y));
                            ASSERTV(LINE1, LINE2, !(Y == X));
                            ASSERTV(LINE1, LINE2,   X != Y);
                            ASSERTV(LINE1, LINE2,   Y != X);

                            ASSERTV(xa.numBlocksTotal() == numX);
                            ASSERTV(ya.numBlocksTotal() == numY);

                            it->second = v;

                            numX = xa.numBlocksTotal();
                            numY = ya.numBlocksTotal();

                            ASSERTV(LINE1, LINE2,   X == Y);
                            ASSERTV(LINE1, LINE2,   Y == X);
                            ASSERTV(LINE1, LINE2, !(X != Y));
                            ASSERTV(LINE1, LINE2, !(Y != X));

                            ASSERTV(xa.numBlocksTotal() == numX);
                            ASSERTV(ya.numBlocksTotal() == numY);
                        }
                    }

                    const size_t NUM_BUCKETS = Y.bucket_count();
                    mY.reserve((Y.size() + 1) * 5);
                    ASSERTV(NUM_BUCKETS < Y.bucket_count());
                    ASSERTV(!EXP || X.bucket_count() != Y.bucket_count());

                    numX = xa.numBlocksTotal();
                    numY = ya.numBlocksTotal();

                    ASSERTV(LINE1, LINE2, CONFIG,  EXP == (X == Y));
                    ASSERTV(LINE1, LINE2, CONFIG,  EXP == (Y == X));

                    ASSERTV(LINE1, LINE2, CONFIG, !EXP == (X != Y));
                    ASSERTV(LINE1, LINE2, CONFIG, !EXP == (Y != X));

//                  EQUAL::enableFunctor();

                    ASSERTV(xa.numBlocksTotal() == numX);
                    ASSERTV(ya.numBlocksTotal() == numY);

                    if (EXP) {
                        const Iter end = mX.end();
                        for (Iter it = mX.begin(); end != it; ++it) {
                            VALUE v = it->second;
                            typedef bsltf::TemplateTestFacility TTF;
                            int id = TTF::getIdentifier(it->second);
                            ++id;
                            it->second = TTF::create<VALUE>(id);

                            numX = xa.numBlocksTotal();
                            numY = ya.numBlocksTotal();

                            ASSERTV(LINE1, LINE2, !(X == Y));
                            ASSERTV(LINE1, LINE2, !(Y == X));
                            ASSERTV(LINE1, LINE2,   X != Y);
                            ASSERTV(LINE1, LINE2,   Y != X);

                            ASSERTV(xa.numBlocksTotal() == numX);
                            ASSERTV(ya.numBlocksTotal() == numY);

                            it->second = v;

                            numX = xa.numBlocksTotal();
                            numY = ya.numBlocksTotal();

                            ASSERTV(LINE1, LINE2,   X == Y);
                            ASSERTV(LINE1, LINE2,   Y == X);
                            ASSERTV(LINE1, LINE2, !(X != Y));
                            ASSERTV(LINE1, LINE2, !(Y != X));

                            ASSERTV(xa.numBlocksTotal() == numX);
                            ASSERTV(ya.numBlocksTotal() == numY);
                        }
                    }
                }
            }
        }
    }
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOC>
void TestDriver<KEY, VALUE, HASH, EQUAL, ALLOC>::testCase4()
{
    // ------------------------------------------------------------------------
    // BASIC ACCESSORS
    // ------------------------------------------------------------------------

    if (veryVerbose) P(testTypeName<KEY>());

    BSLMF_ASSERT((! bslmf::IsSame<Iter, CIter>::value));

    {
        typedef bool (Obj::*MP)() const;
        MP mp = &Obj::empty;
        (void) mp;
    }

    {
        typedef size_t (Obj::*MP)() const;
        MP mp = &Obj::size;
        (void) mp;
        mp    = &Obj::max_size;
        (void) mp;
    }

    {
        typedef Iter (Obj::*MP)();
        MP mp = &Obj::begin;
        (void) mp;
        mp    = &Obj::end;
        (void) mp;
    }

    {
        typedef CIter (Obj::*MP)() const;
        MP mp = &Obj::begin;
        (void) mp;
        mp    = &Obj::end;
        (void) mp;
        mp    = &Obj::cbegin;
        (void) mp;
        mp    = &Obj::cend;
        (void) mp;
    }

    {
        typedef Iter (Obj::*MP)(const KEY&);
        MP mp = &Obj::find;
        (void) mp;
    }

    {
        typedef CIter (Obj::*MP)(const KEY&) const;
        MP mp = &Obj::find;
        (void) mp;
    }

    {
        typedef size_t (Obj::*MP)(const KEY&) const;
        MP mp = &Obj::count;
        (void) mp;
    }

    {
        typedef bsl::pair<Iter, Iter> (Obj::*MP)(const KEY&);
        MP mp = &Obj::equal_range;
        (void) mp;
    }

    {
        typedef bsl::pair<CIter, CIter> (Obj::*MP)(const KEY&) const;
        MP mp = &Obj::equal_range;
        (void) mp;
    }

    {
        typedef size_t (Obj::*MP)() const;
        MP mp = &Obj::bucket_count;
        (void) mp;
        mp    = &Obj::max_bucket_count;
        (void) mp;
    }

    {
        typedef size_t (Obj::*MP)(size_t) const;
        MP mp = &Obj::bucket_size;
        (void) mp;
    }

    {
        typedef size_t (Obj::*MP)(const KEY&) const;
        MP mp = &Obj::bucket;
        (void) mp;
    }

    {
        typedef LIter (Obj::*MP)(size_t);
        MP mp = &Obj::begin;
        (void) mp;
        mp    = &Obj::end;
        (void) mp;
    }

    {
        typedef CLIter (Obj::*MP)(size_t) const;
        MP mp = &Obj::begin;
        (void) mp;
        mp    = &Obj::end;
        (void) mp;
        mp    = &Obj::cbegin;
        (void) mp;
        mp    = &Obj::cend;
        (void) mp;
    }

    TestValues values;    const TestValues& VALUES = values;

    for (size_t ti = 0; ti < DEFAULT_NUM_DATA; ++ti) {
        const DefaultDataRow *pd   = DEFAULT_DATA + ti;
        const char *SPEC           = pd->d_spec;
        const size_t LENGTH        = strlen(SPEC);

        bslma::TestAllocator da("default",  veryVeryVeryVerbose);
        bslma::TestAllocator sc("scratch",  veryVeryVeryVerbose);
        bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);

        bslma::DefaultAllocatorGuard dag(&da);

        Obj mX(&sa);    const Obj& X = mX;

        gg(&mX, SPEC);

        bslma::TestAllocatorMonitor dam(&da);
        bslma::TestAllocatorMonitor sam(&sa);

        ASSERTV(verifySpec(X, SPEC));

        ASSERTV((0 == LENGTH) == X.empty());
        ASSERTV(LENGTH == X.size());
        ASSERTV((size_t) -1 >= X.max_size());   // TBD: have more
                                                // sophisiticated value for
                                                // max_size.

        const Iter begin = mX.begin();
        CIter cBegin = X.begin();
        ASSERTV(begin == cBegin);
        cBegin = X.cbegin();
        ASSERTV(begin == cBegin);
        const Iter end = mX.end();
        CIter cend = X.end();
        ASSERTV(end == cend);
        cend = X.end();
        ASSERTV(end == cend);

        ASSERTV(!LENGTH == (begin == end));

        {
            size_t count = 0;
            for (Iter it = begin; end != it; ++it) {
                ++count;
            }
            ASSERTV(LENGTH == count);
        }

        ASSERTV(dam.isTotalSame());

        for (char c = 'A'; c <= 'Z'; ++c) {
            const char C = c;

            const size_t EXP = numCharInstances(SPEC, C);

            const Pair&  P = VALUES[C - 'A'];
            const KEY&   K = P.first;

            Iter it = mX.find(K);
            ASSERTV(!!EXP == (mX.end() != it));
            if (EXP) {
                ASSERTV(*it == P);
            }

            CIter cit = X.find(K);
            ASSERTV(!!EXP == (mX.end() != cit));
            if (EXP) {
                ASSERTV(*cit == P);
            }

            ASSERTV(EXP == X.count(K));

            const bsl::pair<Iter, Iter>& EQR = mX.equal_range(K);
            ASSERTV(EQR.first == it);
            for (size_t tj = 0; tj < EXP; ++tj) {
                ASSERTV(mX.end() != it);
                ++it;
            }
            ASSERTV(EQR.second == it);

            const bsl::pair<CIter, CIter>& CEQR = X.equal_range(K);
            ASSERTV(CEQR.first == cit);
            for (size_t tj = 0; tj < EXP; ++tj) {
                ASSERTV(X.end() != cit);
                ++cit;
            }
            ASSERTV(CEQR.second == cit);
        }

        ASSERTV((size_t) -1 >= X.max_bucket_count());   // TBD: make more
                                                        // sophisticated.
        const size_t BC = X.bucket_count();

        ASSERTV(BC >= 1);
        ASSERTV(!LENGTH == X.empty());
        ASSERTV(X.empty() == (1 == BC));

        for (size_t tj = 0; tj < BC; ++tj) {
            BSLMF_ASSERT((! bslmf::IsSame<LIter, CLIter>::value));

            LIter bBegin   = mX.begin(tj);
            CLIter cbBegin =  X.begin(tj);
            ASSERTV(bBegin  == cbBegin);
            cbBegin = X.cbegin(tj);
            ASSERTV(bBegin  == cbBegin);
            LIter bEnd     = mX.end(tj);
            CLIter cbEnd   =  X.end(tj);
            ASSERTV(bEnd == cbEnd);
            cbEnd = X.cend(tj);
            ASSERTV(bEnd == cbEnd);

            {
                size_t count = 0;
                for (CLIter lit = cbBegin; cbEnd != lit; ++lit) {
                    ASSERTV(X.bucket(lit->first) == tj);

                    ++count;
                }
                ASSERTV(X.bucket_size(tj) == count);
            }
        }

        ASSERTV(sam.isTotalSame());

        // Make sure objects can be modified through 'Iter' and 'LIter'.

        ASSERTV(verifySpec(X, SPEC, false));   // Looking at mapped values too

        for (Iter it = begin; end != it; ++it) {
            Pair  p = *it;
            VALUE v = it->second;
            ASSERTV(it->first != it->second);
            it->second = it->first;
            ASSERTV(it->first == it->second);
            ASSERTV(it->second != v);
            ASSERTV(*it != p);
        }

        ASSERTV( verifySpec(X, SPEC, true));          // Keys only
        ASSERTV(!LENGTH || !verifySpec(X, SPEC, false));   // mapped values too

        size_t count = 0;
        for (size_t tj = 0; tj < BC; ++tj) {
            LIter bEnd = mX.end(tj);
            for (LIter it = mX.begin(tj); bEnd != it; ++it, ++count) {
                ASSERTV(it->first == it->second);
                int id = bsltf::TemplateTestFacility::getIdentifier(it->first);
                int idB = id - 'A' + '0';
                it->second = bsltf::TemplateTestFacility::create<VALUE>(idB);
                ASSERTV(VALUES[id - 'A'].second == it->second);
                ASSERTV(it->first != it->second);
            }
        }
        ASSERTV(LENGTH == count);

        ASSERTV(verifySpec(X, SPEC, true));    // Keys only
        ASSERTV(verifySpec(X, SPEC, false));   // Looking at mapped values too
    }
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOC>
void TestDriver<KEY, VALUE, HASH, EQUAL, ALLOC>::testCase3()
{
    // ------------------------------------------------------------------------
    // Range c'tor, 'ggg', 'gg', and 'verifySpec' functions.
    // ------------------------------------------------------------------------

    HASH  h( 7);
    EQUAL eq(9);

    for (size_t ti = 0; ti < DEFAULT_NUM_DATA; ++ti) {
        const DefaultDataRow *pd     = DEFAULT_DATA + ti;
        const char           *SPEC   = pd->d_spec;
        const size_t          LENGTH = strlen(SPEC);

        // Verify indexes work.

        bslma::TestAllocator da("default",  veryVeryVeryVerbose);
        bslma::TestAllocator sc("scratch",  veryVeryVeryVerbose);
        bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);

        bslma::DefaultAllocatorGuard dag(&da);

        TestValues src(SPEC, &sc);

        {
            Obj mX(src.begin(), src.end());    const Obj& X = mX;
            src.resetIterators();

            ASSERTV(LENGTH == X.size());

            ASSERTV(!!LENGTH == !!da.numBlocksTotal());
            ASSERTV(!!LENGTH == !!da.numBlocksInUse());

            ASSERTV(verifySpec(X, SPEC));

            for (size_t tj = 0; tj < DEFAULT_NUM_DATA; ++tj) {
                const DefaultDataRow *pdj = DEFAULT_DATA + tj;
                const bool MATCH = (ti == tj);

                ASSERTV(MATCH, SPEC, pdj->d_spec, X.size(),
                                          MATCH == verifySpec(X, pdj->d_spec));
            }
        }

        {
            Obj mX(src.begin(), src.end(), 0, h);    const Obj& X = mX;
            src.resetIterators();

            ASSERTV(LENGTH == X.size());

            ASSERTV(!!LENGTH == !!da.numBlocksTotal());
            ASSERTV(!!LENGTH == !!da.numBlocksInUse());

            ASSERTV(verifySpec(X, SPEC));

            for (size_t tj = 0; tj < DEFAULT_NUM_DATA; ++tj) {
                const DefaultDataRow *pdj = DEFAULT_DATA + tj;
                const bool MATCH = (ti == tj);

                ASSERTV(MATCH, SPEC, pdj->d_spec, X.size(),
                                          MATCH == verifySpec(X, pdj->d_spec));
            }
        }

        {
            Obj mX(src.begin(), src.end(), 0);    const Obj& X = mX;
            src.resetIterators();

            ASSERTV(LENGTH == X.size());

            ASSERTV(!!LENGTH == !!da.numBlocksTotal());
            ASSERTV(!!LENGTH == !!da.numBlocksInUse());

            ASSERTV(verifySpec(X, SPEC));

            for (size_t tj = 0; tj < DEFAULT_NUM_DATA; ++tj) {
                const DefaultDataRow *pdj = DEFAULT_DATA + tj;
                const bool MATCH = (ti == tj);

                ASSERTV(MATCH, SPEC, pdj->d_spec, X.size(),
                                          MATCH == verifySpec(X, pdj->d_spec));
            }
        }

        {
            Obj mX(src.begin(), src.end(), 0, h, eq);    const Obj& X = mX;
            src.resetIterators();

            ASSERTV(LENGTH == X.size());

            ASSERTV(!!LENGTH == !!da.numBlocksTotal());
            ASSERTV(!!LENGTH == !!da.numBlocksInUse());

            ASSERTV(verifySpec(X, SPEC));

            for (size_t tj = 0; tj < DEFAULT_NUM_DATA; ++tj) {
                const DefaultDataRow *pdj = DEFAULT_DATA + tj;
                const bool MATCH = (ti == tj);

                ASSERTV(MATCH, SPEC, pdj->d_spec, X.size(),
                                          MATCH == verifySpec(X, pdj->d_spec));
            }
        }

        {
            Obj mX(src.begin(), src.end(), 0, h, eq, 0);    const Obj& X = mX;
            src.resetIterators();

            ASSERTV(LENGTH == X.size());

            ASSERTV(!!LENGTH == !!da.numBlocksTotal());
            ASSERTV(!!LENGTH == !!da.numBlocksInUse());

            ASSERTV(verifySpec(X, SPEC));

            for (size_t tj = 0; tj < DEFAULT_NUM_DATA; ++tj) {
                const DefaultDataRow *pdj = DEFAULT_DATA + tj;
                const bool MATCH = (ti == tj);

                ASSERTV(MATCH, SPEC, pdj->d_spec, X.size(),
                                          MATCH == verifySpec(X, pdj->d_spec));
            }
        }

        ASSERTV(0 == sa.numBlocksInUse());

        {
            Obj mX(&sa);    const Obj& X = mX;

            ASSERTV(-1 == ggg(&mX, SPEC));

            ASSERTV(0        ==   da.numBlocksInUse());
            ASSERTV(!!LENGTH == !!sa.numBlocksInUse());

            ASSERTV(LENGTH == X.size());

            ASSERTV(verifySpec(X, SPEC));

            for (size_t tj = 0; tj < DEFAULT_NUM_DATA; ++tj) {
                const DefaultDataRow *pdj = DEFAULT_DATA + tj;
                const bool MATCH = (ti == tj);

                ASSERTV(MATCH, SPEC, pdj->d_spec, X.size(),
                                          MATCH == verifySpec(X, pdj->d_spec));
            }

            mX.clear();

            ASSERTV(0 == X.size());

            const Obj& XX = gg(&mX, SPEC);

            ASSERTV(&XX == &X);

            ASSERTV(0        ==   da.numBlocksInUse());
            ASSERTV(!!LENGTH == !!sa.numBlocksInUse());

            ASSERTV(LENGTH == X.size());

            ASSERTV(verifySpec(X, SPEC));
        }
    }

    struct {
        int d_line;
        const char *d_spec;
        const char *d_goodSpec;
    } BAD_SPECS[] = {
        { L_, " ",    "" },
        { L_, "+",    "" },
        { L_, "ABCa", "ABC" },
        { L_, "ABC+", "ABC" },
        { L_, "A+BC", "ABC" },
        { L_, "A,+C", "AC" },
        { L_, "A1BC", "ABC" },
        { L_, "1234", "" } };
    enum { NUM_BAD_SPECS = sizeof BAD_SPECS / sizeof *BAD_SPECS };

    bslma::TestAllocator da("default",  veryVeryVeryVerbose);
    bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);

    bslma::DefaultAllocatorGuard dag(&da);

    for (size_t ti = 0; ti < NUM_BAD_SPECS; ++ti) {
        const char *SPEC      = BAD_SPECS[ti].d_spec;
        const char *GOOD_SPEC = BAD_SPECS[ti].d_goodSpec;

        {
            Obj mX(&sa);    const Obj& X = mX;

            ASSERTV(-1 != ggg(&mX, SPEC, 0));
            ASSERTV(!verifySpec(X, SPEC));
        }

        {
            Obj mX(&sa);    const Obj& X = mX;
            gg(&mX, GOOD_SPEC);

            ASSERTV( verifySpec(X, GOOD_SPEC));
            ASSERTV(!verifySpec(X, SPEC));
        }
    }
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOC>
void TestDriver<KEY, VALUE, HASH, EQUAL, ALLOC>::testCase2()
{
    // ------------------------------------------------------------------------
    // TESTING PRIMARY MANIPULATORS (BOOTSTRAP):
    //   The basic concern is that the default constructor, the destructor,
    //   and, under normal conditions (i.e., no aliasing), the primary
    //   manipulators
    //      - insert
    //      - clear
    //      - erase
    //      - operator[]
    //
    // Concerns:
    //: 1 An object created with the default constructor (with or without a
    //:   supplied allocator) has the contractually specified default value.
    //:
    //: 2 If an allocator is NOT supplied to the default constructor, the
    //:   default allocator in effect at the time of construction becomes the
    //:   object allocator for the resulting object.
    //:
    //: 3 If an allocator IS supplied to the default constructor, that
    //:   allocator becomes the object allocator for the resulting object.
    //:
    //: 4 Supplying a null allocator address has the same effect as not
    //:   supplying an allocator.
    //:
    //: 5 Supplying an allocator to the default constructor has no effect on
    //:   subsequent object values.
    //:
    //: 6 Any memory allocation is from the object allocator.
    //:
    //: 7 There is no temporary allocation from any allocator.
    //:
    //: 8 Every object releases any allocated memory at destruction.
    //:
    //: 9 QoI: The default constructor allocates no memory.
    //:
    //:10 'insert' adds an additional element to the object if the element
    //:   being inserted does not already exist.
    //:
    //:11 'insert' returns a pair with an iterator of the element that was just
    //:   inserted or the element that already exist in the object, and a
    //:   boolean indicating whether element being inserted already exist in
    //:   the object.
    //:
    //:12 'clear' properly destroys each contained element value.
    //:
    //:13 'clear' does not allocate memory.
    //:
    //:14 Any argument can be 'const'.
    //:
    //:15 Any memory allocation is exception neutral.
    //
    // Plan:
    //: 1 For each value of increasing length, 'L':
    //:
    //:   2 Using a loop-based approach, default-construct three distinct
    //:     objects, in turn, but configured differently: (a) without passing
    //:     an allocator, (b) passing a null allocator address explicitly, and
    //:     (c) passing the address of a test allocator distinct from the
    //:     default.  For each of these three iterations: (C-1..14)
    //:
    //:     1 Create three 'bslma::TestAllocator' objects, and install one as
    //:       as the current default allocator (note that a ubiquitous test
    //:       allocator is already installed as the global allocator).
    //:
    //:     2 Use the default constructor to dynamically create an object
    //:       'X', with its object allocator configured appropriately (see
    //:       P-2); use a distinct test allocator for the object's footprint.
    //:
    //:     3 Use the (as yet unproven) 'get_allocator' to ensure that its
    //:       object allocator is properly installed.  (C-2..4)
    //:
    //:     4 Use the appropriate test allocators to verify that no memory is
    //:       allocated by the default constructor.  (C-9)
    //:
    //:     5 Use the individual (as yet unproven) salient attribute accessors
    //:       to verify the default-constructed value.  (C-1)
    //:
    //:     6 Insert 'L - 1' elements in order of increasing value into the
    //:       container.
    //:
    //:     7 Insert the 'L'th value in the presense of exception and use the
    //:       (as yet unproven) basic accessors to verify the container has the
    //:       expected values.  Verify the number of allocation is as expected.
    //:       (C-5..6, 13..14)
    //:
    //:     8 Verify that no temporary memory is allocated from the object
    //:       allocator.  (C-7)
    //:
    //:     9 Invoke 'clear' and verify that the container is empty.  Verify
    //:       that no memory is allocated.  (C-11..12)
    //:
    //:    10 Verify that all object memory is released when the object is
    //:       destroyed.  (C-8)
    //
    // Testing:
    //   default construction (only)
    //   unordered_map(const allocator_type&);  // bslma::Allocator* only
    //   ~unordered_map();
    //   iterator insert(const value_type& value);
    //   void clear();
    // ------------------------------------------------------------------------

    // pair<Iter, bool> emplace(Arg&& ... args);    // N/A C++11
    // Iter emplace_hint(CIter, Arg&& ... args);    // N/A C++11

    {
        typedef Iter (Obj::*MP)(const Pair&);
        MP mp = &Obj::insert;
        (void) mp;
    }

    // template <class P> pair<Iter, bool> insert(P&&);    // N/A C++11

    {
        typedef Iter (Obj::*MP)(CIter, const Pair&);
        MP mp = &Obj::insert;
        (void) mp;
    }

    // template <class P> Iter insert(CIter, P&&);    // N/A C++11

    {
        typedef typename TestValues::iterator TVIter;
        typedef void (Obj::*MP)(TVIter, TVIter);
        MP mp = &Obj::insert;
        (void) mp;
    }

    {
        typedef Iter (Obj::*MP)(CIter);
        MP mp = &Obj::erase;
        (void) mp;
    }

    {
        typedef size_t (Obj::*MP)(const KEY&);
        MP mp = &Obj::erase;
        (void) mp;
    }

    {
        typedef Iter (Obj::*MP)(CIter);
        MP mp = &Obj::erase;
        (void) mp;
    }

    {
        typedef Iter (Obj::*MP)(CIter, CIter);
        MP mp = &Obj::erase;
        (void) mp;
    }

    {
        typedef void (Obj::*MP)();
        MP mp = &Obj::clear;
        (void) mp;
    }

    {
        typedef HASH (Obj::*MP)() const;
        MP mp = &Obj::hash_function;
        (void) mp;
    }

    {
        typedef EQUAL (Obj::*MP)() const;
        MP mp = &Obj::key_eq;
        (void) mp;
    }

    const bool VALUE_TYPE_USES_ALLOC = bslma::UsesBslmaAllocator<Pair>::value;

    if (verbose) { P(VALUE_TYPE_USES_ALLOC); }

    TestValues values;    const TestValues& VALUES = values;
                                      // contains 52 distinct increasing values

    const size_t MAX_LENGTH = 9;

    HASH  tstHash(7);
    EQUAL tstEqual(9);

    ASSERTV(!(HASH()  == tstHash));
    ASSERTV(!(EQUAL() == tstEqual));

    for (size_t ti = 0; ti < MAX_LENGTH; ++ti) {
        const size_t LENGTH = ti;

        if (verbose) {
            printf("\nTesting with various allocator configurations.\n");
        }
        int done = 0;
        for (char cfg = 'a'; cfg <= 'm'; ++cfg) {
            const char CONFIG = cfg;  // how we specify the allocator

            bslma::TestAllocator da("default",   veryVeryVeryVerbose);
            bslma::TestAllocator fa("footprint", veryVeryVeryVerbose);
            bslma::TestAllocator sa("supplied",  veryVeryVeryVerbose);

            bslma::DefaultAllocatorGuard dag(&da);

            // ----------------------------------------------------------------

            if (veryVerbose) {
                printf("\n\tTesting default constructor.\n");
            }

            bslma::TestAllocator&  oa = strchr("chm", CONFIG) ? sa : da;
            bslma::TestAllocator& noa = &da == &oa            ? sa : da;

            Obj                  *objPtr;

            const bool EXP_ONE_BUCKET = CONFIG < 'i';

            int numPasses = 0;
            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                ++numPasses;

                bslma::Allocator * const pNull = 0;
                switch (CONFIG) {
                  case 'a': {
                      objPtr = new (fa) Obj;
                  } break;
                  case 'b': {
                      objPtr = new (fa) Obj(pNull);
                  } break;
                  case 'c': {
                      objPtr = new (fa) Obj(&sa);
                  } break;
                  case 'd': {
                      objPtr = new (fa) Obj((size_t) 0);
                  } break;
                  case 'e': {
                      objPtr = new (fa) Obj(0, tstHash);
                  } break;
                  case 'f': {
                      objPtr = new (fa) Obj(0, tstHash, tstEqual);
                  } break;
                  case 'g': {
                      objPtr = new (fa) Obj(0, tstHash, tstEqual, pNull);
                  } break;
                  case 'h': {
                      objPtr = new (fa) Obj(0, tstHash, tstEqual, &sa);
                  } break;
                  case 'i': {
                      objPtr = new (fa) Obj(100);
                  } break;
                  case 'j': {
                      objPtr = new (fa) Obj(100, tstHash);
                  } break;
                  case 'k': {
                      objPtr = new (fa) Obj(100, tstHash, tstEqual);
                  } break;
                  case 'l': {
                      objPtr = new (fa) Obj(100, tstHash, tstEqual, pNull);
                  } break;
                  case 'm': {
                      objPtr = new (fa) Obj(100, tstHash, tstEqual, &sa);
                      ++done;
                  } break;
                  default: {
                      ASSERTV(CONFIG, !"Bad allocator config.");
                      return;                                         // RETURN
                  } break;
                }
            } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
            ASSERTV(CONFIG, numPasses,
                            (!PLAT_EXC || EXP_ONE_BUCKET) == (1 == numPasses));

            Obj& mX = *objPtr;  const Obj& X = mX;

            // Verify any attribute allocators are installed properly.

            ASSERTV(LENGTH, CONFIG, &oa == X.get_allocator());

            // Verify no allocation from the object/non-object allocators.
            // NOTE THAT THIS QoI TEST IS STILL AN OPEN DESIGN ISSUE

            ASSERTV(CONFIG, oa.numBlocksTotal(),
                                       EXP_ONE_BUCKET == !oa.numBlocksTotal());
            ASSERTV(CONFIG, noa.numBlocksTotal(), 0 == noa.numBlocksTotal());
            ASSERTV(CONFIG, 0 == X.size());
            ASSERTV(CONFIG, X.cbegin() == X.cend());

            // If default constructed, only the the static bucket is present.
            // QOI test that exactly 1 bucket is present.

            ASSERTV(CONFIG, X.bucket_count(),
                                     EXP_ONE_BUCKET ? 1 == X.bucket_count()
                                                    : X.bucket_count() >= 100);

            ASSERTV(CONFIG,(strchr("efghjklm", CONFIG) ? tstHash  : HASH())
                                                         == X.hash_function());
            ASSERTV(CONFIG,(strchr("fghklm",   CONFIG) ? tstEqual : EQUAL())
                                                         == X.key_eq());

            // ----------------------------------------------------------------

            if (veryVerbose) { printf("\n\tTesting 'insert' (bootstrap).\n"); }

            if (0 < LENGTH) {
                if (verbose) printf(
                       "\t\tOn an object of initial length " ZU ".\n", LENGTH);

                for (size_t tj = 0; tj < LENGTH; ++tj) {
                    if (veryVeryVeryVerbose) {
                       printf("\t\t\t\t Inserting: ");
                       P(VALUES[tj]);
                    }

                    // Sometimes test using hints that are valid iterators
                    // but not very useful.  Also note that some configurations
                    // will have already allocated an initially large enough
                    // bucket array.

                    const int MIN_PASSES = 1 + (CONFIG < 'i')
                                             + VALUE_TYPE_USES_ALLOC;
                    Iter result;
                    numPasses = 0;
                    EXCEPTION_TEST_BEGIN(mX) {
                        ++numPasses;

                        if (1 & tj) {
                            result = mX.insert(X.cbegin(), VALUES[tj]);
                        }
                        else {
                            result = mX.insert(            VALUES[tj]);
                        }
                    } EXCEPTION_TEST_END
                    ASSERTV(PLAT_EXC, tj, numPasses,
                            !PLAT_EXC || 0 != tj || numPasses > MIN_PASSES);
                    ASSERTV(LENGTH, tj, CONFIG, VALUES[tj] == *result);
                }

                ASSERTV(LENGTH, CONFIG, LENGTH == X.size());

                if(veryVerbose) {
                    printf("\t\t Testing allocator exceptions\n");
                }
                bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

                ASSERTV(LENGTH, matchFirstValues(X, VALUES, LENGTH));

                // Verify behavior when element already exist in the object

                if(veryVerbose) {
                    printf("\t\t Verifying already inserted values\n");
                }
                for (size_t tj = 0; tj < LENGTH; ++tj) {
                    if (veryVeryVeryVerbose) {
                       printf("\t\t\t\t Inserting: ");
                       P(VALUES[tj]);
                    }

                    EXCEPTION_TEST_BEGIN(mX) {
                        Iter RESULT = mX.insert(VALUES[tj]);
                        ASSERTV(LENGTH, tj, CONFIG, *RESULT, VALUES[tj] ==
                                                                      *RESULT);
                    } EXCEPTION_TEST_END
                }

                ASSERTV(2 * LENGTH == X.size());
            }

            // ----------------------------------------------------------------

            if (veryVerbose) printf("\n\tTesting 'clear'.\n");
            {
                const bsls::Types::Int64 BB = oa.numBlocksTotal();
//                const bsls::Types::Int64 B  = oa.numBlocksInUse();

                if (veryVeryVeryVerbose) printf("mX.clear();\n");
                numPasses = 0;
                EXCEPTION_TEST_BEGIN(mX) {
                    ++numPasses;

                    mX.clear();
                } EXCEPTION_TEST_END
                ASSERTV(1 == numPasses);

                ASSERTV(LENGTH, CONFIG, 0 == X.size());
                ASSERTV(LENGTH, CONFIG, X.cbegin() == X.cend());

                const bsls::Types::Int64 AA = oa.numBlocksTotal();
//                const bsls::Types::Int64 A  = oa.numBlocksInUse();

                ASSERTV(LENGTH, CONFIG, BB == AA);
//                ASSERTV(LENGTH, CONFIG, B, A,
//                        B - (int)LENGTH * TYPE_ALLOC == A);

                for (size_t tj = 0; tj < LENGTH; ++tj) {
                    Iter RESULT = mX.insert(VALUES[tj]);
                    ASSERTV(LENGTH, tj, CONFIG, VALUES[tj] == *RESULT);
                }

                ASSERTV(matchFirstValues(X, VALUES, LENGTH));
            }

            // ----------------------------------------------------------------

            if (veryVerbose) printf("\n\tTesting 'erase(it)'.\n");
            {
                size_t sz = X.size();
                for (Iter it = mX.begin(), nextIt; mX.end() != it;
                                                                 it = nextIt) {
                    numPasses = 0;
                    EXCEPTION_TEST_BEGIN(mX) {
                        ++numPasses;

                        nextIt = mX.erase(it);
                    } EXCEPTION_TEST_END
                    ASSERTV(1 == numPasses);

                    ASSERTV(X.size(), sz, X.size() == --sz);
                    ASSERTV(mX.begin() == nextIt);
#if 0
                    bool found = mX.end() == nextIt;
                    for (Iter itB = mX.begin(); mX.end() != itB; ++itB) {
                        ASSERTV(it != itB);
                        ASSERTV(K != itB->first);
                        if (nextIt == itB) {
                            found = true;
                        }
                    }
                    ASSERTV(found);
#endif
                }
                ASSERT(0 == sz);
                ASSERT(0 == X.size());
            }

            // Reclaim dynamically allocated object under test.

            fa.deleteObject(objPtr);

            // Verify all memory is released on object destruction.

            ASSERTV(LENGTH, CONFIG, da.numBlocksInUse(),
                    0 == da.numBlocksInUse());
            ASSERTV(LENGTH, CONFIG, fa.numBlocksInUse(),
                    0 == fa.numBlocksInUse());
            ASSERTV(LENGTH, CONFIG, sa.numBlocksInUse(),
                    0 == sa.numBlocksInUse());
        }
        ASSERTV(1 == done);

        {
            bslma::TestAllocator da("default",   veryVeryVeryVerbose);
            bslma::TestAllocator sa("supplied",  veryVeryVeryVerbose);

            bslma::DefaultAllocatorGuard dag(&da);

            Obj mX(&sa);    const Obj& X = mX;

            if (veryVerbose) printf("\n\tTesting range 'insert'.\n");
            {
                int numPasses = 0;
                EXCEPTION_TEST_BEGIN(mX) {
                    ++numPasses;

                    // Note that the exception guarantee for range insert is
                    // only that the container will be valid, not that it will
                    // be unchanged, so we have to clear the container every
                    // time we go back.

                    mX.clear();

                    values.resetIterators();
                    mX.insert(values.begin(), values.index(LENGTH));
                } EXCEPTION_TEST_END
                ASSERTV(!PLAT_EXC || 0 == LENGTH || numPasses > 2);

                values.resetIterators();

                ASSERT(LENGTH == X.size());

                ASSERTV(LENGTH, matchFirstValues(X, VALUES, LENGTH));
            }

            if (veryVerbose) printf("\n\tTesting range 'erase'.\n");
            {
                Iter it;
                int numPasses = 0;
                EXCEPTION_TEST_BEGIN(mX) {
                    ++numPasses;

                    it = mX.erase(mX.begin(), mX.end());
                } EXCEPTION_TEST_END
                ASSERTV(1 == numPasses);

                ASSERTV(X.size(), 0 == X.size());
                ASSERTV(X.end() == it);

                ASSERTV(matchFirstValues(X, VALUES, 0));
            }

            mX.insert(values.begin(), values.index(LENGTH));
            values.resetIterators();

            ASSERTV(matchFirstValues(X, VALUES, LENGTH));

            if (veryVerbose) printf("Inserting with useful hint\n");
            {
                {
                    mX.reserve(X.size() * 2);
                    CIter end = X.end();
                    for (CIter cit = X.begin(); end != cit; ) {
                        const KEY& k = cit->first;
                        CIter retIt = mX.insert(cit, *cit);
                        ASSERT(cit != retIt);
                        ASSERT(*retIt == *cit);
                        ASSERT(cit->first == k);
                        while (end != cit && cit->first == k) {
                            ++cit;
                        }
                    }
                }
                ASSERTV(2 * LENGTH == X.size());

                // verify there's 2 of each, adjacent to each other

                size_t count = 0;
                {
                    CIter end = X.end();
                    const KEY *pk = &VALUES['Z' - 'A'].first;
                    for (CIter cit = X.begin(); end != cit; ++count) {
                        ASSERTV(*pk != cit->first);
                        pk = &cit->first;
                        ++cit;
                        ASSERTV(end != cit);
                        ASSERT(cit->first == *pk);
                        ++cit;
                    }
                }
                ASSERTV(2 * count  == X.size());
                ASSERTV(2 * LENGTH == X.size());

                mX.insert(VALUES[LENGTH]);
            }

            ASSERTV(2 * LENGTH + 1 == X.size());

            if (veryVerbose) printf("\n\tTesting 'erase(Key)'.\n");
            {
                ASSERTV(0 != VALUES.size() % 17);

                size_t sum = 0;
                for (size_t tj = 0; tj < VALUES.size(); ++tj) {
                    size_t tvi = (tj * 17) % VALUES.size();

                    size_t result;
                    int numPasses = 0;
                    EXCEPTION_TEST_BEGIN(mX) {
                        ++numPasses;

                        result = mX.erase(VALUES[tvi].first);
                    } EXCEPTION_TEST_END
                    ASSERTV(1 == numPasses);
                    ASSERTV(result == (tvi < LENGTH + 1 ? (tvi < LENGTH
                                                           ? 2 : 1) : 0));
                    sum += result;
                }

                ASSERTV(X.size(), 0 == X.size());
                ASSERTV(2 * LENGTH + 1 == sum);
            }
        }
    }

    {
        const size_t initialNumBuckets[] = { 0, 1, 2, 3, 5, 7, 11, 13, 17, 19,
                                    23, 29, 31, 37, 41, 43, 47, 51, 100, 200 };
        enum { NUM_INITIAL_NUM_BUCKETS = sizeof initialNumBuckets /
                                                   sizeof *initialNumBuckets };

        for (int ti = 0; ti < NUM_INITIAL_NUM_BUCKETS; ++ti) {
            {
                bslma::TestAllocator da("default",   veryVeryVeryVerbose);
                bslma::DefaultAllocatorGuard dag(&da);

                {
                    Obj mX(initialNumBuckets[ti]);    const Obj& X = mX;
                    ASSERTV(X.bucket_count() >= initialNumBuckets[ti]);
                }
                {
                    Obj mX(initialNumBuckets[ti],
                           tstHash);
                    const Obj& X = mX;
                    ASSERTV(X.bucket_count() >= initialNumBuckets[ti]);
                }
                {
                    Obj mX(initialNumBuckets[ti],
                           tstHash,
                           tstEqual);
                    const Obj& X = mX;
                    ASSERTV(X.bucket_count() >= initialNumBuckets[ti]);
                }
                {
                    Obj mX(initialNumBuckets[ti],
                           tstHash,
                           tstEqual,
                           (bslma::Allocator *) 0);
                    const Obj& X = mX;
                    ASSERTV(X.bucket_count() >= initialNumBuckets[ti]);
                }

                ASSERTV(ti, da.numBlocksTotal(),
                                      (ti == 0) == (0 == da.numBlocksTotal()));
            }
            {
                bslma::TestAllocator sa("supplied",  veryVeryVeryVerbose);
                bslma::TestAllocator da("default",   veryVeryVeryVerbose);
                bslma::DefaultAllocatorGuard dag(&da);

                Obj mX(initialNumBuckets[ti],
                       tstHash,
                       tstEqual,
                       &sa);
                const Obj& X = mX;
                ASSERTV(X.bucket_count() >= initialNumBuckets[ti]);

                ASSERTV(0 == da.numBlocksTotal());
                ASSERTV(ti, sa.numBlocksTotal(),
                                      (ti == 0) == (0 == sa.numBlocksTotal()));
            }
        }
    }
}

//=============================================================================
//                                  USAGE
//-----------------------------------------------------------------------------

void usage()
{
#ifndef DONT_DO_USAGE
///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: Creating a Concordance
///- - - - - - - - - - - - - - - - -
// Unordered multimap are useful in situations when there is no meaningful
// way to compare key values, when the order of the keys is irrelevant to the
// problem domain, or (even if there is a meaningful ordering) the benefit of
// ordering the results is outweighed by the higher performance provided by
// unordered maps (compared to ordered maps).
//
// One uses a multi-map (ordered or unordered) when there may be more than one
// mapped value associated with a key value.  In this example we will use
// 'bslstl_unorderedmultimap' to create a concordance (an index of where each
// unique word appears in the set of documents).
//
// Our source of documents is a set of statically initialized arrrays:
//..
    static char document0[] =
    " IN CONGRESS, July 4, 1776.\n"
    "\n"
    " The unanimous Declaration of the thirteen united States of America,\n"
    "\n"
    " When in the Course of human events, it becomes necessary for one\n"
    " people to dissolve the political bands which have connected them with\n"
    " another, and to assume among the powers of the earth, the separate\n"
    " and equal station to which the Laws of Nature and of Nature's God\n"
    " entitle them, a decent respect to the opinions of mankind requires\n"
    " that they should declare the causes which impel them to the\n"
    " separation.  We hold these truths to be self-evident, that all men\n"
    " are created equal, that they are endowed by their Creator with\n"
    " certain unalienable Rights, that among these are Life, Liberty and\n"
    " the pursuit of Happiness.--That to secure these rights, Governments\n"
    " are instituted among Men, deriving their just powers from the consent\n"
    " of the governed, --That whenever any Form of Government becomes\n"
    " destructive of these ends, it is the Right of the People to alter or\n"
    " to abolish it, and to institute new Government, laying its foundation\n"
    " on such principles and organizing its powers in such form, as to them\n"
    " shall seem most likely to effect their Safety and Happiness.\n"
    " Prudence, indeed, will dictate that Governments long established\n"
    " should not be changed for light and transient causes; and accordingly\n"
    " all experience hath shewn, that mankind are more disposed to suffer,\n"
    " while evils are sufferable, than to right themselves by abolishing\n"
    " the forms to which they are accustomed.  But when a long train of\n"
    " abuses and usurpations, pursuing invariably the same Object evinces a\n"
    " design to reduce them under absolute Despotism, it is their right, it\n"
    " is their duty, to throw off such Government, and to provide new\n"
    " Guards for their future security.--Such has been the patient\n"
    " sufferance of these Colonies; and such is now the necessity which\n"
    " constrains them to alter their former Systems of Government.  The\n"
    " history of the present King of Great Britain is a history of repeated\n"
    " injuries and usurpations, all having in direct object the\n"
    " establishment of an absolute Tyranny over these States.  To prove\n"
    " this, let Facts be submitted to a candid world.\n"
    "\n"
    ": o He has refused his Assent to Laws, the most wholesome and\n"
    ":   necessary for the public good.\n"
    ":\n"
    ": o He has forbidden his Governors to pass Laws of immediate and\n"
    ":   pressing importance, unless suspended in their operation till his\n"
    ":   Assent should be obtained; and when so suspended, he has utterly\n"
    ":   neglected to attend to them.\n"
    ":\n"
    ": o He has refused to pass other Laws for the accommodation of large\n"
    ":   districts of people, unless those people would relinquish the\n"
    ":   right of Representation in the Legislature, a right inestimable to\n"
    ":   them and formidable to tyrants only.\n"
    ":\n"
    ": o He has called together legislative bodies at places unusual,\n"
    ":   uncomfortable, and distant from the depository of their public\n"
    ":   Records, for the sole purpose of fatiguing them into compliance\n"
    ":   with his measures.\n"
    ":\n"
    ": o He has dissolved Representative Houses repeatedly, for opposing\n"
    ":   with manly firmness his invasions on the rights of the people.\n"
    ":\n"
    ": o He has refused for a long time, after such dissolutions, to cause\n"
    ":   others to be elected; whereby the Legislative powers, incapable of\n"
    ":   Annihilation, have returned to the People at large for their\n"
    ":   exercise; the State remaining in the mean time exposed to all the\n"
    ":   dangers of invasion from without, and convulsions within.\n"
    ":\n"
    ": o He has endeavoured to prevent the population of these States; for\n"
    ":   that purpose obstructing the Laws for Naturalization of\n"
    ":   Foreigners; refusing to pass others to encourage their migrations\n"
    ":   hither, and raising the conditions of new Appropriations of Lands.\n"
    ":\n"
    ": o He has obstructed the Administration of Justice, by refusing his\n"
    ":   Assent to Laws for establishing Judiciary powers.\n"
    ":\n"
    ": o He has made Judges dependent on his Will alone, for the tenure of\n"
    ":   their offices, and the amount and payment of their salaries.\n"
    ":\n"
    ": o He has erected a multitude of New Offices, and sent hither swarms\n"
    ":   of Officers to harrass our people, and eat out their substance.\n"
    ":\n"
    ": o He has kept among us, in times of peace, Standing Armies without\n"
    ":   the Consent of our legislatures.\n"
    ":\n"
    ": o He has affected to render the Military independent of and superior\n"
    ":   to the Civil power.\n"
    ":\n"
    ": o He has combined with others to subject us to a jurisdiction\n"
    ":   foreign to our constitution, and unacknowledged by our laws;\n"
    ":   giving his Assent to their Acts of pretended Legislation:\n"
    ":\n"
    ": o For Quartering large bodies of armed troops among us: For\n"
    ":   protecting them, by a mock Trial, from punishment for any Murders\n"
    ":   which they should commit on the Inhabitants of these States:\n"
    ":\n"
    ": o For cutting off our Trade with all parts of the world:\n"
    ":\n"
    ": o For imposing Taxes on us without our Consent: For depriving us in\n"
    ":   many cases, of the benefits of Trial by Jury:\n"
    ":\n"
    ": o For transporting us beyond Seas to be tried for pretended offences\n"
    ":\n"
    ": o For abolishing the free System of English Laws in a neighbouring\n"
    ":   Province, establishing therein an Arbitrary government, and\n"
    ":   enlarging its Boundaries so as to render it at once an example and\n"
    ":   fit instrument for introducing the same absolute rule into these\n"
    ":   Colonies:\n"
    ":\n"
    ": o For taking away our Charters, abolishing our most valuable Laws,\n"
    ":   and altering fundamentally the Forms of our Governments:\n"
    ":\n"
    ": o For suspending our own Legislatures, and declaring themselves\n"
    ":   invested with power to legislate for us in all cases whatsoever.\n"
    ":\n"
    ": o He has abdicated Government here, by declaring us out of his\n"
    ":   Protection and waging War against us.\n"
    ":\n"
    ": o He has plundered our seas, ravaged our Coasts, burnt our towns,\n"
    ":   and destroyed the lives of our people.  He is at this time\n"
    ":   transporting large Armies of foreign Mercenaries to compleat the\n"
    ":   works of death, desolation and tyranny, already begun with\n"
    ":   circumstances of Cruelty & perfidy scarcely paralleled in the most\n"
    ":   barbarous ages, and totally unworthy the Head of a civilized\n"
    ":   nation.\n"
    ":\n"
    ": o He has constrained our fellow Citizens taken Captive on the high\n"
    ":   Seas to bear Arms against their Country, to become the\n"
    ":   executioners of their friends and Brethren, or to fall themselves\n"
    ":   by their Hands.\n"
    ":\n"
    ": o He has excited domestic insurrections amongst us, and has\n"
    ":   endeavoured to bring on the inhabitants of our frontiers, the\n"
    ":   merciless Indian Savages, whose known rule of warfare, is an\n"
    ":   undistinguished destruction of all ages, sexes and conditions.\n"
    "\n"
    " In every stage of these Oppressions We have Petitioned for Redress in\n"
    " the most humble terms: Our repeated Petitions have been answered only\n"
    " by repeated injury.  A Prince whose character is thus marked by every\n"
    " act which may define a Tyrant, is unfit to be the ruler of a free\n"
    " people.\n"
    "\n"
    " Nor have We been wanting in attentions to our Brittish brethren.  We\n"
    " have warned them from time to time of attempts by their legislature\n"
    " to extend an unwarrantable jurisdiction over us.  We have reminded\n"
    " them of the circumstances of our emigration and settlement here.  We\n"
    " have appealed to their native justice and magnanimity, and we have\n"
    " conjured them by the ties of our common kindred to disavow these\n"
    " usurpations, which, would inevitably interrupt our connections and\n"
    " correspondence.  They too have been deaf to the voice of justice and\n"
    " of consanguinity.  We must, therefore, acquiesce in the necessity,\n"
    " which denounces our Separation, and hold them, as we hold the rest of\n"
    " mankind, Enemies in War, in Peace Friends.\n"
    "\n"
    " We, therefore, the Representatives of the united States of America,\n"
    " in General Congress, Assembled, appealing to the Supreme Judge of the\n"
    " world for the rectitude of our intentions, do, in the Name, and by\n"
    " Authority of the good People of these Colonies, solemnly publish and\n"
    " declare, That these United Colonies are, and of Right ought to be\n"
    " Free and Independent States; that they are Absolved from all\n"
    " Allegiance to the British Crown, and that all political connection\n"
    " between them and the State of Great Britain, is and ought to be\n"
    " totally dissolved; and that as Free and Independent States, they have\n"
    " full Power to levy War, conclude Peace, contract Alliances, establish\n"
    " Commerce, and to do all other Acts and Things which Independent\n"
    " States may of right do.  And for the support of this Declaration,\n"
    " with a firm reliance on the protection of divine Providence, we\n"
    " mutually pledge to each other our Lives, our Fortunes and our sacred\n"
    " Honor.\n";

    static char document1[] =
    "/The Universal Declaration of Human Rights\n"
    "/-----------------------------------------\n"
    "/Preamble\n"
    "/ - - - -\n"
    " Whereas recognition of the inherent dignity and of the equal and\n"
    " inalienable rights of all members of the human family is the\n"
    " foundation of freedom, justice and peace in the world,\n"
    "\n"
    " Whereas disregard and contempt for human rights have resulted in\n"
    " barbarous acts which have outraged the conscience of mankind, and the\n"
    " advent of a world in which human beings shall enjoy freedom of speech\n"
    " and belief and freedom from fear and want has been proclaimed as the\n"
    " highest aspiration of the common people,\n"
    "\n"
    " Whereas it is essential, if man is not to be compelled to have\n"
    " recourse, as a last resort, to rebellion against tyranny and\n"
    " oppression, that human rights should be protected by the rule of law,\n"
    "\n"
    " Whereas it is essential to promote the development of friendly\n"
    " relations between nations,\n"
    "\n"
    " Whereas the peoples of the United Nations have in the Charter\n"
    " reaffirmed their faith in fundamental human rights, in the dignity\n"
    " and worth of the human person and in the equal rights of men and\n"
    " women and have determined to promote social progress and better\n"
    " standards of life in larger freedom,\n"
    "\n"
    " Whereas Member States have pledged themselves to achieve, in\n"
    " co-operation with the United Nations, the promotion of universal\n"
    " respect for and observance of human rights and fundamental freedoms,\n"
    "\n"
    " Whereas a common understanding of these rights and freedoms is of the\n"
    " greatest importance for the full realization of this pledge, Now,\n"
    " Therefore THE GENERAL ASSEMBLY proclaims THIS UNIVERSAL DECLARATION\n"
    " OF HUMAN RIGHTS as a common standard of achievement for all peoples\n"
    " and all nations, to the end that every individual and every organ of\n"
    " society, keeping this Declaration constantly in mind, shall strive by\n"
    " teaching and education to promote respect for these rights and\n"
    " freedoms and by progressive measures, national and international, to\n"
    " secure their universal and effective recognition and observance, both\n"
    " among the peoples of Member States themselves and among the peoples\n"
    " of territories under their jurisdiction.\n"
    "\n"
    "/Article 1\n"
    "/- - - - -\n"
    " All human beings are born free and equal in dignity and rights.  They\n"
    " are endowed with reason and conscience and should act towards one\n"
    " another in a spirit of brotherhood.\n"
    "\n"
    "/Article 2\n"
    "/- - - - -\n"
    " Everyone is entitled to all the rights and freedoms set forth in this\n"
    " Declaration, without distinction of any kind, such as race, colour,\n"
    " sex, language, religion, political or other opinion, national or\n"
    " social origin, property, birth or other status.  Furthermore, no\n"
    " distinction shall be made on the basis of the political,\n"
    " jurisdictional or international status of the country or territory to\n"
    " which a person belongs, whether it be independent, trust,\n"
    " non-self-governing or under any other limitation of sovereignty.\n"
    "\n"
    "/Article 3\n"
    "/- - - - -\n"
    " Everyone has the right to life, liberty and security of person.\n"
    "\n"
    "/Article 4\n"
    "/- - - - -\n"
    " No one shall be held in slavery or servitude; slavery and the slave\n"
    " trade shall be prohibited in all their forms.\n"
    "\n"
    "/Article 5\n"
    "/- - - - -\n"
    " No one shall be subjected to torture or to cruel, inhuman or\n"
    " degrading treatment or punishment.\n"
    "\n"
    "/Article 6\n"
    "/- - - - -\n"
    " Everyone has the right to recognition everywhere as a person before\n"
    " the law.\n"
    "\n"
    "/Article 7\n"
    "/- - - - -\n"
    " All are equal before the law and are entitled without any\n"
    " discrimination to equal protection of the law.  All are entitled to\n"
    " equal protection against any discrimination in violation of this\n"
    " Declaration and against any incitement to such discrimination.\n"
    "\n"
    "/Article 8\n"
    "/- - - - -\n"
    " Everyone has the right to an effective remedy by the competent\n"
    " national tribunals for acts violating the fundamental rights granted\n"
    " him by the constitution or by law.\n"
    "\n"
    "/Article 9\n"
    "/- - - - -\n"
    " No one shall be subjected to arbitrary arrest, detention or exile.\n"
    "\n"
    "/Article 10\n"
    "/ - - - - -\n"
    " Everyone is entitled in full equality to a fair and public hearing by\n"
    " an independent and impartial tribunal, in the determination of his\n"
    " rights and obligations and of any criminal charge against him.\n"
    "\n"
    "/Article 11\n"
    "/ - - - - -\n"
    ": 1 Everyone charged with a penal offence has the right to be presumed\n"
    ":   innocent until proved guilty according to law in a public trial at\n"
    ":   which he has had all the guarantees necessary for his defence.\n"
    ":\n"
    ": 2 No one shall be held guilty of any penal offence on account of any\n"
    ":   act or omission which did not constitute a penal offence, under\n"
    ":   national or international law, at the time when it was committed.\n"
    ":   Nor shall a heavier penalty be imposed than the one that was\n"
    ":   applicable at the time the penal offence was committed.\n"
    "\n"
    "/Article 12\n"
    "/ - - - - -\n"
    " No one shall be subjected to arbitrary interference with his privacy,\n"
    " family, home or correspondence, nor to attacks upon his honour and\n"
    " reputation.  Everyone has the right to the protection of the law\n"
    " against such interference or attacks.\n"
    "\n"
    "/Article 13\n"
    "/ - - - - -\n"
    ": 1 Everyone has the right to freedom of movement and residence within\n"
    ":   the borders of each state.\n"
    ":\n"
    ": 2 Everyone has the right to leave any country, including his own,\n"
    ":   and to return to his country.\n"
    "\n"
    "/Article 14\n"
    "/ - - - - -\n"
    ": 1 Everyone has the right to seek and to enjoy in other countries\n"
    ":   asylum from persecution.\n"
    ":\n"
    ": 2 This right may not be invoked in the case of prosecutions\n"
    ":   genuinely arising from non-political crimes or from acts contrary\n"
    ":   to the purposes and principles of the United Nations.\n"
    "\n"
    "/Article 15\n"
    "/ - - - - -\n"
    ": 1 Everyone has the right to a nationality.\n"
    ":\n"
    ": 2 No one shall be arbitrarily deprived of his nationality nor denied\n"
    ":   the right to change his nationality.\n"
    "\n"
    "/Article 16\n"
    "/ - - - - -\n"
    ": 1 Men and women of full age, without any limitation due to race,\n"
    ":   nationality or religion, have the right to marry and to found a\n"
    ":   family.  They are entitled to equal rights as to marriage, during\n"
    ":   marriage and at its dissolution.\n"
    ":\n"
    ": 2 Marriage shall be entered into only with the free and full consent\n"
    ":   of the intending spouses.\n"
    ":\n"
    ": 3 The family is the natural and fundamental group unit of society\n"
    ":   and is entitled to protection by society and the State.\n"
    "\n"
    "/Article 17\n"
    "/ - - - - -\n"
    ": 1 Everyone has the right to own property alone as well as in\n"
    ":   association with others.\n"
    ":\n"
    ": 2 No one shall be arbitrarily deprived of his property.\n"
    "\n"
    "/Article 18\n"
    "/ - - - - -\n"
    " Everyone has the right to freedom of thought, conscience and\n"
    " religion; this right includes freedom to change his religion or\n"
    " belief, and freedom, either alone or in community with others and in\n"
    " public or private, to manifest his religion or belief in teaching,\n"
    " practice, worship and observance.\n"
    "\n"
    "/Article 19\n"
    "/ - - - - -\n"
    " Everyone has the right to freedom of opinion and expression; this\n"
    " right includes freedom to hold opinions without interference and to\n"
    " seek, receive and impart information and ideas through any media and\n"
    " regardless of frontiers.\n"
    "\n"
    "/Article 20\n"
    "/ - - - - -\n"
    ": 1 Everyone has the right to freedom of peaceful assembly and\n"
    ":   association.\n"
    ":\n"
    ": 2 No one may be compelled to belong to an association.\n"
    "\n"
    "/Article 21\n"
    "/ - - - - -\n"
    ": 1 Everyone has the right to take part in the government of his\n"
    ":   country, directly or through freely chosen representatives.\n"
    ":\n"
    ": 2 Everyone has the right of equal access to public service in his\n"
    ":   country.\n"
    ":\n"
    ": 3 The will of the people shall be the basis of the authority of\n"
    ":   government; this will shall be expressed in periodic and genuine\n"
    ":   elections which shall be by universal and equal suffrage and shall\n"
    ":   be held by secret vote or by equivalent free voting procedures.\n"
    "\n"
    "/Article 22\n"
    "/ - - - - -\n"
    " Everyone, as a member of society, has the right to social security\n"
    " and is entitled to realization, through national effort and\n"
    " international co-operation and in accordance with the organization\n"
    " and resources of each State, of the economic, social and cultural\n"
    " rights indispensable for his dignity and the free development of his\n"
    " personality.\n"
    "\n"
    "/Article 23\n"
    "/ - - - - -\n"
    ": 1 Everyone has the right to work, to free choice of employment, to\n"
    ":   just and favourable conditions of work and to protection against\n"
    ":   unemployment.\n"
    ":\n"
    ": 2 Everyone, without any discrimination, has the right to equal pay\n"
    ":   for equal work.\n"
    ":\n"
    ": 3 Everyone who works has the right to just and favourable\n"
    ":   remuneration ensuring for himself and his family an existence\n"
    ":   worthy of human dignity, and supplemented, if necessary, by other\n"
    ":   means of social protection.\n"
    ":\n"
    ": 4 Everyone has the right to form and to join trade unions for the\n"
    ":   protection of his interests.\n"
    "\n"
    "/Article 24\n"
    "/ - - - - -\n"
    " Everyone has the right to rest and leisure, including reasonable\n"
    " limitation of working hours and periodic holidays with pay.\n"
    "\n"
    "/Article 25\n"
    "/ - - - - -\n"
    ": 1 Everyone has the right to a standard of living adequate for the\n"
    ":   health and well-being of himself and of his family, including\n"
    ":   food, clothing, housing and medical care and necessary social\n"
    ":   services, and the right to security in the event of unemployment,\n"
    ":   sickness, disability, widowhood, old age or other lack of\n"
    ":   livelihood in circumstances beyond his control.\n"
    ":\n"
    ": 2 Motherhood and childhood are entitled to special care and\n"
    ":   assistance.  All children, whether born in or out of wedlock,\n"
    ":   shall enjoy the same social protection.\n"
    "\n"
    "/Article 26\n"
    "/ - - - - -\n"
    ": 1 Everyone has the right to education.  Education shall be free, at\n"
    ":   least in the elementary and fundamental stages.  Elementary\n"
    ":   education shall be compulsory.  Technical and professional\n"
    ":   education shall be made generally available and higher education\n"
    ":   shall be equally accessible to all on the basis of merit.\n"
    ":\n"
    ": 2 Education shall be directed to the full development of the human\n"
    ":   personality and to the strengthening of respect for human rights\n"
    ":   and fundamental freedoms.  It shall promote understanding,\n"
    ":   tolerance and friendship among all nations, racial or religious\n"
    ":   groups, and shall further the activities of the United Nations for\n"
    ":   the maintenance of peace.\n"
    ":\n"
    ": 3 Parents have a prior right to choose the kind of education that\n"
    ":   shall be given to their children.\n"
    "\n"
    "/Article 27\n"
    "/ - - - - -\n"
    ": 1 Everyone has the right freely to participate in the cultural life\n"
    ":   of the community, to enjoy the arts and to share in scientific\n"
    ":   advancement and its benefits.\n"
    ":\n"
    ": 2 Everyone has the right to the protection of the moral and material\n"
    ":   interests resulting from any scientific, literary or artistic\n"
    ":   production of which he is the author.\n"
    "\n"
    "/Article 28\n"
    "/ - - - - -\n"
    " Everyone is entitled to a social and international order in which the\n"
    " rights and freedoms set forth in this Declaration can be fully\n"
    " realized.\n"
    "\n"
    "/Article 29\n"
    "/ - - - - -\n"
    ": 1 Everyone has duties to the community in which alone the free and\n"
    ":   full development of his personality is possible.\n"
    ":\n"
    ": 2 In the exercise of his rights and freedoms, everyone shall be\n"
    ":   subject only to such limitations as are determined by law solely\n"
    ":   for the purpose of securing due recognition and respect for the\n"
    ":   rights and freedoms of others and of meeting the just requirements\n"
    ":   of morality, public order and the general welfare in a democratic\n"
    ":   society.\n"
    ":\n"
    ": 3 These rights and freedoms may in no case be exercised contrary to\n"
    ":   the purposes and principles of the United Nations.\n"
    "\n"
    "/Article 30\n"
    "/ - - - - -\n"
    " Nothing in this Declaration may be interpreted as implying for any\n"
    " State, group or person any right to engage in any activity or to\n"
    " perform any act aimed at the destruction of any of the rights and\n"
    " freedoms set forth herein.\n";

    static char document2[] =
    "/CHARTER OF FUNDAMENTAL RIGHTS OF THE EUROPEAN UNION\n"
    "/---------------------------------------------------\n"
    " PREAMBLE\n"
    "\n"
    " The peoples of Europe, in creating an ever closer union among them,\n"
    " are resolved to share a peaceful future based on common values.\n"
    "\n"
    " Conscious of its spiritual and moral heritage, the Union is founded\n"
    " on the indivisible, universal values of human dignity, freedom,\n"
    " equality and solidarity; it is based on the principles of democracy\n"
    " and the rule of law.  It places the individual at the heart of its\n"
    " activities, by establishing the citizenship of the Union and by\n"
    " creating an area of freedom, security and justice.\n"
    "\n"
    " The Union contributes to the preservation and to the development of\n"
    " these common values while respecting the diversity of the cultures\n"
    " and traditions of the peoples of Europe as well as the national\n"
    " identities of the Member States and the organisation of their public\n"
    " authorities at national, regional and local levels; it seeks to\n"
    " promote balanced and sustainable development and ensures free\n"
    " movement of persons, goods, services and capital, and the freedom of\n"
    " establishment.\n"
    "\n"
    " To this end, it is necessary to strengthen the protection of\n"
    " fundamental rights in the light of changes in society, social\n"
    " progress and scientific and technological developments by making\n"
    " those rights more visible in a Charter.\n"
    "\n"
    " This Charter reaffirms, with due regard for the powers and tasks of\n"
    " the Community and the Union and the principle of subsidiarity, the\n"
    " rights as they result, in particular, from the constitutional\n"
    " traditions and international obligations common to the Member States,\n"
    " the Treaty on European Union, the Community Treaties, the European\n"
    " Convention for the Protection of Human Rights and Fundamental\n"
    " Freedoms, the Social Charters adopted by the Community and by the\n"
    " Council of Europe and the case-law of the Court of Justice of the\n"
    " European Communities and of the European Court of Human Rights.\n"
    "\n"
    " Enjoyment of these rights entails responsibilities and duties with\n"
    " regard to other persons, to the human community and to future\n"
    " generations.\n"
    "\n"
    " The Union therefore recognises the rights, freedoms and principles\n"
    " set out hereafter.\n"
    "\n"
    "/CHAPTER I\n"
    "/- - - - -\n"
    " DIGNITY\n"
    "\n"
    "/Article 1\n"
    "/  -  -  -\n"
    " Human dignity\n"
    "\n"
    " Human dignity is inviolable.  It must be respected and protected.\n"
    "\n"
    "/Article 2\n"
    "/  -  -  -\n"
    " Right to life\n"
    "\n"
    ": 1 Everyone has the right to life.\n"
    ": 2 No one shall be condemned to the death penalty, or executed.\n"
    "\n"
    "/Article 3\n"
    "/  -  -  -\n"
    " Right to the integrity of the person\n"
    "\n"
    ": 1 Everyone has the right to respect for his or her physical and\n"
    ":   mental integrity.\n"
    ":\n"
    ": 2 In the fields of medicine and biology, the following must be\n"
    ":   respected in particular:\n"
    ":\n"
    ":   o the free and informed consent of the person concerned, according\n"
    ":     to the procedures laid down by law,\n"
    ":\n"
    ":   o the prohibition of eugenic practices, in particular those aiming\n"
    ":     at the selection of persons,\n"
    ":\n"
    ":   o the prohibition on making the human body and its parts as such a\n"
    ":     source of financial gain,\n"
    ":\n"
    ":   o the prohibition of the reproductive cloning of human beings.\n"
    "\n"
    "/Article 4\n"
    "/  -  -  -\n"
    " Prohibition of torture and inhuman or degrading treatment or\n"
    " punishment\n"
    "\n"
    " No one shall be subjected to torture or to inhuman or degrading\n"
    " treatment or punishment.\n"
    "\n"
    "/Article 5\n"
    "/  -  -  -\n"
    " Prohibition of slavery and forced labour\n"
    "\n"
    ": 1 No one shall be held in slavery or servitude.\n"
    ": 2 No one shall be required to perform forced or compulsory labour.\n"
    ": 3 Trafficking in human beings is prohibited.\n"
    "\n"
    "/CHAPTER II\n"
    "/ - - - - -\n"
    " FREEDOMS\n"
    "\n"
    "/Article 6\n"
    "/  -  -  -\n"
    " Right to liberty and security\n"
    "\n"
    " Everyone has the right to liberty and security of person.\n"
    "\n"
    "/Article 7\n"
    "/  -  -  -\n"
    " Respect for private and family life\n"
    "\n"
    " Everyone has the right to respect for his or her private and family\n"
    " life, home and communications.\n"
    "\n"
    "/Article 8\n"
    "/  -  -  -\n"
    " Protection of personal data\n"
    "\n"
    ": 1 Everyone has the right to the protection of personal data\n"
    ":   concerning him or her.\n"
    ":\n"
    ": 2 Such data must be processed fairly for specified purposes and on\n"
    ":   the basis of the consent of the person concerned or some other\n"
    ":   legitimate basis laid down by law.  Everyone has the right of\n"
    ":   access to data which has been collected concerning him or her, and\n"
    ":   the right to have it rectified.\n"
    ":\n"
    ": 3 Compliance with these rules shall be subject to control by an\n"
    ":   independent authority.\n"
    "\n"
    "/Article 9\n"
    "/  -  -  -\n"
    " Right to marry and right to found a family\n"
    "\n"
    " The right to marry and the right to found a family shall be\n"
    " guaranteed in accordance with the national laws governing the\n"
    " exercise of these rights.\n"
    "\n"
    "/Article 10\n"
    "/-  -  -  -\n"
    " Freedom of thought, conscience and religion\n"
    "\n"
    ": 1 Everyone has the right to freedom of thought, conscience and\n"
    ":   religion.  This right includes freedom to change religion or\n"
    ":   belief and freedom, either alone or in community with others and\n"
    ":   in public or in private, to manifest religion or belief, in\n"
    ":   worship, teaching, practice and observance.\n"
    ":\n"
    ": 2 The right to conscientious objection is recognised, in accordance\n"
    ":   with the national laws governing the exercise of this right.\n"
    "\n"
    "/Article 11\n"
    "/-  -  -  -\n"
    " Freedom of expression and information\n"
    "\n"
    ": 1 Everyone has the right to freedom of expression.  This right shall\n"
    ":   include freedom to hold opinions and to receive and impart\n"
    ":   information and ideas without interference by public authority and\n"
    ":   regardless of frontiers.\n"
    ":\n"
    ": 2 The freedom and pluralism of the media shall be respected.\n"
    "\n"
    "/Article 12\n"
    "/-  -  -  -\n"
    " Freedom of assembly and of association\n"
    "\n"
    ": 1 Everyone has the right to freedom of peaceful assembly and to\n"
    ":   freedom of association at all levels, in particular in political,\n"
    ":   trade union and civic matters, which implies the right of everyone\n"
    ":   to form and to join trade unions for the protection of his or her\n"
    ":   interests.\n"
    ":\n"
    ": 2 Political parties at Union level contribute to expressing the\n"
    ":   political will of the citizens of the Union.\n"
    "\n"
    "/Article 13\n"
    "/-  -  -  -\n"
    " Freedom of the arts and sciences\n"
    "\n"
    " The arts and scientific research shall be free of constraint.\n"
    " Academic freedom shall be respected.\n"
    "\n"
    "/Article 14\n"
    "/-  -  -  -\n"
    " Right to education\n"
    "\n"
    ": 1 Everyone has the right to education and to have access to\n"
    ":   vocational and continuing training.\n"
    ":\n"
    ": 2 This right includes the possibility to receive free compulsory\n"
    ":   education.\n"
    ":\n"
    ": 3 The freedom to found educational establishments with due respect\n"
    ":   for democratic principles and the right of parents to ensure the\n"
    ":   education and teaching of their children in conformity with their\n"
    ":   religious, philosophical and pedagogical convictions shall be\n"
    ":   respected, in accordance with the national laws governing the\n"
    ":   exercise of such freedom and right.\n"
    "\n"
    "/Article 15\n"
    "/-  -  -  -\n"
    " Freedom to choose an occupation and right to engage in work\n"
    "\n"
    ": 1 Everyone has the right to engage in work and to pursue a freely\n"
    ":   chosen or accepted occupation.\n"
    ":\n"
    ": 2 Every citizen of the Union has the freedom to seek employment, to\n"
    ":   work, to exercise the right of establishment and to provide\n"
    ":   services in any Member State.\n"
    ":\n"
    ": 3 Nationals of third countries who are authorised to work in the\n"
    ":   territories of the Member States are entitled to working\n"
    ":   conditions equivalent to those of citizens of the Union.\n"
    "\n"
    "/Article 16\n"
    "/-  -  -  -\n"
    " Freedom to conduct a business\n"
    "\n"
    " The freedom to conduct a business in accordance with Community law\n"
    " and national laws and practices is recognised.\n"
    "\n"
    "/Article 17\n"
    "/-  -  -  -\n"
    " Right to property\n"
    "\n"
    ": 1 Everyone has the right to own, use, dispose of and bequeath his or\n"
    ":   her lawfully acquired possessions.  No one may be deprived of his\n"
    ":   or her possessions, except in the public interest and in the cases\n"
    ":   and under the conditions provided for by law, subject to fair\n"
    ":   compensation being paid in good time for their loss.  The use of\n"
    ":   property may be regulated by law in so far as is necessary for the\n"
    ":   general interest.\n"
    ":\n"
    ": 2 Intellectual property shall be protected.\n"
    "\n"
    "/Article 18\n"
    "/-  -  -  -\n"
    " Right to asylum\n"
    "\n"
    " The right to asylum shall be guaranteed with due respect for the\n"
    " rules of the Geneva Convention of 28 July 1951 and the Protocol of 31\n"
    " January 1967 relating to the status of refugees and in accordance\n"
    " with the Treaty establishing the European Community.\n"
    "\n"
    "/Article 19\n"
    "/-  -  -  -\n"
    " Protection in the event of removal, expulsion or extradition\n"
    "\n"
    ": 1 Collective expulsions are prohibited.\n"
    ":\n"
    ": 2 No one may be removed, expelled or extradited to a State where\n"
    ":   there is a serious risk that he or she would be subjected to the\n"
    ":   death penalty, torture or other inhuman or degrading treatment or\n"
    ":   punishment.\n"
    "\n"
    "/CHAPTER III\n"
    "/- - - - - -\n"
    " EQUALITY\n"
    "\n"
    "/Article 20\n"
    "/-  -  -  -\n"
    " Equality before the law\n"
    "\n"
    " Everyone is equal before the law.\n"
    "\n"
    "/Article 21\n"
    "/-  -  -  -\n"
    " Non-discrimination\n"
    "\n"
    ": 1 Any discrimination based on any ground such as sex, race, colour,\n"
    ":   ethnic or social origin, genetic features, language, religion or\n"
    ":   belief, political or any other opinion, membership of a national\n"
    ":   minority, property, birth, disability, age or sexual orientation\n"
    ":   shall be prohibited.\n"
    ":\n"
    ": 2 Within the scope of application of the Treaty establishing the\n"
    ":   European Community and of the Treaty on European Union, and\n"
    ":   without prejudice to the special provisions of those Treaties, any\n"
    ":   discrimination on grounds of nationality shall be prohibited.\n"
    "\n"
    "/Article 22\n"
    "/-  -  -  -\n"
    " Cultural, religious and linguistic diversity\n"
    "\n"
    " The Union shall respect cultural, religious and linguistic diversity.\n"
    "\n"
    "/Article 23\n"
    "/-  -  -  -\n"
    " Equality between men and women\n"
    "\n"
    " Equality between men and women must be ensured in all areas,\n"
    " including employment, work and pay.  The principle of equality shall\n"
    " not prevent the maintenance or adoption of measures providing for\n"
    " specific advantages in favour of the under-represented sex.\n"
    "\n"
    "/Article 24\n"
    "/-  -  -  -\n"
    " The rights of the child\n"
    "\n"
    ": 1 Children shall have the right to such protection and care as is\n"
    ":   necessary for their well-being.  They may express their views\n"
    ":   freely.  Such views shall be taken into consideration on matters\n"
    ":   which concern them in accordance with their age and maturity.\n"
    ":\n"
    ": 2 In all actions relating to children, whether taken by public\n"
    ":   authorities or private institutions, the child's best interests\n"
    ":   must be a primary consideration.\n"
    ":\n"
    ": 3 Every child shall have the right to maintain on a regular basis a\n"
    ":   personal relationship and direct contact with both his or her\n"
    ":   parents, unless that is contrary to his or her interests.\n"
    "\n"
    "/Article 25\n"
    "/-  -  -  -\n"
    " The rights of the elderly\n"
    "\n"
    " The Union recognises and respects the rights of the elderly to lead a\n"
    " life of dignity and independence and to participate in social and\n"
    " cultural life.\n"
    "\n"
    "/Article 26\n"
    "/-  -  -  -\n"
    " Integration of persons with disabilities\n"
    "\n"
    " The Union recognises and respects the right of persons with\n"
    " disabilities to benefit from measures designed to ensure their\n"
    " independence, social and occupational integration and participation\n"
    " in the life of the community.\n"
    "\n"
    "/CHAPTER IV\n"
    "/ - - - - -\n"
    " SOLIDARITY\n"
    "\n"
    "/Article 27\n"
    "/-  -  -  -\n"
    " Workers' right to information and consultation within the undertaking\n"
    "\n"
    " Workers or their representatives must, at the appropriate levels, be\n"
    " guaranteed information and consultation in good time in the cases and\n"
    " under the conditions provided for by Community law and national laws\n"
    " and practices.\n"
    "\n"
    "/Article 28\n"
    "/-  -  -  -\n"
    " Right of collective bargaining and action\n"
    "\n"
    " Workers and employers, or their respective organisations, have, in\n"
    " accordance with Community law and national laws and practices, the\n"
    " right to negotiate and conclude collective agreements at the\n"
    " appropriate levels and, in cases of conflicts of interest, to take\n"
    " collective action to defend their interests, including strike action.\n"
    "\n"
    "/Article 29\n"
    "/-  -  -  -\n"
    " Right of access to placement services\n"
    "\n"
    " Everyone has the right of access to a free placement service.\n"
    "\n"
    "/Article 30\n"
    "/-  -  -  -\n"
    " Protection in the event of unjustified dismissal\n"
    "\n"
    " Every worker has the right to protection against unjustified\n"
    " dismissal, in accordance with Community law and national laws and\n"
    " practices.\n"
    "\n"
    "/Article 31\n"
    "/-  -  -  -\n"
    " Fair and just working conditions\n"
    "\n"
    ": 1 Every worker has the right to working conditions which respect his\n"
    ":   or her health, safety and dignity.\n"
    ":\n"
    ": 2 Every worker has the right to limitation of maximum working hours,\n"
    ":   to daily and weekly rest periods and to an annual period of paid\n"
    ":   leave.\n"
    "\n"
    "/Article 32\n"
    "/-  -  -  -\n"
    " Prohibition of child labour and protection of young people at work\n"
    "\n"
    " The employment of children is prohibited.  The minimum age of\n"
    " admission to employment may not be lower than the minimum\n"
    " school-leaving age, without prejudice to such rules as may be more\n"
    " favourable to young people and except for limited derogations.  Young\n"
    " people admitted to work must have working conditions appropriate to\n"
    " their age and be protected against economic exploitation and any work\n"
    " likely to harm their safety, health or physical, mental, moral or\n"
    " social development or to interfere with their education.\n"
    "\n"
    "/Article 33\n"
    "/-  -  -  -\n"
    " Family and professional life\n"
    "\n"
    ": 1 The family shall enjoy legal, economic and social protection.\n"
    ":\n"
    ": 2 To reconcile family and professional life, everyone shall have the\n"
    ":   right to protection from dismissal for a reason connected with\n"
    ":   maternity and the right to paid maternity leave and to parental\n"
    ":   leave following the birth or adoption of a child.\n"
    "\n"
    "/Article 34\n"
    "/-  -  -  -\n"
    " Social security and social assistance\n"
    "\n"
    ": 1 The Union recognises and respects the entitlement to social\n"
    ":   security benefits and social services providing protection in\n"
    ":   cases such as maternity, illness, industrial accidents, dependency\n"
    ":   or old age, and in the case of loss of employment, in accordance\n"
    ":   with the rules laid down by Community law and national laws and\n"
    ":   practices.\n"
    ":\n"
    ": 2 Everyone residing and moving legally within the European Union is\n"
    ":   entitled to social security benefits and social advantages in\n"
    ":   accordance with Community law and national laws and practices.\n"
    ":\n"
    ": 3 In order to combat social exclusion and poverty, the Union\n"
    ":   recognises and respects the right to social and housing assistance\n"
    ":   so as to ensure a decent existence for all those who lack\n"
    ":   sufficient resources, in accordance with the rules laid down by\n"
    ":   Community law and national laws and practices.\n"
    "\n"
    "/Article 35\n"
    "/-  -  -  -\n"
    " Health care\n"
    "\n"
    " Everyone has the right of access to preventive health care and the\n"
    " right to benefit from medical treatment under the conditions\n"
    " established by national laws and practices.  A high level of human\n"
    " health protection shall be ensured in the definition and\n"
    " implementation of all Union policies and activities.\n"
    "\n"
    "/Article 36\n"
    "/-  -  -  -\n"
    " Access to services of general economic interest\n"
    "\n"
    " The Union recognises and respects access to services of general\n"
    " economic interest as provided for in national laws and practices, in\n"
    " accordance with the Treaty establishing the European Community, in\n"
    " order to promote the social and territorial cohesion of the Union.\n"
    "\n"
    "/Article 37\n"
    "/-  -  -  -\n"
    " Environmental protection\n"
    "\n"
    " A high level of environmental protection and the improvement of the\n"
    " quality of the environment must be integrated into the policies of\n"
    " the Union and ensured in accordance with the principle of sustainable\n"
    " development.\n"
    "\n"
    "/Article 38\n"
    "/-  -  -  -\n"
    " Consumer protection\n"
    "\n"
    " Union policies shall ensure a high level of consumer protection.\n"
    "\n"
    "/CHAPTER V\n"
    "/- - - - -\n"
    " CITIZENS' RIGHTS\n"
    "\n"
    "/Article 39\n"
    "/-  -  -  -\n"
    " Right to vote and to stand as a candidate at elections to the\n"
    " European Parliament\n"
    "\n"
    ": 1 Every citizen of the Union has the right to vote and to stand as a\n"
    ":   candidate at elections to the European Parliament in the Member\n"
    ":   State in which he or she resides, under the same conditions as\n"
    ":   nationals of that State.\n"
    ":\n"
    ": 2 Members of the European Parliament shall be elected by direct\n"
    ":   universal suffrage in a free and secret ballot.\n"
    "\n"
    "/Article 40\n"
    "/-  -  -  -\n"
    " Right to vote and to stand as a candidate at municipal elections\n"
    "\n"
    " Every citizen of the Union has the right to vote and to stand as a\n"
    " candidate at municipal elections in the Member State in which he or\n"
    " she resides under the same conditions as nationals of that State.\n"
    "\n"
    "/Article 41\n"
    "/-  -  -  -\n"
    " Right to good administration\n"
    "\n"
    ": 1 Every person has the right to have his or her affairs handled\n"
    ":   impartially, fairly and within a reasonable time by the\n"
    ":   institutions and bodies of the Union.\n"
    ":\n"
    ": 2 This right includes:\n"
    ":\n"
    ":   o the right of every person to be heard, before any individual\n"
    ":     measure which would affect him or her adversely is taken;\n"
    ":\n"
    ":   o the right of every person to have access to his or her file,\n"
    ":     while respecting the legitimate interests of confidentiality and\n"
    ":     of professional and business secrecy;\n"
    ":\n"
    ":   o the obligation of the administration to give reasons for its\n"
    ":     decisions.\n"
    ":\n"
    ": 3 Every person has the right to have the Community make good any\n"
    ":   damage caused by its institutions or by its servants in the\n"
    ":   performance of their duties, in accordance with the general\n"
    ":   principles common to the laws of the Member States.\n"
    ":\n"
    ": 4 Every person may write to the institutions of the Union in one of\n"
    ":   the languages of the Treaties and must have an answer in the same\n"
    ":   language.\n"
    "\n"
    "/Article 42\n"
    "/-  -  -  -\n"
    " Right of access to documents\n"
    "\n"
    " Any citizen of the Union, and any natural or legal person residing or\n"
    " having its registered office in a Member State, has a right of access\n"
    " to European Parliament, Council and Commission documents.\n"
    "\n"
    "/Article 43\n"
    "/-  -  -  -\n"
    " Ombudsman\n"
    "\n"
    " Any citizen of the Union and any natural or legal person residing or\n"
    " having its registered office in a Member State has the right to refer\n"
    " to the Ombudsman of the Union cases of maladministration in the\n"
    " activities of the Community institutions or bodies, with the\n"
    " exception of the Court of Justice and the Court of First Instance\n"
    " acting in their judicial role.\n"
    "\n"
    "/Article 44\n"
    "/-  -  -  -\n"
    " Right to petition\n"
    "\n"
    " Any citizen of the Union and any natural or legal person residing or\n"
    " having its registered office in a Member State has the right to\n"
    " petition the European Parliament.\n"
    "\n"
    "/Article 45\n"
    "/-  -  -  -\n"
    " Freedom of movement and of residence\n"
    "\n"
    ": 1 Every citizen of the Union has the right to move and reside freely\n"
    ":   within the territory of the Member States.\n"
    ":\n"
    ": 2 Freedom of movement and residence may be granted, in accordance\n"
    ":   with the Treaty establishing the European Community, to nationals\n"
    ":   of third countries legally resident in the territory of a Member\n"
    ":   State.\n"
    "\n"
    "/Article 46\n"
    "/-  -  -  -\n"
    " Diplomatic and consular protection\n"
    "\n"
    " Every citizen of the Union shall, in the territory of a third country\n"
    " in which the Member State of which he or she is a national is not\n"
    " represented, be entitled to protection by the diplomatic or consular\n"
    " authorities of any Member State, on the same conditions as the\n"
    " nationals of that Member State.\n"
    "\n"
    "/CHAPTER VI\n"
    "/ - - - - -\n"
    " JUSTICE\n"
    "\n"
    "/Article 47\n"
    "/-  -  -  -\n"
    " Right to an effective remedy and to a fair trial\n"
    "\n"
    " Everyone whose rights and freedoms guaranteed by the law of the Union\n"
    " are violated has the right to an effective remedy before a tribunal\n"
    " in compliance with the conditions laid down in this Article.\n"
    " Everyone is entitled to a fair and public hearing within a reasonable\n"
    " time by an independent and impartial tribunal previously established\n"
    " by law.  Everyone shall have the possibility of being advised,\n"
    " defended and represented.  Legal aid shall be made available to those\n"
    " who lack sufficient resources in so far as such aid is necessary to\n"
    " ensure effective access to justice.\n"
    "\n"
    "/Article 48\n"
    "/-  -  -  -\n"
    " Presumption of innocence and right of defence\n"
    "\n"
    ": 1 Everyone who has been charged shall be presumed innocent until\n"
    ":   proved guilty according to law.\n"
    ":\n"
    ": 2 Respect for the rights of the defence of anyone who has been\n"
    ":   charged shall be guaranteed.\n"
    "\n"
    "/Article 49\n"
    "/-  -  -  -\n"
    " Principles of legality and proportionality of criminal offences and\n"
    " penalties\n"
    "\n"
    ": 1 No one shall be held guilty of any criminal offence on account of\n"
    ":   any act or omission which did not constitute a criminal offence\n"
    ":   under national law or international law at the time when it was\n"
    ":   committed.  Nor shall a heavier penalty be imposed than that which\n"
    ":   was applicable at the time the criminal offence was committed.\n"
    ":   If, subsequent to the commission of a criminal offence, the law\n"
    ":   provides for a lighter penalty, that penalty shall be applicable.\n"
    ":\n"
    ": 2 This Article shall not prejudice the trial and punishment of any\n"
    ":   person for any act or omission which, at the time when it was\n"
    ":   committed, was criminal according to the general principles\n"
    ":   recognised by the community of nations.\n"
    ":\n"
    ": 3 The severity of penalties must not be disproportionate to the\n"
    ":   criminal offence.\n"
    "\n"
    "/Article 50\n"
    "/-  -  -  -\n"
    " Right not to be tried or punished twice in criminal proceedings for\n"
    " the same criminal offence\n"
    "\n"
    " No one shall be liable to be tried or punished again in criminal\n"
    " proceedings for an offence for which he or she has already been\n"
    " finally acquitted or convicted within the Union in accordance with\n"
    " the law.\n"
    "\n"
    "/CHAPTER VII\n"
    "/- - - - - -\n"
    " GENERAL PROVISIONS\n"
    "\n"
    "/Article 51\n"
    "/-  -  -  -\n"
    " Scope\n"
    "\n"
    ": 1 The provisions of this Charter are addressed to the institutions\n"
    ":   and bodies of the Union with due regard for the principle of\n"
    ":   subsidiarity and to the Member States only when they are\n"
    ":   implementing Union law.  They shall therefore respect the rights,\n"
    ":   observe the principles and promote the application thereof in\n"
    ":   accordance with their respective powers.\n"
    ":\n"
    ": 2 This Charter does not establish any new power or task for the\n"
    ":   Community or the Union, or modify powers and tasks defined by the\n"
    ":   Treaties.\n"
    "\n"
    "/Article 52\n"
    "/-  -  -  -\n"
    " Scope of guaranteed rights\n"
    "\n"
    ": 1 Any limitation on the exercise of the rights and freedoms\n"
    ":   recognised by this Charter must be provided for by law and respect\n"
    ":   the essence of those rights and freedoms.  Subject to the\n"
    ":   principle of proportionality, limitations may be made only if they\n"
    ":   are necessary and genuinely meet objectives of general interest\n"
    ":   recognised by the Union or the need to protect the rights and\n"
    ":   freedoms of others.\n"
    ":\n"
    ": 2 Rights recognised by this Charter which are based on the Community\n"
    ":   Treaties or the Treaty on European Union shall be exercised under\n"
    ":   the conditions and within the limits defined by those Treaties.\n"
    ":\n"
    ": 3 In so far as this Charter contains rights which correspond to\n"
    ":   rights guaranteed by the Convention for the Protection of Human\n"
    ":   Rights and Fundamental Freedoms, the meaning and scope of those\n"
    ":   rights shall be the same as those laid down by the said\n"
    ":   Convention.  This provision shall not prevent Union law providing\n"
    ":   more extensive protection.\n"
    "\n"
    "/Article 53\n"
    "/-  -  -  -\n"
    " Level of protection\n"
    "\n"
    " Nothing in this Charter shall be interpreted as restricting or\n"
    " adversely affecting human rights and fundamental freedoms as\n"
    " recognised, in their respective fields of application, by Union law\n"
    " and international law and by international agreements to which the\n"
    " Union, the Community or all the Member States are party, including\n"
    " the European Convention for the Protection of Human Rights and\n"
    " Fundamental Freedoms, and by the Member States' constitutions.\n"
    "\n"
    "/Article 54\n"
    "/-  -  -  -\n"
    " Prohibition of abuse of rights\n"
    "\n"
    " Nothing in this Charter shall be interpreted as implying any right to\n"
    " engage in any activity or to perform any act aimed at the destruction\n"
    " of any of the rights and freedoms recognised in this Charter or at\n"
    " their limitation to a greater extent than is provided for herein.\n";

    static char * const documents[]  = { document0,
                                         document1,
                                         document2
                                       };
    const int           numDocuments = sizeof documents / sizeof *documents;
//..
// First, we define several aliases to make our code more comprehensible.
//..
    typedef bsl::pair<int, int>                  WordLocation;
        // Document code number ('first') and word offset ('second') in that
        // document specify a word location.  The first word in the document
        // is at word offset 0.

    typedef bsl::unordered_multimap<bsl::string, WordLocation>
                                                 Concordance;
    typedef Concordance::const_iterator          ConcordanceConstItr;
//..
// Next, we create an (empty) unordered map to hold our word tallies.
//..
    Concordance concordance;
//..
// Then, we define the set of characters that define word boundaries:
//..
    const char *delimiters = " \n\t,:;.()[]?!/";
//..
// Next, we extract the words from our documents.  Note that 'strtok' modifies
// the document arrays (which were not made 'const').
//
// As each word is located, we create a map value -- a pair of the word
// converted to a 'bsl::string' and a 'WordLocation' object (itself a pair of
// document code and (word) offset of that word in the document) -- and insert
// the map value into the map.  Note that (unlike maps and unordered maps)
// there is no status to check; the insertion succeeds even if the key is
// already present in the (multi) map.
//..
    for (int idx = 0; idx < numDocuments; ++idx) {
        int wordOffset = 0;
        for (char *cur = strtok(documents[idx], delimiters);
                   cur;
                   cur = strtok(NULL,           delimiters)) {
            WordLocation            location(idx, wordOffset++);
            Concordance::value_type value(bsl::string(cur), location);
            concordance.insert(value);
        }
    }
//..
// Then, we can readily print a complete concordance by interating through the
// map.
//..
    for (ConcordanceConstItr itr  = concordance.begin(),
                             end  = concordance.end();
                             end != itr; ++itr) {
if (verbose) {
        printf("\"%s\", %2d, %4d\n",
               itr->first.c_str(),
               itr->second.first,
               itr->second.second);
}
    }
//..
// Standard output shows:
//..
//  "extent",  2, 3837
//  "greater",  2, 3836
//  "abuse",  2, 3791
//  "constitutions",  2, 3782
//  "affecting",  2, 3727
//  ...
//  "he",  1, 1746
//  "he",  1,  714
//  "he",  0,  401
//  "include",  2,  847
//..
// Next, if there are some particular words of interest, we seek them out using
// the 'equal_range' method of the 'concordance' object:
//..
    const bsl::string wordsOfInterest[] = { "human",
                                            "rights",
                                            "unalienable",
                                            "inalienable"
                                          };
    const int   numWordsOfInterest = sizeof  wordsOfInterest
                                   / sizeof *wordsOfInterest;

    for (int idx = 0; idx < numWordsOfInterest; ++idx) {
       bsl::pair<ConcordanceConstItr,
                 ConcordanceConstItr> found = concordance.equal_range(
                                                         wordsOfInterest[idx]);
       for (ConcordanceConstItr itr  = found.first,
                                end  = found.second;
                                end != itr; ++itr) {
if (verbose) {
           printf("\"%s\", %2d, %4d\n",
                  itr->first.c_str(),
                  itr->second.first,
                  itr->second.second);
}
       }
if (verbose) {
       printf("\n");
}
    }
//..
// Finally, we see on standard output:
//..
//  "human",  2, 3492
//  "human",  2, 2192
//  "human",  2,  534
//  ...
//  "human",  1,   65
//  "human",  1,   43
//  "human",  1,   25
//  "human",  0,   20
//
//  "rights",  2, 3583
//  "rights",  2, 3553
//  "rights",  2, 3493
//  ...
//  "rights",  1,   44
//  "rights",  1,   19
//  "rights",  0,  496
//  "rights",  0,  126
//
//  "unalienable",  0,  109
//
//  "inalienable",  1,   18
//
//..
// {'bslstl_unorderedmap'|Example 3} shows how to use the concordance to create
// an inverse concordance, and how to use the inverse concordance to find the
// context (surrouding words) of a word of interest.
#endif // !defined(DONT_DO_USAGE)
}

//=============================================================================
//                              BREATHING TEST
//-----------------------------------------------------------------------------

namespace BREATHING_TEST {

template<class CONTAINER>
const typename CONTAINER::key_type
keyForValue(const typename CONTAINER::value_type v)
{
//    return v;        // for 'set' containers
    return v.first;  // for 'map' containers
}

template <class CONTAINER>
void testConstEmptyContainer(const CONTAINER& x)
{
    typedef CONTAINER TestType;
    typedef typename TestType::size_type SizeType;

    ASSERT(x.empty());
    ASSERT(0 == x.size());
    ASSERT(0.f == x.load_factor());

    ASSERT(x.begin() == x.end());
    ASSERT(x.cbegin() == x.cend());

    ASSERT(0 == x.count(5));
    ASSERT(x.end() == x.find(5));

    SizeType nBuckets = x.bucket_count();
    for (SizeType i = 0; i != nBuckets; ++i) {
        ASSERT(0 == x.bucket_size(i));
        ASSERT(x.begin(i) == x.end(i));
        ASSERT(x.cbegin(i) == x.cend(i));
    }

    const bsl::pair<typename TestType::const_iterator,
                    typename TestType::const_iterator>
                                                  emptyRange(x.end(), x.end());
    ASSERT(x.equal_range(42) == emptyRange);

    ASSERT(0 == x.count(37));
    ASSERT(x.end() == x.find(26));

    ASSERT(x == x);
    ASSERT(!(x != x));
}

template <class CONTAINER>
void testEmptyContainer(CONTAINER& x)
{
    typedef CONTAINER TestType;
    typedef typename TestType::size_type SizeType;

    ASSERT(x.empty());
    ASSERT(0 == x.size());
    ASSERT(0.f == x.load_factor());

    ASSERT(x.begin() == x.end());
    ASSERT(x.cbegin() == x.cend());
    // Check iterator/const_iterator comparisons compile
    ASSERT(x.begin() == x.cend());
    ASSERT(x.cbegin() == x.end());

    ASSERT(0 == x.count(5));
    ASSERT(x.end() == x.find(5));

    SizeType nBuckets = x.bucket_count();
    for (SizeType i = 0; i != nBuckets; ++i) {
        ASSERT(0 == x.bucket_size(i));
        ASSERT(x.begin(i) == x.end(i));
        ASSERT(x.cbegin(i) == x.cend(i));
    }

    const bsl::pair<typename TestType::iterator, typename TestType::iterator>
                                                  emptyRange(x.end(), x.end());
    ASSERT(x.equal_range(42) == emptyRange);

    ASSERT(0 == x.count(37));
    ASSERT(x.end() == x.find(26));

    typename TestType::iterator it = x.erase(x.begin(), x.end());
                                                          // should not assert

    ASSERT(x.end() == it);

    ASSERT(0 == x.erase(93));

    ASSERT(x == x);
    ASSERT(!(x != x));
}

template <class CONTAINER>
void testContainerHasData(const CONTAINER&                      x,
                          typename CONTAINER::size_type         nCopies,
                          const typename CONTAINER::value_type *data,
                          typename CONTAINER::size_type         size)
{
    typedef          CONTAINER                 TestType;
    typedef typename CONTAINER::size_type      SizeType;
    typedef typename CONTAINER::const_iterator TestIterator;

    LOOP2_ASSERT(x.size(),   nCopies * size,
                 x.size() == nCopies * size);

    for (SizeType i = 0; i != size; ++i) {
        TestIterator it = x.find(keyForValue<CONTAINER>(data[i]));
        ASSERT(x.end() != it);
        ASSERT(*it == data[i]);
        LOOP2_ASSERT(keyForValue<CONTAINER>(data[i]),
                     x.count(keyForValue<CONTAINER>(data[i])),
                     nCopies == x.count(keyForValue<CONTAINER>(data[i])));

        bsl::pair<TestIterator, TestIterator> range =
                                x.equal_range(keyForValue<CONTAINER>(data[i]));
        ASSERT(range.first == it);
        for(SizeType iterations = nCopies; --iterations; ++it) {
            ASSERT(*it == data[i]);
        }
        // Need one extra increment to reach past-the-range iterator.
        if (++it != x.end()) {
            ASSERT(*it != data[i]);
        }
        ASSERT(range.second == it);
    }
}

template <class CONTAINER>
void fillContainerWithData(CONTAINER&                            x,
                           const typename CONTAINER::value_type *data,
                           size_t                                size)
{
    typedef CONTAINER TestType;

    size_t initialSize = x.size();
    x.insert(data, data + size);
    ASSERT(x.size() == initialSize + size);

    for (size_t i = 0; i != size; ++i) {
        typename TestType::iterator it =
                                       x.find(keyForValue<CONTAINER>(data[i]));
        ASSERT(x.end() != it);
        ASSERT(data[i] == *it);
    }
}

template <class CONTAINER>
void validateIteration(CONTAINER &c)
{
    typedef typename CONTAINER::iterator       iterator;
    typedef typename CONTAINER::const_iterator const_iterator;
    typedef typename CONTAINER::size_type      SizeType;

    const SizeType size = c.size();

    SizeType counter = 0;
    for (iterator it = c.begin(); it != c.end(); ++it, ++counter) {}
    ASSERT(size == counter);

    counter = 0;
    for (const_iterator it = c.cbegin(); it != c.cend(); ++it, ++counter) {}
    ASSERT(size == counter);

    const CONTAINER& cc = c;

    counter = 0;
    for (const_iterator it = cc.begin(); it != cc.end(); ++it, ++counter) {}
    ASSERT(size == counter);

    counter = 0;
    for (const_iterator it = cc.cbegin(); it != cc.cend(); ++it, ++counter) {}
    ASSERT(size == counter);
}

template <class CONTAINER>
void testBuckets(CONTAINER& mX)
{
    // Basic test of buckets:
    // number of buckets is returned by bucket_count
    // Number of buckets should reflect load_factor and max_load_factor
    // Each bucket hold a number of elements specified by bucket_size
    // bucket can be iterated from bucket_begin to bucket_end
    //    each element should match the bucket number via bucket(key)
    //    should have as many elements as reported by bucket_count
    // adding elements from all buckets should exactly equal 'size'
    // large buckets imply many hash collisions, which is undesirable
    //    large buckets may be consequence of multicontainers
    typedef typename             CONTAINER::iterator             iterator;
    typedef typename       CONTAINER::const_iterator       const_iterator;
    typedef typename       CONTAINER::local_iterator       local_iterator;
    typedef typename CONTAINER::const_local_iterator const_local_iterator;


    const CONTAINER &x = mX;

    size_t bucketCount = x.bucket_count();
    size_t collisions = 0;
    size_t itemCount  = 0;

    for (unsigned i = 0; i != bucketCount; ++i ) {
        const size_t count = x.bucket_size(i);
        if (0 == count) {
            ASSERT(x.begin(i) == x.end(i));
            ASSERT(mX.begin(i) == mX.end(i));
            ASSERT(mX.cbegin(i) == mX.cend(i));
            // compile test iterator compatibility here, not needed later
            ASSERT(mX.cbegin(i) == mX.end(i));
            ASSERT(mX.begin(i) == mX.cend(i));
            continue;
        }

        itemCount += count;
        collisions += count-1;
        unsigned int bucketItems = 0;
        for (const_local_iterator iter = x.begin(i); iter != x.end(i); ++iter)
        {
            ASSERT(i == x.bucket(keyForValue<CONTAINER>(*iter)));
            ++bucketItems;
        }
        ASSERT(count == bucketItems);

        bucketItems = 0;
        for (local_iterator iter = mX.begin(i); iter != mX.end(i); ++iter) {
            ASSERT(i == x.bucket(keyForValue<CONTAINER>(*iter)));
            ++bucketItems;
        }
        ASSERT(count == bucketItems);

        bucketItems = 0;
        for (const_local_iterator iter = mX.cbegin(i);
             iter != mX.cend(i);
             ++iter)
        {
            ASSERT(i == x.bucket(keyForValue<CONTAINER>(*iter)));
            ++bucketItems;
        }
        ASSERT(count == bucketItems);

    }
    ASSERT(itemCount == x.size());
}


template <class CONTAINER>
void testErase(CONTAINER& mX)
{
    typedef typename CONTAINER::      iterator       iterator;
    typedef typename CONTAINER::const_iterator const_iterator;

    const CONTAINER& x = mX;
    size_t size = x.size();

    // test empty sub-ranges
    ASSERT(x.begin() == mX.erase(x.cbegin(), x.cbegin()));
    ASSERT(x.size() == size);

    ASSERT(x.begin() == mX.erase(x.begin(), x.begin()));
    ASSERT(x.size() == size);

    ASSERT(x.end() == mX.erase(x.cend(), x.cend()));
    ASSERT(x.size() == size);

    ASSERT(x.end() == mX.erase(x.end(), x.end()));
    ASSERT(x.size() == size);

    // eliminate all elements with a matching key
    const_iterator cIter = x.begin();
    bsl::advance(cIter, x.size()/10);
    typename CONTAINER::key_type key = keyForValue<CONTAINER>(*cIter);
    size_t duplicates = x.count(key);
    ASSERT(0 != duplicates);  // already tested, just a sanity check
    bsl::pair<const_iterator, const_iterator> valRange = x.equal_range(key);
    ASSERT(valRange.second != x.end());  // or else container is *tiny*
    ASSERT(mX.erase(valRange.first, valRange.second) == valRange.second);
    ASSERT(x.size() + duplicates == size);
    ASSERT(x.end() == x.find(key));
    validateIteration(mX);

    // cIter is invalidated, so reset and start next sub-test
    // range-erase all matching values for a given key value
    size = x.size();
    cIter = x.begin();
    bsl::advance(cIter, x.size()/5);
    key = keyForValue<CONTAINER>(*cIter);
    duplicates = x.count(key);
    ASSERT(0 != duplicates);  // already tested, just a sanity check
    valRange = x.equal_range(keyForValue<CONTAINER>(*cIter));
    ASSERT(valRange.second != x.end());  // or else container is *tiny*
                            // Will handle subranges terminating in 'end' later
    cIter = valRange.first;
    while (cIter != valRange.second) {
        iterator next = mX.erase(cIter);  // compile check for return type
        cIter = next;
    }
    ASSERT(x.size() + duplicates == size);
    ASSERT(x.end() == x.find(key));
    validateIteration(mX);

    // cIter is invalidated, so reset and start next sub-test
    // range-erase all matching values for a given key value
    size = x.size();
    cIter = x.begin();
    bsl::advance(cIter, x.size()/3);
    key = keyForValue<CONTAINER>(*cIter);
    duplicates = x.count(key);
    ASSERT(0 != duplicates);  // already tested, just a sanity check
    ASSERT(mX.erase(key) == duplicates);
    ASSERT(x.end() == x.find(key));
    ASSERT(x.size() + duplicates == size);
    validateIteration(mX);

    // erase elements straddling a bucket
    // cIter is invalidated, so reset and start next sub-test
    // range-erase all matching values for a given key value
    size = x.size();
    cIter = x.begin();
    key = keyForValue<CONTAINER>(*cIter);
    const_iterator next = cIter;
    while (key == keyForValue<CONTAINER>(*++next)) {
        cIter = next;
    }
    key = keyForValue<CONTAINER>(*next);
    while (key == keyForValue<CONTAINER>(*++next)) {}
    // cIter/next now point to elements either side of a key-range
    // confirm they are not in the same bucket:
    ASSERT(x.bucket(key) != x.bucket(keyForValue<CONTAINER>(*cIter)));
    ASSERT(x.bucket(key) != x.bucket(keyForValue<CONTAINER>(*next)));
    size_t erasures = 0;
    while (cIter != next) {
        cIter = mX.erase(cIter);  // compile check for return type
        ++erasures;
    }
    ASSERT(x.size() + erasures == size);
    ASSERT(x.end() == x.find(key));
    validateIteration(mX);

    // erase elements straddling a bucket, in reverse order
    // cIter is invalidated, so reset and start next sub-test
    // range-erase all matching values for a given key value
    size = x.size();
    cIter = x.begin();
    key = keyForValue<CONTAINER>(*cIter);
    next = cIter;
    while (key == keyForValue<CONTAINER>(*++next)) {
        cIter = next;
    }
    key = keyForValue<CONTAINER>(*next);
    while (key == keyForValue<CONTAINER>(*++next)) {}
    ++next;
    // cIter/next now point to elements either side of a key-range
    // confirm they are not in the same bucket:
    ASSERT(x.bucket(key) != x.bucket(keyForValue<CONTAINER>(*cIter)));
    ASSERT(x.bucket(key) != x.bucket(keyForValue<CONTAINER>(*next)));
    erasures = 0;
    while (cIter != next) {
        const_iterator cursor = cIter;
        const_iterator testCursor = cursor;
        while(++testCursor != next) {
            cursor = testCursor;
        }
        if (cursor == cIter) {
            cIter = mX.erase(cursor);  // compile check for return type
            ASSERT(cIter == next);
            ++erasures;
        }
        else {
            cursor = mX.erase(cursor);  // compile check for return type
            ASSERT(cursor == next);
            ++erasures;
        }
    }
    ASSERT(x.size() + erasures == size);
    ASSERT(x.end() == x.find(key));
    validateIteration(mX);

    // Finally, test erasing the tail of the container, as 'end' is often
    // invalidated.
    // first truncate with a range-based erase
    cIter = mX.erase(cIter, mX.end());  // mixed const/mutable iterators
    ASSERT(cIter == x.end());
    validateIteration(mX);

    // then erase the rest of the container, one item at a time, from the front
    for(iterator it = mX.begin(); it != x.end(); it = mX.erase(it)) {}
    testEmptyContainer(mX);
}

}  // close namespace BREATHING_TEST

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test            = argc > 1 ? atoi(argv[1]) : 0;
    verbose             = argc > 2;
    veryVerbose         = argc > 3;
    veryVeryVerbose     = argc > 4;
    veryVeryVeryVerbose = argc > 5;

    printf("TEST " __FILE__ " CASE %d\n", test);

    bslma::TestAllocator testAlloc("A");
    bslma::Default::setDefaultAllocator(&testAlloc);

    switch (test) { case 0:
      case 17: {
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

        if (verbose) printf("\nUSAGE EXAMPLE"
                            "\n=============\n");
        {
            usage();
        }
      } break;
      case 16: {
        // --------------------------------------------------------------------
        // GROWING FUNCTIONS
        // --------------------------------------------------------------------

        if (verbose) printf("Testing Typedefs\n"
                            "================\n");

        RUN_EACH_TYPE(TestDriver,
                      testCase16,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);
      } break;
      case 15: {
        // --------------------------------------------------------------------
        // GROWING FUNCTIONS
        // --------------------------------------------------------------------

        if (verbose) printf("Testing Growing Functions\n"
                            "=========================\n");

        RUN_EACH_TYPE(TestDriver,
                      testCase15,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);
      } break;
      case 14: {
        // --------------------------------------------------------------------
        // TYPE TRAITS
        // --------------------------------------------------------------------

        if (verbose) printf("Testing Type Traits\n"
                            "===================\n");

        RUN_EACH_TYPE(TestDriver,
                      testCase14,
                      signed char,
                      int,
                      size_t,
                      void *);

        // TBD: all of these types not covered above freak out the bslmf
        // macros, something to do with
        // 'bslstl::HashTable_HashWrapper<FUNCTOR>::d_functor'.
#if 0
        RUN_EACH_TYPE(TestDriver,
                      testCase14,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);
#endif
      } break;
      case 13: {
        // --------------------------------------------------------------------
        // OBJECT WITH STL ALLOCATOR
        // --------------------------------------------------------------------

        if (verbose) printf("Testing Object With STL Allocator\n"
                            "=================================\n");

        RUN_EACH_TYPE(StdAllocTestDriver,
                      testCase13,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);
      } break;
      case 12: {
        // --------------------------------------------------------------------
        // RANGE C'TOR
        // --------------------------------------------------------------------

        if (verbose) printf("Testing RANGE C'TOR\n"
                            "===================\n");

        RUN_EACH_TYPE(TestDriver,
                      testCase12,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);
      } break;
      case 11: {
        // --------------------------------------------------------------------
        // 'g' FUNCTION
        // --------------------------------------------------------------------

        if (verbose) printf("Testing 'g' FUNCTION\n"
                            "====================\n");

        RUN_EACH_TYPE(TestDriver,
                      testCase11,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);
      } break;
      case 10: {
        // --------------------------------------------------------------------
        // BSLX STREAMING -- N/A
        // --------------------------------------------------------------------

        if (verbose) printf("Testing BSLX STREAMING -- N/A\n"
                            "=============================\n");
      } break;
      case 9: {
        // --------------------------------------------------------------------
        // COPY ASSIGNMENT
        // --------------------------------------------------------------------

        if (verbose) printf("Testing Copy Assignment\n"
                            "=======================\n");

        RUN_EACH_TYPE(TestDriver,
                      testCase9,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // Swap
        // --------------------------------------------------------------------

        if (verbose) printf("Testing Swap\n"
                            "============\n");

        RUN_EACH_TYPE(TestDriver,
                      testCase8,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // Copy C'tor
        // --------------------------------------------------------------------

        if (verbose) printf("Testing Copy C'tor\n"
                            "==================\n");

        RUN_EACH_TYPE(TestDriver,
                      testCase7,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // operator==
        // --------------------------------------------------------------------

        if (verbose) printf("Testing operator==\n"
                            "==================\n");

        RUN_EACH_TYPE(TestDriver,
                      testCase6,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // operator<< -- N/A
        // --------------------------------------------------------------------

        if (verbose) printf("Testing operator<< -- N/A\n"
                            "=========================\n");
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // BASIC ACCESSORS
        // --------------------------------------------------------------------

        if (verbose) printf("Testing Basic Accessors\n"
                            "=======================\n");

        RUN_EACH_TYPE(TestDriver,
                      testCase4,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // Range c'tor, 'ggg', 'gg', and 'verifySpec' functions.
        // --------------------------------------------------------------------

        if (verbose) printf(
                       "Testing Range c'tor, 'ggg', 'gg', and 'verifySpec'\n"
                       "==================================================\n");

        RUN_EACH_TYPE(TestDriver,
                      testCase3,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // PRIMARY MANIPULATORS
        // --------------------------------------------------------------------

        if (verbose) printf("Testing Primary Manipulators\n"
                            "============================\n");

        RUN_EACH_TYPE(TestDriver,
                      testCase2,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //
        // Concerns:
        //   That basic functionality appears to work as advertised before
        //   before beginning testing in earnest:
        //   - default and copy constructors
        //   - assignment operator
        //   - primary manipulators, basic accessors
        //   - 'operator==', 'operator!='
        //
        // Plan:
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) printf("\nBREATHING TEST"
                            "\n==============\n");

        using namespace BREATHING_TEST;

        typedef bsl::unordered_multimap<int, int > TestType;

        if (veryVerbose)
            printf("Default construct an unordered_mutlimap, 'x'\n");

        TestType mX;
        const TestType &x = mX;

        if (veryVerbose) printf("Validate default behavior of 'x'\n");

        ASSERT(1.0f == x.max_load_factor());

        testConstEmptyContainer(x);
        testEmptyContainer(mX);

        swap(mX, mX);

        testConstEmptyContainer(x);
        testEmptyContainer(mX);

        validateIteration(mX);

        if (veryVerbose) printf("Prepare a test array of data samples\n");

        typedef TestType::value_type BaseValue;
        const int MAX_SAMPLE = 1000;
        BaseValue *dataSamples = new BaseValue[MAX_SAMPLE];
        for(int i = 0; i != MAX_SAMPLE; ++i) {
            new(&dataSamples[i]) BaseValue(i, i*i);  // inplace-new needed to
                                                     // supply 'const' key
        }

        if (veryVerbose)  printf(
          "Range-construct an unordered_multimap, 'y', from the test array\n");

        TestType mY(dataSamples, dataSamples + MAX_SAMPLE);
        const TestType &y = mY;

        if (veryVerbose)
            printf("Validate behavior of freshly constructed 'y'\n");

        ASSERT(1.0f == y.max_load_factor());

        testContainerHasData(y, 1, dataSamples, MAX_SAMPLE);
        validateIteration(mY);

        if (veryVerbose)
            printf("Assert equality relationships, noting 'x != y'\n");

        ASSERT(x == x);
        ASSERT(!(x != x));
        ASSERT(y != x);
        ASSERT(!(y == x));
        ASSERT(x != y);
        ASSERT(!(x == y));
        ASSERT(y == y);
        ASSERT(!(y != y));

        if (veryVerbose) printf("Swap 'x' and 'y'\n");

        swap(mX, mY);

        if (veryVerbose) printf("Validate swapped values\n");

        testConstEmptyContainer(y);
        testEmptyContainer(mY);

        validateIteration(mY);

        testContainerHasData(x, 1, dataSamples, MAX_SAMPLE);
        validateIteration(mX);

        if (veryVerbose)
            printf("Assert swapped equality relationships, noting 'x != y'\n");

        ASSERT(x == x);
        ASSERT(!(x != x));
        ASSERT(y != x);
        ASSERT(!(y == x));
        ASSERT(x != y);
        ASSERT(!(x == y));
        ASSERT(y == y);
        ASSERT(!(y != y));

        if (veryVerbose) printf("Try to fill 'x' with duplicate values\n");

        for (int i = 0; i != MAX_SAMPLE; ++i) {
            TestType::iterator it = mX.insert(dataSamples[i]);
            ASSERT(x.end() != it);
            ASSERT(*it == dataSamples[i]);
        }
        validateIteration(mX);

        if (veryVerbose) printf(
          "Confirm the value of 'x' with the successfully inserted values.\n");

        testContainerHasData(x, 2, dataSamples, MAX_SAMPLE);

        if (veryVerbose) printf(
                "Create an unordered_multimap, 'z', that is a copy of 'x'.\n");

        TestType mZ = x;
        const TestType &z = mZ;

        if (veryVerbose)
            printf("Validate behavior of freshly constructed 'z'.\n");

        ASSERT(1.0f == z.max_load_factor());
        ASSERT(x == z);
        ASSERT(!(x != z));
        ASSERT(z == x);
        ASSERT(!(z != x));
        testContainerHasData(z, 2, dataSamples, MAX_SAMPLE);

        if (veryVerbose) printf(
                             "Expand 'z' with additional duplicate values.\n");

        validateIteration(mZ);
        for (int i = 3; i != 6; ++i) {
            fillContainerWithData(mZ, dataSamples, MAX_SAMPLE);
            testContainerHasData(z, i, dataSamples, MAX_SAMPLE);
            validateIteration(mZ);
        }
        ASSERT(x != z);
        ASSERT(!(x == z));
        ASSERT(z != x);
        ASSERT(!(z == x));


        if (veryVerbose)
            printf("Confirm that 'x' is unchanged by making the copy.\n");

        testBuckets(mX);
        validateIteration(mX);
        testContainerHasData(x, 2, dataSamples, MAX_SAMPLE);

        if (veryVerbose)
            printf("Clear 'x' and confirm that it is empty.\n");

        mX.clear();
        testEmptyContainer(mX);
        testBuckets(mX);

        if (veryVerbose)
            printf("Assign the value of 'y' to 'x'.\n");

        mX = y;

        if (veryVerbose) printf("Confirm 'x' has the expected value.\n");

        ASSERT(x == y);

        validateIteration(mX);
        testBuckets(mX);

        testErase(mZ);

        if (veryVerbose) printf(
             "Call any remaining methods to be sure they at least compile.\n");

        mX.insert(bsl::pair<const int, int>(1, 1));

        const bsl::allocator<int> alloc   = x.get_allocator();
        const bsl::hash<int>      hasher  = x.hash_function();
        const bsl::equal_to<int>  compare = x.key_eq();

        (void) alloc;
        (void) hasher;
        (void) compare;

        const size_t maxSize    = x.max_size();
        (void) maxSize;
        const size_t buckets    = x.bucket_count();
        const float  loadFactor = x.load_factor();
        const float  maxLF      = x.max_load_factor();

        ASSERT(loadFactor < maxLF);

        mX.rehash(2 * buckets);
        ASSERTV(x.bucket_count(), 2 * buckets, x.bucket_count() > 2 * buckets);
        ASSERTV(x.load_factor(), loadFactor, x.load_factor() < loadFactor);

        mX.reserve(0);
        ASSERTV(x.bucket_count(), 2 * buckets, x.bucket_count() > 2 * buckets);
        ASSERTV(x.load_factor(), loadFactor, x.load_factor() < loadFactor);

        if (veryVerbose)
            printf("Final message to confim the end of the breathing test.\n");
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
}  // Empty test driver

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
