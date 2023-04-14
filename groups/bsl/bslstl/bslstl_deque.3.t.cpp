// bslstl_deque.3.t.cpp                                               -*-C++-*-
#define BSLSTL_DEQUE_0T_AS_INCLUDE
#include <bslstl_deque.0.t.cpp>

// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
// See the test plan in 'bslstl_deque.0.t.cpp'.

// ============================================================================
//              ADDITIONAL TEST MACROS FOR THIS TEST DRIVER
// ----------------------------------------------------------------------------

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP17_BOOL_CONSTANT)
# define DECLARE_BOOL_CONSTANT(NAME, EXPRESSION)                              \
    const BSLS_KEYWORD_CONSTEXPR bsl::bool_constant<EXPRESSION> NAME{}
    // This leading branch is the preferred version for C++17, but the feature
    // test macro is (currently) for documentation purposes only, and never
    // defined.  This is the ideal (simplest) form for such declarations:
#elif defined(BSLS_COMPILERFEATURES_SUPPORT_CONSTEXPR)
# define DECLARE_BOOL_CONSTANT(NAME, EXPRESSION)                              \
    constexpr bsl::integral_constant<bool, EXPRESSION> NAME{}
    // This is the preferred C++11 form for the definition of integral constant
    // variables.  It assumes the presence of 'constexpr' in the compiler as an
    // indication that brace-initialization and traits are available, as it has
    // historically been one of the last C++11 features to ship.
#else
# define DECLARE_BOOL_CONSTANT(NAME, EXPRESSION)                              \
    static const bsl::integral_constant<bool, EXPRESSION> NAME =              \
                 bsl::integral_constant<bool, EXPRESSION>()
    // 'bsl::integral_constant' is not an aggregate prior to C++17 extending
    // the rules, so a C++03 compiler must explicitly initialize integral
    // constant variables in a way that is unambiguously not a vexing parse
    // that declares a function instead.
#endif

// ============================================================================
//                    MOVE SPECIFIC CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

// Define values used to initialize positional arguments for
// 'bsltf::EmplacableTestType' and 'bsltf::AllocEmplacableTestType'
// constructors.  Note, that you cannot change those values as they are used by
// 'TemplateTestFacility::getIdentifier' to map the constructed emplacable
// objects to their integer identifiers.

static const int V01 = 1;
static const int V02 = 20;
static const int V03 = 23;
static const int V04 = 44;
static const int V05 = 66;
static const int V06 = 176;
static const int V07 = 878;
static const int V08 = 8;
static const int V09 = 912;
static const int V10 = 102;

// ============================================================================
//                       MOVE SPECIFIC TEST APPARATUS
// ----------------------------------------------------------------------------

template <class ITER, class VALUE_TYPE>
struct TestMovableTypeUtil {
    static ITER findFirstNotMovedInto(ITER begin, ITER end)
    {
        typedef bsltf::TemplateTestFacility TstFacility;
        typedef bsltf::MoveState            MoveState;

        for (; begin != end; ++begin) {
            MoveState::Enum mState = TstFacility::getMovedIntoState(*begin);
            if (MoveState::e_NOT_MOVED == mState) {
                break;
            }
        }
        return begin;
    }
};

struct TestAllocatorUtil {

  public:
    // CLASS METHODS
    template <class TYPE>
    static void test(const TYPE&, const bslma::Allocator&)
    {
    }

    static void test(const bsltf::AllocEmplacableTestType& value,
                     const bslma::Allocator&               allocator)
    {
        ASSERTV(&allocator == value.arg01().allocator());
        ASSERTV(&allocator == value.arg02().allocator());
        ASSERTV(&allocator == value.arg03().allocator());
        ASSERTV(&allocator == value.arg04().allocator());
        ASSERTV(&allocator == value.arg05().allocator());
        ASSERTV(&allocator == value.arg06().allocator());
        ASSERTV(&allocator == value.arg07().allocator());
        ASSERTV(&allocator == value.arg08().allocator());
        ASSERTV(&allocator == value.arg09().allocator());
        ASSERTV(&allocator == value.arg10().allocator());
    }
};

template <class CONTAINER, class VALUES>
size_t verifyContainer(const CONTAINER& container,
                       const VALUES&    expectedValues,
                       size_t           expectedSize)
    // Verify the specified 'container' has the specified 'expectedSize' and
    // contains the same values as the array in the specified 'expectedValues'.
    // Return 0 if 'container' has the expected values, and a non-zero value
    // otherwise.
{
    ASSERTV(expectedSize, container.size(), expectedSize == container.size());

    if (expectedSize != container.size()) {
        return static_cast<size_t>(-1);                               // RETURN
    }

    typename CONTAINER::const_iterator it = container.cbegin();
    for (size_t i = 0; i < expectedSize; ++i) {
        ASSERTV(it != container.cend());
        ASSERTV(i, expectedValues[i], *it, expectedValues[i] == *it);

        if (bsltf::TemplateTestFacility::getIdentifier(expectedValues[i])
            != bsltf::TemplateTestFacility::getIdentifier(*it)) {
            return i + 1;                                             // RETURN
        }
        ++it;
    }
    return 0;
}

template <class CONTAINER>
size_t numMovedInto(const CONTAINER& X,
                    size_t           startIndex = 0,
                    size_t           endIndex   = 0)
{
    typedef bsltf::TemplateTestFacility TstFacility;
    typedef bsltf::MoveState            MoveState;

    int numMoved = 0;
    size_t upTo = endIndex > startIndex ? endIndex : X.size();
    for (size_t i = startIndex; i < upTo; ++i) {
        MoveState::Enum mState = TstFacility::getMovedIntoState(X[i]);
        if (MoveState::e_MOVED == mState || MoveState::e_UNKNOWN == mState) {
            ++numMoved;
        }
    }
    return numMoved;
}

template <class CONTAINER>
size_t numNotMovedInto(const CONTAINER& X,
                       size_t           startIndex = 0,
                       size_t           endIndex   = 0)
{
    typedef bsltf::TemplateTestFacility TstFacility;
    typedef bsltf::MoveState            MoveState;

    int numNotMoved = 0;
    size_t upTo = endIndex > startIndex ? endIndex : X.size();
    for (size_t i = startIndex; i < upTo; ++i) {
        MoveState::Enum mState = TstFacility::getMovedIntoState(X[i]);
        if (MoveState::e_NOT_MOVED == mState
         || MoveState::e_UNKNOWN   == mState) {
            ++numNotMoved;
        }
    }
    return numNotMoved;
}

                             // ============
                             // class EqPred
                             // ============

template <class TYPE>
struct EqPred
    // A predicate for testing 'erase_if'; it takes a value at construction
    // and uses it for comparisons later.
{
    TYPE d_ch;
    EqPred(TYPE ch) : d_ch(ch) {}

    bool operator() (TYPE ch) const
        // return 'true' if the specified 'ch' is equal to the stored value,
        // and 'false' otherwise.
    {
        return d_ch == ch;
    }
};

// ============================================================================
//                       TEST DRIVER TEMPLATE
// ----------------------------------------------------------------------------

template <class TYPE, class ALLOC = bsl::allocator<TYPE> >
struct TestDriver3 : TestSupport<TYPE, ALLOC> {

                     // NAMES FROM DEPENDENT BASE

    BSLSTL_DEQUE_0T_PULL_TESTSUPPORT_NAMES;
    // Unfortunately the names have to be made available "by hand" due to two
    // phase name lookup not reaching into dependent bases.

                        // MOVE SPECIFIC TYPES

    // PUBLIC TYPES
    typedef TestMovableTypeUtil<CIter, TYPE>      TstMoveUtil;

                     // PART SPECIFIC TEST APPARATUS

    // CLASS METHODS
    static void storeFirstNElemAddr(typename Obj::const_pointer *pointers,
                                    const Obj&                   object,
                                    size_t                       n)
    {
        size_t i = 0;
        for (CIter b = object.cbegin(); b != object.cend() && i < n; ++b) {
            pointers[i++] = bsls::Util::addressOf(*b);
        }
    }

    static
    int checkFirstNElemAddr(typename Obj::const_pointer *pointers,
                            const Obj&                   object,
                            size_t                       n)
    {
        int    count = 0;
        size_t i     = 0;
        for (CIter b = object.cbegin(); b != object.end() && i < n; ++b) {
            if (pointers[i++] != bsls::Util::addressOf(*b)) {
                ++count;
            }
        }
        return count;
    }

    template <class T>
    static bslmf::MovableRef<T> testArg(T& t, bsl::true_type)
    {
        return MoveUtil::move(t);
    }
    template <class T>
    static const T&             testArg(T& t, bsl::false_type)
    {
        return t;
    }

                               // TEST CASES

    static void testCase33();
        // test 'bsl::erase' and 'bsl::erase_if' with 'bsl::deque'.

    static void testCase31();
        // Test 'noexcept' specifications

    static void testCase30();
        // Test 'shrink_to_fit'.

    static void testCase29();
        // Test methods that take an initializer list.

    static void testCase28a();
        // Test forwarding of arguments in 'emplace' method.

    template <int N_ARGS,
              int N01,
              int N02,
              int N03,
              int N04,
              int N05,
              int N06,
              int N07,
              int N08,
              int N09,
              int N10>
    static void testCase28a_RunTest(Obj *target, const_iterator position);
        // Call 'emplace' on the specified 'target' container at the specified
        // 'position'.  Forward (template parameter) 'N_ARGS' arguments to the
        // 'emplace' method and ensure 1) that values are properly passed to
        // the constructor of 'value_type', 2) that the allocator is correctly
        // configured for each argument in the newly inserted element in
        // 'target', and 3) that the arguments are forwarded using copy or move
        // semantics based on integer template parameters '[N01 .. N10]'.

    static void testCase28();
        // Test 'emplace' other than forwarding of arguments (see 29a).

    template <int N_ARGS,
              int N01,
              int N02,
              int N03,
              int N04,
              int N05,
              int N06,
              int N07,
              int N08,
              int N09,
              int N10>
    static void testCase27a_RunTest(Obj *target);
        // Call 'emplace_back' on the specified 'target' container.  Forward
        // (template parameter) 'N_ARGS' arguments to the 'emplace_back' method
        // and ensure 1) that values are properly passed to the constructor of
        // 'value_type', 2) that the allocator is correctly configured for each
        // argument in the newly inserted element in 'target', and 3) that the
        // arguments are forwarded using copy or move semantics based on
        // integer template parameters '[N01 .. N10]'.

    template <int N_ARGS,
              int N01,
              int N02,
              int N03,
              int N04,
              int N05,
              int N06,
              int N07,
              int N08,
              int N09,
              int N10>
    static void testCase27b_RunTest(Obj *target);
        // Call 'emplace_front' on the specified 'target' container.  Forward
        // (template parameter) 'N_ARGS' arguments to the 'emplace_front'
        // method and ensure 1) that values are properly passed to the
        // constructor of 'value_type', 2) that the allocator is correctly
        // configured for each argument in the newly inserted element in
        // 'target', and 3) that the arguments are forwarded using copy or move
        // semantics based on integer template parameters '[N01 .. N10]'.

    static void testCase27a();
        // Test forwarding of arguments in 'emplace_back' method.

    static void testCase27b();
        // Test forwarding of arguments in 'emplace_front' method.

    static void testCase27();
        // Test 'emplace_front' and 'emplace_back' other than forwarding of
        // arguments (see 28a and 28b).

    static void testCase26();
        // Test 'insert' method that takes a movable ref.

    static void testCase25();
        // Test 'push_front' and 'push_back' methods that take a movable ref.

    static void testCase24_move_assignment_noexcept();
        // Test noexcept specification of move assignment operator.

    static void testCase24_dispatch();
        // Test move-assignment operator.

    static void testCase23();
        // Test move constructor.
};

                  // ==================================
                  // template class StdBslmaTestDriver3
                  // ==================================

template <class TYPE>
class StdBslmaTestDriver3 : public StdBslmaTestDriverHelper<TestDriver3, TYPE>
{
};

                                 // ----------
                                 // TEST CASES
                                 // ----------

template <class TYPE, class ALLOC>
void TestDriver3<TYPE, ALLOC>::testCase33()
    // test 'bsl::erase' and 'bsl::erase_if' with 'bsl::deque'.
{
    static const struct {
        int         d_line;       // source line number
        const char *d_initial_p;  // initial values
        char        d_element;    // value to remove
        const char *d_results_p;  // expected result value
    } DATA[] = {
        //line  initial              element  results
        //----  -------------------  -------  -------------------
        { L_,   "",                  'A',     ""                  },
        { L_,   "A",                 'A',     ""                  },
        { L_,   "A",                 'B',     "A"                 },
        { L_,   "B",                 'A',     "B"                 },
        { L_,   "AB",                'A',     "B"                 },
        { L_,   "BA",                'A',     "B"                 },
        { L_,   "BC",                'D',     "BC"                },
        { L_,   "ABC",               'C',     "AB"                },
        { L_,   "CBADEABCDAB",       'B',     "CADEACDA"          },
        { L_,   "CBADEABCDABCDEA",   'E',     "CBADABCDABCDA"     },
        { L_,   "ZZZZZZZZZZZZZZZZ",  'Z',     ""                  }
    };
    enum { NUM_DATA = sizeof DATA / sizeof *DATA };

    for (size_t i = 0; i < NUM_DATA; ++i)
    {
        int         LINE = DATA[i].d_line;
        const char *initial = DATA[i].d_initial_p;
        size_t      initialLen = strlen(initial);
        const char *results = DATA[i].d_results_p;
        size_t      resultsLen = strlen(results);

        Obj    v1(initial, initial + initialLen);
        Obj    v2(initial, initial + initialLen);
        Obj    vres(results, results + resultsLen);
        size_t ret1 = bsl::erase   (v1, DATA[i].d_element);
        size_t ret2 = bsl::erase_if(v2, EqPred<TYPE>(DATA[i].d_element));

        // Are the modified containers correct?
        ASSERTV(LINE, v1 == vres);
        ASSERTV(LINE, v2 == vres);

        // Are the return values correct?
        ASSERTV(LINE, ret1 == initialLen - resultsLen);
        ASSERTV(LINE, ret2 == initialLen - resultsLen);
    }
}

template <class TYPE, class ALLOC>
void TestDriver3<TYPE,ALLOC>::testCase31()
{
    // ------------------------------------------------------------------------
    // 'noexcept' SPECIFICATION
    //
    // Concerns:
    //: 1 The 'noexcept' specification has been applied to all class interfaces
    //:   required by the standard.
    //
    // Plan:
    //: 1 Apply the uniary 'noexcept' operator to expressions that mimic those
    //:   appearing in the standard and confirm that calculated boolean value
    //:   matches the expected value.
    //:
    //: 2 Since the 'noexcept' specification does not vary with the 'TYPE'
    //:   of the container, we need test for just one general type and any
    //:   'TYPE' specializations.
    //
    // Testing:
    //   CONCERN: Methods qualified 'noexcept' in standard are so implemented.
    // ------------------------------------------------------------------------

    if (verbose) {
        P(bsls::NameOf<Obj>())
        P(bsls::NameOf<TYPE>())
        P(bsls::NameOf<ALLOC>())
    }

    // N4594: page 835: 23.3.8 Class template 'deque'

    // page 835
    //..
    //  // 23.3.8.2, construct/copy/destroy:
    //  deque& operator=(deque&& x)
    //      noexcept(allocator_traits<Allocator>::is_always_equal::value);
    //  allocator_type get_allocator() const noexcept;
    //..

    {
        Obj d;    (void) d;
        Obj x;    (void) x;

        ASSERT(false
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR(d = MoveUtil::move(x)));

        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR(d.get_allocator()));
    }

    // page 836
    //..
    //  // iterators:
    //  iterator begin() noexcept;
    //  const_iterator begin() const noexcept;
    //  iterator end() noexcept;
    //  const_iterator end() const noexcept;
    //  reverse_iterator rbegin() noexcept;
    //  const_reverse_iterator rbegin() const noexcept;
    //  reverse_iterator rend() noexcept;
    //  const_reverse_iterator rend() const noexcept;
    //  const_iterator cbegin() const noexcept;
    //  const_iterator cend() const noexcept;
    //  const_reverse_iterator crbegin() const noexcept;
    //  const_reverse_iterator crend() const noexcept;
    //..

    {
        Obj d; const Obj& D = d;    (void) D;

        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR(d.begin()));
        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR(D.begin()));
        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR(d.end()));
        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR(D.end()));
        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR(D.begin()));
        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR(d.rbegin()));
        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR(D.rbegin()));
        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR(d.rend()));
        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR(D.rend()));
        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR(D.cbegin()));
        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR(D.cend()));
        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR(D.crbegin()));
        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR(D.crend()));
    }

    // page 836
    //..
    //  // 23.3.8.3, capacity:
    //  bool empty() const noexcept;
    //  size_type size() const noexcept;
    //  size_type max_size() const noexcept;
    //..

    {
        Obj d;

        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR(d.empty()));
        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR(d.size()));
        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR(d.max_size()));
    }

    // page 836
    //..
    //  // 23.3.8.4, modifiers:
    //  void swap(deque&)
    //  noexcept(allocator_traits<Allocator>::is_always_equal::value);
    //  void clear() noexcept;
    //..

    {
        Obj d;
        Obj x;

        ASSERT(false
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR(d.swap(x)));

        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR(d.clear()));
    }

    // page 837
    //..
    //  // 23.3.8.5, specialized algorithms:
    //  template <class T, class Allocator>
    //  void swap(deque<T, Allocator>& x, deque<T, Allocator>& y)
    //      noexcept(noexcept(x.swap(y)));
    //..

    {
        Obj x;
        Obj y;

        ASSERT(false
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR(swap(x, y)));
    }
}

template <class TYPE, class ALLOC>
void TestDriver3<TYPE, ALLOC>::testCase30()
{
    // ------------------------------------------------------------------------
    // TESTING 'shrink_to_fit'
    //
    // Concerns:
    //: 1 'shrink_to_fit' either reduces memory use or leaves it unchanged
    //:   (i.e., it never increases memory use).
    //:
    //: 2 'shrink_to_fit' either reduces capacity or leaves it unchanged
    //:   (i.e., it never increases capacity).
    //:
    //: 3 'shrink_to_fit' has no effect on object value.
    //:
    //: 4 'shrink_to_fit' does not move any elements; consequently, iterators
    //:   and references to elements remain valid.  (TBD not yet tested)
    //:
    //: 5 Following a call to 'shrink_to_fit', subsequent calls have no effect,
    //:   provided there are no intervening requests to insert or erase one or
    //:   more elements, or calls to 'reserve'.
    //:
    //: 6 'shrink_to_fit' yields the same memory use and capacity when invoked
    //:   on an empty object as when invoked on a default-constructed object.
    //:
    //: 7 All memory allocations, if any, are from the object allocator.
    //:
    //: 8 (white-box) 'shrink_to_fit' does at most one allocation.
    //:
    //: 9 'shrink_to_fit' provides the strong exception guarantee.
    //
    // Plan:
    //: 1 For an initial breathing test, call 'shrink_to_fit' on a
    //:   default-constructed object and verify that the change in capacity and
    //:   memory use is as expected.
    //:
    //: 2 Using the table-based approach, specify a set of initial container
    //:   sizes and the number of elements to erase ("delta") at the end of
    //:   each test iteration.                                   (C-1..3, 5..8)
    //:
    //:   1 For each row, 'R', from P-2, create an object, 'X', having the
    //:     specified initial size; create a second ("control") object, 'Y',
    //:     having the same initial value as 'X'.
    //:
    //:   2 Invoke 'shrink_to_fit' twice on 'X' and verify (following each
    //:     invocation) object value, capacity, and memory use are as expected.
    //:                                                             (C-1..3, 5)
    //:
    //:   3 Verify all allocations, if any, are from the object's allocator.
    //:                                                                (C-7..8)
    //:
    //:   4 Erase from 'X' (and 'Y') the number of elements specified in 'R'.
    //:
    //:   5 Repeat P-2.2..2.4 until 'X' is empty.
    //:
    //:   6 Verify that the now empty 'X' has the same capacity and memory use
    //:     as that of a shrunken default-constructed object.             (C-6)
    //:
    //: 3 Perform all calls to 'shrink_to_fit' in the presence of injected
    //:   exceptions.                                                     (C-9)
    //
    // Testing:
    //   void shrink_to_fit();
    // ------------------------------------------------------------------------

    Int64 minimumBytesInUse = 0;  // after 'shrink_to_fit' called on empty
    Int64 minimumCapacity   = 0;  //   "          "          "    "    "

    if (verbose) printf("\tTesting 'shrink_to_fit'.\n");

    if (verbose) printf("\t\tOn a default-constructed object.\n");
    {
        bslma::TestAllocator da("default", veryVeryVeryVerbose);
        bslma::TestAllocator oa("object",  veryVeryVeryVerbose);
        bslma::TestAllocator za("other",   veryVeryVeryVerbose);
        ALLOC                xoa(&oa);
        ALLOC                xza(&za);

        bslma::DefaultAllocatorGuard dag(&da);

        Obj mX(xoa);  const Obj& X = mX;

        const Obj Y(xza);  // control

        const Int64 BMEM = oa.numBytesInUse();
        const Int64 BCAP = X.capacity();

        if (veryVerbose) { printf("\t\tBEFORE: "); P_(BMEM); P(BCAP); }

        BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
            mX.shrink_to_fit();
        } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

        const Int64 AMEM = oa.numBytesInUse();
        const Int64 ACAP = X.capacity();

        if (veryVerbose) { printf("\t\tAFTER : "); P_(AMEM); P(ACAP); }

        // save minimum values for use in later testing

        minimumBytesInUse = AMEM;
        minimumCapacity   = ACAP;

        ASSERT(   X == Y);
        ASSERT(AMEM <  BMEM);
        ASSERT(ACAP <= BCAP);

        {
            bslma::TestAllocatorMonitor oam(&oa);

            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                mX.shrink_to_fit();
            } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

            ASSERT(   X == Y);
            ASSERT(AMEM == oa.numBytesInUse());
            ASSERT(ACAP == (Int64) X.capacity());

            ASSERT(oam.isTotalSame());
        }

        ASSERT(0 == da.numBlocksTotal());
    }

    if (verbose) printf("\t\tOn objects of various sizes and capacities.\n");
    {
        const TestValues VALUES;
        const size_t     NUM_VALUES = VALUES.size();

        // Powers of 2 are not significant for deque, but we want to test
        // large numbers.

        static const struct {
            int    d_lineNum;      // source line number
            size_t d_numElements;  // initial number of elements
            int    d_delta;        // no. of elements to 'erase' each iteration
        } DATA[] = {
            //line   numElements   delta
            //----   -----------   -----
            { L_,         0,          0   },
            { L_,         1,          1   },
            { L_,         2,          1   },
            { L_,        15,          3   },
            { L_,        32,         10   },
            { L_,        99,         15   },
            { L_,       229,         25   },
            { L_,      2000,        100   }
        };
        enum { NUM_DATA = sizeof DATA / sizeof *DATA };

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int    LINE  = DATA[ti].d_lineNum;
            const size_t NE    = DATA[ti].d_numElements;
            const size_t DELTA = DATA[ti].d_delta;

            if (veryVerbose) { T_ P_(LINE) P_(NE) P(DELTA) }

            bslma::TestAllocator da("default", veryVeryVeryVerbose);
            bslma::TestAllocator oa("object",  veryVeryVeryVerbose);
            bslma::TestAllocator za("other",   veryVeryVeryVerbose);
            ALLOC                xoa(&oa);
            ALLOC                xza(&za);

            bslma::DefaultAllocatorGuard dag(&da);

            Obj mX(xoa);  const Obj& X = mX;
            Obj mY(xza);  const Obj& Y = mY;  // control

            stretch(&mX, NE,
                    TstFacility::getIdentifier(VALUES[ti % NUM_VALUES]));
            stretch(&mY, NE,
                    TstFacility::getIdentifier(VALUES[ti % NUM_VALUES]));

            bool eraseAtFront = true;  // alternate erasing at front and back

            while (1) {
                const Int64 BALLOC = oa.numAllocations();
                const Int64 BMEM   = oa.numBytesInUse();
                const Int64 BCAP   = X.capacity();

                if (veryVerbose) {
                    printf("\t\tBEFORE: "); P_(BALLOC); P_(BMEM); P(BCAP);
                }

                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                    mX.shrink_to_fit();
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                const Int64 AALLOC = oa.numAllocations();
                const Int64 AMEM   = oa.numBytesInUse();
                const Int64 ACAP   = X.capacity();

                if (veryVerbose) {
                    printf("\t\tAFTER : "); P_(AALLOC); P_(AMEM); P(ACAP);
                }

                ASSERT(   X == Y);
                ASSERT(AMEM <= BMEM);
                ASSERT(ACAP <= BCAP);

                // +2 to account for the allocation (if any) that throws in the
                // above exception testing block.

#if defined(BDE_BUILD_TARGET_EXC)
                const Int64 EXC_EXTRA = 2;
#else
                const Int64 EXC_EXTRA = 1;
#endif

                ASSERTV(BALLOC, AALLOC,
                        AALLOC == BALLOC || AALLOC == BALLOC + EXC_EXTRA);

                {
                    bslma::TestAllocatorMonitor oam(&oa);

                    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                        mX.shrink_to_fit();
                    } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                    ASSERT(   X == Y);
                    ASSERT(AMEM == oa.numBytesInUse());
                    ASSERT(ACAP == (Int64) X.capacity());

                    ASSERT(oam.isTotalSame());
                }

                if (X.empty()) {
                    break;
                }

                if (DELTA >= X.size()) {
                    mX.clear();
                    mY.clear();
                }
                else if (eraseAtFront) {
                    mX.erase(X.cbegin(), X.cbegin() + DELTA);
                    mY.erase(Y.cbegin(), Y.cbegin() + DELTA);
                }
                else {
                    mX.erase(X.cend() - DELTA, X.cend());
                    mY.erase(Y.cend() - DELTA, Y.cend());
                }

                ASSERT(X == Y);
                ASSERT(0 == da.numBlocksTotal());
            }
            ASSERT(X.empty() && Y.empty());
            ASSERT(0 == da.numBlocksTotal());

            ASSERT(minimumBytesInUse == oa.numBytesInUse());
            ASSERT(minimumCapacity   == (Int64) X.capacity());
        }
    }
}

template <class TYPE, class ALLOC>
void TestDriver3<TYPE, ALLOC>::testCase29()
{
    // ------------------------------------------------------------------------
    // TESTING METHODS TAKING INITIALIZER LISTS
    //
    // Concerns:
    //: 1 The methods that take an initializer list (constructor, assignment
    //:   operator, 'assign', and 'insert') simply forward to other already
    //:   tested methods.  We are interested here only in ensuring that the
    //:   forwarding is working -- not retesting already verified
    //:   functionality.
    //
    // Plan:
    //: TBD
    //
    // Testing:
    //   deque(initializer_list<T>, const A& = A());
    //   deque& operator=(initializer_list<T>);
    //   void assign(initializer_list<T>);
    //   iterator insert(const_iterator pos, initializer_list<T>);
    // ------------------------------------------------------------------------

#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
    const TestValues V;

    bslma::TestAllocator da("default", veryVeryVeryVerbose);
    bslma::DefaultAllocatorGuard dag(&da);

    if (verbose) printf("\nTesting constructor with initializer lists.\n");
    {
        const struct {
            int                          d_line;      // source line number
            std::initializer_list<TYPE>  d_list;      // source list
            const char                  *d_result_p;  // expected result
        } DATA[] = {
                //line          list             result
                //----          ----             ------
                { L_,   {                  },        ""   },
                { L_,   { V[0]             },       "A"   },
                { L_,   { V[0], V[0]       },      "AA"   },
                { L_,   { V[1], V[0]       },      "BA"   },
                { L_,   { V[0], V[1], V[2] },     "ABC"   },
                { L_,   { V[0], V[1], V[0] },     "ABA"   },
        };
        enum { NUM_DATA = sizeof DATA / sizeof *DATA };

        bslma::TestAllocator oa("object", veryVeryVeryVerbose);
        ALLOC                xoa(&oa);
        bslma::TestAllocatorMonitor dam(&da);

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            ASSERT(0 == oa.numBytesInUse());

            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);
            ALLOC                xscratch(&scratch);

            Obj mY(xscratch);  const Obj& Y = gg(&mY, DATA[ti].d_result_p);

            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                if (veryVeryVerbose) { T_ T_ Q(ExceptionTestBody) }

                Obj mX(DATA[ti].d_list, xoa);  const Obj& X = mX;

                ASSERTV(Y, X, Y == X);

                ASSERT(&oa == X.get_allocator());

            } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

            ASSERT(&scratch == Y.get_allocator());
        }
        ASSERT(dam.isTotalSame());
    }

    {
        const struct {
            int                          d_line;      // source line number
            std::initializer_list<TYPE>  d_list;      // source list
            const char                  *d_result_p;  // expected result
        } DATA[] = {
                //line          list             result
                //----          ----             ------
                { L_,   {                  },        ""   },
                { L_,   { V[0]             },       "A"   },
                { L_,   { V[0], V[0]       },      "AA"   },
                { L_,   { V[1], V[0]       },      "BA"   },
                { L_,   { V[0], V[1], V[2] },     "ABC"   },
                { L_,   { V[0], V[1], V[0] },     "ABA"   },
        };
        enum { NUM_DATA = sizeof DATA / sizeof *DATA };

        bslma::TestAllocator oa("object", veryVeryVeryVerbose);
        bslma::TestAllocatorMonitor dam(&da);

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            ASSERT(0 == oa.numBytesInUse());

            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);
            ALLOC                xscratch(&scratch);

            Obj mY(xscratch);  const Obj& Y = gg(&mY, DATA[ti].d_result_p);

            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                if (veryVeryVerbose) { T_ T_ Q(ExceptionTestBody) }

                Obj mX(DATA[ti].d_list);  const Obj& X = mX;

                ASSERTV(Y, X, Y == X);

                ASSERT(&da == X.get_allocator());

            } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

            ASSERT(&scratch == Y.get_allocator());
        }
        ASSERT(dam.isTotalUp());
    }

    {
        const struct {
            int                          d_line;      // source line number
            std::initializer_list<TYPE>  d_list;      // source list
            const char                  *d_result_p;  // expected result
        } DATA[] = {
                //line          list             result
                //----          ----             ------
                { L_,   {                  },        ""   },
                { L_,   { V[0]             },       "A"   },
                { L_,   { V[0], V[0]       },      "AA"   },
                { L_,   { V[1], V[0]       },      "BA"   },
                { L_,   { V[0], V[1], V[2] },     "ABC"   },
                { L_,   { V[0], V[1], V[0] },     "ABA"   },
        };
        enum { NUM_DATA = sizeof DATA / sizeof *DATA };

        bslma::TestAllocator oa("object", veryVeryVeryVerbose);
        bslma::TestAllocatorMonitor dam(&da);

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            ASSERT(0 == oa.numBytesInUse());

            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);
            ALLOC                xscratch(&scratch);

            Obj mY(xscratch);  const Obj& Y = gg(&mY, DATA[ti].d_result_p);

            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                if (veryVeryVerbose) { T_ T_ Q(ExceptionTestBody) }

                Obj mX = DATA[ti].d_list;  const Obj& X = mX;

                ASSERTV(Y, X, Y == X);

                ASSERT(&da == X.get_allocator());

            } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

            ASSERT(&scratch == Y.get_allocator());
        }
        ASSERT(dam.isTotalUp());
    }

    if (verbose) printf("\nTesting 'operator=' with initializer lists.\n");
    {
        const struct {
            int                          d_line;      // source line number
            const char                  *d_spec_p;    // target string
            std::initializer_list<TYPE>  d_list;      // source list
            const char                  *d_result_p;  // expected result
        } DATA[] = {
                //line  lhs            list                result
                //----  ------         ----                ------
                { L_,   "",       {                  },        ""   },
                { L_,   "",       { V[0]             },       "A"   },
                { L_,   "A",      {                  },        ""   },
                { L_,   "A",      { V[1]             },       "B"   },
                { L_,   "A",      { V[0], V[1]       },      "AB"   },
                { L_,   "A",      { V[1], V[2]       },      "BC"   },
                { L_,   "AB",     {                  },        ""   },
                { L_,   "AB",     { V[0], V[1], V[2] },     "ABC"   },
                { L_,   "AB",     { V[2], V[3], V[4] },     "CDE"   },
        };
        enum { NUM_DATA = sizeof DATA / sizeof *DATA };

        bslma::TestAllocator oa("object", veryVeryVeryVerbose);
        ALLOC                xoa(&oa);
        bslma::TestAllocatorMonitor dam(&da);

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            ASSERT(0 == oa.numBytesInUse());

            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);
            ALLOC                xscratch(&scratch);

            Obj mY(xscratch);  const Obj& Y = gg(&mY, DATA[ti].d_result_p);

            Obj mX(xoa);  const Obj& X = gg(&mX, DATA[ti].d_spec_p);

            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                if (veryVeryVerbose) { T_ T_ Q(ExceptionTestBody) }

                Obj *mR = &(mX = DATA[ti].d_list);
                ASSERTV(mR, &mX, mR == &mX);
                ASSERTV(Y,    X,  Y == X);

            } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
        }
        ASSERT(dam.isTotalSame());
    }

    if (verbose) printf("\nTesting 'assign' with initializer lists.\n");
    {
        const struct {
            int                          d_line;      // source line number
            const char                  *d_spec_p;    // target string
            std::initializer_list<TYPE>  d_list;      // source list
            const char                  *d_result_p;  // expected result
        } DATA[] = {
                //line  lhs            list                result
                //----  ------         ----                ------
                { L_,   "",       {                  },        ""   },
                { L_,   "",       { V[0]             },       "A"   },
                { L_,   "A",      {                  },        ""   },
                { L_,   "A",      { V[1]             },       "B"   },
                { L_,   "A",      { V[0], V[1]       },      "AB"   },
                { L_,   "A",      { V[1], V[2]       },      "BC"   },
                { L_,   "AB",     {                  },        ""   },
                { L_,   "AB",     { V[0], V[1], V[2] },     "ABC"   },
                { L_,   "AB",     { V[2], V[3], V[4] },     "CDE"   },
        };
        enum { NUM_DATA = sizeof DATA / sizeof *DATA };

        bslma::TestAllocator oa("object", veryVeryVeryVerbose);
        ALLOC                xoa(&oa);
        bslma::TestAllocatorMonitor dam(&da);

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            ASSERT(0 == oa.numBytesInUse());

            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);
            ALLOC                xscratch(&scratch);

            Obj mY(xscratch);  const Obj& Y = gg(&mY, DATA[ti].d_result_p);

            Obj mX(xoa);  const Obj& X = gg(&mX, DATA[ti].d_spec_p);

            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                if (veryVeryVerbose) { T_ T_ Q(ExceptionTestBody) }

                mX.assign(DATA[ti].d_list);
                ASSERTV(Y, X, Y == X);

            } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
        }
        ASSERT(dam.isTotalSame());
    }

    if (verbose) printf("\nTesting 'insert' with initializer lists.\n");
    {
        const struct {
            int                          d_line;      // source line number
            const char                  *d_spec_p;    // target string
            int                          d_pos;       // position to insert
            std::initializer_list<TYPE>  d_list;      // source list
            const char                  *d_result_p;  // expected result
        } DATA[] = {
                //line  source   pos   list                result
                //----  ------   ---   ----                ------
                { L_,   "",      -1,   {                  },        ""   },
                { L_,   "",      99,   { V[0]             },       "A"   },
                { L_,   "A",      0,   {                  },       "A"   },
                { L_,   "A",      0,   { V[1]             },      "BA"   },
                { L_,   "A",      1,   { V[1]             },      "AB"   },
                { L_,   "AB",     0,   {                  },      "AB"   },
                { L_,   "AB",     0,   { V[0], V[1]       },    "ABAB"   },
                { L_,   "AB",     1,   { V[1], V[2]       },    "ABCB"   },
                { L_,   "AB",     2,   { V[0], V[1], V[2] },   "ABABC"   },
                { L_,   "ABC",    0,   { V[3]             },    "DABC"   },
        };
        enum { NUM_DATA = sizeof DATA / sizeof *DATA };

        bslma::TestAllocator        oa("object", veryVeryVeryVerbose);
        ALLOC                       xoa(&oa);
        bslma::TestAllocatorMonitor dam(&da);

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int LINE = DATA[ti].d_line;
            const int POS  = DATA[ti].d_pos;

            ASSERT(0 == oa.numBytesInUse());

            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);
            ALLOC                xscratch(&scratch);

            Obj mY(xscratch);  const Obj& Y = gg(&mY, DATA[ti].d_result_p);

            Obj mX(xoa);  const Obj& X = gg(&mX, DATA[ti].d_spec_p);

            const size_t index = -1 == POS ? 0 : 99 == POS ? X.size() : POS;
            iterator result = mX.insert(X.begin() + index, DATA[ti].d_list);
            ASSERTV(LINE,  result == X.begin() + index);
            ASSERTV(LINE, X, Y, X == Y);
        }
        ASSERT(dam.isTotalSame());
    }
#endif
}

template <class TYPE, class ALLOC>
template <int N_ARGS,
          int N01,
          int N02,
          int N03,
          int N04,
          int N05,
          int N06,
          int N07,
          int N08,
          int N09,
          int N10>
void
TestDriver3<TYPE, ALLOC>::testCase28a_RunTest(Obj *target, const_iterator pos)
{
    DECLARE_BOOL_CONSTANT(MOVE_01, N01 == 1);
    DECLARE_BOOL_CONSTANT(MOVE_02, N02 == 1);
    DECLARE_BOOL_CONSTANT(MOVE_03, N03 == 1);
    DECLARE_BOOL_CONSTANT(MOVE_04, N04 == 1);
    DECLARE_BOOL_CONSTANT(MOVE_05, N05 == 1);
    DECLARE_BOOL_CONSTANT(MOVE_06, N06 == 1);
    DECLARE_BOOL_CONSTANT(MOVE_07, N07 == 1);
    DECLARE_BOOL_CONSTANT(MOVE_08, N08 == 1);
    DECLARE_BOOL_CONSTANT(MOVE_09, N09 == 1);
    DECLARE_BOOL_CONSTANT(MOVE_10, N10 == 1);

    bslma::TestAllocator *testAlloc = dynamic_cast<bslma::TestAllocator *>(
                                          target->get_allocator().mechanism());
    if (!testAlloc) {
        ASSERT(!"Allocator in test case 29 is not a test allocator!");
        return;                                                       // RETURN
    }
    bslma::TestAllocator& oa = *testAlloc;
    Obj& mX = *target;  const Obj& X = mX;

    bslma::TestAllocator aa("args", veryVeryVeryVerbose);

    bsls::ObjectBuffer<typename TYPE::ArgType01> BUF01;
    ConstrUtil::construct(BUF01.address(), &aa, V01);
    typename TYPE::ArgType01& A01 = BUF01.object();
    bslma::DestructorGuard<typename TYPE::ArgType01> G01(&A01);

    bsls::ObjectBuffer<typename TYPE::ArgType02> BUF02;
    ConstrUtil::construct(BUF02.address(), &aa, V02);
    typename TYPE::ArgType02& A02 = BUF02.object();
    bslma::DestructorGuard<typename TYPE::ArgType02> G02(&A02);

    bsls::ObjectBuffer<typename TYPE::ArgType03> BUF03;
    ConstrUtil::construct(BUF03.address(), &aa, V03);
    typename TYPE::ArgType03& A03 = BUF03.object();
    bslma::DestructorGuard<typename TYPE::ArgType03> G03(&A03);

    bsls::ObjectBuffer<typename TYPE::ArgType04> BUF04;
    ConstrUtil::construct(BUF04.address(), &aa, V04);
    typename TYPE::ArgType04& A04 = BUF04.object();
    bslma::DestructorGuard<typename TYPE::ArgType04> G04(&A04);

    bsls::ObjectBuffer<typename TYPE::ArgType05> BUF05;
    ConstrUtil::construct(BUF05.address(), &aa, V05);
    typename TYPE::ArgType05& A05 = BUF05.object();
    bslma::DestructorGuard<typename TYPE::ArgType05> G05(&A05);

    bsls::ObjectBuffer<typename TYPE::ArgType06> BUF06;
    ConstrUtil::construct(BUF06.address(), &aa, V06);
    typename TYPE::ArgType06& A06 = BUF06.object();
    bslma::DestructorGuard<typename TYPE::ArgType06> G06(&A06);

    bsls::ObjectBuffer<typename TYPE::ArgType07> BUF07;
    ConstrUtil::construct(BUF07.address(), &aa, V07);
    typename TYPE::ArgType07& A07 = BUF07.object();
    bslma::DestructorGuard<typename TYPE::ArgType07> G07(&A07);

    bsls::ObjectBuffer<typename TYPE::ArgType08> BUF08;
    ConstrUtil::construct(BUF08.address(), &aa,  V08);
    typename TYPE::ArgType08& A08 = BUF08.object();
    bslma::DestructorGuard<typename TYPE::ArgType08> G08(&A08);

    bsls::ObjectBuffer<typename TYPE::ArgType09> BUF09;
    ConstrUtil::construct(BUF09.address(), &aa, V09);
    typename TYPE::ArgType09& A09 = BUF09.object();
    bslma::DestructorGuard<typename TYPE::ArgType09> G09(&A09);

    bsls::ObjectBuffer<typename TYPE::ArgType10> BUF10;
    ConstrUtil::construct(BUF10.address(), &aa, V10);
    typename TYPE::ArgType10& A10 = BUF10.object();
    bslma::DestructorGuard<typename TYPE::ArgType10> G10(&A10);

    const size_t                  len   = X.size();
    const typename Obj::size_type index = pos - X.cbegin();

    iterator result;

    switch (N_ARGS) {
      case 0: {
        result = mX.emplace(pos);
      } break;
      case 1: {
        result = mX.emplace(pos, testArg(A01, MOVE_01));
      } break;
      case 2: {
        result = mX.emplace(pos, testArg(A01, MOVE_01), testArg(A02, MOVE_02));
      } break;
      case 3: {
        result = mX.emplace(pos,
                            testArg(A01, MOVE_01),
                            testArg(A02, MOVE_02),
                            testArg(A03, MOVE_03));
      } break;
      case 4: {
        result = mX.emplace(pos,
                            testArg(A01, MOVE_01),
                            testArg(A02, MOVE_02),
                            testArg(A03, MOVE_03),
                            testArg(A04, MOVE_04));
      } break;
      case 5: {
        result = mX.emplace(pos,
                            testArg(A01, MOVE_01),
                            testArg(A02, MOVE_02),
                            testArg(A03, MOVE_03),
                            testArg(A04, MOVE_04),
                            testArg(A05, MOVE_05));
      } break;
      case 6: {
        result = mX.emplace(pos,
                            testArg(A01, MOVE_01),
                            testArg(A02, MOVE_02),
                            testArg(A03, MOVE_03),
                            testArg(A04, MOVE_04),
                            testArg(A05, MOVE_05),
                            testArg(A06, MOVE_06));
      } break;
      case 7: {
        result = mX.emplace(pos,
                            testArg(A01, MOVE_01),
                            testArg(A02, MOVE_02),
                            testArg(A03, MOVE_03),
                            testArg(A04, MOVE_04),
                            testArg(A05, MOVE_05),
                            testArg(A06, MOVE_06),
                            testArg(A07, MOVE_07));
      } break;
      case 8: {
        result = mX.emplace(pos,
                            testArg(A01, MOVE_01),
                            testArg(A02, MOVE_02),
                            testArg(A03, MOVE_03),
                            testArg(A04, MOVE_04),
                            testArg(A05, MOVE_05),
                            testArg(A06, MOVE_06),
                            testArg(A07, MOVE_07),
                            testArg(A08, MOVE_08));
      } break;
      case 9: {
        result = mX.emplace(pos,
                            testArg(A01, MOVE_01),
                            testArg(A02, MOVE_02),
                            testArg(A03, MOVE_03),
                            testArg(A04, MOVE_04),
                            testArg(A05, MOVE_05),
                            testArg(A06, MOVE_06),
                            testArg(A07, MOVE_07),
                            testArg(A08, MOVE_08),
                            testArg(A09, MOVE_09));
      } break;
      case 10: {
        result = mX.emplace(pos,
                            testArg(A01, MOVE_01),
                            testArg(A02, MOVE_02),
                            testArg(A03, MOVE_03),
                            testArg(A04, MOVE_04),
                            testArg(A05, MOVE_05),
                            testArg(A06, MOVE_06),
                            testArg(A07, MOVE_07),
                            testArg(A08, MOVE_08),
                            testArg(A09, MOVE_09),
                            testArg(A10, MOVE_10));
      } break;
      default: {
        ASSERTV(!"Invalid # of args!");
      } break;
    }
    ASSERTV(len + 1, X.size(), len + 1 == X.size());

    ASSERTV(MOVE_01, A01.movedFrom(),
            MOVE_01 == (MoveState::e_MOVED == A01.movedFrom()) || 2 == N01);
    ASSERTV(MOVE_02, A02.movedFrom(),
            MOVE_02 == (MoveState::e_MOVED == A02.movedFrom()) || 2 == N02);
    ASSERTV(MOVE_03, A03.movedFrom(),
            MOVE_03 == (MoveState::e_MOVED == A03.movedFrom()) || 2 == N03);
    ASSERTV(MOVE_04, A04.movedFrom(),
            MOVE_04 == (MoveState::e_MOVED == A04.movedFrom()) || 2 == N04);
    ASSERTV(MOVE_05, A05.movedFrom(),
            MOVE_05 == (MoveState::e_MOVED == A05.movedFrom()) || 2 == N05);
    ASSERTV(MOVE_06, A06.movedFrom(),
            MOVE_06 == (MoveState::e_MOVED == A06.movedFrom()) || 2 == N06);
    ASSERTV(MOVE_07, A07.movedFrom(),
            MOVE_07 == (MoveState::e_MOVED == A07.movedFrom()) || 2 == N07);
    ASSERTV(MOVE_08, A08.movedFrom(),
            MOVE_08 == (MoveState::e_MOVED == A08.movedFrom()) || 2 == N08);
    ASSERTV(MOVE_09, A09.movedFrom(),
            MOVE_09 == (MoveState::e_MOVED == A09.movedFrom()) || 2 == N09);
    ASSERTV(MOVE_10, A10.movedFrom(),
            MOVE_10 == (MoveState::e_MOVED == A10.movedFrom()) || 2 == N10);

    const TYPE& V = X[index];

    ASSERTV( result == X.begin() + index);
    ASSERTV(*result == V);

    ASSERTV(V01, V.arg01(), V01 == V.arg01() || 2 == N01);
    ASSERTV(V02, V.arg02(), V02 == V.arg02() || 2 == N02);
    ASSERTV(V03, V.arg03(), V03 == V.arg03() || 2 == N03);
    ASSERTV(V04, V.arg04(), V04 == V.arg04() || 2 == N04);
    ASSERTV(V05, V.arg05(), V05 == V.arg05() || 2 == N05);
    ASSERTV(V06, V.arg06(), V06 == V.arg06() || 2 == N06);
    ASSERTV(V07, V.arg07(), V07 == V.arg07() || 2 == N07);
    ASSERTV(V08, V.arg08(), V08 == V.arg08() || 2 == N08);
    ASSERTV(V09, V.arg09(), V09 == V.arg09() || 2 == N09);
    ASSERTV(V10, V.arg10(), V10 == V.arg10() || 2 == N10);

    TestAllocatorUtil::test(V, oa);
}

template <class TYPE, class ALLOC>
void TestDriver3<TYPE,ALLOC>::testCase28()
{
    // ------------------------------------------------------------------------
    // TESTING 'emplace'
    //   Note that the forwarding of arguments is tested in 'testCase29a'; all
    //   other functionality is tested in this function.
    //
    // Concerns:
    //: 1 A new element is inserted at the indicated position in the container
    //:   with the relative order of the existing elements remaining unchanged.
    //:
    //: 2 The returned iterator has the expected value.
    //:
    //: 3 The internal memory management system is hooked up properly so that
    //:   *all* internally allocated memory draws from a user-supplied
    //:   allocator whenever one is specified.
    //:
    //: 4 Insertion is exception neutral w.r.t. memory allocation.
    //:
    //: 5 There is no effect on the validity of references to elements of the
    //:   container if the insertion is at the front or the back.  (TBD not yet
    //:   tested)
    //
    // Plan:
    //: 1 Using the table-based approach, specify a set of initial container
    //:   values, insertion positions, element values (for insertion), and
    //:   expected (post-insertion) container values.
    //:
    //:   1 For each row from P-1, create a container having the specified
    //:     initial value and emplace the specified element value at the
    //:     specified position.
    //:
    //:   2 Verify that the values of the container and the returned iterator
    //:     are as expected.                                            (C-1-2)
    //:
    //:   3 Verify all allocations are from the object's allocator.       (C-3)
    //:
    //: 2 Repeat P-1 under the presence of exceptions.                    (C-4)
    //
    // Testing:
    //   iterator emplace(const_iterator pos, Args&&... args);
    // ------------------------------------------------------------------------

    const TestValues VALUES;

    static const struct {
        int         d_line;       // source line number
        const char *d_spec_p;     // specification string
        int         d_pos;
        char        d_element;    // specification string
        const char *d_results_p;  // expected element values
    } DATA[] = {
        //line  spec                pos    element   results
        //----  --------------      ---    --------   -----------------
        { L_,   "",                 -1,    'Z',      "Z"                    },
        { L_,   "",                 99,    'Z',      "Z"                    },
        { L_,   "A",                 0,    'Z',      "ZA"                   },
        { L_,   "A",                 1,    'Z',      "AZ"                   },
        { L_,   "A",                99,    'Z',      "AZ"                   },
        { L_,   "AB",                0,    'B',      "BAB"                  },
        { L_,   "AB",                1,    'Z',      "AZB"                  },
        { L_,   "AB",                2,    'A',      "ABA"                  },
        { L_,   "AB",               99,    'Z',      "ABZ"                  },
        { L_,   "CAB",               0,    'A',      "ACAB"                 },
        { L_,   "CAB",               1,    'B',      "CBAB"                 },
        { L_,   "CAB",               2,    'C',      "CACB"                 },
        { L_,   "CAB",               3,    'Z',      "CABZ"                 },
        { L_,   "CAB",              99,    'Z',      "CABZ"                 },
        { L_,   "CABD",              0,    'Z',      "ZCABD"                },
        { L_,   "CABD",              1,    'Z',      "CZABD"                },
        { L_,   "CABD",              2,    'B',      "CABBD"                },
        { L_,   "CABD",              3,    'Z',      "CABZD"                },
        { L_,   "CABD",              4,    'B',      "CABDB"                },
        { L_,   "CABD",             99,    'A',      "CABDA"                },
        { L_,   "HGFEDCBA",          0,    'Z',      "ZHGFEDCBA"            },
        { L_,   "HGFEDCBA",          1,    'Z',      "HZGFEDCBA"            },
        { L_,   "HGFEDCBA",          7,    'Z',      "HGFEDCBZA"            },
        { L_,   "HGFEDCBA",          8,    'Z',      "HGFEDCBAZ"            },

        // back-end-loaded specs -- 'gg' does 'push_back' for '[A-Z]'

        { L_,   "ABCDEFGHIJKLMNOPQRSTUVWXY",
                                     0,    'Z',
               "ZABCDEFGHIJKLMNOPQRSTUVWXY"                                 },
        { L_,   "ABCDEFGHIJKLMNOPQRSTUVWXY",
                                    10,    'Z',
                "ABCDEFGHIJZKLMNOPQRSTUVWXY"                                },
        { L_,   "ABCDEFGHIJKLMNOPQRSTUVWXY",
                                    13,    'Z',
                "ABCDEFGHIJKLMZNOPQRSTUVWXY"                                },
        { L_,   "ABCDEFGHIJKLMNOPQRSTUVWXY",
                                    20,    'Z',
                "ABCDEFGHIJKLMNOPQRSTZUVWXY"                                },
        { L_,   "ABCDEFGHIJKLMNOPQRSTUVWXY",
                                    99,    'Z',
                "ABCDEFGHIJKLMNOPQRSTUVWXYZ"                                },

        // front-end-loaded specs -- 'gg' does 'push_front' for '[a-z]'

        { L_,   "yxwvutsrqponmlkjihgfedcba",
                                     0,    'Z',
               "ZABCDEFGHIJKLMNOPQRSTUVWXY"                                 },
        { L_,   "yxwvutsrqponmlkjihgfedcba",
                                    10,    'Z',
                "ABCDEFGHIJZKLMNOPQRSTUVWXY"                                },
        { L_,   "yxwvutsrqponmlkjihgfedcba",
                                    13,    'Z',
                "ABCDEFGHIJKLMZNOPQRSTUVWXY"                                },
        { L_,   "yxwvutsrqponmlkjihgfedcba",
                                    20,    'Z',
                "ABCDEFGHIJKLMNOPQRSTZUVWXY"                                },
        { L_,   "yxwvutsrqponmlkjihgfedcba",
                                    99,    'Z',
                "ABCDEFGHIJKLMNOPQRSTUVWXYZ"                                }
    };
    enum { NUM_DATA = sizeof DATA / sizeof *DATA };

    if (verbose) printf("\nTesting 'emplace' without exceptions.\n");
    {
        for (size_t ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE     = DATA[ti].d_line;
            const char *const SPEC     = DATA[ti].d_spec_p;
            const int         POS      = DATA[ti].d_pos;
            const char        ELEMENT  = DATA[ti].d_element;
            const char *const EXPECTED = DATA[ti].d_results_p;
            const size_t      SIZE     = strlen(SPEC);

            bslma::TestAllocator da("default", veryVeryVeryVerbose);
            bslma::TestAllocator oa("object",  veryVeryVeryVerbose);
            ALLOC                xoa(&oa);

            bslma::DefaultAllocatorGuard dag(&da);

            Obj mX(xoa);  const Obj& X = gg(&mX, SPEC);

            if (veryVerbose) { T_ P_(LINE) P(X) }

            // -------------------------------------------------------
            // Verify any attribute allocators are installed properly.
            // -------------------------------------------------------

            ASSERTV(LINE, xoa == X.get_allocator());

            ASSERTV(LINE, SIZE, X.size(), SIZE == X.size());

            const Int64 BB = oa.numBlocksTotal();
            const Int64 B  = oa.numBlocksInUse();

            if (veryVerbose) { printf("\t\tBEFORE: "); P_(BB); P(B); }

            const size_t index = -1 == POS ? 0 : 99 == POS ? X.size() :POS;

            CIter position = -1 == POS ? X.cbegin()
                                       : 99 == POS
                                         ? X.cend() : X.cbegin() + POS;

            iterator result = mX.emplace(position, VALUES[ELEMENT - 'A']);

            if (veryVerbose) { T_ P_(LINE) P_(ELEMENT) P(X) }

            const Int64 AA = oa.numBlocksTotal();
            const Int64 A  = oa.numBlocksInUse();

            if (veryVerbose) { printf("\t\tAFTER : "); P_(AA); P(A);}

            ASSERTV(LINE, SIZE, X.size(), SIZE + 1 == X.size());
            ASSERTV(LINE,  result == X.begin() + index);
            ASSERTV(LINE, *result == VALUES[ELEMENT - 'A']);

            TestValues exp(EXPECTED);
            ASSERTV(LINE, 0 == verifyContainer(X, exp, SIZE + 1));
        }
    }

    // There is no strong exception guarantee unless the insertion is at the
    // the front or the back, so we install the guard conditionally.

    if (verbose) printf("\nTesting 'emplace' with injected exceptions.\n");
    {
        for (size_t ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE     = DATA[ti].d_line;
            const char *const SPEC     = DATA[ti].d_spec_p;
            const int         POS      = DATA[ti].d_pos;
            const char        ELEMENT  = DATA[ti].d_element;
            const char *const EXPECTED = DATA[ti].d_results_p;
            const size_t      SIZE     = strlen(SPEC);

            if (veryVerbose) { T_ P_(LINE) P_(SPEC) P(EXPECTED) }

            bslma::TestAllocator oa("object", veryVeryVeryVerbose);
            ALLOC                xoa(&oa);

            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                const Int64 AL = oa.allocationLimit();
                oa.setAllocationLimit(-1);

                Obj mX(xoa);  const Obj &X = gg(&mX, SPEC);

                ASSERTV(LINE, SIZE, X.size(), SIZE == X.size());

                oa.setAllocationLimit(AL);

                const size_t index =
                                    -1 == POS ? 0 : 99 == POS ? X.size() : POS;

                // The strong exception guarantee is in effect only if
                // inserting at the front or the back.

                ExceptionProctor<Obj, ALLOC> proctor(
                                          0 == index || SIZE == index ? &X : 0,
                                          LINE);

                CIter position = -1 == POS ? X.cbegin()
                                           : 99 == POS
                                             ? X.cend() : X.cbegin() + POS;

                iterator result = mX.emplace(position, VALUES[ELEMENT - 'A']);

                ASSERTV(LINE, SIZE, X.size(), SIZE + 1 == X.size());
                ASSERTV(LINE, SIZE,  result == X.begin() + index);
                ASSERTV(LINE, SIZE, *result == VALUES[ELEMENT - 'A']);

                TestValues exp(EXPECTED);
                ASSERTV(LINE, 0 == verifyContainer(X, exp, SIZE + 1));

                proctor.release();
            } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
        }
    }
}

template <class TYPE, class ALLOC>
void TestDriver3<TYPE, ALLOC>::testCase28a()
{
    // ------------------------------------------------------------------------
    // TESTING FORWARDING OF ARGUMENTS WITH 'emplace'
    //
    // Concerns:
    //: 1 'emplace' correctly forwards arguments to the constructor of the
    //:   value type, up to 10 arguments, the max number of arguments provided
    //:   for C++03 compatibility.  Note that only the forwarding of arguments
    //:   is tested in this function; all other functionality is tested in
    //:  'testCase29'.
    //:
    //: 2 'emplace' is exception neutral w.r.t. memory allocation.
    //
    // Plan:
    //: 1 This test makes material use of template method 'testCase28a_RunTest'
    //:   with the first integer template parameter indicating the number of
    //:   arguments to use, and the next 10 integer template parameters
    //:   indicating '0' for copy, '1' for move, and '2' for not-applicable
    //:   (i.e., beyond the number of arguments).  'testCase28a_RunTest' takes
    //:   two arguments: an address providing modifiable access to a container
    //:   and a 'const_iterator' indicating the desired emplacement position.
    //:
    //:   1 Create 10 argument values with their own argument-specific
    //:     allocator.
    //:
    //:   2 Based on the (first) template parameter indicating the number of
    //:     arguments to pass, call 'emplace' with the corresponding argument
    //:     values, performing an explicit move of the argument if so indicated
    //:     by the template parameter corresponding to the argument, all in the
    //:     presence of injected exceptions.
    //:
    //:   3 Verify that the argument values were passed correctly.
    //:
    //:   4 Verify that the allocator was forwarded correctly.
    //:
    //:   5 Verify that the move-state for each argument is as expected.
    //:
    //:   6 Verify that the emplaced value was inserted at the correct position
    //:     in the container.
    //:
    //:   7 Verify that the returned iterator has the expected value.
    //:
    //: 2 Create a container with it's own object-specific allocator and
    //:   populate it with four elements (see P-4).
    //:
    //: 3 Call 'testCase28a_RunTest' in various configurations:
    //:   1 For 1..10 arguments, call with the move flag set to '1' and then
    //:     with the move flag set to '0'.
    //:
    //:   2 For 1, 2, 3, and 10 arguments, call with move flags set to '0',
    //:     '1', and each move flag set independently.
    //:
    //: 4 (white-box) For each combination of template arguments, call
    //:   'testCase28a_RunTest' twice, once to emplace at 'cend() - 1' and once
    //:   to emplace at 'cbegin() + 1'.  Note that emplacements at 'cend()' and
    //:   'cbegin()' forward to the (already-tested) 'emplace_back' and
    //:   'emplace_front' methods, respectively.  Also note that there are two
    //:   additional code paths in 'emplace' according to whether the
    //:   emplacement position is nearer to 'cend()' or 'cbegin()'.
    //
    // Testing:
    //   iterator emplace(const_iterator pos, Args&&... args);
    // ------------------------------------------------------------------------

#ifndef BSL_DO_NOT_TEST_MOVE_FORWARDING
    // 'emplace' toward the back, just short of 'emplace_back'

    if (verbose)
        printf("\nTesting emplace 1..10 args, move=1 (toward back)"
               "\n------------------------------------------------\n");
    {
        bslma::TestAllocator oa("object", veryVeryVeryVerbose);

        Obj mX(&oa);  const Obj& X = mX;  mX.resize(4);

        testCase28a_RunTest< 0,2,2,2,2,2,2,2,2,2,2>(&mX, X.cend() - 1);
        testCase28a_RunTest< 1,1,2,2,2,2,2,2,2,2,2>(&mX, X.cend() - 1);
        testCase28a_RunTest< 2,1,1,2,2,2,2,2,2,2,2>(&mX, X.cend() - 1);
        testCase28a_RunTest< 3,1,1,1,2,2,2,2,2,2,2>(&mX, X.cend() - 1);
        testCase28a_RunTest< 4,1,1,1,1,2,2,2,2,2,2>(&mX, X.cend() - 1);
        testCase28a_RunTest< 5,1,1,1,1,1,2,2,2,2,2>(&mX, X.cend() - 1);
        testCase28a_RunTest< 6,1,1,1,1,1,1,2,2,2,2>(&mX, X.cend() - 1);
        testCase28a_RunTest< 7,1,1,1,1,1,1,1,2,2,2>(&mX, X.cend() - 1);
        testCase28a_RunTest< 8,1,1,1,1,1,1,1,1,2,2>(&mX, X.cend() - 1);
        testCase28a_RunTest< 9,1,1,1,1,1,1,1,1,1,2>(&mX, X.cend() - 1);
        testCase28a_RunTest<10,1,1,1,1,1,1,1,1,1,1>(&mX, X.cend() - 1);
    }

    if (verbose)
        printf("\nTesting emplace 1..10 args, move=0 (toward back)"
               "\n------------------------------------------------\n");
    {
        bslma::TestAllocator oa("object", veryVeryVeryVerbose);

        Obj mX(&oa);  const Obj& X = mX;  mX.resize(4);

        testCase28a_RunTest< 0,2,2,2,2,2,2,2,2,2,2>(&mX, X.cend() - 1);
        testCase28a_RunTest< 1,0,2,2,2,2,2,2,2,2,2>(&mX, X.cend() - 1);
        testCase28a_RunTest< 2,0,0,2,2,2,2,2,2,2,2>(&mX, X.cend() - 1);
        testCase28a_RunTest< 3,0,0,0,2,2,2,2,2,2,2>(&mX, X.cend() - 1);
        testCase28a_RunTest< 4,0,0,0,0,2,2,2,2,2,2>(&mX, X.cend() - 1);
        testCase28a_RunTest< 5,0,0,0,0,0,2,2,2,2,2>(&mX, X.cend() - 1);
        testCase28a_RunTest< 6,0,0,0,0,0,0,2,2,2,2>(&mX, X.cend() - 1);
        testCase28a_RunTest< 7,0,0,0,0,0,0,0,2,2,2>(&mX, X.cend() - 1);
        testCase28a_RunTest< 8,0,0,0,0,0,0,0,0,2,2>(&mX, X.cend() - 1);
        testCase28a_RunTest< 9,0,0,0,0,0,0,0,0,0,2>(&mX, X.cend() - 1);
        testCase28a_RunTest<10,0,0,0,0,0,0,0,0,0,0>(&mX, X.cend() - 1);
    }

    if (verbose) printf("\nTesting emplace with 0 args (toward back)"
                        "\n-----------------------------------------\n");
    {
        bslma::TestAllocator oa("object", veryVeryVeryVerbose);

        Obj mX(&oa);  const Obj& X = mX;  mX.resize(4);
        testCase28a_RunTest<0,2,2,2,2,2,2,2,2,2,2>(&mX, X.cend() - 1);
    }

    if (verbose) printf("\nTesting emplace with 1 args (toward back)"
                        "\n-----------------------------------------\n");
    {
        bslma::TestAllocator oa("object", veryVeryVeryVerbose);

        Obj mX(&oa);  const Obj& X = mX;  mX.resize(4);
        testCase28a_RunTest<1,0,2,2,2,2,2,2,2,2,2>(&mX, X.cend() - 1);
        testCase28a_RunTest<1,1,2,2,2,2,2,2,2,2,2>(&mX, X.cend() - 1);
    }

    if (verbose) printf("\nTesting emplace with 2 args (toward back)"
                        "\n-----------------------------------------\n");
    {
        bslma::TestAllocator oa("object", veryVeryVeryVerbose);

        Obj mX(&oa);  const Obj& X = mX;  mX.resize(4);
        testCase28a_RunTest<2,0,0,2,2,2,2,2,2,2,2>(&mX, X.cend() - 1);
        testCase28a_RunTest<2,1,0,2,2,2,2,2,2,2,2>(&mX, X.cend() - 1);
        testCase28a_RunTest<2,0,1,2,2,2,2,2,2,2,2>(&mX, X.cend() - 1);
        testCase28a_RunTest<2,1,1,2,2,2,2,2,2,2,2>(&mX, X.cend() - 1);
    }

    if (verbose) printf("\nTesting emplace with 3 args (toward back)"
                        "\n-----------------------------------------\n");
    {
        bslma::TestAllocator oa("object", veryVeryVeryVerbose);

        Obj mX(&oa);  const Obj& X = mX;  mX.resize(4);
        testCase28a_RunTest<3,0,0,0,2,2,2,2,2,2,2>(&mX, X.cend() - 1);
        testCase28a_RunTest<3,1,0,0,2,2,2,2,2,2,2>(&mX, X.cend() - 1);
        testCase28a_RunTest<3,0,1,0,2,2,2,2,2,2,2>(&mX, X.cend() - 1);
        testCase28a_RunTest<3,0,0,1,2,2,2,2,2,2,2>(&mX, X.cend() - 1);
        testCase28a_RunTest<3,1,1,1,2,2,2,2,2,2,2>(&mX, X.cend() - 1);
    }

    if (verbose) printf("\nTesting emplace with 10 args (toward back)"
                        "\n------------------------------------------\n");
    {
        bslma::TestAllocator oa("object", veryVeryVeryVerbose);

        Obj mX(&oa);  const Obj& X = mX;  mX.resize(4);
        testCase28a_RunTest<10,0,0,0,0,0,0,0,0,0,0>(&mX, X.cend() - 1);
        testCase28a_RunTest<10,1,0,0,0,0,0,0,0,0,0>(&mX, X.cend() - 1);
        testCase28a_RunTest<10,0,1,0,0,0,0,0,0,0,0>(&mX, X.cend() - 1);
        testCase28a_RunTest<10,0,0,1,0,0,0,0,0,0,0>(&mX, X.cend() - 1);
        testCase28a_RunTest<10,0,0,0,1,0,0,0,0,0,0>(&mX, X.cend() - 1);
        testCase28a_RunTest<10,0,0,0,0,1,0,0,0,0,0>(&mX, X.cend() - 1);
        testCase28a_RunTest<10,0,0,0,0,0,1,0,0,0,0>(&mX, X.cend() - 1);
        testCase28a_RunTest<10,0,0,0,0,0,0,1,0,0,0>(&mX, X.cend() - 1);
        testCase28a_RunTest<10,0,0,0,0,0,0,0,1,0,0>(&mX, X.cend() - 1);
        testCase28a_RunTest<10,0,0,0,0,0,0,0,0,1,0>(&mX, X.cend() - 1);
        testCase28a_RunTest<10,0,0,0,0,0,0,0,0,0,1>(&mX, X.cend() - 1);
        testCase28a_RunTest<10,1,1,1,1,1,1,1,1,1,1>(&mX, X.cend() - 1);
    }

    // 'emplace' toward the front, just short of 'emplace_front'

    if (verbose)
        printf("\nTesting emplace 1..10 args, move=1 (toward front)"
               "\n-------------------------------------------------\n");
    {
        bslma::TestAllocator oa("object", veryVeryVeryVerbose);

        Obj mX(&oa);  const Obj& X = mX;  mX.resize(4);

        testCase28a_RunTest< 0,2,2,2,2,2,2,2,2,2,2>(&mX, X.cbegin() + 1);
        testCase28a_RunTest< 1,1,2,2,2,2,2,2,2,2,2>(&mX, X.cbegin() + 1);
        testCase28a_RunTest< 2,1,1,2,2,2,2,2,2,2,2>(&mX, X.cbegin() + 1);
        testCase28a_RunTest< 3,1,1,1,2,2,2,2,2,2,2>(&mX, X.cbegin() + 1);
        testCase28a_RunTest< 4,1,1,1,1,2,2,2,2,2,2>(&mX, X.cbegin() + 1);
        testCase28a_RunTest< 5,1,1,1,1,1,2,2,2,2,2>(&mX, X.cbegin() + 1);
        testCase28a_RunTest< 6,1,1,1,1,1,1,2,2,2,2>(&mX, X.cbegin() + 1);
        testCase28a_RunTest< 7,1,1,1,1,1,1,1,2,2,2>(&mX, X.cbegin() + 1);
        testCase28a_RunTest< 8,1,1,1,1,1,1,1,1,2,2>(&mX, X.cbegin() + 1);
        testCase28a_RunTest< 9,1,1,1,1,1,1,1,1,1,2>(&mX, X.cbegin() + 1);
        testCase28a_RunTest<10,1,1,1,1,1,1,1,1,1,1>(&mX, X.cbegin() + 1);
    }

    if (verbose)
        printf("\nTesting emplace 1..10 args, move=0 (toward front)"
               "\n-------------------------------------------------\n");
    {
        bslma::TestAllocator oa("object", veryVeryVeryVerbose);

        Obj mX(&oa);  const Obj& X = mX;  mX.resize(4);

        testCase28a_RunTest< 0,2,2,2,2,2,2,2,2,2,2>(&mX, X.cbegin() + 1);
        testCase28a_RunTest< 1,0,2,2,2,2,2,2,2,2,2>(&mX, X.cbegin() + 1);
        testCase28a_RunTest< 2,0,0,2,2,2,2,2,2,2,2>(&mX, X.cbegin() + 1);
        testCase28a_RunTest< 3,0,0,0,2,2,2,2,2,2,2>(&mX, X.cbegin() + 1);
        testCase28a_RunTest< 4,0,0,0,0,2,2,2,2,2,2>(&mX, X.cbegin() + 1);
        testCase28a_RunTest< 5,0,0,0,0,0,2,2,2,2,2>(&mX, X.cbegin() + 1);
        testCase28a_RunTest< 6,0,0,0,0,0,0,2,2,2,2>(&mX, X.cbegin() + 1);
        testCase28a_RunTest< 7,0,0,0,0,0,0,0,2,2,2>(&mX, X.cbegin() + 1);
        testCase28a_RunTest< 8,0,0,0,0,0,0,0,0,2,2>(&mX, X.cbegin() + 1);
        testCase28a_RunTest< 9,0,0,0,0,0,0,0,0,0,2>(&mX, X.cbegin() + 1);
        testCase28a_RunTest<10,0,0,0,0,0,0,0,0,0,0>(&mX, X.cbegin() + 1);
    }

    if (verbose) printf("\nTesting emplace with 0 args (toward front)"
                        "\n------------------------------------------\n");
    {
        bslma::TestAllocator oa("object", veryVeryVeryVerbose);

        Obj mX(&oa);  const Obj& X = mX;  mX.resize(4);
        testCase28a_RunTest<0,2,2,2,2,2,2,2,2,2,2>(&mX, X.cbegin() + 1);
    }

    if (verbose) printf("\nTesting emplace with 1 args (toward front)"
                        "\n------------------------------------------\n");
    {
        bslma::TestAllocator oa("object", veryVeryVeryVerbose);

        Obj mX(&oa);  const Obj& X = mX;  mX.resize(4);
        testCase28a_RunTest<1,0,2,2,2,2,2,2,2,2,2>(&mX, X.cbegin() + 1);
        testCase28a_RunTest<1,1,2,2,2,2,2,2,2,2,2>(&mX, X.cbegin() + 1);
    }

    if (verbose) printf("\nTesting emplace with 2 args (toward front)"
                        "\n------------------------------------------\n");
    {
        bslma::TestAllocator oa("object", veryVeryVeryVerbose);

        Obj mX(&oa);  const Obj& X = mX;  mX.resize(4);
        testCase28a_RunTest<2,0,0,2,2,2,2,2,2,2,2>(&mX, X.cbegin() + 1);
        testCase28a_RunTest<2,1,0,2,2,2,2,2,2,2,2>(&mX, X.cbegin() + 1);
        testCase28a_RunTest<2,0,1,2,2,2,2,2,2,2,2>(&mX, X.cbegin() + 1);
        testCase28a_RunTest<2,1,1,2,2,2,2,2,2,2,2>(&mX, X.cbegin() + 1);
    }

    if (verbose) printf("\nTesting emplace with 3 args (toward front)"
                        "\n------------------------------------------\n");
    {
        bslma::TestAllocator oa("object", veryVeryVeryVerbose);

        Obj mX(&oa);  const Obj& X = mX;  mX.resize(4);
        testCase28a_RunTest<3,0,0,0,2,2,2,2,2,2,2>(&mX, X.cbegin() + 1);
        testCase28a_RunTest<3,1,0,0,2,2,2,2,2,2,2>(&mX, X.cbegin() + 1);
        testCase28a_RunTest<3,0,1,0,2,2,2,2,2,2,2>(&mX, X.cbegin() + 1);
        testCase28a_RunTest<3,0,0,1,2,2,2,2,2,2,2>(&mX, X.cbegin() + 1);
        testCase28a_RunTest<3,1,1,1,2,2,2,2,2,2,2>(&mX, X.cbegin() + 1);
    }

    if (verbose) printf("\nTesting emplace with 10 args (toward front)"
                        "\n-------------------------------------------\n");
    {
        bslma::TestAllocator oa("object", veryVeryVeryVerbose);

        Obj mX(&oa);  const Obj& X = mX;  mX.resize(4);
        testCase28a_RunTest<10,0,0,0,0,0,0,0,0,0,0>(&mX, X.cbegin() + 1);
        testCase28a_RunTest<10,1,0,0,0,0,0,0,0,0,0>(&mX, X.cbegin() + 1);
        testCase28a_RunTest<10,0,1,0,0,0,0,0,0,0,0>(&mX, X.cbegin() + 1);
        testCase28a_RunTest<10,0,0,1,0,0,0,0,0,0,0>(&mX, X.cbegin() + 1);
        testCase28a_RunTest<10,0,0,0,1,0,0,0,0,0,0>(&mX, X.cbegin() + 1);
        testCase28a_RunTest<10,0,0,0,0,1,0,0,0,0,0>(&mX, X.cbegin() + 1);
        testCase28a_RunTest<10,0,0,0,0,0,1,0,0,0,0>(&mX, X.cbegin() + 1);
        testCase28a_RunTest<10,0,0,0,0,0,0,1,0,0,0>(&mX, X.cbegin() + 1);
        testCase28a_RunTest<10,0,0,0,0,0,0,0,1,0,0>(&mX, X.cbegin() + 1);
        testCase28a_RunTest<10,0,0,0,0,0,0,0,0,1,0>(&mX, X.cbegin() + 1);
        testCase28a_RunTest<10,0,0,0,0,0,0,0,0,0,1>(&mX, X.cbegin() + 1);
        testCase28a_RunTest<10,1,1,1,1,1,1,1,1,1,1>(&mX, X.cbegin() + 1);
    }
#else
    // 'emplace' toward the back, just short of 'emplace_back'

    if (verbose)
        printf("\nTesting emplace 1..10 args, move=0 (toward back)"
               "\n------------------------------------------------\n");
    {
        bslma::TestAllocator oa("object", veryVeryVeryVerbose);

        Obj mX(&oa);  const Obj& X = mX;  mX.resize(4);

        testCase28a_RunTest< 0,2,2,2,2,2,2,2,2,2,2>(&mX, X.cend() - 1);
        testCase28a_RunTest< 1,0,2,2,2,2,2,2,2,2,2>(&mX, X.cend() - 1);
        testCase28a_RunTest< 2,0,0,2,2,2,2,2,2,2,2>(&mX, X.cend() - 1);
        testCase28a_RunTest< 3,0,0,0,2,2,2,2,2,2,2>(&mX, X.cend() - 1);
        testCase28a_RunTest< 4,0,0,0,0,2,2,2,2,2,2>(&mX, X.cend() - 1);
        testCase28a_RunTest< 5,0,0,0,0,0,2,2,2,2,2>(&mX, X.cend() - 1);
        testCase28a_RunTest< 6,0,0,0,0,0,0,2,2,2,2>(&mX, X.cend() - 1);
        testCase28a_RunTest< 7,0,0,0,0,0,0,0,2,2,2>(&mX, X.cend() - 1);
        testCase28a_RunTest< 8,0,0,0,0,0,0,0,0,2,2>(&mX, X.cend() - 1);
        testCase28a_RunTest< 9,0,0,0,0,0,0,0,0,0,2>(&mX, X.cend() - 1);
        testCase28a_RunTest<10,0,0,0,0,0,0,0,0,0,0>(&mX, X.cend() - 1);
    }

    // 'emplace' toward the front, just short of 'emplace_front'

    if (verbose)
        printf("\nTesting emplace 1..10 args, move=0 (toward front)"
               "\n-------------------------------------------------\n");
    {
        bslma::TestAllocator oa("object", veryVeryVeryVerbose);

        Obj mX(&oa);  const Obj& X = mX;  mX.resize(4);

        testCase28a_RunTest< 0,2,2,2,2,2,2,2,2,2,2>(&mX, X.cbegin() + 1);
        testCase28a_RunTest< 1,0,2,2,2,2,2,2,2,2,2>(&mX, X.cbegin() + 1);
        testCase28a_RunTest< 2,0,0,2,2,2,2,2,2,2,2>(&mX, X.cbegin() + 1);
        testCase28a_RunTest< 3,0,0,0,2,2,2,2,2,2,2>(&mX, X.cbegin() + 1);
        testCase28a_RunTest< 4,0,0,0,0,2,2,2,2,2,2>(&mX, X.cbegin() + 1);
        testCase28a_RunTest< 5,0,0,0,0,0,2,2,2,2,2>(&mX, X.cbegin() + 1);
        testCase28a_RunTest< 6,0,0,0,0,0,0,2,2,2,2>(&mX, X.cbegin() + 1);
        testCase28a_RunTest< 7,0,0,0,0,0,0,0,2,2,2>(&mX, X.cbegin() + 1);
        testCase28a_RunTest< 8,0,0,0,0,0,0,0,0,2,2>(&mX, X.cbegin() + 1);
        testCase28a_RunTest< 9,0,0,0,0,0,0,0,0,0,2>(&mX, X.cbegin() + 1);
        testCase28a_RunTest<10,0,0,0,0,0,0,0,0,0,0>(&mX, X.cbegin() + 1);
    }
#endif
}

template <class TYPE, class ALLOC>
template <int N_ARGS,
          int N01,
          int N02,
          int N03,
          int N04,
          int N05,
          int N06,
          int N07,
          int N08,
          int N09,
          int N10>
void
TestDriver3<TYPE, ALLOC>::testCase27a_RunTest(Obj *target)
{
    DECLARE_BOOL_CONSTANT(MOVE_01, N01 == 1);
    DECLARE_BOOL_CONSTANT(MOVE_02, N02 == 1);
    DECLARE_BOOL_CONSTANT(MOVE_03, N03 == 1);
    DECLARE_BOOL_CONSTANT(MOVE_04, N04 == 1);
    DECLARE_BOOL_CONSTANT(MOVE_05, N05 == 1);
    DECLARE_BOOL_CONSTANT(MOVE_06, N06 == 1);
    DECLARE_BOOL_CONSTANT(MOVE_07, N07 == 1);
    DECLARE_BOOL_CONSTANT(MOVE_08, N08 == 1);
    DECLARE_BOOL_CONSTANT(MOVE_09, N09 == 1);
    DECLARE_BOOL_CONSTANT(MOVE_10, N10 == 1);

    bslma::TestAllocator *testAlloc = dynamic_cast<bslma::TestAllocator *>(
                                          target->get_allocator().mechanism());
    if (!testAlloc) {
        ASSERT(!"Allocator in test case 28 is not a test allocator!");
        return;                                                       // RETURN
    }
    bslma::TestAllocator& oa = *testAlloc;
    Obj& mX = *target;  const Obj& X = mX;

    bslma::TestAllocator aa("args", veryVeryVeryVerbose);
    bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

    size_t len = X.size();

    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {

        // Construct all arguments inside the exception test loop as the
        // exception thrown after moving only a portion of arguments leave the
        // moved arguments in a valid, but unspecified state.
        bsls::ObjectBuffer<typename TYPE::ArgType01> BUF01;
        ConstrUtil::construct(BUF01.address(), &aa, V01);
        typename TYPE::ArgType01& A01 = BUF01.object();
        bslma::DestructorGuard<typename TYPE::ArgType01> G01(&A01);

        bsls::ObjectBuffer<typename TYPE::ArgType02> BUF02;
        ConstrUtil::construct(BUF02.address(), &aa, V02);
        typename TYPE::ArgType02& A02 = BUF02.object();
        bslma::DestructorGuard<typename TYPE::ArgType02> G02(&A02);

        bsls::ObjectBuffer<typename TYPE::ArgType03> BUF03;
        ConstrUtil::construct(BUF03.address(), &aa, V03);
        typename TYPE::ArgType03& A03 = BUF03.object();
        bslma::DestructorGuard<typename TYPE::ArgType03> G03(&A03);

        bsls::ObjectBuffer<typename TYPE::ArgType04> BUF04;
        ConstrUtil::construct(BUF04.address(), &aa, V04);
        typename TYPE::ArgType04& A04 = BUF04.object();
        bslma::DestructorGuard<typename TYPE::ArgType04> G04(&A04);

        bsls::ObjectBuffer<typename TYPE::ArgType05> BUF05;
        ConstrUtil::construct(BUF05.address(), &aa, V05);
        typename TYPE::ArgType05& A05 = BUF05.object();
        bslma::DestructorGuard<typename TYPE::ArgType05> G05(&A05);

        bsls::ObjectBuffer<typename TYPE::ArgType06> BUF06;
        ConstrUtil::construct(BUF06.address(), &aa, V06);
        typename TYPE::ArgType06& A06 = BUF06.object();
        bslma::DestructorGuard<typename TYPE::ArgType06> G06(&A06);

        bsls::ObjectBuffer<typename TYPE::ArgType07> BUF07;
        ConstrUtil::construct(BUF07.address(), &aa, V07);
        typename TYPE::ArgType07& A07 = BUF07.object();
        bslma::DestructorGuard<typename TYPE::ArgType07> G07(&A07);

        bsls::ObjectBuffer<typename TYPE::ArgType08> BUF08;
        ConstrUtil::construct(BUF08.address(), &aa,  V08);
        typename TYPE::ArgType08& A08 = BUF08.object();
        bslma::DestructorGuard<typename TYPE::ArgType08> G08(&A08);

        bsls::ObjectBuffer<typename TYPE::ArgType09> BUF09;
        ConstrUtil::construct(BUF09.address(), &aa, V09);
        typename TYPE::ArgType09& A09 = BUF09.object();
        bslma::DestructorGuard<typename TYPE::ArgType09> G09(&A09);

        bsls::ObjectBuffer<typename TYPE::ArgType10> BUF10;
        ConstrUtil::construct(BUF10.address(), &aa, V10);
        typename TYPE::ArgType10& A10 = BUF10.object();
        bslma::DestructorGuard<typename TYPE::ArgType10> G10(&A10);

        ExceptionProctor<Obj, ALLOC> proctor(&X, L_, &scratch);

        switch (N_ARGS) {
          case 0: {
            mX.emplace_back();
          } break;
          case 1: {
            mX.emplace_back(testArg(A01, MOVE_01));
          } break;
          case 2: {
            mX.emplace_back(testArg(A01, MOVE_01), testArg(A02, MOVE_02));
          } break;
          case 3: {
            mX.emplace_back(testArg(A01, MOVE_01),
                            testArg(A02, MOVE_02),
                            testArg(A03, MOVE_03));
          } break;
          case 4: {
            mX.emplace_back(testArg(A01, MOVE_01),
                            testArg(A02, MOVE_02),
                            testArg(A03, MOVE_03),
                            testArg(A04, MOVE_04));
          } break;
          case 5: {
            mX.emplace_back(testArg(A01, MOVE_01),
                            testArg(A02, MOVE_02),
                            testArg(A03, MOVE_03),
                            testArg(A04, MOVE_04),
                            testArg(A05, MOVE_05));
          } break;
          case 6: {
            mX.emplace_back(testArg(A01, MOVE_01),
                            testArg(A02, MOVE_02),
                            testArg(A03, MOVE_03),
                            testArg(A04, MOVE_04),
                            testArg(A05, MOVE_05),
                            testArg(A06, MOVE_06));
          } break;
          case 7: {
            mX.emplace_back(testArg(A01, MOVE_01),
                            testArg(A02, MOVE_02),
                            testArg(A03, MOVE_03),
                            testArg(A04, MOVE_04),
                            testArg(A05, MOVE_05),
                            testArg(A06, MOVE_06),
                            testArg(A07, MOVE_07));
          } break;
          case 8: {
            mX.emplace_back(testArg(A01, MOVE_01),
                            testArg(A02, MOVE_02),
                            testArg(A03, MOVE_03),
                            testArg(A04, MOVE_04),
                            testArg(A05, MOVE_05),
                            testArg(A06, MOVE_06),
                            testArg(A07, MOVE_07),
                            testArg(A08, MOVE_08));
          } break;
          case 9: {
            mX.emplace_back(testArg(A01, MOVE_01),
                            testArg(A02, MOVE_02),
                            testArg(A03, MOVE_03),
                            testArg(A04, MOVE_04),
                            testArg(A05, MOVE_05),
                            testArg(A06, MOVE_06),
                            testArg(A07, MOVE_07),
                            testArg(A08, MOVE_08),
                            testArg(A09, MOVE_09));
          } break;
          case 10: {
            mX.emplace_back(testArg(A01, MOVE_01),
                            testArg(A02, MOVE_02),
                            testArg(A03, MOVE_03),
                            testArg(A04, MOVE_04),
                            testArg(A05, MOVE_05),
                            testArg(A06, MOVE_06),
                            testArg(A07, MOVE_07),
                            testArg(A08, MOVE_08),
                            testArg(A09, MOVE_09),
                            testArg(A10, MOVE_10));
          } break;
          default: {
            ASSERTV(!"Invalid # of args!");
          } break;
        }
        proctor.release();

        ASSERTV(len + 1, X.size(), len + 1 == X.size());

        ASSERTV(MOVE_01, A01.movedFrom(),
               MOVE_01 == (MoveState::e_MOVED == A01.movedFrom()) || 2 == N01);
        ASSERTV(MOVE_02, A02.movedFrom(),
               MOVE_02 == (MoveState::e_MOVED == A02.movedFrom()) || 2 == N02);
        ASSERTV(MOVE_03, A03.movedFrom(),
               MOVE_03 == (MoveState::e_MOVED == A03.movedFrom()) || 2 == N03);
        ASSERTV(MOVE_04, A04.movedFrom(),
               MOVE_04 == (MoveState::e_MOVED == A04.movedFrom()) || 2 == N04);
        ASSERTV(MOVE_05, A05.movedFrom(),
               MOVE_05 == (MoveState::e_MOVED == A05.movedFrom()) || 2 == N05);
        ASSERTV(MOVE_06, A06.movedFrom(),
               MOVE_06 == (MoveState::e_MOVED == A06.movedFrom()) || 2 == N06);
        ASSERTV(MOVE_07, A07.movedFrom(),
               MOVE_07 == (MoveState::e_MOVED == A07.movedFrom()) || 2 == N07);
        ASSERTV(MOVE_08, A08.movedFrom(),
               MOVE_08 == (MoveState::e_MOVED == A08.movedFrom()) || 2 == N08);
        ASSERTV(MOVE_09, A09.movedFrom(),
               MOVE_09 == (MoveState::e_MOVED == A09.movedFrom()) || 2 == N09);
        ASSERTV(MOVE_10, A10.movedFrom(),
               MOVE_10 == (MoveState::e_MOVED == A10.movedFrom()) || 2 == N10);

        const TYPE& V = X[len];

        ASSERTV(V01, V.arg01(), V01 == V.arg01() || 2 == N01);
        ASSERTV(V02, V.arg02(), V02 == V.arg02() || 2 == N02);
        ASSERTV(V03, V.arg03(), V03 == V.arg03() || 2 == N03);
        ASSERTV(V04, V.arg04(), V04 == V.arg04() || 2 == N04);
        ASSERTV(V05, V.arg05(), V05 == V.arg05() || 2 == N05);
        ASSERTV(V06, V.arg06(), V06 == V.arg06() || 2 == N06);
        ASSERTV(V07, V.arg07(), V07 == V.arg07() || 2 == N07);
        ASSERTV(V08, V.arg08(), V08 == V.arg08() || 2 == N08);
        ASSERTV(V09, V.arg09(), V09 == V.arg09() || 2 == N09);
        ASSERTV(V10, V.arg10(), V10 == V.arg10() || 2 == N10);

        TestAllocatorUtil::test(V, oa);
    } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
}

template <class TYPE, class ALLOC>
template <int N_ARGS,
          int N01,
          int N02,
          int N03,
          int N04,
          int N05,
          int N06,
          int N07,
          int N08,
          int N09,
          int N10>
void
TestDriver3<TYPE, ALLOC>::testCase27b_RunTest(Obj *target)
{
    DECLARE_BOOL_CONSTANT(MOVE_01, N01 == 1);
    DECLARE_BOOL_CONSTANT(MOVE_02, N02 == 1);
    DECLARE_BOOL_CONSTANT(MOVE_03, N03 == 1);
    DECLARE_BOOL_CONSTANT(MOVE_04, N04 == 1);
    DECLARE_BOOL_CONSTANT(MOVE_05, N05 == 1);
    DECLARE_BOOL_CONSTANT(MOVE_06, N06 == 1);
    DECLARE_BOOL_CONSTANT(MOVE_07, N07 == 1);
    DECLARE_BOOL_CONSTANT(MOVE_08, N08 == 1);
    DECLARE_BOOL_CONSTANT(MOVE_09, N09 == 1);
    DECLARE_BOOL_CONSTANT(MOVE_10, N10 == 1);

    bslma::TestAllocator *testAlloc = dynamic_cast<bslma::TestAllocator *>(
                                          target->get_allocator().mechanism());
    if (!testAlloc) {
        ASSERT(!"Allocator in test case 28 is not a test allocator!");
        return;                                                       // RETURN
    }
    bslma::TestAllocator& oa = *testAlloc;
    Obj& mX = *target;  const Obj& X = mX;

    bslma::TestAllocator aa("args", veryVeryVeryVerbose);
    bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

    size_t len = X.size();

    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {

        // Construct all arguments inside the exception test loop as the
        // exception thrown after moving only a portion of arguments leave the
        // moved arguments in a valid, but unspecified state.
        bsls::ObjectBuffer<typename TYPE::ArgType01> BUF01;
        ConstrUtil::construct(BUF01.address(), &aa, V01);
        typename TYPE::ArgType01& A01 = BUF01.object();
        bslma::DestructorGuard<typename TYPE::ArgType01> G01(&A01);

        bsls::ObjectBuffer<typename TYPE::ArgType02> BUF02;
        ConstrUtil::construct(BUF02.address(), &aa, V02);
        typename TYPE::ArgType02& A02 = BUF02.object();
        bslma::DestructorGuard<typename TYPE::ArgType02> G02(&A02);

        bsls::ObjectBuffer<typename TYPE::ArgType03> BUF03;
        ConstrUtil::construct(BUF03.address(), &aa, V03);
        typename TYPE::ArgType03& A03 = BUF03.object();
        bslma::DestructorGuard<typename TYPE::ArgType03> G03(&A03);

        bsls::ObjectBuffer<typename TYPE::ArgType04> BUF04;
        ConstrUtil::construct(BUF04.address(), &aa, V04);
        typename TYPE::ArgType04& A04 = BUF04.object();
        bslma::DestructorGuard<typename TYPE::ArgType04> G04(&A04);

        bsls::ObjectBuffer<typename TYPE::ArgType05> BUF05;
        ConstrUtil::construct(BUF05.address(), &aa, V05);
        typename TYPE::ArgType05& A05 = BUF05.object();
        bslma::DestructorGuard<typename TYPE::ArgType05> G05(&A05);

        bsls::ObjectBuffer<typename TYPE::ArgType06> BUF06;
        ConstrUtil::construct(BUF06.address(), &aa, V06);
        typename TYPE::ArgType06& A06 = BUF06.object();
        bslma::DestructorGuard<typename TYPE::ArgType06> G06(&A06);

        bsls::ObjectBuffer<typename TYPE::ArgType07> BUF07;
        ConstrUtil::construct(BUF07.address(), &aa, V07);
        typename TYPE::ArgType07& A07 = BUF07.object();
        bslma::DestructorGuard<typename TYPE::ArgType07> G07(&A07);

        bsls::ObjectBuffer<typename TYPE::ArgType08> BUF08;
        ConstrUtil::construct(BUF08.address(), &aa,  V08);
        typename TYPE::ArgType08& A08 = BUF08.object();
        bslma::DestructorGuard<typename TYPE::ArgType08> G08(&A08);

        bsls::ObjectBuffer<typename TYPE::ArgType09> BUF09;
        ConstrUtil::construct(BUF09.address(), &aa, V09);
        typename TYPE::ArgType09& A09 = BUF09.object();
        bslma::DestructorGuard<typename TYPE::ArgType09> G09(&A09);

        bsls::ObjectBuffer<typename TYPE::ArgType10> BUF10;
        ConstrUtil::construct(BUF10.address(), &aa, V10);
        typename TYPE::ArgType10& A10 = BUF10.object();
        bslma::DestructorGuard<typename TYPE::ArgType10> G10(&A10);

        ExceptionProctor<Obj, ALLOC> proctor(&X, L_, &scratch);

        switch (N_ARGS) {
          case 0: {
            mX.emplace_front();
          } break;
          case 1: {
            mX.emplace_front(testArg(A01, MOVE_01));
          } break;
          case 2: {
            mX.emplace_front(testArg(A01, MOVE_01), testArg(A02, MOVE_02));
          } break;
          case 3: {
            mX.emplace_front(testArg(A01, MOVE_01),
                             testArg(A02, MOVE_02),
                             testArg(A03, MOVE_03));
          } break;
          case 4: {
            mX.emplace_front(testArg(A01, MOVE_01),
                             testArg(A02, MOVE_02),
                             testArg(A03, MOVE_03),
                             testArg(A04, MOVE_04));
          } break;
          case 5: {
            mX.emplace_front(testArg(A01, MOVE_01),
                             testArg(A02, MOVE_02),
                             testArg(A03, MOVE_03),
                             testArg(A04, MOVE_04),
                             testArg(A05, MOVE_05));
          } break;
          case 6: {
            mX.emplace_front(testArg(A01, MOVE_01),
                             testArg(A02, MOVE_02),
                             testArg(A03, MOVE_03),
                             testArg(A04, MOVE_04),
                             testArg(A05, MOVE_05),
                             testArg(A06, MOVE_06));
          } break;
          case 7: {
            mX.emplace_front(testArg(A01, MOVE_01),
                             testArg(A02, MOVE_02),
                             testArg(A03, MOVE_03),
                             testArg(A04, MOVE_04),
                             testArg(A05, MOVE_05),
                             testArg(A06, MOVE_06),
                             testArg(A07, MOVE_07));
          } break;
          case 8: {
            mX.emplace_front(testArg(A01, MOVE_01),
                             testArg(A02, MOVE_02),
                             testArg(A03, MOVE_03),
                             testArg(A04, MOVE_04),
                             testArg(A05, MOVE_05),
                             testArg(A06, MOVE_06),
                             testArg(A07, MOVE_07),
                             testArg(A08, MOVE_08));
          } break;
          case 9: {
            mX.emplace_front(testArg(A01, MOVE_01),
                             testArg(A02, MOVE_02),
                             testArg(A03, MOVE_03),
                             testArg(A04, MOVE_04),
                             testArg(A05, MOVE_05),
                             testArg(A06, MOVE_06),
                             testArg(A07, MOVE_07),
                             testArg(A08, MOVE_08),
                             testArg(A09, MOVE_09));
          } break;
          case 10: {
            mX.emplace_front(testArg(A01, MOVE_01),
                             testArg(A02, MOVE_02),
                             testArg(A03, MOVE_03),
                             testArg(A04, MOVE_04),
                             testArg(A05, MOVE_05),
                             testArg(A06, MOVE_06),
                             testArg(A07, MOVE_07),
                             testArg(A08, MOVE_08),
                             testArg(A09, MOVE_09),
                             testArg(A10, MOVE_10));
          } break;
          default: {
            ASSERTV(!"Invalid # of args!");
          } break;
        }
        proctor.release();

        ASSERTV(len + 1, X.size(), len + 1 == X.size());

        ASSERTV(MOVE_01, A01.movedFrom(),
               MOVE_01 == (MoveState::e_MOVED == A01.movedFrom()) || 2 == N01);
        ASSERTV(MOVE_02, A02.movedFrom(),
               MOVE_02 == (MoveState::e_MOVED == A02.movedFrom()) || 2 == N02);
        ASSERTV(MOVE_03, A03.movedFrom(),
               MOVE_03 == (MoveState::e_MOVED == A03.movedFrom()) || 2 == N03);
        ASSERTV(MOVE_04, A04.movedFrom(),
               MOVE_04 == (MoveState::e_MOVED == A04.movedFrom()) || 2 == N04);
        ASSERTV(MOVE_05, A05.movedFrom(),
               MOVE_05 == (MoveState::e_MOVED == A05.movedFrom()) || 2 == N05);
        ASSERTV(MOVE_06, A06.movedFrom(),
               MOVE_06 == (MoveState::e_MOVED == A06.movedFrom()) || 2 == N06);
        ASSERTV(MOVE_07, A07.movedFrom(),
               MOVE_07 == (MoveState::e_MOVED == A07.movedFrom()) || 2 == N07);
        ASSERTV(MOVE_08, A08.movedFrom(),
               MOVE_08 == (MoveState::e_MOVED == A08.movedFrom()) || 2 == N08);
        ASSERTV(MOVE_09, A09.movedFrom(),
               MOVE_09 == (MoveState::e_MOVED == A09.movedFrom()) || 2 == N09);
        ASSERTV(MOVE_10, A10.movedFrom(),
               MOVE_10 == (MoveState::e_MOVED == A10.movedFrom()) || 2 == N10);

        const TYPE& V = X[0];

        ASSERTV(V01, V.arg01(), V01 == V.arg01() || 2 == N01);
        ASSERTV(V02, V.arg02(), V02 == V.arg02() || 2 == N02);
        ASSERTV(V03, V.arg03(), V03 == V.arg03() || 2 == N03);
        ASSERTV(V04, V.arg04(), V04 == V.arg04() || 2 == N04);
        ASSERTV(V05, V.arg05(), V05 == V.arg05() || 2 == N05);
        ASSERTV(V06, V.arg06(), V06 == V.arg06() || 2 == N06);
        ASSERTV(V07, V.arg07(), V07 == V.arg07() || 2 == N07);
        ASSERTV(V08, V.arg08(), V08 == V.arg08() || 2 == N08);
        ASSERTV(V09, V.arg09(), V09 == V.arg09() || 2 == N09);
        ASSERTV(V10, V.arg10(), V10 == V.arg10() || 2 == N10);

        TestAllocatorUtil::test(V, oa);
    } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
}

template <class TYPE, class ALLOC>
void TestDriver3<TYPE,ALLOC>::testCase27()
{
    // ------------------------------------------------------------------------
    // TESTING 'emplace_front(Args&&...)' AND 'emplace_back(Args&&...)'
    //   Note that the forwarding of arguments is tested in 'testCase28a' and
    //   'testCase28b'; all other functionality is tested in this function.
    //
    // Concerns:
    //: 1 For 'emplace_back', a new element is added to the back of the
    //:   container with the order of the existing elements in the container
    //:   remaining unchanged.
    //:
    //: 2 For 'emplace_front', a new element is added to the front of the
    //:   container with the order of the existing elements in the container
    //:   remaining unchanged.
    //:
    //: 3 'emplace_back' and 'emplace_front' return a reference to the inserted
    //:   element.
    //:
    //: 4 The internal memory management system is hooked up properly so that
    //:   *all* internally allocated memory draws from a user-supplied
    //:   allocator whenever one is specified.
    //:
    //: 5 'emplace_back' and 'emplace_front' provide the strong exception
    //:   guarantee.
    //:
    //: 6 There is no effect on the validity of references to elements of the
    //:   container.  (TBD not yet tested)
    //
    // Plan:
    //: 1 For 'emplace_back', create objects of varying sizes, then append an
    //:   additional element.                                       (C-1, 3..4)
    //:
    //:   1 Verify that the element was added to the back of the container and
    //:     that the contents of the container is as expected.            (C-1)
    //:
    //:   2 Verify that returned reference points to the inserted value.  (C-3)
    //:
    //:   3 Verify all allocations are from the object's allocator.       (C-4)
    //:
    //: 2 Repeat P-1 under the presence of exceptions.                    (C-5)
    //:
    //: 3 Repeat P-1..2 for 'emplace_front', but instead verify that the new
    //:   element is added to the front of the container (P-1.1).      (C-2..5)
    //
    // Testing:
    //   reference emplace_back(Args&&... args);
    //   reference emplace_front(Args&&... args);
    // ------------------------------------------------------------------------

    const TestValues VALUES;

    // testing 'emplace_back'
    {
        static const struct {
            int         d_line;       // source line number
            const char *d_spec_p;     // specification string
            char        d_element;    // specification string
            const char *d_results_p;  // expected element values
        } DATA[] = {
            //line  spec            element          results
            //----  --------------  --------         ---------------------
            { L_,   "",                 'A',         "A"                     },
            { L_,   "A",                'A',         "AA"                    },
            { L_,   "A",                'B',         "AB"                    },
            { L_,   "B",                'A',         "BA"                    },
            { L_,   "AB",               'A',         "ABA"                   },
            { L_,   "BC",               'D',         "BCD"                   },
            { L_,   "BCA",              'Z',         "BCAZ"                  },
            { L_,   "CAB",              'C',         "CABC"                  },
            { L_,   "CDAB",             'D',         "CDABD"                 },
            { L_,   "DABC",             'Z',         "DABCZ"                 },
            { L_,   "ABCDE",            'Z',         "ABCDEZ"                },
            { L_,   "EDCBA",            'E',         "EDCBAE"                },
            { L_,   "ABCDEA",           'E',         "ABCDEAE"               },
            { L_,   "ABCDEAB",          'Z',         "ABCDEABZ"              },
            { L_,   "BACDEABC",         'D',         "BACDEABCD"             },
            { L_,   "CBADEABCD",        'Z',         "CBADEABCDZ"            },
            { L_,   "CBADEABCDAB",      'B',         "CBADEABCDABB"          },
            { L_,   "CBADEABCDABC",     'Z',         "CBADEABCDABCZ"         },
            { L_,   "CBADEABCDABCDE",   'B',         "CBADEABCDABCDEB"       },
            { L_,   "CBADEABCDABCDEA",  'E',         "CBADEABCDABCDEAE"      },
            { L_,   "CBADEABCDABCDEAB", 'Z',         "CBADEABCDABCDEABZ"     },
            { L_,   "ABCDEFGHIJKLMNOPQRSTUVWXY",
                                        'Z',    "ABCDEFGHIJKLMNOPQRSTUVWXYZ" }
        };
        enum { NUM_DATA = sizeof DATA / sizeof *DATA };

        if (verbose) printf("\nTesting 'emplace_back' without exceptions.\n");
        {
            for (size_t ti = 0; ti < NUM_DATA; ++ti) {
                const int         LINE     = DATA[ti].d_line;
                const char *const SPEC     = DATA[ti].d_spec_p;
                const char        ELEMENT  = DATA[ti].d_element;
                const char *const EXPECTED = DATA[ti].d_results_p;
                const size_t      SIZE     = strlen(SPEC);

                bslma::TestAllocator da("default", veryVeryVeryVerbose);
                bslma::TestAllocator oa("object",  veryVeryVeryVerbose);
                ALLOC                xoa(&oa);

                bslma::DefaultAllocatorGuard dag(&da);

                Obj mX(xoa);  const Obj& X = gg(&mX, SPEC);

                if (veryVerbose) { T_ P_(LINE) P(X) }

                // -------------------------------------------------------
                // Verify any attribute allocators are installed properly.
                // -------------------------------------------------------

                ASSERTV(LINE, xoa == X.get_allocator());

                ASSERTV(LINE, SIZE, X.size(), SIZE == X.size());

                const TYPE& RESULT = mX.emplace_back(VALUES[ELEMENT - 'A']);

                if (veryVerbose) { T_ P_(LINE) P_(ELEMENT) P(X) }

                const TYPE *ADDRESS_OF_RESULT = bsls::Util::addressOf(RESULT);
                const TYPE *ADDRESS_OF_LAST_ELEMENT =
                                               bsls::Util::addressOf(X.back());

                ASSERTV(LINE, SIZE, X.size(), SIZE + 1  == X.size());
                ASSERTV(LINE, SIZE,
                        ADDRESS_OF_LAST_ELEMENT == ADDRESS_OF_RESULT);

                TestValues exp(EXPECTED);
                ASSERTV(LINE, 0 == verifyContainer(X, exp, SIZE + 1));
            }
        }

        if (verbose)
            printf("\nTesting 'emplace_back' with injected exceptions.\n");
        {
            for (size_t ti = 0; ti < NUM_DATA; ++ti) {
                const int         LINE     = DATA[ti].d_line;
                const char *const SPEC     = DATA[ti].d_spec_p;
                const char        ELEMENT  = DATA[ti].d_element;
                const char *const EXPECTED = DATA[ti].d_results_p;
                const size_t      SIZE     = strlen(SPEC);

                bslma::TestAllocator oa("object",  veryVeryVeryVerbose);
                ALLOC                xoa(&oa);

                Obj mX(xoa);  const Obj &X = gg(&mX, SPEC);

                ASSERTV(LINE, SIZE, X.size(), SIZE == X.size());

                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                    // 'emplace_back' provides the strong exception guarantee.

                    ExceptionProctor<Obj, ALLOC> proctor(&X, L_);

                    const TYPE& RESULT =
                                        mX.emplace_back(VALUES[ELEMENT - 'A']);

                    const TYPE *ADDRESS_OF_RESULT =
                                                 bsls::Util::addressOf(RESULT);
                    const TYPE *ADDRESS_OF_LAST_ELEMENT =
                                               bsls::Util::addressOf(X.back());

                    ASSERTV(LINE, SIZE, X.size(), SIZE + 1 == X.size());
                    ASSERTV(LINE, SIZE,
                            ADDRESS_OF_LAST_ELEMENT == ADDRESS_OF_RESULT);

                    TestValues exp(EXPECTED);
                    ASSERTV(LINE, 0 == verifyContainer(X, exp, SIZE + 1));

                    proctor.release();
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
            }
        }
    }

    // testing 'emplace_front'
    {
        static const struct {
            int         d_line;       // source line number
            const char *d_spec_p;     // specification string
            char        d_element;    // specification string
            const char *d_results_p;  // expected element values
        } DATA[] = {
            //line  spec            element          results
            //----  --------------  --------         ---------------------
            { L_,   "",                 'A',         "A"                     },
            { L_,   "a",                'A',         "AA"                    },
            { L_,   "a",                'B',         "BA"                    },
            { L_,   "b",                'A',         "AB"                    },
            { L_,   "ab",               'A',         "ABA"                   },
            { L_,   "bc",               'D',         "DCB"                   },
            { L_,   "bca",              'Z',         "ZACB"                  },
            { L_,   "cab",              'C',         "CBAC"                  },
            { L_,   "cdab",             'D',         "DBADC"                 },
            { L_,   "dabc",             'Z',         "ZCBAD"                 },
            { L_,   "abcde",            'Z',         "ZEDCBA"                },
            { L_,   "edcba",            'E',         "EABCDE"                },
            { L_,   "abcdea",           'E',         "EAEDCBA"               },
            { L_,   "abcdeab",          'Z',         "ZBAEDCBA"              },
            { L_,   "bacdeabc",         'D',         "DCBAEDCAB"             },
            { L_,   "cbadeabcd",        'Z',         "ZDCBAEDABC"            },
            { L_,   "cbadeabcdab",      'B',         "BBADCBAEDABC"          },
            { L_,   "cbadeabcdabc",     'Z',         "ZCBADCBAEDABC"         },
            { L_,   "cbadeabcdabcde",   'B',         "BEDCBADCBAEDABC"       },
            { L_,   "cbadeabcdabcdea",  'E',         "EAEDCBADCBAEDABC"      },
            { L_,   "cbadeabcdabcdeab", 'Z',         "ZBAEDCBADCBAEDABC"     },
            { L_,   "yxwvutsrqponmlkjihgfedcba",
                                        'Z',    "ZABCDEFGHIJKLMNOPQRSTUVWXY" }
        };
        enum { NUM_DATA = sizeof DATA / sizeof *DATA };

        if (verbose) printf("\nTesting 'emplace_front' without exceptions.\n");
        {
            for (size_t ti = 0; ti < NUM_DATA; ++ti) {
                const int         LINE     = DATA[ti].d_line;
                const char *const SPEC     = DATA[ti].d_spec_p;
                const char        ELEMENT  = DATA[ti].d_element;
                const char *const EXPECTED = DATA[ti].d_results_p;
                const size_t      SIZE     = strlen(SPEC);

                bslma::TestAllocator da("default", veryVeryVeryVerbose);
                bslma::TestAllocator oa("object",  veryVeryVeryVerbose);
                ALLOC                xoa(&oa);

                bslma::DefaultAllocatorGuard dag(&da);

                Obj mX(xoa);  const Obj& X = gg(&mX, SPEC);

                if (veryVerbose) { T_ P_(LINE) P(X) }

                // -------------------------------------------------------
                // Verify any attribute allocators are installed properly.
                // -------------------------------------------------------

                ASSERTV(LINE, xoa == X.get_allocator());

                ASSERTV(LINE, SIZE, X.size(), SIZE == X.size());

                const TYPE& RESULT = mX.emplace_front(VALUES[ELEMENT - 'A']);

                if (veryVerbose) { T_ P_(LINE) P_(ELEMENT) P(X) }

                const TYPE *ADDRESS_OF_RESULT = bsls::Util::addressOf(RESULT);
                const TYPE *ADDRESS_OF_FIRST_ELEMENT =
                                              bsls::Util::addressOf(X.front());

                ASSERTV(LINE, SIZE, X.size(), SIZE + 1 == X.size());
                ASSERTV(LINE, SIZE, X.size(),
                        ADDRESS_OF_FIRST_ELEMENT == ADDRESS_OF_RESULT);

                TestValues exp(EXPECTED);
                ASSERTV(LINE, 0 == verifyContainer(X, exp, SIZE + 1));
            }
        }

        if (verbose)
            printf("\nTesting 'emplace_front' with injected exceptions.\n");
        {
            for (size_t ti = 0; ti < NUM_DATA; ++ti) {
                const int         LINE     = DATA[ti].d_line;
                const char *const SPEC     = DATA[ti].d_spec_p;
                const char        ELEMENT  = DATA[ti].d_element;
                const char *const EXPECTED = DATA[ti].d_results_p;
                const size_t      SIZE     = strlen(SPEC);

                bslma::TestAllocator oa("object", veryVeryVeryVerbose);
                ALLOC                xoa(&oa);

                Obj mX(xoa);  const Obj &X = gg(&mX, SPEC);

                ASSERTV(LINE, SIZE, X.size(), SIZE == X.size());

                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                    // 'emplace_front' provides the strong exception guarantee.

                    ExceptionProctor<Obj, ALLOC> proctor(&X, L_);

                    const TYPE& RESULT =
                                       mX.emplace_front(VALUES[ELEMENT - 'A']);

                    const TYPE *ADDRESS_OF_RESULT =
                                                 bsls::Util::addressOf(RESULT);
                    const TYPE *ADDRESS_OF_FIRST_ELEMENT =
                                              bsls::Util::addressOf(X.front());

                    ASSERTV(LINE, SIZE, X.size(), SIZE + 1 == X.size());
                    ASSERTV(LINE, SIZE, X.size(),
                            ADDRESS_OF_FIRST_ELEMENT == ADDRESS_OF_RESULT);

                    TestValues exp(EXPECTED);
                    ASSERTV(LINE, 0 == verifyContainer(X, exp, SIZE + 1));

                    proctor.release();
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
            }
        }
    }
}

template <class TYPE, class ALLOC>
void TestDriver3<TYPE, ALLOC>::testCase27a()
{
    // ------------------------------------------------------------------------
    // TESTING FORWARDING OF ARGUMENTS WITH 'emplace_back'
    //
    // Concerns:
    //: 1 'emplace_back' correctly forwards arguments to the constructor of the
    //:   value type, up to 10 arguments, the max number of arguments provided
    //:   for C++03 compatibility.  Note that only the forwarding of arguments
    //:   is tested in this function; all other functionality is tested in
    //:  'testCase28'.
    //:
    //: 2 'emplace_back' provides the strong exception guarantee.
    //
    // Plan:
    //: 1 This test makes material use of template method 'testCase28a_RunTest'
    //:   with the first integer template parameter indicating the number of
    //:   arguments to use, the next 10 integer template parameters indicating
    //:   '0' for copy, '1' for move, and '2' for not-applicable (i.e., beyond
    //:   the number of arguments), and taking as the only argument an address
    //:   providing modifiable access to a container.
    //:
    //:   1 Create 10 argument values with their own argument-specific
    //:     allocator.
    //:
    //:   2 Based on the (first) template parameter indicating the number of
    //:     arguments to pass, call 'emplace_back' with the corresponding
    //:     argument values, performing an explicit move of the argument if so
    //:     indicated by the template parameter corresponding to the argument,
    //:     all in the presence of injected exceptions.
    //:
    //:   3 Verify that the argument values were passed correctly.
    //:
    //:   4 Verify that the allocator was forwarded correctly.
    //:
    //:   5 Verify that the move-state for each argument is as expected.
    //:
    //:   6 Verify that the emplaced value was appended to the back of the
    //:     container.
    //:
    //: 2 Create a container with it's own object-specific allocator.
    //:
    //: 3 Call 'testCase28a_RunTest' in various configurations:
    //:   1 For 1..10 arguments, call with the move flag set to '1' and then
    //:     with the move flag set to '0'.
    //:
    //:   2 For 1, 2, 3, and 10 arguments, call with move flags set to '0',
    //:     '1', and each move flag set independently.
    //
    // Testing:
    //   void emplace_back(Args&&... args);
    // ------------------------------------------------------------------------

#ifndef BSL_DO_NOT_TEST_MOVE_FORWARDING
    if (verbose) printf("\nTesting emplace_back 1..10 args, move=1"
                        "\n---------------------------------------\n");
    {
        bslma::TestAllocator oa("object", veryVeryVeryVerbose);

        Obj mX(&oa);  mX.resize(4);

        testCase27a_RunTest< 0,2,2,2,2,2,2,2,2,2,2>(&mX);
        testCase27a_RunTest< 1,1,2,2,2,2,2,2,2,2,2>(&mX);
        testCase27a_RunTest< 2,1,1,2,2,2,2,2,2,2,2>(&mX);
        testCase27a_RunTest< 3,1,1,1,2,2,2,2,2,2,2>(&mX);
        testCase27a_RunTest< 4,1,1,1,1,2,2,2,2,2,2>(&mX);
        testCase27a_RunTest< 5,1,1,1,1,1,2,2,2,2,2>(&mX);
        testCase27a_RunTest< 6,1,1,1,1,1,1,2,2,2,2>(&mX);
        testCase27a_RunTest< 7,1,1,1,1,1,1,1,2,2,2>(&mX);
        testCase27a_RunTest< 8,1,1,1,1,1,1,1,1,2,2>(&mX);
        testCase27a_RunTest< 9,1,1,1,1,1,1,1,1,1,2>(&mX);
        testCase27a_RunTest<10,1,1,1,1,1,1,1,1,1,1>(&mX);
    }

    if (verbose) printf("\nTesting emplace_back 1..10 args, move=0"
                        "\n---------------------------------------\n");
    {
        bslma::TestAllocator oa("object", veryVeryVeryVerbose);

        Obj mX(&oa);  mX.resize(4);

        testCase27a_RunTest< 0,2,2,2,2,2,2,2,2,2,2>(&mX);
        testCase27a_RunTest< 1,0,2,2,2,2,2,2,2,2,2>(&mX);
        testCase27a_RunTest< 2,0,0,2,2,2,2,2,2,2,2>(&mX);
        testCase27a_RunTest< 3,0,0,0,2,2,2,2,2,2,2>(&mX);
        testCase27a_RunTest< 4,0,0,0,0,2,2,2,2,2,2>(&mX);
        testCase27a_RunTest< 5,0,0,0,0,0,2,2,2,2,2>(&mX);
        testCase27a_RunTest< 6,0,0,0,0,0,0,2,2,2,2>(&mX);
        testCase27a_RunTest< 7,0,0,0,0,0,0,0,2,2,2>(&mX);
        testCase27a_RunTest< 8,0,0,0,0,0,0,0,0,2,2>(&mX);
        testCase27a_RunTest< 9,0,0,0,0,0,0,0,0,0,2>(&mX);
        testCase27a_RunTest<10,0,0,0,0,0,0,0,0,0,0>(&mX);
    }

    if (verbose) printf("\nTesting emplace_back with 0 args"
                        "\n--------------------------------\n");
    {
        bslma::TestAllocator oa("object", veryVeryVeryVerbose);

        Obj mX(&oa);  mX.resize(4);

        testCase27a_RunTest<0,2,2,2,2,2,2,2,2,2,2>(&mX);
    }

    if (verbose) printf("\nTesting emplace_back with 1 args"
                        "\n--------------------------------\n");
    {
        bslma::TestAllocator oa("object", veryVeryVeryVerbose);

        Obj mX(&oa);  mX.resize(4);

        testCase27a_RunTest<1,0,2,2,2,2,2,2,2,2,2>(&mX);
        testCase27a_RunTest<1,1,2,2,2,2,2,2,2,2,2>(&mX);
    }

    if (verbose) printf("\nTesting emplace_back with 2 args"
                        "\n--------------------------------\n");
    {
        bslma::TestAllocator oa("object", veryVeryVeryVerbose);

        Obj mX(&oa);  mX.resize(4);

        testCase27a_RunTest<2,0,0,2,2,2,2,2,2,2,2>(&mX);
        testCase27a_RunTest<2,1,0,2,2,2,2,2,2,2,2>(&mX);
        testCase27a_RunTest<2,0,1,2,2,2,2,2,2,2,2>(&mX);
        testCase27a_RunTest<2,1,1,2,2,2,2,2,2,2,2>(&mX);
    }

    if (verbose) printf("\nTesting emplace_back with 3 args"
                        "\n--------------------------------\n");
    {
        bslma::TestAllocator oa("object", veryVeryVeryVerbose);

        Obj mX(&oa);  mX.resize(4);

        testCase27a_RunTest<3,0,0,0,2,2,2,2,2,2,2>(&mX);
        testCase27a_RunTest<3,1,0,0,2,2,2,2,2,2,2>(&mX);
        testCase27a_RunTest<3,0,1,0,2,2,2,2,2,2,2>(&mX);
        testCase27a_RunTest<3,0,0,1,2,2,2,2,2,2,2>(&mX);
        testCase27a_RunTest<3,1,1,1,2,2,2,2,2,2,2>(&mX);
    }

    if (verbose) printf("\nTesting emplace_back with 10 args"
                        "\n---------------------------------\n");
    {
        bslma::TestAllocator oa("object", veryVeryVeryVerbose);

        Obj mX(&oa);  mX.resize(4);

        testCase27a_RunTest<10,0,0,0,0,0,0,0,0,0,0>(&mX);
        testCase27a_RunTest<10,1,0,0,0,0,0,0,0,0,0>(&mX);
        testCase27a_RunTest<10,0,1,0,0,0,0,0,0,0,0>(&mX);
        testCase27a_RunTest<10,0,0,1,0,0,0,0,0,0,0>(&mX);
        testCase27a_RunTest<10,0,0,0,1,0,0,0,0,0,0>(&mX);
        testCase27a_RunTest<10,0,0,0,0,1,0,0,0,0,0>(&mX);
        testCase27a_RunTest<10,0,0,0,0,0,1,0,0,0,0>(&mX);
        testCase27a_RunTest<10,0,0,0,0,0,0,1,0,0,0>(&mX);
        testCase27a_RunTest<10,0,0,0,0,0,0,0,1,0,0>(&mX);
        testCase27a_RunTest<10,0,0,0,0,0,0,0,0,1,0>(&mX);
        testCase27a_RunTest<10,0,0,0,0,0,0,0,0,0,1>(&mX);
        testCase27a_RunTest<10,1,1,1,1,1,1,1,1,1,1>(&mX);
    }
#else
    if (verbose) printf("\nTesting emplace_back 1..10 args, move=0"
                        "\n---------------------------------------\n");
    {
        bslma::TestAllocator oa("object", veryVeryVeryVerbose);

        Obj mX(&oa);  mX.resize(4);

        testCase27a_RunTest< 0,2,2,2,2,2,2,2,2,2,2>(&mX);
        testCase27a_RunTest< 1,0,2,2,2,2,2,2,2,2,2>(&mX);
        testCase27a_RunTest< 2,0,0,2,2,2,2,2,2,2,2>(&mX);
        testCase27a_RunTest< 3,0,0,0,2,2,2,2,2,2,2>(&mX);
        testCase27a_RunTest< 4,0,0,0,0,2,2,2,2,2,2>(&mX);
        testCase27a_RunTest< 5,0,0,0,0,0,2,2,2,2,2>(&mX);
        testCase27a_RunTest< 6,0,0,0,0,0,0,2,2,2,2>(&mX);
        testCase27a_RunTest< 7,0,0,0,0,0,0,0,2,2,2>(&mX);
        testCase27a_RunTest< 8,0,0,0,0,0,0,0,0,2,2>(&mX);
        testCase27a_RunTest< 9,0,0,0,0,0,0,0,0,0,2>(&mX);
        testCase27a_RunTest<10,0,0,0,0,0,0,0,0,0,0>(&mX);
    }
#endif
}

template <class TYPE, class ALLOC>
void TestDriver3<TYPE, ALLOC>::testCase27b()
{
    // ------------------------------------------------------------------------
    // TESTING FORWARDING OF ARGUMENTS WITH 'emplace_front'
    //
    // Concerns:
    //: 1 'emplace_front' correctly forwards arguments to the constructor of
    //:   the value type, up to 10 arguments, the max number of arguments
    //:   provided for C++03 compatibility.  Note that only the forwarding of
    //:   arguments is tested in this function; all other functionality is
    //:   tested in 'testCase28'.
    //:
    //: 2 'emplace_front' provides the strong exception guarantee.
    //
    // Plan:
    //: 1 This test makes material use of template method 'testCase28b_RunTest'
    //:   with the first integer template parameter indicating the number of
    //:   arguments to use, the next 10 integer template parameters indicating
    //:   '0' for copy, '1' for move, and '2' for not-applicable (i.e., beyond
    //:   the number of arguments), and taking as the only argument an address
    //:   providing modifiable access to a container.
    //:
    //:   1 Create 10 argument values with their own argument-specific
    //:     allocator.
    //:
    //:   2 Based on the (first) template parameter indicating the number of
    //:     arguments to pass, call 'emplace_front' with the corresponding
    //:     argument values, performing an explicit move of the argument if so
    //:     indicated by the template parameter corresponding to the argument,
    //:     all in the presence of injected exceptions.
    //:
    //:   3 Verify that the argument values were passed correctly.
    //:
    //:   4 Verify that the allocator was forwarded correctly.
    //:
    //:   5 Verify that the move-state for each argument is as expected.
    //:
    //:   6 Verify that the emplaced value was prepended to the front of the
    //:     container.
    //:
    //: 2 Create a container with it's own object-specific allocator.
    //:
    //: 3 Call 'testCase28b_RunTest' in various configurations:
    //:   1 For 1..10 arguments, call with the move flag set to '1' and then
    //:     with the move flag set to '0'.
    //:
    //:   2 For 1, 2, 3, and 10 arguments, call with move flags set to '0',
    //:     '1', and each move flag set independently.
    //
    // Testing:
    //   void emplace_front(Args&&... args);
    // ------------------------------------------------------------------------

#ifndef BSL_DO_NOT_TEST_MOVE_FORWARDING
    if (verbose) printf("\nTesting emplace_front 1..10 args, move=1"
                        "\n----------------------------------------\n");
    {
        bslma::TestAllocator oa("object", veryVeryVeryVerbose);

        Obj mX(&oa);  mX.resize(4);

        testCase27b_RunTest< 0,2,2,2,2,2,2,2,2,2,2>(&mX);
        testCase27b_RunTest< 1,1,2,2,2,2,2,2,2,2,2>(&mX);
        testCase27b_RunTest< 2,1,1,2,2,2,2,2,2,2,2>(&mX);
        testCase27b_RunTest< 3,1,1,1,2,2,2,2,2,2,2>(&mX);
        testCase27b_RunTest< 4,1,1,1,1,2,2,2,2,2,2>(&mX);
        testCase27b_RunTest< 5,1,1,1,1,1,2,2,2,2,2>(&mX);
        testCase27b_RunTest< 6,1,1,1,1,1,1,2,2,2,2>(&mX);
        testCase27b_RunTest< 7,1,1,1,1,1,1,1,2,2,2>(&mX);
        testCase27b_RunTest< 8,1,1,1,1,1,1,1,1,2,2>(&mX);
        testCase27b_RunTest< 9,1,1,1,1,1,1,1,1,1,2>(&mX);
        testCase27b_RunTest<10,1,1,1,1,1,1,1,1,1,1>(&mX);
    }

    if (verbose) printf("\nTesting emplace_front 1..10 args, move=0"
                        "\n----------------------------------------\n");
    {
        bslma::TestAllocator oa("object", veryVeryVeryVerbose);

        Obj mX(&oa);  mX.resize(4);

        testCase27b_RunTest< 0,2,2,2,2,2,2,2,2,2,2>(&mX);
        testCase27b_RunTest< 1,0,2,2,2,2,2,2,2,2,2>(&mX);
        testCase27b_RunTest< 2,0,0,2,2,2,2,2,2,2,2>(&mX);
        testCase27b_RunTest< 3,0,0,0,2,2,2,2,2,2,2>(&mX);
        testCase27b_RunTest< 4,0,0,0,0,2,2,2,2,2,2>(&mX);
        testCase27b_RunTest< 5,0,0,0,0,0,2,2,2,2,2>(&mX);
        testCase27b_RunTest< 6,0,0,0,0,0,0,2,2,2,2>(&mX);
        testCase27b_RunTest< 7,0,0,0,0,0,0,0,2,2,2>(&mX);
        testCase27b_RunTest< 8,0,0,0,0,0,0,0,0,2,2>(&mX);
        testCase27b_RunTest< 9,0,0,0,0,0,0,0,0,0,2>(&mX);
        testCase27b_RunTest<10,0,0,0,0,0,0,0,0,0,0>(&mX);
    }

    if (verbose) printf("\nTesting emplace_front with 0 args"
                        "\n---------------------------------\n");
    {
        bslma::TestAllocator oa("object", veryVeryVeryVerbose);

        Obj mX(&oa);  mX.resize(4);

        testCase27b_RunTest<0,2,2,2,2,2,2,2,2,2,2>(&mX);
    }

    if (verbose) printf("\nTesting emplace_front with 1 args"
                        "\n---------------------------------\n");
    {
        bslma::TestAllocator oa("object", veryVeryVeryVerbose);

        Obj mX(&oa);  mX.resize(4);

        testCase27b_RunTest<1,0,2,2,2,2,2,2,2,2,2>(&mX);
        testCase27b_RunTest<1,1,2,2,2,2,2,2,2,2,2>(&mX);
    }

    if (verbose) printf("\nTesting emplace_front with 2 args"
                        "\n---------------------------------\n");
    {
        bslma::TestAllocator oa("object", veryVeryVeryVerbose);

        Obj mX(&oa);  mX.resize(4);

        testCase27b_RunTest<2,0,0,2,2,2,2,2,2,2,2>(&mX);
        testCase27b_RunTest<2,1,0,2,2,2,2,2,2,2,2>(&mX);
        testCase27b_RunTest<2,0,1,2,2,2,2,2,2,2,2>(&mX);
        testCase27b_RunTest<2,1,1,2,2,2,2,2,2,2,2>(&mX);
    }

    if (verbose) printf("\nTesting emplace_front with 3 args"
                        "\n---------------------------------\n");
    {
        bslma::TestAllocator oa("object", veryVeryVeryVerbose);

        Obj mX(&oa);  mX.resize(4);

        testCase27b_RunTest<3,0,0,0,2,2,2,2,2,2,2>(&mX);
        testCase27b_RunTest<3,1,0,0,2,2,2,2,2,2,2>(&mX);
        testCase27b_RunTest<3,0,1,0,2,2,2,2,2,2,2>(&mX);
        testCase27b_RunTest<3,0,0,1,2,2,2,2,2,2,2>(&mX);
        testCase27b_RunTest<3,1,1,1,2,2,2,2,2,2,2>(&mX);
    }

    if (verbose) printf("\nTesting emplace_front with 10 args"
                        "\n----------------------------------\n");
    {
        bslma::TestAllocator oa("object", veryVeryVeryVerbose);

        Obj mX(&oa);  mX.resize(4);

        testCase27b_RunTest<10,0,0,0,0,0,0,0,0,0,0>(&mX);
        testCase27b_RunTest<10,1,0,0,0,0,0,0,0,0,0>(&mX);
        testCase27b_RunTest<10,0,1,0,0,0,0,0,0,0,0>(&mX);
        testCase27b_RunTest<10,0,0,1,0,0,0,0,0,0,0>(&mX);
        testCase27b_RunTest<10,0,0,0,1,0,0,0,0,0,0>(&mX);
        testCase27b_RunTest<10,0,0,0,0,1,0,0,0,0,0>(&mX);
        testCase27b_RunTest<10,0,0,0,0,0,1,0,0,0,0>(&mX);
        testCase27b_RunTest<10,0,0,0,0,0,0,1,0,0,0>(&mX);
        testCase27b_RunTest<10,0,0,0,0,0,0,0,1,0,0>(&mX);
        testCase27b_RunTest<10,0,0,0,0,0,0,0,0,1,0>(&mX);
        testCase27b_RunTest<10,0,0,0,0,0,0,0,0,0,1>(&mX);
        testCase27b_RunTest<10,1,1,1,1,1,1,1,1,1,1>(&mX);
    }
#else
    if (verbose) printf("\nTesting emplace_front 1..10 args, move=0"
                        "\n----------------------------------------\n");
    {
        bslma::TestAllocator oa("object", veryVeryVeryVerbose);

        Obj mX(&oa);  mX.resize(4);

        testCase27b_RunTest< 0,2,2,2,2,2,2,2,2,2,2>(&mX);
        testCase27b_RunTest< 1,0,2,2,2,2,2,2,2,2,2>(&mX);
        testCase27a_RunTest< 2,0,0,2,2,2,2,2,2,2,2>(&mX);
        testCase27b_RunTest< 3,0,0,0,2,2,2,2,2,2,2>(&mX);
        testCase27b_RunTest< 4,0,0,0,0,2,2,2,2,2,2>(&mX);
        testCase27b_RunTest< 5,0,0,0,0,0,2,2,2,2,2>(&mX);
        testCase27b_RunTest< 6,0,0,0,0,0,0,2,2,2,2>(&mX);
        testCase27b_RunTest< 7,0,0,0,0,0,0,0,2,2,2>(&mX);
        testCase27b_RunTest< 8,0,0,0,0,0,0,0,0,2,2>(&mX);
        testCase27b_RunTest< 9,0,0,0,0,0,0,0,0,0,2>(&mX);
        testCase27b_RunTest<10,0,0,0,0,0,0,0,0,0,0>(&mX);
    }
#endif
}

template <class TYPE, class ALLOC>
void TestDriver3<TYPE,ALLOC>::testCase26()
{
    // ------------------------------------------------------------------------
    // TESTING 'iterator insert(const_iterator pos, T&&)'
    //
    // Concerns:
    //: 1 A new element is inserted at the indicated position in the container
    //:   with the relative order of the existing elements remaining unchanged.
    //:
    //: 2 The returned iterator has the expected value.
    //:
    //: 3 The newly inserted item is move-inserted.
    //:
    //: 4 Internal memory management system is hooked up properly so that *all*
    //:   internally allocated memory draws from a user-supplied allocator
    //:   whenever one is specified.
    //:
    //: 5 Insertion is exception neutral w.r.t. memory allocation.
    //:
    //: 6 There is no effect on the validity of references to elements of the
    //:   container if the insertion is at the front or the back.  (TBD not yet
    //:   tested)
    //
    // Plan:
    //: 1 Using the table-based approach, specify a set of initial container
    //:   values, insertion positions, element values (for insertion), and
    //:   expected (post-insertion) container values.
    //:
    //:   1 For each row from P-1, create a container having the specified
    //:     initial value and move-insert the specified element value at the
    //:     specified position.
    //:
    //:   2 Verify that the values of the container and the returned iterator
    //:     are as expected.                                            (C-1-2)
    //:
    //:   3 Verify that the moved-into state for the new element is MOVED.(C-3)
    //:
    //:   4 Verify all allocations are from the object's allocator.       (C-4)
    //:
    //: 2 Repeat P-1 under the presence of exception                      (C-5)
    //
    // Testing:
    //   iterator insert(const_iterator pos, T&& rvalue);
    // ------------------------------------------------------------------------

    if (verbose) printf("TC 27: %s\n", bsls::NameOf<TYPE>().name());

    const TestValues VALUES;

    static const struct {
        int         d_line;       // source line number
        const char *d_spec_p;     // specification string
        int         d_pos;
        char        d_element;    // specification string
        const char *d_results_p;  // expected element values
    } DATA[] = {
        //line  spec                pos    element   results
        //----  --------------      ---    --------   -----------------
        { L_,   "",                 -1,    'Z',      "Z"                    },
        { L_,   "",                 99,    'Z',      "Z"                    },
        { L_,   "A",                -1,    'Z',      "ZA"                   },
        { L_,   "A",                 0,    'Z',      "ZA"                   },
        { L_,   "A",                 1,    'Z',      "AZ"                   },
        { L_,   "A",                99,    'Z',      "AZ"                   },
        { L_,   "AB",               -1,    'B',      "BAB"                  },
        { L_,   "AB",                0,    'B',      "BAB"                  },
        { L_,   "AB",                1,    'Z',      "AZB"                  },
        { L_,   "AB",                2,    'A',      "ABA"                  },
        { L_,   "AB",               99,    'Z',      "ABZ"                  },
        { L_,   "CAB",              -1,    'Z',      "ZCAB"                 },
        { L_,   "CAB",               0,    'A',      "ACAB"                 },
        { L_,   "CAB",               1,    'B',      "CBAB"                 },
        { L_,   "CAB",               2,    'C',      "CACB"                 },
        { L_,   "CAB",               3,    'Z',      "CABZ"                 },
        { L_,   "CAB",              99,    'Z',      "CABZ"                 },
        { L_,   "CABD",             -1,    'A',      "ACABD"                },
        { L_,   "CABD",              0,    'Z',      "ZCABD"                },
        { L_,   "CABD",              1,    'Z',      "CZABD"                },
        { L_,   "CABD",              2,    'B',      "CABBD"                },
        { L_,   "CABD",              3,    'Z',      "CABZD"                },
        { L_,   "CABD",              4,    'B',      "CABDB"                },
        { L_,   "CABD",             99,    'A',      "CABDA"                },
        { L_,   "HGFEDCBA",          0,    'Z',      "ZHGFEDCBA"            },
        { L_,   "HGFEDCBA",          1,    'Z',      "HZGFEDCBA"            },
        { L_,   "HGFEDCBA",          7,    'Z',      "HGFEDCBZA"            },
        { L_,   "HGFEDCBA",          8,    'Z',      "HGFEDCBAZ"            },

        // back-end-loaded specs -- 'gg' does 'push_back' for '[A-Z]'

        { L_,   "ABCDEFGHIJKLMNOPQRSTUVWXY",
                                     0,    'Z',
               "ZABCDEFGHIJKLMNOPQRSTUVWXY"                                 },
        { L_,   "ABCDEFGHIJKLMNOPQRSTUVWXY",
                                    10,    'Z',
                "ABCDEFGHIJZKLMNOPQRSTUVWXY"                                },
        { L_,   "ABCDEFGHIJKLMNOPQRSTUVWXY",
                                    13,    'Z',
                "ABCDEFGHIJKLMZNOPQRSTUVWXY"                                },
        { L_,   "ABCDEFGHIJKLMNOPQRSTUVWXY",
                                    20,    'Z',
                "ABCDEFGHIJKLMNOPQRSTZUVWXY"                                },
        { L_,   "ABCDEFGHIJKLMNOPQRSTUVWXY",
                                    99,    'Z',
                "ABCDEFGHIJKLMNOPQRSTUVWXYZ"                                },

        // front-end-loaded specs -- 'gg' does 'push_front' for '[a-z]'

        { L_,   "yxwvutsrqponmlkjihgfedcba",
                                     0,    'Z',
               "ZABCDEFGHIJKLMNOPQRSTUVWXY"                                 },
        { L_,   "yxwvutsrqponmlkjihgfedcba",
                                    10,    'Z',
                "ABCDEFGHIJZKLMNOPQRSTUVWXY"                                },
        { L_,   "yxwvutsrqponmlkjihgfedcba",
                                    13,    'Z',
                "ABCDEFGHIJKLMZNOPQRSTUVWXY"                                },
        { L_,   "yxwvutsrqponmlkjihgfedcba",
                                    20,    'Z',
                "ABCDEFGHIJKLMNOPQRSTZUVWXY"                                },
        { L_,   "yxwvutsrqponmlkjihgfedcba",
                                    99,    'Z',
                "ABCDEFGHIJKLMNOPQRSTUVWXYZ"                                }
    };
    const Int64 NUM_DATA = sizeof DATA / sizeof *DATA;

    if (verbose) printf("\nTesting 'insert' without exceptions.\n");
    {
        for (size_t ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE     = DATA[ti].d_line;
            const char *const SPEC     = DATA[ti].d_spec_p;
            const int         POS      = DATA[ti].d_pos;
            const char        ELEMENT  = DATA[ti].d_element;
            const char *const EXPECTED = DATA[ti].d_results_p;
            const size_t      SIZE     = strlen(SPEC);

            for (char cfg = 'a'; cfg <= 'b'; ++cfg) {
                const char CONFIG = cfg;

                bslma::TestAllocator da("default",   veryVeryVeryVerbose);
                bslma::TestAllocator oa("object",    veryVeryVeryVerbose);
                bslma::TestAllocator za("different", veryVeryVeryVerbose);
                ALLOC                xoa(&oa);

                bslma::DefaultAllocatorGuard dag(&da);

                Obj mX(xoa);  const Obj& X = gg(&mX, SPEC);

                bslma::TestAllocator *valAllocator = 0;
                switch (cfg) {
                  case 'a': {
                    // inserted value has same allocator
                    valAllocator = &oa;
                  } break;
                  case 'b': {
                    // inserted value has different allocator
                    valAllocator = &za;
                  } break;
                  default: {
                    ASSERTV(CONFIG, !"Bad allocator config.");
                  } return;                                           // RETURN
                }
                bslma::TestAllocator& sa = *valAllocator;
                ALLOC                 xsa(&sa);

                if (veryVerbose) { T_ P_(LINE) P_(CONFIG) P(X) }

                // -------------------------------------------------------
                // Verify any attribute allocators are installed properly.
                // -------------------------------------------------------

                ASSERTV(LINE, CONFIG, xoa == X.get_allocator());

                ASSERTV(LINE, CONFIG, SIZE, X.size(), SIZE == X.size());

                bsls::ObjectBuffer<ValueType> buffer;
                ValueType *valptr = buffer.address();
                TstFacility::emplace(valptr, ELEMENT, xsa);

                ASSERTV(SIZE, numNotMovedInto(X), SIZE == numNotMovedInto(X));

                const size_t index =
                                    -1 == POS ? 0 : 99 == POS ? X.size() : POS;

                CIter position = -1 == POS ? X.cbegin()
                                           : 99 == POS
                                             ? X.cend() : X.cbegin() + POS;

                iterator result = mX.insert(position, MoveUtil::move(*valptr));

                typename MoveState::Enum mState =
                                       TstFacility::getMovedFromState(*valptr);
                bslma::DestructionUtil::destroy(valptr);

                if (veryVerbose) { T_ P_(LINE) P_(CONFIG) P_(ELEMENT) P(X) }

                if (k_IS_WELL_BEHAVED && &oa != &sa) {
                    ASSERTV(mState, MoveState::e_NOT_MOVED == mState);
                }
                else {
                    ASSERTV(mState, MoveState::e_UNKNOWN   == mState
                                 || MoveState::e_MOVED     == mState);
                }

                ASSERTV(LINE, CONFIG, SIZE, X.size(), SIZE + 1 == X.size());
                ASSERTV(LINE, CONFIG,  result == X.begin() + index);
                ASSERTV(LINE, CONFIG, *result == VALUES[ELEMENT - 'A']);

                TestValues exp(EXPECTED);
                ASSERTV(LINE, CONFIG, 0 == verifyContainer(X, exp, SIZE + 1));
            }
        }
    }

    // There is no strong exception guarantee unless the insertion is at the
    // the front or the back, so we install the guard conditionally.

    if (verbose) printf("\nTesting 'insert' with injected exceptions.\n");
    {
        for (size_t ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE     = DATA[ti].d_line;
            const char *const SPEC     = DATA[ti].d_spec_p;
            const int         POS      = DATA[ti].d_pos;
            const char        ELEMENT  = DATA[ti].d_element;
            const char *const EXPECTED = DATA[ti].d_results_p;
            const size_t      SIZE     = strlen(SPEC);

            for (char cfg = 'a'; cfg <= 'b'; ++cfg) {
                const char CONFIG = cfg;

                bslma::TestAllocator da("default",   veryVeryVeryVerbose);
                bslma::TestAllocator oa("object",    veryVeryVeryVerbose);
                bslma::TestAllocator za("different", veryVeryVeryVerbose);
                ALLOC                xoa(&oa);

                bslma::DefaultAllocatorGuard dag(&da);

                bslma::TestAllocator *valAllocator = 0;
                switch (cfg) {
                  case 'a': {
                    // inserted value has same allocator
                    valAllocator = &oa;
                  } break;
                  case 'b': {
                    // inserted value has different allocator
                    valAllocator = &za;
                  } break;
                  default: {
                    ASSERTV(CONFIG, !"Bad allocator config.");
                  } return;                                           // RETURN
                }
                bslma::TestAllocator& sa = *valAllocator;
                ALLOC                 xsa(&sa);

                bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);
                ALLOC                xscratch(&scratch);

                const size_t index = -1 == POS ? 0 : 99 == POS ? SIZE : POS;

                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                    const Int64 AL = oa.allocationLimit();
                    oa.setAllocationLimit(-1);
                    Obj mX(xoa);  const Obj &X = gg(&mX, SPEC);

                    ASSERTV(LINE, SIZE, X.size(), SIZE == X.size());

                    bsls::ObjectBuffer<ValueType> buffer;
                    ValueType *valptr = buffer.address();
                    TstFacility::emplace(valptr, ELEMENT, xsa);
                    bslma::DestructorGuard<ValueType> guard(valptr);

                    oa.setAllocationLimit(AL);

                    Obj mZ(xscratch);  const Obj& Z = gg(&mZ, SPEC); (void) Z;

                    // The strong exception guarantee is in effect only if
                    // inserting at the front or the back.

                    ExceptionProctor<Obj, ALLOC> proctor(
                                          0 == index || SIZE == index ? &X : 0,
                                          MoveUtil::move(mZ),
                                          LINE);

                    CIter position = -1 == POS ? X.cbegin()
                                               : 99 == POS
                                                 ? X.cend() : X.cbegin() + POS;

                    iterator result = mX.insert(position,
                                                MoveUtil::move(*valptr));

                    proctor.release();

                    ASSERTV(LINE, SIZE, X.size(), SIZE + 1 == X.size());
                    ASSERTV(LINE,  result == X.begin() + index);
                    ASSERTV(LINE, *result == VALUES[ELEMENT - 'A']);

                    TestValues exp(EXPECTED);
                    ASSERTV(LINE, 0 == verifyContainer(X, exp, SIZE + 1));
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
            }
        }
    }
}

template <class TYPE, class ALLOC>
void TestDriver3<TYPE,ALLOC>::testCase25()
{
    // ------------------------------------------------------------------------
    // TESTING 'push_front(T&&)' AND 'push_back(T&&)'
    //
    // Concerns:
    //: 1 For 'push_back', a new element is added to the back of the container
    //:   with the order of the existing elements in the container remaining
    //:   unchanged.
    //:
    //: 2 For 'push_front', a new element is added to the front of the
    //:   container with the order of the existing elements in the container
    //:   remaining unchanged.
    //:
    //: 3 The newly inserted item is move-inserted.
    //:
    //: 4 The internal memory management system is hooked up properly so that
    //:   *all* internally allocated memory draws from a user-supplied
    //:   allocator whenever one is specified.
    //:
    //: 5 'push_back' and 'push_front' provide the strong exception guarantee.
    //:
    //: 6 There is no effect on the validity of references to elements of the
    //:   container.  (TBD not yet tested)
    //
    // Plan:
    //: 1 For 'push_back', create objects of varying sizes, then append an
    //:   additional element.                                       (C-1, 3..4)
    //:
    //:   1 Verify that the element was added to the back of the container and
    //:     that the contents of the container is as expected.            (C-1)
    //:
    //:   2 Verify that the moved-into state for the new element is MOVED.(C-3)
    //:
    //:   3 Verify all allocations are from the object's allocator.       (C-4)
    //:
    //: 2 Repeat P-1 under the presence of exceptions.                    (C-5)
    //:
    //: 3 Repeat P-1..2 for 'push_front', but instead verify that the new
    //:   element is added to the front of the container (P-1.1).      (C-2..5)
    //
    // Testing:
    //   void push_front(T&& rvalue);
    //   void push_back(T&& rvalue);
    // ------------------------------------------------------------------------

    const char *name = bsls::NameOf<TYPE>();

    if (verbose) printf("TC 26: %s\n", name);

    const TestValues VALUES;

    // testing 'push_back'
    {
        static const struct {
            int         d_line;       // source line number
            const char *d_spec_p;     // specification string
            char        d_element;    // specification string
            const char *d_results_p;  // expected element values
        } DATA[] = {
            //line  spec            element          results
            //----  --------------  --------         ---------------------
            { L_,   "",                 'A',         "A"                     },
            { L_,   "A",                'A',         "AA"                    },
            { L_,   "A",                'B',         "AB"                    },
            { L_,   "B",                'A',         "BA"                    },
            { L_,   "AB",               'A',         "ABA"                   },
            { L_,   "BC",               'D',         "BCD"                   },
            { L_,   "BCA",              'Z',         "BCAZ"                  },
            { L_,   "CAB",              'C',         "CABC"                  },
            { L_,   "CDAB",             'D',         "CDABD"                 },
            { L_,   "DABC",             'Z',         "DABCZ"                 },
            { L_,   "ABCDE",            'Z',         "ABCDEZ"                },
            { L_,   "EDCBA",            'E',         "EDCBAE"                },
            { L_,   "ABCDEA",           'E',         "ABCDEAE"               },
            { L_,   "ABCDEAB",          'Z',         "ABCDEABZ"              },
            { L_,   "BACDEABC",         'D',         "BACDEABCD"             },
            { L_,   "CBADEABCD",        'Z',         "CBADEABCDZ"            },
            { L_,   "CBADEABCDAB",      'B',         "CBADEABCDABB"          },
            { L_,   "CBADEABCDABC",     'Z',         "CBADEABCDABCZ"         },
            { L_,   "CBADEABCDABCDE",   'B',         "CBADEABCDABCDEB"       },
            { L_,   "CBADEABCDABCDEA",  'E',         "CBADEABCDABCDEAE"      },
            { L_,   "CBADEABCDABCDEAB", 'Z',         "CBADEABCDABCDEABZ"     },
            { L_,   "ABCDEFGHIJKLMNOPQRSTUVWXY",
                                        'Z',    "ABCDEFGHIJKLMNOPQRSTUVWXYZ" }
        };
        const IntPtr NUM_DATA = sizeof DATA / sizeof *DATA;

        if (verbose) printf("\nTesting 'push_back' without exceptions.\n");
        {
            for (size_t ti = 0; ti < NUM_DATA; ++ti) {
                const int         LINE     = DATA[ti].d_line;
                const char *const SPEC     = DATA[ti].d_spec_p;
                const char        ELEMENT  = DATA[ti].d_element;
                const char *const EXPECTED = DATA[ti].d_results_p;
                const size_t      SIZE     = strlen(SPEC);

                for (char cfg = 'a'; cfg <= 'b'; ++cfg) {
                    const char CONFIG = cfg;

                    bslma::TestAllocator da("default",   veryVeryVeryVerbose);
                    bslma::TestAllocator oa("object",    veryVeryVeryVerbose);
                    bslma::TestAllocator za("different", veryVeryVeryVerbose);
                    ALLOC                xoa(&oa);

                    bslma::DefaultAllocatorGuard dag(&da);

                    Obj mX(xoa);  const Obj& X = gg(&mX, SPEC);

                    bslma::TestAllocator *valAllocator = 0;
                    switch (cfg) {
                      case 'a': {
                        // inserted value has same allocator
                        valAllocator = &oa;
                      } break;
                      case 'b': {
                        // inserted value has different allocator
                        valAllocator = &za;
                      } break;
                      default: {
                        ASSERTV(CONFIG, !"Bad allocator config.");
                      } return;                                       // RETURN
                    }
                    bslma::TestAllocator& sa = *valAllocator;
                    ALLOC                 xsa(&sa);
                    if (veryVerbose) { T_ P_(LINE) P_(CONFIG) P(X) }

                    // -------------------------------------------------------
                    // Verify any attribute allocators are installed properly.
                    // -------------------------------------------------------

                    ASSERTV(LINE, CONFIG, xoa == X.get_allocator());

                    ASSERTV(LINE, CONFIG, SIZE, X.size(), SIZE == X.size());

                    bsls::ObjectBuffer<ValueType> buffer;
                    ValueType *valptr = buffer.address();
                    TstFacility::emplace(valptr, ELEMENT, xsa);

                    ASSERTV(SIZE, numNotMovedInto(X),
                            SIZE == numNotMovedInto(X));

                    mX.push_back(MoveUtil::move(*valptr));

                    typename MoveState::Enum mState =
                                       TstFacility::getMovedFromState(*valptr);
                    bslma::DestructionUtil::destroy(valptr);

                    if (veryVerbose) { T_ P_(LINE) P_(CONFIG) P_(ELEMENT) P(X)}

                    if (k_IS_WELL_BEHAVED && &sa != &oa) {
                        ASSERTV(mState, CONFIG, name,
                                             MoveState::e_NOT_MOVED == mState);
                    }
                    else {
                        ASSERTV(mState, CONFIG, name,
                                               MoveState::e_UNKNOWN == mState
                                            || MoveState::e_MOVED   == mState);
                    }

                    if (0 != SIZE) {
                        ASSERTV(SIZE, numNotMovedInto(X),
                                SIZE == numNotMovedInto(X, 0, SIZE));
                    }

                    ASSERTV(LINE, CONFIG, SIZE, X.size(),
                            SIZE + 1 == X.size());

                    TestValues exp(EXPECTED);
                    ASSERTV(LINE, CONFIG,
                            0 == verifyContainer(X, exp, SIZE + 1));
                }
            }
        }

        if (verbose)
            printf("\nTesting 'push_back' with injected exceptions.\n");
        {
            for (size_t ti = 0; ti < NUM_DATA; ++ti) {
                const int         LINE     = DATA[ti].d_line;
                const char *const SPEC     = DATA[ti].d_spec_p;
                const char        ELEMENT  = DATA[ti].d_element;
                const char *const EXPECTED = DATA[ti].d_results_p;
                const size_t      SIZE     = strlen(SPEC);

                for (char cfg = 'a'; cfg <= 'b'; ++cfg) {
                    const char CONFIG = cfg;

                    bslma::TestAllocator da("default",   veryVeryVeryVerbose);
                    bslma::TestAllocator oa("object",    veryVeryVeryVerbose);
                    bslma::TestAllocator za("different", veryVeryVeryVerbose);
                    ALLOC                xoa(&oa);

                    bslma::DefaultAllocatorGuard dag(&da);

                    Obj mX(xoa);  const Obj& X = gg(&mX, SPEC);

                    bslma::TestAllocator *valAllocator = 0;
                    switch (cfg) {
                      case 'a': {
                        // inserted value has same allocator
                        valAllocator = &oa;
                      } break;
                      case 'b': {
                        // inserted value has different allocator
                        valAllocator = &za;
                      } break;
                      default: {
                        ASSERTV(CONFIG, !"Bad allocator config.");
                      } return;                                       // RETURN
                    }
                    bslma::TestAllocator& sa = *valAllocator;
                    ALLOC                 xsa(&sa);

                    ASSERTV(LINE, SIZE, X.size(), SIZE == X.size());

                    bslma::TestAllocator scratch("scratch",
                                                 veryVeryVeryVerbose);
                    ALLOC                xscratch(&scratch);

                    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                        Obj mZ(xscratch);  const Obj& Z = gg(&mZ, SPEC);
                        ASSERTV(Z, X, Z == X);

                        // 'push_back' provides the strong exception guarantee.

                        ExceptionProctor<Obj, ALLOC>
                                           proctor(&X, MoveUtil::move(mZ), L_);

                        bsls::ObjectBuffer<ValueType> buffer;
                        ValueType *valptr = buffer.address();
                        TstFacility::emplace(valptr, ELEMENT, xsa);
                        bslma::DestructorGuard<ValueType> guard(valptr);

                        mX.push_back(MoveUtil::move(*valptr));

                        ASSERTV(LINE, SIZE, X.size(), SIZE + 1 == X.size());

                        TestValues exp(EXPECTED);
                        ASSERTV(LINE, 0 == verifyContainer(X, exp, SIZE + 1));

                        proctor.release();
                    } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
                }
            }
        }
    }

    // testing 'push_front'
    {
        static const struct {
            int         d_line;       // source line number
            const char *d_spec_p;     // specification string
            char        d_element;    // specification string
            const char *d_results_p;  // expected element values
        } DATA[] = {
            //line  spec            element          results
            //----  --------------  --------         ---------------------
            { L_,   "",                 'A',         "A"                     },
            { L_,   "a",                'A',         "AA"                    },
            { L_,   "a",                'B',         "BA"                    },
            { L_,   "b",                'A',         "AB"                    },
            { L_,   "ab",               'A',         "ABA"                   },
            { L_,   "bc",               'D',         "DCB"                   },
            { L_,   "bca",              'Z',         "ZACB"                  },
            { L_,   "cab",              'C',         "CBAC"                  },
            { L_,   "cdab",             'D',         "DBADC"                 },
            { L_,   "dabc",             'Z',         "ZCBAD"                 },
            { L_,   "abcde",            'Z',         "ZEDCBA"                },
            { L_,   "edcba",            'E',         "EABCDE"                },
            { L_,   "abcdea",           'E',         "EAEDCBA"               },
            { L_,   "abcdeab",          'Z',         "ZBAEDCBA"              },
            { L_,   "bacdeabc",         'D',         "DCBAEDCAB"             },
            { L_,   "cbadeabcd",        'Z',         "ZDCBAEDABC"            },
            { L_,   "cbadeabcdab",      'B',         "BBADCBAEDABC"          },
            { L_,   "cbadeabcdabc",     'Z',         "ZCBADCBAEDABC"         },
            { L_,   "cbadeabcdabcde",   'B',         "BEDCBADCBAEDABC"       },
            { L_,   "cbadeabcdabcdea",  'E',         "EAEDCBADCBAEDABC"      },
            { L_,   "cbadeabcdabcdeab", 'Z',         "ZBAEDCBADCBAEDABC"     },
            { L_,   "yxwvutsrqponmlkjihgfedcba",
                                        'Z',    "ZABCDEFGHIJKLMNOPQRSTUVWXY" }
        };
        const size_t NUM_DATA = sizeof DATA / sizeof *DATA;

        if (verbose) printf("\nTesting 'push_front' without exceptions.\n");
        {
            for (size_t ti = 0; ti < NUM_DATA; ++ti) {
                const int         LINE     = DATA[ti].d_line;
                const char *const SPEC     = DATA[ti].d_spec_p;
                const char        ELEMENT  = DATA[ti].d_element;
                const char *const EXPECTED = DATA[ti].d_results_p;
                const size_t      SIZE     = strlen(SPEC);

                for (char cfg = 'a'; cfg <= 'b'; ++cfg) {
                    const char CONFIG = cfg;

                    bslma::TestAllocator da("default",   veryVeryVeryVerbose);
                    bslma::TestAllocator oa("object",    veryVeryVeryVerbose);
                    bslma::TestAllocator za("different", veryVeryVeryVerbose);
                    ALLOC                xoa(&oa);

                    bslma::DefaultAllocatorGuard dag(&da);

                    Obj mX(xoa);  const Obj& X = gg(&mX, SPEC);

                    bslma::TestAllocator *valAllocator = 0;
                    switch (cfg) {
                      case 'a': {
                        // inserted value has same allocator
                        valAllocator = &oa;
                      } break;
                      case 'b': {
                        // inserted value has different allocator
                        valAllocator = &za;
                      } break;
                      default: {
                        ASSERTV(CONFIG, !"Bad allocator config.");
                      } return;                                       // RETURN
                    }
                    bslma::TestAllocator& sa = *valAllocator;
                    ALLOC                 xsa(&sa);

                    if (veryVerbose) { T_ P_(LINE) P_(CONFIG) P(X) }

                    // -------------------------------------------------------
                    // Verify any attribute allocators are installed properly.
                    // -------------------------------------------------------

                    ASSERTV(LINE, CONFIG, &oa == X.get_allocator());

                    ASSERTV(LINE, CONFIG, SIZE, X.size(), SIZE == X.size());

                    bsls::ObjectBuffer<ValueType> buffer;
                    ValueType *valptr = buffer.address();
                    TstFacility::emplace(valptr, ELEMENT, xsa);

                    ASSERTV(SIZE, numNotMovedInto(X),
                            SIZE == numNotMovedInto(X));

                    mX.push_front(MoveUtil::move(*valptr));

                    typename MoveState::Enum mState =
                                       TstFacility::getMovedFromState(*valptr);
                    bslma::DestructionUtil::destroy(valptr);

                    if (veryVerbose) { T_ P_(LINE) P_(CONFIG) P_(ELEMENT) P(X)}

                    if (k_IS_WELL_BEHAVED && &sa != &oa) {
                        ASSERTV(mState, CONFIG, name,
                                             MoveState::e_NOT_MOVED == mState);
                    }
                    else {
                        ASSERTV(mState, CONFIG, name,
                                               MoveState::e_UNKNOWN == mState
                                            || MoveState::e_MOVED   == mState);
                    }

                    ASSERTV(SIZE, numNotMovedInto(X),
                                  SIZE == numNotMovedInto(X, 1, SIZE + 1));

                    ASSERTV(LINE, CONFIG,
                            SIZE, X.size(), SIZE + 1 == X.size());

                    TestValues exp(EXPECTED);
                    ASSERTV(LINE, CONFIG,
                            0 == verifyContainer(X, exp, SIZE + 1));
                }
            }
        }

        if (verbose)
            printf("\nTesting 'push_front' with injected exceptions.\n");
        {
            for (size_t ti = 0; ti < NUM_DATA; ++ti) {
                const int         LINE     = DATA[ti].d_line;
                const char *const SPEC     = DATA[ti].d_spec_p;
                const char        ELEMENT  = DATA[ti].d_element;
                const char *const EXPECTED = DATA[ti].d_results_p;
                const size_t      SIZE     = strlen(SPEC);

                for (char cfg = 'a'; cfg <= 'b'; ++cfg) {
                    const char CONFIG = cfg;

                    bslma::TestAllocator da("default",   veryVeryVeryVerbose);
                    bslma::TestAllocator oa("object",    veryVeryVeryVerbose);
                    bslma::TestAllocator za("different", veryVeryVeryVerbose);
                    ALLOC                xoa(&oa);

                    bslma::DefaultAllocatorGuard dag(&da);

                    Obj mX(xoa);  const Obj& X = gg(&mX, SPEC);

                    bslma::TestAllocator *valAllocator = 0;
                    switch (cfg) {
                      case 'a': {
                        // inserted value has same allocator
                        valAllocator = &oa;
                      } break;
                      case 'b': {
                        // inserted value has different allocator
                        valAllocator = &za;
                      } break;
                      default: {
                        ASSERTV(CONFIG, !"Bad allocator config.");
                      } return;                                       // RETURN
                    }
                    bslma::TestAllocator& sa = *valAllocator;
                    ALLOC                 xsa(&sa);

                    ASSERTV(LINE, SIZE, X.size(), SIZE == X.size());

                    bslma::TestAllocator scratch("scratch",
                                                 veryVeryVeryVerbose);
                    ALLOC                xscratch(&scratch);

                    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                        Obj mZ(xscratch);  const Obj& Z = gg(&mZ, SPEC);
                        ASSERTV(Z, X, Z == X);

                        // 'push_front' provides the strong exception
                        // guarantee.

                        ExceptionProctor<Obj, ALLOC>
                                           proctor(&X, MoveUtil::move(mZ), L_);

                        bsls::ObjectBuffer<ValueType> buffer;
                        ValueType *valptr = buffer.address();
                        TstFacility::emplace(valptr, ELEMENT, xsa);
                        bslma::DestructorGuard<ValueType> guard(valptr);

                        mX.push_front(MoveUtil::move(*valptr));

                        ASSERTV(LINE, SIZE, X.size(), SIZE + 1 == X.size());

                        TestValues exp(EXPECTED);
                        ASSERTV(LINE, 0 == verifyContainer(X, exp, SIZE + 1));

                        proctor.release();
                    } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
                }
            }
        }
    }
}

template <class TYPE, class ALLOC>
void TestDriver3<TYPE,ALLOC>::testCase24_move_assignment_noexcept()
{
    // ------------------------------------------------------------------------
    // TESTING MOVE-ASSIGNMENT OPERATOR: NOEXCEPT SPECIFICATION
    //
    // Concerns:
    //: 1 If 'allocator_traits<Allocator>::is_always_equal::value' is true, the
    //:   move assignment operator is 'noexcept(true)'.
    //
    // Plan:
    //: 1 Compare the value of the trait with the noexcept specification of the
    //:    move assignment operator.
    //
    // Testing:
    //   deque& operator=(bslmf::MovableRef<deque> rhs);
    // ------------------------------------------------------------------------

#if BSLS_KEYWORD_NOEXCEPT_AVAILABLE
    bsl::deque<TYPE, ALLOC> a, b;

    const bool isNoexcept = AllocatorTraits::is_always_equal::value;
    ASSERT(isNoexcept ==
        BSLS_KEYWORD_NOEXCEPT_OPERATOR(a = bslmf::MovableRefUtil::move(b)));
#endif
}

template <class TYPE, class ALLOC>
void TestDriver3<TYPE,ALLOC>::testCase24_dispatch()
{
    // ------------------------------------------------------------------------
    // TESTING MOVE-ASSIGNMENT OPERATOR
    //
    // Concerns:
    //  TBD: the test does not yet cover the case where allocator propagation
    //       is enabled for move assignment (hard-coded to 'false') -- i.e.,
    //       parts of C-5..6 are currently not addressed.
    //
    //: 1 The signature and return type are standard.
    //:
    //: 2 The reference returned is to the target object (i.e., '*this').
    //:
    //: 3 The move-assignment operator can change the value of a modifiable
    //:   target object to that of any source object.
    //:
    //: 4 The object has its internal memory management system hooked up
    //:   properly so that *all* internally allocated memory draws from a
    //:   user-supplied allocator whenever one is specified.
    //:
    //: 5 If allocator propagation is not enabled for move-assignment, the
    //:   allocator address held by the target object is unchanged; otherwise,
    //:   the allocator address held by the target object is changed to that of
    //:   the source.
    //:
    //: 6 If allocator propagation is enabled for move-assignment, any memory
    //:   allocation from the original target allocator is released after the
    //:   operation has completed.
    //:
    //: 7 All elements in the target object are either move-assigned to or
    //:   destroyed.
    //:
    //: 8 The source object is left in a valid but unspecified state, and the
    //:   allocator address held by the original object is unchanged.
    //:
    //: 9 Subsequent changes to or destruction of the original object have no
    //:   effect on the move-assigned object and vice-versa.
    //:
    //:10 Assigning a source object having the default-constructed value
    //:   allocates no memory; assigning a value to a target object in the
    //:   default state does not allocate or deallocate any memory.
    //:
    //:11 Every object releases any allocated memory at destruction.
    //:
    //:12 Any memory allocation is exception neutral.
    //:
    //:13 Assigning an object to itself behaves as expected (alias-safety).
    //:
    //:14 If 'allocator_traits<Allocator>::is_always_equal::value' is
    //:   true, the move assignment operator is 'noexcept(true)'.
    //
    // Plan:
    //: 1 Use the address of 'operator=' to initialize a member-function
    //:   pointer having the appropriate signature and return type for the
    //:   move-assignment operator defined in this component.             (C-1)
    //:
    //: 2 Iterate over a set of object values with substantial and varied
    //:   differences, ordered by increasing length, and create for each a
    //:   control object representing the source of the assignment, with its
    //:   own scratch allocator.
    //:
    //: 3 Iterate again over the same set of object values and create an object
    //:   representing the target of the assignment, with its own unique object
    //:   allocator.
    //:
    //: 4 In a loop consisting of two iterations, create a source object (a
    //:   copy of the control object in P-1) with 1) a different allocator than
    //:   that of target and 2) the same allocator as that of the target,
    //:
    //: 5 Call the move-assignment operator in the presence of exceptions
    //:   during memory allocations (using a 'bslma::Allocator' and varying
    //:   its allocation limit) and verify the following:                (C-12)
    //:
    //:   1 The address of the return value is the same as that of the target
    //:     object.                                                       (C-2)
    //:
    //:   2 The object being assigned to has the same value as that of the
    //:     source object before assignment (i.e., the control object).   (C-3)
    //:
    //:   3 CONTAINER-SPECIFIC NOTE: none
    //:
    //:   4 If the source and target objects use the same allocator, ensure
    //:     that there is no net increase in memory use from the common
    //:     allocator.  Also consider the following cases:
    //:
    //:     1 If the source object is empty, confirm that there are no bytes
    //:       currently in use from the common allocator.                (C-10)
    //:
    //:     2 If the target object is empty, confirm that there was no memory
    //:       change in memory usage.                                    (C-10)
    //:
    //:   5 If the source and target objects use different allocators, ensure
    //:     that each element in the source object is move-inserted into the
    //:     target object.                                                (C-7)
    //:
    //:   6 Ensure that the source, target, and control objects continue to
    //:     have the correct allocators and that all memory allocations come
    //:     from the appropriate allocator.                               (C-4)
    //:
    //:   7 Manipulate the source object (after assignment) to ensure that it
    //:     is in a valid state, destroy it, and then manipulate the target
    //:     object to ensure that it is in a valid state.                 (C-8)
    //:
    //:   8 Verify all memory is released when the source and target objects
    //:     are destroyed.                                               (C-11)
    //:
    //: 6 Use a test allocator installed as the default allocator to verify
    //:   that no memory is ever allocated from the default allocator.
    //:
    //: 7 To address concern 14, pass allocators with both 'is_always_equal'
    //:   values (true & false).
    //
    // Testing:
    //   deque& operator=(deque&& rhs);
    // ------------------------------------------------------------------------

    const char *name = bsls::NameOf<TYPE>();

    if (verbose) printf("TC 25: %s\n", name);

    // Since this function is called with a variety of template arguments, it
    // is necessary to infer some things about our template arguments in order
    // to print a meaningful banner.

    const bool isPropagate =
                AllocatorTraits::propagate_on_container_move_assignment::value;
    const bool otherTraitsSet =
                AllocatorTraits::propagate_on_container_copy_assignment::value;

    // We can print the banner now:

    if (verbose) printf("%sTESTING MOVE ASSIGN '%s' OTHER:%c PROP:%c"
                                                                " ALLOC: %s\n",
                        veryVerbose ? "\n" : "",
                        bsls::NameOf<TYPE>().name(),
                        otherTraitsSet ? 'T' : 'F',
                        isPropagate ? 'T' : 'F',
                        allocCategoryAsStr());

    // Assign the address of the function to a variable.
    {
        typedef Obj& (Obj::*OperatorMAg)(bslmf::MovableRef<Obj>);

        OperatorMAg op = &Obj::operator=;
        (void) op;  // quash potential compiler warning
    }

    // Create a test allocator and install it as the default.

    bslma::TestAllocator doa("default",   veryVeryVeryVerbose);
    bslma::TestAllocator ooa("object",    veryVeryVeryVerbose);
    bslma::TestAllocator zoa("other",     veryVeryVeryVerbose);
    bslma::TestAllocator soa("scratch",   veryVeryVeryVerbose);
    bslma::TestAllocator foa("footprint", veryVeryVeryVerbose);
    bslma::DefaultAllocatorGuard dag(&doa);

    ALLOC da(&doa);
    ALLOC oa(&ooa);
    ALLOC za(&zoa);
    ALLOC sa(&soa);

    // Check remaining properties of allocator to make sure they all match
    // 'otherTraitsSet'.

    BSLMF_ASSERT(otherTraitsSet ==
                          AllocatorTraits::propagate_on_container_swap::value);
    ASSERT((otherTraitsSet ? sa : da) ==
                   AllocatorTraits::select_on_container_copy_construction(sa));

    // Use a table of distinct object values and expected memory usage.

    enum { NUM_DATA                         = DEFAULT_NUM_DATA };
    const  DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

    // Testing function signature

    {
        typedef Obj& (Obj::*OperatorMoveAssign)(bslmf::MovableRef<Obj>);
        OperatorMoveAssign oma = &Obj::operator=;    (void) oma;
    }

    Obj szc(sa);    const Obj& SZC = szc;
    primaryManipulatorBack(&szc, 'Z');
    const TYPE& zValue = SZC.front();

    // Create first object.
    for (int ti = 0; ti < NUM_DATA; ++ti) {
        const char *const SPEC1   = DATA[ti].d_spec;
        const size_t      LENGTH1 = strlen(SPEC1);

        Obj  mZZ(sa);  const Obj&  ZZ = gg(&mZZ,  SPEC1);

        // Ensure the first row of the table contains the default-constructed
        // value.
        if (0 == ti) {
            ASSERTV(SPEC1, Obj(sa), ZZ, Obj(sa) == ZZ);
        }

        // Create second object.
        for (int tj = 0; tj < NUM_DATA; ++tj) {
            const char *const SPEC2   = DATA[tj].d_spec;
            const size_t      LENGTH2 = strlen(SPEC2);
            for (char cfg = 'a'; cfg <= 'b'; ++cfg) {
                const char CONFIG = cfg;  // how we specify the allocator

                if (veryVerbose) { T_ P_(SPEC1); P_(SPEC2); P(CONFIG); }

                Obj *objPtr = new (foa) Obj(oa);

                Obj& mX = *objPtr;  const Obj& X = gg(&mX, SPEC2);

                Obj *srcPtr = 0;
                ALLOC&                ra  = 'a' == CONFIG ? za  : oa;
                bslma::TestAllocator& roa = 'a' == CONFIG ? zoa : ooa;

                const bool empty = 0 == ZZ.size();

                typename Obj::const_pointer pointers[2];

                Int64 oaBase;
                Int64 zaBase;

                int numPasses = 0;
                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(ooa) {
                    ++numPasses;

                    Int64 al = ooa.allocationLimit();
                    ooa.setAllocationLimit(-1);

                    srcPtr = new (foa) Obj(ra);
                    bslma::RawDeleterProctor<Obj, bslma::Allocator>
                                                         proctor(srcPtr, &foa);
                    gg(srcPtr, SPEC1);

                    Obj& mZ = *srcPtr;  const Obj& Z = mZ;
                    ASSERT(ZZ == Z);

                    // TBD: add exception guard for mX here

                    ASSERTV(SPEC1, SPEC2, Z, X, (Z == X) == (ti == tj));

                    storeFirstNElemAddr(pointers, Z,
                                        sizeof pointers / sizeof *pointers);

                    if (veryVeryVerbose) { T_ T_ Q(ExceptionTestBody) }

                    oaBase = ooa.numAllocations();
                    zaBase = zoa.numAllocations();

                    ooa.setAllocationLimit(al);

                    Obj *mR = &(mX = MoveUtil::move(mZ));
                    ASSERTV(SPEC1, SPEC2, mR, &mX, mR == &mX);

                    // Verify the value of the object.
                    ASSERTV(SPEC1, SPEC2,  X,  ZZ,  X ==  ZZ);

                    proctor.release();
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                Obj& mZ = *srcPtr;  const Obj& Z = mZ;

                ASSERTV(SPEC1, SPEC2, &ra == &oa, X, LENGTH1 == X.size());

                // CONTAINER-SPECIFIC NOTE: For 'deque', if the allocators
                // differ, the source object is left with the same number of
                // elements but with each element in the "moved-from" state
                // (whatever that is); otherwise, the source object has the
                // same number of elements as the destination object had (and
                // vice versa).

                if (&roa == &ooa || isPropagate) {
                    // same allocator

                    // 1. no allocations from the (common) object allocator
                    ASSERTV(SPEC1, SPEC2, ooa.numAllocations() == oaBase);

                    // 2. unchanged address(es) of contained element(s)
                    ASSERT(0 == checkFirstNElemAddr(pointers, X,
                                          sizeof pointers / sizeof *pointers));

                    // 3. CONTAINER-SPECIFIC NOTE: 'src' & 'dst' "swap" lengths
                    ASSERTV(SPEC1, SPEC2, &ra == &oa, Z,
                                       (&ra == &oa ? LENGTH2 : 0) == Z.size());

                    // 4. nothing from the other allocator
                    ASSERTV(SPEC1, SPEC2, isPropagate ||
                                               zaBase == zoa.numAllocations());
                }
                else {
                    // 1. each element in original move-inserted
                    typename Obj::const_iterator exp =
                             k_IS_WELL_BEHAVED && e_STATEFUL != s_allocCategory
                                                         ? X.begin() : X.end();
                    typename Obj::const_iterator result =
                       TstMoveUtil::findFirstNotMovedInto(X.begin(), X.end());

                    ASSERTV(SPEC1,
                            SPEC2,
                            result - X.begin(),
                            bsls::NameOf<TYPE>(),
                            allocCategoryAsStr(),
                            result - exp,
                            exp == result);

                    // 2. CONTAINER-SPECIFIC NOTE: orig obj with same length
                    ASSERTV(SPEC1, SPEC2, &ra != &oa, Z, LENGTH1 == Z.size());

                    // 3. additional memory checks
                    ASSERTV(SPEC1, SPEC2, &ra == &oa,
                            empty || oaBase < ooa.numAllocations());
                    ASSERTV(SPEC1, SPEC2, zoa.numAllocations() == zaBase);
                }

                // Verify that 'X', 'Z', and 'ZZ' have correct allocator.
                ASSERTV(SPEC1, SPEC2, sa == ZZ.get_allocator());
                ASSERTV(SPEC1, SPEC2, (isPropagate ? ra : oa) ==
                                             X.get_allocator());
                ASSERTV(SPEC1, SPEC2, ra ==  Z.get_allocator());

                // Manipulate source object 'Z' to ensure it is in a valid
                // state and is independent of 'X'.

                const size_t zAfterSize = Z.size();

                primaryManipulatorBack(&mZ, 'Z');
                ASSERTV(SPEC1, SPEC2, Z, zAfterSize + 1 == Z.size());
                ASSERTV(SPEC1, SPEC2, zValue == Z.back());
                ASSERTV(SPEC1, SPEC2, Z, ZZ, Z != ZZ);
                ASSERTV(SPEC1, SPEC2, X, ZZ, X == ZZ);

                primaryManipulatorFront(&mZ, 'Z');
                ASSERTV(SPEC1, SPEC2, Z, zAfterSize + 2 == Z.size());
                ASSERTV(SPEC1, SPEC2, zValue == Z.front());
                ASSERTV(SPEC1, SPEC2, X, ZZ, X == ZZ);

                foa.deleteObject(srcPtr);

                ASSERTV(SPEC1, SPEC2, roa.numBlocksInUse(),
                                       empty || ((&ra == &oa || isPropagate) ==
                                                  (0 < roa.numBlocksInUse())));

                // Verify subsequent manipulation of target object 'X'.

                primaryManipulatorBack(&mX, 'Z');
                ASSERTV(SPEC1, SPEC2, LENGTH1 + 1, X.size(),
                        LENGTH1 + 1 == X.size());
                ASSERTV(SPEC1, SPEC2, zValue == X[LENGTH1]);
                ASSERTV(SPEC1, SPEC2, X, ZZ, X != ZZ);

                primaryManipulatorFront(&mX, 'Z');
                ASSERTV(SPEC1, SPEC2, LENGTH1 + 2, X.size(),
                        LENGTH1 + 2 == X.size());
                ASSERTV(SPEC1, SPEC2, zValue == X[0]);

                foa.deleteObject(objPtr);

                ASSERTV(SPEC1, SPEC2, ooa.numBlocksInUse(),
                        0 == ooa.numBlocksInUse());
                ASSERTV(SPEC1, SPEC2, zoa.numBlocksInUse(),
                        0 == zoa.numBlocksInUse());
            }
        }

        // self-assignment

        {
            Obj mZ(oa);   const Obj& Z  = gg(&mZ,  SPEC1);

            ASSERTV(SPEC1, ZZ, Z, ZZ == Z);

            bslma::TestAllocatorMonitor oam(&ooa), sam(&soa);

            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(ooa) {
                if (veryVeryVerbose) { T_ T_ Q(ExceptionTestBody) }

                Obj *mR = &(mZ = MoveUtil::move(mZ));
                ASSERTV(SPEC1, ZZ,  Z, ZZ ==  Z);
                ASSERTV(SPEC1, mR, &Z, mR == &Z);
            } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

            ASSERTV(SPEC1, oa == Z.get_allocator());

            ASSERTV(SPEC1, sam.isTotalSame());
            ASSERTV(SPEC1, oam.isTotalSame());
        }

        // Verify all object memory is released on destruction.

        ASSERTV(SPEC1, ooa.numBlocksInUse(), 0 == ooa.numBlocksInUse());
    }

    ASSERTV(e_STATEFUL == s_allocCategory || 0 == doa.numBlocksTotal());

    // Test noexcept specification of the move assignment operator.
    testCase24_move_assignment_noexcept();
}

template <class TYPE, class ALLOC>
void TestDriver3<TYPE,ALLOC>::testCase23()
{
    // ------------------------------------------------------------------------
    // TESTING MOVE CONSTRUCTOR
    //
    // Concerns:
    //: 1 The newly created object has the same value (using the equality
    //:   operator) as that of the original object before the call.
    //:
    //: 2 All internal representations of a given value can be used to create a
    //:   new object of equivalent value.
    //:
    //: 3 The allocator is propagated to the newly created object if (and only
    //:   if) no allocator is specified in the call to the move constructor.
    //:
    //: 4 A constant-time move, with no additional memory allocations or
    //:   deallocations, is performed when no allocator or the same allocator
    //:   as that of the original object is passed to the move constructor.
    //:
    //: 5 A linear operation, where each element is move-inserted into the
    //:   newly created object, is performed when a '0' or an allocator that is
    //:   different than that of the original object is explicitly passed to
    //:   the move constructor.
    //:
    //: 6 The original object is always left in a valid state; the allocator
    //:   address held by the original object is unchanged.
    //:
    //: 7 Subsequent changes to or destruction of the original object have no
    //:   effect on the move-constructed object and vice-versa.
    //:
    //: 8 The object has its internal memory management system hooked up
    //:   properly so that *all* internally allocated memory draws from a
    //:   user-supplied allocator whenever one is specified.
    //:
    //: 9 Every object releases any allocated memory at destruction.
    //
    //:10 Any memory allocation is exception neutral.
    //:
    // Plan:
    //: 1 Specify a set, 'V', of object values with substantial and varied
    //:   differences, ordered by increasing length, to be used sequentially in
    //:   the following tests; for each entry, create a control object.   (C-2)
    //:
    //: 2 Call the move constructor to create the container in all relevant use
    //:   cases involving the allocator: 1) no allocator passed in, 2) a '0' is
    //    explicitly passed in as the allocator argument, 3) the same allocator
    //:   as that of the original object is explicitly passed in, and 4) a
    //:   different allocator than that of the original object is passed in.
    //:
    //: 3 For each of the object values (P-1) and for each configuration (P-2),
    //:   verify the following:
    //:
    //:   1 Verify the newly created object has the same value as that of the
    //:     original object before the call to the move constructor (control
    //:     value).                                                       (C-1)
    //:
    //:   2 CONTAINER-SPECIFIC NOTE: none
    //:
    //:   3 Where a constant-time move is expected, ensure that no memory was
    //:     allocated, that element addresses did not change, and that the
    //:     original object is left in the default state.         (C-3..5, C-7)
    //:
    //:   4 Where a linear-time move is expected, ensure that the move
    //:     constructor was called for each element.                   (C-6..7)
    //:
    //:   5 CONTAINER-SPECIFIC NOTE: none
    //:
    //:   6 Ensure that the new, original, and control objects continue to have
    //:     the correct allocator and that all memory allocations come from the
    //:     appropriate allocator.                                    (C-3,C-9)
    //:
    //:   7 Manipulate the original object (after the move construction) to
    //:     ensure it is in a valid state, destroy it, and then manipulate the
    //:     newly created object to ensure that it is in a valid state.   (C-8)
    //:
    //:   8 Verify all memory is released when the object is destroyed.  (C-11)
    //;
    //: 4 Perform tests in the presence of exceptions during memory allocations
    //:   using a 'bslma::TestAllocator' and varying its *allocation* *limit*.
    //:                                                                  (C-10)
    //
    // Testing:
    //  deque(deque&& original);
    //  deque(deque&& original, const A& basicAllocator);
    // ------------------------------------------------------------------------

    if (verbose) printf("TC 24: %s\n", bsls::NameOf<TYPE>().name());

    const TestValues VALUES;

    static const char *SPECS[] = {
        "",
        "A",
        "BC",
        "CDE",
        "DEAB",
        "EABCD",
        "ABCDEAB",
        "ABCDEABC",
        "ABCDEABCD",
        "ABCDEABCDEABCDE",
        "ABCDEABCDEABCDEA",
        "ABCDEABCDEABCDEAB",
        "ABCDEFGHIJKLMNOPQRSTUVWXY"
    };
    enum { NUM_SPECS = sizeof SPECS / sizeof *SPECS };

    if (verbose) printf("\nTesting both versions of move constructor.\n");
    {
        for (int ti = 0; ti < NUM_SPECS; ++ti) {
            const char *const SPEC   = SPECS[ti];
            const size_t      LENGTH = strlen(SPEC);

            if (verbose) {
                printf("\nFor an object of length " ZU ":\n", LENGTH);
                P(SPEC);
            }

            // Create control object 'ZZ' with the scratch allocator.

            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);
            ALLOC                xscratch(&scratch);

            Obj mZZ(xscratch);  const Obj& ZZ = gg(&mZZ, SPEC);

            static bool firstFlag = true;
            if (firstFlag) {
                ASSERTV(ti, Obj(), ZZ, Obj() == ZZ);
                firstFlag = false;
            }

            for (char cfg = 'a'; cfg <= 'd'; ++cfg) {

                const char           CONFIG = cfg;  // how we specify
                                                    // the allocator
                bslma::TestAllocator fa("footprint", veryVeryVeryVerbose);
                bslma::TestAllocator da("default",   veryVeryVeryVerbose);
                bslma::TestAllocator sa("supplied",  veryVeryVeryVerbose);
                bslma::TestAllocator za("different", veryVeryVeryVerbose);
                ALLOC                xsa(&sa);
                ALLOC                xza(&za);

                // Create source object 'Z'.
                Obj *srcPtr = new (fa) Obj(xsa);
                Obj& mZ = *srcPtr;  const Obj& Z = gg(&mZ, SPEC);

                typename Obj::const_pointer pointers[2];
                storeFirstNElemAddr(pointers,
                                    Z,
                                    sizeof pointers / sizeof *pointers);

                bslma::DefaultAllocatorGuard dag(&da);

                bslma::TestAllocatorMonitor oam(&da), sam(&sa);

                Obj                  *objPtr;
                bslma::TestAllocator *objAllocatorPtr;
                bslma::TestAllocator *othAllocatorPtr;

                bool empty = 0 == ZZ.size();

                switch (CONFIG) {
                  case 'a': {
                    oam.reset(&sa);
                    objPtr = new (fa) Obj(MoveUtil::move(mZ));
                    objAllocatorPtr = &sa;
                    othAllocatorPtr = &da;
                  } break;
                  case 'b': {
                    oam.reset(&da);
                    objPtr = new (fa) Obj(MoveUtil::move(mZ), ALLOC(0));
                    objAllocatorPtr = &da;
                    othAllocatorPtr = &za;
                  } break;
                  case 'c': {
                    oam.reset(&sa);
                    objPtr = new (fa) Obj(MoveUtil::move(mZ), xsa);
                    objAllocatorPtr = &sa;
                    othAllocatorPtr = &da;
                  } break;
                  case 'd': {
                    oam.reset(&za);
                    objPtr = new (fa) Obj(MoveUtil::move(mZ), xza);
                    objAllocatorPtr = &za;
                    othAllocatorPtr = &da;
                  } break;
                  default: {
                    ASSERTV(CONFIG, !"Bad allocator config.");
                  } return;                                           // RETURN
                }

                bslma::TestAllocator& oa = *objAllocatorPtr;
                bslma::TestAllocator& noa = *othAllocatorPtr;
                ALLOC                 xoa(&oa);

                Obj& mX = *objPtr;  const Obj& X = mX;
                // Verify the value of the object.
                ASSERTV(SPEC, CONFIG, X == ZZ);

                // CONTAINER-SPECIFIC NOTE: For 'deque', if the allocators
                // differ, the original object is left with the same number of
                // elements but with each element in the "moved-from" state
                // (whatever that is); otherwise, the original object is left
                // empty.

                if (&sa == &oa) {
                    // 1. some memory allocation
                    ASSERTV(SPEC, CONFIG, &sa == &oa, oam.isTotalUp());

                    // 2. unchanged address(es) of contained element(s)
                    ASSERT(0 == checkFirstNElemAddr(pointers, X,
                                          sizeof pointers / sizeof *pointers));

                    // 3. original object left empty
                    ASSERTV(SPEC, CONFIG, &sa == &oa, Z, 0 == Z.size());
                }
                else {
                    // 1. each element in original move-inserted
                    typename Obj::const_iterator exp =
                                       k_IS_WELL_BEHAVED ? X.begin() : X.end();

                    ASSERTV(SPEC, CONFIG, exp ==
                       TstMoveUtil::findFirstNotMovedInto(X.begin(), X.end()));

                    // 2. original object left with same size
                    ASSERTV(SPEC, CONFIG, &sa == &oa, Z, LENGTH == Z.size());

                    // 3. additional memory checks
                    ASSERTV(SPEC, CONFIG,
                            &sa == &oa, oam.isTotalUp() || empty);
                }

                // Verify that 'X', 'Z', and 'ZZ' have the correct allocator.
                ASSERTV(SPEC, CONFIG, &scratch == ZZ.get_allocator());
                ASSERTV(SPEC, CONFIG,      &sa ==  Z.get_allocator());
                ASSERTV(SPEC, CONFIG,      &oa ==  X.get_allocator());

                // Verify no allocation from the non-object allocator and that
                // object allocator is hooked up.
                ASSERTV(SPEC, CONFIG, 0 == noa.numBlocksTotal());
                ASSERTV(SPEC, CONFIG, 0 < oa.numBlocksTotal() || empty);

                // Manipulate source object 'Z' to ensure it is in a valid
                // state and is independent of 'X'.

                primaryManipulatorBack(&mZ, 'Z');
                if (&sa == &oa) {
                    ASSERTV(SPEC, CONFIG, Z, 1 == Z.size());
                    ASSERTV(SPEC, CONFIG, VALUES['Z' - 'A'] == Z[0]);
                }
                else {
                    ASSERTV(SPEC, CONFIG, Z, LENGTH + 1 == Z.size());
                    ASSERTV(SPEC, CONFIG, VALUES['Z' - 'A'] == Z[LENGTH]);
                }
                ASSERTV(SPEC, CONFIG, Z, ZZ, Z != ZZ);
                ASSERTV(SPEC, CONFIG, X, ZZ, X == ZZ);

                primaryManipulatorFront(&mZ, 'Z');
                if (&sa == &oa) {
                    ASSERTV(SPEC, CONFIG, Z, 2 == Z.size());
                }
                else {
                    ASSERTV(SPEC, CONFIG, Z, LENGTH + 2 == Z.size());
                }
                ASSERTV(SPEC, CONFIG, VALUES['Z' - 'A'] == Z[0]);

                fa.deleteObject(srcPtr);

                ASSERTV(SPEC, CONFIG, X, ZZ, X == ZZ);

                // Verify subsequent manipulation of new object 'X'.

                primaryManipulatorBack(&mX, 'Z');
                ASSERTV(SPEC, LENGTH + 1 == X.size());
                ASSERTV(SPEC, VALUES['Z' - 'A'] == X[LENGTH]);
                ASSERTV(SPEC, X != ZZ);

                primaryManipulatorFront(&mX, 'Z');
                ASSERTV(SPEC, LENGTH + 2 == X.size());
                ASSERTV(SPEC, VALUES['Z' - 'A'] == X[0]);

                fa.deleteObject(objPtr);
                // Verify all memory is released on object destruction.
                ASSERTV(SPEC, 0 == fa.numBlocksInUse());
                ASSERTV(SPEC, 0 == da.numBlocksInUse());
                ASSERTV(SPEC, 0 == sa.numBlocksInUse());
                ASSERTV(SPEC, 0 == za.numBlocksInUse());
            }
        }
    }
    if (verbose)
        printf("\nTesting move constructor with injected exceptions.\n");
#if defined(BDE_BUILD_TARGET_EXC)
    {
        for (int ti = 0; ti < NUM_SPECS; ++ti) {
            const char *const SPEC   = SPECS[ti];
            const size_t      LENGTH = strlen(SPEC);

            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);
            ALLOC                xscratch(&scratch);

            Obj mZZ(xscratch);  const Obj& ZZ = gg(&mZZ, SPEC);

            if (verbose) {
                printf("\nFor an object of length " ZU ":\n", LENGTH);
                P(SPEC);
            }

            bslma::TestAllocator oa("object",    veryVeryVeryVerbose);
            bslma::TestAllocator za("different", veryVeryVeryVerbose);
            ALLOC                xoa(&oa);
            ALLOC                xza(&za);

            const Int64 BB = oa.numBlocksTotal();
            const Int64  B = oa.numBlocksInUse();

            if (veryVerbose) {
                printf("\t\t\t\tBefore Creation: "); P_(BB); P(B);
            }

            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                Obj mWW(xza);  const Obj& WW = gg(&mWW, SPEC);

                const Obj X(MoveUtil::move(mWW), xoa);
                if (veryVerbose) {
                    printf("\t\t\tException Case  :\n");
                    printf("\t\t\t\tObj : "); P(X);
                }
                ASSERTV(SPEC, ZZ == X);
                ASSERTV(SPEC, WW.size(), LENGTH == WW.size());
                ASSERTV(SPEC, WW.get_allocator() != X.get_allocator());
            } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

            const Int64 AA = oa.numBlocksTotal();
            const Int64  A = oa.numBlocksInUse();

            if (veryVerbose) {
                printf("\t\t\t\tAfter Creation: "); P_(AA); P(A);
            }
        }
    }
#endif  // BDE_BUILD_TARGET_EXC
}

                       // ======================
                       // struct MetaTestDriver3
                       // ======================

template <class TYPE>
struct MetaTestDriver3 {
    // This 'struct' is to be called by the 'RUN_EACH_TYPE' macro, and the
    // functions within it dispatch to functions in 'TestDriver' instantiated
    // with different types of allocator.

    typedef bsl::allocator<TYPE>            BAP;
    typedef bsltf::StdAllocatorAdaptor<BAP> SAA;

    static void testCase24();
        // Test move-assign.
};

template <class TYPE>
void MetaTestDriver3<TYPE>::testCase24()
{
    // Limit the # of tests if we're in C++11 to avoid running out of compiler
    // memory.

    // The low-order bit of the identifier specifies whether the fourth boolean
    // argument of the stateful allocator, which indicates propagate on move
    // assign, is set.

    typedef bsltf::StdStatefulAllocator<TYPE, false, false, false, false> A00;
    typedef bsltf::StdStatefulAllocator<TYPE, false, false, false, true>  A01;
#if !defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
    typedef bsltf::StdStatefulAllocator<TYPE, true,  true,  true,  false> A10;
    typedef bsltf::StdStatefulAllocator<TYPE, true,  true,  true,  true>  A11;
#endif

    if (verbose) printf("\n");

    TestDriver3<TYPE, BAP>::testCase24_dispatch();

    TestDriver3<TYPE, A00>::testCase24_dispatch();
    TestDriver3<TYPE, A01>::testCase24_dispatch();

#if !defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
    TestDriver3<TYPE, SAA>::testCase24_dispatch();

    TestDriver3<TYPE, A10>::testCase24_dispatch();
    TestDriver3<TYPE, A11>::testCase24_dispatch();
#endif

    // is_always_equal == true
    TestDriver3<TYPE, StatelessAllocator<TYPE> >::
        testCase24_move_assignment_noexcept();
}

#ifdef BSLS_COMPILERFEATURES_SUPPORT_CTAD
struct TestDeductionGuides {
    // This struct provides a namespace for functions testing deduction guides.
    // The tests are compile-time only; it is not necessary that these routines
    // be called at run-time.  Note that the following constructors do not have
    // associated deduction guides because the template parameters for
    // 'bsl::deque' cannot be deduced from the constructor parameters.
    //..
    // deque()
    // deque(size_t)
    // deque(ALLOC)
    // deque(size_t, ALLOC)
    //..

#define ASSERT_SAME_TYPE(...) \
 static_assert((bsl::is_same<__VA_ARGS__>::value), "Types differ unexpectedly")

    static void SimpleConstructors ()
        // Test that constructing a 'bsl::deque' from various combinations of
        // arguments deduces the correct type.
        //..
        // deque(const deque&  l)        -> decltype(l)
        // deque(const deque&  l, ALLOC) -> decltype(l)
        // deque(      deque&& l)        -> decltype(l)
        // deque(      deque&& l, ALLOC) -> decltype(l)
        // deque(size_type, VALUE_TYPE)        -> deque<VALUE_TYPE>
        // deque(size_type, VALUE_TYPE, ALLOC) -> deque<VALUE_TYPE, ALLOC>
        // deque(iter, iter)        -> deque<iter::VALUE_TYPE>
        // deque(iter, iter, ALLOC) -> deque<iter::VALUE_TYPE, ALLOC>
        // deque(initializer_list<T>)        -> deque<T>
        // deque(initializer_list<T>, ALLOC) -> deque<T>
        //..
    {
        bslma::Allocator     *a1 = nullptr;
        bslma::TestAllocator *a2 = nullptr;

        bsl::deque<int> d1;
        bsl::deque      d1a(d1);
        ASSERT_SAME_TYPE(decltype(d1a), bsl::deque<int>);

        typedef float T2;
        bsl::deque<T2> d2;
        bsl::deque     d2a(d2, bsl::allocator<T2>());
        bsl::deque     d2b(d2, a1);
        bsl::deque     d2c(d2, a2);
        bsl::deque     d2d(d2, bsl::allocator<int>());
        ASSERT_SAME_TYPE(decltype(d2a), bsl::deque<T2, bsl::allocator<T2>>);
        ASSERT_SAME_TYPE(decltype(d2b), bsl::deque<T2, bsl::allocator<T2>>);
        ASSERT_SAME_TYPE(decltype(d2c), bsl::deque<T2, bsl::allocator<T2>>);
        ASSERT_SAME_TYPE(decltype(d2d), bsl::deque<T2, bsl::allocator<T2>>);

        bsl::deque<short> d3;
        bsl::deque        d3a(std::move(d3));
        ASSERT_SAME_TYPE(decltype(d3a), bsl::deque<short>);

        typedef long double T4;
        bsl::deque<T4> d4;
        bsl::deque     d4a(std::move(d4), bsl::allocator<T4>());
        bsl::deque     d4b(std::move(d4), a1);
        bsl::deque     d4c(std::move(d4), a2);
        bsl::deque     d4d(std::move(d4), bsl::allocator<int>());
        ASSERT_SAME_TYPE(decltype(d4a), bsl::deque<T4, bsl::allocator<T4>>);
        ASSERT_SAME_TYPE(decltype(d4b), bsl::deque<T4, bsl::allocator<T4>>);
        ASSERT_SAME_TYPE(decltype(d4c), bsl::deque<T4, bsl::allocator<T4>>);
        ASSERT_SAME_TYPE(decltype(d4d), bsl::deque<T4, bsl::allocator<T4>>);

    // Turn off complaints about passing allocators in non-allocator positions
    // BDE_VERIFY pragma: push
    // BDE_VERIFY pragma: -AM01
        bsl::deque d5a(42, 3L);
        bsl::deque d5b(42, bsl::allocator<long>{});
        bsl::deque d5c(42, a1); // Deduce a deque of 'bslma::Allocator *'
        bsl::deque d5d(42, std::allocator<long>{});
        ASSERT_SAME_TYPE(decltype(d5a), bsl::deque<long>);
        ASSERT_SAME_TYPE(decltype(d5b), bsl::deque<bsl::allocator<long>>);
        ASSERT_SAME_TYPE(decltype(d5c), bsl::deque<bslma::Allocator *>);
        ASSERT_SAME_TYPE(decltype(d5d), bsl::deque<std::allocator<long>>);
    // BDE_VERIFY pragma: pop

        typedef double T6;
        bsl::deque d6a(42, 3.0, bsl::allocator<T6>());
        bsl::deque d6b(42, 3.0, a1);
        bsl::deque d6c(42, 3.0, a2);
        bsl::deque d6d(42, 3.0, std::allocator<T6>());
        ASSERT_SAME_TYPE(decltype(d6a), bsl::deque<T6, bsl::allocator<T6>>);
        ASSERT_SAME_TYPE(decltype(d6b), bsl::deque<T6, bsl::allocator<T6>>);
        ASSERT_SAME_TYPE(decltype(d6c), bsl::deque<T6, bsl::allocator<T6>>);
        ASSERT_SAME_TYPE(decltype(d6d), bsl::deque<T6, std::allocator<T6>>);

        typedef char T7;
        T7                       *p7b = nullptr;
        T7                       *p7e = nullptr;
        bsl::deque<T7>::iterator  i7b;
        bsl::deque<T7>::iterator  i7e;
        bsl::deque                d7a(p7b, p7e);
        bsl::deque                d7b(i7b, i7e);
        ASSERT_SAME_TYPE(decltype(d7a), bsl::deque<T7>);
        ASSERT_SAME_TYPE(decltype(d7b), bsl::deque<T7>);

        typedef unsigned short T8;
        T8                       *p8b = nullptr;
        T8                       *p8e = nullptr;
        bsl::deque<T8>::iterator  i8b;
        bsl::deque<T8>::iterator  i8e;

        bsl::deque d8a(p8b, p8e, bsl::allocator<T8>());
        bsl::deque d8b(p8b, p8e, a1);
        bsl::deque d8c(p8b, p8e, a2);
        bsl::deque d8d(p8b, p8e, std::allocator<T8>());
        bsl::deque d8e(i8b, i8e, bsl::allocator<T8>());
        bsl::deque d8f(i8b, i8e, a1);
        bsl::deque d8g(i8b, i8e, a2);
        bsl::deque d8h(i8b, i8e, std::allocator<T8>());
        ASSERT_SAME_TYPE(decltype(d8a), bsl::deque<T8, bsl::allocator<T8>>);
        ASSERT_SAME_TYPE(decltype(d8b), bsl::deque<T8, bsl::allocator<T8>>);
        ASSERT_SAME_TYPE(decltype(d8c), bsl::deque<T8, bsl::allocator<T8>>);
        ASSERT_SAME_TYPE(decltype(d8d), bsl::deque<T8, std::allocator<T8>>);
        ASSERT_SAME_TYPE(decltype(d8e), bsl::deque<T8, bsl::allocator<T8>>);
        ASSERT_SAME_TYPE(decltype(d8f), bsl::deque<T8, bsl::allocator<T8>>);
        ASSERT_SAME_TYPE(decltype(d8g), bsl::deque<T8, bsl::allocator<T8>>);
        ASSERT_SAME_TYPE(decltype(d8h), bsl::deque<T8, std::allocator<T8>>);

        bsl::deque d9 {1LL, 2LL, 3LL, 4LL, 5LL};
        ASSERT_SAME_TYPE(decltype(d9), bsl::deque<long long>);

        typedef long long T10;
        std::initializer_list<T10> il = {1LL, 2LL, 3LL, 4LL, 5LL};
        bsl::deque                 d10a(il, bsl::allocator<T10>{});
        bsl::deque                 d10b(il, a1);
        bsl::deque                 d10c(il, a2);
        bsl::deque                 d10d(il, std::allocator<T10>{});
        ASSERT_SAME_TYPE(decltype(d10a), bsl::deque<T10, bsl::allocator<T10>>);
        ASSERT_SAME_TYPE(decltype(d10b), bsl::deque<T10, bsl::allocator<T10>>);
        ASSERT_SAME_TYPE(decltype(d10c), bsl::deque<T10, bsl::allocator<T10>>);
        ASSERT_SAME_TYPE(decltype(d10d), bsl::deque<T10, std::allocator<T10>>);

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        // Compile-fail tests
// #define BSLSTL_DEQUE_COMPILE_FAIL_POINTER_IS_NOT_A_SIZE
#if defined BSLSTL_DEQUE_COMPILE_FAIL_POINTER_IS_NOT_A_SIZE
        bsl::Deque_Util<char> *lnp = nullptr; // pointer to random class type
        bsl::deque d99(lnp, 3.0, a1);
        // This should fail to compile (pointer is not a size)
#endif
    }

#undef ASSERT_SAME_TYPE
};
#endif  // BSLS_COMPILERFEATURES_SUPPORT_CTAD

// ============================================================================
//                              MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;

                verbose = argc > 2;
            veryVerbose = argc > 3;
        veryVeryVerbose = argc > 4;
    veryVeryVeryVerbose = argc > 5;

    bslma::TestAllocator defaultAllocator("default", veryVeryVeryVerbose);
    bslma::DefaultAllocatorGuard dag(&defaultAllocator);

    bslma::TestAllocator globalAllocator("global", veryVeryVeryVerbose);
    bslma::Default::setGlobalAllocator(&globalAllocator);

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:  // Zero is always the leading case.
      case 33: {
        // --------------------------------------------------------------------
        // TESTING FREE FUNCTIONS 'BSL::ERASE' AND 'BSL::ERASE_IF'
        //
        // Concerns:
        //: 1 The free functions exist, and are callable with a deque.
        //
        // Plan:
        //: 1 Fill a deque with known values, then attempt to erase some of
        //:   the values using 'bsl::erase' and 'bsl::erase_if'.  Verify that
        //:   the resultant deque is the right size, contains the correct
        //:   values, and that the value returned from the functions is
        //:   correct.
        //
        // Testing:
        //   size_t erase(deque<T,A>&, const U&);
        //   size_t erase_if(deque<T,A>&, PREDICATE);
        // --------------------------------------------------------------------

        if (verbose)
            printf(
                "\nTESTING FREE FUNCTIONS 'BSL::ERASE' AND 'BSL::ERASE_IF'"
                "\n=======================================================\n");

        TestDriver3<char>::testCase33();
        TestDriver3<int>::testCase33();
        TestDriver3<long>::testCase33();
      } break;
      case 32: {
        //---------------------------------------------------------------------
        // TESTING CLASS TEMPLATE DEDUCTION GUIDES (AT COMPILE TIME)
        //   Ensure that the deduction guides are properly specified to deduce
        //   the template arguments from the arguments supplied to the
        //   constructors.
        //
        // Concerns:
        //: 1 Construction from iterators deduces the value type from the value
        //:   type of the iterator.
        //
        //: 2 Construction with a 'bslma::Allocator *' deduces the correct
        //:   specialization of 'bsl::allocator' for the type of the allocator.
        //
        // Plan:
        //: 1 Create a list by invoking the constructor without supplying the
        //:   template arguments explicitly.
        //:
        //: 2 Verify that the deduced type is correct.
        //
        // Testing:
        //   CLASS TEMPLATE DEDUCTION GUIDES
        //---------------------------------------------------------------------
        if (verbose)
            printf(
              "\nTESTING CLASS TEMPLATE DEDUCTION GUIDES (AT COMPILE TIME)"
              "\n=========================================================\n");

#ifdef BSLS_COMPILERFEATURES_SUPPORT_CTAD
        // This is a compile-time only test case.
        TestDeductionGuides test;
        (void) test; // This variable only exists for ease of IDE navigation.
#endif
      } break;
      case 31: {
        // --------------------------------------------------------------------
        // 'noexcept' SPECIFICATION
        // --------------------------------------------------------------------
        if (verbose) printf("\n" "'noexcept' SPECIFICATION" "\n"
                                 "========================" "\n");

        TestDriver3<int>::testCase31();

      } break;
      case 30: {
        // --------------------------------------------------------------------
        // TESTING 'shrink_to_fit'
        // --------------------------------------------------------------------

        if (verbose) printf("TESTING 'shrink_to_fit'\n"
                            "=======================\n");

        RUN_EACH_TYPE(TestDriver3,
                      testCase30,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);

        RUN_EACH_TYPE(TestDriver3,
                      testCase30,
                      bsltf::MoveOnlyAllocTestType,
                      bsltf::WellBehavedMoveOnlyAllocTestType);

         RUN_EACH_TYPE(StdBslmaTestDriver3,
                      testCase30,
                      bsltf::StdAllocTestType<bsl::allocator<int> >,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_PRIMITIVE);
      } break;
      case 29: {
        // --------------------------------------------------------------------
        // TESTING INITIALIZER LIST FUNCTIONS
        // --------------------------------------------------------------------

        RUN_EACH_TYPE(TestDriver3,
                      testCase29,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);

        RUN_EACH_TYPE(StdBslmaTestDriver3,
                      testCase29,
                      bsltf::StdAllocTestType<bsl::allocator<int> >,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_PRIMITIVE);
     } break;
      case 28: {
        // --------------------------------------------------------------------
        // TESTING 'emplace'
        // --------------------------------------------------------------------

        if (verbose) printf("\nTesting Value Emplacement"
                            "\n=========================\n");

#if !defined(BSLS_PLATFORM_CMP_SUN) || !defined(BSLS_PLATFORM_CMP_GNU)
        RUN_EACH_TYPE(TestDriver3,
                      testCase28,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      bsltf::NonDefaultConstructibleTestType);

        RUN_EACH_TYPE(TestDriver3,
                      testCase28a,
                      bsltf::EmplacableTestType,
                      bsltf::AllocEmplacableTestType);
#else
        Q(Emplace Tests Prevented);
#endif
        RUN_EACH_TYPE(StdBslmaTestDriver3,
                      testCase28,
                      bsltf::StdAllocTestType<bsl::allocator<int> >,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_PRIMITIVE);

      } break;
      case 27: {
        // --------------------------------------------------------------------
        // TESTING 'emplace_front' AND 'emplace_back'
        // --------------------------------------------------------------------

        if (verbose) printf("\nTesting Value Emplacement"
                            "\n=========================\n");

#if !defined(BSLS_PLATFORM_CMP_SUN) || !defined(BSLS_PLATFORM_CMP_GNU)
        RUN_EACH_TYPE(TestDriver3,
                      testCase27,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      bsltf::NonDefaultConstructibleTestType);

        RUN_EACH_TYPE(StdBslmaTestDriver3,
                      testCase27,
                      bsltf::StdAllocTestType<bsl::allocator<int> >,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_PRIMITIVE);

        // 'emplace_back'

        RUN_EACH_TYPE(TestDriver3,
                      testCase27a,
                      bsltf::EmplacableTestType,
                      bsltf::AllocEmplacableTestType);

        // 'emplace_front'

        RUN_EACH_TYPE(TestDriver3,
                      testCase27b,
                      bsltf::EmplacableTestType,
                      bsltf::AllocEmplacableTestType);
#else
        Q(Emplace Tests Prevented);
#endif
      } break;
      case 26: {
        // --------------------------------------------------------------------
        // TESTING 'insert' ON MOVABLE VALUES
        // --------------------------------------------------------------------

        RUN_EACH_TYPE(TestDriver3,
                      testCase26,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);

        RUN_EACH_TYPE(TestDriver3,
                      testCase26,
                      bsltf::MoveOnlyAllocTestType,
                      bsltf::WellBehavedMoveOnlyAllocTestType);

        RUN_EACH_TYPE(StdBslmaTestDriver3,
                      testCase26,
                      bsltf::StdAllocTestType<bsl::allocator<int> >,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_PRIMITIVE);

      } break;
      case 25: {
        // --------------------------------------------------------------------
        // TESTING 'push_front' AND 'push_back' ON MOVABLE VALUES
        // --------------------------------------------------------------------

        RUN_EACH_TYPE(TestDriver3,
                      testCase25,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);

        RUN_EACH_TYPE(TestDriver3,
                      testCase25,
                      bsltf::MoveOnlyAllocTestType,
                      bsltf::WellBehavedMoveOnlyAllocTestType);

        RUN_EACH_TYPE(StdBslmaTestDriver3,
                      testCase25,
                      bsltf::StdAllocTestType<bsl::allocator<int> >,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_PRIMITIVE);

      } break;
      case 24: {
        // --------------------------------------------------------------------
        // TESTING MOVE-ASSIGNMENT OPERATOR
        // --------------------------------------------------------------------

        RUN_EACH_TYPE(MetaTestDriver3,
                      testCase24,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);

        RUN_EACH_TYPE(MetaTestDriver3,
                      testCase24,
                      bsltf::MoveOnlyAllocTestType,
                      bsltf::WellBehavedMoveOnlyAllocTestType);
      } break;
      case 23: {
        // --------------------------------------------------------------------
        // TESTING MOVE CONSTRUCTOR
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING MOVE CONSTRUCTOR"
                            "\n========================\n");

        RUN_EACH_TYPE(TestDriver3,
                      testCase23,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);

        RUN_EACH_TYPE(TestDriver3,
                      testCase23,
                      bsltf::MoveOnlyAllocTestType,
                      bsltf::WellBehavedMoveOnlyAllocTestType);

        RUN_EACH_TYPE(StdBslmaTestDriver3,
                      testCase23,
                      bsltf::StdAllocTestType<bsl::allocator<int> >,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_PRIMITIVE);

      } break;
      case 22: BSLA_FALLTHROUGH;
      case 21: BSLA_FALLTHROUGH;
      case 20: BSLA_FALLTHROUGH;
      case 19: BSLA_FALLTHROUGH;
      case 18: BSLA_FALLTHROUGH;
      case 17: BSLA_FALLTHROUGH;
      case 16: BSLA_FALLTHROUGH;
      case 15: BSLA_FALLTHROUGH;
      case 14: BSLA_FALLTHROUGH;
      case 13: BSLA_FALLTHROUGH;
      case 12: BSLA_FALLTHROUGH;
      case 11: BSLA_FALLTHROUGH;
      case 10: BSLA_FALLTHROUGH;
      case  9: BSLA_FALLTHROUGH;
      case  8: BSLA_FALLTHROUGH;
      case  7: BSLA_FALLTHROUGH;
      case  6: BSLA_FALLTHROUGH;
      case  5: BSLA_FALLTHROUGH;
      case  4: BSLA_FALLTHROUGH;
      case  3: BSLA_FALLTHROUGH;
      case  2: BSLA_FALLTHROUGH;
      case  1: {
        if (verbose)
            printf("Test case %d is in another test driver part.\n"
                   "See 'bslstl_deque.0.t.cpp' for the test plan.\n",
                   test);
      } break;
      default: {
        fprintf(stderr, "WARNING: CASE `%d' NOT FOUND.\n", test);
        testStatus = -1;
      }
    }

    ASSERTV(defaultAllocator.numBlocksInUse(),
            0 == defaultAllocator.numBlocksInUse());

    // CONCERN: In no case does memory come from the global allocator.
    ASSERTV(globalAllocator.numBlocksTotal(),
            0 == globalAllocator.numBlocksTotal());

    if (testStatus > 0) {
        fprintf(stderr, "Error, non-zero test status = %d.\n", testStatus);
    }

    return testStatus;
}

// ----------------------------------------------------------------------------
// Copyright 2020 Bloomberg Finance L.P.
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
